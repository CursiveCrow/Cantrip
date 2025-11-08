# Cursive Language Specification Standards

## Summary

This document establishes the organizational structure and documentation standards for the Cursive language specification. Key features of this organization:

1. **ISO/IEC Alignment**: Follows ISO/IEC Directives Part 2 for document structure, normative language, and numbered paragraphs
2. **Clause Structure**: 16 main clauses covering lexical structure through compile-time evaluation, plus 10 annexes
3. **Basic Concepts Foundation**: Clause 3 establishes foundational concepts before detailed type system
4. **Logical Ordering**: Modules → Declarations → Names → Types → Expressions → Statements → Compile-Time → Advanced features
5. **Constraints/Semantics Separation**: Explicit subheadings distinguish compile-time requirements from runtime behavior
6. **Consistent Rule Naming**: T-/E-/WF-/P- prefixes for type/evaluation/well-formedness/permission rules
7. **Canonical Examples**: Primary example for each concept marked "Example X.Y.Z.N"
8. **Forward References**: Listed explicitly at start of each clause
9. **Algorithm Specifications**: Detailed algorithms in Annex E for implementers
10. **Comprehensive Annexes**: Grammar, behavior classification, formal semantics, implementation guidance, indices

## Organization Principles

1. **Standards Alignment**: Structure follows ISO/IEC and ECMA language specification conventions
2. **Normative Precision**: Uses ISO normative language (shall/shall not/should/may per ISO/IEC Directives)
3. **Progressive Complexity**: Foundational concepts precede advanced features, with minimal forward references
4. **Modular Files**: Each major subclause (X.Y) has its own file for maintainability and version control
5. **Clear Conformance Boundary**: Explicit separation of normative and informative content
6. **Complete Coverage**: All essential language aspects specified with precision
7. **Explicit Dependencies**: Forward references listed at the start of each clause; circular dependencies documented with clear cross-references
8. **Usability and Clarity**: Complex rules accompanied by informative examples; grammar presented both in-context and consolidated (Annex A)
9. **Consistent Subclause Structure**: All specification subclauses follow a standard template with standardized content ordering (overview → syntax → constraints → semantics → examples), numbered paragraphs for traceability, and progression from common to advanced cases (see "Subclause-Level Content Organization" below)

---

## Subclause-Level Content Organization

This section defines how to structure content **within** individual specification subclauses.

**Terminology Note**: Per ISO/IEC Directives Part 2, this specification uses:

- **Clause** for top-level divisions (e.g., Clause 5, Clause 7)
- **Subclause** for subdivisions (e.g., §5.1, §5.2.3)
- **Paragraph** for numbered content within subclauses (e.g., [1], [2], [3])

### Content Structure Principles

1. **Definition First**: Begin with clear, concise definition establishing what the feature is
2. **Syntax Early**: Present formal grammar after conceptual introduction
3. **Progressive Detail**: Move from simple/common cases to complex/edge cases
4. **Examples Illuminate**: Place examples immediately after the concepts they demonstrate
5. **Numbered Paragraphs**: Use sequential numbering for traceability and cross-referencing
6. **Layered Information**: Structure content in progressive layers (essential → important → advanced)

#### Multi-Level Nesting Guidelines

**Nesting Depth Standards**:

- **Standard subclauses**: 2-3 levels (e.g., §3.1, §3.1.1, §3.1.1.1)
- **Complex multi-category subclauses**: 3-4 levels permitted (e.g., §3.2, §3.2.2, §3.2.2.1, §3.2.2.1.1)
- **Maximum depth**: 5 levels per ISO 2145:1978 standard
- **ISO/IEC compliance**: Cursive follows ISO/IEC Directives Part 2 for document structure

**When to Use Deeper Nesting** (3-4 levels):

- Subclauses covering multiple distinct sub-categories (e.g., primitive types covering multiple type families)
- Each sub-category requires comprehensive specification following the standard template pattern
- The added organizational clarity outweighs the complexity of deeper numbering
- Precedent: ISO/IEC 14882 (C++) uses 4 levels in complex subclauses (e.g., §23.3.13.6 for sequence container requirements)

**Numbering Pattern**:

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

### Standard Subclause Template

Every normative subclause should follow this structure combining **ISO-style numbered paragraphs** with **embedded formal elements** (following ISO/IEC 14882):

**Paragraph Numbering Rule**: Paragraph numbering [N] shall reset to [1] at the start of each X.Y.Z subclause. Sub-numbering (N.M) is used for multi-part conditions within a single paragraph.

**Template Structure:**

- **N.1 Overview**: One-sentence definition, formal definition, purpose/context
- **N.2 Syntax**: Grammar production with reference to Annex A
- **N.3 Constraints**: Compile-time well-formedness requirements with formal rules
- **N.4 Semantics**: Runtime behavior, type rules, evaluation rules with canonical example
- **N.5 Advanced Topics** (optional): Edge cases, advanced interactions
- **N.6 Integration** (optional): Cross-feature interactions

**Stable Label Format**: Labels shall follow the pattern `[clause.topic.subtopic]`, using all lowercase letters, dots to separate hierarchical levels, and full words rather than abbreviations. Examples: `[type.primitive.integer]`, `[expr.call.semantics]`

**Cross-Reference Format**: Use the following standardized formats:

- Subclauses within same clause: `§X.Y` (e.g., §5.2)
- Subclauses in different clauses: `Clause X §X.Y` or simply `§X.Y` if unambiguous
- Specific paragraphs: `§X.Y[N]` (e.g., §5.2[3])
- Annexes: `Annex A §A.X` (e.g., Annex A §A.3)
- Formal rules: `rule T-Feature-Case` or `[T-Feature-Case]`

**Formal Rule Naming Convention**: All formal rules shall follow a consistent prefix scheme:

- **T-Feature-Case**: Type formation and typing rules
- **E-Feature-Case**: Evaluation and operational semantics rules
- **WF-Feature-Case**: Well-formedness and static checking rules
- **P-Feature-Case**: Permission and memory safety rules

Additional prefixes used for specialized semantic categories:

- **Prop-Feature-Case**: Behavior satisfaction and property proofs (e.g., Prop-Int-Copy proves integers satisfy Copy behavior)
- **Coerce-Feature-Case**: Type coercion rules (e.g., Coerce-Never)
- **Prov-Feature-Case**: Provenance and aliasing rules (e.g., Prov-Addr)
- **Ptr-Feature-Case**: Pointer-specific properties and constraints (e.g., Ptr-Size)
- **QR-Feature-Case**: Qualified name resolution rules (e.g., QR-Resolve)

**Formal Rule Placement**: Formal rules shall appear in a boxed environment (using `[ Given: ... ]` for preconditions and `$$...$$` for the rule) immediately following the paragraph that introduces the concept. Always provide a prose explanation after the formal rule.

### Multi-Category Subclause Template

For subclauses covering multiple distinct sub-categories, apply the standard template at two levels:

**Structure:**

- **X.Y.1 Overview**: Introduces all categories with cross-references
- **X.Y.2, X.Y.3, ... [Category Names]**: Each follows full standard template (Overview → Syntax → Constraints → Semantics)
- **X.Y.M Integration** (optional): Cross-category interactions

**Key Principles:**

1. Top-level overview introduces all categories
2. Each category follows full standard template internally
3. Integration subclause shows cross-category interactions (optional if minimal)
4. Stable labels follow hierarchical pattern: `[clause.topic.category.subtopic]`
5. Numbering depth: 4 levels maximum (X.Y.Z.W) within ISO 2145 limits
6. Paragraph numbering resets to [1] at each X.Y.Z.W subclause

**When NOT to use:**

- Subclauses with only 1-2 major concepts
- Categories sharing significant semantics (consolidate instead)
- Purely conceptual subclauses without distinct implementable categories

### Documentation Elements

#### Syntax Grammar Formatting

**Grammar Notation** (defined in §1.4):

This specification uses a **hybrid grammar notation approach**:

1. **Annex A (Authoritative Grammar)**: Uses complete **ANTLR-style EBNF notation** suitable for parser generation. This is the single normative source for all grammar productions.

2. **Individual Clauses (Prose Grammar)**: May use **simplified notation** for readability in prose, but must reference Annex A for the authoritative form.

**ANTLR-style notation** (used in Annex A) includes:

- **Non-terminals**: `production_rule`, `type_expression` (lowercase with underscores)
- **Optional elements**: `element?` (zero or one occurrence)
- **Repetition**: `element*` (zero or more), `element+` (one or more)
- **Alternatives**: `|` separator between alternatives
- **Terminals**: Keywords in quotes `'keyword'`, operators in quotes `'+'`
- **Grouping**: Parentheses `( ... )` for grouping alternatives
- **Production end**: `;` terminates each production rule

**Simplified notation** (used in clause prose) may use:

- Descriptive notation in code fences for readability in prose
- Abbreviated forms focusing on key syntax elements
- Inline examples showing common patterns

**Example (ANTLR-style from Annex A):**

```antlr
variable_declaration
    : 'let' identifier (':' type_expression)? '=' expression
    | 'var' identifier (':' type_expression)? '=' expression
    ;
```

**Example (Simplified notation in clause prose):**

```ebnf
VarDeclStmt ::= "let" Pattern (":" Type)? "=" Expr
             | "var" Pattern (":" Type)? "=" Expr
```

**Grammar References**: Every grammar production in clause prose shall include a cross-reference to the authoritative grammar in Annex A. Format: "See Annex A §A.X for complete grammar." or "See §A.X for complete grammar."

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

#### Formal Elements and Mathematics Formatting

**Requirements**:

- All mathematical and formal notation is written in LaTeX, using inline `$…$` or display `\[ … \]`/`$$ … $$` delimiters.
- Judgment forms: `Γ ⊢ e : T` (context `Γ` derives expression `e` with type `T`)

#### Example Formatting

**Requirements**:

- **Placement**: Immediately after the concept being demonstrated
- **Compilability**: All examples should be compilable (or clearly marked as errors)
- **Numbering**: Sequential within each section
- **Titles**: Descriptive titles indicating what is demonstrated
- **Comments**: Inline comments explaining behavior
- **Both paths**: Show correct usage AND common errors

**Example Markers**:

- Mark invalid code with "- invalid" suffix in the example title
- Use `// error:` comments to explain why code is ill-formed
- Valid code requires no special marker
- Keep examples minimal yet complete
- Progress from simple to complex

**Quantity Guidance**:

- **Major concepts**: Exactly 1 canonical example (marked "**Example X.Y.Z.N**") demonstrating the primary use case
- **Complex features**: 2-3 additional examples (common case, edge case, error case)
- **Error scenarios**: 1 example showing typical mistake with clear "- invalid" marker
- **Progressive series**: Simple → intermediate → advanced (when teaching progression)

**Canonical Example Characteristics**:

- Marked with "**Example X.Y.Z.N** (Feature Name):"
- Self-contained and minimal (5-15 lines typical)
- Demonstrates the 80% use case
- Becomes the reference implementation for that concept
- Compilable (or clearly marked as demonstrating an error with "- invalid")

#### Example Naming Conventions

**Purpose**: This section establishes consistent naming patterns for identifiers used in specification examples to enhance clarity, avoid language-specific associations, and maintain professional standards.

**General Principles**:

- **Consistency**: Use the same name for the same concept throughout related examples
- **Clarity**: Use descriptive names for semantic examples; generic names for syntax demonstrations
- **Neutrality**: Avoid language-specific names (especially Rust-specific patterns)
- **Progression**: Start with simple abstract names (x, y, foo), progress to concrete names (Point, Person) as concepts develop

**Variable Names**:

| Category          | Recommended Names                             | Usage Context                                         |
| ----------------- | --------------------------------------------- | ----------------------------------------------------- |
| Generic variables | `x`, `y`, `z`, `a`, `b`, `c`                  | Simple examples, parameters, mathematical expressions |
| Loop indices      | `i`, `j`, `k`                                 | Iteration, counting                                   |
| Numeric values    | `n`, `m`, `count`, `total`, `sum`             | Sizes, quantities, accumulation                       |
| Descriptive       | `value`, `result`, `item`, `element`, `entry` | Semantic examples with clear purpose                  |
| Collections       | `items`, `list`, `array`, `collection`        | Container variables                                   |
| Positions         | `first`, `last`, `current`, `next`, `prev`    | Sequential access                                     |
| Metasyntactic     | `foo`, `bar`, `baz`, `qux`                    | Placeholder examples (use sparingly)                  |

**Type Parameter Names**:

| Parameter | Meaning          | Usage Context                                       |
| --------- | ---------------- | --------------------------------------------------- |
| `T`       | Type (primary)   | Single type parameter in generic definitions        |
| `U`       | Type (secondary) | Second type parameter                               |
| `V`       | Type (tertiary)  | Third type parameter or Value in key-value contexts |
| `E`       | Element          | Collection element type                             |
| `K`       | Key              | Map or associative structure key type               |
| `R`       | Result/Return    | Function return type in transformations             |
| `S`       | State            | State machine or modal type state                   |

**Concrete Type Names**:

| Domain           | Recommended Names                                 | Examples                    |
| ---------------- | ------------------------------------------------- | --------------------------- |
| Geometric        | `Point`, `Rectangle`, `Circle`, `Line`, `Vector`  | Spatial examples            |
| Data structures  | `List`, `Tree`, `Node`, `Graph`, `Stack`, `Queue` | Container types             |
| Domain objects   | `Person`, `Employee`, `Customer`, `Account`       | Business/domain logic       |
| System resources | `FileHandle`, `Connection`, `Buffer`, `Stream`    | I/O and resource management |
| Generic data     | `Record`, `Entry`, `Item`, `Document`, `Entity`   | Abstract data examples      |

**Function and Procedure Names**:

| Category            | Recommended Names                          | Usage Context                           |
| ------------------- | ------------------------------------------ | --------------------------------------- |
| Abstract functions  | `f`, `g`, `h`                              | Type signatures, higher-order functions |
| Abstract procedures | `m`, `n`, `o`                              | Statement/grant demonstrations          |
| Mathematical ops    | `add`, `subtract`, `multiply`, `divide`    | Arithmetic examples                     |
| Standard operations | `map`, `filter`, `reduce`, `transform`     | Collection processing                   |
| Boolean queries     | `is_valid`, `has_value`, `can_access`      | Boolean test functions                  |
| Converters          | `to_string`, `parse_int`, `from_bytes`     | Type conversions                        |
| Generic processing  | `process`, `compute`, `calculate`, `apply` | Algorithm examples                      |

**Field and Member Names**:

| Category    | Recommended Names                          | Usage Context        |
| ----------- | ------------------------------------------ | -------------------- |
| Properties  | `name`, `id`, `value`, `data`              | Object attributes    |
| Dimensions  | `x`, `y`, `z`, `width`, `height`, `length` | Geometric properties |
| State flags | `is_active`, `is_valid`, `has_children`    | Boolean state        |
| Collections | `items`, `entries`, `children`, `elements` | Contained data       |
| Metadata    | `created_at`, `modified_by`, `version`     | Tracking information |

**Names to AVOID**:

**Critical**: Do NOT use the following in Cursive specification examples. These are Rust-specific types, patterns, and syntax that do not exist in Cursive:

| Avoid                       | Reason                     | Cursive Alternative                                            |
| --------------------------- | -------------------------- | -------------------------------------------------------------- |
| `Option<T>`, `Some`, `None` | Rust-specific pattern      | Use Cursive enums if nullable types are needed                 |
| `Result<T, E>`, `Ok`, `Err` | Rust-specific pattern      | Use Cursive enums for error handling patterns                  |
| `Box<T>`                    | Rust heap allocation type  | Use Cursive's pointer types with region/permission annotations |
| `Vec<T>`                    | Rust collection type       | Use Cursive array syntax `[T]` or concrete collection types    |
| `Arc<T>`, `Rc<T>`           | Rust reference counting    | Use Cursive's permission system (shared/unique)                |
| `RefCell<T>`, `Cell<T>`     | Rust interior mutability   | Use Cursive's permission system                                |
| `str`, `&str`               | Rust string types          | Use Cursive's `string` modal type (`string@Managed`, `string@View`) |
| `mut` keyword/prefix        | Rust mutability marker     | Use Cursive's permission annotations (unique, shared, const)   |
| `&T`, `&mut T`              | Rust reference syntax      | Use Cursive's pointer/reference syntax with permissions        |
| `impl Trait`                | Rust syntax                | Use Cursive's behavior syntax                                  |
| `dyn Trait`                 | Rust trait objects         | Use Cursive's witness system                                   |
| `.unwrap()`, `.expect()`    | Rust Option/Result methods | Use Cursive's pattern matching and error handling              |

**Additional Avoidance Patterns**:

- Single letters `l` (looks like 1) or `O` (looks like 0)
- Hungarian notation (`strName`, `iCount`)
- Overly abbreviated names (`cnt`, `idx`, `tmp`) except in very local scopes
- Language keywords from other languages that might confuse readers

**Example Progression Pattern**:

```
// Stage 1: Abstract syntax (use generic names)
let x = 42
let y = x + 10

// Stage 2: Simple semantics (use simple descriptive names)
let count = 0
let total = count + value

// Stage 3: Realistic examples (use domain-appropriate names)
let customer_count = database.query("SELECT COUNT(*) FROM customers")
let total_revenue = calculate_revenue(customer_count, average_sale_price)
```

**Consistency Guidelines**:

- Within a section, use the same names for the same concepts
- In progressive examples, maintain name consistency as complexity increases
- Cross-reference related examples to ensure naming alignment
- Document any intentional name variations with clear rationale

#### Notes and Warnings

**Note Format** (Informative - following ISO/IEC 14882 style):

```markdown
[ Note: Clarifying information that aids understanding but does not
impose normative requirements. May include rationale, implementation
guidance, or comparisons to other languages.
— end note ]
```

**Warning Format** (Safety-Critical):

```markdown
[ Warning: Violating this constraint results in undefined behavior [UB-ID: B.2.15].
See Annex B §B.2 for complete undefined behavior catalog.
— end warning ]
```

**Rationale Format** (Design Decisions):

```markdown
[ Rationale: This restriction ensures [goal] and prevents [problem].
Alternative designs were considered but rejected because [reason].
— end rationale ]
```

**Placement**: Immediately after the rule or concept being clarified.

**UB References**: Undefined behavior conditions shall be referenced inline using the format `[UB-ID: B.2.N]` where N is the catalog entry number in Annex B §B.2.

#### Visual Hierarchy and Formatting

**Text Formatting Standards**:

- **Bold** for normative requirements (shall/shall not)
- _Italic_ for defined terms on first use in each subclause
- `Code font` for language elements (keywords, identifiers, operators)
- Regular text for informative prose

**Structural Elements**:

- **Boxed environments** for formal rules (using `[ Given: ... ]` and `$$...$$`)
- **Indented blocks** for notes, warnings, and rationale (using `[ Note: ... — end note ]` format)
- **Code blocks** with syntax highlighting for examples
- **Numbered lists** for alternatives and multi-part conditions

**Emphasis for Safety**:

- Undefined behavior warnings shall use prominent formatting
- UB-ID references shall be clearly visible: `[UB-ID: B.2.N]`
- Safety-critical constraints shall be marked with warning blocks

**Digital Version Requirements**:

- All cross-references shall be clickable hyperlinks
- Stable labels shall enable permanent URLs
- Grammar productions shall link to Annex A
- Formal rules shall be indexed and searchable
- UB-IDs shall link to Annex B catalog

#### Cross-Reference Format

**Subclause References** (standardized format):

- Within same clause: `§X.Y` (e.g., §5.2)
- Across clauses: `§X.Y` or `Clause X §X.Y` if emphasis needed (e.g., §7.3 or Clause 7 §7.3)
- Annexes: `Annex A §A.X` (e.g., Annex A §A.3)
- Specific paragraphs: `§X.Y[N]` (e.g., §5.2[3])
- Formal rules: `rule T-Feature-Case` or `[T-Feature-Case]`
- UB catalog entries: `[UB-ID: B.2.N]`

**Bidirectional Linking**:

- Forward reference: "See §X.Y for detailed semantics"
- Backward reference: "As described in §A.B, ..."
- Integration reference: "See Clause M for memory model interaction"

**Hyperlinks**: In digital versions, all cross-references shall be clickable hyperlinks. This includes:

- All §X.Y references
- All Annex references
- All formal rule references
- All UB-ID references
- All defined term uses (linking to §1.3 or first definition)

### Subclause Flow Best Practices

#### Opening Structure

**DO:**

- Start with clear, one-sentence definition
- Provide immediate context (why this exists)
- Include simple example early (especially for practical features)
- Establish scope and boundaries
- List forward references if any

**DON'T:**

- Dive into technical details immediately
- Assume reader knows related concepts
- Start with exceptions or edge cases
- Use undefined terms in opening

#### Ordering Within Subclauses

**Standard Flow:**

1. **Overview** (definition, purpose, context)
2. **Syntax** (formal grammar with reference to Annex A)
3. **Constraints** (compile-time requirements, well-formedness)
4. **Semantics** (runtime behavior, type rules, evaluation rules)
5. **Examples** (interspersed immediately after relevant concepts)
6. **Advanced Topics** (optional: edge cases, special conditions)
7. **Integration** (optional: cross-references, interactions if significant)

**Progressive Complexity:**

- Start with most common use case (80/20 rule)
- Introduce basic rules before exceptions
- Show concrete examples before abstractions
- Present familiar concepts before novel ones
- Place canonical example first, then additional examples

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

### Implementation Considerations

**Compilation Phases**: The specification is organized to support a multi-phase compilation model:

1. **Lexical Analysis** (Clause 2): Produces tokens from source text
2. **Parsing** (Annex A): Produces abstract syntax tree (AST)
3. **Name Resolution** (Clause 6): Binds identifiers to declarations
4. **Type Checking** (Clause 7): Assigns types and checks type constraints
5. **Permission Checking** (Clause 12): Verifies memory safety constraints
6. **Contract Verification** (Clause 13): Checks preconditions/postconditions
7. **Code Generation**: Produces executable code

Each clause specifies what information must be available at that phase and what errors must be diagnosed.

**Algorithm Specifications**: Annex E §E.2 provides detailed algorithms for:

- Name resolution (with shadowing and qualification)
- Type inference
- Overload resolution
- Behavior resolution
- Permission checking
- Region escape analysis

**Diagnostic Requirements**: For each ill-formed construct, the specification indicates:

- Whether diagnosis is **required** (shall diagnose)
- Whether diagnosis is **recommended** (should diagnose)
- Suggested diagnostic messages (in Annex E §E.5)

**AST Requirements**: Annex E §E.3 specifies required AST node types, attributes, and well-formedness invariants that must hold after each compilation phase.

---

## Specification Structure

### Main Clauses

- **1 Introduction and Conformance** [intro]

  - 1.1 Scope [intro.scope]
  - 1.2 Normative References [intro.refs]
  - 1.3 Terms and Definitions [intro.terms]
  - 1.4 Notation and Conventions (ANTLR-style EBNF, math, judgments) [intro.notation]
  - 1.5 Conformance (shall/should/may; defined/unspecified/UB; IFNDR) [intro.conformance]
  - 1.6 Document Conventions (stable labels, examples, cross-refs, hyperlinks) [intro.document]
  - 1.7 Versioning and Evolution (semantic versioning, deprecation, stability) [intro.versioning]

- **2 Lexical Structure and Translation** [lex]

  - 2.1 Source Text and Encoding (Unicode, line endings, BOM) [lex.source]
  - 2.2 Translation Phases [lex.phases]
  - 2.3 Lexical Elements (whitespace, comments, identifiers, literals, tokens) [lex.tokens]
  - 2.4 Tokenization and Statement Termination [lex.terminators]
  - 2.5 Compilation Units and Top-Level Forms [lex.units]

- **3 Basic Concepts** [basic]

  **Forward references:** Modules (Clause 4), Declarations (Clause 5), Names and Scopes (Clause 6), Types (Clause 7), Memory Model (Clause 12)

  - 3.1 Objects and Values [basic.object]
  - 3.2 Types (overview and classification) [basic.type]
  - 3.3 Scope and Lifetime (overview) [basic.scope]
  - 3.4 Storage Duration [basic.storage]
  - 3.5 Alignment and Layout (overview) [basic.align]
  - 3.6 Name Binding Categories (value/type/module/predicate/contract/label) [basic.binding]

- **4 Modules** [module]

  **Forward references:** None

  - 4.1 Module Overview and File-Based Organization [module.overview]
  - 4.2 Module Syntax [module.syntax]
  - 4.3 Module Scope Formation [module.scope]
  - 4.4 Export/Import and Re-exports [module.export]
  - 4.5 Qualified Names and Resolution (module paths) [module.qualified]
  - 4.6 Cycles, Initialization Order, Diagnostics [module.initialization]

- **5 Declarations** [decl]

  **Forward references:** Names and Scopes (Clause 6), Expressions (Clause 8), Generics (Clause 11)

  - 5.1 Declaration Overview [decl.overview]
  - 5.2 Variable Bindings and Initializers [decl.variable]
  - 5.3 Binding Patterns (syntax, destructuring, binding categories) [decl.pattern]
  - 5.4 Procedures (parameters, results, callable bodies) [decl.function]
  - 5.5 Type Declarations (records, enums, aliases) [decl.type]
  - 5.6 Visibility Rules (scope-based) [decl.visibility]
  - 5.7 Initialization and Definite Assignment [decl.initialization]
  - 5.8 Program Entry Point and Execution Model [decl.entry]

- **6 Names, Scopes, and Resolution** [name]

  **Forward references:** Expressions (Clause 8)

  - 6.1 Identifiers [name.identifier]
  - 6.2 Scope Formation (blocks, procedures, type bodies, modules) [name.scope]
  - 6.3 Name Introduction and Shadowing [name.shadow]
  - 6.4 Name Lookup: Unqualified and Qualified [name.lookup]
  - 6.5 Disambiguation and Ambiguity Diagnostics [name.ambiguity]
  - 6.6 Predeclared Bindings [name.predeclared]

- **7 Types** [type]

  **Forward references:** Expressions (Clause 8), Generics (Clause 11), Memory Model (Clause 12)

  - 7.1 Type System Overview [type.overview]
  - 7.2 Primitive Types [type.primitive]
  - 7.3 Composite Types (records, tuples, enums, unions) [type.composite]
  - 7.4 Callable (Function) Types [type.function]
  - 7.5 Pointer and Reference Types (safe/raw) [type.pointer]
  - 7.6 Type Equivalence, Subtyping, Compatibility [type.relation]
  - 7.7 Type Introspection (compile-time queries) [type.introspection]

- **8 Expressions** [expr]

  **Forward references:** Statements (Clause 9), Compile-Time Evaluation (Clause 10)

  - 8.1 Expression Fundamentals (model; value/place categories) [expr.fundamental]
  - 8.2 Primary and Postfix (literals, identifiers, calls, field/index) [expr.primary]
  - 8.3 Unary and Binary Operators (precedence, associativity) [expr.operator]
  - 8.4 Structured Expressions (record/enum construction, closures, if/match) [expr.structured]
  - 8.5 Patterns and Exhaustiveness (forms, refutable/irrefutable, guards) [expr.pattern]
  - 8.6 Conversions and Coercions (implicit/explicit; numeric/pointer/unsizing) [expr.conversion]
  - 8.7 Constant Expressions and Comptime Contexts [expr.constant]
  - 8.8 Expression Typing Rules and Ill-formed Cases [expr.typing]

- **9 Statements and Control Flow** [stmt]

  **Forward references:** None

  - 9.1 Statement Fundamentals [stmt.fundamental]
  - 9.2 Simple Statements (let/var/assign/return/defer/labels) [stmt.simple]
  - 9.3 Control Flow (if/while/for/loop/break/continue) [stmt.control]
  - 9.4 Evaluation Order, Sequencing, Short-circuit, Divergence [stmt.order]

- **10 Generics and Behaviors** [generic]

  **Forward references:** Witness System (Clause 13), Memory Model (Clause 11)

  - 10.1 Region Parameters Overview [generic.regions]
  - 10.2 Type Parameters (declaration, scope, defaults) [generic.parameter]
  - 10.3 Bounds and Where-Constraints [generic.bounds]
  - 10.4 Behaviors: Declarations and Items [generic.behavior]
  - 10.5 Behavior Implementations and Coherence (overlap/orphan) [generic.implementation]
  - 10.6 Resolution and Monomorphization [generic.resolution]
  - 10.7 Variance and Its Inference [generic.variance]

- **11 Memory Model, Regions, and Permissions** [memory]

  **Forward references:** Contracts (Clause 12), Witness System (Clause 13)

  - 11.1 Memory Model Overview [memory.overview]
  - 11.2 Objects and Storage Duration [memory.object]
  - 11.3 Regions (syntax, allocation, non-escape, region stack) [memory.region]
  - 11.4 Permissions (const/shared/unique; lattice; upgrades/downgrades) [memory.permission]
  - 11.5 Move/Copy/Clone Semantics [memory.move]
  - 11.6 Layout and Alignment (records, enums, padding) [memory.layout]
  - 11.7 Aliasing and Uniqueness Rules [memory.aliasing]
  - 11.8 Unsafe Blocks and Safety Obligations (general, non-FFI) [memory.unsafe]

- **12 Contracts** [contract]

  **Forward references:** Witness System (Clause 13)

  - 12.1 Overview and Model [contract.overview]
  - 12.2 Sequent: Syntax and Structure (grants, must, will, where) [contract.sequent]
  - 12.3 Grants (within Sequent): catalog, user-defined, subsumption, WF [contract.grant]
  - 12.4 Preconditions: must [contract.precondition]
  - 12.5 Postconditions: will [contract.postcondition]
  - 12.6 Invariants: where [contract.invariant]
  - 12.7 Sequent Checking Flow (call/return/scope) [contract.checking]
  - 12.8 Verification Modes (static/runtime/none) and dynamic hooks to Witnesses [contract.verification]
  - 12.9 Grammar Reference (sequent + parts) [contract.grammar]
  - 12.10 Conformance and Diagnostics [contract.diagnostics]

- **13 Witness System** [witness]

  **Forward references:** None (integrates Clauses 10 and 12)

  - 13.1 Overview and Purpose [witness.overview]
  - 13.2 Witness Kinds (behavior witness, contract witness) [witness.kind]
  - 13.3 Formation and Construction (static vs dynamic sites) [witness.formation]
  - 13.4 Representation and Erasure [witness.representation]
  - 13.5 Dispatch Semantics (static vs dynamic via witness) [witness.dispatch]
  - 13.6 Regions, Permissions, and Grants Interplay [witness.memory]
  - 13.7 Grammar Hooks and References [witness.grammar]
  - 13.8 Diagnostics (missing/ambiguous/incoherent evidence) [witness.diagnostics]

- **14 Concurrency and Memory Ordering** [concurrency]

  **Forward references:** None

  - 14.1 Concurrency Model (threads/tasks surface; library interplay) [concurrency.model]
  - 14.2 Memory Model (happens-before, synchronizes-with, DRF) [concurrency.memory]
  - 14.3 Atomic Operations and Orderings (acq/rel/seqcst/relaxed) [concurrency.atomic]
  - 14.4 Synchronization Primitives (surface specification) [concurrency.synchronization]

- **15 Interoperability and ABI** [interop]

  **Forward references:** None

  - 15.1 FFI Declarations and Safety Obligations [interop.ffi]
  - 15.2 FFI-Specific Unsafe Usage (raw pointers, sequent obligations) [interop.unsafe]
  - 15.3 C Compatibility (repr, strings, procedure pointers, variadics) [interop.c]
  - 15.4 Platform-Specific Features (inline asm, SIMD, intrinsics) [interop.platform]
  - 15.5 Linkage and Symbol Visibility (name mangling, static/dynamic) [interop.linkage]
  - 15.6 ABI Specification (calling conventions, data layout, name mangling) [interop.abi]
  - 15.7 Binary Compatibility [interop.compatibility]

- **16 Compile-Time Evaluation and Reflection** [comptime]

  **Forward references:** None

  - 16.1 Const/Comptime Execution (const procedures/contexts/blocks) [comptime.execution]
  - 16.2 Reflection and Type Queries [comptime.reflection]
  - 16.3 Code Generation Patterns via Attributes (derivations; no macros) [comptime.codegen]

### Annexes

**Normative Annexes:**

- **Annex A: Complete Grammar** [grammar]

  - A.1 Grammar Notation and Conventions [grammar.notation]
  - A.2 Lexical Grammar [grammar.lexical]
  - A.3 Type Grammar [grammar.type]
  - A.4 Expression Grammar [grammar.expression]
  - A.5 Statement Grammar [grammar.statement]
  - A.6 Pattern Grammar [grammar.pattern]
  - A.7 Declaration Grammar [grammar.declaration]
  - A.8 Sequent Grammar (grants/must/will/where) [grammar.sequent]
  - A.9 Attribute Grammar [grammar.attribute]
  - A.10 Consolidated Grammar Reference [grammar.consolidated]

- **Annex B: Behavior Classification** [behavior]
  - B.1 Implementation-Defined Behavior Catalog [behavior.implementation]
  - B.2 Undefined Behavior Catalog (UB-IDs) [behavior.undefined]
  - B.3 Unspecified Behavior Catalog [behavior.unspecified]

**Informative Annexes:**

- **Annex C: Formal Semantics** [formal]

  - C.1 Type System Rules [formal.type]
  - C.2 Operational Semantics [formal.operational]
  - C.3 Denotational Semantics [formal.denotational]
  - C.4 Permission/Move Semantics [formal.permission]
  - C.5 Witness Semantics [formal.witness]
  - C.6 Contract/Sequent Semantics [formal.contract]
  - C.7 Meta-Theory (progress/preservation/soundness) [formal.metatheory]

- **Annex D: Implementation Limits** [limits]

  - D.1 Recommended Minimum Limits [limits.minimum]
  - D.2 Translation Limits [limits.translation]

- **Annex E: Implementation Guidance** [implementation]

  - E.1 Compilation Phases and Information Flow [implementation.phases]
  - E.2 Algorithm Specifications [implementation.algorithms]
    - E.2.1 Name Resolution Algorithm [implementation.algorithms.name]
    - E.2.2 Type Inference Algorithm [implementation.algorithms.type]
    - E.2.3 Overload Resolution Algorithm [implementation.algorithms.overload]
    - E.2.4 Behavior Resolution Algorithm [implementation.algorithms.behavior]
    - E.2.5 Permission Checking Algorithm [implementation.algorithms.permission]
    - E.2.6 Region Escape Analysis Algorithm [implementation.algorithms.region]
  - E.3 AST Requirements and Well-Formedness [implementation.ast]
  - E.4 Optimization Constraints (permission/grant-aware) [implementation.optimization]
  - E.5 Diagnostic Requirements and Quality Guidelines [implementation.diagnostics]
  - E.6 Edition System Implementation [implementation.editions]

- **Annex F: Standard Library Overview** [library]

  [ Note: Full library specification maintained in separate document.
  — end note ]

  - F.1 Library Organization [library.organization]
  - F.2 Core Types and Behaviors [library.core]
  - F.3 Standard Behaviors [library.behaviors]

- **Annex G: Portability Considerations** [portability]

  - G.1 Platform-Specific Behavior [portability.platform]
  - G.2 Separate Compilation and Linking [portability.compilation]
  - G.3 Binary Compatibility Guidelines [portability.compatibility]

- **Annex H: Changes and Evolution** [changes]

  - H.1 Version Changes [changes.editions]
  - H.2 Deprecation Policy [changes.deprecation]
  - H.3 Migration Guidance [changes.migration]

- **Annex I: Glossary** [glossary]

  - I.1 Technical Terms [glossary.terms]
  - I.2 Notation Index [glossary.notation]

- **Annex J: Cross-Reference Indices** [index]
  - J.1 Stable Label Index (machine-readable JSON) [index.labels]
  - J.2 Grammar Production Index [index.grammar]
  - J.3 Formal Rule Index (T-/E-/WF-/P- rules) [index.rules]
  - J.4 Example Index [index.examples]
  - J.5 UB-ID Index [index.undefined]
