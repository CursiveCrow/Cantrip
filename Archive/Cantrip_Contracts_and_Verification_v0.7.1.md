# Contracts and Verification (Normative)

**Version:** 0.7.1  
**Date:** 2025-10-21

Cantrip integrates executable specifications: **preconditions** (`requires`), **postconditions** (`ensures`), and **invariants**. Contracts can be proved statically or checked at runtime.

---

## 1. Function contracts

```cantrip
function sqrt(x: f64): f64
    requires x >= 0.0;
    ensures
        result >= 0.0;
        abs(result * result - x) < 1e-10;
{
    x.sqrt()
}
```

- `result` denotes the return value in postconditions.
- `@old(expr)` captures the entry value of `expr`.

## 2. Messages

```cantrip
requires
    x >= 0, "x must be non-negative";
```

Custom messages improve diagnostics.

## 3. Invariants

### 3.1 Record invariants

```cantrip
record Account {
    balance: f64;
    invariant:
        balance >= 0.0;
}
```

Checked after construction and before/after public procedures.

### 3.2 Modal state invariants

```cantrip
state Committed {
    commit_id: u64;
    must commit_id > 0;
}
```

## 4. Loops

Loop bodies may declare invariants that must hold for every iteration.

```cantrip
var i = 0;
while i < n {
    invariant:
        0 <= i;
        i <= n;
    i += 1;
}
```

## 5. Verification modes

- **Static** (`--verify=static`) — compilation requires proofs.  
- **Runtime** (`--verify=runtime`, default) — checks inserted at boundaries.  
- **None** (`--verify=none`) — unchecked (unsafe).

## 6. Contract fuzzing (testing integration)

A test harness can be generated from contracts to exercise edge cases and found counterexamples.

## 7. Typed holes and verification

Typed holes (`?name: T`) fail under **static** verification (`E9501`). Under **runtime**, they elaborate to panics with precise locations (`E9502` warnings). Holes MUST NOT widen declared effects (`E9503`).

## 8. Diagnostics (selected)

- **E4001** cannot prove postcondition (static mode)  
- **E9501** unfilled typed hole (verify=static)  
- **E9502** typed hole elaborated to trap (verify=runtime)  
- **E9503** hole would widen effects beyond declaration

---

**End of Contracts & Verification v0.7.1**
