# Cantrip Language Specification - Section Structure Standard

**Version:** 1.0
**Date:** 2025-10-22
**Purpose:** Defines the canonical structure for all specification sections

---

## 1. Introduction

This document establishes the standard structure that ALL sections of the Cantrip Language Specification must follow. This standard is based on research into formal language specifications, including:

- **The Definition of Standard ML** (Milner, Tofte, Harper, MacQueen) - The gold standard for formal language specification
- **ISO/IEC Directives, Part 2** - Principles for structuring ISO/IEC documents
- **Rust Reference** and **Ferrocene Language Specification** - Modern approaches to systems language specification
- **Academic type theory literature** - Formal methods for specifying type systems

### 1.1 Goals

- **Consistency:** All sections follow the same organizational pattern
- **Navigability:** Readers know where to find syntax, semantics, examples, and verification
- **Formal Rigor:** Clear separation of concerns (syntax vs. semantics vs. verification)
- **Pedagogical:** Examples support formal rules without overwhelming them
- **Standards Compliance:** Follows ISO/IEC guidelines and academic best practices

### 1.2 Scope

This structure applies to:
- All Type System sections (Part II)
- All language feature sections throughout the specification
- New sections being written
- Existing sections during migration/refactoring

---

## 2. Research Foundation

### 2.1 The Definition of Standard ML

The SML Definition organizes each language feature into:

1. **Syntax of the Core** - Reserved words, identifiers, grammar, syntactic restrictions
2. **Syntax of Modules** - Grammar for modules, syntactic restrictions
3. **Static Semantics for the Core** - Type structures, type environments, inference rules
4. **Static Semantics for Modules** - Signature matching, inference rules
5. **Dynamic Semantics** - Evaluation rules and operational semantics

**Key insight:** Rigorous separation between syntax (what can be written), static semantics (what is well-typed), and dynamic semantics (what it means/does).

### 2.2 ISO/IEC Directives

ISO standards for programming languages establish:

- **Clear scope statements** at the beginning of each section
- **Normative vs. informative** content separation
- **Formal rules precede examples**
- **Cross-references** to related sections
- **Consistent numbering** and hierarchy

### 2.3 Type Theory Literature

Formal type system specifications typically include:

- **Typing judgments** (Γ ⊢ e : T)
- **Well-formedness rules** (T well-formed)
- **Operational semantics** (⟨e, σ⟩ ⇓ ⟨v, σ'⟩)
- **Soundness theorems** (Progress and Preservation)

---

## 3. Standard Section Structure

Every specification section MUST follow this structure:

```markdown
# Part [X]: [Part Name] - §N. [Feature Name]

**Section**: §N | **Part**: [Part Name] (Part [X])
**Previous**: [Link to previous section] | **Next**: [Link to next section]

---

**Definition N.1 ([Feature]):** [Concise formal definition of the feature]

## N. [Feature Name]

### N.1 Overview

**Key innovation/purpose:** [One sentence describing why this feature exists]

**When to use [feature]:**
- [Use case 1]
- [Use case 2]
- [Use case 3]

**When NOT to use [feature]:**
- [Alternative 1]
- [Alternative 2]

**Relationship to other features:** [How this connects to other language features]

### N.2 Syntax

#### N.2.1 Concrete Syntax

**Grammar:**
```ebnf
[BNF or EBNF grammar rules]
```

**Syntax:**
```cantrip
[Cantrip code showing the syntax]
```

#### N.2.2 Abstract Syntax

**Formal representation:**
```
[Mathematical notation for abstract syntax]
```

**Components:**
- [Component 1]: [Description]
- [Component 2]: [Description]

#### N.2.3 Basic Examples

```cantrip
[Simple, clear example demonstrating basic syntax]
```

**Explanation:** [What this example shows]

### N.3 Static Semantics

#### N.3.1 Well-Formedness Rules

**Definition N.2 (Well-Formedness):** [What it means for this construct to be well-formed]

```
[Well-Formed-Rule]
premises about context and components
─────────────────────────────────────
conclusion: construct is well-formed
```

#### N.3.2 Type Rules

**[T-Rule-Name] [Description]:**
```
[T-Rule-Name]
premises: type judgments for components
─────────────────────────────────────
conclusion: type judgment for whole construct
```

**Explanation:** [What this rule establishes]

```cantrip
[Code example illustrating this type rule]
```

**[Additional type rules as needed]**

#### N.3.3 Type Properties

**Theorem N.1 ([Property Name]):** [Statement of property]

**Proof sketch:** [If applicable]

### N.4 Dynamic Semantics

#### N.4.1 Evaluation Rules

**[E-Rule-Name] [Description]:**
```
[E-Rule-Name]
evaluation premises
⟨expression, state⟩ ⇓ ⟨value, state'⟩
─────────────────────────────────────
evaluation conclusion
```

**Explanation:** [What happens at runtime]

#### N.4.2 Memory Representation

[If applicable]

**Layout:**
```
[Description of memory layout]
```

**Size:** [Formula or description]
**Alignment:** [Formula or description]

#### N.4.3 Operational Behavior

[Description of runtime behavior, evaluation order, side effects, etc.]

### N.5 Additional Properties

[This section is OPTIONAL and included only when relevant]

#### N.5.1 Invariants

[If the feature maintains invariants]

**Invariant N.1:** [Statement of invariant]

#### N.5.2 Verification

[If verification is relevant to this feature]

**Verification conditions:**
```
[VC-Name]
[Verification condition formula]
```

#### N.5.3 Algorithms

[If specific algorithms are needed - e.g., exhaustiveness checking, type inference]

**Algorithm N.1 ([Name]):**
```
[Pseudocode or description]
```

### N.6 Advanced Features

[Optional section for advanced or derived constructs]

#### N.6.1 [Advanced Feature 1]

[Syntax, semantics, examples]

#### N.6.2 [Advanced Feature 2]

[Syntax, semantics, examples]

### N.7 Examples and Patterns

[Comprehensive, realistic examples showing idiomatic usage]

#### N.7.1 [Pattern Name 1]

**Pattern:** [Description]

```cantrip
[Complete, realistic example]
```

**Explanation:** [When and why to use this pattern]

#### N.7.2 [Pattern Name 2]

[Continue as needed, but keep concise - 2-4 patterns maximum]

---

**Previous**: [Link] | **Next**: [Link]
```

---

## 4. Component Descriptions

### 4.1 Definition (Required)

**Purpose:** Provide a concise, formal mathematical definition of the feature.

**Format:** `**Definition N.X ([Feature]):** [Mathematical or precise English statement]`

**Guidelines:**
- Must appear immediately after the section header
- Should be 1-2 sentences
- Should use mathematical notation where appropriate
- Should capture the essence of the feature formally

**Examples:**
- Good: "**Definition 7.1 (Enum):** An enum is a sum type (tagged union) with named variants."
- Good: "**Definition 10.1 (Modal):** A modal is a type with an explicit finite state machine, where each state can have different data and the compiler enforces valid state transitions."
- Bad: "Enums are a way to define types that can be one of several variants" (too informal)

### 4.2 Overview (Required)

**Purpose:** Motivate the feature and explain when to use it.

**Must include:**
- **Key innovation/purpose** - Why this feature exists (1 sentence)
- **When to use** - Bulleted list of use cases
- **When NOT to use** - What alternatives exist and when they're better
- **Relationships** - How this connects to other features

**Guidelines:**
- Keep brief (< 50 lines)
- Focus on practical motivation, not implementation
- Provide clear decision criteria

### 4.3 Syntax (Required)

**Purpose:** Define what can be written and what it means abstractly.

**Must include:**
- **Concrete Syntax (N.2.1)** - BNF/EBNF grammar + Cantrip code examples
- **Abstract Syntax (N.2.2)** - Mathematical representation
- **Basic Examples (N.2.3)** - Simple code showing syntax

**Guidelines:**
- Use EBNF notation consistently
- Abstract syntax should use mathematical notation
- Examples should be minimal and clear

### 4.4 Static Semantics (Required)

**Purpose:** Define type rules and compile-time checking.

**Must include:**
- **Well-Formedness Rules (N.3.1)** - When is this construct valid?
- **Type Rules (N.3.2)** - How are types assigned?
- **Type Properties (N.3.3)** - What guarantees does the type system provide?

**Format for rules:**
```
[Rule-Name]
premise₁
premise₂
...
─────────────────
conclusion
```

**Guidelines:**
- Use inference rule notation
- Name rules consistently (T-* for type rules, WF-* for well-formedness)
- Provide explanation after each rule
- Include code example demonstrating the rule

### 4.5 Dynamic Semantics (Required)

**Purpose:** Define runtime behavior.

**Must include:**
- **Evaluation Rules (N.4.1)** - How does this evaluate?
- **Memory Representation (N.4.2)** - How is it laid out in memory? [if applicable]
- **Operational Behavior (N.4.3)** - What happens at runtime?

**Format for evaluation rules:**
```
[E-Rule-Name]
evaluation premises
⟨expression, state⟩ ⇓ ⟨value, state'⟩
```

**Guidelines:**
- Use operational semantics notation
- Explain evaluation order and side effects
- Include memory layout details where relevant

### 4.6 Additional Properties (Optional)

**Purpose:** Document invariants, verification, and algorithms specific to this feature.

**Include when:**
- The feature maintains invariants (e.g., modals, contracts)
- Verification is essential (e.g., modals, effects)
- Specific algorithms are needed (e.g., exhaustiveness checking, type inference)

**Guidelines:**
- Only include what's necessary for this specific feature
- Don't repeat general type system properties
- Be concise

### 4.7 Advanced Features (Optional)

**Purpose:** Document derived forms, sugar, or advanced variations.

**Include when:**
- There are derived constructs built on the basic form
- There are advanced use cases that need explanation
- There are optional features or extensions

**Guidelines:**
- Keep separate from basic features
- Use subsections for each advanced feature
- Follow the same syntax → semantics → example pattern

### 4.8 Examples and Patterns (Optional but Recommended)

**Purpose:** Show realistic, idiomatic usage.

**Guidelines:**
- Keep to 2-4 patterns maximum
- Each pattern should be complete and runnable
- Focus on common patterns, not exhaustive coverage
- Explain WHEN and WHY to use each pattern
- Avoid redundancy with earlier examples

---

## 5. Section Ordering Principles

### 5.1 Mandatory Order

These components MUST appear in this order:

1. Definition (at top, right after header)
2. Overview
3. Syntax
4. Static Semantics
5. Dynamic Semantics

**Rationale:** This follows the standard order from formal semantics:
- Syntax defines what can be written
- Static semantics defines what is well-typed
- Dynamic semantics defines what it means

### 5.2 Optional Components

Optional components appear AFTER the mandatory components:

6. Additional Properties (if needed)
7. Advanced Features (if needed)
8. Examples and Patterns (recommended)

### 5.3 Do Not Reorder

**Never:**
- Put examples before syntax
- Put dynamic semantics before static semantics
- Mix type rules with evaluation rules
- Bury verification in the middle of examples

**Rationale:** Consistent ordering makes the specification navigable and predictable.

---

## 6. Examples of Structure Application

### 6.1 Before: Poor Structure

```markdown
## 10. Modals

### 10.1 Overview
[motivation]

### 10.2 Basic Syntax
[syntax + examples mixed]

### 10.3 Using Modals
[more examples]

### 10.4 State-Dependent Data
[examples]

### 10.5 Complex State Machines
[examples]

### 10.6 State Unions
[type rule buried in examples]

...

### 10.9 Verification
[type rules for verification, buried after 400 lines]

### 10.10 Flow Analysis
[more type rules, scattered]

### 10.11 Common Patterns (300 lines!)
[dominates the section]
```

**Problems:**
- Type rules scattered across multiple sections
- Verification buried after examples
- Examples dominate formal content
- No clear Static vs. Dynamic semantics separation

### 6.2 After: Good Structure

```markdown
## 10. Modals: State Machines as Types

### 10.1 Overview
[concise motivation]

### 10.2 Syntax
  10.2.1 Concrete Syntax [grammar + code]
  10.2.2 Abstract Syntax [state machine graph]
  10.2.3 Basic Examples [simple File example]

### 10.3 Static Semantics
  10.3.1 Well-Formedness [modal well-formed rules]
  10.3.2 Type Rules [all T-* rules together]
  10.3.3 State Invariants [formal invariant rules]

### 10.4 Dynamic Semantics
  10.4.1 State Transitions [evaluation]
  10.4.2 Linear State Transfer [rebinding]
  10.4.3 Memory Layout [if applicable]

### 10.5 Verification
  10.5.1 Verification Conditions
  10.5.2 Static Checking
  10.5.3 Runtime Checks

### 10.6 Advanced Features
  10.6.1 State Unions [type rule + example]
  10.6.2 Common Fields [syntax + semantics]
  10.6.3 Reachability Analysis [algorithm]

### 10.7 Examples and Patterns (condensed to ~100 lines)
  10.7.1 Resource Lifecycle Pattern
  10.7.2 Request-Response Pattern
```

**Improvements:**
- All type rules in Static Semantics (10.3)
- All evaluation in Dynamic Semantics (10.4)
- Verification clearly separated (10.5)
- Patterns condensed and at end (10.7)
- Clear hierarchical organization

---

## 7. Quick Reference Checklist

When writing or migrating a section, ensure:

- [ ] **Definition** at the very top (after section header)
- [ ] **Overview** with motivation, use cases, when NOT to use
- [ ] **Syntax** section with:
  - [ ] Concrete syntax (grammar + code)
  - [ ] Abstract syntax (mathematical)
  - [ ] Basic examples
- [ ] **Static Semantics** section with:
  - [ ] Well-formedness rules
  - [ ] Type rules (all together)
  - [ ] Properties/theorems
- [ ] **Dynamic Semantics** section with:
  - [ ] Evaluation rules
  - [ ] Memory layout (if applicable)
  - [ ] Operational behavior
- [ ] **Additional Properties** (only if needed):
  - [ ] Invariants
  - [ ] Verification
  - [ ] Algorithms
- [ ] **Advanced Features** (only if applicable)
- [ ] **Examples and Patterns** (2-4 patterns, concise)
- [ ] **Navigation links** at top and bottom
- [ ] **Consistent numbering** (N.1, N.2, N.3...)

---

## 8. Special Cases

### 8.1 Pure Formalism Sections

Some sections (like §5 Type Rules and Semantics) may be pure formalism with no concrete syntax.

**Approach:**
- Still use the structure, but Syntax section may reference other sections
- Focus on Static Semantics (judgment forms, meta-theory)
- Include many worked examples showing type derivations

### 8.2 Library Features

Sections describing standard library features (not language features) may:
- De-emphasize formal semantics
- Focus more on API and usage
- Still follow: Definition → Interface → Semantics → Examples

### 8.3 Very Small Features

Features that are very simple may:
- Combine subsections (e.g., Syntax and Static Semantics in one)
- Omit Dynamic Semantics if it's trivial
- Still maintain the overall ordering

**Rule:** When in doubt, include all sections even if brief. It's better to have an empty "Dynamic Semantics" section with "No special runtime behavior" than to omit it and create inconsistency.

---

## 9. Migration Strategy

When migrating existing sections to this structure:

### 9.1 Analysis Phase

1. Read the current section completely
2. Identify all components:
   - Where is syntax defined?
   - Where are type rules?
   - Where are examples?
   - What's missing?

### 9.2 Extraction Phase

1. Extract components into a working document:
   - All syntax → Syntax section
   - All type rules → Static Semantics section
   - All evaluation rules → Dynamic Semantics section
   - All examples → Examples section

### 9.3 Reorganization Phase

1. Create new section with standard structure
2. Place extracted components in correct sections
3. Add missing components (definitions, abstract syntax, etc.)
4. Write transitional text as needed

### 9.4 Refinement Phase

1. Remove redundancy
2. Ensure examples support rules
3. Check cross-references
4. Verify numbering

### 9.5 Priority Order

Migrate sections in this priority:

1. **High priority:** Modals (most complex, most benefit)
2. **Medium priority:** Enums, Traits, Generics (scattered type rules)
3. **Low priority:** Primitives, Records (already fairly good)

---

## 10. Enforcement

### 10.1 Review Criteria

Every specification section will be reviewed against this standard.

**Blocking issues:**
- Missing required sections
- Wrong ordering of sections
- Type rules not in Static Semantics
- Examples before formalism

**Non-blocking issues:**
- Minor numbering inconsistencies
- Verbose examples
- Missing optional sections

### 10.2 Continuous Improvement

This standard is a living document. As the specification evolves:
- Update this document with lessons learned
- Add more special cases as they arise
- Refine guidelines based on experience

---

## 11. References

1. **Milner, R., Tofte, M., Harper, R., MacQueen, D.** (1997). *The Definition of Standard ML (Revised)*. MIT Press.
2. **ISO/IEC Directives, Part 2**: Principles and rules for the structure and drafting of ISO and IEC documents.
3. **Rust Reference**: https://doc.rust-lang.org/reference/
4. **Ferrocene Language Specification**: https://github.com/rust-lang/fls
5. **Pierce, B. C.** (2002). *Types and Programming Languages*. MIT Press.
6. **Harper, R.** (2016). *Practical Foundations for Programming Languages* (2nd ed.). Cambridge University Press.

---

**End of Document**
