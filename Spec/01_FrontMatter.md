# The Cantrip Language Specification

**Section**: Title & Conformance | **Part**: Foundations (Part I)
**Previous**: (Start) | **Next**: [Mathematical Foundations](01_MathFoundations.md)

---

**Version:** 1.3.0
**Date:** October 24, 2025
**Status:** Publication Ready
**Specification Type:** Normative
**Target Audience:** Language implementers, compiler writers, tool developers

---
## Abstract

Cantrip is a systems programming language designed for memory safety, deterministic performance, and AI-assisted development. It achieves these goals through:

- **Lexical Permission System (LPS)**: Compile-time memory safety without garbage collection or borrow checking
- **Explicit Contracts**: Preconditions and postconditions as executable specifications
- **Effect System**: Compile-time tracking of side effects, allocations, and I/O
- **Modal System**: State machines as first-class types with compile-time verification
- **Memory Regions**: Explicit lifetime control with zero-overhead allocation
- **Comptime Metaprogramming**: Compile-time code generation without macros
- **File-Based Modules**: Code organization through file system structure

Cantrip compiles to native code with performance matching C/C++ while providing memory safety guarantees through region-based lifetime management.

**Design Philosophy:**
1. **Explicit over implicit** - All effects, lifetimes, and permissions visible in code
2. **Local reasoning** - Understanding code must minimal global context
3. **Zero abstraction cost** - Safety guarantees without runtime overhead
4. **LLM-friendly** - Predictable patterns for AI code generation
5. **Simple ownership** - No borrow checker complexity
6. **No macros** - Metaprogramming through comptime only for predictability

**Safety Model:**
- **Prevents**: Use-after-free, double-free, memory leaks
- **Provides**: Deterministic deallocation, zero GC pauses
- **Does NOT prevent**: Aliasing bugs, data races (programmer's responsibility)

**Conformance:** An implementation conforms to this specification if and only if it satisfies all normative requirements stated herein.

### Key Words for Requirement Levels
The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**,
**RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in
RFC 2119 and RFC 8174 when, and only when, they appear in all capitals.

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


## Table of Contents

---
- [The Cantrip Language Specification](#the-cantrip-language-specification)
  - [Abstract](#abstract)
    - [Key Words for Requirement Levels](#key-words-for-requirement-levels)
    - [Conformance](#conformance)
    - [Document Conventions](#document-conventions)
  - [Table of Contents](#table-of-contents)
  - [1. Notation and Mathematical Foundations](#1-notation-and-mathematical-foundations)
    - [1.1 Metavariables](#11-metavariables)
    - [1.2 Judgment Forms](#12-judgment-forms)
    - [1.3 Formal Operators](#13-formal-operators)
    - [1.4 Conventions](#14-conventions)
  - [2. Lexical Structure](#2-lexical-structure)
    - [2.1 Source Files](#21-source-files)
    - [2.2 Comments](#22-comments)
    - [2.3 Identifiers](#23-identifiers)
  - [3. Abstract Syntax](#3-abstract-syntax)
    - [3.1 Type Syntax](#31-type-syntax)
    - [3.2 Expression Syntax](#32-expression-syntax)
    - [3.3 Value Syntax](#33-value-syntax)
    - [3.4 Pattern Syntax](#34-pattern-syntax)
    - [3.5 Contract Syntax](#35-contract-syntax)
    - [3.6 Effect Syntax](#36-effect-syntax)
- [Part II: Type System](#part-ii-type-system)
  - [4. Types and Values](#4-types-and-values)
    - [4.1 Primitive Types](#41-primitive-types)
      - [4.1.1 Integer Types](#411-integer-types)
      - [4.1.2 Floating-Point Types](#412-floating-point-types)
      - [4.1.3 Boolean Type](#413-boolean-type)
  - [6. Records and Classes](#6-records-and-classes)
    - [6.1 Record Declaration](#61-record-declaration)
    - [6.6 Structure-of-Arrays](#66-structure-of-arrays)
  - [6.5 Union Types](#65-union-types)
    - [6.5.1 Union Declaration](#651-union-declaration)
    - [6.5.2 Union Safety and FFI](#652-union-safety-and-ffi)
  - [7. Enums and Pattern Matching](#7-enums-and-pattern-matching)
    - [7.1 Enum Declaration](#71-enum-declaration)
    - [7.4 Discriminant Values](#74-discriminant-values)
  - [8. Traits](#8-traits)
    - [8.1 Trait Declaration](#81-trait-declaration)
    - [8.6 Trait-Declared Effects](#86-trait-declared-effects)
    - [9.5 Type Bounds](#95-type-bounds)
    - [9.6 Const Generics](#96-const-generics)
    - [10.3 Using Modals](#103-using-modals)
    - [10.4 State-Dependent Data](#104-state-dependent-data)
    - [10.5 Complex State Machines](#105-complex-state-machines)
    - [10.6 State Unions](#106-state-unions)
    - [10.7 Common Fields](#107-common-fields)
    - [10.10 Modal Instantiation](#1010-modal-instantiation)
      - [10.9.1 Resource Lifecycle Pattern](#1091-resource-lifecycle-pattern)
      - [10.9.2 Request-Response Pattern](#1092-request-response-pattern)
      - [10.9.3 Multi-Stage Pipeline Pattern](#1093-multi-stage-pipeline-pattern)
      - [10.9.4 State Recovery Pattern](#1094-state-recovery-pattern)
    - [10.10 Modal Instantiation](#1010-modal-instantiation-1)
  - [11. Modal Formal Semantics](#11-modal-formal-semantics)
    - [11.1 Modal Definition](#111-modal-definition)
    - [11.2 State Machine Graph](#112-state-machine-graph)
    - [11.5 Transition Validity](#115-transition-validity)
    - [11.6 Reachability Analysis](#116-reachability-analysis)
  - [12. State Transition Verification](#12-state-transition-verification)
    - [12.1 Static Verification](#121-static-verification)
    - [12.2 Dynamic Verification](#122-dynamic-verification)
    - [12.3 Exhaustiveness Checking](#123-exhaustiveness-checking)
    - [12.4 State Flow Analysis](#124-state-flow-analysis)
- [Part IV: Functions and Expressions](#part-iv-functions-and-expressions)
  - [13. Functions and Procedures](#13-functions-and-procedures)
    - [13.1 Function Definition Syntax](#131-function-definition-syntax)
    - [13.2 Pure Functions](#132-pure-functions)
    - [13.4 Functions with Contracts](#134-functions-with-contracts)
    - [13.5 Parameters and Permissions](#135-parameters-and-permissions)
      - [13.5.1 Immutable Reference (Default)](#1351-immutable-reference-default)
      - [13.5.2 Owned Parameters](#1352-owned-parameters)
      - [13.5.3 Mutable Parameters](#1353-mutable-parameters)
    - [13.6 Return Values](#136-return-values)
    - [13.7 Procedure Syntax](#137-procedure-syntax)
    - [14.9 Operator Precedence](#149-operator-precedence)
    - [15.4 Loop Control](#154-loop-control)
  - [16. Higher-Order Functions and Closures](#16-higher-order-functions-and-closures)
    - [16.1 Function Types (fn) and Procedure Types (proc)](#161-function-types-fn-and-procedure-types-proc)
    - [16.4 Function Traits](#164-function-traits)
- [Part V: Contract System](#part-v-contract-system)
  - [17. Contracts and Specifications](#17-contracts-and-specifications)
    - [17.1 Contract Overview](#171-contract-overview)
    - [17.2 Preconditions](#172-preconditions)
    - [17.3 Postconditions](#173-postconditions)
    - [17.4 Contract Messages](#174-contract-messages)
    - [17.5 Empty Contracts](#175-empty-contracts)
    - [17.6 Old-Value References](#176-old-value-references)
    - [17.9 Common Contract Patterns](#179-common-contract-patterns)
      - [17.9.1 Bounds Checking Pattern](#1791-bounds-checking-pattern)
      - [17.9.2 Collection Invariant Pattern](#1792-collection-invariant-pattern)
      - [17.9.3 State Consistency Pattern](#1793-state-consistency-pattern)
      - [17.9.4 Resource Conservation Pattern](#1794-resource-conservation-pattern)
      - [17.9.5 Ordering Preservation Pattern](#1795-ordering-preservation-pattern)
      - [17.9.6 Error Handling Contract Pattern](#1796-error-handling-contract-pattern)
  - [18. Contract Formal Logic](#18-contract-formal-logic)
    - [18.1 Assertion Language](#181-assertion-language)
    - [18.2 Hoare Logic](#182-hoare-logic)
    - [18.3 Weakest Precondition](#183-weakest-precondition)
    - [18.5 Contract Soundness](#185-contract-soundness)
  - [19. Invariants](#19-invariants)
    - [19.1 Record Invariants](#191-record-invariants)
    - [19.3 Class Invariants](#193-class-invariants)
    - [19.4 Modal State Invariants](#194-modal-state-invariants)
    - [19.5 Loop Invariants](#195-loop-invariants)
  - [20. Verification and Testing](#20-verification-and-testing)
    - [20.1 Verification Modes](#201-verification-modes)
    - [20.2 Static Verification](#202-static-verification)
    - [20.3 Runtime Verification](#203-runtime-verification)
    - [20.4 Contract Fuzzing](#204-contract-fuzzing)
    - [20.5 Fuzzing Configuration](#205-fuzzing-configuration)
    - [20.6 Integration with Testing](#206-integration-with-testing)
- [Part VI: Effect System](#part-vi-effect-system)
  - [21. Effects and Side Effects](#21-effects-and-side-effects)
    - [21.1 Effect System Overview](#211-effect-system-overview)
    - [21.2 Effect Syntax](#212-effect-syntax)
    - [21.2.7 User-Defined Effects](#2127-user-defined-effects)
    - [21.3 Memory Effects](#213-memory-effects)
    - [21.7 Standard Effect Definitions](#217-standard-effect-definitions)
      - [21.7.1 Importing Standard Effects](#2171-importing-standard-effects)
      - [21.7.2 Core Standard Effects](#2172-core-standard-effects)
      - [21.7.3 Effect Pattern Examples](#2173-effect-pattern-examples)
      - [21.7.4 Custom Effect Composition](#2174-custom-effect-composition)
      - [21.7.5 Effect Documentation Pattern](#2175-effect-documentation-pattern)
    - [21.9 Rendering Effects](#219-rendering-effects)
      - [21.11.2 File System Effects (`fs.*`)](#21112-file-system-effects-fs)
      - [21.11.3 Network Effects (`net.*`)](#21113-network-effects-net)
      - [21.11.4 I/O Effects (`io.*`)](#21114-io-effects-io)
      - [21.11.5 Time Effects (`time.*`)](#21115-time-effects-time)
      - [21.11.6 Threading Effects (`thread.*`)](#21116-threading-effects-thread)
      - [21.11.7 Rendering Effects (`render.*`)](#21117-rendering-effects-render)
      - [21.11.8 Audio Effects (`audio.*`)](#21118-audio-effects-audio)
      - [21.11.9 Input Effects (`input.*`)](#21119-input-effects-input)
      - [21.11.10 Process Effects (`process.*`)](#211110-process-effects-process)
      - [21.11.11 FFI Effects (`ffi.*`)](#211111-ffi-effects-ffi)
      - [21.11.12 Unsafe Effects (`unsafe.*`)](#211112-unsafe-effects-unsafe)
      - [21.11.13 System Effects](#211113-system-effects)
      - [21.11.14 Complete Effect Hierarchy](#211114-complete-effect-hierarchy)
      - [21.11.15 Standard Effect Definitions](#211115-standard-effect-definitions)
  - [22. Effect Rules and Checking](#22-effect-rules-and-checking)
    - [22.7 Async Effect Masks](#227-async-effect-masks)
    - [22.3.1 Named Effects in Declarations](#2231-named-effects-in-declarations)
    - [22.4 Forbidden Effects](#224-forbidden-effects)
      - [22.4.1 Valid Use Case 1: Wildcard Restriction](#2241-valid-use-case-1-wildcard-restriction)
      - [22.4.2 Valid Use Case 2: Polymorphic Effect Constraint](#2242-valid-use-case-2-polymorphic-effect-constraint)
      - [22.4.3 INVALID: Redundant Forbidden Effects](#2243-invalid-redundant-forbidden-effects)
    - [22.5 Effect Wildcards](#225-effect-wildcards)
    - [22.6 Higher-Order Functions](#226-higher-order-functions)
    - [Async Effect Masking (await)](#async-effect-masking-await)
  - [23. Effect Budgets](#23-effect-budgets)
    - [23.1 Budget Overview](#231-budget-overview)
    - [23.2 Static Budgets](#232-static-budgets)
    - [23.3 Dynamic Budgets](#233-dynamic-budgets)
    - [23.4 Time Budgets](#234-time-budgets)
    - [23.5 Count Budgets](#235-count-budgets)
    - [23.6 Budget Composition](#236-budget-composition)
- [Part VII: Memory Management](#part-vii-memory-management)
  - [25. Lexical Permission System](#25-lexical-permission-system)
    - [25.1 LPS Overview](#251-lps-overview)
    - [25.2 Design Goals](#252-design-goals)
    - [25.3 Memory Safety Model](#253-memory-safety-model)
    - [25.4 Compilation Model](#254-compilation-model)
  - [26. Permission Types and Rules](#26-permission-types-and-rules)
    - [26.1 Permission Overview](#261-permission-overview)
    - [26.2 Immutable Reference (Default)](#262-immutable-reference-default)
    - [26.3 Owned Permission](#263-owned-permission)
    - [26.5 Isolated Permission](#265-isolated-permission)
    - [27.2 Passing by Reference](#272-passing-by-reference)
    - [27.3 Permission Transitions](#273-permission-transitions)
    - [27.8 The Cantrip Safety Model: Trade-offs and Guarantees](#278-the-cantrip-safety-model-trade-offs-and-guarantees)
      - [27.8.1 What Cantrip Guarantees (✅ Compile-Time Safe)](#2781-what-cantrip-guarantees--compile-time-safe)
      - [27.8.2 What Cantrip Does NOT Guarantee (⚠️ Programmer Responsibility)](#2782-what-cantrip-does-not-guarantee-️-programmer-responsibility)
  - [28. Memory Regions](#28-memory-regions)
    - [28.1 Region Overview](#281-region-overview)
    - [28.2 Three Allocation Strategies Compared](#282-three-allocation-strategies-compared)
    - [28.3 Region Declaration](#283-region-declaration)
    - [28.5 Region Allocation Syntax](#285-region-allocation-syntax)
    - [28.6 Implementing Region Allocation](#286-implementing-region-allocation)
    - [28.7 Region vs Heap Decision Guide](#287-region-vs-heap-decision-guide)
    - [28.8 Performance Characteristics](#288-performance-characteristics)
    - [28.9 Common Patterns](#289-common-patterns)
    - [28.10 Safety Restrictions](#2810-safety-restrictions)
  - [29. Region Formal Semantics](#29-region-formal-semantics)
    - [29.1 Region Algebra](#291-region-algebra)
    - [29.2 Allocation Rules](#292-allocation-rules)
    - [29.3 Deallocation Rules](#293-deallocation-rules)
    - [29.4 Escape Analysis](#294-escape-analysis)
    - [29.5 Region Typing](#295-region-typing)
    - [29.6 Memory Model](#296-memory-model)
    - [29.7 Happens-Before Relation](#297-happens-before-relation)
- [Part VIII: Module System](#part-viii-module-system)
  - [30. Modules and Code Organization](#30-modules-and-code-organization)
    - [30.1 File-Based Module System](#301-file-based-module-system)
    - [30.2 Module Definition](#302-module-definition)
    - [30.3 Module Metadata](#303-module-metadata)
    - [30.4 Module Resolution Algorithm](#304-module-resolution-algorithm)
    - [30.6 Modules vs Regions](#306-modules-vs-regions)
  - [31. Import and Export Rules](#31-import-and-export-rules)
    - [31.1 Import Syntax](#311-import-syntax)
    - [31.2 Import Resolution](#312-import-resolution)
    - [31.3 Re-exports](#313-re-exports)
    - [31.6 Import Ordering](#316-import-ordering)
  - [32. Visibility and Access Control](#32-visibility-and-access-control)
    - [32.1 Visibility Modifiers](#321-visibility-modifiers)
    - [32.2 Public Items](#322-public-items)
    - [32.3 Internal Items](#323-internal-items)
    - [32.4 Private Items](#324-private-items)
    - [32.5 Record Field Visibility](#325-record-field-visibility)
    - [32.6 Procedure Visibility](#326-procedure-visibility)
    - [32.7 Trait Implementation Visibility](#327-trait-implementation-visibility)
  - [33. Module Resolution](#33-module-resolution)
    - [33.1 Resolution Rules](#331-resolution-rules)
    - [33.2 Search Path](#332-search-path)
    - [33.4 Package Configuration](#334-package-configuration)
    - [33.5 Module Cache](#335-module-cache)
- [Part IX: Advanced Features](#part-ix-advanced-features)
  - [34. Compile-Time Programming](#34-compile-time-programming)
    - [34.6 Opt‑In Reflection](#346-optin-reflection)
    - [34.1 Comptime Keyword](#341-comptime-keyword)
    - [34.3 Type Introspection](#343-type-introspection)
    - [34.5 Comptime Type Introspection](#345-comptime-type-introspection)
    - [34.6 Comptime Code Generation](#346-comptime-code-generation)
    - [34.7 Comptime Validation](#347-comptime-validation)
  - [35. Concurrency](#35-concurrency)
    - [35.5 Structured Concurrency](#355-structured-concurrency)
    - [35.2 Message Passing](#352-message-passing)
    - [35.3 Send and Sync Traits](#353-send-and-sync-traits)
    - [35.4 Atomic Operations](#354-atomic-operations)
  - [36. Actors (First-Class Type)](#36-actors-first-class-type)
    - [36.1 Actor Pattern Overview](#361-actor-pattern-overview)
    - [36.2 Basic Actor Pattern Implementation](#362-basic-actor-pattern-implementation)
    - [36.4 Standard Library Support](#364-standard-library-support)
  - [37. Async/Await](#37-asyncawait)
    - [37.4 Async Iteration](#374-async-iteration)
    - [37.1 Async Functions](#371-async-functions)
    - [37.2 Await Expressions](#372-await-expressions)
    - [37.3 Select Expression](#373-select-expression)
  - [55. Machine‑Readable Output](#55-machinereadable-output)
- [Part XIV: Foreign Function Interface (FFI)](#part-xiv-foreign-function-interface-ffi)
  - [56. Foreign Function Interface Overview](#56-foreign-function-interface-overview)
  - [57. Declarations and Linkage](#57-declarations-and-linkage)
  - [58. Type Mappings](#58-type-mappings)
  - [59. Ownership and Allocation Across FFI](#59-ownership-and-allocation-across-ffi)
  - [60. Callbacks from C into Cantrip](#60-callbacks-from-c-into-cantrip)
  - [61. Errors and Panics](#61-errors-and-panics)
  - [62. Inline Assembly (Reserved)](#62-inline-assembly-reserved)
- [Part X: Operational Semantics](#part-x-operational-semantics)
  - [38. Small-Step Semantics](#38-small-step-semantics)
    - [38.1 Small-Step Reduction](#381-small-step-reduction)
    - [38.2 Evaluation Contexts](#382-evaluation-contexts)
  - [39. Big-Step Semantics](#39-big-step-semantics)
    - [39.1 Big-Step Evaluation](#391-big-step-evaluation)
  - [40. Memory Model](#40-memory-model)
    - [40.1 Memory State](#401-memory-state)
    - [40.2 Happens-Before](#402-happens-before)
  - [41. Evaluation Order](#41-evaluation-order)
    - [41.1 Left-to-Right Evaluation](#411-left-to-right-evaluation)
    - [41.2 Short-Circuit Evaluation](#412-short-circuit-evaluation)
- [Part XI: Soundness and Properties](#part-xi-soundness-and-properties)
  - [42. Type Soundness](#42-type-soundness)
  - [43. Effect Soundness](#43-effect-soundness)
  - [44. Memory Safety](#44-memory-safety)
  - [45. Modal Safety](#45-modal-safety)
- [Part XII: Standard Library](#part-xii-standard-library)
  - [46. Core Types and Operations](#46-core-types-and-operations)
    - [46.1 Core Type System](#461-core-type-system)
    - [46.2 Option Type](#462-option-type)
    - [46.3 Result Type](#463-result-type)
    - [46.4 String Type](#464-string-type)
  - [47. Collections](#47-collections)
    - [47.1 Vec](#471-vec)
    - [47.2 HashMap\<K, V\>](#472-hashmapk-v)
    - [47.3 HashSet](#473-hashset)
  - [48. I/O and File System](#48-io-and-file-system)
    - [48.1 File I/O](#481-file-io)
    - [48.2 Standard Streams](#482-standard-streams)
    - [48.3 File System Operations](#483-file-system-operations)
  - [49. Networking](#49-networking)
    - [49.1 HTTP Client](#491-http-client)
    - [49.2 TCP Sockets](#492-tcp-sockets)
  - [50. Concurrency Primitives](#50-concurrency-primitives)
      - [50.1 Structured Concurrency Helpers](#501-structured-concurrency-helpers)
    - [50.1 Mutex](#501-mutex)
    - [50.2 Channels](#502-channels)
    - [50.3 Atomic Types](#503-atomic-types)
- [Part XIII: Tooling and Implementation](#part-xiii-tooling-and-implementation)
  - [51. Compiler Architecture](#51-compiler-architecture)
    - [51.1 Compilation Pipeline](#511-compilation-pipeline)
    - [51.2 Compiler Invocation](#512-compiler-invocation)
    - [51.3 Optimization Levels](#513-optimization-levels)
    - [51.4 Verification Modes](#514-verification-modes)
    - [51.5 Incremental Compilation](#515-incremental-compilation)
  - [52. Error Reporting](#52-error-reporting)
    - [52.1 Error Message Format](#521-error-message-format)
    - [52.2 Example Error Messages](#522-example-error-messages)
    - [52.3 Machine-Readable Output](#523-machine-readable-output)
  - [53. Package Management](#53-package-management)
    - [53.1 Project Structure](#531-project-structure)
    - [53.2 Package Manifest](#532-package-manifest)
    - [53.3 Commands](#533-commands)
    - [53.4 Dependency Resolution](#534-dependency-resolution)
  - [54. Testing Framework](#54-testing-framework)
    - [54.1 Unit Tests](#541-unit-tests)
    - [54.2 Integration Tests](#542-integration-tests)
    - [54.3 Property-Based Testing](#543-property-based-testing)
    - [54.4 Benchmarks](#544-benchmarks)
  - [56. Overview](#56-overview)
  - [57. Declarations and Linkage](#57-declarations-and-linkage-1)
  - [58. Type Mappings](#58-type-mappings-1)
  - [59. Ownership and Allocation Across FFI](#59-ownership-and-allocation-across-ffi-1)
  - [60. Callbacks from C into Cantrip](#60-callbacks-from-c-into-cantrip-1)
  - [61. Errors and Panics](#61-errors-and-panics-1)
  - [62. Inline Assembly (Reserved)](#62-inline-assembly-reserved-1)
- [Part XII: Standard Library](#part-xii-standard-library-1)
  - [46. Core Types and Operations {#46-core-types-and-operations}](#46-core-types-and-operations-46-core-types-and-operations)
    - [46.1 Option](#461-option)
    - [46.2 Result](#462-result)
    - [46.3 String and str](#463-string-and-str)
    - [46.4 Vec](#464-vec)
    - [46.5 HashMap / HashSet](#465-hashmap--hashset)
    - [46.6 Core Functions (Normative)](#466-core-functions-normative)
  - [47. Collections {#47-collections}](#47-collections-47-collections)
  - [48. I/O and File System {#48-io-and-file-system}](#48-io-and-file-system-48-io-and-file-system)
  - [49. Networking {#49-networking}](#49-networking-49-networking)
  - [50. Concurrency Primitives {#50-concurrency-primitives}](#50-concurrency-primitives-50-concurrency-primitives)
- [Part XIII: Tooling and Implementation](#part-xiii-tooling-and-implementation-1)
  - [51. Compiler Architecture {#51-compiler-architecture}](#51-compiler-architecture-51-compiler-architecture)
  - [52. Error Reporting {#52-error-reporting}](#52-error-reporting-52-error-reporting)
  - [53. Package Management {#53-package-management}](#53-package-management-53-package-management)
  - [54. Testing Framework {#54-testing-framework}](#54-testing-framework-54-testing-framework)
  - [55. Machine‑Readable Output {#55-machine-readable-output}](#55-machinereadable-output-55-machine-readable-output)
- [Part XIV: Foreign Function Interface](#part-xiv-foreign-function-interface)
  - [56. Overview {#56-overview}](#56-overview-56-overview)
  - [57. Declarations and Linkage {#57-declarations-and-linkage}](#57-declarations-and-linkage-57-declarations-and-linkage)
  - [58. Type Mappings {#58-type-mappings}](#58-type-mappings-58-type-mappings)
  - [59. Ownership and Allocation Across FFI {#59-ownership-and-allocation-across-ffi}](#59-ownership-and-allocation-across-ffi-59-ownership-and-allocation-across-ffi)
  - [60. Callbacks from C into Cantrip {#60-callbacks-from-c-into-cantrip}](#60-callbacks-from-c-into-cantrip-60-callbacks-from-c-into-cantrip)
  - [61. Errors and Panics {#61-errors-and-panics}](#61-errors-and-panics-61-errors-and-panics)
  - [62. Inline Assembly (Reserved) {#62-inline-assembly-reserved}](#62-inline-assembly-reserved-62-inline-assembly-reserved)
  
---

---

**Definition 1.1 (Mathematical Foundations):** This section establishes the mathematical notation, metavariables, and judgment forms used throughout the Cantrip Language Specification to formally describe syntax, typing rules, and operational semantics.

## 1. Notation and Mathematical Foundations

### 1.1 Overview

**Key innovation/purpose:** Provides a formal mathematical foundation for precisely specifying language semantics through standardized notation, judgment forms, and inference rules.

**When to use this section:**
- When reading type rules (T-* rules in §4-§10)
- When reading evaluation semantics (E-* rules in Part X)
- When encountering unfamiliar mathematical notation
- When implementing a type checker or interpreter

**When NOT to use this section:**
- For learning Cantrip syntax (see §2 Lexical Structure)
- For practical programming guidance (see Examples sections)
- For implementation strategies (see Part XIII: Tooling)

**Relationship to other features:**
- **All subsequent sections** use the notation defined here
- **§2 (Lexical Structure)**: Uses set notation (∈, ⊆) for token categories
- **§3 (Abstract Syntax)**: Uses BNF notation for syntax definitions
- **Type System (Part II)**: Uses type judgments (Γ ⊢ e : T) extensively
- **Operational Semantics (Part X)**: Uses evaluation judgments (⟨e, σ⟩ ⇓ ⟨v, σ'⟩)
- **Contract System (Part V)**: Uses Hoare triples ({P} e {Q}) and semantic satisfaction (σ ⊨ P)

**Normative status:** This section is INFORMATIVE. It defines notation but does not impose requirements on implementations. The notation is used to precisely specify requirements in later NORMATIVE sections.

### 1.2 Syntax

#### 1.2.1 Concrete Syntax

**Not applicable:** This section defines mathematical notation and metavariables, not Cantrip language syntax. Concrete syntax for Cantrip appears in §2 (Lexical Structure) and subsequent feature sections.

#### 1.2.2 Abstract Syntax

**Metavariable Categories:**

The following metavariable conventions are used throughout the specification. Metavariables are placeholders that stand for actual program constructs.

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
Δ ∈ Transitions        (state transition relation)
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
Δ ∈ EffContext         (effect contexts: available effects)
σ, σ' ∈ Store          (memory stores: Location → Value)
```

**Context Operations:**
- `Γ, x: T` means "context Γ extended with binding x: T"
- `σ[ℓ ↦ v]` means "store σ updated so location ℓ maps to value v"

**Judgment Form Notation:**

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

**Formal Operators:**

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

**Inference Rule Format:**
```
[Rule-Name]
premise₁    premise₂    ...    premiseₙ
────────────────────────────────────────
conclusion
```

#### 1.2.3 Basic Examples

**Example 1: Variable in typing environment**
```
Context: Γ = {x: i32, y: bool, z: str}
Judgment: Γ ⊢ x : i32

Reading: "In a context where x has type i32, y has type bool, and z has type str,
         the variable x has type i32."
```

**Example 2: Set membership**
```
T ∈ {i8, i16, i32, i64}

Reading: "Type T is one of the signed integer types i8, i16, i32, or i64."
```

**Example 3: Substitution**
```
Expression: (x + y)[x ↦ 5]
Result: 5 + y

Reading: "Substitute 5 for x in the expression x + y, yielding 5 + y."
```

**Example 4: Inference rule application**
```
[T-Add]
Γ ⊢ x : i32
Γ ⊢ 1 : i32
─────────────────────
Γ ⊢ x + 1 : i32

Reading: "If x has type i32 and 1 has type i32 in context Γ,
         then x + 1 has type i32."
```

### 1.3 Static Semantics

#### 1.3.1 Well-Formedness Rules

**Purpose:** These rules define when mathematical notation is well-formed, not when Cantrip programs are well-typed (that comes in later sections).

**Well-formed contexts:**
```
[WF-Context-Empty]
────────────────────
∅ well-formed

[WF-Context-Extend]
Γ well-formed
x ∉ dom(Γ)
Γ ⊢ T : Type
────────────────────
Γ, x: T well-formed
```

**Explanation:** A context is well-formed if it's either empty or built by extending a well-formed context with a fresh variable and a well-formed type.

**Well-formed judgments:**
```
[WF-Typing-Judgment]
Γ well-formed
e ∈ Expr
T ∈ Type
────────────────────
Γ ⊢ e : T is a judgment

[WF-Effect-Judgment]
Γ well-formed
e ∈ Expr
ε ∈ Effect
────────────────────
Γ ⊢ e ! ε is a judgment
```

**Explanation:** A typing judgment is well-formed if the context is well-formed, e is an expression, and T is a type. The judgment may or may not be derivable (provable), but it's syntactically valid.

#### 1.3.2 Notational Properties

**Substitution properties:**
```
Property 1.1 (Substitution Well-Defined):
∀e ∈ Expr, x ∈ Var, v ∈ Value. ∃!e' ∈ Expr. e' = e[x ↦ v]

Reading: "For any expression e, variable x, and value v, there exists a unique
         expression e' that is the result of substituting v for x in e."
```

**Context properties:**
```
Property 1.2 (Context Lookup):
∀Γ, x. (x ∈ dom(Γ)) ⇒ ∃!T. Γ(x) = T

Reading: "If variable x is in the domain of context Γ, then there exists a unique
         type T such that Γ maps x to T."
```

**Set properties:**
```
Property 1.3 (Effect Union Commutativity):
∀ε₁, ε₂ ∈ Effect. ε₁ ∪ ε₂ = ε₂ ∪ ε₁

Property 1.4 (Effect Union Associativity):
∀ε₁, ε₂, ε₃ ∈ Effect. (ε₁ ∪ ε₂) ∪ ε₃ = ε₁ ∪ (ε₂ ∪ ε₃)
```

#### 1.3.3 Naming Conventions

**In Cantrip source code:**
- Type names: `CamelCase` (e.g., `Point`, `Vec`, `HashMap`)
- Functions: `snake_case` (e.g., `read_file`, `compute_sum`, `is_valid`)
- Constants: `SCREAMING_SNAKE` (e.g., `MAX_SIZE`, `DEFAULT_PORT`, `PI`)
- Type variables: Single uppercase letters (e.g., `T`, `U`, `V`, `K`)
- Lifetimes: Single lowercase letter with leading apostrophe (e.g., `'a`, `'b`)

**In formal rules:**
- Metavariables follow conventions in §1.2.2
- Rule names: `[Category-Description]` (e.g., `[T-Add]`, `[E-IfTrue]`, `[WF-Array]`)

**Type syntax shortcuts:**
```
[T]         ≡  slice of T (fat pointer)
[T; n]      ≡  array of T with length n
T?          ≡  Option<T>
T!          ≡  Result<T, Error>
```

### 1.4 Dynamic Semantics

**Not applicable:** Mathematical notation has no runtime semantics. This section defines notation used to specify the dynamic semantics of Cantrip programs in later sections.

**Evaluation judgment forms** (defined here, applied in Part X: Operational Semantics):

**Small-step reduction:**
```
⟨e, σ⟩ → ⟨e', σ'⟩

Reading: "Expression e in store σ reduces in one step to expression e' in store σ'."
```

**Big-step evaluation:**
```
⟨e, σ⟩ ⇓ ⟨v, σ'⟩

Reading: "Expression e in store σ evaluates to value v, producing store σ'."
```

**Example (from §38: Operational Semantics):**
```
Evaluation: ⟨2 + 3, σ⟩ ⇓ ⟨5, σ⟩

Reading: "The expression 2 + 3 evaluates to 5; the store is unchanged."
```

### 1.5 Reading Type Rules: Complete Examples

This subsection demonstrates how the notation defined above appears in actual specification sections. These examples bridge the abstract notation and concrete Cantrip code.

#### 1.5.1 Example 1: Integer Addition (§4 Primitives)

**Cantrip code:**
```cantrip
let x: i32 = 10;
let y: i32 = 20;
let z = x + y;
```

**Type rule:**
```
[T-Add]
Γ ⊢ e₁ : T
Γ ⊢ e₂ : T
T ∈ {i8, i16, i32, i64, u8, u16, u32, u64, f32, f64}
─────────────────────────────────────────────────────
Γ ⊢ e₁ + e₂ : T
```

**Reading this rule:**
- **Γ**: Typing environment (§1.2.2), here `{x: i32, y: i32}`
- **⊢**: Typing judgment (§1.2.2), reads "entails" or "proves"
- **e₁, e₂**: Expression metavariables (§1.2.2), here `x` and `y`
- **T**: Type metavariable (§1.2.2), here `i32`
- **∈**: Set membership (§1.2.2)

**Application to code:**
```
Premises:
  {x: i32, y: i32} ⊢ x : i32     (variable lookup)
  {x: i32, y: i32} ⊢ y : i32     (variable lookup)
  i32 ∈ {i8, i16, ..., f64}      (i32 is a numeric type)

Conclusion:
  {x: i32, y: i32} ⊢ x + y : i32
```

The type checker uses this rule to determine that `x + y` has type `i32`.

#### 1.5.2 Example 2: Function Call (§13 Functions)

**Cantrip code:**
```cantrip
function add(a: i32, b: i32): i32 { a + b }

let result = add(10, 20);
```

**Type rule:**
```
[T-Call]
Γ ⊢ f : fn(T₁, ..., Tₙ) -> U
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
─────────────────────────────────────
Γ ⊢ f(e₁, ..., eₙ) : U
```

**Reading this rule:**
- **f**: Function name metavariable (§1.2.2), here `add`
- **fn(T₁, ..., Tₙ) -> U**: Function type syntax
- **U**: Return type metavariable (§1.2.2), here `i32`

**Application to code:**
```
Premises:
  Γ ⊢ add : fn(i32, i32) -> i32     (function signature)
  Γ ⊢ 10 : i32                       (integer literal)
  Γ ⊢ 20 : i32                       (integer literal)

Conclusion:
  Γ ⊢ add(10, 20) : i32
```

#### 1.5.3 Example 3: Evaluation Semantics (Part X)

**Cantrip code:**
```cantrip
let x = 2 + 3;
```

**Evaluation rule:**
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨e₁ + e₂, σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

**Reading this rule:**
- **⟨e, σ⟩**: Configuration with expression `e` and store `σ` (§1.2.2)
- **⇓**: Big-step evaluation (§1.2.2), reads "evaluates to"
- **n₁, n₂**: Integer value metavariables
- **σ, σ', σ''**: Store metavariables (§1.2.2) tracking state changes

**Application to code:**
```
Evaluation steps:
  ⟨2, σ⟩ ⇓ ⟨2, σ⟩                   (literal evaluates to itself)
  ⟨3, σ⟩ ⇓ ⟨3, σ⟩                   (literal evaluates to itself)
  ───────────────────────────────
  ⟨2 + 3, σ⟩ ⇓ ⟨5, σ⟩               (addition)
```

The evaluator uses this rule to compute that `2 + 3` evaluates to `5`.

#### 1.5.4 Example 4: Modal State Transitions (§10 Modals)

**Cantrip code:**
```cantrip
modal File {
    @Closed -> open() -> @Open;
    @Open -> read() -> @Open;
    @Open -> close() -> @Closed;
}

let file: File@Closed = File.new();
let opened: File@Open = file.open();
```

**State transition rule:**
```
[T-Transition]
Γ ⊢ e : modal P@S₁
S₁ →ₘ S₂ ∈ transitions(P)
Γ ⊢ m : proc(mut self: modal P@S₁) -> modal P@S₂
────────────────────────────────────────────────────
Γ ⊢ e.m() : modal P@S₂
```

**Reading this rule:**
- **modal P@S**: Modal type in state (§1.2.2)
- **→ₘ**: State transition via procedure m (§1.2.2)
- **transitions(P)**: Set of valid transitions for modal P

**Application to code:**
```
Premises:
  Γ ⊢ file : File@Closed                          (file in Closed state)
  Closed →open Open ∈ transitions(File)           (valid transition)
  Γ ⊢ open : proc(mut self: File@Closed) -> File@Open

Conclusion:
  Γ ⊢ file.open() : File@Open
```

#### 1.5.5 Example 5: Contract Verification (§17 Contracts)

**Cantrip code:**
```cantrip
function divide(x: i32, y: i32): i32
    must y != 0
    will result == x / y
{
    x / y
}
```

**Contract verification rule:**
```
[Verify-Ensures]
{P ∧ requires(f)} body(f) {ensures(f)[result ↦ returned_value]}
────────────────────────────────────────────────────────────────
{P} f(...) {ensures(f)}
```

**Reading this rule:**
- **{P} e {Q}**: Hoare triple (§1.2.2)
- **requires(f)**: Precondition of function f (§1.2.2)
- **ensures(f)**: Postcondition of function f (§1.2.2)
- **[result ↦ v]**: Substitution (§1.2.2), replace `result` with `v`

**Application to code:**
```
Premises:
  Precondition: y != 0
  Body: x / y
  Postcondition: result == x / y

Verification:
  {y != 0} x / y {result == x / y}

If the caller will y != 0, then the function will result == x / y.
```

### 1.6 Cross-References to Notation Usage

The notation defined in this section is used throughout the specification:

- **§2 (Lexical Structure)**: Uses set notation (∈, ⊆) for token categories
- **§3 (Abstract Syntax)**: Defines syntax using BNF with `::=` and `|`
- **§4-§10 (Type System)**: Uses type judgments `Γ ⊢ e : T` extensively
- **§10 (Modals)**: Uses state transition judgments `@S₁ →ₘ @S₂`
- **§13-§15 (Functions)**: Uses effect judgments `Γ ⊢ e ! ε`
- **§17 (Contracts)**: Uses Hoare triples `{P} e {Q}` and semantic satisfaction `σ ⊨ P`
- **Part X (Operational Semantics)**: Uses evaluation judgments `⟨e, σ⟩ ⇓ ⟨v, σ'⟩`

---

**Definition 2.1 (Lexical Structure):** The lexical structure defines the syntax of tokens—the smallest elements of Cantrip source text—including identifiers, literals, keywords, operators, and delimiters.

## 2. Lexical Structure

### 2.1 Overview

**Key innovation/purpose:** Defines the atomic elements (tokens) of Cantrip source code, forming the first phase of compilation that transforms character streams into structured token sequences.

**When to use this section:**
- When implementing a lexer/tokenizer
- When defining concrete syntax for new language features
- When understanding character-level source code rules
- When resolving ambiguities in token formation

**When NOT to use this section:**
- For understanding program structure (see §3 Abstract Syntax)
- For type system rules (see Part II: Type System)
- For runtime semantics (see Part X: Operational Semantics)
- For practical programming examples (see feature-specific sections)

**Relationship to other features:**
- **§3 (Abstract Syntax)**: Tokens are parsed into abstract syntax trees
- **§4+ (Type System)**: Type inference assigns types to literal tokens
- **Compilation Pipeline**: Lexical analysis is the first phase
- **Error Reporting (§52)**: Lexical errors are the first class of errors detected

**Phase in compilation pipeline:**
```
Source Text → [Lexer] → Token Stream → [Parser] → AST → [Type Checker] → ...
```

**Token categories:**
- **Identifiers**: Names for variables, functions, types (§2.2.3)
- **Literals**: Integer, float, boolean, character, string constants (§2.2.4)
- **Keywords**: Reserved words with special meaning (§2.2.5)
- **Operators**: Symbolic operators (+, -, *, /, etc.)
- **Delimiters**: Punctuation ({}, [], (), etc.)
- **Comments**: Discarded during tokenization (§2.2.2)

**Scope:** This section defines what tokens LOOK LIKE (their lexical form). Type inference and type checking for literals are defined in §4 (Primitives) static semantics.

### 2.2 Syntax

#### 2.2.1 Concrete Syntax

**Source Files:**

**Definition 2.2 (Source File):** A Cantrip source file is a sequence of Unicode characters encoded in UTF-8.

**Grammar:**
```ebnf
SourceFile ::= Utf8Bom? Item*
Utf8Bom    ::= #xEF #xBB #xBF
```

**Formal properties:**
- Encoding: UTF-8 (REQUIRED)
- Line endings: LF (`\n`), CRLF (`\r\n`), or CR (`\r`) (all accepted, normalized to LF during lexing)
- Newlines are significant tokens: Newlines are preserved as tokens during lexical analysis (not discarded as whitespace)
- BOM: Optional UTF-8 BOM (U+FEFF) is ignored if present
- File extension: `.cantrip` (RECOMMENDED)
- Maximum file size: Implementation-defined (RECOMMENDED: 1MB)
- Normalization: Unicode normalization form NFC is RECOMMENDED but not required

**Comments:**

**Grammar:**
```ebnf
Comment      ::= LineComment | BlockComment | DocComment | ModuleDoc
LineComment  ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" | BlockComment)* "*/"
DocComment   ::= "///" ~[\n\r]*
ModuleDoc    ::= "//!" ~[\n\r]*
```

**Semantic rules:**
- Line comments extend from `//` to end of line
- Block comments nest: `/* outer /* inner */ outer */` is valid
- Doc comments (`///`) document the item that follows
- Module docs (`//!`) document the containing module
- Doc comments are preserved for documentation generation
- Non-doc comments are stripped before parsing
- Comments do not affect statement continuation (newlines after comments are significant)

**Identifiers:**

**Definition 2.3 (Identifier):** An identifier is a sequence of characters used to name variables, functions, types, modules, and other program entities.

**Grammar:**
```ebnf
Identifier    ::= IdentStart IdentContinue*
IdentStart    ::= [a-zA-Z_]
IdentContinue ::= [a-zA-Z0-9_]
```

**Restrictions:**
- Cannot be a reserved keyword (§2.2.5)
- Cannot start with a digit
- Case-sensitive: `Variable` ≠ `variable`
- Maximum length: Implementation-defined (RECOMMENDED: 255 characters)

**Integer Literals:**

**Grammar:**
```ebnf
IntegerLiteral ::= DecimalLiteral IntegerSuffix?
                 | HexLiteral IntegerSuffix?
                 | BinaryLiteral IntegerSuffix?
                 | OctalLiteral IntegerSuffix?

DecimalLiteral ::= [0-9] [0-9_]*
HexLiteral     ::= "0x" [0-9a-fA-F] [0-9a-fA-F_]*
BinaryLiteral  ::= "0b" [01] [01_]*
OctalLiteral   ::= "0o" [0-7] [0-7_]*

IntegerSuffix  ::= "i8" | "i16" | "i32" | "i64" | "isize"
                 | "u8" | "u16" | "u32" | "u64" | "usize"
```

**Underscore separators:**
- Underscores (`_`) may appear between digits for readability
- Underscores are ignored during value computation
- Cannot appear at the start or end of the literal
- Cannot appear adjacent to the base prefix (`0x`, `0b`, `0o`)

**Floating-Point Literals:**

**Grammar:**
```ebnf
FloatLiteral ::= DecimalLiteral "." DecimalLiteral Exponent? FloatSuffix?
               | DecimalLiteral Exponent FloatSuffix?

Exponent     ::= [eE] [+-]? DecimalLiteral
FloatSuffix  ::= "f32" | "f64"
```

**Boolean Literals:**

**Grammar:**
```ebnf
BooleanLiteral ::= "true" | "false"
```

**Character Literals:**

**Grammar:**
```ebnf
CharLiteral    ::= "'" (EscapeSequence | ~['\\]) "'"
EscapeSequence ::= "\\" [nrt\\'"0]
                 | "\\x" HexDigit HexDigit
                 | "\\u{" HexDigit+ "}"
```

**Escape sequences:**
- `\n` — newline (U+000A)
- `\r` — carriage return (U+000D)
- `\t` — tab (U+0009)
- `\\` — backslash (U+005C)
- `\'` — single quote (U+0027)
- `\"` — double quote (U+0022)
- `\0` — null character (U+0000)
- `\xNN` — ASCII character (00-7F)
- `\u{N...}` — Unicode scalar value (up to 6 hex digits)

**String Literals:**

**Grammar:**
```ebnf
StringLiteral ::= '"' (EscapeSequence | ~["\\])* '"'
```

**String properties:**
- Strings are sequences of Unicode scalar values (not code points or grapheme clusters)
- Escape sequences are processed during lexing
- Multi-line strings are supported (newlines in source → newlines in string)

**Keywords:**

**Definition 2.4 (Keyword):** A keyword is a reserved identifier with special syntactic meaning that cannot be used as an identifier.

**Reserved keywords (MUST NOT be used as identifiers):**
```
abstract    as          async       await       break
by          case        comptime    const       continue
defer       effect      else        enum        exists
false       forall      function    if          import
internal    invariant   iso         let         loop
match       modal       module      move        must
mut         new         none        own         private
procedure   protected   public      record      ref
region      result      select      self        Self
state       static      trait       true        type
uses        var         where       will        with
```

**Contextual keywords (special meaning in specific contexts):**
```
effects     pure
```

**Statement Terminators:**

**Definition 2.5 (Statement Termination):** Cantrip uses newlines as primary statement terminators. Semicolons are optional and may be used to separate multiple statements on a single line.

**Grammar:**
```ebnf
Separator ::= NEWLINE | ";"
Whitespace ::= " " | "\t" | "\r"  (not newline)
```

**Termination Rules:**
- **Primary rule:** A newline terminates a statement unless a continuation rule applies
- **Optional separator:** Semicolons `;` may be used to separate statements on the same line
- **Whitespace:** Spaces, tabs, and carriage returns are discarded (not significant)
- **Newlines:** Newlines are preserved as tokens (significant for statement boundaries)

**Statement Continuation Rules:**

A statement continues across newlines in exactly four cases:

**Rule 1: Unclosed Delimiters**
```
Statement continues when `(`, `[`, or `<` remains unclosed.
```

**Rule 2: Trailing Operator**
```
Statement continues when line ends with a binary or assignment operator.
Binary operators: +, -, *, /, %, **, ==, !=, <, <=, >, >=, &&, ||, &, |, ^, <<, >>, .., ..=, =>
Assignment operators: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
```

**Rule 3: Leading Dot**
```
Statement continues when next line begins with `.` (method/field access).
```

**Rule 4: Leading Pipeline**
```
Statement continues when next line begins with `=>` (pipeline operator).
```

**Formal Continuation Predicate:**
```
is_continuation(current: Token, state: ParserState) -> bool:
    // Rule 1: Unclosed delimiters
    if state.paren_depth > 0 || state.bracket_depth > 0 || state.angle_depth > 0:
        return true

    // Rule 2: Trailing operator
    if state.previous_token in BINARY_OPERATORS ∪ ASSIGNMENT_OPERATORS:
        return true

    // Rule 3: Leading dot
    if state.next_token == DOT:
        return true

    // Rule 4: Leading pipeline
    if state.next_token == PIPELINE:
        return true

    return false
```

**Examples:**
```cantrip
// Simple statements (newline terminates)
let x = 1
let y = 2

// Multiple statements on one line (semicolon separates)
let x = 1; let y = 2

// Rule 1: Unclosed parentheses
let result = calculate(
    arg1,
    arg2
)

// Rule 2: Trailing operator
let total = base +
    modifier +
    bonus

// Rule 3: Leading dot (method chaining)
result
    .validate()
    .process()

// Rule 4: Leading pipeline
let result = input
    => validate
    => transform
```

#### 2.2.2 Abstract Syntax

**Token Abstract Representation:**

Tokens are represented internally by the compiler as tagged unions:

```
Token ::= Identifier(name: String)
        | IntLiteral(value: Integer, suffix: Option<IntegerType>)
        | FloatLiteral(value: Float, suffix: Option<FloatType>)
        | BoolLiteral(value: Boolean)
        | CharLiteral(value: Char)
        | StringLiteral(value: String)
        | Keyword(word: KeywordKind)
        | Operator(op: OperatorKind)
        | Delimiter(delim: DelimiterKind)
        | Newline                               (significant token)
        | Semicolon                             (optional separator)
        | EOF

KeywordKind ::= Function | Procedure | Record | Enum | Modal | ...
OperatorKind ::= Plus | Minus | Star | Slash | ...
DelimiterKind ::= LParen | RParen | LBrace | RBrace | ...

IntegerType ::= I8 | I16 | I32 | I64 | ISize | U8 | U16 | U32 | U64 | USize
FloatType ::= F32 | F64
```

**Key changes from traditional lexers:**
- **Newline is preserved:** Unlike most C-family languages, newlines are not discarded as whitespace
- **Semicolon is optional:** Semicolons are tokens but not required for statement termination
- **Whitespace handling:** Only spaces, tabs, and carriage returns are discarded; newlines are kept

**Token Stream:**

A token stream is a sequence of tokens:
```
TokenStream ::= Token*
```

**Formal properties:**
```
∀ source_text ∈ SourceText. ∃! token_stream ∈ TokenStream.
    tokenize(source_text) = token_stream

Reading: "For any source text, there exists a unique token stream produced by tokenization."
```

**Token position tracking:**

Each token carries source location metadata:
```
PositionedToken ::= Token × SourcePosition

SourcePosition ::= {
    file: FilePath,
    line: LineNumber,
    column: ColumnNumber,
    offset: ByteOffset
}
```

#### 2.2.3 Basic Examples

**Example 1: Simple function tokenization**

**Source:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y
}
```

**Token Stream:**
```
Keyword(Function)
Identifier("add")
Delimiter(LParen)
Identifier("x")
Delimiter(Colon)
Identifier("i32")
Delimiter(Comma)
Identifier("y")
Delimiter(Colon)
Identifier("i32")
Delimiter(RParen)
Delimiter(Colon)
Identifier("i32")
Delimiter(LBrace)
Identifier("x")
Operator(Plus)
Identifier("y")
Delimiter(RBrace)
EOF
```

**Observations:**
- Comments are discarded
- Whitespace is not in the token stream
- Keywords are distinct token types
- Identifiers include both variable names (`x`, `y`) and type names (`i32`)

**Example 2: Literal tokenization**

**Source:**
```cantrip
42
0xFF
"hello"
'a'
true
```

**Token Stream:**
```
IntLiteral(42, None)
IntLiteral(255, None)
StringLiteral("hello")
CharLiteral('a')
BoolLiteral(true)
EOF
```

**Example 3: Maximal munch**

**Source:**
```cantrip
x++
0x10
while_loop
42u32
```

**Token Stream:**
```
Identifier("x")
Operator(PlusPlus)
IntLiteral(16, None)
Identifier("while_loop")
IntLiteral(42, Some(U32))
EOF
```

**Explanation:** The lexer always forms the longest possible token (maximal munch principle).

### 2.3 Static Semantics

#### 2.3.1 Well-Formedness Rules

**Reserved Word Rule:**
```
[WF-Reserved]
w ∈ Keywords
────────────────────
w cannot be used as identifier
```

**Identifier Validity:**
```
[WF-Identifier]
id = IdentStart IdentContinue*
id ∉ Keywords
────────────────────────────────
id is a valid identifier
```

**Literal Suffix Validity:**
```
[WF-Int-Suffix]
suffix ∈ {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize}
────────────────────────────────────────────────────────────────
IntegerLiteral suffix valid

[WF-Float-Suffix]
suffix ∈ {f32, f64}
────────────────────────────────
FloatLiteral suffix valid
```

**Character Escape Validation:**
```
[WF-Escape-Simple]
c ∈ {n, r, t, \\, ', ", 0}
────────────────────────────
\c is valid escape sequence

[WF-Escape-Hex]
0x00 ≤ NN ≤ 0x7F
────────────────────────────
\xNN is valid ASCII escape

[WF-Escape-Unicode]
0x0000 ≤ NNNNNN ≤ 0x10FFFF
NNNNNN ∉ [0xD800, 0xDFFF]    (surrogate range)
────────────────────────────────────────────────
\u{NNNNNN} is valid Unicode escape
```

**Underscore Separator Validity:**
```
[WF-Underscore]
'_' appears between two digits
─────────────────────────────────
underscore placement valid
```

#### 2.3.2 Tokenization Rules

**Formal tokenization function:**
```
tokenize: SourceText → TokenStream

tokenize(ε) = []
tokenize(ws · rest) = tokenize(rest)  where ws ∈ Whitespace
tokenize(comment · rest) = tokenize(rest)  where comment ∈ Comment
tokenize(keyword · rest) = Keyword(keyword) :: tokenize(rest)
tokenize(literal · rest) = Literal(literal) :: tokenize(rest)
tokenize(identifier · rest) = Identifier(identifier) :: tokenize(rest)
```

**Maximal Munch Principle:**
```
Property 2.1 (Maximal Munch):
∀ text ∈ SourceText, token₁, token₂ ∈ Token.
    text starts with token₁ ∧
    text starts with token₂ ∧
    length(token₁) > length(token₂)
    ⇒ tokenize(text) begins with token₁

Reading: "When multiple tokens match, the lexer always chooses the longest match."
```

**Examples:**
```
"0xFF" → IntLiteral(255) NOT IntLiteral(0), Identifier("xFF")
"x++" → Identifier("x"), Operator(PlusPlus) NOT Identifier("x"), Operator(Plus), Operator(Plus)
"while_loop" → Identifier("while_loop") NOT Keyword("while"), Identifier("_loop")
```

#### 2.3.3 Type Properties

**Formal property (identity):**
```
Property 2.2 (Identifier Equality):
∀ x, y ∈ Identifier. x = y ⟺ string(x) = string(y)

Reading: "Identifiers are compared as exact character sequences.
         No Unicode normalization is performed during comparison."
```

**Value computation semantics:**
```
Property 2.3 (Integer Literal Value):
⟦n⟧ᵢₙₜ = value of n in base-10
⟦0xN⟧ᵢₙₜ = value of N in base-16
⟦0bN⟧ᵢₙₜ = value of N in base-2
⟦0oN⟧ᵢₙₜ = value of N in base-8

Property 2.4 (Float Literal Value):
⟦d₁.d₂⟧_float = d₁ + d₂ × 10^(-|d₂|)
⟦x eᵏ⟧_float = x × 10^k

Property 2.5 (Boolean Literal Value):
⟦true⟧_bool = ⊤
⟦false⟧_bool = ⊥
```

### 2.4 Dynamic Semantics

**No special runtime behavior:** Lexical analysis is a compile-time phase only. Tokens do not exist at runtime.

**Tokenization is pure:**
```
Property 2.6 (Deterministic Tokenization):
∀ source ∈ SourceText. tokenize(source) = tokenize(source)

Reading: "Tokenization is deterministic and produces the same result every time."
```

**Tokenization never modifies source:**
```
Property 2.7 (No Side Effects):
tokenize: SourceText → TokenStream is a pure function

Reading: "Tokenization has no side effects and does not modify the input."
```

### 2.5 Additional Properties

#### 2.5.1 Ambiguity Resolution

**Maximal munch in action:**

```cantrip
// Example 1: Operators
x++         → [Identifier("x"), PlusPlus]
            NOT [Identifier("x"), Plus, Plus]

// Example 2: Number literals
0x10        → [IntLiteral(16)]
            NOT [IntLiteral(0), Identifier("x10")]

// Example 3: Keywords
while_loop  → [Identifier("while_loop")]
            NOT [Keyword("while"), Identifier("_loop")]

// Example 4: Type suffix
42u32       → [IntLiteral(42, Some(u32))]
            NOT [IntLiteral(42), Identifier("u32")]
```

**Operator precedence:** Resolved in the parser (§3), not the lexer. The token `++` is lexically distinct from two `+` tokens.

#### 2.5.2 Error Conditions

**Invalid tokens:**
```cantrip
@symbol     // ERROR E2001: '@' is not a valid token start character
0xFF_       // ERROR E2002: trailing underscore in integer literal
'\xAB'      // ERROR E2003: hex escape exceeds ASCII range (0x7F)
'\u{D800}'  // ERROR E2004: surrogate code point in Unicode escape
let while = 5  // ERROR E2005: 'while' is reserved keyword
```

**Valid lexically but type-incorrect (type errors detected in §4):**
```cantrip
let x: u8 = 256        // Lexically valid, type error (overflow)
let y: i32 = 3.14      // Lexically valid, type error (mismatch)
```

### 2.6 Examples and Patterns

#### 2.6.1 Complete Tokenization Example

**Input:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y  // Simple addition
}
```

**Token Stream:**
```
[
  Keyword("function"),
  Identifier("add"),
  LParen,
  Identifier("x"),
  Colon,
  Identifier("i32"),
  Comma,
  Identifier("y"),
  Colon,
  Identifier("i32"),
  RParen,
  Colon,
  Identifier("i32"),
  LBrace,
  Identifier("x"),
  Plus,
  Identifier("y"),
  RBrace
]
```

**Observations:**
- Comments are discarded
- Whitespace is not in the token stream
- Keywords are distinct token types
- Identifiers include both variable names (`x`, `y`) and type names (`i32`)

#### 2.6.2 Literal Parsing Examples

**Integer literals:**
```cantrip
42          → IntLiteral(42, None)           // No suffix, default i32
42u64       → IntLiteral(42, Some(u64))      // Explicit u64
0xFF        → IntLiteral(255, None)          // Hexadecimal
0b1010      → IntLiteral(10, None)           // Binary
1_000_000   → IntLiteral(1000000, None)      // Underscores ignored
```

**Floating-point literals:**
```cantrip
3.14        → FloatLiteral(3.14, None)       // No suffix, default f64
3.14f32     → FloatLiteral(3.14, Some(f32))  // Explicit f32
1.0e10      → FloatLiteral(1.0e10, None)     // Scientific notation
```

**String and character literals:**
```cantrip
'a'         → CharLiteral('a')               // ASCII character
'\n'        → CharLiteral('\n')              // Escape sequence
'\u{1F600}' → CharLiteral('😀')              // Unicode escape

"hello"     → StringLiteral("hello")
"line 1\nline 2" → StringLiteral("line 1\nline 2")
```

#### 2.6.3 Statement Termination Examples

**Simple statements:**
```cantrip
let x = 10
let y = 20
let z = x + y
```

**Multi-line expressions:**
```cantrip
let result = base_value +
    adjustment_factor *
    scale_factor

let condition = x > 0 &&
    y < 100 &&
    z != 0
```

**Method chaining:**
```cantrip
let result = builder
    .with_capacity(100)
    .with_name("example")
    .build()
```

**Unclosed delimiters:**
```cantrip
let array = [
    1, 2, 3,
    4, 5, 6
]

let result = compute(
    arg1,
    arg2,
    arg3
)
```

---

**Definition 3.1 (Abstract Syntax):** The abstract syntax defines the structure of Cantrip programs as abstract syntax trees (ASTs), independent of concrete textual representation.

## 3. Abstract Syntax

### 3.1 Overview

**Key innovation/purpose:** Provides a mathematical representation of program structure that separates the essence of code (what it means) from its textual surface form (how it's written), enabling precise formal reasoning.

**When to use this section:**
- When understanding how programs are represented internally
- When reading type rules and operational semantics
- When implementing parsers, type checkers, or interpreters
- When formal reasoning about program transformations

**When NOT to use this section:**
- For writing Cantrip code (see concrete syntax in feature sections)
- For understanding token-level details (see §2 Lexical Structure)
- For type checking rules (see Part II: Type System)
- For practical programming examples (see feature-specific sections)

**Relationship to other features:**
- **§2 (Lexical Structure)**: Tokens are parsed into ASTs
- **Type System (Part II)**: Type rules operate on AST nodes
- **Operational Semantics (Part X)**: Evaluation rules operate on ASTs
- **Contract System (Part V)**: Contracts annotate AST nodes
- **All subsequent sections**: Use abstract syntax as foundation for formal rules

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

**Notation:** Abstract syntax uses mathematical notation from §1. For example:
- `e ::= x | n | e₁ + e₂` defines expression forms
- Subscripts distinguish multiple instances (e₁, e₂)
- Vertical bar `|` separates alternatives (sum type)

**Reading guide:**
- §3.2 defines syntax forms (what can appear in ASTs)
- §3.3 defines static semantics (well-formedness rules)
- §3.4 defines dynamic semantics (evaluation on ASTs)
- §3.5 provides additional properties
- §3.6 provides examples connecting concrete to abstract syntax

### 3.2 Syntax

#### 3.2.1 Concrete Syntax

**Not directly applicable:** Abstract syntax is defined mathematically, independent of concrete textual form. For concrete syntax (how to write Cantrip code), see:
- **§2 (Lexical Structure)**: Token-level syntax
- **Feature sections (§4+)**: Feature-specific concrete syntax

**Purpose of this section:** Define the STRUCTURE of ASTs, not the TEXT that produces them.

**Note:** While concrete syntax varies (with/without parentheses, whitespace, etc.), abstract syntax captures the canonical structure. For example:
```
Concrete: (x + 1) * 2
Concrete: x + 1 * 2     (with operator precedence)
Abstract: BinOp(Mul, BinOp(Add, Var("x"), Lit(1)), Lit(2))  [SAME]
```

#### 3.2.2 Abstract Syntax

**Definition 3.2 (Type Language):** The abstract syntax of types.

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
    | &'a T                                  (lifetime-annotated reference)
    | T@S                                    (type T in modal state @S)
    | ∀α. T                                  (polymorphic type)
    | !                                      (never type)
    | record Name                            (record type)
    | modal Name                             (modal type)
    | enum Name                              (enum type)
    | trait Name                             (trait type)
```

**Key observations:**
- Types are stratified: primitives (i32, bool), compounds ([T; n], (T₁, T₂)), named types (record Name)
- Modals extend types with state machines (T@S)
- Type parameters support polymorphism (∀α. T)
- References have ownership qualifiers (own, mut, iso)
- Function types distinguish pure functions from procedures

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

**Key observations:**
- Variables (`x`) and values (`v`) are base cases
- Operators (arithmetic, logical, comparison) via `⊕`
- Control flow (if, match, while, loop)
- Function calls and lambda abstractions
- Record construction and field access
- Blocks and sequencing (SEP = newline or semicolon)
- Statement separators are abstract: both newlines and semicolons map to the same sequencing construct

**Definition 3.4 (Values):** The abstract syntax of values (fully evaluated expressions).

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

**Key observations:**
- Values are a subset of expressions
- Literals, records, tuples, closures are values
- Values are irreducible (cannot evaluate further)
- Memory locations (ℓ) represent heap/stack addresses

**Definition 3.5 (Patterns):** The abstract syntax of patterns for destructuring.

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

**Key observations:**
- Patterns appear in `match`, `let`, and function parameters
- Support wildcard `_`, variables, literals, constructors
- Nested patterns enable deep destructuring
- Guards (`if e`) add boolean conditions to patterns

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
    uses ε                                 (effect declaration, newline-terminated)
    must P                                 (preconditions, newline-terminated)
    will Q                                 (postconditions, newline-terminated)
```

**Key observations:**
- Contracts are logical assertions using first-order logic
- `@old(e)` refers to value of `e` before function execution
- `result` refers to the return value in postconditions
- Effect annotations (`uses ε`) track side effects

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

**Key observations:**
- Effects track side effects and capabilities
- Pure functions have effect `∅`
- Effects form an algebraic structure (union, sequencing)
- Bounded effects (e.g., `alloc.heap(bytes≤n)`) enable resource analysis

#### 3.2.3 Basic Examples

**Example 1: Simple arithmetic expression**

**Concrete:**
```cantrip
(x + 1) * 2
```

**Abstract:**
```
BinOp(Mul, BinOp(Add, Var("x"), Lit(1)), Lit(2))
```

**Tree representation:**
```
      Mul
     /   \
   Add    Lit(2)
  /   \
Var("x") Lit(1)
```

**Observation:** Parentheses do not appear in abstract syntax—they only control parsing.

**Example 2: Function call**

**Concrete:**
```cantrip
add(10, 20)
```

**Abstract:**
```
Call(Var("add"), [Lit(10), Lit(20)])
```

**Example 3: Let binding**

**Concrete:**
```cantrip
let x = 5 in x + 1
```

**Abstract:**
```
Let("x", Lit(5), BinOp(Add, Var("x"), Lit(1)))
```

**Example 4: Pattern match**

**Concrete:**
```cantrip
match result {
    Result.Ok(value) -> process(value),
    Result.Err(e) -> handle_error(e),
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

### 3.3 Static Semantics

#### 3.3.1 Well-Formed Types

**Purpose:** Not all syntactically valid type expressions are well-formed. These rules define type well-formedness (not type-correctness—that's in later sections).

**Primitive types:**
```
[WF-Int]
T ∈ {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize}
────────────────────────────────────────────────────────
Γ ⊢ T : Type

[WF-Float]
T ∈ {f32, f64}
────────────────
Γ ⊢ T : Type

[WF-Bool]
────────────────
Γ ⊢ bool : Type

[WF-Char]
────────────────
Γ ⊢ char : Type
```

**Compound types:**
```
[WF-Array]
Γ ⊢ T : Type    n > 0
────────────────────────
Γ ⊢ [T; n] : Type

[WF-Slice]
Γ ⊢ T : Type
────────────────
Γ ⊢ [T] : Type

[WF-Tuple]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
Γ ⊢ (T₁, ..., Tₙ) : Type
```

**Function types:**
```
[WF-Function]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
────────────────────────────────────────────────────────
Γ ⊢ fn(T₁, ..., Tₙ): U : Type

[WF-Procedure]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
────────────────────────────────────────────────────────
Γ ⊢ proc(SelfPerm, T₁, ..., Tₙ): U : Type
```

**Modal types:**
```
[WF-Modal]
modal P { ... } well-formed (see §10)
───────────────────────────────────────
Γ ⊢ modal P : Type

[WF-Modal-At-State]
Γ ⊢ T : Type    S ∈ states(T)
────────────────────────────────
Γ ⊢ T@S : Type
```

**Polymorphic types:**
```
[WF-Forall]
Γ, α : Type ⊢ T : Type
───────────────────────
Γ ⊢ ∀α. T : Type
```

#### 3.3.2 Well-Formed AST Nodes

**Function declaration:**
```
[WF-Function-Decl]
function f(x₁: T₁, ..., xₙ: Tₙ): U uses ε { e }
parameters x₁...xₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
effect signature ε well-formed
body e is an expression
─────────────────────────────────────────────────────────
function declaration well-formed
```

**Record declaration:**
```
[WF-Record-Decl]
record R { f₁: T₁, ..., fₙ: Tₙ }
field names f₁...fₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
record declaration well-formed
```

**Enum declaration:**
```
[WF-Enum-Decl]
enum E { V₁(T₁), ..., Vₙ(Tₙ) }
variant names V₁...Vₙ are distinct
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
────────────────────────────────────────
enum declaration well-formed
```

**Modal declaration:**
```
[WF-Modal-Decl]
modal P { states: {s₁, ..., sₙ}, transitions: Δ }
state names s₁...sₙ are distinct
state graph (states, Δ) is connected
────────────────────────────────────────
modal declaration well-formed
```

#### 3.3.3 Pattern Exhaustiveness

**Purpose:** Match expressions must handle all possible values (exhaustiveness).

**Definition:** A set of patterns P = {p₁, ..., pₙ} is exhaustive for type T if every value v : T matches at least one pattern pᵢ.

**Formal rule:**
```
[WF-Match-Exhaustive]
Γ ⊢ e : T
patterns {p₁, ..., pₙ} are exhaustive for T
─────────────────────────────────────────────
match e { p₁ -> e₁, ..., pₙ -> eₙ } well-formed
```

**Examples:**
```cantrip
// Exhaustive
match b: bool {
    true -> 1,
    false -> 0,
}

// Non-exhaustive (ERROR)
match b: bool {
    true -> 1,
}

// Exhaustive with wildcard
match x: i32 {
    0 -> "zero",
    _ -> "non-zero",
}
```

#### 3.3.4 Scoping Rules

**Variable Binding:**
- `let x = e₁ in e₂` introduces variable `x` in scope of `e₂`
- Function parameters introduce variables in function body scope
- Pattern matching introduces variables in match arm scope

**Type Parameter Binding:**
- `<T>` on function/record/enum introduces type parameter `T`
- Scope extends to function body / record fields / enum variants

**Shadowing:**
- Inner bindings shadow outer bindings with the same name
- Shadowing is lexically scoped (block-local)

**Example:**
```cantrip
let x = 10           // x₁ : i32 = 10
let x = x + 5        // x₂ : i32 = 15 (shadows x₁)
{
    let x = "hello"  // x₃ : str = "hello" (shadows x₂ in this block)
    print(x)         // Prints "hello" (refers to x₃)
}
print(x)             // Prints 15 (refers to x₂, x₃ out of scope)
```

### 3.4 Dynamic Semantics

**Purpose:** Abstract syntax nodes are evaluated at runtime according to operational semantics defined in Part X. This section explains how ASTs relate to evaluation.

**Evaluation operates on ASTs:** All evaluation rules in Part X operate on abstract syntax, not concrete text. For example:

**Evaluation judgment:**
```
⟨e, σ⟩ ⇓ ⟨v, σ'⟩

Where:
- e ∈ Expr (abstract expression)
- σ ∈ Store (memory state)
- v ∈ Value (result value)
- σ' ∈ Store (updated memory)
```

**Example evaluation rule (from §38):**
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨BinOp(Add, e₁, e₂), σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

**Key observation:** The rule operates on the AST node `BinOp(Add, e₁, e₂)`, not the text "e₁ + e₂".

**Evaluation order:** Specified in §41 (Evaluation Order), operates on AST structure.

**No runtime representation:** ASTs are compiled to machine code. The AST structure determines:
- Memory layout (§40)
- Evaluation order (§41)
- Effect execution (Part VI)
- Contract checking (Part V)

### 3.5 Additional Properties

#### 3.5.1 AST Structure Properties

**Property 3.1 (Unique Parsing):**
```
∀ source ∈ ValidSourceText. ∃! ast ∈ AST. parse(source) = ast

Reading: "Every valid source text parses to a unique AST."
```

**Property 3.2 (Pretty-Printing Inverse):**
```
∀ ast ∈ AST. parse(pretty_print(ast)) ≡ ast

Reading: "Parsing the pretty-printed form of an AST yields an equivalent AST."
```

**Property 3.3 (AST Traversal):**
```
∀ ast ∈ AST. size(ast) = 1 + Σ(size(child) for child in children(ast))

Reading: "The size of an AST is one plus the sum of sizes of its children."
```

#### 3.5.2 Disambiguation via AST

**Concrete Syntax:**
```cantrip
x + y * z
```

**Ambiguous interpretation 1:** `(x + y) * z`
```
AST: Mul(Add(x, y), z)
```

**Ambiguous interpretation 2:** `x + (y * z)`
```
AST: Add(x, Mul(y, z))
```

The parser resolves this using operator precedence (multiplication before addition), producing interpretation 2. Once in AST form, there is no ambiguity—the structure is explicit.

**All subsequent stages (type checking, optimization, evaluation) operate on the unambiguous AST.**

### 3.6 Examples and Patterns

#### 3.6.1 Function Definition: Concrete to Abstract

**Concrete Syntax:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y
}
```

**Abstract Syntax:**
```
FunctionDef {
    name: "add",
    params: [(x, i32), (y, i32)],
    return_type: i32,
    effects: ∅,
    contracts: none,
    body: BinOp(Add, Var("x"), Var("y"))
}
```

**Tree representation:**
```
FunctionDef
├── name: "add"
├── params: [(x, i32), (y, i32)]
├── return_type: i32
├── effects: ∅
└── body:
    BinOp(Add)
    ├── Var("x")
    └── Var("y")
```

#### 3.6.2 Pattern Matching: Concrete to Abstract

**Concrete Syntax:**
```cantrip
match result {
    Result.Ok(value) -> process(value),
    Result.Err(e) -> handle_error(e),
}
```

**Abstract Syntax:**
```
Match {
    scrutinee: Var("result"),
    arms: [
        Arm {
            pattern: EnumPattern("Result", "Ok", [VarPattern("value")]),
            guard: None,
            body: Call(Var("process"), [Var("value")])
        },
        Arm {
            pattern: EnumPattern("Result", "Err", [VarPattern("e")]),
            guard: None,
            body: Call(Var("handle_error"), [Var("e")])
        }
    ]
}
```

**Tree representation:**
```
Match
├── scrutinee: Var("result")
└── arms:
    ├── Arm
    │   ├── pattern: EnumPattern("Result", "Ok", [VarPattern("value")])
    │   └── body: Call(Var("process"), [Var("value")])
    └── Arm
        ├── pattern: EnumPattern("Result", "Err", [VarPattern("e")])
        └── body: Call(Var("handle_error"), [Var("e")])
```

#### 3.6.3 Record Construction: Concrete to Abstract

**Concrete Syntax:**
```cantrip
record Point { x: f32, y: f32 }

let p = Point { x: 1.0, y: 2.0 }
```

**Abstract Syntax:**
```
RecordDecl {
    name: "Point",
    fields: [(x, f32), (y, f32)]
}

LetBinding {
    var: "p",
    type: record Point,
    value: RecordConstruct {
        name: "Point",
        fields: [(x, Lit(1.0)), (y, Lit(2.0))]
    }
}
```

#### 3.6.4 Relationship to Formal Semantics

**Type Rules (Part II):** Type checking operates on abstract syntax trees, assigning types to expressions.

**Example:** The type rule for addition operates on abstract syntax:
```
[T-Add]
Γ ⊢ e₁ : T
Γ ⊢ e₂ : T
T ∈ {i8, i16, i32, i64, u8, u16, u32, u64, f32, f64}
─────────────────────────────────────────────────────
Γ ⊢ BinOp(Add, e₁, e₂) : T
```

Here `BinOp(Add, e₁, e₂)` refers to the abstract syntax node, not the string "e₁ + e₂". The parser has already transformed the concrete syntax `e₁ + e₂` into the AST node.

**Statement Separators in Type Rules:**

Both newlines and semicolons are treated identically in abstract syntax—they both represent sequencing. The type rule for sequencing applies uniformly:

```
[T-Seq]
Γ ⊢ e₁ : T₁
Γ ⊢ e₂ : T₂
────────────────────────
Γ ⊢ e₁ SEP e₂ : T₂

where SEP = NEWLINE or ";"
```

This means `let x = 1\nlet y = 2` and `let x = 1; let y = 2` have identical abstract syntax and type checking behavior.

**Evaluation Rules (Part X):** Operational semantics evaluates abstract syntax:
```
[E-Add]
⟨e₁, σ⟩ ⇓ ⟨n₁, σ'⟩
⟨e₂, σ'⟩ ⇓ ⟨n₂, σ''⟩
─────────────────────────────────────
⟨BinOp(Add, e₁, e₂), σ⟩ ⇓ ⟨n₁ + n₂, σ''⟩
```

Again, `BinOp(Add, e₁, e₂)` is the AST node. The evaluation judgment `⟨e, σ⟩ ⇓ ⟨v, σ'⟩` operates on ASTs, not concrete text.

**Why this matters:**
- **Uniformity**: All formal rules reference the same abstract structures
- **Precision**: AST nodes are unambiguous (no parsing issues)
- **Compositionality**: Tree structure enables recursive definitions
- **Implementation**: Compilers transform text → AST once, then work with AST throughout

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

**When NOT to use attributes:**
- To implement core language behavior (use language features instead)
- As a macro or metaprogramming system (use `comptime` instead)
- To change program logic (attributes modify HOW code compiles, not WHAT it does)
- For runtime configuration (use function parameters or configuration files)

**Relationship to other features:**
- **Contracts (§17)**: Attributes can select verification mode for contract checking
- **Effects (§21)**: Some attributes affect effect checking
- **Memory Layout (§6.5)**: `[[repr(...)]]` controls record/enum layout
- **FFI (Part XIV)**: Attributes critical for C interoperability
- **Metaprogramming (§34)**: Attributes are declarative; `comptime` is computational

**Design principle:** Attributes are **declarative compiler hints**, not executable code. They configure the compiler's behavior but don't execute at runtime (unless they change runtime behavior like `[[overflow_checks]]`).

### 4.2 Syntax

#### 4.2.1 Concrete Syntax

**Grammar:**
```ebnf
Attribute      ::= "[[" AttributeBody "]]"
AttributeBody  ::= Ident ( "(" AttrArgs? ")" )?
AttrArgs       ::= AttrArg ( "," AttrArg )*
AttrArg        ::= Ident "=" Literal | Literal | Ident

AttributeList  ::= Attribute+
```

**Syntax:**
```cantrip
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
function to_str(x: Debug): String { ... }
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

#### 4.2.2 Abstract Syntax

**Formal representation:**
```
Attribute ::= Attr(name: Ident, args: List⟨AttrArg⟩)
AttrArg   ::= Named(key: Ident, value: Literal)
            | Positional(value: Literal)
            | Flag(name: Ident)

Item      ::= item_kind × attrs: List⟨Attribute⟩
```

**Components:**
- **name**: Attribute identifier (e.g., `repr`, `verify`, `module`)
- **args**: List of attribute arguments
  - **Named**: Key-value pairs (`name = "value"`)
  - **Positional**: Just values (`C`, `static`)
  - **Flag**: Just identifiers (`inline`)

**AST representation:**
```
RecordDecl {
    name: "Point",
    fields: [...],
    attributes: [
        Attr("repr", [Positional(Ident("C"))]),
        Attr("module", [Named("name", String("ffi"))])
    ]
}
```

#### 4.2.3 Basic Examples

```cantrip
// Memory layout control
[[repr(C)]]
record Point {
    x: f32,
    y: f32,
}

// Verification mode
[[verify(static)]]
function safe_divide(x: i32, y: i32): i32
    must y != 0
{ x / y }

// Overflow checking
[[overflow_checks(false)]]
function fast_math(x: u32): u32 {
    x * x  // Won't check for overflow even in debug mode
}

// Module metadata
[[module(name = "core", version = "1.0")]]
module math { ... }

// Tooling support
[[alias("to_string", "stringify")]]
public function to_str(x: Debug): String {
    x.debug()
}
```

### 4.3 Static Semantics

#### 4.3.1 Well-Formedness Rules

**Attribute placement validity:**
```
[WF-Attr-Placement]
attr is an attribute
item is a declaration kind
placement(attr) allows item.kind
──────────────────────────────────
[[attr]] item is well-formed
```

**Attribute argument validation:**
```
[WF-Attr-Args]
attr = Attr(name, args)
∀arg ∈ args. signature(attr.name) accepts arg
─────────────────────────────────────────────
attribute arguments well-formed
```

**No conflicting attributes:**
```
[WF-Attr-No-Conflict]
attrs = list of attributes on item
∀a₁, a₂ ∈ attrs. a₁ ≠ a₂ ⇒ ¬conflicts(a₁, a₂)
───────────────────────────────────────────────
attribute list well-formed
```

**Examples:**
```cantrip
// Valid
[[repr(C)]]
record Point { ... }

// INVALID: repr only applies to records/enums
[[repr(C)]]          // ERROR: repr doesn't apply to functions
function foo() { ... }

// INVALID: conflicting attributes
[[inline]]
[[no_inline]]        // ERROR: inline and no_inline conflict
function bar() { ... }
```

#### 4.3.2 Core Attributes

Cantrip defines the following core attributes. All core attributes are NORMATIVE.

##### 4.3.2.1 `[[repr(...)]]` - Memory Layout Control

**Signature:** `[[repr(layout)]]` where `layout ∈ {C, transparent, packed, align(n)}`

**Applies to:** Records, enums

**Semantics:**
```
[Attr-Repr-C]
[[repr(C)]] record R { ... }
──────────────────────────────
R uses C-compatible layout (see §6.5)

[Attr-Repr-Packed]
[[repr(packed)]] record R { ... }
──────────────────────────────────
R has no padding between fields
```

**Example:**
```cantrip
// C-compatible layout for FFI
[[repr(C)]]
record Point {
    x: f32,  // offset 0
    y: f32,  // offset 4
}  // total size: 8, alignment: 4

// Packed layout (no padding)
[[repr(packed)]]
record Packed {
    a: u8,   // offset 0
    b: u32,  // offset 1 (no alignment padding!)
}  // total size: 5
```

##### 4.3.2.2 `[[verify(...)]]` - Verification Mode

**Signature:** `[[verify(mode)]]` where `mode ∈ {static, runtime, none}`

**Applies to:** Functions, procedures, records (for invariants)

**Semantics:**
```
[Attr-Verify-Static]
[[verify(static)]] function f(...) must P will Q { ... }
────────────────────────────────────────────────────────────────
contracts checked at compile-time via SMT solver

[Attr-Verify-Runtime]
[[verify(runtime)]] function f(...) must P will Q { ... }
──────────────────────────────────────────────────────────────────
contracts checked at runtime (assertions injected)

[Attr-Verify-None]
[[verify(none)]] function f(...) must P will Q { ... }
────────────────────────────────────────────────────────────
contracts are documentation only (no checking)
```

**Example:**
```cantrip
// Static verification (compile-time proof)
[[verify(static)]]
function binary_search(arr: [i32], target: i32): Option<usize>
    must is_sorted(arr)
{ ... }

// Runtime verification (dynamic checks)
[[verify(runtime)]]
function parse_int(s: str): i32
    must !s.is_empty()
    will result >= 0
{ ... }
```

##### 4.3.2.3 `[[overflow_checks(...)]]` - Integer Overflow Behavior

**Signature:** `[[overflow_checks(enabled)]]` where `enabled ∈ {true, false}`

**Applies to:** Functions, procedures, blocks

**Semantics:**
```
[Attr-Overflow-Enable]
[[overflow_checks(true)]] function f(...) { ... }
───────────────────────────────────────────────────
all integer operations in f check for overflow (even in release mode)

[Attr-Overflow-Disable]
[[overflow_checks(false)]] function f(...) { ... }
────────────────────────────────────────────────────
no overflow checks in f (wrapping behavior, even in debug mode)
```

**Example:**
```cantrip
// Always check for overflow (safety-critical code)
[[overflow_checks(true)]]
function account_balance(deposits: [u64]): u64 {
    deposits.sum()  // Panics on overflow even in --release
}

// Performance-critical code (wrapping is acceptable)
[[overflow_checks(false)]]
function hash_combine(a: u64, b: u64): u64 {
    a.wrapping_mul(31).wrapping_add(b)
}
```

##### 4.3.2.4 `[[module(...)]]` - Module Metadata

**Signature:** `[[module(key = value, ...)]]`

**Applies to:** Module declarations

**Semantics:** Provides metadata for package management and tooling.

**Common keys:**
- `name`: Module name (string)
- `version`: Semantic version (string)
- `author`: Author information (string)
- `description`: Module description (string)

**Example:**
```cantrip
[[module(
    name = "cantrip.core",
    version = "1.0.0",
    author = "Cantrip Team",
    description = "Core library for Cantrip language"
)]]
module core {
    ...
}
```

##### 4.3.2.5 `[[alias(...)]]` - Alternative Names for Tooling

**Signature:** `[[alias(name1, name2, ...)]]`

**Applies to:** Functions, procedures, types, modules

**Semantics:**
Declares alternative source-level names for an item to improve diagnostics, code search, and LLM tooling. Aliases are INFORMATIVE for tooling and MUST NOT affect name resolution or symbol linkage.

**Normative rules:**
```
[Attr-Alias-Informative]
[[alias(a₁, ..., aₙ)]] item
─────────────────────────────────────────
aliases are accepted in diagnostics and IDE features only
aliases do NOT create new exported symbols

[Attr-Alias-Collision]
[[alias(a)]] item₁
real_name(item₂) = a
item₁ ≠ item₂
──────────────────────────────────
emit warning W0101: alias collides with real name
real name wins in all resolution
```

**Example:**
```cantrip
// Provide search-friendly aliases
[[alias("to_string", "stringify", "as_str")]]
public function to_str(x: Debug): String {
    x.debug()
}

// IDE may suggest "to_string" when user types it
// but compiled code always uses "to_str"
```

**For FFI symbol names, use `[[export(...)]]` instead** (see Part XIV: FFI).

##### 4.3.2.6 Other Core Attributes

**`[[inline]]`** - Hint to inline this function
**`[[no_inline]]`** - Prevent inlining
**`[[no_mangle]]`** - Preserve exact symbol name (for FFI)
**`[[deprecated(message)]]`** - Mark as deprecated
**`[[must_use]]`** - Warn if return value is ignored
**`[[cold]]`** - Hint this code is rarely executed
**`[[hot]]`** - Hint this code is performance-critical

These are documented in their respective sections.

#### 4.3.3 User-Defined Attributes

**Status:** User-defined attributes are RESERVED for future versions.

Currently, only core attributes defined in this specification are recognized. User-defined attributes will cause a compilation error:

```cantrip
[[my_custom_attr]]    // ERROR: unknown attribute 'my_custom_attr'
function foo() { ... }
```

**Future design:** User-defined attributes may be added in conjunction with procedural macros or compile-time reflection (see §34).

### 4.4 Dynamic Semantics

#### 4.4.1 Compile-Time Processing

Attributes are processed during compilation and do not exist at runtime (with exceptions noted below).

**Processing phases:**
```
Source Code → [Lexer] → [Parser] → AST with Attributes → [Attribute Processing]
                                                                   ↓
                                    [Type Checker] ← Configuration applied
                                          ↓
                                    [Code Generator] ← Layout/optimization directives
```

**Attribute processing order:**
1. **Parsing**: Attributes parsed into AST
2. **Validation**: Well-formedness checking (§4.3.1)
3. **Application**: Attributes modify compilation behavior
4. **Erasure**: Most attributes erased after compilation

#### 4.4.2 Attributes Affecting Runtime Behavior

Some attributes change generated code and thus affect runtime:

**`[[overflow_checks(...)]]`:**
- Changes whether overflow checks are emitted
- Affects runtime performance and behavior

**`[[repr(...)]]`:**
- Changes memory layout
- Affects runtime memory usage and access patterns

**`[[inline]]`, `[[no_inline]]`:**
- Change code generation
- Affect runtime performance (but not observable behavior)

**`[[verify(runtime)]]`:**
- Injects runtime assertion checks
- Affects runtime behavior (can panic)

#### 4.4.3 Attributes Not Affecting Runtime

These attributes are purely compile-time metadata:

**`[[verify(static)]]`, `[[verify(none)]]`:**
- Only affect compile-time checking
- No runtime code generated

**`[[alias(...)]]`:**
- Informative only for tooling
- Zero runtime impact

**`[[module(...)]]`:**
- Metadata only
- May appear in debug info but not executable code

#### 4.4.4 Attribute Inheritance

**Attributes do NOT inherit:**
```cantrip
[[inline]]
record Container {
    [[inline]]      // Must explicitly annotate field if desired
    value: i32,
}

[[verify(static)]]
trait Validated {
    // Implementations do NOT automatically get [[verify(static)]]
    function check(self) -> bool;
}
```

Each item must be annotated independently.

### 4.5 Additional Properties

#### 4.5.1 Attribute Conflicts

**Conflicting attributes:**
- `[[inline]]` and `[[no_inline]]`
- `[[repr(C)]]` and `[[repr(packed)]` (use `[[repr(C, packed)]]` instead)

**Conflict detection:**
```
conflicts([[inline]], [[no_inline]]) = true
conflicts([[verify(static)]], [[verify(runtime)]) = true
conflicts([[repr(C)]], [[repr(transparent)]) = true
```

**Error when conflicts detected:**
```cantrip
[[inline]]
[[no_inline]]    // ERROR E4001: conflicting attributes 'inline' and 'no_inline'
function foo() { ... }
```

#### 4.5.2 Unknown Attribute Handling

**Unknown attributes cause compilation errors:**
```cantrip
[[unknown_attr]]    // ERROR E4002: unknown attribute 'unknown_attr'
function bar() { ... }
```

**Rationale:** Explicit errors prevent typos and forward compatibility issues.

**Namespacing:** Future versions may support namespaced attributes: `[[cantrip::core::inline]]`

### 4.6 Examples and Patterns

#### 4.6.1 FFI Data Structure

**Pattern:** C-compatible record layout for foreign function interface.

```cantrip
// Cantrip side (FFI bindings)
[[repr(C)]]
[[module(name = "ffi.sdl")]]
record SDL_Point {
    x: i32,
    y: i32,
}

[[repr(C)]]
record SDL_Rect {
    x: i32,
    y: i32,
    w: i32,
    h: i32,
}

// C side (SDL library)
// typedef struct {
//     int x;
//     int y;
// } SDL_Point;
```

**Explanation:** `[[repr(C)]]` will memory layout matches C exactly, enabling zero-cost FFI.

#### 4.6.2 Safety-Critical Verification

**Pattern:** Static verification for safety-critical code with runtime checks as fallback.

```cantrip
// Statically verified core logic
[[verify(static)]]
function compute_trajectory(
    velocity: Vector3,
    acceleration: Vector3,
    time: f64
): Vector3
    must time >= 0.0
    will result.magnitude() <= velocity.magnitude() + acceleration.magnitude() * time
{
    velocity + acceleration * time
}

// Runtime-verified external input handling
[[verify(runtime)]]
function parse_sensor_data(raw: [u8]): SensorReading
    must raw.len() >= 16
    will result.timestamp > 0
{
    // Parser implementation with runtime checks
    ...
}
```

**Explanation:** Use `[[verify(static)]]` for provably safe internal logic; use `[[verify(runtime)]]` for external input where static proof may be infeasible.

#### 4.6.3 Performance-Critical Tuning

**Pattern:** Fine-tuned optimization hints for hot code paths.

```cantrip
// High-frequency game loop
[[inline]]
[[overflow_checks(false)]]  // Wrapping is safe here
function update_entity_positions(
    entities: [mut Entity],
    delta_time: f32
) {
    for entity in entities {
        // Position wraps around world bounds
        entity.x = entity.x.wrapping_add((entity.vx * delta_time) as u32);
        entity.y = entity.y.wrapping_add((entity.vy * delta_time) as u32);
    }
}

// Rare error path
[[cold]]
function handle_critical_error(error: Error): ! {
    log_error(error);
    std.process.abort();
}
```

**Explanation:** `[[inline]]` reduces function call overhead; `[[overflow_checks(false)]]` eliminates checks where wrapping is intentional; `[[cold]]` tells optimizer this path is rare.

#### 4.6.4 Tooling-Friendly Aliases

**Pattern:** Provide discoverable aliases for common operations.

```cantrip
// Mathematical vector with multiple naming conventions
[[alias("len", "magnitude", "length", "norm")]]
public function size(v: Vector3): f32 {
    (v.x * v.x + v.y * v.y + v.z * v.z).sqrt()
}

// Collection methods with familiar names
[[alias("push_back", "append", "add")]]
public function push(vec: mut Vec<T>, item: T) {
    vec.internal_push(item);
}
```

**Explanation:** LLMs and developers can find functions using familiar terminology, while maintaining canonical naming in actual code.

---

