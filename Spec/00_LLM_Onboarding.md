# Cantrip Language Specification - LLM Quick Reference

**Version:** 2.0.0
**Date:** 2025-01-28
**Purpose:** Accurate, comprehensive reference for LLMs working with Cantrip
**Target Audience:** LLMs, AI assistants, automated tools
**Specification Version:** 1.3.0

---

## CRITICAL: This is NOT Rust

Cantrip is a distinct systems programming language with fundamentally different semantics from Rust. While some syntax may appear similar, the underlying behavior is different:

- **NO borrow checker** - Cantrip uses region-based memory management
- **Multiple mutable references allowed** - Different from Rust's exclusive &mut
- **Uniform parameter passing** - All types pass by permission, regardless of Copy trait
- **Contracts vs Traits** - Clear distinction between abstract interfaces and concrete code reuse
- **Modal types** - First-class state machines (not in Rust)
- **Newline-terminated statements** - Semicolons are optional

**Do NOT apply Rust semantics, patterns, or assumptions to Cantrip code.**

---

## Table of Contents

1. [Executive Overview](#1-executive-overview)
2. [Type System](#2-type-system)
3. [Contract System](#3-contract-system)
4. [Expression System](#4-expression-system)
5. [Memory Model and Permissions](#5-memory-model-and-permissions)
6. [Syntax Quick Reference](#6-syntax-quick-reference)
7. [Formal Notation](#7-formal-notation)
8. [Quick Lookup Tables](#8-quick-lookup-tables)

---

## 1. Executive Overview

### 1.1 Design Philosophy

Cantrip is designed for **memory safety**, **deterministic performance**, and **AI-assisted development** through:

1. **Explicit over implicit** - All effects, lifetimes, and permissions are visible in code
2. **Local reasoning** - Understanding code requires minimal global context
3. **Zero abstraction cost** - Safety guarantees without runtime overhead
4. **LLM-friendly** - Predictable patterns specifically designed for AI code generation
5. **Simple ownership** - No borrow checker complexity
6. **No macros** - Metaprogramming only through compile-time execution for predictability

### 1.2 Safety Model

**What Cantrip PREVENTS (Compile-Time Safe):**

- ✅ Use-after-free (region-based lifetimes)
- ✅ Double-free (single ownership tracking)
- ✅ Memory leaks (RAII-style cleanup, region deallocation)
- ✅ Null pointer dereferences (except in `unsafe` blocks)
- ✅ Type safety violations (sound type system)

**What Cantrip Does NOT Prevent (Programmer Responsibility):**

- ⚠️ Aliasing bugs - Multiple mutable references are allowed
- ⚠️ Data races - Concurrent mutation not prevented by type system
- ⚠️ Iterator invalidation - No borrow checker to catch this

**This is a deliberate trade-off**: Simpler type system, easier to understand, more LLM-friendly, at the cost of some safety guarantees that Rust provides.

### 1.3 Key Innovations

1. **Lexical Permission System (LPS)** - Compile-time memory safety without borrow checking
2. **Contract System** - Preconditions, postconditions, and effect tracking as executable specifications
3. **Modal System** - State machines as first-class types with compile-time verification
4. **Memory Regions** - Explicit lifetime control with O(1) bulk deallocation
5. **Comptime** - Compile-time code generation without macros
6. **File-Based Modules** - Code organization through file system structure

### 1.4 Comparison to Rust

| Feature               | Cantrip                       | Rust                         |
| --------------------- | ----------------------------- | ---------------------------- |
| **Memory Safety**     | Region-based (simpler)        | Borrow checker (complex)     |
| **Aliasing Control**  | Multiple mutable refs allowed | Exclusive &mut XOR shared &T |
| **Parameter Passing** | Uniform (all by permission)   | Copy types copy, others move |
| **Contracts**         | Built-in (must/will/uses)     | External tools only          |
| **State Machines**    | First-class (modals)          | Manual encoding              |
| **Effects**           | Explicit tracking             | Limited (const/unsafe)       |
| **Statements**        | Newline-terminated            | Semicolon-terminated         |
| **Metaprogramming**   | Comptime only                 | Powerful macros              |
| **Learning Curve**    | Simpler (for LLMs)            | More complex                 |
| **Safety Guarantees** | Fewer (regions + ownership)   | More (borrow checker)        |

---

## 2. Type System

### 2.1 Type Taxonomy

```
Cantrip Types
│
├── Primitive Types
│   ├── Integers: i8, i16, i32, i64, isize, u8, u16, u32, u64, usize
│   ├── Floats: f32, f64
│   ├── Boolean: bool
│   ├── Character: char (Unicode scalar value, 32-bit)
│   ├── Never: ! (bottom type, uninhabited)
│   └── String Types
│       ├── str (string slice, unsized)
│       └── String (owned, heap-allocated, growable)
│
├── Compound Types
│   ├── Product Types
│   │   ├── Unit: ()
│   │   ├── Tuples: (T₁, T₂, ...)
│   │   └── Records: record Name { fields }
│   ├── Sum Types
│   │   ├── Enums: enum Name { variants }
│   │   └── Modals: modal Name { @states }
│   └── Collections
│       ├── Arrays: [T; n] (fixed-size, stack)
│       └── Slices: [T] (dynamic view, dense pointer)
│
├── Reference Types
│   ├── Safe Pointers: Ptr<T>@State
│   └── Raw Pointers: *T, *mut T
│
├── Abstraction Types
│   ├── Behavioral Contracts: contract Name { ... }
│   ├── Code Reuse Traits: trait Name { ... }
│   ├── Generics: T<α₁, ..., αₙ>
│   ├── Const Generics: T<const N: usize>
│   └── Function Types: fn(T₁, ..., Tₙ) -> U
│
└── Permission Types
    ├── Owned: own T
    ├── Mutable: mut T
    └── Immutable: T (default)
```

### 2.2 Primitive Types

| Type     | Size (bytes) | Align | Range             | Copy | Default Literal |
| -------- | ------------ | ----- | ----------------- | ---- | --------------- |
| `i8`     | 1            | 1     | [-128, 127]       | ✓    | -               |
| `i16`    | 2            | 2     | [-32,768, 32,767] | ✓    | -               |
| `i32`    | 4            | 4     | [-2³¹, 2³¹-1]     | ✓    | `42`            |
| `i64`    | 8            | 8     | [-2⁶³, 2⁶³-1]     | ✓    | -               |
| `isize`  | ptr          | ptr   | platform          | ✓    | -               |
| `u8`     | 1            | 1     | [0, 255]          | ✓    | -               |
| `u16`    | 2            | 2     | [0, 65,535]       | ✓    | -               |
| `u32`    | 4            | 4     | [0, 2³²-1]        | ✓    | -               |
| `u64`    | 8            | 8     | [0, 2⁶⁴-1]        | ✓    | -               |
| `usize`  | ptr          | ptr   | platform          | ✓    | -               |
| `f32`    | 4            | 4     | IEEE 754          | ✓    | -               |
| `f64`    | 8            | 8     | IEEE 754          | ✓    | `3.14`          |
| `bool`   | 1            | 1     | {true, false}     | ✓    | -               |
| `char`   | 4            | 4     | Unicode scalar    | ✓    | -               |
| `String` | 24           | 8     | UTF-8 heap        | ✗    | -               |
| `!`      | 0            | 1     | ∅ (uninhabited)   | ✗    | -               |

### 2.3 Records (Product Types)

**Records are nominally typed user-defined types with named fields:**

```cantrip
record Point {
    x: f64
    y: f64

    procedure distance(self: Point, other: Point): f64 {
        let dx = self.x - other.x
        let dy = self.y - other.y
        (dx * dx + dy * dy).sqrt()
    }
}
```

**Memory layout** (64-bit):

```
Point { x: f64, y: f64 }
┌────────────┬────────────┐
│ x (8 bytes)│ y (8 bytes)│
└────────────┴────────────┘
Total: 16 bytes, alignment: 8
```

**Key properties:**

- Nominally typed (distinct by name, even with identical structure)
- Can have procedures (methods) with explicit `self` parameter
- Fields accessed with dot notation: `point.x`
- Default visibility is private

### 2.4 Enums (Sum Types)

**Enums are tagged unions with discriminant:**

```cantrip
enum Option<T> {
    Some(T)
    None
}

enum Result<T, E> {
    Ok(T)
    Err(E)
}

enum Message {
    Quit
    Move { x: i32, y: i32 }
    Write(String)
}
```

**Pattern matching** (exhaustiveness checked):

```cantrip
match result {
    Result.Ok(value) => process(value)
    Result.Err(error) => handle_error(error)
}
```

**Memory layout**:

```
enum Layout = {
    discriminant: usize    // Tag
    payload: union {       // Largest variant
        variant₁: T₁
        ...
    }
}

Size = sizeof(discriminant) + max(sizeof(variants)) + padding
```

### 2.5 Modals (State Machines)

**Modals are unique to Cantrip - types with explicit finite state machines:**

```cantrip
modal File {
    @Closed { path: String }
    @Open { path: String, handle: FileHandle }

    @Closed -> open() -> @Open
    @Open -> read() -> @Open
    @Open -> close() -> @Closed
}
```

**Type-level state tracking:**

```cantrip
let file: File@Closed = File.new("data.txt")
let opened: File@Open = file.open()  // Type changes!
// file.read()  // ERROR: File@Closed has no read()
opened.read()    // OK: File@Open has read()
```

**Compiler ensures:**

- Only valid transitions are allowed
- Invalid state access is caught at compile time
- No runtime overhead (states erased after type checking)

---

## 3. Contract System

### 3.1 CRITICAL DISTINCTION: Contracts vs Traits

**This is fundamental to Cantrip and completely different from Rust:**

#### Contracts: ABSTRACT Interfaces

Contracts define ONLY procedure signatures with clauses. **NO IMPLEMENTATIONS.**

```cantrip
contract Drawable {
    procedure draw($)
        uses io.write           // Abstract requirement - NO BODY

    procedure bounds($): Rectangle
        will {                  // Abstract guarantee - NO BODY
            result.width >= 0.0,
            result.height >= 0.0
        }

    // Contracts are PURELY ABSTRACT - only signatures + clauses
}
```

**Purpose**: Polymorphism, abstract interfaces, behavioral specifications

#### Traits: CONCRETE Code Reuse

Traits provide CONCRETE implementations that can be attached to types.

```cantrip
trait DefaultRenderable {
    procedure render($) {         // Concrete implementation
        println("Rendering default")
    }

    procedure render_with_color($, color: Color) {  // Concrete template
        $.set_color(color)
        $.render()
    }

    // Traits are CONCRETE - all procedures have bodies
}
```

**Purpose**: Code sharing, template implementations, mixins

#### When to Use Each

**Use contracts when:**

- Defining abstract interfaces for polymorphism
- Specifying behavioral requirements across unrelated types
- Building extensible APIs with strong guarantees
- You need interface polymorphism (different types, same interface)

**Use traits when:**

- Sharing concrete implementation code across types
- Providing default/template behavior
- Mixins and code reuse patterns
- Multiple types need the same implementation

### 3.2 Contract Components

Contracts in Cantrip consist of three types of clauses:

#### 3.2.1 Effects (`uses`)

**Effects declare what side effects a procedure may perform:**

```cantrip
procedure log(message: String)
    uses io.write, alloc.heap
{
    println("{}", message)
}
```

**Standard effect categories:**

```
alloc.*         Memory allocation
  .heap         Heap allocation
  .region       Region allocation
  .stack        Stack allocation

io.*            Input/output
  .read         Read operations
  .write        Write operations

fs.*            File system
  .read         Read files
  .write        Write files
  .delete       Delete files

net.*           Network
  .tcp          TCP operations
  .udp          UDP operations
  .http         HTTP operations

time.*          Time operations
  .read         Read current time
  .sleep        Sleep/delay

thread.*        Threading
  .spawn        Spawn threads
  .join         Join threads

ffi.*           Foreign function interface
  .call         FFI calls

unsafe.*        Unsafe operations
  .ptr          Raw pointer operations

panic           May panic
```

**Effect composition:**

- Effects are transitive through calls
- Caller must declare all effects of callees
- Pure procedures have no `uses` clause

#### 3.2.2 Preconditions (`must`)

**Preconditions specify what must be true before execution:**

```cantrip
procedure divide(numerator: f64, denominator: f64): f64
    must denominator != 0.0
{
    numerator / denominator
}
```

**Multiple preconditions:**

```cantrip
procedure transfer(from: mut Account, to: mut Account, amount: i64)
    must {
        amount > 0,
        from.balance >= amount,
        from.id != to.id
    }
{
    from.balance -= amount
    to.balance += amount
}
```

**Properties:**

- Checked at procedure entry
- Caller's responsibility to ensure
- Can be statically verified or dynamically checked

#### 3.2.3 Postconditions (`will`)

**Postconditions specify what must be true after execution:**

```cantrip
procedure increment(counter: mut i32): i32
    will result == @old(counter) + 1
{
    counter += 1
    counter
}
```

**Multiple postconditions:**

```cantrip
procedure clamp(value: i32, min: i32, max: i32): i32
    must min <= max
    will {
        result >= min,
        result <= max,
        (value >= min && value <= max) => result == value
    }
{
    if value < min {
        min
    } else if value > max {
        max
    } else {
        value
    }
}
```

**Special constructs:**

- `result` - The return value
- `@old(expr)` - Value of expression at procedure entry

**Properties:**

- Checked at procedure exit
- Procedure's responsibility to ensure
- Can reference initial state with `@old`

### 3.3 Complete Example with All Clauses

```cantrip
procedure read_config(path: String): Result<Config, Error>
    uses io.read, alloc.heap           // Effects
    must !path.is_empty()              // Precondition
    will match result {                // Postcondition
        Ok(cfg) => cfg.is_valid(),
        Err(_) => true
    }
{
    let contents = std::fs::read_to_string(path)?
    parse_config(contents)
}
```

---

## 4. Expression System

### 4.1 Unified Loop Construct

**Cantrip has ONE loop keyword for all iteration patterns:**

```cantrip
// Infinite loop
loop {
    body
}

// While-style (condition)
loop condition {
    body
}

// For-style (range)
loop i in 0..n {
    body
}

// Iterator-style
loop item in collection {
    body
}

// With termination metric
loop condition by metric {
    body
}

// With invariants
loop condition
    with { inv1, inv2, inv3 }
{
    body
}

// Complete verification
loop i in 0..n by n - i
    with {
        0 <= i,
        i <= n,
        sum == sum_of(arr[0..i])
    }
{
    body
}
```

**Loop control:**

```cantrip
break           // Exit loop (returns ())
break value     // Exit with value
continue        // Next iteration

// With labels
'outer: loop {
    'inner: loop {
        break 'outer     // Break outer loop
        continue 'inner  // Continue inner
    }
}
```

### 4.2 Pattern Matching

```cantrip
match value {
    Pattern1 => expression1,
    Pattern2 if guard => expression2,
    Pattern3 => expression3,
}
```

**Pattern types:**

```cantrip
// Literal patterns
42 => "answer"
true => "yes"

// Variable binding
x => process(x)

// Wildcard
_ => default_case()

// Tuple destructuring
(x, y) => x + y

// Enum patterns
Option.Some(value) => value
Option.None => 0

// Record patterns
Point { x, y } => x + y
Point { x: a, y: b } => a * b

// With guards
x if x > 0 => positive(x)
```

### 4.3 Control Flow

```cantrip
// If expression
let result = if condition {
    value1
} else {
    value2
}

// If-let pattern
if let Option.Some(value) = option_value {
    process(value)
}

// Return
return value

// Pipeline operator
let result = input
    => validate
    => transform
    => process
```

---

## 5. Memory Model and Permissions

### 5.1 Lexical Permission System (NOT Rust's Borrow Checker)

**Three permission types:**

| Permission    | Syntax        | Semantics          | Operations                       | Multiple Refs      |
| ------------- | ------------- | ------------------ | -------------------------------- | ------------------ |
| **Immutable** | `T` (default) | Borrowed reference | Read-only                        | ✓ Unlimited        |
| **Mutable**   | `mut T`       | Mutable reference  | Read + Write                     | ✓ Multiple allowed |
| **Owned**     | `own T`       | Full ownership     | All (read, write, move, destroy) | ✗ Exactly one      |

**KEY DIFFERENCE FROM RUST:**

```
CANTRIP                              RUST
───────────────────────────────────────────────────────
Multiple mut refs allowed            Single &mut XOR multiple &T
No borrow checker                    Borrow checker mandatory
Regions control lifetimes            Lifetime parameters ('a, 'b)
Programmer ensures aliasing safety   Compiler ensures aliasing safety
Simpler rules                        Stronger guarantees
```

### 5.2 Parameter Passing - Uniform Semantics

**ALL types pass by permission (reference-like) by default**, regardless of Copy trait:

```cantrip
procedure process(value: i32) { ... }      // Immutable permission
procedure modify(value: mut i32) { ... }   // Mutable permission
procedure consume(value: own i32) { ... }  // Owned permission

let x: own i32 = 42
process(x)         // Pass as immutable, x remains owned
modify(mut x)      // Pass as mutable, x remains owned
consume(move x)    // Transfer ownership, x unusable after
```

**This is the same for ALL types** (primitives, records, enums, etc.):

```cantrip
// Heap-allocated types - same pattern
let s: own String = String.new("hello")
print(s)           // Pass as immutable, s still usable
mutate(mut s)      // Pass as mutable, s still usable
consume(move s)    // Transfer ownership
```

### 5.3 Copy Trait - Explicit Copying Only

The `Copy` trait indicates a type **CAN be copied explicitly** via `.copy()` method:

```cantrip
let x: i32 = 42
let y = x           // y binds to x's value, x still usable
let z = x.copy()    // Explicit copy creates duplicate

// Copy does NOT change parameter passing:
procedure process(n: i32) {
    // n is passed by permission, no automatic copy
    let local = n.copy()  // Explicit copy when needed
}

process(x)  // x passed by permission, still usable after
```

**Copy-capable types:**

- All primitives (integers, floats, bool, char)
- Slices `[T]` (dense pointer only, not underlying data)
- Tuples/arrays of Copy types
- Records where all fields are Copy

**Non-Copy types:**

- String (heap-allocated)
- Vec<T> (heap-allocated)
- Records containing non-Copy fields
- All heap-allocated types

### 5.4 Memory Regions - Lifetime Control

**Cantrip uses explicit memory regions instead of lifetime parameters:**

```cantrip
region cache {
    let data = alloc_in<cache>(compute_data())
    process(data)
    // data automatically freed when cache ends
}
```

**Three allocation strategies:**

| Strategy   | Location      | Lifetime          | Dealloc Cost    | Use Case          |
| ---------- | ------------- | ----------------- | --------------- | ----------------- |
| **Stack**  | Stack         | Automatic (scope) | O(1)            | Local variables   |
| **Heap**   | Heap          | Manual/RAII       | O(1) per object | Long-lived data   |
| **Region** | Heap (region) | Bulk (region end) | O(1) bulk       | Temporary batches |

**Region properties:**

- Cannot escape - Values cannot outlive their region
- LIFO order - Nested regions deallocate in reverse order
- O(1) free - Entire region freed at once (vs O(n) individual frees)

---

## 6. Syntax Quick Reference

### 6.1 Statement Termination

**Newlines terminate statements by default. Semicolons are optional.**

**Primary rule:** A newline terminates a statement.

**4 Continuation rules** (statement continues across newlines when):

#### Rule 1: Unclosed Delimiters

Statement continues when `(`, `[`, or `<` remains unclosed:

```cantrip
let result = calculate(
    arg1,
    arg2,
    arg3
)

let array = [
    1, 2, 3,
    4, 5, 6
]
```

#### Rule 2: Trailing Operator

Statement continues when line ends with binary or assignment operator:

```cantrip
let total = base +
    modifier +
    bonus

let condition = x > 0 &&
    y < 100 &&
    z == 42
```

#### Rule 3: Leading Dot (Method Chaining)

Statement continues when next line begins with `.`:

```cantrip
result
    .validate()
    .transform()
    .process()

let value = builder
    .with_capacity(100)
    .with_name("example")
    .build()
```

#### Rule 4: Leading Pipeline

Statement continues when next line begins with `=>`:

```cantrip
let result = input
    => validate
    => transform
    => process
```

**Optional semicolons** can separate statements on one line:

```cantrip
let x = 1; let y = 2; let z = 3
```

### 6.2 Grammar Quick Reference

**Type syntax:**

```ebnf
Type ::= PrimitiveType
       | "[" Type ";" IntLiteral "]"    // Array
       | "[" Type "]"                    // Slice
       | "(" Type ("," Type)* ")"        // Tuple
       | Ident ("<" TypeArgs ">")?       // Generic
       | "own" Type | "mut" Type         // Permission

PrimitiveType ::= "i8" | "i16" | "i32" | "i64" | "isize"
                | "u8" | "u16" | "u32" | "u64" | "usize"
                | "f32" | "f64" | "bool" | "char"
                | "str" | "String" | "!"
```

**Expression syntax:**

```ebnf
Expr ::= Literal | Ident | "(" Expr ")"
       | Expr BinOp Expr | UnOp Expr
       | Expr "=>" Expr                    // Pipeline
       | "if" Expr BlockExpr ("else" ...)?
       | "loop" LoopHead? LoopVerif? Block
       | "match" Expr "{" MatchArm+ "}"
       | "let" Ident "=" Expr
       | FunctionCall | MethodCall
       | "{" Statement* "}"                // Block
       | "move" Expr                       // Ownership transfer
       | "region" Ident Block              // Region
```

**Contract syntax:**

```ebnf
ContractClause ::= UsesClause | MustClause | WillClause

UsesClause ::= "uses" EffectList
MustClause ::= "must" PredicateBlock
WillClause ::= "will" PredicateBlock

PredicateBlock ::= Assertion                    // Single
                 | "{" Assertion ("," Assertion)* "}"  // Multiple
```

### 6.3 Declaration Syntax

```cantrip
// Procedure with contracts
procedure name<Generics>(params): ReturnType
    uses effect1, effect2
    must precondition
    will postcondition
{
    body
}

// Record
record Name<T> {
    field1: Type1
    field2: Type2

    procedure method(self: Self, param: T): U { ... }
}

// Enum
enum Name<T> {
    Variant1
    Variant2(T)
    Variant3 { field: Type }
}

// Contract (abstract - NO IMPLEMENTATIONS)
contract Name<T> {
    procedure method(self: Self, param: T): U
        uses effects
        must precondition
        will postcondition
    // Note: NO procedure bodies in contracts
}

// Trait (concrete - WITH IMPLEMENTATIONS)
trait Name<T> {
    procedure method(self: Self, param: T): U {
        // Concrete implementation body
        ...
    }
}

// Modal
modal Name<T> {
    @State1 { fields }
    @State2 { fields }

    @State1 -> transition() -> @State2
}
```

---

## 7. Formal Notation

### 7.1 Metavariables

```
x, y, z ∈ Var          (term variables)
f, g, h ∈ FunName      (function names)
m, n, o ∈ ProcName     (procedure names)
T, U, V ∈ Type         (types)
e, e₁, e₂ ∈ Expr       (expressions)
v, v₁, v₂ ∈ Value      (values)
p, p₁, p₂ ∈ Pattern    (patterns)
ε, ε₁, ε₂ ∈ Effect     (effects)
P, Q, R ∈ Assertion    (contract assertions)
σ, σ', σ'' ∈ Store     (memory stores)
Γ, Δ, Θ ∈ Context      (type contexts)
@S, @S' ∈ State        (modal states)
n, m, k ∈ ℕ            (natural numbers)
```

### 7.2 Judgment Forms

```
Γ ⊢ e : T                  (e has type T in context Γ)
Γ ⊢ e : T ! ε              (e has type T with effects ε)
Γ ⊢ T : Type               (T is well-formed)
Γ ⊢ T₁ <: T₂               (T₁ is subtype of T₂)
Γ ⊢ T₁ ≡ T₂                (T₁ and T₂ are equivalent)
Γ ⊢ p : T ⇝ Γ'             (pattern p matches T, extends context)

⟨e, σ⟩ → ⟨e', σ'⟩          (small-step reduction)
⟨e, σ⟩ ⇓ ⟨v, σ'⟩            (big-step evaluation)

{P} e {Q}                  (Hoare triple)
σ ⊨ P                      (store satisfies assertion)

@S₁ →ₘ @S₂                 (modal state transition via method m)
Σ ⊢ T@S₁ →ₘ T@S₂            (valid state transition)
```

### 7.3 Type Rules (Examples)

```
[T-Var] Variable lookup
x : T ∈ Γ
─────────
Γ ⊢ x : T

[T-Int] Integer literal (default i32)
─────────────
Γ ⊢ n : i32

[T-Let] Let binding
Γ ⊢ e₁ : T₁    Γ, x : T₁ ⊢ e₂ : T₂
───────────────────────────────────
Γ ⊢ let x = e₁ in e₂ : T₂

[T-App] Function application
Γ ⊢ e₁ : T → U    Γ ⊢ e₂ : T
──────────────────────────────
Γ ⊢ e₁(e₂) : U

[T-If] Conditional
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : T    Γ ⊢ e₃ : T
──────────────────────────────────────────
Γ ⊢ if e₁ then e₂ else e₃ : T

[T-Record] Record construction
record R { f₁: T₁; ...; fₙ: Tₙ } well-formed
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────────────
Γ ⊢ R { f₁: e₁, ..., fₙ: eₙ } : R

[T-Match] Pattern matching
Γ ⊢ e : E
enum E { V₁(T₁), ..., Vₙ(Tₙ) }
∀i. Γ, pᵢ ⊢ eᵢ : U
patterns {p₁, ..., pₙ} exhaustive for E
──────────────────────────────────────────────
Γ ⊢ match e { p₁ → e₁, ..., pₙ → eₙ } : U

[T-Modal-Transition] Modal state transition
Γ ⊢ e : modal P@S₁
S₁ →ₘ S₂ ∈ transitions(P)
Γ ⊢ m : proc(mut self: modal P@S₁) → modal P@S₂
────────────────────────────────────────────────────
Γ ⊢ e.m() : modal P@S₂
```

---

## 8. Quick Lookup Tables

### 8.1 Reserved Keywords (54 Total)

```
abstract, as, async, await, break, by, case, comptime, const, continue,
defer, effect, else, enum, exists, false, forall,
if, import, internal, invariant, iso, let, loop,
match, modal, module, move, must, mut, new, none, own, private,
procedure, protected, public, record, ref, region, result,
select, self, Self, state, static, trait, true, type, uses, var, where, will, with
```

**Note:** `needs`, `requires`, and `ensures` are DEPRECATED (replaced by `uses`, `must`, `will`).

### 8.2 Operator Precedence (15 Levels, High to Low)

| Level | Operators                    | Associativity | Example                       |
| ----- | ---------------------------- | ------------- | ----------------------------- |
| 1     | `.` `[]` `()`                | Left          | `obj.field`, `arr[i]`, `f(x)` |
| 2     | `!` `-` `*` `&` `mut`        | Right         | `!x`, `-y`, `*ptr`, `&ref`    |
| 3     | `**`                         | Right         | `x ** y`                      |
| 4     | `*` `/` `%`                  | Left          | `x * y`, `x / y`, `x % y`     |
| 5     | `+` `-`                      | Left          | `x + y`, `x - y`              |
| 6     | `<<` `>>`                    | Left          | `x << 2`, `x >> 3`            |
| 7     | `&`                          | Left          | `x & y`                       |
| 8     | `^`                          | Left          | `x ^ y`                       |
| 9     | `\|`                         | Left          | `x \| y`                      |
| 10    | `<` `<=` `>` `>=` `==` `!=`  | Left          | `x < y`, `x == y`             |
| 11    | `&&`                         | Left          | `x && y`                      |
| 12    | `\|\|`                       | Left          | `x \|\| y`                    |
| 13    | `..` `..=`                   | Left          | `0..10`, `0..=10`             |
| 14    | `=>`                         | Left          | `x => f`                      |
| 15    | `=` `+=` `-=` `*=` `/=` etc. | Right         | `x = y`, `x += 1`             |

### 8.3 Standard Library Core Types

```cantrip
// Option type
enum Option<T> {
    Some(T)
    None
}

// Result type
enum Result<T, E> {
    Ok(T)
    Err(E)
}

// String types
str             // String slice (borrowed, unsized)
String          // Owned string (heap-allocated, growable)

// Collections
Vec<T>          // Growable array
HashMap<K, V>   // Hash table
HashSet<T>      // Hash set

// Concurrency
Mutex<T>        // Mutual exclusion
Channel<T>      // Message passing
AtomicU32       // Atomic operations
```

### 8.4 Common Patterns

#### Pattern 1: Error Handling with ?

```cantrip
procedure read_file(path: String): Result<String, Error>
    uses io.read, alloc.heap
{
    let contents = std::fs::read_to_string(path)?
    Ok(contents)
}
```

#### Pattern 2: Contract-Based API

```cantrip
procedure divide(a: f64, b: f64): Result<f64, String>
    will match result {
        Ok(v) => v == a / b,
        Err(_) => b == 0.0
    }
{
    if b == 0.0 {
        Err("Division by zero")
    } else {
        Ok(a / b)
    }
}
```

#### Pattern 3: Modal State Machine

```cantrip
modal Connection {
    @Disconnected { url: String }
    @Connected { url: String, socket: Socket }

    @Disconnected -> connect() -> @Connected
    @Connected -> send(data: [u8]) -> @Connected
    @Connected -> disconnect() -> @Disconnected
}

procedure use_connection() {
    let conn: Connection@Disconnected = Connection.new("localhost")
    let active: Connection@Connected = conn.connect()
    active.send(data)
    let closed: Connection@Disconnected = active.disconnect()
}
```

#### Pattern 4: Region-Based Allocation

```cantrip
procedure parse_file(path: String): Result<Data, Error>
    uses alloc.region, io.read
{
    region temp {
        let contents = String.new_in<temp>()
        std::fs::read_to_string(path, mut contents)?

        let parsed = parse_json(contents)?

        parsed.to_heap()  // Convert to heap before return
    }  // All temp strings freed in O(1)
}
```

---

## Summary

This document provides:

✓ **Accurate language semantics** — Types, permissions, contracts, memory model as specified in Cantrip 1.3.0
✓ **Clear distinctions from Rust** — Explicit about what is different
✓ **Formal notation** — Metavariables, judgment forms, inference rules
✓ **Practical syntax** — EBNF grammars, code examples, common patterns
✓ **Design philosophy** — Explicit vs implicit, safety trade-offs, LLM-friendly design

**Key Takeaways for LLMs:**

1. **This is NOT Rust** - Do not apply Rust semantics
2. **Contracts ≠ Traits** - Abstract interfaces vs concrete code reuse
3. **Newlines matter** - Statements are newline-terminated with 4 continuation rules
4. **Uniform passing** - All types pass by permission, regardless of Copy
5. **Multiple mutable refs OK** - No borrow checker, programmer responsibility
6. **Regions not lifetimes** - Explicit region blocks, not lifetime parameters
7. **Modal types** - State machines as first-class types (unique to Cantrip)
8. **Syntax: must/will/uses** - Not requires/ensures/needs

---

# Chapter #: System Name (Template)

---

# §X Section Name

## §X.0 Overview

### §X.0.1 Scope and Goals

Outline the mandate of this chapter. State which kinds of type-level constructs MUST be specified here, which topics are deferred to other parts, and any explicit non-goals.

### §X.0.2 Dependencies and Notation

Enumerate cross-chapter dependencies (permissions, effects, contracts, regions) and restate shared metavariables or symbols. Each dependency SHOULD cite the exact section it relies on.

### §X.0.3 Guarantees

State the normative guarantees granted by the type system (memory safety, permission soundness, effect discipline). Use MUST/SHOULD/MAY terminology only.

---

## §X.1 Syntax

### §X.1.1 Concrete Grammar

Provide EBNF productions for all type-related syntactic forms introduced or constrained in this chapter. Reference external productions by anchor when reused.

### §X.1.2 Abstract Syntax

List the abstract syntax (judgment forms, metavariables) with brief explanations. Maintain a consistent order with the concrete grammar.

### §X.1.3 Derived Forms

Document any derived forms or syntactic sugar. Each entry MUST specify its desugaring into core forms defined in §X.1.1.

---

## §X.2 Static Semantics

### §X.2.1 Typing Contexts and Judgments

Define the structure of contexts and the primary judgment forms. Present inference-rule schemata with concise labels (e.g., `[WF-TYPE]`).

### §X.2.2 Well-Formedness Rules

List well-formedness conditions for each construct. Each rule MUST cite prerequisites and the contexts they operate in.

### §X.2.3 Subtyping and Equivalence

Formalize subtype, equivalence, or compatibility relations. Provide axioms and derived rules; identify variance properties explicitly.

### §X.2.4 Permission-Indexed Types

Spell out how type constructors interact with the Lexical Permission System. Include rules for permission propagation and constraints on aliasing.

### §X.2.5 Effect-Annotated Function Types

Define the typing of function, procedure, and closure forms with effect signatures. Contracts MUST use `uses`, `must`, and `will` clauses.

---

## §X.3 Dynamic Semantics

### §X.3.1 Runtime Obligations

Describe runtime checks or obligations imposed by the type system (e.g., bounds checks, modal guard conditions). Reference the operational semantics chapter for transition rules.

### §X.3.2 Region Exit Semantics

Explain how regions interact with the types introduced here. Specify the LIFO deallocation requirements and constraints on value escape.

### §X.3.3 Unsafe Interaction

Detail how unsafe effects (e.g., `unsafe.ptr`) are gated by the type system. Make explicit which permissions/effects MUST be present to permit unsafe operations.

---

## §X.4 Algorithms

### §X.4.1 Bidirectional Typing Procedure

Describe the algorithm used to synthesize and check types. Inputs, required effects, and postconditions MUST be explicit.

### §X.4.2 Constraint Solving and Coherence

Provide the constraint generation and solving process, including occurs checks, trait coherence, and termination arguments.

### §X.4.3 Error Diagnostics

List mandatory diagnostics and their triggering conditions. Reference diagnostic codes where the specification defines them.

---

## §X.5 Proven Properties

### §X.5.1 Progress

State the theorem, premises, and proof obligations. Cite the rules from §X.2 used in the proof sketch.

### §X.5.2 Preservation

As above, detailing how evaluation preserves typing judgments.

### §X.5.3 Soundness Corollaries

Enumerate corollaries (memory safety, permission safety) derived from Progress and Preservation.

### §X.5.4 Parametricity and Principal Types

Specify the scope where these theorems apply. Note any prerequisites (e.g., absence of higher-kinded traits).

---

## §X.6 Interaction Summaries

Create tables summarizing, for each construct, the required permissions, available effects, contract clauses, and region obligations. Each entry MUST link back to the normative rule that enforces it.

---

## §X.A Appendices (Non-Normative)

### §X.A.1 Taxonomy Diagram and Cross-Reference Table

Provide visual summaries or tables that assist navigation. Mark the appendix as non-normative.

### §X.A.2 Extended Examples

Include worked examples demonstrating exhaustive `match` patterns, explicit region scopes, and permission passing. These MUST not introduce new normative requirements.

### §X.A.3 Specification Status & Backlog

Track missing or out-of-scope items with references to issue IDs or future work documents. Keep status content out of the normative sections.

---

**Document Version:** 2.0.0
**Last Updated:** 2025-01-28
**Maintainer:** Cantrip Language Team
**License:** Same as Cantrip Language Specification
