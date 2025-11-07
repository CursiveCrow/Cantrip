# Cursive Language Specification

## Clause 8 — Expressions

**Clause**: 8 — Expressions
**File**: 08-7_Constant-and-Comptime.md  
**Section**: §8.7 Constant Expressions and Comptime Contexts  
**Stable label**: [expr.constant]  
**Forward references**: Clause 2 §2.2 [lex.phases], Clause 5 §5.2 [decl.variable], Clause 10 [comptime], Clause 11 [generic]

---

### §8.7 Constant Expressions and Comptime Contexts [expr.constant]

#### §8.7.1 Overview

[1] Constant expressions are those evaluable during translation. They appear in `const` bindings, array lengths, enum discriminants, attribute arguments, and `comptime` blocks. This subclause defines the admissible constructs inside const contexts and the grant restrictions that apply during compile-time execution.

#### §8.7.2 Const Requirements

[2] An expression context requires const evaluation when:

- It appears in a `const` or `static` binding (§5.2).
- It determines a type-level parameter (array length `[T; n]`, repeat literal `[value; n]`).
- It occurs inside a `comptime { … }` block (Clause 10).
- It provides default values for const generic parameters (§11).

[3] Const expressions may contain:

- Literals and literal operators.
- Previously defined constants.
- Pure `comptime procedure` invocations.
- Non-capturing closures whose bodies meet these constraints.
- `type_name`, `type_id`, and `type_info` queries (§7.8).

[4] Disallowed constructs include heap allocation, IO, clock queries, pointer arithmetic on runtime pointers, and any call that requires a runtime-only grant. Using such constructs inside a const context produces diagnostic E08-700.

#### §8.7.3 Comptime Blocks

[5] Grammar: `ComptimeExpr ::= 'comptime' Block` (Annex A §A.4.18).

[6] Semantics: the block executes during the compile-time execution phase (§2.2) with its own scope. Bindings created inside the block vanish after evaluation; the block’s `result` expression becomes a literal embedded into the compiled program.

[7] Grants: comptime blocks may only request grants whose names begin with `comptime.` (e.g., `comptime.alloc`, `comptime.codegen`). Attempting to use runtime grants such as `io.write` emits E08-701.

[8] Diagnostics: any panic, overflow, or error encountered during comptime evaluation shall be reported as a compile-time error referencing both the comptime block and the offending expression.

#### §8.7.4 Interaction with Generics

[9] Const generic parameters may appear in expressions. During instantiation, each const parameter is substituted with a concrete value, after which the expression behaves like any other const expression. Type-level functions (`type_name`, `type_info`) may use const generics as long as they remain evaluable at compile time.

#### §8.7.5 Diagnostics Summary

| Code | Condition |
| --- | --- |
| E08-700 | Runtime-only construct used in const context |
| E08-701 | Comptime block requested non-comptime grant |

#### §8.7.6 Example

```cursive
const POWERS: [u32; 8] = comptime {
    let mut table = [0; 8]
    var i = 0
    loop i < table.len() {
        table[i] = (1u32 << i)
        i += 1
    }
    result table
}
```

[10] The comptime block allocates a local array, fills it, and returns it. Because it uses only comptime-safe operations, it is valid for const evaluation.

---
