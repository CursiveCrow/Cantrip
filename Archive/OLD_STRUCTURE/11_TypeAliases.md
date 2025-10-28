# Part II: Type System - §11. Type Aliases

**Section**: §11 | **Part**: Type System (Part II)
**Previous**: [Function Types](10_FunctionTypes.md) | **Next**: [Modals](08_Modals.md)

---

**Definition 11.1 (Type Alias):** A type alias introduces a new name for an existing type. Type aliases are transparent: they create a synonym for a type rather than a new distinct type. Formally, if `type A = τ`, then `A ≡ τ` (type equivalence).

## 11. Type Aliases

### 11.1 Overview

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

### 11.2 Syntax

#### 11.2.1 Concrete Syntax

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

#### 11.2.2 Abstract Syntax

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

#### 11.2.3 Basic Examples

```cantrip
// Domain-specific type vocabulary
type UserId = u64
type Temperature = f32
type Timestamp = i64

// Function pointer aliases
type Predicate<T> = fn(T) => bool
type Comparator<T> = fn(T, T) => i32

// Collection aliases
type StringList = Vec<String>
type IntSet = HashSet<i32>

// Result type shortcuts
type IoResult<T> = Result<T, IoError>
type ParseResult = Result<i32, ParseError>
```

**Explanation:** These examples show how type aliases create readable names for types that would otherwise require verbose repetition. Aliases are fully transparent and can be used interchangeably with their definitions.

### 11.3 Static Semantics

#### 11.3.1 Well-Formedness Rules

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

#### 11.3.2 Type Rules

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

#### 11.3.3 Type Properties

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

### 11.4 Dynamic Semantics

#### 11.4.1 Evaluation Rules

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

#### 11.4.2 Memory Representation

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

#### 11.4.3 Operational Behavior

Type aliases do not change operational behavior. All operations on aliased types are identical to operations on their target types.

```cantrip
type Index = usize

let i: Index = 5
let j: usize = 10

let sum = i + j  // Same as: 5_usize + 10_usize
// No conversion, same operation
```

**Performance characteristic:** Type aliases have zero runtime cost. They are purely a compile-time abstraction that improves code readability without affecting performance.

### 11.5 Newtype Pattern

The **newtype pattern** uses single-field record types to create distinct nominal types. Unlike type aliases, newtypes are not transparent and provide type safety through nominal distinctions.

**Syntax:**
```cantrip
record NewtypeName(InnerType)
```

**Comparison with Type Aliases:**

| Feature | Type Alias | Newtype Pattern |
|---------|------------|----------------|
| **Type equivalence** | Transparent (`A ≡ τ`) | Distinct nominal type |
| **Type safety** | No protection | Prevents mixing similar types |
| **Trait implementations** | Inherited from target | Custom implementations possible |
| **Conversion** | Automatic (same type) | Explicit field access |
| **Use case** | Readability, abbreviation | Type safety, abstraction |

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

### 11.6 Generic Type Aliases

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

### 11.7 Examples and Patterns

#### 11.7.1 Domain-Specific Type Vocabulary

**Pattern:** Create a vocabulary of domain-specific types for clarity.

```cantrip
// Physics simulation
type Position = (f64, f64, f64)
type Velocity = (f64, f64, f64)
type Mass = f64
type Force = (f64, f64, f64)

record Particle {
    position: Position
    velocity: Velocity
    mass: Mass
}

function apply_force(p: Particle, f: Force, dt: f64) => Particle {
    let acceleration = (f.0 / p.mass, f.1 / p.mass, f.2 / p.mass)
    let new_velocity = (
        p.velocity.0 + acceleration.0 * dt,
        p.velocity.1 + acceleration.1 * dt,
        p.velocity.2 + acceleration.2 * dt
    )
    let new_position = (
        p.position.0 + new_velocity.0 * dt,
        p.position.1 + new_velocity.1 * dt,
        p.position.2 + new_velocity.2 * dt
    )

    Particle {
        position: new_position,
        velocity: new_velocity,
        mass: p.mass
    }
}
```

**Explanation:** Type aliases create a shared vocabulary that makes physics code self-documenting. Position, Velocity, and Force are all `(f64, f64, f64)` but the names communicate intent.

#### 11.7.2 Simplifying Complex Function Types

**Pattern:** Use type aliases to make function pointer types readable.

```cantrip
// Without aliases (verbose and unclear)
function process_data(
    data: Vec<i32>,
    transform: fn(i32) => f64,
    filter: fn(f64) => bool,
    fold: fn(f64, f64) => f64
) => f64 {
    data.iter()
        .map(transform)
        .filter(filter)
        .fold(0.0, fold)
}

// With aliases (clear and concise)
type Transform = fn(i32) => f64
type Filter = fn(f64) => bool
type Reducer = fn(f64, f64) => f64

function process_data(
    data: Vec<i32>,
    transform: Transform,
    filter: Filter,
    fold: Reducer
) => f64 {
    data.iter()
        .map(transform)
        .filter(filter)
        .fold(0.0, fold)
}

// Usage
let t: Transform = |x| x as f64 * 2.0
let f: Filter = |x| x > 0.0
let r: Reducer = |acc, x| acc + x

process_data(vec![1, 2, 3], t, f, r)
```

**Explanation:** Type aliases make function signatures more readable and easier to maintain. Changes to callback signatures only need to be made in one place.

#### 11.7.3 Result Type Specialization

**Pattern:** Create specialized Result types for different error contexts.

```cantrip
// Define error types for different subsystems
record IoError { message: String; }
record ParseError { line: usize; message: String; }
record NetworkError { code: i32; message: String; }

// Create specialized Result aliases
type IoResult<T> = Result<T, IoError>
type ParseResult<T> = Result<T, ParseError>
type NetworkResult<T> = Result<T, NetworkError>

// Functions use domain-specific result types
function read_file(path: String): IoResult<String>
    uses io.read
{
    // Implementation
    ...
}

function parse_config(content: String): ParseResult<Config> {
    // Implementation
    ...
}

function fetch_data(url: String): NetworkResult<String>
    uses io.network
{
    // Implementation
    ...
}

// Usage clearly indicates error context
let config_result = read_file("config.txt")
    .and_then(|content| parse_config(content))
```

**Explanation:** Specialized Result types make error handling more explicit and self-documenting. The function signature immediately tells you what kind of errors to expect.

#### 11.7.4 API Evolution with Type Aliases

**Pattern:** Use type aliases to create stable public APIs that can evolve internally.

```cantrip
// Public API (stable)
pub type ConnectionId = u64

// Internal implementation (can change)
record Connection {
    id: ConnectionId
    socket: TcpStream
    buffer: Vec<u8>
}

pub function connect(addr: String) => Result<ConnectionId, Error> {
    // Create connection
    let conn = Connection {
        id: generate_id(),
        socket: TcpStream::connect(addr)?,
        buffer: Vec::new()
    }

    // Store connection and return ID
    store_connection(conn)
    Ok(conn.id)
}

pub function send(id: ConnectionId, data: Vec<u8>) => Result<(), Error> {
    let conn = get_connection(id)?
    conn.socket.write(data)?
    Ok(())
}

// Later, can change ConnectionId implementation without breaking API
// pub type ConnectionId = (u64, u32)  // Add generation counter
// pub type ConnectionId = Uuid         // Switch to UUID
```

**Explanation:** The public API uses `ConnectionId`, allowing internal representation to change without breaking client code. This is more flexible than exposing the concrete type directly.

---

**Previous**: [Function Types](10_FunctionTypes.md) | **Next**: [Modals](08_Modals.md)
