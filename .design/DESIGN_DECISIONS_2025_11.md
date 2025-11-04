# Cursive Language Design Decisions
**Session Date**: November 2025
**Status**: Design Discussion Summary
**Authors**: Language Design Team

---

## Executive Summary

This document records critical design decisions reached during a comprehensive review of Cursive's memory model, pointer system, and capability tracking system. Key outcomes:

1. **System 3 Memory Model Preserved** - The two-axis design (3 binding categories × 3 permissions = 9 combinations) is retained without modification
2. **Pointer Types Clarified** - Distinction between pointer **types** (`Ptr<T>`) and binding **categories** (`let`/`var`/`ref`) formalized
3. **"Effects" Renamed to "Grants"** - Terminology change to prevent confusion with algebraic effects from other languages
4. **Borrowing Explicitly Rejected** - Rust-style borrowing concepts do not fit Cursive's design philosophy
5. **External Review Evaluated** - Third-party proposals assessed against design goals; several rejected

---

## 1. Memory Model: System 3 Analysis and Decisions

### 1.1 Current System 3 Overview

Cursive's System 3 memory model uses a **two-axis design** that orthogonally separates:

**Axis 1: Binding Categories** (Who cleans up?)
- `let` - Responsible for cleanup, non-rebindable, transferable via `move`
- `var` - Responsible for cleanup, rebindable, non-transferable
- `ref` - Not responsible for cleanup, non-rebindable, non-transferable

**Axis 2: Permissions** (What operations allowed? Aliasing control?)
- `const` - Immutable, unlimited aliasing allowed
- `unique` - Mutable, exclusive access, compiler-enforced
- `shared` - Mutable, aliasing allowed, programmer-coordinated

**Result**: 9 valid combinations (3 × 3 matrix)

### 1.2 Design Principles Affirmed

The following core principles guided all decisions:

1. **Explicit over implicit** - All ownership, permissions, and capabilities visible in code
2. **Local reasoning** - Understanding code requires minimal global context
3. **Zero runtime overhead** - No garbage collector, no runtime reference counting
4. **Predictable patterns** - Consistent syntax for AI/LLM code generation
5. **No borrow checker** - No lifetime annotations (`'a`), no complex borrow rules
6. **Deterministic cleanup** - RAII with lexical scoping

### 1.3 Evaluated and Rejected: Fourth Binding Category

**Proposal (External Review)**: Add `ptr` as a fourth binding category for "non-responsible + rebindable" pointers.

**Example of Proposed Syntax**:
```cursive
ptr p: i32 = &x;  // Non-owning, rebindable pointer
p = &y;           // Can reassign
// No destructor called for p
```

**Rationale for Rejection**:
1. ❌ **Breaks 3×3 Design** - Would create 12 combinations instead of 9
2. ❌ **Violates Orthogonality** - `ptr` would be special-case, not orthogonal
3. ❌ **Unnecessary** - Can already use `var p: Ptr<T>` (pointer type with var binding)
4. ❌ **Adds Complexity** - Teaching burden increases without commensurate benefit

**Alternative Solution**: Use pointer **types** with existing binding categories (see Section 2).

**Decision**: **REJECTED** - Preserve 3×3 design. No fourth binding category.

### 1.4 Evaluated and Rejected: Move from `var`

**Proposal (External Review)**: Allow `move` from `var` bindings "when safe" (when not used after move).

**Example of Proposed Behavior**:
```cursive
var x = Resource();
process(move x);  // Would be legal if x not used after
// x is now invalid
```

**Rationale for Rejection**:
1. ❌ **Violates Local Reasoning** - Legality of `move` depends on code arbitrarily far below
2. ❌ **Non-Local Analysis Required** - Must scan entire function to validate move
3. ❌ **Contradicts Design Goal** - "Understanding code requires minimal global context"

**Example of Problem**:
```cursive
var x = Resource();
process(move x);  // Legal or not? Depends on line 100!
// ... 95 lines of code ...
if condition {
    x.use();  // This line 100 lines down determines legality above
}
```

**Decision**: **REJECTED** - `var` remains non-transferable. Only `let` can be moved.

### 1.5 Evaluated and Rejected: Borrowing Concepts

Multiple proposals attempted to introduce Rust-style borrowing under different names:

**Proposals Rejected**:
1. ❌ **Fractional Permissions** - "Split unique into multiple const views"
   - This is borrowing with different terminology
   - Example: `split readonly { share1, share2 } from data { ... }`

2. ❌ **Borrow Tokens** - "Lifetime tokens for iterator safety"
   - Explicitly uses "borrow" keyword
   - Example: `borrow token from vec { ... }`

3. ❌ **Disjoint Borrows** - "Non-overlapping slice borrowing"
   - Still fundamentally borrowing
   - Example: `let view1 = data[0..500]; let view2 = data[500..1000];`

4. ❌ **Temporary Borrowing** - Any concept of "lending and returning"
   - Requires lifetime tracking (borrow checker)

**Rationale for Rejection**:
- **Cursive uses `ref`, not borrows** - Different semantic model
- **No lifetime annotations** - Design goal explicitly excludes `'a` style lifetimes
- **Region-based scoping instead** - Lexical scopes determine validity, not borrow checker
- **Borrowing requires borrow checker** - Which Cursive explicitly rejects

**What Cursive Has Instead**:
- `ref` bindings - Non-owning references with lexical lifetime
- Regions - Arena allocation with lexical scoping
- `move` - Explicit ownership transfer
- Permissions - `const`/`unique`/`shared` control aliasing

**Decision**: **REJECTED** - All borrowing-style concepts incompatible with Cursive's design.

### 1.6 Affirmed: System 3 Remains Unchanged

**Decision**: System 3's two-axis, 3×3 design is retained without modification. It successfully achieves:
- Memory safety without garbage collection ✓
- Memory safety without borrow checker ✓
- Explicit ownership and permissions ✓
- Zero runtime overhead ✓
- Deterministic cleanup ✓

---

## 2. Pointer Types: Clarification and Formalization

### 2.1 The Confusion

A critical distinction was clarified between:
- **Pointer TYPES** (e.g., `Ptr<T>`, `*T`) - What the value is
- **Binding CATEGORIES** (e.g., `let`, `var`, `ref`) - How the binding behaves

These are **orthogonal** and **composable**.

### 2.2 Existing Pointer Types in Cursive

Cursive has **two pointer type systems**:

#### 2.2.1 Safe Pointers: `Ptr<T>@State`

**Modal state-tracked pointers** with compile-time safety:

```cursive
// Create pointer with @Valid state
let ptr: Ptr<i32>@Valid = Ptr::new(&x);

// Can dereference only in @Valid state
*ptr;  // ✓ OK: Compiler verified @Valid

// Modal state transitions
let null_ptr: Ptr<i32>@Null = Ptr::null();
*null_ptr;  // ✗ ERROR: Cannot dereference @Null pointer
```

**Properties**:
- Zero runtime overhead (state = compile-time only)
- States: `@Valid`, `@Null`, `@Dangling`
- Type system prevents dereferencing invalid pointers

#### 2.2.2 Raw Pointers: `*T` and `*mut T`

**Unsafe pointers** for FFI and low-level code:

```cursive
// Raw immutable pointer
let raw: *i32 = &raw x;

// Raw mutable pointer
let raw_mut: *mut i32 = &raw mut y;

// Dereference requires unsafe block
unsafe {
    *raw;      // OK in unsafe block
    *raw_mut = 42;
}
```

**Properties**:
- Requires `unsafe { }` blocks
- Requires `unsafe.ptr` grant (see Section 3)
- No safety guarantees
- Used for FFI, low-level systems code

### 2.3 Pointer Types × Binding Categories

Pointer types **compose** with binding categories:

```cursive
// let binding + pointer type
let p: Ptr<i32>@Valid = Ptr::new(&x);
// p is responsible for cleanup (calls destructor)
// p cannot be reassigned
// p can be moved via explicit `move p`

// var binding + pointer type
var p: Ptr<i32>@Valid = Ptr::new(&x);
// p is responsible for cleanup
// p CAN be reassigned: p = Ptr::new(&y);
// p cannot be moved (var is non-transferable)

// ref binding + pointer type
ref p: Ptr<i32>@Valid = some_pointer;
// p is NOT responsible for cleanup
// p cannot be reassigned
// p cannot be moved

// Raw pointer with let
let raw: *i32 = &raw x;
// Owned raw pointer (responsible for cleanup if needed)

// Raw pointer with var
var raw: *mut i32 = &raw mut x;
// Rebindable raw pointer: raw = &raw mut y;
```

### 2.4 Why No `ptr` Binding Category is Needed

**External review proposed**: `ptr` binding for "non-responsible + rebindable"

**Why it's unnecessary**:
```cursive
// What review wanted:
ptr p: i32 = &x;
p = &y;  // Rebindable, non-owning

// Already achievable with var + Ptr type:
var p: Ptr<i32>@Valid = Ptr::new(&x);
p = Ptr::new(&y);  // Rebindable, Ptr handles cleanup
```

**If truly need non-owning rebindable**: This is an edge case better handled by:
1. Using `var` with a smart pointer type
2. Manual management in `unsafe` blocks for extreme low-level code
3. Recognizing this pattern is rare and doesn't justify breaking 3×3

**Decision**: Pointer **types** with existing binding **categories** provide all needed combinations. No new binding category required.

---

## 3. Grant System: Terminology and Semantics

### 3.1 The Confusion: "Effects" Overloaded Term

**Problem Identified**: Cursive uses "effects" to mean **capability tokens** (compile-time permission tracking), but this term is heavily overloaded:

1. **Algebraic Effects** (OCaml, Koka, Eff) - Control flow constructs with suspend/resume
2. **Side Effects** (General PL) - Any observable state change
3. **Effect Systems** (Type theory) - Tracking computational effects in types
4. **Cursive "Effects"** - Capability tokens for permission tracking

**Example of Confusion**:
```ocaml
(* OCaml algebraic effects - control flow *)
effect Ask : string -> int

let compute () =
    let x = perform (Ask "gimme") in  (* Suspends execution *)
    x + 1

let result =
    try compute () with
    | effect (Ask msg) k -> continue k 42  (* Resumes with value *)
```

vs

```cursive
// Cursive "effects" - capability tokens
procedure read_file(path: string) uses fs.read {
    // Just declares: "this procedure may read filesystem"
    // No control flow changes, no suspension, no handlers
}
```

**These are fundamentally different concepts!**

### 3.2 Algebraic Effects vs Cursive Capabilities

| Aspect | Algebraic Effects (OCaml) | Cursive Capabilities |
|--------|---------------------------|---------------------|
| **Purpose** | Control flow abstraction | Permission tracking |
| **Runtime** | Suspend/resume execution | Zero overhead |
| **Implementation** | Delimited continuations | Compile-time only |
| **Provides** | Async, generators, exceptions | Access control |
| **Example** | `perform (Read "file")` | `grants fs.read` |
| **Type system** | Effect polymorphism | Capability checking |

### 3.3 Relationship to Traits and Contracts

**Observation**: Algebraic effects (without continuation magic) are similar to traits/contracts:

**Algebraic Effects (simplified)**:
1. Define abstract operations
2. Implementations provide those operations
3. (Optionally) continuation-based control flow

**Cursive Traits**:
1. Define procedure signatures ✓
2. Provide default implementations ✓
3. No continuation magic (not needed)

**Cursive Contracts**:
1. Define abstract procedure signatures ✓
2. Types implement the contract ✓
3. No continuation magic

**Conclusion**: Cursive already has the "abstract operations + implementations" part via traits/contracts. What Cursive calls "effects" is something entirely different (capability tokens).

### 3.4 Decision: Rename to "Grants"

**New Terminology**:
- **Old**: "effects" and `uses` keyword
- **New**: "grants" and `grants` keyword

**Rationale**:
1. **Clearer semantics** - "This procedure is **granted** permission to X"
2. **Avoids confusion** - No overlap with algebraic effects
3. **More accurate** - Describes capability-based security model
4. **Better verb** - "grants" reads naturally ("grants fs.read")

**Updated Syntax**:
```cursive
// Before (old terminology)
procedure read_file(path: string) uses fs.read {
    // ...
}

// After (new terminology)
procedure read_file(path: string) grants fs.read {
    // ...
}
```

**What Grants Do**:
```cursive
// Built-in grants (catalog to be completed in spec)
grants fs.read          // May read from filesystem
grants fs.write         // May write to filesystem
grants io.read          // May read from I/O
grants io.write         // May write to I/O (stdout, stderr)
grants alloc.heap       // May allocate on heap
grants alloc.region     // May allocate in regions
grants thread.spawn     // May spawn threads
grants atomic.read      // May perform atomic reads
grants atomic.write     // May perform atomic writes
grants unsafe.ptr       // May use raw pointers
grants panic.unwind     // May panic and unwind

// Procedure with multiple grants
procedure process_data(path: string)
    grants fs.read, alloc.heap, io.write
{
    let data = read_file(path);  // OK: has fs.read
    let processed = process(data);  // OK: has alloc.heap
    println("{}", processed);  // OK: has io.write
}

// Pure procedure (no grants)
function calculate(x: i32, y: i32): i32 {
    // Cannot perform I/O, allocation, or other side effects
    result x + y
}
```

**Compile-Time Checking**:
```cursive
function pure_function(x: i32): i32 {
    println("Debug: {}", x);  // ✗ ERROR: No io.write grant
    result x * 2
}

procedure allowed(x: i32) grants io.write {
    println("Debug: {}", x);  // ✓ OK: Has io.write grant
}
```

**Grant Propagation**:
```cursive
procedure helper() grants fs.read {
    // Can read files
}

procedure caller() grants fs.read {
    helper();  // ✓ OK: caller has fs.read, can call helper
}

function pure_caller() {
    helper();  // ✗ ERROR: No fs.read grant
}
```

**Decision**: Rename "effects" to "grants" throughout specification. Update keyword from `uses` to `grants`.

### 3.5 What Grants Are NOT

To prevent future confusion, explicitly document what grants are **not**:

❌ **Not algebraic effects** - No suspend/resume, no continuations
❌ **Not effect handlers** - No runtime interception of operations
❌ **Not side effect tracking** - Just permission declarations
❌ **Not runtime checks** - Compile-time only verification
❌ **Not capabilities in E/OCap sense** - No first-class capability objects (yet)

✅ **Grants are**: Compile-time capability tokens that restrict what operations a procedure may perform.

---

## 4. External Review Evaluation

An external document proposing memory model changes was evaluated. Key findings:

### 4.1 Valid Criticisms Identified

✅ **Concurrency Missing** - Part XIII is 0% specified (10-line placeholder)
✅ **Iterator Invalidation Possible** - `shared` allows concurrent modification during iteration
✅ **Teaching Complexity** - 9 combinations is conceptually dense
✅ **Documentation Needs Improvement** - Diagnostics and guides can be better

### 4.2 Factual Errors in Review

❌ **"Implicit downgrades don't exist"** - FALSE. Spec already has automatic `unique → const` and `shared → const` coercion (CURSIVE_MEMORY_MODEL_SPECIFICATION.md, Part III, lines 1008-1077)

❌ **"Need copyable types"** - Unclear if this already exists; needs verification

### 4.3 Invalid Proposals (Violate Design Goals)

❌ **`ptr` binding category** - Breaks 3×3 design, unnecessary given pointer types
❌ **Move from `var`** - Violates "local reasoning" principle
❌ **Borrowing concepts** - Incompatible with "no borrow checker" goal
❌ **Fractional permissions** - Borrowing in disguise
❌ **Temporary exclusivity** - Requires borrow checker analysis

### 4.4 Proposals to Consider

The review did highlight areas worth exploring (in future work):

1. **Concurrency model** - Critical gap, must be addressed
2. **Better diagnostics** - Implementation quality, not language design
3. **Permission polymorphism** - Potentially useful (see Section 5)
4. **Copy trait** - If doesn't exist, may be useful for trivial types

---

## 5. Recommended Enhancements (Future Work)

Based on this analysis, the following enhancements are recommended for future specification work:

### 5.1 Critical (v1.1): Concurrency Foundation

**Must specify**:
1. Memory model (happens-before, data races, SC-DRF semantics)
2. Thread-safety traits (`Send`/`Sync` auto-traits)
3. Atomic types with memory ordering
4. Thread spawn semantics

**Example**:
```cursive
// Send: Can transfer ownership across threads
// Sync: Can share references across threads

record Point { x: f64, y: f64 }  // Auto Send + Sync

record RefCell<T> { ... }
impl !Send for RefCell<T> {}  // Opt-out

procedure spawn<F>(f: F) grants thread.spawn
    where F: Send + FnOnce()
```

### 5.2 High Priority (v1.2): Permission Polymorphism

**Proposed feature**: Generic over permissions

```cursive
// Generic permission parameter P
procedure process<T, P: Permission>(data: P T): usize
    where P >= const  // At least const permission
{
    result data.len()  // len() only needs const
}

// Usage
let const_vec: const Vec<i32> = vec![1, 2, 3];
let unique_vec: unique Vec<i32> = vec![4, 5, 6];

process(const_vec);   // P = const
process(unique_vec);  // P = unique (coerces to const in body)
```

**Benefits**:
- Reduces code duplication
- Library functions can accept any permission
- Type-safe permission abstraction

**Concerns**:
- Adds complexity to type system
- May not be needed if APIs choose one permission consistently

**Decision**: Propose for v1.2, evaluate actual need in practice

### 5.3 Medium Priority (v1.3): Complete Const Generics

**Current status**: Mentioned in spec but not fully specified

**Needed**:
```cursive
record Array<T, const N: usize> {
    data: [T; N],
}

procedure multiply<const N: usize, const M: usize, const P: usize>(
    a: Matrix<N, M>,
    b: Matrix<M, P>
) -> Matrix<N, P>
    where N > 0, M > 0, P > 0
{
    // Matrix multiplication
}
```

### 5.4 Low Priority (v2.0): Static Refinement Types

**Optional enhancement**: Compile-time contract verification

```cursive
procedure abs(x: i32) -> i32
    ensures result >= 0  // Compiler can verify
{
    result if x < 0 { -x } else { x }
}
```

**Benefits**: Eliminate redundant runtime checks
**Cost**: Requires SMT solver integration (optional, implementation-defined)
**Decision**: Defer to v2.0, not critical

### 5.5 COMPLETED (v1.1): Field-Level Partitioning

**Status**: ✅ **Specification Complete** - See [Field-Level-Partitioning.md](06_Memory-Model-and-Permissions/Field-Level-Partitioning.md)

**Problem Addressed**: The `shared` permission allows multiple mutable aliases with programmer-coordinated safety. This created an acknowledged safety gap:
- ❌ **Iterator invalidation possible** (line 433 of this document)
- ❌ **Accessing same field twice through `shared` can cause bugs**
- ❌ **No compile-time prevention of aliasing hazards**

**Solution: Field-Level Partitioning**

A compile-time mechanism that prevents accessing the same field multiple times through `shared` references in the same scope, while allowing access to different fields (the common, safe pattern).

**Syntax:**
```cursive
record World {
    // Default: Each field is its own partition
    entities: Vec<Entity>,
    positions: Vec<Position>,

    // Explicit grouping: These fields must not be accessed together
    <<Physics>>
    velocities: Vec<Velocity>,
    forces: Vec<Force>,
}

procedure update(world: shared World) {
    ref pos: shared = world.positions;   // ✅ OK
    ref vel: shared = world.velocities;  // ✅ OK: Different partition

    ref pos2: shared = world.positions;  // ❌ ERROR: Same field already accessed
    ref force: shared = world.forces;    // ❌ ERROR: Physics partition already active
}
```

**Key Features:**
1. **Default behavior**: Each field is automatically its own partition (prevents same-field double-access)
2. **Explicit grouping**: Use `<<PartitionName>>` directive to group related fields for mutual exclusion
3. **Zero overhead**: Pure compile-time checking, no runtime cost
4. **Opt-in safety**: Works automatically with safe defaults, add partitions for custom grouping
5. **ECS perfect fit**: Naturally supports Entity Component System patterns (primary use case)

**Design Principles Met:**
- ✅ **Explicit over implicit**: Partition boundaries visible via `<<Partition>>` directives
- ✅ **Local reasoning**: All partition assignments in record definition
- ✅ **Zero runtime overhead**: Compile-time only
- ✅ **No borrow checker**: Not borrowing, just compile-time partition tracking
- ✅ **Practical power**: Prevents real bugs without excessive restrictions
- ✅ **No lifetime annotations**: Lexical scoping only

**Why Field-Level vs. Alternatives:**

| Approach | Expressiveness | Power | Cursive Fit | Status |
|----------|---------------|-------|-------------|--------|
| **Field-Level Partitioning** | Moderate | Prevents field-level bugs | ✅ Excellent | **CHOSEN** |
| **Indexed Partitioning** | Very High | Prevents element-level bugs | ❌ Too complex | Rejected |
| **Phantom Views** | Low | Operation-level only | ❌ Too close to borrowing | Rejected |
| **Borrowing/Lifetimes** | Very High | Full safety | ❌ Violates "no borrow checker" | Rejected |

**Implementation Status:**
- ✅ Complete formal specification created
- ✅ Grammar formalized (partition directives)
- ✅ Semantics defined (activation, conflict detection, scoping)
- ✅ Error codes defined (E4024, E4025, E4026)
- ✅ Integration with existing memory model documented
- ✅ Lexical syntax added (`<<`, `>>` tokens)

**Benefits for Cursive:**
1. **Closes safety gap**: Addresses iterator invalidation concern
2. **Maintains design philosophy**: No borrowing, no lifetimes, no borrow checker
3. **Low learning curve**: Simple "each field is separate" default
4. **Game dev optimized**: Perfect for ECS patterns
5. **Progressive enhancement**: Add partitions incrementally where needed

**Decision**: ✅ **ADOPTED** - Field-level partitioning is now part of Cursive v1.1 specification.

---

## 6. Specification Work Required

Based on these decisions, the following specification work is identified:

### 6.1 Immediate (No Breaking Changes)

1. **Clarify pointer types vs binding categories** (Section 2)
   - Add to Part I (Terms and Definitions)
   - Add examples to Part VI (Memory Model)

2. **Document existing implicit coercion** (Already exists, just formalize)
   - Part VI should explicitly show `unique → const`, `shared → const`

3. **Complete Part XIII: Concurrency** (Currently empty)
   - Memory model
   - Send/Sync traits
   - Atomic types
   - Thread primitives

### 6.2 Proposed Breaking Change (Requires Migration)

**Rename "effects" to "grants"**:
- Change keyword: `uses` → `grants`
- Update all spec references: "effect system" → "grant system"
- Update Part IX title: "Effect System" → "Grant System"
- Migration tool: Automated find-replace `uses` → `grants`

**Justification**: Eliminates fundamental terminology confusion, worth the one-time cost

### 6.3 Future Additions (Opt-In Features)

- Permission polymorphism (v1.2)
- Const generics completion (v1.3)
- Static refinement types (v2.0)

---

## 7. Principles Reaffirmed

This analysis reaffirms Cursive's core design principles:

1. ✅ **Explicit over implicit** - All ownership, permissions, grants visible
2. ✅ **Local reasoning** - No non-local analysis required (rejected var moves)
3. ✅ **Zero runtime overhead** - No GC, no RC, no runtime checks
4. ✅ **No borrow checker** - Rejected all borrowing-style proposals
5. ✅ **Deterministic cleanup** - RAII with lexical scoping
6. ✅ **Orthogonal design** - Keep binding categories separate from permissions
7. ✅ **Simple model** - 3×3 matrix, not 4×3 or more complex

**The two-axis System 3 memory model successfully achieves memory safety without compromise to these principles.**

---

## 8. Summary of Decisions

| Decision | Status | Rationale |
|----------|--------|-----------|
| Preserve System 3 (3×3) | ✅ **AFFIRMED** | Achieves goals, no need to change |
| Reject `ptr` binding category | ❌ **REJECTED** | Breaks 3×3, pointer types sufficient |
| Reject move from `var` | ❌ **REJECTED** | Violates local reasoning |
| Reject borrowing concepts | ❌ **REJECTED** | Incompatible with no-borrow-checker goal |
| Rename "effects" to "grants" | ✅ **APPROVED** | Eliminates confusion with algebraic effects |
| Change keyword `uses` → `grants` | ✅ **APPROVED** | Follows terminology change |
| Add concurrency model | ✅ **REQUIRED** | Critical gap in specification |
| Add permission polymorphism | 🔄 **PROPOSED** | Useful feature, needs design work |
| Complete const generics | 🔄 **PROPOSED** | Already mentioned, needs formalization |
| Add static refinement types | 🔄 **DEFERRED** | Optional enhancement for v2.0 |

---

## 9. Next Steps

1. **Immediate**:
   - Update Part I (Terms and Definitions) to clarify pointer types vs binding categories
   - Document existing permission coercion formally

2. **Short-term (v1.1)**:
   - Write Part XIII: Grant System and Concurrency
   - Specify Send/Sync traits
   - Specify atomic types and memory model

3. **Medium-term (v1.2)**:
   - Design permission polymorphism system
   - Complete const generics specification

4. **Breaking change consideration**:
   - Decide on timeline for `uses` → `grants` transition
   - Provide migration tooling
   - Update all examples in specification

5. **Long-term (v2.0+)**:
   - Static refinement type verification
   - Advanced grant system features
   - Tooling improvements (diagnostics, lints)

---

## 10. Conclusion

This design discussion successfully:

✅ Validated System 3's two-axis memory model
✅ Rejected proposals incompatible with design goals
✅ Clarified pointer types vs binding categories
✅ Identified and corrected terminology confusion
✅ Established roadmap for critical missing features

**Cursive's memory model remains sound, principled, and ready for formalization of concurrency support.**

---

**End of Design Decisions Document**
