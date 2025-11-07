# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-6_Name-Binding-Categories.md
**Section**: §3.6 Name Binding Categories
**Stable label**: [basic.binding]
**Forward references**: §3.1 [basic.object], §3.2 [basic.type], Clause 5 [decl], Clause 6 [name], Clause 7 [type]

---

### §3.6 Name Binding Categories [basic.binding]

#### §3.6.1 Overview

[1] Cursive binds identifiers to entities across multiple _binding categories_. Each category determines what the identifier names, how it participates in lookup (Clause 6), and what operations are permitted on it. Understanding binding categories is essential for resolving identifiers and interpreting declaration forms.

[2] Cursive employs a unified namespace per scope (§6.2): identifiers from all categories share the same binding table. This prevents accidental collisions while enabling unambiguous resolution. Clause 5 specifies how declarations introduce bindings; this subclause classifies the kinds of entities those bindings may name.

#### §3.6.2 Binding Categories [basic.binding.categories]

[3] Every binding belongs to exactly one of the following categories:

1. **Value bindings** — Name objects with types and values (variables, constants)
2. **Type bindings** — Name types introduced by declarations
3. **Module bindings** — Name modules for qualified access
4. **Callable bindings** — Name procedures (a specialized form of value binding)
5. **Predicate bindings** — Name predicates for generic constraints
6. **Contract bindings** — Name contracts for implementation obligations
7. **Label bindings** — Name control-flow targets for `break`/`continue`
8. **Grant bindings** — Name capability tokens for contractual sequents

[4] Table 3.6.1 summarizes each category with introducing forms and usage contexts.

**Table 3.6.1 — Name binding categories**

| Category  | Introducing forms                         | Usage contexts                       | Specified in |
| --------- | ----------------------------------------- | ------------------------------------ | ------------ |
| Value     | `let`, `var`, module-scope bindings       | Expressions, assignments             | §5.2         |
| Type      | `record`, `enum`, `modal`, `type` (alias) | Type annotations, generic arguments  | §5.5         |
| Module    | Compilation units, `import` aliases       | Qualified names `module::item`       | Clause 4     |
| Callable  | `procedure` declarations                  | Call expressions, first-class values | §5.4         |
| Predicate | `predicate` declarations                  | Generic bounds, `with` clauses       | §10.4        |
| Contract  | `contract` declarations                   | Contract clauses on types            | §10.4        |
| Label     | `'label:` in procedure bodies             | `break 'label`, `continue 'label`    | §9.3         |
| Grant     | `grant` declarations                      | Contractual sequent grant sets       | §5.9         |

#### §3.6.3 Value Bindings [basic.binding.value]

[5] _Value bindings_ associate identifiers with objects and values. They are introduced by:

- Variable declarations (`let`, `var`, §5.2)
- Procedure parameters (§5.4)
- Pattern bindings (§5.3)
- Closure captures (§8.4)

[6] Value bindings carry type information, mutability constraints (`let` vs `var`), and permission annotations (`const`, `shared`, `unique`, §11.4). They may be read in expressions and, for mutable bindings with appropriate permissions, written through assignment.

**Example 3.6.3.1 (Value bindings):**

```cursive
let constant = 42                  // Immutable value binding
var mutable = 100                  // Mutable value binding
mutable = mutable + 1              // Assignment permitted

procedure compute(param: i32): i32
{
    // Parameter is a value binding
    let local = param * 2
    result local
}
```

#### §3.6.4 Type Bindings [basic.binding.type]

[7] _Type bindings_ associate identifiers with types. They are introduced by:

- Record declarations (`record R { ... }`, §5.5)
- Enum declarations (`enum E { ... }`, §5.5)
- Modal declarations (`modal M { ... }`, §5.5)
- Type aliases (`type Alias = T`, §5.5)
- Generic type parameters (`<T, U>`, §10.2)

[8] Type bindings are used in type annotations, generic arguments, and type-level expressions. Attempting to use a type binding in a value context (e.g., `let x = RecordTypeName`) produces diagnostic E08-211.

**Example 3.6.4.1 (Type bindings):**

```cursive
record Point { x: f64, y: f64 }    // Introduces type binding `Point`
type Coordinate = f64               // Introduces type alias binding `Coordinate`

let origin: Point = Point { x: 0.0, y: 0.0 }
let x_coord: Coordinate = origin.x
```

#### §3.6.5 Module Bindings [basic.binding.module]

[9] _Module bindings_ represent modules in the import table. They are introduced by:

- Compilation units (implicit module identity, §4.1)
- Import declarations with aliases (`import module as alias`, §4.2)

[10] Module bindings are used exclusively as prefixes in qualified names (`module::item`). They do not appear in value or type contexts. Attempting to use a module binding directly produces diagnostic E06-402.

**Example 3.6.5.1 (Module bindings):**

```cursive
import math::geometry as geom          // Introduces module binding `geom`
import math::statistics                // Module available as `math::statistics`

let area = geom::area_of_circle(5.0)   // Module binding used in qualified name
// let invalid = geom                  // error[E06-402]: module binding in value context
```

#### §3.6.6 Callable Bindings [basic.binding.callable]

[11] _Callable bindings_ are specialized value bindings that name procedures. They combine aspects of value and type bindings:

- As values: Procedures may be passed as arguments, stored in bindings, returned from functions
- As types: Procedure references have callable types (§7.4) that record signatures and contracts

[12] Callable bindings are introduced by procedure declarations (§5.4). They participate in overload resolution when multiple procedures share the same name in different scopes (future feature; currently Cursive requires unique names per scope).

**Example 3.6.6.1 (Callable bindings):**

```cursive
procedure add(a: i32, b: i32): i32
{
    result a + b
}

let operation: (i32, i32) -> i32 = add     // Procedure as first-class value
let result = operation(5, 10)               // Call through binding
```

#### §3.6.7 Predicate and Contract Bindings [basic.binding.trait]

[13] _Predicate bindings_ and _contract bindings_ name type-level abstractions introduced by predicate and contract declarations (§10.4):

- **Predicates**: Collections of procedures with default implementations
- **Contracts**: Interface specifications without implementations

[14] These bindings are used in:

- Generic bounds (`where T: Predicate`, §10.3)
- Type clauses (`record R: Contract with Predicate`, §5.5)
- Witness construction (Clause 13)

**Example 3.6.7.1 (Predicate and contract bindings):**

```cursive
predicate Display {                        // Introduces predicate binding `Display`
    procedure show(~): string@View
}

contract Serializable {                    // Introduces contract binding `Serializable`
    procedure serialize(~): [u8]
}

record User: Serializable with Display {   // Uses both binding categories
    name: string@Managed,

    procedure serialize(~): [u8]
        [[ alloc::heap ]]
    { ... }

    procedure show(~): string@View
    { ... }
}
```

#### §3.6.8 Label Bindings [basic.binding.label]

[15] _Label bindings_ name control-flow targets within procedure bodies. They are introduced by label declarations (`'label:`) and referenced by `break` and `continue` statements (§9.3).

[16] Label bindings have procedure scope: they are visible throughout the procedure but cannot escape it. Labels participate in shadowing rules like other bindings but are distinguished by the leading `'` sigil.

**Example 3.6.8.1 (Label bindings):**

```cursive
procedure search(haystack: [i32], needle: i32): bool
{
    'outer: for row in haystack {
        'inner: for item in row {
            if item == needle {
                break 'outer            // Targets outer loop
            }
        }
    }
    result false
}
```

#### §3.6.9 Grant Bindings [basic.binding.grant]

[17] _Grant bindings_ name capability tokens used in contractual sequent specifications. They are introduced by grant declarations (`grant identifier`, §5.9) and referenced in procedure sequent specifications.

[18] Grant bindings are compile-time only: they have no runtime representation and are fully erased during code generation. They serve to track and verify capability requirements across the codebase.

**Example 3.6.9.1 (Grant bindings):**

```cursive
public grant query                         // Introduces grant binding `query`
public grant write                         // Introduces grant binding `write`

procedure execute_query(sql: string@View): [Row]
    [[ query |- sql.len() > 0 => true ]]   // Uses grant binding `query`
{
    result perform_database_query(sql)
}
```

#### §3.6.10 Unified Namespace [basic.binding.namespace]

[19] All binding categories share a single lexical namespace per scope (§6.2). Within a given scope, identifiers must be unique across categories: a type cannot have the same name as a value binding, a predicate cannot share a name with a procedure, etc.

[20] This unified namespace simplifies lookup (Clause 6) and prevents category-based ambiguities. It means programmers cannot write `let Point = ...` in a scope that already has `record Point { ... }`.

[21] Cross-category uniqueness is enforced during scope formation. Violations produce diagnostic E06-300 (duplicate binding in scope).

**Example 3.6.10.1 (Unified namespace):**

```cursive
record Point { x: f64, y: f64 }

// let Point = 10                     // error[E06-300]: `Point` already bound as type

{
    shadow let Point = 5               // OK: `shadow` explicitly overrides type binding
    println("{}", Point)                // Refers to value 5, not type
}
// Type binding `Point` visible again after block
```

#### §3.6.11 Category Mismatches [basic.binding.mismatch]

[22] Using a binding in the wrong category context produces specific diagnostics:

- Type binding in value context → E08-211 ("expected value, found type")
- Value binding in type context → E07-301 ("expected type, found value")
- Module binding in value/type context → E06-402 ("module binding used incorrectly")
- Predicate/contract binding in value context → E10-401 ("predicate used as value")

[23] The compiler detects category mismatches after lookup succeeds but before semantic validation, providing clear error messages that identify the found category and expected category.

#### §3.6.12 Integration with Name Resolution [basic.binding.integration]

[24] Binding categories integrate with name resolution (Clause 6) as follows:

- **Lookup phase**: Name lookup identifies a binding without regard to category
- **Validation phase**: After lookup, the binding's category is checked against the usage context
- **Error reporting**: Category mismatches produce specific diagnostics identifying both actual and expected categories

[25] This separation enables clear error messages and supports refactoring tools that track identifier uses across multiple categories.

#### §3.6.13 Conformance Requirements [basic.binding.requirements]

[26] Implementations shall:

1. Support all eight binding categories with the semantics specified in this subclause
2. Enforce unified namespace per scope: reject duplicate bindings across categories (E06-300)
3. Perform category validation after name lookup succeeds
4. Emit category-specific diagnostics for mismatches (E08-211, E07-301, E06-402, E10-401)
5. Maintain category information in reflection metadata and diagnostic payloads
6. Ensure labels remain procedure-scoped and grants remain compile-time-only
7. Support qualified access for module, type, and value bindings through `::` operator

[27] Implementations should provide IDE support surfacing binding categories during editing (e.g., "Point is a type" on hover).

---

**Previous**: §3.5 Alignment and Layout (Overview) | **Next**: Clause 4 — Modules
