# The Cantrip Language Specification

**Version:** 1.0.0 (compatible with 0.7.x)
**Date:** October 21, 2025
**Status:** Normative Reference
**Target Audience:** Language implementers, library authors, and application developers

---

## Preface and Editorial Policy

This document is a reorganized, fully normative specification of the **Cantrip** programming language.
It preserves all technical material from the prior draft specification while clarifying structure,
marking normative vs. informative text, and standardizing terminology and code fences.
Where this document introduces clarifications or editorial fixes, the original technical content
is retained and the canonical rules are stated explicitly.


### Key Words for Requirement Levels
The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**,
**RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in
RFC 2119 and RFC 8174 when, and only when, they appear in all capitals.

### Normative vs. Informative

> **Sticky Banners:** Each Part/section heading in this document carries a **(Normative)** or **(Informative)** tag. When a tag appears at the Part level, it applies to all sections within that Part unless a section overrides it explicitly.
Sections explicitly marked **(Normative)** define conformance requirements. Sections marked **(Informative)**
provide examples, guidance, or background and are non-binding. “LLM Quick Reference” is informative.

### Conformance
An implementation conforms to this specification if and only if it satisfies all normative requirements
stated herein. Extensions MUST NOT invalidate any program that is valid under this specification. Diagnostics,
implementation-defined limits, and unspecified behavior MUST be documented.

### Document Conventions
- Code blocks for the language use the fenced code label `cantrip`.
- Grammar productions use `ebnf` fences.
- Mathematical judgments are typeset with standard notation.
- Error identifiers (e.g., `E9001`) are normative and MUST be reported for corresponding violations.

---

# The Cantrip Language Specification

**Version:** 1.0.0 (compatible with 0.7.x)
**Date:** December 2024
**Status:** Normative Reference
**Target Audience:** Language implementers, library authors, and application developers

---

# Part 0: LLM Quick Reference (See Separate Document)

**The complete LLM onboarding and alignment guide has been moved to \Cantrip-LLM-Guide.md\.**

For comprehensive LLM guidance including:
- Core syntax patterns with correct/incorrect examples
- Common idioms and best practices
- Critical differences from similar languages
- Error codes indexed by common LLM mistakes

** See:** \Cantrip-LLM-Guide.md
### Quick Reference

**Key Principles:**
- Effects are NEVER inferred - always explicit with eeds\ clause
- Permissions use sigils: \own\, \mut\, \iso\ (reference has no sigil)
- Modal states use \@State\ syntax
- Contracts use equires\ and \ensures
**For complete LLM guidance, see \Cantrip-LLM-Guide.md\.**

---

# Part I (Normative): Foundational Concepts

## 1. Notation and Mathematical Foundations

### 1.1 Metavariables

Throughout this specification, we use the following metavariable conventions:

```
x, y, z ∈ Var          (variables)
f, g, h ∈ FunName      (function names)
T, U, V ∈ Type         (types)
S, S' ∈ State          (modal states)
e ∈ Expr               (expressions)
v ∈ Value              (values)
ε ∈ Effect             (effects)
P, Q ∈ Assertion       (contract assertions)
σ ∈ Store              (memory stores)
Γ ∈ Context            (type contexts)
Δ ∈ EffContext         (effect contexts)
Σ ∈ StateContext       (state contexts)
```

### 1.2 Judgment Forms

**Type judgments:**
```
Γ ⊢ e : T                         (expression e has type T in context Γ)
Γ ⊢ e : T@S                       (expression e has type T in state S)
Γ ⊢ e : T@S ! ε                   (type T, state S, effect ε)
```

**Effect judgments:**
```
Γ ⊢ e ! ε                         (expression e has effect ε)
effects(e) = ε                    (computed effect of e)
```

**Contract judgments:**
```
{P} e {Q}                         (Hoare triple)
σ ⊨ P                             (store satisfies assertion)
needs(f) = ε                      (function f needs effects ε)
requires(f) = P                   (function f requires precondition P)
ensures(f) = Q                    (function f ensures postcondition Q)
```

**Operational semantics:**
```
⟨e, σ, ε⟩ → ⟨e', σ', ε'⟩          (small-step reduction)
⟨e, σ⟩ ⇓ ⟨v, σ'⟩                   (big-step evaluation)
```

**State transitions:**
```
S₁ →ₘ S₂                          (state transition via procedure m)
Σ ⊢ T@S₁ →ₘ T@S₂                  (valid state transition)
```

### 1.2.6 LLM Accessibility Notation

**For AI Code Generation:** Throughout this specification, we use consistent patterns designed for machine readability:

- **Explicit over implicit**: All ownership, effects, and contracts visible in code
- **Named abstractions**: Effects and modals use descriptive names
- **Consistent syntax**: One way to express each concept
- **Documented patterns**: Common idioms shown with anti-patterns

When generating code, prefer:
- ✅ Explicit ownership annotations at boundaries
- ✅ Named effect definitions from `std.effects`
- ✅ Clear modal state types
- ✅ Contracts with natural language descriptions

Avoid:
- ❌ Implicit ownership transfer
- ❌ Anonymous effect unions
- ❌ Ambiguous state representations
- ❌ Vague or missing specifications

### 1.3 Formal Operators

**Set theory:**
```
∈       (element of)
⊆       (subset)
∪       (union)
∩       (intersection)
∅       (empty set)
×       (cartesian product)
```

**Logic:**
```
∧       (conjunction)
∨       (disjunction)
¬       (negation)
⇒       (implication)
∀       (universal quantification)
∃       (existential quantification)
```

**Relations:**
```
→       (maps to / reduces to)
⇒       (implies / pipeline)
≡       (equivalence)
⊢       (entails / proves)
⊨       (satisfies / models)
```

### 1.4 Conventions

**Naming conventions:**
- Type names: `CamelCase` (e.g., `Point`, `Vec`)
- Functions: `snake_case` (e.g., `read_file`, `compute_sum`)
- Constants: `SCREAMING_SNAKE` (e.g., `MAX_SIZE`, `DEFAULT_PORT`)
- Type variables: Single uppercase letters (e.g., `T`, `U`, `V`)

**Syntactic sugar notation:**
- `[T]` - Array/slice of T
- `T?` - `Option<T>`
- `T!` - `Result<T, Error>`

---

## 2. Lexical Structure

### 2.1 Source Files

**Definition 2.1 (Source File):** An Cantrip source file is a UTF-8 encoded text file with extension `.arc`.

**Formal properties:**
- Encoding: UTF-8
- Line endings: LF (`\n`) or CRLF (`\r\n`)
- BOM: Optional (ignored if present)
- Maximum file size: Implementation-defined (recommended: 1MB)

### 2.2 Comments

**Syntax:**
```cantrip
// Line comment

/* Block comment
   spanning multiple lines */

/// Documentation comment for following item
function example() { ... }

//! Module-level documentation
```

**Formal definition:**
```ebnf
LineComment ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" any)* "*/"
DocComment ::= "///" ~[\n\r]*
ModuleDoc ::= "//!" ~[\n\r]*
```

**Semantic rules:**
- Comments do not affect program semantics
- Comments are stripped before parsing
- Doc comments are preserved for documentation generation

### 2.3 Identifiers

**Definition 2.2 (Identifier):** An identifier is a sequence of characters used to name program entities.

**Syntax:**
```ebnf
Identifier ::= IdentStart IdentContinue*
IdentStart ::= [a-zA-Z_]
IdentContinue ::= [a-zA-Z0-9_]
```

**Examples:**
```cantrip
valid_identifier
_private
CamelCase
snake_case
CONSTANT_VALUE
identifier123
```

**Restrictions:**
- Cannot be a reserved keyword
- Cannot start with a digit
- Case-sensitive: `Variable` ≠ `variable`
- Maximum length: Implementation-defined (recommended: 255 characters)

**Formal property (uniqueness):**
```
∀ x, y ∈ Identifier. x = y ⟺ string(x) = string(y)
```

### 2.4 Literals

#### 2.4.1 Integer Literals

**Syntax:**
```ebnf
IntegerLiteral ::= DecimalLiteral
                 | HexLiteral
                 | BinaryLiteral
                 | OctalLiteral

DecimalLiteral ::= [0-9] [0-9_]*
HexLiteral ::= "0x" [0-9a-fA-F] [0-9a-fA-F_]*
BinaryLiteral ::= "0b" [01] [01_]*
OctalLiteral ::= "0o" [0-7] [0-7_]*
```

**Examples:**
```cantrip
42              // Decimal
0x2A            // Hexadecimal (42)
0b101010        // Binary (42)
0o52            // Octal (42)
1_000_000       // With separators (1000000)
```

**Type inference:**
- Default type: `i32`
- Suffix determines type: `42u64`, `100i8`
- Context can override: `let x: u8 = 42;`

**Formal semantics:**
```
⟦n⟧ᵢₙₜ = value of integer n in base-10
⟦0xN⟧ᵢₙₜ = value of N in base-16
⟦0bN⟧ᵢₙₜ = value of N in base-2
⟦0oN⟧ᵢₙₜ = value of N in base-8
```

#### 2.4.2 Floating-Point Literals

**Syntax:**
```ebnf
FloatLiteral ::= DecimalLiteral "." DecimalLiteral Exponent?
Exponent ::= [eE] [+-]? DecimalLiteral
```

**Examples:**
```cantrip
3.14
1.0e10
2.5e-3
0.1
```

**Type inference:**
- Default type: `f64`
- Suffix determines type: `3.14f32`

**Formal semantics:**
```
⟦d₁.d₂⟧_float = d₁ + d₂ × 10^(-|d₂|)
⟦x eᵏ⟧_float = x × 10^k
```

#### 2.4.3 Boolean Literals

**Syntax:**
```
BooleanLiteral ::= "true" | "false"
```

**Formal semantics:**
```
⟦true⟧_bool = ⊤
⟦false⟧_bool = ⊥
```

#### 2.4.4 Character Literals

**Syntax:**
```ebnf
CharLiteral ::= "'" (EscapeSequence | ~['\\]) "'"
EscapeSequence ::= "\\" [nrt\\'"0]
                 | "\\x" HexDigit HexDigit
                 | "\\u{" HexDigit+ "}"
```

**Examples:**
```cantrip
'a'
'\n'            // Newline
'\t'            // Tab
'\\'            // Backslash
'\x41'          // 'A'
'\u{1F600}'     // 😀 (emoji)
```

**Type:** `char` (32-bit Unicode scalar value)

#### 2.4.5 String Literals

**Syntax:**
```ebnf
StringLiteral ::= '"' (EscapeSequence | ~["\\])* '"'
```

**Examples:**
```cantrip
"hello world"
"line 1\nline 2"
"unicode: 🚀"
"quotes: \" "
```

**Type:** `str` (immutable string slice)

**Formal semantics:**
```
⟦"s"⟧_string = sequence of Unicode code points
```

### 2.5 Keywords


### 2.6 Attributes (Normative)

**Purpose:** Attributes are metadata annotations that modify compilation behavior of the item that follows.
Attributes are parsed but do not change program semantics unless specified below.

**Syntax:**
```ebnf
Attribute      ::= "#[" AttributeBody "]"
AttributeBody  ::= Ident ( "(" AttrArgs? ")" )?
AttrArgs       ::= AttrArg ( "," AttrArg )*
AttrArg        ::= Ident "=" Literal | Literal | Ident
```

**Placement:** Attributes MAY appear on modules, records, enums, traits, impl blocks, functions, procedures,
parameters, type aliases, and effects, unless a specific attribute restricts its placement.

**Core attributes:**

- `#[repr(...)]` — layout control (see §6.5)
- `#[verify(static|runtime|none)]` — verification mode (see §20.1)
- `#[overflow_checks(true|false)]` — integer overflow checking (see §4.1.1)
- `#[module(...)]` — module metadata (see §30.3)

**New attribute: `#[alias(...)]` (Informative tooling; Normative parsing)**

The `#[alias]` attribute declares alternative *source-level* names for an item to improve diagnostics,
code search, and LLM tooling. It is ignored for symbol linkage and ABI.

```cantrip
#[alias("to_string", "stringify")]
public function to_str(x: Debug): String { x.debug() }
```

Rules:
1. **Scope:** Aliases are accepted in diagnostics, code actions, and import suggestions. They MUST NOT
   create new exported symbols nor change name resolution in compiled code.
2. **Collision:** If an alias collides with a real item name in the same scope, the real name wins.
   A warning `W0101` SHOULD be emitted.
3. **Debug-only export:** Aliases are not exported; they MAY appear in machine-readable output (see §55)
   behind `emitAliases=true` to help IDEs/LLMs.
4. **FFI:** To control external symbol names, use `#[repr(C)]` and FFI attributes in §56, not `#[alias]`.

**Definition 2.3 (Keyword):** A keyword is a reserved identifier with special syntactic meaning.

**Reserved keywords (cannot be used as identifiers):**

```
abstract    actor       as          async       await
break       case        comptime    const       continue
defer       effect      else        ensures     enum
exists      false       for         forall      function
if          impl        import      internal    invariant
iso         let         loop        match       modal
module      move        mut         needs       new
none        own         private     protected   procedure
public      record      ref         region      requires
result      select      self        Self        state
static      trait       true        type        var
where       while
```

**Contextual keywords (special meaning in specific contexts):**
```
effects     pure
```


**Contextual keywords (special meaning in specific contexts):**
```
effects     pure
```

---

## 3. Abstract Syntax

### 3.1 Type Syntax

**Definition 3.1 (Type Language):**


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
    | T@S                                    (type T in modal state S)
    | ∀α. T                                  (polymorphic type)
    | !                                      (never type)
    | record Name                            (record type)
    | modal Name                             (modal type)
    | enum Name                              (enum type)
    | trait Name                             (trait type)
    | actor Name                             (actor type)
```


**Type well-formedness:**

```
[T-Int]
────────────────
Γ ⊢ i32 : Type

[T-Array]
Γ ⊢ T : Type    n > 0
────────────────────────
Γ ⊢ [T; n] : Type

[T-Fun]
Γ ⊢ T₁ : Type    Γ ⊢ T₂ : Type
─────────────────────────────────
Γ ⊢ T₁ -> T₂ : Type

[T-Modal]
modal P { ... } well-formed
───────────────────────────────
Γ ⊢ modal P : Type
```

### 3.2 Expression Syntax

**Definition 3.2 (Expression Language):**

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
    | e₁ ; e₂                               (sequencing)
    | move e                                (ownership transfer)
    | region r { e }                        (region block)
    | alloc_in⟨r⟩(e)                        (region allocation)
    | match e { pᵢ -> eᵢ }                  (pattern matching)
    | while e₁ do e₂                        (loop)
    | loop { e }                            (infinite loop)
    | break                                 (loop exit)
    | continue                              (loop continue)
    | return e                              (early return)
    | contract(P, e, Q)                     (contract annotation)
    | transition(e, S)                      (explicit state transition)
    | comptime { e }                        (compile-time execution)
```

### 3.3 Value Syntax

**Definition 3.3 (Values):**

```
v ::= n                                     (integer literal)
    | f                                     (float literal)
    | true | false                          (boolean)
    | ()                                    (unit/none)
    | (v₁, ..., vₙ)                         (tuple value)
    | λx:T. e                               (closure)
    | ℓ                                     (location/pointer)
```

### 3.4 Pattern Syntax

**Definition 3.4 (Patterns):**

```
p ::= x                                     (variable pattern)
    | _                                     (wildcard)
    | n                                     (integer literal)
    | true | false                          (boolean literal)
    | (p₁, ..., pₙ)                         (tuple pattern)
    | Variant(p)                            (enum pattern)
    | Record { f₁: p₁, ..., fₙ: pₙ }       (record pattern)
    | p if e                                (guard pattern)
```

### 3.5 Contract Syntax

**Definition 3.5 (Assertions):**

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
    needs ε;                                (effect declaration)
    requires P;                             (preconditions)
    ensures Q;                              (postconditions)
```

### 3.6 Effect Syntax

**Definition 3.6 (Effects):**

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

---

# Part II (Normative): Type System

## 4. Types and Values

### 4.1 Primitive Types

**Definition 4.1 (Primitive Types):** Cantrip provides built-in types for common data representations.

#### 4.1.1 Integer Types

**Signed integers:**

| Type | Size | Range | Formal Definition |
|------|------|-------|-------------------|
| `i8` | 8 bits | -128 to 127 | ℤ ∩ [-2⁷, 2⁷-1] |
| `i16` | 16 bits | -32,768 to 32,767 | ℤ ∩ [-2¹⁵, 2¹⁵-1] |
| `i32` | 32 bits | -2³¹ to 2³¹-1 | ℤ ∩ [-2³¹, 2³¹-1] |
| `i64` | 64 bits | -2⁶³ to 2⁶³-1 | ℤ ∩ [-2⁶³, 2⁶³-1] |
| `isize` | Platform | Pointer-sized | ℤ ∩ [-2ⁿ⁻¹, 2ⁿ⁻¹-1] where n = ptr_width |

**Unsigned integers:**

| Type | Size | Range | Formal Definition |
|------|------|-------|-------------------|
| `u8` | 8 bits | 0 to 255 | ℕ ∩ [0, 2⁸-1] |
| `u16` | 16 bits | 0 to 65,535 | ℕ ∩ [0, 2¹⁶-1] |
| `u32` | 32 bits | 0 to 4,294,967,295 | ℕ ∩ [0, 2³²-1] |
| `u64` | 64 bits | 0 to 2⁶⁴-1 | ℕ ∩ [0, 2⁶⁴-1] |
| `usize` | Platform | Pointer-sized | ℕ ∩ [0, 2ⁿ-1] where n = ptr_width |

**Overflow behavior:**

**Axiom 4.1 (Integer Arithmetic):**
```
For type T with min M and max N:
- Debug mode: x ⊕ y ∉ [M, N] ⟹ panic
- Release mode: (x ⊕ y) mod (N - M + 1) ∈ [M, N]
```

**Example:**
```cantrip
let x: u8 = 255;
let y = x + 1;  // Debug: panic, Release: 0 (wrapping)
```

**Attribute control:**
```cantrip
#[overflow_checks(true)]
function critical_math(a: i32, b: i32): i32 {
    a * b  // Always checked, even in release
}
```

#### 4.1.2 Floating-Point Types

**Definition 4.2 (Floating-Point Types):**

| Type | Size | Standard | Precision | Formal |
|------|------|----------|-----------|--------|
| `f32` | 32 bits | IEEE 754 | Single | ℝ ∩ representable₃₂ |
| `f64` | 64 bits | IEEE 754 | Double | ℝ ∩ representable₆₄ |

**Special values:**
```cantrip
let inf: f64 = 1.0 / 0.0;       // ∞
let neg_inf: f64 = -1.0 / 0.0;  // -∞
let nan: f64 = 0.0 / 0.0;       // NaN
```

**Formal properties:**
```
⟦f₁ + f₂⟧ = round(⟦f₁⟧ + ⟦f₂⟧)
⟦f₁ * f₂⟧ = round(⟦f₁⟧ × ⟦f₂⟧)
NaN ≠ NaN
x + ∞ = ∞ (for x ≠ -∞)
```

#### 4.1.3 Boolean Type

**Definition 4.3 (Boolean Type):**
```
⟦bool⟧ = {true, false} = {⊤, ⊥}
```

**Operations:**
```
¬⊤ = ⊥
¬⊥ = ⊤
⊤ ∧ ⊤ = ⊤
⊤ ∧ ⊥ = ⊥
⊥ ∧ _ = ⊥    (short-circuit)
```

**Example:**
```cantrip
let a = true;
let b = false;
let c = a && b;  // false
let d = a || b;  // true
```

#### 4.1.4 Character Type

**Definition 4.4 (Character Type):**
```
⟦char⟧ = Unicode Scalar Values = [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]
```

**Properties:**
- Size: 32 bits (4 bytes)
- Valid range: Excludes surrogate pairs [U+D800, U+DFFF]
- Representation: UTF-32

**Example:**
```cantrip
let ch: char = 'A';         // U+0041
let emoji: char = '🚀';     // U+1F680
```

### 4.2 Compound Types

#### 4.2.1 Arrays

**Definition 4.5 (Fixed-Size Array):**
```
[T; n] = { (v₁, ..., vₙ) | ∀i. vᵢ : T }
```

**Formal properties:**
```
size([T; n]) = n × size(T)
align([T; n]) = align(T)
layout = contiguous memory
```

**Example:**
```cantrip
let numbers: [i32; 5] = [1, 2, 3, 4, 5];
let zeros: [i32; 100] = [0; 100];  // Repeat syntax

// Type rules:
// Γ ⊢ numbers : [i32; 5]
// Γ ⊢ numbers[0] : i32
```

**Access semantics:**
```
[T-Array-Index]
Γ ⊢ arr : [T; n]    Γ ⊢ i : usize    i < n
────────────────────────────────────────────
Γ ⊢ arr[i] : T
```

#### 4.2.2 Slices

**Definition 4.6 (Slice):**
```
[T] = { (ptr, len) | ptr : *T, len : usize }
```

**Fat pointer representation:**
```
record SliceRepr<T> {
    data: *T;      // Pointer to first element
    length: usize; // Number of elements
}
```

**Example:**
```cantrip
let arr = [1, 2, 3, 4, 5];
let slice: [i32] = arr[1..3];  // [2, 3]

// Range semantics:
// arr[a..b] = { arr[i] | a ≤ i < b }
```

**Type rules:**
```
[T-Slice]
Γ ⊢ arr : [T; n]    Γ ⊢ start : usize    Γ ⊢ end : usize
start ≤ end ≤ n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..end] : [T]
```

#### 4.2.3 Tuples

**Definition 4.7 (Tuple):**
```
(T₁, ..., Tₙ) = T₁ × T₂ × ... × Tₙ
```

**Example:**
```cantrip
let pair: (i32, str) = (42, "answer");
let (x, y) = pair;          // Destructuring
let first = pair.0;          // Index access
let second = pair.1;

// Type rules:
// Γ ⊢ pair : (i32, str)
// Γ ⊢ pair.0 : i32
// Γ ⊢ pair.1 : str
```

**Formal semantics:**
```
[T-Tuple]
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────
Γ ⊢ (e₁, ..., eₙ) : (T₁, ..., Tₙ)

[T-Tuple-Proj]
Γ ⊢ e : (T₁, ..., Tₙ)    i ∈ [0, n)
───────────────────────────────────────
Γ ⊢ e.i : Tᵢ
```

### 4.3 Never Type

**Definition 4.8 (Never Type):**
```
⟦!⟧ = ∅  (empty type, no values)
```

**Formal property (Bottom Type):**
```
[T-Never]
Γ ⊢ e : !
────────────────  (for any type T)
Γ ⊢ e : T
```

**Uses:**
1. Functions that never return
2. Match arms that are unreachable
3. Error handling branches

**Example:**
```cantrip
function panic(message: str): !
    needs panic;
{
    std.process.abort();
}

function example(x: i32): i32 {
    if x < 0 {
            panic("negative value");  // Type: !
            // Compiler knows this never returns
        } else {
            x * 2  // Only this branch returns
        }
}
```

---

## 5. Type Rules and Semantics

### 5.1 Core Type Rules

**[T-Var] Variable typing:**
```
x : T ∈ Γ
───────────
Γ ⊢ x : T
```

**Example:**
```cantrip
let x: i32 = 42;
// Context: Γ = {x : i32}
// Judgment: Γ ⊢ x : i32
```

**[T-Let] Let binding:**
```
Γ ⊢ e₁ : T₁    Γ, x : T₁ ⊢ e₂ : T₂
───────────────────────────────────────
Γ ⊢ let x = e₁ in e₂ : T₂
```

**Example:**
```cantrip
let result = let x = 42 in x + 1;
// Type derivation:
// ⊢ 42 : i32
// x : i32 ⊢ x + 1 : i32
// ⊢ let x = 42 in x + 1 : i32
```

**[T-App] Function application:**
```
Γ ⊢ e₁ : T -> U    Γ ⊢ e₂ : T
──────────────────────────────
Γ ⊢ e₁(e₂) : U
```

**Example:**
```cantrip
function double(x: i32): i32 {
    x * 2
}

let result = double(21);
// double : i32 -> i32
// 21 : i32
// double(21) : i32
```

**[T-Abs] Lambda abstraction:**
```
Γ, x : T ⊢ e : U
────────────────────
Γ ⊢ λx:T. e : T -> U
```

**Example:**
```cantrip
let add_one = |x: i32| { x + 1 };
// Type: i32 -> i32
```

**[T-If] Conditional:**
```
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : T    Γ ⊢ e₃ : T
────────────────────────────────────────────
Γ ⊢ if e₁ then e₂ else e₃ : T
```

**Example:**
```cantrip
let max = if a > b { a } else { b };
// Both branches must have same type
```

**[T-Pipeline] Pipeline operator:**
```
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T -> U
──────────────────────────────
Γ ⊢ e₁ => e₂ : U
```

**Example:**
```cantrip
let result = data
    => filter(predicate)
    => map(transform)
    => sum;
// Each stage must match types
```

### 5.2 Subtyping

**Definition 5.1 (Subtype Relation):**
```
T <: U  (T is a subtype of U)
```

**[Sub-Refl] Reflexivity:**
```
─────────
T <: T
```

**[Sub-Trans] Transitivity:**
```
T <: U    U <: V
────────────────
T <: V
```

**[Sub-Never] Bottom type:**
```
─────────
! <: T
```

**[Sub-Fun] Function contravariance:**
```
T₂ <: T₁    U₁ <: U₂
────────────────────────
T₁ -> U₁ <: T₂ -> U₂
```

Note: Function parameters are contravariant!

**[Sub-Permission] Permission hierarchy:**
```
own T <: mut T <: T
iso T <: own T
```

### 5.3 Type Inference

**Type inference algorithm:** Hindley-Milner with extensions for effects and modals.

**Constraint generation:**
```
Γ ⊢ e : T | C  (expression e has type T with constraints C)
```

**Example inference:**
```cantrip
function identity(x) {  // No type annotation, return type inferred
    x
}

// Generates constraints:
// ∀α. identity : α -> α
// Solution: polymorphic type
```

**Inference limits:**
1. Function signatures must declare effects explicitly (no inference)
2. Record fields must have explicit types
3. Function return types can be inferred from body
4. Local variables can be inferred from initialization

---

## 6. Records and Classes

**Definition 6.0 (Record as Labeled Product Type):**
A *record* is a labeled Cartesian product of its fields. Formally,
if `R { f₁: T₁; …; fₙ: Tₙ }` then `⟦R⟧ ≅ T₁ × … × Tₙ` with
named projections `fᵢ : R → Tᵢ`. Records provide *procedures* (see §13.7)
via `impl R` blocks; inheritance is not part of the model—use traits and composition.


### 6.1 Record Declaration

**Syntax:**
```cantrip
record Name {
    field₁: Type₁;
    ...
    fieldₙ: Typeₙ;
}
```

**Formal definition:**
```
[Record-Def]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
fields distinct
──────────────────────────────────────────────
Γ ⊢ record Name { f₁: T₁; ...; fₙ: Tₙ } : Type
```

**Example:**
```cantrip
record Point {
    x: f64;
    y: f64;
}

public record User {
    public name: String;
    public email: String;
    private password_hash: String;
}
```

**Memory layout:**
```
Point layout:
[x: 8 bytes][y: 8 bytes]
Total: 16 bytes, align: 8
```

### 6.2 Record Construction

**Syntax:**
```cantrip
Name { field₁: expr₁, ..., fieldₙ: exprₙ }
```

**Type rule:**
```
[T-Record]
record Name { f₁: T₁; ...; fₙ: Tₙ }
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────────────
Γ ⊢ Name { f₁: e₁, ..., fₙ: eₙ } : Name
```

**Example:**
```cantrip
let origin = Point { x: 0.0, y: 0.0 };

// Field shorthand
let x = 3.0;
let y = 4.0;
let point = Point { x, y };  // Equivalent to Point { x: x, y: y }
```

### 6.3 Field Access

**Type rule:**
```
[T-Field]
Γ ⊢ e : record S    field f : T in S
────────────────────────────────────
Γ ⊢ e.f : T
```

**Example:**
```cantrip
let p = Point { x: 1.0, y: 2.0 };
let x_coord = p.x;  // x_coord : f64
```

### 6.4 Procedures

**Definition 6.1 (Procedure):** A procedure is a function with an explicit `self` parameter.

**Syntax:**
```cantrip
impl StructName {
    function method_name(self: Permission StructName, params): T
        requires ...;
        ensures ...;
    {
        ...
    }
}
```

**Example:**
```cantrip
impl Point {
    function new(x: f64, y: f64): own Point {
        own Point { x, y }
    }

    function distance(self: Point, other: Point): f64 {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }

    function move_by(self: mut Point, dx: f64, dy: f64) {
        self.x += dx;
        self.y += dy;
    }
}
```

**Procedure call desugaring:**
```
obj.procedure(args) ≡ Type::procedure(obj, args)
```

**Type rule:**
```
[T-Procedure]
procedure m : Self -> T -> U in impl for S
Γ ⊢ self : S    Γ ⊢ arg : T
───────────────────────────────────────
Γ ⊢ self.m(arg) : U
```

### 6.5 Memory Layout Control

**C-compatible layout:**
```cantrip
#[repr(C)]
record Vector3 {
    x: f32;  // offset 0
    y: f32;  // offset 4
    z: f32;  // offset 8
}
// Size: 12 bytes, Align: 4
```

**Packed layout (no padding):**
```cantrip
#[repr(packed)]
record NetworkHeader {
    magic: u32;     // offset 0
    version: u16;   // offset 4
    length: u16;    // offset 6
}
// Size: 8 bytes, Align: 1
```

**Explicit alignment:**
```cantrip
#[repr(align(64))]
record CacheLine {
    data: [u8; 64];
}
// Aligned to 64-byte boundary
```

**Formal semantics:**
```
layout(record S) = {
    size = ∑ᵢ size(fᵢ) + padding
    align = max(align(f₁), ..., align(fₙ))
    offset(fᵢ) = aligned(∑ⱼ₍ⱼ₍ᵢ₎ size(fⱼ), align(fᵢ))
}
```

### 6.6 Structure-of-Arrays

**Attribute:**
```cantrip
#[soa]
record Particle {
    position: Vec3;
    velocity: Vec3;
    lifetime: f32;
}
```

**Compiler transformation:**
```cantrip
// Generates:
record ParticleSOA {
    positions: Vec<Vec3>;
    velocities: Vec<Vec3>;
    lifetimes: Vec<f32>;
    length: usize;
}

impl ParticleSOA {
    function get(self: ParticleSOA, index: usize): Particle
        requires index < self.length;
    {
        Particle {
            position: self.positions[index],
            velocity: self.velocities[index],
            lifetime: self.lifetimes[index],
        }
    }
}
```

**Performance benefit:** Better cache locality for SIMD operations.


## 7. Enums and Pattern Matching

### 7.1 Enum Declaration

**Definition 7.1 (Enum):** An enum is a sum type (tagged union) with named variants.

**Syntax:**
```cantrip
enum Name {
    Variant₁,
    Variant₂(Type),
    Variant₃ { field: Type },
}
```

**Formal definition:**
```
⟦enum E { V₁(T₁), ..., Vₙ(Tₙ) }⟧ = T₁ + T₂ + ... + Tₙ
```

**Example:**
```cantrip
enum Status {
    Active,
    Pending,
    Inactive,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

enum Option<T> {
    Some(T),
    None,
}

enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(u8, u8, u8),
}
```

**Memory layout:**
```
enum Layout = {
    discriminant: usize,    // Tag
    payload: union {        // Largest variant
        variant₁: T₁,
        ...
        variantₙ: Tₙ,
    }
}
```

**Type rule:**
```
[T-Enum]
enum E { V₁(T₁), ..., Vₙ(Tₙ) } well-formed
Γ ⊢ e : Tᵢ
──────────────────────────────────────────
Γ ⊢ E.Vᵢ(e) : E
```

### 7.2 Pattern Matching

**Syntax:**
```cantrip
match expression {
    pattern₁ -> expression₁,
    pattern₂ -> expression₂,
    ...
}
```

**Note:** Uses `->` (maps to), not `=>`

**Type rule:**
```
[T-Match]
Γ ⊢ e : T
∀i. Γ, pᵢ ⊢ eᵢ : U
patterns exhaustive for T
──────────────────────────────
Γ ⊢ match e { pᵢ -> eᵢ } : U
```

**Exhaustiveness checking:**

**Definition 7.2 (Exhaustive Patterns):** A set of patterns is exhaustive if every possible value matches at least one pattern.

**Algorithm:**
```
exhaustive(patterns, type) = {
    1. Collect all constructors of type
    2. For each constructor C:
       - Check if any pattern matches C
       - Recursively check nested patterns
    3. Return true if all constructors covered
}
```

**Example:**
```cantrip
enum Status {
    Active,
    Pending,
    Inactive,
}

function describe(status: Status): str {
    match status {
        Status.Active -> "active",
        Status.Pending -> "pending",
        Status.Inactive -> "inactive",
        // Exhaustive - all variants covered
    }
}
```

### 7.3 Pattern Forms

**Literal patterns:**
```cantrip
match number {
    0 -> "zero",
    1 -> "one",
    2 -> "two",
    _ -> "many",  // Wildcard
}
```

**Tuple patterns:**
```cantrip
match point {
    (0, 0) -> "origin",
    (0, y) -> "on y-axis",  // Binds y
    (x, 0) -> "on x-axis",  // Binds x
    (x, y) -> "point",       // Binds both
}
```

**Enum patterns:**
```cantrip
match result {
    Result.Ok(value) -> process(value),
    Result.Err(error) -> handle_error(error),
}
```

**Record patterns:**
```cantrip
match person {
    Person { name, age: 0..=17 } -> "minor: {name}",
    Person { name, age: 18..=64 } -> "adult: {name}",
    Person { name, age: 65.. } -> "senior: {name}",
}
```

**Guard patterns:**
```cantrip
match value {
    x if x > 0 -> "positive",
    x if x < 0 -> "negative",
    _ -> "zero",
}
```

**Type rules for patterns:**

```
[P-Var]
────────────────────
Γ ⊢ x : T ⇝ {x : T}

[P-Tuple]
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────────
Γ ⊢ (p₁, ..., pₙ) : (T₁, ..., Tₙ) ⇝ Γ₁ ∪ ... ∪ Γₙ

[P-Enum]
enum E { V(T), ... }
Γ ⊢ p : T ⇝ Γ'
────────────────────
Γ ⊢ E.V(p) : E ⇝ Γ'
```

### 7.4 Discriminant Values

**Explicit discriminants:**
```cantrip
enum HttpStatus {
    Ok = 200,
    NotFound = 404,
    ServerError = 500,
}

let code: u16 = HttpStatus.Ok as u16;  // 200
```

**Type rule:**
```
[T-Discriminant]
enum E { V₁ = n₁, ..., Vₖ = nₖ }
───────────────────────────────────
E as IntType valid
```

---

## 8. Traits

### 8.1 Trait Declaration

**Definition 8.1 (Trait):** An trait specifies a set of procedures that types must implement.

**Syntax:**
```cantrip
public trait InterfaceName {
    function method₁(self: Self, params) {
        requires: ...;
        returns<T>: ...;
    }

    // Default implementation optional
    function method₂(self: Self): U
        requires ...;
        ensures ...;
    {
        ...
    }
}
```

**Example:**
```cantrip
public trait Drawable {
    function draw(self: Self, canvas: Canvas)
        needs render.draw;

    function bounds(self: Self): Rectangle;
}

public trait Comparable<T> {
    function compare(self: Self, other: T): i32
        ensures
            (result < 0) == (self < other);
            (result == 0) == (self == other);
            (result > 0) == (self > other);
}
```

**Type rule:**
```
[T-Trait]
procedures m₁, ..., mₙ well-formed
────────────────────────────────────────
trait I { m₁; ...; mₙ } well-formed
```

### 8.2 Default Procedures

**Definition 8.2 (Default Implementation):** An trait can provide default implementations for procedures.

**Example:**
```cantrip
public trait Logger {
    // Required procedure
    function log(self: Self, message: String)
        needs io.write;

    // Default implementations
    function log_error(self: Self, err: Error)
        needs io.write;
    {
        self.log("ERROR: " + err.to_string());
    }

    function log_warning(self: Self, msg: String)
        needs io.write;
    {
        self.log("WARN: " + msg);
    }
}
```

**Inheritance of defaults:**
```
impl Logger for MyLogger {
    function log(self: Self, message: String)
    needs io.write;
{
    std.io.println(message);
    }
    // log_error and log_warning inherited
}
```

### 8.3 Implementation Blocks

**Syntax:**
```cantrip
impl InterfaceName for TypeName {
    // Implement required procedures
    function procedure(self: Self, params): T
        requires ...;
        ensures ...;
    {
        ...
    }
}
```

**Example:**
```cantrip
record Point { x: f64, y: f64 }

impl Drawable for Point {
    function draw(self: Self, canvas: Canvas)
        needs render.draw;
    {
        canvas.draw_circle(self.x, self.y, 2.0);
    }

    function bounds(self: Self): Rectangle {
        Rectangle.new(self.x - 2.0, self.y - 2.0, 4.0, 4.0)
    }
}
```

**Type rule:**
```
[T-Impl]
trait I { m₁: T₁; ...; mₙ: Tₙ }
record S { ... }
∀i. impl mᵢ : Tᵢ[Self ↦ S]
───────────────────────────────────
impl I for S well-formed
```

**Coherence requirement:** At most one implementation of an trait for a given type.

```
[Coherence]
impl I for T₁
impl I for T₂
T₁ ≠ T₂
─────────────────
Error: Overlapping implementations
```

### 8.4 Generic Constraints

**Syntax:**
```cantrip
function name<T: Constraint>(param: T): U
    where T: AdditionalConstraint;
    requires ...;
    ensures ...;
{
    ...
}
```

**Example:**
```cantrip
function max<T>(a: T, b: T): T
    where T: Comparable<T>
{
    if a.compare(b) > 0 {
        a
    } else {
        b
    }
}
```

**Type rule:**
```
[T-Generic-Constraint]
Γ, T : Type, T : Constraint ⊢ e : U
─────────────────────────────────────────
Γ ⊢ function f<T: Constraint>(...) : U
```

### 8.5 Associated Types

**Syntax:**
```cantrip
public trait Iterator {
    type Item;

    function next(self: mut Self): Option<Self.Item>;
}
```

**Implementation:**
```cantrip
record RangeIterator {
    current: i32;
    end: i32;
}

impl Iterator for RangeIterator {
    type Item = i32;

    function next(self: mut Self): Option<i32> {
        if self.current < self.end {
            let value = self.current;
            self.current += 1;
            Some(value)
        } else {
            None
        }
    }
}
```

**Type rule:**
```
[T-Assoc-Type]
trait I { type A; ... }
impl I for T { type A = U; ... }
──────────────────────────────────
T : I    T.A = U
```

---


### 8.6 Trait-Declared Effects (Normative)

**Motivation:** Implementations of a trait method SHOULD NOT silently widen observable side effects.
To enable local reasoning, a trait MAY declare *required effects* for each method.

**Grammar extension (add to §8.1):**
```ebnf
TraitMethod   ::= "function" Ident "(" ParamList? ")" ( ":" Type )?
                  NeedsClause? RequiresClause? EnsuresClause? TraitMethodBody?
NeedsClause   ::= "needs" EffectExpr ";"
```
The `needs` clause on a trait method declares an *upper bound* on effects that any implementation may perform.
An implementation method MUST satisfy `ε_impl ⊆ ε_trait` (effect subsumption).

**Type rule (trait effect bound):**
```
[T-Trait-Effects]
trait I { function m(...) : U  needs ε_trait; }
impl I for T { function m(...) : U  needs ε_impl;  body }
ε_impl ⊆ ε_trait
──────────────────────────────────────────────────────────
impl well-typed
```

**Diagnostics:**
- `E9120` — *Trait effect exceeds bound*: implementation declares or exhibits effects not permitted by the trait.
  Fix-it: “narrow `needs` to be ≤ specified bound” or “refine trait bound”.

**Examples:**
```cantrip
public trait Logger {
    function log(self: Self, message: String)
        needs io.write;
}

impl Logger for MyLogger {
    function log(self: Self, message: String)
        needs io.write; // OK: equal
    { std.io.println(message); }
}

impl Logger for BadLogger {
    function log(self: Self, message: String)
        needs io.write, fs.write; // ERROR E9120: fs.write exceeds trait bound
    { std.fs.append("log.txt", message) }
}
```

### 8.7 Coherence and Orphan Rules (Normative)

**Coherence (global uniqueness):** For any fully monomorphized pair `(Trait, Type)`, at most one implementation
is visible in the *program* (the primary package plus all transitive dependencies).
Overlapping implementations are a compile error.

```
[Coherence-Global]
(impl I for T) and (impl I for T) from distinct crates
──────────────────────────────────────────────────────
ERROR E8201: overlapping implementations
```

**Orphan rule (locality):** An implementation `impl I for T` is permitted iff at least one of the following holds:
1. `I` is defined in the current package, or
2. the *outermost* type constructor of `T` is defined in the current package.

For generic `T = C<U1, ..., Un>`, the outer constructor `C` must be local.
Parametric arguments do not make an impl local by themselves.

```
[Orphan]
I ∉ Local  ∧  outer(T) ∉ Local
───────────────────────────────
ERROR E8202: orphan implementation
```

**Blanket impls:** Blanket implementations (e.g., `impl<T: Bound> I for Vec<T>`) are allowed if they satisfy the orphan rule.
Specialization is not part of this specification.

**Visibility of impls:** Implementations are public by definition (§32.7).


## 9. Generics and Parametric Polymorphism

### 9.1 Generic Functions

**Definition 9.1 (Parametric Polymorphism):** Generic functions abstract over types.

**Syntax:**
```cantrip
function name<T₁, ..., Tₙ>(params): U {
    ...
}
```

**Example:**
```cantrip
function identity<T>(x: T): T {
    x
}

function swap<T>(a: T, b: T): (T, T) {
    (b, a)
}

function first<T, U>(pair: (T, U)): T {
    pair.0
}
```

**Type rule:**
```
[T-Generic-Fun]
Γ, α₁ : Type, ..., αₙ : Type ⊢ body : T
─────────────────────────────────────────────
Γ ⊢ function f<α₁, ..., αₙ>(...) : ∀α₁...αₙ. T
```

**Instantiation:**
```
[T-Inst]
Γ ⊢ e : ∀α. T    Γ ⊢ U : Type
──────────────────────────────
Γ ⊢ e : T[α ↦ U]
```

### 9.2 Generic Records

**Syntax:**
```cantrip
record Name<T₁, ..., Tₙ> {
    field₁: Type₁,
    ...
}
```

**Example:**
```cantrip
record Pair<T, U> {
    first: T;
    second: U;
}

record Box<T> {
    value: T;
}

impl<T> Box<T> {
    function new(value: T): own Box<T>
        needs alloc.heap;
    {
        own Box { value }
    }

    function get(self: Box<T>): T {
        self.value
    }
}
```

**Type rule:**
```
[T-Generic-Record]
Γ, α₁ : Type, ..., αₙ : Type ⊢ fields well-formed
──────────────────────────────────────────────────────
Γ ⊢ record S<α₁, ..., αₙ> { ... } : Type -> ... -> Type
```

### 9.3 Where Clauses

**Syntax:**
```cantrip
function name<T>(param: T)
    where T: Constraint₁,
          T: Constraint₂,
          U: Constraint₃
{
    ...
}
```

**Example:**
```cantrip
function process<T, U>(data: T): U
    where T: Clone + Serialize,
          U: Display + Debug
    needs alloc.heap;
{
    // Can use clone(), serialize(), display(), debug()
    let copy = data.clone();
    // ...
}
```

**Type rule:**
```
[T-Where]
Γ, T : Type, T : C₁, ..., T : Cₙ ⊢ body : U
────────────────────────────────────────────────
Γ ⊢ function f<T> where T: C₁, ..., Cₙ : U
```

### 9.4 Monomorphization

**Definition 9.2 (Monomorphization):** Generic code is specialized for each concrete type at compile time.

**Transformation:**
```cantrip
// Generic function
function max<T: Comparable>(a: T, b: T): T {
    if a.compare(b) > 0 { a } else { b }
}

// Usage
let x = max(1, 2);       // T = i32
let y = max(1.0, 2.0);   // T = f64

// Compiler generates:
function max_i32(a: i32, b: i32) -> i32 { ... }
function max_f64(a: f64, b: f64) -> f64 { ... }
```

**Performance:** Zero-cost abstraction - no runtime dispatch

**Code size:** May increase binary size (trade-off for speed)

### 9.5 Type Bounds
### 9.6 Const Generics (Normative)

**Purpose:** *Const generics* allow types and functions to be parameterized by compile‑time constants
(e.g., array lengths, matrix dimensions, bit widths) without runtime overhead.

**Syntax (parameters and instantiation):**
```ebnf
ConstParam     ::= "const" Ident ":" ConstType
ConstType      ::= "usize" | "u8" | "u16" | "u32" | "u64" | "isize" | "i8" | "i16" | "i32" | "i64" | "bool"
GenericParams  ::= "<" (TypeParam | ConstParam) ("," (TypeParam | ConstParam))* ">"
ConstArg       ::= IntegerLiteral | BooleanLiteral | ConstExpr
ConstExpr      ::= (literal | arithmetic over consts | sizeof(T) | alignof(T))
```

**Examples:**
```cantrip
// Function parameterized by a const length N
function process<T, const N: usize>(data: [T; N]): T {
    data[0]
}

// Type aliases with const parameters
type Matrix<T, const ROWS: usize, const COLS: usize> = [[T; COLS]; ROWS];

// Records with const parameters
record RingBuffer<T, const CAP: usize> {
    data: [T; CAP];
    head: usize;
    tail: usize;
}
```

**Type rules:**
```
[T-Const-Generic]
Γ, N : const usize ⊢ e : T
───────────────────────────────────────────
Γ ⊢ function f<const N: usize>(...) : T

[T-Const-Inst]
Γ ⊢ n : usize      n is a compile-time constant
Γ ⊢ f : ∀(const N). T
───────────────────────────────────────────
Γ ⊢ f<n> : T[N ↦ n]
```

**Well-formedness of const expressions:** A const argument MUST be evaluable at compile time using only
other consts, integer/boolean arithmetic, and compile‑time queries such as `@sizeOf(T)`, `@alignOf(T)`.
Effects are forbidden in const expressions.

**Interaction with arrays:** The expression `[T; N]` requires `N` to be a const value known at compile time.
Indexing rules (§4.2.1) use the instantiated `N`.

**Monomorphization:** Const parameters participate in monomorphization alongside type parameters.
Each distinct const argument produces a distinct specialized instance.

**Diagnostics:**
- `E9310` — non-constant used where a const generic argument is required.
- `E9311` — const expression not evaluable at compile time.


**Syntax:**
```cantrip
T: Bound₁ + Bound₂ + ... + Boundₙ
```

**Example:**
```cantrip
function serialize<T>(value: T): String
    where T: Serialize + Clone
    needs alloc.heap;
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

---

# Part III (Normative): Modal System

## 10. Modals: State Machines as Types

### 10.1 Modal Overview

**Definition 10.1 (Modal):** A modal is a type with an explicit finite state machine, where each state can have different data and the compiler enforces valid state transitions.

**Key innovation:** State machines are first-class types with compile-time verification.

**When to use modals:**
- Resources with lifecycles (files, connections, transactions)
- Stateful business logic (orders, shopping carts, workflows)
- Network modals (TCP handshakes, authentication flows)
- Any type where "state matters" for correctness

**When to use structs:**
- Simple data with no state machine
- Mathematical types (vectors, matrices)
- Configuration objects

### 10.2 Basic Modal Syntax

**Syntax:**
```cantrip
modal Name {
    state State₁ {
        field₁: Type₁;
        ...
    }

    state State₂ {
        field₁: Type₁;
        field₂: Type₂;  // Different fields!
        ...
    }

    @State₁ >> procedure() >> @State₂
        needs ε;
        requires P;
        ensures Q;
    {
        // Implementation
        State₂ { ... }
    }
}
```

**Example:**
```cantrip
modal File {
    // Each state has its own data shape
    state Closed {
        path: String;
    }

    state Open {
        path: String;
        handle: FileHandle;
        position: usize;
    }

    // State transitions use >> with @ markers
    @Closed >> open() >> @Open
        needs fs.read;
    {
        let handle = FileSystem.open(self.path)?;
        Open {
            path: self.path,
            handle: handle,
            position: 0,
        }
    }

    @Open >> read(n: usize) >> @Open
        needs fs.read;
    {
        let data = self.handle.read(n)?;
        self.position += data.len();
        (data, self)
    }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { path: self.path }
    }
}
```

**Key features:**
- Each state can have **different fields**
- `@State` markers indicate state names in transitions
- `>>` operator shows state flow
- Compiler enforces valid transitions

### 10.3 Using Modals

**Example:**
```cantrip
function example(): Result<(), Error>
    needs fs.read, alloc.heap;
{
    let file = File.new("data.txt");  // File@Closed

    // ERROR: Cannot read from closed file
    // let data = file.read(1024)?;

    let file = file.open()?;  // Now File@Open
    let data = file.read(1024)?;  // OK!

    let file = file.close();  // Back to File@Closed

    // ERROR: Cannot read after closing
    // let data2 = file.read(1024)?;

    Ok(())
}
```

**The compiler prevents invalid operations at compile time!**

**Type annotation:**
```cantrip
let file: File@Closed = File.new("data.txt");
let file: File@Open = file.open()?;
```

### 10.4 State-Dependent Data

**This is the killer feature** - each state has its own data:

```cantrip
modal Transaction {
    state Pending {
        conn: Connection;
        operations: Vec<Operation>;
    }

    state Committed {
        conn: Connection;
        commit_id: u64;        // Only exists after commit!
        timestamp: DateTime;   // Only exists after commit!
    }

    state RolledBack {
        conn: Connection;
        error: Error;          // Only exists after rollback!
    }

    @Pending >> commit() >> @Committed {
        let (id, timestamp) = self.conn.commit(self.operations)?;
        Committed {
            conn: self.conn,
            commit_id: id,
            timestamp: timestamp,
        }
    }

    @Pending >> rollback(err: Error) >> @RolledBack {
        self.conn.rollback();
        RolledBack {
            conn: self.conn,
            error: err,
        }
    }
}

// Usage
let mut tx = Transaction.begin(conn);
tx.add_operation(op1);

let tx = tx.commit()?;

// Compile-time proof that commit_id exists:
println!("Committed as {}", tx.commit_id);  // OK!
```

**Access to state-specific fields:**
```cantrip
function process(tx: Transaction@Committed) {
    // Can access commit_id because state is Committed
    log_commit(tx.commit_id);
    // tx.error;  // ERROR: error only exists in RolledBack state
}
```

### 10.5 Complex State Machines

**Example: TCP Connection**
```cantrip
modal TcpConnection {
    state Disconnected {
        config: ConnectionConfig;
    }

    state Connecting {
        config: ConnectionConfig;
        socket: Socket;
    }

    state Connected {
        config: ConnectionConfig;
        socket: Socket;
        stream: TcpStream;
    }

    state Failed {
        config: ConnectionConfig;
        error: Error;
    }

    @Disconnected >> connect(host: String) >> @Connecting
        needs net.read(outbound);
    {
        let socket = Socket.new(host)?;
        Connecting { config: self.config, socket }
    }

    @Connecting >> handshake() >> (@Connected | @Failed)
        needs net.read, net.write;
    {
        match self.socket.handshake() {
            Ok(stream) -> Ok(Connected {
                config: self.config,
                socket: self.socket,
                stream,
            }),
            Err(err) -> Ok(Failed {
                config: self.config,
                error: err,
            }),
        }
    }

    @Connected >> send(data: [u8]) >> @Connected
        needs net.write;
    {
        self.stream.write(data)?;
        Ok(self)
    }

    @Connected >> disconnect() >> @Disconnected {
        self.stream.close();
        Disconnected { config: self.config }
    }
}
```

### 10.6 State Unions

**Procedures can work with multiple states:**

```cantrip
modal OrderProcessor {
    state Draft { items: Vec<Item>; }
    state Submitted { order_id: u64; items: Vec<Item>; }
    state Cancelled { order_id: u64; reason: String; }

    // Can cancel from either state
    (@Submitted | @Draft) >> cancel(reason: String) >> @Cancelled {
        let order_id = match self {
            Submitted { order_id, .. } -> order_id,
            Draft { .. } -> generate_id(),
        };
        Cancelled { order_id, reason }
    }
}
```

**Type rule:**
```
[T-State-Union]
Γ ⊢ e : P@S₁    S₁ ⊆ (S₂ | S₃)
──────────────────────────────
Γ ⊢ e : P@(S₂ | S₃)
```

### 10.7 Common Fields

**For fields that exist in ALL states:**

```cantrip
modal HttpRequest {
    // Common fields across all states
    common {
        url: String;
        headers: HashMap<String, String>;
    }

    state Building {
        // Only common fields
    }

    state Ready {
        procedure: HttpMethod;
        body: Option<Vec<u8>>;
    }

    state Sent {
        procedure: HttpMethod;
        body: Option<Vec<u8>>;
        sent_at: DateTime;
    }

    @Building >> set_method(m: HttpMethod) >> @Ready {
        Ready {
            // common fields (url, headers) automatically included
            procedure: m,
            body: None,
        }
    }
}
```

**Formal semantics:**
```
state S with common C = {
    fields(S) = C ∪ S.specific_fields
}
```

### 10.8 Modal State Machine Diagrams

**Purpose:** Visual representations help understand state transition logic. This section provides ASCII diagrams and common patterns.

#### 10.8.1 Reading State Diagrams

**Notation:**
```
State --[procedure]--> NextState   // Transition via procedure
State --[event]?--> NextState   // Conditional transition
(StateA | StateB)               // Union of states
[StateC]                        // Final/terminal state
```

#### 10.8.2 File Modal Diagram

```cantrip
modal File {
    //
    // STATE MACHINE:
    //
    //    [Closed] --[open]--> [Open]
    //       ^                    |
    //       |                    |
    //       +------[close]-------+
    //                            |
    //                        [read]
    //                            |
    //                            v
    //                         [Open]  (self-loop)
    //

    state Closed {
        path: String;
    }

    state Open {
        path: String;
        handle: FileHandle;
        position: usize;
    }

    @Closed >> open() >> @Open
        needs fs.read;
    {
        let handle = FileSystem.open(self.path)?;
        Open {
            path: self.path,
            handle: handle,
            position: 0,
        }
    }

    @Open >> read(n: usize) >> @Open
        needs fs.read;
    {
        let data = self.handle.read(n)?;
        self.position += data.len();
        (data, self)
    }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { path: self.path }
    }
}
```

#### 10.8.3 TCP Connection Diagram

```cantrip
modal TcpConnection {
    //
    // STATE MACHINE:
    //
    //                      [timeout]
    //                          |
    //                          v
    //   Disconnected --[connect]--> Connecting --[handshake]?--> Connected
    //        ^                                                         |
    //        |                          +------------------------------+
    //        |                          |                              |
    //        |                          v                              |
    //        +----------------------- Failed                       [send/recv]
    //        |                                                         |
    //        |                                                         |
    //        +------------------[disconnect]-------------------------+
    //

    state Disconnected { config: ConnectionConfig; }
    state Connecting { socket: Socket; }
    state Connected { stream: TcpStream; }
    state Failed { error: Error; }

    @Disconnected >> connect(host: String) >> @Connecting
        needs net.read(outbound);
    {
        let socket = Socket.new(host)?;
        Connecting { socket }
    }

    @Connecting >> handshake() >> (@Connected | @Failed)
        needs net.read, net.write;
    {
        match self.socket.handshake() {
            Ok(stream) -> Ok(Connected { stream }),
            Err(err) -> Ok(Failed { error: err }),
        }
    }

    @Connected >> send(data: [u8]) >> @Connected
        needs net.write;
    {
        self.stream.write(data)?;
        Ok(self)
    }

    @Connected >> disconnect() >> @Disconnected {
        self.stream.close();
        Disconnected { config: self.config }
    }

    @Failed >> retry() >> @Disconnected {
        Disconnected { config: self.config }
    }
}
```

#### 10.8.4 Transaction Modal Diagram

```cantrip
modal Transaction {
    //
    // STATE MACHINE:
    //
    //                    [begin]
    //                      |
    //                      v
    //    Pending --[add_operation]--> Pending  (self-loop)
    //       |
    //       |
    //   [commit]?
    //       |
    //       +---> Committed
    //       |
    //   [rollback]?
    //       |
    //       +---> RolledBack
    //

    state Pending {
        conn: Connection;
        operations: Vec<Operation>;
    }

    state Committed {
        conn: Connection;
        commit_id: u64;
        timestamp: DateTime;
    }

    state RolledBack {
        conn: Connection;
        error: Error;
    }

    @Pending >> add_operation(op: Operation) >> @Pending
        needs alloc.heap;
    {
        self.operations.push(op);
        self
    }

    @Pending >> commit() >> @Committed
        needs net.write, time.read;
    {
        let (id, timestamp) = self.conn.commit(self.operations)?;
        Committed {
            conn: self.conn,
            commit_id: id,
            timestamp: timestamp,
        }
    }

    @Pending >> rollback(err: Error) >> @RolledBack {
        self.conn.rollback();
        RolledBack {
            conn: self.conn,
            error: err,
        }
    }
}
```

#### 10.8.5 Parser Modal Diagram

```cantrip
modal Parser {
    //
    // STATE MACHINE:
    //
    //   Start --[tokenize]?--> Tokenized --[parse]?--> Parsed
    //     |                         |                     |
    //     |                         |                     |
    //     +-------[error]-----------+-----[error]--------+
    //                               |
    //                               v
    //                            Failed
    //

    state Start {
        input: String;
    }

    state Tokenized {
        tokens: Vec<Token>;
    }

    state Parsed {
        ast: AST;
    }

    state Failed {
        error: ParseError;
    }

    @Start >> tokenize() >> (@Tokenized | @Failed)
        needs alloc.heap;
    {
        match lex(self.input) {
            Ok(tokens) -> Ok(Tokenized { tokens }),
            Err(err) -> Ok(Failed { error: err }),
        }
    }

    @Tokenized >> parse() >> (@Parsed | @Failed)
        needs alloc.heap;
    {
        match parse_tokens(self.tokens) {
            Ok(ast) -> Ok(Parsed { ast }),
            Err(err) -> Ok(Failed { error: err }),
        }
    }
}
```

---

### 10.9 Common Modal Patterns

#### 10.9.1 Resource Lifecycle Pattern

**Pattern:** Uninitialized → Ready → InUse → Closed

```cantrip
modal Resource {
    //
    //   Uninitialized --[initialize]--> Ready --[acquire]--> InUse
    //                                     |                     |
    //                                     |                     |
    //                                     +------[release]------+
    //                                     |
    //                                 [close]
    //                                     |
    //                                     v
    //                                  Closed
    //

    state Uninitialized { config: Config; }

    state Ready {
        config: Config;
        handle: Handle;
    }

    state InUse {
        config: Config;
        handle: Handle;
        usage: UsageInfo;
    }

    state Closed { }

    @Uninitialized >> initialize() >> @Ready
        needs alloc.heap;
    {
        let handle = create_handle(self.config)?;
        Ready { config: self.config, handle }
    }

    @Ready >> acquire() >> @InUse
        needs time.read;
    {
        let usage = UsageInfo.start();
        InUse {
            config: self.config,
            handle: self.handle,
            usage,
        }
    }

    @InUse >> release() >> @Ready
        needs time.read;
    {
        self.usage.end();
        Ready {
            config: self.config,
            handle: self.handle,
        }
    }

    (@Ready | @InUse) >> close() >> @Closed
        needs time.read;
    {
        self.handle.close();
        Closed { }
    }
}
```

**Usage:**

```cantrip
function use_resource()
    needs alloc.heap, time.read;
{
    let resource = Resource.new(config);
    let resource = resource.initialize()?;
    let resource = resource.acquire()?;

    // Use resource
    process(resource);

    let resource = resource.release()?;
    resource.close();
}
```

#### 10.9.2 Request-Response Pattern

**Pattern:** Idle → Processing → Responded → Idle (cycle)

```cantrip
modal RequestHandler {
    //
    //   Idle --[receive]--> Processing --[respond]--> Responded
    //    ^                                                 |
    //    |                                                 |
    //    +--------------------[reset]---------------------+
    //

    state Idle { }

    state Processing {
        request: Request;
        started_at: DateTime;
    }

    state Responded {
        request: Request;
        response: Response;
    }

    @Idle >> receive(req: Request) >> @Processing
        needs time.read;
    {
        Processing {
            request: req,
            started_at: DateTime.now(),
        }
    }

    @Processing >> respond(res: Response) >> @Responded {
        Responded {
            request: self.request,
            response: res,
        }
    }

    @Responded >> reset() >> @Idle {
        Idle { }
    }
}
```

#### 10.9.3 Multi-Stage Pipeline Pattern

**Pattern:** Linear progression through stages

```cantrip
modal Pipeline {
    //
    //   Raw --[validate]--> Validated --[transform]--> Transformed
    //                                                       |
    //                                                  [finalize]
    //                                                       |
    //                                                       v
    //                                                   Complete
    //

    state Raw { data: Vec<u8>; }
    state Validated { data: Vec<u8>; schema: Schema; }
    state Transformed { data: ProcessedData; }
    state Complete { result: Output; }

    @Raw >> validate(schema: Schema) >> @Validated
        needs alloc.heap;
    {
        validate_data(self.data, schema)?;
        Validated {
            data: self.data,
            schema,
        }
    }

    @Validated >> transform() >> @Transformed
        needs alloc.heap;
    {
        let processed = process_data(self.data)?;
        Transformed { data: processed }
    }

    @Transformed >> finalize() >> @Complete
        needs alloc.heap;
    {
        let output = generate_output(self.data)?;
        Complete { result: output }
    }
}
```

#### 10.9.4 State Recovery Pattern

**Pattern:** Multiple failure states with recovery paths

```cantrip
modal ResilientService {
    //
    //         [start]
    //            |
    //            v
    //        Running --[fail]--> Failed
    //            ^                  |
    //            |                  |
    //            |             [diagnose]
    //            |                  |
    //            |                  v
    //            |            Diagnosing --[recover]?--> Recovering
    //            |                  |                         |
    //            |                  |                         |
    //            |              [abort]                       |
    //            |                  |                         |
    //            |                  v                         |
    //            |               Stopped                      |
    //            |                                            |
    //            +----------------------[restart]-------------+
    //

    state Running { service: Service; }
    state Failed { service: Service; error: Error; }
    state Diagnosing { service: Service; diagnostics: Diagnostics; }
    state Recovering { service: Service; recovery_plan: RecoveryPlan; }
    state Stopped { }

    @Running >> fail(err: Error) >> @Failed {
        Failed {
            service: self.service,
            error: err,
        }
    }

    @Failed >> diagnose() >> @Diagnosing
        needs alloc.heap;
    {
        let diagnostics = run_diagnostics(self.service, self.error)?;
        Diagnosing {
            service: self.service,
            diagnostics,
        }
    }

    @Diagnosing >> recover() >> @Recovering
        needs alloc.heap;
    {
        let plan = create_recovery_plan(self.diagnostics)?;
        Recovering {
            service: self.service,
            recovery_plan: plan,
        }
    }

    @Recovering >> restart() >> @Running
        needs alloc.heap;
    {
        let recovered_service = execute_recovery(
            self.service,
            self.recovery_plan
        )?;
        Running { service: recovered_service }
    }

    @Diagnosing >> abort() >> @Stopped {
        self.service.shutdown();
        Stopped { }
    }
}
```

---

### 10.10 Modal Instantiation

**Creating initial state:**
```cantrip
modal File {
    state Closed { path: String; }
    state Open { ... }

    // Constructor creates initial state
    function new(path: String): File@Closed {
    File@Closed { path }
    }
}

// Usage
let file = File.new("data.txt");  // Type: File@Closed
```

---

## 11. Modal Formal Semantics

### 11.1 Modal Definition

**Definition 11.1 (Modal Formal Structure):**

```
modal P {
    state S₁ { f₁₁: T₁₁; ...; f₁ₙ: T₁ₙ }
    ...
    state Sₖ { fₖ₁: Tₖ₁; ...; fₖₘ: Tₖₘ }

    @Sᵢ >> mⱼ(params) >> @Sₗ { body }
}
```

**Formal components:**
- States: S = {S₁, ..., Sₖ}
- Transitions: T = {(Sᵢ, m, Sⱼ) | transition exists}
- State data: Fields(S) = {f : T | f declared in S}

### 11.2 State Machine Graph

**Definition 11.2 (State Graph):**

```
G(P) = (V, E) where:
  V = {S₁, ..., Sₖ}
  E = {(Sᵢ, m, Sⱼ) | @Sᵢ >> m >> @Sⱼ in P}
```

**Properties:**
1. **Reachability:** All states must be reachable from initial state
2. **Determinism:** At most one transition per (state, procedure) pair
3. **Totality:** Not required - some procedures may not be available in some states

**Example:**
```
File state graph:
  Closed --[open]--> Open
  Open --[read]--> Open
  Open --[close]--> Closed
```

### 11.3 State Type Rules

**[T-Modal-Type] Modal as type:**
```
modal P { state S₁ {...}; ...; state Sₙ {...}; ... } well-formed
────────────────────────────────────────────────────────────────────
Γ ⊢ modal P : Type
```

**[T-State-Annot] State annotation:**
```
Γ ⊢ e : modal P    Σ ⊢ P has state S    σ ⊨ invariant(S)
─────────────────────────────────────────────────────────────
Γ, Σ ⊢ e : P@S
```

**[T-State-Trans] State transition:**
```
Γ, Σ ⊢ e : P@S₁    
Σ ⊢ @S₁ >> m(args) >> @S₂
needs(m) = ε    requires(m) = P    ensures(m) = Q
σ ⊨ P ∧ invariant(S₁)
───────────────────────────────────────────────────────────
Γ, Σ ⊢ e.m(args) : P@S₂ ! ε
σ' ⊨ Q ∧ invariant(S₂)
```

**[T-State-Procedure] Procedure invocation:**
```
Γ ⊢ self : P@S₁    procedure m : P@S₁ >>ₘ P@S₂ in modal P
──────────────────────────────────────────────────────────
Γ ⊢ self.m(...) : P@S₂
```

**[T-State-Union] State union:**
```
Γ ⊢ e : P@S₁    S₁ ⊆ (S₂ | S₃)
──────────────────────────────
Γ ⊢ e : P@(S₂ | S₃)
```

### 11.4 State Invariants

**Definition 11.3 (State Invariant):**

Each modal state S has an invariant I(S):
```
modal P {
    state S
        must P₁;
        must P₂;
        ...
        must Pₙ;
}

I(S) = P₁ ∧ P₂ ∧ ... ∧ Pₙ
```

**Axiom 11.1 (State Exclusivity):**

For all distinct states S₁, S₂ of modal P:
```
I(S₁) ∧ I(S₂) = false
```

States must be mutually exclusive.

**Axiom 11.2 (State Completeness):**

For all modals P with states S₁, ..., Sₙ:
```
I(S₁) ∨ I(S₂) ∨ ... ∨ I(Sₙ) = true
```

Every valid instance must be in exactly one state.

**Example with invariants:**
```cantrip
modal BankAccount {
    state Active {
        balance: f64;
        account_id: u64;

        must balance >= 0.0;
        must account_id > 0;
    }

    state Frozen {
        balance: f64;
        account_id: u64;
        reason: String;

        must !reason.is_empty();
        must account_id > 0;
    }

    state Closed {
        account_id: u64;
        final_balance: f64;
        closed_at: DateTime;

        must final_balance == 0.0;  // Must be zero to close
    }
}
```

### 11.5 Transition Validity

**Definition 11.4 (Valid Transition):**

A transition @S₁ >> m >> @S₂ is valid if:
1. Procedure m exists in modal
2. Pre-state S₁ exists in modal
3. Post-state S₂ exists in modal
4. Transition preserves invariants

**Formal rule:**
```
[Valid-Transition]
(S₁, m, S₂) ∈ Transitions(P)
σ ⊨ I(S₁)
⟨m(σ), σ⟩ ⇓ ⟨v, σ'⟩
σ' ⊨ I(S₂)
────────────────────────────────
S₁ →ₘ S₂ valid
```

### 11.6 Reachability Analysis

**Algorithm 11.1 (Reachability):**

```
reach(P, S) = {S' | ∃ path in G(P) from initial state to S'}
```

**Compiler verification:**
1. All declared states must be reachable
2. Dead states generate warnings
3. Unreachable transitions generate errors

**Example:**
```cantrip
modal Example {
    state Start { }
    state Middle { }
    state End { }
    state Unreachable { }  // WARNING: Unreachable state

    @Start >> go() >> @Middle
    @Middle >> finish() >> @End
    // No path to Unreachable!
}
```

---

## 12. State Transition Verification

### 12.1 Static Verification

**Verification goal:** Prove that all state transitions preserve invariants.

**Verification condition for transition:**
```
[VC-Transition]
modal P {
    @S₁ >> m(x: T) >> @S₂
        requires P;
        ensures Q;
    { e }
}

Verification Condition:
∀σ. σ ⊨ P ∧ I(S₁) ⟹
  ∃σ', v. ⟨e, σ⟩ ⇓ ⟨v, σ'⟩ ∧ σ' ⊨ Q ∧ I(S₂)
```

**Example:**
```cantrip
modal BankAccount {
    state Active {
        balance: f64;
        must balance >= 0.0;
    }

    @Active >> withdraw(amount: f64) >> @Active
        requires
            amount > 0.0;
            self.balance >= amount;
        ensures
            self.balance == @old(self.balance) - amount;
            self.balance >= 0.0;  // Maintains invariant
    {
        self.balance -= amount;
        self
    }
}
```

**Verification steps:**
1. Check preconditions imply invariant of pre-state
2. Verify implementation satisfies postconditions
3. Check postconditions imply invariant of post-state

### 12.2 Dynamic Verification

**Runtime checks:**
- Preconditions checked at procedure entry
- Postconditions checked at procedure exit
- State invariants checked after transitions

**Generated code:**
```cantrip
// Source
@Active >> withdraw(amount: f64) >> @Active { ... }

// Generated
function withdraw(self: BankAccount@Active, amount: f64) {
    // Check preconditions
    assert!(amount > 0.0, "Precondition: amount > 0.0");
    assert!(self.balance >= amount, "Precondition: balance >= amount");

    let old_balance = self.balance;

    // Execute transition
    self.balance -= amount;

    // Check postconditions
    assert!(self.balance == old_balance - amount, "Postcondition");
    assert!(self.balance >= 0.0, "Invariant: balance >= 0.0");

    self
}
```

### 12.3 Exhaustiveness Checking

**Definition 12.1 (Procedure Exhaustiveness):** For a given state, the compiler checks which procedures are available.

**Example:**
```cantrip
modal File {
    state Closed { ... }
    state Open { ... }

    @Closed >> open() >> @Open
    @Open >> read() >> @Open
    @Open >> close() >> @Closed
}

function process(file: File@Closed) {
    // file.read();  // ERROR: read() not available in Closed state
    let file = file.open();
    file.read();  // OK: read() available in Open state
}
```

**Type checking algorithm:**
```
check_method_call(obj: P@S, procedure: m) = {
    transitions = {(S, m, S') | @S >> m >> @S' in P}
    if transitions.is_empty() {
        error("Procedure {m} not available in state {S}")
    }
    return result_state(transitions)
}
```

### 12.4 State Flow Analysis

**Definition 12.2 (Flow-Sensitive Types):** Modal state types are flow-sensitive - the type changes as the program executes.

**Example:**
```cantrip
function example(): Result<(), Error>
    needs fs.read;
{
    let file = File.new("data.txt");  // Type: File@Closed

    let file = file.open()?;          // Type: File@Open

    let data = file.read(100)?;       // Type: File@Open (still)

    let file = file.close();          // Type: File@Closed

    // file.read(100)?;  // ERROR: Type is File@Closed

        Ok(())
}
```

**Flow analysis:**
```
Point 1: file : File@Closed
Point 2: file : File@Open   (after open())
Point 3: file : File@Open   (read preserves state)
Point 4: file : File@Closed (after close())
```

### 12.5 State Transfer Checking

**Definition 12.3 (Linear State Transfer):** When a transition occurs, the old state is consumed and the new state is produced.

**Type rule:**
```
[Linear-Transition]
Γ ⊢ obj : P@S₁
Γ ⊢ obj.m() : P@S₂
──────────────────────────────
Γ \ {obj : P@S₁} ∪ {obj : P@S₂}
```

**Example:**
```cantrip
let file = File.new("data.txt");  // file : File@Closed

// After this call, file : File@Open
let file = file.open()?;

// Cannot use old binding - consumed by transition
// The name 'file' now refers to the Open state
```

**Rebinding semantics:**
```cantrip
let file = file.open()?;
// Equivalent to:
// let file_closed = file;
// let file = transition(file_closed, Open);
// file_closed is no longer accessible
```

---

# Part IV (Normative): Functions and Expressions

## 13. Functions and Procedures

### 13.1 Function Definition Syntax

**Function signature with optional contract clauses:**

```cantrip
function name(param: Type): ReturnType
    needs effects;        // Optional if pure
    requires contracts;   // Optional if none
    ensures contracts;    // Optional if none
{
    body
}
```

**Components:**
1. **Signature** - `function name(params): ReturnType`
2. **`needs`** - Effect declarations (optional if pure)
3. **`requires`** - Preconditions (optional if none)
4. **`ensures`** - Postconditions (optional if none)
5. **Body** - Implementation in `{ }`

**Design rationale:**
- Consistent with type annotation style (using `:` for return type)
- Effects and contracts are first-class, not comments
- LLM-friendly: consistent structure, easy to parse
- Verification-friendly: contracts machine-checkable

### 13.2 Pure Functions

**Definition 13.1 (Pure Function):** A function with no side effects.

**Syntax:**
```cantrip
function name(params): T {  // Pure, no needs/requires/ensures
    expr
}

// With postconditions:
function name(params): T
    ensures postconditions;
{
    expr
}
```

**Examples:**
```cantrip
function add(a: i32, b: i32): i32 {
    a + b
}

function max(a: i32, b: i32): i32
    ensures
        result >= a;
        result >= b;
        (result == a) || (result == b);
{
    if a > b { a } else { b }
}
```

**Type rule:**
```
[T-Pure-Fun]
Γ, x₁ : T₁, ..., xₙ : Tₙ ⊢ e : U
effects(e) = ∅
─────────────────────────────────────
Γ ⊢ function f(x₁: T₁, ..., xₙ: Tₙ): U {
    e
} : T₁ -> ... -> Tₙ -> U
```

### 13.3 Functions with Effects

**Single effect:**
```cantrip
function print_message(msg: String)
    needs io.write;
{
    std.io.println(msg);
}
```

**Multiple effects:**
```cantrip
function save_config(path: String, data: Config): Result<(), Error>
    needs fs.write, alloc.heap;
    requires !path.is_empty();
{
    std.fs.write(path, data.serialize())
}
```

**Type rule:**
```
[T-Effect-Fun]
Γ, x : T ⊢ e : U ! ε
ε ⊆ declared_effects
─────────────────────────────────────
Γ ⊢ function f(x: T): U
    needs ε;
{
    e
} : fn(T): U ! ε
```

**Critical:** Effects are NEVER inferred. All must be declared explicitly.

### 13.4 Functions with Contracts

**Simple contract:**
```cantrip
function divide(a: f64, b: f64): f64
    requires b != 0.0;
{
    a / b
}
```

**Multiple preconditions:**
```cantrip
function withdraw(account: mut Account, amount: f64)
    requires
        amount > 0.0;
        account.balance >= amount;
{
    account.balance -= amount;
}
```

**Postconditions:**
```cantrip
function sqrt(x: f64): f64
    requires x >= 0.0;
    ensures
        result >= 0.0;
        result * result ≈ x;
{
    x.sqrt()
}
```

**Type rule:**
```
[T-Contract-Fun]
Γ ⊢ P : Assertion    Γ, x : T ⊢ e : U    Γ ⊢ Q : Assertion
{P} e {Q} provable or runtime-checked
──────────────────────────────────────────────────────────
Γ ⊢ function f(x: T): U
    requires P;
    ensures Q;
{
    e
} : T -> U with {P} -> {Q}
```

### 13.5 Parameters and Permissions

#### 13.5.1 Immutable Reference (Default)

**Default parameters are immutable references** (like C++ `const&`):

```cantrip
function process(data: String)
    requires !data.is_empty();
{
    std.io.println(data);  // Can read
    // data.clear();  // ERROR: Cannot mutate
}
```

**Multiple calls allowed (no borrow checking):**
```cantrip
function example()
    needs alloc.heap;
{
    let data = String.from("hello");

        // Can pass many times - no borrow checking
        process(data);
        process(data);
        process(data);
}
```

**Type rule:**
```
[T-Param-Ref]
Γ ⊢ data : String
──────────────────────────────────
function f(data: String) accepts data
```

#### 13.5.2 Owned Parameters

**Take ownership with `own`** (like C++ `unique_ptr`):

```cantrip
function consume(own data: String) {
    // data is consumed
    // Automatically freed at end of scope
}
```

**Must use `move` to transfer:**
```cantrip
function example()
    needs alloc.heap;
{
    let data = String.from("hello");
        consume(move data);
        // data no longer accessible
        // consume(move data);  // ERROR: value moved
}
```

**Type rule:**
```
[T-Param-Own]
Γ ⊢ e : own T
──────────────────────────────────
function f(own x: T) accepts move e
Γ' = Γ \ {e}  (e consumed)
```

#### 13.5.3 Mutable Parameters

**Exclusive mutable access with `mut`** (like C++ `&`):

```cantrip
function increment(counter: mut Counter) {
    counter.value += 1;
}
```

**Multiple mutable calls allowed (programmer's responsibility):**
```cantrip
function example()
    needs alloc.heap;
{
    var data = Data.new();

        // NO borrow checking - all allowed simultaneously
        modify1(mut data);
        modify2(mut data);
        read(data);
        modify3(mut data);

        // Programmer ensures this is safe!
}
```

**Type rule:**
```
[T-Param-Mut]
Γ ⊢ e : T    mutable(e) = true
──────────────────────────────────
function f(e: mut T) accepts mut e
```

**Key difference from Rust:** No borrow checker enforcing aliasing rules. Multiple `mut` references can coexist - programmer ensures correct usage.

### 13.6 Return Values

**Explicit return:**
```cantrip
function max(a: i32, b: i32): i32 {
    if a > b {
            a
        } else {
            b
        }
}
```

**Early return:**
```cantrip
function find(arr: [i32], target: i32): Option<usize> {
    for i in 0..arr.length() {
        if arr[i] == target {
            return Some(i);
        }
    }
    None
}
```

**Void return:**
```cantrip
function log_message(msg: String)  // Void/unit return
    needs io.write;
{
    std.io.println(msg);
}
```

**Type rule:**
```
[T-Return]
Γ ⊢ e : T
function f returns<U>
T = U
─────────────────────
return e : !
```

### 13.7 Procedure Syntax

**Procedures are functions with `self` parameter:**

```cantrip
impl Counter {
    function new(): own Counter
        needs alloc.heap;
    {
        own Counter { value: 0 }
    }

    function increment(self: mut Counter) {
        self.value += 1;
    }

    function get(self: Counter): i32 {
        self.value
    }
}
```

**Procedure call desugaring:**
```
obj.procedure(args) ≡ Type::procedure(obj, args)
```

**Usage:**
```cantrip
var counter = Counter.new();
counter.increment();
let value = counter.get();
```

---

## 14. Expressions and Operators

### 14.1 Expression Language

**Definition 14.1 (Expression):** An expression is a syntactic construct that evaluates to a value.

**Type rule for expressions:**
```
Γ ⊢ e : T
```

**Evaluation semantics:**
```
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
```

### 14.2 Arithmetic Operators

**Binary operators:**
```
+   // Addition
-   // Subtraction
*   // Multiplication
/   // Division
%   // Remainder (modulo)
```

**Type rules:**
```
[T-Add]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Numeric
─────────────────────────────────────────
Γ ⊢ e₁ + e₂ : T

[T-Div]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Numeric
─────────────────────────────────────────
Γ ⊢ e₁ / e₂ : T
```

**Operational semantics:**
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨n₂, σ₂⟩
────────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨n₁ + n₂, σ₂⟩
```

**Examples:**
```cantrip
let sum = a + b;
let product = x * y;
let remainder = n % 10;
```

### 14.3 Comparison Operators

**Operators:**
```
==  // Equal
!=  // Not equal
<   // Less than
<=  // Less than or equal
>   // Greater than
>=  // Greater than or equal
```

**Type rule:**
```
[T-Eq]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Eq
───────────────────────────────────
Γ ⊢ e₁ == e₂ : bool

[T-Ord]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Ord
───────────────────────────────────
Γ ⊢ e₁ < e₂ : bool
```

**Operational semantics:**
```
[E-Eq]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩
────────────────────────────────────────
⟨e₁ == e₂, σ⟩ ⇓ ⟨v₁ = v₂, σ₂⟩
```

### 14.4 Logical Operators

**Operators:**
```
&&  // Logical AND (short-circuit)
||  // Logical OR (short-circuit)
!   // Logical NOT
```

**Type rules:**
```
[T-And]
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : bool
──────────────────────────────
Γ ⊢ e₁ && e₂ : bool

[T-Not]
Γ ⊢ e : bool
────────────
Γ ⊢ !e : bool
```

**Operational semantics (short-circuit):**
```
[E-And-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v, σ₂⟩
────────────────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨v, σ₂⟩

[E-And-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩
────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨false, σ₁⟩
```

**Example:**
```cantrip
let valid = x > 0 && x < 100;
let has_value = opt.is_some() || use_default;
let inverted = !flag;
```

### 14.5 Bitwise Operators

**Operators:**
```
&   // Bitwise AND
|   // Bitwise OR
^   // Bitwise XOR
<<  // Left shift
>>  // Right shift (arithmetic)
```

**Type rules:**
```
[T-BitAnd]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Integer
─────────────────────────────────────────
Γ ⊢ e₁ & e₂ : T

[T-Shift]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : u32    T : Integer
─────────────────────────────────────────
Γ ⊢ e₁ << e₂ : T
```

**Examples:**
```cantrip
let masked = value & 0xFF;
let combined = flags | new_flags;
let shifted = n << 2;  // Multiply by 4
```

### 14.6 Assignment Operators

**Operators:**
```
=   // Assignment
+=  // Add and assign
-=  // Subtract and assign
*=  // Multiply and assign
/=  // Divide and assign
%=  // Modulo and assign
```

**Type rules:**
```
[T-Assign]
Γ ⊢ x : T    Γ ⊢ e : T    mutable(x) = true
────────────────────────────────────────────
Γ ⊢ x = e : ()

[T-Add-Assign]
Γ ⊢ x : T    Γ ⊢ e : T    mutable(x) = true    T : Numeric
──────────────────────────────────────────────────────────
Γ ⊢ x += e : ()
```

**Desugaring:**
```
x += e  ≡  x = x + e
x -= e  ≡  x = x - e
x *= e  ≡  x = x * e
```

### 14.7 Range Operators

**Operators:**
```
..   // Exclusive range: a..b → [a, b)
..=  // Inclusive range: a..=b → [a, b]
```

**Type rules:**
```
[T-Range]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Integer
─────────────────────────────────────────
Γ ⊢ e₁..e₂ : Range<T>

[T-Range-Inclusive]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T    T : Integer
─────────────────────────────────────────
Γ ⊢ e₁..=e₂ : RangeInclusive<T>
```

**Examples:**
```cantrip
for i in 0..10 {  // 0, 1, 2, ..., 9
    println(i);
}

for i in 0..=10 {  // 0, 1, 2, ..., 10
    println(i);
}

let slice = arr[2..5];  // Elements at indices 2, 3, 4
```

### 14.8 Pipeline Operator

**The pipeline operator `=>` chains function calls:**

```cantrip
function process(data: Vec<i32>): i32 {
    data
            => filter(|x| x > 0)
            => map(|x| x * 2)
            => sum
}
```

**Desugaring:**
```
e₁ => e₂ => e₃  ≡  e₃(e₂(e₁))
```

**Type rule:**
```
[T-Pipeline]
Γ ⊢ e₁ : T    Γ ⊢ e₂ : T -> U
──────────────────────────────
Γ ⊢ e₁ => e₂ : U
```

**Operational semantics:**
```
[E-Pipeline]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ⟨e₂(v₁), σ₁⟩ ⇓ ⟨v₂, σ₂⟩
───────────────────────────────────────────────
⟨e₁ => e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩
```

**With effects:**
```cantrip
function load_and_process(path: str): Result<Data, Error>
    needs fs.read, alloc.heap;
{
    read_file(path)
        => parse_json
        => validate
        => transform
}
```

### 14.9 Operator Precedence

**From highest to lowest:**

| Precedence | Operators | Associativity |
|------------|-----------|---------------|
| 1 (Highest) | `!`, `-` (unary), `*` (deref) | Right |
| 2 | `*`, `/`, `%` | Left |
| 3 | `+`, `-` | Left |
| 4 | `<<`, `>>` | Left |
| 5 | `&` | Left |
| 6 | `^` | Left |
| 7 | `|` | Left |
| 8 | `==`, `!=`, `<`, `<=`, `>`, `>=` | Left |
| 9 | `&&` | Left |
| 10 | `||` | Left |
| 11 | `=>` | Left |
| 12 (Lowest) | `=`, `+=`, `-=`, etc. | Right |

**Examples:**
```cantrip
a + b * c       ≡  a + (b * c)
a && b || c     ≡  (a && b) || c
x = y = z       ≡  x = (y = z)
a => f => g     ≡  g(f(a))
```

---


### 14.10 Typed Holes (Normative)

**Goal:** Improve developer flow and LLM-assisted coding by allowing *incomplete* expressions that type-check
and produce precise diagnostics and fix-its, while preserving safety and AOT compilation.

**Syntax (expressions):**
```ebnf
ExprHole      ::= "?" (Ident)? ( ":" Type )?
UntypedHole   ::= "???"
```
- `?` introduces a *typed hole*. The optional `: Type` annotation constrains its type.
  Without an explicit `Type`, the hole participates in normal type inference.
- `???` is a hard untyped hole: it has type `!` and cannot be used except in positions
  that are already diverging.

**Typing:**
```
[T-Hole]
Γ ⊢ ?x : T  (fresh metavariable α; if annotation present, α = T)
Emits obligation: "hole(?x) : α must be defined"
```

**Elaboration and build modes:**
- **Verify/static:** `E9501` — *Unfilled typed hole*. Compilation fails. Fix-it includes a skeleton snippet.
- **Verify/runtime (default dev):** A typed hole elaborates to a trap of type `T` that panics at runtime
  (`needs panic`) with a precise message and source span. `E9502` (warning) is emitted.
- **Verify/none:** Typed holes are denied (`E9501`).

**Effects:** Filling a hole MUST NOT widen declared effects. A hole in a function with `needs ε`
is considered to have effects ⊆ `ε` until filled; otherwise `E9503` is raised.

**Examples:**
```cantrip
function parse(line: String): Result<Item, Error>
    needs alloc.heap;
{
    let name = ?n: String;          // typed hole requiring a String
    let qty  = ?;                   // hole with inferred type
    ???                              // hard hole: diverges here
}
```

**Machine-Readable Output:** See §55 for the `typedHole` field in diagnostics.


## 15. Control Flow

### 15.1 If Expressions

**Syntax:**
```cantrip
if condition {
    // then branch
}

if condition {
    // then branch
} else {
    // else branch
}
```

**Type rule:**
```
[T-If]
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : T    Γ ⊢ e₃ : T
────────────────────────────────────────────
Γ ⊢ if e₁ then e₂ else e₃ : T
```

**Operational semantics:**
```
[E-If-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v, σ₂⟩
────────────────────────────────────────────
⟨if e₁ then e₂ else e₃, σ⟩ ⇓ ⟨v, σ₂⟩

[E-If-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩    ⟨e₃, σ₁⟩ ⇓ ⟨v, σ₂⟩
────────────────────────────────────────────
⟨if e₁ then e₂ else e₃, σ⟩ ⇓ ⟨v, σ₂⟩
```

**If as expression:**
```cantrip
let max = if a > b { a } else { b };
let sign = if x > 0 { 1 } else if x < 0 { -1 } else { 0 };
```

**Type requirement:** Both branches must have same type (or one is `!`)

### 15.2 While Loops

**Syntax:**
```cantrip
while condition {
    // loop body
}
```

**Type rule:**
```
[T-While]
Γ ⊢ e₁ : bool    Γ ⊢ e₂ : ()
──────────────────────────────
Γ ⊢ while e₁ do e₂ : ()
```

**With invariant:**
```cantrip
var i = 0;
while i < n {
    invariant:
        i >= 0;
        i <= n;

    i += 1;
}
```

**Loop invariant verification:**
```
[VC-While]
{I ∧ condition} body {I}
────────────────────────────────────────
{I} while condition { invariant: I; body } {I ∧ ¬condition}
```

**Operational semantics:**
```
[E-While-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨(), σ₂⟩
⟨while e₁ do e₂, σ₂⟩ ⇓ ⟨(), σ₃⟩
────────────────────────────────────────────
⟨while e₁ do e₂, σ⟩ ⇓ ⟨(), σ₃⟩

[E-While-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩
─────────────────────────────
⟨while e₁ do e₂, σ⟩ ⇓ ⟨(), σ₁⟩
```

### 15.3 For Loops

**Range iteration:**
```cantrip
for i in 0..10 {
    std.io.println(i);
}
```

**Desugaring:**
```cantrip
// for i in range { body }
// Desugars to:
{
    var iter = range.into_iter();
    loop {
        match iter.next() {
            Some(i) -> body,
            None -> break,
        }
    }
}
```

**Collection iteration:**
```cantrip
for item in collection {
    process(item);
}
```

**With index:**
```cantrip
for i, item in collection.enumerate() {
    std.io.println("Item {i}: {item}");
}
```

**Type rule:**
```
[T-For]
Γ ⊢ e₁ : T    T : Iterator<Item = U>
Γ, x : U ⊢ e₂ : ()
─────────────────────────────────────
Γ ⊢ for x in e₁ { e₂ } : ()
```

### 15.4 Loop Control

**Infinite loop:**
```cantrip
loop {
    // infinite loop
    if should_exit {
        break;
    }
}
```

**Break with value:**
```cantrip
let result = loop {
    if condition {
        break value;
    }
};
```

**Continue:**
```cantrip
for i in 0..10 {
    if i % 2 == 0 {
        continue;  // Skip even numbers
    }
    println(i);
}
```

**Type rules:**
```
[T-Loop]
Γ ⊢ e : T
─────────────────────
Γ ⊢ loop { e } : !

[T-Break]
Γ ⊢ e : T
in_loop with result type T
─────────────────────────────
Γ ⊢ break e : !

[T-Continue]
in_loop
────────────────
Γ ⊢ continue : !
```

---

## 16. Higher-Order Functions and Closures

### 16.1 Function Types (fn) and Procedure Types (proc)

**Definition 16.1 (Function Type):** The type `fn(T): U` represents functions from `T` to `U`.

**Type constructor:**
```
->  (function type constructor)
```

**Examples:**
```cantrip
type Handler = fn(Request): Response;
type BinaryOp = fn(i32, i32): i32;
type Transform = fn(String): Result<Data, Error>;
```

**Currying:**
```
i32 -> i32 -> i32  ≡  i32 -> (i32 -> i32)
```

### 16.2 Higher-Order Functions

**Definition 16.2 (Higher-Order Function):** A function that takes functions as parameters or returns functions.

**Examples:**
```cantrip
function apply_twice<T>(value: T, f: T -> T): T {
    f(f(value))
}

function filter<T>(items: [T], pred: T -> bool): Vec<T>
    needs alloc.heap;
{
    var result = Vec.new();
    for item in items {
        if pred(item) {
            result.push(item);
        }
    }
    result
}

function compose<A, B, C>(f: fn(A): B, g: fn(B): C): fn(A): C {
    |x: A| { g(f(x)) }
}
```

**Type rule:**
```
[T-Higher-Order]
Γ ⊢ f : T -> U    Γ ⊢ g : U -> V
──────────────────────────────────
compose(f, g) : T -> V
```

### 16.3 Closures

**Definition 16.3 (Closure):** An anonymous function that captures its environment.

**Syntax:**
```cantrip
|param| { body }
|param: Type| -> RetType { body }
move |param| { body }  // Move captured variables
```

**Examples:**
```cantrip
let add_n = |x| { x + n };  // Captures n

// Explicit types
let multiply: fn(i32, i32) -> i32 = |a, b| { a * b };

// Move semantics
let closure = move |x| { x + captured_value };
```

**Type rule:**
```
[T-Closure]
Γ, x : T ⊢ e : U    free_vars(e) ⊆ dom(Γ)
───────────────────────────────────────────
Γ ⊢ |x: T| { e } : T -> U
```

**Capture semantics:**
```
[Capture-Ref]
x : T ∈ Γ
────────────────────────
closure captures x by reference

[Capture-Move]
x : T ∈ Γ
────────────────────────
move closure captures x by value
Γ' = Γ \ {x}
```

**Example with state:**
```cantrip
function make_counter(): fn() -> i32
    needs alloc.heap;
{
    var count = 0;
    move || {
        count += 1;
        count
    }
}

let counter = make_counter();
println(counter());  // 1
println(counter());  // 2
```

### 16.4 Function Traits

**Three closure traits:**

```cantrip
trait Fn<Args> {
    function call(self: Self, args: Args): Output;
}

trait FnMut<Args> {
    function call(self: mut Self, args: Args): Output;
}

trait FnOnce<Args> {
    function call(self: own Self, args: Args): Output;
}
```

**Hierarchy:**
```
Fn <: FnMut <: FnOnce
```

**Usage:**
```cantrip
function apply<F>(f: F, x: i32): i32
    where F: Fn(i32): i32
{
    f.call(x)
}
```

### 16.5 Effect Propagation

**Use `effects(F)` to propagate callback effects:**

```cantrip
function map<T, U, F>(items: [T], mapper: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), alloc.heap;
    requires items.length() > 0;
{
    var result = Vec.with_capacity(items.length());
    for item in items {
        result.push(mapper(item));
    }
    result
}
```

**Type rule:**
```
[T-Effect-Propagate]
Γ ⊢ f : fn(T): U ! ε₁
Γ ⊢ map : (T, T -> U) -> Vec<U> ! (ε₂, effects(f))
──────────────────────────────────────────────────
map(items, f) ! (ε₂ ∪ ε₁)
```

**Constraining callbacks:**
```cantrip
function parallel_map<T, U, F>(items: [T], mapper: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), !time.read, !random, alloc.heap;
    requires items.length() > 0;
{
    std.parallel.map_n(items, 8)
}
```

# Part V (Normative): Contract System

## 17. Contracts and Specifications

### 17.1 Contract Overview

**Definition 17.1 (Contract):** A contract is an executable specification consisting of preconditions, postconditions, and invariants.

**Design goals:**
- Machine-verifiable specifications
- Contracts as first-class citizens
- Clear separation from implementation
- Support both static and dynamic verification

**Contract components:**
1. **Preconditions** - What must be true when function is called
2. **Postconditions** - What is guaranteed when function returns
3. **Invariants** - What is always true for a type

### 17.2 Preconditions

**Syntax:**
```cantrip
function name(params): Type
    needs effects;       // Optional
    requires
        assertion₁;
        assertion₂;
        ...;
```

**Pure preconditions (no effects):**
```cantrip
function divide(a: f64, b: f64): f64
    requires
        b != 0.0;
        a.is_finite();
        b.is_finite();
    ensures result.is_finite();
{
    a / b
}
```

**Preconditions with effects:**
```cantrip
function read_file(path: String): Result<String, Error>
    needs fs.read, alloc.heap;
    requires
        !path.is_empty();
        path.len() < 4096;
{
    std.fs.read_to_string(path)
}
```

**Type rule:**
```
[T-Requires]
Γ ⊢ P : Assertion    Γ ⊢ e : T ! ε
──────────────────────────────────────
Γ ⊢ function f(): T
    needs ε;
    requires P;
{
    e
} : T ! ε with precondition P
```

### 17.3 Postconditions

**Syntax:**
```cantrip
function name(params): Type
    ensures
        assertion₁;
        assertion₂;
        ...;
```

**Postconditions can reference:**
- `result`: The returned value
- `@old(expr)`: Value of expression on entry
- Parameters: Original parameter values

**Example:**
```cantrip
function push(self: mut Vec<T>, item: T)
    needs alloc.heap;
    requires self.length() < self.capacity();
    ensures
        self.length() == @old(self.length()) + 1;
        self[self.length() - 1] == item;
{
    self.data[self.length()] = item;
    self.length += 1;
}
```

**Complex postconditions:**
```cantrip
function sqrt(x: f64): f64
    requires x >= 0.0;
    ensures
        result >= 0.0;
        result * result ≈ x;  // Within floating-point precision
        abs(result * result - x) < 1e-10;
{
    x.sqrt()
}
```

**Type rule:**
```
[T-Ensures]
Γ ⊢ e : T    Γ, result : T ⊢ Q : Assertion
────────────────────────────────────────────
Γ ⊢ function f(): T
    ensures Q;
{
    e
} : T with postcondition Q
```

### 17.4 Contract Messages

**Custom error messages for contract violations:**

```cantrip
function withdraw(self: mut Account, amount: f64)
    requires
        amount > 0.0,
            "Withdrawal amount must be positive";
        self.balance >= amount,
            "Insufficient funds: need {amount}, have {self.balance}";
    ensures self.balance == @old(self.balance) - amount;
{
    self.balance -= amount;
}
```

**Syntax:**
```
assertion, "error message with {interpolation}"
```

**Runtime behavior:**
```cantrip
// If precondition fails:
panic("Contract violation in withdraw:
       Insufficient funds: need 100.0, have 50.0
       at src/account.arc:42")
```

### 17.5 Empty Contracts

**When there are no constraints:**

```cantrip
function add(a: i32, b: i32): i32 {
    a + b
    // No needs, requires, or ensures clauses - all optional
}

// Pure functions simply omit contract clauses
function identity(x: i32): i32 {
    x
}
```

### 17.6 Old-Value References

**Definition 17.2 (@old):** The `@old(expr)` construct captures the value of an expression at function entry.

**Syntax:**
```
@old(expression)
```

**Usage:**
```cantrip
function transfer(from: mut Account, to: mut Account, amount: f64)
    requires
        amount > 0.0;
        from.balance >= amount;
    ensures
        from.balance == @old(from.balance) - amount;
        to.balance == @old(to.balance) + amount;
        @old(from.balance) + @old(to.balance) ==
            from.balance + to.balance;  // Conservation
{
    from.balance -= amount;
    to.balance += amount;
}
```

**Formal semantics:**
```
[Old-Value]
⟦@old(e)⟧σ = ⟦e⟧σ₀  where σ₀ is initial store
```

**Implementation:**
```
// Compiler generates:
function transfer(from: mut Account, to: mut Account, amount: f64) {
    // Capture old values
    let from_balance_old = from.balance;
    let to_balance_old = to.balance;

    // Execute
    from.balance -= amount;
    to.balance += amount;

    // Check postconditions
    assert!(from.balance == from_balance_old - amount);
    assert!(to.balance == to_balance_old + amount);
}
```

### 17.7 Result References

**The `result` identifier in postconditions refers to the return value:**

```cantrip
function factorial(n: u32): u64
    requires n <= 20;  // Prevent overflow
    ensures
        result >= 1;
        n == 0 implies result == 1;
        n > 0 implies result == n * factorial(n - 1);
{
    if n == 0 { 1 } else { n as u64 * factorial(n - 1) }
}
```

**For tuples:**
```cantrip
function divmod(a: i32, b: i32): (i32, i32)
    requires b != 0;
    ensures
        result.0 * b + result.1 == a;
        result.1 >= 0;
        result.1 < abs(b);
{
    (a / b, a % b)
}
```

### 17.9 Common Contract Patterns

#### 17.9.1 Bounds Checking Pattern

**Pattern:** Ensure indices are within valid ranges

```cantrip
function safe_index<T>(arr: [T], index: usize): Option<T>
    ensures
        result.is_some() == (index < arr.length());
        result.is_some() implies result.unwrap() == arr[index];
{
    if index < arr.length() {
        Some(arr[index])
    } else {
        None
    }
}

function get_or_default<T>(arr: [T], index: usize, default: T): T
    ensures
        (index < arr.length()) implies result == arr[index];
        (index >= arr.length()) implies result == default;
{
    if index < arr.length() {
        arr[index]
    } else {
        default
    }
}
```

#### 17.9.2 Collection Invariant Pattern

**Pattern:** Maintain collection properties across operations

```cantrip
function append<T>(left: Vec<T>, right: Vec<T>): Vec<T>
    ensures
        result.length() == left.length() + right.length();
        forall(i in 0..left.length()) {
            result[i] == left[i]
        };
        forall(i in 0..right.length()) {
            result[left.length() + i] == right[i]
        };
    needs alloc.heap;
{
    let mut combined = left.clone();
    for item in right {
        combined.push(item);
    }
    combined
}

function filter<T, F>(items: Vec<T>, predicate: F): Vec<T>
    where F: Fn(T): bool
    ensures
        result.length() <= items.length();
        forall(x in result) { predicate(x) };
    needs effects(F), alloc.heap;
{
    var filtered = Vec.new();
    for item in items {
        if predicate(item) {
            filtered.push(item);
        }
    }
    filtered
}
```

#### 17.9.3 State Consistency Pattern

**Pattern:** Ensure related fields stay synchronized

```cantrip
record CachedValue<T> {
    value: T;
    cached: Option<T>;
    dirty: bool;

    invariant:
        !dirty implies cached.is_some();
        cached.is_some() implies cached.unwrap() == value;
}

impl<T> CachedValue<T> where T: Clone {
    function update(self: mut CachedValue<T>, new_value: T)
        ensures
            self.value == new_value;
            self.dirty == true;
            self.cached.is_none();
    {
        self.value = new_value;
        self.dirty = true;
        self.cached = None;
    }

    function get_cached(self: mut CachedValue<T>): T
        ensures
            !self.dirty;
            self.cached.is_some();
            result == self.value;
        needs alloc.heap;
    {
        if self.dirty {
            self.cached = Some(self.value.clone());
            self.dirty = false;
        }
        self.cached.unwrap()
    }
}
```

#### 17.9.4 Resource Conservation Pattern

**Pattern:** Prove resources are neither created nor destroyed improperly

```cantrip
function transfer_funds(
    from: mut Account,
    to: mut Account,
    amount: f64
)
    requires
        amount > 0.0,
            "Transfer amount must be positive";
        from.balance >= amount,
            "Insufficient funds: need {amount}, have {from.balance}";
    ensures
        from.balance == @old(from.balance) - amount;
        to.balance == @old(to.balance) + amount;
        // Conservation law: total unchanged
        @old(from.balance + to.balance) == (from.balance + to.balance);
{
    from.balance -= amount;
    to.balance += amount;
}

function swap<T>(a: mut T, b: mut T)
    ensures
        *a == @old(*b);
        *b == @old(*a);
{
    let temp = *a;
    *a = *b;
    *b = temp;
}
```

#### 17.9.5 Ordering Preservation Pattern

**Pattern:** Maintain order relationships through transformations

```cantrip
function merge_sorted(left: Vec<i32>, right: Vec<i32>): Vec<i32>
    requires
        is_sorted(left);
        is_sorted(right);
    ensures
        is_sorted(result);
        result.length() == left.length() + right.length();
        forall(x in left) { result.contains(x) };
        forall(x in right) { result.contains(x) };
    needs alloc.heap;
{
    var merged = Vec.new();
    var i = 0;
    var j = 0;

    while i < left.length() && j < right.length() {
        if left[i] <= right[j] {
            merged.push(left[i]);
            i += 1;
        } else {
            merged.push(right[j]);
            j += 1;
        }
    }

    while i < left.length() {
        merged.push(left[i]);
        i += 1;
    }

    while j < right.length() {
        merged.push(right[j]);
        j += 1;
    }

    merged
}

function reverse<T>(arr: Vec<T>): Vec<T>
    ensures
        result.length() == arr.length();
        forall(i in 0..arr.length()) {
            result[i] == arr[arr.length() - 1 - i]
        };
    needs alloc.heap;
{
    var reversed = Vec.new();
    for i in (0..arr.length()).rev() {
        reversed.push(arr[i]);
    }
    reversed
}
```

#### 17.9.6 Error Handling Contract Pattern

**Pattern:** Specify when functions can fail and what they guarantee on success

```cantrip
function parse_int(s: String): Result<i32, ParseError>
    requires !s.is_empty();
    ensures
        result.is_ok() implies (
            result.unwrap() >= i32::MIN &&
            result.unwrap() <= i32::MAX
        );
        result.is_err() implies (
            !is_valid_integer(s)
        );
{
    match s.parse() {
        Ok(n) -> Ok(n),
        Err(_) -> Err(ParseError.InvalidFormat),
    }
}

function divide_checked(a: i32, b: i32): Result<i32, ArithmeticError>
    ensures
        result.is_ok() implies (
            b != 0 &&
            result.unwrap() == a / b
        );
        result.is_err() implies (
            b == 0
        );
{
    if b == 0 {
        Err(ArithmeticError.DivisionByZero)
    } else {
        Ok(a / b)
    }
}
```

---

## 18. Contract Formal Logic

### 18.1 Assertion Language

**Definition 18.1 (Assertion):** An assertion is a boolean-valued predicate over program state.

**Syntax:**
```
P, Q ::= none                               (trivial)
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
```

**Satisfaction relation:**
```
σ ⊨ P  (store σ satisfies assertion P)
```

**Satisfaction rules:**
```
[Sat-Eq]
⟦e₁⟧σ = ⟦e₂⟧σ
──────────────
σ ⊨ e₁ = e₂

[Sat-And]
σ ⊨ P    σ ⊨ Q
──────────────
σ ⊨ P ∧ Q

[Sat-Forall]
∀v. σ[x ↦ v] ⊨ P
─────────────────
σ ⊨ ∀x. P
```

### 18.2 Hoare Logic

**Definition 18.2 (Hoare Triple):** A Hoare triple {P} e {Q} states that if P holds before executing e, then Q holds after (if e terminates).

**Axioms and rules:**

**[H-Skip]**
```
──────────────
{P} skip {P}
```

**[H-Assign]**
```
──────────────────────────
{P[x ↦ e]} x = e {P}
```

**[H-Seq]**
```
{P} e₁ {Q}    {Q} e₂ {R}
─────────────────────────
{P} e₁ ; e₂ {R}
```

**[H-If]**
```
{P ∧ b} e₁ {Q}    {P ∧ ¬b} e₂ {Q}
──────────────────────────────────
{P} if b then e₁ else e₂ {Q}
```

**[H-While]**
```
{I ∧ b} e {I}
──────────────────────────────────
{I} while b do e {I ∧ ¬b}
```

**[H-Consequence]**
```
P' ⇒ P    {P} e {Q}    Q ⇒ Q'
─────────────────────────────
{P'} e {Q'}
```

### 18.3 Weakest Precondition

**Definition 18.3 (Weakest Precondition):** For command c and postcondition Q, the weakest precondition wp(c, Q) is the weakest assertion P such that {P} c {Q}.

**Weakest precondition calculus:**

```
wp(skip, Q) = Q
wp(x = e, Q) = Q[x ↦ e]
wp(c₁ ; c₂, Q) = wp(c₁, wp(c₂, Q))
wp(if b then c₁ else c₂, Q) = (b ⇒ wp(c₁, Q)) ∧ (¬b ⇒ wp(c₂, Q))
wp(while b do c, Q) = I where I is loop invariant
```

**Example:**
```cantrip
// Command: x = x + 1
// Postcondition: x > 0
// wp(x = x + 1, x > 0) = (x + 1 > 0) = (x > -1)

function increment(x: i32): i32
    requires x > -1;  // Weakest precondition
    ensures result > 0;
{
    x + 1
}
```

### 18.4 Verification Conditions

**Definition 18.4 (Verification Condition):** A verification condition (VC) is a logical formula that, if valid, proves correctness of a program with respect to its contracts.

**For function:**
```cantrip
function f(x: T): U
    requires P;
    ensures Q;
{
    e
}
```

**Verification condition:**
```
∀σ, x. σ ⊨ P ⟹ ∃σ', v. ⟨e, σ⟩ ⇓ ⟨v, σ'⟩ ∧ σ' ⊨ Q[result ↦ v]
```

**For state transition:**
```cantrip
modal P {
    @S₁ >> m(x: T) >> @S₂
        requires P;
        ensures Q;
    { e }
}
```

**Verification condition:**
```
∀σ. σ ⊨ P ∧ I(S₁) ⟹
  ∃σ', v. ⟨e, σ⟩ ⇓ ⟨v, σ'⟩ ∧ σ' ⊨ Q ∧ I(S₂)
```

### 18.5 Contract Soundness

**Theorem 18.1 (Contract Soundness):**

If a function f satisfies its contracts and f is called with arguments satisfying the precondition, then f's result satisfies the postcondition (if f terminates).

**Formal statement:**
```
function f(x: T): U
    requires P;
    ensures Q;
{
    e
}

∀v : T. ⊨ P[x ↦ v] ⟹
  ∃w : U. f(v) ⇓ w ∧ ⊨ Q[x ↦ v, result ↦ w]
```

**Proof obligation:** Must be verified for all functions with contracts.

---

## 19. Invariants

### 19.1 Record Invariants

**Definition 19.1 (Record Invariant):** An invariant is a predicate that must hold for all instances of a type at all observable points.

**Syntax:**
```cantrip
record Name {
    fields;

    invariant:
        assertion₁;
        assertion₂;
        ...
}
```

**Example:**
```cantrip
record BankAccount {
    balance: f64;

    invariant:
        balance >= 0.0;
}
```

**Enforcement points:**
1. After construction
2. Before and after public procedure calls
3. Not checked during private procedure execution

**Type rule:**
```
[T-Record-Inv]
record S { fields } with invariant I
Γ ⊢ e : S
─────────────────────────────
σ ⊨ I[self ↦ e]
```

### 19.2 Invariant Preservation

**Procedures must preserve invariants:**

```cantrip
impl BankAccount {
    function new(initial: f64): own BankAccount
        requires initial >= 0.0;
        ensures
            result.balance == initial;
            // Implicitly: result.balance >= 0.0 (invariant)
    {
        own BankAccount { balance: initial }
    }

    function withdraw(self: mut BankAccount, amount: f64)
        requires
            amount > 0.0;
            self.balance >= amount;
        ensures
            self.balance == @old(self.balance) - amount;
            // Implicitly checked: self.balance >= 0.0 (invariant)
    {
        self.balance -= amount;
    }
}
```

**Verification condition:**
```
[VC-Inv-Procedure]
record S with invariant I
procedure m(self: mut S, x: T)
    requires P;
    ensures Q;
{
    e
}

VC: ∀σ. σ ⊨ I ∧ P ⟹
      ∃σ'. ⟨e, σ⟩ ⇓ σ' ∧ σ' ⊨ I ∧ Q
```

### 19.3 Class Invariants

**Classes support invariants with inheritance:**

```cantrip
public record Counter {
    private value: i32;

    invariant:
        value >= 0;

    public function new(): own Counter
        ensures result.value == 0;
    {
        own Counter { value: 0 }
    }

    public function increment(self: mut Counter)
        ensures self.value == @old(self.value) + 1;
    {
        self.value += 1;
    }
}
```

**Derived record must maintain base invariant:**
```cantrip
public record BoundedCounter {  /* migrated: was `extends Counter`; use composition or trait bounds */
    max_value: i32;

    invariant:
        value <= max_value;  // Additional constraint
        max_value > 0;

    public override function increment(self: mut BoundedCounter)
        requires self.value < self.max_value;
        ensures self.value <= self.max_value;
    {
        if self.value < self.max_value {
            super.increment();
        }
    }
}
```

### 19.4 Modal State Invariants

**Modal states can have invariants:**

```cantrip
modal Transaction {
    state Pending {
        operations: Vec<Operation>;

        must !operations.is_empty();
    }

    state Committed {
        commit_id: u64;
        timestamp: DateTime;

        must commit_id > 0;
        must timestamp.is_valid();
    }

    state RolledBack {
        error: Error;

        must !error.message.is_empty();
    }
}
```

**Checked at transition boundaries:**
```cantrip
@Pending >> commit() >> @Committed {
    // Must establish Committed invariants
    let id = generate_commit_id();  // Must be > 0
    let ts = DateTime.now();        // Must be valid

    Committed {
        commit_id: id,
        timestamp: ts,
    }  // Invariants automatically verified here
}
```

### 19.5 Loop Invariants

**Loop invariants specify properties maintained across iterations:**

```cantrip
function sum_array(arr: [i32]): i32 {
    var sum = 0;
        var i = 0;

        while i < arr.length() {
            invariant:
                0 <= i;
                i <= arr.length();
                sum == sum_of(arr[0..i]);

            sum += arr[i];
            i += 1;
        }

        sum
}
```

**Verification:**
1. **Initialization:** Invariant holds before loop
2. **Preservation:** If invariant holds at start of iteration, it holds at end
3. **Termination:** When loop exits, invariant ∧ ¬condition gives postcondition

**Verification conditions:**
```
// Initialization
sum == 0 ∧ i == 0 ⟹ I

// Preservation
I ∧ i < arr.length() ∧ (sum' = sum + arr[i]) ∧ (i' = i + 1) ⟹ I'

// Conclusion
I ∧ ¬(i < arr.length()) ⟹ sum == sum_of(arr[0..arr.length()])
```

---

## 20. Verification and Testing

### 20.1 Verification Modes

**Three verification strategies:**

1. **Static verification:** Prove contracts at compile time
2. **Runtime verification:** Check contracts at runtime (default)
3. **No verification:** Trust the programmer (unsafe)

**Compiler flags:**
```bash
# Static verification (must prove or fail compilation)
arc compile --verify=static main.arc

# Runtime verification (default)
arc compile --verify=runtime main.arc

# No verification (unsafe)
arc compile --verify=none main.arc
```

**Per-function control:**
```cantrip
#[verify(static)]
function calculate_tax(income: f64, rate: f64): f64
    requires
        income >= 0.0;
        rate >= 0.0 && rate <= 1.0;
    ensures
        result >= 0.0;
        result <= income;
{
    income * rate
}

#[verify(runtime)]
function user_input_handler(input: String): Result<Data, Error>
    requires !input.is_empty();
{
    parse(input)
}

#[verify(none)]
unsafe function low_level_operation(ptr: *u8) {
    // Unchecked pointer operations
}
```

### 20.2 Static Verification

**Uses SMT solvers (Z3, CVC5) to prove contract validity:**

**Process:**
1. Generate verification conditions
2. Convert to SMT-LIB format
3. Query SMT solver
4. If provable: compilation succeeds
5. If not provable: compilation fails with counterexample

**Example:**
```cantrip
#[verify(static)]
function abs(x: i32): i32
    ensures
        result >= 0;
        (x >= 0 implies result == x);
        (x < 0 implies result == -x);
{
    if x >= 0 { x } else { -x }
}

// Compiler generates SMT query:
// (declare-fun x () Int)
// (assert (=> (>= x 0) (= (abs x) x)))
// (assert (=> (< x 0) (= (abs x) (- x))))
// (assert (>= (abs x) 0))
// (check-sat)  → sat (provable)
```

**When static verification fails:**
```bash
$ arc compile --verify=static main.arc
error[E4001]: Cannot prove postcondition
  --> src/main.arc:15:5
   |
15 |         result >= 0;
   |         ^^^^^^^^^^^ cannot prove this always holds
   |
   = note: Counterexample: x = -2147483648
   = help: Consider adding precondition: x > i32::MIN
```

### 20.3 Runtime Verification

**Default mode - inserts runtime checks:**

**Generated code:**
```cantrip
// Source
function divide(a: f64, b: f64): f64
    requires b != 0.0;
{
    a / b
}

// Generated (conceptual)
function divide(a: f64, b: f64): f64 {
    // Check preconditions
    if !(b != 0.0) {
        panic!("Precondition violation in divide: b != 0.0
                at src/math.arc:42
                b = {}", b);
    }

    // Execute
    let result = a / b;

    // Check postconditions (if any)

    result
}
```

**Optimization:** Precondition checks can be elided if caller proves contracts.

**Performance:**
- Debug mode: All checks enabled
- Release mode: Configurable (default: enabled)
- Critical path: Use `#[verify(static)]` to eliminate runtime overhead

### 20.4 Contract Fuzzing

**Compiler can automatically fuzz contracts to generate test cases:**

```cantrip
#[fuzz_contracts(cases = 1000)]
function binary_search(arr: [i32], target: i32): Option<usize>
    requires forall(i in 0..arr.length() - 1) {
        arr[i] <= arr[i + 1]
    };
    ensures match result {
        Some(idx) -> arr[idx] == target,
        None -> forall(i in 0..arr.length()) { arr[i] != target },
    };
{
    // Binary search implementation
    var low = 0;
    var high = arr.length();

    while low < high {
        let mid = (low + high) / 2;
        if arr[mid] < target {
            low = mid + 1;
        } else if arr[mid] > target {
            high = mid;
        } else {
            return Some(mid);
        }
    }
    None
}
```

**Compiler generates:**
```cantrip
@generated
test {
    // Edge cases
    expect binary_search(&[], 5) == None;
    expect binary_search(&[42], 42) == Some(0);
    expect binary_search(&[42], 43) == None;

    // Found cases
    expect binary_search(&[1, 2, 3, 4, 5], 3) == Some(2);
    expect binary_search(&[1, 2, 3, 4, 5], 1) == Some(0);
    expect binary_search(&[1, 2, 3, 4, 5], 5) == Some(4);

    // Not found cases
    expect binary_search(&[1, 3, 5, 7], 4) == None;
    expect binary_search(&[1, 3, 5, 7], 0) == None;
    expect binary_search(&[1, 3, 5, 7], 8) == None;

    // ... 991 more generated test cases
}
```

### 20.5 Fuzzing Configuration

**Project configuration:**
```toml
# arc.toml
[testing]
contract_fuzzing = true
fuzz_cases_default = 100
fuzz_shrink_attempts = 100
fuzz_timeout = "30s"
fuzz_seed = 42  # For reproducibility
```

**Command line:**
```bash
arc test --contract-fuzzing
arc test --fuzz-cases=10000
arc test --no-contract-fuzzing
arc test --fuzz-only=binary_search
```

**Fuzzing strategies:**
1. **Random generation:** Generate random inputs
2. **Boundary testing:** Test edge cases (0, max, min)
3. **Shrinking:** Find minimal failing case
4. **Mutation:** Mutate known inputs

### 20.6 Integration with Testing

**Contracts work with unit tests:**

```cantrip
#[test]
function test_withdraw() {
    var account = BankAccount.new(100.0);

    // These contracts are checked:
    account.withdraw(50.0);  // OK

    // This would panic:
    // account.withdraw(60.0);  // Precondition violation
}

#[test]
#[should_panic(expected = "Insufficient funds")]
function test_overdraft() {
    var account = BankAccount.new(100.0);
    account.withdraw(150.0);  // Should panic
}
```

**Property-based testing:**
```cantrip
#[property_test(cases = 1000)]
function test_sum_commutative(a: i32, b: i32) {
    // Property: addition is commutative
    assert_eq!(a + b, b + a);
}

#[property_test(cases = 1000)]
function test_sort_preserves_length(arr: Vec<i32>) {
    let original_len = arr.length();
    let sorted = arr.sort();
    assert_eq!(sorted.length(), original_len);
}
```

---

# Part VI (Normative): Effect System

## 21. Effects and Side Effects

> **Editorial Note (Informative):** Drafts of §21 contained overlapping sub-numbering (e.g., §21.9.* and §21.11.*).
> This specification treats §21.11 “Complete Effect Taxonomy” as canonical for taxonomy definitions and retains
> all previously listed content verbatim for completeness. Where duplicate listings occur, the normative definition
> is the earliest occurrence within §21 and the taxonomy table in §21.11.


### 21.1 Effect System Overview

**Definition 21.1 (Effect):** An effect represents a computational side effect - any observable interaction with the world outside pure computation.

**Design goals:**
- Track allocations, I/O, non-determinism, and resource usage
- Enforce determinism for networking/replay systems
- Prevent allocations in hot loops
- Budget tracking with compile-time and runtime enforcement
- Zero runtime overhead for pure functions

**Effect categories:**
1. **Memory effects** - Heap, stack, region allocations
2. **I/O effects** - File system, network, console
3. **Non-determinism** - Time, random, threading
4. **Rendering** - GPU operations, audio
5. **System** - Process spawning, FFI, unsafe operations

### 21.2 Effect Syntax

**Pure function (no effects):**
```cantrip
function add(a: i32, b: i32): i32 {
    a + b
}
```

**Function with effects:**
```cantrip
function load_data(path: String): Result<Data, Error>
    needs fs.read, alloc.heap;
    requires !path.is_empty();
{
    std.fs.read_to_string(path)
}
```

**Critical rule:** Effects are NEVER inferred. All functions must declare effects explicitly or be pure.

### 21.2.7 User-Defined Effects

**Definition 21.7 (Effect Definition):** A named effect is a composite of primitive effects and/or other named effects.

**Syntax:**

```ebnf
EffectDefinition ::= Visibility? "effect" Identifier "=" EffectExpr ";"

EffectExpr ::= EffectTerm
             | EffectExpr "+" EffectTerm      // Union
             | EffectExpr "-" EffectTerm      // Difference

EffectTerm ::= PrimitiveEffect
             | "!" EffectFactor                // Negation
             | EffectFactor

EffectFactor ::= EffectAtom
               | "(" EffectExpr ")"

EffectAtom ::= EffectName ("." "*")?          // e.g., fs.read or fs.*
             | EffectName "(" EffectParam ")"  // e.g., net.read(inbound)
             | Identifier                      // Reference to named effect
```

**Examples:**

```cantrip
// Simple combination
effect WebService = fs.read + fs.write + net.read(inbound) + net.write;

// With negation
effect NoNetwork = !net.*;

// Composition of named effects
effect SafeWebService = WebService - fs.delete;

// With effect budgets
effect LimitedHeap = alloc.heap(bytes<=1MB);

// Simple effect
effect GameTick = alloc.temp;  // Only temp allocation
```

**Formal semantics:**

```
⟦effect E = ε₁ + ε₂ + ... + εₙ⟧ = ε₁ ∪ ε₂ ∪ ... ∪ εₙ
⟦effect E = E₁ + E₂⟧ = ⟦E₁⟧ ∪ ⟦E₂⟧
⟦effect E = E₁ - ε⟧ = ⟦E₁⟧ \ {ε}
⟦effect E = !ε⟧ = AllEffects \ {ε}
```

**Type rules:**

```
[Effect-Def]
ε₁ : Effect    ...    εₙ : Effect
names distinct
─────────────────────────────────────────
Γ ⊢ effect E = ε₁ + ... + εₙ well-formed

[Effect-Ref]
effect E = expr defined    Γ ⊢ expr : EffectSet
───────────────────────────────────────────────────
Γ ⊢ E : EffectSet    ⟦E⟧ = ⟦expr⟧

[Effect-Union]
Γ ⊢ ε₁ : EffectSet    Γ ⊢ ε₂ : EffectSet
─────────────────────────────────────────
Γ ⊢ ε₁ + ε₂ : EffectSet
⟦ε₁ + ε₂⟧ = ⟦ε₁⟧ ∪ ⟦ε₂⟧

[Effect-Difference]
Γ ⊢ ε₁ : EffectSet    Γ ⊢ ε₂ : EffectSet
─────────────────────────────────────────
Γ ⊢ ε₁ - ε₂ : EffectSet
⟦ε₁ - ε₂⟧ = ⟦ε₁⟧ \ ⟦ε₂⟧

[Effect-Negation]
Γ ⊢ ε : EffectSet
─────────────────────────────────────
Γ ⊢ !ε : EffectSet
⟦!ε⟧ = AllEffects \ ⟦ε⟧
```

**Expansion semantics:**

Effect definitions are expanded inline during effect checking:

```cantrip
// Definition
effect WebService = fs.read + fs.write + net.read(inbound);

// Usage
function handler()
    needs WebService;
{
    { ... }
}

// Expanded form (compile-time only)
function handler()
    needs fs.read, fs.write, net.read(inbound);
{
    { ... }
}
```

**Visibility rules:**

Effect definitions follow standard visibility rules:

```cantrip
// Private (module-only, default)
effect InternalOps = alloc.temp + fs.read;

// Public (exported)
public effect APIEffects = fs.read + net.read(outbound);

// Internal (package-only)
internal effect PackageOps = fs.* + alloc.heap;
```

**Scope:**

Effect definitions can appear:

1. At module level (after imports, before items)
2. In dedicated effect definition modules

```cantrip
// Module-level
import std.fs;

effect LocalOps = fs.read + fs.write;

function process()
    needs LocalOps;
{
    { ... }
}
```

```cantrip
// In effects module: src/effects.arc
public effect WebService = fs.read + fs.write + net.read(inbound);
public effect DatabaseOps = fs.read + fs.write + alloc.heap;

// Import and use: src/handlers.arc
import effects.WebService;

function handler()
    needs WebService;
{
    { ... }
}
```

### 21.3 Memory Effects

**Effect family: `alloc.*`**

```cantrip
alloc.heap          // Heap allocation
alloc.stack         // Explicit stack allocation
alloc.region        // Region allocation
alloc.temp          // Temporary allocations (frame-local)
alloc.*             // Any allocation
```

**Examples:**
```cantrip
function create_vector(): own Vec<i32>
    needs alloc.heap;
{
    Vec.new()
}

function with_temp_buffer(): Result<(), Error>
    needs alloc.temp;
{
    let buffer = [0u8; 1024];  // Stack/temp allocation
    process(buffer)
}

function parse(): Result<Data, Error>
    needs alloc.region;
{
    region temp {
        let tokens = lex_in<temp>(input);
        let ast = parse_in<temp>(tokens);
        Ok(optimize(ast))
    }
}
```

**Type rule:**
```
[T-Alloc]
Γ ⊢ e : T
effects(e) ⊇ alloc.heap
─────────────────────────────
Γ ⊢ function f(): T
    needs alloc.heap;
{
    e
} : T ! alloc.heap
```

### 21.4 File System Effects

**Effect family: `fs.*`**

```cantrip
fs.read             // Read files
fs.write            // Write files
fs.delete           // Delete files
fs.metadata         // Read file metadata
fs.*                // Any filesystem operation
```

**Examples:**
```cantrip
function read_config(path: String): Result<Config, Error>
    needs fs.read, alloc.heap;
    requires !path.is_empty();
{
    let contents = std.fs.read_to_string(path)?;
    Config.parse(contents)
}

function save_log(path: String, message: String): Result<(), Error>
    needs fs.write;
    requires !path.is_empty();
{
    std.fs.write(path, message)
}

function backup_directory(source: String, dest: String): Result<(), Error>
    needs fs.read, fs.write, alloc.heap;
{
    std.fs.copy_dir(source, dest)
}
```

### 21.5 Network Effects

**Effect family: `net.*`**

```cantrip
net.read(inbound)   // Accept incoming connections
net.read(outbound)  // Make outgoing connections
net.write           // Send network data
net.*               // Any network operation
```

**Direction parameter:**
- `inbound`: Server accepting connections
- `outbound`: Client making connections

**Examples:**
```cantrip
function fetch_url(url: String): Result<String, Error>
    needs net.read(outbound), alloc.heap;
    requires !url.is_empty();
{
    let client = HttpClient.new();
    client.get(url)
}

function start_server(port: u16): Result<Server, Error>
    needs net.read(inbound), alloc.heap;
{
    Server.bind(port)
}

function handle_request(req: Request): Result<(), Error>
    needs net.write, alloc.heap;
{
    let response = process(req);
    response.send()
}
```

### 21.6 Non-Determinism Effects

**Time and randomness:**

```cantrip
time.read           // Read system time/clock
time.sleep          // Sleep/delay operations
random              // Non-deterministic RNG
thread.spawn        // Create threads
thread.join         // Wait for threads
```

**Examples:**
```cantrip
function current_timestamp(): i64
    needs time.read;
{
    std.time.now().unix_timestamp()
}

function generate_uuid(): Uuid
    needs random, time.read;
{
    Uuid.new_v4()
}

function parallel_map<T, U>(items: Vec<T>, f: T -> U): Vec<U>
    needs thread.spawn, alloc.heap;
{
    std.parallel.map(items, f)
}
```

**Determinism enforcement:**
```cantrip
function replay_simulation(events: [Event]): State
    // Pure function - all effects denied by default (including time, random)
{
    var state = State.initial();
    for event in events {
        state = state.apply(event);
    }
    state
}
```

### 21.7 Standard Effect Definitions

**Definition 21.7 (Standard Library Effects):** The `std.effects` module provides common effect combinations designed for typical programming patterns. These definitions should be preferred over custom effect unions for consistency and LLM recognition.

#### 21.7.1 Importing Standard Effects

```cantrip
// Import commonly used effects
import std.effects.{SafeIO, WebService, GameTick};

// Use in function signatures
function process_file(path: String): Result<Data, Error>
    needs SafeIO;  // Instead of fs.read + fs.write + alloc.heap
{
    let contents = std.fs.read_to_string(path)?;
    Data.parse(contents)
}
```

#### 21.7.2 Core Standard Effects

**Module:** `std.effects`

```cantrip
// Pure computation (no effects)
public effect Pure = {};

// All allocation types
public effect AllAlloc = alloc.*;

// Safe file I/O (read and write, no delete)
public effect SafeIO = fs.read + fs.write + alloc.heap;

// All I/O operations
public effect AllIO = fs.* + net.* + io.write;

// Network I/O only
public effect NetworkIO = net.read(inbound) + net.read(outbound) + net.write + alloc.heap;

// File I/O only
public effect FileIO = fs.read + fs.write + alloc.heap;

// Console I/O only
public effect ConsoleIO = io.write;

// Forbid all allocation
public effect NoAlloc = !alloc.*;

// Forbid all I/O
public effect NoIO = !(fs.* + net.* + io.write);

// Deterministic computation (no time, no random)
public effect Deterministic = !(time.* + random);

// Game tick constraints (temp allocation only, no I/O)
public effect GameTick = alloc.temp + !alloc.heap + !alloc.region + NoIO;

// Real-time constraints (no heap allocation, no I/O)
public effect RealTime = !alloc.heap + NoIO;

// Web service (network + file I/O + allocation)
public effect WebService = net.* + fs.read + fs.write + alloc.heap;

// Database operations (file I/O + allocation, no network)
public effect DatabaseOps = fs.* + alloc.heap + !net.*;
```

#### 21.7.3 Effect Pattern Examples

**Pattern 1: Web Service Handler**

```cantrip
import std.effects.WebService;

function handle_http_request(req: Request): Response
    needs WebService;
    requires !req.path.is_empty();
{
    // Can use: network, file system, heap allocation
    let data = load_from_disk(req.path)?;
    let processed = transform(data);
    send_response(processed)
}
```

**Pattern 2: Game Engine Update Loop**

```cantrip
import std.effects.GameTick;

function update_entities(entities: mut [Entity], dt: f32)
    needs GameTick;
    requires dt > 0.0 && dt < 1.0;
{
    // Only temporary allocations allowed
    // No heap, no file system, no network
    region frame {
        let temp_data = process_frame_in<frame>(entities, dt);
        apply_changes(entities, temp_data);
    } // O(1) bulk free - perfect for 60 FPS
}
```

**Pattern 3: Real-Time Audio Processing**

```cantrip
import std.effects.RealTime;

function process_audio_buffer(buffer: mut [f32], params: AudioParams)
    needs RealTime;
    requires buffer.length() == 512;
{
    // No heap allocation (latency spike)
    // No I/O (unpredictable timing)
    for sample in buffer {
        *sample = apply_filter(*sample, params);
    }
}
```

**Pattern 4: Pure Computation**

```cantrip
import std.effects.Pure;

function calculate_checksum(data: [u8]): u32
    needs Pure;  // Explicit no-effects marker
    requires !data.is_empty();
{
    var checksum: u32 = 0;
    for byte in data {
        checksum = checksum ^ (byte as u32);
    }
    checksum
}
```

#### 21.7.4 Custom Effect Composition

**Compose standard effects for project-specific needs:**

```cantrip
// Backend API server
effect BackendAPI = std.effects.WebService + std.effects.DatabaseOps;

// Frontend (no file system access)
effect Frontend = std.effects.NetworkIO + !fs.*;

// Read-only service
effect ReadOnly = std.effects.FileIO - fs.write;

// Batch processor
effect BatchProcessor = std.effects.FileIO + std.effects.Deterministic;
```

**Usage:**

```cantrip
function api_endpoint(req: Request): Response
    needs BackendAPI;
{
    let user_data = query_database(req.user_id)?;
    let cached = load_cache_file(req.cache_key)?;
    generate_response(user_data, cached)
}
```

#### 21.7.5 Effect Documentation Pattern

**When defining custom effects, document purpose and usage:**

```cantrip
/// Defines effects for cryptocurrency transaction processing.
///
/// Includes network I/O for blockchain communication, file I/O for
/// local storage, and deterministic computation for reproducibility.
///
/// Explicitly forbids:
/// - Random number generation (must use deterministic sources)
/// - Time operations (must use block timestamps)
///
/// # Examples
/// ```
/// function process_transaction(tx: Transaction): Result<Receipt, Error>
///     needs CryptoEffects;
/// {
///     validate_signatures(tx)?;
///     broadcast_to_network(tx)?;
///     store_locally(tx)
/// }
/// ```
effect CryptoEffects =
    std.effects.NetworkIO +
    std.effects.FileIO +
    std.effects.Deterministic +
    !random +
    !time.read;
```

---

### 21.8 Effect Composition Best Practices

#### 21.8.1 Naming Conventions

**Follow consistent naming for custom effects:**

```cantrip
// ✅ GOOD: Descriptive, domain-specific
effect UserAuthEffects = ...
effect PaymentProcessing = ...
effect DataExport = ...

// ❌ BAD: Vague, generic
effect Effects1 = ...
effect MyEffects = ...
effect Stuff = ...
```

#### 21.8.2 Granularity Guidelines

**Balance between specificity and reusability:**

```cantrip
// ✅ GOOD: Clear scope, reusable
effect WebService = net.* + fs.read + fs.write + alloc.heap;
effect DatabaseRead = fs.read + alloc.heap;
effect DatabaseWrite = fs.write + alloc.heap;

// ❌ TOO SPECIFIC: Not reusable
effect HandleUserLoginRequest = net.read(inbound) + fs.read + alloc.heap;

// ❌ TOO BROAD: Not meaningful
effect Everything = *;
```

#### 21.8.3 Documentation Requirements

**All custom effects must document:**

1. **Purpose:** What the effect is for
2. **Included effects:** What it grants access to
3. **Excluded effects:** What it forbids (if any)
4. **Example usage:** Typical function signatures

```cantrip
/// Effects for microservice internal operations.
///
/// **Purpose:** Standard effects for internal service operations including
/// database access, inter-service communication, and logging.
///
/// **Includes:**
/// - Database I/O (read and write)
/// - Service-to-service network calls
/// - Structured logging to disk
/// - Heap allocation for dynamic data
///
/// **Excludes:**
/// - External network access (use `ExternalService` instead)
/// - File deletion (use `AdminEffects` instead)
///
/// **Example:**
/// ```
/// function update_user_record(user_id: u64, data: UserData): Result<(), Error>
///     needs MicroserviceInternal;
/// {
///     let user = db.load_user(user_id)?;
///     let updated = user.merge(data);
///     db.save_user(updated)?;
///     log.info("User {} updated", user_id);
///     Ok(())
/// }
/// ```
effect MicroserviceInternal =
    fs.read + fs.write +
    net.read(outbound) + net.write +
    io.write +
    alloc.heap;
```

---

### 21.9 Rendering Effects

**GPU and audio:**

```cantrip
render.draw         // GPU draw calls
render.compute      // GPU compute shaders
audio.play          // Audio playback
input.read          // Read input devices
```

**Examples:**
```cantrip
function render_frame(scene: Scene)
    needs render.draw;
{
    gpu.clear();
        for entity in scene.entities {
            gpu.draw(entity);
        }
        gpu.present();
}

function run_physics(data: ComputeData): Result<PhysicsState, Error>
    needs render.compute;
{
    gpu.dispatch_compute_shader(data)
}
```

### 21.10 System Effects

**Process and FFI:**

```cantrip
process.spawn       // Execute external processes
ffi.call            // Call foreign functions
unsafe.ptr          // Raw pointer operations
panic               // May panic/abort
```

**Examples:**
```cantrip
function run_command(cmd: String): Result<Output, Error>
    needs process.spawn, alloc.heap;
{
    std.process.Command.new(cmd).execute()
}

function call_c_library(data: *u8): i32
    needs ffi.call, unsafe.ptr;
{
    unsafe {
            external_c_function(data)
        }
}
```

### 21.11 Complete Effect Taxonomy

**Definition 21.11 (Effect Taxonomy):** The complete set of primitive effects in Cantrip, organized hierarchically by category.

#### 21.11.1 Memory Effects (`alloc.*`)

```cantrip
alloc.heap          // Heap allocation via malloc/new
alloc.stack         // Explicit stack allocation (large arrays)
alloc.region        // Region/arena allocation
alloc.temp          // Frame-local temporary allocation
```

**Usage guidelines:**
- `alloc.heap`: Long-lived data, return values, dynamic structures
- `alloc.stack`: Large stack arrays, local buffers
- `alloc.region`: Batch processing, compilation phases, parsers
- `alloc.temp`: Small frame-local temporaries

**Wildcards:**
- `alloc.*`: Any allocation type

#### 21.9.2 File System Effects (`fs.*`)

```cantrip
fs.read             // Read files and directories
fs.write            // Write/modify files
fs.delete           // Delete files and directories
fs.metadata         // Read file metadata (size, permissions, etc.)
```

**Wildcards:**
- `fs.*`: Any file system operation

#### 21.9.3 Network Effects (`net.*`)

```cantrip
net.read(inbound)   // Accept incoming connections (server)
net.read(outbound)  // Make outgoing connections (client)
net.write           // Send data over network
```

**Direction parameters:**
- `inbound`: Server accepting connections
- `outbound`: Client making connections

**Wildcards:**
- `net.*`: Any network operation
- `net.read`: Both inbound and outbound

#### 21.9.4 I/O Effects (`io.*`)

```cantrip
io.write            // Write to stdout/stderr (console output)
```

**Note:** `io.write` is distinct from `fs.write` (files) and `net.write` (network).

#### 21.9.5 Time Effects (`time.*`)

```cantrip
time.read           // Read system clock/time
time.sleep          // Sleep/delay operations
```

#### 21.9.6 Threading Effects (`thread.*`)

```cantrip
thread.spawn        // Create new threads
thread.join         // Wait for thread completion
thread.atomic       // Atomic operations
```

#### 21.9.7 Rendering Effects (`render.*`)

```cantrip
render.draw         // GPU draw calls
render.compute      // GPU compute shaders
```

#### 21.9.8 Audio Effects (`audio.*`)

```cantrip
audio.play          // Audio playback
```

#### 21.9.9 Input Effects (`input.*`)

```cantrip
input.read          // Read from input devices (keyboard, mouse, gamepad)
```

#### 21.9.10 Process Effects (`process.*`)

```cantrip
process.spawn       // Execute external processes
```

#### 21.9.11 FFI Effects (`ffi.*`)

```cantrip
ffi.call            // Call foreign functions (C, etc.)
```

#### 21.9.12 Unsafe Effects (`unsafe.*`)

```cantrip
unsafe.ptr          // Raw pointer operations
```

#### 21.9.13 System Effects

```cantrip
panic               // May panic/abort execution
async               // Asynchronous computation
random              // Non-deterministic random number generation
```

#### 21.9.14 Complete Effect Hierarchy

```
All Effects (*)
├── alloc.*
│   ├── alloc.heap
│   ├── alloc.stack
│   ├── alloc.region
│   └── alloc.temp
├── fs.*
│   ├── fs.read
│   ├── fs.write
│   ├── fs.delete
│   └── fs.metadata
├── net.*
│   ├── net.read(inbound)
│   ├── net.read(outbound)
│   └── net.write
├── io.*
│   └── io.write
├── time.*
│   ├── time.read
│   └── time.sleep
├── thread.*
│   ├── thread.spawn
│   ├── thread.join
│   └── thread.atomic
├── render.*
│   ├── render.draw
│   └── render.compute
├── audio.*
│   └── audio.play
├── input.*
│   └── input.read
├── process.*
│   └── process.spawn
├── ffi.*
│   └── ffi.call
├── unsafe.*
│   └── unsafe.ptr
├── panic
├── async
└── random
```

#### 21.9.15 Standard Effect Definitions

The standard library (`std.effects`) provides common effect combinations:

```cantrip
// Pure computation (no effects)
public effect Pure = !*;

// All allocation types
public effect AllAlloc = alloc.*;

// All I/O operations
public effect AllIO = fs.* + net.* + io.write;

// Safe I/O (read/write but no delete)
public effect SafeIO = fs.read + fs.write + alloc.heap;

// Network I/O only
public effect NetworkIO = net.* + alloc.heap;

// File I/O only
public effect FileIO = fs.read + fs.write + alloc.heap;

// Console I/O only
public effect ConsoleIO = io.write;

// Forbid all allocation
public effect NoAlloc = !alloc.*;

// Forbid all I/O
public effect NoIO = !(fs.* + net.* + io.write);

// Deterministic computation (no time, no random)
public effect Deterministic = !(time.* + random);

// Game tick constraints (temp allocation only, no I/O)
public effect GameTick = alloc.temp + !alloc.heap + !alloc.region + NoIO;

// Real-time constraints (no heap allocation, no I/O)
public effect RealTime = !alloc.heap + NoIO;

// Web service (network + file I/O + allocation)
public effect WebService = net.* + fs.read + fs.write + alloc.heap;

// Database operations (file I/O + allocation, no network)
public effect DatabaseOps = fs.* + alloc.heap + !net.*;
```

**Usage example:**
```cantrip
import std.effects.SafeIO;

function process_config(path: String): Result<Config, Error>
    needs SafeIO;
{
    let contents = std.fs.read_to_string(path)?;
    Config.parse(contents)
}
```

---

## 22. Effect Rules and Checking


### 22.7 Async Effect Masks (Normative)

Awaiting a future imports its declared effects into the enclosing async function’s *effect mask*.
To reduce boilerplate while preserving explicitness, Cantrip provides targeted diagnostics and fix‑its
for common async sources:

- `net.*` from awaited network futures
- `time.sleep` from awaited timers

**Rule (masking):**
```
[T-Async-Mask]
Γ ⊢ await e : T ! ε_fut
enclosing function declares ε_fun
if ε_fut ⊄ ε_fun then
  ERROR E9201 with fix-it: add missing effects to 'needs' clause
```

**Notes:**
- This is **not** effect inference; no code is accepted unless the signature is updated.
- Implementations MAY surface the exact missing atoms (e.g., `net.read(outbound)`), and SHOULD
  offer “Add `needs net.read(outbound)`” code actions.

**Examples:**
```cantrip
async function fetch(url: str): String
    needs alloc.heap; // missing net.read(outbound)
{
    let body = await http.get(url)?;  // E9201 → fix-it adds net.read(outbound)
    body
}
```


### 22.1 Effect Type System

**Effect judgment:**
```
Γ ⊢ e ! ε  (expression e has effect ε)
```

**Effect lattice:**
```
(E, ⊑, ⊔, ⊓)

where:
  ∅ ⊑ ε                           (pure is subeffect)
  ε₁ ⊑ ε₁ ⊔ ε₂                     (union upper bound)
  ε₁ ⊔ ε₂ = ε₂ ⊔ ε₁                 (commutativity)
  (ε₁ ⊔ ε₂) ⊔ ε₃ = ε₁ ⊔ (ε₂ ⊔ ε₃)    (associativity)
```

### 22.2 Core Effect Rules

**[E-Pure] Pure expression:**
```
───────────────
Γ ⊢ v ! ∅
```

**[E-Var] Variable:**
```
x : T ∈ Γ
───────────────
Γ ⊢ x ! ∅
```

**[E-App] Function application:**
```
Γ ⊢ f : fn(T): U ! ε₁
Γ ⊢ e : T ! ε₂
─────────────────────
Γ ⊢ f(e) ! ε₁ ⊔ ε₂
```

**[E-Seq] Sequencing:**
```
Γ ⊢ e₁ : T₁ ! ε₁    Γ ⊢ e₂ : T₂ ! ε₂
─────────────────────────────────────
Γ ⊢ e₁ ; e₂ : T₂ ! (ε₁ ; ε₂)
```

**[E-If] Conditional:**
```
Γ ⊢ e₁ : bool ! ε₁
Γ ⊢ e₂ : T ! ε₂
Γ ⊢ e₃ : T ! ε₃
──────────────────────────────
Γ ⊢ if e₁ then e₂ else e₃ : T ! (ε₁ ; (ε₂ ⊔ ε₃))
```

**[E-Pipeline] Flow operator:**
```
Γ ⊢ e₁ : T ! ε₁    Γ ⊢ e₂ : fn(T): U ! ε₂
──────────────────────────────────────────
Γ ⊢ e₁ => e₂ : U ! (ε₁ ; ε₂)
```

### 22.3 Explicit Declaration Required

**Effects are NEVER inferred:**

```cantrip
function helper(): Vec<i32>
    needs alloc.heap;  // MUST be explicit
{
    Vec.new()
}

// ERROR: Missing effect declaration
function bad(): Vec<i32> {
    Vec.new()  // ERROR E9001: alloc.heap not declared
}
```

**Type checking:**
```
[Check-Effects]
function f(): T
    needs ε_declared;
{
    e
}

Γ ⊢ e : T ! ε_actual
ε_actual ⊑ ε_declared
─────────────────────────
f well-typed
```

**Error if undeclared:**
```bash
$ arc compile main.arc
error[E9001]: Missing effect declaration
  --> src/main.arc:5:9
   |
 5 |         Vec.new()
   |         ^^^^^^^^^ requires effect alloc.heap
   |
note: function requires no effects
  --> src/main.arc:2:5
   |
 2 |     requires:
   |     ^^^^^^^^^ declared as pure
   |
help: add effect declaration
   |
 2 |     requires<alloc.heap>:
   |             ^^^^^^^^^^^^
```

### 22.3.1 Named Effects in Declarations

Named effects can be used anywhere primitive effects are used:

```cantrip
effect WebOps = fs.read + fs.write + net.read(inbound);

function process()
    needs WebOps;  // Expands to constituent effects
{
    { ... }
}
```

**Combining named and primitive effects:**

```cantrip
function process_with_logging()
    needs WebOps, time.read;  // Named + primitive
{
    { ... }
}
```

**Effect subtraction:**

```cantrip
function readonly_web()
    needs WebOps - fs.write;  // Remove specific effect
{
    { ... }
}
```

**Type checking treats named effects as their expanded form:**

```
[Check-Named-Effects]
effect E = ε₁ + ... + εₙ

function f(): T
    needs E;
{
    e
}

Γ ⊢ e : T ! ε_actual
{ε₁, ..., εₙ} ⊇ ε_actual
─────────────────────────────────
f well-typed
```

**Example integration:**

```cantrip
// Before (with primitive effects)
function load_data(path: String): Result<Data, Error>
    needs fs.read, alloc.heap;
    requires !path.is_empty();
{
    std.fs.read_to_string(path)
}

// After (with named effect)
effect FileOps = fs.read + alloc.heap;

function load_data(path: String): Result<Data, Error>
    needs FileOps;
    requires !path.is_empty();
{
    std.fs.read_to_string(path)
}
```

### 22.4 Forbidden Effects

**Purpose:** Forbidden effects (`!effect`) restrict wildcards and polymorphic effects. They are **NOT** for documenting intent.

#### 22.4.1 Valid Use Case 1: Wildcard Restriction

Restrict a wildcard to exclude specific effects:

**✅ CORRECT - Wildcard Restriction:**

```cantrip
function safe_fs_operations()
    needs fs.*, !fs.delete;  // All fs operations except delete
{
    read_file(path)?;
    write_file(path)?;
    // delete_file(path)?;  // ERROR E9002: fs.delete forbidden
}
```

**❌ WRONG - Not a Wildcard Restriction:**

```cantrip
function broken()
    needs fs.read, fs.write, !fs.delete;  // ERROR E9010: Redundant
{
    read_file(path)?;
}
```

**Type rule:**
```
[T-Wildcard-Restriction]
function f()
    needs epsilon.*, !epsilon.specific;
{
    e
}

epsilon.specific in epsilon.*
epsilon_actual subset (epsilon.* \ {epsilon.specific})
──────────────────────────────────
f well-typed
```

#### 22.4.2 Valid Use Case 2: Polymorphic Effect Constraint

Constrain effects propagated from generic parameters:

**✅ CORRECT - Polymorphic Constraint:**

```cantrip
function deterministic_map<F>(items: [i32], f: F): Vec<i32>
    where F: Fn(i32): i32
    needs effects(F), !time.read, !random;
{
    items.map(f)
}
```

**❌ WRONG - Not Polymorphic:**

```cantrip
function broken()
    needs alloc.heap, !time.read;  // ERROR E9010: Redundant
{
    Vec.new()
}
```

**Type rule:**
```
[T-Polymorphic-Constraint]
function f<F>()
    needs effects(F), !epsilon_forbidden;
{
    e
}

effects(F) intersect epsilon_forbidden = empty
─────────────────────────────
f well-typed
```

#### 22.4.3 INVALID: Redundant Forbidden Effects

These patterns are **compile errors** (E9010):

**❌ WRONG - Redundant Forbidden Effects:**

```cantrip
function broken()
    needs alloc.heap, !fs.*;  // ERROR E9010: fs.* already forbidden
{
    Vec.new()
}
```

**✅ CORRECT - Just Declare What You Need:**

```cantrip
function correct()
    needs alloc.heap;  // Everything else denied by default
{
    Vec.new()
}
```

**Error rule:**
```
[E-Redundant-Forbidden]
function f()
    needs epsilon_1, ..., epsilon_n, !epsilon_forbidden;
{
    e
}

epsilon_forbidden not_in (epsilon_1 union ... union epsilon_n)
not is_wildcard_restriction(!epsilon_forbidden)
not is_polymorphic_constraint(!epsilon_forbidden)
──────────────────────────────────────────
ERROR E9010: Redundant forbidden effect

Forbidden effects are only valid for:
1. Wildcard restriction: needs effect.*, !effect.specific
2. Polymorphic constraint: needs effects(F), !effect
```

### 22.5 Effect Wildcards

**Use `*` for effect families:**

**✅ CORRECT - Wildcard Usage:**

```cantrip
function network_ops()
    needs net.*;  // All network operations
{
    fetch_data()?;
    post_data()?;
}
```

**❌ WRONG - Redundant Forbidden with Wildcard:**

```cantrip
function broken()
    needs fs.read, fs.write, !fs.delete;  // ERROR E9010
{
    // Should be: needs fs.*, !fs.delete (wildcard restriction)
    // OR: needs fs.read, fs.write (delete already forbidden)
}
```

**Wildcard expansion:**
```
alloc.* = {alloc.heap, alloc.stack, alloc.region, alloc.temp}
fs.* = {fs.read, fs.write, fs.delete, fs.metadata}
net.* = {net.read(inbound), net.read(outbound), net.write}
```

### 22.6 Higher-Order Functions

**Use `effects(F)` to propagate callback effects:**

```cantrip
function map<T, U, F>(items: [T], mapper: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), alloc.heap;
    requires items.length() > 0;
{
    var result = Vec.with_capacity(items.length());
    for item in items {
        result.push(mapper(item));  // May have effects
    }
    result
}
```

**Type rule:**
```
[T-Effects-Of]
Γ ⊢ f : fn(T): U ! ε
────────────────────────────────
effects(f) = ε
```

**Usage:**
```cantrip
let numbers = [1, 2, 3, 4, 5];

// Pure mapper
let doubled = map(numbers, |x| x * 2);  // effects(F) = ∅

// Mapper with effects
let logged = map(numbers, |x| {
    std.io.println(x);  // io.write effect
    x * 2
});  // map requires<io.write, alloc.heap>
```

**Constraining callbacks:**
```cantrip
function parallel_map<T, U, F>(items: [T], mapper: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), !time.read, !random, alloc.heap;
    requires items.length() > 0;
{
    std.parallel.map_n(items, 8)
}

// Usage
let result = parallel_map(data, |x| {
    // Cannot use time.read or random in parallel map
    // let now = std.time.now();  // ERROR
    process(x)
});
```

---

## 23. Effect Budgets

### 23.1 Budget Overview

**Definition 23.1 (Effect Budget):** A budget is a quantitative constraint on resource usage, enforced at compile time or runtime.

**Budget types:**
1. **Memory budgets** - Limit allocation size
2. **Time budgets** - Limit operation duration
3. **Count budgets** - Limit operation count

**Syntax:**
```
effect(constraint)

Examples:
alloc.heap(bytes<=1024)
time.sleep(duration<=5s)
thread.spawn(count<=8)
```

### 23.2 Static Budgets

**Compile-time verification of resource limits:**

```cantrip
function small_allocation(): Vec<u8>
    needs alloc.heap(bytes<=1024);
{
    Vec.with_capacity(100)  // OK: 100 bytes < 1024

    // ERROR: Exceeds budget
    // Vec.with_capacity(10000)
}
```

**Verification:**
```
[Check-Budget-Static]
function f(): T
    needs alloc.heap(bytes<=N);
{
    e
}

∀ allocation in e. size(allocation) ≤ N
─────────────────────────────────────────
f satisfies budget
```

**Compiler analysis:**
```cantrip
function bounded(): Vec<i32>
    needs alloc.heap(bytes<=100);
{
    // Compiler computes: Vec<i32> with capacity 10
    // Size = 10 * sizeof(i32) = 10 * 4 = 40 bytes
    Vec.with_capacity(10)  // OK: 40 < 100
}
```

### 23.3 Dynamic Budgets

**Runtime enforcement with budget tracking:**

```cantrip
function process_chunks(data: [u8], chunk_size: usize): Vec<Vec<u8>>
    needs alloc.heap(bytes<=65536);  // 64KB budget
    requires chunk_size > 0;
{
    var result = Vec.new();
    var budget_used = 0;

    for chunk in data.chunks(chunk_size) {
        let chunk_vec = chunk.to_vec();
        budget_used += chunk_vec.capacity() * sizeof<u8>();

        if budget_used > 65536 {
            panic!("Budget exceeded");
        }

        result.push(chunk_vec);
    }
    result
}
```

**Compiler-generated tracking:**
```cantrip
// Source
function f(): Vec<i32>
    needs alloc.heap(bytes<=1000);
{
    dynamic_allocation(n)
}

// Generated
function f() {
    let mut budget = Budget::new(1000);

    let result = dynamic_allocation_tracked(n, &mut budget);

    if budget.exceeded() {
        panic!("Budget exceeded: used {} bytes", budget.used());
    }

    result
}
```

### 23.4 Time Budgets

**Limit operation duration:**

```cantrip
function timeout_operation(): Result<(), Error>
    needs time.sleep(duration<=5s);
{
    std.time.sleep(Duration.from_secs(3))  // OK: 3s < 5s
}

function may_timeout(task: Task): Result<Output, Error>
    needs time.sleep(duration<=10s);
{
    with_timeout(Duration.from_secs(10), || {
        task.execute()
    })
}
```

**Verification:**
```
[Check-Budget-Time]
function f()
    needs time.sleep(duration<=D);
{
    e
}

∑(all sleep calls in e) ≤ D
───────────────────────────
f satisfies time budget
```

### 23.5 Count Budgets

**Limit operation count:**

```cantrip
function parallel_map(items: [Task]): Vec<Result>
    needs thread.spawn(count<=8);
    requires items.length() > 0;
{
    std.parallel.map_n(items, 8)  // OK: spawns 8 threads
}

function limited_retries(operation: Operation): Result<(), Error>
    needs net.read(outbound, count<=3);
{
    var attempts = 0;
    loop {
        match operation.try() {
            Ok(()) -> return Ok(()),
            Err(e) if attempts < 3 -> attempts += 1,
            Err(e) -> return Err(e),
            }
        }
}
```

### 23.6 Budget Composition

**Budgets compose additively:**

```cantrip
function composed()
    needs alloc.heap(bytes<=1000);
{
    small_allocation();   // Uses 100 bytes
        small_allocation();   // Uses 100 bytes
        // Total: 200 bytes < 1000 ✓
}
```

**Type rule:**
```
[T-Budget-Compose]
Γ ⊢ e₁ : T₁ ! alloc.heap(bytes≤N₁)
Γ ⊢ e₂ : T₂ ! alloc.heap(bytes≤N₂)
────────────────────────────────────────
Γ ⊢ e₁ ; e₂ : T₂ ! alloc.heap(bytes≤N₁+N₂)
```

**Example:**
```cantrip
function process()
    needs alloc.heap(bytes<=10000);
{
    step1();  // 3000 bytes
        step2();  // 4000 bytes
        step3();  // 2000 bytes
        // Total: 9000 bytes < 10000 ✓
}
```

### 23.7 Budget Units

**Supported unit types:**

**Memory:**
```cantrip
bytes<=N
KiB<=N    // Kibibytes (1024 bytes)
MiB<=N    // Mebibytes (1024 KiB)
GiB<=N    // Gibibytes (1024 MiB)
```

**Time:**
```cantrip
ms<=N     // Milliseconds
s<=N      // Seconds
min<=N    // Minutes
```

**Count:**
```cantrip
count<=N  // Number of operations
```

**Examples:**
```cantrip
requires<alloc.heap(KiB<=64)>:      // 64 KiB
requires<alloc.heap(MiB<=1)>:       // 1 MiB
requires<time.sleep(ms<=500)>:      // 500ms
requires<thread.spawn(count<=16)>:  // 16 threads
```

---

## 24. Effect Soundness

### 24.1 Effect Soundness Theorem

**Theorem 24.1 (Effect Soundness):**

If Γ ⊢ e : T ! ε and ⟨e, σ, ∅⟩ →* ⟨v, σ', ε'⟩, then ε' ⊆ ε.

**Proof sketch:**
1. By induction on the derivation of Γ ⊢ e : T ! ε
2. Base cases (values, variables) have no effects
3. Inductive cases preserve effect bounds
4. Effect composition rules ensure ε' ⊆ ε

**Consequence:** Declared effects are an upper bound on actual effects.

### 24.2 Budget Compliance

**Theorem 24.2 (Budget Compliance):**

If Γ ⊢ e : T ! alloc.heap(bytes≤N) and ⟨e, σ⟩ →* ⟨v, σ'⟩,
then total_allocated(e) ≤ N.

**Proof obligation:** Must be verified for all functions with budget constraints.

### 24.3 Effect Approximation

**Lemma 24.1 (Effect Subsumption):**

```
Γ ⊢ e : T ! ε₁    ε₁ ⊆ ε₂
────────────────────────────
Γ ⊢ e : T ! ε₂
```

**Application:** Can always overapproximate effects.

**Example:**
```cantrip
function specific()
    needs fs.read;
{
    read_file("config.txt")
}

function general()
    needs fs.*, alloc.heap;  // Overapproximation
{
    specific()  // OK: fs.read ⊆ fs.*
}
```

### 24.4 Effect Monotonicity

**Lemma 24.2 (Effect Monotonicity):**

If e₁ ⊆ e₂ (e₁ is a subexpression of e₂), then effects(e₁) ⊆ effects(e₂).

**Proof:** By structural induction on expressions.

### 24.5 No Hidden Effects

**Axiom 24.1 (Effect Visibility):**

All effects must be explicitly declared in function signatures. No implicit effects exist.

**Violations caught at compile time:**
```cantrip
function sneaky(): i32 {  // Claims pure
    std.io.println("side effect!");  // ERROR E9001
    42
}
```

# Part VII (Normative): Memory Management

## 25. Lexical Permission System

### 25.1 LPS Overview

**Definition 25.1 (Lexical Permission System):** The LPS provides memory safety through explicit permissions, lexical lifetimes, and region-based memory management, without borrow checking or garbage collection.

**Core principles:**
1. **Explicit permissions** - All access rights visible in code
2. **Lexical lifetimes** - Memory scopes follow code structure (regions)
3. **Zero runtime overhead** - Compiles to raw pointers
4. **Local reasoning** - No global program analysis needed
5. **Simple model** - No complex borrow checker

**Key insight:** Cantrip follows the **Cyclone model**, not the Rust model. Regions control WHEN memory is freed, not WHO can access it.

### 25.2 Design Goals

**Safety guarantees provided:**
- ✅ No use-after-free (regions enforce lifetime)
- ✅ No double-free (regions handle deallocation)
- ✅ No memory leaks (automatic cleanup)
- ✅ Deterministic destruction order (LIFO)

**Safety guarantees NOT provided:**
- ❌ No aliasing bugs (can have multiple mut refs)
- ❌ No data races (programmer's responsibility)
- ❌ No iterator invalidation (programmer's responsibility)

**Philosophy:** Provide memory safety and deterministic deallocation, but don't prevent aliasing. Simpler than Rust, safer than C++.

### 25.3 Memory Safety Model

**Three allocation strategies:**

1. **Stack (automatic):**
   - Fastest allocation
   - LIFO deallocation
   - Limited size
   - Example: Local variables

2. **Heap (explicit):**
   - Flexible lifetime
   - Can escape function
   - Requires explicit management
   - Example: `Vec.new()`

3. **Region (bulk deallocation):**
   - Fast allocation (bump pointer)
   - O(1) bulk deallocation
   - Cannot escape region
   - Example: Temporary parsing data

**Formal model:**

```
Memory = Stack ∪ Heap ∪ ⋃ᵢ Regionᵢ

where:
  Stack = {(addr, value, scope) | addr ∈ StackAddrs}
  Heap = {(addr, value) | addr ∈ HeapAddrs}
  Regionᵢ = {(addr, value, region) | addr ∈ RegionAddrs}
```

### 25.4 Compilation Model

**High-level:**
```cantrip
function example() {
    let x = 42;                    // Stack
    let v = Vec.new();             // Heap
    region temp {
        let buf = Vec.new_in<temp>();  // Region
    }
}
```

**Compiles to (conceptual C):**
```c
void example() {
    int32_t x = 42;  // Stack

    Vec* v = vec_new();  // malloc

    Arena temp = arena_create();
    Vec* buf = vec_new_in_arena(&temp);
    arena_destroy(&temp);  // O(1) bulk free

    vec_free(v);  // free
}
```

**No runtime overhead:**
- No reference counting
- No garbage collection
- No borrow checking at runtime
- Just pointers and deterministic cleanup

---

## 26. Permission Types and Rules

### 26.1 Permission Overview

**Definition 26.1 (Permission):** A permission controls what operations are allowed on a value.

**Permission hierarchy:**

| Permission | Sigil | Read | Write | Move | Destroy | Aliasing |
|------------|-------|------|-------|------|---------|----------|
| Reference | (none) | ✓ | ✗ | ✗ | ✗ | Many allowed |
| Owned | `own` | ✓ | ✓ | ✓ | ✓ | One owner |
| Mutable | `mut` | ✓ | ✓ | ✗ | ✗ | Many allowed |
| Isolated | `iso` | ✓ | ✓ | ✓ | ✗ | Exactly one |

**Key difference from Rust:** No borrow checker. Multiple `mut` references can coexist - programmer ensures correct usage.

### 26.2 Immutable Reference (Default)

**Default for function parameters** - like C++ `const&`:

```cantrip
function read_data(data: Data): i32 {  // Immutable reference
    data.field  // Can read
    // data.field = 42;  // ERROR: Cannot write
}
```

**Type rule:**
```
[T-Ref]
Γ ⊢ e : T
──────────────────────
function f(x: T) accepts immutable reference to e
```

**Multiple references allowed (no borrow checking):**
```cantrip
function example()
    needs alloc.heap;
{
    let data = Data.new();

        // All of these work - no borrow checker
        read1(data);
        read2(data);
        read3(data);
        // Programmer ensures this is safe
}
```

**Formal semantics:**
```
⟦T⟧ = { reference to value of type T }
Operations: Read only
Aliasing: Unlimited
```

### 26.3 Owned Permission

**Full ownership** - like C++ `unique_ptr`:

```cantrip
function example()
    needs alloc.heap;
{
    let data: own Data = Data.new();

        data.field = 42;           // Can write
        let x = data.field;        // Can read
        consume(move data);        // Transfer ownership
        // data is gone - compiler enforces this
}
```

**Type rule:**
```
[T-Own]
Γ ⊢ e : T    copyable(T) = false
──────────────────────────────────
Γ ⊢ e : own T
```

**Ownership rules:**
1. Exactly one owner exists
2. Automatically destroyed when scope ends
3. Must use `move` to transfer ownership
4. Can pass by reference without transferring

**Example:**
```cantrip
function take_ownership(own data: Data) {
    // data is owned by this function
    // Automatically freed when function returns
}

function example()
    needs alloc.heap;
{
    let data = Data.new();

        // Pass by reference - still own data
        read(data);

        // Transfer ownership
        take_ownership(move data);
        // data no longer accessible
}
```

**Formal semantics:**
```
⟦own T⟧ = { unique pointer to value of type T }
Operations: Read, Write, Move, Destroy
Aliasing: Exactly one
Lifetime: Lexical scope
```

### 26.4 Mutable Permission

**Mutable reference** - like C++ `&`:

```cantrip
function update_data(data: mut Data) {
    data.field = 42;  // Can read and write
}
```

**Type rule:**
```
[T-Mut]
Γ ⊢ e : T    mutable(e) = true
──────────────────────────────────
Γ ⊢ e : mut T
```

**Multiple mutable references allowed (programmer's responsibility):**
```cantrip
function example()
    needs alloc.heap;
{
    var data = Data.new();

        // NO borrow checking - all allowed simultaneously
        modify1(mut data);
        modify2(mut data);
        read(data);
        modify3(mut data);

        // Programmer must ensure this doesn't cause bugs!
}
```

**This is different from Rust:**
```rust
// Rust: ERROR - can't have multiple mut refs
let mut data = Data::new();
let r1 = &mut data;
let r2 = &mut data;  // ERROR in Rust

// Cantrip: Allowed, programmer's responsibility
var data = Data.new();
modify1(mut data);
modify2(mut data);  // OK in Cantrip
```

**Formal semantics:**
```
⟦mut T⟧ = { mutable reference to value of type T }
Operations: Read, Write
Aliasing: Unlimited (programmer ensures safety)
Lifetime: Lexical scope
```

### 26.5 Isolated Permission

**Unique reference for thread safety:**

```cantrip
record Atomic<T> {
    value: iso T,
}

impl Atomic<T> {
    function swap(self: Atomic<T>, new_value: iso T): iso T
    needs thread.atomic;
{
    atomic_swap(&self.value, new_value)
    }
}
```

**Type rule:**
```
[T-Iso]
Γ ⊢ e : T    unique(e) in Γ
──────────────────────────────
Γ ⊢ e : iso T
```

**Isolation rules:**
1. Guaranteed unique - exactly one `iso` reference
2. Cannot have any other references
3. Can be safely transferred between threads
4. Used for lock-free data structures

**Example:**
```cantrip
function send_to_thread(data: iso Data): JoinHandle<()>
    needs thread.spawn, alloc.heap;
{
    thread.spawn(move || {
        // data is isolated - safe to move between threads
        process(data);
    })
}
```

**Formal semantics:**
```
⟦iso T⟧ = { isolated reference to value of type T }
Operations: Read, Write, Move
Aliasing: Exactly one (enforced)
Thread-safety: Yes
```

### 26.6 Permission Subtyping

**Subtyping hierarchy:**
```
[Sub-Permission]
own T <: mut T <: T
iso T <: own T
```

**Example:**
```cantrip
function read_only(data: Data) {
    data.field
}

function example()
    needs alloc.heap;
{
    let owned = Data.new();        // own Data
        let mutable = mut owned;       // mut Data

        read_only(owned);     // own Data <: Data ✓
        read_only(mutable);   // mut Data <: Data ✓
}
```

---

## 27. Ownership and Transfer

### 27.1 Move Semantics

**Use `move` to explicitly transfer ownership:**

```cantrip
function consume(own data: Data) {
    // data destroyed at end of scope
}

function example()
    needs alloc.heap;
{
    let data = Data.new();
        consume(move data);  // Explicit transfer
        // data no longer accessible
        // data.field;  // ERROR E3004: value moved
}
```

**Type rule:**
```
[T-Move]
Γ ⊢ e : own T    Γ' = Γ \ {e}
──────────────────────────────
Γ ⊢ move e : own T
Γ' ⊬ e  (e no longer accessible)
```

**Operational semantics:**
```
[E-Move]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
────────────────────────────
⟨move e, σ⟩ ⇓ ⟨v, σ'⟩
σ'(e) = moved
```

### 27.2 Passing by Reference

**Default parameters take references without transferring ownership:**

```cantrip
function process(data: Data) {  // Takes reference
    std.io.println(data.field);
}

function example()
    needs alloc.heap;
{
    let data = Data.new();

        // Pass by reference - data still owned here
        process(data);
        process(data);
        process(data);

        // Still own data
        consume(move data);
}
```

**Type rule:**
```
[T-Ref-Pass]
Γ ⊢ e : own T
function f(x: T)
──────────────────────────────
f(e)  valid (temporary reference)
e still owned after call
```

### 27.3 Permission Transitions

**Valid transitions:**
```
own T → (pass by ref)  // Temporary reference
own T → own T          // Transfer with move
own T → iso T          // Convert to isolated
mut T → T              // Borrow immutably
iso T → own T          // Relax isolation
```

**Invalid transitions:**
```
T (ref) → own T    // ERROR: Cannot steal ownership
mut T → own T      // ERROR: Cannot steal from mut ref
T → iso T          // ERROR: Cannot prove unique
```

**Example:**
```cantrip
function example()
    needs alloc.heap;
{
    let owned = Data.new();

        // Can pass by reference
        read(owned);  // OK

        // Can pass mut reference
        modify(mut owned);  // OK

        // Must move to transfer
        consume(move owned);  // OK

        // Cannot use after move
        // read(owned);  // ERROR E3004
}
```

### 27.4 No Borrow Syntax

**Cantrip does NOT have explicit borrow blocks or lifetime annotations.**

Permissions are declared in function signatures:

```cantrip
// Just pass the value - permission determined by signature
function read(data: Data) { ... }      // Takes immutable ref
function modify(data: mut Data) { ... } // Takes mutable ref
function take(own data: Data) { ... }   // Takes ownership

// Usage
let data = Data.new();
read(data);          // Passes ref
modify(mut data);    // Passes mut ref
take(move data);     // Transfers ownership
```

**No lifetime parameters:**
```rust
// Rust
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str { ... }

// Cantrip - no lifetimes needed
function longest(x: str, y: str): str {
    ...
}
```

**Lifetimes are determined by regions, not annotations.**

### 27.8 The Cantrip Safety Model: Trade-offs and Guarantees

**Purpose:** This section explicitly documents what Cantrip's memory model guarantees and what it leaves to programmer responsibility. Understanding these trade-offs is critical for writing safe code.

#### 27.8.1 What Cantrip Guarantees (✅ Compile-Time Safe)

**1. No Use-After-Free**

Regions enforce LIFO deallocation. Once a region ends, all allocations within it are freed. The compiler prevents returning region-allocated data.

```cantrip
function guaranteed_safe()
    needs alloc.region;
{
    region temp {
        let data = Vec.new_in<temp>();
        process(data);  // OK: data valid within region
    } // data freed here

    // Impossible to access data after region ends
}
```

**2. No Double-Free**

Regions handle cleanup automatically. Manual deallocation is not exposed.

```cantrip
function automatic_cleanup()
    needs alloc.region;
{
    region temp {
        let data1 = Data.new_in<temp>();
        let data2 = Data.new_in<temp>();
    } // Both freed exactly once, automatically
}
```

**3. No Memory Leaks**

Deterministic destruction at region boundaries. All allocations in a region are freed when the region ends.

```cantrip
function no_leaks()
    needs alloc.region;
{
    for i in 0..1000 {
        region iteration {
            let large = Vec.with_capacity_in<iteration>(10000);
            process(large);
        } // Freed each iteration
    }
}
```

**4. No Dangling Pointers**

Cannot return pointers to region-allocated data.

```cantrip
function prevented_at_compile_time(): Vec<i32>
    needs alloc.region;
{
    region temp {
        let vec = Vec.new_in<temp>();
        vec  // ❌ ERROR E5001: Cannot return region data
    }
}
```

#### 27.8.2 What Cantrip Does NOT Guarantee (⚠️ Programmer Responsibility)

**1. No Aliasing Prevention**

Multiple `mut` references to the same data are allowed. Programmer must ensure they don't conflict.

```cantrip
function aliasing_allowed()
    needs alloc.heap;
{
    var data = Vec.new();

    let ref1 = mut data;  // First mutable reference
    let ref2 = mut data;  // Second mutable reference - ALLOWED

    ref1.push(1);  // Modifies through ref1
    ref2.push(2);  // Modifies through ref2

    // ⚠️ PROGRAMMER MUST ENSURE: ref1 and ref2 don't cause conflicts
    // No compile-time enforcement
}
```

**Compare to Rust:**

```rust
// Rust prevents this at compile time
fn rust_prevents() {
    let mut data = Vec::new();
    let ref1 = &mut data;  // First mutable borrow
    let ref2 = &mut data;  // ❌ ERROR: cannot borrow as mutable twice
}
```

**2. No Data Race Prevention**

Cantrip allows concurrent mutable access. Use `iso` types or synchronization primitives.

```cantrip
function data_race_possible()
    needs thread.spawn, alloc.heap;
{
    var shared = Vec.new();

    thread.spawn(|| {
        shared.push(1);  // ⚠️ Concurrent modification
    });

    shared.push(2);  // ⚠️ Concurrent modification

    // Programmer must add synchronization
}
```

**Safe alternative with `iso`:**

```cantrip
function data_race_prevented()
    needs thread.spawn, alloc.heap;
{
    let isolated: iso Vec<i32> = Vec.new();

    let handle = thread.spawn(move || {
        isolated.push(1);  // Exclusive access via iso
    });

    let isolated = handle.join();
    isolated.push(2);
}
```

**3. No Iterator Invalidation Prevention**

Can modify collections while iterating. Programmer must ensure safety.

```cantrip
function iterator_invalidation_possible()
    needs alloc.heap;
{
    var numbers = vec![1, 2, 3, 4, 5];

    for n in numbers {
        numbers.push(n * 2);  // ⚠️ Modifies while iterating
        // Behavior is undefined - programmer's responsibility
    }
}
```

**Safe pattern:**

```cantrip
function iterator_safe()
    needs alloc.heap;
{
    let numbers = vec![1, 2, 3, 4, 5];
    var new_numbers = Vec.new();

    for n in numbers {
        new_numbers.push(n);
        new_numbers.push(n * 2);  // ✅ Modifies different collection
    }
}
```

---

## 28. Memory Regions

### 28.1 Region Overview

**Definition 28.1 (Memory Region):** A region is a lexically-scoped memory arena enabling fast allocation and O(1) bulk deallocation.

**Benefits:**
- **LIFO deallocation** - Inner regions freed before outer
- **Zero-overhead allocation** - Pointer bump (3-5 CPU cycles vs 50-100 for malloc)
- **Bulk deallocation** - O(1) regardless of allocation count
- **Explicit lifetimes** - Region boundaries visible in code
- **Cache-friendly** - Contiguous allocations

**Key insight:** Regions control WHEN memory is freed, not WHO can access it.

### 28.2 Three Allocation Strategies Compared

**1. Stack (automatic, fastest):**
```cantrip
function compute(x: i32) {
    let local = x * 2;  // Stack allocated automatically
    local
}
```
- **Speed:** Instantaneous (register adjustment)
- **Lifetime:** Function scope
- **Size:** Limited (typically MB range)
- **Use:** Local variables, small temporary data

**2. Heap (explicit, flexible):**
```cantrip
function create_user(name: String): own User
    needs alloc.heap;
{
    own User { name }  // Heap allocated
}
```
- **Speed:** 50-100 CPU cycles
- **Lifetime:** Until explicitly freed
- **Size:** Large (GB range)
- **Use:** Long-lived data, unknown lifetime, return values

**3. Region (explicit, fast bulk deallocation):**
```cantrip
function parse_file(path: str): Result<Data, Error>
    needs alloc.region;
{
    region temp {
        let tokens = lex_in<temp>(read(path));
        let ast = parse_in<temp>(tokens);
        Ok(optimize(ast))
    } // Bulk free: O(1)
}
```
- **Speed:** 3-5 CPU cycles (10-20x faster than heap)
- **Lifetime:** Region scope (LIFO)
- **Size:** Large (configurable)
- **Use:** Temporary data, batch processing, compilation phases

### 28.3 Region Declaration

**Syntax:**
```cantrip
region name {
    // Region body
    // All allocations in this scope use region
}
// Region destroyed here (O(1))
```

**Example:**
```cantrip
function process_data(input: String): Result<Data, Error>
    needs alloc.region;
    requires !input.is_empty();
{
    region temp {
        // All allocations use 'temp' region
        let buffer = Buffer.new_in<temp>(1024);
        let parser = Parser.new_in<temp>();

        let data = parser.parse(buffer, input)?;

        // Must convert to heap before region ends
        Ok(data.to_heap())
    } // All temp memory freed in O(1)
}
```

**Type rule:**
```
[T-Region]
Γ, r : Region ⊢ e : T ! (ε ∪ alloc.region)
────────────────────────────────────────────
Γ ⊢ region r { e } : T ! ε
```

### 28.4 Region Lifetimes

**Regions enforce strict LIFO hierarchy:**

```cantrip
function nested_regions()
    needs alloc.region;
{
    region outer {
            let outer_data = Vec.new_in<outer>();

            region inner {
                let inner_data = Vec.new_in<inner>();

                // CAN reference outer from inner
                outer_data.push(42);

                // CANNOT return inner_data (would escape region)
            } // inner freed here

            // inner_data no longer accessible
            outer_data.push(100);

        } // outer freed here
}
```

**Lifetime rules:**
1. Inner regions deallocate before outer (LIFO)
2. Inner regions CAN reference outer regions
3. Outer regions CANNOT reference inner regions
4. Cannot return region-allocated data

**Formal semantics:**
```
[Region-LIFO]
r₂ nested in r₁
──────────────────────
dealloc(r₂) <ʜᴀᴘᴘᴇɴs-ʙᴇғᴏʀᴇ dealloc(r₁)

[Region-Escape]
alloc(v, r)    e returns v    e in region r
──────────────────────────────────────────
ERROR E5001: Cannot return region data
```

### 28.5 Region Allocation Syntax

**Allocate into specific regions using `_in<region>`:**

```cantrip
region temp {
    // Standard library support
    let vec = Vec.new_in<temp>();
    let string = String.new_in<temp>();
    let buffer = Buffer.with_capacity_in<temp>(1024);

    // Custom types
    let data = Data.new_in<temp>(args);
}
```

**Type rule:**
```
[T-Alloc-In-Region]
Γ, r : Region ⊢ e : T
──────────────────────────────────
Γ ⊢ Type.new_in<r>(e) : T
alloc(result, r)
```

### 28.6 Implementing Region Allocation

**Custom types can support region allocation:**

```cantrip
record Data {
    values: Vec<i32>,
}

impl Data {
    // Heap allocation
    function new(size: usize): own Data
    needs alloc.heap;
{
    own Data {
                values: Vec.with_capacity(size)
            }
    }

    // Region allocation
    function new_in<'r>(size: usize): own Data
        needs alloc.region;
    {
        own Data {
            values: Vec.with_capacity_in<'r>(size)
        }
    }

    // Convert region-allocated to heap
    function to_heap(self: own Data): own Data
        needs alloc.heap;
    {
        self.clone()  // Deep copy to heap
    }
}
```

**Convention:** Types supporting region allocation provide:
- `new_in<'r>()` - Allocate in region
- `to_heap()` - Convert to heap-allocated

### 28.7 Region vs Heap Decision Guide

**When to use heap:**
- Long-lived data
- Return values
- Unknown lifetime
- Data structure with cycles
- Shared ownership needed

**When to use regions:**
- Temporary data
- Batch processing
- Parsing/compilation phases
- Frame-local game data
- Known short lifetime

**Example - Request/response processing:**
```cantrip
function handle_request(request: Request): Response
    needs alloc.region, alloc.heap;
{
    region request_scope {
            // Temporary parsing data
            let parsed = parse_request_in<request_scope>(request);
            let data = fetch_data_in<request_scope>(parsed);

            // Long-lived result escapes region
            Response.from_data(data)  // Heap allocated
        } // Temporary data freed
}
```

### 28.8 Performance Characteristics

**Allocation speed:**
```
malloc/free:  50-100 CPU cycles
Region bump:  3-5 CPU cycles     (10-20x faster)
Stack:        ~1 CPU cycle       (register adjustment)
```

**Deallocation:**
```
Individual free: O(n) for n allocations
Region bulk:     O(1) regardless of n
Stack:           O(1) (automatic)
```

**Memory layout - Arena with exponential pages:**
```
Region: temp
├── Page 1 (4 KB)    [████████████▒▒▒▒] 75% full
├── Page 2 (8 KB)    [██████▒▒▒▒▒▒▒▒▒▒] 30% full
└── Page 3 (16 KB)   [█▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒]  5% full
                      ↑
                      allocation_ptr
```

**Algorithm:**
1. Check current page has space
2. If yes: bump pointer, return (fast path - 3-5 cycles)
3. If no: allocate new page (2x size), add to list
4. Amortized O(1)

### 28.9 Common Patterns

**Request/response processing:**
```cantrip
function handle_request(request: Request): Response
    needs alloc.region;
{
    region request_scope {
            let parsed = parse_request_in<request_scope>(request);
            let data = fetch_data_in<request_scope>(parsed);
            Response.from_data(data)  // Heap allocated
        } // Temporary data freed
}
```

**Batch processing:**
```cantrip
function process_batch(items: Vec<Data>): Vec<Result>
    needs alloc.region;
{
    var results = Vec.new();

    region batch {
        for item in items {
            let temp = transform_in<batch>(item);
            results.push(finalize(temp));
        }
    } // Batch temps freed

    results
}
```

**Parsing/compilation:**
```cantrip
function compile(source: str): Program
    needs alloc.region;
{
    region parsing {
            let tokens = lex_in<parsing>(source);
            let ast = parse_in<parsing>(tokens);

            region analysis {
                let analyzed = analyze_in<analysis>(ast);
                codegen(analyzed)  // Result on heap
            } // Analysis freed
        } // Parsing freed
}
```

**Game engine frame:**
```cantrip
function game_tick(state: mut GameState, dt: f32)
    needs alloc.region;
{
    region frame {
            let entities = update_entities_in<frame>(state, dt);
            let particles = update_particles_in<frame>(state, dt);
            render(entities, particles);
        } // O(1) bulk free - perfect for 60 FPS
}
```

### 28.10 Safety Restrictions

**Cannot escape region:**
```cantrip
function unsafe_example(): Vec<i32>
    needs alloc.region;
{
    region temp {
        let vec = Vec.new_in<temp>();
        vec  // ERROR E5001: Cannot return region data
    }
}
```

**Must convert to heap:**
```cantrip
function safe_example(): Vec<i32>
    needs alloc.region, alloc.heap;
{
    region temp {
        let vec = Vec.new_in<temp>();
        vec.to_heap()  // OK: Converts to heap
    }
}
```

**Permissions still apply:**
```cantrip
function example()
    needs alloc.region;
{
    region temp {
            let data = Data.new_in<temp>();

            // Can pass refs multiple times - no borrow checking
            read(data);
            modify(mut data);
            read(data);
        } // data freed
}
```

---

## 29. Region Formal Semantics

### 29.1 Region Algebra

**Definition 29.1 (Region):** A region r is a tuple (id, allocations, parent).

```
Region = (RegionId, Set<Allocation>, Option<RegionId>)

where:
  RegionId = unique identifier
  Allocation = (address, size, type)
  parent = enclosing region (if nested)
```

**Region hierarchy:**
```
[Region-Hierarchy]
r₁ = (id₁, A₁, None)        // Top-level region
r₂ = (id₂, A₂, Some(id₁))   // Nested region
r₃ = (id₃, A₃, Some(id₂))   // Deeper nesting
```

### 29.2 Allocation Rules

**[Alloc-Heap]**
```
⟨alloc(size), σ, alloc.heap⟩ → ⟨ℓ, σ[ℓ ↦ uninit(size)], alloc.heap⟩
```

**[Alloc-Region]**
```
⟨alloc_in<r>(size), σ, alloc.region⟩ → ⟨ℓ, σ[ℓ ↦ uninit(size), r], alloc.region⟩
```

**[Alloc-Region-Bump]**
```
r.current_page.has_space(size)
────────────────────────────────────
alloc_in<r>(size) = {
    let ptr = r.current_page.ptr;
    r.current_page.ptr += size;
    ptr
}
```

### 29.3 Deallocation Rules

**[Dealloc-Heap]**
```
ℓ ∈ Heap
────────────────────────────
⟨free(ℓ), σ⟩ → σ \ {ℓ}
```

**[Dealloc-Region]**
```
r = (id, allocs, parent)
────────────────────────────────────
⟨end region r, σ⟩ → σ \ allocs    // O(1)
```

**[Dealloc-Region-Nested]**
```
r₂ nested in r₁
dealloc(r₂) happens before dealloc(r₁)
──────────────────────────────────────
LIFO ordering enforced
```

### 29.4 Escape Analysis

**Definition 29.2 (Escape Analysis):** Determine if a value allocated in region r escapes the region's scope.

**Escape rules:**
```
[Escape-Return]
alloc(v, r)    return v in region r
───────────────────────────────────
ERROR: value escapes region

[Escape-Store]
alloc(v, r)    store v in global/heap
──────────────────────────────────────
ERROR: value escapes region

[No-Escape-Local]
alloc(v, r)    v only used within region r
──────────────────────────────────────────
OK: value does not escape
```

**Compiler verification:**
```
check_region_escape(region r, expr e) = {
    for alloc in r.allocations {
        if escapes(alloc, e) {
            error("Cannot return region data");
        }
    }
}

escapes(alloc, e) = {
    return alloc ∈ returned_values(e)
        || alloc ∈ stored_globally(e)
        || alloc ∈ captured_by_closure(e)
}
```

### 29.5 Region Typing

**[T-Region-Alloc]**
```
Γ, r : Region ⊢ e : T
────────────────────────────
Γ ⊢ alloc_in<r>(e) : T@r

where T@r means "T allocated in region r"
```

**[T-Region-Escape]**
```
Γ ⊢ e : T@r    e escapes region r
───────────────────────────────────
Type error
```

**[T-Region-Convert]**
```
Γ ⊢ e : T@r
────────────────────────────
Γ ⊢ e.to_heap() : T@heap
```

### 29.6 Memory Model

**Definition 29.3 (Memory State):** The memory state σ consists of heap, regions, and stack.

```
σ = (Heap, {r₁, r₂, ..., rₙ}, Stack)

where:
  Heap = {(addr, value) | addr ∈ HeapAddrs}
  rᵢ = (id, allocations, parent)
  Stack = {(addr, value, scope) | addr ∈ StackAddrs}
```

**Axiom 29.1 (Region Disjointness):**
```
∀r₁, r₂. r₁ ≠ r₂ ⟹ allocations(r₁) ∩ allocations(r₂) = ∅
```

**Axiom 29.2 (Region LIFO):**
```
r₂ nested in r₁ ⟹ lifetime(r₂) ⊂ lifetime(r₁)
```

**Axiom 29.3 (No Dangling):**
```
alloc(v, r) ∧ dealloc(r) →ʜᴀᴘᴘᴇɴs-ʙᴇғᴏʀᴇ use(v) ⟹ ERROR
```

### 29.7 Happens-Before Relation

**Definition 29.4 (Happens-Before for Regions):**

```
e₁ →ʜʙ e₂  (event e₁ happens before e₂)

Rules:
1. Program order: e₁ <ᴘ e₂ ⟹ e₁ →ʜʙ e₂
2. Region nesting: alloc(v, r₂) ∧ r₂ in r₁ ⟹ alloc(v, r₁) →ʜʙ alloc(v, r₂)
3. Region end: alloc(v, r) →ʜʙ dealloc(r)
4. Transitivity: e₁ →ʜʙ e₂ ∧ e₂ →ʜʙ e₃ ⟹ e₁ →ʜʙ e₃
```

**Memory safety theorem:**

**Theorem 29.1 (Region Memory Safety):**

If alloc(v, r) →ʜʙ dealloc(r) →ʜʙ use(v), then ERROR.

**Proof:** By construction of region semantics and happens-before relation.

---
# Part VIII (Normative): Module System

## 30. Modules and Code Organization

### 30.1 File-Based Module System

**Definition 30.1 (Module):** A module is a namespace unit defined by a source file, where the module path is determined by the file's location in the project structure.

**Key principle:** There is **no `namespace` keyword**. Module structure follows directory structure.

**Project structure:**
```
myproject/
├── arc.toml
└── src/
    ├── main.arc              # Module: main
    ├── http.arc              # Module: http
    ├── math/
    │   ├── geometry.arc      # Module: math.geometry
    │   └── algebra.arc       # Module: math.algebra
    └── data/
        ├── structures.arc    # Module: data.structures
        └── algorithms.arc    # Module: data.algorithms
```

**Formal definition:**
```
Module = (Path, Declarations, Exports)

where:
  Path = sequence of identifiers (e.g., math.geometry)
  Declarations = set of items defined in module
  Exports = subset of Declarations with public visibility
```

### 30.2 Module Definition

**Each file automatically defines a module based on its path:**

```cantrip
// File: src/math/geometry.arc
// Module path: math.geometry

public function area_of_circle(radius: f64): f64
    requires radius > 0.0;
    ensures result >= 0.0;
{
    3.14159 * radius * radius
}

public function area_of_rectangle(width: f64, height: f64): f64
    requires
        width > 0.0;
        height > 0.0;
    ensures result >= 0.0;
{
    width * height
}

function internal_helper(): i32 {  // Not exported
    42
}
```

**Type rule:**
```
[T-Module]
File at path P defines items I₁, ..., Iₙ
────────────────────────────────────────────
Module M = (module_path(P), {I₁, ..., Iₙ}, exports({I₁, ..., Iₙ}))
```

**Path resolution:**
```
module_path(src/main.arc) = main
module_path(src/http.arc) = http
module_path(src/http/client.arc) = http.client
module_path(src/a/b/c/deep.arc) = a.b.c.deep
```

### 30.3 Module Metadata

**Optional metadata using the `#[module]` attribute:**

```cantrip
// File: src/math/geometry.arc

#[module(
    purpose = "Geometric calculations for 2D and 3D shapes",
    version = "1.2.0",
    requires = ["std.math"],
    deprecated = false
)]

public function area_of_circle(radius: f64): f64
    requires radius > 0.0;
    ensures result >= 0.0;
{
    3.14159 * radius * radius
}
```

**Metadata fields:**
- `purpose: str` - Human-readable description
- `version: str` - Semantic version (follows semver)
- `requires: [str]` - Module dependencies
- `deprecated: bool | str` - Deprecation status/message
- `authors: [str]` - Module authors
- `license: str` - License identifier

**Type rule:**
```
[T-Module-Metadata]
#[module(metadata)] precedes module definition
metadata well-formed
────────────────────────────────────────────
Module M has metadata
```

### 30.4 Module Resolution Algorithm

**Definition 30.2 (Module Resolution):** The process of mapping a module path to a source file.

**Algorithm:**
```
resolve_module(path: ModulePath) -> FilePath {
    1. Split path by '.' separator
    2. Join with OS path separator
    3. Prepend 'src/'
    4. Append '.arc'
    5. Check file exists
    6. Return absolute path
}
```

**Examples:**
```
resolve_module("main")
  → src/main.arc

resolve_module("http.client")
  → src/http/client.arc

resolve_module("data.structures.linked_list")
  → src/data/structures/linked_list.arc
```

**Formal specification:**
```
[Module-Resolution]
path = id₁.id₂.....idₙ
file = src/id₁/id₂/.../idₙ.arc
file exists
────────────────────────────────
resolve(path) = file
```

### 30.5 Standard Library Modules

**Standard library uses the `std` prefix:**

```cantrip
import std.io.File;
import std.collections.Vec;
import std.collections.HashMap;
import std.fs.read_to_string;
import std.net.http.Client;
```

**Standard library structure:**
```
std/
├── collections.arc       # std.collections
├── io.arc               # std.io
├── fs.arc               # std.fs
├── string.arc           # std.string
├── net/
│   ├── http.arc         # std.net.http
│   └── tcp.arc          # std.net.tcp
├── sync/
│   ├── mutex.arc        # std.sync.mutex
│   └── channel.arc      # std.sync.channel
└── math.arc             # std.math
```

**Resolution:**
```
resolve_std_module(path: ModulePath) -> FilePath {
    assert!(path.starts_with("std."));
    let relative = path.strip_prefix("std.");
    return stdlib_root / relative.replace('.', '/') + ".arc";
}
```

### 30.6 Modules vs Regions

**Critical distinction:**

| Aspect | Modules | Regions |
|--------|---------|---------|
| **Purpose** | Code organization | Memory management |
| **Lifetime** | Compile-time only | Runtime memory scope |
| **Scope** | File/package level | Function-local blocks |
| **Syntax** | File path (implicit) | `region name { }` |
| **Visibility** | `public`/`private` | N/A (all local) |
| **Imports** | Yes (`import`) | No |
| **Nesting** | Directory hierarchy | Lexical block nesting |

**Example showing both:**
```cantrip
// File: src/parser/lexer.arc
// Module: parser.lexer (from file path)

#[module(
    purpose = "Lexical analysis",
    version = "1.0.0"
)]

public function tokenize(source: str): Vec<Token>
    needs alloc.region;
    requires !source.is_empty();
{
    // REGION: Runtime memory scope
    region temp {
        let chars = source.chars_in<temp>();
        let tokens = Vec.new();

        for ch in chars {
            tokens.push(classify(ch));
        }

        tokens  // Heap-allocated, survives region
    } // temp memory freed here (O(1) bulk deallocation)
}
```

**Key insight:**
- **Modules** organize code spatially (files and directories)
- **Regions** organize memory temporally (creation and destruction order)

---

## 31. Import and Export Rules

### 31.1 Import Syntax

**Definition 31.1 (Import):** An import statement makes items from another module accessible in the current scope.

**Syntax forms:**

**1. Import specific item:**
```cantrip
import math.geometry.area_of_circle;

let area = area_of_circle(5.0);
```

**2. Import all public items:**
```cantrip
import math.geometry.*;

let area = area_of_circle(5.0);
let rect = area_of_rectangle(10.0, 20.0);
```

**3. Import with alias:**
```cantrip
import math.geometry.area_of_circle as circle_area;

let area = circle_area(5.0);
```

**4. Import module:**
```cantrip
import math.geometry;

let area = geometry.area_of_circle(5.0);
```

**Type rules:**

```
[T-Import-Item]
module M exports item I with type T
────────────────────────────────────
import M.I  adds  I : T  to current scope

[T-Import-Wildcard]
module M exports items {I₁ : T₁, ..., Iₙ : Tₙ}
────────────────────────────────────────────────
import M.*  adds  {I₁ : T₁, ..., Iₙ : Tₙ}  to current scope

[T-Import-Alias]
module M exports item I with type T
────────────────────────────────────
import M.I as J  adds  J : T  to current scope
```

### 31.2 Import Resolution

**Definition 31.2 (Import Resolution):** The process of finding and validating imported items.

**Algorithm:**
```
resolve_import(import_stmt: Import) -> Result<Item, Error> {
    1. Parse module path from import
    2. Resolve module path to file
    3. Parse module to get exports
    4. Check imported item is exported
    5. Check visibility permissions
    6. Add to current scope with appropriate name
}
```

**Example:**
```cantrip
import math.geometry.area_of_circle;

// Resolution steps:
// 1. Module path: math.geometry
// 2. File: src/math/geometry.arc
// 3. Parse and extract exports
// 4. Verify area_of_circle is public
// 5. Add to scope: area_of_circle : f64 -> f64
```

**Error cases:**
```cantrip
import math.geometry.nonexistent;
// ERROR E2001: Item 'nonexistent' not found in module 'math.geometry'

import math.geometry.internal_helper;
// ERROR E2002: Item 'internal_helper' is private

import nonexistent.module.item;
// ERROR E2003: Module 'nonexistent.module' not found
```

### 31.3 Re-exports

**Definition 31.3 (Re-export):** A module can re-export items from other modules to create a unified public API.

**Syntax:**
```cantrip
public import source.module.item;
```

**Example:**
```cantrip
// File: src/collections.arc
// Module: collections

// Re-export from other modules
public import data.structures.Vec;
public import data.structures.HashMap;
public import data.structures.HashSet;
public import data.algorithms.sort;

// Now users can do:
// import collections.Vec;
// Instead of:
// import data.structures.Vec;
```

**Type rule:**
```
[T-Reexport]
module M₁ exports item I with type T
module M₂ contains: public import M₁.I
────────────────────────────────────────
M₂ exports I : T
```

**Use cases:**
1. **Facade pattern:** Hide internal organization
2. **Deprecation:** Maintain old paths while reorganizing
3. **Convenience:** Create logical groupings

### 31.4 Import Cycles

**Definition 31.4 (Import Cycle):** A circular dependency where module A imports from B and B imports from A (directly or transitively).

**Detection:**
```
[Cycle-Detection]
M₁ imports M₂, M₂ imports M₃, ..., Mₙ imports M₁
────────────────────────────────────────────────
ERROR: Import cycle detected
```

**Example:**
```cantrip
// File: src/a.arc
import b.function_b;

public function function_a() {
    function_b()
}

// File: src/b.arc
import a.function_a;  // ERROR: Import cycle

public function function_b() {
    function_a()
}
```

**Error message:**
```
error[E2004]: Import cycle detected
  --> src/b.arc:1:1
   |
 1 | import a.function_a;
   | ^^^^^^^^^^^^^^^^^^^^ import creates cycle
   |
   = note: Cycle: a -> b -> a
   = help: Consider using dependency injection or splitting modules
```

**Solutions:**
1. Introduce a third module for shared functionality
2. Restructure dependencies
3. Use dependency injection

### 31.5 Selective Imports

**Import specific items to avoid namespace pollution:**

```cantrip
// Instead of:
import std.collections.*;
let vec = Vec.new();
let map = HashMap.new();

// Prefer:
import std.collections.Vec;
import std.collections.HashMap;
let vec = Vec.new();
let map = HashMap.new();
```

**Rationale:**
- **Clarity:** Explicit about what's used
- **Compile time:** Faster (fewer symbols to resolve)
- **Maintainability:** Easy to see dependencies

### 31.6 Import Ordering

**Convention (not enforced):**
```cantrip
// 1. Standard library
import std.io.File;
import std.collections.Vec;

// 2. External dependencies
import http.Client;
import json.parse;

// 3. Internal modules
import utils.helpers;
import models.User;
```

---

## 32. Visibility and Access Control

### 32.1 Visibility Modifiers

**Definition 32.1 (Visibility):** Visibility determines which modules can access an item.

**Three visibility levels:**

```cantrip
public      // Accessible everywhere
internal    // Accessible within package (default)
private     // Accessible within module only
```

**Type rules:**

```
[Vis-Public]
public item I in module M
M₂ imports M
────────────────────────
M₂ can access I

[Vis-Internal]
internal item I in module M₁
M₂ in same package as M₁
────────────────────────────
M₂ can access I

[Vis-Private]
private item I in module M
────────────────────────────
Only M can access I
```

### 32.2 Public Items

**Public items are accessible from any module:**

```cantrip
// File: src/math/geometry.arc

public record Point {
    public x: f64;
    public y: f64;
}

public function distance(p1: Point, p2: Point): f64 {
    let dx = p1.x - p2.x;
        let dy = p1.y - p2.y;
        (dx * dx + dy * dy).sqrt()
}
```

**Usage from other modules:**
```cantrip
// File: src/main.arc
import math.geometry.Point;
import math.geometry.distance;

function main() {
    let p1 = Point { x: 0.0, y: 0.0 };
    let p2 = Point { x: 3.0, y: 4.0 };
    let d = distance(p1, p2);
}
```

### 32.3 Internal Items

**Internal visibility (default) - accessible within the same package:**

```cantrip
// File: src/utils/helpers.arc

record Config {  // Internal by default
    data: String;
}

function parse_config(source: String): Config {  // Internal
    // Implementation
}
```

**Package boundary:**
```
Package = {all modules under src/}

Same package: src/a.arc and src/b/c.arc
Different package: external dependency
```

**Type rule:**
```
[Vis-Internal-Same-Package]
item I in module M₁
M₂ in same package as M₁
────────────────────────────
M₂ can access I (if internal or public)
```

### 32.4 Private Items

**Private items are only accessible within the defining module:**

```cantrip
// File: src/database/connection.arc

private function validate_credentials(user: str, pass: str): bool {
    // Only accessible within this file
}

public function connect(user: str, pass: str): Result<Connection, Error> {
    if !validate_credentials(user, pass) {
        return Err(Error.InvalidCredentials);
    }
    // ...
}
```

**Error on access:**
```cantrip
// File: src/main.arc
import database.connection.validate_credentials;
// ERROR E2002: Item 'validate_credentials' is private
```

### 32.5 Record Field Visibility

**Fields can have different visibility:**

```cantrip
public record User {
    public name: String;        // Public field
    public email: String;       // Public field
    private password_hash: String;  // Private field
    internal user_id: u64;      // Internal field
}

// In same module
function update_password(user: mut User, hash: String) {
    user.password_hash = hash;  // OK: private access in same module
}

// In different module
function external_access(user: mut User) {
    let name = user.name;  // OK: public
    // let hash = user.password_hash;  // ERROR: private field
}
```

**Type rule:**
```
[Field-Access]
record S { vis f: T }
access to s.f from module M
────────────────────────────
allowed iff visibility(f, M) permits
```

### 32.6 Procedure Visibility

**Procedures inherit record visibility by default but can be more restrictive:**

```cantrip
public record Counter {
    private value: i32;

    public function new(): own Counter {
    own Counter { value: 0 }
    }

    public function increment(self: mut Counter) {
        self.value += 1;
    }

    private function unsafe_set(self: mut Counter, val: i32) {
        self.value = val;  // Private: only for internal use
    }
}
```

### 32.7 Trait Implementation Visibility

**Trait implementations are public by definition:**

```cantrip
trait Drawable {
    function draw(self: Self);
}

record Shape {
    // ...
}

// Implementation is public
impl Drawable for Shape {
    function draw(self: Shape) {
        // ...
    }
}
```

**Rationale:** Traits define public contracts, so implementations must be accessible.

---

## 33. Module Resolution

### 33.1 Resolution Rules

**Definition 33.1 (Module Resolution):** The process of mapping import statements to source files and validating accessibility.

**Resolution algorithm:**
```
resolve(import_path: Path) -> Result<Module, Error> {
    // 1. Parse import path
    let segments = import_path.split('.');

    // 2. Check if standard library
    if segments[0] == "std" {
        return resolve_std_module(segments);
    }

    // 3. Resolve to source file
    let file_path = src_root / segments.join('/') + ".arc";

    // 4. Check file exists
    if !file_path.exists() {
        return Err(ModuleNotFound(import_path));
    }

    // 5. Parse module
    let module = parse_file(file_path)?;

    // 6. Return resolved module
    Ok(module)
}
```

### 33.2 Search Path

**Module search order:**

1. **Current package** (`src/`)
2. **Standard library** (`std/`)
3. **Dependencies** (from `arc.toml`)

**Example:**
```cantrip
import collections.Vec;

// Search order:
// 1. src/collections.arc (or src/collections/vec.arc)
// 2. std/collections.arc
// 3. dependencies/collections/src/vec.arc
```

### 33.3 Ambiguity Resolution

**If multiple modules match:**

```cantrip
// src/data.arc exists
// dependency 'data' also exists

import data.process;

// ERROR E2005: Ambiguous import 'data'
// note: Could refer to:
//   - src/data.arc (local module)
//   - data v1.0 (dependency)
// help: Use full qualification:
//   - ::data.process (local)
//   - extern::data.process (dependency)
```

**Disambiguation syntax:**
```cantrip
import ::data.process;        // Local module
import extern::data.process;  // External dependency
```

### 33.4 Package Configuration

**Package manifest (`arc.toml`):**

```toml
[package]
name = "myproject"
version = "1.0.0"
authors = ["Your Name <you@example.com>"]
edition = "0.4.0"

[dependencies]
http = "2.0"
json = { version = "1.5", features = ["pretty"] }
```

**Dependency resolution:**
```
resolve_dependency(name: str, version: str) -> Path {
    let dep_path = dependencies_dir / name / version;
    return dep_path / "src";
}
```

### 33.5 Module Cache

**Compiler maintains a module cache:**

```
ModuleCache = HashMap<ModulePath, CompiledModule>

where:
  ModulePath = unique identifier
  CompiledModule = (AST, TypeInfo, Exports)
```

**Benefits:**
- Avoid re-parsing same module
- Incremental compilation
- Faster build times

**Cache invalidation:**
- Source file modified
- Dependency updated
- Compiler flags changed

---

# Part IX (Normative): Advanced Features

## 34. Compile-Time Programming


### 34.6 Opt‑In Reflection (Normative)

Cantrip supports *opt‑in* reflection for types explicitly marked as reflective. Reflection is split into
compile‑time introspection (§34.3) and **runtime reflection** gated by effects and attributes.

**Enabling reflection:**
```cantrip
#[reflect]          // opt-in on type
public record User { name: String; id: u64; }
```
If a type lacks `#[reflect]`, runtime reflection on it fails with `E9601`.

**Effect family:**
```cantrip
reflect.read    // read type/field metadata at runtime
reflect.invoke  // invoke methods by name through dynamic dispatch
```
Using runtime reflection requires a `needs` clause including `reflect.read` and/or `reflect.invoke`.

**Runtime API (stdlib sketch):**
```cantrip
module std.reflect

public trait Any { function type_id(self: Self): u128; }

public record Mirror<T> {
    // opaque handle, only constructed for #[reflect] types
}

public function mirror_of<T>(value: T): Result<Mirror<T>, Error>
    needs reflect.read;

impl<T> Mirror<T> {
    public function fields(self: Mirror<T>): Vec<(String, TypeInfo)>
        needs reflect.read;

    public function get(self: Mirror<T>, name: str): Option<Value>
        needs reflect.read;

    public function call(self: Mirror<T>, name: str, args: [Value]): Result<Value, Error>
        needs reflect.invoke;
}
```

**Privacy & safety:**
- Reflection respects visibility: private fields are inaccessible from other modules.
- Reflection does not bypass the effect system; reflective calls contribute the callee’s effects.
- No heap allocation is implied; any allocation must be explicitly declared by the API call sites.

**Diagnostics:**
- `E9601` — reflection on a non‑`#[reflect]` type.
- `E9602` — unknown member in `reflect.get/call`.
- `E9603` — effect mismatch when invoking reflected member.


### 34.1 Comptime Keyword

**Definition 34.1 (Compile-Time Evaluation):** Code within `comptime` blocks executes during compilation.

**Syntax:**
```cantrip
comptime {
    // Executed at compile time
    expression
}
```

**Example:**
```cantrip
function example(): i32 {
    comptime {
            const SIZE = compute_size();
        }

        let arr = [0; SIZE];  // SIZE known at compile time
        arr.length() as i32
}
```

**Type rule:**
```
[T-Comptime]
Γ ⊢ e : T    effects(e) = ∅    e is constant expression
─────────────────────────────────────────────────────────
Γ ⊢ comptime { e } : T
compile_time_eval(e) succeeds
```

### 34.2 Const Functions

**Functions callable at compile time:**

```cantrip
function factorial(n: u32): u64
    requires n <= 20;
{
    if n == 0 { 1 } else { n as u64 * factorial(n - 1) }
}

comptime {
    const FACT_10 = factorial(10);  // Computed at compile time
}
```

**Requirements for const functions:**
1. Pure (no effects)
2. All parameters must be compile-time constants
3. No mutable state
4. Finite recursion depth

### 34.3 Type Introspection

**Query type properties at compile time:**

```cantrip
@typeInfo(T) -> TypeInfo
@typeName(T) -> str
@hasField(T, "field") -> bool
@sizeOf(T) -> usize
@alignOf(T) -> usize
@isNumeric(T) -> bool
@isCopyable(T) -> bool
```

**Example:**
```cantrip
function serialize<T>(value: T): String {
    comptime {
            if @hasField(T, "serialize") {
                return value.serialize();
            } else {
                return default_serialize(value);
            }
        }
}
```

### 34.4 Code Generation

**Generate types and functions at compile time:**

```cantrip
function Array(comptime T: type, comptime N: usize): type {
    record {
        data: [T; N];

        function new(): own Self {
            own Self { data: [T::default(); N] }
        }

        function length(self: Self): usize {
            N
        }
    }
}

// Usage
type IntArray10 = Array(i32, 10);
let arr = IntArray10.new();
```

### 34.5 Macro System

**Hygienic macros for code generation:**

```cantrip
macro vec[$($elem:expr),*] {
    {
        let mut v = Vec.new();
        $(v.push($elem);)*
        v
    }
}

// Usage
let numbers = vec![1, 2, 3, 4, 5];
```

**Macro expansion:**
```cantrip
// vec![1, 2, 3]
// Expands to:
{
    let mut v = Vec.new();
    v.push(1);
    v.push(2);
    v.push(3);
    v
}
```

---

## 35. Concurrency


### 35.5 Structured Concurrency (Normative)

**Motivation:** Unstructured thread spawning easily leaks joins and lifetime management. Cantrip’s standard library
provides a *scope-based* concurrency helper that guarantees all spawned threads are joined before the scope exits.

**API (stdlib):**
```cantrip
module std.concurrent.scope

public function scope<R, F>(f: F): R
    where F: FnOnce(Scope) -> R
    needs thread.spawn, thread.join;

public record Scope { /* opaque token */ }

impl Scope {
    public function spawn<T, F>(self: Scope, f: F): JoinHandle<T>
        where F: FnOnce() -> T + Send
        needs thread.spawn;
}
```

**Semantics:**
1. Every `JoinHandle` created via `scope.spawn` MUST be joined before `scope` returns.
2. On scope exit, if any handle is not joined, the runtime performs a **forced join** and emits `W7800`.
3. If a spawned task panics, `scope` re-raises on join with context.

**Diagnostics:**
- `E7801` — attempt to detach/escape a `JoinHandle` created by `scope.spawn`.
- `W7800` — implicit join at scope end due to missing explicit `join()`.

**Example:**
```cantrip
import std.concurrent.scope;

function parallel_sum(input: [i32]): i64
    needs thread.spawn, thread.join;
{
    scope(|s| {
        let left  = s.spawn(|| sum(&input[0..input.length()/2]));
        let right = s.spawn(|| sum(&input[input.length()/2..]));
        (left.join() + right.join()) as i64
    })
}
```

### 35.1 Thread Spawning

**Create new threads:**

```cantrip
import std.thread;

function example()
    needs thread.spawn, alloc.heap;
{
    let handle = thread.spawn(|| {
            compute_something();
        });

        handle.join();
}
```

**Type rule:**
```
[T-Thread-Spawn]
Γ ⊢ f : () -> T    T : Send
────────────────────────────────────
Γ ⊢ thread.spawn(f) : JoinHandle<T>
```

### 35.2 Message Passing

**Channel-based communication:**

```cantrip
import std.sync.channel;

function example()
    needs thread.spawn, alloc.heap;
{
    let (sender, receiver) = channel.new<Message>();

        thread.spawn(move || {
            sender.send(Message.new());
        });

        let msg = receiver.receive();
}
```

### 35.3 Send and Sync Traits

**Marker traits for thread safety:**

```cantrip
trait Send {
    // Type can be transferred between threads
}

trait Sync {
    // Type can be shared between threads (immutably)
}
```

**Auto-implementation:**
- Primitives: `Send + Sync`
- `own T`: `Send` if `T: Send`
- Default ref: `Sync` if `T: Sync`
- `mut T`: `Send + Sync` if `T: Send + Sync`
- `iso T`: Always `Send`

### 35.4 Atomic Operations

**Lock-free operations:**

```cantrip
import std.sync.atomic.AtomicI32;

record Counter {
    value: AtomicI32,
}

impl Counter {
    function increment(self: Counter): i32
    needs thread.atomic;
{
    self.value.fetch_add(1)
    }
}
```

---

## 36. Actors (First-Class Type)

**Syntax:**

```cantrip
actor Name {
    // private state (record-like fields)
    state {
        field₁: Type₁;
        ...
    }

    // message-handling procedures (may be async)
    procedure handle_msg(self: mut Name, msg: Msg): Result<(), Error>
        needs alloc.heap;

    // optional constructors
    function new(...): own (Name, NameHandle);
}
```

**Type rules (sketch):**
- `actor A` defines the types `A` (the actor) and `AHandle` (the send-only handle).
- Sending requires `where Msg: Send`.
- Actor state is encapsulated; direct mutable access is not exposed outside procedures.

**Note:** Cantrip does not have a built-in `actor` keyword. The actor model can be implemented as a library pattern using existing language features: channels, async/await, and the permission system.

### 36.1 Actor Pattern Overview

**Definition 36.1 (Actor Pattern):** An actor is a record that encapsulates state and processes messages sequentially through a private channel.

**Core components:**
1. **State**: Private record fields (modal state machine optional)
2. **Mailbox**: Private channel for message passing
3. **Message loop**: Async function processing messages sequentially
4. **Handle**: Public trait for sending messages

**Guarantees provided:**
- **Sequential processing**: One message processed at a time per actor
- **Isolation**: No shared mutable state (enforced by permission system)
- **Fault tolerance**: Actor crashes don't affect other actors
- **Location transparency**: Handle can be sent between threads

**Enforced by:**
- **Permission system**: Actor state is `own`, mailbox is private
- **Type system**: Messages must be `Send` to cross thread boundaries
- **Runtime**: Channel provides FIFO ordering

### 36.2 Basic Actor Pattern Implementation

**Message type:**
```cantrip
enum CounterMessage {
    Increment,
    Get(Channel<usize>),
    Shutdown,
}
```

**Actor state:**
```cantrip
record CounterActor {
    count: usize;
    mailbox: Channel<CounterMessage>;
}

impl CounterActor {
    function new(): own (CounterActor, CounterHandle)
        needs alloc.heap;
    {
        let (sender, receiver) = Channel.new();
        let actor = own CounterActor {
            count: 0,
            mailbox: receiver,
        };
        let handle = CounterHandle { sender };
        (actor, handle)
    }

    // Private message processing loop
    async function run(self: mut CounterActor)
        needs alloc.heap;
    {
        loop {
            match self.mailbox.receive().await {
                CounterMessage.Increment => {
                    self.count += 1;
                },
                CounterMessage.Get(reply) => {
                    reply.send(self.count).await;
                },
                CounterMessage.Shutdown => break,
            }
        }
    }
}
```

**Public handle:**
```cantrip
record CounterHandle {
    sender: Channel<CounterMessage>;
}

impl CounterHandle {
    async function increment(self: CounterHandle)
        needs alloc.heap;
    {
        self.sender.send(CounterMessage.Increment).await;
    }

    async function get(self: CounterHandle): usize
        needs alloc.heap;
    {
        let (reply_sender, reply_receiver) = Channel.new();
        self.sender.send(CounterMessage.Get(reply_sender)).await;
        reply_receiver.receive().await
    }
}
```

**Usage:**
```cantrip
function example()
    needs alloc.heap, thread.spawn;
{
    let (actor, handle) = CounterActor.new();

    // Spawn actor on separate task
    thread.spawn(move || {
        actor.run().await;
    });

    // Interact through handle
    handle.increment().await;
    handle.increment().await;
    let count = handle.get().await;
    std.io.println("Count: {}", count);
}
```

### 36.3 Actor Pattern with Modals

Actors can combine with modals for state machine guarantees:

```cantrip
modal FileActorState {
    state Closed { path: String; }
    state Open { fd: FileDescriptor; }

    @Closed >> open() >> @Open
        needs fs.read;
    { /* transition */ }

    @Open >> close() >> @Closed
        needs fs.write;
    { /* transition */ }
}

enum FileMessage {
    Open(Channel<Result<(), Error>>),
    Read(Channel<Result<Data, Error>>),
    Close,
}

record FileActor {
    state: FileActorState;
    mailbox: Channel<FileMessage>;
}

// Implementation follows pattern above
```

### 36.4 Standard Library Support

The standard library provides actor utilities in `std.actor`:

```cantrip
import std.actor.Actor;
import std.actor.Handle;

// Simplified actor creation
let (actor, handle) = Actor.spawn(initial_state, message_handler);
```

See standard library documentation for details.

---

## 37. Async/Await



### 37.4 Async Iteration (Normative)

**Trait:** Asynchronous iteration yields items over time via an `async next` method.

```cantrip
public trait AsyncIterator {
    type Item;

    async function next(self: mut Self): Option<Self.Item>
        needs effects(Self);
}
```

**Typing rule:**
```
[T-Async-Iterator]
trait AsyncIterator { type Item; async fn next(Self) -> Option<Item>; }
Γ ⊢ T : AsyncIterator
──────────────────────────────────────────────
Γ ⊢ T.next() : Future<Option<T.Item>>
```

**Consumption pattern:**
```cantrip
async function consume<I>(iter: I)
    where I: AsyncIterator
    needs effects(I);
{
    while let Some(item) = await iter.next() {
        process(item);
    }
}
```

**Syntactic sugar (optional):** Implementations MAY provide `for await item in iter { ... }` desugaring to the
`while`/`await next()` form above. If provided, `for await` requires the enclosing function to be `async` and
imports the iterator’s effects into the enclosing effect mask (see §22.7).

### 37.5 Async Effect Masks (Informative)

Tooling guidance: IDEs and LLMs SHOULD surface missing `needs` entries on an `await` site as soon as
the awaited expression is known to perform `net.*` or `time.sleep`. See §22.7 for the normative rule.


### 37.1 Async Functions

**Define asynchronous functions:**

```cantrip
async function fetch_data(url: str): Result<Data, Error>
    needs net.read(outbound), alloc.heap;
    requires !url.is_empty();
{
    let response = await http.get(url)?;
    let data = await response.body()?;
    Ok(data)
}
```

**Type rule:**
```
[T-Async]
Γ ⊢ e : T ! ε
────────────────────────────────
Γ ⊢ async function f() : Future<T> ! ε
```

### 37.2 Await Expressions

**Wait for async operations:**

```cantrip
function example(): Result<(), Error>
    needs net.read(outbound), alloc.heap;
{
    let data = await fetch_data("https://api.example.com")?;
    process(data);
    Ok(())
}
```

### 37.3 Select Expression

**Wait on multiple futures:**

```cantrip
function fetch_with_timeout(url: str): Result<Data, Error>
    needs net.read(outbound), time.read, alloc.heap;
    requires !url.is_empty();
{
    select {
        case data = await http.get(url): Ok(data),
        case _ = await timer(5.seconds): Err(Error.Timeout),
    }
}
```

**Semantics:** First future to complete wins.

---


## 55. Machine‑Readable Output (Normative)

Compilers and tools MUST provide a machine‑readable diagnostics stream to support IDEs and LLMs.

**Format:** JSON Lines (one JSON object per diagnostic).

**Schema (stable fields):**
```json
{
  "version": "0.7.1",
  "severity": "error|warning|note",
  "code": "E9001",
  "message": "Missing effect declaration: alloc.heap",
  "spans": [
    {"file":"src/main.ctr","from": {"line":5,"col":9},"to":{"line":5,"col":16}}
  ],
  "fixIts": [
    {"title":"Add 'needs alloc.heap;'", "edits":[{"file":"src/main.ctr","insertAfterLine":2,"text":"    needs alloc.heap;\n"}]}
  ],
  "context": {
    "effectMask": ["net.read(outbound)"],  // from §22.7 async mask
    "typedHole": {"name":"n","type":"String"}, // see §14.10
    "aliases": ["to_string","stringify"]  // from #[alias], §2.6
  }
}
```

**Stream guarantees:**
- Stable `code` identifiers (Appendix C).
- All primary spans MUST include file/line/column.
- Fix‑its MUST be localized textual edits with no hidden side effects.

**Compiler switches:**
```
--json                         emit JSONL diagnostics to stdout
--json-file=PATH               write JSONL to file
--json-emit-aliases            include alias metadata (see §2.6)
--json-emit-effect-mask        include async effect mask (see §22.7)
```




# Part XIV (Normative): Foreign Function Interface (FFI)

## 56. Foreign Function Interface Overview

The FFI enables calls between Cantrip and external languages (notably C). It defines symbol linkage,
type layouts, calling conventions, and ownership boundaries. FFI is **unsafe by nature**; effect `ffi.call`
and, for raw pointers, `unsafe.ptr` MUST be declared (§21.10).

## 57. Declarations and Linkage

**Extern blocks:**
```cantrip
extern "C" {
    function memcpy(dest: *u8, src: *u8, n: usize): *u8
        needs ffi.call, unsafe.ptr;
}
```

**Linkage names:** Use `extern "C"` for C ABI. Mangling is suppressed for `extern "C"` items.
Library linkage is configured in the build manifest.

## 58. Type Mappings

C ↔ Cantrip canonical mappings (subset):
- `int8_t` ↔ `i8`, `uint8_t` ↔ `u8`, …
- `float` ↔ `f32`, `double` ↔ `f64`
- `char*` ↔ `*u8` (pointer to bytes), length‑managed by caller/callee convention
- `struct` ↔ `#[repr(C)] record` (§6.5)
- Enums with explicit `#[repr(IntType)]` when interoperating with C integer enums

Pointers are raw; dereferencing requires `unsafe` section and `unsafe.ptr` effect.

## 59. Ownership and Allocation Across FFI

**Principle:** The side that allocates owns the memory and must free it using the same allocator family.
Crossing the boundary requires explicit transfer semantics or copy.

**Patterns:**
- **Borrow:** Pass `*const T`/`*mut T` with lifetime limited to the call.
- **Take & return:** `own T` transferred as an opaque pointer handle; provide destroy callbacks.
- **Copy:** Convert to POD or heap‑owned data on one side, copy back results.

## 60. Callbacks from C into Cantrip

C may call back into Cantrip via function pointers. Such callbacks are declared `extern "C"`
and must specify effects explicitly. Re‑entrancy MUST be documented by the callee.

## 61. Errors and Panics

FFI functions MUST NOT unwind across the boundary. Cantrip panics are trapped and converted to error
codes or nulls as specified by the binding. Conversely, C long‑jumps must not cross into Cantrip frames.

## 62. Inline Assembly (Reserved)

Inline assembly is reserved in 1.0. Implementations MAY provide experimental features behind flags,
but no normative syntax is defined.


# Part X (Normative): Operational Semantics

## 38. Small-Step Semantics

### 38.1 Small-Step Reduction

**Definition 38.1 (Small-Step Semantics):** Single-step reduction relation ⟨e, σ, ε⟩ → ⟨e', σ', ε'⟩.

**Core rules:**

**[E-App-β]**
```
─────────────────────────────────────────────
⟨(λx:T. e) v, σ, ε⟩ → ⟨e[x ↦ v], σ, ε⟩
```

**[E-Let-Val]**
```
────────────────────────────────────────
⟨let x = v in e, σ, ε⟩ → ⟨e[x ↦ v], σ, ε⟩
```

**[E-If-True]**
```
────────────────────────────────────────
⟨if true then e₁ else e₂, σ, ε⟩ → ⟨e₁, σ, ε⟩
```

**[E-If-False]**
```
────────────────────────────────────────
⟨if false then e₁ else e₂, σ, ε⟩ → ⟨e₂, σ, ε⟩
```

### 38.2 Evaluation Contexts

**Definition 38.2 (Evaluation Context):** Contexts defining evaluation order.

```
E ::= []                    (hole)
    | E e                   (left of application)
    | v E                   (right of application)
    | let x = E in e        (let binding)
    | E op e₂               (left of operation)
    | v₁ op E               (right of operation)
    | if E then e₁ else e₂  (condition)
```

**Context rule:**
```
[E-Context]
⟨e, σ, ε⟩ → ⟨e', σ', ε'⟩
─────────────────────────────
⟨E[e], σ, ε⟩ → ⟨E[e'], σ', ε'⟩
```

---

## 39. Big-Step Semantics

### 39.1 Big-Step Evaluation

**Definition 39.1 (Big-Step Semantics):** Multi-step evaluation relation ⟨e, σ⟩ ⇓ ⟨v, σ'⟩.

**[Eval-Val]**
```
─────────────────
⟨v, σ⟩ ⇓ ⟨v, σ⟩
```

**[Eval-Let]**
```
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ⟨e₂[x ↦ v₁], σ₁⟩ ⇓ ⟨v₂, σ₂⟩
──────────────────────────────────────────────────
⟨let x = e₁ in e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩
```

**[Eval-App]**
```
⟨e₁, σ⟩ ⇓ ⟨λx:T. e, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩    ⟨e[x ↦ v₂], σ₂⟩ ⇓ ⟨v, σ₃⟩
────────────────────────────────────────────────────────────────────────────────
⟨e₁ e₂, σ⟩ ⇓ ⟨v, σ₃⟩
```

---

## 40. Memory Model

### 40.1 Memory State

**Definition 40.1 (Memory State):**

```
σ = (Heap, {r₁, r₂, ..., rₙ}, Stack)

where:
  Heap = {(addr, value) | addr ∈ HeapAddrs}
  rᵢ = (id, allocations, parent)
  Stack = {(addr, value, scope) | addr ∈ StackAddrs}
```

### 40.2 Happens-Before

**Definition 40.2 (Happens-Before):**

```
→ₕᵦ = (<ₚ ∪ ≺ₛ)⁺  (transitive closure)

where:
  eâ‚ <ₚ e₂  iff  e₁ before e₂ in program order
  e₁ ≺ₛ e₂  iff  e₁ synchronizes-with e₂
```

---

## 41. Evaluation Order

### 41.1 Left-to-Right Evaluation

**Axiom 41.1:** Arguments evaluate left-to-right.

```
f(e₁, e₂, e₃)  evaluates:
  1. e₁
  2. e₂
  3. e₃
  4. f(v₁, v₂, v₃)
```

### 41.2 Short-Circuit Evaluation

**Logical operators:**
```
false && e  →  false  (e not evaluated)
true || e   →  true   (e not evaluated)
```

---

# Part XI (Normative): Soundness and Properties

## 42. Type Soundness

**Theorem 42.1 (Progress):**

If Γ ⊢ e : T and e closed, then either:
1. e is a value, or
2. ∃e', σ, σ'. ⟨e, σ⟩ → ⟨e', σ'⟩

**Theorem 42.2 (Preservation):**

If Γ ⊢ e : T and ⟨e, σ⟩ → ⟨e', σ'⟩, then Γ ⊢ e' : T.

---

## 43. Effect Soundness

**Theorem 43.1 (Effect Approximation):**

If Γ ⊢ e : T ! ε and ⟨e, σ, ∅⟩ →* ⟨v, σ', ε'⟩, then ε' ⊆ ε.

---

## 44. Memory Safety

**Theorem 44.1 (No Use-After-Free):**

If ⟨e, σ⟩ →* ⟨e', σ'⟩ and e' accesses ℓ, then ℓ ∈ dom(σ').

---

## 45. Modal Safety

**Theorem 45.1 (State Invariant Preservation):**

If Γ, Σ ⊢ e : P@S and σ ⊨ I(S) and ⟨e, σ⟩ →* ⟨v, σ'⟩, then σ' ⊨ I(S).

---

# Part XII: Standard Library (See Separate Document)

**The Cantrip Standard Library specification has been moved to a separate document for better organization and maintainability.**

**📄 See:** `Cantrip-Standard-Library-Spec.md`

## Overview

The standard library provides:
- **Core Types:** Option, Result, String, Vec, Box
- **Collections:** Vec, HashMap, HashSet
- **I/O:** File operations, standard streams, file system utilities
- **Networking:** HTTP client, TCP sockets
- **Concurrency:** Mutex, channels, atomic types, structured concurrency

For complete API documentation, effect declarations, complexity guarantees, and usage examples, refer to the **Cantrip Standard Library Specification v1.0.0**.

---

## Quick Reference

### Core Modules

```
std                     # Core types (Option, Result, String)
std.collections         # Vec, HashMap, HashSet
std.io                  # Input/output
std.fs                  # File system operations
std.net                 # Networking
std.net.http            # HTTP client/server
std.net.tcp             # TCP sockets
std.sync                # Synchronization primitives
std.thread              # Threading
std.concurrent.scope    # Structured concurrency
```

### Canonical API Names

| Operation | Correct Name |
|-----------|-------------|
| Get size | `.length()` |
| Add item | `.push(item)` |
| Remove last | `.pop()` |
| Check empty | `.is_empty()` |
| Check contains | `.contains(item)` |

**Note:** Using alternative names (e.g., `.size()`, `.len()`, `.append()`) will result in compilation errors.

### Effect Families

```
alloc.*     # Allocation (heap, stack, region, temp)
fs.*        # File system (read, write, delete, metadata)
net.*       # Networking (read, write)
io.*        # Console I/O (write)
thread.*    # Threading (spawn, join, atomic)
```

**For complete documentation, see `Cantrip-Standard-Library-Spec.md`.**

---

## 46. [REMOVED - See Standard Library Specification]

### 46.1 [REMOVED - See Standard Library Specification]

### 46.2 [REMOVED - See Standard Library Specification]

### 46.3 [REMOVED - See Standard Library Specification]

### 46.4 [REMOVED - See Standard Library Specification]

---

## 47. [REMOVED - See Standard Library Specification]

### 47.1 [REMOVED - See Standard Library Specification]

### 47.2 [REMOVED - See Standard Library Specification]

### 47.3 [REMOVED - See Standard Library Specification]

---

## 48. [REMOVED - See Standard Library Specification]

### 48.1 [REMOVED - See Standard Library Specification]

### 48.2 [REMOVED - See Standard Library Specification]

### 48.3 [REMOVED - See Standard Library Specification]

---

## 49. [REMOVED - See Standard Library Specification]

### 49.1 [REMOVED - See Standard Library Specification]

### 49.2 [REMOVED - See Standard Library Specification]

---

## 50. [REMOVED - See Standard Library Specification]

### 50.1 [REMOVED - See Standard Library Specification]

### 50.2 [REMOVED - See Standard Library Specification]

### 50.3 [REMOVED - See Standard Library Specification]

---

# Part XIII: Tooling and Implementation (See Separate Document)

**The complete tooling and implementation guide has been moved to `Cantrip-Tooling-Guide.md`.**

For comprehensive documentation including:
- Compiler architecture and compilation pipeline
- Compiler invocation and options
- Optimization levels
- Verification modes
- Incremental compilation
- Error reporting and machine-readable output
- Package management (`arc.toml`, commands, dependency resolution)
- Testing framework (unit tests, integration tests, property-based testing, benchmarks)

**📄 See:** `Cantrip-Tooling-Guide.md`

### Quick Reference

**Compiler invocation:**
```bash
arc compile main.arc --opt=2 --verify=runtime
```

**Package management:**
```bash
arc new myproject
arc build --release
arc test
```

**For complete documentation, see `Cantrip-Tooling-Guide.md`.**

---

# Appendices (Informative)

## Appendix A: Complete Grammar

### A.1 Lexical Grammar

```ebnf
(* Comments *)
LineComment ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" any)* "*/"
DocComment ::= "///" ~[\n\r]*
ModuleDoc ::= "//!" ~[\n\r]*

(* Identifiers *)
Identifier ::= IdentStart IdentContinue*
IdentStart ::= [a-zA-Z_]
IdentContinue ::= [a-zA-Z0-9_]

(* Literals *)
IntegerLiteral ::= DecimalLiteral | HexLiteral | BinaryLiteral | OctalLiteral
DecimalLiteral ::= [0-9] [0-9_]*
HexLiteral ::= "0x" [0-9a-fA-F] [0-9a-fA-F_]*
BinaryLiteral ::= "0b" [01] [01_]*
OctalLiteral ::= "0o" [0-7] [0-7_]*

FloatLiteral ::= DecimalLiteral "." DecimalLiteral Exponent?
Exponent ::= [eE] [+-]? DecimalLiteral

BooleanLiteral ::= "true" | "false"

CharLiteral ::= "'" (EscapeSequence | ~['\\]) "'"
StringLiteral ::= '"' (EscapeSequence | ~["\\])* '"'
EscapeSequence ::= "\\" [nrt\\'"0] | "\\x" HexDigit HexDigit | "\\u{" HexDigit+ "}"
```

### A.2 Expression Grammar

```ebnf
Expression ::= PrimaryExpression
             | BinaryExpression
             | UnaryExpression
             | IfExpression
             | MatchExpression
             | WhileExpression
             | ForExpression
             | LoopExpression

PrimaryExpression ::= Literal
                    | Identifier
                    | "(" Expression ")"
                    | BlockExpression
                    | FunctionCall
                    | FieldAccess
                    | IndexExpression

BinaryExpression ::= Expression BinaryOp Expression
UnaryExpression ::= UnaryOp Expression

IfExpression ::= "if" Expression BlockExpression ("else" (BlockExpression | IfExpression))?

MatchExpression ::= "match" Expression "{" MatchArm* "}"
MatchArm ::= Pattern "->" Expression ","

WhileExpression ::= "while" Expression BlockExpression
ForExpression ::= "for" Pattern "in" Expression BlockExpression
LoopExpression ::= "loop" BlockExpression
```

### A.3 Statement Grammar

```ebnf
Statement ::= LetStatement
            | VarStatement
            | ExpressionStatement
            | ReturnStatement
            | BreakStatement
            | ContinueStatement

LetStatement ::= "let" Pattern (":" Type)? "=" Expression ";"
VarStatement ::= "var" Identifier (":" Type)? "=" Expression ";"
ExpressionStatement ::= Expression ";"
ReturnStatement ::= "return" Expression? ";"
BreakStatement ::= "break" Expression? ";"
ContinueStatement ::= "continue" ";"
```

### A.4 Type Grammar

```ebnf
Type ::= PrimitiveType
       | ArrayType
       | SliceType
       | TupleType
       | FunctionType
       | ProtocolType
       | GenericType

PrimitiveType ::= "i8" | "i16" | "i32" | "i64" | "isize"
                | "u8" | "u16" | "u32" | "u64" | "usize"
                | "f32" | "f64"
                | "bool" | "char" | "str"

ArrayType ::= "[" Type ";" IntegerLiteral "]"
SliceType ::= "[" Type "]"
TupleType ::= "(" Type ("," Type)* ")"
FunctionType ::= Type "->" Type
ProtocolType ::= Type "@" StateIdentifier
GenericType ::= Identifier "<" Type ("," Type)* ">"
```

### A.5 Item Grammar

```ebnf
Item ::= FunctionItem
       | StructItem
       | EnumItem
       | ProtocolItem
       | InterfaceItem
       | ImplItem
       | EffectItem
       | ModuleMetadata

FunctionItem ::=
    "function" Identifier GenericParams? "(" Parameters ")" ReturnType
    NeedsClause?
    RequiresClause?
    EnsuresClause?
    FunctionBody

Parameters ::= Parameter ("," Parameter)*
Parameter ::= Identifier ":" Type

ReturnType ::= ":" Type

NeedsClause ::= "needs" EffectList ";"
EffectList ::= Effect ("," Effect)*
Effect ::= EffectName
         | EffectName "." "*"
         | EffectName "(" EffectArgs ")"
         | Identifier

RequiresClause ::= "requires" AssertionList ";"
EnsuresClause ::= "ensures" AssertionList ";"
AssertionList ::= Assertion ("," Assertion)*
Assertion ::= Expression

FunctionBody ::= "{" Statement* Expression? "}"

ModuleMetadata ::= "#[module" "(" MetadataFields ")" "]"

MetadataFields ::= MetadataField ("," MetadataField)*

MetadataField ::= Identifier "=" StringLiteral
                | Identifier "=" BooleanLiteral
                | Identifier "=" ArrayLiteral

ArrayLiteral ::= "[" StringList "]"

StringList ::= StringLiteral ("," StringLiteral)*

StructItem ::= "record" Identifier GenericParams? "{" FieldList "}"
EnumItem ::= "enum" Identifier GenericParams? "{" VariantList "}"
ProtocolItem ::= "modal" Identifier "{" ProtocolBody "}"
InterfaceItem ::= "trait" Identifier GenericParams? "{" InterfaceBody "}"
ImplItem ::= "impl" GenericParams? Type ("for" Type)? "{" ImplBody "}"

EffectItem ::= Visibility? "effect" Identifier "=" EffectExpr ";"

EffectExpr ::= EffectTerm
             | EffectExpr "+" EffectTerm
             | EffectExpr "-" EffectTerm

EffectTerm ::= PrimitiveEffect
             | "!" EffectTerm
             | "(" EffectExpr ")"
             | Identifier

PrimitiveEffect ::= EffectName
                  | EffectName "." "*"
                  | EffectName "(" EffectArgs ")"
```

---

## Appendix B: Keywords and Operators

### B.1 Reserved Keywords

**All reserved keywords (cannot be used as identifiers):**


```
abstract    actor       as          async       await
break       case        comptime    const       continue
defer       effect      else        ensures     enum
exists      false       for         forall      function
if          impl        import      internal    invariant
iso         let         loop        match       modal
module      move        mut         needs       new
none        own         private     protected   procedure
public      record      ref         region      requires
result      select      self        Self        state
static      trait       true        type        var
where       while
```


### B.2 Operators

**Arithmetic operators:**
```
+   -   *   /   %
+=  -=  *=  /=  %=
```

**Comparison operators:**
```
==  !=  <   <=  >   >=
```

**Logical operators:**
```
&&  ||  !
```

**Bitwise operators:**
```
&   |   ^   <<  >>
```

**Special operators:**
```
->   // Function type constructor
=>   // Pipeline operator
..   // Exclusive range
..=  // Inclusive range
@    // Modal state marker
?    // Error propagation
```

**Effect algebra operators (in effect expressions only):**
```
+    // Effect union (e.g., fs.read + fs.write)
-    // Effect difference (e.g., WebOps - fs.delete)
!    // Effect negation (e.g., !net.*)
```

**Note:** The `+`, `-`, and `!` operators have different meanings in effect expressions versus arithmetic/logical expressions, disambiguated by context.

---

## Appendix C: Error Codes (See Separate Document)

**The complete error codes reference has been moved to `Cantrip-Error-Codes.md`.**

For comprehensive documentation including:
- Complete error code listings by category
- Detailed descriptions and examples
- Suggested fixes for common errors
- LLM-specific error guidance

**📄 See:** `Cantrip-Error-Codes.md`

### Quick Reference

**Error Code Categories:**
- **E1xxx:** Syntax errors
- **E2xxx:** Type errors
- **E3xxx:** Modal state errors
- **E4xxx:** Contract errors
- **E5xxx:** Region/Lifetime errors
- **E9xxx:** Effect system errors

**Common Errors:**
- `E2001`: Type mismatch
- `E3003`: Procedure not available in current state
- `E4001`: Precondition violation
- `E5002`: Reference escapes region scope
- `E9001`: Missing effect declaration

**For complete documentation, see `Cantrip-Error-Codes.md`.**

---

## Appendix D: Standard Library Index (See Separate Document)

**The complete standard library index has been moved to `Cantrip-Standard-Library-Spec.md`.**

For comprehensive documentation including:
- Complete module index
- Canonical API names
- Effect families
- Standard effect definitions
- Complexity guarantees
- Usage patterns and examples

**📄 See:** `Cantrip-Standard-Library-Spec.md`

### Quick Reference

**Core Modules:**
```
std, std.collections, std.io, std.fs, std.net, std.sync, std.thread
```

**Canonical API Names:**
- Get size: `.length()` (NOT `.size()` or `.len()`)
- Add item: `.push(item)` (NOT `.append()` or `.add()`)
- Check empty: `.is_empty()` (NOT `.empty()`)

**Effect Families:**
```
alloc.*, fs.*, net.*, io.*, thread.*, time.*, process.*, ffi.*, unsafe.*
```

**For complete documentation, see `Cantrip-Standard-Library-Spec.md`.**

---

## Appendix E: Formal Proofs

### E.1 Type Soundness Proof (Sketch)

**Theorem E.1 (Progress):**
```
If Γ ⊢ e : T and e is closed, then either:
  1. e is a value, or
  2. ∃e', σ, σ'. ⟨e, σ⟩ → ⟨e', σ'⟩
```

**Proof (by induction on typing derivation):**

*Base cases:*
- Variables: Cannot occur (e is closed)
- Values: Satisfy case 1

*Inductive cases:*
- Application: By IH, e₁ reduces to λx.e or e₁ → e₁'
- Let: By IH, e₁ reduces or is value
- If: By IH, condition reduces or is boolean
- (Other cases similar)

∎

**Theorem E.2 (Preservation):**
```
If Γ ⊢ e : T and ⟨e, σ⟩ → ⟨e', σ'⟩, then Γ ⊢ e' : T
```

**Proof (by induction on reduction):**

*Case [E-App-β]:*
```
⟨(λx:T₁. e) v, σ⟩ → ⟨e[x ↦ v], σ⟩

By inversion of T-App:
  Γ ⊢ λx:T₁. e : T₁ -> T
  Γ ⊢ v : T₁

By inversion of T-Abs:
  Γ, x : T₁ ⊢ e : T

By substitution lemma:
  Γ ⊢ e[x ↦ v] : T
```

*Other cases: Similar*

∎

### E.2 Effect Soundness Proof (Sketch)

**Theorem E.3 (Effect Approximation):**
```
If Γ ⊢ e : T ! ε and ⟨e, σ, ∅⟩ →* ⟨v, σ', ε'⟩, then ε' ⊆ ε
```

**Proof (by induction on evaluation):**

The declared effects ε form an upper bound on actual effects ε'.

*Base case:* Values have no effects, ∅ ⊆ ε

*Inductive case:*
- Effect composition preserves bounds
- Each reduction step adds only declared effects

∎

### E.3 Modal Safety Proof (Sketch)

**Theorem E.4 (State Invariant Preservation):**
```
If Γ, Σ ⊢ e : P@S and σ ⊨ I(S) and ⟨e, σ⟩ →* ⟨v, σ'⟩,
then σ' ⊨ I(S)
```

**Proof:**
By induction on state transitions, each transition preserves state invariants by construction.

∎

---

## Bibliography

1. **Cyclone: A Safe Dialect of C**
   - Grossman et al., USENIX 2002
   - Basis for region-based memory management

2. **Typestate: A Programming Language Concept**
   - Strom & Yemini, IEEE TSE 1986
   - Foundation for modal state machines

3. **Practical Affine Types**
   - Tov & Pucella, POPL 2011
   - Linear type system without full linearity

4. **Effect Systems**
   - Nielson & Nielson, 1999
   - Systematic treatment of computational effects

5. **Hoare Logic**
   - Hoare, CACM 1969
   - Foundation for contract-based reasoning

6. **Semantic Subtyping**
   - Frisch et al., POPL 2008
   - Type system foundations

---

## Revision History

**Version 0.5.0** - December 2024
- Corrected language name from "Cantrip" to "Cantrip" throughout
- Standardized all function syntax to canonical form (needs/requires/ensures)
- Removed deprecated syntax (requires<>/returns<>/implements:)
- Converted actor system to library pattern recommendation
- Added formal effect taxonomy specification with standard definitions
- Updated grammar to match canonical syntax
- Corrected keyword lists to include effect, ensures, and needs
- Fixed error codes to mark removed syntax
- Resolved all internal inconsistencies
- Fixed chronologically impossible dates in revision history

**Version 0.4.0** - December 2024
- Complete unified specification
- Formal semantics integrated
- Modal system fully specified
- Effect system formalized
- Memory regions detailed
- Standard library documented

**Version 0.3.0** - November 2024
- Modal system introduced
- Effect budgets added
- Contract fuzzing specified

**Version 0.2.1** - October 2024
- Removed borrow checking
- Clarified permission model
- Enhanced region semantics

**Version 0.1** - September 2024
- Initial release

---

**END OF SPECIFICATION**

© 2025 Cantrip Language Project
Licensed under MIT

---

# Appendix Z (Informative): Coverage Checklist — Mapping from Original to Revised Sections

The following checklist maps each original top-level section to its location in this revised specification.
Since headings and numbering have been preserved (with explicit **Normative/Informative** tags added),
most sections map 1:1. Any editorial clarifications are additive.

- **Part 0: LLM Quick Reference** → **Moved to `Cantrip-LLM-Guide.md`**
- **Part I: Foundational Concepts** → **Part I (Normative): Foundational Concepts**
- **Part II: Type System** → **Part II (Normative): Type System**
- **Part III: Modal System** → **Part III (Normative): Modal System**
- **Part IV: Functions and Expressions** → **Part IV (Normative): Functions and Expressions**
- **Part V: Contract System** → **Part V (Normative): Contract System**
- **Part VI: Effect System** → **Part VI (Normative): Effect System**
  - Note on §21 sub-numbering: See the editorial note at the start of §21. All content from §21.2–§21.11,
    including previously duplicated §21.9.* material, is retained. Canonical taxonomy is §21.11.
- **Part VII: Memory Management** → **Part VII (Normative): Memory Management**
- **Part VIII: Module System** → **Part VIII (Normative): Module System**
- **Part IX: Advanced Features** → **Part IX (Normative): Advanced Features**
- **Part X: Operational Semantics** → **Part X (Normative): Operational Semantics**
- **Part XI: Soundness and Properties** → **Part XI (Normative): Soundness and Properties**
- **Part XII: Standard Library** → **Part XII (Normative): Standard Library**
- **Part XIII: Tooling and Implementation** → **Part XIII (Normative): Tooling and Implementation**
- **Appendices** → **Appendices (Informative)**

**Status of examples and quick references:** All examples, anti-examples, and LLM-oriented guidance are preserved
and explicitly marked as **(Informative)**; all rules, typing judgments, and formal definitions remain **(Normative)**.



---


# Part XIV (Normative): Foreign Function Interface (FFI)

## 56. Overview

The FFI enables interoperable calls between Cantrip and foreign languages with a C‑compatible ABI.
FFI calls are **unsafe** and gated by effects.

- Effect atoms: `ffi.call`, `unsafe.ptr`, `process.spawn` (when launching external tools)
- Layout attributes: `#[repr(C)]`, `#[repr(packed)]`, `#[repr(align(N))]`

## 57. Declarations and Linkage

**Extern declarations:**
```cantrip
extern "C" {
    function puts(s: *const u8): i32
        needs ffi.call, unsafe.ptr;
}
```

**Link metadata:**
```cantrip
#[link(name = "m")]   // link against libm
extern "C" {
    function cos(x: f64): f64
        needs ffi.call;
}
```

**Rules:**
1. Only `extern "C"` is specified by this version. Other ABIs are implementation‑defined.
2. Extern functions implicitly have `unsafe` pointer types; callers MUST provide valid pointers.
3. Effects: every `extern` call contributes `ffi.call`; raw pointer deref requires `unsafe.ptr`.

**Diagnostics:**
- `E9701` — calling `extern` function without `needs ffi.call`.
- `E9702` — passing an invalid or non‑`repr(C)` aggregate across FFI boundary.
- `E9703` — mismatched calling convention or variadic use.

## 58. Type Mappings

**Scalars:** one‑to‑one with C fixed‑width types.
```
i8  <-> int8_t      u8  <-> uint8_t
i16 <-> int16_t     u16 <-> uint16_t
i32 <-> int32_t     u32 <-> uint32_t
i64 <-> int64_t     u64 <-> uint64_t
f32 <-> float       f64 <-> double
bool (u8) <-> _Bool (0/1)
```

**Pointers:**
```
*T        <-> T*
*const T  <-> const T*
*mut T    <-> T*
```
**Aggregates:** Use `#[repr(C)]` records/enums (§6.5, §7.4).

## 59. Ownership and Allocation Across FFI

- A value allocated by Cantrip and returned to C must be freed by a Cantrip‑provided `extern "C"` free function.
- C‑allocated memory handed to Cantrip must specify its ownership policy; raw pointers are `unsafe.ptr`.
- Region‑allocated data MUST NOT cross FFI boundaries (`E9704`).

## 60. Callbacks from C into Cantrip

**Callback type:**
```cantrip
#[repr(C)]
public type CCallback = extern "C" fn(*mut u8): i32;
```
Callbacks re‑entering Cantrip may perform effects as declared by the receiving function. The callee’s
`needs` clause governs the allowed effects.

## 61. Errors and Panics

- Panics MUST NOT unwind across the FFI boundary. Implementations SHOULD abort or translate to error codes.
- Use `Result<T, E>` wrappers in Cantrip and map to C return codes as appropriate.

## 62. Inline Assembly (Reserved)

Inline assembly is reserved for a future version and is not specified here.

---

## Appendix F: Common LLM Mistakes (See Separate Document)

**The complete index of error codes by common LLM mistakes has been moved to \Cantrip-LLM-Guide.md\.**

For comprehensive guidance including:
- Missing effects (E9001, E9201)
- Ownership & moves (E3004, E5001)
- Modals & states (E3003)
- Traits & coherence (E9120, E8201, E8202)
- Typed holes & stubs (E9501, E9502)
- FFI hazards (E9701, E9704)
- Structured concurrency (E7801, W7800)

** See:** \Cantrip-LLM-Guide.md\ Part II

---

