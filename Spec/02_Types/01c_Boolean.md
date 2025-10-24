# Part II: Type System - §5.3 Boolean Type

**Section**: §5.3 | **Part**: Type System (Part II)
**Previous**: [Floating-Point Types](01b_FloatingPoint.md) | **Next**: [Character Type](01d_Character.md)

---

**Definition 5.3.1 (Boolean Type):** The boolean type `bool` represents logical truth values with exactly two inhabitants: `true` and `false`. It provides the foundation for conditional logic and control flow.

## 5.3 Boolean Type

### 5.3.1 Overview

**Key innovation/purpose:** `bool` provides type-safe two-valued logic with short-circuit evaluation for conditional expressions, essential for control flow and predicate logic.

**When to use bool:**
- Conditional control flow (if, while, match guards)
- Logical predicates and assertions
- Boolean flags and state indicators
- Function return values for yes/no questions

**When NOT to use bool:**
- Multiple states → use enums (§9)
- Optional presence → use `Option<T>`
- Numeric values → use integer types (§5.1)
- Bitwise operations on multiple flags → use integer bit masks

**Relationship to other features:**
- **Control flow**: Boolean expressions control `if`, `while`, `match` guards
- **Short-circuit evaluation**: `&&` and `||` operators short-circuit
- **Pattern matching**: `true` and `false` are patterns
- **Copy semantics**: `bool` is a `Copy` type

### 5.3.2 Syntax

#### Concrete Syntax

**Type and literal syntax:**
```ebnf
BoolType    ::= "bool"
BoolLiteral ::= "true" | "false"
```

**Examples:**
```cantrip
let flag: bool = true;
let is_valid = false;

if flag {
    println("Flag is set");
}
```

#### Abstract Syntax

**Type:**
```
τ ::= bool
```

**Values:**
```
v ::= true | false
```

### 5.3.3 Static Semantics

#### Well-Formedness Rules

```
[WF-Bool]
──────────────────
Γ ⊢ bool : Type
```

#### Type Rules

**Boolean literal typing:**

```
[T-Bool-True]
──────────────────
Γ ⊢ true : bool

[T-Bool-False]
──────────────────
Γ ⊢ false : bool
```

**No type inference needed:** Boolean literals always have type `bool` with no context sensitivity or suffixes.

#### Type Properties

**Theorem 5.3.1 (Value Set):**

The boolean type has exactly two values:

```
⟦bool⟧ = {true, false} ≅ {⊤, ⊥} ≅ ℤ₂
```

**Theorem 5.3.2 (Copy Semantics):**

```
bool : Copy
```

Booleans are always copied, never moved.

**Theorem 5.3.3 (Size and Alignment):**

```
size(bool) = 1 byte
align(bool) = 1 byte
```

### 5.3.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**
```
[E-Bool-True]
────────────────────────
⟨true, σ⟩ ⇓ ⟨true, σ⟩

[E-Bool-False]
────────────────────────
⟨false, σ⟩ ⇓ ⟨false, σ⟩
```

**Logical operations:**

```
[E-And-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩
────────────────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩

[E-And-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ'⟩
────────────────────────────────────────
⟨e₁ && e₂, σ⟩ ⇓ ⟨false, σ'⟩    (short-circuit, e₂ not evaluated)

[E-Or-False]
⟨e₁, σ⟩ ⇓ ⟨false, σ₁⟩    ⟨e₂, σ₁⟩ ⇓ ⟨v₂, σ₂⟩
────────────────────────────────────────
⟨e₁ || e₂, σ⟩ ⇓ ⟨v₂, σ₂⟩

[E-Or-True]
⟨e₁, σ⟩ ⇓ ⟨true, σ'⟩
────────────────────────────────────────
⟨e₁ || e₂, σ⟩ ⇓ ⟨true, σ'⟩    (short-circuit, e₂ not evaluated)

[E-Not]
⟨e, σ⟩ ⇓ ⟨v, σ'⟩
────────────────────────────────────────
⟨!e, σ⟩ ⇓ ⟨¬v, σ'⟩    (logical negation)
```

#### Memory Representation

**Boolean layout:**

```
bool memory layout (1 byte):
┌────┐
│0/1 │
└────┘
Size: 1 byte
Alignment: 1 byte

Values:
- false = 0x00 (all bits zero)
- true  = 0x01 (least significant bit set, others zero)
```

**Representation guarantee:**

```
⟦false⟧ = 0x00
⟦true⟧  = 0x01
```

**Note:** While only values 0x00 and 0x01 are valid for `bool`, Cantrip guarantees all boolean values are normalized to these representations. Creating a `bool` with any other bit pattern (via unsafe code) is undefined behavior.

#### Operational Behavior

**Boolean algebra:**

Cantrip booleans follow standard Boolean algebra:

```
Logical AND (∧):
⊤ ∧ ⊤ = ⊤
⊤ ∧ ⊥ = ⊥
⊥ ∧ ⊤ = ⊥
⊥ ∧ ⊥ = ⊥

Logical OR (∨):
⊤ ∨ ⊤ = ⊤
⊤ ∨ ⊥ = ⊤
⊥ ∨ ⊤ = ⊤
⊥ ∨ ⊥ = ⊥

Logical NOT (¬):
¬⊤ = ⊥
¬⊥ = ⊤
```

**Short-circuit evaluation:**

```
⊥ ∧ _ = ⊥    (right side not evaluated)
⊤ ∨ _ = ⊤    (right side not evaluated)
```

**Examples:**
```cantrip
// Short-circuit prevents division by zero
let safe = (y != 0) && (x / y > 10);

// Short-circuit prevents null dereference
let result = (ptr != null) && (*ptr == target);
```

**Comparison:**

```
true == true   = true
false == false = true
true == false  = false
true != false  = true
```

### 5.3.5 Examples and Patterns

#### Conditional Logic

**If expressions:**
```cantrip
let x = 42;
let result = if x > 0 {
    "positive"
} else if x < 0 {
    "negative"
} else {
    "zero"
};
```

**Match with boolean:**
```cantrip
let is_valid: bool = check_input();

match is_valid {
    true -> process_data(),
    false -> show_error(),
}
```

#### Logical Predicates

**Predicate functions:**
```cantrip
function is_even(n: i32): bool {
    n % 2 == 0
}

function is_prime(n: u64): bool {
    if n < 2 { return false; }
    if n == 2 { return true; }
    if n % 2 == 0 { return false; }

    let limit = (n as f64).sqrt() as u64;
    for i in (3..=limit).step_by(2) {
        if n % i == 0 { return false; }
    }
    true
}

function is_palindrome(s: str): bool {
    s == s.chars().rev().collect::<String>()
}
```

#### Boolean Combinations

**Complex conditions:**
```cantrip
function is_leap_year(year: i32): bool {
    (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)
}

function in_range(value: i32, min: i32, max: i32): bool {
    value >= min && value <= max
}

function is_valid_username(name: str): bool {
    let len = name.len();
    len >= 3 && len <= 20 && name.chars().all(|c| c.is_alphanumeric() || c == '_')
}
```

#### Loop Conditions

**While loops:**
```cantrip
let mut running: bool = true;
let mut count = 0;

while running {
    count += 1;
    if count >= 10 {
        running = false;
    }
}
```

**Loop with break:**
```cantrip
let mut found: bool = false;

loop {
    let item = get_next_item();
    if item == target {
        found = true;
        break;
    }
    if item == null {
        break;
    }
}
```

#### Assertions and Contracts

**Runtime assertions:**
```cantrip
function divide(x: f64, y: f64): f64
    must y != 0.0;
{
    assert!(y != 0.0, "division by zero");
    x / y
}

function process_valid_data(data: [u8])
    must data.len() > 0;
    must data[0] == MAGIC_BYTE;
    will result.is_valid();
{
    assert!(!data.is_empty());
    assert_eq!(data[0], MAGIC_BYTE);
    // ...
}
```

#### Boolean Flags

**State flags:**
```cantrip
record ConnectionState {
    connected: bool;
    authenticated: bool;
    encrypted: bool;

    procedure is_ready(self: ConnectionState): bool {
        self.connected && self.authenticated && self.encrypted
    }

    procedure can_send_data(self: ConnectionState): bool {
        self.connected && self.encrypted
    }
}
```

**When to use enum instead:**

```cantrip
// BAD: Many related booleans
record FileState {
    is_open: bool;
    is_closed: bool;
    is_error: bool;
}  // Mutually exclusive states → should be enum!

// GOOD: Use enum for mutually exclusive states
enum FileState {
    Open,
    Closed,
    Error(String),
}
```

---

**Previous**: [Floating-Point Types](01b_FloatingPoint.md) | **Next**: [Character Type](01d_Character.md)
