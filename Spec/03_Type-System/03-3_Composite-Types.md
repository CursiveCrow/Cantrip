# Part III: Type System
## Section 3.3: Composite Types

**File**: `03-3_Composite-Types.md`
**Version**: 2.0
**Status**: Normative
**Previous**: [03-2_Primitive-Types.md](03-2_Primitive-Types.md) | **Next**: [03-4_Collection-Types.md](03-4_Collection-Types.md)

---

## 3.3.1 Overview

[1] This section specifies Cursive's composite types: types constructed by combining multiple values or representing choices between alternatives.

**Definition 3.3.1** (*Composite Types*):
Composite types are types formed by aggregating multiple values (product types), representing choices between alternatives (sum types), or expressing type-level unions (union types).

[2] A conforming implementation shall provide all composite type forms specified in this section.

[3] Cursive shall provide three categories of composite types:
  (3.1) **Product Types** (§3.3.2) - Aggregate multiple values
    (3.1.1) Tuples: Structural anonymous products
    (3.1.2) Records: Nominal named products with dual access
  (3.2) **Sum Types** (§3.3.3) - Discriminated choices between variants
    (3.2.1) Enums: Tagged unions with named variants
  (3.3) **Union Types** (§3.3.4) - Type-level unions for control-flow joins

[4] All composite types shall integrate with Cursive's permission system (Part VI), grant system (Part IX), and contract system (Part X).

[5] Pattern matching shall provide structured access to composite type values (Part V §5.3).

**Cross-references:**
- Type system foundations: §3.0
- Subtyping relation: §3.1
- Primitive types: §3.2
- Permission system: Part VI §6.2
- Pattern matching: Part V §5.3

---

## 3.3.2 Product Types

### 3.3.2.1 Overview

[6] Product types shall aggregate multiple values into a single composite value.

**Definition 3.3.2** (*Product Types*):
A product type is a composite type that combines a fixed number of values (called components or fields) into a single aggregate value. The set of values of a product type τ₁ × τ₂ × ... × τₙ is the Cartesian product of the value sets of its component types.

[7] Cursive shall provide two forms of product types:
  (7.1) **Tuples** (§3.3.2.2): Structural anonymous products with positional access
  (7.2) **Records** (§3.3.2.3): Nominal named products with dual access (both named and positional field access)

[8] The key difference between tuples and records is typing: tuples are structural (identity by shape), while records are nominal (identity by name).

[9] Product type fields shall inherit permissions from the aggregate. Given `let p: const Point = ...`, accessing `p.x` yields a value with `const` permission.

> **Forward reference**: Permission propagation specified in Part VI §6.2.5.

**Example 1**: Product types comparison ✅
```cursive
// TUPLE: structural, positional-only access
let tuple: const (i32, f64) = (42, 3.14)
let first = tuple.0          // Positional access only

// RECORD: nominal, dual access (named + positional)
record Point { x: f64, y: f64 }
let point: const Point = Point { x: 1.0, y: 2.0 }
let x1 = point.x             // Named access
let x2 = point.0             // Positional access (same field)
```

---

### 3.3.2.2 Tuples

[10] A tuple type shall be an anonymous product type with a fixed number of elements accessed by zero-based positional index.

**Definition 3.3.3** (*Tuple Types*):
A tuple type is an anonymous product type with a fixed number of elements accessed by zero-based positional index. Tuples are structurally typed: two tuple types are equivalent if and only if they have the same number of elements and corresponding element types are equivalent.

#### Syntax

[11] The syntax for tuple types shall be:

```
TupleType ::= '(' Type (',' Type)+ ')'
```

> **Normative Reference**: Complete tuple type grammar in Annex A §A.2.

[12] Tuples shall require at least two elements.

[13] Single-element tuples shall be prohibited to avoid ambiguity with parenthesized expressions.

[14] The unit type `()` (zero elements) is specified in §3.2.6.

**Example 2**: Tuple type declarations ✅
```cursive
let pair: const (i32, bool) = (42, true)
let triple: const (f64, char, string) = (3.14, 'x', "text")
let point = (10.0, 20.0)     // Inferred type: (f64, f64)

// Single-element tuple prohibited
// let single = (42,)        // ERROR: use parenthesized expression instead
```

#### Type Formation

[15] Tuple types shall be well-formed when all component types are well-formed:

**Tuple Well-Formedness Rule:**
```
[WF-Tuple]
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
n ≥ 2
────────────────────────────────────────
Γ ⊢ (τ₁, ..., τₙ) : Type
```

[16] Implementations shall reject tuple types with fewer than two elements.

**Tuple Construction Rule:**
```
[T-Tuple-Ctor]
Γ ⊢ e₁ : τ₁    ...    Γ ⊢ eₙ : τₙ
n ≥ 2
────────────────────────────────────────
Γ ⊢ (e₁, ..., eₙ) : (τ₁, ..., τₙ)
```

**Tuple Projection Rule:**
```
[T-Tuple-Projection]
Γ ⊢ e : (τ₁, ..., τₙ)
0 ≤ i < n
────────────────────────────────────────
Γ ⊢ e.i : τᵢ₊₁
```

[17] Tuple indexing shall be zero-based: the first element is accessed via `.0`, the second via `.1`, and so forth.

**Example 3**: Tuple projection and destructuring ✅
```cursive
let point: const (f64, f64) = (3.0, 4.0)

// Positional access (zero-based)
let x: const f64 = point.0   // First element
let y: const f64 = point.1   // Second element

// Destructuring in let binding
let (x2, y2) = point

// Pattern matching
match point {
    (0.0, 0.0) => println("Origin"),
    (x, y) => println("Point at ({}, {})", x, y)
}
```

#### Structural Equivalence

[18] Two tuple types shall be equivalent if and only if they have the same arity and corresponding element types are equivalent:

**Theorem 3.3.1** (*Tuple Structural Equivalence*):
```
(τ₁, ..., τₙ) ≡ (υ₁, ..., υₘ)  ⟺  n = m ∧ τ₁ ≡ υ₁ ∧ ... ∧ τₙ ≡ υₙ
```

[19] Tuple types are structural. The types `(i32, bool)` and `(i32, bool)` are equivalent regardless of where they are declared or used.

#### Type Properties

[20] Tuples shall be invariant in all element types:

**Theorem 3.3.2** (*Tuple Invariance*):
```
(τ₁, ..., τₙ) <: (υ₁, ..., υₙ)  ⟺  τ₁ ≡ υ₁ ∧ ... ∧ τₙ ≡ υₙ
```

**Proof sketch**: Tuples are both readable and writable, requiring invariance to maintain type safety.

[21] A tuple type shall implement the `Copy` predicate if and only if all element types implement `Copy`:

**Theorem 3.3.3** (*Tuple Copy Predicate*):
```
(τ₁, ..., τₙ) : Copy  ⟺  τ₁ : Copy ∧ ... ∧ τₙ : Copy
```

> **Forward reference**: Copy semantics specified in Part VI §6.3.

[22] The size and alignment of a tuple shall be determined by its element types with padding:

**Theorem 3.3.4** (*Tuple Size and Alignment*):
```
size((τ₁, ..., τₙ)) = aligned_sum(size(τ₁), ..., size(τₙ))
align((τ₁, ..., τₙ)) = max(align(τ₁), ..., align(τₙ))
```

Where `aligned_sum` accounts for padding between fields to satisfy alignment requirements.

> **Forward reference**: Memory layout details in Part VI §6.6.2.

#### Memory Representation

[23] Tuple elements shall be laid out sequentially in memory with padding inserted to satisfy each element's alignment requirements.

[24] The overall tuple alignment shall be the maximum alignment of any element.

**Example layout** for `(i32, bool, f64)` on a 64-bit platform:
```
Memory layout (16 bytes total):
┌──────────┬──────┬─────┬──────────┐
│ i32      │ bool │ pad │ f64      │
│ 4 bytes  │ 1 b  │ 3 b │ 8 bytes  │
└──────────┴──────┴─────┴──────────┘
Offset:   0        4      5   8

Tuple alignment: 8 bytes (max of element alignments)
Tuple size: 16 bytes (includes trailing padding)
```

[25] Implementations shall layout tuple elements in declaration order with padding as necessary to satisfy alignment constraints.

#### Dynamic Semantics

[26] Tuple construction shall evaluate elements left-to-right:

**Tuple Construction Rule:**
```
[E-Tuple]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
──────────────────────────────────────────────────────
⟨(e₁, ..., eₙ), σ⟩ ⇓ ⟨(v₁, ..., vₙ), σₙ⟩
```

[27] Tuple element evaluation order shall be left-to-right and shall be observable when elements have side effects.

**Tuple Projection Rule:**
```
[E-Tuple-Projection]
⟨e, σ⟩ ⇓ ⟨(v₁, ..., vₙ), σ'⟩
0 ≤ i < n
──────────────────────────────────────────
⟨e.i, σ⟩ ⇓ ⟨vᵢ₊₁, σ'⟩
```

**Example 4**: Tuple with System 3 permissions ✅
```cursive
// Immutable tuple
let point: const (f64, f64) = (1.0, 2.0)
let x = point.0              // OK: const access

// Rebindable binding
var rebindable: const (i32, i32) = (1, 2)
rebindable = (3, 4)          // OK: var allows rebinding

// Reference binding
let ref_point: const (f64, f64) <- point
let x2 = ref_point.0         // OK: const reference access
```

---

### 3.3.2.3 Records

[28] A record type shall be a nominal product type with named fields accessed via field names or positional indices.

**Definition 3.3.4** (*Record Types*):
A record type is a nominal product type with named fields accessed via field names or positional indices. Records are nominally typed: two record types are equivalent if and only if they have the same name and originate from the same declaration.

#### Syntax

[29] The syntax for record declarations shall be:

```
RecordDecl ::= 'record' Ident GenericParams? '{' FieldDecl* MethodDecl* '}'

FieldDecl ::= Visibility? Ident ':' Type

Visibility ::= 'public' | 'private'
```

> **Normative Reference**: Complete record declaration grammar in Annex A §A.6.

[30] Records shall provide named fields with optional visibility modifiers and support method declarations with `self` parameters.

**Example 5**: Record declarations ✅
```cursive
// Basic record
record Point {
    x: f64,
    y: f64
}

// Record with methods
record Circle {
    center: Point,
    radius: f64,

    function area(self: const Circle): f64 {
        3.14159 * self.radius * self.radius
    }
}

// Generic record
record Pair<T> {
    first: T,
    second: T
}

// Record with visibility
record BankAccount {
    public account_number: u64,
    private balance: f64
}
```

#### Dual Field Access

[31] Records shall support two field access mechanisms: named access and positional access.

**Definition 3.3.5** (*Field Indexing*):
For a record with n fields declared in order `f₁: τ₁, f₂: τ₂, ..., fₙ: τₙ`, each field `fᵢ` has a positional index `i-1` (zero-based indexing). Fields are indexed in their declaration order (left-to-right).

[32] Record fields shall be indexed in their declaration order with zero-based indexing. The first field in the declaration has index 0, the second has index 1, and so forth.

[33] Positional field access shall have the same visibility constraints as named field access. Accessing a private field positionally from outside its defining module shall be a compile-time error.

**Example 6**: Dual field access ✅
```cursive
record Point { x: f64, y: f64 }
let p = Point { x: 1.0, y: 2.0 }

// Named access: semantic clarity
let x_named = p.x            // Field: x

// Positional access: convenience
let x_positional = p.0       // Index 0 = field x
let y_positional = p.1       // Index 1 = field y

// Both access the same underlying field
assert(p.x == p.0)           // true
assert(p.y == p.1)           // true
```

[34] When to use each access method:
  (34.1) **Named access** (`.field_name`): Use when field semantics are important and self-documenting code is prioritized
  (34.2) **Positional access** (`.0`, `.1`): Use for coordinate-like data, generic operations, or when ergonomic terseness is beneficial

**Example 7**: Positional access use case ✅
```cursive
record Color {
    r: u8,  // Index 0
    g: u8,  // Index 1
    b: u8   // Index 2
}

let red = Color { r: 255, g: 0, b: 0 }

// Positional access enables uniform component processing
let components = [red.0, red.1, red.2]
loop component in components {
    println("Component value: {}", component)
}
```

#### Type Formation

[35] Record types shall be well-formed when all field types are well-formed and field names are distinct:

**Record Well-Formedness Rule:**
```
[WF-Record]
record R { f₁: τ₁, ..., fₙ: τₙ } declared
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
field names f₁, ..., fₙ are distinct
────────────────────────────────────────────
Γ ⊢ R : Type
```

[36] Implementations shall reject record declarations with duplicate field names.

**Record Construction Rule:**
```
[T-Record-Ctor]
record R { f₁: τ₁, ..., fₙ: τₙ } declared
Γ ⊢ e₁ : τ₁    ...    Γ ⊢ eₙ : τₙ
────────────────────────────────────────────
Γ ⊢ R { f₁: e₁, ..., fₙ: eₙ } : R
```

**Named Field Access Rule:**
```
[T-Record-Field-Access]
Γ ⊢ e : R
record R { ..., fᵢ: τᵢ, ... } declared
field fᵢ is visible in current context
────────────────────────────────────────────
Γ ⊢ e.fᵢ : τᵢ
```

**Positional Field Access Rule:**
```
[T-Record-Positional-Access]
Γ ⊢ e : R
record R { f₁: τ₁, ..., fₙ: τₙ } declared
0 ≤ i < n
field at index i is visible in current context
────────────────────────────────────────────
Γ ⊢ e.i : τᵢ₊₁
```

[37] Positional field access retrieves the field at zero-based index `i`. Visibility is determined by the field's declared visibility modifier.

#### Record Construction Syntax

[38] Basic record construction shall use the syntax `RecordName { field: value, ... }`.

[39] Field shorthand shall be supported when variable names match field names: `RecordName { x, y }` is equivalent to `RecordName { x: x, y: y }`.

[40] Struct update syntax shall copy all fields not explicitly specified from the expression using `..expr`:

```cursive
let p1 = Point { x: 1.0, y: 2.0 }
let p2 = Point { x: 5.0, ..p1 }  // x: 5.0, y: 2.0 (from p1)
```

[41] Struct update syntax `..expr` shall copy all fields not explicitly specified from the expression `expr`. The expression `expr` shall have the same record type as the record being constructed.

**Example 8**: Record construction syntax ✅
```cursive
record Point { x: f64, y: f64 }

// Basic construction
let p1 = Point { x: 1.0, y: 2.0 }

// Field shorthand
let x = 5.0
let y = 10.0
let p2 = Point { x, y }          // Equivalent to Point { x: x, y: y }

// Struct update syntax
let p3 = Point { x: 7.0, ..p1 }  // x: 7.0, y: 2.0 (from p1)
let p4 = Point { ..p1 }          // Complete copy
```

#### Nominal Equivalence

[42] Two record types shall be equivalent if and only if they have the same name and originate from the same declaration:

**Theorem 3.3.5** (*Record Nominal Equivalence*):
```
R₁ ≡ R₂  ⟺  R₁ and R₂ have the same name and declaration
```

[43] Records with identical structure but different names are distinct types.

**Example 9**: Nominal typing ❌
```cursive
record Point { x: f64, y: f64 }
record Vector { x: f64, y: f64 }

// Point and Vector are DIFFERENT types despite identical structure
let p: Point = Point { x: 1.0, y: 2.0 }
// let v: Vector = p  // ERROR: Point ≢ Vector (nominal typing)
let v: Vector = Vector { x: p.x, y: p.y }  // OK: explicit conversion
```

#### Type Properties

[44] A record type shall implement the `Copy` predicate if and only if all field types implement `Copy` and the record has no custom destructor:

**Theorem 3.3.6** (*Record Copy Predicate*):
```
R : Copy  ⟺  (∀i. τᵢ : Copy) ∧ (R has no destructor)
```

> **Forward reference**: Copy semantics and destructors specified in Part VI §6.3.

[45] The size and alignment of a record shall be determined by its field types with padding:

**Theorem 3.3.7** (*Record Size and Alignment*):
```
size(R) = aligned_sum(size(τ₁), ..., size(τₙ))
align(R) = max(align(τ₁), ..., align(τₙ))
```

> **Forward reference**: Memory layout details in Part VI §6.6.2.

#### Memory Representation

[46] Record fields shall be laid out sequentially in memory in declaration order with padding inserted to satisfy alignment requirements.

[47] Both named and positional access shall refer to the same underlying memory locations.

**Example layout** for `record Point { x: f64, y: f64 }`:
```
Memory layout (16 bytes total):
┌──────────────┬──────────────┐
│ x: f64       │ y: f64       │
│ 8 bytes      │ 8 bytes      │
└──────────────┴──────────────┘
Offset: 0            8

Field indices:
- x is field 0 (offset 0)
- y is field 1 (offset 8)

Access equivalence:
- p.x  ≡  p.0  (both access offset 0)
- p.y  ≡  p.1  (both access offset 8)
```

#### Visibility and Access Control

[48] Fields may be declared `public` (accessible from any module) or `private` (accessible only within the defining module).

[49] Positional access shall enforce the same visibility constraints as named field access. Accessing a field with insufficient visibility shall be a compile-time error regardless of access method.

**Example 10**: Visibility with positional access ✅
```cursive
record BankAccount {
    public account_number: u64,  // Index 0, public
    private balance: f64          // Index 1, private
}

// From outside the module:
let account = BankAccount { account_number: 12345, balance: 1000.0 }

// Named access
let num = account.account_number   // OK: public field
// let bal = account.balance       // ERROR: private field

// Positional access (same visibility rules)
let num2 = account.0                // OK: public field (field 0)
// let bal2 = account.1             // ERROR: private field (field 1)
```

> **Forward reference**: Complete visibility semantics in Part VII §7.11.

#### Dynamic Semantics

[50] Record construction shall evaluate field initializers in declaration order:

**Record Construction Rule:**
```
[E-Record]
record R { f₁: τ₁, ..., fₙ: τₙ } declared
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
────────────────────────────────────────────────────────
⟨R { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨R { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

[51] Field initializer evaluation order shall be declaration order and shall be observable when initializers have side effects.

**Field Access Rules:**
```
[E-Record-Field-Access]
⟨e, σ⟩ ⇓ ⟨R { f₁: v₁, ..., fₙ: vₙ }, σ'⟩
────────────────────────────────────────────
⟨e.fᵢ, σ⟩ ⇓ ⟨vᵢ, σ'⟩


[E-Record-Positional-Access]
⟨e, σ⟩ ⇓ ⟨R { f₁: v₁, ..., fₙ: vₙ }, σ'⟩
0 ≤ i < n
────────────────────────────────────────────
⟨e.i, σ⟩ ⇓ ⟨vᵢ₊₁, σ'⟩
```

**Example 11**: Records with System 3 permissions ✅
```cursive
// Const record
let p: const Point = Point { x: 1.0, y: 2.0 }
let x = p.x              // OK: const named access
let y = p.1              // OK: const positional access

// Rebindable binding
var point: const Point = Point { x: 1.0, y: 2.0 }
point = Point { x: 3.0, y: 4.0 }  // OK: var allows rebinding

// Reference binding
let ref_point: const Point <- p
let x2 = ref_point.x     // OK: const reference named access
let y2 = ref_point.1     // OK: const reference positional access
```

> **Forward reference**: Permission-based field access and mutation detailed in Part VI §6.2.

---

## 3.3.3 Sum Types

[52] Sum types (also called tagged unions or discriminated unions) shall represent values that can be one of several distinct variants.

**Definition 3.3.6** (*Sum Type*):
A sum type is a composite type where each value belongs to exactly one of several named variants. Each variant may carry associated data (payload). The type system ensures exhaustive handling of all variants through pattern matching.

[53] Each value shall carry a runtime tag (discriminant) identifying which variant is active.

[54] Cursive's primary sum type mechanism shall be **enums** (§3.3.3.1), which provide:
  (54.1) Discriminated unions with compile-time variant exhaustiveness checking
  (54.2) Variants with optional payloads (unit, tuple, or struct variants)
  (54.3) Generic type parameters for parameterized variants
  (54.4) Pattern matching as the primary access mechanism

### 3.3.3.1 Enums

[55] An enum shall be a nominal sum type declaring a fixed set of named variants.

**Definition 3.3.7** (*Enum*):
An enum is a nominal sum type declaring a fixed set of named variants. Each variant may have:
- No payload (unit variant)
- Tuple payload: ordered anonymous fields
- Struct payload: named fields

[56] All enum values shall carry a runtime discriminant identifying the active variant.

[57] Pattern matching shall provide the only safe mechanism to access variant data, ensuring exhaustive coverage of all variants.

#### Syntax

[58] The syntax for enum declarations shall be:

```
EnumDecl ::= 'enum' Ident TypeParams? '{' VariantDecl* '}'

VariantDecl ::= Ident                                    // Unit variant
              | Ident '(' Type (',' Type)* ')'          // Tuple variant
              | Ident '{' FieldDecl* '}'                 // Struct variant

EnumValue ::= EnumName '::' VariantName
            | EnumName '::' VariantName '(' Expr (',' Expr)* ')'
            | EnumName '::' VariantName '{' FieldInit (',' FieldInit)* '}'
```

> **Normative reference**: Annex A §A.4.

**Example 12**: Enum declarations ✅
```cursive
// Unit variants only
enum Status {
    Pending,
    Running,
    Completed
}

// Mixed variant types
enum Shape {
    Point,                                         // Unit variant
    Circle(f64),                                   // Tuple variant (radius)
    Rectangle { width: f64, height: f64 }          // Struct variant
}

// Generic enum
enum Option<T> {
    Some(T),
    None
}

// Multiple type parameters
enum Result<T, E> {
    Ok(T),
    Err(E)
}
```

#### Type Formation

[59] An enum declaration shall be well-formed if all variant names are distinct, all payload types are well-formed, and type parameters are consistently used:

**Enum Well-Formedness Rule:**
```
[WF-Enum]
enum E<α₁, ..., αₘ> { V₁: τ₁, ..., Vₙ: τₙ } declared
Γ, α₁, ..., αₘ ⊢ τ₁ : Type    ...    Γ, α₁, ..., αₘ ⊢ τₙ : Type
variant names V₁, ..., Vₙ are distinct
────────────────────────────────────────────
Γ ⊢ E<α₁, ..., αₘ> : Type
```

[60] Variant names within an enum declaration shall be unique.

#### Variant Typing

[61] Enum variants shall be typed according to their payload:

**Enum Variant Typing Rules:**
```
[T-Enum-Unit-Variant]
enum E { ..., V, ... } declared
V has no payload
────────────────────────────────
Γ ⊢ E::V : E


[T-Enum-Tuple-Variant]
enum E { ..., V(τ₁, ..., τₙ), ... } declared
Γ ⊢ e₁ : τ₁    ...    Γ ⊢ eₙ : τₙ
────────────────────────────────────────────
Γ ⊢ E::V(e₁, ..., eₙ) : E


[T-Enum-Struct-Variant]
enum E { ..., V { f₁: τ₁, ..., fₙ: τₙ }, ... } declared
Γ ⊢ e₁ : τ₁    ...    Γ ⊢ eₙ : τₙ
field names match variant declaration
────────────────────────────────────────────────────────
Γ ⊢ E::V { f₁: e₁, ..., fₙ: eₙ } : E


[T-Enum-Generic]
enum E<α₁, ..., αₘ> { ... } declared
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₘ : Type
────────────────────────────────────────
Γ ⊢ E<τ₁, ..., τₘ> : Type
```

**Example 13**: Enum variant construction ✅
```cursive
enum Status {
    Pending,
    Running,
    Completed
}

let status = Status::Running

enum Shape {
    Point,
    Circle(f64),
    Rectangle { width: f64, height: f64 }
}

let circle = Shape::Circle(5.0)
let rect = Shape::Rectangle { width: 10.0, height: 5.0 }
let point = Shape::Point
```

#### Pattern Matching and Exhaustiveness

[62] Pattern matching on enum types shall be exhaustive, covering all component types:

**Enum Pattern Matching Rule:**
```
[T-Match-Enum]
Γ ⊢ e : E
enum E { V₁, ..., Vₙ } declared
∀i ∈ {1..m}. pattern pᵢ : E and Γ, bindingsᵢ ⊢ armᵢ : τᵣ
patterns {p₁, ..., pₘ} exhaustively cover {V₁, ..., Vₙ}
────────────────────────────────────────────────────────
Γ ⊢ match e { p₁ => arm₁, ..., pₘ => armₘ } : τᵣ
```

**Definition 3.3.8** (*Exhaustive Coverage*):
A set of patterns {p₁, ..., pₘ} exhaustively covers an enum E with variants {V₁, ..., Vₙ} if every possible value of type E matches at least one pattern pᵢ.

[63] Pattern matching on enum types shall be exhaustive. The compiler shall reject non-exhaustive matches as a compile-time error.

**Example 14**: Exhaustive pattern matching ✅
```cursive
enum Status {
    Pending,
    Running,
    Completed
}

let status = Status::Running

// Exhaustive match (all variants covered)
match status {
    Status::Pending => println("Not started"),
    Status::Running => println("In progress"),
    Status::Completed => println("Done")
}

// Also exhaustive (wildcard covers remaining)
match status {
    Status::Pending => println("Not started"),
    _ => println("Other status")
}
```

**Example 15**: Non-exhaustive pattern matching ❌
```cursive
// ERROR: Non-exhaustive (missing Completed variant)
// match status {
//     Status::Pending => println("Not started"),
//     Status::Running => println("In progress")
// }  // ERROR: Missing variant Status::Completed
```

#### Type Properties

[64] An enum shall implement the `Copy` predicate if and only if all its variant payloads implement `Copy`:

**Theorem 3.3.8** (*Enum Copy Predicate*):
```
enum E { V₁: τ₁, ..., Vₙ: τₙ } : Copy  ⟺  ∀i ∈ {1..n}. τᵢ : Copy
```

**Proof sketch**: Since an enum value contains exactly one variant's payload, copying the enum requires copying that payload. If any variant's payload is not `Copy`, the enum cannot be `Copy`.

[65] Every enum value shall carry a discriminant tag identifying its active variant:

**Theorem 3.3.9** (*Enum Discriminant Representation*):
The discriminant is represented as a platform-dependent unsigned integer with values in range [0, n-1] for an enum with n variants:
```
discriminant : usize
variant V₁ has discriminant 0
variant V₂ has discriminant 1
...
variant Vₙ has discriminant n-1
```

[66] An enum's size shall be determined by the discriminant size plus the maximum size of all variants:

```
size(E) = sizeof(discriminant) + max(sizeof(V₁), ..., sizeof(Vₙ)) + padding
align(E) = max(align(discriminant), align(V₁), ..., align(Vₙ))
```

[67] The compiler may optimize enum layouts (e.g., using niche optimization where discriminant information is stored in unused bit patterns of payload types), but the logical behavior shall remain equivalent to the explicit discriminant model.

**Example 16**: Enum copy behavior ✅
```cursive
// Copy enum (all variants have Copy payloads)
enum Signal {
    Start,                    // Unit: Copy
    Value(i32),               // i32: Copy
    End
}

let s1 = Signal::Value(42)
let s2 = s1                  // Copy occurs (Signal is Copy)
```

#### Dynamic Semantics

[68] Enum construction shall evaluate payload expressions and assign the appropriate discriminant:

**Enum Construction Rules:**
```
[E-Enum-Unit]
enum E { ..., V, ... } declared
V has no payload
variant V has index i in E
────────────────────────────────────────
⟨E::V, σ⟩ ⇓ ⟨{ discriminant: i }, σ⟩


[E-Enum-Tuple]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
enum E { ..., V(τ₁, ..., τₙ), ... } declared
variant V has index i in E
────────────────────────────────────────────────────
⟨E::V(e₁, ..., eₙ), σ⟩ ⇓ ⟨{ discriminant: i, payload: (v₁, ..., vₙ) }, σₙ⟩


[E-Enum-Struct]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
enum E { ..., V { f₁: τ₁, ..., fₙ: τₙ }, ... } declared
variant V has index i in E
────────────────────────────────────────────────────
⟨E::V { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨{ discriminant: i, payload: { f₁: v₁, ..., fₙ: vₙ } }, σₙ⟩
```

**Pattern Matching Rule:**
```
[E-Match-Enum]
⟨e, σ⟩ ⇓ ⟨{ discriminant: i, payload: data }, σ'⟩
pattern pⱼ matches variant with index i
bindings = extract_bindings(pⱼ, data)
⟨armⱼ[bindings], σ'⟩ ⇓ ⟨v, σ''⟩
────────────────────────────────────────────
⟨match e { p₁ => arm₁, ..., pₙ => armₙ }, σ⟩ ⇓ ⟨v, σ''⟩
```

**Example 17**: Enums with System 3 permissions ✅
```cursive
enum Option<T> {
    Some(T),
    None
}

// Immutable binding (let + const)
let opt: const Option<i32> = Option::Some(42)
// opt = Option::None;  // ERROR: let is non-rebindable

// Rebindable binding (var + const)
var maybe: const Option<i32> = Option::Some(10)
maybe = Option::None                             // OK: var is rebindable
maybe = Option::Some(20)                         // OK
```

---

## 3.3.4 Union Types

[69] Union types shall provide a mechanism for expressing values that may be one of several alternative types.

**Definition 3.3.9** (*Union Type*):
A union type `τ₁ \/ τ₂` represents a value that is *either* of type `τ₁` *or* type `τ₂`. Union types are discriminated (safe) and carry a runtime tag identifying which component type is active.

[70] Unlike enums which require explicit variant declaration and nominal typing, union types shall use structural typing and are formed dynamically from existing types using the union operator `\/`.

[71] Access to union values shall require pattern matching or explicit type testing to ensure type safety.

[72] Union types shall be distinguished from enums as follows:
  (72.1) **Enums**: Nominal types with named variants, declared upfront, domain modeling
  (72.2) **Union types**: Structural types for ad-hoc combinations, control flow, type-level computation

### 3.3.4.1 Overview

[73] Union types shall enable expressing computations where different execution paths produce values of different types.

[74] Union types shall maintain type safety through:
  (74.1) Runtime discriminant tracking which component type is active
  (74.2) Exhaustive pattern matching to access values
  (74.3) Type refinement through successful pattern matches

**Example 18**: Union types motivation ✅
```cursive
// Without union types: explicit enum required
enum IntOrString {
    Int(i32),
    Str(string)
}

function parse_explicit(input: string): IntOrString {
    if is_numeric(input) {
        result IntOrString::Int(parse_int(input))
    } else {
        result IntOrString::Str(input)
    }
}

// With union types: direct type composition
function parse_union(input: string): i32 \/ string {
    if is_numeric(input) {
        result parse_int(input)           // i32
    } else {
        result input                      // string
    }
    // Compiler infers return type: i32 \/ string
}
```

### 3.3.4.2 Syntax

[75] The syntax for union types shall be:

```
UnionType ::= Type '\/' Type
            | Type ('\/' Type)+            // n-ary unions
```

> **Normative reference**: Annex A §A.2 (Type Grammar).

**Example 19**: Union type syntax ✅
```cursive
// Binary union
let value: i32 \/ string = 42

// Ternary union
let result: i32 \/ f64 \/ string = "error"

// Nested unions (flattened by associativity)
let nested: (i32 \/ string) \/ bool = true
// Equivalent to: i32 \/ string \/ bool

// Union of complex types
let complex: (i32, string) \/ [u8] \/ Option<f64> = Option::Some(3.14)
```

### 3.3.4.3 Type Formation

[76] A union type shall be well-formed if all component types are well-formed:

**Union Type Well-Formedness Rules:**
```
[WF-Union-Binary]
Γ ⊢ τ₁ : Type    Γ ⊢ τ₂ : Type
────────────────────────────────
Γ ⊢ τ₁ \/ τ₂ : Type


[WF-Union-Nary]
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
n ≥ 2
────────────────────────────────────────
Γ ⊢ τ₁ \/ ... \/ τₙ : Type
```

[77] Union types shall maintain a runtime discriminant to identify the active component type.

#### Union Introduction and Elimination

[78] Union values shall be constructed through subtyping (introduction):

**Union Introduction Rules:**
```
[T-Union-Intro-Left]
Γ ⊢ e : τ₁
────────────────────────────
Γ ⊢ e : τ₁ \/ τ₂


[T-Union-Intro-Right]
Γ ⊢ e : τ₂
────────────────────────────
Γ ⊢ e : τ₁ \/ τ₂


[T-Union-Intro-Nary]
Γ ⊢ e : τᵢ
i ∈ {1..n}
────────────────────────────────
Γ ⊢ e : τ₁ \/ ... \/ τₙ
```

[79] Union values shall be accessed through pattern matching (elimination):

**Union Elimination Rule:**
```
[T-Union-Match]
Γ ⊢ e : τ₁ \/ τ₂
Γ, x₁ : τ₁ ⊢ arm₁ : τᵣ
Γ, x₂ : τ₂ ⊢ arm₂ : τᵣ
────────────────────────────────────────
Γ ⊢ match e {
      x₁: τ₁ => arm₁,
      x₂: τ₂ => arm₂
    } : τᵣ
```

[80] Pattern matching on union types shall be exhaustive, covering all component types.

**Example 20**: Union type pattern matching ✅
```cursive
function process(value: i32 \/ string): string {
    match value {
        n: i32 => "Number: " + n.to_string(),
        s: string => "String: " + s
    }
    // Both branches must be present (exhaustiveness)
}

let int_value: i32 \/ string = 42
let str_value: i32 \/ string = "hello"

let result1 = process(int_value)   // "Number: 42"
let result2 = process(str_value)   // "String: hello"
```

### 3.3.4.4 Type Properties

[81] Union types shall be commutative:

**Theorem 3.3.10** (*Union Type Commutativity*):
```
τ₁ \/ τ₂ ≡ τ₂ \/ τ₁
```

**Proof sketch**: Since union types are discriminated, the order of component types does not affect the set of possible values or their runtime representation.

[82] Union types shall be associative:

**Theorem 3.3.11** (*Union Type Associativity*):
```
(τ₁ \/ τ₂) \/ τ₃ ≡ τ₁ \/ (τ₂ \/ τ₃) ≡ τ₁ \/ τ₂ \/ τ₃
```

**Proof sketch**: Nested unions flatten into n-ary unions. The grouping does not affect the discriminant or available component types.

[83] Union with the same type shall be equivalent to the type itself:

**Theorem 3.3.12** (*Union Type Idempotence*):
```
τ \/ τ ≡ τ
```

[84] Union with the never type shall be equivalent to the other type:

**Theorem 3.3.13** (*Union with Never Type*):
```
τ \/ ! ≡ τ
```

**Proof sketch**: Since the never type `!` has no values, a union `τ \/ !` can only contain values of type `τ`.

[85] Union types shall have similar memory representation to enums:

```
size(τ₁ \/ τ₂) = sizeof(discriminant) + max(sizeof(τ₁), sizeof(τ₂)) + padding
align(τ₁ \/ τ₂) = max(align(discriminant), align(τ₁), align(τ₂))
```

### 3.3.4.5 Subtyping

[86] Each component type shall be a subtype of the union (subsumption):

**Union Subtyping Rules:**
```
[Sub-Union-Left]
────────────────────────────
τ₁ <: τ₁ \/ τ₂


[Sub-Union-Right]
────────────────────────────
τ₂ <: τ₁ \/ τ₂


[Sub-Union-Nary]
i ∈ {1..n}
────────────────────────────────
τᵢ <: τ₁ \/ ... \/ τₙ
```

[87] These rules state that each component type is a subtype of the union. This allows automatic widening from a specific type to a union containing that type.

**Example 21**: Automatic widening ✅
```cursive
function accepts_union(value: i32 \/ string) {
    match value {
        n: i32 => println("Got number: {}", n),
        s: string => println("Got string: {}", s)
    }
}

let n: i32 = 42
accepts_union(n)               // OK: i32 <: i32 \/ string (automatic widening)

let s: string = "hello"
accepts_union(s)               // OK: string <: i32 \/ string (automatic widening)
```

### 3.3.4.6 Use Cases

[88] Union types shall be particularly useful in the following scenarios:
  (88.1) **Control-flow joins**: When branches of conditionals return different types
  (88.2) **Modal state transitions**: When state machine transitions may result in multiple states
  (88.3) **Error handling**: Combining success and error types without defining custom enums
  (88.4) **Type refinement**: Progressively narrowing type possibilities through pattern matching

**Example 22**: Control-flow joins ✅
```cursive
function parse_config(key: string): i32 \/ bool \/ string {
    match key {
        "max_connections" => 100,                    // i32
        "debug_mode" => true,                        // bool
        "server_name" => "localhost",                // string
        _ => "unknown"                               // string
    }
    // Return type: i32 \/ bool \/ string
}

let config_value = parse_config("debug_mode")

match config_value {
    n: i32 => println("Integer config: {}", n),
    b: bool => println("Boolean config: {}", b),
    s: string => println("String config: {}", s)
}
```

**Example 23**: Error handling ✅
```cursive
function divide_safe(a: f64, b: f64): f64 \/ string {
    if b == 0.0 {
        result "Division by zero"
    } else {
        result a / b
    }
}

let result = divide_safe(10.0, 2.0)

match result {
    value: f64 => println("Result: {}", value),
    error: string => println("Error: {}", error)
}
```

**Example 24**: Type refinement ✅
```cursive
function process_input(input: i32 \/ string \/ bool): string {
    // First refinement: narrow to numeric or non-numeric
    match input {
        n: i32 => {
            // Type refined to i32
            if n > 0 {
                "Positive: " + n.to_string()
            } else {
                "Non-positive: " + n.to_string()
            }
        }
        rest: string \/ bool => {
            // Type refined to string \/ bool
            match rest {
                s: string => "String: " + s,
                b: bool => "Bool: " + b.to_string()
            }
        }
    }
}
```

### 3.3.4.7 Distinction from Unsafe Unions

[89] Cursive shall provide two forms of unions with different safety guarantees:
  (89.1) **Safe union types** (`τ₁ \/ τ₂`): Tagged/discriminated, type-safe access, automatic memory management
  (89.2) **Unsafe unions** (`union` keyword, Part XIV §14.3): Untagged, unsafe access, manual management

**Table 3.3.1**: Safe vs Unsafe Unions

| Feature | Safe Union Types (`\/`) | Unsafe Unions (`union`) |
|---------|------------------------|-------------------------|
| Discriminant | Yes (automatic) | No (manual tracking) |
| Access method | Pattern matching | Direct field access |
| Safety | Type-safe | Requires `unsafe` |
| Memory overhead | Discriminant + max variant | Max variant only |
| Use case | Type composition, control flow | FFI, bit manipulation |

[90] Safe union types shall not allow untagged reinterpretation. For untagged unions, use the `union` keyword with appropriate `unsafe` effects (Part XIV §14.3).

> **Forward references**:
> - Pattern matching semantics: Part V §5.4
> - Unsafe unions and FFI: Part XIV §14.3
> - Modal types: §3.5

---

## 3.3.5 Integration

[91] The composite types specified in this section integrate with the following language components:

**Cross-references:**
- Type system foundations: §3.0
- Subtyping and equivalence: §3.1
- Primitive types: §3.2
- Tuple and record operations: Part IV §4.4
- Pattern matching: Part V §5.3, §5.4
- Permission system: Part VI §6.2
- Method dispatch: Part VII §7.2
- Marker predicates: Part VIII §8.6

[92] Product types (tuples and records) provide the foundation for aggregate data structures.

[93] Sum types (enums) provide the foundation for discriminated unions and algebraic data types.

[94] Union types provide the foundation for ad-hoc type composition in control flow.

[95] All composite types shall follow the same permission model as primitive types, with permissions propagating from the aggregate to its components.

---

**Previous**: [03-2_Primitive-Types.md](03-2_Primitive-Types.md) | **Next**: [03-4_Collection-Types.md](03-4_Collection-Types.md)
