# The Cursive Language Specification (Draft 4)

# Part 0: Preliminaries

## Clause 0: Preliminaries

### 0.1 Semantic Concepts and Terminology

##### Definition

This section defines the fundamental semantic concepts and terminology used throughout this specification. These definitions are normative; rigorous adherence to these meanings is required for the interpretation of subsequent clauses.

**Lexeme**
A **lexeme** is the specific sequence of Unicode scalar values matched by a lexical production pattern during tokenization. It is the concrete realization of a token in the source text.

**Lattice**
A **lattice** is a partially ordered set $(L, \sqsubseteq)$ in which every pair of elements has a unique supremum (least upper bound, or join $\sqcup$) and a unique infimum (greatest lower bound, or meet $\sqcap$).

**Linear Lattice**
A **linear lattice** (or total order) is a lattice in which every pair of elements is comparable.
Formally: $\forall a, b \in L.\ a \sqsubseteq b \lor b \sqsubseteq a$.
In a linear lattice, the path between the top element ($\top$) and bottom element ($\bot$) is unique.

**Type Equivalence**
**Nominal Equivalence**: Two types are nominally equivalent if and only if they refer to the exact same declaration definition. Records, enums, and modal types use nominal equivalence.
**Structural Equivalence**: Two types are structurally equivalent if and only if their constituent components are equivalent and arranged in the same structure. Tuples, arrays, slices, and union types use structural equivalence.

**Payload**
A **payload** is the associated data carried by a sum type variant (such as an `enum` variant or `modal` state). A payload MAY be unit-like (empty), tuple-like (ordered sequence), or record-like (named fields).

**Place Expression**
A **place expression** (or l-value) is an expression that evaluates to a memory location rather than a transient value. Place expressions are the only valid operands for the address-of operator (`&`) and the left-hand side of assignment statements.

**Value Category**
Every expression in Cursive is classified as either a **Place Expression** or a **Value Expression**. This classification determines the operations permitted on the expression.

##### Formal Definition

**Equivalence Relations**

Let $\mathcal{T}$ be the universe of all types. Let $\text{Decl}(T)$ denote the unique declaration associated with a nominal type $T$. Let $\text{Components}(T)$ denote the ordered sequence of component types for a structural type $T$.

The nominal equivalence relation $\equiv_{nom}$ is defined as:
$$T \equiv_{nom} U \iff \text{Decl}(T) = \text{Decl}(U)$$

The structural equivalence relation $\equiv_{struct}$ is defined inductively:
$$T \equiv_{struct} U \iff |\text{Components}(T)| = |\text{Components}(U)| \land \forall i.\ \text{Components}(T)_i \equiv \text{Components}(U)_i$$

**Place Expressions**

Let $\mathcal{E}$ be the set of all expressions. The predicate $\text{IsPlace}(e)$ holds if $e \in \mathcal{E}$ denotes a location.

$$\text{IsPlace}(e) \iff e \in \{ \text{Identifier}_{\text{bound}}, e'.\text{field}, e'[i], *e' \}$$

### 0.2 Specification Notation

##### Definition

This section defines the mathematical notation, judgment forms, and inference rule schemas used to specify the static and dynamic semantics of the language.

**Typing Judgments**
Typing judgments assert relationships between environments, expressions, types, and effects. The notation uses a turnstile symbol ($\vdash$) to separate the context (premises or environment) from the assertion (conclusion).

**Context**
The typing context $\Gamma$ models the static environment, mapping identifiers to types and tracking state properties (such as initialization or move state) through the program.

**Inference Rules**
Inference rules are written in natural deduction style. A rule states that if all premises (above the line) hold, then the conclusion (below the line) holds. Side conditions may appear as clauses alongside the premises.

##### Grammar and Syntax

**Judgment Syntax**

The general form of a typing judgment is:

$$\Gamma \vdash e : T \dashv \Gamma'$$

Where:
- $\Gamma$ is the **input context**: the environment state *before* evaluating $e$.
- $\vdash$ is the **entailment** symbol (read "proves" or "yields").
- $e$ is the **expression** or program term being analyzed.
- $:$ indicates the **typing relation** (read "has type").
- $T$ is the **type** derived for $e$.
- $\dashv$ is the **output** symbol (read "producing" or "resulting in").
- $\Gamma'$ is the **output context**: the environment state *after* evaluating $e$, reflecting changes such as variable initialization, ownership transfer (moves), or permission updates.

**Simplified Forms**

When the context is unmodified ($\Gamma = \Gamma'$), the output context is omitted:
$$\Gamma \vdash e : T$$

When an assertion does not involve a value type (e.g., well-formedness), the type is replaced by a property:
$$\Gamma \vdash T\ \text{wf}$$

**Inference Rule Schema**

$$\frac{\text{Premise}_1 \quad \text{Premise}_2 \quad \dots \quad \text{Premise}_n}{\text{Conclusion}} \quad \text{(Rule-Name)}$$

##### Static Semantics

**Context Structure**

The context $\Gamma$ is a sequence of bindings. The empty context is denoted by $\cdot$ or $\emptyset$.

$$\Gamma ::= \cdot \mid \Gamma, x : T \mid \Gamma, x : T[\text{State}]$$

Where:
- $x : T$ represents an immutable binding of identifier $x$ to type $T$.
- $x : T[\text{State}]$ represents a binding with a tracked state (e.g., `Valid`, `Moved`).

**Context Operations**

1.  **Lookup**: $\Gamma(x)$ denotes the type and state associated with $x$ in $\Gamma$.
2.  **Extension**: $\Gamma, x : T$ appends a new binding to $\Gamma$.
3.  **Update**: $\Gamma[x \mapsto \text{State}]$ denotes a context identical to $\Gamma$ except that the state of $x$ is updated.

**Logic Symbols**

The following logical symbols are used in side conditions:
- $\land$: Logical AND
- $\lor$: Logical OR
- $\neg$: Logical NOT
- $\implies$: Logical implication
- $\iff$: Logical equivalence
- $\forall$: Universal quantification ("for all")
- $\exists$: Existential quantification ("there exists")
- $\in$: Set membership
- $\subseteq$: Subset relation

**Evaluation Order in Judgments**

In a rule with multiple premises involving state threading, the order of premises implies the flow of the context. For example:

$$\frac{\Gamma \vdash e_1 : T_1 \dashv \Gamma_1 \quad \Gamma_1 \vdash e_2 : T_2 \dashv \Gamma_2}{\Gamma \vdash (e_1, e_2) : (T_1, T_2) \dashv \Gamma_2}$$

This implies $e_1$ is analyzed first, producing $\Gamma_1$, which is then used to analyze $e_2$.

Here is the rewritten **Clause 1: General Principles and Conformance**, starting with Section 1.1.

## Clause 1: General Principles and Conformance

### 1.1 Conformance Obligations

##### Definition

A **conforming implementation** is a function that maps source text to either an executable program representation or a set of diagnostics. It MUST accept every well-formed program (producing an executable and a conformance dossier) and reject every ill-formed program (producing at least one error diagnostic), except where the ill-formedness is classified as Ill-Formed, No Diagnostic Required (IFNDR).

A **conforming program** is a Cursive source text that satisfies all lexical, syntactic, and static-semantic constraints defined in this specification (is well-formed) and relies only on Defined Behavior or explicitly documented Implementation-Defined Behavior.

A **Conformance Dossier** is a machine-readable document generated by the implementation that records all Implementation-Defined Behavior (IDB) choices, implementation limits, and specific environment characteristics applicable to the compilation.

**Semantic Domains**

| Domain          | Notation                                                                            | Description                                                         |
| :-------------- | :---------------------------------------------------------------------------------- | :------------------------------------------------------------------ |
| Source Text     | $S \in \mathcal{S}$                                                                 | The set of all Unicode strings constituting input programs.         |
| Executables     | $X \in \mathcal{X}$                                                                 | The set of all valid target machine representations.                |
| Diagnostics     | $E \in \mathcal{E}$                                                                 | The set of all error messages identifying specification violations. |
| Dossiers        | $D \in \mathcal{D}$                                                                 | The set of valid JSON documents conforming to the Dossier schema.   |
| Implementations | $\mathcal{I} : \mathcal{S} \to (\mathcal{X} \times \mathcal{D}) \cup \mathcal{E}^+$ | The set of functions mapping source to output or errors.            |

**Formal Definition**

Let $\vdash P : \text{WF}$ denote the judgment that program $P$ is well-formed according to this specification.
Let $\text{IFNDR}(P)$ be a predicate holding if $P$ contains an IFNDR violation (§1.2.1).

An implementation $I \in \mathcal{I}$ is **conforming** if and only if for all $P \in \mathcal{S}$:

1.  **Acceptance**: $\vdash P : \text{WF} \implies I(P) = (X, D)$
2.  **Rejection**: $\neg (\vdash P : \text{WF}) \land \neg \text{IFNDR}(P) \implies I(P) = \{E_1, \ldots, E_n\}$ where $n \ge 1$.
3.  **Dossier Validity**: If $I(P) = (X, D)$, then $D$ satisfies the schema defined in Appendix C.

##### Static Semantics

**Judgement Forms**

| Judgement                                    | Meaning                                                |
| :------------------------------------------- | :----------------------------------------------------- |
| $\vdash_{\text{impl}} I : \text{Conforming}$ | Implementation $I$ satisfies conformance requirements. |
| $P \vdash_{\text{conf}} \text{Valid}$        | Program $P$ is valid input for compilation.            |

**Conformance Rules**

$$( \text{Conf-Accept} ) \quad \frac{\vdash P : \text{WF}}{I(P) \to (X, D)}$$

$$( \text{Conf-Reject} ) \quad \frac{\vdash P : \text{IllFormed}(C) \quad C \notin \text{IFNDR}}{I(P) \to \{E\} \quad \text{where } \text{Code}(E) = \text{DiagnosticFor}(C)}$$

$$( \text{Conf-Dossier} ) \quad \frac{I(P) = (X, D)}{D \models \text{Schema}_{\text{AppendixC}} \land \forall \beta \in \text{IDB}(P), \beta \in D}$$

**Diagnostics**

| Code         | Condition                                                            | Rule Violated  |
| :----------- | :------------------------------------------------------------------- | :------------- |
| `E-CNF-0101` | Program is ill-formed due to syntactic or static-semantic violation. | (Conf-Reject)  |
| `E-CNF-0102` | Conformance Dossier generation failed.                               | (Conf-Dossier) |

##### Dynamic Semantics

**Observational Equivalence**

The executable $X$ produced by a conforming implementation MUST exhibit observable behavior consistent with the dynamic semantics defined in this specification.

Let $\text{Eval}(P)$ be the trace of observable events defined by the operational semantics.
Let $\text{Exec}(X)$ be the trace of observable events produced by executing $X$ on the target platform.

For any well-formed program $P$:
$$\text{Exec}(X) \approx \text{Eval}(P)$$

where $\approx$ denotes observational equivalence (bisimulation) with respect to Defined Behavior.

Here is the rewritten **Section 1.2: Behavior Classifications**.

---

### 1.2 Behavior Classifications

##### Definition

**Behavior Classifications** partition the set of all possible program executions into four mutually exclusive categories based on the constraints imposed by this specification and the guarantees provided by the implementation.

**Defined Behavior** is execution behavior for which this specification prescribes exactly one permitted outcome. A conforming implementation MUST produce this outcome for all well-formed programs exercising such behavior.

**Implementation-Defined Behavior (IDB)** is execution behavior for which this specification permits a set of valid outcomes $\mathcal{O}_{IDB}$ where $|\mathcal{O}_{IDB}| > 1$. The implementation MUST select exactly one outcome $o \in \mathcal{O}_{IDB}$, document this choice in the Conformance Dossier, and maintain consistency across all compilations with the same configuration.

**Unspecified Behavior (USB)** is execution behavior for which this specification permits a set of valid outcomes $\mathcal{O}_{USB}$ where $|\mathcal{O}_{USB}| > 1$. The implementation MUST select an outcome from this set, but is NOT required to document the choice or maintain consistency between executions.

**Unverifiable Behavior (UVB)** consists of operations whose semantic correctness depends on properties external to the Cursive abstract machine. UVB is permitted ONLY within `unsafe` blocks. A program exhibiting UVB outside an `unsafe` block violates the Safety Invariant.

**Semantic Domains**

| Domain            | Notation            | Description                                                   |
| :---------------- | :------------------ | :------------------------------------------------------------ |
| Program Behaviors | $\mathcal{B}$       | The set of all possible runtime execution traces.             |
| Outcomes          | $o \in \mathcal{O}$ | Observable results of execution (termination, I/O, panic).    |
| Configuration     | $C \in \mathcal{C}$ | The implementation environment (compiler flags, target arch). |

**Formal Definition**

Let $\text{Semantics}(P, C)$ be the set of valid outcomes for program $P$ under configuration $C$.

1.  **Defined**: $|\text{Semantics}(P, C)| = 1$.
2.  **IDB**: $|\text{Semantics}(P, C)| \ge 1$ and the choice is fixed by $C$ and documented.
3.  **USB**: $|\text{Semantics}(P, C)| > 1$ and the choice is non-deterministic or inconsistent.
4.  **UVB**: The outcome is undefined by the abstract machine; correctness relies on the programmer maintaining external invariants.

**Safety Invariant**

Let $P_{safe}$ be a program containing no `unsafe` blocks.
For all executions $E$ of $P_{safe}$:
$$E \notin \text{UVB}$$

The language guarantees that well-formed safe code never exhibits Unverifiable Behavior.

##### Static Semantics

**Judgement Forms**

| Judgement                         | Meaning                                           |
| :-------------------------------- | :------------------------------------------------ |
| $\Gamma \vdash e : \text{Safe}$   | Expression $e$ is free of Unverifiable Behavior.  |
| $\Gamma \vdash e : \text{Unsafe}$ | Expression $e$ may exhibit Unverifiable Behavior. |

**Safety Rules**

$$( \text{Safe-Expr} ) \quad \frac{\forall \text{subexpr } e' \in e.\ \Gamma \vdash e' : \text{Safe}}{\Gamma \vdash e : \text{Safe}}$$

$$( \text{Unsafe-Block} ) \quad \frac{\Gamma \vdash \text{body} : \text{Unsafe}}{\Gamma \vdash \texttt{unsafe } \{ \text{body} \} : \text{Safe}}$$

The `unsafe` block acts as a semantic boundary, converting an `Unsafe` judgment inside to a `Safe` judgment outside by asserting that the programmer has manually verified the invariants.

**Diagnostics**

| Code         | Condition                                | Rule Violated |
| :----------- | :--------------------------------------- | :------------ |
| `E-MEM-3030` | Unsafe operation outside `unsafe` block. | (Safe-Expr)   |

##### Dynamic Semantics

**Behavioral Constraints**

1.  **IDB Consistency**: For a given configuration $C$, if a construct is IDB, then for all executions: $\text{Exec}(C) = \text{Exec}'(C)$.
2.  **USB Safety**: The set of permitted outcomes for USB MUST NOT include memory unsafety or data races in safe code.
3.  **UVB Boundary**: If an execution enters a state defined as UVB, the semantic guarantees of the abstract machine are voided for the remainder of that execution path.

**Diagnostic Coordination**

When a single program violation triggers multiple diagnostic codes (e.g., both a type error and a memory model error), the implementation MUST emit the **Canonical Diagnostic** according to the precedence hierarchy defined in Appendix B.4.

Here is the rewritten **Section 1.2.1: Ill-Formed, No Diagnostic Required (IFNDR)**.

---

#### 1.2.1 Ill-Formed, No Diagnostic Required (IFNDR)

##### Definition

A program is classified as **Ill-Formed, No Diagnostic Required (IFNDR)** if it violates a static-semantic rule where detecting the violation is computationally undecidable or infeasible.

For an IFNDR program, the implementation IS NOT required to emit a diagnostic. However, the program remains ill-formed, and its execution semantics are Unspecified Behavior (USB).

**Classification Criteria**

A rule violation $V$ is classified as IFNDR if and only if detecting $V$ requires solving a problem in a complexity class strictly harder than Polynomial Time ($P$), or if the problem is undecidable.

Common IFNDR categories include:
1.  **Halting Violations**: Violations depending on whether a compile-time computation terminates.
2.  **Resource Exhaustion**: Violations that only manifest when implementation limits are exceeded during complex metaprogramming expansion.
3.  **Infinite Type Expansion**: Type definitions that recurse infinitely without indirection.
4.  **Unbounded Unrolling**: Loop unrolling in `comptime` contexts where bounds cannot be statically determined.

**Semantic Domains**

| Domain     | Notation            | Description                                                |
| :--------- | :------------------ | :--------------------------------------------------------- |
| Violations | $v \in \mathcal{V}$ | The set of all possible static-semantic rule violations.   |
| Complexity | $C(v)$              | The computational complexity class required to detect $v$. |

**Formal Definition**

Let $\mathcal{P}$ be the set of well-formed programs.
Let $\mathcal{V}_{IFNDR} \subset \mathcal{V}$ be the set of violations such that:
$$\forall v \in \mathcal{V}_{IFNDR}.\ C(v) \notin P \lor \text{Undecidable}(v)$$

A program $P$ is IFNDR if:
$$\exists v \in \mathcal{V}_{IFNDR}.\ P \text{ exhibits } v$$

##### Static Semantics

**Safety Boundary**

Although an implementation is not required to diagnose IFNDR violations, the **Safety Invariant** (§1.2) MUST still hold.

If an IFNDR program contains no `unsafe` blocks, its execution MUST NOT exhibit Unverifiable Behavior (UVB). The behavior is Unspecified (USB), meaning the implementation may:
1.  Fail to compile (crash or hang during compilation).
2.  Panic deterministically at runtime.
3.  Execute with arbitrary safe but unspecified results.

It MUST NOT:
1.  Corrupt memory.
2.  Violate type safety.
3.  Invoke Undefined Behavior (UB).

**Judgement Forms**

| Judgement                 | Meaning                                  |
| :------------------------ | :--------------------------------------- |
| $\vdash P : \text{IFNDR}$ | Program $P$ contains an IFNDR violation. |

**Reporting Obligations**

If an implementation successfully detects an IFNDR violation (e.g., via heuristics or timeout), it MUST record the instance in the Conformance Dossier.

$$( \text{Report-IFNDR} ) \quad \frac{\text{Detected}(v) \land v \in \mathcal{V}_{IFNDR}}{\text{Dossier} \leftarrow \text{Record}(v)}$$

##### Dynamic Semantics

**Execution Semantics**

For a program $P$ where $\vdash P : \text{IFNDR}$:

$$\text{Eval}(P) \in \mathcal{O}_{USB}$$

The outcome is drawn from the set of valid Unspecified Behavior outcomes.

**Panic Safety**

If an implementation chooses to panic at runtime for an IFNDR condition (e.g., infinite recursion stack overflow), the panic MUST obey standard unwinding semantics (§3.6).

## Clause 1: General Principles and Conformance 

### 1.1 Conformance Obligations 

##### Definition

A **conforming implementation** is a translator that MUST satisfy all normative requirements of this specification that are applicable to implementations. A translator is any system that maps Cursive source text to an executable representation, whether by compilation, interpretation, or any hybrid strategy.

A **conforming program** is a Cursive source program that MUST satisfy all normative requirements of this specification that are applicable to programs.

A program is **well-formed** if and only if it satisfies all lexical constraints (Clause 2), all syntactic constraints (as defined by the grammar and parsing requirements of this specification), and all static-semantic constraints defined throughout this specification.

A program is **ill-formed** if and only if it is not well-formed. An ill-formed program MAY be classified as **IFNDR** per §1.2.1.

A conforming implementation MUST:

1. accept every well-formed program and translate it to an executable representation;
2. reject every ill-formed program that is not IFNDR with at least one error diagnostic identifying a violated normative rule;
3. generate a **Conformance Dossier** as a build artifact for each successful translation; the dossier MUST be a machine-readable JSON document conforming to the normative schema defined in Appendix C;
4. document all **Implementation-Defined Behavior (IDB)** choices in the Conformance Dossier; and
5. record all detected IFNDR instances as specified in §1.2.1.

**Semantic Domains**

| Domain               | Notation                                  | Description                                                             |
| :------------------- | :---------------------------------------- | :---------------------------------------------------------------------- |
| Implementations      | $I \in \mathcal{I}$                       | Translators subject to implementation conformance                       |
| Programs             | $P \in \mathcal{P}$                       | Cursive source programs (collections of compilation units)              |
| Configuration        | $C \in \mathcal{C}$                       | Translator configuration (target, edition, flags, limits, IDB settings) |
| Executables          | $X \in \mathcal{X}$                       | Executable representations produced by translation                      |
| Dossiers             | $D \in \mathcal{D}$                       | Conformance dossier artifacts (Appendix C)                              |
| Diagnostics          | $\delta \in \mathcal{E}$                  | Diagnostic records (code, severity, span)                               |
| Diagnostic sequences | $\Delta \in \mathcal{E}^*$                | Finite sequences of diagnostics                                         |
| Severities           | $s \in {\textsf{Error},\textsf{Warning}}$ | Diagnostic severities relevant to conformance                           |

**Formal Definition**

Let $\textsf{hasErr}(\Delta) \iff \exists \delta \in \Delta.\ \delta.\textsf{sev}=\textsf{Error}$.

Define the translation outcome domain:
$$
\textsf{Result} ;::=; \textsf{Accept}(X, D, \Delta)\ \mid\ \textsf{Reject}(\Delta).
$$

A translator execution is modeled as a total function:
$$
\textsf{Compile}_I : \mathcal{C} \times \mathcal{P} \to \textsf{Result}.
$$

Define auxiliary predicates (specified authoritatively by referenced clauses):

* $\textsf{LexWF}(P)$ — $P$ satisfies Clause 2;
* $\textsf{SynWF}(P)$ — $P$ parses according to the grammar of this specification;
* $\textsf{SemWF}(P)$ — $P$ satisfies all static-semantic judgements of this specification;
* $\textsf{LimitsOK}_{I,C}(P)$ — $P$ does not exceed implementation limits (§1.4).

Then:
$$
\textsf{WF}*{I,C}(P)\ \stackrel{\textsf{def}}{=}\ \textsf{LexWF}(P)\ \land\ \textsf{SynWF}(P)\ \land\ \textsf{SemWF}(P)\ \land\ \textsf{LimitsOK}*{I,C}(P).
$$
$$
\textsf{IllFormed}*{I,C}(P)\ \stackrel{\textsf{def}}{=}\ \neg \textsf{WF}*{I,C}(P).
$$

**Auxiliary Definitions**

* $D \models \textsf{DossierSchema}$ denotes that dossier $D$ conforms to the normative schema in Appendix C.
* $\textsf{RecordIDB}(D)$ and $\textsf{RecordIFNDR}(D)$ denote the dossier obligations in items (4) and (5) above.

**Well-Formedness**

1. $\textsf{WF}_{I,C}(P)$ MUST hold for any program that a conforming implementation accepts without relying on IFNDR permissiveness.
2. If $\textsf{IllFormed}_{I,C}(P)$ holds and $P$ is not IFNDR, a conforming implementation MUST reject $P$ (Static Semantics, §1.1).

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

No Cursive source-level productions are introduced by §1.1.

**Abstract Syntax**

No AST forms are introduced by §1.1.

**Lexical Constraints**

Not applicable.

**Desugaring Rules**

Not applicable.

**AST Representation**

Not applicable.

##### Static Semantics

**Judgement Forms**

| Judgement                               | Meaning                                                                              |
| :-------------------------------------- | :----------------------------------------------------------------------------------- |
| $I, C \vdash P\ \textsf{wf}$            | Program $P$ is well-formed under $I$ and $C$                                         |
| $I, C \vdash P \Downarrow X, D, \Delta$ | Compilation accepts $P$, producing executable $X$, dossier $D$, diagnostics $\Delta$ |
| $I, C \vdash P \Uparrow \Delta$         | Compilation rejects $P$ with diagnostics $\Delta$                                    |
| $D \models \textsf{DossierSchema}$      | $D$ conforms to Appendix C                                                           |

**Context Structure**

Translation is parameterized by $(I,C)$; no additional typing context is introduced by §1.1.

**Typing Rules**

**(WF-Program)**
$$
\frac{
\textsf{WF}_{I,C}(P)
}{
I, C \vdash P\ \textsf{wf}
}
\quad\text{(WF-Program)}
$$

**(C-Accept)**
$$
\frac{
I, C \vdash P\ \textsf{wf}
\quad
\textsf{Compile}_I(C,P)=\textsf{Accept}(X,D,\Delta)
\quad
\neg\textsf{hasErr}(\Delta)
\quad
D \models \textsf{DossierSchema}
}{
I, C \vdash P \Downarrow X, D, \Delta
}
\quad\text{(C-Accept)}
$$

**(C-Reject-IllFormed)**
$$
\frac{
\textsf{IllFormed}*{I,C}(P)
\quad
\neg \textsf{IFNDR}*{I,C}(P)
\quad
\textsf{Compile}_I(C,P)=\textsf{Reject}(\Delta)
\quad
\textsf{hasErr}(\Delta)
}{
I, C \vdash P \Uparrow \Delta
}
\quad\text{(C-Reject-IllFormed)}
$$

**(C-Reject-Dossier-Fail)**
$$
\frac{
I, C \vdash P\ \textsf{wf}
\quad
\textsf{Compile}_I(C,P)=\textsf{Reject}(\Delta)
\quad
\exists \delta \in \Delta.\ \delta.\textsf{code}=\texttt{E-CNF-0102}
}{
I, C \vdash P \Uparrow \Delta
}
\quad\text{(C-Reject-Dossier-Fail)}
$$

**Diagnostics** 

| Code       | Condition                                                                                                | Rule Violated           |
| :--------- | :------------------------------------------------------------------------------------------------------- | :---------------------- |
| E-CNF-0101 | Program is rejected due to a syntactic or static-semantic violation (excluding pure limit errors, §1.4). | (C-Reject-IllFormed)    |
| E-CNF-0102 | Conformance Dossier generation failed or the produced dossier does not conform to Appendix C.            | (C-Reject-Dossier-Fail) |

##### Dynamic Semantics

§1.1 introduces no runtime-executed language construct. The only operational effects specified here are those of translation acceptance and rejection, which are modeled by the compile-time judgements $I,C \vdash P \Downarrow \cdots$ and $I,C \vdash P \Uparrow \cdots$.

---

### 1.2 Behavior Classifications

##### Definition

**Behavior classifications** SHALL partition all language-relevant semantic choice points into exactly four mutually exclusive classes: **Defined**, **Implementation-Defined (IDB)**, **Unspecified (USB)**, and **Unverifiable (UVB)**.

A **Defined** behavior MUST have exactly one permitted outcome.

An **IDB** behavior MUST have a nonempty set of permitted outcomes, and each conforming implementation MUST select exactly one outcome from that set for each configuration and MUST document the selection in the Conformance Dossier.

A **USB** behavior MUST have a nonempty set of permitted outcomes; a conforming implementation MAY choose any permitted outcome at each occurrence and is not required to document the choice. In safe code, USB outcomes MUST NOT introduce memory unsafety or data races.

A **UVB** behavior is an operation whose correctness depends on properties external to the language’s semantic model. UVB is permitted only within `unsafe` blocks and Foreign Function Interface (FFI) calls. An `unsafe` block assertion transfers responsibility for maintaining the external invariant(s) required by the UVB operation to the programmer.

**Semantic Domains**

| Domain             | Notation                      | Description                                                 |
| :----------------- | :---------------------------- | :---------------------------------------------------------- |
| Behavior instances | $b \in \mathcal{B}$           | Indexed semantic choice points (Appendix I)                 |
| Behavior classes   | $k \in \mathcal{K}_B$         | ${\textsf{Defined},\textsf{IDB},\textsf{USB},\textsf{UVB}}$ |
| Outcomes           | $o \in \mathcal{O}$           | Observable outcomes of an occurrence                        |
| Outcome sets       | $S \subseteq \mathcal{O}$     | Permitted outcome sets                                      |
| Program points     | $p \in \textsf{ProgramPoint}$ | As defined in §1.6                                          |
| Safety predicate   | $\textsf{SafeObs}(o)$         | Outcome has no memory unsafety and no data races            |

**Formal Definition**

A classification is a total function:
$$
\textsf{Class} : \mathcal{B} \to \mathcal{K}_B.
$$

Permitted outcomes are given by:
$$
\textsf{Permitted} : \mathcal{B} \to \mathcal{P}(\mathcal{O})
\quad\text{with}\quad
\forall b.\ \textsf{Permitted}(b) \neq \emptyset.
$$

For IDB, each implementation and configuration determine a documented choice function:
$$
\textsf{Choice}*{I} : \mathcal{C} \times \mathcal{B}*{\textsf{IDB}} \to \mathcal{O}
\quad\text{where}\quad
\mathcal{B}_{\textsf{IDB}}={b \mid \textsf{Class}(b)=\textsf{IDB}}
$$
subject to:
$$
\forall C,b.\ \textsf{Choice}_I(C,b) \in \textsf{Permitted}(b).
$$

**Auxiliary Definitions**

* $\textsf{SafeProgram}(P)$ holds iff $P$ contains no `unsafe` blocks (Clause 3) and performs no FFI calls (Clause 21).
* $\textsf{Occurs}(b,P)$ holds iff the execution of $P$ can encounter behavior instance $b$.

**Well-Formedness**

1. (**Partition**) $\textsf{Class}$ MUST be total and single-valued: $\forall b.\ \exists! k.\ \textsf{Class}(b)=k$.
2. (**IDB documentation**) For every $b$ with $\textsf{Class}(b)=\textsf{IDB}$ that is implemented, the implementation MUST include its selected outcome in the Conformance Dossier (Appendix C).
3. (**UVB boundary**) If $\textsf{SafeProgram}(P)$ and $P$ is well-formed, then no execution of $P$ MAY exhibit UVB (Static Semantics: Safety Invariant).

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

§1.2 introduces no new concrete syntax.

**Abstract Syntax**

No AST forms are introduced by §1.2.

**Lexical Constraints**

Not applicable.

**Desugaring Rules**

Not applicable.

**AST Representation**

Not applicable.

##### Static Semantics

**Judgement Forms**

| Judgement                                    | Meaning                                                                     |
| :------------------------------------------- | :-------------------------------------------------------------------------- |
| $\vdash b : k$                               | Behavior instance $b$ has classification $k$                                |
| $I,C \vdash \textsf{doc}(b)$                 | Implementation $I$ documents its IDB choice for $b$ under configuration $C$ |
| $P \vdash \textsf{safe}$                     | Program $P$ contains no `unsafe` blocks and no FFI calls                    |
| $I,C \vdash P \not\Downarrow_{\textsf{UVB}}$ | No execution of $P$ under $I,C$ may exhibit UVB                             |

**Context Structure**

This subsection introduces no typing context; it constrains the global semantic functions $\textsf{Class}$, $\textsf{Permitted}$, and $\textsf{Choice}_I$.

**Typing Rules**

**(B-Defined-Singleton)**
$$
\frac{
\textsf{Class}(b)=\textsf{Defined}
}{
|\textsf{Permitted}(b)| = 1
}
\quad\text{(B-Defined-Singleton)}
$$

**(B-IDB-Choice-Total)**
$$
\frac{
\textsf{Class}(b)=\textsf{IDB}
}{
\forall I,C.\ \textsf{Choice}_I(C,b) \in \textsf{Permitted}(b)
}
\quad\text{(B-IDB-Choice-Total)}
$$

**(B-IDB-Consistency)**
$$
\frac{
\textsf{Class}(b)=\textsf{IDB}
\quad
C_1 \equiv C_2
}{
\textsf{Choice}_I(C_1,b) = \textsf{Choice}_I(C_2,b)
}
\quad\text{(B-IDB-Consistency)}
$$
where $C_1 \equiv C_2$ means equality of all configuration components that the implementation declares relevant to IDB selection.

**(B-IDB-Documented)**
$$
\frac{
\textsf{Class}(b)=\textsf{IDB}
}{
I,C \vdash \textsf{doc}(b)
}
\quad\text{(B-IDB-Documented)}
$$
with the obligation that $\textsf{doc}(b)$ is satisfied iff the Conformance Dossier contains the selected outcome for $b$.

**(B-USB-Safe)**
$$
\frac{
\textsf{Class}(b)=\textsf{USB}
\quad
P \vdash \textsf{safe}
\quad
b \in \textsf{Occurs}(\cdot,P)
}{
\forall o \in \textsf{Permitted}(b).\ \textsf{SafeObs}(o)
}
\quad\text{(B-USB-Safe)}
$$

**Safety Invariant**

**(B-Safe-No-UVB)**
$$
\frac{
I,C \vdash P\ \textsf{wf}
\quad
P \vdash \textsf{safe}
}{
I,C \vdash P \not\Downarrow_{\textsf{UVB}}
}
\quad\text{(B-Safe-No-UVB)}
$$
This invariant MUST be enforced by the type system (Clause 4) and the permission system (§4.5).

**Diagnostic Coordination**

When the same underlying violation is detectable by multiple subsystems at distinct phases, the canonical error diagnostic MUST be selected per Appendix B.4. Implementations MAY additionally emit non-canonical secondary diagnostics that refer to distinct violated rules.

##### Dynamic Semantics

**Runtime Domains**

| Domain     | Notation            | Description                                        |
| :--------- | :------------------ | :------------------------------------------------- |
| Outcomes   | $o \in \mathcal{O}$ | Observable outcomes                                |
| Executions | $\xi \in \Xi$       | Concrete execution traces (implementation-defined) |

**Behavior Realization**

Let $\llbracket P \rrbracket_{I,C} \subseteq \mathcal{O}$ denote the set of outcomes that executions of $P$ under $I,C$ may produce (including nondeterminism).

* If all behavior instances encountered during an execution are **Defined** or **IDB**, then $\llbracket P \rrbracket_{I,C}$ MUST be deterministic with respect to those instances: for Defined instances it is unique per spec; for IDB instances it is uniquely determined by $\textsf{Choice}_I(C,\cdot)$.
* For **USB** instances, the implementation MAY vary outcomes between executions and within an execution, but MUST remain within $\textsf{Permitted}(b)$ for each occurrence.
* For **UVB** instances, if the external invariant required by the UVB instance does not hold at runtime, the outcome set is unconstrained by this specification; if the invariant holds, the outcome MUST be within $\textsf{Permitted}(b)$.

---

#### 1.2.1 Ill-Formed, No Diagnostic Required (IFNDR) 

##### Definition

A program that violates any static-semantic rule of this specification is **ill-formed**. A static-semantic violation is classified as **Ill-Formed, No Diagnostic Required (IFNDR)** when detecting the violation is computationally undecidable or infeasible under the criteria below. For IFNDR violations, an implementation is not required to emit a diagnostic.

When an implementation detects an IFNDR instance, it MUST record the instance in the Conformance Dossier, including the source file path, line number, and IFNDR category.

**Semantic Domains**

| Domain            | Notation                             | Description                                 |
| :---------------- | :----------------------------------- | :------------------------------------------ |
| IFNDR categories  | $c \in \mathcal{C}_{\textsf{IFNDR}}$ | Classification labels for IFNDR instances   |
| IFNDR instances   | $\iota$                              | Triples $(\textsf{path},\textsf{line},c)$   |
| Dossier IFNDR log | $\textsf{IFNDRLog}(D)$               | Multiset of recorded IFNDR instances in $D$ |

**Formal Definition**

Let $\textsf{IFNDR}_{I,C}(P)$ denote that $P$ contains at least one IFNDR-classified violation (whether detected or not).

An IFNDR instance is a record:
$$
\iota ;::=; (\textsf{path} \in \textsf{FilePath},\ \textsf{line} \in \mathbb{N}^+,\ c \in \mathcal{C}_{\textsf{IFNDR}}).
$$

**Auxiliary Definitions**

* $\textsf{Detected}_{I,C}(\iota,P)$ holds iff implementation $I$ under $C$ detects IFNDR instance $\iota$ in $P$.

**Well-Formedness**

1. IFNDR classification MUST be used only for violations of static-semantic rules.
2. If $\textsf{Detected}_{I,C}(\iota,P)$ holds and compilation proceeds, then $\iota \in \textsf{IFNDRLog}(D)$ MUST hold for the produced dossier $D$.

##### Grammar and Syntax

§1.2.1 introduces no new concrete syntax.

##### Static Semantics

**Judgement Forms**

| Judgement                          | Meaning                                            |
| :--------------------------------- | :------------------------------------------------- |
| $I,C \vdash P : \textsf{IFNDR}$    | Program $P$ contains an IFNDR-classified violation |
| $I,C \vdash \iota \in P$           | IFNDR instance $\iota$ is detected in $P$          |
| $D \vdash \textsf{records}(\iota)$ | Dossier $D$ records IFNDR instance $\iota$         |

**Classification Criteria** 

A violation is classified as IFNDR when at least one of the following conditions holds:

1. the violation depends on a property that is undecidable in general;
2. detecting the violation requires super-polynomial analysis in the size of the program; or
3. detecting the violation depends on runtime values that cannot be statically determined.

**IFNDR Categories** 

$$
\mathcal{C}_{\textsf{IFNDR}} ;=;
{
\textsf{OOBInConstEval},\
\textsf{RecursionLimits},\
\textsf{InfiniteType},\
\textsf{UnboundedUnrolling}
}.
$$

**Recording Rule**

**(IFNDR-Record)**
$$
\frac{
I,C \vdash \iota \in P
\quad
\textsf{Compile}_I(C,P)=\textsf{Accept}(X,D,\Delta)
}{
D \vdash \textsf{records}(\iota)
}
\quad\text{(IFNDR-Record)}
$$

##### Dynamic Semantics

**Safety Boundary** 

If an IFNDR violation occurs (whether or not detected), the observable effects of executing the program are **Unspecified Behavior (USB)**. The Safety Invariant (§1.2) MUST still hold: if the execution is within safe code, outcomes MUST NOT exhibit UVB.

Formally, for any $I,C,P$:
$$
P \vdash \textsf{safe} \land \textsf{IFNDR}*{I,C}(P)
;\Rightarrow;
\forall o \in \llbracket P \rrbracket*{I,C}.\ \textsf{SafeObs}(o).
$$

Permitted outcomes include rejection if detected, runtime panic, or safe execution; the specification imposes no determinism requirement in the presence of IFNDR.

---

### 1.3 Reserved Identifiers

##### Definition

A **reserved identifier** is an identifier that MUST NOT be used as a user-defined identifier in a conforming program, because it is reserved for use by this specification or by conforming implementations.

Reserved identifiers include:

1. all reserved keywords defined in §2.6;
2. all identifiers in the reserved namespace `cursive.*`;
3. all identifiers with the prefix `gen_` when introduced by user declarations; and
4. all universe-protected bindings when introduced at module scope (§8).

An implementation MAY reserve additional identifier patterns. Such reservations are **IDB** and MUST be documented in the Conformance Dossier.

**Semantic Domains**

| Domain                  | Notation                         | Description                                       |
| :---------------------- | :------------------------------- | :------------------------------------------------ |
| Identifier lexemes      | $x \in \mathcal{U}^*$            | Unicode scalar sequences forming identifiers      |
| Qualified names         | $q \in \textsf{QName}$           | Dot-separated identifier segments                 |
| Keyword set             | $\textsf{KW}$                    | Reserved keywords (§2.6)                          |
| Reserved namespace      | $\textsf{NS}_{\textsf{cursive}}$ | Qualified names with first segment `cursive`      |
| Universe-protected      | $\textsf{UP}$                    | Predeclared identifiers forbidden at module scope |
| Implementation patterns | $\textsf{ResPat}_I$              | Implementation-reserved predicates (IDB)          |

**Formal Definition**

Let $\textsf{Segments}(q) = \langle s_0,\ldots,s_n\rangle$ be the segments of a qualified name.

Define:
$$
\textsf{InCursiveNS}(q)\ \stackrel{\textsf{def}}{=}\ (\textsf{Segments}(q)_0 = \text{"cursive"}).
$$
$$
\textsf{HasGenPrefix}(x)\ \stackrel{\textsf{def}}{=}\ x \text{ begins with the byte sequence } \text{"gen_"}.
$$

Universe-protected bindings:

* Primitive type names: ${\texttt{i8},\texttt{i16},\texttt{i32},\texttt{i64},\texttt{i128},\texttt{u8},\texttt{u16},\texttt{u32},\texttt{u64},\texttt{u128},\texttt{f16},\texttt{f32},\texttt{f64},\texttt{bool},\texttt{char},\texttt{usize},\texttt{isize}}$.
* Special types: ${\texttt{Self},\texttt{string},\texttt{Modal}}$.
* Async aliases (§15.3): ${\texttt{Async},\texttt{Future},\texttt{Sequence},\texttt{Stream},\texttt{Pipe},\texttt{Exchange}}$.

**Auxiliary Definitions**

* $\textsf{IsModuleScope}(S)$ holds iff scope $S$ is a module scope as defined in Clause 8.
* $\textsf{Declares}(P,S,x)$ holds iff program $P$ introduces a user declaration of identifier $x$ in scope $S$.

**Well-Formedness**

1. If a keyword token appears where an identifier is required, the program is ill-formed.
2. If $\textsf{Declares}(P,S,x)$ and $\textsf{IsModuleScope}(S)$ and $x \in \textsf{UP}$, the program is ill-formed.
3. If $\textsf{Declares}(P,S,x)$ and $\textsf{HasGenPrefix}(x)$, the program is ill-formed.
4. If a user-defined qualified identifier $q$ satisfies $\textsf{InCursiveNS}(q)$, the program is ill-formed.

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

```ebnf
identifier        ::= IDENTIFIER_TOKEN          (* as defined by Clause 2 *)
qname             ::= identifier ("." identifier)+

gen_identifier    ::= "gen_" identifier_tail
identifier_tail   ::= IDENTIFIER_TAIL           (* Clause 2; excluding '.' *)

(* Reserved namespace predicate: InCursiveNS(qname) iff first segment is "cursive". *)
```

**Abstract Syntax**

$$
\textsf{Name} ::= \textsf{Unqual}(x) \mid \textsf{Qual}(\langle x_0,\ldots,x_n\rangle),\quad n \ge 1
$$

**Lexical Constraints**

* Keywords (as defined by §2.6) MUST be tokenized as keywords and MUST NOT be accepted as identifiers in identifier positions.

**Desugaring Rules**

Not applicable.

**AST Representation**

Parsing of a qualified name MUST map `qname` to $\textsf{Qual}(\langle x_0,\ldots,x_n\rangle)$ in segment order.

##### Static Semantics

**Judgement Forms**

| Judgement                               | Meaning                                                                   |
| :-------------------------------------- | :------------------------------------------------------------------------ |
| $\Gamma \vdash x\ \textsf{usableId}$    | Identifier $x$ may be introduced as a user identifier in context $\Gamma$ |
| $\Gamma \vdash q\ \textsf{usableQName}$ | Qualified name $q$ may be introduced by user code                         |
| $\Gamma \vdash \textsf{shadowOK}(x)$    | A declaration of $x$ is permitted in the current scope                    |
| $I \vdash \textsf{warnRes}(x)$          | Identifier $x$ matches an implementation-reserved pattern (warning)       |

**Context Structure**

$\Gamma$ is a name-resolution context containing at least:

* current lexical scope $S$,
* scope kind information (module scope vs non-module),
* the set of in-scope bindings and their namespaces (Clause 8).

**Typing Rules**

**(RID-Keyword)**
$$
\frac{
x \in \textsf{KW}
}{
\Gamma \nvdash x\ \textsf{usableId}
}
\quad\text{(RID-Keyword)}
$$

**(RID-Cursive-NS)**
$$
\frac{
\textsf{InCursiveNS}(q)
}{
\Gamma \nvdash q\ \textsf{usableQName}
}
\quad\text{(RID-Cursive-NS)}
$$

**(RID-Gen-Prefix)**
$$
\frac{
\textsf{HasGenPrefix}(x)
}{
\Gamma \nvdash x\ \textsf{usableId}
}
\quad\text{(RID-Gen-Prefix)}
$$

**(RID-UniverseProtected-Module)**
$$
\frac{
x \in \textsf{UP}
\quad
\textsf{IsModuleScope}(\Gamma.S)
}{
\Gamma \nvdash \textsf{shadowOK}(x)
}
\quad\text{(RID-UniverseProtected-Module)}
$$

**(RID-Impl-Reserved-Warn)**
$$
\frac{
\textsf{ResPat}_I(x)=\textsf{true}
}{
I \vdash \textsf{warnRes}(x)
}
\quad\text{(RID-Impl-Reserved-Warn)}
$$

**Diagnostics**

| Code       | Condition                                                  | Rule Violated                  |
| :--------- | :--------------------------------------------------------- | :----------------------------- |
| E-CNF-0401 | Reserved keyword used as identifier.                       | (RID-Keyword)                  |
| E-CNF-0402 | Reserved namespace `cursive.*` used by user code.          | (RID-Cursive-NS)               |
| E-CNF-0403 | Primitive type name shadowed at module scope.              | (RID-UniverseProtected-Module) |
| E-CNF-0404 | Shadowing of `Self`, `string`, or `Modal` at module scope. | (RID-UniverseProtected-Module) |
| E-CNF-0405 | Shadowing of async type alias at module scope.             | (RID-UniverseProtected-Module) |
| W-CNF-0401 | Identifier matches an implementation-reserved pattern.     | (RID-Impl-Reserved-Warn)       |
| E-CNF-0406 | User declaration uses `gen_` prefix.                       | (RID-Gen-Prefix)               |

##### Dynamic Semantics

Reserved identifier constraints affect only compilation; they introduce no runtime behavior.

---

### 1.4 Implementation Limits

##### Definition

**Implementation limits** are capacity bounds that a conforming implementation MUST support at least at the minimum values specified in this section. Each conforming implementation MUST define (and document) its effective limits; each effective limit MUST be greater than or equal to the corresponding minimum.

A program that exceeds any effective implementation limit is ill-formed and MUST be rejected with `E-CNF-0301`.

**Semantic Domains**

| Domain           | Notation                   | Description                             |
| :--------------- | :------------------------- | :-------------------------------------- |
| Limit kinds      | $\ell \in \mathcal{L}$     | Named capacity constraints              |
| Measurements     | $m \in \mathbb{N}$         | Natural-number size metrics             |
| Effective limits | $\textsf{Lim}_{I,C}(\ell)$ | Implementation-defined effective limits |
| Minimum limits   | $\textsf{MinLim}(\ell)$    | Specification-mandated minimum limits   |

**Formal Definition**

Let $\textsf{Measure}(P,\ell)$ compute the relevant metric of program $P$ for limit kind $\ell$.

Conformance requires:
$$
\forall I,C,\ell.\ \textsf{Lim}_{I,C}(\ell) \ge \textsf{MinLim}(\ell).
$$

A program is within limits iff:
$$
\textsf{LimitsOK}*{I,C}(P)\ \stackrel{\textsf{def}}{=}\ \forall \ell \in \mathcal{L}.\ \textsf{Measure}(P,\ell) \le \textsf{Lim}*{I,C}(\ell).
$$

**Minimum Guaranteed Limits** 

Define $\mathcal{L}$ and $\textsf{MinLim}$ by:

| $\ell$                         | Meaning                                  | $\textsf{MinLim}(\ell)$ |
| :----------------------------- | :--------------------------------------- | :---------------------- |
| $\ell_{\textsf{SourceSize}}$   | normalized source size                   | $2^{20}$ bytes (1 MiB)  |
| $\ell_{\textsf{LogicalLines}}$ | logical line count                       | $65535$                 |
| $\ell_{\textsf{LineLength}}$   | maximum line length                      | $16384$ Unicode scalars |
| $\ell_{\textsf{NestingDepth}}$ | maximum syntactic nesting depth          | $256$                   |
| $\ell_{\textsf{IdentLength}}$  | maximum identifier length                | $1023$ Unicode scalars  |
| $\ell_{\textsf{ParamCount}}$   | maximum parameters per procedure         | $255$                   |
| $\ell_{\textsf{FieldCount}}$   | maximum fields per aggregate declaration | $1024$                  |

Minimum guaranteed capacities for compile-time execution are specified in §17.8.

**Auxiliary Definitions**

* $\textsf{Measure}(P,\ell_{\textsf{NestingDepth}})$ MUST be computed using the syntactic nesting measures defined in §2.13.

**Well-Formedness**

1. A conforming implementation MUST document each $\textsf{Lim}_{I,C}(\ell)$ in the Conformance Dossier as an IDB choice.
2. If $\neg\textsf{LimitsOK}_{I,C}(P)$ then $P$ is ill-formed.

##### Grammar and Syntax

No concrete syntax is introduced by §1.4.

##### Static Semantics

**Judgement Forms**

| Judgement                                | Meaning                                |
| :--------------------------------------- | :------------------------------------- |
| $I,C \vdash P\ \textsf{limitsOK}$        | Program $P$ is within effective limits |
| $I,C \vdash P\ \textsf{limitsBad}(\ell)$ | Program $P$ exceeds limit $\ell$       |

**Typing Rules**

**(L-OK)**
$$
\frac{
\textsf{LimitsOK}_{I,C}(P)
}{
I,C \vdash P\ \textsf{limitsOK}
}
\quad\text{(L-OK)}
$$

**(L-Exceed)**
$$
\frac{
\textsf{Measure}(P,\ell) > \textsf{Lim}_{I,C}(\ell)
}{
I,C \vdash P\ \textsf{limitsBad}(\ell)
}
\quad\text{(L-Exceed)}
$$

**Diagnostics** 

| Code       | Condition                                      | Rule Violated |
| :--------- | :--------------------------------------------- | :------------ |
| E-CNF-0301 | An effective implementation limit is exceeded. | (L-Exceed)    |

##### Dynamic Semantics

Implementation limits affect only compilation; they introduce no runtime behavior.

---

### 1.5 Language Evolution

##### Definition

**Language evolution** defines how source meaning is parameterized by language version, editions, feature stability classes, deprecation/removal, feature flags, and vendor extensions.

A Cursive language **version identifier** MUST be a semantic version triple $(\textsf{MAJOR},\textsf{MINOR},\textsf{PATCH}) \in \mathbb{N}^3$ and MUST be represented in the form `MAJOR.MINOR.PATCH`.

A program MUST declare its target language version in a project manifest; the mechanism by which a manifest is provided is implementation-defined.

A conforming implementation MUST reject a program if the declared MAJOR version is not supported.

If an implementation supports editions, it MUST allow a program to declare a target edition, and MUST forbid mixing semantics from different editions within a single compilation unit.

Every language feature MUST be classified into exactly one **stability class**: Stable, Preview, or Experimental. Preview and Experimental features MUST be controlled by feature flags.

Vendor extensions MUST use a reverse-domain-style namespace and MUST NOT:

1. alter the meaning of conforming programs that do not use the extension; or
2. suppress any diagnostic required by this specification.

**Semantic Domains**

| Domain            | Notation                                                            | Description                          |
| :---------------- | :------------------------------------------------------------------ | :----------------------------------- |
| Semantic versions | $v \in \textsf{SemVer}$                                             | $v = (M,m,p) \in \mathbb{N}^3$       |
| Editions          | $e \in \textsf{EditionId}$                                          | Edition identifiers                  |
| Features          | $f \in \textsf{FeatureId}$                                          | Feature identifiers                  |
| Stability classes | $k \in {\textsf{Stable},\textsf{Preview},\textsf{Experimental}}$    | Feature stability class              |
| Feature flags     | $\phi \in \textsf{FlagId}$                                          | Identifiers enabling opt-in features |
| Enabled flags     | $\Phi \subseteq \textsf{FlagId}$                                    | Flag set in configuration            |
| Feature status    | $\sigma \in {\textsf{Active},\textsf{Deprecated},\textsf{Removed}}$ | Lifecycle status                     |
| Compilation units | $U$                                                                 | As defined in §8.1                   |

**Formal Definition**

$$
\textsf{SemVer} \stackrel{\textsf{def}}{=} \mathbb{N}\times\mathbb{N}\times\mathbb{N}.
$$

Each configuration $C$ provides:

* $\textsf{TargetVer}(C) \in \textsf{SemVer}$,
* optional $\textsf{TargetEd}(C) \in \textsf{EditionId}$,
* $\textsf{Flags}(C) \subseteq \textsf{FlagId}$,
* supported majors $\textsf{SupportedMajors}(I) \subseteq \mathbb{N}$.

A feature catalog (normative, per version/edition) is modeled as:

* $\textsf{ClassOf}(f)=k$ (exactly one stability class),
* $\textsf{RequiresFlag}(f) \in \textsf{FlagId} \cup {\bot}$,
* $\textsf{Status}(f,v,e)=\sigma$ (status of feature in version $v$ and edition $e$).

**Auxiliary Definitions**

* $\textsf{UsesFeature}(P,f)$ holds iff $P$ contains any construct whose well-formedness or semantics is guarded by $f$.
* $\textsf{UsesExtension}(P,f)$ holds iff $f$ is vendor-defined (reverse-domain namespace) and is referenced by $P$.
* $\textsf{VendorId}(f)$ holds iff $f$ is a reverse-domain namespaced identifier.

**Well-Formedness**

1. Every feature MUST satisfy $\exists!k.\ \textsf{ClassOf}(f)=k$.
2. If $\textsf{ClassOf}(f)\in{\textsf{Preview},\textsf{Experimental}}$, then $\textsf{RequiresFlag}(f)\neq\bot$.
3. If $\textsf{Status}(f,v,e)=\textsf{Removed}$, then any $\textsf{UsesFeature}(P,f)$ is ill-formed for target $(v,e)$.

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

```ebnf
semver        ::= nat "." nat "." nat
nat           ::= DIGIT+           (* no leading sign; base-10 *)

edition_id    ::= IDENTIFIER_TOKEN (* lexical identifier; Clause 2 *)

flag_id       ::= IDENTIFIER_TOKEN | vendor_feature_id
vendor_feature_id ::= domain_label "." domain_label ("." domain_label)*
domain_label  ::= ALPHA (ALPHA | DIGIT | "-")*
```

**Abstract Syntax**

$$
\textsf{SemVerLit} ::= \textsf{SemVer}(M,m,p)
$$
$$
\textsf{FeatureRef} ::= \textsf{UnqualFlag}(x) \mid \textsf{VendorFlag}(\langle d_0,\ldots,d_n\rangle)
$$

**Lexical Constraints**

* A vendor feature identifier MUST have at least two dot-separated domain labels.
* The reserved namespace predicate $\textsf{InCursiveNS}$ from §1.3 applies to vendor feature identifiers and flags; identifiers in `cursive.*` MUST NOT be used for vendor-defined features.

**Desugaring Rules**

Not applicable.

**AST Representation**

Parsing of `semver` MUST yield $\textsf{SemVer}(M,m,p)$ with $M,m,p$ in base-10.

##### Static Semantics

**Judgement Forms**

| Judgement                                 | Meaning                                              |
| :---------------------------------------- | :--------------------------------------------------- |
| $I \vdash v\ \textsf{supported}$          | Implementation supports target version $v$           |
| $I,C \vdash \phi\ \textsf{known}$         | Flag identifier $\phi$ is recognized                 |
| $I,C \vdash P\ \textsf{flagsOK}$          | All required flags for used features are enabled     |
| $I,C \vdash U\ \textsf{editionOK}$        | No edition mixing occurs within compilation unit $U$ |
| $I,C \vdash P\ \textsf{deprecatedUse}(f)$ | $P$ uses deprecated feature $f$ under $(v,e)$        |
| $I,C \vdash P\ \textsf{removedUse}(f)$    | $P$ uses removed feature $f$ under $(v,e)$           |

**Typing Rules**

**(V-Major-Supported)**
$$
\frac{
\textsf{TargetVer}(C)=(M,m,p)
\quad
M \in \textsf{SupportedMajors}(I)
}{
I \vdash \textsf{TargetVer}(C)\ \textsf{supported}
}
\quad\text{(V-Major-Supported)}
$$

**(V-Major-Unsupported)**
$$
\frac{
\textsf{TargetVer}(C)=(M,m,p)
\quad
M \notin \textsf{SupportedMajors}(I)
}{
\textsf{Emit}(\texttt{E-CNF-0601})
}
\quad\text{(V-Major-Unsupported)}
$$

**(F-Unknown-Flag)**
$$
\frac{
\phi \in \textsf{Flags}(C)
\quad
\neg(I,C \vdash \phi\ \textsf{known})
}{
\textsf{Emit}(\texttt{E-CNF-0602})
}
\quad\text{(F-Unknown-Flag)}
$$

**(F-Required-Flag-Missing)**
$$
\frac{
\textsf{UsesFeature}(P,f)
\quad
\textsf{RequiresFlag}(f)=\phi
\quad
\phi \notin \textsf{Flags}(C)
}{
\textsf{Emit}(\texttt{E-CNF-0603})
}
\quad\text{(F-Required-Flag-Missing)}
$$

**(E-Edition-No-Mix)**
$$
\frac{
U \text{ contains source components with effective editions } e_1,\ldots,e_n
\quad
\exists i,j.\ e_i \ne e_j
}{
\textsf{Emit}(\texttt{E-CNF-0604})
}
\quad\text{(E-Edition-No-Mix)}
$$

**(F-Deprecated-Warn)**
$$
\frac{
\textsf{UsesFeature}(P,f)
\quad
\textsf{Status}(f,\textsf{TargetVer}(C),\textsf{TargetEd}(C))=\textsf{Deprecated}
}{
\textsf{Emit}(\texttt{W-CNF-0601})
}
\quad\text{(F-Deprecated-Warn)}
$$

**(F-Removed-Error)**
$$
\frac{
\textsf{UsesFeature}(P,f)
\quad
\textsf{Status}(f,\textsf{TargetVer}(C),\textsf{TargetEd}(C))=\textsf{Removed}
}{
\textsf{Emit}(\texttt{E-CNF-0605})
}
\quad\text{(F-Removed-Error)}
$$

**Vendor Extension Constraints**

* (**Ext-Namespacing**) If $\textsf{VendorId}(f)$ then $f$ MUST be reverse-domain-style and MUST NOT lie in the reserved namespace `cursive.*` (§1.3).
* (**Ext-Noninterference**) If $\neg\textsf{UsesExtension}(P,f)$ for all vendor features $f$, then enabling or disabling vendor extensions MUST NOT change the meaning (set of permitted outcomes) of $P$.
* (**Ext-Diagnostics**) Required diagnostics for violations of this specification MUST be emitted regardless of vendor extensions.

**Diagnostics**

| Code       | Condition                                               | Rule Violated             |
| :--------- | :------------------------------------------------------ | :------------------------ |
| E-CNF-0601 | Declared MAJOR version not supported by implementation. | (V-Major-Unsupported)     |
| E-CNF-0602 | Unknown feature flag.                                   | (F-Unknown-Flag)          |
| E-CNF-0603 | Feature requires flag that is not enabled.              | (F-Required-Flag-Missing) |
| E-CNF-0604 | Edition mixing within compilation unit.                 | (E-Edition-No-Mix)        |
| W-CNF-0601 | Use of deprecated feature.                              | (F-Deprecated-Warn)       |
| E-CNF-0605 | Use of removed feature.                                 | (F-Removed-Error)         |

##### Dynamic Semantics

Language meaning is parameterized by $(I,C)$ through $(\textsf{TargetVer}(C),\textsf{TargetEd}(C),\textsf{Flags}(C))$. For any program $P$, executions are constrained by the behavior classification rules (§1.2) and by the version/edition/flag gating rules above.

Within a fixed MAJOR version, conforming implementations MUST preserve source-level meaning across MINOR version increments for programs that are conforming under the earlier MINOR version and do not rely on removed features; any change in permitted outcomes across MINOR increments is allowed only where the affected behavior is classified as IDB or USB.

---

### 1.6 Foundational Semantic Concepts

##### Definition

A **Program Point** is a unique location in the abstract control-flow representation of a procedure body. Program points are used as indices for compile-time state tracking (e.g., key state, move state, and capability state) and for defining scope boundaries.

A **Lexical Scope** is a contiguous region of source text that defines a lifetime boundary for bindings, keys, and deferred actions. Each lexical scope MUST be associated with an entry program point, an exit program point, and an optional parent scope; these components define a nesting structure.

**Semantic Domains**

| Domain                | Notation                      | Description                                               |
| :-------------------- | :---------------------------- | :-------------------------------------------------------- |
| Procedure identifiers | $\textsf{ProcedureId}$        | Unique procedure identity space                           |
| CFG node identifiers  | $\textsf{CFGNodeId}$          | Unique node identity space per CFG                        |
| Program points        | $p \in \textsf{ProgramPoint}$ | $(\textsf{ProcedureId},\textsf{CFGNodeId})$               |
| Control-flow graphs   | $\textsf{CFG}$                | Directed graphs over CFG nodes                            |
| Lexical scopes        | $S \in \textsf{LexicalScope}$ | $(p_{\textsf{in}},p_{\textsf{out}},S_{\textsf{parent}}?)$ |

**Formal Definition**

$$
\textsf{ProgramPoint} ;::=; (\textsf{ProcedureId},\ \textsf{CFGNodeId}).
$$

Let $\textsf{Edges}(\pi)$ denote the edge relation of the CFG for procedure $\pi$. Define reachability $\leadsto^*$ as the reflexive-transitive closure of $\leadsto$.

The control-flow partial order is:
$$
p_1 \le_{cf} p_2\ \stackrel{\textsf{def}}{=}\ p_2 \text{ is reachable from } p_1 \text{ via } \leadsto^*.
$$

Lexical scopes:
$$
\textsf{LexicalScope} ;::=; (\textsf{EntryPoint},\ \textsf{ExitPoint},\ \textsf{Parent}?).
$$

**Auxiliary Definitions**

* $\textsf{Parent}(S)$ is undefined iff $S$ is the procedure-level scope.
* $\le_{\textsf{nest}}$ is the reflexive-transitive closure of the parent relation: $S_1 \le_{\textsf{nest}} S_2$ iff $S_1$ is $S_2$ or a nested descendant of $S_2$.

**Well-Formedness**

1. (**Uniqueness**) For any procedure $\pi$, CFG node identifiers MUST be unique within $\pi$.
2. (**Scope nesting**) The parent relation on lexical scopes MUST form a forest rooted at procedure scopes (acyclic, each scope has at most one parent).
3. (**Scope boundaries**) Each scope’s entry and exit program points MUST belong to the same enclosing procedure.

##### Grammar and Syntax

§1.6 introduces no concrete syntax.

##### Static Semantics

**Judgement Forms**

| Judgement                     | Meaning                                         |
| :---------------------------- | :---------------------------------------------- |
| $\pi \vdash p\ \textsf{pp}$   | $p$ is a valid program point in procedure $\pi$ |
| $\pi \vdash p_1 \le_{cf} p_2$ | $p_2$ is reachable from $p_1$ in $\pi$          |
| $\vdash S\ \textsf{scope-wf}$ | Lexical scope record $S$ is well-formed         |

**Typing Rules**

**(PP-Valid)**
$$
\frac{
p = (\pi,n)
\quad
n \in \textsf{Nodes}(\textsf{CFG}(\pi))
}{
\pi \vdash p\ \textsf{pp}
}
\quad\text{(PP-Valid)}
$$

**(CF-Reach)**
$$
\frac{
\pi \vdash p_1\ \textsf{pp}
\quad
\pi \vdash p_2\ \textsf{pp}
\quad
p_1 \leadsto^* p_2
}{
\pi \vdash p_1 \le_{cf} p_2
}
\quad\text{(CF-Reach)}
$$

**(Scope-WF)**
$$
\frac{
S = (p_{in},p_{out},S_{par}?)
\quad
\exists \pi.\ \pi \vdash p_{in}\ \textsf{pp} \land \pi \vdash p_{out}\ \textsf{pp}
\quad
(S_{par}?=\bot)\ \lor\ \vdash S_{par}\ \textsf{scope-wf}
}{
\vdash S\ \textsf{scope-wf}
}
\quad\text{(Scope-WF)}
$$

##### Dynamic Semantics

**Runtime Domains**

| Domain        | Notation   | Description                           |
| :------------ | :--------- | :------------------------------------ |
| Program point | $p$        | Current program point label           |
| Scope stack   | $\Sigma_S$ | Stack of active lexical scopes        |
| Runtime state | $\sigma$   | Store/heap/etc. (defined in Clause 3) |

**Small-Step State Form**

Evaluation states MAY be labeled by program point and scope stack:
$$
\langle \textsf{State}, p, \Sigma_S \rangle.
$$

**Scope Entry/Exit Events**

Whenever execution enters the entry point of a lexical scope $S$, $S$ MUST be pushed onto $\Sigma_S$. Whenever execution reaches the exit point of $S$ along any control-flow path, $S$ MUST be popped from $\Sigma_S$ and all scope-exit cleanup actions mandated by other clauses (e.g., destructor invocation, key release, deferred actions) MUST be performed in the order specified by those clauses.

§1.6 does not otherwise constrain expression evaluation order or runtime memory behavior; those are specified in Clauses 3, 12, 14–16, and 17.

## Clause 3: The Object and Memory Model 

### 3.1 Foundational Principles

##### Definition

A Cursive program execution SHALL be **memory-safe** iff it satisfies **Liveness** and **Aliasing** for all dynamically executed operations that access storage. A conforming implementation operating in Safe mode MUST reject any program for which these properties cannot be established by the static semantics of this specification. 

**Liveness** SHALL mean: every dynamic storage access MUST target storage that exists and is not deallocated at the moment of access.

**Aliasing** SHALL mean: for each storage location, concurrent and/or overlapping accesses MUST be consistent with the permissions and synchronization disciplines of the language (including unique exclusivity and key disjointness). 

**Semantic Domains**

| Domain                 | Notation                    | Description                                                |
| :--------------------- | :-------------------------- | :--------------------------------------------------------- |
| Executions             | $\mathcal{X}$               | Sequences of runtime configurations produced by evaluation |
| Runtime configurations | $C$                         | A machine state containing store, stacks, and control      |
| Locations              | $\ell \in \mathsf{Loc}$     | Abstract addresses of storage cells                        |
| Stores                 | $\sigma \in \mathsf{Store}$ | Runtime memory state                                       |
| Permissions            | $P \in \mathsf{Perm}$       | Access disciplines (see §4.5)                              |
| Keys                   | $k \in \mathsf{Key}$        | Synchronization resources (see §14)                        |
| UB class               | $\mathsf{UVB}$              | Unverifiable behavior (see §1.2)                           |

**Formal Definition**

Let $C \xrightarrow{*} C'$ denote a finite sequence of runtime steps (Clause 12 defines evaluation; this clause constrains storage behavior). Let $\mathsf{Alloc}(C) \subseteq \mathsf{Loc}$ be the set of allocated locations in configuration $C$, and let $\mathsf{Live}(C,\ell)$ be the predicate “$\ell \in \mathsf{Alloc}(C)$ and $\ell$’s storage duration has not ended in $C$”.

A configuration $C$ is **live-safe** iff for every executed primitive storage access event $\mathsf{Access}(\ell)$ in $C$ (including reads, writes, and dereferences), $\mathsf{Live}(C,\ell)$ holds.

A configuration $C$ is **alias-safe** iff for every location $\ell$, the multiset of active accesses to $\ell$ in $C$ is consistent with the permission/key rules (defined in §4.5 and §14.1).

An execution $\chi \in \mathcal{X}$ is **memory-safe** iff every configuration $C$ along $\chi$ is live-safe and alias-safe.

**Auxiliary Definitions**

* $\mathsf{Access}(\ell)$ ranges over dynamic events that read/write/dereference $\ell$.
* “Active access” is the language-defined access set induced by evaluation contexts and borrowing rules (see §6.3 and §4.5).

**Well-Formedness**

1. In Safe mode, any program that can produce a configuration violating live-safe or alias-safe MUST be rejected at compile time.
2. In Unsafe contexts (Clause 3.8), violations MAY compile, but any execution that violates live-safe or alias-safe constitutes $\mathsf{UVB}$.

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

No dedicated surface syntax is introduced by this subsection.

**Abstract Syntax**

No dedicated AST node is introduced by this subsection.

**Desugaring Rules**

None.

**AST Representation**

N/A.

##### Static Semantics

**Judgement Forms**

| Judgement                        | Meaning                                                                                                        |
| :------------------------------- | :------------------------------------------------------------------------------------------------------------- |
| $\Gamma \vdash \mathsf{safe}(p)$ | Program $p$ satisfies the static conditions sufficient for memory-safety (by construction of the typing rules) |
| $\Gamma \vdash e : T$            | Expression typing (defined elsewhere; used here by reference)                                                  |

**Normative Requirements**

A conforming implementation in Safe mode MUST enforce:

* liveness via deterministic destruction (§3.6), pointer validity tracking (§6.3), and provenance/escape analysis (§3.3); 
* aliasing via the permission system (§4.5) and the key system (§14.1). 

##### Dynamic Semantics

Any dynamic violation of Liveness or Aliasing (as defined above) constitutes $\mathsf{UVB}$ (Clause 1.2), regardless of whether the program was accepted in Unsafe contexts. Safe-mode programs SHALL admit no such executions.

---

### 3.2 The Object Model

##### Definition

An **object** is a runtime entity consisting of typed storage with a storage duration, an allocation identity, and an initialization state. An object MUST occupy a contiguous region of storage aligned for its type, and its lifetime MUST be bounded by its storage duration. 

**Semantic Domains**

| Domain      | Notation                     | Description                       |
| :---------- | :--------------------------- | :-------------------------------- |
| Types       | $T \in \mathsf{Type}$        | Static types (Clauses 4–7)        |
| Durations   | $d \in \mathsf{Dur}$         | Storage durations                 |
| Locations   | $\ell \in \mathsf{Loc}$      | Addresses of allocated storage    |
| Objects     | $o \in \mathsf{Obj}$         | Allocated typed storage instances |
| Init states | $\iota \in \mathsf{Init}(T)$ | Initialization masks for $T$      |
| Provenances | $\pi \in \mathsf{Prov}$      | Provenance tags (see §3.3)        |

**Formal Definition**

Storage durations:
[
\mathsf{Dur} ::= \mathsf{Global} \mid \mathsf{Heap} \mid \mathsf{Stack}(S) \mid \mathsf{Region}(R)
]
where $S$ ranges over lexical scopes and $R$ ranges over region frames (see §3.7). 

An object is a tuple:
[
o \triangleq \langle \ell, T, d, \iota \rangle
]
where:

* $\ell$ is allocated storage,
* $T$ is the object’s static type,
* $d$ is the object’s storage duration,
* $\iota$ is the initialization state of the object’s subobjects.

Let $\mathsf{prov} : \mathsf{Dur} \to \mathsf{Prov}$ be:
[
\mathsf{prov}(\mathsf{Global})=\pi_{\mathsf{Global}},\quad
\mathsf{prov}(\mathsf{Heap})=\pi_{\mathsf{Heap}},\quad
\mathsf{prov}(\mathsf{Stack}(S))=\pi_{\mathsf{Stack}}(S),\quad
\mathsf{prov}(\mathsf{Region}(R))=\pi_{\mathsf{Region}}(R)
]
and let $\bot \in \mathsf{Prov}$ denote “no provenance bound” (see §3.3). 

**Object Lifetime**

An object $o=\langle\ell,T,d,\iota\rangle$ exists from its allocation event until its deallocation event determined by $d$:

* $\mathsf{Global}$ objects persist until program termination;
* $\mathsf{Stack}(S)$ objects persist until scope $S$ exits (normal or unwinding);
* $\mathsf{Region}(R)$ objects persist until region $R$ exits (normal or unwinding);
* $\mathsf{Heap}$ objects persist until deallocated (or until termination if not deallocated). 

**Auxiliary Definitions**

* $\mathsf{needsDrop}(T)$: predicate that is true iff values of type $T$ require destruction actions (Drop hook and/or recursive field destruction).
* $\mathsf{subobjs}(T)$: a structural decomposition of $T$ into immediate subobjects (defined by Clause 5 for aggregates).

**Well-Formedness**

1. For each allocated location $\ell$, there is at most one owning object header $\langle \ell, T, d, \iota\rangle$; subobjects are represented as derived ranges within $\ell$’s storage.
2. Any access to a subobject whose initialization state is “uninitialized” constitutes $\mathsf{UVB}$ in Unsafe contexts and SHALL be statically rejected in Safe contexts (via §3.4/§3.5 state tracking).

##### Grammar and Syntax

No dedicated surface syntax is introduced by this subsection.

##### Static Semantics

**Normative Mapping of Constructs to Storage Duration**

* `static` bindings denote $\mathsf{Global}$ duration objects. 
* `let`/`var` bindings denote $\mathsf{Stack}(S)$ duration objects for the scope $S$ that introduces them. 
* Region allocations denote $\mathsf{Region}(R)$ duration objects for the chosen region frame $R$ (see §3.7). 
* Heap-allocated objects denote $\mathsf{Heap}$ duration objects. 

##### Dynamic Semantics

**Runtime Domains**

| Domain        | Notation      | Description                   |
| :------------ | :------------ | :---------------------------- |
| Store         | $\sigma$      | Memory state                  |
| Env           | $\rho$        | Variable environment          |
| Region stack  | $\mathcal{R}$ | Stack of active region frames |
| Cleanup stack | $\kappa$      | LIFO stack of cleanup actions |

**Store Model (Abstract)**

A runtime store $\sigma$ SHALL provide:

* allocation: $\mathsf{alloc}(\sigma, T, d) = \langle \ell, \sigma' \rangle$ producing a fresh location $\ell$ aligned for $T$ and tagged with duration $d$;
* read/write over initialized subobjects: $\mathsf{read}(\sigma,\ell,p)$ and $\mathsf{write}(\sigma,\ell,p,v)$ where $p$ is a subobject path;
* initialization updates: $\mathsf{setUninit}(\sigma,\ell,p)$ for moves (see §3.5);
* deallocation: $\mathsf{free}(\sigma,\ell)$, permitted only when the duration rules and responsibility rules allow it (see §3.6, §3.7). 

##### Memory and Layout

**Applicability**

This layout specification applies to all objects $o=\langle\ell,T,d,\iota\rangle$ that are addressable (including via FFI boundaries) and for which `sizeof`/`alignof` are defined.

**Size and Alignment**

For any object of type $T$:
[
\mathsf{sizeof}(o)=\mathsf{sizeof}(T),\qquad \mathsf{alignof}(o)=\mathsf{alignof}(T)
]
where $\mathsf{sizeof}$ and $\mathsf{alignof}$ are defined by the type layout rules (Clause 5). 

**Layout Invariants**

1. The address of $\ell$ MUST be a multiple of $\mathsf{alignof}(T)$.
2. The object’s storage MUST contain at least $\mathsf{sizeof}(T)$ addressable bytes.
3. Endianness, pointer width, and padding byte values are platform-defined unless constrained by Clause 5 or Clause 21 (FFI).

---

### 3.3 The Provenance Model

##### Definition

A **provenance tag** classifies the maximum permitted lifetime of referenced storage for the purpose of preventing escapes of short-lived storage into longer-lived contexts. Implementations MUST infer provenance for pointer/reference-producing expressions and MUST apply the escape rule to assignments, returns, captures, and any other operation that stores a value into a location with independent lifetime.

**Semantic Domains**

| Domain           | Notation                  | Description                      |
| :--------------- | :------------------------ | :------------------------------- |
| Provenances      | $\pi \in \mathsf{Prov}$   | Provenance tags                  |
| Scopes           | $S \in \mathsf{Scope}$    | Lexical scopes                   |
| Regions          | $R \in \mathsf{RegionId}$ | Region frames                    |
| Strict order     | $<$                       | Guaranteed earlier deallocation  |
| Non-strict order | $\le$                     | Guaranteed no-later deallocation |

**Formal Definition**

[
\mathsf{Prov} ::= \pi_{\mathsf{Global}} \mid \pi_{\mathsf{Heap}} \mid \pi_{\mathsf{Stack}}(S) \mid \pi_{\mathsf{Region}}(R) \mid \bot
]

**Provenance Order**

The relations $<$ and $\le$ are constrained as follows (where “inner/outer” refers to lexical containment of region frames): 

1. If region $R_{\text{inner}}$ is nested within $R_{\text{outer}}$, then:
   [
   \pi_{\mathsf{Region}}(R_{\text{inner}}) < \pi_{\mathsf{Region}}(R_{\text{outer}})
   ]
2. If region $R$ is contained in scope $S$, then:
   [
   \pi_{\mathsf{Region}}(R) < \pi_{\mathsf{Stack}}(S)
   ]
3. For any scope $S$:
   [
   \pi_{\mathsf{Stack}}(S) < \pi_{\mathsf{Heap}}
   ]
4. Heap-to-global is non-strict:
   [
   \pi_{\mathsf{Heap}} \le \pi_{\mathsf{Global}}
   ]
5. Global-to-$\bot$ is non-strict:
   [
   \pi_{\mathsf{Global}} \le \bot
   ]
   Additionally, $\le$ MUST be reflexive and transitive, and $<$ MUST be transitive and imply $\le$ (i.e., $\pi_1<\pi_2 \Rightarrow \pi_1\le\pi_2$).

**Auxiliary Definitions**

* $\mathsf{provOfVar}(\Gamma,x)$ returns the provenance of the storage bound to $x$ (global vs stack scope) per binding introduction. 
* $\mathsf{provOfTarget}(p)$ returns the provenance of the location targeted by pointer value $p$ when $p$ is valid (see §6.3).

**Well-Formedness**

1. Any store operation that places a value with provenance $\pi_e$ into a location with provenance $\pi_\ell$ MUST satisfy the escape rule below.
2. The provenance $\bot$ MAY be treated as “unbounded” and SHALL NOT constrain storage (i.e., it is not considered to escape any location). 

##### Grammar and Syntax

No dedicated surface syntax is introduced by this subsection; provenance is a static attribute used in judgements.

##### Static Semantics

**Judgement Forms**

| Judgement                                        | Meaning                                                                     |
| :----------------------------------------------- | :-------------------------------------------------------------------------- |
| $\Gamma \vdash e : T \triangleright \pi$         | Expression $e$ has type $T$ and provenance $\pi$                            |
| $\Gamma \vdash \mathsf{storeOK}(\pi_e,\pi_\ell)$ | Storing provenance $\pi_e$ into location provenance $\pi_\ell$ is permitted |

**Provenance Inference Rules**

The following rules are syntax-directed over common provenance-bearing constructs. (Additional constructs MUST be defined by extension in the clauses that introduce them.) 

[
\frac{}{\Gamma \vdash n : \mathsf{Int} \triangleright \bot};(\textsc{P-Literal})
]

[
\frac{x\ \text{is declared}\ \texttt{static}}{\Gamma \vdash x : T \triangleright \pi_{\mathsf{Global}}};(\textsc{P-Global})
]

[
\frac{x\ \text{is declared in scope}\ S}{\Gamma \vdash x : T \triangleright \pi_{\mathsf{Stack}}(S)};(\textsc{P-Local})
]

Let $R$ be the chosen active region frame (see §3.7):
[
\frac{\Gamma \vdash e : T}{\Gamma \vdash \widehat{\ }e : T \triangleright \pi_{\mathsf{Region}}(R)};(\textsc{P-Region-Alloc})
]

[
\frac{\Gamma \vdash e : R \triangleright \pi}{\Gamma \vdash e.f : T \triangleright \pi};(\textsc{P-Field})
]

[
\frac{\Gamma \vdash e : A[T] \triangleright \pi}{\Gamma \vdash e[i] : T \triangleright \pi};(\textsc{P-Index})
]

[
\frac{\Gamma \vdash p : \mathsf{Ptr}\langle T\rangle@\mathsf{Valid} \triangleright \pi_p \quad \mathsf{provOfTarget}(p)=\pi_t}{\Gamma \vdash *p : T \triangleright \pi_t};(\textsc{P-Deref})
]

**Escape Rule**

Let $p$ range over store targets (bindings, fields, array elements, globals, captures, returns) with location-provenance $\pi_p$. A store of $e$ into $p$ is ill-formed iff $\pi_e < \pi_p$:

[
\frac{\Gamma \vdash e : T \triangleright \pi_e \quad \mathsf{provOfPlace}(\Gamma,p)=\pi_p \quad \pi_e < \pi_p}{\Gamma \vdash \mathsf{store}(p,e)\ \text{ill-formed}};(\textsc{P-Escape})
]
Violations MUST be diagnosed as **E-MEM-3020**. 

**Heap-to-Global Deallocation Constraint**

If the address of heap-allocated storage is stored into a location whose provenance is $\pi_{\mathsf{Global}}$, that heap-allocated storage MUST NOT be deallocated during program execution; deallocating it constitutes $\mathsf{UVB}$. 

**Diagnostics**

| Code       | Condition                                                       | Rule Violated       |
| :--------- | :-------------------------------------------------------------- | :------------------ |
| E-MEM-3020 | A value with provenance $\pi_e$ is stored where $\pi_e < \pi_p$ | (\textsc{P-Escape}) |

##### Dynamic Semantics

Provenance is a static constraint; it does not require a runtime representation. Any execution that violates the intended lifetime ordering (including via unsafe operations that bypass escape checks) constitutes $\mathsf{UVB}$.

---

### 3.4 The Binding Model

##### Definition

A **binding** is a named association between an identifier and a storage location. A binding MUST have a type, a mutability mode (`let` or `var`), a movability mode (selected by `=` or `:=`), and an initialization state. A binding established with `=` is **Movable**: responsibility for its value MAY be transferred by `move`. A binding established with `:=` is **Immovable**: responsibility for its value MUST NOT be transferred by `move`.

A binding’s initialization state MUST be one of:

* **Uninitialized**: no value has been assigned;
* **Valid**: fully initialized;
* **Moved**: value (or the whole value) has been moved out;
* **PartiallyMoved**: some subobject has been moved out and the binding is not fully initialized. 

Bindings are responsible for deterministic destruction (§3.6) iff they are responsible bindings for initialized values at scope exit; responsibility transfer is defined in §3.5.

**Semantic Domains**

| Domain       | Notation                                | Description                    |
| :----------- | :-------------------------------------- | :----------------------------- |
| Variables    | $x \in \mathsf{Var}$                    | Identifiers                    |
| Mutability   | $m \in {\mathsf{Let},\mathsf{Var}}$     | Immutable vs mutable binding   |
| Movability   | $\mu \in {\mathsf{Mov},\mathsf{Immov}}$ | Movable vs immovable binding   |
| Init maps    | $\iota \in \mathsf{Init}(T)$            | Subobject initialization state |
| Binding info | $B$                                     | Descriptor for a binding       |
| Contexts     | $\Gamma$                                | Typing + binding-state context |

**Formal Definition**

A binding descriptor is:
[
B \triangleq \langle T, m, \mu, \iota \rangle
]
and contexts map identifiers to descriptors: $\Gamma : \mathsf{Var} \rightharpoonup B$.

Define predicates:

* $\mathsf{movable}(\Gamma,x) \triangleq (\Gamma(x)=\langle T,m,\mathsf{Mov},\iota\rangle)$
* $\mathsf{mutable}(\Gamma,x) \triangleq (\Gamma(x)=\langle T,\mathsf{Var},\mu,\iota\rangle)$
* $\mathsf{init}(\Gamma,x) \triangleq \iota$ where $\Gamma(x)=\langle T,m,\mu,\iota\rangle$

**Auxiliary Definitions**

* $\mathsf{leastValid}$: a join operator over initialization states used at control-flow merges. It MUST satisfy:
  [
  \mathsf{Valid} > \mathsf{PartiallyMoved} > \mathsf{Moved} > \mathsf{Uninitialized}
  ]
  and merge MUST select the least-valid (minimum) incoming state (see below). 

**Well-Formedness**

1. A binding MAY shadow an outer binding only if name resolution selects the inner binding per Clause 8.
2. For any binding $x$, any use requiring the whole value MUST require $\mathsf{init}(\Gamma,x)=\mathsf{Valid}$; uses of subobjects MUST require the corresponding subobject initialization state to be initialized.
3. Reassignment to an immutable binding (`let`) is ill-formed (diagnostic E-MEM-3003). 

##### Grammar and Syntax

**Concrete Syntax (EBNF)** 

```ebnf
<binding_decl> ::= <let_decl> | <var_decl>
<let_decl>     ::= "let" <pattern> <binding_op> <expression>
<var_decl>     ::= "var" <pattern> <binding_op> <expression>
<binding_op>   ::= "=" | ":="
```

**Abstract Syntax**

Let $\mathsf{Pat}$ be the pattern AST defined in Clause 12.

[
\begin{aligned}
\mathsf{Decl} &::= \mathsf{BindDecl}(m,\mu,p,e) \
m &::= \mathsf{Let} \mid \mathsf{Var} \
\mu &::= \mathsf{Mov} \mid \mathsf{Immov}
\end{aligned}
]

**Lexical Constraints**

`let`, `var`, `=`, `:=` are reserved tokens.

**Desugaring Rules**

Pattern binding MAY be desugared into a sequence of primitive bindings after evaluating the initializer exactly once; such desugaring MUST preserve the destruction/lifetime rules of §3.6 and the temporary lifetime rule below.

**AST Representation**

The parser MUST map each concrete binding declaration to a $\mathsf{BindDecl}$ node preserving $(m,\mu,p,e)$.

##### Static Semantics

**Judgement Forms**

| Judgement                                     | Meaning                                                  |
| :-------------------------------------------- | :------------------------------------------------------- |
| $\Gamma \vdash d \dashv \Gamma'$              | Declaration $d$ is well-typed and yields updated context |
| $\Gamma \vdash e : T \dashv \Gamma'$          | Expression typing with state threading                   |
| $\Gamma \vdash p \Leftarrow T \dashv \Gamma'$ | Pattern $p$ checks against $T$ and introduces bindings   |

**Context Structure**

[
\Gamma ::= \cdot \mid \Gamma, x \mapsto \langle T,m,\mu,\iota\rangle
]

**Typing Rules**

(Primitive single-name pattern shown; general patterns must elaborate to these.)

[
\frac{\Gamma \vdash e : T \dashv \Gamma'}{\Gamma \vdash \mathsf{BindDecl}(\mathsf{Let},\mathsf{Mov},x,e)\dashv \Gamma', x\mapsto \langle T,\mathsf{Let},\mathsf{Mov},\mathsf{Valid}\rangle};(\textsc{T-Let-Movable})
]

[
\frac{\Gamma \vdash e : T \dashv \Gamma'}{\Gamma \vdash \mathsf{BindDecl}(\mathsf{Let},\mathsf{Immov},x,e)\dashv \Gamma', x\mapsto \langle T,\mathsf{Let},\mathsf{Immov},\mathsf{Valid}\rangle};(\textsc{T-Let-Immovable})
]

[
\frac{\Gamma \vdash e : T \dashv \Gamma'}{\Gamma \vdash \mathsf{BindDecl}(\mathsf{Var},\mathsf{Mov},x,e)\dashv \Gamma', x\mapsto \langle T,\mathsf{Var},\mathsf{Mov},\mathsf{Valid}\rangle};(\textsc{T-Var-Movable})
]

[
\frac{\Gamma \vdash e : T \dashv \Gamma'}{\Gamma \vdash \mathsf{BindDecl}(\mathsf{Var},\mathsf{Immov},x,e)\dashv \Gamma', x\mapsto \langle T,\mathsf{Var},\mathsf{Immov},\mathsf{Valid}\rangle};(\textsc{T-Var-Immovable})
]

**Control-Flow Merge**

At any control-flow join point, bindings MUST be merged by identifier. For any identifier $x$ present in all incoming contexts with identical $(T,m,\mu)$, the merged initialization state MUST be the least-valid incoming state (fieldwise for composite init maps), and the merged descriptor MUST preserve $(T,m,\mu)$. 

Define $\mathsf{mergeInit}(\iota_1,\iota_2)$ as:

* if both are $\mathsf{Valid}$, result $\mathsf{Valid}$;
* else if any is $\mathsf{Moved}$, result $\mathsf{Moved}$;
* else if any is $\mathsf{PartiallyMoved}$, result $\mathsf{PartiallyMoved}$ with a fieldwise least-valid merge;
* else if any is $\mathsf{Uninitialized}$, result $\mathsf{Uninitialized}$.

Then:
[
\frac{\Gamma_1(x)=\langle T,m,\mu,\iota_1\rangle \quad \Gamma_2(x)=\langle T,m,\mu,\iota_2\rangle}{(\Gamma_1 \sqcup \Gamma_2)(x)=\langle T,m,\mu,\mathsf{mergeInit}(\iota_1,\iota_2)\rangle};(\textsc{T-Merge})
]

**Temporary Lifetime**

Every temporary value not promoted by binding initialization MUST be destroyed at the end of the innermost statement. A temporary whose value is stored as the initializing value of a binding MUST have its destruction responsibility transferred to that binding’s scope, so that it is not destroyed at statement end. 

**Diagnostics**

| Code       | Condition                                 | Rule Violated       |
| :--------- | :---------------------------------------- | :------------------ |
| E-MEM-3003 | Reassignment of immutable binding (`let`) | (Well-Formedness 3) |

(Additional state/move diagnostics are consolidated in §3.5.) 

##### Dynamic Semantics

**Runtime Domains**

| Domain  | Notation | Description                 |
| :------ | :------- | :-------------------------- |
| Values  | $v$      | Runtime values              |
| Env     | $\rho$   | Maps variables to locations |
| Store   | $\sigma$ | Memory state                |
| Cleanup | $\kappa$ | LIFO cleanup actions        |

**Binding Introduction (Big-Step)**

Let $\sigma \vdash e \Downarrow v, \sigma'$ denote expression evaluation (Clause 12). Binding introduction evaluates the initializer exactly once, allocates stack storage for the new binding, stores the value, and registers a drop cleanup if needed.

[
\frac{
\sigma \vdash e \Downarrow v, \sigma_1
\quad
\mathsf{alloc}(\sigma_1,T,\mathsf{Stack}(S))=\langle \ell,\sigma_2\rangle
\quad
\sigma_3=\mathsf{write}(\sigma_2,\ell,\epsilon,v)
\quad
\kappa'=\mathsf{pushDropIfNeeded}(\kappa,\ell,T)
}{
\langle \mathsf{BindDecl}(m,\mu,x,e),\rho,\sigma,\kappa\rangle
\Downarrow
\langle \rho[x\mapsto \ell],\sigma_3,\kappa'\rangle
};(\textsc{B-Bind})
]
where $\epsilon$ is the empty subobject path.

---

### 3.5 Responsibility and Move Semantics

##### Definition

A **move** transfers responsibility for a value (or subvalue) from one place to another. After a move, the source place becomes uninitialized for the moved portion, and any subsequent use of that moved portion is ill-formed in Safe mode. Responsibility MUST be transferred so that destruction (§3.6) occurs exactly once for each responsible value instance.

A **partial move** is a move of a proper subobject (e.g., a record field) that leaves the containing object partially uninitialized. Partial moves MUST respect uniqueness constraints and MUST update initialization state precisely. 

**Semantic Domains**

| Domain      | Notation | Description                        |
| :---------- | :------- | :--------------------------------- |
| Places      | $p$      | Place expressions denoting storage |
| Subpaths    | $\alpha$ | Field/element paths                |
| Init maps   | $\iota$  | Initialization maps for composites |
| Permissions | $P$      | Required for partial moves (§4.5)  |

**Formal Definition**

Let $\mathsf{loc}(\rho,p)=\ell$ map a place $p$ to its location, and let $\mathsf{path}(p)=\alpha$ be its subobject path (empty for variables).

A move from $p$ is an operation that (i) reads the initialized subobject at $(\ell,\alpha)$, producing value $v$, and (ii) sets that subobject to uninitialized in the store and in the static init map.

**Auxiliary Definitions**

* $\mathsf{moveOut}(\sigma,\ell,\alpha)=\langle v,\sigma'\rangle$ reads the subobject value and makes it uninitialized.
* $\mathsf{updateInit}(\Gamma,p)$ updates the init map of the binding owning $p$.

**Well-Formedness**

1. A move from a binding declared with `:=` is ill-formed (E-MEM-3006).
2. A move from any place whose moved portion is not initialized is ill-formed (E-MEM-3001/E-MEM-3002/E-MEM-3003 as applicable).
3. A partial move from a record field is ill-formed unless the owning binding has unique permission (E-MEM-3004).
4. For any move parameter at a call site, the actual argument MUST be supplied as a `move` expression; otherwise the call is ill-formed (E-DEC-2411). 

##### Grammar and Syntax

**Concrete Syntax (EBNF)** 

```ebnf
<place_expr>   ::= <identifier>
                 | <place_expr> "." <identifier>
                 | <place_expr> "[" <expression> "]"
                 | "*" <place_expr>

<move_expr>    ::= "move" <place_expr>
```

**Abstract Syntax**

[
\mathsf{Expr} ::= \cdots \mid \mathsf{Move}(p)
\qquad
\mathsf{Place} ::= \mathsf{Var}(x)\mid \mathsf{Field}(p,f)\mid \mathsf{Index}(p,e)\mid \mathsf{Deref}(p)
]

**Desugaring Rules**

None.

**AST Representation**

The parser MUST construct $\mathsf{Move}(p)$ with a place AST corresponding to the parsed `<place_expr>`.

##### Static Semantics

**Judgement Forms**

| Judgement                                           | Meaning                       |
| :-------------------------------------------------- | :---------------------------- |
| $\Gamma \vdash p : T \dashv \Gamma'$                | Place typing (Clause 12/6.3)  |
| $\Gamma \vdash \mathsf{Move}(p) : T \dashv \Gamma'$ | Move typing with state update |
| $\mathsf{movable}(\Gamma,x)$                        | Binding $x$ is movable (`=`)  |
| $\mathsf{perm}(\Gamma,x)$                           | Binding’s permission (§4.5)   |

**Typing Rules**

**(T-Move)** (variable move; generalized place forms must reduce to an owning root binding and path)
[
\frac{
\Gamma \vdash x : T
\quad
\mathsf{init}(\Gamma,x)=\mathsf{Valid}
\quad
\mathsf{movable}(\Gamma,x)
}{
\Gamma \vdash \texttt{move }x : T \dashv \Gamma[x\mapsto \langle T,m,\mu,\mathsf{Moved}\rangle]
};(\textsc{T-Move})
] 

**(T-Move-Field)** (record field partial move)
[
\frac{
\Gamma \vdash x : R
\quad
R\ \text{contains field}\ f:T
\quad
\mathsf{perm}(\Gamma,x)=\mathsf{unique}
}{
\Gamma \vdash \texttt{move }x.f : T \dashv \Gamma[x\mapsto \langle R,m,\mu,\mathsf{PartiallyMoved}\rangle]
};(\textsc{T-Move-Field})
] 

Implementations MUST refine $\mathsf{PartiallyMoved}$ to record which subobjects are uninitialized so that subsequent subobject accesses can be validated and destruction can be performed without double-destruction.

**Parameter Responsibility**

If a formal parameter is declared as “move-requiring” (procedure/method syntax defined in Clause 9), then a call is well-formed only if the corresponding actual argument is syntactically a `move` expression. Otherwise, the implementation MUST emit **E-DEC-2411**. 

**Diagnostics** 

| Code       | Condition                                                  | Rule Violated                          |
| :--------- | :--------------------------------------------------------- | :------------------------------------- |
| E-MEM-3001 | Read or move of a binding in Moved or PartiallyMoved state | (Binding-state rules)                  |
| E-MEM-3002 | Read of an Uninitialized binding                           | (Binding-state rules)                  |
| E-MEM-3003 | Reassignment of immutable binding                          | (§3.4 WF-3)                            |
| E-MEM-3004 | Partial move from binding without unique permission        | (\textsc{T-Move-Field} side condition) |
| E-MEM-3006 | Move from immovable binding                                | (\textsc{T-Move} side condition)       |
| E-DEC-2411 | Call-site missing required `move` argument                 | (Parameter Responsibility)             |

##### Dynamic Semantics

**Move Evaluation (Big-Step)**

Let $\mathsf{evalPlace}(\rho,p)=\langle \ell,\alpha\rangle$ evaluate a place to a location and subpath. Then:

[
\frac{
\mathsf{evalPlace}(\rho,p)=\langle \ell,\alpha\rangle
\quad
\mathsf{moveOut}(\sigma,\ell,\alpha)=\langle v,\sigma'\rangle
}{
\langle \texttt{move }p,\rho,\sigma\rangle \Downarrow \langle v,\rho,\sigma'\rangle
};(\textsc{B-Move})
]

If $(\ell,\alpha)$ is uninitialized at runtime and the program reaches this step (possible only via Unsafe violations), the behavior is $\mathsf{UVB}$.

Responsibility transfer is realized dynamically by ensuring that destruction (§3.6) consults the runtime initialization state (drop flags/masks) so that exactly the still-initialized subobjects are destroyed.

---

### 3.6 Deterministic Destruction

##### Definition

A conforming implementation MUST perform **deterministic destruction** of responsible values. On normal scope exit and during panic unwinding, destruction MUST occur in strict LIFO order relative to the scope’s cleanup actions. A value’s destruction MUST execute its Drop hook (if any) followed by recursive destruction of its subobjects and release of its storage, as specified below.

**Semantic Domains**

| Domain          | Notation                | Description                                   |
| :-------------- | :---------------------- | :-------------------------------------------- |
| Cleanup actions | $a \in \mathsf{Act}$    | Drop actions (and other actions by reference) |
| Cleanup stack   | $\kappa$                | LIFO sequence of actions                      |
| Panic state     | $\mathsf{panic}(\cdot)$ | Exceptional control state                     |

**Formal Definition**

A cleanup stack frame is a finite sequence of actions pushed during evaluation of a scope. Let $\kappa = a_1 \cdots a_n$ with $a_n$ the top. Scope exit executes actions in order $a_n,\dots,a_1$.

**Auxiliary Definitions**

* $\mathsf{destroy}(\sigma,\ell,T)$ destroys the initialized portions of the object at $\ell$ of type $T$ and returns an updated store.

**Well-Formedness**

1. Each responsible value instance MUST be destroyed at most once.
2. An implementation MUST ensure that conditionally-moved values are destroyed iff they remain initialized along the taken execution path (i.e., destruction depends on runtime init flags, not merely the conservative merged static state). This requirement is observable via destructor side effects.

##### Grammar and Syntax

No dedicated surface syntax is introduced by this subsection.

##### Static Semantics

**Prohibition of Explicit Drop Calls**

Direct invocation of a Drop hook is forbidden:
[
\Gamma \vdash \texttt{Drop::drop}(e)\ \text{ill-formed}
]
and MUST be diagnosed as **E-MEM-3005**.

**Diagnostics**

| Code       | Condition                     | Rule Violated      |
| :--------- | :---------------------------- | :----------------- |
| E-MEM-3005 | Explicit call to `Drop::drop` | (Drop prohibition) |

##### Dynamic Semantics

**Destroy Algorithm (Normative)** 

For an object at location $\ell$ of type $T$ whose initialization state is $\iota$:

1. **Drop hook**: If $T$ implements `Drop` and the value at $\ell$ is fully initialized for the receiver contract, the implementation MUST invoke `Drop::drop` on the value at $\ell$.
2. **Recursive destruction**: The implementation MUST recursively destroy each initialized subobject of $T$ (fields/elements) in a deterministic order (Clause 5 defines subobject structure).
3. **Release storage**:

   * For $\mathsf{Heap}$ storage, the implementation MUST deallocate $\ell$.
   * For $\mathsf{Stack}(S)$ storage, the implementation MUST mark the value uninitialized; physical reclamation occurs by stack unwinding.
   * For $\mathsf{Region}(R)$ storage, reclamation occurs by region exit (§3.7) after running destruction for region allocations.

**Scope Exit**

Let $\mathsf{exitScope}(\kappa,\rho,\sigma)$ execute all cleanup actions for the scope in LIFO order, applying $\mathsf{destroy}$ for Drop actions whose runtime init flags indicate initialized content.

**Unwinding**

On panic unwinding, cleanup actions MUST run in the same order as normal scope exit. If a destructor panics during unwinding, the program MUST abort (double-panic abort). 

**Interaction with Other Cleanup Actions**

The cleanup stack is shared with `defer` blocks (§12.12) and key releases (§14.2). Implementations MUST preserve the single unified LIFO ordering required by those clauses.

---

### 3.7 Regions

##### Definition

A **region** is a lexical scope providing stack-like allocation for dynamically created objects. Entering a region creates a region frame; allocating into a region pushes allocations onto that frame; exiting the region MUST destroy all region-allocated objects and reclaim the region’s storage. Regions MAY be nested, and nested regions MUST behave as a LIFO stack.

**Semantic Domains**

| Domain            | Notation                   | Description                          |
| :---------------- | :------------------------- | :----------------------------------- |
| Region frames     | $R$                        | Runtime region identifiers           |
| Region stack      | $\mathcal{R}$              | Active region frames (LIFO)          |
| Region metadata   | $\mathsf{Opt}$             | Stack size/name options              |
| Region provenance | $\pi_{\mathsf{Region}}(R)$ | Provenance tag of region allocations |

**Formal Definition**

A region frame is:
[
R \triangleq \langle \mathsf{id}, \mathsf{allocs} \rangle
]
where $\mathsf{allocs}$ is an ordered sequence of locations allocated in the region.

**Auxiliary Definitions**

* $\mathsf{currentRegion}(\mathcal{R})$ returns the topmost active region frame (undefined if none).
* $\mathsf{allocRegion}(\sigma,T,R)=\langle \ell,\sigma'\rangle$ allocates $\ell$ with duration $\mathsf{Region}(R)$ and appends $\ell$ to $R.\mathsf{allocs}$.

**Well-Formedness**

1. Allocation into a region MUST target an active region frame.
2. Values allocated in a region MUST NOT escape the region in Safe mode; escape is enforced via provenance (§3.3).

##### Grammar and Syntax

**Concrete Syntax (EBNF)** 

```ebnf
<region_stmt>    ::= "region" <region_options>? <region_alias>? <block>

<region_options> ::= <region_option>+
<region_option>  ::= ".stack_size" "(" <expression> ")"
                   | ".name" "(" <expression> ")"

<region_alias>   ::= "as" <identifier>

<alloc_expr>     ::= "^" <expression>
                   | <identifier> "^" <expression>
```

**Abstract Syntax**

[
\begin{aligned}
\mathsf{Stmt} &::= \cdots \mid \mathsf{Region}(opts,alias,body) \
\mathsf{Expr} &::= \cdots \mid \mathsf{RegionAlloc}(r?,e)
\end{aligned}
]
where $r?$ is optional (implicit current region if absent).

**Desugaring Rules**

None.

##### Static Semantics

**Judgement Forms**

| Judgement                                                                        | Meaning                               |
| :------------------------------------------------------------------------------- | :------------------------------------ |
| $\Gamma \vdash \mathsf{Region}(\cdot) \dashv \Gamma'$                            | Region statement typing               |
| $\Gamma \vdash \mathsf{RegionAlloc}(r?,e) : T \triangleright \pi \dashv \Gamma'$ | Region allocation typing + provenance |

**Typing Rules**

**(T-Region-Block)**
[
\frac{
\Gamma, r:\mathsf{Region}@\mathsf{Active} \vdash \mathsf{block} : T \dashv \Gamma'
}{
\Gamma \vdash \texttt{region as }r\ \mathsf{block} : T \dashv \Gamma'
};(\textsc{T-Region})
]
If no alias is provided, the implementation MUST still treat the region as active for implicit allocations within its body.

**(T-Alloc-Explicit)** 
[
\frac{
\Gamma(r)=\mathsf{Region}@\mathsf{Active}
\quad
\Gamma \vdash e : T \dashv \Gamma'
}{
\Gamma \vdash r^e : T \triangleright \pi_{\mathsf{Region}}(R_r) \dashv \Gamma'
};(\textsc{T-Alloc-Explicit})
]

**(T-Alloc-Implicit)** (requires an active current region)
[
\frac{
\mathsf{currentRegion}(\Gamma)=R
\quad
\Gamma \vdash e : T \dashv \Gamma'
}{
\Gamma \vdash {^}e : T \triangleright \pi_{\mathsf{Region}}(R) \dashv \Gamma'
};(\textsc{T-Alloc-Implicit})
]

If the referenced/required region is not found or not active, the implementation MUST diagnose **E-REG-1206**. 

##### Dynamic Semantics

**Enter/Exit Region**

Entering a region pushes a fresh region frame onto $\mathcal{R}$. Exiting a region pops that frame and MUST:

1. destroy all objects allocated in the region in reverse allocation order (per §3.6), then
2. reclaim the region’s storage in bulk. 

Region-allocated objects that (illegally) escape via unsafe operations yield dangling references after exit; dereferencing such references constitutes $\mathsf{UVB}$.

---

### 3.8 Unsafe Memory Operations

##### Definition

An **unsafe block** is a lexical construct that permits operations not verified by the Safe static semantics. Operations that require unsafe (including raw pointer dereference, calls to unsafe procedures, transmute, and pointer arithmetic) MUST be rejected outside an unsafe block. Within unsafe blocks, the compiler is not required to enforce the liveness, aliasing exclusivity, and key-disjointness guarantees for operations that inherently bypass those guarantees; any violation of these invariants constitutes $\mathsf{UVB}$. 

**Semantic Domains**

| Domain      | Notation                                | Description      |
| :---------- | :-------------------------------------- | :--------------- |
| Safety mode | $u \in {\mathsf{Safe},\mathsf{Unsafe}}$ | Typing mode flag |

**Formal Definition**

Unsafe context is a lexical property: inside `unsafe { body }`, the typing judgements are evaluated under $u=\mathsf{Unsafe}$.

**Well-Formedness**

1. Any operation classified “unsafe-only” MUST appear in a context with $u=\mathsf{Unsafe}$.
2. The presence of an unsafe block does not change evaluation order; it only changes admissibility of unsafe-only operations.

##### Grammar and Syntax

**Concrete Syntax (EBNF)** 

```ebnf
<unsafe_block>    ::= "unsafe" "{" <block_body> "}"

<transmute_expr>  ::= "transmute" "<" <type> "," <type> ">"
                      "(" <expression> ")"
```

**Abstract Syntax**

[
\mathsf{Expr} ::= \cdots \mid \mathsf{UnsafeBlock}(body) \mid \mathsf{Transmute}(S,T,e)
]

**Desugaring Rules**

None.

##### Static Semantics

**Judgement Forms**

| Judgement                               | Meaning                             |
| :-------------------------------------- | :---------------------------------- |
| $\Gamma;u \vdash e : T \dashv \Gamma'$  | Typing under safety mode            |
| $\Gamma;u \vdash \mathsf{unsafeOK}(op)$ | Unsafe-only operation admissibility |

**Typing Rules**

**(T-Unsafe-Block)**
[
\frac{\Gamma;\mathsf{Unsafe} \vdash \mathsf{block} : T \dashv \Gamma'}{\Gamma;\mathsf{Safe} \vdash \texttt{unsafe}\ \mathsf{block} : T \dashv \Gamma'};(\textsc{T-UnsafeBlock})
]

**(T-Transmute)** 
[
\frac{
\Gamma;\mathsf{Unsafe} \vdash e : S \dashv \Gamma'
\quad
\mathsf{sizeof}(S)=\mathsf{sizeof}(T)
}{
\Gamma;\mathsf{Unsafe} \vdash \texttt{transmute}\langle S,T\rangle(e) : T \dashv \Gamma'
};(\textsc{T-Transmute})
]

Outside unsafe, any `transmute` MUST be rejected:

[
\frac{}{\Gamma;\mathsf{Safe} \vdash \mathsf{Transmute}(S,T,e)\ \text{ill-formed}};(\textsc{T-Transmute-Outside})
]

Unsafe-only operations outside unsafe MUST be diagnosed as **E-MEM-3030**. A transmute failing its constraints MUST be diagnosed as **E-MEM-3031**. 

**Diagnostics** 

| Code       | Condition                                    | Rule Violated               |
| :--------- | :------------------------------------------- | :-------------------------- |
| E-MEM-3030 | Unsafe operation used outside `unsafe` block | (unsafe-only admissibility) |
| E-MEM-3031 | `transmute` constraints not satisfied        | (\textsc{T-Transmute})      |

##### Dynamic Semantics

**Unsafe Block**

`unsafe { body }` has no intrinsic runtime effect beyond evaluating `body`.

**Transmute**

`transmute⟨S,T⟩(e)` evaluates $e$ to a value $v$ of type $S$, then reinterprets the underlying bytes of $v$ as a value of type $T$ without transformation. If the resulting bit-pattern is not a valid representation of $T$, the behavior is $\mathsf{UVB}$. 

## Clause 4: Type System Foundations

### 4.1 Type System Architecture

##### Definition

The **type system** of Cursive SHALL be a static, syntax-directed analysis that assigns types and auxiliary static properties (including permissions and safety modes) to all language phrases. For every well-formed program $P$, a conforming implementation in Safe mode MUST either:

1. derive a global well-typedness judgement for $P$, or
2. reject $P$ with at least one static-semantic diagnostic.

Types classify values, objects, and storage locations. Every expression, statement, and declaration that is not ill-formed MUST have a unique static type up to type equivalence (§0.1). The type system MUST enforce the Safety Invariant of §1.2 (B-Safe-No-UVB) when combined with the permission system (§4.5) and the memory model (Clause 3).

The type system is **predominantly nominal**: type identity for named data and modal types SHALL be determined by their defining declarations. Structural type forms (tuples, arrays, slices, unions, etc.) SHALL use structural equivalence as specified in §0.1. Subtyping SHALL be expressed as a partial order $;<:$ on $\mathsf{Type}$ (§4.3), and typing judgements MAY use subsumption via this order.

**Identity and Classification**

* A **type** is any element of $\mathsf{Type}$.
* A **value type** classifies values that may be stored in objects and manipulated at runtime.
* A **behavioral type** classifies behavioral properties, interfaces, traits, and capability-like types.
* An **extension type** refines or wraps another type (e.g., newtypes, opaque wrappers).

These categories SHALL partition the type universe:

$$
\mathsf{Type} = \mathsf{DataType} \uplus \mathsf{BehType} \uplus \mathsf{ExtType}
$$

where each component subset is further defined by Clauses 5–7.

A **typing derivation** SHALL be a finite proof tree built from the inference rules of this specification.

**Semantic Domains**

| Domain              | Notation                                | Description                                           |
| :------------------ | :-------------------------------------- | :---------------------------------------------------- |
| Types               | $T,U,S \in \mathsf{Type}$               | All types (Clauses 4–7)                               |
| Data types          | $\mathsf{DataType}$                     | Data-carrying value types (Clause 5)                  |
| Behavioral types    | $\mathsf{BehType}$                      | Behavioral/interface/effect types (Clause 6)          |
| Extension types     | $\mathsf{ExtType}$                      | Type refinements/wrappers (Clause 7)                  |
| Kinds               | $\kappa \in \mathsf{Kind}$              | Sorts of types (§4.2)                                 |
| Expressions         | $e \in \mathsf{Expr}$                   | Expression ASTs (Clause 12)                           |
| Declarations        | $d \in \mathsf{Decl}$                   | Declaration ASTs (Clauses 5, 8–11, 21)                |
| Programs            | $P \in \mathcal{P}$                     | Collections of compilation units (§1.1)               |
| Value contexts      | $\Gamma_v$                              | Mappings from term vars to binding descriptors (§3.4) |
| Type contexts       | $\Delta$                                | Mappings from type vars to kinds (§4.2)               |
| Permission contexts | $\Pi$                                   | Mappings from bindings/places to permissions (§4.5)   |
| Static contexts     | $\Gamma$                                | Aggregated context components (see below)             |
| Safety modes        | $u \in {\mathsf{Safe},\mathsf{Unsafe}}$ | Static safety mode (§3.8, §4.4)                       |

**Formal Definition**

A **static context** is a record:
$$
\Gamma \triangleq \langle \Delta,\Gamma_v,\Pi,\ldots\rangle
$$

where:

* $\Delta$ is the type context (kinding assumptions),
* $\Gamma_v$ is the value-binding context, and
* $\Pi$ is the permission context.

For convenience, this specification overloads $\Gamma$:

* In rules from Clause 3, $\Gamma$ SHALL denote the **value-binding component** $\Gamma_v$.
* In rules that mention $\mathsf{perm}(\Gamma,x)$, $\Gamma$ SHALL denote a full context whose permission component is $\Pi$.

Projection operators:

* $\Gamma.\Delta \triangleq \Delta$,
* $\Gamma.\Gamma_v \triangleq \Gamma_v$,
* $\Gamma.\Pi \triangleq \Pi$.

The value-binding component SHALL satisfy the binding model of §3.4:

$$
\Gamma_v : \mathsf{Var} \rightharpoonup B,\quad
B \triangleq \langle T,m,\mu,\iota \rangle.
$$

**Core Judgements**

The core static judgements of the type system are:

* Type formation (kinding): $\Delta \vdash T : \kappa$ (§4.2, Clauses 5–7),
* Subtyping: $\Delta \vdash S <: T$ (§4.3),
* Expression typing: $\Gamma;u \vdash e : T \dashv \Gamma'$ (Clause 12, extended with $u$ by §4.4),
* Declaration typing: $\Gamma \vdash d \dashv \Gamma'$ (Clauses 5, 8–11),
* Program safety: $P \vdash \textsf{safe}$ (§4.4),
* Permission soundness: $\Gamma \vdash \textsf{permOK}$ (§4.5).

**Auxiliary Definitions**

* Let $\textsf{TypeOf}$ denote the partial function induced by the typing relation:
  $$
  \textsf{TypeOf}(\Gamma;u,e) = T \iff \Gamma;u \vdash e : T \dashv \Gamma' \text{ for some }\Gamma'.
  $$
* Type equivalence $T \equiv U$ is as defined in §0.1; this SHALL be a congruence for all type constructors.

**Well-Formedness**

1. Every type constructor introduced in Clauses 5–7 MUST have a well-defined kinding rule (§4.2).
2. For every expression typing rule in this specification, the premises and conclusion MUST be syntax-directed with respect to the shape of $e$, except where explicitly noted (e.g. subsumption).
3. For any derivable judgement $\Gamma;u \vdash e : T \dashv \Gamma'$, the output context $\Gamma'$ MUST be well-formed (§4.2, §4.5).
4. For any two derivations of $\Gamma;u \vdash e : T_1$ and $\Gamma;u \vdash e : T_2$ with identical input context and safety mode, $T_1$ and $T_2$ MUST be equivalent: $T_1 \equiv T_2$.
5. The type system MUST be sound with respect to the memory model (Clause 3) in the sense that the Safety Invariant (§1.2, B-Safe-No-UVB) holds whenever $P \vdash \textsf{safe}$ (§4.4) and $P$ is well-formed.

##### Grammar and Syntax

**Concrete Syntax (EBNF)**

This subsection introduces no additional concrete syntax. All type expressions, declarations, and program forms are defined in Clauses 5–7, 8–12, 15–21.

**Abstract Syntax**

For the purposes of this clause, we treat the following abstract categories:

[
\begin{aligned}
\mathsf{Type} &::= \mathsf{DataType} \mid \mathsf{BehType} \mid \mathsf{ExtType} \
\mathsf{Program} &::= \mathsf{Unit}^* \
\mathsf{Unit} &::= \mathsf{Decl}^*
\end{aligned}
]

Each concrete clause refines these categories with additional constructors.

**Lexical Constraints**

This clause introduces no new lexical tokens.

**Desugaring Rules**

None. Desugarings for type and declaration syntax are specified in the clauses that introduce them.

**AST Representation**

The AST node kinds for types, declarations, and programs MUST be sufficient to represent all forms defined in Clauses 5–7 and 8–12. This clause assumes those ASTs exist and does not constrain their concrete encoding.

##### Static Semantics

**Judgement Forms**

| Judgement                              | Meaning                                               |
| :------------------------------------- | :---------------------------------------------------- |
| $\Delta \vdash T : \kappa$             | Type $T$ has kind $\kappa$ (§4.2)                     |
| $\Delta \vdash S <: T$                 | $S$ is a subtype of $T$ (§4.3)                        |
| $\Gamma;u \vdash e : T \dashv \Gamma'$ | Expression typing under safety mode (§4.4, Clause 12) |
| $\Gamma \vdash d \dashv \Gamma'$       | Declaration is well-typed                             |
| $\Gamma \vdash \textsf{permOK}$        | Permission invariants hold (§4.5)                     |
| $P \vdash \textsf{safe}$               | Program $P$ is statically safe (§4.4)                 |

This clause does not define term-level typing rules; those are defined in Clauses 5, 8–12. It constrains their shape and required meta-properties.

**Context Structure**

A context MUST satisfy:

* $\Gamma.\Delta$ is a finite map from type variables to kinds;
* $\Gamma.\Gamma_v$ is a finite map from variables to binding descriptors as per §3.4;
* $\Gamma.\Pi$ is a finite map from bindings and other permission carriers to permissions (§4.5).

##### Dynamic Semantics

This subsection introduces no runtime constructs. The type system has no direct dynamic semantics; its obligations are expressed via soundness requirements relative to the dynamic semantics of Clauses 3, 12, 14–17, and 21.

##### Memory and Layout

Not applicable. Types and typing judgements have no runtime representation.

---

### 4.2 Kinds and Type Formation

##### Definition

Kinds classify types and type constructors. Every type and type constructor in Cursive MUST have a unique kind derivable from the rules of this clause and the clauses that introduce individual type forms.

Type formation judgements SHALL ensure that all types used in programs are **well-formed**, including their parameters and constraints. Ill-formed types MUST be diagnosed as static errors.

**Semantic Domains**

| Domain        | Notation                   | Description                               |
| :------------ | :------------------------- | :---------------------------------------- |
| Kinds         | $\kappa \in \mathsf{Kind}$ | Sorts of types                            |
| Type contexts | $\Delta$                   | Maps type variables to kinds              |
| Type vars     | $\alpha \in \mathsf{TVar}$ | Universally/existentially quantified vars |

**Formal Definition**

Kinds:

[
\mathsf{Kind} ::= \star \mid \kappa \Rightarrow \kappa
]

where:

* $\star$ classifies **proper value types** (inhabited by runtime values),
* function kinds classify type constructors (type operators), e.g. $\star \Rightarrow \star$.

Additional distinguished kinds (e.g., for keys, capabilities, effects) MAY be introduced in Clauses 6, 13, 14, and 16. When such kinds are introduced, they MUST be subkinds or distinct base kinds, and their formation rules MUST extend this clause.

Type context:

[
\Delta : \mathsf{TVar} \rightharpoonup \mathsf{Kind}.
]

**Core Kinding Rules**

These rules provide a skeleton; each type-introducing clause extends them with more constructors.

*Type Variable*

[
\frac{\Delta(\alpha) = \kappa}
{\Delta \vdash \alpha : \kappa}
\quad(\textsc{K-TVar})
]

*Type Constructor Application*

If $F$ is a type constructor declared with kind $\kappa_1 \Rightarrow \cdots \Rightarrow \kappa_n \Rightarrow \kappa$, and $T_i$ arguments:

[
\frac{
\Delta \vdash F : \kappa_1 \Rightarrow \cdots \Rightarrow \kappa_n \Rightarrow \kappa
\quad
\forall i.\ \Delta \vdash T_i : \kappa_i
}{
\Delta \vdash F\langle T_1,\ldots,T_n\rangle : \kappa
}
\quad(\textsc{K-TyCon-App})
]

Each type constructor $F$ introduced in Clauses 5–7 MUST declare its kind and provide corresponding formation rules.

**Type Well-Formedness**

A type $T$ is **well-formed** under $\Delta$ iff there exists some kind $\kappa$ such that $\Delta \vdash T : \kappa$. The judgement $\Gamma \vdash T\ \text{wf}$ is defined as:

[
\Gamma \vdash T\ \text{wf} \iff \Gamma.\Delta \vdash T : \star.
]

**Auxiliary Definitions**

* $\textsf{FreeTVars}(T)$: the set of free type variables in $T$.
* A type $T$ is **closed** in $\Gamma$ if $\textsf{FreeTVars}(T) \subseteq \text{dom}(\Gamma.\Delta)$.

**Well-Formedness**

1. Any type appearing in a term typing judgement $\Gamma;u \vdash e : T \dashv \Gamma'$ MUST satisfy $\Gamma \vdash T\ \text{wf}$.
2. Any polymorphic declaration MUST bind all free type variables appearing in its signature.
3. For any type constructor $F$ with declared kind $\kappa$, all uses of $F$ MUST obey $\Delta \vdash F : \kappa$ and the application kinding rules.

##### Grammar and Syntax

This subsection introduces no additional concrete syntax. Type-level concrete syntax (including generics, type parameters, and type constructors) is defined in Clauses 5–7 and 10.

**Abstract Syntax**

We assume a family of AST node forms for types:

[
\mathsf{Type} ::= \mathsf{TyVar}(\alpha) \mid \mathsf{TyCon}(F,\overline{T}) \mid \cdots
]

where additional constructors (e.g., function types, tuples, records, enums, modals) are defined in Clauses 5–7.

**Desugaring Rules**

Type-level desugarings (e.g., syntactic sugar for function types or aliases) MUST preserve the kinding derivations specified by this clause.

##### Static Semantics

**Judgement Forms**

| Judgement                    | Meaning                              |
| :--------------------------- | :----------------------------------- |
| $\Delta \vdash T : \kappa$   | Type $T$ has kind $\kappa$           |
| $\Gamma \vdash T\ \text{wf}$ | Type $T$ is a well-formed value type |

Each type-introducing clause MUST extend the kinding rules by adding constructors and their formation rules.

##### Dynamic Semantics

Kinds and type formation have no runtime effect. They are compile-time-only constructs.

##### Memory and Layout

Not applicable. Kinds and type formation do not determine concrete layout; Clause 5 provides layout rules for value types.

---

### 4.3 Subtyping

##### Definition

Subtyping provides a partial order on types that allows a value of one type to be safely used where another type is expected. The subtype relation SHALL be written $\Delta \vdash S <: T$.

A conforming implementation MUST support subsumption in typing derivations:

* If $\Gamma;u \vdash e : S \dashv \Gamma'$ and $\Gamma.\Delta \vdash S <: T$, then $\Gamma;u \vdash e : T \dashv \Gamma'$ (subject to the subsumption rule below).

Subtyping MUST be reflexive and transitive modulo type equivalence, and MUST respect kinding.

**Semantic Domains**

| Domain       | Notation                  | Description                |
| :----------- | :------------------------ | :------------------------- |
| Types        | $S,T,U \in \mathsf{Type}$ | As in §4.1                 |
| Type context | $\Delta$                  | Kinding assumptions (§4.2) |

**Formal Definition**

The subtype relation is a family of relations $(<:_\Delta)$ parameterized by $\Delta$:

[
\Delta \vdash S <: T \subseteq \mathsf{Type} \times \mathsf{Type}
]

subject to:

1. **Reflexivity**:
   [
   \frac{\Delta \vdash T : \kappa}{\Delta \vdash T <: T}
   \quad(\textsc{S-Refl})
   ]
2. **Transitivity**:
   [
   \frac{\Delta \vdash S <: T \quad \Delta \vdash T <: U}{\Delta \vdash S <: U}
   \quad(\textsc{S-Trans})
   ]
3. **Equivalence Compatibility**:
   If $S \equiv S'$ and $T \equiv T'$ (per §0.1) and $\Delta \vdash S <: T$, then $\Delta \vdash S' <: T'$.

Specific subtyping rules for function types, trait/behavioral types, reference types, etc., SHALL be introduced in the clauses that define those type forms. Such rules MUST satisfy:

* **Kind Preservation**:
  [
  \Delta \vdash S <: T \Rightarrow \exists \kappa.\ \Delta \vdash S : \kappa \land \Delta \vdash T : \kappa.
  ]
* **Variance Restrictions**:
  Parameterized types MUST declare a variance (covariant, contravariant, invariant, or bivariant) for each parameter, and their subtyping rules MUST respect those variance annotations.

**Subsumption Rule**

Typing with subtyping:

[
\frac{
\Gamma;u \vdash e : S \dashv \Gamma'
\quad
\Gamma.\Delta \vdash S <: T
}{
\Gamma;u \vdash e : T \dashv \Gamma'
}
\quad(\textsc{T-Sub})
]

Implementations MAY inline subtyping into syntax-directed rules instead of using an explicit subsumption rule, but the induced typing relation MUST be equivalent.

**Well-Formedness**

1. If $\Delta \vdash S <: T$, then both $S$ and $T$ MUST be well-formed under $\Delta$.
2. Subtyping MUST NOT relate types of incompatible categories (e.g., data types with incompatible representation requirements) unless explicitly specified by the defining clause.
3. For nominal types, subtyping MUST be determined only by the relationships declared in their definitions (e.g., extends/implements clauses), not by structural similarity.
4. For structural types that support subtyping (if any), subtyping MUST be defined structurally and MUST respect the equivalence relation of §0.1.

##### Grammar and Syntax

No concrete syntax is introduced for subtyping. Subtype relationships are derived from type declarations (§5–7, §10–11).

##### Static Semantics

**Judgement Forms**

| Judgement               | Meaning                          |
| :---------------------- | :------------------------------- |
| $\Delta \vdash S <: T$  | $S$ is a subtype of $T$          |
| $\Gamma;u \vdash e : T$ | Expression typing with subtyping |

Specific subtyping rules are clause-local (e.g., function type variance in Clause 9).

**Diagnostics**

Implementations SHOULD diagnose unsatisfied subtyping constraints with codes in the `E-TYP-4xxx` range. This clause does not fix specific codes; they are assigned in the clauses that introduce type forms.

##### Dynamic Semantics

Subtyping is purely static and has no direct runtime effect. Where subtyping induces representation-changing coercions, the responsible clause MUST define the corresponding dynamic semantics.

##### Memory and Layout

Subtyping MUST NOT change the runtime representation of values unless the clause defining the coercion explicitly specifies the conversion (e.g., trait object upcasting, existential packing). Where subtyping is representation-preserving, `sizeof` and `alignof` SHALL be equal for related types.

---

### 4.4 Program Safety and Safety Modes

##### Definition

Static **safety** classifies programs that are guaranteed not to exhibit Unverifiable Behavior (UVB) as defined in §1.2 when executed by a conforming implementation in Safe mode.

A program $P$ is **statically safe** iff:

1. $P$ is well-formed (§1.1), and
2. $P$ admits a derivation of the judgement $P \vdash \textsf{safe}$ defined below.

For any such program, the Safety Invariant (B-Safe-No-UVB in §1.2) MUST hold:

[
\frac{
I,C \vdash P\ \textsf{wf} \quad P \vdash \textsf{safe}
}{
I,C \vdash P \not\Downarrow_{\textsf{UVB}}
}
]

Safety modes classify typing environments for terms:

* In mode $u = \mathsf{Safe}$, operations requiring external invariants (UVB) MUST be statically rejected.
* In mode $u = \mathsf{Unsafe}$, such operations MAY be admitted, and the program is not classified as safe solely by typing.

**Semantic Domains**

| Domain      | Notation                                | Description                     |
| :---------- | :-------------------------------------- | :------------------------------ |
| Safety mode | $u \in {\mathsf{Safe},\mathsf{Unsafe}}$ | Static typing mode              |
| Programs    | $P \in \mathcal{P}$                     | As in §1.1                      |
| Units       | $U$                                     | Compilation units making up $P$ |

**Formal Definition**

Typing judgements extended with safety mode:

[
\Gamma;u \vdash e : T \dashv \Gamma'
]

where $u$ is determined lexically by `unsafe` blocks (§3.8) and by clause-specific rules for unsafe-only operations.

We define a **safe-program judgement**:

[
P \vdash \textsf{safe}
]

intuitively meaning “all code of $P$ is type-checked in Safe mode and uses no unsafe-only operations.”

**Auxiliary Definitions**

* $\textsf{UnsafeOnly}(e)$: a predicate that holds if expression $e$ contains an operation that this specification classifies as requiring Unsafe mode (e.g., raw pointer deref, `transmute`, unsafe FFI calls). Each clause that introduces such an operation MUST define its inclusion in $\textsf{UnsafeOnly}$.
* $\textsf{OccursUnsafe}(P)$: the set of AST nodes in $P$ that are either `UnsafeBlock` nodes or contain operations in $\textsf{UnsafeOnly}$.

**Static Safety Judgement**

Program-level safety is defined by:

[
\frac{
\forall U \in P.\ \Gamma_0;\mathsf{Safe} \vdash U\ \textsf{unit-wf} \dashv \Gamma_U
\quad
\textsf{OccursUnsafe}(P) = \emptyset
}{
P \vdash \textsf{safe}
}
\quad(\textsc{P-Safe})
]

where:

* $\Gamma_0$ is the initial context (predeclared universe-protected types, etc., §1.3),
* $\Gamma_0;\mathsf{Safe} \vdash U\ \textsf{unit-wf} \dashv \Gamma_U$ abbreviates unit-level typing judgements (composition of declaration typing judgements),
* unit typing MUST use only rules admissible in Safe mode (§3.8, §4.5, and other clauses).

The requirement $\textsf{OccursUnsafe}(P)=\emptyset$ forbids syntactic `unsafe` blocks and any construct classified as unsafe-only; thus any UVB behavior is syntactically excluded.

**Well-Formedness**

1. A program classified as safe ($P \vdash \textsf{safe}$) MUST NOT contain any `unsafe` block or other construct that requires Unsafe mode.
2. Any operation classified as unsafe-only (§3.8 and clause-local specifications) MUST be rejected in Safe mode:
   [
   \Gamma;\mathsf{Safe} \vdash e\ \text{ill-formed} \quad\text{if}\ \textsf{UnsafeOnly}(e).
   ]
3. Typing rules that admit unsafe-only operations MUST require $u = \mathsf{Unsafe}$ as a premise.

##### Grammar and Syntax

This subsection introduces no new syntax. `unsafe` blocks and `transmute` are defined in §3.8.

##### Static Semantics

**Judgement Forms**

| Judgement                              | Meaning                             |
| :------------------------------------- | :---------------------------------- |
| $\Gamma;u \vdash e : T \dashv \Gamma'$ | Expression typing under safety mode |
| $P \vdash \textsf{safe}$               | Program $P$ is statically safe      |
| $\textsf{UnsafeOnly}(e)$               | $e$ requires Unsafe mode            |

Clause 3.8 defines the core rules for $u$ and for unsafe blocks. This clause constrains how those rules interact with program-level safety classification.

**Diagnostics**

Violations of safety-mode constraints SHOULD be reported using diagnostics in the `E-SAF-4xxx` range; §3.8 already defines:

* **E-MEM-3030** — unsafe operation outside `unsafe` block,
* **E-MEM-3031** — invalid `transmute`.

Additional safety-violating uses of unsafe-only operations MUST be mapped to diagnostics (referenced in the clauses that introduce them).

##### Dynamic Semantics

Safety modes and the $P \vdash \textsf{safe}$ predicate have no direct runtime representation. Their dynamic significance is given exclusively by the Safety Invariant (§1.2) and the fact that UVB is excluded for safe programs.

##### Memory and Layout

Not applicable.

---

### 4.5 Permission System

##### Definition

The **permission system** is a static discipline that tracks which accesses (reads, writes, moves, and borrows) are permitted to each storage location at each program point. Permissions MUST be used, together with the memory model (Clause 3) and the key system (Clause 14), to enforce the aliasing part of memory safety (§3.1).

Permissions are associated with bindings and other access paths and SHALL be tracked in the permission context $\Pi$. The function $\mathsf{perm}(\Gamma,x)$ MUST yield the current permission of binding $x$.

Permissions SHALL be sufficient to enforce **unique exclusivity**:

* at most one mutable, write-capable access (unique permission) to a given location at a time, or
* multiple read-only shared permissions, but no unique permission concurrently.

The permission system is purely static; it MUST NOT require or imply any runtime metadata.

**Semantic Domains**

| Domain              | Notation                                             | Description                 |
| :------------------ | :--------------------------------------------------- | :-------------------------- |
| Permissions         | $P \in \mathsf{Perm}$                                | Permission lattice elements |
| Permission context  | $\Pi : \mathsf{Var} \to \mathsf{Perm}$               | Permissions for bindings    |
| Access modes        | $a \in {\mathsf{Read},\mathsf{Write},\mathsf{Move}}$ | Access kinds                |
| Locations           | $\ell \in \mathsf{Loc}$                              | As in §3.1                  |
| Binding descriptors | $B = \langle T,m,\mu,\iota\rangle$                   | As in §3.4                  |

**Formal Definition**

Permission lattice:

[
\mathsf{Perm} ::= \mathsf{None} \mid \mathsf{Shared} \mid \mathsf{Unique}
]

with partial order $\sqsubseteq$:

* $\mathsf{None} \sqsubseteq \mathsf{Shared}$,
* $\mathsf{None} \sqsubseteq \mathsf{Unique}$,
* $\mathsf{Shared}$ and $\mathsf{Unique}$ are incomparable.

$\mathsf{None}$ denotes “no permission”; $\mathsf{Shared}$ denotes read-only, potentially aliased permission; $\mathsf{Unique}$ denotes exclusive, read-write permission.

Permission context:

[
\Pi : \mathsf{Var} \rightharpoonup \mathsf{Perm}.
]

The extended static context is:

[
\Gamma = \langle \Delta,\Gamma_v,\Pi,\ldots \rangle.
]

The derived function $\mathsf{perm}$ is:

[
\mathsf{perm}(\Gamma,x) \triangleq \Gamma.\Pi(x).
]

**Access Admissibility**

Define a predicate $\mathsf{allow}(P,a)$:

* $\mathsf{allow}(\mathsf{Unique},\mathsf{Read}) = \mathsf{true}$,
* $\mathsf{allow}(\mathsf{Unique},\mathsf{Write}) = \mathsf{true}$,
* $\mathsf{allow}(\mathsf{Unique},\mathsf{Move}) = \mathsf{true}$,
* $\mathsf{allow}(\mathsf{Shared},\mathsf{Read}) = \mathsf{true}$,
* $\mathsf{allow}(\mathsf{Shared},\mathsf{Write}) = \mathsf{false}$,
* $\mathsf{allow}(\mathsf{Shared},\mathsf{Move}) = \mathsf{false}$,
* $\mathsf{allow}(\mathsf{None},a) = \mathsf{false}$ for all $a$.

A read, write, or move of a binding $x$ is **permission-admissible** iff $\mathsf{allow}(\mathsf{perm}(\Gamma,x),a)$ holds, where $a$ is the access mode of the operation.

**Aliasing Invariant**

Let $\textsf{LocOf}(x)$ map a binding $x$ to its storage location (abstracting over the binding model and regions). For each location $\ell$, define the multiset of active permissions:

[
\mathsf{PermSet}_\Gamma(\ell) \triangleq {, \mathsf{perm}(\Gamma,x)\mid \textsf{LocOf}(x)=\ell \land \mathsf{perm}(\Gamma,x)\neq\mathsf{None} ,}.
]

The **alias-safety invariant** for permissions SHALL be:

For all $\ell$:

1. either $\mathsf{PermSet}_\Gamma(\ell)=\emptyset$; or
2. $\mathsf{PermSet}_\Gamma(\ell)\subseteq{\mathsf{Shared}}$ (any number of shared readers); or
3. $\mathsf{PermSet}_\Gamma(\ell)={\mathsf{Unique}}$ (exactly one unique owner).

No other configuration of permissions is permitted in a well-formed context.

**Interaction with Binding Introduction and Moves**

When a binding is introduced according to §3.4:

[
\Gamma_v' = \Gamma_v[x \mapsto \langle T,m,\mu,\mathsf{Valid}\rangle]
]

the permission context MUST be extended with:

[
\Pi' = \Pi[x \mapsto \mathsf{Unique}]
]

unless otherwise specified by the clause introducing the binding (e.g., borrowed bindings may be introduced with $\mathsf{Shared}$ or $\mathsf{None}$). The default root owner of a value thus starts with unique permission.

For moves (§3.5):

* Full move (T-Move):

  After a successful move of the whole binding $x$,

  [
  \Gamma' = \Gamma[x \mapsto \langle T,m,\mu,\mathsf{Moved}\rangle],
  \quad
  \Pi'  = \Pi[x \mapsto \mathsf{None}].
  ]

* Partial move from record field (T-Move-Field):

  After a successful partial move of $x.f$:

  [
  \Gamma' = \Gamma[x \mapsto \langle R,m,\mu,\mathsf{PartiallyMoved}\rangle],
  \quad
  \Pi' = \Pi[x \mapsto \mathsf{Unique}]
  ]

  i.e., permission remains unique but the initialization map records which subfields are uninitialized (§3.5).

The side condition in (\textsc{T-Move-Field}):

[
\mathsf{perm}(\Gamma,x)=\mathsf{unique}
]

MUST be interpreted as $\mathsf{perm}(\Gamma,x)=\mathsf{Unique}$.

**Auxiliary Definitions**

* $\Gamma[x \mapsto P]_{\textsf{perm}}$ denotes the context obtained from $\Gamma$ by updating $\Gamma.\Pi(x)$ to $P$ while leaving other components unchanged.
* $\Gamma[x \mapsto B]_{\textsf{val}}$ similarly updates $\Gamma.\Gamma_v$.

**Well-Formedness**

Define a judgement $\Gamma \vdash \textsf{permOK}$ to mean that permission invariants hold in $\Gamma$:

[
\frac{
\forall \ell.\ \mathsf{PermSet}_\Gamma(\ell) \in {\emptyset,{\mathsf{Unique}},\text{submultiset of }{\mathsf{Shared}}}
}{
\Gamma \vdash \textsf{permOK}
}
\quad(\textsc{P-WF})
]

The following MUST hold:

1. Any typing rule that changes $\Gamma.\Pi$ MUST preserve $\Gamma \vdash \textsf{permOK}$.
2. Every binding introduction that creates a new owning location MUST introduce exactly one binding with permission $\mathsf{Unique}$ for that location.
3. Any operation that requires write access to a location reachable from $x$ MUST require $\mathsf{allow}(\mathsf{perm}(\Gamma,x),\mathsf{Write})$.
4. Any operation that requires a partial move from a subobject MUST require $\mathsf{perm}(\Gamma,x)=\mathsf{Unique}$ (as in \textsc{T-Move-Field} and similar rules).
5. Any operation that consumes a permission (e.g., converting a unique permission into multiple shared permissions via borrowing) MUST be specified in the clause introducing that operation (e.g., Clause 6 for borrows, Clause 13 for capabilities) and MUST preserve $\Gamma \vdash \textsf{permOK}$.

##### Grammar and Syntax

This subsection defines no new concrete syntax. Permission changes are induced by operations defined in other clauses (binding declarations, moves, borrows, capability/key operations).

##### Static Semantics

**Judgement Forms**

| Judgement                       | Meaning                                       |
| :------------------------------ | :-------------------------------------------- |
| $\Gamma \vdash \textsf{permOK}$ | Permission invariants hold in $\Gamma$        |
| $\mathsf{perm}(\Gamma,x)$       | Permission of binding $x$ in context $\Gamma$ |
| $\mathsf{allow}(P,a)$           | Permission $P$ allows access mode $a$         |

**Permission-Admissible Access**

For any expression typing rule that reads or writes $x$ directly (or via a place rooted at $x$), there MUST be a side condition:

*Read:*

[
\frac{
\Gamma \vdash x : T
\quad
\mathsf{allow}(\mathsf{perm}(\Gamma,x),\mathsf{Read})
}{
\Gamma \vdash \textsf{read}(x) : T \dashv \Gamma
}
\quad(\textsc{T-Read-Perm})
]

*Write:*

[
\frac{
\Gamma \vdash x : T
\quad
\mathsf{allow}(\mathsf{perm}(\Gamma,x),\mathsf{Write})
}{
\Gamma \vdash \textsf{write}(x,e) : \mathsf{Unit} \dashv \Gamma'
}
\quad(\textsc{T-Write-Perm})
]

(Concrete forms of read/write are given in Clause 12; this rule schema constrains them.)

**Diagnostics**

Permission violations SHALL be diagnosed as static errors. The following diagnostic codes are reserved for permission system violations:

| Code        | Condition                                                                                | Rule Violated                       |
| :---------- | :--------------------------------------------------------------------------------------- | :---------------------------------- |
| E-PERM-4001 | Write or move attempted with $\mathsf{perm}(\Gamma,x)\in{\mathsf{Shared},\mathsf{None}}$ | (\textsc{T-Write-Perm}, move rules) |
| E-PERM-4002 | Read or write when $\Gamma \nvdash \textsf{permOK}$ for the context (detected locally)   | (P-WF)                              |
| E-PERM-4003 | Static analysis detects multiple $\mathsf{Unique}$ permissions for the same location     | (P-WF)                              |

Clause-local rules MAY refine or add diagnostics that specialize these conditions (e.g., for borrowing or capability usage).

##### Dynamic Semantics

Permissions are compile-time-only; they MUST NOT require runtime storage. The dynamic aliasing constraints in §3.1 (alias-safety) SHALL be enforced by ensuring that all Safe-mode programs satisfy $\Gamma \vdash \textsf{permOK}$ at all control-flow points and that all memory-access rules respect $\mathsf{allow}$.

Any execution that violates the alias-safety invariant despite static permission checking (possible only via Unsafe operations or FFI) constitutes $\mathsf{UVB}$ per §1.2.

##### Memory and Layout

Not applicable. Permissions do not affect object layout or representation.

---

This Clause 4 provides the foundational static framework (kinds, type formation, subtyping, program safety classification, and permission system) on which the detailed typing rules of Clauses 5–7, 8–12, 13, and 14 are defined.
