# Cursive Language Specification

## Clause 7 — Types

**Part**: VII — Type System  
**File**: 07-3_Composite-Types.md
**Section**: 7.3 Composite Types  
**Stable label**: [type.composite]  
**Forward references**: §7.4 [type.function], §7.5 [type.pointer], §7.7 [type.relation], §8.4 [expr.structured], §8.3 [expr.operator], Clause 9 [stmt], Clause 10 [generic], Clause 11 [memory]

---

### §7.3 Composite Types [type.composite]

[1] Composite types combine or select among other types. They cover structural products (tuples), nominal products (records), sequential collections (arrays, slices, strings, ranges), discriminated sums (enums), and safe unions (`τ₁ \/ τ₂`).

[2] Composite types integrate with permissions (§11.2), regions (§11.3), contracts (Clause 12), and generics (Clause 10). Each constructor follows the standard template: syntax, formation constraints, semantics, and canonical examples. Where relevant, Copy predicates and size/alignment rules are stated explicitly.

[3] This subclause is organized as follows:

- §7.3.2 Tuples (structural products)
- §7.3.3 Records (nominal products with dual access)
- §7.3.4 Collections (arrays, slices, strings, ranges)
- §7.3.5 Enums (discriminated sums)
- §7.3.6 Union types (safe structural unions)
- §7.3.7 Conformance requirements

---

#### §7.3.2 Tuples [type.composite.tuple]

##### §7.3.2.1 Overview [type.composite.tuple.overview]

[4] Tuple types are anonymous structural products. Two tuple types are equivalent when they have the same arity and pairwise equivalent element types. Tuple components inherit the permissions of the aggregate.

##### §7.3.2.2 Syntax [type.composite.tuple.syntax]

[5] Tuple types require at least two elements:

```
TupleType ::= '(' Type (',' Type)+ ')'
```

[6] Single-element tuple syntax is prohibited to avoid ambiguity with parenthesized expressions. The zero-element product is the unit type (§7.2.6).

##### §7.3.2.3 Constraints [type.composite.tuple.constraints]

[7] Tuple well-formedness requires each component type to be well-formed:

$$
\dfrac{\Gamma \vdash \tau_1 : \text{Type} \quad \cdots \quad \Gamma \vdash \tau_n : \text{Type} \quad n \ge 2}{\Gamma \vdash (\tau_1, \ldots, \tau_n) : \text{Type}}
\tag{WF-Tuple}
$$

[8] Construction requires matching types for each element:

$$
\dfrac{\Gamma \vdash e_1 : \tau_1 \quad \cdots \quad \Gamma \vdash e_n : \tau_n}{\Gamma \vdash (e_1, \ldots, e_n) : (\tau_1, \ldots, \tau_n)}
\tag{T-Tuple-Ctor}
$$

[9] Projection is zero-based:

$$
\dfrac{\Gamma \vdash e : (\tau_1, \ldots, \tau_n) \quad 0 \le i < n}{\Gamma \vdash e.i : \tau_{i+1}}
\tag{T-Tuple-Proj}
$$

##### §7.3.2.4 Semantics [type.composite.tuple.semantics]

[10] Evaluation proceeds left-to-right; side effects in elements are sequenced accordingly. Runtime representation stores elements contiguously with padding inserted to satisfy each element’s alignment. Tuple alignment equals the maximum alignment of its elements; tuple size equals the padded sum of element sizes.

[11] Copy predicate and variance:

- $(\tau_1, \ldots, \tau_n)$ satisfies `Copy` iff every $\tau_i$ does.
- Tuples are invariant in each parameter position because elements may be both read and written.

##### §7.3.2.5 Examples (Informative) [type.composite.tuple.examples]

```cursive
let point: const (f64, f64) = (3.0, 4.0)
let (x, y) = point                 // Destructuring
let next = (x + 1.0, y + 1.0)      // New tuple, same structure
let dx = point.0                   // Positional access (0-based)
```

---

#### §7.3.3 Records [type.composite.record]

##### §7.3.3.1 Overview [type.composite.record.overview]

[12] Record types are nominal products with named fields and optional methods. They support dual access: named (`value.field`) and positional (`value.0`). Positional indices mirror declaration order and enforce the same visibility as named access.

##### §7.3.3.2 Syntax [type.composite.record.syntax]

[13] Record declarations follow:

```
RecordDecl ::= 'record' Ident GenericParams? '{' FieldDecl* MethodDecl* '}'
FieldDecl  ::= Visibility? Ident ':' Type
```

Visibility modifiers are `public`, `internal` (default), or `private` (§5.6).

##### §7.3.3.3 Constraints [type.composite.record.constraints]

[14] Field names shall be unique. Each field's type shall be well-formed. Formally:

$$
\dfrac{\text{record } R \{ f_1 : \tau_1, \ldots, f_n : \tau_n \} \text{ declared} \quad \Gamma \vdash \tau_i : \text{Type}}{\Gamma \vdash R : \text{Type}}
\tag{WF-Record}
$$

[15] Construction and field access rules:

$$
\dfrac{\Gamma \vdash e_i : \tau_i}{\Gamma \vdash R \{ f_1 : e_1, \ldots, f_n : e_n \} : R}
\tag{T-Record-Ctor}
$$

$$
\dfrac{\Gamma \vdash e : R \quad f_k \text{ visible}}{\Gamma \vdash e.f_k : \tau_k}
\tag{T-Record-Field}
$$

$$
\dfrac{\Gamma \vdash e : R \quad 0 \le i < n \quad \text{field } i \text{ visible}}{\Gamma \vdash e.i : \tau_{i+1}}
\tag{T-Record-Pos}
$$

##### §7.3.3.4 Semantics [type.composite.record.semantics]

[16] Records are nominal: two records are equivalent iff they share the same declaration. Fields are laid out in declaration order with padding inserted as required. Dual access refers to the same storage location. Struct update syntax (`R { field: value, ..expr }`) copies unspecified fields from `expr`.

[17] Copy predicate: a record satisfies `Copy` iff every field does and no user-defined destructor is attached. Variance is invariant per field for the same reason as tuples.

##### §7.3.3.5 Examples (Informative) [type.composite.record.examples]

```cursive
record Point { x: f64, y: f64 }
let origin = Point { x: 0.0, y: 0.0 }
let translated = Point { x: 5.0, ..origin }
let x_named = translated.x         // Named access
let x_pos = translated.0           // Positional access (same field)
```

```cursive
record BankAccount {
    public number: u64,
    private balance: f64,
}

let acct = BankAccount { number: 10_001, balance: 250.0 }
let seen = acct.number             // OK (public)
// let hidden = acct.1            // ERROR: index 1 refers to private field
```

---

#### §7.3.4 Collections [type.composite.collection]

[18] Collections are sequential composites. They include fixed-size arrays, dynamic slices, modal strings, and range types used for iteration and slicing.

##### §7.3.4.1 Arrays [type.composite.collection.array]

###### Overview

[19] Arrays `[T; n]` store exactly `n` elements of type `T`. Length `n` is a compile-time constant. Arrays support indexing, slicing, and iteration.

###### Syntax

```
ArrayType ::= '[' Type ';' ConstExpr ']'
ArrayLiteral ::= '[' Expr (',' Expr)* ']' | '[' Expr ';' ConstExpr ']'
ArrayIndex ::= Expr '[' Expr ']'
```

###### Constraints

[20] Array formation:

$$
\dfrac{\Gamma \vdash \tau : \text{Type} \quad n \in \mathbb{N}^+}{\Gamma \vdash [\tau; n] : \text{Type}}
\tag{WF-Array}
$$

[21] Array literals require each element to type-check as `τ`. Repeat literals `[e; n]` require `e : τ` and `τ : Copy`.

###### Semantics

[22] Elements are stored contiguously with no padding between them. Array alignment equals `align(τ)`; size equals `n × size(τ)`. Indexing evaluates the index expression, converts it to `usize`, and performs bounds checking (§8.4.7). Out-of-bounds indices raise a runtime diagnostic unless proven safe at compile time.

[23] `Copy` holds for `[τ; n]` iff `τ : Copy`.

###### Examples (Informative)

```cursive
let numbers: [i32; 5] = [1, 2, 3, 4, 5]
let zeros: [i32; 8] = [0; 8]         // repeat syntax
let first = numbers[0]
let row: [f64; 3] = [[1.0, 0.0, 0.0],
                     [0.0, 1.0, 0.0],
                     [0.0, 0.0, 1.0]][1]
```

---

##### §7.3.4.2 Slices [type.composite.collection.slice]

###### Overview

[24] Slice types `[T]` are dynamic views over contiguous sequences of `T`. A slice stores a pointer and length; it does not own the underlying data. Permissions on the slice govern aliasing.

###### Syntax

```
SliceType ::= '[' Type ']'
SliceExpr ::= Expr '[' RangeExpr? ']'
```

###### Constraints

[25] Slice formation:

$$
\dfrac{\Gamma \vdash \tau : \text{Type}}{\Gamma \vdash [\tau] : \text{Type}}
\tag{WF-Slice}
$$

Slicing requires the source to be an array, owned string, or pointer supporting the slicing operation, and the range to be within bounds.

###### Semantics

[26] A slice value is `{ ptr: Ptr<τ>, len: usize }`. Copy predicate: a slice satisfies `Copy` because the view is immutable with respect to ownership; mutability is represented through permissions on the pointer. Indexing a slice is identical to array indexing with bounds checking.

###### Examples (Informative)

```cursive
let data: [u8; 10] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
let head: [u8] = data[..4]     // elements 0..3
let tail: [u8] = data[6..]     // elements 6..9
procedure checksum(bytes: [u8]): u8 { bytes.iter().fold(0, add) }
checksum(data[..])
```

---

##### §7.3.4.3 Strings [type.composite.collection.string]

###### Overview

[27] Strings are UTF-8 sequences with two modal states (defined in §7.6):

- `string@Managed`: growable, heap-allocated buffer (`ptr`, `len`, `cap`), not `Copy`.
- `string@View`: read-only view over UTF-8 data (`ptr`, `len`), `Copy`.

(27.1) **String type defaulting**: A bare `string` identifier defaults to `string@View`. The `string@Managed` state shall be explicitly specified. This default aligns with the common use case of string parameters and return types, which typically use read-only views. There is an implicit coercion `string@Managed <: string@View` established via the modal subtyping rule (`Sub-Modal-Widen`).

[ Note: String is a special case of the modal type system (§7.6) where the language provides a default state for ergonomic reasons. The defaulting behavior (`string` → `string@View`) applies in type annotation contexts and is unique to the built-in string type. User-defined modal types do not support default states and must be explicitly annotated with their state in all contexts. In type inference contexts (without explicit annotations), the compiler infers the most specific state based on usage; explicit `string` annotations resolve to `string@View` for common-case ergonomics.
— end note ]

###### Syntax

```
StringType ::= 'string'              // Defaults to string@View
             | 'string' '@Managed'
             | 'string' '@View'
StringLiteral ::= '"' UTF8Text '"'
```

[ Note: The grammar allows bare `string` as a type identifier. When used in type annotations, it resolves to `string@View` per the defaulting rule (27.1). See Annex A §A.2 for the authoritative grammar.
— end note ]

(27.2) `string` is a built-in modal type (§7.6) with two states:

- `string@Managed` — manages heap-allocated UTF-8 storage and may reallocate or mutate.
- `string@View` — an immutable span consisting of pointer + length.
  Transition signatures `string@Managed::view(~) -> @View` and `string@View::to_managed(~) -> @Managed` define the canonical conversions.

###### Constraints

[28] Well-formedness follows the modal formation rules (§7.6). String literals have type `string@View`. Conversions:

- `string.from(view: string@View) : string@Managed`
- `view.to_managed() : string@Managed`
- Implicit coercion `string@Managed` → `string@View`

###### Semantics

[29] Layout on 64-bit targets:

- `string@Managed`: `{ ptr: Ptr<u8>, len: usize, cap: usize }` (24 bytes)
- `string@View`: `{ ptr: Ptr<u8>, len: usize }` (16 bytes)

`string@View : Copy`; `string@Managed` is movable but not `Copy`. All string forms guarantee UTF-8 validity, and `string@Managed` values implicitly coerce to `string@View` by invoking the modal transition above.

**Example 7.3.4.3.1 (Managed/View transitions):**

```cursive
let greeting: string@Managed = string.from("hello")
let view: string@View = greeting.view()    // implicit when needed
print_line(view)

let roundtrip: string@Managed = view.to_managed()
print_line(roundtrip.view())
```

[ Note: The procedures `view()` and `to_managed()` are mnemonics for the modal transitions declared in §7.6; concrete names will follow the eventual Clause 10/standard library surface when specified. — end note ]

###### Examples (Informative)

**Example 7.3.4.3.2 (String type defaulting):**

```cursive
// Bare 'string' defaults to string@View
procedure print_line(text: string) {      // text: string@View
    io::write_all(text)
}

// Explicit states must be specified for managed strings
let managed: string@Managed = string.from("hello")
let view: string@View = managed           // implicit coercion
let also_view: string = managed           // bare 'string' = string@View

print_line(view)
print_line(also_view)
print_line("literal")                     // literal : string@View
```

[ Note: The example demonstrates that bare `string` in type annotations resolves to `string@View`. This defaulting applies only in explicit type annotation contexts; type inference determines the most specific type based on usage.
— end note ]

---

##### §7.3.4.4 Range Types [type.composite.collection.range]

###### Overview

[30] Range types underpin slicing and iteration. Cursive provides six constructors: `Range<T>`, `RangeInclusive<T>`, `RangeFrom<T>`, `RangeTo<T>`, `RangeToInclusive<T>`, and `RangeFull<T>`.

###### Syntax

Range literals map to constructors:

| Literal | Type constructor      | Bounds                         |
| ------- | --------------------- | ------------------------------ |
| `a..b`  | `Range<T>`            | inclusive start, exclusive end |
| `a..=b` | `RangeInclusive<T>`   | inclusive start and end        |
| `a..`   | `RangeFrom<T>`        | inclusive start, unbounded end |
| `..b`   | `RangeTo<T>`          | unbounded start, exclusive end |
| `..=b`  | `RangeToInclusive<T>` | unbounded start, inclusive end |
| `..`    | `RangeFull<T>`        | unbounded start and end        |

###### Constraints

[31] Each constructor is well-formed when `Γ ⊢ T : Type`:

$$
\dfrac{\Gamma \vdash T : \text{Type}}{\Gamma \vdash Range<T> : \text{Type}}
\quad
\cdots
\quad
\dfrac{\Gamma \vdash T : \text{Type}}{\Gamma \vdash RangeFull<T> : \text{Type}}
\tag{WF-Range}
$$

###### Semantics

[32] Ranges carry optional bounds. `Range<T>` stores `{ start: Option<T>, end: Option<T>, inclusive: bool }` with `inclusive = false`. `RangeInclusive<T>` stores `{ start: T, end: T }`. `RangeFull<T>` has no fields. Bounds are evaluated eagerly; illegal constructions (e.g., `a..b` with `b < a` for numeric ranges) are diagnosed by the consuming API.

###### Examples (Informative)

```cursive
let data: [u8; 8] = [10, 11, 12, 13, 14, 15, 16, 17]
let prefix = data[..4]
let suffix = data[4..]
loop i in 0..data.len() { println("{}", data[i]) }
```

---

#### §7.3.5 Enums [type.composite.enum]

##### §7.3.5.1 Overview [type.composite.enum.overview]

[33] Enums are nominal discriminated sums. Variants may be unit-like, tuple-like, or struct-like. Exhaustive pattern matching is required to ensure all variants are handled.

##### §7.3.5.2 Syntax [type.composite.enum.syntax]

```
EnumDecl ::= 'enum' Ident GenericParams? '{' VariantDecl (',' VariantDecl)* ','? '}'
VariantDecl ::= Ident
              | Ident '(' Type (',' Type)* ')'
              | Ident '{' FieldDecl* '}'
```

##### §7.3.5.3 Constraints [type.composite.enum.constraints]

[34] Variants shall have unique names. Payload types shall be well-formed under the enum's generic parameters:

$$
\dfrac{\Gamma, \vec{\alpha} \vdash \tau_i : \text{Type}}{\Gamma \vdash enum\;E\langle \vec{\alpha} \rangle : \text{Type}}
\tag{WF-Enum}
$$

[35] Variant construction typing rules follow the payload form (unit, tuple, struct). Generic instantiation requires well-formed arguments.

##### §7.3.5.4 Semantics [type.composite.enum.semantics]

[36] Each enum value stores a discriminant plus payload. Layout is implementation-defined but shall allocate enough space for the largest payload and align to the maximum payload alignment. `Copy` holds iff every variant payload satisfies `Copy` and no destructor is defined.

[37] Pattern matching shall be exhaustive. The compiler emits a compile-time error when variants remain unmatched. Wildcards (`_`) count as covering remaining cases.

##### §7.3.5.5 Examples (Informative) [type.composite.enum.examples]

```cursive
enum Status {
    Pending,
    Running,
    Completed,
}

match Status::Running {
    Status::Pending => println("Not started"),
    Status::Running => println("In progress"),
    Status::Completed => println("Done"),
}
```

```cursive
enum Shape {
    Point,
    Circle(f64),
    Rectangle { width: f64, height: f64 },
}

let circle = Shape::Circle(2.0)
match circle {
    Shape::Point => println("point"),
    Shape::Circle(r) => println("radius {}", r),
    Shape::Rectangle { width, height } => println("{} × {}", width, height),
}
```

---

#### §7.3.6 Union Types [type.composite.union]

##### §7.3.6.1 Overview [type.composite.union.overview]

[38] Safe union types use the operator `\/` to combine types structurally. They are discriminated unions with automatically inserted runtime tags. Unlike enums, unions are structural and may be formed without a prior declaration.

##### §7.3.6.2 Syntax [type.composite.union.syntax]

```
UnionType ::= Type '\/' Type ('\/' Type)*
```

Unions are associative, commutative, and idempotent: `τ \/* τ` ≡ `τ`; ordering of components is immaterial.

##### §7.3.6.3 Constraints [type.composite.union.constraints]

[39] A union is well-formed when each component type is well-formed:

$$
\dfrac{\Gamma \vdash \tau_1 : \text{Type} \quad \Gamma \vdash \tau_2 : \text{Type}}{\Gamma \vdash \tau_1 \/ \tau_2 : \text{Type}}
\tag{WF-Union}
$$

[40] Each component is a subtype of the union:

$$
\dfrac{}{\tau_i <: \tau_1 \/ \cdots \/ \tau_n}
\tag{Sub-Union-Intro}
$$

[41] Matching on unions requires covering each component type (typically via pattern matching or type tests). Exhaustiveness rules mirror enum coverage.

##### §7.3.6.4 Semantics [type.composite.union.semantics]

[42] The runtime representation stores a discriminant indicating the active component along with the payload. Size equals the maximum component size plus discriminant overhead; alignment equals the maximum component alignment. Copy predicate holds when every component type satisfies `Copy`.

##### §7.3.6.5 Examples (Informative) [type.composite.union.examples]

```cursive
procedure parse(input: string@View): i32 \/ parse::Error
    [[ |- true => true ]]
{
    if input.is_empty() {
        result parse::Error::invalid_data("empty")
    }
    result input.to_i32()
}

match parse("42") {
    value: i32 => println("value {}", value),
    err: parse::Error => println("error {}", err.message()),
}
```

---

#### §7.3.7 Conformance Requirements [type.composite.requirements]

[43] A conforming implementation shall:

- Support tuples, records, arrays, slices, strings, ranges, enums, and safe unions as specified.
- Enforce dual record access while respecting visibility modifiers.
- Uphold the `string@Managed`/`string@View` split, including defaulting rules and implicit coercion.
- Guarantee bounds checking semantics for arrays and slices, with diagnostics that match Clause 8 listings and Annex E §E.5 payload requirements.
- Provide exhaustive pattern diagnostics for enums and unions.
- Preserve the stated size/alignment rules and Copy predicates.

[44] Deviations from layout, semantics, or diagnostic obligations render the implementation non-conforming unless explicitly categorized as implementation-defined elsewhere.

---

**Previous**: §7.2 Primitive Types (§7.2 [type.primitive]) | **Next**: §7.4 Function and Procedure Types (§7.4 [type.function])
