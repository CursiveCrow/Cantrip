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

[1] Patterns provide structural matching inside expressions, complementing the declaration patterns defined in Clause 5 §5.3. They appear in `if let`, match arms, and destructuring statements embedded within expressions. This subclause adapts the pattern typing judgment for expression contexts, details guard semantics, and specifies exhaustiveness and reachability analyses.

#### §8.5.2 Pattern Typing Judgment

[2] The judgment `Γ ⊢ p : τ ⇝ Δ` means “pattern `p` matches scrutinee type `τ` and extends the environment with bindings `Δ`.” The rules mirror Clause 5 but are restated here for clarity:

- **Wildcard (`_`)**: matches any value, introduces no bindings.
- **Identifier (`name`)**: matches any value, introduces binding `name : τ` with the surrounding mutability/permission qualifiers.
- **Literal (`42`, `'x'`, `true`)**: matches only if the scrutinee equals the literal.
- **Tuple pattern `(p₁,…,pₙ)`**: requires `τ = (τ₁,…,τₙ)`; each component is checked recursively.
- **Record pattern `{ field: p }`**: requires `τ` to be a record containing the listed fields; each field pattern is checked recursively. Omitting a required field is ill-formed (E08-500).
- **Enum pattern `Enum::Variant(p)`**: requires `τ` to be the enum; payload patterns are checked recursively.
- **Union component pattern `name: Type`**: matches values of union types when the discriminant indicates the value is of the specified component type (see §8.5.2.5 for complete specification).
- **Or-pattern (`p₁ | p₂ | …`)**: every branch shall introduce the same identifiers with identical types. Violations emit E08-510 (missing identifier) or E08-511 (type mismatch).
- **Refutable patterns** (enum variant patterns such as `Status::Running`, union component patterns) may only appear where failure is handled (`if let`, match arms).

[3] Pattern bindings inherit mutability/permissions from the pattern prefix (`mut`, `own`, `ref`, etc.). Attempting to bind a name that already exists in scope without `shadow` is ill-formed (Clause 5 diagnostics).

#### §8.5.2.5 Union Type Patterns [expr.pattern.union]

[3.1] Union types (`τ₁ \/ τ₂ \/ ... \/ τₙ`) use type annotation patterns for matching. The syntax `identifier: Type` matches when the union's runtime discriminant indicates the value is of the specified component type.

**Grammar:**

**Type annotation patterns** (for union types) match the pattern:
```
<identifier> ":" <type>
```

[ Note: See Annex A §A.3 [grammar.pattern] for the complete pattern grammar.
— end note ]

**Typing rule:**

[ Given: Union type $\tau_1 \/ \cdots \/ \tau_n$, pattern with type annotation ]

$$
\frac{\Gamma \vdash e : \tau_1 \/ \cdots \/ \tau_n \quad \tau_i \in \{\tau_1, \ldots, \tau_n\}}{\Gamma \vdash (x: \tau_i \Rightarrow \text{body}) \text{ matches component } \tau_i \quad \Gamma, x: \tau_i \vdash \text{body}}
\tag{T-Union-Pattern}
$$

**Exhaustiveness:** Union patterns must cover all component types or include a wildcard catch-all. Missing component types produce diagnostic E08-704 (incomplete union match).

**Example 8.5.2.5 (Union type patterns):**

```cursive
procedure handle_result(value: i32 \/ ParseError)
    [[ |- true => true ]]
{
    match value {
        num: i32 => println("Success: {}", num),
        err: ParseError => println("Error: {}", err.message()),
    }
}
```

[3.2] Type annotation patterns introduce a binding with the matched component type. The binding is available only within the arm body and has the refined type (not the union type).

#### §8.5.3 Guards

[4] A guard (`pattern if expr => body`) is evaluated only after the pattern matches and the bindings described by `Δ` are in scope. Guard expressions shall have type `bool`. Guards may not introduce new bindings or move values out of the pattern unless the pattern itself owns them. If the guard evaluates to `false`, control proceeds to the next arm as if the pattern had not matched.

#### §8.5.4 Exhaustiveness and Reachability

[5] Match expressions over enums, unions, modal states, and `bool` shall be exhaustive. The compiler uses the coverage algorithm defined in Annex E §E.2.5 to ensure every constructor is handled. Missing cases produce diagnostic E08-451 with a machine-readable list of unhandled constructors (variant name, module path, payload arity).

[6] Reachability analysis ensures later arms are not shadowed by earlier arms. If an arm can never match because a prior arm covers the same space, the compiler emits E08-512 so the dead arm can be removed or reordered.

#### §8.5.5 Destructuring Statements inside Expressions

[7] Expression contexts may include destructuring statements (`let pattern = expr;`). They are permitted only as standalone statements or at the top level of block expressions; embedding them mid-expression introduces ambiguity and is forbidden (E08-520).

[8] Typing follows the same judgment `Γ ⊢ p : τ ⇝ Δ`. Bindings in `Δ` enter scope after the statement and remain valid until the surrounding block ends, subject to move semantics.

#### §8.5.6 Diagnostics Summary

| Code    | Condition                                                    |
| ------- | ------------------------------------------------------------ |
| E08-500 | Record pattern omits required field                          |
| E08-510 | Or-pattern branches bind different identifiers               |
| E08-511 | Or-pattern branches bind identifiers with incompatible types |
| E08-512 | Match arm is unreachable                                     |
| E08-520 | Destructuring statement used in disallowed context           |
| E08-704 | Union pattern match missing component type(s)                |

#### §8.5.7 Canonical Example

```cursive
match packet {
    Packet::Data { id, payload } if payload.len() > 0 => process(id, payload),
    Packet::Ack(id) | Packet::Nack(id) => update_status(id),
    Packet::Heartbeat => (),
}
```

[9] The or-pattern ensures both `Ack` and `Nack` arms introduce the same `id` binding with type `u64`. The guard accesses `payload` only after the pattern succeeds, illustrating the evaluation order.

---
