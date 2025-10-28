# Part II: Type System - §0. Type System Foundations

**Section**: §0 | **Part**: Type System (Part II)
**Next**: [Primitive Types](PART_A_Primitives/01_PrimitiveTypes.md)

---

**Definition 0.1 (Type System):** The Cantrip type system is a static, nominally-typed system with parametric polymorphism, subtyping, and dependent state types. Formally, the type system consists of:
- A set of types **T**
- Typing judgments **Γ ⊢ e : τ**
- A subtyping relation **<:**
- Well-formedness rules **Γ ⊢ τ : Type**

The type system will memory safety, prevents undefined behavior, and enables zero-cost abstractions through compile-time verification and monomorphization.

## 0. Type System Foundations


**Note:** The type system specification has been reorganized into five parts (A-E) for better navigation:
- **Part A**: Primitive Types (§2.1)
- **Part B**: Composite Types (§2.2-§2.4)
- **Part C**: Reference Types (§2.5)
- **Part D**: Abstraction & Polymorphism (§2.6-§2.8)
- **Part E**: Type Utilities (§2.9-§2.10)

See the organizational structure below for complete details.

---

### 0.1 Overview and Philosophy

**Key innovations/purpose:** Cantrip's type system provides a rigorous foundation for safe systems programming by combining:
- **Nominal typing** for user-defined types with **structural typing** for built-in composite types
- **Zero-cost abstractions** through monomorphization (no runtime type information)
- **State machines as first-class types** (modals) for lifecycle and protocol verification
- **Deep integration** with permission system (Part III), effect system (Part IV), and contracts (Part V)

**Design principles:**

1. **Type Safety** — Well-typed programs do not exhibit undefined behavior
   - No null pointer dereferences
   - No buffer overflows
   - No use-after-free
   - No data races (via permission system)

2. **Expressiveness** — Rich type system for encoding invariants
   - State-dependent types (modals)
   - Refinement types (contracts)
   - Effect tracking (purity, I/O, allocation)

3. **Performance** — No runtime overhead for type checking
   - All types resolved at compile time
   - Monomorphization generates specialized code
   - Zero-cost abstractions through static dispatch

4. **Composability** — Types combine through well-defined operations
   - Product types (tuples, records)
   - Sum types (enums, modals)
   - Trait composition (multiple trait bounds)

**Relationship to other features:**
- **Permissions (Part III):** Every type has an associated permission (own, mut, imm)
- **Effects (Part IV):** Functions are typed by their effect signatures
- **Contracts (Part V):** Types can have invariants and pre/post-conditions
- **Regions (Part VI):** Memory regions provide lifetime safety guarantees

### 0.2 Type Taxonomy

Complete classification of all types in the Cantrip type system:

```
Cantrip Type System (T)
│
├── Value Types (τ)
│   │
│   ├── Primitive Types (§2.1)
│   │   ├── Integer Types
│   │   │   ├── Signed: i8, i16, i32, i64, isize
│   │   │   └── Unsigned: u8, u16, u32, u64, usize
│   │   ├── Floating-Point Types: f32, f64
│   │   ├── Boolean Type: bool
│   │   ├── Character Type: char (Unicode scalar value)
│   │   ├── Never Type: ! (bottom type, uninhabited)
│   │   └── String Types ✅ Specified
│   │       ├── String Slice: str (borrowed, unsized)
│   │       └── Owned String: String (heap-allocated, growable)
│   │
│   ├── Compound Types
│   │   │
│   │   ├── Product Types (Cartesian product)
│   │   │   ├── Tuples: (τ₁, ..., τₙ) (§2.2.1)
│   │   │   │   └── Unit Type: () (0-tuple, zero-sized)
│   │   │   └── Records: record S { f₁: τ₁; ...; fₙ: τₙ } (§2.2.2)
│   │   │       ├── Named records (struct-like)
│   │   │       └── Tuple structs [TO BE SPECIFIED]
│   │   │
│   │   ├── Sum Types (Disjoint union)
│   │   │   ├── Enums: enum E { V₁(τ₁), ..., Vₙ(τₙ) } (§2.3.1 or §2.5)
│   │   │   │   ├── Unit variants
│   │   │   │   ├── Tuple variants
│   │   │   │   └── Struct variants
│   │   │   └── Modals: modal M { @S₁, ..., @Sₙ } (§2.3.3 or §2.10)
│   │   │       └── State-indexed types with transition functions
│   │   │
│   │   └── Collection Types
│   │       ├── Arrays: [τ; n] (fixed-size, stack-allocated) (§2.4 or §2.6)
│   │       └── Slices: [τ] (dynamically-sized view, fat pointer) (§2.4 or §2.6)
│   │           ├── Immutable slices: [τ] (Copy)
│   │           └── Mutable slices: [mut τ] [TO BE SPECIFIED]
│   │
│   └── Reference Types (§2.3.1 or §2.5)
│       ├── Safe Pointers
│       │   └── Ptr<τ>@State (modal heap pointers with state-based aliasing safety)
│       └── Raw Pointers
│           ├── *τ (immutable raw pointer)
│           └── *mut τ (mutable raw pointer)
│
├── Parametric Types
│   ├── Generic Types: T<α₁, ..., αₙ> (§2.7 or §2.9)
│   │   ├── Type parameters (α : Type)
│   │   ├── Const generic parameters: const N: usize (§11.5)
│   │   └── Lifetime parameters: 'a [TO BE SPECIFIED]
│   │
│   ├── Type Aliases [TO BE SPECIFIED]
│   │   ├── Transparent aliases: type Name = τ
│   │   └── Newtype pattern: record Name(τ);
│   │
│   └── Associated Types
│       └── trait I { type A; } (§10.5)
│
├── Abstraction Types
│   ├── Trait Types (§2.8)
│   │   ├── Trait definitions: trait I { ... }
│   │   ├── Trait bounds: T: I₁ + I₂ + ... + Iₙ
│   │   └── Supertrait bounds: trait I: J { ... }
│   │
│   └── Function Types [TO BE SPECIFIED]
│       ├── Function pointers: fn(τ₁, ..., τₙ) → τ
│       └── Closures: |τ₁, ..., τₙ| → τ
│
└── Type Constructors (Kind: Type → Type)
    ├── Generic type constructors: T<_> (§2.7 or §2.9)
    ├── Array constructor: [_; n]
    ├── Slice constructor: [_]
    └── Tuple constructor: (_, _, ...)
```

**Type classification:**
- **Sized types:** Types with compile-time known size (most types)
- **Unsized types (DSTs):** Dynamically-sized types: [T], str
- **Zero-sized types (ZSTs):** Types with size 0: (), !, unit-only enums
- **Inhabited types:** Types with at least one value
- **Uninhabited types:** Types with no values: !

### 0.3 Formal Foundations

#### 0.3.1 Syntax of Types

**Abstract syntax of types:**

```
τ, σ, ρ ::=                                         (types)
    | i8 | i16 | i32 | i64 | isize                 (signed integers)
    | u8 | u16 | u32 | u64 | usize                 (unsigned integers)
    | f32 | f64                                     (floating-point)
    | bool                                          (boolean)
    | char                                          (Unicode scalar)
    | str                                           (string slice)
    | String                                        (owned string)
    | !                                            (never type, ⊥)
    | ()                                           (unit type)
    | (τ₁, ..., τₙ)                                (tuple, n ≥ 2)
    | [τ; n]                                       (array)
    | [τ]                                          (slice)
    | S                                            (record type name)
    | E                                            (enum type name)
    | M@s                                          (modal in state s)
    | T<τ₁, ..., τₙ>                              (generic instantiation)
    | T<const n>                                   (const generic)
    | ∀α₁...αₙ. τ                                  (polymorphic type)
    | τ₁ → τ₂                                      (function type)
    | *τ                                           (raw pointer)
    | *mut τ                                       (mutable raw pointer)
    | α                                            (type variable)

B ::= I | I₁ + I₂ + ... + Iₙ                      (trait bounds)
```

**Kinding judgments:**

```
K ::= Type | Type → Type | Type → Type → Type | ...   (kinds)

Primitive types have kind Type
Generic types have higher kinds:
  Vec     : Type → Type
  HashMap : Type → Type → Type
  [_; n]  : Type → Type
```

#### 0.3.2 Type Judgments

**Definition 0.2 (Typing Context):** A typing context Γ is a finite mapping from variables to types and type variables to kinds:

```
Γ ::= ∅                    (empty context)
    | Γ, x: τ              (term variable binding)
    | Γ, α: K              (type variable binding)
    | Γ, α: B              (type variable with bound)
```

**Definition 0.3 (Typing Judgment):** The judgment `Γ ⊢ e : τ` means "in context Γ, expression e has type τ".

**Core judgment forms:**

```
Γ ⊢ e : τ                  Expression e has type τ
Γ ⊢ τ : Type               Type τ is well-formed
Γ ⊢ τ₁ <: τ₂               Type τ₁ is a subtype of τ₂
Γ ⊢ τ₁ ≡ τ₂                Types τ₁ and τ₂ are equivalent
Γ ⊢ p : τ ⇝ Γ'             Pattern p matches type τ, producing context Γ'
```

**Auxiliary judgments:**

```
Γ ⊢ σ : K                  Type σ has kind K
Γ ⊢ τ : B                  Type τ satisfies bound B
⟨e, σ⟩ ⇓ ⟨v, σ'⟩           Expression e evaluates to value v
```

#### 0.3.3 Subtyping Relation

**Definition 0.4 (Subtyping):** The subtyping relation `τ₁ <: τ₂` (read "τ₁ is a subtype of τ₂") indicates that a value of type τ₁ can be safely used in any context expecting type τ₂.

**Subtyping axioms:**

```
[Sub-Refl]  (Reflexivity)
──────────
τ <: τ

[Sub-Trans]  (Transitivity)
τ₁ <: τ₂    τ₂ <: τ₃
────────────────────
τ₁ <: τ₃

[Sub-Never]  (Never is bottom)
──────────
! <: τ

[Sub-Array]  (Array to slice coercion)
─────────────
[T; n] <: [T]
```

**Invariance principles:**

Most Cantrip compound types are **invariant** in their type parameters to ensure safety:

```
[Inv-Array]
[τ₁; n] ≮: [τ₂; n]    even if τ₁ <: τ₂    (arrays invariant)

[Inv-Slice]
[τ₁] ≮: [τ₂]          even if τ₁ <: τ₂    (slices invariant)

[Inv-Tuple]
(τ₁, τ₂) ≮: (σ₁, σ₂)  even if τ₁ <: σ₁, τ₂ <: σ₂    (tuples invariant)

[Inv-Generic]
T<τ₁> ≮: T<τ₂>        even if τ₁ <: τ₂    (generics invariant by default)
```

**Rationale for invariance:**

Covariance would be unsound in the presence of mutation:

```cantrip
// If arrays were covariant (UNSOUND):
let arr: [mut Animal; 3] = [...];
let cats: [Cat; 3] <: [Animal; 3];  // Suppose this were valid
arr = cats;  // Now arr points to cats
arr[0] = Dog { };  // Write Dog to array of Cats! UNSOUND!
```

**Function subtyping (contravariance in arguments):**

```
[Sub-Fun]  (Functions contravariant in arguments, covariant in return)
σ₁ <: τ₁    τ₂ <: σ₂
────────────────────
(τ₁ → τ₂) <: (σ₁ → σ₂)
```

#### 0.3.4 Type Equivalence

**Definition 0.5 (Type Equivalence):** Two types are equivalent (τ₁ ≡ τ₂) if they denote the same set of values. Type equivalence is computed after:
1. Type alias expansion
2. Generic type parameter substitution
3. Normalization of associated types

**Equivalence axioms:**

```
[Equiv-Refl]
────────
τ ≡ τ

[Equiv-Sym]
τ₁ ≡ τ₂
────────
τ₂ ≡ τ₁

[Equiv-Trans]
τ₁ ≡ τ₂    τ₂ ≡ τ₃
──────────────────
τ₁ ≡ τ₃
```

**Alias equivalence:**

```
[Equiv-Alias]
type A = τ
──────────
A ≡ τ

[Equiv-Generic-Alias]
type F<α> = τ[α]
──────────────────
F<σ> ≡ τ[α ↦ σ]
```

**Nominal vs. Structural Equivalence:**

**Nominal types** (records, enums, modals): Distinct by name even with identical structure

```
record Point { x: f64; y: f64 }
record Vector { x: f64; y: f64 }

Point ≢ Vector    (different names ⟹ different types)
```

**Structural types** (tuples, arrays, primitives): Equivalent if structurally identical

```
(i32, bool) ≡ (i32, bool)        (same structure ⟹ same type)
[u8; 10] ≡ [u8; 10]              (same element type and size)
```

**Justification:** Nominal typing for user types enables:
- Stronger abstraction boundaries
- Different trait implementations for identical structures
- Newtype pattern for type safety

#### 0.3.5 Well-Formedness Rules

**Definition 0.6 (Well-Formed Type):** A type τ is well-formed in context Γ (written `Γ ⊢ τ : Type`) if:
1. All type names are in scope
2. All type variables are bound
3. Generic types are applied to the correct number of arguments
4. All trait bounds are satisfied

**Well-formedness rules:**

```
[WF-Var]
(α : Type) ∈ Γ
──────────────
Γ ⊢ α : Type

[WF-Prim]
────────────────
Γ ⊢ i32 : Type

(and similarly for all primitive types)

[WF-Unit]
────────────────
Γ ⊢ () : Type

[WF-Never]
────────────────
Γ ⊢ ! : Type

[WF-Tuple]
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
──────────────────────────────────────
Γ ⊢ (τ₁, ..., τₙ) : Type

[WF-Array]
Γ ⊢ τ : Type    n > 0
──────────────────────
Γ ⊢ [τ; n] : Type

[WF-Slice]
Γ ⊢ τ : Type
──────────────
Γ ⊢ [τ] : Type

[WF-Record]
(record S { f₁: σ₁; ...; fₙ: σₙ }) ∈ Γ
──────────────────────────────────────
Γ ⊢ S : Type

[WF-Generic]
Γ ⊢ T : Type^n → Type    (T takes n type parameters)
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
────────────────────────────────────────
Γ ⊢ T<τ₁, ..., τₙ> : Type

[WF-Bounded]
Γ ⊢ τ : Type    Γ ⊢ τ : B₁    ...    Γ ⊢ τ : Bₙ
────────────────────────────────────────────────
Γ ⊢ τ : Type    where τ: B₁ + ... + Bₙ

[WF-Const-Generic]
Γ ⊢ T : Type → Type
n : usize    (compile-time constant)
────────────────────────────────────
Γ ⊢ T<const n> : Type

[WF-Function]
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type    Γ ⊢ τ : Type
─────────────────────────────────────────────────────
Γ ⊢ fn(τ₁, ..., τₙ) → τ : Type

[WF-Pointer]
Γ ⊢ τ : Type
──────────────────
Γ ⊢ *τ : Type
Γ ⊢ *mut τ : Type
```

**Recursive type well-formedness:**

Recursive types must be well-founded (positive occurrences only):

```
[WF-Recursive]
record List<T> {
    head: T,
    tail: Ptr<List<T>>@Exclusive,  // OK: positive occurrence through heap pointer
}

// INVALID:
record Bad<T> {
    field: fn(Bad<T>) → T,  // ERROR: negative occurrence (function argument)
}
```

#### 0.3.6 Type Checking Algorithm

**Definition 0.7 (Type Checking):** Type checking verifies that an expression e has a claimed type τ in context Γ.

**Algorithm: `check(Γ, e, τ) → Result<(), TypeError>`**

```
Input: Context Γ, expression e, expected type τ
Output: Ok(()) if e has type τ, or Err(error)

Steps:
1. Synthesize actual type τ' from e: τ' = synth(Γ, e)
2. Check subtyping: τ' <: τ
3. If yes, return Ok(()); otherwise return Err(TypeError)
```

**Type synthesis: `synth(Γ, e) → τ`**

```
synth(Γ, x) = Γ(x)                           (variable lookup)
synth(Γ, n) = i32                             (integer literal, default)
synth(Γ, true) = bool                         (boolean true)
synth(Γ, false) = bool                        (boolean false)
synth(Γ, 'c') = char                          (character literal)
synth(Γ, ()) = ()                             (unit value)

synth(Γ, (e₁, ..., eₙ)) = (τ₁, ..., τₙ)     (tuple)
  where τᵢ = synth(Γ, eᵢ)

synth(Γ, [e₁, ..., eₙ]) = [τ; n]            (array)
  where τ = unify(synth(Γ, e₁), ..., synth(Γ, eₙ))

synth(Γ, S { f₁: e₁, ..., fₙ: eₙ }) = S     (record)
  where Γ ⊢ check(eᵢ, τᵢ) for each field fᵢ: τᵢ in S

synth(Γ, E.V(e)) = E                          (enum variant)
  where Γ ⊢ check(e, τ) and variant V(τ) in E

synth(Γ, e.f) = τ                             (field access)
  where synth(Γ, e) = S and field f: τ in S

synth(Γ, e[i]) = τ                            (array/slice indexing)
  where synth(Γ, e) = [τ; _] or [τ]

synth(Γ, f(e₁, ..., eₙ)) = τ                 (function call)
  where synth(Γ, f) = (σ₁, ..., σₙ) → τ
  and Γ ⊢ check(eᵢ, σᵢ) for all i
```

**Type checking with expected type: `check(Γ, e, τ) → Result<(), Error>`**

```
check(Γ, e, τ):
  τ' = synth(Γ, e)
  if τ' <: τ:
    return Ok(())
  else:
    return Err(TypeError { expected: τ, found: τ' })
```

#### 0.3.7 Type Inference

**Definition 0.8 (Type Inference):** Type inference determines the type τ of expression e when τ is not explicitly annotated, such that `Γ ⊢ e : τ`.

**Inference algorithm:**

Cantrip uses **bidirectional type checking** with **constraint generation and solving**:

```
1. Synthesis phase: Generate type τ from e
2. Constraint generation: Collect unification constraints
3. Constraint solving: Solve for type variables
4. Generalization: Abstract over remaining type variables
```

**Inference rules:**

```
[Infer-Let]
Γ ⊢ e : τ    τ = gen(τ')    (generalize to polytype)
─────────────────────────────
Γ ⊢ (let x = e) ⇝ Γ, x: τ

[Infer-Context]
Γ ⊢ context must type τ
Γ ⊢ e : τ
─────────────────────────────
Γ ⊢ e : τ    (contextual inference)

[Infer-Literal-Default]
n is integer literal without suffix
────────────────────────────────────
Γ ⊢ n : i32    (default type)

[Infer-Literal-Suffix]
n is integer literal with suffix T
────────────────────────────────────
Γ ⊢ n : T    (explicit type from suffix)

[Infer-Literal-Context]
Γ ⊢ context must integer type T
Γ ⊢ n : T    (infer from context)
```

**Type inference constraints:**

```
C ::= τ₁ = τ₂                  (equality constraint)
    | τ₁ <: τ₂                 (subtyping constraint)
    | τ : B                    (trait bound constraint)
    | ∃α. C                    (existential constraint)
    | C₁ ∧ C₂                  (conjunction)
```

**Constraint solving algorithm:**

```
solve(∅) = ∅                                    (empty constraint set)
solve({τ = τ} ∪ C) = solve(C)                  (reflexivity)
solve({α = τ} ∪ C) = [α ↦ τ] ∪ solve(C[α ↦ τ]) (substitution)
solve({T<τ₁> = T<τ₂>} ∪ C) = solve({τ₁ = τ₂} ∪ C) (decomposition)
solve({τ : B} ∪ C) = check_bound(τ, B), solve(C)  (trait bound checking)
```

**Occurs check:** Prevent infinite types

```
If α occurs in τ (α ≠ τ), then {α = τ} has no solution.
```

**Example inference:**

```cantrip
let numbers = [1, 2, 3];  // Infer type [i32; 3]

Steps:
1. synth([1, 2, 3]) generates constraints:
   - τ₁ = type(1) = i32    (default literal type)
   - τ₂ = type(2) = i32
   - τ₃ = type(3) = i32
   - element_type = unify(τ₁, τ₂, τ₃) = i32
   - array_type = [i32; 3]
2. Result: numbers : [i32; 3]
```

### 0.4 Type System Properties

**Theorem 0.1 (Type Safety):** If `∅ ⊢ e : τ` and `⟨e, σ⟩ ⇓ ⟨v, σ'⟩`, then `∅ ⊢ v : τ`.

*Informal proof:* By structural induction on the derivation of `⟨e, σ⟩ ⇓ ⟨v, σ'⟩`, using the Progress and Preservation lemmas.

**Theorem 0.2 (Progress):** If `∅ ⊢ e : τ`, then either:
1. e is a value, or
2. There exists e', σ' such that `⟨e, σ⟩ → ⟨e', σ'⟩`

*Informal proof:* Well-typed expressions do not get stuck. By induction on the typing derivation, every non-value expression has a reduction rule.

**Theorem 0.3 (Preservation):** If `Γ ⊢ e : τ` and `⟨e, σ⟩ → ⟨e', σ'⟩`, then `Γ ⊢ e' : τ`.

*Informal proof:* Type is preserved during evaluation. By case analysis on reduction rules and inversion of typing derivations.

**Theorem 0.4 (Soundness):** The Cantrip type system is sound: well-typed programs do not exhibit undefined behavior.

*Informal proof:* Follows from Progress and Preservation. If `∅ ⊢ e : τ`, then evaluation either:
1. Terminates with a value v of type τ, or
2. Diverges (infinite loop, but no undefined behavior)

**Corollary 0.5 (Memory Safety):** Well-typed programs do not:
- Dereference null pointers (except in unsafe code)
- Access out-of-bounds array elements (runtime checks inserted)
- Use deallocated memory (prevented by permission system)
- Cause data races (prevented by permission system)

**Theorem 0.6 (Parametricity):** For polymorphic functions `f : ∀α. τ`, the behavior of f is uniform across all instantiations of α. The function cannot inspect or depend on the concrete type.

*Informal proof:* Generic functions are parametric—they cannot perform type-based branching or inspect type tags at runtime (no runtime type information in Cantrip).

**Theorem 0.7 (Decidability):** Type checking in Cantrip is decidable.

*Informal proof:* The type checking algorithm terminates for all inputs:
1. Type synthesis recursively descends expression structure (finite)
2. Constraint solving terminates (occurs check prevents infinite unification)
3. Trait resolution uses coherence rules (unique implementations)

**Theorem 0.8 (Principal Types):** Every well-typed expression has a principal type (most general type).

*Note:* This holds for the core type system. Extensions like higher-order trait bounds may affect principal types.

### 0.5 Integration with Other Systems

**Permissions (Part III):**

Types in Cantrip are always annotated with permissions, though the permission is often implicit:

```
τ ::= perm T    where perm ∈ {own, mut, imm}

own T     Owned, exclusive access (movable)
mut T     Mutable borrow (exclusive, not movable)
imm T     Immutable borrow (shareable, not movable)
```

Example:
```cantrip
let x: own String = String.new();   // Owned
var y: mut String = String.new();   // Mutable owned
let z: String = String.new();       // Immutable owned
```

**Parameter Passing Semantics:**

All types pass by permission (reference-like) by default, regardless of whether they implement the Copy trait:

```
procedure process(value: i32) { ... }      // Immutable permission
procedure modify(value: mut i32) { ... }   // Mutable permission
procedure consume(value: own i32) { ... }  // Owned permission

let x: own i32 = 42
process(x)         // Pass as immutable, x remains owned
modify(mut x)      // Pass as mutable, x remains owned
consume(move x)    // Transfer ownership
```

The Copy trait indicates a type CAN be copied explicitly, but does NOT change default passing behavior:

```
procedure double(x: i32): i32
    where i32: Copy  // Constraint on type capability
{
    let mut local = x.copy()  // Explicit copy via .copy() method
    local * 2
}
```

This uniform passing behavior eliminates the confusion of different defaults for different types—all types follow the same rules.

**Effects (Part IV):**

Functions are typed by both their parameter/return types and their effect signatures:

```
function f(x: τ): σ
    uses ε;              Effect declaration
    must P;           Precondition
    will Q;            Postcondition
```

Effect signature: `f : τ →{ε} σ`

**Contracts (Part V):**

Types can have invariants, and functions have pre/post-conditions:

```cantrip
record BoundedCounter {
    value: u32;
    max: u32;
    invariant self.value <= self.max;
}

function increment(self: mut BoundedCounter)
    must self.value < self.max;
    will self.value == old(self.value) + 1;
```

**Regions (Part VI):**

Memory regions provide lifetime safety. Types can be annotated with region parameters:

```
T @ region r
```

Regions ensure references do not outlive their referents.

**Modals (§2.3.3 or §2.10) and State:**

Modals extend the type system with state-indexed types:

```
modal File {
    state Closed { path: String }
    state Open { path: String; handle: FileHandle }
}

File@Closed vs. File@Open are distinct types
```

### 0.6 Copy Trait and Value Semantics

**Definition 0.9 (Copy Trait):** The Copy trait marks types that can be duplicated via bitwise copy without violating safety invariants:

```
trait Copy {
    procedure copy(self: Self): Self
        requires unsafe.bitcopy
}
```

**Semantics:**
- Copy types CAN be copied explicitly via `.copy()` method
- Copy does NOT change parameter passing behavior
- All parameters pass by permission regardless of Copy status
- Assignment creates permission bindings, not copies

**Auto-implementation:** The compiler automatically implements Copy for types where all fields are Copy:

```
[Copy-Primitive]
─────────────
i32 : Copy

[Copy-Slice]
─────────────
[T] : Copy    (fat pointer is always Copy, regardless of T)

[Copy-Tuple]
T₁ : Copy    ...    Tₙ : Copy
──────────────────────────────
(T₁, ..., Tₙ) : Copy

[Copy-Array]
T : Copy
────────────────
[T; n] : Copy

[Copy-Record]
record R { f₁: T₁; ...; fₙ: Tₙ }
T₁ : Copy ∧ ... ∧ Tₙ : Copy
────────────────────────────────
R : Copy
```

**Non-Copy types:**
- Heap-allocated types (String, Vec<T>, Ptr<T>@Exclusive)
- Types with custom drop logic
- Types containing non-Copy fields
- Types with interior mutability

**Example:**
```cantrip
// Copy-capable type
let x: i32 = 42
let y = x           // y binds to x's value, x still usable
let z = x.copy()    // Explicit copy creates new value

// Non-Copy type
let s: own String = String.new("hello")
let t = s           // t binds to s's value, s still usable
// let u = s.copy() // ERROR: String doesn't implement Copy
let u = move s      // Explicit move transfers ownership
```

**Relationship to parameter passing:**

```cantrip
// Copy-capable type - still passes by permission
procedure process(n: i32) {
    // n is passed by permission, no copy unless .copy() called
    let local = n.copy()  // Explicit copy
}

let x = 42
process(x)  // x passed by permission, still usable after

// Non-Copy type - also passes by permission
procedure print(text: String) {
    println(text)  // Access through permission
}

let s = String.new("test")
print(s)  // s passed by permission, still usable after
```

### 0.7 Specification Status

This table tracks the completeness of the type system specification:

| Type Category | Section | Status | Priority |
|---------------|---------|--------|----------|
| **Type System Foundations** | §0 | ✅ **COMPLETE** | ✅ P0 |
| Primitive Types | §5 | ✅ Complete | - |
| - Integer Types | §5.1 | ✅ Specified | - |
| - Floating-Point Types | §5.2 | ✅ Specified | - |
| - Boolean and Character | §5.3 | ✅ Specified | - |
| - **String Types** | §5.4 | ✅ **COMPLETE** | ✅ P0 |
| Arrays | §6 | ✅ **COMPLETE** | ✅ |
| - Fixed-size arrays | §6.1-6.3 | ✅ Specified | - |
| - **Mutable slices** | §6.7 | ✅ **COMPLETE** | ✅ P1 |
| Tuples | §7 | ⚠️ Minor gaps | 🟢 P2 |
| - Tuple types | §7.1 | ✅ Specified | - |
| - Unit type | §7.2 | ✅ Specified | - |
| - **Subtyping rules** | §7.3 | ⚠️ Needs proof | 🟢 P2 |
| Records | §8 | ✅ **COMPLETE** | ✅ |
| - Named records | §8.1-8.7 | ✅ Specified | - |
| - **Tuple structs** | §8.8 | ✅ **COMPLETE** | ✅ P1 |
| - Procedures | §8.5 | ✅ Specified | - |
| Enums | §9 | ✅ **COMPLETE** | ✅ |
| - Enum syntax | §9.1-9.2 | ✅ Specified | - |
| - Pattern matching | §9.3 | ✅ Specified | - |
| - **Memory layout** | §9.4.2 | ✅ **COMPLETE** | ✅ P1 |
| - **Discriminant size** | §9.4.4 | ✅ **COMPLETE** | ✅ P1 |
| Traits | §10 | ⚠️ Needs object safety | 🟡 P1 |
| - Trait definitions | §10.1 | ✅ Specified | - |
| - Trait implementations | §10.2 | ✅ Specified | - |
| - **Object safety** | §10.6 | ❌ **NOT SPECIFIED** | 🟡 P1 |
| - **Trait objects** | §10.7 | ⚠️ Mentioned, not formal | 🟡 P1 |
| Generics | §11 | ⚠️ Needs lifetimes | 🟡 P1 |
| - Type parameters | §11.1 | ✅ Specified | - |
| - Const generics | §11.5 | ✅ Specified | - |
| - **Lifetime parameters** | §11.6 | ❌ **NOT SPECIFIED** | 🟡 P1 |
| - **Associated type bounds** | §11.7 | ❌ **NOT SPECIFIED** | 🟢 P2 |
| Modals | §12 | ⚠️ Needs construction | 🟡 P1 |
| - State machines | §12.1 | ✅ Specified | - |
| - Transitions | §12.2 | ✅ Specified | - |
| - **Initial state & construction** | §12.3 | ❌ **NOT SPECIFIED** | 🟡 P1 |
| - **Generic modals** | §12.4 | ❌ **NOT SPECIFIED** | 🟢 P2 |
| **Pointer Types** | §9 | ✅ **COMPLETE** | ✅ P0 |
| - Safe pointers (Ptr<T>@State) | §9.2-9.4 | ✅ Specified | - |
| - Raw pointers (*T, *mut T) | §9.2-9.4 | ✅ Specified | - |
| - Pointer arithmetic | §9.4 | ✅ Specified | - |
| - Null pointers | §9.5 | ✅ Specified | - |
| **Function Types** | §10 | ✅ **COMPLETE** | ✅ P0 |
| - Function pointers (fn) | §10.2-10.3 | ✅ Specified | - |
| - Closures | §10.5 | ✅ Specified | - |
| - Procedure types | §10.6 | ✅ Specified | - |
| **Type Aliases** | §11 | ✅ **COMPLETE** | ✅ P0 |
| - Transparent aliases | §11.2-11.3 | ✅ Specified | - |
| - Newtype pattern | §11.5 | ✅ Specified | - |
| **Marker Traits** | - | ⚠️ **PARTIAL** | 🟢 P2 |
| - Copy | §0.6 | ✅ **COMPLETE** | ✅ P0 |
| - Clone | - | ❌ **NOT SPECIFIED** | 🟢 P2 |
| - Send, Sync | - | ❌ **NOT SPECIFIED** | 🟢 P2 |
| - Sized, Unsize | - | ❌ **NOT SPECIFIED** | 🟢 P2 |

**Legend:**
- ✅ **COMPLETE**: Fully specified with formal semantics
- ⚠️ **PARTIAL**: Specified but with gaps or unclear areas
- ❌ **MISSING**: Not specified at all
- 🔴 **P0**: Critical for completeness
- 🟡 **P1**: Important for clarity
- 🟢 **P2**: Nice to have

**Summary:**
- **Complete**: 28 sections (62%)
- **Partial**: 9 sections (20%)
- **Missing**: 8 sections (18%)
- **Total**: 45 type system components

**Critical gaps (P0):**
✅ All P0 (Critical) items have been completed!
1. ✅ String types (str vs String) - §5.4 Complete
2. ✅ Function types (fn, closures) - §10 Complete
3. ✅ Type aliases (type Name = T) - §11 Complete

### 0.8 Notation and Conventions

**Metavariables:**

```
τ, σ, ρ           Types
α, β, γ           Type variables
e, e₁, e₂         Expressions
v, v₁, v₂         Values
x, y, z           Term variables
f, g, h           Function names
Γ, Δ, Θ           Typing contexts
σ, σ', σ''        Memory stores
n, m, k           Natural numbers (including array sizes)
S, T, U           Type names (records, enums)
I, J              Trait names
B                 Trait bounds
```

**Judgment notation:**

```
Γ ⊢ e : τ                    e has type τ in context Γ
Γ ⊢ τ : Type                 τ is a well-formed type
Γ ⊢ τ₁ <: τ₂                 τ₁ is a subtype of τ₂
Γ ⊢ τ₁ ≡ τ₂                  τ₁ and τ₂ are equivalent types
Γ ⊢ p : τ ⇝ Γ'               Pattern p of type τ extends Γ to Γ'
⟨e, σ⟩ ⇓ ⟨v, σ'⟩              e evaluates to v, updating store σ to σ'
⟨e, σ⟩ → ⟨e', σ'⟩             e reduces to e' in one step
⟨e, σ⟩ →* ⟨e', σ'⟩            e reduces to e' in zero or more steps
```

**Set notation:**

```
⟦τ⟧                         Semantic interpretation of type τ (set of values)
∅                           Empty set/context
x ∈ S                       x is a member of set S
S ∪ T                       Union of sets S and T
S ∩ T                       Intersection of sets S and T
S \ T                       Set difference (elements in S but not T)
|S|                         Cardinality (size) of set S
```

**Logical notation:**

```
∀x. P                       For all x, P holds (universal quantification)
∃x. P                       There exists x such that P (existential quantification)
P ⟹ Q                       P implies Q (implication)
P ⟺ Q                       P if and only if Q (biconditional)
P ∧ Q                       P and Q (conjunction)
P ∨ Q                       P or Q (disjunction)
¬P                          Not P (negation)
```

**Type constructor notation:**

```
[τ]                         Slice of elements of type τ
[τ; n]                      Array of n elements of type τ
(τ₁, ..., τₙ)               Tuple of types τ₁ through τₙ
T<τ₁, ..., τₙ>              Generic type T instantiated with types τ₁ through τₙ
τ₁ → τ₂                     Function type from τ₁ to τ₂
∀α. τ                       Polymorphic type universally quantified over α
```

**Substitution notation:**

```
τ[α ↦ σ]                    Type τ with type variable α substituted by σ
e[x ↦ v]                    Expression e with variable x substituted by value v
Γ[x ↦ τ]                    Context Γ with x mapped to type τ
```

### 0.9 Reading Guide

**Structure of type specifications:**

Each type section (§5 through §12) follows this standard structure:

1. **Overview** — Purpose, when to use, relationships to other features
2. **Syntax**
   - Concrete syntax (EBNF grammar)
   - Abstract syntax (formal notation)
   - Basic examples
3. **Static Semantics**
   - Well-formedness rules
   - Type rules
   - Type properties (theorems)
4. **Dynamic Semantics**
   - Evaluation rules
   - Memory representation
   - Operational behavior
5. **Examples and Patterns** — Practical usage patterns

**Cross-references:**

- **Permissions (Part III):** How types interact with own/mut/imm
- **Effects (Part IV):** Function types with effect signatures
- **Contracts (Part V):** Type invariants and pre/post-conditions
- **Regions (Part VI):** Lifetime and memory safety
- **Syntax (Part I):** Concrete syntax definitions
- **Semantics (Part VII):** Operational and denotational semantics

**Conventions:**

- Type rules use inference rule notation: premises above the line, conclusion below
- Examples use syntax highlighting with `cantrip` language marker
- Formal definitions use **bold** with numbering: **Definition X.Y**
- Theorems and lemmas use **bold** with numbering: **Theorem X.Y**
- Errors reference diagnostic codes: `E8201`, `E9120`, etc.

**Interpretation:**

- **MUST**: Required behavior for conforming implementations
- **SHOULD**: Recommended behavior
- **MAY**: Optional behavior
- **Well-typed programs**: Programs that pass type checking
- **Undefined behavior**: Behavior not specified by this document

---

**Next**: [Primitive Types](PART_A_Primitives/01_PrimitiveTypes.md)
