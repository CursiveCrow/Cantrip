# Part III: Type System
## Section 3.0: Type System Foundations

**File**: `03-0_Type-Foundations.md`
**Version**: 2.0
**Status**: Normative
**Previous**: [Part II] | **Next**: [03-1_Subtyping-Equivalence.md](03-1_Subtyping-Equivalence.md)

---

## 3.0.1 Overview [type-foundations.overview]

[1] This section specifies the core typing infrastructure of Cursive, including type formation, well-formedness, type environments, equivalence relations, and the foundational properties that ensure type safety.

**Definition 3.0.1** (*Type System*):
The Cursive type system is a static, primarily nominal type system with:
- Structural typing for tuples and function types
- Bidirectional type inference
- Subtyping relation with explicit variance
- Integration with permission system (Part IV)
- Integration with grant system (Part X)

[2] A well-typed program shall not exhibit undefined behavior arising from type errors.

### 3.0.1.1 Type Categories [type-foundations.overview.categories]

[3] Cursive types shall be classified into the following categories:
    (3.1) **Primitive Types** (§3.2): Integer, floating-point, boolean, character, unit, never
    (3.2) **Composite Types** (§3.3): Product types (tuples, records), sum types (enums), union types
    (3.3) **Collection Types** (§3.4): Arrays, slices, strings, range types
    (3.4) **Function Types** (§3.5): Function and procedure types with grant annotations
    (3.5) **Pointer Types** (§3.6): Modal pointers with state annotations
    (3.6) **Type Aliases** (§3.7): Transparent and opaque type aliases
    (3.7) **Type Introspection** (§3.8): typeof operator and type predicates

> **Note**: The distinction between nominal and structural typing:
> - **Nominal types**: Records, enums, modals, predicates (identity by name)
> - **Structural types**: Tuples, arrays, primitives, slices, function types (identity by structure)

### 3.0.1.2 Cross-Part Dependencies [type-foundations.overview.dependencies]

[4] The type system integrates with other parts of the specification as follows:
    (4.1) **Part I (Foundations)**: Uses notation, metavariables, and judgment forms from §1.3
    (4.2) **Part II (Program Model)**: Builds on scopes (§2.5) and memory model overview (§2.8)
    (4.3) **Part IV (Memory Model and Permissions)**: Every typing judgment is parameterized by permission annotations
    (4.4) **Part VII (Declarations)**: Variable bindings, type declarations, scope rules
    (4.5) **Part X (Grant System)**: Function and procedure types carry explicit grant signatures
    (4.6) **Part XI (Contract System)**: Procedure types carry contract clauses

> **Note**: Metavariable Convention - This part uses type-specific metavariables (`τ, υ` for types; `α, β, γ` for type variables; `Γ` for type environments) as defined in §1.3.2.

---

## 3.0.2 Syntax [type-foundations.syntax]

[5] The syntax of types shall conform to the following grammar.

**Syntax:**
```
Type τ ::= PrimitiveType
         | CompositeType
         | CollectionType
         | FunctionType
         | PointerType
         | GenericType⟨τ₁, ..., τₙ⟩
         | τ₁ \/ τ₂              (union type)
         | Self                  (self type in predicate/modal)
         | α                     (type variable)

PrimitiveType ::= IntType | FloatType | bool | char | () | !

IntType ::= i8 | i16 | i32 | i64 | i128 | isize
          | u8 | u16 | u32 | u64 | u128 | usize

FloatType ::= f32 | f64

CollectionType ::= [τ; n] | [τ] | string | RangeType⟨T⟩

RangeType⟨T⟩ ::= Range⟨T⟩ | RangeInclusive⟨T⟩ | RangeFrom⟨T⟩
               | RangeTo⟨T⟩ | RangeToInclusive⟨T⟩ | RangeFull

CompositeType ::= TupleType | RecordType | EnumType

FunctionType ::= (τ₁, ..., τₙ) -> τ_ret GrantAnnotation?

GrantAnnotation ::= '!' GrantSet

PointerType ::= Ptr⟨τ⟩@State
```

> **Note**: Complete grammar in Annex A.2. Forward references: Detailed primitive types (§3.2), Composite types (§3.3), Collection types (§3.4), Function types (§3.5), Pointer types (§3.6), Generic types (Part IX).

---

## 3.0.3 Type Formation [type-foundations.formation]

[6] A type shall be well-formed when all its constructors are defined, type arguments match parameter arity, type bounds are satisfied, and all component types are well-formed.

**Definition 3.0.2** (*Well-Formed Type*):
The judgment $\Gamma \vdash \tau : \text{Type}$ holds when the conditions in paragraph [6] are satisfied.

### 3.0.3.1 Well-Formedness Rules [type-foundations.formation.wf-rules]

[7] The well-formedness rules for types shall be:

**Type Well-Formedness Rules:**

$$
{\small \dfrac{(\alpha : \kappa) \in \Gamma}{\Gamma \vdash \alpha : \text{Type}}}
\tag{WF-TypeVar}
$$

$$
{\small \dfrac{T \in \text{PrimTypes}}{\Gamma \vdash T : \text{Type}}}
\tag{WF-Prim}
$$

where $\text{PrimTypes} = \{\text{i8}, \text{i16}, \text{i32}, \text{i64}, \text{i128}, \text{isize}, \text{u8}, \text{u16}, \text{u32}, \text{u64}, \text{u128}, \text{usize}, \text{f32}, \text{f64}, \text{bool}, \text{char}, \text{string}, \text{!}, \text{()}\}$

$$
{\small \dfrac{\Gamma \vdash \tau_1 : \text{Type} \quad \cdots \quad \Gamma \vdash \tau_n : \text{Type} \quad n \geq 2}{\Gamma \vdash (\tau_1, \ldots, \tau_n) : \text{Type}}}
\tag{WF-Tuple}
$$

$$
{\small \dfrac{\Gamma \vdash \tau : \text{Type} \quad n \in \mathbb{N}^+}{\Gamma \vdash [\tau; n] : \text{Type}}}
\tag{WF-Array}
$$

$$
{\small \dfrac{\Gamma \vdash \tau : \text{Type}}{\Gamma \vdash [\tau] : \text{Type}}}
\tag{WF-Slice}
$$

$$
{\small \dfrac{\Gamma \vdash \tau_1 : \text{Type} \quad \cdots \quad \Gamma \vdash \tau_n : \text{Type} \quad \Gamma \vdash \tau_{\text{ret}} : \text{Type} \quad \varepsilon \text{ valid grant set}}{\Gamma \vdash (\tau_1, \ldots, \tau_n) \to \tau_{\text{ret}}\ !\ \varepsilon : \text{Type}}}
\tag{WF-Function}
$$

$$
{\small \dfrac{\Gamma \vdash \tau_1 : \text{Type} \quad \Gamma \vdash \tau_2 : \text{Type}}{\Gamma \vdash \tau_1 \lor \tau_2 : \text{Type}}}
\tag{WF-Union}
$$

$$
{\small \dfrac{\Gamma \vdash \text{ModalName} : \text{Type} \quad \text{@S is a valid state of ModalName}}{\Gamma \vdash \text{ModalName@S} : \text{Type}}}
\tag{WF-Modal}
$$

**Example 1**: Well-formed primitive types
```cursive
// All primitive types are well-formed
let x: const i32 = 42
let y: const f64 = 3.14
let z: const bool = true
```

**Example 2**: Well-formed composite types
```cursive
// Tuples: structural equivalence
let pair: const (i32, f64) = (42, 3.14)
let triple: const (i32, f64, bool) = (42, 3.14, true)

// Arrays
let arr: const [i32; 5] = [1, 2, 3, 4, 5]
let slice: const [i32] = arr[..]
```

[8] A conforming implementation shall reject any type that is not well-formed according to the rules in paragraph [7].

### 3.0.3.2 Grant Set Well-Formedness [type-foundations.formation.grant-wf]

[9] Grant sets shall be well-formed according to the following rules:

**Grant Set Well-Formedness:**

$$
{\small \dfrac{}{\emptyset \text{ valid grant set}}}
\tag{WF-Grant-Empty}
$$

$$
{\small \dfrac{\text{grant\_path is a declared grant}}{\text{grant\_path valid grant set}}}
\tag{WF-Grant-Single}
$$

$$
{\small \dfrac{\varepsilon_1 \text{ valid grant set} \quad \varepsilon_2 \text{ valid grant set}}{\varepsilon_1 \cup \varepsilon_2 \text{ valid grant set}}}
\tag{WF-Grant-Union}
$$

> **Forward reference**: Complete grant system specification in Part X.

### 3.0.3.3 Kinding Rules [type-foundations.formation.kinding]

[10] Kinds shall classify type-level expressions.

**Definition 3.0.3** (*Kind*):
Kinds classify type-level expressions. The kind system includes:

$$\kappa ::= \text{Type} \mid \text{Type} \to \kappa \mid \text{Eff} \mid \text{Region}$$

where:
- $\text{Type}$ denotes inhabited types
- $\text{Type} \to \kappa$ denotes type constructors
- $\text{Eff}$ denotes grant sets
- $\text{Region}$ denotes region lifetimes

[11] Kinding judgments shall determine the kind of type-level expressions:

**Kinding Rules:**

$$
{\small \dfrac{T \in \text{PrimTypes}}{T : \text{Type}}}
\tag{K-Type}
$$

$$
{\small \dfrac{T \text{ has arity } n \quad \kappa_1, \ldots, \kappa_n \text{ are parameter kinds} \quad \kappa_{\text{ret}} \text{ is result kind}}{T : \kappa_1 \to \cdots \to \kappa_n \to \kappa_{\text{ret}}}}
\tag{K-Constructor}
$$

$$
{\small \dfrac{\varepsilon \text{ is a grant set}}{\varepsilon : \text{Eff}}}
\tag{K-Grant}
$$

$$
{\small \dfrac{\Gamma \vdash F : \kappa_1 \to \kappa_2 \quad \Gamma \vdash T : \kappa_1}{\Gamma \vdash F\langle T \rangle : \kappa_2}}
\tag{K-Application}
$$

**Example 3**: Type constructor kinding
```cursive
// Array is a type constructor: Type -> Type
type IntArray = [i32; 10]

// Generic record is a type constructor: Type -> Type
record Container<T> { value: T }
let container: Container<i32> = Container { value: 42 }
```

---

## 3.0.4 Type Environments [type-foundations.environments]

[12] A type environment shall map term variables to types and type variables to kinds with optional bounds.

**Definition 3.0.4** (*Type Environment*):
A type environment (context) $\Gamma$ maps identifiers to types and kinds:

$$\Gamma ::= \emptyset \mid \Gamma, x : \tau \mid \Gamma, \alpha : \kappa \mid \Gamma, \alpha : \kappa \text{ where } \varphi \mid \Gamma, \text{grants } G$$

where:
- $\emptyset$ is the empty context
- $x : \tau$ is a term binding
- $\alpha : \kappa$ is a type variable binding
- $\alpha : \kappa \text{ where } \varphi$ is a constrained type parameter
- $\text{grants } G$ is a grant parameter binding
- $\varphi$ is a predicate bound of the form $\alpha : \text{PredicateName}$

### 3.0.4.1 Environment Operations [type-foundations.environments.operations]

[13] Environment lookup shall retrieve bindings:

**Lookup Rules:**

$$
{\small \dfrac{(x : \tau) \in \Gamma}{\Gamma(x) = \tau}}
\tag{Env-Lookup-Term}
$$

$$
{\small \dfrac{(\alpha : \kappa) \in \Gamma}{\Gamma(\alpha) = \kappa}}
\tag{Env-Lookup-Type}
$$

[14] Environment extension shall add new bindings:

**Extension Rule:**

$$
{\small \dfrac{x \notin \text{dom}(\Gamma)}{\Gamma, x : \tau \text{ extends } \Gamma}}
\tag{Env-Extend}
$$

[15] A new binding $x : \tau$ may shadow a previous binding of $x$ in an outer scope. The most recent binding shall take precedence.

[16] Within a single scope, a conforming implementation shall reject duplicate bindings of the same identifier. Shadowing is permitted only across nested scopes.

### 3.0.4.2 Well-Formed Environments [type-foundations.environments.wf]

[17] An environment $\Gamma$ shall be well-formed when:
    (17.1) Each term variable appears at most once in the current scope
    (17.2) Each type variable appears at most once in the current scope
    (17.3) All types in $\Gamma$ are well-formed under $\Gamma$ itself
    (17.4) All bounds in $\Gamma$ are satisfied

**Definition 3.0.5** (*Well-Formed Environment*):
An environment $\Gamma$ is well-formed when the conditions in paragraph [17] hold.

**Well-Formed Environment Rules:**

$$
{\small \dfrac{}{\vdash \emptyset\ \text{ok}}}
\tag{WF-Env-Empty}
$$

$$
{\small \dfrac{\vdash \Gamma\ \text{ok} \quad \Gamma \vdash \tau : \text{Type} \quad x \notin \text{dom}(\Gamma)}{\vdash \Gamma, x : \tau\ \text{ok}}}
\tag{WF-Env-Term}
$$

$$
{\small \dfrac{\vdash \Gamma\ \text{ok} \quad \alpha \notin \text{dom}(\Gamma)}{\vdash \Gamma, \alpha : \kappa\ \text{ok}}}
\tag{WF-Env-TypeVar}
$$

$$
{\small \dfrac{\vdash \Gamma\ \text{ok} \quad \Gamma, \alpha : \kappa \vdash \varphi \text{ valid} \quad \alpha \notin \text{dom}(\Gamma)}{\vdash \Gamma, \alpha : \kappa \text{ where } \varphi\ \text{ok}}}
\tag{WF-Env-Bounded}
$$

Note: The condition $x \notin \text{dom}(\Gamma)$ applies within the current scope.

**Example 4**: Type environment evolution
```cursive
// Initial environment: ∅
{
    // Environment: Γ = { x : i32 }
    let x: const i32 = 42

    {
        // Environment: Γ' = Γ, { y : f64 }
        let y: const f64 = 3.14

        // Inner scope can shadow outer binding
        let x: const f64 = 1.0  // OK: shadows outer x

        // x here refers to the inner binding (f64)
    }

    // x here refers to the outer binding (i32)
}
```

---

## 3.0.5 Type Equality and Equivalence [type-foundations.equivalence]

[18] Types shall be equivalent when they denote the same type under the applicable equivalence relation.

**Definition 3.0.6** (*Type Equivalence*):
Types $\tau_1$ and $\tau_2$ are equivalent ($\tau_1 \equiv \tau_2$) when they denote the same type after expanding aliases, substituting generic arguments, and normalizing associated types.

[19] Cursive shall employ:
    (19.1) **Nominal equivalence** for records, enums, modals, predicates, and contracts
    (19.2) **Structural equivalence** for tuples, arrays, slices, primitives, and function types

### 3.0.5.1 Equivalence Rules [type-foundations.equivalence.rules]

[20] Type equivalence shall satisfy reflexivity, symmetry, and transitivity:

**Basic Equivalence Rules:**

$$
{\small \dfrac{}{\tau \equiv \tau}}
\tag{Equiv-Refl}
$$

$$
{\small \dfrac{\tau_1 \equiv \tau_2}{\tau_2 \equiv \tau_1}}
\tag{Equiv-Sym}
$$

$$
{\small \dfrac{\tau_1 \equiv \tau_2 \quad \tau_2 \equiv \tau_3}{\tau_1 \equiv \tau_3}}
\tag{Equiv-Trans}
$$

$$
{\small \dfrac{\text{type } A = \tau}{A \equiv \tau}}
\tag{Equiv-Alias}
$$

$$
{\small \dfrac{\text{type } F\langle\alpha\rangle = \tau[\alpha]}{F\langle\upsilon\rangle \equiv \tau[\alpha \mapsto \upsilon]}}
\tag{Equiv-Generic-Alias}
$$

### 3.0.5.2 Nominal Equivalence [type-foundations.equivalence.nominal]

[21] Two nominal types shall be equivalent if and only if they have the same name and were declared by the same type declaration.

**Definition 3.0.7** (*Nominal Equivalence*):
For nominal types, equivalence is based on type identity, not structure.

**Example 5**: Nominal types with identical structure are not equivalent (invalid)
```cursive
record Point { x: f64, y: f64 }
record Vector { x: f64, y: f64 }

let p: const Point = Point { x: 1.0, y: 2.0 }
// let v: Vector = p  // ERROR: Point ≢ Vector
```

### 3.0.5.3 Structural Equivalence [type-foundations.equivalence.structural]

[22] Two structural types shall be equivalent if and only if they have the same structure.

**Definition 3.0.8** (*Structural Equivalence*):
For structural types, equivalence is based on the structure of the type, not its name.

**Structural Equivalence Rules:**

$$
{\small \dfrac{\tau_1 \equiv \upsilon_1 \quad \cdots \quad \tau_n \equiv \upsilon_n}{(\tau_1, \ldots, \tau_n) \equiv (\upsilon_1, \ldots, \upsilon_n)}}
\tag{Equiv-Tuple}
$$

$$
{\small \dfrac{\tau \equiv \upsilon \quad n = m}{[\tau; n] \equiv [\upsilon; m]}}
\tag{Equiv-Array}
$$

$$
{\small \dfrac{\tau \equiv \upsilon}{[\tau] \equiv [\upsilon]}}
\tag{Equiv-Slice}
$$

$$
{\small \dfrac{\tau_1 \equiv \upsilon_1 \quad \cdots \quad \tau_n \equiv \upsilon_n \quad \tau_{\text{ret}} \equiv \upsilon_{\text{ret}} \quad \varepsilon_1 = \varepsilon_2}{(\tau_1, \ldots, \tau_n) \to \tau_{\text{ret}}\ !\ \varepsilon_1 \equiv (\upsilon_1, \ldots, \upsilon_n) \to \upsilon_{\text{ret}}\ !\ \varepsilon_2}}
\tag{Equiv-Function}
$$

**Example 6**: Structural type equivalence
```cursive
// Tuples: structurally equivalent
let t1: const (i32, f64) = (42, 3.14)
let t2: const (i32, f64) = (100, 2.71)
// t1 and t2 have the same type

// Function types: structurally equivalent
type BinaryOp = (i32, i32) -> i32
function add(a: i32, b: i32): i32 { result a + b }
let op: const BinaryOp = add  // OK: types equivalent
```

> **Forward reference**: Detailed subtyping and equivalence rules in §3.1.

---

## 3.0.6 Subtyping Overview [type-foundations.subtyping]

[23] The subtyping relation determines when one type can safely substitute for another.

**Definition 3.0.9** (*Subtyping Relation*):
The subtyping relation $\tau <: \upsilon$ (read "$\tau$ is a subtype of $\upsilon$") indicates that a value of type $\tau$ can be used safely wherever a value of type $\upsilon$ is expected.

[24] The subtyping relation shall be reflexive and transitive.

**Basic Subtyping Properties:**

$$
{\small \dfrac{}{\tau <: \tau}}
\tag{Sub-Refl}
$$

$$
{\small \dfrac{\tau_1 <: \tau_2 \quad \tau_2 <: \tau_3}{\tau_1 <: \tau_3}}
\tag{Sub-Trans}
$$

$$
{\small \dfrac{}{! <: \tau}}
\tag{Sub-Never}
$$

[25] The never type $!$ shall be the bottom type, a subtype of all types.

**Example 7**: Never type as bottom type
```cursive
function diverge(): ! {
    loop {
        // Never returns
    }
}

function example(): i32 {
    diverge()  // OK: ! <: i32
}
```

> **Forward reference**: Complete subtyping rules and variance in §3.1.

---

## 3.0.7 Type Safety Properties [type-foundations.safety]

[26] The type system shall guarantee type safety through progress and preservation properties.

**Theorem 3.0.1** (*Type Safety*):
If a program is well-typed, then it shall not exhibit undefined behavior arising from type errors.

*Proof*: By progress and preservation theorems. See Annex C.1 for complete proof.

### 3.0.7.1 Progress [type-foundations.safety.progress]

[27] If an expression is well-typed, it shall either evaluate to a value or diverge.

**Theorem 3.0.2** (*Progress*):
If $\emptyset \vdash e : \tau$, then either:
  (a) $e$ is a value, or
  (b) $e \Downarrow e'$ for some $e'$, or
  (c) $e$ diverges

### 3.0.7.2 Preservation [type-foundations.safety.preservation]

[28] If a well-typed expression evaluates, the result shall preserve the type.

**Theorem 3.0.3** (*Preservation*):
If $\Gamma \vdash e : \tau$ and $e \Downarrow e'$, then $\Gamma \vdash e' : \tau$.

### 3.0.7.3 Parametricity [type-foundations.safety.parametricity]

[29] Generic functions shall preserve type abstraction.

**Theorem 3.0.4** (*Parametricity*):
For all well-typed generic functions $f\langle\alpha\rangle$, the behavior of $f$ shall be uniform across all instantiations of $\alpha$.

*Interpretation*: Functions cannot inspect or depend on the specific type argument.

### 3.0.7.4 Integration with Permission System [type-foundations.safety.permissions]

[30] Type safety shall integrate with the permission system to ensure memory safety.

[31] The typing judgment shall be extended with permission annotations:

$$\Gamma; \Pi \vdash e : \tau\ @\ p$$

where:
- $\Gamma$ is the type environment
- $\Pi$ is the permission context
- $\tau$ is the type
- $p$ is the permission (const, shared, unique)

> **Forward reference**: Complete permission system in Part IV §4.2.

### 3.0.7.5 Integration with Grant System [type-foundations.safety.grants]

[32] Type safety shall integrate with the grant system to ensure effect safety.

[33] Function and procedure types shall carry explicit grant annotations:

$$(\tau_1, \ldots, \tau_n) \to \tau_{\text{ret}}\ !\ \varepsilon$$

where $\varepsilon$ is the grant set required by the function.

> **Forward reference**: Complete grant system in Part X.

---

## 3.0.8 Integration [type-foundations.integration]

[34] The type system foundations integrate with the following language components:

**Cross-references:**
- Detailed subtyping and equivalence: §3.1
- Primitive types: §3.2
- Composite types: §3.3
- Collection types: §3.4
- Function types: §3.5
- Pointer types: §3.6
- Type aliases: §3.7
- Type introspection: §3.8
- Generic types and parametric polymorphism: Part IX
- Predicate system and type constraints: Part VIII
- Permission system: Part IV §4.2
- Grant system: Part X
- Contract system: Part XI
- Memory model: Part IV
- Scopes and name resolution: §2.5, Part VII

[35] The formal semantics specified in this section shall be elaborated in Annex C (Formal Semantics), which provides complete typing rules, operational semantics, and meta-theoretic proofs.

---

**Previous**: [Part II] | **Next**: [03-1_Subtyping-Equivalence.md](03-1_Subtyping-Equivalence.md)
