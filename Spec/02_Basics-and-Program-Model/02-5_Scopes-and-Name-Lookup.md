# 2.5 Scopes and Name Lookup

## Overview

This section specifies the scoping rules and name resolution mechanisms of Cursive. It establishes the formal semantics of scope nesting, name binding, and the algorithms for resolving identifiers to their declarations.

Cursive employs lexical scoping: the scope of a name is determined by the program's syntactic structure, not by runtime execution. Name resolution follows a well-defined, predictable algorithm that searches through nested scopes in a specific order.

---

## 2.5.1 Scope Concept

### 2.5.1.1 Formal Definition

**Definition 2.28 (Scope)**: A scope is a lexical region of program text in which a declared name is visible and may be referenced. Formally, a scope is a partial function from identifiers to bindings:

```
Scope: Identifier ⇀ Binding
```

where a binding associates a name with its semantic properties.

**Definition 2.29 (Binding)**: A binding is a 5-tuple associating a name with its semantic properties:

```
Binding = (Name, Entity, Type, Visibility, Location)
```

where:
- **Name**: The identifier string
- **Entity**: The declared entity (variable, function, type, module, etc.)
- **Type**: The type of the entity (for typed entities)
- **Visibility**: The visibility level (`public`, `internal`, `private`, `protected`)
- **Location**: The source code position where the entity was declared

### 2.5.1.2 Scope Properties

**Requirement 2.5.1**: Scopes shall satisfy the following properties:

1. **Static determination**: Scopes are determined statically at compile time based on the syntactic structure of the program
2. **Namespace association**: Each scope has an associated lexical namespace (set of name bindings, see §2.5.9)
3. **Hierarchical nesting**: Scopes form a tree structure with parent-child relationships
4. **Lexical boundaries**: Scope boundaries are explicit and correspond to syntactic constructs (braces `{}`, function bodies, module boundaries)

### 2.5.1.3 Scope as a Set-Theoretic Structure

Let **Scopes** denote the set of all scopes in a program. For any scope S, we define:

- **names(S)** = {N | (N, E, T, V, L) ∈ S} - the set of names bound in S
- **parent(S)** - the immediately enclosing scope of S (undefined for module scope)
- **children(S)** - the set of immediately nested scopes within S
- **ancestors(S)** - the transitive closure of parent(S) (chain of enclosing scopes)

**Well-formedness constraint**:

```
[Scope-Well-Formed]
For all scopes S in a well-formed program:
  1. names(S) contains no duplicate identifiers
  2. parent(S) is defined except for module-level scope
  3. The parent relation forms a tree (no cycles)
────────────────────────────────────────────────────
Scope structure is well-formed
```

---

## 2.5.2 Lexical Scoping

### 2.5.2.1 Lexical Scoping Principle

**Normative Statement 2.5.1**: Cursive uses lexical scoping. The scope of a name is determined by the program's syntactic structure, not by runtime execution order or dynamic context.

**Formal property**: For any name N used at program location L, the binding for N is determined by:
1. The syntactic scope S containing L
2. The static scope chain ancestors(S)

**Not influenced by**:
- Runtime call stack
- Dynamic variable values
- Order of function calls
- Thread of execution

### 2.5.2.2 Lexical Scoping Rules

**Requirement 2.5.2**: The following rules govern lexical scoping:

1. **Nested scope access**: A nested scope can access names declared in outer scopes
2. **Outer scope isolation**: An outer scope cannot access names declared in inner scopes
3. **Syntactic boundaries**: Scope boundaries are determined by syntactic constructs:
   - Braces `{ }` for block scopes
   - Function bodies for function scopes
   - File boundaries for module scopes

**Visibility rule**:

```
[Lexical-Visibility]
Name N declared in scope S_outer
Reference to N at location L in scope S_inner
S_inner is S_outer or S_outer ∈ ancestors(S_inner)
No shadowing of N between S_inner and S_outer
────────────────────────────────────────────────────
N is visible at location L
```

---

## 2.5.3 Scope Nesting

### 2.5.3.1 Scope Hierarchy

Cursive defines three primary scope levels, ordered from outermost to innermost:

1. **Module scope**: Top-level declarations in a compilation unit
2. **Function scope**: Special entities visible throughout a function body (labels, parameters)
3. **Block scope**: Declarations within a block `{ }` or other local construct

### 2.5.3.2 Block Scope

**Definition 2.30 (Block Scope)**: A block `{ ... }` introduces a new lexical scope containing all declarations made within the block.

**Block scope properties**:

1. **Visibility range**: Names declared within a block are visible from the point of declaration to the end of the block
2. **Inheritance**: Nested blocks inherit outer scope bindings (subject to shadowing rules)
3. **Isolation**: Block bindings are not visible outside the block
4. **Storage duration**: Block-scoped variables have automatic storage duration (stack-allocated, freed at block end)

**Example (informative)**:

```cursive
{
    let x = 5  // x visible from here to end of outer block
    {
        let y = 10  // y visible in inner block only
        // Both x and y visible here
    }
    // Only x visible here; y is out of scope
}
// Neither x nor y visible here
```

### 2.5.3.3 Function Scope

**Definition 2.31 (Function Scope)**: Certain declarations have function-wide visibility, accessible from anywhere within the function body regardless of block nesting.

**Function-scoped entities**:

1. **Labels**: Loop labels and block labels (e.g., `'outer`)
2. **Parameters**: Function parameters (conceptually visible in the function's outermost local scope)

**Label visibility rule**:

```
[Function-Scope-Label]
Label L declared in function F at any block nesting level
Reference to L at any location within F's body
────────────────────────────────────────────────────
L is visible at the reference location
```

**Example (informative)**:

```cursive
function process() {
    'outer: loop {
        {
            {
                break 'outer  // ✅ OK: 'outer has function-wide visibility
            }
        }
    }
}
```

### 2.5.3.4 Module Scope

**Definition 2.32 (Module Scope)**: The module scope contains all top-level declarations in a compilation unit.

**Module scope properties**:

1. **Global visibility**: Module-level declarations are visible throughout the module
2. **Forward references**: Two-phase compilation enables references to declarations appearing later in the source file (§2.4.3.1)
3. **Cross-module access**: Visibility modifiers control accessibility from other modules (§2.4.2.3)
4. **Static storage**: Module-level variables have static storage duration

**Forward reference rule**:

```
[Module-Forward-Reference]
Declaration D₁ at location L₁ in module M
Declaration D₂ at location L₂ in module M
L₂ > L₁ (D₂ appears later in source)
D₁ references name defined by D₂
────────────────────────────────────────────────────
Reference is valid (forward reference permitted)
```

### 2.5.3.5 Formal Nesting Relation

**Definition 2.33 (Scope Nesting)**: Scope S_inner is nested within scope S_outer (denoted S_inner ⊂ S_outer) if:
- S_outer = parent(S_inner), or
- S_outer ∈ ancestors(S_inner)

**Transitive nesting**:

```
[Scope-Nesting-Transitive]
S₁ ⊂ S₂
S₂ ⊂ S₃
────────────────────
S₁ ⊂ S₃
```

**Name lookup along nesting chain**:

```
[Scope-Nested-Lookup]
S_inner ⊂ S_outer
Name N looked up in S_inner
N ∉ names(S_inner)
N ∈ names(S_outer)
No intermediate scope shadows N
────────────────────────────────────────
Name resolution continues to S_outer
N resolves to binding in S_outer
```

---

## 2.5.4 Name Binding

### 2.5.4.1 Binding Creation

**Definition 2.34 (Name Binding Creation)**: A declaration statement creates a binding in the current scope, associating the declared name with its entity, type, visibility, and source location.

**Binding creation rule**:

```
[Binding-Create]
Declaration D introduces name N with entity E, type T, visibility V at location L
Current scope is S
N ∉ names(S) (no existing binding for N in S)
────────────────────────────────────────────────────
Binding (N, E, T, V, L) added to scope S
```

**Uniqueness constraint**:

```
[Binding-Unique]
Name N already bound in scope S
Declaration attempts to bind N in S (without `shadow` keyword)
────────────────────────────────────────────────────
ERROR E2019: Duplicate declaration of N in scope S
```

### 2.5.4.2 Binding Properties

A binding associates the following properties with a name:

1. **Name** (identifier): The textual identifier (e.g., `x`, `Point`, `calculate`)
2. **Entity**: The kind of declared entity:
   - Variable (mutable or immutable)
   - Function or procedure
   - Type (record, enum, modal, alias)
   - Module or import
   - Label
3. **Type**: The type of the entity (undefined for modules and labels)
4. **Visibility**: The accessibility level (`public`, `internal`, `private`, `protected`)
5. **Location**: The source file and line:column position of the declaration

### 2.5.4.3 Binding Immutability

**Requirement 2.5.3**: Once a binding is created in a scope, it cannot be modified or rebound within that scope.

**Implication**: To "change" a binding, one must:
- Shadow it in a nested scope (using `shadow` keyword), or
- Mutate the bound variable's value (if the variable is mutable via `var`)

**Distinction**:
- **Rebinding** (not allowed): `let x = 5; let x = 10` in same scope
- **Mutation** (allowed for `var`): `var x = 5; x = 10`
- **Shadowing** (allowed with `shadow`): `let x = 5; { shadow let x = 10 }`

---

## 2.5.5 Name Lookup Algorithm

### 2.5.5.1 Unqualified Name Lookup (Five-Step Algorithm)

**Algorithm**: Unqualified name lookup resolves a single identifier to its binding by searching through scopes in a well-defined order.

```
Algorithm: resolve_unqualified_name(identifier: Identifier) -> Binding | Error

Input: identifier - the name to resolve
Output: the binding for identifier, or an error if not found

Step 1: Current Block Scope
  if identifier ∈ names(current_block_scope):
      return current_block_scope.lookup(identifier)

Step 2: Outward Through Lexical Scopes
  for scope in ancestors(current_block_scope) (innermost to outermost):
      if identifier ∈ names(scope):
          return scope.lookup(identifier)

Step 3: Module Scope
  if identifier ∈ names(module_scope):
      return module_scope.lookup(identifier)

Step 4: Imported Namespaces
  candidates := []
  for import_decl in module_scope.imports:
      if identifier ∈ import_decl.exported_names:
          candidates.append(import_decl.lookup(identifier))

  if |candidates| == 0:
      goto Step 5
  else if |candidates| == 1:
      return candidates[0]
  else:  // |candidates| > 1
      ERROR E2023: Ambiguous identifier '{identifier}' (imported from multiple modules)

Step 5: Universe Scope (Predeclared Identifiers)
  if identifier ∈ universe_scope:
      return universe_scope.lookup(identifier)

  // Not found in any scope
  ERROR E2024: Undefined identifier '{identifier}'
```

**Complexity**: O(d) where d is the lexical nesting depth (distance from current scope to module scope).

### 2.5.5.2 Resolution Order Guarantees

**Requirement 2.5.4**: The five-step lookup algorithm provides the following guarantees:

1. **Innermost precedence**: Names in inner scopes shadow names in outer scopes
2. **Local over imported**: Local declarations take precedence over imported names
3. **Imports over predeclared**: Imported names take precedence over predeclared identifiers
4. **Ambiguity detection**: Multiple imports of the same name are detected and reported

### 2.5.5.3 Ambiguity Detection

**Ambiguity rule**:

```
[Name-Ambiguous]
Identifier I referenced at location L
Multiple import declarations provide I:
  Import M₁ provides I -> Binding B₁
  Import M₂ provides I -> Binding B₂
  ...
  Import Mₙ provides I -> Binding Bₙ
n ≥ 2
────────────────────────────────────────────────────
ERROR E2023: Ambiguous identifier I (imported from M₁, M₂, ..., Mₙ)
```

**Resolution**: The user must qualify the reference (e.g., `M₁::I` or `M₂::I`) to disambiguate.

---

## 2.5.6 Qualified Name Lookup

### 2.5.6.1 Qualified Name Syntax

**Definition 2.35 (Qualified Name)**: A qualified name is an identifier prefixed by a module name or type name, separated by `::`:

```ebnf
QualifiedName ::= Identifier "::" Identifier
```

**Examples**:
- `io::file::File` (module qualification)
- `Status::Active` (type-associated item, enum variant)
- `math::sqrt` (module-qualified function)

### 2.5.6.2 Qualified Name Resolution Algorithm

**Algorithm**: Qualified name lookup resolves a name of the form `Prefix::Name` in two steps:

```
Algorithm: resolve_qualified_name(prefix: Identifier, name: Identifier) -> Binding | Error

Step 1: Resolve Prefix
  prefix_entity := resolve_unqualified_name(prefix)

  if prefix_entity is Error:
      ERROR E2025: Unknown module or type '{prefix}'

  if prefix_entity.entity is not (Module | Type):
      ERROR E2026: '{prefix}' is not a module or type (cannot qualify names)

Step 2: Lookup Name in Prefix's Namespace
  if prefix_entity.entity is Module:
      if name ∈ prefix_entity.exported_names:
          return prefix_entity.lookup(name)
      else if name ∈ prefix_entity.all_names:
          ERROR E2027: '{name}' exists in module '{prefix}' but is not public
      else:
          ERROR E2028: '{name}' not found in module '{prefix}'

  if prefix_entity.entity is Type:
      if name ∈ prefix_entity.associated_items:
          return prefix_entity.lookup(name)
      else:
          ERROR E2029: '{name}' is not an associated item of type '{prefix}'
```

### 2.5.6.3 Visibility Checking for Qualified Access

**Requirement 2.5.5**: When resolving a qualified name `Module::Name`, the implementation shall verify that:
1. The prefix `Module` is accessible from the current location
2. The name `Name` is exported (public) from the module

**Visibility enforcement rule**:

```
[Qualified-Visibility]
Qualified reference: M::N at location L
M is a module accessible from L
N ∈ names(M)
Visibility(N) is not `public`
────────────────────────────────────────────────────
ERROR E2027: N exists in M but is not public
```

---

## 2.5.7 Unqualified Name Lookup

### 2.5.7.1 Definition

**Definition 2.36 (Unqualified Name)**: An unqualified name is a single identifier without a prefix qualifier.

**Examples**: `x`, `calculate`, `Point`, `Buffer`

### 2.5.7.2 Resolution Process

**Requirement 2.5.6**: Unqualified names shall be resolved using the five-step algorithm specified in §2.5.5.1.

**Shadowing consideration**: If multiple scopes along the nesting chain define the same name, the innermost definition takes precedence (shadowing behavior, see §2.5.8).

---

## 2.5.8 Shadowing

### 2.5.8.1 Explicit Shadowing Requirement

**Normative Statement 2.5.2**: Shadowing of an outer scope binding must be explicit using the `shadow` keyword. Redeclaration of a name without the `shadow` keyword is an error.

**Rationale**: Explicit shadowing prevents accidental name collisions and makes programmer intent clear, reducing bugs and improving code clarity for both humans and LLMs.

### 2.5.8.2 Valid Shadowing

**Requirement 2.5.7**: A declaration may shadow an outer scope binding if:
1. The `shadow` keyword is used
2. The name exists in an enclosing scope
3. The declaration is in a nested scope (not the same scope as the shadowed name)

**Shadowing rule**:

```
[Shadow-Valid]
Outer scope S_outer contains binding: N -> (N, E₁, T₁, V₁, L₁)
Inner scope S_inner ⊂ S_outer
Declaration in S_inner: shadow let N : T₂ = expr
────────────────────────────────────────────────────
Binding N -> (N, E₂, T₂, V₂, L₂) added to S_inner
Within S_inner, N refers to new binding
Within S_outer (after S_inner ends), N refers to original binding
```

**Example (valid shadowing)**:

```cursive
let x = 5  // Outer binding: x -> 5 : i32
{
    shadow let x = 10  // ✅ OK: Inner binding shadows outer
    println(x)  // Prints 10 (inner binding)
}
println(x)  // Prints 5 (outer binding restored)
```

### 2.5.8.3 Invalid Redeclaration

**Requirement 2.5.8**: Redeclaration of a name in the same scope without the `shadow` keyword shall be rejected.

**Redeclaration error rule**:

```
[Shadow-Required]
Scope S contains binding: N -> (N, E₁, T₁, V₁, L₁)
Declaration in same scope S: let N : T₂ = expr (without `shadow` keyword)
────────────────────────────────────────────────────
ERROR E2019: Redeclaration of '{N}' in same scope
```

**Example (invalid redeclaration)**:

```cursive
let x = 5
let x = 10  // ❌ ERROR E2019: Redeclaration without shadow
```

### 2.5.8.4 Shadowing Across Scopes

**Nested shadowing**: Shadowing may occur through multiple nesting levels:

```cursive
let x = 1  // Outermost
{
    shadow let x = 2  // First shadow
    {
        shadow let x = 3  // Second shadow (shadows first shadow)
        println(x)  // Prints 3
    }
    println(x)  // Prints 2 (first shadow)
}
println(x)  // Prints 1 (original)
```

**Formal semantics**:

```
[Shadow-Nested]
Scopes S₁ ⊂ S₂ ⊂ S₃
S₃ binds N -> B₃
S₂ binds (shadow) N -> B₂ (shadows B₃)
S₁ binds (shadow) N -> B₁ (shadows B₂)
────────────────────────────────────────────────────
In S₁: N refers to B₁
In S₂: N refers to B₂
In S₃: N refers to B₃
```

### 2.5.8.5 Shadowing Across Namespace Categories

**Requirement 2.5.9**: Shadowing applies within the unified namespace. A shadowed name may be a type in the outer scope and a value in the inner scope, or vice versa.

**Example (type shadowing value)**:

```cursive
record Point { x: f64, y: f64 }  // Type: Point

function example() {
    shadow let Point = Point { x: 0.0, y: 0.0 }  // ✅ OK: Value shadows type
    // Within this scope, Point refers to the value, not the type
}
```

**Rationale**: Unified namespace (§2.5.9) ensures consistent shadowing behavior regardless of whether names refer to types or values.

---

## 2.5.9 Lexical Namespaces

### 2.5.9.1 Namespace Concept

**Definition 2.50 (Lexical Namespace)**: A lexical namespace is the set of all name bindings within a lexical scope. Formally, for a scope S:

```
Namespace(S) = {(name, binding) | name declared in scope S}
```

Each scope has exactly one associated namespace, and the namespace boundaries correspond to lexical scope boundaries.

### 2.5.9.2 The Unified Namespace Rule

**Normative Statement 2.5.3 (Single Namespace Rule)**: Types and values share a single namespace per scope. Within a single scope, a name cannot refer to both a type and a value.

**Implication**: If a name is declared as a type (e.g., `record Point`), it cannot be redeclared as a value (e.g., `let Point = ...`) in the same scope, and vice versa.

**Formal rules**:

```
[Namespace-Unified]
Scope S has namespace N
Name x declared as type T in N
Attempt to declare name x as value V in N (same scope)
────────────────────────────────────────────────────
ERROR E2038: Name 'x' conflicts with existing type declaration
```

```
[Namespace-Unified-Reverse]
Scope S has namespace N
Name x declared as value V in N
Attempt to declare name x as type T in N (same scope)
────────────────────────────────────────────────────
ERROR E2038: Name 'x' conflicts with existing value declaration
```

**Example (type-value conflict)**:

```cursive
record Point { x: i32, y: i32 }  // Point is a type in module namespace

let Point = compute_point()  // ❌ ERROR E2038: 'Point' already declared as type
```

**Example (different scopes - valid)**:

```cursive
function foo() {
    let x = 5  // x is a value in foo's namespace
}

function bar() {
    type x = i32  // ✅ OK: Different scope, independent namespace
}
```

### 2.5.9.3 Name Categories (Informative)

**Definition 2.51 (Name Category)**: A name category classifies names based on the kind of entity they represent, used for semantic analysis and type checking:

1. **Type names**: Records, enums, type aliases, modals, predicates
2. **Value names**: Variables, functions, procedures, constants
3. **Label names**: Loop labels and block labels (separate namespace with function scope)
4. **Module names**: Modules and import aliases

**Important**: Name categories are used for well-formedness checking and error diagnostics, but do not create separate namespaces. Name resolution operates on the single unified namespace per scope.

**Category checking** (post-resolution):

```
[Category-Check-Value]
Expression context expects value
Name n resolved to binding B
Category(B) = Type
────────────────────────────────────────────────────
ERROR: Expected value, found type 'n'
```

```
[Category-Check-Type]
Type context expects type
Name n resolved to binding B
Category(B) = Value
────────────────────────────────────────────────────
ERROR: Expected type, found value 'n'
```

### 2.5.9.4 Comparison with Separate Namespace Languages

**C++ and Rust** maintain separate namespaces for types and values:

```cpp
// C++ - ALLOWED
struct Point { int x, y; };  // Point in type namespace
int Point = 5;               // Point in value namespace
// Context determines which 'Point' is referenced
```

**Cursive** uses a unified namespace:

```cursive
// Cursive - ERROR
record Point { x: i32, y: i32 }  // Point in unified namespace
let Point = 5  // ❌ ERROR E2038: 'Point' already declared as type
```

| Language | Type/Value Namespaces | Resolution | Same Name for Type and Value? |
|----------|----------------------|------------|-------------------------------|
| **C/C++** | Separate | Context-dependent | ✅ Yes |
| **Rust** | Separate | Context-dependent | ✅ Yes |
| **Python** | Unified | Context-independent | ❌ No |
| **Cursive** | Unified | Context-independent | ❌ No |

### 2.5.9.5 Rationale for Unified Namespace

Cursive adopts a unified namespace model for:

1. **Simplified name resolution**: One namespace per scope means one lookup algorithm, no context tracking needed
2. **LLM-friendliness**: AI code generators benefit from unambiguous name semantics without context inference
3. **Reduced confusion**: Each name refers to exactly one entity per scope, eliminating context-dependent meaning changes

**Trade-off**: Separate namespaces allow convenient name reuse (e.g., `File` type and `File` constructor), but Cursive prioritizes clarity and simplicity over naming convenience.

---

## 2.5.10 Canonical Examples

### Example 1: Scope Nesting Hierarchy

The following example demonstrates all three scope levels and their nesting:

```cursive
// Module scope
let MODULE_CONST: i32 = 100

function outer(param: i32) {
    // Function scope: param visible throughout function
    let func_local = param * 2  // Block scope: func_local

    {
        let inner_local = 42  // Inner block scope: inner_local
        // Visible here: MODULE_CONST, param, func_local, inner_local
        println("{} {} {} {}", MODULE_CONST, param, func_local, inner_local)
    }

    // Visible here: MODULE_CONST, param, func_local
    // NOT visible: inner_local (out of scope)
    println("{} {} {}", MODULE_CONST, param, func_local)
}

// Visible here: MODULE_CONST
// NOT visible: param, func_local, inner_local
```

### Example 2: Five-Step Name Lookup

The following example demonstrates all five steps of the name lookup algorithm:

```cursive
// Step 3: Module scope
let MODULE_VAR = 10

// Step 4: Imports
use io::file::File

function example() {
    // Step 2: Enclosing function scope
    let func_var = 20

    {
        // Step 1: Current block scope
        let block_var = 30

        // Resolution demonstration:
        println(block_var)    // Step 1: Current block scope
        println(func_var)     // Step 2: Enclosing function scope
        println(MODULE_VAR)   // Step 3: Module scope
        println(File)         // Step 4: Imported namespace
        println(i32)          // Step 5: Universe scope (predeclared type)
    }
}
```

### Example 3: Qualified Name Resolution

```cursive
// Module: math.cursive
public function sqrt(x: f64): f64 { ... }
public function abs(x: i32): i32 { ... }

// Module: main.cursive
use math

function main(): i32 {
    let value = -5

    // Unqualified lookup (requires `use math`)
    let positive = abs(value)

    // Qualified lookup (explicit module prefix)
    let positive_qualified = math::abs(value)

    result 0
}
```

### Example 4: Valid and Invalid Shadowing

```cursive
function shadowing_examples() {
    let x = 5  // Outer binding

    // ✅ Valid: Explicit shadowing in nested scope
    {
        shadow let x = 10
        println("Inner x: {}", x)  // Prints 10
    }
    println("Outer x: {}", x)  // Prints 5

    // ❌ Invalid: Redeclaration in same scope
    // let x = 15  // ERROR E2019: Redeclaration without shadow

    // ✅ Valid: Nested shadowing
    {
        shadow let x = 20  // Shadows outer x (value 5)
        {
            shadow let x = 30  // Shadows middle x (value 20)
            println("Innermost x: {}", x)  // Prints 30
        }
        println("Middle x: {}", x)  // Prints 20
    }
    println("Outer x: {}", x)  // Prints 5

    // ✅ Valid: Shadowing can change type
    {
        shadow let x = "hello"  // String shadows i32
        println("String x: {}", x)  // Prints "hello"
    }
    println("Int x: {}", x)  // Prints 5
}
```

### Example 5: Ambiguous Import

```cursive
// Module: lib_a.cursive
public function process(): i32 { 1 }

// Module: lib_b.cursive
public function process(): i32 { 2 }

// Module: main.cursive
use lib_a
use lib_b

function main(): i32 {
    // ❌ ERROR E2023: Ambiguous identifier 'process'
    // let result = process()

    // ✅ Correct: Use qualified names
    let result_a = lib_a::process()  // Returns 1
    let result_b = lib_b::process()  // Returns 2

    result 0
}
```

### Example 6: Unified Namespace in Action

```cursive
// Module scope namespace
record Point { x: i32, y: i32 }  // Point (type) in module namespace

// ❌ ERROR: Cannot have both type and value named Point in same namespace
// let Point = Point { x: 0, y: 0 }  // ERROR E2038

function demonstrate_unified_namespace() {
    // Function scope namespace has independent Point type
    record Point { value: string }  // ✅ OK: Different scope

    {
        // Can shadow type with value explicitly
        shadow let Point = Point { value: "data" }  // ✅ OK: Explicit shadow
        println("Value: {}", Point.value)
    }

    // Point refers to the local type again
    let p: Point = Point { value: "test" }
}

function scope_independence() {
    // Each function has independent namespace
    let Counter = 42  // ✅ OK: Different scope from any other Counter
    type Data = i32   // ✅ OK: No conflict with Counter (different names)
}
```

---

## 2.5.11 Error Codes

This section defines error codes related to scoping and name lookup:

- **E2019**: Duplicate declaration of name in same scope
- **E2023**: Ambiguous identifier (imported from multiple modules)
- **E2024**: Undefined identifier (not found in any scope)
- **E2025**: Unknown module or type (qualified name prefix not found)
- **E2026**: Prefix is not a module or type (cannot qualify names)
- **E2027**: Name exists in module but is not public
- **E2028**: Name not found in module
- **E2029**: Name is not an associated item of type
- **E2038**: Name conflict in unified namespace (type/value collision in same scope)

---

## 2.5.12 Conformance Requirements

A conforming implementation shall satisfy the following requirements with respect to scopes and name lookup:

1. **Lexical scoping** (§2.5.2): The implementation shall use lexical scoping, where scope is determined by syntactic structure.

2. **Scope hierarchy** (§2.5.3): The implementation shall support block scope, function scope, and module scope with proper nesting relationships.

3. **Binding uniqueness** (§2.5.4): The implementation shall enforce unique name bindings within each scope (error E2019).

4. **Five-step lookup** (§2.5.5): The implementation shall resolve unqualified names using the five-step algorithm.

5. **Qualified lookup** (§2.5.6): The implementation shall resolve qualified names using the two-step algorithm.

6. **Visibility enforcement** (§2.5.6.3): The implementation shall enforce visibility constraints for qualified access (error E2027).

7. **Explicit shadowing** (§2.5.8): The implementation shall require the `shadow` keyword for shadowing outer bindings.

8. **Ambiguity detection** (§2.5.5.3): The implementation shall detect and report ambiguous imports (error E2023).

9. **Unified namespace** (§2.5.9): The implementation shall enforce a single namespace per scope for both types and values (error E2038).

---

## 2.5.13 Notes and Examples

### Informative Note 1: Dynamic Scoping Contrast

Some languages (e.g., early Lisp dialects, Emacs Lisp) use dynamic scoping, where name resolution depends on the runtime call stack. Cursive explicitly rejects dynamic scoping in favor of lexical scoping for:
- Predictability: Names resolve the same way regardless of execution path
- Tool support: IDEs and analyzers can resolve names without runtime information
- Performance: No runtime scope chain traversal required

### Informative Note 2: Universe Scope

The universe scope (Step 5 of name lookup) contains predeclared identifiers provided by the language:
- Primitive types: `i32`, `u64`, `f64`, `bool`, `char`, `string`, etc.
- Built-in pointer types: `Ptr` (modal pointer type)
- Special constants: `true`, `false`

These identifiers are always available without import but can be shadowed by user declarations.

### Informative Note 3: Unified Lexical Namespaces

Cursive uses a unified namespace model where types and values share the same namespace per scope (§2.5.9). This design choice differs from languages like C++, Rust, and Haskell that maintain separate type and value namespaces.

**Implications**:
- **Single lookup algorithm**: Name resolution is context-independent, simplifying both compiler implementation and human understanding
- **Explicit conflicts**: `record Point` and `let Point` in the same scope produce ERROR E2038, preventing subtle bugs
- **Shadowing flexibility**: Different scopes can independently use the same names for types or values
- **LLM-friendliness**: AI code generators benefit from unambiguous name semantics without requiring syntactic context inference

The trade-off is that convenient patterns like having both `Config` (type) and `Config` (constructor function) in the same scope are disallowed, but this is consistent with Cursive's design philosophy of favoring explicitness and clarity.

---

**End of Section 2.5: Scopes and Name Lookup**
