# Cursive Language Specification
## Annex E — Implementation Guidance (Informative)

**Part**: Annex
**Section**: Annex E - Implementation Diagnostics
**Stable label**: [implementation.diagnostics]
**Forward references**: None

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

| Code    | Section   | Description                                            | Severity |
|---------|-----------|--------------------------------------------------------|----------|
| E02-001 | §2.1.3[1] | Invalid UTF-8 sequence in source file                  | E        |
| E02-002 | §2.1.3[5] | File size exceeds implementation limit                 | E        |
| E02-003 | §2.1.3[2] | Misplaced byte-order mark (BOM)                        | E        |
| E02-004 | §2.1.3[4] | Prohibited code point (U+0000 null character)          | E        |

**Subsection §2.2: Translation Phases**

| Code    | Section   | Description                                            | Severity |
|---------|-----------|--------------------------------------------------------|----------|
| E02-101 | §2.2.3[4] | Comptime evaluation exceeded maximum iterations        | E        |
| E02-102 | §2.2.3[4] | Comptime evaluation exceeded maximum memory            | E        |
| E02-103 | §2.2.3[4] | Comptime evaluation exceeded maximum stack depth       | E        |
| E02-104 | §2.2.3[4] | Comptime evaluation exceeded time limit                | E        |
| E02-105 | §2.2.4[2] | Semantic analysis encountered circular dependency      | E        |
| E02-106 | §2.2.4[3] | Type checking failed due to unresolved dependency      | E        |
| E02-107 | §2.2.5[1] | Code generation encountered unsupported construct      | E        |

**Subsection §2.3: Lexical Elements**

| Code    | Section   | Description                                            | Severity |
|---------|-----------|--------------------------------------------------------|----------|
| E02-200 | §2.3.2[3] | Reserved keyword used as identifier                    | E        |
| E02-201 | §2.3.3[5] | Invalid character in identifier                        | E        |
| E02-202 | §2.3.4[2] | Unterminated string literal                            | E        |
| E02-203 | §2.3.4[3] | Invalid escape sequence in string literal              | E        |
| E02-204 | §2.3.5[2] | Unterminated character literal                         | E        |
| E02-205 | §2.3.5[3] | Empty character literal                                | E        |
| E02-206 | §2.3.6[3] | Integer literal exceeds maximum value                  | E        |
| E02-207 | §2.3.6[4] | Invalid digit in numeric literal                       | E        |
| E02-208 | §2.3.7[2] | Floating-point literal exceeds representable range     | E        |

**Subsection §2.4: Tokenization and Statement Termination**

| Code    | Section   | Description                                            | Severity |
|---------|-----------|--------------------------------------------------------|----------|
| E02-300 | §2.4.3[3] | Delimiter nesting exceeds maximum depth (256)          | E        |
| E02-301 | §2.4.4[5] | Disallowed form at module scope                        | E        |

**Subsection §2.5: Compilation Units and Top-Level Forms**

| Code    | Section   | Description                                            | Severity |
|---------|-----------|--------------------------------------------------------|----------|
| E02-400 | §2.5.3[4] | Redeclaration in same scope                            | E        |
| E02-401 | §2.5.4[6] | Cyclic module initializer dependency (eager edges)     | E        |

##### §E.5.1.2 Clause 04: Modules

**Subsection §4.1: Module Overview**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-001 | §4.1.3[1]  | Multiple source files merged without permitted constructs           | E        |
| E04-002 | §4.1.3[2]  | Source file located outside manifest-declared source roots          | E        |
| E04-003 | §4.1.3[3]  | Module path component is not a valid identifier                     | E        |
| E04-004 | §4.1.3[5]  | Case-insensitive filesystem collision between module paths          | E        |
| E04-005 | §4.1.3[6]  | Module path component uses a reserved keyword                       | E        |
| E04-006 | §4.1.3[2]  | Project manifest missing or missing required tables                 | E        |
| E04-007 | §4.1.3[5.2]| Case-insensitive filesystem collision warning                       | W        |

**Subsection §4.2: Module Syntax (Import/Use)**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-100 | §4.2.3[4]  | Duplicate entry in `use` list                                       | E        |
| E04-101 | §4.2.3[5]  | Wildcard `use` expansion introduces conflicting bindings            | E        |
| E04-102 | §4.2.3[7]  | Visibility modifier applied to an `import` declaration              | E        |

**Subsection §4.3: Module Scope (Use Bindings)**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-200 | §4.3.2[3]  | `use` binding collides with existing identifier without `as` alias  | E        |
| E04-201 | §4.3.2[4]  | Wildcard `use` expansion conflicts with existing bindings           | E        |
| E04-202 | §4.3.2[7]  | `use` references a module that has not been imported                | E        |
| E04-203 | §4.4.2[4]  | Duplicate exported identifier resulting from `public use`           | E        |
| E04-204 | §4.4.2[2]  | `public use` re-exports a non-public item                           | E        |
| E04-205 | §4.3.2[1]  | `import` refers to module outside manifest-declared roots           | E        |

**Subsection §4.5: Qualified Name Resolution**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-400 | §4.5.3[1]  | Qualified-name head is not a known module or alias                  | E        |
| E04-401 | §4.5.3[3]  | Qualified-name head ambiguous across imports                        | E        |
| E04-402 | §4.5.3[4]  | Partial module path does not resolve to a module                    | E        |
| E04-403 | §4.5.3[5]  | Attempt to rebind an existing import alias                          | E        |
| E04-404 | §4.5.3[2]  | Qualified name references an item not exported by the target module | E        |

**Subsection §4.6: Module Initialization and Cycles**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-500 | §4.6.2[4]  | Cycle detected in eager dependency graph                            | E        |
| E04-501 | §4.6.2[6]  | Module initialization blocked by failing predecessor                | E        |
| E04-502 | §4.6.2[8]  | Access to binding before prerequisite module initialises            | E        |
| E04-503 | §4.6.4.4[8]| Lazy dependency misclassified; eager interaction required           | E        |

**Subsection §4.6.7: Type Visibility Integration**

| Code    | Section    | Description                                                         | Severity |
|---------|------------|---------------------------------------------------------------------|----------|
| E04-700 | §4.6.7[5]  | Internal type referenced across module boundary                     | E        |

##### §E.5.1.3 Clause 05: Declarations

**Subsection §5.2: Variable Bindings**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-201 | §5.2.3[1] | `shadow` used without matching outer binding                   | E        |
| E05-202 | §5.2.3[2] | Reassignment of `let` binding                                  | E        |
| E05-203 | §5.2.3[3] | Duplicate identifiers in pattern binding                       | E        |
| E05-204 | §5.2.3[4] | Pattern initializer arity/shape mismatch                       | E        |

**Subsection §5.3: Binding Patterns**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-301 | §5.3.3[2] | Unknown field in record pattern                                | E        |
| E05-302 | §5.3.3[3] | Tuple pattern arity mismatch                                   | E        |
| E05-303 | §5.3.3[4] | Duplicate identifiers in pattern                               | E        |
| E05-304 | §5.3.3[1] | Missing type annotation for multi-identifier pattern           | E        |

**Subsection §5.4: Procedures**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-401 | §5.4.3[2] | Procedure receiver missing or malformed                        | E        |
| E05-402 | §5.4.3[6] | Comptime callable body not compile-time evaluable              | E        |
| E05-403 | §5.4.3[6] | Missing return type annotation where required                  | E        |
| E05-405 | §5.4.3[3] | Procedure declared without contract specification              | E        |
| E05-406 | §5.4.3[5] | Procedure with empty grant set attempts grant-requiring op     | E        |
| E05-407 | §5.4.3[4] | Expression-bodied procedure has non-empty grant set            | E        |

**Subsection §5.5: Type Declarations**

| Code    | Section    | Description                                                    | Severity |
|---------|------------|----------------------------------------------------------------|----------|
| E05-501 | §5.5.3[11] | Cyclic type alias                                              | E        |
| E05-502 | §5.5.3[10] | Unsupported representation attribute                           | E        |
| E05-503 | §5.5.3[6]  | Duplicate contract reference on type declaration               | E        |
| E05-504 | §5.5.3[7]  | Duplicate predicate reference on type declaration              | E        |
| E05-505 | §5.5.3[13] | Empty modal type declaration                                   | E        |
| E05-506 | §5.5.3[14] | Multiple contracts with incompatible signatures                | E        |
| E05-507 | §5.5.3[15] | Infinite-size recursive type                                   | E        |

**Subsection §5.6: Visibility Rules**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-601 | §5.6.3[1] | `private`/`protected` used at module scope                     | E        |
| E05-602 | §5.6.3[4] | Visibility modifier applied to local binding                   | E        |
| E05-603 | §5.6.5[1] | Attempt to widen visibility during re-export                   | E        |
| E05-604 | §5.6.5[2] | Non-`public` modifier used with `use`                          | E        |
| E05-605 | §5.6.4[3] | Protected member access violation                              | E        |

**Subsection §5.7: Initialization and Definite Assignment**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-701 | §5.7.2[2] | Module initializer cycle                                       | E        |
| E05-702 | §5.7.2[3] | Module initializer blocked by failing dependency               | E        |
| E05-703 | §5.7.3[2] | Use of potentially unassigned binding                          | E        |
| E05-704 | §5.7.4[4] | Pattern omits required binding                                 | E        |
| E05-705 | §5.7.5[1] | Reassignment of `let` binding                                  | E        |

**Subsection §5.8: Program Entry Points**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-801 | §5.8.2[1] | Missing or multiple entry points                               | E        |
| E05-802 | §5.8.2[3] | Entry point not `public`                                       | E        |
| E05-803 | §5.8.2[4] | Entry point declared as procedure/comptime/async               | E        |
| E05-804 | §5.8.2[5] | Disallowed attribute on entry point                            | E        |

**Subsection §5.9: Grant Declarations**

| Code    | Section   | Description                                                    | Severity |
|---------|-----------|----------------------------------------------------------------|----------|
| E05-901 | §5.9.3[3] | Grant name conflicts with reserved namespace                   | E        |
| E05-902 | §5.9.3[4] | Grant declaration not at module scope                          | E        |
| E05-903 | §5.9.3[2] | Duplicate grant name in same module                            | E        |

##### §E.5.1.4 Clause 06: Names, Scopes, and Resolution

**Subsection §6.2: Scope Formation**

| Code    | Section    | Description                                                    | Severity |
|---------|------------|----------------------------------------------------------------|----------|
| E06-201 | §6.2.3[5]  | Scope tree cycle or multiple parents                           | E        |
| E06-202 | §6.2.3[10] | Treating `use` as lexical scope                                | E        |

**Subsection §6.3: Name Introduction and Shadowing**

| Code    | Section    | Description                                                    | Severity |
|---------|------------|----------------------------------------------------------------|----------|
| E06-300 | §6.3.3[6]  | Redeclaration of identifier in same scope                      | E        |
| E06-301 | §6.3.3[7]  | `shadow` used outside nested scope (including module)          | E        |
| E06-302 | §6.3.3[8]  | Redeclaration/shadow/alias of predeclared identifier           | E        |
| E06-303 | §6.3.3[10] | Pattern shadow refers to non-existent outer binding            | E        |

**Subsection §6.4: Name Lookup**

| Code    | Section    | Description                                                    | Severity |
|---------|------------|----------------------------------------------------------------|----------|
| E06-400 | §6.4.4     | Ambiguous identifier (multiple imported bindings)              | E        |
| E06-401 | §6.4.4     | Undefined identifier (not found in any scope)                  | E        |
| E06-402 | §6.4.4[13] | Qualified-name prefix is not module or type                    | E        |
| E06-403 | §6.4.4[13] | Qualified-name suffix is not exported                          | E        |
| E06-404 | §6.4.4[13] | Module-qualified name missing target item                      | E        |
| E06-405 | §6.4.4[13] | Type-qualified name missing associated item                    | E        |

##### §E.5.1.5 Clause 07: Type System

**Subsection §7.2: Primitive Types**

| Code    | Section     | Description                                                      | Severity |
|---------|-------------|------------------------------------------------------------------|----------|
| E07-001 | §7.2.1[5]   | Attempt to redeclare or shadow a reserved primitive identifier   | E        |
| E07-100 | §7.2.2.6    | Integer constant overflow during folding                         | E        |
| E07-101 | §7.2.2.6    | Compile-time division or remainder by zero                       | E        |
| E07-102 | §7.2.3.6    | Invalid floating literal suffix                                  | E        |
| E07-103 | §7.2.3.6    | Floating constant overflow or division by zero                   | E        |
| E07-104 | §7.2.4.4    | Non-boolean operand supplied to logical operator                 | E        |
| E07-105 | §7.2.4.4    | Boolean used where numeric type required                         | E        |
| E07-106 | §7.2.5.2    | Character literal uses surrogate code point                      | E        |

**Subsection §7.4: Function Types**

| Code    | Section     | Description                                           | Severity |
|---------|-------------|-------------------------------------------------------|----------|
| E07-200 | §7.4.4[15]  | Grant set references undeclared grant                 | E        |
| E07-201 | §7.4.4[15]  | Contractual sequent references undefined predicate    | E        |
| E07-202 | §7.4.4[15]  | Call site lacks required grants                       | E        |
| E07-203 | §7.4.4[15]  | Call site cannot prove `must` clause                  | E        |
| E07-204 | §7.4.4[15]  | Callable loses grant/contract info when exported      | E        |
| E07-205 | §7.4.4[15]  | Function subtyping violates variance rules            | E        |
| E07-206 | §7.4.4[15]  | Conversion of capturing closure to raw pointer        | E        |

**Subsection §7.5: Pointer Types**

| Code    | Section   | Description                                                      | Severity |
|---------|-----------|------------------------------------------------------------------|----------|
| E07-300 | §7.5.3.2  | Pointer escapes stack/region provenance                          | E        |
| E07-301 | §7.5.2.4  | Dereference of `Ptr<T>@Null`                                     | E        |
| E07-302 | §7.5.2.3  | Address-of applied to non-storage expression                     | E        |
| E07-303 | §7.5.2.3  | Missing explicit pointer type annotation                         | E        |
| E07-304 | §7.5.2.4  | Dereference of `Ptr<T>@Weak` without upgrade                     | E        |
| E07-305 | §7.5.2.4  | Dereference of `Ptr<T>@Expired`                                  | E        |
| E07-400 | §7.5.5.2  | Raw pointer dereference outside `unsafe` context                 | E        |
| E07-401 | §7.5.5.2  | Raw pointer assignment outside `unsafe` context                  | E        |
| E07-402 | §7.5.5.3  | Cast between incompatible raw pointer types                      | E        |

**Subsection §7.6: Modal Types**

| Code    | Section     | Description                                                      | Severity |
|---------|-------------|------------------------------------------------------------------|----------|
| E07-500 | §7.6.8[13]  | Duplicate modal state name                                       | E        |
| E07-501 | §7.6.8[13]  | Transition references undeclared state                           | E        |
| E07-502 | §7.6.8[13]  | Transition body fails to return the required target state        | E        |
| E07-503 | §7.6.8[13]  | Modal pattern match missing required state branch                | E        |
| E07-504 | §7.6.2[6.3] | State-specific field accessed outside its defining state         | E        |
| E07-505 | §7.6.2[6.1] | Transition signature lacks a matching procedure implementation   | E        |

**Subsection §7.6: Modal Types**

| Code    | Section | Description                                                      | Severity |
|---------|---------|------------------------------------------------------------------|----------|
| E07-500 | §7.6.8  | Duplicate modal state name                                       | E        |
| E07-501 | §7.6.8  | Transition references undeclared state                           | E        |
| E07-502 | §7.6.4  | Transition body fails to produce target state                    | E        |
| E07-503 | §7.6.8  | Modal pattern match missing state branch                         | E        |
| E07-504 | §7.6.6  | State field accessed outside defining state                      | E        |

**Subsection §7.7: Type Relations**

| Code    | Section | Description                                                      | Severity |
|---------|---------|------------------------------------------------------------------|----------|
| E07-700 | §7.7.6  | Subtyping check failure / variance violation                     | E        |
| E07-701 | §7.7.6  | Cyclic type alias                                                | E        |
| E07-702 | §7.7.6  | Type compatibility failure for operation                         | E        |
| E07-703 | §7.7.6  | Modal/union pattern lacks branch                                 | E        |
| E07-704 | §7.7.6  | Union pattern missing component type                             | E        |

**Subsection §7.8: Type Introspection**

| Code    | Section   | Description                                                      | Severity |
|---------|-----------|------------------------------------------------------------------|----------|
| E07-900 | §7.8.2    | `typeof` used in value position                                  | E        |
| E07-901 | §7.8.5    | `type_info` result escapes compile-time context                  | E        |
| E07-902 | §7.8.8    | Introspection invoked on ill-formed type                         | E        |
| E07-903 | §7.8.5    | Attempt to inspect private field via introspection               | E        |

---

#### §E.5.2 Reserved Ranges [implementation.diagnostics.reserved]

[6] The following diagnostic code ranges are reserved for future clauses:

- **E01-xxx**: Reserved for Clause 01 (Introduction and Conformance). Currently no diagnostics defined.
- **E03-xxx**: Reserved for Clause 03 (currently undefined).
- **E08-xxx**: Reserved for Clause 08 (Expressions).
- **E09-xxx**: Reserved for Clause 09 (Statements).
- **E10-xxx**: Reserved for Clause 10 (Compile-time Evaluation).
- **E11-xxx**: Reserved for Clause 11 (Generics).
- **E12-xxx**: Reserved for Clause 12 (Memory Model).
- **E13-xxx**: Reserved for Clause 13 (Contracts).
- **E14-xxx**: Reserved for Clause 14 (Witnesses).
- **E15-xxx**: Reserved for Clause 15 (Concurrency).
- **E16-xxx**: Reserved for Clause 16 (Interoperability).

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
      "start": {"line": 42, "column": 15},
      "end": {"line": 42, "column": 18}
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
        "start": {"line": 42, "column": 15},
        "end": {"line": 42, "column": 18}
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
