# Part II: Type System - §9. Pointers

**Section**: §9 | **Part**: Type System (Part II)
**Previous**: [§8. Modals](08_Modals.md) | **Next**: [Part III: Functions](../03_Functions/00_Introduction.md)

---

**Definition 9.1 (Pointers):** Cantrip provides two pointer systems: (1) Safe pointers `Ptr<T>@S` using modal states and effect exclusion for compile-time aliasing safety without runtime overhead, and (2) raw pointers `*T` and `*mut T` for unsafe manual memory management and FFI interoperability. Safe pointers prevent aliasing bugs through the axiom ∀ T. ¬(read<T> ∧ write<T>), while raw pointers provide no safety guarantees and require `unsafe { }` blocks with the `unsafe.ptr` effect.

---

## 9. Pointers

### 9.1 Overview

#### 9.1.1 Pointer Types in Cantrip

Cantrip provides two complementary pointer systems, each designed for specific use cases:

**Safe Pointers (`Ptr<T>@State`):**
- Modal state machines (@Exclusive, @Shared, @Frozen)
- Effect-based aliasing prevention (read<T> ⊥ write<T>)
- Compile-time safety guarantees
- Zero runtime overhead
- For application code, safe data structures

**Raw Pointers (`*T`, `*mut T`):**
- Direct memory addresses (machine words)
- No safety guarantees
- Requires `unsafe { }` blocks and `unsafe.ptr` effect
- Nullable, supports pointer arithmetic
- For FFI, manual memory management, unsafe code

**Key innovation:** Safe pointers use modal types + effect exclusion to achieve memory safety without a borrow checker, while raw pointers provide an explicit escape hatch for unsafe operations.

#### 9.1.2 When to Use Each Type

**Use Safe Pointers (`Ptr<T>`) for:**
- Self-referential records (records containing pointers to their own fields)
- Intrusive data structures (linked lists, trees with embedded node pointers)
- Cyclic data structures (graphs requiring pointer cycles)
- Application-level safe code
- When aliasing safety matters

**Use Raw Pointers (`*T`, `*mut T`) for:**
- FFI/C interop (`extern "C"` functions)
- Manual memory management (custom allocators, arenas)
- Implementing safe abstractions (Vec, String internals)
- Low-level system programming (kernel, drivers)
- Performance-critical unsafe code

**Use Neither (prefer references):**
- Normal data access → Use standard permissions (`T`, `mut T`, `own T`)
- Single ownership → Use owned values
- Temporary borrows → Use references
- Simple collections → Use Vec, HashMap, etc.

#### 9.1.3 Safety Model

**Safe by default:**
```cantrip
// Normal code uses safe pointers
let ptr: Ptr<i32>@Exclusive = Ptr.new(&x)
let value = ptr.read()  // Compile-time safety checks
```

**Unsafe escape hatch:**
```cantrip
// Unsafe code explicitly marked
function use_raw_ptr(raw: *i32): i32
    uses unsafe.ptr  // Effect declaration required
{
    unsafe {  // Explicit unsafe block
        *raw  // Dereference only allowed here
    }
}
```

**Effect declarations:**
- Safe pointers: `read<T>`, `write<T>` (type-specific)
- Raw pointers: `unsafe.ptr` (blanket unsafe permission)
- FFI calls: `ffi.call` + `unsafe.ptr`

#### 9.1.4 Relationship to Other Features

**Cross-references:**

- **§1 (Mathematical Foundations):** Notation used throughout this section:
  - Type judgments: `Γ ⊢ e : T`
  - Effect judgments: `Γ ⊢ e ! ε`
  - Evaluation: `⟨e, σ⟩ ⇓ ⟨v, σ'⟩`
  - Substitution: `e[x ↦ v]`
  - See §1.2 for complete notation reference

- **§8 (Modals):** Safe pointers ARE modal types
  - `Ptr<T>@Exclusive`, `Ptr<T>@Shared`, `Ptr<T>@Frozen` are modal states
  - State transitions follow modal transition rules (§8.3.2)
  - Compile-time state verification (§8.5)

- **Part III (Permission System):** Pointers compose with permissions
  - `own Ptr<T>@Exclusive` — owned pointer (movable)
  - `mut Ptr<T>@Exclusive` — mutable reference to pointer
  - `Ptr<T>@Exclusive` — immutable reference to pointer (default)
  - Raw pointers: `*T` (immutable), `*mut T` (mutable)
  - See §26 for permission semantics

- **Part V (Contract System):**
  - Effect tracking for pointer operations:
    - `read<T>` — reading through pointer
    - `write<T>` — writing through pointer
    - `unsafe.ptr` — raw pointer operations
  - Effect exclusion axiom: `∀ T. ¬(read<T> ∧ write<T>)` (Definition 9.4)
  - Preconditions for validity: `must valid(ptr)`
  - See §21 for effect system details

- **Part VI (Memory Regions):**
  - Safe pointers integrate with region escape analysis
  - `Ptr.new_in<r>(&x)` binds pointer to region r
  - Compiler prevents region escape (§29.3)
  - Raw pointers bypass escape analysis in `unsafe { }` blocks

- **§17 (Contracts):**
  - Safe pointer validity enforced via contracts
  - Raw pointer validity is programmer responsibility
  - Pointer preconditions: `must ptr != null<T>()`

- **§24 (Unsafe):**
  - Raw pointer dereferencing must `unsafe { }` blocks
  - `unsafe.ptr` effect must be declared
  - Unsafe boundary for raw operations

- **§56-62 (FFI):**
  - Raw pointers provide C interoperability
  - Type mappings: `*T` ↔ `const T*`, `*mut T` ↔ `T*`
  - `null<T>()` ↔ `NULL`

---

## 9.2 Syntax

**Key innovation/purpose:** Unified syntax covering both safe and raw pointers, demonstrating the complementary nature of Cantrip's two-tier pointer system.

### 9.2.1 Concrete Syntax

**EBNF Grammar:**

```ebnf
(* ========== Type Syntax ========== *)

PointerType ::= SafePointerType | RawPointerType

SafePointerType ::= "Ptr" "<" Type ">" "@" PointerState

PointerState ::= "Exclusive" | "Shared" | "Frozen"

RawPointerType ::= "*" Type                    (* Immutable raw pointer *)
                 | "*" "mut" Type              (* Mutable raw pointer *)

(* ========== Expression Syntax ========== *)

PointerExpr ::= SafePointerExpr | RawPointerExpr | ConversionExpr

SafePointerExpr ::= SafePointerCreation
                  | SafePointerOperation

SafePointerCreation ::= "Ptr" "." "new" "(" Expr ")"
                      | "Ptr" "." "new_in" "<" Ident ">" "(" Expr ")"

SafePointerOperation ::= Expr "." "share" "(" ")"
                       | Expr "." "freeze" "(" ")"
                       | Expr "." "read" "(" ")"
                       | Expr "." "write" "(" Expr ")"
                       | Expr "." "clone" "(" ")"
                       | Expr "." "as_raw" "(" ")"

RawPointerExpr ::= RawPointerCreation
                 | RawPointerOperation

RawPointerCreation ::= "&raw" Expr
                     | "&raw" "mut" Expr
                     | "null" "<" Type ">" "(" ")"

RawPointerOperation ::= "*" Expr                           (* Dereference *)
                      | Expr "." "offset" "(" Expr ")"
                      | Expr "." "add" "(" Expr ")"
                      | Expr "." "sub" "(" Expr ")"
                      | Expr "." "cast" "<" Type ">" "(" ")"

ConversionExpr ::= "Ptr" "." "from_raw" "(" Expr ")"

(* ========== Effect Syntax ========== *)

PointerEffect ::= "read" "<" Type ">"          (* Type-specific read *)
                | "write" "<" Type ">"         (* Type-specific write *)
                | "unsafe.ptr"                 (* Raw pointer operations *)
```

**Explanation:**
- **Safe pointer types** are parameterized by pointee type `T` and state `@S`
- **Raw pointer types** distinguish immutable (`*T`) and mutable (`*mut T`)
- **Safe operations** (`read`, `write`, `share`, `freeze`) are type-safe
- **Raw operations** (`*ptr`, `offset`, `cast`) require `unsafe { }` blocks
- **Effect annotations** are type-specific for safe pointers, blanket for raw

### 9.2.2 Abstract Syntax

**Formal representation:**

```
(* ========== Types ========== *)

τ, σ, ρ ::= ...                              (types from previous sections)
          | Ptr<τ>@S                         (safe pointer in state S)
          | *τ                               (immutable raw pointer)
          | *mut τ                           (mutable raw pointer)

S ::= Exclusive                              (unique access)
    | Shared                                 (multiple read-only aliases)
    | Frozen                                 (permanently immutable)

(* ========== Expressions ========== *)

e ::= ...                                    (expressions from previous sections)
    (* Safe pointer operations *)
    | Ptr.new(e)                            (heap allocation)
    | Ptr.new_in<r>(e)                      (region allocation)
    | e.share()                             (create aliases)
    | e.freeze()                            (freeze forever)
    | e.read()                              (read value)
    | e.write(e')                           (write value)
    | e.clone()                             (clone pointer)
    | e.as_raw()                            (convert to raw)

    (* Raw pointer operations *)
    | &raw e                                (take raw address - immutable)
    | &raw mut e                            (take raw address - mutable)
    | *e                                    (dereference - UNSAFE)
    | e.offset(n)                           (pointer arithmetic - UNSAFE)
    | e.add(n)                              (pointer addition - UNSAFE)
    | e.sub(n)                              (pointer subtraction - UNSAFE)
    | e.cast<τ>()                           (type casting - UNSAFE)
    | null<τ>()                             (null pointer)

    (* Conversions *)
    | Ptr.from_raw(e)                       (raw to safe - UNSAFE)

(* ========== Values ========== *)

v ::= ...                                    (values from previous sections)
    | Ptr@S { addr: ℓ, region: r }          (safe pointer value - internal representation)
    | *τ { addr: ℓ }                        (raw pointer value)
    | *mut τ { addr: ℓ }                    (mutable raw pointer value)

(* ========== Effects ========== *)

ε ::= ...                                    (effects from previous sections)
    | read<τ>                               (reading type τ)
    | write<τ>                              (writing type τ)
    | unsafe.ptr                            (raw pointer operations)
    | alloc.heap                            (heap allocation)
    | alloc.region                          (region allocation)

(* ========== Effect Exclusion Axiom ========== *)

Axiom (Effect Exclusion):
  ∀ τ ∈ Type. ¬(read<τ> ∧ write<τ>)

  Reading: "For all types τ, read and write effects on τ cannot coexist."
```

**Components:**

**Safe Pointer Internal Representation:**
- `addr: ℓ` — raw memory address (pointer-sized)
- `region: r` — region binding (compile-time only, not stored)
- `state: S` — modal state (@Exclusive, @Shared, @Frozen) (compile-time only)

**Raw Pointer Representation:**
- `addr: ℓ` — raw memory address (pointer-sized)
- No state tracking, no safety guarantees

**Key Observations:**
- Safe pointers have richer type information but identical runtime representation
- Modal states compile away — zero runtime overhead
- Effect annotations enable compile-time safety checking
- Raw pointers provide escape hatch when safety checks not needed

### 9.2.3 Basic Examples

**Safe Pointers:**

```cantrip
// Heap-allocated safe pointer
let x = 42
let ptr: Ptr<i32>@Exclusive = Ptr.new(&x)
let value = ptr.read()  // uses read<i32>
ptr.write(100)          // uses write<i32>

// Region-allocated safe pointer
region temp {
    let data = vec![1, 2, 3]
    let ptr: Ptr<Vec<i32>>@Exclusive = Ptr.new_in<temp>(&data)

    let value = ptr.read()  // OK
    // return ptr  // ❌ ERROR: Cannot escape region temp
}

// Sharing pointers
let exclusive: Ptr<i32>@Exclusive = Ptr.new(&x)
let (ptr1, ptr2) = exclusive.share()
// ptr1, ptr2: Ptr<i32>@Shared

let v1 = ptr1.read()  // uses read<i32>
let v2 = ptr2.read()  // uses read<i32>
// ptr1.write(42)  // ❌ ERROR: @Shared has no write() method

// Freezing pointers
let frozen: Ptr<i32>@Frozen = exclusive.freeze()
let frozen2 = frozen.clone()  // Can clone freely
frozen.read()   // OK
// frozen.write(42)  // ❌ ERROR: @Frozen has no write() method
```

**Raw Pointers:**

```cantrip
// Taking raw addresses (SAFE operation)
let x = 42
let ptr: *i32 = &raw x
let ptr_mut: *mut i32 = &raw mut x

// Dereferencing (UNSAFE - must unsafe block)
function read_raw(ptr: *i32): i32
    uses unsafe.ptr
{
    unsafe {
        *ptr  // Dereference only allowed in unsafe block
    }
}

// Null pointers
let null_ptr: *i32 = null<i32>()
if null_ptr == null<i32>() {
    // Handle null case
}

// Pointer arithmetic (UNSAFE)
function next_element(ptr: *i32): *i32
    uses unsafe.ptr
{
    unsafe {
        ptr.add(1)  // Move to next i32
    }
}

// Type casting (UNSAFE)
function as_bytes(ptr: *i32): *u8
    uses unsafe.ptr
{
    unsafe {
        ptr.cast<u8>()
    }
}
```

**Pointer Conversions:**

```cantrip
// Safe to raw (loses safety)
let safe: Ptr<i32>@Exclusive = Ptr.new(&x)

unsafe {
    let raw: *i32 = safe.as_raw()
    // raw has no safety guarantees
}

// Raw to safe (requires programmer verification)
function wrap_raw(raw: *mut i32): Ptr<i32>@Exclusive
    must raw != null<i32>()
    must valid(raw)
    must unique(raw)
    uses unsafe.ptr
{
    unsafe {
        // Programmer MUST ensure:
        // - raw is valid
        // - raw is unique
        // - no other aliases exist
        Ptr.from_raw(raw)
    }
}
```

**Effect Exclusion Example:**

```cantrip
region temp {
    var x = 42
    let ptr: Ptr<i32>@Exclusive = Ptr.new_in<temp>(&x)

    {
        // Reading through pointer
        let val = ptr.read()  // Acquires read<i32>

        // Cannot mutate x while read effect active
        // x = 100  // ❌ ERROR: uses write<i32>, conflicts with read<i32>

    } // read<i32> released

    // Now can write
    x = 100  // ✓ OK: no read<i32> active
}
```

---

## 9.3 Static Semantics

### 9.3.1 Well-Formedness Rules

**Definition 9.2 (Pointer Well-Formedness):** A pointer type is well-formed if its pointee type is well-formed and, for safe pointers, the state annotation is valid.

**Safe Pointer Types:**

```
[WF-Ptr-Type]
Γ ⊢ T : Type
S ∈ {Exclusive, Shared, Frozen}
────────────────────────────────────
Γ ⊢ Ptr<T>@S : Type

[WF-Ptr-State-Exclusive]
Γ ⊢ Ptr<T>@Exclusive : Type
────────────────────────────────────
internal_fields: { addr: *T, region: RegionId }
(region binding tracked by compiler, not stored)

[WF-Ptr-State-Shared]
Γ ⊢ Ptr<T>@Shared : Type
────────────────────────────────────
internal_fields: { addr: *T, region: RegionId, count: StaticCount }
(count is static analysis only, not runtime)

[WF-Ptr-State-Frozen]
Γ ⊢ Ptr<T>@Frozen : Type
────────────────────────────────────
internal_fields: { addr: *T, region: RegionId }
```

**Raw Pointer Types:**

```
[WF-RawPtr-Immut]
Γ ⊢ T : Type
────────────────────────────────────
Γ ⊢ *T : Type

[WF-RawPtr-Mut]
Γ ⊢ T : Type
────────────────────────────────────
Γ ⊢ *mut T : Type

[WF-Null-Ptr]
Γ ⊢ T : Type
────────────────────────────────────
Γ ⊢ null<T>() : *T
(raw pointers can be null)
```

**Raw Pointer Operations (NEW - P1):**

```
[WF-RawPtr-Deref-Context]
Γ ⊢ ptr : *T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ *ptr well-formed

[WF-RawPtr-Arithmetic-Context]
Γ ⊢ ptr : *T
Γ ⊢ offset : isize | usize
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.offset(offset) well-formed
Γ ⊢ ptr.add(offset) well-formed
Γ ⊢ ptr.sub(offset) well-formed

[WF-RawPtr-Cast-Context]
Γ ⊢ ptr : *T
Γ ⊢ U : Type
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.cast<U>() well-formed

[WF-Ptr-Effect-Context]
Γ ⊢ T : Type
ε ∈ {read<T>, write<T>}
ε ∈ available_effects(Γ)
────────────────────────────────────
effect context ε well-formed for T
```

**Definition 9.3 (Pointer Provenance):** Provenance tracks the origin and valid range of pointers for soundness. All pointers carry provenance metadata (conceptual, not stored at runtime).

**Provenance rules:**
1. Pointers derived from an allocation have provenance of that allocation
2. Pointer arithmetic preserves provenance
3. Casting does not change provenance
4. Accessing outside provenance bounds is undefined behavior

**Definition 9.4 (Effect Exclusion):** For any type T, the read and write effects on T are mutually exclusive:

```
Axiom (Effect Exclusion):
∀ T ∈ Type. ¬(read<T> ∧ write<T>)

Formally:
  If read<T> ∈ active_effects(Γ), then write<T> ∉ active_effects(Γ)
  If write<T> ∈ active_effects(Γ), then read<T> ∉ active_effects(Γ)

This axiom prevents aliasing bugs by making simultaneous reading and writing
to the same type impossible at compile time.
```

**Definition 9.5 (Null Pointer):** A null pointer is a pointer with address 0x0 that does not reference any valid memory location. Dereferencing a null pointer causes undefined behavior.

```
Formal definition:
  null<T>() = *T { addr: 0x0 }

Properties:
  ∀ T. null<T>() : *T
  ∀ T, ptr: *T. (ptr == null<T>()) ⟺ (ptr.addr == 0x0)
  ⟨*null<T>(), σ⟩ ⇒ UNDEFINED BEHAVIOR
```

**Definition 9.6 (Pointer Validity):** A pointer ptr is valid to dereference if and only if:

```
valid(ptr) ⟺
  ptr.addr ≠ 0x0                              ∧    (not null)
  ptr.addr % alignof(T) == 0                  ∧    (properly aligned)
  ptr.addr ∈ allocated_memory(σ)              ∧    (points to allocated memory)
  ¬freed(ptr.addr, σ)                         ∧    (not freed)
  ptr.addr ∈ provenance_bounds(ptr)                (within provenance)

Where:
  allocated_memory(σ) = set of currently allocated memory addresses
  freed(addr, σ) = true if address was allocated but has been freed
  provenance_bounds(ptr) = valid address range for pointer's provenance
```



#### 9.3.2 Type Rules

This section presents all typing rules for pointer operations, organized by category.

##### Safe Pointer Operations

**[T-Ptr-New-Heap] Heap pointer creation:**
```
[T-Ptr-New-Heap]
Γ ⊢ value : T
¬in_region_context(Γ)
────────────────────────────────────────────────────
Γ ⊢ Ptr.new(&value) : Ptr<T>@Exclusive ! alloc.heap
(region = heap)
```

**[T-Ptr-New-Region] Region pointer creation:**
```
[T-Ptr-New-Region]
Γ ⊢ value : T
region r ∈ Γ
value allocated in region r
────────────────────────────────────────────────────
Γ ⊢ Ptr.new_in<r>(&value) : Ptr<T>@Exclusive@r
(internally tagged as belonging to region r)
```

**[T-Ptr-Share] Sharing transition:**
```
[T-Ptr-Share]
Γ ⊢ ptr : Ptr<T>@Exclusive
────────────────────────────────────────────────────
Γ ⊢ ptr.share() : (Ptr<T>@Shared, Ptr<T>@Shared)
(consumes Exclusive, produces two Shared)
```

**[T-Ptr-Freeze] Freezing transition:**
```
[T-Ptr-Freeze]
Γ ⊢ ptr : Ptr<T>@Exclusive
────────────────────────────────────────────────────
Γ ⊢ ptr.freeze() : Ptr<T>@Frozen
(consumes Exclusive, produces Frozen)
```

**[T-Ptr-Clone-Shared] Clone shared pointer:**
```
[T-Ptr-Clone-Shared]
Γ ⊢ ptr : Ptr<T>@Shared
────────────────────────────────────────────────────
Γ ⊢ ptr.clone() : Ptr<T>@Shared
(increments static reference count)
```

**[T-Ptr-Clone-Frozen] Clone frozen pointer:**
```
[T-Ptr-Clone-Frozen]
Γ ⊢ ptr : Ptr<T>@Frozen
────────────────────────────────────────────────────
Γ ⊢ ptr.clone() : Ptr<T>@Frozen
(frozen pointers can be freely cloned)
```

**[T-Ptr-Read-Exclusive] Read through exclusive pointer:**
```
[T-Ptr-Read-Exclusive]
Γ ⊢ ptr : Ptr<T>@Exclusive
read<T> ∈ available_effects(Γ)
valid(ptr.addr)
────────────────────────────────────────────────────
Γ ⊢ ptr.read() : T ! read<T>
```

**[T-Ptr-Read-Shared] Read through shared pointer:**
```
[T-Ptr-Read-Shared]
Γ ⊢ ptr : Ptr<T>@Shared
read<T> ∈ available_effects(Γ)
valid(ptr.addr)
────────────────────────────────────────────────────
Γ ⊢ ptr.read() : T ! read<T>
```

**[T-Ptr-Read-Frozen] Read through frozen pointer:**
```
[T-Ptr-Read-Frozen]
Γ ⊢ ptr : Ptr<T>@Frozen
read<T> ∈ available_effects(Γ)
valid(ptr.addr)
────────────────────────────────────────────────────
Γ ⊢ ptr.read() : T ! read<T>
```

**[T-Ptr-Write] Write through exclusive pointer:**
```
[T-Ptr-Write]
Γ ⊢ ptr : Ptr<T>@Exclusive
Γ ⊢ value : T
write<T> ∈ available_effects(Γ)
¬(read<T> ∈ active_effects(Γ))    (exclusion check)
valid(ptr.addr)
────────────────────────────────────────────────────
Γ ⊢ ptr.write(value) : () ! write<T>
```

**[T-Ptr-As-Raw] Convert to raw pointer:**
```
[T-Ptr-As-Raw]
Γ ⊢ ptr : Ptr<T>@S
unsafe_context(Γ)
────────────────────────────────────────────────────
Γ ⊢ ptr.as_raw() : *T
(loses all safety guarantees)
```

**[T-Ptr-Escape-Check] Region escape prevention:**
```
[T-Ptr-Escape-Check]
Γ ⊢ ptr : Ptr<T>@S
ptr internally tagged with region r
ptr escapes region r
────────────────────────────────────────────────────
ERROR: Cannot return region-scoped pointer
(uses existing escape analysis from §29)
```

**[T-Ptr-With-Permission] Pointers with permissions:**
```
[T-Ptr-With-Permission]
Γ ⊢ ptr : Ptr<T>@S
π ∈ {own, mut, iso}
────────────────────────────────────────────────────
Γ ⊢ ptr : π Ptr<T>@S
(permissions apply to pointer itself, not pointee)
```

**[T-Ptr-Effect-Read] Read effect typing:**
```
[T-Ptr-Effect-Read]
Γ ⊢ e : Ptr<T>@S
S ∈ {Exclusive, Shared, Frozen}
────────────────────────────────────────────────────
effect(e.read()) = read<T>
```

**[T-Ptr-Effect-Write] Write effect typing:**
```
[T-Ptr-Effect-Write]
Γ ⊢ e : Ptr<T>@Exclusive
────────────────────────────────────────────────────
effect(e.write(v)) = write<T>
```

**[T-Ptr-Effect-Exclusion] Effect exclusion enforcement:**
```
[T-Ptr-Effect-Exclusion]
read<T> ∈ active_effects(Γ)
Γ ⊢ e : expr requiring write<T>
────────────────────────────────────────────────────
ERROR: Cannot acquire write<T> while read<T> active

[T-Ptr-Effect-Exclusion-Reverse]
write<T> ∈ active_effects(Γ)
Γ ⊢ e : expr requiring read<T>
────────────────────────────────────────────────────
ERROR: Cannot acquire read<T> while write<T> active
```

**[T-Ptr-Modal-Transition] State transition validity:**
```
[T-Ptr-Modal-Transition]
Γ ⊢ ptr : Ptr<T>@S₁
S₁ →[method] S₂ ∈ valid_transitions(Ptr)
────────────────────────────────────────────────────
Γ ⊢ ptr.method() : Ptr<T>@S₂
```

**[T-Ptr-Invalid-Transition] Invalid transition prevention:**
```
[T-Ptr-Invalid-Transition]
Γ ⊢ ptr : Ptr<T>@Shared
────────────────────────────────────────────────────
ERROR: Cannot call share() on @Shared pointer
(no transition from Shared to Shared via share())

[T-Ptr-Invalid-Transition-2]
Γ ⊢ ptr : Ptr<T>@Frozen
────────────────────────────────────────────────────
ERROR: Cannot call freeze() on @Frozen pointer
(already frozen)
```

##### Raw Pointer Operations

**[T-RawPtr-Addr] Taking raw address (SAFE):**
```
[T-RawPtr-Addr-Immut]
Γ ⊢ e : T
────────────────────────────────────
Γ ⊢ &raw e : *T

[T-RawPtr-Addr-Mut]
Γ ⊢ e : mut T
────────────────────────────────────
Γ ⊢ &raw mut e : *mut T
```

**[T-RawPtr-Deref] Dereferencing (UNSAFE):**
```
[T-RawPtr-Deref-Immut]
Γ ⊢ ptr : *T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ *ptr : T ! unsafe.ptr

[T-RawPtr-Deref-Mut]
Γ ⊢ ptr : *mut T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ *ptr : mut T ! unsafe.ptr
```

**[T-RawPtr-Write] Writing through pointer (UNSAFE):**
```
[T-RawPtr-Write]
Γ ⊢ ptr : *mut T
Γ ⊢ value : T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ *ptr = value : () ! unsafe.ptr
```

**[T-RawPtr-Offset] Pointer arithmetic (UNSAFE):**
```
[T-RawPtr-Offset]
Γ ⊢ ptr : *T
Γ ⊢ offset : isize
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.offset(offset) : *T ! unsafe.ptr

[T-RawPtr-Add]
Γ ⊢ ptr : *T
Γ ⊢ count : usize
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.add(count) : *T ! unsafe.ptr

[T-RawPtr-Sub]
Γ ⊢ ptr : *T
Γ ⊢ count : usize
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.sub(count) : *T ! unsafe.ptr
```

**[T-RawPtr-Cast] Type casting (UNSAFE):**
```
[T-RawPtr-Cast]
Γ ⊢ ptr : *T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
────────────────────────────────────
Γ ⊢ ptr.cast<U>() : *U ! unsafe.ptr
```

**[T-RawPtr-Null] Null pointer:**
```
[T-RawPtr-Null]
Γ ⊢ T : Type
────────────────────────────────────
Γ ⊢ null<T>() : *T
```

**[T-RawPtr-Compare] Pointer comparison:**
```
[T-RawPtr-Compare]
Γ ⊢ ptr1 : *T
Γ ⊢ ptr2 : *T
────────────────────────────────────
Γ ⊢ ptr1 == ptr2 : bool
Γ ⊢ ptr1 != ptr2 : bool
```

**[T-RawPtr-NoSafety] No safety checking:**
```
[T-RawPtr-NoSafety]
Raw pointers provide NO compile-time safety:
- No aliasing checks (multiple mutable pointers allowed)
- No lifetime checks (can outlive pointee)
- No null checks (dereferencing null is UB)
- No bounds checks (out-of-bounds is UB)
- Programmer responsible for all correctness
```

##### Pointer Conversions

**[T-Ptr-To-Raw] Safe to raw conversion:**
```
[T-Ptr-To-Raw]
Γ ⊢ ptr : Ptr<T>@S
unsafe_context(Γ)
────────────────────────────────────
Γ ⊢ ptr.as_raw() : *T
(loses all modal states and safety guarantees)
```

**[T-Raw-To-Ptr] Raw to safe conversion (UNSAFE):**
```
[T-Raw-To-Ptr]
Γ ⊢ raw : *T
unsafe_context(Γ)
unsafe.ptr ∈ available_effects(Γ)
programmer ensures: valid(raw) ∧ unique(raw) ∧ ¬escaped(raw)
────────────────────────────────────
Γ ⊢ Ptr.from_raw(raw) : Ptr<T>@Exclusive ! unsafe.ptr
```

**Preconditions (programmer responsibility):**
1. Pointer is valid (not null, properly aligned)
2. Pointer is unique (no other aliases exist)
3. Pointed-to memory has not escaped
4. Pointed-to memory is properly initialized

**[T-RawPtr-Cast-Types] Casting between raw pointer types:**
```
[T-RawPtr-Cast-Types]
Γ ⊢ ptr : *T
unsafe_context(Γ)
────────────────────────────────────
Γ ⊢ ptr.cast<U>() : *U ! unsafe.ptr
```

**Safety considerations:**
- Alignment must be respected (casting from more-aligned to less-aligned is safe)
- Size compatibility is programmer responsibility
- Type aliasing rules apply (accessing `T` through `*U` may violate type system)


#### 9.3.3 Type Properties

This section presents key theorems about the pointer type system and their proof sketches.

##### Safe Pointer Properties

**Theorem 9.1 (Effect Exclusion Safety):**

If `read<T>` is active in context Γ, then no expression requiring `write<T>` can be typed in Γ, and vice versa.

**Proof sketch:** By induction on typing derivations. The [T-Ptr-Effect-Exclusion] rules explicitly check for conflicting effects and reject programs that would violate exclusion. Effect activation and deactivation follow lexical scoping (effect released when variable goes out of scope).

**Corollary 9.1 (No Aliased Mutation):**

If multiple pointers (Shared or Frozen) point to the same type T, mutation of T through any pointer or direct access is prevented at compile time.

**Theorem 9.2 (State Machine Validity):**

All pointer state transitions follow the modal state machine, and no invalid states are reachable.

**Proof sketch:** Modal type system (§8) enforces valid transitions. Type rules [T-Ptr-Modal-Transition] and [T-Ptr-Invalid-Transition] reject invalid transitions at compile time.

**Theorem 9.3 (Region Escape Safety):**

Pointers created in a region cannot escape that region, preventing use-after-free.

**Proof sketch:** [T-Ptr-Escape-Check] rule integrates with existing region escape analysis (§29). Pointers are internally tagged with their creation region, and escape analysis treats them like any region-allocated value.

**Theorem 9.4 (Zero Runtime Overhead):**

Modal pointer states and effect checking compile to direct memory access with no runtime tracking.

**Proof sketch:**
- Modal states are compile-time only (§8 modal compilation)
- Effects are compile-time only (§21 effect erasure)
- Pointer fields (addr, region) compile to single raw pointer
- Reference counting in @Shared is static analysis only (no runtime counter)

##### Raw Pointer Properties

**Theorem 9.5 (No Safety Guarantees):**

Raw pointer operations provide no compile-time safety guarantees beyond basic type compatibility. All safety is programmer responsibility.

**Proof sketch:** Raw pointers bypass the type system's safety mechanisms:
- No modal states (unlike Ptr<T>)
- No effect exclusion (multiple `*mut T` allowed simultaneously)
- No escape analysis enforcement (can escape regions in unsafe blocks)
- All operations require `unsafe { }` blocks where normal safety rules suspended

**Corollary 9.3 (Unsafe Boundary):**

Safety properties only hold at the boundary where safe code calls unsafe code, not within unsafe blocks themselves.

**Theorem 9.6 (Nullability):**

Raw pointers can be null, unlike references and safe pointers.

**Proof sketch:** `null<T>() : *T` is well-typed. Dereferencing null is undefined behavior, not a type error.

**Theorem 9.7 (Uniform Representation):**

All raw pointer types have identical memory representation regardless of pointed-to type.

**Proof sketch:** `size(*T) = size(*mut T) = size(*U) = sizeof(usize)` for all types T, U. Only the address is stored.

##### Unification Properties

**Theorem 9.8 (Safe-Raw Distinction):**

Safe pointers (`Ptr<T>@S`) and raw pointers (`*T`, `*mut T`) are distinct types with incompatible operations, preventing accidental mixing.

**Proof sketch:** Type rules explicitly distinguish between safe and raw pointer operations. Conversions between them require unsafe blocks and explicit method calls (`.as_raw()`, `Ptr.from_raw()`), preventing implicit conversions.

**Theorem 9.9 (Permission Orthogonality):**

Pointer permissions (`own`, `mut`, `iso`) and pointer states (@Exclusive, @Shared, @Frozen) compose orthogonally without interference.

**Proof sketch:** Permissions apply to the pointer value itself (§11), while states govern aliasing and access (§8). The [T-Ptr-With-Permission] rule shows these are independent dimensions of the type system.


---

### 9.4 Dynamic Semantics

This section presents the runtime behavior and operational semantics of pointer operations.

#### 9.4.1 Evaluation Rules

##### Safe Pointer Evaluation

**[E-Ptr-New-Heap] Heap pointer creation:**
```
[E-Ptr-New-Heap]
⟨value, σ⟩ ⇓ ⟨v@addr, σ₁⟩
────────────────────────────────────────────────────
⟨Ptr.new(&value), σ⟩ ⇓ ⟨Ptr@Exclusive { addr: addr, region: heap }, σ₁⟩
```

**[E-Ptr-New-Region] Region pointer creation:**
```
[E-Ptr-New-Region]
⟨value, σ⟩ ⇓ ⟨v@addr, σ₁⟩
addr ∈ region_bounds(r, σ₁)
────────────────────────────────────────────────────
⟨Ptr.new_in<r>(&value), σ⟩ ⇓ ⟨Ptr@Exclusive { addr: addr, region: r }, σ₁⟩
```

**[E-Ptr-Share] Sharing transition:**
```
[E-Ptr-Share]
⟨ptr, σ⟩ ⇓ ⟨Ptr@Exclusive { addr: a, region: r }, σ₁⟩
────────────────────────────────────────────────────
⟨ptr.share(), σ⟩ ⇓ ⟨(
    Ptr@Shared { addr: a, region: r, count: 1 },
    Ptr@Shared { addr: a, region: r, count: 1 }
  ), σ₁⟩
```

**[E-Ptr-Freeze] Freezing transition:**
```
[E-Ptr-Freeze]
⟨ptr, σ⟩ ⇓ ⟨Ptr@Exclusive { addr: a, region: r }, σ₁⟩
────────────────────────────────────────────────────
⟨ptr.freeze(), σ⟩ ⇓ ⟨Ptr@Frozen { addr: a, region: r }, σ₁⟩
```

**[E-Ptr-Read] Read through pointer:**
```
[E-Ptr-Read]
⟨ptr, σ⟩ ⇓ ⟨Ptr@S { addr: a, ... }, σ₁⟩
S ∈ {Exclusive, Shared, Frozen}
valid(a, σ₁)
σ₁(a) = v
────────────────────────────────────────────────────
⟨ptr.read(), σ⟩ ⇓ ⟨v, σ₁⟩
```

**[E-Ptr-Write] Write through pointer:**
```
[E-Ptr-Write]
⟨ptr, σ⟩ ⇓ ⟨Ptr@Exclusive { addr: a, region: r }, σ₁⟩
⟨value, σ₁⟩ ⇓ ⟨v, σ₂⟩
valid(a, σ₂)
────────────────────────────────────────────────────
⟨ptr.write(value), σ⟩ ⇓ ⟨(), σ₂[a ↦ v]⟩
```

##### Raw Pointer Evaluation

**[E-RawPtr-Addr] Taking address:**
```
[E-RawPtr-Addr]
⟨e, σ⟩ ⇓ ⟨v@addr, σ'⟩
────────────────────────────────────
⟨&raw e, σ⟩ ⇓ ⟨*T { addr: addr }, σ'⟩
```

**[E-RawPtr-Deref] Dereferencing:**
```
[E-RawPtr-Deref]
⟨ptr, σ⟩ ⇓ ⟨*T { addr: a }, σ₁⟩
valid(a, σ₁)    (undefined behavior if invalid)
σ₁(a) = v
────────────────────────────────────
⟨*ptr, σ⟩ ⇓ ⟨v, σ₁⟩
```

**[E-RawPtr-Write] Writing:**
```
[E-RawPtr-Write]
⟨ptr, σ⟩ ⇓ ⟨*mut T { addr: a }, σ₁⟩
⟨value, σ₁⟩ ⇓ ⟨v, σ₂⟩
valid(a, σ₂)    (undefined behavior if invalid)
────────────────────────────────────
⟨*ptr = value, σ⟩ ⇓ ⟨(), σ₂[a ↦ v]⟩
```

**[E-RawPtr-Offset] Pointer arithmetic:**
```
[E-RawPtr-Offset]
⟨ptr, σ⟩ ⇓ ⟨*T { addr: a }, σ₁⟩
⟨offset, σ₁⟩ ⇓ ⟨n, σ₂⟩
────────────────────────────────────
⟨ptr.offset(offset), σ⟩ ⇓ ⟨*T { addr: a + n × sizeof(T) }, σ₂⟩

[E-RawPtr-Add]
⟨ptr, σ⟩ ⇓ ⟨*T { addr: a }, σ₁⟩
⟨count, σ₁⟩ ⇓ ⟨n, σ₂⟩
────────────────────────────────────
⟨ptr.add(count), σ⟩ ⇓ ⟨*T { addr: a + n × sizeof(T) }, σ₂⟩

[E-RawPtr-Sub]
⟨ptr, σ⟩ ⇓ ⟨*T { addr: a }, σ₁⟩
⟨count, σ₁⟩ ⇓ ⟨n, σ₂⟩
────────────────────────────────────
⟨ptr.sub(count), σ⟩ ⇓ ⟨*T { addr: a - n × sizeof(T) }, σ₂⟩
```

**[E-RawPtr-Cast] Type casting:**
```
[E-RawPtr-Cast]
⟨ptr, σ⟩ ⇓ ⟨*T { addr: a }, σ'⟩
────────────────────────────────────
⟨ptr.cast<U>(), σ⟩ ⇓ ⟨*U { addr: a }, σ'⟩
```

**[E-RawPtr-Null] Null pointer:**
```
[E-RawPtr-Null]
────────────────────────────────────
⟨null<T>(), σ⟩ ⇓ ⟨*T { addr: 0x0 }, σ⟩
```

##### Conversion Evaluation

**[E-Ptr-To-Raw] Safe to raw conversion:**
```
[E-Ptr-To-Raw]
⟨ptr, σ⟩ ⇓ ⟨Ptr@S { addr: a, ... }, σ'⟩
────────────────────────────────────
⟨ptr.as_raw(), σ⟩ ⇓ ⟨*T { addr: a }, σ'⟩
```

**[E-Raw-To-Ptr] Raw to safe conversion:**
```
[E-Raw-To-Ptr]
⟨raw, σ⟩ ⇓ ⟨*T { addr: a }, σ'⟩
valid(a, σ') ∧ unique(a, σ') ∧ ¬escaped(a, σ')    (programmer responsibility)
────────────────────────────────────
⟨Ptr.from_raw(raw), σ⟩ ⇓ ⟨Ptr@Exclusive { addr: a, region: heap }, σ'⟩
```

#### 9.4.2 Memory Representation

##### Safe Pointer Layout

**Modal states compile away:**

```
Compile-time:
  Ptr<T>@Exclusive  →  1 pointer (8 bytes on 64-bit)
  Ptr<T>@Shared     →  1 pointer (8 bytes on 64-bit)
  Ptr<T>@Frozen     →  1 pointer (8 bytes on 64-bit)

Runtime layout (all states):
┌────────────────┐
│ addr: *T (8B)  │
└────────────────┘
Total: 8 bytes (64-bit)
Alignment: 8 bytes

Modal state: ❌ NOT stored (compile-time only)
Region tag:  ❌ NOT stored (compile-time only)
Count field: ❌ NOT stored (static analysis only)
```

**Key insight:** Despite having different states (@Exclusive, @Shared, @Frozen) with different fields in the type system, ALL pointer states compile to a single raw pointer at runtime. The state machine and reference counting are purely compile-time constructs.

##### Raw Pointer Layout

```
Raw pointer (all types):
┌────────────────┐
│ addr (word)    │  4 bytes (32-bit) or 8 bytes (64-bit)
└────────────────┘

Total: sizeof(usize)
Alignment: alignof(usize)
```

**Key properties:**
- Same size as machine word (platform-dependent)
- `*T` and `*mut T` have identical layout (mutability is compile-time only)
- No metadata (unlike fat pointers for slices)
- Direct memory address, no indirection

**Null representation:**
```
null pointer = { addr: 0x0 }
```

**Comparison with fat pointers:**
```
Thin pointer (*T):     8 bytes (64-bit)
Fat pointer ([T]):     16 bytes (ptr + len)
```

#### 9.4.3 Operational Behavior

##### Pointer Validity Conditions

A pointer `ptr : *T` is valid for dereferencing if and only if:

```
valid(ptr.addr, σ) ⟺
  ptr.addr ≠ 0x0                              ∧    (not null)
  ptr.addr % alignof(T) == 0                  ∧    (properly aligned)
  ptr.addr ∈ allocated_memory(σ)              ∧    (points to allocated memory)
  ¬freed(ptr.addr, σ)                         ∧    (not freed)
  ptr.addr ∈ provenance_bounds(ptr)                (within provenance)
```

##### Undefined Behavior

**Operations that cause undefined behavior (UB):**

**1. Dereferencing null:**
```cantrip
unsafe {
    let ptr: *i32 = null<i32>()
    let x = *ptr  // ⚠️ UNDEFINED BEHAVIOR
}
```

**Formal specification:**
```
⟨*ptr, σ⟩ where ptr.addr = 0x0 ⟹ UB
```

**2. Out-of-bounds access:**
```cantrip
unsafe {
    let arr = [1, 2, 3]
    let ptr = &raw arr[0]
    let ptr2 = ptr.offset(10)  // Points outside array
    let x = *ptr2  // ⚠️ UNDEFINED BEHAVIOR
}
```

**Formal specification:**
```
⟨*ptr, σ⟩ where ptr.addr ∉ allocated_memory(σ) ⟹ UB
```

**3. Use-after-free:**
```cantrip
unsafe {
    let ptr = malloc(4)
    free(ptr)
    let x = *ptr  // ⚠️ UNDEFINED BEHAVIOR (dangling pointer)
}
```

**Formal specification:**
```
⟨*ptr, σ⟩ where freed(ptr.addr, σ) ⟹ UB
```

**4. Unaligned access:**
```cantrip
unsafe {
    let bytes: [u8; 8] = [0; 8]
    let ptr: *u64 = (&raw bytes[1]).cast<u64>()
    let x = *ptr  // ⚠️ UNDEFINED BEHAVIOR (misaligned)
}
```

**Formal specification:**
```
⟨*ptr, σ⟩ where ptr.addr % alignof(T) ≠ 0 ⟹ UB
```

**5. Data races:**
```cantrip
unsafe {
    // Thread 1: *ptr_mut = 42
    // Thread 2: *ptr_mut = 100
    // ⚠️ UNDEFINED BEHAVIOR (data race)
}
```

**Formal specification:**
```
concurrent_write(ptr.addr, σ₁) ∧ concurrent_access(ptr.addr, σ₂) ⟹ UB
```

**6. Invalid pointer arithmetic:**
```cantrip
unsafe {
    let ptr: *i32 = null<i32>()
    let ptr2 = ptr.add(1)  // ⚠️ UNDEFINED BEHAVIOR (arithmetic on null)
}
```

**Formal specification:**
```
⟨ptr.offset(n), σ⟩ where ptr.addr ∉ valid_provenance(ptr) ⟹ UB
```

##### Safe vs Raw Pointer Behavior

**Compile-time guarantees:**

| Operation | Safe Pointer | Raw Pointer |
|-----------|--------------|-------------|
| Null safety | ✅ Never null | ❌ Can be null |
| Aliasing | ✅ Effect exclusion enforced | ❌ No tracking |
| Region escape | ✅ Prevented | ❌ Not checked in unsafe |
| Alignment | ✅ Guaranteed by type | ⚠️ Programmer responsibility |
| Bounds | ✅ Via effects + ownership | ❌ Programmer responsibility |
| Lifetime | ✅ Tracked via regions | ❌ Programmer responsibility |

**Runtime representation:**

Both safe and raw pointers compile to identical machine code—a single pointer value. All safety is enforced statically.


---

## 9.5 Null Pointer Handling

### 9.5.1 Overview

Cantrip provides two distinct approaches to handling potentially-absent pointers:

1. **Safe pointers with Option<Ptr<T>>**: Explicit nullability using sum types
2. **Nullable raw pointers**: C-compatible null pointers (`*T`, `*mut T`)

**Key difference:**
- Safe pointers (`Ptr<T>`) **cannot be null** - nullability must be explicit via `Option<Ptr<T>>`
- Raw pointers can be null and require manual null checking in unsafe code

### 9.5.2 Safe Approach: Option<Ptr<T>>

**Type:** `Option<Ptr<T>@State>`

Safe pointers use Cantrip's `Option<T>` type to represent nullable pointers:

```cantrip
// Safe nullable pointer
function find_node(key: i32): Option<Ptr<Node>@Shared>
    uses read<HashMap>
{
    match self.lookup(key) {
        Some(node) => Some(node),
        None => None
    }
}

function use_node() {
    match find_node(42) {
        Some(node) => {
            // Statically guaranteed: node is not null
            let value = node.read().value
        }
        None => {
            // Explicitly handle absence
        }
    }
}
```

**Benefits:**
- ✅ Compile-time enforcement of null checks
- ✅ Pattern matching prevents null dereferences
- ✅ Type system tracks nullability
- ✅ Cannot forget to check for null

**When to use:**
- Safe Cantrip code
- Data structures with optional references
- APIs where absence is a common case

### 9.5.3 Unsafe Approach: Nullable Raw Pointers

**Type:** `*T` or `*mut T` with value `null<T>()`

Raw pointers can be null and require manual checking:

**Type rule (from §9.3.2):**
```
[T-RawPtr-Null]
Γ ⊢ T : Type
────────────────────────────────────
Γ ⊢ null<T>() : *T
```

**Evaluation rule (from §9.4.1):**
```
[E-RawPtr-Null]
────────────────────────────────────
⟨null<T>(), σ⟩ ⇓ ⟨*T { addr: 0x0 }, σ⟩
```

**Example:**
```cantrip
extern "C" {
    function malloc(size: usize): *mut u8
        uses ffi.call, unsafe.ptr
}

function allocate_buffer(size: usize): Option<*mut u8>
    uses ffi.call, unsafe.ptr
{
    unsafe {
        let ptr = malloc(size)

        // Manual null check required
        if ptr == null<u8>() {
            return None
        }

        Some(ptr)
    }
}
```

**Danger:**
- ⚠️ Dereferencing null causes undefined behavior
- ⚠️ Compiler cannot enforce null checks
- ⚠️ Easy to forget to check for null

**When to use:**
- FFI with C libraries
- Manual memory management
- Performance-critical unsafe code
- When C compatibility required

### 9.5.4 Null Checking Patterns

#### Pattern 1: Comparison with null

```cantrip
unsafe {
    let ptr: *mut i32 = get_pointer()

    if ptr != null<i32>() {
        // Safe to dereference
        *ptr = 42
    }
}
```

#### Pattern 2: Converting to Option

```cantrip
function ptr_to_option<T>(ptr: *T): Option<*T> {
    if ptr == null<T>() {
        None
    } else {
        Some(ptr)
    }
}

unsafe {
    let ptr: *i32 = c_function()

    match ptr_to_option(ptr) {
        Some(p) => {
            // Use pointer
            let value = *p
        }
        None => {
            // Handle null case
        }
    }
}
```

#### Pattern 3: Early return on null

```cantrip
function process_ptr(ptr: *mut Data): Result<(), Error>
    uses unsafe.ptr
{
    unsafe {
        if ptr == null<Data>() {
            return Err(Error::NullPointer)
        }

        // Proceed with non-null pointer
        (*ptr).process()
        Ok(())
    }
}
```

#### Pattern 4: Null propagation (manual)

```cantrip
unsafe {
    let ptr1: *Node = get_first()
    if ptr1 == null<Node>() { return null<Node>(); }

    let ptr2: *Node = (*ptr1).next
    if ptr2 == null<Node>() { return null<Node>(); }

    ptr2
}
```

### 9.5.5 Comparison: Option<Ptr<T>> vs Nullable Raw Pointers

| Aspect | Option<Ptr<T>@S> | *T (nullable) |
|--------|------------------|---------------|
| **Null safety** | Compile-time enforced | Manual checking required |
| **Type system** | `Option` tracks nullability | Type doesn't indicate nullability |
| **Dereferencing null** | Impossible (compile error) | Undefined behavior |
| **Pattern matching** | ✅ Supported | ❌ Not supported |
| **FFI compatibility** | ❌ Not C-compatible | ✅ C-compatible |
| **Performance** | Same (Option<Ptr> optimized to nullable pointer) | Zero overhead |
| **Context** | Safe code | `unsafe { }` blocks |
| **When to use** | Safe Cantrip code | FFI, manual memory management |

### 9.5.6 Null Pointer Representation

**Memory layout:**
```
Null pointer:
┌────────────────┐
│ 0x00000000     │  (32-bit)
└────────────────┘

┌────────────────────────┐
│ 0x0000000000000000     │  (64-bit)
└────────────────────────────┘
```

**Option<Ptr<T>> optimization:**
```
Some(ptr):
┌────────────────┐
│ 0xNNNNNNNN     │  Non-zero address
└────────────────┘

None:
┌────────────────┐
│ 0x00000000     │  Null representation
└────────────────┘
```

Cantrip optimizes `Option<Ptr<T>>` to use the null representation for `None`, making it the same size as a raw pointer.

### 9.5.7 Undefined Behavior: Dereferencing Null

**From §9.4.3 - Undefined Behavior:**

Dereferencing a null pointer causes undefined behavior:

```cantrip
unsafe {
    let ptr: *i32 = null<i32>()
    let x = *ptr  // ⚠️ UNDEFINED BEHAVIOR
}
```

**Safe alternative using Option:**
```cantrip
let ptr_opt: Option<Ptr<i32>@Shared> = None

match ptr_opt {
    Some(ptr) => {
        let x = ptr.read()  // ✅ Safe - cannot be null
    }
    None => {
        // Compile-time enforced handling
    }
}
```

### 9.5.8 FFI Null Pointer Convention

**C NULL convention:**
```
C: NULL  →  Cantrip: null<T>()
```

**Example with C library:**
```cantrip
extern "C" {
    // Returns NULL on failure
    function fopen(path: *u8, mode: *u8): *mut File
        uses ffi.call, unsafe.ptr

    function fclose(file: *mut File)
        uses ffi.call, unsafe.ptr
}

function open_file(path: &str): Result<*mut File, Error>
    uses ffi.call, unsafe.ptr
{
    unsafe {
        let c_path = path.as_ptr()
        let c_mode = "r".as_ptr()

        let file = fopen(c_path, c_mode)

        if file == null<File>() {
            Err(Error::OpenFailed)
        } else {
            Ok(file)
        }
    }
}
```

### 9.5.9 Best Practices

**DO:**
- ✅ Use `Option<Ptr<T>>` in safe code
- ✅ Always check raw pointers for null before dereferencing
- ✅ Convert raw pointers to `Option` at FFI boundaries
- ✅ Use pattern matching for `Option<Ptr<T>>`
- ✅ Document when functions can return null

**DON'T:**
- ❌ Dereference raw pointers without null check
- ❌ Assume non-null without verification
- ❌ Use raw pointers in safe code (use `Option<Ptr<T>>`)
- ❌ Forget to handle `None` case in pattern matching

**Example - Proper FFI wrapper:**
```cantrip
// C function that may return NULL
extern "C" {
    function c_get_data(): *mut Data
        uses ffi.call, unsafe.ptr
}

// Safe Cantrip wrapper
function get_data(): Option<own Data>
    uses ffi.call, unsafe.ptr, alloc.heap
{
    unsafe {
        let ptr = c_get_data()

        if ptr == null<Data>() {
            None
        } else {
            // Convert to safe ownership
            Some(own *ptr)  // Copy data, take ownership
        }
    }
}

// Now safe code can use it without unsafe:
match get_data() {
    Some(data) => println("Got: {}", data),
    None => println("No data available")
}
```

---

## 9.6 Comparison Table

| Feature | Ptr<T>@State | *T / *mut T |
|---------|--------------|-------------|
| **Safety** | Modal states + effect exclusion | No safety guarantees |
| **Aliasing** | Compile-time checks via effects | No checks, any aliasing allowed |
| **Null** | Cannot be null | Can be null |
| **Context** | Normal safe code | `unsafe { }` blocks only |
| **Effects** | `read<T>`, `write<T>` (type-specific) | `unsafe.ptr` (blanket) |
| **Arithmetic** | Not allowed | Allowed (`offset`, `add`, `sub`) |
| **Casting** | Type-safe via `as_raw()` | Arbitrary casting allowed |
| **Regions** | Escape analysis enforced | No enforcement in unsafe |
| **FFI** | Via `as_raw()` conversion | Direct C interop |
| **Performance** | Zero overhead (compiles to raw) | Zero overhead |
| **Dereferencing** | Type-safe `read()`/`write()` | Unsafe `*ptr` |
| **Use case** | Safe data structures | FFI, manual memory, unsafe code |
| **Runtime cost** | None (same as raw pointer) | None |

---

## 9.7 Advanced Features

### 9.7.1 Pointer Provenance

**Definition 9.3 (Provenance):** Provenance tracks the origin and valid range of pointers for soundness. All pointers carry provenance metadata (conceptual, not stored at runtime).

**Rules:**
1. Pointers derived from an allocation have provenance of that allocation
2. Pointer arithmetic preserves provenance
3. Casting does not change provenance
4. Accessing outside provenance bounds is undefined behavior

**Example:**
```cantrip
unsafe {
    let x = [1, 2, 3]
    let y = [4, 5, 6]

    let px = &raw x[0]
    let py = &raw y[0]

    // px has provenance of x allocation
    // px.offset(2) is within x's provenance (valid)
    // px.offset(100) is OUTSIDE x's provenance (UB)

    // Cannot use px to access y (different provenance)
}
```

### 9.7.2 FFI Attributes

**#[repr(C)] for C-compatible layout:**
```cantrip
#[repr(C)]
record CStruct {
    field1: i32
    field2: *u8  // Raw pointer in C-compatible layout
}

// Guaranteed to match C struct layout:
// struct CStruct {
//     int32_t field1
//     uint8_t* field2
// }
```

**#[null_terminated] for C strings:**
```cantrip
type CString = *u8

function strlen(s: CString): usize
    must s != null<u8>()
    uses unsafe.ptr
{
    unsafe {
        var len = 0
        var ptr = s
        while *ptr != 0 {
            len += 1
            ptr = ptr.add(1)
        }
        len
    }
}
```

### 9.7.3 Volatile Operations

For hardware access (memory-mapped I/O):
```cantrip
function read_volatile<T>(ptr: *T): T
    uses unsafe.ptr
{
    unsafe {
        // Compiler must not optimize away or reorder this read
        @volatile *ptr
    }
}

function write_volatile<T>(ptr: *mut T, value: T)
    uses unsafe.ptr
{
    unsafe {
        // Compiler must not optimize away or reorder this write
        @volatile *ptr = value
    }
}

// Example: Memory-mapped register
const UART_DATA: *mut u8 = 0x4000_0000 as *mut u8

function send_uart_byte(byte: u8)
    uses unsafe.ptr
{
    write_volatile(UART_DATA, byte)
}
```

### 9.7.4 Alignment

**Checking alignment:**
```cantrip
function is_aligned<T>(ptr: *T): bool
    uses unsafe.ptr
{
    unsafe {
        (ptr as usize) % align_of::<T>() == 0
    }
}
```

**Enforcing alignment with attributes:**
```cantrip
#[repr(align(64))]
record CacheLine {
    data: [u8; 64]
}

// Pointers to CacheLine are guaranteed 64-byte aligned
```

---

## 9.8 Examples and Patterns

### 9.8.1 Self-Referential Records (Safe Pointers)

**Problem**: Creating data structures that contain pointers to their own fields while maintaining memory safety.

**Solution**: Use frozen safe pointers to enable safe self-references.

```cantrip
record SelfRef {
    data: Vec<u8>,
    self_ptr: Ptr<Vec<u8>>@Frozen,

    procedure new(capacity: usize): own SelfRef
        must capacity > 0
        will result.self_ptr points to result.data
        uses alloc.heap
    {
        var this = SelfRef {
            data: Vec.with_capacity(capacity),
            self_ptr: Ptr.new(&Vec.new()),  // Temporary
        }

        // Create exclusive pointer to data
        let temp_ptr: Ptr<Vec<u8>>@Exclusive = Ptr.new(&this.data)

        // Freeze it permanently
        this.self_ptr = temp_ptr.freeze()

        this
    }

    procedure get_via_ptr(self: SelfRef): Vec<u8>
        uses read<Vec<u8>>
    {
        // Read through frozen pointer
        self.self_ptr.read()
    }

    procedure modify_directly(self: mut SelfRef, byte: u8)
        uses write<Vec<u8>>  // ❌ Conflicts with read if active!
    {
        self.data.push(byte)
    }
}

// Usage
let s = SelfRef.new(1024)

// Can read through pointer
{
    let data = s.get_via_ptr()  // Acquires read<Vec<u8>>
    // s.modify_directly(42)  // ❌ ERROR: read<Vec<u8>> active
} // read<Vec<u8>> released

// Now can modify
s.modify_directly(42)  // ✓ OK
```

**Key insights:**
- Frozen pointers enable read-only self-references
- Effect exclusion prevents simultaneous read and write
- Zero runtime overhead - compiles to plain pointer

### 9.8.2 FFI Wrapper Pattern (Raw Pointers)

**Wrapping C library safely:**
```cantrip
// C library interface
extern "C" {
    function c_buffer_create(size: usize): *mut u8
        uses ffi.call, unsafe.ptr

    function c_buffer_write(buf: *mut u8, offset: usize, data: *u8, len: usize): i32
        uses ffi.call, unsafe.ptr

    function c_buffer_destroy(buf: *mut u8)
        uses ffi.call, unsafe.ptr
}

// Safe Cantrip wrapper
record Buffer {
    ptr: *mut u8
    size: usize

    procedure new(size: usize): Result<own Buffer, Error>
        uses ffi.call, unsafe.ptr
    {
        unsafe {
            let ptr = c_buffer_create(size)
            if ptr == null<u8>() {
                return Err(Error::AllocationFailed)
            }
            Ok(own Buffer { ptr, size })
        }
    }

    procedure write(self: mut Buffer, offset: usize, data: [u8]): Result<(), Error>
        must offset + data.len() <= self.size
        uses ffi.call, unsafe.ptr
    {
        unsafe {
            let result = c_buffer_write(
                self.ptr,
                offset,
                &raw data[0],
                data.len()
            )
            if result == 0 {
                Ok(())
            } else {
                Err(Error::WriteFailed)
            }
        }
    }

    procedure destroy(self: own Buffer)
        uses ffi.call, unsafe.ptr
    {
        unsafe {
            c_buffer_destroy(self.ptr)
        }
    }
}
```

### 9.8.3 Intrusive Linked List Pattern (Raw Pointers)

**Doubly-linked list with embedded pointers:**
```cantrip
record Node<T> {
    value: T
    next: *mut Node<T>
    prev: *mut Node<T>
}

record LinkedList<T> {
    head: *mut Node<T>
    tail: *mut Node<T>
    len: usize

    procedure new(): own LinkedList<T> {
        own LinkedList {
            head: null<Node<T>>(),
            tail: null<Node<T>>(),
            len: 0,
        }
    }

    procedure push_back(self: mut LinkedList<T>, value: T)
        uses alloc.heap, unsafe.ptr
    {
        unsafe {
            // Allocate raw memory for new node
            let node_ptr = malloc(size_of::<Node<T>>()) as *mut Node<T>

            // Initialize the node in place
            *node_ptr = Node {
                value,
                next: null<Node<T>>(),
                prev: self.tail,
            }

            // Link into list
            if self.tail != null<Node<T>>() {
                (*self.tail).next = node_ptr
            } else {
                self.head = node_ptr
            }

            self.tail = node_ptr
            self.len += 1
        }
    }

    procedure pop_back(self: mut LinkedList<T>): Option<T>
        uses unsafe.ptr
    {
        if self.tail == null<Node<T>>() {
            return None
        }

        unsafe {
            let tail_ptr = self.tail

            // Read the value before deallocating
            let value = (*tail_ptr).value

            // Unlink from list
            self.tail = (*tail_ptr).prev
            if self.tail != null<Node<T>>() {
                (*self.tail).next = null<Node<T>>()
            } else {
                self.head = null<Node<T>>()
            }

            // Free the node memory
            free(tail_ptr as *mut u8)

            self.len -= 1
            Some(value)
        }
    }

    procedure iter(self: LinkedList<T>): LinkedListIter<T> {
        LinkedListIter { current: self.head }
    }
}

record LinkedListIter<T>: Iterator<T> {
    current: *mut Node<T>

    procedure next(self: mut LinkedListIter<T>): Option<T>
        uses unsafe.ptr
    {
        if self.current == null<Node<T>>() {
            return None
        }

        unsafe {
            let value = (*self.current).value
            self.current = (*self.current).next
            Some(value)
        }
    }
}
```

### 9.8.4 Memory Pool Allocator Pattern (Raw Pointers)

**Bump allocator for temporary allocations:**
```cantrip
record MemoryPool {
    base: *mut u8
    size: usize
    used: usize

    procedure new(size: usize): Result<own MemoryPool, Error>
        uses alloc.heap, unsafe.ptr
    {
        unsafe {
            let base = malloc(size)
            if base == null<u8>() {
                return Err(Error::AllocationFailed)
            }
            Ok(own MemoryPool { base, size, used: 0 })
        }
    }

    procedure alloc<T>(self: mut MemoryPool): Option<*mut T>
        must size_of::<T>() > 0
        must align_of::<T>() > 0
        uses unsafe.ptr
    {
        unsafe {
            let size = size_of::<T>()
            let align = align_of::<T>()

            // Align to T's alignment requirement
            let aligned_used = (self.used + align - 1) & !(align - 1)
            let new_used = aligned_used + size

            if new_used > self.size {
                return None  // Out of space
            }

            let ptr = self.base.add(aligned_used).cast<T>()
            self.used = new_used
            Some(ptr)
        }
    }

    procedure reset(self: mut MemoryPool) {
        // Reset used counter (doesn't free individual allocations)
        self.used = 0
    }

    procedure destroy(self: own MemoryPool)
        uses unsafe.ptr
    {
        unsafe {
            free(self.base)
        }
    }
}

// Usage example
function process_batch(items: [Item])
    uses alloc.heap, unsafe.ptr
{
    let pool = MemoryPool.new(1024 * 1024)?  // 1 MB pool

    region batch {
        for item in items {
            // Allocate from pool
            let temp_data: *mut ProcessedData = pool.alloc()?

            unsafe {
                // Initialize and use temp_data
                *temp_data = process(item)
                // ... do work ...
            }
        }
        // All pool allocations freed at once
        pool.reset()
    }

    pool.destroy()
}
```

---

**Previous**: [§8. Modals](08_Modals.md) | **Next**: [Part III: Functions](../03_Functions/00_Introduction.md)
