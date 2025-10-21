# LLM Quick Reference (Informative)

**Canonical function form:**
```cantrip
function name(param: Type): ReturnType
    needs effects;        // omit if pure
    requires conditions;  // optional
    ensures conditions;   // optional
{ body }
```

**Ownership transfer requires `move`:**
```cantrip
function consume(own data: Data) { }
function example() needs alloc.heap {
    let data = Data.new();
    consume(move data);
}
```

**Effects are explicit; use named sets:**
```cantrip
import std.effects.{SafeIO};
function process(path: String): Result<String, Error>
    needs SafeIO;
{ std.fs.read_to_string(path) }
```
