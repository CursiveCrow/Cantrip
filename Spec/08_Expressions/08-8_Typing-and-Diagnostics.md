# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-8_Typing-and-Diagnostics.md  
**Section**: §8.8 Expression Typing Rules and Ill-formed Cases  
**Stable label**: [expr.typing]  
**Forward references**: Clause 5 §5.7 [decl.initialization], Clause 6 §6.4 [name.lookup], Clause 7 §7.7 [type.relation], Clause 12 [memory], Clause 13 [contract], Annex E §E.5 [implementation.diagnostics]

---

### §8.8 Expression Typing Rules and Ill-formed Cases [expr.typing]

#### §8.8.1 Overview

[1] This subclause consolidates the typing judgment introduced in §8.1, explains how value/place/divergent categories interact with assignments and moves, and catalogues common ill-formed constructs along with their diagnostics and required payloads.

#### §8.8.2 Typing Judgment Recap

[2] Every expression typing derivation has the form `Γ ⊢ e : τ ! ε [cat]`, where:

- `Γ` contains lexical bindings, type bindings, region stack, and grant context.
- `τ` is the expression’s type after alias expansion (§7.7).
- `ε` is the cumulative grant set (§8.1.5).
- `[cat] ∈ {value, place, divergent}` (§8.1.4).

[3] The following invariants must hold:

- **Definite assignment:** no identifier is read before it is initialised (Clause 5 §5.7). Violations emit E08-210.
- **Permission safety:** mutations and moves operate only on places with appropriate permissions (`mut` or `own`). Violations emit E08-003.
- **Grant coverage:** the enclosing declaration’s `uses` clause contains ε; otherwise E08-004 lists missing grants.
- **Type compatibility:** operands satisfy the subtyping/compatibility rules in Clause 7. Failures emit E08-800.

#### §8.8.3 Ill-formed Expression Catalog

[4] Table 8.8.1 summarises representative ill-formed cases. Annex E §E.5 specifies the structured payload required for each diagnostic.

| ID | Description | Diagnostic |
| --- | --- | --- |
| IF-01 | Identifier refers to a type/module instead of a value | E08-211 |
| IF-02 | Expression type cannot be inferred and no context exists | E08-002 |
| IF-03 | Place used without sufficient permission | E08-003 |
| IF-04 | Runtime-only construct in const/comptime context | E08-700 |
| IF-05 | Match expression missing mandatory type annotation | E08-450 |
| IF-06 | Pipeline stage omits type while changing type | E08-020 |
| IF-07 | Array/slice index out of range | E08-250 |
| IF-08 | Assignment target is not a place | E08-330 |
| IF-09 | Non-exhaustive match | E08-451 |
| IF-10 | Illegal cast category | E08-601 |

#### §8.8.4 Tooling Integration

[5] Implementations shall expose expression metadata (type, grant set, category, evaluation order) via the tooling interface in Annex E §E.3 so that IDEs and linters can present accurate information to developers. Diagnostic payloads shall include this metadata whenever it aids remediation.

#### §8.8.5 Example Diagnostic Payload

```json
{
  "code": "E08-003",
  "message": "assignment requires mutable place",
  "expr_range": "src/lib.rs:42:9-42:22",
  "binding": "total",
  "required_permission": "mut",
  "observed_binding_permission": "let",
  "suggested_fixes": [
    {
      "description": "declare binding as 'var total'",
      "edits": [ ... ]
    }
  ]
}
```

[6] This payload format follows Annex E: the code, message, source span, structured payload, and optional fix-its make diagnostics machine-readable and human-friendly.

---
