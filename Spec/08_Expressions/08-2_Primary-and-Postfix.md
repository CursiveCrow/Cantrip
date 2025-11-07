# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-2_Primary-and-Postfix.md  
**Section**: §8.2 Primary and Postfix Expressions  
**Stable label**: [expr.primary]  
**Forward references**: §8.3 [expr.operator], §8.4 [expr.structured], Clause 2 §2.3 [lex.tokens], Clause 4 §4.3 [module.scope], Clause 5 §5.2 [decl.variable], Clause 6 §6.4 [name.lookup], Clause 7 §7.5 [type.pointer], Clause 11 [generic], Clause 12 [memory]

---

### §8.2 Primary and Postfix Expressions [expr.primary]

#### §8.2.1 Overview

[1] Primary expressions are the atomic building blocks of Cursive’s expression grammar: literals, identifiers, blocks, and grouped forms. Postfix expressions extend primaries with calls, field access, indexing, slicing, pointer operations, and pipelines. This subclause formalises their syntax (Annex A §A.4.1–§A.4.8), typing rules, evaluation semantics, and diagnostics.

#### §8.2.2 Literals

[2] Grammar reference: Annex A §A.4.1 (`Literal`). Literal tokens are produced by the lexical grammar (§2.3).

[3] Typing rules:

- Integer literal `n` defaults to `i32`. If used in a context requiring integer type `τ`, the literal is accepted when `n ∈ ⟦τ⟧`; otherwise diagnostic E08-201 is emitted.
- Floating-point literals default to `f64`; suffix `f32` selects `f32`.
- Boolean literals `true`, `false` have type `bool`.
- Character literals have type `char` and shall denote exactly one Unicode scalar; invalid scalars trigger E08-202.
- String literals have type `string@View` and refer to statically allocated UTF-8 data.
- Array and tuple literals are covered in §8.4 but rely on literal element typing rules defined here.

[4] Evaluation: literal expressions evaluate to themselves without modifying the store. String literals may reference shared static storage; copying them is equivalent to copying the pointer-length pair (`string@View`).

#### §8.2.3 Identifier References

[5] Grammar reference: Annex A §A.4.1 (`PrimaryExpr ::= IDENT`). Identifiers resolve according to Clause 6. Binding categories are inherited: if the binding is a place, the expression is a place; otherwise it is a value.

[6] Typing rule:

$$
\frac{(x : \tau [cat]) \in \Gamma}{\Gamma \vdash x : \tau ! \emptyset [cat]}
\tag{T-Ident}
$$

[7] Diagnostics: referencing an identifier before definite assignment yields E08-210. Resolving a name that refers to a type, module, contract, or grant in expression position yields E08-211.

#### §8.2.4 Parenthesised Expressions

[8] Grammar: `ParenExpr ::= '(' Expr ')'` (Annex A §A.4.2).

[9] Typing/evaluation: parentheses do not change the type, value/place category, or grant set of the enclosed expression. They are used solely to override precedence or improve readability.

#### §8.2.5 Block Expressions

[10] Grammar: `BlockExpr ::= '{' Statement* ('result' Expr)? '}'` (Annex A §A.4.3).

[11] Typing:

- If a block ends with `result e`, the block’s type is `typeof(e)`; otherwise it is `()`.
- All statements inside the block shall be well-formed (Clause 9).
- Bindings introduced inside the block obey the scoping rules of Clause 6.

[12] The `result` keyword is mandatory when the block contributes a non-unit value. Missing `result` produces diagnostic E08-220.

[13] Evaluation: statements run sequentially; each may mutate the store. When execution reaches `result e`, the expression `e` is evaluated and the block terminates with that value. Without `result`, the block evaluates to `()` after executing all statements.

#### §8.2.6 Unit Expression

[14] `()` is both the literal and the type for the unit value (§7.2.6). It serves as the canonical result for expressions that intentionally produce no value.

#### §8.2.7 Function and Procedure Calls

[15] Grammar: `CallExpr ::= PostfixExpr '(' ArgumentList? ')'` (Annex A §A.4.4). The callee expression may itself be any postfix expression (including qualified procedure paths and closures).

[16] Typing rule:

$$
\frac{\Gamma \vdash f : (\tau_1,\ldots,\tau_n) \to \tau_r ! \varepsilon_f \quad \Gamma \vdash a_i : \tau_i ! \varepsilon_i}
{\Gamma \vdash f(a_1,\ldots,a_n) : \tau_r ! (\varepsilon_f \cup \varepsilon_1 \cup \cdots \cup \varepsilon_n)}
\tag{T-Call}
$$

[17] Arity mismatches emit E08-230. If `f` is polymorphic, Clause 11’s generic inference algorithm instantiates the type parameters before the call rule applies.

[18] Associated procedures (`receiver::method(args)`) desugar to calls on the procedure value defined in Clause 5. The receiver expression is evaluated first; its type must satisfy the required permission (`self: const/shared/unique`). Violations emit E08-231.

[19] Evaluation: evaluate the callee expression, then each argument left-to-right, then execute the callee with the resulting argument tuple. For effect operations (`Effect::op(...)`), the same rule applies but the callee’s grant set always contains the effect being invoked.

#### §8.2.8 Field and Tuple Access

[20] Grammar: `FieldExpr ::= PostfixExpr '.' IDENT` and `TupleProj ::= PostfixExpr '.' INTEGER_LITERAL` (Annex A §A.4.5).

[21] Typing:

- For records/struct enums, if `Γ ⊢ e : R` and record `R` declares field `f : τ` visible in the current module (§5.6), then `Γ ⊢ e.f : τ`. Invisible fields produce E08-240.
- For tuple projections, `Γ ⊢ e : (τ_1, …, τ_n)` and `0 ≤ i < n` imply `Γ ⊢ e.i : τ_{i+1}`. Out-of-range indices emit E08-241.

[22] Value/place propagation: if `e` is a place, `e.f` and `e.i` are places; otherwise they are values. Permission checks occur when the field or projection participates in assignment.

#### §8.2.9 Array Indexing and Slicing

[23] Grammar: `IndexExpr ::= PostfixExpr '[' Expr ']'` and `SliceExpr ::= PostfixExpr '[' RangeExpr? ']'` (Annex A §A.4.6).

[24] Typing rules:

- If `Γ ⊢ target : [τ; n]` (array) or `[τ]` (slice) and `Γ ⊢ index : usize`, then `Γ ⊢ target[index] : τ`. The result is a place when `target` is a place.
- Slicing `[start..end]` requires `start/end : usize` (or inferred defaults) and produces `[τ]`. Range semantics follow §7.3.4.

[25] Evaluation: evaluate the target, then the index/range bounds, all left-to-right. Index bounds are checked at runtime; violating them raises a panic with diagnostic E08-250. Slice bounds must satisfy `0 ≤ start ≤ end ≤ len`; otherwise E08-251 is issued.

#### §8.2.10 Pointer Address-of and Dereference

[26] Grammar: unary `&` and `*` are captured in Annex A §A.4.7 but belong with primaries because they manipulate places.

[27] Address-of (`&place`) is valid only when the operand denotes storage. Typing:

$$
\frac{\Gamma \vdash p : \tau [place]}{\Gamma \vdash \&p : \text{Ptr}\langle\tau\rangle@\text{Valid}}
\tag{T-Addr}
$$

Attempting to take the address of a value expression yields E08-260. Borrowed permissions follow Clause 12.

[28] Dereference typing is defined in §8.3 because it is a unary operator, but its evaluation semantics rely on the pointer states from §7.5.

#### §8.2.11 Pipelines

[29] Grammar: `PipelineExpr ::= PostfixExpr '=>' PostfixExpr ( '=>' PostfixExpr )*` (Annex A §A.4.8).

[30] Semantics: `lhs => stage` desugars to `stage(lhs)`. For stages with additional arguments, a closure form or partial application is required elsewhere.

[31] Typing: let `Γ ⊢ lhs : τ_0`. Each stage `stage_k` must have type `(τ_{k-1}) -> τ_k ! ε_k`. The entire pipeline has type `τ_m` (final stage). Missing annotations are allowed only when `τ_{k-1} ≡ τ_k` (per §8.1.3). Violations produce E08-020 (missing) or E08-021 (mismatched).

[32] Evaluation: evaluate `lhs`, then apply each stage sequentially, using the result of one as the argument to the next. Grants accumulate across stages.

#### §8.2.12 Diagnostics Summary

| Code | Condition |
| --- | --- |
| E08-201 | Integer literal cannot fit target type |
| E08-202 | Invalid character literal |
| E08-210 | Identifier used before definite assignment |
| E08-211 | Identifier does not denote a value |
| E08-220 | Block missing `result` despite non-unit use |
| E08-230 | Procedure call arity mismatch |
| E08-231 | Receiver lacks required permission for method call |
| E08-240 | Field access violates visibility |
| E08-241 | Tuple projection index out of bounds |
| E08-250 | Array/slice index out of range |
| E08-251 | Slice bounds invalid |
| E08-260 | Address-of applied to non-storage expression |
| E08-020 | Pipeline stage omitted type whilst changing type |
| E08-021 | Pipeline stage annotation disagrees with inferred type |

#### §8.2.13 Canonical Example

**Example 8.2.13.1 (Composed primaries and pipelines).**

```cursive
use io::write

procedure summarize(target: mut Record, raw: string@View): ()
    {| io::write, io::read |- raw.len() > 0 => target.last_report.is_some() |}
{
    let report: Result<string@Owned, Error> = raw
        => trim                            // type preserved, annotation omitted
        => parse: Result<Request, Error>   // type changes, annotation required
        => render: Result<string@Owned, Error>

    match report {
        Result::Ok(text) => {
            target.last_report = Option::Some(text.clone())
            io::write(text)
        }
        Result::Err(err) => io::write(err.message()),
    }
}
```

[33] This example demonstrates literals, block expressions with explicit `result`, method calls with receiver permissions, field access (`target.last_report`), and the pipeline annotation rule. The enclosing procedure’s `uses` clause lists both `io::write` (intrinsic) and `io::read` (introduced by `parse`).

---
