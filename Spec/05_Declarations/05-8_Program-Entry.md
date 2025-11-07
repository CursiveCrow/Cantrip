# Cursive Language Specification
## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-8_Program-Entry.md
**Section**: §5.8 Program Entry and Execution Model
**Stable label**: [decl.entry]  
**Forward references**: §4.6 [module.initialization], Clause 9 [stmt], Clause 12 [memory], Clause 15 [concurrency], Clause 16 [interop]

---

### §5.8 Program Entry and Execution Model [decl.entry]

#### §5.8.1 Overview

[1] This subclause defines how an executable Cursive program begins execution, the required form of the entry point, and how exit status is determined.

[2] Library modules omit an entry point and instead expose public APIs.

#### §5.8.2 Entry Point Requirements

[1] An executable module shall define exactly one top-level `procedure main`. Multiple definitions or the absence of `main` produce diagnostic E05-801.

[2] The entry point must be declared as:
```
public procedure main(): i32
    {| |- true => true |}
```
or
```
public procedure main(args: [string]): i32
    {| |- true => true |}
```
Return type `i32` expresses the process exit status.

[3] `main` shall be `public`. Other visibility modifiers are rejected (E05-802).

[4] `main` must be a synchronous procedure whose contractual sequent declares an empty grant set. `comptime` entry points, asynchronous entry points, or procedures that require grants are not permitted (E05-803).

[5] Attributes such as `[[test]]`, `[[bench]]`, or other non-standard annotations shall not decorate `main` (E05-804).

#### §5.8.3 Execution Order

[1] Before `main` executes, all module-scope initializers in the root module and its dependencies run according to §4.6 and §5.7.

[2] `main` executes on the initial thread. Additional concurrency is implementation-defined and initiated explicitly by user code.

[3] If `main` returns normally, its `i32` result is mapped to the platform’s exit status conventions.

[4] If `main` panics or aborts, the program terminates with a non-zero exit status chosen by the implementation. Clause 12 describes destructor semantics in such cases.

#### §5.8.4 Argument Handling

[1] When the signature includes `args: [string]`, the implementation supplies command-line arguments normalized to UTF-8. Invalid data may be replaced or rejected; behavior is documented per implementation.

[2] The first argument (`args[0]`) may contain the executable path; subsequent positions carry user-provided parameters.

#### §5.8.5 Examples (Informative)

**Example 5.8.5.1 (Basic entry point):**
```cursive
public procedure main(): i32
    {| |- true => true |}
{
    println("Hello, Cursive!")
    result 0
}
```

**Example 5.8.5.2 (Entry with arguments):**
```cursive
public procedure main(args: [string]): i32
    {| |- true => true |}
{
    if args.len() < 2 {
        println("Usage: tool <path>")
        result 1
    } else {
        run_tool(args[1])
        result 0
    }
}
```

**Example 5.8.5.3 - invalid (Non-public main):**
```cursive
internal procedure main(): i32
    {| |- true => true |}
{  // error[E05-802]
    result 0
}
```

### §5.8.6 Conformance Requirements [decl.entry.requirements]

[1] Implementations shall require exactly one `public procedure main` whose contractual sequent is explicitly `{| |- true => true |}` per executable program, diagnose missing or duplicate definitions (E05-801), and reject non-public entry points (E05-802).

[2] Compilers shall forbid entry points that are `comptime` procedures, asynchronous procedures, or procedures with non-empty grant sets (E05-803) and reject disallowed attributes on `main` (E05-804).

[3] Before executing `main`, runtime systems shall evaluate module initialisers according to §4.6 and honour the exit-status mapping described in §5.8.3.
