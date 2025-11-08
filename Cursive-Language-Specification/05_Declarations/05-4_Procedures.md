# Cursive Language Specification

## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-4_Functions-and-Procedures.md
**Section**: §5.4 Procedures
**Stable label**: [decl.function]  
**Forward references**: Clause 8 [expr], Clause 9 [stmt], Clause 10 [generic], Clause 11 [memory], Clause 12 [contract], Clause 15 [interop]

---

### §5.4 Procedures [decl.function]

#### §5.4.1 Overview

[1] Procedure declarations introduce named computations at module scope or as associated members of types.

[2] Cursive provides a single callable form: _procedures_. Purity is determined by the contractual sequent's grant set. Procedures whose sequents declare an empty grant set (e.g., omitted sequents defaulting to `[[ ∅ |- true => true ]]`, or explicit `[[ P => Q ]]` without grants) are pure and may be evaluated anywhere without requiring additional capabilities. Procedures whose sequents declare non-empty grant sets may perform the capabilities named in that set.

[3] Procedures may declare a receiver parameter `self` typed as `Self` with permission qualifiers `const`, `shared`, or `unique` (§11.4). When omitted, the procedure behaves as a static callable that still participates in grant checking.

[4] This subclause specifies the declaration surface for procedures; Clause 9 details callable bodies and evaluation, while Clause 12 defines contractual sequent semantics and the grant system.

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
    ::= generic_params? "(" parameter_list? ")" return_clause? sequent_clause?

parameter_list
    ::= parameter ("," parameter)*

parameter
    ::= receiver_shorthand
     | parameter_modifier? parameter_name ":" type_expression

parameter_modifier
    ::= "move"

receiver_shorthand
    ::= "~"            // const Self
     | "~%"           // shared Self
     | "~!"           // unique Self

sequent_clause
    ::= "[[" sequent_expr "]]"

sequent_expr
    ::= /* see Annex A §A.7 for complete sequent grammar */

return_clause
    ::= ":" type_expression

callable_body
    ::= block                   // all procedures
     | "=" expression ";"     // pure procedures only (implicit [[ ∅ |- true => true ]])
     | ";"                     // extern declarations
```

[1] `callable_attributes` encompasses optimisation, diagnostic, and linkage attributes defined in Clauses 9 and 16.

[2] Contractual sequents use semantic brackets `⟦ ⟧` (Unicode U+27E6, U+27E7) or ASCII `[[ ]]` with the form `[[ grants |- must => will ]]` (or `[[ must => will ]]` when no grants). The sequent clause is **optional**: when omitted, it defaults to `[[ ∅ |- true => true ]]` (empty grant set, canonical: `[[ |- true => true ]]`, pure procedure with no grants, preconditions, or postconditions). Procedures shall include explicit sequents only when they require grants, have preconditions, or guarantee postconditions.

(2.1) Smart defaulting rules apply within sequent brackets:

- Grant-only: `[[ io::write ]]` expands to `[[ io::write |- true => true ]]`
- Precondition-only: `[[ x > 0 ]]` expands to `[[ x > 0 => true ]]` (canonical: `[[ |- x > 0 => true ]]`)
- Postcondition-only: `[[ => result > 0 ]]` expands to `[[ true => result > 0 ]]` (canonical: `[[ |- true => result > 0 ]]`)
- No turnstile when no grants: `[[ P => Q ]]` (preferred form, canonical: `[[ |- P => Q ]]`)

[3] Expression-bodied forms (`= expression ;`) are syntactic sugar for pure procedures and shall not include a contractual sequent (the default `[[ ∅ |- true => true ]]` is implicit, canonical: `[[ |- true => true ]]` with empty grant set). Including an explicit sequent with expression bodies is ill-formed (diagnostic E05-408).

#### §5.4.3 Constraints

[1] _Visibility defaults._ Module-scope procedures default to `internal` visibility. Associated procedures inherit the containing type’s visibility unless overridden.

[2] _Receiver parameter._ Procedures that include a receiver must declare it as the first parameter, either explicitly (`self: const/shared/unique Self`) or by using the shorthand `~`/`~%`/`~!`, which desugars respectively to `self: const/shared/unique Self`. A receiver parameter shall not appear in any position other than first; violations produce diagnostic E05-401.

(2.1) _Parameter responsibility modifier._ Parameters may optionally include the `move` modifier to indicate that the procedure assumes cleanup responsibility for the parameter value:

```cursive
procedure process(data: Buffer)             // Non-responsible (default)
procedure consume(move data: Buffer)        // Responsible (takes ownership)
```

(2.2) Without `move`, parameters are non-responsible: they behave like non-responsible bindings (`let param <- argument`) and do not invoke destructors when the procedure returns. With `move`, parameters are responsible: they behave like responsible bindings (`let param = argument`) and invoke destructors when the procedure returns.

(2.3) Call sites must use `move` when calling procedures with `move` parameters: `consume(move x)`. Omitting `move` at the call site when required produces diagnostic E05-409. Using `move` at the call site when the parameter lacks the `move` modifier produces diagnostic E05-410.

[3] _Contractual sequents optional._ Contractual sequent specifications are optional. When omitted, the procedure defaults to `[[ ∅ |- true => true ]]` (empty grant set, canonical: `[[ |- true => true ]]`, pure procedure). Procedures shall include explicit sequents when they require grants, enforce preconditions, or guarantee postconditions. Sequents may appear on the same line as the signature (for simple cases) or on the following line (recommended style for complex contracts).

[4] _Expression bodies._ Expression-bodied procedures (`= expression ;`) shall not include explicit contractual sequents; the default `[[ ∅ |- true => true ]]` is implicit (canonical: `[[ |- true => true ]]` with empty grant set). Including a sequent with expression bodies is ill-formed (diagnostic E05-408).

[5] _Pure procedures._ Procedures whose contractual sequents declare an empty grant set may not call procedures whose grant sets are non-empty nor perform operations that require grants. Violations emit diagnostic E05-406.

[6] _Comptime procedures._ Bodies of `comptime procedure` declarations shall be compile-time evaluable. Any grants referenced must be valid in compile-time contexts. Violations emit diagnostic E05-402.

[7] _Extern procedures._ `extern` procedures declare their signature and contractual sequent but omit a body (`;`). Their linkage is governed by Clause 16.

[8] _Recursion._ Procedures may reference themselves directly or indirectly. Implementations may warn when they detect unprovable termination but shall not reject recursive procedures solely for that reason.

#### §5.4.4 Semantics

[1] Procedure definitions introduce callable entities that, when referenced, produce values of FunctionType (§7.4). The type records parameter types, return type, grant set, and contractual sequent.

[2] The contractual sequent governs three responsibilities:

- **grants clause** (before turnstile): grants that must be available when the procedure executes
- **must** (after turnstile, before arrow): predicates that must hold when the procedure begins execution
- **will** (after arrow): predicates that will hold when the procedure completes normally

[3] Receiver shorthands desugar before semantic analysis so that tooling observes an explicit `self` parameter bound to `Self` with the requested permission qualifier.

[4] When procedures include contractual sequents, the sequent shall appear contiguous with the signature; implementations shall accept either the same line or the immediately following line. For pure procedures, the sequent may be omitted entirely.

[5] `comptime` procedures execute during compile-time evaluation. Failure to evaluate successfully renders any compile-time use ill-formed.

##### §5.4.4.1 Parameter Responsibility Semantics [decl.function.params.responsibility]

[6] Parameter cleanup responsibility is determined by the presence of the `move` modifier:

**Non-responsible parameters (default):**

```cursive
procedure process(data: Buffer)
{
    // data is non-responsible (like 'let data <- argument')
    // data.drop() NOT called when procedure returns
}
```

When a procedure is called with a non-responsible parameter, the argument remains valid after the call. The parameter binding does not invoke a destructor.

**Responsible parameters (`move` modifier):**

```cursive
procedure consume(move data: Buffer)
{
    // data is responsible (like 'let data = argument')
    // data.drop() WILL be called when procedure returns
}
```

When a procedure is called with a responsible parameter, cleanup responsibility transfers from the argument to the parameter. The argument binding becomes invalid (moved) and the parameter binding invokes the destructor at the end of the procedure body.

[7] **Call site requirement.** When calling a procedure with a `move` parameter, the call site must use the `move` keyword on the argument:

```cursive
let buffer = Buffer::new()
process(buffer)             // ✅ OK: non-responsible parameter, buffer still valid
consume(move buffer)        // ✅ OK: responsible parameter, buffer becomes invalid
consume(buffer)             // ❌ ERROR E05-409: missing move for responsible parameter
```

[8] **Binding semantics equivalence.** Parameter responsibility semantics are equivalent to the corresponding local binding forms:

| Parameter Form | Equivalent Local Binding | Responsibility | Destructor Called |
| -------------- | ------------------------ | -------------- | ----------------- |
| `data: T`      | `let data <- argument`   | NO             | NO                |
| `move data: T` | `let data = argument`    | YES            | YES               |

This equivalence ensures consistent behavior between parameter passing and local bindings.

#### §5.4.5 Examples (Informative)

**Example 5.4.5.1 (Pure procedure with expression body):**

```cursive
procedure clamp(value: i32, min: i32, max: i32): i32
= value.max(min).min(max)
```

[1] Expression-bodied procedures are implicitly pure; no sequent is needed.

**Example 5.4.5.2 (Non-responsible parameter):**

```cursive
procedure inspect(data: Buffer)
{
    println("Size: {}", data.size())
    // data remains valid, no destructor called
}

let buffer = Buffer::new()
inspect(buffer)                  // buffer still valid after call
buffer.use()                     // ✅ OK
```

**Example 5.4.5.3 (Responsible parameter with move):**

```cursive
procedure consume(move data: Buffer)
{
    process_data(data)
    // data.drop() called automatically when procedure returns
}

let buffer = Buffer::new()
consume(move buffer)             // buffer becomes invalid
// buffer.use()                  // error[E11-503]: use of moved value
```

**Example 5.4.5.4 (Procedure with receiver and grants):**

```cursive
procedure deposit(~!, amount: i64)
    [[ ledger::post |- amount > 0 => self.balance >= amount ]]
{
    self.balance += amount
}
```

**Example 5.4.5.5 (Pure procedure without sequent):**

```cursive
procedure add(a: i32, b: i32): i32
{
    result a + b
}
```

[2] Pure procedures with no grants, preconditions, or postconditions omit the sequent entirely.

**Example 5.4.5.6 (Parameter responsibility modifiers):**

```cursive
// Non-responsible parameter (default)
procedure inspect(data: Buffer)
{
    println("Size: {}", data.size())
    // data is NOT destroyed when procedure returns
}

// Responsible parameter (takes ownership)
procedure consume(move data: Buffer)
{
    process_buffer(data)
    // data IS destroyed when procedure returns
}

procedure demo()
    [[ alloc::heap |- true => true ]]
{
    let buffer = Buffer::new()

    inspect(buffer)                    // ✅ OK: buffer still valid after call
    println("Still valid: {}", buffer.size())

    consume(move buffer)               // ✅ OK: transfer responsibility
    // buffer is now invalid (moved)
}
```

**Example 5.4.5.7 - invalid (Expression body with explicit sequent):**

```cursive
procedure unsafe_add(lhs: i32, rhs: i32): i32
    [[ io::write ]]
= lhs + rhs  // error[E05-408]: expression bodies cannot have explicit sequents
```

**Example 5.4.5.8 - invalid (Missing move at call site):**

```cursive
procedure consume(move data: Buffer)
{ }

let buffer = Buffer::new()
consume(buffer)              // error[E05-409]: parameter requires move
```

**Example 5.4.5.9 - invalid (Unexpected move at call site):**

```cursive
procedure process(data: Buffer)
{ }

let buffer = Buffer::new()
process(move buffer)         // error[E05-410]: parameter does not accept move
```

#### §5.4.6 Conformance Requirements [decl.function.requirements]

[1] Implementations shall diagnose malformed or missing receiver parameters on procedures (E05-401).

[2] Implementations shall enforce parameter responsibility semantics:

- Track which parameters have the `move` modifier (responsible)
- Parameters without `move` are non-responsible (do not call destructors)
- Verify call sites use `move` when required (E05-409)
- Reject `move` at call sites when parameter is non-responsible (E05-410)

[3] Compilers shall ensure that procedures with empty grant sets do not perform grant-requiring operations (E05-406) and that expression-bodied forms do not include explicit contractual sequents (E05-408).

[4] Implementations shall enforce compile-time restrictions on `comptime` procedures (E05-402) and require explicit return types unless §5.4.3[6] applies (E05-403).

[5] Tooling shall record contractual sequents, grant sets, receiver permissions, parameter responsibility modifiers, and callable kinds in reflection metadata so that downstream analyses can reason about capability requirements and cleanup responsibilities.
