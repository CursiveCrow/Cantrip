# 500 — Effects and Effect Language (Normative)

Effects describe observable behavior. **Effects are never inferred**. Absence of
a `needs` clause implies purity.

### 500.1 Primitive Effects (Taxonomy)
Memory (`alloc.heap|stack|region|temp`), File system (`fs.read|write|delete|metadata`),
Network (`net.read(inbound|outbound)|net.write`), Console `io.write`, Time
(`time.read|time.sleep`), Threads (`thread.spawn|join|atomic`), Rendering
(`render.draw|compute`), Audio (`audio.play`), Input (`input.read`), Process
(`process.spawn`), FFI (`ffi.call`), Unsafe (`unsafe.ptr`), and `panic`, `async`, `random`.

### 500.2 Effect Expressions
Named effects and set algebra:
```cantrip
effect WebService = fs.read + fs.write + net.read(outbound) + net.write + alloc.heap;
function handle(req: Request): Response needs WebService { ... }
```

`+` union, `-` difference, `!` negation over the universe of effects.

### 500.3 Forbidden Effects
Forbidden effects are valid only to restrict a wildcard or a polymorphic
`effects(F)` bound. Otherwise `E9010` (redundant forbidden effect).

### 500.4 Effect Errors
- `E9001` — missing effect declaration
- `E9002` — forbidden effect used
- `E9010` — redundant forbidden effect
- `E9201` — missing effects imported via `await` (see §520)
- `E9120` — trait impl exceeds declared trait effect bound
