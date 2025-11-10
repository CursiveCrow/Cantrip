# Cursive Language Specification

## Clause 11 — Memory Model, Regions, and Permissions

**Part**: XI — Memory Model, Regions, and Permissions
**File**: 11-5_Move-Copy-Clone.md  
**Section**: §11.5 Move, Copy, and Clone Semantics  
**Stable label**: [memory.move]  
**Forward references**: §11.2 [memory.object], §11.4 [memory.permission], Clause 5 §5.2 [decl.variable], Clause 7 [type], Clause 10 [generic]

---

### §11.5 Move, Copy, and Clone Semantics [memory.move]

#### §11.5.1 Overview

[1] This section specifies how values transfer between bindings, when cleanup responsibility transfers, and how types opt into copying behavior. Cursive uses explicit `move` for responsibility transfer and explicit `Copy`/`Clone` predicates for duplication.

[2] The binding category (`let` vs `var`) combined with assignment operator (`=` vs `<-`) determines transfer behavior. Permissions (§11.4) are orthogonal to transfer semantics.

#### §11.5.2 Move Semantics [memory.move.move]

##### §11.5.2.1 Overview

[3] The `move` keyword explicitly transfers cleanup responsibility from one binding to another. After a move, the source binding becomes invalid.

##### §11.5.2.2 Syntax

[4] Move expression syntax:

**Move expressions** match the pattern:
```
"move" <identifier>
```

[ Note: See Annex A §A.4 [grammar.expression] for complete move syntax.
— end note ]

##### §11.5.2.3 Transfer Rules by Binding Category

[5] **Only `let` bindings created with `=` can transfer cleanup responsibility:**

**Table 11.5 — Transfer rules**

| Binding Form     | Responsible | Transferable | `move` Validity              |
| ---------------- | ----------- | ------------ | ---------------------------- |
| `let x = value`  | YES         | YES          | Valid; x becomes invalid     |
| `var x = value`  | YES         | NO           | Invalid (E11-501)            |
| `let x <- value` | NO          | NO           | Invalid (E11-502)            |
| `var x <- value` | NO          | NO           | Invalid (E11-502)            |

[6] Only `let` bindings created with `=` can transfer cleanup responsibility via `move`. This restriction preserves local reasoning: the validity of a `var` binding does not depend on whether it was moved somewhere else in the function. Use `let` with `unique` permission for values that need both mutation and transfer capability.

##### §11.5.2.4 Move Semantics

[7] Move execution:

[ Given: Source binding $x$ with cleanup responsibility ]

$$
\frac{\Gamma \vdash x : \tau \quad x \text{ is responsible and transferable}}{\Gamma \vdash \texttt{move } x : \tau \quad x \text{ becomes invalid}}
\tag{E-Move}
$$

[8] After `move x`:

- Cleanup responsibility transfers to the receiving binding
- `x` becomes invalid (moved-from state)
- Using `x` produces diagnostic E11-503 (use of moved value)

**Example 11.5.2.1 (Move transfers responsibility):**

```cursive
procedure consume(data: Buffer)
    [[ |- true => true ]]
{
    // data is responsible; will call destructor at end
}

procedure demo()
    [[ alloc::heap |- true => true ]]
{
    let buffer = Buffer::new()      // buffer is responsible
    consume(move buffer)             // Responsibility transferred to parameter
    // buffer is now invalid
    // buffer.size()                 // error[E11-503]: use of moved value
}
```

**Example 11.5.2.2 - invalid (Move from var):**

```cursive
var counter = Buffer::new()
consume(move counter)  // error[E11-501]: cannot transfer from var binding
```

[1] `var` bindings cannot transfer because allowing moves would require whole-function analysis to track whether the binding might be rebound after the move, violating Cursive's local reasoning principle.

#### §11.5.3 Copy Semantics [memory.move.copy]

##### §11.5.3.1 Copy Predicate

[9] Types satisfying the `Copy` predicate can be duplicated bitwise. Copying creates a new independent object with its own cleanup responsibility.

[10] **Copy behavior**:

[ Note: The `Copy` predicate is defined in Clause 10 (Generics and Predicates). This section specifies how Copy interacts with the memory model.
— end note ]

[11] Types that satisfy `Copy`:

- All primitive types (§7.2): integers, floats, bool, char, ()
- Tuples and records where all fields satisfy `Copy`
- Types without custom destructors

[12] Types that do NOT satisfy `Copy`:

- Types with custom destructors
- Types containing non-Copy fields
- Most resource-managing types (File, Socket, Buffer)

##### §11.5.3.2 Copy Syntax

[13] Copying requires explicit `copy` keyword:

```cursive
let original: i32 = 42
let duplicate = copy original    // Explicit bitwise copy
```

[14] Attempting to copy non-Copy types produces diagnostic E11-510.

**Example 11.5.3.1 (Copy for primitive types):**

```cursive
let a: i32 = 10
let b = copy a       // OK: i32 satisfies Copy
let c = copy a       // OK: can copy multiple times

// All three bindings (a, b, c) are responsible for their own values
```

#### §11.5.4 Clone Semantics [memory.move.clone]

##### §11.5.4.1 Clone Predicate

[15] The `Clone` predicate enables deep copying for types that cannot use bitwise `Copy`. Clone creates a semantically equivalent but independent object.

[ Note: The `Clone` predicate is defined in Clause 10 (Generics and Predicates).
— end note ]

[16] Types implement `Clone` by providing a `clone` method that performs type-specific duplication (allocating new resources, copying contents, etc.).

**Example 11.5.4.1 (Clone for complex types):**

```cursive
let original: Buffer = Buffer::from_data(data)
let cloned = original.clone()    // Deep copy: allocates new buffer

// Both original and cloned are responsible for their own buffers
```

#### §11.5.5 Moved-From State [memory.move.movedFrom]

##### §11.5.5.1 Invalid Binding State

[17] After `move x`, the binding `x` enters moved-from state. Definite assignment analysis (§5.7) tracks this state and prevents further use.

[18] **Moved-from constraint**:

[ Given: Binding $x$ in moved-from state ]

$$
\frac{x \text{ in moved-from state} \quad \text{attempt to use } x}{\text{ERROR E11-503: use of moved value}}
\tag{WF-No-Use-After-Move}
$$

##### §11.5.5.2 Invalidation of Derived Non-Responsible Bindings

[18.1] Non-responsible bindings reference the object, not the binding. They become invalid when the object **might be destroyed**. The compiler uses **parameter responsibility** to determine when destruction might occur: moving a binding to a **responsible parameter** (marked with `move` modifier, §5.4.3[2.1]) signals that the callee might destroy the object.

[ Given: Non-responsible binding $n$ referencing object via source binding $r$, procedure parameter with responsibility $\rho$ ]

$$
\frac{r \text{ moved to parameter with } \rho = \text{responsible}}
     {r \text{ and } n \text{ both become invalid}}
\tag{WF-NonResp-Invalidate-OnMoveToResponsible}
$$

[18.2] **Key distinction**: Parameter responsibility determines object lifetime:
- **Non-responsible parameter** (no `move`): Callee will NOT destroy object, references remain valid
- **Responsible parameter** (`move`): Callee MIGHT destroy object, references become invalid

[18.3] Definite assignment analysis maintains a dependency graph tracking which non-responsible bindings reference which objects (via their source bindings). When a binding is moved to a responsible parameter, invalidation propagates to all dependent non-responsible bindings.

[18.4] Accessing an invalidated non-responsible binding produces diagnostic E11-504 (use of invalidated reference to potentially destroyed object).

**Example 11.5.5.2 (Invalidation based on parameter responsibility):**

```cursive
procedure inspect(data: Buffer)        // Non-responsible parameter
    [[ |- true => true ]]
{
    println("Size: {}", data.size())
    // data.drop() NOT called
}

procedure consume(move data: Buffer)   // Responsible parameter
    [[ |- true => true ]]
{
    data.process()
    // data.drop() IS called at scope exit
}

let owner = Buffer::new()              // Responsible
let viewer <- owner                    // Non-responsible (references object)
let another <- owner                   // Another non-responsible (references object)

inspect(owner)                         // ✅ Pass to non-responsible param
viewer.read()                          // ✅ VALID: object survived inspect()
another.read()                         // ✅ VALID: object still alive

consume(move owner)                    // Move to responsible param
// owner becomes invalid (moved-from state)
// viewer becomes invalid (object might be destroyed)
// another becomes invalid (object might be destroyed)

// viewer.read()                       // ERROR E11-504: invalidated reference
// another.read()                      // ERROR E11-504: invalidated reference
```

[18.5] **Design rationale**: This invalidation strategy balances safety and zero-cost:
- **Safe**: References cannot access potentially destroyed objects
- **Zero-cost**: Uses parameter responsibility (compile-time information) not runtime tracking
- **Local reasoning**: Procedure signatures declare parameter responsibility, making destruction potential visible
- **Expressive**: Non-responsible parameters enable safe multiple-view patterns

[ Note: The compiler approximates "object destruction" with "moved to responsible parameter" because it cannot track actual destruction without runtime overhead or whole-program analysis. This conservative approximation rejects some safe programs (e.g., where a responsible parameter returns the object without destroying it) but maintains memory safety with zero runtime cost.
— end note ]

##### §11.5.5.3 Rebinding Var Without Transfer

[19] While `var` bindings cannot be moved (they are non-transferable per Table 11.5), they can be reassigned in place, with the old value being destroyed before the new value is bound:

**Example 11.5.5.1 (Var rebinding without move):**

```cursive
var data = Buffer::new()      // data is responsible
// consume(move data)         // error[E11-501]: cannot transfer from var

// However, var bindings can be reassigned:
data = Buffer::new()           // Old buffer destroyed, new buffer bound
data.size()                    // OK: data is valid with new buffer
```

[20] When rebinding a `var`, the old value (if valid) is automatically cleaned up before the new value is assigned. This is distinct from transfer: the binding retains cleanup responsibility for both the old and new values.

**Example 11.5.5.2 (Pattern: Use let + unique for mutable+transferable):**

```cursive
// When you need both mutation AND transfer:
let builder: unique = Builder::new()   // Use let + unique, not var
builder.add(1)                         // Can mutate via unique
builder.add(2)
consume(move builder)                  // Can transfer via let
```

**Example 11.5.5.3 (Move from let binding):**

```cursive
let data = Buffer::new()     // let binding is transferable
consume(move data)           // OK: responsibility transferred
// data.size()               // error[E11-503]: use of moved value
// data = Buffer::new()      // error[E05-202]: cannot reassign let binding
```

[20] `let` bindings cannot be reassigned (§5.2); once moved, they remain invalid for their entire scope.

#### §11.5.6 Diagnostics

[21] Transfer semantics diagnostics:

| Code    | Condition                                            |
| ------- | ---------------------------------------------------- |
| E11-501 | Attempt to move from `var` binding                   |
| E11-502 | Attempt to move from non-responsible binding (`<-`)  |
| E11-503 | Use of moved value                                   |
| E11-504 | Use of non-responsible binding derived from moved value |
| E11-510 | Attempt to copy non-Copy type                        |
| E11-511 | Attempt to clone non-Clone type                      |

#### §11.5.7 Conformance Requirements

[22] Implementations shall:

1. Require explicit `move` keyword for responsibility transfer
2. Allow `move` only from `let` bindings created with `=`
3. Reject `move` from `var` bindings with diagnostic E11-501
4. Reject `move` from non-responsible bindings (`<-`) with diagnostic E11-502
5. Track moved-from state and prevent use-after-move (E11-503)
6. Track non-responsible binding dependencies via definite assignment analysis (which objects they reference)
7. Propagate invalidation: when a binding is moved to a responsible parameter, invalidate the binding and all derived non-responsible bindings
8. Preserve validity: when a binding is passed to a non-responsible parameter (no move), non-responsible bindings remain valid
9. Prevent use of invalidated non-responsible bindings with diagnostic E11-504
10. Support `Copy` predicate for bitwise duplication (reference Clause 10)
11. Support `Clone` predicate for deep copying (reference Clause 10)
12. Integrate move tracking with definite assignment analysis (§5.7)
13. Maintain orthogonality: transfer rules independent of permissions
14. Preserve local reasoning: invalidation determined by parameter responsibility (visible in procedure signature)

---

**Previous**: §11.4 Permissions (§11.4 [memory.permission]) | **Next**: §11.6 Layout and Alignment (§11.6 [memory.layout])
