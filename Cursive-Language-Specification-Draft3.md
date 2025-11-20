# Clause 1: Scope

## 1.1 Scope of the Specification

<u>**Definition**</u>
This document specifies the Cursive programming language. It establishes the normative requirements for:
1.  The **lexical structure** and **abstract syntax** of Cursive translation units.
2.  The **static semantics**, including the type system, name resolution, permission logic, and module system rules.
3.  The **dynamic semantics** of the Cursive Abstract Machine (CAM), including evaluation order, memory model, and control flow.
4.  The **standard library** entities and their required behaviors.
5.  The **conformance requirements** for implementations (compilers, interpreters, and tools) and for programs.

This document is the sole normative reference for the Cursive language. In the event of a contradiction between this text and any other documentation, example code, or reference implementation, this text **MUST** take precedence.

<u>**Verification & Invariants**</u>
*   **Normative Authority:** The requirements in this specification are absolute. No implementation may claim conformance if it violates a **MUST** or **MUST NOT** requirement, unless that requirement is explicitly waived by a specific conformance mode (e.g., `permissive` mode behavior as defined in Clause 4).
*   **Consistency:** A conforming program **MUST** rely only on behavior defined in this specification, including documented Implementation-Defined Behavior (IDB), and **MUST NOT** rely on Unspecified Behavior (USB) or Unverifiable Behavior (UVB).

## 1.2 The Cursive Abstract Machine

<u>**Definition**</u>
The Cursive language is defined in terms of the **Cursive Abstract Machine (CAM)**. The CAM is a theoretical execution environment used to define the observable behavior of Cursive programs.

The CAM consists of:
1.  **Storage:** A linear, byte-addressable address space populated by **Objects**.
2.  **Threads of Execution:** One or more sequential flows of control.
3.  **The Root Context:** An initial capability environment provided at startup.

<u>**Static Semantics**</u>
*   **Translation:** A Cursive program is translated into a sequence of operations on the CAM. A conforming implementation **MUST** translate source code such that the observable behavior of the physical execution matches the behavior predicted by the CAM.
*   **Optimization (The "As-if" Rule):** Implementations **MAY** perform any transformation (optimization) on the program, provided that the observable behavior of the resulting execution is identical to that of the CAM for all well-defined inputs.

<u>**Dynamic Semantics**</u>
*   **Host Interaction:** The CAM interacts with the host environment (the physical machine and operating system) through specific, strictly defined interfaces (the "Host").
*   **Hazards (UVB):** The CAM defines specific states (e.g., dereferencing a null pointer, data races) as **Unverifiable Behavior (UVB)**. If the CAM reaches a UVB state, the guarantees of the specification are voided, and the behavior of the machine becomes undefined.

<u>**Memory & Layout**</u>
*   **Storage Durations:** The CAM manages objects with four distinct storage durations:
    *   **Static:** Valid for the entire program execution.
    *   **Automatic (Stack):** Valid for the scope of a block.
    *   **Region:** Valid for the scope of a `region` block (bulk deallocation).
    *   **Dynamic (Heap):** Valid until manually explicitly released via the `HeapAllocator`.
*   **Abstraction:** The CAM abstracts over the physical layout of memory. While it defines the *size* and *alignment* of types relative to one another, it does not dictate the absolute physical addresses used during execution, except where explicitly constrained by Foreign Function Interface (FFI) attributes (e.g., `[[repr(C)]]`).

<u>**Concurrency & Safety**</u>
*   **Execution Model:** The CAM supports concurrent execution via two mutually exclusive paths:
    1.  **Data Parallelism (CREW):** Deterministic parallel execution with Concurrent Read, Exclusive Write enforcement.
    2.  **System Threading (O-Cap):** Stateful, non-deterministic concurrency managed via `System` capabilities and synchronization primitives.
*   **Memory Consistency:** The CAM enforces a memory consistency model where data races are prevented statically via the Permission System (`const`, `unique`, `partitioned`).

## 1.3 Exclusions

<u>**Definition**</u>
This document does **not** specify:

1.  The specific mechanism by which Cursive source code is transformed into executable artifacts (e.g., the internal structure of a compiler, intermediate representations, or object file formats), except where these details affect the observable semantics (e.g., separate compilation).
2.  The mechanisms for invoking, managing, or terminating the execution environment (e.g., command-line interfaces, shell interactions, or debugger protocols).
3.  The size or complexity of specific algorithms used by the implementation, except where complexity guarantees (e.g., $O(1)$) are explicitly mandated by the standard library specifications.
4.  Bindings to operating system APIs beyond the minimal set required by the `System` capability and the standard library.
5.  Style guides, tutorials, or non-normative best practices, except where provided as informative notes.

Migrating content from Chapter 5 [references] to Clause 2 [Normative References] using the [System Definition] template.

---

# Clause 2: Normative References

## 2.0 General

<u>**Definition**</u>
This clause identifies the external documents that are indispensable for the application of this specification. The documents listed in this clause, in whole or in part, are normatively referenced.

<u>**Static Semantics**</u>
The documents listed contain provisions that, through citation in this specification, constitute normative requirements for conforming implementations. References identified as "Informative" in the source material are excluded from this clause.

<u>**Verification & Invariants**</u>
For dated references, only the edition cited applies. For undated references, the latest edition of the referenced document (including any amendments) applies. Where references are not available freely, they are available for purchase from their respective publishers.


## 2.1 ISO/IEC and International Standards

<u>**Definition**</u>
The following ISO/IEC and related international standards are normatively referenced:

  * **[ISO10646]** ISO/IEC 10646:2020, *Information technology — Universal Coded Character Set (UCS)*.
      * **Scope of Normativity:** Defines the character repertoire permitted in source text (Clause 5) and informs identifier classifications.
  * **[IEEE754]** ISO/IEC 60559:2020 (IEEE 754-2019), *Information technology — Microprocessor Systems — Floating-Point Arithmetic*.
      * **Scope of Normativity:** Governs the representation and arithmetic semantics for the `f16` (binary16), `f32` (binary32), and `f64` (binary64) types (Clause 7.3.4).
  * **[C18]** ISO/IEC 9899:2018, *Programming Languages — C*.
      * **Scope of Normativity:** Defines the foundational object representation, calling conventions, and interoperability semantics relied upon by the Foreign Function Interface (FFI) (Clause 16).


## 2.2 Platform and ABI Standards

<u>**Definition**</u>
The following Application Binary Interface (ABI) standards are normatively referenced. Conforming implementations targeting these platforms **MUST** adhere to the relevant specifications for interoperability (Clause 16).

  * **[SysV-ABI-AMD64]** *System V Application Binary Interface, AMD64 Architecture Processor Supplement*.
      * **Scope of Normativity:** Defines the calling convention and data layout for `extern "C"` and `[[repr(C)]]` on POSIX-compliant x86\_64 platforms.
  * **[AAPCS]** *Procedure Call Standard for the Arm® Architecture (AAPCS)*.
      * **Scope of Normativity:** Defines the calling convention and data layout for `extern "C"` and `[[repr(C)]]` on ARM architectures.
  * **[MS-x64]** *Microsoft x64 Calling Convention*.
      * **Scope of Normativity:** Defines the required calling convention and data layout for implementations targeting Windows x64 platforms.


## 2.3 Unicode Standards

<u>**Definition**</u>
The following Unicode Consortium standards and annexes are normatively referenced for the definition of source text encoding and identifier structure:

  * **[Unicode]** The Unicode Standard, Version 14.0.0 or later.
      * **Scope of Normativity:** Supplies derived properties, normalization forms (NFC), and character classifications leveraged by the lexical grammar (Clause 5).
  * **[UAX31]** Unicode Standard Annex \#31, *Unicode Identifier and Pattern Syntax*.
      * **Scope of Normativity:** Specifies identifier composition rules (`XID_Start`, `XID_Continue`) adopted in lexical tokenization (Clause 5.4).


## 2.4 IETF Standards

<u>**Definition**</u>
The following Internet Engineering Task Force (IETF) Requests for Comments (RFCs) are normatively referenced for the definition of normative terminology and encoding formats:

  * **[RFC2119]** IETF RFC 2119, *Key words for use in RFCs to Indicate Requirement Levels*.
      * **Scope of Normativity:** Defines the interpretation of normative vocabulary (e.g., **MUST**, **SHOULD**, **MAY**) used throughout this specification.
  * **[RFC8174]** IETF RFC 8174, *Ambiguity of Uppercase vs Lowercase in RFC 2119 Key Words*.
      * **Scope of Normativity:** Clarifies that requirement keywords apply only when appearing in all uppercase.
  * **[RFC3629]** IETF RFC 3629, *UTF-8, a transformation format of ISO 10646*.
      * **Scope of Normativity:** Defines the UTF-8 encoding scheme mandated for Cursive source text (Clause 5.1).


## 2.5 Conventions and Specifications

<u>**Definition**</u>
The following external specifications are normatively referenced for defining versioning models and grammar notation:

  * **[SemVer2]** Semantic Versioning 2.0.0.
      * **Scope of Normativity:** Defines the versioning model used for the Cursive language specification and associated artifacts (Clause 4.4.1).
  * **[ANTLR4]** ANTLR 4 Grammar Syntax.
      * **Scope of Normativity:** Defines the notation used in the Formal Grammar (Annex A).

---

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

---

# Clause 4: General Principles and Conformance

## 4.1 Implementation Conformance

\<u\>**Definition**\</u\>
An **Implementation** of the Cursive programming language is any system (compiler, interpreter, or static analysis tool) that processes Cursive source text and produces executable artifacts, diagnostic messages, or formal verification results.

A **Conforming Implementation** is one that satisfies the following verification predicate $\text{Conforms}(I)$:

$$
\text{Conforms}(I) \iff \forall P \in \mathbb{P}_{valid}, (\text{Accepts}(I, P) \land \text{Behavior}(I, P) \cong \text{Semantics}(P))
$$Where:

* $\mathbb{P}_{valid}$ is the set of all well-formed Cursive programs.
* $\text{Accepts}(I, P)$ implies $I$ successfully translates $P$ without error diagnostics, respecting all **MUST** requirements defined in this specification.
* $\text{Behavior}(I, P) \cong \text{Semantics}(P)$ implies the observable behavior of the output matches the normative dynamic semantics, subject to permitted **Implementation-Defined Behavior (IDB)** and **Unspecified Behavior (USB)**.

&lt;u&gt;**Constraints & Legality**&lt;/u&gt;
A conforming implementation **MUST**:

1.  **Rejection of Ill-Formedness:** Detect and reject all programs violating syntax or static semantic rules (type safety, ownership, permissions) with at least one error diagnostic, unless the violation is classified as **IFNDR** (Ill-Formed, No Diagnostic Required).
2.  **Dossier Generation:** Be capable of producing a Conformance Dossier artifact (§4.3) characterizing its specific IDB choices and safety claims.
3.  **Mode Support:** Support, at minimum, the **Strict** and **Permissive** conformance modes defined below.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-CFG-4101` | Error | Implementation capability missing (e.g., target architecture not supported). |
| `E-CFG-4102` | Error | Requested conformance mode not supported. |

&lt;u&gt;**Static Semantics (Conformance Modes)**&lt;/u&gt;
Implementations **MUST** provide a configuration mechanism (e.g., compiler flag) to select the conformance mode.

**4.1.1 Strict Mode Requirements (`strict`)**
Strict mode is the target for production systems and formal verification.

* **Safety Gate:** The compilation **MUST** fail with an error if any `unsafe` block in the program reachable from the entry point lacks a valid `[[attestation]]` attribute (§4.5).
* **Warning Elevation:** All diagnostics classified as `Warning` in the standard **MUST** be elevated to `Error` severity, unless explicitly suppressed by a scoped diagnostic directive.
* **Shadowing:** Implicit variable shadowing **MUST** be diagnosed as an error (`E-NAM-1303`).

**4.1.2 Permissive Mode Requirements (`permissive`)**
Permissive mode is the default for development and prototyping.

* **Safety Gate:** Missing attestations on `unsafe` blocks **MUST** generate a `Warning`, not an Error. The compiler **MUST** record these unattested blocks in the "unattested\_violations" section of the Conformance Dossier.
* **Shadowing:** Implicit variable shadowing **MUST** generate a `Warning` (`W-NAM-1303`).

&lt;u&gt;**Examples**&lt;/u&gt;
**Non-Code Example: Compiler CLI Session**

```bash
# Permissive Mode (Default)
$ cursive build main.cur
warning[W-SAF-001]: Unsafe block missing attestation at main.cur:14:5
success: Binary generated at ./bin/main

# Strict Mode
$ cursive build --mode=strict main.cur
error[E-SAF-001]: Unsafe block missing attestation at main.cur:14:5
|
14|     unsafe {
|     ^^^^^^ strict mode requires [[attestation]] for all unsafe blocks
failure: Compilation aborted
```

## 4.2 Program Conformance

&lt;u&gt;**Definition**&lt;/u&gt;
A **Conforming Program** is a set of compilation units that, when linked, forms a complete executable or library that relies exclusively on **Defined Behavior**.

A program $P$ is conforming if:

1.  It is **Well-Formed** according to the syntax and type system rules of this specification.
2.  It is free of **Unverifiable Behavior (UVB)** in `safe` code.
3.  Any usage of `unsafe` code is encapsulated and valid according to the rules of the Abstract Machine (§29.6).
4.  It does not depend on **Implementation-Defined Behavior (IDB)** unless that dependency is documented (e.g., via `[[repr(C)]]` or specific `comptime` checks).

&lt;u&gt;**Static Semantics**&lt;/u&gt;
The compiler **MUST** synthesize the conformance status of the program by analyzing the reachability graph starting from the entry point (`main`).

Let $U$ be the set of all `unsafe` blocks in program $P$.
Let $A(u)$ be the presence of an attestation on block $u$.

$$\\text{Conforming}(P, \\text{Strict}) \\iff \\text{WellFormed}(P) \\land \\forall u \\in U, A(u)
$$If $\exists u \in U : \neg A(u)$, the program is **Non-Conforming** in Strict Mode, though it may still be executable if compiled in Permissive Mode.

\<u\>**Dynamic Semantics**\</u\>
A conforming program **MUST NOT** exhibit **Undefined Behavior** during execution. If the abstract machine enters a state not defined by this specification (e.g., due to incorrect `unsafe` pointer arithmetic), the semantics of the entire program execution become undefined.

## 4.3 The Conformance Dossier

\<u\>**Definition**\</u\>
The **Conformance Dossier** is a machine-readable artifact (JSON) produced by the implementation that documents the safety properties, build configuration, and implementation-defined choices of a compiled artifact.

\<u\>**Constraints & Legality**\</u\>

  * **Generation:** The implementation **MUST** generate the dossier when requested (e.g., via `--emit=dossier`).
  * **Schema Validity:** The output **MUST** validate against the normative JSON Schema defined in **Annex C**.
  * **Completeness:** The dossier **MUST** include:
    1.  **Safety Report:** A complete inventory of every `unsafe` block and its corresponding attestation (or lack thereof).
    2.  **IDB Documentation:** Explicit values for implementation-defined properties used during compilation (e.g., `pointer_width`, `endianness`, `type_layout`).
    3.  **Limits:** The specific resource limits enforced (e.g., max recursion depth).

\<u\>**Static Semantics**\</u\>
The compiler acts as an aggregator. During the **Parsing** and **Semantic Analysis** phases, it **MUST** collect:

1.  All instances of `unsafe` expressions or blocks.
2.  All `[[attestation]]` attributes associated with those blocks.
3.  The source location (file, line, column) of each instance.

This data is serialized into the `safety_report` section of the dossier.

\<u\>**Examples**\</u\>
**Non-Code Example: Dossier Fragment (JSON)**

```json
{
  "metadata": {
    "compiler_id": "cursivec-llvm",
    "version": "1.0.0",
    "target": "x86_64-unknown-linux-gnu"
  },
  "configuration": {
    "mode": "permissive"
  },
  "safety_report": {
    "unsafe_blocks_count": 1,
    "attestations": [
      {
        "location": { "file": "src/hal.cur", "line": 42 },
        "mechanism": "Manual Audit",
        "auditor": "SecurityTeam",
        "justification": "MMIO read requires raw pointer access."
      }
    ],
    "unattested_violations": []
  }
}
```

## 4.4 Language Evolution and Stability

\<u\>**Definition**\</u\>
Cursive adheres to a **Semantic Versioning** model to manage language evolution. The language version is identified by a triplet `MAJOR.MINOR.PATCH`.

  * **MAJOR**: Incompatible changes (breaking grammar or semantics).
  * **MINOR**: Backwards-compatible feature additions.
  * **PATCH**: Backwards-compatible bug fixes and clarifications.

\<u\>**Constraints & Legality**\</u\>

  * **Manifest Declaration:** Every project **MUST** declare its target language version in the `Cursive.toml` manifest.
  * **Compiler Compatibility:** An implementation $I$ with version $V_{impl}$ **MUST** accept a program $P$ declaring version $V_{prog}$ if and only if:
    $$V_{prog}.\text{MAJOR} = V_{impl}.\text{MAJOR} \land V_{prog}.\text{MINOR} \le V_{impl}.\text{MINOR}$$

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-CFG-4401` | Error | Incompatible language version declared in manifest. |
| `E-CFG-4402` | Error | Usage of unstable feature without feature flag. |
| `W-CFG-4401` | Warning | Usage of deprecated feature. |

\<u\>**Static Semantics**\</u\>

**4.4.1 Feature Stability Classes**
Every syntactic construct and library entity is assigned a stability class:

1.  **Stable:** Available by default. Changes strictly follow SemVer.
2.  **Preview:** Available only if explicitly opted-in via the `[project.features]` manifest key. Subject to change in MINOR releases.
3.  **Experimental:** Available only via specific compiler flags. No stability guarantees.

**4.4.2 Deprecation**
Features marked as **Deprecated** remain functional for at least one MINOR release cycle. Use of such features **MUST** trigger a warning (`W-CFG-4401`). Removal occurs only in MAJOR releases.

**4.4.3 Extension Handling**
Implementations **MAY** provide extensions (new attributes, intrinsics).

  * **Naming:** Extensions **MUST** be namespaced using a reverse-domain vendor prefix (e.g., `[[com.vendor.feature]]`).
  * **Reserved Namespace:** The `cursive` namespace and the top-level namespace are reserved for the standard.

## 4.5 The Attestation System

\<u\>**Definition**\</u\>
The **Attestation System** is a formal mechanism to document and audit the usage of `unsafe` code. It couples a human-provided justification with the machine-checked code, ensuring that deviations from the safety model are intentional, audited, and recorded.

### 4.5.1 The `[[attestation]]` Attribute

\<u\>**Syntax**\</u\>

```ebnf
attestation_attribute ::= "[[" "attestation" "(" attestation_args ")" "]]"
attestation_args ::= arg_pair ("," arg_pair)*
arg_pair ::= key ":" literal
key ::= "method" | "auditor" | "date" | "proof" | "comment"
```

\<u\>**Constraints & Legality**\</u\>

  * **Target:** The `[[attestation]]` attribute **MUST** only be applied to `unsafe` blocks or `unsafe` functions. Application to other constructs **SHOULD** trigger a warning (`W-ATT-4501`).
  * **Required Keys:** The attribute **MUST** contain at least the `method` and `auditor` keys.
  * **Value Types:** All values **MUST** be string literals.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-ATT-4501` | Error | Malformed attestation: missing required keys. |
| `W-ATT-4501` | Warning | Attestation applied to safe code (redundant). |

\<u\>**Static Semantics**\</u\>

1.  **Association:** The compiler associates the attestation metadata with the immediately following `unsafe` scope.
2.  **Dossier Emission:** During the dossier generation phase, the values of the attestation arguments are extracted and serialized into the `safety_report`.
3.  **Strict Mode Check:** If the compiler is in Strict Mode and encounters an `unsafe` block *without* an associated `[[attestation]]`, it **MUST** emit `E-SAF-0001` (or equivalent) and abort compilation.

\<u\>**Examples**\</u\>
**Valid Attested Unsafe Block**

```cursive
[[attestation(
    method: "Manual Audit",
    auditor: "J. Doe",
    date: "2025-11-19",
    comment: "FFI call to legacy system; buffer bounds checked above."
)]]
unsafe {
    legacy_c_api(raw_ptr, length);
}
```

---