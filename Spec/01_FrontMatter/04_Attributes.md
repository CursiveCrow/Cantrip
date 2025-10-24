# Part I: Foundations - §4. Attributes and Annotations

**Section**: §4 | **Part**: Foundations (Part I)
**Previous**: [Abstract Syntax](03_AbstractSyntax.md) | **Next**: [Primitive Types](../01_TypeSystem/01_PrimitiveTypes.md)

---

**Definition 4.1 (Attributes):** An attribute is a metadata annotation that modifies the compilation behavior, analysis, or code generation for the item it decorates, evaluated at compile-time without affecting runtime semantics unless explicitly specified.

## 4. Attributes and Annotations

### 4.1 Overview

**Key innovation/purpose:** Attributes provide declarative compiler directives that modify how code is compiled, verified, or represented without changing the code's fundamental logic.

**When to use attributes:**
- Controlling memory layout for FFI interoperability (`[[repr(C)]]`)
- Selecting verification modes (`[[verify(static)]]`, `[[verify(runtime)]]`)
- Configuring runtime behavior (`[[overflow_checks(true)]]`)
- Providing metadata for tools and documentation (`[[module(...)]]`, `[[alias(...)]]`)
- Optimizing compilation (`[[inline]]`, `[[no_mangle]]`)

**When NOT to use attributes:**
- To implement core language behavior (use language features instead)
- As a macro or metaprogramming system (use `comptime` instead)
- To change program logic (attributes modify HOW code compiles, not WHAT it does)
- For runtime configuration (use function parameters or configuration files)

**Relationship to other features:**
- **Contracts (§17)**: Attributes can select verification mode for contract checking
- **Effects (§21)**: Some attributes affect effect checking
- **Memory Layout (§6.5)**: `[[repr(...)]]` controls record/enum layout
- **FFI (Part XIV)**: Attributes critical for C interoperability
- **Metaprogramming (§34)**: Attributes are declarative; `comptime` is computational

**Design principle:** Attributes are **declarative compiler hints**, not executable code. They configure the compiler's behavior but don't execute at runtime (unless they change runtime behavior like `[[overflow_checks]]`).

### 4.2 Syntax

#### 4.2.1 Concrete Syntax

**Grammar:**
```ebnf
Attribute      ::= "[[" AttributeBody "]]"
AttributeBody  ::= Ident ( "(" AttrArgs? ")" )?
AttrArgs       ::= AttrArg ( "," AttrArg )*
AttrArg        ::= Ident "=" Literal | Literal | Ident

AttributeList  ::= Attribute+
```

**Syntax:**
```cantrip
// Single attribute
[[repr(C)]]
record Point { x: f32, y: f32 }

// Multiple attributes (stacked)
[[verify(static)]]
[[inline]]
function critical_math(x: i32) { ... }

// Multiple attributes (grouped)
[[repr(C), module(name = "ffi")]]
record CPoint { x: f32, y: f32 }

// Attribute with multiple arguments
[[alias("to_string", "stringify", "as_str")]]
function to_str(x: Debug): String { ... }
```

**Placement rules:**
Attributes appear immediately before the item they modify:
- Module declarations
- Record, enum, modal, trait declarations
- Function and procedure declarations
- Function/procedure parameters
- Record/enum fields
- Type aliases
- Effect declarations

#### 4.2.2 Abstract Syntax

**Formal representation:**
```
Attribute ::= Attr(name: Ident, args: List⟨AttrArg⟩)
AttrArg   ::= Named(key: Ident, value: Literal)
            | Positional(value: Literal)
            | Flag(name: Ident)

Item      ::= item_kind × attrs: List⟨Attribute⟩
```

**Components:**
- **name**: Attribute identifier (e.g., `repr`, `verify`, `module`)
- **args**: List of attribute arguments
  - **Named**: Key-value pairs (`name = "value"`)
  - **Positional**: Just values (`C`, `static`)
  - **Flag**: Just identifiers (`inline`)

**AST representation:**
```
RecordDecl {
    name: "Point",
    fields: [...],
    attributes: [
        Attr("repr", [Positional(Ident("C"))]),
        Attr("module", [Named("name", String("ffi"))])
    ]
}
```

#### 4.2.3 Basic Examples

```cantrip
// Memory layout control
[[repr(C)]]
record Point {
    x: f32,
    y: f32,
}

// Verification mode
[[verify(static)]]
function safe_divide(x: i32, y: i32): i32
    must y != 0
{ x / y }

// Overflow checking
[[overflow_checks(false)]]
function fast_math(x: u32): u32 {
    x * x  // Won't check for overflow even in debug mode
}

// Module metadata
[[module(name = "core", version = "1.0")]]
module math { ... }

// Tooling support
[[alias("to_string", "stringify")]]
public function to_str(x: Debug): String {
    x.debug()
}
```

### 4.3 Static Semantics

#### 4.3.1 Well-Formedness Rules

**Attribute placement validity:**
```
[WF-Attr-Placement]
attr is an attribute
item is a declaration kind
placement(attr) allows item.kind
──────────────────────────────────
[[attr]] item is well-formed
```

**Attribute argument validation:**
```
[WF-Attr-Args]
attr = Attr(name, args)
∀arg ∈ args. signature(attr.name) accepts arg
─────────────────────────────────────────────
attribute arguments well-formed
```

**No conflicting attributes:**
```
[WF-Attr-No-Conflict]
attrs = list of attributes on item
∀a₁, a₂ ∈ attrs. a₁ ≠ a₂ ⇒ ¬conflicts(a₁, a₂)
───────────────────────────────────────────────
attribute list well-formed
```

**Examples:**
```cantrip
// Valid
[[repr(C)]]
record Point { ... }

// INVALID: repr only applies to records/enums
[[repr(C)]]          // ERROR: repr doesn't apply to functions
function foo() { ... }

// INVALID: conflicting attributes
[[inline]]
[[no_inline]]        // ERROR: inline and no_inline conflict
function bar() { ... }
```

#### 4.3.2 Core Attributes

Cantrip defines the following core attributes. All core attributes are NORMATIVE.

##### 4.3.2.1 `[[repr(...)]]` - Memory Layout Control

**Signature:** `[[repr(layout)]]` where `layout ∈ {C, transparent, packed, align(n)}`

**Applies to:** Records, enums

**Semantics:**
```
[Attr-Repr-C]
[[repr(C)]] record R { ... }
──────────────────────────────
R uses C-compatible layout (see §6.5)

[Attr-Repr-Packed]
[[repr(packed)]] record R { ... }
──────────────────────────────────
R has no padding between fields
```

**Example:**
```cantrip
// C-compatible layout for FFI
[[repr(C)]]
record Point {
    x: f32,  // offset 0
    y: f32,  // offset 4
}  // total size: 8, alignment: 4

// Packed layout (no padding)
[[repr(packed)]]
record Packed {
    a: u8,   // offset 0
    b: u32,  // offset 1 (no alignment padding!)
}  // total size: 5
```

##### 4.3.2.2 `[[verify(...)]]` - Verification Mode

**Signature:** `[[verify(mode)]]` where `mode ∈ {static, runtime, none}`

**Applies to:** Functions, procedures, records (for invariants)

**Semantics:**
```
[Attr-Verify-Static]
[[verify(static)]] function f(...) must P will Q { ... }
────────────────────────────────────────────────────────────────
contracts checked at compile-time via SMT solver

[Attr-Verify-Runtime]
[[verify(runtime)]] function f(...) must P will Q { ... }
──────────────────────────────────────────────────────────────────
contracts checked at runtime (assertions injected)

[Attr-Verify-None]
[[verify(none)]] function f(...) must P will Q { ... }
────────────────────────────────────────────────────────────
contracts are documentation only (no checking)
```

**Example:**
```cantrip
// Static verification (compile-time proof)
[[verify(static)]]
function binary_search(arr: [i32], target: i32): Option<usize>
    must is_sorted(arr)
{ ... }

// Runtime verification (dynamic checks)
[[verify(runtime)]]
function parse_int(s: str): i32
    must !s.is_empty()
    will result >= 0
{ ... }
```

##### 4.3.2.3 `[[overflow_checks(...)]]` - Integer Overflow Behavior

**Signature:** `[[overflow_checks(enabled)]]` where `enabled ∈ {true, false}`

**Applies to:** Functions, procedures, blocks

**Semantics:**
```
[Attr-Overflow-Enable]
[[overflow_checks(true)]] function f(...) { ... }
───────────────────────────────────────────────────
all integer operations in f check for overflow (even in release mode)

[Attr-Overflow-Disable]
[[overflow_checks(false)]] function f(...) { ... }
────────────────────────────────────────────────────
no overflow checks in f (wrapping behavior, even in debug mode)
```

**Example:**
```cantrip
// Always check for overflow (safety-critical code)
[[overflow_checks(true)]]
function account_balance(deposits: [u64]): u64 {
    deposits.sum()  // Panics on overflow even in --release
}

// Performance-critical code (wrapping is acceptable)
[[overflow_checks(false)]]
function hash_combine(a: u64, b: u64): u64 {
    a.wrapping_mul(31).wrapping_add(b)
}
```

##### 4.3.2.4 `[[module(...)]]` - Module Metadata

**Signature:** `[[module(key = value, ...)]]`

**Applies to:** Module declarations

**Semantics:** Provides metadata for package management and tooling.

**Common keys:**
- `name`: Module name (string)
- `version`: Semantic version (string)
- `author`: Author information (string)
- `description`: Module description (string)

**Example:**
```cantrip
[[module(
    name = "cantrip.core",
    version = "1.0.0",
    author = "Cantrip Team",
    description = "Core library for Cantrip language"
)]]
module core {
    ...
}
```

##### 4.3.2.5 `[[alias(...)]]` - Alternative Names for Tooling

**Signature:** `[[alias(name1, name2, ...)]]`

**Applies to:** Functions, procedures, types, modules

**Semantics:**
Declares alternative source-level names for an item to improve diagnostics, code search, and LLM tooling. Aliases are INFORMATIVE for tooling and MUST NOT affect name resolution or symbol linkage.

**Normative rules:**
```
[Attr-Alias-Informative]
[[alias(a₁, ..., aₙ)]] item
─────────────────────────────────────────
aliases are accepted in diagnostics and IDE features only
aliases do NOT create new exported symbols

[Attr-Alias-Collision]
[[alias(a)]] item₁
real_name(item₂) = a
item₁ ≠ item₂
──────────────────────────────────
emit warning W0101: alias collides with real name
real name wins in all resolution
```

**Example:**
```cantrip
// Provide search-friendly aliases
[[alias("to_string", "stringify", "as_str")]]
public function to_str(x: Debug): String {
    x.debug()
}

// IDE may suggest "to_string" when user types it
// but compiled code always uses "to_str"
```

**For FFI symbol names, use `[[export(...)]]` instead** (see Part XIV: FFI).

##### 4.3.2.6 Other Core Attributes

**`[[inline]]`** - Hint to inline this function
**`[[no_inline]]`** - Prevent inlining
**`[[no_mangle]]`** - Preserve exact symbol name (for FFI)
**`[[deprecated(message)]]`** - Mark as deprecated
**`[[must_use]]`** - Warn if return value is ignored
**`[[cold]]`** - Hint this code is rarely executed
**`[[hot]]`** - Hint this code is performance-critical

These are documented in their respective sections.

#### 4.3.3 User-Defined Attributes

**Status:** User-defined attributes are RESERVED for future versions.

Currently, only core attributes defined in this specification are recognized. User-defined attributes will cause a compilation error:

```cantrip
[[my_custom_attr]]    // ERROR: unknown attribute 'my_custom_attr'
function foo() { ... }
```

**Future design:** User-defined attributes may be added in conjunction with procedural macros or compile-time reflection (see §34).

### 4.4 Dynamic Semantics

#### 4.4.1 Compile-Time Processing

Attributes are processed during compilation and do not exist at runtime (with exceptions noted below).

**Processing phases:**
```
Source Code → [Lexer] → [Parser] → AST with Attributes → [Attribute Processing]
                                                                   ↓
                                    [Type Checker] ← Configuration applied
                                          ↓
                                    [Code Generator] ← Layout/optimization directives
```

**Attribute processing order:**
1. **Parsing**: Attributes parsed into AST
2. **Validation**: Well-formedness checking (§4.3.1)
3. **Application**: Attributes modify compilation behavior
4. **Erasure**: Most attributes erased after compilation

#### 4.4.2 Attributes Affecting Runtime Behavior

Some attributes change generated code and thus affect runtime:

**`[[overflow_checks(...)]]`:**
- Changes whether overflow checks are emitted
- Affects runtime performance and behavior

**`[[repr(...)]]`:**
- Changes memory layout
- Affects runtime memory usage and access patterns

**`[[inline]]`, `[[no_inline]]`:**
- Change code generation
- Affect runtime performance (but not observable behavior)

**`[[verify(runtime)]]`:**
- Injects runtime assertion checks
- Affects runtime behavior (can panic)

#### 4.4.3 Attributes Not Affecting Runtime

These attributes are purely compile-time metadata:

**`[[verify(static)]]`, `[[verify(none)]]`:**
- Only affect compile-time checking
- No runtime code generated

**`[[alias(...)]]`:**
- Informative only for tooling
- Zero runtime impact

**`[[module(...)]]`:**
- Metadata only
- May appear in debug info but not executable code

#### 4.4.4 Attribute Inheritance

**Attributes do NOT inherit:**
```cantrip
[[inline]]
record Container {
    [[inline]]      // Must explicitly annotate field if desired
    value: i32,
}

[[verify(static)]]
trait Validated {
    // Implementations do NOT automatically get [[verify(static)]]
    function check(self) -> bool;
}
```

Each item must be annotated independently.

### 4.5 Additional Properties

#### 4.5.1 Attribute Conflicts

**Conflicting attributes:**
- `[[inline]]` and `[[no_inline]]`
- `[[repr(C)]]` and `[[repr(packed)]` (use `[[repr(C, packed)]]` instead)

**Conflict detection:**
```
conflicts([[inline]], [[no_inline]]) = true
conflicts([[verify(static)]], [[verify(runtime)]) = true
conflicts([[repr(C)]], [[repr(transparent)]) = true
```

**Error when conflicts detected:**
```cantrip
[[inline]]
[[no_inline]]    // ERROR E4001: conflicting attributes 'inline' and 'no_inline'
function foo() { ... }
```

#### 4.5.2 Unknown Attribute Handling

**Unknown attributes cause compilation errors:**
```cantrip
[[unknown_attr]]    // ERROR E4002: unknown attribute 'unknown_attr'
function bar() { ... }
```

**Rationale:** Explicit errors prevent typos and forward compatibility issues.

**Namespacing:** Future versions may support namespaced attributes: `[[cantrip::core::inline]]`

### 4.6 Examples and Patterns

#### 4.6.1 FFI Data Structure

**Pattern:** C-compatible record layout for foreign function interface.

```cantrip
// Cantrip side (FFI bindings)
[[repr(C)]]
[[module(name = "ffi.sdl")]]
record SDL_Point {
    x: i32,
    y: i32,
}

[[repr(C)]]
record SDL_Rect {
    x: i32,
    y: i32,
    w: i32,
    h: i32,
}

// C side (SDL library)
// typedef struct {
//     int x;
//     int y;
// } SDL_Point;
```

**Explanation:** `[[repr(C)]]` will memory layout matches C exactly, enabling zero-cost FFI.

#### 4.6.2 Safety-Critical Verification

**Pattern:** Static verification for safety-critical code with runtime checks as fallback.

```cantrip
// Statically verified core logic
[[verify(static)]]
function compute_trajectory(
    velocity: Vector3,
    acceleration: Vector3,
    time: f64
): Vector3
    must time >= 0.0
    will result.magnitude() <= velocity.magnitude() + acceleration.magnitude() * time
{
    velocity + acceleration * time
}

// Runtime-verified external input handling
[[verify(runtime)]]
function parse_sensor_data(raw: [u8]): SensorReading
    must raw.len() >= 16
    will result.timestamp > 0
{
    // Parser implementation with runtime checks
    ...
}
```

**Explanation:** Use `[[verify(static)]]` for provably safe internal logic; use `[[verify(runtime)]]` for external input where static proof may be infeasible.

#### 4.6.3 Performance-Critical Tuning

**Pattern:** Fine-tuned optimization hints for hot code paths.

```cantrip
// High-frequency game loop
[[inline]]
[[overflow_checks(false)]]  // Wrapping is safe here
function update_entity_positions(
    entities: [mut Entity],
    delta_time: f32
) {
    for entity in entities {
        // Position wraps around world bounds
        entity.x = entity.x.wrapping_add((entity.vx * delta_time) as u32);
        entity.y = entity.y.wrapping_add((entity.vy * delta_time) as u32);
    }
}

// Rare error path
[[cold]]
function handle_critical_error(error: Error): ! {
    log_error(error);
    std.process.abort();
}
```

**Explanation:** `[[inline]]` reduces function call overhead; `[[overflow_checks(false)]]` eliminates checks where wrapping is intentional; `[[cold]]` tells optimizer this path is rare.

#### 4.6.4 Tooling-Friendly Aliases

**Pattern:** Provide discoverable aliases for common operations.

```cantrip
// Mathematical vector with multiple naming conventions
[[alias("len", "magnitude", "length", "norm")]]
public function size(v: Vector3): f32 {
    (v.x * v.x + v.y * v.y + v.z * v.z).sqrt()
}

// Collection methods with familiar names
[[alias("push_back", "append", "add")]]
public function push(vec: mut Vec<T>, item: T) {
    vec.internal_push(item);
}
```

**Explanation:** LLMs and developers can find functions using familiar terminology, while maintaining canonical naming in actual code.

---

**Previous**: [Abstract Syntax](03_AbstractSyntax.md) | **Next**: [Primitive Types](../01_TypeSystem/01_PrimitiveTypes.md)
