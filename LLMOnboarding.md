# Cursive Language: LLM Onboarding Guide

## System Prompt Version

This document provides comprehensive guidance for Large Language Models generating Cursive code. Cursive is a statically typed, systems-level language designed for AI-assisted code generation, strict memory safety without garbage collection, and capability-based security.

---

## 1. DESIGN PHILOSOPHY AND CORE PRINCIPLES

### 1.1 Primary Design Goals

Cursive is explicitly designed for reliable AI code generation. Adhere to these principles:

1. **ONE CORRECT WAY**: For any given task, there should be one obviously correct approach. Avoid ambiguity.

2. **STATIC BY DEFAULT**: All behavior is static. Mutability and side-effects are opt-in only.

3. **SELF-DOCUMENTING CODE**: Write code whose correct use is evident; incorrect use should generate errors.

4. **MEMORY SAFETY WITHOUT COMPLEXITY**: Achieve memory safety without garbage collection or complex borrow checking.

5. **LOCAL REASONING**: Any code fragment should be understandable with minimal global context.

6. **ZERO-COST ABSTRACTIONS**: Compile-time safety guarantees with no runtime overhead.

7. **NO AMBIENT AUTHORITY**: All side effects require explicit capability parameters—no global I/O functions exist.

### 1.2 Target Use Cases

- Systems programming (OS kernels, device drivers)
- Performance-critical applications
- Real-time systems with hard timing constraints
- Embedded development
- Network services and infrastructure
- AI-generated production code requiring high reliability

---

## 2. THE ORTHOGONAL MEMORY MODEL

Cursive's memory model is **orthogonal**: it separates two independent concepts that other languages conflate.

### 2.1 Two Independent Axes

| Axis         | Concept                         | Mechanism                                       |
| ------------ | ------------------------------- | ----------------------------------------------- |
| **Liveness** | Who is responsible for cleanup? | Responsibility (RAII) + `move` keyword          |
| **Aliasing** | How can data be accessed?       | Permission System (`const`, `unique`, `shared`) |

**Critical Rule**: These axes are completely independent. A `const` binding can have cleanup responsibility; a `unique` binding may not. The `move` keyword transfers responsibility, not permission.

### 2.2 Responsibility and Move Semantics

Every resource has exactly one **responsible binding**. Destruction is deterministic via `Drop::drop`.

```cursive
// Binding operators determine movability
let x = value       // Movable: responsibility can transfer via `move`
let x := value      // Immovable: responsibility permanently fixed to x
var x = value       // Mutable + movable
var x := value      // Mutable + immovable (reassignment allowed, not move)

// Explicit move transfers responsibility
let source = Resource::new()
let destination = move source   // source is now INVALID (Moved state)
// Accessing source here is a compile-time error
```

**Parameter Forms**:
```cursive
procedure borrow(data: T)           // Caller retains responsibility
procedure consume(move data: T)     // Callee takes responsibility (caller must use `move`)
```

### 2.3 The Permission Lattice

Permissions form a strict hierarchy:

```
    unique (~!)      Exclusive read-write, no other aliases allowed
       ↓
    shared (~%)      Synchronized aliased mutability (key-protected)
       ↓
     const (~)       Immutable, unlimited aliases (DEFAULT)
```

**Permission Rules**:

| Permission | Abbreviation | Access           | Aliasing  | Use Case              |
| ---------- | ------------ | ---------------- | --------- | --------------------- |
| `const`    | `~`          | Read-only        | Unlimited | Default, safe sharing |
| `unique`   | `~!`         | Read-write       | None      | Exclusive mutation    |
| `shared`   | `~%`         | Key-synchronized | With sync | Concurrent access     |

**Coexistence Matrix**:
| Active | +unique | +shared | +const |
| ------ | ------- | ------- | ------ |
| unique | ✗       | ✗       | ✗      |
| shared | ✗       | ✓       | ✓      |
| const  | ✗       | ✓       | ✓      |

**Method Receiver Shorthand**:
```cursive
procedure read(~)           // self: const Self
procedure mutate(~!)        // self: unique Self
procedure synchronized(~%)  // self: shared Self
```

---

## 3. MODAL TYPES: STATE MACHINES IN THE TYPE SYSTEM

Modal types embed state machines directly into types, enforced at compile time.

### 3.1 Declaration Syntax

```cursive
modal Connection {
    @Disconnected {
        // State-specific payload fields
        last_error: string | None,
    } {
        // State-specific methods and transitions
        transition connect(~!, host: string, timeout: Duration) -> @Connecting {
            // Must return Connection@Connecting
            Connection@Connecting { host, started: Time::now() }
        }
    }
    
    @Connecting {
        host: string,
        started: Time,
    } {
        transition complete(~!, socket: Socket) -> @Connected {
            Connection@Connected { socket, host: self.host }
        }
        
        transition fail(~!, error: string) -> @Disconnected {
            Connection@Disconnected { last_error: Some(error) }
        }
    }
    
    @Connected {
        socket: Socket,
        host: string,
    } {
        procedure send(~!, data: const [u8]) -> usize | IoError {
            self.socket.write(data)
        }
        
        transition disconnect(~!) -> @Disconnected {
            self.socket.close()
            Connection@Disconnected { last_error: None }
        }
    }
}
```

### 3.2 Type Usage

```cursive
// State-specific types enforce valid operations
let conn: Connection@Disconnected = Connection@Disconnected { last_error: None }
let connecting = conn.connect("example.com", Duration::seconds(30))
// conn is now INVALID (consumed by transition)

// General modal type requires pattern matching
procedure handle(conn: Connection) {
    match conn {
        @Disconnected { last_error } => { /* handle disconnected */ }
        @Connecting { host, started } => { /* handle connecting */ }
        @Connected { socket, host } => { /* handle connected */ }
    }
}
```

### 3.3 Key Modal Type Rules

1. **State-specific types are incomparable**: `Connection@Disconnected` cannot convert to `Connection@Connected`
2. **Transitions consume the receiver**: The old state binding becomes invalid
3. **Methods are state-specific**: Calling `.send()` on `@Disconnected` is a compile error
4. **Match must be exhaustive**: All states must be handled

### 3.4 Built-in Modal Types

**`Ptr<T>`** - Safe pointers with compile-time null safety:
```cursive
modal Ptr<T> {
    @Valid   // Non-null, dereferenceable
    @Null    // Guaranteed null
    @Expired // Was valid, now deallocated (compile-time only)
}

let ptr: Ptr<Buffer>@Valid = &buffer
let value = *ptr              // OK: @Valid is dereferenceable
let null_ptr: Ptr<Buffer>@Null = Ptr::null()
// let bad = *null_ptr        // COMPILE ERROR: @Null not dereferenceable
```

**`string`** - Modal string type:
```cursive
modal string {
    @Managed  // Owned heap buffer, has Drop
    @View     // Non-owning slice, Copy
}

let literal: string@View = "hello"  // Static storage
let owned: string@Managed = literal.to_managed(ctx.heap)
let view: string@View = owned.as_view()
```

---

## 4. OBJECT-CAPABILITY SECURITY MODEL

### 4.1 No Ambient Authority

Cursive has **NO global side-effect functions**. All I/O, allocation, and system interaction requires explicit capabilities.

```cursive
// WRONG: No such global function exists
print("Hello")              // Does not exist
File::open("/path/to/file") // Does not exist

// CORRECT: Capabilities are explicit parameters
procedure main(ctx: Context) -> i32 {
    // ctx contains all root capabilities
    ctx.fs.write_stdout("Hello\n")
    
    let file = ctx.fs.open("/path/to/file")?
    let data = ctx.heap.allocate::<[u8; 1024]>()
    
    result 0
}
```

### 4.2 The Context Record

```cursive
record Context {
    fs: witness FileSystem,      // File I/O capability
    net: witness Network,        // Network capability
    sys: System,                 // System operations
    heap: witness HeapAllocator  // Heap allocation capability
}

public procedure main(ctx: Context) -> i32
```

### 4.3 Capability Propagation

Procedures performing effects must accept the required capability:

```cursive
procedure write_log(message: string, fs: witness FileSystem) {
    let file = fs.open_append("/var/log/app.log")?
    file.write(message.as_view())?
}

procedure allocate_buffer(size: usize, heap: witness HeapAllocator) -> Ptr<[u8]>@Valid {
    heap.allocate_bytes(size)
}
```

---

## 5. CONCURRENCY MODEL

### 5.1 Parallel Blocks (CREW: Concurrent Read, Exclusive Write)

```cursive
parallel {
    spawn { /* task 1 */ }
    spawn { /* task 2 */ }
}
// Blocks until all spawned tasks complete
```

**Capture Rules**:
- `unique T` where `T: Sync` → `shared T` (synchronized access)
- `unique T` where `T: !Sync` → `const T` (read-only)

### 5.2 The Key System

The `shared` permission uses an implicit **key system** for fine-grained synchronization:

```cursive
let player: shared Player = Player::new()

parallel {
    spawn { player.health = 100 }   // Key: player.health
    spawn { player.mana = 50 }      // Key: player.mana (DISJOINT - parallel OK)
}

// Same field requires synchronization
parallel {
    spawn { player.health += 10 }   // Write key on player.health
    spawn { player.health += 20 }   // Waits for first to release
}
```

### 5.3 Coarsening Operator (`#`)

Control key granularity for atomicity:

```cursive
// Fine-grained (default): separate keys per field
player.health = 100  // Key: player.health
player.mana = 50     // Key: player.mana

// Coarse-grained: single key for atomic update
#player {
    let total = player.health + player.mana
    player.health = total / 2
    player.mana = total / 2
}

// Inline coarsening
#player.stats.health = 100  // Key: player (not player.stats.health)
```

### 5.4 Compound Access (Required for Read-Modify-Write)

```cursive
// WRONG: Read-then-write on shared path
player.health = player.health + 10   // COMPILE ERROR E-KEY-0060

// CORRECT: Compound assignment
player.health += 10

// CORRECT: Atomic block for complex logic
atomic player.health { |h| max(h - damage, 0) }
```

---

## 6. TYPE SYSTEM ESSENTIALS

### 6.1 Primitive Types

```cursive
// Integers
i8, i16, i32, i64, i128      // Signed
u8, u16, u32, u64, u128      // Unsigned
isize, usize                  // Pointer-sized

// Floats
f16, f32, f64

// Other
bool                          // true, false
char                          // Unicode scalar value
()                            // Unit type
!                             // Never type (bottom)
```

### 6.2 Composite Types

```cursive
// Tuples (structural)
let point: (f64, f64) = (1.0, 2.0)
let x = point.0

// Arrays (fixed size)
let arr: [i32; 5] = [1, 2, 3, 4, 5]

// Slices (dynamic size, borrowed view)
let slice: [i32] = arr[1..4]

// Records (nominal product types)
record Point { x: f64, y: f64 }
let p = Point { x: 1.0, y: 2.0 }

// Enums (nominal sum types)
enum Option<T> {
    Some(T),
    None
}

// Union types (structural sum types)
let result: i32 | Error = compute()
match result {
    n: i32 => use(n),
    e: Error => handle(e)
}
```

### 6.3 Union Types and Error Handling

```cursive
// Union types for errors
procedure read_file(path: string, fs: witness FileSystem) -> string | IoError | NotFound {
    let file = fs.open(path)?      // Propagates IoError, NotFound
    file.read_all()?
}

// Propagation operator (?)
procedure process(path: string, fs: witness FileSystem) -> Result | IoError | ParseError {
    let content = read_file(path, fs)?   // Propagates errors automatically
    parse(content)?
}
```

---

## 7. DECLARATIONS AND PROCEDURES

### 7.1 Procedure Declaration

```cursive
// Full syntax
[visibility] procedure name[<generics>](parameters) -> ReturnType
    [|= precondition]
    [=> postcondition]
{
    body
}

// Examples
public procedure add(a: i32, b: i32) -> i32 {
    result a + b
}

procedure divide(a: i32, b: i32) -> i32 | DivByZero
    |= b != 0  // Precondition
{
    result a / b
}
```

### 7.2 Parameter Passing

```cursive
procedure example(
    readonly: const Buffer,       // Read-only access, caller keeps responsibility
    exclusive: unique Buffer,     // Exclusive access, caller keeps responsibility
    consumed: move Buffer,        // Caller transfers responsibility
    sync_access: shared Buffer    // Synchronized access
) { /* ... */ }

// Call site
example(buf1, buf2, move buf3, buf4)
//                   ^^^^ Required for move parameters
```

### 7.3 Return Statements

```cursive
procedure compute() -> i32 {
    if condition {
        return 42           // Early return
    }
    result expensive()      // Final result (tail position)
}
```

---

## 8. CONTROL FLOW

### 8.1 If Expressions

```cursive
let value = if condition { 
    expression1 
} else { 
    expression2 
}

if condition {
    statement1
} else if other_condition {
    statement2
} else {
    statement3
}
```

### 8.2 Match Expressions

```cursive
match value {
    pattern1 => expression1,
    pattern2 if guard => expression2,
    _ => default_expression
}

// Pattern types
match data {
    0 => "zero",                           // Literal
    1..10 => "small",                      // Range
    Point { x, y: 0 } => "on x-axis",     // Record destructure
    Option::Some(inner) => use(inner),     // Enum variant
    @Connected { socket } => use(socket),  // Modal state
    _ => "other"                           // Wildcard
}
```

### 8.3 Loops

```cursive
// Range loop
loop i in 0..10 {
    process(i)
}

// Conditional loop
loop condition {
    body
}

// Infinite loop (must break or return)
loop {
    if done { break }
}

// With labels
'outer: loop i in 0..10 {
    loop j in 0..10 {
        if found { break 'outer }
    }
}
```

---

## 9. GENERICS AND TRAITS

### 9.1 Generic Types

```cursive
record Container<T> {
    value: T
}

procedure identity<T>(x: T) -> T {
    result x
}
```

### 9.2 Trait Definition and Implementation

```cursive
trait Printable {
    procedure print(~, output: witness FileSystem)
}

record Point { x: f64, y: f64 } <: Printable {
    procedure print(~, output: witness FileSystem) {
        output.write_stdout(f"({self.x}, {self.y})")
    }
}
```

### 9.3 Trait Bounds

```cursive
procedure process<T>(value: T) where T: Printable + Clone {
    let copy = value.clone()
    copy.print(output)
}
```

### 9.4 Important Built-in Traits

| Trait   | Purpose              | Auto-Derive        |
| ------- | -------------------- | ------------------ |
| `Copy`  | Bitwise copyable     | If all fields Copy |
| `Clone` | Explicit duplication | Manual             |
| `Drop`  | Destructor           | Manual             |

---

## 10. REGION-BASED ALLOCATION

### 10.1 Region Blocks

```cursive
// Arena allocation with deterministic cleanup
region arena {
    let a = ^arena Point { x: 1.0, y: 2.0 }  // Allocate in arena
    let b = ^arena Buffer::new(1024)
    
    process(a, b)
}  // All arena allocations freed here (LIFO)

// Nested regions
region outer {
    let data = ^outer Data::new()
    region inner {
        let temp = ^inner compute(data)
    }  // temp freed
    continue_with(data)
}  // data freed
```

---

## 11. METAPROGRAMMING

### 11.1 Compile-Time Evaluation

```cursive
// Compile-time constants
comptime {
    let TABLE: [i32; 256] = generate_lookup_table()
}

// Compile-time conditionals
comptime if TARGET_ARCH == "x86_64" {
    // x86_64-specific code
} else {
    // Generic fallback
}
```

### 11.2 Quote and Emit

```cursive
// AST manipulation
comptime {
    let ast = quote {
        procedure generated() -> i32 { result 42 }
    }
    emit ast
}
```

---

## 12. ERROR HANDLING PATTERNS

### 12.1 Union-Based Errors

```cursive
// Define error types
record IoError { message: string, code: i32 }
record ParseError { message: string, line: usize }

// Return union of success and errors
procedure load_config(path: string, fs: witness FileSystem) -> Config | IoError | ParseError {
    let content = fs.read_file(path)?
    parse_config(content)?
}
```

### 12.2 Propagation Pattern

```cursive
procedure process_all(paths: [string], fs: witness FileSystem) -> Results | IoError | ParseError {
    var results = Vec::new(ctx.heap)
    
    loop path in paths {
        let config = load_config(path, fs)?   // Propagates on error
        results.push(process(config))
    }
    
    result results
}
```

---

## 13. CODE GENERATION GUIDELINES

### 13.1 Always Follow

1. **Explicit Types**: Always annotate top-level declarations
2. **Explicit Move**: Use `move` keyword for all ownership transfers
3. **No Globals**: Pass capabilities explicitly; never assume ambient authority
4. **Permission Honesty**: Use appropriate permission for actual access pattern
5. **State Transitions**: Ensure modal type transitions are logically valid
6. **Exhaustive Matching**: Handle all enum/modal/union variants

### 13.2 Common Patterns

```cursive
// Resource management with move
procedure with_file<T>(
    path: string,
    fs: witness FileSystem,
    action: procedure(file: unique File) -> T
) -> T | IoError {
    let file = fs.open(path)?
    let result = action(file)
    // file automatically dropped here
    result result
}

// Builder pattern with modal types
modal RequestBuilder {
    @Building { headers: Map<string, string> }
    @Ready { request: Request }
}

// Error accumulation
procedure validate(data: Data) -> Validated | Vec<ValidationError> {
    var errors = Vec::new()
    
    if !valid_name(data.name) {
        errors.push(ValidationError::InvalidName)
    }
    if !valid_age(data.age) {
        errors.push(ValidationError::InvalidAge)
    }
    
    if errors.is_empty() {
        result Validated::from(data)
    } else {
        result errors
    }
}
```

### 13.3 Anti-Patterns to Avoid

```cursive
// WRONG: Accessing moved binding
let a = Resource::new()
let b = move a
use(a)  // COMPILE ERROR: a is in Moved state

// WRONG: Mutating through const
procedure bad(data: const Buffer) {
    data.clear()  // COMPILE ERROR: mutation through const path
}

// WRONG: Assuming global I/O
procedure bad() {
    print("hello")  // COMPILE ERROR: print doesn't exist
}

// WRONG: Forgetting move at call site
procedure consume(move data: Buffer) { /* ... */ }
consume(buffer)       // COMPILE ERROR: missing `move`
consume(move buffer)  // CORRECT

// WRONG: Non-exhaustive match
match option {
    Option::Some(v) => use(v)
    // COMPILE ERROR: missing Option::None arm
}
```

---

## 14. STANDARD PATTERNS

### 14.1 Entry Point

```cursive
public procedure main(ctx: Context) -> i32 {
    match run(ctx) {
        () => result 0,
        e: Error => {
            ctx.fs.write_stderr(f"Error: {e.message}\n")
            result 1
        }
    }
}

procedure run(ctx: Context) -> () | Error {
    let config = load_config(ctx.fs)?
    let result = process(config, ctx)?
    save_result(result, ctx.fs)?
}
```

### 14.2 RAII Pattern

```cursive
record MutexGuard<T> <: Drop {
    mutex: shared Mutex<T>,
    
    procedure drop(~!) {
        self.mutex.unlock()
    }
}

procedure with_lock<T, R>(mutex: shared Mutex<T>, action: procedure(unique T) -> R) -> R {
    let guard = mutex.lock()  // Returns MutexGuard
    let result = action(guard.value)
    // guard.drop() called automatically
    result result
}
```


---

## 15. QUICK REFERENCE

### Keywords
```
let, var, procedure, record, enum, modal, trait, match, if, else,
loop, break, continue, return, result, move, const, unique, shared,
parallel, spawn, dispatch, region, comptime, quote, emit, unsafe,
public, internal, private, protected, import, use, as, where,
witness, transition, atomic, defer, widen
```

### Operators
```
// Arithmetic: + - * / % **
// Comparison: == != < <= > >=
// Logical: && || !
// Bitwise: & | ^ << >>
// Assignment: = += -= *= /= %= &= |= ^= <<= >>=
// Special: ? (propagation), # (coarsening), ^ (region alloc), & (address-of), * (deref)
// Method: . (field/method), ~> (method call syntax)
```

### Permission Shorthands
```
~   → const Self (immutable receiver)
~!  → unique Self (exclusive receiver)
~%  → shared Self (synchronized receiver)
```

### Modal Type Syntax
```
Type@State        → State-specific type
@State { ... }    → Modal pattern match
transition ... -> @NewState  → State transition declaration
widen expr        → Widen state-specific to general type
```

---

## 16. DIAGNOSTIC CODES (Common)

| Code       | Meaning                                 |
| ---------- | --------------------------------------- |
| E-MEM-3001 | Access to moved binding                 |
| E-MEM-3003 | Reassignment of immutable binding       |
| E-MEM-3006 | Move from immovable binding             |
| E-TYP-1601 | Mutation through const path             |
| E-TYP-1602 | Unique permission violation (aliasing)  |
| E-TYP-2052 | Invalid state field access              |
| E-TYP-2053 | Invalid state method invocation         |
| E-TYP-2060 | Non-exhaustive modal match              |
| E-KEY-0010 | Dynamic index key conflict              |
| E-KEY-0060 | Read-then-write without compound access |

---

*End of Cursive LLM Onboarding Guide*