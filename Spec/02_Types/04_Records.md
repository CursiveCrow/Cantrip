# Part II: Type System - §8. Records and Classes

**Section**: §8 | **Part**: Type System (Part II)
**Previous**: [Tuples](03_Tuples.md) | **Next**: [Enums](05_Enums.md)

---

**Definition 8.1 (Record):** A record is a labeled Cartesian product of its fields. Formally, if `R { f₁: T₁; …; fₙ: Tₙ }` then `⟦R⟧ ≅ T₁ × … × Tₙ` with named projections `fᵢ : R → Tᵢ`. Records provide procedures (see §13.7) within their definition; inheritance is not part of the model—use traits and composition.

## 8. Records and Classes

### 8.1 Overview

**Key innovation/purpose:** Records provide named product types with procedures, enabling data encapsulation and object-oriented programming without inheritance complexity.

**When to use records:**
- Structured data with named fields
- Data that benefits from semantic field names
- Types requiring associated procedures (methods)
- Building blocks for complex data structures
- Domain modeling with clear structure
- When you need encapsulation and visibility control

**When NOT to use records:**
- Simple heterogeneous groupings → use tuples (§7)
- Discriminated unions → use enums (§9)
- Homogeneous collections → use arrays (§6) or Vec<T>
- Temporary data groupings → use tuples
- Very small data (2-3 fields) without procedures → consider tuples

**Relationship to other features:**
- **Tuples**: Records are like tuples with named fields instead of positional access
- **Traits**: Records implement traits for polymorphic behavior
- **Generics**: Records can be generic over type parameters
- **Procedures**: Records define procedures within their definition
- **Permissions**: Record fields can have different permissions (own, mut, immutable)
- **Memory layout**: Controllable via attributes for FFI and performance

### 8.2 Syntax

#### 8.2.1 Concrete Syntax

**Record declaration:**
```ebnf
RecordDecl   ::= "record" Ident GenericParams? "{" FieldList "}"
FieldList    ::= Field (";" Field)* ";"?
Field        ::= Visibility? Ident ":" Type
Visibility   ::= "public" | "private"
GenericParams ::= "<" TypeParam ("," TypeParam)* ">"
```

**Record construction:**
```ebnf
RecordExpr   ::= Ident "{" FieldInit ("," FieldInit)* ","? "}"
FieldInit    ::= Ident ":" Expr
               | Ident              // Shorthand: field: field
```

**Field access:**
```ebnf
FieldAccess  ::= Expr "." Ident
```

**Examples:**
```cantrip
record Point {
    x: f64;
    y: f64;
}

public record User {
    public name: String;
    public email: String;
    private password_hash: String;
}

let origin = Point { x: 0.0, y: 0.0 };
let x_coord = origin.x;
```

#### 8.2.2 Abstract Syntax

**Record types:**
```
τ ::= record Name { f₁: τ₁; ...; fₙ: τₙ }    (record type)
    | Name                                     (record type reference)
```

**Record expressions:**
```
e ::= Name { f₁: e₁, ..., fₙ: eₙ }            (record literal)
    | e.f                                      (field access)
```

**Record patterns:**
```
p ::= Name { f₁: p₁, ..., fₙ: pₙ }            (record pattern)
    | Name { f₁, ..., fₙ }                     (shorthand pattern)
```

#### 8.2.3 Basic Examples

**Simple record:**
```cantrip
record Point {
    x: f64;
    y: f64;
}

let p = Point { x: 3.0, y: 4.0 };
let distance = ((p.x * p.x) + (p.y * p.y)).sqrt();
```

**Field shorthand:**
```cantrip
let x = 3.0;
let y = 4.0;
let point = Point { x, y };  // Equivalent to Point { x: x, y: y }
```

**Generic record:**
```cantrip
record Pair<T, U> {
    first: T;
    second: U;
}

let pair = Pair { first: 42, second: "hello" };
```

### 8.3 Static Semantics

#### 8.3.1 Well-Formedness Rules

**Record declaration:**
```
[WF-Record]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
fields f₁, ..., fₙ are distinct
──────────────────────────────────────────────
Γ ⊢ record Name { f₁: T₁; ...; fₙ: Tₙ } : Type
```

**Generic record:**
```
[WF-Generic-Record]
Γ, α₁ : Type, ..., αₘ : Type ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
fields f₁, ..., fₙ are distinct
────────────────────────────────────────────────────────────────────
Γ ⊢ record Name<α₁, ..., αₘ> { f₁: T₁; ...; fₙ: Tₙ } : Type
```

#### 8.3.2 Type Rules

**Record construction:**
```
[T-Record]
record Name { f₁: T₁; ...; fₙ: Tₙ } in Γ
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
───────────────────────────────────────────
Γ ⊢ Name { f₁: e₁, ..., fₙ: eₙ } : Name
```

**Field access:**
```
[T-Field]
Γ ⊢ e : record S    field f : T in S
────────────────────────────────────
Γ ⊢ e.f : T
```

**Record pattern:**
```
[T-Record-Pattern]
record Name { f₁: T₁; ...; fₙ: Tₙ }
Γ ⊢ p₁ : T₁ ⇝ Γ₁    ...    Γ ⊢ pₙ : Tₙ ⇝ Γₙ
────────────────────────────────────────────────
Γ ⊢ Name { f₁: p₁, ..., fₙ: pₙ } : Name ⇝ Γ₁ ∪ ... ∪ Γₙ
```

#### 8.3.3 Type Properties

**Theorem 8.1 (Structural Typing):**

Records with identical field names and types are distinct types:
```
record A { x: i32; y: i32 } ≠ record B { x: i32; y: i32 }
```

Cantrip uses nominal typing for records, not structural typing.

**Theorem 8.2 (Copy Semantics):**

A record is `Copy` if and only if all its fields are `Copy`:
```
record R { f₁: T₁; ...; fₙ: Tₙ } : Copy
⟺
T₁ : Copy ∧ ... ∧ Tₙ : Copy
```

**Theorem 8.3 (Alignment and Size):**

For record R with fields f₁: T₁, ..., fₙ: Tₙ:
```
align(R) = max(align(T₁), ..., align(Tₙ))
size(R) = aligned_sum(size(T₁), ..., size(Tₙ), align(R))
```

where aligned_sum accounts for padding between fields.

### 8.4 Dynamic Semantics

#### 8.4.1 Evaluation Rules

**Record construction:**
```
[E-Record]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
─────────────────────────────────────────────────────
⟨Name { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨Name { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

**Field access:**
```
[E-Field]
⟨e, σ⟩ ⇓ ⟨Name { f₁: v₁, ..., fᵢ: vᵢ, ..., fₙ: vₙ }, σ'⟩
──────────────────────────────────────────────────────────
⟨e.fᵢ, σ⟩ ⇓ ⟨vᵢ, σ'⟩
```

#### 8.4.2 Memory Representation

**Default layout:**

Records are laid out in memory with fields in declaration order, with padding inserted for alignment:

```
record Example {
    a: u8;      // offset 0
    b: u32;     // offset 4 (3 bytes padding)
    c: u16;     // offset 8
}

Memory layout:
┌──┬──┬──┬──┬────┬────┬──┐
│a │p │p │p │ b  │ c  │p │
└──┴──┴──┴──┴────┴────┴──┘
Total: 12 bytes (rounded to alignment of 4)
```

**Layout calculation:**
```
layout(record R) = {
    size = ∑ᵢ size(fᵢ) + padding
    align = max(align(f₁), ..., align(fₙ))
    offset(fᵢ) = aligned(∑ⱼ₍ⱼ₍ᵢ₎ size(fⱼ), align(fᵢ))
}
```

#### 8.4.3 Operational Behavior

**Copy vs Move:**

```cantrip
record CopyRecord {
    x: i32;  // Copy type
    y: i32;  // Copy type
}

let r1 = CopyRecord { x: 1, y: 2 };
let r2 = r1;  // Copy (both i32 fields are Copy)
// r1 is still usable

record MoveRecord {
    data: String;  // Non-Copy type
}

let m1 = MoveRecord { data: String::new() };
let m2 = m1;  // Move (String is not Copy)
// m1 is now unusable
```

### 8.5 Procedures

**Definition 8.2 (Procedure):** A procedure is a method with an explicit `self` parameter defined within a record.

**Syntax:**
```cantrip
record TypeName {
    // fields

    procedure method_name(self: Permission TypeName, params): T
        must ...;
        will ...;
    {
        ...
    }
}
```

**Type rule:**
```
[T-Procedure]
procedure m : Self -> T -> U in record S
Γ ⊢ self : S    Γ ⊢ arg : T
───────────────────────────────────────
Γ ⊢ self.m(arg) : U
```

**Procedure call desugaring:**
```
obj.procedure(args) ≡ Type::procedure(obj, args)
```

**Example:**
```cantrip
record Point {
    x: f64;
    y: f64;

    procedure new(x: f64, y: f64): own Point {
        own Point { x, y }
    }

    procedure distance(self: Point, other: Point): f64 {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }

    procedure move_by(self: mut Point, dx: f64, dy: f64) {
        self.x += dx;
        self.y += dy;
    }
}
```

### 8.6 Advanced Features

#### 8.6.1 Memory Layout Control

**C-compatible layout:**
```cantrip
#[repr(C)]
record Vector3 {
    x: f32;  // offset 0
    y: f32;  // offset 4
    z: f32;  // offset 8
}
// Size: 12 bytes, Align: 4
```

**Formal semantics:**
```
#[repr(C)] ⟹ {
    fields laid out in declaration order
    no reordering optimization
    matches C struct layout
    ABI-compatible with extern "C"
}
```

**Packed layout (no padding):**
```cantrip
#[repr(packed)]
record NetworkHeader {
    magic: u32;     // offset 0
    version: u16;   // offset 4
    length: u16;    // offset 6
}
// Size: 8 bytes, Align: 1
```

**Warning:** Packed layouts can cause unaligned access, which may be undefined behavior on some architectures.

**Explicit alignment:**
```cantrip
#[repr(align(64))]
record CacheLine {
    data: [u8; 64];
}
// Aligned to 64-byte boundary for cache optimization
```

#### 8.6.2 Structure-of-Arrays

**Attribute:**
```cantrip
#[soa]
record Particle {
    position: Vec3;
    velocity: Vec3;
    lifetime: f32;
}
```

**Compiler transformation:**
```cantrip
// Generates:
record ParticleSOA {
    positions: Vec<Vec3>;
    velocities: Vec<Vec3>;
    lifetimes: Vec<f32>;
    length: usize;

    procedure get(self: ParticleSOA, index: usize): Particle
        must index < self.length;
    {
        Particle {
            position: self.positions[index],
            velocity: self.velocities[index],
            lifetime: self.lifetimes[index],
        }
    }
}
```

**Benefits:**
- Better cache locality for iterating over specific fields
- SIMD optimization opportunities
- Useful for game engines and data-intensive applications

### 8.7 Examples and Patterns

#### 8.7.1 Basic Data Modeling

**Domain object:**
```cantrip
public record User {
    public id: u64;
    public name: String;
    public email: String;
    private password_hash: String;

    procedure new(name: String, email: String, password: String): own User
        uses alloc.heap, crypto.hash;
    {
        own User {
            id: generate_id(),
            name,
            email,
            password_hash: hash_password(password),
        }
    }

    procedure verify_password(self: User, password: String): bool
        uses crypto.hash;
    {
        hash_password(password) == self.password_hash
    }
}
```

#### 8.7.2 Builder Pattern

**Builder for complex construction:**
```cantrip
record Config {
    host: String;
    port: u16;
    timeout: Duration;
    retries: u32;
}

record ConfigBuilder {
    host: Option<String>;
    port: Option<u16>;
    timeout: Option<Duration>;
    retries: Option<u32>;

    procedure new(): own ConfigBuilder {
        own ConfigBuilder {
            host: None,
            port: None,
            timeout: None,
            retries: None,
        }
    }

    procedure host(self: mut ConfigBuilder, host: String): mut ConfigBuilder {
        self.host = Some(host);
        self
    }

    procedure port(self: mut ConfigBuilder, port: u16): mut ConfigBuilder {
        self.port = Some(port);
        self
    }

    procedure build(self: ConfigBuilder): Result<Config, Error> {
        Ok(Config {
            host: self.host.ok_or("host required")?,
            port: self.port.unwrap_or(8080),
            timeout: self.timeout.unwrap_or(Duration.seconds(30)),
            retries: self.retries.unwrap_or(3),
        })
    }
}
```

#### 8.7.3 Newtype Pattern

**Type safety through wrapping:**
```cantrip
record UserId(u64) {
    procedure new(id: u64): UserId {
        UserId(id)
    }

    procedure as_u64(self: UserId): u64 {
        self.0
    }
}

record PostId(u64);

// Compiler prevents mixing UserId and PostId
function get_user(id: UserId): User { ... }

let user_id = UserId.new(42);
let post_id = PostId.new(42);

get_user(user_id);  // OK
// get_user(post_id);  // ERROR: type mismatch
```

#### 8.7.4 Composition Over Inheritance

**Using records and traits:**
```cantrip
trait Drawable {
    function draw(self: Self, canvas: Canvas)
        uses render.draw;
}

record Circle: Drawable {
    center: Point;
    radius: f64;

    procedure draw(self: Self, canvas: Canvas)
        uses render.draw;
    {
        canvas.draw_circle(self.center, self.radius);
    }
}

record Rectangle: Drawable {
    top_left: Point;
    width: f64;
    height: f64;

    procedure draw(self: Self, canvas: Canvas)
        uses render.draw;
    {
        canvas.draw_rect(self.top_left, self.width, self.height);
    }
}
```

### 8.8 Tuple Structs

**Definition 8.2 (Tuple Struct):** A tuple struct is a nominal record type with unnamed, positionally-accessed fields. Formally, `record Name(T₁, ..., Tₙ)` creates a distinct type with fields accessed as `.0`, `.1`, etc. Unlike type aliases, tuple structs are not transparent and provide nominal type safety.

#### 8.8.1 Syntax

**Grammar:**
```ebnf
TupleStruct     ::= "record" Ident GenericParams? "(" TypeList ")" ";"
TypeList        ::= Type ("," Type)* ","?
TupleStructExpr ::= Ident "(" ExprList ")"
TupleFieldAccess ::= Expr "." IntLiteral
```

**Examples:**
```cantrip
// Basic tuple structs
record Point2D(f64, f64);
record Color(u8, u8, u8);
record Newtype(i32);

// Generic tuple structs
record Wrapper<T>(T);
record Pair<A, B>(A, B);

// Construction
let p = Point2D(1.0, 2.0);
let c = Color(255, 128, 0);

// Field access by position
let x = p.0;  // First field
let y = p.1;  // Second field
```

#### 8.8.2 Abstract Syntax

**Tuple struct types:**
```
τ ::= record Name(τ₁, ..., τₙ)    (tuple struct type)
    | Name                         (tuple struct type reference)
```

**Tuple struct expressions:**
```
e ::= Name(e₁, ..., eₙ)            (tuple struct construction)
    | e.n                          (positional field access, n ∈ ℕ)
```

**Tuple struct patterns:**
```
p ::= Name(p₁, ..., pₙ)            (tuple struct pattern)
```

#### 8.8.3 Well-Formedness Rules

**[WF-TupleStruct] Tuple Struct Declaration:**
```
[WF-TupleStruct]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
n ≥ 1    (at least one field)
────────────────────────────────────────────
Γ ⊢ record Name(T₁, ..., Tₙ) well-formed
```

**Explanation:** Tuple structs must have at least one field. Zero-field tuple structs are not allowed (use unit type `()` instead).

```cantrip
// ✓ Valid: at least one field
record UserId(u64);
record Point(f64, f64);

// ✗ Invalid: zero fields
// record Empty();  // Use () instead
```

**[WF-TupleStruct-Generic] Generic Tuple Struct:**
```
[WF-TupleStruct-Generic]
Γ, α₁ : Type, ..., αₖ : Type ⊢ T₁ : Type    ...    Γ, α₁ : Type, ..., αₖ : Type ⊢ Tₙ : Type
────────────────────────────────────────────
Γ ⊢ record Name<α₁, ..., αₖ>(T₁, ..., Tₙ) well-formed
```

**Explanation:** Generic tuple structs can be parameterized by type variables.

```cantrip
// Generic wrapper
record Box<T>(T);

// Multiple type parameters
record Result<T, E>(bool, T, E);  // (success, value, error)

let b: Box<i32> = Box(42);
let r: Result<String, Error> = Result(true, "ok", error_val);
```

#### 8.8.4 Type Rules

**[T-TupleStruct-Cons] Tuple Struct Construction:**
```
[T-TupleStruct-Cons]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
────────────────────────────────────────────
Γ ⊢ Name(e₁, ..., eₙ) : Name
```

**Explanation:** Tuple struct construction must arguments matching field types in order.

```cantrip
record Point3D(f64, f64, f64);

let p = Point3D(1.0, 2.0, 3.0);  // ✓ Correct types
// let bad = Point3D(1, 2, 3);   // ✗ ERROR: expected f64, found i32
```

**[T-TupleStruct-Field] Positional Field Access:**
```
[T-TupleStruct-Field]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e : Name
0 ≤ i < n
────────────────────────────────────────────
Γ ⊢ e.i : Tᵢ₊₁    (0-indexed)
```

**Explanation:** Field access uses numeric literals: `.0` for first field, `.1` for second, etc.

```cantrip
record Color(u8, u8, u8);

let c = Color(255, 128, 0);
let red = c.0;    // ✓ u8
let green = c.1;  // ✓ u8
let blue = c.2;   // ✓ u8
// let bad = c.3; // ✗ ERROR: no field 3
```

**[T-TupleStruct-Nominal] Nominal Typing:**
```
[T-TupleStruct-Nominal]
record A(T₁, ..., Tₙ) declared
record B(T₁, ..., Tₙ) declared
A ≠ B
────────────────────────────────────────────
A ≢ B    (not equivalent)
```

**Explanation:** Tuple structs with identical field types are distinct types (nominal typing).

```cantrip
record Meters(f64);
record Feet(f64);

let m = Meters(10.0);
let f = Feet(33.0);

// ✗ ERROR: cannot assign Feet to Meters (distinct types)
// let distance: Meters = f;

// Must convert explicitly
function feet_to_meters(f: Feet) -> Meters {
    Meters(f.0 * 0.3048)
}
```

**[T-TupleStruct-Pattern] Pattern Matching:**
```
[T-TupleStruct-Pattern]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ p₁ : T₁    ...    Γ ⊢ pₙ : Tₙ
────────────────────────────────────────────
Γ ⊢ Name(p₁, ..., pₙ) : pattern for Name
```

**Explanation:** Tuple structs can be destructured in patterns.

```cantrip
record Point(i32, i32);

let p = Point(5, 10);

match p {
    Point(x, y) => {
        println("x: {}, y: {}", x, y);
    }
}

// Or using let binding
let Point(x, y) = p;
assert(x == 5 && y == 10);
```

**[T-TupleStruct-Method] Methods on Tuple Structs:**
```
[T-TupleStruct-Method]
record Name(T₁, ..., Tₙ) {
    procedure f(self: Self, ...) : U { e }
}
────────────────────────────────────────────
Methods can access self.0, self.1, ..., self.n-1
```

**Explanation:** Tuple structs support methods within their definition.

```cantrip
record Vector2(f64, f64) {
    procedure length(self: Self): f64 {
        (self.0 * self.0 + self.1 * self.1).sqrt()
    }

    procedure dot(self: Self, other: Vector2): f64 {
        self.0 * other.0 + self.1 * other.1
    }
}

let v = Vector2(3.0, 4.0);
assert(v.length() == 5.0);
```

**[T-TupleStruct-Trait] Trait Implementation:**
```
[T-TupleStruct-Trait]
record Name(T₁, ..., Tₙ): Trait {
    // procedures implementing Trait
}
────────────────────────────────────────────
Name : Trait
```

**Explanation:** Tuple structs can implement traits.

```cantrip
record UserId(u64): Display {
    procedure fmt(self: Self): String {
        "User#" + self.0.to_string()
    }
}

let id = UserId(42);
println("{}", id);  // Output: User#42
```

**[T-TupleStruct-Generic-Inst] Generic Instantiation:**
```
[T-TupleStruct-Generic-Inst]
record Name<α₁, ..., αₖ>(T₁, ..., Tₙ) declared
Γ ⊢ U₁ : Type    ...    Γ ⊢ Uₖ : Type
────────────────────────────────────────────
Γ ⊢ Name<U₁, ..., Uₖ> : Type
```

**Explanation:** Generic tuple structs are instantiated by providing concrete types.

```cantrip
record Wrapper<T>(T);

let i: Wrapper<i32> = Wrapper(42);
let s: Wrapper<String> = Wrapper("hello");

// Generic with multiple parameters
record Pair<A, B>(A, B);
let p: Pair<i32, String> = Pair(1, "one");
```

**[T-TupleStruct-Mutability] Mutable Field Access:**
```
[T-TupleStruct-Mutability]
record Name(T₁, ..., Tₙ) declared
Γ ⊢ e : mut Name
0 ≤ i < n
────────────────────────────────────────────
Γ ⊢ e.i : mut Tᵢ₊₁    (mutable access)
```

**Explanation:** Mutable tuple struct instances allow mutable field access.

```cantrip
record Counter(i32);

var c = Counter(0);
c.0 += 1;  // ✓ Mutable field access
assert(c.0 == 1);
```

#### 8.8.5 Memory Representation

Tuple structs have the same memory layout as tuples with the same field types:

```
record Name(T₁, T₂, ..., Tₙ)

Size: size(T₁) + size(T₂) + ... + size(Tₙ) + padding
Alignment: max(align(T₁), align(T₂), ..., align(Tₙ))
```

**Memory layout:**
```
Tuple Struct Name(T₁, T₂, T₃):
┌───────────┬───────────┬───────────┐
│  field 0  │  field 1  │  field 2  │
│  (T₁)     │  (T₂)     │  (T₃)     │
└───────────┴───────────┴───────────┘
```

**Example:**
```cantrip
record Point(f64, f64);
// Size: 16 bytes (2 × 8 bytes)
// Alignment: 8 bytes

record Color(u8, u8, u8);
// Size: 3 bytes (3 × 1 byte, no padding needed if standalone)
// Alignment: 1 byte
```

**Layout control:**
Tuple structs support the same layout attributes as named records:

```cantrip
#[repr(C)]
record CPoint(f64, f64);  // C-compatible layout

#[repr(packed)]
record Packed(u8, u16, u8);  // No padding

#[repr(align(64))]
record CacheAligned(i32);  // 64-byte alignment
```

#### 8.8.6 Comparison: Tuple Structs vs. Tuples vs. Named Records

| Feature | Tuple `(T₁, T₂)` | Tuple Struct `record Name(T₁, T₂)` | Named Record `record Name { a: T₁; b: T₂ }` |
|---------|------------------|-----------------------------------|---------------------------------------------|
| **Type identity** | Structural | Nominal | Nominal |
| **Field access** | `.0`, `.1` | `.0`, `.1` | `.a`, `.b` |
| **Type safety** | Weak (structural) | Strong (nominal) | Strong (nominal) |
| **Semantic meaning** | Generic grouping | Domain-specific type | Domain-specific type |
| **Trait impls** | Cannot add custom | Can add custom | Can add custom |
| **Methods** | No | Yes (inline) | Yes (inline) |
| **Readability** | Low (positional) | Medium (positional) | High (named fields) |
| **Use case** | Temporary data | Newtype pattern, wrappers | Complex data structures |

**When to use each:**

- **Tuples** `(T₁, T₂)`: Temporary, ad-hoc groupings with no semantic meaning
- **Tuple Structs** `record Name(T₁, T₂)`: Newtype pattern, type safety for similar types, simple wrappers
- **Named Records** `record Name { f₁: T₁; f₂: T₂ }`: Complex data structures with many fields, public APIs

#### 8.8.7 Examples and Patterns

**Pattern 1: Newtype Pattern (Type Safety)**

```cantrip
// Prevent mixing similar types
record UserId(u64);
record ProductId(u64);
record OrderId(u64);

function get_user(id: UserId) -> User { ... }
function get_product(id: ProductId) -> Product { ... }

let user_id = UserId(12345);
let product_id = ProductId(67890);

get_user(user_id);      // ✓ Correct
// get_user(product_id); // ✗ ERROR: type mismatch
```

**Pattern 2: Unit Wrapping**

```cantrip
// Wrap primitive types with units
record Meters(f64) {
    procedure to_feet(self: Self): f64 {
        self.0 * 3.28084
    }
}

record Seconds(f64);
record Kilograms(f64);

function calculate_velocity(distance: Meters, time: Seconds) -> f64 {
    distance.0 / time.0  // Meters per second
}

let d = Meters(100.0);
let t = Seconds(10.0);
let velocity = calculate_velocity(d, t);  // ✓ Type-safe
```

**Pattern 3: Validated Types**

```cantrip
// Wrap types with validation
record Email(String) {
    procedure new(s: String): Result<Email, String> {
        if s.contains('@') && s.len() > 3 {
            Result::Ok(Email(s))
        } else {
            Result::Err("Invalid email format")
        }
    }

    procedure domain(self: Self): String {
        let parts = self.0.split('@');
        parts[1]
    }
}

// Usage
match Email::new("user@example.com") {
    Result::Ok(email) => println("Domain: {}", email.domain()),
    Result::Err(err) => println("Error: {}", err)
}
```

**Pattern 4: Smart Pointers and Wrappers**

```cantrip
// Generic wrapper with additional behavior
record Rc<T>(Arc<T>) {  // Reference-counted pointer
    procedure new(value: T): Rc<T> {
        Rc(Arc::new(value))
    }

    procedure get(self: Self): T
        where T: Clone
    {
        (*self.0).clone()
    }
}

let shared = Rc::new(42);
let value = shared.get();
```

**Pattern 5: Index Types (Type-Safe Indexing)**

```cantrip
// Strongly-typed indices
record UserIndex(usize);
record ProductIndex(usize);

record Database<T> {
    items: Vec<T>;

    procedure get_user(self: Self, idx: UserIndex): Option<T> {
        self.items.get(idx.0)
    }

    procedure get_product(self: Self, idx: ProductIndex): Option<T> {
        self.items.get(idx.0)
    }
}

let users: Database<User> = Database { items: vec![...] };
let idx = UserIndex(0);
let user = users.get_user(idx);  // ✓ Type-safe indexing
```

**Pattern 6: Phantom Types (Compile-Time State)**

```cantrip
// Use tuple structs with phantom type parameters
record PhantomData<T>(());

record TypedId<T> {
    id: u64;
    _phantom: PhantomData<T>;

    procedure new(id: u64): TypedId<T> {
        TypedId { id: id, _phantom: PhantomData(()) }
    }
}

// Usage
let user_id: TypedId<User> = TypedId::new(123);
let product_id: TypedId<Product> = TypedId::new(456);

// ✗ ERROR: cannot assign TypedId<Product> to TypedId<User>
// let x: TypedId<User> = product_id;
```

#### 8.8.8 Type Properties

**Theorem 8.1 (Tuple Struct Nominality):** Tuple structs with identical field types are distinct types. For any tuple structs `record A(T)` and `record B(T)`, `A ≠ B` implies `A ≢ B`.

**Proof sketch:** The type system assigns each tuple struct declaration a unique nominal identity. Even if field types are identical, the nominal identities differ, making the types incompatible. ∎

**Corollary 8.1:** Tuple structs provide type safety through nominal distinction, preventing accidental type confusion.

**Theorem 8.2 (Memory Layout Equivalence):** A tuple struct `record Name(T₁, ..., Tₙ)` has the same memory layout as the tuple type `(T₁, ..., Tₙ)`.

**Proof sketch:** Both tuple structs and tuples arrange fields in declaration order with appropriate padding. The only difference is the nominal type tag, which exists only at compile time. ∎

**Corollary 8.2:** Tuple structs have zero runtime overhead compared to tuples.

**Theorem 8.3 (Pattern Exhaustiveness):** Pattern matching on tuple structs is exhaustive if and only if all fields are matched.

**Proof sketch:** A tuple struct `Name(T₁, ..., Tₙ)` has exactly one variant. A pattern `Name(p₁, ..., pₙ)` is exhaustive if all patterns `pᵢ` are exhaustive for their respective types `Tᵢ`. ∎

---

**Previous**: [Tuples](03_Tuples.md) | **Next**: [Enums](05_Enums.md)
