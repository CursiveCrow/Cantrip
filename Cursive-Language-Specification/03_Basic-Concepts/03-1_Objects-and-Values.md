# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-1_Objects-and-Values.md
**Section**: §3.1 Objects and Values
**Stable label**: [basic.object]
**Forward references**: §3.2 [basic.type], §3.4 [basic.storage], §3.5 [basic.align], Clause 7 [type], Clause 11 [memory]

---

### §3.1 Objects and Values [basic.object]

#### §3.1.1 Overview

[1] This subclause establishes the foundational distinction between _objects_ and _values_ in Cursive. Objects are regions of storage with a type, lifetime, and identity; values are abstract mathematical entities that may occupy objects or exist purely as computational intermediates. Understanding this distinction is essential for reasoning about the memory model (Clause 11), type system (Clause 7), and expression semantics (Clause 8).

[2] Every well-formed Cursive program operates by creating objects, storing values in those objects, computing new values from existing ones, and eventually releasing object storage according to deterministic lifetime rules. The language guarantees memory safety by ensuring that every object access occurs within the object's lifetime and respects its permission and region constraints.

[3] This subclause provides conceptual foundations; concrete mechanisms appear in later clauses: storage duration rules (§3.4), layout and alignment (§3.5), type formation (Clause 7), and the complete memory model with regions and permissions (Clause 11).

#### §3.1.2 Objects [basic.object.objects]

##### §3.1.2.1 Definition

[4] An _object_ is a named or anonymous region of storage with the following properties:

1. **Type**: Every object has exactly one type that governs its size, alignment, valid operations, and interpretation.
2. **Lifetime**: Every object has a bounded lifetime during which the storage is valid; accessing an object outside its lifetime produces undefined behavior.
3. **Storage location**: Every object occupies contiguous bytes at a specific address with alignment matching its type's requirements.
4. **Identity**: Objects have identity—two distinct objects are distinguishable even when they store equivalent values.

[5] Objects are created by variable bindings (§5.2), procedure parameters, record/tuple construction (§8.4), region allocation (§11.3), and other allocation sites specified throughout this document. Objects are destroyed when their lifetime ends, triggering destructor execution if the type provides one (§11.5).

##### §3.1.2.2 Object Properties

[6] **Size and alignment.** Every object has a size (number of bytes) and alignment (address constraint). For type `T`, size is denoted `sizeof(T)` and alignment is `alignof(T)`. These properties are determined at compile time based on the type's structure and any representation attributes (§3.5, §11.6).

[7] **Representation.** An object's representation is the sequence of bytes it occupies. Not all byte patterns are valid for all types; reading an invalid representation produces undefined behavior unless explicitly permitted (e.g., when using raw pointers in unsafe blocks, §11.8).

[8] **Subobjects.** Composite types (records, tuples, arrays) contain subobjects: each field, element, or variant payload is itself an object with its own type, lifetime, and alignment. Subobject lifetimes are nested within their containing object's lifetime. Accessing a subobject after the containing object's lifetime ends is undefined behavior.

##### §3.1.2.3 Object Categories

[9] Objects are classified by their storage duration (§3.4):

- **Automatic objects** — Local bindings and temporaries with lexical scope lifetime
- **Static objects** — Module-scope bindings with program lifetime
- **Region objects** — Values allocated in explicit region blocks with LIFO reclamation
- **Dynamic objects** — Heap-allocated values with programmer-controlled lifetime (via region allocation)

[10] This classification determines when object storage is allocated and released. The memory model (Clause 11) provides complete rules for each category.

##### §3.1.2.4 Examples

**Example 3.1.2.1 (Objects with distinct identity):**

```cursive
let x: i32 = 42
let y: i32 = 42
// x and y are distinct objects even though they store equal values
// They have different storage locations and lifetimes
```

**Example 3.1.2.2 (Subobjects in composite types):**

```cursive
record Point { x: f64, y: f64 }

let origin = Point { x: 0.0, y: 0.0 }
// `origin` is an object of type Point
// `origin.x` and `origin.y` are subobjects of type f64
// All three objects share the same lifetime (the enclosing scope)
```

#### §3.1.3 Values [basic.object.values]

##### §3.1.3.1 Definition

[11] A _value_ is an abstract mathematical entity of a particular type. Values exist independently of storage; they may:

- Reside in an object (stored values)
- Exist transiently during expression evaluation (temporaries)
- Appear only at compile time (constant expressions, type-level computations)
- Never exist at runtime (uninhabited types like `!`)

[12] The _value set_ of a type `T`, denoted $\llbracket T \rrbracket$, is the collection of all abstract values that type `T` can represent. For example, $\llbracket \text{bool} \rrbracket = \{\text{true}, \text{false}\}$ and $\llbracket \text{i32} \rrbracket$ contains all 32-bit signed integers from $-2^{31}$ to $2^{31}-1$.

##### §3.1.3.2 Value Operations

[13] Values participate in three fundamental operations:

1. **Reading** — Interpreting an object's representation as a value of its type
2. **Writing** — Storing a value into an object's storage
3. **Computing** — Producing new values from existing ones through expressions

[14] Reading a value from an object requires that:

- The object's lifetime is active
- The object's representation is valid for its type
- The access respects permission constraints (§11.4)

Violations produce undefined behavior or compile-time diagnostics depending on what can be proven statically.

##### §3.1.3.3 Value Categories

[15] Expression evaluation produces values classified by category (§8.1.4):

- **Pure values** — Results of arithmetic, literals, function calls that exist independently of any storage location
- **Place values** — References to objects that may be written to (l-values in C terminology)
- **Temporary values** — Intermediate results created during evaluation that are destroyed when the full-expression completes

[16] The value category determines what operations are permitted on an expression. For example, only place expressions may appear on the left side of assignments; pure values cannot be assigned to.

##### §3.1.3.4 Examples

**Example 3.1.3.1 (Values without objects):**

```cursive
let result = (2 + 3) * 4
// The intermediate values 2, 3, 5 (=2+3), and 4 never occupy objects
// Only the final result 20 is stored in the object named `result`
```

**Example 3.1.3.2 (Value sets):**

```cursive
let flag: bool = true            // value ∈ ⟦bool⟧
let count: u8 = 255              // value ∈ ⟦u8⟧
let invalid: u8 = 256            // error: 256 ∉ ⟦u8⟧
```

#### §3.1.4 Object-Value Correspondence [basic.object.correspondence]

##### §3.1.4.1 Storage and Interpretation

[17] An object _stores_ a value when its representation corresponds to a member of its type's value set. The function $\text{interpret}: \text{Bytes} \times \text{Type} \to \llbracket T \rrbracket \cup \{\bot\}$ maps byte sequences to values, with $\bot$ indicating invalid representations.

[18] Formally, if object $o$ has type $T$ and representation $r$, then $o$ stores value $v$ when $\text{interpret}(r, T) = v$ and $v \in \llbracket T \rrbracket$.

##### §3.1.4.2 Assignment and Initialization

[19] Assignment and initialization write values to objects:

- **Initialization** — The first write to an object, occurring at its creation site
- **Assignment** — Subsequent writes to objects with mutable bindings (`var`) or unique permissions

[20] Initialization is mandatory: every object shall be initialized before its value is read. Definite assignment analysis (§5.7) enforces this requirement statically.

[21] Assignment to immutable bindings (`let`) is forbidden after initialization. Assignment to mutable bindings (`var`) requires appropriate permissions (§11.4). Assignment replaces the object's stored value; when the previous value's type has a destructor, the destructor executes before the new value is written.

##### §3.1.4.3 Copy and Move

[22] Values are transferred between objects through two mechanisms:

- **Copy** — Duplicating a value by reading it from a source object and writing an equivalent value to a destination object. Both objects remain valid. Available only for types satisfying the `Copy` predicate (§10.4).
- **Move** — Transferring cleanup responsibility for a value from one binding to another, invalidating the source binding. The value is not duplicated; cleanup responsibility transfers to the destination.

[23] Primitive types (integers, floating-point, `bool`, `char`, `()`) always satisfy `Copy`. Composite types satisfy `Copy` when all their components do and no custom destructor is defined. Types that do not satisfy `Copy` must be moved; attempting to copy a non-`Copy` type produces a compile-time diagnostic.

##### §3.1.4.4 Examples

**Example 3.1.4.1 (Copy vs Move):**

```cursive
// Integer values are Copy
let x: i32 = 42
let y = x                 // Value 42 is copied; both x and y are valid
let z = x + y             // Both bindings still accessible

// Non-Copy type (assume Buffer does not satisfy Copy)
let buffer = allocate_buffer()
let moved = buffer        // Value moved; `buffer` becomes inaccessible
// let invalid = buffer   // error[E05-703]: use of moved value
```

**Example 3.1.4.2 (Initialization and assignment):**

```cursive
var counter: i32          // Object created but not initialized
// let x = counter        // error[E05-703]: use of uninitialized binding

counter = 0               // Initialization (first write)
counter = counter + 1     // Assignment (subsequent write) - allowed for `var`

let limit = 100           // Initialization
// limit = 200            // error[E05-202]: cannot reassign `let` binding
```

#### §3.1.5 Object Lifetime and Validity [basic.object.lifetime]

[24] An object's _lifetime_ begins when its storage is allocated and initialization completes, and ends when the object is destroyed. Between these points, the object is _valid_; outside this interval, the object is _invalid_.

[25] Accessing an invalid object through any means (dereferencing a dangling pointer, using a moved-from binding, reading destroyed storage) produces undefined behavior [UB-ID: B.2.01].

[26] Cursive's region-based memory model (§11.3) and permission system (§11.4) prevent most lifetime errors at compile time by ensuring that:

- References do not outlive the objects they point to
- Moved values are not accessed after transfer
- Region-allocated objects are not accessed after region closure

[27] The guarantee is not absolute: unsafe code (§11.8) and FFI interactions (Clause 15) may introduce lifetime errors that cannot be detected statically. Programmers using these features assume responsibility for maintaining lifetime invariants.

##### §3.1.5.1 Examples

**Example 3.1.5.1 (Valid and invalid lifetime access):**

```cursive
procedure demo()
    [[ io::write ]]
{
    let data = [1, 2, 3, 4, 5]
    let view: [i32] = data[..]     // view references data's storage
    println("{}", view[0])          // OK: data's lifetime is active
}
// After scope exit, data's lifetime ends; view would be dangling if it escaped
```

#### §3.1.6 Uninitialized Objects [basic.object.uninit]

[28] An object may exist in an _uninitialized state_ between allocation and initialization. Uninitialized objects have allocated storage and a defined type, but their representation is indeterminate.

[29] Reading an uninitialized object produces undefined behavior [UB-ID: B.2.02] unless the type explicitly permits uninitialized reads (e.g., `u8` arrays in unsafe contexts).

[30] Cursive prevents uninitialized reads through definite assignment analysis (§5.7), which statically verifies that every binding is initialized before use. Programs that pass definite assignment are guaranteed not to read uninitialized values outside unsafe blocks.

#### §3.1.7 Holes and Inference Placeholders [basic.object.holes]

[31] A _hole_ (denoted `_` in many contexts) is a placeholder indicating that the compiler should infer a type, permission, or other annotation. Holes are not objects or values; they are compile-time markers that guide type inference (§7.8) and are replaced with concrete types during semantic analysis.

[32] Holes may appear in:

- Type annotations: `let x: _ = expression` (infer type from expression)
- Generic arguments: `make_vec::<_>()` (infer type parameter from context)
- Permission annotations: `let ptr: _ Ptr<T>` (infer permission from usage)

[33] If inference cannot determine a unique solution, the compiler emits a diagnostic requesting an explicit annotation. Holes that survive inference are ill-formed (E03-101).

**Example 3.1.7.1 (Hole inference):**

```cursive
let numbers: _ = [1, 2, 3, 4]      // Inferred as [i32; 4]
let first: _ = numbers[0]           // Inferred as i32 from array element type

// let ambiguous: _ = if condition { 1 } else { 2.0 }
// error[E03-102]: cannot infer type; branches have incompatible types
```

#### §3.1.8 Integration with Type System [basic.object.integration]

[34] Objects and values integrate with the type system as follows:

- **Type safety**: The type system (Clause 7) ensures that values stored in objects are members of the object's type's value set
- **Permission safety**: The permission system (§11.4) ensures that object modifications respect permission qualifiers
- **Lifetime safety**: The region system (§11.3) ensures that references to objects do not outlive the objects themselves

[35] These three safety dimensions compose to provide Cursive's memory safety guarantee: well-typed programs that do not use unsafe code cannot produce use-after-free, double-free, or null-pointer-dereference errors.

#### §3.1.9 Conformance Requirements [basic.object.requirements]

[36] Implementations shall:

1. Maintain the object-value distinction: objects have storage and identity; values are abstract mathematical entities
2. Enforce that every object is initialized before its value is read (via definite assignment, §5.7)
3. Provide undefined behavior when objects are accessed outside their lifetimes, unless provably safe
4. Support the hole inference mechanism and emit diagnostics when inference fails (E03-101, E03-102)
5. Ensure that the interpret function produces consistent results for valid representations

[37] Deviations from these requirements render an implementation non-conforming.

---

**Previous**: Clause 2 — Lexical Structure and Translation | **Next**: §3.2 Types (Overview and Classification)
