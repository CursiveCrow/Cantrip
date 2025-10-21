# 600 — Memory Model & Permissions (Normative)

Cantrip provides memory safety and deterministic deallocation via **regions** and
**permissions**, without borrow checking or GC.

Permissions:
- reference (default param): read‑only, many aliases
- `mut T`: read/write, many aliases (programmer ensures safety)
- `own T`: unique ownership; must use `move` to transfer
- `iso T`: provably unique, transferable across threads

Guarantees: no use‑after‑free, no double‑free, no leaks within regions.  
Non‑guarantees: aliasing bugs, data races, iterator invalidation.
