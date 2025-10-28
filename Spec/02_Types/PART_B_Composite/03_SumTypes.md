# Chapter 2: Type System - §2.3 Sum Types

**Section**: §2.3 | **Part**: Type System (Part II)
**Previous**: [Product Types](02_ProductTypes.md) | **Next**: [Collection Types](04_CollectionTypes.md)

---

## 2.3 Sum Types Overview

Sum types (also called tagged unions or variant types) represent values that can be one of several alternatives. Cantrip provides three sum type forms:

- **Enums** (§2.3.1): Tagged unions with pattern matching
- **Unions** (§2.3.2): Untagged unions for C FFI and low-level programming
- **Modals** (§2.3.3): State machines as types with compile-time verification

**Key distinction:** Enums are safe (compiler-enforced exhaustiveness checking), unions are unsafe (programmer must ensure correct field access), and modals add state transition verification.

---

**Definition 9.1 (Enum):** An enum is a sum type (tagged union) with named variants. Formally, `enum E { V₁(T₁), ..., Vₙ(Tₙ) }` denotes the disjoint union `⟦E⟧ = T₁ + T₂ + ... + Tₙ`, where each variant Vᵢ is tagged with a discriminant to distinguish values at runtime.

## 2.3.1 Enums and Pattern Matching

### 2.3.1.1 Overview

**Key innovation/purpose:** Enums provide type-safe discriminated unions, enabling exhaustive pattern matching and making illegal states unrepresentable.

**When to use enums:**

- Representing values that can be one of several distinct alternatives
- State machines with discrete states
- Error handling (Result<T, E> pattern)
- Optional values (Option<T> pattern)
- Message passing systems with different message types
- Modeling business logic with mutually exclusive cases
- Protocol implementations with different message formats

**When NOT to use enums:**

- Data with multiple independent boolean flags → use records with bool fields
- Hierarchical type relationships → use traits
- When all variants share common structure → consider records
- Extensible type families → use traits with multiple implementing types

**Relationship to other features:**

- **Pattern matching**: Enums require exhaustive pattern matching for type safety
- **Generics**: Enums can be generic (e.g., `Option<T>`, `Result<T, E>`)
- **Records**: Enum variants can contain record-like data
- **Modals**: For stateful types, modals provide richer state machine semantics
- **Traits**: Enums can implement traits for polymorphic behavior

### 2.3.1.2 Syntax

#### 2.3.1.2.1 Concrete Syntax

**Enum declaration:**

```ebnf
EnumDecl     ::= "enum" Ident GenericParams? "{" VariantList "}"
VariantList  ::= Variant ("," Variant)* ","?
Variant      ::= Ident Discriminant?                // Unit variant
               | Ident "(" TupleFields ")"          // Tuple variant
               | Ident "{" RecordFields "}"         // Record variant
Discriminant ::= "=" IntLiteral                     // Explicit discriminant value
TupleFields  ::= Type ("," Type)*
RecordFields ::= Field ("," Field)*
```

**Pattern matching:**

```ebnf
Match        ::= "match" Expr "{" MatchArm ("," MatchArm)* ","? "}"
MatchArm     ::= Pattern ("if" Expr)? "->" Expr
Pattern      ::= Ident "." Ident                     // Unit pattern
               | Ident "." Ident "(" PatternList ")" // Tuple pattern
               | Ident "." Ident "{" FieldPat "}"    // Record pattern
               | "_"                                 // Wildcard
```

**Examples:**

```cantrip
enum Status {
    Active,
    Pending,
    Inactive,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(u8, u8, u8),
}
```

#### 2.3.1.2.2 Abstract Syntax

**Enum types:**

```
τ ::= enum E { V₁(τ₁), ..., Vₙ(τₙ) }    (enum type)
    | E                                   (enum type reference)
```

**Enum expressions:**

```
e ::= E.Vᵢ                               (unit variant)
    | E.Vᵢ(e)                            (tuple variant)
    | E.Vᵢ { f₁: e₁, ..., fₙ: eₙ }      (record variant)
```

**Pattern syntax:**

```
p ::= E.Vᵢ                               (unit pattern)
    | E.Vᵢ(p)                            (tuple pattern)
    | E.Vᵢ { f₁: p₁, ..., fₙ: pₙ }      (record pattern)
    | _                                  (wildcard)
```

### 2.3.1.3 Static Semantics

#### 2.3.1.3.1 Well-Formedness Rules

**Enum declaration:**

```
[WF-Enum]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
variants V₁, ..., Vₙ are distinct
──────────────────────────────────────────────
Γ ⊢ enum E { V₁(T₁), ..., Vₙ(Tₙ) } : Type
```

**Generic enum:**

```
[WF-Generic-Enum]
Γ, α₁ : Type, ..., αₘ : Type ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
variants V₁, ..., Vₙ are distinct
────────────────────────────────────────────────────────────────────
Γ ⊢ enum E<α₁, ..., αₘ> { V₁(T₁), ..., Vₙ(Tₙ) } : Type
```

#### 2.3.1.3.2 Type Rules

**Enum variant construction:**

```
[T-Enum-Unit]
enum E { ..., V, ... } well-formed
──────────────────────────────────────────
Γ ⊢ E.V : E

[T-Enum-Tuple]
enum E { ..., V(T), ... } well-formed
Γ ⊢ e : T
──────────────────────────────────────────
Γ ⊢ E.V(e) : E

[T-Enum-Record]
enum E { ..., V { f₁: T₁, ..., fₙ: Tₙ }, ... } well-formed
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
──────────────────────────────────────────
Γ ⊢ E.V { f₁: e₁, ..., fₙ: eₙ } : E
```

**Pattern matching:**

```
[T-Match]
Γ ⊢ e : E
enum E { V₁(T₁), ..., Vₙ(Tₙ) }
∀i. Γ, pᵢ ⊢ eᵢ : U
patterns {p₁, ..., pₙ} exhaustive for E
──────────────────────────────────────────────
Γ ⊢ match e { p₁ => e₁, ..., pₙ => eₙ } : U
```

**Pattern typing:**

```
[T-Pattern-Unit]
enum E { ..., V, ... }
────────────────────────────────────────────
Γ ⊢ E.V : E ⇝ Γ

[T-Pattern-Tuple]
enum E { ..., V(T), ... }
Γ ⊢ p : T ⇝ Γ'
────────────────────────────────────────────
Γ ⊢ E.V(p) : E ⇝ Γ'

[T-Pattern-Record]
enum E { ..., V { f₁: T₁, ..., fₙ: Tₙ }, ... }
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────────
Γ ⊢ E.V { f₁: p₁, ..., fₙ: pₙ } : E ⇝ Γ₁ ∪ ... ∪ Γₙ
```

#### 2.3.1.3.3 Type Properties

**Theorem 9.1 (Exhaustiveness Checking):**

**Definition 9.2 (Exhaustive Patterns):** A set of patterns is exhaustive if every possible value matches at least one pattern.

**Algorithm:**

```
exhaustive(patterns, enum E { V₁, ..., Vₙ }) = {
    1. Collect all constructors {V₁, ..., Vₙ} of enum E
    2. For each constructor Vᵢ:
       - Check if any pattern matches Vᵢ
       - Recursively check nested patterns for variant data
    3. Return true if all constructors covered
}
```

**Example:**

```cantrip
enum Status {
    Active,
    Pending,
    Inactive,
}

function describe(status: Status): str {
    match status {
        Status.Active => "active",
        Status.Pending => "pending",
        Status.Inactive => "inactive",
        // Exhaustive - all variants covered
    }
}

// ERROR: Non-exhaustive
function incomplete(status: Status): str {
    match status {
        Status.Active => "active",
        // ERROR: Missing Pending and Inactive
    }
}
```

**Theorem 9.2 (Discriminant Uniqueness):**

Each variant has a unique discriminant value:

```
∀ i ≠ j. discriminant(Vᵢ) ≠ discriminant(Vⱼ)
```

#### 2.3.1.3.4 Pattern Forms

**Purpose:** Pattern matching supports multiple pattern forms for destructuring different data types. These forms are core to using enums effectively and enable expressive, type-safe data access.

**Literal patterns:**

Literal patterns match exact values:

```cantrip
match number {
    0 => "zero",
    1 => "one",
    2 => "two",
    _ => "many",  // Wildcard
}
```

**Type rule:**

```
[T-Pattern-Literal]
────────────────────────────────────────────
Γ ⊢ n : T ⇝ Γ
```

**Tuple patterns:**

Tuple patterns destructure tuple values and bind variables:

```cantrip
match point {
    (0, 0) => "origin",
    (0, y) => "on y-axis",  // Binds y
    (x, 0) => "on x-axis",  // Binds x
    (x, y) => "point",       // Binds both
}
```

**Type rule:**

```
[T-Pattern-Tuple]
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────
Γ ⊢ (p₁, ..., pₙ) : (T₁, ..., Tₙ) ⇝ Γ₁ ∪ ... ∪ Γₙ
```

**Record patterns:**

Record patterns match record types and destructure fields:

```cantrip
match person {
    Person { name, age: 0..=17 } => "minor: {name}",
    Person { name, age: 18..=64 } => "adult: {name}",
    Person { name, age: 65.. } => "senior: {name}",
}
```

**Type rule:**

```
[T-Pattern-Record]
record R { f₁: T₁, ..., fₙ: Tₙ }
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────
Γ ⊢ R { f₁: p₁, ..., fₙ: pₙ } : R ⇝ Γ₁ ∪ ... ∪ Γₙ
```

**Guard patterns:**

Guard patterns add boolean conditions to patterns:

```cantrip
match value {
    x if x > 0 => "positive",
    x if x < 0 => "negative",
    _ => "zero",
}
```

**Type rule:**

```
[T-Pattern-Guard]
Γ ⊢ p : T ⇝ Γ'
Γ' ⊢ e : bool
────────────────────────────────────────────
Γ ⊢ (p if e) : T ⇝ Γ'
```

**Wildcard pattern:**

The wildcard `_` matches any value without binding:

```
[T-Pattern-Wildcard]
────────────────────────────────────────────
Γ ⊢ _ : T ⇝ Γ
```

### 2.3.1.4 Dynamic Semantics

#### 2.3.1.4.1 Evaluation Rules

**Enum variant construction:**

```
[E-Enum-Unit]
────────────────────────────────────────────
⟨E.V, σ⟩ ⇓ ⟨E.V, σ⟩

[E-Enum-Tuple]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
────────────────────────────────────────────
⟨E.V(e), σ⟩ ⇓ ⟨E.V(v), σ'⟩

[E-Enum-Record]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
────────────────────────────────────────────
⟨E.V { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨E.V { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

**Pattern matching evaluation:**

```
[E-Match-First]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
match(p₁, v) = Some(θ)    // Pattern matches
⟨e₁[θ], σ'⟩ ⇓ ⟨v', σ''⟩
────────────────────────────────────────────
⟨match e { p₁ => e₁, ..., pₙ => eₙ }, σ⟩ ⇓ ⟨v', σ''⟩

[E-Match-Rest]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
match(p₁, v) = None    // Pattern doesn't match
⟨match e { p₂ => e₂, ..., pₙ => eₙ }, σ'⟩ ⇓ ⟨v', σ''⟩
────────────────────────────────────────────
⟨match e { p₁ => e₁, p₂ => e₂, ..., pₙ => eₙ }, σ⟩ ⇓ ⟨v', σ''⟩
```

#### 2.3.1.4.2 Memory Representation

**Enum layout:**

```
enum Layout = {
    discriminant: usize,    // Tag (variant index)
    payload: union {        // Largest variant
        variant₁: T₁,
        ...
        variantₙ: Tₙ,
    }
}
```

**Size calculation:**

```
size(enum E { V₁(T₁), ..., Vₙ(Tₙ) }) =
    size(discriminant) + max(size(T₁), ..., size(Tₙ)) + padding

align(enum E) = max(align(discriminant), align(T₁), ..., align(Tₙ))
```

**Example:**

```cantrip
enum Message {
    Quit,                      // size = 0
    Move { x: i32, y: i32 },   // size = 8
    Write(String),             // size = 24 (String = ptr + len + cap)
}

// Layout (64-bit system):
// ┌────────────┬──────────────────────────┐
// │discriminant│      payload (24 bytes)  │
// │  8 bytes   │      (largest variant)    │
// └────────────┴──────────────────────────┘
// Total: 32 bytes
```

#### 2.3.1.4.3 Operational Behavior

**Discriminant access:**

The discriminant value can be accessed for certain enums:

```cantrip
enum HttpStatus {
    Ok = 200,
    NotFound = 404,
    ServerError = 500,
}

let code: u16 = HttpStatus.Ok as u16  // 200
```

**Type rule for discriminant:**

```
[T-Discriminant]
enum E { V₁ = n₁, ..., Vₖ = nₖ }
───────────────────────────────────
E as IntType valid
```

#### 2.3.1.4.4 Discriminant Size and Optimization

**Discriminant sizing:** The compiler automatically selects the smallest integer type that can represent all variants.

**Default discriminant type selection:**

```
Number of variants  | Discriminant type | Size
--------------------|-------------------|------
1-256 variants      | u8                | 1 byte
257-65536 variants  | u16               | 2 bytes
65537-2³² variants  | u32               | 4 bytes
> 2³² variants      | u64/usize         | 8 bytes
```

**Explicit discriminant representation:**

Users can control discriminant size with `#[repr(...)]` attributes:

```cantrip
// Force specific discriminant size
#[repr(u8)]
enum SmallEnum {
    A,
    B,
    C,
}  // Discriminant guaranteed to be u8

#[repr(u16)]
enum MediumEnum {
    Variant1,
    Variant2,
    // ... up to 65536 variants
}

#[repr(u32)]
enum LargeEnum {
    // ... many variants
}
```

**Discriminant value constraints:**

When explicitly specifying discriminant values, they must:

1. Fit within the discriminant type
2. Be unique across all variants
3. Not exceed the maximum value for the chosen representation

```cantrip
#[repr(u8)]
enum HttpStatus {
    Ok = 200,
    Created = 201,
    NotFound = 404,
    ServerError = 500,
}

// ✗ ERROR: Discriminant value 500 doesn't fit in u8 (max 255)
```

**C-compatible layout:**

The `#[repr(C)]` attribute will C-compatible memory layout:

```cantrip
#[repr(C)]
enum Status {
    Active = 0,
    Inactive = 1,
    Pending = 2,
}

// Memory layout compatible with C enum:
// typedef enum {
//     Active = 0,
//     Inactive = 1,
//     Pending = 2
// } Status
```

**Combined with integer type:**

```cantrip
#[repr(C, u32)]
enum NetworkProtocol {
    TCP = 6,
    UDP = 17,
    ICMP = 1,
}

// C-compatible with explicit 32-bit discriminant
```

**Automatic discriminant assignment:**

If discriminants are not explicitly provided, they start at 0 and increment:

```cantrip
enum Color {
    Red,     // discriminant = 0
    Green,   // discriminant = 1
    Blue,    // discriminant = 2
}
```

**Mixed explicit and implicit:**

```cantrip
enum Priority {
    Low = 0,
    Medium,    // discriminant = 1 (auto-increment)
    High,      // discriminant = 2
    Critical = 100,
    Urgent,    // discriminant = 101
}
```

**Discriminant optimization (niche optimization):**

The compiler performs "niche optimization" for enums with specific patterns:

**Option<T> optimization:**

```cantrip
enum Option<T> {
    Some(T),
    None,
}

// If T is a non-nullable pointer (e.g., Ptr<i32>@Exclusive):
// Size(Option<Ptr<T>@Exclusive>) = Size(Ptr<T>@Exclusive)  // No extra discriminant!
// None represented by null pointer
```

**Memory layout comparison:**

```
Without optimization:
Option<Ptr<i32>@Exclusive>:
┌──────────────┬──────────────┐
│discriminant  │  pointer     │
│  8 bytes     │  8 bytes     │
└──────────────┴──────────────┘
Total: 16 bytes

With niche optimization:
Option<Ptr<i32>@Exclusive>:
┌──────────────┐
│  pointer     │  (None = null, Some(_) = valid ptr)
│  8 bytes     │
└──────────────┘
Total: 8 bytes (50% smaller!)
```

**Other niche-optimizable types:**

```cantrip
// NonZeroU32 has niche (0)
enum Option<NonZeroU32> {
    Some(NonZeroU32),
    None,  // Represented as 0
}

// Size(Option<NonZeroU32>) = Size(NonZeroU32) = 4 bytes

// Bool has two niches (2-255)
enum Option<bool> {
    Some(bool),   // 0 or 1
    None,         // 2 (or any value 2-255)
}
```

**Layout for zero-sized types:**

Enums containing only zero-sized types have minimal layout:

```cantrip
enum ZeroSized {
    A,
    B,
    C,
}

// Size: 1 byte (discriminant only, no payload)
// Alignment: 1 byte
```

**Complex variant layout:**

```cantrip
enum Message {
    Quit,                        // ZST, size = 0
    Move { x: i32, y: i32 },     // size = 8
    Write(String),               // size = 24 (on 64-bit)
    ChangeColor(u8, u8, u8),     // size = 3
}

// Layout calculation:
// Discriminant: u8 (4 variants < 256)
// Payload size: max(0, 8, 24, 3) = 24
// Alignment: max(1, 4, 8, 1) = 8
//
// Memory layout:
// ┌─────────────┬─────────────────────────────┐
// │ disc (1 byte)│  payload (24 bytes)        │
// │ + padding    │  (largest variant)         │
// └─────────────┴─────────────────────────────┘
// Total: 32 bytes (1 + 7 padding + 24)
```

**Discriminant size rule:**

```
[DiscriminantSize]
enum E { V₁, ..., Vₙ }
n = number of variants
─────────────────────────────────
size(discriminant) = ⌈log₂(n) / 8⌉ bytes

(rounded up to next integer size: u8, u16, u32, u64)
```

**Type rule for repr attribute:**

```
[T-ReprExplicit]
#[repr(IntType)]
enum E { V₁, ..., Vₙ }
all discriminants fit in IntType
─────────────────────────────────
discriminant type = IntType
```

**Theorem 9.3 (Discriminant Minimality):** Without explicit `#[repr(...)]`, the compiler selects the smallest integer type sufficient to represent all variants.

**Proof sketch:** The compiler counts the number of variants `n` and selects the smallest unsigned integer type `uₖ` such that `2^(8k) ≥ n`. This minimizes memory usage while ensuring all variants are representable. ∎

**Corollary 9.1:** Enums with ≤256 variants have 1-byte discriminants by default.

**Theorem 9.4 (Niche Optimization Soundness):** Niche optimization preserves type safety. A niche-optimized enum can represent all variants without ambiguity.

**Proof sketch:** Niche optimization uses invalid bit patterns (niches) in the payload type to represent discriminant values. Since these bit patterns are invalid for the payload type, they cannot be confused with valid payload values. The compiler will exhaustive case coverage to prevent accessing niches as valid payloads. ∎

**Corollary 9.2:** `Option<Ptr<T>@Exclusive>` has the same size as `Ptr<T>@Exclusive` for all pointer types.

**Layout attributes summary:**

| Attribute              | Effect                                       |
| ---------------------- | -------------------------------------------- |
| `#[repr(u8)]`          | Force u8 discriminant (max 256 variants)     |
| `#[repr(u16)]`         | Force u16 discriminant (max 65536 variants)  |
| `#[repr(u32)]`         | Force u32 discriminant                       |
| `#[repr(u64)]`         | Force u64 discriminant                       |
| `#[repr(C)]`           | C-compatible layout (discriminant + union)   |
| `#[repr(C, u32)]`      | C-compatible with explicit discriminant size |
| `#[repr(transparent)]` | Single-variant enum, same layout as payload  |

**Transparent representation:**

```cantrip
#[repr(transparent)]
enum Wrapper<T> {
    Value(T),
}

// Size(Wrapper<T>) = Size(T)
// No discriminant overhead for single-variant enum
// Useful for newtype pattern with enums
```

**FFI considerations:**

When interfacing with C code, use `#[repr(C)]` to ensure compatible layout:

```cantrip
// Cantrip
#[repr(C)]
enum FileType {
    Regular = 0,
    Directory = 1,
    Symlink = 2,
}

// Corresponding C enum:
// typedef enum {
//     Regular = 0,
//     Directory = 1,
//     Symlink = 2
// } FileType
```

**Examples:**

**Example 1: Minimal discriminant**

```cantrip
enum TinyEnum {
    A,
    B,
}

// 2 variants → u8 discriminant (1 byte)
// Size: 1 byte (discriminant only, no payload)
```

**Example 2: Niche optimization**

```cantrip
record NonNull<T>(*const T)  // Cannot be null

enum Option<NonNull<i32>> {
    Some(NonNull<i32>),  // Valid pointer
    None,                // Null pointer (niche)
}

// Size: 8 bytes (just the pointer)
// No separate discriminant needed
```

**Example 3: Large discriminant**

```cantrip
enum LargeEnum {
    V0, V1, V2, ..., V300,  // 301 variants
}

// 301 variants > 256 → u16 discriminant (2 bytes)
// Size: 2 bytes (discriminant only, no payload)
```

**Example 4: Explicit discriminant control**

```cantrip
#[repr(u32)]
enum Opcode {
    NOP = 0x00,
    LOAD = 0x01,
    STORE = 0x02,
    ADD = 0x10,
    SUB = 0x11,
    HALT = 0xFF,
}

// Discriminant: u32 (4 bytes)
// Explicit values for protocol compatibility
```

---

**Definition 8.5.1 (Union):** A union is an untagged sum type where all variants occupy the same memory location. Formally, if `union U { f₁: T₁, ..., fₙ: Tₙ }` then `⟦U⟧ = T₁ ⊔ ... ⊔ Tₙ` where ⊔ denotes overlapping union (not disjoint sum), with `size(U) = max(size(T₁), ..., size(Tₙ))` and no runtime discriminant. The active variant is tracked by the programmer, making unions inherently unsafe.

## 2.3.2 Unions

### 2.3.2.1 Overview

**Key innovation/purpose:** Unions provide C-compatible untagged sum types for FFI interoperability, enabling direct mapping to C union types without discriminant overhead while maintaining explicit safety boundaries through the effect system.

**When to use unions:**

- Foreign Function Interface (FFI) with C code requiring unions
- Hardware register access with multiple interpretations
- Memory-constrained systems requiring space optimization
- Type punning for bit-level manipulation (with care)
- Interfacing with POSIX APIs, Win32 APIs, or embedded systems
- Network protocol implementations matching C structures

**When NOT to use unions:**

- Pure Cantrip code (use enums for type-safe tagged unions)
- When discriminant overhead is acceptable (use enums)
- General variant types (use enums with pattern matching)
- When you cannot track the active variant (redesign with enums)
- Public APIs in Cantrip libraries (prefer type-safe alternatives)

**Relationship to other features:**

- **Records (§8)**: Unions are like records but with overlapping storage
- **Enums (§9)**: Enums are tagged unions with discriminants; unions are untagged
- **Permissions (Part III)**: Unions support all permissions (own, mut, immutable, iso)
- **Effects (§21)**: Union field access requires `unsafe.union` effect
- **Contracts (§17)**: Preconditions track active variant for safety
- **FFI (Part XIV)**: Primary use case is C interoperability
- **Memory layout**: Uses `[[repr(C)]]` for C-compatible layout

**Safety model:**

- Unions are **inherently unsafe** - reading wrong variant is undefined behavior
- All field access requires `unsafe.union` effect
- Recommended: wrap unions in records with explicit tag fields (see §8.5.7.1)
- Compiler cannot verify variant correctness (programmer responsibility)

### 2.3.2.2 Syntax

#### 2.3.2.2.1 Concrete Syntax

**Union declaration:**

```ebnf
UnionDecl    ::= "union" Ident GenericParams? "{" FieldList "}"
FieldList    ::= Field (Newline Field)* Newline?
Field        ::= Visibility? Ident ":" Type
Visibility   ::= "public" | "private"
GenericParams ::= "<" TypeParam ("," TypeParam)* ">"
```

**Union construction:**

```ebnf
UnionExpr    ::= Ident "{" FieldInit "}"
FieldInit    ::= Ident ":" Expr
```

**Variant access:**

```ebnf
VariantAccess ::= Expr "." Ident
```

**Examples:**

```cantrip
// Simple union
union Value {
    i: i32
    f: f32
}

// C-compatible union
[[repr(C)]]
union Data {
    integer: i32
    floating: f64
    bytes: [u8; 8]
}

// Generic union
union Either<T, U> {
    left: T
    right: U
}

// Construction (only one variant initialized)
let v = Value { i: 42 }
let d = Data { floating: 3.14 }

// Field access (requires unsafe.union effect)
procedure get_int(v: Value): i32
    uses unsafe.union
{
    v.i  // Unsafe: may read invalid data if f was active
}
```

#### 2.3.2.2.2 Abstract Syntax

**Union types:**

```
τ ::= union U { f₁: τ₁, ..., fₙ: τₙ }    (union type)
    | U                                   (union type reference)
```

**Union expressions:**

```
e ::= U { f: e }                          (union literal, single variant)
    | e.f                                 (variant access, unsafe)
```

**Union patterns:**

```
p ::= U { f: p }                          (union pattern, limited support)
```

**Memory representation:**

```
⟦union U { f₁: T₁, ..., fₙ: Tₙ }⟧ = {
    storage: max(⟦T₁⟧, ..., ⟦Tₙ⟧)      (all variants share storage)
    size: max(size(T₁), ..., size(Tₙ))
    align: max(align(T₁), ..., align(Tₙ))
    discriminant: none                    (no tag)
}
```

### 2.3.2.3 Static Semantics

#### 2.3.2.3.1 Well-Formedness Rules

**[WF-Union] Union Declaration:**

```
[WF-Union]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
variants f₁, ..., fₙ are distinct
n ≥ 1    (at least one variant)
──────────────────────────────────────────────
Γ ⊢ union U { f₁: T₁, ..., fₙ: Tₙ } : Type
```

**Explanation:** Unions must have at least one variant with distinct names. All variant types must be well-formed. Unlike records, variants overlap in memory.

```cantrip
// ✓ Valid: at least one variant
union Data {
    i: i32
    f: f32
}

// ✗ Invalid: zero variants
// union Empty { }

// ✗ Invalid: duplicate variant names
// union Bad {
//     x: i32
//     x: f32
// }
```

**[WF-Union-Generic] Generic Union:**

```
[WF-Union-Generic]
Γ, α₁ : Type, ..., αₘ : Type ⊢ T₁ : Type    ...    Γ, α₁ : Type, ..., αₘ : Type ⊢ Tₙ : Type
variants f₁, ..., fₙ are distinct
────────────────────────────────────────────────────────────────────
Γ ⊢ union U<α₁, ..., αₘ> { f₁: T₁, ..., fₙ: Tₙ } : Type
```

**Explanation:** Unions can be generic over type parameters.

```cantrip
// Generic union
union Either<L, R> {
    left: L
    right: R
}

let e1: Either<i32, String> = Either { left: 42 }
let e2: Either<i32, String> = Either { right: String.from("hello") }
```

**[WF-Union-ZST-Restriction] Zero-Sized Type Restriction:**

```
[WF-Union-ZST-Restriction]
union U { f₁: T₁, ..., fₙ: Tₙ }
∃ i. size(Tᵢ) > 0    (at least one non-ZST variant)
────────────────────────────────────────────
Γ ⊢ U well-formed
```

**Explanation:** Unions with only zero-sized types are disallowed (use unit type `()` instead).

```cantrip
// ✗ Invalid: all variants are ZST
// union AllZero {
//     a: ()
//     b: ()
// }

// ✓ Valid: at least one non-ZST
union Mixed {
    nothing: ()
    something: i32
}
```

#### 2.3.2.3.2 Type Rules

**[T-Union-Construct] Union Construction:**

```
[T-Union-Construct]
union U { f₁: T₁, ..., fᵢ: Tᵢ, ..., fₙ: Tₙ } declared
Γ ⊢ e : Tᵢ
────────────────────────────────────────────
Γ ⊢ U { fᵢ: e } : U
```

**Explanation:** Union construction initializes exactly one variant. The expression type must match the variant's type.

```cantrip
union Data {
    i: i32
    f: f32
    s: String
}

let d1 = Data { i: 42 }                    // ✓ i32 matches i variant
let d2 = Data { f: 3.14 }                  // ✓ f32 matches f variant
let d3 = Data { s: String.from("hello") }  // ✓ String matches s variant

// ✗ ERROR: type mismatch
// let bad = Data { i: 3.14 }              // f32 doesn't match i32
```

**[T-Union-Single-Init] Single Variant Initialization:**

```
[T-Union-Single-Init]
union U { f₁: T₁, ..., fₙ: Tₙ }
U { fᵢ: e, fⱼ: e' } where i ≠ j
────────────────────────────────────────────
ERROR: Multiple variant initialization
```

**Explanation:** Only one variant can be initialized. Initializing multiple variants is an error.

```cantrip
union Data {
    i: i32
    f: f32
}

// ✗ ERROR: Cannot initialize multiple variants
// let bad = Data { i: 42, f: 3.14 }
```

**[T-Union-Access] Variant Access (Unsafe):**

```
[T-Union-Access]
union U { f₁: T₁, ..., fᵢ: Tᵢ, ..., fₙ: Tₙ }
Γ ⊢ e : U
unsafe.union ∈ Γ.effects    (requires unsafe.union effect)
────────────────────────────────────────────
Γ ⊢ e.fᵢ : Tᵢ
```

**Explanation:** Accessing union variants requires the `unsafe.union` effect. Reading the wrong variant is undefined behavior.

```cantrip
union Data {
    i: i32
    f: f32
}

// ✓ Correct: declares unsafe.union effect
procedure read_int(d: Data): i32
    uses unsafe.union
{
    d.i
}

// ✗ ERROR: Missing unsafe.union effect
// procedure bad(d: Data): i32 {
//     d.i  // E7001: Union field access requires unsafe.union effect
// }
```

**[T-Union-Safe-Access] Contract-Guarded Access:**

```
[T-Union-Safe-Access]
union U { f₁: T₁, ..., fᵢ: Tᵢ, ..., fₙ: Tₙ }
record R { tag: E, data: U }    (tagged wrapper)
Γ ⊢ self : R
Γ ⊢ self.tag == Eᵢ    (precondition: correct tag)
unsafe.union ∈ Γ.effects
────────────────────────────────────────────
Γ ⊢ self.data.fᵢ : Tᵢ    (safe access)
```

**Explanation:** Contracts can document which variant is active, making access safer (but still requires unsafe.union).

```cantrip
enum Tag { Int, Float }

union Data {
    i: i32
    f: f32
}

record Tagged {
    tag: Tag
    data: Data

    procedure get_int(self: Self): i32
        must self.tag == Tag.Int    // Precondition: tag must match
        uses unsafe.union
    {
        self.data.i  // Safe because precondition guarantees correct variant
    }
}
```

**[T-Union-Assign] Variant Assignment:**

```
[T-Union-Assign]
union U { f₁: T₁, ..., fᵢ: Tᵢ, ..., fₙ: Tₙ }
Γ ⊢ e₁ : mut U
Γ ⊢ e₂ : Tᵢ
unsafe.union ∈ Γ.effects
────────────────────────────────────────────
Γ ⊢ e₁.fᵢ = e₂ : ()    (overwrites all variants)
```

**Explanation:** Assigning to a union variant requires mutable access and overwrites the entire union.

```cantrip
union Data {
    i: i32
    f: f32
}

procedure update(d: mut Data)
    uses unsafe.union
{
    d.i = 42     // Overwrites entire union (including f)
    d.f = 3.14   // Overwrites i
}
```

**[T-Union-Permission] Permission Application:**

```
[T-Union-Permission]
union U { f₁: T₁, ..., fₙ: Tₙ }
π ∈ {own, mut, immutable, iso}
────────────────────────────────────────────
Γ ⊢ π U : Type
```

**Explanation:** Unions support all permission qualifiers.

```cantrip
union Data {
    i: i32
    f: f32
}

procedure consume(data: own Data) { }           // Owned
procedure modify(data: mut Data) { }            // Mutable
procedure read(data: Data) { }                  // Immutable
procedure isolate(data: iso Data) { }           // Isolated
```

**[T-Union-Copy] Copy Semantics:**

```
[T-Union-Copy]
union U { f₁: T₁, ..., fₙ: Tₙ }
T₁ : Copy ∧ ... ∧ Tₙ : Copy
────────────────────────────────────────────
U : Copy
```

**Explanation:** A union is `Copy` if and only if all variants are `Copy`.

```cantrip
// Copy union (all variants are Copy)
union CopyData {
    i: i32      // Copy
    f: f32      // Copy
    b: bool     // Copy
}

let d1 = CopyData { i: 42 }
let d2 = d1  // Copy (bitwise copy)
// d1 still usable

// Non-Copy union (String is not Copy)
union NonCopyData {
    i: i32
    s: String   // Not Copy
}

let d1 = NonCopyData { s: String.from("hello") }
let d2 = d1  // Move
// d1 no longer usable
```

**[T-Union-Move] Move Semantics:**

```
[T-Union-Move]
union U { f₁: T₁, ..., fₙ: Tₙ }
∃ i. Tᵢ : ¬Copy    (at least one non-Copy variant)
Γ ⊢ e₁ : U
────────────────────────────────────────────
Γ ⊢ let x = e₁ ⟹ e₁ moved, cannot be used
```

**Explanation:** Unions with any non-Copy variant are moved by default.

```cantrip
union Data {
    i: i32
    s: String  // Not Copy
}

let d1 = Data { s: String.from("hello") }
let d2 = d1  // Move entire union
// d1 is now invalid, even though we initialized with String

// This is safe because:
// - We don't know which variant is active
// - Must assume String could be active
// - Moving entire union is safe
```

**[T-Union-Size] Size Calculation:**

```
[T-Union-Size]
union U { f₁: T₁, ..., fₙ: Tₙ }
────────────────────────────────────────────
size(U) = max(size(T₁), ..., size(Tₙ))
align(U) = max(align(T₁), ..., align(Tₙ))
```

**Explanation:** Union size is the maximum size of any variant. All variants share the same storage.

```cantrip
union Data {
    i: i32       // size=4, align=4
    f: f64       // size=8, align=8
    bytes: [u8; 16]  // size=16, align=1
}

// size(Data) = max(4, 8, 16) = 16
// align(Data) = max(4, 8, 1) = 8
// Total: 16 bytes aligned to 8-byte boundary
```

**[T-Union-Repr-C] C-Compatible Layout:**

```
[T-Union-Repr-C]
[[repr(C)]]
union U { f₁: T₁, ..., fₙ: Tₙ }
────────────────────────────────────────────
layout(U) matches C union U { T₁ f₁; ...; Tₙ fₙ; }
```

**Explanation:** `[[repr(C)]]` ensures the union has the same layout as a C union.

```cantrip
// Cantrip
[[repr(C)]]
union Value {
    i: i32
    f: f32
}

// Equivalent C
// union Value {
//     int32_t i;
//     float f;
// }
```

**[T-Union-Nested] Nested Unions:**

```
[T-Union-Nested]
union U₁ { ... }
union U₂ { f: U₁, ... }
────────────────────────────────────────────
Γ ⊢ U₂ : Type    (unions can nest)
```

**Explanation:** Unions can contain other unions.

```cantrip
union Inner {
    i: i32
    f: f32
}

union Outer {
    inner: Inner
    s: String
}

let o = Outer { inner: Inner { i: 42 } }
```

**[T-Union-Generic-Inst] Generic Union Instantiation:**

```
[T-Union-Generic-Inst]
union U<α₁, ..., αₖ> { f₁: T₁, ..., fₙ: Tₙ } declared
Γ ⊢ S₁ : Type    ...    Γ ⊢ Sₖ : Type
────────────────────────────────────────────
Γ ⊢ U<S₁, ..., Sₖ> : Type
```

**Explanation:** Generic unions are instantiated by providing concrete types.

```cantrip
union Either<L, R> {
    left: L
    right: R
}

let e1: Either<i32, String> = Either { left: 42 }
let e2: Either<bool, f64> = Either { right: 3.14 }
```

**[T-Union-Subtyping] No Structural Subtyping:**

```
[T-Union-Subtyping]
union U₁ { f₁: T₁, ..., fₙ: Tₙ }
union U₂ { f₁: T₁, ..., fₙ: Tₙ }
U₁ ≠ U₂
────────────────────────────────────────────
U₁ ≢ U₂    (no structural equivalence)
```

**Explanation:** Unions use nominal typing. Identical structure doesn't imply type equivalence.

```cantrip
union A {
    i: i32
    f: f32
}

union B {
    i: i32
    f: f32
}

let a = A { i: 42 }
// let b: B = a  // ✗ ERROR: type mismatch (A ≠ B)
```

**[T-Union-Pattern] Limited Pattern Matching:**

```
[T-Union-Pattern]
union U { f₁: T₁, ..., fₙ: Tₙ }
────────────────────────────────────────────
Pattern matching on unions is restricted (see §8.5.3.3)
```

**Explanation:** Direct pattern matching on unions is limited because the active variant is unknown.

```cantrip
union Data {
    i: i32
    f: f32
}

// ✗ Invalid: Cannot pattern match directly on union
// match data {
//     Data { i } => ...,  // How do we know i is active?
//     Data { f } => ...,
// }

// ✓ Valid: Pattern match on tagged wrapper
enum Tag { Int, Float }
record Tagged { tag: Tag, data: Data }

match tagged {
    Tagged { tag: Tag.Int, data } => {
        // Safe to access data.i
    },
    Tagged { tag: Tag.Float, data } => {
        // Safe to access data.f
    },
}
```

#### 2.3.2.3.3 Type Properties

**Theorem 8.5.1 (Union Nominality):**

Unions with identical variant names and types are distinct types. For any unions `union A { f: T }` and `union B { f: T }`, `A ≠ B` implies `A ≢ B`.

**Proof sketch:** The type system assigns each union declaration a unique nominal identity. Even if variant types are structurally identical, the nominal identities differ. ∎

**Corollary 8.5.1:** Union types provide type safety through nominal distinction.

**Theorem 8.5.2 (Size Maximality):**

For union U with variants of types T₁, ..., Tₙ:

```
size(U) = max(size(T₁), ..., size(Tₙ))
```

**Proof sketch:** All variants occupy the same memory location. The union must be large enough to hold any variant. Therefore, size must be the maximum. ∎

**Corollary 8.5.2:** Unions are space-efficient when variants have similar sizes.

**Theorem 8.5.3 (Copy Capability):**

A union U implements Copy if and only if all variants implement Copy:

```
union U { f₁: T₁, ..., fₙ: Tₙ } : Copy
⟺
T₁ : Copy ∧ ... ∧ Tₙ : Copy
```

**Semantics:** Unions pass by permission by default. `.copy()` performs bitwise copy when all variants are Copy-capable.

**Proof sketch:**

- (⇒) If U is Copy, bitwise copy must be valid. Since any variant could be active, all must support bitwise copy, thus all Tᵢ : Copy.
- (⇐) If all Tᵢ : Copy, bitwise copy is safe regardless of active variant, thus U : Copy.
  ∎

**Theorem 8.5.4 (Unsafe Access):**

All union variant access is inherently unsafe without additional invariants:

```
∀ u : U, ∀ f : variant of U.
access(u.f) is safe ⟺ f is the active variant
```

**Proof sketch:** Reading a variant that is not active results in type confusion - interpreting memory of type Tᵢ as type Tⱼ where i ≠ j, which is undefined behavior in general. Only external invariants (tags, contracts) can establish safety. ∎

**Corollary 8.5.3:** Unions require `unsafe.union` effect for all field access.

**Theorem 8.5.5 (Layout Compatibility):**

For unions marked `[[repr(C)]]`, the memory layout matches the equivalent C union:

```
[[repr(C)]] union U { f₁: T₁, ..., fₙ: Tₙ } in Cantrip
⟺
union U { T₁ f₁; ...; Tₙ fₙ; } in C
```

**Proof sketch:** The `[[repr(C)]]` attribute instructs the compiler to follow C's union layout rules: overlapping storage, maximum size, maximum alignment, no padding between variants (only trailing padding for alignment). ∎

**Theorem 8.5.6 (Zero Runtime Overhead):**

Unions have zero abstraction cost compared to raw memory reinterpretation:

```
cost(union U) = cost(reinterpret_cast in C++)
```

**Proof sketch:** Unions are a compile-time construct. At runtime, they are just memory blocks with maximum size. Field access is a zero-cost offset calculation (always offset 0). No runtime checks, no discriminant, no indirection. ∎

### 2.3.2.4 Dynamic Semantics

#### 2.3.2.4.1 Evaluation Rules

**[E-Union-Construct] Union Construction:**

```
[E-Union-Construct]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
────────────────────────────────────────────
⟨U { f: e }, σ⟩ ⇓ ⟨U { f: v }, σ'⟩
```

**Explanation:** Union construction evaluates the initializer expression and stores it in the union's storage.

**[E-Union-Access] Variant Access:**

```
[E-Union-Access]
⟨e, σ⟩ ⇓ ⟨U { fᵢ: v }, σ'⟩
────────────────────────────────────────────
⟨e.fⱼ, σ'⟩ ⇓ ⟨interpret(storage, Tⱼ), σ'⟩
```

**Explanation:** Accessing variant fⱼ interprets the union's storage as type Tⱼ, regardless of which variant was initialized. If i ≠ j, this is undefined behavior.

**[E-Union-Assign] Variant Assignment:**

```
[E-Union-Assign]
⟨e₁, σ⟩ ⇓ ⟨u, σ₁⟩    u : mut U
⟨e₂, σ₁⟩ ⇓ ⟨v, σ₂⟩   v : Tᵢ
────────────────────────────────────────────
⟨e₁.fᵢ = e₂, σ⟩ ⇓ ⟨(), σ₂[u.storage := v]⟩
```

**Explanation:** Assigning to a variant overwrites the entire union's storage.

#### 2.3.2.4.2 Memory Representation

**Union memory layout:**

```
union U { f₁: T₁, f₂: T₂, ..., fₙ: Tₙ }

All variants share the same memory location:
┌────────────────────────────────┐
│  max(size(T₁), ..., size(Tₙ))  │  All variants overlap
└────────────────────────────────┘
offset 0: f₁ starts here
offset 0: f₂ starts here (overlaps f₁)
...
offset 0: fₙ starts here (overlaps all)

Size: max(size(T₁), ..., size(Tₙ))
Alignment: max(align(T₁), ..., align(Tₙ))
```

**Example: Different-sized variants:**

```cantrip
union Data {
    byte: u8       // size=1, align=1
    word: u32      // size=4, align=4
    dword: u64     // size=8, align=8
}

Memory layout:
┌──────────────────────────────┐
│         8 bytes              │  All variants overlap at offset 0
└──────────────────────────────┘
Size: max(1, 4, 8) = 8 bytes
Alignment: max(1, 4, 8) = 8 bytes
```

**C-compatible layout:**

```cantrip
[[repr(C)]]
union CData {
    i: i32
    f: f32
    d: f64
}

// Matches C: union CData { int32_t i; float f; double d; };
Size: 8 bytes, Alignment: 8 bytes
```

#### 2.3.2.4.3 Operational Behavior

**Initialization and overwrites:**

```cantrip
union Data {
    i: i32
    f: f32
}

let d1 = Data { i: 42 }
// Memory: [42, 0, 0, 0] (little-endian)

var d2 = Data { i: 42 }
d2.f = 3.14
// Memory: [0xC3, 0xF5, 0x48, 0x40] - integer overwritten!
```

**Undefined behavior:**

```cantrip
let d = Data { i: 42 }
let x = d.f  // UB! Reading float when int was stored
```

**Type punning (legal but unsafe):**

```cantrip
union FloatPun {
    f: f32
    bits: u32
}

procedure float_to_bits(f: f32): u32
    uses unsafe.union
{
    FloatPun { f: f }.bits
}
```

### 2.3.2.5 Safety and Integration

#### 2.3.2.5.1 Effect System Integration

**Effect: `unsafe.union`**

All union variant access requires the `unsafe.union` effect:

```cantrip
effect unsafe.union {
    doc = "Access to union variants without discriminant checking"
    category = "unsafe"
}

union Data {
    i: i32
    f: f32
}

// ✓ Correct: declares unsafe.union
procedure read_int(d: Data): i32
    uses unsafe.union
{
    d.i
}

// ✗ ERROR E7001: Missing unsafe.union effect
// procedure bad(d: Data): i32 {
//     d.i
// }
```

**Effect hierarchy:**

```
unsafe
├── unsafe.ptr        (raw pointer dereferencing)
├── unsafe.union      (union variant access)
├── unsafe.cast       (type punning casts)
├── unsafe.asm        (inline assembly)
└── unsafe.transmute  (memory reinterpretation)

ffi
├── ffi.call          (calling C functions)
└── ffi.union         (FFI union operations)
```

**Effect composition:**

```cantrip
// FFI procedure using unions
procedure call_c_function(): i32
    uses ffi.call, unsafe.union
{
    let result = external_c_func()
    result.data.value  // Requires both ffi.call and unsafe.union
}
```

#### 2.3.2.5.2 Contract-Based Safety

**Pattern: Tagged union wrapper:**

```cantrip
enum ValueKind { Integer, Floating }

union ValueData {
    i: i64
    f: f64
}

record Value {
    kind: ValueKind
    data: ValueData

    procedure new_int(value: i64): own Value {
        own Value { kind: ValueKind.Integer, data: ValueData { i: value } }
    }

    procedure get_int(self: Self): i64
        must self.kind == ValueKind.Integer
        uses unsafe.union
    {
        self.data.i  // Safe: precondition ensures i is active
    }

    procedure as_int(self: Self): Option<i64>
        uses unsafe.union
    {
        match self.kind {
            ValueKind.Integer => Option.Some(self.data.i),
            _ => Option.None,
        }
    }
}
```

#### 2.3.2.5.3 Permission System Integration

**Unions support all permissions:**

```cantrip
union Data {
    i: i32
    f: f32
}

procedure consume(data: own Data) { }        // Owned
procedure modify(data: mut Data) { }         // Mutable
procedure read(data: Data) { }               // Immutable
procedure isolate(data: iso Data) { }        // Isolated
```

**Copy semantics:**

```cantrip
// Copy union (all variants Copy)
union CopyData {
    i: i32
    f: f32
}

// Non-Copy union (String not Copy)
union NonCopyData {
    i: i32
    s: String
}
```

#### 2.3.2.5.4 Region Allocation Support

Unions support region allocation for temporary data:

```cantrip
union Data { i: i32, f: f64 }

procedure process_batch(items: [Item]): Result
    uses alloc.region
{
    region temp {
        var buffer = Data { i: 0 }
        for item in items {
            buffer.i = process(item)
        }
        // buffer automatically freed at region end
    }
}
```

### 2.3.2.6 Advanced Features

#### 2.3.2.6.1 FFI Integration

**C structure with embedded union:**

```cantrip
// C: struct Event { uint32_t type; union { ... } data; }

[[repr(C)]]
record Event {
    type_id: u32
    data: EventData
}

[[repr(C)]]
union EventData {
    mouse: MouseEvent
    keyboard: KeyboardEvent
}

[[repr(C)]]
record MouseEvent { x: i32, y: i32 }

[[repr(C)]]
record KeyboardEvent { key: u8 }

extern "C" {
    procedure poll_event(event: mut Event): bool
        uses ffi.call, unsafe.union
}

procedure handle_event(event: Event)
    uses unsafe.union
{
    match event.type_id {
        1 => println("Mouse: ({}, {})", event.data.mouse.x, event.data.mouse.y),
        2 => println("Key: {}", event.data.keyboard.key),
        _ => {},
    }
}
```

**Win32 LARGE_INTEGER:**

```cantrip
// C: typedef union { struct { DWORD LowPart; LONG HighPart; }; LONGLONG QuadPart; } LARGE_INTEGER;

[[repr(C)]]
union LargeInteger {
    parts: LargeIntegerParts
    quad: i64
}

[[repr(C)]]
record LargeIntegerParts { low: u32, high: i32 }

extern "C" {
    procedure QueryPerformanceCounter(counter: mut LargeInteger): bool
        uses ffi.call, unsafe.union
}

procedure get_performance_counter(): i64
    uses ffi.call, unsafe.union
{
    var counter = LargeInteger { quad: 0 }
    QueryPerformanceCounter(mut counter)
    counter.quad
}
```

#### 2.3.2.6.2 Hardware Register Access

**Memory-mapped I/O register:**

```cantrip
[[repr(C)]]
union ControlRegister {
    raw: u32
    bits: ControlBits
}

[[repr(C, packed)]]
record ControlBits { enable: u8, mode: u8, interrupt: u8 }

const CONTROL_REG: *mut ControlRegister = 0x4000_0000 as *mut ControlRegister

procedure enable_device()
    uses unsafe.ptr, unsafe.union
{
    var reg = *CONTROL_REG
    reg.bits.enable = 1
    reg.bits.mode = 3
    *CONTROL_REG = reg
}
```

#### 2.3.2.6.3 Type Punning and Bit Manipulation

**Float/int conversion:**

```cantrip
union FloatInt {
    f: f32
    i: u32
}

procedure float_bits(f: f32): u32
    uses unsafe.union
{
    FloatInt { f: f }.i
}

procedure is_nan(f: f32): bool
    uses unsafe.union
{
    let bits = float_bits(f)
    ((bits >> 23) & 0xFF) == 0xFF && (bits & 0x7F_FFFF) != 0
}
```

**Endianness detection:**

```cantrip
union EndianTest { value: u32, bytes: [u8; 4] }

procedure is_little_endian(): bool
    uses unsafe.union
{
    EndianTest { value: 1 }.bytes[0] == 1
}
```

### 2.3.2.7 Comparison to Alternatives

| Feature              | Union               | Enum                         | Record          | Type Alias      |
| -------------------- | ------------------- | ---------------------------- | --------------- | --------------- |
| **Memory layout**    | Overlapping         | Tagged + largest variant     | Sequential      | Transparent     |
| **Discriminant**     | None                | Automatic                    | N/A             | N/A             |
| **Type safety**      | Unsafe              | Safe                         | Safe            | Transparent     |
| **C FFI**            | Perfect match       | Tag overhead                 | Perfect match   | Depends         |
| **Pattern matching** | Limited             | Full support                 | Full support    | Depends         |
| **Space efficiency** | max(variants)       | discriminant + max(variants) | sum(fields)     | Underlying type |
| **Use case**         | C interop, hardware | General variants             | Structured data | Type branding   |

**When to use each:**

- **Union**: C FFI, hardware registers, space-critical optimization with external tags
- **Enum**: General Cantrip code requiring variant types
- **Record**: Structured data with all fields present
- **Type Alias**: Simple naming, no runtime representation

### 2.3.2.8 Safety Guidelines

**DO:**

- ✓ Always wrap unions in records with explicit tag fields
- ✓ Use contracts to document which variant is active
- ✓ Declare `uses unsafe.union` for all variant access
- ✓ Use `[[repr(C)]]` for FFI unions
- ✓ Prefer enums over unions in pure Cantrip code
- ✓ Use unions only when interfacing with C or hardware
- ✓ Document which variant is active in comments

**DON'T:**

- ✗ Access union variants without knowing which is active
- ✗ Use unions for general variant types (use enums)
- ✗ Assume unions are Copy unless all variants are Copy
- ✗ Forget to handle owned types (String, Vec) in unions properly
- ✗ Use unions without clear external invariants
- ✗ Pattern match directly on unions (match on tags instead)
- ✗ Ignore the `unsafe.union` effect requirement

**Common pitfalls:**

```cantrip
// ❌ BAD: No tag, no way to know active variant
union Bad {
    i: i32
    s: String
}

let x = Bad { s: String.from("hello") }
// Later... which variant is active? Can't tell!

// ✓ GOOD: Tagged wrapper
enum Kind { Int, Str }

union GoodData {
    i: i32
    s: String
}

record Good {
    kind: Kind
    data: GoodData
}
```

---

**Definition 12.1 (Modal):** A modal is a type with an explicit finite state machine, where each state @Sᵢ can have different data fields and the compiler enforces valid state transitions. Formally, a modal M = (S, Σ, δ, s₀) where S is a finite set of states, Σ is the alphabet of procedures, δ: S × Σ → S is the transition function, and s₀ ∈ S is the initial state. Modals bring state machine verification into the type system, ensuring compile-time correctness for stateful resources and business logic.

## 2.3.3 Modals: State Machines as Types

### 2.3.3.1 Overview

**Key innovation/purpose:** Modals make state machines first-class types with compile-time verification, ensuring that state-dependent operations are only called in valid states and all state transitions are explicit and type-checked.

**When to use modals:**

- Resources with lifecycles (files, database connections, network sockets)
- Stateful business logic (orders, shopping carts, payment flows)
- Protocol implementations (TCP handshakes, authentication state)
- Types where "state matters" for correctness
- API wrappers requiring initialization/cleanup sequences
- Workflow engines and state-based processes
- Transaction management (pending, committed, rolled back)

**When NOT to use modals:**

- Simple data with no state machine → use records (§8)
- Discrete alternatives without transitions → use enums (§9)
- Mathematical types (vectors, matrices, points) → use records
- Configuration objects → use records
- Collections → use Vec, HashMap, etc.
- When state doesn't affect available operations

**Relationship to other features:**

- **Records**: Modals extend records with state-dependent fields and procedures
- **Enums**: Modals are like enums but with explicit transitions and state-dependent data
- **Procedures**: Modal procedures specify state transitions via `@State` annotations
- **Contracts**: State invariants verified via `requires`/`ensures`
- **Linear types**: Modal values use linear state transfer (no copying state)
- **Verification**: Reachability and exhaustiveness checked statically

### 2.3.3.2 Syntax

#### 2.3.3.2.1 Concrete Syntax

**Modal declaration:**

```ebnf
ModalDecl    ::= "modal" Ident GenericParams? "{" ModalItem* "}"
ModalItem    ::= StateDecl | CommonDecl | ModalProcedure
StateDecl    ::= "state" Ident "{" FieldList "}"
CommonDecl   ::= "common" "{" FieldList "}"
ModalProcedure ::= "procedure" Ident "@" StatePattern "(" ParamList ")" "->" "@" Ident ContractClauses? Block
StatePattern ::= Ident
               | "(" Ident ("|" Ident)* ")"    // Union of states
```

**State annotations:**

```ebnf
StateAnnot   ::= Type "@" Ident
```

**Examples:**

```cantrip
modal File {
    state Closed {
        path: String
    }

    state Open {
        path: String
        handle: FileHandle
        position: usize
    }

    procedure open@Closed() => @Open
        uses fs.read
    {
        let handle = FileSystem.open(self.path)?
        Open {
            path: self.path,
            handle: handle,
            position: 0,
        }
    }

    procedure close@Open() => @Closed
        uses fs.close
    {
        self.handle.close()
        Closed { path: self.path }
    }
}
```

#### 2.3.3.2.2 Abstract Syntax

**Modal types:**

```
τ ::= modal M { @S₁, ..., @Sₙ }    (modal type with states)
    | M@S                           (modal in specific state)
```

**State machine:**

```
M = (S, Σ, δ, s₀, Fields)
where:
  S = {@S₁, ..., @Sₙ}              (finite set of states)
  Σ = {m₁, ..., mₖ}                (procedures)
  δ : S × Σ → S                     (transition function)
  s₀ ∈ S                            (initial state)
  Fields: S → (Name × Type)*        (state-dependent fields)
```

**Transition syntax:**

```
δ ::= m@Sᵢ(...) => @Sⱼ { e }      (transition from Sᵢ to Sⱼ via m)
```

### 2.3.3.3 Static Semantics

#### 2.3.3.3.1 Well-Formedness Rules

**Modal declaration:**

```
[WF-Modal]
∀i. state @Sᵢ { fields } well-formed
∀j. procedure mⱼ@S_src => @S_tgt well-formed
@S_src, @S_tgt ∈ S
────────────────────────────────────────────
modal M { @S₁, ..., @Sₙ; m₁, ..., mₖ } well-formed
```

**State declaration:**

```
[WF-State]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
field names distinct
────────────────────────────────────────
state @S { f₁: T₁; ...; fₙ: Tₙ } well-formed
```

**Modal procedure:**

```
[WF-Modal-Procedure]
modal M { ..., state @S₁, ..., state @S₂, ... }
Γ, self: M@S₁ ⊢ body : M@S₂
────────────────────────────────────────────────
procedure m@S₁(...) => @S₂ { body } well-formed
```

**Reachability:**

```
[WF-Reachability]
modal M = (S, Σ, δ, s₀, Fields)
∀ @S ∈ S. ∃ path from s₀ to @S in transition graph
────────────────────────────────────────────────
M satisfies reachability
```

**Determinism:**

```
[WF-Determinism]
∀ @S ∈ S, m ∈ Σ. |{@S' | δ(@S, m) = @S'}| ≤ 1
────────────────────────────────────────────────
M is deterministic
```

#### 2.3.3.3.2 Type Rules

**State-annotated type:**

```
[T-State-Type]
Γ ⊢ M : modal
@S ∈ states(M)
────────────────────────────────
Γ ⊢ M@S : Type
```

**Procedure invocation:**

```
[T-Modal-Procedure]
Γ ⊢ self : M@S₁
procedure m@S₁(...) => @S₂ in M
Γ ⊢ args : T
────────────────────────────────────────
Γ ⊢ self.m(args) : M@S₂
```

**State transition:**

```
[T-State-Transition]
Γ, Σ ⊢ e : M@S₁
Σ ⊢ (@S₁, m, @S₂) ∈ δ
Γ ⊢ args : T
────────────────────────────────────────
Γ, Σ ⊢ e.m(args) : M@S₂
```

**Invalid transition:**

```
[T-Invalid-Transition]
Γ ⊢ self : M@S₁
procedure m@S₂(...) => @S₃ in M
S₁ ≠ S₂
────────────────────────────────────────
ERROR E10020: Procedure m not available in state @S₁
```

**Field access (state-dependent):**

```
[T-Modal-Field]
Γ ⊢ self : M@S
field f : T in Fields(@S)
────────────────────────────────────────
Γ ⊢ self.f : T

[T-Modal-Field-Invalid]
Γ ⊢ self : M@S
field f ∉ Fields(@S)
────────────────────────────────────────
ERROR E10021: Field f not available in state @S
```

#### 2.3.3.3.3 State Invariants

**State invariant declaration:**

```
[T-State-Invariant]
state @S {
    fields...
    invariant P(fields)
}
────────────────────────────────────────
invariant(@S) = P
```

**Invariant preservation:**

```
[T-Invariant-Preservation]
procedure m@S₁(...) => @S₂
    must P
    will Q
P ∧ invariant(@S₁) ⟹ Q ∧ invariant(@S₂)
────────────────────────────────────────
Transition preserves invariants
```

**Example:**

```cantrip
modal Account {
    state Active {
        balance: i64
        invariant self.balance >= 0
    }

    state Frozen {
        balance: i64
        reason: String
        invariant self.balance >= 0
    }

    procedure withdraw@Active(amount: i64) => @Active
        must amount > 0
        must self.balance >= amount  // Preserves invariant
        will self.balance == old(self.balance) - amount
    {
        Active { balance: self.balance - amount }
    }
}
```

### 2.3.3.4 Dynamic Semantics

#### 2.3.3.4.1 State Transitions

**Transition evaluation:**

```
[E-Modal-Transition]
⟨self, σ⟩ ⇓ ⟨v_self : M@S₁, σ₁⟩
procedure m@S₁(...) => @S₂ in M
⟨body[self ↦ v_self], σ₁⟩ ⇓ ⟨v_result : M@S₂, σ₂⟩
────────────────────────────────────────────────────
⟨self.m(...), σ⟩ ⇓ ⟨v_result, σ₂⟩
```

**State construction:**

```
[E-State-Construction]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
────────────────────────────────────────────────────
⟨@S { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨M@S { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

#### 2.3.3.4.2 Linear State Transfer

**Linearity rule:**

Modal values use linear semantics—each value must be used exactly once:

```
[Linear-Use]
Γ ⊢ x : M@S
x used exactly once in scope
────────────────────────────────
Well-formed

[Linear-Use-Error]
Γ ⊢ x : M@S
x used zero or multiple times
────────────────────────────────
ERROR E10022: Linear value must be used exactly once
```

**Example:**

```cantrip
let file = File.new("data.txt")  // File@Closed
let file2 = file   // Moves ownership
// let x = file.open()  // ERROR E10022: file already moved

let conn = Connection.new("localhost", 8080)
// conn must be used exactly once
let conn = conn.connect()?
```

**Rebinding pattern:**

The common pattern is to rebind the same variable name:

```cantrip
let file = File.new("data.txt")  // File@Closed
let file = file.open()?           // Rebind: now File@Open
let data = file.read(1024)?       // Still File@Open (returns (data, file))
let file = file.close()           // Back to File@Closed
```

#### 2.3.3.4.3 Memory Layout

**Modal memory representation:**

A modal value consists of:

1. **Discriminant** (which state)
2. **Payload** (fields for that state)

```
Modal layout (similar to enum):
┌──────────────┬─────────────────────────┐
│ discriminant │  state payload          │
│  (which @S)  │  (max size of all states)│
└──────────────┴─────────────────────────┘
```

**Size calculation:**

```
size(modal M { @S₁, ..., @Sₙ }) =
    size(discriminant) + max(size(@S₁), ..., size(@Sₙ)) + padding

align(M) = max(align(discriminant), align(@S₁), ..., align(@Sₙ))
```

**Example:**

```cantrip
modal Example {
    state Small { x: u8; }           // 1 byte
    state Large { data: [u8; 100]; } // 100 bytes
}
// Total size ≈ 104 bytes (discriminant + largest state)
```

### 2.3.3.5 Verification

#### 2.3.3.5.1 Verification Conditions

**State reachability:**

Compiler verifies all states are reachable from initial state:

```
VC-Reachability:
∀ @S ∈ states(M). ∃ path s₀ →* @S
```

**Diagnostic:** `E10023` — State @S is unreachable from initial state

**Transition validity:**

All transitions must target valid states:

```
VC-Valid-Target:
∀ (procedure m@S₁ => @S₂). @S₁, @S₂ ∈ states(M)
```

**Diagnostic:** `E10024` — Transition targets non-existent state @S

#### 2.3.3.5.2 Static Checking

**Exhaustiveness checking:**

When matching on modal states, all states must be covered:

```cantrip
function describe(conn: Connection) => str {
    match conn {
        Connection@Disconnected => "disconnected",
        Connection@Connected => "connected",
        // Compiler will exhaustiveness
    }
}
```

**Type rule:**

```
[T-Modal-Match]
Γ ⊢ e : M
patterns cover all states in M
∀i. Γ, pᵢ ⊢ eᵢ : T
────────────────────────────────────────
Γ ⊢ match e { p₁ => e₁, ..., pₙ => eₙ } : T
```

#### 2.3.3.5.3 Runtime Checks

**Invariant checking:**

When enabled, runtime checks verify state invariants:

```cantrip
#[runtime_checks(true)]
modal SafeAccount {
    state Active {
        balance: i64
        invariant self.balance >= 0
    }

    procedure withdraw@Active(amount: i64) => @Active {
        Active { balance: self.balance - amount }
        // Runtime check: balance >= 0
    }
}
```

### 2.3.3.6 Advanced Features

#### 2.3.3.6.1 State Unions

**Syntax:**

Procedures can accept multiple source states using union syntax:

```cantrip
modal OrderProcessor {
    state Draft { items: Vec<Item>; }
    state Submitted { order_id: u64; items: Vec<Item>; }
    state Cancelled { order_id: u64; reason: String; }

    // Can cancel from either Draft or Submitted
    procedure cancel@(Draft | Submitted)(reason: String) => @Cancelled {
        let order_id = match self {
            Draft { .. } => generate_id(),
            Submitted { order_id, .. } => order_id,
        }
        Cancelled { order_id, reason }
    }
}
```

**Type rule:**

```
[T-State-Union]
procedure m@(@S₁ | ... | @Sₙ)(...) => @S
Γ ⊢ self : M@Sᵢ    where Sᵢ ∈ {@S₁, ..., @Sₙ}
────────────────────────────────────────────────
Γ ⊢ self.m(...) : M@S
```

#### 2.3.3.6.2 Common Fields

**Purpose:** Fields that exist in ALL states can be declared once in a `common` block.

**Syntax:**

```cantrip
modal HttpRequest {
    common {
        url: String
        headers: HashMap<String, String>
    }

    state Building { }

    state Sent {
        response_code: u16
    }

    state Completed {
        response_code: u16
        body: Vec<u8>
    }

    procedure send@Building() => @Sent
        uses net.http
    {
        let code = http_send(self.url, self.headers)?
        Sent { response_code: code }
    }
}
```

**Access:**

Common fields accessible in all states:

```cantrip
function log_url(req: HttpRequest@Building) {
    println(req.url)  // OK: url is common
}

function log_url2(req: HttpRequest@Sent) {
    println(req.url)  // OK: url is common
}
```

**Formal semantics:**

```
common fields ⊆ Fields(@S) for all @S ∈ states(M)
```

#### 2.3.3.6.3 Reachability Analysis

**Algorithm:**

```
reachability(M = (S, Σ, δ, s₀)):
1. Initialize reached = {s₀}
2. Initialize worklist = {s₀}
3. While worklist ≠ ∅:
     a. Remove state @S from worklist
     b. For each procedure m where (@S, m, @S') ∈ δ:
        i.  If @S' ∉ reached:
            - Add @S' to reached
            - Add @S' to worklist
4. Return unreachable = S \ reached
```

**Example:**

```cantrip
modal Protocol {
    state Init { }
    state Ready { }
    state Working { }
    state Done { }
    state Error { }       // Unreachable!
    state Timeout { }     // Unreachable!

    procedure start@Init() => @Ready
    procedure process@Ready() => @Working
    procedure finish@Working() => @Done
    // No transitions to Error or Timeout
}
```

**Diagnostic:** `W10025` — States Error and Timeout are unreachable

---

### 2.3.4 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - sum types combine primitive types
- See §2.2 for [Product Types](02_ProductTypes.md) - duality with sum types (∨ vs ∧)
- See §2.5 for [Pointers](../PART_C_References/05_Pointers.md) - safe pointers use modal states (@Exclusive, @Shared, @Frozen)
- See §2.6 for [Traits](../PART_D_Abstraction/06_Traits.md) - enums and modals implement traits
- See §2.7 for [Generics](../PART_D_Abstraction/07_Generics.md) - generic sum types (Option<T>, Result<T, E>)
- See Part V for [Effect System](../../05_Effects/) - modal transitions require effects
- See §13 for [Pattern Matching](../../13_PatternMatching/) - exhaustive pattern matching on enums
- See §17 for [Contracts](../../17_Contracts/) - state invariants and transition conditions
- See §24 for [Unsafe](../../24_Unsafe/) - union field access requires unsafe.union effect
- See §56-62 for [FFI](../../56_FFI/) - unions provide C-compatible untagged sum types
- **Examples**: See [03_SumExamples.md](../Examples/03_SumExamples.md) for practical usage patterns

### 2.3.5 Notes and Warnings

**Note 2.3.1:** Enums are safe tagged unions with compile-time exhaustiveness checking. Unions are unsafe untagged unions requiring manual tag management.

**Note 2.3.2:** Modals extend enums with explicit state machines and compile-time transition verification. Use modals when state transitions matter for correctness.

**Note 2.3.3:** Enum discriminants are automatically assigned unless explicitly specified with `= value` syntax. Discriminant values can be accessed via `as` cast for unit-only enums.

**Note 2.3.4:** Modals use linear state transfer - each procedure consumes the current state and produces the next state. State cannot be copied or duplicated.

**Performance Note 2.3.1:** Enum size is `size(discriminant) + max(variant sizes)` plus padding for alignment. The compiler uses the smallest discriminant type that can represent all variants.

**Performance Note 2.3.2:** Unions have no discriminant overhead - size is `max(field sizes)`. Use unions only when external tags track the active variant.

**Performance Note 2.3.3:** Modal state transitions compile to zero-cost abstractions. State checking and transition verification happen entirely at compile time.

**Warning 2.3.1:** Union field access without knowing the active variant causes undefined behavior. Always wrap unions in records with explicit tag fields.

**Warning 2.3.2:** Modals enforce linear state transfer. Attempting to use a modal value after a state transition causes a compile error.

**Warning 2.3.3:** Pattern matching on enums must be exhaustive. Missing variants cause compile errors unless a wildcard `_` pattern is provided.

**Warning 2.3.4:** Unions containing non-Copy types (String, Vec) require careful manual management. Dropping the wrong variant causes undefined behavior.

**Warning 2.3.5:** Modal state invariants are only as strong as the contracts you write. The compiler verifies transitions exist but cannot verify business logic correctness.

**Implementation Note 2.3.1:** The exhaustiveness checker uses constructor-based analysis to ensure all enum variants are covered in pattern matches.

**Implementation Note 2.3.2:** Modal reachability analysis ensures all states can be reached from the initial state. Unreachable states generate warnings.

**Implementation Note 2.3.3:** Union safety requires the `unsafe.union` effect to be declared. This makes unsafe boundaries explicit and auditable.

---

**Previous**: [Product Types](02_ProductTypes.md) | **Next**: [Collection Types](04_CollectionTypes.md)

