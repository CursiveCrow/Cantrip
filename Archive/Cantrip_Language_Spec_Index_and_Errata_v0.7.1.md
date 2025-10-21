# Cantrip Language Specification — Index & Errata (Normative wrapper)

**Version:** 0.7.1 (compatible with 0.6.x)  
**Date:** 2025-10-21  
**Status:** Normative pointers and corrections

This document provides a clean, authoritative index of the Cantrip specification and documents errata and editorial corrections consolidated in version **0.7.1**. When a conflict exists with earlier drafts, the corrections herein prevail.

---

## 1. Conformance statement

An implementation **conforms** iff it satisfies all normative requirements across Parts I–XIV. Extensions MUST NOT invalidate programs that are valid under this specification. Diagnostics, implementation-defined limits, and unspecified behavior MUST be documented.

## 2. Document conventions (unchanged)

- Language code fences use `cantrip`.  
- Grammar blocks use `ebnf`.  
- Mathematical judgments use standard notation.  
- Error identifiers (e.g., `E9001`) are normative and MUST be emitted for the corresponding violations.

## 3. Corrected, linearized Table of Contents (Parts 0–XIV)

- **Part 0** (Informative): LLM Quick Reference  
- **Part I**: Foundational Concepts (notation, lexical structure, abstract syntax)  
- **Part II**: Type System (types/values, rules, records, enums, traits, generics)  
- **Part III**: Modal System (modals overview, formal semantics, verification)  
- **Part IV**: Functions and Expressions (functions, expressions, control flow, closures)  
- **Part V**: Contracts (pre/post, invariants, Hoare logic, wp, testing)  
- **Part VI**: Effect System (effects, rules/checking, budgets, soundness)  
- **Part VII**: Memory Management (LPS, permissions, ownership, regions)  
- **Part VIII**: Module System (modules, imports/exports, visibility, resolution)  
- **Part IX**: Advanced Features (compile-time programming, concurrency, actors, async/await)  
- **Part X**: Operational Semantics (small-/big-step, memory model, evaluation order)  
- **Part XI**: Soundness and Properties (type/effect/memory/modal soundness)  
- **Part XII**: Standard Library (core types, collections, I/O, net, concurrency)  
- **Part XIII**: Tooling and Implementation  
  - 51. Compiler Architecture  
  - 52. Error Reporting  
  - 53. Package Management  
  - 54. Testing Framework  
  - 55. Machine‑Readable Output (diagnostics/IDEs/LLMs)  
- **Part XIV**: Foreign Function Interface  
  - 56. Overview  
  - 57. Declarations and Linkage  
  - 58. Type Mappings  
  - 59. Ownership and Allocation Across FFI  
  - 60. Callbacks from C into Cantrip  
  - 61. Errors and Panics  
  - 62. Inline Assembly (Reserved)

## 4. Normative corrections applied in v0.7.1

1. **Version & dates** — All headers unified to **v0.7.1**. Earlier `0.7.0` date strings are superseded.
2. **Effects taxonomy** — The **canonical taxonomy** lives in **§21.11**. Duplicate/earlier lists in §21.9.* are harmonized to §21.11. Wildcards expand exactly as enumerated there.
3. **Forbidden effects** — Redundant `!effect` markers are rejected (**E9010**) unless:
   - **Wildcard restriction**: `needs fs.*, !fs.delete;`
   - **Polymorphic constraint**: `needs effects(F), !time.read;`
4. **Trait-declared effects** — Trait methods may declare effect upper-bounds; implementations MUST be ≤ bound (**E9120**). See §8.6.
5. **Async effect masks** — `await` imports the awaited future’s effects; missing declarations trigger **E9201** with a fix‑it to add the missing atoms (§22.7).
6. **Canonical arrows** —
   - Modal transitions: `@S >> m(...) >> @T` (reject `->`, `=>`).
   - Pattern mapping: `match ... { p -> e }` (reject `=>`).
   - Pipeline operator remains `=>`.
7. **Removed syntaxes** — `requires<effects>:` / `returns<T>:` / `implements:` headers removed (E1010–E1012). Use the canonical function header.
8. **Keywords** — Reserved and contextual keyword lists are de-duplicated; contextual keywords `effects`, `pure` appear once.
9. **Typed holes** — §14.10 clarifies compile-time/runtime behavior (`E9501`, `E9502`, `E9503`) and prohibits effect-widening.

## 5. Machine‑Readable Output (Part XIII §55)

Tooling should expose diagnostics and structure with at least the following stable fields (non-exhaustive; all names lowercase, snake_case):

- `error_id` (e.g., `"E9001"`), `severity`, `message`, `span` (file, line, col), `notes` (array).
- `missing_effects` (array of effect atoms) for **E9001/E9201**.
- `forbidden_effects` (array) for **E9010**.
- `typed_holes` (array of `{name?, inferred_type?, span}`) for **E9501/E9502**.
- `modal_transition` (`from_state`, `procedure`, `to_state?`) for modal diagnostics.
- `fix_its` (array of textual edits with `insert/delete/replace` and a target span).
- Optional `emit_aliases` flag to include `#[alias(...)]` names to aid IDEs/LLMs; aliases have no ABI impact.

## 6. Conformance & documentation requirements

- **Diagnostics** MUST cite error IDs defined by the specification.
- **Implementation-defined limits** (maximum source size, recursion depth, arena page sizes, etc.) MUST be documented.
- **Unspecified behavior** MUST be documented where it can be observed by user code.

---

**End of Index & Errata v0.7.1**  
This file should be shipped alongside the full specification for precise navigation and consistency.
