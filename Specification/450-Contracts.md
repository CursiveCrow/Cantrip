# 450 — Contracts (Normative)

Functions may declare **preconditions** (`requires`) and **postconditions**
(`ensures`). `result` refers to the return value; `@old(expr)` captures entry‑time
values. Contracts are verified statically when possible and otherwise at runtime
depending on the verification mode.

### Verification Modes
- `#[verify(static)]` — proofs required or compilation fails
- `#[verify(runtime)]` — checks emitted (default)
- `#[verify(none)]` — checks suppressed (unsafe)

### Example
```cantrip
function withdraw(a: mut Account, amount: f64)
    requires amount > 0.0; requires a.balance >= amount;
    ensures  a.balance == @old(a.balance) - amount;
{ a.balance -= amount; }
```
