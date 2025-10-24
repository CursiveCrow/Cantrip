# Part II: Type System - §5.1 Integer Types

**Section**: §5.1 | **Part**: Type System (Part II)
**Parent**: [Primitive Types](01_PrimitiveTypes.md) | **Next**: [Floating-Point Types](01b_FloatingPoint.md)

---

**Definition 5.1.1 (Integer Types):** Integer types represent whole numbers with fixed bit-width and explicit signedness. The set of integer types IntTypes = {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize} provides both signed (two's complement) and unsigned representations.

## 5.1 Integer Types

### 5.1.1 Overview

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
- Fractional numbers → use floating-point types (§5.2)
- Boolean flags → use `bool` type (§5.3)
- Text/characters → use `char` or string types

**Relationship to other features:**
- **Default integer type**: Integer literals without suffix default to `i32`
- **Overflow behavior**: Debug mode panics, release mode wraps (two's complement)
- **Type inference**: Context-sensitive (function signatures, annotations)
- **Permission system**: All integers are `Copy` types

### 5.1.2 Syntax

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
let a: i32 = 42;
let b: u64 = 1_000_000;    // Underscores for readability
let c = 42i8;              // Type suffix

// Hexadecimal
let hex: u32 = 0xDEAD_BEEF;

// Octal
let oct: u16 = 0o755;

// Binary
let bin: u8 = 0b1010_1010;
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

#### Basic Examples

**Integer literals in various bases:**
```cantrip
// Decimal (base 10)
let decimal: i32 = 42;
let negative: i16 = -128;

// Hexadecimal (base 16)
let hex: u32 = 0xDEAD_BEEF;
let small_hex: u8 = 0xFF;

// Octal (base 8)
let octal: i16 = 0o755;

// Binary (base 2)
let binary: u8 = 0b1010_1010;
```

**Explanation:** Integer literals can be written in decimal, hexadecimal (0x prefix), octal (0o prefix), or binary (0b prefix) notation. Underscores can be used as visual separators for readability and are ignored by the compiler.

**Type annotations and inference:**
```cantrip
// Explicit type annotation
let age: u8 = 30;
let population: u64 = 8_000_000_000;

// Type suffix
let small = 100u8;
let large = 1_000_000u64;

// Default inference (i32)
let default = 42;  // Inferred as i32
```

**Explanation:** Integer types can be specified via explicit type annotations, type suffixes on literals, or inferred from context. Without explicit specification, integer literals default to `i32`.

**Basic arithmetic operations:**
```cantrip
let a: i32 = 10;
let b: i32 = 3;

let sum = a + b;        // 13
let diff = a - b;       // 7
let product = a * b;    // 30
let quotient = a / b;   // 3 (integer division)
let remainder = a % b;  // 1
```

**Explanation:** Integer types support standard arithmetic operations. Division truncates toward zero, and modulo preserves the sign of the left operand.

### 5.1.3 Static Semantics

#### Well-Formedness Rules

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
let x = 42;           // Type: i32 (default)
let y: u8 = 42;       // Type: u8 (context)
let z = 42u64;        // Type: u64 (suffix)
let w: i8 = 200;      // ERROR E5101: 200 doesn't fit in i8 range [-128, 127]
```

#### Type Properties

**Theorem 5.1.1 (Value Sets):**

For each integer type T, the value set ⟦T⟧ is defined as:

| Type | Value Set ⟦T⟧ | Min Value | Max Value | Size | Align |
|------|---------------|-----------|-----------|------|-------|
| `i8` | ℤ ∩ [-2⁷, 2⁷-1] | -128 | 127 | 1 byte | 1 byte |
| `i16` | ℤ ∩ [-2¹⁵, 2¹⁵-1] | -32,768 | 32,767 | 2 bytes | 2 bytes |
| `i32` | ℤ ∩ [-2³¹, 2³¹-1] | -2,147,483,648 | 2,147,483,647 | 4 bytes | 4 bytes |
| `i64` | ℤ ∩ [-2⁶³, 2⁶³-1] | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807 | 8 bytes | 8 bytes |
| `isize` | ℤ ∩ [-2ⁿ⁻¹, 2ⁿ⁻¹-1] | Platform-dependent | Platform-dependent | Platform | Platform |
| `u8` | ℕ ∩ [0, 2⁸-1] | 0 | 255 | 1 byte | 1 byte |
| `u16` | ℕ ∩ [0, 2¹⁶-1] | 0 | 65,535 | 2 bytes | 2 bytes |
| `u32` | ℕ ∩ [0, 2³²-1] | 0 | 4,294,967,295 | 4 bytes | 4 bytes |
| `u64` | ℕ ∩ [0, 2⁶⁴-1] | 0 | 18,446,744,073,709,551,615 | 8 bytes | 8 bytes |
| `usize` | ℕ ∩ [0, 2ⁿ-1] | 0 | Platform-dependent | Platform | Platform |

Where n = pointer width (32 or 64 bits depending on target architecture).

**Theorem 5.1.2 (Copy Semantics):**

All integer types implement the `Copy` trait:

```
∀ T ∈ IntTypes. T : Copy
```

This means integer values are automatically copied on assignment and parameter passing.

**Theorem 5.1.3 (Platform-Dependent Sizes):**

```
32-bit systems: isize = i32, usize = u32, n = 32
64-bit systems: isize = i64, usize = u64, n = 64
```

**Corollary:** Code using `isize`/`usize` may have different behavior on different architectures.

### 5.1.4 Dynamic Semantics

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
let x: u8 = 255;
let y = x + 1;
// Debug: panic!("attempt to add with overflow")
// Release: y = 0 (wrapping: 256 mod 256 = 0)

// i8: range [-128, 127]
let a: i8 = 127;
let b = a + 1;
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
let x: u32 = 100;
let y: u32 = 200;
let sum = x.checked_add(y)?;  // Some(300)

let max: u32 = u32::MAX;
let overflow = max.checked_add(1);  // None

// Saturating arithmetic (clamps at limits)
let sat_sum = max.saturating_add(1);  // u32::MAX

// Wrapping arithmetic (explicit wrap-around)
let wrap_sum = max.wrapping_add(1);  // 0

// Overflowing arithmetic (returns tuple with overflow flag)
let (result, overflowed) = max.overflowing_add(1);
// result = 0, overflowed = true
```

**Division and modulo:**

```cantrip
// Division by zero: always panics (both debug and release)
let x = 10 / 0;  // panic!("attempt to divide by zero")

// Integer division truncates toward zero
assert!(7 / 2 == 3);
assert!(-7 / 2 == -3);

// Modulo has sign of left operand
assert!(7 % 3 == 1);
assert!(-7 % 3 == -1);
assert!(7 % -3 == 1);
assert!(-7 % -3 == -1);

// Special case: MIN / -1 can overflow
let min: i8 = i8::MIN;  // -128
let result = min / -1;  // Would be 128, which overflows i8
// Debug: panic!("attempt to divide with overflow")
// Release: result = -128 (wraps)
```

### 5.1.5 Examples and Patterns

#### Type Annotation and Inference

**Explicit types:**
```cantrip
let age: u8 = 30;
let population: u64 = 8_000_000_000;
let temperature: i16 = -40;
```

**Type inference from context:**
```cantrip
function calculate(x: f32): f32 {
    x * 2.0
}

let numbers: Vec<i64> = Vec.new();
numbers.push(42);  // 42 inferred as i64

function process(bytes: [u8; 4]) { ... }
process([1, 2, 3, 4]);  // Literals inferred as u8
```

#### Safe Integer Operations

**Checked arithmetic:**
```cantrip
function safe_add(a: u32, b: u32): Option<u32> {
    a.checked_add(b)
}

match safe_add(u32::MAX, 1) {
    Some(result) -> println("Result: {result}"),
    None -> println("Overflow detected"),
}
```

**Saturating arithmetic (clamps at bounds):**
```cantrip
function bounded_increment(counter: u8): u8 {
    counter.saturating_add(1)
}

let x: u8 = 255;
assert!(bounded_increment(x) == 255);  // Clamps at max
```

**Wrapping arithmetic (explicit wrap-around):**
```cantrip
function hash_combine(h1: u64, h2: u64): u64 {
    h1.wrapping_mul(31).wrapping_add(h2)
}
```

#### Bit Manipulation

**Bitwise operations:**
```cantrip
let a: u8 = 0b1010_1010;
let b: u8 = 0b1100_0011;

let and = a & b;         // 0b1000_0010
let or  = a | b;         // 0b1110_1011
let xor = a ^ b;         // 0b0110_1001
let not = !a;            // 0b0101_0101

// Bit shifts
let left = a << 2;       // 0b1010_1000 (logical shift)
let right = a >> 2;      // 0b0010_1010 (logical for unsigned)

// Signed right shift (arithmetic)
let neg: i8 = -64;       // 0b1100_0000
let arith = neg >> 2;    // 0b1111_0000 (-16, sign-extended)
```

#### Type Conversions

**Explicit casts:**
```cantrip
let a: i32 = 42;
let b: i64 = a as i64;       // Widening cast (safe)

let large: i64 = 1_000_000;
let small: i32 = large as i32;  // Narrowing cast (truncates high bits)

// Sign conversions
let signed: i32 = -1;
let unsigned: u32 = signed as u32;  // Bit pattern reinterpretation
// unsigned = 4294967295 (0xFFFFFFFF)
```

**Safe conversions:**
```cantrip
function safe_downcast(x: i64): Option<i32> {
    if x >= i32::MIN as i64 && x <= i32::MAX as i64 {
        Some(x as i32)
    } else {
        None
    }
}

// Or using TryFrom trait
use std::convert::TryFrom;

let x: i64 = 42;
let y = i32::try_from(x)?;  // Returns Result<i32, Error>
```

#### Platform-Specific Code

**Using isize/usize:**
```cantrip
// Array indexing (always use usize)
function get_element<T>(arr: [T], index: usize): Option<T>
    where T: Copy
{
    if index < arr.len() {
        Some(arr[index])
    } else {
        None
    }
}

// Pointer arithmetic (use isize for offsets)
function offset_ptr<T>(ptr: *T, offset: isize): *T {
    unsafe {
        ptr.offset(offset)
    }
}

// Memory sizes and alignments (use usize)
let size: usize = std::mem::size_of::<MyStruct>();
let align: usize = std::mem::align_of::<MyStruct>();
```

---

**Parent**: [Primitive Types](01_PrimitiveTypes.md) | **Next**: [Floating-Point Types](01b_FloatingPoint.md)
