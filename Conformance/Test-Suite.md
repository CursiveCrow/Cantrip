# Conformance Test Plan

- **Positive tests:** accept and run; no diagnostics beyond warnings.
- **Negative tests:** MUST fail with the exact code and message class indicated.
- **Budgeted tests:** MUST track and enforce declared budgets.
- **Modal tests:** MUST reject illegal transitions at compile time.
- **Async masks:** awaiting a future with undeclared effects MUST trigger E9201.
