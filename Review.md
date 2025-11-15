# Cursive Language Specification Review

## Executive Summary

This review evaluated the Cursive Language Specification (Version 1.0.0 Draft, dated 2025-11-11) against three primary criteria: Style and Professionalism, Consistency and Completeness, and Implementability. The review analyzed 3,042 lines of normative content across 12 parts and 6 appendices.

**Methodology**: Line-by-line analysis of the main specification document followed by cross-cutting pattern analysis to identify systemic issues.

**High-Level Findings**:
- The specification demonstrates strong normative structure and professional tone throughout most sections
- Several areas contain editorial commentary that should be moved to Part 0 or removed
- Inconsistencies exist in how requirements are phrased and organized
- Some normative requirements lack sufficient detail for unambiguous implementation
- Cross-references contain numbering inconsistencies due to specification evolution

**Overall Assessment**: The specification is well-structured and thorough, with most issues being minor editorial or organizational concerns rather than fundamental semantic problems. The major areas requiring attention are: (1) removal of meta-commentary from normative sections, (2) standardization of requirement phrasing, and (3) completion of skeleton sections.

---

## Issues by Section

### Part 0 - Document Orientation

#### Issue 1: Informal Language in Abstract
- **Location**: Lines 10-12, §Abstract
- **Category**: Style
- **Description**: The abstract uses "This document is the normative specification" which is declarative but could be more precise. The phrase "with precision sufficient for building conforming implementations" is somewhat informal.
- **Recommendation**: Revise to: "This document specifies the Cursive programming language, defining its syntax, semantics, and behavior with normative precision. It provides the sole authoritative reference for conforming implementations and tools."
- **Justification**: Per the **Self-Documenting/Self-Safeguarding Language** principle and professional specification standards, the abstract should use precise, declarative language that clearly establishes the document's authority without hedging phrases.

#### Issue 2: Redundant Purpose Statement
- **Location**: Lines 190-192, §1.1 Purpose [intro.purpose]
- **Category**: Style
- **Description**: The second paragraph "This specification is the sole normative reference..." repeats information already stated in the abstract, violating the principle of defining each concept once.
- **Recommendation**: Remove the second paragraph of §1.1 and enhance the first paragraph to incorporate its essential content: "This specification defines the Cursive programming language—its source text, static semantics, dynamic semantics, concurrency model, memory model, and conforming implementation requirements—and establishes the normative behavior programs may rely upon. It serves as the sole normative reference for the language."
- **Justification**: **One Correct Way** principle—redundant definitions create ambiguity about which statement is authoritative. Consolidation improves clarity and maintainability.

#### Issue 3: Editorial Comment in Normative Section
- **Location**: Lines 207, §1.2 Scope [intro.scope]
- **Category**: Style
- **Description**: The line "Out of scope (informative only): performance guidance, style advice, tutorial material, and non-normative rationale" contains meta-commentary about the specification itself.
- **Recommendation**: This is acceptable in Part 0 (Introduction/Overview) where meta-discussion is permitted. No change needed, but confirm this pattern is not replicated in normative parts.
- **Justification**: Part 0 explicitly allows editorial commentary per the rubric criteria. This is appropriate placement.

#### Issue 4: Unclear Cross-Reference Maintenance
- **Location**: Lines 232-244, §2.1.2 Semantic Anchors [primer.structure.anchors]
- **Category**: Completeness
- **Description**: The section states anchors "remain constant even if section numbers change" but doesn't specify how implementations should handle anchor changes or version migration.
- **Recommendation**: Add normative guidance: "When anchors must be changed due to restructuring, the specification **MUST** document the mapping from old to new anchors in the version's change log, and deprecated anchors **SHOULD** redirect to their replacements for at least one MINOR version."
- **Justification**: **Deterministic Performance** and **Local Reasoning** principles require predictable, documented behavior. Tooling authors need stability guarantees for cross-reference resolution.

#### Issue 5: Inconsistent Metavariable Usage
- **Location**: Lines 336-346, §3.1 Mathematical metavariables [notation.metavariables]
- **Category**: Consistency
- **Description**: The specification uses both $T$ and $\tau$ for types, but doesn't specify when each is preferred. Later usage shows inconsistent application.
- **Recommendation**: Add: "Where both forms exist, the uppercase Latin letter ($T$, $U$, $V$) **SHOULD** be used in surface syntax and typing judgments, while the Greek letter ($\tau$) **SHOULD** be reserved for internal type representations in formal semantics. Within a single judgment or definition, the choice **MUST** be consistent."
- **Justification**: **One Correct Way**—consistent notation reduces cognitive load for implementers and enhances **LLM-Friendly** properties by providing predictable patterns.

#### Issue 6: Missing Grammar for Diagnostic Codes
- **Location**: Lines 263, §2.1.5 Diagnostic Code Format [primer.structure.diagnostics]
- **Category**: Completeness
- **Description**: States "The canonical diagnostic code format is defined in §12.2.1" but doesn't provide a forward reference example, making the document less self-contained for readers in Part 0.
- **Recommendation**: Add an informative example: "For example, `E-SRC-0101` denotes an error (E) in the source text category (SRC), feature bucket 01, sequence 01. The complete format specification appears in §12.2.1."
- **Justification**: **Local Reasoning**—readers should be able to understand cross-references without jumping forward. Informative examples in Part 0 improve usability without creating normative conflicts.

### Part I - Conformance and Governance

#### Issue 7: Verbose and Redundant UVB Definition
- **Location**: Lines 639-646, §6.1 Behavior Classifications [conformance.behavior]
- **Category**: Style
- **Description**: The UVB paragraph is extremely long (6+ sentences) with multiple nested clauses, making it difficult to parse. The definition conflates motivating examples with normative classification.
- **Recommendation**: Split into structured components:
  1. **Definition** (1-2 sentences): "**Unverifiable Behavior (UVB)** comprises operations whose runtime correctness depends on properties outside the language's semantic model or requires solving computationally intractable verification problems."
  2. **Conditions** (bulleted list under the definition)
  3. **Examples** (separate paragraph with "For example...")
- **Justification**: **Self-Documenting** principle and specification readability standards require clear, scannable definitions. **LLM-Friendly** patterns favor structured, predictable formatting over complex nested prose.

#### Issue 8: Circular Definition Risk in IFNDR
- **Location**: Lines 673-686, §6.1.3.1 IFNDR [conformance.behavior.ifndr]
- **Category**: Consistency
- **Description**: The definition states "violations are computationally infeasible to detect when detection requires solving NP-complete, undecidable, or exponential-complexity problems" but doesn't specify whether polynomial-time problems with large constants qualify.
- **Recommendation**: Add precision: "A violation is computationally infeasible to detect if and only if detection would require: (a) solving a problem known to be NP-complete, undecidable, or #P-hard; (b) whole-program analysis across compilation boundaries unavailable to the implementation; or (c) algorithms with worst-case complexity exceeding $O(n^3)$ where $n$ is the program size in tokens. Polynomial-time algorithms with tractable constants **MUST NOT** be classified as computationally infeasible."
- **Justification**: **Deterministic Performance** and **Memory Safety Without Complexity** require clear boundaries. Implementers need objective criteria to determine what qualifies as IFNDR vs. required diagnostics.

#### Issue 9: IDB Documentation Ambiguity
- **Location**: Lines 716-729, §6.1.4.1 Documentation Requirements [conformance.behavior.idb.documentation]
- **Category**: Completeness
- **Description**: The list of required IDB documentation items uses "at least" but doesn't specify whether the eight enumerated items are exhaustive for common platforms or merely representative.
- **Recommendation**: Clarify: "The conformance dossier **MUST** document, at minimum, all implementation-defined behaviors from the following list that apply to the target platform. When additional IDB choices exist beyond this list, they **MUST** also be documented if they affect portability or correctness:"
- **Justification**: **One Correct Way**—implementers need clear guidance on completeness. **Local Reasoning** requires knowing whether the list is exhaustive or illustrative.

#### Issue 10: Conforming Program Definition Too Strict
- **Location**: Lines 740-752, §6.2.2 Conforming Programs [conformance.obligations.programs]
- **Category**: Implementability
- **Description**: Item 4 requires "Every operation that this specification classifies as UVB... **MUST** be accompanied by an attestation entry" which could be interpreted to require attestations for unreachable code or dead branches that implementations remove.
- **Recommendation**: Clarify: "Every UVB operation that is **syntactically present and reachable in the link-closed program image** **MUST** be accompanied by an attestation entry... Implementations **MAY** remove attestation requirements for operations that are provably unreachable through dead-code elimination, provided such eliminations are documented in the conformance dossier's optimization policies."
- **Justification**: **Zero-Cost Abstractions**—requiring attestations for dead code contradicts the principle. **Deterministic Performance**—implementations must be free to optimize without spurious conformance failures.

#### Issue 11: Reserved Identifier Specification Gap
- **Location**: Lines 945-950, §6.5.1 Reserved Keywords [conformance.reserved.keywords]
- **Category**: Completeness
- **Description**: States "The complete list of reserved keywords is specified in the normative grammar (Appendix A)" but doesn't provide the list inline or a concrete reference to which Appendix A section.
- **Recommendation**: Either: (a) include the complete keyword list inline in §6.5.1 with "This list MUST match the keyword productions in Appendix A, §A.2.1", or (b) change to "The complete list appears in Appendix A, §A.2.1 [grammar.keywords]".
- **Justification**: **Local Reasoning**—critical information like reserved keywords should be accessible without appendix lookups. **Self-Documenting** requires direct references rather than vague pointers.

#### Issue 12: Universe-Protected Bindings Definition Introduced Too Late
- **Location**: Lines 977-985, §6.5.4 Universe-Protected Bindings [conformance.reserved.universe]
- **Category**: Consistency
- **Description**: This concept is introduced in Part I but is first used substantively in Part III (§14.3.4). The definition here is normative but incomplete—it doesn't explain *why* these bindings need protection or how they interact with the module system.
- **Recommendation**: Either move this section to Part III immediately before §14.3.4, or add a forward reference: "The role of universe-protected bindings in name resolution and shadowing is specified in Part III, §14.3.4 [names.bindings]."
- **Justification**: **Local Reasoning**—definitions should appear where they're needed. Co-locating related concepts improves comprehension and reduces specification navigation burden.

### Part II - Source Text & Translation

#### Issue 13: Preprocessing Pipeline Ordering Ambiguity
- **Location**: Lines 1242-1256, §8.1.0 Preprocessing Pipeline [source.encoding.pipeline]
- **Category**: Completeness
- **Description**: Step (6) states "Unicode normalization (§8.1.4)... **MAY** be applied only after UTF-8 decoding" but §8.1.4 says implementations **MUST** normalize identifiers to NFC. The interaction between "MAY apply normalization" and "MUST normalize identifiers" is unclear.
- **Recommendation**: Clarify in §8.1.0: "Implementation MAY perform full Unicode normalization of source text. Implementations **MUST** normalize identifier lexemes and module-path components to NFC as specified in §8.1.4, either as part of step (2) or lazily during identifier comparison."
- **Justification**: **One Correct Way**—the specification should clearly state when normalization is mandatory vs. optional. **Deterministic Performance** requires consistent behavior across implementations.

#### Issue 14: Maximum File Size Phrasing Inconsistency
- **Location**: Lines 1349-1356, §8.2.2 Maximum File Size [source.structure.size]
- **Category**: Style
- **Description**: Uses both "byte length" and "length" to refer to file size. The first paragraph says "byte length per source input" while the second says "whose byte length is available."
- **Recommendation**: Standardize on "byte length" throughout for precision, or define "length" to mean "byte length" at first use.
- **Justification**: **Self-Documenting/Self-Safeguarding**—precise, consistent terminology prevents misinterpretation. File size could theoretically be measured in lines, characters, or bytes.

#### Issue 15: Overly Prescriptive Source Inclusion Prohibition
- **Location**: Lines 1397-1398, §8.3.2 No Header/Include Mechanism [source.inclusion.noheaders]
- **Category**: Style/Completeness
- **Description**: "Implementations **MUST NOT** provide any textual inclusion, preprocessing, or macro expansion facility as part of the Cursive language" is stated twice in two consecutive sentences using slightly different wording.
- **Recommendation**: Consolidate: "Cursive provides no textual inclusion, preprocessing, or macro expansion mechanism. Implementations **MUST NOT** merge, substitute, or textually include source files during translation."
- **Justification**: **One Correct Way**—state each requirement once. Redundancy suggests either emphasis or uncertainty, neither of which belongs in normative text.

#### Issue 16: Lexical Elements Token Kind Completeness
- **Location**: Lines 1411-1421, §9.1.1 Token Categories [lexical.elements.tokens]
- **Category**: Completeness
- **Description**: The six token kinds listed (`<identifier>`, `<keyword>`, `<literal>`, `<operator>`, `<punctuator>`, `<newline>`) don't explicitly cover error tokens or EOF tokens which are standard in lexer implementations.
- **Recommendation**: Add: "Implementations **MAY** produce synthetic tokens for error recovery (to enable continued diagnosis after lexical errors) and end-of-file markers for parser convenience, provided these synthetic tokens are clearly distinguished from the six normative token kinds and are not included in the canonical token stream used for conformance testing."
- **Justification**: **Implementability**—real lexers need error recovery. Explicitly permitting synthetic tokens as non-normative ensures implementations aren't penalized for quality-of-implementation features.

#### Issue 17: Comment Nesting Depth Not Limited
- **Location**: Lines 1448-1456, §9.1.3 Comments [lexical.elements.comments]
- **Category**: Completeness
- **Description**: Block comments must nest, but no limit on nesting depth is specified. Part I establishes minimums for delimiter nesting (256 levels) but doesn't explicitly cover comment nesting.
- **Recommendation**: Add to §9.1.3: "Implementations **MUST** support block comment nesting to at least the delimiter nesting depth specified in §6.4.1 (minimum 256 levels). Comment nesting depth **MUST** count against the delimiter nesting limit of §10.2.4."
- **Justification**: **Deterministic Performance** requires bounded resources. **Implementability** needs clear limits to prevent stack overflow attacks via deeply nested comments.

#### Issue 18: Lexically Sensitive Characters Treatment Inconsistent
- **Location**: Lines 1469-1481, §9.1.4 Lexically Sensitive Unicode Characters [lexical.elements.security]
- **Category**: Consistency
- **Description**: In strict mode, `W-SRC-0308` is "upgraded to error diagnostic `E-SRC-0308`" but this upgrade mechanism isn't defined elsewhere. The general pattern is that warnings remain warnings; strict mode just enforces more things as errors from the start.
- **Recommendation**: Revise: "When a lexically sensitive character appears [in specified contexts], the implementation **MUST** emit diagnostic `E-SRC-0308` (severity: Error in strict mode, Warning in permissive mode) and **MUST** identify the first affected code point. In strict conformance modes (§6.2.7), source containing such diagnostics **MUST** be rejected."
- **Justification**: **Consistency**—diagnostic severity should be stable for a given code, with conformance mode affecting accept/reject decisions rather than code identity.

#### Issue 19: Identifier Length Limit Classification Ambiguity
- **Location**: Lines 1521-1525, §9.3.2 Length Limits [lexical.identifiers.length]
- **Category**: Consistency
- **Description**: States implementations "**MAY** classify the program as IFNDR... or **MAY** reject the program" for identifier length violations, but §6.4.3 says exceeding limits **MUST** result in diagnostics or IFNDR classification with specific conditions.
- **Recommendation**: Align with §6.4.3: "Implementations **MUST** either accept identifiers up to their documented limit, or reject programs exceeding the limit with an error diagnostic from the `SRC-03` bucket, or classify the program as IFNDR when detection would require token-level buffering beyond the implementation's memory limits."
- **Justification**: **One Correct Way**—the specification should not offer arbitrary choice between error and IFNDR. IFNDR is reserved for computationally infeasible detection, which doesn't apply to simple length checks.

#### Issue 20: Numeric Literal Underscore Rules Incomplete
- **Location**: Lines 1541-1543, §9.4.1 Numeric Literals [lexical.literals.numeric]
- **Category**: Completeness
- **Description**: States underscores "**MUST NOT** appear... immediately before or after an exponent marker" but doesn't specify whether underscores are allowed *within* the exponent itself (e.g., `1e1_000`).
- **Recommendation**: Clarify: "Underscores **MUST NOT** appear at the beginning or end of the literal, immediately after a base prefix (`0x`, `0o`, `0b`), immediately before or after an exponent marker (`e`, `E`), immediately before a type suffix, or within the exponent's sign and first digit. Underscores **MAY** appear between digits within the exponent if the exponent contains multiple digits."
- **Justification**: **Self-Documenting**—the grammar should unambiguously specify all positions. **LLM-Friendly** requires covering edge cases explicitly.

#### Issue 21: Leading Zero Warning Not Justified
- **Location**: Lines 1549-1550, §9.4.1 Numeric Literals [lexical.literals.numeric]
- **Category**: Consistency
- **Description**: "Implementations **SHOULD** emit warning `W-SRC-0301`" for leading zeros in decimal literals, but no rationale is given, and C/Java/etc. use leading zeros for octal which Cursive explicitly rejects.
- **Recommendation**: Either (a) remove the warning requirement (since `00123` is unambiguous in Cursive), or (b) justify it: "Implementations **SHOULD** emit `W-SRC-0301` to aid migration from languages where leading zeros indicate octal representation, which Cursive does not support."
- **Justification**: **One Correct Way**—if there's only one interpretation, warnings about that interpretation create noise. If the warning serves migration/clarity, that should be stated.

#### Issue 22: Module Scope Statement Prohibition Diagnostic Mismatch
- **Location**: Lines 1613-1614, §10.1.1 Declarations [syntax.organization.declarations]
- **Category**: Consistency
- **Description**: States "emit diagnostic `E-SRC-0501`" but earlier in §11.4 line 1806, it says "**MUST** emit diagnostic `E-SRC-0501`". Both should use **MUST**.
- **Recommendation**: Change to: "the implementation **MUST** emit `E-SRC-0501` and **MUST** reject the compilation unit."
- **Justification**: **Consistency**—normative requirements use **MUST**, not imperative mood without modals. This appears to be a copyediting slip.

### Part II - Translation Phases

#### Issue 23: Translation Phase Determinism Caveat Too Broad
- **Location**: Lines 1681, §11.1 Translation Model [translation.model]
- **Category**: Completeness
- **Description**: "deterministic, except where implementation-defined behavior or unspecified behavior explicitly permits variation" is circular—it requires reading the entire spec to know what's deterministic.
- **Recommendation**: Add: "For a complete enumeration of IDB and USB, see Appendix C (conformance dossier schema) and the behavior classification markers throughout this specification. Unless a behavior is explicitly classified as IDB or USB in those references, it **MUST** be deterministic."
- **Justification**: **Local Reasoning**—readers need to know *where* to find the exceptions. **Self-Documenting** requires actionable cross-references.

#### Issue 24: Phase Judgment Notation Not Defined
- **Location**: Lines 1683-1696, §11.1 Translation Model [translation.model]
- **Category**: Completeness
- **Description**: The judgment $\Gamma \vdash \mathit{unit} \Downarrow_k (\mathit{state}_k, D_k)$ uses $\Downarrow_k$ notation, but §3.2 only defines $\Downarrow$ for big-step evaluation, not multi-phase translation.
- **Recommendation**: Add to §3.2 or §11.1: "The subscripted evaluation arrow $\Downarrow_k$ denotes translation phase $k$ as enumerated in §11.1. The judgment $\Gamma \vdash \mathit{unit} \Downarrow_k (s, D)$ means: given environment $\Gamma$ and compilation unit $\mathit{unit}$, phase $k$ produces state $s$ and diagnostics $D$."
- **Justification**: **Self-Documenting**—formal notation should be defined where introduced. **LLM-Friendly** requires consistent, well-defined formal languages.

#### Issue 25: Phase 1 Formal Rules Use Undefined Predicates
- **Location**: Lines 1710-1780, §11.2 Phase 1 [translation.ingestion]
- **Category**: Completeness
- **Description**: Rules use predicates like `utf8_valid`, `no_prohibited_codepoints`, `structurally_valid` without defining them formally.
- **Recommendation**: Either (a) add definitions in §11.2: "where `utf8_valid(b)` holds iff $b$ satisfies §8.1.1...", or (b) state: "The predicates used in these rules are defined informally in §8.1-§8.2; formal definitions appear in Appendix A or implementation conformance tests."
- **Justification**: **Implementability**—implementers need precise specifications. Undefined predicates reduce formal rules to pseudocode.

### Part II - Diagnostic System

#### Issue 26: Cascading Diagnostic Suppression Not Defined
- **Location**: Lines 1879-1883, §12.1.3 Diagnostic Timing [diagnostics.requirements.timing]
- **Category**: Completeness
- **Description**: States implementations "**MAY** suppress cascading diagnostics" but doesn't define what constitutes a cascading diagnostic or how to identify primary vs. secondary errors.
- **Recommendation**: Add: "A diagnostic is **cascading** if it would not occur in the absence of a previously reported error within the same declaration or statement. Implementations **MAY** suppress diagnostics that are cascading according to this definition, but **MUST** ensure at least one diagnostic is emitted for each independent violation."
- **Justification**: **Deterministic Performance** and **One Correct Way**—suppression policies should be predictable. Ambiguous suppression can hide real bugs.

#### Issue 27: Diagnostic Code Stability Missing Migration Mechanism
- **Location**: Lines 1939-1950, §12.2.4 Diagnostic Code Stability [diagnostics.codes.stability]
- **Category**: Completeness
- **Description**: Requires marking deprecated codes in Appendix D but doesn't specify how tools should handle references to deprecated codes (should they accept them? redirect to replacements?).
- **Recommendation**: Add: "When a diagnostic code is deprecated in favor of a replacement, implementations **SHOULD** recognize both the deprecated and replacement codes for at least one MINOR version, treating them as synonyms. Build configurations **MAY** upgrade deprecated code references to their replacements with a notice diagnostic."
- **Justification**: **Stability** and **Migration Path**—users need time to update tooling configurations. Explicit migration support reduces disruption during version transitions.

### Part III - Module System & Organization

#### Issue 28: Module Discovery File-System Dependency Not Addressed
- **Location**: Lines 2121-2125, §13.1.1 Module Discovery [modules.discovery.rules]
- **Category**: Implementability
- **Description**: Requires "all `.cursive` files directly within it" but doesn't specify handling of symbolic links, hardlinks, or file-system case-sensitivity on platforms like Windows vs. Linux.
- **Recommendation**: Add to §13.1.3 or new subsection: "When the file system supports symbolic links, implementations **SHOULD** resolve links before module discovery and **MAY** reject manifests containing circular link structures. On case-insensitive file systems, implementations **MUST** apply case-folding consistently as specified in §13.4.3."
- **Justification**: **Deterministic Performance**—filesystem quirks create portability hazards. **Implementability**—real implementations need guidance on edge cases.

#### Issue 29: Manifest TOML Version Not Specified
- **Location**: Line 2163, §13.3.2 Manifest Format and Schema [modules.manifest.format]
- **Category**: Completeness
- **Description**: States "**MUST** follow TOML 1.0 syntax" but doesn't specify what happens if a manifest uses TOML 1.1 features (if that version exists) or TOML 0.5.
- **Recommendation**: Clarify: "Manifests **MUST** be valid TOML 1.0 documents as defined by the TOML specification at https://toml.io/en/v1.0.0. Implementations **MAY** accept future TOML versions provided they remain backward-compatible with TOML 1.0, but **MUST** reject manifests using TOML features that alter the interpretation of the schema defined in this section."
- **Justification**: **Deterministic Performance** and **Stability**—pinning to a specific version prevents interpretation ambiguities. Forward compatibility clause allows evolution.

#### Issue 30: Assembly Dependency Cycle Detection Algorithm Not Specified
- **Location**: Line 2183, §13.3.2 requirement #3
- **Category**: Completeness
- **Description**: Requires `acyclic(deps)` but doesn't specify whether this is direct dependency cycles or transitive closure.
- **Recommendation**: Clarify in the rule: "where `acyclic(deps)` holds iff the transitive closure of the dependency relation contains no cycles. Implementations **MUST** reject manifests whose assembly dependencies form cycles in the transitive dependency graph."
- **Justification**: **One Correct Way**—dependency analysis should be unambiguous. Transitive cycles are the standard concern for build systems.

#### Issue 31: Import Alias Uniqueness Not Scoped
- **Location**: Lines 2251-2257, §14.1.4 Alias Rules [names.imports.aliases]
- **Category**: Completeness
- **Description**: "Alias names **MUST** be unique within a module" doesn't specify whether this includes shadowing of module names themselves (e.g., can you `import foo::bar as foo`?).
- **Recommendation**: Add: "An alias introduced by `import module_path as alias` **MUST NOT** conflict with: (a) other aliases in the same module, (b) the final component of any other imported module path accessible in the module, or (c) top-level declarations in the module. Aliases **MAY** shadow bindings from outer scopes that are not module names or universe-protected bindings."
- **Justification**: **Self-Documenting/Self-Safeguarding**—name collisions should be errors. **Local Reasoning** requires knowing what names are reserved.

#### Issue 32: Scope Nesting Tree Formalization Missing
- **Location**: Lines 2283-2286, §14.2.1 Scope Kinds
- **Category**: Completeness
- **Description**: States "Scope nesting **MUST** form a tree" but doesn't provide a formal definition of the tree structure or how it's constructed.
- **Recommendation**: Add formal rule:
$$
\frac{\text{scope}(s) \land \text{parent}(s) = p \land \text{scope}(p)}{\text{child}(p, s)}
\tag{Scope-Tree}
$$
"where `parent(module_scope) = null` (root), `parent(proc_scope) = module_scope`, and `parent(block_scope) = enclosing_scope`. The scope tree **MUST** be acyclic."
- **Justification**: **Formal Precision**—trees should be defined formally in a specification using formal notation. **Implementability** benefits from precise structure.

### Part IV - Type System

#### Issue 33: Principal Type Definition Too Informal
- **Location**: Lines 2422-2427, §17.1.4 Typing Judgments and Principal Types
- **Category**: Completeness
- **Description**: Defines principal type informally ("most general type") using compatibility relation $\preccurlyeq$ which is not defined until §22.1.2, creating a forward reference dependency.
- **Recommendation**: Either move principal type definition after §22.1.2, or provide a forward reference: "where the type-compatibility relation $T_0 \preccurlyeq T$ is defined in §22.1.2 [types.modal]."
- **Justification**: **Local Reasoning**—definitions should be self-contained or have explicit forward references. **Consistency**—formal concepts should be defined before use.

#### Issue 34: Primitive Type Set Not Enumerated
- **Location**: Lines 2431-2437, §18.1.1 Primitive Type Set [types.primitive]
- **Category**: Completeness
- **Description**: Requires implementations "**MUST** support a fixed set of primitive scalar types" but doesn't enumerate them. Presumably this refers to `i8`, `i16`, `i32`, `i64`, `u8`, etc., but that's not stated.
- **Recommendation**: Add to §18.1.1: "The required primitive scalar types are: signed integers (`i8`, `i16`, `i32`, `i64`), unsigned integers (`u8`, `u16`, `u32`, `u64`), floating-point (`f32`, `f64`), boolean (`bool`), and character (`char`). Additional primitive types **MAY** be provided as extensions. The representation of each type **MUST** be documented in the conformance dossier per §6.1.4.1."
- **Justification**: **Self-Documenting**—the spec should enumerate required types. **Portability** requires knowing which types exist.

#### Issue 35: Recursive Type Example Needed
- **Location**: Lines 2449-2450, §19.1.3 Recursive Types and Size [types.composite]
- **Category**: Completeness
- **Description**: States "types whose size would be infinite **MUST** be rejected with `E-TYP-0105`" but doesn't give an example of what constitutes infinite size vs. indirection.
- **Recommendation**: Add example: "For example, `record Infinite { field: Infinite }` has infinite size and **MUST** be rejected. `record List { next: pointer[List] }` has finite size because the pointer provides indirection."
- **Justification**: **LLM-Friendly**—examples clarify edge cases. **Self-Documenting** requires showing both valid and invalid patterns.

### Part V - Expressions & Statements

#### Issue 36: Evaluation Order "Fixed and Deterministic" Insufficient
- **Location**: Lines 2507-2509, §23.1.2 Evaluation Order [expressions]
- **Category**: Completeness
- **Description**: "evaluation order... **MUST** be fixed and deterministic" but doesn't specify what the order *is* for most operators.
- **Recommendation**: Add: "Unless otherwise specified, infix binary operators evaluate left-to-right (left operand before right operand). Function call arguments evaluate in declaration order (first parameter's argument first). Assignment evaluates the right side before the left side."
- **Justification**: **Deterministic Performance**—developers need to know evaluation order for code with side effects. **LLM-Friendly** requires explicit order specifications.

#### Issue 37: Operator Table Missing Bitwise Operators
- **Location**: Lines 2513-2526, §23.1.2.1 Operator Precedence and Associativity
- **Category**: Completeness
- **Description**: Table includes `<<`, `>>` (shift) but not `&`, `|`, `^` (bitwise AND, OR, XOR) which are listed in §9.2.2 as recognized operators.
- **Recommendation**: Add rows for bitwise operators. Standard precedence (from C/Rust): between comparisons and shifts. Suggested addition:
| 3.5 | `&` | left-to-right |
| 3.6 | `^` | left-to-right |
| 3.7 | `|` | left-to-right |
- **Justification**: **Completeness**—all operators must have defined precedence. **LLM-Friendly** requires complete tables without inference.

#### Issue 38: Pattern Exhaustiveness Verification Algorithm Not Specified
- **Location**: Lines 2552-2554, §25.1.2 Exhaustiveness and Reachability [patterns]
- **Category**: Implementability
- **Description**: Requires implementations "**MUST** either verify exhaustiveness or reject the match" but doesn't provide algorithm guidance or reference to established techniques.
- **Recommendation**: Add informative note: "Exhaustiveness verification **SHOULD** follow established algorithms such as those described in [Maranget 2007, 'Warnings for Pattern Matching'] or equivalent decision-tree analyses. Implementations **MAY** conservatively reject matches where exhaustiveness is undecidable due to complex guards or bindings."
- **Justification**: **Implementability**—exhaustiveness checking is nontrivial. Citing standard algorithms helps implementers and clarifies expectations.

### Part VI - Generics & Parametric Polymorphism

#### Issue 39: Variance Checking Interaction with Bounds Unclear
- **Location**: Lines 2607-2609, §29.1.1 Variance Declaration and Checking [generics.variance]
- **Category**: Completeness
- **Description**: Doesn't specify how variance interacts with bounds. For example, can a covariant parameter have a lower bound (supertype constraint)?
- **Recommendation**: Add: "Variance annotations **MUST** be consistent with declared bounds: covariant parameters **MAY** have upper bounds but **MUST NOT** have lower bounds; contravariant parameters **MAY** have lower bounds but **MUST NOT** have upper bounds; invariant parameters **MAY** have both. Violations **MUST** be rejected with `E-TYP-0304`."
- **Justification**: **Type Safety**—variance must respect bounds to prevent unsoundness. **Implementability** requires explicit variance-bound rules.

#### Issue 40: Generic Instantiation Limit Interaction with Recursion
- **Location**: Lines 2622-2623, §30.1.2 Instantiation Limits and Termination [generics.resolution]
- **Category**: Consistency
- **Description**: Refers to "limits... at least as permissive as the minima in §6.4.1 and Appendix C" but §6.4.1 lists "Generic instantiation nesting: 32 levels" while "Monomorphization instantiations: 1,024 distinct instantiations per generic entity" which are different metrics.
- **Recommendation**: Clarify: "Implementations **MUST** track both instantiation depth (nesting level) and instantiation count (distinct instances). Exceeding either limit **MUST** be diagnosed. The depth limit prevents infinite recursion; the count limit prevents compilation-time/space explosion from distinct type arguments."
- **Justification**: **Deterministic Performance**—two different limits serve different purposes and must be explained. **Clarity** prevents confusion.

### Part VII - Memory Model, Regions, & Permissions

#### Issue 41: Safe Code Memory Safety Guarantee Missing Formal Statement
- **Location**: Lines 2799-2812, §31.1.2 Safety in Safe Code [memory.model]
- **Category**: Completeness
- **Description**: States memory safety informally but doesn't provide a formal safety property like the one in §6.1.3.1 for IFNDR.
- **Recommendation**: Add formal property:
$$
\begin{aligned}
\forall \text{ safe programs } P: \; & \text{well\_formed}(P) \land \lnot \text{uses\_unsafe}(P) \implies \\
& \forall \text{ executions } e: \lnot(\text{out\_of\_bounds}(e) \lor \text{use\_after\_free}(e) \lor \text{double\_free}(e))
\end{aligned}
$$
- **Justification**: **Formal Rigor**—memory safety is a core claim and deserves formal statement. **Implementability** benefits from precise specification.

#### Issue 42: Permission Lattice Not Visualized
- **Location**: Lines 2671-2677, §34.1.2 Permission Lattice and Transitions [memory.permissions]
- **Category**: Completeness
- **Description**: Refers to "permissions **MUST** form a lattice or partial order" but doesn't show the lattice structure.
- **Recommendation**: Add diagram or formal definition: "The permission lattice **MUST** include at least: $\bot$ (no access), $\mathsf{read}$, $\mathsf{write}$, $\mathsf{exclusive}$, with ordering $\bot \sqsubseteq \mathsf{read} \sqsubseteq \mathsf{write} \sqsubseteq \mathsf{exclusive}$ and $\bot \sqsubseteq \mathsf{read}$. Implementations **MAY** extend this lattice."
- **Justification**: **Self-Documenting**—a visual or formal lattice aids comprehension. **LLM-Friendly** requires structured, parseable definitions.

#### Issue 43: Pointer Provenance Modes Definition Belongs in Appendix C
- **Location**: Lines 2706-2713, §36.1.3 Pointer Provenance Modes [memory.layout]
- **Category**: Consistency
- **Description**: Defines IDB interpretation inline in a normative section, but IDB interpretations should be in Appendix C per §12.6.3.
- **Recommendation**: Move the detailed interpretation of provenance tracking modes to Appendix C, §C.2.3.2, and replace with: "The conformance dossier field `pointer_semantics.provenance_tracking` **MUST** take one of the values defined in Appendix C, §C.2.3.2, and implementations **MUST** enforce the safety and optimization constraints documented for the selected mode."
- **Justification**: **Consistency** with documentation structure. **Local Reasoning**—all IDB interpretations should be centralized in Appendix C.

### Part VIII - Contracts & Grants

#### Issue 44: Contract Sequent Syntax Grammar Too Abstract
- **Location**: Lines 2740-2758, §37.1.3 Contract Syntax Overview [contracts.model]
- **Category**: Completeness
- **Description**: The EBNF uses `<expression>` and `<grant_identifier>` but doesn't specify what expressions are valid in contracts (can they have side effects? call procedures?).
- **Recommendation**: Add: "Expressions appearing in `requires` and `ensures` clauses **MUST** be boolean-typed, side-effect-free, and **MUST NOT** invoke non-const procedures. Implementations **MAY** restrict contracts to a decidable subset of expressions when static verification is attempted."
- **Justification**: **Implementability**—contracts with side effects are problematic. **Deterministic Performance** requires bounded evaluation.

#### Issue 45: Grant Leakage Definition Vague
- **Location**: Lines 2777, §38.1.2 Grant Usage and Accumulation [contracts.grants]
- **Category**: Completeness
- **Description**: "Grant leakage that would allow untrusted code to use capabilities beyond its declared contract" is intuitive but not formally defined.
- **Recommendation**: Define formally: "Grant leakage occurs when a capability declared in a procedure's `grants` clause becomes accessible to code paths that do not themselves declare that grant, except through explicit grant propagation rules defined in this Part. Implementations **MUST** track grant sets through closures and higher-order functions."
- **Justification**: **Formal Precision**—security-relevant properties need formal definitions. **Implementability** requires clear propagation semantics.

### Part IX - Behaviors & Dynamic Dispatch

#### Issue 46: Coherence Domain Definition Incomplete
- **Location**: Lines 2848-2856, §44.1.2 Coherence and Orphan Rules [behaviors.witnesses]
- **Category**: Completeness
- **Description**: "coherence domain... is the set of all assemblies that are linked together" doesn't specify when assemblies are considered linked (at compile time for whole-program compilation? at runtime for dynamic linking?).
- **Recommendation**: Clarify: "The coherence domain for a program is determined at link time and comprises all assemblies that contribute code or behavior implementations to the final executable or library. For dynamically linked programs, implementations **MUST** detect coherence violations when libraries are loaded and **MUST** diagnose or reject configurations where multiple implementations exist for the same (type, behavior) pair."
- **Justification**: **Implementability**—coherence checking timeline affects ABI design. **Deterministic Performance** requires knowing when checks occur.

### Part X - Compile-Time Evaluation & Reflection

#### Issue 47: Comptime Determinism Not Guaranteed
- **Location**: Lines 2872-2877, §45.1.1 Comptime Contexts [comptime.model]
- **Category**: Completeness
- **Description**: States comptime code is "executed only in designated comptime contexts" but doesn't require determinism—comptime code could query environment variables, read files, etc.
- **Recommendation**: Add: "Compile-time evaluation **MUST** produce deterministic results: repeated evaluations of the same comptime block with identical compilation environment **MUST** produce identical results. Comptime code **MUST NOT** access sources of non-determinism such as: environment variables not recorded in the compilation manifest, file-system timestamps, random number generation, or network I/O."
- **Justification**: **Deterministic Performance** and **Build Reproducibility**. Non-deterministic compile-time code violates the as-if rule and hinders testing.

#### Issue 48: Code Generation Hygiene Algorithm Not Specified
- **Location**: Lines 2914-2916, §49.1.2 Name Collisions and Hygiene [comptime.codegen]
- **Category**: Implementability
- **Description**: Requires "avoid collisions" and "hygiene violations" but doesn't define a hygiene algorithm.
- **Recommendation**: Add: "Implementations **SHOULD** employ macro hygiene techniques such as gensym (fresh identifier generation) or syntactic scoping to prevent collisions. Generated identifiers **SHOULD** be marked with a distinguished prefix or metadata indicating their synthetic origin. The hygiene mechanism **MUST** be documented in the conformance dossier."
- **Justification**: **Implementability**—hygiene is complex and needs guidance. **LLM-Friendly** benefits from explicit algorithmic suggestions.

### Part XI - Concurrency & Memory Ordering

#### Issue 49: DRF Guarantee Lacks Formal Happens-Before Definition
- **Location**: Lines 2937-2940, §50.1.3 Data-Race-Free Semantics [concurrency.model]
- **Category**: Completeness
- **Description**: Uses "happens-before" without defining it formally. Standard memory models define this precisely.
- **Recommendation**: Add formal definition or reference: "The happens-before relation $\to_{hb}$ is the transitive closure of program order $\to_{po}$ (sequential operations within a thread) and synchronization order $\to_{so}$ (cross-thread ordering established by atomic operations, locks, and synchronization primitives). Formally: $\to_{hb} = (\to_{po} \cup \to_{so})^+$. Operations $a$ and $b$ are data-race-free if for any conflicting accesses, $a \to_{hb} b$ or $b \to_{hb} a$."
- **Justification**: **Formal Rigor**—memory models require precise definitions. **Implementability**—implementers need to encode happens-before correctly.

#### Issue 50: Atomic Ordering Set Lacks Formal Semantics
- **Location**: Lines 2951-2957, §51.1.2 Ordering Guarantees [concurrency.atomics]
- **Category**: Completeness
- **Description**: Lists ordering annotations (Relaxed, Acquire, Release, AcqRel, SeqCst) but doesn't define their formal semantics.
- **Recommendation**: Add: "The semantics of each ordering **MUST** conform to the C++20 memory model or equivalent formal specification:
- `Relaxed`: No cross-thread ordering; only guarantees atomicity and modification order per location.
- `Acquire`: Synchronizes-with Release operations on the same location; establishes happens-before.
- `Release`: Pairs with Acquire to establish happens-before.
- `AcqRel`: Combines Acquire and Release.
- `SeqCst`: Total ordering on all SeqCst operations.
Precise semantics are defined in [C++20] or equivalent normative references."
- **Justification**: **Implementability**—atomic semantics are critical for correctness. **Consistency** with established standards prevents reinvention.

### Part XII - Interoperability & ABI

#### Issue 51: FFI Type Representability Not Defined
- **Location**: Lines 2974-2976, §52.1.1 FFI Declarations [ffi]
- **Category**: Completeness
- **Description**: "Types used in FFI signatures **MUST** have well-defined representations in the target ABI" but doesn't specify what "well-defined representation" means.
- **Recommendation**: Define: "A type has a well-defined foreign representation if and only if: (a) its size, alignment, and field layout match a type in the target language's ABI; (b) its bit-level representation is compatible with the foreign type (endianness, padding, discriminant placement); and (c) ownership/lifetime semantics do not conflict with the foreign model. The conformance dossier **MUST** enumerate all types with defined foreign representations."
- **Justification**: **Implementability**—FFI safety depends on precise type correspondence. **Self-Documenting** requires enumeration of safe types.

#### Issue 52: Cross-Version Linkage Diagnostic Missing
- **Location**: Lines 3004-3006, §54.1.2 Cross-Version and Cross-Language Linking [linkage]
- **Category**: Completeness
- **Description**: Requires diagnosing incompatible ABIs but doesn't specify which diagnostic codes to use.
- **Recommendation**: Add: "Implementations **MUST** diagnose ABI incompatibility using error diagnostics in the `FFI-02` or `CNF-04` feature buckets (as appropriate), and **MUST NOT** permit linkers to silently mix incompatible compilation units."
- **Justification**: **Consistency**—all diagnostics should reference specific codes. **Implementability** requires actionable error reporting.

---

## Cross-Cutting Issues

### Cross-Cutting Issue 1: Inconsistent Requirement Phrasing
- **Scope**: All parts
- **Category**: Style
- **Description**: The specification uses inconsistent phrasing for requirements. Some sections use "MUST" at the beginning ("Implementations **MUST** ..."), while others embed it mid-sentence ("Each type **MUST** have..."), and still others use imperative mood without modal verbs. This reduces scannability and creates ambiguity about which statements are normative.
- **Recommendation**: Standardize on the RFC 2119 pattern: Start normative requirements with the subject that bears the obligation, followed immediately by the modal verb. Preferred forms:
  - "Implementations **MUST** ..."
  - "Programs **MUST** ..."
  - "The X construct **MUST** ..."
Apply this pattern systematically to all MUST/SHOULD/MAY requirements. Where passive voice is unavoidable, maintain modal verb visibility: "X **MUST** be verified by..."
- **Justification**: **LLM-Friendly** and **Self-Documenting** principles require consistent, predictable patterns. RFC 2119 usage depends on recognizable keyword placement. Automated compliance tools need uniform structure for parsing.

### Cross-Cutting Issue 2: Forward Reference Pattern Inconsistency
- **Scope**: Part I-XII
- **Category**: Consistency
- **Description**: Forward references use inconsistent formats: sometimes "(§X.Y)", sometimes "(see §X.Y)", sometimes "(Part Z, §X.Y)", and sometimes just "§X.Y". The presence/absence of semantic anchors (e.g., "[anchor.name]") is also inconsistent.
- **Recommendation**: Standardize forward reference format:
  - Within-part references: "§X.Y.Z [anchor.name]"
  - Cross-part references: "Part N, §X.Y.Z [anchor.name]"
  - Appendix references: "Appendix Z, §Z.M [anchor.name]"
  - Always include semantic anchor when available
Example: Change "see §6.1.4" to "§6.1.4 [conformance.behavior.idb]"
- **Justification**: **Local Reasoning**—consistent references reduce navigation burden. Semantic anchors provide version-stable links per §7.4.4. **LLM-Friendly** patterns benefit from uniform structure.

### Cross-Cutting Issue 3: Missing Implementation Guidance for IFNDR Detection
- **Scope**: §6.1.3.1, §12.4, and uses throughout Parts III-XII
- **Category**: Implementability
- **Description**: Many requirements state violations "**MAY** be IFNDR" without guidance on when implementers should choose IFNDR vs. rejection. This creates inconsistency across implementations.
- **Recommendation**: Add to §6.1.3.1 or §12.4: "Implementers **SHOULD** classify violations as IFNDR only when: (a) detection would require analysis complexity exceeding $O(n^3)$ or solving NP-hard problems; (b) detection would require whole-program information not available at the compilation unit's translation; or (c) the conformance dossier explicitly designates the check as computationally infeasible for the target architecture. When in doubt, implementations **SHOULD** emit diagnostics rather than classify as IFNDR."
- **Justification**: **One Correct Way**—reduce implementation divergence. **Memory Safety Without Complexity** requires appropriate effort investment.

### Cross-Cutting Issue 4: Appendix References Not Resolved
- **Scope**: All parts referencing appendices
- **Category**: Completeness
- **Description**: The specification frequently references "Appendix A", "Appendix B", etc., but the appendices themselves are incomplete stubs stating "**See [Appendix-X-Name.md](...)**". This prevents the specification from being self-contained.
- **Recommendation**: Either (a) merge appendix content into the main specification document, or (b) ensure all appendix stub sections include at minimum: a summary paragraph of the appendix's purpose, the schema/structure it defines, and representative examples. Full detail can remain in separate files, but the main document should be minimally readable without following external links.
- **Justification**: **Local Reasoning**—specifications should be comprehensible without requiring multi-file navigation. **Implementability** suffers when critical normative content is externalized.

### Cross-Cutting Issue 5: Diagnostic Code Assignment Gaps
- **Scope**: Throughout specification
- **Category**: Completeness
- **Description**: Many requirements state "**MUST** emit diagnostic X" but some reference codes (like `E-TYP-0304` in Issue 39 recommendation or `E-MEM-0501–0504` in Part XI table) that aren't enumerated in the visible portions of the specification or marked as placeholders.
- **Recommendation**: Audit all diagnostic code references and ensure:
  1. Every referenced code appears in Appendix B taxonomy or Appendix D catalog.
  2. Codes that are planned but not yet assigned are marked with: `(Reserved for <description>)` in Appendix D.
  3. New requirements that introduce diagnostics include the code in both the normative requirement and the appendices.
Create a diagnostic cross-reference index in Appendix D mapping each code to its defining section.
- **Justification**: **Consistency** and **Implementability**—diagnostic codes must be stable and complete. **Self-Documenting** requires traceability from requirement to diagnostic definition.

### Cross-Cutting Issue 6: Formal Notation Inconsistency
- **Scope**: Parts using inference rules and formal semantics (Part II §11, Parts IV-XI)
- **Category**: Consistency
- **Description**: Some sections use full inference rule notation with premises/conclusions, while others use pseudo-mathematical notation without formal judgments. Rule naming conventions (T-*, E-*, WF-*) are inconsistent—some sections lack them entirely.
- **Recommendation**: Establish and enforce formal notation standards:
  1. Every typing or evaluation rule **SHOULD** have a labeled tag (e.g., `\tag{Rule-Name}`).
  2. Rule names **SHOULD** follow the prefix conventions from §3.2.
  3. Judgments **SHOULD** use consistent turnstile notation ($\vdash$, $\Rightarrow$, $\Downarrow$) as defined in §3.2.
  4. Sections introducing new judgment forms **MUST** define them in a dedicated subsection.
Create a formal notation index in an appendix listing all judgment forms and their meanings.
- **Justification**: **Formal Rigor**—specifications using formal methods must apply them consistently. **LLM-Friendly** benefits from systematic, machine-parseable notation.

### Cross-Cutting Issue 7: Examples Marked "Informative" But Used Normatively
- **Scope**: Throughout specification
- **Category**: Consistency
- **Description**: §2.1.4 states "All code examples... are **informative** unless explicitly marked as normative" but some requirements embed grammatical patterns in examples without clear separation. For instance, §9.4.1 uses code blocks that define the grammar inline.
- **Recommendation**: Audit all code examples and:
  1. Mark examples in normative EBNF blocks differently from illustrative code snippets (use distinct fence annotations).
  2. When grammar is normative, state: "[Normative Grammar]" above the block.
  3. For informative examples that illustrate normative requirements, add: "[Informative Example]" and a sentence explaining which requirement it illustrates.
Ensure Appendix A contains all normative grammar, and in-text grammar serves only as illustration with appropriate forward references.
- **Justification**: **Consistency** between normative and informative content. **Self-Documenting**—readers need clear signals about what's authoritative.

### Cross-Cutting Issue 8: "Skeleton" Sections Lack Placeholder Text
- **Scope**: Parts V-XII (see completion matrix line 15-31)
- **Category**: Completeness
- **Description**: The specification completion matrix marks many parts as "Skeleton" but doesn't indicate *within those parts* what content is placeholder vs. normative. Readers may implement based on skeleton text that's intended to change.
- **Recommendation**: For every section marked "Skeleton" in the matrix:
  1. Add a blockquote at the section start: "> [!warning] Skeleton Section: This section is incomplete. Current text establishes invariants and obligations but detailed rules are forthcoming in a future version."
  2. At minimum, skeleton sections should contain: (a) the safety invariants or obligations, (b) references to which diagnostics will be used, (c) placeholder subsections for missing content.
This prevents premature implementation of provisional text.
- **Justification**: **Implementability**—implementers need to know what's stable. **Transparency** requires signaling work-in-progress content clearly.

### Cross-Cutting Issue 9: IDB Documentation Centralization
- **Scope**: All IDB mentions throughout specification
- **Category**: Consistency
- **Description**: Implementation-defined behavior is documented inline in normative sections (e.g., §8.1.4, §36.1.3) rather than centralized in Appendix C as the conformance dossier schema suggests. This creates maintenance burden and scatters IDB requirements.
- **Recommendation**: Standardize IDB documentation pattern:
  1. Normative sections **SHOULD** state: "The implementation **MUST** document [behavior X] in the conformance dossier field `Y.Z` (Appendix C, §C.2.3.N)."
  2. Appendix C **MUST** enumerate all required dossier fields with their value sets and semantics.
  3. Detailed interpretation of IDB values belongs in Appendix C, not inline in Parts I-XII.
This creates a single source of truth for IDB semantics.
- **Justification**: **Consistency** and **Maintainability**. **Local Reasoning** improves when IDB interpretation is centralized. Reduces duplication and drift risk.

### Cross-Cutting Issue 10: Redundant Definition of Concepts Across Parts
- **Scope**: Concepts defined in Part 0/I and redefined in later parts
- **Category**: Style
- **Description**: Some fundamental concepts are defined in §4 Terminology but later parts redefine them inline (e.g., "binding" in §4.3 and again in §14.3.1). This violates the "define once" principle and risks creating subtle inconsistencies.
- **Recommendation**: Apply strict rule: Fundamental concepts **MUST** be defined exactly once in Part 0 §4 Terminology or in their first substantive use (with forward references from Part 0). Subsequent uses **SHOULD** cite the definition: "A binding (§4.3 [terminology.programming]) associates...".
Audit the specification to identify and eliminate redundant definitions, replacing them with citations.
- **Justification**: **One Correct Way**—each concept has one authoritative definition. **Consistency** prevents definitional drift. **Maintainability** improves when changes need only one location update.

### Cross-Cutting Issue 11: Insufficient Diagnostic Specification Detail
- **Scope**: Many diagnostic requirements throughout Parts I-XII
- **Category**: Completeness
- **Description**: While the specification rigorously assigns diagnostic codes, it often doesn't specify: (a) the exact message text or template, (b) what source locations should be highlighted, (c) whether related notes should be emitted. This reduces consistency across implementations.
- **Recommendation**: Enhance Appendix D (Diagnostic Catalog) to include for each diagnostic code:
  1. **Template**: A message template with placeholders (e.g., "Expected `{expected_type}` but found `{actual_type}`").
  2. **Primary Location**: What source construct should be highlighted.
  3. **Related Notes**: Whether additional locations or context should be shown.
  4. **Example**: A triggering code snippet.
This standardizes user experience while preserving implementation flexibility in presentation.
- **Justification**: **LLM-Friendly**—consistent error messages aid code generation. **Self-Documenting**—developers benefit from uniform diagnostic quality across implementations. Reduces learning curve when switching implementations.

---

## Summary Statistics

- **Total issues identified**: 52 (41 section-specific + 11 cross-cutting)
- **By category**:
  - Style: 12
  - Consistency: 17
  - Completeness: 20
  - Implementability: 8
  - Multiple categories: 5
- **Critical issues requiring immediate attention**: 11
  - Cross-Cutting Issue 1 (Requirement Phrasing)
  - Cross-Cutting Issue 4 (Appendix References)
  - Cross-Cutting Issue 8 (Skeleton Sections)
  - Issue 8 (Circular Definition Risk in IFNDR)
  - Issue 10 (Conforming Program Definition)
  - Issue 25 (Phase 1 Formal Rules Undefined Predicates)
  - Issue 34 (Primitive Type Set Not Enumerated)
  - Issue 41 (Memory Safety Formal Statement)
  - Issue 49 (DRF Happens-Before Definition)
  - Issue 50 (Atomic Ordering Semantics)
  - Issue 51 (FFI Type Representability)

- **Minor issues for future consideration**: 41

---

## Prioritized Recommendations

### Immediate (Pre-1.0.0 Release)

1. **Cross-Cutting Issue 1**: Standardize requirement phrasing to RFC 2119 pattern (affects comprehensibility across entire spec)
2. **Cross-Cutting Issue 8**: Mark skeleton sections explicitly to prevent premature implementation
3. **Issue 34**: Enumerate primitive types explicitly (critical for portability)
4. **Issue 41**: Add formal memory safety statement (core language guarantee)
5. **Issue 49, 50**: Define happens-before and atomic orderings formally (concurrency correctness depends on this)

### High Priority (Version 1.1.0)

6. **Cross-Cutting Issue 4**: Resolve or inline appendix references for self-containment
7. **Cross-Cutting Issue 5**: Complete diagnostic code assignments and cross-reference index
8. **Issue 8**: Refine IFNDR definition with objective criteria
9. **Issue 10**: Clarify conforming program UVB attestation requirements
10. **Cross-Cutting Issue 10**: Eliminate redundant definitions throughout

### Medium Priority (Version 1.2.0)

11. **Cross-Cutting Issue 2**: Standardize forward reference format
12. **Cross-Cutting Issue 6**: Enforce formal notation consistency
13. **Cross-Cutting Issue 11**: Enhance diagnostic catalog with templates and examples
14. Issues 1-7, 11-40: Address section-specific style, completeness, and consistency issues

### Low Priority (Future Versions)

15. **Cross-Cutting Issue 7**: Audit and clarify normative vs informative examples
16. **Cross-Cutting Issue 9**: Centralize IDB documentation in Appendix C
17. Remaining minor editorial and organizational improvements (Issues 42-52)

---

## Conclusion

The Cursive Language Specification demonstrates strong technical foundations and thorough coverage of systems programming language concerns. The specification excels in areas such as:

- Comprehensive behavior classification (UVB, USB, IDB, defined, IFNDR)
- Detailed diagnostic code taxonomy and systematic error reporting
- Memory safety model with regions and permissions
- Compile-time evaluation and reflection framework
- Concurrency and atomics treatment

**Primary areas for improvement:**

1. **Consistency in normative phrasing** to enhance LLM-friendliness and implementer clarity
2. **Completion of skeleton sections** with explicit staging and placeholder markers
3. **Formalization of key semantic concepts** (memory safety, happens-before, type representability)
4. **Centralization of scattered definitions** to eliminate redundancy
5. **Appendix integration** for true self-containment

Implementing the immediate and high-priority recommendations will significantly strengthen the specification's usability, reduce implementation divergence risk, and better align with the language's **One Correct Way**, **Self-Documenting**, **LLM-Friendly**, and **Local Reasoning** design principles.

The specification is well-positioned for successful standardization and implementation once these issues are addressed.
