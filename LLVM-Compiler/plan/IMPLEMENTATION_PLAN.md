# Cursive Language Compiler - Comprehensive Implementation Plan

**Version**: 1.0.0  
**Target**: Full Cursive Language Specification v1.0 Compliance  
**Technology Stack**: C++17/20, LLVM 17+, CMake 3.20+

---

## Executive Summary

This document provides a complete implementation plan for building a production-ready C++ compiler for the Cursive programming language using LLVM as the backend. The plan covers all language features specified in the Cursive Language Specification (16 clauses + 5 annexes), organized into achievable phases with clear milestones, testing requirements, and risk mitigation strategies.

**Key Metrics**:

- **Estimated Implementation Time**: 24-36 months (full-time team)
- **Team Size**: 4-6 engineers (minimum)
- **Lines of Code Estimate**: 150,000-200,000 LOC (C++)
- **Specification Coverage**: 100% (all clauses 1-16)
- **Testing Target**: 95%+ specification compliance

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Phase-by-Phase Roadmap](#2-phase-by-phase-roadmap)
3. [Component Breakdown](#3-component-breakdown)
4. [Testing Strategy](#4-testing-strategy)
5. [Milestones and Deliverables](#5-milestones-and-deliverables)
6. [Risk Assessment and Mitigation](#6-risk-assessment-and-mitigation)
7. [Resource Requirements](#7-resource-requirements)
8. [Conformance Validation](#8-conformance-validation)

---

## 1. Architecture Overview

### 1.1 High-Level Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                     Cursive Compiler                          │
├──────────────────────────────────────────────────────────────┤
│  Source Files (.cursive)                                      │
│         ↓                                                     │
│  ┌────────────────────────────────────────┐                  │
│  │  Frontend (cursivec-front)             │                  │
│  │  ├─ Lexer & Tokenizer                  │                  │
│  │  ├─ Parser (ANTLR-generated + custom)  │                  │
│  │  ├─ AST Construction                   │                  │
│  │  └─ Source Location Tracking           │                  │
│  └────────────────────────────────────────┘                  │
│         ↓                                                     │
│  ┌────────────────────────────────────────┐                  │
│  │  Semantic Analysis (cursivec-sema)     │                  │
│  │  ├─ Module Resolution & Scope          │                  │
│  │  ├─ Name Binding & Lookup              │                  │
│  │  ├─ Type Checking & Inference          │                  │
│  │  ├─ Permission Analysis                │                  │
│  │  ├─ Grant Checking                     │                  │
│  │  ├─ Contract Verification              │                  │
│  │  ├─ Region Escape Analysis             │                  │
│  │  ├─ Move & Definite Assignment         │                  │
│  │  └─ Monomorphization                   │                  │
│  └────────────────────────────────────────┘                  │
│         ↓                                                     │
│  ┌────────────────────────────────────────┐                  │
│  │  Compile-Time Evaluation (cursivec-ct) │                  │
│  │  ├─ Comptime Procedure Executor        │                  │
│  │  ├─ Reflection System                  │                  │
│  │  ├─ Code Generation API                │                  │
│  │  └─ Quote & Interpolation Handler      │                  │
│  └────────────────────────────────────────┘                  │
│         ↓                                                     │
│  ┌────────────────────────────────────────┐                  │
│  │  IR Generation (cursivec-irgen)        │                  │
│  │  ├─ LLVM IR Builder                    │                  │
│  │  ├─ Witness VTable Generator           │                  │
│  │  ├─ RAII Cleanup Insertion             │                  │
│  │  └─ Optimization Pipeline Setup        │                  │
│  └────────────────────────────────────────┘                  │
│         ↓                                                     │
│  ┌────────────────────────────────────────┐                  │
│  │  LLVM Backend                          │                  │
│  │  ├─ Target-Specific Codegen            │                  │
│  │  ├─ Object File Generation             │                  │
│  │  └─ Linking                            │                  │
│  └────────────────────────────────────────┘                  │
│         ↓                                                     │
│  Executable / Library                                         │
└──────────────────────────────────────────────────────────────┘

Runtime Library (libcursive_runtime)
├─ Arena allocator implementation
├─ String (modal type) implementation
├─ Thread, Mutex, Channel implementations
├─ Panic/unwinding support
└─ Standard behaviors (Display, Copy, Clone, etc.)
```

### 1.2 Project Structure

```
cursive-compiler/
├── include/
│   ├── cursive/
│   │   ├── AST/              # AST node definitions
│   │   ├── Basic/            # Source locations, diagnostics
│   │   ├── Parse/            # Parser interface
│   │   ├── Sema/             # Semantic analysis
│   │   ├── CodeGen/          # IR generation
│   │   ├── CompTime/         # Comptime evaluation
│   │   └── Support/          # Utilities
├── lib/
│   ├── Lexer/                # Tokenization
│   ├── Parse/                # Parsing
│   ├── AST/                  # AST implementation
│   ├── Sema/                 # Semantic analysis modules
│   ├── CodeGen/              # LLVM IR generation
│   ├── CompTime/             # Comptime evaluation
│   └── Driver/               # Compiler driver
├── tools/
│   ├── cursivec/             # Main compiler executable
│   ├── cursive-fmt/          # Code formatter
│   ├── cursive-doc/          # Documentation generator
│   └── cursive-lsp/          # Language server
├── runtime/
│   ├── src/                  # Runtime library source
│   │   ├── arena.cpp         # Arena allocator
│   │   ├── string.cpp        # String implementation
│   │   ├── thread.cpp        # Threading primitives
│   │   ├── panic.cpp         # Panic handling
│   │   └── witness.cpp       # Witness vtable support
│   └── include/
│       └── cursive_runtime.h
├── test/
│   ├── spec-conformance/     # Specification compliance tests
│   ├── unit/                 # Unit tests for components
│   ├── integration/          # End-to-end tests
│   └── regression/           # Regression test suite
└── docs/
    ├── implementation/       # Implementation notes
    ├── abi/                  # ABI documentation
    └── diagnostics/          # Diagnostic catalog
```

---

## 2. Phase-by-Phase Roadmap

### Phase 1: Foundation & Lexical Analysis (Months 1-3)

**Objective**: Establish project foundation and implement complete lexical analysis per Clause 2.

#### 2.1.1 Project Setup (Week 1-2)

- [ ] CMake build system configuration
- [ ] LLVM integration (find_package, link libraries)
- [ ] Project directory structure
- [ ] Git repository with submodules
- [ ] CI/CD pipeline (GitHub Actions / GitLab CI)
- [ ] Code formatting (clang-format) and linting
- [ ] Documentation infrastructure (Doxygen)

#### 2.1.2 Basic Infrastructure (Week 3-4)

- [ ] Source location tracking (`SourceLocation`, `SourceRange`)
- [ ] Diagnostic system foundation
  - Diagnostic engine with severity levels
  - Diagnostic code registry (E02-xxx through E16-xxx)
  - JSON output format (Annex E §E.5.6)
  - Human-readable formatting with color
- [ ] File I/O and UTF-8 validation (§2.1)
- [ ] Memory management utilities (arena allocators for compiler)

#### 2.1.3 Lexer Implementation (Week 5-8)

- [ ] Token definitions (all keywords from §2.3.3[4])
- [ ] UTF-8 scanner with BOM handling (§2.1.3)
- [ ] Line ending normalization (LF/CR/CRLF → LF)
- [ ] Numeric literal lexing (decimal, hex, octal, binary with separators)
- [ ] String and character literal lexing with escape sequences
- [ ] Comment handling (line, block, doc, module doc)
- [ ] Identifier recognition (Unicode XID_Start/XID_Continue)
- [ ] Keyword reservation checking
- [ ] Statement termination and continuation rules (§2.4)
  - Newline significance
  - Semicolon handling
  - Continuation predicates (unclosed delimiters, trailing operators, leading dot/pipeline)
- [ ] Maximal munch for operators (`<<=`, `..=`, etc.)
- [ ] Documentation comment attachment
- [ ] **Diagnostic Codes**: E02-001 through E02-401
- [ ] **Tests**: 500+ lexer unit tests covering all token types

#### 2.1.4 Phase 1 Deliverables

- ✅ Working lexer producing token streams
- ✅ Diagnostic system with E02-xxx codes
- ✅ 95%+ coverage of §2.1-§2.4
- ✅ Test suite: lexer edge cases, Unicode, diagnostics

---

### Phase 2: Parsing & AST Construction (Months 4-7)

**Objective**: Implement complete parser per Annex A grammar and build typed AST per Clauses 3-6.

#### 2.2.1 Parser Infrastructure (Week 1-3)

- [ ] ANTLR4 grammar from Annex A (sections A.1-A.9)
- [ ] Handwritten recursive descent parser (or ANTLR-generated + custom actions)
- [ ] AST node class hierarchy
  - Base `Node` class with source locations
  - Declaration nodes (Clause 5)
  - Statement nodes (Clause 9)
  - Expression nodes (Clause 8)
  - Type nodes (Clause 7)
  - Pattern nodes (§8.5)
- [ ] Parse tree → AST transformation
- [ ] Error recovery for better diagnostics

#### 2.2.2 Declaration Parsing (Week 4-6)

- [ ] Module structure (§4.1, §4.2)
  - `import` and `use` declarations
  - Module path derivation from filesystem
  - Manifest (`Cursive.toml`) parsing
- [ ] Variable bindings (§5.2)
  - `let`/`var` with `=` and `<-` operators
  - Pattern destructuring (§5.3)
  - `shadow` keyword handling
- [ ] Procedure declarations (§5.4)
  - Parameter lists with `move` modifier
  - Receiver shorthand (`~`, `~%`, `~!`)
  - Contractual sequents (§12.2)
  - Expression-bodied procedures
- [ ] Type declarations (§5.5)
  - Records with fields and methods
  - Tuple records
  - Enums (unit, tuple, record variants)
  - Modal types with state blocks
  - Type aliases
- [ ] Contract declarations (§10.4, Clause 12)
- [ ] Behavior declarations (§10.4)
- [ ] Grant declarations (§5.9)

#### 2.2.3 Type Expression Parsing (Week 7-8)

- [ ] Primitive types
- [ ] Array and slice types
- [ ] Tuple types
- [ ] Pointer types (raw and safe modal)
- [ ] Function types with grants and sequents
- [ ] Modal type annotations (`T@State`)
- [ ] Witness types (`witness<B>@State`)
- [ ] Union types (`T \/ U`)
- [ ] Permission qualifiers (`const`, `unique`, `shared`)
- [ ] Generic instantiation (`Type<Args>`)

#### 2.2.4 Expression Parsing (Week 9-11)

- [ ] Literals (all forms from §8.2)
- [ ] Identifiers and qualified names
- [ ] Operators with precedence (Table 8.3.1)
  - Arithmetic, bitwise, logical, comparison
  - Assignment and compound assignment
  - Range operators
  - Pipeline operator (`=>`)
- [ ] Structured expressions (§8.4)
  - Record, tuple, array, enum construction
  - If/match expressions
  - Loop expressions (infinite, conditional, iterator)
  - Block expressions with `result`
- [ ] Closures with capture
- [ ] Region blocks and `^` allocation operator
- [ ] Comptime blocks
- [ ] Move expressions

#### 2.2.5 Statement Parsing (Week 12)

- [ ] Variable declarations
- [ ] Assignments (value `=` and reference `<-`)
- [ ] Expression statements
- [ ] Return, break, continue with labels
- [ ] Defer statements
- [ ] Control flow (if, match, loop as statements)

#### 2.2.6 Pattern Parsing (Week 13)

- [ ] Wildcard (`_`)
- [ ] Literal patterns
- [ ] Identifier patterns
- [ ] Tuple patterns
- [ ] Record patterns with field matching
- [ ] Enum patterns
- [ ] Union type annotation patterns
- [ ] Modal state patterns
- [ ] Or-patterns (`p1 | p2`)

#### 2.2.7 Phase 2 Deliverables

- ✅ Complete parser generating AST
- ✅ All Annex A grammar productions implemented
- ✅ Parse error recovery
- ✅ **Tests**: 2000+ parser tests covering all syntax
- ✅ **Diagnostic Codes**: E02-200 through E02-401

---

### Phase 3: Name Resolution & Scoping (Months 8-10)

**Objective**: Implement module system, name lookup, and scope analysis per Clauses 4 and 6.

#### 2.3.1 Module System (Week 1-4)

- [ ] Module table construction from filesystem
  - Manifest parsing (§1.7.6)
  - Source root resolution
  - Module path derivation (§4.1.3)
  - Case sensitivity handling (§4.1.3[5])
- [ ] Import/use resolution (§4.2, §4.3)
  - Import table and alias map
  - Wildcard expansion
  - Re-export handling
  - Visibility checking
- [ ] Module initialization ordering (§4.6)
  - Eager/lazy dependency classification
  - Topological sort of eager dependencies
  - Cycle detection (E04-500)
- [ ] **Diagnostic Codes**: E04-001 through E04-503
- [ ] **Tests**: Module system edge cases, cycles, visibility

#### 2.3.2 Scope Formation (Week 5-7)

- [ ] Scope tree construction (§6.2)
  - Module scope
  - Procedure scope
  - Block scope (including region blocks)
- [ ] Binding table per scope
- [ ] Unified namespace enforcement
- [ ] Shadow detection and validation (§6.3)
- [ ] **Diagnostic Codes**: E06-201, E06-202, E06-300 through E06-303
- [ ] **Tests**: Scope nesting, shadowing, unified namespace

#### 2.3.3 Name Lookup (Week 8-10)

- [ ] Unqualified lookup (§6.4.4)
  - Five-step algorithm (current scope → ancestors → module → imports → universe)
  - Ambiguity detection
- [ ] Qualified lookup (§6.4.5)
  - Module path resolution
  - Associated item lookup
  - Type-qualified names
- [ ] Predeclared bindings (§6.6)
  - Universe scope with primitives
  - No shadowing of predeclared
- [ ] **Diagnostic Codes**: E06-400 through E06-407
- [ ] **Tests**: Lookup edge cases, ambiguity, qualification

#### 2.3.4 Phase 3 Deliverables

- ✅ Complete module resolution
- ✅ Name lookup with full diagnostics
- ✅ Scope system with shadowing
- ✅ **Tests**: 1000+ name resolution tests

---

### Phase 4: Type System Implementation (Months 11-15)

**Objective**: Implement complete type system per Clause 7 with inference, subtyping, and variance.

#### 2.4.1 Type Representation (Week 1-4)

- [ ] Type class hierarchy
  - `Type` base class with kind, size, alignment
  - Primitive types (§7.2)
  - Composite types (records, tuples, enums, arrays, slices)
  - Pointer types (raw and safe modal)
  - Function types with grants/sequents
  - Modal types with state tracking
  - Witness types
  - Union types
- [ ] Type equality and canonical forms
- [ ] Type printing for diagnostics
- [ ] Generic type instantiation tracking

#### 2.4.2 Type Formation & Validation (Week 5-7)

- [ ] Well-formedness checking (WF-\* rules from Clause 7)
- [ ] Size and alignment calculation (§3.5, §11.6)
  - Primitive type layout
  - Record field layout with padding
  - Array/slice layout
  - Enum discriminant and payload layout
  - Union discriminant calculation
- [ ] Permission type formation
- [ ] Modal state type formation
- [ ] **Diagnostic Codes**: E07-001, E07-100 through E07-106
- [ ] **Tests**: Type formation edge cases

#### 2.4.3 Type Inference & Checking (Week 8-12)

- [ ] Bidirectional type inference (§7.1.2, §8.1.6)
  - Inference sites (synthesize type)
  - Checking sites (check against expected type)
  - Contextual typing for literals
- [ ] Unification algorithm
  - Type variable unification
  - Constraint generation
  - Constraint solving
  - Principal type calculation
- [ ] Expression typing (Clause 8)
  - Literals, identifiers, operators
  - Structured expressions (record, tuple, array, enum)
  - Control flow (if, match, loop)
  - Closures with capture type inference
  - Pipeline type checking with annotation validation
- [ ] **Diagnostic Codes**: E08-001, E08-002, E08-201 through E08-800
- [ ] **Tests**: 3000+ expression typing tests

#### 2.4.4 Subtyping & Variance (Week 13-14)

- [ ] Subtyping relations (§7.7)
  - Permission lattice: `unique <: shared <: const`
  - Modal widening: `M@State <: M`
  - Union introduction: `T <: T \/ U`
  - Never coercion: `! <: T`
  - Function subtyping with variance
- [ ] Variance inference (§10.7)
  - Position classification (covariant, contravariant, invariant)
  - Variance calculation for generic types
- [ ] Implicit coercions (§8.6.3)
  - Pointer/modal widening
  - String state coercion (`string@Managed <: string@View`)
  - Union widening
- [ ] **Diagnostic Codes**: E07-700 through E07-704
- [ ] **Tests**: Subtyping edge cases, variance violations

#### 2.4.5 Type Introspection (Week 15-16)

- [ ] `typeof` operator (§7.8.2)
- [ ] `type_name<T>()` intrinsic
- [ ] `type_id<T>()` intrinsic
- [ ] `type_info<T>()` for reflection
- [ ] **Diagnostic Codes**: E07-900 through E07-903
- [ ] **Tests**: Introspection queries

#### 2.4.6 Phase 4 Deliverables

- ✅ Complete type system
- ✅ Bidirectional type inference
- ✅ Subtyping and variance
- ✅ **Tests**: 4000+ type system tests
- ✅ **Coverage**: Clause 7 100%

---

### Phase 5: Generics & Behaviors (Months 16-19)

**Objective**: Implement generic system, behaviors, contracts, and monomorphization per Clause 10.

#### 2.5.1 Generic Parameters (Week 1-3)

- [ ] Type parameters with bounds (§10.2)
- [ ] Const parameters (compile-time values)
- [ ] Grant parameters (capability polymorphism)
- [ ] Default values for parameters
- [ ] Turbofish syntax (`::<Args>`)
- [ ] Where clauses (§10.3)
  - Behavior bounds
  - Associated type equality
  - Grant bounds
- [ ] **Diagnostic Codes**: E10-101 through E10-110
- [ ] **Tests**: Generic parameter edge cases

#### 2.5.2 Behaviors (Week 4-7)

- [ ] Behavior declarations (§10.4)
  - Procedure bodies (mandatory for behaviors)
  - Associated type declarations with defaults
  - Behavior extension (`with`)
  - `Self` type handling
- [ ] Marker behaviors (Copy, Sized, Drop)
  - Auto-derivation rules
  - Structural Copy checking
  - Copy/Drop mutual exclusion
- [ ] Behavior implementations (§10.5)
  - Inline (`with Behavior`)
  - Standalone (`behavior B for T`)
  - Blanket implementations (`for<T> T where ...`)
- [ ] Coherence checking
  - Orphan rule enforcement
  - Duplicate implementation detection
  - Overlap detection for blanket impls
- [ ] **Diagnostic Codes**: E10-201 through E10-413, E10-501 through E10-510
- [ ] **Tests**: Behavior system comprehensive tests

#### 2.5.3 Contracts (Week 8-9)

- [ ] Contract declarations (§12.1, Clause 12)
  - Procedure signatures without bodies
  - Associated type requirements
  - Contract extension
- [ ] Contract vs Behavior distinction enforcement
- [ ] Contract implementation in types
- [ ] Liskov substitution checking
  - Precondition weakening
  - Postcondition strengthening
- [ ] **Diagnostic Codes**: E12-091, E12-043, E12-052
- [ ] **Tests**: Contract interface tests

#### 2.5.4 Resolution & Monomorphization (Week 10-12)

- [ ] Type argument inference (§10.6.2)
  - From call arguments
  - From return type context
  - From bounds
- [ ] Const parameter evaluation
- [ ] Grant parameter inference
- [ ] Bound checking (§10.6.3)
- [ ] Monomorphization (§10.6.4)
  - Instantiation cache
  - Specialized code generation
  - Recursive instantiation support
- [ ] Behavior method resolution
  - Override selection
  - Inherited default selection
  - Blanket vs specific preference
- [ ] Associated type projection resolution
- [ ] **Diagnostic Codes**: E10-601 through E10-612
- [ ] **Tests**: Monomorphization edge cases, inference

#### 2.5.5 Phase 5 Deliverables

- ✅ Complete generic system
- ✅ Behavior and contract support
- ✅ Monomorphization working
- ✅ **Tests**: 2500+ generics tests
- ✅ **Coverage**: Clause 10 100%

---

### Phase 6: Memory Model & Safety Analysis (Months 20-24)

**Objective**: Implement memory safety analysis per Clause 11: permissions, regions, move checking.

#### 2.6.1 Permission System (Week 1-4)

- [ ] Permission tracking (§11.4)
  - Const, unique, shared permissions
  - Permission lattice and downgrades
  - Active reference tracking for unique
- [ ] Permission checking
  - Mutation through const (E11-301)
  - Multiple unique (E11-302)
  - Upgrade attempts (E11-303)
- [ ] Field-level partitioning (§11.4.5)
  - Partition directive parsing (`<<Name>>`)
  - Single-partition-access rule
  - Nested field access tracking
- [ ] **Diagnostic Codes**: E11-301, E11-302, E11-303, E11-310, E11-311
- [ ] **Tests**: Permission system comprehensive tests

#### 2.6.2 Region Analysis (Week 5-8)

- [ ] Provenance tracking (§11.3.3, Annex E §E.2.6)
  - Stack, Region(r), Heap provenance
  - Provenance propagation through expressions
  - Conservative analysis for procedure calls
- [ ] Region block handling
  - Arena modal type (`Arena@Active/Frozen/Freed`)
  - Region desugaring to arena creation
  - Caret operator (`^`) desugaring
  - Caret stacking (`^^`, `^^^`)
- [ ] Escape analysis (§11.3.4, Annex E §E.2.6.3)
  - Escape checking at return statements
  - Escape checking at region boundaries
  - Closure capture checking
  - Field assignment checking
- [ ] **Diagnostic Codes**: E11-101, E11-102, E11-103, E11-110
- [ ] **Tests**: Region allocation, escape detection

#### 2.6.3 Move & Ownership (Week 9-12)

- [ ] Move expression handling (§11.5.2)
  - `move` keyword validation
  - Transferable binding checking (only `let x = value`)
  - Move from var/non-responsible: E11-501, E11-502
- [ ] Definite assignment analysis (§5.7)
  - Initialization tracking
  - Move tracking
  - Use-after-move detection (E11-503)
  - Conditional branch analysis
- [ ] Non-responsible binding tracking (§5.7.5, §11.5.5.2)
  - Dependency graph (which non-resp bindings reference which objects)
  - Parameter responsibility detection (`move` modifier on params)
  - Invalidation propagation when moved to responsible param
  - Validity preservation when passed to non-responsible param
  - Diagnostic E11-504 (use of invalidated reference)
- [ ] Copy/Clone behavior checking
- [ ] **Diagnostic Codes**: E11-501 through E11-504, E11-510, E11-511
- [ ] **Tests**: Move semantics, definite assignment, non-responsible bindings

#### 2.6.4 RAII & Destructor Insertion (Week 13-14)

- [ ] Destructor call insertion (§11.2.4, §11.2.5)
  - LIFO destruction order
  - Custom `drop` procedure calls
  - Automatic field destruction
- [ ] Defer statement lowering
  - Defer queue per scope
  - Execution on all exit paths
- [ ] Cleanup on panic/unwind
- [ ] **Tests**: RAII correctness, defer ordering

#### 2.6.5 Layout & Alignment (Week 15-16)

- [ ] Layout calculation (§11.6)
  - Field offset computation
  - Padding insertion
  - Tail padding for arrays
- [ ] Repr attributes
  - `[[repr(C)]]` C-compatible layout
  - `[[repr(packed)]]` no padding
  - `[[repr(align(N))]]` custom alignment
  - `[[repr(transparent)]]` single-field wrapper
- [ ] Size and alignment queries
- [ ] **Diagnostic Codes**: E11-601, E11-602, E11-603
- [ ] **Tests**: Layout calculations, repr attributes

#### 2.6.6 Phase 6 Deliverables

- ✅ Complete memory safety analysis
- ✅ Permission checking operational
- ✅ Region escape analysis working
- ✅ Move and definite assignment complete
- ✅ **Tests**: 2000+ memory model tests
- ✅ **Coverage**: Clause 11 100%

---

### Phase 7: Contracts & Grants (Months 25-27)

**Objective**: Implement grant system and contractual sequents per Clause 12.

#### 2.7.1 Grant System (Week 1-3)

- [ ] Grant declarations and resolution (§5.9, §12.3)
- [ ] Built-in grant catalog (§12.3.3)
  - alloc, fs, net, io, thread, sync, sys, unsafe, ffi, panic
  - comptime grants
- [ ] Grant accumulation through expressions (§8.1.5)
- [ ] Grant checking at call sites (§12.7.2)
- [ ] Grant polymorphism (grant parameters)
- [ ] **Diagnostic Codes**: E12-006, E12-020, E12-030, E12-031, E12-032
- [ ] **Tests**: Grant propagation, checking

#### 2.7.2 Contractual Sequents (Week 4-6)

- [ ] Sequent parsing (§12.2)
  - Smart defaulting rules
  - Turnstile and implication
  - Grants, must, will clauses
- [ ] Predicate expression typing
  - Boolean type requirement
  - Purity checking
  - `result` identifier scoping
  - `@old` operator implementation
- [ ] **Diagnostic Codes**: E12-001 through E12-010
- [ ] **Tests**: Sequent syntax variants

#### 2.7.3 Contract Checking (Week 7-9)

- [ ] Precondition checking (§12.4, §12.7.2.3)
  - Static verification with dataflow analysis
  - Flow-sensitive proof
  - Dynamic check insertion (per mode)
- [ ] Postcondition checking (§12.5, §12.7.4)
  - `@old` value capture at entry
  - Multi-path validation
  - Dynamic check insertion
- [ ] Invariant desugaring (§12.6)
  - Type invariants to postcondition conjunctions
  - Loop invariants to verification points
- [ ] **Diagnostic Codes**: E12-040 through E12-069
- [ ] **Tests**: Contract verification

#### 2.7.4 Verification Modes (Week 10-12)

- [ ] Mode attribute parsing (`[[verify(mode)]]`)
- [ ] Static verification (§12.8.4)
  - Symbolic execution (optional)
  - SMT solver integration (optional)
- [ ] Dynamic verification (§12.8.5)
  - Assertion insertion
  - Panic on violation
- [ ] Trusted mode (skip checks)
- [ ] Build-mode defaults
- [ ] **Diagnostic Codes**: E12-056, E12-080, E12-081
- [ ] **Tests**: All verification modes

#### 2.7.5 Phase 7 Deliverables

- ✅ Complete grant system
- ✅ Contractual sequent support
- ✅ Contract verification (basic)
- ✅ **Tests**: 1500+ contract tests
- ✅ **Coverage**: Clause 12 100%

---

### Phase 8: Witness System & Dynamic Dispatch (Months 28-30)

**Objective**: Implement witness system for dynamic polymorphism per Clause 13.

#### 2.8.1 Witness Type System (Week 1-2)

- [ ] Witness type representation (§13.4)
  - Dense pointer structure (data_ptr, metadata_ptr)
  - 16-byte layout on 64-bit
- [ ] Witness formation rules (§13.3)
  - Behavior witnesses
  - Contract witnesses
  - Modal state witnesses
- [ ] Allocation state tracking (@Stack, @Region, @Heap)
- [ ] **Tests**: Witness type checking

#### 2.8.2 Witness Construction (Week 3-5)

- [ ] Automatic coercion from implementing types
- [ ] Stack witness construction (non-responsible)
- [ ] Heap witness construction (with `move`)
- [ ] Region witness construction
- [ ] Permission inheritance
- [ ] **Diagnostic Codes**: E13-001, E13-010, E13-011, E13-012
- [ ] **Tests**: Witness construction variants

#### 2.8.3 VTable Generation (Week 6-8)

- [ ] Witness table structure
  - Type ID
  - Size and alignment
  - Optional state tag (for modal witnesses)
  - Drop function pointer
  - Method vtable array
- [ ] VTable construction for behaviors
- [ ] VTable construction for contracts
- [ ] VTable construction for modal states
- [ ] VTable sharing across instances
- [ ] **Tests**: VTable correctness

#### 2.8.4 Dynamic Dispatch (Week 9-10)

- [ ] Method call syntax (`witness::method()`)
- [ ] VTable lookup implementation
- [ ] Indirect call generation
- [ ] Grant propagation through witness calls
- [ ] **Diagnostic Codes**: E13-020, E13-021, E13-022
- [ ] **Tests**: Dynamic dispatch correctness

#### 2.8.5 Witness Transitions (Week 11-12)

- [ ] State transitions (@Stack → @Region → @Heap)
- [ ] `to_region()` and `to_heap()` methods
- [ ] Cleanup responsibility transfer
- [ ] **Diagnostic Codes**: E13-030, E13-031, E13-032
- [ ] **Tests**: Witness transitions

#### 2.8.6 Phase 8 Deliverables

- ✅ Complete witness system
- ✅ Dynamic dispatch operational
- ✅ Heterogeneous collections working
- ✅ **Tests**: 1000+ witness tests
- ✅ **Coverage**: Clause 13 100%

---

### Phase 9: Concurrency & Memory Ordering (Months 31-33)

**Objective**: Implement thread safety and concurrency primitives per Clause 14.

#### 2.9.1 Thread Model (Week 1-3)

- [ ] Thread modal type (§14.1.3)
  - @Spawned, @Joined, @Detached states
  - State transitions
- [ ] Thread spawning (`spawn<T, ε>`)
  - Permission-based thread safety checking
  - Const return type requirement
  - Closure capture validation
- [ ] Thread joining and detaching
- [ ] **Diagnostic Codes**: E14-100 through E14-104
- [ ] **Tests**: Thread lifecycle

#### 2.9.2 Memory Model (Week 4-5)

- [ ] Happens-before relation (§14.2.3)
  - Intra-thread ordering
  - Thread spawn synchronization
  - Thread join synchronization
- [ ] Synchronizes-with relation (§14.2.4)
- [ ] Sequential consistency for DRF programs
- [ ] Permission-based race prevention
  - Const allows sharing
  - Unique requires exclusive ownership
  - Shared requires synchronization
- [ ] **Tests**: Memory ordering validation

#### 2.9.3 Atomic Operations (Week 6-8)

- [ ] Memory ordering enum (Relaxed, Acquire, Release, AcqRel, SeqCst)
- [ ] Atomic intrinsics (§14.3.3)
  - `atomic::load`, `atomic::store`
  - `atomic::compare_exchange`
  - `atomic::fetch_add/sub/and/or/xor`
- [ ] Ordering validation for operations
- [ ] LLVM atomic instruction generation
- [ ] **Diagnostic Codes**: E14-200 through E14-204
- [ ] **Tests**: Atomic correctness

#### 2.9.4 Synchronization Primitives (Week 9-12)

- [ ] Mutex modal type (§14.4.2)
  - @Unlocked, @Locked states
  - Lock/unlock transitions
  - MutexGuard RAII pattern
- [ ] Channel modal type (§14.4.3)
  - @Open, @SenderClosed, @ReceiverClosed states
  - Send/receive operations
- [ ] RwLock modal type (§14.4.4)
  - @Unlocked, @ReadLocked, @WriteLocked states
  - Permission reflection in states
- [ ] Synchronization semantics
  - Synchronizes-with for mutex
  - Synchronizes-with for channel
- [ ] **Diagnostic Codes**: E14-300, E14-301
- [ ] **Tests**: Synchronization primitives

#### 2.9.5 Phase 9 Deliverables

- ✅ Thread safety via permissions
- ✅ Concurrency primitives working
- ✅ Atomic operations functional
- ✅ **Tests**: 800+ concurrency tests
- ✅ **Coverage**: Clause 14 100%

---

### Phase 10: FFI & Interoperability (Months 34-36)

**Objective**: Implement foreign function interface and ABI compliance per Clause 15.

#### 2.10.1 FFI Declarations (Week 1-3)

- [ ] Extern attribute parsing (`[[extern(C)]]`)
- [ ] Calling convention support (§15.6.2)
  - C (mandatory)
  - stdcall, fastcall, system (platform-dependent)
- [ ] FFI-safe type validation (§15.1.4)
- [ ] Unwind behavior attributes
  - `unwind(abort)` (default)
  - `unwind(catch)` (panic catching)
- [ ] **Diagnostic Codes**: E15-001 through E15-010
- [ ] **Tests**: FFI declaration validation

#### 2.10.2 C Compatibility (Week 4-6)

- [ ] `[[repr(C)]]` layout generation (§15.3)
  - C-compatible field ordering
  - Platform padding rules
  - Tag type for enums
- [ ] String conversion
  - `as_c_ptr()` for null-terminated strings
  - `from_c_str()` with UTF-8 validation
- [ ] Function pointer compatibility
  - Extern procedure pointers
  - Callback restrictions
- [ ] **Diagnostic Codes**: E15-030, E15-031, E15-040
- [ ] **Tests**: C interop tests with actual C libraries

#### 2.10.3 Platform Features (Week 7-8)

- [ ] Platform query intrinsics (§15.4.2)
  - `target_os()`, `target_arch()`, `target_endian()`, `target_pointer_width()`
- [ ] Comptime platform conditionals
- [ ] Inline assembly (v1.0: error E15-051)
- [ ] SIMD intrinsics (v1.0: error E15-052)
- [ ] **Diagnostic Codes**: E15-050 through E15-053
- [ ] **Tests**: Platform conditionals

#### 2.10.4 ABI Implementation (Week 9-11)

- [ ] Calling convention codegen (§15.6)
  - System V AMD64 (Linux/Unix x86-64)
  - Microsoft x64 (Windows x86-64)
  - ARM AAPCS (ARM64)
- [ ] Parameter passing in registers
- [ ] Return value handling
- [ ] Stack alignment maintenance
- [ ] **Diagnostic Codes**: E15-080, E15-081, E15-082
- [ ] **Tests**: ABI compliance tests

#### 2.10.5 Linkage & Symbols (Week 12)

- [ ] Symbol visibility (§15.5)
  - External, internal, no linkage
  - One Definition Rule enforcement
- [ ] Name mangling
  - Implementation-defined scheme
  - `[[no_mangle]]` support
  - Extern implies no-mangle
- [ ] Weak symbols (§15.5.7)
  - Platform-dependent support
- [ ] **Diagnostic Codes**: E15-060, E15-061, E15-062, E15-070
- [ ] **Tests**: Linkage and symbols

#### 2.10.6 Phase 10 Deliverables

- ✅ FFI fully operational
- ✅ C interop validated
- ✅ Multiple platform ABIs
- ✅ **Tests**: 600+ FFI/interop tests
- ✅ **Coverage**: Clause 15 100%

---

### Phase 11: Compile-Time Evaluation & Reflection (Months 37-40)

**Objective**: Implement comptime system and reflection per Clause 16.

#### 2.11.1 Comptime Execution (Week 1-4)

- [ ] Comptime procedure execution (§16.2)
  - Comptime-only grant restriction
  - Purity enforcement
  - Resource limits (256 recursion, 1M steps, 64 MiB)
  - Termination checking
- [ ] Comptime blocks (§16.3)
  - Dependency analysis
  - Cycle detection
  - Evaluation ordering
- [ ] Comptime intrinsics (§16.4)
  - Assertions: `comptime_assert`, `comptime_error`, `comptime_warning`, `comptime_note`
  - Config: `cfg()`, `cfg_value()`
  - Platform: `target_os()`, `target_arch()`, etc.
  - Type properties: `is_copy()`, `is_sized()`
  - String utilities: `string_concat()`, `usize_to_string()`
- [ ] **Diagnostic Codes**: E16-001 through E16-042
- [ ] **Tests**: Comptime evaluation

#### 2.11.2 Reflection System (Week 5-8)

- [ ] `[[reflect]]` attribute processing (§16.5)
- [ ] Metadata generation
  - Zero-cost guarantee (no instance overhead)
  - Compile-time metadata only
- [ ] Reflection queries (§16.6)
  - `reflect_type<T>()`
  - `fields_of<T>()`, `procedures_of<T>()`, `variants_of<T>()`
  - `states_of<T>()`, `transitions_of<T>()`
  - Offset and layout queries
- [ ] Visibility enforcement during reflection
- [ ] **Diagnostic Codes**: E16-100 through E16-122
- [ ] **Tests**: Reflection queries

#### 2.11.3 Code Generation API (Week 9-12)

- [ ] TypeRef enumeration (§16.8.2)
- [ ] Specification structures
  - ProcedureSpec, TypeSpec, ParamSpec, SequentSpec
  - FieldSpec, VariantSpec, GenericParamSpec
- [ ] Codegen API procedures
  - `declare_procedure()`, `declare_type()`, `declare_constant()`
  - `add_procedure()`, `add_function()`
- [ ] Quote expressions (§16.7)
  - Quote capture
  - Interpolation (`$(...)` syntax)
  - Identifier vs value splicing
- [ ] Hygiene (`gensym()`)
- [ ] Generated code validation
- [ ] **Diagnostic Codes**: E16-200 through E16-240
- [ ] **Tests**: Code generation, metaprogramming

#### 2.11.4 Phase 11 Deliverables

- ✅ Comptime evaluation working
- ✅ Reflection system operational
- ✅ Code generation functional
- ✅ **Tests**: 1200+ comptime/reflection tests
- ✅ **Coverage**: Clause 16 100%

---

### Phase 12: LLVM IR Generation & Optimization (Months 41-45)

**Objective**: Generate optimized LLVM IR with all runtime support.

#### 2.12.1 Basic IR Generation (Week 1-4)

- [ ] LLVM IR Builder integration
- [ ] Procedure lowering
  - Function prototypes
  - Parameter passing
  - Return values
  - Calling conventions
- [ ] Expression lowering
  - Arithmetic, logical, bitwise operators
  - Calls (direct and indirect)
  - Memory operations (load/store)
- [ ] Statement lowering
  - Control flow (br, switch, phi nodes)
  - RAII cleanup insertion
- [ ] **Tests**: IR generation correctness

#### 2.12.2 Advanced IR Generation (Week 5-8)

- [ ] Modal type lowering
  - State tracking through IR
  - Transition functions
- [ ] Witness lowering
  - Dense pointer representation
  - VTable indirect calls
  - Type erasure
- [ ] Closure lowering
  - Environment record generation
  - Capture handling
  - Apply method generation
- [ ] Region allocation
  - Arena calls
  - Bulk deallocation
- [ ] **Tests**: Advanced feature IR

#### 2.12.3 Optimization Pipeline (Week 9-12)

- [ ] LLVM optimization passes
  - Inlining
  - Dead code elimination
  - Constant propagation
  - Loop optimization
- [ ] Cursive-specific optimizations
  - Witness devirtualization
  - Permission-aware alias analysis
  - Grant-aware DCE
- [ ] Link-time optimization (LTO)
- [ ] **Tests**: Optimization correctness

#### 2.12.4 Target Code Generation (Week 13-16)

- [ ] x86-64 backend
  - Linux (System V ABI)
  - Windows (Microsoft x64)
  - macOS (System V)
- [ ] ARM64 backend
  - Linux (AAPCS)
  - macOS (Apple Silicon)
- [ ] Object file generation
  - ELF (Linux)
  - PE (Windows)
  - Mach-O (macOS)
- [ ] **Tests**: Cross-platform correctness

#### 2.12.5 Phase 12 Deliverables

- ✅ LLVM IR generation complete
- ✅ Optimizations functional
- ✅ Multi-platform support
- ✅ **Tests**: 1500+ codegen tests

---

### Phase 13: Runtime Library (Months 46-48)

**Objective**: Implement standard runtime support per language requirements.

#### 2.13.1 Core Runtime (Week 1-4)

- [ ] Arena allocator (§11.3.2)
  - `Arena@Active/Frozen/Freed` implementation
  - Bump allocation
  - O(1) reset and free
  - Thread-local arena support
- [ ] String modal type (§7.3.4.3)
  - `string@View` (ptr + len)
  - `string@Managed` (ptr + len + cap)
  - UTF-8 validation
  - State transitions
- [ ] Panic and unwinding (§11.2.5.5)
  - Panic handler
  - Stack unwinding (optional)
  - Abort-on-panic mode
  - Double-panic protection
- [ ] **Tests**: Runtime primitives

#### 2.13.2 Concurrency Runtime (Week 5-8)

- [ ] Thread implementation (§14.1)
  - OS thread creation
  - Join/detach operations
- [ ] Mutex implementation (§14.4.2)
  - Platform mutexes (pthread, Windows)
  - State tracking
- [ ] Channel implementation (§14.4.3)
  - MPSC queue
  - Blocking send/receive
- [ ] RwLock implementation (§14.4.4)
- [ ] Atomic operations (map to LLVM atomics)
- [ ] **Tests**: Concurrency runtime

#### 2.13.3 Standard Behaviors (Week 9-12)

- [ ] Copy behavior implementation
- [ ] Clone behavior implementation
- [ ] Display behavior implementation
- [ ] Debug behavior implementation
- [ ] Drop behavior infrastructure
- [ ] **Tests**: Behavior implementations

#### 2.13.4 Phase 13 Deliverables

- ✅ Complete runtime library
- ✅ Concurrency support
- ✅ Standard behaviors
- ✅ **Tests**: 800+ runtime tests

---

### Phase 14: Integration & Polishing (Months 49-52)

**Objective**: End-to-end integration, diagnostic quality, tooling.

#### 2.14.1 Compiler Driver (Week 1-2)

- [ ] Command-line argument parsing
- [ ] Build mode selection (debug/release)
- [ ] Compilation pipeline orchestration
- [ ] Multi-file compilation
- [ ] Incremental compilation (optional)
- [ ] **Tests**: Driver functionality

#### 2.14.2 Diagnostic Enhancement (Week 3-4)

- [ ] High-quality error messages (Annex E §E.5.4)
- [ ] Fix-it hints generation
- [ ] Multi-location diagnostics
- [ ] Diagnostic context and notes
- [ ] **Tests**: Diagnostic quality

#### 2.14.3 Tooling (Week 5-8)

- [ ] Code formatter (`cursive-fmt`)
  - Respect continuation rules
  - Configurable style
- [ ] Language Server Protocol (LSP)
  - Hover information
  - Go-to-definition
  - Completion
  - Diagnostics
- [ ] Documentation generator
  - Extract doc comments
  - Generate HTML/Markdown
- [ ] **Tests**: Tooling functionality

#### 2.14.4 Conformance Validation (Week 9-12)

- [ ] Specification compliance suite
  - Test every diagnostic code
  - Test all language features
  - Test all edge cases
- [ ] Cross-reference validation
- [ ] ABI conformance tests
- [ ] Performance benchmarks
- [ ] **Tests**: 10,000+ conformance tests

#### 2.14.5 Phase 14 Deliverables

- ✅ Production-ready compiler
- ✅ Complete tooling suite
- ✅ Conformance validated
- ✅ **Tests**: Full specification coverage

---

## 3. Component Breakdown

### 3.1 Lexer Component (`lib/Lexer/`)

**Files**:

- `Lexer.h/cpp` — Main lexer class
- `Token.h/cpp` — Token representation
- `Keywords.def` — Keyword table
- `CharInfo.h` — Unicode character classification
- `NumericLiteralParser.h/cpp` — Numeric parsing
- `StringLiteralParser.h/cpp` — String/char parsing

**Key Classes**:

```cpp
class Lexer {
public:
    Lexer(SourceBuffer& buffer, DiagnosticEngine& diags);
    Token lex();
    Token peek();

private:
    Token lexIdentifierOrKeyword();
    Token lexNumericLiteral();
    Token lexStringLiteral();
    Token lexCharLiteral();
    bool skipWhitespace();
    bool skipComment();
    bool checkContinuation();
};

struct Token {
    TokenKind kind;
    SourceRange location;
    std::string lexeme;
    // For literals: parsed value
    std::variant<int64_t, double, std::string, char> value;
};
```

**Complexity**: ~5,000 LOC  
**Dependencies**: DiagnosticEngine, SourceManager  
**Tests**: 500+ unit tests

### 3.2 Parser Component (`lib/Parse/`)

**Files**:

- `Parser.h/cpp` — Main parser class
- `DeclParser.cpp` — Declaration parsing
- `ExprParser.cpp` — Expression parsing
- `StmtParser.cpp` — Statement parsing
- `TypeParser.cpp` — Type expression parsing
- `PatternParser.cpp` — Pattern parsing
- `ContractParser.cpp` — Sequent parsing

**Key Classes**:

```cpp
class Parser {
public:
    Parser(Lexer& lexer, ASTContext& ctx);
    std::unique_ptr<ModuleDecl> parseModule();

private:
    // Declarations
    Decl* parseDecl();
    ProcedureDecl* parseProcedure();
    RecordDecl* parseRecord();
    EnumDecl* parseEnum();
    ModalDecl* parseModal();

    // Expressions
    Expr* parseExpr();
    Expr* parsePrimaryExpr();
    Expr* parseBinaryExpr(int minPrec);

    // Statements
    Stmt* parseStmt();

    // Types
    Type* parseType();

    // Patterns
    Pattern* parsePattern();

    // Sequents
    Sequent* parseSequent();

    // Utilities
    bool expect(TokenKind kind);
    void recover();
};
```

**Complexity**: ~15,000 LOC  
**Dependencies**: Lexer, AST, DiagnosticEngine  
**Tests**: 2000+ parser tests

### 3.3 AST Component (`lib/AST/`)

**Files**:

- `Decl.h/cpp` — Declaration nodes
- `Expr.h/cpp` — Expression nodes
- `Stmt.h/cpp` — Statement nodes
- `Type.h/cpp` — Type nodes
- `Pattern.h/cpp` — Pattern nodes
- `ASTContext.h/cpp` — AST memory management
- `ASTVisitor.h` — Visitor pattern
- `ASTDumper.cpp` — Debug printing

**Key Classes**:

```cpp
// Base classes
class Decl {
public:
    enum DeclKind { VarDecl, ProcedureDecl, RecordDecl, ... };
    DeclKind getKind() const;
    SourceRange getLocation() const;
    virtual ~Decl() = default;
};

class Expr {
public:
    enum ExprKind { Literal, Identifier, BinaryOp, Call, ... };
    ExprKind getKind() const;
    Type* getType() const;
    SourceRange getLocation() const;
};

// Specific declarations
class VarDecl : public Decl {
    Identifier name;
    Type* type;
    BindingOperator op;  // = or <-
    Expr* initializer;
    bool isResponsible;
    bool isRebindable;
};

class ProcedureDecl : public Decl {
    Identifier name;
    GenericParams* generics;
    std::vector<ParamDecl*> params;
    Type* returnType;
    Sequent* contract;
    BlockStmt* body;
};

// Type system
class Type {
    enum TypeKind { Primitive, Record, Enum, Modal, Pointer, Function, ... };
    virtual size_t getSize() const = 0;
    virtual size_t getAlignment() const = 0;
    virtual bool isCopy() const = 0;
};

class RecordType : public Type {
    RecordDecl* decl;
    std::vector<Type*> fieldTypes;
    std::unordered_map<std::string, size_t> fieldOffsets;
};
```

**Complexity**: ~20,000 LOC  
**Dependencies**: None (foundation)  
**Tests**: AST construction and traversal

### 3.4 Semantic Analysis Component (`lib/Sema/`)

**Files** (Major subsystems):

- `Sema.h/cpp` — Main semantic analyzer
- `TypeChecker.h/cpp` — Type checking and inference
- `NameResolver.h/cpp` — Name lookup
- `PermissionChecker.h/cpp` — Permission analysis
- `RegionAnalysis.h/cpp` — Escape analysis
- `MoveChecker.h/cpp` — Move and definite assignment
- `ContractChecker.h/cpp` — Grant and contract verification
- `GenericResolver.h/cpp` — Generic instantiation
- `MonomorphizationEngine.h/cpp` — Template specialization

**Key Classes**:

```cpp
class Sema {
public:
    Sema(ASTContext& ctx, DiagnosticEngine& diags);

    void analyzeModule(ModuleDecl* mod);

    // Type checking
    Type* checkExpr(Expr* expr, Type* expected = nullptr);
    Type* inferType(Expr* expr);

    // Name resolution
    Decl* lookupUnqualified(Identifier name);
    Decl* lookupQualified(ModulePath path, Identifier name);

    // Permission checking
    void checkPermission(Expr* expr, Permission required);

    // Region analysis
    Provenance computeProvenance(Expr* expr);
    void checkEscape(Expr* expr, Scope* target);

    // Move checking
    void checkMove(Expr* moveExpr);
    void trackInvalidation(Binding* binding);

    // Contract checking
    void checkGrants(CallExpr* call);
    void checkPrecondition(CallExpr* call);
    void checkPostcondition(ProcedureDecl* proc);
};

class TypeChecker {
    // Inference context with type variables
    std::unordered_map<TypeVar*, Type*> substitution;
    std::vector<Constraint*> constraints;

    Type* infer(Expr* expr);
    void unify(Type* t1, Type* t2);
    Type* applySubstitution(Type* t);
    Type* getPrincipalType();
};

class PermissionChecker {
    // Active binding tracking for unique enforcement
    struct ActiveBinding {
        Binding* binding;
        Permission permission;
        Scope* scope;
    };
    std::vector<ActiveBinding> activeBindings;

    void checkUniqueExclusive(Binding* binding);
    void checkPartitionAccess(FieldAccess* access);
};

class MoveChecker {
    // Move tracking and invalidation
    std::unordered_set<Binding*> movedBindings;
    std::unordered_map<Binding*, std::vector<Binding*>> dependencies;  // Non-resp → source

    void markMoved(Binding* binding);
    void propagateInvalidation(Binding* binding);
    bool isResponsibleParameter(ParamDecl* param);
};
```

**Complexity**: ~40,000 LOC (largest component)  
**Dependencies**: AST, Type system  
**Tests**: 6000+ semantic analysis tests

### 3.5 Compile-Time Evaluation Component (`lib/CompTime/`)

**Files**:

- `ComptimeEvaluator.h/cpp` — Execution engine
- `ComptimeValue.h/cpp` — Comptime values
- `ReflectionEngine.h/cpp` — Reflection queries
- `CodeGenerator.h/cpp` — Codegen API
- `QuoteHandler.h/cpp` — Quote and interpolation

**Key Classes**:

```cpp
class ComptimeEvaluator {
    struct EvalContext {
        std::unordered_map<Binding*, ComptimeValue> bindings;
        size_t recursionDepth = 0;
        size_t evalSteps = 0;
        size_t memoryUsed = 0;
    };

    ComptimeValue eval(Expr* expr, EvalContext& ctx);
    void checkResourceLimits(EvalContext& ctx);
};

class ReflectionEngine {
    // Metadata storage
    struct TypeMetadata {
        std::string name;
        size_t size, align;
        TypeKind kind;
        std::vector<FieldInfo> fields;
        std::vector<ProcedureInfo> procedures;
        std::vector<VariantInfo> variants;
    };

    std::unordered_map<Type*, TypeMetadata> metadataCache;

    TypeMetadata reflect(Type* type);
    void ensureReflectable(Type* type);  // Check [[reflect]] attribute
};

class CodeGenerator {
    void declareProcedure(ProcedureSpec spec);
    void declareType(TypeSpec spec);
    void declareConstant(std::string name, TypeRef ty, QuotedExpr value);

    std::string gensym(std::string prefix);
    size_t gensymCounter = 0;
};
```

**Complexity**: ~12,000 LOC  
**Dependencies**: AST, Sema  
**Tests**: 1200+ comptime tests

### 3.6 IR Generation Component (`lib/CodeGen/`)

**Files**:

- `CodeGenModule.h/cpp` — Module-level codegen
- `CodeGenFunction.h/cpp` — Procedure-level codegen
- `CGExpr.cpp` — Expression codegen
- `CGStmt.cpp` — Statement codegen
- `CGType.cpp` — Type lowering
- `CGWitness.cpp` — Witness vtable generation
- `CGRAII.cpp` — Cleanup code insertion

**Key Classes**:

```cpp
class CodeGenModule {
    llvm::Module& module;
    llvm::IRBuilder<> builder;

    llvm::Function* genProcedure(ProcedureDecl* decl);
    llvm::StructType* genRecordType(RecordType* type);
    llvm::Constant* genWitnessTable(BehaviorImpl* impl);
};

class CodeGenFunction {
    llvm::Function* fn;
    llvm::IRBuilder<> builder;
    std::unordered_map<Binding*, llvm::Value*> bindings;
    std::vector<llvm::BasicBlock*> cleanupBlocks;  // For RAII

    llvm::Value* genExpr(Expr* expr);
    void genStmt(Stmt* stmt);
    void genCleanup();  // LIFO destruction
};
```

**Complexity**: ~18,000 LOC  
**Dependencies**: LLVM, AST, Sema  
**Tests**: 1500+ codegen tests

### 3.7 Diagnostic Engine (`lib/Basic/Diagnostics/`)

**Files**:

- `DiagnosticEngine.h/cpp`
- `DiagnosticIDs.def` — All E[CC]-[NNN] codes
- `DiagnosticRenderer.h/cpp` — Human-readable output
- `JSONDiagnostics.cpp` — Machine-readable output

**Key Classes**:

```cpp
class DiagnosticEngine {
public:
    enum Severity { Error, Warning, Note };

    void report(DiagnosticCode code, SourceLocation loc,
                std::string message, std::vector<FixItHint> hints = {});

    void setFormat(OutputFormat format);  // Text or JSON
    bool hasErrors() const;

private:
    std::vector<Diagnostic> diagnostics;
    OutputFormat format = Text;
};

struct Diagnostic {
    std::string code;  // E[CC]-[NNN]
    Severity severity;
    std::string message;
    SourceLocation location;
    std::vector<Note> notes;
    std::vector<FixItHint> hints;
};
```

**Complexity**: ~8,000 LOC  
**Dependencies**: SourceManager  
**Tests**: Diagnostic formatting

---

## 4. Testing Strategy

### 4.1 Unit Tests

**Framework**: Google Test (gtest)  
**Coverage Target**: 85%+ line coverage per component

**Test Categories**:

1. **Lexer**: Token recognition, escape sequences, Unicode, diagnostics
2. **Parser**: Syntax variations, error recovery, all grammar productions
3. **Sema**: Type checking, name resolution, permission checking
4. **CodeGen**: IR correctness, optimization preservation

**Test Organization**:

```
test/unit/
├── Lexer/
│   ├── TokenTests.cpp
│   ├── LiteralTests.cpp
│   └── ContinuationTests.cpp
├── Parse/
│   ├── DeclParseTests.cpp
│   ├── ExprParseTests.cpp
│   └── TypeParseTests.cpp
├── Sema/
│   ├── TypeCheckTests.cpp
│   ├── PermissionTests.cpp
│   └── MoveCheckTests.cpp
└── CodeGen/
    ├── IRGenTests.cpp
    └── OptTests.cpp
```

### 4.2 Integration Tests

**Framework**: lit (LLVM Integrated Tester)  
**Format**: `.cursive` files with expected diagnostics

**Test Types**:

1. **Positive tests**: Valid programs that should compile
2. **Negative tests**: Invalid programs with expected diagnostics
3. **End-to-end tests**: Compile, run, verify output

**Example Test**:

```cursive
// RUN: %cursivec %s -verify
// EXPECT: E05-202

procedure test() {
    let x = 10
    x = 20  // EXPECT-ERROR: E05-202 cannot assign to immutable binding
}
```

**Test Count**: 5000+ integration tests

### 4.3 Conformance Tests

**Framework**: Custom specification compliance harness  
**Source**: One test per normative requirement

**Coverage**:

- Every diagnostic code (E02-xxx through E16-xxx): ~350 tests
- Every language feature: ~500 tests
- Every example in specification: ~200 tests
- Edge cases and interactions: ~1000 tests

**Test Organization**:

```
test/spec-conformance/
├── clause02-lexical/
├── clause03-basic-concepts/
├── clause04-modules/
├── clause05-declarations/
├── clause06-names-resolution/
├── clause07-type-system/
├── clause08-expressions/
├── clause09-statements/
├── clause10-generics/
├── clause11-memory-model/
├── clause12-contracts/
├── clause13-witnesses/
├── clause14-concurrency/
├── clause15-ffi/
└── clause16-comptime/
```

### 4.4 Regression Tests

**Maintenance**: Add test for every bug found  
**Organization**: By GitHub issue number or bug ID

### 4.5 Performance Tests

**Benchmarks**:

- Compilation speed (lines/second)
- Generated code performance vs handwritten C++
- Memory usage during compilation
- Monomorphization overhead

**Target**: Within 2x of rustc/clang for equivalent constructs

---

## 5. Milestones and Deliverables

### Milestone 1: Lexer Complete (Month 3)

- ✅ All tokens recognized
- ✅ E02-001 through E02-401 diagnostics
- ✅ 500+ tests passing

### Milestone 2: Parser Complete (Month 7)

- ✅ Full AST from source
- ✅ Parse error recovery
- ✅ 2000+ tests passing

### Milestone 3: Basic Type Checking (Month 12)

- ✅ Expression typing operational
- ✅ Simple programs type-check
- ✅ 2000+ tests passing

### Milestone 4: Generics Working (Month 19)

- ✅ Generic instantiation
- ✅ Monomorphization
- ✅ 2000+ tests passing

### Milestone 5: Memory Safety Complete (Month 24)

- ✅ All permission checking
- ✅ Region escape analysis
- ✅ Move checking
- ✅ 3000+ tests passing

### Milestone 6: Contracts Functional (Month 27)

- ✅ Grant checking
- ✅ Basic contract verification
- ✅ 2000+ tests passing

### Milestone 7: Witnesses & Dynamic Dispatch (Month 30)

- ✅ Witness construction
- ✅ VTable generation
- ✅ Dynamic dispatch
- ✅ 1500+ tests passing

### Milestone 8: Concurrency (Month 33)

- ✅ Thread safety checking
- ✅ Atomic operations
- ✅ Sync primitives
- ✅ 1000+ tests passing

### Milestone 9: FFI (Month 36)

- ✅ C interop
- ✅ Platform ABIs
- ✅ 800+ tests passing

### Milestone 10: Comptime/Reflection (Month 40)

- ✅ Comptime evaluation
- ✅ Reflection working
- ✅ Code generation
- ✅ 1500+ tests passing

### Milestone 11: LLVM IR Generation (Month 45)

- ✅ Complete IR generation
- ✅ Optimization working
- ✅ Multi-platform support
- ✅ 2000+ tests passing

### Milestone 12: Production Ready (Month 52)

- ✅ All features complete
- ✅ Tooling functional
- ✅ 10,000+ tests passing
- ✅ 95%+ spec conformance

---

## 6. Risk Assessment and Mitigation

### Risk 1: Comptime Execution Complexity

**Severity**: HIGH  
**Impact**: Comptime system is novel and complex  
**Mitigation**:

- Start with simple interpreter
- Add optimization later
- Extensive testing of resource limits
- Consider LLVM JIT for comptime (advanced)

### Risk 2: Non-Responsible Binding Tracking

**Severity**: MEDIUM  
**Impact**: Parameter responsibility system is novel  
**Mitigation**:

- Implement dependency graph carefully
- Extensive testing of invalidation propagation
- Clear documentation of algorithm
- Reference implementation in TypeScript first (prototype)

### Risk 3: Witness VTable Generation

**Severity**: MEDIUM  
**Impact**: Dynamic dispatch with permissions is complex  
**Mitigation**:

- Study LLVM's existing vtable support
- Prototype vtable layout early
- Test heterogeneous collections thoroughly

### Risk 4: Cross-Platform ABI Compliance

**Severity**: MEDIUM  
**Impact**: Multiple ABIs to support  
**Mitigation**:

- Use LLVM's ABI lowering where possible
- Test on actual platforms
- Reference platform ABI docs rigorously

### Risk 5: Specification Ambiguities

**Severity**: LOW-MEDIUM  
**Impact**: May discover underspecified areas  
**Mitigation**:

- Document interpretation decisions
- Consult specification authors
- Contribute clarifications back to spec

### Risk 6: Performance of Generated Code

**Severity**: MEDIUM  
**Impact**: Zero-cost abstraction must be validated  
**Mitigation**:

- Benchmark against C++ equivalent
- Profile generated code
- Optimize based on measurements

### Risk 7: Team Size and Expertise

**Severity**: HIGH  
**Impact**: Requires compiler engineering expertise  
**Mitigation**:

- Hire experienced compiler engineers
- Provide training on LLVM
- Pair programming for complex components
- Code review for all changes

---

## 7. Resource Requirements

### 7.1 Team Composition

**Minimum Team (4-6 engineers)**:

1. **Tech Lead / Architect** (1)
   - Overall design and architecture
   - LLVM expertise required
   - Specification interpretation
2. **Frontend Engineers** (2)
   - Lexer, parser, AST
   - C++ and parser generator experience
3. **Semantic Analysis Engineer** (1-2)
   - Type systems expertise
   - Static analysis background
   - Dataflow analysis experience
4. **Backend / CodeGen Engineer** (1)
   - LLVM IR generation
   - Optimization pipeline
   - Multi-platform ABI knowledge
5. **Testing / QA Engineer** (0.5-1)
   - Test infrastructure
   - Conformance validation
   - CI/CD maintenance

**Optional Additions**:

- Comptime/Reflection specialist
- Runtime library engineer
- Tooling developer (LSP, formatter)

### 7.2 Infrastructure

**Hardware**:

- Build servers (Linux, Windows, macOS)
- Test infrastructure (CI runners)
- Development workstations

**Software**:

- LLVM 17+ development libraries
- CMake 3.20+
- ANTLR4 (if used for parser)
- Google Test, LLVM lit
- Git, GitHub/GitLab
- Documentation tools

**Third-Party Dependencies**:

- LLVM (Apache 2.0 license compatible)
- Google Test (BSD)
- fmt library (for C++ string formatting)
- UTF-8 library (optional, can implement from Unicode spec)

### 7.3 Timeline Estimate

**Conservative Estimate** (4-person team):

- **Months 1-12**: Frontend complete (lexer, parser, basic sema)
- **Months 13-24**: Type system and generics complete
- **Months 25-36**: Memory model, contracts, witnesses
- **Months 37-48**: Comptime, FFI, runtime
- **Months 49-52**: Integration, polish, conformance

**Aggressive Estimate** (6-person team):

- **Months 1-8**: Frontend complete
- **Months 9-16**: Type system and generics
- **Months 17-24**: Memory model, contracts
- **Months 25-30**: Witnesses, concurrency, FFI
- **Months 31-36**: Comptime, reflection, runtime
- **Months 37-40**: Integration and conformance

### 7.4 Budget Estimate

**Personnel** (4 engineers × 4 years):

- Engineers: $150k-$250k/year each
- Total: $2.4M - $4.0M

**Infrastructure**:

- CI/CD: $5k-$10k/year
- Servers: $10k-$20k/year
- Licenses/Tools: $5k/year
- Total: $80k - $140k over 4 years

**Total Project Cost**: $2.5M - $4.2M

---

## 8. Conformance Validation

### 8.1 Specification Compliance Checklist

**Clause 2 - Lexical**:

- [x] UTF-8 validation
- [x] BOM handling
- [x] Line ending normalization
- [x] All token types
- [x] Statement termination rules
- [x] Maximal munch
- [x] Diagnostics E02-xxx

**Clause 3 - Basic Concepts**:

- [x] Object model
- [x] Type classification
- [x] Storage duration categories
- [x] Alignment rules
- [x] Binding categories

**Clause 4 - Modules**:

- [x] Module derivation from filesystem
- [x] Import/use resolution
- [x] Dependency ordering
- [x] Visibility enforcement
- [x] Diagnostics E04-xxx

**Clause 5 - Declarations**:

- [x] Variable bindings with = and <-
- [x] Procedures with move parameters
- [x] Type declarations (record, enum, modal)
- [x] Contracts and behaviors
- [x] Grant declarations
- [x] Diagnostics E05-xxx

**Clause 6 - Names & Scopes**:

- [x] Scope formation
- [x] Name lookup (5-step algorithm)
- [x] Shadowing with explicit keyword
- [x] Qualified names
- [x] Diagnostics E06-xxx

**Clause 7 - Types**:

- [x] All primitive types
- [x] Composite types
- [x] Pointer types (raw and safe modal)
- [x] Function types with grants
- [x] Modal types with states
- [x] Witness types
- [x] Union types
- [x] Type relations (equivalence, subtyping)
- [x] Diagnostics E07-xxx

**Clause 8 - Expressions**:

- [x] All expression forms
- [x] Operators with precedence
- [x] Type inference
- [x] Grant accumulation
- [x] Diagnostics E08-xxx

**Clause 9 - Statements**:

- [x] All statement forms
- [x] Control flow outcomes
- [x] Defer statements
- [x] Diagnostics E09-xxx

**Clause 10 - Generics**:

- [x] Type, const, grant parameters
- [x] Bounds and where clauses
- [x] Behaviors with bodies
- [x] Contracts without bodies
- [x] Monomorphization
- [x] Variance
- [x] Diagnostics E10-xxx

**Clause 11 - Memory Model**:

- [x] Permissions (const, unique, shared)
- [x] Region allocation and escape analysis
- [x] Move semantics with explicit keyword
- [x] Non-responsible bindings with parameter responsibility
- [x] RAII and destructors
- [x] Layout and alignment
- [x] Diagnostics E11-xxx

**Clause 12 - Contracts**:

- [x] Contractual sequents with smart defaulting
- [x] Grant system
- [x] Preconditions and postconditions
- [x] Invariants
- [x] Verification modes
- [x] Diagnostics E12-xxx

**Clause 13 - Witnesses**:

- [x] Witness types with allocation states
- [x] Dense pointer representation
- [x] VTable construction
- [x] Dynamic dispatch
- [x] Diagnostics E13-xxx

**Clause 14 - Concurrency**:

- [x] Permission-based thread safety (no Send/Sync)
- [x] Thread modal type
- [x] Atomic operations
- [x] Synchronization primitives
- [x] Happens-before model
- [x] Diagnostics E14-xxx

**Clause 15 - FFI**:

- [x] Extern declarations
- [x] C compatibility with repr(C)
- [x] Multiple calling conventions
- [x] ABI compliance (System V, Microsoft x64, ARM)
- [x] Diagnostics E15-xxx

**Clause 16 - Comptime**:

- [x] Comptime procedures
- [x] Comptime blocks
- [x] Reflection with [[reflect]]
- [x] Quote and interpolation
- [x] Code generation API
- [x] Diagnostics E16-xxx

### 8.2 Conformance Test Execution

**Process**:

1. Generate tests from specification examples
2. Add tests for each diagnostic code
3. Test all edge cases from clause constraints
4. Validate against spec errata
5. Submit conformance report

**Success Criteria**:

- 95%+ of normative requirements tested
- All diagnostic codes covered
- All examples from spec compile or fail as expected
- No known spec violations

---

## 9. Implementation Priorities

### 9.1 Critical Path Features

**Must-Have for Alpha (Months 1-24)**:

1. Lexer and parser (Phases 1-2)
2. Basic type checking (Phase 4.1-4.3)
3. Simple memory model (subset of Phase 6)
4. Basic codegen (Phase 12.1)
5. Runtime library core (Phase 13.1)

**Must-Have for Beta (Months 25-40)**: 6. Complete generics and behaviors (Phase 5) 7. Full permission system (Phase 6) 8. Contracts and grants (Phase 7) 9. Witnesses (Phase 8) 10. Concurrency (Phase 9)

**Must-Have for 1.0 (Months 41-52)**: 11. FFI (Phase 10) 12. Comptime/reflection (Phase 11) 13. Optimization (Phase 12.3) 14. Tooling (Phase 14.3) 15. Full conformance (Phase 14.4)

### 9.2 Feature Dependencies

```
Lexer → Parser → AST
              ↓
        Name Resolution → Type Checking → Generics
                             ↓              ↓
                    Permission Analysis   Monomorphization
                             ↓              ↓
                     Region Analysis    Behavior Resolution
                             ↓              ↓
                      Move Checking    Witness System
                             ↓              ↓
                     Contract Checking → IR Generation
                                            ↓
                                      LLVM Backend
```

---

## 10. Development Practices

### 10.1 Code Quality

- **Language**: C++17 minimum, C++20 preferred
- **Coding Standard**: LLVM Coding Standards
- **Formatting**: clang-format with LLVM style
- **Linting**: clang-tidy with modernize checks
- **Documentation**: Doxygen comments for all public APIs
- **Code Review**: Required for all changes
- **Testing**: Required for all features

### 10.2 Git Workflow

- **Main branch**: Always stable, all tests passing
- **Feature branches**: One feature per branch
- **Pull requests**: Required with review
- **CI**: Must pass before merge
- **Commit messages**: Reference spec sections

### 10.3 Documentation

**Internal Documentation**:

- Architecture decisions
- Implementation notes per component
- Deviation from spec (if any) with rationale
- Algorithm descriptions

**External Documentation**:

- User guide
- Language tour
- API reference (generated)
- ABI documentation
- Diagnostic catalog (user-facing)

---

## 11. Success Criteria

### 11.1 Technical Success

1. ✅ Compiles all valid Cursive programs per spec
2. ✅ Rejects all invalid programs with correct diagnostics
3. ✅ Passes 95%+ of conformance tests
4. ✅ Generates correct code for all platforms
5. ✅ Zero-cost abstractions validated (performance tests)
6. ✅ Memory safety guarantees hold (no UB in safe code)
7. ✅ ABI compatible with platform conventions

### 11.2 Project Success

1. ✅ Delivered on time (within 10% of estimate)
2. ✅ Within budget (within 20% of estimate)
3. ✅ High code quality (85%+ test coverage)
4. ✅ Comprehensive documentation
5. ✅ Community adoption (1000+ users in year 1)

---

## 12. Post-1.0 Roadmap

### Version 1.1 (Year 2)

- Advanced static verification (SMT solver integration)
- Incremental compilation
- Inline assembly support (§15.4.3)
- SIMD intrinsics (§15.4.4)
- Debugger support (DWARF generation)

### Version 1.2 (Year 3)

- IDE plugins (VS Code, IntelliJ)
- Package manager
- Documentation improvements
- Performance optimizations

### Version 2.0 (Year 4+)

- Language evolution per §1.7
- Breaking changes if needed
- Additional backends (WebAssembly, etc.)

---

## 13. Conclusion

This implementation plan provides a systematic path to building a production-quality Cursive compiler. The phased approach ensures each component is thoroughly tested before building upon it. The 4-year timeline is realistic for a small team building a sophisticated compiler with novel features.

**Key Success Factors**:

1. Strong C++ and LLVM expertise
2. Rigorous testing throughout
3. Close adherence to specification
4. Incremental development with frequent milestones
5. Community engagement and feedback

**Risks**: Managed through prototyping, testing, and conservative estimates.

**Outcome**: A conforming, performant, production-ready Cursive compiler enabling safe systems programming with zero-cost abstractions.

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-08  
**Author**: Cursive Compiler Working Group  
**Status**: Active Planning Document
