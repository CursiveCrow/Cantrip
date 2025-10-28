# Part IV: Contracts and Clauses - §4. Precondition Clauses

**Section**: §4 | **Part**: Contracts and Clauses (Part IV)
**Previous**: [Effect Clauses](03_Effects.md) | **Next**: [Postcondition Clauses](05_Postconditions.md)

---

**Definition 4.1 (Precondition Clause):** A precondition clause (`must`) is a predicate that must be true when a procedure is called. Preconditions specify the caller's obligations and define the valid input space for a procedure.

## 4. Precondition Clauses

### 4.1 Overview

**Key innovation/purpose:** Preconditions make procedure requirements explicit, enabling:
- **Input validation**: Specify valid inputs without defensive checks in every procedure
- **Contract-based programming**: Caller and callee responsibilities are clear
- **Static verification**: Many preconditions can be verified at compile time
- **Documentation**: Self-documenting valid input ranges
- **Optimization**: Compiler can assume preconditions hold

**When to use `must`:**
- Input values must satisfy constraints (e.g., `x > 0`)
- Pointers/references must be non-null
- Array indices must be in bounds
- Division by zero must be prevented
- Resources must be in valid states
- Relationships between parameters must hold

**When NOT to use `must`:**
- Validating untrusted external input → use Result types
- Type constraints (use where clauses instead)
- Effect requirements (use uses clause instead)
- Postconditions about results (use ensures instead)

**Relationship to other features:**
- **Postcondition Clauses (§5):** Preconditions of callee become obligations of caller
- **Constraint Clauses (§6):** Where clauses are always-true; must are call-specific
- **Effect Clauses (§3):** Preconditions are orthogonal to effects
- **Procedures**: Preconditions are checked at procedure entry

### 4.2 Syntax

#### 4.2.1 Concrete Syntax

**Precondition clause:**
```ebnf
MustClause ::= "must" PredicateList

PredicateList  ::= Predicate ("," Predicate)*

Predicate      ::= Expression
```

**Examples:**
```cantrip
// Single precondition
procedure sqrt(x: f64): f64
    must x >= 0.0
{
    x.sqrt()
}

// Multiple preconditions
procedure divide(a: i32, b: i32): i32
    must b != 0, a >= i32::MIN, b >= i32::MIN
{
    a / b
}

// Complex preconditions
procedure binary_search<T: Ord>(arr: [T], target: T): Option<usize>
    must is_sorted(arr)
{
    // Binary search implementation
    ...
}

// Preconditions on relationships
procedure transfer(from: mut Account, to: mut Account, amount: i64)
    must {
        amount > 0,
        from.balance >= amount,
        from.id != to.id
    }
{
    from.balance -= amount
    to.balance += amount
}
```

#### 4.2.2 Abstract Syntax

**Precondition representation:**
```
procedure f(x₁: τ₁, ..., xₙ: τₙ): τ must P(x₁, ..., xₙ);

Where:
  P = predicate over parameters
  P : (τ₁, ..., τₙ) → bool
```

**Precondition semantics:**
```
{P} f(args) {Q}

Interpretation:
  If P holds before calling f, then Q holds after f returns
  If P doesn't hold, behavior is undefined or panics
```

**Multiple preconditions:**
```
must P₁, P₂, ..., Pₙ;

Equivalent to:
  must P₁ ∧ P₂ ∧ ... ∧ Pₙ;
```

#### 4.2.3 Basic Examples

**Example 1: Numeric constraints**
```cantrip
procedure factorial(n: i32): i32
    must n >= 0, n <= 12  // Prevent overflow
{
    if n <= 1 {
        1
    } else {
        n * factorial(n - 1)
    }
}
```

**Example 2: Array bounds**
```cantrip
procedure get<T>(arr: [T], index: usize): T
    must index < arr.len()
    must T: Copy
{
    arr[index]  // Safe: precondition ensures bounds
}
```

**Example 3: Non-null precondition**
```cantrip
procedure process_data(data: *Data)
    must data != null  // Raw pointer must not be null
    uses unsafe.ptr
{
    unsafe {
        (*data).process()
    }
}
```

### 4.3 Static Semantics

#### 4.3.1 Well-Formedness Rules

**[WF-Requires] Well-formed precondition:**
```
[WF-Requires]
Γ, params ⊢ P : bool
P references only parameters and pure functions
─────────────────────────────────────────────────
must P well-formed
```

**Explanation:** Preconditions must be boolean expressions over parameters.

**[WF-RequiresProc] Well-formed procedure with precondition:**
```
[WF-RequiresProc]
procedure f(params): τ { body } well-formed
must P well-formed
P references only params
─────────────────────────────────────────────────
procedure f(params): τ must P { body } well-formed
```

**Explanation:** Preconditions can only reference procedure parameters and pure helper functions.

**[WF-RequiresPure] Preconditions must be pure:**
```
[WF-RequiresPure]
must P;
effects(P) = ∅
─────────────────────────────────────────────────
must P well-formed
```

**Explanation:** Preconditions cannot have side effects.

#### 4.3.2 Type Rules

**[T-PrecondCall] Procedure call with precondition:**
```
[T-PrecondCall]
Γ ⊢ f : map(τ₁, ..., τₙ) -> τ must P
Γ ⊢ args : τ₁, ..., τₙ
Γ ⊢ P[params ↦ args] : bool
─────────────────────────────────────────────────
Γ ⊢ f(args) : τ
obligation: P[params ↦ args] must hold
```

**Explanation:** Calling a procedure with precondition P obligates the caller to ensure P holds for the arguments.

**[T-PrecondVerify] Static precondition verification:**
```
[T-PrecondVerify]
procedure f must P
caller context proves P[params ↦ args]
─────────────────────────────────────────────────
precondition statically verified (no runtime check)
```

**Explanation:** If the compiler can prove the precondition holds, no runtime check is needed.

**[T-PrecondCheck] Dynamic precondition checking:**
```
[T-PrecondCheck]
procedure f must P
compiler cannot prove P[params ↦ args]
─────────────────────────────────────────────────
runtime check inserted: assert!(P[params ↦ args])
```

**Explanation:** If the precondition cannot be proven statically, a runtime check is inserted.

**[T-PrecondStrengthen] Precondition strengthening:**
```
[T-PrecondStrengthen]
contract T { procedure m must P₁; }
record Type: T { procedure m must P₂ { ... } }
P₂ ⇒ P₁  (P₂ stronger than P₁)
─────────────────────────────────────────────────
ERROR: Cannot strengthen preconditions
```

**Explanation:** Implementations cannot have stronger preconditions than their trait declarations (Liskov substitution principle).

#### 4.3.3 Type Properties

**Theorem 3.1 (Precondition Soundness):**

If a procedure's precondition holds at call time, the procedure executes safely:

```
Γ ⊢ f(args) must P
P[params ↦ args] = true
─────────────────────────────────────────────────
f(args) executes without precondition violation
```

**Proof sketch:** By precondition checking. Either statically verified or dynamically checked at runtime.

**Theorem 3.2 (Precondition Obligation Transfer):**

Calling a procedure with precondition transfers the obligation to the caller:

```
procedure f must P { body }
procedure g { ... f(args) ... }
─────────────────────────────────────────────────
g must ensure P[params ↦ args] before calling f
```

**Proof sketch:** By [T-PrecondCall]. The type system enforces that callers satisfy callee preconditions.

**Theorem 3.3 (Static Elimination):**

Provably satisfied preconditions have zero runtime cost:

```
procedure f(x: i32) must x > 0 { ... }
procedure g() { f(5); }  // x > 0 provable
─────────────────────────────────────────────────
No runtime check needed in g
```

**Proof sketch:** Compiler can prove 5 > 0 statically, eliminating the check.

**Property 3.1 (Precondition Weakening):**

Callees can weaken (relax) preconditions in compatible ways:

```
contract T { procedure m(x: i32) must x > 0; }

record Type: T { procedure m(x: i32) must x >= 0; }  // Weaker - OK
```

**Property 3.2 (Precondition Independence):**

Preconditions are independent of effects and postconditions:

```cantrip
procedure f(x: i32)
    uses io.write
    must x > 0
    ensures result > x
{
    ...
}
```

Each clause serves a distinct purpose.

### 4.4 Dynamic Semantics

#### 4.4.1 Evaluation Rules

**[E-PrecondSatisfied] Precondition satisfied:**
```
[E-PrecondSatisfied]
procedure f(params) must P { body }
⟨P[params ↦ args], σ⟩ ⇓ ⟨true, σ⟩
⟨body[params ↦ args], σ⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────
⟨f(args), σ⟩ ⇓ ⟨v, σ'⟩
```

**Explanation:** If precondition holds, proceed with body execution.

**[E-PrecondViolated] Precondition violated:**
```
[E-PrecondViolated]
procedure f(params) must P { body }
⟨P[params ↦ args], σ⟩ ⇓ ⟨false, σ⟩
─────────────────────────────────────────────────
⟨f(args), σ⟩ ⇓ panic("precondition violated: P")
```

**Explanation:** If precondition doesn't hold, execution panics.

**[E-PrecondStatic] Statically verified precondition:**
```
[E-PrecondStatic]
procedure f(params) must P { body }
compiler proves P[params ↦ args]
⟨body[params ↦ args], σ⟩ ⇓ ⟨v, σ'⟩
─────────────────────────────────────────────────
⟨f(args), σ⟩ ⇓ ⟨v, σ'⟩  (no runtime check)
```

**Explanation:** Statically proven preconditions skip runtime checking.

#### 4.4.2 Checking Strategies

**Static verification (compile-time):**

The compiler attempts to prove preconditions using:
- Constant propagation
- Range analysis
- Flow-sensitive typing
- SMT solvers (optionally)

```cantrip
procedure needs_positive(x: i32) must x > 0 { ... }

procedure caller() {
    needs_positive(5)   // Proven: 5 > 0 ✓
    let y = 10
    needs_positive(y)   // Proven: 10 > 0 ✓
}
```

**Dynamic checking (runtime):**

When static proof fails, insert runtime checks:

```cantrip
procedure sqrt(x: f64) must x >= 0.0 { ... }

procedure compute(input: f64) {
    sqrt(input)  // Runtime check: assert!(input >= 0.0)
}

// Compiles to:
procedure compute(input: f64) {
    if !(input >= 0.0) {
        panic!("precondition violated: x >= 0.0")
    }
    sqrt(input)
}
```

**Optimization levels:**

```
--checks=none     : Skip all dynamic checks (unsafe, for production after testing)
--checks=debug    : Check in debug builds only
--checks=always   : Always check (default)
--checks=verify   : Use formal verification (must proof annotations)
```

#### 4.4.3 Error Messages

**Precondition violation at runtime:**
```
thread 'main' panicked at 'precondition violated: denominator != 0'
  must b != 0;
  ^^^^^^^^^^^^^^^
note: in procedure `divide` at src/math.ct:45:5
note: called from `calculate` at src/main.ct:12:9
```

**Static verification failure (warning):**
```
warning: cannot verify precondition statically
  --> src/main.ct:15:5
   |
15 |     divide(x, y)
   |     ^^^^^^^^^^^^ precondition `y != 0` cannot be proven
   |
   = note: runtime check will be inserted
   = help: consider adding: must y != 0;
```

### 4.5 Examples and Patterns

#### 4.5.1 Array Access

```cantrip
procedure safe_get<T>(arr: [T], index: usize): T
    must index < arr.len()
    must T: Copy
{
    arr[index]
}

procedure process_array(data: [i32]) {
    for i in 0..data.len() {
        let item = safe_get(data, i)  // Proven: i < data.len()
        println("{}", item)
    }
}
```

#### 4.5.2 Division Safety

```cantrip
procedure safe_divide(a: i32, b: i32): i32
    must b != 0
{
    a / b
}

procedure calculate(x: i32, y: i32): Option<i32> {
    if y != 0 {
        Some(safe_divide(x, y))  // Proven in this branch
    } else {
        None
    }
}
```

#### 4.5.3 Resource State

```cantrip
record File {
    handle: FileHandle
    is_open: bool

    procedure read($): Result<String, Error>
        must $.is_open
        uses io.read, alloc.heap
    {
        // Safe: file is guaranteed open
        $.handle.read_to_string()
    }

    procedure write($, data: String): Result<(), Error>
        must $.is_open
        uses io.write
    {
        $.handle.write_all(data.as_bytes())
    }
}
```

#### 4.5.4 Sorted Data Structures

```cantrip
record SortedVec<T: Ord> {
    elements: Vec<T>

    where {
        is_sorted(elements)
    }

    procedure binary_search($, target: T): Result<usize, usize>
        must is_sorted($.elements)  // Redundant with where, but explicit
    {
        $.elements.binary_search(target)
    }
}
```

#### 4.5.5 Temporal Preconditions

```cantrip
record Transaction {
    started: Instant
    completed: Option<Instant>

    procedure commit(mut $)
        must $.completed.is_none()
    {
        $.completed = Some(Instant::now())
    }

    procedure rollback(mut $)
        must $.completed.is_none()
    {
        // Can only rollback active transactions
        ...
    }
}
```

### 4.6 Advanced Topics

#### 4.6.1 Precondition Inference

The compiler can infer preconditions from procedure bodies:

```cantrip
procedure inferred_precond(x: i32, y: i32): i32 {
    x / y  // Compiler infers: must y != 0
}

// Warning: Missing explicit precondition
// help: add: must y != 0;
```

#### 4.6.2 Preconditions with Quantifiers

```cantrip
procedure all_positive(numbers: [i32]): bool
    must numbers.len() > 0
{
    numbers.iter().all(|n| n > 0)
}

procedure process_positive_list(nums: [i32])
    must {
        nums.len() > 0,
        forall(|i| i < nums.len() => nums[i] > 0)
    }
{
    // All numbers guaranteed positive
    ...
}
```

#### 4.6.3 Frame Conditions

Preconditions can specify framing constraints:

```cantrip
procedure update_field(obj: mut Record, value: i32)
    must obj.state == State::Active
{
    obj.value = value
    // obj.state remains Active (frame condition)
}
```

#### 4.6.4 Preconditions in Generic Code

```cantrip
procedure binary_search_generic<T, F>(arr: [T], predicate: F): Option<usize>
    where F: Fn(T) -> bool
    must {
        arr.len() > 0,
        is_partitioned(arr, predicate)  // arr partitioned by predicate
    }
{
    // Binary search implementation
    ...
}
```

---

**Previous**: [Effect Clauses](03_Effects.md) | **Next**: [Postcondition Clauses](05_Postconditions.md)
