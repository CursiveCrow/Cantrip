# Part III: Type System
## Section 3.1: Subtyping and Equivalence

**File**: `03-1_Subtyping-Equivalence.md`
**Version**: 2.0
**Status**: Normative
**Previous**: [03-0_Type-Foundations.md](03-0_Type-Foundations.md) | **Next**: [03-2_Primitive-Types.md](03-2_Primitive-Types.md)

---

## 3.1.1 Overview

[1] This section specifies the subtyping relation and type equivalence rules for the Cursive type system.

[2] The subtyping relation determines when one type can safely substitute for another, while type equivalence determines when two type expressions denote the same type.

## 3.1.2 Syntax

[3] The subtyping relation shall be denoted `τ <: υ` (read "τ is a subtype of υ").

[4] Type equivalence shall be denoted `τ ≡ υ` (read "τ is equivalent to υ").

**Syntax:**
```
subtyping_judgment
    : type_expression '<:' type_expression
    ;

equivalence_judgment
    : type_expression '≡' type_expression
    ;
```

---

## 3.1.3 Type Equivalence

[5] Type equivalence shall be an equivalence relation (reflexive, symmetric, and transitive).

**Definition 3.1.1** (*Type Equivalence*):
Types τ₁ and τ₂ are equivalent (`τ₁ ≡ τ₂`) when they denote the same type after expanding aliases, substituting generic arguments, and normalizing associated types.

[6] The basic equivalence rules shall be:

**Equivalence Rules:**
```
[Equiv-Refl]
------------
τ ≡ τ


[Equiv-Sym]
τ₁ ≡ τ₂
------------
τ₂ ≡ τ₁


[Equiv-Trans]
τ₁ ≡ τ₂    τ₂ ≡ τ₃
--------------------
τ₁ ≡ τ₃
```

### 3.1.3.1 Type Alias Equivalence

[7] Type aliases shall be transparent. An alias is equivalent to its definition.

**Aliasing Rules:**
```
[Equiv-Alias]
type A = τ
----------
A ≡ τ


[Equiv-Generic-Alias]
type F⟨α⟩ = τ[α]
----------------------
F⟨υ⟩ ≡ τ[α ↦ υ]
```

**Example 1**: Type alias transparency ✅
```cursive
type Coordinate = Point
let p: const Point = Point { x: 1.0, y: 2.0 }
let c: const Coordinate = p  // OK: Coordinate ≡ Point
```

**Example 2**: Generic type alias ✅
```cursive
type Pair<T> = (T, T)
let p1: const Pair<i32> = (1, 2)
let p2: const (i32, i32) = p1  // OK: Pair<i32> ≡ (i32, i32)
```

### 3.1.3.2 Nominal vs Structural Equivalence

[8] Records, enums, modals, and predicates shall use nominal equivalence. Two types with identical structure but different names are not equivalent.

**Definition 3.1.2** (*Nominal Equivalence*):
Two nominal types are equivalent if and only if they have the same name and originate from the same type declaration.

**Example 3**: Nominal types are not structurally equivalent ❌
```cursive
record Point { x: f64, y: f64 }
record Vector { x: f64, y: f64 }
// Point ≢ Vector despite identical structure

let p: const Point = Point { x: 1.0, y: 2.0 }
// let v: Vector = p  // ERROR: Point ≢ Vector (nominal types)
```

[9] Tuples, arrays, slices, primitives, and function types shall use structural equivalence.

**Definition 3.1.3** (*Structural Equivalence*):
Two structural types are equivalent if and only if they have identical structure.

**Example 4**: Structural type equivalence ✅
```cursive
let t1: const (i32, f64) = (42, 3.14)
let t2: const (i32, f64) = (100, 2.71)
// t1 and t2 have the same type: (i32, f64) ≡ (i32, f64)
```

### 3.1.3.3 Structural Equivalence Rules

[10] Structural equivalence rules shall apply to composite types as follows:

**Structural Equivalence Rules:**
```
[Equiv-Tuple]
τ₁ ≡ υ₁    ...    τₙ ≡ υₙ
-----------------------------
(τ₁, ..., τₙ) ≡ (υ₁, ..., υₙ)


[Equiv-Array]
τ ≡ υ    n = m
--------------------
[τ; n] ≡ [υ; m]


[Equiv-Slice]
τ ≡ υ
-----------
[τ] ≡ [υ]


[Equiv-Function]
τ₁ ≡ υ₁    ...    τₙ ≡ υₙ
τ_ret ≡ υ_ret
ε₁ = ε₂    (grant sets equal)
---------------------------------------------------------
(τ₁, ..., τₙ) -> τ_ret ! ε₁ ≡ (υ₁, ..., υₙ) -> υ_ret ! ε₂
```

---

## 3.1.4 Subtyping Rules

[11] The subtyping relation shall be a preorder (reflexive and transitive).

**Definition 3.1.4** (*Subtyping Relation*):
The subtyping relation `τ <: υ` (read "τ is a subtype of υ") indicates that a value of type τ can be used safely wherever a value of type υ is expected.

[12] No implicit coercions beyond subtyping shall be permitted.

### 3.1.4.1 Basic Subtyping Rules

[13] The basic subtyping axioms shall be:

**Subtyping Rules:**
```
[Sub-Refl]
----------
τ <: τ


[Sub-Trans]
τ₁ <: τ₂    τ₂ <: τ₃
--------------------
τ₁ <: τ₃


[Sub-Never]
----------
! <: τ
```

[14] The never type `!` shall be the bottom type, a subtype of all types.

**Example 5**: Never type subtyping ✅
```cursive
function diverge(): ! {
    loop {
        // Infinite loop, never returns
    }
}

function example_never(): i32 {
    diverge()  // OK: ! <: i32
    // This code is unreachable but well-typed
}
```

### 3.1.4.2 Array and Slice Subtyping

[15] Fixed-size arrays shall coerce to slices of the same element type.

**Array-Slice Subtyping:**
```
[Sub-Array-Slice]
-----------------
[τ; n] <: [τ]
```

**Example 6**: Array to slice subtyping ✅
```cursive
let array: const [i32; 5] = [1, 2, 3, 4, 5]
let slice: const [i32] = array[..]  // OK: [i32; 5] <: [i32]
```

**Example 7**: Using array where slice expected ✅
```cursive
function takes_slice(data: const [i32]) {
    loop item in data {
        // Process item
    }
}

let fixed: const [i32; 5] = [1, 2, 3, 4, 5]
takes_slice(fixed[..])  // OK: [i32; 5] <: [i32]
```

### 3.1.4.3 Function Subtyping

[16] Function types shall be contravariant in parameters, covariant in return types, and covariant in grant sets.

**Definition 3.1.5** (*Function Subtyping*):
Function types exhibit the following variance properties:
- **Contravariant parameters**: A function accepting more general parameters is a subtype
- **Covariant return**: A function returning more specific results is a subtype
- **Covariant grants**: A function requiring fewer grants is a subtype (more specific)

**Function Subtyping Rule:**
```
[Sub-Function]
υ₁ <: τ₁    ...    υₙ <: τₙ    (contravariant parameters)
τ_ret <: υ_ret                  (covariant return)
ε₁ ⊆ ε₂                         (covariant grants)
-------------------------------------------------------
(τ₁, ..., τₙ) -> τ_ret ! ε₁  <:  (υ₁, ..., υₙ) -> υ_ret ! ε₂
```

**Example 8**: Function grant subtyping ✅
```cursive
function pure_compute(x: i32): i32 {
    result x * 2
}

procedure io_compute(x: i32): i32
    sequent { [fs::write] |- true => true }
{
    println("Computing {}", x)
    result x * 2
}

// Type of pure_compute: (i32) -> i32 ! ∅
// Type of io_compute: (i32) -> i32 ! {fs::write}
//
// Subtyping: (i32) -> i32 ! ∅  <:  (i32) -> i32 ! {fs::write}
// Pure function (no grants) is subtype of function requiring grants
// (fewer grants = more specific)
```

---

## 3.1.5 Variance

[17] Variance shall describe how subtyping of type parameters relates to subtyping of type constructors.

**Definition 3.1.6** (*Variance*):
A type constructor `F` is:
- **Covariant** in parameter `T` if `τ <: υ` implies `F⟨τ⟩ <: F⟨υ⟩`
- **Contravariant** in parameter `T` if `τ <: υ` implies `F⟨υ⟩ <: F⟨τ⟩`
- **Invariant** in parameter `T` if neither covariance nor contravariance holds

### 3.1.5.1 Variance Table

[18] The following type constructors shall have the specified variance:

**Table 3.1.1**: Type Constructor Variance

| Type Constructor | Variance | Rationale |
|-----------------|----------|-----------|
| `[τ; n]` | Invariant in τ | Mutation requires exact type matching |
| `[τ]` | Invariant in τ | Mutation through slices requires type preservation |
| `(τ₁, ..., τₙ)` | Invariant in all τᵢ | Structural equality requires exact matching |
| Records | Invariant in all fields | Nominal types, no structural subtyping |
| Enums | Invariant in all variants | Nominal types, discriminant safety |
| Modals | Invariant in all states | State machine integrity |
| Function parameters | Contravariant | Parameter positions flip variance |
| Function returns | Covariant | Return positions preserve variance |
| Grants in functions | Covariant | Fewer grants = more specific |
| `Ptr⟨τ⟩@State` | Covariant in τ, Invariant in State | Safe pointer semantics |

> **Note**: Reference bindings (`let x: T <- value`) follow invariance for type safety but do not introduce new type constructors.

### 3.1.5.2 Variance Checking

[19] For a generic type `F⟨T⟩`, variance shall be computed as follows:

**Algorithm 3.1.1** (*Variance Computation*):
```
1. Examine all occurrences of T in the definition of F
2. For each occurrence:
   - In covariant position: contributes covariance
   - In contravariant position: contributes contravariance
   - In invariant position: forces invariance
3. Combine contributions:
   - All covariant → covariant
   - All contravariant → contravariant
   - Mixed or any invariant → invariant
```

**Example 9**: Variance in generic types
```cursive
// Function parameter contravariance
type Processor<T> = (T) -> ()

// If we had subtyping on T, then:
// Processor<SuperType> <: Processor<SubType>
// (contravariant in parameter position)

// Arrays are invariant
let int_array: const [i32; 3] = [1, 2, 3]
// Cannot treat as [i64; 3] even conceptually
```

---

## 3.1.6 Type Compatibility

[20] Type τ shall be compatible with type υ in context C if either:
    (20.1) `τ <: υ` (subtyping), or
    (20.2) An explicit conversion from τ to υ is allowed in context C

**Definition 3.1.7** (*Type Compatibility*):
Type τ is compatible with type υ in context C if the conditions in paragraph [20] are satisfied.

[21] Cursive shall not perform implicit numeric promotions. All numeric conversions shall be explicit.

### 3.1.6.1 Compatibility Contexts

[22] In assignment context `x = e`, the type of `e` shall be a subtype of the declared type of `x`.

[23] In function call context `f(e₁, ..., eₙ)`, each `eᵢ` shall have type that is a subtype of the corresponding parameter type.

[24] In return context `result e`, the type of `e` shall be a subtype of the function's return type.

### 3.1.6.2 Explicit Conversions

[25] Type conversions shall use the `as` operator for explicit casting.

**Example 10**: Explicit numeric conversion required ✅
```cursive
let x32: const i32 = 42
let x64: const i64 = x32 as i64  // Explicit cast required
```

**Example 11**: Implicit conversion not allowed ❌
```cursive
let x: const i32 = 42
// let z: i64 = x  // ERROR: no implicit conversion
```

> **Forward reference**: Complete conversion semantics in §5.8 (Type Conversions and Coercions).

---

## 3.1.7 Integration

[26] The subtyping and equivalence rules integrate with the following language components:

**Cross-references:**
- Type formation: §3.0.2
- Type environments: §3.0.3
- Type conversions and coercions: §5.8
- Grant system: Part X (Grant System)
- Function types: §3.5
- Generic type constraints: Part VIII (Predicates and Type Constraints)
- Permission system interactions: Part IV §4.2

[27] The variance rules specified in paragraph [18] shall apply during generic type checking in Part IX (Generics and Parametric Polymorphism).

---

**Previous**: [03-0_Type-Foundations.md](03-0_Type-Foundations.md) | **Next**: [03-2_Primitive-Types.md](03-2_Primitive-Types.md)
