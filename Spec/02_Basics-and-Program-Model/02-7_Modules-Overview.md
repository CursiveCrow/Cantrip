# 2.7 Modules Overview

## Overview

This section specifies the module system of Cursive. A module is the primary unit of code organization and namespace management in Cursive programs. Cursive employs a file-based module system where each source file implicitly defines a module, and the filesystem structure determines the module hierarchy.

Unlike languages that require explicit `module` declarations, Cursive infers module identity and hierarchy directly from file paths, reducing boilerplate and making module structure immediately visible from the project directory layout.

---

## 2.7.1 Module Concept

### 2.7.1.1 Formal Definition

**Definition 2.43 (Module)**: A module is a namespace unit defined by a single source file. The module's fully-qualified path is determined by the file's location relative to the source root.

Formally, a module is a 3-tuple:

```
Module = (Path, Exports, Declarations)
```

where:
- **Path**: A sequence of identifiers forming the module's fully-qualified name (e.g., `math.geometry`)
- **Exports**: The subset of declarations with `public` visibility that are accessible from other modules
- **Declarations**: The complete set of items (functions, types, variables, etc.) defined in the module

### 2.7.1.2 File-Based Module System

**Normative Statement 2.7.1**: Cursive uses a file-based module system. There is no `module` keyword. Module structure and identity are determined by the filesystem structure.

**Key principle**: One file = one module.

**Module identity determination**:

```
[Module-Identity]
Source file at path: <source_root>/math/geometry.cursive
Source root: <source_root>
Relative path: math/geometry.cursive
────────────────────────────────────────────────────
Module path: math.geometry
```

**Rationale**: File-based modules provide:
- **Zero boilerplate**: No need for `module` declarations
- **Visual clarity**: Directory structure mirrors module hierarchy
- **Tool-friendliness**: Build tools, IDEs, and analyzers can easily discover modules
- **LLM-friendliness**: Module structure is immediately evident from file paths

### 2.7.1.3 Module Properties

**Requirement 2.7.1**: Every module shall have the following properties:

1. **Unique path**: Each module has a unique fully-qualified path derived from its file path
2. **Encapsulation**: Modules provide namespace isolation and encapsulation boundaries
3. **Explicit exports**: Only items declared `public` are exported (visible to other modules)
4. **Explicit imports**: Other modules must explicitly import or use items from the module

**Module namespace rule**:

```
[Module-Namespace]
Module M at path P
M contains declarations D = {d₁, d₂, ..., dₙ}
Exports(M) = {d ∈ D | visibility(d) = public}
────────────────────────────────────────────────────
Other modules may access items in Exports(M) via qualified names: M::item
```

---

## 2.7.2 Module Declaration (Not Applicable)

**Normative Statement 2.7.2**: Module declarations are implicit in Cursive. The filesystem structure determines the module hierarchy. No explicit `module` statements are required or permitted in source files.

**Contrast with other languages**:
- Some languages (e.g., Java, Python) require explicit module/package declarations
- Cursive omits these declarations, inferring module identity from file paths

**Implication**: There is no `module` keyword in Cursive.

---

## 2.7.3 Import and Export Mechanisms

### 2.7.3.1 Import Syntax

**Definition 2.44 (Import)**: An import declaration brings a module into scope, allowing references to the module's exported items via qualified names.

**Syntax**:

```ebnf
ImportDeclaration ::= "import" ModulePath Separator
ModulePath        ::= Identifier ("." Identifier)*
```

**Example**:

```cursive
import math.geometry
// Module math.geometry is now accessible via qualified names:
// math.geometry::area_of_circle
```

**Import semantics**:

```
[Import-Module]
Declaration: import M
Module M exists at file path corresponding to M
────────────────────────────────────────────────────
M is brought into scope
Items exported from M are accessible via qualified names: M::item
```

### 2.7.3.2 Use Syntax

**Definition 2.45 (Use Declaration)**: A use declaration brings specific items from a module into the current scope, allowing unqualified references to those items.

**Syntax**:

```ebnf
UseDeclaration ::= "use" UsePath Separator
UsePath        ::= ModulePath "::" UseTarget
UseTarget      ::= Identifier                        // Single item
                 | Identifier "as" Identifier        // Renamed item
                 | "{" UseItemList "}"               // Multiple items
                 | "*"                               // Wildcard (all public items)
UseItemList    ::= UseItem ("," UseItem)*
UseItem        ::= Identifier ("as" Identifier)?
```

**Use forms**:

1. **Single item**:
   ```cursive
   use math.geometry::area_of_circle
   // Direct access: area_of_circle(5.0)
   ```

2. **Multiple items**:
   ```cursive
   use math.geometry::{area_of_circle, circumference}
   // Direct access: area_of_circle(5.0), circumference(5.0)
   ```

3. **Renamed item**:
   ```cursive
   use math.geometry::area_of_circle as circle_area
   // Direct access: circle_area(5.0)
   ```

4. **Wildcard (all public items)**:
   ```cursive
   use math.geometry::*
   // All public items from math.geometry imported
   ```

### 2.7.3.3 Use Semantics

**Single item use**:

```
[Use-Item]
Declaration: use M::item
Module M exports item (visibility = public)
────────────────────────────────────────────────────
item is imported into current scope
Unqualified references to item resolve to M::item
```

**Wildcard use**:

```
[Use-Wildcard]
Declaration: use M::*
Module M exports items I = {i₁, i₂, ..., iₙ}
No naming conflicts with existing bindings in current scope
────────────────────────────────────────────────────
All items in I are imported into current scope
Unqualified references to any iₖ resolve to M::iₖ
```

**Ambiguous import**:

```
[Import-Ambiguous]
Declaration: use M₁::item
Declaration: use M₂::item
Unqualified reference to item at location L
────────────────────────────────────────────────────
ERROR E2023: Ambiguous identifier 'item' (imported from M₁ and M₂)
User must use qualified names: M₁::item or M₂::item
```

### 2.7.3.4 Export Control

**Normative Statement 2.7.3**: Export control is determined by visibility modifiers. Only items declared `public` are exported from a module.

**Export rule**:

```
[Module-Exports]
Module M contains declaration D
visibility(D) = public
────────────────────────────────────────────────────
D ∈ Exports(M)
D is accessible from other modules via qualified name: M::D
```

**Non-exported items**:

```
[Module-Non-Exported]
Module M contains declaration D
visibility(D) ∈ {internal, private}
────────────────────────────────────────────────────
D ∉ Exports(M)
D is not accessible from other modules
```

---

## 2.7.4 Module Visibility and Boundaries

### 2.7.4.1 Visibility Levels and Module Access

**Requirement 2.7.2**: Module access is governed by visibility modifiers as follows:

| Visibility | Accessible From |
|------------|-----------------|
| `public` | All modules (exported) |
| `internal` | Same package only (not exported to external packages) |
| `private` | Same module only (not exported at all) |

**Module boundary**: The source file boundary defines the module boundary. Items in different modules cannot access each other's `private` items.

### 2.7.4.2 Cross-Module Access

**Public items (exported)**:

```cursive
// Module: math.cursive
public function sqrt(x: f64): f64 { ... }

// Module: main.cursive
import math
function main(): i32 {
    let root = math::sqrt(25.0)  // ✅ OK: public item
    result 0
}
```

**Private items (not exported)**:

```cursive
// Module: math.cursive
private function helper(x: f64): f64 { ... }

// Module: main.cursive
import math
function main(): i32 {
    // let result = math::helper(5.0)  // ❌ ERROR: private item
    result 0
}
```

---

## 2.7.5 Module Resolution Algorithm

### 2.7.5.1 Module Path to File Path Mapping

**Algorithm**: The compiler resolves a module path to a file path using the following steps:

```
Algorithm: resolve_module_path(module_path: string) -> FilePath | Error

Input: module_path - dot-separated module path (e.g., "math.geometry")
Output: absolute file path to the module's source file, or error if not found

Step 1: Split module path by '.' delimiter
  components := split(module_path, '.')
  Example: "math.geometry" -> ["math", "geometry"]

Step 2: Join components with OS path separator
  relative_path := join(components, OS_PATH_SEPARATOR)
  Unix example:    "math/geometry"
  Windows example: "math\geometry"

Step 3: Prepend source root directory
  source_root := get_source_root()  // Typically "src/" by convention
  file_path := source_root + relative_path
  Example: "src/math/geometry"

Step 4: Append file extension
  file_path := file_path + ".cursive"
  Example: "src/math/geometry.cursive"

Step 5: Resolve to absolute path and check existence
  absolute_path := resolve_absolute(file_path)
  if not file_exists(absolute_path):
      ERROR E2035: Module not found: module_path

Step 6: Return absolute file path
  return absolute_path
```

**Complexity**: O(n) where n is the number of components in the module path.

### 2.7.5.2 Module Not Found Error

**Error rule**:

```
[Module-Not-Found]
Import or use declaration references module M
resolve_module_path(M) fails to locate file
────────────────────────────────────────────────────
ERROR E2035: Module not found: M
```

**Diagnostic requirement**:

**Requirement 2.7.3**: When emitting error E2035, the implementation should provide:
1. The module path that could not be resolved
2. The file path that was searched (if computable)
3. Suggestions for similar module names (if available)

---

## 2.7.6 Canonical Example: File-Based Module System

### Project Structure

The following project structure demonstrates the file-based module system:

```
myproject/
├── Cursive.toml              # Project manifest
└── src/
    ├── main.cursive           # Module: main (entry point)
    ├── utils.cursive          # Module: utils
    ├── math/
    │   ├── geometry.cursive   # Module: math.geometry
    │   └── algebra.cursive    # Module: math.algebra
    └── data/
        └── structures.cursive # Module: data.structures
```

**Module path derivation**:
- `src/main.cursive` -> Module: `main`
- `src/utils.cursive` -> Module: `utils`
- `src/math/geometry.cursive` -> Module: `math.geometry`
- `src/math/algebra.cursive` -> Module: `math.algebra`
- `src/data/structures.cursive` -> Module: `data.structures`

### Source Code

#### Module: math.geometry

```cursive
// ========== src/math/geometry.cursive ==========
// Module: math.geometry (inferred from file path)

public function area_of_circle(radius: f64): f64 {
    result 3.14159 * radius * radius
}

public function circumference(radius: f64): f64 {
    result 2.0 * 3.14159 * radius
}

internal function helper(x: f64): f64 {
    result x * 2.0  // Not exported (internal visibility)
}
```

#### Module: math.algebra

```cursive
// ========== src/math/algebra.cursive ==========
// Module: math.algebra

public function solve_linear(a: f64, b: f64): f64 {
    result -b / a
}
```

#### Module: utils

```cursive
// ========== src/utils.cursive ==========
// Module: utils

// Import specific item from math.geometry
use math.geometry::area_of_circle

public function print_circle_area(radius: f64) {
    let area = area_of_circle(radius)  // Unqualified access
    println("Circle area: {}", area)
}
```

#### Module: main (Entry Point)

```cursive
// ========== src/main.cursive ==========
// Module: main (entry point)

// Import entire modules
import math.geometry
import math.algebra

// Use specific items for unqualified access
use utils::print_circle_area

function main(): i32 {
    // Qualified access to imported modules
    let area = math.geometry::area_of_circle(5.0)
    let circ = math.geometry::circumference(5.0)

    println("Area: {}, Circumference: {}", area, circ)

    // Unqualified access (via use declaration)
    print_circle_area(10.0)

    // Qualified access to algebra module
    let solution = math.algebra::solve_linear(2.0, -4.0)
    println("Solution: {}", solution)

    result 0
}
```

**Module resolution examples**:
- `import math.geometry` -> Resolves to `src/math/geometry.cursive`
- `import math.algebra` -> Resolves to `src/math/algebra.cursive`
- `use utils::print_circle_area` -> Resolves to `src/utils.cursive`, exports `print_circle_area`

---

## 2.7.7 Error Codes

This section defines error codes related to modules:

- **E2023**: Ambiguous identifier (imported from multiple modules) - defined in §2.5.10
- **E2035**: Module not found (module path does not resolve to a source file)
- **E2036**: Item not found in module (qualified name references non-existent item)
- **E2037**: Item not exported (item exists but is not public)

---

## 2.7.8 Conformance Requirements

A conforming implementation shall satisfy the following requirements with respect to modules:

1. **File-based modules** (§2.7.1): The implementation shall infer module identity from file paths without requiring `module` declarations.

2. **Module resolution** (§2.7.5): The implementation shall resolve module paths to file paths using the specified algorithm.

3. **Import and use** (§2.7.3): The implementation shall support `import` and `use` declarations with the specified syntax and semantics.

4. **Export control** (§2.7.3.4): The implementation shall export only `public` items from modules.

5. **Visibility enforcement** (§2.7.4): The implementation shall enforce visibility constraints for module access.

6. **Ambiguity detection** (§2.7.3.3): The implementation shall detect and report ambiguous imports (error E2023).

7. **Module not found** (§2.7.5.2): The implementation shall report error E2035 when a module path cannot be resolved.

---

## 2.7.9 Notes and Examples

### Informative Note 1: No Module Keyword

Cursive deliberately omits a `module` keyword. This design choice:
- Reduces boilerplate (no need to declare the module name at the top of every file)
- Eliminates inconsistency (file path and module declaration cannot diverge)
- Improves refactoring (moving a file automatically updates its module path)
- Simplifies tooling (module discovery is a filesystem operation)

### Informative Note 2: Comparison with Other Languages

| Language | Module System |
|----------|---------------|
| **Rust** | File-based with explicit `mod` declarations for submodules |
| **Python** | File-based with optional `__init__.py` and explicit imports |
| **Java** | Explicit `package` declarations at top of each file |
| **Go** | Directory-based with `package` declarations |
| **Cursive** | Purely file-based, no `module` or `package` keyword |

### Informative Note 3: Source Root Convention

The source root is typically `src/` by convention, but implementations may support configuration via project manifests (e.g., `Cursive.toml`). The source root serves as the base directory for module path resolution.

### Informative Note 4: Module Hierarchies

Module hierarchies are flat in the namespace, even though the filesystem is hierarchical:
- `math.geometry` is a distinct module, not a submodule of `math`
- There is no implicit relationship between `math` and `math.geometry`
- To expose items from `math.geometry` through `math`, `math.cursive` must explicitly re-export them

### Informative Note 5: Circular Dependencies

Module imports may form cycles (e.g., `A` imports `B`, `B` imports `A`). Cursive's two-phase compilation (§2.0) allows forward references and circular dependencies at the module level. Detailed circular dependency resolution is specified in §[REF_TBD].

---

## 2.7.10 Cross-References

For more detailed module system specifications, see:
- **Module visibility and packages**: §[REF_TBD]
- **Circular dependency resolution**: §[REF_TBD]
- **Re-exports**: §[REF_TBD]
- **Module-level initialization**: §2.4.2.4

---

**End of Section 2.7: Modules Overview**
