# Migration: 0.7 → 1.0

- **Syntax removed:** legacy `requires<effects>` and `returns<T>` are errors
  (`E1010`/`E1011`). Use `needs` and `: T`.
- **Trait effect bounds:** implementations MAY NOT exceed trait-declared needs
  (new `E9120`).
- **Async effect masks:** awaiting imports effects; missing atoms cause `E9201`.
- **Typed holes:** formalized as traps in runtime verify; new `E9501–E9503`.
- **Forbidden effects:** clarified and tightened (`E9010` redundant cases).
