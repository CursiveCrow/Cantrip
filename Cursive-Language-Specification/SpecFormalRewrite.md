# Cursive Language Specification Clause Rewriting Prompt

## Purpose

Rewrite each clause of the Cursive Language Specification using rigorous formal semantics. The output must be normative specification content suitable for implementation, not tutorial or explanatory material.

## Target Structure

Each rewritten clause MUST contain these sections in order (section 5 is conditional):

### 1. Definition

The **Definition** section provides the formal, normative definition of the language construct. It combines prose normative statements for clarity with formal mathematical definitions for precision.

**Requirements:**
- Begin with a prose normative definition that declaratively states what the construct IS and what it DOES
- The prose definition MUST be normative (using SHALL, MUST, MAY per RFC 2119 conventions)
- Follow prose with mathematical notation where precision demands it
- Define all semantic domains relevant to the construct
- Specify formal relations and functions that characterize the construct
- Include type-theoretic foundations where applicable
- Define any auxiliary functions or predicates needed for the semantics
- Enumerate all well-formedness conditions
- State invariants that the construct must maintain

**Completeness Checklist — Address each applicable aspect:**

| Aspect             | Question to Answer                                                     |
| :----------------- | :--------------------------------------------------------------------- |
| **Identity**       | What IS this construct? What distinguishes it from similar constructs? |
| **Purpose**        | What semantic role does it serve in the language?                      |
| **Constituents**   | What are its component parts (if composite)?                           |
| **Invariants**     | What properties MUST always hold for valid instances?                  |
| **Lifetime**       | When does it come into existence? When does it cease to exist?         |
| **Equality**       | When are two instances considered equal/equivalent?                    |
| **Classification** | What category: value, type, expression, statement, declaration, etc.?  |

**Format:**
```
##### Definition

[Prose normative definition using declarative language. This states WHAT the construct is
and its essential properties. Use RFC 2119 keywords (MUST, SHALL, MAY, etc.).]

[Semantic Domains]
[Table or list of domains with notation and description]

[Formal Definition]
[Mathematical definition using set theory, type theory, or domain theory as appropriate.
This formalizes the prose definition with mathematical precision.]

[Auxiliary Definitions]
[Helper functions, predicates, and relations]

**Well-Formedness**
[Enumerated conditions for valid constructs]
```

### 2. Grammar and Syntax

The **Grammar and Syntax** section defines the concrete and abstract syntax.

**Requirements:**
- Provide complete EBNF grammar productions
- Define the abstract syntax tree (AST) representation using algebraic data type notation
- Specify lexical constraints (if any)
- Define syntactic sugar and desugaring rules (as rewrite rules)
- Include precedence and associativity rules where relevant
- Define the relationship between concrete syntax and AST

**Completeness Checklist — Address each applicable aspect:**

| Aspect                  | Question to Answer                                                                          |
| :---------------------- | :------------------------------------------------------------------------------------------ |
| **Concrete syntax**     | What source text represents this construct? (EBNF production)                               |
| **Abstract syntax**     | What AST node type represents this? (Algebraic definition)                                  |
| **Lexical constraints** | Any token-level restrictions beyond grammar?                                                |
| **Disambiguation**      | Precedence, associativity, dangling-else style ambiguities?                                 |
| **Syntactic sugar**     | What surface forms desugar to this? What does this desugar to?                              |
| **Parse context**       | Where can this construct appear? (expression position, statement position, top-level, etc.) |
| **Recursion**           | Can this construct contain itself? Under what restrictions?                                 |

**Format:**
```
##### Grammar and Syntax

**Concrete Syntax (EBNF)**

[EBNF productions]

**Abstract Syntax**

[AST definition using algebraic notation:]

$$\text{Expr} ::= \text{Var}(x) \mid \text{App}(e_1, e_2) \mid \ldots$$

**Lexical Constraints**

[Any token-level constraints]

**Desugaring Rules**

[Rewrite rules: source → core]

**AST Representation**

[Formal mapping from parse tree to AST]
```

### 3. Static Semantics

The **Static Semantics** section defines compile-time analysis using judgement forms and inference rules.

**Requirements:**
- Define all judgement forms used in the section
- Use explicit context threading: $\Gamma \vdash e : T \dashv \Gamma'$ where contexts change
- Define context structure: $\Gamma ::= \cdot \mid \Gamma, x : T \mid \Gamma, x : T[\sigma]$ where $\sigma$ is state
- Include well-formedness judgements: $\Gamma \vdash T\ \text{wf}$
- Include typing judgements: $\Gamma \vdash e : T$
- Include subtyping judgements: $\Gamma \vdash T_1 <: T_2$
- Include mode/permission judgements where applicable
- Include effect judgements where applicable
- Name all inference rules descriptively: e.g., (T-App), (T-Let-Move)
- State side conditions explicitly below the line or as premises
- Include bidirectional rules where applicable: synthesis ($\Gamma \vdash e \Rightarrow T$) and checking ($\Gamma \vdash e \Leftarrow T$)

**Completeness Checklist — Address each applicable aspect:**

| Aspect                         | Question to Answer                                               |
| :----------------------------- | :--------------------------------------------------------------- |
| **Type formation**             | If this introduces a type, when is that type well-formed?        |
| **Typing rules**               | What type does this expression/statement have?                   |
| **Type synthesis vs checking** | Does this synthesize a type or check against an expected type?   |
| **Scope introduction**         | What names does this introduce? Where are they visible?          |
| **Scope reference**            | What names does this reference? How are they resolved?           |
| **Shadowing**                  | Can introduced names shadow existing names?                      |
| **Subtyping**                  | What subtype relationships apply to/from this type?              |
| **Variance**                   | For parameterized types, what is the variance of each parameter? |
| **Permission/mode**            | What permissions are required? What permissions are granted?     |
| **State tracking**             | What binding states change? (e.g., Valid → Moved)                |
| **Constraint generation**      | What type constraints does this generate for inference?          |
| **Well-formedness**            | What conditions make this construct legal?                       |
| **Ill-formedness**             | What conditions make this illegal? (Map to diagnostic codes)     |
| **Inference direction**        | How does type information flow? (up, down, bidirectional)        |

**Format:**
```
##### Static Semantics

**Judgement Forms**

| Judgement                    | Meaning                                            |
| :--------------------------- | :------------------------------------------------- |
| $\Gamma \vdash e : T$        | Expression $e$ has type $T$ under context $\Gamma$ |
| $\Gamma \vdash T\ \text{wf}$ | Type $T$ is well-formed                            |
| ...                          | ...                                                |

**Context Structure**

$$\Gamma ::= \cdot \mid \Gamma, x : T \mid \Gamma, x : T[\text{Moved}]$$

**Typing Rules**

$$\frac{\text{premise}_1 \quad \text{premise}_2}{\text{conclusion}} \quad \text{(Rule-Name)}$$

[Additional rules...]

**Constraint Generation** (if applicable)

[Constraint-based typing rules]

**Diagnostics**

| Code       | Condition   | Rule Violated |
| :--------- | :---------- | :------------ |
| E-XXX-NNNN | Description | (Rule-Name)   |
```

### 4. Dynamic Semantics

The **Dynamic Semantics** section defines runtime behavior using operational semantics.

**Requirements:**
- Define the runtime state structure (store, heap, stack, etc.)
- Use the **most appropriate** style of operational semantics for each construct:
  - **Small-step** for: stateful operations, concurrent/parallel constructs, constructs where intermediate steps matter, non-deterministic reduction
  - **Big-step** for: pure expressions, deterministic evaluation, constructs where only the final result matters
- Use explicit state threading for effects
- Provide both small-step AND big-step only when both perspectives add value (e.g., a construct that is typically evaluated in one step but may involve side-effects)

**Small-Step Semantics** (when used):
- Use the form: $\langle e, \sigma \rangle \longrightarrow \langle e', \sigma' \rangle$
- Define evaluation contexts $\mathcal{E}$ for context-based reduction
- Define values $v$ as terminal forms
- Include congruence rules
- Include reduction rules
- Define $\longrightarrow^*$ as reflexive-transitive closure

**Big-Step Semantics** (when used):
- Use the form: $\sigma \vdash e \Downarrow v, \sigma'$
- Include all evaluation rules
- Show termination cases and divergence (where applicable)

**Requirements for all dynamic semantics:**
- Define error states and panic semantics
- Include memory operations (allocation, deallocation, read, write) where relevant
- Include concurrency semantics (if applicable to the construct)
- Define determinism/non-determinism boundaries

**Completeness Checklist — Address each applicable aspect:**

| Aspect                  | Question to Answer                                                        |
| :---------------------- | :------------------------------------------------------------------------ |
| **Value domain**        | What runtime values can this construct produce?                           |
| **Evaluation rules**    | How does execution proceed? (inference rules)                             |
| **Evaluation order**    | In what order are sub-expressions evaluated?                              |
| **Termination**         | Does evaluation always terminate? Under what conditions might it diverge? |
| **Side effects**        | What effects occur? (allocation, I/O, mutation, synchronization)          |
| **Effect ordering**     | In what order do side effects occur? What is guaranteed?                  |
| **Memory operations**   | What allocation, read, write, or deallocation occurs?                     |
| **Error conditions**    | What conditions trigger a panic? What is the panic payload?               |
| **Error propagation**   | How do errors propagate through this construct?                           |
| **Resource cleanup**    | What cleanup occurs on normal exit? On error exit?                        |
| **Concurrency**         | Key acquisition? Synchronization? Atomicity guarantees?                   |
| **Non-determinism**     | What outcomes are permitted to vary between executions?                   |
| **Observable behavior** | What can a conforming program observe about this operation?               |
| **Sequencing**          | What happens-before relationships does this establish?                    |

**Format:**
```
##### Dynamic Semantics

**Runtime Domains**

| Domain | Notation | Description    |
| :----- | :------- | :------------- |
| Values | $v$      | Terminal forms |
| Store  | $\sigma$ | Memory state   |
| ...    | ...      | ...            |

**Value Forms**

$$v ::= \text{IntVal}(n) \mid \text{Closure}(\lambda x.e, \rho) \mid \ldots$$

**Small-Step Semantics**

*Evaluation Contexts:*

$$\mathcal{E} ::= [\cdot] \mid \mathcal{E}\ e \mid v\ \mathcal{E} \mid \ldots$$

*Reduction Rules:*

$$\frac{}{\langle (\lambda x.e)\ v, \sigma \rangle \longrightarrow \langle e[v/x], \sigma \rangle} \quad \text{(E-App)}$$

*Congruence:*

$$\frac{\langle e, \sigma \rangle \longrightarrow \langle e', \sigma' \rangle}{\langle \mathcal{E}[e], \sigma \rangle \longrightarrow \langle \mathcal{E}[e'], \sigma' \rangle} \quad \text{(E-Context)}$$

**Big-Step Semantics**

$$\frac{\sigma \vdash e_1 \Downarrow \text{Closure}(\lambda x.e, \rho), \sigma_1 \quad \sigma_1 \vdash e_2 \Downarrow v_2, \sigma_2 \quad \sigma_2, \rho[x \mapsto v_2] \vdash e \Downarrow v, \sigma_3}{\sigma \vdash e_1\ e_2 \Downarrow v, \sigma_3} \quad \text{(B-App)}$$

**Error Semantics**

$$\frac{\text{condition}}{\langle e, \sigma \rangle \longrightarrow \text{panic}(\text{reason})} \quad \text{(E-Panic-XXX)}$$

**Memory Operations**

[Rules for alloc, read, write, free]
```

### 5. Memory and Layout

The **Memory and Layout** section defines the physical representation of values when such representation is normative (observable via FFI, `sizeof`, pointer arithmetic, or ABI boundaries).

**When to include this section:**
- Records, enums, unions, modal types (aggregate layout)
- Primitive types (size and alignment guarantees)
- Pointer and reference types (representation)
- FFI-interoperable types (C ABI compatibility)
- Any type where `sizeof`, `alignof`, or memory representation affects program semantics

**When to OMIT this section:**
- Pure semantic constructs (control flow, expressions with no representation)
- Constructs where layout is entirely implementation-defined

**Requirements:**
- Specify size in bytes (or size formula for parameterized types)
- Specify alignment requirements
- Define field ordering and padding rules
- Define discriminant representation for sum types (enums, modals)
- Specify niche optimization applicability
- Define endianness constraints (or state that it's platform-defined)
- Include ABI compatibility requirements where applicable

**Completeness Checklist — Address each applicable aspect:**

| Aspect                     | Question to Answer                                                    |
| :------------------------- | :-------------------------------------------------------------------- |
| **Size**                   | What is `sizeof` in bytes? (exact value or formula)                   |
| **Alignment**              | What is `alignof`? What alignment is required for this type?          |
| **Field offsets**          | For aggregates, what is the offset of each field?                     |
| **Field ordering**         | Are fields laid out in declaration order? Can they be reordered?      |
| **Padding**                | Where is padding inserted? What are the padding bytes' values?        |
| **Discriminant**           | For sum types, how is the tag encoded? What is its size/position?     |
| **Niche optimization**     | When can discriminant use unused payload bit patterns?                |
| **Endianness**             | What byte order? Platform-defined or specified?                       |
| **Pointer representation** | How are references/pointers to this type represented?                 |
| **ABI compatibility**      | What C type does this correspond to? Calling convention implications? |
| **Volatile semantics**     | Any special memory access semantics?                                  |
| **Atomic semantics**       | Can this be accessed atomically? Under what conditions?               |
| **Zero-initialization**    | Is all-zeros a valid representation? What value does it represent?    |

**Format:**
```
##### Memory and Layout

**Applicability**

[State when this layout specification applies, e.g., "for all instantiations of this type"]

**Size and Alignment**

$$\text{sizeof}(T) = \ldots$$
$$\text{alignof}(T) = \ldots$$

**Field Layout**

[For aggregates: field ordering, padding insertion rules, offset formulas]

| Field | Offset | Size | Alignment |
| :---- | :----- | :--- | :-------- |
| ...   | ...    | ...  | ...       |

**Discriminant Representation** (for sum types)

[Tag encoding, niche optimization conditions, discriminant size]

**ABI Constraints**

[Platform ABI requirements, C interoperability rules]

**Layout Invariants**

[Invariants that must hold: no padding between certain fields, etc.]
```

---

## Authoring Guidelines

### DO:
- Use precise mathematical notation consistently throughout
- Thread state explicitly in all judgements where mutation occurs
- Name every inference rule
- Define all metavariables before use
- Ensure rules are syntax-directed where possible
- Include all error cases as explicit rules or side conditions
- Maintain referential integrity (cite clause numbers for forward/backward references)
- Use standard PL theory notation (see Pierce's TAPL, Harper's PFPL)

### DO NOT:
- Include examples (code snippets for illustration)
- Include tutorial explanations ("For example...", "This means that...")
- Include rationale or motivation ("The reason for this is...")
- Include comparisons to other languages
- Include implementation hints (unless normative for conformance)
- Use informal prose where formal rules suffice
- Duplicate content between sections

### Notation Conventions

**Metavariables:**
- $e, e_1, e_2$ — expressions
- $v, v_1, v_2$ — values
- $T, S, U$ — types
- $\Gamma, \Delta$ — typing contexts
- $\sigma, \sigma'$ — stores/runtime state
- $\rho$ — environments
- $x, y, z$ — variables
- $f, g$ — function names
- $\ell$ — locations/addresses
- $\kappa$ — kinds
- $P$ — permissions (const, unique, shared)
- $M, S$ — modal types and states

**Judgement Arrows:**
- $\vdash$ — entailment (turnstile)
- $\dashv$ — output context production
- $\longrightarrow$ — small-step reduction
- $\longrightarrow^*$ — multi-step reduction
- $\Downarrow$ — big-step evaluation
- $\Rightarrow$ — type synthesis
- $\Leftarrow$ — type checking

**Context Operations:**
- $\Gamma, x : T$ — context extension
- $\Gamma(x)$ — context lookup
- $\text{dom}(\Gamma)$ — domain of context
- $\Gamma[x \mapsto T]$ — context update

---

## Rewriting Process

For each clause:

1. **Extract Core Semantics**: Identify the fundamental semantic content, stripping examples and explanations
2. **Formalize Domains**: Define all semantic domains explicitly
3. **Write Grammar**: Provide complete EBNF and AST definitions
4. **Derive Static Rules**: Convert prose constraints to inference rules
5. **Derive Dynamic Rules**: Convert evaluation descriptions to operational rules
6. **Verify Completeness**: Ensure all cases are covered (use exhaustiveness checking)
7. **Cross-Reference**: Update all forward/backward references to use clause numbers
8. **Validate Diagnostics**: Ensure each diagnostic maps to a violated rule

---

## Template for a Rewritten Clause

````markdown
## Clause N: [Name]

### N.M [Subsection Name]

##### Definition

[Prose normative definition using declarative statements. Example:
"A binding is a named association between an identifier and a value.
A binding MUST have exactly one responsible owner at any program point.
A binding MAY be in one of the following states: Valid, Moved, or Dropped."]

[Semantic Domains]
| Domain | Notation | Description |
| :----- | :------- | :---------- |
| ...    | ...      | ...         |

[Mathematical definitions that formalize the normative prose above]

**Auxiliary Definitions**

[Helper functions and predicates]

**Well-Formedness**

[WF conditions]

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

```ebnf
[productions]
```

**Abstract Syntax**

$$\text{Form} ::= \ldots$$

**Desugaring**

[Rewrite rules]

##### Static Semantics

**Judgement Forms**

[Table]

**Typing Rules**

[Inference rules]

**Diagnostics**

[Diagnostic table with rule references]

##### Dynamic Semantics

**Runtime Domains**

[Table]

**Value Forms**

[Definition]

**Small-Step Semantics**

[Evaluation contexts and reduction rules]

**Big-Step Semantics**

[Evaluation rules]

**Error Semantics**

[Panic rules]

##### Memory and Layout (if applicable)

**Applicability**

[When this layout applies]

**Size and Alignment**

[Formulas]

**Field Layout**

[Offset table or rules]

**Discriminant Representation**

[For sum types]

**ABI Constraints**

[Interoperability requirements]
````

---

## Validation Checklist

Before finalizing a rewritten clause, verify each section:

**Definition Section:**
- [ ] Normative prose definition is present and uses RFC 2119 keywords
- [ ] Prose definition is declarative (states WHAT, not HOW)
- [ ] Identity, purpose, and classification are clear
- [ ] Invariants are explicitly stated
- [ ] Lifetime/existence conditions are defined
- [ ] Equality/equivalence criteria are specified (where applicable)
- [ ] All semantic domains are defined with notation

**Grammar Section:**
- [ ] Complete EBNF production provided
- [ ] Abstract syntax (AST) algebraically defined
- [ ] Disambiguation rules stated (precedence, associativity)
- [ ] Desugaring rules provided as rewrite rules
- [ ] Parse context specified

**Static Semantics Section:**
- [ ] All judgement forms defined in table
- [ ] All inference rules are named (e.g., T-App, T-Let)
- [ ] All rules have explicit premises (no implicit side conditions)
- [ ] Scope introduction and reference rules present
- [ ] Type formation rules present (if type-introducing)
- [ ] Subtyping rules present (if applicable)
- [ ] Permission/mode rules present (if applicable)
- [ ] State tracking rules present (if stateful)
- [ ] Every ill-formed case maps to a diagnostic code

**Dynamic Semantics Section:**
- [ ] Appropriate style chosen (small-step vs big-step)
- [ ] Value domain defined
- [ ] Evaluation order specified
- [ ] Side effects enumerated
- [ ] Error/panic conditions explicit with payloads
- [ ] Resource cleanup specified
- [ ] Non-determinism boundaries marked
- [ ] Sequencing/happens-before relationships defined (if concurrent)

**Memory & Layout Section (where applicable):**
- [ ] Size formula provided
- [ ] Alignment requirements specified
- [ ] Field layout rules defined (for aggregates)
- [ ] Discriminant encoding specified (for sum types)
- [ ] ABI compatibility stated

**Cross-Cutting:**
- [ ] All metavariables introduced before use
- [ ] No examples or tutorial content remains
- [ ] Cross-references use clause numbers
- [ ] Forward references are noted
- [ ] Diagnostic codes link to specific violated rules

---

**Clauses Remaining to Rewrite:**
- [x] Clause 0: Preliminaries
- [x] Clause 1: General Principles and Conformance
- [x] Clause 2: Lexical Structure and Source Text
- [x] Clause 3: The Object and Memory Model
- [x] Clause 4: Type System Foundations
- [ ] Clause 5: Data Types
- [ ] Clause 6: Behavioral Types
- [ ] Clause 7: Type Extensions
- [ ] Clause 8: Modules and Name Resolution
- [ ] Clause 9: Procedures and Methods
- [ ] Clause 10: Classes and Polymorphism
- [ ] Clause 11: Contracts and Verification
- [ ] Clause 12: Expressions & Statements
- [ ] Clause 13: The Capability System
- [ ] Clause 14: The Key System
- [ ] Clause 15: Structured Parallelism
- [ ] Clause 16: Asynchronous Operations
- [ ] Clause 17: Compile-Time Execution
- [ ] Clause 18: Type Reflection
- [ ] Clause 19: Code Generation
- [ ] Clause 20: Derivation
- [ ] Clause 21: Foreign Function Interface
- Appendices (as needed)

## File Information
**Source File:** `C:\Dev\Cursive\Cursive-Language-Specification\CursiveLanguageSpecification.md`
**Target File:** `C:\Dev\Cursive\CursiveLanguageSpec4.md`

Rewrite clause 5 of the Cursive Language Specification using rigorous formal semantics. The output must be normative specification content suitable for implementation, not tutorial or explanatory material. You are free to reorganize sections for better flow and structure as necessary.