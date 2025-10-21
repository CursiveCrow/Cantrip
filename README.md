# Cantrip 1.0 Specification — Complete Document Suite

This bundle contains the **full set of documents** needed to ship and implement
the Cantrip language **1.0** specification: the normative language spec parts,
appendices, machine‑readable schemas, the conformance and test plan, migration
and style guides, governance, and release notes.

> Provenance: This 1.0 suite consolidates and normalizes material from the prior
> *Cantrip Language Specification v0.7.x* while applying editorial fixes,
> clarifications, and a few targeted normative tightenings (e.g., trait-declared
> effect bounds, async effect masks, and typed holes rules).

### Directory Map

- `SPEC-1.0/` — Normative language specification split by topic
- `CONFORMANCE/` — Conformance definition, compliance matrix template, test plan
- `GUIDES/` — Migration (0.7→1.0), style & lints
- `GOVERNANCE/` — Versioning & deprecation policy; release notes
- `SECURITY/` — Safety model and threat considerations
- `SCHEMAS/` — Machine-readable JSON schemas for tools and IDEs
- `TESTS/` — Sample conformance tests (positive/negative)
- `STDLIB/` — Standard effects catalog and index

Each spec file is **normative** unless it explicitly says “Informative.”

### Quick Start

- Read `SPEC-1.0/000-Front-Matter.md` for key words, conformance, and document
  conventions.
- Implementers should work through `CONFORMANCE/CONFORMANCE-1.0.md` and wire up
  the machine-readable outputs described in `SCHEMAS/`.
- Developers can skim `GUIDES/Migration-0.7-to-1.0.md` and `GUIDES/Style-Guide-and-Lints.md`.
