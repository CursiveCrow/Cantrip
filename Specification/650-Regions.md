# 650 — Regions (Normative)

A **region** is a lexically scoped arena with O(1) bulk deallocation.

```cantrip
function parse(input: String): Result<Data, Error>
    needs alloc.region, alloc.heap;
{
    region temp {
        let toks = lex_in<temp>(input);
        let ast  = parse_in<temp>(toks);
        Ok(ast.to_heap())
    }
}
```

Rules:
- Inner regions free before outer (LIFO).
- Values allocated in a region MUST NOT escape it (`E5001`).
- Libraries SHOULD provide `new_in<'r>` and `to_heap()` for region‑aware types.
