# Chapter 2: Type System - §2.1 Primitive Types

**Section**: §2.1 | **Part**: Type System (Part II)
**Previous**: [Type System Introduction](../00_Introduction.md) | **Next**: [Product Types](../PART_B_Composite/02_ProductTypes.md)

---

## 2.1 Primitive Types Overview

This section specifies Cantrip's primitive types, which form the foundation of the type system. Primitive types are built into the language and cannot be defined by users.

**Primitive type categories:**

- **Integer types** (§2.1.1): Fixed-width signed and unsigned integers
- **Floating-point types** (§2.1.2): IEEE 754 binary floating-point numbers
- **Boolean type** (§2.1.3): Two-valued logic (`true`, `false`)
- **Character type** (§2.1.4): Unicode scalar values
- **Never type** (§2.1.5): Bottom type for diverging computations
- **String types** (§2.1.6): UTF-8 encoded text

---

## 2.1.1 Integer Types


##### 2.1.1.1 Overview

**Key innovation/purpose:** Explicit integer sizes enable precise control over memory layout, numeric ranges, and FFI interoperability, essential for systems programming.

**When to use integer types:**

- Counting, indexing, and iteration (prefer `usize` for indices)
- Arithmetic computations with known ranges
- Bit manipulation and low-level operations
- FFI interfacing with C/C++ integer types
- Memory addresses and offsets (`isize`/`usize`)
- Performance-critical numeric code

**When NOT to use integer types:**

- Arbitrary-precision arithmetic → use BigInt library
- Fractional numbers → use floating-point types (§2.1.1)
- Boolean flags → use `bool` type (§2.1.1)
- Text/characters → use `char` or string types

**Relationship to other features:**

- **Default integer type**: Integer literals without suffix default to `i32`
- **Overflow behavior**: Debug mode panics, release mode wraps (two's complement)
- **Type inference**: Context-sensitive (function signatures, annotations)
- **Permission system**: All integers implement `Copy` trait (can be explicitly copied)
- **Parameter passing**: Integers pass by permission like all types (no automatic copying)

##### 2.1.1.2 Syntax

#### Concrete Syntax

**Type syntax:**

```ebnf
IntType     ::= SignedInt | UnsignedInt
SignedInt   ::= "i8" | "i16" | "i32" | "i64" | "isize"
UnsignedInt ::= "u8" | "u16" | "u32" | "u64" | "usize"
```

**Literal syntax:**

```ebnf
IntLiteral  ::= DecLiteral TypeSuffix?
              | HexLiteral TypeSuffix?
              | OctLiteral TypeSuffix?
              | BinLiteral TypeSuffix?

DecLiteral  ::= Digit (Digit | "_")*
HexLiteral  ::= "0x" HexDigit (HexDigit | "_")*
OctLiteral  ::= "0o" OctDigit (OctDigit | "_")*
BinLiteral  ::= "0b" BinDigit (BinDigit | "_")*

TypeSuffix  ::= "i8" | "i16" | "i32" | "i64" | "isize"
              | "u8" | "u16" | "u32" | "u64" | "usize"

Digit       ::= [0-9]
HexDigit    ::= [0-9a-fA-F]
OctDigit    ::= [0-7]
BinDigit    ::= [0-1]
```

**Examples:**

```cantrip
// Decimal literals
let a: i32 = 42
let b: u64 = 1_000_000    // Underscores for readability
let c = 42i8              // Type suffix

// Hexadecimal
let hex: u32 = 0xDEAD_BEEF

// Octal
let oct: u16 = 0o755

// Binary
let bin: u8 = 0b1010_1010
```

#### Abstract Syntax

**Types:**

```
τ ::= i8 | i16 | i32 | i64 | isize      (signed integers)
    | u8 | u16 | u32 | u64 | usize      (unsigned integers)
```

**Values:**

```
v ::= n    (integer value)
```

##### 2.1.1.3 Static Semantics

###### Well-Formedness Rules

```
[WF-Int-Type]
T ∈ {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize}
────────────────────────────────────────────────────────
Γ ⊢ T : Type
```

#### Type Rules

**Integer literal typing:**

```
[T-Int-Literal-Default]
n is an integer literal without type suffix
────────────────────────────────────────
Γ ⊢ n : i32    (default type)

[T-Int-Literal-Suffix]
n is an integer literal with suffix T
T ∈ IntTypes
─────────────────────────────────────────
Γ ⊢ n : T    (explicit type from suffix)

[T-Int-Literal-Context]
n is an integer literal without type suffix
Γ ⊢ context must type T
T ∈ IntTypes
n ∈ ⟦T⟧    (n fits in range of T)
─────────────────────────────────────────
Γ ⊢ n : T    (type from context)
```

**Type inference priority:**

1. Explicit suffix (highest priority): `42u64` → `u64`
2. Context from annotation: `let x: u8 = 42` → `u8`
3. Default type: `let x = 42` → `i32` (lowest priority)

**Examples:**

```cantrip
let x = 42           // Type: i32 (default)
let y: u8 = 42       // Type: u8 (context)
let z = 42u64        // Type: u64 (suffix)
let w: i8 = 200      // ERROR E5101: 200 doesn't fit in i8 range [-128, 127]
```

#### Type Properties

**Theorem 5.1.1 (Value Sets):**

For each integer type T, the value set ⟦T⟧ is defined as:

| Type    | Value Set ⟦T⟧       | Min Value                  | Max Value                  | Size     | Align    |
| ------- | ------------------- | -------------------------- | -------------------------- | -------- | -------- |
| `i8`    | ℤ ∩ [-2⁷, 2⁷-1]     | -128                       | 127                        | 1 byte   | 1 byte   |
| `i16`   | ℤ ∩ [-2¹⁵, 2¹⁵-1]   | -32,768                    | 32,767                     | 2 bytes  | 2 bytes  |
| `i32`   | ℤ ∩ [-2³¹, 2³¹-1]   | -2,147,483,648             | 2,147,483,647              | 4 bytes  | 4 bytes  |
| `i64`   | ℤ ∩ [-2⁶³, 2⁶³-1]   | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807  | 8 bytes  | 8 bytes  |
| `isize` | ℤ ∩ [-2ⁿ⁻¹, 2ⁿ⁻¹-1] | Platform-dependent         | Platform-dependent         | Platform | Platform |
| `u8`    | ℕ ∩ [0, 2⁸-1]       | 0                          | 255                        | 1 byte   | 1 byte   |
| `u16`   | ℕ ∩ [0, 2¹⁶-1]      | 0                          | 65,535                     | 2 bytes  | 2 bytes  |
| `u32`   | ℕ ∩ [0, 2³²-1]      | 0                          | 4,294,967,295              | 4 bytes  | 4 bytes  |
| `u64`   | ℕ ∩ [0, 2⁶⁴-1]      | 0                          | 18,446,744,073,709,551,615 | 8 bytes  | 8 bytes  |
| `usize` | ℕ ∩ [0, 2ⁿ-1]       | 0                          | Platform-dependent         | Platform | Platform |

Where n = pointer width (32 or 64 bits depending on target architecture).

**Theorem 5.1.2 (Copy Capability):**

All integer types implement the `Copy` trait:

```
∀ T ∈ IntTypes. T : Copy
```

**Semantics:**

- Integers pass by permission (reference-like) by default
- The `.copy()` method creates an explicit duplicate when needed
- Small size (1-8 bytes) makes copying cheap when explicitly requested
- Assignment creates permission bindings, not copies

**Example:**

```cantrip
let x: i32 = 42
let y = x           // y binds to x's value, both usable (permission binding)
let z = x.copy()    // Explicit copy creates new value

procedure double(n: i32): i32 {
    n * 2  // Access through permission, no copy
}

let result = double(x)  // x passed by permission, still usable after
```

**Theorem 5.1.3 (Platform-Dependent Sizes):**

```
32-bit systems: isize = i32, usize = u32, n = 32
64-bit systems: isize = i64, usize = u64, n = 64
```

**Corollary:** Code using `isize`/`usize` may have different behavior on different architectures.

##### 2.1.1.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**

```
[E-Int]
────────────────
⟨n, σ⟩ ⇓ ⟨n, σ⟩
```

Integer literals evaluate to themselves without state change.

**Arithmetic operations:**

```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨n₁ ⊕ n₂, σ₂⟩

where ⊕ is:
  - wrapping addition (modulo 2^bits) in release mode
  - checked addition (panics on overflow) in debug mode

[E-Sub]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ - e₂, σ⟩ ⇓ ⟨n₁ ⊖ n₂, σ₂⟩

[E-Mul]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ * e₂, σ⟩ ⇓ ⟨n₁ ⊗ n₂, σ₂⟩

[E-Div]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
n₂ ≠ 0
─────────────────────────────────────────
⟨e₁ / e₂, σ⟩ ⇓ ⟨n₁ ÷ n₂, σ₂⟩

[E-Div-Zero]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨0, σ₂⟩
─────────────────────────────────────────
⟨e₁ / e₂, σ⟩ ⇓ panic("division by zero")
```

#### Memory Representation

**Integer layout:**

Integers are stored in two's complement representation (signed) or unsigned binary (unsigned) with platform-native byte order (typically little-endian on x86-64, ARM64).

```
i8/u8 memory layout (1 byte):
┌────┐
│ b₀ │
└────┘
Alignment: 1 byte

i16/u16 memory layout (2 bytes):
┌────┬────┐
│ b₀ │ b₁ │
└────┴────┘
Alignment: 2 bytes

i32/u32 memory layout (4 bytes):
┌────┬────┬────┬────┐
│ b₀ │ b₁ │ b₂ │ b₃ │
└────┴────┴────┴────┘
Alignment: 4 bytes

i64/u64 memory layout (8 bytes):
┌────┬────┬────┬────┬────┬────┬────┬────┐
│ b₀ │ b₁ │ b₂ │ b₃ │ b₄ │ b₅ │ b₆ │ b₇ │
└────┴────┴────┴────┴────┴────┴────┴────┘
Alignment: 8 bytes
```

**Two's complement representation (signed):**

For signed integer n of bit-width w:

```
If n ≥ 0: binary representation of n
If n < 0: binary representation of 2^w + n

Example (i8):
  127 → 0x7F → 0111_1111
    0 → 0x00 → 0000_0000
   -1 → 0xFF → 1111_1111
 -128 → 0x80 → 1000_0000
```

**Byte order:**

Cantrip follows the platform's native byte order:

- **Little-endian** (x86, x86-64, ARM64): Least significant byte first
- **Big-endian** (some RISC architectures): Most significant byte first

```
Value: 0x12345678 (i32)

Little-endian memory layout:
Address: [0]  [1]  [2]  [3]
Value:   0x78 0x56 0x34 0x12

Big-endian memory layout:
Address: [0]  [1]  [2]  [3]
Value:   0x12 0x34 0x56 0x78
```

#### Operational Behavior

**Integer overflow:**

**Axiom 5.1.1 (Integer Arithmetic):**

For integer type T with min M and max N:

```
Debug mode (checked):
  x ⊕ y ∉ [M, N] ⟹ panic("arithmetic overflow")

Release mode (wrapping):
  result = (x ⊕ y) mod (N - M + 1)
  result ∈ [M, N]
```

**Examples:**

```cantrip
// u8: range [0, 255]
let x: u8 = 255
let y = x + 1
// Debug: panic!("attempt to add with overflow")
// Release: y = 0 (wrapping: 256 mod 256 = 0)

// i8: range [-128, 127]
let a: i8 = 127
let b = a + 1
// Debug: panic!
// Release: b = -128 (two's complement wrap)
```

**Attribute control:**

Override default overflow behavior with attributes:

```cantrip
#[overflow_checks(true)]
function critical_math(a: i32, b: i32): i32 {
    a * b  // Always checked, even in release mode
}

#[overflow_checks(false)]
function fast_hash(x: u64): u64 {
    x.wrapping_mul(0x517cc1b727220a95)  // Never checked
}
```

**Explicit overflow handling:**

Cantrip provides methods for explicit overflow control:

```cantrip
// Checked arithmetic (returns Option)
let x: u32 = 100
let y: u32 = 200
let sum = x.checked_add(y)?  // Some(300)

let max: u32 = u32::MAX
let overflow = max.checked_add(1)  // None

// Saturating arithmetic (clamps at limits)
let sat_sum = max.saturating_add(1)  // u32::MAX

// Wrapping arithmetic (explicit wrap-around)
let wrap_sum = max.wrapping_add(1)  // 0

// Overflowing arithmetic (returns tuple with overflow flag)
let (result, overflowed) = max.overflowing_add(1)
// result = 0, overflowed = true
```

**Division and modulo:**

```cantrip
// Division by zero: always panics (both debug and release)
let x = 10 / 0  // panic!("attempt to divide by zero")

// Integer division truncates toward zero
assert!(7 / 2 == 3)
assert!(-7 / 2 == -3)

// Modulo has sign of left operand
assert!(7 % 3 == 1)
assert!(-7 % 3 == -1)
assert!(7 % -3 == 1)
assert!(-7 % -3 == -1)

// Special case: MIN / -1 can overflow
let min: i8 = i8::MIN  // -128
let result = min / -1  // Would be 128, which overflows i8
// Debug: panic!("attempt to divide with overflow")
// Release: result = -128 (wraps)
```

---

**Parent**: [Primitive Types](01_PrimitiveTypes.md) | **Next**: [Floating-Point Types](01b_FloatingPoint.md)

---

## 2.1.2 Floating-Point Types


##### 2.1.2.1 Overview

**Key innovation/purpose:** IEEE 754-compliant floating-point provides standardized, hardware-accelerated real number approximations with well-defined behavior for special values (±∞, NaN).

**When to use floating-point:**

- Scientific and engineering calculations
- Graphics and game development (positions, transforms)
- Financial calculations (with appropriate precision considerations)
- Machine learning and statistical computations
- Approximate real number representations

**When NOT to use floating-point:**

- Exact decimal arithmetic → use fixed-point or decimal types
- Integers → use integer types (§2.1.2)
- Equality comparisons → use epsilon-based comparisons
- Cryptographic operations → require exact integer arithmetic

**Relationship to other features:**

- **Default float type**: Float literals default to `f64`
- **Special values**: ±0, ±∞, NaN with IEEE 754 semantics
- **NaN propagation**: Operations with NaN produce NaN
- **Copy trait**: Both `f32` and `f64` implement `Copy` (can be explicitly copied)
- **Parameter passing**: Floats pass by permission like all types (no automatic copying)

##### 2.1.2.2 Syntax

#### Concrete Syntax

**Type syntax:**

```ebnf
FloatType ::= "f32" | "f64"
```

**Literal syntax:**

```ebnf
FloatLiteral ::= DecFloatLit TypeSuffix?
               | DecLiteral Exponent TypeSuffix?
               | DecLiteral "." DecLiteral? Exponent? TypeSuffix?
               | "." DecLiteral Exponent? TypeSuffix?

Exponent     ::= ("e" | "E") ("+" | "-")? DecLiteral
TypeSuffix   ::= "f32" | "f64"
```

**Examples:**

```cantrip
// Basic floating-point literals
let pi: f64 = 3.14159
let e: f32 = 2.71828f32

// Scientific notation
let avogadro: f64 = 6.022e23
let planck: f64 = 6.626e-34

// Suffix notation
let x = 1.0f32
let y = 2.5f64

// Underscores for readability
let large: f64 = 299_792_458.0  // Speed of light (m/s)
```

#### Abstract Syntax

**Types:**

```
τ ::= f32 | f64
```

**Values:**

```
v ::= f    (floating-point value, including ±0, ±∞, NaN)
```

##### 2.1.2.3 Static Semantics

#### Well-Formedness Rules

```
[WF-Float]
T ∈ {f32, f64}
──────────────────
Γ ⊢ T : Type
```

#### Type Rules

**Floating-point literal typing:**

```
[T-Float-Literal-Default]
f is a floating-point literal without type suffix
───────────────────────────────────────────────
Γ ⊢ f : f64    (default type)

[T-Float-Literal-Suffix]
f is a floating-point literal with suffix T
T ∈ {f32, f64}
──────────────────────────────────────────
Γ ⊢ f : T    (explicit type from suffix)

[T-Float-Literal-Context]
f is a floating-point literal without type suffix
Γ ⊢ context must type T
T ∈ {f32, f64}
──────────────────────────────────────────
Γ ⊢ f : T    (type from context)
```

**Examples:**

```cantrip
let x = 3.14         // Type: f64 (default)
let y: f32 = 3.14    // Type: f32 (context)
let z = 3.14f32      // Type: f32 (suffix)
```

#### Type Properties

**Theorem 5.2.1 (IEEE 754 Representation):**

Cantrip floating-point types conform to IEEE 754-2008:

| Type  | Format   | Sign bits | Exponent bits | Mantissa bits | Total bits |
| ----- | -------- | --------- | ------------- | ------------- | ---------- |
| `f32` | binary32 | 1         | 8             | 23            | 32         |
| `f64` | binary64 | 1         | 11            | 52            | 64         |

**Theorem 5.2.2 (Value Set):**

```
⟦f32⟧ = IEEE754_single = {
    ±0,                          (positive/negative zero)
    ±∞,                          (positive/negative infinity)
    NaN,                         (not-a-number, many representations)
    normalized values,           (standard representation)
    denormalized values          (gradual underflow)
}

⟦f64⟧ = IEEE754_double (same structure, different precision)
```

**Precision:**

- `f32`: ~7 decimal digits of precision
- `f64`: ~15-17 decimal digits of precision

**Range:**

- `f32`: ±1.18 × 10⁻³⁸ to ±3.40 × 10³⁸
- `f64`: ±2.23 × 10⁻³⁰⁸ to ±1.80 × 10³⁰⁸

##### 2.1.2.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**

```
[E-Float]
────────────────
⟨f, σ⟩ ⇓ ⟨f, σ⟩
```

**Arithmetic operations:**

```
[E-Float-Add]
⟨e₁, σ⟩ ⇓ ⟨f₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨f₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨round(f₁ + f₂), σ₂⟩

where round applies IEEE 754 rounding mode (default: round-to-nearest-even)

[E-Float-Div-Zero]
⟨e₁, σ⟩ ⇓ ⟨f₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨0.0, σ₂⟩
f₁ ≠ 0
─────────────────────────────────────────
⟨e₁ / e₂, σ⟩ ⇓ ⟨±∞, σ₂⟩    (±∞ depending on sign)

[E-Float-NaN]
⟨e₁, σ⟩ ⇓ ⟨NaN, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨f₂, σ₂⟩
─────────────────────────────────────────
⟨e₁ ⊕ e₂, σ⟩ ⇓ ⟨NaN, σ₂⟩    (NaN propagation)
```

#### Memory Representation

**f32 memory layout (4 bytes):**

```
IEEE 754 single-precision:
┌─┬────────┬───────────────────────┐
│S│Exponent│      Mantissa         │
│ │(8 bits)│      (23 bits)        │
└─┴────────┴───────────────────────┘
Size: 4 bytes, Alignment: 4 bytes

Components:
- Sign bit (S): 1 bit
- Exponent: 8 bits (biased by 127)
- Mantissa: 23 bits (implicit leading 1)
```

**f64 memory layout (8 bytes):**

```
IEEE 754 double-precision:
┌─┬───────────┬────────────────────────────────────────────────┐
│S│ Exponent  │              Mantissa                          │
│ │ (11 bits) │              (52 bits)                         │
└─┴───────────┴────────────────────────────────────────────────┘
Size: 8 bytes, Alignment: 8 bytes

Components:
- Sign bit (S): 1 bit
- Exponent: 11 bits (biased by 1023)
- Mantissa: 52 bits (implicit leading 1)
```

#### Operational Behavior

**Formal properties:**

```
⟦f₁ + f₂⟧ = round(⟦f₁⟧ + ⟦f₂⟧)
⟦f₁ * f₂⟧ = round(⟦f₁⟧ × ⟦f₂⟧)
⟦f₁ / f₂⟧ = round(⟦f₁⟧ / ⟦f₂⟧)

where round is IEEE 754 round-to-nearest-even
```

**Special value arithmetic:**

```
x + ∞ = ∞    (for x ≠ -∞)
x - ∞ = -∞   (for x ≠ ∞)
∞ + ∞ = ∞
∞ - ∞ = NaN
x * ∞ = ∞    (for x > 0)
x * ∞ = -∞   (for x < 0)
0 * ∞ = NaN
x / 0 = ±∞   (for x ≠ 0)
0 / 0 = NaN
∞ / ∞ = NaN
```

**NaN properties:**

```
NaN ≠ NaN        (NaN is not equal to itself)
NaN < x = false  (all comparisons with NaN are false)
NaN ≤ x = false
NaN > x = false
NaN ≥ x = false
NaN == x = false
```

**Signed zeros:**

```
+0.0 == -0.0     (equal for comparison)
1.0 / +0.0 = +∞
1.0 / -0.0 = -∞
```

---

## 2.1.3 Boolean Type


##### 2.1.3.1 Overview

**Key innovation/purpose:** `bool` provides type-safe two-valued logic with short-circuit evaluation for conditional expressions, essential for control flow and predicate logic.

**When to use bool:**

- Conditional control flow (if, while, match guards)
- Logical predicates and assertions
- Boolean flags and state indicators
- Function return values for yes/no questions

**When NOT to use bool:**

- Multiple states → use enums (§9)
- Optional presence → use `Option<T>`
- Numeric values → use integer types (§2.1.3)
- Bitwise operations on multiple flags → use integer bit masks

**Relationship to other features:**

- **Control flow**: Boolean expressions control `if`, `while`, `match` guards
- **Short-circuit evaluation**: `&&` and `||` operators short-circuit
- **Pattern matching**: `true` and `false` are patterns
- **Copy trait**: `bool` implements `Copy` (can be explicitly copied)
- **Parameter passing**: Booleans pass by permission like all types (no automatic copying)

##### 2.1.3.2 Syntax

#### Concrete Syntax

**Type and literal syntax:**

```ebnf
BoolType    ::= "bool"
BoolLiteral ::= "true" | "false"
```

**Examples:**

```cantrip
let flag: bool = true
let is_valid = false

if flag {
    println("Flag is set")
}
```

#### Abstract Syntax

**Type:**

```
τ ::= bool
```

**Values:**

```
v ::= true | false
```

##### 2.1.3.3 Static Semantics

#### Well-Formedness Rules

```
[WF-Bool]
──────────────────
Γ ⊢ bool : Type
```

#### Type Rules

**Boolean literal typing:**

```
[T-Bool-True]
──────────────────
Γ ⊢ true : bool

[T-Bool-False]
──────────────────
Γ ⊢ false : bool
```

**No type inference needed:** Boolean literals always have type `bool` with no context sensitivity or suffixes.

#### Type Properties

**Theorem 5.3.1 (Value Set):**

The boolean type has exactly two values:

```
⟦bool⟧ = {true, false} ≅ {⊤, ⊥} ≅ ℤ₂
```

**Theorem 5.3.2 (Copy Capability):**

```
bool : Copy
```

**Semantics:**

- Booleans pass by permission (reference-like) by default
- The `.copy()` method creates an explicit duplicate when needed
- Single-byte size makes copying trivial when explicitly requested

**Theorem 5.3.3 (Size and Alignment):**

```
size(bool) = 1 byte
align(bool) = 1 byte
```

##### 2.1.3.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**

```
[E-Bool-True]
────────────────────────
⟨true, σ⟩ ⇓ ⟨true, σ⟩

[E-Bool-False]
────────────────────────
⟨false, σ⟩ ⇓ ⟨false, σ⟩
```

**Logical operations:**

```
[E-And-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩
────────────────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩

[E-And-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ'⟩
────────────────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨false, σ'⟩    (short-circuit, e₂ not evaluated)

[E-Or-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩
────────────────────────────────────────
⟨e₁ || e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩

[E-Or-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ'⟩
────────────────────────────────────────
⟨e₁ || e₂, σ⟩ ⇓ ⟨true, σ'⟩    (short-circuit, e₂ not evaluated)

[E-Not]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
────────────────────────────────────────
⟨!e, σ⟩ ⇓ ⟨¬v, σ'⟩    (logical negation)
```

#### Memory Representation

**Boolean layout:**

```
bool memory layout (1 byte):
┌────┐
│0/1 │
└────┘
Size: 1 byte
Alignment: 1 byte

Values:
- false = 0x00 (all bits zero)
- true  = 0x01 (least significant bit set, others zero)
```

**Representation guarantee:**

```
⟦false⟧ = 0x00
⟦true⟧  = 0x01
```

**Note:** While only values 0x00 and 0x01 are valid for `bool`, Cantrip guarantees all boolean values are normalized to these representations. Creating a `bool` with any other bit pattern (via unsafe code) is undefined behavior.

#### Operational Behavior

**Boolean algebra:**

Cantrip booleans follow standard Boolean algebra:

```
Logical AND (∧):
⊤ ∧ ⊤ = ⊤
⊤ ∧ ⊥ = ⊥
⊥ ∧ ⊤ = ⊥
⊥ ∧ ⊥ = ⊥

Logical OR (∨):
⊤ ∨ ⊤ = ⊤
⊤ ∨ ⊥ = ⊤
⊥ ∨ ⊤ = ⊤
⊥ ∨ ⊥ = ⊥

Logical NOT (¬):
¬⊤ = ⊥
¬⊥ = ⊤
```

**Short-circuit evaluation:**

```
⊥ ∧ _ = ⊥    (right side not evaluated)
⊤ ∨ _ = ⊤    (right side not evaluated)
```

**Examples:**

```cantrip
// Short-circuit prevents division by zero
let safe = (y != 0) && (x / y > 10)

// Short-circuit prevents null dereference
let result = (ptr != null) && (*ptr == target)
```

**Comparison:**

```
true == true   = true
false == false = true
true == false  = false
true != false  = true
```

---

## 2.1.4 Character Type


##### 2.1.4.1 Overview

**Key innovation/purpose:** `char` provides Unicode-correct character handling by representing Unicode scalar values (not bytes or code units), ensuring text processing correctness for international text.

**When to use char:**

- Single Unicode characters (grapheme clusters may require strings)
- Character-by-character text processing
- Unicode codepoint manipulation
- Character classification and conversion
- Text parsing and lexical analysis

**When NOT to use char:**

- Text strings → use `str` or `String` types
- Byte data → use `u8` type
- ASCII-only data → use `u8` with appropriate validation
- Grapheme clusters (user-perceived characters) → use string libraries

**Relationship to other features:**

- **UTF-32 encoding**: Each `char` is 4 bytes (one Unicode scalar value)
- **No type inference**: Character literals always have type `char`
- **Unicode correctness**: Surrogate pairs (U+D800-U+DFFF) are invalid
- **Copy trait**: `char` implements `Copy` (can be explicitly copied)
- **Parameter passing**: Characters pass by permission like all types (no automatic copying)

##### 2.1.4.2 Syntax

#### Concrete Syntax

**Type and literal syntax:**

```ebnf
CharType    ::= "char"
CharLiteral ::= "'" CharContent "'"

CharContent ::= AnyChar
              | EscapeSeq
              | UnicodeEscape

EscapeSeq   ::= "\\" | "\'" | "\"" | "\n" | "\r" | "\t" | "\0"
UnicodeEscape ::= "\\u{" HexDigit+ "}"
```

**Examples:**

```cantrip
let letter: char = 'A'           // U+0041 (Latin capital A)
let emoji: char = '🚀'            // U+1F680 (Rocket)
let chinese: char = '中'          // U+4E2D (Chinese character)
let null_char: char = '\0'       // U+0000 (null character, valid!)
let tab: char = '\t'             // U+0009 (tab)
let newline: char = '\n'         // U+000A (line feed)
let unicode: char = '\u{1F600}'  // U+1F600 (grinning face emoji)
```

#### Abstract Syntax

**Type:**

```
τ ::= char
```

**Values:**

```
v ::= 'c'    (character literal)
```

##### 2.1.4.3 Static Semantics

###### 2.1.4.3.1 Well-Formedness Rules

```
[WF-Char]
──────────────────
Γ ⊢ char : Type
```

###### 2.1.4.3.2 Type Rules

**Character literal typing:**

```
[T-Char]
c is a valid Unicode scalar value
c ∈ [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]
──────────────────
Γ ⊢ 'c' : char
```

**No type inference:** Character literals always have type `char` with no context sensitivity:

```cantrip
let c1 = 'A'           // Type: char (no inference needed)
let c2: char = '🚀'    // Type: char (explicit annotation)
// No suffixes or context variations like integers
```

**Invalid characters:**

```cantrip
// ERROR E5401: Invalid Unicode scalar value (surrogate pair)
// let invalid: char = '\u{D800}';  // U+D800 is in surrogate range
```

###### 2.1.4.3.3 Type Properties

**Theorem 5.4.1 (Value Set):**

```
⟦char⟧ = Unicode Scalar Values = [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]

Total valid values: 1,112,064 codepoints
- U+0000 to U+D7FF: 55,296 codepoints
- U+E000 to U+10FFFF: 1,056,768 codepoints
```

**Excluded range (surrogate pairs):**

```
[U+D800, U+DFFF] = 2,048 invalid codepoints
```

**Theorem 5.4.2 (Size and Alignment):**

```
size(char) = 4 bytes
align(char) = 4 bytes
```

**Theorem 5.4.3 (Copy Capability):**

```
char : Copy
```

**Semantics:**

- Characters pass by permission (reference-like) by default
- The `.copy()` method creates an explicit duplicate when needed
- Fixed 4-byte size makes copying inexpensive when explicitly requested

##### 2.1.4.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**

```
[E-Char]
────────────────
⟨'c', σ⟩ ⇓ ⟨'c', σ⟩
```

**Character comparison:**

```
[E-Char-Comparison]
⟨c₁, σ⟩ ⇓ ⟨v₁, σ'⟩    ⟨c₂, σ'⟩ ⇓ ⟨v₂, σ''⟩
codepoint(v₁) ⊙ codepoint(v₂) = result
where ⊙ ∈ {<, ≤, >, ≥, ==, ≠}
─────────────────────────────────────────
⟨c₁ ⊙ c₂, σ⟩ ⇓ ⟨result, σ''⟩
```

#### Memory Representation

**Character layout:**

```
char memory layout (4 bytes):
┌─────────────────────┐
│ Unicode codepoint   │
│     (UTF-32)        │
└─────────────────────┘
Size: 4 bytes
Alignment: 4 bytes

Valid range: [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]

Examples:
'A'  → 0x00000041
'€'  → 0x000020AC
'🚀' → 0x0001F680
```

**Byte order:**

Characters follow platform byte order (typically little-endian):

```
'🚀' (U+1F680) on little-endian:
Address: [0]  [1]  [2]  [3]
Value:   0x80 0xF6 0x01 0x00
```

#### Operational Behavior

**Comparison semantics:**

Character comparison uses Unicode scalar value ordering:

```cantrip
'A' < 'Z'              // true (U+0041 < U+005A)
'a' > 'A'              // true (U+0061 > U+0041, lowercase > uppercase)
'0' < '9'              // true (U+0030 < U+0039)
'🚀' == '🚀'            // true (U+1F680 == U+1F680)
```

**Formal property:**

```
∀ c₁, c₂ : char. c₁ < c₂ ⟺ codepoint(c₁) < codepoint(c₂)
```

**Character conversions:**

Characters can be converted to/from `u32` representing the Unicode codepoint:

```cantrip
let c: char = 'A'
let code: u32 = c as u32                // 65 (U+0041)

// Converting u32 to char (validation required)
match char::from_u32(65) {
    Option.Some(valid_char) => valid_char,  // 'A'
    Option.None => panic("invalid codepoint"),
}

let invalid: Option<char> = char::from_u32(0xD800) // None (surrogate)
```

**Type rules for conversions:**

```
[T-Char-To-U32]
Γ ⊢ c : char
────────────────────────────
Γ ⊢ (c as u32) : u32

[T-U32-To-Char]
Γ ⊢ n : u32
────────────────────────────────────────
Γ ⊢ char::from_u32(n) : Option<char>    (runtime validation)
```

---

## 2.1.5 Never Type


##### 2.1.5.1 Overview

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

##### 2.1.5.2 Syntax

#### Concrete Syntax

**Type syntax:**

```ebnf
NeverType ::= "!"
```

**Usage in function signatures:**

```cantrip
function panic(message: str): !
    uses panic
{
    std.process::abort()
}

function exit(code: i32): !
    uses process.exit
{
    std.process::exit(code)
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

##### 2.1.5.3 Static Semantics

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

##### 2.1.5.4 Dynamic Semantics

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
    std.process::abort()
}
```

2. **Exit**: Normal process termination

```cantrip
function exit(code: i32): ! {
    std.process::exit(code)
}
```

3. **Infinite loop**: Non-terminating computation

```cantrip
function serve_forever(): ! {
    loop {
        handle_request()
    }
}
```

## 2.1.6 String Types


##### 2.1.6.1 Overview

**Key innovation/purpose:** String provides a single, unified type for owned text data with automatic memory management and guaranteed UTF-8 validity, simplifying text handling compared to systems requiring explicit distinction between owned and borrowed string types.

**When to use String:**

- Text data that uses to be modified or grown
- Owned text that must outlive its creation scope
- Building strings dynamically (concatenation, formatting)
- Storing text in data structures (records, collections)
- User input, file contents, or network data
- Log messages and error descriptions

**When NOT to use String:**

- Binary data without UTF-8 encoding → use `Vec<u8>`
- Temporary views of existing text → use `str` (string slice)
- Non-UTF-8 text encodings → use `Vec<u8>` with explicit encoding library
- Individual Unicode characters → use `char` type
- Static text that never changes → use string literals (`"text"` of type `str`)

**Relationship to other features:**

- **String slice (`str`)**: String literals have type `str`, which is an immutable view into UTF-8 data. String owns its data, while `str` borrows it.
- **Permission system**: String follows standard permission rules - `own String` for ownership, `mut String` for mutable references, `String` for immutable references.
- **Region system**: String lifetime is managed through lexical regions, not explicit lifetime parameters.
- **`char` type**: String is composed of Unicode scalar values (U+0000 to U+D7FF and U+E000 to U+10FFFF), matching the `char` type representation.
- **`Vec<u8>`**: String is internally a vector of UTF-8 bytes with validity enforcement.
- **Permission-based aliasing**: Multiple references to String are allowed; programmer manages safety.

##### 2.1.6.2 Syntax

###### 2.1.6.2.1 Concrete Syntax

**Type syntax:**

```ebnf
StringType ::= "String"
```

**String literal syntax (produces `str` type):**

```ebnf
StringLiteral  ::= '"' (EscapeSequence | ~["\\])* '"'
EscapeSequence ::= "\\" [nrt\\'"0]
                 | "\\x" HexDigit HexDigit
                 | "\\u{" HexDigit+ "}"
```

**Permission annotations:**

```ebnf
OwnedString    ::= "own" "String"
MutableString  ::= "mut" "String"
RefString      ::= "String"              # Immutable reference (default)
IsolatedString ::= "iso" "String"
```

**Examples:**

```cantrip
// Type annotations
let message: own String = String.new()
let greeting: String = "hello"          // ERROR: type mismatch (str vs String)

// String construction
let empty: own String = String.new()
let from_literal: own String = String.from("hello")

// Permission annotations
function read_only(text: String) { ... }         // Immutable reference
function modify(text: mut String) { ... }        // Mutable reference
function consume(own text: String) { ... }       // Takes ownership

// String literals (type: str, not String)
"hello world"
"line 1\nline 2"
"unicode: 🚀"
"quotes: \" "
"hex byte: \x41"
"unicode escape: \u{1F600}"
```

###### 2.1.6.2.2 Abstract Syntax

**Type representation:**

```
τ ::= String                                    (String type)
    | own String                                (owned String)
    | mut String                                (mutable reference to String)
    | iso String                                (isolated String)

String = record {
    data: own Vec<u8>                          (UTF-8 byte vector)
}
```

**Value set:**

```
⟦String⟧ = { s | s = (ptr, len, cap),
             ptr : *u8,
             len : usize,
             cap : usize,
             len ≤ cap,
             valid_utf8(ptr[0..len]) }

where valid_utf8(bytes) ⟹ bytes form valid UTF-8 sequence
```

**Components:**

- **ptr**: Pointer to heap-allocated UTF-8 byte data
- **len**: Current length in bytes (not characters)
- **cap**: Allocated capacity in bytes
- **Invariant**: `data` contains valid UTF-8 at all times

**UTF-8 invariant (formal):**

```
∀ s : String. valid_utf8(s.data[0..s.len])

valid_utf8(bytes) ⟺
  ∀ i. (bytes[i] ∈ [0x00, 0x7F]) ∨
       (bytes[i] ∈ [0xC2, 0xDF] ∧ continuation(bytes[i+1])) ∨
       (bytes[i] ∈ [0xE0, 0xEF] ∧ continuation(bytes[i+1..i+2])) ∨
       (bytes[i] ∈ [0xF0, 0xF4] ∧ continuation(bytes[i+1..i+3]))

where continuation(b) ⟺ b ∈ [0x80, 0xBF]
```


##### 2.1.6.3 Static Semantics

###### 2.1.6.3.1 Well-Formedness Rules

**Definition 2.1.6.2 (Well-Formed String):** The String type is well-formed in any context. String values must maintain the UTF-8 validity invariant.

**[WF-String]** - String type is well-formed:

```
────────────────
Γ ⊢ String : Type
```

**Explanation:** String is a built-in type available in all contexts without imports.

**[WF-String-Owned]** - Owned string is well-formed:

```
[WF-String-Owned]
Γ ⊢ String : Type
─────────────────────
Γ ⊢ own String : Type
```

**Explanation:** Owned String is the standard form for heap-allocated strings. The `own` permission indicates ownership of heap memory.

**[WF-String-Mut]** - Mutable string reference:

```
[WF-String-Mut]
Γ ⊢ String : Type
─────────────────────
Γ ⊢ mut String : Type
```

**Explanation:** Mutable references to String allow modification operations like `push_str`. Multiple mutable references are permitted in Cantrip.

**[WF-String-UTF8-Invariant]** - UTF-8 validity invariant:

```
[WF-String-UTF8-Invariant]
Γ ⊢ s : String
─────────────────────────────
valid_utf8(s.ptr, s.len) = true
```

**Explanation:** All String values satisfy UTF-8 validity. The String API will this invariant is never violated.

###### 2.1.6.3.2 Type Rules

**[T-String-Literal-Conv]** - String literal conversion:

```
[T-String-Literal-Conv]
Γ ⊢ lit : str
valid_utf8(lit) = true
────────────────────────────────
Γ ⊢ String.from(lit) : own String
    uses alloc.heap
```

**Explanation:** String literals have type `str` (immutable string slice). Converting to `String` allocates heap memory and copies the data.

**Example:**

```cantrip
function create_greeting(): own String
    uses alloc.heap
{
    String.from("Hello, World!")
    // Type: own String (heap-allocated copy)
}
```

**[T-String-New]** - Empty string construction:

```
[T-String-New]
────────────────────────────────
Γ ⊢ String.new() : own String
    uses alloc.heap
```

**Explanation:** Creating a new empty String allocates minimal heap storage and returns owned String.

**Example:**

```cantrip
function create_buffer(): own String
    uses alloc.heap
{
    String.new()  // Empty string, capacity may be 0 or small
}
```

**[T-String-From-Slice]** - String from slice:

```
[T-String-From-Slice]
Γ ⊢ data : str
valid_utf8(data) = true
────────────────────────────────
Γ ⊢ String.from(data) : own String
    uses alloc.heap
```

**Explanation:** Construct String from any string slice. Allocates heap memory and copies the UTF-8 data.

**Example:**

```cantrip
function duplicate(text: str): own String
    uses alloc.heap
{
    String.from(text)  // Heap-allocated copy
}
```

**[T-String-Method]** - String method invocation:

```
[T-String-Method]
Γ ⊢ s : String
method m has signature (String, T₁, ..., Tₙ) → U ! ε
Γ ⊢ arg₁ : T₁    ...    Γ ⊢ argₙ : Tₙ
─────────────────────────────────────────────────
Γ ⊢ s.m(arg₁, ..., argₙ) : U ! ε
```

**Explanation:** Method calls on String follow standard method resolution. The receiver permission (String, mut String, own String) must match method signature.

**Example:**

```cantrip
let s: own String = String.from("hello")
let n: usize = s.len()        // Method: len(self: String): usize
let empty: bool = s.is_empty() // Method: is_empty(self: String): bool
```

**[T-String-Mut-Method]** - Mutable string method:

```
[T-String-Mut-Method]
Γ ⊢ s : own String
method m has signature (mut String, T₁, ..., Tₙ) → U ! ε
Γ ⊢ arg₁ : T₁    ...    Γ ⊢ argₙ : Tₙ
─────────────────────────────────────────────────
Γ ⊢ s.m(arg₁, ..., argₙ) : U ! ε
```

**Explanation:** Mutable methods like `push_str` require mutable reference. Owned String can provide mutable access.

**Example:**

```cantrip
function append_text(text: str): own String
    uses alloc.heap
{
    var s = String.new()
    s.push_str(text)  // Mutates s (requires mut String)
    move s
}
```

**[T-String-Permission-Upcast]** - Permission compatibility:

```
[T-String-Permission-Upcast]
Γ ⊢ s : own String
────────────────────────────
Γ ⊢ s : String
```

**Explanation:** Owned String can be passed as immutable reference without transferring ownership. This enables read-only access while retaining ownership.

**Example:**

```cantrip
function print(text: String) {
    // Read-only access to text
}

function example()
    uses alloc.heap
{
    let msg: own String = String.from("data")
    print(msg)  // Passes immutable reference (own String <: String)
    print(msg)  // msg still owned here
    // msg destroyed at end of scope
}
```

**[T-String-Move]** - Ownership transfer:

```
[T-String-Move]
Γ ⊢ s : own String
Γ' = Γ \ {s}
────────────────────────────
Γ ⊢ move s : own String
Γ' ⊬ s
```

**Explanation:** Moving a String transfers ownership of heap buffer. Original binding becomes inaccessible after move.

**Example:**

```cantrip
function consume(own s: String) {
    // s destroyed here
}

function example()
    uses alloc.heap
{
    let s1: own String = String.from("text")
    let s2: own String = move s1  // Transfer ownership
    // s1 no longer accessible
    consume(move s2)  // Transfer to function
}
```

###### 2.1.6.3.3 Type Properties

**Theorem 5.6.1 (String is NOT Copy):**

String does NOT implement the Copy trait. Assignment and parameter passing transfer ownership via move semantics.

**Formal statement:**

```
∀s : String. s ∉ Copy
```

**Proof sketch:** String contains `own Vec<u8>` which owns heap memory. Copying would require deep copy of heap buffer, violating Copy trait semantics (bitwise copy). Therefore String is move-only.

**Example:**

```cantrip
let s1: own String = String.from("hello")
let s2: own String = move s1  // Explicit move required
// s1 no longer accessible

// Copying would require:
let s3: own String = s2.clone()  // Explicit clone for deep copy
```

**Theorem 5.6.2 (UTF-8 Invariant Preservation):**

All String values satisfy the UTF-8 validity invariant. All String operations preserve this invariant.

**Formal statement:**

```
∀s : String. valid_utf8(s.ptr, s.len) = true

∀op ∈ StringOps. (valid_utf8(s) ∧ s' = op(s)) ⇒ valid_utf8(s')
```

**Proof sketch:** String API only accepts UTF-8 validated input (`str` slices, which are validated). Internal mutations preserve byte boundaries. Therefore invariant is maintained.

**Property 5.6.1 (Memory Layout):**

String has the following memory characteristics:

```
sizeof(String) = 3 × sizeof(usize) = 24 bytes (on 64-bit systems)
alignof(String) = alignof(usize) = 8 bytes (on 64-bit systems)
```

**Memory representation:**

```
String {
    ptr: *u8,      // 8 bytes (64-bit pointer)
    len: usize,    // 8 bytes (current length)
    cap: usize,    // 8 bytes (allocated capacity)
}
```

**Property 5.6.2 (Permission Semantics):**

String interacts with the permission system as follows:

| Permission | Syntax       | Semantics          | Operations Allowed                             |
| ---------- | ------------ | ------------------ | ---------------------------------------------- |
| Immutable  | `String`     | Borrowed reference | Read-only methods (len, is_empty, chars, etc.) |
| Mutable    | `mut String` | Mutable reference  | Modification methods (push_str, clear, etc.)   |
| Owned      | `own String` | Full ownership     | All operations, destroyed at scope end         |

**Note:** Unlike Rust, Cantrip permits multiple `mut String` references simultaneously. Programmer is responsible for ensuring safe usage.

**Example:**

```cantrip
function permissions_demo()
    uses alloc.heap
{
    let s: own String = String.from("base")

    // Immutable reference
    let len: usize = s.len()  // OK: s implicitly borrowed as String

    // Mutable reference
    var s_mut = s  // s_mut has type own String
    s_mut.push_str(" text")  // OK: can mutate owned string

    // Multiple mut refs (allowed in Cantrip!)
    let ref1 = mut s_mut
    let ref2 = mut s_mut
    // Programmer responsible for safe usage
}
```

**Property 5.6.3 (Allocation Requirements):**

String operations requiring heap allocation must declare the `alloc.heap` effect:

```
String.new() : own String ! {alloc.heap}
String.from(str) : own String ! {alloc.heap}
String.push_str(mut String, str) ! {alloc.heap}
```

**Rationale:** Explicit effect tracking makes memory allocation visible in function signatures, enabling resource budget analysis and constraint enforcement.

**Property 5.6.4 (Slice Relationship):**

String and str are related but distinct types:

```
String ≠ str

String → str  (via deref coercion or explicit .as_str())
str → String  (via String.from(), must allocation)
```

**Relationship rules:**

```
[String-To-Slice]
Γ ⊢ s : String
────────────────────────────
Γ ⊢ s : str  (deref coercion)

[Slice-To-String]
Γ ⊢ s : str
────────────────────────────
Γ ⊢ String.from(s) : own String ! {alloc.heap}
```

**Example:**

```cantrip
function accepts_slice(text: str) {
    // Read-only access to text
}

function example()
    uses alloc.heap
{
    let owned: own String = String.from("data")

    // String can be passed where str expected (deref coercion)
    accepts_slice(owned)  // OK: String → str

    // str to String must allocation
    let slice: str = "literal"
    let owned2: own String = String.from(slice)  // Allocates
}
```

**Theorem 5.6.3 (String Destruction):**

When an owned String goes out of scope, its heap memory is automatically deallocated.

**Formal statement:**

```
{s : own String}
block { ... }
{deallocated(s.ptr)}
```

**Operational semantics:**

```
[E-String-Destroy]
⟨block, σ[s ↦ string_val(ptr, len, cap)]⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────────
σ' = σ \ {ptr}  ∧  free_heap(ptr, cap)
```

**Example:**

```cantrip
function automatic_cleanup()
    uses alloc.heap
{
    {
        let temp: own String = String.from("temporary")
        // Use temp
    } // temp.ptr deallocated here automatically

    // temp no longer accessible
}
```

##### 2.1.6.4 Dynamic Semantics

###### 2.1.6.4.1 Evaluation Rules

**String literal evaluation:**

String literals evaluate to String values pointing to static read-only memory (.rodata section). No heap allocation occurs during evaluation.

```
[E-String-Literal]
"text" stored at static address addr
len = byte_length("text")
─────────────────────────────────────────────────────────────
⟨"text", σ⟩ ⇓ ⟨String{ptr: addr, len: len, cap: len}, σ⟩
```

**Explanation:** String literals are embedded in the executable's read-only data section. The resulting String value is a dense pointer to this static memory. Since no heap allocation is performed, the store σ remains unchanged.

**String heap allocation:**

Creating a new owned string allocates an empty buffer on the heap with default capacity.

```
[E-String-New]
alloc_heap(DEFAULT_CAP) = addr
─────────────────────────────────────────────────────────────
⟨String.new(), σ⟩ ⇓ ⟨String{ptr: addr, len: 0, cap: DEFAULT_CAP}, σ'⟩
    where σ' = σ ∪ {addr ↦ [0u8; DEFAULT_CAP]}
```

**Explanation:** String.new() allocates a heap buffer and returns an owned String with length 0. The capacity (DEFAULT_CAP) is implementation-defined but typically 8-16 bytes. The store σ' includes the new allocation.

**String from UTF-8 validation:**

Constructing a string from untrusted bytes must validation to ensure UTF-8 correctness.

```
[E-String-From-UTF8-Valid]
⟨bytes, σ⟩ ⇓ ⟨data, σ'⟩
validate_utf8(data) = true
alloc_heap(data.len) = addr
memcpy(addr, data.ptr, data.len)
─────────────────────────────────────────────────────────────
⟨String.from_utf8(bytes), σ⟩ ⇓ ⟨Ok(String{ptr: addr, len: data.len, cap: data.len}), σ''⟩
    where σ'' = σ' ∪ {addr ↦ data}

[E-String-From-UTF8-Invalid]
⟨bytes, σ⟩ ⇓ ⟨data, σ'⟩
validate_utf8(data) = false
─────────────────────────────────────────────────────────────
⟨String.from_utf8(bytes), σ⟩ ⇓ ⟨Err(Utf8Error), σ'⟩
```

**Explanation:** UTF-8 validation is performed in O(n) time where n = byte length. Valid sequences result in heap allocation and Ok(String). Invalid sequences return Err without allocating.

**String concatenation:**

Concatenating two strings may require reallocation if capacity is exceeded.

```
[E-String-Concat-No-Realloc]
⟨s1, σ⟩ ⇓ ⟨String{ptr: p1, len: l1, cap: c1}, σ'⟩
⟨s2, σ'⟩ ⇓ ⟨String{ptr: p2, len: l2, cap: c2}, σ''⟩
l1 + l2 ≤ c1
memcpy(p1 + l1, p2, l2)
─────────────────────────────────────────────────────────────
⟨s1.push_str(s2), σ⟩ ⇓ ⟨(), σ'''⟩
    where σ'''(p1).len = l1 + l2

[E-String-Concat-Realloc]
⟨s1, σ⟩ ⇓ ⟨String{ptr: p1, len: l1, cap: c1}, σ'⟩
⟨s2, σ'⟩ ⇓ ⟨String{ptr: p2, len: l2, cap: c2}, σ''⟩
l1 + l2 > c1
new_cap = max(c1 × 2, l1 + l2)
alloc_heap(new_cap) = addr
memcpy(addr, p1, l1)
memcpy(addr + l1, p2, l2)
dealloc_heap(p1)
─────────────────────────────────────────────────────────────
⟨s1.push_str(s2), σ⟩ ⇓ ⟨(), σ'''⟩
    where σ''' = (σ'' \ {p1 ↦ _}) ∪ {addr ↦ [l1 + l2 bytes]}
          s1 = String{ptr: addr, len: l1 + l2, cap: new_cap}
```

**Explanation:** If the existing capacity is sufficient, bytes are appended in-place. Otherwise, a new buffer is allocated with doubled capacity (or exact size if doubling is insufficient), existing data is copied, new data is appended, and the old buffer is freed.

**String slicing:**

Slicing a string creates a new string view (borrows the underlying data).

```
[E-String-Slice]
⟨s, σ⟩ ⇓ ⟨String{ptr: p, len: l, cap: c}, σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b ≤ l
is_char_boundary(p, a) ∧ is_char_boundary(p, b)
─────────────────────────────────────────────────────────────
⟨s[start..end], σ⟩ ⇓ ⟨String{ptr: p + a, len: b - a, cap: b - a}, σ'''⟩

[E-String-Slice-Invalid-Boundary]
⟨s, σ⟩ ⇓ ⟨String{ptr: p, len: l, cap: c}, σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b ≤ l
¬(is_char_boundary(p, a) ∧ is_char_boundary(p, b))
─────────────────────────────────────────────────────────────
⟨s[start..end], σ⟩ ⇓ panic("byte index not at char boundary")
```

**Explanation:** String slicing must occur at UTF-8 character boundaries. A byte index is a character boundary if it is 0, equal to the string length, or points to a byte that is not a UTF-8 continuation byte (i.e., not in range 0x80-0xBF). Invalid boundaries cause runtime panic.

**Character boundary predicate:**

```
is_char_boundary(ptr, idx) ⟺
    idx = 0 ∨
    idx = len ∨
    (ptr[idx] & 0xC0) ≠ 0x80
```

###### 2.1.6.4.2 Memory Representation

**Record Layout:**

The String type is represented as a dense pointer with three fields:

```
String record layout (24 bytes on 64-bit):
┌─────────────────┬──────────────┬──────────────┐
│ ptr: *u8        │ len: usize   │ cap: usize   │
│ 8 bytes         │ 8 bytes      │ 8 bytes      │
└─────────────────┴──────────────┴──────────────┘
Offset:  0              8               16

Field Semantics:
• ptr:  Pointer to UTF-8 encoded byte array
• len:  Number of BYTES currently used (NOT character count)
• cap:  Total capacity in BYTES (for owned strings only)
```

**Size and alignment:**

```
sizeof(String)  = 24 bytes  (on 64-bit platforms)
                = 12 bytes  (on 32-bit platforms)
alignof(String) = 8 bytes   (on 64-bit platforms)
                = 4 bytes   (on 32-bit platforms)
```

**Important distinctions:**

- `len` represents byte length, not character count
- `cap` represents allocated byte capacity
- Character count must O(n) iteration over UTF-8 sequences

**String literal storage:**

String literals are stored in the executable's read-only data section:

```
.rodata section (static memory):
┌─────────────────────────────────┐
│ string_0: "Hello, World!\0"     │ ← addr_0
│ string_1: "Cantrip\0"           │ ← addr_1
│ string_2: "🦀 Rust inspired\0"  │ ← addr_2
└─────────────────────────────────┘

String literal expression "Hello, World!" evaluates to:
String {
    ptr: addr_0,
    len: 13,      // byte length (no null terminator in len)
    cap: 13,
}
```

**Properties of literal strings:**

- Immutable (stored in read-only memory)
- Zero allocation cost (no heap usage)
- Lifetime: 'static (valid for entire program execution)
- Null-terminated in .rodata for C FFI, but null not included in len

**Heap-allocated strings:**

Owned strings allocate a contiguous buffer on the heap:

```
Heap memory layout:
┌──────────────────────────────────────┐
│ UTF-8 byte array                     │
│ [capacity bytes allocated]           │
│ [first 'len' bytes used]             │
│ [remaining bytes unused/uninitialized]│
└──────────────────────────────────────┘
          ↑
          │ (points to start)
          │
String { ptr: ─┘, len: used_bytes, cap: total_allocated }
```

**Example:**

```cantrip
let mut s: own String = String.new()  // Allocate with DEFAULT_CAP
s.push_str("Hello")                   // Append 5 bytes

Heap state:
┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
│H│e│l│l│o│?│?│?│?│?│?│?│?│?│?│?│  (assuming DEFAULT_CAP = 16)
└─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
 ↑ len = 5              ↑ cap = 16
 ptr
```

**UTF-8 byte encoding in memory:**

Characters are stored as UTF-8 byte sequences:

```
Character: 'A'     → [0x41]                     (1 byte, ASCII)
Character: '©'     → [0xC2, 0xA9]               (2 bytes)
Character: '中'    → [0xE4, 0xB8, 0xAD]         (3 bytes)
Character: '🦀'    → [0xF0, 0x9F, 0xA6, 0x80]   (4 bytes, emoji)

String "A©中🦀" memory layout:
┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
│0x41│0xC2│0xA9│0xE4│0xB8│0xAD│0xF0│0x9F│0xA6│0x80│
└────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
  A      ©           中              🦀
 1 byte  2 bytes    3 bytes         4 bytes

Total: 10 bytes, 4 characters
len = 10 (byte length)
s.chars().count() = 4 (character count, must iteration)
```

**Memory layout theorem:**

**Theorem 5.6.4 (String Memory Invariants):**

For any String value `s = String{ptr: p, len: l, cap: c}`, the following invariants hold:

1. **Capacity constraint:** `0 ≤ l ≤ c`
2. **UTF-8 validity:** `validate_utf8(p[0..l]) = true`
3. **Allocation validity:** If `c > 0`, then `p` points to valid heap allocation of size `c`
4. **Null-pointer invariant:** If `c = 0`, then `p` may be null or dangling (zero-capacity strings need not allocate)
5. **Alignment:** `p` is aligned to 1-byte boundary (no alignment requirement for u8 arrays)

**Proof sketch:**

- Invariant (1) enforced by all String operations (construction, push, etc.)
- Invariant (2) enforced by UTF-8 validation in String.from_utf8() and maintained by all operations
- Invariants (3-4) enforced by allocator interface and String.new() semantics
- Invariant (5) follows from u8 alignment requirements (trivially satisfied)

###### 2.1.6.4.3 Operational Behavior

**Allocation strategy:**

**String literals (zero-cost):**

- Storage: Read-only data section (.rodata)
- Allocation: None (embedded in executable)
- Mutability: Immutable (any attempt to modify causes compiler error or runtime fault)
- Lifetime: 'static (valid for entire program execution)
- Cost: Zero runtime overhead

**Owned strings (heap-allocated):**

- Storage: Heap memory
- Allocation: Dynamic via allocator (uses `alloc.heap`)
- Mutability: Mutable if declared with `mut`
- Lifetime: Lexically scoped (freed when owner goes out of scope)
- Cost: Allocation/deallocation overhead, possible reallocation on growth

**Growth strategy (capacity management):**

When appending data exceeds current capacity, strings employ an amortized doubling strategy:

```
new_capacity = max(old_capacity × 2, required_size)
```

**Example growth sequence:**

```
Initial:     cap = 0,  len = 0
After "Hi":  cap = 8,  len = 2   (allocated with DEFAULT_CAP)
After 6 more bytes (len=8):  cap = 8,  len = 8   (fits, no realloc)
After 1 more byte (len=9):   cap = 16, len = 9   (realloc: 8 × 2 = 16)
After 7 more bytes (len=16): cap = 16, len = 16  (fits, no realloc)
After 1 more byte (len=17):  cap = 32, len = 17  (realloc: 16 × 2 = 32)
```

**Amortized analysis:**

- Individual push operations: O(1) amortized
- Worst case (reallocation): O(n) where n = current length
- Total cost of n pushes: O(n) amortized

**Deallocation behavior:**

Owned strings (`own String`) are automatically deallocated when their owner's scope ends:

```cantrip
function example()
    uses alloc.heap
{
    let s: own String = String.new()  // Allocate heap buffer
    s.push_str("data")                // Append data (may realloc)
    // Automatic deallocation occurs here
}
```

**Deallocation rule:**

```
[E-String-Dealloc]
⟨{ let s: own String = e; body }, σ⟩ ⇓ ⟨v, σ'⟩
s = String{ptr: p, len: l, cap: c}
c > 0
─────────────────────────────────────────────────────────────
final σ'' = σ' \ {p ↦ _}  (heap buffer freed)
```

**Exception:** Zero-capacity strings (empty string constants) may not allocate, hence no deallocation.

**Permission transitions:**

Unlike Rust, Cantrip allows multiple mutable references through region-based lifetime management:

```
Ownership transitions:
own String ────→ String      (pass as immutable reference)
own String ────→ mut String  (pass as mutable reference)

Region guarantees:
• Multiple `mut String` references can coexist
• Region system ensures lifetime safety through escape analysis
• All references must not outlive the region containing the owned data
```

**Example:**

```cantrip
function process(s1: mut String, s2: mut String)
    uses alloc.heap
{
    s1.push_str("A")
    s2.push_str("B")  // OK: multiple mutable references allowed
}

let mut owner: own String = String.new()
process(owner, owner)  // OK in Cantrip (unlike Rust)
                        // Regions ensure owner outlives both references
```

**Performance characteristics:**

```
Operation              Time Complexity    Space Complexity
──────────────────────────────────────────────────────────
String literal         O(1)               O(1) - static memory
String.new()           O(1)               O(cap) - heap allocation
push_str(s)            O(m) amortized     O(m) worst case (realloc)
                       where m = s.len
s[a..b]                O(1)               O(1) - dense pointer creation
s.chars()              O(n)               O(1) - iterator creation
s.chars().count()      O(n)               O(1)
validate_utf8(s)       O(n)               O(1)
──────────────────────────────────────────────────────────
where n = byte length of string
```

**Memory overhead:**

- String struct: 24 bytes (on 64-bit)
- Heap allocation: len + (cap - len) bytes
- Minimum overhead: 24 bytes + 0-16 bytes (DEFAULT_CAP)

**Cache efficiency:**

- UTF-8 data stored contiguously for excellent cache locality
- Sequential iteration exhibits near-optimal cache behavior
- Random character access must O(n) scan (UTF-8 is variable-width)

##### 2.1.6.5 Additional Properties

**Definition 2.1.6.3 (UTF-8 Encoding):** UTF-8 is a variable-width character encoding for Unicode, using 1-4 bytes per Unicode scalar value. All String values in Cantrip MUST contain valid UTF-8 byte sequences.

###### 2.1.6.5.1 Invariants

UTF-8 encodes Unicode scalar values (U+0000 to U+10FFFF, excluding surrogates U+D800-U+DFFF) using 1-4 bytes:

**1-byte sequence (ASCII range: U+0000 to U+007F):**

```
Byte pattern: 0xxxxxxx
Range:        U+0000 to U+007F (0 to 127 decimal)
Bits:         7 bits for codepoint
```

**Examples:**

```
'A'   (U+0041) → [01000001]        → 0x41
'0'   (U+0030) → [00110000]        → 0x30
'\n'  (U+000A) → [00001010]        → 0x0A
' '   (U+0020) → [00100000]        → 0x20
```

**2-byte sequence (U+0080 to U+07FF):**

```
Byte pattern: 110xxxxx 10xxxxxx
Range:        U+0080 to U+07FF (128 to 2,047 decimal)
Bits:         11 bits for codepoint (5 + 6)
```

**Examples:**

```
'©'   (U+00A9) → [11000010 10101001] → 0xC2 0xA9
'é'   (U+00E9) → [11000011 10101001] → 0xC3 0xA9
'Ω'   (U+03A9) → [11001110 10101001] → 0xCE 0xA9
```

**3-byte sequence (U+0800 to U+FFFF, excluding surrogates):**

```
Byte pattern: 1110xxxx 10xxxxxx 10xxxxxx
Range:        U+0800 to U+FFFF (2,048 to 65,535 decimal)
              EXCLUDING U+D800 to U+DFFF (surrogate pairs)
Bits:         16 bits for codepoint (4 + 6 + 6)
```

**Examples:**

```
'中'  (U+4E2D) → [11100100 10111000 10101101] → 0xE4 0xB8 0xAD
'€'   (U+20AC) → [11100010 10000010 10101100] → 0xE2 0x82 0xAC
'—'   (U+2014) → [11100010 10000000 10010100] → 0xE2 0x80 0x94
```

**4-byte sequence (U+10000 to U+10FFFF):**

```
Byte pattern: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
Range:        U+10000 to U+10FFFF (65,536 to 1,114,111 decimal)
Bits:         21 bits for codepoint (3 + 6 + 6 + 6)
```

**Examples:**

```
'🦀'  (U+1F980) → [11110000 10011111 10100110 10000000] → 0xF0 0x9F 0xA6 0x80
'🚀'  (U+1F680) → [11110000 10011111 10011010 10000000] → 0xF0 0x9F 0x9A 0x80
'😀'  (U+1F600) → [11110000 10011111 10011000 10000000] → 0xF0 0x9F 0x98 0x80
```

**Invariant 5.6.1 (UTF-8 Validity):** All String values MUST contain valid UTF-8 at all times.

**Formally:**

```
�^?s : String. validate_utf8(s.as_bytes()) = true
```

**Operations that maintain invariant:**

1. **String literals (compiler validated):**

   ```cantrip
   let s = "Hello, �,-�O"  // Compiler will UTF-8 validity
   ```

2. **String.new() (creates empty valid string):**

   ```cantrip
   let s = String.new()  // Empty string is trivially valid UTF-8
   ```

3. **String.from_utf8() (validates or errors):**

   ```cantrip
   let s = String.from_utf8(bytes)?  // Returns Err if invalid
   ```

4. **push_str(valid_string) (concatenation preserves validity):**

   ```cantrip
   s1.push_str(s2)  // If s1 and s2 valid, result is valid
   ```

5. **Slicing at character boundaries:**
   ```cantrip
   let slice = s[0..5]  // Panics if not at char boundary
   ```

**Theorem 5.6.5 (UTF-8 Invariant Preservation):**

If `s1` and `s2` are valid UTF-8 strings, then:

1. `s1.push_str(s2)` produces a valid UTF-8 string
2. `s1 + s2` (if concatenation operator exists) produces a valid UTF-8 string
3. `s[a..b]` produces a valid UTF-8 string if `a` and `b` are character boundaries

**Proof sketch:**

1. Valid UTF-8 sequences concatenated produce a valid UTF-8 sequence
2. Slicing at character boundaries preserves sequence validity
3. No operation modifies the byte content except by appending valid UTF-8

###### 2.1.6.5.2 Verification

Verification will that all byte sequences stored in String instances satisfy the UTF-8 invariant before being exposed to user code.

##### Invalid Sequences

**Normative requirement:** The following byte sequences are INVALID in UTF-8 and MUST be rejected by validation:

**1. Overlong encodings:**

Encoding a codepoint with more bytes than necessary.

```
Invalid: 'A' (U+0041) as [11000001 10000001] → 0xC1 0x81  (2-byte overlong)
Valid:   'A' (U+0041) as [01000001]           → 0x41      (1-byte correct)

Invalid: '©' (U+00A9) as [11100000 10000010 10101001] → 0xE0 0x82 0xA9  (3-byte overlong)
Valid:   '©' (U+00A9) as [11000010 10101001]           → 0xC2 0xA9      (2-byte correct)
```

**Detection rule:** A sequence is overlong if the codepoint value could be encoded in fewer bytes.

**2. UTF-16 surrogate halves (U+D800 to U+DFFF):**

These codepoints are reserved for UTF-16 encoding and are invalid in UTF-8.

```
Invalid: [11101101 10100000 10000000] → 0xED 0xA0 0x80  (U+D800, high surrogate)
Invalid: [11101101 10111111 10111111] → 0xED 0xBF 0xBF  (U+DFFF, low surrogate)
```

**Rationale:** UTF-8 encodes Unicode scalar values directly; surrogate pairs are a UTF-16 encoding artifact.

**3. Codepoints beyond U+10FFFF:**

Unicode defines codepoints only up to U+10FFFF.

```
Invalid: [11110100 10010000 10000000 10000000] → 0xF4 0x90 0x80 0x80  (U+110000)
Invalid: [11111000 10000000 10000000 10000000] → 0xF8 0x80 0x80 0x80  (5-byte sequence)
```

**Maximum valid codepoint:** U+10FFFF

**4. Incomplete sequences:**

Missing continuation bytes.

```
Invalid: [11000010]                 → 0xC2      (2-byte lead, missing continuation)
Invalid: [11100100 10111000]        → 0xE4 0xB8 (3-byte lead, only 1 continuation)
Invalid: [11110000 10011111 10100110] → 0xF0 0x9F 0xA6 (4-byte lead, only 2 continuations)
```

**5. Invalid continuation bytes:**

Continuation bytes outside the range 0x80-0xBF.

```
Invalid: [11000010 01000001]        → 0xC2 0x41 (second byte not in 0x80-0xBF)
Invalid: [11100100 11000000 10101101] → 0xE4 0xC0 0xAD (second byte not continuation)
```

**6. Unexpected continuation bytes:**

Continuation bytes without a leading byte.

```
Invalid: [10101001]                 → 0xA9      (continuation byte with no lead)
Invalid: [01000001 10101001]        → 0x41 0xA9 (continuation after ASCII)
```

**7. Invalid leading bytes:**

Bytes 0xC0, 0xC1, 0xF5-0xFF are never valid UTF-8 leading bytes.

```
Invalid: [11000000 10000000]        → 0xC0 0x80 (invalid leading byte 0xC0)
Invalid: [11111000 10000000 ...]    → 0xF8 ...  (5-byte sequence, invalid)
```

##### Validation Procedure

**Validation requirement:** All String construction from untrusted bytes MUST perform UTF-8 validation.

**Validation algorithm complexity:** O(n) where n = byte length

**Validation predicate:**

```
validate_utf8(bytes: [u8]) → bool:
    let i = 0
    while i < bytes.len:
        let b0 = bytes[i]

        // 1-byte sequence (ASCII)
        if b0 < 0x80:
            i += 1
            continue

        // Invalid leading bytes
        if b0 < 0xC2 || b0 > 0xF4:
            return false

        // 2-byte sequence
        if b0 < 0xE0:
            if i + 1 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]):
                return false
            let cp = ((b0 & 0x1F) << 6) | (bytes[i + 1] & 0x3F)
            if cp < 0x80:
                return false  // overlong
            i += 2
            continue

        // 3-byte sequence
        if b0 < 0xF0:
            if i + 2 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]) || !is_continuation(bytes[i + 2]):
                return false
            let cp = ((b0 & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F)
            if cp < 0x800:
                return false  // overlong
            if 0xD800 ≤ cp ≤ 0xDFFF:
                return false  // surrogate
            i += 3
            continue

        // 4-byte sequence
        if b0 < 0xF5:
            if i + 3 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]) || !is_continuation(bytes[i + 2]) || !is_continuation(bytes[i + 3]):
                return false
            let cp = ((b0 & 0x07) << 18) | ((bytes[i + 1] & 0x3F) << 12) | ((bytes[i + 2] & 0x3F) << 6) | (bytes[i + 3] & 0x3F)
            if cp < 0x10000:
                return false  // overlong
            if cp > 0x10FFFF:
                return false  // out of range
            i += 4
            continue

        return false  // invalid leading byte

    return true

is_continuation(byte: u8) → bool:
    return (byte & 0xC0) == 0x80
```

**Safe construction API:**

```cantrip
function String.from_utf8(bytes: [u8]): Result<own String, Utf8Error>
    uses alloc.heap
{
    if validate_utf8(bytes) {
        let mut s = String.new()
        s.reserve(bytes.len)
        memcpy(s.ptr, bytes.ptr, bytes.len)
        s.len = bytes.len
        Ok(s)
    } else {
        Err(Utf8Error::InvalidSequence)
    }
}
```

**Unsafe operations (programmer responsibility):**

Unsafe operations bypass validation and place the burden on the programmer:

```cantrip
// UNSAFE: Programmer must ensure `bytes` contains valid UTF-8
function String.from_utf8_unchecked(bytes: [u8]): own String
    uses alloc.heap, unsafe.memory
{
    // No validation performed
    let mut s = String.new()
    s.reserve(bytes.len)
    memcpy(s.ptr, bytes.ptr, bytes.len)
    s.len = bytes.len
    s
}
```

###### 2.1.6.5.3 Algorithms

##### UTF-8 Encoding

**Encoding a Unicode scalar value to UTF-8:**

```
encode_utf8(codepoint: u32) → [u8]:
    if codepoint ≤ 0x7F:
        return [codepoint]
    else if codepoint ≤ 0x7FF:
        return [
            0xC0 | (codepoint >> 6),
            0x80 | (codepoint & 0x3F)
        ]
    else if codepoint ≤ 0xFFFF:
        if 0xD800 ≤ codepoint ≤ 0xDFFF:
            error("surrogate codepoint invalid")
        return [
            0xE0 | (codepoint >> 12),
            0x80 | ((codepoint >> 6) & 0x3F),
            0x80 | (codepoint & 0x3F)
        ]
    else if codepoint ≤ 0x10FFFF:
        return [
            0xF0 | (codepoint >> 18),
            0x80 | ((codepoint >> 12) & 0x3F),
            0x80 | ((codepoint >> 6) & 0x3F),
            0x80 | (codepoint & 0x3F)
        ]
    else:
        error("codepoint out of range")
```

##### Character Iteration

**Iterating over characters (not bytes):**

String provides an iterator that yields `char` values by decoding UTF-8:

```cantrip
function String.chars(self: String): CharIterator {
    CharIterator { ptr: self.ptr, end: self.ptr + self.len }
}

impl Iterator<char> for CharIterator {
    function next(mut self): Option<char> {
        if self.ptr >= self.end {
            return None
        }

        let (ch, byte_len) = decode_utf8_char(self.ptr)
        self.ptr += byte_len
        Some(ch)
    }
}
```

**UTF-8 decoding algorithm:**

```
decode_utf8_char(ptr: *u8) → (char, usize):
    let b0 = *ptr

    if b0 < 0x80:  // 1-byte (ASCII)
        return (b0 as char, 1)

    if b0 < 0xE0:  // 2-byte
        let b1 = *(ptr + 1)
        let cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F)
        return (char::from_u32_unchecked(cp), 2)

    if b0 < 0xF0:  // 3-byte
        let b1 = *(ptr + 1)
        let b2 = *(ptr + 2)
        let cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F)
        return (char::from_u32_unchecked(cp), 3)

    // 4-byte
    let b1 = *(ptr + 1)
    let b2 = *(ptr + 2)
    let b3 = *(ptr + 3)
    let cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F)
    return (char::from_u32_unchecked(cp), 4)
```

**Note:** `from_u32_unchecked` is safe here because the String invariant guarantees valid UTF-8.

**Example:**

```cantrip
let s = "A©中🦀"
for ch in s.chars() {
    // Yields: 'A', '©', '中', '🦀'
}

// Byte iteration (raw UTF-8 bytes)
for byte in s.as_bytes() {
    // Yields: 0x41, 0xC2, 0xA9, 0xE4, 0xB8, 0xAD, 0xF0, 0x9F, 0xA6, 0x80
}
```

##### 2.1.6.6 Advanced Features

###### 2.1.6.6.1 Permission Integration

**Definition 2.1.6.4 (String Permissions):** The `String` type interacts with Cantrip's Lexical Permission System through three permission modes: immutable reference (default), mutable reference (`mut`), and owned (`own`).

**Philosophy:** Cantrip follows the Cyclone model, not Rust. Permissions control _access rights_, while regions control _lifetimes_. Multiple mutable references are allowed—programmer's responsibility to prevent aliasing bugs.

####### 2.1.6.6.1.1 Immutable Reference (Default)

**Syntax:** `String` or `text: String`

**Type rule:**

```
[T-String-Ref]
Γ ⊢ s : String    immutable(s)
─────────────────────────────────
Γ ⊢ function f(x: String) accepts reference to s
```

**Semantics:**

- Can read string contents
- Cannot modify contents
- Cannot take ownership
- Many references can coexist (permission-based model)

**Operations allowed:**

```cantrip
function get_length(s: String): usize {
    s.len()              // OK: reading
}

function get_char(s: String, idx: usize): char {
    s.char_at(idx)       // OK: indexing
}

function compare(s1: String, s2: String): bool {
    s1 == s2             // OK: comparison
}
```

**Operations prohibited:**

```cantrip
function illegal_modify(s: String) {
    s.push_str("!")     // ERROR E3002: Cannot modify immutable reference
    s.clear()           // ERROR E3002: Cannot modify immutable reference
}
```

**Example - Multiple references allowed:**

```cantrip
function process_multiple(text: String)
    uses io.write
{
    let len1 = get_length(text)      // OK: first reference
    let len2 = get_length(text)      // OK: can pass again
    compare(text, text)              // OK: two references simultaneously

    // text still accessible here
    std.io.println(text)
}
```

**Formal semantics:**

```
⟦String⟧ = { reference to UTF-8 byte sequence }
Operations: Read-only (len, char_at, slice, ==, <, etc.)
Aliasing: Unlimited
```

####### 2.1.6.6.1.2 Mutable Reference

**Syntax:** `mut String` or `text: mut String`

**Type rule:**

```
[T-String-Mut]
Γ ⊢ s : String    mutable(s) = true
─────────────────────────────────────
Γ ⊢ s : mut String
```

**Semantics:**

- Can read and modify string contents
- Cannot take ownership (remains with caller)
- Multiple `mut` references CAN coexist (programmer's responsibility!)

**Operations allowed:**

```cantrip
function modify_string(s: mut String) {
    s.push_str("!")           // OK: can append
    s.clear()                 // OK: can clear
    s[0] = 'X'                // OK: can modify bytes (if valid UTF-8)
    let len = s.len()         // OK: can also read
}
```

**Example - Basic mutation:**

```cantrip
function append_exclamation(s: mut String) {
    s.push_str("!")
}

function example()
    uses alloc.heap
{
    var text: own String = String.from("Hello")
    append_exclamation(mut text)
    // text now contains "Hello!"
    std.io.println(text)  // Prints: Hello!
}
```

**CRITICAL DIFFERENCE FROM RUST:** Multiple mutable references allowed:

```cantrip
function modify1(s: mut String) {
    s.push_str(" world")
}

function modify2(s: mut String) {
    s.push_str("!")
}

function example()
    uses alloc.heap
{
    var text: own String = String.from("Hello")

    // Both allowed simultaneously - permission-based aliasing
    modify1(mut text)
    modify2(mut text)

    // Result: "Hello world!"
    // Programmer must ensure correct ordering!
}
```

**Aliasing responsibility:**

```cantrip
// UNSAFE PATTERN - Programmer's responsibility to avoid!
function dangerous_pattern()
    uses alloc.heap
{
    var text: own String = String.from("data")

    let ref1: mut String = mut text
    let ref2: mut String = mut text

    // Both ref1 and ref2 can modify - could cause bugs
    // Cantrip allows this, but programmer must be careful!
}
```

**Formal semantics:**

```
⟦mut String⟧ = { mutable reference to UTF-8 byte sequence }
Operations: Read, Write (push_str, clear, insert, remove, etc.)
Aliasing: Unlimited (programmer will safety)
Lifetime: Lexical scope
```

####### 2.1.6.6.1.3 Owned Permission

**Syntax:** `own String`

**Type rule:**

```
[T-String-Own]
Γ ⊢ e : String    copyable(String) = false
─────────────────────────────────────────────
Γ ⊢ e : own String
```

**Semantics:**

- Full ownership of the string
- Can read, modify, move, destroy
- Automatically destroyed when scope ends (no memory leaks)
- Exactly one owner (enforced by move semantics)

**Operations - Full control:**

```cantrip
function example()
    uses alloc.heap
{
    let s: own String = String.new()

    s.push_str("data")     // Can modify (owner has all rights)
    let len = s.len()      // Can read
    consume(move s)        // Can transfer ownership
    // s no longer accessible here
}
```

**Creating owned strings:**

```cantrip
function create_greeting(): own String
    uses alloc.heap
{
    var s: own String = String.new()
    s.push_str("Hello, ")
    s.push_str("World!")
    s  // Return ownership to caller
}
```

**Automatic destruction:**

```cantrip
function auto_cleanup()
    uses alloc.heap
{
    let temp: own String = String.from("temporary")
    temp.push_str(" data")
    // temp automatically freed here (no explicit drop needed)
}
```

**Move semantics:**

```cantrip
function consume_string(text: own String) {
    std.io.println(text)
    // text destroyed here
}

function example()
    uses alloc.heap
{
    let message: own String = String.from("Hello")

    consume_string(move message)  // Explicit transfer
    // message.len()  // ERROR E3004: Value moved
}
```

**Formal semantics:**

```
⟦own String⟧ = { unique pointer to UTF-8 byte sequence }
Operations: Read, Write, Move, Destroy
Aliasing: Exactly one owner
Lifetime: Lexical scope (automatic destruction)
Destruction: RAII-style cleanup
```

####### 2.1.6.6.1.4 Permission Transitions

**Subtyping hierarchy:**

```
[Sub-String-Permission]
own String <: mut String <: String
```

**Explicit transitions:**

```cantrip
let s: own String = String.from("data")

// 1. Pass as immutable reference (keep ownership)
read(s)           // s: String (reference)

// 2. Pass as mutable reference (keep ownership)
modify(mut s)     // s: mut String (mutable reference)

// 3. Transfer ownership (lose access)
consume(move s)   // s: own String (moved)
// s no longer accessible
```

**Complex example showing all transitions:**

```cantrip
function demonstrate_permissions()
    uses alloc.heap, io.write
{
    var text: own String = String.from("initial")

    // Transition 1: own → reference (temporary, keep ownership)
    let len1 = get_length(text)
    std.io.println(format!("Length: {}", len1))

    // Transition 2: own → mut reference (temporary, keep ownership)
    append_data(mut text)

    // Transition 3: own → reference again
    let len2 = get_length(text)

    // Transition 4: own → own (permanent move)
    send_to_logger(move text)

    // text no longer accessible
}
```

####### 2.1.6.6.1.5 Safety Guarantees

**What Cantrip PREVENTS:**

1. **Use-after-free** (regions enforce lifetime):

```cantrip
function safe_example(): String {
    region temp {
        let s = String.new_in<temp>()
        s.push_str("data")
        // return s  // ERROR E8301: Cannot escape region
    }
}
```

2. **Double-free** (one owner only):

```cantrip
function safe_example()
    uses alloc.heap
{
    let s = String.from("data")
    consume(move s)
    // consume(move s)  // ERROR E3004: Value already moved
}
```

3. **Memory leaks** (automatic destruction):

```cantrip
function safe_example()
    uses alloc.heap
{
    let s = String.from("data")
    // No need for explicit free - automatic cleanup
}  // s destroyed here, memory reclaimed
```

**What Cantrip does NOT prevent (programmer's responsibility):**

1. **Aliasing bugs** (multiple `mut` refs allowed):

```cantrip
// ALLOWED but potentially dangerous
var text = String.from("data")
modify1(mut text)
modify2(mut text)  // Could conflict with modify1
```

2. **Data races** (no automatic synchronization):

```cantrip
// Programmer must use proper synchronization for threads
thread1.spawn(|| modify(mut shared_string))
thread2.spawn(|| modify(mut shared_string))  // Race condition!
```

3. **Modification during iteration** (no iterator invalidation protection):

```cantrip
var text = String.from("data")
for c in text.chars() {
    text.push_str("x")  // ALLOWED but dangerous!
}
```

**Error codes:**

- `E3002` — Attempted modification of immutable reference
- `E3004` — Use of moved value
- `E8301` — Region-allocated value cannot escape scope

**Design philosophy:**

> Cantrip provides memory safety guarantees (no use-after-free, no leaks) through region-based escape analysis and permission tracking. Aliasing safety is managed through permissions and effects, trading compile-time restrictions for runtime flexibility.

###### 2.1.6.6.2 Region Integration

**Definition 2.1.6.5 (Region-Allocated String):** A `String` allocated within a memory region for fast bulk deallocation. Region-allocated strings cannot escape their region scope.

**Philosophy:** Regions control WHEN memory is freed (lexical scope), not WHO can access it. Perfect for temporary string processing.

####### 2.1.6.6.2.1 Region Allocation Syntax

**Basic syntax:**

```cantrip
region name {
    let s: own String = String.new_in<name>()
    // s allocated in region 'name'
}  // s freed here (O(1) bulk deallocation)
```

**Type rule:**

```
[T-String-Region]
Γ ⊢ region r active
─────────────────────────────────────
Γ ⊢ String.new_in<r>() : own String
    uses alloc.region
```

**Complete example:**

```cantrip
function parse_file(path: String): Result<Data, Error>
    uses alloc.region, io.read
{
    region temp {
        // Allocate parsing buffer in region
        let contents: own String = String.new_in<temp>()

        // Read file into region-allocated buffer
        std.fs.read_to_string(path, mut contents)?

        // Parse (may use more temp strings)
        let parsed = parse_json(contents)?

        // Must return heap-allocated data
        Ok(parsed.to_heap())
    }  // All temp strings freed in O(1)
}
```

####### 2.1.6.6.2.2 String Region Methods

**Standard allocation methods:**

| Method                           | Signature               | Allocation  |
| -------------------------------- | ----------------------- | ----------- |
| `String.new()`                   | `() => own String`      | Heap        |
| `String.new_in<'r>()`            | `() => own String`      | Region `'r` |
| `String.from(s)`                 | `(str) => own String`   | Heap        |
| `String.from_in<'r>(s)`          | `(str) => own String`   | Region `'r` |
| `String.with_capacity(n)`        | `(usize) => own String` | Heap        |
| `String.with_capacity_in<'r>(n)` | `(usize) => own String` | Region `'r` |

**Example usage:**

```cantrip
region temp {
    // Empty string in region
    let s1 = String.new_in<temp>()

    // From literal
    let s2 = String.from_in<temp>("hello")

    // With capacity
    let s3 = String.with_capacity_in<temp>(1024)
}
```

####### 2.1.6.6.2.3 Lifetime Enforcement

**Cannot escape region:**

```cantrip
function bad_example(): own String
    uses alloc.region
{
    region temp {
        let s = String.new_in<temp>()
        s.push_str("data")
        s  // ERROR E8301: Cannot return region-allocated value
    }
}
```

**Compiler error:**

```
Error E8301: Value allocated in region 'temp' cannot escape region scope
  --> example.ct:5:9
   |
3  |     region temp {
   |     ----------- region 'temp' declared here
4  |         let s = String.new_in<temp>()
   |                 --------------------- allocated in region 'temp'
5  |         s  // ERROR
   |         ^ cannot return region-allocated value
   |
   = note: Region-allocated values are freed when region ends
   = help: Convert to heap: `s.to_heap()` or `s.clone()`
```

**Correct pattern - convert to heap:**

```cantrip
function safe_example(): own String
    uses alloc.region, alloc.heap
{
    region temp {
        var s = String.new_in<temp>()
        s.push_str("data")
        s.to_heap()  // OK: Deep copy to heap
    }
}
```

**Type rule for escape prevention:**

```
[Region-Escape]
Γ ⊢ region r { e }    e : T    alloc(e, r)    e escapes r
──────────────────────────────────────────────────────────
ERROR E8301: Cannot return region-allocated value
```

####### 2.1.6.6.2.4 Performance Characteristics

**Allocation comparison:**

| Operation      | Heap          | Region     | Speedup   |
| -------------- | ------------- | ---------- | --------- |
| Allocation     | 50-100 cycles | 3-5 cycles | 10-20×    |
| Deallocation   | O(n) frees    | O(1) bulk  | 100-1000× |
| Cache locality | Scattered     | Contiguous | 2-4×      |

**Region allocation algorithm:**

```
1. Check: current_page has space?
2. Yes: bump pointer (3-5 cycles)
3. No: allocate new page, update list
4. Return: pointer to allocation
5. Region end: free entire page list (O(1))
```

**Memory layout:**

```
Region: temp
├── Page 1 (4 KB)    [String1][String2][String3]▒▒▒▒
├── Page 2 (8 KB)    [String4][String5]▒▒▒▒▒▒▒▒▒▒▒▒▒
└── Page 3 (16 KB)   [String6]▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
                      ↑
                      allocation_ptr
```

####### 2.1.6.6.2.5 Region vs Heap Decision

**Use heap allocation (`String.new()`)** when:

- String uses to outlive current function
- String lifetime is unpredictable
- Building long-lived data structures
- String uses to be stored in persistent collections

**Use region allocation (`String.new_in<r>()`)** when:

- String only needed temporarily (within function/phase)
- Batch processing many short-lived strings
- Performance-critical code with many allocations
- Parsing, lexing, or compilation phases

**Decision flowchart:**

```
Does string need to escape function?
├─ Yes → Use heap allocation
└─ No → Is this performance-critical?
    ├─ Yes → Use region allocation
    └─ No → Either (prefer heap for simplicity)
```

####### 2.1.6.6.2.6 Nested Regions

**Regions support nesting (LIFO order):**

```cantrip
function nested_example()
    uses alloc.region
{
    region outer {
        let s1 = String.from_in<outer>("outer")

        region inner {
            let s2 = String.from_in<inner>("inner")

            // Can access outer from inner
            s1.push_str(" modified")

            // Cannot return s2 (would escape inner)
        }  // s2 freed here

        // s1 still valid
        s1.push_str(" again")
    }  // s1 freed here
}
```

**Region hierarchy rules:**

1. Inner regions deallocate before outer (LIFO)
2. Inner regions CAN access outer region values
3. Outer regions CANNOT access inner region values
4. Cannot return inner-region values to outer scopes

**Formal semantics:**

```
[Region-LIFO]
r₂ nested in r₁
──────────────────────────────────────
dealloc(r₂) <ʜᴀᴘᴘᴇɴs-ʙᴇғᴏʀᴇ> dealloc(r₁)

[Region-Nesting]
Γ, r₁ : Region ⊢ region r₂ { e₂ }    parent(r₂) = r₁
──────────────────────────────────────────────────────
Γ ⊢ e₂ can reference values in r₁
```

####### 2.1.6.6.2.7 Common Patterns

**Pattern 1: Parsing with regions**

```cantrip
function tokenize(source: String): Vec<Token>
    uses alloc.heap, alloc.region
{
    var tokens = Vec.new()  // Heap (return value)

    region temp {
        for line in source.lines() {
            // Temporary string for processing
            var trimmed = String.new_in<temp>()
            trimmed.push_str(line.trim())

            let token = parse_token(trimmed)
            tokens.push(token)  // token copied to heap

            // trimmed stays in region (will be freed)
        }
    }  // All temp strings freed (O(1))

    tokens  // Return heap-allocated result
}
```

**Pattern 2: Request processing**

```cantrip
function handle_request(request: Request): Response
    uses alloc.region, alloc.heap
{
    region request_scope {
        // Parse request (many temp strings)
        var body = String.new_in<request_scope>()
        request.read_body(mut body)?

        var headers = parse_headers_in<request_scope>(request)
        let data = extract_data(body, headers)?

        // Return heap-allocated response
        Response.from_data(data)
    }  // All request temps freed instantly
}
```

**Pattern 3: Batch string processing**

```cantrip
function process_batch(items: Vec<String>): Vec<Result>
    uses alloc.region, alloc.heap
{
    var results = Vec.new()

    region batch {
        for item in items {
            // Temporary processing strings
            var normalized = String.new_in<batch>()
            normalized.push_str(item.to_lowercase())

            var cleaned = String.new_in<batch>()
            cleaned.push_str(normalized.trim())

            let result = process(cleaned)
            results.push(result)  // result copied to heap

            // normalized, cleaned stay in region
        }
    }  // O(1) bulk free - perfect for batch jobs

    results
}
```

**Pattern 4: Compilation phases**

```cantrip
function compile(source: String): Program
    uses alloc.region, alloc.heap
{
    region parsing {
        var tokens = lex_into_strings<parsing>(source)

        region analysis {
            let ast = parse_in<analysis>(tokens)
            let optimized = optimize(ast)

            codegen(optimized)  // Return heap-allocated Program
        }  // Analysis temps freed
    }  // Parsing temps freed
}
```

####### 2.1.6.6.2.8 Converting Between Heap and Region

**Methods:**

```cantrip
impl String {
    // Clone from heap to region
    function clone_in<'r>(self: String): own String
        uses alloc.region

    // Convert region → heap (deep copy)
    function to_heap(self: own String): own String
        uses alloc.heap
}
```

**Example:**

```cantrip
function example()
    uses alloc.heap, alloc.region
{
    let heap_str = String.from("data")

    region temp {
        // Clone heap string into region
        let region_str = heap_str.clone_in<temp>()

        // Process in region
        region_str.push_str(" processed")

        // Convert back to heap if needed
        let result = region_str.to_heap()

        // result escapes, region_str freed
    }
}
```

####### 2.1.6.6.2.9 Permissions Still Apply

**Regions control lifetime, not access:**

```cantrip
function example()
    uses alloc.region
{
    region temp {
        var text = String.new_in<temp>()

        // Permissions work as normal:
        read(text)          // Immutable reference
        modify(mut text)    // Mutable reference
        read(text)          // Can reference again

        // Multiple mut refs still allowed
        modify1(mut text)
        modify2(mut text)
    }  // text freed (region determines WHEN)
}
```

**Key insight:**

- **Permissions** control WHO can do WHAT (read/write/move)
- **Regions** control WHEN memory is freed (scope-based)
- These are orthogonal concerns in Cantrip

**Error codes:**

- `E8301` — Region-allocated value cannot escape region scope
- `E8302` — Region reference lifetime violation
- `E8303` — Invalid region nesting

###### 2.1.6.6.3 String Operations

String provides a comprehensive API for construction, access, modification, and manipulation of UTF-8 text. Operations are organized by category and permission requirements.

####### 2.1.6.6.3.1 Construction Operations

**Empty string creation:**

```cantrip
function String.new(): own String
    uses alloc.heap
```

Creates an empty String with zero length. May allocate minimal capacity.

**Example:**

```cantrip
function create_buffer(): own String
    uses alloc.heap
{
    String.new()  // Returns empty String
}
```

**From UTF-8 bytes (validated):**

```cantrip
function String.from_utf8(bytes: [u8]): Result<own String, Utf8Error>
    uses alloc.heap
```

Constructs a String from a byte array, validating UTF-8 encoding. Returns error if bytes are invalid UTF-8.

**Example:**

```cantrip
function parse_bytes(data: [u8]): Result<own String, Utf8Error>
    uses alloc.heap
{
    String.from_utf8(data)  // Validates UTF-8
}

let valid_bytes: [u8] = [72, 101, 108, 108, 111]  // "Hello"
let s = String.from_utf8(valid_bytes)?  // Ok(String)

let invalid_bytes: [u8] = [0xFF, 0xFE]  // Invalid UTF-8
let err = String.from_utf8(invalid_bytes)  // Err(Utf8Error)
```

**From UTF-8 bytes (unchecked):**

```cantrip
function String.from_utf8_unchecked(bytes: [u8]): own String
    uses alloc.heap
```

Constructs a String from bytes without validation. **Unsafe**: Violates UTF-8 invariant if bytes are invalid.

**Example:**

```cantrip
// ONLY use if you KNOW bytes are valid UTF-8
function trusted_conversion(validated_bytes: [u8]): own String
    uses alloc.heap
{
    String.from_utf8_unchecked(validated_bytes)  // No validation overhead
}
```

**Warning:** Using `from_utf8_unchecked` with invalid UTF-8 creates undefined behavior. Use `from_utf8` unless performance is critical and data is pre-validated.

**From string slice:**

```cantrip
function String.from(slice: str): own String
    uses alloc.heap
```

Constructs a String by copying data from a string slice. Allocation required for heap storage.

**Example:**

```cantrip
function duplicate_literal(): own String
    uses alloc.heap
{
    let literal: str = "Hello, world!"
    String.from(literal)  // Allocates and copies
}
```

**With initial capacity:**

```cantrip
function String.with_capacity(cap: usize): own String
    uses alloc.heap
```

Creates an empty String with pre-allocated capacity. Useful for avoiding reallocations when final size is known.

**Example:**

```cantrip
function build_large_string(parts: [str]): own String
    uses alloc.heap
{
    let total_len: usize = parts.map(|p| p.len()).sum()
    let mut result: own String = String.with_capacity(total_len)

    for part in parts {
        result.push_str(part)  // No reallocation needed
    }

    result
}
```

**Region-allocated string:**

```cantrip
function String.new_in<'r>(): own String
    uses alloc.region
```

Creates an empty String allocated in the specified region instead of the heap. String lifetime is bound to the region.

**Example:**

```cantrip
function temporary_strings(): own String
    uses alloc.heap, alloc.region
{
    region temp {
        let s: own String = String.new_in<temp>()
        s.push_str("temporary")

        // Cannot return s (region-bound)
        // Must convert to heap:
        String.from(s.as_str())  // Heap copy
    }  // s deallocated here
}
```

####### 2.1.6.6.3.2 Access Operations (Immutable)

**Get byte length:**

```cantrip
function len(self: String): usize
```

Returns the length of the String in bytes (NOT characters). O(1) operation.

**Example:**

```cantrip
let s: own String = String.from("Hello🦀")
assert!(s.len() == 10)  // 5 ASCII bytes + 4 byte emoji
```

**Check if empty:**

```cantrip
function is_empty(self: String): bool
```

Returns `true` if the String contains zero bytes. Equivalent to `self.len() == 0`.

**Example:**

```cantrip
let empty: own String = String.new()
assert!(empty.is_empty())

let non_empty: own String = String.from("x")
assert!(!non_empty.is_empty())
```

**Get capacity:**

```cantrip
function capacity(self: String): usize
```

Returns the allocated capacity in bytes. Capacity is always greater than or equal to length.

**Example:**

```cantrip
let s: own String = String.with_capacity(100)
assert!(s.capacity() >= 100)
assert!(s.len() == 0)
```

**Convert to str slice:**

```cantrip
function as_str(self: String): str
```

Returns a string slice view of the String's contents. Zero-copy operation.

**Example:**

```cantrip
function print_text(text: str) {
    println(text)
}

function example()
    uses alloc.heap
{
    let s: own String = String.from("data")
    print_text(s.as_str())  // Explicit conversion
    print_text(s)  // Or use deref coercion
}
```

**Get bytes:**

```cantrip
function as_bytes(self: String): [u8]
```

Returns a byte slice view of the String's UTF-8 data. Zero-copy operation.

**Example:**

```cantrip
let s: own String = String.from("ABC")
let bytes: [u8] = s.as_bytes()
assert!(bytes[0] == 65)  // 'A'
assert!(bytes[1] == 66)  // 'B'
assert!(bytes[2] == 67)  // 'C'
```

####### 2.1.6.6.3.3 Modification Operations (Requires own or mut)

**Append string:**

```cantrip
function push_str(self: mut String, s: str)
    uses alloc.heap
```

Appends a string slice to the end of the String. May reallocate if capacity is insufficient.

**Example:**

```cantrip
function concatenate(a: str, b: str): own String
    uses alloc.heap
{
    let mut result: own String = String.from(a)
    result.push_str(b)
    result
}
```

**Append character:**

```cantrip
function push(self: mut String, ch: char)
    uses alloc.heap
```

Appends a Unicode scalar value to the String. Character is encoded as UTF-8 (1-4 bytes).

**Example:**

```cantrip
let mut s: own String = String.from("Hello")
s.push(' ')
s.push('🦀')
assert!(s.as_str() == "Hello 🦀")
```

**Insert string at position:**

```cantrip
function insert_str(self: mut String, idx: usize, s: str)
    uses alloc.heap
    must idx <= self.len()
    must is_char_boundary(self, idx)
```

Inserts a string slice at the specified byte index. May reallocate. Index must be at a UTF-8 character boundary.

**Example:**

```cantrip
let mut s: own String = String.from("HelloWorld")
s.insert_str(5, ", ")  // Index 5 is after "Hello"
assert!(s.as_str() == "Hello, World")

// ERROR: Invalid boundary
// s.insert_str(1, "x")  // Panic if idx=1 is mid-character
```

**Remove range:**

```cantrip
function remove(self: mut String, start: usize, end: usize)
    must start <= end
    must end <= self.len()
    must is_char_boundary(self, start)
    must is_char_boundary(self, end)
```

Removes characters in the byte range [start, end). Indices must be at UTF-8 character boundaries.

**Example:**

```cantrip
let mut s: own String = String.from("Hello, World!")
s.remove(5, 7)  // Remove ", "
assert!(s.as_str() == "HelloWorld!")
```

**Clear all contents:**

```cantrip
function clear(self: mut String)
```

Removes all characters from the String, setting length to 0. Capacity is unchanged.

**Example:**

```cantrip
let mut s: own String = String.from("temporary")
let old_cap: usize = s.capacity()

s.clear()
assert!(s.is_empty())
assert!(s.capacity() == old_cap)  // Capacity preserved
```

**Truncate to length:**

```cantrip
function truncate(self: mut String, new_len: usize)
    must new_len <= self.len()
    must is_char_boundary(self, new_len)
```

Shortens the String to the specified byte length. `new_len` must be at a character boundary.

**Example:**

```cantrip
let mut s: own String = String.from("Hello, World!")
s.truncate(5)  // Keep only "Hello"
assert!(s.as_str() == "Hello")
```

**Reserve additional capacity:**

```cantrip
function reserve(self: mut String, additional: usize)
    uses alloc.heap
```

Reserves capacity for at least `additional` more bytes. May reallocate if current capacity is insufficient.

**Example:**

```cantrip
function build_string(parts: [str]): own String
    uses alloc.heap
{
    let mut s: own String = String.new()
    let total: usize = parts.map(|p| p.len()).sum()

    s.reserve(total)  // Pre-allocate

    for part in parts {
        s.push_str(part)  // No reallocation
    }

    s
}
```

####### 2.1.6.6.3.4 View Operations (Zero-Copy)

**Slice to str:**

```cantrip
function slice(self: String, start: usize, end: usize): str
    must start <= end
    must end <= self.len()
    must is_char_boundary(self, start)
    must is_char_boundary(self, end)
```

Returns a string slice view of the byte range [start, end). Zero-copy operation. Indices must be at UTF-8 character boundaries.

**Example:**

```cantrip
let s: own String = String.from("Hello, World!")
let hello: str = s.slice(0, 5)  // "Hello"
let world: str = s.slice(7, 12) // "World"
```

**Split by delimiter:**

```cantrip
function split(self: String, delimiter: str): Iterator<str>
    must !delimiter.is_empty()
```

Returns an iterator over substrings separated by the delimiter. Delimiter is not included in results.

**Example:**

```cantrip
let s: own String = String.from("one,two,three")
let parts: [str] = s.split(",").collect()
assert!(parts.len() == 3)
assert!(parts[0] == "one")
assert!(parts[1] == "two")
assert!(parts[2] == "three")
```

**Get lines:**

```cantrip
function lines(self: String): Iterator<str>
```

Returns an iterator over lines (substrings separated by `\n` or `\r\n`). Line terminators are not included in results.

**Example:**

```cantrip
let doc: own String = String.from("Line 1\nLine 2\nLine 3")
for line in doc.lines() {
    println(line)
}
// Output:
// Line 1
// Line 2
// Line 3
```

**Trim whitespace:**

```cantrip
function trim(self: String): str
```

Returns a string slice with leading and trailing whitespace removed. Zero-copy operation.

**Example:**

```cantrip
let s: own String = String.from("  hello  ")
let trimmed: str = s.trim()
assert!(trimmed == "hello")
```

####### 2.1.6.6.3.5 Iteration Operations

**Iterate bytes:**

```cantrip
function bytes(self: String): Iterator<u8>
```

Returns an iterator over the String's bytes. Each byte is yielded individually.

**Example:**

```cantrip
let s: own String = String.from("ABC")
for byte in s.bytes() {
    println("{byte}")  // 65, 66, 67
}
```

**Iterate characters:**

```cantrip
function chars(self: String): Iterator<char>
```

Returns an iterator over Unicode scalar values (characters). Multi-byte UTF-8 sequences are decoded.

**Example:**

```cantrip
let s: own String = String.from("Hello🦀")
for ch in s.chars() {
    println("{ch}")  // H, e, l, l, o, 🦀
}
```

####### 2.1.6.6.3.6 Comparison Operations

**Equality (bytes):**

```cantrip
function equals(self: String, other: String): bool
```

Returns `true` if both Strings contain identical byte sequences. Equivalent to `==` operator.

**Example:**

```cantrip
let a: own String = String.from("hello")
let b: own String = String.from("hello")
let c: own String = String.from("world")

assert!(a.equals(b))
assert!(!a.equals(c))
```

**Lexicographic comparison:**

```cantrip
function compare(self: String, other: String): Ordering
```

Compares Strings lexicographically by byte values. Returns `Less`, `Equal`, or `Greater`.

**Example:**

```cantrip
let a: own String = String.from("apple")
let b: own String = String.from("banana")

match a.compare(b) {
    Ordering::Less => println("apple < banana"),
    Ordering::Equal => println("equal"),
    Ordering::Greater => println("apple > banana"),
}
```

**Starts with prefix:**

```cantrip
function starts_with(self: String, prefix: str): bool
```

Returns `true` if the String begins with the specified prefix.

**Example:**

```cantrip
let s: own String = String.from("https://example.com")
assert!(s.starts_with("https://"))
assert!(!s.starts_with("http://"))
```

**Ends with suffix:**

```cantrip
function ends_with(self: String, suffix: str): bool
```

Returns `true` if the String ends with the specified suffix.

**Example:**

```cantrip
let filename: own String = String.from("document.txt")
assert!(filename.ends_with(".txt"))
assert!(!filename.ends_with(".pdf"))
```

###### 2.1.6.6.4 Indexing and Iteration

####### 2.1.6.6.4.1 Why No Direct Indexing

String does NOT support direct indexing via `s[i]` syntax. This is a deliberate design decision based on UTF-8's variable-width encoding.

**Problem with character indexing:**

UTF-8 encodes Unicode scalar values using 1-4 bytes per character. Direct indexing creates ambiguity:

```cantrip
let s: own String = String.from("Hello🦀World")  // Contains emoji (4 bytes)

// If s[6] were allowed, what would it return?
// Option 1: Byte at index 6? (middle of emoji - INVALID UTF-8!)
// Option 2: 6th character? (requires O(n) scan from start)

// ERROR: s[6] is not allowed
// let ch = s[6]  // Compile error: String does not support indexing
```

**Why byte indexing is unsafe:**

```cantrip
// UTF-8 encoding of "Hello🦀":
// H     e     l     l     o     🦀              (characters)
// 0x48  0x65  0x6C  0x6C  0x6F  0xF0 0x9F 0xA6 0x80  (bytes)
// 0     1     2     3     4     5    6    7    8     (indices)

// Accessing index 6 would return 0x9F (middle of 🦀)
// This is NOT a valid UTF-8 sequence!
```

**Why character indexing is avoided:**

Users typically expect `[i]` to be O(1) constant-time. However, UTF-8 character access must scanning from the start:

```cantrip
// To find the nth character, must scan from beginning
function nth_char_naive(s: String, n: usize): Option<char> {
    let mut count: usize = 0
    for ch in s.chars() {
        if count == n {
            return Some(ch)
        }
        count = count + 1
    }
    None
}
// O(n) operation!
```

**Rationale:**

By prohibiting `s[i]`, Cantrip makes performance characteristics explicit:

- Byte iteration: `s.bytes()` - clearly O(1) per byte
- Character iteration: `s.chars()` - clearly must decoding
- Slicing: `s.slice(start, end)` - must explicit boundary indices

####### 2.1.6.6.4.2 Byte Iteration

For byte-level access (protocols, parsing, low-level operations):

```cantrip
function process_bytes(s: String) {
    for byte in s.bytes() {
        // byte has type u8
        if byte == 0x0A {  // Newline
            println("Found newline")
        }
    }
}
```

**Performance:** O(1) per byte (direct array access).

**Use cases:**

- Binary protocol parsing
- ASCII-only processing
- Byte-level validation
- Low-level file I/O

####### 2.1.6.6.4.3 Character Iteration

For Unicode scalar value access:

```cantrip
function count_chars(s: String): usize {
    let mut count: usize = 0
    for ch in s.chars() {
        // ch has type char (Unicode scalar value)
        count = count + 1
    }
    count
}

let s: own String = String.from("Hello🦀")
assert!(count_chars(s) == 6)  // 5 ASCII + 1 emoji
```

**Performance:** O(n) to traverse entire string. Getting the nth character must O(n) scan.

**Example: Finding nth character:**

```cantrip
function nth_char(s: String, n: usize): Option<char> {
    let mut iter = s.chars()
    iter.skip(n).next()  // O(n) operation
}

let s: own String = String.from("Hello")
assert!(nth_char(s, 0) == Some('H'))
assert!(nth_char(s, 4) == Some('o'))
assert!(nth_char(s, 10) == None)
```

####### 2.1.6.6.4.4 Grapheme Clusters

**Important caveat:** `.chars()` returns Unicode scalar values (code points), NOT grapheme clusters (user-perceived characters).

```cantrip
// Example: é can be represented two ways
let composed: str = "é"     // U+00E9 (single code point)
let decomposed: str = "é"   // U+0065 U+0301 (e + combining accent)

let s1: own String = String.from(composed)
let s2: own String = String.from(decomposed)

// Character counts differ!
assert!(s1.chars().count() == 1)  // 1 code point
assert!(s2.chars().count() == 2)  // 2 code points (e + accent)

// But they look identical when displayed:
println("{}", s1)  // é
println("{}", s2)  // é
```

**For true grapheme iteration:**

Use the Unicode segmentation library (not in core):

```cantrip
// Hypothetical library usage:
// use std.unicode.graphemes
//
// for grapheme in s.graphemes() {
//     // grapheme is a user-perceived character
// }
```

####### 2.1.6.6.4.5 Safe Slicing

Slicing must respect UTF-8 character boundaries to preserve the UTF-8 invariant:

```cantrip
let s: own String = String.from("Hello🦀World")

// OK: Slices at character boundaries
let hello: str = s.slice(0, 5)  // "Hello"
let world: str = s.slice(9, 14) // "World"

// PANIC: Index 7 is mid-character (inside 🦀 emoji)
// let bad: str = s.slice(0, 7)  // Runtime panic!
// Error: "byte index 7 is not a char boundary"
```

**Character boundary rules:**

A byte index is a valid character boundary if:

1. It equals 0 or `s.len()`, OR
2. The byte at that index is NOT a UTF-8 continuation byte (0x80-0xBF)

```cantrip
// UTF-8 continuation bytes: 10xxxxxx (0x80-0xBF)
function is_char_boundary(s: String, idx: usize): bool {
    if idx > s.len() {
        return false
    }
    if idx == 0 || idx == s.len() {
        return true
    }

    let byte: u8 = s.as_bytes()[idx]
    (byte & 0xC0) != 0x80  // NOT a continuation byte
}
```

**Safe slicing pattern:**

When you need to slice at a specific character position:

```cantrip
function slice_first_n_chars(s: String, n: usize): str {
    let mut char_count: usize = 0
    let mut byte_idx: usize = 0

    for (idx, ch) in s.char_indices() {
        if char_count >= n {
            break
        }
        byte_idx = idx + ch.len_utf8()
        char_count = char_count + 1
    }

    s.slice(0, byte_idx)
}

let s: own String = String.from("Hello🦀World")
let first5: str = slice_first_n_chars(s, 5)
assert!(first5 == "Hello")
```

###### 2.1.6.6.5 Common Pitfalls

**Pitfall 1: UTF-8 Boundary Violation**

```cantrip
// WRONG: Slicing mid-character
function bad_slice()
    uses alloc.heap
{
    let s: own String = String.from("Hello🦀")

    // 🦀 is 4 bytes: 0xF0 0x9F 0xA6 0x80
    // Byte indices: H=0, e=1, l=2, l=3, o=4, 🦀=5-8

    let bad: str = s.slice(0, 7)  // PANIC at runtime!
    // Error: "byte index 7 is not a char boundary"
}

// CORRECT: Use character iteration or valid boundaries
function good_slice()
    uses alloc.heap
{
    let s: own String = String.from("Hello🦀")

    // Option 1: Slice at known character boundaries
    let hello: str = s.slice(0, 5)  // "Hello"

    // Option 2: Use character iteration
    let first5: own String = String.new()
    for (idx, ch) in s.chars().enumerate() {
        if idx < 5 {
            first5.push(ch)
        }
    }
}
```

**Explanation:** UTF-8 multi-byte characters cannot be split. Index 7 falls inside the 🦀 emoji (4 bytes at indices 5-8), causing a panic. Always slice at character boundaries or use character-based iteration.

**Pitfall 2: Multiple Mutable References (Use-After-Realloc)**

```cantrip
// DANGEROUS: Reference invalidation
function dangerous(s: mut String)
    uses alloc.heap
{
    let bytes: [u8] = s.as_bytes()  // Get reference to byte data
    let first: u8 = bytes[0]        // Read from reference

    s.push_str("x")  // Modifies String, MAY reallocate!

    // bytes pointer is now INVALID if reallocation occurred!
    // Accessing bytes[1] could read freed memory (undefined behavior)
    // let second: u8 = bytes[1]  // USE-AFTER-FREE!
}

// SAFE: Don't hold references across mutations
function safe(s: mut String)
    uses alloc.heap
{
    // Copy values, not references
    let len: usize = s.len()
    let first: u8 = s.as_bytes()[0]

    s.push_str("x")  // OK: no outstanding references

    // Re-acquire reference after mutation
    let new_len: usize = s.len()
    assert!(new_len == len + 1)
}
```

**Explanation:** Unlike Rust, Cantrip allows multiple mutable references and doesn't prevent invalidation. Programmer must ensure references aren't held across reallocating operations. Copy values instead of holding references.

**Pitfall 3: Assuming Character Count = Byte Count**

```cantrip
// WRONG: Confusing bytes and characters
function bad_count()
    uses alloc.heap
{
    let s: own String = String.from("Hello🦀")
    let char_count: usize = s.len()  // Returns 10 (bytes), not 6!
    println("Characters: {char_count}")  // Wrong: prints 10
}

// CORRECT: Use chars().count()
function good_count()
    uses alloc.heap
{
    let s: own String = String.from("Hello🦀")

    let byte_count: usize = s.len()           // 10 bytes
    let char_count: usize = s.chars().count() // 6 characters

    println("Bytes: {byte_count}")      // 10
    println("Characters: {char_count}") // 6
}
```

**Explanation:** `len()` returns byte length, not character count. Use `chars().count()` for character count, but note this is O(n) because it must decode the entire UTF-8 string.

**Pitfall 4: Region Escape Attempt**

```cantrip
// WRONG: Trying to return region-allocated string
function bad_region(): own String
    uses alloc.region
{
    region temp {
        let s: own String = String.new_in<temp>()
        s.push_str("data")
        s  // ERROR: cannot escape region (lifetime violation)
    }
}

// CORRECT: Convert to heap before returning
function good_region(): own String
    uses alloc.heap, alloc.region
{
    region temp {
        let s: own String = String.new_in<temp>()
        s.push_str("data")

        // Copy to heap before escaping region
        String.from(s.as_str())  // Heap allocation
    }  // s freed here (region ends)
}
```

**Explanation:** Region-allocated Strings cannot escape their region's scope. To return a region-built string, copy it to the heap using `String.from()` before the region ends.

**Pitfall 5: Grapheme vs. Character Count**

```cantrip
// WRONG: Assuming chars() gives grapheme count
function bad_grapheme()
    uses alloc.heap
{
    // "é" can be U+00E9 (composed) or U+0065 U+0301 (decomposed)
    let s1: own String = String.from("é")  // Composed (1 code point)
    let s2: own String = String.from("é")  // Decomposed (2 code points)

    // Character counts differ!
    assert!(s1.chars().count() == 1)  // Passes
    assert!(s2.chars().count() == 1)  // FAILS! (actually 2)
}

// CORRECT: Be aware of normalization
function good_grapheme()
    uses alloc.heap
{
    let s: own String = String.from("é")  // Could be 1 or 2 code points

    // For user-perceived character count, need Unicode library:
    // use std.unicode.graphemes
    // let grapheme_count = s.graphemes().count()

    // Or normalize before counting:
    // let normalized = s.nfc()  // NFC normalization
    // let char_count = normalized.chars().count()
}
```

**Explanation:** Unicode allows multiple representations of the same visual character. `chars()` counts code points, not graphemes (user-perceived characters). For accurate user-visible character counts, use Unicode segmentation libraries.

---

## 2.1.7 Built-In Sum Types

Cantrip provides two fundamental sum types that are so pervasive they are considered built-in primitives despite being implemented as enums. These types provide the foundation for Cantrip's error handling and null safety.

**Definition 2.1.7.1 (Built-In Sum Types):** The types `Option<T>` and `Result<T, E>` are built-in generic sum types that provide type-safe alternatives to null pointers and exception-based error handling. While structurally implemented as enums, they receive special treatment in the type system and integrate deeply with Cantrip's contract and effect systems.

### 2.1.7.1 Option<T>

#### Overview

**Key innovation/purpose:** `Option<T>` provides type-safe optional values, eliminating null pointer errors by making the absence of a value explicit in the type system.

**When to use Option<T>:**

- Values that may or may not be present
- Function returns that may have no meaningful result
- Nullable fields in data structures
- Search operations that may fail to find a result
- Configuration values with defaults

**When NOT to use Option<T>:**

- Error conditions with diagnostic information → use `Result<T, E>`
- Boolean flags → use `bool` directly
- Collections that may be empty → use empty collection
- Values that are always present → use `T` directly

**Relationship to other features:**

- **Pattern matching**: Option must be matched exhaustively
- **Pointer types**: `Option<Ptr<T>>` uses niche optimization (null = None)
- **Permissions**: Option wraps permissions: `Option<own T>`, `Option<mut T>`
- **Contracts**: Postconditions can specify Option return conditions

#### Syntax

**Type definition (conceptual - actually built-in):**

```cantrip
enum Option<T> {
    Some(T),
    None,
}
```

**Construction:**

```cantrip
// Create Some variant
let some_value: Option<i32> = Option.Some(42)

// Create None variant
let no_value: Option<i32> = Option.None
```

**Pattern matching:**

```cantrip
match some_value {
    Option.Some(x) => println("Got value: {x}"),
    Option.None => println("No value"),
}
```

#### Static Semantics

**Type rules:**

```
[T-Option-Some]
Γ ⊢ e : T
──────────────────────────
Γ ⊢ Option.Some(e) : Option<T>

[T-Option-None]
──────────────────────────
Γ ⊢ Option.None : Option<T>
    (T inferred from context)
```

**Pattern matching rule:**

```
[T-Option-Match]
Γ ⊢ e : Option<T>
Γ, x : T ⊢ e_some : U
Γ ⊢ e_none : U
──────────────────────────────────────────────
Γ ⊢ match e {
      Option.Some(x) => e_some,
      Option.None => e_none
    } : U
```

#### Memory Representation

**Standard layout:**

```
Option<T>:
┌──────────────┬──────────────────────┐
│ Discriminant │    Payload (T)       │
│  (1-8 bytes) │  (sizeof(T) bytes)   │
└──────────────┴──────────────────────┘

Size: max(sizeof(T) + discriminant_size, pointer_size)
Alignment: max(alignof(T), alignof(discriminant))
```

**Niche optimization (for non-nullable pointers):**

```
Option<Ptr<T>@Exclusive>:
┌──────────────┐
│   Pointer    │  Some = valid pointer, None = null (0x0)
│   8 bytes    │  No discriminant needed!
└──────────────┘

Size: 8 bytes (same as Ptr<T>)
Alignment: 8 bytes
```

**Theorem 2.1.7.1 (Niche Optimization):** For types T with invalid bit patterns (e.g., non-nullable pointers), `sizeof(Option<T>) = sizeof(T)`. The compiler uses the invalid pattern to represent `None`.

#### Working with Option Values

**Primary mechanism:** Pattern matching (exhaustive checking enforced by compiler)

```cantrip
// Extract value with pattern matching
match optional_value {
    Option.Some(x) => {
        // Use x here
        process(x)
    },
    Option.None => {
        // Handle absence
        use_default()
    },
}

// Conditional binding
if let Option.Some(x) = optional_value {
    // x is bound here
    use_value(x)
}
```

**Note:** Standard library (Part XII) provides utility procedures for common operations (checking presence, extracting with defaults, etc.), but pattern matching is the idiomatic Cantrip approach for Option handling.


### 2.1.7.2 Result<T, E>

#### Overview

**Key innovation/purpose:** `Result<T, E>` provides type-safe error handling without exceptions, making errors explicit in function signatures and enabling exhaustive error checking through pattern matching.

**When to use Result<T, E>:**

- Operations that can fail with error information
- I/O operations (file, network)
- Parsing and validation
- Resource allocation that may fail
- Any fallible computation needing error details

**When NOT to use Result<T, E>:**

- Logic errors that should never happen → use `must` preconditions or `panic`
- Simple presence/absence → use `Option<T>`
- Control flow without errors → use enums
- Errors that should terminate → use `panic` or `exit`

**Relationship to other features:**

- **Contract system**: Result integrates with effects and postconditions
- **Pattern matching**: Result must be matched exhaustively
- **Error propagation**: Natural composition through match or helper methods
- **Type aliases**: Common to alias `Result<T, E>` with fixed error type

#### Syntax

**Type definition (conceptual - actually built-in):**

```cantrip
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

**Construction:**

```cantrip
// Success case
let success: Result<i32, String> = Result.Ok(42)

// Error case
let failure: Result<i32, String> = Result.Err("computation failed")
```

**Pattern matching:**

```cantrip
match success {
    Result.Ok(value) => println("Success: {value}"),
    Result.Err(error) => println("Error: {error}"),
}
```

#### Static Semantics

**Type rules:**

```
[T-Result-Ok]
Γ ⊢ e : T
──────────────────────────
Γ ⊢ Result.Ok(e) : Result<T, E>
    (E inferred from context)

[T-Result-Err]
Γ ⊢ e : E
──────────────────────────
Γ ⊢ Result.Err(e) : Result<T, E>
    (T inferred from context)
```

**Pattern matching rule:**

```
[T-Result-Match]
Γ ⊢ e : Result<T, E>
Γ, x : T ⊢ e_ok : U
Γ, err : E ⊢ e_err : U
──────────────────────────────────────────────
Γ ⊢ match e {
      Result.Ok(x) => e_ok,
      Result.Err(err) => e_err
    } : U
```

**Effect integration:**

```
[Effect-Result]
procedure p(...): Result<T, E> uses effects
──────────────────────────────────────────────
Effects include all potential error-producing effects
```

#### Memory Representation

**Standard layout:**

```
Result<T, E>:
┌──────────────┬──────────────────────────────────┐
│ Discriminant │  Payload (max(T, E))             │
│  (1-8 bytes) │  (max(sizeof(T), sizeof(E)))     │
└──────────────┴──────────────────────────────────┘

Size: discriminant_size + max(sizeof(T), sizeof(E))
Alignment: max(alignof(T), alignof(E), alignof(discriminant))
```

**Example sizes:**

```
Result<i32, String>:
  - Discriminant: 1 byte
  - i32: 4 bytes
  - String: 24 bytes (ptr + len + cap)
  - Total: ~32 bytes (with padding)

Result<(), ErrorCode>:  // Common pattern for operations without meaningful return
  - Discriminant: 1 byte
  - (): 0 bytes
  - ErrorCode: depends on error type
```

#### Working with Result Values

**Primary mechanism:** Pattern matching for explicit error handling

```cantrip
// Handle both success and error cases
match operation_result {
    Result.Ok(value) => {
        // Use successful value
        continue_with(value)
    },
    Result.Err(error) => {
        // Handle error
        log_error(error)
        recover()
    },
}

// Propagate errors through explicit matching
procedure process_file(path: String): Result<Data, Error>
    uses io.read, alloc.heap
{
    match std.fs.read_to_string(path) {
        Result.Ok(contents) => {
            match parse_data(contents) {
                Result.Ok(data) => Result.Ok(data),
                Result.Err(e) => Result.Err(e),
            }
        },
        Result.Err(e) => Result.Err(e),
    }
}
```

**Note:** Standard library (Part XII) may provide utility procedures for result composition, but explicit pattern matching is the idiomatic Cantrip approach for error handling.

### 2.1.7.3 Design Rationale

**Why are Option and Result primitives?**

1. **Pervasive use**: Every Cantrip program uses these types extensively
2. **Compiler integration**: Niche optimization, pattern exhaustiveness checking
3. **Effect system**: Result integrates with Cantrip's effect tracking
4. **Error handling foundation**: Result is Cantrip's primary error mechanism
5. **Null safety**: Option eliminates entire class of null pointer bugs

**Why not exceptions?**

Cantrip uses Result<T, E> instead of exceptions because:

- **Explicit in types**: Function signatures show they can fail
- **Exhaustive handling**: Compiler enforces error checking
- **Composable**: Easy to chain fallible operations
- **Zero overhead**: No exception unwinding machinery
- **Permission-safe**: Works naturally with permission system

**Why not built-in `?` operator?**

Cantrip doesn't have Rust's `?` operator because:

- **Explicit over implicit**: Pattern matching makes control flow visible
- **Simpler language**: One less special case operator
- **Methods suffice**: `and_then` and `map` provide composition
- **Clarity**: Error propagation is explicit at each step

However, library or macro support for error propagation patterns is expected.

### 2.1.7.4 Comparison with Other Approaches

**Cantrip vs Rust:**

- **Same**: Option and Result as primary mechanisms
- **Different**: No `?` operator (yet), explicit pattern matching
- **Different**: Permission system with region-based escape analysis affects ownership transfer

**Cantrip vs Java/C++:**

- **Cantrip**: Type-safe Option/Result with exhaustiveness checking
- **Java/C++**: Exceptions with no compile-time checking, nullable types

**Cantrip vs Go:**

- **Cantrip**: Result<T, E> with type-safe errors
- **Go**: (T, error) tuple with convention-based checking

**Cantrip vs Haskell:**

- **Similar**: Maybe/Either monads map to Option/Result
- **Different**: Cantrip uses explicit match, Haskell uses `do` notation

---

## 2.1.8 Related Sections

- See §2.2 for [Product Types](../PART_B_Composite/02_ProductTypes.md) - combining primitives into tuples and records
- See §2.3 for [Sum Types](../PART_B_Composite/03_SumTypes.md) - Option<T> and Result<T, E> as sum types
- See §2.4 for [Collection Types](../PART_B_Composite/04_CollectionTypes.md) - arrays and slices of primitive types
- See §2.5 for [Pointers](../PART_C_References/05_Pointers.md) - references to primitive values
- See §2.7 for [Generics](../PART_D_Abstraction/07_Generics.md) - generic types parameterized over primitives
- See Part III (Permissions) for permission semantics with primitive types
- See Part IV (Regions) for region-based allocation of String and heap types
- See Part V (Effects) for effect tracking with heap-allocating types (String)
- **Examples**: See [01_PrimitiveExamples.md](../Examples/01_PrimitiveExamples.md) for practical usage patterns

## 2.1.9 Notes and Warnings

**Note 1 (Integer Overflow):** Integer overflow wraps in release mode (two's complement) but panics in debug mode. Use checked arithmetic methods (`checked_add`, `checked_mul`, etc.) for overflow detection in release builds.

**Note 2 (Default Integer Type):** Unsuffixed integer literals default to `i32`. Use type suffixes (`42u64`) or type annotations (`: u16`) for other integer types.

**Note 3 (Platform-Dependent Sizes):** Types `isize` and `usize` are pointer-sized (32-bit on 32-bit platforms, 64-bit on 64-bit platforms). Use these for array indexing and memory offsets.

**Note 4 (Floating-Point Precision):** `f32` provides ~7 decimal digits of precision; `f64` provides ~15 decimal digits. Use `f64` by default unless memory/performance constraints require `f32`.

**Note 5 (NaN Behavior):** Floating-point NaN values do not compare equal to themselves (`NaN != NaN`). Use `.is_nan()` method to check for NaN. NaN propagates through most arithmetic operations.

**Note 6 (Character vs String):** `char` represents a single Unicode scalar value (4 bytes), while `str` represents UTF-8 encoded text (variable bytes per character). A grapheme cluster (user-perceived character) may require multiple `char` values.

**Note 7 (String Heap Allocation):** `String` type requires heap allocation (effect `alloc.heap`). Use `str` slices when possible to avoid allocations. String literals have type `str` (borrowed string slice).

**Note 8 (UTF-8 Invariant):** All `String` and `str` values maintain UTF-8 validity as a type invariant. Invalid UTF-8 cannot be constructed through safe operations. Unsafe operations that violate this invariant cause undefined behavior.

**Note 9 (Never Type Divergence):** Functions returning `Never` (`!`) never return normally—they must loop forever, panic, or exit. The `Never` type can coerce to any other type, enabling uniform error handling in match arms.

**Warning 1 (Integer Division by Zero):** Division by zero panics at runtime for integer types. Use checked division (`checked_div`) or explicit zero checks for fallible division.

**Warning 2 (Floating-Point Equality):** Avoid direct equality comparisons with floating-point types due to rounding errors. Use epsilon-based comparisons or relative tolerance checks.

**Warning 3 (String Indexing):** Strings cannot be indexed by position (`s[i]` is invalid) because UTF-8 encoding has variable-width characters. Use `.chars()` iterator or byte slicing (unsafe for invalid boundaries).

**Warning 4 (Option/Result Unwrapping):** Methods like `Option.unwrap()` and `Result.unwrap()` panic if the value is `None` or `Err`. Use pattern matching or fallible unwrapping methods (`unwrap_or`, `unwrap_or_else`) for safer handling.

**Performance Note 1 (Copy Trait):** All primitive types except `String` implement the `Copy` trait and have trivial copy semantics. Copying is automatic when passing by value.

**Performance Note 2 (String Cloning):** Cloning `String` requires heap allocation and memory copying. Prefer passing `str` slices by reference when ownership transfer is not needed.

**Implementation Note 1 (Niche Optimization):** `Option<T>` uses niche optimization for types with unused bit patterns (e.g., `Option<&T>` has same size as `&T` using null for `None`). Similarly optimized for `Option<bool>`, non-zero integers, etc.

**Implementation Note 2 (Result Size):** `Result<T, E>` has size `max(sizeof(T), sizeof(E)) + discriminant`. Use small error types or boxed errors (`Result<T, Box<E>>`) to reduce size overhead.

---

**Previous**: [Type System Introduction](../00_Introduction.md) | **Next**: [Product Types](../PART_B_Composite/02_ProductTypes.md)
