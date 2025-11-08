# Cursive Language Specification

## Clause 13 — Witness System

**Clause**: 13 — Witness System
**File**: 13-1_Overview-and-Purpose.md
**Section**: §13.1 Overview and Purpose
**Stable label**: [witness.overview]  
**Forward references**: §13.2 [witness.kind], §13.3 [witness.formation], §13.4 [witness.representation], §13.5 [witness.dispatch], §13.6 [witness.memory], §13.7 [witness.grammar], §13.8 [witness.diagnostics], Clause 10 [generic], Clause 12 [contract]

---

### §13.1 Overview and Purpose [witness.overview]

#### §13.1.1 Overview

[1] The _witness system_ provides runtime evidence of type-level properties, enabling dynamic polymorphism and runtime verification as an explicit opt-in feature. A witness is a dense pointer (multi-component pointer) that packages a value with proof of a property: behavior implementation, contract satisfaction, or modal state membership.

[2] Witnesses enable:

- **Dynamic polymorphism**: Heterogeneous collections of different types sharing a common interface
- **Runtime state tracking**: Dynamic verification of modal states when compile-time tracking is insufficient
- **Plugin systems**: Loading implementations at runtime from external sources
- **Type erasure**: Hiding concrete types behind interfaces
- **Runtime contract verification**: Dynamic checking of behavior/contract satisfaction

[3] The witness system integrates behaviors (Clause 10), contracts (Clause 12), and modal types (§7.6) into a unified runtime evidence mechanism. Static dispatch through monomorphization remains the zero-cost default; witnesses are an explicit opt-in for scenarios requiring runtime flexibility.

#### §13.1.2 Static vs Dynamic Verification [witness.overview.static]

[4] Cursive provides two verification strategies for type-level properties:

**Static verification** (default, zero-cost):

- Behavior implementation: Generic bounds `<T: Display>`
- Contract satisfaction: Generic bounds `<T: Serializable>`
- Modal states: Type system tracking `FileHandle@Open`
- Compile-time proof, monomorphization, no runtime overhead

**Dynamic verification** (opt-in, explicit cost):

- Behavior implementation: Witness type `witness<Display>`
- Contract satisfaction: Witness type `witness<Serializable>`
- Modal states: Witness type `witness<FileHandle@Open>`
- Runtime evidence via dense pointers, vtable dispatch, small overhead

[5] Table 13.1 compares the two strategies:

**Table 13.1 — Static vs dynamic verification**

| Aspect      | Static (Monomorphization)   | Dynamic (Witnesses)                  |
| ----------- | --------------------------- | ------------------------------------ |
| Syntax      | Generic bounds `<T: B>`     | Witness type `witness<B>`            |
| Type check  | Compile-time                | Runtime                              |
| Dispatch    | Direct call (inlined)       | Vtable indirect call                 |
| Memory      | Zero overhead               | 16 bytes per witness (dense pointer) |
| Flexibility | Homogeneous collections     | Heterogeneous collections            |
| Cost        | Zero                        | Small (one indirection)              |
| Use when    | Types known at compile time | Types determined at runtime          |

[6] The witness system makes the cost of dynamic dispatch **explicit**: the `witness<B>` type in signatures clearly indicates runtime polymorphism.

#### §13.1.3 Witness as Modal Type [witness.overview.modal]

[7] The `witness` type is a built-in modal type with three allocation states:

```cursive
modal witness<B> {
    @Stack   // Non-responsible reference to stack value
    @Region  // Responsible region-allocated value
    @Heap    // Responsible heap-allocated value
}
```

[8] Default: `witness<B>` without state annotation means `witness<B>@Stack` (conservative, local reference).

[9] Allocation states determine cleanup responsibility:

- `@Stack`: Non-responsible binding (like `let x <- value`), does not call destructor
- `@Region`: Responsible binding (like `let x = move value`), calls destructor at region exit
- `@Heap`: Responsible binding (like `let x = move value`), calls destructor at scope exit

[10] Transitions between states follow the chain: `@Stack → @Region → @Heap`, each transition requiring explicit method calls and appropriate grants.

#### §13.1.4 Dense Pointer Representation [witness.overview.representation]

[11] Witnesses use _dense pointer_ representation: a multi-component pointer containing both data and metadata. Dense pointers contrast with _sparse pointers_ (single-component pointers like `Ptr<T>`).

[12] A witness value contains:

```
witness<B> = {
    data_ptr: Ptr<T>,            // Pointer to concrete value (8 bytes)
    metadata: Ptr<WitnessTable>  // Pointer to witness metadata (8 bytes)
}

WitnessTable = {
    type_id: TypeId,                // Runtime type identification
    state_tag: Option<StateTag>,    // For modal witnesses
    vtable: [fn(*const ()) -> ...], // Method dispatch table
    drop: Option<fn(*mut ())>       // Destructor function
}
```

[13] Total size: 16 bytes on 64-bit platforms. The dense pointer representation enables type-safe dynamic dispatch with minimal overhead.

#### §13.1.5 Unified Witness Concept [witness.overview.unified]

[14] The witness system provides a **unified mechanism** for runtime evidence of different type-level properties:

**Behavior witnesses** (`witness<Display>`):

- Runtime evidence that a value satisfies behavior Display
- VTable contains pointers to behavior procedure implementations
- Enables heterogeneous collections of different types with common behavior

**Contract witnesses** (`witness<Serializable>`):

- Runtime evidence that a value satisfies contract Serializable
- VTable contains pointers to contract procedure implementations
- Same representation as behavior witnesses (behaviors and contracts distinguished at compile time)

**Modal state witnesses** (`witness<FileHandle@Open>`):

- Runtime evidence that a modal value is in state @Open
- State tag stored in witness metadata
- Enables dynamic state tracking when compile-time tracking insufficient

[15] All witness kinds use the same dense pointer representation and dispatch mechanism. The property in angle brackets (`<Display>`, `<Serializable>`, `<FileHandle@Open>`) determines what is being witnessed.

#### §13.1.6 Design Philosophy [witness.overview.philosophy]

[16] The witness system embodies Cursive's design principles:

**Explicit over implicit**:
[17] Dynamic dispatch is opt-in via explicit `witness<B>` types in signatures. Programmers see the performance cost in the type annotation. Generic bounds remain the zero-cost default.

**Zero-cost abstraction default**:
[18] Static dispatch (monomorphization) is the default for all polymorphism. Witnesses are used only when runtime flexibility is actually needed: plugin systems, heterogeneous collections, runtime-determined types.

**Pay for what you use**:
[19] Programs using only static dispatch pay zero witness overhead. Programs needing dynamic dispatch pay exactly the cost of dense pointers and vtable indirection, visible in type signatures.

**Local reasoning**:
[20] Witness types show the complete dispatch strategy. `witness<Display>@Heap` clearly indicates: dynamic dispatch (witness), Display interface, heap allocation, responsible cleanup.

**Memory safety maintained**:
[21] Witnesses integrate with permissions, regions, and RAII. Cleanup responsibility follows allocation state; permissions are inherited from underlying values; region witnesses cannot escape their regions.

#### §13.1.7 Integration Points [witness.overview.integration]

[22] The witness system integrates with:

**Behaviors (Clause 10)**:

- Behavior declarations provide vtable structure
- Behavior implementations compile to vtable entries
- `witness<Behavior>` types enable dynamic behavior dispatch

**Contracts (Clause 12)**:

- Contract declarations define abstract interfaces
- Contract implementations compile to vtable entries
- `witness<Contract>` types enable dynamic contract dispatch
- Dynamic verification hooks generate witness evidence

**Modal Types (§7.6)**:

- Modal declarations define state machines
- `witness<Type@State>` enables runtime state tracking
- State transitions verifiable dynamically when needed

**Memory Model (Clause 11)**:

- Witness allocation states integrate with regions
- Cleanup responsibility follows binding semantics
- Permissions inherited from underlying values
- Dense pointers subject to escape analysis

**Type System (Clause 7)**:

- Witness types are modal types with allocation states
- Method dispatch typed through behavior/contract signatures
- Subtyping follows modal widening rules

#### §13.1.8 Conformance Requirements [witness.overview.requirements]

[23] Implementations shall:

1. Provide `witness<B>` as built-in modal type with states @Stack, @Region, @Heap
2. Support witness construction from values implementing behaviors/contracts
3. Implement dense pointer representation (data pointer + metadata pointer)
4. Support dynamic dispatch via vtable for behavior/contract witnesses
5. Support runtime state tracking for modal state witnesses
6. Integrate witness cleanup responsibility with RAII (§11.2)
7. Enforce permission inheritance from underlying values
8. Apply region escape analysis to witnesses
9. Require explicit `move` for @Heap/@Region witness creation
10. Emit diagnostics for witness violations (E13-xxx family)
11. Maintain witness as opt-in feature with zero cost when unused

---

**Previous**: Clause 12 — Contracts (§12.10 [contract.diagnostics]) | **Next**: §13.2 Witness Kinds (§13.2 [witness.kind])
