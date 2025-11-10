# Cursive Language Specification

## Clause 10 — Generics and Behaviors

**Clause**: 10 — Generics and Behaviors
**File**: 10-1_Region-Parameters.md
**Section**: §10.1 Region Parameters Overview
**Stable label**: [generic.regions]  
**Forward references**: §10.2 [generic.parameter], §10.3 [generic.bounds], §10.6 [generic.resolution], §11.3 [memory.region], §7.5 [type.pointer]

---

### §10.1 Region Parameters Overview [generic.regions]

#### §10.1.1 Overview

[1] This subclause specifies how region allocation integrates with the generic type system. Cursive uses explicit region blocks with the `^` allocation operator (§11.3) to provide lexically-scoped memory management without garbage collection or lifetime annotations.

[2] The current specification uses _provenance tracking_ to associate allocated values with their region of origin. Region-polymorphic type parameters—types parameterized over region identifiers—are deferred to a future edition of this specification to allow field experience with the provenance model to inform design decisions.

[3] Forward references: §10.2 (type parameters), §10.3 (bounds), §10.6 (resolution), §11.3 (region semantics), §7.5 (pointer provenance).

#### §10.1.2 Current Design: Provenance Without Parameterization

[4] Under the current design, region allocation uses the `^` prefix operator within explicit `region` blocks:

```cursive
region session {
    let data = ^Buffer::new()
    process(data)
}
// All allocations in 'session' freed at O(1) when block exits
```

[5] The compiler tracks provenance metadata (Stack, Region(r), Heap) for each allocated object as described in §11.3.4 and §7.5.3. Escape analysis prevents region-allocated values from outliving their region without requiring type-level region parameters.

[6] Generic types currently do not expose region parameters. A type such as `List<T>` may contain region-allocated elements, but the region is determined by allocation sites rather than by parameterization. The provenance of contained elements is tracked via the existing escape analysis mechanism.

#### §10.1.3 Syntax (Current Edition)

[7] Region allocation syntax (defined in §11.3.2):

**Region blocks** match the pattern:
```
"region" [ <identifier> ] <block_stmt>
```

**Region allocation expressions** take one of the following forms:
```
"^" <expression>
"^" "^" ... <expression>    // Caret stacking for parent regions
```

[ Note: See §11.3.2 [memory.region] for complete region allocation semantics.
— end note ]

[8] Generic parameter lists (§10.2.2) currently support type, const, and grant parameters but not region parameters:

**Generic parameters** match the pattern:
```
"<" <generic_param_list> ">"
```

where **generic parameter lists** match:
```
<generic_param> [ "," <generic_param> ... ]
```

and each **generic parameter** takes one of the following forms:
```
<type_param>
<const_param>
<grant_param>
// region_param reserved for future edition
```

[ Note: See §10.2.2 [generic.params] and Annex A §A.6 for the complete generic parameter grammar.
— end note ]

#### §10.1.4 Constraints (Current Edition)

[9] Region allocation requires an active region block. Using `^` outside a region context produces diagnostic E11-103 (defined in §11.3).

[10] Escape analysis (§11.3.3, §7.5.3) prevents region-allocated values from escaping their originating region. This constraint is enforced through provenance tracking without requiring type-level region annotations.

[11] Generic types containing region-allocated elements shall not expose those elements in ways that would allow escape. The prohibition is enforced through the same provenance and escape rules that apply to non-generic types.

#### §10.1.5 Semantics (Current Edition)

[12] When a generic type `Container<T>` is instantiated with region-allocated elements:

1. The element type `T` carries no region annotation
2. Elements allocated with `^` have provenance Region(r) where r is the active region
3. Escape analysis prevents storing those elements in containers with longer lifetimes
4. The container itself may be allocated in any region or on the heap

[13] The lack of region parameterization means that containers cannot abstract over region lifetimes. Each container instance is associated with a specific allocation site, and region provenance is determined by that site rather than by type parameters.

#### §10.1.6 Examples (Current Edition)

**Example 10.1.6.1 (Region allocation without parameterization):**

```cursive
record List<T> {
    data: Ptr<T>@Valid,
    len: usize,
    cap: usize,
}

procedure demo()
    [[ alloc::region, alloc::heap |- true => true ]]
{
    region temp {
        let items = ^List::new()        // List allocated in temp
        items.push(^Item::new())         // Item allocated in temp
        // Both List and Items freed when temp exits
    }

    let permanent = List::new()          // List allocated on heap
    permanent.push(Item::new())          // Item allocated on heap
    // Separate lifetime from temp region
}
```

**Example 10.1.6.2 (Escape prevention through provenance):**

```cursive
procedure invalid_escape(): List<Data>
    [[ alloc::region, alloc::heap |- true => true ]]
{
    region temp {
        let list = ^List::new()
        list.push(^Data::new())
        result list  // error[E11-101]: region-allocated value cannot escape
    }
}

procedure valid_heap_return(): List<Data>
    [[ alloc::region, alloc::heap |- true => true ]]
{
    region temp {
        let list = ^List::new()
        list.push(^Data::new())
        result list.to_heap()  // Explicit heap conversion before escape
    }
}
```

#### §10.1.7 Design Decision: No Region Type Parameters

[14] This specification deliberately does **not** introduce region type parameters (syntax like `List<^r, T>`). Region allocation is managed through the `Arena` modal type (§11.3.2) and provenance metadata, not type-level parameterization.

[ Rationale: Region type parameters (e.g., `record List<^r, T> { ... }`) would reintroduce complexity similar to Rust lifetime parameters, contradicting Cursive's "regions not lifetimes" philosophy.

**Fundamental difference**:

- **Rust lifetimes** (`<'a, T>`): Abstract temporal bounds on reference validity; enforce borrow checking; pervasive type-level annotations
- **Cursive arenas**: Concrete memory allocation arenas; managed through modal types; opt-in explicit allocation

**Why type parameters would be problematic**:

1. Type pollution: `<^r, T>` parameters would appear throughout type signatures
2. Variance complexity: Would require variance rules for region parameters
3. Subtyping complexity: Would need region outlives relations (`^r: ^s`)
4. Mental model confusion: Regions are allocation arenas (spatial), not lifetime bounds (temporal)
5. Implementation burden: Every generic type would need region parameter support

**Current design (Arena modal type)**:

- Arenas are first-class modal values (`Arena@Active`)
- Region blocks create scoped arenas automatically
- Named bindings enable passing arenas to procedures
- No type-level pollution - arena allocation is a runtime choice
- Zero complexity when not needed - most code uses Stack/Heap provenance

**If more control is needed**:
Rather than type parameters, Cursive provides:

- Named arena bindings (`region r as workspace`)
- Arena parameters (`procedure f(workspace: Arena@Active)`)
- Explicit allocation (`workspace.alloc()` or `^` sugar)
- Modal state machine (@Active/@Frozen/@Freed)

This approach keeps allocation strategy as an explicit runtime concern rather than a type-level abstraction, aligning with Cursive's explicit-over-implicit philosophy.
— end rationale ]

[15] Region type parameters are not supported in version 1.0. Future editions may reconsider based on field experience if the Arena modal type proves insufficient for all use cases. If additional arena control is needed beyond named bindings and modal states, extensions will prioritize first-class arena values (library types, collection patterns) rather than type-level parameterization.

#### §10.1.8 Integration with Other Systems

[17] Region allocation integrates with:

- **Type system (Clause 7)**: Pointer provenance (§7.5.3) tracks allocation source
- **Memory model (Clause 11)**: Region blocks (§11.3), escape analysis (§11.3.3)
- **Permissions (§11.4)**: Orthogonal—region allocation works with all permissions
- **Generics (§10.6)**: Monomorphization preserves region semantics

[18] These integrations ensure that region allocation remains safe and deterministic even when combined with generic types, permissions, and contracts.

#### §10.1.9 Conformance Requirements

[19] Implementations shall:

1. Support region allocation via `^` operator within region blocks per §11.3
2. Track provenance metadata (Stack, Region(r), Heap) for all allocated objects
3. Perform escape analysis preventing region-allocated values from escaping their region
4. Support generic types with region-allocated elements using provenance tracking
5. Reject region escapes with diagnostic E11-101 (defined in §11.3)
6. Integrate region allocation with monomorphization (§10.6) so that generic instantiations preserve region semantics
7. Document that region type parameters are not supported in this edition

[20] Implementations shall not introduce region parameterization syntax without specification approval, as such extensions would be non-conforming.

---

**Previous**: Clause 9 — Statements and Control Flow (§9.4 [stmt.order]) | **Next**: §10.2 Type Parameters (§10.2 [generic.parameter])
