# Cursive Language Specification Style Guide

**Version:** 1.0
**Last Updated:** 2025-11-09
**Applies to:** All chapters and annexes of the Cursive specification

This guide establishes conventions for writing and maintaining the Cursive language specification. Follow these rules to ensure consistency, clarity, and professionalism.

---

## 1. Document Structure

### 1.1 Terminology

**Use:**

- **Chapter** for top-level divisions (Chapter 1, Chapter 2, etc.)
- **Section** for subdivisions (§1.1, §7.2, etc.)
- **Annex** for back matter (Annex A, Annex B, etc.)

**Don't use:**

- "Clause" (too formal/ISO-specific)
- "Subclause" (use "section" or just the number)
- "Appendix" (use "Annex")

**Example:**

```markdown
✅ Chapter 7 defines the type system. See §7.2 for primitive types.
❌ Clause 7 defines the type system. See subclause 7.2 for primitive types.
```

### 1.2 Heading Format

**Format:** `## X.Y Title [label]`

**Example:**

```markdown
# Chapter 7: Types

## 7.1 Type Foundations [type.foundations]

### 7.1.1 Basic Concepts [type.foundations.basic]
```

**Rules:**

- Always include section numbers in headings
- Always include stable labels in square brackets
- Use sentence case for titles, not title case
- Use descriptive, not generic, labels

### 1.3 Section Numbering

**Never use paragraph numbering:**

```markdown
❌ [1] This section defines...
❌ [2] The specification governs...

✅ This section defines...
✅ The specification governs...
```

**Use list items for enumeration:**

```markdown
✅ The specification covers:

- lexical structure and translation phases;
- foundational concepts and memory model;
- type system and expressions.
```

---

## 2. Normative Language

### 2.1 Keywords

**Absolute requirements:**

- **must** / **must not** — for requirements and prohibitions

**Recommendations:**

- **should** / **should not** — for strong recommendations

**Permissions:**

- **may** / **may not** — for optional features

**Informative:**

- **can** / **cannot** — for capability statements

**Example:**

```markdown
✅ Implementations must reject ill-formed programs.
✅ Compilers should provide helpful error messages.
✅ Implementations may support additional features.
✅ Unicode identifiers can include emoji characters.

❌ Implementations shall reject ill-formed programs.
```

### 2.2 Avoid Double Negatives

```markdown
❌ must never
❌ should never
❌ may never

✅ must not
✅ should not
✅ may not
```

### 2.3 Active Voice

Prefer active voice over passive voice:

```markdown
✅ The compiler must reject invalid programs.
❌ Invalid programs must be rejected.

✅ Implementations should provide clear diagnostics.
❌ Clear diagnostics should be provided.
```

---

## 3. Cross-References

### 3.1 Format

**Within text:**

```markdown
✅ See §7.2 [type.primitive] for details.
✅ The FFI (§15.1 [interop.ffi]) relies on C conventions.
✅ Defined in §1.5.3 [intro.conformance.impldef].

❌ See 7.2 for details. (missing §)
❌ See Section 7.2 for details. (verbose)
```

**Chapters:**

```markdown
✅ Chapter 7 defines the type system.
✅ Chapters 4–8 cover declarations and expressions.

❌ Clause 7 defines the type system.
```

**Annexes:**

```markdown
✅ The grammar appears in Annex A [grammar].
✅ Undefined behavior is catalogued in Annex B.2 [behavior.undefined].

❌ The grammar appears in Appendix A.
```

### 3.2 Forward References

Mark forward references explicitly at the chapter start:

```markdown
**Forward references**: §2.1–§2.5 [lex.source]–[lex.units],
Chapter 3 [basic], Chapters 4–10, §11–§14, §16.6 [interop.abi],
Annex B.2 [behavior.undefined]
```

---

## 4. Spelling and Grammar

### 4.1 Spelling Variant

**Use American English spelling:**

```markdown
✅ initialization, behavior, color, optimize, analyze
❌ initialisation, behaviour, colour, optimise, analyse
```

**Exception:** Keep original spelling in quoted standards (e.g., "ISO/IEC" citations).

### 4.2 Consistency

**Standard terms:**

- `UTF-8` (not `utf-8` or `UTF8`)
- `type system` (not `type-system`)
- `well-formed` (hyphenated when used as adjective)
- `ill-formed` (hyphenated)

### 4.3 Articles

Include articles for clarity:

```markdown
✅ Grammar uses the standard meta-symbols.
❌ Grammar uses standard meta-symbols.

✅ The manifest must contain a version entry.
❌ Manifest must contain a version entry.
```

### 4.4 Relative Clauses

Prefer relative clauses for clarity:

```markdown
✅ Each rule must be followed by prose that explains its effect.
❌ Each rule must be followed by prose explaining its effect.
```

---

## 5. Mathematical Notation

### 5.1 Inline Math

Use single `$...$` for inline mathematics:

```markdown
✅ The type $\tau$ is inferred from context $\Gamma$.
✅ Let $x : T$ denote a binding.
```

### 5.2 Display Math

Use double `$$...$$` for display mathematics:

```markdown
$$
\frac{\Gamma \vdash e : \tau}{\Gamma \vdash e : \tau'}
\quad \text{[T-Sub]}
$$
```

### 5.3 Inference Rules

**Format:**

```markdown
$$
\frac{\text{premise}_1 \quad \cdots \quad \text{premise}_n}{\text{conclusion}} \quad \text{[Rule-Name]}
$$

where $\Gamma$ is a well-formed type environment.

Prose explanation of what the rule means.
```

**Rules:**

- Place rule tag `[T-Feature]` in `\text{}` after the rule
- Use `where` clause for side conditions (unindented)
- Always follow with prose explanation
- Rule names use format: `[Prefix-Feature-Case]`

### 5.4 Where Clauses

**Single condition:**

```markdown
$$
\frac{\Gamma \vdash e : \tau}{\Gamma \vdash e : \tau'}
$$

where $\tau <: \tau'$.
```

**Multiple conditions (as list):**

```markdown
$$
\frac{\Gamma \vdash e : \tau}{\Gamma \vdash e : \tau'}
$$

where:

- $\tau <: \tau'$
- $\text{wf}(\Gamma)$ holds
- $\tau' \in \text{Types}$
```

### 5.5 Metavariable Conventions

**Standard metavariables:**

- $x, y, z$ — identifiers
- $T, U, V$ — concrete type names
- $\tau, \tau', \tau_1$ — types in formal rules
- $e, f, g$ — expressions
- $p$ — patterns
- $s$ — statements
- $\Gamma, \Sigma, \Delta$ — contexts (type, state, region)
- $G$ — grant sets
- $\rho$ — regions

**Be consistent:** Use these conventions throughout.

---

## 6. Code Examples

### 6.1 Code Fences

**Use language-specific fences:**

````markdown
```cursive
let x: Int = 42;
```
````

```

```

```ebnf
type_declaration
    ::= 'type' identifier '=' type
```

```algorithm
function infer_type(expr):
    match expr:
        | IntLiteral(n) => Int
        | StrLiteral(s) => Str
```

````

### 6.2 Example Numbering

**Format:** `**Example X.Y.Z.N**` where X.Y.Z is the section number:

```markdown
**Example 7.2.1.1** (Integer literal typing):

```cursive
let x = 42;  // Type: Int
````

```

```

The literal `42` has type `Int` by default.

````

### 6.3 Invalid Examples

Mark invalid code clearly:

```markdown
```cursive
let x: Int = "string";  // ERROR E07-042: Type mismatch
                        // Expected: Int
                        // Found:    Str
````

```

**Error:** Type mismatch.
**Reason:** String literals cannot be assigned to `Int` bindings.

```

---

## 7. Terms and Definitions

### 7.1 Format

**Format:** `**term** — definition (reference).`

```markdown
**binding** — the association between an identifier and an entity
within a particular scope.

**grant** — a token representing authority to perform a capability
such as file I/O (§12.3 [contract.grant]).
```

### 7.2 Emphasis

- **Bold** for the term being defined
- Use `code` for syntax elements
- Use _italics_ sparingly (reserved for _ill-formed_ and emphasis)

```markdown
✅ **permission** — a capability (`const`, `shared`, `unique`)
describing how a value may be accessed.

❌ _permission_ - a capability (const, shared, unique) describing...
```

---

## 8. Lists and Bullets

### 8.1 Punctuation

**Use semicolons for list items that are sentence fragments:**

```markdown
The specification covers:

- lexical structure and translation phases;
- foundational concepts and memory model;
- type system and expressions.
```

**Use periods for complete sentences:**

```markdown
Implementations must:

- Accept all conforming programs.
- Reject ill-formed programs with diagnostics.
- Document implementation-defined behavior.
```

### 8.2 Parallelism

Keep list items grammatically parallel:

```markdown
✅ Requirements:

- Accept conforming programs
- Reject ill-formed code
- Document behavior

❌ Requirements:

- Accept conforming programs
- Ill-formed code must be rejected
- Documentation of behavior
```

---

## 9. Prose Style

### 9.1 Clarity Over Brevity

Favor clarity over conciseness:

```markdown
✅ Diagnostics include errors (which cause compilation to fail) and
warnings (which do not prevent compilation).

❌ Diagnostics include errors (rejecting) and warnings (permissive).
```

### 9.2 Avoid Jargon

Explain technical terms on first use:

```markdown
✅ The principal type — the most general type that satisfies all
constraints — is unique when type inference succeeds.

❌ The principal type is unique when type inference succeeds.
```

### 9.3 Transitions

Use proper transition words:

```markdown
✅ Diagnostic style is implementation-defined; however, tools should
strive for clarity.

❌ Diagnostic style is implementation-defined, yet tools should strive
for clarity.
```

### 9.4 Avoid Anthropomorphism

Don't attribute human qualities to code:

```markdown
✅ The compiler rejects invalid programs.
❌ The compiler refuses to accept invalid programs.

✅ Implementations must document their choices.
❌ Implementations must explain their decisions.
```

---

## 10. Normative vs Informative

### 10.1 Marking Informative Content

> [informative!]
> This section provides guidance on best practices for implementing the type checker. Implementations are not required to follow this advice.

### 10.2 Notes and Rationale

> [note!]
> Warning and note diagnostics may use prefix `W` or `N` in future versions.

> [rationale!]
> Separating stable, preview, and experimental features allows for controlled evolution while maintaining backward compatibility.

> [warning!]
> Warning and note diagnostics may use prefix `W` or `N` in future versions.

---

## 11. Common Mistakes to Avoid

### 11.1 Don't

❌ Use paragraph numbers `[1]`, `[2]`
❌ Use "shall" or "shall not"
❌ Use "Clause" or "subclause"
❌ Mix British and American spelling
❌ Write "must never" (use "must not")
❌ Reference editorial standards (ISO Directives, RFC 2119, ECMA-334)
❌ Use passive voice unnecessarily
❌ Start sentences with "It is" or "There are"
❌ Use "this document" (use "this specification")

### 11.2 Do

✅ Use clear, descriptive headings
✅ Include stable labels `[label]` everywhere
✅ Use American English spelling
✅ Follow math notation conventions
✅ Provide prose explanations after formal rules
✅ Mark forward references explicitly
✅ Use active voice
✅ Define terms before using them

---

## 12. File Naming and Organization

### 12.1 File Names

**Format:** `NN_Title-With-Hyphens.md` or `NN_Title/NN-N_Subsection.md`

```
✅ 07_Types/07-1_Type-Foundations.md
✅ 07_Types/07-2_Primitive-Types.md

❌ 07_types.md
❌ types-chapter.md
```

### 12.2 Directory Structure

```
Cursive-Language-Specification/
├── 01_Introduction-and-Conformance/
│   └── 01_Introduction-and-Conformance.md
├── 07_Types/
│   ├── 07-1_Type-Foundations.md
│   ├── 07-2_Primitive-Types.md
│   └── 07-3_Composite-Types.md
├── Annex/
│   ├── A_Grammar.md
│   └── B_Undefined-Behavior.md
└── STYLE-GUIDE.md
```

---

## 13. Checklist for New Content

Before committing new specification content, verify:

- [ ] Uses "Chapter" not "Clause"
- [ ] Uses "must/should/may" not "shall"
- [ ] No paragraph numbering `[1]`, `[2]`
- [ ] American English spelling (initialization, behavior)
- [ ] Section numbers in all headings
- [ ] Stable labels `[label]` in all headings
- [ ] Proper cross-reference format `§X.Y [label]`
- [ ] Code examples use proper fences (``` cursive`,  ``ebnf`)
- [ ] Math uses `$...$` and `$$...$$`
- [ ] Inference rules followed by prose
- [ ] Forward references marked at chapter start
- [ ] Active voice preferred
- [ ] No editorial meta-commentary
- [ ] Terms defined before use
- [ ] Consistent terminology throughout

---

## 14. Version History

| Version | Date       | Changes                                          |
| ------- | ---------- | ------------------------------------------------ |
| 1.0     | 2025-11-09 | Initial style guide based on Chapter 1 revisions |

---

**Questions?** This guide is a living document. If you encounter situations not covered here, document the decision and update this guide accordingly.

```

```

```

```
