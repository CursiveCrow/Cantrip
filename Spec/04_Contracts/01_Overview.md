# Part IV: Contracts and Clauses - §1. Overview

**Section**: §1 | **Part**: Contracts and Clauses (Part IV)
**Next**: [Behavioral Contracts](02_BehavioralContracts.md)

---

**Definition 1.1 (Contract System):** The contract system in Cantrip provides mechanisms for specifying and enforcing behavioral guarantees on procedures and types. Contracts include effects declarations (`uses`), preconditions (`must`), postconditions (`will`), type-level invariants (`where`), and behavioral contracts (`contract`) for polymorphism.

## 1. Contract System Overview

### 1.1 Introduction

**Purpose:** The contract system enables developers to specify precise behavioral requirements and guarantees, supporting:
- **Effect tracking**: What computational effects a procedure may perform
- **Preconditions**: What must be true before a procedure executes
- **Postconditions**: What must be true after a procedure executes
- **Type invariants**: What must always be true for a type's instances

**Design philosophy:**
- **Explicit over implicit**: All effects and requirements must be declared
- **Compositional**: Contracts compose naturally through procedure calls
- **Verifiable**: Contracts are checked statically where possible, dynamically where necessary
- **Zero-cost when unused**: Contracts that can be proven statically have no runtime cost

### 1.2 Contract Components

The Cantrip contract system consists of four major components:

#### 1.2.1 Effects (`uses`)

Effects declare what computational side effects a procedure may perform:

```cantrip
procedure log(message: String)
    uses io.write, alloc.heap;
{
    println!("{}", message);
}
```

**Properties:**
- Effects must be explicitly declared
- Effects compose transitively through calls
- Pure procedures have no `uses` clause
- See [§3. Effect Clauses](03_Effects.md)

#### 1.2.2 Preconditions (`must`)

Preconditions specify what must be true before a procedure executes:

```cantrip
procedure divide(numerator: f64, denominator: f64): f64
    must denominator != 0.0;
{
    numerator / denominator
}
```

**Properties:**
- Checked at procedure entry
- Caller's responsibility to ensure
- Can be statically verified or dynamically checked
- See [§4. Precondition Clauses](04_Preconditions.md)

#### 1.2.3 Postconditions (`will`)

Postconditions specify what must be true after a procedure executes:

```cantrip
procedure increment(counter: mut i32): i32
    will result == @old(counter) + 1;
{
    counter += 1;
    counter
}
```

**Properties:**
- Checked at procedure exit
- Procedure's responsibility to ensure
- Can reference `result` and `@old(expr)` for pre-values
- See [§5. Postcondition Clauses](05_Postconditions.md)

#### 1.2.4 Type Constraints (`where`)

Type constraints specify invariants that must hold for all instances of a type:

```cantrip
record Percentage {
    value: i32;

    where {
        value >= 0,
        value <= 100,
    }
}
```

**Properties:**
- Checked at construction and mutation
- Define valid state space
- Maintained across all operations
- See [§6. Constraint Clauses](06_TypeConstraints.md)

### 1.3 Contract Syntax Summary

**Complete procedure signature with contracts:**

```cantrip
procedure name<Generics>(params): ReturnType
    uses effect1, effect2, ...;
    must {
        precondition1,
        precondition2,
        ...
    }
    will {
        postcondition1,
        postcondition2,
        ...
    }
{
    body
}
```

**Type with constraints:**

```cantrip
record TypeName<Generics> {
    fields;

    where {
        constraint1,
        constraint2,
        ...
    }
}
```

### 1.4 Contract Checking

Contracts are checked through a combination of static and dynamic verification:

**Static checking (compile-time):**
- Effect type checking
- Simple precondition/postcondition verification
- Type constraint checking for const generics
- Flow-sensitive analysis

**Dynamic checking (runtime):**
- Preconditions that depend on runtime values
- Postconditions that cannot be proven statically
- Type constraints on field values
- Assertion-based verification

**Optimization:**
The compiler eliminates redundant checks and proven contracts to minimize runtime overhead.

### 1.5 Contract Hierarchy

```
Contract System
├── Procedure-Level Contracts
│   ├── Effects (uses)
│   ├── Preconditions (must)
│   └── Postconditions (will)
│
├── Type-Level Contracts
│   └── Invariants (where)
│
└── Behavioral Contracts
    └── Abstract Interfaces (contract)
```

**Interaction:**
- Type constraints are always maintained
- Preconditions can reference type constraints
- Postconditions can reference type constraints
- Effects are orthogonal to conditions
- Behavioral contracts declare abstract procedures with contracts

### 1.6 Contract Composition

Contracts compose naturally through procedure calls:

```cantrip
procedure helper(x: i32): i32
    must x > 0;
    will result > x;
{
    x + 1
}

procedure caller(y: i32): i32
    must y > 5;        // Implies y > 0
    will result > y;    // Transitively > 5
{
    helper(y)              // helper's contracts apply
}
```

**Composition rules:**
1. **Effects aggregate**: Caller uses all effects of callees
2. **Preconditions strengthen**: Caller must ensure callee preconditions
3. **Postconditions weaken**: Caller can rely on callee postconditions
4. **Type constraints persist**: Always maintained

See [§7. Clause Composition](07_ContractComposition.md)

### 1.7 Relationship to Other Features

**Generics (Part II, §7):**
- Contracts can constrain generic parameters
- Generic procedures inherit contract requirements

**Traits (Part II, §6):**
- Trait procedures can declare contracts
- Attaching types inherit contract obligations

**Self Type (Part II, §12):**
- Contracts can reference `Self` in trait/type contexts

**Permissions (Part III):**
- Contracts work with permission system
- Postconditions can specify permission changes

**Effect Clauses (Part IV, §3):**
- Effects are part of procedure type signatures
- Effect checking is statically enforced

### 1.8 Design Rationale

**Why explicit contracts?**

1. **Correctness**: Catch errors at compile time or procedure boundaries
2. **Documentation**: Contracts serve as machine-checked documentation
3. **Optimization**: Compiler can optimize based on contract guarantees
4. **Verification**: Enable formal verification and proof
5. **Clarity**: Make implicit assumptions explicit

**Why multiple contract types?**

Each contract type serves a distinct purpose:
- **Effects**: Track computational side effects
- **Preconditions**: Specify caller obligations
- **Postconditions**: Specify procedure guarantees
- **Type constraints**: Define valid state spaces

**Why both static and dynamic checking?**

Some contracts are decidable at compile time (e.g., effect checking, const generic constraints), while others depend on runtime values (e.g., `x > 0` where `x` is a parameter). Hybrid checking provides the best balance of safety and flexibility.

### 1.9 Examples

**Example 1: Safe division**
```cantrip
procedure safe_divide(a: f64, b: f64): Result<f64, String>
    will match result {
        Ok(v) => v == a / b,
        Err(_) => b == 0.0,
    };
{
    if b == 0.0 {
        Err("Division by zero")
    } else {
        Ok(a / b)
    }
}
```

**Example 2: Array bounds**
```cantrip
procedure get<T>(arr: [T], index: usize): T
    must index < arr.len();
    will result == arr[index];
{
    arr[index]  // Safe: precondition ensures bounds
}
```

**Example 3: Sorted insertion**
```cantrip
record SortedVec<T: Ord> {
    elements: Vec<T>;

    where {
        is_sorted(elements),
    }

    procedure insert(mut $, item: T)
        will is_sorted($.elements);
    {
        let pos = $.elements.binary_search(&item).unwrap_or_else(|e| e);
        $.elements.insert(pos, item);
        // Postcondition automatically checked
    }
}
```

**Example 4: Effect composition**
```cantrip
procedure read_config(path: String): Result<Config, Error>
    uses io.read, alloc.heap;
{
    let contents = std::fs::read_to_string(path)?;  // uses io.read
    parse_config(contents)                           // uses alloc.heap
}

procedure parse_config(json: String): Result<Config, Error>
    uses alloc.heap;
{
    // Parse JSON into Config
    ...
}
```

### 1.10 Specification Structure

This part of the specification is organized as follows:

1. **Overview** (this document) - Introduction and motivation
2. **Behavioral Contracts** - The `contract` keyword for polymorphism and extension
3. **Effect Clauses** - The `uses` clause and effect system
4. **Precondition Clauses** - The `must` clause
5. **Postcondition Clauses** - The `will` clause
6. **Constraint Clauses** - The `where` clause for types
7. **Clause Composition** - How contracts and clauses combine and interact

Each section provides:
- Concrete and abstract syntax
- Static semantics (well-formedness and type rules)
- Dynamic semantics (evaluation and checking)
- Examples and patterns
- Interaction with other language features

---

**Next**: [Behavioral Contracts](02_BehavioralContracts.md)
