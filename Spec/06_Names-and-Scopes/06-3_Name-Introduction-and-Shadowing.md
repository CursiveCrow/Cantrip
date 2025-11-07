# Cursive Language Specification
## Clause 6 — Names, Scopes, and Resolution

**Clause**: 6 — Names, Scopes, and Resolution
**File**: 06-3_Name-Introduction-and-Shadowing.md
**Section**: §6.3 Name Introduction and Shadowing
**Stable label**: [name.shadow]  
**Forward references**: §2.3 [lex.tokens], §5.2 [decl.variable], §5.5 [decl.type], §6.4 [name.lookup], §12.4 [memory.permission]

---

### §6.3 Name Introduction and Shadowing [name.shadow]

#### §6.3.1 Overview [name.shadow.overview]

[1] This subclause defines how declarations introduce bindings into the scope tree described in §6.2, the properties recorded on each binding, and the rules governing shadowing. Cursive’s unified namespace means every declaration, regardless of category (type, value, module, label), shares the same binding table per scope.

[2] The explicit `shadow` keyword enforces deliberate rebinding of outer-scope names. Without it, redeclaration in the same scope—or in a nested scope that would otherwise hide an outer binding—is ill-formed, preventing accidental name capture and improving clarity for both humans and tooling.

#### §6.3.2 Syntax [name.shadow.syntax]

[3] Binding-introducing declarations employ the following productions (excerpted from Appendix A.4):

```ebnf
declaration
    ::= variable_declaration
     | type_declaration
     | procedure_declaration
     | contract_declaration
     | predicate_declaration
     | effect_declaration
     | module_declaration
     | label_declaration

variable_declaration
    ::= binding_head pattern type_annotation? "=" expression

binding_head
    ::= "let"
     | "var"
     | "shadow" "let"
     | "shadow" "var"
     | "shadow" identifier_declaration_head
```

[4] `identifier_declaration_head` captures non-variable declarations (type, procedure, module, etc.) that may legally use `shadow`. Full grammar details appear in Appendix A.4; this subclause restricts when the `shadow` prefix is valid.

#### §6.3.3 Constraints [name.shadow.constraints]

[5] Every declaration introduces at most one binding per identifier. Multiple declarators in the same statement are prohibited; use pattern destructuring when multiple identifiers are required (§5.2).

[6] Redeclaring a name within the same scope without `shadow` is ill-formed (diagnostic E06-300). The rule applies uniformly across categories: a type cannot be redeclared as a value, nor a value as a type, in the same scope.

[7] The `shadow` keyword may appear only in a scope strictly nested within the scope that defines the original binding. Using `shadow` in the same scope as the target binding or at module scope is ill-formed (diagnostic E06-301).

[8] `shadow` shall not target predeclared/built-in identifiers from the universe scope (§6.6). Attempts to shadow such bindings are rejected (diagnostic E06-302).

[9] Shadowing preserves visibility modifiers from the new declaration; visibility is not inherited from the shadowed binding. A `shadow` declaration must spell out its own visibility when applicable.

[10] Pattern destructuring with `shadow` (e.g., `shadow let {x, y}`) may only shadow identifiers that already exist in an enclosing scope. All identifiers in the pattern must correspond to bindings that can be shadowed; otherwise, diagnostic E06-303 is emitted.

#### §6.3.4 Semantics [name.shadow.semantics]

[11] When a declaration introduces a binding, the compiler records the tuple `(identifier, entity, type, visibility, source_location)` and inserts it into the current scope’s binding table. Two-phase compilation (§2.2) ensures that all bindings are recorded before semantic resolution proceeds.

[12] During name lookup (§6.4), when a reference traverses from an inner scope to an outer scope, any inner binding marked with `shadow` masks the outer binding for the duration of that inner scope. Once control exits the inner scope, the original binding becomes visible again.

[13] Shadowing does not alter the lifetime or storage duration of the shadowed value; it simply changes which binding a given identifier resolves to. Values captured by closures retain the semantics defined in Clause 12 regardless of shadowing.

[14] Label declarations (e.g., `'loop`) participate in shadowing rules but are confined to procedure scope. A label declared with `shadow` must target a label from an enclosing block within the same procedure; cross-procedure label shadowing is impossible because labels do not escape their procedure (§6.2).

#### §6.3.5 Examples (Informative) [name.shadow.examples]

**Example 6.3.5.1 (Shadowing across nested scopes):**
```cursive
let threshold = 100  // Module-scope binding

procedure process(value: i32): i32
    {| |- true => true |}
{
    let result = value

    {
        shadow let result = result + threshold  // Shadows outer `result`
        if result > 200 {
            shadow let threshold = 150  // Shadows module-scope `threshold`
            return result - threshold
        }
        // `threshold` here still refers to 100; shadow lasts only inside inner block
    }

    return result
}

// Invalid: redeclaration without `shadow`
// let threshold = 200  // ERROR E06-300

// Invalid: `shadow` in same scope
// shadow let threshold = 200  // ERROR E06-301
```

#### §6.3.6 Conformance Requirements [name.shadow.requirements]

[15] Implementations shall maintain a single binding table per scope and reject duplicate entries unless the declaration uses `shadow` in a nested scope (E06-300).

[16] Implementations shall verify that `shadow` targets an existing binding in an enclosing scope, raising E06-301 when it targets the current scope and E06-303 when pattern elements do not correspond to existing bindings.

[17] Implementations shall prohibit shadowing of predeclared identifiers (E06-302) and ensure that shadowed labels remain confined to their procedure scope.
