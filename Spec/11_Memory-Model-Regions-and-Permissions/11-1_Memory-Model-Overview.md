# Cursive Language Specification

## Clause 11 — Memory Model, Regions, and Permissions

**Part**: XI — Memory Model, Regions, and Permissions
**File**: 11-1_Memory-Model-Overview.md  
**Section**: §11.1 Memory Model Overview  
**Stable label**: [memory.overview]  
**Forward references**: §11.2 [memory.object], §11.3 [memory.region], §11.4 [memory.permission], §11.5 [memory.move], Clause 3 §3.1 [basic.object], Clause 10 [generic], Clause 12 [contract]

---

### §11.1 Memory Model Overview [memory.overview]

#### §11.1.1 Overview and Design Goals

[1] Cursive achieves memory safety through compile-time enforcement of cleanup responsibility, permission-based access control, and region-scoped allocation—without garbage collection or borrow checking. The memory model ensures that well-typed programs cannot produce use-after-free errors, double-free errors, or dangling references.

[2] This clause specifies the complete memory model, including: object lifetimes and storage duration (§11.2), region-based allocation through the `Arena` modal type (§11.3), the permission system (§11.4), transfer semantics and binding categories (§11.5), memory layout (§11.6), aliasing rules (§11.7), and unsafe operations (§11.8).

[3] The model is built on three foundational principles:

- **Deterministic cleanup**: Resources are released at statically determined program points via RAII (Resource Acquisition Is Initialization)
- **Explicit transfers**: Cleanup responsibility transfers require the explicit `move` keyword at call sites
- **Zero runtime overhead**: All safety mechanisms operate at compile time with no runtime cost

#### §11.1.2 Two-Axis Orthogonal Design

[4] Cursive's memory model separates two independent concerns into orthogonal axes:

**Axis 1: Binding Categories** — Who has cleanup responsibility?

- **Responsible bindings**: Call destructors at scope exit (`let`, `var` with `=`)
- **Non-responsible bindings**: Do not call destructors (`let`, `var` with `<-`)

**Axis 2: Permissions** — What access operations are allowed?

- **const**: Read-only access, unlimited aliasing
- **unique**: Exclusive mutable access, no aliasing
- **shared**: Shared mutable access, programmer-coordinated

[5] These axes are completely orthogonal. Table 11.1 shows all valid combinations:

**Table 11.1 — Binding forms and permissions**

| Binding Form     | Cleanup Responsibility | Rebindable | Transferable via `move` | Permission Options    |
| ---------------- | ---------------------- | ---------- | ----------------------- | --------------------- |
| `let x = value`  | YES (responsible)      | NO         | YES                     | const, unique, shared |
| `var x = value`  | YES (responsible)      | YES        | NO                      | const, unique, shared |
| `let x <- value` | NO (non-responsible)   | NO         | NO                      | const, unique, shared |
| `var x <- value` | NO (non-responsible)   | YES        | NO                      | const, unique, shared |

[6] The orthogonal design means binding categories (who cleans up, whether rebindable) are independent from permissions (what operations are allowed). A binding can be responsible yet immutable (`let x: const`), or non-responsible yet permit unique access (`let x: unique <-`). Only `let` bindings created with `=` can transfer cleanup responsibility via `move`; this restriction preserves local reasoning by avoiding the need to track whether a `var` binding might be rebound after being moved.

[6.1] **Non-responsible binding invalidation**: Non-responsible bindings (`<-`) reference objects, not bindings. They remain valid as long as the object exists. When a binding is moved to a **responsible parameter** (procedure parameter with `move` modifier, §5.4.3[2.1]), the object might be destroyed by the callee. Therefore, all non-responsible bindings referencing that object become invalid. Conversely, passing to **non-responsible parameters** (no `move` modifier) guarantees object survival, preserving reference validity. The compiler uses parameter responsibility as a compile-time signal for potential destruction, tracking dependencies through definite assignment analysis (§5.7.4[5]) with zero runtime overhead.

#### §11.1.3 No Garbage Collection

[7] Cursive does not employ garbage collection. Resource cleanup is deterministic and occurs at statically determined program points. This provides:

- **Predictable performance**: No unpredictable GC pauses
- **Deterministic timing**: Cleanup occurs exactly when scope exits
- **Real-time suitability**: Bounded execution time without GC latency
- **Resource correctness**: Files, sockets, locks released immediately

[8] Cleanup follows RAII (Resource Acquisition Is Initialization): objects are automatically destroyed at scope exit in LIFO (last-in, first-out) order. Destructors execute deterministically, releasing resources without programmer intervention.

[9] Comparison with garbage-collected languages:

**Table 11.2 — GC vs RAII comparison**

| Aspect           | Garbage Collection (Java, Go, Python) | Cursive RAII                   |
| ---------------- | ------------------------------------- | ------------------------------ |
| Cleanup timing   | Non-deterministic (GC decides)        | Deterministic (scope exit)     |
| Performance      | Unpredictable pauses                  | Bounded, predictable           |
| Resource release | Delayed (finalization queues)         | Immediate (LIFO at scope exit) |
| Real-time        | Difficult (GC pauses)                 | Natural (no pauses)            |
| Memory overhead  | GC metadata, heap pressure            | Minimal (stack/region only)    |

#### §11.1.4 No Borrow Checker

[10] Cursive achieves memory safety without borrow checking or lifetime annotations. The design explicitly avoids lifetime parameters and exclusive borrowing rules characteristic of borrow-checked systems.

[11] **Alternative mechanisms:**

- **Regions** provide lexical scoping for allocations without lifetime parameters
- **Permissions** (`const`, `unique`, `shared`) control access without borrow rules
- **Explicit `move`** makes responsibility transfer visible without lifetime tracking
- **Field partitioning** prevents aliasing hazards for `shared` permission

[ Rationale: Cursive's memory model differs fundamentally from borrow-checked approaches. Borrow checking systems typically use:

- Lifetime annotations (`'a`, `'b` parameters)
- Exclusive mutable borrowing (shared XOR mutable rule)
- Complex lifetime inference
- Implicit moves in many contexts

Cursive instead uses:

- Lexical regions (no annotation parameters)
- Three-tier permission system (const/unique/shared)
- Explicit move keyword (always visible)
- Local reasoning (no whole-program analysis)

This approach provides memory safety with simpler mental model, better support for complex data structures (via `shared` permission), and more predictable behavior for LLM-assisted development.
— end rationale ]

#### §11.1.5 Explicit Over Implicit Philosophy

[14] The memory model embodies the "explicit over implicit" design principle:

(14.1) **Explicit responsibility transfer**: The `move` keyword is required at call sites to transfer cleanup responsibility. Implicit transfers are forbidden.

(14.2) **Explicit permissions**: Type annotations include permission qualifiers (`const`, `unique`, `shared`) when needed; `const` is the default but can be stated explicitly for documentation.

(14.3) **Explicit allocation**: Region allocation requires the `^` operator; there are no hidden heap allocations.

(14.4) **Explicit coordination**: The `shared` permission requires programmers to coordinate access; the compiler does not hide synchronization.

(14.5) **Explicit escape**: Escaping region-allocated data to the heap requires explicit `.to_heap()` calls.

[15] These explicit markers make memory behavior locally visible without global analysis.

#### §11.1.6 Zero Runtime Overhead

[16] All memory safety mechanisms operate at compile time. The memory model guarantees:

- **No runtime checks**: Permission violations detected at compile time
- **No metadata**: Objects carry no runtime permission or responsibility tags
- **No reference counting**: Cleanup responsibility is static, not dynamic
- **No GC metadata**: No garbage collector infrastructure
- **Equivalent assembly**: Safe code compiles to identical assembly as unsafe code performing the same operations

[17] Field-level partitioning, unique enforcement, region escape analysis, and move tracking are purely compile-time analyses with zero runtime representation.

#### §11.1.7 Integration with Type System

[18] The memory model integrates with the type system (Clause 7) and expression evaluation (Clause 8):

- **Permissions** attach to types at binding sites (§7.1.3)
- **Ptr<T> modal states** track pointer validity through type system (§7.5)
- **Value categories** (§8.1.4) determine whether expressions are places or values
- **Definite assignment** (§5.7) prevents use of uninitialized or moved-from bindings

[19] These integrations compose to provide end-to-end memory safety without runtime enforcement.

#### §11.1.8 Reference Tracking Without Runtime Cost

[21] A key innovation in Cursive's memory model is tracking object lifetimes for non-responsible bindings **without runtime overhead**. The mechanism:

1. **Compile-time dependency tracking**: Non-responsible bindings (`<-`) recorded as referencing objects via source bindings
2. **Parameter responsibility signal**: Procedure parameters declare whether they will destroy objects (`move` modifier)
3. **Invalidation propagation**: Move to responsible parameter → invalidate all references to that object
4. **Validity preservation**: Pass to non-responsible parameter → references remain valid

This provides the **safety of garbage collection** (no dangling references) with the **cost of manual management** (zero runtime overhead) by leveraging information already present in procedure signatures.

**Comparison to other approaches**:

| Approach               | Safety | Cost       | How Achieved                 |
| ---------------------- | ------ | ---------- | ---------------------------- |
| Garbage Collection     | ✅     | Runtime GC | Runtime reference tracking   |
| Rust Borrow Checker    | ✅     | Zero       | Compile-time lifetime params |
| **Cursive References** | ✅     | **Zero**   | **Parameter responsibility** |

Cursive achieves safe references through explicit parameter annotations rather than lifetime parameters or runtime tracking.

#### §11.1.9 Conformance Requirements

[22] Implementations shall:

1. Enforce cleanup responsibility through binding categories (let/var with =/←)
2. Implement the permission system (const/unique/shared) with compile-time checking
3. Provide region-based allocation with escape analysis preventing dangling references
4. Support explicit `move` keyword for responsibility transfer
5. Track non-responsible binding dependencies and invalidate based on parameter responsibility
6. Implement RAII with LIFO destruction order at scope exit
7. Guarantee zero runtime overhead for all safety mechanisms
8. Reject programs with memory safety violations at compile time
9. Provide the four storage duration categories: static, automatic, region, heap

---

**Previous**: Clause 9 — Statements and Control Flow (§9.4 [stmt.order]) | **Next**: §11.2 Objects and Storage Duration (§11.2 [memory.object])
