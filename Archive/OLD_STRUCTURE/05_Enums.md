# Part II: Type System - §9. Enums and Pattern Matching

**Section**: §9 | **Part**: Type System (Part II)
**Previous**: [Unions](04a_Unions.md) | **Next**: [Traits](06_Traits.md)

---

**Definition 9.1 (Enum):** An enum is a sum type (tagged union) with named variants. Formally, `enum E { V₁(T₁), ..., Vₙ(Tₙ) }` denotes the disjoint union `⟦E⟧ = T₁ + T₂ + ... + Tₙ`, where each variant Vᵢ is tagged with a discriminant to distinguish values at runtime.

## 9. Enums and Pattern Matching

### 9.1 Overview

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

### 9.2 Syntax

#### 9.2.1 Concrete Syntax

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

#### 9.2.2 Abstract Syntax

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

#### 9.2.3 Basic Examples

**Simple enum:**
```cantrip
enum Direction {
    North,
    South,
    East,
    West,
}

let dir = Direction.North
```

**Enum with data:**
```cantrip
enum Option<T> {
    Some(T),
    None,
}

let some_value: Option<i32> = Option.Some(42)
let no_value: Option<i32> = Option.None
```

**Pattern matching:**
```cantrip
match some_value {
    Option.Some(x) => println("Value: {x}"),
    Option.None => println("No value"),
}
```

### 9.3 Static Semantics

#### 9.3.1 Well-Formedness Rules

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

#### 9.3.2 Type Rules

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

#### 9.3.3 Type Properties

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

#### 9.3.4 Pattern Forms

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

### 9.4 Dynamic Semantics

#### 9.4.1 Evaluation Rules

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

#### 9.4.2 Memory Representation

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

#### 9.4.3 Operational Behavior

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

#### 9.4.4 Discriminant Size and Optimization

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

| Attribute | Effect |
|-----------|--------|
| `#[repr(u8)]` | Force u8 discriminant (max 256 variants) |
| `#[repr(u16)]` | Force u16 discriminant (max 65536 variants) |
| `#[repr(u32)]` | Force u32 discriminant |
| `#[repr(u64)]` | Force u64 discriminant |
| `#[repr(C)]` | C-compatible layout (discriminant + union) |
| `#[repr(C, u32)]` | C-compatible with explicit discriminant size |
| `#[repr(transparent)]` | Single-variant enum, same layout as payload |

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

### 9.5 Examples and Patterns

#### 9.5.1 Option Type Pattern

**Standard Option implementation:**
```cantrip
enum Option<T> {
    Some(T),
    None,
}

function divide(x: i32, y: i32): Option<i32> {
    if y == 0 {
        Option.None
    } else {
        Option.Some(x / y)
    }
}

match divide(10, 2) {
    Option.Some(result) => println("Result: {result}"),
    Option.None => println("Division by zero"),
}
```

#### 9.5.2 Result Type Pattern

**Error handling:**
```cantrip
enum Result<T, E> {
    Ok(T),
    Err(E),
}

function parse_number(s: str): Result<i32, String> {
    if let Some(n) = try_parse(s) {
        Result.Ok(n)
    } else {
        Result.Err("invalid number")
    }
}

match parse_number("123") {
    Result.Ok(n) => println("Parsed: {n}"),
    Result.Err(error) => println("Error: {error}"),
}
```

#### 9.5.3 State Machine Pattern

**Enum-based state machine:**
```cantrip
enum ConnectionState {
    Disconnected,
    Connecting { started_at: DateTime },
    Connected { session_id: String, connected_at: DateTime },
    Error { message: String },
}

function handle_state(state: ConnectionState): ConnectionState {
    match state {
        ConnectionState.Disconnected => {
            ConnectionState.Connecting { started_at: DateTime.now() }
        },
        ConnectionState.Connecting { started_at } => {
            if should_timeout(started_at) {
                ConnectionState.Error { message: "connection timeout" }
            } else {
                state  // Keep trying
            }
        },
        ConnectionState.Connected { .. } => state,
        ConnectionState.Error { .. } => ConnectionState.Disconnected,
    }
}
```

#### 9.5.4 Message Passing Pattern

**Protocol messages:**
```cantrip
enum Request {
    Get { key: String },
    Set { key: String, value: String },
    Delete { key: String },
    List,
}

enum Response {
    Value(String),
    Ok,
    NotFound,
    Error(String),
}

function handle_request(req: Request): Response {
    match req {
        Request.Get { key } => {
            if let Some(value) = lookup(key) {
                Response.Value(value)
            } else {
                Response.NotFound
            }
        },
        Request.Set { key, value } => {
            store(key, value)
            Response.Ok
        },
        Request.Delete { key } => {
            remove(key)
            Response.Ok
        },
        Request.List => {
            Response.Value(list_keys().join(","))
        },
    }
}
```

---

**Previous**: [Unions](04a_Unions.md) | **Next**: [Traits](06_Traits.md)
