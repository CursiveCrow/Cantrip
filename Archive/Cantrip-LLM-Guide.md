# The Cantrip LLM Onboarding Guide

**Version:** 1.0.0 (compatible with 0.7.x)  
**Date:** October 21, 2025  
**Status:** Informative Guide  
**Target Audience:** Large Language Models and AI coding assistants

---

## Preface

This document provides comprehensive onboarding and alignment guidance for Large Language Models (LLMs) generating Cantrip code. It includes:

- **Quick Reference:** Core syntax patterns with correct/incorrect examples
- **Common Idioms:** Idiomatic Cantrip code patterns
- **Critical Differences:** How Cantrip differs from similar languages
- **Common Mistakes:** Error codes indexed by frequent LLM mistakes

This guide is **informative** and supplements the normative language specification. For complete language semantics, refer to Cantrip-Language-Spec-1.0.0.md.

### Document Structure

- **Part I: LLM Quick Reference** - Core syntax patterns and idioms
- **Part II: Common LLM Mistakes** - Error codes indexed by frequent mistakes

### Cross-References

- **Main Specification:** Cantrip-Language-Spec-1.0.0.md
- **Standard Library:** Cantrip-Standard-Library-Spec.md
- **Tooling Guide:** Cantrip-Tooling-Guide.md
- **Error Codes:** Cantrip-Error-Codes.md

---

# Part I: LLM Quick Reference

**(Extracted from Part 0 of the main specification)**

**Purpose:** This section provides a crash course for Large Language Models generating Cantrip code. It demonstrates correct syntax patterns with anti-patterns, common idioms, and critical differences from similar languages.

**For Human Readers:** This section supplements but does not replace the formal specification. Read Part I-XIII for complete language semantics.

---

## 0.1 Core Syntax Patterns

### 0.1.1 Function Declaration Syntax

**Canonical Form (ONLY valid syntax):**
```cantrip
function name(param: Type): ReturnType
    needs effects;        // Optional: omit if pure
    requires conditions;  // Optional: omit if none
    ensures conditions;   // Optional: omit if none
{
    body
}
```

**✅ CORRECT Example:**

```cantrip
// Comprehensive function with all optional clauses
function divide(a: f64, b: f64): f64
    needs alloc.heap;          // Optional: effects
    requires b != 0.0;          // Optional: precondition
    ensures result == a / b;    // Optional: postcondition
{
    a / b
}

// Pure functions omit all clauses
function add(x: i32, y: i32): i32 { x + y }
```

**❌ WRONG - Removed Syntax (ERROR E1010, E1011, E1012):**

```cantrip
// ERROR E1010: Old syntax removed
function example()
    requires<alloc.heap>:  // ❌ WRONG - use 'needs alloc.heap;'
{ }

// ERROR E1011: Old syntax removed
function example() returns<i32>:  // ❌ WRONG - use ': i32' in signature
{ }

// ERROR E1012: Old syntax removed
function example()
    implements:  // ❌ WRONG - just write the body
{ }
```

### 0.1.2 Ownership and Move Semantics

**Key Principle:** Ownership transfer requires explicit `move` keyword.

**✅ CORRECT Example:**

```cantrip
// Explicit move required for ownership transfer
function consume(own data: Data) {
    // data owned and destroyed here
}

function example()
    needs alloc.heap;
{
    let data = Data.new();
    consume(move data);  // Explicit move required
    // data no longer accessible
}
```

**❌ WRONG - Missing Move:**

```cantrip
function consume(own data: Data) { }

function broken()
    needs alloc.heap;
{
    let data = Data.new();
    consume(data);  // ERROR E3004: missing 'move'
}
```

**Critical Difference from Rust:**

```rust
// Rust: Move is implicit
fn consume(data: Data) { }
fn example() {
    let data = Data::new();
    consume(data);  // Implicit move in Rust
    // data no longer valid
}
```

```cantrip
// Cantrip: Move is explicit
function consume(own data: Data) { }
function example() {
    let data = Data.new();
    consume(move data);  // Explicit move required
    // data no longer valid
}
```

### 0.1.3 Effect Declaration Syntax

**Key Principle:** Effects are NEVER inferred. They MUST be declared explicitly, otherwise the item is pure.

**✅ CORRECT Example:**

```cantrip
// Effect declarations
function process_file(path: String): Result<Data, Error>
    needs fs.read, fs.write, alloc.heap;  // Declare all effects
{
    let contents = std.fs.read_to_string(path)?;
    std.fs.write("output.txt", contents)
}

// Pure function needs no clause
function calculate(x: i32): i32 { x * x }
```

**❌ WRONG - Missing Effect Declaration:**

```cantrip
function broken(): Vec<i32> {
    Vec.new()  // ERROR E9001: alloc.heap not declared
}
```

**Effect Hierarchy:**

```
alloc.*          // All allocation types
  ├─ alloc.heap
  ├─ alloc.stack
  ├─ alloc.region
  └─ alloc.temp

fs.*             // All file system
  ├─ fs.read
  ├─ fs.write
  ├─ fs.delete
  └─ fs.metadata

net.*            // All network
  ├─ net.read(inbound)
  ├─ net.read(outbound)
  └─ net.write
```

### 0.1.4 Modal State Transitions

**Syntax:** `@State >> procedure() >> @NextState`

**✅ CORRECT Examples:**

```cantrip
modal File {
    state Closed {
        path: String;
    }

    state Open {
        path: String;
        handle: FileHandle;
    }

    // Transition uses >> with @ markers (modal)
    @Closed >> open() >> @Open
        needs fs.read;
    {
        let handle = FileSystem.open(self.path)?;
        Open {
            path: self.path,
            handle: handle,
        }
    }

    @Open >> read(n: usize) >> @Open
        needs fs.read;
    {
        let data = self.handle.read(n)?;
        (data, self)
    }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { path: self.path }
    }
}

// Usage
function example(): Result<(), Error>
    needs fs.read;
{
    let file = File.new("data.txt");  // Type: File@Closed

    // Cannot read in Closed state
    // let data = file.read(1024)?;  // ERROR E3003

    let file = file.open()?;          // Now File@Open
    let data = file.read(1024)?;      // OK in Open state
    let file = file.close();          // Back to File@Closed

    Ok(())
}
```

**❌ WRONG - Invalid Syntax:**

```cantrip
modal Broken {
    state Start { }
    state End { }

    // ❌ ERROR E1006: Wrong arrow syntax
    Start -> procedure() -> End { }     // Wrong: use >> not ->
    @Start => procedure() => @End { }   // Wrong: use >> not =>
    Start.procedure() -> End { }        // Wrong: missing @
}
```

### 0.1.5 Region-Based Memory Management

**Key Principle:** Regions provide O(1) bulk deallocation with LIFO ordering.

**✅ CORRECT Example:**

```cantrip
// Basic region usage
function parse_data(input: String): Result<Data, Error>
    needs alloc.region, alloc.heap;
{
    region temp {
        let tokens = lex_in<temp>(input);
        let ast = parse_in<temp>(tokens);
        Ok(ast.to_heap())  // Convert before escaping
    }
}
```

**❌ WRONG - Escaping Region:**

```cantrip
function broken(): Vec<i32>
    needs alloc.region;
{
    region temp {
        let vec = Vec.new_in<temp>();
        vec  // ERROR E5001: Cannot return region data
    }
}
```

---

## 0.2 Common Patterns Library

### 0.2.1 Error Handling Pattern

**Pattern: Early Return with ?**

```cantrip
function pipeline(path: String): Result<Output, Error>
    needs fs.read, alloc.heap;
    requires !path.is_empty();
{
    let raw = read_file(path)?;        // Early return on error
    let parsed = parse(raw)?;          // Chain cleanly
    let validated = validate(parsed)?; // No nesting
    Ok(transform(validated))
}
```

**Pattern: Custom Error Types**

```cantrip
enum ProcessError {
    IoError(std.io.Error),
    ParseError(String),
    ValidationError(String),
}

function process(path: String): Result<Data, ProcessError>
    needs fs.read, alloc.heap;
{
    let contents = std.fs.read_to_string(path)
        .map_err(|e| ProcessError.IoError(e))?;

    let parsed = parse(contents)
        .map_err(|e| ProcessError.ParseError(e))?;

    validate(parsed)
        .map_err(|e| ProcessError.ValidationError(e))
}
```

### 0.2.2 Resource Management Pattern

**Pattern: RAII with Modals**

```cantrip
modal DatabaseConnection {
    state Disconnected { config: Config; }
    state Connected { handle: DbHandle; }

    @Disconnected >> connect() >> @Connected
        needs net.read(outbound);
    {
        let handle = establish_connection(self.config)?;
        Connected { handle }
    }

    @Connected >> disconnect() >> @Disconnected {
        self.handle.close();
        Disconnected { config: self.config }
    }
}

function with_connection<F, T>(config: Config, f: F): Result<T, Error>
    where F: Fn(DatabaseConnection@Connected): Result<T, Error>
    needs net.read(outbound);
{
    let conn = DatabaseConnection.new(config);
    let conn = conn.connect()?;
    let result = f(conn)?;
    conn.disconnect();
    Ok(result)
}
```

### 0.2.3 Effect Composition Pattern

**Pattern: Named Effect Definitions**

```cantrip
// Define at module level
effect WebService = fs.read + fs.write + net.read(inbound) + net.write + alloc.heap;
effect GameTick = alloc.temp;  // Only temp allocation, everything else denied by default
effect DatabaseOps = fs.read + fs.write + alloc.heap;

// Use in function signatures
function handle_request(req: Request): Response
    needs WebService;
{
    let data = load_from_disk()?;
    let processed = transform(data);
    send_response(processed)
}

function update_game_state(state: mut GameState, dt: f32)
    needs GameTick;
{
    region frame {
        let temp_data = process_frame_in<frame>(state, dt);
        apply_changes(state, temp_data);
    }
}
```

### 0.2.4 Contract Pattern

**Pattern: Bounds Checking**

```cantrip
function safe_index<T>(arr: [T], index: usize): Option<T>
    ensures
        result.is_some() == (index < arr.length());
        result.is_some() implies result.unwrap() == arr[index];
{
    if index < arr.length() {
        Some(arr[index])
    } else {
        None
    }
}
```

**Pattern: State Preservation**

```cantrip
function transfer_funds(
    from: mut Account,
    to: mut Account,
    amount: f64
)
    requires
        amount > 0.0,
            "Transfer amount must be positive";
        from.balance >= amount,
            "Insufficient funds: need {amount}, have {from.balance}";
    ensures
        from.balance == @old(from.balance) - amount;
        to.balance == @old(to.balance) + amount;
        @old(from.balance + to.balance) == (from.balance + to.balance);
{
    from.balance -= amount;
    to.balance += amount;
}
```

---

## 0.3 Language Comparison Guide

### 0.3.1 For Rust Developers

**Key Differences:**

| Feature | Rust | Cantrip |
|---------|------|---------|
| **Borrow Checker** | Yes - enforces at compile time | No - programmer's responsibility |
| **Move Semantics** | Implicit by default | Explicit `move` keyword required |
| **Lifetimes** | Explicit `'a` annotations | Implicit via regions |
| **Multiple Mut Refs** | Compile error | Allowed (programmer ensures safety) |
| **Memory Model** | Ownership + borrowing | Ownership + regions |

**Example Comparison:**

```rust
// Rust: Multiple mutable borrows forbidden
fn rust_example() {
    let mut data = Vec::new();
    let r1 = &mut data;  // First mutable borrow
    let r2 = &mut data;  // ERROR: cannot borrow as mutable twice
}
```

```cantrip
// Cantrip: Multiple mutable references allowed
function cantrip_example()
    needs alloc.heap;
{
    var data = Vec.new();
    let r1 = mut data;  // Mutable reference
    let r2 = mut data;  // OK - no borrow checker
    // Programmer ensures r1 and r2 don't conflict
}
```

**What Cantrip Does NOT Prevent:**

- ❌ Aliasing bugs (multiple mut refs can conflict)
- ❌ Data races (no compile-time enforcement)
- ❌ Iterator invalidation

**What Cantrip DOES Prevent:**

- ✅ Use-after-free (regions enforce LIFO)
- ✅ Double-free (regions handle cleanup)
- ✅ Memory leaks (deterministic destruction)

### 0.3.2 For Python Developers

**Key Differences:**

| Feature | Python | Cantrip |
|---------|--------|---------|
| **Types** | Dynamic, optional | Static, required |
| **Effects** | Implicit (any function can do anything) | Explicit (must declare) |
| **Memory** | Garbage collected | Manual with regions |
| **Errors** | Exceptions | Result types |

**Example Comparison:**

```python
# Python: Implicit everything
def process(path):
    return open(path).read()  # Hidden: fs.read, alloc, exceptions
```

```cantrip
// Cantrip: Explicit everything
function process(path: String): Result<String, Error>
    needs fs.read, alloc.heap;  // Explicit effects
    requires !path.is_empty();   // Explicit preconditions
{
    std.fs.read_to_string(path)  // Explicit error handling
}
```

### 0.3.3 For C++ Developers

**Key Differences:**

| Feature | C++ | Cantrip |
|---------|-----|---------|
| **RAII** | Yes (implicit destructors) | Yes (via modals/regions) |
| **Smart Pointers** | std::unique_ptr, std::shared_ptr | `own`, regions |
| **Move** | std::move() | `move` keyword |
| **Memory Safety** | Undefined behavior possible | Compile-time prevention |

**Example Comparison:**

```cpp
// C++: RAII with unique_ptr
std::unique_ptr<File> file = std::make_unique<File>("data.txt");
file->read();
// Automatic cleanup via destructor
```

```cantrip
// Cantrip: RAII with modals
modal File {
    state Open { handle: FileHandle; }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { }
    }
}

function example()
    needs fs.read;
{
    let file = File.new("data.txt").open()?;
    file.read()?;
    file.close();  // Explicit but required by type system
}
```

---

## 0.4 Standard Library Quick Reference

### 0.4.1 Core Types

```cantrip
// Option<T> - Nullable values
let some: Option<i32> = Some(42);
let none: Option<i32> = None;

match some {
    Some(value) -> println(value),
    None -> println("no value"),
}

// Result<T, E> - Error handling
let ok: Result<i32, String> = Ok(42);
let err: Result<i32, String> = Err("failed");

match ok {
    Ok(value) -> println(value),
    Err(error) -> println(error),
}

// Vec<T> - Dynamic array
let mut numbers = Vec.new();  // needs alloc.heap
numbers.push(1);
numbers.push(2);
let first = numbers[0];

// String - UTF-8 string
let text = String.from("hello");  // needs alloc.heap
let length = text.length();
```

### 0.4.2 Standard Effects

```cantrip
import std.effects.{
    Pure,          // No effects
    SafeIO,        // fs.read + fs.write + alloc.heap
    WebService,    // fs.* + net.* + alloc.heap
    GameTick,      // alloc.temp + !alloc.heap + !io.*
    Deterministic, // !time.* + !random
};
```

### 0.4.3 Common Operations

```cantrip
// File I/O
function read_file(path: String): Result<String, Error>
    needs fs.read, alloc.heap;
{
    std.fs.read_to_string(path)
}

function write_file(path: String, data: String): Result<(), Error>
    needs fs.write;
{
    std.fs.write(path, data)
}

// Console I/O
function print(msg: String)
    needs io.write;
{
    std.io.println(msg);
}

// Collections
function create_map<K, V>(): HashMap<K, V>
    where K: Hash + Eq
    needs alloc.heap;
{
    HashMap.new()
}
```

---

## 0.5 Error Code Quick Reference

| Code | Category | Description |
|------|----------|-------------|
| **E1010** | Syntax | Removed: `requires<effects>` (use `needs effects;`) |
| **E1011** | Syntax | Removed: `returns<Type>` (use `: Type`) |
| **E1012** | Syntax | Removed: `implements:` (just write body) |
| **E2001** | Type | Type mismatch |
| **E3003** | Modal | Procedure not available in current state |
| **E3004** | Modal | Use of moved value |
| **E5001** | Region | Cannot return region data |
| **E9001** | Effect | Missing effect declaration |
| **E9002** | Effect | Forbidden effect used |
| **E9010** | Effect | Redundant forbidden effect (use only with wildcards/polymorphic) |

**Most Common Errors:**

1. **Missing effect declaration** (E9001)
   ```cantrip
   // ERROR:
   function f() { Vec.new() }
   // FIX:
   function f() needs alloc.heap { Vec.new() }
   ```

2. **Missing move keyword** (E3004)
   ```cantrip
   // ERROR:
   consume(data)  // where consume takes `own data`
   // FIX:
   consume(move data)
   ```

3. **Modal state error** (E3003)
   ```cantrip
   // ERROR:
   file.read()  // when file is File@Closed
   // FIX:
   let file = file.open()?;
   file.read()
   ```

---

**END OF PART 0**

*This crash course provides the essential patterns for LLM code generation. For complete formal semantics, see Parts I-XIII.*

---

## Abstract

Cantrip is a systems programming language designed for memory safety, deterministic performance, and AI-assisted development. It achieves these goals through:

- **Lexical Permission System (LPS)**: Compile-time memory safety without garbage collection or borrow checking
- **Explicit Contracts**: Preconditions and postconditions as executable specifications
- **Effect System**: Compile-time tracking of side effects, allocations, and I/O
- **Modal System**: State machines as first-class types with compile-time verification
- **Memory Regions**: Explicit lifetime control with zero-overhead allocation
- **File-Based Modules**: Code organization through file system structure

Cantrip compiles to native code with performance matching C/C++ while providing memory safety guarantees through region-based lifetime management.

**Design Philosophy:**
1. **Explicit over implicit** - All effects, lifetimes, and permissions visible in code
2. **Local reasoning** - Understanding code requires minimal global context
3. **Zero abstraction cost** - Safety guarantees without runtime overhead
4. **LLM-friendly** - Predictable patterns for AI code generation
5. **Simple ownership** - No borrow checker complexity

**Safety Model:**
- ✅ **Prevents**: Use-after-free, double-free, memory leaks
- ✅ **Provides**: Deterministic deallocation, zero GC pauses
- ❌ **Does NOT prevent**: Aliasing bugs, data races (programmer's responsibility)

**Conformance:** An implementation conforms to this specification if and only if it satisfies all normative requirements stated herein.

---

## Table of Contents

### Part 0: LLM Quick Reference
0.1. [Core Syntax Patterns](#01-core-syntax-patterns)
0.2. [Common Patterns Library](#02-common-patterns-library)
0.3. [Language Comparison Guide](#03-language-comparison-guide)
0.4. [Standard Library Quick Reference](#04-standard-library-quick-reference)
0.5. [Error Code Quick Reference](#05-error-code-quick-reference)

### Part I: Foundational Concepts
1. [Notation and Mathematical Foundations](#1-notation-and-mathematical-foundations)
2. [Lexical Structure](#2-lexical-structure)
3. [Abstract Syntax](#3-abstract-syntax)

### Part II: Type System
4. [Types and Values](#4-types-and-values)
5. [Type Rules and Semantics](#5-type-rules-and-semantics)
6. [Records](#6-structs-and-classes)
7. [Enums and Pattern Matching](#7-enums-and-pattern-matching)
8. [Traits](#8-traits-and-traits)
9. [Generics and Parametric Polymorphism](#9-generics-and-parametric-polymorphism)

    - 9.6. [Const Generics](#96-const-generics)

### Part III: Modal System
10. [Modals: State Machines as Types](#10-modals-state-machines-as-types)
11. [Modal Formal Semantics](#11-modal-formal-semantics)
12. [State Transition Verification](#12-state-transition-verification)

### Part IV: Functions and Expressions
13. [Functions and Procedures](#13-functions-and-procedures)
14. [Expressions and Operators](#14-expressions-and-operators)
15. [Control Flow](#15-control-flow)
16. [Higher-Order Functions and Closures](#16-higher-order-functions-and-closures)

### Part V: Contract System
17. [Contracts and Specifications](#17-contracts-and-specifications)
18. [Contract Formal Logic](#18-contract-formal-logic)
19. [Invariants](#19-invariants)
20. [Verification and Testing](#20-verification-and-testing)

### Part VI: Effect System
21. [Effects and Side Effects](#21-effects-and-side-effects)
22. [Effect Rules and Checking](#22-effect-rules-and-checking)
23. [Effect Budgets](#23-effect-budgets)
24. [Effect Soundness](#24-effect-soundness)

### Part VII: Memory Management
25. [Lexical Permission System](#25-lexical-permission-system)
26. [Permission Types and Rules](#26-permission-types-and-rules)
27. [Ownership and Transfer](#27-ownership-and-transfer)
28. [Memory Regions](#28-memory-regions)
29. [Region Formal Semantics](#29-region-formal-semantics)

### Part VIII: Module System
30. [Modules and Code Organization](#30-modules-and-code-organization)
31. [Import and Export Rules](#31-import-and-export-rules)
32. [Visibility and Access Control](#32-visibility-and-access-control)
33. [Module Resolution](#33-module-resolution)

### Part IX: Advanced Features
34. [Compile-Time Programming](#34-compile-time-programming)
35. [Concurrency](#35-concurrency)
36. [Actors (First-Class Type)](#36-actors-and-message-passing)
37. [Async/Await](#37-asyncawait)

    - 37.1. Async Functions
    - 37.2. Await Expressions
    - 37.3. Select Expression
    - 37.4. Async Iteration
    - 37.5. Async Effect Masks (Informative)

### Part X: Operational Semantics
38. [Small-Step Semantics](#38-small-step-semantics)
39. [Big-Step Semantics](#39-big-step-semantics)
40. [Memory Model](#40-memory-model)
41. [Evaluation Order](#41-evaluation-order)

### Part XI: Soundness and Properties
42. [Type Soundness](#42-type-soundness)
43. [Effect Soundness](#43-effect-soundness)
44. [Memory Safety](#44-memory-safety)
45. [Modal Safety](#45-modal-safety)

### Part XII: Standard Library (See Separate Document)
**📄 See:** `Cantrip-Standard-Library-Spec.md` for complete API documentation

### Part XIII: Tooling and Implementation (See Separate Document)
**📄 See:** `Cantrip-Tooling-Guide.md`

### Part XIV: Foreign Function Interface
56. [Overview](#56-overview)
57. [Declarations and Linkage](#57-declarations-and-linkage)
58. [Type Mappings](#58-type-mappings)
59. [Ownership and Allocation Across FFI](#59-ownership-and-allocation-across-ffi)
60. [Callbacks from C into Cantrip](#60-callbacks-from-c-into-cantrip)
61. [Errors and Panics](#61-errors-and-panics)
62. [Inline Assembly (Reserved)](#62-inline-assembly-reserved)

### Appendices (Informative)
- [A: Complete Grammar](#appendix-a-complete-grammar)
- [B: Keywords and Operators](#appendix-b-keywords-and-operators)
- [C: Error Codes (See Separate Document)](#appendix-c-error-codes-see-separate-document)
- [D: Standard Library Index (See Separate Document)](#appendix-d-standard-library-index-see-separate-document)
- [E: Formal Proofs](#appendix-e-formal-proofs)
- [F: Error Codes by Common LLM Mistakes](#appendix-f-informative-error-codes-indexed-by-common-llm-mistakes)

---


---

# Part II: Common LLM Mistakes

**(Extracted from Appendix F of the main specification)**

This appendix groups canonical diagnostics by mistakes frequently observed in LLM‑generated code.

**1. Missing effects**  
- `E9001` — missing effect declaration  
- `E9201` — async effect mask exposes missing `needs` (net.*, time.sleep) (§22.7)  
Fix‑its: add required `needs ...;` in the function signature.

**2. Ownership & moves**  
- `E3004` — value moved (missing `move`)  
- `E5001` — region data escapes its region (returning region‑allocated value)  
Fix‑its: add `move`, convert to heap via `.to_heap()`, or adjust region scope.

**3. Modals & states**  
- `E3003` — procedure not available in current state  
Fix‑its: perform the required state transition first.

**4. Traits & coherence**  
- `E9120` — trait implementation exceeds declared method effects (§8.6)  
- `E8201` — overlapping impls; `E8202` — orphan impl (§8.7)  
Fix‑its: narrow effects or relocate impl to a local crate/type.

**5. Typed holes & stubs**  
- `E9501` — unfilled typed hole stops compilation; `E9502` — dev‑mode trap emitted (§14.10)  
Fix‑its: fill the hole or accept a generated skeleton.

**6. FFI hazards**  
- `E9701` — calling extern without `ffi.call`  
- `E9704` — region‑allocated value crossing FFI boundary  
Fix‑its: add effects, use `#[repr(C)]`, convert region data to heap, provide free functions.

**7. Structured concurrency**  
- `E7801` — escaping/detached `JoinHandle` from `scope.spawn` (§35.5)  
- `W7800` — implicit joins at scope end  
Fix‑its: hold and join every handle within the scope.

