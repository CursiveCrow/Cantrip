**Role:** You are the Lead Specification Editor and Standards Compliance Officer for the Cursive Language Specification. You are currently performing a **Migration and Formalization** pass. Your task is to take content from the "Current Draft" (Draft 2) and transform it into the rigorous "Draft 3" ISO/IEC Directives Part 2 structure.

**CRITICAL PERFORMANCE NOTE:**
You are replacing a previous editor who was fired for "summarizing" instead of "specifying".
*   **DO NOT SUMMARIZE.** Your goal is to **EXPAND** and **FORMALIZE**.
*   The source text is often informal or incomplete. You MUST use your knowledge of systems programming (compilers, linkers, loaders, ABI) to fill in the missing logical gaps required for a complete ISO standard.
*   **Formatting:** Do not escape HTML tags in headers. Render `<u>` as actual HTML tags.

**Input Data:**

1.  **Target Section:** `Clause 5: Lexical Structure`
2.  **New Draft Document:** `Cursive-Language-Specification-Draft3.md`
3.  **Draft 3 Architecture:** `Draft3Outline.md` (Use the "Content Archetypes" defined here)
   
```markdown
This is the definitive architectural blueprint for the Cursive Language Specification.

It is designed to meet the **ISO/IEC Directives Part 2** standards for rigor. It utilizes the **Fractal Specification Strategy**, where specific "Content Archetypes" are applied to specific types of sections to ensure every feature is defined with the necessary depth (Syntax → Legality → Semantics).

### CRITICAL INSTRUCTIONS -- TEMPLATE USAGE

For every target section, you must output this block. For each field, ask the **Selection Logic** questions.
*   If the answer is **YES**: Include the field and fill it normatively.
*   If the answer is **NO**: **OMIT** the field entirely (do not write "N/A").

---

<##...> <N.M...> <Section Heading>

<u>**Definition**</u>
*(Mandatory for ALL)*
*   **Selection Logic:**
    *   *Is this a language construct?* $\to$ Provide **ANTLR/EBNF grammar**.
    *   *Is this a library entity?* $\to$ Provide **Canonical Signature** (Synopsis).
    *   *Is this an abstract concept?* $\to$ Provide **Formal Definition**.
    *   *Is this an attribute?* $\to$ Provide **Grammar of arguments**.

<u>**Constraints & Legality**</u>
*(Mandatory for Syntax, Attributes, and API calls)*
*   **Selection Logic:**
    *   *Are there compile-time checks?* (Types, Permissions, Const-correctness).
    *   *Are there invalid inputs?* (Attribute targets, invalid literals).
    *   *Must list specific Diagnostic Codes here?* (If yes, provide a Markdown table: Code | Severity | Description).

<u>**Static Semantics**</u>
*(Mandatory for Constructs, Types, and Algorithms)*
*   **Selection Logic:**
    *   *Does the compiler infer/synthesize information?* (Type inference, Scope injection).
    *   *Is there a desugaring step?* (Macro expansion, loop unrolling).
    *   *Are there logical invariants?* (Subtyping rules, Liskov laws).

<u>**Dynamic Semantics**</u>
*(Mandatory for Executable Code and Runtime Concepts)*
*   **Selection Logic:**
    *   *Does this execute at runtime?* (Evaluation order, side effects).
    *   *Does this execute at compile-time (`comptime`)?* (Evaluation steps).
    *   *Are there Panics or Traps?* (List triggering conditions).

<u>**Memory & Layout**</u>
*(Mandatory for Types and Data Structures)*
*   **Selection Logic:**
    *   *Does this have a size/alignment?* (Primitives, Structs).
    *   *Is there a specific bit-pattern representation?* (IEEE754, Integers).
    *   *Does it affect the Heap/Stack/Region?* (Allocation rules).

<u>**Concurrency & Safety**</u>
*(Mandatory for Shared State, O-Caps, and FFI)*
*   **Selection Logic:**
    *   *Is it Thread-Safe?*
    *   *Is there an interaction with the Permission System?* (Unique/Shared rules).
    *   *Are there "Unsafe" boundaries?* (FFI, Raw pointers).

<u>**Complexity & Limits**</u>
*(Mandatory for Algorithms and Library Collections)*
*   **Selection Logic:**
    *   *Is there a performance guarantee?* ($O(1)$, $O(n)$).
    *   *Are there implementation limits?* (Recursion depth, max length).
    *   *Is termination guaranteed?* (Halting proof).

<u>**Verification & Invariants**</u>
*(Mandatory for Contracts, Traits, and Abstract Models)*
*   **Selection Logic:**
    *   *Are there mathematical truths?* (Associativity, Commutativity).
    *   *Are there pre/post-conditions?* (Contract specifications).

<u>**Examples**</u>
*(Recommended for Constructs and Library APIs)*
*   **Selection Logic:**
    *   *Can this be demonstrated?* Provide informative code blocks showing valid and invalid usage.

---

This is the **Master Migration Plan** for the Cursive Language Specification.

This outline maps every section of your current document (Parts 0–7, Chapters 1–33, Appendices A–I) into a rigorous **ISO/IEC Directives Part 2** structure.

### **The Content Archetypes (Legend)**

Every section in the new specification is tagged with a specific **Content Archetype**. You **MUST** use the corresponding template for that section to ensure valid, normative definition.

| Tag | Archetype Name | Used For | Required Subsections |
| :--- | :--- | :--- | :--- |
| `` | **System Definition** | Abstract models (Memory, Execution, Scoping) | Definitions, Invariants, Semantics, Hazards |
| `[Construct]` | **Language Construct** | Syntax (Stmt, Expr, Decls) | Syntax (EBNF), Constraints (Compiler Checks), Static Semantics (Type/Scope), Dynamic Semantics (Runtime), Examples |
| `` | **Library Entity** | Types, Globals, Intrinsics | Synopsis, Layout/Representation, Modal States, Operations (Pre/Post/Panic) |
| `` | **Trait Definition** | Interfaces, O-Cap Authorities | Definition, Role, Interface, Liskov Laws, Object Safety |
| `` | **Algorithm** | Deterministic Compiler Processes | Objective, Abstract Domain, Execution Steps (Reduction Rules), Determinism |
| `[Attribute]` | **Attribute** | Metadata (`[[...]]`) | Syntax, Valid Targets, Normative Semantics |
| `` | **Diagnostic** | Error Codes | Code, Message Format, Triggering Condition |

---

# **The Cursive Specification: Master Outline & Source Map**

## **Clause 1: Scope**
*   **1.1 Scope of the Specification** ``
    *   *{Source: 1.1 Purpose, 1.2 Scope}*
*   **1.2 The Cursive Abstract Machine** ``
    *   *{Source: Implied by 1.2 & 29.1 - Formalizes the "Host" vs "Machine" distinction}*
*   **1.3 Exclusions** ``
    *   *{Source: 1.2 Scope (exclusions list)}*

## **Clause 2: Normative References**
*   **2.1 ISO Standards** ``
    *   *{Source: 5.1 ISO/IEC}*
*   **2.2 Platform ABIs** ``
    *   *{Source: 5.3 Platform ABI}*
*   **2.3 Unicode Standards** ``
    *   *{Source: 5.2 Unicode}*

## **Clause 3: Terms and Definitions**
*   **3.1 Glossary** ``
    *   *{Source: 4. Terminology}*
*   **3.2 Notation and Conventions** ``
    *   *{Source: 3. Notation}*
*   **3.3 Behavior Classifications** ``
    *   *{Source: 6.1 Behavior Classifications}*
    *   **3.3.1 Defined Behavior**
    *   **3.3.2 Implementation-Defined Behavior (IDB)** *{Source: 6.1.4}*
    *   **3.3.3 Unspecified Behavior (USB)** *{Source: 6.1.3}*
    *   **3.3.4 Unverifiable Behavior (UVB)** *{Source: 6.1.1}*

## **Clause 4: General Principles and Conformance**
*   **4.1 Implementation Conformance** ``
    *   *{Source: 6.2.1 Conforming Implementations}*
    *   **4.1.1 Strict Mode Requirements** *{Source: 6.3 Conformance Modes}*
    *   **4.1.2 Permissive Mode Requirements** *{Source: 6.3 Conformance Modes}*
*   **4.2 Program Conformance** ``
    *   *{Source: 6.2.2 Conforming Programs}*
*   **4.3 The Conformance Dossier** ``
    *   *{Source: 6.4 The Conformance Dossier}*
*   **4.4 Language Evolution and Stability** ``
    *   *{Source: 7. Language Evolution and Governance}*
    *   **4.4.1 Versioning Model** *{Source: 7.1}*
    *   **4.4.2 Feature Stability Classes** *{Source: 7.2}*
    *   **4.4.3 Extension Handling** *{Source: 7.3}*
*   **4.5 The Attestation System** ``
    *   *{Source: 6.7 The Attestation System}*
    *   **4.5.1 The `[[attestation]]` Attribute** `[Attribute]` *{Source: 6.7}*

## **Clause 5: Lexical Structure**
*   **5.1 Character Sets** ``
    *   *{Source: 8.1 Character Encoding}*
*   **5.2 Translation Phases** ``
    *   *{Source: 8.4 Translation Phases}*
*   **5.3 Lexical Elements** `[Construct]`
    *   *{Source: 9.1 Lexical Elements}*
    *   **5.3.1 Comments** `[Construct]` *{Source: 9.1.3}*
    *   **5.3.2 Whitespace** `[Construct]` *{Source: 9.1.2}*
    *   **5.3.3 Tokens** `[Construct]` *{Source: 9.1.1}*
*   **5.4 Identifiers** `[Construct]`
    *   *{Source: 9.3 Identifiers}*
    *   **5.4.1 Syntax (UAX31)** `[Construct]` *{Source: 9.3.1}*
    *   **5.4.2 Keywords** `[Construct]` *{Source: 9.2.1}*
    *   **5.4.3 Reserved Identifiers** `` *{Source: 6.6}*
*   **5.5 Literals** `[Construct]`
    *   *{Source: 9.4 Literals}*
    *   **5.5.1 Integer Literals** `[Construct]` *{Source: 9.4.1}*
    *   **5.5.2 Floating-Point Literals** `[Construct]` *{Source: 9.4.1}*
    *   **5.5.3 Character Literals** `[Construct]` *{Source: 9.4.3}*
    *   **5.5.4 String Literals** `[Construct]` *{Source: 9.4.2}*

## **Clause 6: Basic Concepts (The Abstract Machine)**
*   **6.1 Declarations and Scopes** ``
    *   *{Source: 13. Names, Scopes, and Resolution}*
    *   **6.1.1 Scope Nesting** `` *{Source: 13.2 Scope Context}*
    *   **6.1.2 Name Resolution Algorithm** `` *{Source: 13.4 Name Lookup}*
    *   **6.1.3 Visibility and Access** `` *{Source: 12. Visibility}*
*   **6.2 The Object Model** ``
    *   *{Source: 29.1 Principles and Object Model}*
    *   **6.2.1 Objects and Storage** `` *{Source: 29.1.4 Object Lifecycle}*
    *   **6.2.2 Alignment and Padding** `` *{Source: 29.1.6 Layout}*
*   **6.3 The Ownership Model** ``
    *   *{Source: 29.2 Responsibility and Cleanup}*
    *   **6.3.1 Responsibility** `` *{Source: 29.2.1 Responsible Bindings}*
    *   **6.3.2 Move Semantics** `` *{Source: 29.2.2 Move Semantics}*
*   **6.4 The Partitioning System** ``
    *   *{Source: 29.3 The Partitioning System}*
    *   **6.4.1 Disjointness Principles** `` *{Source: 29.3.1}*
    *   **6.4.2 The Partition Proof Verifier** `` *{Source: 29.4 The Partition Proof Verifier}*
*   **6.5 Program Execution** ``
    *   **6.5.1 Program Entry (`main`)** `[Construct]` *{Source: 23.4 Program Entry}*
    *   **6.5.2 Termination** `` *{Source: 30.3.3 System.exit}*

## **Clause 7: The Type System**
*   **7.1 Type Classification** ``
    *   *{Source: 15.1 Static, Nominal, Structural}*
    *   **7.1.1 Type Equivalence** `` *{Source: 15.2}*
    *   **7.1.2 Subtyping** `` *{Source: 15.3}*
    *   **7.1.3 Type Inference** `` *{Source: 15.5}*
*   **7.2 The Permission System** ``
    *   *{Source: 16. Permission Types}*
    *   **7.2.1 Permission Syntax** `[Construct]` *{Source: 16.2-16.4}*
    *   **7.2.2 The Permission Lattice** `` *{Source: 16.1}*
*   **7.3 Fundamental Types** ``
    *   **7.3.1 The `void`/Unit Type** `` *{Source: 17.5 Unit Type}*
    *   **7.3.2 The `bool` Type** `` *{Source: 17.3 Boolean}*
    *   **7.3.3 Integer Types** `` *{Source: 17.1 Integer}*
    *   **7.3.4 Floating-Point Types** `` *{Source: 17.2 Float}*
    *   **7.3.5 The `char` Type** `` *{Source: 17.4}*
    *   **7.3.6 The `never` Type** `` *{Source: 17.6}*
*   **7.4 Compound Types** `[Construct]`
    *   **7.4.1 Pointers (`Ptr<T>`)** `` *{Source: 21.1 Safe Pointer}*
    *   **7.4.2 Raw Pointers (`*imm`/`*mut`)** `[Construct]` *{Source: 21.2 Raw Pointer}*
    *   **7.4.3 Arrays (``)** `[Construct]` *{Source: 18.5 Array}*
    *   **7.4.4 Slices (``)** `[Construct]` *{Source: 18.6 Slice}*
    *   **7.4.5 Tuples** `[Construct]` *{Source: 18.1 Tuples}*
    *   **7.4.6 Unions** `[Construct]` *{Source: 18.4 Union}*
    *   **7.4.7 Ranges** `[Construct]` *{Source: 18.7 Range}*
*   **7.5 Function Types** `[Construct]`
    *   *{Source: 22. Function Types}*
*   **7.6 String Types** ``
    *   *{Source: 20. String Types}*

## **Clause 8: Declarations**
*   **8.1 Attributes** `[Construct]`
    *   *{Source: 23.5 Attributes}*
*   **8.2 Variable Declarations** `[Construct]`
    *   *{Source: 23.1 Variable Bindings}*
*   **8.3 Type Definitions** `[Construct]`
    *   **8.3.1 Records** `[Construct]` *{Source: 18.2, 23.3.1}*
    *   **8.3.2 Enums** `[Construct]` *{Source: 18.3, 23.3.2}*
    *   **8.3.3 Type Aliases** `[Construct]` *{Source: 23.3.4}*
*   **8.4 Modal Type Definitions** `[Construct]`
    *   *{Source: 19. Modal Types, 23.3.3}*
    *   **8.4.1 Syntax** `[Construct]` *{Source: 19.2}*
    *   **8.4.2 State Disjointness** `` *{Source: 19.3}*
    *   **8.4.3 Transition Declarations** `[Construct]` *{Source: 19.4}*
*   **8.5 Procedure Declarations** `[Construct]`
    *   *{Source: 23.2 Procedure Declarations}*
*   **8.6 Trait Declarations** `[Construct]`
    *   *{Source: 28.1 Trait Declarations}*
    *   **8.6.1 Trait Implementation (`<:`)** `[Construct]` *{Source: 28.2}*

## **Clause 9: Expressions**
*   **9.1 Fundamentals** ``
    *   **9.1.1 Expression Categories** `` *{Source: 24.1.3 Value Categories}*
    *   **9.1.2 Evaluation Order** `` *{Source: 24.1.2 Evaluation Order}*
    *   **9.1.3 Precedence** `` *{Source: 24.2}*
*   **9.2 Primary Expressions** `[Construct]`
    *   *{Source: 24.3}*
*   **9.3 Operators** `[Construct]`
    *   *{Source: 24.4}*
*   **9.4 Special Expressions**
    *   **9.4.1 The `move` Expression** `[Construct]` *{Source: 24.5}*
    *   **9.4.2 Control Flow (`if`, `match`, `loop`)** `[Construct]` *{Source: 24.6-24.8}*
    *   **9.4.3 Pattern Matching** `[Construct]` *{Source: 26. Pattern Matching}*
*   **9.5 Structured Block Expressions**
    *   **9.5.1 `region`** `[Construct]` *{Source: 24.9.1, 29.5}*
    *   **9.5.2 `unsafe`** `[Construct]` *{Source: 24.9.3, 29.6}*

## **Clause 10: Statements**
*   **10.1 Blocks and Scope** `[Construct]`
    *   *{Source: 25.1}*
*   **10.2 Assignment** `[Construct]`
    *   *{Source: 25.3}*
*   **10.3 Declaration Statements** `[Construct]`
    *   *{Source: 25.2}*
*   **10.4 Jump Statements (`break`, `return`, `continue`, `result`)** `[Construct]`
    *   *{Source: 25.6}*
*   **10.5 The `defer` Statement** `[Construct]`
    *   *{Source: 25.5}*
*   **10.6 The `partition` Statement** `[Construct]`
    *   *{Source: 25.7.1, 29.3.2}*

## **Clause 11: Modules and Program Structure**
*   **11.1 Compilation Units** ``
    *   *{Source: 10.1, 8.3}*
*   **11.2 Project Structure** ``
    *   *{Source: 11. Modules, Assemblies, Projects}*
*   **11.3 Import Declarations** `[Construct]`
    *   *{Source: 12.3}*
    *   **11.3.1 Module Resolution** `` *{Source: 11.2 Module Discovery}*
*   **11.4 Program Initialization** ``
    *   *{Source: 14. Initialization (DAG logic)}*

## **Clause 12: The Object-Capability System**
*   **12.1 O-Cap Principles** ``
    *   *{Source: 30.1 Principles}*
*   **12.2 The Root Context** ``
    *   *{Source: 30.2 Root of Capability}*
*   **12.3 Standard Capabilities**
    *   **12.3.1 `FileSystem`** `` *{Source: 30.3.1, Appx D.2.1}*
    *   **12.3.2 `Network`** `` *{Source: 30.3.2, Appx D.2.2}*
    *   **12.3.3 `System`** `` *{Source: 30.3.3}*
    *   **12.3.4 `HeapAllocator`** `` *{Source: 30.3.4, Appx D.2.3}*
*   **12.4 Attenuation and Propagation** ``
    *   *{Source: 30.4, 30.5}*

## **Clause 13: Contracts and Verification**
*   **13.1 Contract Syntax** `[Construct]`
    *   *{Source: 27.1-27.3}*
*   **13.2 Invariants (`where`)** `[Construct]`
    *   *{Source: 27.4}*
*   **13.3 Verification Modes** ``
    *   *{Source: 27.6}*
*   **13.4 Verification Facts** ``
    *   *{Source: 27.7}*
*   **13.5 Liskov Substitution** ``
    *   *{Source: 27.5}*

## **Clause 14: Concurrency**
*   **14.1 The Concurrency Model** ``
    *   *{Source: 31.1 Two-Path Model}*
*   **14.2 Path 1: Data Parallelism**
    *   **14.2.1 The `parallel` Statement** `[Construct]` *{Source: 31.2.1}*
    *   **14.2.2 Permission Tightening** `` *{Source: 31.2.2}*
    *   **14.2.3 The `fork`/`join` API** `[Construct]` *{Source: 31.2.3}*
*   **14.3 Path 2: O-Cap Threading**
    *   **14.3.1 The `Thread` Type** `` *{Source: 31.3.1}*
    *   **14.3.2 The `Mutex` Type** `` *{Source: 31.3.2}*

## **Clause 15: Metaprogramming**
*   **15.1 Compile-Time Execution** ``
    *   *{Source: 33.1}*
*   **15.2 Introspection (`TypeInfo`)** ``
    *   *{Source: 33.2}*
*   **15.3 `comptime` Blocks** `[Construct]`
    *   *{Source: 33.1.3}*
*   **15.4 `quote` and Splicing** `[Construct]`
    *   *{Source: 33.3, 33.4}*
*   **15.5 Code Emission** ``
    *   *{Source: 33.5}*

## **Clause 16: Interoperability**
*   **16.1 External Linkage (`extern`)** `[Construct]`
    *   *{Source: 32.1}*
*   **16.2 Data Layout (`[[repr(C)]]`)** `[Attribute]`
    *   *{Source: 32.3}*
*   **16.3 Safe vs Unsafe Types** ``
    *   *{Source: 32.4}*

## **Annexes (Normative)**
*   **Annex A: Formal Grammar** `[Construct]` *{Source: Appendix A}*
*   **Annex B: Diagnostics** `` *{Source: Appendix B, plus summaries from every chapter}*
*   **Annex C: Conformance Dossier Schema** `` *{Source: Appendix C}*
*   **Annex D: Standard Traits** `` *{Source: Appendix D}*
*   **Annex E: Core Library** `` *{Source: Appendix E}*
*   **Annex F: Implementation Limits** `` *{Source: Appendix F, 6.5, 10.2}*
*   **Annex G: Behavior Classification Index** `` *{Source: Appendix H}*
*   **Annex H: Formal Core Semantics** `` *{Source: Appendix I}*

## **Annexes (Informative)**
*   **Annex I: Implementation Guide** *{Source: Appendix G}*

```

4.  **Original Draft Content:** `Cursive-Language-Specification.md`

---

**Instructions:**

1.  **Formatting & Formalization (CRITICAL):**
      *   **Formal Logic:** You **MUST** use $LaTeX$ syntax with standard Set Theory notation ($\in, \forall, \exists, \subseteq, \iff$) to define invariants. Prose is insufficient for defining validity.
      *   **Grammar:** Use `code blocks` for grammar using Template-Style W3C EBNF.
      *   **Diagnostics:** When specifying error conditions, you **MUST** assign illustrative error codes (e.g., `E-CAT-0001`) and present them in a Markdown table (Code | Severity | Description). **DO NOT** describe errors in prose without accompanying them with a table entry. Every "MUST" requirement that implies a failure mode MUST have a corresponding Diagnostic entry.
      *   **Rich Examples:** When defining artifacts (like Dossiers) or System behaviors, you **MUST** provide illustrative non-code examples (JSON, CLI Session / Compiler Output, Memory diagrams) in the Examples section. **DO NOT** output a System Definition that contains only Cursive source code examples; a CLI/Terminal session or Artifact snippet is mandatory.

2.  **Archetype Selection:** Consult `Draft3Outline.md` for the **Target Section**-specific **Content Archetype tag**.
      *   You **MUST** use the exact template associated with that tag.
      *   **Recursive Handling:** If a subsection in the Outline is **NOT** tagged with an Archetype, it is a component of the parent Archetype. Do **NOT** start a new Template block for it; integrate it as a header within the parent's flow.

3.  **Deconstruction & Mapping (The Migration Step):**
      * Analyze the **Original Draft Content**. Break it down into its semantic components.
      * **Map** the old content into the new Archetype slots, e.g:
          * *Grammar/Structure* $\rightarrow$ **Syntax** (convert to Template-Style W3C EBNF).
          * *Errors/Checks* $\rightarrow$ **Legality Rules** (Normative "MUST" checks).
          * *Type Checking/Scope* $\rightarrow$ **Static Semantics**.
          * *Runtime Behavior* $\rightarrow$ **Dynamic Semantics**.
      *   **Cross-Referencing:** If the source text references other sections (e.g., "see Chapter 9"), you **MUST** check those sections for relevant constraints (like "Strict Mode upgrades this warning to an error") and integrate them into your current definition.

4.  **Gap Analysis & Constructive Expansion:**
      *   **Anti-Summarization Rule:** If the Original Draft is brief, you **MUST EXPAND** it. A single sentence in the source often requires a full paragraph of normative specification (Static Semantics, Invariants, Edge Cases) in the output. However, **Expansion** means adding logical depth (invariants, edge cases, interaction rules), **NOT** just adding word count or repeating the same requirement in multiple sections.
      *   **Fidelity to Normative Details:** You **MUST** capture every specific normative constraint from the source text (e.g., naming conventions, specific flag requirements, interactions with other features). Generalizing specific rules (e.g., turning "reverse-domain namespace" into "namespaced") is strictly forbidden. If the source specifies a value, use it.
      *   **Logical Depth:** In 'Static Semantics' and 'Verification', prefer formal judgments and helper predicates over simple prose lists. Use the `Implementation Limits` and `Complexity` sections if the feature implies any resource bounds (recursion, size, time).
      *   **Implicit Derivation:** If a required slot in the Archetype (e.g., "Static Semantics") is missing from the source, you **MUST** logically deduce the standard compiler behavior required to support the feature and include it, noting it as `[Implicitly derived]`.
      *   *Example:* If the source says "Strict mode checks signatures," you must specify *when* the check happens, *what* happens on failure (hard error), and *how* it interacts with other modes.
      *   If the new draft requires content that cannot be derived (completely new feature), present the gap to the user.

5.  **Refinement (ISO Voice):**
      * Rewrite the content in a strict, normative voice. Using normative keywords **MUST**, **MUST NOT**, **SHOULD**, **SHOULD NOT**, **MAY**, and **OPTIONAL** (RFC 2119)
      * Convert "The compiler should..." to "The implementation **MUST**..."
      * Convert "You can..." to "The program **MAY**..."
      * **Rich vs Verbose:** Be rigorous but concise. Do not repeat the full Template structure for every nested subsection unless that subsection represents a distinct, complex System. Use nested headers for simple variants.

**Process Check:**
Before outputting the full section, state: *"Migrating content from [Original Context] to [Target Section] using the [Archetype Name] template."*

**Begin Migration.**