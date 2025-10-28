# Part II: Type System - §7. Tuples

**Section**: §7 | **Part**: Type System (Part II)
**Previous**: [Arrays and Slices](02_ArraysAndSlices.md) | **Next**: [Records](04_Records.md)

---

**Definition 7.1 (Tuple):** A tuple is an anonymous product type containing a fixed number of heterogeneous elements: `(T₁, ..., Tₙ) = T₁ × T₂ × ... × Tₙ`. Tuple types and literals use the same syntax `(T₁, ..., Tₙ)` and `(e₁, ..., eₙ)`, with the unit type `()` as the zero-element tuple.

## 7. Tuples

### 7.1 Overview

**Key innovation/purpose:** Tuples provide lightweight, anonymous product types for temporary grouping of heterogeneous values without the ceremony of defining a record type. They are essential for multiple return values and destructuring patterns.

**When to use tuples:**
- Multiple return values from functions
- Temporary grouping of related but heterogeneous data
- Destructuring in pattern matching
- Coordinate pairs or triples (e.g., `(x, y)`, `(r, g, b)`)
- Function arguments that naturally group together

**When NOT to use tuples:**
- Complex data structures with many fields → use records (§8)
- Data that benefits from named fields → use records
- Homogeneous collections → use arrays (§6)
- Long-lived data structures (tuples lack semantic meaning)

**Relationship to other features:**
- **Records**: Tuples are anonymous records with positional access (`.0`, `.1`) instead of named fields
- **Pattern matching**: Tuples support destructuring patterns
- **Type inference**: Tuple types are inferred from their elements
- **Unit type**: The 0-tuple `()` represents "no value" and is the return type of procedures

### 7.2 Syntax

#### 7.2.1 Concrete Syntax

**Concrete Syntax:**
```ebnf
TupleType    ::= "(" Type ("," Type)* ")"
               | "(" ")"                     // Unit type
TupleLiteral ::= "(" Expr ("," Expr)* ")"
               | "(" ")"                     // Unit value
TupleProj    ::= Expr "." IntLiteral
TuplePattern ::= "(" Pattern ("," Pattern)* ")"
```

**Examples:**
```cantrip
// Tuple types and literals
let pair: (i32, str) = (42, "answer")
let triple: (f64, f64, f64) = (1.0, 2.0, 3.0)
let nested: ((i32, i32), str) = ((10, 20), "point")

// Tuple projection (field access by index)
let first = pair.0          // 42
let second = pair.1         // "answer"

// Tuple destructuring
let (x, y) = pair
let ((a, b), label) = nested
```

**Unit type (0-tuple):**
```cantrip
let unit: () = ()  // Type and value both written as ()

function print_message(msg: str): () {
    std.io.println(msg)
    // Implicit return of ()
}
```

The unit type `()` is a special case of tuples with zero elements. It represents "no value" and is used for:
- Functions that perform side effects but return nothing meaningful
- Empty states in enums
- Placeholder in generic contexts

#### 7.2.2 Abstract Syntax

**Tuples:**
```
τ ::= (τ₁, ..., τₙ)  (tuple type)
    | ()             (unit type)

e ::= (e₁, ..., eₙ)  (tuple literal)
    | e.i            (tuple projection)
    | ()             (unit value)

p ::= (p₁, ..., pₙ)  (tuple pattern)
```

#### 7.2.3 Basic Examples

**Multiple return values:**
```cantrip
function divide_with_remainder(x: i32, y: i32): (i32, i32)
    must y != 0
{
    (x / y, x % y)
}

let (quotient, remainder) = divide_with_remainder(17, 5)
// quotient = 3, remainder = 2
```

**Coordinate systems:**
```cantrip
// 2D point
let point: (f64, f64) = (3.0, 4.0)
let distance = ((point.0 * point.0) + (point.1 * point.1)).sqrt()

// RGB color
let color: (u8, u8, u8) = (255, 128, 0)
let (r, g, b) = color
```

### 7.3 Static Semantics

#### 7.3.1 Well-Formedness Rules

**Tuple type well-formedness:**
```
[WF-Tuple]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
Γ ⊢ (T₁, ..., Tₙ) : Type

[WF-Unit]
────────────────
Γ ⊢ () : Type
```

**Explanation:** A tuple type is well-formed if all its component types are well-formed. The unit type `()` is always well-formed.

#### 7.3.2 Type Rules

**Tuple literal typing:**
```
[T-Tuple]
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────
Γ ⊢ (e₁, ..., eₙ) : (T₁, ..., Tₙ)
```

**Explanation:** A tuple literal has tuple type when each element expression has the corresponding component type.

**Example:**
```cantrip
let x: i32 = 42
let y: str = "hello"
let pair = (x, y)  // Type: (i32, str)
```

**Tuple projection typing:**
```
[T-Tuple-Proj]
Γ ⊢ e : (T₁, ..., Tₙ)
i ∈ [0, n)
───────────────────────────────────────
Γ ⊢ e.i : Tᵢ
```

**Explanation:** Accessing the i-th field of a tuple (0-indexed) yields the type of that field.

**Example:**
```cantrip
let pair: (i32, str) = (42, "answer")
let first = pair.0   // Type: i32
let second = pair.1  // Type: str
```

**Unit type:**
```
[T-Unit]
────────────────
Γ ⊢ () : ()
```

**Explanation:** The unit value `()` has unit type `()`.

**Tuple pattern typing:**
```
[T-Tuple-Pattern]
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────────
Γ ⊢ (p₁, ..., pₙ) : (T₁, ..., Tₙ) ⇝ Γ₁ ∪ ... ∪ Γₙ
```

**Explanation:** A tuple pattern matches a tuple type when each sub-pattern matches the corresponding component type. The resulting context is the union of all sub-pattern contexts.

**Example:**
```cantrip
let (x, y) = (42, "hello")
// Pattern (x, y) : (i32, str) ⇝ {x: i32, y: str}
```

#### 7.3.3 Type Properties

**Property 7.1 (Tuple Type Uniqueness):**

For any expression `(e₁, ..., eₙ)`, if it is well-typed, it has exactly one type:
```
∀e₁...eₙ. (∃T. Γ ⊢ (e₁, ..., eₙ) : T) ⇒ (∃!T₁...Tₙ. T = (T₁, ..., Tₙ))
```

**Property 7.2 (Tuple Type Decomposition):**

A tuple type determines the types of its components:
```
Γ ⊢ e : (T₁, ..., Tₙ) ⇒ ∀i ∈ [0, n). Γ ⊢ e.i : Tᵢ
```

**Type Inference:**

Tuple types are inferred component-wise without requiring unification. Each element can have a different type, determined independently:

```cantrip
let tuple = (42, "hello", 3.14)  // Inferred: (i32, str, f64)
let nested = ((1, 2), (3, 4))    // Inferred: ((i32, i32), (i32, i32))
```

**Contextual Type Inference:**

When a tuple appears in a context expecting a specific tuple type, that type information flows to the elements:

```cantrip
let pair: (u32, f32) = (10, 3.5) // 10 inferred as u32, 3.5 as f32

function process(data: (i64, bool)) { ... }
process((100, true))  // 100 inferred as i64
```

### 7.4 Dynamic Semantics

#### 7.4.1 Evaluation Rules

**Tuple evaluation:**
```
[E-Tuple]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
─────────────────────────────────────────────────────
⟨(e₁, ..., eₙ), σ⟩ ⇓ ⟨(v₁, ..., vₙ), σₙ⟩

[E-Unit]
────────────────
⟨(), σ⟩ ⇓ ⟨(), σ⟩
```

**Tuple projection evaluation:**
```
[E-Tuple-Proj]
⟨e, σ⟩ ⇓ ⟨(v₀, ..., vₙ₋₁), σ'⟩
i < n
─────────────────────────────────────
⟨e.i, σ⟩ ⇓ ⟨vᵢ, σ'⟩
```

**Formal semantics:**
```
⟦(v₁, ..., vₙ)⟧ = (⟦v₁⟧, ..., ⟦vₙ⟧)  (tuple value)
⟦(v₁, ..., vₙ).i⟧ = ⟦vᵢ⟧              (projection)
```

#### 7.4.2 Memory Representation

**Tuple layout:**
```
(T₁, T₂, T₃) memory layout:
┌────┬───┬────┬───┬────┐
│ T₁ │pad│ T₂ │pad│ T₃ │
└────┴───┴────┴───┴────┘
```

**Properties:**
- Each field aligned to its type's alignment
- Padding inserted between fields to maintain alignment
- Total size rounded up to tuple's alignment
- Fields ordered as declared (no reordering optimization)

**Alignment calculation:**
```
align((T₁, ..., Tₙ)) = max(align(T₁), ..., align(Tₙ))
size((T₁, ..., Tₙ)) = aligned_sum(size(T₁), ..., size(Tₙ))
```

**Examples:**
```cantrip
// (u8, u32, u16) on typical 64-bit system
// align(u8) = 1, align(u32) = 4, align(u16) = 2
// Layout:
// Offset:  0  1  2  3  4  5  6  7
//         ┌──┬──┬──┬──┬────┬────┬──┬──┐
//         │u8│p │p │p │u32 │u16 │p │p │
//         └──┴──┴──┴──┴────┴────┴──┴──┘
// Total: 8 bytes (rounded to alignment of 4)

// (f64, u8) on 64-bit system
// Offset:  0              8  9
//         ┌──────────────┬──┬──┬──┬──┬──┬──┬──┐
//         │     f64      │u8│p │p │p │p │p │p │p │
//         └──────────────┴──┴──┴──┴──┴──┴──┴──┘
// Total: 16 bytes (f64 must 8-byte alignment)
```

**Unit type:**
```
() memory layout:
(no memory allocated, zero-sized type)
```

The unit type has size 0 and alignment 1.

#### 7.4.3 Operational Behavior

**Construction and access:**

Tuple operations are zero-cost abstractions with predictable evaluation:

- **Construction**: Tuple elements are evaluated left-to-right, with each element fully evaluated before the next begins
- **Field access**: Access via `.i` is a compile-time offset calculation with zero runtime overhead
- **Pattern matching**: Tuple destructuring is optimized away by the compiler when possible

**Copy Capability:**

```
Tuples implement Copy ⟺ all fields implement Copy
```

**Parameter Passing:**
- All tuples pass by permission by default (like all types)
- `.copy()` available for Copy-capable tuples
- Non-Copy tuples require explicit `move` for ownership transfer
- **Partial moves**: Not allowed—tuples move as a complete unit

**Example:**
```cantrip
// Copy-capable tuple
let point: (i32, i32) = (10, 20)
procedure distance(p: (i32, i32)): f64 { ... }
distance(point)  // Permission, point still usable

let copy = point.copy()  // Explicit copy if needed

// Non-Copy tuple
let record: (String, i32) = (String.new("test"), 42)
process(record)  // Permission
consume(move record)  // Explicit move
// record no longer usable
```

**Pattern matching optimization:**

The compiler optimizes tuple construction and destructuring:

```cantrip
// Intermediate tuple construction may be eliminated
let (x, y) = compute_pair()  // Compiler may use two separate variables

// Swap without temporary allocation
let (a, b) = (b, a)  // Optimized to register swaps
```

**Performance characteristics:**

```
Tuple construction:  O(sum of field construction costs)
Field access:        O(1), compile-time offset
Pattern matching:    O(1), zero-cost destructuring
Memory overhead:     Padding for alignment (no discriminant tag)
```

### 7.5 Additional Properties

#### 7.5.1 Parameter Passing and Copy Capability

**Tuples:**
- `(T₁, ..., Tₙ)` implements Copy if all Tᵢ implement Copy
- All tuples pass by permission regardless of Copy capability
- Explicit `.copy()` for Copy-capable tuples
- Explicit `move` for ownership transfer

```cantrip
let pair1: (i32, f64) = (42, 3.14)
procedure process(p: (i32, f64)) { ... }
process(pair1)  // Permission, pair1 still usable

let pair2 = pair1.copy()  // Explicit copy

let owned: (String, Vec<i32>) = (String.new(), Vec.new())
process_owned(owned)  // Permission, owned still usable
consume(move owned)  // Explicit move
// owned is now unusable
```

#### 7.5.2 Subtyping

Tuples are **invariant** in their type parameters:

```
(T₁, T₂) is NOT a subtype of (U₁, U₂) even if T₁ <: U₁, T₂ <: U₂
```

**Rationale:** Allowing covariance would violate type safety in mutable contexts.

### 7.6 Examples and Patterns

#### 7.6.1 Multiple Return Values

**Error handling (before Result enum):**
```cantrip
function parse_number(s: str): (Option<i32>, str) {
    if let Some(n) = try_parse(s) {
        (Some(n), "")
    } else {
        (None, "invalid number")
    }
}

let (result, error) = parse_number("123")
match result {
    Some(n) => println("Parsed: {n}"),
    None => println("Error: {error}"),
}
```

**Splitting operations:**
```cantrip
function split_at_first_space(s: str): (str, str) {
    if let Some(index) = s.find(' ') {
        (s[..index], s[index + 1..])
    } else {
        (s, "")
    }
}

let (first_word, rest) = split_at_first_space("hello world")
```

#### 7.6.2 Temporary Grouping

**Function arguments:**
```cantrip
function distance(p1: (f64, f64), p2: (f64, f64)): f64 {
    let dx = p2.0 - p1.0
    let dy = p2.1 - p1.1
    (dx * dx + dy * dy).sqrt()
}

let d = distance((0.0, 0.0), (3.0, 4.0))  // 5.0
```

**Swapping values:**
```cantrip
function swap<T>(a: T, b: T): (T, T) {
    (b, a)
}

let (x, y) = swap(10, 20)  // x = 20, y = 10
```

#### 7.6.3 Nested Tuples

**Complex structures:**
```cantrip
// Before parsing into proper records
let person: (str, i32, (str, str)) =
    ("Alice", 30, ("alice@example.com", "+1-555-0123"))

let (name, age, (email, phone)) = person
```

**Matrix operations:**
```cantrip
function matrix_multiply(
    a: ((f64, f64), (f64, f64)),
    b: ((f64, f64), (f64, f64))
): ((f64, f64), (f64, f64)) {
    // 2x2 matrix multiplication
    let ((a11, a12), (a21, a22)) = a
    let ((b11, b12), (b21, b22)) = b
    (
        (a11 * b11 + a12 * b21, a11 * b12 + a12 * b22),
        (a21 * b11 + a22 * b21, a21 * b12 + a22 * b22),
    )
}
```

#### 7.6.4 Pattern Matching

**Destructuring in match:**
```cantrip
function classify_point(p: (i32, i32)): str {
    match p {
        (0, 0) => "origin",
        (0, _) => "on y-axis",
        (_, 0) => "on x-axis",
        (x, y) if x == y => "on diagonal",
        (x, y) if x > 0 && y > 0 => "quadrant I",
        _ => "other",
    }
}
```

**Nested destructuring:**
```cantrip
let data: ((i32, i32), (i32, i32)) = ((1, 2), (3, 4))

match data {
    ((0, 0), _) => println("First point is origin"),
    (_, (0, 0)) => println("Second point is origin"),
    ((x1, y1), (x2, y2)) => {
        println("Points: ({x1}, {y1}) and ({x2}, {y2})")
    }
}
```

#### 7.6.5 When to Prefer Records

**Bad (tuple with unclear meaning):**
```cantrip
function get_user_info(): (str, i32, str, bool) {
    ("Alice", 30, "alice@example.com", true)
}

let info = get_user_info()
let name = info.0     // What is .0?
let age = info.1      // What is .1?
let email = info.2    // What is .2?
let active = info.3   // What is .3?
```

**Good (use a record instead):**
```cantrip
record UserInfo {
    name: str
    age: i32
    email: str
    active: bool
}

function get_user_info(): UserInfo {
    UserInfo {
        name: "Alice",
        age: 30,
        email: "alice@example.com",
        active: true,
    }
}

let info = get_user_info()
let name = info.name    // Clear semantic meaning
```

**Rule of thumb:** If a tuple has more than 3 elements or will be used in multiple places, use a record instead.

---

**Previous**: [Arrays and Slices](02_ArraysAndSlices.md) | **Next**: [Records](04_Records.md)
