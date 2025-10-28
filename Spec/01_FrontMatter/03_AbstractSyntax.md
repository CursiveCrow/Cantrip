# Part I: Foundations - §3. Abstract Syntax

**Section**: §3 | **Part**: Foundations (Part I)
**Previous**: [Lexical Structure](02_LexicalStructure.md) | **Next**: [Attributes](04_Attributes.md)

---

**Definition 3.1 (Abstract Syntax):** The abstract syntax defines the structure of Cantrip programs as abstract syntax trees (ASTs), independent of concrete textual representation.

## 3. Abstract Syntax

### 3.1 Overview

**Key innovation/purpose:** Provides a mathematical representation of program structure that separates the essence of code (what it means) from its textual surface form (how it's written), enabling precise formal reasoning.

**When to use this section:**
- When understanding how programs are represented internally
- When reading type rules and operational semantics
- When implementing parsers, type checkers, or interpreters
- When formal reasoning about program transformations

**When NOT to use this section:**
- For writing Cantrip code (see concrete syntax in feature sections)
- For understanding token-level details (see §2 Lexical Structure)
- For type checking rules (see Part II: Type System)
- For practical programming examples (see feature-specific sections)

**Relationship to other features:**
- **§2 (Lexical Structure)**: Tokens are parsed into ASTs
- **Type System (Part II)**: Type rules operate on AST nodes
- **Operational Semantics (Part X)**: Evaluation rules operate on ASTs
- **Contract System (Part V)**: Contracts annotate AST nodes
- **All subsequent sections**: Use abstract syntax as foundation for formal rules

**Why separate abstract from concrete syntax?**
- **Multiple concrete forms**: `x + y` and `(x) + (y)` have different concrete syntax but identical abstract syntax
- **Easier analysis**: Type checking and evaluation operate on ASTs, not strings
- **Formal reasoning**: Mathematical semantics are defined over abstract syntax
- **Implementation**: Compilers work with ASTs internally

**Relationship in compilation:**
```
§2 Concrete Syntax → [Parser] → §3 Abstract Syntax → [Type Checker] → [Evaluator]
     Token Stream              AST                   Typed AST         Values
```

**Notation:** Abstract syntax uses mathematical notation from §1. For example:
- `e ::= x | n | e₁ + e₂` defines expression forms
- Subscripts distinguish multiple instances (e₁, e₂)
- Vertical bar `|` separates alternatives (sum type)

**Reading guide:**
- §3.2 defines syntax forms (what can appear in ASTs)
- §3.3 defines static semantics (well-formedness rules)
- §3.4 defines dynamic semantics (evaluation on ASTs)
- §3.5 provides additional properties
- §3.6 provides examples connecting concrete to abstract syntax

### 3.2 Syntax

#### 3.2.1 Concrete Syntax

**Not directly applicable:** Abstract syntax is defined mathematically, independent of concrete textual form. For concrete syntax (how to write Cantrip code), see:
- **§2 (Lexical Structure)**: Token-level syntax
- **Feature sections (§4+)**: Feature-specific concrete syntax

**Purpose of this section:** Define the STRUCTURE of ASTs, not the TEXT that produces them.

**Note:** While concrete syntax varies (with/without parentheses, whitespace, etc.), abstract syntax captures the canonical structure. For example:
```
Concrete: (x + 1) * 2
Concrete: x + 1 * 2     (with operator precedence)
Abstract: BinOp(Mul, BinOp(Add, Var("x"), Lit(1)), Lit(2))  [SAME]
```

#### 3.2.2 Abstract Syntax

**Definition 3.2 (Type Language):** The abstract syntax of types.

```
T ::= i8 | i16 | i32 | i64 | isize          (signed integers)
    | u8 | u16 | u32 | u64 | usize          (unsigned integers)
    | f32 | f64                              (floats)
    | bool                                   (booleans)
    | char                                   (characters)
    | str                                    (string slice)
    | [T; n]                                 (fixed array)
    | [T]                                    (slice)
    | (T₁, ..., Tₙ)                          (tuple)
    | fn(T₁, ..., Tₙ): U                     (function type)
    | proc(SelfPerm, T₁, ..., Tₙ): U         (procedure type)
    | own T                                  (owned type)
    | mut T                                  (mutable reference)
    | iso T                                  (isolated reference)
    | &'a T                                  (lifetime-annotated reference)
    | T@S                                    (type T in modal state @S)
    | ∀α. T                                  (polymorphic type)
    | !                                      (never type)
    | record Name                            (record type)
    | modal Name                             (modal type)
    | enum Name                              (enum type)
    | trait Name                             (trait type)
```

**Key observations:**
- Types are stratified: primitives (i32, bool), compounds ([T; n], (T₁, T₂)), named types (record Name)
- Modals extend types with state machines (T@S)
- Type parameters support polymorphism (∀α. T)
- References have ownership qualifiers (own, mut, iso)
- Function types distinguish pure functions from procedures

**Definition 3.3 (Expression Language):** The abstract syntax of expressions.

```
e ::= x                                     (variable)
    | v                                     (value)
    | e₁ ⊕ e₂                               (binary operation)
    | e₁ => e₂                              (pipeline operation)
    | if e₁ then e₂ else e₃                (conditional)
    | let x = e₁ in e₂                     (let binding)
    | var x = e₁ in e₂                     (mutable binding)
    | f(e₁, ..., eₙ)                        (function call)
    | λx:T. e                               (lambda abstraction)
    | e₁ SEP e₂                             (sequencing, SEP = newline or semicolon)
    | move e                                (ownership transfer)
    | region r { e }                        (region block)
    | alloc_in⟨r⟩(e)                        (region allocation)
    | match e { pᵢ -> eᵢ }                  (pattern matching)
    | loop e { e₂ }                         (conditional loop)
    | loop p in e { e₂ }                   (iteration loop)
    | loop { e }                            (infinite loop)
    | break                                 (loop exit)
    | continue                              (loop continue)
    | return e                              (early return)
    | contract(P, e, Q)                     (contract annotation)
    | transition(e, S)                      (explicit state transition)
    | comptime { e }                        (compile-time execution)
    | e.f                                   (field access)
    | e[i]                                  (array/slice index)
    | {f₁: e₁, ..., fₙ: eₙ}                (record construction)
    | Variant(e)                            (enum construction)

where SEP ::= NEWLINE | ";"                 (statement separator)
```

**Key observations:**
- Variables (`x`) and values (`v`) are base cases
- Operators (arithmetic, logical, comparison) via `⊕`
- Control flow (if, match, while, loop)
- Function calls and lambda abstractions
- Record construction and field access
- Blocks and sequencing (SEP = newline or semicolon)
- Statement separators are abstract: both newlines and semicolons map to the same sequencing construct

**Definition 3.4 (Values):** The abstract syntax of values (fully evaluated expressions).

```
v ::= n                                     (integer literal)
    | f                                     (float literal)
    | true | false                          (boolean)
    | 'c'                                   (character literal)
    | "s"                                   (string literal)
    | ()                                    (unit/none)
    | (v₁, ..., vₙ)                         (tuple value)
    | [v₁, ..., vₙ]                         (array value)
    | {f₁: v₁, ..., fₙ: vₙ}                (record value)
    | Variant(v)                            (enum variant value)
    | λx:T. e                               (closure)
    | ℓ                                     (location/pointer)
```

**Key observations:**
- Values are a subset of expressions
- Literals, records, tuples, closures are values
- Values are irreducible (cannot evaluate further)
- Memory locations (ℓ) represent heap/stack addresses

**Definition 3.5 (Patterns):** The abstract syntax of patterns for destructuring.

```
p ::= x                                     (variable pattern)
    | _                                     (wildcard)
    | n                                     (integer literal)
    | true | false                          (boolean literal)
    | 'c'                                   (character literal)
    | (p₁, ..., pₙ)                         (tuple pattern)
    | Variant(p)                            (enum pattern)
    | Record { f₁: p₁, ..., fₙ: pₙ }       (record pattern)
    | [p₁, ..., pₙ]                         (array pattern)
    | p if e                                (guard pattern)
    | p as x                                (binding pattern)
```

**Key observations:**
- Patterns appear in `match`, `let`, and function parameters
- Support wildcard `_`, variables, literals, constructors
- Nested patterns enable deep destructuring
- Guards (`if e`) add boolean conditions to patterns

**Definition 3.6 (Assertions):** The abstract syntax of pre/post-conditions and invariants.

```
P, Q ::= none                               (trivial assertion)
       | e₁ = e₂                            (equality)
       | e₁ < e₂ | e₁ ≤ e₂                  (ordering)
       | P ∧ Q | P ∨ Q                      (conjunction/disjunction)
       | ¬P                                 (negation)
       | P ⇒ Q                              (implication)
       | ∀x. P                              (universal quantification)
       | ∃x. P                              (existential quantification)
       | forall(x in c) { P }               (collection quantification)
       | exists(x in c) { P }               (existential over collection)
       | @old(e)                            (value on entry)
       | result                             (return value)

Function Contracts ::=
    uses ε                                 (effect declaration, newline-terminated)
    must P                                 (preconditions, newline-terminated)
    will Q                                 (postconditions, newline-terminated)
```

**Key observations:**
- Contracts are logical assertions using first-order logic
- `@old(e)` refers to value of `e` before function execution
- `result` refers to the return value in postconditions
- Effect annotations (`uses ε`) track side effects

**Definition 3.7 (Effects):** The abstract syntax of effect signatures.

```
ε ::= ∅                                     (pure/no effects)
    | alloc.heap                            (heap allocation)
    | alloc.heap(bytes≤n)                   (bounded heap)
    | alloc.region                          (region allocation)
    | alloc.stack                           (stack allocation)
    | alloc.temp                            (temporary allocation)
    | alloc.*                               (any allocation)
    | fs.read | fs.write | fs.delete       (filesystem)
    | net.read(d) | net.write               (network, d ∈ {inbound, outbound})
    | time.read | time.sleep(duration≤d)    (time operations)
    | random                                (non-determinism)
    | thread.spawn(count≤n) | thread.join   (threading)
    | render.draw | render.compute          (rendering)
    | audio.play | input.read               (I/O)
    | panic                                 (divergence)
    | ε₁ ∪ ε₂                               (effect union)
    | ε₁ ; ε₂                               (effect sequencing)
    | ¬ε                                    (forbidden effect)
    | effects(f)                            (effects of function f)
    | name                                  (named effect reference)
    | _?                                    (effect hole)
```

**Key observations:**
- Effects track side effects and capabilities
- Pure functions have effect `∅`
- Effects form an algebraic structure (union, sequencing)
- Bounded effects (e.g., `alloc.heap(bytes≤n)`) enable resource analysis

#### 3.2.3 Basic Examples

**Example 1: Simple arithmetic expression**

**Concrete:**
```cantrip
(x + 1) * 2
```

**Abstract:**
```
BinOp(Mul, BinOp(Add, Var("x"), Lit(1)), Lit(2))
```

**Tree representation:**
```
      Mul
     /   \
   Add    Lit(2)
  /   \
Var("x") Lit(1)
```

**Observation:** Parentheses do not appear in abstract syntax—they only control parsing.

**Example 2: Function call**

**Concrete:**
```cantrip
add(10, 20)
```

**Abstract:**
```
Call(Var("add"), [Lit(10), Lit(20)])
```

**Example 3: Let binding**

**Concrete:**
```cantrip
let x = 5 in x + 1
```

**Abstract:**
```
Let("x", Lit(5), BinOp(Add, Var("x"), Lit(1)))
```

**Example 4: Pattern match**

**Concrete:**
```cantrip
match result {
    Result.Ok(value) -> process(value),
    Result.Err(e) -> handle_error(e),
}
```

**Abstract:**
```
Match(
    Var("result"),
    [
        Arm(EnumPattern("Result", "Ok", [VarPattern("value")]),
            None,
            Call(Var("process"), [Var("value")])),
        Arm(EnumPattern("Result", "Err", [VarPattern("e")]),
            None,
            Call(Var("handle_error"), [Var("e")]))
    ]
)
```

### 3.3 Static Semantics

#### 3.3.1 Well-Formed Types

**Purpose:** Not all syntactically valid type expressions are well-formed. These rules define type well-formedness (not type-correctness—that's in later sections).

**Primitive types:**
```
[WF-Int]
T ∈ {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize}
────────────────────────────────────────────────────────
Γ ⊢ T : Type

[WF-Float]
T ∈ {f32, f64}
────────────────
Γ ⊢ T : Type

[WF-Bool]
────────────────
Γ ⊢ bool : Type

[WF-Char]
────────────────
Γ ⊢ char : Type
```

**Compound types:**
```
[WF-Array]
Γ ⊢ T : Type    n > 0
────────────────────────
Γ ⊢ [T; n] : Type

[WF-Slice]
Γ ⊢ T : Type
────────────────
Γ ⊢ [T] : Type

[WF-Tuple]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
Γ ⊢ (T₁, ..., Tₙ) : Type
```

**Function types:**
```
[WF-Function]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
────────────────────────────────────────────────────────
Γ ⊢ fn(T₁, ..., Tₙ): U : Type

[WF-Procedure]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
────────────────────────────────────────────────────────
Γ ⊢ proc(SelfPerm, T₁, ..., Tₙ): U : Type
```

**Modal types:**
```
[WF-Modal]
modal P { ... } well-formed (see §10)
───────────────────────────────────────
Γ ⊢ modal P : Type

[WF-Modal-At-State]
Γ ⊢ T : Type    S ∈ states(T)
────────────────────────────────
Γ ⊢ T@S : Type
```

**Polymorphic types:**
```
[WF-Forall]
Γ, α : Type ⊢ T : Type
───────────────────────
Γ ⊢ ∀α. T : Type
```

#### 3.3.2 Well-Formed AST Nodes

**Function declaration:**
```
[WF-Function-Decl]
function f(x₁: T₁, ..., xₙ: Tₙ): U uses ε { e }
parameters x₁...xₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
effect signature ε well-formed
body e is an expression
─────────────────────────────────────────────────────────
function declaration well-formed
```

**Record declaration:**
```
[WF-Record-Decl]
record R { f₁: T₁, ..., fₙ: Tₙ }
field names f₁...fₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
record declaration well-formed
```

**Enum declaration:**
```
[WF-Enum-Decl]
enum E { V₁(T₁), ..., Vₙ(Tₙ) }
variant names V₁...Vₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
enum declaration well-formed
```

**Modal declaration:**
```
[WF-Modal-Decl]
modal P { states: {s₁, ..., sₙ}, transitions: Δ }
state names s₁...sₙ are distinct
state graph (states, Δ) is connected
────────────────────────────────────────
modal declaration well-formed
```

#### 3.3.3 Pattern Exhaustiveness

**Purpose:** Match expressions must handle all possible values (exhaustiveness).

**Definition:** A set of patterns P = {p₁, ..., pₙ} is exhaustive for type T if every value v : T matches at least one pattern pᵢ.

**Formal rule:**
```
[WF-Match-Exhaustive]
Γ ⊢ e : T
patterns {p₁, ..., pₙ} are exhaustive for T
─────────────────────────────────────────────
match e { p₁ -> e₁, ..., pₙ -> eₙ } well-formed
```

**Examples:**
```cantrip
// Exhaustive
match b: bool {
    true -> 1,
    false -> 0,
}

// Non-exhaustive (ERROR)
match b: bool {
    true -> 1,
}

// Exhaustive with wildcard
match x: i32 {
    0 -> "zero",
    _ -> "non-zero",
}
```

#### 3.3.4 Scoping Rules

**Variable Binding:**
- `let x = e₁ in e₂` introduces variable `x` in scope of `e₂`
- Function parameters introduce variables in function body scope
- Pattern matching introduces variables in match arm scope

**Type Parameter Binding:**
- `<T>` on function/record/enum introduces type parameter `T`
- Scope extends to function body / record fields / enum variants

**Shadowing:**
- Inner bindings shadow outer bindings with the same name
- Shadowing is lexically scoped (block-local)

**Example:**
```cantrip
let x = 10           // x₁ : i32 = 10
let x = x + 5        // x₂ : i32 = 15 (shadows x₁)
{
    let x = "hello"  // x₃ : str = "hello" (shadows x₂ in this block)
    print(x)         // Prints "hello" (refers to x₃)
}
print(x)             // Prints 15 (refers to x₂, x₃ out of scope)
```

### 3.4 Dynamic Semantics

**Purpose:** Abstract syntax nodes are evaluated at runtime according to operational semantics defined in Part X. This section explains how ASTs relate to evaluation.

**Evaluation operates on ASTs:** All evaluation rules in Part X operate on abstract syntax, not concrete text. For example:

**Evaluation judgment:**
```
⟨e, σ⟩ ⇓ ⟨v, σ'⟩

Where:
- e ∈ Expr (abstract expression)
- σ ∈ Store (memory state)
- v ∈ Value (result value)
- σ' ∈ Store (updated memory)
```

**Example evaluation rule (from §38):**
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨BinOp(Add, e₁, e₂), σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

**Key observation:** The rule operates on the AST node `BinOp(Add, e₁, e₂)`, not the text "e₁ + e₂".

**Evaluation order:** Specified in §41 (Evaluation Order), operates on AST structure.

**No runtime representation:** ASTs are compiled to machine code. The AST structure determines:
- Memory layout (§40)
- Evaluation order (§41)
- Effect execution (Part VI)
- Contract checking (Part V)

### 3.5 Additional Properties

#### 3.5.1 AST Structure Properties

**Property 3.1 (Unique Parsing):**
```
∀ source ∈ ValidSourceText. ∃! ast ∈ AST. parse(source) = ast

Reading: "Every valid source text parses to a unique AST."
```

**Property 3.2 (Pretty-Printing Inverse):**
```
∀ ast ∈ AST. parse(pretty_print(ast)) ≡ ast

Reading: "Parsing the pretty-printed form of an AST yields an equivalent AST."
```

**Property 3.3 (AST Traversal):**
```
∀ ast ∈ AST. size(ast) = 1 + Σ(size(child) for child in children(ast))

Reading: "The size of an AST is one plus the sum of sizes of its children."
```

#### 3.5.2 Disambiguation via AST

**Concrete Syntax:**
```cantrip
x + y * z
```

**Ambiguous interpretation 1:** `(x + y) * z`
```
AST: Mul(Add(x, y), z)
```

**Ambiguous interpretation 2:** `x + (y * z)`
```
AST: Add(x, Mul(y, z))
```

The parser resolves this using operator precedence (multiplication before addition), producing interpretation 2. Once in AST form, there is no ambiguity—the structure is explicit.

**All subsequent stages (type checking, optimization, evaluation) operate on the unambiguous AST.**

### 3.6 Examples and Patterns

#### 3.6.1 Function Definition: Concrete to Abstract

**Concrete Syntax:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y
}
```

**Abstract Syntax:**
```
FunctionDef {
    name: "add",
    params: [(x, i32), (y, i32)],
    return_type: i32,
    effects: ∅,
    contracts: none,
    body: BinOp(Add, Var("x"), Var("y"))
}
```

**Tree representation:**
```
FunctionDef
├── name: "add"
├── params: [(x, i32), (y, i32)]
├── return_type: i32
├── effects: ∅
└── body:
    BinOp(Add)
    ├── Var("x")
    └── Var("y")
```

#### 3.6.2 Pattern Matching: Concrete to Abstract

**Concrete Syntax:**
```cantrip
match result {
    Result.Ok(value) -> process(value),
    Result.Err(e) -> handle_error(e),
}
```

**Abstract Syntax:**
```
Match {
    scrutinee: Var("result"),
    arms: [
        Arm {
            pattern: EnumPattern("Result", "Ok", [VarPattern("value")]),
            guard: None,
            body: Call(Var("process"), [Var("value")])
        },
        Arm {
            pattern: EnumPattern("Result", "Err", [VarPattern("e")]),
            guard: None,
            body: Call(Var("handle_error"), [Var("e")])
        }
    ]
}
```

**Tree representation:**
```
Match
├── scrutinee: Var("result")
└── arms:
    ├── Arm
    │   ├── pattern: EnumPattern("Result", "Ok", [VarPattern("value")])
    │   └── body: Call(Var("process"), [Var("value")])
    └── Arm
        ├── pattern: EnumPattern("Result", "Err", [VarPattern("e")])
        └── body: Call(Var("handle_error"), [Var("e")])
```

#### 3.6.3 Record Construction: Concrete to Abstract

**Concrete Syntax:**
```cantrip
record Point { x: f32, y: f32 }

let p = Point { x: 1.0, y: 2.0 }
```

**Abstract Syntax:**
```
RecordDecl {
    name: "Point",
    fields: [(x, f32), (y, f32)]
}

LetBinding {
    var: "p",
    type: record Point,
    value: RecordConstruct {
        name: "Point",
        fields: [(x, Lit(1.0)), (y, Lit(2.0))]
    }
}
```

#### 3.6.4 Relationship to Formal Semantics

**Type Rules (Part II):** Type checking operates on abstract syntax trees, assigning types to expressions.

**Example:** The type rule for addition operates on abstract syntax:
```
[T-Add]
Γ ⊢ e₁ : T
Γ ⊢ e₂ : T
T ∈ {i8, i16, i32, i64, u8, u16, u32, u64, f32, f64}
─────────────────────────────────────────────────────
Γ ⊢ BinOp(Add, e₁, e₂) : T
```

Here `BinOp(Add, e₁, e₂)` refers to the abstract syntax node, not the string "e₁ + e₂". The parser has already transformed the concrete syntax `e₁ + e₂` into the AST node.

**Statement Separators in Type Rules:**

Both newlines and semicolons are treated identically in abstract syntax—they both represent sequencing. The type rule for sequencing applies uniformly:

```
[T-Seq]
Γ ⊢ e₁ : T₁
Γ ⊢ e₂ : T₂
────────────────────────
Γ ⊢ e₁ SEP e₂ : T₂

where SEP = NEWLINE or ";"
```

This means `let x = 1\nlet y = 2` and `let x = 1; let y = 2` have identical abstract syntax and type checking behavior.

**Evaluation Rules (Part X):** Operational semantics evaluates abstract syntax:
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨BinOp(Add, e₁, e₂), σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

Again, `BinOp(Add, e₁, e₂)` is the AST node. The evaluation judgment `⟨e, σ⟩ ⇓ ⟨v, σ'⟩` operates on ASTs, not concrete text.

**Why this matters:**
- **Uniformity**: All formal rules reference the same abstract structures
- **Precision**: AST nodes are unambiguous (no parsing issues)
- **Compositionality**: Tree structure enables recursive definitions
- **Implementation**: Compilers transform text → AST once, then work with AST throughout

---

**Previous**: [Lexical Structure](02_LexicalStructure.md) | **Next**: [Attributes](04_Attributes.md)
