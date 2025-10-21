# 950 — Tooling & Implementation Notes (Informative)

- **Compiler architecture:** front‑end (lex/parse/AST), typing, modal checker,
  effects checker, contract verifier, codegen.
- **Diagnostics:** machine‑readable JSON per `SCHEMAS/diagnostics.schema.json`.
- **Package manager:** deterministic builds; verify mode flags propagate.
- **Testing:** contract fuzzing hooks; property tests and conformance suites.
