# 400 — Functions and Expressions (Normative)

### Declaration
```cantrip
function name(params): ReturnType
    needs effects;        // OPTIONAL: omit if pure
    requires conditions;  // OPTIONAL
    ensures conditions;   // OPTIONAL
{ body }
```

Parameters are references by default. `own` takes ownership (requires `move`),
`mut` requests a mutable reference, and `iso` is an isolated unique reference.

### Expressions
Arithmetic, comparison, logical, bitwise, assignment, ranges, `if`, `while`,
`for`, `loop`, `break`, `continue`, `return`, pattern `match`, pipelines `=>`.

### Typed Holes
- `?name: Type` or bare `?` participate in type checking.
- Unfilled holes: `E9501` (static verify mode), `E9502` warning (runtime verify),
  elaborating to a trap that requires `needs panic`.
- Holes MUST NOT widen a function’s declared effects (`E9503`).

### Pipeline Operator
`e1 => f => g` desugars to `g(f(e1))`, left‑associative.
