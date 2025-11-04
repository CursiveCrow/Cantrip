# CURSIVE GRANTS SYSTEM SPECIFICATION
## Authoritative Design Document

**Version**: 1.0 Draft
**Date**: 2025-11-03
**Status**: Design Authority - To Be Formalized
**Scope**: Compile-time capability tracking system

---

## EXECUTIVE SUMMARY

This document is the authoritative specification for Cursive's grants system - a compile-time capability tracking mechanism that restricts what operations procedures may perform.

**Core Achievement**: Fine-grained permission control with zero runtime overhead through static verification.

**Key Design Decisions**:
1. Compile-time only verification (zero runtime cost)
2. Explicit grant declarations via `grants` keyword
3. Subset-based propagation rules
4. Integration with contracts and procedures
5. Grant polymorphism for generic code
6. NOT algebraic effects (different goals and mechanisms)

---

## PART I: FOUNDATIONAL CONCEPTS

### 1. What Grants Are

**Definition**: Grants are compile-time capability tokens that declare what side effects and operations a procedure is permitted to perform.

**Core Properties**:
- **Compile-time only**: Grants are checked during compilation, not at runtime
- **Zero overhead**: No runtime representation or checking
- **Explicit**: All capabilities must be declared in procedure signatures
- **Compositional**: Grants flow through call chains via subset rules
- **Fine-grained**: Individual permissions for specific operations

**Example**:
```cursive
// Procedure declares it may write to I/O
procedure log_message(msg: string)
    grants io.write
{
    println(msg)
}

// Pure function - no grants clause, cannot perform side effects
function calculate(x: i32, y: i32): i32 {
    result x + y
}
```

---

### 2. What Grants Are NOT

#### 2.1 NOT Algebraic Effects

Grants are fundamentally different from algebraic effects systems:

| Aspect | Algebraic Effects | Cursive Grants |
|--------|-------------------|----------------|
| **Purpose** | Control flow abstraction | Permission tracking |
| **Runtime** | Suspend/resume execution | Zero overhead |
| **Implementation** | Delimited continuations | Static analysis only |
| **Provides** | Async, generators, exceptions | Access control |
| **Resumption** | Can resume suspended computation | No suspension mechanism |

**Algebraic effects** allow suspending and resuming computation (like async/await, generators).

**Cursive grants** only track permissions - no control flow changes.

#### 2.2 NOT Runtime Checks

Grants are **not** runtime capability objects:

```cursive
// This is what grants ARE NOT:
// ❌ Not runtime objects
// ❌ Not passed as parameters
// ❌ Not checked at runtime

// This is what grants ARE:
// ✅ Compile-time declarations
// ✅ Statically verified
// ✅ Zero runtime cost
procedure example() grants fs.read {
    // Compiler verifies at compile-time that:
    // - This procedure declares fs.read
    // - All calls are to procedures with compatible grants
}
```

#### 2.3 NOT First-Class Values

Grants cannot be:
- Stored in variables
- Passed as function arguments (except via polymorphism)
- Returned from functions
- Manipulated at runtime

They are purely compile-time annotations.

---

### 3. Design Philosophy

#### 3.1 Explicit Over Implicit

**Principle**: All side effects must be declared explicitly in procedure signatures.

**Manifestation**:
```cursive
// ✅ EXPLICIT: Grant declared in signature
procedure write_log(path: string, message: string)
    grants fs.write
{
    write_file(path, message)
}

// ❌ IMPLICIT: No grant declaration (would be rejected)
procedure implicit_write(path: string, message: string) {
    write_file(path, message)  // ERROR: fs.write not in grants
}
```

**Rationale**: Reading a procedure signature immediately reveals what capabilities it requires.

#### 3.2 Local Reasoning

**Principle**: Understanding grant requirements should not require whole-program analysis.

**Manifestation**:
- Grant requirements visible in procedure signature
- No action at a distance
- No hidden capabilities
- Call site can verify compatibility by looking at signatures only

```cursive
// Can verify compatibility by inspection:
procedure caller() grants fs.read, io.write {
    helper()  // ✅ Can verify: helper needs ⊆ caller has
}

procedure helper() grants io.write {
    println("Working")
}
```

#### 3.3 Zero Runtime Overhead

**Principle**: Grants have no runtime representation or cost.

**Manifestation**:
- All checking at compile-time
- No runtime grant objects
- No runtime permission checks
- Generated code identical to code without grants

**Verification**: Assembly output for procedures with/without grants is identical (grants only affect compile-time checking).

#### 3.4 Capability-Based Security

**Principle**: Grants provide fine-grained control over what operations code can perform.

**Security Benefits**:
- Least privilege: Procedures get only grants they need
- Isolation: Pure functions cannot perform I/O
- Auditability: Grant requirements visible in signatures
- Composability: Grants flow through call chains

---

### 4. Terminology

**Grant**: A compile-time capability token (e.g., `fs.read`, `io.write`)
**Grant Clause**: The `grants` keyword followed by grant list in procedure signature
**Grant Set**: Set of grants a procedure declares
**Grant Path**: Hierarchical grant identifier (e.g., `fs.read`, `alloc.heap`)
**Pure Function**: Function with no grant clause (cannot perform side effects)
**Procedure**: May have grant clause (can perform side effects)
**Subset Rule**: Callee grants must be subset of caller grants

---

## PART II: GRANT SYNTAX AND SEMANTICS

### 5. Basic Grant Syntax

#### 5.1 Grant Clause

**Syntax**:
```ebnf
GrantClause ::= "grants" GrantSet
GrantSet    ::= GrantPath ("," GrantPath)* ","?
GrantPath   ::= Identifier ("::" Identifier)*
```

**Single Grant**:
```cursive
procedure log(message: string)
    grants io.write
{
    println(message)
}
```

**Multiple Grants**:
```cursive
procedure process_file(path: string)
    grants fs.read, fs.write, alloc.heap
{
    let data = read_file(path)
    let processed = transform(data)
    write_file(path, processed)
}
```

**Trailing Comma Allowed**:
```cursive
procedure complex_operation()
    grants
        fs.read,
        fs.write,
        io.write,
        alloc.heap,
{
    // ...
}
```

#### 5.2 Pure Functions

**Functions** (declared with `function` keyword) **cannot** have grant clauses:

```cursive
// ✅ OK: Pure function, no grants
function add(x: i32, y: i32): i32 {
    result x + y
}

// ❌ ERROR: Functions cannot have grants
function invalid(x: i32) grants io.write: i32 {
    result x
}
```

**Rationale**: Functions are pure by definition. Use `procedure` for operations with side effects.

#### 5.3 Procedures

**Procedures** (declared with `procedure` keyword) may have grant clauses:

```cursive
// Procedure with grants
procedure display(value: i32)
    grants io.write
{
    println("{}", value)
}

// Procedure with no grants (pure procedure)
procedure compute(x: i32): i32 {
    result x * 2
}
```

---

### 6. Grant Paths

#### 6.1 Hierarchical Structure

Grants use dot notation for hierarchy:

```cursive
fs.read           // Filesystem read
fs.write          // Filesystem write
io.read           // I/O read
io.write          // I/O write
alloc.heap        // Heap allocation
alloc.region      // Region allocation
```

#### 6.2 Grant Hierarchy Rules

**Currently**: Grants are atomic - no automatic subset relationships.

```cursive
// These are INDEPENDENT grants:
grants fs.read    // Only allows filesystem reads
grants fs.write   // Only allows filesystem writes
grants fs          // INVALID: fs is not a grant (fs.read and fs.write are)
```

**Future Extension**: May add hierarchical subsumption (e.g., `fs` implies `fs.read` and `fs.write`).

#### 6.3 Qualified Paths

Grant paths can be namespaced:

```cursive
grants std::fs::read
grants custom::logging::write
```

---

## PART III: GRANT PROPAGATION

### 7. The Subset Rule

**Core Rule**: When procedure A calls procedure B, the grants required by B must be a subset of the grants declared by A.

**Formal Definition**:
```
For call site: A() calls B()
Requirement: grants(B) ⊆ grants(A)
```

**Canonical Example**:
```cursive
// Helper requires io.write
procedure log(message: string)
    grants io.write
{
    println(message)
}

// Caller declares io.write - satisfies subset rule
procedure process()
    grants io.write
{
    log("Processing")  // ✅ OK: {io.write} ⊆ {io.write}
}

// Caller does NOT declare io.write - violates subset rule
procedure broken() {
    log("Test")  // ❌ ERROR: {io.write} ⊄ {}
}
```

---

### 8. Grant Composition

#### 8.1 Union of Requirements

If a procedure calls multiple helpers, it needs the **union** of their grants:

```cursive
procedure helper_a() grants fs.read { /* ... */ }
procedure helper_b() grants io.write { /* ... */ }

procedure caller()
    grants fs.read, io.write  // Union: {fs.read} ∪ {io.write}
{
    helper_a()  // Needs: {fs.read}
    helper_b()  // Needs: {io.write}
}
```

#### 8.2 Transitive Closure

Grants propagate transitively through call chains:

```cursive
procedure level_3() grants io.write {
    println("Level 3")
}

procedure level_2() grants io.write {
    level_3()  // OK: {io.write} ⊆ {io.write}
}

procedure level_1() grants io.write {
    level_2()  // OK: {io.write} ⊆ {io.write}
}
```

---

### 9. Grant Checking Algorithm

#### 9.1 Call Graph Analysis

**Phase 1**: Build call graph
```
For each procedure P:
  declared_grants(P) = grants from P's signature

For each call site P₁ → P₂:
  required_grants(P₁ → P₂) = declared_grants(P₂)
```

**Phase 2**: Verify subset requirements
```
For each call site P₁ → P₂:
  if required_grants(P₁ → P₂) ⊄ declared_grants(P₁):
    ERROR: Grant requirement violation
```

#### 9.2 Example Analysis

```cursive
procedure main() grants fs.read, io.write {
    read_config()   // Requires: {fs.read}
    display()       // Requires: {io.write}
}

procedure read_config() grants fs.read {
    // ...
}

procedure display() grants io.write {
    // ...
}
```

**Verification**:
1. `main` → `read_config`: `{fs.read} ⊆ {fs.read, io.write}` ✅
2. `main` → `display`: `{io.write} ⊆ {fs.read, io.write}` ✅

---

## PART IV: BUILT-IN GRANTS CATALOG

### 10. File System Grants

#### 10.1 `grants fs.read`

**Permits**:
- Opening files for reading
- Reading file contents
- Querying file metadata
- Directory listing

**Canonical Example**:
```cursive
record FileData {
    content: [char; 4096]
    length: usize
}

procedure read_file(path: string): FileData
    grants fs.read
{
    let file = open_file_read(path)
    let buffer = [char; 4096]
    let length = read_into(file, buffer)
    close_file(file)

    result FileData { content: buffer, length: length }
}
```

#### 10.2 `grants fs.write`

**Permits**:
- Creating files
- Writing to files
- Deleting files
- Modifying file metadata

**Canonical Example**:
```cursive
procedure write_log(path: string, message: string)
    grants fs.write
{
    let file = create_file(path)
    write_string(file, message)
    close_file(file)
}
```

---

### 11. I/O Grants

#### 11.1 `grants io.write`

**Permits**:
- Writing to stdout
- Writing to stderr
- Printing to console
- Logging output

**Canonical Example**:
```cursive
procedure display_result(value: i32)
    grants io.write
{
    println("Result: {}", value)
}
```

#### 11.2 `grants io.read`

**Permits**:
- Reading from stdin
- Getting user input
- Console input

**Canonical Example**:
```cursive
procedure get_input(): string
    grants io.read, io.write
{
    println("Enter value:")
    result read_line()
}
```

---

### 12. Allocation Grants

#### 12.1 `grants alloc.heap`

**Permits**:
- Allocating memory on heap
- Creating heap-allocated pointers
- Escape-to-heap operations

**Canonical Example**:
```cursive
record Buffer {
    data: Ptr<[u8]>@Valid
    capacity: usize
}

procedure create_buffer(size: usize): Buffer
    grants alloc.heap
{
    let ptr = heap_allocate<u8>(size)
    result Buffer { data: ptr, capacity: size }
}
```

#### 12.2 `grants alloc.region`

**Permits**:
- Creating regions
- Allocating within regions
- Region-scoped allocation

**Canonical Example**:
```cursive
procedure compute_with_scratch(data: [i32; 100]): i32
    grants alloc.region
{
    region temp {
        let scratch = alloc_in<temp>([i32; 100])
        // Use scratch for temporary computations
        let result = process(data, scratch)
        result result
    }
}
```

---

### 13. Concurrency Grants

#### 13.1 `grants thread.spawn`

**Permits**:
- Spawning new threads
- Creating parallel tasks

**Canonical Example**:
```cursive
record ThreadHandle {
    id: usize
}

procedure parallel_work(): ThreadHandle
    grants thread.spawn
{
    result spawn_thread(worker_procedure)
}

procedure worker_procedure() {
    // Worker code
}
```

#### 13.2 `grants atomic.read`

**Permits**:
- Performing atomic read operations
- Loading atomic values

**Canonical Example**:
```cursive
record Counter {
    value: Atomic<i32>
}

procedure get_count(counter: const Counter): i32
    grants atomic.read
{
    result counter.value.load()
}
```

#### 13.3 `grants atomic.write`

**Permits**:
- Performing atomic write operations
- Storing atomic values
- Atomic read-modify-write operations

**Canonical Example**:
```cursive
procedure increment(counter: unique Counter)
    grants atomic.write
{
    counter.value.fetch_add(1)
}

procedure set_value(counter: unique Counter, value: i32)
    grants atomic.write
{
    counter.value.store(value)
}
```

---

### 14. Unsafe Grants

#### 14.1 `grants unsafe.ptr`

**Permits**:
- Using raw pointers
- Pointer arithmetic
- Dereferencing raw pointers
- Casting between pointer types

**Canonical Example**:
```cursive
procedure read_raw(addr: usize): i32
    grants unsafe.ptr
{
    let ptr = addr as *i32
    unsafe {
        result *ptr
    }
}

procedure write_raw(addr: usize, value: i32)
    grants unsafe.ptr
{
    let ptr = addr as *mut i32
    unsafe {
        *ptr = value
    }
}
```

#### 14.2 `grants panic.unwind`

**Permits**:
- Calling panic()
- Unwinding stack
- Triggering program termination

**Canonical Example**:
```cursive
procedure validate_positive(value: i32)
    grants panic.unwind
{
    if value <= 0 {
        panic("Value must be positive")
    }
}
```

---

## PART V: GRANT POLYMORPHISM

### 15. Generic Grant Parameters

**Syntax**: Procedures can be generic over grant sets.

```cursive
procedure with_logging<G>(action: procedure() grants G)
    grants io.write, G
{
    println("Starting action")
    action()
    println("Action complete")
}
```

**Usage**:
```cursive
procedure read_data() grants fs.read {
    // read operations
}

procedure write_data() grants fs.write {
    // write operations
}

procedure main() grants io.write, fs.read, fs.write {
    with_logging(read_data)   // G = {fs.read}
    with_logging(write_data)  // G = {fs.write}
}
```

---

### 16. Grant Bounds

**Syntax**: Grant parameters can have bounds.

```cursive
procedure safe_wrapper<G>(action: procedure() grants G)
    where G extends io.write
    grants G
{
    action()
}
```

**Meaning**: Grant parameter G must include at least `io.write`.

---

### 17. Grant Inference

The compiler infers minimal required grant sets:

```cursive
procedure caller() grants io.write, fs.read {
    // Compiler infers lambda needs {io.write}
    let action = || { println("Test") }

    // Inferred type: procedure() grants io.write
    action()
}
```

---

## PART VI: INTEGRATION WITH CONTRACTS

### 18. Contracts with Grants

**Contract Definition**:
```cursive
contract Serializable {
    procedure serialize(self: const Self): string
        grants alloc.heap
}
```

**Implementation** (CURSIVE SYNTAX - not Rust):
```cursive
record Point: Serializable {
    x: f64
    y: f64

    // Method defined INSIDE record body
    procedure serialize(self: const Self): string
        grants alloc.heap
    {
        let buffer = allocate_string_buffer(64)
        format_into(buffer, "({}, {})", self.x, self.y)
        result buffer_to_string(buffer)
    }
}
```

**Key Points**:
- Contract declares grant requirements
- Implementation must satisfy grant constraints
- Methods defined **inside** record body (not separate `impl` block)
- Use `: ContractName` syntax in record declaration

---

### 19. Multiple Contracts

```cursive
contract Drawable {
    procedure draw(self: const Self)
        grants io.write
}

contract Serializable {
    procedure serialize(self: const Self): string
        grants alloc.heap
}

record Shape: Drawable, Serializable {
    x: f64
    y: f64

    procedure draw(self: const Self)
        grants io.write
    {
        println("Shape at ({}, {})", self.x, self.y)
    }

    procedure serialize(self: const Self): string
        grants alloc.heap
    {
        result format("Shape({}, {})", self.x, self.y)
    }
}
```

---

### 20. Grant Compatibility Rules

**Rule**: Implementation grants must be subset of contract grants.

```cursive
contract Writer {
    procedure write_data(self: const Self)
        grants fs.write, alloc.heap
}

// ✅ OK: Implementation grants ⊆ contract grants
record FileWriter: Writer {
    path: string

    procedure write_data(self: const Self)
        grants fs.write  // {fs.write} ⊆ {fs.write, alloc.heap}
    {
        write_file(self.path, "data")
    }
}

// ❌ ERROR: Implementation grants ⊄ contract grants
record NetworkWriter: Writer {
    url: string

    procedure write_data(self: const Self)
        grants network.write  // {network.write} ⊄ {fs.write, alloc.heap}
    {
        send_http(self.url, "data")
    }
}
```

---

## PART VII: GRANT NARROWING AND SCOPING

### 21. Grant Narrowing

**Principle**: Inner scopes can use fewer grants than declared.

```cursive
procedure complex_operation()
    grants fs.read, fs.write, io.write
{
    // All grants available here

    // Pure computation - uses no grants
    let result = calculate(10, 20)

    // Uses only io.write
    println("Result: {}", result)

    // Uses fs.read and fs.write
    process_file("data.txt")
}
```

**Key Point**: Procedures don't have to use all declared grants - they declare maximum permissions.

---

### 22. Lexical Scoping

Grants are lexically scoped to procedure boundaries:

```cursive
procedure outer()
    grants fs.read, io.write
{
    log("Starting")  // Uses io.write

    {
        // Inner scope has same grants as outer
        let data = read_file("config")  // Uses fs.read
    }

    log("Complete")  // Uses io.write
}
```

---

## PART VIII: STATIC VERIFICATION

### 23. Compile-Time Checking

**Phase 1**: Parse grant clauses
```
For each procedure P:
  Parse grants clause
  Build grant set: grants(P) = {g₁, g₂, ..., gₙ}
```

**Phase 2**: Build call graph
```
For each call site in P₁:
  If P₁ calls P₂:
    Add edge: P₁ → P₂
    Associate required_grants = grants(P₂)
```

**Phase 3**: Verify subset requirements
```
For each edge P₁ → P₂:
  If grants(P₂) ⊄ grants(P₁):
    Report error at call site
```

---

### 24. Error Messages

**Missing Grant**:
```
ERROR[E9001]: Procedure requires grant 'io.write' not in current context
  --> example.cursive:10:5
   |
10 |     println("Hello")
   |     ^^^^^^^^^^^^^^^^ requires grant 'io.write'
   |
note: Current procedure has no grants
   |
help: Add 'grants io.write' to procedure signature:
   |
   | procedure example() grants io.write {
```

**Grant Subset Violation**:
```
ERROR[E9002]: Called procedure requires grants not declared
  --> example.cursive:15:5
   |
15 |     process_file(path)
   |     ^^^^^^^^^^^^^^^^^^ requires grants {fs.read, fs.write}
   |
note: Current procedure grants: {fs.read}
note: Missing grants: {fs.write}
   |
help: Add missing grants to signature:
   |
   | procedure caller() grants fs.read, fs.write {
```

---

## PART IX: GRANT INFERENCE

### 25. Lambda Grant Inference

Grants for lambdas/closures are inferred from body:

```cursive
procedure example() grants io.write {
    // Grant inferred: procedure() grants io.write
    let action = || {
        println("Test")
    }

    action()
}
```

---

### 26. Minimal Grant Sets

Compiler computes minimal required grants:

```cursive
procedure example() grants fs.read, fs.write, io.write {
    // Body only uses io.write
    println("Hello")
}

// Compiler warning: Declared grants {fs.read, fs.write} never used
```

---

## PART X: DESIGN RATIONALE

### 27. Why Compile-Time Only

**Decision**: Grants are purely compile-time annotations.

**Rationale**:
1. **Zero Overhead**: No runtime cost for capability checking
2. **Static Guarantees**: Errors caught at compile time
3. **Performance**: No runtime permission objects or checks
4. **Simplicity**: No runtime capability system to implement

**Trade-off**: Cannot dynamically revoke capabilities (but this aligns with Cursive's static nature).

---

### 28. Why Not Algebraic Effects

**Algebraic Effects Provide**:
- Control flow manipulation
- Suspend/resume computation
- Delimited continuations
- Effect handlers that intercept operations

**Cursive Grants Provide**:
- Permission tracking
- Capability declarations
- Static verification
- Zero runtime overhead

**Why Different Systems**:
- Different goals: control flow vs permissions
- Different implementations: continuations vs static analysis
- Different costs: runtime overhead vs zero cost
- Different use cases: async/generators vs access control

**Comparison Table**:

| Feature | Algebraic Effects | Cursive Grants |
|---------|-------------------|----------------|
| Suspend computation | Yes | No |
| Resume with value | Yes | No |
| Handler interception | Yes | No |
| Runtime overhead | Yes | No |
| Permission tracking | Secondary | Primary |
| Static verification | Optional | Always |
| Implementation | Continuations | Type checking |

---

### 29. Why Explicit Declaration

**Design Choice**: Require explicit `grants` clauses rather than inferring from body.

**Rationale**:
1. **Documentation**: Procedure signature reveals capabilities
2. **Stability**: Adding internal call doesn't change signature
3. **Intent**: Declares maximum permissions, not just current usage
4. **API Clarity**: Callers know requirements without reading implementation

**Example**:
```cursive
// Explicit: Clear from signature
procedure example() grants fs.read {
    // Future: might add fs.read operations
    // Signature already permits this
}

// Inferred: Would need to check body
procedure implicit_example() {
    // Is fs.read allowed? Must read implementation to know
}
```

---

## PART XI: ADVANCED PATTERNS

### 30. Grant Forwarding Pattern

**Pattern**: Wrapper that forwards grants from parameter.

```cursive
procedure with_retry<G>(
    action: procedure() grants G,
    max_attempts: i32
) grants G {
    var attempts = 0
    loop attempts < max_attempts {
        action()
        attempts = attempts + 1
    }
}
```

**Usage**:
```cursive
procedure read_file() grants fs.read { /* ... */ }

procedure main() grants fs.read {
    with_retry(read_file, 3)  // Forwards fs.read grant
}
```

---

### 31. Grant Union Pattern

**Pattern**: Combining multiple grant requirements.

```cursive
procedure log_and_save(message: string)
    grants io.write, fs.write
{
    println("Saving: {}", message)
    write_file("log.txt", message)
}
```

---

### 32. Conditional Grants Pattern

**Pattern**: Different code paths use different grants.

```cursive
enum OutputMode {
    Console,
    File { path: string }
}

procedure output(mode: OutputMode, message: string)
    grants io.write, fs.write
{
    match mode {
        OutputMode::Console => {
            println(message)  // Uses io.write
        },
        OutputMode::File { path } => {
            write_file(path, message)  // Uses fs.write
        }
    }
}
```

---

## PART XII: MIGRATION FROM "EFFECTS" TERMINOLOGY

### 33. Historical Context

**Original Name**: "Effects" with `uses` keyword
**Current Name**: "Grants" with `grants` keyword

**Reason for Change**: Avoid confusion with algebraic effects from other languages.

**Timeline**:
- Pre-2025-11: Used `uses effect_name` syntax
- 2025-11: Renamed to `grants grant_name` syntax
- Specification update: In progress

**Example Migration**:
```cursive
// Old syntax (deprecated):
procedure old_example() uses io.write {
    println("Hello")
}

// New syntax (current):
procedure new_example() grants io.write {
    println("Hello")
}
```

---

## PART XIII: FUTURE EXTENSIONS

### 34. Planned Features

**Version 1.1**:
- Grant hierarchies (e.g., `fs` implies `fs.read` and `fs.write`)
- Grant parameterization (e.g., `fs.read<"/tmp">` for path restrictions)
- Grant algebra (intersection, union operations)

**Version 1.2**:
- User-defined grants
- Grant inheritance
- Grant composition operators

**Version 2.0**:
- First-class grant objects (capability-passing style)
- Dynamic grant checking (optional runtime mode)
- Grant reflection

---

## CONCLUSION

This document specifies Cursive's complete grants system. Key achievements:

1. ✅ **Compile-time capability tracking** - Static verification only
2. ✅ **Zero runtime overhead** - No runtime cost
3. ✅ **Explicit permissions** - Clear from signatures
4. ✅ **Local reasoning** - Understand code without whole-program analysis
5. ✅ **Compositional** - Grants flow through call chains
6. ✅ **Fine-grained** - Individual permissions for specific operations
7. ✅ **Not algebraic effects** - Different goals and mechanisms

**Next Steps**: Formalize into specification sections (Part IX), update grammar, migrate terminology.

---

**END OF GRANTS SYSTEM SPECIFICATION**
