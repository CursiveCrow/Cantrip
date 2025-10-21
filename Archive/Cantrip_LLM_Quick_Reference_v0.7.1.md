# Cantrip Language — LLM Quick Reference (Informative)

**Version:** 0.7.1 (compatible with 0.6.x)  
**Date:** 2025-10-21  
**Status:** Informative companion to the normative specification

---

This quick reference distills the *Cantrip* programming language into stable, machine-friendly idioms for code generation and review. It favors a single canonical way to express each concept and pairs it with anti-patterns and diagnostics.

> **Scope.** This document is an informative guide. The normative rules are the Cantrip Language Specification v0.7.1.

## 0. Core canonical syntax

### 0.1 Function declaration

**Canonical form (single, supported syntax):**
```cantrip
function name(params): ReturnType
    needs effects;        // optional if pure
    requires conditions;  // optional
    ensures conditions;   // optional
{
    body
}
```

**Correct examples:**
```cantrip
function add(x: i32, y: i32): i32 { x + y }

function divide(a: f64, b: f64): f64
    requires b != 0.0;
    ensures result == a / b;
{
    a / b
}
```

**Removed/invalid forms (compile errors):**
```cantrip
// E1010: removed effect syntax
function f()
    requires<alloc.heap>:
{ }

// E1011: removed returns<T> syntax
function g() returns<i32>:
{ }

// E1012: removed implements: header
function h()
    implements:
{ }
```

### 0.2 Ownership and `move`

Ownership transfer is explicit:

```cantrip
function consume(own data: Data) { }

function demo()
    needs alloc.heap;
{
    let data = Data.new();
    consume(move data);     // E3004 if 'move' omitted
}
```

### 0.3 Effects are explicit (never inferred)

Declare **all** effects performed by a function; otherwise it is pure.

```cantrip
function make_vec(n: usize): Vec<i32>
    needs alloc.heap;
{
    Vec.with_capacity(n)
}
```

Named effects may be defined and reused:

```cantrip
effect FileIO = fs.read + fs.write + alloc.heap;

function load_and_save(path: String): Result<(), Error>
    needs FileIO;
{
    let s = std.fs.read_to_string(path)?;
    std.fs.write(path + ".bak", s)
}
```

> **Forbidden-effect diagnostics.**
>
> - Wildcard restriction is valid: `needs fs.*, !fs.delete;`
> - Polymorphic constraint is valid: `needs effects(F), !time.read;`
> - Redundant forbids (e.g., `needs fs.read, !fs.delete;`) are **E9010**.

### 0.4 Modals (state machines as types)

Use `@State >> procedure(...) >> @Next` for transitions.

```cantrip
modal File {
    state Closed { path: String; }
    state Open   { path: String; handle: FileHandle; }

    @Closed >> open() >> @Open
        needs fs.read;
    {
        let h = FileSystem.open(self.path)?;
        Open { path: self.path, handle: h }
    }

    @Open >> close() >> @Closed {
        self.handle.close();
        Closed { path: self.path }
    }
}
```

Invalid arrow tokens are rejected:
```cantrip
// E1006
Start -> do() -> End { }     // use '>>' and '@State' markers
```

### 0.5 Regions (arena allocation)

Bulk deallocation with lexical scope. Region values cannot escape.

```cantrip
function parse(input: String): Result<AST, Error>
    needs alloc.region, alloc.heap;
{
    region temp {
        let toks = lex_in<temp>(input);
        let ast = parse_in<temp>(toks);
        Ok(ast.to_heap())
    }
}

// E5001 if region data escapes:
function broken(): Vec<i32>
    needs alloc.region;
{
    region temp {
        let v = Vec.new_in<temp>();
        v   // E5001
    }
}
```

### 0.6 Contracts (pre/post)

```cantrip
function transfer(from: mut Account, to: mut Account, amount: f64)
    requires
        amount > 0.0,
            "amount must be positive";
        from.balance >= amount,
            "need {amount}, have {from.balance}";
    ensures
        from.balance == @old(from.balance) - amount;
        to.balance   == @old(to.balance) + amount;
{
    from.balance -= amount;
    to.balance   += amount;
}
```

### 0.7 Pattern matching uses `->` (not `=>`)

```cantrip
match result {
    Ok(x)  -> handle(x),
    Err(e) -> handle_err(e),
}
```

### 0.8 Typed holes (dev-time only)

```cantrip
function parse(line: String): Result<Item, Error>
    needs alloc.heap;
{
    let name = ?n: String;  // typed hole (E9501 in verify=static)
    ???                     // untyped hole, type '!' (diverges)
}
```

## 1. Error codes most often encountered

| Code   | Category | Meaning / Fix |
|--------|----------|----------------|
| E9001  | Effect   | Missing `needs` declaration; add required effects. |
| E9010  | Effect   | Redundant/invalid forbidden effect; only valid with wildcards or polymorphic constraints. |
| E9120  | Effect   | Trait method impl exceeds trait’s declared effect bound; narrow effects or loosen trait. |
| E9201  | Effect   | Async `await` imports effects not declared; add them to `needs`. |
| E3003  | Modal    | Procedure not available in current state. |
| E3004  | Modal    | Use of moved value (forgot `move`). |
| E5001  | Region   | Region-allocated value escapes its region. |
| E1010–12 | Syntax | Removed legacy syntaxes from pre-0.7. |

## 2. Standard effects to prefer

```cantrip
import std.effects.{Pure, SafeIO, FileIO, NetworkIO, WebService, GameTick, RealTime, Deterministic};

// Pure is optional (documentation aid):
function checksum(data: [u8]): u32 { ... }
```

- **Pure**: no effects (marker, optional)  
- **SafeIO**: `fs.read + fs.write + alloc.heap`  
- **FileIO**: same scope as above, for file-only ops  
- **NetworkIO**: `net.read(inbound|outbound) + net.write + alloc.heap`  
- **WebService**: `fs.* + net.* + alloc.heap`  
- **GameTick**: `alloc.temp` and explicit forbids of heavy effects  
- **Deterministic**: forbids `time.*` and `random`

## 3. Callback effects

Propagate effects of callbacks using `effects(F)`:

```cantrip
function map<T,U,F>(xs: [T], f: F): Vec<U>
    where F: Fn(T): U
    needs effects(F), alloc.heap;
{
    var out = Vec.with_capacity(xs.length());
    for x in xs { out.push(f(x)); }
    out
}
```

## 4. Async effect masks

Awaiting a future imports the future’s effects; missing ones cause **E9201** with a fix‑it hint.

```cantrip
async function fetch(url: str): String
    needs alloc.heap; // missing net.read(outbound)
{
    let body = await http.get(url)?; // E9201 → add net.read(outbound)
    body
}
```

## 5. “Previously identified issues” — corrections applied in 0.7.1

- **Canonical arrows:** modal transitions use `>>`, patterns use `->`, pipeline uses `=>`.
- **Effects never inferred:** declarations are mandatory; named-effects expand in place.
- **Forbidden effects:** limited to wildcard restrictions and polymorphic constraints; redundant forbids are **E9010**.
- **Trait effect bounds:** implementations must not exceed bounds; **E9120** if they do.
- **Async masks:** `await` requires corresponding effects in the enclosing `needs` (**E9201**).
- **Keywords list de-duplicated:** contextual keywords `effects` and `pure` appear once in listings.
- **Version unification:** this guide targets spec **v0.7.1**.

---

### Appendix: Quick comparison

| Topic | Rust | Cantrip |
|--|--|--|
| Move | implicit | `move` keyword |
| Borrow checker | yes | none (explicit responsibility) |
| Lifetimes | annotations | regions |
| Effects | n/a | explicit `needs` |
| Modals | n/a | first-class with `@State >> ... >> @State` |

