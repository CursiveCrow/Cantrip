# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.0: Scope

**File**: `01-0_Scope.md`
**Version**: 1.0
**Status**: Normative
**Previous**: (Start) | **Next**: [[REF_TBD]](01-1_Normative-References.md)

---

## 1.0.1 Purpose of This Specification

This document specifies the syntax, semantics, and constraints of the Cursive programming language, Version 1.0. It defines:

1. The lexical structure, grammar, and syntactic rules of Cursive programs
2. The static semantics, including type system, grant system, and permission system
3. The dynamic semantics and execution model
4. The requirements for conforming implementations
5. The requirements for conforming programs

This specification is intended to enable:

- **Implementers** to create conforming Cursive compilers, interpreters, and analysis tools
- **Tool developers** to build IDEs, linters, formatters, and other language tools
- **Language designers** to understand and extend the language in a principled manner
- **Programmers** to write portable, well-defined Cursive code

This specification does not prescribe:

- Implementation techniques or internal representations
- Optimization strategies
- Diagnostic message formats (though minimum requirements are specified)
- Development tools, build systems, or package managers
- Standard library implementation details (see [REF_TBD]

## 1.0.2 Scope and Applicability

This specification applies to all implementations claiming conformance to Cursive Version 1.0. It defines:

**In Scope:**

- Lexical structure (tokens, keywords, literals, comments)
- Syntactic structure (grammar productions for expressions, statements, declarations)
- Type system (primitive types, composite types, generic types, subtyping)
- Memory model (two-axis system: binding categories `let`/`var` and permissions `const`/`unique`/`shared`)
- Grant system (compile-time tracking of side effects, I/O, allocation)
- Contract system (preconditions, postconditions, invariants)
- Modal types (state machines as types with compile-time transition verification)
- Resource management (regions, storage duration, object lifetime, RAII)
- Expression and statement semantics
- Declaration and scope rules
- Name resolution and visibility
- Type inference and holes
- Compile-time evaluation (comptime)
- Metaprogramming (code generation, reflection)
- Foreign function interface (FFI) and C interoperability
- Unsafe operations

**Out of Scope (Specified in Companion Documents):**

- Standard library specification (see [REF_TBD]
- Platform-specific ABIs and calling conventions (implementation-defined)
- Concurrency primitives (threads, tasks, synchronization) — deferred to future versions
- Memory model for concurrent programs (data races, happens-before) — deferred to future versions

**Implementation-Defined:**

Certain aspects of the language are explicitly implementation-defined, meaning conforming implementations may make different choices. These include:

- Integer and floating-point type sizes (within specified minimums)
- Alignment requirements
- Memory layout details (except where specified by `[[repr]]` attributes)
- Maximum program size and nesting depths
- Optimization strategies
- Diagnostic message content and formatting
- Resource limits (stack size, heap size, compilation time)

All implementation-defined behaviors shall be documented by conforming implementations (see [REF_TBD]

## 1.0.3 Target Audiences

This specification is written for three primary audiences:

### Implementers

Compiler writers, interpreter developers, and tool implementers require a precise, unambiguous definition of language semantics. This specification provides:

- Formal grammar in Extended Backus-Naur Form (EBNF)
- Typing rules and inference algorithms
- Well-formedness constraints
- Static and dynamic semantic rules
- Diagnostic requirements

Implementers should focus on normative sections (marked **Normative**) and may use informative sections and annexes as guidance.

### Tool Developers

Developers of IDEs, linters, formatters, debuggers, and other tools require a complete syntactic and semantic specification. This specification provides:

- Complete lexical and syntactic grammar
- Type system and name resolution algorithms
- Scope and visibility rules
- Cross-references between language constructs

Tool developers may leverage the formal semantics (Annex C) and grammar reference (Annex A).

### Language Designers and Researchers

Those extending Cursive or studying its design require understanding of design principles, tradeoffs, and formal foundations. This specification provides:

- Design philosophy and safety model
- Formal semantics and type theory
- Relationship between language features
- Rationale for design decisions (in informative notes)

Language designers should study Part I for foundations, then relevant feature-specific parts.

## 1.0.4 Relationship to Companion Documents

This specification is accompanied by several companion documents:

### Normative Companion Documents

#### Cursive Standard Library Specification (Normative, Separate Document)

The standard library is specified in a separate normative document to enable independent evolution. The standard library specification defines:

- Core modules (`core`, `alloc`, `io`)
- Primitive type extensions and standard predicates
- Collection types and algorithms
- I/O and system interfaces

**Relationship to Language Specification:**

- The language specification defines the language itself
- The standard library specification defines the required standard library
- Conforming implementations shall provide both the language and the standard library
- Library versions may evolve independently of language versions

**Cross-references:**

- Language spec -> Library spec: Permitted (e.g., "See collection types in Standard Library")
- Library spec -> Language spec: Discouraged (library should be version-agnostic where possible)

An informative overview of the standard library is provided in Annex E.

### Informative Companion Documents

The following documents provide non-normative guidance:

#### Cursive Programming Guide (Informative)

Practical guidance for Cursive programmers, including:

- Idiomatic patterns and best practices
- Common pitfalls and how to avoid them
- Performance considerations
- Migration guides between versions
- Worked examples and tutorials

**Target Audience**: Programmers learning or using Cursive

#### Cursive LLM Development Guide (Informative)

Guidance for AI-assisted Cursive development, including:

- Predictable patterns for code generation
- Prompt engineering for Cursive code
- Common templates and skeletons
- Verification strategies for generated code

**Target Audience**: AI tool developers and users leveraging LLMs for Cursive development

#### Cursive Compiler Diagnostics Reference (Informative)

Comprehensive catalog of compiler error codes and warnings, including:

- Error code definitions (E####)
- Suggested diagnostic messages
- Common causes and solutions
- Code examples triggering each diagnostic

**Target Audience**: Compiler implementers and tool developers

**Relationship to Language Specification:**

This specification defines which violations require diagnostics ([REF_TBD]), but does not prescribe diagnostic message wording. The Diagnostics Reference provides recommended messages.

---

## Conformance to ISO/IEC Standards Practice

This specification follows conventions established by ISO/IEC and ECMA language standards, including:

- ISO/IEC 14882 (C++)
- ISO/IEC 9899 (C)
- ECMA-334 (C#)
- Java Language Specification (JLS)

While not formally submitted to a standards body, this specification is designed to be suitable for future standardization.

---

**Previous**: (Start) | **Next**: [[REF_TBD]](01-1_Normative-References.md)
