# Cursive Language Specification

## Clause 11 — Memory Model, Regions, and Permissions

**Part**: XI — Memory Model, Regions, and Permissions
**File**: 11-4_Permissions.md  
**Section**: §11.4 Permissions  
**Stable label**: [memory.permission]  
**Forward references**: §11.2 [memory.object], §11.5 [memory.move], §11.7 [memory.aliasing], Clause 5 §5.5 [decl.type], Clause 7 §7.1.3 [type.overview.permissions]

---

### §11.4 Permissions [memory.permission]

#### §11.4.1 Overview

[1] Permissions control what operations are allowed on values. Cursive provides three permissions: `const` (read-only), `unique` (exclusive mutable), and `shared` (shared mutable with programmer coordination).

[2] Permissions are orthogonal to cleanup responsibility (§11.2). A binding can be responsible yet immutable, or non-responsible yet mutable. Permissions attach to types at binding sites as specified in §7.1.3.

[3] This section specifies permission semantics, the permission lattice, compile-time enforcement rules, and field-level partitioning for safe `shared` usage.

#### §11.4.2 Permission Definitions [memory.permission.definitions]

##### §11.4.2.1 Const Permission

[4] The `const` permission provides read-only access with unlimited aliasing. Multiple `const` bindings can reference the same object safely.

**Semantics:**

- ✅ Read object fields and call const methods
- ❌ Mutate object fields
- ❌ Call methods requiring unique or shared
- ✅ Create multiple `const` bindings to same object (unlimited aliasing)

[5] `const` is the default permission when omitted in type annotations.

**Example 11.4.2.1 (Const permission):**

```cursive
let data: const Buffer = Buffer::new()
let ref1: const <- data
let ref2: const <- data
let ref3: const <- data      // Multiple const bindings allowed

println("{}", data.size())   // Read-only access OK
// data.modify()             // error[E11-301]: cannot mutate through const
```

##### §11.4.2.2 Unique Permission

[6] The `unique` permission provides exclusive mutable access. Only one `unique` binding to an object can exist at a time; the compiler enforces this statically.

**Semantics:**

- ✅ Read and mutate object fields
- ✅ Call any methods (const, unique, shared)
- ❌ Create second `unique` binding while first is active
- ✅ Temporary downgrade to `const` (§11.4.4)

**Example 11.4.2.2 (Unique permission):**

```cursive
let data: unique Buffer = Buffer::new()

data.append(42)              // Mutation allowed

let second: unique <- data   // error[E11-302]: unique already active
```

##### §11.4.2.3 Shared Permission

[7] The `shared` permission provides shared mutable access. Multiple `shared` bindings can coexist; programmers coordinate to prevent conflicts.

**Semantics:**

- ✅ Read and mutate object fields
- ✅ Create multiple `shared` bindings
- ⚠️ Programmer responsible for coordination
- ✅ Field-level partitioning prevents same-field multi-access (§11.4.6)

**Example 11.4.2.3 (Shared permission):**

```cursive
let data: shared Buffer = Buffer::new()
let ref1: shared <- data
let ref2: shared <- data     // Multiple shared bindings allowed

ref1.set_flag(true)          // Both can mutate
ref2.set_counter(10)         // Programmer ensures no conflicts
```

##### §11.4.2.4 No Interior Mutability

[8] **Design principle**: `const` means const. There is no interior mutability: if a value has `const` permission, it is immutable. No hidden mutation through `const` is permitted.

[9] This differs from languages with interior mutability (Rust's `Cell<T>`, `RefCell<T>`). Cursive rejects interior mutability to maintain "explicit over implicit" design.

#### §11.4.3 Permission Lattice [memory.permission.lattice]

##### §11.4.3.1 Subtyping Relations

[10] Permissions form a lattice with subtyping relations:

$$
\text{unique} <: \text{shared} <: \text{const}
$$

[ Note: This notation means unique is a subtype of shared, and shared is a subtype of const. Values with stronger permissions can be used where weaker permissions are expected.
— end note ]

[11] **Downgrade rules**: Permissions can downgrade (stronger → weaker) implicitly:

$$
\frac{\Gamma \vdash e : \text{unique } \tau}{\Gamma \vdash e : \text{shared } \tau}
\tag{Coerce-Unique-Shared}
$$

$$
\frac{\Gamma \vdash e : \text{unique } \tau}{\Gamma \vdash e : \text{const } \tau}
\tag{Coerce-Unique-Const}
$$

$$
\frac{\Gamma \vdash e : \text{shared } \tau}{\Gamma \vdash e : \text{const } \tau}
\tag{Coerce-Shared-Const}
$$

[12] **No upgrades**: Permissions cannot upgrade (weaker → stronger). Attempting to treat `const` as `unique` produces diagnostic E11-303.

##### §11.4.3.2 Permission Lattice Diagram

[13] Visual representation:

```
        unique (most restrictive)
         /  \
        /    \
    shared   (middle)
        \    /
         \  /
        const (least restrictive)
```

#### §11.4.4 Permission Checking [memory.permission.checking]

##### §11.4.4.1 Active Reference Tracking

[14] The compiler tracks active bindings with non-`const` permissions to enforce uniqueness and detect conflicts:

(14.1) When a `unique` binding is created, the compiler records it as active.

(14.2) While a `unique` binding is active, creating another `unique` or `shared` binding to the same object is forbidden.

(14.3) When the `unique` binding goes out of scope, it becomes inactive.

[15] **Unique enforcement rule**:

[ Given: Object $o$, active `unique` binding $x$ ]

$$
\frac{x : \text{unique } \tau \text{ active} \quad y \text{ attempts } \text{unique or shared binding to same object}}{\text{ERROR E11-302: unique permission already active}}
\tag{WF-Unique-Exclusive}
$$

##### §11.4.4.2 Temporary Downgrade

[16] A `unique` binding can temporarily downgrade to `const`:

**Example 11.4.4.1 (Temporary const downgrade):**

```cursive
let data: unique Buffer = Buffer::new()

{
    let reader: const <- data   // Temporarily downgrade to const
    println("{}", reader.size())
    // data.modify()             // error: cannot use unique while const active
}

// Const binding out of scope
data.modify()                    // OK: unique restored
```

#### §11.4.5 Field-Level Partitioning [memory.permission.partitioning]

##### §11.4.5.1 Overview

[17] Field-level partitioning prevents accessing the same field multiple times through `shared` bindings, eliminating iterator invalidation and aliasing hazards while preserving the ability to access different fields simultaneously.

##### §11.4.5.2 Partition Directive Syntax

[18] Partition directives appear in record bodies:

```ebnf
partition_directive
    ::= "<<" identifier ">>"
     | "<<" "_" ">>"    // Reset to default

record_body
    ::= "{" (partition_directive | record_field)* "}"
```

[ Note: See Annex A §A.6 [grammar.declaration] for complete record grammar with partition directives.
— end note ]

[19] **Default behavior**: Each field is its own partition unless a directive groups them.

**Example 11.4.5.1 (Partition directives):**

```cursive
record World {
    entities: [Entity],      // Default: World::entities partition

    <<Physics>>
    positions: [Position],   // Physics partition
    velocities: [Velocity],  // Physics partition

    <<Combat>>
    healths: [Health],       // Combat partition
}
```

##### §11.4.5.3 Partition Access Rules

[20] **Single-access rule**: Through a `shared` binding, each partition can be accessed at most once in a scope.

[ Given: Shared binding $s$, field $f$ in partition $p$ ]

$$
\frac{s : \text{shared } R \quad f \in \text{partition } p \quad \text{active access to partition } p}{\text{ERROR E11-310: partition already accessed}}
\tag{WF-Partition-Single}
$$

[21] **Different-partition freedom**: Accessing fields in different partitions is allowed:

**Example 11.4.5.2 (Partition access):**

```cursive
let world: shared World = World::new()

let pos_ref: shared <- world.positions     // Access Physics partition
let health_ref: shared <- world.healths    // Access Combat partition (OK: different)

// let vel_ref: shared <- world.velocities  // error[E11-310]: Physics partition already active
```

[22] The `<<_>>` directive resets to default partitioning (each field separate).

##### §11.4.5.4 Nested Field Access and Partitions [memory.permission.partitioning.nested]

[23] **Nested field access behavior.** Partition access tracking applies only to direct field access on the `shared` binding itself. Once a field reference is obtained, further nested field access through that reference does not count as re-accessing the partition.

[ Given: Shared binding $s$ with field $f$ in partition $p$, resulting reference $r$ ]

$$
\frac{s: \text{shared } R \quad r \gets s.f \quad f \in \text{partition } p}{\text{nested accesses through } r \text{ do not re-check partition } p}
\tag{P-Partition-Nested}
$$

[24] The partition constraint is enforced at the initial field access point. Subsequent operations through the obtained reference (further field access, method calls, indexing) are unrestricted by partition rules.

**Example 11.4.5.3 (Nested access through partition):**

```cursive
record Inner { x: i32, y: i32 }
record Outer {
    <<PartA>>
    field1: Inner,
    field2: Inner,
}

let obj: shared Outer = Outer::new()
let ref1: shared <- obj.field1       // Accesses PartA partition
let x_coord = ref1.x                 // OK: accessing through ref1, not obj
let y_coord = ref1.y                 // OK: nested access doesn't re-check partition

// let ref2: shared <- obj.field2    // error[E11-310]: PartA partition already accessed
```

[25] This design allows normal operation on obtained references while preventing multiple direct accesses to the same partition, balancing safety with usability.

#### §11.4.6 Diagnostics

[26] Permission diagnostics:

| Code    | Condition                                          |
| ------- | -------------------------------------------------- |
| E11-301 | Mutation through `const` permission                |
| E11-302 | Multiple `unique` bindings to same object          |
| E11-303 | Permission upgrade attempt (const → unique)        |
| E11-310 | Partition accessed multiple times through `shared` |
| E11-311 | Invalid partition directive (unknown identifier)   |

#### §11.4.7 Conformance Requirements

[27] Implementations shall:

1. Support three permissions (const, unique, shared) with specified semantics
2. Enforce permission lattice with downgrades, reject upgrades
3. Track active `unique` bindings, reject multiple simultaneous access
4. Allow unlimited `const` bindings to same object
5. Support `shared` with programmer coordination
6. Implement field-level partitioning with `<<Partition>>` directives
7. Enforce single-partition-access rule for `shared` bindings at direct field access level (§11.4.5.4)
8. Reject const interior mutability (const means const)
9. Provide compile-time permission checking with zero runtime overhead

---

**Previous**: §11.3 Regions (§11.3 [memory.region]) | **Next**: §11.5 Move/Copy/Clone Semantics (§11.5 [memory.move])
