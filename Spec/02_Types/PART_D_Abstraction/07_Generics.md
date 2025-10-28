# Chapter 2: Type System - §2.7. Generics and Parametric Polymorphism

**Section**: §2.7 | **Part**: Type System (Part II)
**Previous**: [Traits](06_Traits.md) | **Next**: [Map Types](08_MapTypes.md)

---

**Definition 11.1 (Parametric Polymorphism):** Generic functions and types abstract over type parameters. A generic function `∀α. e : τ` is universally quantified over type variable α, and can be instantiated at any concrete type. Generic types enable code reuse while maintaining type safety through parametric polymorphism.

## 2.7. Generics and Parametric Polymorphism

### 2.7.1 Overview

**Key innovation/purpose:** Generics enable writing code once that works with multiple types, providing type-safe abstraction without runtime overhead through compile-time monomorphization.

**When to use generics:**

- Collections (Vec<T>, HashMap<K, V>, etc.)
- Functions that work with multiple types identically
- Data structures that hold values of any type
- Algorithms that don't depend on specific type details
- Trait-bounded polymorphism (generic + trait bounds)
- Type-safe containers and wrappers
- Library code requiring broad applicability

**When NOT to use generics:**

- When working with a single known type → use concrete types
- When types need different implementations → use enums with pattern matching
- When you need a closed set of variants → use enums explicitly
- Excessive complexity without clear benefit → keep it simple
- When monomorphization bloat is a concern → use enums instead

**Relationship to other features:**

- **Traits**: Provide bounds for generic type parameters
- **Monomorphization**: Generics compiled to specialized code for each instantiation
- **Type inference**: Generic parameters often inferred from context
- **Higher-kinded types**: Not supported (generics are first-order only)
- **Const generics**: Allow compile-time constants as parameters

### 2.7.2 Syntax

#### 2.7.2.1 Concrete Syntax

**Generic function:**

```ebnf
GenericFunction ::= "function" Ident GenericParams ParamList (":" Type)? WhereClauses? ContractClauses? Block
GenericParams   ::= "<" TypeParam ("," TypeParam)* ">"
TypeParam       ::= Ident (":" Bounds)?
                  | "const" Ident ":" ConstType
Bounds          ::= Bound ("+" Bound)*
Bound           ::= Ident GenericArgs?
WhereClauses    ::= "where" WhereClause ("," WhereClause)*
WhereClause     ::= Type ":" Bound ("+" Bound)*
```

**Generic record:**

```ebnf
GenericRecord   ::= "record" Ident GenericParams "{" FieldList "}"
```

**Generic enum:**

```ebnf
GenericEnum     ::= "enum" Ident GenericParams "{" VariantList "}"
```

**Examples:**

```cantrip
function identity<T>(x: T): T {
    x
}

function swap<T>(a: T, b: T): (T, T) {
    (b, a)
}

record Pair<T, U> {
    first: T
    second: U
}

enum Option<T> {
    Some(T),
    None,
}
```

#### 2.7.2.2 Abstract Syntax

**Generic function types:**

```
τ ::= ∀α. τ    (universal quantification)
```

**Generic type application:**

```
e ::= e[τ]     (type application)
τ ::= T⟨τ₁, ..., τₙ⟩    (generic type instantiation)
```

**Type schemes:**

```
σ ::= ∀α₁...αₙ. τ    (polytype/type scheme)
```

#### 2.7.2.3 Basic Examples

**Generic identity:**

```cantrip
function identity<T>(x: T): T {
    x
}

let a = identity(42)        // T = i32, returns 42
let b = identity("hello")   // T = str, returns "hello"
```

**Generic container:**

```cantrip
record Container<T> {
    value: T
}

let int_cont = Container { value: 42 }
let str_cont = Container { value: "hello" }
```

### 2.7.3 Static Semantics

#### 2.7.3.1 Well-Formedness Rules

**Generic function:**

```
[WF-Generic-Fun]
Γ, α₁ : Type, ..., αₙ : Type ⊢ body : τ
─────────────────────────────────────────────
Γ ⊢ function f<α₁, ..., αₙ>(...): ∀α₁...αₙ. τ
```

**Generic record:**

```
[WF-Generic-Record]
Γ, α₁ : Type, ..., αₘ : Type ⊢ fields well-formed
──────────────────────────────────────────────────────
Γ ⊢ record S<α₁, ..., αₘ> { ... } : Type → ... → Type
```

**Type parameter bounds:**

```
[WF-Bounded-Param]
Γ ⊢ B₁ : Trait    ...    Γ ⊢ Bₙ : Trait
──────────────────────────────────────────────────────
Γ ⊢ α : B₁ + ... + Bₙ
```

#### 2.7.3.2 Type Rules

**Type instantiation:**

```
[T-Inst]
Γ ⊢ e : ∀α. τ
Γ ⊢ U : Type
──────────────────────────────
Γ ⊢ e[U] : τ[α ↦ U]
```

**Bounded instantiation:**

```
[T-Bounded-Inst]
Γ ⊢ e : ∀α : B. τ
Γ ⊢ U : Type
U : B    (U satisfies bound B)
──────────────────────────────
Γ ⊢ e[U] : τ[α ↦ U]
```

**Generic type construction:**

```
[T-Generic-Type]
Γ ⊢ T : Type → ... → Type    (n parameters)
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
──────────────────────────────────────────
Γ ⊢ T⟨τ₁, ..., τₙ⟩ : Type
```

**Where clause checking:**

```
[T-Where]
Γ, T : Type, T : C₁, ..., T : Cₙ ⊢ body : U
────────────────────────────────────────────────
Γ ⊢ function f<T> where T: C₁, ..., Cₙ : U
```

#### 2.7.3.3 Type Properties

**Theorem 11.1 (Parametricity):**

Generic functions cannot inspect or depend on the specific type of their parameters:

```
∀α. f : α → β
```

The implementation of f must work uniformly for all types α.

**Theorem 11.2 (Monomorphization):**

Every generic instantiation produces a distinct specialized function:

```
f<i32> ≠ f<String>
```

Each instantiation is compiled separately with no shared code.

**Theorem 11.3 (Type Substitution):**

Type substitution preserves well-typedness:

```
If Γ, α : Type ⊢ e : τ and Γ ⊢ U : Type
Then Γ ⊢ e[α ↦ U] : τ[α ↦ U]
```

#### 2.7.3.4 Type Bounds

**Purpose:** Type bounds constrain generic type parameters to types that satisfy specific trait requirements.

**Syntax:**

```cantrip
function serialize<T>(value: T): String
    where T: Serialize + Clone
    uses alloc.heap
{
    value.clone().to_json()
}
```

**Type rule:**

```
[T-Bounds]
Γ ⊢ T : B₁    ...    Γ ⊢ T : Bₙ
──────────────────────────────────
Γ ⊢ T : B₁ + ... + Bₙ
```

**Example with multiple bounds:**

```cantrip
function process<T, U>(data: T): U
    where
        T: Clone + Serialize,
        U: Display + Debug
    uses alloc.heap
{
    let copy = data.clone()
    let json = copy.to_json()
    U.from_json(json)
}
```

### 2.7.4 Dynamic Semantics

#### 2.7.4.1 Evaluation Rules

**Monomorphization semantics:**

Generic functions are not evaluated directly. Instead, they are instantiated at compile time:

```
[E-Monomorphize]
f<T> where T = ConcreteType
────────────────────────────────────────────
Generates specialized function f_ConcreteType
```

**Generic value construction:**

```
[E-Generic-Record]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
──────────────────────────────────────────────────────
⟨T⟨U⟩ { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨T⟨U⟩ { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

#### 2.7.4.2 Memory Representation

**Generic types:**

Generic types have the same memory layout as their instantiated versions:

```
size(Container<i32>) = size(i32) = 4 bytes
size(Container<String>) = size(String) = 24 bytes (on 64-bit)
```

Each instantiation has its own distinct layout.

**Monomorphization:**

```cantrip
// Generic function
function max<T: Comparable>(a: T, b: T): T {
    if a.compare(b) > 0 { a } else { b }
}

// Compiler generates specialized versions:
function max_i32(a: i32, b: i32): i32 { ... }
function max_f64(a: f64, b: f64): f64 { ... }
function max_String(a: String, b: String): String { ... }
```

**No runtime overhead:**

- All type parameters resolved at compile time
- No type tags or vtables
- Direct function calls (no indirection)
- Same performance as hand-written specialized code

#### 2.7.4.3 Operational Behavior

**Code generation:**

Each unique instantiation generates separate machine code:

```cantrip
let a = identity(42)       // Calls identity_i32
let b = identity(3.14)     // Calls identity_f64
let c = identity("hello")  // Calls identity_str
```

**Trade-off:**

- **Benefit**: Zero runtime overhead, maximum performance
- **Cost**: Increased binary size (code bloat for many instantiations)

### 2.7.5 Advanced Features

#### 2.7.5.1 Const Generics

**Purpose:** Const generics allow types and functions to be parameterized by compile-time constants (e.g., array lengths, matrix dimensions).

**Syntax:**

```ebnf
ConstParam     ::= "const" Ident ":" ConstType
ConstType      ::= "usize" | "u8" | "u16" | "u32" | "u64" | "isize" | "i8" | "i16" | "i32" | "i64" | "bool"
ConstArg       ::= IntegerLiteral | BooleanLiteral | ConstExpr
ConstExpr      ::= literal | arithmetic | sizeof(T) | alignof(T)
```

**Type rules:**

```
[T-Const-Generic]
Γ, N : const usize ⊢ e : τ
───────────────────────────────────────────
Γ ⊢ function f<const N: usize>(...) : τ

[T-Const-Inst]
Γ ⊢ n : usize      n is a compile-time constant
Γ ⊢ f : ∀(const N). τ
───────────────────────────────────────────
Γ ⊢ f<n> : τ[N ↦ n]
```

**Examples:**

```cantrip
// Function parameterized by a const length N
function process<T, const N: usize>(data: [T; N]): T {
    data[0]
}

// Type aliases with const parameters
type Matrix<T, const ROWS: usize, const COLS: usize> = [[T; COLS]; ROWS]

// Records with const parameters
record RingBuffer<T, const CAP: usize> {
    data: [T; CAP]
    head: usize
    tail: usize

    procedure new(): own RingBuffer<T, CAP>
        must CAP > 0
    {
        own RingBuffer {
            data: [T::default(); CAP],
            head: 0,
            tail: 0,
        }
    }

    procedure capacity(self: RingBuffer<T, CAP>): usize {
        CAP
    }
}
```

**Const expression evaluation:**

Const arguments must be evaluable at compile time:

```cantrip
const SIZE: usize = 10

// OK: const value
let buf: RingBuffer<i32, SIZE>

// OK: literal
let buf2: RingBuffer<i32, 100>

// OK: const arithmetic
const DOUBLED: usize = SIZE * 2
let buf3: RingBuffer<i32, DOUBLED>

// ERROR E9310: not a compile-time constant
let n = read_input()
let buf4: RingBuffer<i32, n>  // ERROR
```

**Diagnostics:**

- `E9310` — Non-constant used where const generic argument required
- `E9311` — Const expression not evaluable at compile time

#### 2.7.5.2 Higher-Rank Types (Not Supported)

Cantrip does not support higher-rank polymorphism (rank-2 or higher types):

```cantrip
// NOT SUPPORTED:
type F = function<T>(f: function<U>(U) => U): T
```

Generics are first-order only.

---

## 2.7.7 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - primitive types as generic parameters
- See §2.2 for [Product Types](../PART_B_Composite/02_ProductTypes.md) - generic tuples and records
- See §2.3 for [Sum Types](../PART_B_Composite/03_SumTypes.md) - generic enums (Option<T>, Result<T,E>)
- See §2.4 for [Collection Types](../PART_B_Composite/04_CollectionTypes.md) - generic array and slice operations
- See §2.6 for [Traits](06_Traits.md) - trait bounds on generic parameters
- See §2.8 for [Map Types](08_MapTypes.md) - generic map/dictionary types
- See §2.9 for [Type Aliases](../PART_E_Utilities/09_TypeAliases.md) - aliasing generic types
- See Part VI (Procedures) for generic function definitions
- See Part VII (Contracts) for generic contracts and where clauses
- **Examples**: See [07_GenericExamples.md](../Examples/07_GenericExamples.md) for practical generic patterns

## 2.7.8 Notes and Warnings

**Note 1 (Monomorphization):** Generics are compiled through monomorphization - the compiler generates specialized code for each concrete type used. This provides zero-cost abstraction but increases binary size.

**Note 2 (Type Inference):** Generic type parameters are often inferred from usage context. Explicit turbofish syntax `function::<Type>()` can specify types when inference fails.

**Note 3 (Trait Bounds):** Generic parameters can be constrained with trait bounds: `<T: Display + Clone>` requires T to have both Display and Clone traits attached.

**Note 4 (Where Clauses):** Complex bounds use where clauses for readability: `where T: Display, U: Clone`. Where clauses appear after parameter list.

**Note 5 (Lifetime Parameters):** Generics can include lifetime parameters (Part IV) alongside type parameters: `<'a, T>` where 'a is a lifetime and T is a type.

**Note 6 (Const Generics):** Compile-time constant parameters enable array sizes and other compile-time values: `<T, const SIZE: usize>`.

**Note 7 (No Higher-Kinded Types):** Cantrip generics are first-order only. You cannot abstract over type constructors (no `F<T>` where F itself is a generic parameter).

**Note 8 (Associated Types):** Traits can define associated types that act as type members, providing an alternative to generic parameters in some cases.

**Warning 1 (Binary Bloat):** Excessive generic instantiations increase binary size. Each unique concrete type generates separate code. Consider using dynamic dispatch or enums for many variants.

**Warning 2 (Orphan Rules):** You can only implement traits for generic types if you defined either the trait or the generic type in your crate (coherence/orphan rules).

**Warning 3 (Recursive Bounds):** Avoid overly complex recursive trait bounds that make type checking difficult. Keep bounds simple and composable.

**Warning 4 (Type Inference Failures):** Complex generic code may require explicit type annotations when the compiler cannot infer types. Use turbofish syntax or variable type annotations.

**Performance Note 1 (Zero Cost):** Properly used generics have zero runtime cost compared to hand-written specialized code. The compiler generates the same machine code.

**Performance Note 2 (Inline Optimization):** Generic functions are excellent candidates for inlining across specializations, enabling aggressive optimization.

**Implementation Note 1 (Default Type Parameters):** Generic types may support default type parameters: `record Foo<T = i32>` where T defaults to i32 if not specified.

**Implementation Note 2 (Generic Limits):** Compilers may impose practical limits on generic parameter counts and nesting depth to ensure reasonable compilation times.

---

**Previous**: [Traits](06_Traits.md) | **Next**: [Map Types](08_MapTypes.md)
