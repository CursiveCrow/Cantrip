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

[10] Escape analysis prevents region-allocated values from outliving their region. The compiler statically verifies that region-allocated data does not escape the region block through provenance tracking and escape checking at region boundaries.

[11] Escape analysis operates on compile-time **provenance metadata** associated with each value. Provenance tracks the allocation source (stack, region, or heap) and is propagated through operations. Complete algorithm specifications appear in Annex E §E.2.6 [implementation.algorithms.region].

##### §11.3.3.2 Provenance Metadata [memory.region.provenance]

[12] Every value has associated provenance metadata indicating its allocation source:

**Provenance Type:**

$$
\text{Provenance} ::= \text{Stack} \mid \text{Region}(r) \mid \text{Heap}
$$

where $r$ is a region identifier bound to a lexical region scope.

[13] **Provenance assignment rules**:

- Values allocated with `^` in region $r$ have provenance $\text{Region}(r)$
- Local variables (without `^`) have provenance $\text{Stack}$
- Values converted via `.to_heap()` have provenance $\text{Heap}$
- Module-scope values have provenance $\text{Static}$ (treated as Heap for escape purposes)

[14] **Provenance is metadata**: Provenance information is tracked by the compiler but is not part of the type system. A value of type `T` has the same type regardless of whether it has provenance Stack, Region(r), or Heap. Provenance affects only escape analysis, not type checking.

##### §11.3.3.3 Provenance Propagation [memory.region.propagation]

[15] Provenance propagates through operations according to these rules:

**Address-of propagation:**

[ Given: Value $v$ with provenance $\pi$ ]

$$
\frac{\text{prov}(v) = \pi}{\text{prov}(\&v) = \pi}
\tag{Prov-Addr}
$$

Taking the address of a value produces a pointer with the same provenance as the value.

**Field access propagation:**

[ Given: Structure $s$ with provenance $\pi$, field $f$ ]

$$
\frac{\text{prov}(s) = \pi}{\text{prov}(s.f) = \pi}
\tag{Prov-Field}
$$

Accessing a field inherits the structure's provenance.

**Procedure return propagation:**

[ Given: Procedure call $p(\text{args})$ with conservative provenance ]

$$
\frac{}{\text{prov}(p(\text{args})) = \text{Heap}}
\tag{Prov-Call-Conservative}
$$

Procedure calls conservatively assume Heap provenance unless the procedure's signature specifies otherwise (future extension). Implementations may refine this with interprocedural analysis.

[16] Complete provenance propagation rules for all expression forms are specified in Annex E §E.2.6 [implementation.algorithms.region].

##### §11.3.3.4 Escape Checking [memory.region.escape.checking]

[17] **Escape prohibition**: Values with region provenance shall not escape their region. Implementations shall check escape at:

1. **Return statements**: Returning a Region(r)-provenance value when r is local
2. **Region block exit**: Storing region-allocated values in outer scopes
3. **Closure capture**: Capturing region values in closures that outlive the region
4. **Field assignment**: Assigning region values to fields of longer-lived structures

[18] **Formal escape constraint**:

[ Given: Expression $e$ with provenance $\pi$, target scope $s$ ]

$$
\frac{\text{prov}(e) = \text{Region}(r) \quad r \text{ is lexically nested in } s}
     {\text{escape}_s(e) \Rightarrow \text{ERROR E11-101}}
\tag{WF-No-Region-Escape}
$$

The rule states: if a value has region provenance and the region is local to the current analysis scope, escaping that value to the target scope is forbidden.

[19] **Diagnostic E11-101**: "Cannot escape region-allocated value from region `{region_id}`"

The diagnostic shall include:
- Region identifier
- Allocation site (where `^` occurred)
- Escape site (where escape was attempted)
- Provenance path (how provenance propagated to escape site)

##### §11.3.3.5 Permitted Escape via Heap Conversion [memory.region.escape.heap]

[20] Region-allocated values may escape only after explicit conversion to heap allocation:

**Heap conversion rule:**

[ Given: Value $v$ with provenance $\text{Region}(r)$ ]

$$
\frac{\text{prov}(v) = \text{Region}(r)}{\text{prov}(v\texttt{.to\_heap()}) = \text{Heap}}
\tag{Prov-Heap-Convert}
$$

[21] The `.to_heap()` operation (standard library) copies the value to heap storage and returns a heap-allocated value with provenance Heap. The original region-allocated value remains valid until the region exits.

[22] Heap conversion requires the `alloc::heap` grant in the procedure's contractual sequent. Attempting to call `.to_heap()` without the grant produces diagnostic E11-102 (heap allocation requires alloc::heap grant).

##### §11.3.3.6 Examples [memory.region.escape.examples]

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

**Example 11.3.3.2 (Heap conversion allows escape):**

```cursive
procedure create_data(): Buffer
    [[ alloc::region, alloc::heap |- true => true ]]
{
    region temp {
        let data = ^Buffer::new()
        data.prepare()
        result data.to_heap()  // ✅ OK: explicit heap conversion
    }
}
```

**Example 11.3.3.3 (Pointer escape prevention):**

```cursive
procedure get_pointer(): Ptr<i32>@Valid
    [[ alloc::region |- true => true ]]
{
    region r {
        let value = ^42
        let ptr = &value           // ptr has provenance Region(r)
        result ptr                 // error[E11-101]: pointer provenance is Region(r)
    }
}
```

**Example 11.3.3.4 (Stack values can escape):**

```cursive
procedure create_value(): i32
    [[ |- true => true ]]
{
    region r {
        let stack_value = 42       // Provenance: Stack (no ^)
        result stack_value         // ✅ OK: stack values can escape
    }
}
```

##### §11.3.3.7 Algorithm Reference [memory.region.escape.algorithm]

[23] The complete escape analysis algorithm, including:
- Provenance propagation for all expression forms
- Interprocedural provenance tracking
- Escape checking at all control-flow points
- Conservative analysis for complex cases

is specified in Annex E §E.2.6 [implementation.algorithms.region]. Implementations shall follow the algorithm or a refinement that rejects strictly more programs (i.e., more conservative).

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
