# The Cursive Language Specification

**Part**: I - Foundations  
**File**: 01_Foundations.md  
**Previous**: (Start) | **Next**: [Type System](02_Type-System.md)

---

## Abstract

Cursive is a systems programming language designed for memory safety, deterministic performance, and AI-assisted development.

It achieves these goals through:

- **Lexical Permission System (LPS)**: Compile-time memory safety without garbage collection or borrow checking
- **Explicit Contracts**: Preconditions and postconditions as executable specifications
- **Effect System**: Compile-time tracking of side effects, allocations, and I/O
- **Modal System**: State machines as first-class types with compile-time verification
- **Memory Regions**: Explicit lifetime control with zero-overhead allocation
- **Comptime Metaprogramming**: Compile-time code generation without macros
- **File-Based Modules**: Code organization through file system structure

Cursive compiles to native code with performance matching C/C++ while providing memory safety guarantees through region-based lifetime management.

### Design Philosophy

1. **Explicit over implicit** - All effects, lifetimes, and permissions visible in code
2. **Local reasoning** - Understanding code requires minimal global context
3. **Zero abstraction cost** - Safety guarantees without runtime overhead (no garbage collection pauses or virtual machine needed)
4. **Predictable patterns** - Consistent syntax and semantics for AI code generation
5. **Simple ownership** - No borrow checker complexity
6. **No macros** - Metaprogramming through comptime only for predictability

### Safety Model

- **Prevents**: Use-after-free, double-free, memory leaks
- **Provides**: Deterministic deallocation, zero GC pauses
- **Does NOT prevent**: Aliasing bugs, data races *(these remain the programmer's responsibility)*

## Conformance

An implementation conforms to this specification if and only if it satisfies all normative requirements stated herein. Extensions MUST NOT invalidate any program that is valid under this specification. Diagnostics, implementation-defined limits, and unspecified behavior MUST be documented.

### Key Words for Requirement Levels

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in RFC 2119 and RFC 8174 when, and only when, they appear in all capitals.

### Document Conventions

- Code blocks for the language use the fenced code label `cursive`.
- Grammar productions use `ebnf` fences.
- Mathematical judgments are typeset with standard notation.
- Error identifiers (e.g., `E1001`) are normative and MUST be reported for corresponding violations.

---

## 1. Notation and Mathematical Foundations

**Definition 1.1 (Mathematical Foundations):** This section establishes the mathematical notation, metavariables, and judgment forms used throughout the Cursive Language Specification to formally describe syntax, typing rules, and operational semantics.

### 1.1 Overview

**Key innovation/purpose:** Provides a formal mathematical foundation for precisely specifying language semantics through standardized notation, judgment forms, and inference rules.

**When to use this section:**

- When reading type rules (T-\* rules in subsequent sections)
- When reading evaluation semantics (E-\* rules in operational semantics)
- When encountering unfamiliar mathematical notation
- When implementing a type checker or interpreter

**Relationship to other features:**

- **All subsequent sections** use the notation defined here
- **§2 (Lexical Structure)**: Uses set notation (∈, ⊆) for token categories
- **§3 (Abstract Syntax)**: Uses BNF notation for syntax definitions
- **Type System**: Uses type judgments (Γ ⊢ e : T) extensively
- **Operational Semantics**: Uses evaluation judgments (⟨e, σ⟩ ⇓ ⟨v, σ'⟩)
- **Contract System**: Uses Hoare triples ({P} e {Q}) and semantic satisfaction (σ ⊨ P)

**Normative status:** This section is INFORMATIVE. It defines notation but does not impose requirements on implementations. The notation is used to precisely specify requirements in later NORMATIVE sections.

### 1.2 Grammar Notation

Grammar productions are expressed in Extended Backus-Naur Form (EBNF). The following conventions are used:

- `::=` denotes "is defined as"
- `|` separates alternatives
- `*` means zero or more repetitions
- `+` means one or more repetitions
- `?` means zero or one occurrence (optional)
- `~` means negation/complement (character class negation)
- `[ ]` enclose optional elements or character classes
- `( )` group elements
- Terminal symbols are enclosed in double quotes or shown in monospace
- Non-terminal symbols are shown in italics

Complete literal productions are provided in Appendix A.1; the examples throughout this chapter reference those definitions rather than re-stating them.

### 1.3 Metavariables

The following metavariable conventions are used throughout the specification:

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
Θ ∈ TransitionSet      (state transition relation)
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
Δ, Δ' ∈ RegionCtx      (region contexts: stack of active regions)
ε_ctx ∈ EffContext     (effect contexts: available effects)
σ, σ' ∈ Store          (memory stores: Location → Value)
```

**Context Operations:**

- `Γ, x: T` means "context Γ extended with binding x: T"
- `σ[ℓ ↦ v]` means "store σ updated so location ℓ maps to value v"

### 1.4 Judgment Forms

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

### 1.5 Formal Operators

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

### 1.6 Inference Rule Format

Inference rules are presented in the following format:

```
[Rule-Name]
premise₁    premise₂    ...    premiseₙ
----------------------------------------
conclusion
```

The premises appear above the horizontal line, and the conclusion appears below. All premises must hold for the conclusion to be valid.

**Example: Function application**

```
[T-App]
Γ ⊢ f : T₁ → T₂
Γ ⊢ e : T₁
-----------------
Γ ⊢ f(e) : T₂
```

This rule reads: "If f has function type from T₁ to T₂, and e has type T₁, then the application f(e) has type T₂."

### 1.7 Reading Type Rules: Complete Examples

This subsection demonstrates how the notation defined above appears in actual specification sections.

#### Example: Integer Addition

**Cursive code:**

```cursive
let x: i32 = 10
let y: i32 = 20
let z = x + y
```

**Type rule:**

```
[T-Add]
Γ ⊢ e₁ : T
Γ ⊢ e₂ : T
T ∈ {i8, i16, i32, i64, u8, u16, u32, u64, f32, f64}
-----------------------------------------------------
Γ ⊢ e₁ + e₂ : T
```

**Application to code:**

```
Premises:
  {x: i32, y: i32} ⊢ x : i32     (variable lookup)
  {x: i32, y: i32} ⊢ y : i32     (variable lookup)
  i32 ∈ {i8, i16, ..., f64}      (i32 is a numeric type)

Conclusion:
  {x: i32, y: i32} ⊢ x + y : i32
```

#### Example: Function Call

**Cursive code:**

```cursive
function add(a: i32, b: i32): i32 { a + b }

let result = add(10, 20)
```

**Type rule:**

```
[T-Call]
Γ ⊢ f : (T₁, ..., Tₙ) → U ! ε
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
-------------------------------------
Γ ⊢ f(e₁, ..., eₙ) : U
```

**Application to code:**

```
Premises:
  Γ ⊢ add : (i32, i32) → i32 ! ∅  (function type)
  Γ ⊢ 10 : i32                       (integer literal)
  Γ ⊢ 20 : i32                       (integer literal)

Conclusion:
  Γ ⊢ add(10, 20) : i32
```

#### Example: Effect Propagation Through Calls

**Cursive code:**

```cursive
function read_file(path: string): string
    uses fs.read
{
    // Implementation reads file
}

procedure process_data()
    uses fs.read, io.write
{
    let content = read_file("/data.txt")
    println("Read: {}", content)
}
```

**Effect aggregation rule:**

```
[Effect-Aggregate-Call]
Γ ⊢ f : (τ₁, ..., τₙ) → U ! ε_f
Γ ⊢ aᵢ : τᵢ ! ε_aᵢ  (∀i)
ε_total = ε_f ∪ ε_a₁ ∪ ... ∪ ε_aₙ
--------------------------------------------------------
Γ ⊢ f(a₁, ..., aₙ) : U ! ε_total
```

**Application to code:**

```
Step 1: Type check read_file call
  Γ ⊢ read_file : (string) → string ! {fs.read}
  Γ ⊢ "/data.txt" : string ! ∅
  ε_total = {fs.read} ∪ ∅ = {fs.read}
  Γ ⊢ read_file("/data.txt") : string ! {fs.read}

Step 2: Type check println call
  Γ ⊢ println : (string, string) → () ! {io.write}
  Γ ⊢ "Read: {}" : string ! ∅
  Γ ⊢ content : string ! ∅
  ε_total = {io.write} ∪ ∅ ∪ ∅ = {io.write}
  Γ ⊢ println("Read: {}", content) : () ! {io.write}

Step 3: Check effect availability in process_data
  Required effects: {fs.read, io.write}
  Declared effects: {fs.read, io.write}
  {fs.read, io.write} ⊆ {fs.read, io.write} ✓

Conclusion: process_data is well-typed
```

#### Example: Permission Checking Across Assignments

**Cursive code:**

```cursive
record Buffer { data: [u8] }

function consume(buf: own Buffer): usize {
    result buf.data.len()
}

procedure example() {
    let buffer: own Buffer = Buffer { data: [1, 2, 3] }
    let len = consume(move buffer)
    // buffer is now invalid
}
```

**Permission checking rules:**

```
[Perm-Assign]
Γ ⊢ target : perm τ
perm ∈ {mut, own}
Γ ⊢ rhs : τ
----------------------
Γ ⊢ (target = rhs) : ()

[Move-Transfer]
Γ ⊢ e : own τ
----------------------------
Γ ⊢ move e : own τ
After: e is invalidated
```

**Application to code:**

```
Step 1: Check buffer declaration
  Γ ⊢ Buffer { data: [1, 2, 3] } : own Buffer
  Γ' = Γ, buffer : own Buffer

Step 2: Check consume call
  Γ' ⊢ consume : (own Buffer) → usize ! ∅
  Γ' ⊢ buffer : own Buffer
  Γ' ⊢ move buffer : own Buffer

  After move: buffer invalidated
  Γ'' = Γ', buffer : ⊥ (invalidated)

Step 3: Any subsequent use of buffer would fail
  Γ'' ⊢ buffer : ⊥
  ERROR E4006: use of moved value `buffer`

Conclusion: Assignment is valid; buffer cannot be used after move
```

#### Example: Modal State Transitions

**Cursive code:**

```cursive
modal File {
    states { Closed, Open }

    procedure File@Closed::open(self: own File@Closed): Result<File@Open, Error>
        uses fs.open
    {
        // Transition from Closed to Open state
    }

    procedure File@Open::read(self: File@Open, buf: mut [u8]): Result<usize, Error>
        uses fs.read
    {
        // Only available in Open state
    }
}

procedure use_file(path: string)
    uses fs.open, fs.read
{
    let file = File@Closed::new(path)
    match file::open() {
        Result::Ok(open_file) => {
            var buffer = [0u8; 1024]
            open_file::read(buffer)?
        },
        Result::Err(e) => { }
    }
}
```

**Modal transition rule:**

```
[Modal-Transition]
Γ ⊢ e : T@S₁
procedure p : (self: own T@S₁) → T@S₂ ! ε
ε ⊆ Γ.effects
--------------------------------------------------------
Γ ⊢ e::p() : T@S₂ ! ε
After: e invalidated, result has state @S₂
```

**Application to code:**

```
Step 1: Create file in Closed state
  Γ ⊢ File@Closed::new(path) : File@Closed
  Γ' = Γ, file : File@Closed

Step 2: Transition Closed → Open
  Γ' ⊢ file : File@Closed
  Γ' ⊢ File@Closed::open : (own File@Closed) → Result<File@Open, Error> ! {fs.open}
  Γ' ⊢ file::open() : Result<File@Open, Error> ! {fs.open}

  After transition: file invalidated
  Γ'' = Γ', file : ⊥

Step 3: In Ok branch, open_file has @Open state
  Γ'' ⊢ open_file : File@Open
  Γ'' ⊢ open_file::read : (File@Open, mut [u8]) → Result<usize, Error> ! {fs.read}
  Γ'' ⊢ open_file::read(buffer) : Result<usize, Error> ! {fs.read}

  File@Open::read is valid (state matches)

Conclusion: State transitions enforce protocol compliance
```

#### Example: Contract Verification

**Cursive code:**

```cursive
function divide(numerator: i32, denominator: i32): i32
    must denominator != 0
    will result * denominator == numerator
{
    result numerator / denominator
}

procedure safe_division(a: i32, b: i32): Option<i32> {
    if b != 0 {
        Option::Some(divide(a, b))
    } else {
        Option::None
    }
}
```

**Contract checking rules:**

```
[Contract-Precondition]
Γ ⊢ f : (τ₁, ..., τₙ) → U must P
Γ ⊢ aᵢ : τᵢ  (∀i)
Γ ⊨ P[params ↦ args]  (precondition holds)
--------------------------------------------------------
Γ ⊢ f(a₁, ..., aₙ) : U

[Contract-Postcondition]
Γ ⊢ body : τ
Γ, result : τ ⊨ Q  (postcondition holds)
--------------------------------------------------------
function f() will Q { body } is well-formed
```

**Application to code:**

```
Step 1: Check divide call in safe_division
  Context: inside `if b != 0` branch
  Γ ⊢ divide : (i32, i32) → i32 must denominator != 0
  Γ ⊢ a : i32
  Γ ⊢ b : i32

  Precondition check:
    Γ ⊨ b != 0  (true in this branch due to if condition)
    Precondition satisfied ✓

Step 2: Verify divide implementation satisfies postcondition
  Body: numerator / denominator
  Postcondition: result * denominator == numerator

  For integer division (assuming no overflow):
    (numerator / denominator) * denominator == numerator ✓
    (due to mathematical properties of division)

Step 3: In else branch
  Precondition b != 0 would be false
  divide is NOT called (Option::None returned)

Conclusion: Contract enforces safe division usage
```

#### Example: Region Escape Analysis

**Cursive code:**

```cursive
function create_buffer(): &[u8] {
    region temp {
        let buffer = alloc_in<temp>([1u8, 2, 3])
        // ERROR: cannot return buffer - would escape region
        // result &buffer
    }

    // Must allocate outside region to return
    let persistent = [1u8, 2, 3]
    result &persistent
}
```

**Region escape rules:**

```
[Region-Alloc]
Γ; Δ · r ⊢ v : τ
----------------------------------
Γ; Δ · r ⊢ alloc_in<r>(v) : own τ@r

[Region-Escape-Error]
Γ; Δ · r ⊢ v : τ@r
target context Δ' where r ∉ Δ'
----------------------------------
ERROR E4014: value escapes region r

[Region-Close]
Γ; Δ · r ⊢ body : τ
τ does not contain references to r
----------------------------------------
Γ; Δ ⊢ region r { body } : τ
```

**Application to code:**

```
Attempt 1: Return buffer from region (INVALID)
  Δ = ε (empty region stack)

  Step 1: Enter region temp
    Δ' = Δ · temp

  Step 2: Allocate in temp
    Γ; Δ' ⊢ [1u8, 2, 3] : [u8; 3]
    Γ; Δ' ⊢ alloc_in<temp>([1u8, 2, 3]) : own [u8; 3]@temp
    Γ' = Γ, buffer : own [u8; 3]@temp; Δ'

  Step 3: Attempt to return &buffer
    Γ' ⊢ &buffer : &[u8]@temp
    Return type context: Δ = ε (no temp region)
    temp ∈ Δ' but temp ∉ Δ
    ERROR E4014: buffer allocated in temp cannot escape

Correct approach: Allocate outside region
  Γ ⊢ [1u8, 2, 3] : [u8; 3]  (stack allocation, not in region)
  Γ' = Γ, persistent : own [u8; 3]
  Γ' ⊢ &persistent : &[u8]  (no region annotation)
  No escape violation ✓

Conclusion: Region escape analysis prevents dangling references
```

#### Example: Generic Constraint Resolution

**Cursive code:**

```cursive
contract Numeric {
    procedure add(self: Self, other: Self): Self
    procedure mul(self: Self, other: Self): Self
}

function dot_product<T: Numeric>(a: [T], b: [T]): T
    must a.len() == b.len()
{
    var sum = a[0]::add(a[0])  // Initialize to T + T = T
    loop i in 1..a.len() {
        let product = a[i]::mul(b[i])
        sum = sum::add(product)
    }
    result sum
}

let vec_i32 = [1, 2, 3]
let vec_f64 = [1.0, 2.0, 3.0]

let result_i32 = dot_product(vec_i32, vec_i32)  // T = i32
let result_f64 = dot_product(vec_f64, vec_f64)  // T = f64
```

**Generic resolution rules:**

```
[Generic-Instantiate]
Γ ⊢ f<α : Bound> : (τ₁[α], ..., τₙ[α]) → U[α]
Γ ⊢ aᵢ : υᵢ  (∀i)
Infer: α ↦ T where Γ ⊢ T : Bound
--------------------------------------------------------
Γ ⊢ f(a₁, ..., aₙ) : U[α ↦ T]

[Contract-Bound-Check]
Γ ⊢ T : Type
contract C { procedure p₁; ...; procedure pₙ }
T implements all procedures p₁, ..., pₙ
--------------------------------------------------------
Γ ⊢ T : C (T satisfies contract C)
```

**Application to code:**

```
Call 1: dot_product(vec_i32, vec_i32)
  Step 1: Infer type parameter
    Γ ⊢ vec_i32 : [i32]
    Array element type: i32
    T ↦ i32

  Step 2: Check constraint T : Numeric
    i32 implements Numeric::add ✓
    i32 implements Numeric::mul ✓
    Γ ⊢ i32 : Numeric ✓

  Step 3: Instantiate function
    dot_product<i32> : ([i32], [i32]) → i32
    Γ ⊢ dot_product(vec_i32, vec_i32) : i32

Call 2: dot_product(vec_f64, vec_f64)
  Step 1: Infer type parameter
    Γ ⊢ vec_f64 : [f64]
    Array element type: f64
    T ↦ f64

  Step 2: Check constraint T : Numeric
    f64 implements Numeric::add ✓
    f64 implements Numeric::mul ✓
    Γ ⊢ f64 : Numeric ✓

  Step 3: Instantiate function
    dot_product<f64> : ([f64], [f64]) → f64
    Γ ⊢ dot_product(vec_f64, vec_f64) : f64

Invalid call example:
  let vec_string = ["a", "b", "c"]
  // dot_product(vec_string, vec_string)  // ERROR

  T ↦ string
  string does NOT implement Numeric
  Γ ⊬ string : Numeric
  ERROR E4001: type `string` does not satisfy bound `Numeric`

Conclusion: Generic constraints ensure type safety across instantiations
```

---

### 1.8 Diagnostic Format Examples

This section specifies the format and content of diagnostic messages produced by Cursive compilers.

#### 1.8.1 Diagnostic Message Structure

**Definition 1.8 (Diagnostic Message):** A diagnostic message consists of:

1. **Primary message**: Error code and summary
2. **Source location**: File, line, column
3. **Code snippet**: Relevant source code with highlights
4. **Notes**: Additional context and explanations
5. **Help**: Suggested fixes or related information

**Canonical format:**

```
error[CODE]: primary message
  --> file.cursive:line:column
   |
LL | source code line
   | ^^^^^ highlight
   |
note: explanatory note
help: suggested fix
```

#### 1.8.2 Error Categories and Codes

| Category | Code Range | Description |
|----------|------------|-------------|
| Type errors | E4001-E4099 | Type mismatches, inference failures |
| Permission errors | E4003, E4006, E4014-E4015 | Ownership, borrowing, moves |
| Effect errors | E4004, E7C01-E7C03 | Effect availability, propagation |
| Contract errors | E7C02-E7C05, E7C07-E7C09 | Pre/postconditions, invariants |
| Pattern errors | E4005, E5005 | Non-exhaustive matches |
| Control flow errors | E4011-E4013 | Break/continue/return outside context |
| Region errors | E4014-E4015, E3D09 | Escape analysis violations |
| Syntax errors | E1001-E1099 | Parsing failures |

#### 1.8.3 Type Error Examples

**E4001: Type Mismatch**

```
error[E4001]: mismatched types
  --> example.cursive:12:18
   |
12 |     let x: i32 = "hello"
   |                  ^^^^^^^ expected `i32`, found `string`
   |
note: expected type `i32`
          found type `string`
help: you can convert a string to a number using `.parse()`
   |
12 |     let x: i32 = "hello".parse()?
   |                  ~~~~~~~~~~~~~~~~
```

**Application of rule:**

```
[T-Let]
Γ ⊢ e : τ₁
τ₁ ≡ τ₂  (types must be equivalent)
----------------------------------------
Γ ⊢ let x: τ₂ = e well-formed

Violation:
  Γ ⊢ "hello" : string
  string ≠ i32
  ERROR E4001
```

#### 1.8.4 Permission Error Examples

**E4003: Permission Violation**

```
error[E4003]: cannot mutate through immutable binding
  --> example.cursive:8:5
   |
 8 |     value.x = 10
   |     ^^^^^^^ cannot mutate `value`
   |
note: `value` is declared as immutable (default `let`)
   |
 6 |     let value = Point { x: 0, y: 0 }
   |         ----- help: consider making this binding mutable: `let mut value`
   |
note: or use a procedure that doesn't require mutation
```

**Application of rule:**

```
[Perm-Assign]
Γ ⊢ target : perm τ
perm ∈ {mut, own}  (requires mutable permission)
----------------------------------------
Γ ⊢ (target = value) : ()

Violation:
  Γ ⊢ value.x : imm i32  (immutable field of immutable record)
  imm ∉ {mut, own}
  ERROR E4003
```

**E4006: Use After Move**

```
error[E4006]: use of moved value: `buffer`
  --> example.cursive:15:20
   |
12 |     let buffer = Buffer::new()
   |         ------ move occurs because `buffer` has type `own Buffer`
13 |     consume(move buffer)
   |                  ------ value moved here
14 |
15 |     println("{}", buffer.len())
   |                    ^^^^^^ value used here after move
   |
help: consider cloning the value if the type implements `Clone`
   |
13 |     consume(buffer.clone())
   |                    ++++++++
```

**Application of rule:**

```
[Move-Invalidation]
Γ ⊢ move e : own τ
----------------------------------------
Γ' = Γ[e ↦ ⊥] (e is invalidated)

[Use-After-Move]
Γ ⊢ x : ⊥  (x is invalidated)
----------------------------------------
ERROR E4006: use of moved value
```

#### 1.8.5 Effect Error Examples

**E4004: Effect Unavailable**

```
error[E4004]: effect `fs.read` not available in this context
  --> example.cursive:18:17
   |
16 | function pure_function() {
   |          ------------- this function has no `uses` clause (pure)
17 |     let content = read_file("/data.txt")
18 |                   ^^^^^^^^^
   |                   |
   |                   this function requires effect `fs.read`
   |
note: `read_file` declared with `uses fs.read`
   |
 3 | function read_file(path: string): string
 4 |     uses fs.read
   |     ^^^^^^^^^^^^ effect declared here
   |
help: add `uses fs.read` to the function signature
   |
16 | function pure_function()
17 |     uses fs.read
   | ++++++++++++++++
```

**Application of rule:**

```
[Effect-Check]
Γ ⊢ f : (τ₁, ..., τₙ) → U ! ε_f
ε_f ⊆ Γ.effects  (required effects must be available)
----------------------------------------
Γ ⊢ f(...) permitted

Violation:
  Γ.effects = ∅  (pure context)
  ε_f = {fs.read}
  {fs.read} ⊈ ∅
  ERROR E4004
```

#### 1.8.6 Contract Error Examples

**E7C02: Precondition Violation**

```
error[E7C02]: precondition violated at runtime
  --> example.cursive:23:12
   |
20 | function divide(n: i32, d: i32): i32
21 |     must d != 0
   |     ----------- precondition declared here
22 | {
23 |     divide(10, 0)
   |            ^
   |            |
   |            precondition `d != 0` is false
   |
note: this check failed: d != 0
   where d = 0
help: ensure denominator is non-zero before calling
   |
22 |     if d != 0 {
23 |         divide(10, d)
24 |     }
```

**Application of rule:**

```
[Contract-Precondition-Runtime]
Γ ⊢ f must P
Γ ⊢ f(args)
σ ⊭ P[params ↦ args]  (precondition false at runtime)
----------------------------------------
Runtime ERROR E7C02
```

**E7C03: Postcondition Violation**

```
error[E7C03]: postcondition violated at runtime
  --> example.cursive:30:12
   |
28 | function increment(x: i32): i32
29 |     will result > x
   |     -------------- postcondition declared here
30 | {
31 |     result x - 1  // BUG: decrements instead of incrementing
   |            ^^^^^ postcondition `result > x` is false
   |
note: this check failed: result > x
   where result = 4, x = 5
help: the implementation does not satisfy the postcondition
```

**Application of rule:**

```
[Contract-Postcondition-Runtime]
Γ ⊢ function f() will Q { body }
Γ ⊢ body : τ
σ, result ↦ v ⊭ Q  (postcondition false after execution)
----------------------------------------
Runtime ERROR E7C03
```

#### 1.8.7 Pattern Match Error Examples

**E4005: Non-Exhaustive Match**

```
error[E4005]: non-exhaustive patterns: `Option::None` not covered
  --> example.cursive:18:11
   |
18 |     match maybe_value {
   |           ^^^^^^^^^^^ pattern `Option::None` not covered
   |
note: `Option<T>` defined with 2 variants
  --> std/option.cursive:5:1
   |
 5 | enum Option<T> {
 6 |     Some(T),
 7 |     None
   |     ---- not covered
   |
help: ensure that all possible cases are being handled
   |
19 |         Option::Some(x) => process(x),
20 +        Option::None => { },
```

**Application of rule:**

```
[Match-Exhaustiveness]
Γ ⊢ e : τ
patterns p₁, ..., pₙ
∀v : τ. ∃i. v matches pᵢ  (all values covered)
----------------------------------------
Γ ⊢ match e { p₁ => e₁, ..., pₙ => eₙ } well-formed

Violation:
  τ = Option<i32>
  patterns = {Option::Some(_)}
  Option::None not covered
  ERROR E4005
```

#### 1.8.8 Region Escape Error Examples

**E4014: Region Value Escape**

```
error[E4014]: value allocated in region `temp` cannot escape
  --> example.cursive:15:12
   |
12 | region temp {
   |        ---- region `temp` declared here
13 |     let buffer = alloc_in<temp>(Buffer::new())
   |                                 -------------- allocated in region `temp`
14 |     global_buffer = buffer
   |                     ^^^^^^ value escapes region boundary here
15 | }
   | - `temp` ends here, but `buffer` lives longer
   |
note: value has type `own Buffer@temp` (region-tagged)
help: consider allocating outside the region
   |
   | let buffer = Buffer::new()  // allocates on stack, not in region
```

**Application of rule:**

```
[Region-Escape]
Γ; Δ · r ⊢ v : own τ@r
target context Δ' where r ∉ Δ'
----------------------------------------
ERROR E4014: escape from region r
```

#### 1.8.9 Control Flow Error Examples

**E4011: Break Outside Loop**

```
error[E4011]: `break` outside of loop context
  --> example.cursive:8:5
   |
 8 |     break
   |     ^^^^^ cannot `break` outside of a loop
   |
help: this `break` statement must be inside a `loop` body
```

**Application of rule:**

```
[Break-Context]
break in loop body
----------------------------------------
Γ ⊢ break : !

[Break-Outside-Loop]
break not in loop body
----------------------------------------
ERROR E4011
```

#### 1.8.10 Multi-Line Diagnostic Example

**Complex diagnostic with multiple notes:**

```
error[E4003]: cannot move out of `data` because it is behind a mutable reference
  --> example.cursive:28:18
   |
25 | procedure process(container: mut Container) {
   |                   --------- help: consider changing this to be owned: `own Container`
26 |     let item = container.data
   |                ---------- move occurs because `container.data` has type `own Data`
27 |
28 |     consume(move item)
   |                  ^^^^ value moved here
29 |     container.data = Data::new()  // Attempt to replace
   |     ^^^^^^^^^^^^^^ but container is `mut`, not `own`
   |
note: moving out of a field of a `mut` reference is not allowed
   = help: the borrow checker prevents partial moves from borrowed data
note: if `Container` implemented `Default`, you could use `std::mem::replace`
   |
28 |     consume(std::mem::replace(&mut container.data, Data::default()))
```

This complex diagnostic shows:
- Multiple code locations
- Nested notes and help messages
- Suggestions for fixes using standard library
- Explanation of borrow checker semantics

---

### 1.9 Reading Algorithms

This section explains how to interpret algorithmic specifications in the Cursive Language Specification.

#### 1.9.1 Algorithm Format Conventions

Algorithms in this specification use structured pseudocode with the following conventions:

**Format elements:**

```
Algorithm N.M: Algorithm Name

Inputs:
  - input₁: Type (description)
  - input₂: Type (description)

Outputs:
  - output: Type (description)

Steps:
  1. Description of step 1
     a. Sub-step
     b. Sub-step
        if condition:
            nested operation
  2. Description of step 2
  ...
  N. Final step

Return output
```

**Control flow notation:**

- **Conditional**: `if condition: ... else: ...`
- **Loop**: `for each x in collection: ...` or `while condition: ...`
- **Early exit**: `return value` or `ERROR (code): message`
- **Assertion**: `require condition` (must be true)
- **Assignment**: `variable ← value`

**Metasyntax:**

- `Type` refers to a type from §3.2
- `⊢`, `∈`, `⊆` use mathematical notation from §1.5
- `Γ`, `σ`, `τ`, `υ` use metavariables from §1.3

#### 1.9.2 Complete Algorithm Example

**Algorithm 1.1 (Type Inference for Let Bindings)**

This algorithm demonstrates how type inference works for variable bindings.

```
Algorithm 1.1: Type Inference for Let Bindings

Inputs:
  - binding: let pattern = expr (or let pattern: annotation = expr)
  - context: Γ (type environment)

Outputs:
  - Extended context Γ' with new bindings
  - Or type error

Steps:
  1. Check if type annotation is present
     if binding has type annotation τ_annot:
         goto ANNOTATED_INFERENCE
     else:
         goto UNANNOTATED_INFERENCE

  2. ANNOTATED_INFERENCE:
     a. Verify annotation is well-formed
        require Γ ⊢ τ_annot : Type
        if not well-formed:
            ERROR (E4001): ill-formed type annotation

     b. Type check expression against annotation
        Infer type of expr: Γ ⊢ expr : τ_expr

     c. Check compatibility
        if τ_expr <: τ_annot:  (subtyping check)
            τ_final ← τ_annot
        else:
            ERROR (E4001): type mismatch
                expected τ_annot
                found τ_expr

     d. goto BIND_PATTERN

  3. UNANNOTATED_INFERENCE:
     a. Infer expression type
        Γ ⊢ expr : τ_expr

     b. Check inference succeeded
        if τ_expr contains holes:
            ERROR (E4412): cannot infer complete type

     c. Use inferred type
        τ_final ← τ_expr

     d. goto BIND_PATTERN

  4. BIND_PATTERN:
     a. Match pattern structure
        match pattern:
            Identifier(x):
                Γ' ← Γ, x : τ_final
                return Γ'

            Tuple(p₁, ..., pₙ):
                require τ_final = (τ₁, ..., τₙ)
                for i in 1..n:
                    Γ ← bind_pattern(Γ, pᵢ, τᵢ)
                return Γ

            Record {f₁: p₁, ..., fₙ: pₙ}:
                require τ_final = record Name { ... }
                for each field fᵢ with type τ_fᵢ:
                    Γ ← bind_pattern(Γ, pᵢ, τ_fᵢ)
                return Γ

            Wildcard(_):
                return Γ  (no binding)

            _:
                ERROR: invalid pattern in let binding

Return Γ'
```

**Mapping to inference rules:**

The algorithm implements these formal rules:

```
[T-Let-Annotated]
Γ ⊢ τ : Type
Γ ⊢ e : υ
υ <: τ
----------------------------------------
Γ ⊢ let x: τ = e  extends to  Γ, x : τ

[T-Let-Inferred]
Γ ⊢ e : τ
τ complete (no holes)
----------------------------------------
Γ ⊢ let x = e  extends to  Γ, x : τ
```

**Example execution:**

**Input code:**

```cursive
let point: Point = Point { x: 10, y: 20 }
```

**Algorithm execution trace:**

```
Step 1: Check annotation presence
  annotation present: Point
  → goto ANNOTATED_INFERENCE

Step 2: ANNOTATED_INFERENCE
  2a. Verify annotation well-formed
      Γ ⊢ Point : Type ✓

  2b. Type check expression
      Γ ⊢ Point { x: 10, y: 20 } : Point
      τ_expr = Point

  2c. Check compatibility
      Point <: Point ✓
      τ_final = Point

  2d. → goto BIND_PATTERN

Step 4: BIND_PATTERN
  pattern = Identifier("point")
  Γ' = Γ, point : Point

Return Γ, point : Point
```

#### 1.9.3 Reading Algorithm Complexity

Algorithms may include complexity annotations:

- **Time complexity**: How execution time grows with input size
- **Space complexity**: Memory usage
- **Determinism**: Whether output is unique

**Example notation:**

```
Algorithm X.Y: Name
Complexity:
  Time: O(n) where n = number of patterns
  Space: O(1) (constant)
  Deterministic: Yes

...
```

#### 1.9.4 Algorithm Error Paths

Algorithms specify error conditions explicitly:

**Error format:**

```
ERROR (code): message
where:
  variable₁ = value₁
  variable₂ = value₂
```

**Example:**

```
if denominator == 0:
    ERROR (E4008): division by zero
    where:
      numerator = n
      denominator = 0
```

This maps to runtime diagnostic:

```
error[E4008]: division by zero
note: denominator = 0
```

---

### 1.10 Cross-Reference Navigation

This section explains how to navigate cross-references within the specification.

#### 1.10.1 Citation Format

Cross-references use the format:

```
CITE: Part X §Y.Z — Title
```

Where:
- `Part X`: Roman numeral part number (I, II, III, ...)
- `§Y.Z`: Section and subsection numbers
- `Title`: Brief description (optional)

**Examples:**

```
CITE: Part II §2.0.6.1 — Subtyping
CITE: Part IV §4.7 — Ownership Transfer
CITE: Part IX §9.6.3 — Resolution Algorithm
```

#### 1.10.2 Multi-Chapter Feature Trace

Many features span multiple chapters. This subsection shows how to trace a feature across parts.

**Example trace: Closures**

Closures are specified across 4 parts:

```
Trace: Closure Feature

Part II §2.10 — Closure Types
  ├- Defines: Function types (τ₁, ..., τₙ) → U ! ε
  ├- Defines: Closure type representation
  └- CITE: Part V §5.10 for capture semantics

Part IV §4.10 — Closures and Captures
  ├- Defines: Permission requirements for captures
  ├- Defines: Capture classification (own, mut, imm)
  ├- CITE: Part II §2.10 for type rules
  └- CITE: Part V §5.10 for syntax

Part V §5.10 — Closure Expressions
  ├- Defines: Lambda syntax |params| -> expr
  ├- Defines: Capture analysis algorithm
  ├- CITE: Part IV §4.10 for permissions
  └- CITE: Part IX §9.7 for integration with functions

Part IX §9.7 — Closures and Function Pointers
  ├- Defines: Closure vs function pointer distinction
  ├- Defines: Zero-capture coercion rules
  ├- CITE: Part II §2.10 for types
  └- CITE: Part V §5.10 for expressions
```

**Reading order for closures:**

1. Start with Part V §5.10 (concrete syntax)
2. Read Part II §2.10 (type system)
3. Read Part IV §4.10 (permission semantics)
4. Read Part IX §9.7 (integration with functions)

#### 1.10.3 Dependency Graphs

Some features have complex dependencies:

**Example: Effect System Dependencies**

```
Effect System Dependency Graph:

Part VII §7.3 — Effect Clauses (PRIMARY)
  ↓ depends on
Part II §2.9 — Effect Types
  ↓ depends on
Part I §3.7 — Effect Language (Abstract Syntax)
  ↓ used by
Part V §4.19 — Effect Composition
  ↓ affects
Part IX §9.6.6 — Effect Propagation at Call Sites
  ↓ integrates with
Part IV §4.14 — Effects and Permissions
```

**Reading strategy:**

- **Bottom-up**: Start with Part I §3.7, progress upward
- **Top-down**: Start with Part VII §7.3, follow CITEs downward
- **Use-case**: Start with Part IX §9.6.6 (practical), trace dependencies

#### 1.10.4 Resolving Circular References

Some features have circular dependencies:

**Example: Functions and Types**

```
Part II §2.10 — Function Types
  CITE: Part IX §9.2 for function declarations

Part IX §9.2 — Function Declarations
  CITE: Part II §2.10 for type syntax
```

**Resolution strategy:**

1. Read both sections for overview
2. Identify which is authoritative for which aspect:
   - Part II: Authoritative for TYPE FORMATION
   - Part IX: Authoritative for DECLARATION SYNTAX
3. Cross-check examples against both

#### 1.10.5 Forward References

The specification sometimes references future chapters:

**Handling forward references:**

```
Part IV §4.13.3 — Deferred Dependencies

"Part VIII (deferred) will provide runtime semantics for regions..."
```

**Reading approach:**

- **Normative rules**: Apply the rules stated in current chapter
- **Missing details**: Note as "implementation-defined" until Part VIII published
- **Consistency**: When Part VIII publishes, verify consistency

---

### 1.11 Notational Well‑Formedness (Informative)

The following rules state when contexts and notational judgments are well‑formed. They concern the mathematics used in this Part and do not assert that any program is well‑typed.

```
[WF-Context-Empty]
--------------------
∅ well-formed

[WF-Context-Extend]
Γ well-formed    x ∉ dom(Γ)    Γ ⊢ T : Type
--------------------------------------------
Γ, x: T well-formed

[WF-Typing-Judgment]
Γ well-formed    e ∈ Expr    T ∈ Type
--------------------------------------
Γ ⊢ e : T is a judgment

[WF-Effect-Judgment]
Γ well-formed    e ∈ Expr    ε ∈ Effect
--------------------------------------
Γ ⊢ e ! ε is a judgment
```

---

## 2. Lexical Structure

**Definition 2.1 (Lexical Structure):** The lexical structure defines the syntax of tokens—the smallest elements of Cursive source text—including identifiers, literals, keywords, operators, and delimiters.

### 2.1 Overview

**Key innovation/purpose:** Defines the atomic elements (tokens) of Cursive source code, forming the first phase of compilation that transforms character streams into structured token sequences.

**When to use this section:**

- When implementing a lexer/tokenizer
- When defining concrete syntax for new language features
- When understanding character-level source code rules
- When resolving ambiguities in token formation

**Relationship to other features:**

- **§3 (Abstract Syntax)**: Tokens are parsed into abstract syntax trees
- **Type System**: Type inference assigns types to literal tokens
- **Compilation Pipeline**: Lexical analysis is the first phase
- **Error Reporting**: Lexical errors are the first class of errors detected

**Phase in compilation pipeline:**

```
Source Text → [Lexer] → Token Stream → [Parser] → AST → [Type Checker] → ...
```

**Token categories:**

- **Identifiers**: Names for variables, functions, types (§2.2)
- **Literals**: Integer, float, boolean, character, string constants (§2.3)
- **Keywords**: Reserved words with special meaning (§2.4)
- **Operators**: Symbolic operators (+, -, \*, /, etc.)
- **Delimiters**: Punctuation (`{}`, `[]`, `()`, `,`, `:`)
- **Comments**: Discarded during tokenization
- **Newlines**: Significant tokens for statement termination

### 2.2 Characters and Source Text Encoding

**Source Files:**

A Cursive source file is a sequence of Unicode characters encoded in UTF-8.

**Formal properties:**

- Encoding: UTF-8 (REQUIRED)
- Line endings: LF (`\n`), CRLF (`\r\n`), or CR (`\r`) (all accepted, normalized to LF during lexing)
- Newlines are significant tokens: Newlines are preserved as tokens during lexical analysis
- BOM: Optional UTF-8 BOM (U+FEFF) is ignored if present
- File extension: `.cursive` (RECOMMENDED)
- Maximum file size: Implementation-defined (RECOMMENDED ≤ 1 MiB)
- Normalization: Unicode normalization form NFC is RECOMMENDED but not required

**Character Restrictions:**

- NUL bytes (U+0000) are disallowed in source text
- Control characters other than horizontal tab, line feed, carriage return, and form feed are disallowed outside string and character literals

### 2.3 Comments and Whitespace

The lexical productions for comments and whitespace appear in Appendix A.1. Key semantic rules:

**Semantic rules:**

- Line comments extend from `//` to end of line
- Block comments nest: `/* outer /* inner */ outer */` is valid
- Doc comments (`///`) document the item that follows
- Module docs (`//!`) document the containing module
- Doc comments are preserved for documentation generation
- Non-doc comments are stripped before parsing
- Comments do not affect statement continuation (newlines after comments are significant)

**Whitespace:**

Whitespace characters (spaces, tabs, carriage returns) are generally ignored except where they serve as token separators. Newlines are preserved as tokens for statement termination rules.

### 2.4 Identifiers

**Definition 2.2 (Identifier):** An identifier is a sequence of characters used to name variables, functions, types, modules, and other program entities. See Appendix A.1 for the formal production.

**Restrictions:**

- Cannot be a reserved keyword (see §2.5)
- Cannot start with a digit
- Case-sensitive: `Variable` ≠ `variable`
- Maximum length: Implementation-defined (RECOMMENDED: 255 characters)

### 2.5 Keywords

**Definition 2.3 (Keyword):** A keyword is a reserved identifier with special syntactic meaning that cannot be used as an identifier.

**Reserved keywords (MUST NOT be used as identifiers):**

```
abstract    as          async       await       break
by          case        comptime    continue
defer       effect      else        enum        exists
false       forall      function    if          import
internal    invariant   let         loop
match       modal       module      move        must
mut         new         none        own         private
procedure   protected   public      record      ref
region      result      select      self        Self
shadow      state       static      trait       true
type        uses        var         where       will
with
```

**Contextual keywords (special meaning in specific contexts):**

```
effects     pure
```

**Note:** `needs`, `requires`, and `ensures` are DEPRECATED (replaced by `uses`, `must`, `will`).

### 2.6 Literals

Literals are token sequences that directly represent constant values.

#### 2.6.1 Numeric Literals

**Integer Literals:** The lexical forms are defined in Appendix A.1.

**Underscore separators:**

- Underscores (`_`) may appear between digits for readability
- Underscores are ignored during value computation
- Cannot appear at the start or end of the literal
- Cannot appear adjacent to the base prefix (`0x`, `0b`, `0o`)

**Examples:**

```cursive
42          → IntLiteral(42, None)           // No suffix, default i32
42u64       → IntLiteral(42, Some(u64))      // Explicit u64
0xFF        → IntLiteral(255, None)          // Hexadecimal
0b1010      → IntLiteral(10, None)           // Binary
1_000_000   → IntLiteral(1000000, None)      // Underscores ignored
```

**Default types:** Unsuffixed integer literals default to `i32`. Floating-point literals default to `f64`.

**Floating-Point Literals:** See Appendix A.1 for the syntactic forms of floating-point literals and their optional suffixes.

**Examples:**

```cursive
3.14        → FloatLiteral(3.14, None)       // No suffix, default f64
3.14f32     → FloatLiteral(3.14, Some(f32))  // Explicit f32
1.0e10      → FloatLiteral(1.0e10, None)     // Scientific notation
```

#### 2.6.2 String Literals

The lexical production for `StringLiteral` and the escape rules are defined in Appendix A.1.

**String properties:**

- Strings are sequences of Unicode scalar values
- Escape sequences are processed during lexing
- Multi-line strings are supported (newlines in source → newlines in string)



**Escape sequences:**

| Sequence   | Description     | Unicode Code Point          |
| ---------- | --------------- | --------------------------- |
| `\n`       | newline         | U+000A                      |
| `\r`       | carriage return | U+000D                      |
| `\t`       | tab             | U+0009                      |
| `\\`       | backslash       | U+005C                      |
| `\'`       | single quote    | U+0027                      |
| `\"`       | double quote    | U+0022                      |
| `\0`       | null character  | U+0000                      |
| `\xNN`     | ASCII character | U+00NN (00-7F)              |
| `\u{N...}` | Unicode scalar  | U+N... (up to 6 hex digits) |

**Examples:**

```cursive
"hello"                    → StringLiteral("hello")
"line 1\nline 2"          → StringLiteral("line 1\nline 2")
"path\\to\\file"          → StringLiteral("path\\to\\file")
"\u{1F600}"                → StringLiteral("😀")
```

#### 2.6.3 Character Literals

The lexical form of `CharLiteral` is defined in Appendix A.1; escape sequences mirror those for strings.

**Character properties:**

- Character literals represent a single Unicode scalar value
- Must contain exactly one character (or one escape sequence)
- Escape sequences are the same as for string literals

**Examples:**

```cursive
'a'         → CharLiteral('a')               // ASCII character
'\n'        → CharLiteral('\n')              // Escape sequence
'\u{1F600}' → CharLiteral('😀')              // Unicode escape
```

#### 2.6.4 Boolean Literals

`BooleanLiteral` is defined in Appendix A.1.

Boolean literals represent the values `true` and `false`. They are keywords and cannot be used as identifiers.

### 2.7 Statement Termination Rules

Cursive uses newlines as primary statement terminators. Semicolons are optional and may be used to separate multiple statements on a single line.

**Primary rule:** A newline terminates a statement unless a continuation rule applies.

**Optional separator:** Semicolons `;` may be used to separate statements on the same line.

**Continuation Rules:**

A statement continues across newlines in exactly four cases:

#### Rule 2.7-1 (Unclosed Delimiters)

Statement continues when `(`, `[`, or `<` remains unclosed.

**Example:**

```cursive
let result = calculate(
    arg1,
    arg2,
    arg3
)
```

#### Rule 2.7-2 (Trailing Operator)

Statement continues when line ends with a binary or assignment operator.

Binary operators: `+`, `-`, `*`, `/`, `%`, `**`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`, `&`, `|`, `^`, `<<`, `>>`, `..`, `..=`, `=>`

Assignment operators: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`

**Example:**

```cursive
let total = base +
    modifier +
    bonus
```

#### Rule 2.7-3 (Leading Dot)

Statement continues when next line begins with `.` (field access). Use `::` for scope/procedure/effect calls; `::` is not a chaining operator. For chaining of calls, use the pipeline operator `=>`.

**Example:**

```cursive
// Use the pipeline operator for chaining calls
result = input
    => validate
    => transform
    => process
```

#### Rule 2.7-4 (Leading Pipeline)

Statement continues when next line begins with `=>` (pipeline operator).

**Example:**

```cursive
let result = input
    => validate
    => transform
    => process
```

**Multiple statements on one line:**

```cursive
let x = 1; let y = 2; let z = 3
```

### 2.8 Token Formation

**Token Formation:**

The lexer forms tokens using the maximal munch principle: given multiple possible token matches at a position, the longest match is chosen.

**Examples:**

```
0xFF        → IntLiteral(255) NOT IntLiteral(0), Identifier("xFF")
x++         → Identifier("x"), Operator(PlusPlus) NOT Identifier("x"), Operator(Plus), Operator(Plus)
while_loop  → Identifier("while_loop") NOT Keyword("while"), Identifier("_loop")
```

---

**Definition 3.1 (Abstract Syntax):** The abstract syntax defines the structure of Cursive programs as abstract syntax trees (ASTs), independent of concrete textual representation.

## 3. Abstract Syntax

### 3.1 Overview

**Key innovation/purpose:** Provides a mathematical representation of program structure that separates the essence of code (what it means) from its textual surface form (how it's written), enabling precise formal reasoning.

**When to use this section:**

- When understanding how programs are represented internally
- When reading type rules and operational semantics
- When implementing parsers, type checkers, or interpreters
- When formal reasoning about program transformations

**Relationship to other features:**

- **§2 (Lexical Structure)**: Tokens are parsed into ASTs
- **Type System**: Type rules operate on AST nodes
- **Operational Semantics**: Evaluation rules operate on ASTs
- **Contract System**: Contracts annotate AST nodes

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

### 3.2 Type Language

**Definition 3.2 (Type Language):** The abstract syntax of types.

```
T ::= i8 | i16 | i32 | i64 | isize          (signed integers)
    | u8 | u16 | u32 | u64 | usize          (unsigned integers)
    | f32 | f64                              (floats)
    | bool                                   (booleans)
    | char                                   (characters)
    | ()                                     (unit type)
    | string                                 (unified string type)
    | [T; n]                                 (fixed array)
    | [T]                                    (slice)
    | (T₁, ..., Tₙ)                          (tuple)
    | (T₁, ..., Tₙ) → U ! ε                  (function type)
    | own T                                  (owned type)
    | mut T                                  (mutable reference)
    | imm T                                  (immutable reference)
    | T@S                                    (type T in modal state @S)
    | ∀α. T                                  (polymorphic type)
    | !                                      (never type)
    | record Name                            (record type)
    | modal Name                             (modal type)
    | enum Name                              (enum type)
    | trait Name                             (trait type)
```

 
### 3.3 Expression Language

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

### 3.4 Pattern Language

**Definition 3.4 (Patterns):** The abstract syntax of patterns for destructuring.

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

### 3.5 Value Language

**Definition 3.5 (Values):** The abstract syntax of values (fully evaluated expressions).

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

### 3.6 Assertion Language

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
    uses ε                                 (effect declaration)
    must P                                 (preconditions)
    will Q                                 (postconditions)
```

### 3.7 Effect Language

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
    | net.read(d) | net.write               (network)
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

### 3.8 AST Representation

**Well-Formedness:**

Not all syntactically valid ASTs are well-formed. Well-formedness constraints include:

- **Variables in scope**: All variable references must be bound
- **Type well-formedness**: All type expressions must be valid
- **Pattern exhaustiveness**: Match expressions must handle all cases
- **State validity**: Modal state transitions must be valid

**Examples:**

**Simple arithmetic expression:**

**Concrete:**

```cursive
(x + 1) * 2
```

**Abstract:**

```
BinOp(Mul, BinOp(Add, Var("x"), Lit(1)), Lit(2))
```

**Pattern match:**

**Concrete:**

```cursive
match result {
    Result::Ok(value) -> process(value),
    Result::Err(e) -> handle_error(e),
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

---

**Definition 4.1 (Attributes):** An attribute is a metadata annotation that modifies the compilation behavior, analysis, or code generation for the item it decorates, evaluated at compile-time without affecting runtime semantics unless explicitly specified.

## 4. Attributes and Annotations

### 4.1 Overview

**Key innovation/purpose:** Attributes provide declarative compiler directives that modify how code is compiled, verified, or represented without changing the code's fundamental logic.

**When to use attributes:**

- Controlling memory layout for FFI interoperability (`[[repr(C)]]`)
- Selecting verification modes (`[[verify(static)]]`, `[[verify(runtime)]]`)
- Configuring runtime behavior (`[[overflow_checks(true)]]`)
- Providing metadata for tools and documentation (`[[module(...)]]`, `[[alias(...)]]`)
- Optimizing compilation (`[[inline]]`, `[[no_mangle]]`)

**Relationship to other features:**

- **Contracts**: Attributes can select verification mode for contract checking
- **Effects**: Some attributes affect effect checking
- **Memory Layout**: `[[repr(...)]]` controls record/enum layout
- **FFI**: Attributes critical for C interoperability

**Design principle:** Attributes are **declarative compiler hints**, not executable code. They configure the compiler's behavior but don't execute at runtime (unless they change runtime behavior like `[[overflow_checks]]`).

### 4.2 Syntax

The formal productions for `Attribute` and `AttributeBody` appear in Appendix A.1.

**Syntax examples:**

```cursive
// Single attribute
[[repr(C)]]
record Point { x: f32, y: f32 }

// Multiple attributes (stacked)
[[verify(static)]]
[[inline]]
function critical_math(x: i32) { ... }

// Multiple attributes (grouped)
[[repr(C), module(name = "ffi")]]
record CPoint { x: f32, y: f32 }

// Attribute with multiple arguments
[[alias("to_string", "stringify", "as_str")]]
function to_str(x: Debug): string { ... }
```

**Placement rules:**

Attributes appear immediately before the item they modify:

- Module declarations
- Record, enum, modal, trait declarations
- Function and procedure declarations
- Function/procedure parameters
- Record/enum fields
- Type aliases
- Effect declarations

### 4.3 Core Attributes

| Attribute | Applies To | Effect | Notes |
|-----------|------------|--------|-------|
| `[[repr(C|transparent|packed|align(n))]]` | Records, enums | Selects layout strategy | Packed removes padding; align(n) raises minimum alignment. |
| `[[verify(static|runtime|none)]]` | Functions, procedures, record types | Chooses when `uses/must/will` clauses run | `static` proves clauses, `runtime` injects checks, `none` documents intent. |
| `[[overflow_checks(true|false)]]` | Functions, procedures, blocks | Forces integer overflow behaviour | Overrides build-mode defaults. |
| `[[module(key=..., ...)]]` | Module declarations | Records metadata | Keys are implementation-defined; no semantic impact. |
| `[[alias(name₁, …, nameₙ)]]` | Top-level declarations | Supplies tooling aliases | Does not change linkage or lookup. |
| `[[inline]]`, `[[no_inline]]` | Functions, procedures | Inlining hints | Conflicting hints are rejected. |
| `[[no_mangle]]` | External-facing declarations | Preserves symbol names | Typically used for FFI. |
| `[[deprecated(message)]]` | Any declaration | Emits diagnostic on use | Message SHOULD describe the replacement. |
| `[[must_use]]` | Functions, return types | Warns if result ignored | Common on resource-returning APIs. |
| `[[cold]]`, `[[hot]]` | Functions, procedures | Optimisation temperature hints | Advisory only. |
### 4.4 Attribute Semantics

- Attributes precede the declarations they modify.
- Placement is validated statically; using an attribute on an unsupported declaration is an error.
- Conflicting annotations (e.g. `[[inline]]` with `[[no_inline]]`, or two incompatible `[[repr(..)]]` variants) are rejected.
- Each declaration is annotated independently; attributes do not inherit to nested members.
### 4.5 Attribute Effects

`[[verify(runtime)]]`, layout attributes, overflow hints, and inlining hints can change generated code. All other attributes are compile-time metadata with no direct runtime impact.
### 4.6 User-Defined Attributes

**Status:** User-defined attributes are RESERVED for future versions.

Currently, only core attributes defined in this specification are recognized. User-defined attributes will cause a compilation error:

```cursive
[[my_custom_attr]]    // ERROR: unknown attribute 'my_custom_attr'
function foo() { ... }
```

**Future design:** User-defined attributes may be added in conjunction with procedural macros or compile-time reflection.

---

**Previous**: (Start) | **Next**: [Type System](02_Type-System.md)
## 7. Glossary

| Term | Definition |
|------|------------|
| Callable | **Generic term** for any function or procedure (collectively, callables are invocable declarations with parameters and return types). Use this term when referring to functions and procedures without distinction. See Part IX. |
| Contract | Abstract behavioral specification with procedure signatures and clauses (`uses`, `must`, `will`). See Part VII §7.1. |
| Contract clause | One of `uses`, `must`, `will` attached to a callable signature. |
| Effect set | Finite set of capability tokens (e.g., `fs.write`, `alloc.heap`). |
| Function | A callable declared without a `self` parameter. Functions may be declared at module scope or as associated functions within a type's scope (invoked via `::`). Functions cannot directly access instance fields. Declared with the `function` keyword. See Part IX §9.2. |
| Mixin | **Informal term** for trait-based code composition pattern. In Cursive, use the `trait` keyword for reusable implementations. See "Trait" below. |
| Modal state | A value of a modal type annotated with `@State`. |
| Permission wrapper | One of `own T`, `mut T`, or `imm T`, indicating ownership semantics. |
| Pipeline stage | An expression of the form `expr => stage: Type` chaining transformations. |
| Procedure | A callable with an explicit `self` parameter that operates on a receiver value. Procedures are invoked using the scope operator `::` (e.g., `value::method(args)`). Procedures can access instance fields through `self`. Declared with the `procedure` keyword. See Part IX §9.3. |
| Region | Lexically-scoped allocation arena released in LIFO order. |
| Trait | Reusable procedure implementations (enables mixin-style composition). All procedures in a trait MUST have bodies. Distinct from contracts (which have no bodies). See Part II §2.7. |
| Transition arm | Match arm that performs a modal transition (`pattern => transition() => body`). |

## Appendix A: Grammar Reference

The canonical grammar for Cursive is maintained in `Spec/A1_Grammar.md`. Chapters reference that appendix for lexical, type, expression, and statement productions instead of re-stating them inline. Implementers SHOULD consult:

- **A.1 Lexical Grammar** — token-level productions shared by all parts.
- **A.2 Type Grammar** — all type constructors, permission wrappers, pointers, and unions.
- **A.3 Pattern Grammar** — patterns used in match expressions and destructuring.
- **A.4 Expression Grammar** — precedence hierarchy and expression forms.
- **A.5 Statement Grammar** — block, declaration, assignment, and control-flow statements.

Helper productions (e.g., `AssignOp`, `RangeOp`) and lexical tokens follow the same appendix; Foundations, the Type System, Expressions, and Statements now reference those sections directly.

## 8. Diagnostics Directory

| ID | Summary | Primary Definition |
|----|---------|--------------------|
| E3D10 | Assignment to immutable binding | §3.2.8 (Declarations) |
| E4001 | Type mismatch | §4.5 (Expressions) |
| E4002 | Arity mismatch | §4.3 (Expressions) |
| E4003 | Permission violation | §4.3/§5.4 |
| E4004 | Effect unavailable | §4.3/§4.19 |
| E4005 | Non-exhaustive match | §4.8 |
| E4006 | Use after move | §4.4.5 |
| E4007 | Index out of bounds | §4.3.5 |
| E4008 | Division by zero | §4.5.1 |
| E4009 | Integer overflow (debug) | §4.5.1 |
| E4010 | Uninitialised variable | §3.2.7 (Declarations) |
| E4011 | Break outside loop | §4.9.5 / §5.8 |
| E4012 | Continue outside loop | §4.9.6 / §5.8 |
| E4013 | Return outside function | §4.14 / §5.7 |
| E4014 | Region value escape | §2.0.8.4 / §5.8 |
| E4015 | Closure captures escaping region | §2.10 / §5.8 |
| E4016 | Contract precondition violation | §4.16 |
| E4017 | Contract postcondition violation | §4.16 |
| E4401 | Missing `result` keyword in block expression | §4.2.4 |
| E4402 | Missing type annotation on match binding | §4.8.1 |
| E4403 | Missing type annotation on pipeline stage | §4.5.9 |
| E4404 | Missing type annotation on loop iterator | §4.9.3 |
| E5001 | Duplicate label | §5.18 |
| E5002 | Undefined label | §5.18 |
| E5003 | Continue targeting non-loop label | §5.18 |
| E5004 | Invalid state transition arm | §5.18 |
| E5005 | Non-exhaustive modal match | §5.18 |
| E5006 | Loop postcondition violation | §5.18 |
| E5007 | Block precondition violation | §5.18 |
| E5008 | Block effect exceeds function effects | §5.18 |
| E5009 | Panic inside [[no_panic]] section | §5.18 |
| E7C01 | Undeclared effect used in body | Part VII §7.3 |
| E7C02 | Precondition violated at runtime | Part VII §7.4 |
| E7C03 | Postcondition violated at runtime | Part VII §7.5 |
| E7C04 | Type invariant violated at construction | Part VII §7.6 |
| E7C05 | Type invariant violated after mutation | Part VII §7.6 |
| E7C06 | Body provided inside contract signature | Part VII §7.2 |
| E7C07 | Implementation strengthens contract precondition | Part VII §7.2 |
| E7C08 | Implementation weakens contract postcondition | Part VII §7.2 |
| E7C09 | Implementation effect set exceeds contract | Part VII §7.2 |
| E7C10 | Incompatible contract requirements | Part VII §7.2 |
| E7C11 | Missing implementation for contract member | Part VII §7.2 |
| E7C12 | Orphan contract implementation | Part VII §7.2 |
| E7C13 | Duplicate contract implementation | Part VII §7.2 |
| E7C14 | Recursive associated type definition | Part VII §7.2 |
| E7C15 | Contract extends itself (inheritance cycle) | Part VII §7.2 |
| E7C16 | Illegal `@old` capture | Part VII §7.5 |
| E7C17 | Effect inference ambiguity | Part VII §7.3 |
| E7C18 | Duplicate contract clause | Part VII §7.2 |
| E4411 | Unresolved expression hole | Part VIII §8.5 |
| E4412 | Unresolved/ambiguous type hole (or ambiguous kind) | Part VIII §8.6 |
| E4413 | Unresolved/ambiguous permission hole | Part VIII §8.7 |
| E4414 | Unresolved/ambiguous modal‑state hole | Part VIII §8.8 |
| E4415 | Hole in explicitness‑required position | Part VIII §8.10 |
