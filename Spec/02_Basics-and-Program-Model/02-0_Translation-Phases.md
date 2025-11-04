# The Cursive Language Specification

**Part**: II — Basics and Program Model
**File**: 02-0_Translation-Phases.md
**Section**: 2.0 Translation Phases
**Previous**: [Foundations](../01_Foundations.md) | **Next**: [Source Text Encoding](02-1_Source-Text-Encoding.md)

---

## 2.0 Translation Phases

### 2.0.1 Overview

This section specifies the **translation phases** through which a Cursive source program is processed to produce an executable program. Unlike traditional compiled languages with preprocessing directives, Cursive employs a **five-phase compilation model** that integrates metaprogramming directly into the compilation pipeline.

**Definition 2.0.1 (Translation Unit):** A **translation unit** is a single source file together with all the files it imports, directly or indirectly, through import declarations.

**Definition 2.0.2 (Program):** A **program** consists of one or more translation units that are separately compiled and then linked together to form an executable.

**Definition 2.0.3 (Well-Formed Program):** A program is **well-formed** if and only if all its translation units successfully complete all five translation phases without diagnostic errors that prevent code generation.

#### 2.0.1.1 Phase Ordering

Translation SHALL occur in the following deterministic order:

1. **Phase 1: Parsing** — Source text is tokenized and parsed into an Abstract Syntax Tree (AST)
2. **Phase 2: Compile-Time Execution** — Comptime blocks execute in dependency order
3. **Phase 3: Code Generation** — Generated declarations are emitted to the AST
4. **Phase 4: Type Checking** — The complete AST (original and generated code) is type-checked
5. **Phase 5: Lowering** — The validated AST is lowered to intermediate representation

Each phase MUST complete successfully before the subsequent phase begins. A diagnostic error in any phase SHALL prevent subsequent phases from executing for that translation unit.

**NORMATIVE NOTE**: Cursive has **no preprocessing phase**. There are no text-based macros, no conditional compilation directives (#ifdef), and no source-level text substitution. All metaprogramming is performed during Phase 2 using explicit comptime constructs.

#### 2.0.1.2 No Text Preprocessing

Traditional C-family languages process source text through a preprocessor before compilation begins. Cursive explicitly rejects this model. All conditional compilation, code generation, and compile-time computation occurs within the language's type-safe comptime execution model.

**Rationale:**
- **Type safety**: Comptime code is type-checked like runtime code
- **Debuggability**: No hidden macro expansions or token pasting
- **Tool support**: IDEs and analyzers can reason about code without preprocessing
- **AI-friendliness**: Clear, explicit syntax without macro obscurity
- **Local reasoning**: No global macro state affecting local code

#### 2.0.1.3 Phase Independence

Each phase operates on well-defined input and produces well-defined output:

```
Source Text
    ↓ [Phase 1: Parsing]
AST (original declarations)
    ↓ [Phase 2: Comptime Execution]
Comptime State (values, constants)
    ↓ [Phase 3: Code Generation]
AST (original + generated declarations)
    ↓ [Phase 4: Type Checking]
Typed AST
    ↓ [Phase 5: Lowering]
Intermediate Representation (IR)
```

---

### 2.0.2 Phase 1: Parsing

**Definition 2.0.4 (Parsing Phase):** The **parsing phase** transforms source text into an Abstract Syntax Tree (AST) representing the syntactic structure of the program.

#### 2.0.2.1 Input and Output

- **Input**: UTF-8 encoded source text ([REF_TBD] Source Text Encoding)
- **Output**: Untyped AST containing all syntactic elements
- **Failure Mode**: Lexical or syntax errors prevent AST construction

#### 2.0.2.2 Operations Performed

The parsing phase SHALL:

1. **Tokenize** source text into a sequence of lexical tokens ([REF_TBD] Lexical Elements)
2. **Parse** token sequences according to the Cursive grammar ([REF_TBD] Grammar)
3. **Construct** an Abstract Syntax Tree representing program structure
4. **Preserve** source location information for diagnostic reporting
5. **Identify** but not execute comptime blocks
6. **Record** all declarations without resolving names

**NORMATIVE REQUIREMENT**: The parser MUST accept all syntactically valid programs regardless of semantic correctness. Type errors, undefined names, and other semantic violations SHALL NOT be diagnosed during parsing.

#### 2.0.2.3 Comptime Block Recognition

During parsing, comptime blocks are recognized syntactically but **not executed**:

```cursive
comptime {
    // This code is recognized as a comptime block
    // but is NOT executed during Phase 1
    let x = compute_value()
}
```

The parser constructs an AST node representing the comptime block, preserving its structure for execution in Phase 2.

#### 2.0.2.4 Two-Phase Declaration Collection

Parsing supports **two-phase name resolution** by collecting declarations before resolving references:

**Phase 1a — Declaration Collection:**
- Record all type, function, procedure, and constant declarations
- Build symbol tables mapping names to declaration locations
- Do not resolve type references or validate signatures

**Phase 1b — Reference Recording:**
- Record all name uses (type references, function calls, variable uses)
- Associate references with source locations
- Do not resolve references to declarations

This two-phase approach enables forward references within a scope:

```cursive
function f(): i32 {
    result g()  // Forward reference to g (allowed)
}

function g(): i32 {
    result 42
}
```

---

### 2.0.3 Phase 2: Compile-Time Execution

**Definition 2.0.5 (Compile-Time Execution Phase):** The **compile-time execution phase** executes all comptime blocks in dependency order, computing constants and preparing for code generation.

#### 2.0.3.1 Input and Output

- **Input**: Untyped AST from Phase 1
- **Output**: Evaluated comptime state (constants, computed values)
- **Failure Mode**: Comptime errors (exceeded limits, assertions, runtime effects)

#### 2.0.3.2 Comptime Execution Model

Comptime blocks execute as **pure, deterministic functions** with controlled side effects:

```cursive
comptime function f(x: i32): i32
    uses comptime.alloc
{
    result x * 2
}

comptime {
    let result = f(21)  // Executes at compile time
    // result = 42
}
```

**Permitted Operations:**
- All compile-time evaluable expressions ([REF_TBD] Constant Expressions)
- Compile-time control flow (if, loop, match)
- Compile-time data structures (arrays, tuples, records)
- Recursion within implementation-defined depth limits
- Effects: `comptime.alloc`, `comptime.config`, `comptime.diag`

**Prohibited Operations:**
- Runtime I/O effects (`fs.read`, `fs.write`, `net.send`, etc.)
- Runtime allocation effects (`alloc.heap`)
- Non-deterministic operations (random number generation)
- Unbounded loops (all loops MUST terminate within step limits)

#### 2.0.3.3 Execution Order

Comptime blocks execute in **dependency order**:

```cursive
comptime {
    let size = 1024  // Executes first
}

type Buffer = [u8; size]  // Depends on size

comptime {
    let buf_size = size_of::<Buffer>()  // Executes second (depends on Buffer)
}
```

**NORMATIVE REQUIREMENT**: An implementation MUST execute comptime blocks such that all data dependencies are satisfied before a block executes. If circular dependencies exist, the implementation SHALL issue a diagnostic error.

**Dependency Graph Construction:**

1. Parse all comptime blocks and record their source locations
2. Analyze each block to identify:
   - Constants it defines (e.g., `let x = ...`)
   - Constants it references (e.g., uses of previously defined names)
   - Type reflection queries (e.g., `size_of::<T>()`)
3. Construct a directed acyclic graph (DAG) where:
   - Nodes represent comptime blocks
   - Edges represent dependencies (B depends on A if B reads A's outputs)
4. Topologically sort the DAG to determine execution order
5. If cycles exist, report E02CT-0001: Circular comptime dependencies

#### 2.0.3.4 Resource Limits

**NORMATIVE REQUIREMENTS:**

A conforming implementation MUST enforce the following **minimum** limits:

| Resource | Minimum Limit | Diagnostic Code |
|----------|---------------|-----------------|
| Recursion depth | 256 levels | E02CT-0103 |
| Evaluation steps | 1,000,000 steps per block | E02CT-0104 |
| Memory allocation | 64 MB per compilation unit | E02CT-0105 |
| String size | 1 MB per string value | E02CT-0106 |
| Collection size | 10,000 elements | E02CT-0107 |

An implementation MAY provide higher limits but SHALL NOT provide lower limits.

**NORMATIVE RECOMMENDATION:**

Implementations SHOULD issue a warning when a single comptime block exceeds:
- 10 seconds of evaluation time
- 50% of the maximum evaluation steps
- 50% of the maximum memory allocation

**Example Diagnostic:**

```
warning[E02CT-0108]: comptime evaluation time excessive
  --> src/codegen.cur:42:1
   |
42 | comptime {
   | ^^^^^^^^^^ this comptime block has been evaluating for 12.4 seconds
   |
   = note: consider optimizing or splitting into multiple blocks
   = help: use --comptime-time-limit to adjust the warning threshold
```

#### 2.0.3.5 Comptime Effects

Comptime code operates under a restricted effect system ([REF_TBD] Effects):

```cursive
comptime function build_config(): Config
    uses comptime.config, comptime.alloc
{
    let host = cfg_value("build.host")
    result Config { host }
}
```

**Permitted Comptime Effects:**

- `comptime.alloc` — Arena-based compile-time allocation
- `comptime.config` — Configuration and environment queries
- `comptime.codegen` — Code generation operations (used in Phase 3)
- `comptime.diag` — Diagnostic emission (errors, warnings, notes)

**Prohibited Runtime Effects:**

```cursive
comptime function invalid(): string
    uses fs.read  // ERROR E02CT-0201: Runtime effect prohibited
{
    fs::read_to_string("data.txt")  // Cannot perform I/O at compile time
}
```

---

### 2.0.4 Phase 3: Code Generation

**Definition 2.0.6 (Code Generation Phase):** The **code generation phase** emits programmatically generated declarations to the AST based on comptime block directives.

#### 2.0.4.1 Input and Output

- **Input**: AST from Phase 1 + comptime state from Phase 2
- **Output**: Augmented AST containing original and generated declarations
- **Failure Mode**: Name collisions, invalid specifications

#### 2.0.4.2 Code Generation Model

Comptime blocks use explicit APIs to generate code:

```cursive
comptime {
    codegen::declare_function(FunctionSpec {
        name: "max_i32",
        visibility: Visibility::Public,
        params: [
            ParamSpec { name: "a", ty: TypeRef::Named("i32"), permission: Permission::Const },
            ParamSpec { name: "b", ty: TypeRef::Named("i32"), permission: Permission::Const },
        ],
        return_type: TypeRef::Named("i32"),
        uses_clause: EffectSet::empty(),
        must_clause: ContractClause::None,
        will_clause: ContractClause::None,
        body: quote {
            result if a > b { a } else { b }
        },
    })
}
```

This generates the declaration:

```cursive
public function max_i32(a: i32, b: i32): i32 {
    result if a > b { a } else { b }
}
```

#### 2.0.4.3 Code Generation APIs

The `codegen` module provides type-safe APIs for programmatic code generation:

```cursive
// Function generation
comptime function codegen::declare_function(spec: FunctionSpec): ()
    uses comptime.codegen

// Type generation
comptime function codegen::declare_type(spec: TypeSpec): ()
    uses comptime.codegen

// Constant generation
comptime function codegen::declare_constant(name: string, ty: TypeRef, value: QuotedExpr): ()
    uses comptime.codegen

// Statement insertion
comptime function codegen::insert_statement(stmt: QuotedBlock): ()
    uses comptime.codegen
```

**NORMATIVE REQUIREMENT**: All code generation MUST use these explicit APIs. There is no text-based macro system, no token-pasting, and no string-based code synthesis.

#### 2.0.4.4 Quote Expressions

Quote expressions capture code as data:

```cursive
comptime {
    let multiplier = 10
    let body = quote {
        result x * #(multiplier)  // Interpolates the value 10
    }
}
// Generates: result x * 10
```

**Interpolation syntax**: `#(expr)` NOT `$(expr)` (dollar is reserved for self parameters [REF_TBD] Self Parameters)

#### 2.0.4.5 Name Collision Prevention

**NORMATIVE REQUIREMENT**: If a generated declaration has the same name as an existing declaration in the same scope, the implementation SHALL issue diagnostic E02CG-0102: Name collision in generated code.

**Hygiene Support**: The `gensym` intrinsic generates unique identifiers:

```cursive
comptime {
    let unique_name = gensym("buffer")  // Returns "buffer_g12345"
    codegen::declare_constant(unique_name, TypeRef::Named("i32"), quote { 42 })
}
```

---

### 2.0.5 Phase 4: Type Checking

**Definition 2.0.7 (Type Checking Phase):** The **type checking phase** validates the semantic correctness of the complete AST, including both original and generated declarations.

#### 2.0.5.1 Input and Output

- **Input**: Augmented AST from Phase 3 (original + generated code)
- **Output**: Fully typed AST with resolved types and checked constraints
- **Failure Mode**: Type errors, undefined names, contract violations

#### 2.0.5.2 Type Checking Operations

The type checking phase SHALL:

1. **Resolve names** to declarations using scope rules ([REF_TBD] Scopes and Name Lookup)
2. **Infer types** for expressions using type inference ([REF_TBD] Type Inference)
3. **Check types** for compatibility and correctness ([REF_TBD] Type System)
4. **Validate effects** against declared effect clauses ([REF_TBD] Effect System)
5. **Check contracts** for preconditions and postconditions ([REF_TBD] Contracts)
6. **Verify permissions** for ownership and borrowing ([REF_TBD] Permissions)
7. **Validate modals** for state machine correctness ([REF_TBD] Modal Types)

#### 2.0.5.3 No Distinction Between Original and Generated Code

Type checking makes **no distinction** between hand-written and generated code. Both are subject to identical semantic rules:

```cursive
// Hand-written code
function add(a: i32, b: i32): i32 {
    result a + b
}

comptime {
    // Generated code (identical semantics)
    codegen::declare_function(FunctionSpec {
        name: "multiply",
        // ... specification
        body: quote { result a * b },
    })
}

// Both functions are type-checked identically in Phase 4
```

If generated code contains semantic errors, the implementation SHALL report diagnostics referencing the comptime block that generated the erroneous code.

#### 2.0.5.4 Forward References

Due to two-phase declaration collection in Phase 1, forward references within a scope are permitted:

```cursive
record Node {
    value: i32
    next: Ptr<Node>@Valid  // Forward reference to Node (allowed)
}
```

Mutual recursion across declarations is similarly supported:

```cursive
function even(n: u32): bool {
    result if n == 0 { true } else { odd(n - 1) }
}

function odd(n: u32): bool {
    result if n == 0 { false } else { even(n - 1) }
}
```

---

### 2.0.6 Phase 5: Lowering

**Definition 2.0.8 (Lowering Phase):** The **lowering phase** transforms the typed AST into an intermediate representation (IR) suitable for optimization and code generation.

#### 2.0.6.1 Input and Output

- **Input**: Fully typed AST from Phase 4
- **Output**: Intermediate representation (IR)
- **Failure Mode**: Internal compiler errors (not user-facing)

#### 2.0.6.2 Lowering Operations

The lowering phase SHALL:

1. **Eliminate** all comptime constructs (blocks are already executed)
2. **Desugar** high-level constructs into primitive operations
3. **Monomorphize** generic functions and types
4. **Insert** permission checks and lifetime tracking
5. **Generate** cleanup code for RAII and destructors
6. **Transform** modal state machines into state tags and dispatch logic
7. **Elaborate** effect handlers into explicit control flow

#### 2.0.6.3 Comptime Elimination

After Phase 5, no comptime constructs remain in the IR:

- Comptime blocks have already executed (Phase 2)
- Generated code is indistinguishable from original code (Phase 4)
- Comptime intrinsics (`size_of`, `type_name_of`, etc.) are replaced with constants

**Example:**

```cursive
// Before lowering (Phase 4 AST)
comptime {
    let size = size_of::<Point>()
    codegen::declare_constant("POINT_SIZE", TypeRef::Named("usize"), quote { #(size) })
}

// After lowering (Phase 5) - generated constant
let POINT_SIZE: usize = 16  // Constant value folded during lowering
```

#### 2.0.6.4 Monomorphization

Generic functions and types are **monomorphized** (specialized per concrete type):

```cursive
function max<T>(a: T, b: T): T
where
    T: Ord
{
    result if a > b { a } else { b }
}

// Usage
let x = max::<i32>(10, 20)
let y = max::<f64>(1.5, 2.5)

// After lowering: two specialized functions
// function max_i32(a: i32, b: i32): i32 { ... }
// function max_f64(a: f64, b: f64): f64 { ... }
```

**NORMATIVE NOTE**: Monomorphization is an implementation strategy. An implementation MAY use alternative approaches (e.g., runtime generics for certain types) provided observable behavior is identical.

#### 2.0.6.5 RAII and Cleanup

The lowering phase inserts deterministic cleanup code for RAII types:

```cursive
procedure process_file()
    uses fs.read, fs.close
{
    let file = File::open("data.txt")?  // Acquire resource
    let content = file::read_all()?
    process(content)
    // Lowering inserts: file::close() here (before scope exit)
}
```

Cleanup is inserted at every exit point (return, early return, panic unwinding).

---

### 2.0.7 Translation Phase Example

This canonical example demonstrates all five translation phases:

**Source Code (example.cur):**

```cursive
// Phase 1: This entire file is parsed into an AST

comptime {
    // Phase 2: This block executes during compile-time execution
    let max_size = cfg_value("buffer_size").parse::<usize>()

    // Phase 3: Code generation emits declarations
    codegen::declare_constant(
        name: "BUFFER_SIZE",
        ty: TypeRef::Named("usize"),
        value: quote { #(max_size) }
    )

    codegen::declare_type(TypeSpec {
        kind: TypeKind::Record,
        name: "Buffer",
        visibility: Visibility::Public,
        generics: [],
        fields: [
            FieldSpec {
                name: "data",
                ty: TypeRef::Array(TypeRef::Named("u8"), max_size),
                visibility: Visibility::Private
            },
        ],
        procedures: [],
        functions: [],
        variants: [],
    })
}

// Phase 4: Type checking validates this function
public function create_buffer(): Buffer {
    result Buffer {
        data: [0u8; BUFFER_SIZE]  // Uses generated constant
    }
}

// Phase 5: Lowering transforms this to IR
```

**Phase-by-Phase Transformation:**

**Phase 1 Output (AST):**
```
File {
    items: [
        ComptimeBlock {
            stmts: [...]  // Parsed but not executed
        },
        FunctionDecl {
            name: "create_buffer",
            params: [],
            return_type: TypeRef("Buffer"),
            body: BlockExpr([...])
        }
    ]
}
```

**Phase 2 Output (Comptime State):**
```
ComptimeState {
    constants: {
        "max_size": Value::Usize(4096)  // Computed from config
    }
}
```

**Phase 3 Output (Augmented AST):**
```
File {
    items: [
        ConstantDecl {
            name: "BUFFER_SIZE",
            ty: TypeRef("usize"),
            value: LiteralExpr(4096)
        },
        TypeDecl {
            name: "Buffer",
            kind: Record,
            fields: [
                Field {
                    name: "data",
                    ty: ArrayType(U8, 4096)
                }
            ]
        },
        ComptimeBlock { ... },  // Preserved for reference but not re-executed
        FunctionDecl { ... }
    ]
}
```

**Phase 4 Output (Typed AST):**
```
All declarations now have resolved types:
- BUFFER_SIZE: usize = 4096
- Buffer: Type = record { data: [u8; 4096] }
- create_buffer: () -> Buffer
```

**Phase 5 Output (IR - conceptual):**
```
define @create_buffer() -> %Buffer {
entry:
    %0 = alloca [u8 x 4096], align 1
    call @memset(%0, i8 0, usize 4096)
    %1 = struct %Buffer { ptr %0 }
    ret %Buffer %1
}
```

---

### 2.0.8 Conformance Requirements

A conforming implementation MUST:

1. Execute all five phases in the specified order
2. Enforce minimum resource limits for comptime execution (§2.0.3.4)
3. Support comptime effects: `comptime.alloc`, `comptime.config`, `comptime.codegen`, `comptime.diag`
4. Prohibit runtime effects in comptime contexts
5. Execute comptime blocks in dependency order (§2.0.3.3)
6. Type-check generated code identically to hand-written code
7. Support two-phase declaration collection for forward references
8. Eliminate all comptime constructs during lowering

A conforming implementation MAY:

1. Exceed minimum resource limits for comptime execution
2. Perform optimizations across phases (provided observable behavior is preserved)
3. Cache comptime execution results across compilation runs
4. Parallelize independent compilation units

A conforming implementation MUST NOT:

1. Execute comptime code with non-deterministic results
2. Permit text-based preprocessing or macro expansion
3. Allow runtime effects in comptime contexts
4. Generate code that violates type safety
5. Reorder phases or merge phases in ways that change program semantics
6. Execute subsequent phases after a phase produces diagnostic errors

---

### 2.0.9 Error Codes

**Comptime Execution Errors:**

- **E02CT-0001**: Circular comptime dependencies detected
- **E02CT-0103**: Comptime recursion depth exceeded (minimum: 256 levels)
- **E02CT-0104**: Comptime evaluation steps exceeded (minimum: 1,000,000)
- **E02CT-0105**: Comptime memory allocation exceeded (minimum: 64 MB)
- **E02CT-0106**: Comptime string size exceeded (minimum: 1 MB)
- **E02CT-0107**: Comptime collection size exceeded (minimum: 10,000 elements)
- **E02CT-0108**: Comptime evaluation time warning (not an error)
- **E02CT-0201**: Runtime effect prohibited in comptime context

**Code Generation Errors:**

- **E02CG-0102**: Name collision in generated code
- **E02CG-0103**: Invalid type reference in code generation specification
- **E02CG-0104**: Generated code failed type checking

---

**Previous**: [Foundations](../01_Foundations.md) | **Next**: [Source Text Encoding](02-1_Source-Text-Encoding.md)

**END OF SECTION 2.0**
