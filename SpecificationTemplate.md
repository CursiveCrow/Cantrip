## 4. Section Template Standards

### 4.1 Universal Section Template

Every specification section MUST follow this template:

````markdown
# Part X: [Part Name] - §Y. [Section Title]

**Section**: §Y | **Part**: [Part Name] (Part X)
**Previous**: [Link to previous section] | **Next**: [Link to next section]

---

**Definition Y.1 ([Concept Name]):** [Formal mathematical definition using set theory,
type theory, or operational semantics notation. Should be precise and unambiguous.]

## Y. [Section Title]

### Y.1 Overview

**Purpose:** [What problem does this feature solve? What makes it unique?]

**When to use [feature]:**

- [Use case 1]
- [Use case 2]
- [Use case 3]
- [Specific scenarios where this feature is the right choice]

**When NOT to use [feature]:**

- [Anti-pattern 1] → use [alternative] instead
- [Anti-pattern 2] → use [alternative] instead
- [Scenarios where another approach is better]

**Relationship to other features:**

- **[Related Feature 1] (§X.Y):** [How they interact]
- **[Related Feature 2] (§X.Y):** [How they interact]
- **[Related Feature 3] (§X.Y):** [How they interact]

### Y.2 Syntax

#### Y.2.1 Concrete Syntax

**EBNF Grammar:**

```ebnf
[Production rules in Extended Backus-Naur Form]
```
````

#### Y.2.2 Abstract Syntax

**Formal syntax notation:**

```
[Abstract syntax using mathematical notation]
```

### Y.3 Static Semantics

#### Y.3.1 Well-Formedness Rules

Rules defining syntactically valid programs:

```
[WF-RuleName]
Premise₁
Premise₂
─────────────
Conclusion
```

#### Y.3.2 Type Rules

Type system inference rules:

```
[Type-RuleName]
Γ ⊢ e₁ : τ₁    Γ ⊢ e₂ : τ₂
────────────────────────────
Γ ⊢ e : τ
```

#### Y.3.3 Type Properties

**Theorem Y.3.3.1 ([Property Name]):** [statement of theorem]

**Proof** Y.3.3.2 [formal proof]

**Corollary Y.3.3.2:** [Consequence of theorem]

### Y.4 Dynamic Semantics

#### Y.4.1 Evaluation Rules

Operational semantics:

```
[Eval-RuleName]
e₁ → e₁'
─────────────────
op(e₁, e₂) → op(e₁', e₂)
```

#### Y.4.2 Memory Representation

**Memory Layout:**

```
[Visual diagram or description of memory layout]

Field Layout:
- Field 1: offset 0, size X bytes
- Field 2: offset X, size Y bytes
Total size: Z bytes
Alignment: A bytes
```

**Table Y.4.1: [Type] Memory Characteristics**
| Property | Value | Notes |
|----------|-------|-------|
| Size | X bytes | [Explanation] |
| Alignment | Y bytes | [Explanation] |
| Layout | [description] | [Explanation] |

#### Y.4.3 Operational Behavior

Runtime behavior description:

- Execution model
- Performance characteristics
- Edge cases

### Y.5 Related Sections

- See §X.Y for [related topic]
- Compare with §X.Z for [alternative approach]
- Prerequisites: §A.B, §C.D

### Y.6 Notes and Warnings

**Note Y.7.1:** [Informational note about implementation detail, historical context, or clarification]

**Note Y.7.2:** [Another informational note]

**Performance Note Y.7.1:** [Note about performance characteristics or optimization opportunities]

---

**Previous**: [Link] | **Next**: [Link]

```

```
