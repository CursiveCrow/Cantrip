Correction required: **Formalism & Diagnostic Rigor**.

The previous output failed the following QA criteria:

1.  **Diagnostic Completeness:** While "Ill-formed" was defined, the specific diagnostic code taxonomy (required by the Source Appendix B mapping) was not referenced or instantiated, leaving the "MUST reject" requirement abstract.
2.  **Archetype Adherence:** The **System Definition** archetype requires specific headers (`Invariants`, `Semantics`, `Hazards`) which were applied loosely or omitted in Section 3.3.
3.  **Logical Rigor:** The partition of behavior classifications was described in prose rather than formal set theory invariants.

Regenerating **Clause 3** with strict ISO/IEC formalism and Content Archetype adherence.

## Clause 3: Terms and Definitions

### 3.1 Glossary [def.glossary]

<u>**Definition**</u>

For the purposes of this document, the following terms and definitions apply.

**3.1.1 General Terms**

**Implementation**
An abstract machine or software system (compiler, interpreter, JIT, analysis tool) that processes Cursive source text. An implementation is characterized by its adherence to the normative requirements of this specification (see §4.1).

**Program**
A complete, standalone unit of execution comprising one or more compilation units, linked against necessary libraries, intended to run on a host environment.

  * **Formal Definition:** A program $P$ is a tuple $(M, E, \Gamma_{root})$ where $M$ is a set of modules, $E$ is a unique entry point procedure, and $\Gamma_{root}$ is the capability context.

**Translation**
The transformation process of converting a well-formed source text into an executable artifact or intermediate representation.

**Execution**
The dynamic process wherein the semantics of a translated program are realized by the abstract machine, producing observable effects ($Observable(P)$).

**3.1.2 Conformance Terms**

**Conforming Implementation**
An implementation that accepts any strictly conforming program and behaves according to the semantics defined in this specification, subject to defined implementation limits.

**Conforming Program**
A program that is well-formed, relies only on defined or implementation-defined behavior, and does not produce unverifiable behavior during execution unless explicitly attested via the mechanism defined in §4.5.

**Well-formed Program**
A source text that satisfies all lexical, syntactic, and static-semantic rules defined in this specification.

  * **Invariant:** $P \in \text{WellFormed} \iff (\text{Parse}(P) \neq \bot) \land (\Gamma \vdash P : \text{Ok})$.

**Ill-formed Program**
A source text that violates one or more normative rules (lexical, syntactic, or static-semantic).

  * **Constraint:** Implementations **MUST** reject ill-formed programs and emit at least one error diagnostic, unless the violation is classified as IFNDR.

**Ill-Formed, No Diagnostic Required (IFNDR)**
A class of ill-formedness where the violation is theoretically decidable but computationally infeasible to detect in general (e.g., violations dependent on complex runtime state reachable only through undecidable static paths).

**3.1.3 Programming Terms**

**Binding**
The association between an identifier $x$ and a static entity (value, type, or module).

  * **Immutable Binding (`let`):** A binding that **MUST NOT** be the target of an assignment statement after initialization.
  * **Mutable Binding (`var`):** A binding that **MAY** be the target of an assignment statement.

**Object**
A discrete region of storage.

  * **Formal:** An object $O$ is a tuple $(Addr, Size, Type, Lifetime, Value)$.

**Permission**
A type qualifier $\pi \in \{ \text{const}, \text{unique}, \text{partitioned} \}$ determining the aliasing and mutation capabilities of a reference to an object (see §7.2).

**Capability**
A first-class object $\kappa$ representing the authority to perform an observable external effect (e.g., I/O, memory allocation).

**Context**
The root capability object $\Gamma_{root}$ (of type `Context`) injected into the program entry point, holding all ambient authority granted by the host environment.

**Contract**
A set of logical predicates (Preconditions $P$, Postconditions $Q$, Invariants $I$) bound to a procedure or type.

  * **Notation:** $\{P\} \ S \ \{Q\}$.

**Static Invalidation**
The compile-time transition of a binding's state in the typing context $\Gamma$ from *Valid* to *Moved*, rendering it unusable for subsequent operations.

**Verification Fact**
A virtual, zero-size compile-time artifact $F(P, L)$ representing a proof that predicate $P$ holds at location $L$.

**3.1.4 Symbols and Abbreviations**

  * **ABI:** Application Binary Interface
  * **AST:** Abstract Syntax Tree
  * **EBNF:** Extended Backus-Naur Form
  * **FFI:** Foreign Function Interface
  * **IDB:** Implementation-Defined Behavior
  * **IFNDR:** Ill-Formed, No Diagnostic Required
  * **LPS:** Lexical Permission System
  * **RAII:** Resource Acquisition Is Initialization
  * **USB:** Unspecified Behavior
  * **UVB:** Unverifiable Behavior

<u>**Invariants**</u>

  * **Consistency:** The definitions provided here **MUST** be used consistently throughout the specification.
  * **Scope:** Terms defined in this clause are normative for the interpretation of requirement statements.

### 3.2 Notation and Conventions [def.notation]

<u>**Definition**</u>

**3.2.1 Mathematical Metavariables**
This specification uses the following metavariables to denote syntactic and semantic entities in formal rules:

  * $x, y, z$: Variables and identifiers.
  * $T, U, V$ (or $\tau$): Types.
  * $e, f, g$: Expressions.
  * $p$: Patterns.
  * $s$: Statements.
  * $\Gamma, \Delta, \Sigma$: Contexts (Typing environments).
  * $\sigma$: Program stores (Memory state).
  * $\pi$: Permissions where $\pi \in \{ \text{const}, \text{unique}, \text{partitioned} \}$.
  * $Tr$: Traits.
  * $\kappa$: Capability objects.
  * $P, Q$: Predicates.
  * $\ell$: Memory locations.
  * $v$: Values.

**3.2.2 Inference Rules**
Formal semantics are specified using Natural Deduction inference rules:

$$
\frac{\text{premise}_1 \quad \cdots \quad \text{premise}_n}{\text{conclusion}} \tag{Rule-Name}
$$

The conclusion holds if and only if all premises hold.

**3.2.3 Grammar Notation**
Syntax is defined using a **Template-Style W3C EBNF**.

* `::=`: Defines a production rule.
* `"literal"`: Terminal string literal.
* `<placeholder>`: Non-terminal.
* `?`: Zero or one occurrences ($0..1$).
* `*`: Zero or more occurrences ($0..N$).
* `+`: One or more occurrences ($1..N$).
* `(...)`: Grouping.
* `|`: Alternative choice (Exclusive OR).

<u>**Static Semantics**</u>

**Syntactic Stability:**
* **Stable Order:** Syntactic forms combining keywords **MUST** use the specific order defined in the grammar.
* **Context-Free Keywords:** Tokens defined as keywords **MUST NOT** be used as identifiers, regardless of context.
* **Constraint:** Implementations **MUST** provide a canonical formatter whose output relies solely on the token stream.

### 3.3 Behavior Classifications [def.behavior]

<u>**Definition**</u>

This specification classifies program behavior into four distinct categories. Every execution path of a valid Cursive program **MUST** fall into one of these categories.

Let $\mathcal{B}$ be the set of all possible program behaviors.
$\mathcal{B} = \{ \text{Defined}, \text{IDB}, \text{USB}, \text{UVB} \}$.

**3.3.1 Defined Behavior**
Behavior that is strictly and explicitly specified by this document. A conforming implementation **MUST** produce the exact results and effects described.

**3.3.2 Implementation-Defined Behavior (IDB)**
Behavior where the specification permits a choice among a fixed set of alternatives, and the implementation **MUST** document its choice.

* **Requirement:** IDB choices **MUST** be recorded in the Conformance Dossier (see §4.3).

**3.3.3 Unspecified Behavior (USB)**
Behavior where the specification permits a set of possible outcomes, and the implementation **MAY** choose any one of them without documentation.

* **Constraint:** The outcome **MUST** be one of the permitted possibilities defined by the specification. It **MUST NOT** result in a violation of memory safety or type safety.

**3.3.4 Unverifiable Behavior (UVB)**
Behavior resulting from operations whose safety cannot be guaranteed by the Cursive static semantics (Type System, Permission System, Lifetime Analysis).

* **Definition:** Operations that depend on external contracts not visible to the compiler (e.g., `unsafe` blocks, FFI).
* **Consequence:** If UVB occurs, the abstract machine enters an undefined state. The specification imposes no requirements on the subsequent behavior of the program.

<u>**Invariants**</u>

* **Safety Partition:**
The set of behaviors for any **Safe Program** $P_{safe}$ (a program containing no `unsafe` blocks) is a subset of definable behaviors.
$$\forall \text{execution } E \text{ of } P_{safe}, \text{Behavior}(E) \in \{ \text{Defined}, \text{IDB}, \text{USB} \}$$
$$\forall \text{execution } E \text{ of } P_{safe}, \text{Behavior}(E) \notin \{ \text{UVB} \}$$

* **Documentation Completeness:**
$$\forall b \in \text{IDB}, \exists d \in \text{Dossier} : \text{Documents}(d, b)$$

<u>**Hazards**</u>

The following constructs introduce the potential for **Unverifiable Behavior (UVB)**. Their use requires the programmer to uphold invariants manually.

1.  **Unsafe Blocks:** `unsafe { ... }` (see §9.5.2).
2.  **Foreign Function Interface:** Calls to `extern` procedures (see §16.1).
3.  **Trusted Contracts:** `[[verify(trusted)]]` (see §13.3).

<u>**Examples**</u>

* **Defined:** `1 + 1` evaluates to `2`.
* **IDB:** `sizeof(usize)` evaluates to 4 or 8 depending on the target architecture documented in the dossier.
* **USB:** The iteration order of a hash map (if not specified as deterministic).
* **UVB:** Dereferencing a null pointer inside an `unsafe` block; Accessing memory after it has been freed via FFI.
