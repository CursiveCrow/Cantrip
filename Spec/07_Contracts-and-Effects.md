# The Cursive Language Specification

**Part**: VII - Contracts and Effects  
**File**: 07_Contracts-and-Effects.md  
**Previous**: [Statements and Control Flow](06_Statements-and-Control-Flow.md) | **Next**: [Holes and Inference](08_Holes-and-Inference.md)

## Abstract

Part VII specifies Cursive’s behavioral contract system: contract declarations, effect clauses (`uses`), preconditions (`must`), postconditions (`will`), and type invariants (`where`). It defines the grammar that extends Appendix A, the static and dynamic semantics of each clause, contract composition rules, and the diagnostics that enforce them.

Notation update: Function types in this part use arrow syntax `(T₁, …, Tₙ) → U ! ε` (replacing the earlier `map(T₁, …, Tₙ) → U ! ε` alias). Where examples still show `map(...)`, read them as arrow function types.

---

## 7.0 Contracts and Effects Foundations

### Definition 7.1 (Contracts and Effects Foundations)

This part is the authoritative specification for behavioral contracts, effect clauses, preconditions, postconditions, and type-level invariants in Cursive. It defines the syntax, static semantics, dynamic semantics, and diagnostic catalog for contract-driven programming. Wherever other parts of the specification refer to contracts or effects, they rely on the rules in this chapter.

### 7.0.1 Scope

This chapter provides the complete normative description of:

* behavioral contracts declared with the `contract` keyword;
* `uses` (effect clauses) on procedures and functions;
* `must` (precondition clauses);
* `will` (postcondition clauses);
* `where` (type constraint clauses on records and other nominal types);
* composition rules that govern how contracts interact across calls; and
* the diagnostics that signal contract or effect violations.

### 7.0.2 Cross-Part Dependencies

Other chapters remain authoritative for the topics they introduce:

* **Part II** — Types, modal state machines, trait coherence, and effect-polymorphic map types;
* **Part III** — Declaration forms (`let`, `var`, shadowing) and scope rules;
* **Part V** — Expression formation, closure capture rules, effect inference, and value categories;
* **Part VI** — Block contracts, effect narrowing, loop verification, and `[[no_panic]]`; and
* **Part I (Foundations)** — Attributes, diagnostic formatting, judgment notation, statement separators, and grammar conventions.

Note: Part V (Expressions and Operators) retains the §4.x internal numbering used in earlier editions; all references in this chapter follow that numbering. CITE: Part V §4.0 — Expression Foundations.

CITE: Part II §2.7 — Traits and Dispatch; Part III §3.2 — Declarations and Scope; Part V §4.19 — Effect Composition; Part VI §6.13 — Block Contracts and Verification Modes; Part I §2.7 — Statement Separators.

### 7.0.3 Design Principles

Contracts and effects reinforce Cursive’s core philosophy:

* **Explicit over implicit** — Call-site obligations (`uses`, `must`, `will`) are spelled out in signatures.
* **Local reasoning** — Implementations verify clauses using only their own bodies and declared contracts.
* **Zero-cost abstraction** — Contracts add no runtime dispatch unless coupled with explicit dynamic mechanisms (§7.2.11).

### 7.0.4 Notational Conventions

The chapter relies on the standard judgment forms and metavariables defined in Part I. In particular:

* `Γ ⊢` judgments follow the typing/effect conventions of Part V;
* effect sets (`ε`) are finite sets closed under union/subset inclusion;
* predicates reuse the assertion grammar from Appendix A.8; and
* inference rules follow the format established in Part I §1.6.

### 7.0.5 Organization

The remainder of this chapter is organized as follows:

1. Overview (`§7.1`)
2. Behavioral contracts (`§7.2`)
3. Effect clauses (`§7.3`)
4. Precondition clauses (`§7.4`)
5. Postcondition clauses (`§7.5`)
6. Type constraint clauses (`§7.6`)
7. Clause composition (`§7.7`)
8. Diagnostics (`§7.8`)

Each section presents concrete grammar, abstract syntax, static semantics, dynamic semantics (where applicable), and illustrative examples.

---

## 7.1 Overview

This section provides a high-level introduction to Cursive's contract system before diving into the detailed specifications of each component.

### 7.1.1 Contract System Components

Cursive's contract system consists of five primary components that work together to enable design-by-contract programming:

1. **Behavioral contracts** (`contract` keyword) — Abstract interfaces specifying procedure signatures with clauses
2. **Effect clauses** (`uses`) — Declarations of computational side effects
3. **Precondition clauses** (`must`) — Requirements that MUST hold at procedure entry
4. **Postcondition clauses** (`will`) — Guarantees that MUST hold at procedure exit
5. **Type constraint clauses** (`where`) — Invariants that MUST hold for all instances of a type

### 7.1.2 Design Rationale

The contract system enforces three key principles:

* **Explicit obligations** — All behavioral requirements are visible in signatures, enabling local reasoning without examining implementations
* **Static verification** — Contracts are checked at compile time when possible, with optional runtime checks for dynamic conditions
* **Zero-cost abstraction** — Contract checking adds no runtime overhead when statically verified; runtime checks can be disabled in production builds

### 7.1.3 Relationship to Other Language Features

Contracts integrate with other Cursive features:

* **Permissions (Part IV)** — Contract predicates may reference permissions but MUST NOT violate permission invariants
* **Effects (this chapter)** — Effect clauses compose with the effect system to track side effects through call chains
* **Regions (Part VIII)** — Contracts may reference region-allocated data subject to escape analysis
* **Modals (Part X)** — Contracts may constrain modal state transitions and state-dependent operations

The following sections specify each component in detail.

---

## 7.2 Behavioral Contracts

Behavioral contracts define abstract interfaces consisting purely of procedure signatures annotated with clauses (`uses`, `must`, `will`). Unlike traits (Part II §2.7), contracts never provide implementations; they establish obligations that concrete types MUST satisfy.

### 7.2.1 Motivation and Design Goals

Contracts provide:

* **Abstract behavioral specifications** — state what a type MUST do, not how it does it;
* **Interface polymorphism** — any implementing type can be used where the contract is required;
* **Static verification** — implementations are checked against the declared clauses; and
* **Zero-cost abstraction** — contracts introduce no runtime dispatch unless paired with explicit dynamic dispatch mechanisms.

#### Contracts vs Traits

| Aspect | Contracts | Traits |
| ------ | --------- | ------ |
| Purpose | Abstract behavioral requirements | Concrete code reuse |
| Body requirements | **No** bodies permitted | **All** procedures have bodies |
| Polymorphism support | ✓ (implements-style) | ✗ (mixin only) |
| Code sharing | ✗ | ✓ |
| Typical use | Interfaces, API surfaces | Default implementations, mixins |

A type MAY simultaneously implement a contract and attach traits, but the semantic roles are distinct.

### 7.2.2 Grammar

The productions for contracts are added to Appendix A.6 (Declaration Grammar), Appendix A.7 (Contract Grammar), Appendix A.8 (Assertion Grammar), and Appendix A.9 (Effect Grammar) and repeated here for convenience:

```ebnf
ContractDecl   ::= Attribute* Visibility? "contract" Ident GenericParams?
                    ExtendsClause? "{" ContractItem* "}"
ExtendsClause  ::= "extends" Ident ("," Ident)*
ContractItem   ::= ProcedureSignature | AssociatedTypeDecl
ProcedureSignature
                ::= "procedure" Ident "(" ParamList? ")"
                    (":" Type)? WhereClause? ContractClause*
AssociatedTypeDecl
                ::= "type" Ident TypeBound?
```

`Attribute`, `Visibility`, `GenericParams`, `ParamList`, `TypeBound`, `WhereClause`, and `ContractClause` are defined in Appendix A.6 and Appendix A.7. Generic parameter lists MAY include effect parameters (e.g., `ε`) in addition to type and const parameters (see the `EffectParam` production in Appendix A.6), as detailed in Part II §2.9.4. Contract clauses MAY appear in any order and each clause MAY be included at most once per signature; duplicate clauses are rejected with E7C18. Contract items follow the standard statement separator rules (newline or semicolon). CITE: Part I §2.7 — Statement Separators.

### 7.2.3 Static Semantics

#### Contract well-formedness

```
[Contract-Item-WF]
Γ, GenericParams ⊢ Signature well-formed
Signature MUST have no body
──────────────────────────────────
Γ ⊢ ContractItem(Signature) well-formed

[Contract-WF]
∀i. Γ ⊢ ContractItemᵢ well-formed
Identifiers(Signatureᵢ) are pairwise distinct
────────────────────────────────────────────
Γ ⊢ contract C { ContractItem₁; …; ContractItemₙ; } ok
```

A contract item is well-formed when its parameters, result type, and clause list satisfy the rules in §§7.3–7.5. Contracts MUST NOT declare fields or provide bodies.

#### Implementing a contract

A record (or other nominal type) implements a contract by listing it after the colon in its declaration:

```cursive
record Circle: Drawable {
    radius: f64

    procedure draw(self: Self)
        uses fs.write
    {
        println("Circle radius {}", self.radius)
    }

    procedure bounds(self: Self): Rectangle
        will result.width >= 0.0, result.height >= 0.0
    {
        Rectangle {
            x: self.radius * -1.0,
            y: self.radius * -1.0,
            width: self.radius * 2.0,
            height: self.radius * 2.0,
        }
    }
}
```

Static semantics enforce that every contract procedure is implemented exactly once with compatible clauses:

```
[Contract-Impl]
contract C { Signature₁; …; Signatureₙ; }
record T: C { Proc₁; …; Procₙ; Members }
∀i. Signatureᵢ ≡ Procᵢ[Self ↦ T]
────────────────────────────────────────────
Γ ⊢ record T: C well-formed

[Contract-Clause-Compat]
Signature ≡ procedure p(...) uses ε_c must P_c will Q_c
Procedure ≡ procedure p(...) uses ε_m must P_m will Q_m { body }
Γ ⊢ body : τ
P_c ⇒ P_m
Q_m ⇒ Q_c
ε_m ⊆ ε_c
────────────────────────────────────────────
Procedure satisfies contract clause compatibility
```

Omitting a clause uses the identity element for that clause type: leaving out `uses` denotes the empty effect set `ε = ∅`, while omitting `must` or `will` denotes predicates that are trivially `true`. The compatibility relations above therefore still apply.

##### Generic implementations

Contracts and implementing types MAY both be generic. During checking, the compiler universally quantifies the contract’s `GenericParams` and instantiates them with the type’s parameters before comparing signatures:

```
[Contract-Impl-Generic]
contract C<Ξ_c> { Signature₁; …; Signatureₙ; }
record T<Ξ_t>: C<Φ> { Proc₁; …; Procₙ; Members }
Φ : Ξ_c ← Ξ_t          // argument list respects bounds/constraints
∀i. Signatureᵢ[Ξ_c ↦ Φ] ≡ Procᵢ[Self ↦ T<Ξ_t>]
──────────────────────────────────────────────────────────────
Γ ⊢ record T<Ξ_t>: C well-formed
```

Associated type obligations are checked after substitution: for every `type Item : Bounds` in the contract, the implementation MUST provide `type Item = Concrete` such that `Concrete[Ξ_t]` satisfies `Bounds[Self ↦ T<Ξ_t>]`. Blanket implementations (`record T<A>: C` where constraints involve `A`) are valid provided the coherence rules in §7.2.6 are met.

**Example — Generic iterator:**

```cursive
contract Iterator<T> {
    type Item : Copy
    procedure next(self: mut Self): Option<Self::Item>
}

record SliceIterator<T>: Iterator<T> {
    buffer: [T]
    index: usize
    type Item = T  // T must satisfy Copy bound from contract

    procedure next(self: mut Self): Option<Self::Item> {
        if self.index < self.buffer.len() {
            let item = self.buffer[self.index]
            self.index += 1
            Option::Some(item)
        } else {
            Option::None
        }
    }
}
```

If any compatibility test fails, the implementation is rejected with the diagnostics listed in §7.8 (E7C07–E7C09).
CITE: Appendix A.7 — ProcedureSignature; Part I §8 — Diagnostics Directory.

### 7.2.4 Contract Extension

Contracts MAY extend one or more contracts. The grammar adds an optional `extends` clause:

```ebnf
ContractDecl ::= "contract" Ident GenericParams?
                  ("extends" Ident ("," Ident)*)?
                  "{" ContractItem* "}"
```

Static semantics:

```
[Contract-Ext-WF]
contract C extends B₁, …, Bₖ { Items }
∀j. Γ ⊢ Bⱼ well-formed contract
CombinedSignatures = Signatures(B₁) ∪ … ∪ Signatures(Bₖ) ∪ Signatures(C)
Identifiers(CombinedSignatures) distinct
────────────────────────────────────────────────────────
Γ ⊢ contract C extends B₁, …, Bₖ { Items } well-formed
```

When a type implements an extending contract, it MUST satisfy every inherited signature and clause. Any conflict between inherited signatures is diagnosed at contract declaration time (E7C10).

```cursive
contract Transformable extends Drawable {
    procedure translate(self: mut Self, dx: f64, dy: f64)
    procedure scale(self: mut Self, factor: f64)
        must factor > 0.0
}
```

##### Conflict detection in inheritance graphs

The compiler constructs the transitive closure of all parent contracts before validating `contract C`. For every procedure name `p` appearing in the closure, it ensures the signatures are pairwise compatible:

```
[Contract-Ext-Conflict]
contract C extends B₁, …, Bₖ
signatures(Bᵢ, p) = {σ₁, …, σ_m}
∃(σ_a, σ_b). σ_a ≠ σ_b        // after α-renaming and clause normalisation
───────────────────────────────────────────────────────
ERROR E7C10 (incompatible inherited contract member)
```

Compatibility requires matching parameter lists, return types, and clause sets (after instantiating each parent’s generics with C’s arguments). Diamond inheritance is therefore well-defined only if all inheritance paths (through different parent contracts) agree on every shared member. Informative notes SHOULD indicate which parents contributed conflicting declarations. A contract that appears anywhere in its own inheritance closure triggers E7C15 before this rule is applied.

Contracts MUST NOT provide bodies for inherited members; if an extending contract wishes to refine a clause, it MUST redeclare the signature with strengthened or weakened constraints subject to the substitution rules below.
CITE: Appendix A.7 — ContractBody; Part I §8 — Diagnostics Directory.

### 7.2.5 Associated Types

Contracts MAY declare associated types that implementations MUST fix to concrete types. The grammar reuses `AssociatedTypeDecl` from Appendix A:

```ebnf
AssociatedTypeDecl ::= "type" Ident TypeBound?
```

Static semantics:

```
[AssocType-WF]
contract C { … type Item : Bounds?; … }
Bounds (when present) well-formed trait/contract bounds (Part II §2.9)
───────────────────────────────────────────────────────
Associated type declaration well-formed

[AssocType-Impl]
contract C { type Item : Bounds; … }
record T: C { … type Item = Concrete; … }
Γ ⊢ Concrete : Type
Γ ⊢ Concrete satisfies Bounds[Self ↦ T]
───────────────────────────────────────────────────────
Associated type implementation well-formed
```

```cursive
contract Iterator {
    type Item
    procedure next(self: mut Self): Option<Self::Item>
}

record Range: Iterator {
    start: i32
    end: i32
    type Item = i32

    procedure next(self: mut Self): Option<Self::Item> {
        if self.start < self.end {
            let value = self.start
            self.start += 1
            Option::Some(value)
        } else {
            Option::None
        }
    }
}
```

Associated types MAY carry their own bounds (e.g., `type Item : Copy`). Implementations MUST supply concrete types satisfying those bounds. Recursive definitions that reference the associated type without an intervening constructor are rejected with E7C14. Contracts do not declare associated constants; use associated types together with const generics to express constant requirements. CITE: Part I §8 — Diagnostics Directory.

#### Contract variance

Contracts are invariant in all of their generic parameters: two instantiations `C<T>` and `C<U>` are considered distinct obligations unless `T` and `U` are definitionally equal. This mirrors the variance rules for traits in Part II and prevents unsound substitutability across different concrete types. CITE: Part II §2.7 — Trait Variance.

Formally:

```
[Contract-Variance]
contract C<T>
T ≢ U  (T and U are not definitionally equal)
────────────────────────────────────────
C<T> ≢ C<U>  (distinct contract obligations)
```

**Rationale:** Invariance ensures that a type implementing `C<T>` cannot be substituted where `C<U>` is required, preventing type confusion in contract dispatch. This is necessary because contracts may expose associated types and methods that depend on the exact type parameter, and covariant or contravariant substitution would violate type safety.

##### Completeness of implementations

Implementations are obligated to provide a body (or associated type definition) for every item declared in the contract closure. Missing members are diagnosed immediately after the compatibility pass:

```
[Contract-Member-Missing]
contract C { Signature₁; …; Signatureₙ; }
record T: C { Proc₁; …; Procₖ; Members }
∃i. Signatureᵢ has no corresponding Procⱼ
────────────────────────────────────────────
ERROR E7C11 (missing implementation)
```

The check occurs before coherence analysis to ensure error messages are localized to the implementing type.

### 7.2.6 Coherence and Orphan Rules

Cursive follows the same coherence principles as traits (Part II §2.7):

```
[Contract-Orphan]
implementation of contract C for type T is provided in compilation unit K
K does not define C and K does not define T
────────────────────────────────────────
ERROR E7C12 (orphan implementation)

[Contract-Coherence]
Two implementations of (T, C) exist in the compilation
────────────────────────────────────────
ERROR E7C13 (duplicate implementation)
```

These rules guarantee that contract dispatch is unambiguous within a compilation unit.
CITE: Part II §2.7 — Trait Coherence; Part I §8 — Diagnostics Directory.

Contract implementation checks run when the compilation unit that defines the `record` (or equivalent nominal type) is compiled. Linkers and downstream packages MAY re-run coherence and completeness checks to guard against stale or incorrectly versioned artifacts, but the language requires each compilation unit to be internally valid before it is exported. CITE: Part XII (Modules, Packages, and Imports) §12.4 — Separate Compilation.

### 7.2.7 Usage and Examples

**Example — Polymorphic procedures:**

```cursive
procedure render_all<T>(items: [T])
    where T: Drawable
        uses fs.write
{
    for item in items {
        item::draw()
    }
}
```

Monomorphization instantiates the procedure for each concrete `T` used, preserving zero-cost abstractions.

**Example — Contract vs trait attachment:** A type MAY attach traits for code reuse while separately implementing a contract; the governing trait rules remain in Part II.

**Example — Higher-order usage:** Contracts can contain methods that accept other contracts as parameters, provided the callee is statically known to implement those contracts:

```cursive
contract Reducer<T> {
    procedure apply(self: mut Self, element: T)
}

procedure fold<T, R>(mut items: [T], reducer: mut R)
    where R: Reducer<T>
{
    for item in items {
        reducer.apply(item)
    }
}
```

Here `reducer` MAY be any type implementing `Reducer<T>`, including adapter wrappers around closures (see §7.2.9 for closure-to-contract adaptation patterns).

---

### 7.2.8 Edge Cases and Restrictions

* **Empty contracts:** A contract MAY contain zero items; implementing it is a no-op beyond coherence participation.
* **Recursive associated types:** An associated type definition that refers to itself without an intervening type constructor is rejected with E7C14 (in addition to the type well-formedness checks in Part II).
* **Effect conflicts:** If an implementation’s `uses` clause introduces effects not present in the contract (after closure), E7C09 is emitted.
* **Never-returning members:** Procedures whose result type is `!` need not supply postconditions; any declared `will` clause applies only to normal return paths.
* **Always-false preconditions:** Contracts MAY declare contradictory `must` predicates; implementations need only prove they do not strengthen the precondition. Call sites will inevitably fail verification, triggering E7C02.
* **Self-extension:** A contract that extends itself (directly or via a cycle) is rejected with E7C15 before inheritance closure is formed.
* **Recursive signatures:** Contracts MAY reference `Self` within member signatures (`Self::Item`, `procedure next(self: Self)`, etc.) but MUST NOT reference the contract name in a position that would require an as-yet-unspecified implementation (such as `procedure f(): ContractName`). Such illegal recursion produces E7C14.
* **Duplicate clauses:** Repeating a `uses`, `must`, or `will` clause within a single signature is rejected with E7C18.
* **Conflicting postconditions:** If multiple inherited signatures declare `will` clauses that cannot be jointly satisfied, the conflict is diagnosed with E7C10 during inheritance closure.
* **Extra members:** Implementations MAY define additional methods or associated items beyond those required by the contract; such additions are not subject to contract clause compatibility checking.

Implementers SHOULD surface these diagnostics eagerly so that violating contracts are caught before downstream types depend on them.
CITE: Part I §8 — Diagnostics Directory.

---

### 7.2.9 Contracts and closures

Anonymous functions and closures are values of function type (`(params) → τ ! ε`) and MAY NOT directly implement contracts (the `record/contract` syntax applies only to nominal types). Nevertheless, closures can satisfy contract-bounded parameters by being wrapped in nominal adapters that forward to the captured callable while preserving clause obligations.

```cursive
contract Handler {
    procedure handle(self: mut Self, message: string)
        uses fs.write
}

record ClosureHandler {
    callback: (string) → () ! {fs.write}

    procedure handle(self: mut Self, message: string)
        uses fs.write
    {
        self.callback(message)
    }
}
```

Generic bounds such as `where T: Handler` therefore accept ordinary records as well as adapter wrappers around closures. Effect variables declared on contracts compose with closure captures using the inference rules in Part V §4.19.5. CITE: Part V §4.16 — Contract Annotation Expressions.

### 7.2.10 Contracts and unsafe or foreign code

Contract clauses remain in force inside `unsafe` blocks and across FFI boundaries: the compiler still checks `uses`, `must`, and `will`, but the programmer assumes responsibility for upholding the contract when invoking unchecked operations. Unsafe code MUST re-establish any invariants it MAY temporarily violate before control returns to safe code, and FFI shims SHOULD document how the foreign implementation satisfies the declared clauses. CITE: Part XV (Unsafe Behaviors and FFI) §15.2 — Safety Obligations.

#### Contract checking in unsafe contexts

The contract checking algorithm for unsafe code follows these rules:

```
[Unsafe-Contract-Check]
unsafe { body }
body contains call to f with contract clauses (uses ε, must P, will Q)
────────────────────────────────────────────────────────────────
Compiler checks:
  1. Effect ε is declared in surrounding scope's uses clause
  2. Precondition P is satisfied at call site (static or runtime check)
  3. Postcondition Q is assumed to hold after call (programmer responsibility)
  4. Type invariants are checked at unsafe block boundaries
```

**Programmer obligations in unsafe code:**

1. **Effect discipline:** All effects performed in unsafe blocks MUST be declared in the enclosing procedure's `uses` clause
2. **Precondition satisfaction:** Preconditions MUST be satisfied before calling procedures, even when the call is inside an unsafe block
3. **Postcondition establishment:** When an unsafe block calls foreign code or performs unchecked operations, the programmer MUST ensure postconditions hold
4. **Invariant restoration:** Type invariants MAY be temporarily violated within an unsafe block, but MUST be restored before the block exits

**Example — Unsafe FFI with contracts:**

```cursive
// Foreign function declaration with contract
extern "C" procedure c_sqrt(x: f64): f64
    must x >= 0.0
    will result >= 0.0

procedure safe_sqrt(x: f64): f64
    must x >= 0.0
    will result >= 0.0
    uses ffi.call
{
    unsafe {
        // Precondition checked before unsafe call
        // Postcondition assumed to hold (programmer verifies C implementation)
        c_sqrt(x)
    }
}
```

**Verification strategy:** Implementations SHOULD provide a mode that inserts runtime checks for contract clauses at unsafe block boundaries, enabling detection of contract violations during testing even when static verification is not possible.

### 7.2.11 Dynamic dispatch

Contracts introduce no implicit runtime dispatch. To obtain dynamic polymorphism, programs MUST use contract witnesses as specified in Part II §2.13. Witnesses provide explicit type erasure with permission tracking, effect polymorphism, and modal state verification. See §7.2.12 for contract-specific usage patterns and examples. CITE: Part II §2.13 — Contract Witnesses.

### 7.2.12 Runtime Polymorphism

#### 7.2.12.1 Overview

Runtime polymorphism enables dynamic dispatch for contracts through **contract witnesses**. Unlike compile-time polymorphism via monomorphization (§7.2.7), witnesses allow:

- **Heterogeneous collections:** Store different types implementing the same contract
- **Plugin systems:** Load implementations at runtime
- **Dependency injection:** Swap implementations for testing or configuration
- **Effect polymorphism:** Abstract over effect sets at runtime

**Key principle:** Witnesses make allocation and indirection explicit while preserving Cursive's safety guarantees.

CITE: Part II §2.13 — Contract Witnesses (complete type system specification).

#### 7.2.12.2 Contract Witnesses

**Basic usage:**

```cursive
contract Drawable {
    procedure draw(self)
        uses io.write
}

// Create witness (explicit heap allocation)
let shape: own witness<Drawable, {io.write}> = witness::heap(Circle{radius: 5.0})

// Dynamic dispatch
    shape::draw()  // Calls Circle::draw() via vtable
```

**Witness construction:**

```cursive
// Heap allocation (default)
witness::heap(value)

// Inline storage (stack, fixed size)
witness::inline::<64>(value)

// Region allocation
region r {
    witness::region::<r>(value)
}

// Borrowed (no allocation)
witness::borrow(&value)
```

#### 7.2.12.3 Testing and Mocking

**Problem:** Testing effectful code without performing real effects.

**Solution:** Use witnesses with effect polymorphism.

```cursive
contract HttpClient {
    procedure get(self, url: string): Result<Response, Error>
        uses net.read

    procedure post(self, url: string, body: string): Result<Response, Error>
        uses net.write
}

// Real implementation
record RealHttpClient: HttpClient {
    timeout: Duration

    procedure get(self: RealHttpClient, url: string): Result<Response, Error>
        uses net.read
    {
        http_get(url, self.timeout)
    }

    procedure post(self: RealHttpClient, url: string, body: string): Result<Response, Error>
        uses net.write
    {
        http_post(url, body, self.timeout)
    }
}

// Mock implementation (NO effects!)
record MockHttpClient: HttpClient {
    responses: Map<string, Response>

    procedure get(self: MockHttpClient, url: string): Result<Response, Error> {
        // ∅ effects (pure)
        Result::Ok(self.responses.get(url).unwrap())
    }

    procedure post(self: MockHttpClient, url: string, body: string): Result<Response, Error> {
        // ∅ effects (pure)
        Result::Ok(Response{status: 200, body: "OK"})
    }
}

// Application code (effect-polymorphic)
procedure fetch_user<ε>(client: witness<HttpClient, ε>, id: i32): User
    uses ε
{
    let response = client.get("https://api.example.com/users/{id}")?
    parse_user(response)
}

// Production: real HTTP
procedure main()
    uses net.read
{
    let client = witness::heap(RealHttpClient{timeout: 30.seconds})
    let user = fetch_user(client, 42)
}

// Testing: mock (NO effects!)
#[test]
procedure test_fetch_user() {
    let mock = MockHttpClient{
        responses: Map.from([
            ("https://api.example.com/users/42",
             Response{status: 200, body: "{\"name\": \"Alice\"}"})
        ])
    }

    let client = witness::heap(mock)
    let user = fetch_user(client, 42)  // No net.read effect!
    assert_eq(user.name, "Alice")
}
```

**Key insight:** Effect subtyping (∅ ⊆ {net.read}) allows mock implementations with fewer effects.

#### 7.2.12.4 Plugin Systems

**Problem:** Load different implementations at runtime.

**Solution:** Use witnesses to abstract over implementations.

```cursive
contract Renderer {
    procedure render(self, scene: Scene)
        uses io.write, alloc.heap
}

// Load plugin at runtime
procedure load_renderer(name: string): own witness<Renderer, {io.write, alloc.heap}>
    uses fs.read, alloc.heap
{
    match name {
        "opengl" => witness::heap(OpenGLRenderer{...}),
        "vulkan" => witness::heap(VulkanRenderer{...}),
        "software" => witness::heap(SoftwareRenderer{...}),
        _ => panic("Unknown renderer: {name}")
    }
}

// Use plugin
let renderer = load_renderer("vulkan")
renderer.render(scene)
```

#### 7.2.12.5 Heterogeneous Collections

**Problem:** Store different types implementing the same contract.

**Solution:** Use arrays of witnesses.

```cursive
contract Drawable {
    procedure draw(self)
        uses io.write
}

// Collection of different shapes
let shapes: [own witness<Drawable, {io.write}>] = [
    witness::heap(Circle{radius: 5.0}),
    witness::heap(Rectangle{width: 10.0, height: 20.0}),
    witness::heap(Triangle{a: p1, b: p2, c: p3}),
]

// Render all
for shape in shapes {
    shape.draw()  // Dynamic dispatch
}
```

#### 7.2.12.6 Modal Contract Witnesses

Witnesses track modal states at runtime:

```cursive
contract Connection {
    modal states {
        Disconnected
        Connected
        Closed
    }

    procedure connect(self: mut Connection@Disconnected): Result<(), Error>
        uses net.connect
        will match result {
            Ok(_) => self is Connection@Connected,
            Err(_) => self is Connection@Disconnected
        }

    procedure send(self: mut Connection@Connected, data: string): Result<(), Error>
        uses net.write

    procedure close(self: mut Connection@Connected)
        uses net.write
        will self is Connection@Closed
}

// Witness in Disconnected state
let conn: own witness<Connection@Disconnected, {net.connect, net.write}> =
    witness::heap(TcpConnection{...})

// State transition
let conn = conn.connect()?
// Type: witness<Connection@Connected, {net.connect, net.write}>

// State-dependent operations
conn.send("Hello")?  // ✅ OK (Connected state)
conn.connect()?      // ❌ ERROR: expected @Disconnected, found @Connected
```

**Runtime verification:** In debug mode, witnesses verify state transitions at runtime and panic on state mismatches.

#### 7.2.12.7 Permission-Aware Witnesses

Witnesses integrate with the Lexical Permission System:

```cursive
contract Counter {
    procedure get(self: imm Self): i32
    procedure increment(self: mut Self)
    procedure reset(self: own Self)
}

// Immutable witness: only immutable methods
let counter: imm witness<Counter, ∅> = witness::borrow(&my_counter)
counter.get()        // ✅ OK
counter.increment()  // ❌ ERROR: need mut permission

// Mutable witness: immutable + mutable methods
let counter: mut witness<Counter, ∅> = witness::heap(MyCounter{value: 0})
counter.get()        // ✅ OK
counter.increment()  // ✅ OK
counter.reset()      // ❌ ERROR: need own permission

// Owned witness: all methods
let counter: own witness<Counter, ∅> = witness::heap(MyCounter{value: 0})
counter.get()        // ✅ OK
counter.increment()  // ✅ OK
counter.reset()      // ✅ OK (consumes witness)
```

#### 7.2.12.8 Performance Considerations

**Costs:**

- **Allocation:** Heap/region/inline allocation at witness construction
- **Indirection:** One vtable pointer dereference per procedure call
- **State tracking:** One state tag check per modal procedure call (debug mode)

**Optimization:**

- **Sealed witnesses:** Preserve concrete type for devirtualization (Part II §2.13.6)
- **Inline storage:** Avoid heap allocation for small objects
- **Region allocation:** Batch allocation/deallocation

**When to use:**

- ✅ Heterogeneous collections
- ✅ Plugin systems
- ✅ Testing/mocking
- ✅ Dependency injection
- ❌ Hot loops (prefer monomorphization)
- ❌ Known types at compile time (prefer static dispatch)

---

CITE: Part II §2.13 — Contract Witnesses; Part IV — Lexical Permission System; Part X — Modals.

### 7.2.13 Modal integration

Contracts interact with modal types by constraining the operations available in each state. Implementations that expose modal transitions MUST ensure their contract clauses hold for every reachable state, and modal transition procedures MAY reference contract predicates when defining admissible transitions. Detailed rules for modal state machines reside in Part X (Modals) §10.4; this chapter assumes those semantics. CITE: Part X §10.4 — Modal State Machines.

#### Modal state contracts

Contracts MAY specify state-dependent preconditions and postconditions using modal state patterns:

**Example — File handle with modal states:**

```cursive
modal FileHandle {
    @Closed { path: string }
    @Open { path: string, descriptor: i32 }
}

contract FileOperations {
    procedure open(self: mut FileHandle@Closed): Result<(), Error>
        uses fs.read
        will match result {
            Ok(_) => self is FileHandle@Open,
            Err(_) => self is FileHandle@Closed
        }

    procedure read(self: mut FileHandle@Open, buffer: mut [u8]): Result<usize, Error>
        uses fs.read
        must buffer.len() > 0
        will match result {
            Ok(n) => n <= buffer.len(),
            Err(_) => true
        }

    procedure close(self: mut FileHandle@Open)
        uses fs.write
        will self is FileHandle@Closed
}
```

**State-dependent contract checking:**

```
[Modal-Contract-Check]
procedure p(self: T@S₁) → T@S₂
contract C requires p with clauses (uses ε, must P, will Q)
────────────────────────────────────────────────────────────────
Compiler verifies:
  1. P references only fields accessible in state S₁
  2. Q references only fields accessible in state S₂
  3. State transition S₁ → S₂ is valid per modal definition
  4. Effects ε are permitted in both states
```

**Rationale:** Modal contracts enable compile-time verification of protocol adherence, ensuring that operations are only called in valid states and that state transitions preserve invariants.


### 7.2.14 Contract Patterns

This subsection catalogs common contract usage patterns in Cursive, demonstrating how contracts establish behavioral abstractions across different domains.

#### Interface Contracts

Interface contracts define abstract capabilities without specifying implementation details:

```cursive
contract Serializable {
    procedure serialize(self: imm Self): string
        uses alloc.heap
        will result::len() > 0

    procedure deserialize(data: string): Result<own Self, Error>
        uses alloc.heap
        must data::len() > 0
}

record User: Serializable {
    id: i32,
    name: string,
    email: string

    procedure serialize(self: imm Self): string
        uses alloc.heap
        will result::len() > 0
    {
        result string::from("User(id={}, name={}, email={})", self.id, self.name, self.email)
    }

    procedure deserialize(data: string): Result<own User, Error>
        uses alloc.heap
        must data::len() > 0
    {
        // Parse data and construct User
        result Result::Ok(User { id: 1, name: string::from("Alice"), email: string::from("alice@example.com") })
    }
}
```

#### Invariant-Preserving Contracts

Contracts enforcing type invariants across operations:

```cursive
contract Bounded {
    procedure min_value(self: imm Self): i32
    procedure max_value(self: imm Self): i32
    procedure current_value(self: imm Self): i32
        will result >= self::min_value() && result <= self::max_value()
}

record Counter: Bounded {
    value: i32,
    min: i32,
    max: i32
    where value >= min && value <= max

    procedure min_value(self: imm Self): i32 {
        result self.min
    }

    procedure max_value(self: imm Self): i32 {
        result self.max
    }

    procedure current_value(self: imm Self): i32
        will result >= self::min_value() && result <= self::max_value()
    {
        result self.value
    }

    procedure increment(self: mut Self)
        must self.value < self.max
        will self.value <= self.max
    {
        self.value = self.value + 1
    }
}
```

#### Protocol Contracts

Contracts specifying multi-step protocols and operation ordering:

```cursive
contract Connection {
    procedure connect(self: mut Self, host: string, port: i32): Result<(), Error>
        uses network.connect
        must port > 0 && port < 65536
        will match result {
            Ok(_) => true,
            Err(_) => true
        }

    procedure send(self: mut Self, data: [u8]): Result<usize, Error>
        uses network.send
        must data::len() > 0
        will match result {
            Ok(n) => n <= data::len(),
            Err(_) => true
        }

    procedure receive(self: mut Self, buffer: mut [u8]): Result<usize, Error>
        uses network.receive
        must buffer::len() > 0
        will match result {
            Ok(n) => n <= buffer::len(),
            Err(_) => true
        }

    procedure disconnect(self: mut Self)
        uses network.disconnect
}

record TcpConnection: Connection {
    socket: i32,
    connected: bool

    procedure connect(self: mut Self, host: string, port: i32): Result<(), Error>
        uses network.connect
        must port > 0 && port < 65536
        will match result {
            Ok(_) => self.connected == true,
            Err(_) => self.connected == false
        }
    {
        // Implementation details
        self.connected = true
        result Result::Ok(())
    }

    procedure send(self: mut Self, data: [u8]): Result<usize, Error>
        uses network.send
        must data::len() > 0 && self.connected
        will match result {
            Ok(n) => n <= data::len(),
            Err(_) => true
        }
    {
        // Send implementation
        result Result::Ok(data::len())
    }

    procedure receive(self: mut Self, buffer: mut [u8]): Result<usize, Error>
        uses network.receive
        must buffer::len() > 0 && self.connected
        will match result {
            Ok(n) => n <= buffer::len(),
            Err(_) => true
        }
    {
        // Receive implementation
        result Result::Ok(0)
    }

    procedure disconnect(self: mut Self)
        uses network.disconnect
        will self.connected == false
    {
        self.connected = false
    }
}
```

#### Resource Management Contracts

Contracts governing resource acquisition and release:

```cursive
contract Resource {
    procedure acquire(self: mut Self): Result<(), Error>
        uses alloc.heap

    procedure release(self: mut Self)
        uses alloc.heap

    procedure is_acquired(self: imm Self): bool
}

record FileResource: Resource {
    path: string,
    handle: Option<i32>

    procedure acquire(self: mut Self): Result<(), Error>
        uses alloc.heap, fs.open
        will match result {
            Ok(_) => self::is_acquired(),
            Err(_) => !self::is_acquired()
        }
    {
        self.handle = Option::Some(42)
        result Result::Ok(())
    }

    procedure release(self: mut Self)
        uses alloc.heap, fs.close
        will !self::is_acquired()
    {
        self.handle = Option::None
    }

    procedure is_acquired(self: imm Self): bool {
        self.handle match {
            Option::Some(_) => result true,
            Option::None => result false
        }
    }
}
```

#### State Machine Contracts

Contracts modeling finite state machines:

```cursive
contract StateMachine {
    type State

    procedure current_state(self: imm Self): Self::State
    procedure transition(self: mut Self, event: i32): Result<(), Error>
        will self::current_state() != @old(self::current_state()) || result is Err
}

enum TrafficLightState {
    Red,
    Yellow,
    Green
}

record TrafficLight: StateMachine {
    state: TrafficLightState
    type State = TrafficLightState

    procedure current_state(self: imm Self): Self::State {
        result self.state
    }

    procedure transition(self: mut Self, event: i32): Result<(), Error>
        uses alloc.heap
        will self::current_state() != @old(self::current_state()) || result is Err
    {
        self.state match {
            TrafficLightState::Red => {
                if event == 1 {
                    self.state = TrafficLightState::Green
                    result Result::Ok(())
                } else {
                    result Result::Err(Error::new())
                }
            },
            TrafficLightState::Yellow => {
                if event == 2 {
                    self.state = TrafficLightState::Red
                    result Result::Ok(())
                } else {
                    result Result::Err(Error::new())
                }
            },
            TrafficLightState::Green => {
                if event == 3 {
                    self.state = TrafficLightState::Yellow
                    result Result::Ok(())
                } else {
                    result Result::Err(Error::new())
                }
            }
        }
    }
}
```

### 7.2.15 Generic Contract Patterns

Generic contracts parameterize behavioral abstractions over types, enabling reusable specifications across different data structures.

#### Type-Parameterized Contracts

Contracts generic over element types:

```cursive
contract Container<T> {
    procedure add(self: mut Self, item: T)
        uses alloc.heap

    procedure remove(self: mut Self): Option<T>
        uses alloc.heap

    procedure size(self: imm Self): usize
        will result >= 0

    procedure is_empty(self: imm Self): bool
        will result == (self::size() == 0)
}

record Stack<T>: Container<T> {
    items: Vec<T>

    procedure add(self: mut Self, item: T)
        uses alloc.heap
    {
        self.items::push(item)
    }

    procedure remove(self: mut Self): Option<T>
        uses alloc.heap
    {
        result self.items::pop()
    }

    procedure size(self: imm Self): usize
        will result >= 0
    {
        result self.items::len()
    }

    procedure is_empty(self: imm Self): bool
        will result == (self::size() == 0)
    {
        result self.items::len() == 0
    }
}

record Queue<T>: Container<T> {
    items: Vec<T>

    procedure add(self: mut Self, item: T)
        uses alloc.heap
    {
        self.items::push(item)
    }

    procedure remove(self: mut Self): Option<T>
        uses alloc.heap
    {
        if self.items::len() > 0 {
            result Option::Some(self.items::remove(0))
        } else {
            result Option::None
        }
    }

    procedure size(self: imm Self): usize
        will result >= 0
    {
        result self.items::len()
    }

    procedure is_empty(self: imm Self): bool
        will result == (self::size() == 0)
    {
        result self.items::len() == 0
    }
}
```

#### Effect-Parameterized Contracts

Contracts parameterized by effect sets:

```cursive
contract Processor<T, ε> {
    procedure process(self: mut Self, input: T): Result<T, Error>
        uses ε
}

record LoggingProcessor<T>: Processor<T, {io.write, alloc.heap}> {
    log_file: string

    procedure process(self: mut Self, input: T): Result<T, Error>
        uses io.write, alloc.heap
    {
        println("Processing input")
        result Result::Ok(input)
    }
}

record NetworkProcessor<T>: Processor<T, {network.send, alloc.heap}> {
    endpoint: string

    procedure process(self: mut Self, input: T): Result<T, Error>
        uses network.send, alloc.heap
    {
        // Send to network
        result Result::Ok(input)
    }
}
```

#### Bounded Generic Contracts

Contracts with type parameter constraints:

```cursive
contract Comparable<T> where T: Ord {
    procedure compare(self: imm Self, other: imm T): Ordering

    procedure less_than(self: imm Self, other: imm T): bool
        will result == (self::compare(other) == Ordering::Less)
}

record ComparableWrapper<T>: Comparable<T> where T: Ord {
    value: T

    procedure compare(self: imm Self, other: imm T): Ordering {
        result self.value::cmp(other)
    }

    procedure less_than(self: imm Self, other: imm T): bool
        will result == (self::compare(other) == Ordering::Less)
    {
        result self.value < other
    }
}
```

#### Multi-Parameter Generic Contracts

Contracts with multiple type parameters:

```cursive
contract Converter<From, To> {
    procedure convert(self: imm Self, input: From): Result<To, Error>
        uses alloc.heap
}

record StringToIntConverter: Converter<string, i32> {
    procedure convert(self: imm Self, input: string): Result<i32, Error>
        uses alloc.heap
    {
        // Parse string to integer
        result Result::Ok(42)
    }
}

record IntToStringConverter: Converter<i32, string> {
    procedure convert(self: imm Self, input: i32): Result<string, Error>
        uses alloc.heap
    {
        result Result::Ok(string::from(input))
    }
}
```

### 7.2.16 Contract Composition Examples

This subsection demonstrates how contracts compose through extension, multiple implementations, and interaction patterns.

#### Multiple Contract Implementation

A single type implementing multiple contracts:

```cursive
contract Drawable {
    procedure draw(self: imm Self)
        uses fs.write
}

contract Transformable {
    procedure translate(self: mut Self, dx: f64, dy: f64)
    procedure rotate(self: mut Self, angle: f64)
}

contract Bounded {
    procedure bounds(self: imm Self): Rectangle
        will result.width >= 0.0 && result.height >= 0.0
}

record Shape: Drawable, Transformable, Bounded {
    x: f64,
    y: f64,
    width: f64,
    height: f64

    procedure draw(self: imm Self)
        uses fs.write
    {
        println("Drawing shape at ({}, {})", self.x, self.y)
    }

    procedure translate(self: mut Self, dx: f64, dy: f64) {
        self.x = self.x + dx
        self.y = self.y + dy
    }

    procedure rotate(self: mut Self, angle: f64) {
        // Rotation implementation
    }

    procedure bounds(self: imm Self): Rectangle
        will result.width >= 0.0 && result.height >= 0.0
    {
        result Rectangle {
            x: self.x,
            y: self.y,
            width: self.width,
            height: self.height
        }
    }
}
```

#### Contract Extension Chains

Contracts extending other contracts:

```cursive
contract BasicIterator<T> {
    procedure next(self: mut Self): Option<T>
        uses alloc.heap
}

contract DoubleEndedIterator<T> extends BasicIterator<T> {
    procedure next_back(self: mut Self): Option<T>
        uses alloc.heap
}

contract ExactSizeIterator<T> extends BasicIterator<T> {
    procedure len(self: imm Self): usize
        will result >= 0
}

record VectorIterator<T>: DoubleEndedIterator<T>, ExactSizeIterator<T> {
    items: Vec<T>,
    front: usize,
    back: usize

    procedure next(self: mut Self): Option<T>
        uses alloc.heap
    {
        if self.front < self.back {
            let item = self.items[self.front]
            self.front = self.front + 1
            result Option::Some(item)
        } else {
            result Option::None
        }
    }

    procedure next_back(self: mut Self): Option<T>
        uses alloc.heap
    {
        if self.front < self.back {
            self.back = self.back - 1
            let item = self.items[self.back]
            result Option::Some(item)
        } else {
            result Option::None
        }
    }

    procedure len(self: imm Self): usize
        will result >= 0
    {
        result self.back - self.front
    }
}
```

#### Diamond Inheritance Resolution

Multiple inheritance paths converging on a common contract:

```cursive
contract Base {
    procedure base_operation(self: imm Self): i32
}

contract LeftDerived extends Base {
    procedure left_operation(self: imm Self): i32
}

contract RightDerived extends Base {
    procedure right_operation(self: imm Self): i32
}

contract Diamond extends LeftDerived, RightDerived {
    procedure diamond_operation(self: imm Self): i32
}

record DiamondImpl: Diamond {
    value: i32

    procedure base_operation(self: imm Self): i32 {
        result self.value
    }

    procedure left_operation(self: imm Self): i32 {
        result self.value * 2
    }

    procedure right_operation(self: imm Self): i32 {
        result self.value * 3
    }

    procedure diamond_operation(self: imm Self): i32 {
        result self.value * 4
    }
}
```

#### Contract-Based Polymorphism

Using contracts for polymorphic function parameters:

```cursive
contract Logger {
    procedure log(self: mut Self, message: string)
        uses io.write
}

record ConsoleLogger: Logger {
    procedure log(self: mut Self, message: string)
        uses io.write
    {
        println("{}", message)
    }
}

record FileLogger: Logger {
    path: string

    procedure log(self: mut Self, message: string)
        uses io.write, fs.write
    {
        // Write to file
        println("Logging to file: {}", message)
    }
}

procedure process_with_logging<L>(data: [i32], logger: mut L)
    where L: Logger
    uses io.write, alloc.heap
{
    logger::log(string::from("Processing started"))

    loop item in data {
        logger::log(string::from("Processing item: {}", item))
    }

    logger::log(string::from("Processing completed"))
}

function example()
    uses io.write, alloc.heap
{
    let console = ConsoleLogger {}
    let file = FileLogger { path: string::from("/tmp/log.txt") }

    let data = [1, 2, 3, 4, 5]

    process_with_logging(data, mut console)
    process_with_logging(data, mut file)
}
```

#### Adapter Pattern with Contracts

Wrapping types to implement contracts:

```cursive
contract DataSource {
    procedure read_data(self: mut Self): Result<[u8], Error>
        uses alloc.heap
}

record FileDataSource: DataSource {
    path: string

    procedure read_data(self: mut Self): Result<[u8], Error>
        uses alloc.heap, fs.read
    {
        // Read from file
        result Result::Ok([1, 2, 3, 4, 5])
    }
}

record MemoryDataSource: DataSource {
    buffer: Vec<u8>

    procedure read_data(self: mut Self): Result<[u8], Error>
        uses alloc.heap
    {
        result Result::Ok(self.buffer::as_slice())
    }
}

record NetworkDataSourceAdapter: DataSource {
    url: string

    procedure read_data(self: mut Self): Result<[u8], Error>
        uses alloc.heap, network.receive
    {
        // Fetch from network
        result Result::Ok([10, 20, 30])
    }
}

procedure process_data<S>(source: mut S)
    where S: DataSource
    uses alloc.heap
{
    let data = source::read_data()

    data match {
        Result::Ok(bytes) => {
            println("Received {} bytes", bytes::len())
        },
        Result::Err(e) => {
            println("Error reading data")
        }
    }
}
```


---


## 7.X Comprehensive Contract Examples and Patterns

This section consolidates real-world contract usage patterns, demonstrating how contracts compose with Cursive's permission system, effect system, and modal types to create robust behavioral abstractions.

### 7.X.1 Basic Contract Usage

Fundamental contract patterns demonstrating declaration, implementation, and polymorphic usage.

#### Simple Service Contract

```cursive
contract Service {
    procedure start(self: mut Self): Result<(), Error>
        uses alloc.heap

    procedure stop(self: mut Self)
        uses alloc.heap

    procedure is_running(self: imm Self): bool
}

record HttpServer: Service {
    port: i32,
    running: bool

    procedure start(self: mut Self): Result<(), Error>
        uses alloc.heap, network.bind
        will match result {
            Ok(_) => self.running == true,
            Err(_) => self.running == false
        }
    {
        if self.port > 0 && self.port < 65536 {
            self.running = true
            result Result::Ok(())
        } else {
            result Result::Err(Error::new())
        }
    }

    procedure stop(self: mut Self)
        uses alloc.heap
        will self.running == false
    {
        self.running = false
    }

    procedure is_running(self: imm Self): bool {
        result self.running
    }
}

function manage_service<S>(service: mut S)
    where S: Service
    uses alloc.heap
{
    let start_result = service::start()

    start_result match {
        Result::Ok(_) => {
            if service::is_running() {
                println("Service started successfully")
                service::stop()
            }
        },
        Result::Err(_) => {
            println("Failed to start service")
        }
    }
}
```

#### Collection Contract

```cursive
contract Collection<T> {
    procedure insert(self: mut Self, item: T)
        uses alloc.heap

    procedure contains(self: imm Self, item: T): bool
        where T: Eq

    procedure size(self: imm Self): usize
        will result >= 0
}

record HashSet<T>: Collection<T> where T: Hash, T: Eq {
    buckets: Vec<Vec<T>>

    procedure insert(self: mut Self, item: T)
        uses alloc.heap
    {
        // Hash and insert
        self.buckets[0]::push(item)
    }

    procedure contains(self: imm Self, item: T): bool
        where T: Eq
    {
        // Search in buckets
        result false
    }

    procedure size(self: imm Self): usize
        will result >= 0
    {
        var count = 0
        loop bucket in self.buckets {
            count = count + bucket::len()
        }
        result count
    }
}

record TreeSet<T>: Collection<T> where T: Ord {
    root: Option<Box<TreeNode<T>>>

    procedure insert(self: mut Self, item: T)
        uses alloc.heap
    {
        // Tree insertion
    }

    procedure contains(self: imm Self, item: T): bool
        where T: Eq
    {
        result false
    }

    procedure size(self: imm Self): usize
        will result >= 0
    {
        result 0
    }
}
```

#### Validation Contract

```cursive
contract Validator<T> {
    procedure validate(self: imm Self, value: T): Result<(), string>
        uses alloc.heap
}

record EmailValidator: Validator<string> {
    procedure validate(self: imm Self, value: string): Result<(), string>
        uses alloc.heap
    {
        if value::contains("@") {
            result Result::Ok(())
        } else {
            result Result::Err(string::from("Invalid email"))
        }
    }
}

record RangeValidator: Validator<i32> {
    min: i32,
    max: i32

    procedure validate(self: imm Self, value: i32): Result<(), string>
        uses alloc.heap
    {
        if value >= self.min && value <= self.max {
            result Result::Ok(())
        } else {
            result Result::Err(string::from("Value out of range"))
        }
    }
}

function validate_input<T, V>(value: T, validator: imm V): bool
    where V: Validator<T>
    uses alloc.heap
{
    let validation_result = validator::validate(value)

    validation_result match {
        Result::Ok(_) => result true,
        Result::Err(msg) => {
            println("Validation failed: {}", msg)
            result false
        }
    }
}
```

### 7.X.2 Stateful Contracts with Modal Types

Contracts integrating with modal types to enforce state-dependent protocols.

#### Connection Lifecycle Contract

```cursive
modal Connection {
    @Disconnected { address: string }
    @Connecting { address: string, timeout: i32 }
    @Connected { address: string, socket: i32 }
    @Failed { address: string, error: string }
}

contract ConnectionProtocol {
    procedure connect(self: mut Connection@Disconnected): Connection@Connecting
        uses network.connect, alloc.heap

    procedure await_connection(self: mut Connection@Connecting): Result<Connection@Connected, Connection@Failed>
        uses network.connect, alloc.heap

    procedure send(self: mut Connection@Connected, data: [u8]): Result<usize, Error>
        uses network.send, alloc.heap
        must data::len() > 0

    procedure disconnect(self: mut Connection@Connected): Connection@Disconnected
        uses network.disconnect, alloc.heap
}

record TcpConnection: ConnectionProtocol {
    procedure connect(self: mut Connection@Disconnected): Connection@Connecting
        uses network.connect, alloc.heap
    {
        result Connection@Connecting {
            address: self.address,
            timeout: 30
        }
    }

    procedure await_connection(self: mut Connection@Connecting): Result<Connection@Connected, Connection@Failed>
        uses network.connect, alloc.heap
    {
        result Result::Ok(Connection@Connected {
            address: self.address,
            socket: 42
        })
    }

    procedure send(self: mut Connection@Connected, data: [u8]): Result<usize, Error>
        uses network.send, alloc.heap
        must data::len() > 0
    {
        result Result::Ok(data::len())
    }

    procedure disconnect(self: mut Connection@Connected): Connection@Disconnected
        uses network.disconnect, alloc.heap
    {
        result Connection@Disconnected {
            address: self.address
        }
    }
}
```

#### Transaction State Contract

```cursive
modal Transaction {
    @NotStarted { id: i32 }
    @InProgress { id: i32, operations: Vec<string> }
    @Committed { id: i32 }
    @RolledBack { id: i32, reason: string }
}

contract TransactionProtocol {
    procedure begin(self: mut Transaction@NotStarted): Transaction@InProgress
        uses alloc.heap

    procedure add_operation(self: mut Transaction@InProgress, op: string)
        uses alloc.heap

    procedure commit(self: mut Transaction@InProgress): Result<Transaction@Committed, Transaction@RolledBack>
        uses alloc.heap, db.write

    procedure rollback(self: mut Transaction@InProgress, reason: string): Transaction@RolledBack
        uses alloc.heap
}

record DatabaseTransaction: TransactionProtocol {
    procedure begin(self: mut Transaction@NotStarted): Transaction@InProgress
        uses alloc.heap
    {
        result Transaction@InProgress {
            id: self.id,
            operations: Vec::new()
        }
    }

    procedure add_operation(self: mut Transaction@InProgress, op: string)
        uses alloc.heap
    {
        self.operations::push(op)
    }

    procedure commit(self: mut Transaction@InProgress): Result<Transaction@Committed, Transaction@RolledBack>
        uses alloc.heap, db.write
    {
        result Result::Ok(Transaction@Committed {
            id: self.id
        })
    }

    procedure rollback(self: mut Transaction@InProgress, reason: string): Transaction@RolledBack
        uses alloc.heap
    {
        result Transaction@RolledBack {
            id: self.id,
            reason: reason
        }
    }
}
```

#### Session Management Contract

```cursive
modal UserSession {
    @Anonymous { ip: string }
    @Authenticated { ip: string, user_id: i32, token: string }
    @Expired { ip: string, user_id: i32 }
}

contract SessionManagement {
    procedure authenticate(self: mut UserSession@Anonymous, username: string, password: string): Result<UserSession@Authenticated, UserSession@Anonymous>
        uses alloc.heap, auth.verify

    procedure refresh(self: mut UserSession@Authenticated): Result<UserSession@Authenticated, UserSession@Expired>
        uses alloc.heap, auth.verify

    procedure logout(self: mut UserSession@Authenticated): UserSession@Anonymous
        uses alloc.heap
}

record WebSession: SessionManagement {
    procedure authenticate(self: mut UserSession@Anonymous, username: string, password: string): Result<UserSession@Authenticated, UserSession@Anonymous>
        uses alloc.heap, auth.verify
    {
        if username::len() > 0 && password::len() > 0 {
            result Result::Ok(UserSession@Authenticated {
                ip: self.ip,
                user_id: 1,
                token: string::from("token123")
            })
        } else {
            result Result::Err(UserSession@Anonymous { ip: self.ip })
        }
    }

    procedure refresh(self: mut UserSession@Authenticated): Result<UserSession@Authenticated, UserSession@Expired>
        uses alloc.heap, auth.verify
    {
        result Result::Ok(UserSession@Authenticated {
            ip: self.ip,
            user_id: self.user_id,
            token: string::from("new_token")
        })
    }

    procedure logout(self: mut UserSession@Authenticated): UserSession@Anonymous
        uses alloc.heap
    {
        result UserSession@Anonymous {
            ip: self.ip
        }
    }
}
```

### 7.X.3 Resource Management Contracts with Permissions

Contracts demonstrating ownership transfer, permission requirements, and RAII patterns.

#### Owning Resource Contract

```cursive
contract OwnedResource {
    procedure acquire(): Result<own Self, Error>
        uses alloc.heap

    procedure use_resource(self: mut Self): Result<(), Error>
        uses alloc.heap

    procedure release(self: own Self)
        uses alloc.heap
}

record FileHandle: OwnedResource {
    path: string,
    descriptor: i32

    procedure acquire(): Result<own FileHandle, Error>
        uses alloc.heap, fs.open
    {
        result Result::Ok(FileHandle {
            path: string::from("/tmp/file.txt"),
            descriptor: 42
        })
    }

    procedure use_resource(self: mut Self): Result<(), Error>
        uses alloc.heap, fs.write
    {
        result Result::Ok(())
    }

    procedure release(self: own Self)
        uses alloc.heap, fs.close
    {
        // Close file descriptor
        println("Closing file: {}", self.path)
    }
}

function with_resource<R>(operation: (mut R) -> Result<(), Error>)
    where R: OwnedResource
    uses alloc.heap
{
    let resource_result = R::acquire()

    resource_result match {
        Result::Ok(mut resource) => {
            let op_result = operation(mut resource)

            op_result match {
                Result::Ok(_) => {
                    println("Operation succeeded")
                },
                Result::Err(_) => {
                    println("Operation failed")
                }
            }

            resource::release()
        },
        Result::Err(_) => {
            println("Failed to acquire resource")
        }
    }
}
```

#### Permission-Aware Processing Contract

```cursive
contract Processor<T> {
    procedure process_owned(self: mut Self, data: own T): own T
        uses alloc.heap

    procedure process_mutable(self: mut Self, data: mut T)
        uses alloc.heap

    procedure process_immutable(self: imm Self, data: imm T): i32
}

record DataProcessor: Processor<Vec<i32>> {
    multiplier: i32

    procedure process_owned(self: mut Self, data: own Vec<i32>): own Vec<i32>
        uses alloc.heap
    {
        var result = Vec::new()

        loop item in data {
            result::push(item * self.multiplier)
        }

        result result
    }

    procedure process_mutable(self: mut Self, data: mut Vec<i32>)
        uses alloc.heap
    {
        loop mut item in data {
            item = item * self.multiplier
        }
    }

    procedure process_immutable(self: imm Self, data: imm Vec<i32>): i32 {
        var sum = 0

        loop item in data {
            sum = sum + item
        }

        result sum
    }
}
```

#### Builder Contract with Ownership

```cursive
contract Builder<T> {
    procedure new(): own Self
        uses alloc.heap

    procedure with_field(self: own Self, value: i32): own Self
        uses alloc.heap

    procedure build(self: own Self): own T
        uses alloc.heap
}

record ConfigBuilder: Builder<Config> {
    timeout: i32,
    retries: i32

    procedure new(): own ConfigBuilder
        uses alloc.heap
    {
        result ConfigBuilder {
            timeout: 30,
            retries: 3
        }
    }

    procedure with_field(self: own Self, value: i32): own ConfigBuilder
        uses alloc.heap
    {
        result ConfigBuilder {
            timeout: value,
            retries: self.retries
        }
    }

    procedure build(self: own Self): own Config
        uses alloc.heap
    {
        result Config {
            timeout: self.timeout,
            retries: self.retries
        }
    }
}

record Config {
    timeout: i32,
    retries: i32
}

function create_config()
    uses alloc.heap
{
    let config = ConfigBuilder::new()
        => (|b| -> b::with_field(60))
        => (|b| -> b::build())

    println("Config: timeout={}, retries={}", config.timeout, config.retries)
}
```

### 7.X.4 Effect-Polymorphic Contracts

Contracts demonstrating effect polymorphism and effect composition.

#### Effect-Parameterized Handler

```cursive
contract Handler<T, ε> {
    procedure handle(self: mut Self, input: T): Result<T, Error>
        uses ε
}

record LoggingHandler<T>: Handler<T, {io.write, alloc.heap}> {
    name: string

    procedure handle(self: mut Self, input: T): Result<T, Error>
        uses io.write, alloc.heap
    {
        println("Handler {} processing input", self.name)
        result Result::Ok(input)
    }
}

record NetworkHandler<T>: Handler<T, {network.send, alloc.heap}> {
    endpoint: string

    procedure handle(self: mut Self, input: T): Result<T, Error>
        uses network.send, alloc.heap
    {
        println("Sending to {}", self.endpoint)
        result Result::Ok(input)
    }
}

record CompositeHandler<T>: Handler<T, {io.write, network.send, alloc.heap}> {
    logger: LoggingHandler<T>,
    network: NetworkHandler<T>

    procedure handle(self: mut Self, input: T): Result<T, Error>
        uses io.write, network.send, alloc.heap
    {
        let log_result = self.logger::handle(input)

        log_result match {
            Result::Ok(logged_data) => {
                result self.network::handle(logged_data)
            },
            Result::Err(e) => {
                result Result::Err(e)
            }
        }
    }
}
```

#### Pure and Effectful Contracts

```cursive
contract Computation<T> {
    procedure compute(self: imm Self, input: T): T
}

record PureComputation: Computation<i32> {
    factor: i32

    procedure compute(self: imm Self, input: i32): i32 {
        result input * self.factor
    }
}

contract EffectfulComputation<T, ε> {
    procedure compute_with_effects(self: mut Self, input: T): Result<T, Error>
        uses ε
}

record DatabaseComputation: EffectfulComputation<i32, {db.read, db.write, alloc.heap}> {
    table: string

    procedure compute_with_effects(self: mut Self, input: i32): Result<i32, Error>
        uses db.read, db.write, alloc.heap
    {
        // Database operations
        result Result::Ok(input * 2)
    }
}
```


## 7.3 Unified Effect System

### 7.3.1 Overview

Cursive's effect system unifies capability-based security with algebraic effects, providing both static effect tracking and flexible implementation override. This unified approach enables:

1. **Static effect tracking** — All computational side effects are declared in `uses` clauses and checked at compile time
2. **Capability-based security** — Effects are unforgeable tokens that cannot be forged or synthesized
3. **Implementation flexibility** — Effect implementations can be overridden for testing, mocking, or alternative behavior
4. **Zero-cost abstraction** — Default implementations are inlined by the compiler, imposing no runtime overhead

#### Three Kinds of Effects

**1. Marker effects** — Effects with no operations, used purely for capability tracking:

```cursive
effect unsafe.transmute
effect panic.may_panic
```

**2. Effects with default implementations** — Effects whose operations have default implementations provided by the runtime or OS:

```cursive
effect FileSystem {
    read(path: string): string {
        os_read(path)  // Default: use OS
    }
    
    write(path: string, data: string): () {
        os_write(path, data)  // Default: use OS
    }
}
```

**3. Effects without default implementations** — Effects that require the user to provide an implementation:

```cursive
effect Async<T> {
    await(future: Future<T>): T
    // No default - user must provide
}
```

#### Key Mechanisms

- **Effect declarations** (`effect` keyword) — Define effect operations and optional default implementations
- **Uses clauses** (`uses` keyword) — Declare required effects in procedure signatures
- **With blocks** (`with` keyword) — Provide custom implementations for specific scopes
- **Implicit perform** — Effect operations are invoked like procedure calls (no `perform` keyword)
- **Implicit continue** — Simple implementations automatically resume computation
- **Explicit control** — `continue()` and `abort()` provide explicit control over computation flow

#### Capitalization Convention

Effects are distinguished from modules by capitalization:

- **Effects**: `FileSystem`, `Network`, `Async` (capitalized)
- **Modules**: `fs`, `net`, `io` (lowercase)
- **Types**: `String`, `Vec`, `Option` (capitalized)

This convention makes effect operations visually distinct from module function calls.

CITE: Part II §2.10 — Effect Types; Part V §4.19 — Effect Composition.

---

### 7.3.2 Effect Declarations

Effect declarations define the operations that comprise an effect and optionally provide default implementations.

#### Syntax

**Grammar:**

```ebnf
EffectDecl ::= "effect" EffectPath EffectBody?

EffectPath ::= Ident ("." Ident)*

EffectBody ::= "{" EffectItem* "}"

EffectItem ::= EffectOperation

EffectOperation ::= Ident "(" ParamList ")" ":" Type EffectBody?

EffectImplBody ::= "{" Expr "}"
```

**Three forms:**

1. **Marker effect** (no operations):
   ```cursive
   effect unsafe.transmute
   ```

2. **Effect with default implementation**:
   ```cursive
   effect FileSystem {
       read(path: string): string {
           os_read(path)
       }
   }
   ```

3. **Effect without default implementation**:
   ```cursive
   effect Async<T> {
       await(future: Future<T>): T
   }
   ```

#### Static Semantics

**Well-formedness:**

```
[Effect-Decl-WF]
effect E { op₁(x₁: τ₁): τ₁' { body₁ }; ...; opₙ(xₙ: τₙ): τₙ' { bodyₙ } }
∀i. Γ, xᵢ: τᵢ ⊢ bodyᵢ : τᵢ'
operation names distinct
────────────────────────────────────────────────────────────────────────
Γ ⊢ effect E well-formed
```

**Effect without default:**

```
[Effect-Decl-No-Default-WF]
effect E { op₁(x₁: τ₁): τ₁'; ...; opₙ(xₙ: τₙ): τₙ' }
operation names distinct
────────────────────────────────────────────────────────────────────────
Γ ⊢ effect E well-formed (no default implementation)
```

**Marker effect:**

```
[Effect-Marker-WF]
effect E
────────────────────────────────────────────────────────────────────────
Γ ⊢ effect E well-formed (marker)
```

#### Examples

**System effect with default implementation:**

```cursive
effect FileSystem {
    read(path: string): string {
        os_read(path)
    }
    
    write(path: string, data: string): () {
        os_write(path, data)
    }
    
    delete(path: string): () {
        if path.starts_with("/system") {
            abort(Error.Forbidden)
        }
        continue(os_delete(path))
    }
}
```

**User-defined effect without default:**

```cursive
effect Async<T> {
    await(future: Future<T>): T
}

effect Generator<T> {
    yield(value: T): ()
}

effect Choice {
    choose(): bool
}
```

**Marker effects:**

```cursive
effect unsafe.transmute
effect panic.may_panic
effect alloc.heap
```

#### Diagnostics

- **E7E01**: Effect operation names must be distinct within an effect
- **E7E02**: Effect operation body type mismatch
- **E7E03**: Effect path must be valid identifier path
- **E7E04**: Marker effect cannot have operations

CITE: Appendix A — Grammar; Part I §8 — Diagnostics Directory.

---

### 7.3.3 Effect Operations

Effect operations are the individual operations that can be performed within an effect.

#### Operation Types

Each effect operation has a type that includes its effect context:

```
[Effect-Op-Type]
effect E { op(x₁: τ₁, ..., xₙ: τₙ): τᵣ { body } }
────────────────────────────────────────────────
Γ ⊢ E.op : (τ₁, ..., τₙ) → τᵣ ! {E}
```

**Example:**

```cursive
effect FileSystem {
    read(path: string): string { os_read(path) }
}

// FileSystem.read has type:
// (string) → string ! {FileSystem}
```

#### Operation Invocation

Effect operations are invoked using qualified names (implicit perform):

```cursive
procedure foo()
    uses FileSystem
{
    let content = FileSystem::read("/tmp/file.txt")
    //            ^^^^^^^^^^^^^^^^ Effect operation invocation
}
```

**Type rule:**

```
[Effect-Op-Call]
Γ ⊢ E.op : (τ₁, ..., τₙ) → τᵣ ! {E}
Γ ⊢ e₁ : τ₁, ..., Γ ⊢ eₙ : τₙ
E ∈ ε_context  (E is available in context)
────────────────────────────────────────────────
Γ ⊢ E.op(e₁, ..., eₙ) : τᵣ ! {E}
```

#### Generic Effect Operations

Effect operations may be generic:

```cursive
effect Async<T> {
    await(future: Future<T>): T
}

// Async<Response>.await has type:
// (Future<Response>) → Response ! {Async<Response>}
```

CITE: Part II §2.10 — Effect Types; Part V §4.19 — Effect Composition.

---

### 7.3.4 Default Implementations

Default implementations define the standard behavior of effect operations.

#### Implicit Continue Semantics

Simple implementations automatically resume computation with the result:

```cursive
effect FileSystem {
    read(path: string): string {
        os_read(path)  // Implicit: continue(os_read(path))
    }
}
```

**Desugaring:**

```cursive
read(path: string): string {
    os_read(path)
}

// Equivalent to:
read(path: string): string {
    continue(os_read(path))
}
```

#### Explicit Continue

For complex implementations, use explicit `continue()`:

```cursive
effect FileSystem {
    read(path: string): string {
        log("Reading {path}")
        let content = os_read(path)
        log("Read {content.len()} bytes")
        continue(content)
    }
}
```

#### Abort (Don't Resume)

Use `abort()` to terminate computation without resuming:

```cursive
effect FileSystem {
    delete(path: string): () {
        if path.starts_with("/system") {
            abort(Error.Forbidden)
        }
        continue(os_delete(path))
    }
}
```

#### Static Semantics

**Continue:**

```
[Continue]
Γ ⊢ e : τ
────────────────────────────────────────────────
Γ ⊢ continue(e) : τ
```

**Abort:**

```
[Abort]
Γ ⊢ e : τ
────────────────────────────────────────────────
Γ ⊢ abort(e) : ⊥  (diverges)
```

#### Diagnostics

- **E7E05**: `continue()` can only be used in effect implementations
- **E7E06**: `abort()` can only be used in effect implementations
- **E7E07**: `continue()` type mismatch with operation return type

CITE: Part VI §6.x — Control Flow.

---

### 7.3.5 Effect Usage

Procedures declare required effects in `uses` clauses.

#### Uses Clause Syntax

**Grammar:**

```ebnf
UsesClause ::= "uses" EffectSet

EffectSet ::= EffectRef ("," EffectRef)*

EffectRef ::= EffectPath ("::" Ident)?
```

**Three forms:**

1. **Entire effect** (all operations):
   ```cursive
   procedure foo()
       uses FileSystem
   ```

2. **Specific operation** (fine-grained):
   ```cursive
   procedure foo()
       uses FileSystem.read
   ```

3. **Multiple effects/operations**:
   ```cursive
   procedure foo()
       uses FileSystem.read, FileSystem.write, Network.get
   ```

#### Fine-Grained Capabilities

Granting access to specific operations enables principle of least privilege:

```cursive
procedure read_config()
    uses FileSystem.read  // Only read, not write
{
    FileSystem::read("/etc/config")  // ✅ OK
    FileSystem::write("/etc/config", "data")  // ❌ ERROR: no write capability
}
```

**Type rule:**

```
[Uses-Fine-Grained]
procedure p() uses E.op₁, E.op₂ { body }
Γ ⊢ body ! ε_body
ε_body ⊆ {E.op₁, E.op₂}
────────────────────────────────────────────────
Γ ⊢ procedure p well-formed
```

#### Effect Polymorphism

Procedures may be polymorphic over effects:

```cursive
procedure with_logging<ε>(action: fn() → () ! ε)
    uses FileSystem, ε
{
    FileSystem::write("/tmp/log.txt", "Starting")
    action()
    FileSystem::write("/tmp/log.txt", "Done")
}
```

**Type rule:**

```
[Uses-Polymorphic]
procedure p<ε>(...) uses ε₁, ε { body }
Γ, ε: EffectVar ⊢ body ! ε_body
ε_body ⊆ ε₁ ∪ ε
────────────────────────────────────────────────
Γ ⊢ procedure p<ε> well-formed
```

#### Diagnostics

- **E7E08**: Procedure body uses undeclared effect
- **E7E09**: Procedure body uses undeclared effect operation
- **E7E10**: Effect not available in context

CITE: Part II §2.10 — Effect Polymorphism; Part V §4.19 — Effect Composition.

---

### 7.3.6 Custom Implementations (With Blocks)

Custom implementations override default implementations for a specific scope using `with` blocks.

#### Syntax

**Grammar:**

```ebnf
WithBlock ::= "with" EffectList "{" ImplementationList "}" "{" Block "}"

EffectList ::= EffectPath ("," EffectPath)*

ImplementationList ::= Implementation ("," Implementation)*

Implementation ::= EffectPath "." Ident "(" ParamList ")" "=>" Expr
```

#### External (Caller Provides)

The caller provides custom implementations for the called procedure:

```cursive
// Caller provides implementation
with FileSystem {
    read(path) => mock_read(path)
    write(path, data) => mock_write(path, data)
} {
    foo()  // foo uses mock implementation
}

procedure foo()
    uses FileSystem
{
    FileSystem::read("/tmp/file.txt")
}
```

**Use cases:**
- Testing with mocks
- Dependency injection
- Runtime configuration

#### Internal (Procedure Provides)

A procedure provides custom implementations for its own code:

```cursive
procedure foo()
    uses FileSystem
{
    // Procedure provides implementation for its own code
    with FileSystem {
        read(path) => cached_read(path)
    } {
        let a = FileSystem::read("/tmp/a")
        let b = FileSystem::read("/tmp/b")
    }
}
```

**Use cases:**
- Caching layer
- Logging wrapper
- Scoped behavior changes

#### Multiple Effects

Override multiple effects in a single block:

```cursive
with FileSystem, Network {
    FileSystem::read(path) => mock_read(path)
    FileSystem::write(path, data) => mock_write(path, data)
    Network.get(url) => mock_get(url)
} {
    foo()
}
```

#### Partial Implementations

Provide implementations for some operations, others use default:

```cursive
with FileSystem {
    read(path) => mock_read(path)
    // write uses default implementation
} {
    foo()
}
```

#### Static Semantics

**Well-formedness:**

```
[With-Block-WF]
with E₁, ..., Eₙ { impl₁; ...; implₘ } { body }
∀i. Γ ⊢ Eᵢ well-formed effect
∀j. implⱼ implements operation from some Eᵢ
∀j. Γ ⊢ implⱼ : τⱼ where τⱼ matches operation signature
Γ, E₁ ↦ {impl₁, ...}, ..., Eₙ ↦ {...} ⊢ body : τ
────────────────────────────────────────────────────────────
Γ ⊢ with E₁, ..., Eₙ { ... } { body } : τ
```

**Implementation compatibility:**

```
[With-Impl-Compat]
effect E { op(x: τ₁): τ₂ { default_body } }
with E { op(x) => custom_body } { ... }
Γ, x: τ₁ ⊢ custom_body : τ₂
────────────────────────────────────────────────────────────
Implementation compatible
```

#### Dynamic Semantics

**Implementation resolution:**

1. Check innermost `with` block for custom implementation
2. If not found, check outer `with` blocks
3. If not found, use default implementation
4. If no default, error (effect requires implementation)

**Evaluation:**

```
⟨with E { E.op(x) => e_impl } { body }, σ⟩ ⇓ ⟨v, σ'⟩
where body evaluated with E.op bound to λx. e_impl
```

#### Examples

**Testing with mocks:**

```cursive
procedure test_fetch_user() {
    with FileSystem, Network {
        FileSystem::read(path) => {
            if path.contains("cache") {
                abort(Error.NotFound)  // Cache miss
            }
            continue("")
        }

        Network.get(url) => {
            assert(url.contains("users/42"))
            continue(Response { body: '{"id": 42, "name": "Alice"}' })
        }
    } {
        let user = fetch_user(42)
        assert(user.name == "Alice")
    }
}
```

**Async runtime:**

```cursive
procedure main() {
    with Async<T> {
        await(future) => {
            future.on_complete(|result| continue(result))
        }
    } {
        run_async_app()
    }
}
```

**Caching layer:**

```cursive
procedure fetch_with_cache(id: i32): User
    uses FileSystem, Network
{
    with FileSystem {
        read(path) => {
            if let Some(cached) = cache.get(path) {
                continue(cached)
            }
            continue(FileSystem::read(path))  // Delegate to outer
        }
    } {
        let config = FileSystem::read("/etc/config")
        let data = fetch_from_network(id)
    }
}
```

#### Diagnostics

- **E7E11**: `with` block implementation does not match operation signature
- **E7E12**: `with` block references undefined effect
- **E7E13**: `with` block references undefined operation
- **E7E14**: Effect requires implementation but none provided

CITE: Part V §4.19 — Effect Composition; Part VI §6.x — Block Statements.

---

### 7.3.7 Control Flow Operations

Effect implementations use `continue()` and `abort()` to control computation flow.

#### Continue (Resume Computation)

`continue(value)` resumes the suspended computation with the given value:

```cursive
effect FileSystem {
    read(path: string): string {
        log("Reading {path}")
        continue(os_read(path))  // Resume with result
    }
}
```

**Type rule:**

```
[Continue]
effect E { op(...): τ { body } }
Γ ⊢ e : τ
────────────────────────────────────────────────
Γ ⊢ continue(e) : τ (in context of E.op implementation)
```

**Implicit continue:**

Simple expressions automatically continue:

```cursive
read(path: string): string {
    os_read(path)  // Implicit: continue(os_read(path))
}
```

#### Abort (Terminate Without Resuming)

`abort(value)` terminates the computation without resuming:

```cursive
effect FileSystem {
    delete(path: string): () {
        if path.starts_with("/system") {
            abort(Error.Forbidden)  // Don't resume
        }
        continue(os_delete(path))
    }
}
```

**Type rule:**

```
[Abort]
Γ ⊢ e : τ
────────────────────────────────────────────────
Γ ⊢ abort(e) : ⊥  (diverges)
```

#### Multiple Resume (Non-Determinism)

Implementations may resume multiple times:

```cursive
effect Choice {
    choose(): bool {
        continue(true)   // Try true
        continue(false)  // Try false
    }
}

with Choice {
    choose() => {
        continue(true)
        continue(false)
    }
} {
    let x = Choice.choose()
    println(x)  // Prints twice: true, false
}
```

**Semantics:**

Each `continue()` creates a separate continuation that executes independently.

#### Diagnostics

- **E7E15**: `continue()` outside effect implementation
- **E7E16**: `abort()` outside effect implementation
- **E7E17**: `continue()` type mismatch
- **E7E18**: Multiple `continue()` in non-copyable continuation context

CITE: Part VI §6.x — Control Flow; Part V §4.x — Continuation Semantics.

---

### 7.3.8 Effect Composition

Effects compose through set union and subtyping.

#### Effect Sets

Effect sets are finite sets of effect references:

```cursive
ε ::= ∅                    // Empty set
    | {E}                  // Single effect
    | {E.op}               // Single operation
    | ε₁ ∪ ε₂              // Union
```

**Examples:**

```cursive
procedure foo()
    uses FileSystem, Network  // ε = {FileSystem, Network}

procedure bar()
    uses FileSystem.read, FileSystem.write  // ε = {FileSystem.read, FileSystem.write}
```

#### Effect Subtyping

Effect sets form a subtyping relation based on **superset inclusion**:

```
[Effect-Subtype]
ε₁ ⊇ ε₂  // ε₁ has at least all effects in ε₂ (possibly more)
------------------------------------------------
ε₁ <: ε₂  // ε₁ can be used where ε₂ is required
```

**Intuition: "Having more permissions is a subtype"**

This rule may appear counterintuitive at first, but it follows the principle of **capability-based subtyping**:

- **ε₁ ⊇ ε₂** means: "ε₁ grants at least all the capabilities that ε₂ grants (and possibly more)"
- **ε₁ <: ε₂** means: "ε₁ can be used in any context where ε₂ is expected"

**Why this makes sense:**

If a function requires effect set ε₂ = {FileSystem.read} to execute, and you're in a context that grants ε₁ = {FileSystem.read, FileSystem.write}, you have **at least** what the function needs, so the call is safe. Having extra permissions doesn't make the call unsafe—you simply won't use them all.

**Mnemonic: "More capabilities = narrower type"**

Think of it like keys: if you need a key to room A (ε₂), and you have a keyring with keys to rooms A and B (ε₁), you can still open room A. Having extra keys (ε₁ ⊇ ε₂) makes your keyring usable wherever the single key is needed (ε₁ <: ε₂).

**Formal Statement:**

For effect sets ε₁ and ε₂:
- **ε₁ ⊇ ε₂** is read as "ε₁ is a superset of ε₂" (set theory)
- **ε₁ <: ε₂** is read as "ε₁ is a subtype of ε₂" (type theory)
- The rule states: **wider capabilities make narrower type**

**Example:**

```cursive
// ε₁ = {FileSystem} ⊇ ε₂ = {FileSystem.read}
procedure foo()
    uses FileSystem  // Has all operations
{
    bar()  // ✅ OK: bar requires only read, we have all operations
}

procedure bar()
    uses FileSystem.read  // Requires only read
```

#### Effect Polymorphism

Procedures may be polymorphic over effects:

```cursive
procedure with_logging<ε>(action: fn() → () ! ε)
    uses FileSystem, ε
{
    FileSystem::write("/tmp/log.txt", "Starting")
    action()
    FileSystem::write("/tmp/log.txt", "Done")
}
```

**Type rule:**

```
[Effect-Poly-Call]
Γ ⊢ f : (τ₁, ..., τₙ) → τᵣ ! ε_f
Γ ⊢ e₁ : τ₁, ..., Γ ⊢ eₙ : τₙ
ε_context ⊇ ε_f  // Context has required effects
────────────────────────────────────────────────
Γ ⊢ f(e₁, ..., eₙ) : τᵣ ! ε_f
```

#### Effect Inference

Effect sets can be inferred from procedure bodies:

```cursive
procedure foo() {  // Inferred: uses FileSystem.read
    FileSystem::read("/tmp/file.txt")
}
```

**Inference rule:**

```
[Effect-Infer]
Γ ⊢ body ! ε_body
────────────────────────────────────────────────
Γ ⊢ procedure p() { body } : (...) → τ ! ε_body
```

CITE: Part II §2.10 — Effect Types; Part V §4.19 — Effect Composition.

---

### 7.3.9 Complete Examples

This section provides comprehensive examples demonstrating the unified effect system.

#### Example 1: Testing with Mocks

```cursive
// ============================================
// Effect Declarations
// ============================================

effect FileSystem {
    read(path: string): string {
        os_read(path)
    }

    write(path: string, data: string): () {
        os_write(path, data)
    }
}

effect Network {
    get(url: string): Response {
        http_get(url)
    }
}

// ============================================
// Application Code
// ============================================

procedure fetch_user_data(id: i32): User
    uses FileSystem.read, Network.get
{
    // Check cache
    let cache_key = "/tmp/cache/user_{id}"
    if let Some(cached) = try_read(cache_key) {
        return parse_user(cached)
    }

    // Fetch from API
    let url = "https://api.example.com/users/{id}"
    let response = Network.get(url)

    // Cache result
    FileSystem::write(cache_key, response.body)

    parse_user(response.body)
}

// ============================================
// Production
// ============================================

procedure main() {
    let user = fetch_user_data(42)
    println("User: {user.name}")
}

// ============================================
// Testing
// ============================================

procedure test_fetch_user_data() {
    with FileSystem, Network {
        FileSystem::read(path) => {
            if path.contains("cache") {
                abort(Error.NotFound)  // Cache miss
            }
            continue("")
        }

        FileSystem::write(path, data) => {
            assert(path.contains("cache"))
            continue(())
        }

        Network.get(url) => {
            assert(url.contains("users/42"))
            continue(Response { body: '{"id": 42, "name": "Alice"}' })
        }
    } {
        let user = fetch_user_data(42)
        assert(user.name == "Alice")
    }
}
```

#### Example 2: Async/Await

```cursive
// ============================================
// Effect Declaration
// ============================================

effect Async<T> {
    await(future: Future<T>): T
    // No default - user must provide
}

// ============================================
// Application Code
// ============================================

procedure fetch_data(url: string): Response
    uses Network, Async<Response>
{
    let future = Network.get_async(url)
    Async.await(future)
}

// ============================================
// Runtime Provides Implementation
// ============================================

procedure main() {
    with Async<T> {
        await(future) => {
            // Suspend current task
            runtime.suspend_current_task()

            // Register callback
            future.on_complete(|result| {
                runtime.resume_task_with(result)
            })

            // Yield to scheduler
            runtime.yield_to_scheduler()
        }
    } {
        let response = fetch_data("https://example.com")
        println("Got response: {response.status}")
    }
}
```

#### Example 3: Generators

```cursive
// ============================================
// Effect Declaration
// ============================================

effect Generator<T> {
    yield(value: T): ()
}

// ============================================
// Generator Function
// ============================================

procedure range(start: i32, end: i32): ()
    uses Generator<i32>
{
    var i = start
    while i < end {
        Generator.yield(i)
        i += 1
    }
}

// ============================================
// Consumer
// ============================================

procedure main() {
    var values: Vec<i32> = Vec.new()

    with Generator<i32> {
        yield(value) => {
            values.push(value)
            continue(())
        }
    } {
        range(0, 10)
    }

    println("Values: {values}")  // [0, 1, 2, ..., 9]
}
```

#### Example 4: Transactions

```cursive
// ============================================
// Effect Declaration
// ============================================

effect Transaction {
    commit(): ()
    rollback(): ()
}

// ============================================
// Transactional Code
// ============================================

procedure transfer(from: Account, to: Account, amount: i32): Result<(), Error>
    uses Database, Transaction
{
    Database.update("UPDATE accounts SET balance = balance - {amount} WHERE id = {from.id}")
    Database.update("UPDATE accounts SET balance = balance + {amount} WHERE id = {to.id}")

    if verify_balances() {
        Transaction.commit()
        Ok(())
    } else {
        Transaction.rollback()
        Err(Error.InvalidBalance)
    }
}

// ============================================
// Transaction Runtime
// ============================================

procedure main() {
    with Transaction {
        commit() => {
            db.commit_transaction()
            continue(())
        }

        rollback() => {
            db.rollback_transaction()
            abort(Error.TransactionAborted)
        }
    } {
        let result = transfer(account1, account2, 100)
        match result {
            Ok(()) => println("Transfer successful")
            Err(e) => println("Transfer failed: {e}")
        }
    }
}
```

#### Example 5: Non-Determinism

```cursive
// ============================================
// Effect Declaration
// ============================================

effect Choice {
    choose(): bool
}

// ============================================
// Non-Deterministic Code
// ============================================

procedure find_solution(): Option<(i32, i32)>
    uses Choice
{
    let x = if Choice.choose() { 1 } else { 2 }
    let y = if Choice.choose() { 3 } else { 4 }

    if x + y == 5 {
        Some((x, y))
    } else {
        None
    }
}

// ============================================
// Backtracking Search
// ============================================

procedure main() {
    var solutions: Vec<(i32, i32)> = Vec.new()

    with Choice {
        choose() => {
            // Try both branches
            continue(true)
            continue(false)
        }
    } {
        if let Some(solution) = find_solution() {
            solutions.push(solution)
        }
    }

    println("Solutions: {solutions}")  // [(1, 4), (2, 3)]
}
```

---

### 7.3.10 Diagnostics

This section catalogs all diagnostic codes for the effect system.

#### Effect Declaration Errors (E7E01-E7E04)

- **E7E01**: Effect operation names must be distinct within an effect
  ```cursive
  effect Bad {
      read(path: string): string { ... }
      read(file: File): string { ... }  // ❌ ERROR E7E01
  }
  ```

- **E7E02**: Effect operation body type mismatch
  ```cursive
  effect Bad {
      read(path: string): string {
          42  // ❌ ERROR E7E02: expected string, got i32
      }
  }
  ```

- **E7E03**: Effect path must be valid identifier path
  ```cursive
  effect 123.invalid { }  // ❌ ERROR E7E03
  ```

- **E7E04**: Marker effect cannot have operations
  ```cursive
  effect unsafe.transmute {  // ❌ ERROR E7E04
      op(): ()
  }
  ```

#### Control Flow Errors (E7E05-E7E07)

- **E7E05**: `continue()` can only be used in effect implementations
  ```cursive
  procedure foo() {
      continue(42)  // ❌ ERROR E7E05
  }
  ```

- **E7E06**: `abort()` can only be used in effect implementations
  ```cursive
  procedure foo() {
      abort(Error.Bad)  // ❌ ERROR E7E06
  }
  ```

- **E7E07**: `continue()` type mismatch with operation return type
  ```cursive
  effect Bad {
      read(path: string): string {
          continue(42)  // ❌ ERROR E7E07: expected string, got i32
      }
  }
  ```

#### Effect Usage Errors (E7E08-E7E10)

- **E7E08**: Procedure body uses undeclared effect
  ```cursive
  procedure foo() {  // ❌ ERROR E7E08: uses FileSystem but not declared
      FileSystem::read("/tmp/file.txt")
  }
  ```

- **E7E09**: Procedure body uses undeclared effect operation
  ```cursive
  procedure foo()
      uses FileSystem.read
  {
      FileSystem::write("/tmp/file.txt", "data")  // ❌ ERROR E7E09
  }
  ```

- **E7E10**: Effect not available in context
  ```cursive
  procedure foo() {
      bar()  // ❌ ERROR E7E10: bar requires FileSystem
  }

  procedure bar()
      uses FileSystem
  ```

#### With Block Errors (E7E11-E7E14)

- **E7E11**: `with` block implementation does not match operation signature
  ```cursive
  with FileSystem {
      read(path) => 42  // ❌ ERROR E7E11: expected string, got i32
  } { ... }
  ```

- **E7E12**: `with` block references undefined effect
  ```cursive
  with UndefinedEffect { ... } { ... }  // ❌ ERROR E7E12
  ```

- **E7E13**: `with` block references undefined operation
  ```cursive
  with FileSystem {
      undefined_op() => ...  // ❌ ERROR E7E13
  } { ... }
  ```

- **E7E14**: Effect requires implementation but none provided
  ```cursive
  procedure foo()
      uses Async<Response>
  {
      Async.await(future)  // ❌ ERROR E7E14: Async has no default
  }
  ```

#### Continuation Errors (E7E15-E7E18)

- **E7E15**: `continue()` outside effect implementation
  (Same as E7E05)

- **E7E16**: `abort()` outside effect implementation
  (Same as E7E06)

- **E7E17**: `continue()` type mismatch
  (Same as E7E07)

- **E7E18**: Multiple `continue()` in non-copyable continuation context
  ```cursive
  effect Bad {
      op(): () {
          continue(())
          continue(())  // ❌ ERROR E7E18: continuation already consumed
      }
  }
  ```

CITE: Part I §8 — Diagnostics Directory.

---

### 7.3.11 Effect Composition Patterns

This section demonstrates practical patterns for composing effects in real-world scenarios.

#### Union Composition

Multiple effects compose via set union to express combined capabilities:

```cursive
// ============================================
// File Processing with Multiple Effects
// ============================================

procedure process_file(input_path: string, output_path: string)
    uses FileSystem.read, FileSystem.write, alloc.heap
{
    let content = FileSystem::read(input_path)
    let processed = content::to_uppercase()
    FileSystem::write(output_path, processed)
}

// Effect set: {FileSystem.read, FileSystem.write, alloc.heap}
```

#### Subset Refinement

Functions requiring fewer effects can be called from contexts with more effects:

```cursive
// ============================================
// Effect Subset Pattern
// ============================================

// Read-only operation
procedure read_config(path: string): string
    uses FileSystem.read
{
    result FileSystem::read(path)
}

// Read-write operation can call read-only
procedure update_config(path: string, new_value: string)
    uses FileSystem.read, FileSystem.write
{
    let current = read_config(path)  // ✅ OK: superset includes read
    let updated = string::replace(current, "old", new_value)
    FileSystem::write(path, updated)
}
```

**Type checking:**

```
[Subset-Refinement]
ε_caller = {FileSystem.read, FileSystem.write}
ε_callee = {FileSystem.read}
ε_callee ⊆ ε_caller
--------------------------------------------
Call is well-typed
```

#### Composition in Higher-Order Functions

Effect polymorphism enables generic abstractions over computational effects:

```cursive
// ============================================
// Generic Retry Logic
// ============================================

function retry<T, ε>(
    operation: fn() -> Result<T, Error> ! ε,
    max_attempts: i32
): Result<T, Error>
    uses ε
    must max_attempts > 0
{
    var attempts = 0

    loop {
        attempts = attempts + 1

        let result = operation()

        result match {
            Result::Ok(value) => {
                result Result::Ok(value)
            },
            Result::Err(error) => {
                if attempts >= max_attempts {
                    result Result::Err(error)
                }
                // Continue loop for retry
            }
        }
    }
}

// Usage: retry inherits effects from passed operation
procedure fetch_data(): Result<string, Error>
    uses Network.get
{
    let result = retry(
        fn() -> Result<string, Error> ! {Network.get} {
            result Network::get("https://api.example.com/data")
        },
        3
    )

    result result
}
```

#### Effect Narrowing in Scopes

`with` blocks can narrow effect usage within specific scopes:

```cursive
// ============================================
// Controlled Effect Scopes
// ============================================

procedure safe_file_operation()
    uses FileSystem
{
    // Normal filesystem operations
    FileSystem::read("/tmp/data.txt")

    // Narrow to read-only within this scope
    with FileSystem {
        read(path) => {
            if path::starts_with("/system") {
                abort(Error::Forbidden)
            }
            continue(os_read(path))
        }

        write(path, data) => {
            // Block all writes in this scope
            abort(Error::ReadOnlyMode)
        }
    } {
        // Only reads succeed here
        FileSystem::read("/tmp/file.txt")  // ✅ OK
        // FileSystem::write would abort with ReadOnlyMode
    }
}
```

#### Multi-Effect Coordination

Complex operations coordinate multiple effect systems:

```cursive
// ============================================
// Database Transaction with Logging
// ============================================

effect Database {
    query(sql: string): Vec<Row> {
        os_query(sql)
    }

    execute(sql: string): i32 {
        os_execute(sql)
    }
}

procedure transfer_funds(from_account: i32, to_account: i32, amount: i32): Result<(), Error>
    uses Database, FileSystem.write, alloc.heap
    must amount > 0
{
    // Log transaction start
    FileSystem::write("/var/log/transactions.log", "Transfer started")

    // Execute database operations
    let debit_result = Database::execute(
        "UPDATE accounts SET balance = balance - {amount} WHERE id = {from_account}"
    )

    if debit_result == 0 {
        FileSystem::write("/var/log/transactions.log", "Transfer failed: invalid account")
        result Result::Err(Error::InvalidAccount)
    }

    let credit_result = Database::execute(
        "UPDATE accounts SET balance = balance + {amount} WHERE id = {to_account}"
    )

    if credit_result == 0 {
        // Rollback needed (simplified)
        FileSystem::write("/var/log/transactions.log", "Transfer failed: rollback")
        result Result::Err(Error::TransactionFailed)
    }

    FileSystem::write("/var/log/transactions.log", "Transfer completed")
    result Result::Ok(())
}
```

#### Effect Abstraction Layers

Build layered effect abstractions:

```cursive
// ============================================
// Logging Abstraction Layer
// ============================================

effect Logger {
    log(level: string, message: string): ()
}

// Concrete implementation using filesystem
procedure with_file_logger<T, ε>(
    log_path: string,
    operation: fn() -> T ! ε
): T
    uses FileSystem.write, ε
{
    with Logger {
        log(level, message) => {
            let entry = "{level}: {message}\n"
            FileSystem::write(log_path, entry)
            continue(())
        }
    } {
        result operation()
    }
}

// Usage: Logger effect is abstract, implementation is concrete
procedure do_work()
    uses Logger
{
    Logger::log("INFO", "Starting work")
    // ... work happens ...
    Logger::log("INFO", "Work completed")
}

procedure main() {
    with_file_logger("/var/log/app.log", fn() -> () ! {Logger} {
        do_work()
        result ()
    })
}
```

CITE: Part V §4.19 — Effect Composition; Part II §2.10 — Effect Types.

---

### 7.3.12 Effect Polymorphism Examples

This section provides comprehensive examples of effect-polymorphic programming patterns.

#### Generic Effect Parameters

Functions can be polymorphic over effect sets:

```cursive
// ============================================
// Effect-Polymorphic Map
// ============================================

function map<T, U, ε>(
    items: Vec<T>,
    transform: fn(T) -> U ! ε
): Vec<U>
    uses ε, alloc.heap
{
    var result: Vec<U> = Vec::new()

    for item in items {
        let transformed = transform(item)
        result::push(transformed)
    }

    result result
}

// Usage with pure transformation
procedure example_pure() {
    let numbers = vec![1, 2, 3, 4, 5]

    let doubled = map(
        numbers,
        fn(x: i32) -> i32 { result x * 2 }
    )
    // Effect ε = ∅ (pure)
}

// Usage with effectful transformation
procedure example_effectful()
    uses FileSystem.read
{
    let paths = vec!["/tmp/a.txt", "/tmp/b.txt", "/tmp/c.txt"]

    let contents = map(
        paths,
        fn(path: string) -> string ! {FileSystem.read} {
            result FileSystem::read(path)
        }
    )
    // Effect ε = {FileSystem.read}
}
```

#### Effect Bounds and Constraints

Constrain effect parameters for specific requirements:

```cursive
// ============================================
// Effect-Bounded Caching
// ============================================

record Cache<K, V, ε> {
    storage: HashMap<K, V>,
    loader: fn(K) -> V ! ε
}

// Constructor constrains that ε must include alloc.heap
function Cache::new<K, V, ε>(loader: fn(K) -> V ! ε) -> own Cache<K, V, ε>
    uses alloc.heap
    where ε includes alloc.heap  // Constraint: loader must use heap
{
    result Cache {
        storage: HashMap::new(),
        loader: loader
    }
}

procedure get<K, V, ε>(self: mut Cache<K, V, ε>, key: K): V
    uses ε, alloc.heap
    where K: Hash, K: Eq
{
    // Check cache
    if let Some(value) = self.storage::get(key) {
        result value
    }

    // Load and cache
    let value = (self.loader)(key)
    self.storage::insert(key, value)
    result value
}

// Usage
procedure example()
    uses FileSystem.read, alloc.heap
{
    var file_cache = Cache::new(
        fn(path: string) -> string ! {FileSystem.read, alloc.heap} {
            result FileSystem::read(path)
        }
    )

    let content = file_cache::get("/tmp/data.txt")
}
```

#### Higher-Order Effect Composition

Compose multiple effect-polymorphic operations:

```cursive
// ============================================
// Pipeline with Effect Composition
// ============================================

function compose<T, U, V, ε₁, ε₂>(
    f: fn(T) -> U ! ε₁,
    g: fn(U) -> V ! ε₂
): fn(T) -> V ! {ε₁ ∪ ε₂}
{
    result fn(x: T) -> V ! {ε₁ ∪ ε₂} {
        let intermediate = f(x)
        result g(intermediate)
    }
}

// Usage
procedure example()
    uses FileSystem.read, Network.send
{
    // read_file: string -> string ! {FileSystem.read}
    let read_file = fn(path: string) -> string ! {FileSystem.read} {
        result FileSystem::read(path)
    }

    // send_network: string -> bool ! {Network.send}
    let send_network = fn(data: string) -> bool ! {Network.send} {
        result Network::send("https://api.example.com", data)
    }

    // Composed: string -> bool ! {FileSystem.read, Network.send}
    let read_and_send = compose(read_file, send_network)

    let success = read_and_send("/tmp/data.txt")
}
```

#### Effect Inference with Polymorphism

The compiler infers effect parameters from usage:

```cursive
// ============================================
// Automatic Effect Inference
// ============================================

function apply_twice<T, ε>(f: fn(T) -> T ! ε, x: T): T
    uses ε
{
    let once = f(x)
    result f(once)
}

// Inference example 1: Pure function
procedure example_inferred_pure() {
    let result = apply_twice(
        fn(x: i32) -> i32 { result x + 1 },
        10
    )
    // Inferred: ε = ∅
    // result = 12
}

// Inference example 2: Effectful function
procedure example_inferred_effectful()
    uses FileSystem.write
{
    let result = apply_twice(
        fn(x: i32) -> i32 ! {FileSystem.write} {
            FileSystem::write("/tmp/counter.txt", x::to_string())
            result x + 1
        },
        10
    )
    // Inferred: ε = {FileSystem.write}
    // result = 12, writes "10" and "11" to file
}
```

#### Effect Polymorphic Data Structures

Data structures can be polymorphic over the effects of their contents:

```cursive
// ============================================
// Effect-Polymorphic Lazy Evaluation
// ============================================

record Lazy<T, ε> {
    thunk: Option<fn() -> T ! ε>,
    cached: Option<T>
}

function Lazy::new<T, ε>(thunk: fn() -> T ! ε) -> own Lazy<T, ε> {
    result Lazy {
        thunk: Option::Some(thunk),
        cached: Option::None
    }
}

procedure force<T, ε>(self: mut Lazy<T, ε>): T
    uses ε
    where T: Copy
{
    self.cached match {
        Option::Some(value) => {
            result value
        },
        Option::None => {
            self.thunk match {
                Option::Some(thunk) => {
                    let value = thunk()
                    self.cached = Option::Some(value)
                    self.thunk = Option::None
                    result value
                },
                Option::None => {
                    panic("Lazy value already consumed")
                }
            }
        }
    }
}

// Usage with effects
procedure example()
    uses FileSystem.read
{
    var lazy_content = Lazy::new(
        fn() -> string ! {FileSystem.read} {
            result FileSystem::read("/tmp/config.txt")
        }
    )

    // First access: reads file
    let content1 = lazy_content::force()

    // Second access: uses cached value
    let content2 = lazy_content::force()
}
```

#### Effect Polymorphic Contracts

Contracts can specify effect polymorphism requirements:

```cursive
// ============================================
// Effect-Polymorphic Contract
// ============================================

contract Processor<T, ε> {
    procedure process(self: mut Self, input: T): Result<T, Error>
        uses ε
}

// Implementation with specific effects
record FileProcessor: Processor<string, {FileSystem.read, FileSystem.write}> {
    log_path: string

    procedure process(self: mut Self, input: string): Result<string, Error>
        uses FileSystem.read, FileSystem.write
    {
        FileSystem::write(self.log_path, "Processing: {input}")

        let processed = input::to_uppercase()

        FileSystem::write(self.log_path, "Completed: {processed}")

        result Result::Ok(processed)
    }
}

// Implementation with different effects
record NetworkProcessor: Processor<string, {Network.send, alloc.heap}> {
    endpoint: string

    procedure process(self: mut Self, input: string): Result<string, Error>
        uses Network.send, alloc.heap
    {
        let response = Network::send(self.endpoint, input)

        response match {
            Ok(data) => {
                result Result::Ok(data)
            },
            Err(e) => {
                result Result::Err(e)
            }
        }
    }
}

// Generic function using contract
function pipeline<T, P, ε>(processor: mut P, inputs: Vec<T>): Vec<Result<T, Error>>
    uses ε, alloc.heap
    where P: Processor<T, ε>
{
    var results: Vec<Result<T, Error>> = Vec::new()

    for input in inputs {
        let result = processor::process(input)
        results::push(result)
    }

    result results
}
```

CITE: Part II §2.10 — Effect Types; Part V §4.19 — Effect Composition; §7.2 — Behavioral Contracts.

---

## Summary

The unified effect system provides:

1. **Static effect tracking** - All effects declared and checked at compile time
2. **Capability-based security** - Effects are unforgeable tokens
3. **Implementation flexibility** - Effects can be overridden for testing/mocking
4. **Zero-cost abstraction** - Default implementations are inlined
5. **Ergonomic syntax** - Implicit perform, implicit continue, natural `with` blocks

**Key features:**

- Three kinds of effects (marker, with default, without default)
- Fine-grained capabilities (per-operation access control)
- External and internal custom implementations
- Explicit control flow (`continue`, `abort`)
- Effect polymorphism and composition
- Comprehensive examples (testing, async, generators, transactions, non-determinism)

This system enables powerful patterns while maintaining Cursive's core principles of explicitness, local reasoning, and zero-cost abstraction.


## 7.4 Precondition Clauses (`must`)

Preconditions declare requirements that MUST hold at procedure entry. If a caller cannot prove a precondition statically, the compiler emits a runtime check in verification modes that require it (§7.8).

## Definition 7.3 (Precondition Clauses)

A precondition clause is a predicate over the procedure’s parameters (and immutable state reachable from them) that MUST evaluate to `true` at call time. The typing judgment `[Must-WF]` ensures the predicate is pure (`effects(P) = ∅`) and has type `bool`. CITE: Appendix A.7 — MustClause; Appendix A.8 — Assertion Grammar.

### 7.4.1 Syntax

```cursive
procedure divide(n: f64, d: f64): f64
    must d != 0.0
{
    n / d
}
```

Preconditions consist of either a single assertion or a brace-delimited list of assertions separated by commas (the `PredicateBlock` production in Appendix A.7). Each assertion MUST be a pure boolean expression referencing parameters (and possibly constant/pure helper functions).

Formal syntax is given in Appendix A.7 (`MustClause ::= "must" PredicateBlock`). A predicate block is either a single assertion or a brace-delimited list using the assertion grammar from Appendix A.8.

### 7.4.2 Static Semantics

```
[Must-WF]
Γ, params ⊢ P : bool
effects(P) = ∅
────────────────────────────────
must P well-formed

[Call-Must]
Γ ⊢ f : (params) → τ must P
Γ ⊢ args : params
Γ ⊢ P[params ↦ args] provable or checkable
──────────────────────────────────────────────
Call to f(args) permitted
```

Implementations of a contract procedure MAY only weaken preconditions (`P_contract ⇒ P_impl`). Strengthening emits diagnostic E7C07.

"Provable" means the verification pipeline described in Part VI §6.13 discharges the assertion statically (`[[verify(static)]]` or stronger analyses); "checkable" means the runtime mode in effect is permitted to insert a dynamic guard (e.g., `[[verify(runtime)]]`). If neither condition holds, the call is rejected during type checking.

#### 7.4.2.1 Provable vs checkable calls

Let `call f(args)` be a procedure invocation with precondition `P`.

* The call is **provable** when Part VI’s static verifier establishes `Γ ⊢ P[params ↦ args]` using compile-time reasoning (`[[verify(static)]]` or stronger modes).
* The call is **checkable** when the compilation pipeline is configured to emit a runtime guard that enforces `P` (typically via `[[verify(runtime)]]`), and the surrounding effect set includes `panic` if the guard can fail.

Calls that are neither provable nor checkable are ill-formed. CITE: Part VI §6.13 — Block Contracts and Verification Modes.

### 7.4.3 Dynamic Semantics

```
[Must-Static]
Γ ⊢ P[params ↦ args] proved true by analysis or constant evaluation
──────────────────────────────────────────────────────────────
No runtime check emitted

[Must-Runtime]
Γ ⊢ P[params ↦ args] not proven
──────────────────────────────────────────────────────────────
Emit runtime assertion: if !P(args) panic("precondition violated: …")
```

Runtime diagnostics follow the format in Part I §8 (E7C02). Verification attributes (`[[verify(static)]]`, `[[verify(runtime)]]`, `[[verify(none)]]`) in Part VI §6.13 control whether the runtime assertion is generated.
CITE: Part VI §6.13 — Verification Modes; Part I §8 — Diagnostics Directory.

The precondition guard executes before the body; if evaluating the guard panics (for example, due to a divide-by-zero inside the predicate), the call fails with E7C02 rather than propagating into the body.

---

### 7.4.4 Predicate expression restrictions

Contract predicates reuse the assertion grammar in Appendix A.8. Consequently:

* Predicates MUST be pure; any expression with non-empty effects is rejected.
* Calls inside predicates are limited to declarations whose effect set is `∅`.
* Quantifiers (`forall`, `exists`), boolean connectives, comparisons, and `@old` are permitted; arbitrary loops or effectful constructs are not.
* Accessing heap data is allowed only through immutable views. Mutating a captured value violates the purity requirement and fails the `[Must-WF]` rule.

Implementations SHOULD surface precise diagnostics indicating which subexpression introduced disallowed effects.

---

### 7.4.5 Quantifiers in predicates

Quantified predicates reuse the assertion grammar productions `forall` and `exists` from Appendix A.8. A quantifier has the form `forall (x in expr) { Predicate }` (likewise for `exists`) where `expr` evaluates to an iterable domain at verification time. The verifier interprets the body predicate with `x` bound to each element of the domain. Runtime checks are emitted only when the verifier cannot statically prove the quantified statement. CITE: Appendix A.8 — Assertion Grammar.

```cursive
procedure sorted(slice: [i32])
    will forall (i in 0 .. slice.len() - 2) {
        slice[i] <= slice[i + 1]
    }
{
    // body that establishes ascending order
}
```

Quantifiers MUST be pure (their body predicates obey `[Must-WF]` / `[Will-WF]`), and any domain expression appearing inside a quantifier is evaluated exactly once per dynamic check to avoid recomputation side effects.

---

## 7.5 Postcondition Clauses (`will`)

Postconditions describe guarantees that hold whenever a procedure returns normally. They MAY reference the return value via `result` and pre-state expressions via `@old(expr)`.

## Definition 7.4 (Postcondition Clauses)

A postcondition clause is a predicate over the procedure’s result (and optionally the pre-state captured through `@old`) that MUST hold on every normal return path. The `[Will-WF]` rule types these predicates and enforces purity, while `[Will-Compat]` governs contract substitution. CITE: Appendix A.7 — WillClause; Appendix A.8 — Assertion Grammar.

### 7.5.1 Syntax

```cursive
procedure increment(x: i32): i32
    will result == x + 1
{
    x + 1
}
```

For mutating procedures:

```cursive
procedure push<T>(buffer: mut Vec<T>, item: T)
    will buffer.len() == @old(buffer.len()) + 1
{
    buffer.push(item)
}
```

```ebnf
WillClause  ::= "will" Predicate ("," Predicate)*
Predicate   ::= Expression  // May mention result and @old(expr)
```

`result` denotes the return value of the procedure. `@old(expr)` captures the value of `expr` at procedure entry; `expr` MAY reference parameters (and fields reachable from `self`) and MUST be pure.

### 7.5.2 Static Semantics

```
[Will-WF]
Γ, params, result: τ ⊢ Q : bool
@old expressions reference only parameters (and their accessible fields)
────────────────────────────────────────────
will Q well-formed

[Will-Compat]
Contract postcondition Q_c, implementation postcondition Q_m
Q_m ⇒ Q_c
────────────────────────────────────────────
Implementation satisfies contract postcondition
```

### 7.5.3 Dynamic Semantics

```
[Will-Capture]
@old(expr) appears in Q
────────────────────────────────────────────
Evaluate expr at entry; retain snapshot for later comparison

[Will-Static]
Analysis proves Q holds on every return path
────────────────────────────────────────────
No runtime check emitted

[Will-Runtime]
Cannot prove Q statically
────────────────────────────────────────────
Evaluate Q at each return; if false panic("postcondition violated: …")
```

Violations raise diagnostic E7C03. Verification attributes in Part VI §6.13 determine whether runtime checks are emitted.

Implementations MAY apply the same optimization strategy as preconditions: if static analysis or `[[verify(static)]]` proves the postconditions, the runtime checks are elided; `[[verify(runtime)]]` retains the checks for debugging or safety-critical builds.
CITE: Part VI §6.13 — Verification Modes; Part I §8 — Diagnostics Directory.

Every return site (explicit `return`, fall-through, or early exit) executes the postcondition check; compilers MUST NOT defer the evaluation to a single epilogue if doing so would miss intermediate exits.
CITE: Part VI §6.13 — Verification Modes; Part I §8 — Diagnostics Directory.

---

### 7.5.4 `@old` capture semantics and exceptional exits

* Each `@old(expr)` is evaluated once on entry, before any mutation occurs. The expression MUST satisfy the same purity requirements as preconditions (`effects(expr) = ∅`).
* Captured values are logically immutable snapshots; moving or mutating the underlying storage after capture does not affect the recorded value.
* For `Copy` types the snapshot is a by-value copy taken at entry. For non-`Copy` types, the snapshot expression MUST either call a pure helper that produces an immutable copy (e.g., a `Clone` function whose effects are declared) or use an equivalent copy-on-write strategy that preserves the entry state. CITE: Part II §2.3 — Copy Types.
* Nested `@old(@old(expr))` is ill-formed; only one level is permitted.
* If a procedure panics or otherwise diverges, its `will` clauses are not enforced—the guarantees only apply to normal returns.

Expressions whose values cannot be duplicated (e.g., unique resources or region-limited references) are ineligible for `@old` unless the program explicitly provides a safe, effect-free snapshot mechanism (such as a documented `clone` procedure). Attempting to capture such a value with `@old` emits diagnostic E7C16 (see §7.8). CITE: Part I §8 — Diagnostics Directory.

**Example — snapshotting state:**

```cursive
procedure pop(mut stack: Vec<i32>): i32
    must stack.len() > 0
    will stack.len() == @old(stack.len()) - 1
{
    stack.pop().expect("stack underflow")
}
```

---

## 7.6 Type Constraint Clauses (`where`)

Type-level `where` clauses define invariants that MUST hold for every externally visible instance of a nominal type.

*Terminology:* In this chapter "invariant" refers to type invariants attached to nominal declarations. These are distinct from block-level invariants introduced in Part VI §6.13, which govern statement scopes rather than type structure.

## Definition 7.5 (Type Constraint Clauses)

A type constraint clause `where { Invariant* }` specifies predicates that MUST hold for every observable state of the declared type. Each invariant is an assertion over the type’s fields evaluated in both static and dynamic contexts as described in §7.6.2–§7.6.4. CITE: Appendix A.6 — TypeConstraint; Appendix A.8 — Assertion Grammar.

### 7.6.1 Syntax

```cursive
record Percentage {
    value: i32
}
where {
    value >= 0,
    value <= 100
}
```

```ebnf
TypeConstraint ::= "where" "{" Invariant ("," Invariant)* "}"
Invariant      ::= Expression  // Must be pure bool over the type’s fields
```

### 7.6.2 Static Semantics

```
[Where-WF]
Γ, fields ⊢ Invariantᵢ : bool
effects(Invariantᵢ) = ∅
────────────────────────────────────────
record T where { Invariant* } well-formed

[Where-Update]
record T where { Invariant* }
public operation mutates T
────────────────────────────────────────
Operation must ensure each invariant holds after mutation
```

### 7.6.3 Dynamic Semantics

Where invariants cannot be discharged statically, the compiler inserts checks according to the following formal rules:

```
[Where-Check-Construct]
record T where { Invariant₁, ..., Invariantₙ }
value v : T constructed
∀i. Invariantᵢ[fields ↦ v.fields] not proven statically
────────────────────────────────────────────────────────────────
Emit runtime check: if !Invariantᵢ(v) panic("invariant violated: ...")

[Where-Check-MutatingExit]
procedure p(self: mut T) where T has invariants { Invariant₁, ..., Invariantₙ }
exit path e (normal return or early exit)
∀i. Invariantᵢ[fields ↦ self.fields] not proven statically at e
────────────────────────────────────────────────────────────────
Emit runtime check at e: if !Invariantᵢ(self) panic("invariant violated: ...")

[Where-Check-FieldWrite]
record T where { Invariant₁, ..., Invariantₙ }
field write: t.field = value
∀i. Invariantᵢ[fields ↦ t.fields] not proven statically after write
────────────────────────────────────────────────────────────────
Emit runtime check after write: if !Invariantᵢ(t) panic("invariant violated: ...")

[Where-Check-ReturnByValue]
procedure p(...): T where T has invariants { Invariant₁, ..., Invariantₙ }
return value v : T
∀i. Invariantᵢ[fields ↦ v.fields] not proven statically
────────────────────────────────────────────────────────────────
Emit runtime check before return: if !Invariantᵢ(v) panic("invariant violated: ...")

[Where-Check-Elision]
Invariant I proven statically at site s
────────────────────────────────────────────────────────────────
No runtime check emitted for I at s
```

**Check site semantics:**

* *Construct* events include record literals, builder functions, and composite initializers that yield `T`. Constructors with explicit bodies run to completion first; the check executes immediately afterward using the fully initialized state.
* *MutatingExit* applies to any procedure (public or private) whose signature grants mutable access (`self: mut Self`, `mut T`, etc.). Private helpers MAY temporarily violate invariants internally provided they restore them before returning to a caller visible outside the defining module.
* *FieldWrite* checks apply when a field is directly assigned. The compiler MAY batch multiple field writes and check invariants once after the final write if it can prove no observable intermediate state exists.
* *ReturnByValue* checks apply when a procedure returns a value of type `T` by value (not by reference).

**Unsafe code obligations:** Mutations performed through unsafe pointers (`unsafe.ptr`) transfer responsibility to the surrounding scope; failing to re-establish the invariant triggers E7C05. When a type participates in inheritance, every override is responsible for re-validating the invariants declared by its ancestors in addition to its own declarations.

If static verification (e.g., via `[[verify(static)]]`) proves the invariants for a site, the corresponding runtime checks are suppressed. Violations raise diagnostics E7C04 (construction) or E7C05 (mutation). CITE: Part VI §6.13 — Verification Modes; Part I §8 — Diagnostics Directory.

### 7.6.4 Performance considerations

Repeated invariant checking can be expensive for large aggregates. Implementations SHOULD:

* combine multiple checks on the same path into a single verification point;
* respect `[[verify(runtime)]]` / `[[verify(none)]]` attributes, falling back to runtime only when mandated; and
* allow profiling hooks that identify hotspots where additional static proof annotations would remove checks.

Because invariants are part of the public contract, compilers MUST NOT omit checks without proof that the constraints hold.
CITE: Part VI §6.13 — Verification Modes; Part I §8 — Diagnostics Directory.

#### Performance complexity analysis

**Time complexity:**

Let `T` be a type with `n` invariants, each of complexity `O(f(k))` where `k` is the size of the type's data:

* **Construction check:** `O(n · f(k))` — all invariants evaluated once
* **Mutation check:** `O(n · f(k))` per mutating operation exit
* **Field write check:** `O(n · f(k))` per write (unless batched)
* **Return check:** `O(n · f(k))` per return site

**Optimization strategies:**

1. **Static elimination:** When the verifier proves an invariant holds, the check is elided entirely (`O(0)`)
2. **Batching:** Multiple field writes in sequence can share a single check at the end (`O(n · f(k))` instead of `m · O(n · f(k))` for `m` writes)
3. **Incremental checking:** For invariants that depend on a subset of fields, only re-check when those fields change
4. **Caching:** For pure invariants over immutable data, cache the result and reuse it

**Space complexity:**

* **`@old` captures:** `O(k)` per postcondition using `@old`, where `k` is the size of captured data
* **Invariant evaluation:** `O(1)` stack space for predicate evaluation (assuming no deep recursion)
* **Check metadata:** `O(n)` per type for storing invariant predicates

**Worst-case scenario:** A type with `n` complex invariants (`O(k²)` each) mutated `m` times in a tight loop incurs `O(m · n · k²)` overhead. Implementations SHOULD warn when invariant checking dominates execution time and suggest static verification or check elision strategies.

**Best practices for performance:**

* Keep invariants simple (`O(1)` or `O(k)` complexity preferred)
* Use `[[verify(static)]]` to eliminate checks in hot paths
* Batch related mutations to share invariant checks
* Consider splitting types with expensive invariants into smaller components

---

## 7.7 Clause Composition

### 7.7.1 Effect propagation

```
[Compose-Effects]
Γ ⊢ f : (params) → τ ! ε_f
caller budget ε_call
─────────────────────────────────────────────
call permitted only if ε_f ⊆ ε_call
```

Composite expressions and statement blocks accumulate effects using the union rules in Part V §4.19.4; procedure calls inherit the callee’s declared set. For effect-polymorphic callees (`∀ε`), the instantiated effect set determined by inference (see §7.3.8) is the one checked against the caller budget. Failure raises E7C01 or E4004. CITE: Part V §4.19.4 — Effect Union.

E7C01 is emitted when a procedure’s body performs an operation requiring an undeclared effect; E4004 is emitted when a caller’s effect budget does not subsume the callee’s requirements. Diagnostics SHOULD clearly report the offending effect token. CITE: Part I §8 — Diagnostics Directory.

### 7.7.2 Preconditions across call chains

```
[Compose-Pre]
Γ ⊢ f : (params) → τ must P
Γ ⊢ args : params
Γ ⊢ P[params ↦ args] provable or checkable
─────────────────────────────────────────────
Γ ⊢ call f(args) safe
```

Implementations MAY weaken inherited preconditions (`P_contract ⇒ P_impl`) but MUST NOT strengthen them (§7.2.3); violations trigger E7C07. "Weaker" here means the implementation accepts every argument the contract allows (possibly more). For multiple inheritance, the implementation MUST satisfy the conjunction of all inherited preconditions.
CITE: Part I §8 — Diagnostics Directory.

**Example — Valid weakening:**

```cursive
contract Positive {
    procedure consume(n: i32)
        must n > 0
}

record Counter: Positive {
    value: i32

    procedure consume(self: mut Self, n: i32)
        must n >= 0    // allowed: (n > 0) ⇒ (n >= 0)
    {
        if n == 0 {
            // additional case permitted by the weaker precondition
            return
        }
        self.value -= n
    }
}
```

### 7.7.3 Postconditions and assumptions

```
[Compose-Post]
Γ ⊢ f : … will Q
call returns value v
─────────────────────────────────────────────
Caller may assume Q[params ↦ args, result ↦ v]
```

Implementations MUST strengthen inherited `will` clauses (`Q_impl ⇒ Q_contract`); weakening emits E7C08. Callers MAY rely on the conjunction of postconditions from all contracts satisfied by the callee.
CITE: Part I §8 — Diagnostics Directory.

**Example — Violated strengthening (diagnostic E7C08):**

```cursive
contract Sized {
    procedure len(self: Self): usize
        will result >= 0
}

record MaybeSized: Sized {
    procedure len(self: Self): usize
        will result <= 0   // incompatible: does not imply result >= 0
    { 0 }
}
```

### 7.7.4 Substitution checklist

When substituting an implementation for a contract member, the compiler MUST verify:

1. `uses`: implementation effect set ⊆ contract effect set (else E7C09).
2. `must`: implementation precondition is weaker or equal (formally `P_contract ⇒ P_impl`, else E7C07).
3. `will`: implementation postcondition is stronger or equal (else E7C08).

Only after these checks succeed does the call graph treat the implementation as a drop-in replacement for the contract signature. Diagnostics MUST identify which clause failed and, when applicable, the conflicting contracts in an inheritance graph.
CITE: Part I §8 — Diagnostics Directory.

---

## 7.Y Integration with Language Features

This section demonstrates how contracts interact with other Cursive language features, including functions, permissions, regions, modal types, generics, and unsafe code.

### 7.Y.1 Contracts and Functions

Contracts specify behavioral requirements for both procedures (methods) and standalone functions.

#### Function-Scope Contracts

Standalone functions can fulfill contract requirements:

```cursive
contract Transformer<T> {
    procedure transform(self: imm Self, input: T): T
        uses alloc.heap
}

// Record implementing contract via procedure
record Doubler: Transformer<i32> {
    factor: i32

    procedure transform(self: imm Self, input: i32): i32
        uses alloc.heap
    {
        result input * self.factor
    }
}

// Function implementing contract behavior (no Self)
function transform_uppercase(input: string): string
    uses alloc.heap
{
    result input::to_uppercase()
}
```

#### Generic Functions with Contract Bounds

Generic functions can require contract implementations:

```cursive
contract Serializable {
    procedure to_json(self: imm Self): string
        uses alloc.heap
}

function save_to_file<T>(item: T, path: string)
    uses FileSystem.write, alloc.heap
    where T: Serializable
{
    let json = item::to_json()
    FileSystem::write(path, json)
}

record User: Serializable {
    name: string,
    age: i32

    procedure to_json(self: imm Self): string
        uses alloc.heap
    {
        result string::from("{\"name\": \"{self.name}\", \"age\": {self.age}}")
    }
}

procedure example() {
    let user = User { name: string::from("Alice"), age: 30 }
    save_to_file(user, "/tmp/user.json")  // ✅ OK: User implements Serializable
}
```

#### Higher-Order Functions with Contracts

Contract requirements can be passed through higher-order functions:

```cursive
contract Validator<T> {
    procedure validate(self: imm Self, value: T): bool
}

function filter_valid<T, V>(items: Vec<T>, validator: V): Vec<T>
    uses alloc.heap
    where V: Validator<T>, T: Copy
{
    var result: Vec<T> = Vec::new()

    for item in items {
        if validator::validate(item) {
            result::push(item)
        }
    }

    result result
}

record RangeValidator: Validator<i32> {
    min: i32,
    max: i32

    procedure validate(self: imm Self, value: i32): bool {
        result value >= self.min && value <= self.max
    }
}

procedure example() {
    let numbers = vec![1, 5, 10, 15, 20, 25]
    let validator = RangeValidator { min: 5, max: 20 }
    let valid_numbers = filter_valid(numbers, validator)
    // Result: [5, 10, 15, 20]
}
```

CITE: Part V §4.x — Function Types; Part IX §9.x — Generic Functions.

---

### 7.Y.2 Contracts and Permissions

Contracts interact with Cursive's permission system (own/mut/imm) to specify access requirements.

#### Permission-Aware Contracts

Contract procedures can specify different permission requirements:

```cursive
contract Resource {
    // Immutable inspection
    procedure is_valid(self: imm Self): bool

    // Mutable modification
    procedure update(self: mut Self, data: string)
        uses alloc.heap

    // Ownership consumption
    procedure close(self: own Self)
        uses alloc.heap
}

record FileHandle: Resource {
    path: string,
    is_open: bool

    procedure is_valid(self: imm Self): bool {
        result self.is_open
    }

    procedure update(self: mut Self, data: string)
        uses alloc.heap
    {
        if self.is_open {
            // Write data
            self.is_open = true
        }
    }

    procedure close(self: own Self)
        uses alloc.heap
    {
        // Consume handle
        // self is moved here, cannot be used after
    }
}
```

#### Permission Coercion with Contracts

Implementations must respect permission lattice (own → mut → imm):

```cursive
contract Readable {
    procedure read(self: imm Self): string
        uses alloc.heap
}

record Config: Readable {
    data: string

    procedure read(self: imm Self): string
        uses alloc.heap
    {
        result self.data
    }
}

procedure use_readable<R>(resource: imm R): string
    uses alloc.heap
    where R: Readable
{
    result resource::read()
}

procedure example() {
    let config = Config { data: string::from("settings") }

    // All permission levels can be passed where imm is required
    let s1 = use_readable(imm config)  // ✅ imm
    let s2 = use_readable(mut config)  // ✅ mut coerces to imm
    let s3 = use_readable(own config)  // ✅ own coerces to imm
}
```

#### Contracts with Permission Polymorphism

Contracts can be polymorphic over permissions:

```cursive
contract Container<T, P> {
    procedure get(self: P Self): Option<T>
        where P in {imm, mut, own}
}

record Box<T>: Container<T, imm>, Container<T, mut>, Container<T, own> {
    value: Option<T>

    procedure get(self: imm Self): Option<T>
        where T: Copy
    {
        result self.value
    }

    procedure get(self: mut Self): Option<T> {
        result self.value
    }

    procedure get(self: own Self): Option<T> {
        result self.value
    }
}
```

CITE: Part IV §4.x — Permission System; Part III §3.x — Permission Lattice.

---

### 7.Y.3 Contracts and Modal Types

Contracts can enforce protocol compliance through modal state requirements.

#### Modal State Contracts

Contracts specify required modal states for operations:

```cursive
modal Connection {
    @Disconnected { address: string }
    @Connected { address: string, socket: i32 }
}

contract NetworkProtocol {
    procedure connect(self: mut Connection@Disconnected): Connection@Connected
        uses network.connect

    procedure send(self: mut Connection@Connected, data: [u8]): Result<usize, Error>
        uses network.send
        must data::len() > 0

    procedure disconnect(self: mut Connection@Connected): Connection@Disconnected
        uses network.disconnect
}

record TcpConnection: NetworkProtocol {
    timeout: i32

    procedure connect(self: mut Connection@Disconnected): Connection@Connected
        uses network.connect
    {
        let socket = network_connect(self.address, self.timeout)

        result Connection@Connected {
            address: self.address,
            socket: socket
        }
    }

    procedure send(self: mut Connection@Connected, data: [u8]): Result<usize, Error>
        uses network.send
        must data::len() > 0
    {
        result network_send(self.socket, data)
    }

    procedure disconnect(self: mut Connection@Connected): Connection@Disconnected
        uses network.disconnect
    {
        network_close(self.socket)

        result Connection@Disconnected {
            address: self.address
        }
    }
}
```

#### Modal Transitions in Contracts

Contracts enforce state machine protocols:

```cursive
modal Transaction {
    @NotStarted
    @Active { id: i32 }
    @Committed { id: i32 }
    @RolledBack { id: i32, reason: string }
}

contract TransactionProtocol {
    procedure begin(self: mut Transaction@NotStarted): Transaction@Active
        uses Database

    procedure commit(self: mut Transaction@Active): Transaction@Committed
        uses Database

    procedure rollback(self: mut Transaction@Active, reason: string): Transaction@RolledBack
        uses Database
}

// Usage enforces correct state transitions at compile time
procedure safe_transaction<T>(db: mut T)
    uses Database
    where T: TransactionProtocol
{
    var tx = Transaction@NotStarted

    var active_tx = tx::begin()  // NotStarted → Active

    // Can only commit or rollback from Active state
    let committed_tx = active_tx::commit()  // Active → Committed

    // ⚠ ERROR: Cannot call rollback on committed transaction
    // committed_tx::rollback("reason")
}
```

CITE: Part X §10.x — Modal Types; Part VI §6.x — State Machine Verification.

---

### 7.Y.4 Contracts and Generics

Contracts work seamlessly with Cursive's generic type system.

#### Generic Contract Implementations

Contracts can be implemented for generic types:

```cursive
contract Comparable {
    procedure compare(self: imm Self, other: imm Self): Ordering
}

enum Ordering {
    Less,
    Equal,
    Greater
}

record Pair<T, U>: Comparable
    where T: Comparable, U: Comparable
{
    first: T,
    second: U

    procedure compare(self: imm Self, other: imm Self): Ordering {
        let first_cmp = self.first::compare(other.first)

        first_cmp match {
            Ordering::Equal => {
                result self.second::compare(other.second)
            },
            _ => {
                result first_cmp
            }
        }
    }
}
```

#### Associated Types in Generic Contracts

Contracts with associated types enable flexible abstractions:

```cursive
contract Collection {
    type Item
    type Iterator: Iterator<Self::Item>

    procedure iter(self: imm Self): own Self::Iterator
        uses alloc.heap

    procedure len(self: imm Self): usize

    procedure is_empty(self: imm Self): bool
}

record Vec<T>: Collection {
    items: [T]
    count: usize

    type Item = T
    type Iterator = VecIterator<T>

    procedure iter(self: imm Self): own VecIterator<T>
        uses alloc.heap
    {
        result VecIterator { data: self.items, index: 0 }
    }

    procedure len(self: imm Self): usize {
        result self.count
    }

    procedure is_empty(self: imm Self): bool {
        result self.count == 0
    }
}
```

#### Generic Contracts with Effect Polymorphism

Combine generics, contracts, and effects:

```cursive
contract Mapper<T, U, ε> {
    procedure map(self: imm Self, f: fn(T) -> U ! ε): Vec<U>
        uses ε, alloc.heap
}

record DataSet<T>: Mapper<T, U, ε>
    where T: Copy
{
    data: Vec<T>

    procedure map<U, ε>(self: imm Self, f: fn(T) -> U ! ε): Vec<U>
        uses ε, alloc.heap
    {
        var result: Vec<U> = Vec::new()

        for item in self.data {
            let transformed = f(item)
            result::push(transformed)
        }

        result result
    }
}

// Usage with different effect contexts
procedure example()
    uses FileSystem.write
{
    let dataset = DataSet { data: vec![1, 2, 3] }

    // Pure mapping
    let doubled = dataset::map(fn(x: i32) -> i32 { result x * 2 })

    // Effectful mapping
    let logged = dataset::map(fn(x: i32) -> i32 ! {FileSystem.write} {
        FileSystem::write("/tmp/log.txt", x::to_string())
        result x
    })
}
```

CITE: Part II §2.x — Generic Types; Part V §4.x — Type Parameters.

---

### 7.Y.5 Contracts and Unsafe Code

Contracts can specify safety requirements for unsafe operations.

#### Unsafe Contracts

Contracts that require unsafe operations declare them explicitly:

```cursive
contract RawPointer<T> {
    procedure from_raw(ptr: *const T): own Self
        uses unsafe.transmute
        must ptr != null

    procedure as_ptr(self: imm Self): *const T
        uses unsafe.transmute

    procedure read(self: imm Self): T
        uses unsafe.transmute
        must self::as_ptr() != null
}

record Ptr<T>: RawPointer<T> {
    raw: *const T

    procedure from_raw(ptr: *const T): own Ptr<T>
        uses unsafe.transmute
        must ptr != null
    {
        result Ptr { raw: ptr }
    }

    procedure as_ptr(self: imm Self): *const T
        uses unsafe.transmute
    {
        result self.raw
    }

    procedure read(self: imm Self): T
        uses unsafe.transmute
        must self::as_ptr() != null
    {
        unsafe {
            result *self.raw
        }
    }
}
```

#### Safe Wrappers for Unsafe Contracts

Contracts can provide safe abstractions over unsafe implementations:

```cursive
contract Allocator {
    procedure allocate(self: mut Self, size: usize): *mut u8
        uses unsafe.transmute, alloc.heap
        must size > 0
        will result != null

    procedure deallocate(self: mut Self, ptr: *mut u8, size: usize)
        uses unsafe.transmute, alloc.heap
        must ptr != null
        must size > 0
}

record HeapAllocator: Allocator {
    allocated: usize

    procedure allocate(self: mut Self, size: usize): *mut u8
        uses unsafe.transmute, alloc.heap
        must size > 0
        will result != null
    {
        unsafe {
            let ptr = os_malloc(size)
            self.allocated = self.allocated + size
            result ptr
        }
    }

    procedure deallocate(self: mut Self, ptr: *mut u8, size: usize)
        uses unsafe.transmute, alloc.heap
        must ptr != null
        must size > 0
    {
        unsafe {
            os_free(ptr)
            self.allocated = self.allocated - size
        }
    }
}

// Safe wrapper using the unsafe contract
function with_allocation<T>(allocator: mut HeapAllocator, operation: fn(*mut u8) -> T): T
    uses unsafe.transmute, alloc.heap
{
    let ptr = allocator::allocate(1024)

    let result = operation(ptr)

    allocator::deallocate(ptr, 1024)

    result result
}
```

#### Verified Unsafe Operations

Contracts specify verification conditions for unsafe code:

```cursive
contract MemoryCopy {
    procedure copy_memory(dest: *mut u8, src: *const u8, count: usize)
        uses unsafe.transmute
        must dest != null
        must src != null
        must count > 0
        must dest + count <= src || src + count <= dest  // No overlap
        will forall i in 0..count: *(dest + i) == *(src + i)
}

record FastMemCopy: MemoryCopy {
    procedure copy_memory(dest: *mut u8, src: *const u8, count: usize)
        uses unsafe.transmute
        must dest != null
        must src != null
        must count > 0
        must dest + count <= src || src + count <= dest
        will forall i in 0..count: *(dest + i) == *(src + i)
    {
        unsafe {
            memcpy(dest, src, count)
        }
    }
}
```

CITE: Part XV §15.x — Unsafe Code; Part IV §4.x — Unsafe Effects.

---

## 7.8 Diagnostics

This section catalogs all errors, warnings, and lints related to contracts, effects, preconditions, postconditions, and type invariants.

### 7.8.1 Contract Declaration Errors (E7C-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7C-0101 | Contract procedure contains body (contracts must be abstract) | `contract C { procedure p() { ... } }` |
| E7C-0102 | Contract extends itself (direct or indirect cycle) | `contract C extends C` |
| E7C-0103 | Conflicting inherited contract requirements | Contract extends two contracts with incompatible signatures |
| E7C-0104 | Recursive associated type definition | `contract C { type T = C::T }` |
| E7C-0105 | Contract associated type lacks required bounds | Implementation doesn't satisfy bounds |
| E7C-0106 | Duplicate procedure signature in contract | Same procedure declared twice |
| E7C-0107 | Duplicate contract clause in signature | `uses fs.read, fs.read` |

### 7.8.2 Contract Implementation Errors (E7C-02xx)

| Code | Description | Example |
|------|-------------|---------|
| E7C-0201 | Missing implementation for contract member | Type implements contract but omits procedure |
| E7C-0202 | Implementation signature does not match contract | Wrong parameter types or return type |
| E7C-0203 | Implementation strengthens contract precondition | `must` weaker in contract, stronger in implementation |
| E7C-0204 | Implementation weakens contract postcondition | `will` stronger in contract, weaker in implementation |
| E7C-0205 | Implementation effect set exceeds contract declaration | Implementation uses undeclared effects |
| E7C-0206 | Orphan contract implementation | Neither type nor contract is local to crate |
| E7C-0207 | Duplicate contract implementation for type | Type implements same contract twice |
| E7C-0208 | Associated type not specified in implementation | Missing `type Item = T` |

### 7.8.3 Effect System Errors (E7E-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7E-0101 | Effect operation names not distinct | `effect E { op(), op() }` |
| E7E-0102 | Effect operation body type mismatch | Body returns wrong type |
| E7E-0103 | Effect path must be valid identifier | `effect 123.invalid` |
| E7E-0104 | Marker effect cannot have operations | `effect marker { op() }` |
| E7E-0105 | `continue()` only allowed in effect implementations | Used outside effect body |
| E7E-0106 | `abort()` only allowed in effect implementations | Used outside effect body |
| E7E-0107 | `continue()` type mismatch with operation return | Continuing with wrong type |
| E7E-0108 | Undeclared effect used in body | Effect not in `uses` clause |
| E7E-0109 | Undeclared effect operation used | Specific operation not granted |
| E7E-0110 | Effect not available in context | Called function requires unavailable effect |
| E7E-0111 | `with` block implementation signature mismatch | Handler doesn't match operation |
| E7E-0112 | `with` block references undefined effect | Unknown effect in `with` |
| E7E-0113 | `with` block references undefined operation | Unknown operation in handler |
| E7E-0114 | Effect requires implementation but none provided | No default and no `with` block |
| E7E-0115 | Multiple `continue()` in non-copyable context | Continuation consumed twice |

### 7.8.4 Precondition Errors (E7M-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7M-0101 | Precondition violated at runtime | `must` clause evaluated to false |
| E7M-0102 | Precondition contains non-pure expression | `must` uses effectful operation |
| E7M-0103 | Precondition type is not `bool` | `must` clause doesn't evaluate to boolean |
| E7M-0104 | Precondition references undefined parameter | `must x > 0` where x not a parameter |
| E7M-0105 | Precondition captures mutable state | `must` references non-immutable data |

### 7.8.5 Postcondition Errors (E7W-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7W-0101 | Postcondition violated at runtime | `will` clause evaluated to false |
| E7W-0102 | Postcondition contains non-pure expression | `will` uses effectful operation |
| E7W-0103 | Postcondition type is not `bool` | `will` doesn't evaluate to boolean |
| E7W-0104 | Postcondition references unavailable binding | `will x > 0` where x not in scope |
| E7W-0105 | Illegal `@old` capture (nested or non-snapshot) | `@old(@old(x))` or `@old` of non-Copy type |
| E7W-0106 | `result` used outside postcondition | `result` keyword in precondition |

### 7.8.6 Type Invariant Errors (E7I-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7I-0101 | Type invariant violated at construction | `where` clause false after initialization |
| E7I-0102 | Type invariant violated after mutation | `where` clause false after procedure call |
| E7I-0103 | Type invariant contains non-pure expression | `where` uses effectful operation |
| E7I-0104 | Type invariant type is not `bool` | `where` doesn't evaluate to boolean |
| E7I-0105 | Type invariant references non-field data | `where` accesses external state |

### 7.8.7 Clause Composition Errors (E7X-01xx)

| Code | Description | Example |
|------|-------------|---------|
| E7X-0101 | Effect set union exceeds available effects | Callee requires effects not in caller's `uses` |
| E7X-0102 | Conflicting postconditions from multiple sources | Diamond inheritance with incompatible `will` clauses |
| E7X-0103 | Effect inference ambiguity (no principal effect) | Multiple valid effect sets, none most general |
| E7X-0104 | Circular dependency in contract clauses | `must` depends on `will` from same signature |

### 7.8.8 Warnings

| Code | Description | Example |
|------|-------------|---------|
| W7C-0001 | Unused contract parameter | Contract procedure parameter never referenced |
| W7C-0002 | Contract always satisfied (trivial precondition) | `must true` |
| W7C-0003 | Contract never satisfied (contradictory precondition) | `must false` |
| W7C-0004 | Effect declared but never used | `uses fs.read` but no file operations |
| W7C-0005 | Postcondition redundant with type system | `will result >= 0` for `usize` return |

### 7.8.9 Selected Diagnostic Examples

**E7C-0101: Contract Procedure Contains Body**

```cursive
contract Serializable {
    procedure to_json(self: imm Self): string
        uses alloc.heap
    {
        // ⚠ ERROR E7C-0101: contract procedure cannot have body
        result string::from("{}")
    }
}
// ERROR E7C-0101: body provided inside contract signature (contracts must be abstract)
// note: contracts define behavioral specifications without implementations
// help: remove the procedure body from the contract
// help: provide implementations in types that implement this contract
```

**E7C-0203: Implementation Strengthens Contract Precondition**

```cursive
contract Processor {
    procedure process(self: mut Self, value: i32)
        must value >= 0
}

record StrictProcessor: Processor {
    procedure process(self: mut Self, value: i32)
        must value > 0  // ⚠ ERROR: stricter than contract
    {
        // Implementation
    }
}
// ERROR E7C-0203: implementation strengthens contract precondition
// note: contract requires `value >= 0`
// note: implementation requires `value > 0`
// help: implementation precondition must be weaker or equal to contract
// help: consider removing the stronger constraint from implementation
```

**E7C-0204: Implementation Weakens Contract Postcondition**

```cursive
contract Counter {
    procedure increment(self: mut Self)
        will self.count > @old(self.count)
}

record BrokenCounter: Counter {
    count: i32

    procedure increment(self: mut Self)
        will self.count >= @old(self.count)  // ⚠ ERROR: weaker than contract
    {
        // May not actually increment
    }
}
// ERROR E7C-0204: implementation weakens contract postcondition
// note: contract guarantees `self.count > @old(self.count)`
// note: implementation only guarantees `self.count >= @old(self.count)`
// help: implementation postcondition must be stronger or equal to contract
```

**E7C-0205: Implementation Effect Set Exceeds Contract**

```cursive
contract Logger {
    procedure log(self: mut Self, message: string)
        uses fs.write
}

record NetworkLogger: Logger {
    procedure log(self: mut Self, message: string)
        uses fs.write, network.send  // ⚠ ERROR: extra effect
    {
        FileSystem::write("/var/log/app.log", message)
        Network::send("log.example.com", message)
    }
}
// ERROR E7C-0205: implementation effect set not subset of contract effect set
// note: contract declares `uses fs.write`
// note: implementation declares `uses fs.write, network.send`
// help: remove `network.send` from implementation or add to contract
```

**E7E-0108: Undeclared Effect Used in Body**

```cursive
procedure save_data(path: string, data: string) {
    FileSystem::write(path, data)
}
// ERROR E7E-0108: undeclared effect used in body
// note: procedure body uses effect `FileSystem.write`
// note: procedure has no `uses` clause
// help: add `uses FileSystem.write` to procedure signature
```

**E7E-0110: Effect Not Available in Context**

```cursive
procedure write_log(message: string)
    uses FileSystem.write
{
    // Body uses FileSystem.write
}

procedure pure_computation() {
    write_log("Computing...")
}
// ERROR E7E-0110: effect not available in context
// note: `write_log` requires effect `FileSystem.write`
// note: caller `pure_computation` does not declare this effect
// help: add `uses FileSystem.write` to `pure_computation` signature
```

**E7M-0101: Precondition Violated at Runtime**

```cursive
procedure divide(numerator: i32, denominator: i32): i32
    must denominator != 0
{
    result numerator / denominator
}

procedure example() {
    let result = divide(10, 0)
}
// ERROR E7M-0101: precondition violated at runtime
// note: precondition `denominator != 0` evaluated to false
// note: denominator value: 0
// help: ensure precondition is satisfied before calling procedure
```

**E7W-0101: Postcondition Violated at Runtime**

```cursive
procedure get_positive(): i32
    will result > 0
{
    result -1  // ⚠ ERROR: violates postcondition
}
// ERROR E7W-0101: postcondition violated at runtime
// note: postcondition `result > 0` evaluated to false
// note: result value: -1
// help: ensure all return paths satisfy the postcondition
```

**E7W-0105: Illegal `@old` Capture**

```cursive
procedure update(self: mut Self)
    will self.value > @old(@old(self.value))  // ⚠ ERROR: nested @old
{
    self.value = self.value + 1
}
// ERROR E7W-0105: illegal nested or non-snapshot-able `@old` capture
// note: `@old` cannot be nested inside another `@old`
// help: use single `@old` capture: `@old(self.value)`
```

**E7I-0101: Type Invariant Violated at Construction**

```cursive
record PositiveCounter
    where self.count > 0
{
    count: i32
}

procedure example() {
    let counter = PositiveCounter { count: 0 }
}
// ERROR E7I-0101: type invariant violated at construction
// note: invariant `self.count > 0` evaluated to false
// note: count value: 0
// help: ensure field values satisfy type invariants at construction
```

**E7X-0101: Effect Set Union Exceeds Available Effects**

```cursive
procedure read_file(path: string): string
    uses FileSystem.read
{
    result FileSystem::read(path)
}

procedure write_file(path: string, data: string)
    uses FileSystem.write
{
    FileSystem::write(path, data)
}

procedure copy_file(source: string, dest: string)
    uses FileSystem.read  // ⚠ ERROR: missing FileSystem.write
{
    let data = read_file(source)
    write_file(dest, data)  // Requires FileSystem.write
}
// ERROR E7X-0101: effect set union exceeds available effects
// note: `write_file` requires effect `FileSystem.write`
// note: caller `copy_file` only declares `FileSystem.read`
// help: add `FileSystem.write` to `copy_file` signature
// help: or declare both: `uses FileSystem.read, FileSystem.write`
```

Diagnostics MUST be reported using the standard format defined in Part I §8.

---

## 7.Z Advanced Topics

This section covers advanced patterns, algorithms, and implementation strategies for contracts and effects.

### 7.Z.1 Effect Inference Algorithms

The Cursive compiler employs constraint-based effect inference to determine the minimal effect set for procedures without explicit `uses` clauses.

#### Inference Algorithm Overview

**Algorithm 7.Z.1** (Effect Inference):

```
Input: Procedure body B, environment Γ
Output: Minimal effect set ε or ERROR

1. Initialize ε := ∅
2. For each expression e in B (depth-first traversal):
   a. If e is an effect operation E.op:
      - Add E to ε
   b. If e is a procedure call p(args):
      - Resolve p to procedure signature
      - Extract declared effects ε_p from signature
      - ε := ε ∪ ε_p
   c. If e is a closure that captures variables:
      - Infer effects ε_c from closure body
      - ε := ε ∪ ε_c
3. If ε contains conflicting effects:
   - ERROR E7X-0103 (ambiguity)
4. Return ε
```

#### Example: Inference in Action

```cursive
// No explicit uses clause
procedure process_files(paths: Vec<string>): Vec<string> {
    var results: Vec<string> = Vec::new()

    for path in paths {
        // Inferred: FileSystem.read
        let content = FileSystem::read(path)

        // Inferred: alloc.heap (from Vec::push)
        results::push(content)
    }

    result results
}
// Inferred signature:
// procedure process_files(paths: Vec<string>): Vec<string>
//     uses FileSystem.read, alloc.heap
```

#### Principal Effect Types

An effect set ε is **principal** if:
1. ε satisfies all constraints in the procedure body
2. For any other effect set ε' satisfying the constraints: ε ⊆ ε'

**Theorem 7.Z.1** (Effect Principal Types): Every well-typed procedure body has a unique principal effect set or inference fails with E7X-0103.

CITE: Part V §4.19.5 — Effect Inference Rules.

---

### 7.Z.2 Contract Refinement

Contract refinement enables deriving more specific contracts from general ones while preserving behavioral compatibility.

#### Refinement Rules

**Definition 7.Z.1** (Contract Refinement):

Contract C' **refines** contract C (written C' ⊑ C) if:

1. **Precondition weakening**: For all procedures p,
   ```
   pre_C(p) ⇒ pre_C'(p)
   ```

2. **Postcondition strengthening**: For all procedures p,
   ```
   post_C'(p) ⇒ post_C(p)
   ```

3. **Effect subset**: For all procedures p,
   ```
   effects_C'(p) ⊆ effects_C(p)
   ```

#### Refinement Example

```cursive
// General contract
contract Reader {
    procedure read(self: imm Self): string
        uses alloc.heap
}

// Refined contract (more specific)
contract ValidatedReader {
    procedure read(self: imm Self): string
        uses alloc.heap
        must self::is_valid()  // Stronger precondition
        will result::len() > 0  // Stronger postcondition
}

// ValidatedReader ⊑ Reader (valid refinement)
```

#### Substitutability

**Theorem 7.Z.2** (Liskov Substitution for Contracts): If C' ⊑ C, then any type implementing C' can be used where C is required without violating contracts.

**Proof sketch**:
1. Precondition weakening ensures callers of C satisfy C'
2. Postcondition strengthening ensures C' implementations provide stronger guarantees
3. Effect subset ensures C' doesn't perform additional side effects

CITE: Part II §2.7 — Contract Coherence.

---

### 7.Z.3 Verification Strategies

Cursive supports multiple verification modes for contract checking.

#### Verification Modes

| Mode | Preconditions | Postconditions | Invariants | Performance |
|------|---------------|----------------|------------|-------------|
| **None** | Not checked | Not checked | Not checked | Zero overhead |
| **Debug** | Runtime checks | Runtime checks | Runtime checks | ~20% overhead |
| **Optimized** | Eliminated if proven | Assumed | Checked at boundaries | ~5% overhead |
| **Formal** | SMT verification | SMT verification | SMT verification | Compile-time only |

#### Static Verification

The compiler can eliminate runtime checks for statically provable contracts:

```cursive
procedure safe_divide(numerator: i32, denominator: i32): i32
    must denominator != 0
{
    result numerator / denominator
}

procedure example() {
    let x = safe_divide(10, 5)  // ✅ Statically proven: 5 != 0

    let y: i32 = get_user_input()
    let z = safe_divide(10, y)  // ⚠ Runtime check: y != 0 unknown
}
```

#### Verification Condition Generation

**Algorithm 7.Z.2** (VC Generation):

For each procedure with contracts:

1. **Extract preconditions** as assumptions
2. **Symbolically execute** procedure body
3. **Generate VCs** for postconditions at each exit point
4. **Submit VCs** to SMT solver
5. If **proven**: eliminate runtime checks
6. If **unknown**: insert runtime checks
7. If **false**: ERROR at compile time

#### Example: Verification Conditions

```cursive
procedure increment(self: mut Counter)
    must self.count < i32::MAX
    will self.count == @old(self.count) + 1
{
    self.count = self.count + 1
}

// Generated VCs:
// VC1: Precondition safety
//   assume: count_0 < i32::MAX
//   prove: count_0 + 1 does not overflow
//
// VC2: Postcondition
//   assume: count_0 < i32::MAX
//   assume: count_1 = count_0 + 1
//   prove: count_1 == count_0 + 1  // ✅ Trivially true
```

CITE: Part VI §6.13 — Verification Modes.

---

### 7.Z.4 Effect System Metatheory

This section presents key theoretical properties of the unified effect system.

#### Soundness

**Theorem 7.Z.3** (Effect Soundness): If a procedure is well-typed with effect set ε, then execution performs only effects in ε.

**Proof sketch**:
1. Effect operations are the only source of effects (by construction)
2. Type system tracks all effect operations (rule [Effect-Op-Call])
3. Effect set union preserves this property (rule [Effect-Poly-Call])
4. Therefore, actual effects ⊆ declared effects □

#### Completeness

**Theorem 7.Z.4** (Effect Completeness): For any procedure, there exists a minimal effect set that type-checks.

**Proof**: By construction via Algorithm 7.Z.1 (Effect Inference) □

#### Effect Polymorphism Coherence

**Theorem 7.Z.5** (Effect Coherence): Effect-polymorphic instantiation preserves type safety.

**Lemma 7.Z.5.1**: If `f<ε>` is well-typed for effect ε₁, and ε₁ ⊆ ε₂, then `f<ε₂>` is also well-typed.

**Proof**: Effect subtyping is monotonic with respect to capability granting □

CITE: Part II §2.10 — Effect Type Theory.

---

### 7.Z.5 Performance Considerations

This section provides guidance on the performance implications of contracts and effects.

#### Contract Overhead

**Precondition checks**:
- **Best case**: Eliminated by static verification (0% overhead)
- **Typical case**: Single branch + comparison (~2-5 cycles)
- **Worst case**: Complex predicate evaluation (~50-100 cycles)

**Postcondition checks**:
- **Best case**: Eliminated or assumed (~0-1% overhead)
- **Typical case**: Checked at return sites (~5-10 cycles per return)
- **Worst case**: Complex assertion with @old captures (~100-200 cycles)

#### Effect System Overhead

**Static dispatch** (default):
- Effect operations inline to direct calls: **0% overhead**
- Effect checking: **Compile-time only**

**Dynamic dispatch** (with `with` blocks):
- Virtual dispatch per operation: **~2-5 ns per call**
- Handler setup cost: **~10-20 ns per `with` block**

#### Optimization Strategies

**Strategy 1: Precondition Hoisting**

```cursive
// Before optimization
procedure process_batch(items: Vec<i32>) {
    for item in items {
        process_item(item)  // Precondition check in loop
            must item > 0
    }
}

// After optimization (hoisted invariant)
procedure process_batch(items: Vec<i32>)
    must forall i in items: i > 0  // Check once
{
    for item in items {
        process_item(item)  // No check needed
            must item > 0
    }
}
```

**Strategy 2: Effect Fusion**

```cursive
// Before fusion: Multiple effect operations
procedure copy_and_transform(src: string, dest: string) {
    let data = FileSystem::read(src)     // Effect: FileSystem.read
    let transformed = transform(data)
    FileSystem::write(dest, transformed)  // Effect: FileSystem.write
}

// Compiler fuses to single I/O operation where possible
// Runtime: Two I/O operations (unavoidable)
// Effect tracking: Zero overhead (compile-time)
```

**Strategy 3: Witness Elimination**

When witnesses are used only for verification, the compiler can erase them:

```cursive
procedure verify_sorted<T>(arr: [T], proof: witness<Sorted<T>>)
    where T: Ord
{
    // proof is erased at runtime (zero overhead)
    // Verification happens at compile time
}
```

#### Benchmarks

Typical overhead measurements (Chapter 9 functions as baseline):

| Feature | Overhead | Notes |
|---------|----------|-------|
| Contract checking (debug) | 15-25% | All checks enabled |
| Contract checking (optimized) | 0-5% | Most checks eliminated |
| Effect tracking | 0% | Compile-time only |
| Effect handlers (`with`) | 2-8% | Virtual dispatch cost |
| Type invariants | 3-10% | Checked at boundaries |
| Witness parameters | 0% | Erased at runtime |

CITE: Part I §1.5 — Performance Model.

---

### 7.Z.6 Design Patterns

This section catalogs proven contract and effect patterns.

#### Pattern 1: Builder with Contracts

```cursive
contract Builder<T> {
    procedure build(self: own Self): T
        uses alloc.heap
        must self::is_valid()
        will result::satisfies_invariants()
}

modal ConfigBuilder {
    @Empty
    @Partial { fields: Map<string, string> }
    @Complete { config: Config }
}

record ConfigBuilder: Builder<Config> {
    procedure set_field(self: mut ConfigBuilder@Partial, key: string, value: string): ConfigBuilder@Partial
        uses alloc.heap
    {
        self.fields::insert(key, value)
        result self
    }

    procedure build(self: own ConfigBuilder@Partial): Config
        uses alloc.heap
        must self::is_valid()
        will result::satisfies_invariants()
    {
        // Construct Config from fields
        result Config::from_fields(self.fields)
    }

    procedure is_valid(self: imm ConfigBuilder@Partial): bool {
        result self.fields::contains_key("required_field")
    }
}
```

#### Pattern 2: Effect Layering

```cursive
// Layer 1: Low-level effects
effect RawIO {
    read_bytes(fd: i32, count: usize): [u8]
    write_bytes(fd: i32, data: [u8]): usize
}

// Layer 2: Buffered effects
effect BufferedIO {
    read_line(fd: i32): string
    write_line(fd: i32, line: string): ()
}

// Layer 3: Application effects
effect ApplicationIO {
    read_config(path: string): Config
    write_log(level: LogLevel, message: string): ()
}

// Implementation uses layering
procedure read_config(path: string): Config
    uses RawIO, alloc.heap
{
    with BufferedIO {
        read_line(fd) => {
            // Implement using RawIO
            let bytes = RawIO::read_bytes(fd, 1024)
            continue(string::from_utf8(bytes))
        }
    } {
        with ApplicationIO {
            read_config(path) => {
                // Implement using BufferedIO
                let fd = open_file(path)
                let lines = read_all_lines(fd)
                continue(Config::parse(lines))
            }
        } {
            result ApplicationIO::read_config(path)
        }
    }
}
```

#### Pattern 3: Witness-Based Verification

```cursive
contract Sorted<T> {
    procedure invariant(arr: imm [T]): bool
        where T: Ord
        will result == forall i, j: i < j ⇒ arr[i] <= arr[j]
}

procedure binary_search<T>(
    arr: imm [T],
    target: T,
    proof: witness<Sorted<T>>
): Option<usize>
    where T: Ord, T: Eq
{
    // Compiler uses witness to verify binary search is safe
    // Runtime: witness erased, zero overhead

    var low: usize = 0
    var high: usize = arr::len()

    loop {
        if low >= high {
            result Option::None
        }

        let mid = (low + high) / 2
        let mid_val = arr[mid]

        if mid_val == target {
            result Option::Some(mid)
        } else if mid_val < target {
            low = mid + 1
        } else {
            high = mid
        }
    }
}
```

CITE: Part II §2.12 — Design Pattern Catalog.

---

* Behavioral contracts integrate with trait implementations (Part II §2.7) and generic bounds (Part II §2.9).
* Effect polymorphism uses the map-type notation and inference rules in Part II §2.9.4 and Part V §4.19.4–§4.19.5 (see §7.0.2 regarding numbering).
* Statement-level contract constructs (block contracts, loop invariants, `[[no_panic]]`, effect narrowing) remain in Part VI; this chapter references them where needed without duplicating the rules.
* Diagnostics listed above correspond to the entries recorded in Part I §8.

---

*End of Part VII — Contracts and Effects.*
