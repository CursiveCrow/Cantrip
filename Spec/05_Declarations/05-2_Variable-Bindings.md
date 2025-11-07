# Cursive Language Specification
## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-2_Variable-Bindings.md
**Section**: §5.2 Variable Bindings and Initialisers
**Stable label**: [decl.variable]  
**Forward references**: §4.6 [module.initialization], §5.3 [decl.pattern], §5.7 [decl.initialization], §12.4 [memory.permission]

---

### §5.2 Variable Bindings and Initialisers [decl.variable]

#### §5.2.1 Overview

[1] Variable bindings introduce identifiers into the lexical namespace with explicit binding mutability (`let`, `var`) and optional pattern destructuring.

[2] Bindings may appear at module scope or within block scopes; their storage duration and initialisation ordering depend on location (§5.7, §4.6).

[3] This subclause defines binding syntax, pattern forms, initialiser requirements, and the interaction with shadowing, permissions, and compile-time constants.

#### §5.2.2 Syntax

```ebnf
variable_binding
    ::= binding_head pattern type_annotation? binding_operator initializer

binding_head
    ::= "let"
     | "var"
     | "shadow" "let"
     | "shadow" "var"

pattern
    ::= identifier
     | "{" identifier_list "}"

identifier_list
    ::= identifier ("," identifier)*

type_annotation
    ::= ":" type_expression

binding_operator
    ::= "="
     | "<-"

initializer
    ::= expression
```

[ Note: See Annex A §A.7 [grammar.declaration] for complete declaration grammar.
— end note ]

[1] Every binding introduces at least one identifier. Patterns specify that identifier set explicitly; anonymous bindings are ill-formed.

[2] When a pattern binds more than one identifier, a `:{Type}` annotation is mandatory and applies uniformly to every identifier in the pattern.

[3] `initializer` must be present for every binding form. The binding operator determines whether the binding owns the produced value (`=`) or establishes a reference binding (`<-`) as described in §5.7.5 [decl.initialization].

#### §5.2.3 Constraints

[1] *Explicit shadowing.* `shadow let` and `shadow var` shall only appear when an enclosing scope already defines the same identifier. Absence of a matching outer binding yields diagnostic E05-201.

[2] *Single assignment.* `let` bindings shall not be reassigned after initialisation. `var` bindings may be reassigned. Attempts to reassign a `let` binding emit diagnostic E05-202.

[3] *Pattern uniformity.* Multi-identifier patterns require a type annotation `:{Type}` that applies to every identifier in the pattern. All identifiers in the pattern must be distinct. Violations produce diagnostic E05-203.

[4] *Initialiser completeness.* The initialiser expression shall provide values for every identifier in the pattern. If destructuring fails (for example, due to mismatched arity or missing fields), diagnostic E05-204 is issued.

[5] *Compile-time constants.* A binding is a compile-time constant only when it is a module-scope `let` with a compile-time evaluable initialiser or a binding declared within a `comptime` block. Function- or block-scope bindings remain runtime values regardless of their initialiser expression.

[6] *Permission independence.* Binding mutability controls reassignment, not value mutation. Value mutation permissions arise from the type’s permission qualifiers or region attributes (§12.4).

[7] *Forward references.* Due to two-phase compilation (§2.2), bindings may reference declarations that appear later in the translation unit. Shadowing rules are evaluated after parsing, ensuring forward references do not bypass explicit shadow requirements.

#### §5.2.4 Semantics

[1] Binding introduction inserts the identifier or identifiers into the current lexical scope’s namespace, honouring the single-namespace rule (§5.1.3) and shadowing constraints.

[2] For patterns, the initialiser expression is evaluated once; its value is destructured into the constituent bindings. The implementation behaves as if a temporary value were created, followed by individual bindings drawing from that value.

[3] Module-scope bindings participate in the eager dependency graph (§4.6); their initialisers shall not depend on bindings that, directly or indirectly, require the binding being initialised.

[4] Pattern bindings do not change visibility or storage semantics: each identifier inherits the visibility modifier and scope of the encompassing binding.

#### §5.2.5 Examples (Informative)

**Example 5.2.5.1 (Simple bindings):**
```cursive
let ORIGIN: i32 = 0
var counter = 1
shadow let counter = counter + 1
```

**Example 5.2.5.2 (Pattern binding with uniform type):**
```cursive
let {x, y, z}: Point3 = make_point()
// Semantically equivalent to introducing a temporary Point3
```

**Example 5.2.5.3 - invalid (Implicit shadowing):**
```cursive
let limit = 10
{
    let limit = limit + 5  // error[E05-201]: use 'shadow let limit'
}
```

**Example 5.2.5.4 - invalid (Pattern arity mismatch):**
```cursive
let {left, right}: Pair = make_triple()  // error[E05-204]
```

### §5.2.6 Conformance Requirements [decl.variable.requirements]

[1] Implementations shall diagnose uses of `shadow` that do not match an existing outer binding with E05-201 and forbid the redeclaration.

[2] Implementations shall reject reassignments to `let` bindings (E05-202) and ensure that pattern bindings obey the uniqueness and completeness constraints in §5.2.3 (E05-203–E05-204).

[3] Module-scope bindings shall participate in the dependency analysis of §4.6; implementations shall detect initialiser cycles (E05-701) and block dependent initialisers per §5.7.

[4] Binding mutability shall not alter permission semantics; compilers shall defer permission enforcement to Clause 12 while ensuring the binding itself respects §5.2.3[6].
