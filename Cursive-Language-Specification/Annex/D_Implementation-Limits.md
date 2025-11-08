# Cursive Language Specification

## Annex D — Implementation Limits (Informative)

**Part**: Annex
**Section**: Annex D - Implementation Limits
**Stable label**: [limits]
**Forward references**: None

---

### §D.1 Recommended Minimum Limits [limits.minimum]

[1] This annex shall specify recommended minimum limits that conforming implementations should support. These limits provide guidance for implementers and help programmers write portable code.

[ Note: This section is reserved for the comprehensive implementation limits catalog.

Planned content includes recommended minimums for:

**Translation Limits** (§D.2):
- Source file size: at least 1 MiB (§2.1.3[5])
- Nesting depth of blocks, expressions, types: at least 256 levels
- Number of parameters in a procedure: at least 255
- Number of fields in a record: at least 1024
- Number of variants in an enum: at least 1024
- Length of identifiers: at least 1023 characters
- Number of generic parameters: at least 256

**Compile-Time Limits** (§2.2.3):
- Recursion depth: at least 256 frames
- Evaluation steps: at least 1,000,000 per comptime block
- Memory allocation: at least 64 MiB per compilation unit
- String size: at least 1 MiB
- Collection cardinality: at least 10,000 elements

**Runtime Limits**:
- Stack size: implementation-defined, must be documented
- Maximum allocation size: platform-dependent
- Concurrent threads: platform-dependent

**Monomorphization Limits** (§10.6.9.3):
- Distinct instantiations per generic: at least 1024
- Nested generic instantiation depth: at least 32
- Generic parameters per entity: at least 256

**Qualified Name Limits** (§6.4.4.2):
- Qualified name chain length: at least 32 components

**Grant Set Limits** (§12.3.9):
- Grants per sequent clause: at least 256

**Delimiter Nesting** (§2.4.3):
- Delimiter nesting depth: at least 256

Implementations may support higher limits and shall document actual limits in their conformance documentation.
— end note ]

---

### §D.2 Translation Limits [limits.translation]

[2] This section shall specify limits applicable during compilation phases.

[ Note: Reserved for detailed translation limit specifications.
— end note ]

---

**Previous**: Annex C — Formal Semantics ([formal]) | **Next**: Annex E — Implementation Guidance ([implementation])

