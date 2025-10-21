# 520 — Effect Rules, Checking, and Async Masks (Normative)

### Rules
- Function body effects MUST be a subset of declared `needs`.
- Higher‑order functions use `needs effects(F)` to propagate callback effects.
- Wildcards (e.g., `fs.*`) expand to the family; use `!fs.delete` to exclude.

### Async Effect Masks
`await f` imports `f`’s declared effects to the enclosing function. If missing
from the signature, emit `E9201` with a fix‑it to add the exact atoms (e.g.,
`net.read(outbound)`). This is not inference; the code still fails until the
signature is updated.
