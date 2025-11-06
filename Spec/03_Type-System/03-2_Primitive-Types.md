# Part III: Type System
## Section 3.2: Primitive Types

**File**: `03-2_Primitive-Types.md`
**Version**: 2.0
**Status**: Normative
**Previous**: [03-1_Subtyping-Equivalence.md](03-1_Subtyping-Equivalence.md) | **Next**: [03-3_Composite-Types.md](03-3_Composite-Types.md)

---

## 3.2.1 Overview [primitive-types.overview]

[1] This section specifies Cursive's primitive types: the built-in scalar types that form the foundation of the type system.

[2] Primitive types shall have predefined semantics, memory layouts, and operations, and shall not be definable by users.

**Definition 3.2.1** (*Primitive Types*):
Primitive types are built-in types provided by the language with compiler-defined representations and operations.

[3] A conforming implementation shall provide all six categories of primitive types:
  (3.1) Integer Types (§3.2.2) - Fixed-width signed and unsigned integers
  (3.2) Floating-Point Types (§3.2.3) - IEEE 754 binary floating-point numbers
  (3.3) Boolean Type (§3.2.4) - Two-valued logic type
  (3.4) Character Type (§3.2.5) - Unicode scalar values
  (3.5) Unit Type (§3.2.6) - Zero-sized type representing absence of value
  (3.6) Never Type (§3.2.7) - Bottom type representing diverging computations

[4] All primitive types shall integrate with Cursive's permission system (Part VI) and grant system (Part IX).

[5] Integer and floating-point types shall provide well-defined overflow and special value semantics. Character types shall maintain UTF-8 validity guarantees.

**Cross-references:**
- Type system foundations: §3.0
- Subtyping relation: §3.1
- Permission system: Part VI §6.2
- Lexical elements and literals: Part II §2.2

---

## 3.2.2 Integer Types [primitive-types.integers]

### 3.2.2.1 Overview [primitive-types.integers.overview]

[6] Integer types shall provide fixed-width signed and unsigned integer values with explicit size and range guarantees.

[7] Cursive shall provide twelve integer types:
  (7.1) Five signed types: `i8`, `i16`, `i32`, `i64`, `i128`
  (7.2) Five unsigned types: `u8`, `u16`, `u32`, `u64`, `u128`
  (7.3) Two platform-dependent types: `isize`, `usize`

### 3.2.2.2 Syntax [primitive-types.integers.syntax]

[8] The syntax for integer types shall be:

**Definition 3.2.2** (*Integer Type Family*):
```
IntegerType ::= SignedInteger | UnsignedInteger

SignedInteger ::= 'i8' | 'i16' | 'i32' | 'i64' | 'i128' | 'isize'

UnsignedInteger ::= 'u8' | 'u16' | 'u32' | 'u64' | 'u128' | 'usize'
```

> **Normative Reference**: Complete grammar in Annex A §A.2 (Type Grammar).

### 3.2.2.3 Value Sets and Ranges [primitive-types.integers.ranges]

[9] For each integer type τ, the value set ⟦τ⟧ shall be defined as specified in Table 3.2.1.

**Table 3.2.1**: Integer Type Value Sets

| Type | Value Set ⟦τ⟧ | Minimum Value | Maximum Value | Size | Alignment |
|------|---------------|---------------|---------------|------|-----------|
| `i8` | ℤ ∩ [-2⁷, 2⁷-1] | -128 | 127 | 1 byte | 1 byte |
| `i16` | ℤ ∩ [-2¹⁵, 2¹⁵-1] | -32,768 | 32,767 | 2 bytes | 2 bytes |
| `i32` | ℤ ∩ [-2³¹, 2³¹-1] | -2,147,483,648 | 2,147,483,647 | 4 bytes | 4 bytes |
| `i64` | ℤ ∩ [-2⁶³, 2⁶³-1] | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807 | 8 bytes | 8 bytes |
| `i128` | ℤ ∩ [-2¹²⁷, 2¹²⁷-1] | -(2¹²⁷) | 2¹²⁷-1 | 16 bytes | 8 or 16 bytes* |
| `isize` | ℤ ∩ [-2ⁿ⁻¹, 2ⁿ⁻¹-1] | Platform-dependent | Platform-dependent | n/8 bytes | n/8 bytes |
| `u8` | ℕ ∩ [0, 2⁸-1] | 0 | 255 | 1 byte | 1 byte |
| `u16` | ℕ ∩ [0, 2¹⁶-1] | 0 | 65,535 | 2 bytes | 2 bytes |
| `u32` | ℕ ∩ [0, 2³²-1] | 0 | 4,294,967,295 | 4 bytes | 4 bytes |
| `u64` | ℕ ∩ [0, 2⁶⁴-1] | 0 | 18,446,744,073,709,551,615 | 8 bytes | 8 bytes |
| `u128` | ℕ ∩ [0, 2¹²⁸-1] | 0 | 2¹²⁸-1 | 16 bytes | 8 or 16 bytes* |
| `usize` | ℕ ∩ [0, 2ⁿ-1] | 0 | Platform-dependent | n/8 bytes | n/8 bytes |

*Note: Alignment of `i128` and `u128` is implementation-defined; conforming implementations shall use either 8-byte or 16-byte alignment.

[10] Where n represents the pointer width of the target platform (32 or 64 bits).

[11] On 32-bit platforms, `isize` shall be equivalent to `i32` and `usize` shall be equivalent to `u32`. On 64-bit platforms, `isize` shall be equivalent to `i64` and `usize` shall be equivalent to `u64`.

```
32-bit platforms: isize ≡ i32, usize ≡ u32, n = 32
64-bit platforms: isize ≡ i64, usize ≡ u64, n = 64
```

[12] Implementations shall document the platform-specific values of `isize` and `usize`.

**Example 1**: Integer type value ranges ✅
```cursive
let tiny: const i8 = 127            // Maximum i8 value
let small: const i16 = -32_768      // Minimum i16 value
let default: const i32 = 42         // Default integer literal type
let large: const u64 = 18_446_744_073_709_551_615u64
let platform: const usize = 1000    // Platform-dependent size
```

### 3.2.2.4 Memory Representation [primitive-types.integers.representation]

[13] Signed integer types shall use two's complement representation.

[14] Unsigned integer types shall use standard unsigned binary representation.

[15] Byte order (endianness) is implementation-defined. Implementations shall document whether they use little-endian, big-endian, or support both.

### 3.2.2.5 Type Formation [primitive-types.integers.formation]

[16] Integer types shall be well-formed in any type context:

**Well-Formedness Rule:**
```
[WF-Int-Type]
τ ∈ {i8, i16, i32, i64, i128, isize, u8, u16, u32, u64, u128, usize}
────────────────────────────────────────────────────────────────────
Γ ⊢ τ : Type
```

### 3.2.2.6 Integer Literal Typing [primitive-types.integers.literal-typing]

[17] Integer literals without type suffixes shall default to type `i32`:

**Integer Literal Typing Rules:**
```
[Type-Int-Literal-Default]
n is an integer literal without type suffix
───────────────────────────────────────────
Γ ⊢ n : i32    (default type)


[Type-Int-Literal-Suffix]
n is an integer literal with suffix τ
τ ∈ IntegerTypes
───────────────────────────────────────
Γ ⊢ n : τ    (explicit type from suffix)


[Type-Int-Literal-Context]
n is an integer literal without type suffix
Γ ⊢ context expects type τ
τ ∈ IntegerTypes
n ∈ ⟦τ⟧    (n fits in range of τ)
────────────────────────────────────────
Γ ⊢ n : τ    (type from context)
```

[18] Type inference priority shall be: (1) Explicit suffix (highest), (2) Context annotation, (3) Default `i32` (lowest).

**Example 2**: Integer literal type inference ✅
```cursive
let default_type = 100              // Infers i32 (default)
let context_type: u16 = 100         // Context provides u16
let suffix_type = 100u64            // Suffix overrides (highest priority)
let hex_value: const u32 = 0xFF00   // Context infers u32
```

### 3.2.2.7 Integer Literal Syntax [primitive-types.integers.literal-syntax]

[19] Integer literals may be written in decimal, hexadecimal, octal, or binary form:
  (19.1) **Decimal**: `0`, `42`, `1_000_000`
  (19.2) **Hexadecimal**: `0xFF`, `0xDEAD_BEEF`
  (19.3) **Octal**: `0o755`, `0o644`
  (19.4) **Binary**: `0b1010`, `0b1111_0000`

[20] Underscores (`_`) may appear between digits for readability.

[21] Underscores shall not appear at the start or end of a literal, nor immediately after a base prefix (`0x`, `0o`, `0b`).

[22] Any integer type name may be appended to an integer literal as a type suffix: `42u8`, `0xFFi64`, `1000usize`.

**Example 3**: Integer literal forms ✅
```cursive
let decimal: const i32 = 42
let hex: const u32 = 0xFF00
let octal: const u16 = 0o755
let binary: const u8 = 0b1010_1010
let with_suffix: const u64 = 1_000_000u64
let separated: const i32 = 1_234_567
```

### 3.2.2.8 Arithmetic Operations [primitive-types.integers.arithmetic]

[23] Arithmetic operations on integer types shall include addition (`+`), subtraction (`-`), multiplication (`*`), division (`/`), and remainder (`%`).

[24] Overflow behavior (wrapping, checked, saturating, or panicking) shall be specified at the operation level, not the type level.

> **Forward reference**: Complete specification of arithmetic semantics, including overflow behavior, is provided in Part IV §4.4.1 (Arithmetic Operators).

**Example 4**: Integer arithmetic ✅
```cursive
let a: const i32 = 10
let b: const i32 = 3

let sum = a + b         // 13
let diff = a - b        // 7
let product = a * b     // 30
let quotient = a / b    // 3 (integer division)
let remainder = a % b   // 1
```

### 3.2.2.9 Type Properties [primitive-types.integers.properties]

[25] All integer types shall implement the `Copy` predicate:

**Theorem 3.2.1** (*Integer Copy Predicate*):
```
∀τ ∈ IntegerTypes. τ : Copy
```

**Proof**: Integer types have no heap-allocated components and are bitwise-copyable.

> **Forward reference**: Copy semantics specified in Part VI §6.3 (Move Semantics) and Part VIII §8.6 (Marker Predicates).

**Example 5**: Integer copy semantics ✅
```cursive
let x: const i32 = 42
let y: const i32 = x    // Copy occurs (x is still valid)
let z: const i32 = x    // Another copy (x still valid)
```

---

## 3.2.3 Floating-Point Types [primitive-types.floats]

### 3.2.3.1 Overview [primitive-types.floats.overview]

[26] Cursive shall provide two floating-point types conforming to IEEE 754-2019 binary floating-point arithmetic: `f32` (binary32, single precision) and `f64` (binary64, double precision).

### 3.2.3.2 Syntax [primitive-types.floats.syntax]

[27] The syntax for floating-point types shall be:

**Definition 3.2.3** (*Floating-Point Type Family*):
```
FloatType ::= 'f32' | 'f64'
```

> **Normative Reference**: Complete grammar in Annex A §A.2 (Type Grammar).

[28] Cursive floating-point types shall conform to IEEE 754-2019 binary floating-point arithmetic standard.

### 3.2.3.3 Value Sets and Special Values [primitive-types.floats.values]

[29] Cursive floating-point types shall conform to IEEE 754-2019 as specified in Table 3.2.2.

**Table 3.2.2**: Floating-Point Type Specifications

| Type | IEEE 754 Name | Size | Alignment | Precision | Exponent Range |
|------|---------------|------|-----------|-----------|----------------|
| `f32` | binary32 | 4 bytes | 4 bytes | 24 bits (≈7 decimal digits) | -126 to +127 |
| `f64` | binary64 | 8 bytes | 8 bytes | 53 bits (≈16 decimal digits) | -1022 to +1023 |

[30] Both types shall support the following special values defined by IEEE 754:
  (30.1) **Positive infinity** (`+∞`): Result of overflow or division of positive number by zero
  (30.2) **Negative infinity** (`-∞`): Result of overflow or division of negative number by zero
  (30.3) **Not-a-Number** (`NaN`): Result of undefined operations (0/0, ∞-∞, sqrt(-1), etc.)
  (30.4) **Positive zero** (`+0.0`)
  (30.5) **Negative zero** (`-0.0`)

[31] All operations on floating-point values shall follow IEEE 754-2019 semantics, including special value propagation.

**Example 6**: Floating-point special values ✅
```cursive
let pos_infinity: const f64 = 1.0 / 0.0         // +∞
let neg_infinity: const f64 = -1.0 / 0.0        // -∞
let nan_zero_div: const f64 = 0.0 / 0.0         // NaN from 0/0
let nan_inf_sub: const f64 = pos_infinity - pos_infinity // NaN from ∞ - ∞
let pos_zero: const f64 = 0.0                   // +0.0
let neg_zero: const f64 = -0.0                  // -0.0
```

### 3.2.3.4 Type Formation [primitive-types.floats.formation]

[32] Floating-point types shall be well-formed in any type context:

**Well-Formedness Rule:**
```
[WF-Float-Type]
τ ∈ {f32, f64}
──────────────
Γ ⊢ τ : Type
```

### 3.2.3.5 Floating-Point Literal Typing [primitive-types.floats.literal-typing]

[33] Floating-point literals without suffixes shall default to `f64`:

**Floating-Point Literal Typing Rules:**
```
[Type-Float-Literal-Default]
f is a floating-point literal without type suffix
─────────────────────────────────────────────────
Γ ⊢ f : f64    (default type)


[Type-Float-Literal-Suffix]
f is a floating-point literal with suffix τ
τ ∈ {f32, f64}
──────────────────────────────────────────
Γ ⊢ f : τ    (explicit type from suffix)
```

### 3.2.3.6 Floating-Point Literal Syntax [primitive-types.floats.literal-syntax]

[34] Floating-point literals may be written in the following forms:
  (34.1) **Decimal form**: `3.14`, `0.5`, `2.0`
  (34.2) **Scientific notation**: `6.022e23`, `1.5e-10`, `2.99E8`
  (34.3) **With type suffix**: `3.14f32`, `2.71828f64`

[35] At least one digit shall appear before or after the decimal point.

[36] Scientific notation shall use `e` or `E` followed by an optional sign and exponent digits.

**Example 7**: Floating-point literal forms ✅
```cursive
let pi: const f64 = 3.14159265359
let e: const f32 = 2.71828f32
let avogadro: const f64 = 6.022e23
let planck: const f64 = 6.626e-34
```

### 3.2.3.7 Special Value Semantics [primitive-types.floats.special-values]

[37] NaN shall not be equal to any value, including itself:

**Theorem 3.2.2** (*NaN Inequality*):
```
NaN ≠ NaN
∀x. (x ≟ NaN) = false
```

[38] Positive and negative zero shall compare equal:

**Theorem 3.2.3** (*Signed Zero Equality*):
```
+0.0 == -0.0
```

[39] However, signed zeros shall behave differently in certain operations (e.g., `1.0 / +0.0 = +∞`, `1.0 / -0.0 = -∞`).

**Theorem 3.2.4** (*Infinity Operations*):
```
∀x ∈ ℝ. x / +0.0 = +∞  (x > 0)
∀x ∈ ℝ. x / +0.0 = -∞  (x < 0)
∀x ∈ ℝ. +∞ + x = +∞
∀x ∈ ℝ. -∞ + x = -∞
+∞ - +∞ = NaN
```

> **Forward reference**: Complete floating-point arithmetic semantics in Part IV §4.4.1.

**Example 8**: Special value semantics ✅
```cursive
// NaN inequality: NaN ≠ NaN
let nan1: const f64 = 0.0 / 0.0
let nan2: const f64 = 0.0 / 0.0
println("Is NaN == NaN? {}", nan1 == nan2)  // false!

// Signed zero equality: +0.0 == -0.0
println("Is +0.0 == -0.0? {}", 0.0 == -0.0) // true

// But signed zeros behave differently:
println("1.0 / +0.0 = {}", 1.0 / 0.0)       // +∞
println("1.0 / -0.0 = {}", 1.0 / -0.0)      // -∞
```

### 3.2.3.8 Type Properties [primitive-types.floats.properties]

[40] Both floating-point types shall implement the `Copy` predicate:

**Theorem 3.2.5** (*Float Copy Predicate*):
```
f32 : Copy
f64 : Copy
```

> **Note**: NaN values present challenges for equality-based operations. See Part VIII §8.6 for discussion of `Eq` vs `PartialEq` predicates.

**Example 9**: Floating-point precision ✅
```cursive
let precise_f64: const f64 = 0.1 + 0.2      // ≈0.30000000000000004
let precise_f32: const f32 = 0.1f32 + 0.2f32 // ≈0.3 (less precision)
println("f64: 0.1 + 0.2 = {}", precise_f64) // Shows precision limits
```

---

## 3.2.4 Boolean Type [primitive-types.bool]

### 3.2.4.1 Overview [primitive-types.bool.overview]

[41] The boolean type `bool` shall represent two-valued logic with values `true` and `false`.

[42] Boolean operations shall use short-circuit evaluation semantics for logical AND and OR operators.

### 3.2.4.2 Syntax [primitive-types.bool.syntax]

[43] The syntax for the boolean type shall be:

**Definition 3.2.4** (*Boolean Type*):
```
BoolType ::= 'bool'

BoolLiteral ::= 'true' | 'false'
```

> **Normative Reference**: Complete grammar in Annex A §A.2 (Type Grammar) and A §A.1 (Lexical Grammar).

### 3.2.4.3 Type Formation [primitive-types.bool.formation]

[44] The boolean type shall be well-formed:

**Well-Formedness and Literal Typing Rules:**
```
[WF-Bool-Type]
──────────────────
Γ ⊢ bool : Type


[Type-Bool-True]
────────────────
Γ ⊢ true : bool


[Type-Bool-False]
─────────────────
Γ ⊢ false : bool
```

### 3.2.4.4 Value Set and Properties

[45] The boolean type shall have exactly two values:

**Theorem 3.2.6** (*Boolean Value Set*):
```
⟦bool⟧ = {true, false} ≅ {⊤, ⊥} ≅ ℤ₂
```

The boolean type is isomorphic to the two-element field ℤ₂.

**Theorem 3.2.7** (*Boolean Size and Alignment*):
```
sizeof(bool) = 1 byte
alignof(bool) = 1 byte
```

[46] A conforming implementation shall represent `false` as `0x00` and `true` as `0x01`.

[47] Implementations may treat any non-zero byte value as `true` when loading from unsafe contexts, but shall normalize to `0x01` when storing.

**Theorem 3.2.8** (*Boolean Copy Predicate*):
```
bool : Copy
```

**Example 10**: Boolean values and operations ✅
```cursive
let true_val: const bool = true
let false_val: const bool = false

// Logical operations
let and_result: const bool = true && false      // false
let or_result: const bool = true || false       // true
let not_result: const bool = !true              // false
let xor_result: const bool = true ^ false       // true
```

### 3.2.4.5 Boolean Operations

[48] Cursive shall provide the following boolean operations:
  (48.1) **Logical AND** (`&&`): Short-circuit conjunction
  (48.2) **Logical OR** (`||`): Short-circuit disjunction
  (48.3) **Logical NOT** (`!`): Negation
  (48.4) **Logical XOR** (`^`): Exclusive or

[49] For `e₁ && e₂`, if `e₁` evaluates to `false`, then `e₂` shall not be evaluated.

[50] For `e₁ || e₂`, if `e₁` evaluates to `true`, then `e₂` shall not be evaluated.

> **Forward reference**: Complete operational semantics in Part IV §4.4.3 (Logical Operators).

**Example 11**: Short-circuit evaluation ✅
```cursive
var count: const i32 = 0

// Short-circuit AND: second operand NOT evaluated when first is false
let short_and = false && {
    count = count + 1  // This NEVER executes
    true
}
println("Count = {}", count)  // Still 0

// Short-circuit OR: second operand NOT evaluated when first is true
let short_or = true || {
    count = count + 1  // This NEVER executes
    false
}
println("Count = {}", count)  // Still 0
```

---

## 3.2.5 Character Type

### 3.2.5.1 Overview

[51] The character type `char` shall represent a Unicode scalar value: any Unicode code point in the range U+0000 to U+D7FF or U+E000 to U+10FFFF.

[52] Surrogate code points (U+D800 to U+DFFF) shall be excluded from valid character values.

### 3.2.5.2 Syntax

[53] The syntax for character types shall be:

**Definition 3.2.5** (*Character Type*):
```
CharType ::= 'char'

CharLiteral ::= "'" CharContent "'"

CharContent ::= SimpleChar
              | EscapeSequence
              | UnicodeEscape
```

> **Normative Reference**: Complete literal syntax in Annex A §A.1 (Lexical Grammar).

### 3.2.5.3 Valid Character Values

[54] The value set for `char` shall be:

**Theorem 3.2.9** (*Character Value Set*):
```
⟦char⟧ = [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]

Total valid values: 1,112,064 Unicode scalar values
  • U+0000 to U+D7FF: 55,296 code points
  • U+E000 to U+10FFFF: 1,056,768 code points

Excluded: [U+D800, U+DFFF] = 2,048 surrogate code points
```

[55] Surrogate code points (U+D800 through U+DFFF) shall not be valid `char` values.

[56] A conforming implementation shall reject character literals containing surrogate code points with a compile-time diagnostic (Error E0301).

### 3.2.5.4 Type Formation

[57] The character type shall be well-formed:

**Character Type Formation Rules:**
```
[WF-Char-Type]
──────────────────
Γ ⊢ char : Type


[Type-Char-Valid]
c is a character literal
codepoint(c) ∈ [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]
────────────────────────────────────────────────────
Γ ⊢ c : char


[Type-Char-Invalid]
c is a character literal
codepoint(c) ∈ [U+D800, U+DFFF]    (surrogate range)
────────────────────────────────────────────────────
Ill-formed: Surrogate code points are invalid
Error: E0301 - Invalid character literal (surrogate code point)
```

### 3.2.5.5 Character Literal Syntax

[58] Character literals shall be written as a single character enclosed in single quotes (`'`).

[59] The escape sequences specified in Table 3.2.3 shall be recognized.

**Table 3.2.3**: Character Escape Sequences

| Escape | Meaning | Unicode |
|--------|---------|---------|
| `\n` | Newline (line feed) | U+000A |
| `\r` | Carriage return | U+000D |
| `\t` | Horizontal tab | U+0009 |
| `\\` | Backslash | U+005C |
| `\'` | Single quote | U+0027 |
| `\"` | Double quote | U+0022 |
| `\0` | Null character | U+0000 |
| `\u{HHHH}` | Unicode escape (1-6 hex digits) | U+HHHH |

[60] The `\u{HHHH}` form shall accept 1 to 6 hexadecimal digits representing a valid Unicode scalar value.

**Example 12**: Character literals ✅
```cursive
let ascii: const char = 'A'                  // U+0041
let digit: const char = '7'                  // U+0037
let emoji: const char = '🦀'                 // U+1F980 (crab emoji)
let chinese: const char = '中'               // U+4E2D
let newline: const char = '\n'               // U+000A
let tab: const char = '\t'                   // U+0009
let unicode: const char = '\u{1F600}'        // U+1F600 (grinning face)
let null: const char = '\0'                  // U+0000
```

**Example 13**: Invalid character literals ❌
```cursive
// ERROR E0301: Surrogate code points are invalid
// let invalid: const char = '\u{D800}'
// let also_invalid: const char = '\u{DFFF}'
```

### 3.2.5.6 Size, Alignment, and Representation

[61] The size and alignment of `char` shall be:

**Theorem 3.2.10** (*Character Size and Alignment*):
```
sizeof(char) = 4 bytes
alignof(char) = 4 bytes
```

[62] A `char` value shall be stored as a 32-bit unsigned integer representing the Unicode code point (UTF-32 representation).

**Example representation**:
```
'A'  → 0x00000041
'€'  → 0x000020AC
'🚀' → 0x0001F680
```

### 3.2.5.7 Type Properties

[63] The character type shall implement the `Copy` predicate:

**Theorem 3.2.11** (*Character Copy Predicate*):
```
char : Copy
```

[64] Character values shall be totally ordered by their Unicode scalar value:

**Theorem 3.2.12** (*Character Ordering*):
```
∀c₁, c₂ : char. c₁ < c₂ ⟺ codepoint(c₁) < codepoint(c₂)
```

**Example 14**: Character comparison ✅
```cursive
// ASCII ordering
println("'A' < 'Z': {}", 'A' < 'Z')         // true (U+0041 < U+005A)
println("'a' > 'A': {}", 'a' > 'A')         // true (U+0061 > U+0041)
println("'0' < '9': {}", '0' < '9')         // true (U+0030 < U+0039)

// Unicode ordering
println("'A' < '中': {}", 'A' < '中')       // true (U+0041 < U+4E2D)
```

---

## 3.2.6 Unit Type

### 3.2.6.1 Overview

[65] The unit type `()` shall be a zero-sized type with a single value, also written `()`.

[66] The unit type shall represent the absence of a meaningful value and is commonly used as the return type of procedures that perform side effects but do not return data.

### 3.2.6.2 Syntax

[67] The syntax for the unit type shall be:

**Definition 3.2.6** (*Unit Type*):
```
UnitType ::= '(' ')'

UnitValue ::= '(' ')'
```

The same syntax `()` denotes both the unit type and its sole inhabitant.

### 3.2.6.3 Type Formation

[68] The unit type shall be well-formed:

**Unit Type Formation Rules:**
```
[WF-Unit-Type]
─────────────────
Γ ⊢ () : Type


[Type-Unit-Value]
─────────────────
Γ ⊢ () : ()
```

### 3.2.6.4 Value Set and Properties

[69] The unit type shall have exactly one value:

**Theorem 3.2.13** (*Unit Value Set*):
```
⟦()⟧ = {()}
```

**Theorem 3.2.14** (*Unit Zero Size*):
```
sizeof(()) = 0 bytes
alignof(()) = 1 byte
```

[70] Implementations shall not allocate storage for values of unit type.

**Theorem 3.2.15** (*Unit Copy Predicate*):
```
() : Copy
```

Copying a unit value is trivial (zero bytes copied).

### 3.2.6.5 Usage and Semantics

[71] The unit type shall be used primarily in two contexts:
  (71.1) **Procedure return types**: Procedures that perform effects but return no data
  (71.2) **Empty tuple**: The unit type is the zero-element tuple

[72] In expression contexts, a block that does not end with a result expression shall have type `()`.

**Example 15**: Unit type usage ✅
```cursive
// Procedure returning unit (implicit)
procedure print_message(msg: string)
    grants io::write
{
    println(msg)
    // Implicit return of ()
}

// Explicit unit return
procedure log_event(event: string) -> ()
    grants io::write
{
    println("Event: {}", event)
    result ()  // Explicit unit value
}

// Unit value binding
let unit_val: const () = ()

// Unit in arrays (zero bytes!)
let unit_array: [(); 10] = [(); 10]
```

---

## 3.2.7 Never Type

### 3.2.7.1 Overview

[73] The never type `!` shall be the uninhabited bottom type (⊥) that represents computations which never complete normally.

[74] Functions returning `!` shall either diverge (loop forever), panic (abnormal termination), or exit the process.

### 3.2.7.2 Syntax

[75] The syntax for the never type shall be:

**Definition 3.2.7** (*Never Type*):
```
NeverType ::= '!'
```

[76] The never type shall be uninhabited—there are no values of type `!`.

### 3.2.7.3 Type Formation and Subtyping

[77] The never type shall be well-formed:

**Never Type Formation Rules:**
```
[WF-Never-Type]
────────────────
Γ ⊢ ! : Type


[Sub-Never]
For all types τ:
────────────────
! <: τ
```

[78] The never type shall be a subtype of every other type:

**Theorem 3.2.16** (*Bottom Type*):
The never type is a subtype of every other type. This property allows diverging expressions to appear in any type context.

[79] Since a function returning `!` never produces a value, it can be safely used where any type is expected. The subtyping relation `! <: τ` enables type checking of branches where one arm diverges.

### 3.2.7.4 Empty Value Set

[80] The never type shall have no values:

**Theorem 3.2.17** (*Uninhabited Type*):
```
⟦!⟧ = ∅
```

It is impossible to construct a value of type `!`.

**Theorem 3.2.18** (*Never Zero Size*):
```
sizeof(!) = 0 bytes
alignof(!) = 1 byte
```

Since no values of type `!` can exist, the never type occupies zero bytes.

### 3.2.7.5 Typing Rules

[81] An expression of type `!` can be used where any type `τ` is expected:

**Never Coercion Rule:**
```
[Type-Never-Coercion]
Γ ⊢ e : !
Γ ⊢ τ : Type
─────────────
Γ ⊢ e : τ
```

**Example 16**: Never type in branching ✅
```cursive
function get_value(condition: bool): i32
    grants panic
{
    if condition {
        result 42         // Type: i32
    } else {
        panic("Failed")   // Type: !, coerces to i32
    }
    // Both branches must have compatible types
    // ! <: i32, so this is valid
}
```

### 3.2.7.6 Diverging Expressions

[82] Expressions that have type `!` shall include:
  (82.1) **Infinite loops**: `loop { }`
  (82.2) **Panic expressions**: `panic("Error message")`
  (82.3) **Process exit**: `std::process::exit(1)`
  (82.4) **Functions declared to return `!`**

**Example 17**: Diverging expressions ✅
```cursive
// Infinite loop
function infinite_loop(): ! {
    loop {
        // Never exits
    }
}

// Panic expression
function always_fails(reason: string): !
    grants panic
{
    panic(reason)
}

// Process exit
function exit_program(code: i32): !
    grants process::exit
{
    std::process::exit(code)
}
```

**Example 18**: Never type in match expressions ✅
```cursive
function process_code(code: i32): string
    grants panic
{
    match code {
        0 => panic("Invalid code 0"),        // Type: !
        1 => "Success",                       // Type: string
        2 => "Pending",                       // Type: string
        n => format!("Unknown code: {}", n)   // Type: string
    }
    // All arms must have compatible types
    // ! <: string, so the panic arm is compatible
}
```

> **Forward reference**: For complete semantics of diverging expressions and control flow, see Part V (Statements and Control Flow).

---

## 3.2.8 Integration

[83] The primitive types specified in this section integrate with the following language components:

**Cross-references:**
- Type system foundations: §3.0
- Subtyping and equivalence: §3.1
- Arithmetic operations: Part IV §4.4.1
- Logical operations: Part IV §4.4.3
- Permission system: Part VI §6.2
- Move semantics and Copy predicate: Part VI §6.3
- Marker predicates: Part VIII §8.6
- Control flow and divergence: Part V §5.2
- Lexical elements: Part II §2.2

[84] All primitive types are nominal types with well-defined sizes, alignments, and value sets.

[85] The never type's bottom type property (`! <: τ`) shall be used during type checking to allow diverging computations in any type context.

---

**Previous**: [03-1_Subtyping-Equivalence.md](03-1_Subtyping-Equivalence.md) | **Next**: [03-3_Composite-Types.md](03-3_Composite-Types.md)
