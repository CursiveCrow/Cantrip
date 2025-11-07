# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-1_Expression-Fundamentals.md  
**Section**: §8.1 Expression Fundamentals  
**Stable label**: [expr.fundamental]  
**Forward references**: §8.2 [expr.primary], §8.3 [expr.operator], §8.4 [expr.structured], §8.5 [expr.pattern], §8.6 [expr.conversion], §8.7 [expr.constant], §8.8 [expr.typing], Clause 2 §2.3 [lex.tokens], Clause 5 §5.2 [decl.variable], Clause 6 §6.4 [name.lookup], Clause 7 §7.5 [type.pointer], Clause 7 §7.7 [type.relation], Clause 9 §9.4 [stmt.order], Clause 11 [generic], Clause 12 [memory], Clause 13 [contract]

---

### §8.1 Expression Fundamentals [expr.fundamental]

#### §8.1.1 Overview

[1] An *expression* is a syntactic form that produces a value, denotes a storage location, or diverges. Expressions link declarations (§5), names (§6), types (§7), statements (§9), and contracts (§13). This subclause establishes the global obligations that apply to every expression form before the remainder of Clause 8 defines each category.

[2] Expressions are analysed inside a *context* containing (a) the typing environment `Γ`, (b) the active grant set supplied by the enclosing `uses` clause, (c) permission and region metadata (Clause 12), and (d) a deterministic evaluation order (§8.1.3). An expression is well-formed only if all contextual obligations are satisfied; otherwise the diagnostics in §8.1.7 apply.

#### §8.1.2 Grammar and Expression Families

[3] Annex A §A.4 is the authoritative grammar for expressions. Table 8.1 maps the grammar categories to the specification subclauses that define their semantics.

| Grammar production (Annex A) | Description | Subclause |
| --- | --- | --- |
| `PrimaryExpr`, `PostfixExpr` | Literals, identifiers, blocks, calls, indexing, pipeline | §8.2 |
| `UnaryExpr`, `BinaryExpr`, `AssignExpr` | Prefix, infix, and assignment operators | §8.3 |
| `StructuredExpr`, `IfExpr`, `MatchExpr`, `LoopExpr` | Composite constructors and control expressions | §8.4 |
| `Pattern`, `MatchArm` | Expression patterns, guards, exhaustiveness | §8.5 |
| `CastExpr` | Explicit conversions (`as`) | §8.6 |
| `ComptimeExpr` | Constant/comptime blocks | §8.7 |
| `Expr` (typing summary) | Global typing and diagnostics | §8.8 |

[4] Each subclause follows the ISO template mandated by SpecificationOnboarding.md: overview, syntax, constraints, semantics, examples, diagnostics, and forward references.

#### §8.1.3 Deterministic Evaluation Model

[5] **Strict left-to-right evaluation.** Subexpressions evaluate in the lexical order in which they appear. For any compound expression `e = f(e_1, …, e_n)` the semantics require evaluating `e_1` before `e_2`, etc. The formal rule is:

$$
\frac{
    \langle e_1, \sigma \rangle \Downarrow \langle v_1, \sigma_1 \rangle \\
    \cdots \\
    \langle e_n[v_1,\ldots,v_{n-1}], \sigma_{n-1} \rangle \Downarrow \langle v_n, \sigma_n \rangle
}{
    \langle f(e_1,\ldots,e_n), \sigma \rangle \Downarrow \langle f(v_1,\ldots,v_n), \sigma_n \rangle
}
\tag{E-LeftToRight}
$$

[6] **Short-circuit exception.** Logical `&&` and `||` (§8.3.6) may skip evaluating their right operand when the left operand determines the result. No other construct may elide evaluation.

[7] **Call-by-value.** Procedures, effect operations, and pipeline stages evaluate their arguments completely before invoking the callee. Divergent subexpressions propagate divergence to the enclosing expression.

[8] **Determinism.** Given a fixed program, inputs, and grant budget, expression evaluation yields the same observable behaviour on every conforming implementation. There is no unspecified evaluation order or hidden parallelism.

[9] **Comptime vs runtime contexts.** Expressions appearing in const/comptime positions (§8.7) must be evaluable during the compile-time execution phase (§2.2). Using runtime-only constructs in those contexts yields diagnostic E08-001 unless explicitly whitelisted.

[10] **Pipeline annotation rule.** A stage in `lhs => stage : Type` may omit `: Type` only when the stage’s output type equals its input type modulo type equivalence (§7.7). Stages that change type shall declare the new type explicitly; otherwise diagnostic E08-020 is issued. The compiler records the before/after types and references this rule in the diagnostic payload.

#### §8.1.4 Categories: Value, Place, Divergent

[11] Every expression is classified as:

- **Value** — produces a temporary result (literals, arithmetic expressions, function calls). Values can be moved or copied depending on their type’s `Copy` predicate (§7.2, §7.3).
- **Place** — denotes a storage location that can be read or written (variables, fields, tuple components, indexed elements, dereferenced pointers). Places participate in assignments (§8.3.8) when they carry `mut` or `own` permission (§12.4).
- **Divergent** — expressions of type `!` that never return (`loop {}` without `break`, `panic`). Divergent expressions coerce to any type via the rule in §7.2.7.

[12] The typing judgment records the category: `Γ ⊢ e : τ ! ε [cat]`. Tooling and diagnostics derive user-facing messages from `cat`. The language core does not expose `cat` as syntax; future editions may explore comptime predicates (`is_place(expr)`, `is_value(expr)`) if metaprogramming demand warrants it. Until then, categories remain implicit in the typing rules to preserve surface simplicity.

#### §8.1.5 Grant Accumulation and Enforcement

[13] Cursive uses *grants* (Clause 13) to track observable capabilities (IO, allocation, unsafe operations). Each expression carries a grant set `ε`. The total grant requirement of a compound expression is the union of its subexpression grants plus any intrinsic grants introduced by that expression form.

[14] Grant checking rule:

$$
\frac{
    \Gamma \vdash e_i : \tau_i ! \varepsilon_i
}{
    \Gamma \vdash f(e_1,\ldots,e_n) : \tau ! \left(\bigcup_i \varepsilon_i \cup \varepsilon_f\right)
}
\tag{Grant-Union}
$$

where `ε_f` captures intrinsic effects (e.g., `unsafe.ptr` for raw dereference).

[15] The enclosing declaration must list at least this union in its `uses` clause; otherwise diagnostic E08-004 enumerates the missing grants. No grant bundling mechanism exists in this edition; all obligations remain explicit so that humans and LLMs can reason locally. (Informative note: if real programs consistently accumulate large unions, a future edition may introduce named bundles, provided they remain explicit declarations.)

#### §8.1.6 Typing Discipline

[16] Expression typing is bidirectional:

- **Inference sites** synthesise a type without relying on context (literals, identifiers, many operator operands).
- **Checking sites** require a contextual type (result expressions, match arms). If inference fails and no context exists, E08-002 suggests adding a type annotation.
- **Bidirectional sites** (function calls, pipeline stages) attempt to meet the contextual expectation first; if that fails they synthesise a type and ask the context to accept it.

[17] Type compatibility uses the equivalence and subtyping relations in §7.7. When operands fail compatibility, diagnostic E08-800 reports both types, the relevant rule (variance, equality), and suggested fixes.

[18] Definite assignment (§5.7) and permission rules (§12.4) are enforced simultaneously: the typing judgment tracks whether bindings have been initialised and what permissions apply to each place. Violations surface through E08-102 (use before initialisation) and E08-003 (insufficient permission).

#### §8.1.7 Diagnostics and Payload Requirements

[19] Table 8.1 lists the diagnostics introduced in this subclause. Subsequent subclauses extend the table with operator- and construct-specific codes.

| Code | Condition | Required payload |
| --- | --- | --- |
| **E08-001** | Runtime-only construct used in comptime context | Expression range, enclosing comptime site, offending construct |
| **E08-002** | Type cannot be inferred and no contextual type supplied | Expression range, partial type information, suggested annotation |
| **E08-003** | Place used without sufficient permission | Binding name, required permission, observed operation |
| **E08-004** | Missing grant(s) from enclosing `uses` clause | Expression range, required grant set, provided grant set |
| **E08-020** | Pipeline stage omitted type annotation while changing type | Stage text, inferred before/after types |

[20] Diagnostics follow Annex E §E.5: they include the error code, a short description, structured fields (JSON), and optional fix-it hints.

#### §8.1.8 Examples

**Example 8.1.8.1 (Block result explicitness).**

```cursive
let report: string@View = {
    audit.log("enter")
    result format_report(state)
}
```

Omitting `result` would produce E08-110 because the block’s type would default to `()`.

**Example 8.1.8.2 (Grant accumulation).**

```cursive
procedure write_normalized(input: string@View): ()
    {| io::write, io::read |- input.len() > 0 => true |}
{
    let normalized = input
        => trim
        => lookup_dictionary: Result<string@Owned, io::Error>
    match normalized {
        Result::Ok(text) => io::write(text),
        Result::Err(err) => panic(err.message()),
    }
}
```

The pipeline stage `lookup_dictionary` introduces grant `io::read`; the enclosing procedure therefore lists both `io::write` (intrinsic to `io::write`) and `io::read` (introduced by the stage).

---
