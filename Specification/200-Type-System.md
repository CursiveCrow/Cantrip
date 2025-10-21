# 200 — Types and Values (Normative)

Primitive numeric (`i8..i64`, `u8..u64`, `isize`, `usize`), float (`f32`,`f64`),
`bool`, `char`, `str`, arrays `[T; n]`, slices `[T]`, tuples, function types
`fn(A,...): R`, procedure types `proc(Self, A,...): R`, permissions (`own T`,
`mut T`, `iso T`), modal states `T@State`, polymorphism `∀α.T`, and `!` (never).

## 200.1 Never Type
`!` has no values and is a subtype of all types. It inhabits non‑returning code.

## 200.2 Arrays and Slices
- `[T; n]` is contiguous; index is bounds‑checked in debug builds.
- `[T]` is a fat pointer `(data: *T, length: usize)`.

## 200.3 Subtyping
```
! <: T
own T <: mut T <: T
iso T <: own T
```
Function types are contravariant in parameters and covariant in results.

## 200.4 Inference
Local variables and return types may be inferred from bodies; **effects are
never inferred** (see §500).
