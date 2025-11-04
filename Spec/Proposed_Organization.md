# Cursive Language Specification — Organization

**Version**: 3.1 (Standards-Aligned + Best Practice Enhancements)
**Date**: 2025-11-02
**Status**: Proposal (ISO-Ready)
**Purpose**: Formal specification aligned with ISO/ECMA standards practice
**Review**: Evaluated against C++, C, Rust, Go, Zig standards (Grade: A, ISO-ready with industry best practices integrated)

---

## Organization Principles

1. **Standards Alignment**: Structure follows ISO/IEC and ECMA language specification conventions
2. **Normative Precision**: Uses ISO normative language (shall/shall not/should/may per ISO/IEC Directives)
3. **Progressive Complexity**: Foundational concepts precede advanced features, with minimal forward references (documented in §1.5.5)
4. **Modular Files**: Each major section (X.Y) has its own file for maintainability and version control
5. **Clear Conformance Boundary**: Explicit separation of normative and informative content
6. **Complete Coverage**: All essential language aspects specified with precision
7. **Explicit Dependencies**: Forward references and circular dependencies are documented with clear cross-references
8. **Usability and Clarity**: Complex rules accompanied by informative examples; grammar presented both in-context and consolidated (Annex A)
9. **Future-Proofing**: Edition system enables language evolution while maintaining backward compatibility

---

## Companion Documents

The following documents accompany this specification:

**Normative (Separate Document):**
- **Cursive Standard Library Specification** - Detailed specification of the standard library; maintained separately from language specification for independent evolution

**Non-Normative:**
- **Cursive Programming Guide** - Practical guidance, patterns, idioms, and examples for language users
- **Cursive LLM Development Guide** - AI-assisted development and code generation guidance
- **Cursive Compiler Diagnostics Reference** - Complete catalog of compiler error codes and warnings with suggested diagnostic messages

---

## Part Structure

### **PART I: INTRODUCTION AND CONFORMANCE** *(Normative)*

#### **Section 1.0: Scope**
**File**: `01-0_Scope.md`

- 1.0.1 Purpose of this specification
- 1.0.2 Scope and applicability
- 1.0.3 Target audiences (implementers, tool developers, language designers)
- 1.0.4 Relationship to companion documents
  - Standard Library Specification (normative, separate)
  - Programming Guide (informative)
  - LLM Development Guide (informative)
  - Compiler Diagnostics Reference (informative)

#### **Section 1.1: Normative References**
**File**: `01-1_Normative-References.md`

- 1.1.1 ISO/IEC standards
- 1.1.2 Unicode standard
- 1.1.3 Other normative references

#### **Section 1.2: Terms and Definitions**
**File**: `01-2_Terms-Definitions.md`

- 1.2.1 General terms
- 1.2.2 Language-specific terms
- 1.2.3 Technical terms
- 1.2.4 Symbols and abbreviated terms

#### **Section 1.3: Notation and Conventions**
**File**: `01-3_Notation-Conventions.md`

- 1.3.1 Grammar Notation (EBNF)
- 1.3.2 Metavariables
- 1.3.3 Mathematical Notation
- 1.3.4 Judgment Forms
- 1.3.5 Inference Rule Format
- 1.3.6 Typography and Formatting Conventions

#### **Section 1.4: Conformance**
**File**: `01-4_Conformance.md`

- 1.4.1 Conformance Requirements
- 1.4.2 Normative Language (shall, shall not, should, may per ISO/IEC Directives)
  - Requirements (shall/shall not)
  - Recommendations (should/should not)
  - Permissions (may)
  - Capability vs. requirement (can vs. shall)
  - Note on RFC 2119 equivalence
- 1.4.3 Conforming Implementation
- 1.4.4 Conforming Program
- 1.4.5 Implementation-Defined Behavior
- 1.4.6 Undefined Behavior
  - Definition and consequences
  - Labeling convention [UB] or [Undefined Behavior]
  - Cross-reference to Annex B.2 catalog (format: [UB: B.2 #N])
  - Common causes (out-of-bounds, null deref, data races, region escape, use-after-move)
  - Audit requirement: all UB instances shall be listed in Annex B.2
- 1.4.7 Unspecified Behavior
- 1.4.8 Diagnosable Rules
  - Ill-formed programs (diagnostic required)
  - Ill-formed, no diagnostic required (IFNDR)
  - Diagnostic format is implementation-defined
- 1.4.9 Diagnostic Guidance
  - Relationship to Compiler Diagnostics Reference (companion, non-normative)
  - Diagnostic quality expectations
  - Implementation documentation requirements

#### **Section 1.5: Document Conventions**
**File**: `01-5_Document-Conventions.md`

- 1.5.1 Cross-References
  - Cross-reference format (§X.Y, Part X §Y.Z, Annex X §Y)
  - Cross-reference requirements (every dependency shall cite target section)
  - Bidirectional cross-referencing (key definitions list referencing sections)
  - Digital hyperlinks required in electronic versions
- 1.5.1a Cross-Reference Validation
  - All references shall resolve to existing sections before publication
  - Automated validation tooling required
  - Dead reference prohibition
  - Orphan detection (sections with zero inbound references flagged)
  - Reference resolution tables
- 1.5.2 Examples (Informative)
  - Example purpose and status (informative, do not impose normative requirements)
  - Example coverage requirements (non-trivial semantics should include examples; complex features must include examples)
  - Example formatting (fenced code blocks with ```cursive, ✅/❌ markers)
  - Example quality (minimal yet complete, consistent naming)
- 1.5.3 Notes (Informative)
- 1.5.4 Section Organization
- 1.5.4a Grammar Presentation
  - Dual presentation: in-context (within semantic sections) and consolidated (Annex A)
  - In-context grammar requirements (sections introducing syntactic constructs shall include relevant grammar)
  - In-line grammar formatting (EBNF notation per §1.3.1, 2-5 productions typical)
  - Grammar synchronization (in-line must match Annex A; Annex A is authoritative)
- 1.5.5 Forward References and Dependencies
- 1.5.6 Reading Guide for Different Audiences

#### **Section 1.6: Versioning and Evolution**
**File**: `01-6_Versioning-Evolution.md`

- 1.6.1 Semantic Versioning
  - Version format `MAJOR.MINOR.PATCH`
  - This specification defines language version 1.0.0
  - Program manifests declare a minimum language version (`version = "1.0"`)
  - Compatibility rules (1.x remains backward compatible; major bumps can break)
  - Implementation requirements for advertising supported versions
- 1.6.2 Feature Stability
  - Stable features (no `[Experimental]`/`[Preview]`; semantics stable within the major line)
  - Experimental features (marked `[Experimental]`; require explicit opt-in)
  - Preview features preparing for the next major release (`[Preview: v2.0]`)
  - Stabilization process (feedback, formal text, implementation validation)
- 1.6.3 Deprecation Policy
  - Deprecation marking (`[Deprecated: vMAJOR.MINOR]`)
  - Deprecation timeline (warn in minor releases before removal in the next major)
  - Migration guidance and automation expectations
- 1.6.4 Relationship to ISO Standardization
  - ISO revisions align with major Cursive versions
  - Minor/patch releases can occur between ISO ballots
- 1.6.5 Implementation Guidance
  - Feature gates for experimental/preview capabilities (e.g., `#![feature(name)]`)
  - Language-version selection flags and manifest handling
  - Migration tooling for major-version upgrades

---

### **PART II: BASICS AND PROGRAM MODEL** *(Normative)*

#### **Section 2.0: Translation Phases**
**File**: `02-0_Translation-Phases.md`

- 2.0.1 Phase Overview
- 2.0.2 Source File Processing
- 2.0.3 Lexical Analysis
- 2.0.4 Syntactic Analysis
- 2.0.5 Semantic Analysis
- 2.0.6 Code Generation

#### **Section 2.1: Source Text Encoding**
**File**: `02-1_Source-Encoding.md`

- 2.1.1 Character Encoding (UTF-8)
- 2.1.2 Source File Structure
- 2.1.3 Line Endings
- 2.1.4 Byte Order Mark Handling

#### **Section 2.2: Lexical Elements**
**File**: `02-2_Lexical-Elements.md`

- 2.2.1 Input Elements
- 2.2.2 Whitespace
- 2.2.3 Comments
- 2.2.4 Identifiers
- 2.2.5 Keywords
- 2.2.6 Literals
- 2.2.7 Operators and Punctuation
- 2.2.8 Statement Termination
- 2.2.9 Token Formation

#### **Section 2.3: Syntax Notation**
**File**: `02-3_Syntax-Notation.md`

- 2.3.1 Grammar Organization
  - Complete grammar in Annex A (normative)
  - In-context grammar snippets throughout specification (informative illustrations)
  - For complete production rules, consult Annex A
  - EBNF notation defined in §1.3.1
  - Grammar presentation policy in §1.5.4a
- 2.3.2 Syntax Categories
- 2.3.3 Reference to Appendix A (Complete Grammar)

#### **Section 2.4: Program Structure**
**File**: `02-4_Program-Structure.md`

- 2.4.1 Compilation Units
- 2.4.2 Top-Level Constructs
- 2.4.3 Declaration Order
- 2.4.4 Program Entry Point

#### **Section 2.5: Scopes and Name Lookup**
**File**: `02-5_Scopes-Name-Lookup.md`

- 2.5.1 Scope Concept
- 2.5.2 Lexical Scoping
- 2.5.3 Scope Nesting
- 2.5.4 Name Binding
- 2.5.5 Name Lookup Algorithm
- 2.5.6 Qualified Name Lookup
- 2.5.7 Unqualified Name Lookup
- 2.5.8 Shadowing

#### **Section 2.6: Linkage and Program Units**
**File**: `02-6_Linkage-Program-Units.md`

- 2.6.1 Linkage Concept
- 2.6.2 External Linkage
- 2.6.3 Internal Linkage
- 2.6.4 No Linkage
- 2.6.5 One Definition Rule

#### **Section 2.7: Modules (Overview)**
**File**: `02-7_Modules-Overview.md`

- 2.7.1 Module Concept
- 2.7.2 Module Declaration
- 2.7.3 Import and Export
- 2.7.4 Module Visibility
- 2.7.5 Cross-reference to §7.10 for detailed rules

#### **Section 2.8: Memory and Objects (Overview)**
**File**: `02-8_Memory-Objects-Overview.md`

- 2.8.1 Object Model
- 2.8.2 Memory Locations
- 2.8.3 Object Lifetime (Overview)
- 2.8.4 Storage Duration (Overview)
- 2.8.5 Alignment
- 2.8.6 Cross-reference to Part VI for detailed memory model

#### **Section 2.9: Namespaces**
**File**: `02-9_Namespaces.md`

- 2.9.1 Namespace Categories
- 2.9.2 Type Namespace
- 2.9.3 Value Namespace
- 2.9.4 Module Namespace

---

### **PART III: TYPE SYSTEM** *(Normative)*

#### **Section 3.0: Type System Foundations**
**File**: `03-0_Type-Foundations.md`

- 3.0.1 Type System Overview
- 3.0.2 Type Formation Rules
- 3.0.3 Type Environments (Γ)
- 3.0.4 Type Equality and Equivalence
- 3.0.5 Subtyping Relation
- 3.0.6 Type Safety Properties

#### **Section 3.1: Subtyping and Equivalence**
**File**: `03-1_Subtyping-Equivalence.md`

- 3.1.1 Type Equivalence Rules
- 3.1.2 Subtyping Rules
- 3.1.3 Variance
- 3.1.4 Type Compatibility

#### **Section 3.2: Primitive Types**
**File**: `03-2_Primitive-Types.md`

- 3.2.1 Integer Types
- 3.2.2 Floating-Point Types
- 3.2.3 Boolean Type
- 3.2.4 Character Type
- 3.2.5 Unit Type
- 3.2.6 Never Type
- 3.2.7 Range Types

#### **Section 3.3: Composite Types**
**File**: `03-3_Composite-Types.md`

- 3.3.1 Product Types (Tuples, Records, Newtypes)
- 3.3.2 Sum Types (Enums, Option, Result)
- 3.3.3 Union Types

#### **Section 3.4: Collection Types**
**File**: `03-4_Collection-Types.md`

- 3.4.1 Arrays
- 3.4.2 Slices
- 3.4.3 Strings

#### **Section 3.5: Function Types**
**File**: `03-5_Function-Types.md`

- 3.5.1 Function Type Syntax
- 3.5.2 Function Type Formation
- 3.5.3 Function Subtyping
- 3.5.4 Closure Types

#### **Section 3.6: Pointer Types**
**File**: `03-6_Pointer-Types.md`

- 3.6.1 Safe Pointers
- 3.6.2 Raw Pointers

#### **Section 3.7: Predicates**
**File**: `03-7_Predicates.md`

- 3.7.1 Predicate Concept and Purpose
- 3.7.2 Marker Predicates (Copy, Send, Sync, Sized)
- 3.7.3 Predicate Bounds and Constraints
- 3.7.4 Standard Library Predicates (Preview)

#### **Section 3.8: Type Constructors and Generics**
**File**: `03-8_Generics.md`

- 3.8.1 Type Parameters
- 3.8.2 Generic Types
- 3.8.3 Generic Functions
- 3.8.4 Const Generics
- 3.8.5 Associated Types

#### **Section 3.9: Type Bounds and Constraints**
**File**: `03-9_Type-Bounds.md`

- 3.9.1 Predicate Bounds
- 3.9.2 Lifetime Bounds
- 3.9.3 Grant Bounds
- 3.9.4 Where Clauses

#### **Section 3.10: Type Aliases**
**File**: `03-10_Type-Aliases.md`

- 3.10.1 Type Alias Declaration
- 3.10.2 Transparent vs Opaque Aliases
- 3.10.3 Generic Type Aliases

#### **Section 3.11: Type Introspection**
**File**: `03-11_Type-Introspection.md`

- 3.11.1 typeof operator
- 3.11.2 Type predicates
- 3.11.3 Compile-time type queries

---

### **PART IV: EXPRESSIONS AND CONVERSIONS** *(Normative)*

#### **Section 4.0: Expression Fundamentals**
**File**: `04-0_Expression-Fundamentals.md`

- 4.0.1 Expression Evaluation Model
- 4.0.2 Value Categories (lvalue, rvalue, place) *Note: See §6.2 for interaction with permission system*
- 4.0.3 Expression Classification
- 4.0.4 Type and Effect of Expressions

#### **Section 4.1: Primary Expressions**
**File**: `04-1_Primary-Expressions.md`

- 4.1.1 Literal Expressions
- 4.1.2 Identifier Expressions
- 4.1.3 Parenthesized Expressions
- 4.1.4 Tuple Expressions
- 4.1.5 Array Literals
- 4.1.6 Block Expressions

#### **Section 4.2: Postfix Expressions**
**File**: `04-2_Postfix-Expressions.md`

- 4.2.1 Field Access
- 4.2.2 Index Expressions
- 4.2.3 Function Calls
- 4.2.4 Procedure Calls
- 4.2.5 Type Ascription

#### **Section 4.3: Unary Expressions**
**File**: `04-3_Unary-Expressions.md`

- 4.3.1 Unary Operators
- 4.3.2 Dereferencing
- 4.3.3 Address-of
- 4.3.4 Negation
- 4.3.5 Logical NOT

#### **Section 4.4: Binary Expressions**
**File**: `04-4_Binary-Expressions.md`

- 4.4.1 Arithmetic Operators
- 4.4.2 Comparison Operators
- 4.4.3 Logical Operators
- 4.4.4 Bitwise Operators
- 4.4.5 Range Operators

#### **Section 4.5: Operator Precedence and Associativity**
**File**: `04-5_Operator-Precedence.md`

- 4.5.1 Precedence Table
- 4.5.2 Associativity Rules
- 4.5.3 Parenthesization

#### **Section 4.6: Structured Expressions**
**File**: `04-6_Structured-Expressions.md`

- 4.6.1 Record Construction
- 4.6.2 Enum Construction
- 4.6.3 Closure Expressions
- 4.6.4 If Expressions
- 4.6.5 Match Expressions

#### **Section 4.7: Other Expressions**
**File**: `04-7_Other-Expressions.md`

- 4.7.1 Pipeline Expressions
- 4.7.2 Range Expressions
- 4.7.3 Unsafe Blocks

#### **Section 4.8: Type Conversions and Coercions**
**File**: `04-8_Conversions-Coercions.md`

- 4.8.1 Implicit Conversions
- 4.8.2 Explicit Conversions (as operator)
- 4.8.3 Coercion Sites
- 4.8.4 Numeric Conversions
- 4.8.5 Pointer Conversions
- 4.8.6 Reference Conversions
- 4.8.7 Unsizing Coercions
- 4.8.8 Well-formedness of Conversions

#### **Section 4.9: Constant Expressions**
**File**: `04-9_Constant-Expressions.md`

- 4.9.1 Constant Expression Definition
- 4.9.2 Constant Evaluation Context
- 4.9.3 Constant Functions
- 4.9.4 Compile-time Evaluation Rules
- 4.9.5 Constant Propagation

#### **Section 4.10: Expression Typing Rules**
**File**: `04-10_Expression-Typing.md`

- 4.10.1 Typing Judgments
- 4.10.2 Type Checking Algorithm
- 4.10.3 Type Inference Integration
- 4.10.4 Ill-formed Expressions

---

### **PART V: STATEMENTS AND CONTROL FLOW** *(Normative)*

#### **Section 5.0: Statement Fundamentals**
**File**: `05-0_Statement-Fundamentals.md`

- 5.0.1 Statement Classification
- 5.0.2 Statement Execution
- 5.0.3 Statement Termination

#### **Section 5.1: Simple Statements**
**File**: `05-1_Simple-Statements.md`

- 5.1.1 Expression Statements
- 5.1.2 Let Statements
- 5.1.3 Assignment Statements
- 5.1.4 Return Statements
- 5.1.5 Break and Continue
- 5.1.6 Defer Statements

#### **Section 5.2: Control Flow Statements**
**File**: `05-2_Control-Flow-Statements.md`

- 5.2.1 If Statements
- 5.2.2 If-Let Statements
- 5.2.3 While Loops
- 5.2.4 For Loops
- 5.2.5 Loop Labels
- 5.2.6 Infinite Loops

#### **Section 5.3: Pattern Matching**
**File**: `05-3_Pattern-Matching.md`

- 5.3.1 Pattern Syntax
- 5.3.2 Pattern Semantics
- 5.3.3 Irrefutable Patterns
- 5.3.4 Refutable Patterns
- 5.3.5 Pattern Exhaustiveness
- 5.3.6 Pattern Guards
- 5.3.7 Or Patterns

#### **Section 5.4: Evaluation Order and Semantics**
**File**: `05-4_Evaluation-Semantics.md`

- 5.4.1 Evaluation Order
- 5.4.2 Sequencing
- 5.4.3 Short-circuit Evaluation
- 5.4.4 Divergence
- 5.4.5 Determinism Guarantees

---

### **PART VI: MEMORY MODEL AND PERMISSIONS** *(Normative)*

#### **Section 6.0: Memory Model Overview**
**File**: `06-0_Memory-Model-Overview.md`

- 6.0.1 Memory Safety Goals
- 6.0.2 Permission-Based Memory Management
- 6.0.3 No Garbage Collection
- 6.0.4 No Borrow Checker
- 6.0.5 Relationship to Part II §2.8

#### **Section 6.1: Objects and Memory Locations**
**File**: `06-1_Objects-Memory.md`

- 6.1.1 Object Definition
- 6.1.2 Memory Location
- 6.1.3 Object Creation
- 6.1.4 Object Destruction
- 6.1.5 Object Lifetime

#### **Section 6.2: Permission System**
**File**: `06-2_Permission-System.md`

- 6.2.1 Permission Lattice (imm < mut < own)
- 6.2.2 Permission Syntax
- 6.2.3 Permission Declarations
- 6.2.4 Permission Checking Rules
- 6.2.5 Permission Propagation
- 6.2.6 Permission Inference
- 6.2.7 Permission Upgrades and Downgrades

#### **Section 6.3: Move Semantics**
**File**: `06-3_Move-Semantics.md`

- 6.3.1 Move Operations
- 6.3.2 Move Checking
- 6.3.3 Copy Types
- 6.3.4 Clone Operations
- 6.3.5 Dropped Values

#### **Section 6.4: Regions and Lifetimes**
**File**: `06-4_Regions-Lifetimes.md`

- 6.4.1 Region Concept
- 6.4.2 Region Syntax
- 6.4.3 Region Allocation
- 6.4.4 Region Escape Analysis
- 6.4.5 Region Stack (Δ)
- 6.4.6 Region Cleanup

#### **Section 6.5: Storage Duration**
**File**: `06-5_Storage-Duration.md`

- 6.5.1 Static Storage
- 6.5.2 Function Storage
- 6.5.3 Region Storage
- 6.5.4 Heap Storage

#### **Section 6.6: Memory Layout**
**File**: `06-6_Memory-Layout.md`

- 6.6.1 Type Sizes and Alignment
- 6.6.2 Record Layout
- 6.6.3 Enum Layout
- 6.6.4 Layout Attributes
- 6.6.5 Padding and Packing

#### **Section 6.7: Aliasing and Uniqueness**
**File**: `06-7_Aliasing-Uniqueness.md`

- 6.7.1 Aliasing Rules
- 6.7.2 Aliasing Violations
- 6.7.3 Uniqueness Guarantees

#### **Section 6.8: Unsafe Operations**
**File**: `06-8_Unsafe-Operations.md`

- 6.8.1 Unsafe Block Semantics
- 6.8.2 Safety Obligations
- 6.8.3 Unsafe Capabilities
- 6.8.4 When Unsafe is Required
- 6.8.5 Cross-reference to §14.2 for FFI-specific unsafe usage

---

### **PART VII: DECLARATIONS, SCOPE, AND MODULES** *(Normative)*

#### **Section 7.0: Declaration System Overview**
**File**: `07-0_Declaration-Overview.md`

- 7.0.1 Declaration Categories
- 7.0.2 Declaration Syntax
- 7.0.3 Visibility and Privacy

#### **Section 7.1: Variable Declarations**
**File**: `07-1_Variable-Declarations.md`

- 7.1.1 Let Declarations
- 7.1.2 Var Declarations
- 7.1.3 Shadow Declarations
- 7.1.4 Destructuring Declarations
- 7.1.5 Type Annotations

#### **Section 7.2: Function and Procedure Declarations**
**File**: `07-2_Function-Procedure-Declarations.md`

- 7.2.1 Function Declarations
- 7.2.2 Procedure Declarations
- 7.2.3 Parameters
- 7.2.4 Return Types
- 7.2.5 Generic Functions
- 7.2.6 Local Functions (Prohibition)

#### **Section 7.3: Type Declarations**
**File**: `07-3_Type-Declarations.md`

- 7.3.1 Record Declarations
- 7.3.2 Enum Declarations
- 7.3.3 Type Alias Declarations
- 7.3.4 Newtype Declarations

#### **Section 7.4: Predicate Declarations**
**File**: `07-4_Predicate-Declarations.md`

- 7.4.1 Predicate Syntax
- 7.4.2 Predicate Items
- 7.4.3 Associated Types
- 7.4.4 Associated Constants

#### **Section 7.5: Contract Declarations**
**File**: `07-5_Contract-Declarations.md`

- 7.5.1 Contract Syntax
- 7.5.2 Contract Procedures
- 7.5.3 Contract Associated Types

#### **Section 7.6: Effect Declarations**
**File**: `07-6_Effect-Declarations.md`

- 7.6.1 Effect Syntax
- 7.6.2 Effect Operations
- 7.6.3 Marker Effects

#### **Section 7.7: Modal Declarations**
**File**: `07-7_Modal-Declarations.md`

- 7.7.1 Modal Type Syntax
- 7.7.2 State Declarations
- 7.7.3 Transition Declarations

#### **Section 7.8: Scope Rules (Detailed)**
**File**: `07-8_Scope-Rules.md`

- 7.8.1 Scope Regions
- 7.8.2 Block Scopes
- 7.8.3 Function Scopes
- 7.8.4 Module Scopes
- 7.8.5 Shadowing Rules (Detailed)

#### **Section 7.9: Name Resolution (Detailed)**
**File**: `07-9_Name-Resolution.md`

- 7.9.1 Resolution Algorithm
- 7.9.2 Ambiguity Resolution
- 7.9.3 Resolution Precedence

#### **Section 7.10: Modules and Imports (Detailed)**
**File**: `07-10_Modules-Imports.md`

- 7.10.1 Module Structure
- 7.10.2 Module Paths
- 7.10.3 Import Declarations
- 7.10.4 Export Control
- 7.10.5 Import Resolution
- 7.10.6 Import Conflicts
- 7.10.7 Circular Dependencies

#### **Section 7.11: Visibility**
**File**: `07-11_Visibility.md`

- 7.11.1 Visibility Levels
- 7.11.2 Visibility Rules
- 7.11.3 Visibility Checking
- 7.11.4 Privacy Violations

#### **Section 7.12: Predeclared Identifiers**
**File**: `07-12_Predeclared-Identifiers.md`

- 7.12.1 Predeclared Types
- 7.12.2 Predeclared Constants
- 7.12.3 Predeclared Functions
- 7.12.4 Shadowing Prohibition

---

### **PART VIII: PREDICATES AND DISPATCH** *(Normative)*

#### **Section 8.0: Predicate System Overview**
**File**: `08-0_Predicate-Overview.md`

- 8.0.1 Predicate Fundamentals
- 8.0.2 Predicate System Goals
- 8.0.3 Relationship to Contracts

#### **Section 8.1: Predicate Definitions**
**File**: `08-1_Predicate-Definitions.md`

- 8.1.1 Predicate Declaration
- 8.1.2 Predicate Items
- 8.1.3 Default Implementations
- 8.1.4 Predicate Inheritance

#### **Section 8.2: Predicate Implementation**
**File**: `08-2_Predicate-Implementation.md`

- 8.2.1 Implementation Blocks
- 8.2.2 Implementation Requirements
- 8.2.3 Implementation Completeness
- 8.2.4 Blanket Implementations

#### **Section 8.3: Predicate Bounds**
**File**: `08-3_Predicate-Bounds.md`

- 8.3.1 Bound Syntax
- 8.3.2 Bound Checking
- 8.3.3 Where Clauses
- 8.3.4 Higher-ranked Predicate Bounds

#### **Section 8.4: Predicate Coherence**
**File**: `08-4_Predicate-Coherence.md`

- 8.4.1 Orphan Rule
- 8.4.2 Overlap Rules
- 8.4.3 Fundamental Types
- 8.4.4 Coherence Violations

#### **Section 8.5: Static Dispatch**
**File**: `08-5_Static-Dispatch.md`

- 8.5.1 Monomorphization
- 8.5.2 Type Resolution
- 8.5.3 Performance Characteristics

#### **Section 8.6: Marker Predicates**
**File**: `08-6_Marker-Predicates.md`

- 8.6.1 Marker Predicate Definition
- 8.6.2 Copy, Clone
- 8.6.3 Send, Sync (if applicable)
- 8.6.4 Auto Predicates

#### **Section 8.7: Predicate Objects**
**File**: `08-7_Predicate-Objects.md`

- 8.7.1 Dynamic Dispatch
- 8.7.2 Object Safety
- 8.7.3 vtable Representation
- 8.7.4 Limitations

---

### **PART IX: GRANT SYSTEM** *(Normative)*

#### **Section 9.0: Grant System Overview**
**File**: `09_Grant-System/09-0_Grant-System-Overview.md`

- 9.0.1 Grant System Purpose
- 9.0.2 Grant Tracking Goals
- 9.0.3 Grant System Model
- 9.0.4 Terminology
- 9.0.5 Relationship to Other Language Features
- 9.0.6 Conformance Requirements
- 9.0.7 Organization of Part IX

#### **Section 9.1: Grant Clauses**
**File**: `09_Grant-System/09-1_Grant-Clauses.md`

- 9.1.1 Grant Clause Syntax
- 9.1.2 Grant Sets
- 9.1.3 Grant Paths
- 9.1.4 Procedure Grant Requirements
- 9.1.5 Grant Clause Composition
- 9.1.6 Deprecated Syntax
- 9.1.7 Grammar Reference
- 9.1.8 Well-Formedness Constraints
- 9.1.9 Interaction with Functions
- 9.1.10 Conformance Requirements

#### **Section 9.2: Grant Tracking and Propagation**
**File**: `09_Grant-System/09-2_Grant-Tracking.md`

- 9.2.1 Grant Propagation Fundamentals
- 9.2.2 Transitive Grant Propagation
- 9.2.3 Grant Verification Algorithm
- 9.2.4 Special Cases
- 9.2.5 Compile-Time Erasure
- 9.2.6 Grant Checking for Operations
- 9.2.7 Grant Violations and Diagnostics
- 9.2.8 Grant Inference
- 9.2.9 Conformance Requirements

#### **Section 9.3: Grant Polymorphism**
**File**: `09_Grant-System/09-3_Grant-Polymorphism.md`

- 9.3.1 Grant Parameter Fundamentals
- 9.3.2 Grant Parameter Bounds
- 9.3.3 Grant Parameter Instantiation
- 9.3.4 Grant Parameter Composition
- 9.3.5 Grant Polymorphic Patterns
- 9.3.6 Grant Parameter Verification
- 9.3.7 Advanced Grant Polymorphism
- 9.3.8 Limitations and Restrictions
- 9.3.9 Conformance Requirements

#### **Section 9.4: Built-In Grants**
**File**: `09_Grant-System/09-4_Built-In-Grants.md`

- 9.4.1 Built-In Grant Catalog
- 9.4.2 Allocation Grants (`alloc`)
- 9.4.3 File System Grants (`fs`)
- 9.4.4 Network Grants (`net`)
- 9.4.5 Time Grants (`time`)
- 9.4.6 Thread Grants (`thread`)
- 9.4.7 FFI Grants (`ffi`)
- 9.4.8 Unsafe Grants (`unsafe`)
- 9.4.9 Panic Grant (`panic`)
- 9.4.10 Grant Hierarchy Summary
- 9.4.11 Grant Subsumption
- 9.4.12 Conformance Requirements

#### **Section 9.5: User-Defined Grants**
**File**: `09_Grant-System/09-5_User-Defined-Grants.md`

- 9.5.1 Grant Declaration Fundamentals
- 9.5.2 Grant Path Resolution
- 9.5.3 Visibility and Access Control
- 9.5.4 Grant Usage and Propagation
- 9.5.5 Wildcard Grants
- 9.5.6 Grant Polymorphism with User Grants
- 9.5.7 Well-Formedness Constraints
- 9.5.8 Verification Algorithm
- 9.5.9 Compile-Time Erasure
- 9.5.10 Interaction with Built-In Grants
- 9.5.11 Grammar Reference
- 9.5.12 Conformance Requirements

#### **Section 9.6: Grant Integration**
**File**: `09_Grant-System/09-6_Grant-Integration.md`

- 9.6.1 Integration Overview
- 9.6.2 Grants and Contracts
- 9.6.3 Grants and Permissions
- 9.6.4 Grants and Modal Types
- 9.6.5 Grants and Unsafe Code
- 9.6.6 Grants and Functions vs Procedures
- 9.6.7 Grants and Closures
- 9.6.8 Grants and Generics
- 9.6.9 Grants and Predicates
- 9.6.10 Grants and Attributes
- 9.6.11 Grants and Modules
- 9.6.12 Grants and Metaprogramming
- 9.6.13 Grant Composition Patterns
- 9.6.14 Conformance Requirements

---

### **PART X: CONTRACT SYSTEM** *(Normative)*

#### **Section 10.0: Contract System Overview**
**File**: `10-0_Contract-Overview.md`

- 10.0.1 Design by Contract Philosophy
- 10.0.2 Contract Components
- 10.0.3 Static and Dynamic Checking

#### **Section 10.1: Behavioral Contracts**
**File**: `10-1_Behavioral-Contracts.md`

- 10.1.1 Contract Declaration
- 10.1.2 Contract Procedures
- 10.1.3 Contract Associated Types
- 10.1.4 Contract Implementation
- 10.1.5 Contract Extension
- 10.1.6 Contract Coherence
- 10.1.7 Contract vs Predicate

#### **Section 10.2: Preconditions**
**File**: `10-2_Preconditions.md`

- 10.2.1 Precondition Syntax (must)
- 10.2.2 Precondition Semantics
- 10.2.3 Precondition Checking
- 10.2.4 Precondition Predicates
- 10.2.5 Precondition Weakening
- 10.2.6 Precondition Composition

#### **Section 10.3: Postconditions**
**File**: `10-3_Postconditions.md`

- 10.3.1 Postcondition Syntax (will)
- 10.3.2 Postcondition Semantics
- 10.3.3 Postcondition Checking
- 10.3.4 Postcondition Predicates
- 10.3.5 Postcondition Strengthening
- 10.3.6 @old Capture Semantics

#### **Section 10.4: Type Invariants**
**File**: `10-4_Type-Invariants.md`

- 10.4.1 Type Invariant Syntax (where)
- 10.4.2 Type Invariant Semantics
- 10.4.3 Invariant Checking
- 10.4.4 Invariant Predicates
- 10.4.5 Invariant Preservation
- 10.4.6 Invariant Performance

#### **Section 10.5: Verification Modes**
**File**: `10-5_Verification-Modes.md`

- 10.5.1 Verification Attributes
- 10.5.2 Static Verification
- 10.5.3 Runtime Verification
- 10.5.4 No Verification
- 10.5.5 Verification Mode Inheritance

#### **Section 10.6: Contract Clause Composition**
**File**: `10-6_Clause-Composition.md`

- 10.6.1 Effect Propagation with Contracts
- 10.6.2 Precondition Composition
- 10.6.3 Postcondition Composition
- 10.6.4 Clause Compatibility

#### **Section 10.7: Contract Integration**
**File**: `10-7_Contract-Integration.md`

- 10.7.1 Contracts and Permissions
- 10.7.2 Contracts and Effects
- 10.7.3 Contracts and Modals
- 10.7.4 Contracts and Unsafe Code

---

### **PART XI: TYPE-LEVEL ADVANCED FEATURES** *(Normative)*

**Note:** This part covers advanced type-level features that build upon and integrate multiple concepts from Parts III-X. These features are grouped here because they require comprehensive understanding of the type system, permissions, effects, and contracts.

#### **Section 11.0: Advanced Features Overview**
**File**: `11-0_Advanced-Features-Overview.md`

- 11.0.1 Modal Types (state-dependent type systems)
- 11.0.2 Contract Witnesses (runtime evidence of contract satisfaction)
- 11.0.3 Type Inference and Holes (automated type derivation)

#### **Section 11.1: Modal Types and State Machines**
**File**: `11-1_Modal-Types.md`

- 11.1.1 Modal Type Declaration
- 11.1.2 Modal States
- 11.1.3 State Transitions
- 11.1.4 Modal Type Instantiation
- 11.1.5 State-Dependent Operations
- 11.1.6 Modal Patterns
- 11.1.7 Modal State Inference
- 11.1.8 Modal Type Checking
- 11.1.9 Modal Integration

#### **Section 11.2: Contract Witnesses**
**File**: `11-2_Contract-Witnesses.md`

- 11.2.1 Witness Concept
- 11.2.2 Witness Construction
- 11.2.3 Witness Types
- 11.2.4 Witness Operations
- 11.2.5 Witness Memory Management
- 11.2.6 Effect Polymorphism in Witnesses

#### **Section 11.3: Type Inference and Holes**
**File**: `11-3_Type-Inference-Holes.md`

- 11.3.1 Type Inference Overview
- 11.3.2 Hole Syntax
- 11.3.3 Constraint Generation
- 11.3.4 Constraint Solving
- 11.3.5 Type Hole Resolution
- 11.3.6 Permission Hole Resolution
- 11.3.7 Modal State Hole Resolution
- 11.3.8 Effect Hole Resolution
- 11.3.9 Inference Limitations
- 11.3.10 Elaboration

---

### **PART XII: METAPROGRAMMING** *(Normative)*

#### **Section 12.0: Metaprogramming Overview**
**File**: `12-0_Metaprogramming-Overview.md`

- 12.0.1 Compile-Time Computation
- 12.0.2 Code Generation
- 12.0.3 Reflection Capabilities

#### **Section 12.1: Attributes**
**File**: `12-1_Attributes.md`

- 12.1.1 Attribute Syntax
- 12.1.2 Attribute Placement
- 12.1.3 Core Attributes (repr, verify, inline, etc.)
- 12.1.4 User-Defined Attributes
- 12.1.5 Attribute Semantics

#### **Section 12.2: Compile-Time Evaluation**
**File**: `12-2_Compile-Time-Evaluation.md`

- 12.2.1 Const Expressions (detailed)
- 12.2.2 Const Functions (detailed)
- 12.2.3 Const Context
- 12.2.4 Comptime Blocks
- 12.2.5 Const Propagation

#### **Section 12.3: Code Generation**
**File**: `12-3_Code-Generation.md`

- 12.3.1 Macros
- 12.3.2 Derive Macros
- 12.3.3 Procedural Macros
- 12.3.4 Template Expansion

#### **Section 12.4: Reflection**
**File**: `12-4_Reflection.md`

- 12.4.1 Type Reflection
- 12.4.2 Compile-Time Type Queries
- 12.4.3 Runtime Type Information
- 12.4.4 Reflection Limitations

---

### **PART XIII: CONCURRENCY AND MEMORY ORDERING** *(Normative)*

**Note:** The memory model (§13.2) and atomic operations (§13.3) are mandatory for all conforming implementations. Thread and task primitives (§13.1, §13.4) may be library-provided, but the memory model must be normatively specified to define data race semantics and memory ordering guarantees.

#### **Section 13.0: Concurrency Model Overview**
**File**: `13-0_Concurrency-Overview.md`

- 13.0.1 Concurrency Concepts
- 13.0.2 Thread Model (may be library-provided)
- 13.0.3 Memory Model Goals

#### **Section 13.1: Threads and Tasks**
**File**: `13-1_Threads-Tasks.md`

- 13.1.1 Thread Creation
- 13.1.2 Thread Lifetime
- 13.1.3 Task Concept

#### **Section 13.2: Memory Model**
**File**: `13-2_Memory-Model.md`

- 13.2.1 Happens-Before Relation
- 13.2.2 Synchronizes-With Relation
- 13.2.3 Sequential Consistency
- 13.2.4 Data Races
- 13.2.5 Race Freedom

#### **Section 13.3: Atomic Operations**
**File**: `13-3_Atomic-Operations.md`

- 13.3.1 Atomic Types
- 13.3.2 Memory Ordering
- 13.3.3 Acquire-Release Semantics
- 13.3.4 Sequentially Consistent Ordering
- 13.3.5 Relaxed Ordering

#### **Section 13.4: Synchronization Primitives**
**File**: `13-4_Synchronization.md`

- 13.4.1 Mutexes
- 13.4.2 Condition Variables
- 13.4.3 Barriers
- 13.4.4 Channels (if applicable)

---

### **PART XIV: INTEROPERABILITY AND UNSAFE** *(Normative)*

#### **Section 14.0: Interoperability Overview**
**File**: `14-0_Interop-Overview.md`

- 14.0.1 Foreign Function Interface
- 14.0.2 C Compatibility Goals
- 14.0.3 Safety Boundaries

#### **Section 14.1: Foreign Function Interface**
**File**: `14-1_FFI.md`

- 14.1.1 External Declarations
- 14.1.2 Calling Conventions
- 14.1.3 FFI Types
- 14.1.4 FFI Safety
- 14.1.5 Name Mangling

#### **Section 14.2: Unsafe Operations in FFI Context**
**File**: `14-2_Unsafe-FFI.md`

**Note:** General unsafe block semantics are specified in §6.8. This section covers FFI-specific unsafe operations and obligations.

- 14.2.1 Unsafe Blocks in FFI (see §6.8 for general semantics)
- 14.2.2 Unsafe External Functions
- 14.2.3 FFI Safety Obligations
- 14.2.4 Raw Pointers from External Code
- 14.2.5 Memory Safety Across FFI Boundaries

#### **Section 14.3: C Compatibility**
**File**: `14-3_C-Compatibility.md`

- 14.3.1 C Type Representations
- 14.3.2 C String Handling
- 14.3.3 C Function Pointers
- 14.3.4 C Variadic Functions
- 14.3.5 C Macros and Preprocessing

#### **Section 14.4: Platform-Specific Features**
**File**: `14-4_Platform-Specific.md`

- 14.4.1 Target Architecture
- 14.4.2 Operating System Interface
- 14.4.3 Inline Assembly
- 14.4.4 SIMD and Intrinsics

#### **Section 14.5: Linkage and Symbol Visibility**
**File**: `14-5_Linkage-Symbols.md`

- 14.5.1 Linkage Semantics
- 14.5.2 Symbol Visibility
- 14.5.3 Name Mangling Rules
- 14.5.4 Static and Dynamic Linking

#### **Section 14.6: ABI Considerations**
**File**: `14-6_ABI-Considerations.md`

- 14.6.1 ABI Stability (Implementation-Defined)
- 14.6.2 Platform Calling Conventions
- 14.6.3 Data Layout Guarantees
- 14.6.4 ABI Versioning

---

## ANNEXES

### **ANNEX A: COMPLETE GRAMMAR** *(Normative)*

#### **Section A.0: Grammar Organization**
**File**: `Annex-A-0_Grammar-Organization.md`

- A.0.1 Notation Recap
- A.0.2 Grammar Structure
- A.0.3 Cross-reference to Normative Text

#### **Section A.1: Lexical Grammar**
**File**: `Annex-A-1_Lexical-Grammar.md`

- A.1.1 Input Elements
- A.1.2 Whitespace and Comments
- A.1.3 Identifiers
- A.1.4 Keywords
- A.1.5 Literals
- A.1.6 Operators
- A.1.7 Punctuation

#### **Section A.2: Type Grammar**
**File**: `Annex-A-2_Type-Grammar.md`

- A.2.1 Type Expressions
- A.2.2 Primitive Types
- A.2.3 Composite Types
- A.2.4 Function Types
- A.2.5 Generic Types
- A.2.6 Permission-Wrapped Types

#### **Section A.3: Expression Grammar**
**File**: `Annex-A-3_Expression-Grammar.md`

- A.3.1 Literal Expressions
- A.3.2 Identifier Expressions
- A.3.3 Operator Expressions
- A.3.4 Call Expressions
- A.3.5 Field and Index Expressions
- A.3.6 Block Expressions
- A.3.7 Control Flow Expressions
- A.3.8 Closure Expressions
- A.3.9 Compound Expressions

#### **Section A.4: Statement Grammar**
**File**: `Annex-A-4_Statement-Grammar.md`

- A.4.1 Expression Statements
- A.4.2 Declaration Statements
- A.4.3 Assignment Statements
- A.4.4 Control Flow Statements

#### **Section A.5: Pattern Grammar**
**File**: `Annex-A-5_Pattern-Grammar.md`

- A.5.1 Pattern Forms
- A.5.2 Literal Patterns
- A.5.3 Variable Patterns
- A.5.4 Wildcard Patterns
- A.5.5 Tuple/Record/Enum Patterns
- A.5.6 Or Patterns
- A.5.7 Range Patterns

#### **Section A.6: Declaration Grammar**
**File**: `Annex-A-6_Declaration-Grammar.md`

- A.6.1 Variable Declarations
- A.6.2 Function Declarations
- A.6.3 Type Declarations
- A.6.4 Predicate Declarations
- A.6.5 Contract Declarations
- A.6.6 Effect Declarations
- A.6.7 Modal Declarations

#### **Section A.7: Contract Grammar**
**File**: `Annex-A-7_Contract-Grammar.md`

- A.7.1 Contract Clauses
- A.7.2 Uses Clauses
- A.7.3 Must Clauses (Preconditions)
- A.7.4 Will Clauses (Postconditions)
- A.7.5 Where Clauses (Invariants)

#### **Section A.8: Assertion Grammar**
**File**: `Annex-A-8_Assertion-Grammar.md`

- A.8.1 Predicate Expressions
- A.8.2 Quantifiers
- A.8.3 Old Expressions
- A.8.4 Result References

#### **Section A.9: Effect Grammar**
**File**: `Annex-A-9_Effect-Grammar.md`

- A.9.1 Effect Paths
- A.9.2 Effect Sets
- A.9.3 Effect Parameters

#### **Section A.10: Attribute Grammar**
**File**: `Annex-A-10_Attribute-Grammar.md`

- A.10.1 Attribute Syntax
- A.10.2 Attribute Arguments

#### **Section A.11: Grammar Reference**
**File**: `Annex-A-11_Grammar-Reference.md`

**Note:** This section provides a consolidated EBNF reference for implementers. Sections A.1-A.10 contain grammar productions integrated with explanatory prose; this section consolidates all productions in pure EBNF format without prose for quick reference.

- Complete EBNF listing (all productions from A.1-A.10, consolidated)
- Cross-references to normative text
- Production index

---

### **ANNEX B: PORTABILITY AND BEHAVIOR** *(Informative)*

#### **Section B.0: Behavior Classification**
**File**: `Annex-B-0_Behavior-Classification.md`

- B.0.1 Classification Purpose
- B.0.2 Terminology

#### **Section B.1: Implementation-Defined Behavior**
**File**: `Annex-B-1_Implementation-Defined.md`

- Enumeration of all implementation-defined behaviors with cross-references to normative text
- Examples: integer sizes, alignment, endianness, etc.

#### **Section B.2: Undefined Behavior**
**File**: `Annex-B-2_Undefined-Behavior.md`

- Enumeration of all undefined behaviors with cross-references to normative text
- Each entry shall include:
  1. Catalog number (UB-001, UB-002, etc.)
  2. Brief description
  3. Cross-reference to normative text
  4. Example illustrating the violation
- Format:
  ```
  **UB-001: Out-of-bounds array access**
  Specification: §4.2.2 (Index Expressions)
  Description: Accessing array element outside bounds [0, length)
  Example: `arr[arr.len()]` where arr has length n
  ```
- Examples: out-of-bounds access, null dereference, data races, region escape violations, use-after-move, etc.

#### **Section B.3: Unspecified Behavior**
**File**: `Annex-B-3_Unspecified-Behavior.md`

- Enumeration of all unspecified behaviors with cross-references to normative text
- Examples: evaluation order (where not specified), etc.

---

### **ANNEX C: FORMAL SEMANTICS** *(Informative)*

#### **Section C.0: Overview**
**File**: `Annex-C-0_Semantics-Overview.md`

- C.0.1 Formal Methods Used
- C.0.2 Judgment Forms Summary
- C.0.3 Notation Conventions
- C.0.4 Relationship to Normative Text

#### **Section C.1: Type System Rules**
**File**: `Annex-C-1_Type-System-Rules.md`

- Typing judgments, subtyping, equivalence, well-formedness

#### **Section C.2: Operational Semantics**
**File**: `Annex-C-2_Operational-Semantics.md`

- Evaluation judgments, reduction rules, value forms

#### **Section C.3: Permission Semantics**
**File**: `Annex-C-3_Permission-Semantics.md`

- Permission lattice, checking rules, move semantics

#### **Section C.4: Effect Semantics**
**File**: `Annex-C-4_Effect-Semantics.md`

- Effect tracking, propagation, handlers, principal effects

#### **Section C.5: Contract Semantics**
**File**: `Annex-C-5_Contract-Semantics.md`

- Precondition/postcondition checking, invariants, verification

#### **Section C.6: Modal Semantics**
**File**: `Annex-C-6_Modal-Semantics.md`

- State machines, transitions, reachability, modal typing

#### **Section C.7: Meta-Theory**
**File**: `Annex-C-7_Meta-Theory.md`

- Type safety theorem, progress and preservation

---

### **ANNEX D: IMPLEMENTATION GUIDANCE** *(Informative)*

#### **Section D.0: Implementation Notes**
**File**: `Annex-D-0_Implementation-Notes.md`

- D.0.1 Purpose and Scope
- D.0.2 Non-normative Nature

#### **Section D.1: Abstract Syntax Representation**
**File**: `Annex-D-1_Abstract-Syntax.md`

- D.1.1 Suggested AST Representation
- D.1.2 AST Node Categories
- D.1.3 AST Well-Formedness
- D.1.4 AST Transformations
- D.1.5 Note: Implementations may use alternative representations

#### **Section D.2: Optimization Opportunities**
**File**: `Annex-D-2_Optimizations.md`

- D.2.1 Permission-based Optimizations
- D.2.2 Effect-based Optimizations
- D.2.3 Whole-program Optimizations

#### **Section D.3: Diagnostic Quality**
**File**: `Annex-D-3_Diagnostic-Quality.md`

- D.3.1 Error Message Guidelines
- D.3.2 Warning Guidelines
- D.3.3 Suggestion Mechanisms

#### **Section D.4: Edition System Implementation**
**File**: `Annex-D-4_Edition-System.md`

- D.4.1 Compiler Architecture for Editions
  - Parser configuration per edition
  - Semantic analysis per edition
  - Cross-edition interoperability layer
- D.4.2 Migration Tooling
  - AST transformation for syntax changes
  - Diagnostic guidance for semantic changes
  - Batch migration for large projects
- D.4.3 Testing Across Editions
  - Test suite organization per edition
  - Regression testing for backward compatibility
  - Edition-specific test flags

---

### **ANNEX E: STANDARD LIBRARY OVERVIEW** *(Informative)*

**Decision:** This annex provides an INFORMATIVE overview of the Cursive standard library. Detailed normative specification is maintained separately in "Cursive Standard Library Specification" document (following Rust/Go model).

#### **Section E.0: Standard Library Organization**
**File**: `Annex-E-0_Stdlib-Organization.md`

- E.0.1 Purpose and Scope
  - This annex provides informative overview only
  - Detailed semantics defined in "Cursive Standard Library Specification" (separate document)
  - Not normative for library implementation details
- E.0.2 Conformance Requirements
  - Conforming implementations shall provide core modules listed in §E.1
  - Module interfaces (types, functions) shall match library specification
  - Implementation details (algorithms, optimizations) are implementation-defined
- E.0.3 Library Specification Relationship
  - Library specification maintained separately for independent evolution
  - Library versions may update without language specification revision
  - Cross-references: Language spec → Library spec allowed; reverse discouraged
- E.0.4 Module Structure
- E.0.5 Prelude
- E.0.6 Versioning (independent of language edition)

#### **Section E.1: Core Modules**
**File**: `Annex-E-1_Core-Modules.md`

- E.1.1 Required Core Modules (Conforming implementations shall provide)
  - `core`: Fundamental types (Option, Result, basic predicates)
  - `alloc`: Allocation primitives
  - `io`: Input/output abstractions
- E.1.2 Primitive types, Option, Result, collections overview
- E.1.3 Cross-reference to Cursive Standard Library Specification for details

#### **Section E.2: I/O and System**
**File**: `Annex-E-2_IO-System.md`

- Console I/O, file I/O, system interfaces overview

#### **Section E.3: Standard Predicates and Contracts**
**File**: `Annex-E-3_Predicates-Contracts.md`

- Copy, Clone, Debug, Display, comparison predicates overview

---

### **ANNEX F: GLOSSARY** *(Informative)*

**File**: `Annex-F_Glossary.md`

Alphabetical listing of all technical terms with definitions and cross-references to normative text (A-Z).

---

## File Naming Convention

**Format**: `PartNumber-SectionNumber_Section-Name.md`

**Examples**:
- `01-0_Scope.md`
- `01-4_Conformance.md`
- `01-5_Document-Conventions.md`
- `01-6_Versioning-Evolution.md` (new in v3.1)
- `02-3_Syntax-Notation.md`
- `02-5_Scopes-Name-Lookup.md`
- `04-8_Conversions-Coercions.md`
- `Annex-A-11_Grammar-Reference.md`
- `Annex-B-2_Undefined-Behavior.md`
- `Annex-D-4_Edition-System.md` (new in v3.1)
- `Annex-E-0_Stdlib-Organization.md`

**Rules**:
- Use two digits for part number (01-14)
- Use one or two digits for section number (0-99)
- Use hyphens for multi-word section names
- Use descriptive but concise names
- Prefix annexes with "Annex-"
- Maintain alphabetical sorting by filename

**New Files in Version 3.1:**
- `01-6_Versioning-Evolution.md` - Edition system and language evolution policy
- `Annex-D-4_Edition-System.md` - Implementation guidance for edition support

---

## Normative Language Usage

Throughout the normative portions of this specification, the following keywords have the indicated meanings per ISO/IEC Directives Part 2:

- **shall** / **shall not** - Indicates a requirement (mandatory)
- **should** / **should not** - Indicates a recommendation (strongly advised)
- **may** / **may not** - Indicates permission (optional)
- **can** / **cannot** - Indicates capability (informative, not normative)

**Note:** This specification follows ISO/IEC conventions for language specification standards. Readers familiar with RFC 2119 should note: "shall" ≡ "MUST", "should" ≡ "SHOULD", "may" ≡ "MAY".

**Additional Terminology:**
- **ill-formed** - Program violates a rule requiring a compile-time diagnostic
- **ill-formed, no diagnostic required (IFNDR)** - Program violates a rule but no diagnostic is required
- **undefined behavior** - No constraints on program behavior; labeled [UB] and cataloged in Annex B.2

Informative sections (marked as such) and annexes marked "(informative)" are not part of the normative requirements.

---

## Conformance and Standardization Path

This specification is designed to be suitable for submission to ISO/IEC JTC 1/SC 22 (Programming Languages) or ECMA TC39-equivalent working group for potential standardization. The structure, normative language, and content organization follow established conventions from:

- ISO/IEC 14882 (C++)
- ISO/IEC 9899 (C)
- ECMA-334 (C#)
- Java Language Specification (JLS)

Alternatively, this specification may serve as a de facto standard with formal versioning and governance, following the Rust or Go model.
