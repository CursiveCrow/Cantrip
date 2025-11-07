# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-5_Patterns-and-Exhaustiveness.md  
**Section**: §8.5 Patterns and Exhaustiveness  
**Stable label**: [expr.pattern]  
**Forward references**: §8.6 [expr.conversion], Clause 5 §5.3 [decl.pattern], Clause 7 §7.3 [type.composite], Clause 7 §7.6 [type.modal]

---

### §8.5 Patterns and Exhaustiveness [expr.pattern]

#### §8.5.1 Overview

[1] Patterns provide structural matching in expression contexts (`if let`, `match` arms, destructuring statements). Clause 5 §5.3 defines the syntax and base rules; this clause adapts them to expression semantics, adds guard behaviour, and specifies the exhaustiveness analyses required for match expressions.

#### §8.5.2 Pattern Typing Judgment

[2] The judgment `Γ ⊢ p : τ ⇝ Δ` states that pattern `p` matches scrutinee type `τ` and extends the environment with bindings `Δ`. Patterns are checked using structural recursion:

- **Literal patterns** (`42`, `'x'`): `τ` must equal the literal type.
- **Identifier pattern** (`name`): matches any value of type `τ`, binding `name : τ`.
- **Tuple patterns** (`(p₁, …, pₙ)`): require `τ = (τ₁, …, τₙ)` and check each component recursively.
- **Record patterns** (`{ field: p }`): require `τ` to be a record containing the listed fields and check each field’s pattern against its declared type.
- **Enum patterns** (`Enum::Variant(p)`): require `τ` to be the enum and destructure the variant payload.
- **Or-patterns** (`p₁ | p₂`): both branches must bind the same identifiers with the same types; otherwise diagnostics E08-510/E08-511 are emitted.
- **Wildcard (`_`)**: matches any value without binding.

[3] Bindings inherit mutability and permissions from the pattern prefix (`mut`, `own`, etc.). Attempting to rebind a name already in scope without `shadow` remains ill-formed.

#### §8.5.3 Guards

[4] Guards (`pattern if condition`) are evaluated only after the pattern matches and the bindings described in `Δ` exist. Guard expressions must have type `bool`. Guards cannot bind new names. If a guard evaluates to `false`, the match proceeds to the next arm as if the pattern had failed.

#### §8.5.4 Exhaustiveness Checking

[5] Match expressions over enums, unions, modal states, and booleans shall be exhaustive. The compiler applies the coverage algorithm from Annex E §E.2.5. If coverage is incomplete, diagnostic E08-451 lists missing constructors (variant name, arity, module path). Developers may use `_` as a catch-all when appropriate.

[6] Reachability: if an arm is statically shadowed by earlier arms (e.g., wildcard before specific patterns), diagnostic E08-512 is emitted so the dead arm can be removed.

#### §8.5.5 Destructuring Bindings in Expressions

[7] Expression contexts may include destructuring `let` statements:

```cursive
let (x, y) = expr
```

[8] Typing: the pattern is checked with the same judgment `Γ ⊢ p : τ ⇝ Δ`. The statement introduces bindings `Δ` for the remainder of the current block. Using destructuring in positions other than statements or at the start of block expressions emits E08-520.

#### §8.5.6 Diagnostics Summary

| Code | Condition |
| --- | --- |
| E08-510 | Or-pattern branches bind different identifiers |
| E08-511 | Or-pattern branches bind identifiers with incompatible types |
| E08-512 | Match arm is unreachable due to prior coverage |
| E08-520 | Destructuring binding used outside permitted statement contexts |

#### §8.5.7 Example

```cursive
match message {
    Packet::Data { id, payload } if payload.len() > 0 => handle_data(id, payload),
    Packet::Ack(id) | Packet::Nack(id) => handle_ack(id),
    Packet::Heartbeat => (),
}
```

[9] The or-pattern ensures both `Ack` and `Nack` arms bind `id` with type `u64`. Guards run after pattern binding (`payload` is in scope when evaluating the guard).

---
