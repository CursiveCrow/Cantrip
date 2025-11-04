# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.5: Document Conventions

**File**: `01-5_Document-Conventions.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-4_Conformance.md) | **Next**: [[REF_TBD]](01-6_Versioning-Evolution.md)

---

This section defines the organizational and presentation conventions used throughout this specification.

## 1.5.1 Cross-References

### Cross-Reference Format

Cross-references use the following formats:

| Format | Meaning | Example |
|--------|---------|---------|
| `[REF_TBD]` | Section X.Y in the current part | `[REF_TBD]` (Section 2.1) |
| `[REF_TBD]` | Subsection X.Y.Z | `[REF_TBD]` (Subsection 3.4.2) |
| `Part X [REF_TBD]` | Section Y.Z in Part X | `Part VI [REF_TBD]` |
| `Annex X [REF_TBD]` | Section Y in Annex X | `Annex A [REF_TBD]` |
| `Annex X` | Entire annex | `Annex B` (Portability and Behavior) |

**Examples**:
- "See [REF_TBD]"
- "Permission system is defined in Part VI [REF_TBD]"
- "Complete grammar is in Annex A"

### Cross-Reference Requirements

**Normative requirement**: Every dependency between sections **shall** cite the target section.

**Forward references** (references to later sections) shall be:
1. Clearly marked as forward references
2. Limited to essential dependencies
3. Documented in [REF_TBD]

**Circular dependencies** (when unavoidable) shall be:
1. Documented with bidirectional cross-references
2. Explained in the relevant sections
3. Listed in [REF_TBD]

### Bidirectional Cross-Referencing

Key definitions list the sections that reference them:

**Example** (hypothetical):

> **Definition 3.1 (Primitive Types):** ...
>
> **Referenced in**: [REF_TBD], [REF_TBD], [REF_TBD]

This helps readers understand the impact and usage of each definition.

### Digital Hyperlinks Required in Electronic Versions

In PDF and HTML versions of this specification:

- All cross-references **shall** be hyperlinks
- Clicking a reference shall navigate to the target section
- Back-navigation should be supported (browser/reader dependent)

**Quality of implementation**: Hyperlinks should open in the same window/tab with appropriate navigation history.

## 1.5.1a Cross-Reference Validation

### Validation Requirements

Before publication, all cross-references shall be validated:

1. **All references shall resolve** to existing sections
2. **Dead references** (pointing to nonexistent sections) are **prohibited**
3. **Orphan detection**: Sections with zero inbound references should be flagged for review

### Automated Validation Tooling Required

The specification project **should** include automated tools that:

- Parse all cross-references
- Verify targets exist
- Generate reference resolution tables
- Report dead links and orphans

**Recommended**: Run validation on every commit/build.

### Reference Resolution Tables

For maintenance, a reference resolution table **may** be generated listing:

- Each section
- All sections it references (outbound links)
- All sections that reference it (inbound links)

**Example** (fragment):

| Section | Outbound References | Inbound References |
|---------|--------------------|--------------------|
| [REF_TBD]| [REF_TBD], [REF_TBD]| [REF_TBD], [REF_TBD], [REF_TBD]|
| [REF_TBD]| [REF_TBD], [REF_TBD]| [REF_TBD], [REF_TBD]|

## 1.5.2 Examples (Informative)

### Example Purpose and Status

Examples illustrate the specification but **do not impose normative requirements**.

**Example status**: Informative (unless explicitly stated otherwise)

**Purpose**:
- Clarify abstract rules with concrete code
- Show idiomatic usage
- Demonstrate corner cases

### Example Coverage Requirements

**Non-trivial semantics should include examples**:
- If a rule is complex or subtle, provide an example
- If a feature has unexpected behavior, show an example

**Complex features must include examples**:
- Major language features (e.g., modals, effects, contracts) require multiple examples
- Edge cases and interactions require examples

### Example Formatting

**Code examples** use fenced code blocks with the `cursive` language marker:

```cursive
function add(x: i32, y: i32): i32 {
    result x + y
}
```

**Correctness markers** indicate valid or invalid code:

- ✅ **Valid code** (well-formed, conforming)
- ❌ **Invalid code** (ill-formed, should produce diagnostic)

**Example with markers**:

```cursive
// ✅ Valid: immutable binding with unique permission
let x: unique i32 = 42;

// ❌ Invalid: type mismatch
let y: i32 = "hello";  // ERROR E4001
```

**Diagnostic annotations** show expected error codes:

```cursive
let x: i32 = "hello";  // ERROR E4001: type mismatch
```

### Example Quality

**Minimal yet complete**: Examples should be:
- As simple as possible to illustrate the point
- Complete enough to understand without extensive context
- Self-contained (minimize dependencies on other examples)

**Consistent naming**: Use consistent conventions:
- Variables: `x`, `y`, `z`, `value`, `result`
- Functions: `add`, `multiply`, `process`
- Types: `Point`, `Rectangle`, `FileHandle`

## 1.5.3 Notes (Informative)

### Note Format

Notes provide additional information, clarifications, or guidance.

**Formatting**:

> **Note**: This is a note providing additional context.

Or:

> **Note (Rationale)**: This note explains the design rationale.

**Note (cross-reference to ISO conventions)**: Notes follow ISO/IEC Directives, Part 2 conventions for informative content.

### Note Status

Notes are **informative** and do not impose normative requirements, even when appearing in normative sections.

**Examples of note usage**:
- Design rationale
- Historical context
- Common pitfalls
- Relationship to other languages
- Future directions

### Warnings and Cautions

For important warnings:

> **Warning**: Programs with undefined behavior are invalid, even if they appear to execute correctly.

For implementation cautions:

> **Caution**: Implementers should carefully validate region escape to prevent memory unsafety.

## 1.5.4 Section Organization

### Hierarchical Structure

This specification uses hierarchical section numbering:

```
Part I: Introduction and Conformance
  [REF_TBD]
    [REF_TBD]
    [REF_TBD]
  [REF_TBD]
    [REF_TBD]/IEC Standards
    [REF_TBD]
```

### Section Components

Each major section typically includes:

1. **Title and metadata**: Section number, file, version, navigation
2. **Overview**: Brief introduction to the section's purpose
3. **Normative content**: Rules, definitions, constraints
4. **Examples**: Illustrative code (informative)
5. **Cross-references**: Links to related sections
6. **Notes**: Additional guidance (informative)

### Typical Section Template

```markdown
# Part X: Title
## Section X.Y: Subsection Title

**File**: `XX-Y_Title.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]] | **Next**: [[REF_TBD]+1]

---

## X.Y.1 Subsection

[Normative content]

**Example**:
```cursive
[Example code]
```

**Note**: [Informative note]

---

**Previous**: [[REF_TBD]] | **Next**: [[REF_TBD]+1]
```

## 1.5.4a Grammar Presentation

### Dual Presentation

Grammar is presented in **two places**:

1. **In-context** (within semantic sections)
2. **Consolidated** (Annex A)

### In-Context Grammar Requirements

Sections introducing syntactic constructs **shall** include relevant grammar excerpts.

**Purpose**:
- Provide immediate syntactic reference
- Show grammar in context of semantic rules
- Improve readability

**Format**:

```ebnf
FunctionDecl ::= "function" Ident "(" ParamList? ")" ":" Type Block
```

**Typical excerpt size**: 2-5 productions per section

### In-Line Grammar Formatting

In-context grammar uses EBNF notation ([REF_TBD]):

- Fenced code blocks with `ebnf` marker
- Concise, focused excerpts
- Cross-reference to Annex A for complete productions

**Example**:

> Function declarations use the following syntax:
>
> ```ebnf
> FunctionDecl ::= "function" Ident "(" ParamList? ")" ":" Type Block
> ```
>
> See Annex A.6 for complete function declaration grammar.

### Grammar Synchronization

**Authoritative source**: Annex A is the **authoritative** grammar.

**Synchronization requirement**:
- In-context grammar **must** match Annex A
- Discrepancies are errors in the specification
- Automated validation should verify consistency

**Quality of implementation**: Build systems should validate that all grammar excerpts match the consolidated grammar.

## 1.5.5 Forward References and Dependencies

### Forward Reference Policy

This specification strives for **progressive complexity**: foundational concepts precede advanced features.

However, some forward references are unavoidable due to:
- Interdependent features (e.g., types and expressions)
- Pedagogical ordering (e.g., introducing concepts before formalizing them)

### Documented Forward References

Major forward references are documented below:

| Section | Forward Reference | Target | Justification |
|---------|------------------|--------|---------------|
| [REF_TBD]| Mentions types | [REF_TBD]| Need to reference types when defining scope rules |
| [REF_TBD]| Mentions effects | [REF_TBD]| Function types include effect annotations |
| [REF_TBD]| Mentions permissions | [REF_TBD]| Expression typing requires permission system |
| [REF_TBD]| Mentions contracts | [REF_TBD]| Functions can have contract clauses |

**Guideline**: Forward references should be limited to essential dependencies and should not create circular reasoning.

### Circular Dependencies

Some features have inherent circular dependencies:

- **Types and expressions**: Expressions have types; types contain expressions (e.g., array size)
- **Permissions and types**: Types have permissions; permissions are checked during typing
- **Effects and expressions**: Expressions have effects; effect handlers contain expressions

**Resolution strategy**:
1. Define each feature independently
2. Define integration in a separate section
3. Use formal foundations to break circularity (e.g., mutually recursive definitions)

**Example**: Part III defines types, Part IV defines expressions, and [REF_TBD]

### Dependency Graph

A complete dependency graph for major sections is provided in **Annex D.0** (Implementation Notes).

## 1.5.6 Reading Guide for Different Audiences

### For Implementers

**Recommended reading order**:
1. Part I (Introduction and Conformance) — understanding requirements
2. [REF_TBD]— learning formal notation
3. Part II (Basics) — lexical structure, scopes, modules
4. Part III (Type System) — type checking and inference
5. Part IV-V (Expressions and Statements) — semantics
6. Part VI (Memory Model and Permissions) — memory safety
7. Annex A (Grammar) — complete reference
8. Remaining parts as needed

**Key sections**:
- [REF_TBD]
- [REF_TBD]
- Annex A (Grammar)
- Annex B (Behavior catalog)

### For Tool Developers

**Recommended reading order**:
1. Part I [REF_TBD]
2. Part II [REF_TBD]
3. Part II [REF_TBD]
4. Part III (Type System)
5. Annex A (Grammar)

**Key sections**:
- Annex A (Grammar) — complete syntactic reference
- Part II [REF_TBD]
- Part III (Type System) — type checking
- Part VII (Declarations, Scope, Modules)

### For Language Designers and Researchers

**Recommended reading order**:
1. Part I (Introduction) — design philosophy
2. [REF_TBD]— formal foundations
3. Part III [REF_TBD]
4. Part VI (Memory Model and Permissions)
5. Part IX (Effect System)
6. Part X (Contract System)
7. Part XI (Advanced Features)
8. Annex C (Formal Semantics)

**Key sections**:
- [REF_TBD]
- Annex C (Formal Semantics)
- Part XI (Advanced Features) — modals, witnesses, holes

### For Programmers

**Recommended**: Consult the **Cursive Programming Guide** (companion document, informative) rather than this specification.

If reading this specification directly:
1. Part I [REF_TBD]
2. Part II [REF_TBD]— syntax basics
3. Part III [REF_TBD]— primitive and composite types
4. Part IV-V (Expressions and Statements) — writing code
5. Part VII [REF_TBD]— functions and variables

---

**Previous**: [[REF_TBD]](01-4_Conformance.md) | **Next**: [[REF_TBD]](01-6_Versioning-Evolution.md)
