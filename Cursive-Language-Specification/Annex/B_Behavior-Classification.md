# Cursive Language Specification

## Annex B — Behavior Classification (Normative)

**Part**: Annex
**Section**: Annex B - Behavior Classification
**Stable label**: [behavior]
**Forward references**: None

---

### §B.1 Implementation-Defined Behavior Catalog [behavior.implementation]

[1] This annex shall enumerate all behaviors for which this specification permits multiple valid implementations. Each entry shall identify the clause where the behavior is described, enumerate the permitted choices, and specify the documentation obligations imposed on conforming implementations.

[ Note: This section is reserved for the comprehensive catalog of implementation-defined behaviors. When complete, it will enumerate all behaviors where implementations must choose among valid options and document their choices.

Examples of implementation-defined behaviors referenced in the main specification include:
- Integer and floating-point representation widths beyond mandated minima (§3.5 [basic.align])
- Pointer and region size, alignment, and layout details (§11.6 [memory.layout])
- Resource ceilings such as maximum recursion depth or compilation unit size
- Diagnostic formatting and display conventions (§1.5.5 [intro.conformance.diagnostics])
- ABI choices for each supported platform (Clause 15)
— end note ]

---

### §B.2 Undefined Behavior Catalog [behavior.undefined]

[2] This annex shall provide the authoritative catalog of undefined behaviors (UB-IDs) referenced throughout the specification. Each entry shall include the UB identifier, a description of the undefined behavior, the operations that trigger it, and references to clauses where it is mentioned.

[ Note: This section is reserved for the complete undefined behavior catalog. Each UB-ID referenced in the main specification clauses will be fully specified here.

Example structure for catalog entries:

**B.2.01** — Access to object outside its lifetime
- **Description**: Accessing an object through any means (dereferencing a dangling pointer, using a moved-from binding, reading destroyed storage) after the object's lifetime has ended or before it has begun.
- **References**: §3.1.5[25], §11.2, §11.5
- **Detection**: Generally not detectable at compile time; produces undefined behavior at runtime

**B.2.02** — Read of uninitialized memory  
- **Description**: Reading the value of an object that has allocated storage but has not been initialized.
- **References**: §3.1.6[29], §5.7
- **Detection**: Prevented by definite assignment analysis (§5.7) in safe code

**B.2.10** — Misaligned object access
- **Description**: Accessing an object at an address that does not satisfy the type's alignment requirement on platforms with strict alignment enforcement.
- **References**: §3.5.2.2[6], §11.6
- **Detection**: Platform-dependent; may cause hardware faults or silent corruption

**B.2.25** — Invalid transmute
- **Description**: Using transmute with incompatible sizes or interpreting bit patterns invalid for the target type.
- **References**: §11.8.3.3[11]
- **Detection**: Size mismatch caught at compile time; validity checking is programmer responsibility

**B.2.50** — Data race
- **Description**: Concurrent access to the same memory location where at least one access is a write and accesses are not ordered by synchronization.
- **References**: §14.2.2[3-5], §14.2.5
- **Detection**: Prevented by permission system in safe code; possible in unsafe code or with incorrect synchronization

**B.2.51** — Misaligned atomic operation
- **Description**: Performing atomic operation on location that does not satisfy natural alignment for the type.
- **References**: §14.3.4[4]
- **Detection**: Platform-dependent; undefined behavior on most platforms

**B.2.52** — Dropping locked mutex
- **Description**: Destroying a Mutex value while in @Locked state (mutex dropped without unlock).
- **References**: §14.4.5[4]
- **Detection**: Type system prevents in safe code; possible with unsafe state coercion

Additional entries will be added as the specification evolves.
— end note ]

---

### §B.3 Unspecified Behavior Catalog [behavior.unspecified]

[3] This annex shall enumerate behaviors for which this specification permits multiple outcomes without requiring implementation documentation. Unlike implementation-defined behavior, implementations need not document which choice they make for unspecified behaviors.

[ Note: This section is reserved for the comprehensive catalog of unspecified behaviors.

Unspecified behaviors permit multiple well-defined outcomes without documentation requirements. Each outcome must be well-defined (not undefined behavior), but implementations may choose freely among the alternatives without documenting their choice.

Examples might include:
- Evaluation order of independent subexpressions (currently specified as left-to-right in Clause 8, so not unspecified)
- Padding byte values in structures (currently specified as indeterminate in §3.5.3.3)

The distinction between implementation-defined and unspecified behavior enables implementers to provide stronger diagnostics and enables programmers to reason about portability.
— end note ]

---

**Previous**: Annex A — Grammar ([grammar]) | **Next**: Annex C — Formal Semantics ([formal])

