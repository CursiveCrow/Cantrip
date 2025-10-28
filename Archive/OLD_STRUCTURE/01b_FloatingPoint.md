# Part II: Type System - §5.2 Floating-Point Types

**Section**: §5.2 | **Part**: Type System (Part II)
**Previous**: [Integer Types](01a_Integers.md) | **Next**: [Boolean Type](01c_Boolean.md)

---

**Definition 5.2.1 (Floating-Point Types):** Floating-point types represent real numbers using IEEE 754 binary floating-point arithmetic. Cantrip provides two floating-point types: `f32` (single-precision) and `f64` (double-precision).

## 5.2 Floating-Point Types

### 5.2.1 Overview

**Key innovation/purpose:** IEEE 754-compliant floating-point provides standardized, hardware-accelerated real number approximations with well-defined behavior for special values (±∞, NaN).

**When to use floating-point:**
- Scientific and engineering calculations
- Graphics and game development (positions, transforms)
- Financial calculations (with appropriate precision considerations)
- Machine learning and statistical computations
- Approximate real number representations

**When NOT to use floating-point:**
- Exact decimal arithmetic → use fixed-point or decimal types
- Integers → use integer types (§5.1)
- Equality comparisons → use epsilon-based comparisons
- Cryptographic operations → require exact integer arithmetic

**Relationship to other features:**
- **Default float type**: Float literals default to `f64`
- **Special values**: ±0, ±∞, NaN with IEEE 754 semantics
- **NaN propagation**: Operations with NaN produce NaN
- **Copy trait**: Both `f32` and `f64` implement `Copy` (can be explicitly copied)
- **Parameter passing**: Floats pass by permission like all types (no automatic copying)

### 5.2.2 Syntax

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

### 5.2.3 Static Semantics

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

| Type | Format | Sign bits | Exponent bits | Mantissa bits | Total bits |
|------|--------|-----------|---------------|---------------|------------|
| `f32` | binary32 | 1 | 8 | 23 | 32 |
| `f64` | binary64 | 1 | 11 | 52 | 64 |

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

### 5.2.4 Dynamic Semantics

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

### 5.2.5 Examples and Patterns

#### Safe Floating-Point Comparison

**Epsilon-based comparison:**
```cantrip
function approx_equal(a: f64, b: f64, epsilon: f64): bool {
    (a - b).abs() < epsilon
}

const EPSILON: f64 = 1e-10

let x = 0.1 + 0.2
let y = 0.3

// Direct comparison unreliable due to rounding:
assert!(x != y)  // May fail due to floating-point precision

// Use epsilon comparison:
assert!(approx_equal(x, y, EPSILON))
```

**Relative epsilon comparison:**
```cantrip
function relative_eq(a: f64, b: f64, epsilon: f64): bool {
    let abs_diff = (a - b).abs()
    let largest = a.abs().max(b.abs())
    abs_diff <= largest * epsilon
}
```

#### Handling Special Values

**Check for special values:**
```cantrip
function safe_compute(x: f64): Option<f64> {
    if x.is_nan() {
        None  // NaN detected
    } else if x.is_infinite() {
        None  // Infinity detected
    } else if x.is_finite() {
        Some(compute(x))  // Normal value
    } else {
        None
    }
}

function classify(x: f64): str {
    if x.is_nan() { "NaN" }
    else if x.is_infinite() {
        if x > 0.0 { "+Infinity" } else { "-Infinity" }
    }
    else if x == 0.0 {
        if x.is_sign_positive() { "+0" } else { "-0" }
    }
    else if x.is_normal() { "Normal" }
    else { "Subnormal" }
}
```

#### Mathematical Operations

**Common mathematical functions:**
```cantrip
let x: f64 = 2.0

// Exponential and logarithmic
let exp_x = x.exp()           // e^x
let ln_x = x.ln()             // natural log
let log10_x = x.log10()       // base-10 log
let log2_x = x.log2()         // base-2 log

// Trigonometric
let sin_x = x.sin()
let cos_x = x.cos()
let tan_x = x.tan()
let atan2_y_x = y.atan2(x)    // arctangent of y/x

// Power and roots
let sqrt_x = x.sqrt()         // square root
let cbrt_x = x.cbrt()         // cube root
let pow_x_y = x.powf(y)       // x^y

// Rounding
let floor_x = x.floor()       // round down
let ceil_x = x.ceil()         // round up
let round_x = x.round()       // round to nearest
let trunc_x = x.trunc()       // truncate toward zero
```

#### Type Conversions

**Float-to-float:**
```cantrip
let x: f64 = 3.14159265358979323846
let y: f32 = x as f32  // Precision loss: 3.1415927
```

**Float-to-integer:**
```cantrip
let f: f64 = 3.7
let i: i32 = f as i32  // Truncates: i = 3

// Saturating conversion
let large: f64 = 1e100
let saturated: i32 = large as i32  // i32::MAX (saturates)

let neg_large: f64 = -1e100
let saturated_neg: i32 = neg_large as i32  // i32::MIN

// NaN/Infinity conversion
let nan: f64 = 0.0 / 0.0
let nan_int: i32 = nan as i32  // 0 (implementation-defined)

let inf: f64 = 1.0 / 0.0
let inf_int: i32 = inf as i32  // i32::MAX (saturates)
```

**Integer-to-float:**
```cantrip
let i: i64 = 123456789
let f: f32 = i as f32  // May lose precision for large integers

// Exact conversion (if in range)
let small: i32 = 42
let exact: f64 = small as f64  // 42.0 (exact)
```

---

**Previous**: [Integer Types](01a_Integers.md) | **Next**: [Boolean Type](01c_Boolean.md)
