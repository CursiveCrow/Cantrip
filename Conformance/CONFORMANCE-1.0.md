# Cantrip 1.0 Conformance (Normative)

An implementation **conforms** if it:
1. Accepts all programs in `TESTS/positive` and rejects those in `TESTS/negative`
   with the specified error codes.
2. Enforces all effect declarations and budgets (§500–§550).
3. Verifies modal transitions and rejects invalid calls (`E3003`).
4. Implements contracts per verification mode (§450).
5. Supports machine‑readable outputs (§980) and reports the required codes (§970).

Implementations MAY provide extensions, but MUST document them and MUST NOT
invalidate valid 1.0 programs.
