# Part I: Foundations - §1. Notation and Mathematical Foundations

**Section**: §1 | **Part**: Foundations (Part I)
**Previous**: [Table of Contents](00_TableOfContents.md) | **Next**: [Lexical Structure](02_LexicalStructure.md)

---

**Definition 1.1 (Mathematical Foundations):** This section establishes the mathematical notation, metavariables, and judgment forms used throughout the Cantrip Language Specification to formally describe syntax, typing rules, and operational semantics.

## 1. Notation and Mathematical Foundations

### 1.1 Overview

**Key innovation/purpose:** Provides a formal mathematical foundation for precisely specifying language semantics through standardized notation, judgment forms, and inference rules.

**When to use this section:**
- When reading type rules (T-* rules in §4-§10)
- When reading evaluation semantics (E-* rules in Part X)
- When encountering unfamiliar mathematical notation
- When implementing a type checker or interpreter

**When NOT to use this section:**
- For learning Cantrip syntax (see §2 Lexical Structure)
- For practical programming guidance (see Examples sections)
- For implementation strategies (see Part XIII: Tooling)

**Relationship to other features:**
- **All subsequent sections** use the notation defined here
- **§2 (Lexical Structure)**: Uses set notation (∈, ⊆) for token categories
- **§3 (Abstract Syntax)**: Uses BNF notation for syntax definitions
- **Type System (Part II)**: Uses type judgments (Γ ⊢ e : T) extensively
- **Operational Semantics (Part X)**: Uses evaluation judgments (⟨e, σ⟩ ⇓ ⟨v, σ'⟩)
- **Contract System (Part V)**: Uses Hoare triples ({P} e {Q}) and semantic satisfaction (σ ⊨ P)

**Normative status:** This section is INFORMATIVE. It defines notation but does not impose requirements on implementations. The notation is used to precisely specify requirements in later NORMATIVE sections.

### 1.2 Syntax

#### 1.2.1 Concrete Syntax

**Not applicable:** This section defines mathematical notation and metavariables, not Cantrip language syntax. Concrete syntax for Cantrip appears in §2 (Lexical Structure) and subsequent feature sections.

#### 1.2.2 Abstract Syntax

**Metavariable Categories:**

The following metavariable conventions are used throughout the specification. Metavariables are placeholders that stand for actual program constructs.

**Program Entity Metavariables:**
```
x, y, z ∈ Var          (variables)
f, g, h ∈ FunName      (function names)
m, n, o ∈ ProcName     (procedure names)
R, S, T ∈ RecordName   (record names)
E, F, G ∈ EnumName     (enum names)
```

**Type and Expression Metavariables:**
```
T, U, V ∈ Type         (types)
e, e₁, e₂ ∈ Expr       (expressions)
v, v₁, v₂ ∈ Value      (values)
p, p₁, p₂ ∈ Pattern    (patterns)
```

**Modal and Effect Metavariables:**
```
@S, @S', @S₁ ∈ State   (modal states)
ε, ε₁, ε₂ ∈ Effect     (effects)
Δ ∈ Transitions        (state transition relation)
```

**Contract and Assertion Metavariables:**
```
P, Q, R ∈ Assertion    (contract assertions)
{P} ... {Q}            (Hoare triple notation)
```

**Context Metavariables:**
```
Γ, Γ' ∈ Context        (type contexts: Var → Type)
Σ, Σ' ∈ StateContext   (state contexts: modal state tracking)
Δ ∈ EffContext         (effect contexts: available effects)
σ, σ' ∈ Store          (memory stores: Location → Value)
```

**Context Operations:**
- `Γ, x: T` means "context Γ extended with binding x: T"
- `σ[ℓ ↦ v]` means "store σ updated so location ℓ maps to value v"

**Judgment Form Notation:**

Judgments are formal statements about programs. Each judgment form has a specific meaning:

```
Γ ⊢ e : T                         (expression e has type T in context Γ)
Γ ⊢ e : T@S                       (type T in modal state @S)
Γ ⊢ e : T@S ! ε                   (type T, state @S, effect ε)
Γ ⊢ e ! ε                         (expression e has effect ε)
{P} e {Q}                         (Hoare triple: if P holds before e, Q holds after)
σ ⊨ P                             (store σ satisfies assertion P)
⟨e, σ⟩ → ⟨e', σ'⟩                 (small-step reduction)
⟨e, σ⟩ ⇓ ⟨v, σ'⟩                   (big-step evaluation)
@S₁ →ₘ @S₂                        (state transition via procedure m)
```

**Formal Operators:**

**Set Theory:**
```
∈       (element of)               x ∈ S           "x is an element of set S"
⊆       (subset)                   A ⊆ B           "A is a subset of B"
∪       (union)                    A ∪ B           "union of sets A and B"
∩       (intersection)             A ∩ B           "intersection of sets A and B"
∅       (empty set)                ε = ∅           "effect ε is the empty set (pure)"
×       (cartesian product)        A × B           "product of sets A and B"
```

**Logic:**
```
∧       (conjunction)              P ∧ Q           "P and Q both hold"
∨       (disjunction)              P ∨ Q           "P or Q (or both) holds"
¬       (negation)                 ¬P              "P does not hold"
⇒       (implication)              P ⇒ Q           "if P then Q"
⟺      (bi-implication)           P ⟺ Q           "P if and only if Q"
∀       (universal quantification) ∀x. P           "for all x, P holds"
∃       (existential quantification) ∃x. P         "there exists x such that P holds"
```

**Relations:**
```
→       (maps to / reduces to)     x → y           "x maps to y" or "x reduces to y"
⇒       (implies / pipeline)       P ⇒ Q           "P implies Q"
⇓       (evaluates to)             e ⇓ v           "expression e evaluates to value v"
≡       (equivalence)              e₁ ≡ e₂         "e₁ and e₂ are equivalent"
⊢       (entails / proves)         Γ ⊢ e : T       "context Γ entails e has type T"
⊨       (satisfies / models)       σ ⊨ P           "store σ satisfies assertion P"
```

**Substitution:**
```
[x ↦ v]                            (substitution: replace x with v)
e[x ↦ v]                           (expression e with x replaced by v)
```

**Inference Rule Format:**
```
[Rule-Name]
premise₁    premise₂    ...    premiseₙ
────────────────────────────────────────
conclusion
```

#### 1.2.3 Basic Examples

**Example 1: Variable in typing environment**
```
Context: Γ = {x: i32, y: bool, z: str}
Judgment: Γ ⊢ x : i32

Reading: "In a context where x has type i32, y has type bool, and z has type str,
         the variable x has type i32."
```

**Example 2: Set membership**
```
T ∈ {i8, i16, i32, i64}

Reading: "Type T is one of the signed integer types i8, i16, i32, or i64."
```

**Example 3: Substitution**
```
Expression: (x + y)[x ↦ 5]
Result: 5 + y

Reading: "Substitute 5 for x in the expression x + y, yielding 5 + y."
```

**Example 4: Inference rule application**
```
[T-Add]
Γ ⊢ x : i32
Γ ⊢ 1 : i32
─────────────────────
Γ ⊢ x + 1 : i32

Reading: "If x has type i32 and 1 has type i32 in context Γ,
         then x + 1 has type i32."
```

### 1.3 Static Semantics

#### 1.3.1 Well-Formedness Rules

**Purpose:** These rules define when mathematical notation is well-formed, not when Cantrip programs are well-typed (that comes in later sections).

**Well-formed contexts:**
```
[WF-Context-Empty]
────────────────────
∅ well-formed

[WF-Context-Extend]
Γ well-formed
x ∉ dom(Γ)
Γ ⊢ T : Type
────────────────────
Γ, x: T well-formed
```

**Explanation:** A context is well-formed if it's either empty or built by extending a well-formed context with a fresh variable and a well-formed type.

**Well-formed judgments:**
```
[WF-Typing-Judgment]
Γ well-formed
e ∈ Expr
T ∈ Type
────────────────────
Γ ⊢ e : T is a judgment

[WF-Effect-Judgment]
Γ well-formed
e ∈ Expr
ε ∈ Effect
────────────────────
Γ ⊢ e ! ε is a judgment
```

**Explanation:** A typing judgment is well-formed if the context is well-formed, e is an expression, and T is a type. The judgment may or may not be derivable (provable), but it's syntactically valid.

#### 1.3.2 Notational Properties

**Substitution properties:**
```
Property 1.1 (Substitution Well-Defined):
∀e ∈ Expr, x ∈ Var, v ∈ Value. ∃!e' ∈ Expr. e' = e[x ↦ v]

Reading: "For any expression e, variable x, and value v, there exists a unique
         expression e' that is the result of substituting v for x in e."
```

**Context properties:**
```
Property 1.2 (Context Lookup):
∀Γ, x. (x ∈ dom(Γ)) ⇒ ∃!T. Γ(x) = T

Reading: "If variable x is in the domain of context Γ, then there exists a unique
         type T such that Γ maps x to T."
```

**Set properties:**
```
Property 1.3 (Effect Union Commutativity):
∀ε₁, ε₂ ∈ Effect. ε₁ ∪ ε₂ = ε₂ ∪ ε₁

Property 1.4 (Effect Union Associativity):
∀ε₁, ε₂, ε₃ ∈ Effect. (ε₁ ∪ ε₂) ∪ ε₃ = ε₁ ∪ (ε₂ ∪ ε₃)
```

#### 1.3.3 Naming Conventions

**In Cantrip source code:**
- Type names: `CamelCase` (e.g., `Point`, `Vec`, `HashMap`)
- Functions: `snake_case` (e.g., `read_file`, `compute_sum`, `is_valid`)
- Constants: `SCREAMING_SNAKE` (e.g., `MAX_SIZE`, `DEFAULT_PORT`, `PI`)
- Type variables: Single uppercase letters (e.g., `T`, `U`, `V`, `K`)
- Lifetimes: Single lowercase letter with leading apostrophe (e.g., `'a`, `'b`)

**In formal rules:**
- Metavariables follow conventions in §1.2.2
- Rule names: `[Category-Description]` (e.g., `[T-Add]`, `[E-IfTrue]`, `[WF-Array]`)

**Type syntax shortcuts:**
```
[T]         ≡  slice of T (fat pointer)
[T; n]      ≡  array of T with length n
T?          ≡  Option<T>
T!          ≡  Result<T, Error>
```

### 1.4 Dynamic Semantics

**Not applicable:** Mathematical notation has no runtime semantics. This section defines notation used to specify the dynamic semantics of Cantrip programs in later sections.

**Evaluation judgment forms** (defined here, applied in Part X: Operational Semantics):

**Small-step reduction:**
```
⟨e, σ⟩ → ⟨e', σ'⟩

Reading: "Expression e in store σ reduces in one step to expression e' in store σ'."
```

**Big-step evaluation:**
```
⟨e, σ⟩ ⇓ ⟨v, σ'⟩

Reading: "Expression e in store σ evaluates to value v, producing store σ'."
```

**Example (from §38: Operational Semantics):**
```
Evaluation: ⟨2 + 3, σ⟩ ⇓ ⟨5, σ⟩

Reading: "The expression 2 + 3 evaluates to 5; the store is unchanged."
```

### 1.5 Reading Type Rules: Complete Examples

This subsection demonstrates how the notation defined above appears in actual specification sections. These examples bridge the abstract notation and concrete Cantrip code.

#### 1.5.1 Example 1: Integer Addition (§4 Primitives)

**Cantrip code:**
```cantrip
let x: i32 = 10;
let y: i32 = 20;
let z = x + y;
```

**Type rule:**
```
[T-Add]
Γ ⊢ e₁ : T
Γ ⊢ e₂ : T
T ∈ {i8, i16, i32, i64, u8, u16, u32, u64, f32, f64}
─────────────────────────────────────────────────────
Γ ⊢ e₁ + e₂ : T
```

**Reading this rule:**
- **Γ**: Typing environment (§1.2.2), here `{x: i32, y: i32}`
- **⊢**: Typing judgment (§1.2.2), reads "entails" or "proves"
- **e₁, e₂**: Expression metavariables (§1.2.2), here `x` and `y`
- **T**: Type metavariable (§1.2.2), here `i32`
- **∈**: Set membership (§1.2.2)

**Application to code:**
```
Premises:
  {x: i32, y: i32} ⊢ x : i32     (variable lookup)
  {x: i32, y: i32} ⊢ y : i32     (variable lookup)
  i32 ∈ {i8, i16, ..., f64}      (i32 is a numeric type)

Conclusion:
  {x: i32, y: i32} ⊢ x + y : i32
```

The type checker uses this rule to determine that `x + y` has type `i32`.

#### 1.5.2 Example 2: Function Call (§13 Functions)

**Cantrip code:**
```cantrip
function add(a: i32, b: i32): i32 { a + b }

let result = add(10, 20);
```

**Type rule:**
```
[T-Call]
Γ ⊢ f : fn(T₁, ..., Tₙ) -> U
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
─────────────────────────────────────
Γ ⊢ f(e₁, ..., eₙ) : U
```

**Reading this rule:**
- **f**: Function name metavariable (§1.2.2), here `add`
- **fn(T₁, ..., Tₙ) -> U**: Function type syntax
- **U**: Return type metavariable (§1.2.2), here `i32`

**Application to code:**
```
Premises:
  Γ ⊢ add : fn(i32, i32) -> i32     (function signature)
  Γ ⊢ 10 : i32                       (integer literal)
  Γ ⊢ 20 : i32                       (integer literal)

Conclusion:
  Γ ⊢ add(10, 20) : i32
```

#### 1.5.3 Example 3: Evaluation Semantics (Part X)

**Cantrip code:**
```cantrip
let x = 2 + 3;
```

**Evaluation rule:**
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

**Reading this rule:**
- **⟨e, σ⟩**: Configuration with expression `e` and store `σ` (§1.2.2)
- **⇓**: Big-step evaluation (§1.2.2), reads "evaluates to"
- **n₁, n₂**: Integer value metavariables
- **σ, σ', σ''**: Store metavariables (§1.2.2) tracking state changes

**Application to code:**
```
Evaluation steps:
  ⟨2, σ⟩ ⇓ ⟨2, σ⟩                   (literal evaluates to itself)
  ⟨3, σ⟩ ⇓ ⟨3, σ⟩                   (literal evaluates to itself)
  ───────────────────────────────
  ⟨2 + 3, σ⟩ ⇓ ⟨5, σ⟩               (addition)
```

The evaluator uses this rule to compute that `2 + 3` evaluates to `5`.

#### 1.5.4 Example 4: Modal State Transitions (§10 Modals)

**Cantrip code:**
```cantrip
modal File {
    @Closed -> open() -> @Open;
    @Open -> read() -> @Open;
    @Open -> close() -> @Closed;
}

let file: File@Closed = File.new();
let opened: File@Open = file.open();
```

**State transition rule:**
```
[T-Transition]
Γ ⊢ e : modal P@S₁
S₁ →ₘ S₂ ∈ transitions(P)
Γ ⊢ m : proc(mut self: modal P@S₁) -> modal P@S₂
────────────────────────────────────────────────────
Γ ⊢ e.m() : modal P@S₂
```

**Reading this rule:**
- **modal P@S**: Modal type in state (§1.2.2)
- **→ₘ**: State transition via procedure m (§1.2.2)
- **transitions(P)**: Set of valid transitions for modal P

**Application to code:**
```
Premises:
  Γ ⊢ file : File@Closed                          (file in Closed state)
  Closed →open Open ∈ transitions(File)           (valid transition)
  Γ ⊢ open : proc(mut self: File@Closed) -> File@Open

Conclusion:
  Γ ⊢ file.open() : File@Open
```

#### 1.5.5 Example 5: Contract Verification (§17 Contracts)

**Cantrip code:**
```cantrip
function divide(x: i32, y: i32): i32
    must y != 0
    will result == x / y
{
    x / y
}
```

**Contract verification rule:**
```
[Verify-Ensures]
{P ∧ requires(f)} body(f) {ensures(f)[result ↦ returned_value]}
────────────────────────────────────────────────────────────────
{P} f(...) {ensures(f)}
```

**Reading this rule:**
- **{P} e {Q}**: Hoare triple (§1.2.2)
- **requires(f)**: Precondition of function f (§1.2.2)
- **ensures(f)**: Postcondition of function f (§1.2.2)
- **[result ↦ v]**: Substitution (§1.2.2), replace `result` with `v`

**Application to code:**
```
Premises:
  Precondition: y != 0
  Body: x / y
  Postcondition: result == x / y

Verification:
  {y != 0} x / y {result == x / y}

If the caller will y != 0, then the function will result == x / y.
```

### 1.6 Cross-References to Notation Usage

The notation defined in this section is used throughout the specification:

- **§2 (Lexical Structure)**: Uses set notation (∈, ⊆) for token categories
- **§3 (Abstract Syntax)**: Defines syntax using BNF with `::=` and `|`
- **§4-§10 (Type System)**: Uses type judgments `Γ ⊢ e : T` extensively
- **§10 (Modals)**: Uses state transition judgments `@S₁ →ₘ @S₂`
- **§13-§15 (Functions)**: Uses effect judgments `Γ ⊢ e ! ε`
- **§17 (Contracts)**: Uses Hoare triples `{P} e {Q}` and semantic satisfaction `σ ⊨ P`
- **Part X (Operational Semantics)**: Uses evaluation judgments `⟨e, σ⟩ ⇓ ⟨v, σ'⟩`

---

**Previous**: [Table of Contents](00_TableOfContents.md) | **Next**: [Lexical Structure](02_LexicalStructure.md)
