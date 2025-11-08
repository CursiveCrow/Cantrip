# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions  
**File**: 08-3_Unary-and-Binary-Operators.md  
**Section**: §8.3 Unary and Binary Operators  
**Stable label**: [expr.operator]  
**Forward references**: §8.4 [expr.structured], §8.6 [expr.conversion], Clause 5 §5.2 [decl.variable], Clause 7 §7.2 [type.primitive], Clause 7 §7.5 [type.pointer], Clause 7 §7.7 [type.relation], Clause 11 §11.4 [memory.permission]

---

### §8.3 Unary and Binary Operators [expr.operator]

#### §8.3.1 Overview

[1] Operators extend expressions with prefix (`!`, `-`, `&`, `move`), infix (arithmetic, comparison, logical, range), exponentiation, and assignment semantics. Annex A §A.4.9 defines their precedence and associativity. This subclause restates the table, then specifies typing, evaluation, and diagnostics for each operator.

#### §8.3.2 Precedence hierarchy

[2] Table 8.3.1 lists operators from highest to lowest precedence. All binary operators associate left-to-right except exponentiation and assignments, which associate right-to-left.

| Level | Operators                                      | Associativity        | Notes    |
| ----- | ---------------------------------------------- | -------------------- | -------- |
| 1     | Postfix (`()`, `[]`, `.`, `::`, pipeline `=>`) | Left                 | See §8.2 |
| 2     | Prefix (`!`, `-`, `&`, `move`)                 | Right                | §8.3.3   |
| 3     | `**`                                           | Right                | §8.3.4   |
| 4     | `*`, `/`, `%`                                  | Left                 | §8.3.4   |
| 5     | `+`, `-`                                       | Left                 | §8.3.4   |
| 6     | `<<`, `>>`                                     | Left                 | §8.3.4   |
| 7     | `..`, `..=` (binary form)                      | Left                 | §8.3.5   |
| 8     | Bitwise `&`                                    | Left                 | §8.3.4   |
| 9     | Bitwise `^`                                    | Left                 | §8.3.4   |
| 10    | Bitwise `\|`                                   | Left                 | §8.3.4   |
| 11    | `<`, `<=`, `>`, `>=`                           | Left                 | §8.3.6   |
| 12    | `==`, `!=`                                     | Left                 | §8.3.6   |
| 13    | `&&`                                           | Left (short-circuit) | §8.3.7   |
| 14    | `\|\|`                                         | Left (short-circuit) | §8.3.7   |
| 15    | `=`, `op=`                                     | Right                | §8.3.8   |

#### §8.3.3 Unary operators

[3] Grammar: `UnaryExpr ::= ('!' | '-' | '&' | 'move') UnaryExpr | PostfixExpr` (Annex A §A.4.9).

**Logical NOT (`!`)**

[4] Operand must have type `bool`. Result type is `bool`. Using a non-boolean operand yields E08-320.

**Arithmetic negation (`-`)**

[5] Operand must belong to `NumericTypes`. Result type equals operand type. Negating the minimum signed integer in checked builds triggers a panic with diagnostic E08-330; release builds wrap according to Clause 7 §7.2. Developers may opt into always-checked or always-wrapping modes via attributes (Clause 1 §1.6).

**Address-of (`&`)**

[6] Covered in §8.2.10; invalid uses emit E08-260.

**Move (`move e`)**

[7] The `move` operator explicitly transfers cleanup responsibility from a binding to the move expression's consumer. It is required when passing responsible bindings to procedures that accept responsibility for cleanup.

**Syntax**:
```ebnf
move_expr ::= "move" identifier
```

**Constraints**:

(7.1) The operand shall be an identifier referring to a `let` binding created with `= value` (responsible and transferable). Attempting to move from non-transferable bindings produces diagnostics:
- `var` bindings: E11-501 (cannot transfer from var binding)
- Non-responsible bindings (`<-`): E11-502 (cannot transfer from non-responsible binding)

(7.2) The binding shall be accessible in the current scope and shall not have been previously moved. Using a moved binding produces E11-503 (use of moved value).

**Typing rule**:

[ Given: Binding `x` with type `τ` and cleanup responsibility ]

$$
\frac{\Gamma \vdash x : \tau \quad x \text{ is responsible and transferable}}{\Gamma \vdash \texttt{move } x : \tau \quad x \text{ becomes invalid}}
\tag{T-Move}
$$

**Semantics**:

(7.3) Evaluation transfers cleanup responsibility to the recipient. The moved-from binding becomes invalid and shall not be used again in the current scope. Definite assignment analysis (§5.7) tracks moved bindings and prevents further use.

(7.4) **Invalidation propagation**: When a binding is moved, all non-responsible bindings derived from it (created via `let n <- source`) also become invalid. This prevents use-after-free by ensuring that references cannot access values after ownership has transferred (§5.7.5.2, §11.5.5.1A).

(7.5) At scope exit, moved bindings do not invoke destructors (responsibility has transferred). Non-moved bindings invoke destructors normally per RAII rules (§11.2).

**Examples**:

**Example 8.3.3.1 (Valid move transfer):**
```cursive
procedure consume(buffer: Buffer)
    [[ |- true => true ]]
{
    // buffer now responsible for cleanup
}

procedure demo()
    [[ alloc::heap |- true => true ]]
{
    let data = Buffer::new()     // data is responsible
    consume(move data)            // Responsibility transferred
    // data is now invalid
    // data.size()                // error[E11-503]: use of moved value
}
```

**Example 8.3.3.2 - invalid (Move from var):**
```cursive
var counter = 0
let moved = move counter  // error[E11-501]: cannot transfer from var binding
```

**Example 8.3.3.3 - invalid (Move from reference binding):**
```cursive
let original = Buffer::new()
let ref <- original
consume(move ref)  // error[E11-502]: cannot transfer from non-responsible binding
```

**Example 8.3.3.4 (Invalidation propagation to derived bindings):**
```cursive
let owner = Buffer::new()      // Responsible
let viewer <- owner            // Non-responsible (depends on owner)

consume(move owner)            // owner becomes invalid
// viewer ALSO becomes invalid (derived from moved binding)
// viewer.read()               // error[E11-504]: use of moved-derived binding
```

#### §8.3.4 Arithmetic and bitwise operators

[8] Grammar: `MulExpr`, `AddExpr`, `ShiftExpr`, `BitAndExpr`, `BitXorExpr`, `BitOrExpr` in Annex A §A.4.9.

[9] Typing rules:

- Operands shall have identical numeric type `τ`. Mixed-type arithmetic requires explicit casts (§8.6) and therefore is ill-formed by default (E08-301).
- `%` is defined only for integer types. Using it with floating types emits E08-302.
- Shift operators require left operand of integer type `τ` and right operand of type `usize`. The shift amount must satisfy `amount < bitwidth(τ)`; otherwise E08-303.

[10] Evaluation: both operands evaluate left-to-right. Division or modulo by zero panic with diagnostic E08-304. Integer overflow follows the semantics configured for the compilation mode: checked builds panic, release builds wrap (Clause 7 §7.2). Bitwise operators operate on the binary representation without additional checks.

#### §8.3.5 Power and range operators

[11] Exponentiation `**` accepts numeric operands of identical type and returns that type. It associates right-to-left, so `a ** b ** c` parses as `a ** (b ** c)`.

[12] Binary range operators `a..b` and `a..=b` create range values (Clause 7 §7.3.4). Operands must have the same numeric type. Evaluation occurs left-to-right: compute `a`, compute `b`, then package the range structure. Invalid ordering (e.g., `b < a`) is permitted; it is diagnosed only when consumers demand ordered ranges (e.g., slicing).

#### §8.3.6 Comparison and equality

[13] `<`, `<=`, `>`, `>=` require operands of identical numeric or `char` type. Result is `bool`. Comparisons on `f32`/`f64` follow IEEE 754 semantics (`NaN` is unordered).

[14] `==` and `!=` are available exactly when the operand type implements the `Eq` predicate (Clause 11). Primitive types implement `Eq` by default. Custom types shall derive or implement `Eq`; otherwise E08-310 is emitted. Equality is symmetric and reflexive for non-`NaN` values.

#### §8.3.7 Logical operators

[15] `&&` and `||` require operands of type `bool`. Both operators short-circuit: `a && b` evaluates `b` only when `a` is `true`; `a || b` evaluates `b` only when `a` is `false`. Their result is `bool`. Mis-typed operands emit E08-320.

[16] Bitwise XOR (`^`) is distinct from logical XOR; it operates on integers. For boolean logical XOR, compose `a != b`.

#### §8.3.8 Assignment and compound assignment

[17] Grammar: `AssignExpr ::= UnaryExpr AssignmentOperator Expr`, where `AssignmentOperator ∈ { '=', '+=', '-=', '*=', '/=', '%=', '&=', '|=', '^=', '<<=', '>>=' }` (Annex A §A.4.9).

[18] Typing constraints:

- Left operand must be a place with permission `mut` or `own` and type `τ`.
- Right operand must have type `τ`.
- `op=` forms require `op` to be valid for type `τ` (e.g., `+=` valid on numeric types).

[19] Evaluation order: evaluate the left operand to obtain the target place, then evaluate the right operand, then perform the assignment. For compound assignments, the compiler reads the current value of the place exactly once, applies the operator, and writes the result back.

[20] Diagnostics:

- Non-place left operand → E08-340.
- Insufficient permission → E11-403.
- Unsupported compound operator/type combination → E08-341.
- Chained assignment attempts (e.g., `x = y = z`) emit E08-342 unless each assignment is standalone.

#### §8.3.9 Move interaction

[21] Operators follow move semantics. If an operand’s type is move-only (`own`) and lacks `Copy`, the operator consumes it; subsequent uses of the binding are ill-formed (Clause 5 diagnostics). Commutative operators still evaluate left-to-right, so moving the left operand before evaluating the right operand is observable.

#### §8.3.10 Diagnostics summary

| Code    | Condition                                              |
| ------- | ------------------------------------------------------ |
| E08-301 | Operand types mismatch for arithmetic/bitwise operator |
| E08-302 | Modulo operator applied to non-integer type            |
| E08-303 | Shift amount ≥ bit width                               |
| E08-304 | Division or modulo by zero                             |
| E08-310 | Equality invoked on type without `Eq` implementation   |
| E08-320 | Logical operator operands not `bool`                   |
| E08-330 | Checked-mode arithmetic negation overflow              |
| E08-331 | `move` applied to value without ownership              |
| E08-340 | Assignment target is not a place                       |
| E08-341 | Compound assignment invalid for operand type           |
| E08-342 | Chained assignment is ill-formed                       |

#### §8.3.11 Canonical example

```cursive
var counter: mut i32 = 0

procedure tick(delta: i32): ()
    [[ |- true => true ]]
{
    counter += delta                 // compound assignment
    if counter < 0 {
        counter = 0
    }
    if counter % 5 == 0 && delta != 0 {
        log::info("milestone reached")
    }
}
```

[22] The example demonstrates compound assignment, comparison, modulo, and logical short-circuiting, while respecting evaluation order and permissions.

---
