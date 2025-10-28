# Chapter 2: Type System - §2.9. Type Aliases

**Section**: §2.9 | **Part**: Type System (Part II)
**Previous**: [Function Types](../PART_D_Abstraction/08_FunctionTypes.md) | **Next**: [Self Type](10_SelfType.md)

---

**Definition 11.1 (Type Alias):** A type alias introduces a new name for an existing type. Type aliases are transparent: they create a synonym for a type rather than a new distinct type. Formally, if `type A = τ`, then `A ≡ τ` (type equivalence).

## 2.9. Type Aliases

### 2.9.1 Overview

**Key innovation/purpose:** Type aliases provide meaningful names for complex types, improving code readability and maintainability without introducing runtime overhead or new type distinctions.

**When to use type aliases:**

- Simplify complex type signatures (function pointers, nested generics)
- Document domain-specific meanings of types
- Reduce repetition of lengthy type expressions
- Create stable APIs that can evolve internally
- Abbreviate commonly used type combinations
- Improve error messages with semantic names
- Define project-specific vocabulary for types

**When NOT to use type aliases:**

- Need distinct types with different semantics → use newtype pattern (`record Name(T);`)
- Need different trait implementations → use newtype pattern
- Want to prevent accidental type confusion → use newtype pattern
- Type safety must nominal distinction → use newtype pattern
- Simple types that don't need renaming → use original type directly

**Relationship to other features:**

- **Newtype Pattern (§4):** Records with single fields create distinct nominal types
- **Associated Types (§6.5):** Type aliases within trait definitions for polymorphism
- **Generics (§7):** Type aliases can be parameterized by type variables
- **Transparency:** Aliases are interchangeable with their definitions everywhere
- **Type Equivalence:** `A ≡ τ` means A and τ are the same type

### 2.9.2 Syntax

#### 2.9.2.1 Concrete Syntax

**Grammar:**

```ebnf
TypeAlias     ::= "type" Ident GenericParams? WhereClauses? "=" Type ";"
GenericParams ::= "<" TypeParam ("," TypeParam)* ">"
TypeParam     ::= Ident (":" Bounds)?
WhereClauses  ::= "where" WhereClause ("," WhereClause)*
WhereClause   ::= Type ":" Bound ("+" Bound)*
```

**Syntax:**

```cantrip
// Simple type alias
type Kilometers = f64

// Generic type alias
type Result<T> = Result<T, Error>

// Complex type alias (function pointer)
type Callback = fn(String) => bool

// Type alias with bounds
type CompareFn<T> = fn(T, T) => i32
    where T: Ord

// Type alias for nested generics
type StringMap<V> = HashMap<String, V>
```

#### 2.9.2.2 Abstract Syntax

**Formal representation:**

```
TypeDef ::= type A<α₁, ..., αₙ> = τ

where:
  A      = alias name (identifier)
  α₁...αₙ = type parameters (optional)
  τ      = target type expression
```

**Type equivalence rule:**

```
[Equiv-Alias]
type A<α₁, ..., αₙ> = τ
──────────────────────────────────────
A<τ₁, ..., τₙ> ≡ τ[α₁ ↦ τ₁, ..., αₙ ↦ τₙ]
```

**Components:**

- **Alias name:** Identifier introducing the new type name
- **Type parameters:** Optional generic parameters `<α₁, ..., αₙ>`
- **Target type:** The type being aliased (right-hand side)
- **Where clauses:** Optional trait bounds on type parameters


### 2.9.3 Static Semantics

#### 2.9.3.1 Well-Formedness Rules

**Definition 11.2 (Well-Formed Type Alias):** A type alias `type A<α₁, ..., αₙ> = τ` is well-formed if:

1. The target type τ is well-formed under the given type parameters
2. All type parameters appear in τ (no unused parameters)
3. All trait bounds in where clauses are satisfiable
4. The alias does not create cycles (no recursive aliases without indirection)

```
[WF-TypeAlias]
Γ, α₁ : Type, ..., αₙ : Type ⊢ τ : Type
FV(τ) ⊆ {α₁, ..., αₙ}    (no free variables)
No cycles: A ∉ FV(τ)      (non-recursive)
────────────────────────────────────────
Γ ⊢ type A<α₁, ..., αₙ> = τ  well-formed
```

**Well-formedness examples:**

**Valid:**

```cantrip
type Point2D = (f64, f64)                    // ✓ Well-formed
type Callback<T> = fn(T) => Option<T>       // ✓ Generic parameter used
type Matrix<T> = Vec<Vec<T>> where T: Num   // ✓ Bounded parameter used
```

**Invalid:**

```cantrip
type Invalid<T> = String    // ✗ Unused type parameter T
type Cycle = Vec<Cycle>     // ✗ Recursive without indirection
type Bad<T> = fn(U) => T    // ✗ Free variable U not in parameters
```

#### 2.9.3.2 Type Rules

**[T-TypeAlias] Type Alias Declaration:**

```
[T-TypeAlias]
Γ ⊢ τ : Type
type A = τ declared in scope
─────────────────────────────
Γ ⊢ A ≡ τ    (type equivalence)
```

**Explanation:** Type alias declarations establish type equivalence. The alias name A and its definition τ are fully interchangeable in all contexts.

```cantrip
type Distance = f64

let d: Distance = 5.5      // ✓ Distance = f64
let x: f64 = d             // ✓ Transparent, no conversion needed
```

**[T-TypeAliasInst] Generic Type Alias Instantiation:**

```
[T-TypeAliasInst]
type A<α₁, ..., αₙ> = τ declared
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
─────────────────────────────────────────
Γ ⊢ A<τ₁, ..., τₙ> ≡ τ[α₁ ↦ τ₁, ..., αₙ ↦ τₙ]
```

**Explanation:** Generic type aliases are instantiated by substituting concrete types for type parameters.

```cantrip
type Pair<T> = (T, T)

let p: Pair<i32> = (1, 2)        // ✓ Pair<i32> ≡ (i32, i32)
let q: (i32, i32) = p            // ✓ Transparent equivalence
```

**[T-AliasTransparency] Transparency Rule:**

```
[T-AliasTransparency]
type A = τ
Γ ⊢ e : τ
───────────
Γ ⊢ e : A

Γ ⊢ e : A
───────────
Γ ⊢ e : τ
```

**Explanation:** Expressions can be typed as either the alias or its definition interchangeably without conversion.

```cantrip
type Age = u32

function is_adult(age: Age) => bool {
    age >= 18     // ✓ Age is transparent to u32, >= works
}

let years: u32 = 25
is_adult(years)  // ✓ u32 can be passed where Age expected
```

**[T-AliasSubstitution] Substitution in Types:**

```
[T-AliasSubstitution]
type A = τ₁
τ₂ contains A
────────────────────────
τ₂[A ↦ τ₁] is valid type
```

**Explanation:** Aliases can be substituted in any type expression.

```cantrip
type Score = i32
type GameState = HashMap<String, Score>

// GameState ≡ HashMap<String, i32>
```

**[T-AliasBounds] Bounded Type Alias:**

```
[T-AliasBounds]
type A<α> = τ where α: Trait
Γ, α : Type, α : Trait ⊢ τ : Type
Γ ⊢ T : Trait    (bound satisfied)
──────────────────────────────────
Γ ⊢ A<T> ≡ τ[α ↦ T]
```

**Explanation:** Type aliases with where clauses require bounds to be satisfied at instantiation.

```cantrip
type Comparator<T> = fn(T, T) => i32
    where T: Ord

// ✓ i32 implements Ord
let cmp: Comparator<i32> = |a, b| a - b

// ✗ Error: Vec<T> does not implement Ord
// let bad: Comparator<Vec<i32>> = ...
```

**[T-AliasNesting] Nested Type Aliases:**

```
[T-AliasNesting]
type A = τ₁
type B = τ₂[A]    (B contains A)
─────────────────────────────────
B ≡ τ₂[A ↦ τ₁]    (transitive)
```

**Explanation:** Type aliases compose transitively.

```cantrip
type Key = String
type Value = i32
type Cache = HashMap<Key, Value>

// Cache ≡ HashMap<String, i32>
```

**[T-AliasFunction] Function Type Aliases:**

```
[T-AliasFunction]
type F = fn(τ₁, ..., τₙ) → τ
Γ ⊢ f : fn(τ₁, ..., τₙ) → τ
───────────────────────────────
Γ ⊢ f : F
```

**Explanation:** Function pointer types can be aliased for readability.

```cantrip
type BinaryOp = fn(i32, i32) => i32

let add: BinaryOp = |a, b| a + b
let sub: BinaryOp = |a, b| a - b

function apply(op: BinaryOp, x: i32, y: i32) => i32 {
    op(x, y)
}
```

**[T-AliasInference] Type Inference with Aliases:**

```
[T-AliasInference]
type A = τ
Γ ⊢ e : _    (type variable)
e used in context expecting τ
─────────────────────────────
Γ ⊢ e : A    (inferred as alias)
```

**Explanation:** Type inference can infer aliases or their definitions.

```cantrip
type Index = usize

let indices = vec![1, 2, 3]  // Inferred as Vec<Index> or Vec<usize>
```

**[T-AliasError] Error Reporting with Aliases:**

```
[T-AliasError]
type A = τ
Γ ⊢ e : τ'    τ' ≠ τ
Expected type: A
─────────────────────────────
ERROR: Expected A (which is τ), found τ'
```

**Explanation:** Error messages display both the alias name and its definition for clarity.

```cantrip
type UserId = u64

function get_user(id: UserId) { ... }

get_user("invalid")
// ERROR: Expected UserId (which is u64), found &str
```

**[T-AliasVisibility] Visibility and Scope:**

```
[T-AliasVisibility]
type A = τ declared in module M
A visible in current scope
────────────────────────────
Can use A ≡ τ
```

**Explanation:** Type aliases respect module visibility rules.

```cantrip
module math {
    pub type Vector = (f64, f64, f64)  // Public alias
    type Internal = i32                 // Private alias
}

use math::Vector
let v: Vector = (1.0, 2.0, 3.0)  // ✓ Public alias accessible

// let x: math::Internal = 5     // ✗ Private alias not accessible
```

**[T-AliasRecursive] Recursive Types with Indirection:**

```
[T-AliasRecursive]
type List<T> = Option<Ptr<(T, List<T>)>@Exclusive>
Ptr provides heap indirection
─────────────────────────────────────────
Recursive type is well-formed
```

**Explanation:** Recursive type aliases require heap indirection (via Ptr) to have finite size.

```cantrip
// ✗ Invalid: Direct recursion
// type List<T> = (T, List<T>)  // Infinite size!

// ✓ Valid: Recursion through heap pointer
type List<T> = Option<Ptr<(T, List<T>)>@Exclusive>

let list: List<i32> = Some(Ptr.new(&(1, None)))
```

**[T-AliasAssociated] Associated Type Aliases:**

```
[T-AliasAssociated]
trait I { type A; }
impl I for T { type A = τ; }
─────────────────────────────
T : I  ∧  T::A ≡ τ
```

**Explanation:** Associated types in trait implementations are type aliases bound to the implementing type.

```cantrip
trait Container {
    type Item
    function get(self: Self, index: usize) => Option<Self::Item>
}

impl Container for Vec<i32> {
    type Item = i32  // Associated type alias

    function get(self: Self, index: usize) => Option<i32> {
        if index < self.len() {
            Some(self[index])
        } else {
            None
        }
    }
}
```

**[T-AliasEffect] Type Aliases with Effects:**

```
[T-AliasEffect]
type F = fn(τ₁) → τ₂ ! ε
Γ ⊢ f : fn(τ₁) → τ₂ ! ε
─────────────────────────
Γ ⊢ f : F
```

**Explanation:** Type aliases can include effect signatures for functions.

```cantrip
type Logger = fn(String) => () ! {io.write}
type Reader = fn() => String ! {io.read}

let log: Logger = |msg| println(msg)
```

**[T-AliasPermission] Type Aliases with Permissions:**

```
[T-AliasPermission]
type A = π τ    (alias includes permission)
Γ ⊢ e : π τ
─────────────
Γ ⊢ e : A
```

**Explanation:** Type aliases can encode permission qualifiers.

```cantrip
type OwnedString = own String
type SharedInt = imm i32
type MutableVec<T> = mut Vec<T>
```

**[T-AliasConversion] No Implicit Conversion:**

```
[T-AliasConversion]
type A = τ
A and τ are same type
────────────────────────
No conversion needed: A = τ (not A → τ)
```

**Explanation:** Type aliases are transparent; no conversion or coercion occurs.

```cantrip
type Meters = f64

let distance: Meters = 10.5
let value: f64 = distance    // ✓ No conversion, same type

// Newtype would require explicit conversion:
// record MetersNewtype(f64)
// let d = MetersNewtype(10.5)
// let v: f64 = d.0  // Must access field
```

**[T-AliasCopy] Trait Implementation Inheritance:**

```
[T-AliasCopy]
type A = τ
τ : Trait
─────────
A : Trait
```

**Explanation:** Type aliases inherit all trait implementations from their target type.

```cantrip
type Point = (i32, i32)

// (i32, i32) implements Copy, Clone, Debug, etc.
// Therefore Point also implements Copy, Clone, Debug, etc.

let p1: Point = (5, 10)
let p2 = p1  // ✓ Both bind to same value (permission), p1 still usable
let p3 = p1.copy()  // ✓ Explicit copy because (i32, i32) is Copy
```

#### 2.9.3.3 Type Properties

**Theorem 11.1 (Type Equivalence Transitivity):** If `type A = B` and `type B = C`, then `A ≡ C`.

**Proof sketch:** By definition of type alias, `A ≡ B` and `B ≡ C`. Type equivalence is transitive, so `A ≡ C` follows. ∎

**Corollary 11.1:** Type alias chains resolve to their final target type.

**Theorem 11.2 (Type Alias Transparency):** For any type alias `type A = τ`, expressions of type A and expressions of type τ are interchangeable in all contexts.

**Proof sketch:** By [T-AliasTransparency], `Γ ⊢ e : τ` implies `Γ ⊢ e : A` and vice versa. Therefore, A and τ can be used interchangeably. ∎

**Corollary 11.2:** Type aliases do not affect type checking, only improve readability.

**Theorem 11.3 (Well-Founded Recursion):** Recursive type aliases are well-formed if and only if they contain indirection (Box, reference, or pointer).

**Proof sketch:** Without indirection, the type has infinite size, violating well-formedness. With indirection, the size is finite (pointer size + heap allocation). ∎

**Theorem 11.4 (Monomorphization Erasure):** Type aliases are erased during monomorphization and do not affect generated code.

**Proof sketch:** Since type aliases are transparent equivalences, the compiler replaces all alias occurrences with their definitions before code generation. The resulting code is identical to code written without aliases. ∎

**Corollary 11.3:** Type aliases have zero runtime overhead.

### 2.9.4 Dynamic Semantics

#### 2.9.4.1 Evaluation Rules

Type aliases have no evaluation rules because they are purely compile-time constructs. They are erased before code generation.

**[E-AliasErased] Compile-Time Erasure:**

```
[E-AliasErased]
type A = τ
e contains type A
─────────────────────────
e[A ↦ τ] evaluated at runtime (A erased)
```

**Explanation:** At runtime, all type aliases are replaced by their definitions. Type aliases exist only in the source code and type system.

```cantrip
type Distance = f64

let d: Distance = 10.5
// At runtime: let d: f64 = 10.5;  (Distance erased)
```

#### 2.9.4.2 Memory Representation

**Layout:** Type aliases have the same memory layout as their target type.

```
Size(type A = τ) = Size(τ)
Align(type A = τ) = Align(τ)
```

**Examples:**

```cantrip
type Coordinate = (f32, f32)
// Size: 8 bytes (2 × f32)
// Align: 4 bytes (alignment of f32)

type Callback = fn(i32) => bool
// Size: 0 bytes (function types are zero-sized)
// Align: 1 byte
```

**Representation invariant:** The memory representation of a value of type A is identical to the memory representation of the same value of type τ.

#### 2.9.4.3 Operational Behavior

Type aliases do not change operational behavior. All operations on aliased types are identical to operations on their target types.

```cantrip
type Index = usize

let i: Index = 5
let j: usize = 10

let sum = i + j  // Same as: 5_usize + 10_usize
// No conversion, same operation
```

**Performance characteristic:** Type aliases have zero runtime cost. They are purely a compile-time abstraction that improves code readability without affecting performance.

### 2.9.5 Newtype Pattern

The **newtype pattern** uses single-field record types to create distinct nominal types. Unlike type aliases, newtypes are not transparent and provide type safety through nominal distinctions.

**Syntax:**

```cantrip
record NewtypeName(InnerType)
```

**Comparison with Type Aliases:**

| Feature                   | Type Alias                | Newtype Pattern                 |
| ------------------------- | ------------------------- | ------------------------------- |
| **Type equivalence**      | Transparent (`A ≡ τ`)     | Distinct nominal type           |
| **Type safety**           | No protection             | Prevents mixing similar types   |
| **Trait implementations** | Inherited from target     | Custom implementations possible |
| **Conversion**            | Automatic (same type)     | Explicit field access           |
| **Use case**              | Readability, abbreviation | Type safety, abstraction        |

**Examples:**

**Type alias (transparent):**

```cantrip
type Meters = f64
type Feet = f64

let m: Meters = 10.0
let f: Feet = 33.0

// ✗ Oops! Can mix Meters and Feet (both are f64)
let sum = m + f  // ✓ Compiles but semantically wrong
```

**Newtype pattern (distinct types):**

```cantrip
record Meters(f64)
record Feet(f64)

let m = Meters(10.0)
let f = Feet(33.0)

// ✓ Cannot mix Meters and Feet (compiler error)
// let sum = m + f  // ✗ ERROR: cannot add Meters and Feet

// Must convert explicitly
function feet_to_meters(f: Feet) => Meters {
    Meters(f.0 * 0.3048)
}

let sum_meters = m.0 + feet_to_meters(f).0  // ✓ Explicit
```

**When to use newtype pattern:**

- **Type safety:** Prevent accidental mixing of similar types (currencies, units, IDs)
- **Trait encapsulation:** Provide different trait implementations than inner type
- **Abstraction:** Hide implementation details behind a nominal type
- **API stability:** Inner type can change without breaking API

**Example: Preventing ID confusion**

```cantrip
record UserId(u64)
record ProductId(u64)

function get_user(id: UserId) => User { ... }
function get_product(id: ProductId) => Product { ... }

let user_id = UserId(12345)
let product_id = ProductId(67890)

get_user(user_id)      // ✓ Correct
get_user(product_id)   // ✗ ERROR: Expected UserId, found ProductId
```

### 2.9.6 Generic Type Aliases

Generic type aliases allow parameterization by type variables, enabling reusable type abbreviations.

**Syntax:**

```cantrip
type AliasName<T1, T2, ...> = ComplexType<T1, T2, ...>
```

**Examples:**

**Standard library patterns:**

```cantrip
// Result type with fixed error type
type IoResult<T> = Result<T, IoError>
type ParseResult<T> = Result<T, ParseError>

function read_config(): IoResult<Config> {
    // Return type is Result<Config, IoError>
    ...
}
```

**Collection type aliases:**

```cantrip
type StringMap<V> = HashMap<String, V>
type IntSet = HashSet<i32>
type Matrix<T> = Vec<Vec<T>>

let scores: StringMap<i32> = HashMap::new()
let grid: Matrix<f64> = vec![vec![1.0, 2.0], vec![3.0, 4.0]]
```

**Function type aliases:**

```cantrip
type Predicate<T> = fn(T) => bool
type Transform<T, U> = fn(T) => U
type FoldFn<T, A> = fn(A, T) => A

function filter<T>(items: Vec<T>, pred: Predicate<T>) => Vec<T> {
    items.into_iter().filter(pred).collect()
}
```

**Trait-bounded generic aliases:**

```cantrip
type Comparator<T> = fn(T, T) => i32
    where T: Ord

type Serializer<T> = fn(T) => String
    where T: Display

type Cloner<T> = fn(T) => T
    where T: Clone
```

**Complex nested generics:**

```cantrip
// Simplify callback signatures
type AsyncCallback<T> = fn(Result<T, Error>) => ()
type EventHandler<E> = fn(E) => EventResult
type StateTransition<S> = fn(S) => Option<S>

// Simplify data structure signatures
type Graph<N, E> = HashMap<N, Vec<(N, E)>>
type Cache<K, V> = Arc<Mutex<HashMap<K, V>>>
```

## 2.9.7 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - primitive types as alias targets
- See §2.2 for [Product Types](../PART_B_Composite/02_ProductTypes.md) - tuple and record types in aliases
- See §2.3 for [Sum Types](../PART_B_Composite/03_SumTypes.md) - enum types and newtype pattern
- See §2.4 for [Collection Types](../PART_B_Composite/04_CollectionTypes.md) - collection type aliases
- See §2.6 for [Traits](../PART_D_Abstraction/06_Traits.md) - trait bounds in generic aliases
- See §2.7 for [Generics](../PART_D_Abstraction/07_Generics.md) - generic type parameters in aliases
- See §2.8 for [Map Types](../PART_D_Abstraction/08_MapTypes.md) - function type aliases
- See Part VIII (Modules) for type alias visibility and imports
- **Examples**: See [09_TypeAliasExamples.md](../Examples/09_TypeAliasExamples.md) for practical alias patterns

## 2.9.8 Notes and Warnings

**Note 1 (Transparency):** Type aliases are completely transparent - they create alternative names, not new types. `type A = B` means A and B are interchangeable everywhere.

**Note 2 (Zero Cost):** Type aliases have zero runtime cost. They exist only at compile time for improved readability and documentation.

**Note 3 (Generic Aliases):** Type aliases can have generic parameters, enabling parameterized abbreviations like `type Result<T> = Result<T, MyError>`.

**Note 4 (Recursive Aliases):** Type aliases can be recursive if they appear through indirection (pointer, Box, etc.): `type List<T> = Option<Box<(T, List<T>)>>`.

**Note 5 (No Separate Implementations):** You cannot implement traits specifically for a type alias. Trait implementations apply to the underlying type.

**Note 6 (Newtype for Safety):** For type safety (preventing misuse of similar types), use the newtype pattern (single-field records) instead of type aliases.

**Note 7 (Module Visibility):** Type aliases respect module visibility. Private aliases are only accessible within their defining module.

**Note 8 (Documentation):** Type aliases serve as inline documentation, making complex types self-explanatory. Use them to communicate intent.

**Warning 1 (No Type Safety):** Type aliases provide NO type safety. `type Meters = f64` and `type Feet = f64` are both `f64` and can be mixed accidentally.

**Warning 2 (Confusing Errors):** Compiler error messages may show either the alias name or the expanded type, potentially causing confusion.

**Warning 3 (Infinite Recursion):** Direct recursive aliases without indirection are invalid: `type Foo = (i32, Foo)` causes infinite expansion. Use `type Foo = (i32, Box<Foo>)`.

**Warning 4 (Cannot Override):** You cannot "override" or "refine" an alias. `type A = B` in one module and `type A = C` in another creates a conflict if both are imported.

**Performance Note 1:** Type aliases have literally zero runtime overhead. All alias resolution happens at compile time.

**Performance Note 2:** Using aliases for complex generic types (like `HashMap<K, V>`) doesn't affect performance - the generated code is identical to using the full type name.

**Implementation Note 1:** Compilers typically implement type aliases through simple substitution during type checking and name resolution.

**Implementation Note 2:** Debuggers may show aliased types by their underlying representation, not the alias name. This is implementation-dependent.

---

**Previous**: [Map Types](../PART_D_Abstraction/08_MapTypes.md) | **Next**: [Self Type](10_SelfType.md)
