# Cursive Language Specification: Part Mapping

**Version**: 1.0
**Last Updated**: 2025-11-02
**Purpose**: Authoritative mapping from file names to part numbers for cross-reference validation

---

## Overview

This document provides the **canonical mapping** between specification file names and their corresponding part numbers/names. Use this file as the single source of truth when writing cross-references to prevent numbering inconsistencies.

---

## Part Number Mapping

### Main Specification

| File Name | Part Number | Part Name | Status |
|-----------|-------------|-----------|--------|
| `00_LLM_Onboarding.md` | Part 0 | LLM Onboarding Guide | Complete |
| `01_Foundations.md` | Part I | Foundations | Complete |
| `02_Type-System.md` | Part II | Type System | Complete |
| `03_Declarations-and-Scope.md` | Part III | Declarations and Scope | Complete |
| `04_Lexical-Permissions.md` | Part IV | Lexical Permission System | Complete |
| `05_Expressions-and-Operators.md` | Part V | Expressions and Operators | Complete |
| `06_Statements-and-Control-Flow.md` | Part VI | Statements and Control Flow | Complete |
| `07_Contracts-and-Effects.md` | Part VII | Contracts and Effects | Complete |
| `08_Holes-and-Inference.md` | Part VIII | Holes and Inference | Complete |
| `09_Functions.md` | Part IX | Functions and Procedures | Complete |
| `10_Modals.md` | Part X | Modal Types | Complete |
| `11_Metaprogramming.md` | Part XI | Metaprogramming | Complete |
| `12_Modules-Packages-and-Imports.md` | Part XII | Modules, Packages, and Imports | Stub |
| `13_Memory-Permissions-and-Concurrancy.md` | Part XIII | Memory, Permissions, and Concurrency | Stub |
| `14_Errors-and-Exceptions.md` | Part XIV | Errors and Exceptions | Stub |
| `15_Unsafe-Behaviors-and-FFI.md` | Part XV | Unsafe Behaviors and FFI | Stub |
| `16_The-Standard-Library.md` | Part XVI | The Standard Library | Stub |

### Appendices

| File Name | Appendix | Title | Status |
|-----------|----------|-------|--------|
| `A1_Grammar.md` | Appendix A | Grammar | Complete |
| `A2_Well-Formedness-Rules.md` | Appendix B | Well-Formedness Rules | Placeholder |

---

## Quick Reference Table

**When you see a file number, use this part number:**

```
00 → Part 0    (LLM Onboarding)
01 → Part I    (Foundations)
02 → Part II   (Type System)
03 → Part III  (Declarations and Scope)
04 → Part IV   (Lexical Permission System)
05 → Part V    (Expressions and Operators)
06 → Part VI   (Statements and Control Flow)
07 → Part VII  (Contracts and Effects)
08 → Part VIII (Holes and Inference)
09 → Part IX   (Functions and Procedures)
10 → Part X    (Modal Types)
11 → Part XI   (Metaprogramming)
12 → Part XII  (Modules, Packages, and Imports)
13 → Part XIII (Memory, Permissions, and Concurrency)
14 → Part XIV  (Errors and Exceptions)
15 → Part XV   (Unsafe Behaviors and FFI)
16 → Part XVI  (The Standard Library)
```

---

## Common Cross-Reference Patterns

### Format: Part Number Only

```markdown
See Part VII for details on contracts.
```

### Format: Part Number with Section

```markdown
See Part VII §7.3 for effect clauses.
```

### Format: Part Name (with Number in Parentheses)

```markdown
**Part VII (Contracts and Effects):**
- §7.2: Behavioral contracts
- §7.3: Effect clauses
```

### Format: Full Citation

```markdown
CITE: Part I §1.3 — Metavariables; Part VII §7.3 — Effect Clauses; Part IX §9.2.9 — Comptime Functions.
```

---

## Validation Checklist

Before committing changes that include cross-references, verify:

- [ ] All "Part" references use Roman numerals (I, II, III, IV, V, VI, VII, VIII, IX, X, XI, XII, XIII, XIV, XV, XVI)
- [ ] Part numbers match the file numbers according to this mapping
- [ ] Part names match the canonical names in the mapping table above
- [ ] Citations to incomplete parts (XII-XVI) include a note about deferral if appropriate
- [ ] Section numbers (§X.Y) match the actual section structure of the referenced file
- [ ] File links use relative paths (e.g., `[Part VII](07_Contracts-and-Effects.md)`)

---

## Common Mistakes to Avoid

### ❌ Incorrect: Using File Numbers as Part Numbers

```markdown
See Part 07 for contracts.  // WRONG: Should be "Part VII"
```

### ❌ Incorrect: Outdated Part Numbers

```markdown
Part VII (Functions and Procedures)  // WRONG: Part VII is Contracts, Part IX is Functions
```

### ❌ Incorrect: Missing Roman Numerals

```markdown
See Part 7 §7.3  // WRONG: Should be "Part VII §7.3"
```

### ❌ Incorrect: Wrong Part Name

```markdown
Part VII (Effects)  // WRONG: Should be "Part VII (Contracts and Effects)"
```

### ✅ Correct Examples

```markdown
Part VII (Contracts and Effects)
Part VII §7.3 — Effect Clauses
See Part IX for function syntax
CITE: Part VII §7.3 — Effect Clauses; Part IX §9.2.9 — Comptime Functions.
```

---

## Section Number Conventions

Each part uses a consistent section numbering scheme:

- **§X.0**: Introduction/Overview
- **§X.1-X.9**: Major sections
- **§X.Y.Z**: Subsections (as needed)

When referencing sections, always verify the section number exists in the target file.

---

## Updating This Document

This document should be updated whenever:

1. A new part/file is added to the specification
2. A part is renamed or reorganized
3. Part numbers are reassigned
4. Appendices are added or renamed

**Maintainer Note**: After updating this file, run a global search for cross-references to the affected parts and update them accordingly.

---

## Automation Opportunities

Future tooling could validate cross-references automatically by:

1. Parsing all "Part" references in markdown files
2. Checking part numbers/names against this mapping
3. Verifying section references exist in target files
4. Generating a cross-reference validation report

---

## Related Documents

- **SpecCleanup1.md**: Comprehensive diagnostic review and corrective action plan
- **CLEANUP_SUMMARY.md**: Status of cleanup tasks and completion metrics
- **00_LLM_Onboarding.md**: Table of contents with current part status

---

**END OF PART_MAPPING.md**
