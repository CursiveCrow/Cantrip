# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-5_Alignment-and-Layout.md
**Section**: §3.5 Alignment and Layout (Overview)
**Stable label**: [basic.align]
**Forward references**: §3.1 [basic.object], §3.2 [basic.type], Clause 7 [type], Clause 11 [memory], Clause 15 [interop]

---

### §3.5 Alignment and Layout (Overview) [basic.align]

#### §3.5.1 Overview

[1] _Alignment_ specifies the address constraints for objects; _layout_ describes how composite types arrange their constituent parts in memory. These properties determine object sizes, padding requirements, and ABI compatibility. While Clause 11 provides complete layout rules, this subclause establishes foundational concepts.

[2] Alignment is a compile-time property: for any type `T`, the alignment requirement `alignof(T)` is a power of two indicating that objects of type `T` must be placed at addresses divisible by that value.

[3] Layout determines field offsets, array element spacing, and total type size. Understanding layout is essential for unsafe code (§11.8), FFI interoperation (Clause 15), and performance-critical algorithms.

#### §3.5.2 Alignment Requirements [basic.align.alignment]

##### §3.5.2.1 Definition

[4] For every type `T`, the alignment requirement `alignof(T)` is a power of two satisfying:

$$
\text{alignof}(T) = 2^k \text{ where } k \in \mathbb{N} \cup \{0\}
$$

[5] An object of type `T` shall be placed at an address `addr` where:

$$
\text{addr} \equiv 0 \pmod{\text{alignof}(T)}
$$

[6] Violating alignment requirements produces undefined behavior (UB-ID: B.2.10 "misaligned object access") on platforms with strict alignment enforcement. On platforms with relaxed alignment, misaligned access may succeed with performance penalties.

##### §3.5.2.2 Alignment Rules by Type Family

[7] Table 3.5.1 specifies alignment for each type family. Detailed rules appear in Clause 7 and Clause 11.

**Table 3.5.1 — Alignment by type family**

| Type family        | Alignment rule              | Notes                                 |
| ------------------ | --------------------------- | ------------------------------------- |
| Primitive integers | Equal to size               | `alignof(i32) = 4`, `alignof(u8) = 1` |
| Floating-point     | Equal to size               | `alignof(f64) = 8`                    |
| `bool`, `char`     | Implementation-defined, ≥ 1 | Typically 1 byte                      |
| `()`               | 1                           | Unit type has minimal alignment       |
| Tuples             | max(alignof(Tᵢ))            | Maximum of element alignments         |
| Records            | max(alignof(fieldᵢ))        | Maximum of field alignments           |
| Arrays `[T; n]`    | alignof(T)                  | Same as element type                  |
| Pointers           | Platform pointer alignment  | Typically 4 or 8 bytes                |
| Enums              | max(alignof(variantᵢ))      | Maximum of variant payload alignments |

[8] Implementations shall document the alignment for each primitive type and pointer size. These values are implementation-defined but shall remain constant for a given target platform.

##### §3.5.2.3 Alignment Attributes

[9] Types may specify explicit alignment using the `[[repr(align(N))]]` attribute where `N` is a power of two. Explicit alignment must be at least as large as the natural alignment:

$$
N \geq \text{alignof}_{\text{natural}}(T)
$$

[10] Violating this constraint produces diagnostic E03-201. Over-alignment (specifying `N >` natural alignment) is permitted and may improve performance or satisfy ABI requirements.

##### §3.5.2.4 Examples

**Example 3.5.2.1 (Alignment requirements):**

```cursive
// Natural alignment
let byte: u8 = 0             // alignof(u8) = 1, can be at any address
let word: i32 = 0            // alignof(i32) = 4, must be at address divisible by 4
let wide: i64 = 0            // alignof(i64) = 8, must be at address divisible by 8

// Tuple alignment is maximum of elements
let tuple: (u8, i32) = (1, 2)  // alignof((u8, i32)) = 4
```

**Example 3.5.2.2 (Explicit alignment attribute):**

```cursive
[[repr(align(16))]]
record CacheLine {
    data: [u8; 64],
}
// Objects of type CacheLine are guaranteed 16-byte alignment
```

#### §3.5.3 Layout and Padding [basic.align.layout]

##### §3.5.3.1 Field Ordering and Offsets

[11] For composite types, _layout_ specifies the offset of each field or element within the aggregate. Layout determines the object's total size and the positions where subobjects reside.

[12] **Records**: Fields are laid out in declaration order with padding inserted between fields to satisfy alignment requirements. For record `R { f₁: T₁, …, fₙ: Tₙ }`, each field `fᵢ` has offset `offsetof(R, fᵢ)` such that:

$$
\text{offsetof}(R, f_i) \equiv 0 \pmod{\text{alignof}(T_i)}
$$

[13] **Tuples**: Elements are laid out in index order following the same padding rules as records.

[14] **Arrays**: Elements are laid out contiguously with no padding between elements. Element `i` is at offset `i × sizeof(T)`.

##### §3.5.3.2 Size Calculation

[15] The size of a type includes all fields and necessary padding:

$$
\text{sizeof}(T) = \text{last\_field\_end} + \text{tail\_padding}
$$

where tail padding ensures that arrays of `T` maintain alignment for each element.

[16] Tail padding is computed as:

$$
\text{tail\_padding} = (-\text{last\_field\_end}) \bmod \text{alignof}(T)
$$

This ensures `sizeof(T)` is a multiple of `alignof(T)`.

##### §3.5.3.3 Padding

[17] _Padding_ consists of unused bytes inserted between fields or at the end of types to satisfy alignment constraints. Padding bytes have indeterminate values; reading them produces unspecified results.

[18] Implementations may use padding for internal purposes (e.g., storing metadata, detecting buffer overflows in debug builds), but such use shall not affect the observable behavior of conforming programs.

##### §3.5.3.4 Layout Attributes

[19] Types may control layout through representation attributes:

- `[[repr(packed)]]`: Removes padding, setting alignment to 1 for all fields
- `[[repr(transparent)]]`: Adopts the layout of a single non-zero-sized field
- `[[repr(align(N))]]`: Forces minimum alignment
- `[[repr(C)]]`: Uses C-compatible layout for FFI (Clause 15)

[20] Layout attributes affect only the memory representation; they do not change type identity or semantics beyond size/alignment calculations.

##### §3.5.3.5 Examples

**Example 3.5.3.1 (Field layout with padding):**

```cursive
record Example {
    a: u8,      // Offset 0, size 1
    // 3 bytes padding here to align `b`
    b: i32,     // Offset 4, size 4
    c: u8,      // Offset 8, size 1
    // 3 bytes tail padding to make size multiple of 4
}
// sizeof(Example) = 12, alignof(Example) = 4
```

**Example 3.5.3.2 (Packed layout):**

```cursive
[[repr(packed)]]
record Packed {
    a: u8,      // Offset 0, size 1
    b: i32,     // Offset 1, size 4 (no padding!)
    c: u8,      // Offset 5, size 1
}
// sizeof(Packed) = 6, alignof(Packed) = 1
// WARNING: Accessing `b` may be slower due to misalignment
```

**Example 3.5.3.3 (Array layout):**

```cursive
let array: [i32; 4] = [10, 20, 30, 40]
// Element 0 at offset 0
// Element 1 at offset 4
// Element 2 at offset 8
// Element 3 at offset 12
// sizeof([i32; 4]) = 16, alignof([i32; 4]) = 4
```

#### §3.5.4 Platform Dependencies [basic.align.platform]

[21] Several layout properties are platform-dependent:

- **Pointer size and alignment**: Determined by target architecture (32-bit, 64-bit, etc.)
- **Endianness**: Byte order for multi-byte values (little-endian, big-endian)
- **Primitive alignment**: Some platforms over-align primitives for performance
- **Struct packing**: Some ABIs impose stricter or looser packing rules

[22] Implementations shall document all platform-dependent layout choices and ensure they remain consistent within a target platform. Cross-platform programs should use `[[repr(C)]]` for types exposed through FFI or binary serialization.

#### §3.5.5 Integration with ABI [basic.align.abi]

[23] Layout determines ABI compatibility. Types used in FFI contexts (Clause 15) shall:

- Use `[[repr(C)]]` or equivalent platform-specific attributes
- Match the layout expected by foreign code
- Document any platform-specific padding or alignment assumptions

[24] Clause 15 specifies ABI requirements for interoperation. Types without explicit layout attributes use the Cursive native layout, which is implementation-defined and optimized for performance rather than external compatibility.

#### §3.5.6 Conformance Requirements [basic.align.requirements]

[25] Implementations shall:

1. Enforce alignment requirements and produce undefined behavior for misaligned accesses
2. Compute field offsets, padding, and sizes according to the rules in §3.5.3
3. Support layout attributes (`packed`, `transparent`, `align`, `C`) with the semantics specified in Clause 11
4. Document platform-dependent alignment and size properties
5. Ensure ABI-compatible layout for types marked with `[[repr(C)]]`
6. Emit diagnostic E03-201 when explicit alignment is smaller than natural alignment
7. Provide introspection queries `sizeof(T)` and `alignof(T)` returning compile-time constants

[26] Layout computation shall be deterministic: identical type declarations on the same target platform shall produce identical layouts across compilations.

---

**Previous**: §3.4 Storage Duration | **Next**: §3.6 Name Binding Categories
