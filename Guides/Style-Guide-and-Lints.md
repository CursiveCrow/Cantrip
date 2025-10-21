# Style Guide & Lints (Informative)

- Prefer named effects from `std.effects` over ad‑hoc unions.
- Single canonical function form: signature, then `needs/requires/ensures`, then body.
- Use explicit `move` for ownership transfers; avoid implicit semantics in examples.
- Keep modal state names in `CamelCase` and procedures in `snake_case`.
- Provide contract messages for user‑facing API preconditions.
- Lints: `missing-needs`, `redundant-forbidden-effect`, `unused-move`, `unreachable-state`.
