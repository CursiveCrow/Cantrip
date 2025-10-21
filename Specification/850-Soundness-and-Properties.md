# 850 — Soundness & Meta‑Properties (Normative)

- **Type Soundness:** Progress + preservation for core expression forms.
- **Effect Soundness:** If `Γ ⊢ e : T ! ε` and `⟨e, σ, ∅⟩ →* ⟨v, σ', ε'⟩` then `ε' ⊆ ε`.
- **Modal Safety:** Valid transitions preserve state invariants.

Sketch proofs and lemmas are provided in Appendix E (informative outlines for mechanization).
