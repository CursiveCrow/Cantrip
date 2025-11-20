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
    2.  **System Threading (Capability):** Stateful, non-deterministic concurrency managed via `System` capabilities and synchronization primitives.
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
A first-class value $\kappa$ representing the authority to perform an observable external effect (e.g., I/O, memory allocation).

**Context**
The root capability record $\Gamma_{root}$ (of type `Context`) injected into the program entry point, holding all ambient authority granted by the host environment.

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
  * $\kappa$: Capabilities.
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

<u>**Examples**</u>
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

<u>**Definition**</u>
A **Conforming Program** is a set of compilation units that, when linked, forms a complete executable or library that relies exclusively on **Defined Behavior**.

A program $P$ is conforming if:

1.  It is **Well-Formed** according to the syntax and type system rules of this specification.
2.  It is free of **Unverifiable Behavior (UVB)** in `safe` code.
3.  Any usage of `unsafe` code is encapsulated and valid according to the rules of the Abstract Machine (§29.6).
4.  It does not depend on **Implementation-Defined Behavior (IDB)** unless that dependency is documented (e.g., via `[[repr(C)]]` or specific `comptime` checks).

<u>**Static Semantics**</u>
The compiler **MUST** synthesize the conformance status of the program by analyzing the reachability graph starting from the entry point (`main`).

Let $U$ be the set of all `unsafe` blocks in program $P$.
Let $A(u)$ be the presence of an attestation on block $u$.

$$\\text{Conforming}(P, \\text{Strict}) \\iff \\text{WellFormed}(P) \\land \\forall u \\in U, A(u)
$$If $\exists u \in U : \neg A(u)$, the program is **Non-Conforming** in Strict Mode, though it may still be executable if compiled in Permissive Mode.

<u>**Dynamic Semantics**</u>
A conforming program **MUST NOT** exhibit **Undefined Behavior** during execution. If the abstract machine enters a state not defined by this specification (e.g., due to incorrect `unsafe` pointer arithmetic), the semantics of the entire program execution become undefined.

## 4.3 The Conformance Dossier

<u>**Definition**</u>
The **Conformance Dossier** is a machine-readable artifact (JSON) produced by the implementation that documents the safety properties, build configuration, and implementation-defined choices of a compiled artifact.

<u>**Constraints & Legality**</u>

  * **Generation:** The implementation **MUST** generate the dossier when requested (e.g., via `--emit=dossier`).
  * **Schema Validity:** The output **MUST** validate against the normative JSON Schema defined in **Annex C**.
  * **Completeness:** The dossier **MUST** include:
    1.  **Safety Report:** A complete inventory of every `unsafe` block and its corresponding attestation (or lack thereof).
    2.  **IDB Documentation:** Explicit values for implementation-defined properties used during compilation (e.g., `pointer_width`, `endianness`, `type_layout`).
    3.  **Limits:** The specific resource limits enforced (e.g., max recursion depth).

<u>**Static Semantics**</u>
The compiler acts as an aggregator. During the **Parsing** and **Semantic Analysis** phases, it **MUST** collect:

1.  All instances of `unsafe` expressions or blocks.
2.  All `[[attestation]]` attributes associated with those blocks.
3.  The source location (file, line, column) of each instance.

This data is serialized into the `safety_report` section of the dossier.

<u>**Examples**</u>
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

<u>**Definition**</u>
Cursive adheres to a **Semantic Versioning** model to manage language evolution. The language version is identified by a triplet `MAJOR.MINOR.PATCH`.

  * **MAJOR**: Incompatible changes (breaking grammar or semantics).
  * **MINOR**: Backwards-compatible feature additions.
  * **PATCH**: Backwards-compatible bug fixes and clarifications.

<u>**Constraints & Legality**</u>

  * **Manifest Declaration:** Every project **MUST** declare its target language version in the `Cursive.toml` manifest.
  * **Compiler Compatibility:** An implementation $I$ with version $V_{impl}$ **MUST** accept a program $P$ declaring version $V_{prog}$ if and only if:
    $$V_{prog}.\text{MAJOR} = V_{impl}.\text{MAJOR} \land V_{prog}.\text{MINOR} \le V_{impl}.\text{MINOR}$$

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-CFG-4401` | Error | Incompatible language version declared in manifest. |
| `E-CFG-4402` | Error | Usage of unstable feature without feature flag. |
| `W-CFG-4401` | Warning | Usage of deprecated feature. |

<u>**Static Semantics**</u>

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

<u>**Definition**</u>
The **Attestation System** is a formal mechanism to document and audit the usage of `unsafe` code. It couples a human-provided justification with the machine-checked code, ensuring that deviations from the safety model are intentional, audited, and recorded.

### 4.5.1 The `[[attestation]]` Attribute

<u>**Syntax**</u>

```ebnf
attestation_attribute ::= "[[" "attestation" "(" attestation_args ")" "]]"
attestation_args ::= arg_pair ("," arg_pair)*
arg_pair ::= key ":" literal
key ::= "method" | "auditor" | "date" | "proof" | "comment"
```

<u>**Constraints & Legality**</u>

  * **Target:** The `[[attestation]]` attribute **MUST** only be applied to `unsafe` blocks or `unsafe` functions. Application to other constructs **SHOULD** trigger a warning (`W-ATT-4501`).
  * **Required Keys:** The attribute **MUST** contain at least the `method` and `auditor` keys.
  * **Value Types:** All values **MUST** be string literals.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-ATT-4501` | Error | Malformed attestation: missing required keys. |
| `W-ATT-4501` | Warning | Attestation applied to safe code (redundant). |

<u>**Static Semantics**</u>

1.  **Association:** The compiler associates the attestation metadata with the immediately following `unsafe` scope.
2.  **Dossier Emission:** During the dossier generation phase, the values of the attestation arguments are extracted and serialized into the `safety_report`.
3.  **Strict Mode Check:** If the compiler is in Strict Mode and encounters an `unsafe` block *without* an associated `[[attestation]]`, it **MUST** emit `E-SAF-0001` (or equivalent) and abort compilation.

<u>**Examples**</u>
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
**4.1.1 Strict Mode Requirements (`strict`)**
Strict mode is the target for production systems and formal verification.

* **Safety Gate:** The compilation **MUST** fail with an error if any `unsafe` block in the program reachable from the entry point lacks a valid `[[attestation]]` attribute (§4.5).
* **Warning Elevation:** All diagnostics classified as `Warning` in the standard **MUST** be elevated to `Error` severity, unless explicitly suppressed by a scoped diagnostic directive.
* **Shadowing:** Implicit variable shadowing **MUST** be diagnosed as an error (`E-NAM-1303`).

**4.1.2 Permissive Mode Requirements (`permissive`)**
Permissive mode is the default for development and prototyping.

* **Safety Gate:** Missing attestations on `unsafe` blocks **MUST** generate a `Warning`, not an Error. The compiler **MUST** record these unattested blocks in the "unattested\_violations" section of the Conformance Dossier.
* **Shadowing:** Implicit variable shadowing **MUST** generate a `Warning` (`W-NAM-1303`).

<u>**Examples**</u>
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

<u>**Definition**</u>
A **Conforming Program** is a set of compilation units that, when linked, forms a complete executable or library that relies exclusively on **Defined Behavior**.

A program $P$ is conforming if:

1.  It is **Well-Formed** according to the syntax and type system rules of this specification.
2.  It is free of **Unverifiable Behavior (UVB)** in `safe` code.
3.  Any usage of `unsafe` code is encapsulated and valid according to the rules of the Abstract Machine (§29.6).
4.  It does not depend on **Implementation-Defined Behavior (IDB)** unless that dependency is documented (e.g., via `[[repr(C)]]` or specific `comptime` checks).

<u>**Static Semantics**</u>
The compiler **MUST** synthesize the conformance status of the program by analyzing the reachability graph starting from the entry point (`main`).

Let $U$ be the set of all `unsafe` blocks in program $P$.
Let $A(u)$ be the presence of an attestation on block $u$.

$$\\text{Conforming}(P, \\text{Strict}) \\iff \\text{WellFormed}(P) \\land \\forall u \\in U, A(u)
$$If $\exists u \in U : \neg A(u)$, the program is **Non-Conforming** in Strict Mode, though it may still be executable if compiled in Permissive Mode.

<u>**Dynamic Semantics**</u>
A conforming program **MUST NOT** exhibit **Undefined Behavior** during execution. If the abstract machine enters a state not defined by this specification (e.g., due to incorrect `unsafe` pointer arithmetic), the semantics of the entire program execution become undefined.

## 4.3 The Conformance Dossier

<u>**Definition**</u>
The **Conformance Dossier** is a machine-readable artifact (JSON) produced by the implementation that documents the safety properties, build configuration, and implementation-defined choices of a compiled artifact.

<u>**Constraints & Legality**</u>

  * **Generation:** The implementation **MUST** generate the dossier when requested (e.g., via `--emit=dossier`).
  * **Schema Validity:** The output **MUST** validate against the normative JSON Schema defined in **Annex C**.
  * **Completeness:** The dossier **MUST** include:
    1.  **Safety Report:** A complete inventory of every `unsafe` block and its corresponding attestation (or lack thereof).
    2.  **IDB Documentation:** Explicit values for implementation-defined properties used during compilation (e.g., `pointer_width`, `endianness`, `type_layout`).
    3.  **Limits:** The specific resource limits enforced (e.g., max recursion depth).

<u>**Static Semantics**</u>
The compiler acts as an aggregator. During the **Parsing** and **Semantic Analysis** phases, it **MUST** collect:

1.  All instances of `unsafe` expressions or blocks.
2.  All `[[attestation]]` attributes associated with those blocks.
3.  The source location (file, line, column) of each instance.

This data is serialized into the `safety_report` section of the dossier.

<u>**Examples**</u>
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

<u>**Definition**</u>
Cursive adheres to a **Semantic Versioning** model to manage language evolution. The language version is identified by a triplet `MAJOR.MINOR.PATCH`.

  * **MAJOR**: Incompatible changes (breaking grammar or semantics).
  * **MINOR**: Backwards-compatible feature additions.
  * **PATCH**: Backwards-compatible bug fixes and clarifications.

<u>**Constraints & Legality**</u>

  * **Manifest Declaration:** Every project **MUST** declare its target language version in the `Cursive.toml` manifest.
  * **Compiler Compatibility:** An implementation $I$ with version $V_{impl}$ **MUST** accept a program $P$ declaring version $V_{prog}$ if and only if:
    $$V_{prog}.\text{MAJOR} = V_{impl}.\text{MAJOR} \land V_{prog}.\text{MINOR} \le V_{impl}.\text{MINOR}$$

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-CFG-4401` | Error | Incompatible language version declared in manifest. |
| `E-CFG-4402` | Error | Usage of unstable feature without feature flag. |
| `W-CFG-4401` | Warning | Usage of deprecated feature. |

<u>**Static Semantics**</u>

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

<u>**Definition**</u>
The **Attestation System** is a formal mechanism to document and audit the usage of `unsafe` code. It couples a human-provided justification with the machine-checked code, ensuring that deviations from the safety model are intentional, audited, and recorded.

### 4.5.1 The `[[attestation]]` Attribute

<u>**Syntax**</u>

```ebnf
attestation_attribute ::= "[[" "attestation" "(" attestation_args ")" "]]"
attestation_args ::= arg_pair ("," arg_pair)*
arg_pair ::= key ":" literal
key ::= "method" | "auditor" | "date" | "proof" | "comment"
```

<u>**Constraints & Legality**</u>

  * **Target:** The `[[attestation]]` attribute **MUST** only be applied to `unsafe` blocks or `unsafe` functions. Application to other constructs **SHOULD** trigger a warning (`W-ATT-4501`).
  * **Required Keys:** The attribute **MUST** contain at least the `method` and `auditor` keys.
  * **Value Types:** All values **MUST** be string literals.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-ATT-4501` | Error | Malformed attestation: missing required keys. |
| `W-ATT-4501` | Warning | Attestation applied to safe code (redundant). |

<u>**Static Semantics**</u>

1.  **Association:** The compiler associates the attestation metadata with the immediately following `unsafe` scope.
2.  **Dossier Emission:** During the dossier generation phase, the values of the attestation arguments are extracted and serialized into the `safety_report`.
3.  **Strict Mode Check:** If the compiler is in Strict Mode and encounters an `unsafe` block *without* an associated `[[attestation]]`, it **MUST** emit `E-SAF-0001` (or equivalent) and abort compilation.

<u>**Examples**</u>
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
legacy_c_api(raw_ptr, length);
}
```

---

# Clause 5: Lexical Structure

## 5.1 Character Sets

<u>**Definition**</u>
The **Cursive Character Set** $\Sigma_{cur}$ is defined as the set of Unicode scalar values encoded in UTF-8.
*   **Source Encoding:** Cursive source files **MUST** be encoded in UTF-8 (RFC 3629).
*   **Scalar Values:** The atomic unit of processing is the Unicode Scalar Value $u \in U_{scalar}$, where $U_{scalar} = [0, \text{D7FF}_{16}] \cup [\text{E000}_{16}, \text{10FFFF}_{16}]$.
*   **Normalization:** Source text is processed in Normalized Form C (NFC) for identifiers.

<u>**Invariants**</u>
The source ingestion pipeline $P_{ingest}$ must satisfy the following invariants for any input byte stream $B$:

1.  **Valid UTF-8:** $\forall b \in B, \text{IsValidUTF8}(b)$. Overlong encodings and surrogate code points **MUST** be rejected.
2.  **No Byte Order Mark (BOM):** The output stream $S$ **MUST NOT** contain U+FEFF.
    $$S = P_{ingest}(B) \implies \text{U+FEFF} \notin S$$
3.  **Prohibited Characters:** The source **MUST NOT** contain control characters (Cc category) except for Horizontal Tab (U+0009), Line Feed (U+000A), Form Feed (U+000C), and Carriage Return (U+000D).

<u>**Semantics**</u>
The **Source Ingestion Pipeline** transforms a raw byte stream into a sequence of normalized Unicode scalar values. This process **MUST** occur in the following deterministic order:

1.  **Size Validation:** The implementation **MUST** verify that $|B| \le \text{MAX\_FILE\_SIZE}$ (Minimum limit: 1 MiB).
2.  **Decoding:** The byte stream is decoded into Unicode scalar values. Invalid sequences trigger `E-SRC-0101`.
3.  **BOM Stripping:** If $S[0] = \text{U+FEFF}$, it is discarded.
4.  **Line Ending Normalization:** All line ending sequences (CR, LF, CRLF) are normalized to a single Line Feed (LF, U+000A).
5.  **Prohibited Character Check:** The stream is scanned for prohibited control characters.

<u>**Hazards**</u>
Failure to adhere to the encoding rules results in immediate rejection of the compilation unit.

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-SRC-0101` | Error | Invalid UTF-8 byte sequence. |
| `E-SRC-0102` | Error | Source file exceeds implementation-defined maximum size. |
| `E-SRC-0103` | Error | Embedded BOM found after the first position. |
| `E-SRC-0104` | Error | Forbidden control character or null byte. |
| `W-SRC-0101` | Warning | UTF-8 BOM present (stripped). |

## 5.2 Translation Phases

<u>**Definition**</u>
The translation of a Cursive program is a deterministic function $T: \text{Source} \to \text{Artifacts}$ composed of four distinct phases.

<u>**Invariants**</u>
*   **Determinism:** For a given input $I$ and configuration $C$, the output $O$ is invariant.
    $$\forall t_1, t_2 : T(I, C)_{t_1} \equiv T(I, C)_{t_2}$$
*   **Phase Ordering:** The phases **MUST** execute in the strict order defined below.

<u>**Semantics**</u>
The **Translation Pipeline** consists of:

1.  **Phase 1: Parsing ($\Phi_1$)**
    *   Input: Normalized Source Text.
    *   Output: Abstract Syntax Tree (AST).
    *   Action: Tokenization, parsing, and symbol table population. Handles "Two-Phase Compilation" for forward references.

2.  **Phase 2: Compile-Time Execution ($\Phi_2$)**
    *   Input: AST.
    *   Output: Expanded AST.
    *   Action: Execution of `comptime` blocks, metaprogramming expansion (`quote`/`splice`).

3.  **Phase 3: Semantic Analysis ($\Phi_3$)**
    *   Input: Expanded AST.
    *   Output: Validated AST + Type/Effect Table.
    *   Action: Type checking, permission checking (LPS), Capability analysis.

4.  **Phase 4: Code Generation ($\Phi_4$)**
    *   Input: Validated AST.
    *   Output: Machine Code + Conformance Dossier.
    *   Action: IR lowering, optimization, artifact emission.

$$T(Source) = \Phi_4(\Phi_3(\Phi_2(\Phi_1(Source))))$$

<u>**Hazards**</u>
*   **Circular Dependencies:** Circular dependencies in `comptime` execution or variable initialization **MUST** be detected and reported as errors (`E-CMP-0201`).

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-CMP-0201` | Error | Circular dependency detected in compile-time execution. |

## 5.3 Lexical Elements

<u>**Definition**</u>
Lexical analysis converts the normalized sequence of scalar values into a stream of tokens, comments, and whitespace.

### 5.3.1 Comments

<u>**Definition**</u>
Comments are lexical elements used for documentation and annotation. They do not contribute to the syntactic structure of the program but may be preserved for tooling.

<u>**Syntax**</u>
```ebnf
comment ::= line_comment | block_comment
line_comment ::= "//" [^\n]*
block_comment ::= "/*" ( [^*] | "*" [^/] | block_comment )* "*/"
```
*Note: The EBNF for `block_comment` is an approximation. Block comments support arbitrary nesting.*

<u>**Constraints & Legality**</u>
*   **Nesting:** Block comments `/* ... */` **MUST** nest correctly.
*   **Termination:** An unterminated block comment at EOF **MUST** trigger `E-LEX-0301`.
*   **Content:** Comments **MAY** contain any Unicode scalar value.

<u>**Static Semantics**</u>
*   **Erasure:** Comments are effectively replaced by whitespace during tokenization, except for Documentation Comments (`///` and `//!`) which are attached to the AST.
*   **No Tokens:** Comments **MUST NOT** produce tokens in the output stream.

### 5.3.2 Whitespace

<u>**Definition**</u>
Whitespace serves as a delimiter between tokens and drives the layout-sensitive statement termination logic.

<u>**Syntax**</u>
```ebnf
whitespace ::= " " | "\t" | "\f" | "\n"
```

<u>**Static Semantics**</u>
*   **Separation:** Whitespace separates tokens (e.g., `let x` vs `letx`).
*   **Newlines:** The Line Feed (U+000A) is tokenized as `<newline>` and is semantically significant for statement termination (see Clause 10.3). All other whitespace is discarded.

### 5.3.3 Tokens

<u>**Definition**</u>
A token is the fundamental syntactic unit of the language.

<u>**Syntax**</u>
```ebnf
token ::= identifier | keyword | literal | operator | punctuator | newline
```

<u>**Constraints & Legality**</u>
*   **Maximal Munch:** The lexer **MUST** consume the longest possible valid token at each step.
*   **Unclassifiable Sequences:** Any sequence that cannot be classified as a valid token or whitespace **MUST** trigger a diagnostic.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-LEX-0301` | Error | Unterminated block comment. |
| `E-LEX-0302` | Error | Invalid character in source input. |

## 5.4 Identifiers

<u>**Definition**</u>
Identifiers are names used to bind entities (variables, types, procedures).

<u>**Syntax**</u>
```ebnf
identifier ::= xid_start xid_continue*
```
*   `xid_start`: Unicode `XID_Start` property + `_`
*   `xid_continue`: Unicode `XID_Continue` property

<u>**Constraints & Legality**</u>
*   **Reserved Words:** Identifiers **MUST NOT** match any reserved keyword (Clause 5.4.2). Violation triggers `E-LEX-0401`.
*   **Length:** Identifiers **MUST NOT** exceed implementation-defined limits (Minimum: 1,023 scalar values). Violation triggers `E-LEX-0402`.
*   **Prohibited:** Identifiers **MUST NOT** contain prohibited characters or non-characters.

<u>**Static Semantics**</u>
*   **Normalization:** Identifiers are normalized to NFC before comparison.
*   **Equivalence:** Two identifiers $id_1, id_2$ are equivalent iff $\text{NFC}(id_1) = \text{NFC}(id_2)$.

<u>**Examples**</u>
```cursive
// Valid
let my_var = 10
let _unused = 0
let αβγ = 3.14

// Invalid
let 1var = 0      // Starts with digit
let break = 0     // Reserved keyword
let my-var = 0    // Hyphen not allowed (parsed as subtraction)
```

### 5.4.2 Keywords

<u>**Definition**</u>
Keywords are reserved lexical forms that have special meaning in the language.

<u>**Constraints**</u>
The following identifiers are **RESERVED** and **MUST NOT** be used as user-defined identifiers:
`abstract`, `as`, `break`, `char`, `comptime`, `const`, `continue`, `double`, `else`, `enum`, `extern`, `false`, `float`, `fork`, `if`, `half`, `imm`, `import`, `int`, `internal`, `let`, `loop`, `match`, `modal`, `module`, `move`, `mut`, `override`, `parallel`, `partitioned`, `private`, `procedure`, `protected`, `public`, `quote`, `record`, `region`, `result`, `return`, `self`, `Self`, `shadow`, `static`, `true`, `trait`, `type`, `uint`, `unique`, `unsafe`, `use`, `var`, `witness`, `where`.

### 5.4.3 Reserved Identifiers

<u>**Definition**</u>
Certain identifier patterns are reserved for implementation or specification use.

<u>**Constraints**</u>
*   **Cursive Namespace:** The `cursive.*` namespace is reserved.
*   **Implementation:** Identifiers starting with `__` (double underscore) are reserved for implementation artifacts.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-LEX-0401` | Error | Usage of reserved keyword as identifier. |
| `E-LEX-0402` | Error | Identifier exceeds maximum length limit. |
| `E-LEX-0403` | Error | Usage of reserved namespace or prefix. |

## 5.5 Literals

<u>**Definition**</u>
Literals are source code representations of fixed values.

### 5.5.1 Integer Literals

<u>**Syntax**</u>
```ebnf
integer_literal ::= (decimal | hex | octal | binary) suffix?
decimal ::= [0-9] ("_"? [0-9])*
hex     ::= "0x" [0-9a-fA-F] ("_"? [0-9a-fA-F])*
octal   ::= "0o" [0-7] ("_"? [0-7])*
binary  ::= "0b" [0-1] ("_"? [0-1])*
suffix  ::= "u8" | "u16" | "u32" | "u64" | "i8" | "i16" | "i32" | "i64"
```

<u>**Constraints & Legality**</u>
*   **Value Range:** The literal value **MUST** fit within the range of the specified (or inferred) type. Violation triggers `E-LEX-0501`.
*   **Underscores:** Underscores **MUST NOT** appear at the start, end, or immediately after the prefix.

<u>**Static Semantics**</u>
*   **Type Inference:** If no suffix is present, the type is inferred from context, defaulting to `i32` if unconstrained.

### 5.5.2 Floating-Point Literals

<u>**Syntax**</u>
```ebnf
float_literal ::= digit_seq "." digit_seq exponent? suffix?
                | digit_seq exponent suffix?
digit_seq     ::= [0-9] ("_"? [0-9])*
exponent      ::= ("e" | "E") ("+" | "-")? digit_seq
suffix        ::= "f16" | "f32" | "f64"
```

<u>**Constraints & Legality**</u>
*   **Format:** A floating-point literal **MUST** contain either a decimal point or an exponent (or both) to distinguish it from an integer.

### 5.5.3 Character Literals

<u>**Syntax**</u>
```ebnf
char_literal ::= "'" (char_content | escape_seq) "'"
char_content ::= [^'\\\n\r]
escape_seq   ::= "\\" ("n" | "r" | "t" | "\\" | "'" | '"' | "0" | "x" hex_digit{2} | "u{" hex_digit+ "}")
hex_digit    ::= [0-9a-fA-F]
```

<u>**Constraints & Legality**</u>
*   **Length:** **MUST** represent exactly one Unicode scalar value. Violation triggers `E-LEX-0502`.
*   **Escapes:** Invalid escape sequences trigger `E-LEX-0503`.

### 5.5.4 String Literals

<u>**Syntax**</u>
```ebnf
string_literal ::= '"' (string_content | escape_seq)* '"'
string_content ::= [^"\\\n\r]
```

<u>**Constraints & Legality**</u>
*   **No Newlines:** Unescaped line feeds (U+000A) **MUST NOT** appear in a string literal.
*   **Encoding:** The string value is the UTF-8 encoding of the sequence of scalar values.

### 5.5.5 Boolean Literals

<u>**Syntax**</u>
```ebnf
bool_literal ::= "true" | "false"
```

<u>**Static Semantics**</u>
*   **Type:** The type of a boolean literal is `bool`.

<u>**Examples**</u>
```cursive
// Integers
let a = 42
let b = 0xDEAD_BEEF_u32
let c = 0b1010_1010

// Floats
let d = 3.14159
let e = 1.0e-10_f64

// Characters
let f = 'x'
let g = '\n'
let h = '\u{1F4A9}'

// Strings
let i = "Hello, World!"
let j = "Line 1\nLine 2"
```

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-LEX-0501` | Error | Literal value out of range for type. |
| `E-LEX-0502` | Error | Character literal must contain exactly one scalar value. |
| `E-LEX-0503` | Error | Invalid escape sequence in literal. |
| `E-LEX-0504` | Error | Invalid suffix for literal type. |

# Clause 6: Basic Concepts (The Abstract Machine)

## 6.1 Declarations and Scopes

<u>**Definition**</u>
A **Declaration** is a syntactic form that introduces a name (identifier) into the program and binds it to a static entity (such as a variable, type, module, or procedure).

A **Scope** is the lexical region of source text where a specific set of name bindings is valid. Cursive employs strict lexical scoping, where scopes are nested hierarchically corresponding to the block structure of the program.

**Unified Namespace:**
Cursive **MUST** be implemented with a single, unified namespace per scope. An identifier's meaning is determined solely by its spelling and the scope resolution, independent of the syntactic context of its use (e.g., a type name and a variable name cannot collide).

<u>**Static Semantics**</u>
*   **Name Introduction:** A declaration $D$ introducing name $x$ into scope $S$ is well-formed if and only if $x$ is not already bound in $S$.
    $$ \forall x, S: \text{Declare}(x, S) \implies x \notin \text{dom}(S) $$
*   **Shadowing:** A declaration in an inner scope $S_{inner}$ **MAY** bind a name $x$ that is already bound in an outer scope $S_{outer}$.
    *   **Explicit Shadowing:** The declaration **MUST** use the `shadow` keyword.
    *   **Implicit Shadowing:** Shadowing without the `shadow` keyword is **discouraged** and subject to conformance mode checks (see §4.1).

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-NAM-1302` | Error | Duplicate name: the identifier is already declared in this scope. |
| `E-NAM-1303` | Error | Shadowing existing binding without `shadow` keyword (Strict Mode). |
| `W-NAM-1303` | Warning | Shadowing existing binding without `shadow` keyword (Permissive Mode). |
| `E-NAM-1306` | Error | Unnecessary use of `shadow` keyword (no outer binding exists). |

### 6.1.1 Scope Nesting

<u>**Definition**</u>
The **Scope Context** $\Gamma$ represents the stack of active scopes at any point in the program. It is an ordered list $\Gamma = [S_{local}, S_{proc}, S_{module}, S_{universe}]$, where $S_{local}$ is the innermost (active) scope.

<u>**Static Semantics**</u>
*   **Scope Creation:** Entering a block expression (`{ ... }`), loop, or function body pushes a new empty scope $S_{new}$ onto $\Gamma$.
*   **Scope Destruction:** Exiting a block pops the innermost scope.
*   **Visibility:** Bindings in $S_{local}$ are immediately visible. Bindings in outer scopes are visible unless shadowed.

### 6.1.2 Name Resolution Algorithm

<u>**Objective**</u>
To determine the unique entity $E$ denoted by an identifier $x$ or a qualified path $p$ within a specific scope context $\Gamma$.

<u>**Abstract Domain**</u>
*   **Input:** Identifier $x$ or Path $p$, Context $\Gamma$.
*   **Output:** Entity $E$ or Error $\bot$.

<u>**Execution Steps**</u>

**1. Unqualified Lookup ($x$):**
The implementation **MUST** search the scope stack $\Gamma$ from innermost ($S_0$) to outermost ($S_n$).

$$
\frac{x \in \text{dom}(S_0)}{\Gamma \vdash x \Rightarrow S_0(x)}
\tag{Step-Local}
$$

$$
\frac{x \notin \text{dom}(S_0) \quad [S_1, \dots, S_n] \vdash x \Rightarrow E}{\Gamma \vdash x \Rightarrow E}
\tag{Step-Recurse}
$$

If the stack is exhausted and $x$ is not found, the algorithm terminates with `E-NAM-1301`.

**2. Qualified Lookup ($p::i$):**
To resolve a path $p::i$:
1.  Resolve the prefix $p$ to a module or type $M$ using this algorithm recursively.
2.  Search for $i$ within the export set of $M$.
3.  Verify accessibility (see §6.1.3).

$$
\frac{\Gamma \vdash p \Rightarrow M \quad M \vdash i \Rightarrow E \quad \Gamma \vdash \text{can\_access}(E)}{\Gamma \vdash p::i \Rightarrow E}
\tag{Step-Qualified}
$$

<u>**Determinism**</u>
The name resolution algorithm is deterministic. Given a well-formed program and a specific location, an identifier always resolves to the same entity.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-NAM-1301` | Error | Unresolved name: cannot find identifier in any accessible scope. |
| `E-NAM-1304` | Error | Unresolved module: a path prefix did not resolve to a module. |
| `E-NAM-1305` | Error | Unresolved or private item in path. |

### 6.1.3 Visibility and Access

<u>**Definition**</u>
**Visibility** controls the accessibility of declarations across module boundaries. Every top-level declaration has an assigned visibility level.

*   **`public`**: Visible to all modules.
*   **`internal`** (Default): Visible only within the defining assembly.
*   **`private`**: Visible only within the defining module.
*   **`protected`**: Visible within the defining type and its trait implementations (intra-assembly only).

<u>**Constraints & Legality**</u>
*   **Access Check:** A usage of entity $E$ at location $L$ is legal if and only if $E$ is visible at $L$.
*   **Protected Constraint:** The `protected` modifier **MUST NOT** be used on top-level declarations.

<u>**Static Semantics**</u>
The judgment $\Gamma \vdash \text{can\_access}(E)$ is defined as:

1.  **Public:** Always true.
2.  **Internal:** True iff $Assembly(\Gamma) = Assembly(E)$.
3.  **Private:** True iff $Module(\Gamma) = Module(E)$.
4.  **Protected:** True iff $\Gamma$ is within $Type(E)$ OR ($\Gamma$ is within `impl` of $Type(E)$ AND $Assembly(\Gamma) = Assembly(E)$).

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-MOD-1207` | Error | Cannot access a `protected` item from this scope. |
| `E-MOD-1205` | Error | Attempt to `public use` a non-public item. |

Migrating content from **Part 6, Chapter 29 (The Cursive Memory Model)** to **Clause 6.2 (The Object Model)** using the **System Definition** template.

-----

## 6.2 The Object Model

\<u\>**Definition**\</u\>

The Cursive Object Model defines the abstract entities that exist during program execution. An **Object** is a discrete entity within the abstract machine, formally defined by the tuple:

$$O = (S, T, L, V)$$

Where:

  * $S$ (**Storage**): A contiguous sequence of bytes in the virtual address space, characterized by a start address $addr(S)$ and a size in bytes $size(S)$.
  * $T$ (**Type**): A static classification determining the interpretation of the bits in $S$, the alignment requirements $align(T)$, and the set of valid operations.
  * $L$ (**Lifetime**): The temporal interval $[t_{start}, t_{end})$ during execution for which $S$ is valid, reserved, and strictly associated with $O$.
  * $V$ (**Value**): The specific bit-pattern contained within $S$ at a given time $t \in L$, which **MUST** constitute a valid representation for type $T$.

The memory model enforces safety through two orthogonal axes:

1.  **Liveness**: The guarantee that an access to $O$ occurs only when $t \in L$.
2.  **Aliasing**: The management of multiple paths (bindings or pointers) to $S$, ensuring data race freedom via the Permission System (Clause 7.2).

\<u\>**Constraints & Legality**\</u\>

The compiler **MUST** enforce the following constraints on all objects:

1.  **Zero Runtime Overhead:** With the exception of dynamic bounds checking for arrays/slices and user-opt-in runtime contracts, all memory safety checks defined in this clause **MUST** be resolved at compile time. The implementation **MUST NOT** insert implicit reference counting or garbage collection barriers.
2.  **Explicit Management:** Operations affecting ownership, allocation, or synchronization **MUST** be syntactically explicit (e.g., `^` for region allocation, `move` for ownership transfer).

\<u\>**Static Semantics**\</u\>

The compiler **MUST** model the state of every object to enforce the **Liveness Invariant**:

$$\forall \text{access}(O) \text{ at } t, \quad \text{start}(L) \le t < \text{end}(L)$$

To support this, objects are categorized by their **Storage Duration**, which determines how $L$ is derived.

### 6.2.1 Objects and Storage

Objects are classified into four storage duration categories. The implementation **MUST** manage the lifetime of objects according to their category:

| Duration Category | Allocation Event | Deallocation Event | Provenance Tag |
| :--- | :--- | :--- | :--- |
| **Static** | Program Load | Program Exit | `Global` |
| **Automatic (Stack)** | Scope Entry | Scope Exit (LIFO) | `Stack` |
| **Region (Arena)** | `^` Expression | Region Scope Exit | `Region(ID)` |
| **Dynamic (Heap)** | Explicit `alloc` | Explicit `free` / RAII | `Heap` |

**Validation Logic:**

  * **Static Duration:** Objects declared at module level or marked `static`. The lifetime is the entire program execution.
  * **Automatic Duration:** Local bindings (`let`/`var`). The lifetime corresponds strictly to the lexical scope.
  * **Region Duration:** Objects allocated via the region operator `^`. The lifetime is bound to the `Arena` backing the enclosing `region` block.
  * **Dynamic Duration:** Objects allocated via `HeapAllocator`. The lifetime is managed manually or via responsible types (e.g., `Box`, `Vec`).

**Provenance Invariant:**
The compiler **MUST** track the provenance $\pi$ of every pointer. A pointer with provenance $\pi_{source}$ **MUST NOT** be stored in a location with lifetime $L_{target}$ if the source lifetime is shorter than the target lifetime.

$$\text{valid\_store}(ptr, target) \iff \text{lifetime}(\pi_{ptr}) \supseteq \text{lifetime}(target)$$

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-MEM-3020` | Error | Region pointer escape: Attempt to assign a pointer with `Region` provenance to a location that outlives the region. |
| `E-MEM-3021` | Error | Region allocation `^` used outside of a valid `region` scope. |

\<u\>**Dynamic Semantics**\</u\>

The runtime lifecycle of an object $O$ proceeds through three phases:

1.  **Allocation & Initialization ($t_{start}$):**

      * Storage $S$ is reserved.
      * If $S$ cannot be reserved (OOM), the thread **MUST** panic (for Automatic/Region/Dynamic duration).
      * A valid initial value $V_{init}$ is written to $S$.
      * **Invariant:** $V_{init} \in \text{ValidValues}(T)$.

2.  **Usage ($t \in L$):**

      * The value $V$ is read or modified.
      * **Access Hazard:** Accessing $S$ via a raw pointer when $t \notin L$ constitutes **Unverifiable Behavior (UVB)**.

3.  **Destruction & Deallocation ($t_{end}$):**

      * The object's cleanup logic (`Drop::drop`) is executed.
      * $S$ is released.
      * **Invariant:** After $t_{end}$, any pointer to $S$ possesses the `@Expired` state (in the modal model) or is a dangling raw pointer.

\<u\>**Memory & Layout**\</u\>

### 6.2.2 Alignment and Padding

Every type $T$ has a statically determined size and alignment.

**Definitions:**

  * **Size (`sizeof(T)`):** The number of bytes of storage required to hold an instance of $T$, including any internal or trailing padding.
  * **Alignment (`alignof(T)`):** An integer $N = 2^k$ ($k \ge 0$). The address of any object of type $T$ **MUST** be a multiple of $N$.

**Constraints:**

  * $\text{sizeof}(T) \pmod{\text{alignof}(T)} \equiv 0$.
  * Zero-sized types (`()`, `!`) have $\text{sizeof} = 0$ and $\text{alignof} = 1$.

**Primitive Layouts:**

| Type Family | Type(s) | Size (Bytes) | Alignment (Bytes) |
| :--- | :--- | :--- | :--- |
| **Byte** | `i8`, `u8`, `bool` | 1 | 1 |
| **Short** | `i16`, `u16`, `f16` | 2 | 2 |
| **Word** | `i32`, `u32`, `f32`, `char` | 4 | 4 |
| **Double** | `i64`, `u64`, `f64` | 8 | 8 |
| **Quad** | `i128`, `u128` | 16 | Platform (8 or 16) |
| **Pointer** | `usize`, `isize`, `*T`, `Ptr<T>` | Platform (4 or 8) | Platform (4 or 8) |

**Composite Layouts:**

1.  **Default Layout:** For `record` and `enum` types without attributes, the layout (field ordering and padding) is **Implementation-Defined Behavior (IDB)**. The implementation **MAY** reorder fields to minimize padding.
2.  **C Layout (`[[repr(C)]]`):**
      * Fields **MUST** be stored in declaration order.
      * Padding **MUST** be inserted only to satisfy alignment requirements of subsequent fields and the total type size.
      * This layout **MUST** be compatible with the system C ABI.
3.  **Arrays (`[T; N]`):**
      * $\text{sizeof}([T; N]) = \text{sizeof}(T) \times N$.
      * $\text{alignof}([T; N]) = \text{alignof}(T)$.
      * There is **NO** padding between elements.

**Dense Pointer Layout:**
Certain types are represented as "dense pointers" (fat pointers), occupying two machine words ($2 \times \text{sizeof}(usize)$):

  * **Slice (`[T]`):** `(ptr: *T, len: usize)`
  * **Witness (`witness T`):** `(data: *void, vtable: *VTable)`

<u>**Examples**</u>

**Example 1: Object Lifecycle and Liveness**

```cursive
region r {
    // Allocation: 'x' is allocated in arena 'r'.
    // Lifecycle: Starts here. Provenance: Region('r').
    let x = ^ 42;

    // Usage: Valid because we are inside 'r'.
    print(x);

    // Escape Error:
    // The provenance of 'x' is Region('r').
    // 'global_ptr' has Static lifetime.
    // Static > Region('r'), so this assignment is rejected.
    // global_ptr = &x; // Error E-MEM-3020
}
// Destruction: 'r' exits. 'x' is deallocated.
// Liveness: 'x' is no longer live.
```

**Example 2: Memory Layout (Visual)**
Consider a `[[repr(C)]]` record on a 64-bit system:

```cursive
[[repr(C)]]
record Mixed {
    a: u8,
    b: u32,
    c: u64
}
```

**Memory Map:**

```text
Offset | Size | Content     | Reason
-------|------|-------------|------------------------------
0x00   | 1    | field 'a'   | u8 requires align 1
0x01   | 3    | [PADDING]   | field 'b' requires align 4, next offset must be 0x04
0x04   | 4    | field 'b'   | u32
0x08   | 8    | field 'c'   | u64 requires align 8
-------|------|-------------|------------------------------
Total  | 16   | Mixed       | sizeof must be multiple of align (8)
```

Correction required: **Data Loss detected.** The source text (§29.2.5.3 "Panic Semantics") regarding destructor behavior during unwinding was omitted from the *Dynamic Semantics* section.

Regenerating section `6.3 The Responsibility Model` to include missing normative requirements...

## 6.3 The Responsibility Model

<u>**Definition**</u>

The **Responsibility Model** is the Cursive mechanism for enforcing Resource Acquisition Is Initialization (RAII) and ensuring deterministic resource cleanup. It is a static ownership system that assigns the duty of deallocation to exactly one named binding at any point in the program's execution.

This model operates on the principle of **Responsible Bindings**. A binding $b$ is responsible if it holds the exclusive right and duty to manage the lifetime of an object $O$.

**Formal Definition:**

Let $\mathcal{B}$ be the set of all variable bindings in a scope.

Let $\mathcal{S}$ be the set of binding states: $\{ \text{Uninitialized}, \text{Live}, \text{Moved}, \text{PartiallyMoved} \}$.

The Responsibility Model is defined by a state-tracking function $\sigma: \mathcal{B} \to \mathcal{S}$ that evolves as the implementation traverses the Control Flow Graph (CFG).

**Core Axioms:**

1.  **Single Responsibility:** For every object $O$ with a non-static lifetime, there exists exactly one responsible binding $b \in \mathcal{B}$ such that $\sigma(b) = \text{Live}$

2.  **Conservation of Responsibility:** Responsibility is never duplicated; it is either created (allocation), transferred (`move`), or destroyed (`drop`).

3.  **Deterministic Destruction:** When a binding $b$ goes out of scope, if $\sigma(b) = \text{Live}$, the destructor for the bound object **MUST** be invoked.

<u>**Constraints & Legality**</u>

The implementation **MUST** enforce the following constraints on responsible bindings:

1.  **Initialization Requirement:** A responsible binding **MUST** be initialized before use.
2.  **Explicit Transfer:** Responsibility **MUST** be transferred explicitly using the `move` operator. Implicit copying of responsible resources is forbidden.
      * *Exception:* Types implementing the `Copy` trait (§D.1.2) are not subject to move semantics; they are duplicated bitwise.
3.  **Destruction Prohibition:** User code **MUST NOT** explicitly invoke the destructor (defined by the `Drop` trait) of a responsible binding. Destruction is the exclusive province of the compiler.

**Diagnostic Codes:**

| Code | Severity | Description |
| :--- | :--- | :--- |
| `E-MEM-3005` | Error | Explicit call to destructor (`Drop::drop`). |
| `E-MEM-3001` | Error | Use of moved value (accessing a binding in `Moved` or `PartiallyMoved` state). |

<u>**Static Semantics**</u>

The implementation **MUST** track the state of every binding $b$ through the program's control flow.

**Binding States:**

  * **Uninitialized:** The binding is declared but holds no value.
  * **Live:** The binding holds a value and is responsible for it.
  * **Moved:** The value and responsibility have been transferred to another context. The binding is statically invalidated.
  * **PartiallyMoved:** One or more fields of the composite value bound to $b$ have been moved. The binding itself is invalid, but remaining `Live` fields **MAY** be accessed.

**6.3.1 Responsibility Creation and Scope**
Responsibility is established when a binding is initialized via the assignment operator (`=`) or when a parameter declared with `move` enters scope.

**6.3.2 Move Semantics**
The `move` operator transfers responsibility from a source binding $b_{src}$ to a target $b_{dst}$ (or temporary).

**State Transition Rule:**

$$
\frac{
\Gamma \vdash b_{src} : T \quad \sigma(b_{src}) = \text{Live}
}{
\Gamma \vdash \text{move } b_{src} \Rightarrow \Gamma[b_{src} \mapsto \text{Moved}]
}
$$

**Partial Move Semantics:**
For a composite binding $b$ (Record or Tuple):

1.  Moving a field $b.f$ transitions $b$ to $\text{PartiallyMoved}$ and $b.f$ (tracked individually) to $\text{Moved}$.
2.  A binding in the $\text{PartiallyMoved}$ state **MUST NOT** be used as an operand to a full `move` or passed to a procedure.
3.  Remaining fields $b.g$ where $\sigma(b.g) = \text{Live}$ **MAY** be accessed or moved individually.
4.  Partial moves are permitted **ONLY** if $b$ is mutable (`var`) or has `unique` permission. Partial moves from `const` or `partitioned` bindings are **forbidden** to preserve reference integrity.

<u>**Dynamic Semantics**</u>

**Runtime Representation:**

* **Allocation:** Creating a responsible binding allocates storage for the type $T$ on the stack (or register).
* **Move Operation:** At runtime, a `move` operation **MUST** perform a bitwise copy (`memcpy`) of the object's immediate representation (e.g., the pointer, struct fields) from the source location to the destination. It **MUST NOT** perform a deep copy.
* **No Runtime Overhead:** The tracking of binding states ($\sigma$) is purely compile-time. No state flags are emitted into the executable.

**Destruction (Drop):**
When a scope $S$ exits (via normal completion, `return`, `break`, or `result`):

1.  The implementation **MUST** identify all bindings declared in $S$.
2.  Destruction proceeds in strict **Last-In, First-Out (LIFO)** order based on declaration.
3.  For each binding $b$:
* If $\sigma(b) = \text{Live}$, the implementation **MUST** invoke `Drop::drop(b)`.
* If $\sigma(b) = \text{Moved}$, no code is generated.
* If $\sigma(b) = \text{PartiallyMoved}$, the implementation **MUST** invoke `Drop::drop` (or recursive destruction) **ONLY** for the fields of $b$ that are still `Live`.

**Panic Semantics:**
If the program terminates abnormally via a panic (stack unwinding):

1.  The runtime **MUST** attempt to execute destructors for all live responsible bindings in the current stack frame before unwinding to the caller.
2.  **Double Panic:** If a destructor invoked during unwinding itself panics, the runtime **MUST** immediately abort the process (fail-fast) to prevent undefined behavior.

<u>**Memory & Layout**</u>

* **Storage Duration:** Responsible bindings typically have **Automatic** (Stack) storage duration.
* **Backing Store:** A binding in the `Moved` state still occupies stack space until the function returns, but this space is considered logically uninitialized by the language semantics.

<u>**Concurrency & Safety**</u>

* **Transfer Safety:** The `move` operator is the primary mechanism for safe concurrency. Moving a `unique` resource into a closure destined for another thread ($T_{new}$) guarantees that the original thread ($T_{old}$) can no longer access it, preventing data races.
* **Drop Safety:** Destructors triggered by the Responsibility Model **MUST** adhere to the `unique` receiver requirement (`~!`) of the `Drop` trait (§D.1.1). This ensures that when `drop` is called, the object is not aliased by any other thread or reference.

<u>**Verification & Invariants**</u>

The implementation **MUST** enforce the **Dominator Requirement for Moves**:
A binding $b$ is `Live` at a usage point $P$ if and only if:

1.  It was initialized at some point $I$ dominating $P$.
2.  There is no `move` operation on $b$ on any control flow path between $I$ and $P$.

If a `move` occurs on some but not all paths reaching $P$, $b$ is considered **MaybeMoved** and **MUST** be treated as `Moved` (invalid) for safety, unless re-initialized on all paths.

<u>**Examples**</u>

**Example 1: Basic Responsibility Transfer**

```cursive
record Buffer { ptr: Ptr<u8>@Valid, len: usize }

procedure process(move b: Buffer) {
// 'b' is owned by 'process'. It will be dropped at end of scope.
}

procedure main(ctx: Context) {
// 1. Creation: 'buf' becomes the responsible binding.
let buf = Buffer { ... }; 

// 2. Transfer: Responsibility moves to 'process'.
//    At runtime, this is a memcpy of the Buffer record.
process(move buf);

// 3. Invalidation: Usage after move is a compile-time error.
// let size = buf.len; // Error: E-MEM-3001
} 
// 'buf' is in 'Moved' state here; no destructor is called for 'buf'.
```

**Example 2: Partial Moves and Partial Drop**

```cursive
record Player {
name: string@Managed,
stats: Stats, // Assume Copy
inventory: Vec<Item>
}

procedure disassemble(move p: Player) {
// 'p' is Live.

let name = move p.name; 
// 'p' is now PartiallyMoved. 
// 'p.name' is Moved. 'p.inventory' is Live.

// let p2 = move p; // Error: Cannot move partially moved binding.

// At scope exit:
// 1. 'name' is Live -> Drop::drop(name) called.
// 2. 'p.inventory' is Live (inside p) -> Drop::drop(p.inventory) called.
// 3. 'p.name' is Moved -> No drop.
// 4. 'p' itself -> No top-level Drop::drop(p) called because it is broken.
}
```

## 6.4 The Partitioning System

<u>**Definition**</u>

The **Partitioning System** is a static analysis component of the Cursive Abstract Machine responsible for verifying the safety of **aliased mutability**. It governs access to data whose type is qualified by the `partitioned` permission (see 7.2).

The system's primary objective is to guarantee data-race freedom and memory safety when multiple access paths exist to mutable data. Unlike the `unique` permission, which forbids aliasing during mutation, the Partitioning System permits aliasing, provided that it can statically prove that simultaneous mutable accesses target **disjoint** regions of memory.

<u>**Static Semantics**</u>

The Partitioning System activates whenever the type system identifies an access path $P$ derived from a root binding qualified with the `partitioned` permission.

1.  **Default Assumption:** By default, the compiler **MUST** assume that any two access paths derived from the same `partitioned` root may overlap unless proven otherwise.
2.  **Safety Requirement:** The fundamental legality rule enforced is the **Disjoint Access Requirement** (defined below in 6.4.1).

<u>**Dynamic Semantics**</u>

The Partitioning System is purely static. It **MUST NOT** generate runtime checks or impose runtime overhead. The safety of the generated code relies entirely on the static verification performed at compile time.

<u>**Concurrency & Safety**</u>

1.  **Data-Race Freedom:** By enforcing disjointness for all simultaneous mutable accesses, the system guarantees that no two execution contexts (e.g., threads in a `parallel` block) can simultaneously access the same memory location if at least one access is a write, provided the access occurs through safe `partitioned` paths.
2.  **Memory Integrity:** The system ensures that overlapping writes do not corrupt the internal structure or invariants of the data.

### 6.4.1 Disjointness Principles

<u>**Definition**</u>

**Disjointness** is the property that two access paths refer to non-overlapping regions of memory.

Let $Region(P)$ be the set of memory locations accessible through access path $P$. Two access paths $P_1$ and $P_2$ are disjoint if and only if:
$$ Region(P_1) \cap Region(P_2) = \emptyset $$

<u>**Constraints & Legality**</u>

**The Disjoint Access Requirement:**
Given two simultaneously live access paths, $P_1$ and $P_2$, derived from the same `partitioned` root object $O$. The implementation **MUST** verify the safety of their simultaneous use according to the following rules:

1.  **Read/Read Safety:** If both $P_1$ and $P_2$ are used only for reading (i.e., they are coerced to `const`), the access is always safe, regardless of overlap.
2.  **Write Safety (Disjointness Proof):** If at least one path ($P_1$ or $P_2$) is used for writing (i.e., it retains `partitioned` or is upgraded to `unique`), the implementation **MUST** statically prove that $P_1$ and $P_2$ are disjoint.

**Violation:** If the implementation cannot prove disjointness when required by the Write Safety rule, the program is ill-formed.

| Code         | Severity | Description                                                                                                 |
| :----------- | :------- | :---------------------------------------------------------------------------------------------------------- |
| `E-MEM-2931` | Error    | Potential overlapping mutable access detected. The Partitioning System cannot prove disjointness. |

<u>**Static Semantics**</u>

The compiler employs the **Disjointness Judgment**, $\Gamma \vdash Disjoint(P_1, P_2)$, to determine if two paths can be proven disjoint under the current context $\Gamma$. This judgment relies on structural and indexed properties.

**1. Structural Disjointness (Records/Tuples):**
Distinct fields of the same product type instance are inherently disjoint.

**Inference Rule (Structural Disjointness):**

$$
\frac{
\Gamma \vdash R : \text{Record}(T) \quad F_1, F_2 \in \text{fields}(T) \quad F_1 \neq F_2
}{
\Gamma \vdash Disjoint(R.F_1, R.F_2)
}
\tag{P-Disjoint-Struct}
$$  

**Semantics:** 
Accessing `my_record.field_a` and `my_record.field_b` simultaneously is automatically permitted, even if mutable, due to this rule.

**2. Indexed Disjointness (Arrays/Slices):**
Elements of the same sequence are disjoint if and only if their indices are proven to be unequal.

***Inference Rule (Constant Index Disjointness):***

$$
\frac{
\Gamma \vdash A : \text{Sequence}(T) \quad C_1, C_2 : \text{const usize} \quad C_1 \neq C_2
}{
\Gamma \vdash Disjoint(A[C_1], A[C_2])
}
\tag{P-Disjoint-Index-Const}
$$**Inference Rule (Symbolic Index Disjointness):**

$$
\frac{
\Gamma \vdash A : \text{Sequence}(T) \quad i, j : \text{usize} \quad (\Gamma \implies i \neq j)
}{
\Gamma \vdash Disjoint(A[i], A[j])
}
\tag{P-Disjoint-Index-Symbolic}
$$  

<u>**Semantics:**</u> 
The judgment relies on the implication $(\Gamma \implies i \neq j)$. If the indices are runtime variables, the compiler generally cannot derive this implication automatically.

**3. Proof Injection (The `partition` statement):**
The `partition` statement (Syntax defined in 10.6) is the mechanism by which the programmer explicitly injects the required proof of disjointness when automatic derivation fails. The validity of this proof is checked by the Partition Proof Verifier (6.4.2).

<u>**Verification & Invariants**</u>

The core invariant maintained by the system guarantees the safety of aliased mutation across the entire program.

**Formal Invariant (Safety of Partitioned Access):**

Let $\mathcal{P}(O)$ be the set of all live access paths derived from a `partitioned` object $O$. Let $Mutable(P)$ be true if access path $P$ is used for writing.

$$\forall O, \forall P_1, P_2 \in \mathcal{P}(O), (P_1 \neq P_2 \land (Mutable(P_1) \lor Mutable(P_2))) \implies (Region(P_1) \cap Region(P_2) = \emptyset)
$$This invariant **MUST** hold true at all points during program execution for safe code.

### 6.4.2 The Partition Proof Verifier

<u>**Definition**</u>

The **Partition Proof Verifier** (PPV) is the specialized subsystem of the compiler responsible for validating the logical predicates asserted in the `where` clause of `partition` statements. It is a constraint solver operating over a restricted, decidable abstract domain.

<u>**Static Semantics**</u>

The PPV operates during the static analysis phase.

1.  **Objective:** The objective of the PPV is to determine the validity of the predicate $P$ provided in the `where` clause, given the current compilation context $\Gamma$. Validity means $P$ is a tautology (always true) within that context.
    $$ Verify(\Gamma, P) \iff (\Gamma \implies P) $$
2.  **Properties:**
      * **Soundness (Mandatory):** The PPV **MUST** be sound. It must not accept invalid proofs.
      * **Decidability (Mandatory):** The logic implemented by the PPV **MUST** be decidable. This ensures compilation always terminates.
      * **Incompleteness (Permitted):** The PPV is not required to be complete. It may fail to prove a true statement.

**The Verifiable Expression Subset (Abstract Domain):**
To ensure decidability and soundness, the expressions allowed within a `partition` statement's `where` clause are strictly restricted to the **Verifiable Expression Subset**.

The predicate $P$ **MUST** be composed exclusively of expressions within this subset:

1.  **Supported Operations:**

      * Arithmetic comparison (`!=`, `<`, `<=`, `>`, `>=`) on primitive integer types (specifically `isize`, `usize`).
      * Logical conjunction (`&&`).
      * Parentheses for grouping.

2.  **Supported Operands:**

      * The index variables introduced by the `partition` statement's `by` clause.
      * Integer literals.
      * References to `const` bindings visible in the scope, provided their values are compile-time constants.
      * Simple arithmetic (`+`, `-`) involving only the above operands, provided the result remains within the domain of Linear Integer Arithmetic.

3.  **Exclusions:**
    The following constructs **MUST NOT** appear in a verifiable expression: Procedure calls, mutation, side effects, loops, recursion, floating-point arithmetic, logical disjunction (`||`), or negation (`!`).

<u>**Constraints & Legality**</u>

If the PPV cannot establish the validity of the predicate $P$, or if $P$ contains constructs outside the Verifiable Expression Subset, the implementation **MUST** reject the `partition` statement.

| Code         | Severity | Description                                                                                           |
| :----------- | :------- | :---------------------------------------------------------------------------------------------------- |
| `E-MEM-2941` | Error    | Partition proof failed. The Partition Proof Verifier cannot validate the `where` clause predicate. |
| `E-MEM-2942` | Error    | Expression in `partition where` clause is outside the Verifiable Expression Subset.                |

> [!NOTE] Implementation Guidance
> Conforming implementations are encouraged to implement the PPV using techniques such as Abstract Interpretation or by leveraging an SMT solver configured for the Theory of Linear Integer Arithmetic (LIA), which aligns with the defined Verifiable Expression Subset.