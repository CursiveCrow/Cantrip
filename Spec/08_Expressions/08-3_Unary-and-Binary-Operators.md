# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-3_Unary-and-Binary-Operators.md  
**Section**: §8.3 Unary and Binary Operators  
**Stable label**: [expr.operator]  
**Forward references**: §8.4 [expr.structured], §8.6 [expr.conversion], Clause 5 §5.2 [decl.variable], Clause 7 §7.2 [type.primitive], Clause 7 §7.5 [type.pointer], Clause 12 §12.4 [memory.permission]

---

### §8.3 Unary and Binary Operators [expr.operator]

#### §8.3.1 Overview

[1] Operator expressions extend primary/postfix expressions with prefix (`!`, `-`, `&`, `move`), infix (arithmetic, comparison, logical, range), and assignment forms. Annex A §A.4.9 defines the precedence and associativity hierarchy. This subclause restates the normative table and specifies the typing, evaluation order, and diagnostics for every operator.

#### §8.3.2 Precedence and Associativity

[2] Table 8.3.1 lists operator levels from highest to lowest precedence. All binary operators associate left-to-right except exponentiation (`**`) and assignments (`=`, `op=`), which associate right-to-left. Pipeline `=>` is handled in §8.2 but appears in the table for completeness.

| Level | Operators | Associativity | Reference |
| --- | --- | --- | --- |
| 1 | Postfix (`()`, `[]`, `.`, `::`, `=>`) | left | §8.2 |
| 2 | Prefix (`!`, `-`, `&`, `move`) | right | §8.3.3 |
| 3 | Power `**` | right | §8.3.4 |
| 4 | `*`, `/`, `%` | left | §8.3.4 |
| 5 | `+`, `-` | left | §8.3.4 |
| 6 | `<<`, `>>` | left | §8.3.4 |
| 7 | `..`, `..=` | left | §8.3.5 |
| 8 | `&` (bitwise) | left | §8.3.4 |
| 9 | `^` (bitwise) | left | §8.3.4 |
| 10 | `|` (bitwise) | left | §8.3.4 |
| 11 | `<`, `<=`, `>`, `>=` | left | §8.3.6 |
| 12 | `==`, `!=` | left | §8.3.6 |
| 13 | `&&` | left (short-circuit) | §8.3.7 |
| 14 | `||` | left (short-circuit) | §8.3.7 |
| 15 | `=`, `op=` | right | §8.3.8 |

#### §8.3.3 Unary Operators

[3] Grammar: `UnaryExpr ::= ('!' | '-' | '&' | 'move') UnaryExpr | PostfixExpr` (Annex A §A.4.9).

**Logical NOT (`!`)**

[4] Typing: operand must have type `bool`. Result type is `bool`.

$$
\frac{\Gamma \vdash e : bool ! \varepsilon}{\Gamma \vdash !e : bool ! \varepsilon}
\tag{T-Not}
$$

**Arithmetic negation (`-`)**

[5] Operand must be a numeric primitive (`τ ∈ NumericTypes`). Result type is `τ`. Negating the minimum signed integer in checked mode causes a panic; release mode wraps per §7.2.

**Address-of (`&`)**

[6] Covered in §8.2.10. Attempting to take the address of a non-place expression yields E08-260.

**Move (`move e`)**

[7] `move` consumes an `own` value to prevent implicit copies. It has type `typeof(e)` and category `value`. Using the moved binding afterwards results in the standard “use after move” diagnostic (Clause 5). The operand must be a value; attempting to move a place without ownership emits E08-300.

#### §8.3.4 Arithmetic and Bitwise Operators

[8] Grammar: `AddExpr`, `MulExpr`, `ShiftExpr`, `BitAndExpr`, `BitXorExpr`, `BitOrExpr` (Annex A §A.4.9).

[9] Typing:

- Operands shall share the same numeric type `τ`. Mixing integer and floating operands is ill-formed (E08-301).
- `%` requires an integer type; using floating point produces E08-302.
- Shifts require the right operand to be `usize`; the amount must be less than the bit width, otherwise E08-303.

[10] Evaluation: operands evaluate left-to-right. Division or remainder by zero cause runtime panic (diagnostic E08-304). Integer overflow follows the debug/optimized semantics described in Clause 7.

#### §8.3.5 Power and Range Operators

[11] Exponentiation `**` requires numeric operands with matching type. Result type is the operand type. Overflow rules follow §7.2.

[12] Binary range operators (`a..b`, `a..=b`) construct `Range<T>` or `RangeInclusive<T>` values (§7.3.4). Operands must share the same numeric type. Range literals are value expressions; slicing semantics are handled in §8.2.9.

#### §8.3.6 Comparison and Equality

[13] Comparison operators `<`, `<=`, `>`, `>=` accept numeric or `char` operands of the same type and produce `bool`. For `f32`/`f64`, comparisons follow IEEE 754; `NaN` propagates (i.e., `NaN == NaN` is `false`).

[14] Equality operators `==`, `!=` require the type to implement the `Eq` predicate (Clause 11). Primitive types satisfy `Eq`; structural types must derive or implement it. Attempting to compare a type without `Eq` yields E08-310.

#### §8.3.7 Logical Operators

[15] `&&` and `||` require operands of type `bool`. They short-circuit: `a && b` evaluates `b` only when `a` is `true`, and `a || b` evaluates `b` only when `a` is `false`.

[16] Diagnostics: using non-boolean operands produces E08-320.

#### §8.3.8 Assignment and Compound Assignment

[17] Grammar: `AssignExpr ::= UnaryExpr AssignmentOperator Expr` (Annex A §A.4.9). `AssignmentOperator` is `=` or `op=`.

[18] Typing:

- Left operand must be a place with `mut` or `own` permission and type `τ`.
- Right operand must have type `τ` (or be convertible via explicit cast).
- Compound assignments `target op= expr` are equivalent to `target = target op expr` but evaluate `target` exactly once.

[19] Diagnostics:

- Assigning to a non-place expression emits E08-330.
- Attempting to mutate a `let` binding or place without sufficient permission emits E08-003.
- Using compound assignment with unsupported operator/type combinations emits E08-331.

#### §8.3.9 Move Interaction and Temporaries

[20] Operators respect move semantics. When an operand of type `own T` lacks the `Copy` predicate, the operator consumes it, and later uses must treat the binding as moved. Temporaries introduced by operators drop at the end of the full expression (§12.2).

#### §8.3.10 Diagnostics Summary

| Code | Condition |
| --- | --- |
| E08-300 | `move` applied to value lacking ownership |
| E08-301 | Operands of numeric operator have mismatched types |
| E08-302 | Modulo operator used with floating type |
| E08-303 | Shift amount ≥ bit width |
| E08-304 | Division or remainder by zero |
| E08-310 | Equality used on type without `Eq` |
| E08-320 | Logical operator operands not `bool` |
| E08-330 | Assignment target is not a place |
| E08-331 | Unsupported compound assignment |

#### §8.3.11 Canonical Example

```cursive
var counter: mut i32 = 0
procedure step(delta: i32): ()
    {| |- true => true |}
{
    counter += delta          // compound assignment evaluates `counter` once
    if counter >= LIMIT && delta != 0 {
        counter = 0
    }
}
```

[21] This example exercises compound assignment, logical short-circuiting, and ordinary assignment, illustrating the permission requirements on the left-hand side.

---
