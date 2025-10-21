# 300 — Modals: State Machines as Types (Normative)

A *modal* declares named states with distinct fields and verified transitions.

### Syntax
```cantrip
modal File {
    state Closed { path: String; }
    state Open   { path: String; handle: FileHandle; pos: usize; }

    @Closed >> open() >> @Open needs fs.read { ... }
    @Open   >> read(n: usize) >> @Open needs fs.read { ... }
    @Open   >> close() >> @Closed { ... }
}
```

- `@State` markers annotate pre/post states.
- Type flows are linear across transitions; the pre‑state is consumed.
- State unions are permitted in pre/post positions: `(@A | @B) >> m >> (@C | @D)`.

### Invariants
Each state may specify invariants; transitions must establish post‑state invariants.

### Errors
- `E3003` — procedure not available in current state
- `E3004` — use of moved value (ownership transfer without `move`)
