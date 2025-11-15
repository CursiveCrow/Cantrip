# Feature-Slice Evaluation Template

This file defines:

- A table of **feature slices** in the current Cursive specification that should go through a design/completeness/correctness + diagnostics pass.
- A reusable **LLM prompt template**. To run multiple LLMs in parallel, you:
  - Select a row from the table,
  - Fill in the “Feature Slice Context” block in the template,
  - Paste in the slice’s source text,
  - Leave the rest of the template unchanged.

---

## Feature Slices to Review (Current Specification)

These slices are chosen from the parts of the spec that already contain substantive normative text and are central to conformance, translation, and source structure.

| Completion | Feature Slice Name                             | Sections                                                    | Primary Buckets (expected)                       | Notes                                                               |
| ---------: | ---------------------------------------------- | ----------------------------------------------------------- | ------------------------------------------------ | ------------------------------------------------------------------- |
|      - [x] | Behavior Model & Conformance                   | §6, §12 (behavior classes, IFNDR, conformance, diagnostics) | `SRC-01`, `CNF-xx`, `DIA-xx`                     | Global behavior classes, conformance obligations, diag system glue. |
|      - [x] | Versioning, Evolution & Extensions             | §7 (evolution, feature flags, extensions)                   | `CNF-01..03`, `SRC-11`                           | Feature lifecycle, feature flags, extension policies.               |
|      - [-] | Source Text & Encoding                         | §8 (source, encoding, file structure, inclusion)            | `SRC-01`, `SRC-02`                               | UTF‑8, BOM, control chars, size limits, preprocessing.              |
|      - [ ] | Lexical Structure                              | §9 (tokens, identifiers, literals, comments)                | `SRC-03`, `SRC-01`                               | Token kinds, identifier rules, numeric/string/char literals.        |
|      - [ ] | Syntactic Structure & Nesting Limits           | §10 (grammar organization, limits, notation)                | `SRC-04`, `SRC-14`, `EXP-01`                     | Appendix‑A alignment, nesting limits, EOF/delimiter behavior.       |
|      - [ ] | Translation Phases & Pipeline                  | §11 (phases 1–6)                                            | `SRC-01..05`, `SRC-02`, `EXP-04`                 | Phase ordering, ownership of diagnostics, error propagation.        |
|      - [ ] | Diagnostic System Framework                    | §12 (diag format, categories, severities, IFNDR)            | `SRC-xx`, `DIA-xx`, `CNF-xx`                     | Code format, severity, IFNDR policy, taxonomy linkage.              |
|      - [ ] | Modules & Assemblies                           | §13 (modules, assemblies, manifest)                         | `SRC-11`, `SRC-13`, `SRC-14`, `SRC-15`, `SRC-16` | Module discovery, manifests, path grammar, assembly graph.          |
|      - [ ] | Imports & Name Resolution                      | §14 (imports, scopes, bindings, lookup)                     | `SRC-12`, `SRC-14`, `SRC-15`, `SRC-16`           | Import forms, aliasing, shadowing, qualified/unqualified lookup.    |
|      - [ ] | Initialization & Dependencies                  | §15 (init graph, eager/lazy edges)                          | `SRC-13`, `SRC-16`                               | Dependency graphs, eager/lazy edges, blocked modules.               |
|      - [ ] | Tooling Integration & Compiler APIs            | §16 (compiler APIs, LSP, metadata, IDE)                     | `DIA-xx`, `SRC-02`, `CNF-xx`                     | Incremental/query compilation, tooling hooks (many placeholders).   |
|      - [ ] | Type Foundations, Primitive & Composite Types  | §17–§19 (as currently written)                              | `TYP-01..02`, `MEM-01`                           | High-level type foundations; still skeletal but normative.          |
|      - [ ] | Function Types & Procedures                    | §20–§21 (as currently written)                              | `TYP-01`, `EXP-01..02`                           | Procedure typing, returns, control-flow obligations.                |
|      - [ ] | Memory Model, Regions & Permissions (overview) | §31–§36 (overview and key guarantees)                       | `MEM-01..05`, `FFI-01..04`                       | High-level memory guarantees; detailed parts pending.               |

You can add or refine rows as other parts of the spec mature.

---

## LLM Prompt Template (Single Edit Point)

> To use this template for a specific slice:
> 1. Choose a row from the table above.  
> 2. Populate the **Feature Slice Context** block with:
>    - `Feature slice name` (from the table),
>    - `Section list` (from the table),
>    - A 1–2 sentence purpose summary, and  
>    - The full text of the relevant sections.  
> 3. Leave everything after that block unchanged.

### 0. Feature Slice Context

You are evaluating exactly one feature slice of the Cursive specification in Cursive-Language-Specification.md.

```yaml

[Feature Slice]
Slice: Source Text & Encoding
Sections: §8 (source, encoding, file structure, inclusion)
Purpose: UTF‑8, BOM, control chars, size limits, preprocessing.
Expected_Buckets: SRC-01, SRC-02

```

### 1. Shared Context

You also have the following global excerpts (read them as needed; they are authoritative where applicable):

```markdown
[Behavior model: UVB / USB / IDB / IFNDR]
BEHAVIOR_MODEL_TEXT   # e.g. Part I §6.1–§6.2

[Relevant grammar]
GRAMMAR_SNIPPET_TEXT  # e.g. Appendix A + any local grammar fragments

[Diagnostic taxonomy (Appendix B)]
DIAGNOSTIC_TAXONOMY_TEXT

[Diagnostic catalog (Appendix D excerpt)]
DIAGNOSTIC_CATALOG_TEXT

[Style & process constraints for this pass]
STYLE_CONSTRAINTS
# Example constraints:
# - Do NOT change section numbers or headings.
# - New normative text MUST be in blockquotes using RFC 2119 terms.
# - Formal rules MUST be in fenced code/math blocks.
# - For this pass, do NOT add examples or rationale prose; normative + formal + diagnostics only.
```

---

### 2. Global Rules You MUST Follow

- **Behavior model**
  - Do NOT introduce any notion of “undefined behavior” beyond UVB / USB / IDB / IFNDR as defined in the behavior‑model excerpt.
  - Any new rule MUST classify behavior consistently with that model.

- **Diagnostics**
  - Codes MUST follow `K-CAT-FFNN` (e.g. `E-SRC-0304`).
  - Buckets and categories MUST match the diagnostic taxonomy.
  - Do NOT mint arbitrary new concrete codes.  
    - If a new diagnostic is clearly needed, you MAY propose:
      - A bucket (e.g. `SRC-03`),
      - Severity (`E`, `W`, or `N`),
      - A short title/description,
      - A placeholder code like `E-SRC-03??` (editor will assign real digits).

- **Grammar**
  - Appendix A is canonical for syntax; do NOT change precedence or associativity.
  - Any grammar fragment you suggest MUST be consistent “surface views” of the canonical grammar.

- **Document structure**
  - Do NOT renumber or rename sections.
  - All proposals MUST be phrased as text to insert/adjust under the existing headings of this slice.

---

### 3. Tasks (Apply ALL to the Feature Slice in the Context Block)

#### 3.1 Slice understanding

- Briefly summarize:
  - What this feature slice is specifying.
  - Which behavior classes (UVB / USB / IDB / IFNDR) and which diagnostic buckets it currently uses or obviously should use.

#### 3.2 Design evaluation (vs. Cursive goals)

For the feature slice:

- Evaluate how well it aligns with Cursive’s goals (one‑correct‑way, const‑by‑default, incorrectness‑is‑an‑error, LPS/regions safety, deterministic performance, local reasoning, zero‑cost abstractions).
- Classify key design aspects into:
  - “Design‑good: … (reason)”
  - “Subtle but acceptable: … (risk/concern)”
  - “Problematic: … (conflict with goals + suggested direction: tighten / reframe / mark as open design question)”

#### 3.3 Completeness evaluation

- Enumerate the observable behaviors, states, and interactions covered by this slice:
  - Inputs, state transitions, error cases, interactions with other phases (lexing, parsing, modules, types, etc.) if relevant.
- For each:
  - Identify which existing normative requirement(s) govern it.
  - If none govern it, mark a **completeness gap** and classify as:
    - “Critical gap” – behavior is underspecified or contradicts the behavior model.
    - “Non‑critical gap” – missing detail but behavior is still reasonably constrained.
- For gaps:
  - Either recommend a new normative requirement (see 3.5), or
  - Explicitly argue that it should be USB or IDB, and check consistency with the behavior‑model excerpt (and, for IDB, with Appendix C if relevant).

#### 3.4 Correctness & consistency evaluation

Check three levels:

1. **Local correctness (per requirement)**  
   - Is each requirement clear, checkable, and internally coherent?

2. **Internal consistency (within this slice)**  
   - Do any two requirements contradict each other, overlap ambiguously, or assign different behavior classes/diagnostics to the same situation?

3. **Global consistency**  
   - Are behavior classifications consistent with the behavior model?
   - Are diagnostics mapped to plausible buckets and severities given the taxonomy?
   - Is there any clash with the pipeline, conformance, or grammar rules given in the shared context?

Label issues as:

- “Conflict: …”
- “Ambiguity: …”
- “Over‑specification: …” / “Under‑specification: …”

and explain why.

#### 3.5 Proposed normative changes (text only)

For this slice, and ONLY where needed to address design/completeness/correctness issues:

- Propose **minimal** changes, using only:
  - New normative requirements (blockquotes with RFC 2119 keywords),
  - Tightening or clarifying existing normative sentences (only where necessary),
  - Explicit behavior‑class statements (e.g. “this is USB” / “this is IDB / IFNDR”).
- Constraints:
  - All new or revised requirements MUST be in Markdown blockquotes (`>`).
  - RFC 2119 terms MUST be ALL CAPS.
  - Grammar or formal templates MUST be in fenced code/math blocks, not inline prose.
  - Do NOT add examples or rationale unless explicitly allowed in the style constraints.
- For each proposed change, specify:
  - `Location: §X.Y.Z [anchor_if_available]`
  - Then provide the exact snippet to insert/replace.

#### 3.6 Formal inference / judgment rules

Where appropriate:

- Add or refine formal rules (typing, well‑formedness, translation‑pipeline, or behavior rules) that correspond directly to the requirements you just evaluated.
- Each rule MUST:
  - Use the judgment and metavariable conventions from the notation section (e.g. `Γ ⊢`, `σ`, `e`, etc.).
  - Align precisely with the stated requirements (no hidden extra obligations).
- Present each rule inside a fenced block (text or LaTeX‑style), for example:

  ```text
  Γ ⊢ unit ⇓ₖ (stateₖ, Dₖ)
  -------------------------------
  Γ ⊢ unit ⇓ (stateₖ, Dₖ)
  ```

- If a rule cannot be made precise because some prerequisite is missing, mark it as a **TODO rule** and briefly state what background is missing.

#### 3.7 Diagnostic coverage mapping

- Build a mapping from **violation conditions** in this slice to diagnostics:
  - For each distinct violation condition:
    - State the condition.
    - Map it to one or more diagnostics:
      - Existing code(s) from the catalog excerpt, if appropriate; OR
      - A proposed new diagnostic concept with:
        - Bucket (e.g. `SRC-03`),
        - Severity (`E` / `W` / `N`),
        - Placeholder code like `E-SRC-03??`,
        - One‑line summary,
        - Justification for the chosen bucket/severity.
- Ensure:
  - Every requirement that says something MUST be diagnosed or rejected has at least one mapped diagnostic.
  - No mapping contradicts the diagnostic taxonomy or behavior model.

---

### 4. Output Format

Return your findings in this structure:

1. `**1. Slice Summary**`
   - 2–5 bullets summarizing what this slice specifies and which behavior classes / diagnostic buckets it uses.

2. `**2. Design Evaluation**`
   - Grouped bullets under:
     - “Design‑good: …”
     - “Subtle but acceptable: …”
     - “Problematic: …”

3. `**3. Completeness Evaluation**`
   - `3.1 Behaviors covered` – bullets listing well‑covered behaviors.
   - `3.2 Gaps` – bullets, each labeled “Critical gap” or “Non‑critical gap”, with section references.

4. `**4. Correctness & Consistency**`
   - Bullets for:
     - Local correctness issues.
     - Internal conflicts/ambiguities.
     - Global consistency issues (behavior model, diagnostics, grammar).

5. `**5. Proposed Normative Changes (Text)**`
   - For each change:
     - `Location: §…`
     - A fenced block with the exact proposed Markdown snippet.

6. `**6. Proposed Formal Rules**`
   - Fenced blocks with rules + 1–2 bullets explaining each rule’s intent and how it ties to specific requirements.

7. `**7. Diagnostic Mapping**`
   - A small table or bullet list of:
     - Violation condition → bucket/code (existing or placeholder) → justification.

8. `**8. Open Questions / TODOs**`
   - Any unresolved design questions, ambiguous areas, or things that require editor input or global decisions.

Always stay within the feature slice given in the **Feature Slice Context** block. Do not assume or modify other parts of the spec.

