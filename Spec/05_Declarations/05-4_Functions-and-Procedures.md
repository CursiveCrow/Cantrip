# Cursive Language Specification

## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-4_Functions-and-Procedures.md
**Section**: §5.4 Procedures
**Stable label**: [decl.function]  
**Forward references**: Clause 8 [expr], Clause 9 [stmt], Clause 11 [generic], Clause 12 [memory], Clause 13 [contract], Clause 16 [interop]

---

### §5.4 Procedures [decl.function]

#### §5.4.1 Overview

[1] Procedure declarations introduce named computations at module scope or as associated members of types.

[2] Cursive provides a single callable form: _procedures_. Purity is determined by the contractual sequent’s grant set. Procedures whose sequents declare an empty grant set (`{| |- … |}`) are pure and may be evaluated anywhere a side-effect-free computation is required. Procedures whose sequents declare non-empty grant sets may perform the capabilities named in that set.

[3] Procedures may declare a receiver parameter `self` typed as `Self` with permission qualifiers `const`, `shared`, or `unique` (§12.4). When omitted, the procedure behaves as a static callable that still participates in grant checking.

[4] This subclause specifies the declaration surface for procedures; Clause 9 details callable bodies and evaluation, while Clause 13 defines contractual sequent semantics and the grant system.

#### §5.4.2 Syntax

```ebnf
procedure_declaration
    ::= callable_attributes? visibility_modifier? procedure_head procedure_signature callable_body

procedure_head
    ::= procedure_kind identifier

procedure_kind
    ::= "procedure"
     | "comptime" "procedure"
     | "extern" string_literal? "procedure"

procedure_signature
    ::= generic_params? "(" parameter_list? ")" return_clause? contract_spec

parameter_list
    ::= parameter ("," parameter)*

parameter
    ::= receiver_shorthand
     | parameter_name ":" type_expression

receiver_shorthand
    ::= "~"            // const Self
     | "~%"           // shared Self
     | "~!"           // unique Self

contract_spec
    ::= "{|" contract_expression? "|}"

contract_expression
    ::= /* see Clause 13 for full grammar; omitting any position defaults it to `true` */

return_clause
    ::= ":" type_expression

callable_body
    ::= block                   // all procedures
     | "=" expression ";"     // permitted only when the contractual sequent declares an empty grant set
     | ";"                     // extern declarations
```

[1] `callable_attributes` encompasses optimisation, diagnostic, and linkage attributes defined in Clauses 9 and 16.

[2] `contract_spec` (Clause 13) shall appear on every procedure declaration. The ASCII form `{| grants |- must => will |}` is used throughout this clause. Omitting `grants`, `must`, or `will` defaults the missing predicate to `true`. Pure procedures write `{| |- … |}` explicitly to make the empty grant set visible.

[3] Expression-bodied forms (`= expression ;`) are syntactic sugar for pure procedures. The compiler shall verify that the associated contractual sequent declares an empty grant set when this form is used.

#### §5.4.3 Constraints

[1] _Visibility defaults._ Module-scope procedures default to `internal` visibility. Associated procedures inherit the containing type’s visibility unless overridden.

[2] _Receiver parameter._ Procedures that include a receiver must declare it as the first parameter, either explicitly (`self: const/shared/unique Self`) or by using the shorthand `~`/`~%`/`~!`, which desugars respectively to `self: const/shared/unique Self`. A receiver parameter shall not appear in any position other than first; violations produce diagnostic E05-401.

[3] _Contractual sequents required._ Every procedure declaration shall include a contractual sequent specification. Omitting the contractual sequent is ill-formed (diagnostic E05-405). While the contractual sequent may appear on the same line as the signature, placing it on the following line is the recommended style.

[4] _Expression bodies and grants._ Expression-bodied procedures shall declare an empty grant set. Attempting to pair an expression body with a non-empty grant set is ill-formed (diagnostic E05-407).

[5] _Pure procedures._ Procedures whose contractual sequents declare an empty grant set may not call procedures whose grant sets are non-empty nor perform operations that require grants. Violations emit diagnostic E05-406.

[6] _Comptime procedures._ Bodies of `comptime procedure` declarations shall be compile-time evaluable. Any grants referenced must be valid in compile-time contexts. Violations emit diagnostic E05-402.

[7] _Extern procedures._ `extern` procedures declare their signature and contractual sequent but omit a body (`;`). Their linkage is governed by Clause 16.

[8] _Recursion._ Procedures may reference themselves directly or indirectly. Implementations may warn when they detect unprovable termination but shall not reject recursive procedures solely for that reason.

#### §5.4.4 Semantics

[1] Procedure definitions introduce callable entities that, when referenced, produce values of FunctionType (§7.4). The type records parameter types, return type, grant set, and contractual sequent.

[2] The contractual sequent governs three responsibilities:
- **uses**: grants that must be available when the procedure executes
- **must**: predicates that must hold when the procedure begins execution
- **will**: predicates that will hold when the procedure completes normally

[3] Receiver shorthands desugar before semantic analysis so that tooling observes an explicit `self` parameter bound to `Self` with the requested permission qualifier.

[4] Procedures shall provide a contractual sequent contiguous with the signature; implementations shall accept either the same line or the immediately following line.

[5] `comptime` procedures execute during compile-time evaluation. Failure to evaluate successfully renders any compile-time use ill-formed.

#### §5.4.5 Examples (Informative)

**Example 5.4.5.1 (Pure procedure with expression body):**

```cursive
procedure clamp(value: i32, min: i32, max: i32): i32
    {| |- value >= min && value <= max => result >= min && result <= max |}
= value.max(min).min(max)
```

**Example 5.4.5.2 (Procedure with receiver and grants):**

```cursive
procedure deposit(~!, amount: i64)
    {| ledger::post |- amount > 0 => self.balance >= amount |}
{
    self.balance += amount
}
```

**Example 5.4.5.3 (Static procedure with grants):**

```cursive
procedure reset_all(): ()
    {| admin::reset |- true => true |}
{
    result ()
}
```

**Example 5.4.5.4 - invalid (Expression body with grants):**

```cursive
procedure unsafe_add(lhs: i32, rhs: i32): i32
    {| io::write |- true => true |}
= lhs + rhs  // error[E05-407]
```

**Example 5.4.5.5 - invalid (Missing contractual sequent):**

```cursive
procedure balance(~): i64 {
    result self.balance  // error[E05-405]
}
```

#### §5.4.6 Conformance Requirements [decl.function.requirements]

[1] Implementations shall diagnose malformed or missing receiver parameters on procedures (E05-401) and reject procedure declarations lacking a contractual sequent specification (E05-405).

[2] Compilers shall ensure that procedures with empty grant sets do not perform grant-requiring operations (E05-406) and that expression-bodied forms are accepted only for procedures whose grant sets are empty (E05-407).

[3] Implementations shall enforce compile-time restrictions on `comptime` procedures (E05-402) and require explicit return types unless §5.4.3[6] applies (E05-403).

[4] Tooling shall record contractual sequents, grant sets, receiver permissions, and callable kinds in reflection metadata so that downstream analyses can reason about capability requirements.
