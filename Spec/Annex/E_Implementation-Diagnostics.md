# Cursive Language Specification

## Annex E — Implementation Guidance (Informative)

**Part**: Annex
**Section**: Annex E - Implementation Guidance
**Stable label**: [implementation]
**Forward references**: None

---

### §E.2 Algorithm Specifications [implementation.algorithms]

[1] This section provides detailed algorithms for key compiler phases. These algorithms are informative: conforming implementations may use different approaches provided they satisfy the normative requirements in the main specification clauses.

#### §E.2.6 Region Escape Analysis Algorithm [implementation.algorithms.region]

##### §E.2.6.1 Overview

[2] This subsection specifies the complete escape analysis algorithm referenced by §11.3.3 [memory.region.escape]. The algorithm determines whether region-allocated values escape their regions through provenance tracking and escape checking.

##### §E.2.6.2 Provenance Tracking Algorithm

[3] **Input**: Expression $e$ in environment $\Gamma$  
**Output**: Provenance metadata $\pi \in \{\text{Stack}, \text{Region}(r), \text{Heap}\}$

**Algorithm:**

```
function compute_provenance(e, Γ):
    match e:
        // Region allocation
        case ^expr in region r:
            return Region(r)

        // Stack-allocated local variable
        case identifier x where x is local binding without ^:
            return Stack

        // Module-scope or static
        case identifier x where x is module-scope:
            return Heap  // Static treated as Heap

        // Heap conversion
        case expr.to_heap():
            return Heap

        // Address-of: inherits from target
        case &value:
            π_value = compute_provenance(value, Γ)
            return π_value

        // Field access: inherits from structure
        case struct.field:
            π_struct = compute_provenance(struct, Γ)
            return π_struct

        // Procedure call: conservative (Heap)
        case f(args):
            return Heap  // Conservative assumption

        // Literals: Stack
        case literal:
            return Stack

        // Default: Stack (conservative)
        default:
            return Stack
```

[4] **Provenance Propagation Table**: Complete propagation rules for all expression forms:

| Expression Form         | Provenance Rule                  |
| ----------------------- | -------------------------------- |
| `^expr` in region r     | Region(r)                        |
| Local variable (no `^`) | Stack                            |
| Module-scope variable   | Heap                             |
| `&value`                | Same as value                    |
| `struct.field`          | Same as struct                   |
| `array[index]`          | Same as array                    |
| `tuple.N`               | Same as tuple                    |
| `*pointer`              | Heap (conservative)              |
| `expr.to_heap()`        | Heap                             |
| `f(args)`               | Heap (conservative)              |
| Literals                | Stack                            |
| Binary operations       | Stack (if both Stack), else Heap |
| Conditional `if`        | Most restrictive of branches     |

##### §E.2.6.3 Escape Checking Algorithm

[5] **Input**: Expression $e$, target scope $s$, region environment $R$  
**Output**: Valid or Error(E11-101)

**Algorithm:**

```
function check_escape(e, target_scope s, region_env R):
    π = compute_provenance(e, Γ)

    match π:
        case Stack:
            return Valid  // Stack values can escape

        case Heap:
            return Valid  // Heap values can escape

        case Region(r):
            if r ∈ lexical_ancestors(s):
                // Region r is local to scope s
                return Error(E11-101, {
                    region: r,
                    allocation_site: find_allocation_site(e, r),
                    escape_site: current_location,
                    provenance_path: trace_provenance(e)
                })
            else:
                // Region r is not local (parent region allocation via ^^)
                return Valid
```

[6] **Escape Check Points**: The algorithm is invoked at:

1. **Return statements**: `check_escape(return_value, containing_procedure, R)`
2. **Region block boundaries**: Before exiting region, check all values that might escape
3. **Closure captures**: For each captured value, check against closure's lifetime
4. **Field assignments**: When assigning to field of longer-lived structure

##### §E.2.6.4 Interprocedural Analysis (Optional Refinement)

[7] Implementations may perform interprocedural analysis to refine provenance for procedure calls:

**Conservative approach** (required minimum):

- All procedure calls return Heap provenance

**Refined approach** (optional):

- Track provenance through procedure signatures (future extension)
- Analyze procedure bodies to determine return provenance
- Propagate provenance through call chain

[8] Refined analysis may accept more programs but shall never accept programs that would be rejected by conservative analysis (soundness preservation).

##### §E.2.6.5 Diagnostic Construction

[9] When escape violation is detected (E11-101), construct diagnostic with:

**Required fields:**

- Region identifier where value was allocated
- Allocation site (source location of `^` operator)
- Escape site (source location of escape attempt)

**Optional fields (recommended):**

- Provenance path showing how provenance propagated
- Suggested fix (use `.to_heap()` if applicable)
- Related notes (region definition location)

**Example diagnostic output:**

```
error[E11-101]: cannot escape region-allocated value
  --> src/server.cursive:125:12
    |
125 |         result data
    |                ^^^^ region-allocated value escapes here
    |
note: value allocated in region 'temp' at line 122
  --> src/server.cursive:122:21
    |
122 |         let data = ^Buffer::new()
    |                    ^ allocated in region 'temp'
    |
note: region 'temp' defined here
  --> src/server.cursive:121:5
    |
121 |     region temp {
    |     ^^^^^^^^^^^ region exits at line 126
    |
help: convert to heap before returning
    |
125 |         result data.to_heap()
    |                    ++++++++++
```

##### §E.2.6.6 Worked Examples

**Example E.2.6.1**: Direct region escape (invalid)

```cursive
procedure build(): Buffer [[alloc::region]] {
    region r {
        let data = ^Buffer::new()  // prov(data) = Region(r)
        result data                // check_escape(data, build, R)
                                   // → Region(r) ∈ lexical_ancestors(build)
                                   // → ERROR E11-101
    }
}
```

**Example E.2.6.2**: Pointer escape via address-of (invalid)

```cursive
procedure get_ptr(): Ptr<i32>@Valid [[alloc::region]] {
    region r {
        let value = ^42            // prov(value) = Region(r)
        let ptr = &value           // prov(ptr) = prov(value) = Region(r)
        result ptr                 // check_escape(ptr, get_ptr, R)
                                   // → ERROR E11-101
    }
}
```

**Example E.2.6.3**: Heap conversion (valid)

```cursive
procedure build(): Buffer [[alloc::region, alloc::heap]] {
    region r {
        let data = ^Buffer::new()      // prov(data) = Region(r)
        let heap_data = data.to_heap() // prov(heap_data) = Heap
        result heap_data               // check_escape(heap_data, build, R)
                                       // → Heap provenance
                                       // → Valid
    }
}
```

**Example E.2.6.4**: Parent region allocation (valid)

```cursive
procedure build(): Buffer [[alloc::region]] {
    region outer {
        region inner {
            let data = ^^Buffer::new()  // prov(data) = Region(outer)
            result data                 // check_escape(data, inner, R)
                                        // → Region(outer) ∉ lexical_ancestors(inner)
                                        // → Valid (escapes inner, stays in outer)
        }
        // data still valid in outer
        result data
    }
}
```

##### §E.2.6.7 Implementation Considerations

[10] **Performance**: Provenance tracking is compile-time only with zero runtime overhead. Implementations should:

- Track provenance as metadata on AST nodes
- Propagate during type checking phase
- Perform escape checking during region boundary analysis
- Cache provenance to avoid recomputation

[11] **Precision**: The conservative analysis (procedure calls return Heap) may reject safe programs. Implementations may refine with interprocedural analysis but shall maintain soundness.

[12] **Integration**: Escape analysis integrates with:

- Type checking (§2.2.4.3)
- Move checking (Clause 11)
- Definite assignment (§5.7)
- Control-flow analysis (Clause 9)

---

### §E.5 Implementation Diagnostics [implementation.diagnostics]

[1] This annex provides comprehensive guidance for implementing conforming diagnostic systems in Cursive compilers and toolchains. It includes the authoritative diagnostic code registry, payload schemas, quality guidelines, and machine-readable output specifications.

[2] All diagnostic codes follow the canonical format `E[CC]-[NNN]` where:

- `E` denotes an error diagnostic
- `[CC]` is a two-digit clause number with leading zero (02, 04, 05, 06, 07, etc.)
- `-` is a hyphen separator for visual clarity
- `[NNN]` is a three-digit sequential number with leading zeros

[3] The format specification is normatively defined in §1.6.6 [intro.document.diagnostics].

#### §E.5.1 Diagnostic Code Registry [implementation.diagnostics.registry]

[4] The following tables enumerate all diagnostic codes defined in the Cursive language specification. Each entry includes:

- **Code**: The canonical diagnostic identifier in format `E[CC]-[NNN]`;
- **Section**: The specific section reference (e.g., §2.1.3[1]);
- **Description**: A brief summary of the error condition;
- **Severity**: Error (E), Warning (W), or Note (N).

[5] All codes use severity "E" (Error) in the current version. Future versions may introduce warning and note diagnostics.

##### §E.5.1.1 Clause 02: Lexical Structure and Translation

**Subsection §2.1: Source Text Encoding**

| Code    | Section   | Description                                   | Severity |
| ------- | --------- | --------------------------------------------- | -------- |
| E02-001 | §2.1.3[1] | Invalid UTF-8 sequence in source file         | E        |
| E02-002 | §2.1.3[5] | File size exceeds implementation limit        | E        |
| E02-003 | §2.1.3[2] | Misplaced byte-order mark (BOM)               | E        |
| E02-004 | §2.1.3[4] | Prohibited code point (U+0000 null character) | E        |

**Subsection §2.2: Translation Phases**

| Code    | Section   | Description                                       | Severity |
| ------- | --------- | ------------------------------------------------- | -------- |
| E02-101 | §2.2.3[4] | Comptime evaluation exceeded maximum iterations   | E        |
| E02-102 | §2.2.3[4] | Comptime evaluation exceeded maximum memory       | E        |
| E02-103 | §2.2.3[4] | Comptime evaluation exceeded maximum stack depth  | E        |
| E02-104 | §2.2.3[4] | Comptime evaluation exceeded time limit           | E        |
| E02-105 | §2.2.4[2] | Semantic analysis encountered circular dependency | E        |
| E02-106 | §2.2.4[3] | Type checking failed due to unresolved dependency | E        |
| E02-107 | §2.2.5[1] | Code generation encountered unsupported construct | E        |

**Subsection §2.3: Lexical Elements**

| Code    | Section   | Description                                        | Severity |
| ------- | --------- | -------------------------------------------------- | -------- |
| E02-200 | §2.3.2[3] | Reserved keyword used as identifier                | E        |
| E02-201 | §2.3.3[5] | Invalid character in identifier                    | E        |
| E02-202 | §2.3.4[2] | Unterminated string literal                        | E        |
| E02-203 | §2.3.4[3] | Invalid escape sequence in string literal          | E        |
| E02-204 | §2.3.5[2] | Unterminated character literal                     | E        |
| E02-205 | §2.3.5[3] | Empty character literal                            | E        |
| E02-206 | §2.3.6[3] | Integer literal exceeds maximum value              | E        |
| E02-207 | §2.3.6[4] | Invalid digit in numeric literal                   | E        |
| E02-208 | §2.3.7[2] | Floating-point literal exceeds representable range | E        |

**Subsection §2.4: Tokenization and Statement Termination**

| Code    | Section   | Description                                   | Severity |
| ------- | --------- | --------------------------------------------- | -------- |
| E02-300 | §2.4.3[3] | Delimiter nesting exceeds maximum depth (256) | E        |
| E02-301 | §2.4.4[5] | Disallowed form at module scope               | E        |

**Subsection §2.5: Compilation Units and Top-Level Forms**

| Code    | Section   | Description                                        | Severity |
| ------- | --------- | -------------------------------------------------- | -------- |
| E02-400 | §2.5.3[4] | Redeclaration in same scope                        | E        |
| E02-401 | §2.5.4[6] | Cyclic module initializer dependency (eager edges) | E        |

##### §E.5.1.2 Clause 04: Modules

**Subsection §4.1: Module Overview**

| Code    | Section     | Description                                                | Severity |
| ------- | ----------- | ---------------------------------------------------------- | -------- |
| E04-001 | §4.1.3[1]   | Multiple source files merged without permitted constructs  | E        |
| E04-002 | §4.1.3[2]   | Source file located outside manifest-declared source roots | E        |
| E04-003 | §4.1.3[3]   | Module path component is not a valid identifier            | E        |
| E04-004 | §4.1.3[5]   | Case-insensitive filesystem collision between module paths | E        |
| E04-005 | §4.1.3[6]   | Module path component uses a reserved keyword              | E        |
| E04-006 | §4.1.3[2]   | Project manifest missing or missing required tables        | E        |
| E04-007 | §4.1.3[5.2] | Case-insensitive filesystem collision warning              | W        |

**Subsection §4.2: Module Syntax (Import/Use)**

| Code    | Section   | Description                                              | Severity |
| ------- | --------- | -------------------------------------------------------- | -------- |
| E04-100 | §4.2.3[4] | Duplicate entry in `use` list                            | E        |
| E04-101 | §4.2.3[5] | Wildcard `use` expansion introduces conflicting bindings | E        |
| E04-102 | §4.2.3[7] | Visibility modifier applied to an `import` declaration   | E        |

**Subsection §4.3: Module Scope (Use Bindings)**

| Code    | Section   | Description                                                        | Severity |
| ------- | --------- | ------------------------------------------------------------------ | -------- |
| E04-200 | §4.3.2[3] | `use` binding collides with existing identifier without `as` alias | E        |
| E04-201 | §4.3.2[4] | Wildcard `use` expansion conflicts with existing bindings          | E        |
| E04-202 | §4.3.2[7] | `use` references a module that has not been imported               | E        |
| E04-203 | §4.4.2[4] | Duplicate exported identifier resulting from `public use`          | E        |
| E04-204 | §4.4.2[2] | `public use` re-exports a non-public item                          | E        |
| E04-205 | §4.3.2[1] | `import` refers to module outside manifest-declared roots          | E        |

**Subsection §4.5: Qualified Name Resolution**

| Code    | Section   | Description                                                         | Severity |
| ------- | --------- | ------------------------------------------------------------------- | -------- |
| E04-400 | §4.5.3[1] | Qualified-name head is not a known module or alias                  | E        |
| E04-401 | §4.5.3[3] | Qualified-name head ambiguous across imports                        | E        |
| E04-402 | §4.5.3[4] | Partial module path does not resolve to a module                    | E        |
| E04-403 | §4.5.3[5] | Attempt to rebind an existing import alias                          | E        |
| E04-404 | §4.5.3[2] | Qualified name references an item not exported by the target module | E        |

**Subsection §4.6: Module Initialization and Cycles**

| Code    | Section     | Description                                               | Severity |
| ------- | ----------- | --------------------------------------------------------- | -------- |
| E04-500 | §4.6.2[4]   | Cycle detected in eager dependency graph                  | E        |
| E04-501 | §4.6.2[6]   | Module initialization blocked by failing predecessor      | E        |
| E04-502 | §4.6.2[8]   | Access to binding before prerequisite module initialises  | E        |
| E04-503 | §4.6.4.4[8] | Lazy dependency misclassified; eager interaction required | E        |

**Subsection §4.6.7: Type Visibility Integration**

| Code    | Section   | Description                                     | Severity |
| ------- | --------- | ----------------------------------------------- | -------- |
| E07-750 | §4.6.7[5] | Internal type referenced across module boundary | E        |

##### §E.5.1.2A Clause 03: Basic Concepts

**Subsection §3.1: Objects and Values**

| Code    | Section    | Description                                      | Severity |
| ------- | ---------- | ------------------------------------------------ | -------- |
| E03-101 | §3.1.7[33] | Inference hole could not be resolved             | E        |
| E03-102 | §3.1.7     | Type inference failed; incompatible branch types | E        |

**Subsection §3.5: Alignment and Layout**

| Code    | Section      | Description                             | Severity |
| ------- | ------------ | --------------------------------------- | -------- |
| E03-201 | §3.5.2.3[10] | Explicit alignment smaller than natural | E        |

---

##### §E.5.1.3 Clause 05: Declarations

**Subsection §5.2: Variable Bindings**

| Code    | Section   | Description                                  | Severity |
| ------- | --------- | -------------------------------------------- | -------- |
| E05-201 | §5.2.3[1] | `shadow` used without matching outer binding | E        |
| E05-202 | §5.2.3[2] | Reassignment of `let` binding                | E        |
| E05-203 | §5.2.3[3] | Duplicate identifiers in pattern binding     | E        |
| E05-204 | §5.2.3[4] | Pattern initializer arity/shape mismatch     | E        |

**Subsection §5.3: Binding Patterns**

| Code    | Section   | Description                                          | Severity |
| ------- | --------- | ---------------------------------------------------- | -------- |
| E05-301 | §5.3.3[2] | Unknown field in record pattern                      | E        |
| E05-302 | §5.3.3[3] | Tuple pattern arity mismatch                         | E        |
| E05-303 | §5.3.3[4] | Duplicate identifiers in pattern                     | E        |
| E05-304 | §5.3.3[1] | Missing type annotation for multi-identifier pattern | E        |

**Subsection §5.4: Procedures**

| Code    | Section     | Description                                                | Severity |
| ------- | ----------- | ---------------------------------------------------------- | -------- |
| E05-401 | §5.4.3[2]   | Procedure receiver missing or malformed                    | E        |
| E05-402 | §5.4.3[6]   | Comptime callable body not compile-time evaluable          | E        |
| E05-403 | §5.4.3[6]   | Missing return type annotation where required              | E        |
| E05-406 | §5.4.3[5]   | Procedure with empty grant set attempts grant-requiring op | E        |
| E05-408 | §5.4.3[4]   | Expression-bodied procedure includes explicit sequent      | E        |
| E05-409 | §5.4.3[2.3] | Call site missing `move` for responsible parameter         | E        |
| E05-410 | §5.4.3[2.3] | Call site uses `move` for non-responsible parameter        | E        |

**Subsection §5.5: Type Declarations**

| Code    | Section    | Description                                       | Severity |
| ------- | ---------- | ------------------------------------------------- | -------- |
| E05-501 | §5.5.3[11] | Cyclic type alias                                 | E        |
| E05-502 | §5.5.3[10] | Unsupported representation attribute              | E        |
| E05-503 | §5.5.3[6]  | Duplicate contract reference on type declaration  | E        |
| E05-504 | §5.5.3[7]  | Duplicate predicate reference on type declaration | E        |
| E05-505 | §5.5.3[13] | Empty modal type declaration                      | E        |
| E05-506 | §5.5.3[14] | Multiple contracts with incompatible signatures   | E        |
| E05-507 | §5.5.3[15] | Infinite-size recursive type                      | E        |

**Subsection §5.6: Visibility Rules**

| Code    | Section   | Description                                  | Severity |
| ------- | --------- | -------------------------------------------- | -------- |
| E05-601 | §5.6.3[1] | `private`/`protected` used at module scope   | E        |
| E05-602 | §5.6.3[4] | Visibility modifier applied to local binding | E        |
| E05-603 | §5.6.5[1] | Attempt to widen visibility during re-export | E        |
| E05-604 | §5.6.5[2] | Non-`public` modifier used with `use`        | E        |
| E05-605 | §5.6.4[3] | Protected member access violation            | E        |

**Subsection §5.7: Initialization and Definite Assignment**

| Code    | Section   | Description                                      | Severity |
| ------- | --------- | ------------------------------------------------ | -------- |
| E05-701 | §5.7.2[2] | Module initializer cycle                         | E        |
| E05-702 | §5.7.2[3] | Module initializer blocked by failing dependency | E        |
| E05-703 | §5.7.3[2] | Use of potentially unassigned binding            | E        |
| E05-704 | §5.7.4[4] | Pattern omits required binding                   | E        |
| E05-705 | §5.7.5[1] | Reassignment of `let` binding                    | E        |

**Subsection §5.8: Program Entry Points**

| Code    | Section   | Description                                      | Severity |
| ------- | --------- | ------------------------------------------------ | -------- |
| E05-801 | §5.8.2[1] | Missing or multiple entry points                 | E        |
| E05-802 | §5.8.2[3] | Entry point not `public`                         | E        |
| E05-803 | §5.8.2[4] | Entry point declared as procedure/comptime/async | E        |
| E05-804 | §5.8.2[5] | Disallowed attribute on entry point              | E        |

**Subsection §5.9: Grant Declarations**

| Code    | Section   | Description                                  | Severity |
| ------- | --------- | -------------------------------------------- | -------- |
| E05-901 | §5.9.3[3] | Grant name conflicts with reserved namespace | E        |
| E05-902 | §5.9.3[4] | Grant declaration not at module scope        | E        |
| E05-903 | §5.9.3[2] | Duplicate grant name in same module          | E        |

##### §E.5.1.4 Clause 06: Names, Scopes, and Resolution

**Subsection §6.2: Scope Formation**

| Code    | Section    | Description                          | Severity |
| ------- | ---------- | ------------------------------------ | -------- |
| E06-201 | §6.2.3[5]  | Scope tree cycle or multiple parents | E        |
| E06-202 | §6.2.3[10] | Treating `use` as lexical scope      | E        |

**Subsection §6.3: Name Introduction and Shadowing**

| Code    | Section    | Description                                           | Severity |
| ------- | ---------- | ----------------------------------------------------- | -------- |
| E06-300 | §6.3.3[6]  | Redeclaration of identifier in same scope             | E        |
| E06-301 | §6.3.3[7]  | `shadow` used outside nested scope (including module) | E        |
| E06-302 | §6.3.3[8]  | Redeclaration/shadow/alias of predeclared identifier  | E        |
| E06-303 | §6.3.3[10] | Pattern shadow refers to non-existent outer binding   | E        |

**Subsection §6.4: Name Lookup**

| Code    | Section      | Description                                       | Severity |
| ------- | ------------ | ------------------------------------------------- | -------- |
| E06-400 | §6.4.4       | Ambiguous identifier (multiple imported bindings) | E        |
| E06-401 | §6.4.4       | Undefined identifier (not found in any scope)     | E        |
| E06-402 | §6.4.4[13]   | Qualified-name prefix is not module or type       | E        |
| E06-403 | §6.4.4[13]   | Qualified-name suffix is not exported             | E        |
| E06-404 | §6.4.4[13]   | Module-qualified name missing target item         | E        |
| E06-405 | §6.4.4[13]   | Type-qualified name missing associated item       | E        |
| E06-406 | §6.4.4[12.2] | Qualified name chain too long                     | E        |
| E06-407 | §6.4.4[13.1] | Empty qualified name component                    | E        |

##### §E.5.1.5 Clause 07: Type System

**Subsection §7.2: Primitive Types**

| Code    | Section   | Description                                                    | Severity |
| ------- | --------- | -------------------------------------------------------------- | -------- |
| E07-001 | §7.2.1[5] | Attempt to redeclare or shadow a reserved primitive identifier | E        |
| E07-100 | §7.2.2.6  | Integer constant overflow during folding                       | E        |
| E07-101 | §7.2.2.6  | Compile-time division or remainder by zero                     | E        |
| E07-102 | §7.2.3.6  | Invalid floating literal suffix                                | E        |
| E07-103 | §7.2.3.6  | Floating constant overflow or division by zero                 | E        |
| E07-104 | §7.2.4.4  | Non-boolean operand supplied to logical operator               | E        |
| E07-105 | §7.2.4.4  | Boolean used where numeric type required                       | E        |
| E07-106 | §7.2.5.2  | Character literal uses surrogate code point                    | E        |

**Subsection §7.4: Function Types**

| Code    | Section    | Description                                        | Severity |
| ------- | ---------- | -------------------------------------------------- | -------- |
| E07-200 | §7.4.4[15] | Grant set references undeclared grant              | E        |
| E07-201 | §7.4.4[15] | Contractual sequent references undefined predicate | E        |
| E07-202 | §7.4.4[15] | Call site lacks required grants                    | E        |
| E07-203 | §7.4.4[15] | Call site cannot prove `must` clause               | E        |
| E07-204 | §7.4.4[15] | Callable loses grant/contract info when exported   | E        |
| E07-205 | §7.4.4[15] | Function subtyping violates variance rules         | E        |
| E07-206 | §7.4.4[15] | Conversion of capturing closure to raw pointer     | E        |

**Subsection §7.5: Pointer Types**

| Code    | Section  | Description                                      | Severity |
| ------- | -------- | ------------------------------------------------ | -------- |
| E07-300 | §7.5.3.2 | Pointer escapes stack/region provenance          | E        |
| E07-301 | §7.5.2.4 | Dereference of `Ptr<T>@Null`                     | E        |
| E07-302 | §7.5.2.3 | Address-of applied to non-storage expression     | E        |
| E07-303 | §7.5.2.3 | Missing explicit pointer type annotation         | E        |
| E07-304 | §7.5.2.4 | Dereference of `Ptr<T>@Weak` without upgrade     | E        |
| E07-305 | §7.5.2.4 | Dereference of `Ptr<T>@Expired`                  | E        |
| E07-400 | §7.5.5.2 | Raw pointer dereference outside `unsafe` context | E        |
| E07-401 | §7.5.5.2 | Raw pointer assignment outside `unsafe` context  | E        |
| E07-402 | §7.5.5.3 | Cast between incompatible raw pointer types      | E        |

**Subsection §7.6: Modal Types**

| Code    | Section     | Description                                                    | Severity |
| ------- | ----------- | -------------------------------------------------------------- | -------- |
| E07-500 | §7.6.8[13]  | Duplicate modal state name                                     | E        |
| E07-501 | §7.6.8[13]  | Transition references undeclared state                         | E        |
| E07-502 | §7.6.8[13]  | Transition body fails to return the required target state      | E        |
| E07-503 | §7.6.8[13]  | Modal pattern match missing required state branch              | E        |
| E07-504 | §7.6.2[6.3] | State-specific field accessed outside its defining state       | E        |
| E07-505 | §7.6.2[6.1] | Transition signature lacks a matching procedure implementation | E        |

**Subsection §7.3: Composite Types (Union Types)**

| Code    | Section    | Description                                       | Severity |
| ------- | ---------- | ------------------------------------------------- | -------- |
| E07-710 | §7.3.6[60] | Union component type not well-formed              | E        |
| E07-711 | §7.3.6[60] | Type mismatch: cannot widen to union              | E        |
| E07-712 | §7.3.6[60] | Single-component union (should use type directly) | E        |

**Subsection §7.7: Type Relations**

| Code    | Section | Description                                  | Severity |
| ------- | ------- | -------------------------------------------- | -------- |
| E07-700 | §7.7.6  | Subtyping check failure / variance violation | E        |
| E07-701 | §7.7.6  | Cyclic type alias                            | E        |
| E07-702 | §7.7.6  | Type compatibility failure for operation     | E        |
| E07-703 | §7.7.6  | Modal/union pattern lacks branch             | E        |
| E07-704 | §7.7.6  | Union pattern missing component type         | E        |

**Subsection §7.8: Type Introspection**

| Code    | Section | Description                                        | Severity |
| ------- | ------- | -------------------------------------------------- | -------- |
| E07-900 | §7.8.2  | `typeof` used in value position                    | E        |
| E07-901 | §7.8.5  | `type_info` result escapes compile-time context    | E        |
| E07-902 | §7.8.8  | Introspection invoked on ill-formed type           | E        |
| E07-903 | §7.8.5  | Attempt to inspect private field via introspection | E        |

##### §E.5.1.6 Clause 08: Expressions

**Subsection §8.1: Expression Fundamentals**

| Code    | Section   | Description                                                | Severity |
| ------- | --------- | ---------------------------------------------------------- | -------- |
| E08-001 | §8.1.3[8] | Runtime-only construct used in const/comptime context      | E        |
| E08-002 | §8.1.6    | Type cannot be inferred and no contextual type provided    | E        |
| E08-004 | §8.1.5    | Missing grant(s) from grants clause in contractual sequent | E        |
| E08-020 | §8.1.3[9] | Pipeline stage omitted type annotation while changing type | E        |
| E08-021 | §8.2.11   | Pipeline stage annotation disagrees with inferred type     | E        |

**Subsection §8.2: Primary and Postfix Expressions**

| Code    | Section   | Description                                     | Severity |
| ------- | --------- | ----------------------------------------------- | -------- |
| E08-201 | §8.2.2[3] | Integer literal cannot fit target type          | E        |
| E08-202 | §8.2.2[3] | Invalid character literal                       | E        |
| E08-210 | §8.2.3[7] | Identifier used before definite assignment      | E        |
| E08-211 | §8.2.3[5] | Identifier resolves to non-value entity         | E        |
| E08-220 | §8.2.5    | Block producing non-unit value without `result` | E        |
| E08-230 | §8.2.7    | Procedure call arity mismatch                   | E        |
| E08-231 | §8.2.7    | Procedure call has extra arguments              | E        |
| E08-232 | §8.2.7    | Method receiver lacks required permission       | E        |
| E08-233 | §8.2.7    | Method receiver has incompatible type           | E        |
| E08-240 | §8.2.8    | Field access violates visibility                | E        |
| E08-241 | §8.2.8    | Tuple projection index out of range             | E        |
| E08-250 | §8.2.9    | Array/slice index out of range                  | E        |
| E08-251 | §8.2.9    | Slice bounds invalid                            | E        |
| E08-260 | §8.2.10   | Address-of applied to non-place expression      | E        |

**Subsection §8.3: Unary and Binary Operators**

| Code    | Section | Description                                            | Severity |
| ------- | ------- | ------------------------------------------------------ | -------- |
| E08-301 | §8.3.4  | Operand types mismatch for arithmetic/bitwise operator | E        |
| E08-302 | §8.3.4  | Modulo operator applied to non-integer type            | E        |
| E08-303 | §8.3.4  | Shift amount ≥ bit width                               | E        |
| E08-304 | §8.3.4  | Division or modulo by zero                             | E        |
| E08-310 | §8.3.6  | Equality invoked on type without `Eq` implementation   | E        |
| E08-320 | §8.3.7  | Logical operator operands not `bool`                   | E        |
| E08-330 | §8.3.3  | Checked-mode arithmetic negation overflow              | E        |
| E08-331 | §8.3.9  | `move` applied to value without ownership              | E        |
| E08-340 | §8.3.8  | Assignment target is not a place                       | E        |
| E08-341 | §8.3.8  | Compound assignment invalid for operand type           | E        |
| E08-342 | §8.3.8  | Chained assignment is ill-formed                       | E        |

**Subsection §8.4: Structured Expressions**

| Code    | Section | Description                                                | Severity |
| ------- | ------- | ---------------------------------------------------------- | -------- |
| E08-400 | §8.4.2  | Record literal missing required field                      | E        |
| E08-401 | §8.4.2  | Record literal duplicates field                            | E        |
| E08-402 | §8.4.2  | Record literal references field not visible in scope       | E        |
| E08-403 | §8.4.2  | Single-element tuple literal disallowed                    | E        |
| E08-404 | §8.4.2  | Enum variant payload mismatch                              | E        |
| E08-430 | §8.4.3  | Array literal elements have mismatched types               | E        |
| E08-431 | §8.4.3  | Repeat array requires `Copy` element or comptime length    | E        |
| E08-440 | §8.4.4  | `if` expression requires `else` but none provided          | E        |
| E08-450 | §8.4.5  | Match result type inference failed (use typed hole)        | E        |
| E08-451 | §8.4.5  | Match is non-exhaustive                                    | E        |
| E08-452 | §8.4.5  | Match arm unreachable                                      | E        |
| E08-460 | §8.4.6  | Loop break values disagree                                 | E        |
| E08-461 | §8.4.6  | Iterator loop missing element type annotation              | E        |
| E08-462 | §8.4.6  | Iterator does not satisfy iteration protocol               | E        |
| E08-463 | §8.4.6  | `break`/`continue` used outside loop                       | E        |
| E08-470 | §8.4.7  | Closure captures moved-from value                          | E        |
| E08-471 | §8.4.7  | Closure outlives captured reference (escape violation)     | E        |
| E08-472 | §8.4.7  | Closure captures unique binding while uniqueness is active | E        |
| E08-473 | §8.4.7  | Closure permission incompatible with captured variable     | E        |

**Subsection §8.5: Patterns and Exhaustiveness**

| Code    | Section | Description                                                  | Severity |
| ------- | ------- | ------------------------------------------------------------ | -------- |
| E08-500 | §8.5.2  | Record pattern omits required field                          | E        |
| E08-510 | §8.5.2  | Or-pattern branches bind different identifiers               | E        |
| E08-511 | §8.5.2  | Or-pattern branches bind identifiers with incompatible types | E        |
| E08-512 | §8.5.4  | Match arm is unreachable                                     | E        |
| E08-520 | §8.5.5  | Destructuring statement used in disallowed context           | E        |
| E08-704 | §8.5.2  | Union pattern match missing component type(s)                | E        |

**Subsection §8.6: Conversions and Coercions**

| Code    | Section | Description                          | Severity |
| ------- | ------- | ------------------------------------ | -------- |
| E08-600 | §8.6.2  | Constant cast loses information      | E        |
| E08-601 | §8.6.2  | Cast not covered by a legal category | E        |

**Subsection §8.7: Constant and Comptime**

| Code    | Section | Description                                           | Severity |
| ------- | ------- | ----------------------------------------------------- | -------- |
| E08-700 | §8.7.2  | Runtime-only construct used in const/comptime context | E        |
| E08-701 | §8.7.3  | Comptime block requested non-comptime grant           | E        |

**Subsection §8.8: Expression Typing**

| Code    | Section | Description                                        | Severity |
| ------- | ------- | -------------------------------------------------- | -------- |
| E08-800 | §8.8.2  | Type compatibility failure (general typing errors) | E        |
| E11-403 | §11.4   | Place used without required permission             | E        |

##### §E.5.1.7 Clause 09: Statements and Control Flow

**Subsection §9.2: Simple Statements**

| Code    | Section    | Description                                                | Severity |
| ------- | ---------- | ---------------------------------------------------------- | -------- |
| E09-101 | §9.2.3[12] | Assignment to immutable (`let`) binding                    | E        |
| E09-102 | §9.2.3[13] | Type mismatch in assignment                                | E        |
| E09-103 | §9.2.3[15] | Assignment operator mismatch (= vs <-)                     | E        |
| E09-120 | §9.2.6[30] | Defer block produces non-unit value                        | E        |
| E09-121 | §9.2.6[31] | Defer block contains control transfer escaping defer scope | E        |

**Subsection §9.3: Control Flow Statements**

| Code    | Section    | Description                                   | Severity |
| ------- | ---------- | --------------------------------------------- | -------- |
| E09-201 | §9.3.3[20] | Return statement outside procedure body       | E        |
| E09-202 | §9.3.3[21] | Return value type mismatch                    | E        |
| E09-203 | §9.3.3[22] | Missing return value on control-flow path     | E        |
| E09-211 | §9.3.4[29] | Break statement outside loop or labeled block | E        |
| E09-212 | §9.3.4[30] | Break/continue references undefined label     | E        |
| E09-213 | §9.3.4[31] | Break values have incompatible types          | E        |
| E09-221 | §9.3.5[37] | Continue statement outside loop               | E        |
| E09-222 | §9.3.5[38] | Continue targets non-loop label               | E        |
| E09-231 | §9.3.6[44] | Duplicate label in procedure                  | E        |

##### §E.5.1.8 Clause 10: Generics and Behaviors

**Subsection §10.2: Type Parameters**

| Code    | Section | Description                                                        | Severity |
| ------- | ------- | ------------------------------------------------------------------ | -------- |
| E10-101 | §10.2.3 | Type parameter cannot be inferred; explicit type argument required | E        |
| E10-102 | §10.2.4 | Const argument not compile-time evaluable                          | E        |
| E10-103 | §10.2.4 | Const parameter used in runtime-only context                       | E        |
| E10-104 | §10.2.6 | Parameter with default appears before parameter without default    | E        |
| E10-105 | §10.2.6 | Default value references later parameter                           | E        |
| E10-106 | §10.2.7 | Partial explicit arguments skip early parameters                   | E        |
| E10-107 | §10.2.8 | Duplicate parameter name in generic parameter list                 | E        |
| E10-108 | §10.2.8 | Const parameter has invalid type (not integer or bool)             | E        |
| E10-109 | §10.2.8 | Grant parameter uses `:` instead of `⊆` for bound                  | E        |
| E10-110 | §10.2.8 | Const parameter used as runtime value                              | E        |

**Subsection §10.3: Bounds and Where-Constraints**

| Code    | Section | Description                                                     | Severity |
| ------- | ------- | --------------------------------------------------------------- | -------- |
| E10-201 | §10.3.9 | Bound references undefined or non-visible behavior/contract     | E        |
| E10-202 | §10.3.9 | Type argument does not satisfy bound                            | E        |
| E10-203 | §10.3.9 | Associated type equality constraint: types do not match         | E        |
| E10-204 | §10.3.9 | Grant bound references invalid grant identifier                 | E        |
| E10-205 | §10.3.9 | Cyclic bound dependencies through associated type projections   | E        |
| E10-206 | §10.3.9 | Conflicting associated type equality constraints                | E        |
| E10-301 | §10.3.7 | Grant parameter exceeds bound (grant set not subset of allowed) | E        |

**Subsection §10.4: Behavior Declarations**

| Code    | Section | Description                                                     | Severity |
| ------- | ------- | --------------------------------------------------------------- | -------- |
| E10-401 | §10.4.3 | Behavior procedure declared without body                        | E        |
| E10-402 | §10.4.3 | Cyclic behavior extension (behavior extends itself)             | E        |
| E10-403 | §10.4.4 | Associated type does not satisfy bound                          | E        |
| E10-404 | §10.4.8 | Behavior extends non-behavior (e.g., contract)                  | E        |
| E10-405 | §10.4.8 | Behavior name collision with existing binding                   | E        |
| E10-406 | §10.4.8 | Duplicate associated type name in behavior                      | E        |
| E10-407 | §10.4.8 | Behavior procedure has invalid receiver                         | E        |
| E10-411 | §10.4.5 | Type claims Copy but does not satisfy structural requirements   | E        |
| E10-412 | §10.4.5 | Type implements both Copy and Drop (mutual exclusion violation) | E        |
| E10-413 | §10.4.3 | Conflicting procedures from extended behaviors                  | E        |

**Subsection §10.5: Behavior Implementations and Coherence**

| Code    | Section  | Description                                             | Severity |
| ------- | -------- | ------------------------------------------------------- | -------- |
| E10-501 | §10.5.4  | Incomplete implementation (missing procedures or types) | E        |
| E10-502 | §10.5.4  | Procedure signature incompatible with behavior          | E        |
| E10-503 | §10.5.4  | Associated type specification missing                   | E        |
| E10-504 | §10.5.5  | Blanket implementation lacks where clause               | E        |
| E10-505 | §10.5.6  | Orphan rule violation                                   | E        |
| E10-506 | §10.5.7  | Duplicate implementation (coherence violation)          | E        |
| E10-507 | §10.5.7  | Overlapping blanket implementations                     | E        |
| E10-508 | §10.5.11 | Implementation for non-behavior (e.g., contract)        | E        |
| E10-509 | §10.5.11 | Implementation type does not exist or is not visible    | E        |
| E10-510 | §10.5.11 | Generic implementation parameter mismatch               | E        |

**Subsection §10.6: Resolution and Monomorphization**

| Code    | Section | Description                                              | Severity |
| ------- | ------- | -------------------------------------------------------- | -------- |
| E10-601 | §10.6.2 | Cannot infer type arguments; explicit turbofish required | E        |
| E10-602 | §10.6.3 | Type argument does not satisfy behavior bound            | E        |
| E10-603 | §10.6.3 | Type argument does not satisfy contract bound            | E        |
| E10-604 | §10.6.3 | Associated type constraint not satisfied                 | E        |
| E10-605 | §10.6.3 | Grant parameter exceeds bound                            | E        |
| E10-606 | §10.6.5 | Cyclic associated type projection                        | E        |
| E10-607 | §10.6.7 | Ambiguous blanket implementation (multiple match)        | E        |
| E10-608 | §10.6.7 | Behavior not satisfied for type                          | E        |
| E10-609 | §10.6.8 | Type unification failed                                  | E        |
| E10-610 | §10.6.8 | No principal type exists (ambiguous inference)           | E        |
| E10-611 | §10.6.4 | Infinite-size recursive type                             | E        |
| E10-612 | §10.6.4 | Monomorphization limit exceeded                          | E        |

**Subsection §10.7: Variance and Inference**

| Code    | Section  | Description                                        | Severity |
| ------- | -------- | -------------------------------------------------- | -------- |
| E10-701 | §10.7.10 | Variance violation in generic type substitution    | E        |
| E10-702 | §10.7.12 | Conflicting variance requirements (internal error) | E        |

##### §E.5.1.9 Clause 11: Memory Model, Regions, and Permissions

**Subsection §11.3: Regions**

| Code    | Section     | Description                                   | Severity |
| ------- | ----------- | --------------------------------------------- | -------- |
| E11-101 | §11.3.4[18] | Region-allocated value escapes region         | E        |
| E11-102 | §11.3.4[22] | Heap allocation requires alloc::heap grant    | E        |
| E11-103 | §11.3.8     | Region allocation outside region block        | E        |
| E11-104 | §11.3.5     | Invalid caret stacking (too many ^ for depth) | E        |
| E11-110 | §11.3.7[33] | Arena binding escapes region                  | E        |

**Subsection §11.4: Permissions**

| Code    | Section | Description                                        | Severity |
| ------- | ------- | -------------------------------------------------- | -------- |
| E11-301 | §11.4.6 | Mutation through `const` permission                | E        |
| E11-302 | §11.4.6 | Multiple `unique` bindings to same object          | E        |
| E11-303 | §11.4.6 | Permission upgrade attempt (const → unique)        | E        |
| E11-310 | §11.4.6 | Partition accessed multiple times through `shared` | E        |
| E11-311 | §11.4.6 | Invalid partition directive (unknown identifier)   | E        |
| E11-403 | §11.4   | Place used without required permission             | E        |

**Subsection §11.5: Move/Copy/Clone**

| Code    | Section | Description                                                                | Severity |
| ------- | ------- | -------------------------------------------------------------------------- | -------- |
| E11-501 | §11.5.6 | Attempt to move from `var` binding                                         | E        |
| E11-502 | §11.5.6 | Attempt to move from non-responsible binding                               | E        |
| E11-503 | §11.5.6 | Use of moved value                                                         | E        |
| E11-504 | §11.5.6 | Use of non-responsible binding after source moved to responsible parameter | E        |
| E11-510 | §11.5.6 | Attempt to copy non-Copy type                                              | E        |
| E11-511 | §11.5.6 | Attempt to clone non-Clone type                                            | E        |

**Subsection §11.6: Layout and Alignment**

| Code    | Section | Description                             | Severity |
| ------- | ------- | --------------------------------------- | -------- |
| E11-601 | §11.6.5 | Explicit alignment smaller than natural | E        |
| E11-602 | §11.6.5 | Invalid repr attribute combination      | E        |
| E11-603 | §11.6.5 | Transparent applied to multi-field type | E        |

**Subsection §11.7: Aliasing**

| Code    | Section | Description                               | Severity |
| ------- | ------- | ----------------------------------------- | -------- |
| E11-701 | §11.7.6 | Multiple `unique` bindings to same object | E        |
| E11-702 | §11.7.6 | `shared` binding while `unique` active    | E        |
| E11-703 | §11.7.6 | `unique` binding while `shared` active    | E        |

**Subsection §11.8: Unsafe Blocks**

| Code    | Section | Description                            | Severity |
| ------- | ------- | -------------------------------------- | -------- |
| E11-801 | §11.8.6 | Raw pointer dereference outside unsafe | E        |
| E11-802 | §11.8.6 | Raw pointer write outside unsafe       | E        |
| E11-803 | §11.8.6 | Transmute outside unsafe               | E        |
| E11-804 | §11.8.6 | Transmute size mismatch                | E        |
| E11-805 | §11.8.6 | Inline assembly outside unsafe         | E        |

##### §E.5.1.10 Clause 12: Contracts

**Subsection §12.2: Sequent Syntax and Structure**

| Code    | Section    | Description                               | Severity |
| ------- | ---------- | ----------------------------------------- | -------- |
| E12-001 | §12.2.3[1] | Invalid sequent brackets (not [[]])       | E        |
| E12-002 | §12.2.3[2] | Missing or duplicate turnstile            | E        |
| E12-003 | §12.2.3[3] | Missing or duplicate implication operator | E        |
| E12-004 | §12.2.3[4] | Sequent components in wrong order         | E        |
| E12-005 | §12.2.3[5] | Effectful expression in must/will clause  | E        |
| E12-006 | §12.2.3[6] | Undefined grant in grant clause           | E        |
| E12-007 | §12.2.3[7] | result identifier used in must clause     | E        |
| E12-008 | §12.2.3[8] | @old operator used in must clause         | E        |
| E12-009 | §12.2.3[8] | Nested @old operators                     | E        |
| E12-010 | §12.2.3[9] | Ambiguous abbreviated sequent             | E        |

**Subsection §12.3: Grants**

| Code    | Section      | Description                                   | Severity |
| ------- | ------------ | --------------------------------------------- | -------- |
| E12-020 | §12.3.3.9[9] | Comptime grant used in runtime procedure      | E        |
| E12-030 | §12.3.8[21]  | Call site missing required grants             | E        |
| E12-031 | §12.3.9[1]   | Grant visibility violation (non-public grant) | E        |
| E12-032 | §12.3.9[3]   | Grant set exceeds implementation limit        | E        |

**Subsection §12.4: Preconditions**

| Code    | Section     | Description                                        | Severity |
| ------- | ----------- | -------------------------------------------------- | -------- |
| E12-040 | §12.4.3[1]  | Precondition expression has non-bool type          | E        |
| E12-041 | §12.4.3[2]  | Precondition performs side effects (not pure)      | E        |
| E12-042 | §12.4.3[3]  | Precondition references local binding (not param)  | E        |
| E12-043 | §12.4.6[20] | Contract implementation has stronger precondition  | E        |
| E12-044 | §12.4.3[4]  | Precondition cannot be evaluated (ill-formed expr) | E        |

**Subsection §12.5: Postconditions**

| Code    | Section       | Description                                          | Severity |
| ------- | ------------- | ---------------------------------------------------- | -------- |
| E12-050 | §12.5.4.1[13] | Effectful expression in @old(...)                    | E        |
| E12-051 | §12.5.4.1[14] | @old references local binding (not parameter)        | E        |
| E12-052 | §12.5.6[26]   | Contract implementation has weaker postcondition     | E        |
| E12-053 | §12.5.7[1]    | Postcondition expression has non-bool type           | E        |
| E12-054 | §12.5.7[2]    | Postcondition performs side effects (not pure)       | E        |
| E12-055 | §12.5.7[3]    | Postcondition references local binding               | E        |
| E12-056 | §12.8.4[15]   | Postcondition unprovable in static verification mode | E        |

**Subsection §12.6: Invariants**

| Code    | Section       | Description                                       | Severity |
| ------- | ------------- | ------------------------------------------------- | -------- |
| E12-060 | §12.6.9[1]    | Invariant expression has non-bool type            | E        |
| E12-061 | §12.6.9[2]    | Invariant performs side effects (not pure)        | E        |
| E12-062 | §12.6.9[3]    | Invariant references inaccessible binding         | E        |
| E12-063 | §12.6.9[4]    | @old operator used in where clause                | E        |
| E12-064 | §12.6.9[5]    | result identifier used in type invariant          | E        |
| E12-065 | §12.6.9[6]    | Unsatisfiable type invariant (type uninhabitable) | E        |
| E12-066 | §12.6.2.4[10] | Invariant violated at construction                | E        |
| E12-067 | §12.6.2.4[10] | Invariant violated after mutation                 | E        |
| E12-068 | §12.6.3.3[15] | Loop invariant violated at entry                  | E        |
| E12-069 | §12.6.3.3[15] | Loop invariant not preserved by iteration         | E        |

**Subsection §12.7: Sequent Checking Flow**

| Code    | Section     | Description                                | Severity |
| ------- | ----------- | ------------------------------------------ | -------- |
| E12-070 | §12.7.7[20] | Grant accumulation exceeds declared grants | E        |

**Subsection §12.8: Verification Modes**

| Code    | Section     | Description                                            | Severity |
| ------- | ----------- | ------------------------------------------------------ | -------- |
| E12-080 | §12.8.10[1] | verify attribute misplaced (not before procedure)      | E        |
| E12-081 | §12.8.10[2] | Invalid verification mode (not static/dynamic/trusted) | E        |

**Subsection §12.9: Grammar and Contract Declarations**

| Code    | Section    | Description                                    | Severity |
| ------- | ---------- | ---------------------------------------------- | -------- |
| E12-090 | §12.9.4[7] | Quantifier (forall/exists) unsupported         | E        |
| E12-091 | §12.9.5[9] | Contract procedure has body (must be abstract) | E        |

##### §E.5.1.10 Clause 14: Concurrency and Memory Ordering

**Subsection §14.1: Concurrency Model**

| Code    | Description                                  | Section    |
| ------- | -------------------------------------------- | ---------- |
| E14-100 | Thread return type is not const              | §14.1.7[1] |
| E14-101 | Shared data captured without synchronization | §14.1.7[2] |
| E14-102 | Thread dropped without join or detach        | §14.1.7[4] |
| E14-103 | Thread spawning missing thread::spawn grant  | §14.1.7[3] |
| E14-104 | Thread join missing thread::join grant       | §14.1.7[3] |

**Subsection §14.3: Atomic Operations**

| Code    | Description                                      | Section    |
| ------- | ------------------------------------------------ | ---------- |
| E14-200 | Atomic operation on non-atomic type              | §14.3.4[1] |
| E14-201 | Atomic operation with incorrect permission       | §14.3.4[2] |
| E14-202 | Invalid memory ordering for operation            | §14.3.4[3] |
| E14-203 | Atomic operation on unsupported type (e.g. i128) | §14.3.4[5] |
| E14-204 | Misaligned atomic access                         | §14.3.4[4] |

**Subsection §14.4: Synchronization Primitives**

| Code    | Description                                  | Section    |
| ------- | -------------------------------------------- | ---------- |
| E14-300 | Type constraint violation for sync primitive | §14.4.5[1] |
| E14-301 | Missing grant for synchronization operation  | §14.4.5[3] |

##### §E.5.1.11 Clause 13: Witness System

**Subsection §13.3: Formation and Construction**

| Code    | Description                                                 | Section    |
| ------- | ----------------------------------------------------------- | ---------- |
| E13-001 | Type does not satisfy behavior/satisfy contract/have state  | §13.3.6[1] |
| E13-010 | Missing `move` for responsible witness (@Heap/@Region)      | §13.3.6[2] |
| E13-011 | Region witness escapes region                               | §13.3.6[5] |
| E13-012 | Grant missing for witness construction (alloc::heap/region) | §13.3.6[4] |

**Subsection §13.5: Dispatch Semantics**

| Code    | Description                                 | Section     |
| ------- | ------------------------------------------- | ----------- |
| E13-020 | Method not found in behavior/contract       | §13.5.8[7]  |
| E13-021 | Method not available in modal state         | §13.5.5[10] |
| E13-022 | Insufficient grants for witness method call | §13.5.7[14] |

**Subsection §13.6: Memory Integration**

| Code    | Description                                 | Section     |
| ------- | ------------------------------------------- | ----------- |
| E13-030 | Permission upgrade on witness               | §13.6.4[6]  |
| E13-031 | Witness transition requires grant           | §13.6.7[20] |
| E13-032 | Invalid witness transition (no such method) | §13.6.7[19] |

---

#### §E.5.2 Reserved Ranges [implementation.diagnostics.reserved]

[6] The following diagnostic code ranges are reserved for future clauses:

- **E01-xxx**: Reserved for Clause 01 (Introduction and Conformance). Currently no diagnostics defined.
- **E03-xxx**: Reserved for Clause 03 (Basic Concepts).
- **E14-xxx**: Reserved for Clause 14 (Concurrency and Memory Ordering).
- **E15-xxx**: Reserved for Clause 15 (Interoperability and ABI).
- **E16-xxx**: Reserved for Clause 16 (Compile-Time Evaluation and Reflection).

[7] Implementations shall not define diagnostics in reserved ranges. When new clauses are added, their diagnostic codes shall follow the sequential allocation strategy defined in §1.6.6.

---

#### §E.5.3 Diagnostic Payload Schemas [implementation.diagnostics.payloads]

[8] Diagnostic payloads shall be machine-readable and support structured output formats. This section defines JSON schemas for diagnostic messages.

##### §E.5.3.1 Basic Diagnostic Structure

[9] A conforming diagnostic message in JSON format shall have the following structure:

```json
{
  "code": "E02-001",
  "severity": "error",
  "message": "Invalid UTF-8 sequence in source file",
  "location": {
    "file": "src/main.cursive",
    "line": 42,
    "column": 15,
    "span": {
      "start": { "line": 42, "column": 15 },
      "end": { "line": 42, "column": 18 }
    }
  },
  "notes": [
    {
      "message": "UTF-8 validation failed at byte offset 0x2A",
      "location": null
    }
  ],
  "fix_hints": [
    {
      "message": "Replace invalid sequence with valid UTF-8",
      "span": {
        "start": { "line": 42, "column": 15 },
        "end": { "line": 42, "column": 18 }
      },
      "replacement": "\uFFFD"
    }
  ]
}
```

[10] **Fields**:

- `code` (string, required): The diagnostic code in canonical format `E[CC]-[NNN]`.
- `severity` (string, required): One of `"error"`, `"warning"`, or `"note"`.
- `message` (string, required): Human-readable description of the diagnostic.
- `location` (object, required): Source location where the diagnostic was triggered.
  - `file` (string, required): Absolute or relative path to source file.
  - `line` (integer, required): 1-indexed line number.
  - `column` (integer, required): 1-indexed column number (UTF-8 code units).
  - `span` (object, optional): Start and end positions for multi-character spans.
- `notes` (array, optional): Additional context or explanatory notes.
- `fix_hints` (array, optional): Suggested fixes (compiler may offer automated corrections).

##### §E.5.3.2 Multi-Location Diagnostics

[11] Some diagnostics reference multiple source locations (e.g., "type mismatch: expected X here, found Y there"). In such cases, use the `related_locations` field:

```json
{
  "code": "E07-003",
  "severity": "error",
  "message": "Type mismatch in expression",
  "location": {
    "file": "src/main.cursive",
    "line": 50,
    "column": 10
  },
  "related_locations": [
    {
      "message": "Expected type 'i32' from context",
      "location": {
        "file": "src/main.cursive",
        "line": 45,
        "column": 5
      }
    },
    {
      "message": "Expression has type 'f64'",
      "location": {
        "file": "src/main.cursive",
        "line": 50,
        "column": 10
      }
    }
  ]
}
```

---

#### §E.5.4 Quality Guidelines [implementation.diagnostics.quality]

[12] Implementations should strive to produce high-quality diagnostics that aid users in understanding and correcting errors. This section provides recommendations.

##### §E.5.4.1 Message Clarity

[13] Diagnostic messages should:

- **Be concise**: Avoid verbose explanations in the main message. Use notes for extended context.
- **Be specific**: Identify the exact problem (e.g., "expected ';' after statement" not "syntax error").
- **Be actionable**: Suggest what the user should do to fix the issue.
- **Avoid jargon**: Use terminology from the specification, but explain technical terms when necessary.

**Example (Good)**:

```
error[E05-202]: cannot assign to immutable binding 'x'
  --> src/main.cursive:10:5
   |
10 |     x = 42;
   |     ^^^^^^ assignment to immutable binding
   |
note: consider making the binding mutable with 'var'
  --> src/main.cursive:8:5
   |
8  |     let x = 10;
   |     --- help: change this to 'var x = 10;'
```

**Example (Poor)**:

```
error: illegal operation
```

##### §E.5.4.2 Source Context

[14] Diagnostics should include:

- **Line and column numbers**: 1-indexed for compatibility with editors.
- **Source snippet**: Show the offending line(s) with caret (^) or underline indicating the error span.
- **Surrounding context**: For complex errors, show 1-2 lines before/after for orientation.

##### §E.5.4.3 Suggested Fixes

[15] Where feasible, implementations should provide fix hints (automated or manual). Fix hints shall:

- Be syntactically correct;
- Preserve program semantics where possible;
- Be conservative (avoid guessing user intent when multiple fixes are plausible).

**Example**:

```
error[E02-200]: reserved keyword 'type' cannot be used as identifier
  --> src/main.cursive:5:9
   |
5  |     let type = 42;
   |         ^^^^ reserved keyword
   |
help: use a different identifier, or prefix with 'r#' for raw identifier
   |
5  |     let r#type = 42;
   |         ++++++
```

---

#### §E.5.5 Severity Levels [implementation.diagnostics.severity]

[16] The current specification defines three severity levels:

- **Error (E)**: Violates a normative requirement. Compilation cannot proceed past errors. All diagnostics in this version use severity "E".
- **Warning (W)**: Indicates potentially problematic code that does not violate normative requirements (e.g., unused variable, unreachable code). Reserved for future use.
- **Note (N)**: Provides additional context for errors or warnings. Not standalone diagnostics.

[17] Future versions may introduce warnings for common mistakes or deprecated features. Implementations may define implementation-specific warnings (e.g., performance hints) but shall clearly distinguish them from specification-defined diagnostics.

---

#### §E.5.6 Machine-Readable Output [implementation.diagnostics.output]

[18] Implementations should support at least two output formats:

- **Human-readable text**: Default for interactive terminal use. Supports color, styling, and Unicode.
- **JSON**: Machine-readable structured output for tooling (IDEs, CI/CD, linters).

##### §E.5.6.1 JSON Output Format

[19] When the `--diagnostic-format=json` flag is provided (or equivalent), compilers shall emit one JSON object per diagnostic, separated by newlines (JSON Lines format):

```json
{"code":"E02-001","severity":"error","message":"...","location":{...}}
{"code":"E07-003","severity":"error","message":"...","location":{...}}
```

[20] This format enables efficient streaming and parsing by external tools.

##### §E.5.6.2 Text Output Format

[21] Human-readable text output should follow this general template:

```
<severity>[<code>]: <message>
  --> <file>:<line>:<column>
   |
<line_num> | <source_line>
   | <indicator>
   |
<notes...>
<fix_hints...>
```

**Example**:

```
error[E07-003]: type mismatch in expression
  --> src/main.cursive:50:10
   |
50 |     let x: i32 = 3.14;
   |                  ^^^^ expected 'i32', found 'f64'
   |
note: the expression has type 'f64'
  --> src/main.cursive:50:10
   |
help: use an integer literal instead
   |
50 |     let x: i32 = 3;
   |                  ~
```

---

#### §E.5.7 Diagnostic Registry Maintenance [implementation.diagnostics.maintenance]

[22] The diagnostic code registry in this annex is authoritative. When adding new diagnostics:

1. Allocate the next available sequential code within the appropriate clause and subsection range.
2. Update this registry table with code, section reference, and description.
3. Document the diagnostic in the corresponding specification clause.
4. Add examples demonstrating the error condition.
5. Provide suggested diagnostic message templates.

[23] Deprecated diagnostics shall be marked in the registry but not removed, to preserve historical records and support legacy code analysis tools.

---

**Previous**: Annex E §E.4 (undefined) | **Next**: Annex E §E.6 (undefined)
