# Part II: Type System - §5.5 Never Type

**Section**: §5.5 | **Part**: Type System (Part II)
**Previous**: [Character Type](01d_Character.md) | **Next**: [Arrays and Slices](../02_ArraysAndSlices.md)

---

**Definition 5.5.1 (Never Type):** The never type `!` is the bottom type (⊥) representing computations that never produce a value. It is uninhabited (⟦!⟧ = ∅) and is a subtype of all types (`∀τ. ! <: τ`).

## 5.5 Never Type

### 5.5.1 Overview

**Key innovation/purpose:** The never type enables the compiler to verify that certain code paths diverge (never return), supporting type-safe control flow analysis and exhaustiveness checking.

**When to use never type:**
- Functions that never return (call `exit`, `panic`, infinite loops)
- Unreachable branches in control flow
- Type-level proof of impossibility
- Match arms that diverge
- Error types for infallible operations

**When NOT to use never type:**
- Functions that might not return → use `Option<T>` or `Result<T, E>`
- Empty enums → use explicit empty enum type
- Functions with side effects that return () → use unit type `()`

**Relationship to other features:**
- **Bottom type**: `! <: τ` for all types τ (universal subtyping)
- **Control flow**: Compiler knows code after `!` is unreachable
- **Pattern matching**: `!` patterns prove exhaustiveness
- **Zero-sized**: `size(!) = 0` (never instantiated)

### 5.5.2 Syntax

#### Concrete Syntax

**Type syntax:**
```ebnf
NeverType ::= "!"
```

**Usage in function signatures:**
```cantrip
function panic(message: str): !
    uses panic;
{
    std::process::abort();
}

function exit(code: i32): !
    uses process.exit;
{
    std::process::exit(code);
}

function infinite_loop(): ! {
    loop {
        // Never returns
    }
}
```

#### Abstract Syntax

**Type:**
```
τ ::= !
```

**Value:**
```
(no values: ⟦!⟧ = ∅)
```

### 5.5.3 Static Semantics

#### Well-Formedness Rules

```
[WF-Never]
──────────────────
Γ ⊢ ! : Type
```

#### Type Rules

**Never type as bottom:**

```
[T-Never]
Γ ⊢ e : !
────────────────  (for any type T)
Γ ⊢ e : T
```

The never type coerces to any type because it represents diverging computation—if the expression never produces a value, it's safe to claim it has any type.

**Typing diverging functions:**

```
[T-Diverge-Panic]
Γ ⊢ panic(...) : !

[T-Diverge-Exit]
Γ ⊢ exit(...) : !

[T-Diverge-Loop]
loop { ... } has no break
────────────────────────
Γ ⊢ loop { ... } : !
```

#### Type Properties

**Theorem 5.5.1 (Empty Value Set):**

The never type is uninhabited:

```
⟦!⟧ = ∅
```

There are no values of type `!`.

**Theorem 5.5.2 (Bottom Type):**

The never type is a subtype of all types:

```
∀ τ. ! <: τ
```

This allows expressions of type `!` to be used in contexts expecting any type.

**Theorem 5.5.3 (Size and Alignment):**

```
size(!) = 0 bytes
align(!) = 1 byte
```

The never type is zero-sized because it's never instantiated.

**Corollary:** The never type cannot be Copy (no values to copy), but this is irrelevant since no values exist.

### 5.5.4 Dynamic Semantics

#### Evaluation Rules

There are no evaluation rules for the never type itself, as it represents diverging computation:

```
(no evaluation rules: ! values never exist)
```

**Diverging expression semantics:**

```
[E-Diverge]
⟨e, σ⟩ ⇓ ⊥    (diverges: never produces a value)
```

Where ⊥ indicates divergence (non-termination, panic, or exit).

#### Memory Representation

**Never type layout:**

```
! memory layout:
(no memory allocated, zero-sized type)
Size: 0 bytes
Alignment: 1 byte
```

The never type has no runtime representation since values of this type never exist.

#### Operational Behavior

**Divergence categories:**

1. **Panic**: Abnormal termination
```cantrip
function panic(msg: str): ! {
    std::process::abort();
}
```

2. **Exit**: Normal process termination
```cantrip
function exit(code: i32): ! {
    std::process::exit(code);
}
```

3. **Infinite loop**: Non-terminating computation
```cantrip
function serve_forever(): ! {
    loop {
        handle_request();
    }
}
```

### 5.5.5 Examples and Patterns

#### Functions That Never Return

**Panic and error handling:**
```cantrip
function panic(message: str): !
    uses panic;
{
    std::eprintln("PANIC: {message}");
    std::process::abort();
}

function unreachable(): ! {
    panic("entered unreachable code");
}

function unimplemented(): ! {
    panic("not yet implemented");
}
```

**Process termination:**
```cantrip
function exit(code: i32): !
    uses process.exit;
{
    std::process::exit(code);
}

function fatal_error(error: Error): ! {
    std::eprintln("Fatal error: {error}");
    exit(1);
}
```

**Infinite loops:**
```cantrip
function event_loop(): ! {
    loop {
        let event = wait_for_event();
        process_event(event);
    }
}

function server_main(port: u16): ! {
    let listener = TcpListener::bind(port).unwrap();
    loop {
        let conn = listener.accept().unwrap();
        handle_connection(conn);
    }
}
```

#### Diverging Branches in Control Flow

**Type compatibility:**

Since `! <: τ` for all types, diverging branches are compatible with any expected return type:

```cantrip
function process(x: i32): i32 {
    if x < 0 {
        panic("negative value");  // Type: !, compiler knows this diverges
        // No need for return or unreachable code here
    } else {
        x * 2  // Type: i32
    }
    // Both branches are compatible (! coerces to i32)
}

function safe_divide(x: i32, y: i32): i32 {
    if y == 0 {
        panic("division by zero");  // Type: !
    }
    x / y  // Compiler knows y != 0 here
}
```

**In match expressions:**

```cantrip
enum NetworkResult {
    Success(i32),
    Pending,
    Error(String),
}

function extract_value(result: NetworkResult): i32 {
    match result {
        NetworkResult::Success(value) -> value,           // Type: i32
        NetworkResult::Pending -> panic("called on pending result"),  // Type: !
        NetworkResult::Error(msg) -> panic("error: {msg}"),           // Type: !
    }
    // All arms compatible (! coerces to i32)
}
```

**Type compatibility examples:**

```cantrip
// Never type coerces to any type
let x: i32 = if condition {
    42                    // Type: i32
} else {
    panic("error");      // Type: !, coerces to i32
};

// Works in any position
let y: String = match option {
    Some(s) -> s,        // Type: String
    None -> exit(1),     // Type: !, coerces to String
};

// Even in nested contexts
let z: Vec<f64> = if valid {
    vec![1.0, 2.0, 3.0]  // Type: Vec<f64>
} else {
    unimplemented();     // Type: !, coerces to Vec<f64>
};
```

#### Unreachable Code Detection

**Compiler verification:**

```cantrip
function example(x: i32): i32 {
    if x > 0 {
        return x;
    } else {
        panic("x must be positive");
    }
    // Warning: unreachable code
    // println("This is never executed");
}

function process_or_exit(data: Option<Data>): () {
    let d = match data {
        Some(d) -> d,
        None -> exit(1),  // Type: !, compiler knows we don't continue
    };
    // Compiler knows d has type Data here
    process(d);
}
```

#### Exhaustiveness Proving

**Empty enum (uninhabited type):**

```cantrip
enum Void {}  // No variants, uninhabited like !

function absurd<T>(v: Void): T {
    match v {
        // No arms needed! Type Void has no values
    }
}

// Proving unreachability
function handle_infallible(result: Result<i32, Void>): i32 {
    match result {
        Result::Ok(x) -> x,
        Result::Err(v) -> absurd(v),  // Proves this can't happen
    }
}
```

#### Type-Level Impossibility

**Infallible conversions:**

```cantrip
contract TryFrom<T> {
    type Error;
    procedure try_from(value: T): Result<Self, Self.Error>;
}

// Infallible conversion uses Never for error type
// u32 implements TryFrom<u8>:
modal u32: TryFrom<u8> {
    type Error = !;  // Can never fail

    procedure try_from(value: u8): Result<u32, !> {
        Result::Ok(value as u32)  // Always succeeds
    }
}

// Pattern matching on Result<T, !>
let result: Result<u32, !> = u32::try_from(42u8);
let value: u32 = match result {
    Result::Ok(v) -> v,
    // Result::Err case impossible (! has no values)
};

// Or simply:
let value: u32 = result.unwrap();  // Can't panic (no error possible)
```

#### Integration with Control Flow

**Early returns and exits:**

```cantrip
function validate_and_process(input: str): i32 {
    if input.is_empty() {
        eprintln("Error: empty input");
        exit(1);  // Type: !
        // Compiler knows we never continue from here
    }

    if !input.is_ascii() {
        panic("non-ASCII input");  // Type: !
    }

    // Compiler has proven input is non-empty and ASCII here
    process_valid_input(input)
}
```

**Exhaustiveness checking:**

```cantrip
enum Status {
    Active,
    Inactive,
}

function handle_status(status: Status): i32 {
    match status {
        Status::Active -> 1,
        Status::Inactive -> panic("inactive status not supported"),  // Type: !
        // Exhaustive: all variants covered, even though one panics
    }
}
```

---

**Previous**: [Character Type](01d_Character.md) | **Next**: [Arrays and Slices](../02_ArraysAndSlices.md)
