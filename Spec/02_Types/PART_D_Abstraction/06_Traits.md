# Chapter 2: Type System - §2.6. Traits

**Section**: §2.6 | **Part**: Type System (Part II)
**Previous**: [Pointers](../PART_C_References/05_Pointers.md) | **Next**: [Generics](07_Generics.md)

---

**Definition 6.1 (Trait):** A trait is a concrete, reusable implementation template that provides procedures for types. Traits are composition units (mixins) that types can **attach** to gain their implementations. Formally, a trait `T` defines procedures `{m₁, ..., mₙ}` with concrete implementations using `Self` to be generic over the attaching type.

## 2.6. Traits

### 2.6.1 Overview

**Key innovation/purpose:** Traits enable code reuse through composition rather than inheritance. A trait provides concrete procedure implementations that can be attached to any compatible type, similar to mixins or default interface implementations. This promotes horizontal composition and reduces code duplication.

**When to use traits:**

- Sharing common implementations across multiple unrelated types
- Providing utility procedures that work generically
- Composing behavior from reusable building blocks
- Creating libraries of attachable functionality
- Avoiding code duplication for similar operations
- Building layered abstractions through composition

**When NOT to use traits:**

- Abstract contracts without implementation → use protocols (§TBD)
- Type-specific behavior that doesn't generalize → use regular procedures
- Simple data containers → use records (§8)
- Single-use implementations → write procedures directly
- When inheritance semantics are needed → Cantrip uses composition instead

**Relationship to other features:**

- **Records (§8):** Can attach traits to gain procedures
- **Enums (§9):** Can attach traits to gain procedures
- **Generics (§7):** Traits use `Self` as a generic type parameter
- **Procedures (§10.6):** Trait procedures become type procedures when attached
- **Protocols (§TBD):** Protocols define contracts; traits provide implementations
- **Self Type (§12):** Traits use `Self` to refer to the attaching type

**Trait vs Protocol:**

- **Trait** = concrete implementation you attach (mixin)
- **Protocol** = abstract behavioral contract you fulfill (interface)

### 2.6.2 Syntax

#### 2.6.2.1 Concrete Syntax

**Trait declaration:**

```ebnf
TraitDecl    ::= "public"? "trait" Ident GenericParams? RequiredMethods? "{" TraitProcedure* "}"
RequiredMethods ::= "requires" "{" ProcedureSignature+ "}"
TraitProcedure ::= "procedure" Ident "(" ParamList ")" (":" Type)? NeedsClause? ContractClauses? Block
```

**Trait attachment:**

```ebnf
RecordDecl   ::= "record" Ident GenericParams? AttachClause? "{" Fields "}"
AttachClause ::= "attach" TraitList
TraitList    ::= Ident ("," Ident)*
```

**Examples:**

```cantrip
// Trait providing concrete implementations
trait Debug {
    procedure debug($): String
        uses alloc.heap
    {
        format!("{}@{:p}", Self::type_name(), $)
    }

    procedure print_debug($)
        uses io.write, alloc.heap
    {
        println("{}", $.debug())
    }
}

// Trait with required procedure
trait Display
    must {
        procedure fmt($): String
    }
{
    procedure display($)
        uses io.write, alloc.heap
    {
        println("{}", $.fmt())
    }

    procedure to_string($): String
        uses alloc.heap
    {
        $.fmt()
    }
}

// Trait with generic Self
trait Clone {
    procedure clone($): own Self
        uses alloc.heap
    {
        // Generic clone implementation
        Self::deep_copy($)
    }
}

// Record attaching traits
record Point attach Debug, Clone {
    x: f64
    y: f64
}

// Point now has debug(), print_debug(), and clone() procedures
```

#### 2.6.2.2 Abstract Syntax

**Trait declaration:**

```
D ::= trait I must {sig₁, ..., sigₖ} { m₁ = e₁; ...; mₙ = eₙ }

where:
  I = trait identifier
  sig = required procedure signature (abstract)
  m = procedure name
  e = procedure implementation (concrete)
```

**Trait attachment:**

```
D ::= record T attach {I₁, ..., Iₙ} { fields }

Desugars to:
  record T { fields }
  + procedures from I₁
  + procedures from I₂
  + ...
  + procedures from Iₙ
```

**Trait composition:**

```
record T attach {I₁, I₂} { ... }

Procedures available on T:
  T_procedures ∪ I₁_procedures ∪ I₂_procedures

With `Self` in each trait substituted by `T`:
  Self ↦ T
```

#### 2.6.2.3 Basic Examples

**Example 1: Simple trait**

```cantrip
trait Identifiable {
    procedure id($): u64 {
        std.ptr.addr_of($) as u64
    }

    procedure is_same($, other: Self): bool {
        $.id() == other.id()
    }
}

record Counter attach Identifiable {
    value: i32
}

// Usage
let c1 = Counter { value: 0 }
let c2 = Counter { value: 0 }
println("{}", c1.id())              // 140736... (memory address)
println("{}", c1.is_same(c2))      // false (different objects)
```

**Example 2: Trait requiring procedures**

```cantrip
trait Comparable
    must {
        procedure compare($, other: Self): i32
    }
{
    procedure less_than($, other: Self): bool {
        $.compare(other) < 0
    }

    procedure greater_than($, other: Self): bool {
        $.compare(other) > 0
    }

    procedure equals($, other: Self): bool {
        $.compare(other) == 0
    }
}

record Point attach Comparable {
    x: i32
    y: i32

    // Must provide required procedure
    procedure compare($, other: Self): i32 {
        if $.x != other.x {
            $.x - other.x
        } else {
            $.y - other.y
        }
    }
}

// Usage
let p1 = Point { x: 1, y: 2 }
let p2 = Point { x: 3, y: 4 }
println("{}", p1.less_than(p2))     // true (from trait)
println("{}", p1.equals(p2))        // false (from trait)
```

**Example 3: Trait with effects**

```cantrip
trait Serializable {
    procedure save($, path: String): Result<(), Error>
        uses io.write, alloc.heap
    {
        let data = $.to_json()
        std.fs.write(path, data)
    }

    procedure load(path: String): Result<own Self, Error>
        uses io.read, alloc.heap
    {
        let data = std.fs.read(path)?
        Self::from_json(data)
    }
}

record Config attach Serializable {
    host: String
    port: u16

    procedure to_json($): String
        uses alloc.heap
    {
        format!("{{\"host\":\"{}\",\"port\":{}}}", $.host, $.port)
    }

    procedure from_json(json: String): own Self
        uses alloc.heap
    {
        // Parse JSON and return Config
        ...
    }
}
```

### 2.6.3 Static Semantics

#### 2.6.3.1 Well-Formedness Rules

**[WF-Trait] Well-formed trait:**

```
[WF-Trait]
procedures m₁, ..., mₙ well-formed
each procedure uses Self consistently
required signatures sig₁, ..., sigₖ well-formed
procedure names distinct
────────────────────────────────────────
trait I must {sig₁, ..., sigₖ} { m₁, ..., mₙ } well-formed
```

**Explanation:** A trait is well-formed if all its procedures type-check with `Self` as a type variable, all required signatures are valid, and there are no name conflicts.

**[WF-TraitAttach] Well-formed trait attachment:**

```
[WF-TraitAttach]
record T { fields } well-formed
traits I₁, ..., Iₙ well-formed
∀j ∈ {1..n}. T provides required procedures for Iⱼ
no conflicting procedure names across traits
────────────────────────────────────────────────
record T attach {I₁, ..., Iₙ} { fields } well-formed
```

**Explanation:** A type can attach traits if it provides all required procedures and there are no naming conflicts.

**[WF-RequiredProc] Required procedure satisfaction:**

```
[WF-RequiredProc]
trait I must { procedure m(Self, τ₁, ..., τₙ): τ }
record T attach I { ... }
T has procedure m(Self, τ₁, ..., τₙ): τ
────────────────────────────────────────────────
T satisfies requirement for I
```

**Explanation:** Types must provide procedures matching required signatures.

**[WF-NoConflict] No procedure name conflicts:**

```
[WF-NoConflict]
record T attach {I₁, I₂} { ... }
I₁ provides procedure m
I₂ provides procedure n
m ≠ n
────────────────────────────────────────────────
no conflict
```

**Explanation:** Attached traits cannot provide procedures with the same name unless explicitly resolved.

#### 2.6.3.2 Type Rules

**[T-TraitProc] Trait procedure typing:**

```
[T-TraitProc]
trait I { procedure m(Self, τ₁, ..., τₙ): τ { body } }
Γ, Self : Type ⊢ body : τ
────────────────────────────────────────────────
Γ ⊢ trait I well-typed
```

**Explanation:** Trait procedures type-check with `Self` as an abstract type parameter.

**[T-AttachProc] Attached procedure typing:**

```
[T-AttachProc]
trait I { procedure m($, params): τ { body } }
record T attach I { fields }
────────────────────────────────────────────────
Γ ⊢ T.m : (Self = T, params) → τ
```

**Explanation:** When a trait is attached, its procedures become available on the type with `Self` substituted.

**[T-ProcCall] Procedure call via trait:**

```
[T-ProcCall]
record T attach I { ... }
trait I { procedure m($): τ }
Γ ⊢ obj : T
────────────────────────────────────────────────
Γ ⊢ obj.m() : τ
```

**Explanation:** Procedures from attached traits are callable on values of the type.

**[T-RequiredProc] Required procedure call:**

```
[T-RequiredProc]
trait I must { procedure req($): τ }
  { procedure m($): U { ... $.req() ... } }
record T attach I { ... procedure req($): τ { ... } }
────────────────────────────────────────────────
Γ ⊢ T.m calls T.req
```

**Explanation:** Trait procedures can call required procedures, which are resolved to the type's implementation.

#### 2.6.3.3 Type Properties

**Theorem 6.1 (Trait Instantiation):**

When a trait is attached to a type, all trait procedures are instantiated with `Self` bound to the concrete type:

```
trait I { procedure m(Self, τ): U }
record T attach I { ... }
─────────────────────────────────
T.m : (T, τ) → U    (where Self = T)
```

**Proof sketch:** By trait attachment desugaring. The compiler instantiates each trait procedure with the concrete type substituted for `Self`.

**Theorem 6.2 (Required Procedure Satisfaction):**

A type can only attach a trait if it provides all required procedures:

```
trait I must { procedure req($): τ }
record T attach I { ... }
─────────────────────────────────
T must provide: procedure req($): τ
```

**Proof sketch:** By [WF-RequiredProc]. Type checker verifies all requirements before allowing attachment.

**Theorem 6.3 (Trait Composition):**

Multiple traits can be attached if their procedure names don't conflict:

```
trait I₁ { procedures M₁ }
trait I₂ { procedures M₂ }
M₁ ∩ M₂ = ∅
record T attach I₁, I₂ { ... }
─────────────────────────────────
T has procedures: M₁ ∪ M₂
```

**Proof sketch:** By [WF-NoConflict] and trait desugaring. Non-conflicting procedures are composed.

**Property 6.1 (Static Dispatch):**

All trait procedure calls use static dispatch (no vtables):

```cantrip
record T attach I { ... }
let obj = T { ... }
obj.m()  // Resolved at compile time to T's implementation
```

**Property 6.2 (Zero-Cost Abstraction):**

Trait attachment has zero runtime overhead. Procedures are monomorphized at compile time, identical to writing them directly in the type.

**Property 6.3 (Coherence):**

Each type has exactly one set of attached trait procedures. There is no ambiguity in procedure resolution.

### 2.6.4 Dynamic Semantics

#### 2.6.4.1 Evaluation Rules

**[E-TraitProc] Trait procedure call:**

```
[E-TraitProc]
record T attach I { ... }
trait I { procedure m($, params): τ { body } }
⟨obj, σ⟩ ⇓ ⟨v_obj, σ₁⟩
⟨body[Self ↦ T, $ ↦ v_obj, params ↦ args], σ₁⟩ ⇓ ⟨v_result, σ₂⟩
────────────────────────────────────────────────────────────────
⟨obj.m(args), σ⟩ ⇓ ⟨v_result, σ₂⟩
```

**Explanation:** Calling a trait procedure evaluates the trait's implementation with `Self` bound to the concrete type.

**[E-RequiredProc] Required procedure dispatch:**

```
[E-RequiredProc]
record T attach I { procedure req($): τ { body_T } }
trait I must { procedure req($): τ }
  { procedure m($): U { ... $.req() ... } }
⟨obj.m(), σ⟩
  ⇝ evaluate trait body with $.req() calling body_T
────────────────────────────────────────────────────
⟨obj.m(), σ⟩ ⇓ ⟨result, σ'⟩
```

**Explanation:** When trait procedures call required procedures, the call dispatches to the type's implementation.

#### 2.6.4.2 Memory Representation

**Trait procedures:**

Traits have **no runtime representation**. All trait procedures are monomorphized at compile time.

```
trait I { procedure m($): i32 { $.value + 1 } }

record T attach I {
    value: i32
}

Memory layout of T:
┌─────────────┐
│ value: i32  │  (4 bytes)
└─────────────┘

No trait metadata stored.
```

**Procedure resolution:**

All trait procedure calls are resolved statically:

```cantrip
// Source
let obj = T { value: 5 }
obj.m()

// After monomorphization (conceptual)
let obj = T { value: 5 }
T_m_from_I(obj)  // Direct function call

// Compiled (pseudo-assembly)
mov rdi, [obj]     ; load obj pointer
call T_m           ; direct call, no indirection
```

**Attachment compilation:**

```cantrip
// Source
record Point attach Debug, Clone {
    x: i32
    y: i32
}

// Conceptual desugaring
record Point {
    x: i32
    y: i32
}

// Debug trait procedures monomorphized for Point
procedure Point_debug(self: Point): String { ... }
procedure Point_print_debug(self: Point) { ... }

// Clone trait procedures monomorphized for Point
procedure Point_clone(self: Point): own Point { ... }
```

#### 2.6.4.3 Operational Behavior

**Example: Trait attachment and usage**

```cantrip
trait Incrementable {
    procedure increment(mut $) {
        $.value += 1
    }

    procedure add(mut $, n: i32) {
        for _ in 0..n {
            $.increment()
        }
    }
}

record Counter attach Incrementable {
    value: i32
}

// Evaluation trace
let mut c = Counter { value: 0 }
c.add(3)

⇝ Counter { value: 0 }
⇝ c.add(3) where add is from Incrementable
⇝ for _ in 0..3 { c.increment(); }
⇝ c.value += 1  (iteration 1)
⇝ Counter { value: 1 }
⇝ c.value += 1  (iteration 2)
⇝ Counter { value: 2 }
⇝ c.value += 1  (iteration 3)
⇝ Counter { value: 3 }
```

### 2.6.5 Trait Patterns

#### 2.6.5.1 Utility Traits

**Example: Debug trait**

```cantrip
trait Debug {
    procedure type_name(): String {
        // Compiler intrinsic
        intrinsic::type_name::<Self>()
    }

    procedure debug($): String
        uses alloc.heap
    {
        format!("{}{{ ... }}", Self::type_name())
    }

    procedure print_debug($)
        uses io.write, alloc.heap
    {
        println("{}", $.debug())
    }
}

// Attach to any type
record Config attach Debug {
    host: String
    port: u16
}

let cfg = Config { host: "localhost", port: 8080 }
cfg.print_debug()  // Output: Config{ ... }
```

#### 2.6.5.2 Builder Pattern Trait

```cantrip
trait Builder
    must {
        procedure new(): own Self
    }
{
    procedure with_default(): own Self {
        Self::new()
    }

    procedure build_multiple(count: usize): Vec<own Self>
        uses alloc.heap
    {
        var result = Vec::with_capacity(count)
        for _ in 0..count {
            result.push(Self::new())
        }
        result
    }
}

record Connection attach Builder {
    socket: Socket

    procedure new(): own Self {
        own Self { socket: Socket::new() }
    }
}

// Usage
let connections = Connection::build_multiple(10)
```

#### 2.6.5.3 Default Values Trait

```cantrip
trait Default
    must {
        procedure default(): own Self
    }
{
    procedure or_default(opt: Option<Self>): own Self {
        match opt {
            Some(val) => val,
            None => Self::default(),
        }
    }

    procedure array<const N: usize>(): [Self; N]
        where N > 0
    {
        [Self::default(); N]
    }
}

record Config attach Default {
    max_connections: i32

    procedure default(): own Self {
        own Self { max_connections: 100 }
    }
}

// Usage
let cfg = Config::or_default(None)
let cfgs: [Config; 5] = Config::array()
```

#### 2.6.5.4 Comparison Trait

```cantrip
trait Comparable
    must {
        procedure compare($, other: Self): i32
    }
{
    procedure less_than($, other: Self): bool {
        $.compare(other) < 0
    }

    procedure less_equal($, other: Self): bool {
        $.compare(other) <= 0
    }

    procedure greater_than($, other: Self): bool {
        $.compare(other) > 0
    }

    procedure greater_equal($, other: Self): bool {
        $.compare(other) >= 0
    }

    procedure equals($, other: Self): bool {
        $.compare(other) == 0
    }

    procedure clamp($, min: Self, max: Self): Self {
        if $.less_than(min) {
            min
        } else if $.greater_than(max) {
            max
        } else {
            $
        }
    }
}

record Score attach Comparable {
    value: i32

    procedure compare($, other: Self): i32 {
        $.value - other.value
    }
}
```

#### 2.6.5.5 Conversion Trait

```cantrip
trait ToBytes
    must {
        procedure to_bytes($): Vec<u8>
    }
{
    procedure to_hex($): String
        uses alloc.heap
    {
        let bytes = $.to_bytes()
        bytes.iter()
            .map(|b| format!("{:02x}", b))
            .collect()
    }

    procedure byte_count($): usize {
        $.to_bytes().len()
    }
}

record Message attach ToBytes {
    data: String

    procedure to_bytes($): Vec<u8>
        uses alloc.heap
    {
        $.data.as_bytes().to_vec()
    }
}
```

### 2.6.6 Advanced Features

#### 2.6.6.1 Trait Dependencies

Traits can use other traits:

```cantrip
trait Display {
    procedure to_string($): String
        uses alloc.heap
}

trait Loggable
    must {
        procedure level($): LogLevel
    }
{
    // Can require that Self has Display
    procedure log($)
        where Self: Display
        uses io.write, alloc.heap
    {
        println("[{}] {}", $.level(), $.to_string())
    }
}
```

#### 2.6.6.2 Parameterized Traits

Traits can be generic:

```cantrip
trait Convertible<T> {
    procedure convert($): T

    procedure try_convert($): Result<T, String> {
        Ok($.convert())
    }
}

record Number attach Convertible<String> {
    value: i32

    procedure convert($): String
        uses alloc.heap
    {
        $.value.to_string()
    }
}
```

#### 2.6.6.3 Conditional Trait Procedures

Procedures can have trait bounds:

```cantrip
trait Container {
    procedure print_all($)
        where Self: Iterator, Self::Item: Display
        uses io.write, alloc.heap
    {
        for item in $ {
            println("{}", item.to_string())
        }
    }
}
```

### 2.6.7 Trait vs Protocol Comparison

| Aspect             | Trait                           | Protocol                                 |
| ------------------ | ------------------------------- | ---------------------------------------- |
| **Nature**         | Concrete implementation (mixin) | Abstract contract (interface)            |
| **Provides**       | Working code                    | Signatures only                          |
| **Keyword**        | `trait` with implementations    | `protocol` (to be defined)               |
| **Relationship**   | "attach" to types               | "implement" by types                     |
| **Purpose**        | Code reuse through composition  | Define behavioral requirements           |
| **Required procs** | Can require some procedures     | All procedures are required              |
| **Examples**       | `Debug`, `Clone`, `Default`     | `Comparable`, `Serializable` (contracts) |

**Example showing both:**

```cantrip
// Protocol (abstract contract)
protocol Drawable {
    procedure draw($, canvas: Canvas)
    procedure bounds($): Rectangle
}

// Trait (concrete implementation)
trait Cacheable
    must {
        procedure compute_hash($): u64
    }
{
    procedure with_cache($, cache: mut Cache): Result<T> {
        let hash = $.compute_hash()
        if let Some(cached) = cache.get(hash) {
            return Ok(cached)
        }
        // ... compute and cache
    }
}

// Type implements protocol, attaches trait
record Shape : Drawable attach Cacheable {
    ...
}
```

---

## 2.6.8 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - traits can be attached to primitive type wrappers
- See §2.2 for [Product Types](../PART_B_Composite/02_ProductTypes.md) - records as primary recipients of trait attachments
- See §2.3 for [Sum Types](../PART_B_Composite/03_SumTypes.md) - enums can attach traits
- See §2.7 for [Generics](07_Generics.md) - traits use Self type and generic parameters
- See §2.10 for [Self Type](../PART_E_Utilities/10_SelfType.md) - the Self keyword in trait definitions
- See Part VI (Procedures) for procedure definitions and trait methods
- See Part VIII (Modules) for trait visibility and imports
- **Examples**: See [06_TraitExamples.md](../Examples/06_TraitExamples.md) for practical trait patterns

## 2.6.9 Notes and Warnings

**Note 1 (Trait as Mixin):** Traits provide concrete implementations (mixins), not abstract interfaces. For abstract contracts, use protocols (when available) or trait bounds on generic parameters.

**Note 2 (Self Type):** The `Self` keyword in trait definitions refers to the type that will attach the trait. It enables generic implementations that work across all attaching types.

**Note 3 (Trait Attachment):** Types attach traits using `attach TraitName` syntax. Multiple traits can be attached by separating with commas: `attach Trait1, Trait2, Trait3`.

**Note 4 (No Override):** When a type attaches a trait, it gains all trait procedures. Types cannot selectively override individual trait procedures - it's all or nothing.

**Note 5 (Trait Bounds):** Generic type parameters can require traits: `procedure foo<T: Display>(value: T)` requires T to have Display trait attached.

**Note 6 (Composition Over Inheritance):** Cantrip uses trait composition instead of class inheritance. Complex behaviors are built by composing multiple simple traits rather than creating deep inheritance hierarchies.

**Warning 1 (Name Conflicts):** If a type defines a procedure with the same name as an attached trait's procedure, there is a name conflict. Cantrip resolves this through explicit qualification or reports a compilation error.

**Warning 2 (Coherence):** Trait implementations must follow coherence rules - you can only attach a trait to a type if you defined either the trait or the type in your crate. This prevents conflicting attachments from multiple libraries.

**Warning 3 (No Partial Attachment):** You cannot attach only some procedures from a trait. Attachment is all-or-nothing - the entire trait implementation comes with attachment.

**Performance Note 1 (Static Dispatch):** Trait procedures attached to concrete types use static dispatch (direct function calls). No runtime overhead compared to regular procedures.

**Performance Note 2 (Monomorphization):** When traits are used with generic parameters, the compiler generates specialized code for each concrete type, enabling full optimization with zero abstraction cost.

**Implementation Note 1 (Trait Objects):** While this specification covers trait attachment to types, Cantrip may support trait objects (dynamic dispatch) through separate mechanisms. See implementation documentation.

**Implementation Note 2 (Standard Traits):** The standard library provides fundamental traits like `Copy`, `Clone`, `Debug`, `Display`, `Default` that types commonly attach for standard behaviors.

---

**Previous**: [Pointers](../PART_C_References/05_Pointers.md) | **Next**: [Generics](07_Generics.md)
