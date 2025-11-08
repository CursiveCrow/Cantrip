# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-3_Scope-and-Lifetime.md
**Section**: §3.3 Scope and Lifetime (Overview)
**Stable label**: [basic.scope]
**Forward references**: §3.1 [basic.object], §3.4 [basic.storage], Clause 6 [name], Clause 11 [memory]

---

### §3.3 Scope and Lifetime (Overview) [basic.scope]

#### §3.3.1 Overview

[1] _Scope_ and _lifetime_ are dual concepts governing name visibility and object validity. Scope determines where a binding may be referenced; lifetime determines when an object's storage is valid. These concepts interact to ensure that programs can reference only valid objects through visible names.

[2] Clause 6 specifies scope formation and name resolution in detail. Clause 11 specifies lifetime management through regions and permissions. This subclause establishes the conceptual relationship between these mechanisms.

#### §3.3.2 Scope [basic.scope.scope]

##### §3.3.2.1 Definition

[3] A _scope_ is a region of source text where a binding is visible. Scopes are determined lexically by syntactic constructs: module boundaries, procedure bodies, block statements, and region blocks.

[4] Every binding has exactly one introducing scope (the scope where the declaration appears) and is visible in that scope and all nested scopes, subject to shadowing rules (§6.3).

##### §3.3.2.2 Scope Hierarchy

[5] Scopes form a tree structure:

- **Module scope**: The outermost scope of a compilation unit (§2.5), containing top-level declarations
- **Procedure scope**: Established by procedure bodies, containing parameters and local bindings
- **Block scope**: Established by `{ … }` blocks, containing block-local declarations
- **Region scope**: A special form of block scope with additional lifetime constraints (§11.3)

[6] Child scopes may access bindings from ancestor scopes. When a nested scope declares a binding with the same identifier as an outer scope, the `shadow` keyword is required (§6.3); without it, the redeclaration is ill-formed.

##### §3.3.2.3 Visibility

[7] A binding is _visible_ in a scope when:

1. The binding was introduced in that scope, OR
2. The binding was introduced in an ancestor scope and not shadowed by an intervening declaration

[8] Visibility is purely lexical and determined at compile time. Module boundaries add a visibility modifier dimension (`public`, `internal`, `private`, `protected`, §5.6) that restricts cross-module access, but within a module, lexical scope rules apply uniformly.

##### §3.3.2.4 Examples

**Example 3.3.2.1 (Scope nesting):**

```cursive
let MODULE_CONST = 100          // Module scope

procedure helper(value: i32): i32
{
    // Procedure scope begins
    let doubled = value * 2      // Procedure-local binding

    {
        // Block scope begins
        shadow let value = doubled + MODULE_CONST
        result value             // Refers to shadowed `value`
    }
    // Block scope ends; shadowed `value` no longer visible

    result doubled               // Original `value` parameter visible
}
```

#### §3.3.3 Lifetime [basic.scope.lifetime]

##### §3.3.3.1 Definition

[9] The _lifetime_ of an object is the interval during program execution when the object's storage is allocated and valid. Lifetimes have precise beginning and end points:

- **Begin**: Storage allocation and initialization complete
- **End**: Object destruction begins (destructor executes, storage released)

[10] Between these points, the object is _live_; outside this interval, the object is _dead_. Accessing dead objects produces undefined behavior [UB-ID: B.2.01].

##### §3.3.3.2 Lifetime Categories

[11] Lifetimes are categorized by their determination mechanism:

- **Lexical lifetimes**: Determined by syntactic scope (automatic and region objects)
- **Static lifetime**: Spans the entire program execution (module-scope bindings)
- **Manual lifetimes**: Programmer-controlled through explicit allocation and deallocation (not directly supported; use region blocks instead)

[12] Cursive enforces LIFO (last-in, first-out) discipline for lexical lifetimes: objects created later are destroyed first. Region blocks (§11.3) make this discipline explicit and enable fine-grained control over destruction order.

##### §3.3.3.3 Scope-Lifetime Correspondence

[13] For most bindings, scope and lifetime are aligned: an object's lifetime matches the lexical scope of its binding. When a scope exits, objects created in that scope are destroyed in reverse declaration order.

[14] Reference bindings (`let x <- expr`, §5.7.5) break this correspondence: the binding's scope may be shorter than the referenced object's lifetime. The permission system ensures that such references remain valid by preventing the referenced object from being destroyed while references exist.

##### §3.3.3.4 Examples

**Example 3.3.3.1 (Lexical lifetime and destruction order):**

```cursive
procedure demo()
    [[ alloc::heap |- true => true ]]
{
    let first = allocate_resource()     // Created first
    let second = allocate_resource()    // Created second
    let third = allocate_resource()     // Created third

    // Destruction occurs in reverse order:
    // third destroyed, then second, then first
}
```

**Example 3.3.3.2 (Reference binding with extended lifetime):**

```cursive
let permanent = create_static_config()  // Static lifetime

procedure use_config()
{
    let config_ref <- permanent          // Reference binding
    process(config_ref)
    // config_ref scope ends but permanent's lifetime continues
}
```

#### §3.3.4 Lifetime Extension [basic.scope.extension]

[15] In specific situations, Cursive extends object lifetimes to prevent use-after-free:

- **Temporary lifetime extension**: Temporary values created during expression evaluation survive until the full-expression completes
- **Static promotion**: Compile-time constant expressions may be promoted to static storage with program lifetime

[16] Explicit lifetime extension through reference counting or garbage collection is not provided by the core language. Programs requiring such patterns shall use explicit region management (§11.3) or library abstractions.

#### §3.3.5 Region-Based Lifetime Management [basic.scope.regions]

[17] Cursive's primary lifetime control mechanism is the region block:

```cursive
region session {
    let handle = allocate_in_region()
    process(handle)
}
// All objects allocated in `session` are destroyed here
```

[18] Region blocks provide:

- Explicit lifetime boundaries marked in source code
- LIFO destruction order
- Compile-time escape analysis preventing dangling references
- Zero runtime overhead (compared to manual tracking)

[19] Complete region semantics, including escape analysis and region polymorphism, are specified in §11.3.

#### §3.3.6 Integration [basic.scope.integration]

[20] Scope and lifetime rules integrate with:

- **Name resolution (Clause 6)**: Scopes determine which bindings are visible to name lookup
- **Definite assignment (§5.7)**: Lifetime boundaries determine when objects must be initialized
- **Memory model (Clause 11)**: Regions and permissions enforce lifetime safety
- **Type system (Clause 7)**: Type rules ensure operations occur only on live objects

[21] The separation of concerns allows local reasoning: programmers understand scope through lexical structure and lifetime through regions and permissions, without tracking global state.

#### §3.3.7 Conformance Requirements [basic.scope.requirements]

[22] Implementations shall:

1. Form scopes according to the lexical structure defined in Clause 6
2. Enforce lexical visibility rules and shadowing requirements
3. Align object lifetimes with binding scopes for non-reference bindings
4. Destroy objects in reverse declaration order when scopes exit
5. Extend temporary lifetimes to full-expression boundaries
6. Implement region-based lifetime management with escape analysis
7. Produce undefined behavior when objects are accessed outside their lifetimes

[23] Conforming programs shall not rely on unspecified destruction order or timing beyond the guarantees provided by this clause and Clause 11.

---

**Previous**: §3.2 Types (Overview and Classification) | **Next**: §3.4 Storage Duration
