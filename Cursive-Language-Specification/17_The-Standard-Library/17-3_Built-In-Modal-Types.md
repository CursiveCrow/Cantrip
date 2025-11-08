# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-3_Built-In-Modal-Types.md  
**Section**: §17.3 Built-In Modal Types  
**Stable label**: [stdlib.types]  
**Forward references**: Clause 7 §7.6 [type.modal], Clause 11 §11.3 [memory.region], Clause 14 [concurrency]

---

### §17.3 Built-In Modal Types [stdlib.types]

#### §17.3.1 Overview

[1] This section specifies the built-in modal types provided by the language runtime. These types are referenced throughout the specification and have compiler and runtime support beyond what pure Cursive source code can provide.

[2] Built-in modal types include:

- `string` — UTF-8 string with @View and @Managed states (§7.3.4.3)
- `Arena` — Memory arena for region allocation (§11.3.2)
- `Thread<T>` — Thread lifecycle management (§14.1.3)
- `Ptr<T>` — Safe modal pointers (§7.5)
- `Mutex<T>`, `Channel<T>`, `RwLock<T>` — Synchronization primitives (§14.4)

#### §17.3.2 String Modal Type

##### §17.3.2.1 Complete Specification

[3] The `string` modal type is specified in §7.3.4.3. This section provides the complete standard library interface:

```cursive
modal string {
    @View {
        ptr: Ptr<u8>@Valid,
        len: usize,
    }

    @View::to_managed(~): string@Managed
    @View::is_empty(~): bool
    @View::len(~): usize
    @View::as_bytes(~): [u8]
    @View::as_c_ptr(~): *const u8

    @Managed {
        ptr: Ptr<u8>@Valid,
        len: usize,
        capacity: usize,
    }

    @Managed::view(~): string@View
    @Managed::is_empty(~): bool
    @Managed::len(~): usize
    @Managed::capacity(~): usize
    @Managed::push(~!, ch: char)
    @Managed::append(~!, other: string@View)
    @Managed::clear(~!)
    @Managed::reserve(~!, additional: usize)
}

// Static constructor
public procedure string::from(s: string@View): string@Managed
    [[ alloc::heap |- true => result.len() == s.len() ]]
{
    // Allocate and copy
}
```

[4] Default state: Bare `string` in type annotations defaults to `string@View` (§7.3.4.3[27.1]).

##### §17.3.2.2 String Procedures

[5] Standard string operations:

```cursive
public procedure string::concat(a: string@View, b: string@View): string@Managed
    [[ alloc::heap |- true => result.len() == a.len() + b.len() ]]

public procedure string::from_bytes(bytes: [u8]): string@Managed \/ ParseError
    [[ alloc::heap |- bytes.len() > 0 => true ]]

public procedure string::from_c_str(ptr: *const u8): string@Managed \/ ParseError
    [[ alloc::heap, unsafe::ptr |- ptr != null => true ]]
```

#### §17.3.3 Arena Modal Type

[6] Complete specification in §11.3.2. Standard library interface:

```cursive
// Built-in modal type for region allocation
modal Arena {
    @Active { /* fields */ }
    @Frozen { /* fields */ }
    @Freed { /* fields */ }

    // Complete transition specifications in §11.3.2
}

public procedure Arena::new_scoped(): Arena@Active
    [[ alloc::heap |- true => true ]]
```

#### §17.3.4 Synchronization Primitives

[7] Complete specifications for `Mutex<T>`, `Channel<T>`, and `RwLock<T>` appear in §14.4. The standard library provides their implementations.

#### §17.3.5 Conformance Requirements

[8] Implementations shall:

1. Provide `string` modal type with complete interface
2. Provide `Arena` modal type for region support
3. Provide synchronization primitives per Clause 14
4. Ensure UTF-8 validity for all string operations
5. Implement modal state transitions correctly
6. Document all procedures with full contractual sequents

---

**Previous**: §17.2 Core Types and Utilities (§17.2 [stdlib.core]) | **Next**: §17.4 Standard Behaviors (§17.4 [stdlib.behaviors])
