# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-4_Structured-Expressions.md  
**Section**: §8.4 Structured Expressions  
**Stable label**: [expr.structured]  
**Forward references**: §8.5 [expr.pattern], §8.7 [expr.constant], Clause 5 §5.5 [decl.type], Clause 7 §7.3 [type.composite], Clause 9 §9.2 [stmt.control], Clause 11 [generic]

---

### §8.4 Structured Expressions [expr.structured]

#### §8.4.1 Overview

[1] Structured expressions construct composite values (records, tuples, arrays, enums) and control evaluation (`if`, `match`, `loop`). They introduce nested scopes, bindings, and flow-sensitive typing. Annex A §A.4.10–§A.4.16 provides the grammar for these forms.

#### §8.4.2 Record, Tuple, and Enum Construction

**Record literals**

[2] Grammar: `RecordExpr ::= Path '{' FieldInit (',' FieldInit)* '}'` (Annex A §A.4.10).

[3] Typing:

- Let record `R` declare fields `{f₁: τ₁, …, fₙ: τₙ}`. The literal `R { f_i : e_i }` is well-formed iff every field appears exactly once and `Γ ⊢ e_i : τ_i`.
- Field order is irrelevant; shorthand `{ field }` expands to `{ field: field }`.

[4] Diagnostics: missing fields emit E08-400; duplicate fields emit E08-401; referencing a field not declared in the record emits E08-402.

**Tuple literals**

[5] Grammar: `TupleExpr ::= '(' Expr ',' Expr (',' Expr)* ')'` (Annex A §A.4.11).

[6] Typing: `Γ ⊢ (e₁, …, eₙ) : (τ₁, …, τₙ)` provided each `Γ ⊢ e_i : τ_i` and `n ≥ 2`. The one-element form `(e,)` is prohibited (E08-403) to avoid confusion with parenthesised expressions; use `e` directly or `tuple::from(e)`.

**Enum variants**

[7] Grammar: `EnumExpr ::= Path`, `Path '(' ExprList ')'`, or `Path '{' FieldInit* '}'` (Annex A §A.4.12).

[8] Typing: the selected variant must exist in the enum and each payload expression must match the declared payload type (positional or named). Mismatches emit E08-404.

#### §8.4.3 Array Literals and Repetition

[9] Grammar: `ArrayLiteral ::= '[' Expr (',' Expr)* ']' | '[' Expr ';' ConstExpr ']'` (Annex A §A.4.13).

[10] Typing:

- Element expressions must all have the same type `τ`. If the array literal is annotated (`let xs: [τ; n] = […]`), the literal is checked against that type. Otherwise the type is inferred from the elements.
- Repeat form `[value; n]` requires `τ : Copy` and `n` to be a comptime `usize`. Violations produce E08-430/E08-431.

#### §8.4.4 Conditionals (`if` / `if let`)

[11] Grammar: `IfExpr ::= 'if' Expr Block ('else' Clause)?` and `IfLetExpr ::= 'if' 'let' Pattern '=' Expr Block ('else' Block)?` (Annex A §A.4.14).

[12] Typing rules:

- `if`: condition must be `bool`. Both branches must have the same type `τ`. If the `else` clause is omitted, the expression has type `()` and no value may be expected downstream (E08-440).
- `if let`: the pattern is checked against the scrutinee type (Clause 8.5). Bindings introduced by the pattern exist only in the `then` block. Both branches must have a common type; omission of `else` is allowed only when the surrounding context accepts `()`.

[13] Guards (`if let pattern = expr if guard`) are evaluated after the pattern succeeds. Guards must have type `bool`.

#### §8.4.5 Match Expressions

[14] Grammar: `MatchExpr ::= 'match' Expr '{' MatchArm (',' MatchArm)* '}'` (Annex A §A.4.15).

[15] Mandatory typing annotation: any binding receiving the result of a `match` shall declare its type (`let result: Result<T, E> = match …`). Omission yields E08-450.

[16] Typing:

- Scrutinee type `τ_s` is inferred.
- Each arm pattern `p_i` is checked against `τ_s` (Clause 8.5) producing bindings `Γ_i`.
- Arm expressions are type-checked under `Γ ∪ Γ_i` and must share a common type `τ_r`.
- Guards must have type `bool`.

[17] Exhaustiveness: matches over enums, unions, modal states, and booleans shall cover every constructor. Non-exhaustive matches emit E08-451 listing missing cases. Reachability analysis warns about arms shadowed by earlier ones (E08-452).

[18] Evaluation: evaluate the scrutinee, then arms top-to-bottom. For each arm, attempt to match the pattern; if it succeeds, evaluate the guard if present; if the guard evaluates to `true`, execute the arm expression and return. If no arm matches, the match is ill-formed (should be caught by exhaustiveness checking).

#### §8.4.6 Loop Expressions

[19] Grammar: `LoopExpr ::= 'loop' Block | 'loop' Expr Block | 'loop' Pattern ':' Type 'in' Expr Block` (Annex A §A.4.16).

**Infinite loops**

[20] `loop { body }` defaults to type `!` (divergent). If all `break value` statements supply the same type `τ`, the loop has type `τ`. Mixed break types produce E08-460. Code paths without `break` remain divergent.

**Conditional loops**

[21] `loop condition { body }` requires `condition : bool`. Type is `()` unless break values specify otherwise.

**Iterator loops**

[22] Syntax requires explicit iterator annotation: `loop item: ItemType in collection { … }`. Missing `ItemType` yields E08-461. The collection must implement the iterator protocol defined in Clause 11; otherwise E08-462 is raised. Within the body, `item` has type `ItemType` and inherits the collection’s permissions.

**Break/continue**

[23] `break` exits the nearest loop, optionally yielding a value; its type must match the loop’s annotated type. `continue` restarts the current loop iteration. Both constructs are ill-formed outside loops (E08-463).

#### §8.4.7 Scope Interaction

[24] `if`, `match`, and `loop` expressions introduce nested lexical scopes. Bindings introduced inside these scopes shadow outer bindings only when `shadow` is used (§5.2). Drop order for temporaries follows Clause 12 (reverse creation order at scope exit).

#### §8.4.8 Diagnostics Summary

| Code | Condition |
| --- | --- |
| E08-400 | Record literal missing field |
| E08-401 | Record literal duplicates field |
| E08-402 | Record literal references unknown field |
| E08-403 | Single-element tuple literal disallowed |
| E08-404 | Enum variant payload mismatch |
| E08-430 | Array literal element type mismatch |
| E08-431 | Repeat form requires `Copy` element or comptime length |
| E08-440 | `if` expression lacks required `else` branch |
| E08-450 | Match result binding lacks explicit type |
| E08-451 | Match is non-exhaustive |
| E08-452 | Match arm unreachable |
| E08-460 | Loop break values disagree |
| E08-461 | Iterator loop missing element type annotation |
| E08-462 | Collection does not satisfy iterator protocol |
| E08-463 | `break`/`continue` used outside loop |

#### §8.4.9 Canonical Examples

**Example 8.4.9.1 (Record literal with shorthand).**

```cursive
let position = Point { x, y }          // expands to { x: x, y: y }
```

**Example 8.4.9.2 (Typed match).**

```cursive
let outcome: Result<Order, Error> = match command {
    Command::Buy(request) => process_buy(request),
    Command::Sell(request) => process_sell(request),
    Command::Cancel(id) => cancel(id),
}
```

**Example 8.4.9.3 (Iterator loop annotation).**

```cursive
loop entry: LogEntry in stream.records() {
    if entry.is_warning() {
        warn(entry.message)
    }
}
```

[25] The iterator loop explicitly states the item type `LogEntry`, making the type of `entry` clear both to readers and tooling.

---
