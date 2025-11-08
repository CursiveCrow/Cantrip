# Cursive Language Specification

## Clause 16 — Compile-Time Evaluation and Reflection

**Part**: XVI — Compile-Time Evaluation and Reflection  
**File**: 16-1_Overview.md  
**Section**: §16.1 Overview and Integration  
**Stable label**: [comptime.overview]  
**Forward references**: §16.2 [comptime.procedures], §16.3 [comptime.blocks], §16.4 [comptime.intrinsics], §16.5 [comptime.reflect.optin], §16.6 [comptime.reflect.queries], §16.7 [comptime.quote], §16.8 [comptime.codegen.api], §16.9 [comptime.conformance], Clause 2 §2.2 [lex.phases], Clause 5 §5.4 [decl.function], Clause 7 §7.8 [type.introspection], Clause 8 §8.7 [expr.constant], Clause 10 [generic], Clause 12 [contract]

---

### §16.1 Overview and Integration [comptime.overview]

#### §16.1.1 Scope [comptime.overview.scope]

[1] This clause specifies Cursive's compile-time evaluation and reflection system: mechanisms for executing code during compilation, introspecting type structure, and programmatically generating declarations. The system provides:

- **Compile-time execution**: Pure computation during compilation through comptime procedures and blocks
- **Type reflection**: Opt-in introspection of type structure, layout, and metadata
- **Code generation**: Programmatic declaration creation through explicit APIs
- **Zero-cost abstraction**: Compile-time features incur no runtime overhead when unused

[2] Clause 16 governs every facility required to perform metaprogramming, reflection, and compile-time computation in portable Cursive code. The governed areas are:

- Comptime procedures and blocks (§§16.2–16.3): syntax, execution model, grant restrictions
- Comptime intrinsics and configuration queries (§16.4): assertions, platform detection, diagnostics
- Reflection opt-in and attributes (§16.5): `[[reflect]]` attribute, zero-cost guarantee
- Type metadata queries (§16.6): structure introspection, field/procedure/variant/modal reflection
- Quote expressions and interpolation (§16.7): code capture with `$(...)` interpolation
- Code generation APIs and specifications (§16.8): TypeRef, ProcedureSpec, codegen procedures
- Hygiene, validation, and conformance (§16.9): gensym, type checking, requirements

#### §16.1.2 Design Principles [comptime.overview.principles]

[3] Cursive's compile-time system adheres to core language principles:

**No macros:**
[4] Cursive provides no textual macro system. All metaprogramming uses ordinary procedures executed at compile time. This makes metaprogramming code as readable and debuggable as regular code, with full access to type checking, name resolution, and error diagnostics.

**Explicit over implicit:**
[5] All compile-time execution is marked with the `comptime` keyword. Reflection requires explicit `[[reflect]]` attribute. Code generation uses explicit API calls. There are no implicit compile-time evaluations or hidden metaprogramming.

**Deterministic:**
[6] Compilation order and comptime evaluation are deterministic. Given identical source code and compilation environment, comptime evaluation produces identical results. No unspecified evaluation order or non-deterministic operations.

**Pure with controlled effects:**
[7] Comptime code uses an explicit grant system (comptime::alloc, comptime::codegen, comptime::config, comptime::diag) that permits only compile-time-safe operations. Runtime I/O, heap allocation, and FFI are prohibited.

**Terminating:**
[8] All comptime computation must complete within implementation-defined resource limits (recursion depth, evaluation steps, memory allocation). Non-terminating comptime code triggers compilation errors.

**Zero-cost when unused:**
[9] Types without `[[reflect]]` have no reflection metadata overhead. Unused comptime features contribute zero bytes to compiled binaries. The specification guarantees that opting out of compile-time features incurs no penalty.

**Local reasoning:**
[10] Metaprogramming code is readable without global context. Quote expressions clearly show what code will be generated. Code generation APIs use type-safe specifications rather than string templates.

#### §16.1.3 Integration with Language Features [comptime.overview.integration]

[11] The compile-time system integrates with other Cursive subsystems:

**Translation phases (Clause 2 §2.2):**

[12] Comptime execution occurs during the compile-time execution phase (§2.2.4.2), after parsing and before type checking. Code generation emits declarations during the code generation phase (§2.2.4.4). This ordering ensures generated code is available for type checking.

**Declarations (Clause 5):**

[13] Comptime procedures use standard procedure declaration syntax (§5.4) with the `comptime` modifier. Generated procedures and types follow the same declaration rules as hand-written code.

**Type system (Clause 7):**

[14] Reflection builds upon basic type introspection (§7.8). TypeRef in code generation references all Cursive type constructors (primitives, composites, pointers, modals). Generated code is fully type-checked.

**Expressions (Clause 8):**

[15] Comptime blocks are expressions (§8.7) that execute at compile time. Quote expressions capture code for generation. Interpolation splices compile-time values into quoted code.

**Generics (Clause 10):**

[16] Generic parameters (type, const, grant) appear in comptime contexts. Reflection APIs are generic over types. Generated code may be generic and participates in monomorphization.

**Memory model (Clause 11):**

[17] Comptime allocation uses the `comptime::alloc` grant with arena-based memory management. Region semantics do not apply to compile-time allocations; all comptime memory is automatically freed after compilation.

**Contracts (Clause 12):**

[18] Comptime procedures declare contractual sequents using the same syntax as runtime procedures. The comptime grant system is an extension of the general grant system with compile-time-specific capabilities.

#### §16.1.4 Capabilities and Use Cases [comptime.overview.capabilities]

[19] The compile-time system enables:

**Configuration-driven compilation:**

[20] Query platform (target OS, architecture, endianness) and build configuration to generate platform-specific code. Eliminate runtime checks for compile-time-known constants.

**Type introspection:**

[21] Inspect type structure (fields, procedures, variants) to generate derived implementations (serialization, debug formatting, equality, cloning).

**Code generation:**

[22] Programmatically create procedures, types, and constants. Generate boilerplate, implement design patterns automatically, create domain-specific languages.

**Compile-time validation:**

[23] Assert invariants at compile time (layout constraints, configuration validity, type property requirements). Fail compilation early for violations.

**Metaprogramming patterns:**

[24] Implement derive-like functionality, builder pattern generation, struct-of-arrays transformations, ECS component registration, and other advanced metaprogramming patterns.

#### §16.1.5 Cross-Clause Dependencies [comptime.overview.dependencies]

[25] This clause integrates with other parts as follows:

**Clause 2 (Lexical Structure and Translation):**

- §2.2.4.2: Compile-time execution phase timing and ordering
- §2.2.3: Resource limits for comptime evaluation (recursion, steps, memory)
- §2.2.4.4: Code generation phase where declarations are emitted

**Clause 5 (Declarations):**

- §5.4: Procedure declaration syntax for comptime procedures
- §5.5: Type declarations that may carry `[[reflect]]` attribute

**Clause 7 (Types):**

- §7.8: Basic type introspection (typeof, type_name, type_id)
- All type subclauses: TypeRef references all Cursive type constructors

**Clause 8 (Expressions):**

- §8.7: Constant expressions and comptime blocks
- Quote expressions extend expression grammar

**Clause 10 (Generics):**

- Generic parameters in reflection queries
- Generic parameter specifications in generated code

**Clause 11 (Memory Model):**

- Comptime::alloc grant provides compile-time arena allocation
- No region escape analysis for comptime allocations

**Clause 12 (Contracts):**

- Comptime grant system extends general grant system
- Generated procedures include full contractual sequents

**Annex A (Grammar):**

- Quote expression grammar
- Comptime block grammar
- [[reflect]] attribute grammar

**Annex E (Implementation Guidance):**

- Diagnostic code registry (E16-001 through E16-299)
- Comptime evaluation algorithms

#### §16.1.6 Chapter Organization [comptime.overview.organization]

[26] This clause is organized into the following subclauses:

- **§16.2 Comptime Procedures**: Syntax, constraints, grant restrictions, execution semantics
- **§16.3 Comptime Blocks and Contexts**: Block expressions, scope rules, dependency ordering
- **§16.4 Comptime Intrinsics and Configuration**: Assertions, diagnostics, platform queries
- **§16.5 Reflection Opt-In and Attributes**: `[[reflect]]` attribute, zero-cost guarantee
- **§16.6 Type Reflection and Metadata Queries**: Comprehensive reflection APIs for types, fields, procedures, variants, modal states
- **§16.7 Quote Expressions and Interpolation**: Code capture, `$(...)` interpolation syntax
- **§16.8 Code Generation API and Specifications**: TypeRef, ProcedureSpec, TypeSpec, codegen namespace
- **§16.9 Hygiene, Validation, and Conformance**: Name collision prevention, type checking, conformance requirements

[27] Each subclause follows the standard template: overview, syntax, constraints, semantics, examples, diagnostics, conformance requirements.

#### §16.1.7 Notation and Conventions [comptime.overview.notation]

[28] This clause uses the following notational conventions in addition to those established in §1.4:

**Metavariables:**

- $\Gamma_{\text{ct}}$ — Compile-time environment
- $\sigma_{\text{ct}}$ — Compile-time store
- $v$ — Compile-time value
- $q$ — Quoted code block

**Judgment forms:**

- $\Gamma_{\text{ct}} \vdash e : \tau$ — Comptime type checking
- $\langle e, \sigma_{\text{ct}} \rangle \Downarrow_{\text{comptime}} \langle v, \sigma'_{\text{ct}} \rangle$ — Comptime evaluation
- $\vdash \texttt{reflect}(T) : \text{permitted}$ — Reflection permission
- $\vdash \texttt{codegen}(\text{spec}) : d$ — Code generation

**Special operators:**

- `comptime { ... }` — Comptime block expression
- `quote { ... }` — Quote expression
- `$(expr)` — Quote interpolation
- `[[reflect]]` — Reflection opt-in attribute

#### §16.1.8 Informative Summary [comptime.overview.summary]

[29] Table 16.1 summarizes compile-time evaluation capabilities:

**Table 16.1 — Compile-time system capabilities**

| Feature             | Syntax                           | Purpose                        | Specified in |
| ------------------- | -------------------------------- | ------------------------------ | ------------ |
| Comptime procedures | `comptime procedure f() { ... }` | Pure compile-time computation  | §16.2        |
| Comptime blocks     | `comptime { expr }`              | Inline compile-time evaluation | §16.3        |
| Assertions          | `comptime_assert(cond, msg)`     | Compile-time validation        | §16.4        |
| Configuration       | `cfg("key")`, `target_os()`      | Platform/build queries         | §16.4        |
| Reflection          | `[[reflect]]`, `fields_of<T>()`  | Type introspection             | §§16.5–16.6  |
| Quote               | `quote { code }`                 | Code capture                   | §16.7        |
| Interpolation       | `$(value)`, `$(ident)`           | Splice values into quotes      | §16.7        |
| Code generation     | `codegen::declare_type(spec)`    | Programmatic declarations      | §16.8        |
| Hygiene             | `gensym("prefix")`               | Unique identifier generation   | §16.9        |

[30] All features execute at compile time with zero runtime cost.

---

**Previous**: Clause 15 — Interoperability and ABI (§15.7 [interop.compatibility]) | **Next**: §16.2 Comptime Procedures (§16.2 [comptime.procedures])
