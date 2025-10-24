# Cantrip Language Specification - LLM Quick Reference

**Version:** 1.2.0  
**Date:** 2025-10-24  
**Purpose:** Token-efficient, complete reference for LLMs working with/extending Cantrip specification  
**Target Audience:** LLMs, AI assistants, automated tools

**Recent Changes (v1.2.0):**
- Unified loop syntax: ONE `loop` keyword for all iteration patterns
- Loop verification: `by` (variant) and `with` (invariants) clauses
- Break with values: `break expr` returns value from loop
- Loop labels: `'label: loop` for nested loop control

**Previous Changes (v1.1.0):**
- Contract syntax updated: `needs`→`uses`, `requires`→`must`, `ensures`→`will`
- Block syntax added for multiple contract conditions
- Effects now comma-separated
- 40% reduction in contract keyword characters

---

## 1. Executive Overview

### 1.1 Design Philosophy

Cantrip is a systems programming language with five core principles:

1. **Explicit over implicit** — All effects, lifetimes, and permissions visible in code
2. **Local reasoning** — Understanding code requires minimal global context
3. **Zero-cost abstractions** — Safety guarantees without runtime overhead
4. **LLM-friendly** — Predictable patterns for AI code generation
5. **Simple ownership** — No borrow checker complexity

### 1.2 Safety Model

**What Cantrip PREVENTS (✓):**
- Use-after-free (region-based lifetimes)
- Double-free (single ownership)
- Memory leaks (RAII-style cleanup)

**What Cantrip does NOT prevent (programmer's responsibility):**
- Aliasing bugs (multiple mutable references allowed)
- Data races (no automatic synchronization)
- Iterator invalidation (no borrow checker)

**Trade-off:** Memory safety without borrow checker complexity, at the cost of requiring programmer discipline for aliasing safety.

### 1.3 Key Innovations

1. **LPS (Lexical Permission System)**: Compile-time memory safety without borrow checking
2. **Contract System** (with effects): Preconditions + postconditions + effect tracking (NOT a separate system)
3. **Modal System**: State machines as first-class types with compile-time verification
4. **Memory Regions**: Explicit lifetime control with O(1) bulk deallocation
5. **Comptime**: Compile-time code generation without macros

### 1.4 Architecture

```
Core Systems:
├── Type System (Part II)
│   ├── Primitives (i32, f64, bool, char, String, !)
│   ├── Compounds (tuples, arrays, records, enums)
│   ├── Generics (parametric polymorphism)
│   └── Modals (state machines as types)
├── Permission System (Part III) - LPS
│   ├── own (ownership)
│   ├── mut (mutable reference)
│   ├── [default] (immutable reference)
│   └── iso (isolated reference)
├── Contracts and Clauses (Part IV)
│   ├── Behavioral Contracts (contract keyword)
│   ├── Effect Clauses (uses)
│   ├── Precondition Clauses (must)
│   ├── Postcondition Clauses (will)
│   └── Constraint Clauses (where)
├── Memory Regions (Part VI)
│   ├── Stack (automatic)
│   ├── Heap (manual/RAII)
│   └── Region (bulk deallocation)
└── Module System (Part VIII)
```

### 1.5 Comparison to Other Languages

| Feature | Cantrip | Rust | Cyclone | ML |
|---------|---------|------|---------|-----|
| Ownership | ✓ | ✓ | ✓ | ✗ |
| Borrow checker | ✗ | ✓ | ✗ | ✗ |
| Regions | ✓ | ✗ | ✓ | ✗ |
| Multiple mut refs | ✓ | ✗ | ✓ | N/A |
| Formal semantics | ✓ | Informal | Formal | ✓ |
| Effect system | ✓ | ✗ | ✗ | ✗ |
| State machines | ✓ (modals) | ✗ | ✗ | ✗ |

**Key difference from Rust:** Cantrip allows multiple mutable references simultaneously (Cyclone model), trading compile-time aliasing guarantees for simpler ownership rules.

---

## 2. Type System Complete

### 2.1 Type Taxonomy (45+ forms)

```
Value Types (τ)
│
├── Primitive Types
│   ├── Integers: i8, i16, i32, i64, isize (signed)
│   │            u8, u16, u32, u64, usize (unsigned)
│   ├── Floats: f32 (IEEE 754 single), f64 (IEEE 754 double)
│   ├── Boolean: bool
│   ├── Character: char (Unicode scalar value, 32-bit)
│   ├── String Types: String
│   └── Never: ! (bottom type, uninhabited)
│
├── Compound Types
│   ├── Products (Cartesian product)
│   │   ├── Unit: ()
│   │   ├── Tuples: (T₁, ..., Tₙ)
│   │   ├── Records: record R { f₁: T₁; ...; fₙ: Tₙ }
│   │   ├── Arrays: [T; n] (fixed-size, stack)
│   │   └── Slices: [T] (dynamic view, fat pointer)
│   │
│   └── Sums (Disjoint union)
│       ├── Enums: enum E { V₁(T₁), ..., Vₙ(Tₙ) }
│       └── Modals: modal M { @S₁, ..., @Sₙ } (state-indexed)
│
├── Parametric Types
│   ├── Generics: T<α₁, ..., αₙ>
│   ├── Const Generics: T<const N: usize>
│   └── Associated Types: trait I { type A; }
│
├── Abstraction Types
│   ├── Traits: trait I { ... }
│   ├── Trait Objects: dyn I (dynamic dispatch)
│   ├── Function Types: fn(T₁, ..., Tₙ) → U
│   └── Procedure Types: proc(self: Self, T₁, ..., Tₙ) → U
│
└── Permission-Annotated Types
    ├── own T (owned, exclusive)
    ├── mut T (mutable reference)
    ├── T (immutable reference, default)
    └── iso T (isolated reference)
```

### 2.2 Primitive Types

| Type | Size | Align | Range | Copy | Default Literal |
|------|------|-------|-------|------|-----------------|
| `i8` | 1 | 1 | [-128, 127] | ✓ | — |
| `i16` | 2 | 2 | [-32,768, 32,767] | ✓ | — |
| `i32` | 4 | 4 | [-2³¹, 2³¹-1] | ✓ | `42` |
| `i64` | 8 | 8 | [-2⁶³, 2⁶³-1] | ✓ | — |
| `isize` | ptr | ptr | platform | ✓ | — |
| `u8` | 1 | 1 | [0, 255] | ✓ | — |
| `u16` | 2 | 2 | [0, 65,535] | ✓ | — |
| `u32` | 4 | 4 | [0, 2³²-1] | ✓ | — |
| `u64` | 8 | 8 | [0, 2⁶⁴-1] | ✓ | — |
| `usize` | ptr | ptr | platform | ✓ | — |
| `f32` | 4 | 4 | IEEE 754 | ✓ | — |
| `f64` | 8 | 8 | IEEE 754 | ✓ | `3.14` |
| `bool` | 1 | 1 | {true, false} | ✓ | — |
| `char` | 4 | 4 | Unicode scalar | ✓ | — |
| `String` | 24 | 8 | UTF-8 heap | ✗ | — |
| `!` | 0 | 1 | ∅ (uninhabited) | ✗ | — |

**Platform-dependent sizes:**
- 32-bit: `isize` = `i32`, `usize` = `u32`, pointers = 4 bytes
- 64-bit: `isize` = `i64`, `usize` = `u64`, pointers = 8 bytes

### 2.3 String Type (Exemplar Specification)

**Definition:** String is a heap-allocated, growable, UTF-8 encoded text type.

**Memory layout (64-bit):**
```
String { ptr: *u8, len: usize, cap: usize }
┌────────────┬────────────┬────────────┐
│ ptr (8B)    │ len (8B)    │ cap (8B)   │
└────────────┴────────────┴────────────┘
Total: 24 bytes, alignment: 8 bytes
```

**Invariant:** `valid_utf8(ptr[0..len]) = true` at all times

**Key operations:**
```cantrip
String.new() → own String               // uses alloc.heap
String.from(s: str) → own String        // uses alloc.heap
s.push_str(text: str)                   // uses alloc.heap (may realloc)
s.len() → usize                         // pure
s.chars() → Iterator<char>              // pure
```

**Copy vs Move:** String is NOT Copy (heap-allocated), requires explicit `move` for ownership transfer.

### 2.4 Type Rules (Core 15+ Patterns)

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

[T-Abs] Lambda abstraction
Γ, x : T ⊢ e : U
────────────────────
Γ ⊢ λx:T. e : T → U

[T-If] Conditional
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : T    Γ ⊢ e₃ : T
──────────────────────────────────────────
Γ ⊢ if e₁ then e₂ else e₃ : T

[T-Record] Record construction
record R { f₁: T₁; ...; fₙ: Tₙ } well-formed
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────────────
Γ ⊢ R { f₁: e₁, ..., fₙ: eₙ } : R

[T-Field] Field access
Γ ⊢ e : record R    field f : T in R
────────────────────────────────────
Γ ⊢ e.f : T

[T-Enum] Enum variant construction
enum E { ..., V(T), ... } well-formed
Γ ⊢ e : T
──────────────────────────────────────────
Γ ⊢ E.V(e) : E

[T-Match] Pattern matching
Γ ⊢ e : E
enum E { V₁(T₁), ..., Vₙ(Tₙ) }
∀i. Γ, pᵢ ⊢ eᵢ : U
patterns {p₁, ..., pₙ} exhaustive for E
──────────────────────────────────────────────
Γ ⊢ match e { p₁ → e₁, ..., pₙ → eₙ } : U

[T-Array] Array construction
Γ ⊢ e₁ : T    ...    Γ ⊢ eₙ : T
──────────────────────────────────
Γ ⊢ [e₁, ..., eₙ] : [T; n]

[T-Index] Array indexing
Γ ⊢ arr : [T; n]    Γ ⊢ i : usize    i < n
───────────────────────────────────────────────
Γ ⊢ arr[i] : T

[T-Tuple] Tuple construction
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────
Γ ⊢ (e₁, ..., eₙ) : (T₁, ..., Tₙ)

[T-Pipeline] Pipeline operator
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T → U
──────────────────────────────
Γ ⊢ e₁ => e₂ : U

[T-Modal-Transition] Modal state transition
Γ ⊢ e : modal P@S₁
S₁ →ₘ S₂ ∈ transitions(P)
Γ ⊢ m : proc(mut self: modal P@S₁) → modal P@S₂
────────────────────────────────────────────────────
Γ ⊢ e.m() : modal P@S₂
```

### 2.5 Subtyping Rules

```
[Sub-Refl] Reflexivity
─────────
T <: T

[Sub-Trans] Transitivity
T <: U    U <: V
────────────────
T <: V

[Sub-Never] Never is bottom
─────────
! <: T

[Sub-Permission] Permission hierarchy
own T <: mut T <: T
iso T <: own T

[Sub-Fun] Function (contravariant args, covariant return)
T₂ <: T₁    U₁ <: U₂
────────────────────────
T₁ → U₁ <: T₂ → U₂
```

**Invariance:** Most compound types (arrays, slices, tuples, generics) are invariant to prevent unsound mutation.

### 2.6 Records

**Syntax:**
```cantrip
record Point {
    x: f64;
    y: f64;

    procedure distance(self: Point, other: Point): f64 {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }
}
```

**Memory layout:**
```
record Point { x: f64; y: f64; }
┌────────────┬────────────┐
│ x (8 bytes) │ y (8 bytes)│
└────────────┴────────────┘
Total: 16 bytes, alignment: 8

record Padded { a: u8; b: u32; c: u16; }
┌──┬───────┬────────┬────┬──┐
│a │ pad    │   b     │ c  │p │
└──┴───────┴────────┴────┴──┘
Total: 12 bytes (aligned to 4)
```

**Procedures:** Methods on records with explicit `self` parameter.

### 2.7 Enums (Sum Types)

**Syntax:**
```cantrip
enum Option<T> {
    Some(T),
    None,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
}
```

**Pattern matching (exhaustiveness checked):**
```cantrip
match result {
    Result.Ok(value) → process(value),
    Result.Err(error) → handle_error(error),
}
```

**Memory layout:**
```
enum Layout = {
    discriminant: usize,    // Tag
    payload: union {        // Largest variant
        variant₁: T₁,
        ...
    }
}

Size = sizeof(discriminant) + max(sizeof(T₁), ..., sizeof(Tₙ)) + padding
```

### 2.8 Modals (State Machines)

**Definition:** Modals are types with explicit finite state machines, where each state can have different data and the compiler enforces valid state transitions.

**Syntax:**
```cantrip
modal File {
    @Closed { path: String }
    @Open { path: String; handle: FileHandle }
    
    @Closed → open() → @Open;
    @Open → read() → @Open;
    @Open → close() → @Closed;
}
```

**Type-level state tracking:**
```cantrip
let file: File@Closed = File.new("data.txt");
let opened: File@Open = file.open();  // Type changes!
// file.read();  // ERROR: File@Closed has no read()
opened.read();    // OK: File@Open has read()
```

**Verification:** Compiler ensures only valid transitions, preventing invalid state access.

---

## 3. Permission System (LPS)

### 3.1 Four Permissions

| Permission | Syntax | Semantics | Operations | Multiple Refs |
|------------|--------|-----------|------------|---------------|
| **Immutable** | `T` or `x: T` | Borrowed reference | Read-only (len, get, ==, <) | ✓ Unlimited |
| **Mutable** | `mut T` | Mutable reference | Read + Write (push, clear) | ✓ Multiple allowed |
| **Owned** | `own T` | Full ownership | All (read, write, move, destroy) | ✗ Exactly one |
| **Isolated** | `iso T` | Isolated reference | Transfer-safe | Restricted |

### 3.2 Key Differences from Rust

```
CANTRIP                              RUST
─────────────────────────────────────────────────────────
Multiple mut refs allowed            Single mut ref XOR multiple immut refs
No borrow checker                    Borrow checker mandatory
Regions control lifetimes            Lifetime parameters ('a, 'b)
Programmer ensures aliasing safety   Compiler ensures aliasing safety
Trade-off: Simpler rules             Trade-off: Stronger guarantees
```

### 3.3 Permission Transitions

```
own T → T        (pass as immutable reference, keep ownership)
own T → mut T    (pass as mutable reference, keep ownership)
own T → [moved]  (explicit transfer: move x)

Subtyping: own T <: mut T <: T
```

**Example:**
```cantrip
procedure read_only(text: String) { ... }      // Immutable
procedure modify(text: mut String) { ... }     // Mutable
procedure consume(text: own String) { ... }    // Owned

let s: own String = String.new();
read_only(s);           // Pass as immutable, keep ownership
modify(mut s);          // Pass as mutable, keep ownership
consume(move s);        // Transfer ownership, s unusable after
```

### 3.4 Copy vs Move Semantics

**Copy types (automatically copied on assignment):**
- All primitives: integers, floats, bool, char
- Tuples/arrays of Copy types
- Records where all fields are Copy

**Move types (require explicit `move`):**
- String (heap-allocated)
- Vec<T> (heap-allocated)
- Records containing non-Copy fields
- All heap-allocated types

**Example:**
```cantrip
// Copy
let x: i32 = 42;
let y = x;  // x still usable (copied)

// Move
let s1: own String = String.new();
let s2 = move s1;  // s1 no longer usable (moved)
```

### 3.5 Safety Guarantees

**What Cantrip PREVENTS (✓):**
```
✓ Use-after-free
    region temp {
        let s = String.new_in<temp>();
        // return s;  // ERROR: Cannot escape region
    }

✓ Double-free
    let s = String.new();
    consume(move s);
    // consume(move s);  // ERROR: Value already moved

✓ Memory leaks
    {
        let temp = String.new();
    }  // Automatically freed (RAII)
```

**What Cantrip does NOT prevent (⚠ programmer's responsibility):**
```
⚠ Aliasing bugs
    var text = String.new();
    modify1(mut text);
    modify2(mut text);  // Both can modify - could conflict!

⚠ Data races
    thread1.spawn(|| modify(mut shared));
    thread2.spawn(|| modify(mut shared));  // Race!

⚠ Iterator invalidation
    for c in text.chars() {
        text.push_str("x");  // ALLOWED but dangerous!
    }
```

---

## 4. Contract System (with Effects as Component)

### 4.1 Contract Components

Contracts specify function behavior through three components:

1. **Effects** (uses) — Side effects and capabilities
2. **Preconditions** (must) — What must be true before execution
3. **Postconditions** (will) — What must be true after execution

**Syntax:**
```cantrip
procedure divide(x: i32, y: i32): i32
    uses alloc.heap;              // ← EFFECTS (part of contract)
    must y != 0;                  // ← PRECONDITION
    will result == x / y;         // ← POSTCONDITION
{ x / y }
```

### 4.2 Effects (Component of Contracts)

**IMPORTANT:** Effects are NOT a separate system. They are a component of the contract system, specifying side effects and resource usage.

**Effect Categories:**
```
alloc.*         Memory allocation
  .heap         Heap allocation (malloc/free)
  .region       Region allocation (bulk free)
  .stack        Stack allocation
  .temp         Temporary allocation
  .*            Any allocation

io.*            Input/output
  .read         Read operations
  .write        Write operations
  .*            Any I/O

fs.*            File system
  .read         Read files/directories
  .write        Write files/directories
  .delete       Delete files/directories
  .*            Any file system operation

net.*           Network
  .read(d)      Network read (d ∈ {inbound, outbound})
  .write        Network write
  .*            Any network operation

time.*          Time operations
  .read         Read current time
  .sleep(d≤n)   Sleep/delay (bounded)
  .*            Any time operation

thread.*        Threading
  .spawn(n≤k)   Spawn threads (bounded count)
  .join         Join threads
  .*            Any threading

panic           Divergence (function may panic)
random          Non-determinism
```

**Effect Syntax:**
```cantrip
// Single effect
uses alloc.heap;

// Multiple effects
uses alloc.heap, io.write, fs.read;

// Wildcards
uses alloc.*;       // Any allocation
uses fs.*, net.*;   // Any FS or network

// Forbidden effects (negative)
uses *, !panic;     // All effects except panic
uses io.*, !io.write;  // Read-only I/O

// Bounded effects
uses alloc.heap(bytes≤1024);     // At most 1KB
uses thread.spawn(count≤4);      // At most 4 threads
uses time.sleep(duration≤10ms);  // At most 10ms
```

**Effect Checking:**
```
Functions MUST declare all effects they perform:
- Directly in body
- Transitively through called functions

effect(function) = effect(body) ∪ ⋃(effect(called_fn))
```

**Example:**
```cantrip
procedure write_log(message: String)
    uses fs.write, alloc.heap;
{
    let path = String.from("/var/log/app.log");  // uses alloc.heap
    std.fs.append(path, message);                // uses fs.write
}

procedure process_request(req: Request): Response
    uses fs.write, alloc.heap, io.write;  // Includes transitive effects
{
    write_log(format!("Processing {}", req));  // uses fs.write, alloc.heap
    respond(req)                               // uses io.write
}
```

### 4.3 Preconditions (must)

**Syntax:**
```cantrip
procedure get<T>(arr: [T], idx: usize): T
    must idx < arr.len();
{ arr[idx] }
```

**Multiple preconditions (conjunction):**
```cantrip
procedure transfer(from: mut Account, to: mut Account, amount: u64)
    must {
        from.balance >= amount,
        amount > 0,
        to.id != from.id,
    }
{ ... }
```

**Assertion language:**
```
P, Q ::= true | false
       | e₁ == e₂ | e₁ != e₂
       | e₁ < e₂ | e₁ <= e₂ | e₁ > e₂ | e₁ >= e₂
       | P ∧ Q | P ∨ Q | ¬P | P ⇒ Q
       | ∀x. P | ∃x. P
       | forall(x in collection) { P }
       | exists(x in collection) { P }
```

### 4.4 Postconditions (will)

**Syntax:**
```cantrip
procedure increment(x: mut i32)
    will *x == @old(*x) + 1;
{ *x += 1; }

procedure append<T>(vec: mut Vec<T>, item: T)
    uses alloc.heap;
    will {
        vec.len() == @old(vec.len()) + 1,
        vec[vec.len() - 1] == item,
    }
{ vec.push(item); }
```

**Special constructs:**
- `@old(expr)` — Value of expression before function execution
- `result` — Return value (in postcondition)

**Example:**
```cantrip
procedure divide(x: i32, y: i32): i32
    must y != 0;
    will {
        result == x / y,
        result * y <= x,  // Integer division rounds down
    }
{ x / y }
```

### 4.5 Hoare Logic

**Hoare triple:** `{P} e {Q}` means "If P holds before e, then Q holds after"

**Verification rules:**
```
[Hoare-Consequence]
{P'} e {Q'}    P ⇒ P'    Q' ⇒ Q
───────────────────────────────
{P} e {Q}

[Hoare-Sequence]
{P} e₁ {Q}    {Q} e₂ {R}
──────────────────────────
{P} e₁; e₂ {R}

[Hoare-If]
{P ∧ b} e₁ {Q}    {P ∧ ¬b} e₂ {Q}
─────────────────────────────────
{P} if b then e₁ else e₂ {Q}
```

### 4.6 Verification Modes

Control how contracts are checked:

```cantrip
#[verify(static)]    // Compile-time verification via SMT solver
procedure critical(x: i32): i32
    must x >= 0;
    will result >= x;
{ x + 1 }

#[verify(runtime)]   // Runtime assertion injection
procedure parse(s: str): i32
    must !s.is_empty();
{ s.parse().unwrap() }

#[verify(none)]      // Documentation only, no checking
procedure approximate(x: f64): f64
    will result >= 0.0;
{ x.abs() }
```

---

## 5. Memory Model

### 5.1 Three Allocation Strategies

| Strategy | Location | Lifetime | Dealloc Cost | Allocation Cost | Use Case |
|----------|----------|----------|--------------|-----------------|----------|
| **Stack** | Stack | Automatic (scope) | O(1) | O(1) | Local variables |
| **Heap** | Heap | Manual/RAII | O(1) per object | O(1) per object | Long-lived data |
| **Region** | Heap (region) | Bulk (region end) | O(1) bulk | O(1) bump | Temporary batches |

### 5.2 Region-Based Memory

**Syntax:**
```cantrip
region name {
    let s = String.new_in<name>();
    // s allocated in region 'name'
}  // All region memory freed in O(1)
```

**Properties:**
- **Cannot escape:** Values cannot outlive their region
- **LIFO order:** Nested regions deallocate in reverse order
- **O(1) free:** Entire region freed at once (vs. O(n) individual frees)

**Example:**
```cantrip
procedure parse_file(path: String): Result<Data, Error>
    uses alloc.region, io.read;
{
    region temp {
        let contents = String.new_in<temp>();  // Temporary buffer
        std.fs.read_to_string(path, mut contents)?;
        
        let parsed = parse_json(contents)?;    // Parsing uses temp region
        
        parsed.to_heap()  // Convert to heap before return
    }  // All temp strings freed in O(1)
}
```

**Region Rules:**
```
[Region-Escape]
Γ ⊢ region r { e }    e : T    alloc(e, r)    e escapes r
──────────────────────────────────────────────────────────
ERROR: Cannot return region-allocated value

[Region-LIFO]
r₂ nested in r₁
──────────────────────────────────────
dealloc(r₂) happens-before dealloc(r₁)
```

### 5.3 Memory Layouts

**Primitives:**
```
i8, u8, bool:  1 byte,  align 1
i16, u16:      2 bytes, align 2
i32, u32, f32: 4 bytes, align 4
i64, u64, f64: 8 bytes, align 8
char:          4 bytes, align 4
```

**String (fat pointer):**
```
String { ptr: *u8, len: usize, cap: usize }
┌────────────┬────────────┬────────────┐
│ ptr        │ len        │ cap        │
│ 8 bytes    │ 8 bytes    │ 8 bytes    │
└────────────┴────────────┴────────────┘
Total: 24 bytes (64-bit), alignment: 8
```

**Array (contiguous):**
```
[T; n] = n × sizeof(T) bytes
[i32; 5] = 5 × 4 = 20 bytes
```

**Tuple (with padding):**
```
(u8, u32, u16)
┌──┬───────┬────────┬────┬──┐
│u8│ pad   │  u32   │u16 │p │
└──┴───────┴────────┴────┴──┘
Total: 12 bytes (aligned to 4)
```

**Record (with padding):**
```
record Example { a: u8; b: u32; c: u16; }
┌──┬───────┬────────┬────┬──┐
│a │ pad   │   b    │ c  │p │
└──┴───────┴────────┴────┴──┘
Total: 12 bytes, alignment: 4
```

**Enum (tagged union):**
```
enum Message {
    Quit,                    // 0 bytes payload
    Move { x: i32, y: i32 }, // 8 bytes payload
    Write(String),           // 24 bytes payload (largest)
}

Layout:
┌──────────┬────────────────────────┐
│ tag (8B) │ payload (24B, largest) │
└──────────┴────────────────────────┘
Total: 32 bytes (64-bit)
```

### 5.4 Representation Attributes

```cantrip
#[repr(C)]        // C-compatible layout (no reordering)
record Point { x: f32; y: f32; }

#[repr(packed)]   // No padding (unaligned access risk)
record Header { magic: u32; version: u16; }

#[repr(align(64))] // Cache line alignment
record CacheLine { data: [u8; 64]; }
```

---

## 6. Syntax Quick Reference

### 6.1 Lexical Elements

**Comments:**
```cantrip
// Line comment
/* Block comment */
/// Doc comment for item
//! Module-level doc
```

**Identifiers:**
```
Pattern: [a-zA-Z_][a-zA-Z0-9_]*
Case-sensitive: Variable ≠ variable
Conventions: snake_case (vars), CamelCase (types), SCREAMING_SNAKE (consts)
```

**Literals:**
```cantrip
// Integer (default i32)
42, 0xFF, 0b1010, 0o755, 1_000_000
42i8, 100u64  // Type suffix

// Float (default f64)
3.14, 1.0e10, 2.5e-3
3.14f32  // Type suffix

// Boolean
true, false

// Character (Unicode scalar)
'a', '\n', '\u{1F600}'

// String (UTF-8)
"hello", "line 1\nline 2", "unicode: 🚀"
```

**Keywords (54 reserved):**
```
abstract, as, async, await, break, by, case, comptime, const, continue,
defer, effect, else, enum, exists, false, forall,
if, import, internal, invariant, iso, let, loop,
match, modal, module, move, must, mut, new, none, own, private,
procedure, protected, public, record, ref, region, result,
select, self, Self, state, static, trait, true, type, uses, var, where, will, with
```

### 6.2 Type Syntax

```ebnf
Type ::= PrimitiveType | CompoundType | NamedType | PermissionType

PrimitiveType ::= "i8" | "i16" | "i32" | "i64" | "isize"
                | "u8" | "u16" | "u32" | "u64" | "usize"
                | "f32" | "f64" | "bool" | "char"
                | "str" | "String" | "!"

CompoundType ::= "[" Type ";" IntLiteral "]"    // Array
               | "[" Type "]"                    // Slice
               | "(" Type ("," Type)* ")"        // Tuple
               | "()"                            // Unit

NamedType ::= Ident ("<" TypeArgs ">")?         // Generic

PermissionType ::= "own" Type | "mut" Type | "iso" Type
```

### 6.3 Expression Syntax

```ebnf
Expr ::= Literal | Ident | "(" Expr ")"
       | Expr BinOp Expr | UnOp Expr
       | Expr "=>" Expr                         // Pipeline
       | "if" Expr "then" Expr "else" Expr
       | "loop" LoopHead? LoopVerif? Block      // Loop (unified)
       | "match" Expr "{" MatchArm+ "}"
       | "let" Ident "=" Expr "in" Expr
       | "var" Ident "=" Expr "in" Expr
       | FunctionCall | MethodCall
       | FieldAccess | ArrayIndex
       | "{" Expr (";" Expr)* "}"               // Block
       | "move" Expr                            // Ownership transfer
       | "region" Ident "{" Expr "}"            // Region

LoopHead ::= Pattern "in" Expr                  // For-style
           | Expr                               // While-style (no "in")

LoopVerif ::= ("by" Expr)? ("with" Predicates ";")?  // Variant & invariants

BinOp ::= "+" | "-" | "*" | "/" | "%" | "**"    // Arithmetic
        | "==" | "!=" | "<" | "<=" | ">" | ">=" // Comparison
        | "&&" | "||"                           // Logical
        | "&" | "|" | "^" | "<<" | ">>"         // Bitwise
        | ".."  | "..="                         // Range

UnOp ::= "-" | "!" | "*" | "&" | "mut"
```

### 6.4 Loop Syntax (Unified)

Cantrip has ONE loop construct (`loop`) that handles all iteration patterns.

**Basic forms:**
```cantrip
// Infinite loop
loop { body }

// While-style (condition)
loop condition { body }

// For-style (range)
loop i in 0..n { body }

// Iterator-style
loop item in collection { body }
```

**With verification (optional):**
```cantrip
// Termination metric (variant)
loop condition by variant { body }

// Loop invariants
loop condition
    with invariant1, invariant2;
{ body }

// Both
loop i in 0..n by n - i
    with {
        0 <= i,
        i <= n,
        sum == sum_of(arr[0..i]),
    };
{ body }
```

**Control flow:**
```cantrip
break;           // Exit loop (returns ())
break value;     // Exit with value
continue;        // Next iteration

// With labels
'outer: loop {
    'inner: loop {
        break 'outer;     // Break outer loop
        continue 'inner;  // Continue inner
    }
}
```

**Key rules:**
- `by expr` specifies decreasing termination metric
- `with { inv1, inv2, ... };` specifies loop invariants
- Pattern with `in` → for-style; expression without `in` → while-style
- Loop type = type of break expressions (or `Never` if no breaks)

### 6.5 Pattern Syntax

```ebnf
Pattern ::= "_"                                 // Wildcard
          | Ident                               // Variable binding
          | Literal                             // Literal match
          | "(" Pattern ("," Pattern)* ")"      // Tuple
          | Ident "." Ident                     // Unit variant
          | Ident "." Ident "(" Pattern ")"     // Tuple variant
          | Ident "." Ident "{" FieldPat "}"    // Record variant
          | Pattern "if" Expr                   // Guard
```

### 6.5 Declaration Syntax

```cantrip
// Function
procedure name<T>(param: T): U
    needs effect1, effect2;
    requires precondition;
    ensures postcondition;
{ body }

// Record
record Name<T> {
    field1: Type1;
    field2: Type2;

    procedure method(self: Self, param: T): U { ... }
}

// Enum
enum Name<T> {
    Variant1,
    Variant2(T),
    Variant3 { field: Type },
}

// Trait
trait Name<T> {
    procedure method(self: Self, param: T): U;
}

// Record implementing trait
record Type: Name<T> {
    // fields

    procedure method(self: Self, param: T): U { ... }
}

// Modal
modal Name<T> {
    @State1 { field: Type }
    @State2 { field: Type }
    
    @State1 -> transition() -> @State2;
}
```

### 6.6 Attributes

```cantrip
#[repr(C)]               // C-compatible layout
#[repr(packed)]          // No padding
#[repr(align(n))]        // Explicit alignment

#[verify(static)]        // Compile-time verification
#[verify(runtime)]       // Runtime checks
#[verify(none)]          // Documentation only

#[overflow_checks(bool)] // Integer overflow behavior

#[module(key = "val")]   // Module metadata

#[alias("name1", "name2")] // Alternative names (tooling)

#[inline]                // Inlining hint
#[no_inline]             // Prevent inlining
```

---

## 7. Formal Notation

### 7.1 Metavariables

```
x, y, z ∈ Var        (term variables)
f, g, h ∈ FunName    (function names)
m, n, o ∈ ProcName   (procedure names)
T, U, V ∈ Type       (types)
e, e₁, e₂ ∈ Expr     (expressions)
v, v₁, v₂ ∈ Value    (values)
p, p₁, p₂ ∈ Pattern  (patterns)
ε, ε₁, ε₂ ∈ Effect   (effects)
P, Q, R ∈ Assertion  (contract assertions)
σ, σ', σ'' ∈ Store   (memory stores)
Γ, Δ, Θ ∈ Context    (type contexts)
@S, @S' ∈ State      (modal states)
n, m, k ∈ ℕ          (natural numbers)
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

### 7.3 Operators

**Set theory:**
```
∈  (element of)      x ∈ S
⊆  (subset)          A ⊆ B
∪  (union)           A ∪ B
∩  (intersection)    A ∩ B
∅  (empty set)       ε = ∅
×  (product)         A × B
```

**Logic:**
```
∧  (and)        P ∧ Q
∨  (or)         P ∨ Q
¬  (not)        ¬P
⇒  (implies)    P ⇒ Q
⟺  (iff)        P ⟺ Q
∀  (forall)     ∀x. P
∃  (exists)     ∃x. P
```

**Relations:**
```
→  (maps to / reduces to)
⇒  (implies)
⇓  (evaluates to)
≡  (equivalent)
⊢  (entails / proves)
⊨  (satisfies / models)
<: (subtype)
```

### 7.4 Inference Rule Format

```
[Rule-Name]  (descriptive name)
premise₁    premise₂    ...    premiseₙ
─────────────────────────────────────────
conclusion
```

**Reading:** If all premises hold, then conclusion holds.

**Example:**
```
[T-Add]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T ∈ NumericTypes
──────────────────────────────────────────────
Γ ⊢ e₁ + e₂ : T
```

### 7.5 Common Evaluation Rules

```
[E-Var] Variable lookup
────────────────────
⟨x, σ[x↦v]⟩ ⇓ ⟨v, σ⟩

[E-Add] Addition
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨n₁ + n₂, σ₂⟩

[E-App] Function application
⟨f, σ⟩ ⇓ ⟨λx.e, σ₁⟩
⟨arg, σ₁⟩ ⇓ ⟨v, σ₂⟩
⟨e[x ↦ v], σ₂⟩ ⇓ ⟨w, σ₃⟩
─────────────────────────────────────
⟨f(arg), σ⟩ ⇓ ⟨w, σ₃⟩

[E-Let] Let binding
⟨e₁, σ⟩ ⇓ ⟨v, σ₁⟩
⟨e₂[x ↦ v], σ₁⟩ ⇓ ⟨w, σ₂⟩
─────────────────────────────────────
⟨let x = e₁ in e₂, σ⟩ ⇓ ⟨w, σ₂⟩

[E-If-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩
⟨e₂, σ₁⟩ ⇓ ⟨v, σ₂⟩
─────────────────────────────────────
⟨if e₁ then e₂ else e₃, σ⟩ ⇓ ⟨v, σ₂⟩

[E-If-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩
⟨e₃, σ₁⟩ ⇓ ⟨v, σ₂⟩
─────────────────────────────────────
⟨if e₁ then e₂ else e₃, σ⟩ ⇓ ⟨v, σ₂⟩
```

---

## 8. Section Structure Template

### 8.1 Required Components (MUST appear in this order)

```markdown
# Part X: PartName - §N. Feature Name

**Section**: §N | **Part**: PartName (Part X)
**Previous**: [PrevSection](link.md) | **Next**: [NextSection](link.md)

---

**Definition N.1 (Feature):** [Formal mathematical definition in 1-2 sentences]

## N. Feature Name

### N.1 Overview

**Key innovation/purpose:** [One sentence explaining why this feature exists]

**When to use [feature]:**
- Use case 1 (when this is appropriate)
- Use case 2
- Use case 3

**When NOT to use [feature]:**
- Alternative 1 → use X instead
- Alternative 2 → use Y instead

**Relationship to other features:** [Cross-references to related sections]

### N.2 Syntax

#### N.2.1 Concrete Syntax

**Grammar:**
```ebnf
[EBNF grammar rules]
```

**Syntax:**
```cantrip
[Cantrip code examples showing syntax]
```

#### N.2.2 Abstract Syntax

**Formal representation:**
```
[Mathematical notation for abstract syntax]
```

**Components:**
- Component 1: Description
- Component 2: Description

#### N.2.3 Basic Examples

```cantrip
[Simple, clear examples demonstrating basic usage]
```

**Explanation:** [What these examples show]

### N.3 Static Semantics

#### N.3.1 Well-Formedness Rules

**Definition N.2 (Well-Formedness):** [What it means to be well-formed]

```
[WF-Rule-Name]
premises about context and components
─────────────────────────────────────
conclusion: construct is well-formed
```

#### N.3.2 Type Rules

**[T-Rule-Name] Description:**
```
[T-Rule-Name]
premises: type judgments for components
─────────────────────────────────────
conclusion: type judgment for whole
```

**Explanation:** [What this rule establishes]

```cantrip
[Code example illustrating this type rule]
```

**[Additional type rules as needed, minimum 5-7 for new types]**

#### N.3.3 Type Properties

**Theorem N.1 (Property Name):** [Statement of property]

**Proof sketch:** [Informal proof or proof outline]

**Corollary N.1:** [Derived result]

### N.4 Dynamic Semantics

#### N.4.1 Evaluation Rules

**[E-Rule-Name] Description:**
```
[E-Rule-Name]
evaluation premises
⟨expression, state⟩ ⇓ ⟨value, state'⟩
─────────────────────────────────────
evaluation conclusion
```

**Explanation:** [What happens at runtime]

#### N.4.2 Memory Representation

**Layout:**
```
[Description/diagram of memory layout]
```

**Size:** [Formula or concrete value]
**Alignment:** [Formula or concrete value]

#### N.4.3 Operational Behavior

[Description of runtime behavior, evaluation order, side effects]

### N.5 Additional Properties (OPTIONAL - include if needed)

#### N.5.1 Invariants

**Invariant N.1:** [Statement of invariant that always holds]

#### N.5.2 Verification

[Verification conditions if relevant to feature]

#### N.5.3 Algorithms

**Algorithm N.1 (Name):**
```
[Pseudocode or description of algorithm]
```

### N.6 Advanced Features (OPTIONAL - include if applicable)

#### N.6.1 [Advanced Feature 1]

[Syntax, semantics, examples for derived/advanced forms]

#### N.6.2 [Advanced Feature 2]

[Continue as needed]

### N.7 Examples and Patterns (OPTIONAL but RECOMMENDED)

#### N.7.1 Pattern Name 1

**Pattern:** [Description of pattern]

```cantrip
[Complete, realistic example]
```

**Explanation:** [When and why to use this pattern]

#### N.7.2 Pattern Name 2

[2-4 patterns maximum, keep concise]

---

**Previous**: [PrevSection](link.md) | **Next**: [NextSection](link.md)
```

### 8.2 Numbering Scheme

- **Sections:** §1, §2, §3, ...
- **Subsections:** §N.1, §N.2, §N.3, ...
- **Sub-subsections:** §N.3.1, §N.3.2, ...
- **Definitions:** Definition N.X
- **Theorems:** Theorem N.X
- **Corollaries:** Corollary N.X
- **Rules:** [Category-Name] where:
  - Category = WF (well-formed), T (type), E (evaluation), Sub (subtyping)
  - Name = descriptive identifier

### 8.3 Rule Naming Conventions

```
[WF-Feature]       Well-formedness rules
[T-Feature-Op]     Type rules
[E-Feature-Op]     Evaluation rules
[Sub-Feature]      Subtyping rules
```

**Examples:**
```
[WF-Record]        Record well-formedness
[T-Record-Field]   Record field access typing
[E-Record-Cons]    Record construction evaluation
[Sub-Permission]   Permission subtyping
```

---

## 9. Extension Guidelines

### 9.1 Adding New Types

**Follow String (§5.6) as Exemplar:**

String is the most complete type specification (91,766 characters, 3,143 lines). Use it as template for new types.

**Required sections (15+ type rules minimum):**
1. ✓ Definition (formal statement)
2. ✓ Overview (purpose, when to use, when NOT to use)
3. ✓ Syntax (concrete EBNF + abstract math + examples)
4. ✓ Static Semantics (well-formedness + 15+ type rules + properties)
5. ✓ Dynamic Semantics (evaluation rules + memory layout + behavior)
6. ✓ Additional Properties (invariants, verification, algorithms)
7. ✓ Advanced Features (permission integration, region integration)
8. ✓ Examples and Patterns (2-4 practical patterns)

**Type rules to include:**
```
[WF-TypeName]          Well-formedness
[T-TypeName-Literal]   Literal construction
[T-TypeName-New]       Constructor function
[T-TypeName-Method]    Method invocation
[T-TypeName-Field]     Field access (if applicable)
[T-TypeName-Index]     Indexing (if applicable)
[T-TypeName-Move]      Ownership transfer
[Sub-TypeName]         Subtyping rules
... (5-10+ more depending on complexity)
```

### 9.2 Integration Checklist

When adding new features, ensure integration with:

**✓ Permission System:**
- [ ] Define behavior with `own`, `mut`, immutable
- [ ] Specify Copy vs Move semantics
- [ ] Document permission transitions
- [ ] Provide examples for each permission mode

**✓ Contract System (with Effects):**
- [ ] Identify all effects operations perform
- [ ] Specify `needs` clauses for all operations
- [ ] Define preconditions where applicable
- [ ] Define postconditions where applicable

**✓ Memory Management:**
- [ ] Specify memory layout (size, alignment, padding)
- [ ] Provide region allocation methods (`new_in<r>()`)
- [ ] Document heap vs region allocation trade-offs
- [ ] Define destruction semantics (RAII)

**✓ Formal Semantics:**
- [ ] Write well-formedness rules
- [ ] Write type rules (15+ for types)
- [ ] Write evaluation rules
- [ ] State and prove key properties (theorems)

**✓ Cross-References:**
- [ ] Link to §1 (Mathematical Foundations) for notation
- [ ] Link to Part III (Permission System)
- [ ] Link to Part IV (Contracts and Clauses)
- [ ] Link to Part VI (Memory Regions)
- [ ] Link to related type sections

### 9.3 Writing Type Rules

**Pattern:**
```
[T-Feature-Operation]  (descriptive name)
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ    (premises: type judgments)
additional_constraint₁
additional_constraint₂
─────────────────────────────────────────────────
Γ ⊢ operation(e₁, ..., eₙ) : U    (conclusion)
```

**Example:**
```
[T-Vec-Push]
Γ ⊢ vec : own Vec<T>
Γ ⊢ item : T
─────────────────────────────────────
Γ ⊢ vec.push(item) : () ! alloc.heap
```

### 9.4 Writing Evaluation Rules

**Pattern:**
```
[E-Feature-Operation]  (descriptive name)
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩
...
⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
additional_constraints
─────────────────────────────────────
⟨operation(e₁, ..., eₙ), σ⟩ ⇓ ⟨result, σ'⟩
```

**Example:**
```
[E-Vec-Push]
⟨vec, σ⟩ ⇓ ⟨Vec{ptr: p, len: l, cap: c}, σ₁⟩
⟨item, σ₁⟩ ⇓ ⟨v, σ₂⟩
l < c    (capacity check)
─────────────────────────────────────────────────
⟨vec.push(item), σ⟩ ⇓ ⟨(), σ₂[p+l ↦ v, vec.len ↦ l+1]⟩
```

### 9.5 Common Pitfalls

**❌ Don't:**
- Skip formal definitions (every section needs Definition N.1)
- Provide only examples without formal rules
- Ignore permission/effect/region integration
- Forget to specify memory layout
- Omit cross-references to notation (§1)

**✓ Do:**
- Start with formal definition
- Provide 15+ type rules for new types
- Include evaluation rules with state transitions
- Specify exact memory layout (bytes, alignment)
- Cross-reference all notation from §1
- Use inference rule format consistently
- Provide both formal rules AND examples
- State theorems with proof sketches

---

## 10. Quick Lookup Tables

### 10.1 Type Sizes (64-bit Platform)

| Type | Size (bytes) | Alignment | Notes |
|------|--------------|-----------|-------|
| `i8`, `u8`, `bool` | 1 | 1 | Single byte |
| `i16`, `u16` | 2 | 2 | Two bytes |
| `i32`, `u32`, `f32` | 4 | 4 | Four bytes |
| `i64`, `u64`, `f64`, `isize`, `usize` | 8 | 8 | Eight bytes |
| `char` | 4 | 4 | Unicode scalar |
| `()` (unit) | 0 | 1 | Zero-sized |
| `!` (never) | 0 | 1 | Uninhabited |
| `String` | 24 | 8 | Fat pointer (ptr+len+cap) |
| `[T; n]` | `n × size(T)` | `align(T)` | Contiguous array |
| `[T]` | 16 | 8 | Fat pointer (ptr+len) |
| `(T, U)` | varies | `max(align(T), align(U))` | With padding |

### 10.2 Reserved Keywords (55 Total)

```
abstract    as          async       await       break
case        comptime    const       continue    defer
effect      else        ensures     enum        exists
false       for         forall      procedure    if
impl        import      internal    invariant   iso
let         loop        match       modal       module
move        mut         needs       new         none
own         private     procedure   protected   public
record      ref         region      requires    result
select      self        Self        state       static
trait       true        type        var         where
while
```

### 10.3 Operator Precedence (15 Levels, High to Low)

| Level | Operators | Associativity | Example |
|-------|-----------|---------------|---------|
| 1 | `.` `[]` `()` | Left | `obj.field`, `arr[i]`, `f(x)` |
| 2 | `!` `-` `*` `&` `mut` | Right | `!x`, `-y`, `*ptr`, `&ref` |
| 3 | `**` | Right | `x ** y` |
| 4 | `*` `/` `%` | Left | `x * y`, `x / y`, `x % y` |
| 5 | `+` `-` | Left | `x + y`, `x - y` |
| 6 | `<<` `>>` | Left | `x << 2`, `x >> 3` |
| 7 | `&` | Left | `x & y` |
| 8 | `^` | Left | `x ^ y` |
| 9 | `\|` | Left | `x \| y` |
| 10 | `<` `<=` `>` `>=` `==` `!=` | Left | `x < y`, `x == y` |
| 11 | `&&` | Left | `x && y` |
| 12 | `\|\|` | Left | `x \|\| y` |
| 13 | `..` `..=` | Left | `0..10`, `0..=10` |
| 14 | `=>` | Left | `x => f` |
| 15 | `=` `+=` `-=` `*=` `/=` etc. | Right | `x = y`, `x += 1` |

### 10.4 Error Code Ranges

| Range | Category | Examples |
|-------|----------|----------|
| E2xxx | Lexical errors | E2001: Invalid token, E2002: Invalid literal |
| E3xxx | Permission errors | E3002: Cannot modify immutable, E3004: Use of moved value |
| E4xxx | Attribute errors | E4001: Conflicting attributes, E4002: Unknown attribute |
| E5xxx | Type errors | E5101: Type mismatch, E5102: Inference failure |
| E8xxx | Region errors | E8301: Cannot escape region, E8302: Lifetime violation |
| E9xxx | Contract errors | E9001: Effect violation, E9002: Precondition failed |

### 10.5 Standard Library Core Types

```cantrip
// Option type
enum Option<T> {
    Some(T),
    None,
}

// Result type
enum Result<T, E> {
    Ok(T),
    Err(E),
}

// String type
record String {
    ptr: *u8;
    len: usize;
    cap: usize;
}

// Vector type
record Vec<T> {
    ptr: *T;
    len: usize;
    cap: usize;
}

// HashMap type
record HashMap<K, V> {
    // Implementation details
}
```

### 10.6 Common Type Traits

```cantrip
// Copy trait (implicit bitwise copy)
trait Copy { }

// Clone trait (explicit deep copy)
trait Clone {
    procedure clone(self: Self): Self;
}

// Debug trait (debug formatting)
trait Debug {
    procedure debug(self: Self): String;
}

// PartialEq trait (equality comparison)
trait PartialEq {
    procedure eq(self: Self, other: Self): bool;
}

// PartialOrd trait (ordering comparison)
trait PartialOrd {
    procedure cmp(self: Self, other: Self): Ordering;
}
```

### 10.7 Effect Quick Reference

| Effect | Meaning | Example |
|--------|---------|---------|
| `alloc.heap` | Heap allocation | `String.new()` |
| `alloc.region` | Region allocation | `String.new_in<r>()` |
| `io.read` | Read I/O | `stdin.read_line()` |
| `io.write` | Write I/O | `println("text")` |
| `fs.read` | File read | `std.fs.read_to_string()` |
| `fs.write` | File write | `std.fs.write()` |
| `net.read` | Network read | `socket.recv()` |
| `net.write` | Network write | `socket.send()` |
| `time.read` | Read time | `DateTime.now()` |
| `thread.spawn` | Spawn thread | `thread.spawn(\|\| { ... })` |
| `panic` | May panic | `unwrap()`, `expect()` |
| `random` | Non-determinism | `rand.gen()` |

---

## 11. Summary

This document provides:

✓ **Complete language semantics** — Types, permissions, contracts with effects, memory model
✓ **Formal notation** — Metavariables, judgment forms, inference rules
✓ **Practical syntax** — EBNF grammars, code examples, common patterns
✓ **Design philosophy** — Explicit vs implicit, safety trade-offs, comparison to other languages
✓ **Extension templates** — Standard structure, type rules, evaluation rules
✓ **Quick reference** — Type sizes, keywords, operators, error codes

**Key Points:**
1. **Effects are part of contracts**, not a separate system
2. **Multiple mutable references allowed** (unlike Rust, like Cyclone)
3. **Regions control lifetimes**, not lifetime parameters
4. **Follow String (§5.6) as exemplar** for new type specifications
5. **15+ type rules minimum** for new types
6. **Always integrate**: permissions, effects, regions, formal semantics

**Usage:** Reference this document when:
- Writing new specification sections
- Understanding existing semantics
- Extending the type system
- Implementing compiler/tooling
- Verifying specification completeness

**Updates:** This document should be updated whenever:
- New language features are added
- Specification structure changes
- Key design decisions are revised
- Error code ranges are modified

---

**Document Version:** 1.0.2  
**Last Updated:** 2025-10-23  
**Maintainer:** Cantrip Language Team  
**License:** Same as Cantrip Language Specification
