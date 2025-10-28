# Chapter 2: Type System - §2.4. Arrays and Slices

**Section**: §2.4 | **Part**: Type System (Part II)
**Previous**: [Sum Types](03_SumTypes.md) | **Next**: [Pointers](../PART_C_References/05_Pointers.md)

---

**Definition 6.1 (Arrays and Slices):** Arrays are fixed-size, contiguous sequences of homogeneous elements: `[T; n] = { (v₁, ..., vₙ) | ∀i ∈ [0, n). vᵢ : T }`. Slices are dynamically-sized views into contiguous data, implemented as dense pointers: `[T] = { (ptr, len) | ptr : *T, len : usize }`.

## 2.4. Arrays and Slices

### 2.4.1 Overview

**Key innovation/purpose:** Arrays and slices provide memory-efficient, predictable data layout for homogeneous collections. Arrays are stack-allocated with compile-time known size, while slices enable dynamic views without heap allocation.

**When to use arrays:**

- Fixed-size collections with compile-time known length
- Stack-allocated buffers
- Embedded systems with no dynamic allocation
- FFI with C arrays
- Performance-critical code where size is known

**When to use slices:**

- Function parameters that accept any array size
- Dynamic views into existing data
- Substring/subarray operations without copying
- Iterating over portions of arrays

**When NOT to use arrays/slices:**

- Heterogeneous data → use tuples (§7) or records (§8)
- Growable collections → use Vec<T> (standard library)
- Discriminated unions → use enums (§9)

**Relationship to other features:**

- **Memory layout**: Contiguous, predictable layout essential for FFI and performance
- **Type inference**: Array and slice types can be inferred from literals and context
- **Pattern matching**: Arrays support destructuring patterns
- **Generics**: `Vec<T>` and other collections internally use slices
- **Bounds checking**: Compiler inserts runtime checks for dynamic indices

### 2.4.2 Syntax

#### 2.4.2.1 Concrete Syntax

**Array Syntax:**

```ebnf
ArrayType    ::= "[" Type ";" IntLiteral "]"
ArrayLiteral ::= "[" Expr ("," Expr)* "]"
               | "[" Expr ";" IntLiteral "]"    // Repeat syntax
ArrayIndex   ::= Expr "[" Expr "]"
```

**Slice Syntax:**

```ebnf
SliceType  ::= "[" Type "]"
SliceRange ::= Expr "[" Range "]"
Range      ::= Expr? ".." Expr?        // start..end (exclusive)
             | Expr? "..=" Expr?       // start..=end (inclusive)
```

**Range semantics:**

```
arr[a..b]   = { arr[i] | a ≤ i < b }
arr[a..=b]  = { arr[i] | a ≤ i ≤ b }
arr[..b]    = arr[0..b]
arr[a..]    = arr[a..len(arr)]
arr[..]     = arr[0..len(arr)]
```

#### 2.4.2.2 Abstract Syntax

**Arrays:**

```
τ ::= [τ; n]         (array type)
e ::= [e₁, ..., eₙ]  (array literal)
    | [e; n]         (repeat literal)
    | e[e']          (array indexing)
```

**Slices:**

```
τ ::= [τ]            (slice type)
e ::= e[e₁..e₂]      (slice range)
    | e[e₁..=e₂]     (inclusive range)
    | e[..e₂]        (from start)
    | e[e₁..]        (to end)
    | e[..]          (full slice)
```

### 2.4.3 Static Semantics

#### 2.4.3.1 Well-Formedness Rules

**Array well-formedness:**

```
[WF-Array]
Γ ⊢ T : Type    n > 0
────────────────────────
Γ ⊢ [T; n] : Type

[WF-Array-Lit]
Γ ⊢ e₁ : T    ...    Γ ⊢ eₙ : T
──────────────────────────────────
Γ ⊢ [e₁, ..., eₙ] : [T; n]

[WF-Array-Repeat]
Γ ⊢ e : T
──────────────────────
Γ ⊢ [e; n] : [T; n]
```

**Slice well-formedness:**

```
[WF-Slice]
Γ ⊢ T : Type
────────────────
Γ ⊢ [T] : Type

[WF-Slice-Range]
Γ ⊢ arr : [T; n]    Γ ⊢ start : usize    Γ ⊢ end : usize
start ≤ end ≤ n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..end] : [T]

[WF-Slice-Range-Inclusive]
Γ ⊢ arr : [T; n]    Γ ⊢ start : usize    Γ ⊢ end : usize
start ≤ end < n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..=end] : [T]
```

#### 2.4.3.2 Type Rules

**Array type rules:**

```
[T-Array-Index]
Γ ⊢ arr : [T; n]
Γ ⊢ i : usize
i < n
────────────────────────────────────────────
Γ ⊢ arr[i] : T

[T-Array-Lit]
Γ ⊢ e₁ : T    ...    Γ ⊢ eₙ : T
──────────────────────────────────
Γ ⊢ [e₁, ..., eₙ] : [T; n]

[T-Array-Repeat]
Γ ⊢ e : T
──────────────────────
Γ ⊢ [e; n] : [T; n]
```

**Slice type rules:**

```
[T-Slice]
Γ ⊢ arr : [T; n]
Γ ⊢ start : usize
Γ ⊢ end : usize
start ≤ end ≤ n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..end] : [T]

[T-Slice-Inclusive]
Γ ⊢ arr : [T; n]
Γ ⊢ start : usize
Γ ⊢ end : usize
start ≤ end < n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..=end] : [T]

[T-Slice-Index]
Γ ⊢ slice : [T]
Γ ⊢ i : usize
────────────────────────────────────────────
Γ ⊢ slice[i] : T    (checked at runtime: i < len(slice))

[T-Slice-Full]
Γ ⊢ arr : [T; n]
────────────────────────────────────────────
Γ ⊢ arr[..] : [T]
```

**Array type inference:**

```
[Infer-Array-Homogeneous]
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
T = unify(T₁, ..., Tₙ)
──────────────────────────────────
Γ ⊢ [e₁, ..., eₙ] : [T; n]
```

**Example:**

```cantrip
let arr = [1, 2, 3]        // Inferred: [i32; 3]
let mixed = [1, 2u64, 3]   // ERROR E2301: type mismatch in array literal: expected i32, found u64
```

**Contextual type inference:**

```cantrip
let arr: [u8; 3] = [1, 2, 3]    // Literals inferred as u8 from context
function process(data: [f32; 4]) { ... }
process([1.0, 2.0, 3.0, 4.0])   // Literals inferred as f32
```

**Bounds checking:**

_Static bounds checking (when possible):_

```
[T-Array-Index-Static]
Γ ⊢ arr : [T; n]
i is a compile-time constant
0 ≤ i < n
────────────────────────────────────────────
Γ ⊢ arr[i] : T    (no runtime check needed)
```

**Runtime bounds checking:**

```
[T-Array-Index-Dynamic]
Γ ⊢ arr : [T; n]
Γ ⊢ i : usize
i is not a compile-time constant
────────────────────────────────────────────
Γ ⊢ arr[i] : T    (runtime check: panic if i ≥ n)
```

**Example:**

```cantrip
let arr = [1, 2, 3, 4, 5]
let x = arr[2]           // Static: guaranteed in bounds
let i: usize = get_index()
let y = arr[i]           // Dynamic: runtime check inserted
```

#### 2.4.3.3 Type Properties

**Theorem 6.1 (Array Layout and Size):**

Arrays have a predictable, contiguous memory layout:

```
size([T; n]) = n × size(T)
align([T; n]) = align(T)
layout = contiguous memory, no padding between elements
offset(element i) = i × size(T)
```

**Theorem 6.2 (Slice Representation):**

Slices are dense pointers with fixed size independent of the viewed data:

```
size([T]) = size(*T) + size(usize) = 2 × word_size
align([T]) = word_size
layout = { ptr: *T, len: usize }  (dense pointer)
```

The slice itself is always 16 bytes on 64-bit systems (8 bytes pointer + 8 bytes length), regardless of the data it views.

**Theorem 6.3 (Copy Capability):**

```
[T; n] implements Copy ⟺ T implements Copy
[T] always implements Copy
```

**Semantics:**

- Arrays pass by permission by default (like all types)
- `.copy()` method performs element-wise bitwise copy for Copy-capable arrays
- Slices always implement Copy (copying the dense pointer descriptor, not underlying data)
- Slice `.copy()` duplicates the (ptr, len) pair, creating another view of same data

**Example:**

```cantrip
// Array parameter - pass by permission
procedure sum(arr: [i32; 5]): i32 {
    let mut total = 0
    for x in arr {  // Iterates through permission to array
        total += x
    }
    total
}

let numbers = [1, 2, 3, 4, 5]
let result = sum(numbers)  // numbers passed by permission
// numbers still usable here

// Explicit copy when needed
let copy_of_numbers = numbers.copy()  // Element-wise copy
```

### 2.4.4 Dynamic Semantics

#### 2.4.4.1 Evaluation Rules

**Array literal evaluation:**

```
[E-Array]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
─────────────────────────────────────────────────────
⟨[e₁, ..., eₙ], σ⟩ ⇓ ⟨[v₁, ..., vₙ], σₙ⟩

[E-Array-Repeat]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────────
⟨[e; n], σ⟩ ⇓ ⟨[v, v, ..., v], σ'⟩    (n copies of v)
```

**Array indexing evaluation:**

```
[E-Array-Index]
⟨arr, σ⟩ ⇓ ⟨[v₀, ..., vₙ₋₁], σ'⟩
⟨i, σ'⟩ ⇓ ⟨k, σ''⟩
k < n
─────────────────────────────────────
⟨arr[i], σ⟩ ⇓ ⟨vₖ, σ''⟩

[E-Array-Index-OOB]
⟨arr, σ⟩ ⇓ ⟨[v₀, ..., vₙ₋₁], σ'⟩
⟨i, σ'⟩ ⇓ ⟨k, σ''⟩
k ≥ n
─────────────────────────────────────
⟨arr[i], σ⟩ ⇓ panic("index out of bounds: {k} >= {n}")
```

**Slice creation evaluation:**

```
[E-Slice]
⟨arr, σ⟩ ⇓ ⟨[v₀, ..., vₙ₋₁], σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b ≤ n
─────────────────────────────────────
⟨arr[start..end], σ⟩ ⇓ ⟨slice(&arr[a], b - a), σ'''⟩

[E-Slice-Inclusive]
⟨arr, σ⟩ ⇓ ⟨[v₀, ..., vₙ₋₁], σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b < n
─────────────────────────────────────
⟨arr[start..=end], σ⟩ ⇓ ⟨slice(&arr[a], b - a + 1), σ'''⟩

[E-Slice-OOB]
⟨arr, σ⟩ ⇓ ⟨[v₀, ..., vₙ₋₁], σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a > b ∨ b > n
─────────────────────────────────────
⟨arr[start..end], σ⟩ ⇓ panic("slice index out of bounds")
```

**Slice indexing evaluation:**

```
[E-Slice-Index]
⟨slice, σ⟩ ⇓ ⟨(ptr, len), σ'⟩
⟨i, σ'⟩ ⇓ ⟨k, σ''⟩
k < len
─────────────────────────────────────
⟨slice[i], σ⟩ ⇓ ⟨*(ptr + k), σ''⟩
```

#### 2.4.4.2 Memory Representation

**Array layout:**

```
[T; n] memory layout:
┌────┬────┬────┬────┬────┐
│ T₀ │ T₁ │ T₂ │... │Tₙ₋₁│
└────┴────┴────┴────┴────┘
↑ aligned to align(T)
```

**Properties:**

- Contiguous memory
- No padding between elements
- Element i at offset: `i × size(T)`
- Total size: `n × size(T)`
- Alignment: `align(T)`

**Example:**

```cantrip
// [i32; 4] layout (assuming size(i32) = 4, align(i32) = 4)
// Offset:  0    4    8    12
//         ┌────┬────┬────┬────┐
//         │ v₀ │ v₁ │ v₂ │ v₃ │
//         └────┴────┴────┴────┘
// Total: 16 bytes, align: 4
```

**Slice layout:**

```
[T] memory layout (dense pointer):
┌─────────────┬─────────────┐
│ ptr: *T     │ len: usize  │
└─────────────┴─────────────┘
```

**Properties:**

- Size: `2 × word_size` (typically 16 bytes on 64-bit)
- Alignment: `word_size`
- Does NOT contain the actual data
- Points to data owned by array or other container

**Example:**

```cantrip
// On 64-bit system:
// [i32] layout:
//         ┌────────────────┬────────────────┐
//         │ ptr: 8 bytes   │ len: 8 bytes   │
//         └────────────────┴────────────────┘
// Total: 16 bytes, align: 8
```

#### 2.4.4.3 Operational Behavior

**Bounds checking behavior:**

Arrays and slices provide memory-safe access through compile-time and runtime bounds checking:

- **Static indices** (compile-time constants): When the compiler can prove an index is in bounds, no runtime overhead is incurred
- **Dynamic indices**: Runtime checks inserted, panic on out-of-bounds access with error message indicating the index and length
- **Performance**: Modern CPUs can often predict bounds check branches, minimizing performance impact

**Parameter Passing:**

- **All arrays** pass by permission (reference-like) regardless of Copy capability
- **Copy-capable arrays** can be explicitly copied with `.copy()` method
- **Non-Copy arrays** require explicit `move` for ownership transfer
- **Slices** always Copy-capable (dense pointer), pass by permission by default

**Example:**

```cantrip
let arr: [i32; 3] = [1, 2, 3]
procedure process(a: [i32; 3]) { ... }
process(arr)  // Pass by permission, arr still usable

let explicit_copy = arr.copy()  // Explicit copy if needed
```

**Performance characteristics:**

```
Array access:        O(1) with potential bounds check (often elided)
Slice creation:      O(1) (pointer arithmetic only)
Array construction:  O(n) (initialize all elements)
Sequential access:   Cache-friendly due to contiguous layout
```

**Example - bounds check elimination:**

```cantrip
// Compiler can prove i < 5 for all iterations
for i in 0..5 {
    arr[i] = i  // Bounds check eliminated
}
```

### 2.4.5 Additional Properties

#### 2.4.5.1 Parameter Passing and Copying

**All arrays and slices pass by permission:**

```cantrip
// Small array - passes by permission
let arr1: [i32; 3] = [1, 2, 3]
procedure process(a: [i32; 3]) { ... }
process(arr1)  // Permission, not copy
// arr1 still usable

// Explicit copy when needed
let arr2 = arr1.copy()  // Element-wise copy

// Large array - passes by permission (efficient!)
let big: [i32; 1000] = initialize_big_array()
process_big(big)  // Permission only, zero cost
// No expensive copy unless explicitly requested

// Non-Copy array - passes by permission or moves
let strings: [String; 3] = [String.new("a"), String.new("b"), String.new("c")]
process_strings(strings)  // Permission
consume_strings(move strings)  // Explicit move
// strings no longer usable
```

**Slices - always Copy-capable:**

```cantrip
let arr = [1, 2, 3, 4, 5]
let slice: [i32] = arr[..]

// Slices pass by permission like everything else
procedure process(s: [i32]) { ... }
process(slice)  // Permission to dense pointer

// Explicit copy duplicates dense pointer (cheap)
let slice2 = slice.copy()  // Copies (ptr, len), NOT data
// Both slice and slice2 view same data

// Data copying requires explicit element iteration
let mut data_copy = Vec.new()
for x in slice {
    data_copy.push(x.copy())
}
```

#### 2.4.5.2 Subtyping

Arrays and slices are **invariant** in their type parameter:

```
[T; n] is NOT a subtype of [U; n] even if T <: U
[T] is NOT a subtype of [U] even if T <: U
```

**Rationale:** Allowing covariance would violate type safety in mutable contexts.

**Example:**

```cantrip
// This would be unsound if allowed:
let arr: [own String; 3] = [...]
let arr2: [mut String; 3] = arr  // ERROR: not a subtype
```

#### 2.4.5.3 Array-to-Slice Coercion

Arrays can be implicitly coerced to slices in certain contexts:

```
[T-Array-Coercion]
Γ ⊢ arr : [T; n]
────────────────────────────────────────────
Γ ⊢ arr : [T]    (implicit coercion)
```

**Example:**

```cantrip
function process(data: [i32]) {
    // ...
}

let arr: [i32; 5] = [1, 2, 3, 4, 5]
process(arr)  // OK: [i32; 5] coerces to [i32]
```

### 2.4.5 Mutable Slices

**Definition 6.2 (Mutable Slice):** A mutable slice `[mut T]` is a dynamically-sized view into contiguous mutable data, allowing in-place modification of elements. Like immutable slices, mutable slices are dense pointers `(ptr: *mut T, len: usize)`, but require `mut` permission to create and use.

#### 2.4.5.1 Syntax

**Grammar:**

```ebnf
MutSliceType ::= "[" "mut" Type "]"
MutSliceRange ::= MutExpr "[" Range "]"
MutSliceIndex ::= MutExpr "[" Expr "]"
```

**Examples:**

```cantrip
// Mutable slice types
let mut arr = [1, 2, 3, 4, 5]
let slice: [mut i32] = arr[1..4]  // Mutable view

// Mutable indexing
slice[0] = 10  // Modify through slice

// Mutable slice from mutable array
var numbers = [0; 100]
let view: [mut i32] = numbers[10..20]
```

#### 2.4.5.2 Type Rules

**[T-MutSlice] Mutable Slice Creation:**

```
[T-MutSlice]
Γ ⊢ arr : mut [T; n]    (mutable array)
Γ ⊢ start : usize
Γ ⊢ end : usize
start ≤ end ≤ n
────────────────────────────────────────────────────────────
Γ ⊢ arr[start..end] : [mut T]
```

**Explanation:** Only mutable arrays can produce mutable slices. The `mut` permission is required.

```cantrip
let arr = [1, 2, 3, 4, 5]           // Immutable array
// let s: [mut i32] = arr[1..3]     // ✗ ERROR: cannot create mutable slice from immutable array

var mut_arr = [1, 2, 3, 4, 5]       // Mutable array
let s: [mut i32] = mut_arr[1..3]   // ✓ Mutable slice
```

**[T-MutSliceIndex] Mutable Slice Indexing:**

```
[T-MutSliceIndex]
Γ ⊢ slice : [mut T]
Γ ⊢ i : usize
────────────────────────────────────────────
Γ ⊢ slice[i] : mut T    (checked at runtime: i < len(slice))
```

**Explanation:** Indexing a mutable slice yields a mutable reference to the element.

```cantrip
var arr = [10, 20, 30]
let slice: [mut i32] = arr[..]

// Read element
let value = slice[1]  // 20

// Modify element
slice[1] = 25         // ✓ Mutable access
assert(slice[1] == 25)
```

**[T-MutSliceAssign] Mutable Slice Assignment:**

```
[T-MutSliceAssign]
Γ ⊢ slice : [mut T]
Γ ⊢ i : usize
Γ ⊢ v : T
────────────────────────────────────────────
Γ ⊢ (slice[i] = v) : ()    (modifies slice[i])
```

**Explanation:** Elements can be assigned through mutable slices.

```cantrip
var data = [1, 2, 3, 4, 5]
let slice: [mut i32] = data[1..4]

slice[0] = 100  // Modifies data[1]
slice[1] = 200  // Modifies data[2]
slice[2] = 300  // Modifies data[3]

assert(data == [1, 100, 200, 300, 5])
```

**[T-MutSliceIter] Mutable Slice Iteration:**

```
[T-MutSliceIter]
Γ ⊢ slice : [mut T]
────────────────────────────────────────────
Γ ⊢ slice.iter_mut() : Iterator<Item = mut T>
```

**Explanation:** Mutable slices provide mutable iterators for in-place modification.

```cantrip
var numbers = [1, 2, 3, 4, 5]
let slice: [mut i32] = numbers[..]

// Mutable iteration
for x in slice.iter_mut() {
    *x *= 2  // Double each element in place
}

assert(numbers == [2, 4, 6, 8, 10])
```

**[T-SlicePermission] Permission Requirements:**

```
[T-SlicePermission]
Γ ⊢ arr : π [T; n]
────────────────────────────────────────────
Γ ⊢ arr[start..end] : [π T]

where π ∈ {own, mut, imm}
```

**Explanation:** Slice permission matches the permission of the source array.

```cantrip
// Immutable array → immutable slice
let arr: imm [i32; 5] = [1, 2, 3, 4, 5]
let slice: [imm i32] = arr[1..3]  // ✓ Immutable slice

// Mutable array → mutable slice
var mut_arr: mut [i32; 5] = [1, 2, 3, 4, 5]
let mut_slice: [mut i32] = mut_arr[1..3]  // ✓ Mutable slice

// Cannot create mutable slice from immutable array
// let bad: [mut i32] = arr[1..3]  // ✗ ERROR: permission mismatch
```

**[T-MutSliceSplit] Mutable Slice Splitting:**

```
[T-MutSliceSplit]
Γ ⊢ slice : [mut T]
Γ ⊢ mid : usize
────────────────────────────────────────────
Γ ⊢ slice.split_at_mut(mid) : ([mut T], [mut T])
```

**Explanation:** Mutable slices can be split into non-overlapping mutable sub-slices.

```cantrip
var arr = [1, 2, 3, 4, 5, 6]
let slice: [mut i32] = arr[..]

let (left, right) = slice.split_at_mut(3)
// left: [mut i32] = [1, 2, 3]
// right: [mut i32] = [4, 5, 6]

left[0] = 10
right[0] = 40

assert(arr == [10, 2, 3, 40, 5, 6])
```

**[T-MutSliceNonOverlap] Non-Overlapping Guarantee:**

```
[T-MutSliceNonOverlap]
Γ ⊢ arr : mut [T; n]
slice₁ = arr[a₁..b₁]
slice₂ = arr[a₂..b₂]
(a₁..b₁) ∩ (a₂..b₂) = ∅    (disjoint ranges)
────────────────────────────────────────────
slice₁ and slice₂ can coexist as [mut T]
```

**Explanation:** Multiple mutable slices of the same array are allowed only if they don't overlap.

```cantrip
var arr = [1, 2, 3, 4, 5, 6]

// ✓ Non-overlapping mutable slices
let slice1: [mut i32] = arr[0..3]
let slice2: [mut i32] = arr[3..6]
slice1[0] = 10
slice2[0] = 40

// ✗ ERROR: Overlapping mutable slices not allowed
// let bad1: [mut i32] = arr[0..4]
// let bad2: [mut i32] = arr[2..6]  // Overlaps with bad1
```

**[T-MutSliceCoercion] Array to Mutable Slice Coercion:**

```
[T-MutSliceCoercion]
Γ ⊢ arr : mut [T; n]
────────────────────────────────────────────
Γ ⊢ arr : [mut T]    (coerces to mutable slice)
```

**Explanation:** Mutable arrays automatically coerce to mutable slices in function calls.

```cantrip
function double_elements(data: [mut i32]) {
    for x in data.iter_mut() {
        *x *= 2
    }
}

var numbers = [1, 2, 3, 4, 5]
double_elements(numbers)  // ✓ Coerces [i32; 5] to [mut i32]
assert(numbers == [2, 4, 6, 8, 10])
```

#### 2.4.5.3 Memory Representation

Mutable slices have the same memory representation as immutable slices:

```
[mut T] = (ptr: *mut T, len: usize)

Size: 2 × usize (16 bytes on 64-bit)
Alignment: align(usize) = 8 bytes on 64-bit
```

**Key difference:** The pointer type is `*mut T` (mutable raw pointer) instead of `*const T`.

**Memory layout:**

```
Mutable Slice [mut T]:
┌─────────────────┬─────────────────┐
│  ptr: *mut T    │  len: usize     │
│  (8 bytes)      │  (8 bytes)      │
└─────────────────┴─────────────────┘
       ↓
       Points to mutable memory region:
       [T₀, T₁, T₂, ..., Tₙ₋₁]
```

#### 2.4.5.4 Operational Behavior

**In-place modification:**

```cantrip
var data = [1, 2, 3, 4, 5]
let slice: [mut i32] = data[1..4]

// Modify slice element
slice[1] = 100

// Original array is modified
assert(data == [1, 2, 100, 4, 5])
```

**Mutable vs. Immutable slices:**

| Feature          | Immutable Slice `[T]`             | Mutable Slice `[mut T]`                      |
| ---------------- | --------------------------------- | -------------------------------------------- |
| **Pointer type** | `*const T`                        | `*mut T`                                     |
| **Permission**   | `imm` or `own`                    | `mut` required                               |
| **Modification** | Read-only                         | Can modify elements                          |
| **Aliasing**     | Multiple immutable slices allowed | Only one mutable slice or multiple immutable |
| **Copy trait**   | ✓ Implements Copy                 | ✓ Implements Copy                            |
| **Size**         | 16 bytes (64-bit)                 | 16 bytes (64-bit)                            |

**Permission rules:**

```cantrip
var arr = [1, 2, 3, 4, 5]

// ✓ Multiple immutable slices allowed
let s1: [i32] = arr[0..2]
let s2: [i32] = arr[2..5]

// ✓ One mutable slice allowed
let ms: [mut i32] = arr[..]

// ✗ Cannot have mutable and immutable slices simultaneously
// let bad_imm: [i32] = arr[0..2]      // ✗ ERROR: already borrowed mutably
// let bad_mut: [mut i32] = arr[3..5]  // ✗ ERROR: already borrowed mutably
```

#### 2.4.5.5 Type Properties

**Theorem 6.1 (Mutable Slice Safety):** Mutable slices preserve memory safety through permission checking. At most one mutable slice to overlapping memory can exist at any time.

**Proof sketch:** The type system enforces that mutable slices require `mut` permission, and the permission system guarantees exclusive access to mutable data. ∎

**Corollary 6.1:** No data races can occur through mutable slices.

**Theorem 6.2 (Non-Overlapping Split):** Split operations like `split_at_mut` produce non-overlapping slices that can be safely used simultaneously.

**Proof sketch:** The split operation divides the slice into disjoint ranges `[0..mid)` and `[mid..len)`. Since these ranges don't overlap, multiple mutable accesses are safe. ∎

---

### 2.4.6 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - arrays use primitive element types
- See §2.2 for [Product Types](02_ProductTypes.md) - arrays can contain tuples and records
- See §2.3 for [Sum Types](03_SumTypes.md) - arrays can contain enums
- See §2.7 for [Generics](../PART_D_Abstraction/07_Generics.md) - generic collections over array/slice types
- Compare with Vec<T> in standard library for dynamic arrays
- See Part V (Effects) for effect tracking with array/slice operations
- See Part III (Permissions) for permission semantics with mutable slices
- **Examples**: See [04_CollectionExamples.md](../Examples/04_CollectionExamples.md) for practical usage patterns

### 2.4.7 Notes and Warnings

**Note 2.4.1:** Array sizes must be compile-time constants. For runtime-sized collections, use Vec<T> from the standard library.

**Note 2.4.2:** Slices are views, not owned data. They cannot outlive the data they reference. The region system enforces this automatically.

**Performance Note 2.4.1:** Array indexing performs bounds checking by default. For performance-critical loops where bounds are statically known, the compiler can eliminate checks through loop analysis.

**Performance Note 2.4.2:** Slices have zero overhead compared to raw pointers with manual length tracking. The dense pointer representation (ptr + len) is the same size as separate variables.

**Warning 2.4.1:** Creating a slice from an array does NOT copy data - it creates a view. Modifying the array affects the slice and vice versa (for mutable slices).

**Implementation Note 2.4.1:** Array-to-slice coercion is a zero-cost conversion. No code is generated; the compiler simply adjusts the type.

---

**Previous**: [Sum Types](03_SumTypes.md) | **Next**: [Pointers](../PART_C_References/05_Pointers.md)
