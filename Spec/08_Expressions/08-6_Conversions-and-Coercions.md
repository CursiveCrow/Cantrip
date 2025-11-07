# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-6_Conversions-and-Coercions.md  
**Section**: §8.6 Conversions and Coercions  
**Stable label**: [expr.conversion]  
**Forward references**: §8.7 [expr.constant], Clause 7 §7.2 [type.primitive], Clause 7 §7.5 [type.pointer], Clause 7 §7.6 [type.modal], Clause 11 [generic]

---

### §8.6 Conversions and Coercions [expr.conversion]

#### §8.6.1 Overview

[1] Cursive differentiates between **explicit casts** (`expr as Type`) and **implicit coercions** performed by the type checker. This subclause enumerates the legal cast categories, the semantics of each, and the narrow set of coercions that occur automatically.

#### §8.6.2 Explicit Casts (`as`)

[2] Grammar: `CastExpr ::= UnaryExpr 'as' Type` (Annex A §A.4.17).

[3] Typing rules:

- **Numeric-to-numeric:** `(τ_src) as τ_dst` is permitted when both are primitive numeric types. Narrowing conversions may lose information; constant expressions that cannot be represented in `τ_dst` emit E08-600.
- **Pointer casts:** `Ptr<T>@Valid as Ptr<U>@Valid` requires a proof that `T` and `U` have identical layout (Clause 12). Casting to the unconstrained pointer `Ptr<U>` is always legal (widening). Raw pointer casts (`*const T as *const U`) require grant `unsafe.ptr` and are unchecked at runtime.
- **Modal widening:** `ModalType@State as ModalType` is always legal. Narrowing (introducing a specific state) is illegal without a witness from a transition, and attempting it emits E08-601.
- **Enum discriminants:** `Enum::Variant(...) as usize` returns the discriminant value. Casting integers back to enums is illegal because it would bypass exhaustiveness checking.
- **Boolean casts:** Only conversions that already produce `bool` (e.g., comparison results) are allowed. Numeric-to-bool casts are forbidden to avoid implicit truthiness.

[4] All other casts are rejected with E08-601.

[5] Evaluation: casts evaluate their operand first, then apply the conversion. Numeric casts follow the two’s-complement and IEEE 754 rules in Clause 7. Pointer casts are bit reinterpretations; they do not alter permissions or provenance.

#### §8.6.3 Implicit Coercions

[6] The type checker performs the following coercions automatically:

- **Unit coercion:** `()` may be inserted when a block lacking `result` feeds a context expecting `()`.
- **Pointer/modal widening:** `Ptr<T>@State` coerces to `Ptr<T>`, and `ModalType@State` coerces to `ModalType` per the subtyping rules in §7.7.
- **String defaulting:** bare `string` in type position defaults to `string@View`; `string@Owned` coerces to `string@View` when required.
- **Pipeline equality:** when a pipeline stage omits its type annotation, the checker inserts an equality coercion `τ → τ`, verifying equivalence before allowing the omission (§8.1.3).

[7] No other implicit conversions occur. In particular, numeric promotions (e.g., `i32` + `f32`) are forbidden; developers must insert explicit casts when combining disparate types.

#### §8.6.4 Diagnostics

| Code | Condition |
| --- | --- |
| E08-600 | Constant cast loses information |
| E08-601 | Cast not covered by an allowed category |

#### §8.6.5 Example

```cursive
let precise: f64 = distance as f64
let bucket: u8 = (hash as u32 % 256u32) as u8  // narrowing permitted
let view: string@View = owned_string as string@View  // coercion via `as`
```

[8] The example shows explicit numeric casts, a narrowing conversion that truncates runtime values, and a modal/string coercion that forces the type checker to treat `string@Owned` as a view explicitly.

---
