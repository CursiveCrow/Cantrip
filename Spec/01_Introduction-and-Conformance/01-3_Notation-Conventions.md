# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.3: Notation and Conventions

**File**: `01-3_Notation-Conventions.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-2_Terms-Definitions.md) | **Next**: [[REF_TBD]](01-4_Conformance.md)

---

This section establishes the mathematical notation, metavariables, and judgment forms used throughout the Cursive Language Specification to formally describe syntax, typing rules, and operational semantics.

## 1.3.1 Grammar Notation (EBNF)

Grammar productions throughout this specification are expressed in **Extended Backus-Naur Form (EBNF)**.

### EBNF Symbols

| Symbol | Meaning | Example |
|--------|---------|---------|
| `::=` | is defined as | `Expr ::= Literal \| Ident` |
| `\|` | alternatives (or) | `Type ::= i32 \| f64 \| bool` |
| `*` | zero or more | `Param*` (zero or more parameters) |
| `+` | one or more | `Stmt+` (one or more statements) |
| `?` | optional (zero or one) | `ElseClause?` (optional else) |
| `~` | negation (not) | `~['\n']` (not newline) |
| `[ ]` | optional elements or character class | `[a-zA-Z_]` |
| `( )` | grouping | `(Expr (',' Expr)*)` |

### Character and String Literals in Grammar

- **Single quotes** denote literal characters: `'{'`, `';'`, `'\n'`
- **Double quotes** denote literal strings: `"function"`, `"owned"`
- **Backslash escapes** use standard C conventions: `'\n'`, `'\t'`, `'\\'`

### Grammar Production Format

Productions are written in the form:

```ebnf
NonTerminal ::= production₁ | production₂ | ... | productionₙ
```

**Example:**

```ebnf
Literal ::= IntLit | FloatLit | StringLit | BoolLit | CharLit
```

### Reference to Complete Grammar

The complete normative grammar is consolidated in **Annex A (Complete Grammar)**. Individual sections of this specification present relevant grammar excerpts for clarity, but Annex A is authoritative in case of discrepancies.

**Cross-reference**: Annex A.1 (Lexical Grammar), A.2 (Type Grammar), A.3 (Expression Grammar), etc.

## 1.3.2 Metavariables

The following metavariable conventions are used consistently throughout the specification.

### Program Entity Metavariables

```
x, y, z ∈ Var          (variables, identifiers)
f, g, h ∈ FunName      (function names)
m, n, o ∈ ProcName     (procedure names)
R, S, T ∈ RecordName   (record type names)
E, F, G ∈ EnumName     (enum type names)
C, C' ∈ ContractName   (contract names)
```

### Type and Expression Metavariables

```
T, U, V ∈ Type         (types)
τ, τ₁, τ₂ ∈ Type       (types, alternative notation)
e, e₁, e₂ ∈ Expr       (expressions)
v, v₁, v₂ ∈ Value      (values, runtime results)
p, p₁, p₂ ∈ Pattern    (patterns)
s, s₁, s₂ ∈ Stmt       (statements)
```

**Note**: Uppercase Roman letters (T, U, V) and lowercase Greek tau (τ) are used interchangeably for types, following different formal traditions.

### Modal and Grant Metavariables

```
@S, @S', @S₁ ∈ State   (modal states)
G, G₁, G₂ ∈ Grant      (grant sets)
Θ ∈ TransitionSet      (state transition relation)
```

**Examples**:
- `FileHandle@Open` — modal type `FileHandle` in state `Open`
- `G = {fs::read, alloc::heap}` — grant set containing file system read and heap allocation

### Contract and Assertion Metavariables

```
P, Q, R ∈ Assertion    (contract assertions, predicates)
{P} e {Q}              (Hoare triple notation)
```

**Hoare triple interpretation**: If precondition `P` holds before executing `e`, then postcondition `Q` holds after `e` (if it terminates normally).

### Context Metavariables

```
Γ, Γ', Γ₁ ∈ Context        (type contexts: Var -> Type)
Σ, Σ', Σ₁ ∈ StateContext   (modal state contexts)
Δ, Δ', Δ₁ ∈ RegionCtx      (region contexts: stack of active regions)
G_ctx ∈ GrantContext        (grant contexts: available grants)
σ, σ', σ₁ ∈ Store           (memory stores: Location -> Value)
```

**Purpose**: Contexts track information during type checking, compilation, and execution.

### Context Operations

The following notational conventions apply to contexts:

- **Extension**: `Γ, x: T` denotes context Γ extended with binding `x: T`
- **Lookup**: `Γ(x)` retrieves the type of `x` in context Γ
- **Update**: `Γ[x ↦ T]` denotes Γ with `x` mapped to `T`
- **Store update**: `σ[ℓ ↦ v]` denotes store σ updated so location `ℓ` maps to value `v`
- **Union**: `Γ₁ ∪ Γ₂` denotes the union of two contexts (disjoint domains assumed)

## 1.3.3 Mathematical Notation

### Set Theory Operators

```
∈       (element of)               x ∈ S           "x is an element of set S"
∉       (not element of)           x ∉ S           "x is not in S"
<:       (subset)                   A <: B           "A is a subset of B"
⊇       (superset)                 A ⊇ B           "A is a superset of B"
∪       (union)                    A ∪ B           "union of sets A and B"
∩       (intersection)             A ∩ B           "intersection of sets A and B"
∅       (empty set)                G = ∅           "grant set G is empty (pure)"
×       (Cartesian product)        A × B           "product of sets A and B"
```

**Examples**:
- `G <: G_ctx` — the grant set `G` is allowed by the grant context `G_ctx`
- `G₁ ∪ G₂` — union of two grant sets
- `Type = PrimType ∪ CompType` — the set of types is the union of primitive and composite types

### Logical Operators

```
∧       (conjunction, "and")              P ∧ Q           "P and Q both hold"
\/       (disjunction, "or")               P \/ Q           "P or Q (or both) holds"
¬       (negation, "not")                 ¬P              "P does not hold"
⇒       (implication, "implies")          P ⇒ Q           "if P holds, then Q holds"
⟺      (bi-implication, "iff")           P ⟺ Q           "P holds if and only if Q holds"
∀       (universal quantification)        ∀x. P           "for all x, P holds"
∃       (existential quantification)      ∃x. P           "there exists x such that P holds"
```

**Examples**:
- `∀x ∈ Vars. Γ(x) is well-formed` — all variables in context Γ have well-formed types
- `P ∧ Q ⇒ R` — if both P and Q hold, then R holds

### Relations and Judgments

```
->       (maps to, reduces to)         x -> y           "x maps to y" or "x reduces to y"
⇒       (pipeline, entails)           e₁ ⇒ e₂         "e₁ pipelines to e₂"
⇓       (evaluates to)                e ⇓ v           "expression e evaluates to value v"
->*      (transitive closure)          e ->* v          "e reduces to v in zero or more steps"
≡       (equivalence)                 e₁ ≡ e₂         "e₁ and e₂ are equivalent"
<:      (subtype relation)            T <: U          "T is a subtype of U"
⊢       (entails, proves, types)      Γ ⊢ e : T       "context Γ proves e has type T"
⊨       (satisfies, models)           σ ⊨ P           "store σ satisfies assertion P"
```

**Examples**:
- `unique T <: const T` and `shared T <: const T` — permission coercion (note: `unique` and `shared` are not subtypes of each other)
- `Γ ⊢ e : T` — in context Γ, expression e has type T

### Substitution Notation

```
[x ↦ v]                            (substitution: replace x with v)
e[x ↦ v]                           (expression e with x replaced by v)
T[α ↦ U]                           (type T with type variable α replaced by U)
```

**Example**: `(λx. x + 1)[x ↦ 5] = 5 + 1 = 6`

## 1.3.4 Judgment Forms

Judgments are formal statements about programs, used in typing rules and operational semantics. Each judgment form has a specific interpretation.

### Typing Judgments

```
Γ ⊢ e : T                         (expression e has type T in context Γ)
Γ ⊢ e : T@S                       (expression e has type T in modal state S)
Γ ⊢ e : T@S ! G                   (expression e has type T, state S, and grant set G)
Γ ⊢ e : T ! G                     (expression e has type T and grant set G)
```

**Interpretation**:
- The judgment `Γ ⊢ e : T` asserts that in typing context Γ, expression e is well-typed with type T.
- The symbol `⊢` is read as "entails" or "proves" or "types as".

**Example**:
```
Γ = {x: i32, y: i32}
Γ ⊢ x + y : i32
```

### Grant Judgments

```
Γ ⊢ e ! G                         (expression e has grant set G)
G₁ <: G₂                           (grant set G₁ is contained in G₂)
```

**Example**: `Γ ⊢ read_file(path) ! {fs::read}`

### Contract Judgments

```
{P} e {Q}                         (Hoare triple: precondition P, postcondition Q)
σ ⊨ P                             (store σ satisfies assertion P)
```

**Interpretation**: If precondition P holds in the initial state and expression e is evaluated, then postcondition Q holds in the final state (assuming termination).

**Example**:
```
{x > 0} let y = x + 1 {y > 1}
```

### Operational Semantics Judgments

```
⟨e, σ⟩ -> ⟨e', σ'⟩                 (small-step reduction: e in store σ reduces to e' in σ')
⟨e, σ⟩ ⇓ ⟨v, σ'⟩                   (big-step evaluation: e in σ evaluates to value v in σ')
⟨e, σ⟩ ↯                          (expression e diverges or errors)
```

**Interpretation**:
- `⟨e, σ⟩ -> ⟨e', σ'⟩` — one step of computation transforms e to e', updating store σ to σ'
- `⟨e, σ⟩ ⇓ ⟨v, σ'⟩` — evaluating e in store σ yields final value v and store σ'

### Modal State Judgments

```
@S₁ ->ₘ @S₂                        (state transition from S₁ to S₂ via transition m)
Γ; Σ ⊢ e : T@S                    (e has type T in state S, with state context Σ)
```

**Example**: `@Open ->close @Closed` — calling `close` transitions from `Open` to `Closed`

## 1.3.5 Inference Rule Format

Inference rules specify how judgments are derived from other judgments. The standard format is:

```
[Rule-Name]
premise₁    premise₂    ...    premiseₙ
----------------------------------------
conclusion
```

- **Premises** appear above the horizontal line
- **Conclusion** appears below the line
- **Interpretation**: If all premises hold, then the conclusion holds
- **Rule name** appears in brackets on the left

### Example: Function Application Type Rule

```
[T-App]
Γ ⊢ f : T₁ -> T₂
Γ ⊢ e : T₁
-----------------
Γ ⊢ f(e) : T₂
```

**Reading**: "If function f has type T₁ -> T₂, and expression e has type T₁, then the application f(e) has type T₂."

### Example: Grant Aggregation Rule

```
[Grant-Aggregate-Call]
Γ ⊢ f : (τ₁, ..., τₙ) -> U ! G_f
Γ ⊢ aᵢ : τᵢ ! G_aᵢ  (for all i)
G_total = G_f ∪ G_a₁ ∪ ... ∪ G_aₙ
--------------------------------------------------------
Γ ⊢ f(a₁, ..., aₙ) : U ! G_total
```

**Reading**: "To type a function call, compute the union of the function's grant set and all argument grant sets."

### Axioms (Rules with No Premises)

Rules with no premises are called **axioms**:

```
[T-Int]
-----------------
Γ ⊢ n : i32       (where n is an integer literal)
```

## 1.3.6 Typography and Formatting Conventions

### Code Formatting

**Cursive code examples** use fenced code blocks with the `cursive` language marker:

```cursive
function factorial(n: i32): i32 {
    if n <= 1 {
        result 1
    } else {
        result n * factorial(n - 1)
    }
}
```

**Grammar productions** use the `ebnf` language marker:

```ebnf
FunctionDecl ::= "function" Ident GenericParams? "(" ParamList? ")" ":" Type ContractClauses? Block
```

### Emphasis and Keywords

- **Bold** indicates normative keywords (`shall`, `must`, `required`) or emphasis
- *Italics* indicate informal emphasis or when introducing a term
- `Monospace` indicates code, identifiers, types, keywords, or technical terms

### Correctness Markers in Examples

Examples may use visual markers to indicate correct or incorrect code:

- ✅ Indicates valid, well-formed code
- ❌ Indicates invalid code that violates a rule

**Example**:

```cursive
// ✅ Valid: immutable binding with const permission
let x: const i32 = 42;

// ❌ Invalid: cannot reassign to immutable binding
let y = 10;
y = 20;  // ERROR E3D10
```

### Cross-References

Cross-references to other sections use the format `[REF_TBD]` or `[REF_TBD]`:

- `[REF_TBD]` refers to Section 2.1
- `[REF_TBD]` refers to Section 3.4.2
- `Part VI [REF_TBD]` refers to Section 6.2 in Part VI
- `Annex A.1` refers to Section A.1 in Annex A

### Informative vs. Normative

- Sections marked **(Normative)** contain requirements for conforming implementations
- Sections marked **(Informative)** provide guidance, examples, or rationale
- **Notes** are informative, even in normative sections
- **Examples** are informative, unless explicitly stated otherwise

---

## Reading Guide

**For implementers**: Focus on grammar (EBNF), typing rules (inference rules), and judgment forms.

**For tool developers**: Understand metavariables and notation to parse formal specifications.

**For researchers**: Study the formal semantics notation and meta-theory (Annex C).

---

**Previous**: [[REF_TBD]](01-2_Terms-Definitions.md) | **Next**: [[REF_TBD]](01-4_Conformance.md)
