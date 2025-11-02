# Appendix A2: Well-Formedness Rules

**Status**: =§ **Deferred to Post-v1.0** =§

**Last Updated**: 2025-11-02

---

## Purpose

This appendix will provide a comprehensive, consolidated reference of all well-formedness rules for Cursive's type system, including:

- Type well-formedness conditions
- Effect system well-formedness
- Region and lifetime constraints
- Modal state machine validation
- Contract and trait coherence rules
- Generic parameter constraints

---

## Current Status

This appendix is currently a **placeholder**. Well-formedness rules are currently specified **inline** within their respective chapters. This consolidation is planned for a future release.

---

## Where to Find Well-Formedness Rules Now

Until this appendix is populated, refer to the following sections for well-formedness specifications:

### Type Well-Formedness

**Part II  Type System**
- §2.1  Primitive Types
- §2.2  Composite Types (structs, tuples, arrays)
- §2.3  Modal Types (state machine well-formedness)
- §2.4  Slice Types
- §2.5  Reference Types
- §2.6  Generic Types (parameter constraints)
- §2.7  Trait Coherence
- §2.10  Function Types (effect polymorphism)

### Permission Well-Formedness

**Part IV  Lexical Permission System**
- §4.2  Permission Rules
- §4.3  Ownership and Move Semantics
- §4.4  Permission State Transitions

### Effect System Well-Formedness

**Part VII  Contracts and Effects**
- §7.3  Effect Declaration and Composition
- §7.4  Effect Subtyping Rules
- §7.5  Precondition Well-Formedness (`must` clauses)
- §7.6  Postcondition Well-Formedness (`will` clauses)

### Region Well-Formedness

**Part II  Type System**
- §2.8  Region Annotations
- §2.9  Lifetime Constraints

**Part IV  Lexical Permission System**
- §4.6  Region Scoping

### Modal State Machine Well-Formedness

**Part II  Type System**
- §2.3.3  Modal Type Validation
- §2.3.3.5  Modal Verification Algorithm (Theorem 2.47)

**Part X  Modal Types**
- §10.2  State Graph Construction
- §10.3  Transition Validation
- §10.4  Exhaustiveness Checking

### Generic Constraint Well-Formedness

**Part II  Type System**
- §2.6.3  Generic Bounds (`where` clauses)
- §2.7.4  Trait Implementation Coherence

**Part VII  Contracts and Effects**
- §7.7  Type Constraint Clauses (`where`)

### Function and Contract Well-Formedness

**Part IX  Functions**
- §9.2  Function Signature Validation
- §9.3  Parameter Constraints
- §9.4  Return Type Constraints

**Part VII  Contracts and Effects**
- §7.1  Contract Declaration Rules
- §7.8  Contract Composition

---

## Planned Structure (Future)

When populated, this appendix will be organized as follows:

### A2.1 Type Well-Formedness
- Primitive types
- Composite types
- Generic types
- Function types
- Modal types

### A2.2 Permission Well-Formedness
- Permission states
- Ownership rules
- Move semantics
- Borrowing rules

### A2.3 Effect Well-Formedness
- Effect declarations
- Effect composition
- Effect subtyping
- Effect inference constraints

### A2.4 Region Well-Formedness
- Region declarations
- Region scoping
- Lifetime constraints
- Region polymorphism

### A2.5 Modal State Machine Well-Formedness
- State graph construction
- Transition validation
- Exhaustiveness requirements
- Procedure state compatibility

### A2.6 Contract and Trait Well-Formedness
- Contract declarations
- Trait declarations
- Implementation coherence
- Orphan rules

### A2.7 Generic Constraint Well-Formedness
- Type parameter bounds
- Where clause validation
- Constraint satisfaction
- Variance rules

---

## Implementation Notes

**For Implementers:**

Currently, you **must** consult the inline well-formedness rules in each chapter listed above. This appendix will eventually consolidate all rules for convenient reference, but the authoritative specifications remain in their respective chapters.

**For Specification Editors:**

When populating this appendix:
1. Extract well-formedness rules from inline specifications
2. Preserve original numbering and cross-references
3. Add bidirectional links (appendix ” chapter)
4. Ensure no semantic drift from source chapters
5. Mark rules as either "normative" or "informative" (derived)

---

## Contribution

If you're interested in helping populate this appendix, please see the Cursive specification repository's contribution guidelines.

---

## Changelog

- **2025-11-02**: Placeholder created with cross-references to inline rules
- **Future**: Full consolidation of well-formedness rules from all chapters
