# Cursive Language Specification

## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-9_Grant-Declarations.md
**Section**: §5.9 Grant Declarations
**Stable label**: [decl.grant]
**Forward references**: Clause 13 [contract], Clause 14 [witness]

[ Note: TODO — These references will be updated once the contracts and witness clauses are authored; the current text captures intent but not the final cross-references. — end note ]

---

### §5.9 Grant Declarations [decl.grant]

#### §5.9.1 Overview

[1] Grant declarations introduce user-defined capability tokens at module scope. Grants are compile-time annotations used in procedure contractual sequent specifications to track side effects and operational capabilities.

[2] This section specifies the declaration syntax and basic semantics for user-defined grants. The complete grant system, including built-in grants, verification rules, and propagation semantics, is specified in Clause 9 [grant].

#### §5.9.2 Syntax

```ebnf
grant_declaration
    ::= visibility_modifier? "grant" identifier
```

[3] Grant declarations consist of an optional visibility modifier followed by the `grant` keyword and an identifier.

#### §5.9.3 Constraints

[1] _Visibility defaults._ Grant declarations at module scope default to `internal` visibility, matching the default for module-scoped declarations (§5.6.3). The modifiers `public`, `internal`, `private`, and `protected` control accessibility across modules.

[2] _Namespace uniqueness._ Grant names share the lexical namespace with other identifiers (§5.1.3) and shall not collide within the same scope.

[3] _Reserved namespaces._ Grant names shall not conflict with reserved grant namespaces. The following top-level namespaces are reserved for built-in grants:

- `alloc`
- `fs`
- `net`
- `thread`
- `time`
- `ffi`
- `unsafe`
- `panic`

Violations produce diagnostic E05-901.

[4] _Module scope only._ Grant declarations shall appear at module level only, not nested within types, procedures, or blocks. Violations produce diagnostic E05-902.

[5] _Forward references._ Grant declarations may be referenced before their definition within the same module due to two-phase compilation (§2.2).

#### §5.9.4 Semantics

[1] Grant declarations introduce compile-time capability tokens with no runtime representation. The fully qualified grant path is constructed from the module path and grant name: for grant `g` in module `M`, the path is `M::g`.

[2] User-defined grants are referenced in procedure contractual sequent specifications using their fully qualified paths (see §5.4 [decl.function] for contractual sequent syntax).

[3] Grant visibility determines which modules may reference the grant in their contractual sequents. Private grants are accessible only within the declaring module; public grants are accessible from any module that imports the declaring module.

[4] Grant declarations have zero runtime cost and are fully erased during compilation. All grant checking is performed statically at compile time.

#### §5.9.5 Examples

**Example 5.9.5.1 (Basic grant declarations):**

```cursive
public grant query
public grant write
internal grant admin
private grant internal_cache
```

**Example 5.9.5.2 (Using grants in contractual sequents):**

```cursive
// File: database.cursive
public grant query
public grant write

procedure execute_query(sql: string@View): [i32]
    {| query |- sql.len() > 0 => true |}
{
    // Implementation uses local grant 'query'
    result perform_query(sql)
}

// File: application.cursive
import database

procedure fetch_items(): [i32]
    {| database::query |- true => true |}
{
    result database::execute_query("SELECT id FROM items")
}
```

**Example 5.9.5.3 (Mixed built-in and user-defined grants):**

```cursive
import database

procedure save_query_results(path: string@View): ()
    {| database::query, fs::write, alloc::heap |- path.len() > 0 => true |}
{
    let results = database::execute_query("SELECT * FROM items")
    write_to_file(path, results)
}
```

#### §5.9.6 Diagnostics

[1] Minimal diagnostics:

| Code  | Condition                                    |
| ----- | -------------------------------------------- |
| E05-901 | Grant name conflicts with reserved namespace |
| E05-902 | Grant declaration not at module scope        |
| E05-903 | Duplicate grant name in same module          |

#### §5.9.7 Integration with Grant System

[1] Grant declarations defined in this section participate in the grant system specified in Clause 9:

- Grant path resolution (§9.5.2 [grant.user.resolution])
- Visibility and access control (§9.5.3 [grant.user.visibility])
- Subset verification and propagation (§9.5.4 [grant.user.usage])
- Wildcard expansion `M::*` (§9.5.5 [grant.user.wildcard])
- Grant polymorphism (§9.5.6 [grant.user.polymorphism])

[2] After declaration, user-defined grants are treated identically to built-in grants for all verification and propagation purposes.

#### §5.9.8 Conformance Requirements [decl.grant.requirements]

[1] Implementations shall recognize grant declarations at module scope and verify:

- Grant names do not conflict with reserved namespaces (E05-901)
- Grant declarations appear only at module scope (E05-902)
- Grant names are unique within their module (E05-903)

[2] Implementations shall construct fully qualified grant paths from module paths and grant names.

[3] Implementations shall enforce visibility constraints on grant usage in contractual sequent specifications.

[4] Implementations shall forward grant declarations to the grant system (Clause 9) for verification and propagation checking.

---

**Previous**: §5.8 Program Entry (§5.8 [decl.entry]) | **Next**: Clause 6 Names and Scopes (§6.1 [name.ident])
