This is the definitive architectural blueprint for the Cursive Language Specification.

It is designed to meet the **ISO/IEC Directives Part 2** standards for rigor. It utilizes the **Fractal Specification Strategy**, where specific "Content Archetypes" are applied to specific types of sections to ensure every feature is defined with the necessary depth (Syntax → Legality → Semantics).

### Content Archetypes

You must apply the specific template below based on the tag assigned to each section in the outline.

#### 1. `[Construct]` (Syntactic Feature)
*Used for: Statements, Expressions, Declarations (e.g., `if`, `loop`, `record`).*
1.  **Syntax:** Formal ANTLR/EBNF grammar production.
2.  **Legality Rules:** Compile-time constraints (types, permissions) that trigger errors if violated.
3.  **Static Semantics:** Compile-time properties (type synthesis, scope injection, control-flow graph generation).
4.  **Dynamic Semantics:** Runtime behavior (evaluation order, side effects, memory operations).
5.  **Examples:** (Informative) Valid/Invalid usage.

#### 2. `` (Abstract Mechanism)
*Used for: Cross-cutting logics (e.g., Memory Model, Scoping, Conformance).*
1.  **Definition:** Formal definition of terms and abstract entities (e.g., "Object", "Region").
2.  **Invariants:** Logical truths that must always hold (e.g., "No two unique pointers alias").
3.  **Operational Rules:** The logic governing the system (e.g., "How permission lowering works").
4.  **Hazards:** (Optional) Undefined or Unverifiable behaviors.

#### 3. `` (Library Entity)
*Used for: Built-in types and intrinsics (e.g., `string`, `Context`).*
1.  **Synopsis:** Canonical declaration (signature, attributes).
2.  **Description:** High-level intent.
3.  **Layout:** (Normative) Memory representation guarantees.
4.  **States:** (Modal types only) Table of `@State` and payloads.
5.  **Operations:** Methods with Pre/Post-conditions and Panics.

#### 4. `` (Traits)
*Used for: Traits and O-Caps (e.g., `Drop`, `FileSystem`).*
1.  **Definition:** Canonical `trait` declaration.
2.  **Role:** Semantic purpose.
3.  **Procedures:** Abstract/Concrete methods (using *LibSpec* style).
4.  **Laws:** Invariants implementations must obey (Liskov Substitution).
5.  **Object Safety:** Safe/Unsafe classification for `object Trait` use.

#### 5. `` (Deterministic Process)
*Used for: Compiler algorithms (e.g., Type Inference, Resolution).*
1.  **Objective:** Input/Output definition ($f(E) \to T$).
2.  **Abstract Domain:** Symbols and contexts ($\Gamma$) used.
3.  **Execution Steps:** Step-by-step reduction rules.
4.  **Determinism:** Conflict resolution rules.

#### 6. `` (Attribute)
*Used for: Metadata (e.g., `[[repr]]`, `[[verify]]`).*
1.  **Syntax:** Grammar of arguments.
2.  **Targets:** Where it can be placed.
3.  **Semantics:** Normative effect on compilation.

#### 7. `` (Diagnostic)
*Used for: Appendix entries.*
1.  **Code:** Stable ID (e.g., `E-TYP-1042`).
2.  **Message:** Canonical format string.
3.  **Cause:** Triggering condition.
This is the **Master Migration Plan** for the Cursive Language Specification.

This outline maps every section of your current document (Parts 0–7, Chapters 1–33, Appendices A–I) into a rigorous **ISO/IEC Directives Part 2** structure.

### **The Content Archetypes (Legend)**

Every section in the new specification is tagged with a specific **Content Archetype**. You **MUST** use the corresponding template for that section to ensure valid, normative definition.

| Tag | Archetype Name | Used For | Required Subsections |
| :--- | :--- | :--- | :--- |
| `` | **System Definition** | Abstract models (Memory, Execution, Scoping) | Definitions, Invariants, Operational Rules, Hazards |
| `[Construct]` | **Language Construct** | Syntax (Stmt, Expr, Decls) | Syntax (EBNF), Legality Rules (Compiler Checks), Static Semantics (Type/Scope), Dynamic Semantics (Runtime), Examples |
| `` | **Library Entity** | Types, Globals, Intrinsics | Synopsis, Layout/Representation, Modal States, Operations (Pre/Post/Panic) |
| `` | **Trait Definition** | Interfaces, O-Cap Authorities | Definition, Role, Interface, Liskov Laws, Object Safety |
| `` | **Algorithm** | Deterministic Compiler Processes | Objective, Abstract Domain, Execution Steps (Reduction Rules), Determinism |
| `[Attribute]` | **Attribute** | Metadata (`[[...]]`) | Syntax, Valid Targets, Normative Semantics |
| `` | **Diagnostic** | Error Codes | Code, Message Format, Triggering Condition |

---

### **The Cursive Specification: Master Outline & Source Map**

#### **Clause 1: Scope**
*   **1.1 Scope of the Specification** ``
    *   *{Source: 1.1 Purpose, 1.2 Scope}*
*   **1.2 The Cursive Abstract Machine** ``
    *   *{Source: Implied by 1.2 & 29.1 - Formalizes the "Host" vs "Machine" distinction}*
*   **1.3 Exclusions** ``
    *   *{Source: 1.2 Scope (exclusions list)}*

#### **Clause 2: Normative References**
*   **2.1 ISO Standards** ``
    *   *{Source: 5.1 ISO/IEC}*
*   **2.2 Platform ABIs** ``
    *   *{Source: 5.3 Platform ABI}*
*   **2.3 Unicode Standards** ``
    *   *{Source: 5.2 Unicode}*

#### **Clause 3: Terms and Definitions**
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

#### **Clause 4: General Principles and Conformance**
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

#### **Clause 5: Lexical Structure**
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

#### **Clause 6: Basic Concepts (The Abstract Machine)**
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

#### **Clause 7: The Type System**
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

#### **Clause 8: Declarations**
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

#### **Clause 9: Expressions**
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

#### **Clause 10: Statements**
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

#### **Clause 11: Modules and Program Structure**
*   **11.1 Compilation Units** ``
    *   *{Source: 10.1, 8.3}*
*   **11.2 Project Structure** ``
    *   *{Source: 11. Modules, Assemblies, Projects}*
*   **11.3 Import Declarations** `[Construct]`
    *   *{Source: 12.3}*
    *   **11.3.1 Module Resolution** `` *{Source: 11.2 Module Discovery}*
*   **11.4 Program Initialization** ``
    *   *{Source: 14. Initialization (DAG logic)}*

#### **Clause 12: The Object-Capability System**
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

#### **Clause 13: Contracts and Verification**
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

#### **Clause 14: Concurrency**
*   **14.1 The Concurrency Model** ``
    *   *{Source: 31.1 Two-Path Model}*
*   **14.2 Path 1: Data Parallelism**
    *   **14.2.1 The `parallel` Statement** `[Construct]` *{Source: 31.2.1}*
    *   **14.2.2 Permission Tightening** `` *{Source: 31.2.2}*
    *   **14.2.3 The `fork`/`join` API** `[Construct]` *{Source: 31.2.3}*
*   **14.3 Path 2: O-Cap Threading**
    *   **14.3.1 The `Thread` Type** `` *{Source: 31.3.1}*
    *   **14.3.2 The `Mutex` Type** `` *{Source: 31.3.2}*

#### **Clause 15: Metaprogramming**
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

#### **Clause 16: Interoperability**
*   **16.1 External Linkage (`extern`)** `[Construct]`
    *   *{Source: 32.1}*
*   **16.2 Data Layout (`[[repr(C)]]`)** `[Attribute]`
    *   *{Source: 32.3}*
*   **16.3 Safe vs Unsafe Types** ``
    *   *{Source: 32.4}*

#### **Annexes (Normative)**
*   **Annex A: Formal Grammar** `[Construct]` *{Source: Appendix A}*
*   **Annex B: Diagnostics** `` *{Source: Appendix B, plus summaries from every chapter}*
*   **Annex C: Conformance Dossier Schema** `` *{Source: Appendix C}*
*   **Annex D: Standard Traits** `` *{Source: Appendix D}*
*   **Annex E: Core Library** `` *{Source: Appendix E}*
*   **Annex F: Implementation Limits** `` *{Source: Appendix F, 6.5, 10.2}*
*   **Annex G: Behavior Classification Index** `` *{Source: Appendix H}*
*   **Annex H: Formal Core Semantics** `` *{Source: Appendix I}*

#### **Annexes (Informative)**
*   **Annex I: Implementation Guide** *{Source: Appendix G}*