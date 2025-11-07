# Diagnostic Code Migration Map

**Document**: Diagnostic Code Migration Map
**Version**: 1.0
**Date**: 2025-11-06
**Purpose**: Document migration from legacy and inconsistent diagnostic codes to the standardized `E[CC]-[NNN]` format

---

## Overview

This document provides a comprehensive mapping from legacy diagnostic codes to the new standardized format established in §1.6.6 [intro.document.diagnostics]. It serves as a reference for:

- **Compiler implementers**: Updating diagnostic messages to use canonical codes
- **Tool developers**: Supporting both legacy and new diagnostic codes during transition
- **Specification editors**: Tracking code evolution and ensuring consistency

### Standardized Format

**Canonical format**: `E[CC]-[NNN]`
- `E`: Error prefix
- `[CC]`: Two-digit clause number with leading zero (01, 02, ..., 16)
- `-`: Hyphen separator
- `[NNN]`: Three-digit sequential number with leading zeros (001, 002, ..., 999)

**Example**: `E02-001` (Clause 02, diagnostic 001)

---

## Migration Tables

### Clause 02: Lexical Structure and Translation

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E2001 | E02-001 | Invalid UTF-8 sequence | Migrated |
| E2002 | E02-002 | File size exceeds limit | Migrated |
| E2003 | E02-003 | Misplaced BOM | Migrated |
| E2004 | E02-004 | Prohibited code point (null) | Migrated |
| E2007 | E02-205 | Unterminated floating literal | Migrated |
| E2009 | E02-206 | Integer literal exceeds range | Migrated |
| E2010 | E02-207 | Float literal exceeds range | Migrated |
| E2016 | E02-301 | Disallowed form at module level | Migrated |
| E2017 | E02-401 | Cyclic module initializer dependency | Migrated |
| E2019 | E02-400 | Redeclaration in same scope | Migrated |
| E2020 | E02-302 | Multiple entry points | Migrated |
| E2021 | E02-303 | Entry point invalid signature | Migrated |
| E2022 | E02-304 | Entry point invalid visibility | Migrated |
| E02CT-0100 | E02-100 | Comptime exceeded iterations | Migrated |
| E02CT-0101 | E02-101 | Comptime exceeded memory | Migrated |
| E02CT-0102 | E02-102 | Comptime exceeded stack depth | Migrated |
| E02CT-0103 | E02-103 | Comptime exceeded time limit | Migrated |

### Clause 04: Modules

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E4001 | E04-001 | Invalid module path structure | Migrated |
| E4002 | E04-002 | Module file not found | Migrated |
| E4003 | E04-003 | Unknown file extension | Migrated |
| E4004 | E04-004 | Missing Cursive.toml | Migrated |
| E4005 | E04-005 | Malformed manifest | Migrated |
| E4006 | E04-006 | Invalid manifest schema | Migrated |
| E4101 | E04-100 | Import path does not resolve | Migrated |
| E4102 | E04-101 | Wildcard import conflicts | Migrated |
| E4103 | E04-102 | Import alias shadows name | Migrated |
| E4104 | E04-103 | Circular import | Migrated |
| E4201 | E04-200 | Use binding conflicts | Migrated |
| E4202 | E04-201 | Wildcard expansion duplicates | Migrated |
| E4203 | E04-202 | Use target not found | Migrated |
| E4204 | E04-203 | Re-export of private item | Migrated |
| E4301 | E04-400 | Module qualifier does not resolve | Migrated |
| E4302 | E04-401 | Name not found in qualified module | Migrated |
| E4303 | E04-402 | Qualified name resolves to private | Migrated |
| E4304 | E04-403 | Qualification chain exceeds length | Migrated |
| E4401 | E04-500 | Module initialization cycle (eager) | Migrated |
| E4402 | E04-501 | Initializer side effect ordering | Migrated |
| E4403 | E04-502 | Lazy initialization deferred | Migrated |
| E4701 | E04-700 | Internal type in public API | Migrated |
| E2035 | E04-002 | Referenced module missing (duplicate of E04-002) | Consolidated |
| E2036 | E04-401 | Item not exported (duplicate of E04-401) | Consolidated |
| E2037 | E04-203 | Public use of non-public (duplicate of E04-203) | Consolidated |

**Note**: E2035, E2036, E2037 were module-related diagnostics originally assigned to Clause 02 but have been consolidated with their equivalents in Clause 04.

### Clause 05: Declarations

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E5201 | E05-200 | Let binding requires initializer | Migrated |
| E5202 | E05-201 | Type annotation missing | Migrated |
| E5203 | E05-202 | Assignment to immutable binding | Migrated |
| E5204 | E05-203 | Pattern binding refutability | Migrated |
| E5301 | E05-300 | Pattern type mismatch | Migrated |
| E5302 | E05-301 | Non-exhaustive pattern match | Migrated |
| E5303 | E05-302 | Unreachable pattern | Migrated |
| E5304 | E05-303 | Pattern binding name collision | Migrated |
| E5401 | E05-400 | Procedure signature mismatch | Migrated |
| E5402 | E05-401 | Procedure body missing | Migrated |
| E5403 | E05-402 | Return type annotation required | Migrated |
| E5404 | E05-403 | Receiver on non-method | Migrated |
| E5405 | E05-404 | Multiple receiver parameters | Migrated |
| E5406 | E05-405 | Generic parameter constraint | Migrated |
| E5407 | E05-406 | External procedure has body | Migrated |
| E5501 | E05-500 | Type alias cycle | Migrated |
| E5502 | E05-501 | Struct field name duplication | Migrated |
| E5503 | E05-502 | Enum variant name duplication | Migrated |
| E5504 | E05-503 | Union variant type restriction | Migrated |
| E5505 | E05-504 | Type parameter shadowing | Migrated |
| E5506 | E05-505 | Newtype must wrap one field | Migrated |
| E5507 | E05-506 | Recursive type requires indirection | Migrated |
| E5601 | E05-600 | Visibility modifier conflicts | Migrated |
| E5602 | E05-601 | Private item in public signature | Migrated |
| E5603 | E05-602 | Restricted visibility path missing | Migrated |
| E5604 | E05-603 | Visibility escalation in re-export | Migrated |
| E5701 | E05-700 | Variable used before initialization | Migrated |
| E5702 | E05-701 | Possibly-uninitialized in conditional | Migrated |
| E5703 | E05-702 | Partial struct initialization | Migrated |
| E5704 | E05-703 | Move from uninitialized | Migrated |
| E5705 | E05-704 | Initialization order violation | Migrated |
| E5801 | E05-800 | Entry point 'main' not found | Migrated |
| E5802 | E05-801 | Entry point invalid signature | Migrated |
| E5803 | E05-802 | Multiple entry points | Migrated |
| E5804 | E05-803 | Entry point visibility | Migrated |
| E5901 | E05-900 | Grant target does not exist | Migrated |
| E5902 | E05-901 | Grant conflicts | Migrated |

### Clause 06: Names and Scopes

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E2023 | E06-400 | Undefined identifier | Migrated |
| E2024 | E06-401 | Ambiguous name resolution | Migrated |
| E2026 | E06-403 | Qualified name does not resolve | Migrated |
| E2027 | E06-500 | Module alias target missing | Migrated |
| E2028 | E06-501 | Circular module alias | Migrated |
| E2029 | E06-502 | Module alias shadows import | Migrated |
| E6201 | E06-200 | Scope nesting exceeds limit | Migrated |
| E6202 | E06-201 | Capture violates scope rules | Migrated |
| E6301 | E06-300 | Illegal shadowing in same scope | Migrated |
| E6302 | E06-301 | Shadowing predeclared identifier | Migrated |
| E6303 | E06-302 | Parameter shadows outer binding | Migrated |

**Note**: Several E2xxx codes were name-resolution diagnostics that logically belong to Clause 06, now migrated accordingly.

### Clause 07: Type System

**This is the most significant migration**, with three different legacy patterns consolidated:

#### 7.1 Basic Type Errors

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E0301 | E07-001 | Character literal with surrogate | Migrated |
| E2009 | E07-002 | Type annotation required (if context is typing, not literal) | Migrated |
| E2010 | E07-003 | Type mismatch in expression | Migrated |

#### 7.2 Primitive Type Subcategory Codes (E8XXX-NNNN Pattern → E07-xxx)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E8INT-0001 | E07-100 | Integer constant overflow | Migrated |
| E8INT-0002 | E07-101 | Integer constant underflow | Migrated |
| E8F-0001 | E07-200 | Float constant out of range | Migrated |
| E8F-0002 | E07-201 | Float precision loss | Migrated |
| E8BOOL-0001 | E07-300 | Invalid boolean constant | Migrated |
| E8BOOL-0002 | E07-301 | Boolean type mismatch | Migrated |

#### 7.4 Function Type Subcategory Codes (E7FN-NNNN Pattern → E07-xxx)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E7FN-0001 | E07-500 | Function parameter count mismatch | Migrated |
| E7FN-0002 | E07-501 | Function parameter type mismatch | Migrated |
| E7FN-0003 | E07-502 | Function return type mismatch | Migrated |
| E7FN-0004 | E07-503 | Grant annotation mismatch | Migrated |
| E7FN-0005 | E07-504 | Precondition violation | Migrated |
| E7FN-0006 | E07-505 | Postcondition violation | Migrated |
| E7FN-0007 | E07-506 | Effect annotation mismatch | Migrated |

#### 7.5 Pointer/Modal/Relation Errors (E4XXX Pattern → E07-xxx)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E4027 | E07-700 | Pointer permission violation | Migrated |
| E4028 | E07-701 | Dereference of null pointer | Migrated |
| E4029 | E07-702 | Raw pointer in safe context | Migrated |
| E4030 | E07-703 | Permission insufficient | Migrated |
| E4031 | E07-704 | Aliasing violation | Migrated |
| E4050 | E07-705 | Raw pointer error (additional) | Migrated |
| E4051 | E07-706 | Raw pointer error (additional) | Migrated |
| E4052 | E07-707 | Raw pointer error (additional) | Migrated |
| E4053 | E07-708 | Raw pointer error (additional) | Migrated |
| E4500 | E07-800 | Invalid modal transition | Migrated |
| E4501 | E07-801 | Modal state violation | Migrated |
| E4502 | E07-802 | Modal type in invalid context | Migrated |
| E4503 | E07-803 | Missing modal transition guard | Migrated |
| E4504 | E07-804 | Modal error (additional) | Migrated |
| E4700 | E07-900 | Types not equivalent | Migrated |
| E4701 | E07-901 | Subtyping relation fails | Migrated |
| E4702 | E07-902 | Type compatibility check failed | Migrated |
| E4703 | E07-903 | Variance annotation violation | Migrated |
| E4704 | E07-904 | Type relation error (additional) | Migrated |
| E4900 | E07-950 | Union type construction invalid | Migrated |
| E4901 | E07-951 | Union member access without narrowing | Migrated |
| E4902 | E07-952 | Union type widening failed | Migrated |
| E4903 | E07-953 | Type introspection error (additional) | Migrated |

**Note**: Clause 07 had significant overlap with E4xxx range. All type-related diagnostics now use E07-xxx exclusively. E04-xxx is reserved for module-specific errors only.

---

## Legacy Codes from Old_* Files

The following codes appear in archived `Old_*.md` files and are documented here for historical reference. These codes are **deprecated** and should not be used in new implementations.

### Old_Declarations-and-Scope.md (E3D Pattern)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E3D01 | E06-300 | Redeclaration in same scope | Deprecated |
| E3D02 | E05-201 | Type annotation required | Deprecated |
| E3D03 | E06-400 | Undefined identifier | Deprecated |
| E3D04 | E05-700 | Use before initialization | Deprecated |
| E3D05 | E05-701 | Possibly uninitialized | Deprecated |
| E3D06 | E05-702 | Uninitialized binding | Deprecated |
| E3D10 | E05-202 | Reassign to let binding | Deprecated |
| E3D11 | E05-704 | Initialization order | Deprecated |
| E3D12 | E06-302 | Parameter name collision | Deprecated |

### Old_Contracts-and-Effects.md (E7C Pattern)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E7C01 | E13-001 | Precondition violation | Deprecated (reserved for Clause 13) |
| E7C02 | E13-002 | Postcondition violation | Deprecated |
| E7C03 | E13-003 | Invariant violation | Deprecated |
| E7C04-E7C18 | E13-004 to E13-018 | Various contract errors | Deprecated |

**Note**: Contract diagnostics will use E13-xxx range once Clause 13 (Contracts) is finalized.

### Old_Functions.md (E9F/E9P Pattern)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E9F-0101 | E05-400 | Function declaration error | Deprecated |
| E9F-0102 | E05-401 | Function body missing | Deprecated |
| E9F-0103 | E05-402 | Return type required | Deprecated |
| E9F-9301 | E05-403 | Self parameter error | Deprecated |
| E9F-9302 | E05-404 | Multiple self parameters | Deprecated |
| E9P-0101 | E05-400 | Procedure declaration error | Deprecated |
| E9P-0102 | E05-401 | Procedure body missing | Deprecated |

**Note**: Functions and procedures are now unified in Clause 05 (Declarations), so E9xxx codes are obsolete.

### Old_Modals.md (E6004)

| Legacy Code | Standardized Code | Description | Status |
|-------------|-------------------|-------------|--------|
| E6004 | E07-800 | Invalid modal transition | Deprecated |

---

## Migration Strategy

### For Compiler Implementers

1. **Phase 1: Dual Support** (Recommended during transition)
   - Recognize both legacy and new codes internally
   - Always **emit** new standardized codes in output
   - Optionally provide a compatibility flag `--legacy-diagnostics` to emit old codes

2. **Phase 2: Legacy Deprecation**
   - Issue warnings when encountering tools that expect legacy codes
   - Update documentation to reference only new codes

3. **Phase 3: Legacy Removal**
   - After sufficient transition period (e.g., 6 months), remove legacy code recognition
   - All diagnostics use only E[CC]-[NNN] format

### For Tool Developers

1. Update parsers to recognize `E\d{2}-\d{3}` pattern
2. Maintain backward compatibility by mapping legacy codes using this document
3. Prefer new codes in all output and documentation

### For Specification Editors

1. Replace all legacy codes in specification text with standardized equivalents
2. Update clause index files to use new format
3. Update Annex E §E.5 diagnostic registry
4. Cross-reference this migration map in §1.6.6

---

## Validation Checklist

- [x] All legacy codes documented
- [x] Mapping to standardized codes complete
- [x] Consolidated duplicates identified (E2035, E2036, E2037, E4701)
- [x] Reserved ranges for future clauses documented (E08-xxx through E16-xxx)
- [x] Migration strategy provided
- [ ] All specification files updated to use new codes (in progress)
- [ ] All clause index files updated (in progress)
- [ ] Annex E §E.5 updated (completed)
- [ ] Validation report generated (pending)

---

## Contact and Maintenance

**Maintained by**: Cursive Language Specification Editors
**Last updated**: 2025-11-06
**Version**: 1.0

For questions or clarifications about diagnostic code migrations, consult:
- §1.6.6 [intro.document.diagnostics] — Canonical format specification
- Annex E §E.5 [implementation.diagnostics] — Authoritative diagnostic registry
- This document — Legacy code mapping

---

**End of Migration Map**
