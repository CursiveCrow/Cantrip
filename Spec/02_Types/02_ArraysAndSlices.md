# Part II: Type System - §6. Arrays and Slices

**Section**: §6 | **Part**: Type System (Part II)
**Previous**: [Primitive Types](01_PrimitiveTypes.md) | **Next**: [Tuples](03_Tuples.md)

---

**Definition 6.1 (Arrays and Slices):** Arrays are fixed-size, contiguous sequences of homogeneous elements: `[T; n] = { (v₁, ..., vₙ) | ∀i ∈ [0, n). vᵢ : T }`. Slices are dynamically-sized views into contiguous data, implemented as fat pointers: `[T] = { (ptr, len) | ptr : *T, len : usize }`.

## 6. Arrays and Slices

### 6.1 Overview

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

### 6.2 Syntax

#### 6.2.1 Concrete Syntax

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

#### 6.2.2 Abstract Syntax

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

#### 6.2.3 Basic Examples

**Array examples:**
```cantrip
// Array types
let numbers: [i32; 5] = [1, 2, 3, 4, 5];
let zeros: [i32; 100] = [0; 100];  // Repeat: [value; count]
let matrix: [[f64; 3]; 3];          // Nested arrays

// Array indexing
let first = numbers[0];             // 1
let third = numbers[2];             // 3
```

**Slice examples:**
```cantrip
let arr = [1, 2, 3, 4, 5];

// Exclusive range (..): excludes end index
let slice: [i32] = arr[1..4];   // [2, 3, 4] (indices 1, 2, 3)

// Inclusive range (..=): includes end index
let inclusive: [i32] = arr[1..=4];  // [2, 3, 4, 5] (indices 1, 2, 3, 4)

// Partial ranges
let all: [i32] = arr[..];       // [1, 2, 3, 4, 5]
let from: [i32] = arr[2..];     // [3, 4, 5]
let to: [i32] = arr[..3];       // [1, 2, 3]
let to_incl: [i32] = arr[..=3]; // [1, 2, 3, 4]
```

### 6.3 Static Semantics

#### 6.3.1 Well-Formedness Rules

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

#### 6.3.2 Type Rules

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
let arr = [1, 2, 3];        // Inferred: [i32; 3]
let mixed = [1, 2u64, 3];   // ERROR: cannot unify i32 and u64
```

**Contextual type inference:**
```cantrip
let arr: [u8; 3] = [1, 2, 3];    // Literals inferred as u8 from context
function process(data: [f32; 4]) { ... }
process([1.0, 2.0, 3.0, 4.0]);   // Literals inferred as f32
```

**Bounds checking:**

*Static bounds checking (when possible):*
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
let arr = [1, 2, 3, 4, 5];
let x = arr[2];           // Static: guaranteed in bounds
let i: usize = get_index();
let y = arr[i];           // Dynamic: runtime check inserted
```

#### 6.3.3 Type Properties

**Theorem 6.1 (Array Layout and Size):**

Arrays have a predictable, contiguous memory layout:

```
size([T; n]) = n × size(T)
align([T; n]) = align(T)
layout = contiguous memory, no padding between elements
offset(element i) = i × size(T)
```

**Theorem 6.2 (Slice Representation):**

Slices are fat pointers with fixed size independent of the viewed data:

```
size([T]) = size(*T) + size(usize) = 2 × word_size
align([T]) = word_size
layout = { ptr: *T, len: usize }  (fat pointer)
```

The slice itself is always 16 bytes on 64-bit systems (8 bytes pointer + 8 bytes length), regardless of the data it views.

**Theorem 6.3 (Copy Semantics):**

```
[T; n] is Copy ⟺ T is Copy
[T] is always Copy (copies fat pointer, not data)
```

### 6.4 Dynamic Semantics

#### 6.4.1 Evaluation Rules

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

#### 6.4.2 Memory Representation

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
[T] memory layout (fat pointer):
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

#### 6.4.3 Operational Behavior

**Bounds checking behavior:**

Arrays and slices provide memory-safe access through compile-time and runtime bounds checking:

- **Static indices** (compile-time constants): When the compiler can prove an index is in bounds, no runtime overhead is incurred
- **Dynamic indices**: Runtime checks inserted, panic on out-of-bounds access with error message indicating the index and length
- **Performance**: Modern CPUs can often predict bounds check branches, minimizing performance impact

**Copy vs Move semantics:**

- **Arrays**: Copy if element type `T` is `Copy`; otherwise move semantics apply. Large arrays may have significant copy overhead.
- **Slices**: Always `Copy` (fat pointer copied, not underlying data). This makes slices cheap to pass to functions.

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
    arr[i] = i;  // Bounds check eliminated
}
```

### 6.5 Additional Properties

#### 6.5.1 Copy vs Move Semantics

**Arrays:**
- `[T; n]` is Copy if T is Copy
- Large arrays are expensive to copy (consider using slices)
- Arrays of non-Copy types are moved

```cantrip
let arr1: [i32; 3] = [1, 2, 3];
let arr2 = arr1;    // Copy (i32 is Copy, small array)

let big: [String; 1000] = ...;
let moved = big;    // Move (String not Copy)
// big is now unusable
```

**Slices:**
- `[T]` is always Copy (copies the fat pointer, not the data)
- Underlying data is not copied
- Cheap to pass slices to functions

```cantrip
let arr = [1, 2, 3, 4, 5];
let slice: [i32] = arr[..];
let slice2 = slice; // Copy (fat pointer copied, not data)
// Both slice and slice2 point to same data
```

#### 6.5.2 Subtyping

Arrays and slices are **invariant** in their type parameter:

```
[T; n] is NOT a subtype of [U; n] even if T <: U
[T] is NOT a subtype of [U] even if T <: U
```

**Rationale:** Allowing covariance would violate type safety in mutable contexts.

**Example:**
```cantrip
// This would be unsound if allowed:
let arr: [own String; 3] = [...];
let arr2: [mut String; 3] = arr;  // ERROR: not a subtype
```

#### 6.5.3 Array-to-Slice Coercion

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

let arr: [i32; 5] = [1, 2, 3, 4, 5];
process(arr);  // OK: [i32; 5] coerces to [i32]
```

### 6.6 Examples and Patterns

#### 6.6.1 Array Patterns

**Stack-allocated buffer:**
```cantrip
function process_batch(items: [Item; 32]) {
    // Fixed-size processing, no heap allocation
    for item in items {
        process(item);
    }
}
```

**Compile-time known sizes:**
```cantrip
const BUFFER_SIZE: usize = 4096;
let buffer: [u8; BUFFER_SIZE] = [0; BUFFER_SIZE];
```

**Multidimensional arrays:**
```cantrip
// 3x3 matrix
let matrix: [[f64; 3]; 3] = [
    [1.0, 0.0, 0.0],
    [0.0, 1.0, 0.0],
    [0.0, 0.0, 1.0],
];

let element = matrix[1][2];  // 0.0
```

#### 6.6.2 Slice Patterns

**Function parameters (prefer slices over arrays):**
```cantrip
// Good: accepts any size
function sum(numbers: [i32]): i32 {
    let mut total = 0;
    for n in numbers {
        total += n;
    }
    total
}

// Can call with any array size:
sum([1, 2, 3]);         // OK
sum([1, 2, 3, 4, 5]);   // OK
```

**Slicing arrays:**
```cantrip
let data = [1, 2, 3, 4, 5, 6, 7, 8];

// Exclusive ranges (..)
let first_half = data[..4];     // [1, 2, 3, 4] (indices 0..4, excludes 4)
let second_half = data[4..];    // [5, 6, 7, 8] (indices 4..8)
let middle = data[2..6];        // [3, 4, 5, 6] (indices 2..6, excludes 6)

// Inclusive ranges (..=)
let first_four = data[..=3];    // [1, 2, 3, 4] (indices 0..=3, includes 3)
let last_four = data[4..=7];    // [5, 6, 7, 8] (indices 4..=7, includes 7)
let middle_incl = data[2..=5];  // [3, 4, 5, 6] (indices 2..=5, includes 5)
```

**Window operations:**
```cantrip
function sliding_window(data: [i32], size: usize) {
    for i in 0..(data.len() - size + 1) {
        let window = data[i..(i + size)];
        process_window(window);
    }
}
```

#### 6.6.3 Performance Patterns

**Avoiding copies with slices:**
```cantrip
// Bad: copies array on each call
function bad_process(data: [i32; 1000]) {
    // ...
}

// Good: passes view without copying
function good_process(data: [i32]) {
    // ...
}
```

**Zero-cost abstraction:**
```cantrip
// Iterator over slice elements
function sum_efficient(numbers: [i32]): i32 {
    numbers.iter().sum()  // No bounds checks in tight loop
}
```

### 6.7 Mutable Slices

**Definition 6.2 (Mutable Slice):** A mutable slice `[mut T]` is a dynamically-sized view into contiguous mutable data, allowing in-place modification of elements. Like immutable slices, mutable slices are fat pointers `(ptr: *mut T, len: usize)`, but require `mut` permission to create and use.

#### 6.7.1 Syntax

**Grammar:**
```ebnf
MutSliceType ::= "[" "mut" Type "]"
MutSliceRange ::= MutExpr "[" Range "]"
MutSliceIndex ::= MutExpr "[" Expr "]"
```

**Examples:**
```cantrip
// Mutable slice types
let mut arr = [1, 2, 3, 4, 5];
let slice: [mut i32] = arr[1..4];  // Mutable view

// Mutable indexing
slice[0] = 10;  // Modify through slice

// Mutable slice from mutable array
var numbers = [0; 100];
let view: [mut i32] = numbers[10..20];
```

#### 6.7.2 Type Rules

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
let arr = [1, 2, 3, 4, 5];           // Immutable array
// let s: [mut i32] = arr[1..3];     // ✗ ERROR: cannot borrow as mutable

var mut_arr = [1, 2, 3, 4, 5];       // Mutable array
let s: [mut i32] = mut_arr[1..3];   // ✓ Mutable slice
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
var arr = [10, 20, 30];
let slice: [mut i32] = arr[..];

// Read element
let value = slice[1];  // 20

// Modify element
slice[1] = 25;         // ✓ Mutable access
assert(slice[1] == 25);
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
var data = [1, 2, 3, 4, 5];
let slice: [mut i32] = data[1..4];

slice[0] = 100;  // Modifies data[1]
slice[1] = 200;  // Modifies data[2]
slice[2] = 300;  // Modifies data[3]

assert(data == [1, 100, 200, 300, 5]);
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
var numbers = [1, 2, 3, 4, 5];
let slice: [mut i32] = numbers[..];

// Mutable iteration
for x in slice.iter_mut() {
    *x *= 2;  // Double each element in place
}

assert(numbers == [2, 4, 6, 8, 10]);
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
let arr: imm [i32; 5] = [1, 2, 3, 4, 5];
let slice: [imm i32] = arr[1..3];  // ✓ Immutable slice

// Mutable array → mutable slice
var mut_arr: mut [i32; 5] = [1, 2, 3, 4, 5];
let mut_slice: [mut i32] = mut_arr[1..3];  // ✓ Mutable slice

// Cannot create mutable slice from immutable array
// let bad: [mut i32] = arr[1..3];  // ✗ ERROR: permission mismatch
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
var arr = [1, 2, 3, 4, 5, 6];
let slice: [mut i32] = arr[..];

let (left, right) = slice.split_at_mut(3);
// left: [mut i32] = [1, 2, 3]
// right: [mut i32] = [4, 5, 6]

left[0] = 10;
right[0] = 40;

assert(arr == [10, 2, 3, 40, 5, 6]);
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
var arr = [1, 2, 3, 4, 5, 6];

// ✓ Non-overlapping mutable slices
let slice1: [mut i32] = arr[0..3];
let slice2: [mut i32] = arr[3..6];
slice1[0] = 10;
slice2[0] = 40;

// ✗ ERROR: Overlapping mutable slices not allowed
// let bad1: [mut i32] = arr[0..4];
// let bad2: [mut i32] = arr[2..6];  // Overlaps with bad1
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
        *x *= 2;
    }
}

var numbers = [1, 2, 3, 4, 5];
double_elements(numbers);  // ✓ Coerces [i32; 5] to [mut i32]
assert(numbers == [2, 4, 6, 8, 10]);
```

#### 6.7.3 Memory Representation

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

#### 6.7.4 Operational Behavior

**In-place modification:**
```cantrip
var data = [1, 2, 3, 4, 5];
let slice: [mut i32] = data[1..4];

// Modify slice element
slice[1] = 100;

// Original array is modified
assert(data == [1, 2, 100, 4, 5]);
```

**Mutable vs. Immutable slices:**

| Feature | Immutable Slice `[T]` | Mutable Slice `[mut T]` |
|---------|----------------------|------------------------|
| **Pointer type** | `*const T` | `*mut T` |
| **Permission** | `imm` or `own` | `mut` required |
| **Modification** | Read-only | Can modify elements |
| **Aliasing** | Multiple immutable slices allowed | Only one mutable slice or multiple immutable |
| **Copy trait** | ✓ Implements Copy | ✓ Implements Copy |
| **Size** | 16 bytes (64-bit) | 16 bytes (64-bit) |

**Permission rules:**
```cantrip
var arr = [1, 2, 3, 4, 5];

// ✓ Multiple immutable slices allowed
let s1: [i32] = arr[0..2];
let s2: [i32] = arr[2..5];

// ✓ One mutable slice allowed
let ms: [mut i32] = arr[..];

// ✗ Cannot have mutable and immutable slices simultaneously
// let bad_imm: [i32] = arr[0..2];      // ✗ ERROR: already borrowed mutably
// let bad_mut: [mut i32] = arr[3..5];  // ✗ ERROR: already borrowed mutably
```

#### 6.7.5 Examples and Patterns

**Pattern 1: In-place transformation**

```cantrip
function normalize(values: [mut f64]) {
    let max = values.iter().fold(0.0, |acc, &x| if x > acc { x } else { acc });
    if max > 0.0 {
        for v in values.iter_mut() {
            *v /= max;
        }
    }
}

var data = [10.0, 20.0, 30.0, 40.0];
normalize(data);
assert(data == [0.25, 0.5, 0.75, 1.0]);
```

**Pattern 2: Partitioning**

```cantrip
function partition<T>(slice: [mut T], pred: fn(T) -> bool) -> usize
    where T: Copy
{
    let mut left = 0;
    let mut right = slice.len() - 1;

    while left < right {
        if pred(slice[left]) {
            left += 1;
        } else {
            // Swap with right
            let temp = slice[left];
            slice[left] = slice[right];
            slice[right] = temp;
            right -= 1;
        }
    }

    left  // Return partition point
}

var numbers = [1, 2, 3, 4, 5, 6, 7, 8];
let pivot = partition(numbers, |x| x % 2 == 0);
// Even numbers moved to front
```

**Pattern 3: Parallel mutable access**

```cantrip
function swap_halves(data: [mut i32]) {
    let mid = data.len() / 2;
    let (left, right) = data.split_at_mut(mid);

    // Can modify both halves independently (non-overlapping)
    for i in 0..left.len() {
        let temp = left[i];
        left[i] = right[i];
        right[i] = temp;
    }
}

var arr = [1, 2, 3, 4, 5, 6];
swap_halves(arr);
assert(arr == [4, 5, 6, 1, 2, 3]);
```

**Pattern 4: Accumulation with mutation**

```cantrip
function running_sum(values: [mut i32]) {
    if values.len() == 0 {
        return;
    }

    for i in 1..values.len() {
        values[i] += values[i - 1];
    }
}

var data = [1, 2, 3, 4, 5];
running_sum(data);
assert(data == [1, 3, 6, 10, 15]);  // Cumulative sum
```

**Pattern 5: Safe mutable windowing**

```cantrip
function smooth(data: [mut f64], window_size: usize) {
    if data.len() < window_size || window_size == 0 {
        return;
    }

    let mut temp = Vec::with_capacity(data.len());

    // Copy original values
    for &x in data.iter() {
        temp.push(x);
    }

    // Apply smoothing
    for i in 0..(data.len() - window_size + 1) {
        let mut sum = 0.0;
        for j in 0..window_size {
            sum += temp[i + j];
        }
        data[i] = sum / (window_size as f64);
    }
}

var signal = [1.0, 5.0, 2.0, 8.0, 3.0];
smooth(signal, 3);
// Applies moving average smoothing
```

#### 6.7.6 Type Properties

**Theorem 6.1 (Mutable Slice Safety):** Mutable slices preserve memory safety through permission checking. At most one mutable slice to overlapping memory can exist at any time.

**Proof sketch:** The type system enforces that mutable slices require `mut` permission, and the permission system guarantees exclusive access to mutable data. ∎

**Corollary 6.1:** No data races can occur through mutable slices.

**Theorem 6.2 (Non-Overlapping Split):** Split operations like `split_at_mut` produce non-overlapping slices that can be safely used simultaneously.

**Proof sketch:** The split operation divides the slice into disjoint ranges `[0..mid)` and `[mid..len)`. Since these ranges don't overlap, multiple mutable accesses are safe. ∎

---

**Previous**: [Primitive Types](01_PrimitiveTypes.md) | **Next**: [Tuples](03_Tuples.md)
