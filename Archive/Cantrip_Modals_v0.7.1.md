# Modals: State Machines as Types (Normative)

**Version:** 0.7.1  
**Date:** 2025-10-21

A **modal** defines a finite-state machine where each state has its own fields and only certain procedures are available. The type of a modal value encodes its current state: `T@State`.

---

## 1. Syntax

```cantrip
modal Name {
    state S1 { /* fields for S1 */ }
    state S2 { /* fields for S2 */ }

    @S1 >> op(args) >> @S2
        needs ε;
        requires P;
        ensures Q;
    {
        /* produce S2 (or tuple with S2) */
    }
}
```

- Use `@State >> procedure(...) >> @NextState`
- `>>` is the only valid transition arrow (reject `->` / `=>`, **E1006**)
- Procedures may preserve state (`@Open >> read() >> @Open`)

## 2. Example: File

```cantrip
modal File {
    state Closed { path: String; }
    state Open   { path: String; handle: FileHandle; position: usize; }

    @Closed >> open() >> @Open
        needs fs.read;
    {
        let h = FileSystem.open(self.path)?;
        Open { path: self.path, handle: h, position: 0 }
    }

    @Open >> read(n: usize) >> @Open
        needs fs.read;
    {
        let data = self.handle.read(n)?;
        self.position += data.len();
        (data, self)
    }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { path: self.path }
    }
}
```

## 3. State-dependent data and unions

Each state may have different fields. Procedures may accept/return **state unions**:

```cantrip
(@Submitted | @Draft) >> cancel(reason: String) >> @Cancelled { ... }
```

## 4. Common fields

States may share `common` fields (present in every state).

## 5. Formal structure (summary)

- Graph nodes are states; edges are labeled by procedures.  
- **Determinism:** at most one transition per `(state, procedure)` pair.  
- **Reachability:** compilers warn on unreachable states.  
- **Linear transfer:** after transition, the old state is consumed and replaced by the new one.

## 6. Invariants

States can declare invariants (`must ...;`) that must hold after construction and after every transition into that state.

```cantrip
state Active { balance: f64; must balance >= 0.0; }
```

## 7. Verification

### 7.1 Static

Transitions generate verification conditions from `requires/ensures` plus state invariants.

### 7.2 Runtime

In runtime verification mode, preconditions/postconditions and invariants produce checks at call boundaries.

## 8. Diagnostics

- **E3003** procedure not available in current state  
- **E1006** invalid arrow tokens in transitions  
- **E3004** use of moved value (ownership errors during modal flows)

---

**End of Modals v0.7.1**
