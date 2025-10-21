# 210 — Records, Enums, Traits (Normative)

## Records
Labeled product types with field visibility. `#[repr(C|packed|align(N))]` controls
layout where applicable.

## Enums
Tagged unions; discriminants MAY be explicit. Exhaustive `match` is required.

## Traits
Interfaces with required and optional default procedures.

### Trait‑Declared Effects
A trait method may declare an upper bound on effects:
```cantrip
trait Logger {
    function log(self: Self, msg: String) needs io.write;
}
impl Logger for My { function log(self: Self, msg: String) needs io.write { ... } }
```
Implementations MUST satisfy `ε_impl ⊆ ε_trait`. Violation: `E9120`.

### Coherence and Orphan Rules
Global coherence: at most one `impl Trait for Type` in the whole program.  
Orphan rule: an impl is allowed iff the trait **or** the outermost type
constructor is local. Violations: `E8201`, `E8202`.
