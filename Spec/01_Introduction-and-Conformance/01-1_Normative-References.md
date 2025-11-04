# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.1: Normative References

**File**: `01-1_Normative-References.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-0_Scope.md) | **Next**: [[REF_TBD]](01-2_Terms-Definitions.md)

---

## 1.1.1 ISO/IEC Standards

The following ISO/IEC standards contain provisions which, through reference in this text, constitute provisions of this specification:

### ISO/IEC Directives, Part 2 (2021)

**ISO/IEC Directives, Part 2: Principles and rules for the structure and drafting of ISO and IEC documents** (Eighth edition, 2021)

Referenced for normative language conventions (`shall`, `should`, `may`) defined in [REF_TBD]

**Availability**: https://www.iso.org/directives-and-policies.html

### ISO/IEC 10646:2020

**ISO/IEC 10646:2020 Information technology — Universal Coded Character Set (UCS)**

Referenced for:
- Character repertoire ([REF_TBD]), [REF_TBD]
- UTF-8 encoding specification ([REF_TBD])
- Unicode scalar value definition ([REF_TBD])
- Character classification ([REF_TBD])

**Applicable Version**: ISO/IEC 10646:2020 or later

**Note**: This specification uses the Unicode Standard ([REF_TBD]), which maintains alignment with ISO/IEC 10646.

### ISO/IEC 60559:2020 (IEC 60559:2020)

**ISO/IEC 60559:2020 Information technology — Microprocessor Systems — Floating-Point arithmetic**

Also published as **IEEE 754-2019: IEEE Standard for Floating-Point Arithmetic**

Referenced for:
- Floating-point type semantics ([REF_TBD])
- Special values (NaN, infinity)
- Rounding modes
- Exception handling

**Applicable Version**: ISO/IEC 60559:2020 (equivalent to IEEE 754-2019)

**Note**: Conforming implementations shall support IEEE 754 binary32 (`f32`) and binary64 (`f64`) formats.

### RFC 2119 and RFC 8174

**RFC 2119: Key words for use in RFCs to Indicate Requirement Levels** (March 1997)
**RFC 8174: Ambiguity of Uppercase vs Lowercase in RFC 2119 Key Words** (May 2017)

Referenced for normative language keyword definitions ([REF_TBD])

**Availability**:
- RFC 2119: https://www.rfc-editor.org/rfc/rfc2119
- RFC 8174: https://www.rfc-editor.org/rfc/rfc8174

**Note**: This specification uses ISO/IEC Directives terminology (`shall`/`should`/`may`) but notes equivalence to RFC 2119 keywords for readers familiar with IETF standards.

## 1.1.2 Unicode Standard

### The Unicode Standard, Version 15.0.0 or later

**The Unicode Standard, Version 15.0.0** (September 2022)

Referenced for:
- Character properties ([REF_TBD])
- Normalization forms (NFC, NFD, NFKC, NFKD) ([REF_TBD])
- Character categories (Letter, Number, Symbol, etc.)
- Identifier syntax (UAX #31)
- Grapheme cluster boundaries

**Applicable Version**: Unicode 15.0.0 or later

**Availability**: https://www.unicode.org/versions/Unicode15.0.0/

**Note**: Conforming implementations should use the latest Unicode version available at implementation time. Programs remain valid across Unicode versions unless explicitly dependent on specific character properties.

### Unicode Technical Standard #18 (UTS #18)

**Unicode Regular Expressions**

Referenced for pattern matching semantics (future extension).

### Unicode Technical Standard #31 (UAX #31)

**Unicode Identifier and Pattern Syntax**

Referenced for identifier syntax rules ([REF_TBD])

**Applicable Version**: Unicode 15.0.0 UAX #31 or later

## 1.1.3 Other Normative References

### Cursive Standard Library Specification

**Cursive Standard Library Specification, Version 1.0**

The standard library specification is a separate normative document maintained independently from the language specification. See [REF_TBD]

**Cross-references**: This language specification may reference types, predicates, and modules from the standard library (e.g., collection types, `Copy` predicate, I/O modules).

### System V Application Binary Interface (ABI)

**System V Application Binary Interface** (various CPU supplements)

Referenced for:
- Default calling conventions on Unix-like systems ([REF_TBD])
- Struct layout and alignment (implementation-defined, [REF_TBD]
- Name mangling (implementation-defined, [REF_TBD]

**Note**: ABI conformance is implementation-defined. This specification does not mandate a specific ABI, but implementations shall document their ABI choices.

**Availability**:
- AMD64 ABI: https://gitlab.com/x86-psABIs/x86-64-ABI
- ARM AAPCS: https://github.com/ARM-software/abi-aa

### C Language Standards (for FFI)

**ISO/IEC 9899:2018 (C17/C18)** or **ISO/IEC 9899:2023 (C23)**

Referenced for:
- C type compatibility ([REF_TBD])
- C calling conventions ([REF_TBD])
- C string representation ([REF_TBD])
- C variadic function semantics ([REF_TBD])

**Note**: FFI compatibility targets C17/C18 as baseline. Implementations may support newer C standards.

---

## Availability of Referenced Documents

### Freely Available:

- RFC 2119, RFC 8174: Available from IETF (https://www.rfc-editor.org/)
- Unicode Standard: Available from Unicode Consortium (https://www.unicode.org/)
- System V ABI documents: Available from respective architecture communities

### Purchase Required:

- ISO/IEC standards: Available from ISO (https://www.iso.org/) and national standards bodies
- IEEE standards: Available from IEEE (https://www.ieee.org/)

### Normative vs. Informative Use:

All references in this section are **normative** — conforming implementations shall comply with the referenced specifications where applicable. Informative references appear in individual sections and annexes.

---

## Updates to Referenced Standards

When referenced standards are updated:

1. **Backward Compatibility**: Programs valid under earlier versions remain valid unless:
   - Explicitly dependent on deprecated features
   - Relying on undefined behavior clarified in updates

2. **Forward Compatibility**: Implementations should use the latest available version of referenced standards, particularly:
   - Unicode (character properties evolve)
   - IEEE 754 (clarifications and errata)

3. **Edition Boundaries**: Major updates to referenced standards may trigger new language editions (see [REF_TBD]

---

**Previous**: [[REF_TBD]](01-0_Scope.md) | **Next**: [[REF_TBD]](01-2_Terms-Definitions.md)
