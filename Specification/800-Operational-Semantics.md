# 800 — Operational Semantics (Normative)

We provide small‑step and big‑step semantics over configurations ⟨e, σ⟩ (expression
and store). Evaluation order is left‑to‑right. Effects accumulate monotonically
and are bounded by the declared `needs` (effect soundness). Typed holes elaborate
to traps in runtime mode.
