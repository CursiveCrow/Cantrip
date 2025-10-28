# Part IV: Contracts and Clauses - §5. Postcondition Clauses

**Section**: §5 | **Part**: Contracts and Clauses (Part IV)
**Previous**: [Precondition Clauses](04_Preconditions.md) | **Next**: [Constraint Clauses](06_TypeConstraints.md)

---

**Definition 5.1 (Postcondition Clause):** A postcondition clause (`will`) is a predicate that must be true when a procedure returns. Postconditions specify the procedure's guarantees and define what callers can rely upon after the call completes.

## 5. Postcondition Clauses

### 5.1 Overview

**Key innovation/purpose:** Postconditions make procedure guarantees explicit, enabling:
- **Output validation**: Specify properties of return values and mutated parameters
- **Contract-based programming**: Procedure guarantees are explicit
- **Static verification**: Many postconditions can be verified at compile time
- **Documentation**: Self-documenting behavior and results
- **Optimization**: Compiler can use postconditions for optimization

**When to use `will`:**
- Return values must satisfy properties (e.g., `result > 0`)
- Mutated parameters must maintain invariants
- Relationships between inputs and outputs
- State transitions are valid
- Collections maintain properties (sorted, non-empty, etc.)
- Successful operation guarantees

**When NOT to use `will`:**
- Requirements on inputs (use must instead)
- Effect declarations (use uses clause instead)
- Type invariants (use where clauses instead)
- Error cases (use Result types)

**Relationship to other features:**
- **Precondition Clauses (§4):** Ensures specifies outputs; must specifies inputs
- **Constraint Clauses (§6):** Where clauses are always-true; will apply to specific calls
- **Effect Clauses (§3):** Postconditions are orthogonal to effects
- **Procedures**: Postconditions are checked at procedure exit

### 5.2 Syntax

#### 5.2.1 Concrete Syntax

**Postcondition clause:**
```ebnf
WillClause ::= "will" PredicateList

PredicateList ::= Predicate ("," Predicate)*

Predicate     ::= Expression

Expression    ::= "result" RelOp Expression
                | "@old" "(" Expression ")"
                | ParamRef RelOp Expression
                | BooleanExpr
```

**Special identifiers:**
- `result` — The return value of the procedure
- `@old(expr)` — The value of `expr` at procedure entry

**Examples:**
```cantrip
// Return value constraint
procedure abs(x: i32): i32
    will result >= 0
{
    if x < 0 { -x } else { x }
}

// Relationship with input
procedure increment(x: i32): i32
    will result == x + 1
{
    x + 1
}

// Mutation guarantee
procedure push<T>(vec: mut Vec<T>, item: T)
    will vec.len() == @old(vec.len()) + 1
{
    vec.push(item)
}

// Multiple postconditions
procedure clamp(value: i32, min: i32, max: i32): i32
    must min <= max
    will {
        result >= min,
        result <= max,
        (value >= min && value <= max) => result == value
    }
{
    if value < min {
        min
    } else if value > max {
        max
    } else {
        value
    }
}
```

#### 5.2.2 Abstract Syntax

**Postcondition representation:**
```
procedure f(x₁: τ₁, ..., xₙ: τₙ): τ will Q(x₁, ..., xₙ, result);

Where:
  Q = predicate over parameters and result
  Q : (τ₁, ..., τₙ, τ) → bool
```

**Hoare triple semantics:**
```
{P} f(args) {Q}

Where:
  P = precondition (requires)
  Q = postcondition (will)

If P holds before f, then Q holds after f
```

**@old operator:**
```
@old(expr) = value of expr at procedure entry

Semantics:
  @old(expr) is evaluated and saved before procedure body executes
```

**Multiple postconditions:**
```
will Q₁, Q₂, ..., Qₙ;

Equivalent to:
  will Q₁ ∧ Q₂ ∧ ... ∧ Qₙ;
```

#### 5.2.3 Basic Examples

**Example 1: Return value properties**
```cantrip
procedure square(x: i32): i32
    will result >= 0
{
    x * x
}
```

**Example 2: Relationship between input and output**
```cantrip
procedure double(x: i32): i32
    will result == 2 * x
{
    x + x
}
```

**Example 3: Mutation postcondition**
```cantrip
procedure increment(counter: mut i32)
    will counter == @old(counter) + 1
{
    counter += 1
}
```

### 5.3 Static Semantics

#### 5.3.1 Well-Formedness Rules

**[WF-Ensures] Well-formed postcondition:**
```
[WF-Ensures]
procedure f(params): τ { body }
Γ, params, result: τ ⊢ Q : bool
Q can reference params, result, @old(expr)
─────────────────────────────────────────────────
will Q well-formed
```

**Explanation:** Postconditions must be boolean expressions over parameters, result, and @old values.

**[WF-EnsuresProc] Well-formed procedure with postcondition:**
```
[WF-EnsuresProc]
procedure f(params): τ { body } well-formed
will Q well-formed
Q does not perform effects
─────────────────────────────────────────────────
procedure f(params): τ will Q { body } well-formed
```

**Explanation:** Postconditions must be pure and side-effect free.

**[WF-OldExpr] Well-formed @old expression:**
```
[WF-OldExpr]
Γ, params ⊢ expr : τ
expr references parameters or their fields
─────────────────────────────────────────────────
Γ ⊢ @old(expr) : τ
```

**Explanation:** @old can only reference expressions that are valid at procedure entry.

#### 5.3.2 Type Rules

**[T-EnsuresProc] Procedure with postcondition:**
```
[T-EnsuresProc]
Γ ⊢ procedure f(params): τ { body }
Γ, params ⊢ body : τ
Γ, params, result: τ ⊢ Q : bool
body guarantees Q
─────────────────────────────────────────────────
Γ ⊢ f : map(params) -> τ will Q
```

**Explanation:** Procedure body must guarantee the postcondition holds for all return paths.

**[T-EnsuresCall] Call site relies on postcondition:**
```
[T-EnsuresCall]
Γ ⊢ f : map(τ₁, ..., τₙ) -> τ will Q
Γ ⊢ args : τ₁, ..., τₙ
let result = f(args)
─────────────────────────────────────────────────
Γ ⊢ Q[params ↦ args, result ↦ result] : bool (can be assumed)
```

**Explanation:** After calling f, the caller can assume the postcondition holds.

**[T-EnsuresVerify] Static postcondition verification:**
```
[T-EnsuresVerify]
procedure f will Q { body }
compiler proves body ⇒ Q for all return paths
─────────────────────────────────────────────────
postcondition statically verified (no runtime check)
```

**Explanation:** If the compiler can prove the postcondition always holds, no runtime check is needed.

**[T-EnsuresWeaken] Postcondition weakening:**
```
[T-EnsuresWeaken]
contract T { procedure m will Q₁; }
record Type: T { procedure m will Q₂ { ... } }
Q₂ ⇒ Q₁  (Q₂ stronger than Q₁)
─────────────────────────────────────────────────
OK: Can strengthen postconditions
```

**Explanation:** Implementations can provide stronger guarantees (Liskov substitution).

#### 5.3.3 Type Properties

**Theorem 4.1 (Postcondition Soundness):**

If a procedure will Q and executes successfully, Q holds at return:

```
Γ ⊢ f : map(params) -> τ will Q
⟨f(args), σ⟩ ⇓ ⟨result, σ'⟩
─────────────────────────────────────────────────
Q[params ↦ args, result ↦ result] holds in σ'
```

**Proof sketch:** By postcondition checking. Either statically verified or dynamically checked.

**Theorem 4.2 (Postcondition Guarantee Transfer):**

Calling a procedure with postcondition allows caller to assume the guarantee:

```
procedure f will Q { body }
procedure g { let x = f(args); ... }
─────────────────────────────────────────────────
After f(args), g can assume Q[result ↦ x]
```

**Proof sketch:** By [T-EnsuresCall]. Type system allows callers to rely on callee postconditions.

**Theorem 4.3 (Static Elimination):**

Provably satisfied postconditions have zero runtime cost:

```
procedure f(x: i32): i32 will result > 0 {
    x.abs() + 1  // Always > 0
}
─────────────────────────────────────────────────
No runtime check needed (proven by compiler)
```

**Proof sketch:** Compiler analyzes body and proves postcondition always holds.

**Property 4.1 (Postcondition Strengthening):**

Callees can strengthen (provide better) postconditions:

```
contract T { procedure m(): i32 will result > 0; }

record Type: T { procedure m(): i32 will result > 10; }  // Stronger - OK
```

**Property 4.2 (@old Capture):**

@old expressions are evaluated at procedure entry:

```
procedure f(x: mut i32) will x == @old(x) + 1 {
    x += 1;  // @old(x) captured before this
}
```

### 5.4 Dynamic Semantics

#### 5.4.1 Evaluation Rules

**[E-EnsuresSatisfied] Postcondition satisfied:**
```
[E-EnsuresSatisfied]
procedure f(params): τ will Q { body }
⟨body[params ↦ args], σ⟩ ⇓ ⟨v, σ'⟩
⟨Q[params ↦ args, result ↦ v], σ'⟩ ⇓ ⟨true, σ'⟩
─────────────────────────────────────────────────
⟨f(args), σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** If body produces result and postcondition holds, return result.

**[E-EnsuresViolated] Postcondition violated:**
```
[E-EnsuresViolated]
procedure f(params): τ will Q { body }
⟨body[params ↦ args], σ⟩ ⇓ ⟨v, σ'⟩
⟨Q[params ↦ args, result ↦ v], σ'⟩ ⇓ ⟨false, σ'⟩
─────────────────────────────────────────────────
⟨f(args), σ⟩ ⇓ panic("postcondition violated: Q")
```

**Explanation:** If postcondition doesn't hold at return, panic.

**[E-OldCapture] @old expression capture:**
```
[E-OldCapture]
procedure f(...) will ... @old(expr) ... { body }
⟨expr, σ_entry⟩ ⇓ ⟨v_old, σ_entry⟩
⟨body, σ_entry⟩ ⇓ ⟨v_result, σ_exit⟩
─────────────────────────────────────────────────
@old(expr) in postcondition evaluates to v_old
```

**Explanation:** @old expressions are captured at procedure entry, before body executes.

#### 5.4.2 Checking Strategies

**Static verification (compile-time):**

The compiler attempts to prove postconditions using:
- Symbolic execution
- Path-sensitive analysis
- Dataflow analysis
- SMT solvers (optionally)

```cantrip
procedure abs(x: i32): i32
    will result >= 0
{
    if x < 0 { -x } else { x }
}

// Compiler proves:
// Path 1: x < 0 → result = -x → -x >= 0 ✓ (when x < 0)
// Path 2: x >= 0 → result = x → x >= 0 ✓
```

**Dynamic checking (runtime):**

When static proof fails, insert runtime checks:

```cantrip
procedure complex_calc(x: i32): i32
    will result > 0
{
    // Complex calculation
    let r = compute(x)
    r  // Runtime check: assert!(r > 0)
}

// Compiles to:
procedure complex_calc(x: i32): i32 {
    let r = compute(x)
    if !(r > 0) {
        panic!("postcondition violated: result > 0")
    }
    r
}
```

**Multiple return points:**

Postconditions are checked at all return points:

```cantrip
procedure find_positive(numbers: [i32]): Option<i32>
    will match result {
        Some(n) => n > 0,
        None => true
    }
{
    for n in numbers {
        if n > 0 {
            return Some(n)  // Check here
        }
    }
    None  // Check here too
}
```

#### 5.4.3 @old Implementation

**Capture mechanism:**

@old expressions are evaluated and stored before body execution:

```cantrip
procedure increment(counter: mut i32)
    will counter == @old(counter) + 1
{
    counter += 1
}

// Compiles to:
procedure increment(counter: mut i32) {
    let __old_counter = counter  // Capture @old value
    counter += 1
    if !(counter == __old_counter + 1) {  // Check using captured value
        panic!("postcondition violated")
    }
}
```

**Multiple @old references:**

Each @old expression is captured once:

```cantrip
procedure swap(a: mut i32, b: mut i32)
    will a == @old(b), b == @old(a)
{
    let temp = a
    a = b
    b = temp
}

// Captures:
// let __old_a = a
// let __old_b = b
// Then checks: a == __old_b && b == __old_a
```

### 5.5 Examples and Patterns

#### 5.5.1 Sorted Insertion

```cantrip
record SortedVec<T> where T: Ord {
    elements: Vec<T>

    procedure insert(mut $, item: T)
        will {
            $.len() == @old($.len()) + 1,
            is_sorted($.elements),
            $.contains(item)
        }
    {
        let pos = $.elements.binary_search(item).unwrap_or_else(|e| e)
        $.elements.insert(pos, item)
    }
}
```

#### 5.5.2 Bank Account Transfer

```cantrip
procedure transfer(from: mut Account, to: mut Account, amount: i64)
    must {
        amount > 0,
        from.balance >= amount,
        from.id != to.id
    }
    will {
        from.balance == @old(from.balance) - amount,
        to.balance == @old(to.balance) + amount
    }
{
    from.balance -= amount
    to.balance += amount
}
```

#### 5.5.3 Cache Guarantees

```cantrip
record Cache<K, V> where V: Copy {
    data: HashMap<K, V>

    procedure get_or_insert(mut $, key: K, value: V): V
        will {
            $.contains_key(key),
            result == $.get(key).unwrap()
        }
    {
        if !$.contains_key(key) {
            $.insert(key, value)
        }
        $.get(key).unwrap()
    }
}
```

#### 5.5.4 Search Guarantees

```cantrip
procedure binary_search<T: Ord>(arr: [T], target: T): Result<usize, usize>
    must is_sorted(arr)
    will match result {
        Ok(index) => {
            index < arr.len(),
            arr[index] == target
        },
        Err(index) => {
            index <= arr.len(),
            forall(|i| i < arr.len() => arr[i] != target)
        }
    }
{
    arr.binary_search(target)
}
```

#### 5.5.5 State Machine Transitions

```cantrip
record Connection {
    state: State

    procedure connect(mut $)
        must $.state == State::Disconnected
        will $.state == State::Connected
        uses net.tcp
    {
        // Perform connection
        $.state = State::Connected
    }

    procedure disconnect(mut $)
        must $.state == State::Connected
        will $.state == State::Disconnected
    {
        // Perform disconnection
        $.state = State::Disconnected
    }
}
```

### 5.6 Advanced Topics

#### 5.6.1 Quantified Postconditions

```cantrip
procedure map_all<T, U>(items: Vec<T>, f: map(T) -> U): Vec<U>
    will {
        result.len() == items.len(),
        forall(|i| i < result.len() =>
            result[i] == f(items[i])
        )
    }
{
    items.into_iter().map(f).collect()
}
```

#### 5.6.2 Frame Conditions

Postconditions can specify what didn't change:

```cantrip
procedure update_name(person: mut Person, name: String)
    will {
        person.name == name,
        person.age == @old(person.age),  // Frame: age unchanged
        person.id == @old(person.id)    // Frame: id unchanged
    }
{
    person.name = name
}
```

#### 5.6.3 Postconditions with Result Types

```cantrip
procedure parse_int(s: String): Result<i32, ParseError>
    will match result {
        Ok(n) => {
            n.to_string() == s.trim()
        },
        Err(e) => {
            !is_valid_int(s)
        }
    }
{
    s.trim().parse()
}
```

---

**Previous**: [Precondition Clauses](04_Preconditions.md) | **Next**: [Constraint Clauses](06_TypeConstraints.md)
