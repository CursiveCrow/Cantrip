# Cursive Language Specification

## Clause 11 — Memory Model, Regions, and Permissions

**Part**: XI — Memory Model, Regions, and Permissions
**File**: 11-3_Regions.md  
**Section**: §11.3 Regions  
**Stable label**: [memory.region]  
**Forward references**: §11.2 [memory.object], §11.5 [memory.move], §11.7 [memory.aliasing], Clause 3 §3.3 [basic.scope], Clause 3 §3.4 [basic.storage], Clause 7 §7.5 [type.pointer]

---

### §11.3 Regions [memory.region]

#### §11.3.1 Overview

[1] Regions provide lexically-scoped, stack-like allocation arenas with O(1) bulk deallocation. They enable efficient temporary allocation without garbage collection or manual memory management.

[2] Region allocation uses the `^` prefix operator. Region-allocated values have lifetimes bounded by the region block; escape analysis prevents values from outliving their region.

[3] This section specifies region syntax, allocation semantics, escape analysis, and the relationship between regions and binding responsibility.

[4] **Provenance metadata.** The compiler tracks allocation source (Stack, Region, Heap) for every object as metadata separate from its type. Escape analysis operates on this provenance metadata to prevent values from outliving their allocation scope. Provenance is not part of the type system but is checked during type checking and influences escape diagnostics. The provenance model is formally specified in §7.5.3 (Provenance and Escape Analysis).

#### §11.3.2 Region Block Syntax [memory.region.syntax]

##### §11.3.2.1 Region Blocks

[4] Region block syntax:

```ebnf
region_block
    ::= "region" identifier? block_stmt
```

[ Note: See Annex A §A.5 [grammar.statement] for complete region grammar.
— end note ]

[5] Regions may be named (for caret stacking, §11.3.4) or anonymous. Named regions enable allocating in parent regions via multiple carets.

**Example 11.3.2.1 (Region blocks):**

```cursive
region temp {
    let data = ^Buffer::new(1024)
    process(data)
}
// All allocations in temp freed here (O(1))

region outer {
    region inner {
        let a = ^Data::new()   // Allocated in inner
    }
    // inner region freed
    let b = ^Data::new()       // Allocated in outer
}
// outer region freed
```

##### §11.3.2.2 Region Allocation Operator

[6] The `^` prefix operator allocates in the active region:

```ebnf
region_alloc_expr
    ::= "^" expression
     | "^"+ expression    // Multiple carets for parent regions
```

[7] **Critical semantics**: `^expr` evaluates `expr` and allocates the result in the active region, returning a value of type `T` (NOT `Ptr<T>`).

[ Given: Expression $e$ with type $\tau$, active region $r$ ]

$$
\frac{\langle e, \sigma \rangle \Downarrow \langle v, \sigma' \rangle}{\langle \texttt{^} e, \sigma' \rangle \Downarrow \langle v_r, \sigma''[r \mapsto v_r] \rangle}
\tag{E-Region-Alloc}
$$

where $v_r$ is the value $v$ allocated in region $r$.

[8] Type rule for region allocation:

$$
\frac{\Gamma \vdash e : \tau}{\Gamma \vdash \texttt{^} e : \tau}
\tag{T-Region-Alloc}
$$

[9] The result type is `τ`, not `Ptr<τ>`. To obtain a pointer, use address-of: `&(^expr)` produces `Ptr<τ>@Valid`.

**Example 11.3.2.2 (Region allocation returns value type):**

```cursive
region r {
    let value: i32 = ^42            // Type is i32, not Ptr<i32>
    let data: Buffer = ^Buffer::new()   // Type is Buffer, not Ptr<Buffer>

    // To get pointer:
    let ptr: Ptr<Buffer>@Valid = &data  // Address-of creates pointer
}
```

#### §11.3.3 Escape Analysis [memory.region.escape]

##### §11.3.3.1 Overview

[10] Escape analysis prevents region-allocated values from outliving their region. The compiler statically verifies that region-allocated data does not escape the region block.

##### §11.3.3.2 Escape Rules

[11] **Non-escape constraint**: Values allocated in a region shall not escape that region. Violations produce diagnostic E11-101.

(11.1) A value _escapes_ a region when:

- It is returned from a procedure containing the region
- It is stored in a location with longer lifetime (module-scope, parent region without explicit parent allocation)
- A pointer to it escapes (provenance tracking, §7.5.3)

[12] **Escape checking rule**:

[ Given: Region $r$, value $v$ allocated in $r$, target scope $s$ ]

$$
\frac{v \text{ allocated in region } r \quad s \text{ outlives } r}{v \text{ escapes to } s \Rightarrow \text{ERROR E11-101}}
\tag{WF-No-Escape}
$$

**Example 11.3.3.1 - invalid (Region escape):**

```cursive
procedure create_data(): Buffer
    [[ alloc::region |- true => true ]]
{
    region temp {
        let data = ^Buffer::new()
        result data  // error[E11-101]: region-allocated value cannot escape
    }
}
```

##### §11.3.3.3 Escape via Heap

[13] To return region-allocated data, explicitly convert to heap allocation:

**Example 11.3.3.2 (Explicit heap escape):**

```cursive
procedure create_data(): Buffer
    [[ alloc::region, alloc::heap |- true => true ]]
{
    region temp {
        let data = ^Buffer::new()
        data.prepare()
        result data.to_heap()  // Explicit conversion before escape
    }
}
```

[14] The `.to_heap()` method (standard library, implementation-defined) allocates on the heap and returns a heap-backed value that can escape.

#### §11.3.4 Caret Stacking for Parent Regions [memory.region.stacking]

##### §11.3.4.1 Parent Region Allocation

[15] Multiple caret operators allocate in parent regions:

- `^expr` — Allocate in innermost region
- `^^expr` — Allocate in parent region (one level up)
- `^^^expr` — Allocate in grandparent region (two levels up)

[16] This enables allocating data with longer lifetimes without escaping to the heap.

**Example 11.3.4.1 (Caret stacking):**

```cursive
region outer {
    let outer_data = ^Data::new()  // Allocated in outer

    region inner {
        let inner_data = ^Data::new()   // Allocated in inner
        let parent_data = ^^Data::new()  // Allocated in outer (parent)
    }
    // inner region freed (inner_data destroyed)
    // parent_data still alive (allocated in outer)

    use(parent_data)
}
// outer region freed (outer_data and parent_data destroyed)
```

[17] Caret stacking requires named regions in future versions for explicit parent references. The current version uses implicit parent resolution based on lexical nesting.

[ Note: Region parameters and explicit parent naming are deferred to a future version of this specification.
— end note ]

#### §11.3.5 Bulk Deallocation [memory.region.bulk]

##### §11.3.5.1 O(1) Deallocation

[18] When a region exits, all allocations within that region are freed simultaneously in O(1) constant time. This is achieved through arena-style allocation where the region maintains a single allocation pointer.

[19] **Performance characteristic**:

- **Allocation**: O(1) pointer bump
- **Individual deallocation**: Not supported (bulk only)
- **Bulk deallocation**: O(1) reset region pointer

[20] This makes regions ideal for temporary allocations in tight loops or recursive procedures.

**Example 11.3.5.1 (Efficient batch processing):**

```cursive
procedure process_items(items: [Item])
    [[ alloc::region, io::write |- items.len() > 0 => true ]]
{
    loop item: Item in items {
        region iteration {
            let buffer = ^Buffer::new(4096)
            let processed = ^transform(item)
            output(buffer, processed)
        }
        // O(1) bulk free - no accumulation across iterations
    }
}
```

#### §11.3.6 Diagnostics

[21] Region diagnostics:

| Code    | Condition                                     |
| ------- | --------------------------------------------- |
| E11-101 | Region-allocated value escapes region         |
| E11-102 | Invalid caret stacking (too many ^ for depth) |
| E11-103 | Region allocation outside region block        |

#### §11.3.7 Conformance Requirements

[22] Implementations shall:

1. Support region blocks with lexical scoping
2. Implement `^` operator returning value type T (not pointer type)
3. Support caret stacking (`^^`, `^^^`) for parent region allocation
4. Perform static escape analysis rejecting region escapes with E11-101
5. Provide O(1) bulk deallocation when regions exit
6. Track region provenance for pointer escape checking (§7.5.3)
7. Integrate region lifetimes with binding responsibility and RAII cleanup

---

**Previous**: §11.2 Objects and Storage Duration (§11.2 [memory.object]) | **Next**: §11.4 Permissions (§11.4 [memory.permission])
