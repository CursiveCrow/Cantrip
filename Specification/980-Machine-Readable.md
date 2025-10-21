# 980 — Machine‑Readable Output (Normative)

Compilers and tools MUST support JSON output for diagnostics and symbol indices.

## 980.1 Diagnostics JSON
Conforms to `SCHEMAS/diagnostics.schema.json`. Each diagnostic includes:
- `code`, `severity`, `message`, `file`, `range`
- `notes[]`, `fixits[]`
- Optional `typedHole` object: `{ "name": "n", "type": "String" }`

## 980.2 Effect Set JSON
`SCHEMAS/effects.schema.json` defines serialization of declared and inferred
(per expression) effects for IDEs.

## 980.3 Symbol Index
Emit per‑module symbol index with `aliases[]` when `emitAliases=true`.
