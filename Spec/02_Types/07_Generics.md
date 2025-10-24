# Part II: Type System - §11. Generics and Parametric Polymorphism

**Section**: §11 | **Part**: Type System (Part II)
**Previous**: [Traits](06_Traits.md) | **Next**: [Modals](08_Modals.md)

---

**Definition 11.1 (Parametric Polymorphism):** Generic functions and types abstract over type parameters. A generic function `∀α. e : τ` is universally quantified over type variable α, and can be instantiated at any concrete type. Generic types enable code reuse while maintaining type safety through parametric polymorphism.

## 11. Generics and Parametric Polymorphism

### 11.1 Overview

**Key innovation/purpose:** Generics enable writing code once that works with multiple types, providing type-safe abstraction without runtime overhead through compile-time monomorphization.

**When to use generics:**
- Collections (Vec<T>, HashMap<K, V>, etc.)
- Functions that work with multiple types identically
- Data structures that hold values of any type
- Algorithms that don't depend on specific type details
- Trait-bounded polymorphism (generic + trait bounds)
- Type-safe containers and wrappers
- Library code requiring broad applicability

**When NOT to use generics:**
- When working with a single known type → use concrete types
- When types need different implementations → use traits with multiple impls
- Runtime polymorphism needed → use trait objects (dyn Trait)
- Excessive complexity without clear benefit → keep it simple
- When monomorphization bloat is a concern → consider alternatives

**Relationship to other features:**
- **Traits**: Provide bounds for generic type parameters
- **Monomorphization**: Generics compiled to specialized code for each instantiation
- **Type inference**: Generic parameters often inferred from context
- **Higher-kinded types**: Not supported (generics are first-order only)
- **Const generics**: Allow compile-time constants as parameters

### 11.2 Syntax

#### 11.2.1 Concrete Syntax

**Generic function:**
```ebnf
GenericFunction ::= "function" Ident GenericParams ParamList (":" Type)? WhereClauses? ContractClauses? Block
GenericParams   ::= "<" TypeParam ("," TypeParam)* ">"
TypeParam       ::= Ident (":" Bounds)?
                  | "const" Ident ":" ConstType
Bounds          ::= Bound ("+" Bound)*
Bound           ::= Ident GenericArgs?
WhereClauses    ::= "where" WhereClause ("," WhereClause)*
WhereClause     ::= Type ":" Bound ("+" Bound)*
```

**Generic record:**
```ebnf
GenericRecord   ::= "record" Ident GenericParams "{" FieldList "}"
```

**Generic enum:**
```ebnf
GenericEnum     ::= "enum" Ident GenericParams "{" VariantList "}"
```

**Examples:**
```cantrip
function identity<T>(x: T): T {
    x
}

function swap<T>(a: T, b: T): (T, T) {
    (b, a)
}

record Pair<T, U> {
    first: T;
    second: U;
}

enum Option<T> {
    Some(T),
    None,
}
```

#### 11.2.2 Abstract Syntax

**Generic function types:**
```
τ ::= ∀α. τ    (universal quantification)
```

**Generic type application:**
```
e ::= e[τ]     (type application)
τ ::= T⟨τ₁, ..., τₙ⟩    (generic type instantiation)
```

**Type schemes:**
```
σ ::= ∀α₁...αₙ. τ    (polytype/type scheme)
```

#### 11.2.3 Basic Examples

**Generic identity:**
```cantrip
function identity<T>(x: T): T {
    x
}

let a = identity(42);        // T = i32, returns 42
let b = identity("hello");   // T = str, returns "hello"
```

**Generic container:**
```cantrip
record Box<T> {
    value: T;
}

let int_box = Box { value: 42 };
let str_box = Box { value: "hello" };
```

### 11.3 Static Semantics

#### 11.3.1 Well-Formedness Rules

**Generic function:**
```
[WF-Generic-Fun]
Γ, α₁ : Type, ..., αₙ : Type ⊢ body : τ
─────────────────────────────────────────────
Γ ⊢ function f<α₁, ..., αₙ>(...): ∀α₁...αₙ. τ
```

**Generic record:**
```
[WF-Generic-Record]
Γ, α₁ : Type, ..., αₘ : Type ⊢ fields well-formed
──────────────────────────────────────────────────────
Γ ⊢ record S<α₁, ..., αₘ> { ... } : Type → ... → Type
```

**Type parameter bounds:**
```
[WF-Bounded-Param]
Γ ⊢ B₁ : Trait    ...    Γ ⊢ Bₙ : Trait
──────────────────────────────────────────────────────
Γ ⊢ α : B₁ + ... + Bₙ
```

#### 11.3.2 Type Rules

**Type instantiation:**
```
[T-Inst]
Γ ⊢ e : ∀α. τ
Γ ⊢ U : Type
──────────────────────────────
Γ ⊢ e[U] : τ[α ↦ U]
```

**Bounded instantiation:**
```
[T-Bounded-Inst]
Γ ⊢ e : ∀α : B. τ
Γ ⊢ U : Type
U : B    (U satisfies bound B)
──────────────────────────────
Γ ⊢ e[U] : τ[α ↦ U]
```

**Generic type construction:**
```
[T-Generic-Type]
Γ ⊢ T : Type → ... → Type    (n parameters)
Γ ⊢ τ₁ : Type    ...    Γ ⊢ τₙ : Type
──────────────────────────────────────────
Γ ⊢ T⟨τ₁, ..., τₙ⟩ : Type
```

**Where clause checking:**
```
[T-Where]
Γ, T : Type, T : C₁, ..., T : Cₙ ⊢ body : U
────────────────────────────────────────────────
Γ ⊢ function f<T> where T: C₁, ..., Cₙ : U
```

#### 11.3.3 Type Properties

**Theorem 11.1 (Parametricity):**

Generic functions cannot inspect or depend on the specific type of their parameters:
```
∀α. f : α → β
```
The implementation of f must work uniformly for all types α.

**Theorem 11.2 (Monomorphization):**

Every generic instantiation produces a distinct specialized function:
```
f<i32> ≠ f<String>
```
Each instantiation is compiled separately with no shared code.

**Theorem 11.3 (Type Substitution):**

Type substitution preserves well-typedness:
```
If Γ, α : Type ⊢ e : τ and Γ ⊢ U : Type
Then Γ ⊢ e[α ↦ U] : τ[α ↦ U]
```

#### 11.3.4 Type Bounds

**Purpose:** Type bounds constrain generic type parameters to types that satisfy specific trait requirements.

**Syntax:**
```cantrip
function serialize<T>(value: T): String
    where T: Serialize + Clone
    uses alloc.heap;
{
    value.clone().to_json()
}
```

**Type rule:**
```
[T-Bounds]
Γ ⊢ T : B₁    ...    Γ ⊢ T : Bₙ
──────────────────────────────────
Γ ⊢ T : B₁ + ... + Bₙ
```

**Example with multiple bounds:**
```cantrip
function process<T, U>(data: T): U
    where
        T: Clone + Serialize,
        U: Display + Debug
    uses alloc.heap;
{
    let copy = data.clone();
    let json = copy.to_json();
    U.from_json(json)
}
```

### 11.4 Dynamic Semantics

#### 11.4.1 Evaluation Rules

**Monomorphization semantics:**

Generic functions are not evaluated directly. Instead, they are instantiated at compile time:

```
[E-Monomorphize]
f<T> where T = ConcreteType
────────────────────────────────────────────
Generates specialized function f_ConcreteType
```

**Generic value construction:**
```
[E-Generic-Record]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
──────────────────────────────────────────────────────
⟨T⟨U⟩ { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨T⟨U⟩ { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

#### 11.4.2 Memory Representation

**Generic types:**

Generic types have the same memory layout as their instantiated versions:

```
size(Box<i32>) = size(i32) = 4 bytes
size(Box<String>) = size(String) = 24 bytes (on 64-bit)
```

Each instantiation has its own distinct layout.

**Monomorphization:**

```cantrip
// Generic function
function max<T: Comparable>(a: T, b: T): T {
    if a.compare(b) > 0 { a } else { b }
}

// Compiler generates specialized versions:
function max_i32(a: i32, b: i32): i32 { ... }
function max_f64(a: f64, b: f64): f64 { ... }
function max_String(a: String, b: String): String { ... }
```

**No runtime overhead:**
- All type parameters resolved at compile time
- No type tags or vtables
- Direct function calls (no indirection)
- Same performance as hand-written specialized code

#### 11.4.3 Operational Behavior

**Code generation:**

Each unique instantiation generates separate machine code:

```cantrip
let a = identity(42);       // Calls identity_i32
let b = identity(3.14);     // Calls identity_f64
let c = identity("hello");  // Calls identity_str
```

**Trade-off:**
- **Benefit**: Zero runtime overhead, maximum performance
- **Cost**: Increased binary size (code bloat for many instantiations)

### 11.5 Advanced Features

#### 11.5.1 Const Generics

**Purpose:** Const generics allow types and functions to be parameterized by compile-time constants (e.g., array lengths, matrix dimensions).

**Syntax:**
```ebnf
ConstParam     ::= "const" Ident ":" ConstType
ConstType      ::= "usize" | "u8" | "u16" | "u32" | "u64" | "isize" | "i8" | "i16" | "i32" | "i64" | "bool"
ConstArg       ::= IntegerLiteral | BooleanLiteral | ConstExpr
ConstExpr      ::= literal | arithmetic | sizeof(T) | alignof(T)
```

**Type rules:**
```
[T-Const-Generic]
Γ, N : const usize ⊢ e : τ
───────────────────────────────────────────
Γ ⊢ function f<const N: usize>(...) : τ

[T-Const-Inst]
Γ ⊢ n : usize      n is a compile-time constant
Γ ⊢ f : ∀(const N). τ
───────────────────────────────────────────
Γ ⊢ f<n> : τ[N ↦ n]
```

**Examples:**
```cantrip
// Function parameterized by a const length N
function process<T, const N: usize>(data: [T; N]): T {
    data[0]
}

// Type aliases with const parameters
type Matrix<T, const ROWS: usize, const COLS: usize> = [[T; COLS]; ROWS];

// Records with const parameters
record RingBuffer<T, const CAP: usize> {
    data: [T; CAP];
    head: usize;
    tail: usize;

    procedure new(): own RingBuffer<T, CAP>
        must CAP > 0;
    {
        own RingBuffer {
            data: [T::default(); CAP],
            head: 0,
            tail: 0,
        }
    }

    procedure capacity(self: RingBuffer<T, CAP>): usize {
        CAP
    }
}
```

**Const expression evaluation:**

Const arguments must be evaluable at compile time:
```cantrip
const SIZE: usize = 10;

// OK: const value
let buf: RingBuffer<i32, SIZE>;

// OK: literal
let buf2: RingBuffer<i32, 100>;

// OK: const arithmetic
const DOUBLED: usize = SIZE * 2;
let buf3: RingBuffer<i32, DOUBLED>;

// ERROR E9310: not a compile-time constant
let n = read_input();
let buf4: RingBuffer<i32, n>;  // ERROR
```

**Diagnostics:**
- `E9310` — Non-constant used where const generic argument required
- `E9311` — Const expression not evaluable at compile time

#### 11.5.2 Higher-Rank Types (Not Supported)

Cantrip does not support higher-rank polymorphism (rank-2 or higher types):

```cantrip
// NOT SUPPORTED:
type F = function<T>(f: function<U>(U) -> U): T;
```

Generics are first-order only.

### 11.6 Examples and Patterns

#### 11.6.1 Generic Collections

**Vec implementation sketch:**
```cantrip
record Vec<T> {
    data: own [T];
    length: usize;
    capacity: usize;

    procedure new(): own Vec<T>
        uses alloc.heap;
    {
        own Vec {
            data: own [],
            length: 0,
            capacity: 0,
        }
    }

    procedure push(self: mut Vec<T>, value: T)
        uses alloc.heap;
    {
        if self.length == self.capacity {
            self.grow();
        }
        self.data[self.length] = value;
        self.length += 1;
    }

    procedure get(self: Vec<T>, index: usize): Option<T>
        where T: Copy
    {
        if index < self.length {
            Some(self.data[index])
        } else {
            None
        }
    }
}
```

#### 11.6.2 Generic Option and Result

**Option type:**
```cantrip
enum Option<T> {
    Some(T),
    None,

    procedure map<U>(self: Option<T>, f: map(T) -> U): Option<U> {
        match self {
            Option.Some(value) -> Option.Some(f(value)),
            Option.None -> Option.None,
        }
    }

    procedure unwrap_or(self: Option<T>, default: T): T {
        match self {
            Option.Some(value) -> value,
            Option.None -> default,
        }
    }
}
```

**Result type:**
```cantrip
enum Result<T, E> {
    Ok(T),
    Err(E),

    procedure map<U>(self: Result<T, E>, f: map(T) -> U): Result<U, E> {
        match self {
            Result.Ok(value) -> Result.Ok(f(value)),
            Result.Err(error) -> Result.Err(error),
        }
    }

    procedure and_then<U>(
        self: Result<T, E>,
        f: map(T) -> Result<U, E>
    ): Result<U, E> {
        match self {
            Result.Ok(value) -> f(value),
            Result.Err(error) -> Result.Err(error),
        }
    }
}
```

#### 11.6.3 Generic Algorithms

**Sorting with trait bounds:**
```cantrip
function bubble_sort<T>(arr: [mut T])
    where T: Comparable<T> + Copy
{
    let n = arr.len();
    for i in 0..n {
        for j in 0..(n - i - 1) {
            if arr[j].compare(arr[j + 1]) > 0 {
                let temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

let mut numbers = [64, 34, 25, 12, 22, 11, 90];
bubble_sort(numbers);
```

**Generic map function:**
```cantrip
function map<T, U>(items: [T], f: function(T): U): Vec<U>
    uses alloc.heap;
{
    let mut result = Vec.new();
    for item in items {
        result.push(f(item));
    }
    result
}

let numbers = [1, 2, 3, 4, 5];
let doubled = map(numbers, function(x) { x * 2 });
```

#### 11.6.4 Generic Builder Pattern

**Type-safe builder:**
```cantrip
record RequestBuilder<T> {
    url: String;
    method: HttpMethod;
    body: Option<T>;

    procedure new(url: String): own RequestBuilder<T> {
        own RequestBuilder {
            url,
            method: HttpMethod.GET,
            body: None,
        }
    }

    procedure method(self: mut RequestBuilder<T>, method: HttpMethod): mut RequestBuilder<T> {
        self.method = method;
        self
    }

    procedure body<U>(self: RequestBuilder<T>, body: U): RequestBuilder<U> {
        RequestBuilder {
            url: self.url,
            method: self.method,
            body: Some(body),
        }
    }

    procedure build(self: RequestBuilder<T>): Request<T>
        where T: Serialize
    {
        Request {
            url: self.url,
            method: self.method,
            body: self.body,
        }
    }
}
```

#### 11.6.5 Const Generic Arrays

**Fixed-size buffer:**
```cantrip
record FixedBuffer<T, const SIZE: usize> {
    data: [T; SIZE];
    len: usize;

    procedure new(): FixedBuffer<T, SIZE>
        where T: Default
    {
        FixedBuffer {
            data: [T::default(); SIZE],
            len: 0,
        }
    }

    procedure push(self: mut FixedBuffer<T, SIZE>, value: T): Result<(), Error>
        must self.len < SIZE;
    {
        if self.len >= SIZE {
            return Err(Error.new("Buffer full"));
        }
        self.data[self.len] = value;
        self.len += 1;
        Ok(())
    }

    procedure capacity(self: FixedBuffer<T, SIZE>): usize {
        SIZE
    }
}

// Usage:
let mut buffer: FixedBuffer<i32, 10> = FixedBuffer.new();
buffer.push(42);
assert(buffer.capacity() == 10);
```

---

**Previous**: [Traits](06_Traits.md) | **Next**: [Modals](08_Modals.md)
