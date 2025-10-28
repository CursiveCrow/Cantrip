# Cantrip Language Specification: Organization Proposal

**Document Version:** 1.0
**Date:** October 26, 2025
**Status:** Proposal
**Based On:** Cross-language specification analysis (Java JLS, C++ Standard, Go Spec, Rust Reference, Swift Language Reference)

---

## Executive Summary

This document proposes a comprehensive organization structure for the Cantrip Language Specification based on analysis of five major language specifications. The proposal provides:

1. Complete part ordering and dependency structure
2. Detailed section-by-section organization for missing parts
3. Consistent formatting guidelines across all sections
4. Implementation priorities and timeline
5. Quality standards and review criteria

**Current Status:** Parts I, II, and IV are complete (35 files). Parts III, V-XIV need to be written (~150 files estimated).

**Key Finding:** Cantrip's type system specification is excellent and complete. The remaining work focuses on operational semantics, memory model, and practical language features.

---

## Table of Contents

- [1. Research Findings: Cross-Language Specification Patterns](#1-research-findings-cross-language-specification-patterns)
- [2. Recommended Specification Structure](#2-recommended-specification-structure)
- [3. Detailed Part Organization](#3-detailed-part-organization)
- [4. Section Template Standards](#4-section-template-standards)
- [5. Formatting and Style Guidelines](#5-formatting-and-style-guidelines)
- [6. Implementation Roadmap](#6-implementation-roadmap)
- [7. Quality Assurance Criteria](#7-quality-assurance-criteria)

---

## 1. Research Findings: Cross-Language Specification Patterns

### 1.1 Universal Organization Principles

Analysis of Java JLS, C++ Standard, Go Spec, Rust Reference, and Swift revealed consistent patterns:

#### Principle 1: Graduated Complexity
**Pattern:** Simple → Intermediate → Advanced

- **Java JLS:** Primitives (Ch 4) → Objects (Ch 8) → Generics (Ch 9)
- **Rust Reference:** Basic types → Ownership → Advanced features
- **Go Spec:** Types → Declarations → Expressions
- **Swift:** Basic types → Collections → Protocols → Generics

**Application to Cantrip:** Foundation types first, then composition, then abstraction, then advanced features.

#### Principle 2: Dependency Order
**Pattern:** Each part builds on previous parts

- **C++ Standard:** Lexical conventions → Basics → Standard library (strict ordering)
- **Java JLS:** Types before expressions before statements
- **Rust Reference:** Progressive complexity (but different memory model than Cantrip)

**Application to Cantrip:** Types → Expressions → Functions → Modules → Advanced features

#### Principle 3: Tripartite Semantics
**Pattern:** Syntax → Static Semantics → Dynamic Semantics

- **Java JLS:** Grammar → Type rules → Runtime behavior
- **C++ Standard:** Syntax → Constraints → Semantics
- **Rust Reference:** Syntax → Validity → Operational semantics

**Application to Cantrip:** Already following this pattern (maintain consistency).

#### Principle 4: Practical Before Theoretical
**Pattern:** Operational features before formal proofs

- **Java JLS:** Language features (Ch 1-18) → VM Spec (separate document)
- **Rust Reference:** Language reference → Formal verification (separate)
- **Go Spec:** Practical spec → Formal proofs not included

**Application to Cantrip:** Core language parts → Formal semantics part at end

### 1.2 Section Structure Patterns

Common section elements across all specifications:

```
1. HEADER & NAVIGATION
   - Section number and title
   - Previous/Next links
   - Formal definition (boxed)

2. OVERVIEW
   - Purpose and motivation
   - When to use / when NOT to use
   - Relationship to other features

3. SYNTAX
   - Concrete syntax (EBNF grammar)
   - Abstract syntax (formal notation)
   - Basic examples (2-4 simple cases)

4. STATIC SEMANTICS
   - Well-formedness rules
   - Type rules (inference rules)
   - Theorems and properties

5. DYNAMIC SEMANTICS
   - Evaluation rules
   - Memory representation
   - Operational behavior

6. EXAMPLES & PATTERNS
   - Real-world usage patterns
   - Common idioms
   - Anti-patterns to avoid

7. CROSS-REFERENCES & NOTES
   - Related sections
   - Warnings and caveats
   - Implementation notes
```

### 1.3 Numbering and Labeling Conventions

#### Java JLS Pattern (Most Formal)
- Sections: §4.2.3
- Examples: "Example 4.2.3-1: Integer Overflow"
- Tables: "Table 4.2.3-A: Integer Type Ranges"
- Notes: Numbered paragraphs with labeled "Note:" markers

#### Rust Reference Pattern (Most Practical)
- Sections: Simple numbers
- Examples: Inline code blocks with explanatory text
- Tables: Standard markdown tables with captions
- Notes: Blockquote > formatting

#### C++ Standard Pattern (Most Exhaustive)
- Sections: Numbered paragraphs (every paragraph numbered)
- Examples: "[Example: ... —end example]"
- Notes: "[Note: ... —end note]"
- Tables: Numbered with descriptive captions

**Recommendation for Cantrip:** Blend Java + Rust patterns
- Use Java's formal numbering for definitions, theorems, examples
- Use Rust's markdown-friendly formatting
- Use C++'s labeled note/example format for clarity

### 1.4 Cross-Reference Patterns

All specifications use consistent cross-reference formats:

- **Section references:** §X.Y or (§X.Y)
- **Forward references:** "See §X.Y for details"
- **Backward references:** "As defined in §X.Y"
- **External references:** [Standard-Year] format

---

## 2. Recommended Specification Structure

### 2.1 Complete Part Ordering

Based on dependency analysis and cross-language patterns:

```
PART I: FOUNDATIONS (Complete ✓)
├── Mathematical Foundations
├── Lexical Structure
├── Abstract Syntax
└── Attributes

PART II: TYPE SYSTEM (Complete ✓)
├── 00_Introduction
├── Primitives (01a-01f)
├── Collections (02)
├── Product Types (03, 04, 04a)
├── Sum Types (05)
├── Traits (06)
├── Generics (07)
├── Modals (08)
├── Pointers (09)
├── Map Types (10)
├── Type Aliases (11)
└── Self Type (12)

PART III: EXPRESSIONS & STATEMENTS (CRITICAL - FOUNDATION)
├── 00_Introduction
├── 01_Literals
├── 02_Identifiers
├── 03_Operators
├── 04_FunctionCalls
├── 05_Construction
├── 06_Blocks
├── 07_Conditionals
├── 08_PatternMatching
├── 09_Loops (exists)
├── 10_Assignment
├── 11_Returns
└── 12_ErrorHandling

PART IV: CONTRACTS (Complete ✓)
├── Overview
├── Behavioral Contracts
├── Effects
├── Preconditions
├── Postconditions
├── Type Constraints
└── Contract Composition

PART V: PERMISSIONS & MEMORY (CRITICAL - CORE SAFETY)
├── 00_Overview
├── 01_PermissionSystem
├── 02_Ownership
├── 03_PermissionSemantics
├── 04_Isolation
├── 05_PermissionTransitions
├── 06_MemoryRegions
└── 07_SafetyProofs

PART VI: FUNCTIONS & PROCEDURES (DEPENDS ON III, V)
├── 00_Overview
├── 01_FunctionDeclaration
├── 02_ProcedureDeclaration
├── 03_Parameters
├── 04_ClosuresAndCapture
├── 05_HigherOrderFunctions
└── 06_Overloading (or document absence)

PART VII: PATTERN MATCHING (DEPENDS ON III, V)
├── 00_Overview
├── 01_PatternSyntax
├── 02_StructuredPatterns
├── 03_RefutablePatterns
└── 04_PatternBindings

PART VIII: MODULES & VISIBILITY (DEPENDS ON II)
├── 01_FileBasedModules
├── 02_ImportExport
├── 03_Visibility
├── 04_ModuleResolution
└── 05_PackageStructure

PART IX: METAPROGRAMMING (DEPENDS ON II, VI)
├── 01_ComptimeOverview
├── 02_ComptimeExpressions
├── 03_TypeIntrospection
├── 04_CodeGeneration
└── 05_ComptimeValidation

PART X: CONCURRENCY (DEPENDS ON V, VI)
├── 01_ConcurrencyModel
├── 02_AsyncAwait
├── 03_Actors
├── 04_MessagePassing
└── 05_Synchronization

PART XI: FFI (DEPENDS ON V, VI)
├── 01_FFIOverview
├── 02_ForeignDeclarations
├── 03_TypeMappings
├── 04_OwnershipAcrossFFI
├── 05_Callbacks
└── 06_SafetyBoundaries

PART XII: STANDARD LIBRARY (DEPENDS ON ALL CORE PARTS)
├── 01_CoreTypes
├── 02_Collections
├── 03_IOAndFileSystem
├── 04_Networking
└── 05_ConcurrencyPrimitives

PART XIII: FORMAL SEMANTICS (REFERENCES ALL PARTS)
├── 01_SmallStepSemantics
├── 02_BigStepSemantics
├── 03_MemoryModel
├── 04_TypeSoundness
├── 05_EffectSoundness
├── 06_MemorySafety
└── 07_ModalSafety

PART XIV: TOOLING & IMPLEMENTATION (NON-NORMATIVE)
├── 01_CompilerArchitecture
├── 02_ErrorReporting
├── 03_PackageManagement
└── 04_TestingFramework

APPENDICES
├── Grammar (exists)
├── Errors (exists)
├── Keywords
└── Index
```

### 2.2 Part Dependencies

Dependency graph showing which parts can be written in parallel:

```
Layer 1 (Foundation - Can be parallel):
  I: Foundations ✓
  II: Type System ✓

Layer 2 (Depends on Layer 1):
  III: Expressions (depends on II)
  IV: Contracts ✓

Layer 3 (Depends on Layer 2):
  V: Permissions (depends on II, III)
  VII: Pattern Matching (depends on II, III)

Layer 4 (Depends on Layer 3):
  VI: Functions (depends on III, V)

Layer 5 (Depends on Layer 4):
  VIII: Modules (depends on II, VI)
  IX: Metaprogramming (depends on II, VI)
  X: Concurrency (depends on V, VI)
  XI: FFI (depends on V, VI)

Layer 6 (Depends on Layer 5):
  XII: Standard Library (depends on all core parts)

Layer 7 (Final):
  XIII: Formal Semantics (references all parts)
  XIV: Tooling (non-normative)
```

---

## 3. Detailed Part Organization

### Part III: Expressions & Statements

**Critical Priority:** Cannot implement language without expression semantics

#### Structure

```markdown
Part III: Expressions and Statements

00_Introduction.md
  - Expression vs statement distinction (if any)
  - Expression taxonomy
  - Evaluation order (left-to-right)
  - Type of expressions

01_Literals.md
  §3.1 Integer Literals
    - Decimal, hexadecimal, binary, octal notation
    - Type inference rules
    - Overflow behavior

  §3.2 Floating-Point Literals
    - Scientific notation
    - IEEE 754 compliance

  §3.3 Boolean Literals (true, false)
  §3.4 Character Literals (Unicode)
  §3.5 String Literals
    - Raw strings
    - String interpolation (if supported)

  §3.6 Unit Literal ()

02_Identifiers.md
  §3.7 Variable References
    - Name resolution
    - Shadowing rules

  §3.8 Field Access (expr.field)
    - Dot notation
    - Permission checking

  §3.9 Method Access (expr.method)
    - Self parameter binding

  §3.10 Qualified Names (Module::item)

03_Operators.md
  §3.11 Arithmetic Operators
    - Addition (+), Subtraction (-), Multiplication (*), Division (/), Remainder (%)
    - Overflow behavior (wrapping, panicking, or checked)
    - Type constraints

  §3.12 Comparison Operators
    - Equality (==, !=)
    - Ordering (<, >, <=, >=)
    - Return type (bool)

  §3.13 Logical Operators
    - AND (&&), OR (||), NOT (!)
    - Short-circuit evaluation semantics

  §3.14 Bitwise Operators
    - AND (&), OR (|), XOR (^)
    - Shift left (<<), Shift right (>>)

  §3.15 Operator Precedence Table
    [Complete precedence and associativity table]

  §3.16 Operator Overloading (if supported)

04_FunctionCalls.md
  §3.17 Function Call Syntax
    - Call syntax: func(arg1, arg2)
    - Named arguments (if supported)

  §3.18 Argument Evaluation Order
    - Left-to-right evaluation (normative)
    - Sequence points

  §3.19 Method Call Syntax
    - Receiver syntax: receiver.method(args)
    - Self parameter binding ($)

  §3.20 Associated Function Calls
    - Type::function() syntax
    - Static functions

05_Construction.md
  §3.21 Record Construction
    - Record { field1: value1, field2: value2 }
    - Field initialization order
    - Field punning (if supported)

  §3.22 Tuple Construction
    - (value1, value2, ...)
    - Unit tuple ()

  §3.23 Enum Variant Construction
    - Enum.Variant syntax
    - Variant with data

  §3.24 Array Construction
    - [elem1, elem2, ...]
    - [elem; count] repetition syntax

06_Blocks.md
  §3.25 Block Expressions
    - Syntax: { stmt1; stmt2; ... }
    - Block as expression (last value)

  §3.26 Block Scope
    - Lexical scoping
    - Variable shadowing

  §3.27 Block Return Values
    - Implicit return (last expression)
    - Early return with return keyword

07_Conditionals.md
  §3.28 if Expressions
    - Syntax: if condition { ... }
    - Condition must be bool type

  §3.29 else Clauses
    - else { ... }
    - else if { ... }

  §3.30 Conditional as Expression
    - Both branches must have same type
    - Type inference rules

08_PatternMatching.md
  §3.31 match Expression Syntax
    - match value { pattern => expr, ... }
    - Evaluation semantics

  §3.32 Pattern Syntax
    - Literal patterns
    - Identifier patterns
    - Wildcard (_)
    - Or patterns (|)

  §3.33 Exhaustiveness Checking
    - Completeness requirement
    - Unreachable pattern detection

  §3.34 Guard Clauses
    - pattern if condition => expr
    - Guard evaluation order

  §3.35 Pattern Refutability
    - Irrefutable vs refutable patterns
    - Where each is allowed

09_Loops.md (EXISTS - review and enhance)
  §3.36 loop (infinite loop)
  §3.37 while Loops
  §3.38 for Loops
  §3.39 Loop Labels
  §3.40 break and continue
  §3.41 Loop Return Values

10_Assignment.md
  §3.42 let Bindings
    - Immutable bindings (default)
    - Type inference

  §3.43 let mut Bindings
    - Mutable bindings
    - Permission requirements

  §3.44 Destructuring Assignment
    - Tuple destructuring: let (a, b) = tuple
    - Record destructuring

  §3.45 Mutation (=)
    - Requires mut permission
    - Lvalue requirements

  §3.46 Compound Assignment
    - +=, -=, *=, /=, %=
    - &=, |=, ^=, <<=, >>=

11_Returns.md
  §3.47 return Statement
    - Explicit return
    - Return type checking

  §3.48 Implicit Returns
    - Last expression value
    - Block without semicolon

  §3.49 Early Returns
    - Multiple return points
    - Control flow analysis

12_ErrorHandling.md
  §3.50 ? Operator
    - Result<T, E> propagation
    - Early return on Err
    - Type conversion

  §3.51 panic! Macro
    - Unrecoverable errors
    - Stack unwinding (if applicable)

  §3.52 unreachable! Macro
    - Marking unreachable code
    - Optimization hints
```

### Part V: Permissions & Memory Management

**Critical Priority:** Core to Cantrip's safety model (LPS)

#### Structure

```markdown
Part V: Permissions and Memory Management

00_Overview.md
  - Lexical Permission System (LPS) philosophy
  - NOT Rust's borrow checker - fundamentally different approach
    * Cantrip: Permission-based with regions
    * Rust: Borrow checking with lifetime parameters
    * Cantrip: Simpler, no lifetime annotations
  - Safety guarantees
  - What LPS prevents: use-after-free, double-free, memory leaks
  - What LPS does NOT prevent: data races, aliasing bugs (programmer responsibility)
  - Design choice: Explicit simplicity over compile-time race prevention

01_PermissionSystem.md
  §5.1 Permission Types
    - own: Owned values (move semantics)
    - mut: Mutable permission (allows mutation)
    - imm: Immutable permission (default, read-only)
    - iso: Isolated permission (deep ownership, thread-safe)

  §5.2 Permission Annotations
    - Syntax for each permission
    - Default rules (imm by default)

  §5.3 Permission Inference
    - When explicit annotations required
    - When inference applies

  §5.4 Permission Constraints
    - Compile-time permission checking
    - Permission flow rules

02_Ownership.md
  §5.5 Own Permission Semantics
    - Transfer of ownership
    - Move semantics (value consumed)

  §5.6 Move Semantics
    - When moves occur
    - Copy vs Move (based on type)

  §5.7 Ownership Transfer
    - Function parameters
    - Return values
    - Assignment

  §5.8 Drop and Destruction
    - Deterministic deallocation
    - Drop order (reverse of creation)
    - Manual drop (if supported)

03_PermissionSemantics.md
  §5.9 Immutable Permission (default)
    - Default parameter passing mode
    - Multiple immutable accesses allowed
    - Read-only access semantics

  §5.10 Mutable Permission (mut)
    - Syntax: mut keyword
    - Allows in-place mutation
    - Permission verified at compile time

  §5.11 Permission Checking
    - Lexical scope-based (no lifetime annotations)
    - Compile-time verification
    - Region-based memory safety

  §5.12 Cantrip LPS vs Rust Borrowing
    - Cantrip: Permission system + Region-based memory
    - Rust: Borrow checker + Lifetime parameters
    - Cantrip: Simpler, no lifetimes, regions handle safety
    - Rust: More complex, lifetimes everywhere, prevents data races
    - Different trade-offs: Cantrip prioritizes simplicity

04_Isolation.md
  §5.13 Isolated Permission (iso)
    - Deep ownership guarantee
    - No shared references exist

  §5.14 Deep Ownership
    - Transitive ownership through object graph
    - No external aliases

  §5.15 Isolation Guarantees
    - Safe concurrency (can send across threads)
    - Safe mutation

05_PermissionTransitions.md
  §5.16 Permission Flow
    - How permissions flow through program
    - Permission loss and recovery

  §5.17 Function Boundaries
    - Permission transfer at calls
    - Return permission rules

  §5.18 Method Permission Requirements
    - Self parameter permissions
    - Procedure permission declarations

06_MemoryRegions.md
  §5.19 Region Overview
    - Stack vs Heap vs Region allocation
    - Region-based memory management

  §5.20 Region Declaration
    - region keyword
    - Region scope

  §5.21 Region Allocation
    - Allocating into regions
    - Syntax and semantics

  §5.22 Region Deallocation
    - Automatic deallocation on region end
    - Bulk deallocation performance

  §5.23 Escape Analysis
    - Preventing references from escaping region
    - Compile-time checking

  §5.24 Performance Characteristics
    - Allocation cost: O(1)
    - Deallocation cost: O(1)
    - Comparison to malloc/free

07_SafetyProofs.md
  §5.25 No Use-After-Free
    - Theorem: Well-typed programs cannot use freed memory
    - Proof sketch

  §5.26 No Double-Free
    - Theorem: Memory freed exactly once
    - Linear types guarantee

  §5.27 No Memory Leaks
    - Deterministic deallocation guarantee
    - Region cleanup guarantee

  §5.28 What LPS Does NOT Prevent
    - Data races (allowed by design)
    - Aliasing bugs (programmer checks needed)
    - Logic errors
    - Explicit rationale for trade-offs
```

### Part VI: Functions & Procedures

```markdown
Part VI: Functions and Procedures

00_Overview.md
  - Function vs Procedure distinction
  - Pure functions (no effects) vs Procedures (with effects)
  - First-class functions

01_FunctionDeclaration.md
  §6.1 Function Syntax
    - function keyword
    - Parameters and return type

  §6.2 Pure Functions
    - No effect annotations
    - Referential transparency
    - Optimization opportunities

  §6.3 Function Type Signature
    - map(T1, T2) -> R syntax
    - Relationship to §10 (Map Types)

02_ProcedureDeclaration.md
  §6.4 Procedure Syntax
    - procedure keyword
    - Required effect declarations

  §6.5 Effect Declarations (uses)
    - Declaring effects procedure uses
    - Effect checking

  §6.6 Contracts (must, will)
    - Preconditions (must)
    - Postconditions (will)
    - Integration with Part IV (Contracts)

03_Parameters.md
  §6.7 Parameter Permissions
    - Default: immutable reference
    - own: Takes ownership
    - mut: Mutable reference

  §6.8 Default Parameters (if supported)

  §6.9 Variadic Parameters (if supported)
    - Syntax
    - Type constraints

04_ClosuresAndCapture.md
  §6.10 Closure Syntax
    - |param1, param2| expr
    - Block closures |params| { ... }

  §6.11 Environment Capture
    - What can be captured
    - Capture modes (by reference, by value, by move)

  §6.12 Fn/FnMut/FnOnce Contracts
    - Fn: Can be called repeatedly, captures immutably
    - FnMut: Can be called repeatedly, captures mutably
    - FnOnce: Can be called once, consumes captures

  §6.13 Move Closures
    - move keyword
    - Transfer ownership into closure

05_HigherOrderFunctions.md
  §6.14 Functions as Parameters
    - Accepting function types
    - Generic over functions

  §6.15 Returning Functions
    - Function return types
    - Closure return types

  §6.16 Function Composition
    - Combining functions
    - Common patterns

06_Overloading.md
  §6.17 Function Overloading Support
    - If supported: Overload resolution rules
    - If NOT supported: Explicit documentation of design choice
```

### Part VII: Pattern Matching

```markdown
Part VII: Pattern Matching

00_Overview.md
  - Pattern contexts (match, let, function parameters, if let)
  - Exhaustiveness requirement
  - Pattern evaluation order

01_PatternSyntax.md
  §7.1 Literal Patterns
    - Integer, boolean, character, string literals
    - Matching semantics

  §7.2 Identifier Patterns
    - Binding variables
    - Shadowing in patterns

  §7.3 Wildcard Pattern (_)
    - Ignoring values
    - Partial destructuring

  §7.4 Rest Pattern (..)
    - Ignoring multiple elements
    - In tuples and arrays

  §7.5 Or Patterns (|)
    - Multiple alternatives
    - Binding consistency across alternatives

02_StructuredPatterns.md
  §7.6 Tuple Patterns
    - (pat1, pat2, ...)
    - Nested tuples

  §7.7 Record Patterns
    - Record { field1: pat1, field2: pat2 }
    - Field punning
    - Rest patterns in records

  §7.8 Enum Patterns
    - Enum.Variant(pat)
    - Enum.Variant { field: pat }

  §7.9 Nested Patterns
    - Arbitrary nesting depth
    - Evaluation order

03_RefutablePatterns.md
  §7.10 Refutability
    - Refutable: Can fail to match
    - Irrefutable: Always matches

  §7.11 Refutable Pattern Contexts
    - match arms (refutable allowed)
    - if let (refutable allowed)
    - while let (refutable allowed)

  §7.12 Irrefutable Pattern Contexts
    - let bindings (irrefutable required)
    - function parameters (irrefutable required)

  §7.13 Exhaustiveness Checking
    - Compiler ensures all cases covered
    - Unreachable pattern warnings

04_PatternBindings.md
  §7.14 Binding Modes
    - By value (default)
    - By reference

  §7.15 Mutable Bindings in Patterns
    - mut keyword in patterns
    - Permission requirements

  §7.16 @ Bindings (if supported)
    - Binding entire value while destructuring
    - Syntax: pattern @ identifier
```

### Part VIII: Modules & Visibility

```markdown
Part VIII: Modules and Code Organization

01_FileBasedModules.md
  §8.1 File-Based Module System
    - One module per file
    - File name = module name
    - No module keyword needed

  §8.2 Directory Structure
    - Directories as module hierarchies
    - Root module

  §8.3 Module Naming Conventions
    - snake_case for file names
    - Corresponding module identifiers

02_ImportExport.md
  §8.4 Import Syntax
    - import path::to::module
    - import path::to::{item1, item2}

  §8.5 Qualified vs Unqualified Imports
    - Qualified: import std::io (use as std::io::function)
    - Unqualified: import std::io::{read, write}

  §8.6 Export (public keyword)
    - public: Exported from module
    - Default: Private to module

  §8.7 Re-exports
    - public import to re-export
    - Building module interfaces

03_Visibility.md
  §8.8 Public Items (public)
    - Visible outside module
    - Part of module's public API

  §8.9 Internal Items (package-level, if supported)
    - Visible within package, not outside

  §8.10 Private Items (default)
    - Only visible within module

  §8.11 Field Visibility
    - Record fields can have individual visibility
    - public field vs private field

  §8.12 Procedure Visibility
    - public procedure vs private procedure

04_ModuleResolution.md
  §8.13 Search Paths
    - Standard library location
    - User code paths
    - Dependencies

  §8.14 Name Resolution
    - Absolute paths (from root)
    - Relative paths
    - Ambiguity resolution

  §8.15 Circular Dependencies
    - If allowed: Resolution strategy
    - If prohibited: Compile-time error

05_PackageStructure.md
  §8.16 Package Definition
    - Package manifest file
    - Package metadata

  §8.17 Dependencies
    - Declaring dependencies
    - Version constraints

  §8.18 Build Configuration
    - Compilation units
    - Feature flags (if supported)
```

### Part IX: Metaprogramming

```markdown
Part IX: Compile-Time Programming

01_ComptimeOverview.md
  §9.1 comptime Philosophy
    - Compile-time code generation
    - No macro system (explicit design choice)

  §9.2 Compile-Time vs Runtime
    - Phase distinction semantics
    - What can be done at compile-time

  §9.3 Use Cases
    - Generic specialization
    - Type-based code generation
    - Compile-time validation

02_ComptimeExpressions.md
  §9.4 comptime Blocks
    - Syntax: comptime { ... }
    - Evaluation at compile time

  §9.5 comptime Variables
    - comptime let x = ...
    - Must be compile-time computable

  §9.6 Restrictions
    - No I/O at compile-time
    - No heap allocation at compile-time (unless const)
    - Pure computations only

03_TypeIntrospection.md
  §9.7 Type Information
    - Getting type information
    - Type equality checks

  §9.8 Field Reflection
    - Enumerating record fields
    - Field types and names

  §9.9 Opt-In Reflection
    - Explicit reflection attributes
    - Privacy-preserving reflection

04_CodeGeneration.md
  §9.10 Generic Specialization
    - Monomorphization at compile-time
    - Type-specific optimization

  §9.11 Conditional Compilation
    - comptime if (type condition)
    - Platform-specific code

  §9.12 Type-Based Code Generation
    - Generating implementations
    - Deriving functionality

05_ComptimeValidation.md
  §9.13 Static Assertions
    - comptime assert
    - Compile-time validation

  §9.14 Contract Checking at Compile Time
    - Static contract verification
    - Integration with Part IV
```

### Part X: Concurrency

```markdown
Part X: Concurrency

01_ConcurrencyModel.md
  §10.1 Structured Concurrency
    - Task hierarchy
    - Automatic cancellation propagation

  §10.2 Data Race Policy
    - No compile-time prevention (programmer responsibility)
    - Documentation of guarantees and non-guarantees

  §10.3 Send and Sync Contracts
    - Send: Safe to transfer across threads
    - Sync: Safe to share across threads
    - Auto-implementation rules

02_AsyncAwait.md
  §10.4 async Functions
    - async function syntax
    - Returns Future type

  §10.5 await Expressions
    - await operator
    - Suspension points

  §10.6 Future Type
    - Future contract
    - Polling semantics

  §10.7 Async Effect System Integration
    - async effects
    - Effect tracking in async contexts

03_Actors.md
  §10.8 Actor Declaration
    - actor keyword (if first-class)
    - Actor state isolation

  §10.9 Actor Isolation
    - No shared mutable state
    - Message-based communication

  §10.10 Message Passing
    - Sending messages to actors
    - Message types

  §10.11 Actor Lifecycle
    - Creation and destruction
    - Supervision (if supported)

04_MessagePassing.md
  §10.12 Channels
    - Channel creation
    - Send and receive operations

  §10.13 Channel Types
    - Unbounded vs bounded
    - MPSC, SPSC, MPMC

  §10.14 Select Expression
    - Selecting over multiple channels
    - Non-blocking operations

05_Synchronization.md
  §10.15 Mutex
    - Mutex<T> type
    - Lock acquisition
    - Automatic unlock on scope exit

  §10.16 Atomic Operations
    - Atomic types (AtomicI32, etc.)
    - Atomic operations
    - Memory ordering

  §10.17 Barriers and Other Primitives
```

### Part XI: FFI

```markdown
Part XI: Foreign Function Interface

01_FFIOverview.md
  §11.1 FFI Philosophy
    - Safe Cantrip, unsafe boundaries
    - C ABI compatibility

  §11.2 Safety Model
    - unsafe blocks at FFI boundaries
    - Invariant preservation

02_ForeignDeclarations.md
  §11.3 extern Declarations
    - Declaring foreign functions
    - Linkage specifications

  §11.4 Calling Conventions
    - C calling convention
    - Platform-specific conventions

  §11.5 Name Mangling
    - C name preservation
    - No mangling for extern "C"

03_TypeMappings.md
  §11.6 Primitive Type Mappings
    - Cantrip types to C types
    - Size and alignment guarantees

  §11.7 Struct Representation
    - #[repr(C)] attribute
    - Layout guarantees

  §11.8 Pointer Types
    - Raw pointers (*T) at FFI boundary
    - Null pointer handling

04_OwnershipAcrossFFI.md
  §11.9 Passing Ownership to C
    - Transferring Cantrip objects
    - Manual memory management

  §11.10 Receiving Ownership from C
    - Taking ownership of C-allocated memory
    - Deallocation responsibilities

  §11.11 Passing Permissions Across FFI
    - Passing Cantrip values to C
    - Permission preservation at boundaries

05_Callbacks.md
  §11.12 C Calling Cantrip Functions
    - Function pointer conversions
    - Callback registration

  §11.13 Closure Conversion
    - Converting closures to function pointers
    - Capture limitations

06_SafetyBoundaries.md
  §11.14 unsafe Blocks
    - Marking FFI code as unsafe
    - Safety invariants to maintain

  §11.15 Safety Wrappers
    - Building safe APIs around unsafe FFI
    - Encapsulation patterns
```

### Part XII: Standard Library

```markdown
Part XII: Standard Library

01_CoreTypes.md
  §12.1 Option<T> (Reference Part II)
  §12.2 Result<T, E> (Reference Part II)
  §12.3 String and str
  §12.4 Vec<T>
  §12.5 Box<T> (if heap allocation supported)

02_Collections.md
  §12.6 HashMap<K, V>
  §12.7 HashSet<T>
  §12.8 BTreeMap<K, V>
  §12.9 BTreeSet<T>
  §12.10 LinkedList<T> (if included)

03_IOAndFileSystem.md
  §12.11 File I/O
  §12.12 Standard Streams (stdin, stdout, stderr)
  §12.13 Buffered I/O
  §12.14 File System Operations

04_Networking.md
  §12.15 TCP Sockets
  §12.16 UDP Sockets
  §12.17 HTTP Client (if included)

05_ConcurrencyPrimitives.md
  §12.18 Mutex<T>
  §12.19 RwLock<T>
  §12.20 Channels
  §12.21 Atomic Types
```

### Part XIII: Formal Semantics

```markdown
Part XIII: Formal Semantics and Soundness

01_SmallStepSemantics.md
  §13.1 Small-Step Reduction Rules
  §13.2 Evaluation Contexts
  §13.3 Reduction Relation (→)

02_BigStepSemantics.md
  §13.4 Big-Step Evaluation Judgments
  §13.5 Value Judgments
  §13.6 Divergence

03_MemoryModel.md
  §13.7 Memory State Representation
  §13.8 Heap Model
  §13.9 Region Model
  §13.10 Happens-Before Relation

04_TypeSoundness.md
  §13.11 Progress Theorem
  §13.12 Preservation Theorem
  §13.13 Type Safety Proof

05_EffectSoundness.md
  §13.14 Effect Soundness Theorem
  §13.15 Effect System Proofs

06_MemorySafety.md
  §13.16 Use-After-Free Prevention Proof
  §13.17 Double-Free Prevention Proof
  §13.18 Memory Leak Prevention Proof

07_ModalSafety.md
  §13.19 State Transition Validity Proof
  §13.20 Modal Invariant Preservation
```

### Part XIV: Tooling & Implementation

```markdown
Part XIV: Tooling and Implementation (Non-Normative)

01_CompilerArchitecture.md
  §14.1 Compilation Pipeline
  §14.2 Optimization Levels
  §14.3 Incremental Compilation

02_ErrorReporting.md
  §14.4 Error Message Format
  §14.5 Error Codes
  §14.6 Diagnostic Quality Guidelines

03_PackageManagement.md
  §14.7 Package Manifest Format
  §14.8 Dependency Resolution
  §14.9 Lock Files

04_TestingFramework.md
  §14.10 Unit Test Framework
  §14.11 Integration Tests
  §14.12 Property-Based Testing
  §14.13 Benchmarking
```

---

## 4. Section Template Standards

### 4.1 Universal Section Template

Every specification section MUST follow this template:

```markdown
# Part X: [Part Name] - §Y. [Section Title]

**Section**: §Y | **Part**: [Part Name] (Part X)
**Previous**: [Link to previous section] | **Next**: [Link to next section]

---

**Definition Y.1 ([Concept Name]):** [Formal mathematical definition using set theory,
type theory, or operational semantics notation. Should be precise and unambiguous.]

## Y. [Section Title]

### Y.1 Overview

**Key innovation/purpose:** [What problem does this feature solve? What makes it unique?]

**When to use [feature]:**
- [Use case 1]
- [Use case 2]
- [Use case 3]
- [Specific scenarios where this feature is the right choice]

**When NOT to use [feature]:**
- [Anti-pattern 1] → use [alternative] instead
- [Anti-pattern 2] → use [alternative] instead
- [Scenarios where another approach is better]

**Relationship to other features:**
- **[Related Feature 1] (§X.Y):** [How they interact]
- **[Related Feature 2] (§X.Y):** [How they interact]
- **[Related Feature 3] (§X.Y):** [How they interact]

### Y.2 Syntax

#### Y.2.1 Concrete Syntax

**EBNF Grammar:**
```ebnf
[Production rules in Extended Backus-Naur Form]
```

#### Y.2.2 Abstract Syntax

**Formal syntax notation:**
```
[Abstract syntax using mathematical notation]
```

#### Y.2.3 Basic Examples

**Example Y.2.1: [Descriptive Title]**
```cantrip
// Simple introductory example
[code]
```

**Example Y.2.2: [Descriptive Title]**
```cantrip
// Slightly more complex example
[code]
```

### Y.3 Static Semantics

#### Y.3.1 Well-Formedness Rules

Rules defining syntactically valid programs:

```
[WF-RuleName]
Premise₁
Premise₂
─────────────
Conclusion
```

#### Y.3.2 Type Rules

Type system inference rules:

```
[Type-RuleName]
Γ ⊢ e₁ : τ₁    Γ ⊢ e₂ : τ₂
────────────────────────────
Γ ⊢ e : τ
```

#### Y.3.3 Type Properties

**Theorem Y.3.1 ([Property Name]):** [Statement of theorem]

**Proof sketch:** [High-level proof outline or reference to formal proof in Part XIII]

**Corollary Y.3.1:** [Consequence of theorem]

### Y.4 Dynamic Semantics

#### Y.4.1 Evaluation Rules

Operational semantics:

```
[Eval-RuleName]
e₁ → e₁'
─────────────────
op(e₁, e₂) → op(e₁', e₂)
```

#### Y.4.2 Memory Representation

**Memory Layout:**
```
[Visual diagram or description of memory layout]

Field Layout:
- Field 1: offset 0, size X bytes
- Field 2: offset X, size Y bytes
Total size: Z bytes
Alignment: A bytes
```

**Table Y.4.1: [Type] Memory Characteristics**
| Property | Value | Notes |
|----------|-------|-------|
| Size | X bytes | [Explanation] |
| Alignment | Y bytes | [Explanation] |
| Layout | [description] | [Explanation] |

#### Y.4.3 Operational Behavior

Runtime behavior description:
- Execution model
- Performance characteristics
- Edge cases

### Y.5 Examples and Patterns

#### Y.5.1 [Common Pattern 1 Name]

**Example Y.5.1: [Descriptive Title]**
```cantrip
// Practical real-world example
[code]
```

**Output:**
```
[Expected output]
```

**Explanation:** [Why this pattern works, what it demonstrates]

#### Y.5.2 [Common Pattern 2 Name]

[Similar structure to Y.5.1]

#### Y.5.3 Common Pitfalls

**Warning Y.5.1:** [Description of common mistake]

**Example Y.5.X: Incorrect Usage**
```cantrip
// Example of what NOT to do
[bad code]
```

**Error:** `E[error-code]: [error message]`

**Correct approach:**
```cantrip
// Corrected version
[good code]
```

### Y.6 Related Sections

- See §X.Y for [related topic]
- Compare with §X.Z for [alternative approach]
- Prerequisites: §A.B, §C.D

### Y.7 Notes and Warnings

**Note Y.7.1:** [Informational note about implementation detail, historical context, or clarification]

**Note Y.7.2:** [Another informational note]

**Performance Note Y.7.1:** [Note about performance characteristics or optimization opportunities]

---

**Previous**: [Link] | **Next**: [Link]
```

### 4.2 Required Elements

Every section MUST include:
1. ✅ Definition box (formal definition)
2. ✅ Overview (purpose, when to use, relationships)
3. ✅ Syntax (concrete EBNF + abstract + examples)
4. ✅ Static semantics (type rules, theorems)
5. ✅ Dynamic semantics (evaluation, memory, behavior)
6. ✅ Examples (at least 3 numbered examples)
7. ✅ Navigation (Previous/Next links)

Every section SHOULD include:
- Common patterns subsection
- Common pitfalls/anti-patterns
- Performance notes (where relevant)
- Cross-references to related sections

---

## 5. Formatting and Style Guidelines

### 5.1 Numbering Conventions

#### Section Numbering
- Parts: Roman numerals (Part I, Part II, ...)
- Sections within parts: §X.Y (§3.5 = Part 3, Section 5)
- Subsections: §X.Y.Z (§3.5.2 = Part 3, Section 5, Subsection 2)

#### Element Numbering
All numbered elements follow `Section.Subsection.Number` format:

**Definitions:**
```markdown
**Definition 3.5.1 ([Concept Name]):** [Definition text]
**Definition 3.5.2 ([Concept Name]):** [Definition text]
```

**Theorems:**
```markdown
**Theorem 3.5.1 ([Property Name]):** [Theorem statement]
**Corollary 3.5.1:** [Corollary statement]
**Lemma 3.5.1:** [Lemma statement]
```

**Examples:**
```markdown
**Example 3.5.1: [Descriptive Title]**
```cantrip
[code]
```

**Example 3.5.2: [Descriptive Title]**
```cantrip
[code]
```
```

**Tables:**
```markdown
**Table 3.5.1: [Descriptive Caption]**
| Column 1 | Column 2 | Column 3 |
|----------|----------|----------|
| ...      | ...      | ...      |
```

**Figures:**
```markdown
**Figure 3.5.1: [Descriptive Caption]**
```
[Diagram or illustration]
```
```

**Notes:**
```markdown
**Note 3.5.1:** [Note text]
**Warning 3.5.1:** [Warning text]
**Performance Note 3.5.1:** [Performance note text]
```

### 5.2 Grammar Notation

Use Extended Backus-Naur Form (EBNF) consistently:

```ebnf
NonTerminal ::= Production
              | AlternativeProduction
              | "terminal" NonTerminal  // Terminals in quotes
              | NonTerminal*             // Zero or more
              | NonTerminal+             // One or more
              | NonTerminal?             // Optional
              | (Grouped | Productions)  // Grouping
```

**Conventions:**
- Non-terminals: PascalCase (e.g., `Expression`, `TypeName`)
- Terminals: Double quotes (e.g., `"function"`, `"+"`)
- Optional: `?` suffix
- Repetition: `*` (zero or more), `+` (one or more)
- Grouping: Parentheses `( )`
- Alternatives: `|` (vertical bar)

### 5.3 Formal Notation

#### Type Judgments
```
Γ ⊢ e : τ           "Under context Γ, expression e has type τ"
Γ ⊢ e₁ : τ₁ ⇒ τ₂    "Under context Γ, e₁ has function type τ₁ → τ₂"
```

#### Inference Rules
```
[Rule-Name]
Premise₁    Premise₂    Premise₃
─────────────────────────────────
Conclusion
```

#### Evaluation Rules
```
e → e'              "Expression e reduces to e'"
e ⇓ v               "Expression e evaluates to value v"
```

#### Type Equality and Subtyping
```
τ₁ = τ₂             "Types τ₁ and τ₂ are equal"
τ₁ <: τ₂            "Type τ₁ is a subtype of τ₂"
```

### 5.4 Code Examples

All code examples MUST:
1. Use ```cantrip fenced code blocks
2. Include descriptive titles (`Example X.Y.N: Title`)
3. Be syntactically correct and type-checkable
4. Include comments explaining key points
5. Show expected output where relevant

**Format:**
```markdown
**Example 3.5.1: Integer Arithmetic**
```cantrip
// Calculate factorial recursively
function factorial(n: i32): i32 {
    if n <= 1 {
        1
    } else {
        n * factorial(n - 1)
    }
}

let result = factorial(5)  // result = 120
```

**Output:**
```
120
```
```

### 5.5 Cross-References

**Within specification:**
```markdown
See §3.5 for details
As defined in §2.1
Compare with (§4.2)
[Section Name](link.md) (§3.5)
```

**To other parts:**
```markdown
Part II: Type System (§5-12)
Part IV: Contracts (§17-20)
```

**External references:**
```markdown
[IEEE754-2008]
[RFC-2119]
```

### 5.6 Notes and Warnings

**Informational Notes:**
```markdown
**Note 3.5.1:** This behavior is implementation-defined.
```

**Critical Warnings:**
```markdown
**Warning 3.5.1:** This operation is unsafe and requires an unsafe block.
```

**Performance Notes:**
```markdown
**Performance Note 3.5.1:** This operation is O(1) with region allocation.
```

### 5.7 Tables

All tables MUST have:
1. Numbered caption: `**Table X.Y.N: [Caption]**`
2. Header row
3. Alignment appropriate to content
4. Explanatory text if needed

**Example:**
```markdown
**Table 3.5.1: Integer Type Ranges**
| Type  | Size    | Minimum         | Maximum        |
|-------|---------|-----------------|----------------|
| i8    | 1 byte  | -128            | 127            |
| i16   | 2 bytes | -32,768         | 32,767         |
| i32   | 4 bytes | -2,147,483,648  | 2,147,483,647  |
```

---

## 6. Implementation Roadmap

### 6.1 Phase-Based Approach

#### Phase 1: Critical Foundation (Months 1-3)
**Priority:** CRITICAL - Cannot implement language without these

**Parts to complete:**
1. Part III: Expressions & Statements (12 files)
2. Part V: Permissions & Memory (7 files)
3. Part VI: Functions & Procedures (6 files)

**Total:** ~25 files, ~50,000 lines estimated

**Dependencies:** All depend on Part II (complete)

**Deliverables:**
- Complete syntax for all expressions
- Complete permission system specification
- Complete function/procedure semantics
- Can write basic Cantrip programs

**Success Criteria:**
- All basic language constructs specified
- Type system + expressions + functions = executable semantics
- Can write non-trivial examples

#### Phase 2: Core Language Features (Months 4-6)
**Priority:** HIGH - Essential for practical use

**Parts to complete:**
4. Part VII: Pattern Matching (4 files)
5. Part VIII: Modules & Visibility (5 files)
6. Part XII: Standard Library - Core (5 files, basic collections only)

**Total:** ~14 files, ~30,000 lines estimated

**Dependencies:** Depend on Phase 1 completion

**Deliverables:**
- Complete pattern matching specification
- Module system fully specified
- Basic standard library (Vec, HashMap, Option, Result, String)

**Success Criteria:**
- Can organize code into modules
- Pattern matching fully specified
- Basic programs can use standard collections

#### Phase 3: Advanced Features (Months 7-9)
**Priority:** MEDIUM - Important but not blocking

**Parts to complete:**
7. Part IX: Metaprogramming (5 files)
8. Part X: Concurrency (5 files)
9. Part XI: FFI (6 files)

**Total:** ~16 files, ~35,000 lines estimated

**Dependencies:** Depend on Phase 1-2 completion

**Deliverables:**
- comptime system fully specified
- Concurrency model complete
- FFI with C fully specified

**Success Criteria:**
- Can do compile-time code generation
- Can write concurrent programs
- Can call C code safely

#### Phase 4: Formalization & Tooling (Months 10-12)
**Priority:** LOW - Important for completeness, non-blocking

**Parts to complete:**
10. Part XII: Standard Library - Complete (10 more files)
11. Part XIII: Formal Semantics (7 files)
12. Part XIV: Tooling (4 files)
13. Appendices completion

**Total:** ~21 files, ~45,000 lines estimated

**Dependencies:** Depend on all previous phases

**Deliverables:**
- Full standard library specification
- Complete formal soundness proofs
- Tooling and implementation guidelines

**Success Criteria:**
- Complete standard library
- Type soundness, effect soundness, memory safety proved
- Implementation guidelines for conforming compilers

### 6.2 Parallel Work Streams

Multiple parts can be written in parallel within each phase:

**Phase 1 Parallel Streams:**
- Stream A: Part III (Expressions)
- Stream B: Part V (Permissions)
- Stream C: Part VI (Functions)

**Phase 2 Parallel Streams:**
- Stream A: Part VII (Pattern Matching)
- Stream B: Part VIII (Modules)
- Stream C: Part XII Core (Stdlib basics)

**Phase 3 Parallel Streams:**
- Stream A: Part IX (Metaprogramming)
- Stream B: Part X (Concurrency)
- Stream C: Part XI (FFI)

**Phase 4 Parallel Streams:**
- Stream A: Part XII Complete (Stdlib)
- Stream B: Part XIII (Formal Semantics)
- Stream C: Part XIV (Tooling)

### 6.3 Estimated Effort

**Total Specification Size:**
- Current: 35 files, ~70,000 lines
- Remaining: ~150 files, ~160,000 lines estimated
- Total: ~185 files, ~230,000 lines

**Breakdown by Part:**
- Part III: 12 files × ~2,500 lines = ~30,000 lines
- Part V: 7 files × ~3,000 lines = ~21,000 lines
- Part VI: 6 files × ~2,500 lines = ~15,000 lines
- Part VII: 4 files × ~2,000 lines = ~8,000 lines
- Part VIII: 5 files × ~2,000 lines = ~10,000 lines
- Part IX: 5 files × ~2,500 lines = ~12,500 lines
- Part X: 5 files × ~3,000 lines = ~15,000 lines
- Part XI: 6 files × ~2,500 lines = ~15,000 lines
- Part XII: 15 files × ~2,000 lines = ~30,000 lines
- Part XIII: 7 files × ~4,000 lines = ~28,000 lines (formal proofs)
- Part XIV: 4 files × ~2,000 lines = ~8,000 lines

**Writing Rate Estimates:**
- Experienced specification writer: ~500-750 lines/day
- With review and iteration: ~300-400 lines/day effective
- Phase 1 (50,000 lines): 125-165 person-days (~6 months with 1 writer)
- Total (160,000 lines): 400-530 person-days (~2 years with 1 writer)

**With parallel work (3 writers):**
- Phase 1: 2-3 months
- Phase 2: 2-3 months
- Phase 3: 2-3 months
- Phase 4: 3-4 months
- **Total: 9-13 months** (with 3 experienced writers)

### 6.4 Review and Quality Gates

Each phase must pass quality review before next phase:

**Quality Gates:**
1. ✅ All sections follow template
2. ✅ All examples are syntactically correct
3. ✅ All cross-references are valid
4. ✅ Formal definitions are mathematically sound
5. ✅ Static semantics rules are complete
6. ✅ Dynamic semantics rules are complete
7. ✅ No "[TO BE SPECIFIED]" markers remain
8. ✅ Peer review by 2+ reviewers
9. ✅ Technical review by implementer
10. ✅ Consistency check across all parts

**Review Process:**
1. Author completes draft
2. Self-review against checklist
3. Peer review (2 reviewers)
4. Technical review (implementer perspective)
5. Revision based on feedback
6. Final approval

---

## 7. Quality Assurance Criteria

### 7.1 Completeness Checklist

Every specification section MUST satisfy:

#### Syntax Completeness
- [ ] EBNF grammar covers all constructs
- [ ] Abstract syntax is defined
- [ ] Grammar is unambiguous
- [ ] Examples demonstrate all syntax forms

#### Static Semantics Completeness
- [ ] Well-formedness rules cover all constructs
- [ ] Type rules cover all expressions
- [ ] Type properties are stated as theorems
- [ ] Edge cases are addressed

#### Dynamic Semantics Completeness
- [ ] Evaluation rules cover all expressions
- [ ] Memory representation is specified
- [ ] Operational behavior is described
- [ ] Runtime errors are specified

#### Example Completeness
- [ ] At least 3 basic examples
- [ ] At least 2 real-world pattern examples
- [ ] Anti-patterns / pitfalls documented
- [ ] Edge cases demonstrated

#### Cross-Reference Completeness
- [ ] Related sections referenced
- [ ] Dependencies stated
- [ ] Forward references to later sections
- [ ] Backward references to prerequisites

### 7.2 Consistency Checks

#### Internal Consistency
- [ ] Terminology used consistently
- [ ] Notation used consistently
- [ ] Numbering follows scheme
- [ ] Cross-references are valid

#### Cross-Section Consistency
- [ ] Type rules consistent across parts
- [ ] Examples don't contradict other sections
- [ ] Formal definitions align with usage
- [ ] No conflicting semantics

#### Cross-Part Consistency
- [ ] Permission system consistent in all parts
- [ ] Effect system consistent in all parts
- [ ] Type system assumptions consistent
- [ ] Memory model consistent

### 7.3 Formal Rigor Standards

#### Mathematical Definitions
- [ ] Precise notation
- [ ] Well-defined semantics
- [ ] No ambiguity
- [ ] References to formal foundations (Part I)

#### Inference Rules
- [ ] Correct rule format
- [ ] All premises stated
- [ ] Conclusion follows from premises
- [ ] Rule names descriptive

#### Theorems and Proofs
- [ ] Theorem statements are precise
- [ ] Proof sketches provided
- [ ] Full proofs in Part XIII (or referenced)
- [ ] Corollaries properly derived

### 7.4 Readability Standards

#### Clarity
- [ ] Jargon explained on first use
- [ ] Complex concepts introduced gradually
- [ ] Diagrams where helpful
- [ ] Examples before formalism

#### Organization
- [ ] Logical section flow
- [ ] Clear headings
- [ ] Table of contents accurate
- [ ] Navigation links working

#### Code Quality
- [ ] Examples are idiomatic
- [ ] Code is formatted consistently
- [ ] Comments explain non-obvious code
- [ ] Output shown where relevant

### 7.5 Conformance Requirements

Every specification section MUST clearly distinguish:

**Normative Requirements:**
- Use RFC 2119 keywords: MUST, MUST NOT, REQUIRED, SHALL, SHALL NOT, SHOULD, SHOULD NOT, RECOMMENDED, MAY, OPTIONAL
- Clearly state what conforming implementations must do
- Specify error conditions

**Non-Normative Content:**
- Mark as "Non-normative" or "Informative"
- Examples, notes, diagrams (unless explicitly normative)
- Implementation suggestions
- Historical context

**Implementation-Defined Behavior:**
- Clearly marked as "implementation-defined"
- Document what aspects are implementation choices
- Examples: optimization strategies, error message format

**Unspecified Behavior:**
- Clearly marked as "unspecified"
- Document what language does not guarantee
- Examples: evaluation order where not specified

---

## 8. Appendices

### Appendix A: File Naming Conventions

```
[PartNumber]_[PartName]/[SectionNumber]_[SectionName].md

Examples:
02_Types/01a_Integers.md
02_Types/01b_FloatingPoint.md
03_Expressions/01_Literals.md
05_Memory/01_PermissionSystem.md
```

**Rules:**
- Part directories: Two-digit number + underscore + PascalCase name
- Section files: Two-digit number (or letter) + underscore + PascalCase name
- Use .md extension
- No spaces in filenames

### Appendix B: Git Repository Structure

```
cantrip-spec/
├── Spec/
│   ├── 01_FrontMatter/
│   ├── 02_Types/
│   ├── 03_Expressions/
│   ├── 04_Contracts/
│   ├── 05_Memory/
│   ├── 06_Functions/
│   ├── 07_PatternMatching/
│   ├── 08_Modules/
│   ├── 09_Metaprogramming/
│   ├── 10_Concurrency/
│   ├── 11_FFI/
│   ├── 12_StandardLibrary/
│   ├── 13_FormalSemantics/
│   ├── 14_Tooling/
│   └── 15_Appendices/
├── Examples/
│   └── [code examples referenced in spec]
├── Templates/
│   ├── section_template.md
│   └── checklist.md
├── Tools/
│   ├── cross-reference-checker.py
│   ├── example-validator.py
│   └── consistency-checker.py
├── CONTRIBUTING.md
├── README.md
└── SPECIFICATION_ORGANIZATION_PROPOSAL.md (this document)
```

### Appendix C: Section Template File

See §4.1 for the complete section template.

Template file available at: `Templates/section_template.md`

### Appendix D: Example Checklist

Before submitting a specification section:

**Syntax:**
- [ ] EBNF grammar provided
- [ ] Abstract syntax provided
- [ ] Basic examples (3+)

**Semantics:**
- [ ] Static semantics (type rules)
- [ ] Dynamic semantics (evaluation rules)
- [ ] Memory representation

**Documentation:**
- [ ] Overview with motivation
- [ ] When to use / not use
- [ ] Relationships documented
- [ ] Cross-references added

**Quality:**
- [ ] All examples tested
- [ ] No "[TO BE SPECIFIED]"
- [ ] Peer reviewed
- [ ] Consistency checked

### Appendix E: Glossary of Notation

| Symbol | Meaning | Example |
|--------|---------|---------|
| τ, σ | Type | τ₁ → τ₂ |
| e | Expression | e₁ + e₂ |
| v | Value | v = 42 |
| Γ | Type context | Γ ⊢ e : τ |
| ⊢ | Entails / proves | Γ ⊢ e : τ |
| : | Has type | x : i32 |
| → | Function type | τ₁ → τ₂ |
| ⇒ | Evaluates to | e ⇒ v |
| ⇓ | Big-step evaluation | e ⇓ v |
| <: | Subtype | τ₁ <: τ₂ |
| ≡ | Equivalent | e₁ ≡ e₂ |

---

## 9. References

### 9.1 Language Specifications Analyzed

1. **Java Language Specification (JLS)**
   - Edition: Java SE 17
   - URL: https://docs.oracle.com/javase/specs/
   - Key insights: Formal numbering, exhaustive coverage, graduated complexity

2. **C++ Standard (ISO/IEC 14882)**
   - Edition: C++20
   - URL: https://isocpp.org/std/the-standard
   - Key insights: Numbered paragraphs, labeled notes/examples, formal rigor

3. **Go Language Specification**
   - URL: https://go.dev/ref/spec
   - Key insights: Concise, grammar-first, practical focus

4. **Rust Reference**
   - URL: https://doc.rust-lang.org/reference/
   - Key insights: Markdown format, practical examples, ownership focus

5. **Swift Language Reference**
   - URL: https://docs.swift.org/swift-book/
   - Key insights: User-friendly, code-heavy, less formal

### 9.2 Specification Writing Standards

1. **RFC 2119**: Key words for use in RFCs to Indicate Requirement Levels
2. **RFC 8174**: Ambiguity of Uppercase vs Lowercase in RFC 2119 Key Words
3. **ISO/IEC Directives, Part 2**: Principles and rules for the structure and drafting of ISO and IEC documents

---

## 10. Conclusion

This proposal provides a complete roadmap for organizing and completing the Cantrip Language Specification. Key takeaways:

1. **Type system is complete** - No gaps identified
2. **Critical path**: Expressions → Permissions → Functions → Everything else
3. **Estimated effort**: 9-13 months with 3 experienced specification writers
4. **Quality standards**: Comprehensive checklists and review processes
5. **Based on proven patterns**: Java JLS, Rust Reference, Go Spec, C++ Standard

**Next Steps:**
1. Approve organization structure
2. Assign writers to Phase 1 parts
3. Set up quality review process
4. Begin writing Part III (Expressions)

**Success Metrics:**
- All sections follow template
- No "[TO BE SPECIFIED]" markers
- All examples validate
- Peer review approval
- Technical review approval

---

**Document Version History:**
- v1.0 (2025-10-26): Initial proposal based on cross-language specification analysis

**Approval:**
- [ ] Language Designer
- [ ] Specification Lead
- [ ] Technical Reviewer

**Status:** DRAFT - Pending Approval
