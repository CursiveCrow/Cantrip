# 550 — Effect Budgets (Normative)

Quantitative constraints:
- Memory: `alloc.heap(bytes<=N)` (also `KiB|MiB|GiB`)
- Time: `time.sleep(duration<=Ds|ms|min)`
- Count: `thread.spawn(count<=N)`; directioned reads may carry counts

Budgets compose additively across sequences. Static proofs MAY discharge fixed
bounds; otherwise runtime tracking MUST enforce limits and panic on overflow.
