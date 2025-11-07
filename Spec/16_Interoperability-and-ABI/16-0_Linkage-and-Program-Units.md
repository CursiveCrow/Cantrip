# 2.6 Linkage and Program Units

## Overview

This section specifies the linkage model for Cursive programs. Linkage determines whether a symbol (the name of an entity) is visible across compilation unit boundaries at the binary and linking phases of program construction.

Linkage is distinct from but related to visibility: visibility controls compile-time name resolution and access control, while linkage controls symbol table entries in the compiled binary and cross-unit linking behavior.

---

## 2.6.1 Linkage Concept

### 2.6.1.1 Formal Definition

**Definition 2.37 (Linkage)**: Linkage is the property of a declared entity that determines whether its symbol is visible across compilation unit boundaries in the compiled binary.

Formally, linkage is a function:

```
linkage: Entity -> {External, Internal, None}
```

where:
- **External**: Symbol exported in binary, visible across compilation units
- **Internal**: Symbol may appear in binary as local symbol, not visible across units
- **None**: No symbol in binary

### 2.6.1.2 Linkage vs Visibility

**Definition 2.38 (Linkage-Visibility Distinction)**:

- **Visibility**: A compile-time access control mechanism determining which declarations are accessible during name resolution (§2.5)
- **Linkage**: A link-time property determining which symbols appear in the binary and whether they are exported

**Relationship**:

```
[Linkage-Visibility-Relationship]
Entity E has visibility V and linkage L
Visibility affects compile-time name resolution
Linkage affects binary symbol table
────────────────────────────────────────────────────
V and L are independent but related properties
```

**Key insight**: An entity may be visible at compile time (can be named and accessed) but have no linkage (not in the binary symbol table). Conversely, an entity with external linkage must have sufficient visibility to be accessed from other compilation units.

### 2.6.1.3 Linkage Categories

**Requirement 2.6.1**: Every declared entity has exactly one of three linkage categories:

1. **External linkage**: Symbol is exported and visible across compilation units
2. **Internal linkage**: Symbol is local to a compilation unit or package
3. **No linkage**: No symbol in the binary

**Linkage determination rule**:

```
[Linkage-Determination]
Entity E declared with visibility V
If V = public:
    linkage(E) = External (by default, unless overridden by attributes)
If V = internal:
    linkage(E) = Internal
If V = private:
    linkage(E) = None
If E is local variable:
    linkage(E) = None (regardless of visibility)
────────────────────────────────────────────────────
Linkage determined by visibility and entity kind
```

---

## 2.6.2 External Linkage

### 2.6.2.1 Definition

**Definition 2.39 (External Linkage)**: An entity has external linkage if its symbol is exported in the compiled binary and can be referenced from other compilation units.

**Default rule**:

> **Normative Statement 2.6.1**: Items declared with `public` visibility have external linkage by default.

### 2.6.2.2 Properties of External Linkage

**Requirement 2.6.2**: Entities with external linkage shall satisfy the following properties:

1. **Symbol export**: The entity's symbol is exported in the binary symbol table
2. **Cross-unit access**: The entity may be referenced from other compilation units
3. **Name mangling**: Implementation may apply name mangling (implementation-defined), unless overridden by attributes
4. **One Definition Rule**: The entity is subject to the One Definition Rule (§2.6.5)

**Symbol visibility rule**:

```
[External-Linkage-Visible]
Entity E has external linkage
Compilation unit CU₁ defines E
Compilation unit CU₂ imports module containing E
────────────────────────────────────────────────────
CU₂ may reference E via qualified name
Linker resolves reference to definition in CU₁
```

### 2.6.2.3 External Linkage Example

```cursive
// File: math.cursive
public procedure add(x: i32, y: i32): i32
    {| |- true => true |}
{
    result x + y
}
// Symbol 'add' has external linkage
// Exported in binary as: _Cursive_math_add (example mangled name)

// File: main.cursive
import math

procedure main(): i32
    {| |- true => true |}
{
    let sum = math::add(5, 3)  // Links to external symbol _Cursive_math_add
    result 0
}
```

### 2.6.2.4 Name Mangling

**Definition 2.40 (Name Mangling)**: Name mangling is the process of encoding additional information (module path, parameter types, etc.) into a symbol name to support overloading and avoid naming conflicts.

**Requirement 2.6.3**: For entities with external linkage, the implementation may apply name mangling. The mangling scheme is implementation-defined.

**Mangling bypass**: Attributes such as `[[no_mangle]]` and `[[extern("C")]]` (§2.6.6) disable name mangling for FFI interoperability.

---

## 2.6.3 Internal Linkage

### 2.6.3.1 Definition

**Definition 2.41 (Internal Linkage)**: An entity has internal linkage if its symbol is local to a compilation unit or package and is not visible outside that unit/package.

**Default rule**:

> **Normative Statement 2.6.2**: Items declared with `internal` visibility (the default for top-level declarations) have internal linkage.

### 2.6.3.2 Properties of Internal Linkage

**Requirement 2.6.4**: Entities with internal linkage shall satisfy the following properties:

1. **Local symbol**: The symbol may appear in the binary as a local (non-exported) symbol
2. **Unit isolation**: The entity is not accessible from other compilation units outside the package
3. **Optimization opportunity**: The compiler may inline, eliminate, or otherwise optimize entities with internal linkage
4. **ODR exemption**: Not subject to the One Definition Rule across compilation units (may have multiple definitions in different units)

**Internal linkage isolation**:

```
[Internal-Linkage-Isolated]
Entity E has internal linkage
Compilation unit CU₁ defines E
Compilation unit CU₂ attempts to reference E
CU₂ not in same package as CU₁
────────────────────────────────────────────────────
Reference to E fails at compile time (visibility error)
No link-time reference possible
```

### 2.6.3.3 Internal Linkage Example

```cursive
// File: math.cursive
internal procedure helper(x: i32): i32
    {| |- true => true |}
{
    result x * 2
}
// Symbol 'helper' has internal linkage
// Not exported; may be inlined or eliminated

public procedure compute(x: i32): i32
    {| |- true => true |}
{
    result helper(x) + 1  // OK: same compilation unit
}

// File: main.cursive
import math

procedure main(): i32
    {| |- true => true |}
{
    // CANNOT access math::helper - internal linkage
    // let x = math::helper(5)  // ❌ Compile error: not visible

    let y = math::compute(5)  // ✅ OK: external linkage
    result 0
}
```

---

## 2.6.4 No Linkage

### 2.6.4.1 Definition

**Definition 2.42 (No Linkage)**: An entity has no linkage if it does not produce a symbol in the binary symbol table.

**Default rule**:

> **Normative Statement 2.6.3**: Private items and local variables have no linkage.

### 2.6.4.2 Properties of No Linkage

**Requirement 2.6.5**: Entities with no linkage shall satisfy the following properties:

1. **No symbol**: No symbol appears in the binary symbol table
2. **Complete isolation**: The entity is not accessible outside its declaring scope
3. **Aggressive optimization**: The compiler may eliminate the entity entirely if unused
4. **Automatic storage**: Local variables with no linkage have automatic storage duration (stack-allocated)

**No linkage entities**:
- Local variables (all variables declared within procedures or blocks)
- `private` procedures, types, and constants
- Procedure parameters
- Temporary values

### 2.6.4.3 No Linkage Example

```cursive
private procedure utility()
    {| |- true => true |}
{
    println("Private utility")
}
// No symbol in binary; completely local to this file

procedure example()
    {| |- true => true |}
{
    let local_var = 42  // No linkage; automatic storage
    // Compiler may eliminate if unused
}

procedure main(): i32
    {| |- true => true |}
{
    let x = 10  // No linkage
    {
        let y = 20  // No linkage; destroyed at block end
    }
    // y no longer exists

    result 0
}
```

---

## 2.6.5 One Definition Rule (ODR)

### 2.6.5.1 ODR for Types

**Requirement 2.6.6 (ODR for Types)**: A type with external linkage shall have exactly one definition across the entire program.

**Type uniqueness rule**:

```
[ODR-Type-Unique]
Program P contains type definitions D₁, D₂
Both D₁ and D₂ define type T with same fully-qualified name
D₁ ≠ D₂ (structurally distinct definitions)
────────────────────────────────────────────────────
ERROR E2030: Multiple conflicting definitions of type T
(detected at link time or during cross-unit semantic analysis)
```

**Structural equivalence**: Two type definitions are considered the same if they are structurally identical (same fields, same types, same order).

**Example (ODR violation)**:

```cursive
// File: types_a.cursive
public record Point {
    x: f64
    y: f64
}

// File: types_b.cursive
public record Point {  // ❌ Same name
    x: f32  // Different structure
    y: f32
}

// ❌ ERROR E2030: Multiple definitions of Point with different structure
```

### 2.6.5.2 ODR for Procedures

**Requirement 2.6.7 (ODR for Procedures)**: A procedure with external linkage shall have exactly one definition. Multiple declarations are permitted, but only one definition.

**Procedure definition rule**:

```
[ODR-Procedure-Single-Definition]
Program P contains procedure definitions D₁, D₂
Both D₁ and D₂ define procedure f with same signature
Both D₁ and D₂ have external linkage
────────────────────────────────────────────────────
ERROR E2031: Multiple definitions of procedure f
(detected at link time)
```

**Declaration vs Definition**:
- **Declaration**: Introduces a name and signature without providing implementation
- **Definition**: Provides the implementation (procedure body)

**Example (valid - declaration + definition)**:

```cursive
// File: math.cursive
public procedure sqrt(x: f64): f64  // Declaration (if separate)
    {| |- true => true |}

public procedure sqrt(x: f64): f64
    {| |- true => true |}
{  // Definition
    // Implementation
    result /* ... */
}
```

**Example (invalid - multiple definitions)**:

```cursive
// File: utils_a.cursive
public procedure compute(x: i32): i32
    {| |- true => true |}
{
    result x * 2
}

// File: utils_b.cursive
public procedure compute(x: i32): i32
    {| |- true => true |}
{  // ❌ Same signature
    result x * 3  // Different implementation
}

// ❌ ERROR E2031: Multiple definitions of compute
```

### 2.6.5.3 ODR Exemptions

**Inline procedures**: Procedures marked with the `[[inline]]` attribute may have multiple definitions across compilation units, as the implementation is expected to inline them and perform deduplication.

**Requirement 2.6.8**: The implementation should deduplicate identical inline procedure definitions. The deduplication mechanism is implementation-defined.

**Generic/monomorphized procedures**: Instantiations of generic procedures may appear in multiple compilation units. The linker deduplicates identical instantiations.

### 2.6.5.4 ODR Violation Consequences

**Requirement 2.6.9**: When an ODR violation is detected:
1. **Detectable violations**: The implementation shall issue a link-time error
2. **Undetectable violations**: If the implementation cannot detect the violation, the behavior is undefined

**Undefined behavior**: If multiple definitions of a procedure or type with external linkage exist and the violation is not detected, the program exhibits undefined behavior (see §[REF_TBD] Undefined Behavior Catalog).

---

## 2.6.6 Linkage Control Attributes

### 2.6.6.1 Overview

**Purpose**: Linkage control attributes provide opt-in mechanisms to override default linkage behavior for special cases such as Foreign Function Interface (FFI) and low-level system programming.

**Requirement 2.6.10**: Linkage control attributes shall only apply to entities with `public` visibility.

### 2.6.6.2 The `[[no_mangle]]` Attribute

**Purpose**: Preserve the exact symbol name without name mangling.

**Syntax**:

```cursive
[[no_mangle]]
public procedure my_c_function(x: i32): i32
    {| |- true => true |}
{
    result x * 2
}
```

**Semantics**: The symbol name in the binary is exactly `my_c_function`, with no mangling applied.

**Use cases**:
- Exporting procedures for C FFI
- Interoperating with external linkers expecting specific symbol names
- Debugging (predictable symbol names)

**Requirement 2.6.11**: Entities marked `[[no_mangle]]` must have `public` visibility and external linkage.

### 2.6.6.3 The `[[extern("C")]]` Attribute

**Purpose**: Use C calling convention and disable name mangling.

**Syntax**:

```cursive
[[extern("C")]]
public procedure ffi_function(x: i32, y: i32): i32
    {| |- true => true |}
{
    result x + y
}
```

**Semantics**:
1. Symbol name is not mangled (`ffi_function` in binary)
2. C calling convention is used (platform-specific ABI)
3. Compatible with C linkers and FFI

**Use cases**:
- Exposing Cursive procedures to C code
- Implementing system call wrappers
- Writing library bindings

**Requirement 2.6.12**: Entities marked `[[extern("C")]]` must have `public` visibility, external linkage, and C-compatible types (no Cursive-specific constructs like permissions or modals in the signature).

### 2.6.6.4 The `[[weak]]` Attribute

**Purpose**: Declare a weak symbol that may be overridden by a strong symbol with the same name.

**Syntax**:

```cursive
[[weak]]
public procedure default_handler()
    {| |- true => true |}
{
    println("Default handler")
}
```

**Semantics**: If another compilation unit provides a non-weak (strong) definition of `default_handler`, the strong definition takes precedence at link time.

**Use cases**:
- Providing default implementations that can be overridden
- Plugin architectures
- Weak linking for optional dependencies

**Requirement 2.6.13**: Weak symbol resolution is implementation-defined and platform-dependent. Not all platforms support weak symbols.

### 2.6.6.5 Attribute Interactions

**Requirement 2.6.14**: Linkage control attributes shall satisfy the following constraints:

1. **`[[no_mangle]]`** and **`[[extern("C")]]`** are mutually exclusive (extern("C") implies no_mangle)
2. **`[[weak]]`** may be combined with **`[[no_mangle]]`** or **`[[extern("C")]]`**
3. All linkage control attributes require **`public`** visibility

**Invalid combinations**:

```cursive
[[no_mangle]]
[[extern("C")]]  // ❌ ERROR: Redundant (extern("C") implies no_mangle)
public procedure foo()
    {| |- true => true |}
{ }
```

---

## 2.6.7 Canonical Example: Multi-File Program with All Linkage Types

The following multi-file example demonstrates all linkage categories and their interactions:

```cursive
// ========== File: math.cursive ==========

// External linkage (public)
public procedure add(x: i32, y: i32): i32
    {| |- true => true |}
{
    result internal_add_impl(x, y)
}
// Binary symbol: _Cursive_math_add (example mangled name)

// Internal linkage (internal - default)
internal procedure internal_add_impl(x: i32, y: i32): i32
    {| |- true => true |}
{
    result x + y
}
// Binary symbol: local symbol, not exported

// No linkage (private)
private procedure unused_helper()
    {| |- true => true |}
{
    println("Never called")
}
// No symbol in binary (may be eliminated by optimizer)

// No linkage (local variable)
procedure compute(n: i32): i32
    {| |- true => true |}
{
    let temp = n * 2  // Automatic storage, no linkage
    result temp
}

// External linkage with no mangling (FFI)
[[no_mangle]]
public procedure c_compatible_add(x: i32, y: i32): i32
    {| |- true => true |}
{
    result add(x, y)
}
// Binary symbol: c_compatible_add (exact name)

// External linkage with C calling convention
[[extern("C")]]
public procedure c_ffi_multiply(x: i32, y: i32): i32
    {| |- true => true |}
{
    result x * y
}
// Binary symbol: c_ffi_multiply (C ABI)

// Weak symbol (can be overridden)
[[weak]]
public procedure default_error_handler(msg: string)
    {| |- true => true |}
{
    println("Default error: {}", msg)
}
// Weak symbol, may be overridden by strong definition

// ========== File: main.cursive ==========
import math

procedure main(): i32
    {| |- true => true |}
{
    // External linkage: accessible
    let sum = math::add(5, 3)
    println("Sum: {}", sum)

    // Internal linkage: NOT accessible (compile error)
    // let impl_result = math::internal_add_impl(5, 3)  // ❌ ERROR

    // No linkage (private): NOT accessible
    // math::unused_helper()  // ❌ ERROR: private

    // External linkage (no mangle): accessible
    let c_sum = math::c_compatible_add(10, 20)

    // External linkage (extern C): accessible
    let product = math::c_ffi_multiply(4, 5)

    // Weak symbol: accessible
    math::default_error_handler("Test error")

    result 0
}

// ========== Linking Phase ==========
// Linker processes:
// 1. Resolves math::add to _Cursive_math_add (external linkage)
// 2. May inline math::internal_add_impl (internal linkage)
// 3. Eliminates math::unused_helper if unused (no linkage)
// 4. Exports c_compatible_add with exact name
// 5. Exports c_ffi_multiply with C ABI
// 6. Marks default_error_handler as weak symbol
```

---

## 2.6.8 Error Codes

This section defines error codes related to linkage:

- **E2030**: Multiple conflicting definitions of type (ODR violation for types)
- **E2031**: Multiple definitions of a procedure with external linkage (ODR violation for procedures)
- **E2032**: Linkage control attribute applied to non-public entity
- **E2033**: Invalid combination of linkage control attributes
- **E2034**: `[[extern("C")]]` applied to a procedure with a non-C-compatible signature

---

## 2.6.9 Conformance Requirements

A conforming implementation shall satisfy the following requirements with respect to linkage:

1. **Linkage determination** (§2.6.1): The implementation shall assign linkage to entities according to visibility and entity kind.

2. **External linkage** (§2.6.2): The implementation shall export symbols with external linkage in the binary symbol table.

3. **Internal linkage** (§2.6.3): The implementation shall not export symbols with internal linkage outside their compilation unit or package.

4. **No linkage** (§2.6.4): The implementation shall not create symbols for entities with no linkage.

5. **ODR enforcement** (§2.6.5): The implementation shall detect and report ODR violations for types and procedures with external linkage (errors E2030, E2031).

6. **Attribute support** (§2.6.6): The implementation should support linkage control attributes (`[[no_mangle]]`, `[[extern("C")]]`, `[[weak]]`) where platform capabilities permit.

7. **Name mangling** (§2.6.2.4): The implementation may apply name mangling to symbols with external linkage, except where overridden by attributes.

---

## 2.6.10 Notes and Examples

### Informative Note 1: Linkage and Optimization

Internal linkage and no linkage enable aggressive compiler optimizations:
- **Inlining**: Procedures with internal linkage or no linkage may be inlined freely
- **Dead code elimination**: Unused entities with internal/no linkage may be removed
- **Constant propagation**: Values with no linkage may be optimized away entirely

External linkage inhibits some optimizations because the entity must be preserved in the binary for potential external references.

### Informative Note 2: Separate Compilation

The linkage model supports separate compilation: each compilation unit is compiled independently, producing an object file with exported symbols (external linkage) and local symbols (internal linkage). The linker combines object files, resolving external symbol references and detecting ODR violations.

### Informative Note 3: Platform-Specific Linkage

Some linkage concepts are platform-dependent:
- **Weak symbols**: Supported on Unix-like systems (ELF), not universally available
- **Symbol visibility**: Controlled differently on Windows (DLL export) vs Unix (ELF visibility)
- **Name mangling**: Scheme varies by platform and ABI

Cursive's linkage model abstracts these platform differences, with implementation-defined behavior where necessary.

---

**End of Section 2.6: Linkage and Program Units**
