# Chapter 2: Type System - §2.8. Map Types

**Section**: §2.8 | **Part**: Type System (Part II)
**Previous**: [Generics](07_Generics.md) | **Next**: [Type Aliases](../PART_E_Utilities/09_TypeAliases.md)

---

**Definition 10.1 (Map Type):** A map type `map(T₁, ..., Tₙ) → U` represents a callable function value with a known signature. Formally, it is the type of functions mapping n-tuples of argument types to a return type: `⟦map(T₁, ..., Tₙ) → U⟧ ≅ ⟦T₁ × ... × Tₙ⟧ → ⟦U⟧`. Map types are first-class values with static dispatch and zero runtime overhead.

## 2.8. Map Types

### 2.8.1 Overview

**Key innovation/purpose:** Map types provide first-class functions with static dispatch, enabling higher-order programming without dynamic allocation or runtime overhead. Cantrip distinguishes between **procedures** (what you declare), **functions** (values you can pass around), and **map types** (the type system representation).

**Terminology clarification:**

The distinction between procedures and functions is fundamental:

- **Procedure**: A declaration made with the `procedure` keyword. This is compile-time construct that defines named, executable code. Procedures exist at the **declaration site**.

- **Function**: A first-class value of map type. When you reference a procedure name without calling it, you get a function value that can be passed as an argument, stored in variables, or returned from other procedures. Functions exist at the **use site**.

- **Map type**: The type `map(T₁, ..., Tₙ) → U` describing the signature of a function value. This specifies the parameter types and return type.

- **Closure**: An anonymous function expression with environment capture (see §10.5).

- **Method**: A procedure with explicit `self` parameter using `$` syntax (see §10.6).

**Key insight:** One procedure declaration can produce many function values:

```cantrip
procedure increment(x: i32): i32 { x + 1 }  // ONE procedure declaration

let f = increment   // f binds to the function value
let g = increment   // g binds to the same function value (permission)
let h = increment   // h binds to the same function value

// All three (f, g, h) have type: map(i32) -> i32
// All three reference the same procedure code
```

Think of it this way:

- **Procedures** are like blueprints (what you declare)
- **Functions** are like instances (what you use)
- **Map types** are like type signatures (how you describe them)

**When to use map types:**

- Higher-order programming (map, filter, fold)
- Callback parameters
- Strategy pattern implementations
- Configurable behavior with explicit function types
- Function composition and pipelining
- Zero-cost abstraction over callable code

**When NOT to use map types:**

- Need to capture environment variables → use closures (§2.8.5)
- Need different function implementations → use enums with match
- Single-use inline code → use closure expressions directly

**Relationship to other features:**

- **Closures (§2.8.5):** Map types are the simplest callable form; closures extend them with environment capture
- **Methods (§2.8.6):** Procedures with explicit `self` parameter (using `$` syntax)
- **Enums (§5):** For different callable implementations, use enums wrapping map types
- **Raw Pointers (§9):** Map types are DISTINCT from code pointers; they live in separate type space
- **Effects (Part V):** Map types can carry effect signatures
- **Permissions (Part III):** Functions themselves have permission annotations
- **Self Type (§12):** Methods use `$` for self parameter with `Self` type

### 2.8.2 Syntax

#### 2.8.2.1 Concrete Syntax

**Map type syntax:**

```ebnf
MapType ::= "map" "(" ParamTypes? ")" "->" ReturnType

ParamTypes ::= Type ("," Type)*

ReturnType ::= Type
```

**Procedure declaration syntax:**

```ebnf
ProcedureDecl ::= "procedure" Ident GenericParams? "(" Params? ")" (":" ReturnType)? Block

Params ::= Param ("," Param)*
Param ::= Ident ":" Type
      | Permission? "$"                    // Self parameter shorthand

Permission ::= "own" | "mut"
```

**Function value creation:**

```ebnf
FunctionValue ::= Ident                    // Named procedure
                | ClosureExpr              // Anonymous function
```

**Examples:**

```cantrip
// Procedure declarations
procedure increment(x: i32): i32 { x + 1 }
procedure validate(s: String): bool { s.len() > 0 }
procedure action() { std.io.println("done"); }

// Map type annotations
let add: map(i32, i32) -> i32 = ...
let predicate: map(String) -> bool = ...
let callback: map() -> () = ...

// Named procedure as function value
procedure increment(x: i32): i32 { x + 1 }
let f: map(i32) -> i32 = increment

// Function values cannot be null (unlike raw pointers)
// let null_fn: map(i32) -> i32 = null  // ERROR: No null for map types
```

**Map types with effects:**

```ebnf
MapTypeWithEffects ::= "map" "(" ParamTypes? ")" "->" ReturnType "!" Effects

Effects ::= "{" Effect ("," Effect)* "}"
```

**Examples with effects:**

```cantrip
// Function that performs I/O
let logger: map(String) -> () ! {io.write} = log_message

// Function that allocates
let builder: map(i32) -> String ! {alloc.heap} = int_to_string

// Pure function (no effects)
let pure_math: map(f64, f64) -> f64 = multiply
```

**Method syntax (procedure with self):**

```cantrip
record Counter {
    value: i32

    // Immutable self
    procedure get($): i32 {
        $.value
    }

    // Mutable self
    procedure increment(mut $) {
        $.value += 1
    }

    // Owned self
    procedure consume(own $): i32 {
        $.value
    }
}
```

#### 2.8.2.2 Abstract Syntax

**Type representation:**

```
τ ::= map(τ₁, ..., τₙ) → τ                 (map type)
    | map(τ₁, ..., τₙ) → τ ! ε              (map type with effects)
    | π map(τ₁, ..., τₙ) → τ                (map type with permission)

where:
  τ₁, ..., τₙ are parameter types (contravariant)
  τ is return type (covariant)
  ε is effect set
  π ∈ {own, mut, [default]} is permission
```

**Declaration vs Value (Critical Distinction):**

Procedures exist in the **declaration space**:

```
decl ::= procedure f(x₁: τ₁, ..., xₙ: τₙ): τ { e }    (procedure declaration)
```

Functions exist in the **value space**:

```
value ::= f                                             (procedure name referenced as value)
        | |x₁, ..., xₙ| e                              (closure expression)
```

**The relationship:**

1. You **declare** a procedure with `procedure f(...) { ... }`
2. You **obtain** a function value by referencing `f` without calling it
3. The function value has type `map(T₁, ..., Tₙ) → U` matching the procedure's signature

**Value set (denotational semantics):**

```
⟦map(T₁, ..., Tₙ) → U⟧ = { v | v represents code at address addr,
                           code has signature (T₁, ..., Tₙ) → U,
                           v ≠ null }
```

**Key properties:**

- **Code address:** Function values reference executable code in text segment (NOT heap/stack data)
- **Signature:** Full type information captured in map type (arity, parameter types, return type)
- **Non-null:** Map types cannot be null (unlike raw pointers)
- **Static dispatch:** Call target resolved at compile time through monomorphization
- **Zero-size:** Function values are zero-sized at runtime (compile to direct calls)
- **Copy semantics:** Function values are Copy (no environment capture)
- **Declaration/value split:** One procedure declaration → many function values (all referencing same code)

#### 2.8.2.3 Basic Examples

**Example 1: Declaration Site vs Use Site**

```cantrip
// DECLARATION SITE: Define procedure
procedure add_one(x: i32): i32 { x + 1 }

// USE SITE: Reference procedure as function value
let f: map(i32) -> i32 = add_one   // 'add_one' becomes a function value
let g = add_one                     // Type inference: g has type map(i32) -> i32

// You can bind multiple names to the same function value
let h = add_one   // Another binding to the same function value

// All three can be called
f(5)   // 6
g(5)   // 6
h(5)   // 6

// Direct procedure call (not through function value)
add_one(5)   // 6
```

**Example 2: Multiple Procedures**

```cantrip
// Declare two procedures
procedure add_one(x: i32): i32 { x + 1 }
procedure multiply(x: i32, y: i32): i32 { x * y }

// Create function values
let f1: map(i32) -> i32 = add_one
let f2: map(i32, i32) -> i32 = multiply

// Call through function values
let result1 = f1(42)        // 43
let result2 = f2(6, 7)      // 42
```

**Example 3: Higher-Order Procedures (procedures that accept function values)**

```cantrip
// Procedure that takes a FUNCTION VALUE as parameter
procedure apply(f: map(i32) -> i32, x: i32): i32 {
    f(x)   // Call the function value
}

procedure twice(f: map(i32) -> i32, x: i32): i32 {
    f(f(x))   // Call the function value twice
}

// Pass 'add_one' procedure as a function value
let result1 = apply(add_one, 5)      // 6
let result2 = twice(add_one, 5)      // 7
```

**Example 4: Returning Function Values**

```cantrip
// Procedure that RETURNS a function value
procedure compose<A, B, C>(
    f: map(B) -> C,
    g: map(A) -> B
): map(A) -> C {
    |x| f(g(x))  // Returns closure (a function value)
}
```

**Example 5: What You CANNOT Do**

```cantrip
// ❌ Cannot call a map TYPE (types aren't callable)
let t: map(i32) -> i32 = ...
// map(i32) -> i32(42)  // ERROR: Types aren't values

// ❌ Cannot have null function values
// let null_fn: map(i32) -> i32 = null  // ERROR: No null for map types

// ❌ Cannot modify a procedure declaration
// add_one = something_else  // ERROR: Procedures aren't variables
```

### 2.8.3 Static Semantics

#### 2.8.3.1 Well-Formedness Rules

**Definition 10.2 (Well-Formed Map Type):** A map type is well-formed if all parameter types and the return type are well-formed in the given context.

**[WF-MapType] Map type well-formedness:**

```
[WF-MapType]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
─────────────────────────────────────────────────────
Γ ⊢ map(T₁, ..., Tₙ) → U : Type
```

**Explanation:** A map type is well-formed if all component types are well-formed.

**[WF-MapType-Effects] Map type with effects:**

```
[WF-MapType-Effects]
Γ ⊢ map(T₁, ..., Tₙ) → U : Type
ε ⊆ EffectSet
─────────────────────────────────────────────────────
Γ ⊢ map(T₁, ..., Tₙ) → U ! ε : Type
```

**Explanation:** Effects must be a valid subset of the available effect system.

**[WF-MapType-Recursive] Recursive map types:**

```
[WF-MapType-Recursive]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
Γ ⊢ U : Type
map(T₁, ..., Tₙ) → U does NOT appear negatively in U
─────────────────────────────────────────────────────
Γ ⊢ map(T₁, ..., Tₙ) → U : Type
```

**Explanation:** Map types can appear recursively only in positive positions (return types, not as function parameters) to ensure well-foundedness.

**[WF-MapType-Permission] Map type with permission:**

```
[WF-MapType-Permission]
Γ ⊢ map(T₁, ..., Tₙ) → U : Type
π ∈ {own, mut, [default]}
─────────────────────────────────────────────────────
Γ ⊢ π map(T₁, ..., Tₙ) → U : Type
```

**Explanation:** Permissions apply to the function value itself, not to its behavior.

#### 2.8.3.2 Type Rules

##### Procedure Declaration Rules

**[T-Procedure] Procedure declaration:**

```
[T-Procedure]
Γ, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
─────────────────────────────────────────────────────
Γ ⊢ procedure f(x₁: T₁, ..., xₙ: Tₙ): U { e } well-typed
Γ, f : map(T₁, ..., Tₙ) → U
```

**Explanation:** A procedure declaration introduces a new procedure with the corresponding map type.

**[T-ProcedureAsFunction] Procedure name as function value:**

```
[T-ProcedureAsFunction]
procedure f(x₁: T₁, ..., xₙ: Tₙ): U { e } declared in Γ
─────────────────────────────────────────────────────
Γ ⊢ f : map(T₁, ..., Tₙ) → U
```

**Explanation:** When a procedure name appears in expression position (not being called), it is treated as a function value. The procedure's signature `(T₁, ..., Tₙ) → U` becomes the function's map type `map(T₁, ..., Tₙ) → U`.

**Key insight:** This rule bridges the declaration space and value space:

- **Input:** A procedure declaration (compile-time construct)
- **Output:** A function value (runtime construct with map type)

**Example:**

```cantrip
// DECLARATION SPACE: Procedure declaration
procedure square(x: i32): i32 { x * x }

// VALUE SPACE: Function value derived from procedure
let f: map(i32) -> i32 = square  // Type: map(i32) -> i32

// The rule allows 'square' (a procedure name) to appear as a value expression
```

**Distinction:**

```cantrip
square(5)   // Direct call: procedure invocation (not using this rule)
square      // Value reference: becomes function value (uses this rule)
```

##### Function Call Rules

**[T-FunctionCall] Function call through function value:**

```
[T-FunctionCall]
Γ ⊢ f : map(T₁, ..., Tₙ) → U
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
─────────────────────────────────────────────────────
Γ ⊢ f(e₁, ..., eₙ) : U
```

**Explanation:** Calling a function value with arguments of the correct types produces a value of the return type.

**Example:**

```cantrip
let f: map(i32, i32) -> i32 = add
let result: i32 = f(10, 20)  // Type: i32
```

**[T-FunctionCall-Effects] Function call with effects:**

```
[T-FunctionCall-Effects]
Γ ⊢ f : map(T₁, ..., Tₙ) → U ! ε
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
ε ⊆ available_effects(Γ)
─────────────────────────────────────────────────────
Γ ⊢ f(e₁, ..., eₙ) : U ! ε
```

**Explanation:** Calling a function with effects must those effects to be available in the calling context.

**Example:**

```cantrip
procedure process_data(f: map(String) -> () ! {io.write}, msg: String)
    uses io.write
{
    f(msg)  // OK: io.write available
}
```

##### Higher-Order Rules

**[T-HigherOrder] Higher-order procedure parameter:**

```
[T-HigherOrder]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type    Γ ⊢ U : Type
Γ, f : map(T₁, ..., Tₙ) → U ⊢ e : V
─────────────────────────────────────────────────────
Γ ⊢ procedure g(f: map(T₁, ..., Tₙ) → U, ...): V { e } well-typed
```

**Explanation:** Procedures can accept functions as parameters (higher-order procedures).

**Example:**

```cantrip
procedure map<T, U>(transform: map(T) -> U, items: [T]): Vec<U>
    uses alloc.heap
{
    var result = Vec.new()
    for item in items {
        result.push(transform(item))
    }
    result
}
```

**[T-ReturnFunction] Procedure returning function:**

```
[T-ReturnFunction]
Γ ⊢ e : map(T₁, ..., Tₙ) → U
─────────────────────────────────────────────────────
Γ ⊢ procedure g(...): map(T₁, ..., Tₙ) → U { ...; e } well-typed
```

**Explanation:** Procedures can return functions (currying, partial application).

**Example:**

```cantrip
procedure make_adder(x: i32): map(i32) -> i32 {
    |y| x + y  // Returns closure
}
```

##### Subtyping Rules

**[Sub-MapType] Map type subtyping (contravariant/covariant):**

```
[Sub-MapType]
T₂ <: T₁    ...    Tₙ' <: Tₙ    (contravariant in parameters)
U <: U'                         (covariant in return)
─────────────────────────────────────────────────────
map(T₁, ..., Tₙ) → U <: map(T₂, ..., Tₙ') → U'
```

**Explanation:** Map types are contravariant in parameters (can accept more general inputs) and covariant in return type (can return more specific outputs). This is the Liskov Substitution Principle for functions.

**Example:**

```cantrip
// If String <: Object and i32 <: f64:
// map(Object) -> i32 <: map(String) -> f64

procedure process_object(f: map(Object) -> i32, s: String) {
    let more_specific: map(String) -> f64 = ...
    // Can pass more_specific where map(Object) -> i32 expected
}
```

**[Sub-MapType-Effects] Effect subtyping:**

```
[Sub-MapType-Effects]
map(T₁, ..., Tₙ) → U <: map(T₁, ..., Tₙ) → U
ε₁ ⊆ ε₂
─────────────────────────────────────────────────────
map(T₁, ..., Tₙ) → U ! ε₁ <: map(T₁, ..., Tₙ) → U ! ε₂
```

**Explanation:** A function with fewer effects can substitute for one requiring more effects (contravariance on effects).

**[Sub-MapType-Never] Never type in return:**

```
[Sub-MapType-Never]
─────────────────────────────────────────────────────
map(T₁, ..., Tₙ) → ! <: map(T₁, ..., Tₙ) → U
```

**Explanation:** A function that never returns (returns `!`) can substitute for any function type with the same parameters.

##### Permission Integration Rules

**[T-MapType-Owned] Owned function value:**

```
[T-MapType-Owned]
Γ ⊢ f : map(T₁, ..., Tₙ) → U
─────────────────────────────────────────────────────
Γ ⊢ f : own map(T₁, ..., Tₙ) → U
```

**Explanation:** Function values can be owned (though they're typically zero-sized).

**[T-MapType-Move] Moving function values:**

```
[T-MapType-Move]
Γ ⊢ f : own map(T₁, ..., Tₙ) → U
Γ' = Γ \ {f}
─────────────────────────────────────────────────────
Γ ⊢ move f : own map(T₁, ..., Tₙ) → U
Γ' ⊬ f
```

**Explanation:** Function values can be moved, transferring ownership (though in practice they're often Copy).

**[T-MapType-Copy] Function values are Copy:**

```
[T-MapType-Copy]
─────────────────────────────────────────────────────
map(T₁, ..., Tₙ) → U : Copy
```

**Explanation:** Function values (without environment capture) are Copy because they're just code addresses.

##### Arity and Currying Rules

**[T-MapType-ZeroArity] Zero-argument functions:**

```
[T-MapType-ZeroArity]
Γ ⊢ U : Type
─────────────────────────────────────────────────────
Γ ⊢ map() → U : Type
```

**Explanation:** Functions can take zero arguments.

**Example:**

```cantrip
procedure get_random(): i32
    uses random
{
    rand.gen()
}

let f: map() -> i32 = get_random
```

**[T-MapType-Curry] Currying transformation:**

```
[T-MapType-Curry]
Γ ⊢ f : map(T₁, T₂) → U
─────────────────────────────────────────────────────
curry(f) : map(T₁) -> map(T₂) -> U
```

**Explanation:** Multi-parameter functions can be curried into nested single-parameter functions.

**Example:**

```cantrip
procedure curry<A, B, C>(f: map(A, B) -> C): map(A) -> map(B) -> C {
    |a| |b| f(a, b)
}
```

##### Generic Map Type Rules

**[T-MapType-Generic] Generic map types:**

```
[T-MapType-Generic]
Γ, α : Type ⊢ T₁ : Type    ...    Γ, α : Type ⊢ Tₙ : Type
Γ, α : Type ⊢ U : Type
─────────────────────────────────────────────────────
Γ ⊢ map<α>(T₁, ..., Tₙ) → U : Type
```

**Explanation:** Map types can be generic over type parameters.

**Example:**

```cantrip
procedure identity<T>(x: T): T { x }

let f: map<T>(T) -> T = identity
```

**[T-MapType-Generic-Call] Calling generic functions:**

```
[T-MapType-Generic-Call]
Γ ⊢ f : map<α>(T₁, ..., Tₙ) → U
Γ ⊢ V : Type
Γ ⊢ e₁ : T₁[α ↦ V]    ...    Γ ⊢ eₙ : Tₙ[α ↦ V]
─────────────────────────────────────────────────────
Γ ⊢ f<V>(e₁, ..., eₙ) : U[α ↦ V]
```

**Explanation:** Generic functions are instantiated with concrete types when called.

##### Effect Composition Rules

**[T-MapType-EffectComposition] Effect propagation:**

```
[T-MapType-EffectComposition]
Γ ⊢ f : map(T₁, ..., Tₙ) → U ! ε₁
Γ ⊢ g : map(U) → V ! ε₂
─────────────────────────────────────────────────────
Γ ⊢ (g ∘ f) : map(T₁, ..., Tₙ) → V ! (ε₁ ∪ ε₂)
```

**Explanation:** Composing functions with effects produces a function with the union of effects.

**[T-MapType-EffectRequired] Effect availability check:**

```
[T-MapType-EffectRequired]
Γ ⊢ f : map(T₁, ..., Tₙ) → U ! ε
ε ⊆ available_effects(Γ)
─────────────────────────────────────────────────────
Γ ⊢ f callable in context Γ
```

**Explanation:** A function can only be called if all its required effects are available.

##### Type Equivalence Rules

**[Equiv-MapType] Map type equivalence:**

```
[Equiv-MapType]
T₁ ≡ T₁'    ...    Tₙ ≡ Tₙ'    U ≡ U'
─────────────────────────────────────────────────────
map(T₁, ..., Tₙ) → U ≡ map(T₁', ..., Tₙ') → U'
```

**Explanation:** Map types are equivalent if their parameter types and return type are pairwise equivalent.

**[Equiv-MapType-Eta] Eta equivalence:**

```
[Equiv-MapType-Eta]
Γ ⊢ f : map(T₁, ..., Tₙ) → U
─────────────────────────────────────────────────────
f ≡ |x₁, ..., xₙ| f(x₁, ..., xₙ)
```

**Explanation:** A function is equivalent to a lambda that immediately calls it (eta equivalence).

#### 2.8.3.3 Type Properties

##### Safety Properties

**Theorem 10.1 (Type Safety):**

If `Γ ⊢ f : map(T₁, ..., Tₙ) → U` and `Γ ⊢ eᵢ : Tᵢ` for all `i`, then `⟨f(e₁, ..., eₙ), σ⟩` either:

1. Evaluates to a value of type `U`, or
2. Diverges (infinite loop or recursion), or
3. Panics (explicit failure)

**Proof sketch:** By Progress and Preservation lemmas. Well-typed function calls with well-typed arguments cannot get stuck (Progress). If the call reduces, the result maintains the expected type (Preservation).

**Corollary 10.1 (No Invalid Calls):**

Function calls with incorrect argument counts or types are rejected at compile time.

**Theorem 10.2 (Non-Nullability):**

Map types cannot be null. There is no value `null : map(T₁, ..., Tₙ) → U`.

**Proof sketch:** Map types inhabit a different value space than pointers. The type system provides no constructor for null function values, unlike raw pointers which have `null<T>()`.

**Theorem 10.3 (Static Dispatch):**

All map type calls are resolved at compile time with no runtime indirection.

**Proof sketch:** Map types are monomorphized during compilation. Each call site knows the exact target function, enabling direct call instructions without vtable lookup.

##### Subtyping Properties

**Theorem 10.4 (Contravariance in Parameters):**

If `T₂ <: T₁` then `map(T₁) → U <: map(T₂) → U`.

**Proof sketch:** A function accepting more general inputs (T₁) can safely substitute for one requiring more specific inputs (T₂), by Liskov Substitution Principle.

**Example:**

```cantrip
// String <: Object
// map(Object) -> bool <: map(String) -> bool

procedure accepts_string_predicate(f: map(String) -> bool, s: String): bool {
    f(s)
}

procedure general_predicate(obj: Object): bool { true }

// Can pass general_predicate where map(String) -> bool expected
accepts_string_predicate(general_predicate, "hello")  // OK
```

**Theorem 10.5 (Covariance in Return Type):**

If `U <: V` then `map(T₁, ..., Tₙ) → U <: map(T₁, ..., Tₙ) → V`.

**Proof sketch:** A function returning more specific outputs (U) can safely substitute for one returning more general outputs (V).

**Theorem 10.6 (Subtyping Transitivity):**

Map type subtyping is transitive: if `F <: G` and `G <: H`, then `F <: H`.

**Proof sketch:** Follows from transitivity of subtyping on parameter and return types.

##### Equivalence Properties

**Theorem 10.7 (Eta Equivalence):**

For any function `f : map(T₁, ..., Tₙ) → U`, we have:

```
f ≡ |x₁: T₁, ..., xₙ: Tₙ| f(x₁, ..., xₙ)
```

**Proof sketch:** Both expressions denote the same function value. Wrapping a function in a lambda that immediately calls it produces an equivalent function.

**Theorem 10.8 (Beta Equivalence):**

For any lambda `|x: T| e` and value `v : T`:

```
(|x: T| e)(v) ≡ e[x ↦ v]
```

**Proof sketch:** Function application is substitution. Calling a lambda with an argument replaces the parameter with the argument in the body.

##### Performance Properties

**Theorem 10.9 (Zero Runtime Overhead):**

Map types compile to direct function calls with no runtime indirection, vtable lookup, or allocation.

**Proof sketch:** Function values are zero-sized at runtime. All call targets are known at compile time through monomorphization, enabling direct call instructions.

**Property 10.1 (Memory Representation):**

Map types have zero size:

```
sizeof(map(T₁, ..., Tₙ) → U) = 0 bytes
```

**Rationale:** Function values are compile-time constructs that resolve to direct calls. No runtime storage is needed.

**Property 10.2 (Copy Semantics):**

Map types (without environment capture) are `Copy`:

```
∀ T₁, ..., Tₙ, U. map(T₁, ..., Tₙ) → U : Copy
```

**Proof sketch:** Function values are zero-sized, so copying them is trivial (no-op at runtime).

### 2.8.4 Dynamic Semantics

#### 2.8.4.1 Evaluation Rules

**[E-ProcedureValue] Procedure value evaluation:**

```
[E-ProcedureValue]
procedure f(...) { body } declared at address addr
─────────────────────────────────────────────────────
⟨f, σ⟩ ⇓ ⟨fn_val(addr), σ⟩
```

**Explanation:** A named procedure evaluates to a function value pointing to its code address. The store is unchanged because no allocation occurs.

**[E-FunctionCall] Function call evaluation:**

```
[E-FunctionCall]
⟨f, σ⟩ ⇓ ⟨fn_val(addr), σ₁⟩
⟨e₁, σ₁⟩ ⇓ ⟨v₁, σ₂⟩
...
⟨eₙ, σₙ⟩ ⇓ ⟨vₙ, σₙ₊₁⟩
⟨body[params ↦ v₁, ..., vₙ], σₙ₊₁⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────────
⟨f(e₁, ..., eₙ), σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** Function call evaluates the function expression, evaluates each argument left-to-right, then executes the function body with parameters substituted by argument values.

**[E-FunctionCall-Direct] Direct function call optimization:**

```
[E-FunctionCall-Direct]
procedure f(x₁, ..., xₙ) { body } known at compile time
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
⟨body[x₁ ↦ v₁, ..., xₙ ↦ vₙ], σₙ⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────────
⟨f(e₁, ..., eₙ), σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** When the function is known at compile time, the call can be directly compiled without indirection.

**[E-HigherOrder] Higher-order function call:**

```
[E-HigherOrder]
⟨f_arg, σ⟩ ⇓ ⟨fn_val(addr), σ₁⟩
⟨g, σ₁⟩ ⇓ ⟨fn_val(addr_g), σ₂⟩
⟨body_g[param ↦ fn_val(addr)], σ₂⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────────
⟨g(f_arg), σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** When passing a function as an argument, the function value is passed directly (zero-sized), then the receiving function can call it.

**[E-ReturnFunction] Returning function value:**

```
[E-ReturnFunction]
⟨e, σ⟩ ⇓ ⟨fn_val(addr), σ'⟩
─────────────────────────────────────────────────────
⟨return e, σ⟩ ⇓ ⟨fn_val(addr), σ'⟩
```

**Explanation:** Procedures can return function values. The returned value is the code address.

**[E-Compose] Function composition:**

```
[E-Compose]
⟨f, σ⟩ ⇓ ⟨fn_val(addr_f), σ₁⟩
⟨g, σ₁⟩ ⇓ ⟨fn_val(addr_g), σ₂⟩
compose_fn(addr_f, addr_g) = addr_composed
─────────────────────────────────────────────────────
⟨f ∘ g, σ⟩ ⇓ ⟨fn_val(addr_composed), σ₂⟩
```

**Explanation:** Function composition creates a new function that calls both functions in sequence.

**[E-PartialApply] Partial application:**

```
[E-PartialApply]
⟨f, σ⟩ ⇓ ⟨fn_val(addr), σ₁⟩
⟨e₁, σ₁⟩ ⇓ ⟨v₁, σ₂⟩
...
⟨eₖ, σₖ⟩ ⇓ ⟨vₖ, σₖ₊₁⟩
k < n
partial_apply(addr, v₁, ..., vₖ) = addr_partial
─────────────────────────────────────────────────────
⟨f(e₁, ..., eₖ), σ⟩ ⇓ ⟨fn_val(addr_partial), σₖ₊₁⟩
```

**Explanation:** Partially applying a function with fewer arguments than required creates a new function expecting the remaining arguments (requires closure conversion).

#### 2.8.4.2 Memory Representation

**Function value representation:**

Map types are **zero-sized** at runtime. They compile to direct calls with no runtime indirection.

```
Compile-time:
  map(T₁, ..., Tₙ) → U

Runtime representation:
┌─────────────────────────────┐
│ (nothing - zero bytes)      │
└─────────────────────────────┘
Size: 0 bytes
Alignment: 1 byte
```

**Code location:**

Procedures live in the **text segment** (executable code), not the data segment:

```
Memory Layout:
┌─────────────────────────────┐
│ Text Segment (Read-Execute) │
│ ┌─────────────────────────┐ │
│ │ procedure_1() { ... }   │ ← Code address
│ │ procedure_2() { ... }   │ ← Code address
│ │ procedure_3() { ... }   │ ← Code address
│ └─────────────────────────┘ │
└─────────────────────────────┘

┌─────────────────────────────┐
│ Data Segment (Read-Write)   │
│ (function values NOT here)  │
└─────────────────────────────┘
```

**Comparison with other callable types:**

```
Type                    Size (bytes)    Indirection    Allocation
─────────────────────────────────────────────────────────────────
map(i32) -> i32         0               None           None
Closure (no capture)    0               None           None
Closure (with capture)  varies          None           Possible
Raw fn pointer (*fn)    8               None           None (C FFI)
```

**Calling convention:**

Function calls use standard calling conventions (platform-dependent):

- **x86-64:** System V AMD64 ABI (Linux/Unix) or Microsoft x64 (Windows)
- **ARM64:** AAPCS64
- Parameters passed in registers first, then stack
- Return value in designated register

**Example compiled code:**

```cantrip
procedure add(x: i32, y: i32): i32 { x + y }

let f: map(i32, i32) -> i32 = add
let result = f(10, 20)
```

Compiles to (x86-64 assembly):

```asm
add:
    mov eax, edi        ; x in edi
    add eax, esi        ; y in esi
    ret                 ; result in eax

main:
    mov edi, 10         ; First argument
    mov esi, 20         ; Second argument
    call add            ; Direct call (no indirection!)
    ; result in eax
```

**Key insight:** No runtime representation for function values. All calls are direct (static dispatch).

#### 2.8.4.3 Operational Behavior

**Call-by-value semantics:**

Cantrip uses strict call-by-value evaluation:

1. Evaluate function expression to code address
2. Evaluate arguments left-to-right
3. Pass argument values to function
4. Execute function body
5. Return result value

**Example:**

```cantrip
procedure expensive_computation(): i32
    uses io.write
{
    std.io.println("Computing...")
    42
}

procedure use_twice(x: i32, y: i32): i32 {
    x + y
}

// Arguments evaluated exactly once before call
let result = use_twice(expensive_computation(), expensive_computation())
// Prints "Computing..." twice
```

**Evaluation order:**

Arguments are evaluated **left-to-right** before the function is called:

```cantrip
procedure side_effect(x: i32, msg: String): i32
    uses io.write
{
    std.io.println(msg)
    x
}

// Left-to-right evaluation
side_effect(
    side_effect(1, "first"),   // Executed first
    side_effect(2, "second")    // Executed second
)
// Output: "first" then "second"
```

**Tail call optimization:**

Cantrip implementations SHOULD optimize tail calls:

```cantrip
procedure factorial_tail(n: i32, acc: i32): i32 {
    if n <= 1 {
        acc
    } else {
        factorial_tail(n - 1, n * acc)  // Tail call - no stack growth
    }
}
```

**Compilation strategy:** Tail calls can be compiled to jumps instead of calls, preventing stack overflow in recursive functions.

**Function inlining:**

Small functions are typically inlined at call sites:

```cantrip
procedure add_one(x: i32): i32 { x + 1 }

let y = add_one(42)  // Compiled to: let y = 42 + 1
```

**Stack frame layout:**

Function calls create stack frames with:

```
┌─────────────────────────────┐
│ Return address              │
├─────────────────────────────┤
│ Saved registers             │
├─────────────────────────────┤
│ Local variables             │
├─────────────────────────────┤
│ Parameters (if stack-passed)│
└─────────────────────────────┘
```

**Performance characteristics:**

```
Operation                  Cost
──────────────────────────────────────────
Function value creation    0 (compile-time)
Function call (direct)     ~5 cycles (call + ret)
Function call (inline)     0 (inlined away)
Parameter passing          1-2 cycles per register
```

### 2.8.5 Closures

**Definition 10.3 (Closure):** A closure is an anonymous function with environment capture. Formally, a closure is a pair `⟨λ(x₁, ..., xₙ). e, ρ⟩` where `λ(x₁, ..., xₙ). e` is the function body and `ρ` is the captured environment.

#### 2.8.5.1 Closure Syntax

**Closure expression syntax:**

```ebnf
ClosureExpr ::= "|" ParamList? "|" Expr
              | "|" ParamList? "|" "->" Type Expr

ParamList ::= Param ("," Param)*
Param ::= Ident (":" Type)?
```

**Examples:**

```cantrip
// No parameters
let f = || 42

// With parameters (type inference)
let add = |x, y| x + y

// With explicit types
let multiply: map(i32, i32) -> i32 = |x: i32, y: i32| x * y

// With explicit return type
let divide = |x: i32, y: i32| -> f64 { (x as f64) / (y as f64) }

// Multi-line body
let complex = |x| {
    let temp = x * 2
    temp + 1
}
```

#### 2.8.5.2 Environment Capture

**Capture modes:**

Closures can capture variables from their environment in three ways:

1. **By immutable value** (default):

```cantrip
let x = 42
let f = || x + 1  // Captures x (Copy)
```

2. **By mutable capture** (if closure mutates):

```cantrip
var count = 0
let mut increment = || { count += 1; }  // Captures count mutably
```

3. **By move** (takes ownership):

```cantrip
let s = String.from("hello")
let f = move || s.len()  // Captures own String
// s no longer accessible
```

#### 2.8.5.3 Closure Types

Closures implement one of three traits based on their capture behavior:

**Fn trait (immutable access):**

```cantrip
trait Fn<Args> {
    type Output
    procedure call($, args: Args): Self.Output
}
```

Example:

```cantrip
let x = 10
let f = |y| x + y  // Closure capturing &x: inferred type <closure(i32) => i32>
```

**FnMut contract (mutable access):**

```cantrip
contract FnMut<Args> {
    type Output
    procedure call_mut(mut $, args: Args): Self.Output
}
```

Example:

```cantrip
var count = 0
let f = || { count += 1; count }  // Closure capturing count mutably
```

**FnOnce contract (consumes captured values):**

```cantrip
contract FnOnce<Args> {
    type Output
    procedure call_once(own $, args: Args): Self.Output
}
```

Example:

```cantrip
let s = String.from("data")
let f = move || s.len()  // Closure with move, consumes String
```

#### 2.8.5.4 Closure Type Rules

**[T-Closure-Simple] Closure without capture:**

```
[T-Closure-Simple]
Γ, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
no free variables in e
─────────────────────────────────────────────────────
Γ ⊢ |x₁: T₁, ..., xₙ: Tₙ| e : map(T₁, ..., Tₙ) → U
```

**Explanation:** Closures without environment capture have map type.

**[T-Closure-Capture-Immut] Closure with immutable capture:**

```
[T-Closure-Capture-Immut]
Γ, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
free_vars(e) = {y₁: S₁, ..., yₘ: Sₘ} ⊆ Γ
all yᵢ used immutably in e
─────────────────────────────────────────────────────
Γ ⊢ |x₁, ..., xₙ| e : <closure(T₁, ..., Tₙ) -> U> : Fn(T₁, ..., Tₙ) -> U
```

**Explanation:** Closures capturing immutable references have closure type implementing `Fn` contract.

**[T-Closure-Capture-Mut] Closure with mutable capture:**

```
[T-Closure-Capture-Mut]
Γ, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
free_vars(e) = {y₁: S₁, ..., yₘ: Sₘ} ⊆ Γ
some yᵢ used mutably in e
─────────────────────────────────────────────────────
Γ ⊢ |x₁, ..., xₙ| e : <closure(T₁, ..., Tₙ) -> U> : FnMut(T₁, ..., Tₙ) -> U
```

**Explanation:** Closures requiring mutable access to captured variables implement `FnMut` contract.

**[T-Closure-Move] Closure with move capture:**

```
[T-Closure-Move]
Γ, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
free_vars(e) = {y₁: S₁, ..., yₘ: Sₘ} ⊆ Γ
Γ' = Γ \ {y₁, ..., yₘ}
─────────────────────────────────────────────────────
Γ ⊢ move |x₁, ..., xₙ| e : <closure(T₁, ..., Tₙ) -> U> : FnOnce(T₁, ..., Tₙ) -> U
Γ' ⊬ y₁, ..., yₘ
```

**Explanation:** Move closures take ownership of captured variables, implementing `FnOnce` contract.

#### 2.8.5.5 Closure Memory Representation

**Without captures (zero-sized):**

```
Closure with no captures:
┌────────────────────┐
│ (empty - 0 bytes)  │
└────────────────────┘
```

**With captures:**

```
Closure with captures:
┌────────────────────────────┐
│ Captured variable 1        │
├────────────────────────────┤
│ Captured variable 2        │
├────────────────────────────┤
│ ...                        │
└────────────────────────────┘
Size: sum of captured variable sizes
Alignment: max alignment of captured variables
```

**Example:**

```cantrip
let x: i32 = 10
let y: f64 = 3.14
let closure = |z: i32| x + z + (y as i32)

// Closure representation:
// ┌──────────┬──────────┐
// │ x (4B)   │ y (8B)   │
// └──────────┴──────────┘
// Total: 16 bytes (8-byte aligned)
```

### 2.8.6 Methods (Procedures with Self)

**Definition 10.4 (Method):** A method is a procedure with an explicit self parameter, defined within a record or modal type definition. Methods use the `$` shorthand for `self: Permission Self`.

#### 2.8.6.1 Method Syntax

```cantrip
record TypeName {
    // fields...

    procedure method_name(Permission? $, params): ReturnType {
        body
    }
}
```

**Self parameter forms:**

- `$` — Immutable self (desugars to `self: Self`)
- `mut $` — Mutable self (desugars to `self: mut Self`)
- `own $` — Owned self (desugars to `self: own Self`)

**Examples:**

```cantrip
record Counter {
    value: i32

    // Constructor (no self parameter)
    procedure new(initial: i32): own Self {
        own Self { value: initial }
    }

    // Immutable method
    procedure get($): i32 {
        $.value
    }

    // Mutable method
    procedure increment(mut $) {
        $.value += 1
    }

    // Consuming method
    procedure consume(own $): i32 {
        $.value  // self destroyed after return
    }
}
```

#### 2.8.6.2 Method Type Rules

**[T-Method-Immut] Immutable method:**

```
[T-Method-Immut]
record T { procedure m($, x₁: T₁, ..., xₙ: Tₙ): U { e } }
Γ, self: T, x₁: T₁, ..., xₙ: Tₙ ⊢ e : U
─────────────────────────────────────────────────────
Γ ⊢ T::m : map(T, T₁, ..., Tₙ) → U
```

**[T-Method-Call] Method call:**

```
[T-Method-Call]
Γ ⊢ obj : T
Γ ⊢ T::m : map(T, T₁, ..., Tₙ) → U
Γ ⊢ e₁ : T₁    ...    Γ ⊢ eₙ : Tₙ
─────────────────────────────────────────────────────
Γ ⊢ obj.m(e₁, ..., eₙ) : U
```

**Explanation:** Method call syntax `obj.m(args)` desugars to `T::m(obj, args)`.

#### 2.8.6.3 UFCS (Universal Function Call Syntax)

Methods can be called using function syntax:

```cantrip
let counter = Counter.new(0)

// Method syntax
counter.increment()

// Function syntax (UFCS)
Counter::increment(counter)

// Both are equivalent
```

## 2.8.7 Related Sections

- See §2.1 for [Primitive Types](../PART_A_Primitives/01_PrimitiveTypes.md) - functions using primitive parameters and returns
- See §2.2 for [Product Types](../PART_B_Composite/02_ProductTypes.md) - functions with tuple/record parameters
- See §2.3 for [Sum Types](../PART_B_Composite/03_SumTypes.md) - functions returning Result/Option
- See §2.5 for [Pointers](../PART_C_References/05_Pointers.md) - distinction between map types and raw function pointers
- See §2.6 for [Traits](06_Traits.md) - Fn/FnMut/FnOnce traits for closures
- See §2.7 for [Generics](07_Generics.md) - generic functions and type parameters
- See §2.10 for [Self Type](../PART_E_Utilities/10_SelfType.md) - the $ (self) parameter in methods
- See Part III (Permissions) for permission semantics with function values
- See Part V (Effects) for effect tracking in map types
- See Part VI (Procedures) for procedure declaration and implementation details
- **Examples**: See [08_MapExamples.md](../Examples/08_MapExamples.md) for practical function and closure patterns

## 2.8.8 Notes and Warnings

**Note 1 (Terminology):** Cantrip distinguishes between *procedures* (what you declare), *functions* (first-class values), and *map types* (the type system representation). This differs from languages that use "function" for all three concepts.

**Note 2 (Zero-Size Functions):** Function values without captured environment are zero-sized at runtime. They compile to direct calls with no allocation or indirection.

**Note 3 (Static Dispatch):** All map type calls use static dispatch (compile-time resolution). There is no dynamic dispatch or vtable lookup for plain function values.

**Note 4 (Non-Nullable):** Map types cannot be null, unlike raw function pointers in FFI contexts. Use Option<map(T) -> U> for nullable functions.

**Note 5 (Copy Semantics):** Function values (without environment capture) implement Copy trait. Passing a function value creates no copies at runtime.

**Note 6 (Closure Capture):** Closures can capture variables by immutable reference (Fn), mutable reference (FnMut), or by move (FnOnce). Capture mode is inferred from closure body.

**Note 7 (Method Sugar):** The `$` parameter is syntactic sugar for `self: Permission Self`. Methods can be called as `obj.method()` or `Type::method(obj)` (UFCS).

**Note 8 (Contravariance):** Map types are contravariant in parameters (accept more general inputs) and covariant in returns (return more specific outputs).

**Warning 1 (Closure Size):** Closures with captured environment have non-zero size equal to the sum of captured variable sizes. Large captures can impact performance.

**Warning 2 (Move Closures):** The `move` keyword forces ownership transfer of all captured variables. Original variables become inaccessible after closure creation.

**Warning 3 (Recursive Map Types):** Map types can appear recursively only in positive positions (return types, not parameters) to ensure well-foundedness.

**Warning 4 (Effect Requirements):** Functions with effects (`map(T) -> U ! {e}`) can only be called in contexts where those effects are available via `uses` declarations.

**Performance Note 1 (Inlining):** Small functions are typically inlined at call sites, eliminating call overhead entirely. Use `#[inline]` attributes to control inlining.

**Performance Note 2 (Tail Call Optimization):** Cantrip implementations should optimize tail calls to prevent stack overflow in recursive functions. Tail-recursive functions compile to loops.

**Performance Note 3 (Monomorphization):** Generic functions are monomorphized - separate code generated for each concrete type instantiation. This enables full optimization but increases binary size.

**Implementation Note 1 (Calling Conventions):** Function calls follow platform-specific calling conventions (System V AMD64, Microsoft x64, AAPCS64). First parameters pass in registers, remainder on stack.

**Implementation Note 2 (Function Pointers for FFI):** For C FFI, use raw function pointers (`*fn`) instead of map types. Map types are Cantrip-specific and don't match C ABI.

---

**Previous**: [Generics](07_Generics.md) | **Next**: [Type Aliases](../PART_E_Utilities/09_TypeAliases.md)
