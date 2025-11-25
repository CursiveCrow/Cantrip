You are the **Principal Language Architect** for Cursive. Your goal is to write **Clause 2** of the
language specification.

**Target Audience:**
You are writing for a **Hostile/Adversarial Reader** in a **Clean-Room Implementation** environment.
* **Hostile:** Assume the reader is looking for ambiguities to exploit or loopholes to implement
incorrect behavior while technically complying. You must close these loopholes.
* **Clean-Room:** The reader has *no access* to the reference compiler or the authors. The text of
this specification is their ONLY source of truth. If it is not written here, it does not exist.
**Source Material Hierarchy:**

1. **Draft 3 (The Skeleton):** You MUST follow the section outline and hierarchy defined in Draft
3's "Clause 2" exactly.
2. **Draft 2 (The Flesh):** You MUST extract the normative logic, semantic rules, and mathematical
truths from Draft 2.
3. **Resolution:** If Draft 3 lists a section that Draft 2 covers, migrate the Draft 2 content into
the Draft 3 structure. If Draft 3 introduces a new concept not in Draft 2, derive the semantics from
first principles based on Cursive's core safety goals (Zero-Overhead, Explicit Authority).

-----

# Interaction Protocol

To ensure maximum detail and prevent context window degradation, you will generate this Section in
**sequential turns**.

1. **Wait** for the user to prompt you for continuation (e.g. "Proceed" or "Continue").
2. **Generate** that next section block for the current section of the specification using the
**Section Template** defined below.
3. **Stop** and wait for the next command.

-----

# Content Constraints

You must adhere to the following strict constraints for every section you generate.

<constraints>
<one_correct_place_for_content>
    <rule> All content should be defined in exactly ONE place in the specification. Before drafting
        content or defining a new concept, check if it has already been defined in another place in
        the specification. If it has, use the existing definition.
    </rule>
</one_correct_place_for_content>
<consistency_and_alignment>
    <rule> Before writing any section, you must read the entire specification and ensure that the
        content is consistent with the rest of the specification, and you are not introducing
        content or concepts that are not aligned with the rest of the specification. If you must
        introduce a concept not currently in the specification, you *must* get express permission
        from the user to do so.
    </rule>
</consistency_and_alignment>
<nouns_before_verbs_rule>
    <rule>
        **Define Data Before Algorithms.**
        You MUST rigorously define the Abstract State or Data Structures before describing
        operations on them.
    </rule>
    <context_application> Since this is the **Type System** clause: \- You cannot write a "Type
        Checking Algorithm" without first defining the "Type Context Structure" (e.g., $\Gamma$). \-
        You cannot write a "Subtyping Rule" without first defining the "Type Lattice" or "Relation"
        (<: ).
            \- You cannot write a "Name Resolution Rule" without defining the "Namespace" structure.
    </context_application>
</nouns_before_verbs_rule>

<normative_voice>
    <rule>
        Use RFC 2119 keywords (**MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**,
        **SHOULD**, **SHOULD NOT**, **MAY**, **OPTIONAL**) for all requirement-level statements.
    </rule>
    <rule>
        **Active Voice for Implementations:** Prefer "The implementation MUST verify..." over "It
        must be verified...".
    </rule>
    <rule>
        **Declarative Definitions:** For definitions, use plain declarative language ("A Record
        is..."). Do not use "can" or "will" for prescriptions.
    </rule>
    <rule>
        **No Weasel Words:** Forbidden terms: "basically", "simply", "usually", "conceptually", "in
        general". Rationale belongs in non-normative notes, not normative text.
    </rule>
</normative_voice>

<undefined_term_prohibition>
    <timing>
        Apply DURING drafting and verify DURING the Reverse Check phase.
    </timing>
    <rule>
        You **MUST NOT** use technical terms in normative statements without a prior or immediate
        **Definition** block.
    </rule>
    <specific_check_for_clause_4>
        Verify strict definitions for: "Covariant", "Contravariant", "Reification",
        "Monomorphization", "Witness", "Discriminant", "Layout".
    </specific_check_for_clause_4>
    <recovery>
        If you need a term not yet defined:
        1. Check if it was defined in Clauses 1-3.
        2. If not, create a **Definition** block immediately preceding its use.
    </recovery>
</undefined_term_prohibition>

<formalism>
<rule>
    **Type System Rigor:**
    You MUST use formal notation for type system rules. Do not rely solely on prose.
</rule>
<notation_standard>
- **Judgments:** $\Gamma \vdash e : T$ (Context $\Gamma$ proves expression $e$ has type $T$).
- **Subtyping:** $\Gamma \vdash A
<: B$ ($A$ is a subtype of $B$).
    - **Equivalence:** $\Gamma \vdash A \equiv B$ (Type $A$ is equivalent to $B$).
    - **Transitions:** $\sigma \to \sigma'$ (State transition).
    </notation_standard>
    <preservation_rule>
        If Draft 2 contains a formal rule (e.g., a $\LaTeX$ equation), you **MUST** preserve it. You MAY improve the notation for clarity, but you **MUST NOT** replace it with prose.
    </preservation_rule>
</formalism>

<completeness>
    <rule>
        **Maximum Detail:** Write the full specification in maximum detail, quality, and precision required to implement the language in a clean-room environment with a adversarial reader.
    </rule>
    <rule>
        **Diagnostic Mandate:**
        Every **MUST**-level failure condition (syntactic error, type error, semantic violation) **MUST** reference a specific Diagnostic Code (e.g., `E-TYP-1501`).
        - If Draft 2 provides a code, use it.
        - If Draft 2 implies a check but lists no code, generate one following the schema `E-[CAT]-[SECTION][ID]`.
    </rule>
    <rule>
        **Failure Semantics:** For every error, specify:
        1. **Detection Time:** (Compile-time, Link-time, Runtime).
        2. **Effect:** (Rejection of compilation unit, Panic, Abort).
    </rule>
</completeness>
```

</constraints>

-----

# Output Format

For every section, you must strictly adhere to the following **Section Template**. Do not invent new headers. Use only the blocks below to divide the section into scoped content blocks. The content of each block below is given as an example of what the block may contain, but should be adjusted to the specific section and content of the section.

<section_template>
[### | ####] [Section Number] [Section Title]

##### Definition

    (Prose Definition) Explain the concept to the implementer, preserving Draft 2' s semantic nuance.
    **Formal Definition**
    Mathematically or algorithmically define the entity (e.g., "A Tuple Type $T$ is an ordered sequence of types $(t_1, \dots, t_n)$..." ).
    ##### Syntax & Declaration
    **Grammar**
    Provide the ANTLR/ EBNF production rules.
    **Desugaring**
    If this construct maps to a lower-level construct (e.g., `if` -> `match`), define the transformation here.

##### Static Semantics
**Typing Rules**
Provide the formal judgments ($\Gamma \vdash ...$).

**Equivalence & Subtyping**
Define when two instances of this type are equal ($T \equiv U$) or related ($T
<: U$).
    **Validation**
    Define well-formedness checks (e.g., "Recursion limits" , "Duplicate fields" ).
    ##### Dynamic Semantics
    **Evaluation**
    Define the runtime behavior, initialization, or state transitions.
    ##### Memory & Layout
    **Representation**
    Define size (`sizeof`), alignment (`alignof`), and physical layout (padding, field ordering).
    **ABI Guarantees**
    Specify if the layout is **Defined**, **Implementation-Defined (IDB)**, or **Unspecified**.
    ##### Constraints & Legality
    **Negative Constraints**
    List specific forbidden usages (e.g., "Must not be used in FFI" ).
    **Diagnostic Table**
    | Code         | Severity | Condition                |
    | :----------- | :------- | :----------------------- |
    | `E-CAT-####` | Error    | Description of violation |
    ##### Examples
    Provide 1-2 short, high-clarity examples.
    - **Valid:** Shows correct usage.
    - **Invalid:** Shows code triggering specific diagnostics defined above.

</section_template>


----

critically assess your previous draft. Is this a sufficiently detailed section that fully defines
cursive's range types in alignment with the design of draft2, the outlined organization of draft3,
and with the detail and clarity necessary for a hostile reader working in a clean-room environment?

----------------------------------------------------
Outline updates
----------------------------------------------------

# Instruction

## Role
You are the Lead Language Designer and Specification Editor for the Cursive programming language. Your goal is to produce a rigorous, normative specification text that is robust enough for a "clean-room" implementation with a hostile reader (i.e., an implementer can build the compiler solely by reading this text, without reference to the reference implementation).

## Context

### Previous draft / design

`Draft2_CursiveLanguageSpecfication.md`

### Release Candidate Specification Outline

`Draft3_CursiveLanguageSpecification.md`

## Task
Read the *entire* contents of the both Draft2_CursiveLanguageSpecfication.md and Draft3_CursiveLanguageSpecification.md to deeply familiarize yourself with the previous and current designs. Then draft the full, release-ready specification content for section 4.5 such that it adheres to the style and formatting constraints of the specification writing style. 

Where Draft3 and Draft2 diverge, Draft3's design takes precedence. 

Pause after each section you generate and perform a comprehensive review of the section to check for redundant, repeated, or non-valuable content and alignment/adherence to the specification writing style. Wait for user approval to proceed to the next section of the clause.


---

carefully review your draft to check for redundant, repeated, or non-valuable content and alignment/adherence to the specification writing style.


---

<constraints>
    <rule> You must adhere to the following strict constraints for every section you generate. </rule>
    <style>
        <rule> 
        The document must follow a standard profossional style, suitable for a programming language specification. It should be declarative,    normative, and appropriately formal; following standard writing conventions for programming language specifications. 
        </rule>
    </style>
    <one_correct_place_for_content>
        <rule> All content should be defined in exactly ONE place in the specification. Before drafting
            content or defining a new concept, check if it has already been defined in another place in
            the specification. If it has, use the existing definition.
        </rule>
    </one_correct_place_for_content>
    <consistency_and_alignment>
        <rule> Before writing any section, you must read the entire specification and ensure that the
            content is consistent with the rest of the specification, and you are not introducing
            content or concepts that are not aligned with the rest of the specification. If you must
            introduce a concept not currently in the specification, you *must* get express permission
            from the user to do so.
        </rule>
    </consistency_and_alignment>
    <minimal_section_nesting>
        <rule>
            Content blocks (##### Definition, ##### Syntax & Declaration, etc.) are the ONLY permitted
            divisions within a section.

            Before creating an h4 subsection, you MUST verify that the content cannot fit into ANY of
            the standard content blocks. If the content is syntax, it belongs in Syntax & Declaration.
            If it is a typing rule, it belongs in Static Semantics.

            Create an h4 ONLY when a single section covers multiple INDEPENDENTLY USABLE constructs
            (e.g., a section on "Composite Types" covering records, enums, and tuples that each need
            their own grammar and typing rules).
        </rule>
        <subsection_creation_checklist>
            Before creating an h4 subsection, answer ALL of the following:
            1. Does this content describe syntax/grammar? → If yes, use ##### Syntax & Declaration
            2. Does this content describe typing judgments or validation? → If yes, use ##### Static
            Semantics
            3. Does this content describe runtime behavior? → If yes, use ##### Dynamic Semantics
            4. Does this content describe size/alignment/representation? → If yes, use ##### Memory &
            Layout
            5. Does this content describe errors or prohibitions? → If yes, use ##### Constraints &
            Legality

            Create an h4 ONLY if the answer to ALL five is "no" AND the parent section covers multiple
            independent language constructs requiring separate treatment.
        </subsection_creation_checklist>
    </minimal_section_nesting>
<source_structure_independence>
    <rule>
        Do NOT replicate the heading structure of the source document (Draft 2). Source section
        numbers (e.g., "{Source: Draft 2 §19.2}") are provenance references, NOT structural
        templates. Flatten source subsections into content blocks unless they meet the h4 creation
        criteria above.
    </rule>
</source_structure_independence>
<nouns_before_verbs_rule>
    <rule>
        **Define Data Before Algorithms.**
        You MUST rigorously define the Abstract State or Data Structures before describing
        operations on them.
    </rule>
    <context_application> Since this is the **Type System** clause: \- You cannot write a "Type
        Checking Algorithm" without first defining the "Type Context Structure" (e.g., $\Gamma$). \-
        You cannot write a "Subtyping Rule" without first defining the "Type Lattice" or "Relation"
        (<: ).
            \- You cannot write a "Name Resolution Rule" without defining the "Namespace" structure.
    </context_application>
</nouns_before_verbs_rule>
<normative_voice>
    <rule>
        Use RFC 2119 keywords (**MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**,
        **SHOULD**, **SHOULD NOT**, **MAY**, **OPTIONAL**) for all requirement-level statements.
    </rule>
    <rule>
        **Active Voice for Implementations:** Prefer "The implementation MUST verify..." over "It
        must be verified...".
    </rule>
    <rule>
        **Declarative Definitions:** For definitions, use plain declarative language ("A Record
        is..."). Do not use "can" or "will" for prescriptions.
    </rule>
    <rule>
        **No Weasel Words:** Forbidden terms: "basically", "simply", "usually", "conceptually", "in
        general". Rationale belongs in non-normative notes, not normative text.
    </rule>
</normative_voice>
<undefined_term_prohibition>
    <timing>
        Apply DURING drafting and verify DURING the Reverse Check phase.
    </timing>
    <rule>
        You **MUST NOT** use technical terms in normative statements without a prior or immediate
        **Definition** block.
    </rule>
    <specific_check_for_clause_4>
        Verify strict definitions for: "Covariant", "Contravariant", "Reification",
        "Monomorphization", "Witness", "Discriminant", "Layout".
    </specific_check_for_clause_4>
    <recovery>
        If you need a term not yet defined:
        1. Check if it was defined in Clauses 1-3.
        2. If not, create a **Definition** block immediately preceding its use.
    </recovery>
</undefined_term_prohibition>
<formalism>
<rule>
    **Type System Rigor:**
    You MUST use formal notation for type system rules. Do not rely solely on prose.
</rule>
<notation_standard>
- **Judgments:** $\Gamma \vdash e : T$ (Context $\Gamma$ proves expression $e$ has type $T$).
- **Subtyping:** $\Gamma \vdash A
<: B$ ($A$ is a subtype of $B$).
    - **Equivalence:** $\Gamma \vdash A \equiv B$ (Type $A$ is equivalent to $B$).
    - **Transitions:** $\sigma \to \sigma'$ (State transition).
            </notation_standard>
            <preservation_rule>
                If Draft 2 contains a formal rule (e.g., a $\LaTeX$ equation), you **MUST** preserve it. You MAY improve the notation for clarity, but you **MUST NOT** replace it with prose.
            </preservation_rule>
</formalism>
<completeness>
    <rule>
        **Maximum Detail:** Write the full specification in maximum detail, quality, and precision required to implement the language in a clean-room environment with a adversarial reader.
    </rule>
    <rule>
        **Diagnostic Mandate:**
        Every **MUST**-level failure condition (syntactic error, type error, semantic violation) **MUST** reference a specific Diagnostic Code (e.g., `E-TYP-1501`).
        - If Draft 2 provides a code, use it.
        - If Draft 2 implies a check but lists no code, generate one following the schema `E-[CAT]-[SECTION][ID]`.
    </rule>
    <rule>
        **Failure Semantics:** For every error, specify:
        1. **Detection Time:** (Compile-time, Link-time, Runtime).
        2. **Effect:** (Rejection of compilation unit, Panic, Abort).
    </rule>
</completeness>
</constraints>
<output_formatting>
<rule> For every section, you must strictly adhere to the following **Section Template**. Do not invent new headers. Use only the blocks below to divide the section into scoped content blocks. The content of each block below is given as an example of what the block may contain, but should be adjusted to the specific section and content of the section. </rule> 

<section_template_blocks>
    [### | ####] [Section Number] [Section Title]

    ##### Definition

    (Prose Definition) Explain the concept to the implementer, preserving Draft 2' s semantic nuance.
    **Formal Definition**
    Mathematically or algorithmically define the entity (e.g., "A Tuple Type $T$ is an ordered sequence of types $(t_1, \dots, t_n)$..." ).

    ##### Syntax & Declaration

    **Grammar**
    Provide the ANTLR/ EBNF production rules.
    **Desugaring**
    If this construct maps to a lower-level construct (e.g., `if` -> `match`), define the transformation here.

    ##### Static Semantics

    **Typing Rules**
    Provide the formal judgments ($\Gamma \vdash ...$).

    **Equivalence & Subtyping**
    Define when two instances of this type are equal ($T \equiv U$) or related ($T
    <: U$).

    **Validation**
    Define well-formedness checks (e.g., "Recursion limits" , "Duplicate fields" ).

    ##### Dynamic Semantics
    **Evaluation**
    Define the runtime behavior, initialization, or state transitions.

    ##### Memory & Layout

    **Representation**
    Define size `sizeof`), alignment `alignof`), and physical layout (padding, field ordering).
    **ABI Guarantees**
    Specify if the layout is **Defined**, **Implementation-Defined (IDB)**, or **Unspecified**.

    ##### Constraints & Legality

    **Negative Constraints**
    List specific forbidden usages (e.g., "Must not be used in FFI" ).
    **Diagnostic Table**
    | Code         | Severity | Condition                |
    | :----------- | :------- | :----------------------- |
    | `E-CAT-####` | Error    | Description of violation |

    ##### Examples

    Provide 1-2 short, high-clarity examples.
    - **Valid:** Shows correct usage.
    - **Invalid:** Shows code triggering specific diagnostics defined above.
</section_template>
<clause_notes>
<rule> at the end of the clause, you must write a note documenting the information that was defined, referenced, or deferred following the <clause_note_template>. 
<clause_note_template>
### Clause [NUMBER]: [TITLE] Cross-Reference Notes

**Terms defined in Clause [NUMBER] that MUST NOT be redefined elsewhere:**

| Term   | Section    | Description   |
| :----- | :--------- | :------------ |
| [TERM] | §[SECTION] | [DESCRIPTION] |


**Terms referenced:**

| Term   | Source            | Usage in Clause [NUMBER] |
| :----- | :---------------- | :----------------------- |
| [TERM] | [NUMBER]: [TITLE] | [DESCRIPTION]            |


**Terms deferred to later clauses:**

| Term   | Deferred To       | Reason        |
| :----- | :---------------- | :------------ |
| [TERM] | [NUMBER]: [TITLE] | [DESCRIPTION] |

</clause_note_template>
</clause_notes>
</output_formatting>