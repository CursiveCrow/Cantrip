# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.4: Conformance

**File**: `01-4_Conformance.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-3_Notation-Conventions.md) | **Next**: [[REF_TBD]](01-5_Document-Conventions.md)

---

## 1.4.1 Conformance Requirements

A Cursive implementation **conforms** to this specification if and only if:

1. It accepts all conforming programs ([REF_TBD])
2. It rejects all ill-formed programs with a diagnostic ([REF_TBD])
3. It correctly implements the semantics specified for well-formed programs
4. It documents all implementation-defined behaviors ([REF_TBD])
5. It produces the required diagnostics for violations of static semantic rules
6. Extensions, if any, do not invalidate any conforming program

**Note**: Conformance does not require a particular implementation strategy, optimization level, or runtime performance.

## 1.4.2 Normative Language (shall, shall not, should, may per ISO/IEC Directives)

This specification uses normative keywords as defined by **ISO/IEC Directives, Part 2** to indicate the level of requirement.

### Requirements (shall / shall not)

**shall** / **shall not**

Indicates an absolute requirement or prohibition for conforming implementations.

**Examples**:
- "Conforming implementations shall support UTF-8 encoding" ([REF_TBD])
- "Implementations shall not accept programs with circular type definitions" ([REF_TBD])

**Equivalence**: In RFC 2119 terminology, `shall` ≡ `MUST`, `shall not` ≡ `MUST NOT`.

### Recommendations (should / should not)

**should** / **should not**

Indicates a strong recommendation. Deviations are permitted but must be documented and justified.

**Examples**:
- "Implementations should provide optimizations for pure functions" (informative)
- "Programmers should avoid shadowing predeclared identifiers" ([REF_TBD])

**Equivalence**: In RFC 2119 terminology, `should` ≡ `SHOULD`, `should not` ≡ `SHOULD NOT`.

### Permissions (may / may not)

**may** / **may not**

Indicates permission. Implementations are free to include or omit the described behavior.

**Examples**:
- "Implementations may inline pure functions" (optimization freedom)
- "Implementations may limit recursion depth" (resource limits)

**Equivalence**: In RFC 2119 terminology, `may` ≡ `MAY`, `may not` ≡ `MAY NOT`.

### Capability vs. Requirement (can / cannot)

**can** / **cannot**

Indicates a statement of possibility or impossibility, not a requirement. This usage is **informative**, not normative.

**Examples**:
- "Functions cannot access instance fields" (statement of language semantics)
- "Programmers can use holes for type inference" (capability description)

**Note**: `can` and `cannot` describe what is possible or impossible within the language, but do not impose requirements on implementations.

### Note on RFC 2119 Equivalence

For readers familiar with **RFC 2119** and **RFC 8174** (IETF standards):

| ISO/IEC Directive | RFC 2119/8174 | Meaning |
|-------------------|---------------|---------|
| shall | MUST | Absolute requirement |
| shall not | MUST NOT | Absolute prohibition |
| should | SHOULD | Strong recommendation |
| should not | SHOULD NOT | Strong recommendation against |
| may | MAY | Permission (optional) |

This specification follows ISO/IEC conventions, but the meanings are equivalent to RFC 2119 keywords.

## 1.4.3 Conforming Implementation

A **conforming implementation** is a compiler, interpreter, or other tool that processes Cursive programs and:

1. **Accepts all conforming programs**: Processes syntactically and semantically valid programs according to this specification

2. **Rejects ill-formed programs**: Issues diagnostics for programs that violate static semantic rules ([REF_TBD])

3. **Implements specified semantics**: Produces behavior consistent with the operational semantics defined in this specification, except where undefined behavior occurs ([REF_TBD])

4. **Documents implementation-defined behavior**: Provides documentation for all implementation-defined aspects ([REF_TBD])

5. **Does not break conforming programs**: If the implementation provides extensions, those extensions shall not cause any conforming program to be invalid or change its meaning

### Implementation Freedom

Conforming implementations have freedom in:

- **Internal representations**: AST structure, intermediate representations, type system implementation
- **Optimization strategies**: Inlining, dead code elimination, constant propagation, loop transformations
- **Diagnostic format**: Error message wording, formatting, and presentation (subject to [REF_TBD]
- **Resource limits**: Stack depth, heap size, maximum file size, compilation time limits
- **Performance**: Execution speed, memory usage, code size

Conforming implementations are **not** required to:

- Optimize code in any particular way
- Produce any specific output format (beyond what is observable in program behavior)
- Support specific platforms or architectures
- Provide any particular tooling (IDEs, debuggers, profilers)

### Partial Implementations

Implementations may omit features marked as:

- **[Experimental]**: Features under development ([REF_TBD])
- Platform-specific features (e.g., inline assembly for specific architectures)

Such implementations are not fully conforming but may still be useful. Documentation shall clearly state which features are unsupported.

## 1.4.4 Conforming Program

A **conforming program** is a Cursive program that:

1. **Is syntactically well-formed**: Adheres to the grammar defined in Annex A
2. **Is statically well-formed**: Satisfies all static semantic rules (types, scopes, permissions, effects)
3. **Uses only defined features**: Does not rely on undefined behavior ([REF_TBD])[REF_TBD]
4. **Respects implementation limits**: Does not exceed documented implementation-defined limits
5. **Declares its version**: Specifies the language version (e.g., `version = "1.0"`) if required ([REF_TBD])

**Note**: A conforming program does not contain:
- Syntax errors
- Type errors
- Permission violations
- Effect violations
- Undefined behavior

### Conforming vs. Well-Formed

- **Well-formed**: Satisfies syntax and static semantics
- **Conforming**: Well-formed + avoids undefined behavior + respects limits

All conforming programs are well-formed, but well-formed programs may exhibit undefined behavior (e.g., out-of-bounds array access).

## 1.4.5 Implementation-Defined Behavior

**Implementation-defined behavior** is behavior that may vary between conforming implementations but must be documented.

### Definition

Behavior for which this specification provides a range of acceptable choices, and conforming implementations:

1. **Shall** choose one of the allowed behaviors
2. **Shall** document the choice
3. **Shall** apply the choice consistently

### Examples of Implementation-Defined Behavior

| Aspect | Specification | Implementation-Defined |
|--------|--------------|----------------------|
| Integer sizes | Minimum sizes given (e.g., `i32` ≥ 32 bits) | Exact size (e.g., 32 or 64 bits) |
| Pointer size | Depends on target architecture | `isize`, `usize` size |
| Alignment | Must be valid for type | Exact alignment requirements |
| Maximum file size | Must support reasonable programs | Specific limit (e.g., 1 MiB recommended) |
| Stack depth | Must support reasonable recursion | Exact stack limit |
| Diagnostic format | Must include error code | Exact message wording, formatting |

### Documentation Requirement

Conforming implementations shall provide documentation listing all implementation-defined behaviors, including:

- Integer and floating-point type sizes
- Pointer and reference sizes (`isize`, `usize`)
- Alignment requirements
- Maximum nesting depths (types, expressions, blocks)
- Resource limits (stack size, heap size, compilation time)
- Platform-specific calling conventions ([REF_TBD])
- ABI details ([REF_TBD])

**Format**: Documentation may be in user manuals, compiler documentation, or online references, as long as it is accessible to users.

## 1.4.6 Undefined Behavior

**Undefined behavior** (UB) is behavior for which this specification imposes **no requirements**.

### Definition

When undefined behavior occurs, a conforming implementation may:

- Abort execution
- Continue with arbitrary results
- Appear to execute correctly
- Corrupt memory or data structures
- **Any other behavior** (including security vulnerabilities)

**Warning**: Programs exhibiting undefined behavior are **not conforming**, even if they appear to work correctly on a particular implementation.

### Labeling Convention

Throughout this specification, undefined behavior is labeled with **[UB]** or **[Undefined Behavior]**.

**Example**:
> Accessing an array element outside its bounds is **undefined behavior** [UB].

### Cross-Reference to Annex B.2 Catalog

All undefined behaviors are cataloged in **Annex B.2** with a unique identifier:

**Format**: `[UB: B.2 #N]` where N is the catalog number

**Example**:
> Out-of-bounds array access [UB: B.2 #042]

**Audit Requirement**: All UB instances in this specification **shall** be listed in Annex B.2. Any undefined behavior not cataloged is an error in the specification.

### Common Causes of Undefined Behavior

The following are common sources of undefined behavior (non-exhaustive):

1. **Memory violations**:
   - Out-of-bounds array or slice access ([REF_TBD])
   - Null pointer dereference ([REF_TBD])
   - Use-after-move ([REF_TBD])
   - Region value escape ([REF_TBD])

3. **Concurrency violations** (future version):
   - Data races ([REF_TBD])
   - Accessing mutexes after destruction

4. **Unsafe operations**:
   - Invalid raw pointer dereference ([REF_TBD])
   - Violating safety invariants in `unsafe` blocks ([REF_TBD])

5. **FFI violations**:
   - Calling C functions with incorrect signatures ([REF_TBD])
   - Violating C calling conventions

**Defined arithmetic traps**: Integer overflow, integer division by zero, and IEEE-754 exceptional values are not undefined behavior. Debug configurations shall raise a panic diagnostic, and release configurations shall follow the semantics mandated in Part IV (wrapping integers or producing IEEE-754 results). Implementations shall surface these conditions as diagnosable runtime errors rather than treating them as UB.

### Consequences of Undefined Behavior

Programs with undefined behavior:

- Are **not conforming programs**
- May produce different results on different implementations
- May produce different results on different runs
- May exhibit security vulnerabilities
- **Should be rejected** by static analysis tools where detectable

**Best Practice**: Use static analysis, dynamic checkers, and testing to detect undefined behavior.

## 1.4.7 Unspecified Behavior

**Unspecified behavior** is behavior that may vary between executions or implementations, but is limited to a set of **valid choices**.

### Definition

Behavior for which this specification provides multiple acceptable outcomes, but:

1. All outcomes are well-defined
2. Implementations need not document which outcome occurs
3. Outcome may vary between executions of the same program

### Unspecified vs. Undefined vs. Implementation-Defined

| Category | Variation | Documentation Required | Validity |
|----------|-----------|----------------------|---------|
| **Specified** | No variation | N/A | Always valid |
| **Implementation-Defined** | Between implementations | Yes | Always valid |
| **Unspecified** | Between executions or implementations | No | Always valid |
| **Undefined** | No constraints | N/A | Invalid |

### Examples of Unspecified Behavior

1. **Evaluation order** (where not specified):
   ```cursive
   f(g(), h())  // Order of g() and h() evaluation is unspecified
   ```

2. **Hash table iteration order**:
   ```cursive
   for (key, value) in map {
       // Iteration order is unspecified
   }
   ```

3. **Memory addresses**:
   ```cursive
   let x = 42;
   // Address of x is unspecified (may vary between runs)
   ```

**Note**: In Cursive, evaluation order is generally **specified** left-to-right ([REF_TBD]), so unspecified evaluation order is rare.

## 1.4.8 Diagnosable Rules

### Ill-Formed Programs (Diagnostic Required)

An **ill-formed program** violates a static semantic rule and **shall** be rejected with a diagnostic.

**Diagnostic obligation**: Conforming implementations shall issue a diagnostic message for every ill-formed program, identifying:

1. The nature of the violation
2. The location in the source code (file, line, column)
3. An error code (e.g., `E4001`)

**Examples of ill-formed programs**:

```cursive
// ERROR E4001: Type mismatch
let x: i32 = "hello";

// ERROR E4003: Permission violation
let y: const i32 = 42;
*y = 10;  // Cannot mutate through const permission

// ERROR E4005: Non-exhaustive match
match option {
    Some(x) => x,
    // Missing None case
}
```

### Ill-Formed, No Diagnostic Required (IFNDR)

In rare cases, violations may be **ill-formed, no diagnostic required (IFNDR)**. These are violations that:

- Are difficult or impossible to detect statically
- Violate semantic rules but may be undetectable at compile time

**Treatment**: Implementations are **not required** to diagnose IFNDR violations, but may do so as a quality-of-implementation feature.

**Example**:
```cursive
// IFNDR: Exceeds implementation limits (may not be detectable until link time)
function deeply_nested() {
    if true { if true { if true { /* ... 10,000 levels deep ... */ } } }
}
```

**Note**: IFNDR is rare in Cursive. Most violations are diagnosable.

### Diagnostic Format is Implementation-Defined

While the **obligation** to diagnose is specified, the **format** of diagnostics is implementation-defined ([REF_TBD])

Implementations have freedom in:

- Error message wording
- Formatting and presentation
- Additional context or suggestions
- Color and styling (in terminals)

**Minimum requirement**: Diagnostics shall include:
1. Error code (e.g., `E4001`)
2. Location (file, line, column)
3. Brief description of the violation

**Recommended**: Diagnostics should include:
- Suggested fixes
- Related information (e.g., "type declared here")
- Code snippets with highlighting

## 1.4.9 Diagnostic Guidance

### Relationship to Compiler Diagnostics Reference (Companion, Non-Normative)

This specification defines:

- **Which violations** require diagnostics (normative)
- **Error code ranges** and identifiers (normative)

The **Cursive Compiler Diagnostics Reference** (companion document, informative) provides:

- Suggested diagnostic message wording
- Common causes and solutions
- Examples triggering each diagnostic

**Cross-reference**: See [REF_TBD]

### Diagnostic Quality Expectations

While diagnostic format is implementation-defined, **high-quality diagnostics** are essential for usability.

Implementations should strive to provide:

1. **Clear messages**: Explain the violation in plain language
2. **Actionable suggestions**: Suggest how to fix the error
3. **Context**: Show relevant code with highlighting
4. **Related information**: Point to related declarations or definitions
5. **Consistent formatting**: Use a predictable format

**Example of high-quality diagnostic**:

```
error[E4001]: type mismatch
 --> main.cursive:5:18
  |
5 |     let x: i32 = "hello";
  |                  ^^^^^^^ expected `i32`, found `string`
  |
help: if you meant to use a string, change the type annotation
  |
5 |     let x: string = "hello";
  |            ~~~~~~

error: aborting due to previous error
```

### Implementation Documentation Requirements

Conforming implementations shall document:

1. **Diagnostic format**: Describe the format of error messages
2. **Error code ranges**: List all error codes and their meanings
3. **Severity levels**: Define how errors, warnings, and notes are distinguished
4. **Suppression mechanisms**: If warnings can be suppressed, document how

**Recommended**: Implementations should provide machine-readable diagnostic output (e.g., JSON format) for tool integration.

---

## Summary

| Concept | Definition | Implementation Obligation |
|---------|-----------|--------------------------|
| **Conforming Implementation** | Correctly implements this specification | Shall accept/reject programs correctly |
| **Conforming Program** | Well-formed program without UB | N/A (program property) |
| **shall** / **shall not** | Absolute requirement | Shall comply |
| **should** / **should not** | Strong recommendation | Encouraged, deviations documented |
| **may** / **may not** | Permission | Free choice |
| **Implementation-Defined** | Choice from allowed options | Shall document choice |
| **Undefined Behavior** | No constraints on behavior | No obligation; program is invalid |
| **Unspecified Behavior** | Limited set of valid choices | No documentation required |
| **Ill-Formed** | Static semantic violation | Shall diagnose |
| **IFNDR** | Violation, diagnostic not required | May diagnose (QoI) |

---

**Previous**: [[REF_TBD]](01-3_Notation-Conventions.md) | **Next**: [[REF_TBD]](01-5_Document-Conventions.md)
