# Chapter 2: Type System - §2.2 Product Types

**Section**: §2.2 | **Part**: Type System (Part II)
**Previous**: [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) | **Next**: [Sum Types](03_SumTypes.md)

---

## 2.2 Product Types Overview

Product types combine multiple values into a single composite value. Cantrip provides three product type forms:

- **Tuples** (§2.2.1): Anonymous ordered collections with positional access
- **Records** (§2.2.2): Named field collections with nominal typing
- **Tuple Structs** (§2.2.3): Named wrappers around tuples

**Key distinction:** Tuples use structural typing (two tuples with the same types are interchangeable), while records use nominal typing (two records with different names are distinct types even if they have the same fields).

---

**Definition 7.1 (Tuple):** A tuple is an anonymous product type containing a fixed number of heterogeneous elements: `(T₁, ..., Tₙ) = T₁ × T₂ × ... × Tₙ`. Tuple types and literals use the same syntax `(T₁, ..., Tₙ)` and `(e₁, ..., eₙ)`, with the unit type `()` as the zero-element tuple.

## 2.2.1 Tuples

#### 2.2.1.1 Overview

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

#### 2.2.1.2 Syntax

##### 2.2.1.2.1 Concrete Syntax

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

##### 2.2.1.2.2 Abstract Syntax

**Tuples:**

```
τ ::= (τ₁, ..., τₙ)  (tuple type)
    | ()             (unit type)

e ::= (e₁, ..., eₙ)  (tuple literal)
    | e.i            (tuple projection)
    | ()             (unit value)

p ::= (p₁, ..., pₙ)  (tuple pattern)
```

#### 2.2.1.3 Static Semantics

##### 2.2.1.3.1 Well-Formedness Rules

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

##### 2.2.1.3.2 Type Rules

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

##### 2.2.1.3.3 Type Properties

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

#### 2.2.1.4 Dynamic Semantics

##### 2.2.1.4.1 Evaluation Rules

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

##### 2.2.1.4.2 Memory Representation

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

##### 2.2.1.4.3 Operational Behavior

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

#### 2.2.1.5 Additional Properties

##### 2.2.1.5.1 Parameter Passing and Copy Capability

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

##### 2.2.1.5.2 Subtyping

Tuples are **invariant** in their type parameters:

```
(T₁, T₂) is NOT a subtype of (U₁, U₂) even if T₁ <: U₁, T₂ <: U₂
```

**Rationale:** Allowing covariance would violate type safety in mutable contexts.

---

**Definition 8.1 (Record):** A record is a labeled Cartesian product of its fields. Formally, if `R { f₁: T₁; …; fₙ: Tₙ }` then `⟦R⟧ ≅ T₁ × … × Tₙ` with named projections `fᵢ : R → Tᵢ`. Records provide procedures (see §13.7) within their definition; inheritance is not part of the model—use traits and composition.

## 2.2.2 Records and Classes

#### 2.2.2.1 Overview

**Key innovation/purpose:** Records provide named product types with procedures, enabling data encapsulation and object-oriented programming without inheritance complexity.

**When to use records:**

- Structured data with named fields
- Data that benefits from semantic field names
- Types requiring associated procedures (methods)
- Building blocks for complex data structures
- Domain modeling with clear structure
- When you need encapsulation and visibility control

**When NOT to use records:**

- Simple heterogeneous groupings → use tuples (§7)
- Discriminated unions → use enums (§9)
- Homogeneous collections → use arrays (§6) or Vec<T>
- Temporary data groupings → use tuples
- Very small data (2-3 fields) without procedures → consider tuples

**Relationship to other features:**

- **Tuples**: Records are like tuples with named fields instead of positional access
- **Traits**: Records implement traits for polymorphic behavior
- **Generics**: Records can be generic over type parameters
- **Procedures**: Records define procedures within their definition
- **Permissions**: Record fields can have different permissions (own, mut, immutable)
- **Memory layout**: Controllable via attributes for FFI and performance

#### 2.2.2.2 Syntax

##### 2.2.2.2.1 Concrete Syntax

**Record declaration:**

```ebnf
RecordDecl   ::= "record" Ident GenericParams? "{" FieldList "}"
FieldList    ::= Field (";" Field)* ";"?
Field        ::= Visibility? Ident ":" Type
Visibility   ::= "public" | "private"
GenericParams ::= "<" TypeParam ("," TypeParam)* ">"
```

**Record construction:**

```ebnf
RecordExpr   ::= Ident "{" FieldInit ("," FieldInit)* ","? "}"
FieldInit    ::= Ident ":" Expr
               | Ident              // Shorthand: field: field
```

**Field access:**

```ebnf
FieldAccess  ::= Expr "." Ident
```

**Examples:**

```cantrip
record Point {
    x: f64
    y: f64
}

public record User {
    public name: String
    public email: String
    private password_hash: String
}

let origin = Point { x: 0.0, y: 0.0 }
let x_coord = origin.x
```

##### 2.2.2.2.2 Abstract Syntax

**Record types:**

```
τ ::= record Name { f₁: τ₁; ...; fₙ: τₙ }    (record type)
    | Name                                     (record type reference)
```

**Record expressions:**

```
e ::= Name { f₁: e₁, ..., fₙ: eₙ }            (record literal)
    | e.f                                      (field access)
```

**Record patterns:**

```
p ::= Name { f₁: p₁, ..., fₙ: pₙ }            (record pattern)
    | Name { f₁, ..., fₙ }                     (shorthand pattern)
```

#### 2.2.2.3 Static Semantics

##### 2.2.2.3.1 Well-Formedness Rules

**Record declaration:**

```
[WF-Record]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
fields f₁, ..., fₙ are distinct
──────────────────────────────────────────────
Γ ⊢ record Name { f₁: T₁; ...; fₙ: Tₙ } : Type
```

**Generic record:**

```
[WF-Generic-Record]
Γ, α₁ : Type, ..., αₘ : Type ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
fields f₁, ..., fₙ are distinct
────────────────────────────────────────────────────────────────────
Γ ⊢ record Name<α₁, ..., αₘ> { f₁: T₁; ...; fₙ: Tₙ } : Type
```

##### 2.2.2.3.2 Type Rules

**Record construction:**

```
[T-Record]
record Name { f₁: T₁; ...; fₙ: Tₙ } in Γ
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────────────
Γ ⊢ Name { f₁: e₁, ..., fₙ: eₙ } : Name
```

**Field access:**

```
[T-Field]
Γ ⊢ e : record S    field f : T in S
────────────────────────────────────
Γ ⊢ e.f : T
```

**Record pattern:**

```
[T-Record-Pattern]
record Name { f₁: T₁; ...; fₙ: Tₙ }
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────────
Γ ⊢ Name { f₁: p₁, ..., fₙ: pₙ } : Name ⇝ Γ₁ ∪ ... ∪ Γₙ
```

##### 2.2.2.3.3 Type Properties

**Theorem 8.1 (Structural Typing):**

Records with identical field names and types are distinct types:

```
record A { x: i32; y: i32 } ≠ record B { x: i32; y: i32 }
```

Cantrip uses nominal typing for records, not structural typing.

**Theorem 8.2 (Copy Capability):**

A record implements `Copy` if and only if all its fields implement `Copy`:

```
record R { f₁: T₁; ...; fₙ: Tₙ } : Copy
⟺
T₁ : Copy ∧ ... ∧ Tₙ : Copy
```

**Parameter Passing:** Records pass by permission regardless of Copy capability.

**Theorem 8.3 (Alignment and Size):**

For record R with fields f₁: T₁, ..., fₙ: Tₙ:

```
align(R) = max(align(T₁), ..., align(Tₙ))
size(R) = aligned_sum(size(T₁), ..., size(Tₙ), align(R))
```

where aligned_sum accounts for padding between fields.

#### 2.2.2.4 Dynamic Semantics

##### 2.2.2.4.1 Evaluation Rules

**Record construction:**

```
[E-Record]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
─────────────────────────────────────────────────────
⟨Name { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨Name { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

**Field access:**

```
[E-Field]
⟨e, σ⟩ ⇓ ⟨Name { f₁: v₁, ..., fᵢ: vᵢ, ..., fₙ: vₙ }, σ'⟩
──────────────────────────────────────────────────────────
⟨e.fᵢ, σ⟩ ⇓ ⟨vᵢ, σ'⟩
```

##### 2.2.2.4.2 Memory Representation

**Default layout:**

Records are laid out in memory with fields in declaration order, with padding inserted for alignment:

```
record Example {
    a: u8      // offset 0
    b: u32     // offset 4 (3 bytes padding)
    c: u16     // offset 8
}

Memory layout:
┌──┬──┬──┬──┬────┬────┬──┐
│a │p │p │p │ b  │ c  │p │
└──┴──┴──┴──┴────┴────┴──┘
Total: 12 bytes (rounded to alignment of 4)
```

**Layout calculation:**

```
layout(record R) = {
    size = ∑ᵢ size(fᵢ) + padding
    align = max(align(f₁), ..., align(fₙ))
    offset(fᵢ) = aligned(∑ⱼ₍ⱼ₍ᵢ₎ size(fⱼ), align(fᵢ))
}
```

##### 2.2.2.4.3 Operational Behavior

**Copy Capability vs Parameter Passing:**

```cantrip
record Point {
    x: i32  // Copy-capable
    y: i32  // Copy-capable
}
// Point is Copy-capable (all fields are Copy)

let p1 = Point { x: 1, y: 2 }
procedure process(pt: Point) { ... }
process(p1)  // Permission, p1 still usable

let p2 = p1.copy()  // Explicit copy
assert(p1.x == p2.x)

record Entity {
    name: String  // Non-Copy
    id: i32       // Copy-capable
}
// Entity is NOT Copy (contains String)

let e1 = Entity { name: String.new("test"), id: 1 }
process_entity(e1)  // Permission, e1 still usable
consume_entity(move e1)  // Explicit move
// e1 no longer usable
```

#### 2.2.2.5 Procedures

**Definition 8.2 (Procedure):** A procedure is a method with an explicit `self` parameter defined within a record.

**Syntax:**

```cantrip
record TypeName {
    // fields

    procedure method_name(self: Permission TypeName, params): T
        must ...
        will ...
    {
        ...
    }
}
```

**Type rule:**

```
[T-Procedure]
procedure m : Self => T => U in record S
Γ ⊢ self : S    Γ ⊢ arg : T
───────────────────────────────────────
Γ ⊢ self.m(arg) : U
```

**Procedure call desugaring:**

```
obj.procedure(args) ≡ Type::procedure(obj, args)
```

**Example:**

```cantrip
record Point {
    x: f64
    y: f64

    procedure new(x: f64, y: f64): own Point {
        own Point { x, y }
    }

    procedure distance(self: Point, other: Point): f64 {
        let dx = self.x - other.x
        let dy = self.y - other.y
        (dx * dx + dy * dy).sqrt()
    }

    procedure move_by(self: mut Point, dx: f64, dy: f64) {
        self.x += dx
        self.y += dy
    }
}
```

#### 2.2.2.6 Advanced Features

##### 2.2.2.6.1 Memory Layout Control

**C-compatible layout:**

```cantrip
[[repr(C)]]
record Vector3 {
    x: f32  // offset 0
    y: f32  // offset 4
    z: f32  // offset 8
}
// Size: 12 bytes, Align: 4
```

**Formal semantics:**

```
[[repr(C)]] ⟹ {
    fields laid out in declaration order
    no reordering optimization
    matches C struct layout
    ABI-compatible with extern "C"
}
```

**Packed layout (no padding):**

```cantrip
[[repr(packed)]]
record NetworkHeader {
    magic: u32     // offset 0
    version: u16   // offset 4
    length: u16    // offset 6
}
// Size: 8 bytes, Align: 1
```

**Warning:** Packed layouts can cause unaligned access, which may be undefined behavior on some architectures.

**Explicit alignment:**

```cantrip
[[repr(align(64))]]
record CacheLine {
    data: [u8; 64]
}
// Aligned to 64-byte boundary for cache optimization
```

##### 2.2.2.6.2 Structure-of-Arrays

**Attribute:**

```cantrip
[[soa]]
record Particle {
    position: Vec3
    velocity: Vec3
    lifetime: f32
}
```

**Compiler transformation:**

```cantrip
// Generates:
record ParticleSOA {
    positions: Vec<Vec3>
    velocities: Vec<Vec3>
    lifetimes: Vec<f32>
    length: usize

    procedure get(self: ParticleSOA, index: usize): Particle
        must index < self.length
    {
        Particle {
            position: self.positions[index],
            velocity: self.velocities[index],
            lifetime: self.lifetimes[index],
        }
    }
}
```

**Benefits:**

- Better cache locality for iterating over specific fields
- SIMD optimization opportunities
- Useful for game engines and data-intensive applications

#### 2.2.2.8 Tuple Structs

**Definition 8.2 (Tuple Struct):** A tuple struct is a nominal record type with unnamed, positionally-accessed fields. Formally, `record Name(T₁, ..., Tₙ)` creates a distinct type with fields accessed as `.0`, `.1`, etc. Unlike type aliases, tuple structs are not transparent and provide nominal type safety.

##### 2.2.2.8.1 Syntax

**Grammar:**

```ebnf
TupleStruct     ::= "record" Ident GenericParams? "(" TypeList ")" ";"
TypeList        ::= Type ("," Type)* ","?
TupleStructExpr ::= Ident "(" ExprList ")"
TupleFieldAccess ::= Expr "." IntLiteral
```

**Examples:**

```cantrip
// Basic tuple structs
record Point2D(f64, f64)
record Color(u8, u8, u8)
record Newtype(i32)

// Generic tuple structs
record Wrapper<T>(T)
record Pair<A, B>(A, B)

// Construction
let p = Point2D(1.0, 2.0)
let c = Color(255, 128, 0)

// Field access by position
let x = p.0  // First field
let y = p.1  // Second field
```

##### 2.2.2.8.2 Abstract Syntax

**Tuple struct types:**

```
τ ::= record Name(τ₁, ..., τₙ)    (tuple struct type)
    | Name                         (tuple struct type reference)
```

**Tuple struct expressions:**

```
e ::= Name(e₁, ..., eₙ)            (tuple struct construction)
    | e.n                          (positional field access, n ∈ ℕ)
```

**Tuple struct patterns:**

```
p ::= Name(p₁, ..., pₙ)            (tuple struct pattern)
```

##### 2.2.2.8.3 Well-Formedness Rules

**[WF-TupleStruct] Tuple Struct Declaration:**

```
[WF-TupleStruct]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
n ≥ 1    (at least one field)
────────────────────────────────────────────
Γ ⊢ record Name(T₁, ..., Tₙ) well-formed
```

**Explanation:** Tuple structs must have at least one field. Zero-field tuple structs are not allowed (use unit type `()` instead).

```cantrip
// ✓ Valid: at least one field
record UserId(u64)
record Point(f64, f64)

// ✗ Invalid: zero fields
// record Empty()  // Use () instead
```

**[WF-TupleStruct-Generic] Generic Tuple Struct:**

```
[WF-TupleStruct-Generic]
Γ, α₁ : Type, ..., αₖ : Type ⊢ T₁ : Type    ...    Γ, α₁ : Type, ..., αₖ : Type ⊢ Tₙ : Type
────────────────────────────────────────────
Γ ⊢ record Name<α₁, ..., αₖ>(T₁, ..., Tₙ) well-formed
```

**Explanation:** Generic tuple structs can be parameterized by type variables.

```cantrip
// Generic wrapper
record Wrapper<T>(T)

// Multiple type parameters
record Result<T, E>(bool, T, E)  // (success, value, error)

let w: Wrapper<i32> = Wrapper(42)
let r: Result<String, Error> = Result(true, "ok", error_val)
```

##### 2.2.2.8.4 Type Rules

**[T-TupleStruct-Cons] Tuple Struct Construction:**

```
[T-TupleStruct-Cons]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
────────────────────────────────────────────
Γ ⊢ Name(e₁, ..., eₙ) : Name
```

**Explanation:** Tuple struct construction must arguments matching field types in order.

```cantrip
record Point3D(f64, f64, f64)

let p = Point3D(1.0, 2.0, 3.0)  // ✓ Correct types
// let bad = Point3D(1, 2, 3)   // ✗ ERROR: expected f64, found i32
```

**[T-TupleStruct-Field] Positional Field Access:**

```
[T-TupleStruct-Field]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e : Name
0 ≤ i < n
────────────────────────────────────────────
Γ ⊢ e.i : Tᵢ₊₁    (0-indexed)
```

**Explanation:** Field access uses numeric literals: `.0` for first field, `.1` for second, etc.

```cantrip
record Color(u8, u8, u8)

let c = Color(255, 128, 0)
let red = c.0    // ✓ u8
let green = c.1  // ✓ u8
let blue = c.2   // ✓ u8
// let bad = c.3 // ✗ ERROR: no field 3
```

**[T-TupleStruct-Nominal] Nominal Typing:**

```
[T-TupleStruct-Nominal]
record A(T₁, ..., Tₙ) declared
record B(T₁, ..., Tₙ) declared
A ≠ B
────────────────────────────────────────────
A ≢ B    (not equivalent)
```

**Explanation:** Tuple structs with identical field types are distinct types (nominal typing).

```cantrip
record Meters(f64)
record Feet(f64)

let m = Meters(10.0)
let f = Feet(33.0)

// ✗ ERROR: cannot assign Feet to Meters (distinct types)
// let distance: Meters = f

// Must convert explicitly
function feet_to_meters(f: Feet) => Meters {
    Meters(f.0 * 0.3048)
}
```

**[T-TupleStruct-Pattern] Pattern Matching:**

```
[T-TupleStruct-Pattern]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ p₁ : T₁    ...    Γ ⊢ pₙ : Tₙ
────────────────────────────────────────────
Γ ⊢ Name(p₁, ..., pₙ) : pattern for Name
```

**Explanation:** Tuple structs can be destructured in patterns.

```cantrip
record Point(i32, i32)

let p = Point(5, 10)

match p {
    Point(x, y) => {
        println("x: {}, y: {}", x, y)
    }
}

// Or using let binding
let Point(x, y) = p
assert(x == 5 && y == 10)
```

**[T-TupleStruct-Method] Methods on Tuple Structs:**

```
[T-TupleStruct-Method]
record Name(T₁, ..., Tₙ) {
    procedure f(self: Self, ...) : U { e }
}
────────────────────────────────────────────
Methods can access self.0, self.1, ..., self.n-1
```

**Explanation:** Tuple structs support methods within their definition.

```cantrip
record Vector2(f64, f64) {
    procedure length(self: Self): f64 {
        (self.0 * self.0 + self.1 * self.1).sqrt()
    }

    procedure dot(self: Self, other: Vector2): f64 {
        self.0 * other.0 + self.1 * other.1
    }
}

let v = Vector2(3.0, 4.0)
assert(v.length() == 5.0)
```

**[T-TupleStruct-Trait] Trait Implementation:**

```
[T-TupleStruct-Trait]
record Name(T₁, ..., Tₙ): Trait {
    // procedures implementing Trait
}
────────────────────────────────────────────
Name : Trait
```

**Explanation:** Tuple structs can implement traits.

```cantrip
record UserId(u64): Display {
    procedure fmt(self: Self): String {
        "User#" + self.0.to_string()
    }
}

let id = UserId(42)
println("{}", id)  // Output: User#42
```

**[T-TupleStruct-Generic-Inst] Generic Instantiation:**

```
[T-TupleStruct-Generic-Inst]
record Name<α₁, ..., αₖ>(T₁, ..., Tₙ) declared
Γ ⊢ U₁ : Type    ...    Γ ⊢ Uₖ : Type
────────────────────────────────────────────
Γ ⊢ Name<U₁, ..., Uₖ> : Type
```

**Explanation:** Generic tuple structs are instantiated by providing concrete types.

```cantrip
record Wrapper<T>(T)

let i: Wrapper<i32> = Wrapper(42)
let s: Wrapper<String> = Wrapper("hello")

// Generic with multiple parameters
record Pair<A, B>(A, B)
let p: Pair<i32, String> = Pair(1, "one")
```

**[T-TupleStruct-Mutability] Mutable Field Access:**

```
[T-TupleStruct-Mutability]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e : mut Name
0 ≤ i < n
────────────────────────────────────────────
Γ ⊢ e.i : mut Tᵢ₊₁    (mutable access)
```

**Explanation:** Mutable tuple struct instances allow mutable field access.

```cantrip
record Counter(i32)

var c = Counter(0)
c.0 += 1  // ✓ Mutable field access
assert(c.0 == 1)
```

##### 2.2.2.8.5 Memory Representation

Tuple structs have the same memory layout as tuples with the same field types:

```
record Name(T₁, T₂, ..., Tₙ)

Size: size(T₁) + size(T₂) + ... + size(Tₙ) + padding
Alignment: max(align(T₁), align(T₂), ..., align(Tₙ))
```

**Memory layout:**

```
Tuple Struct Name(T₁, T₂, T₃):
┌───────────┬───────────┬───────────┐
│  field 0  │  field 1  │  field 2  │
│  (T₁)     │  (T₂)     │  (T₃)     │
└───────────┴───────────┴───────────┘
```

**Example:**

```cantrip
record Point(f64, f64)
// Size: 16 bytes (2 × 8 bytes)
// Alignment: 8 bytes

record Color(u8, u8, u8)
// Size: 3 bytes (3 × 1 byte, no padding needed if standalone)
// Alignment: 1 byte
```

**Layout control:**
Tuple structs support the same layout attributes as named records:

```cantrip
[[repr(C)]]
record CPoint(f64, f64)  // C-compatible layout

[[repr(packed)]]
record Packed(u8, u16, u8)  // No padding

[[repr(align(64))]]
record CacheAligned(i32)  // 64-byte alignment
```

##### 2.2.2.8.6 Comparison: Tuple Structs vs. Tuples vs. Named Records

| Feature              | Tuple `(T₁, T₂)`  | Tuple Struct `record Name(T₁, T₂)` | Named Record `record Name { a: T₁; b: T₂ }` |
| -------------------- | ----------------- | ---------------------------------- | ------------------------------------------- |
| **Type identity**    | Structural        | Nominal                            | Nominal                                     |
| **Field access**     | `.0`, `.1`        | `.0`, `.1`                         | `.a`, `.b`                                  |
| **Type safety**      | Weak (structural) | Strong (nominal)                   | Strong (nominal)                            |
| **Semantic meaning** | Generic grouping  | Domain-specific type               | Domain-specific type                        |
| **Trait impls**      | Cannot add custom | Can add custom                     | Can add custom                              |
| **Methods**          | No                | Yes (inline)                       | Yes (inline)                                |
| **Readability**      | Low (positional)  | Medium (positional)                | High (named fields)                         |
| **Use case**         | Temporary data    | Newtype pattern, wrappers          | Complex data structures                     |

**When to use each:**

- **Tuples** `(T₁, T₂)`: Temporary, ad-hoc groupings with no semantic meaning
- **Tuple Structs** `record Name(T₁, T₂)`: Newtype pattern, type safety for similar types, simple wrappers
- **Named Records** `record Name { f₁: T₁; f₂: T₂ }`: Complex data structures with many fields, public APIs

##### 2.2.2.8.7 Type Properties

**Theorem 8.1 (Tuple Struct Nominality):** Tuple structs with identical field types are distinct types. For any tuple structs `record A(T)` and `record B(T)`, `A ≠ B` implies `A ≢ B`.

**Proof sketch:** The type system assigns each tuple struct declaration a unique nominal identity. Even if field types are identical, the nominal identities differ, making the types incompatible. ∎

**Corollary 8.1:** Tuple structs provide type safety through nominal distinction, preventing accidental type confusion.

**Theorem 8.2 (Memory Layout Equivalence):** A tuple struct `record Name(T₁, ..., Tₙ)` has the same memory layout as the tuple type `(T₁, ..., Tₙ)`.

**Proof sketch:** Both tuple structs and tuples arrange fields in declaration order with appropriate padding. The only difference is the nominal type tag, which exists only at compile time. ∎

**Corollary 8.2:** Tuple structs have zero runtime overhead compared to tuples.

**Theorem 8.3 (Pattern Exhaustiveness):** Pattern matching on tuple structs is exhaustive if and only if all fields are matched.

**Proof sketch:** A tuple struct `Name(T₁, ..., Tₙ)` has exactly one variant. A pattern `Name(p₁, ..., pₙ)` is exhaustive if all patterns `pᵢ` are exhaustive for their respective types `Tᵢ`. ∎

---

### 2.2.3 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - product types combine primitive types
- See §2.3 for [Sum Types](03_SumTypes.md) - duality with product types (∧ vs ∨)
- See §2.4 for [Collection Types](04_CollectionTypes.md) - arrays and slices are homogeneous products
- See §2.6 for [Traits](../PART_D_Abstraction/06_Traits.md) - records implement traits for polymorphic behavior
- See §2.7 for [Generics](../PART_D_Abstraction/07_Generics.md) - generic product types (Pair<T, U>)
- See Part III for [Permission System](../../03_Permissions/) - permissions apply to product types
- See §13 for [Pattern Matching](../../13_PatternMatching/) - destructuring tuples and records
- See §18 for [Procedures](../../18_Procedures/) - record methods and procedures
- **Examples**: See [02_ProductExamples.md](../Examples/02_ProductExamples.md) for practical usage patterns

### 2.2.4 Notes and Warnings

**Note 2.2.1:** Tuples use structural typing (two tuples with the same component types are interchangeable), while records and tuple structs use nominal typing (distinct type identities).

**Note 2.2.2:** The unit type `()` is both a type and a value. It represents "no data" and is used for procedures that perform effects without returning meaningful values.

**Note 2.2.3:** All product types (tuples, records, tuple structs) implement Copy if and only if all their fields implement Copy. Regardless of Copy capability, all types pass by permission.

**Note 2.2.4:** Tuple structs provide a lightweight alternative to full records when you need nominal typing without named fields. They are ideal for the newtype pattern.

**Performance Note 2.2.1:** Product types have zero runtime overhead for field access. All projections (`.0`, `.field_name`) compile to direct offset calculations.

**Performance Note 2.2.2:** The compiler may optimize tuple construction and destructuring, especially in cases like `let (a, b) = (b, a)` which can become register swaps.

**Performance Note 2.2.3:** Records with #[repr(C)] guarantee C-compatible layout at the cost of disabling field reordering optimizations.

**Warning 2.2.1:** Tuples with more than 3-4 fields become hard to understand. Use records instead for clarity and maintainability.

**Warning 2.2.2:** Packed records (#[repr(packed)]) can cause unaligned memory access, which is undefined behavior on some architectures. Use with caution.

**Warning 2.2.3:** Partial moves are not allowed for product types. A tuple or record moves as a complete unit, not field-by-field.

**Warning 2.2.4:** Field order in tuples and records matters for memory layout. Reordering fields can change padding and total size.

**Implementation Note 2.2.1:** Pattern matching on product types is exhaustive checking. The compiler ensures all fields are accounted for in patterns.

**Implementation Note 2.2.2:** Structure-of-Arrays (#[soa]) is a compiler transformation that reorganizes data layout for cache efficiency in data-intensive applications.

---

**Previous**: [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) | **Next**: [Sum Types](03_SumTypes.md)
