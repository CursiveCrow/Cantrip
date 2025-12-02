# Cursive Foreign Function Interface (FFI) System

## Proposal Revision 3.0 (Corrected)

**Status:** Draft  
**Authors:** Language Design Team  
**Last Updated:** 2025-12-02

---

## Table of Contents

- [Cursive Foreign Function Interface (FFI) System](#cursive-foreign-function-interface-ffi-system)
  - [Proposal Revision 3.0 (Corrected)](#proposal-revision-30-corrected)
  - [Table of Contents](#table-of-contents)
  - [Clause 20: Foreign Function Interface](#clause-20-foreign-function-interface)
    - [20.1 Design Principles](#201-design-principles)
    - [20.2 The FfiSafe Form](#202-the-ffisafe-form)
    - [20.3 Foreign Procedure Declarations](#203-foreign-procedure-declarations)
    - [20.4 Foreign Global Declarations](#204-foreign-global-declarations)
    - [20.5 Exported Procedures](#205-exported-procedures)
    - [20.6 Capability Isolation Patterns](#206-capability-isolation-patterns)
    - [20.7 Standard FFI Types](#207-standard-ffi-types)
    - [20.8 Foreign Contracts](#208-foreign-contracts)
    - [20.9 Platform Type Aliases](#209-platform-type-aliases)
    - [20.10 Dynamic Loading](#2010-dynamic-loading)
    - [20.11 Unwind Semantics](#2011-unwind-semantics)
    - [20.12 Diagnostics](#2012-diagnostics)
  - [Appendix A: C Type Mapping Reference](#appendix-a-c-type-mapping-reference)
  - [Appendix B: Compile-Time Layout Verification](#appendix-b-compile-time-layout-verification)
  - [Appendix C: Complete Example — Wrapping a C Library](#appendix-c-complete-example--wrapping-a-c-library)

---

## Clause 20: Foreign Function Interface

### 20.1 Design Principles

##### Definition

The **Foreign Function Interface (FFI)** is the subsystem through which Cursive code interoperates with code written in other languages—primarily C and C-compatible ABIs. FFI enables Cursive programs to call foreign procedures, access foreign global variables, and export Cursive procedures for invocation by foreign code.

##### Core Principles

The Cursive FFI adheres to five foundational design principles:

**Principle 1: Static-by-Default**

FFI bindings are resolved at compile time or link time. Ad-hoc runtime symbol lookup is not permitted in safe code. Dynamic loading (§20.10) is an explicit, capability-controlled extension.

**Principle 2: Unverifiable Behavior (UVB) at Boundary**

Foreign code executes outside Cursive's semantic model. All interactions with foreign code constitute **Unverifiable Behavior** per §1.2 and MUST occur within `unsafe` blocks.

**Principle 3: Capability Blindness**

Foreign code cannot exercise, inspect, or forge Cursive capabilities. Witness types are prohibited in FFI signatures. The capability system's invariants are maintained by ensuring capabilities never cross the FFI boundary.

**Principle 4: Separation from Comptime**

FFI is a runtime facility. Comptime code (§16) MUST NOT invoke foreign procedures or access foreign globals, except for binding generation via the Introspection API.

**Principle 5: Two-Layer Architecture**

FFI operates in two layers:

| Layer         | Safety   | Purpose                                             |
| :------------ | :------- | :-------------------------------------------------- |
| Raw FFI       | `unsafe` | Direct foreign calls, raw pointers, no abstractions |
| Safe Wrappers | Safe     | Capability-requiring APIs built atop raw FFI        |

Safe wrappers transform raw FFI into idiomatic Cursive APIs that integrate with the capability and type systems.

##### Rationale

These principles ensure that:

1. **Safety is explicit:** The `unsafe` keyword marks all FFI interaction points, making audit boundaries clear.
2. **Capabilities remain unforgeable:** Foreign code cannot conjure authority it was not granted.
3. **Verification is scoped:** The contract and region systems operate normally within Cursive; only the foreign boundary is unverifiable.
4. **Interop is practical:** Real-world systems programming requires calling C libraries; the FFI makes this possible without compromising the safety model elsewhere.

---

### 20.2 The FfiSafe Form

##### Definition

The `FfiSafe` form classifies types whose runtime representation is compatible with a stable C-style ABI. Only `FfiSafe` types may appear in:

* Parameter and return types of foreign procedures
* Types of foreign globals
* Parameter and return types of exported Cursive procedures

##### Syntax & Declaration

```cursive
/// Types that can safely cross the FFI boundary.
/// Provides compile-time layout introspection for ABI compatibility verification.
form FfiSafe {
    /// The C-compatible size of this type in bytes.
    comptime procedure c_size() -> usize;
    
    /// The C-compatible alignment of this type in bytes.
    comptime procedure c_alignment() -> usize;
    
    /// Verifies this type's layout matches expectations.
    /// Returns true if layout matches, false otherwise.
    comptime procedure verify_layout(expected_size: usize, expected_align: usize) -> bool {
        Self::c_size() == expected_size and Self::c_alignment() == expected_align
    }
}
```

##### Static Semantics

**Well-Formedness Constraint (WF-FfiSafe)**

A type `T` is FFI-safe if and only if:

1. `T` declares `<: FfiSafe` and provides implementations of `c_size()` and `c_alignment()`
2. `T` has a complete, fixed layout (`HasLayout(T)` holds per §17.6.4)
3. `T` does not contain any prohibited types (see below)

$$\frac{
  T <: \texttt{FfiSafe} \quad
  \text{HasLayout}(T) \quad
  \neg\text{ContainsProhibited}(T)
}{
  \text{FfiSafe}(T)
} \quad \text{(WF-FfiSafe)}$$

**Automatic Implementations**

The following types have compiler-provided `FfiSafe` implementations:

| Type Category            | Examples                                   | Notes                                          |
| :----------------------- | :----------------------------------------- | :--------------------------------------------- |
| Signed integers          | `i8`, `i16`, `i32`, `i64`, `i128`, `isize` | Fixed-width, two's complement                  |
| Unsigned integers        | `u8`, `u16`, `u32`, `u64`, `u128`, `usize` | Fixed-width                                    |
| Floating-point           | `f16`, `f32`, `f64`                        | IEEE 754 representation                        |
| Raw pointers             | `*imm T`, `*mut T`                         | For any `T` (inner type need not be `FfiSafe`) |
| Fixed-size arrays        | `[T; N]`                                   | When `T <: FfiSafe`                            |
| Sparse function pointers | `(T₁, ..., Tₙ) -> R`                       | When all `Tᵢ` and `R` are `FfiSafe`            |

**Primitive Type Definitions (Conceptual)**

```cursive
// Compiler-provided primitive definitions
record i8 <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { 1 }
    comptime procedure c_alignment() -> usize { 1 }
}

record i16 <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { 2 }
    comptime procedure c_alignment() -> usize { 2 }
}

record i32 <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { 4 }
    comptime procedure c_alignment() -> usize { 4 }
}

record i64 <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { 8 }
    comptime procedure c_alignment() -> usize { 8 }
}

record i128 <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { 16 }
    comptime procedure c_alignment() -> usize { 16 }
}

record isize <: FfiSafe, Copy, Clone, Eq, Ord, Hash {
    comptime procedure c_size() -> usize { introspect~>size_of::<isize>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<isize>() }
}

// Similar definitions for u8, u16, u32, u64, u128, usize, f16, f32, f64
```

##### User-Defined FFI-Safe Types

A user-defined type implements `FfiSafe` by:

1. Applying the `[[layout(C)]]` attribute
2. Declaring `<: FfiSafe` in the type definition
3. Providing `c_size()` and `c_alignment()` implementations
4. Ensuring all fields are themselves `FfiSafe`

```cursive
[[layout(C)]]
record Point <: FfiSafe {
    x: f64,
    y: f64,
    
    comptime procedure c_size() -> usize { 16 }
    comptime procedure c_alignment() -> usize { 8 }
}

[[layout(C)]]
record Rect <: FfiSafe {
    origin: Point,
    size: Point,
    
    comptime procedure c_size() -> usize { 32 }
    comptime procedure c_alignment() -> usize { 8 }
}
```

##### Derive Support

The `[[derive(FfiSafe)]]` attribute generates the `FfiSafe` implementation automatically:

```cursive
[[layout(C)]]
[[derive(FfiSafe)]]
record Point {
    x: f64,
    y: f64,
}
```

The derive macro:

1. Verifies `[[layout(C)]]` is present (else emits `E-FFI-3301`)
2. Verifies all fields implement `FfiSafe` (else emits `E-FFI-3302`)
3. Verifies the type contains no prohibited types (else emits `E-FFI-3303`)
4. Generates `c_size()` and `c_alignment()` using introspection

**Generated Code (Conceptual)**

```cursive
// [[derive(FfiSafe)]] generates:
record Point <: FfiSafe {
    x: f64,
    y: f64,
    
    comptime procedure c_size() -> usize { 
        introspect~>size_of::<Point>() 
    }
    comptime procedure c_alignment() -> usize { 
        introspect~>align_of::<Point>() 
    }
}
```

##### Generic FFI-Safe Types

For a generic type to be FFI-safe, type parameters appearing in the layout MUST be constrained to `FfiSafe`:

```cursive
[[layout(C)]]
record Pair<T <: FfiSafe> <: FfiSafe {
    first: T,
    second: T,
    
    comptime procedure c_size() -> usize { 
        introspect~>size_of::<Pair<T>>() 
    }
    comptime procedure c_alignment() -> usize { 
        introspect~>align_of::<Pair<T>>() 
    }
}

// Usage:
// Pair<i32> is FFI-safe ✓
// Pair<string@View> is ill-formed (constraint violation) ✗
```

##### Prohibited Types

The following types MUST NOT implement `FfiSafe`:

| Type Category                  | Rationale                                                    |
| :----------------------------- | :----------------------------------------------------------- |
| `bool`                         | C booleans may have non-normalized bit patterns; use `CBool` |
| Modal types                    | Runtime discriminant encoding is not C-compatible            |
| `Ptr<T>` (safe pointer)        | Modal type with state tracking                               |
| Witness types (`witness Form`) | Zero-sized capability tokens have no ABI representation      |
| Capability forms               | Authority cannot cross the FFI boundary                      |
| `Context`                      | Contains capability witnesses                                |
| Types containing witnesses     | Transitive prohibition                                       |
| Opaque forms (`opaque`)        | Size unknown; layout undefined                               |
| Tuples                         | C ABI handling is not portable                               |
| Slices (`[T]`)                 | Fat pointer representation                                   |
| String types                   | Modal types; use `CStr`/`CString`                            |
| Closure types (`\|T\| -> U`)   | Contains environment pointer                                 |

**Diagnostic for Prohibited Types**

```
E-FFI-3303: Type `Connection` cannot implement `FfiSafe`: contains modal state
  --> src/network.cur:42:1
   |
42 | record Connection <: FfiSafe {
   | ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   |
   = note: modal types have runtime discriminants incompatible with C ABI
   = help: use a raw pointer or handle type for FFI
```

##### RAII Types and FfiSafe

A type implementing both `Drop` and `FfiSafe` requires the `[[ffi_pass_by_value]]` attribute if it will be passed by value across the FFI boundary:

```cursive
[[layout(C)]]
[[ffi_pass_by_value]]
record CString <: FfiSafe, Drop {
    ptr: *mut c_char,
    len: usize,
    cap: usize,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<CString>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<CString>() }
    
    procedure drop(~!) {
        if self.ptr != null {
            unsafe { c_free(self.ptr as *mut opaque c_void); }
        }
    }
}
```

**Rationale:** Foreign code may copy the value per C semantics. Cursive's `Drop` runs only on the Cursive-side value. The attribute acknowledges this potential for resource duplication.

Without `[[ffi_pass_by_value]]`, passing a `Drop + FfiSafe` type by value in FFI emits `E-FFI-3306`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-FFI-3301` | Error    | `FfiSafe` on type without `[[layout(C)]]`                 | Compile-time | Rejection |
| `E-FFI-3302` | Error    | `FfiSafe` type has non-FFI-safe field                     | Compile-time | Rejection |
| `E-FFI-3303` | Error    | `FfiSafe` on prohibited type category                     | Compile-time | Rejection |
| `E-FFI-3304` | Error    | Generic `FfiSafe` type with unconstrained parameter       | Compile-time | Rejection |
| `E-FFI-3305` | Error    | `FfiSafe` type has field with incomplete type             | Compile-time | Rejection |
| `E-FFI-3306` | Error    | `Drop + FfiSafe` by-value without `[[ffi_pass_by_value]]` | Compile-time | Rejection |

---

### 20.3 Foreign Procedure Declarations

##### Definition

A **foreign procedure declaration** introduces a procedure whose implementation is provided by foreign (non-Cursive) code. Foreign procedures are declared in `extern` blocks and MUST only be called within `unsafe` blocks.

##### Syntax & Declaration

**Grammar**

```ebnf
extern_block        ::= "extern" string_literal "{" extern_item* "}"

extern_item         ::= foreign_procedure | foreign_global

foreign_procedure   ::= [visibility] "procedure" identifier 
                        "(" [param_list] ")" ["->" type]
                        [variadic_spec] [foreign_contract] ";"

variadic_spec       ::= "..." | "...," type

param_list          ::= param ("," param)*

param               ::= identifier ":" type

foreign_contract    ::= "|=" foreign_assumes

foreign_assumes     ::= "@foreign_assumes" "(" predicate_list ")"

predicate_list      ::= predicate ("," predicate)*
```

**Example**

```cursive
extern "C" {
    procedure malloc(size: usize) -> *mut opaque c_void;
    procedure free(ptr: *mut opaque c_void);
    procedure memcpy(
        dest: *mut opaque c_void,
        src: *imm opaque c_void,
        n: usize
    ) -> *mut opaque c_void;
    
    procedure printf(format: *imm c_char, ...) -> c_int;
    
    procedure read(fd: c_int, buf: *mut opaque c_void, count: usize) -> isize
        |= @foreign_assumes(fd >= 0, buf != null);
}
```

##### Static Semantics

**ABI String**

The string literal following `extern` specifies the calling convention:

| ABI String     | Meaning                                  |
| :------------- | :--------------------------------------- |
| `"C"`          | Platform's standard C calling convention |
| `"C-unwind"`   | C convention with unwinding support      |
| `"system"`     | Platform's system call convention        |
| `"stdcall"`    | x86 stdcall (Windows)                    |
| `"fastcall"`   | x86 fastcall                             |
| `"vectorcall"` | x86-64 vectorcall (Windows)              |

The default is `"C"` if omitted.

**Type Restrictions**

All parameter types and return types in foreign procedure declarations MUST be `FfiSafe`:

$$\frac{
  \texttt{extern}\ \textit{abi}\ \{\ \texttt{procedure}\ f(p_1: T_1, \ldots, p_n: T_n) \to R;\ \}
}{
  \forall i.\ T_i <: \texttt{FfiSafe} \quad R <: \texttt{FfiSafe}
} \quad \text{(T-Extern-Proc)}$$

Violation emits `E-FFI-3310`.

**Variadic Procedures**

C-style variadic procedures are declared with `...`:

```cursive
extern "C" {
    // Untyped variadic (like C's printf)
    procedure printf(format: *imm c_char, ...) -> c_int;
    
    // Typed variadic (all varargs must be same type)
    procedure sum_ints(count: c_int, ..., c_int) -> c_int;
}
```

Variadic arguments undergo **default argument promotion** per C semantics:

| Original Type            | Promoted To |
| :----------------------- | :---------- |
| `i8`, `i16`, `u8`, `u16` | `c_int`     |
| `f32`                    | `f64`       |
| All others               | Unchanged   |

**Call-Site Semantics**

Invoking a foreign procedure:

1. MUST occur within an `unsafe` block
2. Evaluates arguments left-to-right
3. Performs any necessary type promotions for variadic arguments
4. Transfers control to foreign code
5. Returns result (if any) upon foreign code completion

```cursive
procedure example() {
    let ptr = unsafe { malloc(1024) };  // ✓ unsafe block
    
    // let ptr2 = malloc(1024);  // ✗ E-FFI-3320: extern call outside unsafe
}
```

##### Link Attributes

**`[[link_name]]`** — Override symbol name:

```cursive
extern "C" {
    [[link_name("__real_malloc")]]
    procedure malloc(size: usize) -> *mut opaque c_void;
}
```

**`[[link]]`** — Specify library linkage:

```cursive
[[link(name: "ssl", kind: "dylib")]]
extern "C" {
    procedure SSL_new(ctx: *mut SSL_CTX) -> *mut SSL;
}
```

Link kinds:

| Kind          | Meaning                   |
| :------------ | :------------------------ |
| `"dylib"`     | Dynamic library (default) |
| `"static"`    | Static library            |
| `"framework"` | macOS framework           |
| `"raw-dylib"` | Windows delay-load        |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-FFI-3310` | Error    | Non-FfiSafe type in extern signature     | Compile-time | Rejection |
| `E-FFI-3311` | Error    | Unknown ABI string                       | Compile-time | Rejection |
| `E-FFI-3312` | Error    | Variadic without fixed parameters        | Compile-time | Rejection |
| `E-FFI-3313` | Error    | Body provided for foreign procedure      | Compile-time | Rejection |
| `E-FFI-3320` | Error    | Extern call outside `unsafe` block       | Compile-time | Rejection |
| `E-FFI-3321` | Error    | Non-promotable type in variadic position | Compile-time | Rejection |

---

### 20.4 Foreign Global Declarations

##### Definition

A **foreign global declaration** introduces a global variable whose storage is provided by foreign code.

##### Syntax & Declaration

**Grammar**

```ebnf
foreign_global      ::= [visibility] ["mut"] identifier ":" type ";"
```

**Example**

```cursive
extern "C" {
    errno: c_int;                      // immutable binding to foreign global
    mut environ: *mut *mut c_char;     // mutable binding
}
```

##### Static Semantics

**Type Restriction**

The declared type MUST be `FfiSafe`. Violation emits `E-FFI-3330`.

**Mutability**

- Without `mut`: Read-only access; writes are ill-formed
- With `mut`: Read and write access permitted

**Access Semantics**

All access to foreign globals MUST occur within `unsafe` blocks:

```cursive
procedure get_errno() -> c_int {
    unsafe { errno }  // ✓
}

procedure clear_errno() {
    // errno = 0;  // ✗ not declared mut
}
```

##### Constraints & Legality

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-FFI-3330` | Error    | Non-FfiSafe type for foreign global  | Compile-time | Rejection |
| `E-FFI-3331` | Error    | Write to non-mut foreign global      | Compile-time | Rejection |
| `E-FFI-3332` | Error    | Foreign global access outside unsafe | Compile-time | Rejection |

---

### 20.5 Exported Procedures

##### Definition

An **exported procedure** is a Cursive procedure made callable from foreign code. Exported procedures use the `[[export]]` attribute and have restricted signatures.

##### Syntax & Declaration

**Grammar**

```ebnf
export_attr         ::= "[[" "export" "(" string_literal ")" "]]"
                      | "[[" "export" "(" string_literal "," export_opts ")" "]]"

export_opts         ::= export_opt ("," export_opt)*

export_opt          ::= "link_name" ":" string_literal
```

**Example**

```cursive
[[export("C")]]
public procedure add(a: c_int, b: c_int) -> c_int {
    a + b
}

[[export("C", link_name: "mylib_init")]]
public procedure initialize(config: *imm Config) -> c_int {
    // initialization logic
    0
}
```

##### Static Semantics

**Visibility Requirement**

Exported procedures MUST be `public`. Non-public exports emit `E-FFI-3350`.

**Signature Restrictions**

Exported procedure signatures are subject to strict constraints:

1. All parameter types MUST be `FfiSafe`
2. Return type MUST be `FfiSafe` (or unit `()`)
3. Parameters MUST NOT be witness types or capability forms
4. Parameters MUST NOT contain `Context` or witness-containing types
5. The procedure MUST NOT have capability witness parameters

```cursive
// ✓ Valid export
[[export("C")]]
public procedure process(data: *imm u8, len: usize) -> c_int {
    // ...
}

// ✗ Invalid: witness parameter
[[export("C")]]
public procedure bad_export(fs: witness FileSystem) -> c_int {  // E-FFI-3351
    // ...
}

// ✗ Invalid: Context parameter
[[export("C")]]
public procedure also_bad(ctx: Context) -> c_int {  // E-FFI-3353
    // ...
}
```

**Calling Convention**

The ABI string specifies the calling convention, identical to `extern` blocks.

**Symbol Name**

By default, the exported symbol name matches the procedure name. Use `link_name` to override:

```cursive
[[export("C", link_name: "crs_buffer_new")]]
public procedure buffer_new(cap: usize) -> *mut Buffer {
    // ...
}
```

**No Mangling**

Exported procedures use unmangled names suitable for C linkage. Name conflicts with other exports emit `E-FFI-3354`.

##### Constraints & Legality

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-FFI-3350` | Error    | `[[export]]` on non-public procedure         | Compile-time | Rejection |
| `E-FFI-3351` | Error    | Witness/capability parameter in export       | Compile-time | Rejection |
| `E-FFI-3352` | Error    | Non-FfiSafe type in export signature         | Compile-time | Rejection |
| `E-FFI-3353` | Error    | Context or witness-containing type in export | Compile-time | Rejection |
| `E-FFI-3354` | Error    | Duplicate export symbol name                 | Link-time    | Rejection |

---

### 20.6 Capability Isolation Patterns

##### Overview

Cursive's capability system (§13) MUST NOT be compromised by FFI. This section defines patterns that maintain capability isolation while enabling practical interoperability.

##### 20.6.1 The Fundamental Constraint

**Capability Blindness Rule**

Foreign code cannot:
- Receive capability witnesses
- Return capability witnesses
- Inspect or forge capabilities
- Exercise Cursive-mediated authority

This is enforced by prohibiting witness types in all FFI signatures.

##### 20.6.2 Runtime Handle Pattern

For APIs where foreign code initiates calls into Cursive (e.g., library exports), use opaque handles:

**Handle Type**

```cursive
[[layout(C)]]
[[derive(FfiSafe)]]
record RuntimeHandle {
    id: u64,
    
    procedure is_valid(~) -> bool { self.id != 0 }
}

public let INVALID_HANDLE: RuntimeHandle = RuntimeHandle { id: 0 };
```

**Context Registry**

```cursive
module std::ffi::runtime_context {
    use std::collections::HashMap;
    use std::sync::Mutex;
    
    record ContextEntry {
        ctx: Context,
        // Additional metadata as needed
    }
    
    var registry: shared Mutex<HashMap<u64, ContextEntry>> := 
        Mutex::new(HashMap::new());
    var next_id: shared Mutex<u64> := Mutex::new(1);
    
    /// Register a context and obtain a handle for foreign code.
    public procedure register(ctx: Context) -> RuntimeHandle {
        let id = #next_id { 
            let current = *next_id;
            *next_id = current + 1;
            current
        };
        
        #registry {
            registry~>insert(id, ContextEntry { ctx });
        }
        
        RuntimeHandle { id }
    }
    
    /// Look up context from handle. Returns error if handle is invalid.
    public procedure lookup(handle: RuntimeHandle) -> Context | HandleError {
        #registry {
            match registry~>get(handle.id) {
                entry: ContextEntry => entry.ctx,
                _ => HandleError::InvalidHandle,
            }
        }
    }
    
    /// Unregister a handle, releasing the context.
    public procedure unregister(handle: RuntimeHandle) -> () | HandleError {
        #registry {
            match registry~>remove(handle.id) {
                _: ContextEntry => (),
                _ => HandleError::InvalidHandle,
            }
        }
    }
}
```

**Exported API Pattern**

```cursive
use std::ffi::runtime_context;

[[export("C", link_name: "mylib_create")]]
public procedure create() -> RuntimeHandle {
    // Obtain capabilities through normal Cursive means
    let ctx = Context::current();
    runtime_context::register(ctx)
}

[[export("C", link_name: "mylib_process")]]
public procedure process(handle: RuntimeHandle, data: *imm u8, len: usize) -> c_int {
    let ctx = match runtime_context::lookup(handle) {
        ctx: Context => ctx,
        _: HandleError => { return -1; },
    };
    
    unsafe {
        let buf = BufferView::from_raw(data, len);
        match do_processing(buf, ctx.fs) {
            (): () => 0,
            _: ProcessError => -1,
        }
    }
}

[[export("C", link_name: "mylib_destroy")]]
public procedure destroy(handle: RuntimeHandle) -> c_int {
    match runtime_context::unregister(handle) {
        (): () => 0,
        _: HandleError => -1,
    }
}
```

**Invariants**

1. `Context` never appears in `[[export]]` signatures
2. Capabilities flow: Cursive code → registry → handle → foreign code → handle → registry → Cursive code
3. Foreign code possesses only opaque `u64` identifiers

##### 20.6.3 Callback Context Pattern

For C callbacks that accept a `void*` user data parameter, use `CallbackContext<T>`:

```cursive
[[layout(C)]]
record CallbackContext<T> <: FfiSafe {
    ptr: *mut T,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<*mut T>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<*mut T>() }
    
    procedure as_void_ptr(~) -> *mut opaque c_void {
        self.ptr as *mut opaque c_void
    }
}

module callback_context {
    /// Create a new callback context by heap-allocating the value.
    public procedure new<T>(value: T, heap: witness HeapAllocator) -> CallbackContext<T> {
        let ptr = heap~>allocate::<T>();
        unsafe { *ptr = value; }
        CallbackContext { ptr }
    }
    
    /// Reconstruct from void pointer (in callback).
    public unsafe procedure from_void_ptr<T>(ptr: *mut opaque c_void) -> CallbackContext<T> {
        CallbackContext { ptr: ptr as *mut T }
    }
    
    /// Borrow the contained value.
    public procedure borrow<T>(ctx: CallbackContext<T>) -> const T {
        unsafe { *ctx.ptr }
    }
    
    /// Mutably borrow the contained value.
    public procedure borrow_mut<T>(ctx: ~! CallbackContext<T>) -> ~! T {
        unsafe { *ctx.ptr }
    }
    
    /// Destroy the context and deallocate.
    public procedure destroy<T>(ctx: CallbackContext<T>, heap: witness HeapAllocator) {
        unsafe { heap~>deallocate(ctx.ptr); }
    }
}
```

**Usage Example**

```cursive
extern "C" {
    procedure register_callback(
        cb: (*mut opaque c_void, c_int) -> (),
        user_data: *mut opaque c_void
    );
}

record MyCallbackData {
    counter: i32,
    fs: witness FileSystem,  // Capability stored here, not in FFI signature
}

procedure setup_callback(fs: witness FileSystem, heap: witness HeapAllocator) {
    let data = MyCallbackData { counter: 0, fs };
    let ctx = callback_context::new(data, heap);
    
    unsafe {
        register_callback(my_callback, ctx~>as_void_ptr());
    }
}

[[export("C")]]
public procedure my_callback(user_data: *mut opaque c_void, event: c_int) {
    unsafe {
        let ctx = callback_context::from_void_ptr::<MyCallbackData>(user_data);
        let data = callback_context::borrow_mut(ctx);
        
        data.counter += 1;
        // Can use data.fs capability here
    }
}
```

**Advantages**

- Zero synchronization overhead (no global registry lookup)
- Direct pointer access in callback
- Capability stored in heap-allocated struct, not in FFI signature

**Caution**

The programmer MUST ensure:
1. The callback context outlives all callbacks
2. `destroy` is called exactly once after all callbacks complete
3. Thread safety if callbacks may occur concurrently

##### 20.6.4 Region Pointer Escape Detection

Pointers to region-local storage MUST NOT escape via FFI:

```cursive
procedure bad_example() {
    region r {
        var local: i32 := 42;
        let ptr = &local as *mut i32;
        
        unsafe {
            store_pointer(ptr);  // E-FFI-3360: region-local pointer escapes via FFI
        }
    }
}
```

The compiler performs escape analysis on pointers derived from region-local bindings. A pointer escapes via FFI if it flows to:

1. An argument of a foreign procedure call
2. Assignment to a foreign global
3. Storage in a struct passed to foreign code

**Diagnostic**

```
E-FFI-3360: Region-local pointer escapes via FFI
  --> src/example.cur:8:13
   |
 5 |     region r {
   |            - pointer originates in this region
 ...
 8 |         store_pointer(ptr);
   |             ^^^^^^^^^^^^^ pointer escapes here
   |
   = note: foreign code may retain pointer beyond region lifetime
   = help: allocate on heap or use callback context pattern
```

---

### 20.7 Standard FFI Types

This section defines the standard library types provided for FFI interoperability.

##### 20.7.1 String Types

**`CStr` — Borrowed Null-Terminated String**

```cursive
[[layout(C)]]
record CStr <: FfiSafe {
    ptr: *imm c_char,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<*imm c_char>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<*imm c_char>() }
    
    /// Returns the raw pointer.
    procedure as_ptr(~) -> *imm c_char { self.ptr }
    
    /// Attempts to convert to a Cursive string view.
    /// Returns error if not valid UTF-8.
    procedure to_string_view(~) -> string@View | Utf8Error {
        unsafe {
            let len = c_strlen(self.ptr);
            let bytes = BufferView::from_raw(self.ptr as *imm u8, len);
            string::from_utf8_view(bytes)
        }
    }
    
    /// Returns the length in bytes (excluding null terminator).
    procedure len(~) -> usize {
        unsafe { c_strlen(self.ptr) }
    }
}

module cstr {
    /// Create a CStr from a raw pointer.
    /// # Safety
    /// Pointer must be valid and null-terminated.
    public unsafe procedure from_ptr(ptr: *imm c_char) -> CStr {
        CStr { ptr }
    }
}
```

**`CString` — Owned Null-Terminated String**

```cursive
[[layout(C)]]
[[ffi_pass_by_value]]
record CString <: FfiSafe, Drop {
    ptr: *mut c_char,
    len: usize,
    cap: usize,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<CString>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<CString>() }
    
    procedure drop(~!) {
        if self.ptr != null {
            unsafe { c_free(self.ptr as *mut opaque c_void); }
        }
    }
    
    /// Borrow as CStr.
    procedure as_cstr(~) -> CStr {
        CStr { ptr: self.ptr as *imm c_char }
    }
    
    /// Returns the raw pointer.
    procedure as_ptr(~) -> *imm c_char { 
        self.ptr as *imm c_char 
    }
    
    /// Returns mutable raw pointer.
    procedure as_mut_ptr(~!) -> *mut c_char { 
        self.ptr 
    }
    
    /// Consumes self and returns raw pointer without deallocating.
    procedure into_raw(~!) -> *mut c_char {
        let p = self.ptr;
        self.ptr = null;
        p
    }
    
    /// Length in bytes (excluding null terminator).
    procedure len(~) -> usize { self.len }
}

module cstring {
    /// Create CString from Cursive string.
    public procedure from_str(s: string@View, heap: witness HeapAllocator) -> CString {
        let len = s~>byte_len();
        let ptr = unsafe { heap~>allocate_bytes(len + 1) } as *mut c_char;
        
        unsafe {
            mem::copy(s~>as_ptr() as *imm c_char, ptr, len);
            *(ptr + len) = 0;  // null terminator
        }
        
        CString { ptr, len, cap: len + 1 }
    }
    
    /// Create CString from raw parts.
    /// # Safety
    /// Pointer must be valid, null-terminated, and allocated with c_malloc.
    public unsafe procedure from_raw(ptr: *mut c_char, len: usize, cap: usize) -> CString {
        CString { ptr, len, cap }
    }
}
```

##### 20.7.2 Pointer Wrappers

**`OwnedPtr<T>` — RAII Wrapper for Foreign Pointers**

```cursive
[[layout(C)]]
record OwnedPtr<T <: FfiSafe> <: FfiSafe, Drop {
    ptr: *mut T,
    deallocator: (*mut T) -> (),
    
    comptime procedure c_size() -> usize { introspect~>size_of::<OwnedPtr<T>>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<OwnedPtr<T>>() }
    
    procedure drop(~!) {
        if self.ptr != null {
            (self.deallocator)(self.ptr);
        }
    }
    
    /// Borrow immutably.
    procedure borrow(~) -> *imm T { 
        self.ptr as *imm T 
    }
    
    /// Borrow mutably.
    procedure borrow_mut(~!) -> *mut T { 
        self.ptr 
    }
    
    /// Check if null.
    procedure is_null(~) -> bool { 
        self.ptr == null 
    }
    
    /// Consume and return raw pointer without calling deallocator.
    procedure into_raw(~!) -> *mut T {
        let p = self.ptr;
        self.ptr = null;
        p
    }
}

module owned_ptr {
    /// Create OwnedPtr from raw pointer and deallocator.
    public procedure new<T <: FfiSafe>(
        ptr: *mut T, 
        deallocator: (*mut T) -> ()
    ) -> OwnedPtr<T> {
        OwnedPtr { ptr, deallocator }
    }
}
```

**`BorrowedPtr<T>` — Non-Owning Pointer Wrapper**

```cursive
[[layout(C)]]
record BorrowedPtr<T <: FfiSafe> <: FfiSafe {
    ptr: *imm T,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<*imm T>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<*imm T>() }
    
    procedure get(~) -> *imm T { self.ptr }
    procedure is_null(~) -> bool { self.ptr == null }
}

module borrowed_ptr {
    public procedure new<T <: FfiSafe>(ptr: *imm T) -> BorrowedPtr<T> {
        BorrowedPtr { ptr }
    }
}
```

##### 20.7.3 Buffer Types

**`BufferView<T>` — Borrowed Slice-Like View**

```cursive
[[layout(C)]]
record BufferView<T <: FfiSafe> <: FfiSafe {
    ptr: *imm T,
    len: usize,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<BufferView<T>>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<BufferView<T>>() }
    
    procedure as_ptr(~) -> *imm T { self.ptr }
    procedure len(~) -> usize { self.len }
    procedure is_empty(~) -> bool { self.len == 0 }
    
    /// Get element at index (bounds checked).
    procedure get(~, index: usize) -> T | OutOfBoundsError {
        if index >= self.len {
            return OutOfBoundsError { index, len: self.len };
        }
        unsafe { *(self.ptr + index) }
    }
    
    /// Get element at index (unchecked).
    unsafe procedure get_unchecked(~, index: usize) -> T {
        *(self.ptr + index)
    }
}

module buffer_view {
    /// Create BufferView from raw parts.
    /// # Safety
    /// Pointer must be valid for `len` elements.
    public unsafe procedure from_raw<T <: FfiSafe>(ptr: *imm T, len: usize) -> BufferView<T> {
        BufferView { ptr, len }
    }
    
    /// Create empty BufferView.
    public procedure empty<T <: FfiSafe>() -> BufferView<T> {
        BufferView { ptr: null, len: 0 }
    }
}
```

**`BufferMut<T>` — Mutable Buffer View**

```cursive
[[layout(C)]]
record BufferMut<T <: FfiSafe> <: FfiSafe {
    ptr: *mut T,
    len: usize,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<BufferMut<T>>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<BufferMut<T>>() }
    
    procedure as_ptr(~) -> *mut T { self.ptr }
    procedure len(~) -> usize { self.len }
    procedure is_empty(~) -> bool { self.len == 0 }
    
    procedure as_view(~) -> BufferView<T> {
        BufferView { ptr: self.ptr as *imm T, len: self.len }
    }
    
    /// Set element at index (bounds checked).
    procedure set(~!, index: usize, value: T) -> () | OutOfBoundsError {
        if index >= self.len {
            return OutOfBoundsError { index, len: self.len };
        }
        unsafe { *(self.ptr + index) = value; }
        ()
    }
    
    /// Set element at index (unchecked).
    unsafe procedure set_unchecked(~!, index: usize, value: T) {
        *(self.ptr + index) = value;
    }
}

module buffer_mut {
    /// Create BufferMut from raw parts.
    /// # Safety
    /// Pointer must be valid for `len` elements and allow mutation.
    public unsafe procedure from_raw<T <: FfiSafe>(ptr: *mut T, len: usize) -> BufferMut<T> {
        BufferMut { ptr, len }
    }
}
```

**`OwnedBuffer<T>` — Owned Foreign Buffer**

```cursive
[[layout(C)]]
record OwnedBuffer<T <: FfiSafe> <: FfiSafe, Drop {
    ptr: *mut T,
    len: usize,
    cap: usize,
    deallocator: (*mut T, usize) -> (),
    
    comptime procedure c_size() -> usize { introspect~>size_of::<OwnedBuffer<T>>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<OwnedBuffer<T>>() }
    
    procedure drop(~!) {
        if self.ptr != null {
            (self.deallocator)(self.ptr, self.cap);
        }
    }
    
    procedure as_view(~) -> BufferView<T> {
        BufferView { ptr: self.ptr as *imm T, len: self.len }
    }
    
    procedure as_mut(~!) -> BufferMut<T> {
        BufferMut { ptr: self.ptr, len: self.len }
    }
    
    procedure len(~) -> usize { self.len }
    procedure capacity(~) -> usize { self.cap }
    
    /// Consume and return raw parts without deallocating.
    procedure into_raw_parts(~!) -> (*mut T, usize, usize) {
        let parts = (self.ptr, self.len, self.cap);
        self.ptr = null;
        parts
    }
}

module owned_buffer {
    public procedure new<T <: FfiSafe>(
        ptr: *mut T,
        len: usize,
        cap: usize,
        deallocator: (*mut T, usize) -> ()
    ) -> OwnedBuffer<T> {
        OwnedBuffer { ptr, len, cap, deallocator }
    }
}
```

##### 20.7.4 C Boolean Type

```cursive
[[layout(C)]]
record CBool <: FfiSafe {
    value: u8,
    
    comptime procedure c_size() -> usize { 1 }
    comptime procedure c_alignment() -> usize { 1 }
    
    /// Convert to Cursive bool (any non-zero is true).
    procedure to_bool(~) -> bool { 
        self.value != 0 
    }
    
    /// Convert to Cursive bool assuming normalized value.
    /// # Safety
    /// Value must be 0 or 1.
    unsafe procedure to_bool_unchecked(~) -> bool {
        transmute::<u8, bool>(self.value)
    }
}

public let CBOOL_FALSE: CBool = CBool { value: 0 };
public let CBOOL_TRUE: CBool = CBool { value: 1 };

module cbool {
    /// Create CBool from Cursive bool.
    public procedure from_bool(b: bool) -> CBool {
        CBool { value: if b { 1 } else { 0 } }
    }
}
```

##### 20.7.5 File Descriptor Types

**`Fd` — Raw File Descriptor**

```cursive
[[layout(C)]]
record Fd <: FfiSafe, Copy, Clone {
    raw: c_int,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<c_int>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<c_int>() }
    
    procedure as_raw(~) -> c_int { self.raw }
    procedure is_valid(~) -> bool { self.raw >= 0 }
}

public let INVALID_FD: Fd = Fd { raw: -1 };
public let STDIN_FD: Fd = Fd { raw: 0 };
public let STDOUT_FD: Fd = Fd { raw: 1 };
public let STDERR_FD: Fd = Fd { raw: 2 };
```

**`OwnedFd` — RAII File Descriptor**

```cursive
// Note: OwnedFd does NOT implement FfiSafe - pass Fd instead
record OwnedFd <: Drop {
    fd: Fd,
    
    procedure drop(~!) {
        if self.fd.raw >= 0 {
            unsafe { close(self.fd.raw); }
        }
    }
    
    /// Borrow as raw Fd.
    procedure as_fd(~) -> Fd { self.fd }
    
    /// Get raw value.
    procedure as_raw(~) -> c_int { self.fd.raw }
    
    /// Consume and return raw value without closing.
    procedure into_raw(~!) -> c_int {
        let raw = self.fd.raw;
        self.fd.raw = -1;
        raw
    }
}

module owned_fd {
    /// Create OwnedFd from raw file descriptor.
    /// # Safety
    /// The fd must be valid and owned by the caller.
    public unsafe procedure from_raw(raw: c_int) -> OwnedFd {
        OwnedFd { fd: Fd { raw } }
    }
}
```

##### 20.7.6 Opaque Void Type

```cursive
/// C's void type for generic pointer operations.
opaque c_void;

// Note: The compiler specially handles *mut opaque c_void and *imm opaque c_void
// as FfiSafe types for interoperability with C's void*.
```

##### 20.7.7 Error Types

```cursive
[[layout(C)]]
record Errno <: FfiSafe, Copy, Clone {
    code: c_int,
    
    comptime procedure c_size() -> usize { introspect~>size_of::<c_int>() }
    comptime procedure c_alignment() -> usize { introspect~>align_of::<c_int>() }
    
    procedure is_success(~) -> bool { self.code == 0 }
    procedure as_code(~) -> c_int { self.code }
}

enum HandleError {
    /// The handle ID does not correspond to any registered context.
    InvalidHandle,
    /// The handle was valid but has been unregistered.
    Expired,
}

enum LoadError {
    /// Library file not found at specified path.
    NotFound,
    /// Insufficient permissions to load library.
    PermissionDenied,
    /// File exists but is not a valid dynamic library.
    InvalidFormat,
    /// Operating system reported an error.
    OsError { code: Errno },
}

enum SymbolError {
    /// No symbol with this name exists in the library.
    NotFound,
    /// Symbol exists but cannot be loaded.
    LoadFailed { code: Errno },
}

enum Utf8Error {
    /// Invalid UTF-8 byte sequence at given offset.
    InvalidByte { offset: usize },
    /// Incomplete UTF-8 sequence at end.
    IncompleteSequence,
}

record OutOfBoundsError {
    index: usize,
    len: usize,
}
```

##### 20.7.8 Modal Foreign Resource

```cursive
/// A modal type for managing foreign resources with compile-time state tracking.
/// Prevents double-free and use-after-free at compile time.
modal ForeignResource<T> {
    @Uninitialized { }
    
    @Initialized {
        handle: *mut T,
        destructor: (*mut T) -> (),
    } {
        /// Borrow the raw handle.
        procedure borrow(~) -> *mut T {
            self.handle
        }
        
        /// Borrow immutably.
        procedure borrow_imm(~) -> *imm T {
            self.handle as *imm T
        }
    }
    
    @Released { }
    
    /// Initialize with a handle and destructor.
    transition initialize(
        handle: *mut T,
        destructor: (*mut T) -> ()
    ) : @Uninitialized -> @Initialized {
        ForeignResource@Initialized { handle, destructor }
    }
    
    /// Release ownership and return the raw handle.
    /// Destructor will NOT be called.
    transition release(~!) : @Initialized -> @Released -> *mut T {
        let h = self.handle;
        ForeignResource@Released { };
        h
    }
    
    /// Forget the resource without running destructor.
    transition forget(~!) : @Initialized -> @Released {
        ForeignResource@Released { }
    }
}

// Drop only runs in @Initialized state
record ForeignResource@Initialized<T> <: Drop {
    handle: *mut T,
    destructor: (*mut T) -> (),
    
    procedure drop(~!) {
        (self.destructor)(self.handle);
    }
}
```

**Usage Example**

```cursive
extern "C" {
    procedure create_widget() -> *mut Widget;
    procedure destroy_widget(w: *mut Widget);
}

procedure example() {
    // Create uninitialized
    var resource: ForeignResource<Widget>@Uninitialized := 
        ForeignResource@Uninitialized { };
    
    // Initialize with foreign allocation
    let ptr = unsafe { create_widget() };
    resource = resource~>initialize(ptr, destroy_widget);
    
    // Use while in @Initialized state
    let handle = resource~>borrow();
    unsafe { do_something_with(handle); }
    
    // Destructor runs automatically when resource goes out of scope
    // OR explicitly release:
    // let raw = resource~>release();  // Now in @Released, no destructor called
}
```

##### 20.7.9 Variadic Helpers

**`CPromotable` Form**

```cursive
/// Types that undergo C default argument promotion in variadic contexts.
form CPromotable {
    /// The type this promotes to under C variadic calling conventions.
    type Promoted <: FfiSafe;
    
    /// Perform the promotion.
    procedure promote(value: Self) -> Self::Promoted;
}
```

**Implementations for Primitives**

```cursive
// Primitives that widen under C promotion rules

record f32 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = f64;
    
    procedure promote(value: f32) -> f64 {
        value as f64
    }
}

record i8 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = c_int;
    
    procedure promote(value: i8) -> c_int {
        value as c_int
    }
}

record i16 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = c_int;
    
    procedure promote(value: i16) -> c_int {
        value as c_int
    }
}

record u8 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = c_int;
    
    procedure promote(value: u8) -> c_int {
        value as c_int
    }
}

record u16 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = c_int;
    
    procedure promote(value: u16) -> c_int {
        value as c_int
    }
}

// Types that don't change under promotion

record i32 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = i32;
    
    procedure promote(value: i32) -> i32 {
        value
    }
}

record i64 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = i64;
    
    procedure promote(value: i64) -> i64 {
        value
    }
}

record f64 <: FfiSafe, CPromotable, Copy, Clone {
    // ... FfiSafe procedures ...
    
    type Promoted = f64;
    
    procedure promote(value: f64) -> f64 {
        value
    }
}
```

---

### 20.8 Foreign Contracts

##### Definition

**Foreign contracts** extend the contract system (§10) to FFI boundaries. They specify preconditions that callers must satisfy before invoking foreign procedures.

##### Syntax & Declaration

**Grammar**

```ebnf
foreign_contract    ::= "|=" "@foreign_assumes" "(" predicate_list ")"

predicate_list      ::= predicate ("," predicate)*

predicate           ::= comparison_expr | null_check | range_check

comparison_expr     ::= expr comparison_op expr

null_check          ::= expr "!=" "null" | expr "==" "null"

range_check         ::= expr "in" range_expr
```

**Example**

```cursive
extern "C" {
    procedure write(fd: c_int, buf: *imm u8, count: usize) -> isize
        |= @foreign_assumes(fd >= 0, buf != null, count > 0);
    
    procedure read(fd: c_int, buf: *mut u8, count: usize) -> isize
        |= @foreign_assumes(fd >= 0, buf != null);
    
    procedure memcpy(dest: *mut u8, src: *imm u8, n: usize) -> *mut u8
        |= @foreign_assumes(dest != null, src != null);
}
```

##### Static Semantics

**Predicate Context**

Predicates in `@foreign_assumes` may reference:

- Parameter names from the procedure signature
- Literal constants
- Pure functions and operators
- Fields of parameter values (for record types)

Predicates MUST NOT reference:

- Global mutable state
- Values not in scope at the call site
- Effectful operations

**Verification Modes**

Foreign contracts integrate with the contract verification modes defined in §10.4:

| Mode                   | Behavior                                                                  |
| :--------------------- | :------------------------------------------------------------------------ |
| `[[static]]` (default) | Caller must prove predicates at compile time. Failure emits `E-CON-2850`. |
| `[[dynamic]]`          | Runtime checks inserted before `unsafe` call. Violation causes panic.     |
| `[[assume]]`           | Predicates assumed true. No checks. For optimization only.                |

**Static Verification**

```cursive
extern "C" {
    procedure process(buf: *imm u8, len: usize) -> c_int
        |= @foreign_assumes(buf != null, len > 0);
}

procedure example(data: BufferView<u8>) {
    if data~>is_empty() {
        return;
    }
    
    // Compiler can prove: data.ptr != null (from BufferView invariants)
    // Compiler can prove: data.len > 0 (from !is_empty() check)
    unsafe {
        process(data~>as_ptr(), data~>len());  // ✓ Contracts satisfied
    }
}

procedure bad_example() {
    unsafe {
        process(null, 0);  // E-CON-2850: Cannot prove `buf != null`
    }
}
```

**Dynamic Verification**

```cursive
[[dynamic]]
extern "C" {
    procedure risky_call(ptr: *mut u8) -> c_int
        |= @foreign_assumes(ptr != null);
}

procedure example(ptr: *mut u8) {
    // Runtime check inserted: if ptr == null { panic!(...) }
    unsafe {
        risky_call(ptr);
    }
}
```

##### Future Extension: `@foreign_ensures`

> **Note:** Postconditions (`@foreign_ensures`) for foreign procedures are deferred to a future revision. These would specify conditions the foreign procedure guarantees upon return.

```cursive
// Future syntax (not yet supported):
extern "C" {
    procedure malloc(size: usize) -> *mut u8
        |= @foreign_assumes(size > 0)
        |= @foreign_ensures(result != null or out_of_memory);
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-CON-2850` | Error    | Cannot prove `@foreign_assumes` predicate | Compile-time | Rejection |
| `E-CON-2851` | Error    | Invalid predicate in foreign contract     | Compile-time | Rejection |
| `E-CON-2852` | Error    | Predicate references out-of-scope value   | Compile-time | Rejection |
| `P-CON-2860` | Panic    | `[[dynamic]]` contract violation          | Runtime      | Panic     |

---

### 20.9 Platform Type Aliases

##### Definition

Platform type aliases provide portable names for C types whose sizes vary across platforms.

##### Declarations

```cursive
module std::ffi::ctypes {
    // Character types
    public type c_char = i8;        // IDB: may be u8 on some platforms
    public type c_schar = i8;
    public type c_uchar = u8;
    
    // Short types
    public type c_short = i16;
    public type c_ushort = u16;
    
    // Int types
    public type c_int = i32;
    public type c_uint = u32;
    
    // Long types (platform-dependent)
    public type c_long = i64;       // IDB: i32 on 32-bit Windows
    public type c_ulong = u64;      // IDB: u32 on 32-bit Windows
    
    // Long long types
    public type c_longlong = i64;
    public type c_ulonglong = u64;
    
    // Size types
    public type c_size_t = usize;
    public type c_ssize_t = isize;
    public type c_ptrdiff_t = isize;
    
    // Floating-point types
    public type c_float = f32;
    public type c_double = f64;
}
```

> **Note:** `IDB` indicates Implementation-Defined Behavior. The actual type depends on the target platform's C ABI.

##### Platform-Specific Overrides

Implementations MUST provide correct type aliases for each supported target. For example:

| Type       | LP64 (Linux/macOS 64-bit) | LLP64 (Windows 64-bit) | ILP32 (32-bit) |
| :--------- | :------------------------ | :--------------------- | :------------- |
| `c_long`   | `i64`                     | `i32`                  | `i32`          |
| `c_ulong`  | `u64`                     | `u32`                  | `u32`          |
| `c_size_t` | `u64`                     | `u64`                  | `u32`          |

---

### 20.10 Dynamic Loading

##### Definition

**Dynamic loading** is the capability to load shared libraries at runtime and resolve symbols dynamically. This is an explicit extension to the static-by-default FFI model.

##### Capability Requirement

Dynamic loading requires the `DynamicLoader` capability:

```cursive
form DynamicLoader {
    /// Load a shared library from the filesystem.
    procedure load(~, path: string@View) -> DynamicLibrary | LoadError;
}
```

##### The `DynamicLibrary` Form

```cursive
form DynamicLibrary {
    /// Look up a raw symbol by name.
    procedure symbol(~, name: string@View) -> *mut opaque c_void | SymbolError;
    
    /// Look up a typed function pointer by name.
    procedure procedure_symbol<F>(~, name: string@View) -> F | SymbolError
        where F <: FfiSafe;
    
    /// Unload the library.
    procedure unload(~!);
}
```

##### Standard Implementation

```cursive
module std::ffi::dl {
    /// Handle to a dynamically loaded library.
    record Library <: DynamicLibrary, Drop {
        handle: *mut opaque c_void,
        path: CString,
        
        procedure drop(~!) {
            if self.handle != null {
                unsafe { dlclose(self.handle); }
            }
        }
        
        procedure symbol(~, name: string@View) -> *mut opaque c_void | SymbolError {
            let cname = cstring::from_str(name, Context::heap());
            let sym = unsafe { dlsym(self.handle, cname~>as_ptr()) };
            
            if sym == null {
                return SymbolError::NotFound;
            }
            sym
        }
        
        procedure procedure_symbol<F>(~, name: string@View) -> F | SymbolError
            where F <: FfiSafe
        {
            let sym = self~>symbol(name)?;
            unsafe { transmute::<*mut opaque c_void, F>(sym) }
        }
        
        procedure unload(~!) {
            if self.handle != null {
                unsafe { dlclose(self.handle); }
                self.handle = null;
            }
        }
    }
    
    /// Standard dynamic loader implementation.
    record Loader <: DynamicLoader {
        procedure load(~, path: string@View) -> Library | LoadError {
            let cpath = cstring::from_str(path, Context::heap());
            let handle = unsafe { dlopen(cpath~>as_ptr(), RTLD_NOW) };
            
            if handle == null {
                return LoadError::NotFound;  // Simplified; real impl checks dlerror()
            }
            
            Library { handle, path: cpath }
        }
    }
    
    extern "C" {
        procedure dlopen(path: *imm c_char, mode: c_int) -> *mut opaque c_void;
        procedure dlsym(handle: *mut opaque c_void, symbol: *imm c_char) -> *mut opaque c_void;
        procedure dlclose(handle: *mut opaque c_void) -> c_int;
        procedure dlerror() -> *imm c_char;
    }
    
    public let RTLD_NOW: c_int = 2;
    public let RTLD_LAZY: c_int = 1;
}
```

##### Usage Example

```cursive
use std::ffi::dl;

extern "C" {
    type MathFunc = (f64) -> f64;
}

procedure load_math_library(loader: witness DynamicLoader) -> MathFunc | LoadError | SymbolError {
    let lib = loader~>load("libm.so.6")?;
    let sin_func: MathFunc = lib~>procedure_symbol("sin")?;
    sin_func
}
```

##### Safety Considerations

1. **Symbol type safety is not guaranteed.** The programmer must ensure the declared function type matches the actual symbol.

2. **Library lifetime.** Functions obtained from a library become invalid after `unload()`. Calling them is UVB.

3. **Thread safety.** Loading/unloading libraries may not be thread-safe on all platforms.

---

### 20.11 Unwind Semantics

##### Definition

**Unwind semantics** define behavior when control flow crosses the FFI boundary abnormally (via exceptions, panics, or longjmp).

##### The `[[unwind]]` Attribute

```cursive
[[unwind("abort")]]     // Abort if foreign code attempts to unwind through this frame
[[unwind("allowed")]]   // Allow unwinding (requires "C-unwind" ABI)
```

##### Default Behavior

| Scenario                         | Default | Effect             |
| :------------------------------- | :------ | :----------------- |
| Foreign exception enters Cursive | Abort   | Process terminates |
| Cursive panic in callback to C   | Abort   | Process terminates |
| C longjmp across Cursive frames  | UVB     | Undefined behavior |

##### Safe Unwinding

To allow unwinding across FFI:

```cursive
extern "C-unwind" {
    [[unwind("allowed")]]
    procedure may_throw() -> c_int;
}

[[export("C-unwind")]]
[[unwind("allowed")]]
public procedure callback_that_may_panic() {
    // Panic here will unwind into C++ caller
}
```

> **Warning:** Allowing unwinding across FFI boundaries requires careful coordination with foreign code. Mismatched unwind semantics cause undefined behavior.

##### Constraints & Legality

| Code         | Severity | Condition                                  | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------- | :----------- | :-------- |
| `E-FFI-3370` | Error    | `[[unwind("allowed")]]` without unwind ABI | Compile-time | Rejection |
| `E-FFI-3371` | Error    | Invalid `[[unwind]]` argument              | Compile-time | Rejection |
| `A-FFI-3372` | Abort    | Unwind across non-unwind FFI boundary      | Runtime      | Abort     |

---

### 20.12 Diagnostics

##### Diagnostic Code Summary

**FfiSafe Diagnostics (E-FFI-33xx)**

| Code         | Condition                                                 |
| :----------- | :-------------------------------------------------------- |
| `E-FFI-3301` | `FfiSafe` on type without `[[layout(C)]]`                 |
| `E-FFI-3302` | `FfiSafe` type has non-FFI-safe field                     |
| `E-FFI-3303` | `FfiSafe` on prohibited type category                     |
| `E-FFI-3304` | Generic `FfiSafe` with unconstrained type parameter       |
| `E-FFI-3305` | `FfiSafe` type has field with incomplete type             |
| `E-FFI-3306` | `Drop + FfiSafe` by-value without `[[ffi_pass_by_value]]` |

**Foreign Declaration Diagnostics (E-FFI-331x)**

| Code         | Condition                            |
| :----------- | :----------------------------------- |
| `E-FFI-3310` | Non-FfiSafe type in extern signature |
| `E-FFI-3311` | Unknown ABI string                   |
| `E-FFI-3312` | Variadic without fixed parameters    |
| `E-FFI-3313` | Body provided for foreign procedure  |

**Call-Site Diagnostics (E-FFI-332x)**

| Code         | Condition                                |
| :----------- | :--------------------------------------- |
| `E-FFI-3320` | Extern call outside `unsafe` block       |
| `E-FFI-3321` | Non-promotable type in variadic position |

**Foreign Global Diagnostics (E-FFI-333x)**

| Code         | Condition                            |
| :----------- | :----------------------------------- |
| `E-FFI-3330` | Non-FfiSafe type for foreign global  |
| `E-FFI-3331` | Write to non-mut foreign global      |
| `E-FFI-3332` | Foreign global access outside unsafe |

**Export Diagnostics (E-FFI-335x)**

| Code         | Condition                                    |
| :----------- | :------------------------------------------- |
| `E-FFI-3350` | `[[export]]` on non-public procedure         |
| `E-FFI-3351` | Witness/capability parameter in export       |
| `E-FFI-3352` | Non-FfiSafe type in export signature         |
| `E-FFI-3353` | Context or witness-containing type in export |
| `E-FFI-3354` | Duplicate export symbol name                 |

**Escape Analysis Diagnostics (E-FFI-336x)**

| Code         | Condition                            |
| :----------- | :----------------------------------- |
| `E-FFI-3360` | Region-local pointer escapes via FFI |

**Unwind Diagnostics (E-FFI-337x)**

| Code         | Condition                                             |
| :----------- | :---------------------------------------------------- |
| `E-FFI-3370` | `[[unwind("allowed")]]` without unwind ABI            |
| `E-FFI-3371` | Invalid `[[unwind]]` argument                         |
| `A-FFI-3372` | Unwind across non-unwind FFI boundary (runtime abort) |

**Contract Diagnostics (E-CON-285x)**

| Code         | Condition                                        |
| :----------- | :----------------------------------------------- |
| `E-CON-2850` | Cannot prove `@foreign_assumes` predicate        |
| `E-CON-2851` | Invalid predicate in foreign contract            |
| `E-CON-2852` | Predicate references out-of-scope value          |
| `P-CON-2860` | `[[dynamic]]` contract violation (runtime panic) |

---

## Appendix A: C Type Mapping Reference

| C Type               | Cursive Type                               | Notes                         |
| :------------------- | :----------------------------------------- | :---------------------------- |
| `void`               | `()` (return) or `opaque c_void` (pointer) |                               |
| `_Bool`              | `CBool`                                    | NOT `bool`                    |
| `char`               | `c_char`                                   | Platform-dependent signedness |
| `signed char`        | `c_schar` / `i8`                           |                               |
| `unsigned char`      | `c_uchar` / `u8`                           |                               |
| `short`              | `c_short` / `i16`                          |                               |
| `unsigned short`     | `c_ushort` / `u16`                         |                               |
| `int`                | `c_int` / `i32`                            |                               |
| `unsigned int`       | `c_uint` / `u32`                           |                               |
| `long`               | `c_long`                                   | IDB: 32 or 64 bits            |
| `unsigned long`      | `c_ulong`                                  | IDB: 32 or 64 bits            |
| `long long`          | `c_longlong` / `i64`                       |                               |
| `unsigned long long` | `c_ulonglong` / `u64`                      |                               |
| `size_t`             | `c_size_t` / `usize`                       |                               |
| `ssize_t`            | `c_ssize_t` / `isize`                      |                               |
| `ptrdiff_t`          | `c_ptrdiff_t` / `isize`                    |                               |
| `float`              | `c_float` / `f32`                          |                               |
| `double`             | `c_double` / `f64`                         |                               |
| `T*`                 | `*mut T` or `*imm T`                       |                               |
| `const T*`           | `*imm T`                                   |                               |
| `T[]` / `T*` (array) | `BufferView<T>` / `BufferMut<T>`           |                               |
| `char*` (string)     | `CStr` / `CString`                         |                               |
| `void*`              | `*mut opaque c_void`                       |                               |
| Function pointer     | Sparse function type                       |                               |

---

## Appendix B: Compile-Time Layout Verification

For critical FFI types, layout can be verified at compile time:

```cursive
[[layout(C)]]
[[derive(FfiSafe)]]
record MyStruct {
    field1: i32,
    field2: f64,
    field3: *mut u8,
}

comptime {
    // Verify layout matches C expectations
    static_assert(
        MyStruct::verify_layout(24, 8),
        "MyStruct layout mismatch with C definition"
    );
    
    // Or verify individual properties
    static_assert(
        MyStruct::c_size() == 24,
        "MyStruct size mismatch"
    );
    static_assert(
        MyStruct::c_alignment() == 8,
        "MyStruct alignment mismatch"
    );
}
```

---

## Appendix C: Complete Example — Wrapping a C Library

This example demonstrates wrapping the standard C `stdio.h` file operations.

```cursive
module mylib::stdio {
    use std::ffi::*;
    use std::ffi::ctypes::*;
    
    // Foreign declarations
    extern "C" {
        procedure fopen(path: *imm c_char, mode: *imm c_char) -> *mut FILE;
        procedure fclose(file: *mut FILE) -> c_int;
        procedure fread(
            ptr: *mut opaque c_void,
            size: c_size_t,
            count: c_size_t,
            stream: *mut FILE
        ) -> c_size_t;
        procedure fwrite(
            ptr: *imm opaque c_void,
            size: c_size_t,
            count: c_size_t,
            stream: *mut FILE
        ) -> c_size_t;
        procedure fseek(stream: *mut FILE, offset: c_long, whence: c_int) -> c_int;
        procedure ftell(stream: *mut FILE) -> c_long;
        procedure feof(stream: *mut FILE) -> c_int;
        procedure ferror(stream: *mut FILE) -> c_int;
    }
    
    // Opaque C type
    opaque FILE;
    
    // Constants
    public let SEEK_SET: c_int = 0;
    public let SEEK_CUR: c_int = 1;
    public let SEEK_END: c_int = 2;
    
    // Error type
    public enum FileError {
        OpenFailed,
        ReadFailed,
        WriteFailed,
        SeekFailed,
        Eof,
    }
    
    // Safe wrapper type
    public record File <: Drop {
        handle: *mut FILE,
        
        procedure drop(~!) {
            if self.handle != null {
                unsafe { fclose(self.handle); }
            }
        }
        
        /// Read bytes into buffer. Returns number of bytes read.
        public procedure read(~!, buf: BufferMut<u8>) -> usize | FileError {
            let count = unsafe {
                fread(
                    buf~>as_ptr() as *mut opaque c_void,
                    1,
                    buf~>len(),
                    self.handle
                )
            };
            
            if count == 0 {
                if unsafe { feof(self.handle) } != 0 {
                    return FileError::Eof;
                }
                if unsafe { ferror(self.handle) } != 0 {
                    return FileError::ReadFailed;
                }
            }
            
            count
        }
        
        /// Write bytes from buffer. Returns number of bytes written.
        public procedure write(~!, buf: BufferView<u8>) -> usize | FileError {
            let count = unsafe {
                fwrite(
                    buf~>as_ptr() as *imm opaque c_void,
                    1,
                    buf~>len(),
                    self.handle
                )
            };
            
            if count < buf~>len() {
                return FileError::WriteFailed;
            }
            
            count
        }
        
        /// Seek to position.
        public procedure seek(~!, offset: i64, whence: c_int) -> () | FileError {
            let result = unsafe { fseek(self.handle, offset as c_long, whence) };
            if result != 0 {
                return FileError::SeekFailed;
            }
            ()
        }
        
        /// Get current position.
        public procedure tell(~) -> i64 {
            unsafe { ftell(self.handle) as i64 }
        }
    }
    
    /// Open a file. Requires FileSystem capability.
    public procedure open(
        path: string@View,
        mode: string@View,
        fs: witness FileSystem
    ) -> File | FileError {
        let cpath = cstring::from_str(path, Context::heap());
        let cmode = cstring::from_str(mode, Context::heap());
        
        let handle = unsafe { fopen(cpath~>as_ptr(), cmode~>as_ptr()) };
        
        if handle == null {
            return FileError::OpenFailed;
        }
        
        File { handle }
    }
}
```

**Usage**

```cursive
use mylib::stdio;

procedure example(fs: witness FileSystem) {
    let file = stdio::open("data.txt", "r", fs)?;
    
    var buffer: [u8; 1024];
    let buf_mut = buffer_mut::from_raw(&buffer[0], 1024);
    
    loop {
        match file~>read(buf_mut) {
            n: usize => {
                // Process n bytes
            },
            FileError::Eof => break,
            err: FileError => {
                // Handle error
                return;
            },
        }
    }
    
    // File automatically closed when `file` goes out of scope
}
```

---

**End of FFI System Proposal**