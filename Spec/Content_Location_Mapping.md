# Cursive Specification - Content Location Mapping

**Purpose**: Maps each section of Proposed_Organization.md to its current location in the existing specification
**Date**: 2025-11-02
**Status**: Complete Research Results

This document shows where each topic in the proposed reorganization currently exists in the Cursive specification files.

**Legend**:
- ✅ **Complete**: Content fully present and comprehensive
- ⚠️ **Partial**: Content present but incomplete or scattered
- ❌ **Missing**: Content not found in current specification
- 📝 **Placeholder**: File exists but is empty/stub

---

## PART I: INTRODUCTION AND CONFORMANCE

### Section 1.0: Scope
**Status**: ⚠️ Partial
**Current Location**: `01_Foundations.md`
**Lines**: 9-38 (Abstract), see also `00_LLM_Onboarding.md` lines 1-100
**Content**: Design philosophy and language overview present, but formal scope statement, target audiences, and companion document relationships are missing.

### Section 1.1: Normative References
**Status**: ❌ Missing
**Current Location**: Not found
**Notes**: No ISO/IEC standards, Unicode standard references, or other normative references documented.

### Section 1.2: Terms and Definitions
**Status**: ⚠️ Partial
**Current Location**: `01_Foundations.md`
**Lines**: 825-841 (§7 Glossary)
**Content**: Defines 11 key terms; missing comprehensive general/technical terms, symbols.

### Section 1.3: Notation and Conventions
**Status**: ✅ Complete
**Current Location**: `01_Foundations.md`
**Lines**: 57-251 (§1 Notation and Mathematical Foundations)
**Content**: Comprehensive coverage including:
- 1.3.1 Grammar Notation (EBNF): Lines 65-80
- 1.3.2 Metavariables: Lines 83-134
- 1.3.3 Mathematical Notation: Lines 151-192
- 1.3.4 Judgment Forms: Lines 136-149
- 1.3.5 Inference Rule Format: Lines 194-217
- 1.3.6 Typography: Lines 48-54

### Section 1.4: Conformance
**Status**: ⚠️ Partial
**Current Location**: `01_Foundations.md`
**Lines**: 40-54 (Conformance section)
**Content**:
- Basic conformance statement and RFC 2119 keywords present (lines 42-46)
- Missing: Detailed conformance requirements, implementation-defined behavior catalog, undefined behavior catalog, unspecified behavior definitions, diagnosable rules, diagnostic guidance

### Section 1.5: Document Conventions
**Status**: ⚠️ Partial
**Current Location**: `01_Foundations.md`
**Lines**: 48-54 (Document Conventions - minimal)
**Content**: Brief notes only; missing cross-reference policy, examples policy, section organization, grammar presentation policy, forward references, reading guide.

### Section 1.6: Versioning and Evolution
**Status**: ❌ Missing
**Current Location**: Not in specification
**Notes**: Edition system content exists in `Proposed_Organization.md` lines 138-165 but not yet implemented in actual spec.

---

## PART II: BASICS AND PROGRAM MODEL

### Section 2.0: Translation Phases
**Status**: ❌ Missing
**Notes**: Two-phase compilation mentioned in `03_Declarations-and-Scope.md` line 76, but no dedicated translation phases section.

### Section 2.1: Source Text Encoding
**Status**: ✅ Complete
**Current Location**: `01_Foundations.md`
**Lines**: 318-337 (§2.2 Characters and Source Text Encoding)
**Content**: UTF-8 encoding, line endings, BOM handling, file structure all specified.

### Section 2.2: Lexical Elements
**Status**: ✅ Complete
**Current Location**: `01_Foundations.md`
**Lines**: 340-518 (§2.3-2.8)
**Content**:
- 2.2.2 Whitespace: Lines 340-354
- 2.2.3 Comments: Lines 340-354
- 2.2.4 Identifiers: Lines 356-365
- 2.2.5 Keywords: Lines 367-395
- 2.2.6 Literals: Lines 397-481
- 2.2.8 Statement Termination: Lines 483-513
- 2.2.9 Token Formation: Lines 514-518

### Section 2.3: Syntax Notation
**Status**: ✅ Complete
**Current Location**: `01_Foundations.md` + `A1_Grammar.md`
**Lines**: 65-81 (EBNF notation), `A1_Grammar.md` lines 1-561 (complete grammar)
**Content**: Grammar organization, EBNF notation, reference to Appendix A all present.

### Section 2.4: Program Structure
**Status**: ⚠️ Partial
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 25-791 (top-level declarations, module scope)
**Content**: Top-level declarations and forward references covered; missing program entry point, compilation units enumeration.

### Section 2.5: Scopes and Name Lookup
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 723-858 (§3.7), 1377-1491 (§3.11)
**Content**:
- Scope hierarchy: Lines 725-792
- Shadowing rules: Lines 794-858
- Name lookup algorithm: Lines 1379-1442
- Qualified name resolution: Lines 1444-1490

### Section 2.6: Linkage and Program Units
**Status**: ❌ Missing
**Notes**: Visibility modifiers exist (lines 1163-1375) but these are access control, not linkage semantics.

### Section 2.7: Modules (Overview)
**Status**: 📝 Placeholder
**Current Location**: `12_Modules-Packages-and-Imports.md`
**Lines**: 1-10 (file is essentially empty)
**Partial Coverage**: `03_Declarations-and-Scope.md` lines 905-1023 (§3.8 Import/use declarations)

### Section 2.8: Memory and Objects (Overview)
**Status**: ⚠️ Partial
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 1027-1159 (§3.9 Lifetime and Storage Duration)
**Content**: Storage classes, region bindings, RAII cleanup; missing object model, memory locations, alignment.

### Section 2.9: Namespaces
**Status**: ⚠️ Partial
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 859-902 (§3.7.3 Namespace Unification)
**Content**: Single namespace policy only; not separate namespace categories.

---

## PART III: TYPE SYSTEM

### Section 3.0: Type System Foundations
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 26-1211 (§2.0 Type Foundations)
**Content**: Comprehensive coverage of overview, formation rules, type environments, type equality, subtyping (lines 169-249), type safety properties.

### Section 3.1: Primitive Types
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 1212-2050 (§2.1)
**Content**:
- Integer types: Lines 1218-1365
- Floating-point types: Lines 1366-1434
- Boolean type: Lines 1435-1546
- Character type: Lines 1547-1659
- Never type: Lines 1660-1758
- String types: Lines 1759-2050

### Section 3.2: Composite Types
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 2051-3256 (§2.2-2.3)
**Content**:
- Tuples: Lines 2081-2222
- Records: Lines 2223-2427
- Tuple-structs (newtypes): Lines 2428-2565
- Enums: Lines 2596-2767
- Option/Result: Lines 2611-2635 (examples)

### Section 3.3: Collection Types
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 3257-3683 (§2.4), 1759-2050 (strings in §2.1.6)
**Content**:
- Arrays: Lines 3348-3503
- Slices: Lines 3504-3660
- Strings: Lines 1759-2050

### Section 3.4: Function Types
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 4898-5130 (§2.10), also 207-223 (subtyping)
**Content**: Function type syntax, formation, subtyping (contravariant/covariant), effect operation types.

### Section 3.5: Pointer Types
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 3684-3919 (§2.5-2.6)
**Content**:
- Safe pointers: Lines 3684-3779
- Raw pointers: Lines 3780-3919

### Section 3.6: Type Constructors and Generics
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 4087-4897 (§2.9 Parametric Features)
**Content**: Type parameters, const parameters, generic types, generic procedures, associated types (lines 4199-4644), where clauses.

### Section 3.7: Type Bounds and Constraints
**Status**: ⚠️ Partial (scattered)
**Current Location**: `02_Type-System.md`
**Content**:
- Predicate bounds: Lines 4119-4165
- Effect bounds: Lines 4645-4897
- Associated type bounds: Lines 4284-4371
- Region bounds: Lines 972-1027

### Section 3.8: Subtyping and Equivalence
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 169-336 (§2.0.6)
**Content**:
- Subtyping rules: Lines 169-249
- Variance table: Lines 224-237
- Type equivalence: Lines 250-336

### Section 3.9: Type Aliases
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 5131-5216 (§2.11)
**Content**: Type alias declaration, transparent aliases, generic type aliases, cycle detection.

### Section 3.10: Type Introspection
**Status**: ⚠️ Partial
**Current Location**: `02_Type-System.md`
**Lines**: 5217-5328 (§2.12 Self Type and Intrinsics)
**Content**: Self type and intrinsic operations covered; typeof operator and type predicates not explicitly found.

---

## PART IV: EXPRESSIONS AND CONVERSIONS

### Section 4.0: Expression Fundamentals
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 40-266, 4953-5001
**Content**:
- Evaluation model: Lines 161-195
- Value categories: Lines 200-266, 4953-5001
- Expression classification: Lines 40-160

### Section 4.1: Primary Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Content**:
- Literals: Lines 394-407
- Identifiers: Lines 408-479
- Parenthesized: Lines 480-514
- Tuples: Lines 926-980
- Arrays: Lines 848-925
- Blocks: Lines 515-581

### Section 4.2: Postfix Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Content**:
- Field access: Lines 1321-1437
- Index: Lines 1511-1612
- Function calls: Lines 985-1205
- Procedure calls: Lines 1206-1320
- Tuple projection: Lines 1438-1510

### Section 4.3: Unary Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 1869-2077 (§5.4)
**Content**: Logical NOT, negation, dereference, move expressions.

### Section 4.4: Binary Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 2078-2689 (§5.5)
**Content**: Arithmetic, power, comparison, equality, logical, bitwise, shift, range, pipeline operators.

### Section 4.5: Operator Precedence and Associativity
**Status**: ✅ Complete (by reference)
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 388-391
**Content**: References Appendix A.3 for canonical precedence table (15 levels).

### Section 4.6: Structured Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Content**:
- Record construction: Lines 627-748
- Enum construction: Lines 749-847
- Closures: Lines 4071-4345
- If expressions: Lines 2889-3095
- Match expressions: Lines 3096-3603

### Section 4.7: Other Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Content**:
- Pipeline: Lines 2583-2689
- Range: Lines 2484-2582
- Unsafe blocks: References Part XII (lines 145, 1995-1996, 4525-4531)

### Section 4.8: Type Conversions and Coercions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 4489-4532 (§5.13)
**Content**: Explicit casts, numeric conversions (no implicit), permission coercions, modal coercions, unsafe transmute.

### Section 4.9: Constant Expressions
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 4454-4483 (§5.12 Comptime Expressions)
**Content**: Comptime blocks, evaluable expressions, restrictions, use cases.

### Section 4.10: Expression Typing Rules
**Status**: ✅ Complete (distributed)
**Current Location**: `05_Expressions-and-Operators.md`
**Content**: Typing judgments distributed throughout all expression sections using `Γ ⊢ e : τ ! ε` notation.

---

## PART V: STATEMENTS AND CONTROL FLOW

### Section 5.0: Statement Fundamentals
**Status**: ✅ Complete
**Current Location**: `06_Statements-and-Control-Flow.md`
**Content**:
- Classification: Lines 183-220
- Execution: Lines 221-275
- Termination: Lines 2646-2649

### Section 5.1: Simple Statements
**Status**: ✅ Complete
**Current Location**: `06_Statements-and-Control-Flow.md`
**Content**:
- Expression statements: Lines 674-747
- Let statements: Lines 392-490
- Assignment: Lines 491-673
- Return: Lines 917-1067
- Break/Continue: Lines 1068-1313
- Defer: Lines 1314-1455

### Section 5.2: Control Flow Statements
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md` + `06_Statements-and-Control-Flow.md`
**Content**:
- If/If-let: `05_Expressions-and-Operators.md` lines 2889-3095
- While: `05_Expressions-and-Operators.md` lines 3682-3737
- For: `05_Expressions-and-Operators.md` lines 3738-3830
- Loop labels: `06_Statements-and-Control-Flow.md` lines 750-916
- Infinite loops: `05_Expressions-and-Operators.md` lines 3608-3681

### Section 5.3: Pattern Matching
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md`
**Lines**: 3096-3603 (§5.8 Match Expressions)
**Content**: Pattern syntax/semantics, all pattern forms, exhaustiveness (lines 3418-3505), guards (lines 3331-3417), OR patterns (lines 3273-3313).

### Section 5.4: Evaluation Order and Semantics
**Status**: ✅ Complete
**Current Location**: `05_Expressions-and-Operators.md` + `06_Statements-and-Control-Flow.md`
**Content**:
- Evaluation order: `05_Expressions-and-Operators.md` lines 4815-4949 (authoritative)
- Sequencing: `06_Statements-and-Control-Flow.md` lines 2601-2645
- Short-circuit: `05_Expressions-and-Operators.md` lines 2443-2483
- Divergence: `06_Statements-and-Control-Flow.md` lines 979-988, 1170-1178, 2145-2266
- Determinism: `05_Expressions-and-Operators.md` lines 4875-4948

---

## PART VI: MEMORY MODEL AND PERMISSIONS

### Section 6.0: Memory Model Overview
**Status**: ✅ Complete
**Current Location**: `04_Lexical-Permissions.md`
**Lines**: 9-50
**Content**: Memory safety goals, permission-based management, no GC, no borrow checker clearly stated.

### Section 6.1: Objects and Memory Locations
**Status**: ⚠️ Partial
**Current Location**: `02_Type-System.md`
**Lines**: 974-1027 (regions), 1076-1154 (object lifetime basics)
**Content**: Region-based allocation and Copy predicate covered; formal object/memory location definitions not normatively specified.

### Section 6.2: Permission System
**Status**: ✅ Complete
**Current Location**: `04_Lexical-Permissions.md`
**Content**:
- Permission lattice (imm<mut<own): Lines 52-91
- Permission syntax: Lines 56-74
- Permission checking rules: Lines 132-223
- Permission propagation: Lines 659-673
- Permission coercions: Lines 226-266

### Section 6.3: Move Semantics
**Status**: ✅ Complete
**Current Location**: `04_Lexical-Permissions.md` + `02_Type-System.md`
**Content**:
- Move operations: `04_Lexical-Permissions.md` lines 269-314
- Copy types: `02_Type-System.md` lines 1076-1121
- Clone operations: Referenced but deferred
- Dropped values: Multiple references throughout

### Section 6.4: Regions and Lifetimes
**Status**: ✅ Complete
**Current Location**: `04_Lexical-Permissions.md` + `02_Type-System.md`
**Content**:
- Region concept: `04_Lexical-Permissions.md` lines 2293-2340
- Region syntax: `02_Type-System.md` lines 974-984
- Region allocation: `04_Lexical-Permissions.md` lines 2305-2314
- Region escape: Lines 2317-2324, 716-736
- Region stack (Δ): Lines 2295-2301
- Region cleanup: `02_Type-System.md` lines 1015-1023

### Section 6.5: Storage Duration
**Status**: ⚠️ Partial
**Current Location**: `02_Type-System.md`
**Lines**: 648, 650 (effect markers), 974-1027 (regions)
**Content**: Region-based storage and heap/stack references; formal specification of storage duration categories not complete.

### Section 6.6: Memory Layout
**Status**: ❌ Missing
**Notes**: Type sizes, alignment, record/enum layout, layout attributes, padding/packing not specified.

### Section 6.7: Aliasing and Uniqueness
**Status**: ⚠️ Partial
**Current Location**: `04_Lexical-Permissions.md`
**Lines**: 34-35, 78-85
**Content**: Aliasing discipline and uniqueness guarantees implicit in permission semantics; explicit aliasing violation catalog missing.

### Section 6.8: Unsafe Operations
**Status**: ✅ Complete
**Current Location**: `04_Lexical-Permissions.md`
**Lines**: 2343-2380 (§4.14)
**Content**: Unsafe block semantics, safety obligations, unsafe capabilities (raw pointers), effect requirements.

---

## PART VII: DECLARATIONS, SCOPE, AND MODULES

### Section 7.0: Declaration System Overview
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 1-77
**Content**: Complete overview, declaration categories, syntax, visibility/privacy.

### Section 7.1: Variable Declarations
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 80-383
**Content**: Let/var declarations, shadowing, destructuring, type annotations, definite assignment.

### Section 7.2: Function and Procedure Declarations
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md` + `09_Functions.md`
**Content**:
- Overview: `03_Declarations-and-Scope.md` lines 605-672
- Comprehensive spec: `09_Functions.md` lines 1-5590
  - Function declarations: Lines 267-500
  - Procedure declarations: Lines 1580-2148
  - Parameters: Lines 2150-2478
  - Return semantics: Lines 2479-2953

### Section 7.3: Type Declarations
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md` + `02_Type-System.md`
**Content**:
- Overview: `03_Declarations-and-Scope.md` lines 477-602
- Records: `02_Type-System.md` lines 2223-2385
- Enums: Lines 2596-2767
- Type aliases: Lines 5131-5213
- Tuple-structs (newtypes): Lines 2428-2566

### Section 7.4: Predicate Declarations
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 3920-4060 (§2.7 Predicates and Code Reuse)
**Content**: Predicate syntax, items, static semantics, coherence, object safety.

### Section 7.5: Contract Declarations
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md` + `07_Contracts-and-Effects.md`
**Content**:
- Overview: `03_Declarations-and-Scope.md` lines 674-721
- Full spec: `07_Contracts-and-Effects.md` lines 118-467
  - Contract syntax: Lines 126-160
  - Implementation: Lines 162-225
  - Extension: Lines 256-306
  - Associated types: Lines 318-388

### Section 7.6: Effect Declarations
**Status**: ⚠️ Partial (integrated)
**Current Location**: `07_Contracts-and-Effects.md`
**Lines**: 25-31, 89-93, 1086-1105
**Content**: Effect clauses integrated throughout contract system; no dedicated effect declaration section.

### Section 7.7: Modal Declarations
**Status**: ✅ Complete
**Current Location**: `10_Modals.md`
**Lines**: 182-893 (§10.1)
**Content**: Modal type syntax, state declarations, transition declarations, well-formedness.

### Section 7.8: Scope Rules (Detailed)
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 723-902 (§3.7)
**Content**: Scope hierarchy, block/procedure/module scopes, shadowing rules, namespace unification.

### Section 7.9: Name Resolution (Detailed)
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 1377-1492 (§3.11)
**Content**: 5-step resolution algorithm, qualified name resolution, ambiguity resolution.

### Section 7.10: Modules and Imports (Detailed)
**Status**: 📝 Placeholder
**Current Location**: `12_Modules-Packages-and-Imports.md` (empty)
**Partial**: `03_Declarations-and-Scope.md` lines 905-1023 (§3.8 Import/use declarations)
**Content**: Import syntax and scoping rules present; full module system specification missing.

### Section 7.11: Visibility
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 1162-1374 (§3.10)
**Content**: Visibility modifiers (public/internal/private/protected), default policy, field-level visibility, access checking.

### Section 7.12: Predeclared Identifiers
**Status**: ✅ Complete
**Current Location**: `03_Declarations-and-Scope.md`
**Lines**: 1496-1589 (§3.12)
**Content**: Predeclared types, constants, procedures, protection rules.

---

## PART VIII: PREDICATES AND DISPATCH

### Section 8.0-8.7: Predicate System
**Status**: ⚠️ Partial (~40% coverage)
**Current Location**: `02_Type-System.md`
**Content**:
- §2.7 Predicates and Code Reuse (lines 3920-4060): Predicate fundamentals, syntax, semantics, coherence basics, object safety
- §2.8 Marker Predicates (lines 4062-4082): Copy, Clone, Send, Sync, auto-implementation
- §2.9.4 Effect Polymorphism (lines 4645-4894): Comprehensive

**Missing**: Static dispatch details, predicate implementation mechanics (blanket implementations), detailed coherence rules, dynamic dispatch/vtables.

---

## PART IX: EFFECT SYSTEM

### Section 9.0-9.7: Effect System
**Status**: ⚠️ Partial (~35% coverage)
**Current Location**: `02_Type-System.md` + `07_Contracts-and-Effects.md` + `05_Expressions-and-Operators.md`
**Content**:
- Effect polymorphism: `02_Type-System.md` lines 4645-4894 (comprehensive)
- Contract foundations: `07_Contracts-and-Effects.md` lines 15-78
- Effect propagation: Lines 965-978
- Effect composition: `05_Expressions-and-Operators.md` lines 4759-4811
- Diagnostics: `07_Contracts-and-Effects.md` lines 1086-1115

**Missing**: Effect definitions, effect handlers, built-in effects catalog, detailed tracking rules. **Critical gap**: Section 7.3 (Effect Clauses) missing from specification.

---

## PART X: CONTRACT SYSTEM

### Section 10.0-10.7: Contracts
**Status**: ✅ Complete
**Current Location**: `07_Contracts-and-Effects.md` (primary)
**Content**:
- Overview: Lines 15-113
- Behavioral contracts: Lines 116-614
- Preconditions (must): Lines 617-719
- Postconditions (will): Lines 721-823
- Type invariants (where): Lines 824-960
- Verification modes: Lines 683-684, 791-794 + `01_Foundations.md` lines 735, 761, 791
- Clause composition: Lines 963-1054
- Integration: Lines 106-112, 490-542, 600-603
- Diagnostics: Lines 1057-1202

---

## PART XI: TYPE-LEVEL ADVANCED FEATURES

### Section 11.1: Modal Types and State Machines
**Status**: ✅ Complete
**Current Location**: `10_Modals.md`
**Lines**: 1-4837 (entire file)
**Content**: Complete specification including declarations, states, transitions, instantiation, state-dependent operations, patterns, state inference, type checking, integration.

### Section 11.2: Contract Witnesses
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md`
**Lines**: 5329-6029+ (§2.13)
**Content**: Concept, construction, types, operations, memory management, effect polymorphism, modal state tracking, comptime optimization.

### Section 11.3: Type Inference and Holes
**Status**: ✅ Complete
**Current Location**: `08_Holes-and-Inference.md`
**Lines**: 1-896 (entire file)
**Content**: Overview, hole syntax (`_?`, `@_?`), constraint generation/solving, type/permission/modal/effect hole resolution, limitations, elaboration.

---

## PART XII: METAPROGRAMMING

### Section 12.0-12.4: Metaprogramming
**Status**: ✅ Complete
**Current Location**: `11_Metaprogramming.md`
**Lines**: 1-2875 (entire file)
**Content**:
- Overview: Lines 1-113
- Comptime execution: Lines 115-573
- Type reflection: Lines 576-907
- Code generation: Lines 910-1797
  - Quote expressions: Lines 1397-1559
- Attributes: Lines 2536-2590 (grammar), distributed throughout
- Runtime type information: Lines 1858-1932

---

## PART XIII: CONCURRENCY AND MEMORY ORDERING

### Section 13.0-13.4: Concurrency
**Status**: ❌ Missing (deferred)
**Current Location**: `13_Memory-Permissions-and-Concurrancy.md`
**Lines**: 1-10 (placeholder only)
**Content**: File states content is deferred, remains unwritten. Data races mentioned only in `01_Foundations.md` line 38 (informative).

---

## PART XIV: INTEROPERABILITY AND UNSAFE

### Section 14.0: Interoperability Overview
**Status**: ⚠️ Minimal
**Content**: Outlined in `Proposed_Organization.md` but not in actual spec.

### Section 14.1: Foreign Function Interface
**Status**: ✅ Complete
**Current Location**: `09_Functions.md`
**Lines**: 1200-1349 (§9.2.7)
**Content**:
- External declarations: Lines 1207-1213
- Calling conventions: Lines 1215-1217
- FFI types: Lines 1251-1260
- FFI safety: Lines 1228-1248
- Name mangling: Lines 1264-1287
- Variadic procedures: Lines 1303-1316, 2422-2474

### Section 14.2: Unsafe Operations in FFI Context
**Status**: ✅ Complete
**Current Location**: `09_Functions.md` + `04_Lexical-Permissions.md`
**Content**:
- Unsafe procedures: `09_Functions.md` lines 1120-1190
- Unsafe blocks in FFI: Lines 1128-1168
- Raw pointer requirements: `04_Lexical-Permissions.md` lines 181, 2347

### Section 14.2.4: Raw Pointers from External Code
**Status**: ✅ Complete
**Current Location**: `02_Type-System.md` + `05_Expressions-and-Operators.md`
**Content**:
- Raw pointer types: `02_Type-System.md` lines 3786-3891
- Dereference operator: `05_Expressions-and-Operators.md` lines 1994-2016

### Section 14.3: C Compatibility
**Status**: ⚠️ Partial
**Current Location**: `09_Functions.md` + `02_Type-System.md`
**Content**:
- C string handling: `09_Functions.md` lines 1327-1334
- C variadic procedures: Lines 1303-1316, 2422-2474
- C type representations: `02_Type-System.md` line 2387 (`[[repr(C)]]`)
- **Missing**: C macros/preprocessing

### Section 14.4: Platform-Specific Features
**Status**: ⚠️ Mentioned only
**Current Location**: Scattered references
**Content**:
- Inline assembly: `09_Functions.md` line 1128 (mentioned)
- Intrinsics: `02_Type-System.md` lines 5308-5323
- SIMD: `11_Metaprogramming.md` lines 2147-2276 (discussion)
- **Missing**: Target architecture, OS interface, detailed specifications

### Section 14.5: Linkage and Symbol Visibility
**Status**: ⚠️ Partial
**Current Location**: `09_Functions.md` + `01_Foundations.md`
**Content**:
- Name mangling: `09_Functions.md` lines 1264-1287
- Attributes: `01_Foundations.md` lines 794-796 (`[[no_mangle]]`, `[[alias]]`)
- **Missing**: Linkage semantics, static/dynamic linking

### Section 14.6: ABI Considerations
**Status**: ❌ Stub only
**Current Location**: `09_Functions.md`
**Line**: 2635 (section header only, no content)

---

## ANNEXES

### ANNEX A: COMPLETE GRAMMAR
**Status**: ✅ Complete (95%)
**Current Location**: `A1_Grammar.md`
**Lines**: 1-561
**Content**:
- A.1 Lexical Grammar: Lines 5-76
- A.2 Type Grammar: Lines 80-136
- A.3 Pattern Grammar: Lines 144-173
- A.4 Expression Grammar: Lines 175-294
- A.5 Statement Grammar: Lines 298-359
- A.6 Declaration Grammar: Lines 360-491
- A.7 Contract Grammar: Lines 493-510
- A.8 Assertion Grammar: Lines 511-529
- A.9 Effect Grammar: Lines 531-560
- **Missing**: A.10 Attribute Grammar, A.11 Grammar Reference (consolidated)

### ANNEX B: PORTABILITY AND BEHAVIOR
**Status**: ⚠️ Placeholder (10%)
**Current Location**: `A2_Well-Formedness-Rules.md`
**Lines**: 1-177 (marked "Deferred to Post-v1.0")
**Content**: Cross-references to inline rules; missing dedicated behavior classification, implementation-defined catalog, undefined behavior catalog (UB-001, etc.), unspecified behavior catalog.

### ANNEX C: FORMAL SEMANTICS
**Status**: ✅ Scattered (70%)
**Current Locations**: Multiple files
**Content**:
- Notation and judgment forms: `01_Foundations.md` lines 57-253, 520-726
- Type system rules: `02_Type-System.md` (throughout)
- Permission semantics: `04_Lexical-Permissions.md` (throughout)
- Effect semantics: `07_Contracts-and-Effects.md` (throughout)
- Contract semantics: `07_Contracts-and-Effects.md` (throughout)
- Modal semantics: `10_Modals.md` lines 84-100+
- **Missing**: Consolidated annex, meta-theory (type safety theorems)

### ANNEX D: IMPLEMENTATION GUIDANCE
**Status**: ⚠️ Scattered (40%)
**Current Location**: `01_Foundations.md` + `Proposed_Organization.md`
**Content**:
- AST representation: `01_Foundations.md` lines 520-726
- Diagnostic format: Lines 253-313
- Edition system: `Proposed_Organization.md` lines 1363-1376
- **Missing**: Optimization opportunities, consolidated diagnostic quality guidance

### ANNEX E: STANDARD LIBRARY OVERVIEW
**Status**: 📝 Empty
**Current Location**: `16_The-Standard-Library.md`
**Lines**: 1 (empty file)
**Note**: Deferred to separate "Cursive Standard Library Specification" document.

### ANNEX F: GLOSSARY
**Status**: ⚠️ Header only (5%)
**Current Location**: `01_Foundations.md`
**Line**: 825 (header only)
**Note**: Definitions scattered using "Definition X.Y" format; alphabetical glossary not compiled.

---

## Summary Statistics

**Content Coverage by Part:**
- PART I (Introduction): 35% (notation complete, conformance partial, scope/references missing)
- PART II (Basics): 75% (lexical/scopes complete, translation phases/linkage missing)
- PART III (Types): 95% (comprehensive, minor gaps in introspection)
- PART IV (Expressions): 100% (complete and comprehensive)
- PART V (Statements): 100% (complete and comprehensive)
- PART VI (Memory): 80% (permissions/regions complete, layout missing)
- PART VII (Declarations): 90% (comprehensive, modules file empty)
- PART VIII (Predicates): 40% (basics present, dispatch details missing)
- PART IX (Effects): 35% (polymorphism good, handlers/clauses missing)
- PART X (Contracts): 100% (complete and comprehensive)
- PART XI (Advanced): 100% (modals/witnesses/holes all complete)
- PART XII (Metaprogramming): 100% (complete and comprehensive)
- PART XIII (Concurrency): 0% (deferred)
- PART XIV (Interop): 60% (FFI complete, platform/ABI partial/missing)
- ANNEXES: 50% (grammar complete, others partial/missing)

**Overall Specification Completeness**: ~75%

**Key Strengths**:
- Excellent coverage of type system, expressions, statements
- Complete specifications for permissions, contracts, modals
- Comprehensive metaprogramming and code generation
- Strong formal foundations

**Key Gaps**:
- Concurrency (entirely deferred)
- Effect system (handlers and detailed tracking)
- Predicate dispatch details
- Memory layout specifications
- Platform-specific features
- Behavior catalogs (UB, implementation-defined, unspecified)
- Consolidated annexes

---

**End of Content Location Mapping**
