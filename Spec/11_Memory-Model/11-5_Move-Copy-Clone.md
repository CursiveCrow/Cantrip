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

```ebnf
move_expr
    ::= "move" identifier
```

[ Note: See Annex A §A.4 [grammar.expression] for complete move syntax.
— end note ]

##### §11.5.2.3 Transfer Rules by Binding Category

[5] **Transferability depends on binding category:**

**Table 11.5 — Transfer rules**

| Binding Form     | Responsible | Transferable | Move Validity              |
| ---------------- | ----------- | ------------ | -------------------------- |
| `let x = value`  | YES         | YES          | `move x` valid             |
| `var x = value`  | YES         | NO           | `move x` invalid (E11-501) |
| `let x <- value` | NO          | NO           | `move x` invalid (E11-502) |
| `var x <- value` | NO          | NO           | `move x` invalid (E11-502) |

[6] Only `let` bindings created with `=` can transfer cleanup responsibility via `move`.

##### §11.5.2.4 Move Semantics

[7] Move execution:

[ Given: Source binding $x$ with cleanup responsibility, target binding $y$ ]

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
var counter = 0
let moved = move counter  // error[E11-501]: cannot transfer from var binding
```

#### §11.5.3 Copy Semantics [memory.move.copy]

##### §11.5.3.1 Copy Predicate

[9] Types satisfying the `Copy` predicate can be duplicated bitwise. Copying creates a new independent object with its own cleanup responsibility.

[10] **Copy predicate**:

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

##### §11.5.5.2 Reassignment of Var Bindings

[19] While `var` bindings cannot be moved (they are non-transferable per Table 11.5), they can be reassigned to restore validity after other forms of invalidation:

**Example 11.5.5.1 (Reassignment of var after move-like invalidation):**

```cursive
var data = Buffer::new()
// NOTE: Cannot move from var bindings (E11-501)
// consume(move data)        // error[E11-501]: cannot transfer from var binding

// However, var bindings can be reassigned after being invalidated by other means
data = Buffer::new()         // Reassignment restores validity
data.size()                  // OK: data is valid again
```

**Example 11.5.5.2 (Move from let binding):**

```cursive
let data = Buffer::new()     // let binding is transferable
consume(move data)           // OK: responsibility transferred
// data.size()               // error[E11-503]: use of moved value
// data = Buffer::new()      // error[E05-202]: cannot reassign let binding
```

[20] `let` bindings cannot be reassigned (§5.2); once moved, they remain invalid for their entire scope.

#### §11.5.6 Diagnostics

[21] Transfer semantics diagnostics:

| Code    | Condition                                           |
| ------- | --------------------------------------------------- |
| E11-501 | Attempt to move from `var` binding                  |
| E11-502 | Attempt to move from non-responsible binding (`<-`) |
| E11-503 | Use of moved value                                  |
| E11-510 | Attempt to copy non-Copy type                       |
| E11-511 | Attempt to clone non-Clone type                     |

#### §11.5.7 Conformance Requirements

[22] Implementations shall:

1. Require explicit `move` keyword for responsibility transfer
2. Allow `move` only from `let` bindings created with `=`
3. Reject `move` from `var` and `<-` bindings (E11-501, E11-502)
4. Track moved-from state and prevent use-after-move (E11-503)
5. Support `Copy` predicate for bitwise duplication (reference Clause 10)
6. Support `Clone` predicate for deep copying (reference Clause 10)
7. Integrate move tracking with definite assignment analysis (§5.7)
8. Maintain orthogonality: transfer rules independent of permissions

---

**Previous**: §11.4 Permissions (§11.4 [memory.permission]) | **Next**: §11.6 Layout and Alignment (§11.6 [memory.layout])
