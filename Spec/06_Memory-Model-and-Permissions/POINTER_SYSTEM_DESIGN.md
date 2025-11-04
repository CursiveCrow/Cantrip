# Cursive Pointer and Reference System - Design Document

**Status**: Approved Design
**Date**: 2025-01-04
**Version**: 1.0

---

## Executive Summary

This document specifies the complete design for Cursive's pointer and reference system, including:

- **Three binding categories**: `let`, `var`, `alias`
- **Region allocation operator**: `^expr` returns value type `T`
- **Alias bindings**: Pure syntactic aliases with no memory location
- **Modal pointers**: `Ptr<T>` with four states (@Null, @Valid, @Weak, @Expired)
- **Address-of operator**: `&expr` for creating pointers
- **Dereference operator**: `*ptr` for accessing values through pointers
- **Escape analysis**: Compile-time prevention of dangling pointers

This design achieves memory safety through:
1. Compile-time escape analysis (no runtime overhead)
2. Modal type states (null safety, weak reference tracking)
3. Explicit operations (no implicit pointer creation)
4. Integration with Cursive's permission system

---

## 1. Design Principles

The pointer system adheres to Cursive's core principles:

| Principle | Application |
|-----------|-------------|
| **Explicit over implicit** | Pointer creation requires explicit `&expr` with declared type |
| **Memory safety** | Escape analysis prevents dangling pointers at compile time |
| **Zero-cost abstractions** | All safety checks are compile-time, zero runtime overhead |
| **LLM-friendly** | Clear, unambiguous semantics with position-based parsing |
| **Move semantics** | Pointers follow move-by-default semantics |
| **Permission integration** | Pointers work with const/unique/shared permissions |

---

## 2. Binding Categories

Cursive defines three binding category keywords:

### 2.1 Binding Category Table

| Keyword | Ownership | Has Location? | Mutability | Purpose |
|---------|-----------|---------------|------------|---------|
| `let` | Owning | ✅ Yes | Immutable | Immutable owning binding |
| `var` | Owning | ✅ Yes | Mutable | Mutable owning binding |
| `alias` | Non-owning | ❌ No | Follows target | Pure syntactic alias |

### 2.2 Semantics

```cursive
let value = ^Data::new();    // Owning binding with memory location
var counter = 0;              // Owning mutable binding with location
alias a = value;              // Non-owning alias, NO memory location
```

**Critical distinction**: `alias` is purely syntactic sugar for aliasing. It has:
- Zero runtime representation
- No addressable memory location
- Auto-dereferences like the target value
- Cannot have its address taken (`&alias` is a compile error)

---

## 3. Operators

### 3.1 Operator Table

| Operator | Position | Purpose | Returns | Example |
|----------|----------|---------|---------|---------|
| `^` | Prefix (unary) | Region allocation | `T` (value type) | `^Data::new()` |
| `^` | Infix (binary) | Bitwise XOR | Integer type | `a ^ b` |
| `&` | Prefix (unary) | Address-of (create pointer) | `Ptr<T>` | `&value` |
| `&` | Infix (binary) | Bitwise AND | Integer type | `a & b` |
| `*` | Prefix (unary) | Dereference | `T` | `*ptr` |
| `*` | Infix (binary) | Multiply | Numeric type | `a * b` |

### 3.2 Disambiguation Rules

**Position-based parsing** disambiguates operators:

- **Prefix position**: `^`, `&`, `*` are unary operators
- **Infix position**: `^`, `&`, `*` are binary operators

Examples:
```cursive
let data = ^Data::new();      // Prefix ^: region allocation
let xor = a ^ b;              // Infix ^: bitwise XOR

let ptr: Ptr<i32> = &value;   // Prefix &: address-of (creates Ptr<T>)
let mask = a & b;             // Infix &: bitwise AND

let val = *ptr;               // Prefix *: dereference
let product = a * b;          // Infix *: multiply
```

---

## 4. Region Allocation (`^`)

### 4.1 Syntax

```ebnf
UnaryExpr ::= CaretPrefix PrimaryExpr
CaretPrefix ::= "^"+
```

### 4.2 Stacking

```cursive
^expr        // Allocate in innermost region
^^expr       // Allocate one region level up
^^^expr      // Allocate two regions up
```

### 4.3 Critical Semantics

**`^expr` allocates in region and returns the VALUE type `T`, NOT `Ptr<T>`**

```cursive
region r {
    let data: Data = ^Data::new();  // data has type Data, not Ptr<Data>
    // data is allocated in region r but accessed as a value
}
```

**Type rule**:
```
[Region-Alloc]
Γ ⊢ expr: T
───────────────────────────────
Γ ⊢ ^expr: T (allocated in region)
```

### 4.4 Invalid Syntax

```cursive
let ptr: Ptr<i32> = ^42;  // ❌ ERROR: type mismatch
                          // ^42 has type i32, not Ptr<i32>
```

**Correct**:
```cursive
let value: i32 = ^42;       // ✅ value has type i32
let ptr: Ptr<i32> = &value; // ✅ Create pointer explicitly
```

---

## 5. Alias Bindings

### 5.1 Syntax

```cursive
alias name = expr;
```

### 5.2 Semantics

An `alias` binding:
1. Creates a non-owning reference to an existing value
2. Auto-dereferences transparently (no `*` needed)
3. Has **no memory location** (cannot take its address)
4. Has zero runtime cost (purely compile-time construct)
5. Cannot be stored in data structures
6. Cannot be returned from functions
7. Cannot have its address taken

### 5.3 Examples

```cursive
let data = ^Data::new();
alias a1 = data;            // a1 is another name for data
alias a2 = data;            // a2 is another name for data

// All auto-dereference:
println("{}", data);        // ✅ OK
println("{}", a1);          // ✅ OK
println("{}", a2);          // ✅ OK

// Address operations:
let ptr1: Ptr<Data> = &data; // ✅ OK - data has location
let ptr2: Ptr<Data> = &a1;   // ❌ ERROR E4030: alias has no location
```

### 5.4 Error: Address of Alias

```
error[E4030]: cannot take address of alias binding
  --> example.cursive:5:28
   |
5  | let ptr: Ptr<Data> = &a;
   |                       ^^ alias bindings have no memory location
   |
   = note: alias is a pure syntactic alias to another value
   = help: take the address of the original value instead:

           let ptr: Ptr<Data> = &data;
```

---

## 6. Modal Pointers (`Ptr<T>`)

### 6.1 Type Definition

```cursive
modal Ptr<T> {
    @Null      // Null pointer (no valid address)
    @Valid     // Strong reference, points to valid memory
    @Weak      // Weak reference, doesn't prevent destruction
    @Expired   // Weak reference after target was destroyed
}
```

### 6.2 Modal State Semantics

| State | Meaning | Dereference? | Reference Strength |
|-------|---------|--------------|-------------------|
| `@Null` | Explicit null pointer | ❌ Compile error | N/A |
| `@Valid` | Points to valid, accessible memory | ✅ OK | Strong (keeps target alive) |
| `@Weak` | Weak reference, target may be destroyed | ❌ Must upgrade first | Weak (doesn't prevent destruction) |
| `@Expired` | Weak reference, target was destroyed | ❌ Target gone | N/A |

### 6.3 Why Four States?

**@Null and @Valid** are fundamental:
- @Null: Explicit null representation
- @Valid: Non-null, dereferenceable pointer

**@Weak and @Expired** enable essential patterns:
- Breaking reference cycles (parent-child trees)
- Observer patterns (observers don't prevent subject destruction)
- Cache implementations (cache entries don't prevent eviction)
- Graph back-edges (forward edges strong, back edges weak)

**Without weak references**, cyclic data structures would require:
- Unsafe manual lifecycle management
- Reference counting with cycle detection overhead
- External bookkeeping structures

### 6.4 State Transition Diagram

```
Ptr::null() ──────────→ [@Null]
                           │
                           │ (can widen to unconstrained Ptr<T>)
                           ↓
                        [Ptr<T>]
                           ↑
                           │ (can widen to unconstrained Ptr<T>)
                           │
Ptr::new(&value) ─────→ [@Valid] ←────────────┐
                           │                   │
                           │ downgrade()       │ upgrade()
                           ↓                   │
                        [@Weak] ───────────────┤
                           │                   │
                           │ (target          │ (returns @Null
                           │  destroyed)       │  if expired)
                           ↓                   │
                       [@Expired] ─────────────┘
```

### 6.5 Properties

- **First-class type**: Can be stored, passed, returned
- **Nullable**: Can represent null via `@Null` state
- **Modal state tracking**: Compile-time prevention of null/expired dereference
- **Provenance tracking**: Compiler tracks storage source (stack/region/heap)
- **Permission integration**: Works with `Ptr<const T>`, `Ptr<unique T>`, `Ptr<shared T>`

---

## 7. Pointer Construction and Operations

### 7.1 Construction

```cursive
// Create null pointer
Ptr::null(): Ptr<T>@Null

// Create valid pointer from address-of expression
&expr: Ptr<T>@Valid
```

**Key design**: The address-of operator `&expr` directly produces `Ptr<T>` (not an intermediate address value). Type declaration is required.

### 7.2 Examples

```cursive
// Null pointer
let null_ptr: Ptr<i32> = Ptr::null();  // Ptr<i32>@Null

// Valid pointer from address-of
let value: i32 = 42;
let valid_ptr: Ptr<i32> = &value;      // Ptr<i32>@Valid

// Weak reference
let weak_ptr: Ptr<i32>@Weak = valid_ptr.downgrade();  // Ptr<i32>@Weak

// Upgrade weak to strong
match weak_ptr.upgrade() {
    @Valid => { /* target still alive */ }
    @Null => { /* target was destroyed */ }
}
```

### 7.3 Type Widening and Narrowing

**Widening** (safe, implicit):
```cursive
let valid: Ptr<i32>@Valid = &value;
let maybe: Ptr<i32> = valid;  // ✅ OK: widen to unconstrained
```

**Narrowing** (requires pattern matching):
```cursive
let ptr: Ptr<i32> = get_ptr();
let valid: Ptr<i32>@Valid = ptr;  // ❌ ERROR: must match first

// Correct:
match ptr {
    @Valid => {
        let valid: Ptr<i32>@Valid = ptr;  // ✅ OK in @Valid branch
    }
    @Null => { /* handle null */ }
}
```

---

## 8. Address-Of (`&`) and Dereference (`*`)

### 8.1 Address-Of Operator

**Syntax**: `&expr`

**Semantics**: Takes the address of a value with a memory location and produces a `Ptr<T>` directly

**Type rule**:
```
[Address-Of]
Γ ⊢ expr: T
expr has memory location
───────────────────────────────
Γ ⊢ &expr: Ptr<T>@Valid
```

**Examples**:
```cursive
let value = ^Data::new();
let ptr: Ptr<Data> = &value;  // ✅ OK: value has location

alias a = value;
let ptr: Ptr<Data> = &a;      // ❌ ERROR: alias has no location
```

### 8.2 Dereference Operator

**Syntax**: `*ptr`

**Semantics**: Dereferences `Ptr<T>@Valid` to get `T`

**Type rule**:
```
[Deref]
Γ ⊢ ptr: Ptr<T>@Valid
───────────────────────────────
Γ ⊢ *ptr: T
```

**State restrictions**:
```cursive
let valid: Ptr<i32>@Valid = Ptr::new(&value);
let x = *valid;  // ✅ OK: dereference @Valid

let null: Ptr<i32>@Null = Ptr::null();
let y = *null;   // ❌ ERROR: cannot dereference @Null

let weak: Ptr<i32>@Weak = valid.downgrade();
let z = *weak;   // ❌ ERROR: cannot dereference @Weak directly
```

**Correct weak dereference**:
```cursive
match weak.upgrade() {
    @Valid => {
        let value = *weak;  // ✅ OK: refined to @Valid in this branch
    }
    @Null => { /* expired */ }
}
```

---

## 9. Escape Analysis

### 9.1 Provenance Tracking

Every `Ptr<T>` has compile-time provenance metadata:

```
Provenance ::= Stack | Region(RegionId) | Heap
```

### 9.2 Escape Rules

**Rule 1**: Pointers to stack-allocated data cannot escape function scope

**Rule 2**: Pointers to region-allocated data cannot escape their region

**Rule 3**: Pointers to heap-allocated data can escape (subject to ownership)

### 9.3 Examples

**Invalid: Region escape**
```cursive
procedure returns_dangling(): Ptr<Data> {
    region r {
        let data = ^Data::new();
        let ptr: Ptr<Data> = &data;
        result ptr;  // ❌ ERROR E4027: Cannot escape region r
    }
}
```

**Valid: Heap escape**
```cursive
procedure returns_valid(): Ptr<Data>
    grants alloc.region, alloc.heap
{
    region r {
        let data = ^Data::new();
        let heap_data = data.to_heap();  // Explicit heap escape
        result &heap_data;               // ✅ OK: heap-backed pointer
    }
}
```

**Valid: Stack pointer in local scope**
```cursive
function process() {
    let value: i32 = 42;
    let ptr: Ptr<i32> = &value;  // ✅ OK: used locally
    println("{}", *ptr);
    // ptr destroyed at end of scope
}
```

### 9.4 Provenance Propagation

```
[Address-Of-Provenance]
Γ ⊢ value: T
storage_of(value) = provenance
────────────────────────────────────
provenance(&value) = provenance
```

### 9.5 Escape Check

```
[Ptr-Escape-Check]
Γ ⊢ result ptr: Ptr<T>
provenance(ptr) = Region(r)
r is local to current function
────────────────────────────────────
ERROR E4027: Cannot escape region-backed pointer
```

---

## 10. Weak References

### 10.1 Purpose

Weak references solve the cyclic structure problem:

```cursive
// Without weak: MEMORY LEAK (cycle)
record Node {
    parent: Ptr<Node>,        // Strong reference (creates cycle!)
    children: Vec<Ptr<Node>>,
}

// With weak: NO LEAK
record Node {
    parent: Ptr<Node>@Weak,        // Weak reference (breaks cycle)
    children: Vec<Ptr<Node>@Valid>,
}
```

### 10.2 API

```cursive
modal Ptr<T> {
    // Downgrade strong to weak
    procedure @Valid downgrade(self): Ptr<T>@Weak {
        // Implementation
    }

    // Upgrade weak to strong
    procedure @Weak upgrade(self): Ptr<T> {
        // Returns @Valid if target alive, @Null if expired
    }

    // Check if weak reference expired
    procedure @Weak is_expired(self): bool {
        // Implementation
    }
}
```

### 10.3 Usage Example

```cursive
record TreeNode {
    value: i32,
    parent: Ptr<TreeNode>@Weak,      // Weak: doesn't own parent
    children: Vec<Ptr<TreeNode>>,     // Strong: owns children
}

procedure build_tree(): Ptr<TreeNode>
    grants alloc.region, alloc.heap
{
    region temp {
        var root = ^TreeNode {
            value: 1,
            parent: Ptr::null(),
            children: Vec::new(),
        };

        var child = ^TreeNode {
            value: 2,
            parent: Ptr::null(),
            children: Vec::new(),
        };

        // Set up bidirectional links
        child.parent = (&root).downgrade();  // Weak parent link
        root.children.push(&child);          // Strong child link

        // Escape to heap
        result &root.to_heap()
    }
}

procedure traverse_up(node: Ptr<TreeNode>) {
    alias current = node;
    loop {
        match current.parent.upgrade() {
            @Valid => {
                current = current.parent;
                println("Parent value: {}", (*current).value);
            }
            @Null => {
                println("Reached root");
                break;
            }
        }
    }
}
```

---

## 11. Pattern Matching and State Refinement

### 11.1 State Refinement

Pattern matching refines pointer state within branches:

```cursive
let ptr: Ptr<i32> = get_ptr();

match ptr {
    @Null => {
        // In this branch: ptr is Ptr<i32>@Null
        println("Pointer is null");
    }
    @Valid => {
        // In this branch: ptr is Ptr<i32>@Valid
        let value = *ptr;  // ✅ Safe to dereference
        println("Value: {}", value);
    }
    @Weak => {
        // In this branch: ptr is Ptr<i32>@Weak
        match ptr.upgrade() {
            @Valid => { /* use it */ }
            @Null => { /* expired */ }
        }
    }
    @Expired => {
        // In this branch: ptr is Ptr<i32>@Expired
        println("Weak reference expired");
    }
}
```

### 11.2 Partial Matching

```cursive
// Match subset of states
match ptr {
    @Null | @Expired => {
        println("No valid target");
    }
    @Valid => {
        println("Value: {}", *ptr);
    }
    @Weak => {
        println("Weak reference, check before use");
    }
}
```

---

## 12. Integration with Permission System

### 12.1 Permission-Qualified Pointers

```cursive
// Immutable pointer
let ptr: Ptr<const Data> = &data;

// Unique mutable pointer
let ptr: Ptr<unique Data> = &data;

// Shared mutable pointer
let ptr: Ptr<shared Data> = &data;
```

### 12.2 Combining States and Permissions

```cursive
// Nullable unique pointer
let ptr: Ptr<unique Data> = get_maybe_null();

match ptr {
    @Null => { /* handle null */ }
    @Valid => {
        // ptr is Ptr<unique Data>@Valid here
        (*ptr).mutate();  // ✅ OK: unique permission allows mutation
    }
}
```

---

## 13. Error Codes

| Code | Error | Trigger | Example |
|------|-------|---------|---------|
| **E4027** | Region-backed pointer escape | Returning pointer to region data | `result Ptr::new(&region_value)` |
| **E4028** | Dereference null pointer | `*ptr` where `ptr: Ptr<T>@Null` | `let x = *null_ptr;` |
| **E4030** | Address of alias binding | `&alias` | `Ptr::new(&alias_binding)` |
| **E4031** | Dereference alias with `*` | `*alias` (aliases auto-deref) | `let x = *alias_binding;` |
| **E4032** | Dereference weak pointer | `*ptr` where `ptr: Ptr<T>@Weak` | `let x = *weak_ptr;` |
| **E4033** | Dereference expired pointer | `*ptr` where `ptr: Ptr<T>@Expired` | `let x = *expired_ptr;` |

---

## 14. Complete Examples

### 14.1 Linked List with Regions

```cursive
record Node {
    value: i32,
    next: Ptr<Node>,
}

procedure build_list(): Ptr<Node>
    grants alloc.region, alloc.heap
{
    region temp {
        var node1 = ^Node { value: 1, next: Ptr::null() };
        var node2 = ^Node { value: 2, next: Ptr::null() };
        var node3 = ^Node { value: 3, next: Ptr::null() };

        node3.next = &node2;
        node2.next = &node1;

        alias head = node3;

        let heap_list = head.to_heap();
        result &heap_list
    }
}
```

### 14.2 Tree with Parent Pointers (Weak)

```cursive
record TreeNode {
    value: i32,
    parent: Ptr<TreeNode>@Weak,
    left: Ptr<TreeNode>,
    right: Ptr<TreeNode>,
}

procedure insert(root: Ptr<TreeNode>, value: i32)
    grants alloc.heap
{
    match root {
        @Null => {
            // Create new root
        }
        @Valid => {
            if value < (*root).value {
                if (*root).left.is_null() {
                    var new_node = Node {
                        value,
                        parent: root.downgrade(),  // Weak parent link
                        left: Ptr::null(),
                        right: Ptr::null(),
                    }.to_heap();
                    (*root).left = &new_node;
                } else {
                    insert((*root).left, value);
                }
            } else {
                // Handle right subtree similarly
            }
        }
    }
}
```

### 14.3 Observer Pattern with Weak References

```cursive
record Subject {
    value: i32,
    observers: Vec<Ptr<Observer>@Weak>,
}

record Observer {
    id: i32,
}

procedure Subject.notify(self: unique Self) {
    for obs in self.observers {
        match obs.upgrade() {
            @Valid => {
                println("Notifying observer {}", (*obs).id);
            }
            @Null => {
                // Observer was destroyed, remove from list
            }
        }
    }
}

procedure Subject.attach(self: unique Self, obs: Ptr<Observer>) {
    self.observers.push(obs.downgrade());  // Store weak reference
}
```

---

## 15. Grammar Summary

### 15.1 Operator Productions

```ebnf
UnaryExpr       ::= CaretPrefix UnaryExpr
                 | UnaryOp UnaryExpr
                 | PostfixExpr

CaretPrefix     ::= "^"+

UnaryOp         ::= "-" | "!" | "*" | "&"

BitwiseXorExpr  ::= BitwiseAndExpr ("^" BitwiseAndExpr)*
BitwiseAndExpr  ::= CompareExpr ("&" CompareExpr)*
MulExpr         ::= UnaryExpr ("*" UnaryExpr)*
```

### 15.2 Binding Declarations

```ebnf
LetDecl     ::= "let" Pattern (":" Type)? "=" Expr
VarDecl     ::= "var" Pattern (":" Type)? "=" Expr
AliasDecl   ::= "alias" Identifier (":" Type)? "=" Expr
```

### 15.3 Pointer Type

```ebnf
PtrType     ::= "Ptr" "<" Type ">" ("@" ModalState)?
ModalState  ::= "Null" | "Valid" | "Weak" | "Expired"
```

### 15.4 Pointer Construction

```ebnf
PtrAddr     ::= "&" Expr             ; Produces Ptr<T>@Valid
PtrNull     ::= "Ptr" "::" "null" "(" ")"
```

---

## 16. Implementation Requirements

A conforming implementation SHALL:

1. **Track provenance** for all `Ptr<T>` values at compile time
2. **Reject escapes** where region-backed pointers leave their region (E4027)
3. **Enforce state constraints** preventing dereference of @Null, @Weak, @Expired (E4028, E4032, E4033)
4. **Reject address-of-alias** operations (E4030)
5. **Support weak references** with downgrade() and upgrade() operations
6. **Implement pattern matching** state refinement for modal pointers
7. **Provide clear diagnostics** for all error conditions
8. **Zero runtime overhead** for all safety checks (compile-time only)

---

## 17. Design Rationale

### 17.1 Why `alias` instead of `ref`?

**Problem**: `ref` collides with pointer terminology (C++ references, Rust &T)

**Solution**: `alias` clearly means "another name" with no pointer implications

**Benefits**:
- Zero confusion with pointer concepts
- Aligns with "aliasing" terminology already in spec
- Makes "no memory location" property obvious

### 17.2 Why `^expr` returns `T` not `Ptr<T>`?

**Problem**: Dual semantics would violate "explicit over implicit"

**Solution**: `^` only allocates, pointer creation is separate and explicit

**Benefits**:
- Clear separation of concerns (allocation vs pointer creation)
- Predictable types (^expr always returns T)
- Forces explicit pointer creation (&expr)
- LLM-friendly (no context-dependent magic)

### 17.3 Why four modal states?

**Problem**: Cyclic structures impossible with only @Null/@Valid

**Solution**: Add @Weak/@Expired for weak reference support

**Benefits**:
- Enables safe parent-child trees
- Enables observer patterns without leaks
- Enables graph back-edges
- Prevents memory leaks from cycles
- Still simple enough to understand

### 17.4 Why alias has no location?

**Problem**: If alias had location, what does &alias mean?

**Solution**: Alias is pure syntactic sugar with zero storage

**Benefits**:
- Clear semantics: just another name
- Zero runtime cost (compile-time only)
- Forces explicit pointer creation from original values
- Avoids "pointer to alias" confusion

---

## 18. Future Extensions

### 18.1 Deferred to Later Versions

**@Uninit state** (unsafe only):
- For performance-critical bulk allocation
- Only allowed in `unsafe` blocks
- Provides undefined behavior if dereferenced before init

**Async states** (@Pending, @Loading):
- When async/await support is added
- For lazy initialization patterns
- For async I/O operations

**Atomic pointers** (AtomicPtr<T>):
- When concurrency support is added
- Atomic load/store/compare-exchange
- Integration with memory ordering

---

## 19. Conformance Checklist

A conforming implementation of this design SHALL:

- [ ] Support three binding categories: `let`, `var`, `alias`
- [ ] Implement `^` operator returning value type `T`
- [ ] Support region stacking: `^^`, `^^^`
- [ ] Implement `&` operator for address-of
- [ ] Implement `*` operator for dereference
- [ ] Define `Ptr<T>` modal type with four states
- [ ] Implement escape analysis preventing region escapes
- [ ] Reject `&alias` with error E4030
- [ ] Support `&expr` producing `Ptr<T>`, `Ptr::null()`, `.downgrade()`, `.upgrade()`
- [ ] Implement pattern matching state refinement
- [ ] Integrate with permission system (const, unique, shared)
- [ ] Provide all specified error codes with clear diagnostics
- [ ] Achieve zero runtime overhead for all safety checks

---

**End of Document**
