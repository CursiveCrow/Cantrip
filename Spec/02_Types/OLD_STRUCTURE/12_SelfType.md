# Part II: Type System - §12. Self Type

**Section**: §12 | **Part**: Type System (Part II)
**Previous**: [Type Aliases](11_TypeAliases.md) | **Next**: TBD

---

**Definition 12.1 (Self Type):** The `Self` type is a first-class type alias that refers to the enclosing type. Within procedures defined in a record or modal type `T`, `Self` is equivalent to `T`. The `Self` type enables generic and abstract references to the implementing type without repetition.

## 12. Self Type

### 12.1 Overview

**Key innovation/purpose:** The `Self` type provides a first-class way to refer to "the type being implemented" without repeating complex type names. It reduces verbosity, improves maintainability, and enables cleaner generic implementations. The `$` parameter syntax provides syntactic sugar for `self: Self` with permission modifiers.

**When to use Self:**
- Constructor procedures returning the implementing type
- Method parameters that accept the same type
- Associated type bounds and constraints
- Generic type definitions where the type name is verbose
- Any procedure signature within a record or modal definition
- Contract implementations with type-dependent signatures

**When NOT to use Self:**
- Outside of type definitions (records, modals, contracts) → no enclosing type context
- When you need a different related type → use explicit type
- In static procedures without `self` parameter → explicit type is clearer

**Relationship to other features:**
- **Records (§4):** `Self` is valid within procedure definitions in records
- **Generics (§7):** `Self` carries generic parameters from the type definition
- **Procedures (§10.6):** The `$` syntax desugars to `self: Permission Self`
- **Permissions (Part III):** `Self` can be used with any permission (`own Self`, `mut Self`)
- **Contracts (Part IV):** `Self` in contracts refers to the implementing type

### 12.2 Syntax

#### 12.2.1 Concrete Syntax

**Self type syntax:**
```ebnf
SelfType ::= "Self"
```

**Self parameter syntax (shorthand):**
```ebnf
SelfParam ::= Permission? "$"

Permission ::= "own" | "mut" | ε
```

**Examples:**
```cantrip
record Counter {
    value: i32

    // Self in return type
    procedure new(initial: i32): own Self {
        own Self { value: initial }
    }

    // Self parameter with $ syntax
    procedure get($): i32 {
        $.value
    }

    procedure increment(mut $) {
        $.value += 1
    }

    procedure consume(own $): i32 {
        $.value
    }

    // Self in parameter type
    procedure add($, other: Self): i32 {
        $.value + other.value
    }
}
```

**Desugaring:**
```cantrip
// $ desugars to self: Self with appropriate permission

procedure get($): i32
// desugars to:
procedure get(self: Self): i32

procedure increment(mut $)
// desugars to:
procedure increment(self: mut Self)

procedure consume(own $): i32
// desugars to:
procedure consume(self: own Self): i32
```

**Explicit type override:**
```cantrip
record Counter {
    value: i32

    // Override Self type when needed
    procedure from_immut($: Counter): i32 {
        $.value
    }

    procedure from_ptr($: Ptr<Counter>@Exclusive): i32
        uses read<Counter>
    {
        $.read().value
    }
}
```

#### 12.2.2 Abstract Syntax

**Type representation:**
```
τ ::= Self                           (self type reference)
    | ...

Within context record T { ... }:
  Self ≡ T

Within context record T<α₁, ..., αₙ> { ... }:
  Self ≡ T<α₁, ..., αₙ>
```

**Parameter representation:**
```
param ::= self : π Self              (explicit self parameter)
        | π $                        (self parameter shorthand)

where π ∈ {own, mut, [default]} is permission

Desugaring:
  $        ≡ self : Self
  mut $    ≡ self : mut Self
  own $    ≡ self : own Self
```

**Scope rules:**
```
Self is valid in context Γ iff:
  - Γ contains a type definition: record T { ... } or modal T { ... }
  - Γ contains a trait definition: trait T { ... }
  - Γ contains a contract definition: contract T { ... }

Outside these contexts, Self is undefined.
```

#### 12.2.3 Basic Examples

**Constructor pattern:**
```cantrip
record Counter {
    value: i32

    procedure new(initial: i32): own Self {
        own Self { value: initial }
    }
}

// Equivalent to:
record Counter {
    value: i32

    procedure new(initial: i32): own Counter {
        own Counter { value: initial }
    }
}
```

**Method with self parameter:**
```cantrip
record Counter {
    value: i32

    procedure get($): i32 {
        $.value
    }
}

// Equivalent to:
record Counter {
    value: i32

    procedure get(self: Counter): i32 {
        self.value
    }
}
```

**Generic implementation:**
```cantrip
record Stack<T> {
    items: Vec<T>

    procedure new(): own Self {
        // Self ≡ Stack<T>
        own Self { items: Vec.new() }
    }

    procedure push(mut $, item: T) {
        // $ : mut Self ≡ mut Stack<T>
        $.items.push(item)
    }
}
```

### 12.3 Static Semantics

#### 12.3.1 Well-Formedness Rules

**[WF-Self] Self type in type definition:**
```
[WF-Self]
record T { ... } in Γ  (or modal T { ... })
Γ ⊢ T : Type
─────────────────────────────────────────────────────
Γ ⊢ Self : Type    (where Self ≡ T)
```

**Explanation:** Within a record or modal definition for type T, `Self` is a well-formed type equivalent to T.

**[WF-Self-Generic] Self type in generic type definition:**
```
[WF-Self-Generic]
record T<α₁, ..., αₙ> { ... } in Γ
Γ, α₁ : Type, ..., αₙ : Type ⊢ T<α₁, ..., αₙ> : Type
─────────────────────────────────────────────────────
Γ ⊢ Self : Type    (where Self ≡ T<α₁, ..., αₙ>)
```

**Explanation:** In generic type definitions, `Self` includes all type parameters.

**[WF-Self-Invalid] Self outside type definition context:**
```
[WF-Self-Invalid]
no record, modal, trait, or contract definition in Γ
─────────────────────────────────────────────────────
Γ ⊬ Self : Type    (ERROR)
```

**Explanation:** `Self` is only valid within type definitions (records, modals, traits, contracts).

**[WF-SelfParam] Self parameter shorthand:**
```
[WF-SelfParam]
record T { procedure m(π $, ...) { ... } } in Γ
π ∈ {own, mut, [default]}
Γ ⊢ Self : Type    (where Self ≡ T)
─────────────────────────────────────────────────────
π $ ≡ self : π Self
Γ, self : π T ⊢ procedure m well-formed
```

**Explanation:** The `$` parameter desugars to `self: Permission Self`.

#### 12.3.2 Type Rules

**[T-Self] Self type equivalence:**
```
[T-Self]
record T { ... } in Γ  (or modal T { ... })
Γ ⊢ T : Type
─────────────────────────────────────────────────────
Γ ⊢ Self ≡ T
```

**Explanation:** `Self` is definitionally equal to the enclosing type.

**[T-SelfAccess] Self parameter access:**
```
[T-SelfAccess]
record T { procedure m(π $, ...) { ... body ... } }
Γ, self : π T ⊢ body : U
$ appears in body
─────────────────────────────────────────────────────
Γ ⊢ $ : π T    (where $ denotes self)
```

**Explanation:** Within method body, `$` refers to the self parameter with appropriate permission.

**[T-SelfField] Field access through $:**
```
[T-SelfField]
record T { procedure m($, ...) { ... $.field ... } }
Γ ⊢ T has field field : U
─────────────────────────────────────────────────────
Γ ⊢ $.field : U
```

**Explanation:** Field access through `$` works like any self parameter.

**[T-SelfReturn] Returning Self:**
```
[T-SelfReturn]
record T { procedure m(...): Self { ... return e ... } }
Γ ⊢ e : T
─────────────────────────────────────────────────────
Γ ⊢ return e : Self    (valid)
```

**Explanation:** Expressions of type T can be returned where Self is expected.

**[T-SelfParam-Permission] Permission on self parameter:**
```
[T-SelfParam-Permission]
record T { procedure m(π $, ...) { ... } }
π ∈ {own, mut, [default]}
─────────────────────────────────────────────────────
Γ ⊢ $ : π T
```

**Explanation:** Permission modifiers on `$` determine self's permission.

#### 12.3.3 Type Properties

**Theorem 12.1 (Self Type Soundness):**

Within a type definition for type T, all uses of `Self` are equivalent to T and maintain type safety.

**Proof sketch:** By definition, `Self` is a syntactic alias for T within the type definition scope. All type checking proceeds as if T were written directly.

**Theorem 12.2 (Self Parameter Equivalence):**

The `$` parameter syntax is exactly equivalent to explicit `self: Permission Self` syntax.

```
$ ≡ self : Self
mut $ ≡ self : mut Self
own $ ≡ self : own Self
```

**Proof sketch:** By desugaring definition. The compiler transforms `$` to the explicit form before type checking.

**Theorem 12.3 (Self Scope Restriction):**

`Self` is only valid within type definitions (records, modals, traits, contracts). Use outside these contexts is a compile-time error.

**Proof sketch:** The type checker maintains context information about the current type definition. `Self` resolution fails if no type definition context exists in the environment.

**Property 12.1 (Generic Preservation):**

In generic type definitions, `Self` carries all type parameters:
```
record Pair<T, U> { ... }
  Self ≡ Pair<T, U>
```

**Property 12.2 (Permission Independence):**

`Self` can be used with any permission:
```cantrip
own Self    // Owned self type
mut Self    // Mutable self type
Self        // Immutable self type (default)
```

### 12.4 Dynamic Semantics

#### 12.4.1 Evaluation Rules

**[E-SelfType] Self type resolution:**
```
[E-SelfType]
record T { ... } at compile time
Self appears in expression or type
─────────────────────────────────────────────────────
Self ⇝ T    (compile-time substitution)
```

**Explanation:** `Self` is resolved to the concrete type at compile time. No runtime representation.

**[E-SelfParam] Self parameter evaluation:**
```
[E-SelfParam]
record T { procedure m($, ...) { body } }
obj : T
obj.m(args)
─────────────────────────────────────────────────────
⟨body[$ ↦ obj], σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** The `$` parameter is bound to the receiver object at call time.

**[E-SelfField] Field access through $:**
```
[E-SelfField]
record T { procedure m($, ...) { ... $.field ... } }
obj : T with field : v
⟨$.field, σ[self ↦ obj]⟩
─────────────────────────────────────────────────────
⟨$.field, σ⟩ ⇓ ⟨v, σ⟩
```

**Explanation:** Field access through `$` evaluates to the field value of the self object.

**[E-SelfConstruct] Constructing Self:**
```
[E-SelfConstruct]
record T { procedure new(...): own Self { own Self { fields } } }
⟨own Self { fields }, σ⟩
─────────────────────────────────────────────────────
⟨own T { fields }, σ⟩ ⇓ ⟨T_value, σ'⟩
```

**Explanation:** Constructing `Self` evaluates identically to constructing the concrete type.

#### 12.4.2 Memory Representation

**Compile-time only:**

`Self` has **no runtime representation**. It is purely a compile-time alias.

```
Compile-time:
  Self (within record T)

Compile-time substitution:
  Self ⇝ T

Runtime:
  (nothing - Self is erased)
```

**Self parameter representation:**

The `$` parameter compiles to the same representation as any other self parameter:

```cantrip
// Source
record Counter {
    value: i32
    procedure get($): i32 { $.value }
}

// After desugaring
record Counter {
    value: i32
    procedure get(self: Counter): i32 { self.value }
}

// Compiled (pseudo-assembly)
Counter_get:
    mov rdi, [self_ptr]    ; self parameter in first register
    mov eax, [rdi]         ; load value field
    ret
```

**Memory layout (identical to explicit self):**
```
Stack frame for method call:
┌─────────────────────────────┐
│ Return address              │
├─────────────────────────────┤
│ self pointer (rdi/rcx)      │  ← $ points here
├─────────────────────────────┤
│ Other parameters            │
├─────────────────────────────┤
│ Local variables             │
└─────────────────────────────┘
```

#### 12.4.3 Operational Behavior

**Self type usage:**

```cantrip
record Vec<T> {
    data: *T
    len: usize
    cap: usize

    // Self ≡ Vec<T>
    procedure new(): own Self {
        own Self { data: null(), len: 0, cap: 0 }
    }

    procedure clone($): own Self {
        // $ : Vec<T>
        // return : own Vec<T>
        own Self {
            data: $.data.clone(),
            len: $.len,
            cap: $.cap,
        }
    }
}

// Usage
let v1 = Vec::<i32>.new()  // Returns own Vec<i32>
let v2 = v1.clone()         // Returns own Vec<i32>
```

**Evaluation trace:**
```
v1.clone()
⇝ Vec<i32>::clone(v1)
⇝ { $ ↦ v1, Self ↦ Vec<i32> }
⇝ own Vec<i32> { data: v1.data.clone(), len: v1.len, cap: v1.cap }
⇝ Vec<i32> { data: [...], len: 5, cap: 8 }
```

### 12.5 Examples and Patterns

#### 12.5.1 Builder Pattern

```cantrip
record RequestBuilder {
    url: String
    method: String
    headers: Vec<(String, String)>

    procedure new(url: String): own Self {
        own Self {
            url,
            method: String.from("GET"),
            headers: Vec.new(),
        }
    }

    procedure method(mut $, m: String): mut Self {
        $.method = m
        mut $  // Return self for chaining
    }

    procedure header(mut $, key: String, value: String): mut Self {
        $.headers.push((key, value))
        mut $
    }

    procedure build(own $): Request {
        Request {
            url: $.url,
            method: $.method,
            headers: $.headers,
        }
    }
}

// Usage
let request = RequestBuilder.new("https://api.example.com")
    .method("POST")
    .header("Content-Type", "application/json")
    .build()
```

#### 12.5.2 Generic Container

```cantrip
record Stack<T> {
    items: Vec<T>

    procedure new(): own Self {
        own Self { items: Vec.new() }
    }

    procedure push(mut $, item: T) {
        $.items.push(item)
    }

    procedure pop(mut $): Option<T> {
        $.items.pop()
    }

    procedure peek($): Option<T>
        must T: Copy
    {
        $.items.last()
    }

    procedure from_vec(items: Vec<T>): own Self {
        own Self { items }
    }

    procedure merge(mut $, other: own Self) {
        $.items.extend(other.items)
    }
}
```

#### 12.5.3 Trait Implementation

```cantrip
contract Clone {
    procedure clone($): own Self
}

record Vec<T>: Clone where T: Clone {
    data: *T
    len: usize
    cap: usize

    procedure clone($): own Self {
        var cloned = Vec.with_capacity($.len())
        for item in $ {
            cloned.push(item.clone())
        }
        own cloned
    }
}
```

#### 12.5.4 Associated Constructor

```cantrip
record Counter {
    value: i32

    procedure zero(): own Self {
        own Self { value: 0 }
    }

    procedure from_value(n: i32): own Self {
        own Self { value: n }
    }

    procedure max(): own Self {
        own Self { value: i32.MAX }
    }
}

// Usage
let c1 = Counter.zero()
let c2 = Counter.from_value(42)
let c3 = Counter.max()
```

---

**Previous**: [Type Aliases](11_TypeAliases.md) | **Next**: TBD
