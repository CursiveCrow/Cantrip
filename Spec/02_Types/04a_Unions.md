# Part II: Type System - §8.5. Union Types

**Section**: §8.5 | **Part**: Type System (Part II)
**Previous**: [Records](04_Records.md) | **Next**: [Enums](05_Enums.md)

---

**Definition 8.5.1 (Union):** A union is an untagged sum type where all variants occupy the same memory location. Formally, if `union U { f₁: T₁, ..., fₙ: Tₙ }` then `⟦U⟧ = T₁ ⊔ ... ⊔ Tₙ` where ⊔ denotes overlapping union (not disjoint sum), with `size(U) = max(size(T₁), ..., size(Tₙ))` and no runtime discriminant. The active variant is tracked by the programmer, making unions inherently unsafe.

## 8.5 Union Types

### 8.5.1 Overview

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

### 8.5.2 Syntax

#### 8.5.2.1 Concrete Syntax

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

#### 8.5.2.2 Abstract Syntax

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

#### 8.5.2.3 Basic Examples

**Simple union:**
```cantrip
union IntOrFloat {
    i: i32
    f: f32
}

let value = IntOrFloat { i: 42 }

// Accessing variant (unsafe)
procedure read_int(v: IntOrFloat): i32
    uses unsafe.union
{
    v.i
}
```

**C-compatible union:**
```cantrip
[[repr(C)]]
union Data {
    integer: i32
    floating: f64
    bytes: [u8; 8]
}

let d = Data { floating: 3.14 }
```

### 8.5.3 Static Semantics

#### 8.5.3.1 Well-Formedness Rules

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

#### 8.5.3.2 Type Rules

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

#### 8.5.3.3 Type Properties

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

### 8.5.4 Dynamic Semantics

#### 8.5.4.1 Evaluation Rules

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

#### 8.5.4.2 Memory Representation

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

#### 8.5.4.3 Operational Behavior

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

### 8.5.5 Safety and Integration

#### 8.5.5.1 Effect System Integration

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

#### 8.5.5.2 Contract-Based Safety

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

#### 8.5.5.3 Permission System Integration

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

#### 8.5.5.4 Region Allocation Support

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

### 8.5.6 Advanced Features

#### 8.5.6.1 FFI Integration

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

#### 8.5.6.2 Hardware Register Access

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

#### 8.5.6.3 Type Punning and Bit Manipulation

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

### 8.5.7 Examples and Patterns

#### 8.5.7.1 Pattern 1: Safe Tagged Union (Recommended)

**Problem:** Need variant types with type safety.

**Solution:** Wrap union in record with explicit tag field.

```cantrip
enum ValueTag { Int, Float }

union ValueData {
    i: i32
    f: f32
}

record SafeValue {
    tag: ValueTag
    data: ValueData

    procedure new_int(value: i32): own SafeValue {
        own SafeValue { tag: ValueTag.Int, data: ValueData { i: value } }
    }

    procedure as_int(self: Self): Option<i32>
        uses unsafe.union
    {
        match self.tag {
            ValueTag.Int => Option.Some(self.data.i),
            ValueTag.Float => Option.None,
        }
    }
}

// Usage: Tag ensures type-safe access
let val = SafeValue.new_int(42)
match val.as_int() {
    Option.Some(n) => println("Value: {}", n),
    Option.None => println("Not an int"),
}
```

#### 8.5.7.2 Pattern 2: FFI Compatibility

**Problem:** Need to call C API with union parameters.

**Solution:** Define union with `[[repr(C)]]` matching C layout.

```cantrip
// C: typedef struct { int type; union { int i; float f; } value; } Variant;

[[repr(C)]]
record Variant {
    type_id: i32
    value: VariantValue
}

[[repr(C)]]
union VariantValue {
    i: i32
    f: f32
}

extern "C" {
    procedure process_variant(v: Variant): i32
        uses ffi.call, unsafe.union
}

procedure call_c_api(value: i32): i32
    uses ffi.call, unsafe.union
{
    let v = Variant { type_id: 1, value: VariantValue { i: value } }
    process_variant(v)
}
```

#### 8.5.7.3 Pattern 3: Space Optimization

**Problem:** Need to store one of several types, minimize memory.

**Solution:** Use union when only one variant is active at a time.

```cantrip
// Instead of:
// enum Message {
//     Text(String),       // 24 bytes + discriminant
//     Number(i64),        // 8 bytes + discriminant
//     Binary(Vec<u8>),    // 24 bytes + discriminant
// }
// Size: ~32 bytes

// Space-optimized version:
enum MessageType {
    Text,
    Number,
    Binary,
}

union MessageData {
    text: String        // 24 bytes
    number: i64         // 8 bytes
    binary: Vec<u8>     // 24 bytes
}

record Message {
    type_tag: MessageType  // 1 byte
    data: MessageData      // 24 bytes
}
// Size: ~32 bytes (similar, but more control)

// Use when you have many instances and know the active type through
// external invariants (e.g., protocol headers, state machines).
```

### 8.5.8 Comparison to Alternatives

| Feature | Union | Enum | Record | Type Alias |
|---------|-------|------|--------|------------|
| **Memory layout** | Overlapping | Tagged + largest variant | Sequential | Transparent |
| **Discriminant** | None | Automatic | N/A | N/A |
| **Type safety** | Unsafe | Safe | Safe | Transparent |
| **C FFI** | Perfect match | Tag overhead | Perfect match | Depends |
| **Pattern matching** | Limited | Full support | Full support | Depends |
| **Space efficiency** | max(variants) | discriminant + max(variants) | sum(fields) | Underlying type |
| **Use case** | C interop, hardware | General variants | Structured data | Type branding |

**When to use each:**

- **Union**: C FFI, hardware registers, space-critical optimization with external tags
- **Enum**: General Cantrip code requiring variant types
- **Record**: Structured data with all fields present
- **Type Alias**: Simple naming, no runtime representation

### 8.5.9 Safety Guidelines

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

**Previous**: [Records](04_Records.md) | **Next**: [Enums](05_Enums.md)
