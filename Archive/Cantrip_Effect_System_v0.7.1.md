# The Cantrip Effect System (Normative)

**Version:** 0.7.1  
**Date:** 2025-10-21

This document is the normative reference for effects, including syntax, rules, budgets, higher-order propagation, async masking, and diagnostics.

---

## 1. Overview

An **effect** denotes any observable interaction beyond pure computation. Cantrip tracks at compile time whether a function may allocate memory, perform I/O, observe time, spawn threads, panic, and more.

**Principles**: (1) explicit over implicit; (2) local reasoning; (3) zero-cost when pure.

> **Rule 1 — Explicitness.** Effects are **never inferred**. A function is pure unless it declares `needs ...;`.

## 2. Syntax

### 2.1 Function header

```cantrip
function f(params): T
    needs EffectExpr;   // optional if pure
{
    body
}
```

### 2.2 Effect expressions

```
EffectExpr ::= ε | ε + ε | ε - ε | !ε | name | (EffectExpr)
ε ::= alloc.heap | alloc.stack | alloc.region | alloc.temp
    | fs.read | fs.write | fs.delete | fs.metadata
    | net.read(inbound) | net.read(outbound) | net.write
    | io.write
    | time.read | time.sleep
    | thread.spawn | thread.join | thread.atomic
    | render.draw | render.compute
    | audio.play | input.read
    | process.spawn | ffi.call | unsafe.ptr
    | panic | async | random
```

**Union** `+`, **difference** `-`, **negation** `!` are set operations over effect atoms. Parentheses group as usual.

### 2.3 Named effects

```cantrip
public effect WebService = fs.* + net.* + alloc.heap;
effect ReadOnlyFS = fs.read + fs.metadata;
effect SafeIO     = fs.read + fs.write + alloc.heap;
```

- Named effects expand inline during checking.  
- Visibility follows normal module rules.  
- Named effects can reference other named effects.

### 2.4 Wildcards

```
alloc.* = {alloc.heap, alloc.stack, alloc.region, alloc.temp}
fs.*    = {fs.read, fs.write, fs.delete, fs.metadata}
net.*   = {net.read(inbound), net.read(outbound), net.write}
io.*    = {io.write}
```

## 3. Checking and subsumption

If a function body has actual effects `ε_actual`, the declaration must over-approximate: `ε_actual ⊆ ε_declared`. Otherwise **E9001** (missing effect) or **E9002** (forbidden effect) is emitted with a fix‑it.

### 3.1 Forbidden effects — **only two valid uses**

1) **Wildcard restriction**

```cantrip
function safe_fs()
    needs fs.*, !fs.delete;
{ ... }
```

2) **Polymorphic constraint** (with callback effects)

```cantrip
function deterministic_map<F>(xs: [i32], f: F): Vec<i32>
    where F: Fn(i32): i32
    needs effects(F), !time.read, !random, alloc.heap;
{ ... }
```

Any other forbids are **E9010** (redundant). Declare only what you need otherwise.

### 3.2 Trait-declared effect bounds (upper bounds)

```cantrip
public trait Logger {
    function log(self: Self, msg: String)
        needs io.write;
}

impl Logger for MyLog {
    function log(self: Self, msg: String)
        needs io.write; // OK (≤ bound)
    { std.io.println(msg); }
}

impl Logger for BadLog {
    function log(self: Self, msg: String)
        needs io.write, fs.write; // E9120 exceeds bound
    { std.fs.append("log.txt", msg) }
}
```

### 3.3 Higher‑order propagation

```cantrip
function map<T,U,F>(xs: [T], f: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), alloc.heap;
{ ... }
```

`effects(f)` is the exact set of effects of `f` used to check the caller.

## 4. Async effect masks

Awaiting a future brings that future’s effects into the enclosing async function. Missing atoms in the header cause **E9201** with a fix‑it suggestion.

```cantrip
async function fetch(url: str): String
    needs alloc.heap; // missing net.read(outbound)
{
    let body = await http.get(url)?; // E9201
    body
}
```

## 5. Budgets (quantitative constraints)

Budgets can bound memory, time, and operation counts. They may be statically verified, dynamically tracked, or both.

```cantrip
function small(): Vec<u8>
    needs alloc.heap(bytes<=1024);
{ Vec.with_capacity(100) } // OK
```

### 5.1 Memory budgets

- `alloc.heap(bytes<=N)` (also `KiB<=`, `MiB<=`)  
- Static proof when sizes are compile‑time known; otherwise dynamic tracking is inserted.

### 5.2 Time budgets

- `time.sleep(duration<=500ms)`  
- Total sleep in the function must not exceed the bound (checked statically when provable or dynamically).

### 5.3 Count budgets

- `thread.spawn(count<=8)` to limit thread creation.  
- Additive composition across calls.

## 6. Taxonomy (canonical, exhaustive)

```
All Effects (*)
├── alloc.* → heap | stack | region | temp
├── fs.*    → read | write | delete | metadata
├── net.*   → read(inbound) | read(outbound) | write
├── io.*    → write
├── time.*  → read | sleep
├── thread.*→ spawn | join | atomic
├── render.*→ draw | compute
├── audio.* → play
├── input.* → read
├── process.* → spawn
├── ffi.*     → call
├── unsafe.*  → ptr
├── panic
├── async
└── random
```

## 7. Soundness summaries

- **Effect Soundness.** If `Γ ⊢ e : T ! ε` then every evaluation of `e` performs a subset of `ε`.
- **Budget Compliance.** If `alloc.heap(bytes<=N)` is declared, total allocation by the function is ≤ `N`.

## 8. Diagnostics (non‑exhaustive)

- **E9001** Missing effect declaration  
- **E9002** Forbidden effect used  
- **E9010** Redundant forbidden effect (outside wildcard/polymorphic cases)  
- **E9120** Trait impl exceeds method’s effect bound  
- **E9201** Async `await` imports undeclared effects

---

**End of Effect System v0.7.1**
