# Part IV: Contracts and Clauses - §3. Effect Clauses

**Section**: §3 | **Part**: Contracts and Clauses (Part IV)
**Previous**: [Behavioral Contracts](02_BehavioralContracts.md) | **Next**: [Precondition Clauses](04_Preconditions.md)

---

**Definition 3.1 (Effect Clause):** An effect clause (`uses`) specifies the computational side effects that a procedure may perform. Effects include I/O operations, memory allocation, network access, and other observable interactions with the environment outside of pure computation.

## 3. Effect Clauses

### 3.1 Overview

**Key innovation/purpose:** The effect system makes side effects explicit in procedure signatures, enabling:
- **Static tracking**: Know what effects a procedure performs without reading its body
- **Effect polymorphism**: Generic procedures can be bounded by effect requirements
- **Compositional reasoning**: Effects compose transitively through calls
- **Optimization**: Pure procedures (no effects) can be optimized aggressively

**When to use `uses`:**
- Procedure performs I/O (file, network, terminal)
- Procedure allocates heap memory
- Procedure accesses global state
- Procedure spawns threads or tasks
- Procedure performs FFI calls
- Any observable interaction beyond pure computation

**When NOT to use `uses`:**
- Pure computation (arithmetic, logic, local variables)
- Stack allocation (automatic in Cantrip)
- Immutable data structure operations
- Const expression evaluation

**Relationship to other features:**
- **Procedures**: Effects are part of procedure type signatures
- **Map Types (§10)**: Map types can include effect bounds
- **Traits**: Trait procedures can declare effect requirements
- **Generics**: Generic procedures can be bounded by effects
- **Contracts**: Effects compose with preconditions/postconditions

### 3.2 Syntax

#### 3.2.1 Concrete Syntax

**Effect declaration:**
```ebnf
UsesClause ::= "uses" EffectExpr ";"

EffectExpr  ::= EffectPath ("," EffectPath)*
              | "pure"

EffectPath  ::= Ident ("." Ident)*
```

**Examples:**
```cantrip
// Single effect
procedure print(s: String)
    uses io.write;
{
    println!("{}", s);
}

// Multiple effects
procedure load_config(path: String): Config
    uses io.read, alloc.heap;
{
    let contents = std::fs::read(path)?;
    parse_json(contents)
}

// Explicit purity (optional)
procedure add(x: i32, y: i32): i32
    uses pure;
{
    x + y
}

// No uses clause = pure by default
procedure multiply(x: i32, y: i32): i32 {
    x * y
}
```

#### 3.2.2 Abstract Syntax

**Effect representation:**
```
ε ::= ∅                           (no effects / pure)
    | {e₁, e₂, ..., eₙ}          (effect set)
    | ε₁ ∪ ε₂                     (effect union)

e ::= io.read | io.write
    | net.tcp | net.udp
    | alloc.heap
    | thread.spawn
    | ffi.call
    | ...
```

**Procedure signature with effects:**
```
procedure f(τ₁, ..., τₙ): τ uses ε

Desugars to map type:
map(τ₁, ..., τₙ) -> τ ! ε
```

**Effect lattice:**
```
               ⊤ (all effects)
              / | \
          io  net  alloc  ...
         / \   |
    read write |
              ...
               |
               ∅ (pure)
```

#### 3.2.3 Basic Examples

**Example 1: I/O effects**
```cantrip
procedure read_file(path: String): Result<String, Error>
    uses io.read, alloc.heap;
{
    std::fs::read_to_string(path)
}

procedure write_file(path: String, content: String): Result<(), Error>
    uses io.write;
{
    std::fs::write(path, content)
}
```

**Example 2: Network effects**
```cantrip
procedure fetch_url(url: String): Result<Response, Error>
    uses net.tcp, alloc.heap;
{
    http::get(url)
}
```

**Example 3: Pure procedure (no effects)**
```cantrip
procedure fibonacci(n: i32): i32 {
    if n <= 1 {
        n
    } else {
        fibonacci(n - 1) + fibonacci(n - 2)
    }
}
```

### 3.3 Static Semantics

#### 3.3.1 Well-Formedness Rules

**[WF-Effect] Well-formed effect:**
```
[WF-Effect]
e ∈ EffectDomain
─────────────────
e well-formed
```

**Explanation:** Effects must come from the defined effect domain.

**[WF-EffectSet] Well-formed effect set:**
```
[WF-EffectSet]
e₁ well-formed    ...    eₙ well-formed
─────────────────────────────────────────
{e₁, ..., eₙ} well-formed
```

**Explanation:** An effect set is well-formed if all its elements are well-formed effects.

**[WF-UsesClause] Well-formed uses clause:**
```
[WF-UsesClause]
ε well-formed
procedure f(...): τ { body } well-formed
─────────────────────────────────────────
procedure f(...): τ uses ε { body } well-formed
```

**Explanation:** A uses clause is well-formed if its effect set is well-formed.

#### 3.3.2 Type Rules

**[T-PureProc] Pure procedure:**
```
[T-PureProc]
Γ ⊢ procedure f(params): τ { body }
body performs no effects
─────────────────────────────────────
Γ ⊢ f : map(params) -> τ ! ∅
```

**Explanation:** Procedures without uses clauses and no effectful operations are pure.

**[T-EffectProc] Effectful procedure:**
```
[T-EffectProc]
Γ ⊢ procedure f(params): τ uses ε { body }
Γ ⊢ body : τ
effects(body) ⊆ ε
─────────────────────────────────────
Γ ⊢ f : map(params) -> τ ! ε
```

**Explanation:** A procedure's declared effects must include all effects performed in its body.

**[T-EffectCall] Procedure call with effects:**
```
[T-EffectCall]
Γ ⊢ f : map(τ₁, ..., τₙ) -> τ ! ε_f
Γ ⊢ args : τ₁, ..., τₙ
─────────────────────────────────────
Γ ⊢ f(args) : τ
effects(f(args)) = ε_f
```

**Explanation:** Calling a procedure incurs its declared effects.

**[T-EffectCompose] Effect composition:**
```
[T-EffectCompose]
Γ ⊢ e₁ : τ₁    effects(e₁) = ε₁
Γ ⊢ e₂ : τ₂    effects(e₂) = ε₂
─────────────────────────────────────
Γ ⊢ {e₁; e₂} : τ₂
effects({e₁; e₂}) = ε₁ ∪ ε₂
```

**Explanation:** Sequential composition unions effects.

**[T-EffectSubsume] Effect subsumption:**
```
[T-EffectSubsume]
Γ ⊢ procedure f(...): τ uses ε_declared { body }
effects(body) = ε_actual
ε_actual ⊆ ε_declared
─────────────────────────────────────
procedure f well-typed
```

**Explanation:** Actual effects must be a subset of declared effects.

**[T-EffectViolation] Effect violation:**
```
[T-EffectViolation]
Γ ⊢ procedure f(...): τ uses ε_declared { body }
effects(body) = ε_actual
ε_actual ⊈ ε_declared
─────────────────────────────────────
ERROR: Undeclared effect
```

**Explanation:** Using effects not in the uses clause is an error.

#### 3.3.3 Type Properties

**Theorem 2.1 (Effect Soundness):**

If a procedure declares effects ε, then it performs at most effects in ε:

```
Γ ⊢ f : map(τ) -> U ! ε
f executes with trace σ
effects_observed(σ) ⊆ ε
```

**Proof sketch:** By effect type checking. The compiler verifies that all effectful operations in the body are covered by the declared effects.

**Theorem 2.2 (Effect Composition):**

Effects compose transitively through procedure calls:

```
f uses ε_f
g uses ε_g, calls f
────────────────────
g's actual effects ⊇ ε_f ∪ ε_g
```

**Proof sketch:** By [T-EffectCompose]. Each call adds its effects to the total.

**Theorem 2.3 (Pure Procedure Optimization):**

Procedures with no effects can be:
- Reordered (no observable side effects)
- Memoized (pure functions)
- Eliminated if result unused (dead code elimination)

**Property 2.1 (Effect Monotonicity):**

Adding effects to a procedure makes it less restrictive (subsumption):

```
f : map(τ) -> U ! ε₁
ε₁ ⊆ ε₂
────────────────────
f can be used where map(τ) -> U ! ε₂ is expected
```

**Property 2.2 (Effect Lattice):**

Effects form a lattice:
- Bottom: ∅ (pure)
- Join: ε₁ ∪ ε₂ (union)
- Top: all effects
- Ordering: ε₁ ⊆ ε₂

### 3.4 Dynamic Semantics

#### 3.4.1 Evaluation Rules

**[E-PureEval] Pure procedure evaluation:**
```
[E-PureEval]
procedure f(params): τ { body }
no effects declared
⟨body[params ↦ args], σ⟩ ⇓ ⟨v, σ⟩
─────────────────────────────────────
⟨f(args), σ⟩ ⇓ ⟨v, σ⟩
```

**Explanation:** Pure procedures don't modify external state (σ unchanged).

**[E-EffectEval] Effectful procedure evaluation:**
```
[E-EffectEval]
procedure f(params): τ uses ε { body }
⟨body[params ↦ args], σ, π⟩ ⇓ ⟨v, σ', π'⟩
effects_performed(π') ⊆ ε
─────────────────────────────────────
⟨f(args), σ, π⟩ ⇓ ⟨v, σ', π'⟩
```

**Explanation:** Effectful procedures can modify state and perform effects in ε.

Where:
- σ = memory state
- π = effect trace (sequence of effects performed)

**[E-EffectIO] I/O effect:**
```
[E-EffectIO]
procedure write_file uses io.write
⟨write_file(path, data), σ, π⟩
performs file write operation
─────────────────────────────────────
⟨write_file(path, data), σ, π · io.write⟩ ⇓ ⟨(), σ, π · io.write⟩
```

**Explanation:** I/O effects are recorded in the effect trace.

#### 3.4.2 Effect Checking

**Static checking (compile-time):**

The compiler tracks effects through control flow:

```cantrip
procedure example(flag: bool) uses io.write {
    if flag {
        println!("true");    // io.write
    } else {
        println!("false");   // io.write
    }
    // Both branches have io.write, so uses io.write
}
```

**Conservative analysis:**

The compiler is conservative - if an effect might occur, it must be declared:

```cantrip
procedure conditional_io(flag: bool)
    uses io.write;  // Required even though might not execute
{
    if flag {
        println!("Hello");  // io.write
    }
    // No else branch, but io.write is possible
}
```

**Effect inference:**

The compiler can infer effects for procedure bodies:

```cantrip
procedure helper(x: i32): i32 {
    x + 1  // Compiler infers: pure (no effects)
}

procedure caller() uses io.write {
    let y = helper(5);  // OK: pure ⊆ {io.write}
    println!("{}", y);  // io.write
}
```

#### 3.4.3 Runtime Behavior

Effects have no runtime representation - they are purely compile-time information:

```cantrip
// Source
procedure log(msg: String) uses io.write {
    println!("{}", msg);
}

// Compiled (no effect metadata)
void log(String msg) {
    println(msg);  // Just the operation
}
```

**No runtime cost:** Effect checking is entirely static. There's no runtime effect tracking or checking overhead.

### 3.5 Standard Effects

Cantrip provides a standard hierarchy of effects:

#### 3.5.1 I/O Effects

```
io.read      - Read from files, stdin
io.write     - Write to files, stdout, stderr
io.seek      - File seeking operations
io.metadata  - File metadata queries
```

#### 3.5.2 Network Effects

```
net.tcp      - TCP socket operations
net.udp      - UDP socket operations
net.dns      - DNS lookups
net.http     - HTTP client operations
```

#### 3.5.3 Memory Effects

```
alloc.heap   - Heap allocation (Vec, Box, String, etc.)
alloc.global - Global allocator access
```

#### 3.5.4 Concurrency Effects

```
thread.spawn - Spawn new threads
thread.join  - Join threads
sync.lock    - Acquire locks
sync.atomic  - Atomic operations
```

#### 3.5.5 System Effects

```
sys.env      - Environment variable access
sys.time     - System time queries
sys.random   - Random number generation
sys.exit     - Process termination
```

#### 3.5.6 FFI Effects

```
ffi.call     - Foreign function calls
unsafe.ptr   - Unsafe pointer operations
```

### 3.6 Examples and Patterns

#### 3.6.1 Effect Polymorphism

```cantrip
procedure map<T, U, E>(
    items: Vec<T>,
    f: map(T) -> U ! E
): Vec<U> ! E
    uses E, alloc.heap;
{
    var result = Vec::with_capacity(items.len());
    for item in items {
        result.push(f(item));  // f's effects propagate
    }
    result
}

// Usage with pure function
let numbers = vec![1, 2, 3];
let doubled = map(numbers, |x| x * 2);  // E = ∅ (pure)

// Usage with effectful function
let messages = vec!["a", "b", "c"];
let logged = map(messages, |s| {
    println!("{}", s);  // E = {io.write}
    s
});
```

#### 3.6.2 Effect Bounds on Traits

```cantrip
contract Processor {
    procedure process($, data: Data): Result
        uses alloc.heap;  // All implementations can use heap
}

record SimpleProcessor: Processor {
    // fields...

    procedure process($, data: Data): Result
        uses alloc.heap;  // Exactly matches contract
    {
        // Process with allocation
        ...
    }
}

record PureProcessor: Processor {
    // fields...

    procedure process($, data: Data): Result
        // No uses clause: ∅ ⊆ {alloc.heap}
    {
        // Process without allocation
        ...
    }
}
```

#### 3.6.3 Conditional Effects

```cantrip
procedure save_if_changed<T>(
    cache: mut Cache<T>,
    key: String,
    value: T
) uses alloc.heap, io.write;
{
    if cache.get(&key) != Some(&value) {
        cache.insert(key.clone(), value);  // alloc.heap
        persist_cache(&cache);             // io.write
    }
    // Effects declared even though conditionally executed
}
```

#### 3.6.4 Effect Isolation

```cantrip
procedure run_with_io<T>(f: map() -> T ! {io.read, io.write}): T
    uses io.read, io.write;
{
    f()  // f's effects contained within this procedure
}

procedure main() uses io.read, io.write {
    let config = run_with_io(|| {
        load_config("config.toml")  // io.read
    });

    run_with_io(|| {
        save_results(config)  // io.write
    });
}
```

#### 3.6.5 Pure Procedures

```cantrip
// Explicitly pure
procedure factorial(n: i32): i32 uses pure {
    if n <= 1 {
        1
    } else {
        n * factorial(n - 1)
    }
}

// Implicitly pure (no uses clause)
procedure sum(numbers: &[i32]): i32 {
    var total = 0;
    for num in numbers {
        total += num;
    }
    total
}
```

### 3.7 Advanced Topics

#### 3.7.1 Effect Refinement

Procedures can be more specific than their contract bounds:

```cantrip
contract Logger {
    procedure log($, msg: String)
        uses io.write, alloc.heap;  // Upper bound
}

record BufferedLogger: Logger {
    buffer: Vec<String>;

    procedure log($, msg: String)
        uses alloc.heap;  // More specific (no io.write yet)
    {
        $.buffer.push(msg);  // Just buffering
    }
}
```

#### 3.7.2 Effect Annotations for Optimization

Pure procedures enable aggressive optimization:

```cantrip
procedure expensive_computation(x: i32): i32 {
    // Pure - can be memoized
    let mut result = 1;
    for _ in 0..x {
        result = result * 2 + 1;
    }
    result
}

// Compiler can:
// - Memoize results
// - Reorder calls
// - Eliminate if unused
```

#### 3.7.3 Effect Tracking Through Closures

```cantrip
procedure with_effects() uses io.write, alloc.heap {
    let logger = |msg: String| {
        println!("{}", msg);  // io.write
    };

    let data = vec![1, 2, 3];  // alloc.heap

    logger("Processing");
    for x in data {
        logger(format!("{}", x));  // alloc.heap + io.write
    }
}
```

---

**Previous**: [Behavioral Contracts](02_BehavioralContracts.md) | **Next**: [Precondition Clauses](04_Preconditions.md)
