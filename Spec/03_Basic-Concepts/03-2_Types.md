# Cursive Language Specification

## Clause 3 — Basic Concepts

**Part**: III — Basic Concepts
**File**: 03-2_Types.md
**Section**: §3.2 Types (Overview and Classification)
**Stable label**: [basic.type]
**Forward references**: §3.1 [basic.object], §3.5 [basic.align], Clause 7 [type], Clause 10 [generic], Clause 11 [memory]

---

### §3.2 Types (Overview and Classification) [basic.type]

#### §3.2.1 Overview

[1] A _type_ classifies values and determines the operations permitted on those values. Every object has a type; every expression has a type; every binding introduces a type association. The type system (Clause 7) provides the complete specification; this subclause establishes conceptual foundations and terminology.

[2] Cursive's type system is primarily nominal (types are distinguished by declarations), with structural types for tuples, arrays, and unions. Type checking is static and occurs before code generation (§2.2), ensuring that well-typed programs satisfy memory safety and contract obligations at compile time.

#### §3.2.2 Type Classification [basic.type.classification]

##### §3.2.2.1 Overview

[3] Types are organized into families based on their structure and capabilities. Table 3.2.1 provides the taxonomy; each family is specified in detail in Clause 7.

**Table 3.2.1 — Type families**

| Family          | Examples                                | Value set property                          | Specified in |
| --------------- | --------------------------------------- | ------------------------------------------- | ------------ |
| Primitive types | `i32`, `f64`, `bool`, `char`, `()`, `!` | Finite or infinite scalar values            | §7.2         |
| Composite types | Tuples, records, arrays, enums          | Structured combinations of other types      | §7.3         |
| Pointer types   | `Ptr<T>`, `Ptr<T>@State`                | Memory addresses with permissions/states    | §7.5         |
| Callable types  | `(T₁, T₂) -> R ! ε`                     | Procedure references with grant annotations | §7.4         |
| Modal types     | `File@Open`, `File@Closed`              | State-dependent type projections            | §7.6         |
| Union types     | `T \/ U`                                | Discriminated unions                        | §7.3.6       |
| Generic types   | `List<T>`, `Map<K, V>`                  | Parameterized over types, regions, grants   | §11          |

##### §3.2.2.2 Primitive Types

[4] _Primitive types_ are built-in scalar types with fixed representation defined by this specification. They include:

- **Integer types**: `i8`, `i16`, `i32`, `i64`, `i128`, `isize` (signed); `u8`, `u16`, `u32`, `u64`, `u128`, `usize` (unsigned)
- **Floating-point types**: `f32` (IEEE 754 binary32), `f64` (IEEE 754 binary64)
- **Boolean type**: `bool` with values `true` and `false`
- **Character type**: `char` representing Unicode scalar values
- **Unit type**: `()` with single value `()`
- **Never type**: `!` with no values (uninhabited)

[5] All primitive types satisfy the `Copy` predicate and have deterministic size and alignment (§7.2). Primitive type identifiers are reserved and cannot be redeclared (§6.6).

##### §3.2.2.3 Composite Types

[6] _Composite types_ combine or select among other types:

- **Tuples**: Structural products `(T₁, T₂, …, Tₙ)` with positional access
- **Records**: Nominal products with named fields
- **Arrays**: Fixed-size sequences `[T; n]`
- **Slices**: Dynamic views `[T]` over contiguous sequences
- **Enums**: Discriminated sums with named variants
- **Unions**: Safe structural unions `T \/ U` with runtime tags

[7] Composite types may be generic (parameterized over types) and may carry permission and region annotations. Their value sets are derived compositionally from their constituent types.

##### §3.2.2.4 Nominal vs Structural Types

[8] _Nominal types_ are identified by their declaration: two record types are equivalent only if they refer to the same declaration, even if their fields are identical. Records, enums, and modal types are nominal.

[9] _Structural types_ are identified by their structure: two tuple types are equivalent when they have the same arity and element types. Tuples, arrays, slices, unions, and callable types are structural.

[10] Type aliases (§5.5) are transparent: they introduce alternate names for existing types without creating new nominal identities. Everywhere a type alias appears, it may be replaced with its definition without changing program semantics.

##### §3.2.2.5 Type Constructors

[11] _Type constructors_ are operations that build new types from existing ones:

- **Tuple constructor**: `(T₁, …, Tₙ)` forms a product type
- **Array constructor**: `[T; n]` forms a fixed-size array
- **Slice constructor**: `[T]` forms a dynamic view
- **Pointer constructor**: `Ptr<T>` forms a pointer type
- **Union constructor**: `T \/ U` forms a discriminated union
- **Generic instantiation**: `List<i32>` instantiates a generic type with concrete arguments

[12] Type constructors may be nested arbitrarily: `[Ptr<(i32, f64)>; 10]` is a valid type representing an array of pointers to tuples.

#### §3.2.3 Type Properties [basic.type.properties]

##### §3.2.3.1 Sized Types

[13] A type is _sized_ when its size and alignment can be determined at compile time. Most types are sized. Slices (`[T]`) and trait objects (future feature) are _unsized_ because their size depends on runtime values.

[14] Sized types may be stored in bindings directly. Unsized types must be accessed through pointers or references that carry runtime size information. The type system enforces that unsized types appear only in positions where size information is available (behind pointers, in generic bounds).

##### §3.2.3.2 Copy Types

[15] A type satisfies `Copy` when values of that type can be duplicated bitwise without violating safety invariants. The `Copy` predicate is specified in §10.4; types that implement `Copy` may be copied implicitly during assignment and parameter passing.

[16] Non-`Copy` types must be explicitly moved. After a move, the source binding becomes inaccessible (definite assignment analysis tracks this, §5.7).

##### §3.2.3.3 Inhabited Types

[17] A type is _inhabited_ when its value set is non-empty: $\llbracket T \rrbracket \ne \emptyset$. Most types are inhabited. The never type `!` is _uninhabited_: $\llbracket ! \rrbracket = \emptyset$.

[18] Expressions of uninhabited types represent computations that never produce values (divergence). Such expressions coerce to any type (§7.2.7) and are used to model functions that never return (panic, exit, infinite loops).

##### §3.2.3.4 Examples

**Example 3.2.3.1 (Sized and unsized types):**

```cursive
let numbers: [i32; 5] = [1, 2, 3, 4, 5]    // Sized: size = 5 × sizeof(i32) = 20 bytes
let view: [i32] = numbers[..]               // Unsized: size depends on runtime slice bounds
// let invalid: [i32]                       // error: unsized type in binding requires indirection
```

**Example 3.2.3.2 (Copy and non-Copy types):**

```cursive
// i32 satisfies Copy
let a: i32 = 10
let b = a                  // Copied
let c = a                  // `a` still valid

// Assume Buffer does not satisfy Copy
let buf1 = Buffer::new()
let buf2 = buf1            // Moved
// let buf3 = buf1         // error[E05-703]: use of moved value
```

#### §3.2.4 Type Equivalence and Subtyping [basic.type.relations]

[19] Two types may be related through equivalence or subtyping:

- **Type equivalence** (`τ₁ ≡ τ₂`): Types are interchangeable in all contexts after expanding aliases and substituting generic arguments. Defined formally in §7.7.2.
- **Subtyping** (`τ₁ <: τ₂`): Values of type `τ₁` may be used where `τ₂` is expected, possibly with implicit coercion. Defined formally in §7.7.3.

[20] Subtyping in Cursive includes:

- Permission downgrades: `unique T <: shared T <: const T`
- Modal state widening: `M@State <: M`
- Union introduction: `T <: T \/ U`
- Never-type coercion: `! <: T` for all types `T`

[21] Subtyping is transitive but not symmetric. It enables flexible APIs while maintaining type safety through the rules in §7.7.

#### §3.2.5 Type Annotations and Inference [basic.type.inference]

[22] Type annotations use the syntax `: Type` and may appear on bindings, procedure parameters, procedure returns, and in various type contexts. Annotations serve two purposes:

1. **Specification**: Explicitly document the intended type
2. **Constraint**: Restrict inference to produce the specified type or a subtype

[23] When annotations are omitted, Cursive performs bidirectional type inference (§7.1.2): information flows from expressions to contexts and from contexts to expressions. Inference succeeds when a unique principal type exists; ambiguous situations require annotations.

[24] Inference preserves determinism: given identical source code and compilation environment, inference always produces the same types. There is no ordering dependence or backtracking that could yield non-deterministic results.

**Example 3.2.5.1 (Annotations and inference):**

```cursive
let explicit: i64 = 42                    // Explicit annotation
let inferred = 42                         // Inferred as i32 (default)
let contextual: i64 = inferred            // error: type mismatch i32 vs i64

procedure compute(value: i32): i64
{
    let doubled = value * 2                // Inferred as i32
    result doubled as i64                  // Explicit conversion required
}
```

#### §3.2.6 Integration Points [basic.type.integration]

[25] The type system integrates with other language features:

- **Expressions (§8)**: Every expression has a type computed by type-checking rules
- **Declarations (§5)**: Bindings and procedures introduce typed entities
- **Generics (Clause 10)**: Type parameters abstract over concrete types
- **Memory (Clause 11)**: Permissions and regions attach to types
- **Contracts (Clause 12)**: Contractual sequents reference types in predicates

[26] Clause 7 provides complete type system specification. This subclause establishes only the conceptual foundation necessary for understanding object-value relationships and storage properties.

#### §3.2.7 Conformance Requirements [basic.type.requirements]

[27] Implementations shall:

1. Support all type families listed in Table 3.2.1 with the semantics specified in Clause 7
2. Distinguish nominal and structural types according to §3.2.2.4
3. Enforce sized type restrictions: unsized types shall not appear in direct bindings
4. Implement bidirectional type inference deterministically
5. Maintain type equivalence and subtyping relations as specified in §7.7
6. Emit diagnostics for inference failures (E03-101, E03-102) and type mismatches (Clause 7 diagnostic family)

---

**Previous**: §3.1 Objects and Values | **Next**: §3.3 Scope and Lifetime (Overview)
