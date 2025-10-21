# 000 — Front Matter (Normative)

**Version:** 1.0.0  
**Status:** Normative Reference  
**Audience:** Language implementers, library authors, application developers

## Key Words for Requirement Levels

The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**,
**SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **MAY**, and **OPTIONAL** are to be
interpreted as in RFC 2119 / RFC 8174 when, and only when, they appear in ALL CAPS.

## Normative vs. Informative

Sections explicitly marked **(Normative)** define conformance requirements.
Sections marked **(Informative)** provide guidance and are non-binding.

## Conformance

An implementation conforms to this specification iff it satisfies **all**
normative requirements across the SPEC-1.0 documents. Extensions MUST NOT
invalidate any program that is valid under this spec. Implementation-defined
limits and unspecified behavior MUST be documented.

## Document Conventions

- Language code fences use `cantrip` (examples are informative unless stated).
- Grammar productions use `ebnf` fences.
- Error identifiers like `E9001` are normative and MUST be reported for the
  corresponding violations.
- All effects are explicit: absence of a `needs` clause means **pure**.

## Compatibility & Provenance (Informative)

This 1.0 edition organizes and clarifies the material from the 0.7.x drafts,
removing deprecated syntaxes and freezing the effect/modals/contracts model
as the 1.0 baseline. A migration guide is provided in `GUIDES/`.
