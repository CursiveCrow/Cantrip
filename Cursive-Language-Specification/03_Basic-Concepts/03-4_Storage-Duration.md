# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-4_Storage-Duration.md
**Section**: §3.4 Storage Duration
**Stable label**: [basic.storage]
**Forward references**: §3.1 [basic.object], §3.3 [basic.scope], §5.2 [decl.variable], §5.7 [decl.initialization], Clause 11 [memory]

---

### §3.4 Storage Duration [basic.storage]

#### §3.4.1 Overview

[1] _Storage duration_ determines when object storage is allocated and released. Cursive provides four storage duration categories: static, automatic, region, and temporary. Each category has deterministic lifetime rules that enable compile-time reasoning about object validity.

[2] Understanding storage duration is essential for reasoning about object lifetime (§3.3.3), memory allocation (§11.3), and RAII patterns where destruction timing is semantically significant.

#### §3.4.2 Static Storage Duration [basic.storage.static]

[3] Objects with _static storage duration_ are allocated before program execution begins and destroyed after program execution completes. Module-scope bindings (§2.5.4) have static storage duration.

**Allocation timing:**
[4] Static objects are allocated during the program load phase, before `main` executes. Their storage addresses are determined at link time and remain constant throughout execution.

**Initialization timing:**
[5] Static object initialization occurs during module initialization (§4.6) according to the eager dependency graph. Initialization order is deterministic and specified by topological sort of module dependencies. Each static binding is initialized exactly once.

**Lifetime:**
[6] Static objects live for the entire program execution. They are never destroyed explicitly; their destructors (if any) execute during program termination after `main` returns or `exit` is called.

**Examples:**

**Example 3.4.2.1 (Static storage and initialization):**

```cursive
// Module-scope binding: static storage duration
let GLOBAL_CONFIG: Config = load_configuration()

public procedure main(): i32
    [[ io::write |- true => true ]]
{
    // GLOBAL_CONFIG is already initialized when main executes
    println("Config version: {}", GLOBAL_CONFIG.version)
    result 0
}
// GLOBAL_CONFIG destroyed during program termination
```

#### §3.4.3 Automatic Storage Duration [basic.storage.automatic]

[7] Objects with _automatic storage duration_ are allocated when execution reaches their declaration and destroyed when the enclosing scope exits. Procedure parameters and block-local bindings have automatic storage duration.

**Allocation timing:**
[8] Automatic objects are allocated on the call stack when control flow reaches the binding declaration. Storage is reserved as part of the stack frame for the enclosing procedure or block.

**Initialization timing:**
[9] Automatic objects are initialized when execution reaches their binding statement:

```cursive
procedure demo(flag: bool)
{
    let always = 1                  // Initialized unconditionally

    if flag {
        let conditional = 2          // Initialized only when flag is true
    }
}
```

**Lifetime:**
[10] An automatic object's lifetime begins at initialization and ends when the scope that introduced the binding exits. Destruction occurs in reverse declaration order (LIFO discipline).

**Stack frame lifetime:**
[11] Procedure parameters have lifetimes spanning the entire procedure body. They are initialized when the procedure is called and destroyed when the procedure returns (normally or via panic). Local bindings nested within the procedure have shorter lifetimes bounded by their enclosing blocks.

**Examples:**

**Example 3.4.3.1 (Automatic storage and LIFO destruction):**

```cursive
procedure allocate_resources(): i32
    [[ alloc::heap |- true => true ]]
{
    let first = Resource::new(1)      // Allocated first
    let second = Resource::new(2)     // Allocated second
    let third = Resource::new(3)      // Allocated third

    result compute(first, second, third)

    // Destruction order: third, second, first (LIFO)
}
```

**Example 3.4.3.2 (Conditional initialization):**

```cursive
procedure conditional_allocation(should_allocate: bool): i32
    [[ alloc::heap |- true => true ]]
{
    var result: i32 = 0   // Initialize with default value

    if should_allocate {
        let data = allocate_buffer()   // Automatic storage, created in if-block
        result = process(data)
        // `data` destroyed here when if-block exits
    }

    result result
}
```

#### §3.4.4 Region Storage Duration [basic.storage.region]

[12] Objects with _region storage duration_ are allocated within an explicit region block and destroyed when that region exits. Region storage provides fine-grained lifetime control with deterministic deallocation.

[ Note: Region allocation is implemented through the `Arena` modal type (§11.3.2), a built-in modal type with states representing the arena lifecycle (@Active, @Frozen, @Freed). The `region` keyword creates a scoped `Arena@Active` with automatic cleanup. This note provides early context; complete semantics are specified in §11.3.
— end note ]

**Syntax:**
[13] Region blocks use the `region` keyword:

```cursive
region arena {
    let obj = allocate_in_region()
}
// All objects allocated in `arena` are destroyed here
```

**Allocation timing:**
[14] Objects are allocated in the specified region when execution reaches their allocation site. The region maintains a stack-like allocation arena that supports fast allocation without individual deallocation.

**Lifetime:**
[15] Objects allocated in a region have lifetimes bounded by the region block's lexical extent. When the region exits, all objects allocated within it are destroyed in reverse allocation order (LIFO), and the region's entire storage is reclaimed in a single operation.

**Escape analysis:**
[16] Pointers or references to region-allocated objects shall not escape the region block. The compiler performs static escape analysis (§11.3) to enforce this constraint. Violations produce compile-time diagnostics (region escape errors).

**Examples:**

**Example 3.4.4.1 (Region-based allocation):**

```cursive
procedure process_batch(items: [Item])
    [[ alloc::region, io::write |- true => true ]]
{
    region batch {
        let workspace = create_workspace_in(batch)

        loop item: Item in items {
            let processed = transform(item, workspace)
            output(processed)
        }

        // workspace and all region-allocated objects destroyed here
    }
    // Region storage reclaimed in single operation
}
```

**Example 3.4.4.2 - invalid (Region escape):**

```cursive
procedure escape_region(): Ptr<Buffer>
    [[ alloc::region |- true => true ]]
{
    region temp {
        let buffer = allocate_buffer_in_region()
        result Ptr::new(&buffer)     // error: cannot escape region-allocated object
    }
}
```

#### §3.4.5 Temporary Storage Duration [basic.storage.temporary]

[17] _Temporary values_ are unnamed objects created during expression evaluation. They have storage duration extending from creation to the end of the full-expression containing their creation.

**Creation sites:**
[18] Temporaries are created by:

- Intermediate computation results (e.g., `(a + b)` in `(a + b) * c`)
- Record/tuple/array literals not immediately bound
- Returned values from procedure calls before assignment
- Implicit conversions and coercions

**Lifetime:**
[19] A temporary's lifetime begins when it is created and ends when the full-expression completes. A _full-expression_ is an expression that is not a subexpression of another expression.

[20] Temporaries are destroyed in reverse creation order after the full-expression completes but before the next statement begins.

**Lifetime extension:**
[21] When a temporary is bound to a `let` binding, its lifetime is extended to match the binding's scope. This prevents use-after-free when binding to expression results:

```cursive
let ref <- create_temporary()   // Temporary lifetime extended to binding scope
```

**Examples:**

**Example 3.4.5.1 (Temporary creation and destruction):**

```cursive
procedure compute(a: i32, b: i32, c: i32): i32
{
    // Temporary (a + b) created and destroyed within this expression
    result (a + b) * c

    // Execution flow:
    // 1. Evaluate a → value₁
    // 2. Evaluate b → value₂
    // 3. Create temporary temp₁ = value₁ + value₂
    // 4. Evaluate c → value₃
    // 5. Create temporary temp₂ = temp₁ * value₃
    // 6. Destroy temp₁ (reverse order)
    // 7. Destroy temp₂ after assignment to result
}
```

**Example 3.4.5.2 (Temporary lifetime extension):**

```cursive
let data = create_buffer()        // Temporary from create_buffer() bound to `data`
                                   // Lifetime extended to match `data`'s scope
```

#### §3.4.6 Storage Duration Summary [basic.storage.summary]

[22] Table 3.4.1 summarizes the four storage duration categories:

**Table 3.4.1 — Storage duration categories**

| Category  | Declaration site | Allocation timing | Destruction timing  | Lifetime bounds       |
| --------- | ---------------- | ----------------- | ------------------- | --------------------- |
| Static    | Module scope     | Before `main`     | After `main`        | Program execution     |
| Automatic | Procedure/block  | Scope entry       | Scope exit (LIFO)   | Lexical scope         |
| Region    | Region block     | Allocation site   | Region exit (LIFO)  | Region scope          |
| Temporary | Expression       | Creation point    | Full-expression end | Expression evaluation |

#### §3.4.7 Destruction and RAII [basic.storage.raii]

[23] Cursive follows the RAII (Resource Acquisition Is Initialization) principle: resource acquisition occurs during object initialization, and resource release occurs during object destruction. Destruction is automatic and deterministic.

[24] When an object's scope or region exits:

1. If the object's type has a destructor, the destructor executes
2. Subobjects are destroyed in reverse field-declaration order
3. Storage is released according to the storage duration category

[25] Destructor execution is guaranteed for all storage duration categories except when the program panics or is forcibly terminated. Unwinding behavior during panics is specified in Clause 11.

**Example 3.4.7.1 (RAII pattern):**

```cursive
record FileHandle {
    path: string@Managed,
    descriptor: i32,

    // Destructor ensures file is closed when object is destroyed
    procedure drop(~!)
        [[ fs::close |- true => true ]]
    {
        os::close(self.descriptor)
    }
}

procedure process_file(path: string@View)
    [[ fs::open, fs::write |- true => true ]]
{
    let file = FileHandle::open(path)
    write_data(file, data)
    // file.drop() called automatically when scope exits
    // No manual cleanup required
}
```

#### §3.4.8 Integration with Permissions [basic.storage.permissions]

[26] Storage duration interacts with the permission system (§11.4):

- **Unique permission**: Grants exclusive access to an object, enabling moves and mutable operations
- **Shared permission**: Allows concurrent read-only access from multiple bindings
- **Const permission**: Provides read-only access without exclusivity requirements

[27] Permission constraints are enforced at compile time and do not affect storage duration categories. A static object may be accessed through const, shared, or unique permissions; an automatic object may likewise carry any permission the type system allows.

#### §3.4.9 Conformance Requirements [basic.storage.requirements]

[28] Implementations shall:

1. Support all four storage duration categories with the allocation and destruction timing specified in this subclause
2. Allocate static objects before `main` executes and initialize them according to module dependency order (§4.6)
3. Allocate automatic objects on the call stack or equivalent storage with LIFO discipline
4. Implement region blocks with escape analysis preventing region-allocated references from escaping
5. Create and destroy temporaries according to full-expression boundaries
6. Execute destructors deterministically in reverse construction order
7. Emit diagnostics for initialization order violations, region escapes, and use-after-move

[29] Implementations shall document their stack size limits and region allocation strategies, as these are implementation-defined properties (§1.5.3).

---

**Previous**: §3.3 Scope and Lifetime (Overview) | **Next**: §3.5 Alignment and Layout (Overview)
