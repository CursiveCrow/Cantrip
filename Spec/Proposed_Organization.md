# Cursive Language Specification — Organization

**Version**: 3.3 (ISO Style with Embedded Formal Elements)
**Date**: 2025-11-05
**Status**: Proposal (ISO-Ready)
**Purpose**: Formal specification aligned with ISO/ECMA standards practice
**Review**: Evaluated against C++, C, Rust, Go, Zig standards (Grade: A, ISO-ready with industry best practices integrated)
**Update**: Updated Universal Section Template to use ISO-style numbered paragraphs with embedded formal elements (definitions, theorems, typing rules), following ISO/IEC 14882 (C++) model for optimal balance of accessibility and mathematical rigor

---

## Organization Principles

1. **Standards Alignment**: Structure follows ISO/IEC and ECMA language specification conventions
2. **Normative Precision**: Uses ISO normative language (shall/shall not/should/may per ISO/IEC Directives)
3. **Progressive Complexity**: Foundational concepts precede advanced features, with minimal forward references (documented in §1.5.5)
4. **Modular Files**: Each major section (X.Y) has its own file for maintainability and version control
5. **Clear Conformance Boundary**: Explicit separation of normative and informative content
6. **Complete Coverage**: All essential language aspects specified with precision
7. **Explicit Dependencies**: Forward references and circular dependencies are documented with clear cross-references
8. **Usability and Clarity**: Complex rules accompanied by informative examples; grammar presented both in-context and consolidated (Annex A)
9. **Future-Proofing**: Edition system enables language evolution while maintaining backward compatibility
10. **Consistent Section Structure**: All specification sections follow a universal template with standardized content ordering (definition → syntax → semantics → examples → integration), numbered paragraphs for traceability, and progressive disclosure from common to advanced cases (see "Section-Level Content Organization" below)

---

## Section-Level Content Organization

This section defines how to structure content **within** individual specification sections, based on best practices from ISO/IEC 14882 (C++), ECMA-334 (C#), and the Rust Reference.

### Content Structure Principles

1. **Definition First**: Begin with clear, concise definition establishing what the feature is
2. **Syntax Early**: Present formal grammar after conceptual introduction
3. **Progressive Detail**: Move from simple/common cases to complex/edge cases
4. **Examples Illuminate**: Place examples immediately after the concepts they demonstrate
5. **Numbered Paragraphs**: Use sequential numbering for traceability and cross-referencing
6. **Layered Information**: Structure content in progressive layers (essential → important → advanced)

### Multi-Level Nesting Guidelines

**Nesting Depth Standards**:
- **Standard sections**: 2-3 levels (e.g., §3.1, §3.1.1, §3.1.1.1)
- **Complex multi-category sections**: 3-4 levels permitted (e.g., §3.2, §3.2.2, §3.2.2.1, §3.2.2.1.1)
- **Maximum depth**: 5 levels per ISO 2145:1978 standard
- **ISO/IEC compliance**: Cursive follows ISO/IEC Directives Part 2 for document structure

**When to Use Deeper Nesting** (3-4 levels):
- Sections covering multiple distinct sub-categories (e.g., §3.2 Primitive Types covering 6 type families)
- Each sub-category requires comprehensive specification following the universal template pattern
- The added organizational clarity outweighs the complexity of deeper numbering
- Precedent: ISO/IEC 14882 (C++) uses 4 levels in complex sections (e.g., §23.3.13.6 for sequence container requirements)

**Rationale**:
- **C++ Standard**: Uses 2 levels for simple sections, 4 levels for complex multi-faceted sections
- **C# Standard (ECMA-334)**: Uses 3 levels for type system (e.g., 11.1.4 Integral types)
- **Cursive approach**: Balances detail (more than C++) with clarity (within ISO 5-level limit)
- **Justification**: Cursive's integration of permissions, grants, contracts, and formal semantics requires more detailed organization than typical language specifications

**Numbering Pattern** for multi-category sections:
```
X.Y Section Name
├── X.Y.1 Overview (cross-cutting concerns)
├── X.Y.2 Category A
│   ├── X.Y.2.1 Overview
│   ├── X.Y.2.2 Syntax
│   ├── X.Y.2.3 Semantics
│   └── X.Y.2.N Advanced Topics
├── X.Y.3 Category B (follows same internal structure)
└── X.Y.M Integration (how categories interact)
```

### Universal Section Template

Every normative section should follow this structure combining **ISO-style numbered paragraphs** with **embedded formal elements** (following ISO/IEC 14882 C++ Standard model):

```markdown
# N. [Topic Name] [stable-label]

## N.1 Overview [stable-label.overview]

[1] [One-sentence definition establishing what this is]

**Definition N.1.1** (*Feature Name*):
A [feature] is [formal mathematical definition using precise notation].

[2] [Purpose and context - why this feature exists]

[Optional: Simple motivating example]
```cursive
// Immediate practical demonstration
```

## N.2 Syntax [stable-label.syntax]

[3] The syntax of [feature] shall conform to the following grammar.

**Syntax:**
```
production_rule
    : alternative_1
    | alternative_2 optional_part?
    | alternative_3
    ;
```

[4] A [production_rule] shall satisfy the following well-formedness constraints:
    (4.1) [First constraint]
    (4.2) [Second constraint]

**Formal Well-Formedness Rule:**

$$
{\small
\dfrac{\text{Premise 1} \quad \text{Premise 2}}{\Gamma \vdash \text{production wf}}
}
\tag{Rule-Name}
$$

[Cross-reference to complete grammar in Annex A §A.X]

## N.3 [Main Concept 1] [stable-label.concept1]

[5] [Basic semantic rule - common case, using "shall" for requirements]

**Type Formation Rule:**

$$
{\small
\dfrac{\Gamma \vdash \text{component}_1 : \tau_1 \quad \Gamma \vdash \text{component}_2 : \tau_2}{\Gamma \vdash \text{construct}(\text{component}_1, \text{component}_2) : \tau}
}
\tag{Formation-Rule-Name}
$$

[6] The type of [construct] shall be determined according to the formation rule above.

**Example N**: [Descriptive title]
```cursive
// Clear, compilable code demonstrating paragraph [5] and formation rule
let example: Type = construct(value1, value2);
```

This demonstrates [specific aspect]. The type is [τ] as determined by the formation rule.

[7] Additional semantic constraints shall apply:
    (7.1) [First constraint with normative "shall"]
    (7.2) [Second constraint]

**Theorem N.1** (*Soundness Property*):
If Γ ⊢ e : τ according to the formation rules, then e evaluates to a value of type τ.

*Proof sketch*: By structural induction on typing derivation. See Annex C.X for complete proof.

> **Note**: This property ensures type safety as specified in §1.0.6.

[8] [More detailed semantics]

**Example N+1**: [Another aspect]
```cursive
// Demonstrating additional behavior from paragraph [8]
```

## N.4 [Main Concept 2] [stable-label.concept2]

[9] [Related semantic rules with "shall"]

**Evaluation Semantics:**

$$
{\small
\dfrac{e_1 \Downarrow v_1 \quad e_2 \Downarrow v_2}{\text{construct}(e_1, e_2) \Downarrow \text{construct\_value}(v_1, v_2)}
}
\tag{Eval-Rule-Name}
$$

[10] Constraints and restrictions shall be:
    (10.1) [Constraint condition]
    (10.2) [Restriction]

> **Warning**: [Safety-critical information, undefined behavior conditions]

**Example N+2**: [Error case] (invalid)
```cursive
// Code that violates rule from paragraph [10.1]
// error: explanation of why this is ill-formed
```

This is invalid because it violates the constraint specified in paragraph [10.1].

## N.5 [Edge Cases / Special Topics] [stable-label.special]

[11] [Less common scenarios]

[12] [Advanced interactions]

## N.6 Integration [stable-label.integration]

[13] This feature interacts with the following language components:

**Cross-references**:
- See §X.Y for [related topic]
- As described in §A.B, ...
- Cross-reference to Part IV §C.D for [memory/permission interaction]

[14] The interaction with [other feature] shall satisfy the compatibility requirements specified in §X.Y.
```

### Applying Template to Multi-Category Sections

For sections that cover multiple distinct sub-categories (e.g., §3.2 Primitive Types covering 6 type families), apply the universal template at two levels:

**Pattern Structure:**
```markdown
## X.Y [Section Name] [stable-label]

### X.Y.1 Overview [stable-label.overview]

[1] [One-sentence definition of the overall section scope]

[2] This section specifies N categories:
    (2.1) Category A (§X.Y.2)
    (2.2) Category B (§X.Y.3)
    ...
    (2.N) Category N (§X.Y.M-1)

[3] [Common properties across all categories]

**Cross-references:** §X.Y.M for integration

### X.Y.2 [Category A Name] [stable-label.category-a]

#### X.Y.2.1 Overview [stable-label.category-a.overview]

[4] [One-sentence definition of Category A]

**Definition X.Y.2.1** (*Category A Term*):
[Formal definition]

[5] [Purpose and context]

#### X.Y.2.2 Syntax [stable-label.category-a.syntax]

[6] The syntax of [Category A] shall conform to the following grammar.

**Syntax:**
```
[grammar production rules]
```

#### X.Y.2.3 [Main Concept] [stable-label.category-a.concept]

[7] [Semantic rules with formation/typing rules]

**Example X**: [Demonstrating concept]
```cursive
// Code example
```

[Continue following universal template pattern...]

### X.Y.3 [Category B Name] [stable-label.category-b]

[Apply same internal structure as X.Y.2]

### X.Y.M Integration [stable-label.integration]

[Final section showing how all categories interact]
```

**Key Principles:**
1. **Top-level overview** (X.Y.1) introduces all categories
2. **Each category** (X.Y.2, X.Y.3, ...) follows the full universal template internally
3. **Integration section** (X.Y.M) at end shows cross-category interactions
4. **Stable labels** follow hierarchical pattern: `[parent.category.subcategory]`
5. **Numbering depth**: 4 levels maximum (X.Y.Z.W) within ISO 2145 limits

**Example from Specification:**
- §3.2 Primitive Types uses this pattern
- §3.2.1 Overview introduces 6 primitive type categories
- §3.2.2-3.2.7 each specify one type category with full detail
- §3.2.8 Integration shows how primitive types interact with permission/grant systems

**When NOT to use this pattern:**
- Sections with only 1-2 major concepts (use standard 2-3 level template)
- Sections where categories share significant semantics (consolidate instead)
- Purely conceptual sections without distinct implementable categories

### Documentation Elements

#### Syntax Grammar Formatting

**EBNF Notation** (defined in §1.3.1):
- **Non-terminals**: `production_rule`, `type_expression`
- **Optional elements**: `element?` (zero or one)
- **Repetition**: `element*` (zero or more), `element+` (one or more)
- **Alternatives**: `|` separator
- **Terminals**: Keywords in quotes `'let'`, `'fn'`
- **Grouping**: Parentheses `( ... )`

**Example:**
```
variable_declaration
    : 'let' identifier (':' type_expression)? '=' expression
    | 'var' identifier (':' type_expression)? '=' expression
    ;
```

#### Semantic Rules Formatting

**Normative Language** (per ISO/IEC Directives Part 2 external standard, §1.4.2):
- **shall** / **shall not** — Mandatory requirement
- **should** / **should not** — Recommendation (strongly advised)
- **may** / **may not** — Permission (optional)
- **can** / **cannot** — Capability (informative, not normative)

**Numbered Paragraphs**:
- Use sequential numbering `[1]`, `[2]`, `[3]` within each section
- Use sub-numbering `(4.1)`, `(4.2)` for multi-part conditions
- Enables precise cross-referencing: "See §3.2 paragraph 5"

**Example (Hybrid Approach):**
```markdown
[4] An integer literal shall have a type determined by its suffix or context.

**Definition 3.2.5** (*Integer Literal Type*):
The type of an integer literal lit is:
- iN or uN if lit has explicit suffix (e.g., 42i32, 255u8)
- type inferred from context if no suffix
- i32 if no suffix and context does not constrain type

[5] An integer literal without a suffix shall be:
    (5.1) assigned type τ if context requires type τ and value fits in τ, or
    (5.2) assigned default type i32 if context does not constrain type.

**Typing Rule:**

$$
{\small
\dfrac{\text{value} \in \text{range}(\text{i32}) \quad \Gamma \text{ does not constrain literal type}}{\Gamma \vdash \text{value} : \text{i32}}
}
\tag{T-IntLit-Default}
$$

[6] If the literal value does not fit in the target type, the program shall be ill-formed.

**Example 3**: Integer literal typing
```cursive
let x: i32 = 42;        // Rule [T-IntLit-Default] with explicit context
let y = 42;             // Rule [T-IntLit-Default], defaults to i32
let z: i64 = 1000;      // Context constrains to i64
```

**Example 4**: Type mismatch (invalid)
```cursive
let w: u8 = 256;        // error: value 256 does not fit in u8
```
This violates paragraph [6] because 256 ∉ range(u8).
```

#### Example Formatting

**Requirements**:
- **Placement**: Immediately after the concept being demonstrated
- **Compilability**: All examples should be compilable (or clearly marked as errors)
- **Numbering**: Sequential within each section
- **Titles**: Descriptive titles indicating what is demonstrated
- **Comments**: Inline comments explaining behavior
- **Both paths**: Show correct usage AND common errors

**Format:**
```markdown
**Example N**: [Descriptive title]
```cursive
let x: i32 = 42;        // explicit type annotation
let y = 42;             // type inferred as i32
// Both are valid and equivalent
```

**Example N+1**: [Invalid case]
```cursive
let z: i32;             // error: missing initializer
```
This is ill-formed because §X.Y requires an initial value.
```

**Markers**:
- Mark valid/recommended patterns with "(valid)" or no marker
- Mark invalid/discouraged patterns with "(invalid)" or "(error)"
- Keep examples minimal yet complete
- Progress from simple to complex

**Quantity Guidance**:
- **Major concepts**: Exactly 1 canonical example demonstrating the primary use case
- **Complex features**: 2-3 examples (common case, edge case, error case)
- **Error scenarios**: 1 example showing typical mistake with clear "(invalid)" or "(error)" marker
- **Progressive series**: Simple → intermediate → advanced (when teaching progression)

**Canonical Example Characteristics**:
- Self-contained and minimal (5-15 lines typical)
- Demonstrates the 80% use case
- Uses naming conventions from §1.5.2a
- Becomes the reference implementation for that concept
- Compilable (or clearly marked as demonstrating an error)

#### Notes and Warnings

**Note Format** (Informative):
```markdown
> **Note**: Clarifying information that aids understanding but does not
> impose normative requirements. May include rationale, implementation
> guidance, or comparisons to other languages.
```

**Warning Format** (Safety-Critical):
```markdown
> **Warning**: [Undefined Behavior] Violating this constraint results in
> undefined behavior [UB: B.2 #N]. See Annex B.2 for catalog.
```

**Rationale Format** (Design Decisions):
```markdown
> **Rationale**: This restriction ensures [goal] and prevents [problem].
> Alternative designs were considered but rejected because [reason].
```

**Placement**: Immediately after the rule or concept being clarified.

#### Cross-Reference Format

**Section References**:
- Within same part: `§3.2`, `§4.5.1`
- Across parts: `Part IV §4.2`, `Part III §3.8`
- Annexes: `Annex A §A.3`, `Annex B.2`
- Specific paragraphs: `§3.2 paragraph 5`, `§3.2[5]`

**Bidirectional Linking**:
- Forward reference: "See §X.Y for detailed semantics"
- Backward reference: "As described in §A.B, ..."
- Integration reference: "Cross-reference to Part IV for memory model interaction"

**Hyperlinks**: In digital versions, all cross-references shall be clickable hyperlinks.

### Progressive Disclosure Patterns

#### Layer 1: Essential Definition (What?)
```markdown
[1] A [term] is [concise definition in one sentence].
```

#### Layer 2: Basic Syntax (How to write?)
```markdown
**Syntax:**
```
basic_construct
    : common_form
    ;
```
```

#### Layer 3: Core Semantics (How does it behave?)
```markdown
[2] [Basic rule for the 80% common case]

**Example**: [Common usage]
```

#### Layer 4: Constraints and Rules (What are the limits?)
```markdown
[3] [Requirements using "shall"]
[4] [Restrictions and conditions]
```

#### Layer 5: Advanced Topics (What about edge cases?)
```markdown
## N.5 Advanced [Topic]

[5] [Less common scenarios]
[6] [Complex interactions]
```

#### Layer 6: Integration (How does it fit together?)
```markdown
## N.6 Integration

[7] This feature interacts with:
- [Feature A]: See §X.Y
- [Feature B]: See §P.Q
```

### Section Flow Best Practices

#### Opening Structure

**DO:**
- Start with clear, one-sentence definition
- Provide immediate context (why this exists)
- Include simple example early (especially for practical features)
- Establish scope and boundaries

**DON'T:**
- Dive into technical details immediately
- Assume reader knows related concepts
- Start with exceptions or edge cases
- Use undefined terms in opening

#### Ordering Within Sections

**Standard Flow:**
1. **Definition** (1-2 paragraphs)
2. **Syntax** (formal grammar)
3. **Basic Semantics** (common cases, numbered paragraphs)
4. **Detailed Semantics** (complex interactions, constraints)
5. **Examples** (interspersed after relevant concepts)
6. **Notes** (clarifications where needed)
7. **Edge Cases** (advanced topics, special conditions)
8. **Integration** (cross-references, interactions)

**Progressive Complexity:**
- Start with most common use case (80/20 rule)
- Introduce basic rules before exceptions
- Show concrete examples before abstractions
- Present familiar concepts before novel ones

#### Multi-Part Rules

When a rule has multiple conditions, use sub-numbering:

```markdown
[4] An expression is well-formed if and only if:
    (4.1) all subexpressions are well-formed,
    (4.2) the types of operands match operator requirements, and
    (4.3) no undefined behavior conditions are triggered.
```

#### Example Placement

**DO:**
- Place examples immediately after the concept they illustrate
- Show both valid and invalid usage
- Progress from simple to complex
- Include explanatory comments
- Number examples for reference

**DON'T:**
- Batch all examples at end of section
- Show only "happy path" code
- Use contrived, unrealistic examples
- Omit expected output or behavior

### Comparative Note

This section-level organization draws on proven patterns from:

- **C++ Standard (ISO/IEC 14882)**: Numbered paragraphs, normative language, **embedded formal semantics in boxes**, distributed integration
- **Rust Reference**: Syntax-first presentation, compilable examples, safety warnings
- **C# Specification (ECMA-334)**: Hierarchical structure, example integration, clear terminology

Cursive adopts a **hybrid approach** that combines:
- **ISO/IEC numbered paragraphs** for conformance clarity and standards compliance
- **Embedded formal elements** (definitions, theorems, inference rules) for mathematical precision
- **Practical-first prose** (C#-inspired) for accessibility
- **Rigorous formalism** (C++-inspired) for type system correctness
- **Safety emphasis** (Rust-inspired) with formal verification support

This balances accessibility for developers, precision for implementers, and formalism for researchers.

---

## Companion Documents

The following documents accompany this specification:

**Normative (Separate Document):**
- **Cursive Standard Library Specification** - Detailed specification of the standard library; maintained separately from language specification for independent evolution

**Non-Normative:**
- **Cursive Programming Guide** - Practical guidance, patterns, idioms, and examples for language users
- **Cursive LLM Development Guide** - AI-assisted development and code generation guidance
- **Cursive Compiler Diagnostics Reference** - Complete catalog of compiler error codes and warnings with suggested diagnostic messages

---

## Part Structure

### **PART I: INTRODUCTION AND CONFORMANCE** *(Normative)*

#### **Section 1.0: Scope**
**File**: `01-0_Scope.md`

- 1.0.1 Purpose of this specification
- 1.0.2 Scope and applicability
- 1.0.3 Target audiences (implementers, tool developers, language designers)
- 1.0.4 Relationship to companion documents
  - Standard Library Specification (normative, separate)
  - Programming Guide (informative)
  - LLM Development Guide (informative)
  - Compiler Diagnostics Reference (informative)

#### **Section 1.1: Normative References**
**File**: `01-1_Normative-References.md`

- 1.1.1 ISO/IEC standards
- 1.1.2 Unicode standard
- 1.1.3 Other normative references

#### **Section 1.2: Terms and Definitions**
**File**: `01-2_Terms-Definitions.md`

- 1.2.1 General terms
- 1.2.2 Language-specific terms
- 1.2.3 Technical terms
- 1.2.4 Symbols and abbreviated terms

#### **Section 1.3: Notation and Conventions**
**File**: `01-3_Notation-Conventions.md`

- 1.3.1 Grammar Notation (EBNF)
- 1.3.2 Metavariables
- 1.3.3 Mathematical Notation
- 1.3.4 Judgment Forms
- 1.3.5 Inference Rule Format
- 1.3.6 Typography and Formatting Conventions

#### **Section 1.4: Conformance**
**File**: `01-4_Conformance.md`

- 1.4.1 Conformance Requirements
- 1.4.2 Normative Language (shall, shall not, should, may per ISO/IEC Directives)
  - Requirements (shall/shall not)
  - Recommendations (should/should not)
  - Permissions (may)
  - Capability vs. requirement (can vs. shall)
  - Note on RFC 2119 equivalence
  - **Application in sections**: See "Section-Level Content Organization" → "Documentation Elements" → "Semantic Rules Formatting" for usage in numbered paragraphs and multi-part rules
- 1.4.3 Conforming Implementation
- 1.4.4 Conforming Program
- 1.4.5 Implementation-Defined Behavior
- 1.4.6 Undefined Behavior
  - Definition and consequences
  - Labeling convention [UB] or [Undefined Behavior]
  - Cross-reference to Annex B.2 catalog (format: [UB: B.2 #N])
  - Common causes (out-of-bounds, null deref, data races, region escape, use-after-move)
  - Audit requirement: all UB instances shall be listed in Annex B.2
- 1.4.7 Unspecified Behavior
- 1.4.8 Diagnosable Rules
  - Ill-formed programs (diagnostic required)
  - Ill-formed, no diagnostic required (IFNDR)
  - Diagnostic format is implementation-defined
- 1.4.9 Diagnostic Guidance
  - Relationship to Compiler Diagnostics Reference (companion, non-normative)
  - Diagnostic quality expectations
  - Implementation documentation requirements

#### **Section 1.5: Document Conventions**
**File**: `01-5_Document-Conventions.md`

- 1.5.1 Cross-References
  - Cross-reference format (§X.Y, Part X §Y.Z, Annex X §Y)
  - Cross-reference requirements (every dependency shall cite target section)
  - Bidirectional cross-referencing (key definitions list referencing sections)
  - Digital hyperlinks required in electronic versions
  - **Detailed formatting guidance**: See "Section-Level Content Organization" → "Documentation Elements" → "Cross-Reference Format" for complete notation conventions and bidirectional linking patterns
- 1.5.1a Cross-Reference Validation
  - All references shall resolve to existing sections before publication
  - Automated validation tooling required
  - Dead reference prohibition
  - Orphan detection (sections with zero inbound references flagged)
  - Reference resolution tables
- 1.5.2 Examples (Informative)
  - Example purpose and status (informative, do not impose normative requirements)
  - Example coverage requirements (non-trivial semantics should include examples; complex features must include examples)
  - Example formatting (fenced code blocks with ```cursive, validity markers for invalid examples)
  - Example quality (minimal yet complete, consistent naming per §1.5.2a)
  - **Detailed formatting guidance**: See "Section-Level Content Organization" → "Documentation Elements" → "Example Formatting" for complete requirements on placement, numbering, and structure
  - **Naming conventions**: See §1.5.2a for comprehensive naming standards and avoidance lists
- 1.5.2a Example Naming Conventions (Informative)
  - **Purpose**: Establish consistent, language-neutral naming patterns that avoid implying Rust-specific or non-Cursive types
  - **Naming Reference Table**:

| Category | Element | Standard Names | Example | Usage Context |
|----------|---------|----------------|---------|---------------|
| **Syntax Examples** | Variables | `x`, `y`, `z`, `w` | `let x = 5` | Grammar demonstrations only |
| | Type parameters | `T`, `U`, `V`, `W` | `f<T>(x: T)` | Generic syntax only |
| | Functions | `f`, `g`, `h` | `proc f(x: i32)` | Minimal syntax examples |
| | Metasyntactic | `foo`, `bar`, `baz` | `let foo = bar` | Pure syntax (avoid in semantics) |
| **Semantics Examples** | Simple values | `value`, `item`, `element` | `let value = 42` | General-purpose demonstrations |
| | Numeric values | `count`, `total`, `result` | `let count = items.len()` | Arithmetic/counting |
| | Boolean values | `found`, `is_valid`, `ready` | `let found = true` | Predicate results |
| | Collections | `items`, `elements`, `values` | `let items = [1, 2, 3]` | Arrays, slices, sequences |
| | Iterators | `iter`, `current`, `next` | `let iter = items.iter()` | Iterator patterns |
| | Text values | `text`, `message`, `name` | `let name = "Alice"` | String/text data |
| | Record types | `Container`, `Holder`, `Wrapper` | `record Container<T>` | Generic containers |
| | Record instances | `container`, `holder` | `let container = Container { ... }` | Container instances |
| | Enum types | `Status`, `State`, `Mode` | `enum Status` | State machines, variants |
| | Enum instances | `status`, `state`, `mode` | `let status = Status::Ready` | Enum values |
| | Functions | `process`, `compute`, `transform` | `proc process(items: [i32])` | Semantic operations |
| | Procedures | `update`, `validate`, `initialize` | `proc update(var state: State)` | State-changing operations |
| | Generic functions | `map`, `filter`, `fold` | `proc map<T, U>(...)` | Higher-order patterns |
| | Closures | `predicate`, `transform`, `action` | `let predicate = \|x\| x > 0` | Lambda/closure parameters |
| | Type aliases | `ValueType`, `ElementType` | `type ValueType = i32` | Generic type naming |
| **Domain Examples** | Domain-specific | `Point`, `Connection`, `User` | `record Point { x: f64, y: f64 }` | Only when demonstrating domain modeling |
| | Domain values | `point`, `connection`, `user` | `let point = Point { x: 0.0, y: 0.0 }` | Domain instances (use sparingly) |

  - **Decision Matrix**:
    - Use **syntax names** (x, y, T) when demonstrating pure grammar/syntax rules
    - Use **semantics names** (value, items, Container) for teaching language semantics
    - Use **domain names** (Point, User) ONLY when the example specifically demonstrates domain modeling
    - Use **metasyntactic names** (foo, bar) sparingly and only for abstract syntax patterns

  - **Avoidance List** (Critical):
    - **Rust types**: `Vec`, `String`, `Option`, `Box`, `Rc`, `Arc`, `HashMap`, `BTreeMap`, `RefCell`, `Cell`
    - **Rust patterns**: `Some`, `None`, `Ok`, `Err`, `unwrap`, `expect`
    - **C++ types**: `std::`, `vector`, `unique_ptr`, `shared_ptr`, `optional`, `variant`
    - **C# types**: `List<T>`, `Dictionary`, `IEnumerable`, `Nullable<T>`
    - **Java types**: `ArrayList`, `HashMap`, `Optional`, `List`
    - **Python patterns**: `dict`, `list` (as type names)

  - **Rationale**: Cursive is a distinct language with its own type system. Using other languages' type names in examples creates false associations and implies semantics that may not match Cursive's design. Generic, descriptive names keep examples focused on Cursive's actual semantics.

- 1.5.3 Notes (Informative)
  - **Detailed formatting guidance**: See "Section-Level Content Organization" → "Documentation Elements" → "Notes and Warnings" for complete requirements on note types, placement, and formatting
- 1.5.3a Visual Diagrams (Optional, Informative)
  - **Purpose**: Strategic use of visual diagrams to clarify complex relationships, state machines, and architectural patterns
  - **When to Use Diagrams**:
    - Complex permission/ownership relationships
    - State machine transitions
    - Type hierarchies and subtyping relationships
    - Memory layout and pointer relationships
    - Control flow for complex constructs
    - Do NOT use for simple syntax (prose is clearer)
  - **Diagram Format**:
    - **Primary format**: ASCII art (70-character width maximum)
    - **Rationale**: Version control friendly, accessible, renders in all contexts
    - **Optional enhancement**: SVG generation via mdbook-svgbob for HTML rendering
    - **Tools**: asciiflow.com for creation, mdbook-svgbob for rendering
  - **Accessibility Requirements** (Mandatory):
    - Every diagram MUST have a complete prose description in a numbered paragraph
    - Describe all elements, relationships, and semantics in text
    - The prose description must be sufficient to understand the concept without viewing the diagram
    - Diagrams supplement prose; they do not replace it
  - **Diagram Formatting**:
    - Place in fenced code blocks (no language tag for ASCII art)
    - Number diagrams per-section: `**Figure §4.2-1**: [Title]`
    - Follow with numbered paragraph describing the diagram
  - **Example Diagram** (showing how a figure would appear in §4.2):

```
**Figure §4.2-1**: Cursive Permission System

    ┌──────────┐
    │  unique  │ ──── Exclusive access (mutable)
    └─────┬────┘
          │
    ┌─────▼────┐
    │  shared  │ ──── Shared access (read-write under constraints)
    └─────┬────┘
          │
    ┌─────▼────┐
    │  const   │ ──── Constant access (immutable)
    └──────────┘
```

[1] This diagram illustrates the Cursive permission system. The three
permission levels are: `unique` (exclusive access allowing mutation),
`shared` (non-exclusive access with constraints), and `const` (immutable
read-only access). Arrows indicate permission subtyping relationships,
where more restrictive permissions can be used where less restrictive
permissions are expected.

- 1.5.4 Section Organization
  - **Detailed structural guidance**: See "Section-Level Content Organization" for complete templates, best practices, and progressive disclosure patterns
- 1.5.4a Grammar Presentation
  - Dual presentation: in-context (within semantic sections) and consolidated (Annex A)
  - In-context grammar requirements (sections introducing syntactic constructs shall include relevant grammar)
  - In-line grammar formatting (EBNF notation per §1.3.1, 2-5 productions typical)
  - Grammar synchronization (in-line must match Annex A; Annex A is authoritative)
  - **Detailed formatting guidance**: See "Section-Level Content Organization" → "Documentation Elements" → "Syntax Grammar Formatting" for EBNF notation details and formatting conventions
- 1.5.5 Forward References and Dependencies
  - Forward reference policy (minimize where possible; use when pedagogically necessary)
  - Dependency order (Part II → Part III → Part IV progressive build-up)
  - Cross-reference conventions (use §X.Y notation; link to specific numbered paragraphs when possible)
  - Documentation element references:
    - Example naming: §1.5.2a
    - Visual diagrams: §1.5.3a
    - Grammar presentation: §1.5.4a
    - Cross-reference validation: §1.5.1a
- 1.5.6 Reading Guide for Different Audiences

#### **Section 1.6: Versioning and Evolution**
**File**: `01-6_Versioning-Evolution.md`

- 1.6.1 Semantic Versioning
  - Version format `MAJOR.MINOR.PATCH`
  - This specification defines language version 1.0.0
  - Program manifests declare a minimum language version (`version = "1.0"`)
  - Compatibility rules (1.x remains backward compatible; major bumps can break)
  - Implementation requirements for advertising supported versions
- 1.6.2 Feature Stability
  - Stable features (no `[Experimental]`/`[Preview]`; semantics stable within the major line)
  - Experimental features (marked `[Experimental]`; require explicit opt-in)
  - Preview features preparing for the next major release (`[Preview: v2.0]`)
  - Stabilization process (feedback, formal text, implementation validation)
- 1.6.3 Deprecation Policy
  - Deprecation marking (`[Deprecated: vMAJOR.MINOR]`)
  - Deprecation timeline (warn in minor releases before removal in the next major)
  - Migration guidance and automation expectations
- 1.6.4 Relationship to ISO Standardization
  - ISO revisions align with major Cursive versions
  - Minor/patch releases can occur between ISO ballots
- 1.6.5 Implementation Guidance
  - Feature gates for experimental/preview capabilities (e.g., `#![feature(name)]`)
  - Language-version selection flags and manifest handling
  - Migration tooling for major-version upgrades

---

### **PART II: BASICS AND PROGRAM MODEL** *(Normative)*

#### **Section 2.0: Translation Phases**
**File**: `02-0_Translation-Phases.md`

- 2.0.1 Phase Overview
- 2.0.2 Source File Processing
- 2.0.3 Lexical Analysis
- 2.0.4 Syntactic Analysis
- 2.0.5 Semantic Analysis
- 2.0.6 Code Generation

#### **Section 2.1: Source Text Encoding**
**File**: `02-1_Source-Encoding.md`

- 2.1.1 Character Encoding (UTF-8)
- 2.1.2 Source File Structure
- 2.1.3 Line Endings
- 2.1.4 Byte Order Mark Handling

#### **Section 2.2: Lexical Elements**
**File**: `02-2_Lexical-Elements.md`

- 2.2.1 Input Elements
- 2.2.2 Whitespace
- 2.2.3 Comments
- 2.2.4 Identifiers
- 2.2.5 Keywords
- 2.2.6 Literals
- 2.2.7 Operators and Punctuation
- 2.2.8 Statement Termination
- 2.2.9 Token Formation

#### **Section 2.3: Syntax Notation**
**File**: `02-3_Syntax-Notation.md`

- 2.3.1 Grammar Organization
  - Complete grammar in Annex A (normative)
  - In-context grammar snippets throughout specification (informative illustrations)
  - For complete production rules, consult Annex A
  - EBNF notation defined in §1.3.1
  - Grammar presentation policy in §1.5.4a
- 2.3.2 Syntax Categories
- 2.3.3 Reference to Appendix A (Complete Grammar)

#### **Section 2.4: Program Structure**
**File**: `02-4_Program-Structure.md`

- 2.4.1 Compilation Units
- 2.4.2 Top-Level Constructs
- 2.4.3 Declaration Order
- 2.4.4 Program Entry Point

#### **Section 2.5: Scopes and Name Lookup**
**File**: `02-5_Scopes-Name-Lookup.md`

- 2.5.1 Scope Concept
- 2.5.2 Lexical Scoping
- 2.5.3 Scope Nesting
- 2.5.4 Name Binding
- 2.5.5 Name Lookup Algorithm
- 2.5.6 Qualified Name Lookup
- 2.5.7 Unqualified Name Lookup
- 2.5.8 Shadowing

#### **Section 2.6: Linkage and Program Units**
**File**: `02-6_Linkage-Program-Units.md`

- 2.6.1 Linkage Concept
- 2.6.2 External Linkage
- 2.6.3 Internal Linkage
- 2.6.4 No Linkage
- 2.6.5 One Definition Rule

#### **Section 2.7: Modules (Overview)**
**File**: `02-7_Modules-Overview.md`

- 2.7.1 Module Concept
- 2.7.2 Module Declaration
- 2.7.3 Import and Export
- 2.7.4 Module Visibility
- 2.7.5 Cross-reference to §7.10 for detailed rules

#### **Section 2.8: Memory and Objects (Overview)**
**File**: `02-8_Memory-Objects-Overview.md`

- 2.8.1 Object Model
- 2.8.2 Memory Locations
- 2.8.3 Object Lifetime (Overview)
- 2.8.4 Storage Duration (Overview)
- 2.8.5 Alignment
- 2.8.6 Cross-reference to Part IV for detailed memory model

#### **Section 2.9: Namespaces**
**File**: `02-9_Namespaces.md`

- 2.9.1 Namespace Categories
- 2.9.2 Type Namespace
- 2.9.3 Value Namespace
- 2.9.4 Module Namespace

---

### **PART III: TYPE SYSTEM FOUNDATIONS** *(Normative)*

**Binding System Overview:**

Cursive uses an orthogonal binding system with three independent axes:
- **Rebindability**: `let` (non-rebindable) vs `var` (rebindable)
- **Ownership**: `=` (responsible) vs `<-` (reference)
- **Permission**: `const` / `unique` / `shared`

This enables precise control through compositional declarations like `let x: const <- value` (non-rebindable reference with immutable access).

#### **Section 3.0: Type System Foundations**
**File**: `03-0_Type-Foundations.md`

- 3.0.1 Type System Overview
- 3.0.2 Type Formation Rules
- 3.0.3 Type Environments (Γ)
- 3.0.4 Type Equality and Equivalence
- 3.0.5 Subtyping Relation
- 3.0.6 Type Safety Properties

#### **Section 3.1: Subtyping and Equivalence**
**File**: `03-1_Subtyping-Equivalence.md`

- 3.1.1 Type Equivalence Rules
- 3.1.2 Subtyping Rules
- 3.1.3 Variance
- 3.1.4 Type Compatibility

#### **Section 3.2: Primitive Types**
**File**: `03-2_Primitive-Types.md`

> **Note**: This section uses 4-level nesting (§3.2.X.Y) due to covering 6 distinct type categories, each requiring detailed specification. See "Multi-Level Nesting Guidelines" below for rationale.

- 3.2.1 Primitive Types Overview
- 3.2.2 Integer Types
- 3.2.3 Floating-Point Types
- 3.2.4 Boolean Type
- 3.2.5 Character Type
- 3.2.6 Unit Type
- 3.2.7 Never Type
- 3.2.8 Integration

#### **Section 3.3: Composite Types**
**File**: `03-3_Composite-Types.md`

- 3.3.1 Product Types (Tuples, Records)
- 3.3.2 Sum Types (Enums)
- 3.3.3 Union Types

#### **Section 3.4: Collection Types**
**File**: `03-4_Collection-Types.md`

- 3.4.1 Arrays
- 3.4.2 Slices
- 3.4.3 Strings
- 3.4.4 Range Types

#### **Section 3.5: Function Types**
**File**: `03-5_Function-Types.md`

- 3.5.1 Function Type Syntax
- 3.5.2 Function Type Formation
- 3.5.3 Function Subtyping
- 3.5.4 Closure Types

#### **Section 3.6: Pointer Types**
**File**: `03-6_Pointer-Types.md`

- 3.6.1 Safe Pointers
- 3.6.2 Raw Pointers

#### **Section 3.7: Type Aliases**
**File**: `03-7_Type-Aliases.md`

- 3.7.1 Type Alias Declaration
- 3.7.2 Transparent vs Opaque Aliases
- 3.7.3 Generic Type Aliases

#### **Section 3.8: Type Introspection**
**File**: `03-8_Type-Introspection.md`

- 3.8.1 typeof operator
- 3.8.2 Type predicates
- 3.8.3 Compile-time type queries

> **Note**: Predicates and marker predicates (former §3.7) have been moved to Part VIII (Predicates and Type Constraints). Generics and type constructors (former §3.8) have been moved to Part IX (Generics and Parametric Polymorphism). Type bounds and constraints (former §3.9) have been integrated into Part VIII §8.3.

---

### **PART IV: MEMORY MODEL AND PERMISSIONS** *(Normative)*

#### **Section 4.0: Memory Model Overview**
**File**: `04-0_Memory-Model-Overview.md`

- 4.0.1 Memory Safety Goals
- 4.0.2 Permission-Based Memory Management
- 4.0.3 No Garbage Collection
- 4.0.4 No Borrow Checker
- 4.0.5 Relationship to Part II §2.8

#### **Section 4.1: Objects and Memory Locations**
**File**: `04-1_Objects-Memory.md`

- 4.1.1 Object Definition
- 4.1.2 Memory Location
- 4.1.3 Object Creation
- 4.1.4 Object Destruction
- 4.1.5 Object Lifetime

#### **Section 4.2: Permission System**
**File**: `04-2_Permission-System.md`

- 4.2.1 Permission Lattice (const / shared / unique)
- 4.2.2 Permission Syntax
- 4.2.3 Permission Declarations
- 4.2.4 Permission Checking Rules
- 4.2.5 Permission Propagation
- 4.2.6 Permission Inference
- 4.2.7 Permission Upgrades and Downgrades

#### **Section 4.3: Move Semantics**
**File**: `04-3_Move-Semantics.md`

- 4.3.1 Move Operations
- 4.3.2 Move Checking
- 4.3.3 Copy Types
- 4.3.4 Clone Operations
- 4.3.5 Dropped Values

#### **Section 4.4: Regions and Lifetimes**
**File**: `04-4_Regions-Lifetimes.md`

- 4.4.1 Region Concept
- 4.4.2 Region Syntax
- 4.4.3 Region Allocation
- 4.4.4 Region Escape Analysis
- 4.4.5 Region Stack (Δ)
- 4.4.6 Region Cleanup

#### **Section 4.5: Storage Duration**
**File**: `04-5_Storage-Duration.md`

- 4.5.1 Static Storage
- 4.5.2 Function Storage
- 4.5.3 Region Storage
- 4.5.4 Heap Storage

#### **Section 4.6: Memory Layout**
**File**: `04-6_Memory-Layout.md`

- 4.6.1 Type Sizes and Alignment
- 4.6.2 Record Layout
- 4.6.3 Enum Layout
- 4.6.4 Layout Attributes
- 4.6.5 Padding and Packing

#### **Section 4.7: Aliasing and Uniqueness**
**File**: `04-7_Aliasing-Uniqueness.md`

- 4.7.1 Aliasing Rules
- 4.7.2 Aliasing Violations
- 4.7.3 Uniqueness Guarantees

#### **Section 4.8: Unsafe Operations**
**File**: `04-8_Unsafe-Operations.md`

- 4.8.1 Unsafe Block Semantics
- 4.8.2 Safety Obligations
- 4.8.3 Unsafe Capabilities
- 4.8.4 When Unsafe is Required
- 4.8.5 Cross-reference to §15.2 for FFI-specific unsafe usage

---

### **PART V: EXPRESSIONS AND CONVERSIONS** *(Normative)*

#### **Section 5.0: Expression Fundamentals**
**File**: `05-0_Expression-Fundamentals.md`

- 5.0.1 Expression Evaluation Model
- 5.0.2 Value Categories (lvalue, rvalue, place) *Note: See §4.2 for interaction with permission system*
- 5.0.3 Expression Classification
- 5.0.4 Type and Effect of Expressions

#### **Section 5.1: Primary Expressions**
**File**: `05-1_Primary-Expressions.md`

- 5.1.1 Literal Expressions
- 5.1.2 Identifier Expressions
- 5.1.3 Parenthesized Expressions
- 5.1.4 Tuple Expressions
- 5.1.5 Array Literals
- 5.1.6 Block Expressions

#### **Section 5.2: Postfix Expressions**
**File**: `05-2_Postfix-Expressions.md`

- 5.2.1 Field Access
- 5.2.2 Index Expressions
- 5.2.3 Function Calls
- 5.2.4 Procedure Calls
- 5.2.5 Type Ascription

#### **Section 5.3: Unary Expressions**
**File**: `05-3_Unary-Expressions.md`

- 5.3.1 Unary Operators
- 5.3.2 Dereferencing
- 5.3.3 Address-of
- 5.3.4 Negation
- 5.3.5 Logical NOT

#### **Section 5.4: Binary Expressions**
**File**: `05-4_Binary-Expressions.md`

- 5.4.1 Arithmetic Operators
- 5.4.2 Comparison Operators
- 5.4.3 Logical Operators
- 5.4.4 Bitwise Operators
- 5.4.5 Range Operators

#### **Section 5.5: Operator Precedence and Associativity**
**File**: `05-5_Operator-Precedence.md`

- 5.5.1 Precedence Table
- 5.5.2 Associativity Rules
- 5.5.3 Parenthesization

#### **Section 5.6: Structured Expressions**
**File**: `05-6_Structured-Expressions.md`

- 5.6.1 Record Construction
- 5.6.2 Enum Construction
- 5.6.3 Closure Expressions
- 5.6.4 If Expressions
- 5.6.5 Match Expressions

#### **Section 5.7: Other Expressions**
**File**: `05-7_Other-Expressions.md`

- 5.7.1 Pipeline Expressions
- 5.7.2 Range Expressions
- 5.7.3 Unsafe Blocks

#### **Section 5.8: Type Conversions and Coercions**
**File**: `05-8_Conversions-Coercions.md`

- 5.8.1 Implicit Conversions
- 5.8.2 Explicit Conversions (as operator)
- 5.8.3 Coercion Sites
- 5.8.4 Numeric Conversions
- 5.8.5 Pointer Conversions
- 5.8.6 Reference Conversions
- 5.8.7 Unsizing Coercions
- 5.8.8 Well-formedness of Conversions

#### **Section 5.9: Constant Expressions**
**File**: `05-9_Constant-Expressions.md`

- 5.9.1 Constant Expression Definition
- 5.9.2 Constant Evaluation Context
- 5.9.3 Constant Functions
- 5.9.4 Compile-time Evaluation Rules
- 5.9.5 Constant Propagation

#### **Section 5.10: Expression Typing Rules**
**File**: `05-10_Expression-Typing.md`

- 5.10.1 Typing Judgments
- 5.10.2 Type Checking Algorithm
- 5.10.3 Type Inference Integration
- 5.10.4 Ill-formed Expressions

---

### **PART VI: STATEMENTS AND CONTROL FLOW** *(Normative)*

#### **Section 6.0: Statement Fundamentals**
**File**: `06-0_Statement-Fundamentals.md`

- 6.0.1 Statement Classification
- 6.0.2 Statement Execution
- 6.0.3 Statement Termination

#### **Section 6.1: Simple Statements**
**File**: `06-1_Simple-Statements.md`

- 6.1.1 Expression Statements
- 6.1.2 Let Statements
- 6.1.3 Assignment Statements
- 6.1.4 Return Statements
- 6.1.5 Break and Continue
- 6.1.6 Defer Statements

#### **Section 6.2: Control Flow Statements**
**File**: `06-2_Control-Flow-Statements.md`

- 6.2.1 If Statements
- 6.2.2 If-Let Statements
- 6.2.3 While Loops
- 6.2.4 For Loops
- 6.2.5 Loop Labels
- 6.2.6 Infinite Loops

#### **Section 6.3: Pattern Matching**
**File**: `06-3_Pattern-Matching.md`

- 6.3.1 Pattern Syntax
- 6.3.2 Pattern Semantics
- 6.3.3 Irrefutable Patterns
- 6.3.4 Refutable Patterns
- 6.3.5 Pattern Exhaustiveness
- 6.3.6 Pattern Guards
- 6.3.7 Or Patterns

#### **Section 6.4: Evaluation Order and Semantics**
**File**: `06-4_Evaluation-Semantics.md`

- 6.4.1 Evaluation Order
- 6.4.2 Sequencing
- 6.4.3 Short-circuit Evaluation
- 6.4.4 Divergence
- 6.4.5 Determinism Guarantees

---

### **PART VII: DECLARATIONS, SCOPE, AND MODULES** *(Normative)*

#### **Section 7.0: Declaration System Overview**
**File**: `07-0_Declaration-Overview.md`

- 7.0.1 Declaration Categories
- 7.0.2 Declaration Syntax
- 7.0.3 Visibility and Privacy

#### **Section 7.1: Variable Declarations**
**File**: `07-1_Variable-Declarations.md`

- 7.1.1 Let Declarations
- 7.1.2 Var Declarations
- 7.1.3 Shadow Declarations
- 7.1.4 Destructuring Declarations
- 7.1.5 Type Annotations

#### **Section 7.2: Function and Procedure Declarations**
**File**: `07-2_Function-Procedure-Declarations.md`

- 7.2.1 Function Declarations
- 7.2.2 Procedure Declarations
- 7.2.3 Parameters
- 7.2.4 Return Types
- 7.2.5 Generic Functions
- 7.2.6 Local Functions (Prohibition)

#### **Section 7.3: Type Declarations**
**File**: `07-3_Type-Declarations.md`

- 7.3.1 Record Declarations
- 7.3.2 Enum Declarations
- 7.3.3 Type Alias Declarations
- 7.3.4 Newtype Declarations

#### **Section 7.4: Predicate Declarations**
**File**: `07-4_Predicate-Declarations.md`

- 7.4.1 Predicate Syntax
- 7.4.2 Predicate Items
- 7.4.3 Associated Types
- 7.4.4 Associated Constants

#### **Section 7.5: Contract Declarations**
**File**: `07-5_Contract-Declarations.md`

- 7.5.1 Contract Syntax
- 7.5.2 Contract Procedures
- 7.5.3 Contract Associated Types

#### **Section 7.6: Effect Declarations**
**File**: `07-6_Effect-Declarations.md`

- 7.6.1 Effect Syntax
- 7.6.2 Effect Operations
- 7.6.3 Marker Effects

#### **Section 7.7: Modal Declarations**
**File**: `07-7_Modal-Declarations.md`

- 7.7.1 Modal Type Syntax
- 7.7.2 State Declarations
- 7.7.3 Transition Declarations

#### **Section 7.8: Scope Rules (Detailed)**
**File**: `07-8_Scope-Rules.md`

- 7.8.1 Scope Regions
- 7.8.2 Block Scopes
- 7.8.3 Function Scopes
- 7.8.4 Module Scopes
- 7.8.5 Shadowing Rules (Detailed)

#### **Section 7.9: Name Resolution (Detailed)**
**File**: `07-9_Name-Resolution.md`

- 7.9.1 Resolution Algorithm
- 7.9.2 Ambiguity Resolution
- 7.9.3 Resolution Precedence

#### **Section 7.10: Modules and Imports (Detailed)**
**File**: `07-10_Modules-Imports.md`

- 7.10.1 Module Structure
- 7.10.2 Module Paths
- 7.10.3 Import Declarations
- 7.10.4 Export Control
- 7.10.5 Import Resolution
- 7.10.6 Import Conflicts
- 7.10.7 Circular Dependencies

#### **Section 7.11: Visibility**
**File**: `07-11_Visibility.md`

- 7.11.1 Visibility Levels
- 7.11.2 Visibility Rules
- 7.11.3 Visibility Checking
- 7.11.4 Privacy Violations

#### **Section 7.12: Predeclared Identifiers**
**File**: `07-12_Predeclared-Identifiers.md`

- 7.12.1 Predeclared Types
- 7.12.2 Predeclared Constants
- 7.12.3 Predeclared Functions
- 7.12.4 Shadowing Prohibition

---

### **PART VIII: PREDICATES AND TYPE CONSTRAINTS** *(Normative)*

#### **Section 8.0: Predicate System Overview**
**File**: `08-0_Predicate-Overview.md`

- 8.0.1 Predicate Fundamentals
- 8.0.2 Predicate System Goals
- 8.0.3 Relationship to Contracts

#### **Section 8.1: Predicate Definitions**
**File**: `08-1_Predicate-Definitions.md`

- 8.1.1 Predicate Declaration
- 8.1.2 Predicate Items
- 8.1.3 Default Implementations
- 8.1.4 Predicate Inheritance

#### **Section 8.2: Predicate Implementation**
**File**: `08-2_Predicate-Implementation.md`

- 8.2.1 Implementation Blocks
- 8.2.2 Implementation Requirements
- 8.2.3 Implementation Completeness
- 8.2.4 Blanket Implementations

#### **Section 8.3: Type Bounds and Constraints**
**File**: `08-3_Type-Bounds-Constraints.md`

- 8.3.1 Bound Syntax
- 8.3.2 Predicate Bounds
- 8.3.3 Where Clauses
- 8.3.4 Type Constraints
- 8.3.5 Constraint Solving
- 8.3.6 Higher-ranked Predicate Bounds
- 8.3.7 Associated Type Constraints

> **Note**: This section consolidates type bounds and constraints previously in Part III §3.9, integrating them with the predicate system.

#### **Section 8.4: Predicate Coherence**
**File**: `08-4_Predicate-Coherence.md`

- 8.4.1 Orphan Rule
- 8.4.2 Overlap Rules
- 8.4.3 Fundamental Types
- 8.4.4 Coherence Violations

#### **Section 8.5: Static Dispatch**
**File**: `08-5_Static-Dispatch.md`

- 8.5.1 Monomorphization
- 8.5.2 Type Resolution
- 8.5.3 Performance Characteristics

#### **Section 8.6: Marker Predicates**
**File**: `08-6_Marker-Predicates.md`

- 8.6.1 Marker Predicate Definition
- 8.6.2 Copy, Clone
- 8.6.3 Send, Sync (if applicable)
- 8.6.4 Auto Predicates

#### **Section 8.7: Predicate Objects**
**File**: `08-7_Predicate-Objects.md`

- 8.7.1 Dynamic Dispatch
- 8.7.2 Object Safety
- 8.7.3 vtable Representation
- 8.7.4 Limitations

---

### **PART IX: GENERICS AND PARAMETRIC POLYMORPHISM** *(Normative)*

#### **Section 9.0: Generics Overview**
**File**: `09-0_Generics-Overview.md`

- 9.0.1 Parametric Polymorphism Goals
- 9.0.2 Type Parameters vs Value Parameters
- 9.0.3 Monomorphization Model
- 9.0.4 Relationship to Predicates

> **Note**: This part consolidates generic type system content previously in Part III §3.8, providing comprehensive coverage of parametric polymorphism.

#### **Section 9.1: Type Parameters**
**File**: `09-1_Type-Parameters.md`

- 9.1.1 Type Parameter Declaration
- 9.1.2 Type Parameter Scope
- 9.1.3 Type Parameter Bounds
- 9.1.4 Default Type Parameters
- 9.1.5 Lifetime Parameters

#### **Section 9.2: Generic Functions and Procedures**
**File**: `09-2_Generic-Functions.md`

- 9.2.1 Generic Function Declaration
- 9.2.2 Generic Procedure Declaration
- 9.2.3 Type Parameter Inference
- 9.2.4 Turbofish Syntax
- 9.2.5 Generic Function Constraints

#### **Section 9.3: Generic Types**
**File**: `09-3_Generic-Types.md`

- 9.3.1 Generic Record Declarations
- 9.3.2 Generic Enum Declarations
- 9.3.3 Generic Type Aliases
- 9.3.4 Phantom Type Parameters
- 9.3.5 Associated Types

#### **Section 9.4: Generic Implementations**
**File**: `09-4_Generic-Implementations.md`

- 9.4.1 Generic Implementation Blocks
- 9.4.2 Implementation Constraints
- 9.4.3 Blanket Implementations
- 9.4.4 Specialization (if supported)

#### **Section 9.5: Type Constructors**
**File**: `09-5_Type-Constructors.md`

- 9.5.1 Type Constructor Kinds
- 9.5.2 Higher-kinded Types
- 9.5.3 Type-level Functions
- 9.5.4 Partial Application

#### **Section 9.6: Variance**
**File**: `09-6_Variance.md`

- 9.6.1 Covariance
- 9.6.2 Contravariance
- 9.6.3 Invariance
- 9.6.4 Variance Inference
- 9.6.5 Variance and Subtyping

#### **Section 9.7: Generic Constraints and Resolution**
**File**: `09-7_Generic-Constraints.md`

- 9.7.1 Constraint Specification
- 9.7.2 Constraint Checking
- 9.7.3 Resolution Algorithm
- 9.7.4 Ambiguity Resolution

---

### **PART X: GRANT SYSTEM** *(Normative)*

#### **Section 10.0: Grant System Overview**
**File**: `10_Grant-System/10-0_Grant-System-Overview.md`

- 10.0.1 Grant System Purpose
- 10.0.2 Grant Tracking Goals
- 10.0.3 Grant System Model
- 10.0.4 Terminology
- 10.0.5 Relationship to Other Language Features
- 10.0.6 Conformance Requirements
- 10.0.7 Organization of Part X

#### **Section 10.1: Grant Clauses**
**File**: `10_Grant-System/10-1_Grant-Clauses.md`

- 10.1.1 Grant Clause Syntax
- 10.1.2 Grant Sets
- 10.1.3 Grant Paths
- 10.1.4 Procedure Grant Requirements
- 10.1.5 Grant Clause Composition
- 10.1.6 Deprecated Syntax
- 10.1.7 Grammar Reference
- 10.1.8 Well-Formedness Constraints
- 10.1.9 Interaction with Functions
- 10.1.10 Conformance Requirements

#### **Section 10.2: Grant Tracking and Propagation**
**File**: `10_Grant-System/10-2_Grant-Tracking.md`

- 10.2.1 Grant Propagation Fundamentals
- 10.2.2 Transitive Grant Propagation
- 10.2.3 Grant Verification Algorithm
- 10.2.4 Special Cases
- 10.2.5 Compile-Time Erasure
- 10.2.6 Grant Checking for Operations
- 10.2.7 Grant Violations and Diagnostics
- 10.2.8 Grant Inference
- 10.2.9 Conformance Requirements

#### **Section 10.3: Grant Polymorphism**
**File**: `10_Grant-System/10-3_Grant-Polymorphism.md`

- 10.3.1 Grant Parameter Fundamentals
- 10.3.2 Grant Parameter Bounds
- 10.3.3 Grant Parameter Instantiation
- 10.3.4 Grant Parameter Composition
- 10.3.5 Grant Polymorphic Patterns
- 10.3.6 Grant Parameter Verification
- 10.3.7 Advanced Grant Polymorphism
- 10.3.8 Limitations and Restrictions
- 10.3.9 Conformance Requirements

#### **Section 10.4: Built-In Grants**
**File**: `10_Grant-System/10-4_Built-In-Grants.md`

- 10.4.1 Built-In Grant Catalog
- 10.4.2 Allocation Grants (`alloc`)
- 10.4.3 File System Grants (`fs`)
- 10.4.4 Network Grants (`net`)
- 10.4.5 Time Grants (`time`)
- 10.4.6 Thread Grants (`thread`)
- 10.4.7 FFI Grants (`ffi`)
- 10.4.8 Unsafe Grants (`unsafe`)
- 10.4.9 Panic Grant (`panic`)
- 10.4.10 Grant Hierarchy Summary
- 10.4.11 Grant Subsumption
- 10.4.12 Conformance Requirements

#### **Section 10.5: User-Defined Grants**
**File**: `10_Grant-System/10-5_User-Defined-Grants.md`

- 10.5.1 Grant Declaration Fundamentals
- 10.5.2 Grant Path Resolution
- 10.5.3 Visibility and Access Control
- 10.5.4 Grant Usage and Propagation
- 10.5.5 Wildcard Grants
- 10.5.6 Grant Polymorphism with User Grants
- 10.5.7 Well-Formedness Constraints
- 10.5.8 Verification Algorithm
- 10.5.9 Compile-Time Erasure
- 10.5.10 Interaction with Built-In Grants
- 10.5.11 Grammar Reference
- 10.5.12 Conformance Requirements

#### **Section 10.6: Grant Integration**
**File**: `10_Grant-System/10-6_Grant-Integration.md`

- 10.6.1 Integration Overview
- 10.6.2 Grants and Contracts
- 10.6.3 Grants and Permissions
- 10.6.4 Grants and Modal Types
- 10.6.5 Grants and Unsafe Code
- 10.6.6 Grants and Functions vs Procedures
- 10.6.7 Grants and Closures
- 10.6.8 Grants and Generics
- 10.6.9 Grants and Predicates
- 10.6.10 Grants and Attributes
- 10.6.11 Grants and Modules
- 10.6.12 Grants and Metaprogramming
- 10.6.13 Grant Composition Patterns
- 10.6.14 Conformance Requirements

---

### **PART XI: CONTRACT SYSTEM** *(Normative)*

#### **Section 11.0: Contract System Overview**
**File**: `11-0_Contract-Overview.md`

- 11.0.1 Design by Contract Philosophy
- 11.0.2 Contract Components
- 11.0.3 Static and Dynamic Checking

#### **Section 11.1: Behavioral Contracts**
**File**: `11-1_Behavioral-Contracts.md`

- 11.1.1 Contract Declaration
- 11.1.2 Contract Procedures
- 11.1.3 Contract Associated Types
- 11.1.4 Contract Implementation
- 11.1.5 Contract Extension
- 11.1.6 Contract Coherence
- 11.1.7 Contract vs Predicate

#### **Section 11.2: Preconditions**
**File**: `11-2_Preconditions.md`

- 11.2.1 Precondition Syntax (must)
- 11.2.2 Precondition Semantics
- 11.2.3 Precondition Checking
- 11.2.4 Precondition Predicates
- 11.2.5 Precondition Weakening
- 11.2.6 Precondition Composition

#### **Section 11.3: Postconditions**
**File**: `11-3_Postconditions.md`

- 11.3.1 Postcondition Syntax (will)
- 11.3.2 Postcondition Semantics
- 11.3.3 Postcondition Checking
- 11.3.4 Postcondition Predicates
- 11.3.5 Postcondition Strengthening
- 11.3.6 @old Capture Semantics

#### **Section 11.4: Type Invariants**
**File**: `11-4_Type-Invariants.md`

- 11.4.1 Type Invariant Syntax (where)
- 11.4.2 Type Invariant Semantics
- 11.4.3 Invariant Checking
- 11.4.4 Invariant Predicates
- 11.4.5 Invariant Preservation
- 11.4.6 Invariant Performance

#### **Section 11.5: Verification Modes**
**File**: `11-5_Verification-Modes.md`

- 11.5.1 Verification Attributes
- 11.5.2 Static Verification
- 11.5.3 Runtime Verification
- 11.5.4 No Verification
- 11.5.5 Verification Mode Inheritance

#### **Section 11.6: Contract Clause Composition**
**File**: `11-6_Clause-Composition.md`

- 11.6.1 Effect Propagation with Contracts
- 11.6.2 Precondition Composition
- 11.6.3 Postcondition Composition
- 11.6.4 Clause Compatibility

#### **Section 11.7: Contract Integration**
**File**: `11-7_Contract-Integration.md`

- 11.7.1 Contracts and Permissions
- 11.7.2 Contracts and Effects
- 11.7.3 Contracts and Modals
- 11.7.4 Contracts and Unsafe Code

---

### **PART XII: TYPE-LEVEL ADVANCED FEATURES** *(Normative)*

> **Note**: This part covers advanced type-level features that build upon and integrate multiple concepts from Parts III-X. These features are grouped here because they require comprehensive understanding of the type system, permissions, effects, and contracts.

#### **Section 12.0: Advanced Features Overview**
**File**: `12-0_Advanced-Features-Overview.md`

- 12.0.1 Modal Types (state-dependent type systems)
- 12.0.2 Contract Witnesses (runtime evidence of contract satisfaction)
- 12.0.3 Type Inference and Holes (automated type derivation)

#### **Section 12.1: Modal Types and State Machines**
**File**: `12-1_Modal-Types.md`

- 12.1.1 Modal Type Declaration
- 12.1.2 Modal States
- 12.1.3 State Transitions
- 12.1.4 Modal Type Instantiation
- 12.1.5 State-Dependent Operations
- 12.1.6 Modal Patterns
- 12.1.7 Modal State Inference
- 12.1.8 Modal Type Checking
- 12.1.9 Modal Integration

#### **Section 12.2: Contract Witnesses**
**File**: `12-2_Contract-Witnesses.md`

- 12.2.1 Witness Concept
- 12.2.2 Witness Construction
- 12.2.3 Witness Types
- 12.2.4 Witness Operations
- 12.2.5 Witness Memory Management
- 12.2.6 Effect Polymorphism in Witnesses

#### **Section 12.3: Type Inference and Holes**
**File**: `12-3_Type-Inference-Holes.md`

- 12.3.1 Type Inference Overview
- 12.3.2 Hole Syntax
- 12.3.3 Constraint Generation
- 12.3.4 Constraint Solving
- 12.3.5 Type Hole Resolution
- 12.3.6 Permission Hole Resolution
- 12.3.7 Modal State Hole Resolution
- 12.3.8 Effect Hole Resolution
- 12.3.9 Inference Limitations
- 12.3.10 Elaboration

---

### **PART XIII: METAPROGRAMMING** *(Normative)*

#### **Section 13.0: Metaprogramming Overview**
**File**: `13-0_Metaprogramming-Overview.md`

- 13.0.1 Compile-Time Computation
- 13.0.2 Code Generation
- 13.0.3 Reflection Capabilities

#### **Section 13.1: Attributes**
**File**: `13-1_Attributes.md`

- 13.1.1 Attribute Syntax
- 13.1.2 Attribute Placement
- 13.1.3 Core Attributes (repr, verify, inline, etc.)
- 13.1.4 User-Defined Attributes
- 13.1.5 Attribute Semantics

#### **Section 13.2: Compile-Time Evaluation**
**File**: `13-2_Compile-Time-Evaluation.md`

- 13.2.1 Const Expressions (detailed)
- 13.2.2 Const Functions (detailed)
- 13.2.3 Const Context
- 13.2.4 Comptime Blocks
- 13.2.5 Const Propagation

#### **Section 13.3: Code Generation**
**File**: `13-3_Code-Generation.md`

- 13.3.1 Macros
- 13.3.2 Derive Macros
- 13.3.3 Procedural Macros
- 13.3.4 Template Expansion

#### **Section 13.4: Reflection**
**File**: `13-4_Reflection.md`

- 13.4.1 Type Reflection
- 13.4.2 Compile-Time Type Queries
- 13.4.3 Runtime Type Information
- 13.4.4 Reflection Limitations

---

### **PART XIV: CONCURRENCY AND MEMORY ORDERING** *(Normative)*

> **Note**: The memory model (§13.2) and atomic operations (§13.3) are mandatory for all conforming implementations. Thread and task primitives (§13.1, §13.4) may be library-provided, but the memory model must be normatively specified to define data race semantics and memory ordering guarantees.

#### **Section 14.0: Concurrency Model Overview**
**File**: `14-0_Concurrency-Overview.md`

- 14.0.1 Concurrency Concepts
- 14.0.2 Thread Model (may be library-provided)
- 14.0.3 Memory Model Goals

#### **Section 14.1: Threads and Tasks**
**File**: `14-1_Threads-Tasks.md`

- 14.1.1 Thread Creation
- 14.1.2 Thread Lifetime
- 14.1.3 Task Concept

#### **Section 14.2: Memory Model**
**File**: `14-2_Memory-Model.md`

- 14.2.1 Happens-Before Relation
- 14.2.2 Synchronizes-With Relation
- 14.2.3 Sequential Consistency
- 14.2.4 Data Races
- 14.2.5 Race Freedom

#### **Section 14.3: Atomic Operations**
**File**: `14-3_Atomic-Operations.md`

- 14.3.1 Atomic Types
- 14.3.2 Memory Ordering
- 14.3.3 Acquire-Release Semantics
- 14.3.4 Sequentially Consistent Ordering
- 14.3.5 Relaxed Ordering

#### **Section 14.4: Synchronization Primitives**
**File**: `14-4_Synchronization.md`

- 14.4.1 Mutexes
- 14.4.2 Condition Variables
- 14.4.3 Barriers
- 14.4.4 Channels (if applicable)

---

### **PART XV: INTEROPERABILITY AND UNSAFE** *(Normative)*

#### **Section 15.0: Interoperability Overview**
**File**: `15-0_Interop-Overview.md`

- 15.0.1 Foreign Function Interface
- 15.0.2 C Compatibility Goals
- 15.0.3 Safety Boundaries

#### **Section 15.1: Foreign Function Interface**
**File**: `15-1_FFI.md`

- 15.1.1 External Declarations
- 15.1.2 Calling Conventions
- 15.1.3 FFI Types
- 15.1.4 FFI Safety
- 15.1.5 Name Mangling

#### **Section 15.2: Unsafe Operations in FFI Context**
**File**: `15-2_Unsafe-FFI.md`

> **Note**: General unsafe block semantics are specified in §4.8. This section covers FFI-specific unsafe operations and obligations.

- 15.2.1 Unsafe Blocks in FFI (see §4.8 for general semantics)
- 15.2.2 Unsafe External Functions
- 15.2.3 FFI Safety Obligations
- 15.2.4 Raw Pointers from External Code
- 15.2.5 Memory Safety Across FFI Boundaries

#### **Section 15.3: C Compatibility**
**File**: `15-3_C-Compatibility.md`

- 15.3.1 C Type Representations
- 15.3.2 C String Handling
- 15.3.3 C Function Pointers
- 15.3.4 C Variadic Functions
- 15.3.5 C Macros and Preprocessing

#### **Section 15.4: Platform-Specific Features**
**File**: `15-4_Platform-Specific.md`

- 15.4.1 Target Architecture
- 15.4.2 Operating System Interface
- 15.4.3 Inline Assembly
- 15.4.4 SIMD and Intrinsics

#### **Section 15.5: Linkage and Symbol Visibility**
**File**: `15-5_Linkage-Symbols.md`

- 15.5.1 Linkage Semantics
- 15.5.2 Symbol Visibility
- 15.5.3 Name Mangling Rules
- 15.5.4 Static and Dynamic Linking

#### **Section 15.6: ABI Considerations**
**File**: `15-6_ABI-Considerations.md`

- 15.6.1 ABI Stability (Implementation-Defined)
- 15.6.2 Platform Calling Conventions
- 15.6.3 Data Layout Guarantees
- 15.6.4 ABI Versioning

---

## ANNEXES

### **ANNEX A: COMPLETE GRAMMAR** *(Normative)*

#### **Section A.0: Grammar Organization**
**File**: `Annex-A-0_Grammar-Organization.md`

- A.0.1 Notation Recap
- A.0.2 Grammar Structure
- A.0.3 Cross-reference to Normative Text

#### **Section A.1: Lexical Grammar**
**File**: `Annex-A-1_Lexical-Grammar.md`

- A.1.1 Input Elements
- A.1.2 Whitespace and Comments
- A.1.3 Identifiers
- A.1.4 Keywords
- A.1.5 Literals
- A.1.6 Operators
- A.1.7 Punctuation

#### **Section A.2: Type Grammar**
**File**: `Annex-A-2_Type-Grammar.md`

- A.2.1 Type Expressions
- A.2.2 Primitive Types
- A.2.3 Composite Types
- A.2.4 Function Types
- A.2.5 Generic Types
- A.2.6 Permission-Wrapped Types

#### **Section A.3: Expression Grammar**
**File**: `Annex-A-3_Expression-Grammar.md`

- A.3.1 Literal Expressions
- A.3.2 Identifier Expressions
- A.3.3 Operator Expressions
- A.3.4 Call Expressions
- A.3.5 Field and Index Expressions
- A.3.6 Block Expressions
- A.3.7 Control Flow Expressions
- A.3.8 Closure Expressions
- A.3.9 Compound Expressions

#### **Section A.4: Statement Grammar**
**File**: `Annex-A-4_Statement-Grammar.md`

- A.4.1 Expression Statements
- A.4.2 Declaration Statements
- A.4.3 Assignment Statements
- A.4.4 Control Flow Statements

#### **Section A.5: Pattern Grammar**
**File**: `Annex-A-5_Pattern-Grammar.md`

- A.5.1 Pattern Forms
- A.5.2 Literal Patterns
- A.5.3 Variable Patterns
- A.5.4 Wildcard Patterns
- A.5.5 Tuple/Record/Enum Patterns
- A.5.6 Or Patterns
- A.5.7 Range Patterns

#### **Section A.6: Declaration Grammar**
**File**: `Annex-A-6_Declaration-Grammar.md`

- A.6.1 Variable Declarations
- A.6.2 Function Declarations
- A.6.3 Type Declarations
- A.6.4 Predicate Declarations
- A.6.5 Contract Declarations
- A.6.6 Effect Declarations
- A.6.7 Modal Declarations

#### **Section A.7: Contract Grammar**
**File**: `Annex-A-7_Contract-Grammar.md`

- A.7.1 Contract Clauses
- A.7.2 Uses Clauses
- A.7.3 Must Clauses (Preconditions)
- A.7.4 Will Clauses (Postconditions)
- A.7.5 Where Clauses (Invariants)

#### **Section A.8: Assertion Grammar**
**File**: `Annex-A-8_Assertion-Grammar.md`

- A.8.1 Predicate Expressions
- A.8.2 Quantifiers
- A.8.3 Old Expressions
- A.8.4 Result References

#### **Section A.9: Effect Grammar**
**File**: `Annex-A-9_Effect-Grammar.md`

- A.9.1 Effect Paths
- A.9.2 Effect Sets
- A.9.3 Effect Parameters

#### **Section A.10: Attribute Grammar**
**File**: `Annex-A-10_Attribute-Grammar.md`

- A.10.1 Attribute Syntax
- A.10.2 Attribute Arguments

#### **Section A.11: Grammar Reference**
**File**: `Annex-A-11_Grammar-Reference.md`

> **Note**: This section provides a consolidated EBNF reference for implementers. Sections A.1-A.10 contain grammar productions integrated with explanatory prose; this section consolidates all productions in pure EBNF format without prose for quick reference.

- Complete EBNF listing (all productions from A.1-A.10, consolidated)
- Cross-references to normative text
- Production index

---

### **ANNEX B: PORTABILITY AND BEHAVIOR** *(Informative)*

#### **Section B.0: Behavior Classification**
**File**: `Annex-B-0_Behavior-Classification.md`

- B.0.1 Classification Purpose
- B.0.2 Terminology

#### **Section B.1: Implementation-Defined Behavior**
**File**: `Annex-B-1_Implementation-Defined.md`

- Enumeration of all implementation-defined behaviors with cross-references to normative text
- Examples: integer sizes, alignment, endianness, etc.

#### **Section B.2: Undefined Behavior**
**File**: `Annex-B-2_Undefined-Behavior.md`

- Enumeration of all undefined behaviors with cross-references to normative text
- Each entry shall include:
  1. Catalog number (UB-001, UB-002, etc.)
  2. Brief description
  3. Cross-reference to normative text
  4. Example illustrating the violation
- Format:
  ```
  **UB-001: Out-of-bounds array access**
  Specification: §4.2.2 (Index Expressions)
  Description: Accessing array element outside bounds [0, length)
  Example: `arr[arr.len()]` where arr has length n
  ```
- Examples: out-of-bounds access, null dereference, data races, region escape violations, use-after-move, etc.

#### **Section B.3: Unspecified Behavior**
**File**: `Annex-B-3_Unspecified-Behavior.md`

- Enumeration of all unspecified behaviors with cross-references to normative text
- Examples: evaluation order (where not specified), etc.

---

### **ANNEX C: FORMAL SEMANTICS** *(Informative)*

#### **Section C.0: Overview**
**File**: `Annex-C-0_Semantics-Overview.md`

- C.0.1 Formal Methods Used
- C.0.2 Judgment Forms Summary
- C.0.3 Notation Conventions
- C.0.4 Relationship to Normative Text

#### **Section C.1: Type System Rules**
**File**: `Annex-C-1_Type-System-Rules.md`

- Typing judgments, subtyping, equivalence, well-formedness

#### **Section C.2: Operational Semantics**
**File**: `Annex-C-2_Operational-Semantics.md`

- Evaluation judgments, reduction rules, value forms

#### **Section C.3: Permission Semantics**
**File**: `Annex-C-3_Permission-Semantics.md`

- Permission lattice, checking rules, move semantics

#### **Section C.4: Effect Semantics**
**File**: `Annex-C-4_Effect-Semantics.md`

- Effect tracking, propagation, handlers, principal effects

#### **Section C.5: Contract Semantics**
**File**: `Annex-C-5_Contract-Semantics.md`

- Precondition/postcondition checking, invariants, verification

#### **Section C.6: Modal Semantics**
**File**: `Annex-C-6_Modal-Semantics.md`

- State machines, transitions, reachability, modal typing

#### **Section C.7: Meta-Theory**
**File**: `Annex-C-7_Meta-Theory.md`

- Type safety theorem, progress and preservation

---

### **ANNEX D: IMPLEMENTATION GUIDANCE** *(Informative)*

#### **Section D.0: Implementation Notes**
**File**: `Annex-D-0_Implementation-Notes.md`

- D.0.1 Purpose and Scope
- D.0.2 Non-normative Nature

#### **Section D.1: Abstract Syntax Representation**
**File**: `Annex-D-1_Abstract-Syntax.md`

- D.1.1 Suggested AST Representation
- D.1.2 AST Node Categories
- D.1.3 AST Well-Formedness
- D.1.4 AST Transformations
- D.1.5 Note: Implementations may use alternative representations

#### **Section D.2: Optimization Opportunities**
**File**: `Annex-D-2_Optimizations.md`

- D.2.1 Permission-based Optimizations
- D.2.2 Effect-based Optimizations
- D.2.3 Whole-program Optimizations

#### **Section D.3: Diagnostic Quality**
**File**: `Annex-D-3_Diagnostic-Quality.md`

- D.3.1 Error Message Guidelines
- D.3.2 Warning Guidelines
- D.3.3 Suggestion Mechanisms

#### **Section D.4: Edition System Implementation**
**File**: `Annex-D-4_Edition-System.md`

- D.4.1 Compiler Architecture for Editions
  - Parser configuration per edition
  - Semantic analysis per edition
  - Cross-edition interoperability layer
- D.4.2 Migration Tooling
  - AST transformation for syntax changes
  - Diagnostic guidance for semantic changes
  - Batch migration for large projects
- D.4.3 Testing Across Editions
  - Test suite organization per edition
  - Regression testing for backward compatibility
  - Edition-specific test flags

---

### **ANNEX E: STANDARD LIBRARY OVERVIEW** *(Informative)*

**Decision:** This annex provides an INFORMATIVE overview of the Cursive standard library. Detailed normative specification is maintained separately in "Cursive Standard Library Specification" document (following Rust/Go model).

#### **Section E.0: Standard Library Organization**
**File**: `Annex-E-0_Stdlib-Organization.md`

- E.0.1 Purpose and Scope
  - This annex provides informative overview only
  - Detailed semantics defined in "Cursive Standard Library Specification" (separate document)
  - Not normative for library implementation details
- E.0.2 Conformance Requirements
  - Conforming implementations shall provide core modules listed in §E.1
  - Module interfaces (types, functions) shall match library specification
  - Implementation details (algorithms, optimizations) are implementation-defined
- E.0.3 Library Specification Relationship
  - Library specification maintained separately for independent evolution
  - Library versions may update without language specification revision
  - Cross-references: Language spec → Library spec allowed; reverse discouraged
- E.0.4 Module Structure
- E.0.5 Prelude
- E.0.6 Versioning (independent of language edition)

#### **Section E.1: Core Modules**
**File**: `Annex-E-1_Core-Modules.md`

- E.1.1 Required Core Modules (Conforming implementations shall provide)
  - `core`: Fundamental types (Option, Result, basic predicates)
  - `alloc`: Allocation primitives
  - `io`: Input/output abstractions
- E.1.2 Primitive types, Option, Result, collections overview
- E.1.3 Cross-reference to Cursive Standard Library Specification for details

#### **Section E.2: I/O and System**
**File**: `Annex-E-2_IO-System.md`

- Console I/O, file I/O, system interfaces overview

#### **Section E.3: Standard Predicates and Contracts**
**File**: `Annex-E-3_Predicates-Contracts.md`

- Copy, Clone, Debug, Display, comparison predicates overview

---

### **ANNEX F: GLOSSARY** *(Informative)*

**File**: `Annex-F_Glossary.md`

Alphabetical listing of all technical terms with definitions and cross-references to normative text (A-Z).

---

## File Naming Convention

**Format**: `PartNumber-SectionNumber_Section-Name.md`

**Examples**:
- `01-0_Scope.md`
- `01-4_Conformance.md`
- `01-5_Document-Conventions.md`
- `01-6_Versioning-Evolution.md` (new in v3.1)
- `02-3_Syntax-Notation.md`
- `02-5_Scopes-Name-Lookup.md`
- `04-2_Permission-System.md` (Memory Model - Part IV)
- `05-8_Conversions-Coercions.md` (Expressions - Part V)
- `Annex-A-11_Grammar-Reference.md`
- `Annex-B-2_Undefined-Behavior.md`
- `Annex-D-4_Edition-System.md` (new in v3.1)
- `Annex-E-0_Stdlib-Organization.md`

**Rules**:
- Use two digits for part number (01-15)
- Use one or two digits for section number (0-99)
- Use hyphens for multi-word section names
- Use descriptive but concise names
- Prefix annexes with "Annex-"
- Maintain alphabetical sorting by filename

**Files Added in Version 3.1:**
- `01-6_Versioning-Evolution.md` - Edition system and language evolution policy
- `Annex-D-4_Edition-System.md` - Implementation guidance for edition support

---

## Normative Language Usage

Throughout the normative portions of this specification, the following keywords have the indicated meanings per ISO/IEC Directives Part 2:

- **shall** / **shall not** - Indicates a requirement (mandatory)
- **should** / **should not** - Indicates a recommendation (strongly advised)
- **may** / **may not** - Indicates permission (optional)
- **can** / **cannot** - Indicates capability (informative, not normative)

**Note:** This specification follows ISO/IEC conventions for language specification standards. Readers familiar with RFC 2119 should note: "shall" ≡ "MUST", "should" ≡ "SHOULD", "may" ≡ "MAY".

**Additional Terminology:**
- **ill-formed** - Program violates a rule requiring a compile-time diagnostic
- **ill-formed, no diagnostic required (IFNDR)** - Program violates a rule but no diagnostic is required
- **undefined behavior** - No constraints on program behavior; labeled [UB] and cataloged in Annex B.2

Informative sections (marked as such) and annexes marked "(informative)" are not part of the normative requirements.

---

## Conformance and Standardization Path

This specification is designed to be suitable for submission to ISO/IEC JTC 1/SC 22 (Programming Languages) or ECMA TC39-equivalent working group for potential standardization. The structure, normative language, and content organization follow established conventions from:

- ISO/IEC 14882 (C++)
- ISO/IEC 9899 (C)
- ECMA-334 (C#)
- Java Language Specification (JLS)

Alternatively, this specification may serve as a de facto standard with formal versioning and governance, following the Rust or Go model.
