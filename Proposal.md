## Clause 15: Asynchronous Operations

This clause defines Cursive's unified model for asynchronous computation. Asynchronous operations are computations that may suspend execution and resume later, producing values incrementally, completing after external events, or interleaving execution with other computations.

---

### 15.1 Foundational Concepts

##### Definition

An **asynchronous operation** is a computation with addressable resumption points. Unlike a standard procedure that runs to completion in a single invocation, an asynchronous operation may pause and resume multiple times, exchanging values with its caller at each resumption point.

**Formal Definition**

An asynchronous operation $A$ is characterized by the tuple:

$$A = (\mathcal{S}, s_0, \delta, \omega)$$

Where:
- $\mathcal{S}$ is a finite set of internal states (captured local bindings)
- $s_0 \in \mathcal{S}$ is the initial state
- $\delta : \mathcal{S} \times In \to \mathcal{S} \times (Out \mid Result \mid E)$ is the transition function
- $\omega \subseteq \mathcal{S}$ is the set of terminal states

Each invocation of `resume` applies $\delta$, transitioning the operation to a new state and producing either an intermediate output, a final result, or an error.

##### Static Semantics

**Design Principles**

The asynchronous model adheres to the following principles:

**Unified Abstraction**: All asynchronous patterns—sequences, futures, coroutines, streams—are instances of a single parameterized modal type. There are no separate mechanisms for different asynchronous patterns.

**Explicit Suspension**: Every point at which execution may pause is syntactically visible via the `yield` expression. There is no implicit suspension.

**No Function Coloring**: Procedures are not marked with special modifiers in their declaration. The return type indicates asynchronous behavior. A procedure returning `Async<...>` may be called from any context; the caller decides how to consume the result.

**Pull-Based Execution**: Asynchronous operations advance only when explicitly resumed by their consumer. This provides natural backpressure and resource control.

**Capability Integration**: External events (I/O, timers) require capabilities. Pure computation sequences require no capabilities.

> **Note:** The `yield` keyword MUST be added to the reserved keywords table in §2.6.

---

### 15.2 The Async Modal Type

##### Definition

`Async<Out, In, Result, E>` is a modal type representing an asynchronous computation. The type parameters specify the data flow:

| Parameter | Meaning                                          | Default  |
| :-------- | :----------------------------------------------- | :------- |
| `Out`     | Type of values produced at each suspension point | Required |
| `In`      | Type of values received when resumed             | `()`     |
| `Result`  | Type of the final completion value               | `()`     |
| `E`       | Type of the error on failure                     | `!`      |

When `E` is the never type (`!`), the async operation cannot fail.

##### Syntax & Declaration

**Grammar**

```ebnf
async_type ::= "Async" "<" type_arg ("," type_arg)* ">"
```

**Modal Declaration**

```cursive
modal Async<Out, In = (), Result = (), E = !> {
    @Suspended {
        output: Out,
    } {
        /// Advance the computation by providing input.
        /// Returns the new state after one step.
        transition resume(~!, input: In) -> @Suspended | @Completed | @Failed
    }
    
    @Completed {
        value: Result,
    }
    
    @Failed {
        error: E,
    }
}
```

**Default Parameters**

When fewer than four type arguments are provided:
- `Async<T>` ≡ `Async<T, (), (), !>`
- `Async<T, U>` ≡ `Async<T, U, (), !>`
- `Async<T, U, R>` ≡ `Async<T, U, R, !>`

##### Static Semantics

**State Semantics**

| State        | Meaning                                          | Payload         |
| :----------- | :----------------------------------------------- | :-------------- |
| `@Suspended` | Computation paused, intermediate value available | `output: Out`   |
| `@Completed` | Computation finished successfully                | `value: Result` |
| `@Failed`    | Computation terminated with error                | `error: E`      |

**State Transition Diagram**

```
              resume(input)           resume(input)
    ┌────────────────────┐   ┌────────────────────┐
    │                    ▼   │                    ▼
@Suspended ──────────► @Suspended ──────────► @Completed
    │                                               
    │           resume(input)                       
    └──────────────────────────────────────────► @Failed
```

**Initial State**

When a procedure returning `Async<Out, In, Result, E>` is called:

1. The procedure body executes until the first `yield` expression or completion.
2. If `yield v` is reached, the result is `Async@Suspended { output: v }`.
3. If the procedure completes without `yield`, the result is `Async@Completed { value: r }` where `r` is the result value.
4. If an error propagates before any `yield`, the result is `Async@Failed { error: e }`.

**Well-Formedness**

$$\frac{
    \Gamma \vdash Out\ \text{wf} \quad
    \Gamma \vdash In\ \text{wf} \quad
    \Gamma \vdash Result\ \text{wf} \quad
    \Gamma \vdash E\ \text{wf}
}{
    \Gamma \vdash \texttt{Async}\langle Out, In, Result, E \rangle\ \text{wf}
} \quad \text{(Async-WF)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                         | Detection    | Effect    |
| :------------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-ASYNC-0001` | Error    | `Async` type parameter is not well-formed         | Compile-time | Rejection |
| `E-ASYNC-0002` | Error    | Error type `E` does not satisfy error constraints | Compile-time | Rejection |

---

### 15.3 Common Type Aliases

##### Definition

The following type aliases provide convenient names for common asynchronous patterns. These are type aliases, not distinct types; all share the same underlying machinery.

##### Syntax & Declaration

```cursive
/// Produces multiple values, no input needed, trivial completion
type Sequence<T> = Async<T, (), (), !>

/// No intermediate values, completes with result or error
type Future<T, E = !> = Async<(), (), T, E>

/// Receives input, produces transformed output
type Pipe<In, Out> = Async<Out, In, (), !>

/// Symmetric bidirectional communication
type Exchange<T> = Async<T, T, T, !>

/// Stream with possible failure
type Stream<T, E> = Async<T, (), (), E>
```

##### Static Semantics

**Equivalence**

Type aliases are structurally equivalent to their expanded forms:

$$\texttt{Sequence}\langle T \rangle \equiv \texttt{Async}\langle T, (), (), ! \rangle$$

$$\texttt{Future}\langle T, E \rangle \equiv \texttt{Async}\langle (), (), T, E \rangle$$

---

### 15.4 The `yield` Expression

##### Definition

The `yield` expression suspends an asynchronous computation, producing an intermediate value and awaiting resumption with an input value.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_expr ::= "yield" expression
```

##### Static Semantics

**Typing Rule**

Within a procedure returning `Async<Out, In, Result, E>`:

$$\frac{
    \text{ReturnType}(\text{enclosing}) = \texttt{Async}\langle Out, In, Result, E \rangle \quad
    \Gamma \vdash e : Out
}{
    \Gamma \vdash \texttt{yield}\ e : In
} \quad \text{(T-Yield)}$$

The operand expression must have type `Out`. The `yield` expression evaluates to type `In`—the value provided when the computation is resumed.

**Context Requirement**

The `yield` expression is valid only within a procedure whose return type is `Async<Out, In, Result, E>` for some `Out`, `In`, `Result`, `E`.

##### Dynamic Semantics

**Evaluation**

Evaluation of `yield e`:

1. Let $v$ be the result of evaluating $e$.
2. Transition the `Async` to `@Suspended { output: v }`.
3. Return control to the caller.
4. When `resume(input)` is called, bind $input$ as the result of the `yield` expression.
5. Continue execution from the point immediately after `yield`.

**Example**

```cursive
procedure echo_twice() -> Async<i32, i32, i32> {
    var received = yield 0           // Output 0, receive first input
    received = yield received * 2    // Output doubled, receive second input
    result received                  // Complete with final input
}
```

##### Constraints & Legality

**Negative Constraints**

1. `yield` MUST NOT appear outside an async-returning procedure.
2. `yield` MUST NOT appear inside a `block` expression.
3. `yield` MUST NOT appear while any keys are logically held (§13).

**Diagnostic Table**

| Code           | Severity | Condition                                 | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0010` | Error    | `yield` outside async-returning proc      | Compile-time | Rejection |
| `E-ASYNC-0011` | Error    | `yield` operand type does not match `Out` | Compile-time | Rejection |
| `E-ASYNC-0012` | Error    | `yield` inside `block` expression         | Compile-time | Rejection |
| `E-ASYNC-0013` | Error    | `yield` while key is held                 | Compile-time | Rejection |

---

### 15.5 The `yield from` Expression

##### Definition

The `yield from` expression delegates to another asynchronous computation, forwarding all its outputs and inputs until completion, then evaluating to the delegated computation's result.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_from_expr ::= "yield" "from" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \text{ReturnType}(\text{enclosing}) = \texttt{Async}\langle Out, In, \_, E_1 \rangle \quad
    \Gamma \vdash e : \texttt{Async}\langle Out, In, R, E_2 \rangle \quad
    E_2 <: E_1
}{
    \Gamma \vdash \texttt{yield from}\ e : R
} \quad \text{(T-Yield-From)}$$

The source expression must be an `Async` with:
- Identical `Out` and `In` parameters to the enclosing procedure
- Error type `E_2` that is a subtype of the enclosing error type `E_1`

The expression evaluates to the source's `Result` type.

##### Dynamic Semantics

**Evaluation**

Evaluation of `yield from source`:

1. Let $s$ be the result of evaluating $source$.
2. Loop:
   - Match $s$:
     - `@Suspended { output }`: Execute `yield output` in the enclosing computation. When resumed with $input$, let $s$ := $s$.`resume`$(input)$.
     - `@Completed { value }`: The `yield from` expression evaluates to $value$. Exit loop.
     - `@Failed { error }`: Propagate $error$ to the enclosing computation. Exit loop.

**Example**

```cursive
procedure concat<T>(a: Sequence<T>, b: Sequence<T>) -> Sequence<T> {
    yield from a
    yield from b
}

procedure flatten<T>(sources: [Sequence<T>]) -> Sequence<T> {
    loop source in sources {
        yield from source
    }
}
```

##### Constraints & Legality

**Negative Constraints**

1. `yield from` MUST NOT appear outside an async-returning procedure.
2. `yield from` MUST NOT appear inside a `block` expression.
3. `yield from` MUST NOT appear while any keys are logically held.
4. The delegated `Async` MUST have compatible `Out` and `In` parameters.

**Diagnostic Table**

| Code           | Severity | Condition                                    | Detection    | Effect    |
| :------------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0020` | Error    | `yield from` outside async-returning proc    | Compile-time | Rejection |
| `E-ASYNC-0021` | Error    | Incompatible `Out` parameter in `yield from` | Compile-time | Rejection |
| `E-ASYNC-0022` | Error    | Incompatible `In` parameter in `yield from`  | Compile-time | Rejection |
| `E-ASYNC-0023` | Error    | `yield from` inside `block` expression       | Compile-time | Rejection |
| `E-ASYNC-0024` | Error    | `yield from` while key is held               | Compile-time | Rejection |
| `E-ASYNC-0025` | Error    | Error type not compatible in `yield from`    | Compile-time | Rejection |

---

### 15.6 Async-Returning Procedures

##### Definition

A procedure returns an `Async` value by declaring the appropriate return type. The compiler transforms the procedure body into a state machine.

##### Syntax & Declaration

**Grammar**

```ebnf
async_procedure ::= procedure_decl
                    (* where return type is Async<...> *)
```

No special syntax modifier is required. The return type determines async behavior.

##### Static Semantics

**Procedure Transformation**

When a procedure's return type is `Async<Out, In, Result, E>`:

1. The compiler transforms the procedure body into a state machine.
2. Local bindings become fields of the state object.
3. Each `yield` expression becomes a suspension point.
4. The `result` expression produces the final `@Completed` value.
5. Returning without `result` in a procedure with `Result = ()` implicitly completes with `()`.
6. Error propagation via `?` transitions to `@Failed`.

**Example**

```cursive
procedure range(start: i32, end: i32) -> Sequence<i32> {
    var i = start
    loop i < end {
        yield i
        i += 1
    }
}
```

##### Dynamic Semantics

**Normal Completion**

A procedure completes by executing `result value` or by reaching the end of its body (for `Result = ()`):

```cursive
procedure countdown(n: i32) -> Async<i32, (), i32> {
    var i = n
    loop i > 0 {
        yield i
        i -= 1
    }
    result 0
}
```

**Failure**

An `Async` fails when an error propagates via `?` or explicit error return:

```cursive
procedure read_lines(path: string, fs: witness FileSystem) -> Stream<string, IoError> {
    let file = fs.open(path)?
    loop {
        let line = yield from fs.read_line(file)?
        if line.is_empty() {
            return
        }
        yield line
    }
}
```

##### Constraints & Legality

**Negative Constraints**

1. A procedure returning `Async<Out, In, Result, E>` where `E = !` MUST NOT contain error-propagating expressions (`?`) for fallible operations.

**Diagnostic Table**

| Code           | Severity | Condition                                       | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0030` | Error    | Error propagation in infallible async procedure | Compile-time | Rejection |

---

### 15.7 Consuming Async Values

#### 15.7.1 Iteration

##### Definition

The `loop...in` construct iterates over an `Async<T, (), R, E>`, consuming each yielded value until completion or failure.

##### Syntax & Declaration

**Grammar**

```ebnf
async_loop ::= "loop" pattern "in" expression block
```

**Desugaring**

```cursive
loop item in source {
    body
}

// Desugars to:
{
    var __s = source
    loop {
        match __s {
            @Suspended { output: item } => {
                body
                __s = __s.resume(())
            }
            @Completed { .. } => break,
            @Failed { error } => panic(error),
        }
    }
}
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle T, (), R, E \rangle \quad
    \Gamma, x : T \vdash \text{body} : ()
}{
    \Gamma \vdash \texttt{loop}\ x\ \texttt{in}\ e\ \{ \text{body} \} : ()
} \quad \text{(T-Async-Loop)}$$

**Constraint**: Iteration requires `In = ()`. For async values requiring non-unit input, use manual stepping (§15.7.2).

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                           | Detection    | Effect    |
| :------------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-ASYNC-0040` | Error    | Iteration over async with `In ≠ ()` | Compile-time | Rejection |

---

#### 15.7.2 Manual Stepping

##### Definition

Direct interaction with the `Async` state machine via pattern matching and explicit `resume` calls.

##### Dynamic Semantics

**Example**

```cursive
var acc = accumulator(0)

match acc {
    @Suspended { output } => {
        assert(output == 0)
        acc = acc.resume(5)
    }
    _ => panic("expected suspended"),
}

match acc {
    @Suspended { output } => {
        assert(output == 5)
        acc = acc.resume(3)
    }
    _ => panic("expected suspended"),
}

match acc {
    @Completed { value } => assert(value == 8),
    _ => panic("expected completed"),
}
```

---

#### 15.7.3 The `block` Expression

##### Definition

The `block` expression runs a `Future<T, E>` (equivalently `Async<(), (), T, E>`) to completion synchronously, blocking the current execution context until the result is available.

##### Syntax & Declaration

**Grammar**

```ebnf
block_expr ::= "block" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle (), (), T, E \rangle
}{
    \Gamma \vdash \texttt{block}\ e : T \mid E
} \quad \text{(T-Block)}$$

The operand must be an `Async` with `Out = ()` and `In = ()`.

**Context Restriction**

The `block` expression is permitted only in non-async contexts (procedures not returning `Async`).

##### Dynamic Semantics

**Evaluation**

Evaluation of `block e`:

1. Let $a$ be the result of evaluating $e$.
2. Loop:
   - Match $a$:
     - `@Suspended { output: () }`: Let $a$ := $a$.`resume`$(())$.
     - `@Completed { value }`: Return $value$.
     - `@Failed { error }`: Propagate $error$.

**Example**

```cursive
procedure main(ctx: Context) -> i32 {
    let data: Data = block fetch_data(url, ctx.net)?
    process(data)
    result 0
}
```

##### Constraints & Legality

**Negative Constraints**

1. `block` MUST NOT appear inside an async-returning procedure.
2. The operand MUST have `Out = ()` and `In = ()`.

**Diagnostic Table**

| Code           | Severity | Condition                           | Detection    | Effect    |
| :------------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-ASYNC-0050` | Error    | `block` inside async-returning proc | Compile-time | Rejection |
| `E-ASYNC-0051` | Error    | `block` operand has `Out ≠ ()`      | Compile-time | Rejection |
| `E-ASYNC-0052` | Error    | `block` operand has `In ≠ ()`       | Compile-time | Rejection |

---

### 15.8 Concurrent Composition

#### 15.8.1 The `race` Expression

##### Definition

The `race` expression initiates multiple asynchronous operations concurrently and returns when the first completes.

##### Syntax & Declaration

**Grammar**

```ebnf
race_expr ::= "race" "{" race_arm ("," race_arm)* [","] "}"
race_arm  ::= expression "->" "|" pattern "|" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \forall i \in 1..n.\ \Gamma \vdash e_i : \texttt{Async}\langle (), (), T_i, E_i \rangle \quad
    \forall i \in 1..n.\ \Gamma, x_i : T_i \vdash r_i : R \quad
    n \geq 2
}{
    \Gamma \vdash \texttt{race}\ \{ e_1 \to |x_1|\ r_1, \ldots, e_n \to |x_n|\ r_n \} : R \mid (E_1 | \ldots | E_n)
} \quad \text{(T-Race)}$$

All handler arms must produce the same result type. The error type is the union of all constituent error types.

##### Dynamic Semantics

**Evaluation**

1. Initiate all async expressions concurrently.
2. When any operation reaches `@Completed` or `@Failed`:
   - Execute the corresponding handler arm (binding the result or error to the pattern).
   - Cancel all other operations (drop them, invoking cleanup).
   - Return the handler's result.
3. If an operation fails, the failure propagates unless explicitly handled in the arm.

**Example**

```cursive
let result = race {
    ctx.net.fetch(primary_url)   -> |response| Response::Primary(response),
    ctx.net.fetch(fallback_url)  -> |response| Response::Fallback(response),
    ctx.time.after(5.seconds)    -> |_| Response::Timeout,
}
```

##### Constraints & Legality

**Negative Constraints**

1. `race` MUST have at least 2 arms.
2. All arms MUST have compatible result types.
3. All operand expressions MUST have `Out = ()` and `In = ()`.

**Diagnostic Table**

| Code           | Severity | Condition                           | Detection    | Effect    |
| :------------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-ASYNC-0060` | Error    | `race` with fewer than 2 arms       | Compile-time | Rejection |
| `E-ASYNC-0061` | Error    | `race` arms have incompatible types | Compile-time | Rejection |
| `E-ASYNC-0062` | Error    | `race` operand has `Out ≠ ()`       | Compile-time | Rejection |

---

#### 15.8.2 The `all` Expression

##### Definition

The `all` expression initiates multiple asynchronous operations concurrently and returns when all complete successfully, or when any fails.

##### Syntax & Declaration

**Grammar**

```ebnf
all_expr ::= "all" "{" expression ("," expression)* [","] "}"
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \forall i \in 1..n.\ \Gamma \vdash e_i : \texttt{Async}\langle (), (), T_i, E_i \rangle
}{
    \Gamma \vdash \texttt{all}\ \{ e_1, \ldots, e_n \} : (T_1, \ldots, T_n) \mid (E_1 | \ldots | E_n)
} \quad \text{(T-All)}$$

The result is a tuple of all completion values. The error type is the union of all constituent error types.

##### Dynamic Semantics

**Evaluation**

1. Initiate all async expressions concurrently.
2. Wait for all operations to complete.
3. If all succeed: return a tuple of results in declaration order.
4. If any fails: cancel remaining operations, propagate the first error.

**Example**

```cursive
let (users, posts, config) = all {
    ctx.net.fetch("/users"),
    ctx.net.fetch("/posts"),
    ctx.fs.read("/config.json"),
}?
```

##### Constraints & Legality

**Negative Constraints**

1. All operand expressions MUST have `Out = ()` and `In = ()`.

**Diagnostic Table**

| Code           | Severity | Condition                    | Detection    | Effect    |
| :------------- | :------- | :--------------------------- | :----------- | :-------- |
| `E-ASYNC-0070` | Error    | `all` operand has `Out ≠ ()` | Compile-time | Rejection |
| `E-ASYNC-0071` | Error    | `all` operand has `In ≠ ()`  | Compile-time | Rejection |

---

### 15.9 Transformations and Combinators

##### Definition

The `Async` type provides methods for transforming and composing asynchronous computations.

##### Static Semantics

**Core Transformations**

**`map`**: Transform output values.

```cursive
procedure map<Out, In, Result, E, U>(
    self: Async<Out, In, Result, E>,
    f: procedure(Out) -> U,
) -> Async<U, In, Result, E>
```

**`filter`**: Conditionally yield values.

```cursive
procedure filter<T, E>(
    self: Async<T, (), (), E>,
    predicate: procedure(const T) -> bool,
) -> Async<T, (), (), E>
```

**`take`**: Limit number of outputs.

```cursive
procedure take<T, E>(
    self: Async<T, (), (), E>,
    count: usize,
) -> Async<T, (), (), E>
```

**`fold`**: Reduce to single value.

```cursive
procedure fold<T, A, E>(
    self: Async<T, (), (), E>,
    initial: A,
    combine: procedure(A, T) -> A,
) -> A | E
```

**`chain`**: Sequence dependent async operations.

```cursive
procedure chain<T, U, E>(
    self: Async<(), (), T, E>,
    next: procedure(T) -> Async<(), (), U, E>,
) -> Async<(), (), U, E>
```

##### Dynamic Semantics

**Chaining Example**

```cursive
ctx.net.fetch("/user/1")
    ~>chain(|user| ctx.net.fetch(user.profile_url))
    ~>chain(|profile| parse_profile(profile))
```

---

### 15.10 Condition Waiting

##### Definition

**Condition waiting** suspends an async operation until a predicate on shared data becomes true. This enables efficient event-driven patterns without polling.

##### Syntax & Declaration

**The `until` Method**

Every `shared` type provides a compiler-generated `until` method:

```cursive
procedure until<T, R>(
    self: shared T,
    predicate: procedure(const T) -> bool,
    action: procedure(unique T) -> R,
) -> Future<R>
```

##### Dynamic Semantics

**Evaluation**

1. If `predicate(self)` is true immediately: acquire key, execute `action(self)`, complete with result.
2. Otherwise: register for notification, suspend.
3. When any mutation occurs to `self`: re-evaluate predicate.
4. When predicate becomes true: acquire key, execute action, complete.

The runtime integrates condition waiting with the key system—waiters do not hold keys while suspended.

**Example: Queue Consumer**

```cursive
procedure consume<T>(queue: shared Queue<T>) -> Sequence<T> {
    loop {
        let item = yield from queue.until(
            |q| !q.is_empty(),
            |q| q.pop(),
        )
        yield item
    }
}
```

**Example: Semaphore**

```cursive
procedure acquire(sem: shared Semaphore) -> Future<()> {
    yield from sem.until(
        |s| s.count > 0,
        |s| { s.count -= 1 },
    )
}

procedure release(sem: shared Semaphore) {
    #sem { sem.count += 1 }
}
```

---

### 15.11 Integration with Capabilities

##### Definition

Async operations that interact with external resources require explicit capabilities. Pure computation sequences require no capabilities.

##### Static Semantics

**Capability Requirements**

```cursive
// I/O requires FileSystem capability
procedure read_file(path: string, fs: witness FileSystem) -> Future<Data, IoError> {
    let file = fs.open(path)?
    let data = yield from fs.read(file)
    result data
}

// Timing requires Time capability  
procedure delay(duration: Duration, time: witness Time) -> Future<()> {
    yield from time.after(duration)
}

// Pure computation requires no capability
procedure fibonacci() -> Sequence<u64> {
    var (a, b) = (0u64, 1u64)
    loop {
        yield a
        (a, b) = (b, a + b)
    }
}
```

**Capability Propagation**

Capabilities captured by an async procedure remain valid across suspensions. The capability witness is stored in the async state object.

##### Dynamic Semantics

**I/O Capability Methods**

Capability forms provide methods returning `Async`:

```cursive
form FileSystem {
    procedure open(~, path: string) -> FileHandle | IoError
    procedure read(~, file: FileHandle) -> Future<Data, IoError>
    procedure write(~, file: FileHandle, data: const [u8]) -> Future<usize, IoError>
    procedure close(~, file: FileHandle) -> Future<(), IoError>
}

form Network {
    procedure connect(~, addr: NetAddr) -> Future<Connection, NetError>
    procedure send(~, conn: Connection, data: const [u8]) -> Future<usize, NetError>
    procedure receive(~, conn: Connection) -> Future<[u8], NetError>
    procedure fetch(~, url: string) -> Future<Response, NetError>
}

form Time {
    procedure now(~) -> Timestamp
    procedure after(~, duration: Duration) -> Future<()>
}
```

---

### 15.12 Integration with the Permission System

##### Definition

An async procedure captures bindings from its environment. Capture and key rules ensure safe concurrent access across suspension points.

##### Static Semantics

**Capture Rules**

| Permission | Capturable | Behavior                                |
| :--------- | :--------- | :-------------------------------------- |
| `const T`  | Yes        | Referenced data must outlive the async  |
| `unique T` | Yes        | Exclusive access for async's lifetime   |
| `shared T` | Yes        | Subject to key rules at each suspension |
| `move T`   | Yes        | Ownership transfers to async's state    |

**Lifetime Constraint**

For any binding $b$ with permission $p$ captured by an async procedure:

$$\text{lifetime}(b) \geq \text{lifetime}(\texttt{Async})$$

The captured data must remain valid for the entire lifetime of the `Async` value.

**Key Prohibition at Suspension**

**(A-No-Keys)**: A `yield` or `yield from` expression MUST NOT occur while any keys are logically held.

$$\frac{
    \Gamma_{\text{keys}} \neq \emptyset \quad
    \text{IsYield}(e) \lor \text{IsYieldFrom}(e)
}{
    \text{Emit}(\texttt{E-ASYNC-0013})
} \quad \text{(A-No-Keys)}$$

> **Rationale:** Keys represent synchronization locks. Holding a key across suspension could cause deadlock or priority inversion.

**Valid Pattern**

```cursive
procedure read_player_stats(player: shared Player) -> Sequence<i32> {
    loop {
        let health = #player { player.health }    // Key acquired and released
        yield health                               // No key held
        yield from ctx.time.after(1.second)
    }
}
```

**Invalid Pattern**

```cursive
procedure bad(player: shared Player) -> Sequence<i32> {
    #player {
        yield player.health    // ERROR E-ASYNC-0013: key held at yield
    }
}
```

**Permission Requirements for `resume`**

The `resume` transition requires `unique` permission:

```cursive
transition resume(~!, input: In) -> @Suspended | @Completed | @Failed
```

This ensures only one execution context can advance the async operation at a time.

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                               | Detection    | Effect    |
| :------------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0013` | Error    | `yield` or `yield from` while key held  | Compile-time | Rejection |
| `E-ASYNC-0080` | Error    | Captured binding does not outlive async | Compile-time | Rejection |

---

### 15.13 Integration with Parallel Blocks

##### Definition

Async operations compose with parallel blocks (§14). Workers may suspend on async operations, yielding to other work items while waiting.

##### Static Semantics

**Async in Parallel Contexts**

`spawn` blocks may contain async operations:

```cursive
parallel ctx.cpu {
    spawn {
        let data = yield from ctx.net.fetch(url)
        process(data)
    }
    spawn {
        let config = yield from ctx.fs.read("/config")
        apply(config)
    }
}
```

**Async Dispatch**

`dispatch` can iterate over async sequences:

```cursive
parallel ctx.cpu {
    dispatch item in async_producer() {
        process(item)
    }
}
```

Each yielded item is processed as a separate work item.

**Parallel Reduction of Async Sequences**

```cursive
let sum = parallel ctx.cpu {
    dispatch value in range(0, 1000000) [reduce: +] {
        expensive_compute(value)
    }
}
```

##### Constraints & Legality

Per §13, keys MUST NOT be held across suspension points within parallel blocks.

---

### 15.14 Error Handling

##### Definition

Errors propagate through async boundaries via the `?` operator or explicit matching.

##### Static Semantics

**Error Propagation**

The `?` operator propagates errors through async boundaries:

```cursive
procedure fetch_all(
    urls: [string], 
    net: witness Network,
) -> Future<[Response], NetError> {
    var responses: [Response] = []
    loop url in urls {
        let response = yield from net.fetch(url)?
        responses.push(response)
    }
    result responses
}
```

**Error Recovery**

Match on `@Failed` for explicit error handling:

```cursive
var op = fallible_operation()
loop {
    match op {
        @Suspended { output } => {
            use(output)
            op = op.resume(())
        }
        @Completed { value } => {
            finish(value)
            break
        }
        @Failed { error } => {
            handle_error(error)
            break
        }
    }
}
```

##### Dynamic Semantics

**Cleanup on Failure**

Captured resources are cleaned up via `Drop` when an async fails or is abandoned:

```cursive
procedure with_temp_file(fs: witness FileSystem) -> Stream<string, IoError> {
    let temp = fs.create_temp()?
    defer { fs.delete(temp) }    // Runs on completion, failure, or drop
    
    loop line in lines {
        fs.write(temp, line)?
        yield line
    }
    
    result fs.finalize(temp)?
}
```

---

### 15.15 Cancellation

##### Definition

Cancellation terminates an async operation before completion, ensuring proper cleanup of resources.

##### Dynamic Semantics

**Drop-Based Cancellation**

When an `Async@Suspended` value is dropped, the operation is cancelled:

```cursive
{
    let op = ctx.net.fetch(url)
    // op dropped here without being consumed
}    // Fetch cancelled
```

**Explicit Cancellation**

For controlled cancellation, use `race` with a cancellation signal:

```cursive
procedure cancellable_fetch(
    url: string,
    cancel: shared CancelFlag,
    net: witness Network,
) -> Future<Response | Cancelled, NetError> {
    race {
        net.fetch(url) -> |response| result response,
        cancel.until(|c| c.is_set, |_| ()) -> |_| result Cancelled,
    }
}
```

**Cleanup Guarantees**

When cancelled:

1. Execution does not continue past the current suspension point.
2. `defer` blocks are executed in reverse declaration order.
3. `Drop` implementations are called for all captured resources.
4. In-flight I/O is aborted (best-effort, implementation-defined).

---

### 15.16 Memory Model

##### Definition

The compiler transforms an async procedure's local state into a state object that persists across suspensions.

##### Memory & Layout

**State Representation**

```cursive
// Source
procedure range(start: i32, end: i32) -> Sequence<i32> {
    var i = start
    loop i < end {
        yield i
        i += 1
    }
}

// Conceptual transformation (not user-visible)
record __RangeState {
    i: i32,
    end: i32,
    __resumption_point: u8,
}
```

**Size Formula**

$$\text{sizeof}(\texttt{Async}\langle Out, In, Result, E \rangle) = \text{sizeof}(\text{State}) + \max(\text{sizeof}(Out), \text{sizeof}(Result), \text{sizeof}(E)) + \text{sizeof}(\text{Discriminant})$$

**ABI Guarantees**

| Property              | Classification         |
| :-------------------- | :--------------------- |
| State object layout   | Implementation-Defined |
| Discriminant encoding | Implementation-Defined |
| Alignment             | Implementation-Defined |

##### Dynamic Semantics

**Region Allocation**

Async state can be allocated in regions:

```cursive
region task_arena {
    let ops = [
        ^task_arena range(0, 100),
        ^task_arena range(100, 200),
    ]
    
    loop op in ops {
        loop value in op {
            process(value)
        }
    }
}    // All async states freed here
```

**Heap Allocation**

When async values escape their defining scope:

```cursive
procedure create_counter(
    start: i32, 
    heap: witness HeapAllocator,
) -> Ptr<Sequence<i32>>@Valid {
    heap.alloc(range(start, i32::MAX))
}
```

---

### 15.17 Well-Formedness Rules

##### Definition

This section consolidates the well-formedness rules for async constructs.

##### Static Semantics

**Rule (A-YIELD-CONTEXT)**

`yield` and `yield from` are valid only within a procedure returning `Async<Out, In, Result, E>`.

$$\frac{
    \text{ReturnType}(\text{enclosing}) \neq \texttt{Async}\langle \_, \_, \_, \_ \rangle \quad
    \text{IsYield}(e) \lor \text{IsYieldFrom}(e)
}{
    \text{Emit}(\texttt{E-ASYNC-0010}) \lor \text{Emit}(\texttt{E-ASYNC-0020})
}$$

**Rule (A-YIELD-TYPE)**

The yield operand must match the `Out` parameter.

$$\frac{
    \text{ReturnType}(\text{proc}) = \texttt{Async}\langle Out, In, Result, E \rangle \quad
    \Gamma \vdash e : T \quad
    T \neq Out
}{
    \text{Emit}(\texttt{E-ASYNC-0011})
}$$

**Rule (A-YIELD-FROM-COMPAT)**

The delegated async must have compatible parameters.

$$\frac{
    \text{ReturnType}(\text{proc}) = \texttt{Async}\langle Out_1, In_1, \_, E_1 \rangle \quad
    \Gamma \vdash e : \texttt{Async}\langle Out_2, In_2, \_, E_2 \rangle \quad
    (Out_1 \neq Out_2 \lor In_1 \neq In_2)
}{
    \text{Emit}(\texttt{E-ASYNC-0021}) \lor \text{Emit}(\texttt{E-ASYNC-0022})
}$$

**Rule (A-NO-BLOCK-YIELD)**

`yield` must not occur inside `block` expressions.

$$\frac{
    \text{InsideBlock}(e) \quad
    \text{IsYield}(e) \lor \text{IsYieldFrom}(e)
}{
    \text{Emit}(\texttt{E-ASYNC-0012}) \lor \text{Emit}(\texttt{E-ASYNC-0023})
}$$

---

### 15.18 Diagnostic Summary

| Code           | Severity | Condition                                      |
| :------------- | :------- | :--------------------------------------------- |
| `E-ASYNC-0001` | Error    | `Async` type parameter is not well-formed      |
| `E-ASYNC-0002` | Error    | Error type does not satisfy error constraints  |
| `E-ASYNC-0010` | Error    | `yield` outside async-returning procedure      |
| `E-ASYNC-0011` | Error    | `yield` operand type mismatch                  |
| `E-ASYNC-0012` | Error    | `yield` inside `block` expression              |
| `E-ASYNC-0013` | Error    | `yield` while key is held                      |
| `E-ASYNC-0020` | Error    | `yield from` outside async-returning procedure |
| `E-ASYNC-0021` | Error    | Incompatible `Out` parameter in `yield from`   |
| `E-ASYNC-0022` | Error    | Incompatible `In` parameter in `yield from`    |
| `E-ASYNC-0023` | Error    | `yield from` inside `block` expression         |
| `E-ASYNC-0024` | Error    | `yield from` while key is held                 |
| `E-ASYNC-0025` | Error    | Error type not compatible in `yield from`      |
| `E-ASYNC-0030` | Error    | Error propagation in infallible async          |
| `E-ASYNC-0040` | Error    | Iteration over async with `In ≠ ()`            |
| `E-ASYNC-0050` | Error    | `block` inside async-returning procedure       |
| `E-ASYNC-0051` | Error    | `block` operand has `Out ≠ ()`                 |
| `E-ASYNC-0052` | Error    | `block` operand has `In ≠ ()`                  |
| `E-ASYNC-0060` | Error    | `race` with fewer than 2 arms                  |
| `E-ASYNC-0061` | Error    | `race` arms have incompatible result types     |
| `E-ASYNC-0062` | Error    | `race` operand has `Out ≠ ()`                  |
| `E-ASYNC-0070` | Error    | `all` operand has `Out ≠ ()`                   |
| `E-ASYNC-0071` | Error    | `all` operand has `In ≠ ()`                    |
| `E-ASYNC-0080` | Error    | Captured binding does not outlive async        |
| `W-ASYNC-0001` | Warning  | Large captured state (performance advisory)    |

---

### 15.19 Examples

> **Note:** The following examples are informative and illustrate correct usage patterns.

#### 15.19.1 File Processing Pipeline

```cursive
procedure process_log_file(
    path: string,
    fs: witness FileSystem,
) -> Stream<LogEntry, IoError> {
    let file = fs.open(path)?
    defer { fs.close(file) }
    
    loop {
        let line = yield from fs.read_line(file)?
        if line.is_empty() {
            break
        }
        
        match parse_log_entry(line) {
            entry: LogEntry => yield entry,
            _: ParseError => continue,
        }
    }
}

procedure main(ctx: Context) -> i32 {
    let processor = process_log_file("/var/log/app.log", ctx.fs)
    
    loop entry in processor {
        if entry.level == Level::Error {
            ctx.fs.write_stderr(format_entry(entry))
        }
    }
    
    result 0
}
```

#### 15.19.2 Concurrent Web Fetcher

```cursive
procedure scrape_urls(
    urls: [string],
    ctx: Context,
) -> Future<[Page], ScrapeError> {
    var results: [Page] = []
    
    loop url in urls {
        let page = race {
            ctx.net.fetch(url)~>chain(|r| parse_page(r)) 
                -> |page| Some(page),
            ctx.time.after(10.seconds) 
                -> |_| None,
        }
        
        match page {
            Some(p) => results.push(p),
            None => results.push(Page::timeout(url)),
        }
    }
    
    result results
}
```

#### 15.19.3 Producer-Consumer with Backpressure

```cursive
procedure producer(buffer: shared Buffer<Item>) -> Sequence<()> {
    var id = 0
    loop {
        yield from buffer.until(
            |b| !b.is_full(),
            |b| b.push(Item::new(id)),
        )
        id += 1
        yield ()
    }
}

procedure consumer(buffer: shared Buffer<Item>) -> Sequence<Item> {
    loop {
        let item = yield from buffer.until(
            |b| !b.is_empty(),
            |b| b.pop(),
        )
        yield item
    }
}

procedure main(ctx: Context) -> i32 {
    let buffer: shared Buffer<Item> = Buffer::new(100)
    
    parallel ctx.cpu {
        spawn { loop _ in producer(buffer)~>take(1000) {} }
        spawn {
            loop item in consumer(buffer)~>take(1000) {
                process(item)
            }
        }
    }
    
    result 0
}
```

#### 15.19.4 Interactive Coroutine

```cursive
procedure calculator() -> Async<string, string, i32> {
    var accumulator = 0
    
    loop {
        let input = yield f"Current: {accumulator}. Enter operation:"
        
        match parse_operation(input) {
            Op::Add(n) => accumulator += n,
            Op::Sub(n) => accumulator -= n,
            Op::Mul(n) => accumulator *= n,
            Op::Div(n) => {
                if n == 0 {
                    yield "Error: division by zero"
                    continue
                }
                accumulator /= n
            }
            Op::Quit => result accumulator,
            Op::Invalid => yield "Invalid operation",
        }
    }
}

procedure main(ctx: Context) -> i32 {
    var calc = calculator()
    
    loop {
        match calc {
            @Suspended { output: prompt } => {
                ctx.fs.write_stdout(prompt)
                let input = block ctx.fs.read_line()
                calc = calc.resume(input)
            }
            @Completed { value: final_value } => {
                ctx.fs.write_stdout(f"Final result: {final_value}")
                result 0
            }
            @Failed { error } => {
                ctx.fs.write_stderr(f"Error: {error}")
                result 1
            }
        }
    }
}
```

---

### 15.20 Summary

Cursive provides a unified asynchronous computation model based on a single parameterized modal type:

| Aspect              | Specification                                                |
| :------------------ | :----------------------------------------------------------- |
| **Core Type**       | `Async<Out, In = (), Result = (), E = !>`                    |
| **States**          | `@Suspended`, `@Completed`, `@Failed`                        |
| **Suspension**      | `yield value` — produce value, pause                         |
| **Delegation**      | `yield from source` — delegate to another async              |
| **Iteration**       | `loop x in async { }` — consume outputs (requires `In = ()`) |
| **Blocking**        | `block expr` — run to completion synchronously               |
| **Concurrency**     | `race { }`, `all { }` — compose concurrent operations        |
| **Conditions**      | `shared.until(pred, action)` — wait for predicate            |
| **Key Integration** | Keys prohibited at suspension points                         |
| **Cancellation**    | Drop-based; `defer` and `Drop` ensure cleanup                |

All traditional asynchronous patterns—generators, futures, streams, coroutines—are instances of this single mechanism with different type parameters.

> **Cross-Reference:** §13.8 MUST be updated to reference `yield` expressions rather than the deprecated `wait` construct. The diagnostic `E-KEY-0050` should be removed in favor of `E-ASYNC-0013`.

---
