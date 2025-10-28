# Part IV: Contracts and Clauses - §2. Behavioral Contracts

**Section**: §2 | **Part**: Contracts and Clauses (Part IV)
**Previous**: [Overview](01_Overview.md) | **Next**: [Effect Clauses](03_Effects.md)

---

**Definition 2.1 (Behavioral Contract):** A behavioral contract defines an abstract set of procedure signatures with associated contracts (effects, preconditions, postconditions) that types can implement to participate in polymorphic behavior.

## 2. Behavioral Contracts

### 2.1 Overview

**Key innovation/purpose:** Behavioral contracts enable:
- **Abstract behavioral specifications**: Define what types must do, not how
- **Polymorphism through contracts**: Any type satisfying a contract can be used polymorphically
- **Contract extension**: Contracts can extend other contracts, inheriting requirements
- **Compile-time verification**: Contract satisfaction is statically verified
- **Zero abstraction cost**: No vtables or runtime dispatch overhead

**Contrast with traits:**

| Aspect | Contracts | Traits |
|--------|-----------|--------|
| **Purpose** | Abstract behavioral specification | Concrete code reuse |
| **Implementations** | **Abstract only** (signatures + clauses) | **Concrete only** (template code) |
| **Polymorphism** | ✓ Yes (interface-based) | ✗ No (mixin-based) |
| **Code sharing** | ✗ No (purely abstract) | ✓ Yes (template implementations) |
| **Use for** | Defining interfaces, polymorphism | Sharing code across types |

**Critical distinction:**
- **Contracts are ABSTRACT**: Only procedure signatures with contract clauses (uses, must, will). No procedure bodies.
- **Traits are CONCRETE**: Template implementations that are attached to types. Always have procedure bodies.

**Example - Contract (abstract):**
```cantrip
contract Drawable {
    procedure draw($)
        uses io.write           // Abstract requirement

    procedure bounds($): Rectangle
        will {                  // Abstract guarantee
            result.width >= 0.0,
            result.height >= 0.0
        }

    // NO IMPLEMENTATIONS - contracts are purely abstract
}
```

**Example - Trait (concrete):**
```cantrip
trait DefaultRenderable {
    procedure render($) {         // Concrete implementation
        println("Rendering default")
    }

    procedure render_with_color($, color: Color) {  // Concrete template
        $.set_color(color)
        $.render()
    }

    // ALL PROCEDURES HAVE BODIES - traits are concrete templates
}
```

**When to use contracts:**
- Defining abstract interfaces for polymorphism
- Specifying behavioral requirements across unrelated types
- Building extensible APIs with strong guarantees
- Replacing traditional inheritance hierarchies
- When you need interface polymorphism (different types, same interface)

**When to use traits:**
- Sharing concrete implementation code across types
- Providing default/template behavior
- Mixins and code reuse patterns
- When multiple types need the same implementation

**When not to use contracts:**
- Sharing concrete implementation code (use traits instead)
- Providing default implementations (use traits instead)
- Simple type aliases or wrappers
- Internal implementation details

### 2.2 Contract Declaration

#### 2.2.1 Basic Syntax

**Concrete syntax:**
```cantrip
contract ContractName {
    procedure_signature;
    procedure_signature;
    ...
}
```

**Example:**
```cantrip
contract Drawable {
    procedure draw($)
    procedure bounds($): Rectangle
}
```

#### 2.2.2 Contracts with Requirements

**Syntax with contracts:**
```cantrip
contract Comparable {
    procedure compare($, other: Self): i32
        will {
            (result < 0) == ($ < other),
            (result == 0) == ($ == other),
            (result > 0) == ($ > other)
        }
}
```

**Example with effects:**
```cantrip
contract Serializable {
    procedure serialize($): Result<String, Error>
        uses alloc.heap

    procedure deserialize(data: String): Result<Self, Error>
        must !data.is_empty()
        uses alloc.heap
}
```

### 2.3 Contract Implementation

Types implement contracts by providing concrete implementations of all required procedures.

#### 2.3.1 Implementation Syntax

**Concrete syntax:**
```cantrip
record TypeName: ContractName {
    // fields

    procedure signature { body }
    procedure signature { body }
    ...
}
```

**Example:**
```cantrip
record Point: Drawable {
    x: f64
    y: f64

    procedure draw($) {
        println("Point at ({}, {})", $.x, $.y)
    }

    procedure bounds($): Rectangle {
        Rectangle { x: $.x, y: $.y, width: 0.0, height: 0.0 }
    }
}
```

#### 2.3.2 Contract Satisfaction Rules

**Rule:**
```
contract C {
    procedure m(T₁) -> T₂
        must P;
        will Q;
        uses ε;
}

record T: C {
    procedure m(T₁) -> T₂
        must P';
        will Q';
        uses ε';
}
─────────────────────────────────────────────────
Valid iff:
  P ⇒ P'      (implementation weakens precondition)
  Q' ⇒ Q      (implementation strengthens postcondition)
  ε' ⊆ ε      (implementation uses fewer effects)
```

**Example:**
```cantrip
contract Validator {
    procedure validate($, value: i32): bool
        must value >= 0
        will result => value < 100
}

record StrictValidator: Validator {
    procedure validate($, value: i32): bool
        must value >= -10          // Weaker (accepts more inputs)
        will result => value < 50   // Stronger (better guarantee)
    {
        value >= -10 && value < 50
    }
}
```

### 2.4 Contract Extension

Contracts can extend other contracts, inheriting all their requirements.

#### 2.4.1 Extension Syntax

**Concrete syntax:**
```cantrip
contract SubContract extends SuperContract {
    additional_signatures;
}
```

**Example:**
```cantrip
contract Displayable {
    procedure display($)
}

contract Interactive extends Displayable {
    procedure handle_click($, x: i32, y: i32)
    procedure handle_key($, key: char)
}
```

#### 2.4.2 Multiple Extension

Contracts can extend multiple contracts:

```cantrip
contract Serializable {
    procedure serialize($): String
}

contract Comparable {
    procedure compare($, other: Self): i32
}

contract Storable extends Serializable, Comparable {
    procedure id($): u64
}
```

#### 2.4.3 Extension Semantics

**Rule:**
```
contract C₂ extends C₁

record T: C₂ requires:
  - Implement all procedures from C₁
  - Implement all procedures from C₂
  - Satisfy all contracts from both C₁ and C₂
```

**Example:**
```cantrip
contract Shape {
    procedure area($): f64
        will result >= 0.0
}

contract ColoredShape extends Shape {
    procedure color($): Color
    procedure set_color(mut $, c: Color)
}

record Circle: ColoredShape {
    radius: f64
    color: Color

    // Must implement Shape's requirements
    procedure area($): f64
        will result >= 0.0
    {
        std::f64::consts::PI * $.radius * $.radius
    }

    // Plus ColoredShape's requirements
    procedure color($): Color { $.color }
    procedure set_color(mut $, c: Color) { $.color = c }
}
```

### 2.5 Polymorphism Through Contracts

#### 2.5.1 Contract Types

Any type implementing a contract can be used where that contract is required.

**Type syntax:**
```cantrip
procedure process_drawable<T>(d: T) where T: Drawable {
    d.draw()
}

procedure process_drawables<T>(items: Vec<T>) where T: Drawable {
    for item in items {
        item.draw()
    }
}
```

#### 2.5.2 Static Dispatch

Contract-based polymorphism uses static dispatch (monomorphization):

```cantrip
contract Processor {
    procedure process($, data: [u8]): Result<Vec<u8>, Error>
        uses alloc.heap
}

procedure batch_process<T: Processor>(
    processor: T,
    datasets: Vec<[u8]>
): Vec<Result<Vec<u8>, Error>> {
    datasets.map(|data| processor.process(data))
}
// Monomorphized for each concrete type implementing Processor
```

#### 2.5.3 Contract Bounds

Type parameters can be bounded by contracts:

```cantrip
record Container<T> where T: Serializable {
    items: Vec<T>

    procedure save($, path: String): Result<(), Error>
        uses io.write, alloc.heap
    {
        let data = $.items
            .iter()
            .map(|item| item.serialize())
            .collect::<Result<Vec<_>, _>>()?
        std::fs::write(path, data.join("\n"))
    }
}
```

### 2.6 Associated Types in Contracts

Contracts can declare associated types that implementations must specify.

#### 2.6.1 Associated Type Syntax

```cantrip
contract Iterator {
    type Item

    procedure next(mut $): Option<Self::Item>
}

record Range: Iterator {
    current: i32
    end: i32

    type Item = i32

    procedure next(mut $): Option<i32> {
        if $.current < $.end {
            let result = $.current
            $.current += 1
            Some(result)
        } else {
            None
        }
    }
}
```

#### 2.6.2 Associated Types with Constraints

```cantrip
contract Container {
    type Element where Element: Clone

    procedure get($, index: usize): Option<Self::Element>
    procedure set(mut $, index: usize, value: Self::Element)
}
```

### 2.7 Implementing Contracts with Different Types

When you need to work with different types implementing the same contract, use enums for explicit, zero-cost polymorphism:

```cantrip
contract Logger {
    procedure log($, message: String)
        uses io.write
}

record FileLogger {
    path: String

    procedure log($, message: String) uses io.write {
        // Write to file
    }
}

record ConsoleLogger {
    procedure log($, message: String) uses io.write {
        println("{}", message)
    }
}

// Explicit, zero-cost polymorphism with enums
enum AnyLogger {
    File(FileLogger),
    Console(ConsoleLogger),
}

record Application {
    logger: AnyLogger  // Explicit, zero-cost

    procedure log_event($, event: String)
        uses io.write
    {
        match $.logger {
            AnyLogger.File(l) => l.log(event),
            AnyLogger.Console(l) => l.log(event),
        }
    }
}
```

**Why enums instead of dynamic dispatch:**
- Zero runtime cost (no vtables or indirection)
- Explicit (you see all possible types)
- Exhaustive checking (compiler ensures all cases handled)
- Aligns with Cantrip's "explicit over implicit" philosophy

### 2.8 Coherence and Orphan Rules

Contract implementations must follow coherence rules to ensure uniqueness.

#### 2.8.1 Orphan Rule

You can implement a contract for a type only if:
- The contract is defined in your crate, OR
- The type is defined in your crate

**Valid:**
```cantrip
// In crate A:
contract MyContract { ... }

// In crate B:
record MyType: MyContract {
    // fields
    // procedures implementing MyContract
}  // OK: MyType is local
```

**Invalid:**
```cantrip
// In crate C (different from A and B):
// Cannot implement MyContract for MyType  // ERROR: Neither is local
```

#### 2.8.2 Blanket Implementations

Generic implementations must not overlap:

```cantrip
contract Debug {
    procedure debug($): String
}

// Note: Blanket implementations require language support
// for automatic implementation across types with specific constraints
// Syntax for blanket implementations is TBD

// ERROR: Would overlap if String also tries to implement Debug
// when blanket implementation exists for all T: Display
```

### 2.9 Advanced Contract Patterns

#### 2.9.1 Marker Contracts

Contracts with no procedures serve as markers:

```cantrip
contract Send {}  // Types safe to send between threads
contract Sync {}  // Types safe to share between threads

record ThreadSafeCounter: Send, Sync {
    count: AtomicU32;

    // Marker contracts have no procedures to implement
}
```

#### 2.9.2 Sealed Contracts

Contracts can be sealed to prevent external implementation:

```cantrip
contract Sealed {
    private procedure seal_marker();
}

contract MyContract extends Sealed {
    procedure my_method($);
}

// Only types in this module can implement MyContract
// (cannot implement private procedure from outside)
```

#### 2.9.3 Supertrait Requirements

Contracts can require implementers to also implement other contracts:

```cantrip
contract Eq {
    procedure eq($, other: Self): bool
}

contract Ord where Self: Eq {
    procedure cmp($, other: Self): Ordering
}

// To implement Ord, must also implement Eq
record Priority: Eq, Ord {
    value: i32

    procedure eq($, other: Self): bool {
        $.value == other.value
    }

    procedure cmp($, other: Self): Ordering {
        $.value.cmp(other.value)
    }
}
```

### 2.10 Formal Semantics

#### 2.10.1 Contract Well-Formedness

**Rule [ContractWF]:**
```
Γ ⊢ signature₁ : τ₁
Γ ⊢ signature₂ : τ₂
...
Γ ⊢ signatureₙ : τₙ
All signatures distinct
─────────────────────────────────────────────────
Γ ⊢ contract C { signature₁; ...; signatureₙ } ok
```

#### 2.10.2 Implementation Well-Formedness

**Rule [ImplWF]:**
```
Γ ⊢ contract C { s₁; ...; sₙ }
Γ ⊢ record T: C { m₁; ...; mₙ }
∀i. signature(mᵢ) matches sᵢ
∀i. contracts(mᵢ) satisfy contracts(sᵢ)
─────────────────────────────────────────────────
Γ ⊢ implementation valid
```

#### 2.10.3 Contract Extension

**Rule [ExtensionWF]:**
```
Γ ⊢ contract C₁ { s₁; ...; sₘ }
Γ ⊢ contract C₂ extends C₁ { t₁; ...; tₙ }
{s₁, ..., sₘ} ∩ {t₁, ..., tₙ} = ∅
─────────────────────────────────────────────────
Γ ⊢ C₂ inherits s₁, ..., sₘ, t₁, ..., tₙ
```

### 2.11 Complete Example

A comprehensive example showing contracts, extension, and polymorphism:

```cantrip
// Base contract
contract Drawable {
    procedure draw($)
        uses io.write

    procedure bounds($): Rectangle
        will {
            result.width >= 0.0,
            result.height >= 0.0
        }
}

// Extended contract
contract Transformable extends Drawable {
    procedure translate(mut $, dx: f64, dy: f64)
    procedure scale(mut $, factor: f64)
        must factor > 0.0
}

// Record implementing extended contract
record Circle: Transformable {
    x: f64
    y: f64
    radius: f64

    where {
        radius > 0.0
    }

    procedure draw($)
        uses io.write
    {
        println("Circle at ({}, {}) with radius {}", $.x, $.y, $.radius)
    }

    procedure bounds($): Rectangle
        will {
            result.width >= 0.0,
            result.height >= 0.0
        }
    {
        Rectangle {
            x: $.x - $.radius,
            y: $.y - $.radius,
            width: $.radius * 2.0,
            height: $.radius * 2.0
        }
    }

    procedure translate(mut $, dx: f64, dy: f64) {
        $.x += dx
        $.y += dy
    }

    procedure scale(mut $, factor: f64)
        must factor > 0.0
    {
        $.radius *= factor
    }
}

// Polymorphic function using contract bound
procedure render_shapes<T>(shapes: Vec<T>)
    where T: Drawable
{
    for shape in shapes {
        shape.draw()
        let bounds = shape.bounds()
        println("  Bounds: {}x{}", bounds.width, bounds.height)
    }
}

// Usage
procedure main() {
    let circles = vec![
        Circle { x: 0.0, y: 0.0, radius: 5.0 },
        Circle { x: 10.0, y: 10.0, radius: 3.0 }
    ]

    render_shapes(circles)  // Monomorphized for Circle
}
```

---

**Previous**: [Overview](01_Overview.md) | **Next**: [Effect Clauses](03_Effects.md)
