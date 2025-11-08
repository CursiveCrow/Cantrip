# Cursive Language Specification

## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-1_Declaration-Overview.md
**Section**: §5.1 Declaration Overview
**Stable label**: [decl.overview]  
**Forward references**: §2.5 [lex.units], Clause 4 [module], Clause 6 [name], Clause 7 [type], Clause 8 [expr], Clause 10 [generic], Clause 11 [memory], Clause 12 [contract], Clause 13 [witness]

---

### §5.1 Declaration Overview [decl.overview]

#### §5.1.1 Overview

[1] Clause 5 specifies the syntactic and semantic forms that introduce names in Cursive programs. A _declaration_ binds an identifier to a category (value, type, callable, predicate, contract, or grant), assigns visibility and storage attributes, and determines how later references participate in the permission, grant, and modal systems.

[2] This clause governs module-scope and block-scope declarations, destructuring patterns, callable definitions, type introductions, and visibility modifiers. It interacts with Clause 4 for module organisation, Clause 6 for name lookup, Clause 7 for type formation, Clause 8 for expression semantics, Clause 11 for generics, and Clause 12 for the memory model. Forward references to those clauses are identified within the relevant subclauses.

#### §5.1.2 Declaration Categories

[3] Every declaration belongs to exactly one of the following categories:

- **Variable bindings** (`let`, `var`, `shadow let`, `shadow var`) introduce bindings with explicit mutability and optional pattern destructuring.
- **Binding patterns** extend variable bindings with structured destructuring that binds multiple identifiers simultaneously.
- **Callable declarations** (`procedure`, `comptime procedure`, `extern procedure`) introduce named computations, including their parameter lists, contracts, and grant obligations.
- **Type declarations** (`record`, `enum`, `modal`, `type`) introduce nominal or transparent types into the unified namespace.
- **Visibility modifiers** (`public`, `internal`, `private`, `protected`) refine the accessibility of the associated declaration.
- **Auxiliary declarations** (behaviors, contracts, grants) rely on their dedicated clauses for semantics but use this clause for name introduction and visibility rules. Grant declarations are specified in §5.9 [decl.grant]; grant system semantics are specified in Clause 12 [contract].

[4] Declarations located at module scope participate in module initialisation ordering (§4.6) and contribute to the program entry model (§5.8). Declarations inside block scopes follow the storage-duration rules established in §5.7 and the scoping rules of Clause 6.

#### §5.1.3 Namespace and Scope Invariants

[5] Cursive maintains a single lexical identifier namespace per scope: within a given scope, types and values share the same identifier space. Redeclaration within that scope is ill-formed. Shadowing an outer-scope binding requires an explicit `shadow` keyword.

[6] Two-phase compilation (§2.2) guarantees that declarations may appear in any order within a translation unit. Parsing records the complete declaration inventory before semantic analysis, so forward references are resolved without separate stub declarations.

#### §5.1.4 Integration with Other Systems

[7] The permission system (Clause 11) interprets binding mutability, region annotations, and ownership qualifiers attached to declarations.

[8] The grant system and contractual sequents (Clause 12) extend callable declarations. Clause 5 defines where these sequents attach to declarations while Clause 12 provides the semantic obligations.

[9] **Name resolution integration.** Clause 6 depends on the scope and visibility metadata emitted here when performing unqualified and qualified name lookup. During two-phase compilation (§2.2), the parser records all declarations in the compilation unit before name resolution proceeds. This ensures that:

- Forward references within the same compilation unit are resolved without requiring stub declarations.
- Qualified name lookup (§6.4) can access module-scope declarations based on visibility modifiers (§5.6).
- Shadowing rules (§6.3) are evaluated after all declarations are recorded, ensuring that explicit `shadow` keywords are validated against the complete scope hierarchy.
- Import and use bindings (§4.3) participate in name lookup using the same visibility rules as local declarations.

[10] **Module initialization integration.** Module-scope declarations participate in the initialization ordering defined in §4.6. Variable bindings (§5.2) with initializers are evaluated according to the eager/lazy dependency classification, and definite assignment (§5.7) ensures that all bindings are initialized before use.

#### §5.1.5 Examples (Informative)

**Example 5.1.5.1 (Representative module declarations):**

```cursive
public record Account { id: u64, balance: i64 }

use bank::ledger::post_entry

let DEFAULT_LIMIT = 10_000
shadow var session_state = Session::new()

public procedure create_account(id: u64): Account
{
    result Account { id, balance: 0 }
}
```

[1] This module defines a public record, imports a ledger operation, introduces immutable and mutable bindings, and exposes a callable—all forms elaborated in subsequent subclauses.

### §5.1.6 Conformance Requirements [decl.overview.requirements]

[1] Implementations shall recognise each declaration category enumerated in §5.1.2 and apply the specialised rules defined in §§5.2–5.8 to every occurrence.

[2] Implementations shall enforce the single lexical identifier namespace described in §5.1.3 and require `shadow` when redeclaring identifiers from an enclosing scope.
