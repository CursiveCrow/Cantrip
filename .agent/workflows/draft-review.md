---
description: Review a written draft section.
---

**SELF-CORRECTION & QUALITY CONTROL PROTOCOL**

**Stop.** Do not proceed to the next task yet. You must now perform a **Mandatory Quality Assurance Pass** on the content you just generated in the previous turn.

You are the Lead Specification Editor. Your reputation depends on "Spec-Quality" rigor, not "Summary-Quality" overview.

**QA Checklist:**

1.  **Archetype Integrity:**
    *   Identify the specific **Content Archetype** tag required for this section (from `Draft3Outline.md`).
    *   Did you output the **exact** template headers for that archetype?
    *   Did you apply the **Selection Logic** correctly? (i.e., Did you *omit* sections where the answer was "No", and *populate* mandatory sections?)
    *   *Pass Condition:* The structure matches the template exactly.

2.  **Formalism & Notation:**
    *   **Logic:** Did you use $LaTeX$ syntax ($\forall, \exists, \in, \implies, \equiv$) to define invariants and type rules? (Prose explanations of logic are **insufficient**).
    *   **Grammar:** Is the syntax defined in `code blocks` using Template-Style W3C EBNF?
    *   **Diagnostics:** Did you include a **Diagnostic Table** (Code | Severity | Description) for every error condition mentioned? (Textual descriptions of errors without codes are **unacceptable**).

3.  **Normative Voice (RFC 2119):**
    *   Scan your text for weak phrases: "should", "can", "might", "is expected to".
    *   *Pass Condition:* These must be replaced with **MUST**, **MUST NOT**, **MAY**, **OPTIONAL**.
    *   *Pass Condition:* "The compiler" must be replaced with "The implementation".

4.  **The "Anti-Summarization" Test:**
    *   Look at the source text you migrated. Did you just reword it?
    *   *Pass Condition:* You must have **EXPANDED** the content.
        *   *Example:* If source says "Checks types", you must specify *when* (Phase 3), *how* (unification), and *what* happens on failure (Diagnostic E-TYP-xxxx).
    *   **Systems Depth:** Did you fill in the logical gaps regarding ABI, Memory Layout, and Linker symbols that strict specification requires?

5.  **Source Fidelity & Correctness:**
    *   **Verification:** Cross-reference your output against the specific source text from `Cursive-Language-Specification.md` (as mapped in the Outline).
    *   **No Data Loss:** Did you preserve every specific value, flag name, constraint, and behavior defined in the original?
    *   **No Design Drift:** Did you accidentally alter a core design decision (e.g., changing a static check to a runtime check, or altering the permission lattice)?
    *   *Pass Condition:* The formalization must expand the *rigor* but strictly adhere to the *design intent* of the original.

6.  **"Cursive Is Not Rust" Check:**
    *   Scan for forbidden Rust terminology: `impl` blocks, lifetimes (`'a`), `Box`, `Rc`, `Arc`, `Option`, `Result`, `match` (if used like Rust), `borrow checker`.
    *   *Pass Condition:* Ensure the semantics describe **Cursive** (Static-by-default, Region-based or Manual memory, O-Caps).

**Action Required:**

*   **IF** you find **ANY** critical flaws (especially missing Logic, missing Diagnostic tables, or weak "summarization"):
    *   State: *"Correction required: [List failures]. Regenerating section..."*
    *   **REWRITE** the entire section immediately with the corrections applied.
*   **IF AND ONLY IF** the content is flawless:
    *   Output: *"QA PASS: Content is strictly compliant with Draft 3 ISO/IEC directives."*
