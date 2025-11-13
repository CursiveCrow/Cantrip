# The Cursive Language Specification

**Version**: 1.0.0 Draft  
**Date**: 2025-11-11  
**Status**: In Progress  
**Purpose**: Normative reference for Cursive language implementation and tooling

---

## Abstract

This document is the normative specification for the Cursive programming language. It defines the language's syntax, semantics, and behavior with precision sufficient for building conforming implementations and tools. The specification serves implementers, tool developers, and educators, covering all aspects from lexical structure to concurrency and interoperability.

---

## Table of Contents[toc]

- [The Cursive Language Specification](#the-cursive-language-specification)
  - [Abstract](#abstract)
  - [Table of Contents\[toc\]](#table-of-contentstoc)
- [Part 0 - Document Orientation](#part-0---document-orientation)
  - [1. Purpose, Scope, and Audience \[intro\]](#1-purpose-scope-and-audience-intro)
    - [1.1 Purpose \[intro.purpose\]](#11-purpose-intropurpose)
    - [1.2 Scope \[intro.scope\]](#12-scope-introscope)
    - [1.3 Audience \[intro.audience\]](#13-audience-introaudience)
  - [2. Document Primer \[primer\]](#2-document-primer-primer)
    - [2.1 Structure and Reference \[primer.structure\]](#21-structure-and-reference-primerstructure)
    - [Cross-References](#cross-references)
    - [2.2 Annotations \[primer.annotations\]](#22-annotations-primerannotations)
    - [2.3 Versioning \[primer.versioning\]](#23-versioning-primerversioning)
    - [2.4 Normative Requirement Organization \[primer.requirements\]](#24-normative-requirement-organization-primerrequirements)
  - [3 Notation \[notation\]](#3-notation-notation)
    - [3.1 Mathematical metavariables \[notation.metavariables\]](#31-mathematical-metavariables-notationmetavariables)
    - [3.1.1 Grammar template syntax \[notation.grammar\]](#311-grammar-template-syntax-notationgrammar)
    - [3.1.2 Mathematical Notation \[notation.mathematical\]](#312-mathematical-notation-notationmathematical)
  - [3.2 Inference Rules and Formal Semantics \[notation.inference\]](#32-inference-rules-and-formal-semantics-notationinference)
  - [4 Terminology \[terminology\]](#4-terminology-terminology)
    - [4.1 General Terms \[terminology.general\]](#41-general-terms-terminologygeneral)
    - [4.2 Conformance Terms \[terminology.conformance\]](#42-conformance-terms-terminologyconformance)
    - [4.3 Programming Terms \[terminology.programming\]](#43-programming-terms-terminologyprogramming)
    - [4.4 Symbols and Abbreviations \[terminology.symbols\]](#44-symbols-and-abbreviations-terminologysymbols)
  - [5. References and Citations \[references\]](#5-references-and-citations-references)
    - [5.1 ISO/IEC and International Standards \[references.iso\]](#51-isoiec-and-international-standards-referencesiso)
    - [5.2 Unicode Standards \[references.unicode\]](#52-unicode-standards-referencesunicode)
    - [5.3 Platform and ABI Standards \[references.platform\]](#53-platform-and-abi-standards-referencesplatform)
    - [5.4 IETF Standards \[references.ietf\]](#54-ietf-standards-referencesietf)
    - [5.5 Conventions, Tooling, and Miscellaneous \[references.conventions\]](#55-conventions-tooling-and-miscellaneous-referencesconventions)
    - [5.6 Research \[references.research\]](#56-research-referencesresearch)
    - [5.7 Reference Availability \[references.availability\]](#57-reference-availability-referencesavailability)
- [Part I - Conformance, Diagnostics, and Governance](#part-i---conformance-diagnostics-and-governance)
  - [6. Fundamental Conformance \[conformance\]](#6-fundamental-conformance-conformance)
    - [6.1 Behavior Classifications \[conformance.behavior\]](#61-behavior-classifications-conformancebehavior)
    - [6.2 Conformance Obligations \[conformance.obligations\]](#62-conformance-obligations-conformanceobligations)
  - [7. Diagnostics \[diagnostics\]](#7-diagnostics-diagnostics)
    - [7.1 Overview \[diagnostics.overview\]](#71-overview-diagnosticsoverview)
    - [7.2 Diagnostic Structure \[diagnostics.structure\]](#72-diagnostic-structure-diagnosticsstructure)
    - [7.3 Emission Requirements \[diagnostics.emission\]](#73-emission-requirements-diagnosticsemission)
    - [7.4 Diagnostic Code System \[diagnostics.codes\]](#74-diagnostic-code-system-diagnosticscodes)
    - [7.5 Message Quality \[diagnostics.quality\]](#75-message-quality-diagnosticsquality)
    - [7.6 Optional Enhancements \[diagnostics.enhancements\]](#76-optional-enhancements-diagnosticsenhancements)
  - [8. Extension Requirements \[extensions\]](#8-extension-requirements-extensions)
    - [8.1 Extension Model \[extensions.model\]](#81-extension-model-extensionsmodel)
    - [8.2 Forbidden Extension Classes \[extensions.forbidden\]](#82-forbidden-extension-classes-extensionsforbidden)
    - [8.3 Conformance Modes \[extensions.modes\]](#83-conformance-modes-extensionsmodes)
    - [8.4 Extension Versioning \[extensions.versioning\]](#84-extension-versioning-extensionsversioning)
  - [9. Version Compatibility \[versions\]](#9-version-compatibility-versions)
    - [9.1 Backward Compatibility \[versions.backward\]](#91-backward-compatibility-versionsbackward)
  - [10. Conformance Validation \[validation\]](#10-conformance-validation-validation)
    - [10.1 Testing Requirements \[validation.testing\]](#101-testing-requirements-validationtesting)
    - [10.2 Feature Classification \[validation.classification\]](#102-feature-classification-validationclassification)
  - [Part II - Program Structure \& Translation](#part-ii---program-structure--translation)
  - [8. Source Text \& Translation Pipeline](#8-source-text--translation-pipeline)
    - [8.1 Source encoding and normalization](#81-source-encoding-and-normalization)
    - [8.2 Translation phases](#82-translation-phases)
    - [8.3 Phase semantics](#83-phase-semantics)
  - [9. Lexical Structure](#9-lexical-structure)
    - [9.1 Tokens, identifiers, and comments](#91-tokens-identifiers-and-comments)
    - [9.2 Automatic statement termination](#92-automatic-statement-termination)
    - [9.3 Lexical diagnostics](#93-lexical-diagnostics)
  - [10. Modules, Assemblies, and Projects](#10-modules-assemblies-and-projects)
    - [10.1 Folder-scoped modules](#101-folder-scoped-modules)
    - [10.2 Assemblies and projects](#102-assemblies-and-projects)
    - [10.3 Imports, aliases, and qualified visibility](#103-imports-aliases-and-qualified-visibility)
    - [10.4 Dependency graph and initialization](#104-dependency-graph-and-initialization)
  - [11. Scopes, Bindings, and Lookup](#11-scopes-bindings-and-lookup)
    - [11.1 Scope tree](#111-scope-tree)
    - [11.2 Shadowing and binding metadata](#112-shadowing-and-binding-metadata)
    - [11.3 Name lookup](#113-name-lookup)
    - [11.4 Universe scope and protected identifiers](#114-universe-scope-and-protected-identifiers)
  - [12. Tooling hooks](#12-tooling-hooks)
    - [12.1 Compiler data surfaces](#121-compiler-data-surfaces)
  - [Part III - Declarations \& the Type System](#part-iii---declarations--the-type-system)
  - [10. Declarations](#10-declarations)
    - [10.0 Declaration Judgment](#100-declaration-judgment)
    - [10.1 Bindings \[decl.bindings\]](#101-bindings-declbindings)
    - [10.2 Procedures \[decl.procedure\]](#102-procedures-declprocedure)
    - [10.3 Type Declarations \[decl.types\]](#103-type-declarations-decltypes)
  - [11. Type Foundations](#11-type-foundations)
    - [11.0 Type Formation Judgment](#110-type-formation-judgment)
    - [11.1 Kinds and Universes](#111-kinds-and-universes)
    - [11.2 Equivalence \& Subtyping](#112-equivalence--subtyping)
    - [11.3 Type Inference Boundaries](#113-type-inference-boundaries)
  - [12. Primitive \& Composite Types](#12-primitive--composite-types)
    - [12.1 Primitive Scalars](#121-primitive-scalars)
    - [12.2 Composite Types](#122-composite-types)
  - [13. Modal Types (General Framework)](#13-modal-types-general-framework)
    - [13.1 Definition](#131-definition)
    - [13.2 Syntax \& Formation](#132-syntax--formation)
    - [13.3 Contracts and Witnesses](#133-contracts-and-witnesses)
  - [14. Pointer \& Reference Model](#14-pointer--reference-model)
    - [14.1 Safe Pointer `Ptr<T>`](#141-safe-pointer-ptrt)
    - [14.2 Raw Pointers `*const/*mut`](#142-raw-pointers-constmut)
    - [14.3 Escape \& Provenance Rules](#143-escape--provenance-rules)
  - [15. Generics \& Parametric Polymorphism](#15-generics--parametric-polymorphism)
    - [15.1 Parameters and Bounds](#151-parameters-and-bounds)
    - [15.2 Variance](#152-variance)
    - [15.3 Monomorphization](#153-monomorphization)
    - [15.4 Generic Constants \& Determinism](#154-generic-constants--determinism)
  - [Part IV - Ownership, Permissions, Regions](#part-iv---ownership-permissions-regions)
  - [16. Ownership \& Responsibility](#16-ownership--responsibility)
    - [16.1 Concepts and Axes](#161-concepts-and-axes)
    - [16.2 Responsible vs Non-responsible Bindings](#162-responsible-vs-non-responsible-bindings)
    - [16.3 Move Semantics](#163-move-semantics)
    - [16.4 RAII \& Deterministic Cleanup](#164-raii--deterministic-cleanup)
    - [16.5 Copy \& Drop](#165-copy--drop)
  - [17. Lexical Permission System (LPS)](#17-lexical-permission-system-lps)
    - [17.1 Overview](#171-overview)
    - [17.2 Permission Lattice and Coercions](#172-permission-lattice-and-coercions)
    - [17.3 Permission Semantics](#173-permission-semantics)
    - [17.4 Field-Level Partitioning and Reborrowing](#174-field-level-partitioning-and-reborrowing)
    - [17.5 Receivers and Parameters](#175-receivers-and-parameters)
  - [18. Region-Based Memory \& Arenas](#18-region-based-memory--arenas)
    - [18.1 Regions and `region {}` Blocks](#181-regions-and-region--blocks)
    - [18.2 Arena Modal Type](#182-arena-modal-type)
    - [18.3 Interplay with Heap and Stack](#183-interplay-with-heap-and-stack)
  - [19. Moves, Assignments, and Destruction](#19-moves-assignments-and-destruction)
    - [19.1 Assignment Operators and Responsibility](#191-assignment-operators-and-responsibility)
    - [19.2 Deterministic Drop Semantics](#192-deterministic-drop-semantics)
    - [19.3 Non-responsible Bindings and Safety Patterns](#193-non-responsible-bindings-and-safety-patterns)
  - [Part V - Expressions, Statements, and Control Flow](#part-v---expressions-statements-and-control-flow)
  - [20. Expressions](#20-expressions)
    - [20.1 Expression Model and Categories](#201-expression-model-and-categories)
    - [20.2 Operators and Precedence](#202-operators-and-precedence)
    - [20.3 Unary and Binary Operators](#203-unary-and-binary-operators)
    - [20.4 Calls, Methods, and Pipelines](#204-calls-methods-and-pipelines)
    - [20.5 Literals and Construction](#205-literals-and-construction)
    - [20.6 Address-of and Dereference](#206-address-of-and-dereference)
    - [20.7 Conversions and Coercions](#207-conversions-and-coercions)
    - [20.8 Diagnostics and Metadata](#208-diagnostics-and-metadata)
  - [21. Statements](#21-statements)
    - [21.0 Statement Judgment](#210-statement-judgment)
    - [21.1 Fundamentals and Outcomes](#211-fundamentals-and-outcomes)
    - [21.2 Variable Declarations](#212-variable-declarations)
    - [21.3 Assignments](#213-assignments)
    - [21.4 Blocks, `defer`, and Regions](#214-blocks-defer-and-regions)
    - [21.5 Return, Break, Continue](#215-return-break-continue)
    - [21.6 Unsafe Blocks](#216-unsafe-blocks)
  - [22. Control Flow and Pattern Matching](#22-control-flow-and-pattern-matching)
    - [22.1 Conditionals and Blocks](#221-conditionals-and-blocks)
    - [22.2 `match` Expressions](#222-match-expressions)
    - [22.3 Loops](#223-loops)
    - [22.4 Short-Circuiting](#224-short-circuiting)
  - [23. Error Handling and Panics](#23-error-handling-and-panics)
    - [23.1 Contracts vs. Runtime Errors](#231-contracts-vs-runtime-errors)
    - [23.2 Panics](#232-panics)
    - [23.3 Interoperability with FFI Errors](#233-interoperability-with-ffi-errors)
  - [When `[[unwind(catch)]]` is used, specifications **SHOULD** model error returns explicitly (e.g., unions) and postconditions **SHOULD** describe the mapping from panic to foreign error values.](#when-unwindcatch-is-used-specifications-should-model-error-returns-explicitly-eg-unions-and-postconditions-should-describe-the-mapping-from-panic-to-foreign-error-values)
  - [Part VI - Contracts, and Grants](#part-vi---contracts-and-grants)
  - [14. Contract Language](#14-contract-language)
    - [14.1 Overview and Sequent Notation](#141-overview-and-sequent-notation)
    - [14.2 Components and Scope](#142-components-and-scope)
    - [14.3 Grammar and Abbreviation Rules](#143-grammar-and-abbreviation-rules)
    - [14.4 Composition, Subtyping, and Implementation Obligations](#144-composition-subtyping-and-implementation-obligations)
    - [14.5 Purity and Compile-Time Execution](#145-purity-and-compile-time-execution)
  - [15. Grants](#15-grants)
    - [15.1 Grant Taxonomy](#151-grant-taxonomy)
    - [15.2 Availability, Propagation, and Intrinsics](#152-availability-propagation-and-intrinsics)
    - [15.3 Grant Checking](#153-grant-checking)
    - [15.4 Grant Polymorphism](#154-grant-polymorphism)
    - [15.5 Callable Types and Purity](#155-callable-types-and-purity)
  - [16. Diagnostics for Contracts and Effects](#16-diagnostics-for-contracts-and-effects)
    - [16.1 Sequent Syntax Diagnostics (Parsing/Well-Formedness)](#161-sequent-syntax-diagnostics-parsingwell-formedness)
    - [16.2 Checking Flow Diagnostics (Typing/Verification)](#162-checking-flow-diagnostics-typingverification)
    - [16.3 Preconditions (Must) - Verification and Runtime](#163-preconditions-must---verification-and-runtime)
    - [16.4 Postconditions (Will) - Obligations and Enforcement](#164-postconditions-will---obligations-and-enforcement)
    - [16.5 Verification Modes](#165-verification-modes)
    - [16.6 FFI, Unsafe, and Intrinsics](#166-ffi-unsafe-and-intrinsics)
    - [Conformance Summary (Part VI)](#conformance-summary-part-vi)
  - [Part VII - Abstraction, Behaviors, and Dynamic Dispatch](#part-vii---abstraction-behaviors-and-dynamic-dispatch)
  - [17. Behaviors (Interfaces/Traits)](#17-behaviors-interfacestraits)
    - [17.1 Purpose and Model](#171-purpose-and-model)
    - [17.2 Declaration Syntax and Items](#172-declaration-syntax-and-items)
    - [17.3 Extension (Behavior Inheritance)](#173-extension-behavior-inheritance)
    - [17.4 Implementations and Coherence](#174-implementations-and-coherence)
    - [17.5 Interaction with Permissions](#175-interaction-with-permissions)
  - [18. Witness System (Runtime Polymorphism)](#18-witness-system-runtime-polymorphism)
    - [18.1 Overview and Design Principles](#181-overview-and-design-principles)
    - [18.2 Types, Allocation States, and Permissions](#182-types-allocation-states-and-permissions)
    - [18.3 Representation and Storage](#183-representation-and-storage)
    - [18.4 Construction](#184-construction)
    - [18.5 Dispatch Semantics](#185-dispatch-semantics)
    - [18.6 Storage Strategies](#186-storage-strategies)
    - [18.7 Diagnostics](#187-diagnostics)
  - [19. Generics \& Behaviors Integration](#19-generics--behaviors-integration)
    - [19.1 Bounds and Associated Types](#191-bounds-and-associated-types)
    - [19.2 Static vs Dynamic Dispatch Selection](#192-static-vs-dynamic-dispatch-selection)
    - [19.3 Monomorphization and Performance (Informative)](#193-monomorphization-and-performance-informative)
    - [Conformance Summary](#conformance-summary)
  - [Part VIII - Compile-Time Evaluation \& Reflection (No Macros)](#part-viii---compile-time-evaluation--reflection-no-macros)
  - [20. Comptime Execution](#20-comptime-execution)
    - [20.1 Overview](#201-overview)
    - [20.2 Declaring Comptime Procedures](#202-declaring-comptime-procedures)
    - [20.3 Execution Semantics and Determinism](#203-execution-semantics-and-determinism)
    - [20.4 Comptime Grants and Effects](#204-comptime-grants-and-effects)
    - [20.5 Results, Constness, and Embedding](#205-results-constness-and-embedding)
    - [20.6 Diagnostics](#206-diagnostics)
  - [21. Opt-In Reflection](#21-opt-in-reflection)
    - [21.1 Enabling Reflection](#211-enabling-reflection)
    - [21.2 Availability and Phase Restrictions](#212-availability-and-phase-restrictions)
    - [21.3 Query Surface](#213-query-surface)
    - [21.4 Security and Privacy](#214-security-and-privacy)
    - [21.5 Diagnostics](#215-diagnostics)
  - [22. Code Generation APIs (Explicit Metaprogramming)](#22-code-generation-apis-explicit-metaprogramming)
    - [22.1 Principles (No Macros)](#221-principles-no-macros)
    - [22.2 Emission Model](#222-emission-model)
    - [22.3 Quotation and Typed Builders](#223-quotation-and-typed-builders)
    - [22.4 Name Hygiene and Scoping](#224-name-hygiene-and-scoping)
    - [22.5 Interaction with Contracts and Grants](#225-interaction-with-contracts-and-grants)
    - [22.6 Error Reporting from Comptime](#226-error-reporting-from-comptime)
    - [22.7 ABI and Reproducibility](#227-abi-and-reproducibility)
    - [Conformance Summary (Part VIII)](#conformance-summary-part-viii)
  - [Part IX - Memory Model, Layout, and ABI](#part-ix---memory-model-layout-and-abi)
  - [23. Memory Layout \& Alignment](#23-memory-layout--alignment)
    - [23.1 Representation Attributes](#231-representation-attributes)
    - [23.2 Natural Alignment](#232-natural-alignment)
    - [23.3 Explicit Alignment `[[repr(align(N))]]`](#233-explicit-alignment-repralignn)
    - [23.4 Size and Layout (Records/Tuples)](#234-size-and-layout-recordstuples)
    - [23.5 Arrays](#235-arrays)
    - [23.6 Packed Layout and Misalignment](#236-packed-layout-and-misalignment)
    - [23.7 Pointer Provenance (Overview)](#237-pointer-provenance-overview)
    - [23.8 Data-Race Model (Baseline)](#238-data-race-model-baseline)
  - [24. Interoperability \& FFI](#24-interoperability--ffi)
    - [24.1 FFI Declarations](#241-ffi-declarations)
    - [24.2 FFI-Safe Types](#242-ffi-safe-types)
    - [24.3 Grants and Safety at the Boundary](#243-grants-and-safety-at-the-boundary)
    - [24.4 Calling Conventions \& Linkage Hints](#244-calling-conventions--linkage-hints)
    - [24.5 C Strings, Pointers, and Lifetimes](#245-c-strings-pointers-and-lifetimes)
    - [24.6 Panic and Unwinding Across FFI](#246-panic-and-unwinding-across-ffi)
    - [24.7 Diagnostics (FFI)](#247-diagnostics-ffi)
  - [25. Linkage, ODR, and Binary Compatibility](#25-linkage-odr-and-binary-compatibility)
    - [25.1 Linkage Categories and Determination](#251-linkage-categories-and-determination)
    - [25.2 One Definition Rule (ODR)](#252-one-definition-rule-odr)
    - [25.3 Name Mangling and Attributes](#253-name-mangling-and-attributes)
    - [25.4 ABI Policies](#254-abi-policies)
    - [25.5 Stable Binary Interfaces (Guidance)](#255-stable-binary-interfaces-guidance)
    - [25.6 Conformance Summary (Part IX)](#256-conformance-summary-part-ix)
  - [Appendix A – Formal ANTLR Grammar (Normative)](#appendix-a--formal-antlr-grammar-normative)
  - [Appendix B – Diagnostic Code Taxonomy (Normative)](#appendix-b--diagnostic-code-taxonomy-normative)
    - [B.1 Feature Bucket Reference](#b1-feature-bucket-reference)
    - [B.2 `TYP` Category Buckets](#b2-typ-category-buckets)
    - [B.3 `MEM` Category Buckets](#b3-mem-category-buckets)
    - [B.4 `EXP` Category Buckets](#b4-exp-category-buckets)
    - [B.5 `GRN` Category Buckets](#b5-grn-category-buckets)
    - [B.6 `FFI` Category Buckets](#b6-ffi-category-buckets)
    - [B.7 `DIA` Category Buckets](#b7-dia-category-buckets)
    - [B.8 `CNF` Category Buckets](#b8-cnf-category-buckets)
  - [Appendix C – Implementation-Defined Behavior Index (Normative)](#appendix-c--implementation-defined-behavior-index-normative)
  - [Appendix D – Diagnostic Catalog (Normative)](#appendix-d--diagnostic-catalog-normative)

---

# Part 0 - Document Orientation

## 1. Purpose, Scope, and Audience [intro]

### 1.1 Purpose [intro.purpose]

This specification **defines the Cursive programming language**: its source text, static semantics (typing and name resolution), dynamic semantics (runtime behavior), concurrency and memory model, and the required behavior of conforming implementations. Its purpose is to make the language precise for **implementers** (compiler/runtime authors) and **practitioners** building systems software.

This specification is the sole normative reference for the Cursive programming language. All implementations must conform to the requirements stated herein.

### 1.2 Scope [intro.scope]

The specification covers:

- The **lexical grammar**, **concrete syntax**, and **abstract syntax** of Cursive source files.
- The **type system**, including generics and variance, nominal types, and contracts (preconditions, postconditions, and effects).
- The **module, assembly, and package system** as well as cross-compilation units.
- The **execution semantics**, including evaluation order, error conditions, and observable program behavior.
- The **concurrency and memory model**, including happens-before relations and data-race freedom conditions where applicable.
- The definition of **unverifiable behavior (UVB)**, **implementation-defined behavior**, and **unspecified behavior**.
- The **Foreign Function Interface (FFI)** surface, insofar as the behavior of Cursive programs depends on it.
- Minimal **core library** requirements necessary for program execution where the language mandates them.

Out of scope (informative only): performance guidance, style advice, tutorial material, and non-normative rationale.

### 1.3 Audience [intro.audience]

The primary audiences of this document are: (a) **implementation authors** who must conform to these rules and (b) **users writing systems software** who require predictable, portable, and safe behavior. Secondary audiences include tool authors (linters, formatters, analyzers) and educators.

---

## 2. Document Primer [primer]

### 2.1 Structure and Reference [primer.structure]

This specification uses hierarchical decimal numbering (ISO 2145) with semantic anchors for stable cross-references.

#### Document Structure

```
# Document Title                              (Level 1 - Title)
# Part X - Part Name                          (Level 2 - Part headers)
## N. Chapter Name [chapter.id]               (Level 2 - Numbered chapters)
### N.M Grouping Section [chapter.section]    (Level 3 - Organizational sections)
#### N.M.K Normative Requirement              (Level 4 - All MUST/SHOULD/MAY statements)
##### N.M.K.J Sub-requirement                 (Level 5 - Rare exceptional cases only)
```

### Cross-References

In-text cross-references use section numbers accompanied by semantic anchors, e.g. (1.2 [intro.scope]).

#### Semantic Anchors

Major sections include semantic anchors in square brackets `[category.subcategory]` or `[category.subcategory.detail]` providing stable links across document versions. Examples:

- `[intro.scope]` - Introduction and scope
- `[decl.bindings]` - Declaration of bindings
- `[types.composite.modal]` - Composite types, modal subsection

These anchors remain constant even if section numbers change between specification versions.

#### 2.1.1 Typographic Conventions [primer.structure.typography]

- Source code and tokens appear in `monospace`.
- Metavariables and definitions use _italics_.
- Keywords appear in **bold** when called out in prose.
- **_Bold italics_** denote intra-sectional divisions, such as **_Formal rule_**, **_Example_**, and **_Explanation_**.

The keywords **MUST**, **MUST NOT**, **SHOULD**, **SHOULD NOT**, **REQUIRED**, **RECOMMENDED**, and **MAY** are to be interpreted as described in RFC 2119 and RFC 8174 when, and only when, they appear in all capitals.

#### 2.1.2 Code Examples [primer.structure.examples]

Code examples in this document use fenced blocks with the `cursive` info string. Unless the text indicates otherwise, examples are informative; a trailing comment like `// error[E-CCC-FFNN]: …` marks the diagnostic the example is expected to trigger.

#### 2.1.3 Diagnostic Code Format [primer.structure.diagnostics]

Diagnostics follow the format `K-CCC-FFNN` where **K** indicates severity (`E` for error, `W` for warning, `N` for note), **CCC** is a three-letter category code, **FF** is a two-digit feature bucket, and **NN** is a sequence number within the bucket.

### 2.2 Annotations [primer.annotations]

The specification uses Markdown callout syntax for non-normative annotations:

> [!note]
> Non-normative clarifications, additional context, or explanatory remarks.

> [!tip] Rationale
> Design decisions and justifications (used sparingly, only for non-obvious choices).

> [!caution] Deprecated
> This feature is scheduled for removal in version <MAJOR.MINOR.PATCH>.
>
> This marker indicates a feature that:
>
> - The feature remains part of the current specification
> - The feature may be removed in the indicated version.
> - Implementations are encouraged to provide warnings when deprecated features are used (see §7.6 for implementation requirements)

> [!warning] Experimental
> This feature is experimental and subject to change.
>
> This annotation indicates a feature that:
>
> - Is not required for conformance
> - May change significantly in future versions
> - Require explicit opt-in at the implementation level

### 2.3 Versioning [primer.versioning]

This specification uses semantic versioning with the format `MAJOR.MINOR.PATCH`:

- **MAJOR** - incremented for incompatible changes that break existing conforming programs or change language semantics. Examples: new keywords that were previously valid identifiers, changes to type system rules, modifications to evaluation order.
- **MINOR** - incremented for backwards-compatible additions and clarifications that do not change the meaning of existing conforming programs. Examples: new language features with new syntax, clarifications that resolve ambiguity without changing behavior, additional diagnostic requirements.
- **PATCH** - incremented for backwards-compatible fixes and editorial corrections. Examples: typo corrections, improved wording, formatting changes, correction of internal cross-references.

The version identifier appears in the document header. This versioning scheme follows standard semantic versioning principles as defined in [SemVer 2.0.0](https://semver.org/).

### 2.4 Normative Requirement Organization [primer.requirements]

Throughout this specification, individual requirements generally follow this presentation pattern:

#### Normative Requirement Description

> The requirement itself, enclosed in a blockquote using normative keywords (**MUST**, **SHOULD**, **MAY**).
>
> **_Syntax:_**
>
> ```cursive
> Grammar patterns, when present, appear within the blockquote as part of the normative statement using template-based syntax (e.g., `<condition>`, `<block>`).
> ```

**_Formal rule:_**
$$ \text{A inference rule or judgment (when the requirement involves typing, evaluation, or well-formedness)} $$

**_Explanation:_**
Elaboration via prose, tables, or diagrams clarifying the requirement's meaning, rationale, or scope.

**_Example - Example Description_**

```cursive
Concrete code demonstrating the rule in practice, often including both valid and invalid cases
```

#### 2.2.1 Subordinate Requirements [primer.requirements.subordinate]

Some requirements contain subordinate subsections that refine or constrain the parent requirement. When present, subordinate requirements have the following structure:

- #### Main Heading
- > Main Requirement
  - ##### Subordinate Heading 1
  - > Subordinate Requirement 1
  - **_Subordinate Formal Rule 1_**
  - **_Subordinate Explanation 1_**
  - **_Subordinate Example 1_**
  - ##### Subordinate Heading 2
  - > Subordinate Requirement 2
  - [...]
- **_Main Formal Rule_**
- **_Main Explanation_**
- **_Main Example_**

**_Example - Syntactic and semantic requirement:_**

````markdown
##### 10.1.1 Binding kinds

> Every declaration **MUST** use either `let` (immutable) or `var` (rebindable).
> Rebinding a `let` binding **MUST NOT** be permitted.
>
> **_Syntax:_**
>
> ```
> let <pattern> = <expr>
> let <pattern> <- <expr>
> var <pattern> = <expr>
> var <pattern> <- <expr>
> ```

**_Formal rule:_**
[inference rule for well-formed bindings]

The distinction between `let` and `var` affects only rebindability, not the value's
internal mutability. The `=` operator creates a responsible binding that runs
destructors at scope exit, while `<-` creates a non-responsible binding.

**_Example - Valid bindings:_**

```cursive
let x = 5;
x = 10;  // error: cannot rebind `let` binding

var y = 5;
y = 10;  // ok: var allows rebinding
```
````

> [!note]
> The complete, formal ANTLR-format grammar production of Cursive is provided in Appendix A.

---

## 3 Notation [notation]

### 3.1 Mathematical metavariables [notation.metavariables]

Formal notation throughout this specification uses the following metavariables to denote syntactic and semantic entities:

- $x, y, z$ - _variables and identifiers_
- $T, U, V$ _(or $\tau$) - types_
- $e, f, g$ - _expressions_
- $p$ - _patterns_
- $s$ - _statements_
- $\Gamma, \Delta, \Sigma$ - _contexts_
- $\sigma$ - _program stores_
- $G$ - _grant sets_
- $@S$ - _modal states_
- $\ell$ - _memory locations_
- $v$ - _values_

**_Metavariable conventions:_**

- $'$ (prime) - _resulting or "after" states (e.g., $\Gamma'$, $\sigma'$)_
- Subscripts - _variants or specialized forms (e.g., $\Gamma_{\text{terms}}$, $\sigma_{\text{ct}}$)\_
- $\cdot$ - _empty context_

These symbols serve as placeholders in inference rules, judgments, and formal definitions.

### 3.1.1 Grammar template syntax [notation.grammar]

Grammar productions within normative statements use angle-bracket placeholders to denote syntactic categories. These placeholders represent portions of syntax that must be filled with concrete constructs:

**\*Common syntactic categories:**

- `<identifier>` - any valid identifier (as defined in Part II, §9.1.1)
- `<expression>` - any expression
- `<statement>` or `<statements>` - one or more statements
- `<pattern>` - a binding pattern (identifier, tuple destructuring, etc.)
- `<condition>` - a boolean-valued expression
- `<block>` - a brace-enclosed block of statements `{ ... }`
- `<type>` - a type expression
- `<literal>` - a literal value (numeric, string, character, or boolean)
- `<parameters>` - comma-separated list of function parameters

**_Repetition and optional elements:_**
When inline grammar fragments require repetition or optionality, the following standard conventions apply:

- `element*` - zero or more repetitions
- `element+` - one or more repetitions
- `element?` - optional (zero or one occurrence)
- `alt1 | alt2` - alternatives (either alt1 or alt2)

**_Example usage in normative statements:_**

```
let <pattern> = <expr>
if <condition> { <statements> }
procedure <identifier>(<parameters>) -> <type>
```

These placeholders are purely descriptive and indicate what kind of syntax is expected in each position. The complete formal ANTLR grammar appears in an appendix; inline grammar fragments use this template notation for readability.

### 3.1.2 Mathematical Notation [notation.mathematical]

Formal definitions and inference rules use standard mathematical notation:

**_Set operations:_**

- $\in$, $\notin$ - set membership and non-membership
- $\cup$ - union
- $\cap$ - intersection
- $\subseteq$ - subset relation

**_Logical connectives:_**

- $\land$ - logical and
- $\lor$ - logical or
- $\lnot$ - logical negation
- $\implies$ - implication
- $\iff$ - if and only if (bidirectional implication)

**_Quantifiers:_**

- $\forall$ - universal quantification ("for all")
- $\exists$ - existential quantification ("there exists")

**_Functions and mappings:_**

- $f: A \to B$ - (total) function from type $A$ to type $B$
- $f: A \rightharpoonup B$ - partial function (may be undefined for some inputs)
- $(a, b, c)$ - tuple or sequence of elements

These symbols follow their standard mathematical meanings without additional specification-specific interpretation.

## 3.2 Inference Rules and Formal Semantics [notation.inference]

Inference rules use standard fraction notation:

$$
\frac{\text{premise}_1 \quad \cdots \quad \text{premise}_n}{\text{conclusion}}
\tag{Rule-Name}
$$

Rules without premises are axioms. Side conditions appear to the right of the line when additional constraints apply.

**_Judgment grammar_:**

- $\vdash$ - turnstile; $\Gamma \vdash J$ means judgment $J$ holds under context $\Gamma$
- $\Rightarrow$ - transformation; $\Gamma \vdash X \Rightarrow \Gamma'$ means processing $X$ transforms $\Gamma$ to $\Gamma'$
- $\Downarrow$ - big-step evaluation; $\sigma \vdash e \Downarrow v, \sigma'$ means $e$ evaluates to $v$ in store $\sigma$ producing store $\sigma'$
- $[\text{cat}]$ - categorization; indicates expression category in typing judgments
- $,$ (comma) - sequencing; separates input/output components (e.g., $v, \sigma'$)
- $\Gamma, x : B$ - context extension (adds binding of $x$ to $B$)
- $\Gamma[x \mapsto \tau]$ - context update (replaces binding for $x$)

**_Rule naming prefixes_:**

- **T**-_Feature_-_Case_ - Type formation and typing rules
- **E**-_Feature_-_Case_ - Evaluation and operational semantics
- **WF**-_Feature_-_Case_ - Well-formedness and static checking
- **P**-_Feature_-_Case_ - Permission and memory safety
- **Prop**-_Feature_-_Case_ - Behavior satisfaction and property proofs
- **Coerce**-_Feature_-_Case_ - Type coercion rules
- **Prov**-_Feature_-_Case_ - Provenance and aliasing
- **Ptr**-_Feature_-_Case_ - Pointer-specific properties
- **QR**-_Feature_-_Case_ - Qualified name resolution

---

## 4 Terminology [terminology]

The vocabulary below defines terms as used throughout this specification. Terms defined in this section carry the same meaning in all later sections.

### 4.1 General Terms [terminology.general]

**Implementation**
_Any compiler, interpreter, JIT, transpiler, or runtime system that accepts Cursive source code and produces program behavior. Unless stated otherwise, every requirement labeled "implementation" applies to all of these tool categories (cf. §7.1)._

**Program**
_A complete, standalone unit comprising one or more Cursive translation units intended for execution. Programs may link against or embed libraries. For conformance evaluation, the entire program (including all linked code) must be well-formed, and execution must not trigger unverifiable behavior. A conforming program, together with its dependencies, exhibits only defined behavior (cf. §6.2.2)._

**Library**
_A reusable collection of declarations intended for use by programs or other libraries. Libraries are evaluated for conformance independently. A conforming library must not rely on unverifiable behavior and must document all implementation-defined behavior choices (cf. §6.2.2)._

**Translation**
_The process of converting source code into executable artifacts._

**Execution**
_The runtime evaluation of translated code._

### 4.2 Conformance Terms [terminology.conformance]

**Conforming Implementation**
_An implementation is conforming iff it satisfies all MUST requirements of this specification and, for any accepted program, produces observable behavior that matches the dynamic semantics, except where implementation‑defined or unspecified behavior permits variation. See §6.2.1._

**Conforming Program**
_A program is conforming iff it is well‑formed, relies only on documented IDB, uses extensions only when explicitly enabled, and does not trigger UVB during execution unless every executed UVB site is attested by an external proof artifact recorded in the conformance dossier (see §6.2.4). The presence of unattested UVB site executions makes a program non‑conforming._

**Unverifiable Behavior (UVB)**
_Behavior arising from operations whose correctness the implementation cannot verify statically or dynamically. Implementations are not required to detect or prevent UVB and may terminate the process outside normal control flow if it occurs; external contracts (e.g., FFI) govern soundness at such sites. See §6.1.1 and §6.1.2._

**Unspecified Behavior (USB)**
_A set of outcomes permitted by this specification for a construct where the implementation need not document which outcome it chooses; the set of permitted outcomes is constrained by this specification. See §6.1.3._

**Implementation-Defined Behavior (IDB)**
_A choice among multiple outcomes permitted by this specification that the implementation MUST document (e.g., in a conformance dossier or target notes). Programs MUST NOT rely on undocumented choices. See §6.1.4 and §6.1.4.1._

**Well-formed Program**
_Source that satisfies all lexical, syntactic, and static‑semantic rules. Implementations MUST accept well‑formed programs. See §6.2.3._

**Ill-formed Program**
_Source that violates lexical, syntactic, or static‑semantic rules. Implementations MUST diagnose and reject such programs; no artifacts may be produced. Certain violations may be classified IFNDR when detection is computationally infeasible (§6.1.3.1); otherwise, at least one error MUST be issued. See §6.2.3._

### 4.3 Programming Terms [terminology.programming]

**Binding**
_the association between an identifier and an entity within a particular scope._

**Translation Unit**
_the smallest source artifact processed as a whole during translation_.

**Declaration**
_a syntactic form that introduces a name and determines its category._

**Diagnostic**
_a message issued when a program violates a rule requiring detection. Diagnostics include errors (rejecting) and warnings (permissive)._

**Entity**
_any value, type, or module that may be named or referenced._

**Object**
_a region of storage with a type, storage location (contiguous bytes at a specific memory address), lifetime (bounded interval from creation to destruction), alignment constraint, and cleanup responsibility (association with zero or more responsible bindings)._

**Value**
_a temporary result produced by an expression that may be moved (consumed) or copied. Literals, arithmetic expressions, and most procedure calls produce values._

**Expression**
_a syntactic form that yields a value or place._

**Place**
_a memory location that can be the target of assignment or the source of address-of operations; expressions are categorized as yielding values or places._

**Modal State**
_a named compile-time state (denoted `@State`) in a modal type's state machine; modal values must inhabit exactly one state at any time, and transitions between states are enforced by the type system._

**Statement**
_a syntactic form that executes for effects on program state without directly yielding a value._

**Scope**
_the syntactic region where a binding is visible._

**Principal Type**
_in a type inference context, the most general type that satisfies all constraints. When type inference succeeds, the principal type is unique._

**Nominal Type**
_a type identified by a declaration and name, possibly parametrized; variance of parameters is declared per parameter._

### 4.4 Symbols and Abbreviations [terminology.symbols]

- **ABI** - Application Binary Interface
- **AST** - Abstract Syntax Tree
- **EBNF** - Extended Backus-Naur Form
- **FFI** - Foreign Function Interface
- **IFNDR** - Ill-Formed, No Diagnostic Required
- **LPS** - Lexical Permission System
- **RAII** - Resource Acquired on Initialization
- **UVB** - Unverifiable Behavior
- **UTF-8** - Unicode Transformation Format, 8-bit

---

## 5. References and Citations [references]

The documents listed below contain provisions that, through citation, form requirements of this specification. Documents identified as normative define requirements for conforming implementations.

### 5.1 ISO/IEC and International Standards [references.iso]

- **[ISO10646]** ISO/IEC 10646:2020 - _Information technology - Universal Coded Character Set (UCS)_. Defines the character repertoire permitted in source text and informs identifier classifications.

- **[IEEE754]** ISO/IEC 60559:2020 (IEEE 754-2019) - _Floating-Point Arithmetic_. Governs semantics for binary32 (`f32`) and binary64 (`f64`) arithmetic used throughout numeric type definitions and runtime semantics.

- **[C18]** ISO/IEC 9899:2018 - _Programming Languages - C_. The Foreign Function Interface (FFI) relies on C definitions of object representation, calling conventions, and interoperability semantics.

### 5.2 Unicode Standards [references.unicode]

- **[Unicode]** The Unicode Standard, Version 15.0.0 or later. Supplies derived properties, normalization forms, and identifier recommendations leveraged by the lexical grammar.

- **[UAX31]** Unicode Standard Annex #31 - _Unicode Identifier and Pattern Syntax_. Specifies identifier composition rules (`XID_Start`, `XID_Continue`) adopted in lexical tokenization.

### 5.3 Platform and ABI Standards [references.platform]

- **[SysV-ABI]** System V Application Binary Interface (AMD64 Architecture Processor Supplement, ARM Architecture Procedure Call Standard). Guides interoperability obligations for POSIX-compliant platforms.

- **[MS-x64]** Microsoft x64 Calling Convention - Calling convention documentation for Windows x64 platforms. Required for conforming implementations targeting Windows.

### 5.4 IETF Standards [references.ietf]

- **[RFC2119]** RFC 2119 - _Key words for use in RFCs to Indicate Requirement Levels_. Defines normative vocabulary (**MUST**, **SHOULD**, **MAY**).

- **[RFC8174]** RFC 8174 - _Ambiguity of Uppercase vs Lowercase in RFC 2119 Key Words_. Clarifies that requirement keywords apply only when in ALL CAPS.

### 5.5 Conventions, Tooling, and Miscellaneous [references.conventions]

- **[SemVer2]** Tom Preston‑Werner, “Semantic Versioning 2.0.0.” Defines versioning rules for artifacts and tools referenced by this specification.

- **[ANTLR4]** Terence Parr and Sam Harwell, “ANTLR 4 Grammar Syntax and Toolchain.” Cited for grammar notation and tooling guidance used in the formal grammar appendix.

### 5.6 Research [references.research]

- **[C18-Spec]** ISO/IEC 9899:2018 — Programming Languages — C. Informative comparison point for FFI object representation and linkage models.
- **[CXX23]** ISO/IEC 14882:2023 — Programming Languages — C++. Referenced for comparative discussion of templates, layout, and ABI interactions.
- **[Rust-Ref]** The Rust Reference — The Rust Project Developers. Informative background on contracts, ownership models, and diagnostics organization.
- **[Go-Spec]** The Go Programming Language Specification — The Go Authors. Informative background on packages, interfaces, and memory model choices.
- **[ECMA-334]** ECMA‑334 — C# Language Specification (latest edition). Informative background on attributes, metadata, and generics.
- **[PLP5]** Michael L. Scott, "Programming Language Pragmatics," Fifth Edition. ISBN‑13: 978‑0323999663. General reference for language design trade‑offs and formalism.

### 5.7 Reference Availability [references.availability]

Where references are not available freely, they are available for purchase from their respective publishers.

---

# Part I - Conformance, Diagnostics, and Governance

This part establishes the foundational requirements for conforming Cursive implementations, defines diagnostic reporting standards, and specifies governance rules for extensions and backward compatibility. It provides the normative framework that all other parts of this specification build upon, including conformance obligations, diagnostic taxonomy, and behavior classification.

## 6. Fundamental Conformance [conformance]

This chapter defines fundamental conformance requirements for implementations and programs. Section 6.1 classifies behavior types (unverifiable, unspecified, implementation-defined, defined, and IFNDR), establishing the foundation for reasoning about program portability and correctness. Section 6.2 specifies conformance obligations for implementations and programs, including well-formedness requirements, conformance dossier documentation, and the handling of IFNDR violations.

### 6.1 Behavior Classifications [conformance.behavior]

This section defines four categories of program behavior based on how this specification constrains outcomes and verification feasibility.

**Unverifiable Behavior (UVB)** [§4.2] comprises operations whose runtime correctness depends on properties that cannot be verified because verification requires:

1. **Logical impossibility**: Properties depend on external contracts, foreign memory validity, or non-deterministic external behavior
2. **Computational infeasibility**: Verification requires solving NP-hard, undecidable, or exponential-complexity problems
3. **Unbounded resources**: Verification requires unbounded time, memory, or whole-program analysis

UVB classification is based on verification problem structure, not implementation capabilities. Operations are UVB when their correctness depends on information outside the language's verification model. Operations verifiable through feasible static analysis or runtime checks (see Part VI, §16.5) are NOT UVB, regardless of implementation capabilities or configuration.

**Unspecified Behavior (USB)** [§4.2] occurs when this specification permits multiple valid outcomes but does not require implementations to document which outcome is chosen. Implementations may choose any permitted outcome, and the choice may vary between executions. Unlike implementation-defined behavior, no documentation is required. All permitted outcomes fall within bounds established by this specification.

**Implementation-Defined Behavior (IDB)** [§4.2] occurs when this specification permits multiple valid outcomes and requires implementations to document their choice. Unlike unspecified behavior, implementations must document their choices, enabling portability analysis. Common sources include target architecture characteristics (pointer size, endianness, alignment), calling conventions, and memory layout decisions.

**Defined Behavior** [§4.2] comprises all program behavior fully specified by this specification. Operations produce deterministic outcomes identical across conforming implementations (except where IDB allows documented variation). Defined behavior is the complement of UVB, USB, and IDB. Most language operations exhibit defined behavior: arithmetic on in-range values, control flow, pattern matching, bounded memory access, etc.

#### 6.1.1 Acceptance of UVB Programs [conformance.behavior.uvb]

> Implementations **MUST** accept well-formed programs containing UVB operations and **MUST NOT** reject programs solely for containing UVB.
>
> Translation **MUST** preserve program semantics up to and including UVB operation initiation. When UVB operations execute, implementations **MAY** produce any behavior, including abnormal termination.

**_Explanation:_**
Implementations need not detect UVB at compile time, emit warnings, or provide runtime protections. Programmers must ensure UVB operation correctness through external means (documentation, testing, manual proof, or external verification tools). The scope of “any behavior” at a UVB site is limited to the current process’s address space and externally observable effects of that process; the specification makes no guarantees outside that scope.

#### 6.1.2 Verification Modes and Resource Limits [conformance.behavior.verification]

> UVB classification is independent of verification modes (see Part VI, §16.5). Properties verifiable through feasible static analysis or runtime checks are NOT UVB, regardless of resource limits or policy choices.
>
> When verification fails due to resource constraints, implementations **MUST** either (a) reject the program with a diagnostic in strict mode, or (b) insert runtime checks in dynamic verification mode. Implementations **MUST NOT** insert runtime checks in strict mode unless explicitly enabled by a build flag or attribute.

**_Explanation:_**
Implementations cannot avoid verification obligations by claiming resource exhaustion. UVB operations are inherently unverifiable due to problem structure; non-UVB operations may be expensive to verify but remain the implementation's responsibility.

**_Example - UVB from FFI (logical impossibility):_**

```cursive
// Foreign function contract exists outside Cursive's semantic model
[[extern(C)]]
procedure legacy_get_buffer(): *mut u8;

unsafe {
    let ptr: *mut u8 = legacy_get_buffer();
    let first_byte = *ptr;  // UVB: Correctness depends on foreign contract:
                            // - Is the pointer non-null? (unknowable)
                            // - Does it point to valid, initialized memory? (unknowable)
                            // - Is the memory region accessible? (unknowable)
                            // - What is the pointer's provenance? (unknowable)
                            //
                            // Implementation cannot verify these properties.
                            // Programmer must ensure correctness through external means.
}
```

#### 6.1.3 Unspecified Behavior Requirements [conformance.behavior.usb]

> Permitted outcomes for USB **MUST** be constrained by this specification. Implementations **MAY** choose any permitted outcome. Different executions **MAY** produce different outcomes.

**_Explanation:_**
Unlike IDB (§6.1.4), implementations need not document their choice, and the choice may vary between executions. All permitted outcomes fall within specification bounds. Programs exhibiting USB are conforming.

**_Example - Unspecified padding bytes:_**

```cursive
record Point { x: i32, y: i32 }

procedure example() {
    let p = Point { x: 10, y: 20 };
    // Unspecified: padding bytes between fields and after the last field
    // may contain any bit pattern. Reading padding is well-formed but
    // produces unspecified values.

    let bytes = &p as *const u8;
    // bytes[0..3] are specified (x's representation)
    // bytes[4..7] are specified (y's representation)
    // Any padding bytes are unspecified
}
```

##### 6.1.3.1 Ill-Formed, No Diagnostic Required (IFNDR) [conformance.behavior.ifndr]

> Programs violating static-semantic rules are ill-formed. When violations are computationally infeasible to detect, implementations need not issue diagnostics. Such programs are **Ill-Formed, No Diagnostic Required (IFNDR)**. IFNDR program behavior is unspecified but **MUST NOT** include UVB (§6.1.1).

**_Explanation:_**
Violations are computationally infeasible to detect when detection requires solving NP-complete, undecidable, or exponential-complexity problems, or whole-program properties across compilation boundaries.

IFNDR differs from UVB: IFNDR represents ill-formedness; UVB represents operations whose runtime correctness depends on external properties. IFNDR programs are not conforming (§6.2.2); behavior is unspecified but bounded to exclude UVB.

> [!note] IFNDR vs UVB distinction
>
> - **IFNDR**: Program violated a language rule, but detecting the violation is computationally infeasible. Behavior is unspecified but constrained.
> - **UVB**: Program is well-formed, but operation correctness depends on unverifiable properties. Behavior is unconstrained.

**_Example - IFNDR via Semantic Unreachability_**

```cursive
// Whether this loop terminates reduces to the halting problem for 'f'.
procedure f(n: i64) -> i64 { /* ... Turing-complete ... */ }
procedure g(n: i64) {
    match f(n) {
        0 => handle_zero(),
        _ => handle_nonzero(),
    }
    // Omitting a 'panic' branch that is reachable iff f(n) never halts
    // makes 'g' ill-formed only if one can prove non-termination.
    // Diagnosing that is computationally infeasible: IFNDR.
}
```

#### 6.1.4 Implementation-Defined Behavior [conformance.behavior.idb]

> IDB occurs when this specification permits multiple valid outcomes and requires implementations to document their choice. Permitted outcomes **MUST** be constrained by this specification.
>
> Implementations **MUST** document all IDB choices in a conformance dossier (§6.2.4). Programs **MUST NOT** rely on undocumented IDB.

**_Explanation:_**
IDB allows target-appropriate choices while ensuring predictability. Unlike USB [§6.1.3], implementations must document their choices. Common sources include target architecture characteristics (pointer size, endianness, alignment), calling conventions, and memory layout.

##### 6.1.4.1 Documentation Requirements [conformance.behavior.idb.documentation]

> Implementations **MUST** document behaviors affecting portability or correctness:
>
> 1. Primitive type sizes, alignments, and representations (see Part III, §12.1; Part IX, §23.1)
> 2. Pointer width, representation, and provenance rules (see Part III, §14; Part IV, §17)
> 3. Foreign function calling conventions (see Part IX, §24.4)
> 4. Endianness for multi-byte values
> 5. Memory layout and padding for compound types (see Part IX, §23.4)
> 6. Signed integer overflow behavior
> 7. Thread scheduling, memory ordering, and synchronization (see Part IX, §23.8; Part IV, §17)
> 8. Platform-specific panic and unwind behavior (see Part V, §23.2)
>
> Documentation **SHOULD** be accessible via `--version`, online docs, or metadata files, organized by target triple and language version.

**_Example - Target-specific sizes and calling conventions:_**

```cursive
// Implementation-defined: size and alignment of primitive types
let x: i32 = 42;        // Size: documented in conformance dossier (typically 4 bytes)
                        // Alignment: documented per target (typically 4 bytes)

let p: usize = 0x1000;  // Size: pointer width, documented per target
                        // 8 bytes on 64-bit targets, 4 bytes on 32-bit targets

// Implementation-defined: endianness affects byte representation
let value: u32 = 0x01020304;
let bytes = &value as *const u8;
// Byte order documented for target:
// - Little-endian (x86-64, ARM64): [0x04, 0x03, 0x02, 0x01]
// - Big-endian (some MIPS, PowerPC): [0x01, 0x02, 0x03, 0x04]

// Implementation-defined: calling convention for extern functions
[[extern(C)]]    // C convention documented per platform ABI:
                 // - x86-64 Linux: System V AMD64 ABI
                 // - x86-64 Windows: Microsoft x64 calling convention
                 // - ARM64: ARM AAPCS64
procedure foreign_call();
```

### 6.2 Conformance Obligations [conformance.obligations]

This section defines conformance for implementations and programs.

#### 6.2.1 Conforming Implementations [conformance.obligations.implementations]

> Implementations **MUST** satisfy all MUST requirements and produce programs whose observable behavior matches the dynamic semantics defined herein, except where IDB (§6.1.4) or USB (§6.1.3) permits variation.

**_Explanation:_**
Observable behavior includes program output, side effects on external resources, termination status, and any other effects visible outside the program's address space, as defined throughout this specification.

#### 6.2.2 Conforming Programs [conformance.obligations.programs]

> A program conforms when it satisfies:
>
> 1. **Well-formedness**: Satisfies all lexical, syntactic, and static-semantic rules (§6.2.3)
> 2. **Documented IDB reliance**: Relies only on documented IDB (§6.1.4.1, §6.2.4)
> 3. **Extension usage**: Uses extensions only when explicitly enabled (§8)
> 4. **UVB avoidance**: Does not trigger UVB (§6.1.1) unless correctness is established externally

**_Explanation:_**
Conforming programs are portable across conforming implementations with compatible IDB. Programs triggering UVB may conform if correctness is established externally (documentation, testing, verification), but sacrifice verifiability guarantees.

Programs containing IFNDR violations (§6.1.3.1) are ill-formed and non-conforming, even without diagnostics.

#### 6.2.3 Well-Formedness [conformance.obligations.wellformedness]

> Implementations **MUST** accept well-formed programs and **MUST** reject ill-formed programs by issuing at least one error diagnostic, except for IFNDR violations (§6.1.3.1).
>
> Implementations **MUST NOT** produce executable artifacts for rejected programs. Implementations **MAY** produce non-executable artifacts for tooling (syntax trees, symbol tables, diagnostic metadata).
>
> Implementations **MAY** issue warnings for accepted programs without affecting conformance.

**_Explanation:_**
The executable/non-executable distinction supports tooling (IDEs, analyzers) for ill-formed programs while preventing execution.

IFNDR programs remain ill-formed and non-conforming even without diagnostics. Implementations are encouraged to diagnose IFNDR when feasible (§7.6.5).

#### 6.2.4 Conformance Dossier [conformance.obligations.dossier]

> An implementation **MUST** document all "Implementation‑defined behaviors" per target in a JSON or TOML document with stable keys: primitive_sizes, endianness, pointer_width, panic_policy, thread_model, etc. “UVB attestations” **MUST** enumerate executed UVB sites by file, span, rule, and attach proof_uri or an embedded proof hash. Conformance tools **MUST** be able to fail the build when any executed UVB site lacks an attestation entry. Documentation **SHOULD** be accessible via online docs, metadata files (JSON, TOML), or command-line flags (`--print-targets`, `--print-features`, `--print-conformance`).

**_Explanation:_**
The conformance dossier is the authoritative reference for implementation-specific behavior, enabling portability assessment and conformance testing.

**_Example - Conformance dossier structure:_**

```
Cursive Reference Implementation Conformance Dossier
Version: 1.0.0
Release Date: 2025-01-15
Vendor: Example Corporation

1. Supported Targets
  - x86_64-unknown-linux-gnu
    - Pointer size: 8 bytes
    - Endianness: little-endian
    - Default calling convention: System V AMD64 ABI
    - Maximum alignment: 16 bytes
    [... detailed target characteristics ...]

2. Implementation-Defined Behaviors
  2.1 Primitive Type Sizes
    - i8, u8: 1 byte, alignment 1
    - i16, u16: 2 bytes, alignment 2
    - i32, u32: 4 bytes, alignment 4
    [...]

  2.2 Memory Layout
    - Aggregate field ordering: declaration order preserved
    - Padding insertion: minimum required for alignment
    [...]

3. Extensions
  [Feature table listing all extensions...]

4. Diagnostic Codes
  [Complete diagnostic taxonomy...]
```

## 7. Diagnostics [diagnostics]

This chapter defines requirements for diagnostic emission, structure, and presentation. Section 7.1 establishes the scope and foundational requirements. Section 7.2 specifies the structure of diagnostic objects and their required components. Section 7.3 defines when and how diagnostics must be emitted. Section 7.4 establishes the diagnostic code system for stable identification across implementations and versions. Section 7.5 provides requirements for effective diagnostic messages. Section 7.6 describes optional enhancement features that improve the developer experience.

### 7.1 Overview [diagnostics.overview]

#### 7.1.1 Purpose and Scope [diagnostics.overview.purpose]

Diagnostics serve three purposes: (a) notifying programmers of specification violations, (b) providing actionable guidance for correction, and (c) enabling tool integration through stable, machine-readable identifiers.

Well-formed programs (§6.2.3) accepted by an implementation produce no error-severity diagnostics. Ill-formed programs trigger at least one error diagnostic, except for IFNDR violations (§6.1.3.1) which implementations need not detect. Accepted programs may produce warnings or notes without affecting conformance.

#### 7.1.2 Mandatory vs Recommended Features [diagnostics.overview.levels]

> Implementations **MUST** support the features marked "Required" in the table below. Features marked "Recommended" **SHOULD** be supported to improve programmer experience. Features marked "Optional" **MAY** be provided at the implementation's discretion.

| Feature                                   | Requirement Level | Reference |
| ----------------------------------------- | ----------------- | --------- |
| Error diagnostics for ill-formed programs | Required          | 7.3.1     |
| Diagnostic code in `K-CCC-FFNN` format    | Required          | 7.4.1     |
| Severity level (error/warning/note)       | Required          | 7.2.1     |
| Summary message                           | Required          | 7.2.2     |
| Primary source span                       | Required          | 7.2.3     |
| Compilation phase identifier              | Required          | 7.2.2     |
| Structured data for diagnostic type       | Required          | 7.2.4     |
| Multi-error reporting                     | Recommended       | 7.3.2     |
| Related source spans                      | Recommended       | 7.5.3     |
| Structured message format                 | Recommended       | 7.5.1     |
| Machine-applicable fixes                  | Recommended       | 7.6.1     |
| JSON diagnostic output                    | Recommended       | 7.6.2     |
| Extended explanations                     | Optional          | 7.6.3     |
| Diagnostic suppression                    | Optional          | 7.6.4     |
| IFNDR detection                           | Optional          | 7.6.5     |

### 7.2 Diagnostic Structure [diagnostics.structure]

This section defines the components of a diagnostic object. Each diagnostic represents a single observation about the program being compiled.

#### 7.2.1 Severity Levels [diagnostics.structure.severity]

> Every diagnostic **MUST** be assigned exactly one severity level from the following set:
>
> - **error**: The program violates a specification rule and is ill-formed. Compilation **MUST** fail (6.2.3). Implementations **MUST NOT** emit error-severity diagnostics for well-formed programs.
> - **warning**: The program is well-formed but exhibits patterns that are likely erroneous or undesirable. Compilation **MAY** succeed. Implementations **MUST NOT** treat specification violations as mere warnings.
> - **note**: Supplementary information attached to an error or warning. Notes **MUST NOT** appear without an associated primary diagnostic.

**_Explanation:_**
Severity determines compilation outcomes and diagnostic presentation. Errors prevent execution; warnings permit it. Notes provide context for other diagnostics but carry no independent semantic weight.

**_Example - Severity usage:_**

```
E-TYP-0201: type mismatch in assignment
  --> example.cursive:12:9
   |
12 |     x = "hello";
   |         ^^^^^^^ expected `i32`, found `string@View`
   |

N-TYP-0202: variable `x` declared here
  --> example.cursive:11:9
   |
11 |     let x: i32 = 42;
   |         ^

W-MEM-0301: unused variable `y`
  --> example.cursive:15:9
   |
15 |     let y = compute();
   |         ^ consider using `_y` if intentionally unused
```

#### 7.2.2 Required Payload Fields [diagnostics.structure.payload]

> Each diagnostic **MUST** include the following fields:
>
> 1. **Diagnostic code**: A stable identifier following the `K-CCC-FFNN` format (§7.4.1)
> 2. **Severity**: One of `error`, `warning`, or `note` (§7.2.1)
> 3. **Summary message**: A description of the issue suitable for display to programmers
> 4. **Primary source span**: Location information identifying where the diagnostic applies (§7.2.3)
> 5. **Compilation phase**: The phase that detected the issue
> 6. **Structured data**: Type-specific fields relevant to the diagnostic code (§7.2.4)

> Implementations **MUST** use one of the following phase identifiers: `parsing`, `comptime_execution`, `type_checking`, `code_generation`, `lowering`. Implementations **MAY** define additional phases using identifiers prefixed with `x-` (e.g., `x-borrow-checking`, `x-optimization`). Custom phase identifiers **SHOULD** be documented in the conformance dossier (§6.2.4).

**_Example - Phase usage:_**

```
Lexical error detected in `parsing` phase:
  error[E-SRC-0301]: unterminated string literal

Type error detected in `type_checking` phase:
  error[E-TYP-0305]: cannot apply operator `+` to `bool`

Contract violation detected in custom phase:
  error[E-GRN-0401]: missing grant `io::fs`
  phase: x-contract-checking
```

#### 7.2.3 Source Spans and Locations [diagnostics.structure.spans]

> The primary source span **MUST** include:
>
> 1. **File path**: Absolute or workspace-relative path to the source file
> 2. **Start position**: Byte offset or line/column pair indicating where the issue begins
> 3. **End position**: Byte offset or line/column pair indicating where the issue ends
> 4. **Offset mode**: Explicit documentation of whether positions use UTF-8 byte offsets or UTF-16 code unit offsets

> Implementations **MUST** document their chosen offset mode and **MUST** use it consistently across all diagnostics. Implementations **MUST NOT** mix offset modes within a single compilation.

> When representing positions as line/column pairs, implementations **MUST** document whether:
>
> - Lines and columns are 0-indexed or 1-indexed
> - Columns count bytes, code points, or grapheme clusters
> - Tab characters contribute 1 or multiple columns

**_Explanation:_**
Source spans enable editors, IDEs, and linters to highlight relevant code. Explicit documentation of position semantics ensures tools can interpret spans correctly. Different editors use different conventions; implementations must choose one and document it clearly.

**_Example - Position documentation:_**

```
Implementation: Cursive Reference Compiler 1.0
Offset mode: UTF-8 byte offsets (0-indexed)
Line/column mode: 1-indexed lines, 1-indexed UTF-8 byte columns
Tab width: 1 column (literal tab character)

Example span:
  File: src/main.cursive
  Start: byte 324 (line 12, column 5)
  End: byte 329 (line 12, column 10)
```

#### 7.2.4 Structured Data [diagnostics.structure.data]

> Each diagnostic code **MUST** define a structured data schema documenting the type-specific fields that accompany diagnostics of that code. Implementations **MUST** document these schemas in the conformance dossier (§6.2.4).

> Common structured fields include:
>
> - Type mismatches: `expected_type`, `found_type`
> - Permission violations: `required_permission`, `actual_permission`
> - Missing grants: `required_grants`, `available_grants`
> - Undefined names: `name`, `similar_names` (suggestions)
> - Exhaustiveness failures: `missing_patterns`

**_Explanation:_**
Structured data enables machine processing of diagnostics. Tools can extract types, permissions, and other semantic information without parsing human-readable messages. This supports automated fixes, IDE integration, and diagnostic filtering.

**_Example - Structured data for type mismatch:_**

```json
{
  "code": "E-TYP-0201",
  "severity": "error",
  "message": "type mismatch in assignment",
  "span": { "file": "src/main.cursive", "start": 324, "end": 329 },
  "phase": "type_checking",
  "structured_data": {
    "expected_type": "i32",
    "found_type": "string@View",
    "binding_name": "x",
    "binding_location": { "file": "src/main.cursive", "start": 298, "end": 299 }
  }
}
```

#### 7.2.5 Related Diagnostics and Notes [diagnostics.structure.related]

> Diagnostics **MAY** include related spans that reference relevant declarations, prior definitions, or contextual information. Related spans **MUST** include the same location fields as primary spans (§7.2.3) plus a descriptive label.

> Note-severity diagnostics **MUST** be associated with a primary error or warning. Notes **SHOULD** provide supplementary information such as:
>
> - Locations of conflicting declarations
> - Relevant type definitions
> - Grant availability locations
> - Suggestions for related configuration changes

**_Explanation:_**
Complex violations often involve multiple source locations. Related spans and notes help programmers understand the full context without repeating the primary diagnostic. For example, a "conflicting definition" error benefits from showing both the original and duplicate declarations.

**_Example - Related spans and notes:_**

```
error[E-SRC-1501]: redeclaration of binding `x` without `shadow` keyword
  --> example.cursive:23:9
   |
23 |     let x = 20;
   |         ^ redeclared here
   |
note[N-SRC-1502]: previous declaration of `x`
  --> example.cursive:20:9
   |
20 |     let x = 10;
   |         ^ originally declared here
   |
note[N-SRC-1503]: use `shadow` keyword to explicitly shadow
   |
   = help: let shadow x = 20;
```

### 7.3 Emission Requirements [diagnostics.emission]

This section defines when diagnostics must be issued, how implementations should handle multiple errors, and ordering requirements.

#### 7.3.1 When Diagnostics Must Be Issued [diagnostics.emission.when]

> Implementations **MUST** issue at least one error-severity diagnostic for each distinct violation of:
>
> 1. Lexical rules (see Part II, §9)
> 2. Syntactic rules (see Part II, §8-§12)
> 3. Static-semantic rules (see Parts III-VI)
> 4. Module system rules (see Part II, §10)

> Implementations **MUST NOT** issue error diagnostics for well-formed programs. Implementations **MAY** issue warning-severity diagnostics for well-formed programs exhibiting patterns deemed undesirable.

> Violations classified as Ill-Formed, No Diagnostic Required (IFNDR) (§6.1.3.1) are exempt from mandatory diagnostic requirements. Implementations **MAY** detect and diagnose IFNDR violations (§7.6.5) but are not required to do so.

**_Explanation:_**
The "at least one error" requirement permits implementations to emit multiple diagnostics for a single underlying violation (e.g., a primary error plus contextual notes). The phrase "distinct violation" means implementations should not issue redundant diagnostics for the same specification breach at the same location.

Well-formed programs are conforming (§6.2.2); implementations must accept them without errors. Warnings are advisory and do not affect conformance.

**_Example - Error issuance:_**

```cursive
// Ill-formed: type mismatch (§TODO)
let x: i32 = "hello";  // MUST issue E-TYP-XXXX

// Well-formed but potentially undesirable
let y = compute();     // MAY issue W-MEM-XXXX (unused variable)

// IFNDR: termination depends on unsolvable halting problem
// (Implementation NEED NOT diagnose, but MAY if feasible)
procedure f() { loop { if undecidable_predicate() { break; } } }
```

#### 7.3.2 Error Recovery and Continuation [diagnostics.emission.recovery]

> Implementations **SHOULD** continue analysis after detecting errors to report additional violations when feasible. Implementations **MAY** halt analysis when error recovery requires non-deterministic choices about syntactic structure or semantic intent.

> Implementations **SHOULD NOT** issue diagnostics for violations that would not occur if a previously reported error were corrected, except when both violations represent independent specification breaches.

**_Explanation:_**
Multi-error reporting improves programmer productivity by surfacing multiple issues in a single compilation. However, error recovery is challenging: after a syntax error, the parser may be unable to determine the intended structure. Implementations may halt when recovery becomes ambiguous.

Cascading errors—where one violation triggers spurious diagnostics elsewhere—should be suppressed. For example, if a type is undefined, subsequent uses of that type should not generate additional "unknown type" errors.

**_Example - Error recovery:_**

```cursive
procedure example() {
    let x: UnknownType = 42;  // Error 1: undefined type
    let y = x + 1;            // Don't issue "type of x unknown" (cascading)

    let z: i32 = "hello";     // Error 2: independent violation, should report
}

// Both Error 1 and Error 2 should be reported, but cascading errors
// caused by UnknownType should be suppressed.
```

> Implementations **SHOULD** document any limits on the number of diagnostics reported and **SHOULD** provide configuration options to adjust these limits. When a limit is reached, implementations **MUST** issue a diagnostic or message stating that the diagnostic limit was reached and that unreported errors may exist.

**_Example - Diagnostic limit message:_**

```
error: aborting due to 100 previous errors
note: diagnostic limit reached; additional errors may exist but were not reported
note: use `--error-limit N` to adjust the diagnostic limit
```

#### 7.3.3 Multi-Unit Compilation [diagnostics.emission.multiunit]

> When one or more compilation units fail static-semantic validation, implementations **MAY** continue analyzing remaining units to emit diagnostics for all units. Implementations **MUST NOT** produce executable artifacts, intermediate object files, or proceed to execution when any compilation unit fails validation.

> Implementations **SHOULD** include the validation status (accepted or rejected) for each compilation unit in diagnostic output to help programmers identify which files passed validation.

**_Explanation:_**
Multi-unit projects may contain errors in several files. Continuing analysis across units surfaces all issues in a single compilation, avoiding iterative fix-recompile cycles. However, executable output must not be generated if any unit is ill-formed.

**_Example - Multi-unit summary:_**

```
Compilation summary:
  src/main.cursive: accepted
  src/utils.cursive: rejected (2 errors)
  src/config.cursive: rejected (1 error)

Build failed due to errors in 2 compilation units.
```

#### 7.3.4 Ordering and Determinism [diagnostics.emission.ordering]

> When multiple errors exist in a program, implementations **MAY** report them in any order. Implementations **SHOULD** report errors in phase order (lexical errors before syntactic errors before semantic errors) when error locations are independent.

> Implementations **SHOULD** report errors in source order (earlier in the file before later) within a single phase when feasible, but this is not required for conformance.

> Implementations **MUST** produce deterministic diagnostic output for identical source inputs and configuration. Running the same compilation twice **MUST** produce diagnostics in the same order with identical codes, messages, and spans.

**_Explanation:_**
Deterministic output is critical for reproducible builds, testing, and tool integration. While implementations may report errors in any order, determinism ensures that repeated compilations produce identical results.

### 7.4 Diagnostic Code System [diagnostics.codes]

This section establishes the diagnostic code format, stability guarantees, and taxonomic structure that enables stable cross-references in documentation, tooling, and error suppression.

#### 7.4.1 Code Format Specification [diagnostics.codes.format]

> Every diagnostic code **MUST** follow the pattern `K-CCC-FFNN` where:
>
> - **K**: Severity indicator — `E` (error), `W` (warning), or `N` (note)
> - **CCC**: Three-letter category code (§7.4.3)
> - **FF**: Two-digit feature bucket (§7.4.4)
> - **NN**: Two-digit sequence number within the bucket (01–99)

> Examples: `E-SRC-0301` (error in source ingestion), `W-MEM-0205` (warning in memory category), `N-TYP-1003` (note for type system).

**_Explanation:_**
The structured code format enables:

- **Stable identification**: Codes remain constant across compiler versions (§7.4.2)
- **Categorical grouping**: Related diagnostics share a category prefix
- **Fine-grained suppression**: Tooling can suppress specific codes without suppressing entire categories
- **Documentation cross-references**: User guides and internal docs reference codes unambiguously

The severity prefix `K` allows tools to identify error/warning/note at a glance. The category `CCC` groups diagnostics by language subsystem. The feature bucket `FF` organizes codes within categories. The sequence number `NN` uniquely identifies each diagnostic.

#### 7.4.2 Stability Guarantees [diagnostics.codes.stability]

> Diagnostic codes **MUST** remain stable within a major language version. Implementations **MUST NOT** assign a diagnostic code to a different violation or rule within the same major version.

> When transitioning between major language versions, implementations **MAY** retire codes or repurpose them for new diagnostics. The conformance dossier (§6.2.4) **SHOULD** document code changes between versions, including retired codes and their replacements.

> Implementations **MUST** document all diagnostic codes, their meanings, and triggering conditions in user-facing documentation accessible via online docs, metadata files, or command-line flags (e.g., `--explain E-TYP-0201`).

**_Explanation:_**
Major version boundaries permit necessary changes while preserving stability within a version family.

**_Example - Code stability across versions:_**

```
Cursive 1.0–1.9: E-TYP-0201 = "type mismatch in assignment"
Cursive 2.0+:    E-TYP-0201 = retired, replaced by E-TYP-0210 (refined message)

Conformance dossier documents:
  "E-TYP-0201: Retired in 2.0.0, replaced by E-TYP-0210 with enhanced
   structured data for generic type arguments."
```

#### 7.4.3 Category Taxonomy [diagnostics.codes.categories]

> The canonical category set for `CCC` **MUST** include the following codes. Implementations **MUST NOT** issue diagnostics whose category does not match the category scope defined below. Implementations **MAY** propose new categories through specification revision.

| Code  | Scope                                                                                                                    |
| ----- | ------------------------------------------------------------------------------------------------------------------------ |
| `SRC` | Source ingestion: translation pipeline, lexical analysis, parsing, module discovery, import resolution, name scaffolding |
| `TYP` | Type system: type formation, declarations, type checking, permissions, variance, generics, witnesses, behaviors          |
| `MEM` | Memory model: ownership, regions, pointers, provenance, escape analysis, concurrency guarantees, RAII                    |
| `EXP` | Expressions and control flow: operators, statements, pattern matching, comptime usage, code generation, reflection       |
| `GRN` | Grants and contracts: effect checking, sequents, preconditions, postconditions, verification modes, grant propagation    |
| `FFI` | Interoperability: foreign function interface, ABI, calling conventions, representation attributes, linkage               |
| `DIA` | Diagnostics and tooling: diagnostic system itself, tooling hooks, reporting schemas, metadata formats                    |
| `CNF` | Conformance: meta rules, conformance targets, versioning, edition migration, extension policy, feature flags             |

**_Explanation:_**
The `DIA` category is meta: diagnostics about diagnostics (e.g., invalid diagnostic suppression syntax). The `CNF` category covers specification compliance and extension usage.

#### 7.4.4 Feature Buckets [diagnostics.codes.buckets]

> Within each category, the two-digit feature bucket `FF` organizes diagnostics by feature area. Feature buckets **MUST** be assigned systematically to avoid collisions and ensure room for expansion.

> The complete feature bucket taxonomy for all diagnostic categories **MUST** be defined in an appendix or conformance dossier. Implementations **MUST** follow the bucket assignments specified therein.

**_Explanation:_**
Feature buckets provide finer granularity than categories. For example, within `TYP` (type system), bucket `01` might cover primitive types, bucket `02` structs/records, bucket `03` enums, bucket `04` unions, etc.

Systematic assignment prevents collisions: if bucket `TYP-02` is reserved for records, implementations must not use `E-TYP-0201` for an unrelated enum diagnostic.

**_Example - Feature bucket assignment (illustrative):_**

```
Category SRC (Source ingestion):
  Bucket 01: UTF-8 and encoding (E-SRC-0101–0199)
  Bucket 02: Lexical analysis (E-SRC-0201–0299)
  Bucket 03: Parsing (E-SRC-0301–0399)
  Bucket 04: Module discovery (E-SRC-0401–0499)
  ...

Category TYP (Type system):
  Bucket 01: Primitive types (E-TYP-0101–0199)
  Bucket 02: Composite types (E-TYP-0201–0299)
  Bucket 03: Generics (E-TYP-0301–0399)
  ...
```

> Appendix TODO **MUST** provide the complete bucket taxonomy with examples for each bucket.

#### 7.4.5 Code Examples [diagnostics.codes.examples]

This subsection provides concrete examples of diagnostic codes across categories.

**_Source ingestion (SRC):_**

- `E-SRC-0101`: Invalid UTF-8 sequence
- `E-SRC-0301`: Unterminated string literal
- `E-SRC-0401`: Unexpected end-of-file
- `W-SRC-1105`: Module name may collide on case-insensitive filesystems

**_Type system (TYP):_**

- `E-TYP-0201`: Type mismatch in assignment
- `E-TYP-0305`: Cannot apply operator to operand types
- `E-TYP-0420`: Non-exhaustive match patterns
- `N-TYP-0421`: Missing pattern suggestion

**_Memory model (MEM):_**

- `E-MEM-0101`: Use of moved value
- `E-MEM-0205`: Pointer escapes region lifetime
- `W-MEM-0301`: Unused variable binding
- `N-MEM-0302`: Consider using underscore prefix for intentionally unused bindings

**_Expressions (EXP):_**

- `E-EXP-0101`: Cannot assign to immutable binding
- `E-EXP-0205`: Break outside loop
- `E-EXP-0310`: Pipeline stage type mismatch
- `W-EXP-0405`: Unreachable code

**_Grants and contracts (GRN):_**

- `E-GRN-0101`: Missing required grant
- `E-GRN-0205`: Precondition not satisfied
- `W-GRN-0301`: Unused grant in sequent
- `N-GRN-0302`: Grant available from enclosing scope

**_Foreign function interface (FFI):_**

- `E-FFI-0101`: Unsupported calling convention for target
- `E-FFI-0205`: Invalid `repr` attribute combination
- `W-FFI-0301`: Implicit padding in `repr(C)` struct
- `N-FFI-0302`: Reorder fields to eliminate padding

**_Diagnostics (DIA):_**

- `E-DIA-0101`: Invalid diagnostic code in suppression attribute
- `W-DIA-0201`: Suppression attribute has no effect (no diagnostic emitted)

**_Conformance (CNF):_**

- `E-CNF-0101`: Unknown feature identifier
- `E-CNF-0201`: Extension usage in strict conformance mode
- `W-CNF-0301`: Conflicting feature flags

### 7.5 Message Quality [diagnostics.quality]

This section establishes requirements for effective diagnostic messages that guide programmers toward solutions. While implementations have flexibility in presentation, messages should prioritize clarity and actionability.

#### 7.5.1 Message Structure Requirements [diagnostics.quality.structure]

> Diagnostic messages **SHOULD** follow a three-part structure:
>
> 1. **State the failure**: Begin with a concise description of what went wrong (e.g., "type mismatch", "undefined variable", "non-exhaustive match")
> 2. **Explain the cause**: Identify why the failure occurred (e.g., "expected `i32`, found `string@View`", "no binding named `x` in scope", "missing variants: `A`, `B`")
> 3. **Suggest the fix**: Provide actionable guidance for remediation (e.g., "convert to integer using `.parse()`", "declare `x` before use", "add missing cases or a wildcard `_` pattern")

**_Example - Structured message:_**

```
error[E-TYP-0201]: type mismatch in assignment
  --> example.cursive:12:9
   |
12 |     x = "hello";
   |         ^^^^^^^ expected `i32`, found `string@View`
   |
   = note: variable `x` has type `i32` declared at line 11
   = help: convert the string to an integer: x = "hello".parse::<i32>()?
```

#### 7.5.2 Clarity and Actionability [diagnostics.quality.clarity]

> Messages **SHOULD** use precise terminology from this specification (e.g., "binding" rather than "variable" when referring to `let`/`var` declarations, "procedure" rather than "function", "sequent" rather than "contract clause").

> Messages **SHOULD** avoid jargon and abbreviations not defined in the specification. When technical terms are necessary, messages **SHOULD** provide brief explanations or references to relevant specification sections.

> Messages **MUST NOT** rely solely on color, styling, or formatting to convey required diagnostic information (§7.2.2). Plain-text output **MUST** include all mandatory fields.

**_Explanation:_**
Diagnostics must remain useful in monochrome terminals, screen readers, and automated processing pipelines. Color and styling enhance but must not replace textual content.

**_Example - Accessible diagnostic:_**

```
// Good: Fully textual, screen-reader friendly
error[E-GRN-0101]: missing required grant `io::fs`
  --> example.cursive:5:5
   |
5  |     let file = File::open("data.txt");
   |                ^^^^^^^^^^^^^^^^^^^^^^^ requires grant `io::fs`
   |
   = note: the `File::open` procedure requires grant `io::fs`
   = help: add `io::fs` to the grant list in the procedure's sequent

// Bad: Relies on color alone (inaccessible)
error[E-GRN-0101]: missing grant
  --> example.cursive:5:5
   |
5  |     let file = File::open("data.txt");
   |                ^^^^^^^^^^^^^^^^^^^^^^^
   |
   = note: this operation requires a grant (shown in red above)
```

#### 7.5.3 Context and Related Information [diagnostics.quality.context]

> Diagnostics **SHOULD** include related source spans (§7.2.5) that reference:
>
> - Original declarations for redeclaration errors
> - Type definitions for type mismatch errors
> - Grant availability locations for missing grant errors
> - Relevant imports for name resolution failures

> Diagnostics **SHOULD** suggest similar names when a name lookup fails and candidates exist (e.g., "no binding `lenght` found; did you mean `length`?").

**_Example - Contextual diagnostic:_**

```
error[E-SRC-1501]: redeclaration of binding `counter` without `shadow` keyword
  --> example.cursive:45:9
   |
45 |     let counter = 0;
   |         ^^^^^^^ redeclared here
   |
note[N-SRC-1502]: previous declaration of `counter`
  --> example.cursive:40:9
   |
40 |     let counter = initial_value();
   |         ^^^^^^^ originally declared here
   |
   = help: use `let shadow counter = 0;` to explicitly shadow the previous binding
```

**_Example - Name suggestion:_**

```
error[E-SRC-1602]: undefined binding `lenght`
  --> example.cursive:23:14
   |
23 |     if lenght > 0 {
   |        ^^^^^^^ not found in this scope
   |
   = help: a binding with a similar name exists: `length`
```

#### 7.5.4 Examples and Anti-Patterns [diagnostics.quality.examples]

This subsection illustrates effective diagnostic design through examples and contrasts with anti-patterns.

**_Example 1: Good diagnostic (comprehensive, actionable)_**

```
error[E-TYP-0420]: non-exhaustive match patterns
  --> example.cursive:18:5
   |
18 | /     match result {
19 | |         Ok(val) => process(val),
20 | |         Err(e) if e.is_recoverable() => retry(),
21 | |     }
   | |_____^ missing patterns: `Err(_)` (non-recoverable errors)
   |
   = note: the matched type is `Result<T, E>`
   = help: add a catch-all pattern: `Err(_) => handle_error()`
   = help: or add a pattern for non-recoverable errors: `Err(e) => panic(e)`
```

**_Example 2: Bad diagnostic (vague, unhelpful)_**

```
error: match error
  --> example.cursive:18:5
   |
18 |     match result {
   |     ^^^^^^^^^^^^^ incomplete
```

**_Example 3: Good diagnostic (permission violation)_**

```
error[E-MEM-0101]: cannot mutate through `const` binding
  --> example.cursive:30:5
   |
30 |     data.push(42);
   |     ^^^^^^^^^^^^^ cannot mutate `data`
   |
note[N-MEM-0102]: binding `data` has `const` permission
  --> example.cursive:28:9
   |
28 |     let data: const Vec<i32> = vec![1, 2, 3];
   |         ^^^^ declared with `const` permission here
   |
   = note: `const` bindings allow reads but prohibit mutation
   = help: change to `shared` or `unique` if mutation is required:
           let data: unique Vec<i32> = vec![1, 2, 3];
```

**_Example 4: Bad diagnostic (misleading, lacks context)_**

```
error: cannot modify variable
  --> example.cursive:30:5
   |
30 |     data.push(42);
   |     ^^^^
```

### 7.6 Optional Enhancements [diagnostics.enhancements]

This section describes optional features that significantly improve developer experience. While not required for conformance, implementations are encouraged to provide these enhancements.

#### 7.6.1 Machine-Applicable Fixes [diagnostics.enhancements.fixes]

> Implementations **SHOULD** emit machine-applicable fix-it hints for errors with unambiguous corrections. Fix-its **MUST** specify:
>
> - The source span to be replaced
> - The replacement text
> - A human-readable description of the fix

> Fix-its **SHOULD** be provided only when the correction is syntactically and semantically unambiguous and does not depend on programmer intent.

**_Example - Machine-applicable fix:_**

```json
{
  "code": "E-SRC-1501",
  "message": "redeclaration without `shadow` keyword",
  "span": { "file": "src/main.cursive", "start": 450, "end": 453 },
  "fixes": [
    {
      "description": "Insert `shadow` keyword",
      "span": { "file": "src/main.cursive", "start": 446, "end": 446 },
      "replacement": "shadow "
    }
  ]
}
```

Applied fix transforms:

```cursive
let counter = 0;
```

Into:

```cursive
let shadow counter = 0;
```

#### 7.6.2 Machine-Readable Formats [diagnostics.enhancements.formats]

> Implementations **SHOULD** support JSON-structured diagnostic output for tool integration. JSON output **MUST** include all mandatory fields (§7.2.2) and **SHOULD** include optional enhancements (related spans, fix-its, structured data).

> Implementations **MAY** support additional formats (e.g., XML, Protocol Buffers, LSP Diagnostic format) but **SHOULD** document format schemas and maintain stability within major versions.

**_Example - JSON diagnostic format:_**

```json
{
  "diagnostics": [
    {
      "code": "E-TYP-0201",
      "severity": "error",
      "message": "type mismatch in assignment",
      "span": {
        "file": "src/main.cursive",
        "start": { "line": 12, "column": 9 },
        "end": { "line": 12, "column": 16 }
      },
      "phase": "type_checking",
      "structured_data": {
        "expected_type": "i32",
        "found_type": "string@View",
        "binding_name": "x"
      },
      "related": [
        {
          "label": "variable declared here",
          "span": {
            "file": "src/main.cursive",
            "start": { "line": 11, "column": 9 },
            "end": { "line": 11, "column": 10 }
          }
        }
      ]
    }
  ],
  "summary": {
    "error_count": 1,
    "warning_count": 0,
    "note_count": 0
  }
}
```

#### 7.6.3 Extended Explanations [diagnostics.enhancements.explanations]

> Implementations **MAY** provide extended explanations for diagnostic codes accessible via command-line flags (e.g., `--explain E-TYP-0201`) or online documentation. Extended explanations **SHOULD** include:
>
> - Detailed description of the rule or requirement violated
> - Common causes of the diagnostic
> - Multiple examples showing violations and corrections
> - References to relevant specification sections

**_Example - Extended explanation structure:_**

```
$ cursive-compiler --explain E-TYP-0201

E-TYP-0201: Type mismatch in assignment

Description:
  [Detailed description of the rule violated]

Common causes:
  • [Cause 1]
  • [Cause 2]

Examples:
  [Code examples showing violations and corrections]

Specification references:
  • [Relevant specification sections]

Related diagnostics:
  • [Related error codes]
```

#### 7.6.4 Diagnostic Suppression [diagnostics.enhancements.suppression]

> Implementations **MAY** support diagnostic suppression via attributes allowing programmers to silence specific warnings or notes. Suppression **MUST NOT** silence error-severity diagnostics.

> When supported, suppression attributes **SHOULD** use the syntax:
>
> ```cursive
> #[allow(diagnostic_code)]
> ```

> Suppression **MAY** be scoped to:
>
> - Individual declarations (procedures, types, bindings)
> - Blocks or statement sequences
> - Entire modules

> Implementations **SHOULD** warn when suppression attributes have no effect (no diagnostic of the specified code was emitted in the suppressed scope).

**_Explanation:_**
Errors must not be suppressible: they represent specification violations, and silencing them creates ill-formed programs.

**_Example - Diagnostic suppression:_**

```cursive
// Suppress unused variable warning for intentionally unused binding
#[allow(W-MEM-0301)]
procedure example() {
    let result = expensive_computation();  // Computed for side effects
}

// Suppress at module level
#![allow(W-MEM-0301)]
module legacy {
    // All unused variable warnings suppressed in this module
}
```

**_Example - Suppression warning:_**

```
warning[W-DIA-0201]: suppression attribute has no effect
  --> example.cursive:15:3
   |
15 | #[allow(W-MEM-0301)]
   | ^^^^^^^^^^^^^^^^^^^^ no diagnostic W-MEM-0301 emitted in this scope
   |
   = note: the binding is used at line 17, so no unused warning is generated
   = help: remove this attribute if it is no longer needed
```

#### 7.6.5 IFNDR Detection [diagnostics.enhancements.ifndr]

> Implementations **SHOULD** diagnose IFNDR violations (§6.1.3.1) when detection does not require exponential-time algorithms or solving undecidable problems. Implementations **MAY** use heuristics, conservative approximations, or optional extended analysis modes to detect IFNDR.

> When IFNDR violations are detected, implementations **SHOULD** issue error diagnostics using the appropriate diagnostic code for the underlying rule violation, noting in the message that the violation is IFNDR and may not be detected consistently.

**_Explanation:_**
IFNDR violations are ill-formed programs that implementations need not diagnose due to computational infeasibility. However, implementations benefit users by detecting them when feasible. Noting the IFNDR classification helps programmers understand that the diagnostic may not appear in all contexts or implementations.

**_Example - IFNDR detection:_**

```
error[E-TYP-0525]: potential non-termination in type unification (IFNDR)
  --> example.cursive:42:10
   |
42 |     type RecursiveAlias = Vec<RecursiveAlias>;
   |          ^^^^^^^^^^^^^^ recursive type alias may not terminate
   |
   = note: this is an IFNDR violation; detection is not guaranteed across
           all implementations or compilation contexts
   = note: this implementation uses conservative cycle detection heuristics
   = help: break the recursion using a nominal type (struct or enum)
```

## 8. Extension Requirements [extensions]

### 8.1 Extension Model [extensions.model]

#### 8.1.1 Opt-in requirement [extensions.model.optin]

> Implementations **MAY** provide extensions only if those extensions are opt-in and not enabled by default for code that claims full conformance. A conforming implementation **MUST** provide at least one mode that enforces all mandatory diagnostics while rejecting extension-dependent semantics changes (see §8.3).

#### 8.1.2 Semantic stability [extensions.model.stability]

> Implementations **MUST** ensure that a conforming program compiles and behaves identically with extensions disabled. Extensions **MUST NOT** alter the meaning of conforming programs: when extension features are not explicitly invoked, program behavior **MUST** remain unchanged.

#### 8.1.3 Feature-test visibility [extensions.model.visibility]

> When extensions are enabled, they **MUST** be discoverable via the portable feature-test mechanism defined in §8.4.2. All extensions **MUST** be documented in the implementation's conformance dossier (§6.2.4), including their stability classification (Stable, Preview, or Experimental per §10.2.1).

#### 8.1.4 Unverifiable-behavior prohibition [extensions.model.uvb]

> Extensions **MUST NOT** introduce new unverifiable behavior except where this specification explicitly permits implementation-defined behavior that may lead to unverifiable behavior when misused. Extensions **MUST NOT** expand the set of operations classified as unverifiable behavior beyond what this specification permits.

### 8.2 Forbidden Extension Classes [extensions.forbidden]

The following extension classes are explicitly prohibited to preserve the guarantees of conforming code:

#### 8.2.1 Implicit conversions [extensions.forbidden.conversions]

> Extensions **MUST NOT** add implicit conversions, including numeric promotions, boolean coercions, or provenance-altering pointer conversions. All type conversions introduced by extensions **MUST** require explicit syntax.

#### 8.2.2 Macro and textual substitution [extensions.forbidden.macros]

> Extensions **MUST NOT** add macro systems, textual-substitution mechanisms, or text-based code generation that operates before lexical analysis. Code transformation **MUST** operate on the abstract syntax tree or later representations.

#### 8.2.3 Zero-cost abstraction violations [extensions.forbidden.zerocost]

> Extensions **MUST NOT** insert default runtime checks that change zero-cost behavior. Dynamic contract checks, bounds checks, and similar runtime validations **MUST** remain opt-in via explicit attributes, build flags, or verification modes (see Part VI, §16.5).

#### 8.2.4 Permission lattice violations [extensions.forbidden.permissions]

> Extensions **MUST NOT** violate the permission lattice defined in Part IV, §17.2 or introduce hidden interior mutability through `const` bindings. The permission system's guarantees **MUST** remain intact.

#### 8.2.5 Region and pointer safety [extensions.forbidden.regions]

> Extensions **MUST NOT** relax region-escape rules (see Part IV, §18.1.2) or pointer-validity requirements (see Part III, §14.1.4). Extensions **MAY** provide additional pointer types with different safety properties, but they **MUST NOT** weaken the guarantees of standard pointer types.

#### 8.2.6 Evaluation order [extensions.forbidden.evaluation]

> Extensions **MUST NOT** change evaluation order (see Part V, §20.1.2) or short-circuit semantics (see Part V, §22.4) for standard language constructs. Extensions **MAY** introduce new constructs with explicitly documented evaluation behavior.

#### 8.2.7 ABI stability [extensions.forbidden.abi]

> Extensions **MUST NOT** modify the meaning of `[[repr(C)]]` or other ABI-related attributes relative to the platform C ABI. Extensions **MAY** provide additional representation attributes, but standard attributes **MUST** retain their specified meaning.

### 8.3 Conformance Modes [extensions.modes]

> Implementations **MUST** provide at least one **Strict** conformance mode that rejects code relying on extensions and treats preview/experimental features as unavailable. Strict mode **MUST** enforce all mandatory diagnostics and produce behavior consistent with this specification without extension influence.

> A **Permissive** mode **MAY** accept extensions provided it reports their use through diagnostics or logging mechanisms. Permissive mode **MUST** always offer a command-line or configuration switch that produces strictly conforming output by disabling extensions.

### 8.4 Extension Versioning [extensions.versioning]

> Extensions evolve over time. When an extension's behavior changes incompatibly, implementations **MUST** assign a new feature identifier. The feature identifier **MAY** include version information (e.g., `vendor.feature.v2`). Implementations **SHOULD** support multiple versions of an extension simultaneously when feasible, allowing gradual migration.

> Implementations **MUST** document in the conformance dossier (§6.2.4):
>
> - The stability classification of each extension (Stable, Preview, Experimental)
> - The language/compiler version when each extension became available
> - Migration guidance when extension semantics change incompatibly

#### 8.4.1 Feature compatibility [extensions.versioning.compatibility]

> When multiple extensions are enabled via `--enable-feature` or manifest declarations, implementations **MUST** validate that the enabled features are mutually compatible. If two features fundamentally conflict, implementations **MUST** raise error diagnostic E-CNF-0301 identifying the conflicting features.

> Implementations **SHOULD** document known feature interactions and compatibility constraints in the conformance dossier. When features interact in non-obvious ways, the documentation **SHOULD** provide examples and guidance.

#### 8.4.2 Portable feature-test mechanism [extensions.versioning.featuretest]

> Every implementation **MUST** expose the `feature` intrinsic namespace with the following interface for testing extension availability:

**feature::supports**

> `feature::supports(id: string@View): bool`
>
> This intrinsic is pure, grant-free, and available at both runtime and comptime. The result **MUST** be constant for a given target triple, language version, and compiler configuration. When `id` references an unknown feature, the intrinsic **MUST** return `false` and emit diagnostic W-CNF-0101 to alert programmers of potential typos or version mismatches.

**feature::require**

> `feature::require(id: string@View)`
>
> This intrinsic is a pure, grant-free helper that **MUST** raise error diagnostic E-CNF-0102 if `feature::supports(id)` would return `false`. This enables compile-time feature guards without requiring bespoke diagnostic handling.

**Feature identifier format**

> Feature identifiers **MUST** follow the grammar `feature-id ::= ident ("." ident)*` where `ident ::= [a-z][a-z0-9_]*`. Implementations **MUST** document every extension using these identifiers. Implementations **MUST** accept feature identifiers via the repeatable CLI flag `--enable-feature feature-id`. Attempting to enable an undocumented feature identifier **MUST** raise error diagnostic E-CNF-0103.

**Comptime and tooling support**

> Both intrinsics **MUST** be callable from comptime code. Their return values **MUST** be treated as compile-time constants so conditional compilation remains deterministic. Tooling interfaces (LSP, JSON diagnostics) **MUST** expose the list of enabled feature identifiers for each compilation unit so editors can provide consistent gating and completion.

#### 8.4.3 Conformance-mode control surface [extensions.versioning.control]

> Implementations **MUST** provide two user-facing controls for selecting the conformance mode defined in §8.3:

**Command-line interface**

> Every compiler driver **MUST** accept `--conformance=strict` or `--conformance=permissive`. When the flag is omitted, implementations **MUST** default to `strict` mode.

**Project manifest**

> The project manifest (see Part II, §10.1.5) **MUST** accept the key `build.conformance` with values `strict` or `permissive`. When both manifest and CLI specify a conformance mode, the CLI **MUST** take precedence. If they disagree, implementations **MUST** emit warning diagnostic W-CNF-0201 to alert the programmer.

**Mode enforcement**

> In Strict mode, encountering any extension that is not explicitly enabled via `--enable-feature` or manifest feature lists **MUST** raise error diagnostic E-CNF-0202. In Permissive mode, the same situation **MUST** emit warning diagnostic W-CNF-0203 and continue compilation.

**Introspection support**

> Implementations **MUST** expose the intrinsic `compiler::conformance_mode(): ConformanceMode` (pure, grant-free) returning an enum with literals `Strict` and `Permissive`. This enables code generators and tooling to tailor behavior without re-parsing command-line flags.

## 9. Version Compatibility [versions]

### 9.1 Backward Compatibility [versions.backward]

#### 9.1.1 Stability Guarantees [versions.backward.stability]

> Future versions of this specification **SHOULD** preserve the behavior of previously conforming programs. When behavioral changes are unavoidable (e.g., to fix specification defects or address security issues), this specification **SHOULD** provide both a migration path and a deprecation window to allow programmers time to adapt.

> Implementations **SHOULD** document any divergence from previous specification versions in their conformance dossier (§6.2.4), including the version where changes occurred and recommended migration strategies.

#### 9.1.2 Deprecated Feature Diagnostics [versions.backward.deprecated]

> Features may be marked as deprecated in later versions of this specification. Deprecated features remain part of the specification and **MUST** continue to function as specified until formally removed in a future major version.

> When a feature is deprecated, implementations **SHOULD** provide warning-severity diagnostics when deprecated features are used, provided relevant warning flags are enabled. Deprecation diagnostics **SHOULD** identify the deprecated feature, explain why it was deprecated, and suggest alternative approaches when available.

> The specification **MUST** provide at least one major version cycle (or equivalent time period) between deprecation and removal to allow ecosystem migration.

## 10. Conformance Validation [validation]

### 10.1 Testing Requirements [validation.testing]

#### 10.1.1 Test Suite Usage [validation.testing.suite]

> If an official conformance test suite accompanies this specification, implementations **SHOULD** pass all applicable tests. However, implementations **MUST NOT** treat test suite passage as a substitute for satisfying every normative requirement in this specification: the suite validates common scenarios but cannot exhaustively test all specification requirements.

> When a conformance suite is bundled with the implementation, the conformance dossier (§6.2.4) **SHOULD** document how to execute the suite in both strict and permissive conformance modes (§8.3) and how to interpret results.

#### 10.1.2 Testable Criteria [validation.testing.criteria]

> Implementations **MUST** ship self-tests covering at least the following areas:
>
> - Lexical analysis and syntax parsing
> - Type checking and permission validation
> - Region analysis and escape checking
> - Contract and grant system
> - Witness dispatch and dynamic resolution
> - FFI signatures and ABI conformance
> - Memory layout and representation attributes
> - Panic and unwind behavior across boundaries

> Each test **MUST** declare its expected outcome: either the expected diagnostics for ill-formed programs or the expected observable behavior for well-formed programs. This enables automated test harnesses to determine pass/fail outcomes deterministically without manual inspection.

#### 10.1.3 Feature Interaction Testing [validation.testing.interaction]

> Beyond testing individual features in isolation, implementations **SHOULD** include tests that exercise combinations of features where interactions may reveal defects. At minimum, implementations **SHOULD** test:
>
> - Generics combined with contracts and witnesses
> - Region-allocated types crossing FFI boundaries
> - Concurrent operations on types with permission restrictions
> - Comptime evaluation interacting with dynamic semantics
> - Extension features combined with core language features

> When interactions between features introduce additional semantic constraints, those constraints **MUST** be tested and validated.

### 10.2 Feature Classification [validation.classification]

#### 10.2.1 Feature Table Requirements [validation.classification.table]

> Implementations **MUST** publish a **feature table** that classifies every extension as **Stable**, **Preview**, or **Experimental**:
>
> - **Stable** features have finalized semantics and are supported for production use
> - **Preview** features have provisional semantics subject to refinement based on feedback
> - **Experimental** features are under active development with no stability guarantees

> Preview and Experimental features **MUST** be opt-in and disabled by default in strict conformance mode (§8.3). The feature table **MUST** indicate the minimum compiler version where each feature becomes available and, for stable features, the version where they achieved stable status.

> The feature table **MUST** be included in the conformance dossier (§6.2.4) and **SHOULD** be queryable via compiler flags (e.g., `--list-features`).

## Part II - Program Structure & Translation

This part defines how Cursive source text is ingested, translated, organized into modules/assemblies/projects, and how bindings are resolved. All clauses here are normative. Only the general policies from Part I are assumed.

## 8. Source Text & Translation Pipeline

### 8.1 Source encoding and normalization

#### 8.1.1 UTF‑8 acceptance

Implementations **MUST** accept only byte streams that decode to legal UTF‑8 scalars; invalid sequences **MUST** raise E-SRC-0101.

#### 8.1.2 BOM handling

Implementations **MUST** strip an initial U+FEFF before further analysis and **MUST** raise E-SRC-0103 for any subsequent U+FEFF.

#### 8.1.3 Line-ending normalization

Implementations **MUST** normalize CR and CRLF to a single LF prior to lexing, ensuring mixed endings yield deterministic results.

#### 8.1.4 Control-character restrictions

Outside literals, only HT, LF, CR, and FF **MAY** appear; encountering U+0000 or any other control character **MUST** raise E-SRC-0104.

#### 8.1.5 Minimum file size support

Implementations **MUST** accept files of at least 1 MiB and **MUST** diagnose larger files that exceed their documented limit with E-SRC-0102.

#### 8.1.6 NEWLINE tokenization

After normalization, LF **MUST** emit a `NEWLINE` token that participates in [NDF_REF] termination. The preprocessing target **MUST** follow:

```ebnf
source_file ::= bom? normalized_line*
normalized_line ::= code_point* NEWLINE?
bom ::= "\uFEFF"
```

### 8.2 Translation phases

#### 8.2.1 Pipeline ordering

Compilation **MUST** proceed through the ordered pipeline:

```text
parsing → comptime_execution → type_checking → code_generation → lowering
```

Later phases **MUST NOT** observe artifacts from earlier phases that emitted errors, and each phase **MUST** finish before the next one begins.

#### 8.2.2 Phase determinism

Observable results of each phase (diagnostics, generated declarations, IR) **MUST** be deterministic with respect to the source plus configuration.

#### 8.2.3 Comptime resource minima

During `comptime_execution`, each compilation unit **MUST** support at least:

| Resource               | Minimum             | Diagnostic |
| ---------------------- | ------------------- | ---------- |
| Recursion depth        | 256 frames          | E-SRC-0201 |
| Evaluation steps       | 1_000_000 per block | E-SRC-0202 |
| Comptime heap          | 64 MiB              | E-SRC-0203 |
| String literal size    | 1 MiB               | E-SRC-0204 |
| Collection cardinality | 10_000 elements     | E-SRC-0205 |

Exceeding any limit **MUST** raise the listed diagnostic.

#### 8.2.4 Comptime grant safety

Comptime bodies **MUST** declare every grant they require and **MUST** be rejected with E-SRC-0206 if they request grants outside `{comptime::alloc, comptime::codegen, comptime::config, comptime::diag}`.

#### 8.2.5 Generated-symbol hygiene

Generated identifiers **MUST** be hygienic; collisions with existing declarations **MUST** raise E-SRC-0207.

### 8.3 Phase semantics

#### 8.3.1 Parsing phase

Implementations **MUST** consume the normalized token stream, **MUST** record every declaration and scope, and **MUST NOT** perform semantic validation in this phase so forward references remain valid.

#### 8.3.2 Comptime execution phase

Implementations **MUST** build a dependency graph over `comptime` blocks and **MUST** execute it in dependency order; cycles **MUST** raise E-SRC-0208. Each block **MUST** allocate only in the comptime heap and **MUST** respect its declared grants.

#### 8.3.3 Type-checking phase

Implementations **MUST** resolve names, **MUST** validate permissions/contracts, and **MUST NOT** proceed if any required module initialization failed ([NDF_REF]). Only well-typed programs **MUST** advance.

#### 8.3.4 Code-generation phase

Implementations **MUST** materialize declarations requested by comptime code only after type checking succeeds, and generated declarations **MUST** retain provenance metadata tying them to their comptime origin.

#### 8.3.5 Lowering phase

Implementations **MUST** convert validated ASTs into IR, **MUST** erase comptime constructs, and **MUST** make deterministic cleanup explicit while preserving observable behavior.

## 9. Lexical Structure

### 9.1 Tokens, identifiers, and comments

#### 9.1.1 Identifier grammar

Implementations **MUST** accept identifiers that follow the Unicode `XID_Start` + `XID_Continue*` grammar, **MUST NOT** accept identifiers that match any reserved keyword, and **MUST** compare identifiers by code-point equality after [NDF_REF] preprocessing to keep name resolution deterministic.

#### 9.1.2 Reserved keywords

The keyword set `abstract as async await behavior break by case comptime const continue contract defer else enum exists false forall grant if import internal invariant let loop match modal module move must new none private procedure protected public record region result select self Self shadow shared state static true type unique var where will with witness` **MUST** remain reserved, and using any of them as identifiers **MUST** raise E-SRC-0305.

#### 9.1.3 Whitespace tokens

The lexer **MUST** treat SPACE, HT, and FF as separators only, **MUST** emit LF as the `NEWLINE` token, and **MUST NOT** infer indentation semantics so layout does not change parsing outcomes.

#### 9.1.4 Comment handling

Line comments beginning with `//` **MUST** be discarded, block comments `/* … */` **MUST** nest and raise E-SRC-0306 when unterminated, and documentation comments `///`/`//!` **MUST** be preserved as metadata yet never enter the token stream.

#### 9.1.5 Integer literal forms

Numeric literals **MUST** support decimal, hexadecimal (`0x`), octal (`0o`), and binary (`0b`) forms with `_` separators permitted only between digits; malformed separators or overflow **MUST** raise E-SRC-0304, and unsuffixed integers **MUST** default to `i32`.

#### 9.1.6 Floating literal forms

Floating-point literals **MUST** consist of an integer part, optional fraction, optional exponent, and optional `f32`/`f64` suffix, and literals outside representable ranges **MUST** trigger diagnostics so constant folding remains defined.

#### 9.1.7 String literal escapes

String literals **MUST** use `"` delimiters, **MUST** accept only the escapes `\n \r \t \\ \" \' \0 \xNN \u{…}`, and multi-line strings **MUST** be formed via escapes rather than raw newlines; unterminated or invalid strings **MUST** raise E-SRC-0301/E-SRC-0302.

#### 9.1.8 Character literals

Character literals **MUST** use `'` delimiters, **MUST** encode exactly one Unicode scalar, and violations **MUST** raise E-SRC-0303.

#### 9.1.9 Identifier Unicode validity

Identifiers containing invalid Unicode **MUST** raise E-SRC-0307, and implementations **MUST NOT** attempt to repair such sequences so that the source text remains authoritative.

### 9.2 Automatic statement termination

#### 9.2.1 Termination grammar

> Statement termination **MUST** follow this grammar so lexing and parsing agree on separators:
>
> ```ebnf
> statement_sequence ::= statement (separator statement)*
> separator ::= newline | ";"
> continuation ::= trailing_operator | leading_dot | leading_pipeline | unclosed_delimiter
> ```

#### 9.2.2 Newline termination

Each `newline` token **MUST** terminate the current statement unless one of the continuation predicates holds, ensuring newline-sensitive parsing stays deterministic.

#### 9.2.3 Unclosed delimiters

The `unclosed_delimiter` predicate **MUST** hold whenever `(`, `[`, `{`, or `<` lacks a matching closer so multi-line expressions remain valid without extra delimiters.

#### 9.2.4 Trailing operators

The `trailing_operator` predicate **MUST** hold when a line ends with any binary, assignment, range, or pipeline operator (`+ - * / % ** == != < <= > >= && || & | ^ << >> .. ..= => = += -= *= /= %= &= |= ^= <<= >>=`) so infix forms can span lines safely.

#### 9.2.5 Leading heads

The `leading_dot` predicate **MUST** activate when the next line begins with `.`, and the `leading_pipeline` predicate **MUST** activate when it begins with `=>`, enabling fluent method chains and pipelines without superfluous delimiters.

#### 9.2.6 Maximal munch and lexeme preservation

The lexer **MUST** apply maximal munch globally, and when token reinterpretation (e.g., splitting `>>` into two closers) is required, the parser **MUST** preserve the original lexemes to keep diagnostic spans accurate.

#### 9.2.7 EOF and nesting diagnostics

If end-of-file occurs while any continuation predicate is active, implementations **MUST** emit E-SRC-0401, and they **MUST** support delimiter nesting of at least 256 levels, raising E-SRC-0402 when that bound is exceeded.

### 9.3 Lexical diagnostics

#### 9.3.1 Required diagnostics

Implementations **MUST** surface at least the following diagnostics while enforcing [NDF_REF]–[NDF_REF] so that toolchains present consistent remediation guidance:

| Code       | Condition                                    |
| ---------- | -------------------------------------------- |
| E-SRC-0101 | Invalid UTF-8 sequence                       |
| E-SRC-0102 | Source file exceeds implementation limit     |
| E-SRC-0103 | Embedded BOM after first scalar              |
| E-SRC-0104 | Forbidden control character                  |
| E-SRC-0301 | Unterminated string literal                  |
| E-SRC-0302 | Invalid escape sequence                      |
| E-SRC-0303 | Invalid character literal                    |
| E-SRC-0304 | Malformed numeric literal                    |
| E-SRC-0305 | Reserved keyword used as identifier          |
| E-SRC-0306 | Unterminated block comment                   |
| E-SRC-0307 | Invalid Unicode in identifier                |
| E-SRC-0401 | Unexpected EOF during unterminated statement |
| E-SRC-0402 | Delimiter nesting too deep                   |

## 10. Modules, Assemblies, and Projects

### 10.1 Folder-scoped modules

#### 10.1.1 Module discovery

Any folder containing at least one `.cursive` source file **MUST** be treated as a module, all files directly within it **MUST** belong to that module, and subfolders **MUST** be treated as child modules with `::`-separated names.

$$
\frac{assembly(name) ∈ Γ \quad root ∈ Roots \quad path(module) = root/segments}
{Γ ⊢ module(name::segments) ⇒ Γ, module(name::segments)}
\tag{WF-Module}
$$

#### 10.1.2 Compilation unit uniqueness

Each module **MUST** contain exactly one compilation unit per source file, and a source file **MUST NOT** contribute to multiple modules.

#### 10.1.3 Manifest requirements

Manifests **MUST** enumerate every source root with unique normalized paths. Missing manifests, undefined roots, or duplicates **MUST** be diagnosed (E-SRC-1101…E-SRC-1102).

#### 10.1.4 Path validity

Module path components **MUST** be valid identifiers and **MUST NOT** collide by case on case-insensitive platforms; violations raise E-SRC-1103/E-SRC-1104, and implementations **SHOULD** warn (W-SRC-1105) about potential collisions.

#### 10.1.5 Manifest format and schema

Every project **MUST** include a UTF‑8 manifest file named `Cursive.toml` at the project root. The manifest **MUST** follow TOML 1.0 syntax with the schema below; unknown keys **MUST** raise W-SRC-1106 while malformed structures **MUST** raise E-SRC-1106.

```toml
[language]
version = "1.0.0"            # REQUIRED. Matches the Part I version triplet.

[roots]                        # REQUIRED. Maps logical root names to relative paths.
app = "src"
tests = "tests"

[[assemblies]]                # At least one entry required.
name = "app"                  # REQUIRED, unique per manifest.
roots = ["app"]              # REQUIRED. Entries must reference [roots] keys.
deps = ["runtime"]           # OPTIONAL. Names of other assemblies in the same manifest.

[build]
conformance = "strict"       # OPTIONAL. Overrides default per [NDF_REF].

[features]
enable = ["net.socket.experimental"]   # OPTIONAL. Matches [NDF_REF] identifiers.
```

Semantic requirements (failures raise the listed diagnostics):

1. `language.version` **MUST** be a semantic-version string whose `MAJOR` matches the compiler (E-SRC-1107).

   $$
   \frac{parse_{semver}(v) = (M,m,p) \quad M = M_{compiler}}
   {Γ ⊢ manifest.language(version = v) ⇒ Γ}
   \tag{WF-Manifest-Language}
   $$

2. `[roots]` **MUST** contain at least one entry and each value **MUST** be a normalized relative path without `..` segments.

   $$
   \frac{\forall r ∈ Roots.\ normalized(r) ∧ no\_dotdot(r)}
   {Γ ⊢ manifest.roots(Roots) ⇒ Γ, roots(Roots)}
   \tag{WF-Manifest-Roots}
   $$

3. Each `[[assemblies]]` entry **MUST** reference only declared roots, **MUST** use unique `name` fields, and **MUST NOT** form dependency cycles (E-SRC-1108).

   $$
   \frac{Γ ⊢ manifest.roots(Roots) ⇒ Γ' \quad R \subseteq dom(Roots) \quad deps ⊆ declared\_assemblies \quad acyclic(deps)}
   {Γ' ⊢ assembly(name, R, deps) ⇒ Γ', assembly(name)}
   \tag{WF-Assembly}
   $$

4. The optional `[features].enable` array **MUST** contain only feature identifiers defined in [NDF_REF]; unknown identifiers raise W-CNF-0101 unless overridden by `--enable-feature`.
5. Additional manifest sections **MUST** be ignored only when they appear in a vendor namespace of the form `vendor."domain"` to preserve forward compatibility.

Failing any premise **MUST** trigger the diagnostics enumerated in [NDF_REF] (E/W-SRC-1101…1108).

### 10.2 Assemblies and projects

#### 10.2.1 Assembly definition

An assembly **MUST** be treated as a named collection of modules discovered under a manifest entry. Assemblies may reference each other but remain separately buildable units.

#### 10.2.2 Project definition

A project **MUST** consist of one or more assemblies linked, packaged, or distributed together, **MUST** declare which assemblies it includes, and **MUST** expose a target language version consistent with Part I.

#### 10.2.3 Discovery linkage

Module discovery within an assembly **MUST** follow the folder rules in [NDF_REF]; assemblies merely group discovered modules for distribution/import control.

### 10.3 Imports, aliases, and qualified visibility

#### 10.3.1 Qualified access

Implementations **MUST** make every declaration inside a module available via qualified names derived from folder structure, so intra-assembly access **MUST NOT** require extra keywords.

#### 10.3.2 Import forms

`import module_path` **MUST** precede references to modules in other assemblies or projects, while `import module_path as alias` **MAY** be used for any module to introduce a local alias for qualified access.

$$
\frac{Γ ⊢ module(p) ⇒ Γ' \quad module\_path(p)}
{Γ' ⊢ import\ module(p) ⇒ Γ'}
\tag{WF-Import}
$$

#### 10.3.3 Binding constraints

Imports **MUST NOT** introduce unqualified bindings; qualified references **MUST** be `module::item` or `alias::item`.

#### 10.3.4 Alias rules

Alias names **MUST** be unique within a module and **MUST NOT** be rebound (E-SRC-1201). Referencing undefined module paths **MUST** raise E-SRC-1202.

$$
\frac{Γ' ⊢ import\ module(p) ⇒ Γ' \quad alias \notin dom(aliases)}
{Γ' ⊢ import\ module(p)\ as\ alias ⇒ Γ', alias ↦ p}
\tag{WF-Import-Alias}
$$

#### 10.3.5 Import availability

Imports **MUST** appear somewhere in the module before dependent references, though textual order **MAY** vary because parsing inventories all imports up front.

#### 10.3.6 Re-export policy

Public declarations **MUST** enter the export set automatically, and re-exporting **MUST** occur explicitly via public declarations since `use`/`public use` are unavailable.

#### 10.3.7 Assembly policies

Assemblies **MAY** impose additional import policies, and violations **MUST** be diagnosed with `E-SRC-1203` so policy enforcement remains within the imports bucket and tooling can surface a stable, specific error.

#### 10.3.8 Module-path grammar

The grammar for `module_path` referenced in [NDF_REF]–[NDF_REF] **MUST** be:

```ebnf
module_path ::= identifier ("::" identifier)*
identifier  ::= lexical identifier defined in [NDF_REF]
```

Leading `::` tokens, empty components, and trailing separators **MUST** be rejected with E-SRC-1204. Module paths **MUST NOT** exceed 32 components (matching [NDF_REF]); longer paths raise E-SRC-1205. Implementations **MUST** normalize each component using the same Unicode normalization as [NDF_REF] before comparing paths so manifests, imports, and on-disk layouts stay consistent.

##### 10.3.8.1 Import inference rules

$$
\frac{Γ ⊢ module(p) ⇒ Γ' \quad module\_path(p)}
{Γ' ⊢ import\ module(p) ⇒ Γ'}
\tag{WF-Import}
$$

$$
\frac{Γ' ⊢ import\ module(p) ⇒ Γ' \quad alias \notin dom(aliases)}
{Γ' ⊢ import\ module(p)\ as\ alias ⇒ Γ', alias ↦ p}
\tag{WF-Import-Alias}
$$

Premise failures (unknown module, malformed path, duplicate alias) **MUST** raise the diagnostics listed in [NDF_REF] and [NDF_REF].

### 10.4 Dependency graph and initialization

#### 10.4.1 Graph construction

Implementations **MUST** build a module dependency graph with vertices as modules and edges arising from imports and module-scope initializers/comptime blocks referencing other modules’ exports.

#### 10.4.2 Eager vs. lazy edges

Edges **MUST** be marked eager when initialization reads bindings from the target; otherwise they are lazy.

#### 10.4.3 Eager subgraph constraints

The eager subgraph **MUST** be acyclic (E-SRC-1301), and initialization **MUST** run in topological order; consuming an uninitialized eager dependency **MUST** raise E-SRC-1302.

#### 10.4.4 Lazy semantics

Lazy edges **MAY** form cycles, and implementations **MAY** evaluate initializers lazily provided they execute at most once, preserve observable behavior, and maintain ordering.

#### 10.4.5 Failure propagation

If a module’s initialization fails, every module reachable via eager edges **MUST** be reported as blocked (E-SRC-1303). Safe retries **MAY** occur only if failed effects can be rolled back.

## 11. Scopes, Bindings, and Lookup

### 11.1 Scope tree

#### 11.1.1 Tree structure

Scopes **MUST** form a rooted tree comprising module, procedure, block, region, and generic scopes. Multiple parents or cycles **MUST** raise E-SRC-1401.

#### 11.1.2 Import effect

`import` statements **MUST NOT** create scopes; they only populate the alias map.

#### 11.1.3 Implicit blocks

Implicit control-flow blocks (`if`, `loop`, `match`, etc.) **MUST** act as lexical scopes equivalent to explicit braces.

#### 11.1.4 Region scope behavior

Region scopes **MUST** follow ordinary block visibility and **MUST NOT** introduce lookup rules beyond the lexical nesting defined in this part.

#### 11.1.5 Module-scope restrictions

Module scope **MUST** contain only top-level declarations and imports; expression statements or control-flow constructs **MUST** trigger E-SRC-0501.

### 11.2 Shadowing and binding metadata

#### 11.2.1 Single-binding declarations

Every declaration **MUST** introduce exactly one binding per identifier; multi-declarator statements **MUST NOT** exist, so multiple identifiers **MUST** be expressed through pattern destructuring instead.

#### 11.2.2 Shadow keyword requirement

Redeclaring a name within the same scope **MUST** use the `shadow` keyword, and implementations **MUST** emit E-SRC-1501 whenever a redeclaration omits it.

#### 11.2.3 Scoped shadow eligibility

The `shadow` keyword **MAY** appear only inside a scope that is strictly nested relative to the binding being shadowed and **MUST NOT** appear at module scope or against predeclared identifiers; violating either rule **MUST** raise E-SRC-1502 or E-SRC-1503.

#### 11.2.4 Pattern shadow coverage

Pattern shadowing **MUST** shadow every identifier in the pattern simultaneously, and any mismatch **MUST** raise E-SRC-1504 so programmers fix the incomplete declaration.

#### 11.2.5 Binding metadata tracking

Each binding **MUST** record its identifier, entity kind, type, visibility, cleanup responsibility (`=` vs `<-` vs `move` parameters), rebindability (`let` vs `var`), and source location so diagnostics and tooling can expose precise binding metadata.

### 11.3 Name lookup

#### 11.3.1 Unqualified lookup ordering

Implementations **MUST** resolve every unqualified identifier using this order and **MUST** stop once a binding is found: (1) current block scope, (2) lexical ancestors up to the enclosing procedure or module, (3) module-scope declarations, (4) alias-map entries created via `import … as …`, and (5) universe scope ([NDF_REF]). Aliases only authorize qualified heads; duplicate alias names **MUST** raise E-SRC-1601, and missing identifiers **MUST** raise E-SRC-1602.

$$
\frac{x : τ ∈ Γ_{block}}
{Γ ⊢ x ⇓ τ}
\tag{QR-Resolve-Local}
$$

$$
\frac{Γ_{parent} ⊢ x ⇓ τ \quad scope(parent) ⊂ scope(child)}
{Γ_{child} ⊢ x ⇓ τ}
\tag{QR-Resolve-Enclosing}
$$

$$
\frac{alias ↦ p ∈ Γ_{alias}}
{Γ ⊢ alias ⇓ module(p)}
\tag{QR-Resolve-Alias}
$$

#### 11.3.2 Qualified lookup requirements

For references of the form `head::identifier`, implementations **MUST** apply the unqualified algorithm to resolve `head`, diagnose E-SRC-1603 if the head is neither a module nor a module alias, and then enforce:

1. `identifier` **MUST** exist (E-SRC-1605 otherwise) and **MUST** be public when referenced across module boundaries (E-SRC-1604).
2. Multi-component chains (`a::b::c::d`) **MUST** be evaluated left-to-right, and the first missing component **MUST** be diagnosed to avoid cascaded errors.
3. Qualified chains **MUST NOT** exceed 32 components (E-SRC-1606) and **MUST NOT** contain empty components such as `::Name` (E-SRC-1607).
4. Alias ambiguity discovered during qualified lookup **MUST** continue to raise E-SRC-1601 until the programmer disambiguates via qualification or renaming.

$$
\frac{Γ ⊢ module(p::q) ⇒ Γ' \quad Γ' ⊢ q ⇓ entity}
{Γ ⊢ p::q ⇓ entity}
\tag{QR-Resolve-Module}
$$

### 11.4 Universe scope and protected identifiers

#### 11.4.1 Canonical universe bindings

The universe scope **MUST** contain the primitive numeric types, `bool`, `char`, `string`, unit `()`, never `!`, boolean literals (`true`, `false`), intrinsic helpers (`panic`, `assert`), and pointer helpers such as `Ptr`, forming the canonical built-in namespace.

#### 11.4.2 Protection rules

Universe-scope identifiers **MUST NOT** be redeclared, shadowed, or aliased via `import`; attempts **MUST** raise E-SRC-1503 and programmers **MUST** rely on `type` aliases when they need alternate names.

#### 11.4.3 Lookup participation

Universe bindings **MUST** participate only at Step 5 of the unqualified lookup algorithm so that user-defined names in closer scopes always take precedence.

$$
\frac{x ∈ Universe \quad x \notin dom(Γ_{local})}
{Γ ⊢ x ⇓ Universe(x)}
\tag{QR-Resolve-Universe}
$$

##### 11.4.3.1 QR-Resolve-Universe

$$
\frac{x ∈ Universe \quad x \notin dom(Γ_{local})}
{Γ ⊢ x ⇓ Universe(x)}
\tag{QR-Resolve-Universe}
$$

This rule enforces the protection contract from [NDF_REF].

## 12. Tooling hooks

### 12.1 Compiler data surfaces

#### 12.1.1 Phase artifact exposure

Implementations **MUST** expose, through a documented interface, each compilation phase’s outputs together with the module table (folder-derived hierarchy), alias maps, dependency graph, initialization schedule, and the diagnostics enumerated in this part so that tooling can reason about translation consistently.

#### 12.1.2 Lexeme and span preservation

Implementations **SHOULD** preserve original lexemes and source spans for every token and binding; when this data is surfaced through the tooling interface it **MUST** be precise enough for downstream tools to reconstruct user code faithfully.

> [!note]
>
> - There is no textual macro or preprocessing system. All effects are explicit in the syntax and semantics defined above.
> - Comptime side effects (allocation, code generation, diagnostics) occur only at compile time and never mutate runtime state.
> - Deterministic statement and expression rules support reproducible builds and tooling.

---

## Part III - Declarations & the Type System

## 10. Declarations

### 10.0 Declaration Judgment

Declaration well-formedness is described by the judgment:

$$
\Gamma \vdash \text{decl} \Rightarrow \Gamma'
$$

This judgment states: "Declaration $\text{decl}$ is well-formed in context $\Gamma$ and extends the environment to $\Gamma'$."

**Components:**

- $\Gamma$ - the environment before processing the declaration
- $\text{decl}$ - the declaration being validated (variable binding, procedure, type, module, etc.)
- $\Gamma'$ - the resulting environment with new bindings introduced by the declaration

This judgment validates that declarations are well-formed and tracks how they extend the naming environment. Each declaration form has an accompanying well-formedness rule (prefixed `WF-*` in formal rules) that derives this judgment. Implementations **MUST** ensure all declarations are well-formed before they can be referenced.

### 10.1 Bindings [decl.bindings]

#### 10.1.1 Binding kinds

Every declaration **MUST** use either `let` (immutable) or `var` (rebindable). Rebinding a `let` binding **MUST NOT** be permitted; rebinding a `var` binding **MUST** follow [NDF_REF]. The `var` keyword affects only the binding, not the value’s internal mutability.

#### 10.1.2 Cleanup responsibility

The assignment operator **MUST** determine cleanup responsibility: `=` **MUST** create a responsible binding that runs its destructor at scope exit, while `<-` **MUST** create a non-responsible binding that never runs a destructor. Responsible and non-responsible bindings **MUST** obey the invalidation and move rules defined in the memory model.

#### 10.1.3 Definite assignment

Bindings **MUST** be definitely assigned before use. Both `let` and `var` declarations **MUST** trigger diagnostics when used uninitialized, per Clause 8.

#### 10.1.4 Shadowing controls

Shadowing **MAY** occur only when the `shadow` keyword is present; otherwise redeclaration in the same scope **MUST** be rejected. Predeclared universe-scope identifiers **MUST NOT** be shadowed or redeclared.

#### 10.1.5 Pattern bindings

Pattern destructuring **MAY** appear in declarations, but pattern forms **MUST** respect exhaustiveness requirements (e.g., enums, unions, modal states). Destructuring statements **MUST NOT** appear where expressions are forbidden (E-07-520).

#### 10.1.6 Permissions at bindings

Lexical permissions (`const`, `shared`, `unique`) **MUST** attach to bindings and flow through types. When omitted, implementations **SHOULD** infer `const` unless evidence requires otherwise. Ambiguous inference **MUST** emit E-10-320 demanding annotation. Downgrades (`unique→shared→const`) **MAY** occur implicitly; upgrades **MUST NOT**.

---

### 10.2 Procedures [decl.procedure]

#### 10.2.1 Surface form

A procedure declaration consists of optional attributes and visibility, a head (`procedure`, `comptime procedure`, or `extern(...) procedure`), a signature (optional generics, parameters, optional return type, optional **sequent**), and a body. Expression-bodied forms are sugar for pure procedures and **must not** include an explicit sequent; they default to `[[ |- true => true ]]`. Violations **must** diagnose E-05-408.

#### 10.2.2 Receivers

Receiver shorthands **must** desugar as follows and **must** be the first parameter:  
`~` → `self: const Self`, `~%` → `self: shared Self`, `~!` → `self: unique Self`. Using a receiver elsewhere than first position **must** be rejected (E-05-401). When used as first-class callables, the receiver appears as the leading parameter in the FunctionType.

#### 10.2.3 Parameter responsibility

Parameters **may** be marked `move` to indicate that the callee assumes cleanup responsibility. Call sites **must** pass such parameters using `move arg`; omission or superfluous `move` **must** be diagnosed (E-05-409/E-05-410). Unmarked parameters are non-responsible.

#### 10.2.4 Contracts (Sequents)

Procedures **may** attach a **contractual sequent** of the form `[[ grants |- must => will ]]`. When omitted, it **must** default to the pure, trivial contract `[[ |- true => true ]]`. Grant accumulation for a procedure body **must** be a superset of the grants named in the sequent; otherwise emit E-07-004. Sequent entailment **must** be enforced at call sites and during callable subtyping.

#### 10.2.5 Purity

A procedure is **pure** iff its grant set is empty and its sequent is `[[ |- true => true ]]`. Expression-bodied procedures are implicitly pure. Re-exporting a callable with a **wider** grant set or **weaker** `will` clause than its implementation **must not** be allowed (E-07-204).

#### 10.2.6 Overloading and resolution

Multiple procedures may share a name provided their **signatures** differ. Resolution **must** be type-directed using the bidirectional typing rules; inability to infer a principal type **must** produce E-08-002, and ambiguous resolution **must** produce E-10-610. Name mangling for distinct overloads is implementation-defined but **must** preserve semantic distinctions. Capturing closures **must not** be convertible to raw pointers (E-07-206).

---

### 10.3 Type Declarations [decl.types]

#### 10.3.1 Records

Records **MUST** be nominal product types with both positional and named field access. Field visibility **MUST** respect module visibility, layout contracts and reflection **MUST** report field offsets in declaration order, and representation attributes (e.g., `[[repr(C)]]`, `[[repr(packed)]]`, `[[repr(align(N))]]`) **MUST** influence layout per Clause 11.

#### 10.3.2 Enums

Enums **MUST** be nominal sum types with explicit variants and optional payloads. Implementations **MAY** expose discriminants via casts to `usize`, but casts from integers to enums **MUST NOT** be allowed. Match expressions over enums **MUST** be exhaustive.

#### 10.3.3 Unions

The `\/` constructor **MUST** form structural unions whose components are well-formed types. Widening to a union (`τ <: τ \/ …`) **MUST** be supported, and matches **MUST** be exhaustive or include a catch-all arm (diagnostics E-07-710…E-07-712, E-08-450).

#### 10.3.4 Tuples

Tuples **MUST** be structural product types with fixed arity and positional projection. Tuple element access **MUST** be statically bounds-checked; out-of-range indices **MUST** be diagnosed.

#### 10.3.5 Arrays and slices

Arrays `[T; n]` **MUST** be fixed-size contiguous buffers with mandatory runtime bounds checks for indexing. Slices `[T]` **MUST** behave as `(ptr,len)` views and **MUST** be `Copy` regardless of `T`. Repeat literals `[e; n]` **MUST** require `T: Copy`.

#### 10.3.6 Strings

`string@Managed` (growable, heap) and `string@View` (read-only) **MUST** represent the modal states of the string type. A bare `string` **MUST** default to `string@View`; the coercion `string@Managed <: string@View` **MUST** be permitted. C interop **MUST** use explicit conversions; null termination **MUST NOT** be inserted automatically.

#### 10.3.7 Type aliases

Type aliases declared as `type A = T` **MUST** be transparent for type equivalence, and cyclic aliases **MUST** be diagnosed.

---

## 11. Type Foundations

### 11.0 Type Formation Judgment

Type well-formedness is described by the judgment:

$$
\Gamma \vdash \tau : \text{Type}
$$

This judgment states: "In context $\Gamma$, type expression $\tau$ is well-formed and classifies values."

**Components:**

- $\Gamma$ - the typing context containing type variable bindings, generic parameters, and available type names
- $\tau$ - the type expression being validated
- $\text{Type}$ - the kind (classification) of $\tau$, indicating it can classify runtime values

This judgment ensures that all type expressions are meaningful before they're used in typing rules for expressions and declarations. Implementations **MUST** validate type well-formedness before using types in subsequent judgments.

### 11.1 Kinds and Universes

Cursive defines a single kind `Type` for the formation judgment `Γ ⊢ τ : Type`. Type environments **must** bind term variables to types, type variables to kinds (optionally with behavior bounds), grant parameters to grant sets, and modal parameters to state constraints. No additional kind polymorphism is specified in this edition.

### 11.2 Equivalence & Subtyping

#### 11.2.1 Type equivalence

Two types **MUST** be considered equivalent (`≡`) when they denote the same type after alias expansion and structural normalization. Equivalence **MUST** be reflexive, symmetric, and transitive; structural types compare element-wise, and nominal types are equivalent only when originating from the same declaration.

#### 11.2.2 Subtyping preorder

Subtyping (`<:`) **MUST** form a preorder that includes at least: function types (contravariant in parameters, grants, and `must`, covariant in result and `will`), pointer/modal widening (`Ptr<T>@S <: Ptr<T>` and `M@S <: M`), union inclusion (`τ <: τ \/ …`), and invariant treatment of primitives, tuples, arrays, records, enums, and modals unless stated otherwise.

#### 11.2.3 Variance defaults

User-defined generic types **MUST** default to invariant unless inference proves otherwise; union types **MUST** be covariant in each component, and callable types **MUST** follow the function subtyping variance rule. Violations **MUST** raise E-10-701.

### 11.3 Type Inference Boundaries

Cursive employs **bidirectional** typing. Inference sites synthesize types; checking sites require contextual types. When the principal type cannot be inferred, the implementation **must** emit diagnostics (e.g., E-08-002) requesting annotations. Pipeline stages that change type **must** carry explicit annotations, and missing/incorrect annotations **must** be diagnosed (E-08-020/E-08-021).

---

## 12. Primitive & Composite Types

### 12.1 Primitive Scalars

Primitive numeric types, `bool`, `char`, and unit `()` **must** have fixed, target-specified sizes/alignments per Clause 11 and **must** participate only in explicitly defined conversions-no implicit numeric promotions are permitted. Divergent expressions (`!`) **must** subtype all types. (See Clause 8 for operators.)

### 12.2 Composite Types

#### 12.2.1 Structural composites

Composite forms (records, tuples, arrays, slices, strings, enums, unions) **MUST** follow [NDF_REF]. Layout, padding, and alignment **MUST** honor Clause 11. Enum/union exhaustiveness **MUST** be enforced. Array and slice indexing **MUST** perform runtime bounds checks unless statically proven safe.

#### 12.2.2 Literals and implicit conversions

The language **MUST NOT** perform implicit numeric promotions or bool↔integer coercions; casts **MUST** be explicit via `as`. Only the narrow implicit coercions listed in [NDF_REF] (e.g., pointer/modal widening, unit insertion, `string@Managed → string@View`) **MAY** occur.

---

## 13. Modal Types (General Framework)

### 13.1 Definition

Modal types define **compile-time state machines**. Each modal value **must** inhabit exactly one named state at a time; transitions **must** be declared and enforced by the type system. Built-in modals include `Ptr<T>` and `Arena`; user-defined modals follow the same rules.

### 13.2 Syntax & Formation

Modal declarations use `modal M { @State { … } … }` with optional state fields, methods, and **transition signatures** `@S::name(params) -> @T`. Each signature **must** have a corresponding procedure `M.name` implementing `@S -> @T` with a receiver shorthand (`~`, `~%`, or `~!`) that matches permission requirements. Missing or mismatched implementations **must** be diagnosed (E-07-505). Field access **must** be permitted only in the appropriate state (E-07-504).

### 13.3 Contracts and Witnesses

Transition procedures **must** state entry/exit witnesses in their sequents and **must** return a value in the target state on every non-diverging path. Callers **must** supply the entry witness and **must** refine the result via pattern matching when a transition yields multiple possible states (e.g., `upgrade`).

---

## 14. Pointer & Reference Model

### 14.1 Safe Pointer `Ptr<T>`

#### 14.1.1 Pointer states

`Ptr<T>` **MUST** track modal states `@Null`, `@Valid`, `@Weak`, and `@Expired`. Narrowing from unconstrained `Ptr<T>` **MUST** require refinement via patterns or known transitions; widening back to `Ptr<T>` **MAY** occur implicitly. Dereference **MUST** be permitted only in `@Valid` (E-07-301/304/305).

#### 14.1.2 Creation and address-of

The expression `&place` **MUST** yield `Ptr<T>@Valid`. Address-of **MUST NOT** apply to non-storage expressions (E-07-302). Pointer declarations **MUST** carry explicit pointer types (E-07-303). `Ptr::null<T>()` **MUST** produce `@Null`.

#### 14.1.3 Weak/upgrade transitions

`downgrade()` **MUST** produce `@Weak`, and `upgrade()` **MUST** yield `Ptr<T>` that callers refine into `@Valid` or `@Null`. No transition **MAY** convert `@Expired` to a live state.

#### 14.1.4 Provenance and escape

Implementations **MUST** track pointer provenance (`Stack`, `Region(r)`, `Heap`) and **MUST** reject illegal escapes (E-07-300). Diagnostics **SHOULD** include allocation and escape sites with remediation guidance.

#### 14.1.5 Layout and Copy semantics

All pointer states **MUST** be `Copy` and **MUST** share machine-word size and alignment.

### 14.2 Raw Pointers `*const/*mut`

Raw pointer formation **must** be allowed for any well-formed pointee type; dereference, writes, arithmetic, and casts **must** require `unsafe.ptr` (E-07-400…402). Unverifiable behavior results if alignment/lifetime/aliasing is violated; the spec imposes no extra compile-time checks beyond the `unsafe` requirement.

### 14.3 Escape & Provenance Rules

Returning or storing a `Stack`/`Region` pointer beyond its lifetime **must** be rejected; `Heap` is unrestricted (subject to ownership). Region and heap transitions are specified in Clause 11; interop rules for null pointers and FFI **must** rely on explicit checks and contracts.

---

## 15. Generics & Parametric Polymorphism

### 15.1 Parameters and Bounds

Generic parameters **may** include **type**, **const**, and **grant** parameters. Bounds **may** be written inline or in `where` clauses and **must** be enforced at instantiation time. Satisfaction of behavior bounds **must** require an implementation of the behavior for the concrete type; associated-type and contract bounds **must** be honored.

### 15.2 Variance

Implementations **must** enforce variance per Table 7.7.1; user-defined types **must** default to invariant; a conservative **inference** algorithm **must** be applied where specified; violations **must** emit E-10-701. Union types are covariant; callable types follow function subtyping variance.

### 15.3 Monomorphization

Each distinct combination of type/const/grant arguments **must** yield a distinct instantiation. Implementations **may** share code across compatible instantiations as a QoI optimization, but semantics **must not** change. Behavior calls **must** monomorphize to direct calls when dispatch is static (no runtime vtables).

### 15.4 Generic Constants & Determinism

Const generics **must** be substituted before const/comptime evaluation; results **must** be deterministic for identical inputs. Implementations **may** cache instantiations and comptime results provided observable behavior is unchanged.

---

## Part IV - Ownership, Permissions, Regions

## 16. Ownership & Responsibility

### 16.1 Concepts and Axes

#### 16.1.1 Responsibility–permission separation

Cursive **MUST** treat cleanup responsibility (controlled by `=` vs `<-`) as orthogonal to permissions (`const`, `shared`, `unique`). Implementations **MUST NOT** conflate these axes when analyzing programs.

### 16.2 Responsible vs Non-responsible Bindings

#### 16.2.1 Binding operators

`=` **MUST** create responsible bindings that run destructors at scope exit, whereas `<-` **MUST** create non-responsible bindings that never run destructors. This rule **MUST** apply regardless of `let` vs `var` or permission qualifiers.

#### 16.2.2 Type preservation

Using `<-` **MUST NOT** change the binding's static type; responsible and non-responsible bindings **MUST** share identical types.

#### 16.2.3 Invalidation rule

When a source binding is moved to a responsible parameter (declared `move`), **all** non-responsible bindings referencing that object **MUST** be invalidated. Passing to non-responsible parameters **MUST NOT** invalidate derived non-responsible bindings.

#### 16.2.4 Rebinding

`var` bindings **MAY** be rebound with either operator. Rebinding via `=` **MUST** drop the old value before binding the new one; rebinding via `<-` **MUST NOT** drop the old value.

#### 16.2.5 Definite assignment

Every binding **MUST** be definitely assigned before use. Implementations **MUST** track non-responsible dependencies across initialization per the rules in Clause 8.

### 16.3 Move Semantics

#### 16.3.1 Move keyword usage

Cleanup responsibility **MUST** transfer only through explicit `move`. Attempts to move non-responsible bindings **MUST** be rejected.

#### 16.3.2 Parameter responsibility

Procedures **MAY** declare responsible parameters using `move`. Call sites **MUST** supply `move arg` when invoking such parameters, and non-responsible parameters **MUST NOT** destroy their arguments.

### 16.4 RAII & Deterministic Cleanup

#### 16.4.1 Scope-exit destruction

Responsible bindings **MUST** run destructors at scope exit—including early exits—and destructors **MUST** execute in reverse declaration order (LIFO).

#### 16.4.2 Temporary destruction

Expression temporaries **MUST** drop exactly once at scope exit in reverse creation order, analyzing branches independently.

### 16.5 Copy & Drop

#### 16.5.1 Marker behaviors

Implementations **MUST** auto-derive `Copy` and `Sized` when structural requirements hold and **MUST** enforce mutual exclusion between `Copy` and `Drop`.

#### 16.5.2 Copy types

A type **MUST** qualify as `Copy` iff all fields/variants are `Copy` and the type lacks `Drop`. Implementations **MUST** reject explicit `Copy` attachments that violate this rule.

#### 16.5.3 Drop types

A type with `Drop` **MUST** provide `procedure drop(~!)`, and destruction order **MUST** follow RAII rules with subobjects destroyed in reverse field order.

---

## 17. Lexical Permission System (LPS)

### 17.1 Overview

#### 17.1.1 Permission qualifiers

Cursive **MUST** recognize `const`, `unique`, and `shared` as permission qualifiers that are orthogonal to cleanup responsibility, and implementations **MUST NOT** provide interior mutability through `const`.

### 17.2 Permission Lattice and Coercions

#### 17.2.1 Subtyping lattice

Permissions **MUST** form the lattice `unique <: shared <: const`. Downgrades (`unique→shared`, `unique→const`, `shared→const`) **MAY** occur implicitly; upgrades **MUST NOT** occur and **MUST** be diagnosed.

#### 17.2.2 Permission inference

When omitted, permission **MUST** be inferred by preferring explicit annotations, then initializer permissions, and finally defaulting to `const`. Ambiguity **MUST** raise E-10-320 requiring explicit annotation.

### 17.3 Permission Semantics

#### 17.3.1 Const bindings

Multiple `const` bindings **MAY** coexist; reads are permitted but mutation through `const` **MUST NOT** occur.

#### 17.3.2 Unique bindings

While a `unique` binding is active, creating another `unique` or any `shared` binding to the same object **MUST** be rejected. Temporary downgrades to `const` **MAY** occur, during which the `unique` binding is inactive and regains exclusivity afterward.

#### 17.3.3 Shared bindings

Multiple `shared` bindings **MAY** coexist but programmers **MUST** coordinate mutation. Field-level partitioning (below) constrains aliasing granularity.

### 17.4 Field-Level Partitioning and Reborrowing

#### 17.4.1 Partition rules

Record declarations **MAY** define partitions; through any single `shared` binding, each partition **MUST** be accessed at most once per scope, while different partitions **MAY** be accessed independently.

#### 17.4.2 Nested access

Partition tracking **MUST** apply only at the direct access through the original `shared` binding; nested references **MUST NOT** re-check the partition.

#### 17.4.3 Reborrows

A `unique` binding **MAY** create temporary `const` views; during the view's lifetime the original `unique` binding **MUST** remain inactive and **MUST** reactivate only after the view ends.

### 17.5 Receivers and Parameters

#### 17.5.1 Permission-qualified receivers

Receiver shorthands and parameter declarations **MUST** carry permission-qualified types (e.g., `~` → `self: const Self`, `~%` → `self: shared Self`, `~!` → `self: unique Self`). Call sites **MUST** satisfy the required permissions, and violations (e.g., mutation through `const`) **MUST** be diagnosed.

---

## 18. Region-Based Memory & Arenas

### 18.1 Regions and `region {}` Blocks

#### 18.1.1 Region creation

Entering a `region` block **MUST** create an implicit arena with LIFO lifetime. The `^` operator **MUST** allocate into the active arena, and caret stacking (`^^`, `^^^`) **MUST** reference parent arenas. Region allocations **MUST** be freed in O(1) at block exit.

#### 18.1.2 Escape analysis

Values allocated within a region **MUST NOT** escape that region's lifetime; violations **MUST** be diagnosed (E-11-101). Arena bindings themselves **MUST NOT** escape (E-11-110).

#### 18.1.3 Nested arenas

Nested regions **MUST** release inner arenas before outer ones. Managing multiple arenas that share a lifetime **MUST** be handled at the library level rather than by the core language.

### 18.2 Arena Modal Type

#### 18.2.1 Arena states and operations

Implementations **MUST** provide a built-in `Arena` modal with states `@Active`, `@Frozen`, and `@Freed`. Creation on `region` entry, the `^` desugaring to `arena.alloc`, and transitions such as `freeze`, `thaw`, `reset`, and `free` **MUST** conform to the specified state machine, culminating in bulk deallocation at region exit.

### 18.3 Interplay with Heap and Stack

#### 18.3.1 Provenance tracking

Pointer provenance (`Stack`, `Region(r)`, `Heap`) **MUST** be tracked, and escape rules **MUST** reject stack/region escapes unless values are explicitly converted to heap.

#### 18.3.2 Heap promotion

Region values and witnesses **MAY** be promoted to the heap via `.to_heap()` when the appropriate grant is available; promoted values **MAY** outlive the region thereafter.

---

## 19. Moves, Assignments, and Destruction

### 19.1 Assignment Operators and Responsibility

#### 19.1.1 Responsibility binding

`=` **MUST** bind cleanup responsibility; `<-` **MUST NOT**. Both operators **MUST** leave the binding’s type unchanged, and rebinding a `var` with `=` **MUST** drop the old value before storing the new one, whereas rebinding with `<-` **MUST NOT** drop.

#### 19.1.2 Move-only values

Types lacking `Copy` **MUST** be treated as move-only. Moving such a value **MUST** invalidate the source, and any subsequent use **MUST** be rejected (see Clause 8/Annex E diagnostics).

### 19.2 Deterministic Drop Semantics

#### 19.2.1 Scope-exit ordering

Responsible bindings **MUST** drop in reverse declaration order, with destructors running on both normal and early exits before `defer` blocks complete as specified.

#### 19.2.2 Temporary destruction

Temporaries **MUST** drop at scope exit in reverse creation order. Across branches, the only guarantee **MUST** be that each temporary drops exactly once.

### 19.3 Non-responsible Bindings and Safety Patterns

#### 19.3.1 Preservation of non-responsible views

Implementations **SHOULD** encourage patterns that keep the source binding alive while non-responsible views exist. Moving the source into a responsible parameter **MUST** invalidate all outstanding non-responsible views.

---

## Part V - Expressions, Statements, and Control Flow

## 20. Expressions

### 20.1 Expression Model and Categories

#### 20.1.1 Typing judgment

Expression typing is described by the judgment:

$$
\Gamma \vdash e : \tau \,!\, G \,[\text{cat}]
$$

This judgment states: "In context $\Gamma$, expression $e$ has type $\tau$, requires grants $G$, and belongs to category $\text{cat}$."

**Components:**

- $\Gamma$ - typing context containing variable bindings, available names, and permissions
- $e$ - the expression being typed
- $\tau$ - the static type of the expression
- $G$ - the set of grants (capabilities/effects) required to evaluate the expression
- $\text{cat} \in \{\text{value}, \text{place}, \text{divergent}\}$ - the expression category:
  - **value**: produces a value (e.g., literals, arithmetic)
  - **place**: denotes a memory location (e.g., variables, field access)
  - **divergent**: never produces a value (e.g., `return`, `break`, infinite loops)

Implementations **MUST** track all four components (type, grants, category, and context) during type checking and **MUST** expose them to diagnostics and tooling.

#### 20.1.2 Evaluation order

Composite expressions **MUST** evaluate left-to-right deterministically. Temporaries **MUST** be destroyed at scope exit in reverse creation order, with each branch analyzed independently so that every temporary drops exactly once.

#### 20.1.3 Grant accumulation

The required grants of a composite expression **MUST** equal the union of its subexpressions plus any intrinsic grants contributed by the expression form. The enclosing procedure's sequent **MUST** include at least this union or the program is ill-formed.

#### 20.1.4 Evaluation judgment

Expression evaluation is described using big-step operational semantics:

$$
\sigma \vdash e \Downarrow v, \sigma'
$$

This judgment states: "Expression $e$ evaluated in store $\sigma$ reduces to value $v$ with resulting store $\sigma'$."

**Store notation conventions:**

- $\sigma$ - current program store (maps locations to values)
- $\sigma'$ - resulting store after evaluation (prime indicates "after")
- $\sigma_{\text{ct}}$ - compile-time evaluation store (for comptime contexts)
- $\sigma_{\text{entry}}, \sigma_{\text{exit}}$ - temporal stores (procedure entry/exit)
- $\sigma[x \mapsto v]$ - store $\sigma$ updated with binding $x$ mapped to value $v$

**Evaluation contexts:**

- $\Downarrow$ - big-step evaluation (expression to value)
- $\Downarrow_{\text{comptime}}$ - compile-time evaluation (for comptime blocks)
- $\to$ - small-step reduction (rarely used; expression to expression)

This specification **MUST** treat big-step semantics as the primary evaluation model; small-step reductions exist only for completeness, and every evaluation rule **MUST** thread stores consistently using the notation above.

### 20.2 Operators and Precedence

#### 20.2.1 Table conformance

Implementations **MUST** conform to the operator precedence and associativity table in Clause 7: binary operators associate left-to-right except exponentiation/assignments, which associate right-to-left; short-circuiting operators **MUST** behave as marked.

#### 20.2.2 Operator semantics coverage

Calls, field access, indexing, slicing, pipelines, unary operators, arithmetic, comparison, bitwise, logical, and assignment operators **MUST** follow the typing/evaluation rules in [NDF_REF]–[NDF_REF].

### 20.3 Unary and Binary Operators

#### 20.3.1 Logical and arithmetic unary

`!` **MUST** require `bool`, and unary `-` **MUST** require numeric types. Checked builds **MUST** panic when negating the minimum signed integer; release builds **MUST** wrap per numeric semantics.

#### 20.3.2 Address-of

`&` **MUST** apply only to places and **MUST** produce `Ptr<T>@Valid`; taking the address of non-places **MUST** be diagnosed (E-08-260).

#### 20.3.3 Move expressions

`move e` **MUST** transfer cleanup responsibility and consume the source when the type is move-only, and any subsequent use **MUST** be rejected per the memory model.

#### 20.3.4 Binary arithmetic and bitwise

Binary arithmetic/bitwise operators **MUST** require identical numeric operand types; `%` **MUST** be integer-only; shifts **MUST** take `usize` amounts below the bit-width. Division/modulo by zero **MUST** panic. Evaluation **MUST** be left-to-right.

#### 20.3.5 Logical operators

`&&` and `||` **MUST** short-circuit left-to-right and **MUST** reject non-boolean operands.

#### 20.3.6 Assignments

Assignment and compound-assignment left operands **MUST** be places with sufficient permission. Implementations **MUST** read each place exactly once for `op=` before writing back, and chained assignments lacking explicit sequencing **MUST** be rejected.

### 20.4 Calls, Methods, and Pipelines

#### 20.4.1 Call evaluation

Call evaluation **MUST** evaluate the callee expression first and then the arguments left-to-right. Method receivers **MUST** supply the permission required by the method’s `self` parameter.

#### 20.4.2 Pipelines

`lhs => stage` **MUST** desugar to `stage(lhs)`. Each stage **MUST** accept a single argument (the prior result), and the pipeline’s grant set **MUST** equal the union of the lhs and all stages. Stages that change type **MUST** include explicit type annotations or trigger E-08-020/E-08-021.

### 20.5 Literals and Construction

#### 20.5.1 Numeric literals

Unsuffixed integer literals **MUST** default to `i32`, and unsuffixed floating literals **MUST** default to `f64`. Contextual coercions **MUST** reject values that do not fit the target type.

#### 20.5.2 Structured constructors

Record, tuple, enum, and array literals **MUST** obey the structured-expression formation rules; mismatched fields or element types **MUST** be diagnosed. Repeat arrays **MUST** require `Copy` elements or compile-time lengths.

### 20.6 Address-of and Dereference

#### 20.6.1 Safe pointer dereference

Dereferencing `Ptr<T>` **MUST** require state `@Valid`; dereferencing `@Null`, `@Weak`, or `@Expired` **MUST** be rejected with the specified diagnostics.

#### 20.6.2 Raw pointer operations

Dereferencing or writing through `*const T`/`*mut T` **MUST** be allowed only inside `unsafe` blocks and **MUST** require grant `unsafe.ptr`. Outside `unsafe`, diagnostics E-07-400/E-07-401 **MUST** be emitted.

#### 20.6.3 Address-of formation

`&place` **MUST** produce `Ptr<T>@Valid`, and implementations **MUST NOT** form addresses of non-storage expressions.

### 20.7 Conversions and Coercions

#### 20.7.1 Explicit casts

Only the conversion categories listed in [NDF_REF] **MAY** be used (numeric↔numeric, specific pointer casts requiring `unsafe.ptr`, modal widening, enum→discriminant, defined opaque handles). All other casts **MUST** be rejected (E-08-601). Cast operands **MUST** be evaluated before the cast.

#### 20.7.2 Implicit coercions

Implementations **MAY** apply only the explicitly permitted implicit coercions (unit insertion, pointer/modal widening, `string@Managed → string@View`, pipeline type-preserving rule). Numeric promotions and boolean conversions **MUST NOT** occur implicitly.

#### 20.7.3 Boolean coercion prohibition

Non-boolean expressions **MUST NOT** be implicitly converted to `bool`; logical operators **MUST** require boolean operands.

### 20.8 Diagnostics and Metadata

#### 20.8.1 Diagnostic payloads

Expression diagnostics **MUST** include the computed type, grant set, category, and evaluation-order indices whenever those details aid remediation.

#### 20.8.2 Canonical error codes

Operator and expression errors (e.g., non-place assignment target, missing receiver permission, out-of-range indexing) **MUST** be reported using the diagnostic codes enumerated in Clause 7.

---

## 21. Statements

### 21.0 Statement Judgment

Statement execution is described by the judgment:

$$
\Gamma \vdash s \Rightarrow \Gamma'
$$

This judgment states: "Statement $s$ executes with starting environment $\Gamma$ and produces successor environment $\Gamma'$."

**Components:**

- $\Gamma$ - the environment before executing the statement, containing variable bindings and their definite-assignment status
- $s$ - the statement being executed
- $\Gamma'$ - the resulting environment after executing the statement, reflecting new bindings, updated definite-assignment tracking, and changes in responsibility

This judgment tracks definite assignment and responsibility transfer throughout statement execution. Implementations **MUST** use this judgment to ensure variables are initialized before use and that responsibility (cleanup obligations) is properly tracked across control flow.

### 21.1 Fundamentals and Outcomes

#### 21.1.1 Control outcomes

Statements **MUST** produce one of the standard control outcomes (`Normal`, `Return(v?)`, `Break(ℓ?, v?)`, `Continue(ℓ?)`, or `Panic`), and control-transfer outcomes **MUST** bypass the remainder of the current sequence, propagating outward per the formal rules.

#### 21.1.2 Termination syntax

Statement termination **MUST** follow the lexical rules: newline terminators unless a continuation predicate applies, with optional semicolons separating statements.

### 21.2 Variable Declarations

#### 21.2.1 Execution semantics

Variable-declaration statements **MUST** introduce bindings into the current scope and **MUST** participate in definite-assignment analysis, obeying all formation constraints from Clause 10.

### 21.3 Assignments

#### 21.3.1 Evaluation order

Executing `place = expr` **MUST** evaluate the left operand to determine the target place, then evaluate the right operand, then perform the write with permission checks. Compound assignments **MUST** read the place exactly once before writing back.

#### 21.3.2 Permission failures

If the target place lacks sufficient permission, implementations **MUST** emit the specified diagnostic and **MUST NOT** perform the write.

### 21.4 Blocks, `defer`, and Regions

#### 21.4.1 Block scopes and `defer`

Blocks **MUST** introduce scopes for bindings/temporaries, and `defer` blocks **MUST** execute in LIFO order on all exits (normal or control-transfer).

#### 21.4.2 Panic modes

In abort-on-panic mode, panics **MUST** terminate the process immediately and destructors/`defer` blocks **MUST NOT** run. Implementations **MUST** document whether the default panic behavior is unwind or abort.

#### 21.4.3 Region integration

Region blocks **MUST** integrate arena allocation with automatic cleanup, and escape analysis **MUST** prevent region-allocated values from escaping, following the region rules in Clause 18.

### 21.5 Return, Break, Continue

#### 21.5.1 Control transfer statements

`return` **MUST** exit the innermost procedure and **MUST** carry a value matching the declared return type if present. `break`/`continue` **MUST** target the innermost matching loop (or explicit label) with values typed according to the loop’s break type, and outcome propagation **MUST** follow the formal rules.

### 21.6 Unsafe Blocks

#### 21.6.1 Unsafe-only operations

Operations requiring `unsafe` (raw pointer deref/write, pointer arithmetic, `transmute`, calling `unsafe` functions, inline assembly, FFI) **MUST** be permitted only inside `unsafe { … }`, and attempts outside **MUST** emit the corresponding diagnostics.

#### 21.6.2 Safety obligations

Code inside `unsafe` **MUST** uphold all stated safety obligations (validity, alignment, initialization, lifetime). Violations remain unverifiable behavior.

---

## 22. Control Flow and Pattern Matching

### 22.1 Conditionals and Blocks

#### 22.1.1 Conditional completeness

`if` expressions **MUST** be structured expressions whose values are discarded when used as statements, and any missing required `else` branch **MUST** be diagnosed.

### 22.2 `match` Expressions

#### 22.2.1 Exhaustiveness and guards

`match` expressions **MUST** be exhaustive; non-exhaustive matches **MUST** be rejected and unreachable arms **MUST** be diagnosed. Guards **MUST** execute only after pattern success, and or-patterns **MUST** bind the same identifiers with compatible types.

#### 22.2.2 Statement usage

When used as statements, `match` expression values **MUST** be discarded while preserving control-flow semantics.

### 22.3 Loops

#### 22.3.1 Loop semantics

Loop expressions **MUST** follow the structured-expression rules; when used as statements, their values are discarded. Infinite loops without `break` **MUST** have type `!` and never complete normally.

#### 22.3.2 Break/continue constraints

`break`/`continue` outside a loop **MUST** be rejected, and typed loops **MUST** enforce consistent break values.

### 22.4 Short-Circuiting

#### 22.4.1 Permitted short-circuiting

Only logical `&&` and `||` **MAY** short-circuit (left-to-right). All other operators **MUST NOT** short-circuit.

---

## 23. Error Handling and Panics

### 23.1 Contracts vs. Runtime Errors

#### 23.1.1 Verification requirements

Preconditions **MUST** encode caller obligations. Implementations **MUST** attempt static verification and **MAY** insert dynamic checks when proofs fail, but any dynamic check **MUST** execute immediately before the call according to the selected verification mode.

#### 23.1.2 Verification modes

Verification modes (`static`, `dynamic`, `trusted`) **MUST** behave as specified, and build flags/attributes **MUST** resolve deterministically to an effective mode.

### 23.2 Panics

#### 23.2.1 Panic semantics

Invoking `panic` **MUST** require the intrinsic `panic` grant. Implementations **MUST** define and document whether the default behavior is unwind-on-panic or abort-on-panic; in abort-on-panic, destructors and `defer` blocks **MUST NOT** run.

#### 23.2.2 FFI boundaries

Panics crossing an FFI boundary **MUST** abort by default. When `[[unwind(catch)]]` is declared, the runtime **MUST** catch the panic at the boundary and map it to the documented ABI error value.

### 23.3 Interoperability with FFI Errors

#### 23.3.1 Foreign error handling

Programs **MUST** check foreign-returned error codes, null pointers, and alignment guarantees before dereference, honoring calling-convention and lifetime obligations; violations constitute unverifiable behavior.

#### 23.3.2 Modeling with unwinding

## When `[[unwind(catch)]]` is used, specifications **SHOULD** model error returns explicitly (e.g., unions) and postconditions **SHOULD** describe the mapping from panic to foreign error values.

## Part VI - Contracts, and Grants

## 14. Contract Language

### 14.1 Overview and Sequent Notation

#### 14.1.1 Canonical form

Every contractual sequent **MUST** attach to a procedure declaration to specify required grants, preconditions (`must`), and postconditions (`will`). The canonical form **MUST** be `[[ G |- P => Q ]]`, and implementations **MUST** accept either semantic brackets `⟦ ⟧` or ASCII `[[ ]]`, turnstile `⊢` or `|-`, and implication `⇒` or `=>`. Abbreviations **MUST** desugar deterministically into the canonical form.

#### 14.1.2 Semantic interpretation

The semantic interpretation **MUST** satisfy: if the caller has grants `G` and `P` holds at entry, then `Q` holds at normal exit. Empty components **MUST** default to `G = ∅`, `P = true`, `Q = true`. Omitted sequents **MUST** default to `[[ |- true => true ]]`.

### 14.2 Components and Scope

#### 14.2.1 Grant component

Grant clauses **MUST** list visible, well-formed qualified identifiers and **MUST NOT** use wildcards.

#### 14.2.2 Preconditions

`must` predicates **MUST** be boolean, pure expressions over parameters (and encoded constraints when present). Using `result` or `@old` inside `must` **MUST** be rejected.

#### 14.2.3 Postconditions

`will` predicates **MUST** be boolean, pure expressions over parameters, `result`, and `@old(expr)` captures taken at entry, and **MUST** hold on every non-diverging return path. Multiple postconditions **MUST** conjoin.

#### 14.2.4 Identifier scope

The identifier `result` **MUST** be in scope only within `will`, and `@old` **MUST** appear only inside `will` without nesting. Violations **MUST** trigger the documented diagnostics.

### 14.3 Grammar and Abbreviation Rules

#### 14.3.1 Parsing and defaulting

Implementations **MUST** parse sequents according to the contract grammar and **MUST** apply smart defaulting so grant-only, precondition-only, or postcondition-only forms expand to `[[ G |- P => Q ]]`. Ambiguous abbreviations **MUST** be rejected.

#### 14.3.2 Placement rules

A sequent **MAY** appear on the same line as the signature or the immediately following line. Expression-bodied pure procedures **MUST NOT** include an explicit sequent; the default applies automatically.

### 14.4 Composition, Subtyping, and Implementation Obligations

#### 14.4.1 Grant composition

A caller’s grant clause **MUST** include the union of grants required by all executed subexpressions and callees. Implementations **MUST** accumulate grants bottom-up and **MUST** verify subset inclusion at call sites, diagnosing missing grants.

#### 14.4.2 Grant subsumption

Callable subtyping **MUST** treat grant inclusion monotonically: fewer grants is more specific, so procedures with `∅` grants **MAY** substitute wherever grants are required.

#### 14.4.3 Liskov requirements

Implementations of contract members **MUST** weaken preconditions and **MUST** strengthen postconditions relative to the contract, diagnosing violations.

#### 14.4.4 Compositional reasoning

Callers **MAY** use callee postconditions as facts to discharge downstream preconditions without inspecting callee bodies (modular checking).

### 14.5 Purity and Compile-Time Execution

#### 14.5.1 Pure procedures

Procedures with empty grant sets **MUST NOT** perform operations requiring grants or call grant-requiring procedures.

#### 14.5.2 Comptime procedures

A procedure callable at compile time **MUST** be declared `comptime procedure`, its grants **MUST** be limited to `{comptime::alloc, comptime::codegen, comptime::config, comptime::diag}`, bodies **MUST** be free of runtime effects, parameter/return types **MUST** be compile-time representable, receivers **MUST NOT** appear, and resource limits/diagnostics **MUST** be enforced.

---

## 15. Grants

### 15.1 Grant Taxonomy

#### 15.1.1 Built-in grants

Implementations **MUST** provide the built-in grant catalog (allocation, filesystem, networking, standard I/O, concurrency, system, unsafe, FFI, panic) as listed in the text.

#### 15.1.2 Comptime-only grants

Comptime-only grants **MUST** be usable exclusively during compile-time execution and **MUST NOT** appear in runtime procedures.

#### 15.1.3 User-defined grants

User-defined grants **MUST** be declared at module scope with standard visibility modifiers and **MUST** follow the same checking rules as built-ins, respecting reserved-namespace constraints.

### 15.2 Availability, Propagation, and Intrinsics

#### 15.2.1 Lexical availability

Inside a procedure body, the available grant set **MUST** equal the set declared in the procedure’s sequent, and grants **MUST** remain lexically scoped without inheriting across nested declarations.

#### 15.2.2 Propagation

Expression typing **MUST** accumulate grants via set union, and the caller’s sequent **MUST** include at least that union. Intrinsic operations (e.g., `^`, raw pointer deref, panic, FFI, `to_heap`) **MUST** contribute their intrinsic grants.

#### 15.2.3 Witness dispatch

Methods invoked via witnesses **MUST** propagate the callee’s grants to the call site, and callers **MUST** possess those grants.

### 15.3 Grant Checking

#### 15.3.1 Call-site inclusion

For any call to `f` with required grants `G_f`, the caller’s declared grants `G_c` **MUST** satisfy `G_f ⊆ G_c`; otherwise the implementation **MUST** emit a missing-grant diagnostic listing the absent grants.

#### 15.3.2 Accumulation bounds

The union of all grants used within a body **MUST** remain within the declared grant clause; exceeding it **MUST** produce a diagnostic.

#### 15.3.3 Visibility requirements

Grant identifiers **MUST** be visible and resolvable, and unresolved or reserved-namespace misuse **MUST** be diagnosed.

### 15.4 Grant Polymorphism

#### 15.4.1 Grant parameters

Procedures **MAY** quantify over grant sets via parameters such as `ε`. At instantiation, `ε` **MUST** be replaced by the callable argument’s grant set, and bounds (e.g., `ε ⊆ {io::write, io::read}`) **MUST** be enforced. Implementations **SHOULD** infer `ε` from the argument’s grant set when possible.

#### 15.4.2 Bound enforcement

Violations of grant-parameter bounds **MUST** be rejected, and callers **MUST** include both explicit and parameter-derived grants in their own grant clauses.

### 15.5 Callable Types and Purity

#### 15.5.1 Callable representation

Function types **MUST** carry parameter/return types along with the required grant set and sequent. Pure callables **MUST** have `∅` grants and the trivial sequent, and receivers **MUST** desugar to the first parameter `Self` with appropriate permission qualifiers.

---

## 16. Diagnostics for Contracts and Effects

### 16.1 Sequent Syntax Diagnostics (Parsing/Well-Formedness)

#### 16.1.1 Required diagnostics

Implementations **MUST** emit diagnostics for at least: invalid brackets, missing/duplicate turnstiles or implication symbols, incorrect component order, effectful predicates, undefined grants, illegal use of `result`/`@old`, nested `@old`, and ambiguous abbreviations.

### 16.2 Checking Flow Diagnostics (Typing/Verification)

#### 16.2.1 Missing grants

When `G_f ⊄ G_c`, implementations **MUST** emit the canonical missing-grant diagnostic listing each absent grant and the callee that required it.

#### 16.2.2 Static mode failures

In `[[verify(static)]]`, any unproven pre-/postcondition **MUST** reject compilation with the specified diagnostic.

#### 16.2.3 Excess accumulation

If a procedure’s accumulated grant set exceeds its declaration, implementations **MUST** issue the corresponding diagnostic.

#### 16.2.4 Witness dispatch

Insufficient grants for witness method calls **MUST** be diagnosed according to the witness rules.

### 16.3 Preconditions (Must) - Verification and Runtime

#### 16.3.1 Predicate requirements

Preconditions **MUST** be boolean and pure. Implementations **MUST** attempt static discharge and, in dynamic modes, **MUST** insert runtime checks immediately before the call when a predicate is not statically provable. Violations **MUST** panic with a predicate identifier.

#### 16.3.2 Liskov compliance

Contract implementations **MUST NOT** strengthen preconditions relative to their contracts; violations **MUST** be diagnosed under Liskov rules.

### 16.4 Postconditions (Will) - Obligations and Enforcement

#### 16.4.1 Runtime enforcement

Postconditions **MUST** hold on every non-diverging return path, and when unprovable in dynamic modes, implementations **MUST** insert checks at each return site.

#### 16.4.2 Liskov strengthening

Contract implementations **MUST** strengthen postconditions relative to the contract; violations **MUST** be diagnosed.

### 16.5 Verification Modes

#### 16.5.1 Mode semantics

Implementations **MUST** support `verify(static|dynamic|trusted)` attributes and **MUST** follow the build-mode policy for defaults. Static mode **MUST** prove or reject, dynamic mode **MUST** insert checks, and trusted mode **MUST** skip checks.

### 16.6 FFI, Unsafe, and Intrinsics

#### 16.6.1 Intrinsic grant enforcement

Capabilities that intrinsically require grants (FFI calls, raw pointer dereference, panic, region/heap allocation) **MUST** be accounted for during grant accumulation and **MUST** be reported if missing. Unsafe operations **MUST** also satisfy their separate safety obligations.

---

### Conformance Summary (Part VI)

A conforming implementation **shall**:

- Parse and canonicalize sequents, enforce scope/purity, and desugar abbreviations deterministically.
- Track and check grants through accumulation, composition, subsumption, and witnesses; diagnose missing or excessive grants.
- Enforce Liskov rules for contract implementations (precondition weakening, postcondition strengthening).
- Support verification modes and their compile-/run-time effects.
- Respect comptime restrictions and the comptime grant set.

---

## Part VII - Abstraction, Behaviors, and Dynamic Dispatch

## 17. Behaviors (Interfaces/Traits)

### 17.1 Purpose and Model

#### 17.1.1 Behavioral definition

A behavior **MUST** provide reusable, concrete implementations (procedures and associated types) that types attach to gain functionality via static dispatch (monomorphization). Contracts **MUST** remain the mechanism for abstract requirements without bodies, and this distinction **MUST** be preserved.

#### 17.1.2 Procedure bodies

Every procedure declared inside a behavior **MUST** have a body; behavior declarations lacking bodies **MUST** be rejected (E-10-401).

### 17.2 Declaration Syntax and Items

#### 17.2.1 Grammar compliance

Behavior declarations **MUST** follow the grammar in [NDF_REF], supporting optional generics, optional `with` extensions, and bodies containing associated type and procedure declarations.

#### 17.2.2 `Self` semantics

Within a behavior body, `Self` **MUST** denote the implementing type and **MUST** be usable in signatures, bodies, bounds, and type expressions, resolving at monomorphization.

#### 17.2.3 Associated types

Associated types **MAY** declare optional defaults, and implementing types **MAY** override them. Declarations **MUST** use the form `type Name [= Default] [: Bounds]`.

#### 17.2.4 Receiver requirements

Behavior procedures **MUST** declare a receiver (`~`, `~%`, or `~!`) that determines permissible operations on `self`, and receiver permissions **MUST** constrain procedure bodies accordingly.

### 17.3 Extension (Behavior Inheritance)

#### 17.3.1 Extension rules

A behavior **MAY** extend others via `with`; such extensions **MUST** import the extended behavior’s procedures and associated types. Cyclic extensions **MUST** be rejected (E-10-402).

#### 17.3.2 Conflict resolution

When multiple extended behaviors declare the same procedure, identical signatures **MAY** share one implementation; differing signatures **MUST** be overridden explicitly by the extending behavior or E-10-413 **MUST** be emitted.

#### 17.3.3 Bound entailment

If behavior `P` extends `Q`, then `T: P` **MUST** entail `T: Q` during bound checking and overload resolution.

### 17.4 Implementations and Coherence

#### 17.4.1 Attachment forms

Types **MAY** attach behaviors inline (`with Behavior`) or via standalone `behavior B for T`; both forms **MUST** obey the same rules.

#### 17.4.2 Coherence rule

For any `(Behavior, Type)` pair, at most one visible implementation **MUST** exist per compilation context; duplicates **MUST** be rejected (E-10-506) and overlapping blanket implementations **MUST** be rejected (E-10-507).

#### 17.4.3 Orphan rule

Implementations **MUST** satisfy the orphan rule (the behavior or the type constructor must be local); violations **MUST** raise E-10-505.

#### 17.4.4 No specialization

More specific implementations **MUST NOT** override general ones; specialization is unsupported.

#### 17.4.5 Completeness

Implementations **MUST** provide all required procedures and associated types with compatible signatures, receivers, and sequents; otherwise diagnostics per [NDF_REF] **MUST** be issued.

### 17.5 Interaction with Permissions

#### 17.5.1 Receiver permissions

Receiver permissions **MUST** govern permissible operations on `self`: `~` forbids mutation, `~%` permits shared mutation subject to partition rules, and `~!` permits exclusive mutation. These constraints **MUST** be enforced by the LPS.

#### 17.5.2 Concurrency safety

Thread-safety markers (`Send`/`Sync`) **MUST NOT** exist; concurrency safety **MUST** be expressed solely via permissions (`const` for sharing, `unique` for transfer, `shared` with explicit synchronization).

---

## 18. Witness System (Runtime Polymorphism)

### 18.1 Overview and Design Principles

#### 18.1.1 Witness definition

A witness **MUST** be a dense pointer packaging a value with runtime evidence (behavior implementation, contract satisfaction, or modal state). Static dispatch **MUST** remain the default, and witnesses **MUST** be an explicit opt-in mechanism.

#### 18.1.2 Representation unification

All witness kinds **MUST** share a unified representation and construction pipeline while remaining semantically distinct, and implementations **MUST** support behavior, contract, and modal state witnesses.

### 18.2 Types, Allocation States, and Permissions

#### 18.2.1 Witness type syntax

Witness types **MUST** follow the syntax `witness<Property>@State`, where `Property` is a behavior, contract, or modal state, and omitted states **MUST** default to `@Stack`.

#### 18.2.2 Allocation states

Allocation states **MUST** control responsibility: `@Stack` (non-responsible), `@Region` (responsible at region exit), and `@Heap` (responsible at scope exit). Transitions **MUST** be explicit and follow `@Stack → @Region → @Heap`.

#### 18.2.3 Permission inheritance and grants

Witness formation **MUST** inherit permissions from the source value; upgrades **MUST NOT** occur. Creating responsible witnesses **MUST** use `move`, required grants (`alloc::region`, `alloc::heap`) **MUST** be enforced, and region witnesses **MUST NOT** escape their region.

### 18.3 Representation and Storage

#### 18.3.1 Dense pointer representation

Witnesses **MUST** be represented as dense pointers (data pointer + metadata pointer). Size **MUST** be `2 × sizeof(Ptr<T>)`, alignment **MUST** equal pointer alignment, and witness tables **MUST** be statically allocated (shared when possible).

#### 18.3.2 ABI uniformity

Representation **MUST** be uniform across concrete types to ensure ABI stability for `witness<B>` in parameters, returns, and fields. Type erasure **MUST** hide the concrete type; recovery **MAY** be implementation-defined.

### 18.4 Construction

#### 18.4.1 Formation checks

Witness formation **MUST** verify that the source type satisfies the witnessed property (diagnosing E-13-001 otherwise). Automatic coercion from implementing types **MUST** be supported, and responsible formations **MUST** use `move` (E-13-010).

#### 18.4.2 Allocation guarantees

Construction **MUST NOT** perform hidden dynamic allocations beyond those implied by the chosen allocation state, and dispatch **MUST NOT** allocate.

### 18.5 Dispatch Semantics

#### 18.5.1 Invocation syntax

Method invocation syntax **MUST** use `::` identical to static calls; the witness type alone determines dynamic dispatch.

#### 18.5.2 Dispatch algorithm

Dispatch **MUST** load the witness table, fetch the function pointer, and perform an indirect call. Method existence **MUST** be checked statically (E-12-020).

#### 18.5.3 Grant propagation

Grant requirements **MUST** mirror static calls: target method grants **MUST** be available at the call site (E-12-022 if missing).

#### 18.5.4 Performance guarantees

Per call, implementations **MUST** limit overhead to one metadata load, one vtable entry load, and one indirect call. Implementations **MAY** common-subexpression-eliminate vtable loads or erase to static dispatch when monomorphizable, preserving semantics.

### 18.6 Storage Strategies

#### 18.6.1 Responsibility models

Stack witnesses (`@Stack`) **MUST** be non-responsible references that drop without destructors. Region and heap witnesses **MUST** obey RAII matching their allocation semantics.

#### 18.6.2 Platform documentation

Implementations **MUST** document platform-specific sizes and alignments for witnesses on 32-bit and 64-bit targets.

### 18.7 Diagnostics

#### 18.7.1 Diagnostic coverage

Implementations **MUST** emit the cataloged witness diagnostics: formation (E-12-001, E-12-010, E-12-011, E-12-012), dispatch (E-12-020…E-12-022), and memory integration (E-12-030…E-12-032), with payloads including source/target types, method, state, permission, and grants as applicable.

---

## 19. Generics & Behaviors Integration

### 19.1 Bounds and Associated Types

#### 19.1.1 Bound satisfaction

Generic bounds **MUST** accept behaviors or contracts, and satisfaction **MUST** require a visible implementation (`behavior B for T`) or contract clause per the bound. Multiple bounds **MUST** be satisfied simultaneously, and associated-type equalities **MUST** reference declared associated types.

#### 19.1.2 Entailment

Bound entailment from behavior extension **MUST** be applied during checking (e.g., `T: Debug` implies `T: Display` when `Debug with Display`).

### 19.2 Static vs Dynamic Dispatch Selection

#### 19.2.1 Default dispatch policy

When types are known at compile time, implementations **MUST** prefer static verification and monomorphization; witnesses remain an opt-in mechanism for runtime flexibility.

#### 19.2.2 Use cases

Behavior witnesses **SHOULD** be used for heterogeneous collections or runtime-determined types; contract and modal witnesses follow analogous guidance.

### 19.3 Monomorphization and Performance (Informative)

1. Static dispatch eliminates vtable overhead; dynamic dispatch via witnesses incurs 1-2 pointer loads and an indirect call. Code size vs. flexibility trade-offs are documented; implementations **may** erase witnesses to direct calls when monomorphizable.
2. Resolution and monomorphization **must** follow the generic resolution algorithm and generate specialized code per instantiation while preserving semantics.

---

### Conformance Summary

A conforming implementation **shall**:

- Enforce behavior declaration rules (bodies required, receiver permissions, associated types, extension with conflict handling).
- Enforce implementation rules (coherence, orphan, no specialization, completeness).
- Provide unified witness formation, representation, allocation states, permission inheritance, and grant enforcement; prevent region escapes.
- Implement dynamic dispatch semantics and diagnostics; propagate grants through witness calls.
- Prefer static dispatch by default and document performance characteristics and ABI details for witnesses.

---

## Part VIII - Compile-Time Evaluation & Reflection (No Macros)

## 20. Comptime Execution

### 20.1 Overview

#### 20.1.1 Comptime procedure model

A comptime procedure **MUST** execute during compilation in a dedicated evaluation context, **MAY** compute values for types/initializers, **MAY** query reflection metadata (when enabled), **MAY** emit declarations via code-generation APIs, and **MUST NOT** perform runtime effects or depend on runtime state.

#### 20.1.2 Comptime blocks

A `comptime { ... }` block **MUST** execute at compile time in its enclosing context under the same restrictions that apply to comptime procedures.

### 20.2 Declaring Comptime Procedures

#### 20.2.1 Declaration requirements

A comptime procedure **MUST** use the head `comptime procedure`, **MUST NOT** declare receivers (`~`, `~%`, `~!`), and **MUST** restrict parameter and result types to compile-time-representable forms.

#### 20.2.2 Contract and grant constraints

A comptime procedure’s contract **MUST** use the canonical sequent form and be satisfiable in the compile-time environment. Procedures with non-empty grant sets **MUST** use only comptime grants ([NDF_REF]), and attempts to call non-comptime procedures from comptime **MUST** be rejected.

### 20.3 Execution Semantics and Determinism

#### 20.3.1 Determinism

Comptime evaluation **MUST** be deterministic relative to the build environment (language version, target triple, explicit configuration). Accessing nondeterministic resources **MUST NOT** occur unless provided by APIs that define deterministic semantics.

#### 20.3.2 Side-effect boundaries

Comptime code **MUST NOT** perform runtime I/O, spawn threads, access the network, or allocate into runtime heaps; effects **MUST** remain within the comptime domain (compiler memory, diagnostics, configuration queries, code generation).

#### 20.3.3 Resource limits

Implementations **MUST** define termination/resource ceilings (steps, time, memory), and exceeding a limit **MUST** produce a diagnostic identifying the responsible comptime entity and exhausted resource.

#### 20.3.4 Evaluation order

Comptime execution **MUST** occur after parsing/name binding and before monomorphization/code generation of dependents. Dependency cycles among comptime entities **MUST** be detected and rejected with diagnostics describing the cycle.

### 20.4 Comptime Grants and Effects

#### 20.4.1 Comptime-only grants

Comptime procedures **MUST** use only the comptime-only grants (`comptime::alloc`, `comptime::codegen`, `comptime::config`, `comptime::diag`), listing each required grant in their sequents. Runtime grants **MUST NOT** appear in comptime sequents.

#### 20.4.2 Grant enforcement

The grant set actually used by a comptime body **MUST** be a subset of its declared grants; missing grants at call sites **MUST** be diagnosed, and grant accumulation **MUST** follow the runtime union rules.

#### 20.4.3 Runtime invocation

A comptime procedure **MAY** be called at runtime only when its grant set is empty and its semantics are pure; otherwise the call **MUST** be rejected as context-mismatched.

### 20.5 Results, Constness, and Embedding

#### 20.5.1 Embedding requirements

Values produced by comptime execution and embedded into program artifacts **MUST** be serializable into the compiled program without hidden side effects.

#### 20.5.2 Runtime constness

A value returned from a comptime procedure **MUST** be treated as a runtime constant unless explicitly wrapped by code-generation APIs that emit mutable declarations.

### 20.6 Diagnostics

#### 20.6.1 Failure reporting

Comptime failures (unsatisfied preconditions, missing grants, resource exhaustion) **MUST** stop compilation of dependents and **SHOULD** include source locations for both caller and callee.

#### 20.6.2 Use-site diagnostics

Comptime values used in types or constant expressions that fail to evaluate **MUST** emit diagnostics at the use site with a causal chain back to the failing comptime entity.

---

## 21. Opt-In Reflection

### 21.1 Enabling Reflection

#### 21.1.1 Opt-in requirement

Reflection **MUST** be opt-in: declarations are non-reflectable by default, and applying `[[reflect]]` **MUST** enable metadata generation for the declaration and its directly nested members unless explicitly disabled inside.

#### 21.1.2 Attribute applicability

`[[reflect]]` **MAY** be applied to modules, records/enums/unions, behaviors and implementations, procedures (including receivers/parameters), and type aliases; finer-grained attributes on fields/variants **MAY** be supported.

### 21.2 Availability and Phase Restrictions

#### 21.2.1 Phase restrictions

Reflection APIs **MUST** be callable only at compile time; runtime calls **MUST** be rejected. Implementations **MAY** provide comptime-only namespaces (e.g., `reflect::*`).

#### 21.2.2 Metadata purity

Reflection metadata **MUST** remain immutable and side-effect free. Queries **MUST NOT** allocate in runtime heaps or observe evaluation order of unrelated declarations.

### 21.3 Query Surface

#### 21.3.1 Minimum query set

Implementations **MUST** provide the documented reflection queries (`type_id`, `type_name`, `type_info`, `proc_info`, `behavior_info`/`impl_info`, `module_members`). Record layouts **MUST** be documented and stable within each language version.

### 21.4 Security and Privacy

#### 21.4.1 Visibility guarantees

Unreflected declarations **MUST NOT** be discoverable via reflection APIs, and implementations **MUST NOT** leak private/internal details unless exposed elsewhere.

#### 21.4.2 Information disclosure limits

Reflection **MUST NOT** expose code text or source spans unless a separate `comptime::diag`-guarded facility grants access.

#### 21.4.3 Reproducibility guidance

Implementations **SHOULD** avoid embedding absolute paths or environment-specific data in reflection outputs to preserve reproducibility.

### 21.5 Diagnostics

#### 21.5.1 Diagnostic requirements

Using reflection in runtime contexts **MUST** emit phase-misuse diagnostics, and cross-module reflection **MUST** respect visibility (rejecting non-public declarations unless exported with `[[reflect]]`).

---

## 22. Code Generation APIs (Explicit Metaprogramming)

### 22.1 Principles (No Macros)

#### 22.1.1 Typed APIs only

Cursive **MUST NOT** provide textual macros; all metaprogramming **MUST** use typed, hygienic APIs that create declarations explicitly in the compiler’s IR.

#### 22.1.2 Semantic parity

Generated declarations **MUST** have the same semantics as hand-written code, **MUST** carry explicit visibility, and **MUST** appear in well-defined scopes.

### 22.2 Emission Model

#### 22.2.1 Grant requirement

Code-generation APIs **MUST** require `comptime::codegen`, and emission **MUST** be explicit (no implicit emission from constructing quotations).

#### 22.2.2 Symbol-table integration

Emitted items **MUST** enter the current symbol table at a well-defined point (end of the enclosing comptime block/procedure) and **MUST** participate in subsequent resolution/type checking identically to ordinary declarations.

#### 22.2.3 Cycle rejection

Implementations **MUST** reject emission attempts that would create dependency cycles unrecoverable by standard analysis.

### 22.3 Quotation and Typed Builders

#### 22.3.1 Quotation semantics

Quotation constructs (e.g., `quote { ... }`) **MUST** produce typed code values with interpolations type-checked at quoting time, and quoted values **MUST** remain inert until explicitly emitted.

#### 22.3.2 Builder requirements

Builder APIs **MUST** require explicit names, visibility, attributes (contracts/grants), and bodies, diagnosing missing components before emission.

### 22.4 Name Hygiene and Scoping

#### 22.4.1 Hygiene guarantees

Emitted names **MUST** be hygienic by default and **MUST NOT** capture or shadow unrelated bindings; APIs **SHOULD** provide fresh-name generation.

#### 22.4.2 Scope compliance

Emitted declarations **MUST** obey module/block scoping rules, and emitting into another module **MUST** require visibility/mutability in the current build context.

### 22.5 Interaction with Contracts and Grants

#### 22.5.1 Sequents for emitted procedures

Emitted procedures **MAY** include explicit sequents (defaulting to `[[ |- true => true ]]` when omitted), and implementations **MUST** type-check emitted bodies against declared grants at emission time.

#### 22.5.2 Grant constraints

Emitting code that requires non-comptime grants **MUST** be rejected during comptime; such code **MAY** only run at runtime after standard type checking.

### 22.6 Error Reporting from Comptime

#### 22.6.1 Diagnostic support

Code-generation APIs **MUST** support emitting diagnostics with source locations mapped to the quoting site and generated declaration, requiring `comptime::diag`.

#### 22.6.2 Failure reporting

Emission failures (duplicate names, visibility violations, invalid contracts, cycles) **MUST** produce precise diagnostics with suggested fixes.

### 22.7 ABI and Reproducibility

#### 22.7.1 Deterministic emission

Code generation **MUST** be deterministic for a fixed build environment; template-generated identifiers **MUST** remain stable unless fresh names are explicitly requested.

#### 22.7.2 Stability documentation

Implementations **MUST** document which reflection metadata and generated code are stable per language version and which remain implementation-defined.

---

### Conformance Summary (Part VIII)

A conforming implementation **shall**:

- Enforce the comptime execution model (phase ordering, determinism, resource ceilings, no runtime effects).
- Enforce comptime grants, reject runtime grants in comptime contexts, and check grant accumulation.
- Provide opt-in reflection guarded by `[[reflect]]`, available only at compile time, with a documented and stable query surface.
- Provide typed, hygienic code-generation APIs requiring `comptime::codegen`, with explicit emission points and full type/contract checking of generated declarations.
- Provide precise diagnostics for comptime failures, reflection misuse, and emission errors, and preserve build reproducibility.

---

## Part IX - Memory Model, Layout, and ABI

## 23. Memory Layout & Alignment

### 23.1 Representation Attributes

#### 23.1.1 Supported representation attributes

Implementations **MUST** support the attributes `[[repr(C)]]`, `[[repr(packed)]]`, and `[[repr(align(N))]]` on nominal types. `[[repr(C)]]` guarantees C-compatible layout (field order, padding, tail padding) with padding bytes remaining indeterminate; `[[repr(packed)]]` forces minimal offsets and alignment 1 without silently fixing misalignment; `[[repr(align(N))]]` forces a minimum alignment of `N` bytes and **MUST** diagnose `N` values smaller than the natural alignment.

#### 23.1.2 Attribute combination rules

`[[repr(C)]]` **MAY** combine with `[[repr(packed)]]` or `[[repr(align(N))]]` where the target ABI supports the combination. When `[[repr(C)]]` and `[[repr(packed)]]` are combined, the effective alignment **MUST** remain 1 even if C would require more, and implementations **MUST** document any platform-specific restrictions.

#### 23.1.3 Enum discriminant selection

C-compatible enums **MUST** declare a discriminant representation with `[[repr(C, tag_type = "...")]]`, where `tag_type ∈ {u8, u16, u32, u64, i8, i16, i32, i64}`; any other type **MUST** be rejected.

#### 23.1.4 FFI-safety enforcement

Types annotated with `[[repr(C)]]` **MUST** satisfy the FFI-safety restrictions from Part I and Clause 34 (only FFI-safe fields, no permission-qualified or modal-only fields), and violations **MUST** be diagnosed.

### 23.2 Natural Alignment

#### 23.2.1 Power-of-two alignment

A type’s alignment **MUST** be a power of two, and implementations **MUST** enforce the canonical alignment table for primitives, pointers, tuples, records, and arrays.

#### 23.2.2 Misalignment semantics

Violating an alignment requirement (for example via unsafe casts) **MUST** be treated as unverifiable behavior on targets that demand strict alignment; implementations **SHOULD** document which targets trap or emulate misaligned accesses.

### 23.3 Explicit Alignment `[[repr(align(N))]]`

#### 23.3.1 Alignment-raising semantics

Applying `[[repr(align(N))]]` **MUST** raise the annotated type’s alignment to at least `N`, **MUST** require all objects of the type to reside at addresses that are multiples of `N`, and **MUST** reject any annotation where `N` is less than the natural alignment.

#### 23.3.2 Usage guidance

Implementations **SHOULD** support cache-line and device-alignment use cases by honoring large `N` values provided the target ABI can allocate such alignments; where the ABI imposes limits, exceeding them **MUST** trigger a diagnostic.

### 23.4 Size and Layout (Records/Tuples)

#### 23.4.1 Field ordering

Fields in records and tuples **MUST** be laid out in declaration order, with each field offset chosen as the smallest value ≥ the prior field’s end that satisfies `offset mod align(field) == 0`.

#### 23.4.2 Aggregate alignment and tail padding

Aggregate alignment **MUST** equal the maximum field alignment unless `[[repr(packed)]]` is present, and the final size **MUST** be rounded up to a multiple of that alignment (tail padding included).

#### 23.4.3 Padding byte semantics

Padding bytes **MUST** be treated as unspecified data; programs **MUST NOT** read padding, and doing so constitutes unverifiable behavior.

#### 23.4.4 `repr(C)` layout parity

For `[[repr(C)]]` aggregates, implementations **MUST** match the platform C compiler’s layout (offsets, padding, tail padding) without requiring programmers to insert manual padding, though programmers **MAY** add padding fields for clarity or legacy ABI compatibility.

### 23.5 Arrays

#### 23.5.1 Array layout formulas

Arrays **MUST** satisfy `size([T; n]) = n × size(T)` and `align([T; n]) = align(T)`, ensuring contiguous storage whose alignment derives from the element type.

### 23.6 Packed Layout and Misalignment

#### 23.6.1 Packed alignment rule

Types annotated with `[[repr(packed)]]`, alone or combined with `[[repr(C)]]`, **MUST** have alignment 1, and compilers **MUST NOT** assume hardware will transparently correct misaligned accesses.

#### 23.6.2 Misaligned access safety

Dereferencing misaligned addresses through ordinary loads/stores **MUST** be considered unverifiable behavior on targets lacking hardware support, so programmers **MUST** use explicit unaligned-load helpers or copy via byte buffers when aligned access is required.

#### 23.6.3 Usage guidance

Packed layout **SHOULD** be reserved for ABI-constrained scenarios (e.g., GPU headers, firmware tables), and implementations **SHOULD** encourage compile-time offset validation to mitigate the performance and safety trade-offs.

### 23.7 Pointer Provenance (Overview)

#### 23.7.1 Provenance tracking

Safe pointers **MUST** carry provenance metadata (`Stack`, `Region(r)`, `Heap`), and escape analysis **MUST** reject attempts to pass stack/region addresses into contexts requiring longer lifetimes; raw pointers bypass provenance checks and therefore **MUST** require `unsafe` blocks.

### 23.8 Data-Race Model (Baseline)

#### 23.8.1 Single-threaded ordering

In single-threaded execution, operations **MUST** observe program order with deterministic evaluation as defined in Part V.

#### 23.8.2 Concurrency guarantees

Concurrency semantics (happens-before, synchronizes-with, DRF) **MUST** follow the Concurrency clause: well-typed code without `unsafe` **MUST** remain data-race-free via the permission system (`const`, `unique`, `shared` plus synchronization).

#### 23.8.3 Foreign code obligations

Foreign code lies outside the permission model, so maintaining thread safety across FFI boundaries **MUST** be treated as an explicit contractual obligation.

---

## 24. Interoperability & FFI

### 24.1 FFI Declarations

#### 24.1.1 Extern attribute requirement

Foreign procedures **MUST** use the `[[extern("...")]]` attribute with a supported calling-convention string, and may attach `no_mangle`, `weak`, or `unwind(...)` as documented; the grammar for these attributes **MUST** match Clause 14.

#### 24.1.2 Supported calling conventions

Implementations **MUST** support at least `"C"` plus platform-conventional variants (e.g., `"stdcall"`, `"system"`), **MUST** document any additional conventions, and **MUST** diagnose mismatched conventions whenever they are detectable, because such mismatches constitute unverifiable behavior.

### 24.2 FFI-Safe Types

#### 24.2.1 Signature constraints

Extern signatures **MUST** contain only FFI-safe types: primitives, raw pointers to FFI-safe types, `[[repr(C)]]` records/enums, and extern function pointers. Using permission-qualified, modal, or managed-string types **MUST** be rejected.

#### 24.2.2 Variadic prohibition

Variadic externs are unsupported in this edition, so any attempt to declare one **MUST** be diagnosed.

### 24.3 Grants and Safety at the Boundary

#### 24.3.1 Required grants

Every extern declaration **MUST** declare `ffi::call`, and any signature that passes or returns pointers **MUST** add `unsafe::ptr`; missing grants **MUST** be diagnosed.

#### 24.3.2 Unsafe obligations

Code inside `unsafe` blocks **MUST** uphold pointer validity (non-null, properly aligned, initialized, live). Violations produce unverifiable behavior even when the call crosses an FFI boundary.

### 24.4 Calling Conventions & Linkage Hints

#### 24.4.1 Attribute semantics

`[[extern(C)]]` **MUST** imply unmangled symbol names, `[[no_mangle]]` **MUST** suppress mangling for any extern, `[[weak]]` **MAY** declare a weak symbol subject to platform support, and applying linkage attributes to non-public declarations **MUST** be rejected.

#### 24.4.2 Tooling disclosure

Implementations **SHOULD** provide demangling tools and **SHOULD** document the exact mangling scheme whenever mangling applies so that linkers and tooling remain interoperable.

### 24.5 C Strings, Pointers, and Lifetimes

#### 24.5.1 Null-terminated attribute

The `[[null_terminated]]` parameter attribute **MUST** document C-string expectations; the compiler does not synthesize terminators, so callers **MUST** ensure null termination prior to the call.

#### 24.5.2 Library conversions

Standard library conversions **MUST** be explicit: `string@... .as_c_ptr()` (Cursive→C) yields a raw pointer valid for the source string’s lifetime, and `string::from_c_str(*const u8 [[null_terminated]])` (C→Cursive) **MUST** validate and copy the incoming data before producing a managed string.

#### 24.5.3 Lifetime restrictions

Passing pointers derived from ephemeral storage (temporaries, region data) beyond their lifetime **MUST** be rejected or diagnosed, and implementers **SHOULD** encourage callers to allocate managed or heap storage when longer lifetimes are required.

### 24.6 Panic and Unwinding Across FFI

#### 24.6.1 Default panic policy

The default for extern procedures called from foreign code **MUST** be abort-on-panic, and unwinding **MUST NOT** cross the FFI boundary unless `[[unwind(catch)]]` is specified. When `catch` is present, implementations **MUST** document the panic-to-error mapping they use.

### 24.7 Diagnostics (FFI)

#### 24.7.1 Diagnostic coverage

Implementations **MUST** provide diagnostics for at least null-dereference hazards, calling-convention mismatches, misuse of region/stack pointers across FFI, and general ODR/linkage issues so that boundary violations are actionable.

---

## 25. Linkage, ODR, and Binary Compatibility

### 25.1 Linkage Categories and Determination

#### 25.1.1 Linkage categories

Linkage **MUST** be treated as distinct from visibility: every entity has exactly one of external linkage (exported symbol), internal linkage (unit-local symbol), or no linkage (no symbol). Public module-scope declarations **MUST** receive external linkage automatically unless explicitly overridden.

#### 25.1.2 Symbol emission policy

Implementations **MUST** emit external-linkage symbols, **MUST** keep internal-linkage symbols local to their object file, and **MUST** omit entities without linkage from symbol tables.

### 25.2 One Definition Rule (ODR)

#### 25.2.1 Single-definition requirement

For each externally linked entity (type or procedure), a program **MUST NOT** provide multiple conflicting definitions. Multiple declarations without bodies are permitted, but a second body **MUST** be rejected unless it is an inline/generic instantiation eligible for linker deduplication.

#### 25.2.2 ODR diagnostics

ODR violations detected at link time **MUST** be diagnosed, and implementations **MUST** document that undetected violations result in unverifiable behavior.

### 25.3 Name Mangling and Attributes

#### 25.3.1 Mangling documentation

Name mangling **MUST** be treated as implementation-defined but documented, `[[no_mangle]]` **MUST** inhibit mangling, and `[[extern(C)]]` **MUST** imply the C ABI’s unmangled symbol form.

#### 25.3.2 Weak symbol support

`[[weak]]` **MAY** declare overridable defaults, but implementations **MUST** document whether the platform supports weak linkage and how it behaves.

### 25.4 ABI Policies

#### 25.4.1 ABI documentation

Because the ABI is platform-specific, implementations **MUST** document the calling conventions, data layouts, register usage, and object file formats they target (e.g., SysV, Windows x64, AAPCS).

#### 25.4.2 `repr(C)` compatibility

Implementations **MUST** maintain `[[repr(C)]]` layout compatibility with the platform C compiler on every supported target.

#### 25.4.3 Tooling support

Implementations **SHOULD** provide symbol-inspection/demangling tools and **SHOULD** warn about ABI-breaking changes when they can be detected statically.

### 25.5 Stable Binary Interfaces (Guidance)

#### 25.5.1 Export stability

Libraries **SHOULD** publish stable entry points using `[[extern(C), no_mangle]]` and versioned procedures rather than silently changing signatures.

#### 25.5.2 Opaque handle guidance

Libraries **SHOULD** expose opaque pointers (handles) to hide representation details from foreign code and preserve ABI stability across releases.

### 25.6 Conformance Summary (Part IX)

A conforming implementation **shall**:

- Enforce representation attributes and layout formulas; diagnose invalid `repr` usage.
- Guarantee alignment rules and detect/diagnose misalignment where possible.
- Implement FFI declarations and attribute semantics (extern/no_mangle/weak/unwind); enforce FFI-safe signatures and required grants.
- Provide calling convention implementations and diagnostics for mismatches.
- Implement linkage determination, ODR rules, symbol emission, and provide tooling for mangling/demangling.
- Document ABI policies and maintain `repr(C)` compatibility.

---

## Appendix A – Formal ANTLR Grammar (Normative)

**[Placeholder]**

This appendix will contain the complete ANTLR grammar for Cursive, providing a machine-readable specification of the language's lexical and syntactic structure.

---

## Appendix B – Diagnostic Code Taxonomy (Normative)

This appendix defines the complete taxonomy of diagnostic codes used throughout the specification. The code schema `K-CCC-FFNN` is defined in [§7.4.1](#74-diagnostic-code-system-diagnosticscodes), and this appendix provides the detailed feature bucket assignments for each category.

### B.1 Feature Bucket Reference

> For the `SRC` category, feature buckets (`FF`) **MUST** follow this table so diagnostic codes remain stable across parts:

| Bucket | Scope (SRC)                                                                      | Examples                        |
| ------ | -------------------------------------------------------------------------------- | ------------------------------- |
| `01`   | Source ingestion and normalization (UTF-8, BOM, control characters, size limits) | `E-SRC-0101`                    |
| `02`   | Translation pipeline and comptime resource enforcement                           | `E-SRC-0201…0208`               |
| `03`   | Lexical tokens, literals, comments                                               | `E-SRC-0301…0307`               |
| `04`   | Statement termination, delimiter handling, EOF conditions                        | `E-SRC-0401…0402`               |
| `05`   | Module-scope form restrictions                                                   | `E-SRC-0501`                    |
| `11`   | Module discovery, manifests, and case collisions                                 | `E-SRC-1101…1104`, `W-SRC-1105` |
| `12`   | Imports and alias formation                                                      | `E-SRC-1201…1203`               |
| `13`   | Module dependency graphs and initialization ordering                             | `E-SRC-1301…1303`               |
| `14`   | Scope tree formation                                                             | `E-SRC-1401`                    |
| `15`   | Shadowing and binding metadata (including universe protections)                  | `E-SRC-1501…1504`               |
| `16`   | Name lookup (unqualified/qualified, alias conflicts)                             | `E-SRC-1601…1607`               |

> Each remaining category **MUST** define disjoint feature buckets with the same two-digit format.

### B.2 `TYP` Category Buckets

| Bucket | Scope (TYP)                                          | Examples          |
| ------ | ---------------------------------------------------- | ----------------- |
| `01`   | Type formation, kinding, alias expansion             | `E-TYP-0101`      |
| `02`   | Declaration well-formedness (records, enums, modals) | `E-TYP-0201…0210` |
| `03`   | Generic parameters, variance, instantiation          | `E-TYP-0301…0308` |
| `04`   | Permission annotations and defaults                  | `E-TYP-0401…0404` |
| `05`   | Contract/witness attachment to types                 | `E-TYP-0501…0505` |

### B.3 `MEM` Category Buckets

| Bucket | Scope (MEM)                                         | Examples          |
| ------ | --------------------------------------------------- | ----------------- |
| `01`   | Responsibility/move semantics                       | `E-MEM-0101…0108` |
| `02`   | Permission lattice violations at runtime boundaries | `E-MEM-0201`      |
| `03`   | Region creation/escape analysis                     | `E-MEM-0301…0306` |
| `04`   | Pointer provenance/dereference state errors         | `E-MEM-0401…0407` |
| `05`   | Concurrency and happens-before diagnostics          | `E-MEM-0501…0504` |

### B.4 `EXP` Category Buckets

| Bucket | Scope (EXP)                                       | Examples          |
| ------ | ------------------------------------------------- | ----------------- |
| `01`   | Expression typing/evaluation order                | `E-EXP-0101…0105` |
| `02`   | Statement/control-flow violations (if/match/loop) | `E-EXP-0201…0210` |
| `03`   | Pipeline/operator misuse                          | `E-EXP-0301…0306` |
| `04`   | Comptime execution failures inside expressions    | `E-EXP-0401…0403` |
| `05`   | Code-generation/quote misuse                      | `E-EXP-0501…0504` |

### B.5 `GRN` Category Buckets

| Bucket | Scope (GRN)                          | Examples          |
| ------ | ------------------------------------ | ----------------- |
| `01`   | Grant accumulation and leakage       | `E-GRN-0101…0104` |
| `02`   | Sequent syntax/verification failures | `E-GRN-0201…0208` |
| `03`   | Contract Liskov compliance           | `E-GRN-0301…0303` |
| `04`   | Witness grant propagation            | `E-GRN-0401…0404` |

### B.6 `FFI` Category Buckets

| Bucket | Scope (FFI)                                              | Examples          |
| ------ | -------------------------------------------------------- | ----------------- |
| `01`   | Signature/ABI conformance (unsupported types, variadics) | `E-FFI-0101…0106` |
| `02`   | Calling convention and linkage attributes                | `E-FFI-0201…0204` |
| `03`   | Panic/unwind boundaries and null-termination rules       | `E-FFI-0301…0305` |
| `04`   | Unsafe pointer obligations crossing FFI                  | `E-FFI-0401…0404` |

### B.7 `DIA` Category Buckets

| Bucket | Scope (DIA)                                | Examples          |
| ------ | ------------------------------------------ | ----------------- |
| `01`   | Diagnostic payload structure/JSON schema   | `E-DIA-0101…0103` |
| `02`   | Tooling hooks (LSP bridges, span encoding) | `E-DIA-0201…0204` |
| `03`   | Diagnostic taxonomy consistency            | `E-DIA-0301…0302` |

### B.8 `CNF` Category Buckets

| Bucket | Scope (CNF)                              | Examples                        |
| ------ | ---------------------------------------- | ------------------------------- |
| `01`   | Feature enablement/disablement controls  | `E-CNF-0101…0103`, `W-CNF-0101` |
| `02`   | Conformance-mode selection conflicts     | `E-CNF-0201…0203`, `W-CNF-0201` |
| `03`   | Versioning/deprecation policy violations | `E-CNF-0301…0303`               |

---

## Appendix C – Implementation-Defined Behavior Index (Normative)

**[Placeholder]**

This appendix will provide a comprehensive index of all implementation-defined behaviors referenced throughout the specification, with cross-references to the defining sections.

---

## Appendix D – Diagnostic Catalog (Normative)

This appendix aggregates every diagnostic defined elsewhere in the specification. Editors **MUST** update this catalog whenever a clause adds, removes, or renames a diagnostic so implementers can audit coverage from a single location.

| Code         | Severity | Clause    | Summary                                                      |
| ------------ | -------- | --------- | ------------------------------------------------------------ |
| `E-SRC-0101` | error    | [NDF_REF] | Invalid UTF‑8 sequence in source input                       |
| `E-SRC-0102` | error    | [NDF_REF] | Source file exceeds documented size limit                    |
| `E-SRC-0103` | error    | [NDF_REF] | Embedded BOM found after the first scalar value              |
| `E-SRC-0104` | error    | [NDF_REF] | Forbidden control character outside literals                 |
| `E-SRC-0201` | error    | [NDF_REF] | Comptime recursion depth exceeds guaranteed minimum          |
| `E-SRC-0202` | error    | [NDF_REF] | Comptime evaluation step budget exceeded                     |
| `E-SRC-0203` | error    | [NDF_REF] | Comptime heap limit exceeded                                 |
| `E-SRC-0204` | error    | [NDF_REF] | String literal exceeds guaranteed size during comptime       |
| `E-SRC-0205` | error    | [NDF_REF] | Collection cardinality exceeds comptime minimum              |
| `E-SRC-0206` | error    | [NDF_REF] | Comptime body requested undeclared or disallowed grants      |
| `E-SRC-0207` | error    | [NDF_REF] | Generated identifier collided with an existing declaration   |
| `E-SRC-0208` | error    | [NDF_REF] | Cycle detected in comptime dependency graph                  |
| `E-SRC-0301` | error    | [NDF_REF] | Unterminated string literal                                  |
| `E-SRC-0302` | error    | [NDF_REF] | Invalid escape sequence                                      |
| `E-SRC-0303` | error    | [NDF_REF] | Invalid character literal                                    |
| `E-SRC-0304` | error    | [NDF_REF] | Malformed numeric literal                                    |
| `E-SRC-0305` | error    | [NDF_REF] | Reserved keyword used as identifier                          |
| `E-SRC-0306` | error    | [NDF_REF] | Unterminated block comment                                   |
| `E-SRC-0307` | error    | [NDF_REF] | Invalid Unicode in identifier                                |
| `E-SRC-0401` | error    | [NDF_REF] | Unexpected EOF during unterminated statement or continuation |
| `E-SRC-0402` | error    | [NDF_REF] | Delimiter nesting depth exceeded supported bound             |
