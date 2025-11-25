# The Cursive Language Specification (Draft 3)

## Clause 1: General Principles and Conformance

This clause establishes the foundational framework for the Cursive language specification. It defines the normative requirements for conforming implementations and programs, the classification of program behaviors, the minimum capacities implementations MUST support, and the rules governing language evolution.

---

### 1.1 Conformance Obligations

##### Definition

A **conforming implementation** is a translator (compiler, interpreter, or hybrid) that satisfies all normative requirements of this specification applicable to implementations. A **conforming program** is a Cursive source program that satisfies all normative requirements of this specification applicable to programs.

**Formal Definition**

Let $\mathcal{I}$ denote the set of all possible implementations and $\mathcal{P}$ denote the set of all possible Cursive source programs. Define:

$$\text{Conforming}_{\mathcal{I}}(i) \iff i \text{ satisfies all MUST-level requirements for implementations}$$

$$\text{WellFormed}(p) \iff p \text{ satisfies all syntactic and static-semantic rules}$$

$$\text{Conforming}_{\mathcal{P}}(p) \iff \text{WellFormed}(p) \land p \text{ compiles without errors}$$

##### Static Semantics

**Implementation Requirements**

A conforming implementation:

1. MUST accept all well-formed programs and translate them to an executable representation.

2. MUST reject all ill-formed programs (except those classified as IFNDR per §1.2.1) with at least one error diagnostic identifying the violation.

3. MUST generate a **Conformance Dossier** as a build artifact. The dossier MUST be a machine-readable JSON document conforming to the schema defined in Appendix C.

4. MUST document all Implementation-Defined Behavior (IDB) choices in the Conformance Dossier.

5. MUST record all IFNDR instances encountered during compilation in the Conformance Dossier.

**Program Requirements**

A **well-formed program** satisfies the following conditions:

1. Satisfies all lexical constraints defined in Clause 2.
2. Satisfies all syntactic constraints defined in Clause 3.
3. Satisfies all static-semantic constraints defined throughout this specification.

A **conforming program** is a well-formed program that compiles without errors.

##### Constraints & Legality

**Negative Constraints**

An implementation MUST NOT silently accept an ill-formed program without issuing at least one diagnostic (except for IFNDR cases as defined in §1.2.1).

An implementation MUST NOT reject a well-formed program without issuing at least one diagnostic explaining the rejection.

An implementation MUST NOT alter the observable behavior of a conforming program based on implementation-specific optimizations in ways that violate the semantic guarantees of this specification.

**Diagnostic Table**

| Code         | Severity | Condition                                                            | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0101` | Error    | Program is ill-formed due to syntactic or static-semantic violation. | Compile-time | Rejection |
| `E-CNF-0102` | Error    | Conformance Dossier generation failed.                               | Compile-time | Rejection |
| `E-CNF-0103` | Error    | Implementation limit exceeded (see §1.4).                            | Compile-time | Rejection |

---

### 1.2 Behavior Classifications

##### Definition

**Behavior classifications** partition all program behaviors into four mutually exclusive categories based on how this specification constrains outcomes and what guarantees implementations provide.

**Formal Definition**

Let $\mathcal{B}$ be the universe of all possible program behaviors. Define the partition:

$$\mathcal{B} = \mathcal{B}_{\text{Defined}} \uplus \mathcal{B}_{\text{IDB}} \uplus \mathcal{B}_{\text{USB}} \uplus \mathcal{B}_{\text{UVB}}$$

where $\uplus$ denotes disjoint union.

| Category                     | Symbol                         | Spec Constrains Outcome?  | Impl. Documents? | Safe Code Permitted? |
| :--------------------------- | :----------------------------- | :------------------------ | :--------------- | :------------------- |
| Defined Behavior             | $\mathcal{B}_{\text{Defined}}$ | Exactly one outcome       | N/A              | Yes                  |
| Implementation-Defined (IDB) | $\mathcal{B}_{\text{IDB}}$     | Set of permitted outcomes | MUST             | Yes                  |
| Unspecified Behavior (USB)   | $\mathcal{B}_{\text{USB}}$     | Set of permitted outcomes | Need not         | Yes                  |
| Unverifiable Behavior (UVB)  | $\mathcal{B}_{\text{UVB}}$     | No constraint             | N/A              | `unsafe` only        |

##### Static Semantics

**Defined Behavior** is program behavior for which this specification prescribes exactly one permitted outcome. Conforming implementations MUST produce this outcome for all well-formed programs exercising such behavior.

**Implementation-Defined Behavior (IDB)** occurs when this specification permits multiple valid outcomes and requires the implementation to document which outcome it chooses. The implementation MUST select exactly one outcome from the permitted set, document its choice in the Conformance Dossier, and maintain consistency across all compilations with the same configuration. Programs relying on a specific IDB choice are conforming but non-portable. See Appendix G for the complete IDB index.

**Unspecified Behavior (USB)** occurs when this specification permits a set of valid outcomes but does not require the implementation to document which outcome is chosen. The choice MAY vary between executions, compilations, or within a single execution. USB in safe code MUST NOT introduce memory unsafety or data races. See Appendix G for the complete USB index.

**Unverifiable Behavior (UVB)** comprises operations whose runtime correctness depends on properties external to the language's semantic model. UVB is permitted only within `unsafe` blocks and Foreign Function Interface (FFI) calls. By using an `unsafe` block, the programmer asserts responsibility for upholding an external contract that the compiler cannot verify. See Appendix G for the complete UVB index.

##### Constraints & Legality

**Safety Invariant**

Safe code cannot exhibit Unverifiable Behavior.

**Formal Statement**

For any well-formed program $p$ where all expressions are outside of `unsafe` blocks:

$$p \text{ MUST NOT exhibit any behavior in } \mathcal{B}_{\text{UVB}}$$

This invariant is enforced by the type system (Clause 4), the permission system (Clause 5), and the module system (Clause 6).

---

#### 1.2.1 Ill-Formed, No Diagnostic Required (IFNDR)

##### Definition

A program that violates a static-semantic rule of the language is **ill-formed**. When detecting such a violation is computationally undecidable or infeasible, an implementation is not required to issue a diagnostic. Such a program is classified as **Ill-Formed, No Diagnostic Required (IFNDR)**.

**Formal Definition**

Let $\text{IllFormed}(p)$ denote that program $p$ violates at least one static-semantic rule. Define:

$$\text{IFNDR}(p) \iff \text{IllFormed}(p) \land \text{DetectionInfeasible}(p)$$

where $\text{DetectionInfeasible}(p)$ indicates that detecting the violation would require solving an undecidable problem or exceed reasonable computational bounds.

##### Static Semantics

**Classification Criteria**

A violation is classified as IFNDR when at least one of the following conditions holds:

1. The violation involves a property that is undecidable in general (e.g., halting, unbounded symbolic execution).
2. The violation requires analysis that exceeds polynomial complexity in the size of the program.
3. The violation depends on runtime values that cannot be statically determined.

**IFNDR Categories**

| Category            | Description                                                                         |
| :------------------ | :---------------------------------------------------------------------------------- |
| OOB in Const Eval   | Index out of bounds during constant evaluation where index is not a static constant |
| Recursion Limits    | Exceeding implementation limits in complex metaprogramming that cannot be bounded   |
| Infinite Type       | Type definitions that would require infinite expansion                              |
| Unbounded Unrolling | Loop unrolling in `comptime` context that cannot be statically bounded              |

##### Constraints & Legality

**Safety Boundary**

The behavior of an IFNDR program is Unspecified Behavior (USB). However, when the IFNDR condition occurs in safe code, the resulting behavior MUST NOT introduce Unverifiable Behavior (UVB).

**Formal Statement**

$$\text{IFNDR}(p) \land \text{Safe}(p) \implies \text{Behavior}(p) \in \mathcal{B}_{\text{USB}}$$

The effects of IFNDR in safe code MUST be bounded by the language's safety guarantees. Permitted outcomes include:

- Compilation failure (if detected)
- Deterministic panic at runtime
- Non-deterministic but safe execution

Prohibited outcomes include:

- Memory corruption
- Data races
- Arbitrary code execution

**Tracking Requirement**

Implementations MUST track all instances where IFNDR rules apply during compilation, even when no diagnostic is issued. This tracking enables:

- Post-hoc analysis of potentially problematic code
- Audit trails for security-critical systems
- Tooling integration for static analysis

**Reporting Requirement**

All IFNDR instances MUST be reported in the Conformance Dossier (see Appendix C).

**Instance Data**

Each IFNDR instance recorded in the Conformance Dossier MUST include the following fields:

| Field      | Type   | Description                                                    |
| :--------- | :----- | :------------------------------------------------------------- |
| `location` | Object | Source location (file path, line number, column, module path)  |
| `category` | String | Category identifier (e.g., `"OOB in Const Eval"`)              |
| `details`  | String | Human-readable description of the specific instance (OPTIONAL) |

---

### 1.3 Reserved Identifiers

##### Definition

A **reserved identifier** is an identifier that is reserved for use by this specification or by implementations, and MUST NOT be used as a user-defined identifier in conforming programs.

##### Static Semantics

**Reserved Keywords**

The identifiers listed in the keyword table (Clause 2, §2.3) are reserved keywords. They MUST NOT be used as user-defined identifiers.

**Reserved Namespaces**

The `cursive.*` namespace prefix is reserved for specification-defined features. User programs and vendor extensions MUST NOT use this namespace.

**Implementation Reservations**

Implementations MAY reserve additional identifier patterns (e.g., identifiers beginning with `__` or `_[A-Z]`). Such reservations:

1. MUST be documented in the implementation's Conformance Dossier.
2. SHOULD follow common conventions to avoid collision with user code.

Conforming programs SHOULD NOT use patterns that are commonly reserved by implementations.

**Universe-Protected Bindings**

Core language identifiers, including primitive type names (`i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `bool`, `char`, `usize`, `isize`), MUST NOT be shadowed by user declarations at module scope. Attempting to do so MUST be diagnosed as an error.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-CNF-0401` | Error    | Reserved keyword used as identifier.              | Compile-time | Rejection |
| `E-CNF-0402` | Error    | Reserved namespace `cursive.*` used by user code. | Compile-time | Rejection |
| `E-CNF-0403` | Error    | Primitive type name shadowed at module scope.     | Compile-time | Rejection |
| `W-CNF-0401` | Warning  | Implementation-reserved pattern used.             | Compile-time | N/A       |

---

### 1.4 Implementation Limits

##### Definition

**Implementation limits** are minimum guaranteed capacities that all conforming implementations MUST support. Programs that exceed these limits are ill-formed.

##### Static Semantics

**Minimum Guaranteed Limits**

Conforming implementations MUST support at least the following limits:

| Category           | Limit  | Unit                  | Description                                      |
| :----------------- | :----- | :-------------------- | :----------------------------------------------- |
| Source File Size   | 1      | MiB (1,048,576 bytes) | Maximum byte length of a source file             |
| Line Length        | 16,384 | Unicode scalar values | Maximum characters per logical line              |
| Logical Lines      | 65,535 | lines                 | Maximum logical lines per source file            |
| Nesting Depth      | 256    | levels                | Maximum nesting of blocks and expressions        |
| Identifier Length  | 1,023  | Unicode scalar values | Maximum length of an identifier                  |
| Parameters         | 255    | parameters            | Maximum parameters per procedure signature       |
| Fields             | 1,024  | fields                | Maximum fields per record type                   |
| Comptime Recursion | 256    | frames                | Maximum recursion depth in `comptime` evaluation |

**Exceeding Limits**

A program that exceeds any implementation limit is ill-formed. The implementation MUST issue an error diagnostic when a limit is exceeded.

**Extended Limits**

Implementations MAY support limits greater than the minimums specified above. Extended limits:

1. MUST be documented in the Conformance Dossier.
2. Are classified as Implementation-Defined Behavior (IDB).

Programs relying on extended limits are conforming but non-portable.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-CNF-0501` | Error    | Source file exceeds size limit.    | Compile-time | Rejection |
| `E-CNF-0502` | Error    | Line exceeds length limit.         | Compile-time | Rejection |
| `E-CNF-0503` | Error    | File exceeds logical line limit.   | Compile-time | Rejection |
| `E-CNF-0504` | Error    | Nesting depth exceeded.            | Compile-time | Rejection |
| `E-CNF-0505` | Error    | Identifier exceeds length limit.   | Compile-time | Rejection |
| `E-CNF-0506` | Error    | Too many parameters in procedure.  | Compile-time | Rejection |
| `E-CNF-0507` | Error    | Too many fields in record.         | Compile-time | Rejection |
| `E-CNF-0508` | Error    | Comptime recursion depth exceeded. | Compile-time | Rejection |

---

### 1.5 Language Evolution

##### Definition

**Language evolution** governs the versioning, feature lifecycle, edition system, and extension mechanisms for the Cursive language.

##### Static Semantics

**Versioning Model**

The Cursive language version identifier MUST follow Semantic Versioning (SemVer):

```
MAJOR.MINOR.PATCH
```

| Component | Increment Semantics                                     |
| :-------- | :------------------------------------------------------ |
| `MAJOR`   | Incompatible changes to stable features                 |
| `MINOR`   | Backwards-compatible additions; preview feature changes |
| `PATCH`   | Backwards-compatible bug fixes and clarifications       |

**Version Declaration**

Programs MUST declare their target language version in the project manifest. The mechanism for version declaration is implementation-defined.

**Version Compatibility**

An implementation MUST reject a program if the declared `MAJOR` version does not match a `MAJOR` version supported by the implementation.

Implementations MUST maintain source-level backward compatibility for conforming programs across `MINOR` version increments within the same `MAJOR` version.

**Editions**

Editions provide an optional mechanism for grouping incompatible changes without incrementing the `MAJOR` version for every breaking change.

When editions are supported:

1. An implementation MUST allow a program to declare its target edition.
2. Semantics from different editions MUST NOT be mixed within a single compilation unit.
3. An edition MAY introduce new keywords, alter syntax, or change semantic rules.
4. Changes introduced by an edition apply only to code that has opted into that edition.

**Feature Stability Classes**

Every language feature MUST be classified into exactly one stability class:

| Class          | Availability          | Change Policy                                                |
| :------------- | :-------------------- | :----------------------------------------------------------- |
| `Stable`       | Default (no opt-in)   | No breaking changes except in `MAJOR` increments or editions |
| `Preview`      | Requires feature flag | MAY change between `MINOR` versions                          |
| `Experimental` | Requires feature flag | MAY change or be removed in any version                      |

**Deprecation Policy**

The deprecation policy defines the lifecycle for removing language features:

1. Deprecated features MUST remain functional for at least one full `MINOR` version after the version in which they are deprecated.

2. Implementations MUST issue a warning diagnostic when a deprecated feature is used.

3. Features, other than `Experimental` ones, MAY be removed only in `MAJOR` version increments or new editions.

4. Removal of a deprecated feature MUST result in an error diagnostic.

5. Features with discovered security vulnerabilities MAY follow an expedited deprecation and removal timeline, documented in the release notes.

**Feature Flags**

Extensions and preview features requiring opt-in MUST be controlled through feature flags.

1. Feature flags for specification-defined preview features MUST use identifiers without vendor prefixes.

2. An implementation MUST issue an error for an unknown feature flag identifier.

3. Use of a feature that requires a flag, when that flag is not enabled, MUST be diagnosed as an error.

**Vendor Extensions**

Vendor-specific extensions enable implementations to provide additional capabilities beyond the core specification:

1. Vendor-specific extensions MUST use a reverse-domain-style namespace (e.g., `com.vendor.feature`).

2. The `cursive.*` namespace is reserved for specification-defined features and MUST NOT be used by vendor extensions.

3. Extensions MUST NOT alter the meaning of conforming programs written without the extension.

4. Extensions MUST NOT suppress required diagnostics.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-CNF-0601` | Error    | Declared MAJOR version not supported by implementation. | Compile-time | Rejection |
| `E-CNF-0602` | Error    | Unknown feature flag.                                   | Compile-time | Rejection |
| `E-CNF-0603` | Error    | Feature requires flag that is not enabled.              | Compile-time | Rejection |
| `E-CNF-0604` | Error    | Edition mixing within compilation unit.                 | Compile-time | Rejection |
| `W-CNF-0601` | Warning  | Use of deprecated feature.                              | Compile-time | N/A       |
| `E-CNF-0605` | Error    | Use of removed feature.                                 | Compile-time | Rejection |

---

### Clause 1 Cross-Reference Notes

**Terms Defined in Clause 1 That MUST NOT Be Redefined Elsewhere:**

| Term                            | Section | Description                                                  |
| :------------------------------ | :------ | :----------------------------------------------------------- |
| Conforming Implementation       | §1.1    | An implementation satisfying all MUST-level requirements     |
| Conforming Program              | §1.1    | A well-formed program compiling without errors               |
| Well-Formed Program             | §1.1    | A program satisfying all syntactic and static-semantic rules |
| Defined Behavior                | §1.2    | Behavior with exactly one permitted outcome                  |
| Implementation-Defined Behavior | §1.2    | Behavior requiring implementation to document choice         |
| Unspecified Behavior            | §1.2    | Behavior with permitted set, no documentation required       |
| Unverifiable Behavior           | §1.2    | Behavior outside language guarantees, `unsafe` only          |
| IFNDR                           | §1.2.1  | Ill-formed program where diagnostic is not required          |
| Reserved Identifier             | §1.3    | Identifier reserved by spec or implementation                |
| Implementation Limit            | §1.4    | Minimum guaranteed capacity for programs                     |
| Stability Class                 | §1.5    | Feature classification (Stable/Preview/Experimental)         |
| Edition                         | §1.5    | Mechanism for grouping incompatible changes                  |
| Feature Flag                    | §1.5    | Opt-in mechanism for preview/experimental features           |
| Conformance Dossier             | §1.1    | Machine-readable build artifact documenting IDB and IFNDR    |

**Terms Referenced (Defined Elsewhere):**

| Term                  | Source     | Usage in Clause 1                                      |
| :-------------------- | :--------- | :----------------------------------------------------- |
| `unsafe` block        | Clause 7   | Referenced as boundary for UVB                         |
| `comptime`            | Clause 8   | Referenced in IFNDR examples and implementation limits |
| Diagnostic            | Appendix B | Error/warning classification used throughout           |
| Appendix C            | Appendix C | Schema for Conformance Dossier                         |
| Appendix G            | Appendix G | Complete behavior classification index                 |
| `[[repr(C)]]`         | Clause 4   | Referenced in IDB for type layout                      |
| `[[verify(trusted)]]` | Clause 7   | Referenced in UVB index                                |

**Terms Deferred to Later Clauses:**

| Term                  | Deferred To | Reason                                               |
| :-------------------- | :---------- | :--------------------------------------------------- |
| Lexical constraints   | Clause 2    | Detailed token and identifier rules                  |
| Syntactic constraints | Clause 3    | Grammar and parse rules                              |
| Type Layout           | Clause 4    | Size, alignment, representation details              |
| Permission System     | Clause 5    | `unique`, `const`, `partitioned`, `concurrent` rules |
| Module System         | Clause 6    | Visibility, imports, compilation units               |
| `unsafe` semantics    | Clause 7    | Full rules for unsafe blocks and operations          |
| `comptime` semantics  | Clause 8    | Compile-time evaluation rules                        |
| FFI                   | Clause 9    | Foreign function interface rules                     |

---

# Clause 2: Lexical Structure and Source Text

This clause defines the lexical structure of Cursive: how source text is encoded, preprocessed, and transformed into a sequence of tokens.

---

## 2.1 Source Text Encoding

##### Definition

**Source Text** is a sequence of Unicode scalar values that constitutes the input to the Cursive translation pipeline. A **Source Byte Stream** is the raw sequence of bytes read from a file or other input source before decoding. A **Normalized Source File** is the Unicode scalar sequence obtained from a source byte stream after the preprocessing pipeline (§2.3) has successfully completed.

**Formal Definition**

Let $\mathcal{U}$ denote the set of Unicode scalar values. Let $\mathcal{F}$ denote the set of prohibited code points (defined below). A valid source text $S$ is:

$$S \in \mathcal{U}^* \setminus \{s : \exists c \in s. c \in \mathcal{F}\}$$

##### Static Semantics

**UTF-8 Mandate**

Cursive source input MUST be encoded as UTF-8 as specified in RFC 3629.

A byte sequence is a legal UTF-8 sequence if and only if it satisfies all of the following conditions:

1. Each code unit sequence correctly encodes a Unicode scalar value (U+0000–U+D7FF and U+E000–U+10FFFF).
2. The encoding uses the shortest possible representation for each scalar value (overlong encodings are forbidden).
3. No byte sequence encodes a surrogate code point (U+D800–U+DFFF).

Invalid byte sequences MUST trigger diagnostic `E-SRC-0101` identifying the byte offset of the first invalid byte.

**BOM Handling**

If a source file begins with the UTF-8 byte order mark (BOM, U+FEFF) as the first decoded scalar value, the implementation MUST strip the BOM before lexical analysis. A BOM appearing at any position after the first MUST trigger diagnostic `E-SRC-0103`.

When a source file begins with a UTF-8 BOM and otherwise satisfies the constraints of this clause, a conforming implementation SHOULD emit warning `W-SRC-0101` while still accepting the file.

**Unicode Normalization**

Unicode normalization of source text outside identifiers and module-path components is Implementation-Defined Behavior (IDB). Implementations MUST document any normalization they perform in the Conformance Dossier (§1.1).

For identifier lexemes and module-path components, implementations MUST normalize the corresponding scalar sequences to Unicode Normalization Form C (NFC) prior to equality comparison, hashing, or name lookup.

Any normalization performed MUST NOT change logical line boundaries or the byte offsets used for diagnostic locations.

##### Constraints & Legality

**Prohibited Code Points**

Source files MUST NOT contain prohibited code points outside of string and character literals (§2.8). The prohibited code points $\mathcal{F}$ are:

$$\mathcal{F} = \{\text{U+0000}\} \cup \{c : \text{GeneralCategory}(c) = \texttt{Cc}\} \setminus \{\text{U+0009}, \text{U+000A}, \text{U+000C}, \text{U+000D}\}$$

A source file containing a prohibited code point outside of a literal MUST be rejected with diagnostic `E-SRC-0104`.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0101` | Error    | Invalid UTF-8 byte sequence.                 | Compile-time | Rejection |
| `E-SRC-0103` | Error    | Embedded BOM found after the first position. | Compile-time | Rejection |
| `E-SRC-0104` | Error    | Forbidden control character or null byte.    | Compile-time | Rejection |
| `W-SRC-0101` | Warning  | UTF-8 BOM present at the start of the file.  | Compile-time | N/A       |

---

## 2.2 Source File Structure

##### Definition

A **Logical Line** is a sequence of Unicode scalar values terminated by a line feed character (U+000A) or end-of-file. **Line Ending Normalization** is the transformation that converts all platform-specific line ending sequences to a single canonical line feed character.

**Formal Definition**

$$L ::= c^* \cdot (\text{U+000A})? \quad \text{where } c \in \mathcal{U} \setminus (\mathcal{F} \cup \{\text{U+000A}\})$$

A source file is a sequence of logical lines: $S = L_1 \cdot L_2 \cdot \ldots \cdot L_n$.

##### Syntax & Declaration

A source file MUST conform to the following grammar after preprocessing:

```ebnf
<source_file>     ::= <normalized_line>*
<normalized_line> ::= <code_point>* <line_terminator>?
<line_terminator> ::= U+000A
<code_point>      ::= /* any Unicode scalar value other than U+000A 
                         and those prohibited by §2.1 */
```

##### Static Semantics

Implementations MUST normalize line endings before tokenization as follows:

1. Replace each CR LF (U+000D U+000A) with a single LF (U+000A).
2. Replace each standalone CR (U+000D) not followed by LF with a single LF (U+000A).
3. Each LF (U+000A) that does not follow a CR remains unchanged.

Mixed line endings are permitted within a single source file.

##### Constraints & Legality

Implementations MUST enforce the implementation limits specified in §1.4 for source file size, logical line count, and line length.

**Diagnostic Table**

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-SRC-0102` | Error    | Source file exceeds maximum size.    | Compile-time | Rejection |
| `E-SRC-0105` | Error    | Maximum logical line count exceeded. | Compile-time | Rejection |
| `E-SRC-0106` | Error    | Maximum line length exceeded.        | Compile-time | Rejection |

---

## 2.3 Preprocessing Pipeline

##### Definition

The **Preprocessing Pipeline** is the mandatory sequence of transformations applied to a source byte stream to produce a normalized source file suitable for tokenization.

**Formal Definition**

$$\text{Preprocess} : \text{ByteStream} \to \text{TokenStream} \cup \{\bot\}$$

The pipeline is the sequential composition of six steps, where $\bot$ indicates failure:

$$\text{Preprocess} = \text{Tokenize} \circ \text{ValidateCtrl} \circ \text{NormLines} \circ \text{StripBOM} \circ \text{DecodeUTF8} \circ \text{CheckSize}$$

##### Dynamic Semantics

Source text preprocessing MUST execute in the following order:

| Step | Name                         | Operation                                    | Failure Diagnostic |
| :--- | :--------------------------- | :------------------------------------------- | :----------------- |
| 1    | Size Check                   | Verify file size ≤ documented maximum.       | `E-SRC-0102`       |
| 2    | UTF-8 Decoding               | Decode byte stream as UTF-8.                 | `E-SRC-0101`       |
| 3    | BOM Removal                  | Strip leading U+FEFF; reject embedded BOMs.  | `E-SRC-0103`       |
| 4    | Line Normalization           | Convert CR, LF, CRLF → LF.                   | —                  |
| 5    | Control Character Validation | Reject prohibited code points.               | `E-SRC-0104`       |
| 6    | Tokenization                 | Produce token stream from normalized source. | (per §2.4–§2.9)    |

Each step MUST complete successfully before the next step begins. The pipeline MUST be deterministic for a given source byte stream and compilation configuration.

---

## 2.4 Lexical Vocabulary

##### Definition

A **Token** is the atomic unit of lexical analysis. Tokenization transforms a normalized source file into a finite, ordered sequence of tokens.

**Formal Definition**

A token is a triple $t = (\kappa, \lambda, \sigma)$ where:
- $\kappa \in \mathcal{T} = \{\texttt{identifier}, \texttt{keyword}, \texttt{literal}, \texttt{operator}, \texttt{punctuator}, \texttt{newline}\}$
- $\lambda \in \mathcal{U}^*$ is the lexeme
- $\sigma \in \text{Span}$ is a source span (file, start position, end position)

##### Static Semantics

The lexer MUST classify every non-comment, non-whitespace fragment as exactly one token kind:

| Token Kind | Description                                    |
| :--------- | :--------------------------------------------- |
| identifier | An identifier satisfying §2.5                  |
| keyword    | A reserved word listed in §2.6                 |
| literal    | A numeric, string, or character literal (§2.8) |
| operator   | An operator symbol (§2.7)                      |
| punctuator | A punctuation symbol (§2.7)                    |
| newline    | A line terminator (U+000A)                     |

Tokenization MUST be deterministic for a given normalized source file.

##### Constraints & Legality

Any maximal character sequence that cannot be classified MUST trigger diagnostic `E-SRC-0309`.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0309` | Error    | Unclassifiable character sequence in source. | Compile-time | Rejection |

---

## 2.5 Identifiers

##### Definition

An **Identifier** is a sequence of Unicode scalar values that names a declaration, binding, type, or other program entity, following Unicode Standard Annex #31 (UAX #31).

**Formal Definition**

$$\text{Ident} = \{s \in \mathcal{U}^+ : s_0 \in \text{XID\_Start} \cup \{\_\} \land \forall i > 0.\, s_i \in \text{XID\_Continue} \cup \{\_\}\}$$

##### Syntax & Declaration

```ebnf
<identifier>     ::= <ident_start> <ident_continue>*
<ident_start>    ::= /* Unicode XID_Start */ | "_"
<ident_continue> ::= /* Unicode XID_Continue */ | "_"
```

##### Static Semantics

Identifiers MUST NOT contain code points prohibited by §2.1, surrogate code points (U+D800–U+DFFF), or non-characters.

Reserved keywords (§2.6) MUST NOT be treated as identifiers.

**Name Equivalence**

Two identifiers are the same binding name if and only if their NFC-normalized forms (per §2.1) are identical.

##### Constraints & Legality

Implementations MUST accept identifiers whose length is at least the minimum specified in §1.4.

**Diagnostic Table**

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-SRC-0305` | Error    | Reserved keyword used as identifier. | Compile-time | Rejection |
| `E-SRC-0307` | Error    | Invalid Unicode in identifier.       | Compile-time | Rejection |

---

## 2.6 Keywords

##### Definition

A **Keyword** is a reserved lexeme that has special meaning in the language grammar and MUST NOT be used where an identifier is expected.

**Formal Definition**

$$\mathcal{K} = \{\texttt{abstract}, \texttt{as}, \texttt{break}, \texttt{char}, \ldots, \texttt{witness}, \texttt{where}\}$$

The complete enumeration follows.

##### Static Semantics

The reserved keywords are:

```
abstract    as          break       char        comptime    const
continue    double      else        enum        extern      false
float       fork        half        if          imm         import
int         internal    let         loop        match       modal
module      move        mut         override    parallel    partitioned
private     procedure   protected   public      quote       record
region      result      return      self        Self        shadow
static      true        trait       type        uint        unique
unsafe      use         var         witness     where
```

Implementations MUST tokenize these as `<keyword>`, not `<identifier>`. The keyword set MUST be identical across conforming implementations for a given language version.

##### Constraints & Legality

Using a reserved keyword where an identifier is expected MUST trigger diagnostic `E-SRC-0305` (§2.5).

---

## 2.7 Operators and Punctuators

##### Definition

An **Operator** is a symbol or symbol sequence denoting an operation. A **Punctuator** is a symbol or symbol sequence used for syntactic structure.

**Formal Definition**

$$\mathcal{O}_\text{multi} = \{\texttt{==}, \texttt{!=}, \texttt{<=}, \texttt{>=}, \texttt{\&\&}, \texttt{||}, \texttt{<<}, \texttt{>>}, \texttt{..}, \texttt{..=}, \texttt{=>}, \texttt{->}, \texttt{**}, \texttt{::}, \texttt{\~>}, \texttt{\~!}, \texttt{\~\%}, \texttt{\~|}\}$$

$$\mathcal{O}_\text{single} = \{+, -, *, /, \%, <, >, =, !, \&, |, \hat{}, \sim, .\}$$

$$\mathcal{P} = \{(, ), [, ], \{, \}, ,, :, ;\}$$

##### Syntax & Declaration

**Multi-Character Tokens**

| Category          | Tokens                       |
| :---------------- | :--------------------------- |
| Comparison        | `==`, `!=`, `<=`, `>=`       |
| Logical           | `&&`, `\|\|`                 |
| Bitwise Shift     | `<<`, `>>`, `<<=`, `>>=`     |
| Range             | `..`, `..=`                  |
| Arrow             | `=>`, `->`                   |
| Other             | `**`, `::`                   |
| Receiver/Dispatch | `~`, `~>`, `~!`, `~%`, `~\|` |

**Single-Character Tokens**

```
+  -  *  /  %  <  >  =  !  &  |  ^  ~  .  ,  :  ;  (  )  [  ]  {  }
```

**Receiver Token Semantics**

| Token | Semantic Meaning                          |
| :---- | :---------------------------------------- |
| `~`   | Shorthand for `self` in method signatures |
| `~>`  | Method invocation on a receiver value     |
| `~!`  | Shorthand for `self: unique Self`         |
| `~%`  | Shorthand for `self: partitioned Self`    |
| `~\|` | Shorthand for `self: concurrent Self`     |

> **Rationale:** The tilde (`~`) was chosen for its mnemonic association with "home" in Unix systems—representing the method's "home" (the receiver instance).

##### Static Semantics

**Maximal Munch Rule**

When multiple tokenizations are possible, the lexer MUST emit the longest valid token.

**Generic Argument Exception**

When parsing generic type arguments, the implementation MAY split `>>` into two `>` tokens while preserving the original lexeme for diagnostics.

##### Examples

**Valid:** Method declaration and dispatch:

```cursive
record Counter {
    value: i32,
    procedure get(~): i32 { result self.value }
    procedure increment(~!) { self.value = self.value + 1 }
}

let c = Counter { value: 0 }
c~>increment()
```

---

## 2.8 Literals

##### Definition

A **Literal** is a token representing a compile-time constant value. Cursive supports integer, floating-point, string, character, and boolean literals.

**Formal Definition**

$$\mathcal{L} = \mathcal{L}_\text{int} \cup \mathcal{L}_\text{float} \cup \mathcal{L}_\text{string} \cup \mathcal{L}_\text{char} \cup \mathcal{L}_\text{bool}$$

Each subset is defined by the grammar below.

##### Syntax & Declaration

**Integer Literals**

```ebnf
<integer_literal>  ::= <decimal_integer> | <hex_integer> 
                     | <octal_integer> | <binary_integer>
<decimal_integer>  ::= <dec_digit> ("_"* <dec_digit>)*
<hex_integer>      ::= "0x" <hex_digit> ("_"* <hex_digit>)*
<octal_integer>    ::= "0o" <oct_digit> ("_"* <oct_digit>)*
<binary_integer>   ::= "0b" <bin_digit> ("_"* <bin_digit>)*
```

**Floating-Point Literals**

```ebnf
<float_literal> ::= <decimal_integer> "." <decimal_integer>? <exponent>? <suffix>?
<exponent>      ::= ("e" | "E") ("+" | "-")? <decimal_integer>
```

**String Literals**

```ebnf
<string_literal>   ::= '"' (<string_char> | <escape_sequence>)* '"'
<escape_sequence>  ::= "\n" | "\r" | "\t" | "\\" | "\"" | "\'" | "\0"
                     | "\x" <hex_digit> <hex_digit>
                     | "\u{" <hex_digit>+ "}"
```

**Character Literals**

```ebnf
<char_literal> ::= "'" (<char_content> | <escape_sequence>) "'"
```

##### Static Semantics

Underscores in numeric literals MUST NOT appear at the beginning or end, immediately after a base prefix, adjacent to an exponent marker, or before a type suffix.

Based integer literals MUST begin with the appropriate prefix (`0x`, `0o`, `0b`) followed by at least one valid digit.

A decimal integer with a leading zero followed by additional digits MUST be accepted; implementations SHOULD emit warning `W-SRC-0301`.

A string literal MUST begin with `"` and end with a matching unescaped `"`. A line feed or end-of-file before the closing quote MUST trigger `E-SRC-0301`.

A character literal MUST represent exactly one Unicode scalar value. Empty or multi-value character literals MUST trigger `E-SRC-0303`.

Any escape sequence not in the set `{\n, \r, \t, \\, \", \', \0, \xHH, \u{H+}}` MUST trigger `E-SRC-0302`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                         | Detection    | Effect    |
| :----------- | :------- | :-------------------------------- | :----------- | :-------- |
| `E-SRC-0301` | Error    | Unterminated string literal.      | Compile-time | Rejection |
| `E-SRC-0302` | Error    | Invalid escape sequence.          | Compile-time | Rejection |
| `E-SRC-0303` | Error    | Invalid character literal.        | Compile-time | Rejection |
| `E-SRC-0304` | Error    | Malformed numeric literal.        | Compile-time | Rejection |
| `W-SRC-0301` | Warning  | Leading zeros in decimal literal. | Compile-time | N/A       |

---

## 2.9 Whitespace and Comments

##### Definition

**Whitespace** characters are code points that serve as token separators. **Comments** are lexical constructs for human-readable annotations that do not contribute to the token stream.

**Formal Definition**

$$\mathcal{W} = \{\text{U+0020}, \text{U+0009}, \text{U+000C}\}$$

Whitespace characters separate tokens but are not emitted. Line feed (U+000A) is emitted as a `<newline>` token.

##### Static Semantics

**Whitespace Characters**

| Character      | Code Point | Treatment                    |
| :------------- | :--------- | :--------------------------- |
| Space          | U+0020     | Token separator; not emitted |
| Horizontal Tab | U+0009     | Token separator; not emitted |
| Form Feed      | U+000C     | Token separator; not emitted |
| Line Feed      | U+000A     | Emitted as `<newline>` token |

**Line Comments**

A line comment begins with `//` outside a literal and extends to but does not include the next U+000A or end-of-file.

**Documentation Comments**

Comments beginning with `///` are declaration documentation comments. Comments beginning with `//!` are module documentation comments. Both MUST be preserved for association with declarations but MUST NOT appear as ordinary tokens.

**Block Comments**

A block comment is delimited by `/*` and `*/`. Block comments MUST nest: each `/*` increases nesting depth, each `*/` decreases it. End-of-file with non-zero nesting depth MUST trigger `E-SRC-0306`.

Characters within comments MUST NOT contribute to token formation, delimiter nesting, or statement termination.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------- | :----------- | :-------- |
| `E-SRC-0306` | Error    | Unterminated block comment. | Compile-time | Rejection |

---

## 2.10 Lexical Security

##### Definition

**Lexically Sensitive Characters** are Unicode code points that may alter the visual appearance of source code without changing its tokenization.

**Formal Definition**

$$\mathcal{S} = \{\text{U+202A–U+202E}\} \cup \{\text{U+2066–U+2069}\} \cup \{\text{U+200C}, \text{U+200D}\}$$

##### Static Semantics

When a character in $\mathcal{S}$ appears unescaped in an identifier, operator/punctuator, or adjacent to token boundaries outside literals and comments, the implementation MUST emit a diagnostic. Severity depends on conformance mode:

| Mode       | Diagnostic   | Effect   |
| :--------- | :----------- | :------- |
| Permissive | `W-SRC-0308` | Accepted |
| Strict     | `E-SRC-0308` | Rejected |

Lexically sensitive characters within string or character literals MUST NOT affect well-formedness.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `W-SRC-0308` | Warning  | Lexically sensitive Unicode character (Permissive). | Compile-time | N/A       |
| `E-SRC-0308` | Error    | Lexically sensitive Unicode character (Strict).     | Compile-time | Rejection |

---

## 2.11 Statement Termination

##### Definition

A **Statement Terminator** marks the end of a statement. Cursive supports explicit termination (`;`) and implicit termination (`<newline>`).

**Formal Definition**

$$\text{Terminates}(t, L) = \begin{cases}
\texttt{true} & \text{if } t = \texttt{`;'} \\
\texttt{true} & \text{if } t = \texttt{<newline>} \land \neg\text{Continues}(L) \\
\texttt{false} & \text{otherwise}
\end{cases}$$

##### Static Semantics

**Line Continuation Rules**

A `<newline>` does NOT terminate a statement ($\text{Continues}(L) = \texttt{true}$) when any of the following conditions hold:

| Condition            | Description                                                              |
| :------------------- | :----------------------------------------------------------------------- |
| Open Delimiter       | The newline appears inside an unclosed `()`, `[]`, or `{}`.              |
| Trailing Operator    | The last non-comment token on line $L$ is a binary operator or `,`.      |
| Leading Continuation | The first non-comment token on the following line is `.`, `::`, or `~>`. |

##### Examples

**Valid:** Line continuation via open delimiter:

```cursive
let result = (
    a + b + c
)
```

**Valid:** Line continuation via trailing operator:

```cursive
let sum = a +
    b + c
```

---

## 2.12 Translation Phases

##### Definition

A **Translation Phase** is a discrete stage in the compilation pipeline. A **Compilation Unit** is the collection of all normalized source files that contribute to a single module.

**Formal Definition**

$$\text{Translate} : \text{CompilationUnit} \to \text{Executable} \cup \{\bot\}$$

$$\text{Translate} = \text{CodeGen} \circ \text{TypeCheck} \circ \text{ComptimeExec} \circ \text{Parse}$$

##### Dynamic Semantics

Compilation MUST proceed through the following phases:

| Phase | Name                   | Description                                              |
| :---- | :--------------------- | :------------------------------------------------------- |
| 1     | Parsing                | Source → AST; all declarations recorded.                 |
| 2     | Compile-Time Execution | `comptime` blocks executed; may generate declarations.   |
| 3     | Type Checking          | Semantic validation including permissions and contracts. |
| 4     | Code Generation        | AST → intermediate representation → machine code.        |

##### Static Semantics

Phase 1 MUST complete for all source files in a compilation unit before Phase 2 begins. Forward references to declarations within the same compilation unit MUST be permitted.

Cursive provides no C-style textual inclusion (`#include`) or preprocessing (`#define`).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                    | Detection    | Effect    |
| :----------- | :------- | :--------------------------- | :----------- | :-------- |
| `E-NAM-1301` | Error    | Unresolved symbol reference. | Compile-time | Rejection |

---

## 2.13 Syntactic Nesting Limits

##### Definition

**Block Nesting Depth** is the count of lexically nested block statements. **Expression Nesting Depth** is the count of lexically nested expressions.

**Formal Definition**

For any syntactic position $p$:

$$\text{BlockDepth}(p) = |\{b : b \text{ is a block ancestor of } p\}| \leq \text{BLOCK\_LIMIT}$$

$$\text{ExprDepth}(p) = |\{e : e \text{ is an expression ancestor of } p\}| \leq \text{EXPR\_LIMIT}$$

where BLOCK_LIMIT and EXPR_LIMIT are the implementation's documented limits (minimum 256 per §1.4).

##### Constraints & Legality

Implementations MUST support at least the minimum nesting limits specified in §1.4.

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-SYN-0101` | Error    | Block nesting depth exceeded.      | Compile-time | Rejection |
| `E-SYN-0102` | Error    | Expression nesting depth exceeded. | Compile-time | Rejection |

---

## Clause 2 Cross-Reference Notes

**Terms defined in Clause 2 that MUST NOT be redefined elsewhere:**

| Term                          | Section | Description                                            |
| :---------------------------- | :------ | :----------------------------------------------------- |
| Source Text                   | §2.1    | Unicode scalar value sequence constituting input       |
| Source Byte Stream            | §2.1    | Raw byte sequence before decoding                      |
| Normalized Source File        | §2.1    | Unicode sequence after preprocessing                   |
| Logical Line                  | §2.2    | Code point sequence terminated by U+000A               |
| Line Ending Normalization     | §2.2    | Platform line ending conversion to LF                  |
| Preprocessing Pipeline        | §2.3    | 6-step transformation from bytes to tokens             |
| Token                         | §2.4    | Atomic unit of lexical analysis                        |
| Identifier                    | §2.5    | UAX #31 compliant naming sequence                      |
| Keyword                       | §2.6    | Reserved lexeme with grammatical meaning               |
| Operator                      | §2.7    | Symbol sequence denoting an operation                  |
| Punctuator                    | §2.7    | Symbol sequence for syntactic structure                |
| Literal                       | §2.8    | Token representing compile-time constant               |
| Escape Sequence               | §2.8    | Backslash-introduced special character                 |
| Whitespace                    | §2.9    | Token separator code points                            |
| Comment                       | §2.9    | Human-readable annotation                              |
| Lexically Sensitive Character | §2.10   | Code point altering appearance without changing tokens |
| Statement Terminator          | §2.11   | Lexical element marking statement end                  |
| Translation Phase             | §2.12   | Discrete compilation pipeline stage                    |
| Compilation Unit              | §2.12   | Source files constituting a single module              |
| Block Nesting Depth           | §2.13   | Count of nested block statements                       |
| Expression Nesting Depth      | §2.13   | Count of nested expressions                            |

**Terms referenced from other clauses:**

| Term                            | Source | Usage in Clause 2                        |
| :------------------------------ | :----- | :--------------------------------------- |
| Conformance Dossier             | §1.1   | Target for documenting IDB choices       |
| Implementation-Defined Behavior | §1.2   | Classification for normalization choices |
| Implementation Limits           | §1.4   | Size, line, nesting limits               |
| Ill-Formed                      | §1.2   | Programs violating static rules          |
| Permissive Mode                 | §1.2   | Affects lexical security severity        |
| Strict Mode                     | §1.2   | Affects lexical security severity        |

**Terms deferred to later clauses:**

| Term                                  | Deferred To | Reason                     |
| :------------------------------------ | :---------- | :------------------------- |
| Module                                | Clause 6    | Requires module system     |
| AST                                   | Clause 3    | Requires syntax definition |
| `comptime` Block                      | Clause 8    | Requires metaprogramming   |
| `self`, `Self`                        | Clause 4    | Requires type definitions  |
| `unique`, `partitioned`, `concurrent` | Clause 5    | Requires permission system |

---

# Clause 3: The Object and Memory Model

This clause defines the Cursive memory model: how objects are represented, how their lifetimes are tracked, and how memory safety is enforced.

---

## 3.1 Foundational Principles

##### Definition

**Memory Safety Axes**

Cursive's memory safety is decomposed into two orthogonal concerns:

1.  **Liveness**: The property that all pointers and bindings refer to allocated, initialized memory.

2.  **Aliasing**: The property that concurrent or reentrant access to the same memory location does not violate data integrity.

**Formal Definition**

Let $\mathcal{M}$ be the set of all memory locations. A program $P$ is **memory-safe** if and only if:

$$\text{Safe}(P) \iff \text{Liveness}(P) \land \text{Aliasing}(P)$$

where:

$$\text{Liveness}(P) \iff \forall \text{access } a \in P.\; \text{target}(a) \in \text{Live}(P, \text{time}(a))$$

$$\text{Aliasing}(P) \iff \forall t.\; \neg\exists \ell \in \mathcal{M}.\; \text{WriteConflict}(\ell, t)$$

##### Static Semantics

**Liveness Enforcement**

Liveness is enforced through RAII and deterministic destruction (§3.6), modal pointer states (§4.13), and region escape analysis (§3.3).

**Aliasing Enforcement**

Aliasing is enforced through permission types (§4.5) and the partitioning system (§3.7).

##### Constraints & Legality

**The Explicit-Over-Implicit Principle**

Memory operations affecting ownership, allocation, or synchronization MUST be syntactically explicit in source code:

| Operation          | Required Syntax                                             |
| :----------------- | :---------------------------------------------------------- |
| Region Allocation  | The `^` operator                                            |
| Heap Allocation    | Explicit allocator method call with `witness HeapAllocator` |
| Ownership Transfer | The `move` keyword at call sites for consuming parameters   |
| Synchronization    | Explicit primitives (`Mutex`, `Atomic`)                     |

**The Zero Runtime Overhead Principle**

All memory safety checks, except for dynamic array/slice bounds checks and explicitly requested dynamic contract verification (`[[verify(dynamic)]]`), MUST be resolved at compile time.

Conforming implementations MUST NOT insert reference counting, garbage collection barriers, dynamic lifetime tracking metadata, or hidden synchronization primitives in code paths that do not explicitly request such mechanisms.

---

## 3.2 The Object Model

##### Definition

An **Object** is the fundamental unit of typed storage in Cursive, defined by a quadruple:

$$\text{Object} ::= (\text{Storage}, \text{Type}, \text{Lifetime}, \text{Value})$$

**Storage Duration** classifies an object's lifetime based on when and how its storage is allocated and deallocated:

$$\text{StorageDuration} ::= \text{Static} \mid \text{Automatic} \mid \text{Region}(R) \mid \text{Dynamic}$$

##### Static Semantics

**Object Lifecycle**

Every object passes through exactly three phases:

1.  **Allocation and Initialization**: Storage is reserved and a valid initial value is written.
2.  **Usage**: The object's value may be read or modified via valid bindings or pointers, subject to permission constraints (§4.5).
3.  **Destruction and Deallocation**: If the object's type implements `Drop` (§6.5), its destructor is invoked. Storage is then released.

**Storage Duration Categories**

| Duration              | Allocation Trigger | Deallocation Trigger     |
| :-------------------- | :----------------- | :----------------------- |
| **Static**            | Program load       | Program termination      |
| **Automatic (Stack)** | Scope entry        | Scope exit (LIFO)        |
| **Region (Arena)**    | `^` operator       | Region block exit (bulk) |
| **Dynamic (Heap)**    | Allocator call     | Allocator call or RAII   |

Each storage duration category corresponds to a provenance tag as defined in §3.3.

##### Memory & Layout

The size, alignment, and internal layout of objects are governed by their types. Specific layout rules are defined in §4.4. Implementations MUST document their layout strategy in the Conformance Dossier (Appendix C).

---

## 3.3 The Provenance Model

##### Definition

A **Provenance Tag** ($\pi$) is a compile-time annotation attached to every pointer and reference type, indicating the origin of the address.

**Formal Definition**

$$\pi ::= \pi_{\text{Global}} \mid \pi_{\text{Stack}}(S) \mid \pi_{\text{Heap}} \mid \pi_{\text{Region}}(R) \mid \bot$$

where:
-   $\pi_{\text{Global}}$: Addresses of static storage.
-   $\pi_{\text{Stack}}(S)$: Addresses of automatic storage in scope $S$.
-   $\pi_{\text{Heap}}$: Addresses of dynamically allocated storage.
-   $\pi_{\text{Region}}(R)$: Addresses of storage allocated in region $R$.
-   $\bot$: No provenance (literals and constants).

**Lifetime Partial Order**

Provenance tags form a partial order ($\le$) based on containment:

$$\pi_{\text{Region}}(\text{Inner}) < \pi_{\text{Region}}(\text{Outer}) < \pi_{\text{Stack}}(S) < \pi_{\text{Heap}} \le \pi_{\text{Global}}$$

The relation $\pi_A \le \pi_B$ holds if and only if storage with provenance $\pi_A$ is guaranteed to be deallocated no later than storage with provenance $\pi_B$.

##### Static Semantics

**Provenance Inference Rules**

The compiler MUST infer provenance for all pointer and reference expressions:

**(P-Literal)**
$$\frac{c \text{ is a literal constant}}{\Gamma \vdash c : T, \bot}$$

**(P-Global)**
$$\frac{\Gamma \vdash \texttt{static } x : T}{\Gamma \vdash x : T, \pi_{\text{Global}}}$$

**(P-Local)**
$$\frac{\Gamma \vdash \texttt{let } x : T = e \quad x \text{ declared in scope } S}{\Gamma \vdash x : T, \pi_{\text{Stack}}(S)}$$

**(P-Region-Alloc)**
$$\frac{\Gamma \vdash e : T \quad R \text{ is the innermost active region}}{\Gamma \vdash \hat{\ } e : T, \pi_{\text{Region}}(R)}$$

**(P-Field)**
$$\frac{\Gamma \vdash e : \texttt{record } T, \pi \quad f \in \text{fields}(T)}{\Gamma \vdash e.f : \text{typeof}(f), \pi}$$

**(P-Index)**
$$\frac{\Gamma \vdash e : [T; N], \pi \quad \Gamma \vdash i : \texttt{usize}}{\Gamma \vdash e[i] : T, \pi}$$

**(P-Deref)**
$$\frac{\Gamma \vdash p : \texttt{Ptr}\langle T \rangle @\texttt{Valid}, \pi_p \quad \text{target}(p) \text{ has provenance } \pi_t}{\Gamma \vdash *p : T, \pi_t}$$

**The Escape Rule**

A value with provenance $\pi_e$ MUST NOT be assigned to a location with provenance $\pi_x$ if $\pi_e < \pi_x$:

$$\frac{\Gamma \vdash e : T, \pi_e \quad \Gamma \vdash x : T, \pi_x \quad \pi_e < \pi_x}{\text{ill-formed}: \texttt{E-MEM-3020}}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                             | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3020` | Error    | Value with shorter-lived provenance escapes to longer-lived location. | Compile-time | Rejection |

---

## 3.4 The Binding Model

##### Definition

A **Binding** is a named association between an identifier and an object. Bindings are introduced by `let` and `var` declarations, pattern matching, and procedure parameters.

A **Responsible Binding** is a binding that manages the lifecycle of its associated object. When a responsible binding goes out of scope, it triggers destruction of the object (§3.6).

**Binding State**

$$\text{BindingState} ::= \text{Uninitialized} \mid \text{Valid} \mid \text{Moved} \mid \text{PartiallyMoved}$$

A **Temporary Value** is an object resulting from expression evaluation that is not immediately bound to a named identifier.

##### Syntax & Declaration

```ebnf
<binding_decl>     ::= <let_decl> | <var_decl>
<let_decl>         ::= "let" <pattern> (":" <type>)? "=" <expression>
<var_decl>         ::= "var" <pattern> (":" <type>)? "=" <expression>
```

##### Static Semantics

**Binding Mutability**

`let` establishes an immutable binding; `var` establishes a mutable binding. Binding mutability is orthogonal to data permission (§4.5).

**Typing Rules**

**(T-Let)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{let } x = e : () \dashv \Gamma, x : T^{\text{Valid}}}$$

**(T-Var)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{var } x = e : () \dashv \Gamma, x : T^{\text{Valid}}_{\text{mut}}}$$

**State Tracking**

The compiler MUST track binding state through control flow. A binding's state is determined by its declaration, any `move` expressions consuming the binding, partial moves consuming fields, and reassignment of `var` bindings.

**Responsibility Establishment**

The assignment operator (`=`) establishes cleanup responsibility. The target binding becomes responsible for the assigned value. If the target was previously responsible for another value, that value is dropped first.

**Temporary Lifetime**

Temporaries have a lifetime extending from their creation to the end of the innermost enclosing statement. If a temporary is used to initialize a `let` or `var` binding, its lifetime is promoted to match the binding's scope. Temporaries that are not moved or bound MUST be destroyed at statement end, in reverse order of creation.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-MEM-3002` | Error    | Access to binding in Uninitialized state. | Compile-time | Rejection |
| `E-MEM-3003` | Error    | Reassignment of immutable binding.        | Compile-time | Rejection |

##### Examples

**Valid:** Binding state transitions:

```cursive
let a: unique Data = Data::new()     // 'a' is Valid
var b: unique Data = Data::new()     // 'b' is Valid
let c = move a                       // 'a' is Moved, 'c' is Valid
b = Data::new()                      // 'b' remains Valid (reassignment)
```

---

## 3.5 Responsibility and Move Semantics

##### Definition

A **Move** is an operation that transfers responsibility for an object from one binding to another. After a move, the source binding is invalidated and the destination binding becomes responsible for the object's lifecycle.

**Formal Definition**

$$\text{move} : \text{Binding}^{\text{Valid}} \to (\text{Binding}^{\text{Moved}}, \text{Value})$$

##### Syntax & Declaration

```ebnf
<move_expr>        ::= "move" <place_expr>
<partial_move>     ::= "move" <place_expr> "." <identifier>
```

##### Static Semantics

**Typing Rules**

**(T-Move)**
$$\frac{\Gamma \vdash x : T \quad \text{state}(\Gamma, x) = \text{Valid}}{\Gamma \vdash \texttt{move } x : T \dashv \Gamma[x \mapsto \text{Moved}]}$$

**(T-Move-Field)**
$$\frac{\Gamma \vdash x : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \} \quad \text{perm}(\Gamma, x) = \text{unique}}{\Gamma \vdash \texttt{move } x.f : T \dashv \Gamma[x \mapsto \text{PartiallyMoved}(f)]}$$

**Move Semantics**

`move x` transitions the source binding `x` to the Moved state. Access to a Moved binding is forbidden. A `var` binding in Moved state MAY be reassigned, restoring it to Valid state. A `let` binding in Moved state MUST NOT be reassigned.

**Partial Move Constraints**

Partial moves are permitted ONLY from bindings with `unique` permission or from mutable `var` bindings. While the parent is PartiallyMoved, accessing the moved field is an error, accessing other valid fields is permitted, and using the parent as a whole is an error. At scope exit, the compiler MUST generate cleanup only for valid fields; the parent's `Drop::drop` MUST NOT be called.

**Parameter Responsibility**

A **Transferring Parameter** is declared with the `move` modifier. The callee assumes responsibility; the caller MUST apply `move` at the call site; the source binding is invalidated after the call.

A **Non-Transferring Parameter** is declared without `move`. The callee receives a view; responsibility remains with the caller; the source binding remains valid.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3001` | Error    | Access to a binding in Moved or PartiallyMoved state.        | Compile-time | Rejection |
| `E-MEM-3004` | Error    | Partial move from `const` or `partitioned` binding.          | Compile-time | Rejection |
| `E-DEC-2411` | Error    | Missing `move` keyword at call site for consuming parameter. | Compile-time | Rejection |

##### Examples

**Valid:** Move and reassignment:

```cursive
var x: unique Data = Data::new()
let y = move x          // 'x' is Moved
x = Data::new()         // 'x' is Valid again
```

**Invalid:** Use of moved value triggers `E-MEM-3001`:

```cursive
let a = Data::new()
let b = move a
let c = a.field         // E-MEM-3001
```

---

## 3.6 Deterministic Destruction

##### Definition

**RAII (Resource Acquisition Is Initialization)** is the pattern where resources are acquired during object initialization and released during object destruction. Destruction in Cursive is deterministic—it occurs at a statically known program point.

**Drop Order** specifies that bindings MUST be destroyed in strict LIFO order relative to their declaration within a scope.

##### Dynamic Semantics

**Normal Destruction**

At scope exit, the compiler MUST generate code to destroy all responsible bindings in reverse declaration order:

1.  If the binding's type implements `Drop` (§6.5), invoke `Drop::drop`.
2.  Recursively destroy all fields and elements.
3.  Release storage.

**Unwinding**

During panic propagation, destructors for all in-scope responsible bindings MUST be invoked in LIFO order as each frame is unwound.

**Double Panic**

If a destructor panics while the thread is already unwinding, the runtime MUST abort the process immediately.

##### Constraints & Legality

Explicit calls to `Drop::drop` are forbidden. To destroy an object before scope exit, use the `mem::drop` utility procedure.

**Diagnostic Table**

| Code         | Severity | Condition                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------ | :----------- | :-------- |
| `E-MEM-3005` | Error    | Explicit call to `Drop::drop` method. | Compile-time | Rejection |

##### Examples

**Valid:** Early destruction via `mem::drop`:

```cursive
let file = File::open("data.txt")
mem::drop(move file)     // 'file' is destroyed here
```

---

## 3.7 The Partitioning System

##### Definition

The **Partitioning System** enables safe aliased mutability by verifying that concurrent mutable accesses target disjoint portions of data.

A **Partition** is a named or anonymous subset of a record's fields or a collection's indices that may be accessed independently from other partitions.

**Verification Modes** determine how disjointness is validated:

| Mode      | Description                                             | Runtime Cost |
| :-------- | :------------------------------------------------------ | :----------- |
| `static`  | Compiler proves disjointness using the verifier (§3.8)  | Zero         |
| `dynamic` | Runtime intersection check                              | Bounds check |
| `trusted` | Programmer assertion; no verification (UVB if violated) | Zero         |

##### Syntax & Declaration

**Record Partitioning**

```ebnf
<partition_decl>   ::= "partition" <identifier> "{" <field_decl>+ "}"
```

**Collection Partitioning**

```ebnf
<partition_stmt>   ::= "[[verify(" <verify_mode> ")]]"?
                       "partition" <expr> "by" "(" <range_list> ")"
                       ("where" "(" <predicate> ")")?
                       <block>
```

##### Static Semantics

**Typing Rule (T-Partition)**

$$\frac{\Gamma \vdash arr : \texttt{Array}[T, N] \quad \text{disjoint}(R_1, \ldots, R_k) \quad \text{perm}(\Gamma, arr) \in \{\text{unique}, \text{partitioned}\}}{\Gamma \vdash \texttt{partition } arr \texttt{ by } (R_1, \ldots, R_k) : () \dashv \Gamma, p_1 : \texttt{Slice}[T]^{\%}, \ldots, p_k : \texttt{Slice}[T]^{\%}}$$

The source MUST be a place expression with array or slice type. The source MUST have `unique` or `partitioned` permission. The ranges MUST be proven disjoint per the verification mode. The original binding becomes inaccessible within the partition block.

**Record Partitioning Rules**

When accessing a record via a `partitioned` binding, the compiler MUST track active partitions. Accessing a field acquires the partition containing that field. Simultaneous access to fields in the same partition is a compile-time error.

**Implicit Partitions**

Any field not declared within a named `partition` block belongs to a unique, anonymous partition.

**Disjointness Definition**

Two ranges $R_1 = [L_1, U_1)$ and $R_2 = [L_2, U_2)$ are disjoint if and only if $(U_1 \le L_2) \lor (U_2 \le L_1)$. The partition proof verifier (§3.8) validates this statically.

##### Dynamic Semantics

When `[[verify(dynamic)]]` is specified, the compiler injects code to compute the intersection. If non-empty, the runtime triggers a panic.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-MEM-3010` | Error    | Static partition conflict: overlapping access. | Compile-time | Rejection |
| `E-MEM-3012` | Error    | Partition contract proof failed.               | Compile-time | Rejection |
| `P-MEM-3013` | Panic    | Dynamic partition overlap.                     | Runtime      | Panic     |

##### Examples

**Valid:** Record partitioning:

```cursive
record World {
    partition physics { bodies: [Body], gravity: f64 }
    partition ai { agents: [Agent] }
    time: i64,
}

let w: partitioned World = ...
let p = w.bodies        // Locks 'physics'
let a = w.agents        // OK: 'ai' is disjoint
```

**Invalid:** Partition conflict triggers `E-MEM-3010`:

```cursive
let w: partitioned World = ...
let p = w.bodies        // Locks 'physics'
let g = w.gravity       // E-MEM-3010: 'physics' already locked
```

---

## 3.8 The Partition Proof Verifier

##### Definition

The **Partition Proof Verifier** is the static analysis pass responsible for validating partition disjointness.

**Canonical Linear Form** is the abstract domain for index expressions:

$$E ::= C_{\text{base}} + \sum_{i} (C_i \times V_i)$$

where $C_{\text{base}}, C_i \in \mathbb{Z}$ are constants and $V_i$ are symbolic variables. Non-linear terms result in `Unknown`.

##### Static Semantics

**Algorithm: Canonical Reduction**

```
Reduce(Literal(n))     = { base: n, terms: [] }
Reduce(Identifier(id)) = { base: 0, terms: [{coeff: 1, var: id}] }
Reduce(Add(lhs, rhs))  = AddLinear(Reduce(lhs), Reduce(rhs))
Reduce(Sub(lhs, rhs))  = AddLinear(Reduce(lhs), ScaleLinear(Reduce(rhs), -1))
Reduce(Mul(lhs, rhs))  =
    if Reduce(lhs).is_constant() then ScaleLinear(Reduce(rhs), Reduce(lhs).base)
    else if Reduce(rhs).is_constant() then ScaleLinear(Reduce(lhs), Reduce(rhs).base)
    else Unknown
Reduce(_)              = Unknown
```

**Algorithm: GatherFacts**

```
GatherFacts(Target):
    Facts = {}
    Node = Target.immediate_dominator
    while Node != null:
        match Node.statement:
            "let x = E":        Facts.add(Eq(x, Reduce(E)))
            "loop i in s..e":   Facts.add(Ge(i, Reduce(s))); Facts.add(Lt(i, Reduce(e)))
            "if C" (then):      Facts.add(ReduceCondition(C))
            "if C" (else):      Facts.add(ReduceCondition(Negate(C)))
        Node = Node.immediate_dominator
    return Facts
```

**Proof Evaluation**

To validate `partition arr by (A, B)`, the verifier reduces range bounds to linear forms, substitutes known facts, and checks if the disjointness inequality simplifies to a tautology.

**Hybrid Enforcement**

If the static proof relies on the absence of integer overflow, the compiler MUST inject a runtime assertion unless value-range analysis proves overflow impossible.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3014` | Error    | Expression not reducible to Canonical Linear Form. | Compile-time | Rejection |

Partition contract proof failures are diagnosed as `E-MEM-3012` per §3.7.

---

## 3.9 Regions and Arena Allocation

##### Definition

A **Region** is a lexical scope associated with a specific `Arena` instance, providing deterministic bulk memory allocation.

**Arena** is a built-in modal type (§4.8) with states:

$$\text{ArenaState} ::= @\text{Active} \mid @\text{Frozen} \mid @\text{Freed}$$

##### Syntax & Declaration

**Region Block**

```ebnf
<region_stmt>      ::= "region" <identifier> <block>
```

**Allocation Operator**

```ebnf
<alloc_expr>       ::= "^"+ <expression>
```

**Desugaring**

The statement `region r { body }` is equivalent to:

```cursive
{
    let r: unique Arena@Active = Arena::new()
    defer { r~>free() }
    { body }
}
```

##### Static Semantics

**Typing Rules**

**(T-Region)**
$$\frac{\Gamma, r : \texttt{Arena@Active}^{!} \vdash \textit{body} : T}{\Gamma \vdash \texttt{region } r \{ \textit{body} \} : T}$$

**(T-Alloc)**
$$\frac{\Gamma \vdash e : T \quad \text{region\_depth}(\Gamma) \ge n}{\Gamma \vdash \underbrace{\hat{} \cdots \hat{}}_{n} e : \texttt{Ptr}[T]^{!}_{\pi_{\text{Region}}(R_n)}}$$

**Caret Resolution**

A single `^` selects the lexically innermost active region. Each additional `^` selects the immediate parent region. The number of carets MUST NOT exceed the current region nesting depth.

**Escape Analysis**

Region pointers are subject to the escape rule (§3.3).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-MEM-3021` | Error    | Allocation operator `^` outside region block. | Compile-time | Rejection |
| `E-MEM-3022` | Error    | Caret depth exceeds region nesting depth.     | Compile-time | Rejection |

Region pointer escape is diagnosed as `E-MEM-3020` per §3.3.

##### Examples

**Valid:** Nested regions:

```cursive
region outer {
    let a = ^Point { x: 1.0, y: 2.0 }
    region inner {
        let b = ^Point { x: 3.0, y: 4.0 }
        let c = ^^Point { x: 5.0, y: 6.0 }  // Allocated in 'outer'
    }
    // 'b' destroyed; 'a' and 'c' valid
}
```

**Invalid:** Escape triggers `E-MEM-3020`:

```cursive
var escaped: Ptr<Point>
region r {
    escaped = ^Point { x: 1.0, y: 2.0 }  // E-MEM-3020
}
```

---

## 3.10 Unsafe Memory Operations

##### Definition

An **Unsafe Block** is a lexical scope in which the compiler suspends specific memory safety enforcement mechanisms.

##### Syntax & Declaration

```ebnf
<unsafe_block>     ::= "unsafe" <block>
<transmute_expr>   ::= "transmute" "::" "<" <type> "," <type> ">" "(" <expr> ")"
```

##### Static Semantics

**Typing Rules**

**(T-Unsafe)**
$$\frac{\Gamma \vdash_{\text{unsafe}} \textit{body} : T}{\Gamma \vdash \texttt{unsafe } \{ \textit{body} \} : T}$$

**(T-Transmute)**
$$\frac{\Gamma \vdash_{\text{unsafe}} e : S \quad \text{sizeof}(S) = \text{sizeof}(T)}{\Gamma \vdash_{\text{unsafe}} \texttt{transmute}::\langle S, T \rangle(e) : T}$$

**Suspended Checks**

Within an `unsafe` block, the compiler SHALL NOT enforce liveness, aliasing exclusivity, or partitioning disjointness.

**Enabled Operations**

The following operations are permitted ONLY within an `unsafe` block: raw pointer dereference, unsafe procedure calls, `transmute`, and pointer arithmetic. Raw pointer types (`*imm T`, `*mut T`) are defined in §4.13.

**Transmute Constraints**

`sizeof(S)` MUST equal `sizeof(T)`. The programmer MUST guarantee the bit pattern is valid for the target type; creating an invalid value constitutes Unverifiable Behavior (UVB).

##### Constraints & Legality

The use of an `unsafe` block constitutes a normative assertion by the programmer that the code within satisfies all memory safety invariants. The effects of `unsafe` code MUST NOT compromise the safety of safe code outside the block.

**Diagnostic Table**

| Code         | Severity | Condition                       | Detection    | Effect    |
| :----------- | :------- | :------------------------------ | :----------- | :-------- |
| `E-MEM-3030` | Error    | Unsafe operation outside block. | Compile-time | Rejection |
| `E-MEM-3031` | Error    | Transmute size mismatch.        | Compile-time | Rejection |

##### Examples

**Valid:** Transmute with matching sizes:

```cursive
procedure u32_to_bytes(value: u32): [u8; 4] {
    unsafe { result transmute::<u32, [u8; 4]>(value) }
}
```

**Invalid:** Unsafe operation outside block triggers `E-MEM-3030`:

```cursive
let ptr: *mut i32 = ...
let value = *ptr    // E-MEM-3030
```

---

## Clause 3 Cross-Reference Notes

**Terms defined in Clause 3 that MUST NOT be redefined elsewhere:**

| Term                   | Section | Description                                               |
| :--------------------- | :------ | :-------------------------------------------------------- |
| Object                 | §3.2    | Fundamental unit of typed storage                         |
| Storage Duration       | §3.2    | Classification of object lifetime                         |
| Provenance Tag         | §3.3    | Compile-time annotation indicating address origin         |
| Lifetime Partial Order | §3.3    | Ordering relation on provenance tags                      |
| Escape Rule            | §3.3    | Constraint preventing assignment across provenance bounds |
| Binding                | §3.4    | Named association between identifier and object           |
| Responsible Binding    | §3.4    | Binding managing lifecycle of its object                  |
| Binding State          | §3.4    | Valid, Moved, PartiallyMoved, or Uninitialized            |
| Temporary Value        | §3.4    | Unnamed intermediate expression result                    |
| Move                   | §3.5    | Operation transferring responsibility between bindings    |
| Transferring Parameter | §3.5    | Parameter declared with `move` modifier                   |
| RAII                   | §3.6    | Resource Acquisition Is Initialization pattern            |
| Drop Order             | §3.6    | LIFO destruction order                                    |
| Partition              | §3.7    | Named disjoint subset of fields or indices                |
| Verification Mode      | §3.7    | Static, Dynamic, or Trusted                               |
| Canonical Linear Form  | §3.8    | Abstract domain for partition index verification          |
| Region                 | §3.9    | Lexical scope with associated Arena                       |
| Arena                  | §3.9    | Modal type managing region memory lifecycle               |
| Unsafe Block           | §3.10   | Lexical scope with suspended safety enforcement           |
| `transmute`            | §3.10   | Bitwise reinterpretation intrinsic                        |

**Terms referenced from other clauses:**

| Term             | Source | Usage in Clause 3                          |
| :--------------- | :----- | :----------------------------------------- |
| Permission Types | §4.5   | Govern mutation and aliasing               |
| Modal Types      | §4.8   | Arena is a modal type                      |
| Pointer Types    | §4.13  | Raw pointers defined there; semantics here |
| Drop Trait       | §6.5   | Destructor mechanism                       |
| Place Expression | §8.1   | Required for `move` operand                |
| HeapAllocator    | §9.3   | Capability for dynamic allocation          |

**Terms deferred to later clauses:**

| Term    | Deferred To | Reason                        |
| :------ | :---------- | :---------------------------- |
| Panic   | §10         | Error handling semantics      |
| Abort   | §10         | Process termination semantics |
| FFI     | §12         | Foreign function interface    |
| Mutex   | §11         | Synchronization primitive     |
| Atomic  | §11         | Lock-free synchronization     |
| `defer` | §7          | Deferred execution statement  |

---

## Clause 4: Types and the Type System

---

### 4.1 Type System Architecture

##### Definition

The **type system** is the static semantic framework that assigns a type to every well-formed expression and validates the consistency of those types across the program. A **type** is a classification of values that determines their representation, the operations applicable to them, and the constraints governing their use.

**Formal Definition**

Let $\mathcal{T}$ denote the universe of all types. Let $\mathcal{V}$ denote the universe of all values. Each type $T \in \mathcal{T}$ defines a subset of values:

$$\text{Values}(T) \subseteq \mathcal{V}$$

Two types $T$ and $U$ are **equivalent**, written $T \equiv U$, if and only if they classify the same values under the equivalence rules defined in this section.

##### Static Semantics

**Static Type Checking**

Cursive MUST be implemented as a statically typed language. All type checking MUST be performed at compile time. A program that fails type checking MUST be diagnosed as ill-formed and rejected.

**Nominal vs. Structural Typing**

The type system is primarily **nominal**. Two types with different names are never equivalent, even if their structure is identical. The following constructs are nominal:

- Record types (§4.8)
- Enum types (§4.9)
- Modal types (§4.11)

The following constructs are **structural**. Their equivalence is determined by their composition:

- Tuple types (§4.7.1)
- Anonymous union types (§4.10)
- Function types (§4.14)
- Array and slice types (§4.7.2, §4.7.3)

**The Type Context ($\Gamma$)**

The **type context** is the environment mapping identifiers to types during type checking:

$$\Gamma ::= \emptyset \mid \Gamma, x : T$$

where $\emptyset$ denotes the empty context and $\Gamma, x : T$ extends context $\Gamma$ by binding identifier $x$ to type $T$. The judgment $\Gamma \vdash e : T$ asserts that expression $e$ has type $T$ in context $\Gamma$.

**Type Equivalence ($\equiv$)**

Two types $T$ and $U$ are equivalent, written $T \equiv U$, if one of the following rules holds. Type equivalence MUST be reflexive, symmetric, and transitive.

**(T-Equiv-Nominal)** Nominal Equivalence:
$$\frac{D(T) = D(U)}{\Gamma \vdash T \equiv U}$$

Two nominal types are equivalent if and only if they refer to the same declaration $D$.

**(T-Equiv-Tuple)** Tuple Equivalence:
$$\frac{T = (T_1, \ldots, T_n) \quad U = (U_1, \ldots, U_n) \quad \forall i \in 1..n,\ \Gamma \vdash T_i \equiv U_i}{\Gamma \vdash T \equiv U}$$

**(T-Equiv-Func)** Function Type Equivalence:
$$\frac{T = (P_1, \ldots, P_n) \to R_T \quad U = (Q_1, \ldots, Q_n) \to R_U \quad \Gamma \vdash R_T \equiv R_U \quad \forall i,\ \Gamma \vdash P_i \equiv Q_i}{\Gamma \vdash T \equiv U}$$

**(T-Equiv-Union)** Union Type Equivalence:
$$\frac{\text{multiset\_equiv}(\text{members}(T), \text{members}(U))}{\Gamma \vdash T \equiv U}$$

Union types are equivalent if their member type multisets are equal (order-independent).

**(T-Equiv-Permission)** Permission Equivalence:
$$\frac{P_1 = P_2 \quad \Gamma \vdash T \equiv U}{\Gamma \vdash P_1\ T \equiv P_2\ U}$$

Permissions are part of the type. `unique T` is not equivalent to `const T`.

**Type Categories**

Every well-formed type belongs to exactly one **type category**. The category determines applicable operations and runtime representation.

$$\text{TypeKind} : \text{Type} \to \mathcal{K}$$

| Category   | Section | Description                                    |
| :--------- | :------ | :--------------------------------------------- |
| Primitive  | §4.6    | Built-in scalar types (`i32`, `bool`, `f64`)   |
| Record     | §4.8    | Named product types with fields                |
| Enum       | §4.9    | Tagged sum types with variants                 |
| Union      | §4.10   | Structural anonymous sum types (`A \| B`)      |
| Modal      | §4.11   | State-machine types (`M@State`)                |
| Tuple      | §4.7.1  | Structural product types `(T, U)`              |
| Array      | §4.7.2  | Fixed-size homogeneous sequences `[T; N]`      |
| Slice      | §4.7.3  | Dynamically-sized views `[T]`                  |
| Pointer    | §4.13   | Safe (`Ptr<T>`) and raw (`*imm T`) pointers    |
| Procedure  | §4.14   | Callable signatures `(T) -> U`                 |
| Generic    | §4.15   | Uninstantiated type parameters                 |
| Refinement | §4.19   | Types with predicate constraints `T where {P}` |
| String     | §4.12   | Built-in string types (`string@Managed`)       |

##### Constraints & Legality

Top-level declarations (procedures, module-scope bindings, type definitions) MUST include explicit type annotations. Local bindings within procedure bodies MAY omit type annotations when the type is inferable (§4.4).

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1501` | Error    | Type mismatch in expression or assignment.        | Compile-time | Rejection |
| `E-TYP-1502` | Error    | No valid type derivable for expression.           | Compile-time | Rejection |
| `E-TYP-1503` | Error    | Operation not supported for operand type.         | Compile-time | Rejection |
| `E-TYP-1505` | Error    | Missing required type annotation at module scope. | Compile-time | Rejection |

##### Examples

**Valid:** Nominal type distinction:

```cursive
record Point2D { x: f64, y: f64 }
record Vector2D { x: f64, y: f64 }

let p: Point2D = Point2D { x: 1.0, y: 2.0 }
let v: Vector2D = Vector2D { x: 1.0, y: 2.0 }

// p and v have identical structure but different types
// let q: Point2D = v   // E-TYP-1501: type mismatch
```

**Valid:** Structural tuple equivalence:

```cursive
let a: (i32, bool) = (42, true)
let b: (i32, bool) = a   // OK: tuples are structural
```

**Invalid:** Missing type annotation triggers `E-TYP-1505`:

```cursive
// At module scope
static let CONFIG = load_config()   // E-TYP-1505: type required
```

---

### 4.2 Subtyping and Coercion

##### Definition

The **subtype relation** is a partial order on types that determines when a value of one type may be used where a value of another type is expected. A type $S$ is a **subtype** of type $T$, written $S <: T$, if a value of type $S$ may be safely used in any context requiring a value of type $T$.

**Coercion** is the implicit conversion that occurs when an expression of a subtype is used in a context expecting a supertype.

**Formal Definition**

The subtype relation $<:$ is a binary relation on types:

$$<:\ \subseteq\ \mathcal{T} \times \mathcal{T}$$

The relation MUST be reflexive ($T <: T$ for all $T$) and transitive (if $S <: T$ and $T <: U$, then $S <: U$).

##### Static Semantics

**Coercion Rule**

When an expression has a subtype of the expected type, implicit coercion occurs:

$$\frac{\Gamma \vdash e : S \quad \Gamma \vdash S <: T}{\Gamma \vdash e : T} \quad \text{(T-Coerce)}$$

**Permission Subtyping**

A type with a stronger permission is a subtype of the same base type with a weaker permission:

$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{const}\ T} \quad \frac{}{\Gamma \vdash \texttt{partitioned}\ T <: \texttt{const}\ T} \quad \text{(Sub-Perm)}$$

The complete permission lattice is defined in §4.5. The subtyping relationships are:

- `unique T` is a subtype of `partitioned T`, `concurrent T`, and `const T`
- `partitioned T` is a subtype of `const T`
- `concurrent T` is a subtype of `const T`

**Sibling Incompatibility**

The permissions `partitioned` and `concurrent` are incompatible siblings. Neither is a subtype of the other:

$$\Gamma \nvdash \texttt{partitioned}\ T <: \texttt{concurrent}\ T \qquad \Gamma \nvdash \texttt{concurrent}\ T <: \texttt{partitioned}\ T$$

**Trait Implementation Subtyping**

A concrete type that implements a trait is a subtype of that trait:

$$\frac{\Gamma \vdash T\ \texttt{implements}\ \textit{Tr}}{\Gamma \vdash T <: \textit{Tr}} \quad \text{(Sub-Trait)}$$

**Additional Subtyping Rules**

The following subtyping relationships are defined in their respective sections:

- Modal widening ($M@S <: M$): §4.11
- Union member subtyping: §4.10
- Refinement type subtyping: §4.19

**No Implicit Upgrade**

A weaker permission MUST NOT be implicitly coerced to a stronger permission. `const T` is not a subtype of `unique T`, `partitioned T`, or `concurrent T`.

##### Constraints & Legality

A coercion MUST NOT introduce Unverifiable Behavior (UVB).

An implementation MUST reject any assignment or argument where the source type is not a subtype of the target type.

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1510` | Error    | Source type is not a subtype of target type.       | Compile-time | Rejection |
| `E-TYP-1511` | Error    | Implicit permission upgrade attempted.             | Compile-time | Rejection |
| `E-TYP-1512` | Error    | Coercion between incompatible sibling permissions. | Compile-time | Rejection |

---

### 4.3 Variance

##### Definition

**Variance** specifies how subtyping of type arguments relates to subtyping of parameterized types. For a generic type constructor $F$ with parameter $T$, variance determines whether $F[A] <: F[B]$ when $A <: B$, when $B <: A$, both, or neither.

**Formal Definition**

The variance of a type parameter is one of four polarities:

| Polarity      | Symbol | Condition for $F[A] <: F[B]$ |
| :------------ | :----- | :--------------------------- |
| Covariant     | $+$    | $A <: B$                     |
| Contravariant | $-$    | $B <: A$                     |
| Invariant     | $=$    | $A \equiv B$                 |
| Bivariant     | $\pm$  | Always (parameter unused)    |

##### Static Semantics

**Variance Determination**

The variance of a type parameter is determined by its usage within the type definition:

- **Covariant ($+$):** Parameter appears only in output positions (return types, immutable fields).
- **Contravariant ($-$):** Parameter appears only in input positions (parameter types).
- **Invariant ($=$):** Parameter appears in both input and output positions, or in mutable storage.
- **Bivariant ($\pm$):** Parameter does not appear in the type's structure.

**Generic Subtyping Rule**

For a generic type $\textit{Name}[T_1, \ldots, T_n]$ with parameter polarities $P_1, \ldots, P_n$:

$$\frac{\forall i.\ \text{variance\_check}(P_i, A_i, B_i)}{\Gamma \vdash \textit{Name}[A_1, \ldots, A_n] <: \textit{Name}[B_1, \ldots, B_n]} \quad \text{(Sub-Generic)}$$

where $\text{variance\_check}(P, A, B)$ holds iff:
- $P = +$ and $\Gamma \vdash A <: B$
- $P = -$ and $\Gamma \vdash B <: A$
- $P = {=}$ and $\Gamma \vdash A \equiv B$
- $P = \pm$

**Function Type Variance**

Function types exhibit contravariant parameters and covariant return types:

$$\frac{\Gamma \vdash U <: T \quad \Gamma \vdash R_1 <: R_2}{\Gamma \vdash (T) \to R_1 <: (U) \to R_2} \quad \text{(Var-Func)}$$

A function accepting a supertype may substitute for one accepting a subtype. A function returning a subtype may substitute for one returning a supertype.

**Permission Interaction**

The `const` permission enables covariant treatment of otherwise invariant container types:

$$\frac{\Gamma \vdash A <: B}{\Gamma \vdash \texttt{const}\ C[A] <: \texttt{const}\ C[B]} \quad \text{(Var-Const)}$$

This rule applies because `const` prohibits mutation, eliminating the safety concern that motivates invariance. The rule does not apply to `unique` or `partitioned` permissions:

$$\frac{A \not\equiv B}{\Gamma \nvdash \texttt{unique}\ C[A] <: \texttt{unique}\ C[B]}$$

##### Constraints & Legality

An implementation MUST reject a subtyping judgment that violates variance rules.

When a type parameter has invariant variance, the implementation MUST require exact type equivalence for that parameter in subtyping checks.

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1520` | Error    | Variance violation in generic type instantiation.   | Compile-time | Rejection |
| `E-TYP-1521` | Error    | Invariant type parameter requires exact type match. | Compile-time | Rejection |

---

### 4.4 Bidirectional Type Inference

##### Definition

**Type inference** is the process by which the compiler derives type information not explicitly annotated in the source program. Cursive employs **bidirectional type inference**, combining two complementary modes of type propagation.

**Formal Definition**

Bidirectional type inference defines two judgment forms:

| Judgment                        | Name      | Meaning                                                  |
| :------------------------------ | :-------- | :------------------------------------------------------- |
| $\Gamma \vdash e \Rightarrow T$ | Synthesis | Type $T$ is derived from the structure of expression $e$ |
| $\Gamma \vdash e \Leftarrow T$  | Checking  | Expression $e$ is validated against expected type $T$    |

In synthesis mode, the type flows outward from the expression. In checking mode, an expected type flows inward from context to guide type derivation for subexpressions.

##### Static Semantics

**Synthesis Rules**

Variable lookup synthesizes the type from the context:

$$\frac{(x : T) \in \Gamma}{\Gamma \vdash x \Rightarrow T} \quad \text{(Synth-Var)}$$

Tuple construction synthesizes component types:

$$\frac{\Gamma \vdash e_1 \Rightarrow T_1 \quad \Gamma \vdash e_2 \Rightarrow T_2}{\Gamma \vdash (e_1, e_2) \Rightarrow (T_1, T_2)} \quad \text{(Synth-Tuple)}$$

Function application synthesizes the return type while checking arguments:

$$\frac{\Gamma \vdash f \Rightarrow (T_1, \ldots, T_n) \to R \quad \forall i.\ \Gamma \vdash a_i \Leftarrow T_i}{\Gamma \vdash f(a_1, \ldots, a_n) \Rightarrow R} \quad \text{(Synth-App)}$$

**Checking Rules**

Synthesis satisfies checking when the synthesized type is a subtype of the expected type:

$$\frac{\Gamma \vdash e \Rightarrow S \quad \Gamma \vdash S <: T}{\Gamma \vdash e \Leftarrow T} \quad \text{(Check-Sub)}$$

Closures check against an expected function type, propagating parameter types inward:

$$\frac{\Gamma, x : T_1 \vdash e \Leftarrow T_2}{\Gamma \vdash |x|\ e \Leftarrow (T_1) \to T_2} \quad \text{(Check-Lambda)}$$

**Annotation Rule**

An explicit type annotation switches from checking to synthesis:

$$\frac{\Gamma \vdash e \Leftarrow T}{\Gamma \vdash (e : T) \Rightarrow T} \quad \text{(Synth-Annot)}$$

**Locality Requirement**

Type inference MUST be local: inference operates within a single procedure body and MUST NOT propagate type information across procedure boundaries.

**Mandatory Annotations**

The following positions MUST have explicit type annotations:

- Procedure parameter types
- Procedure return types
- Public and internal module-scope bindings

**Permitted Omissions**

The following positions MAY omit type annotations when the type is inferable:

- Local bindings within procedure bodies (`let`, `var`)
- Closure parameters when the closure is checked against a known function type

##### Constraints & Legality

An implementation MUST reject a program if inference fails to derive a unique type for any expression.

An implementation MUST NOT infer types for public API boundaries.

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1505` | Error    | Missing required type annotation at module scope. | Compile-time | Rejection |
| `E-TYP-1530` | Error    | Type inference failed; unable to determine type.  | Compile-time | Rejection |

---

### 4.5 Permission Types {Source: Draft 2 §16; Amendment 1}

##### Definition

**Permission**

A permission is a **type qualifier** that governs how the **data** referenced by a binding may be accessed, mutated, and aliased. Permissions are fundamental to the language's memory safety guarantees. If no permission is explicitly written, `const` is the default.

##### The Orthogonality Principle

The Permission System is **strictly orthogonal** to two other concepts that are often conflated in other languages:

1. **Binding Mutability** (§4.18): Whether a binding can be re-assigned to a different value.
   - Controlled by `let` (immutable binding) vs `var` (mutable binding)
   - Orthogonal to permissions: a `let` binding may hold `unique` data; a `var` binding may hold `const` data

2. **Cleanup Responsibility** (§3.4-§3.5): Which binding is responsible for destroying an object.
   - Controlled **exclusively** by the `move` keyword
   - Permissions do **NOT** affect responsibility—`unique` does not mean "responsible for cleanup"

**Comparison with Rust:**

In Rust, these concepts are partially conflated:
- `T` (owned) = exclusive access + cleanup responsibility
- `&mut T` = exclusive access + NO cleanup responsibility
- `&T` = shared read access + NO cleanup responsibility

Rust cannot directly express "exclusive mutable access where the caller retains cleanup responsibility" without additional wrapper types.

In Cursive, these axes are fully independent:

| Cursive              | Permission        | Cleanup Responsibility |
| :------------------- | :---------------- | :--------------------- |
| `unique T` (no move) | Exclusive mutable | Caller retains         |
| `move x: unique T`   | Exclusive mutable | Callee takes           |
| `const T`            | Read-only         | Caller retains         |
| `move x: const T`    | Read-only         | Callee takes           |

This orthogonality enables patterns that are awkward or impossible in Rust:

```cursive
// Callee gets exclusive access but caller remains responsible for cleanup
procedure transform_in_place(data: unique Buffer) {
    // Can mutate data freely
    data.bytes[0] = 0xFF
}   // data goes out of scope but is NOT dropped—caller still responsible

// Callee takes responsibility for read-only data
procedure consume_readonly(move data: const Config) {
    // Can only read data, but will drop it when done
    log(data.name)
}   // data IS dropped here—callee took responsibility via move
```

**Key Insight:** In Cursive, `unique` answers "who can access this right now?" while `move` answers "who must clean this up?" These are separate questions with separate answers.

**The Permission Lattice**

The permission system consists of four permissions forming a directed lattice:

$$\text{unique} <: \{\text{partitioned}, \text{concurrent}\} <: \text{const}$$

| Permission        | Access               | Aliasing                     | Mutation             | Requirement                |
| :---------------- | :------------------- | :--------------------------- | :------------------- | :------------------------- |
| `const` (default) | Read-only            | Unlimited                    | Forbidden            | —                          |
| `partitioned`     | Disjoint mutable     | Multiple (verified disjoint) | Permitted            | Partitioning System (§3.7) |
| `concurrent`      | Synchronized mutable | Multiple (synchronized)      | Permitted (interior) | Synchronized methods       |
| `unique`          | Exclusive mutable    | None                         | Permitted            | —                          |

##### Permission Semantics (Detailed)

**`const`** — Immutable/Read-Only Access
- The data cannot be mutated through this path
- Unlimited aliasing is permitted (many `const` references may coexist)
- This is the default when no permission is specified
- Analogous to `&T` in Rust (but without lifetime annotations)

**`unique`** — Exclusive Mutable Access
- The data may be mutated through this path
- No other live references (of any permission) may exist to this data
- Does **NOT** imply cleanup responsibility (see Orthogonality Principle above)
- Analogous to having sole access, but responsibility is tracked separately

**`partitioned`** — Spatially-Shared Mutable Access
- The data may be mutated through this path
- Other `partitioned` references may coexist if they access **disjoint** portions
- Requires compile-time or runtime proof of disjointness (§3.7)
- Enables parallel mutation of different array slices or record fields

**`concurrent`** — Temporally-Shared Mutable Access
- The data may be mutated through synchronized methods only
- Other `concurrent` references may coexist (synchronization prevents races)
- Direct field mutation is forbidden; must use type's synchronized methods
- The type must provide methods accepting `concurrent self` (or `~|`)

##### Static Semantics

**Subtyping (Downgrading)**

A value with a stronger permission **MAY** be used in a context expecting a weaker permission. This is an implicit coercion:

- `unique` coerces to `partitioned`, `concurrent`, or `const`.
- `partitioned` coerces to `const`.
- `concurrent` coerces to `const`.

**Sibling Incompatibility**

`partitioned` and `concurrent` are **incompatible siblings**:

- `partitioned` (spatial disjointness) **MUST NOT** coerce to `concurrent` (temporal synchronization) because `partitioned` guarantees no overlap, while `concurrent` implies overlap managed by locks.
- `concurrent` **MUST NOT** coerce to `partitioned` because `concurrent` implies shared state that cannot be statically split.

**`concurrent` Permission Operations**

The `concurrent` permission governs what operations are permitted on a reference. Unlike `unique` or `partitioned`, `concurrent` does **not** permit direct field mutation:

| Operation                                        | Permitted? |
| :----------------------------------------------- | :--------- |
| Field read                                       | Yes        |
| Field mutation                                   | **No**     |
| Method call (accepts `concurrent self` or `~\|`) | Yes        |
| Method call (accepts `const self` or `~`)        | Yes        |
| Method call (requires `unique` or `partitioned`) | No         |

**User-Defined Concurrent Types**

A type is usable with `concurrent` permission when it provides methods accepting `self: concurrent` (or the shorthand `~|`). Such methods **MUST** use internal synchronization primitives (`Mutex`, `Atomic`, etc.) to ensure thread-safe mutation.

**Concurrent-Safe Method Rule**

A method $m$ on type $T$ is callable through a `concurrent` reference iff the method's receiver accepts `concurrent` or a weaker permission (`const`):

$$\frac{\Gamma \vdash m : T.\text{method} \quad m.\text{receiver} \in \{\texttt{concurrent}, \texttt{const}\}}{\Gamma \vdash \texttt{concurrent } T \vdash m \text{ callable}}$$

**Rationale**: Thread safety is enforced through the permission system and method signatures, not marker traits. Types opt-in to concurrent usage by providing synchronized methods.

**No Upgrade**

A value with a weaker permission **MUST NOT** be implicitly coerced to a stronger permission. `const` **MUST NOT** coerce to `unique`, `partitioned`, or `concurrent`.

**Exclusion Principle**

- **`unique`**: Valid **ONLY** if no other live permissions (read or write) exist to the same object. The compiler **MUST** reject the formation of any alias to an object while a `unique` path to that object remains active.
- **`partitioned`**: Allows multiple mutable aliases (disjoint) but forbids `unique`.
- **`concurrent`**: Allows multiple mutable aliases (synchronized) but forbids `unique`.
- **`const`**: Allows multiple read-only aliases but forbids `unique`, `partitioned`, or `concurrent` coexistence.

**Downgrading Scope**

A `unique` type **MAY** be temporarily downgraded to `const` or `partitioned` for a bounded scope (e.g., passing to a procedure). During this scope, the original `unique` path is considered **inactive** and cannot be used. When the downgrade scope ends, the original path regains `unique` status.

##### Responsibility vs Permission: A Clarifying Example

Consider a procedure that needs temporary exclusive access to modify data:

```cursive
record Document {
    content: string@Managed,
    modified: bool,
}

// Permission: unique (exclusive access)
// Responsibility: caller retains (no 'move' on parameter)
procedure mark_modified(doc: unique Document) {
    doc.modified = true
}

// Permission: unique (exclusive access)
// Responsibility: callee takes (has 'move' on parameter)
procedure archive(move doc: unique Document) {
    save_to_disk(doc)
}   // doc is dropped here

public procedure main(ctx: Context): i32 {
    var doc = Document {
        content: "Hello",
        modified: false
    }

    // doc is temporarily inaccessible during this call
    mark_modified(doc)      // No 'move' at call site—we keep responsibility

    // doc is accessible again; we still own it
    if doc.modified {
        archive(move doc)   // 'move' at call site—transferring responsibility
    }

    // doc is now invalid (moved)
    // doc.content          // ERROR: use of moved value

    result 0
}
```

This separation allows fine-grained control over both access patterns and resource lifetimes.

##### Constraints & Legality

| Code         | Severity | Condition                                                           |
| :----------- | :------- | :------------------------------------------------------------------ |
| `E-TYP-1601` | Error    | Attempt to mutate data via a `const` reference.                     |
| `E-TYP-1602` | Error    | Violation of `unique` exclusion (aliasing detected).                |
| `E-TYP-1603` | Error    | Partitioning system violation.                                      |
| `E-TYP-1604` | Error    | Field mutation through `concurrent` reference (use method instead). |
| `E-TYP-1605` | Error    | Method requires stronger permission than `concurrent` provides.     |

---

### 4.6 Primitive Types {Source: Draft 2 §17}

##### Definition

Primitive types are the built-in scalar types that form the basis for all other types and represent the fundamental units of data.

- **Integer types** (`iN`/`uN`): Fixed-width integers. Signed integers **MUST** use two's complement representation.
- **Floating-point types** (`fN`): IEEE 754 binary floating-point numbers.
- **Character type** (`char`): A Unicode Scalar Value (U+0000–U+D7FF, U+E000–U+10FFFF).
- **Boolean type** (`bool`): Logical true/false.
- **Unit type** (`()`): The type with exactly one value, zero size.
- **Never type** (`!`): The uninhabited type; an expression of type `!` never produces a value.

##### Syntax & Declaration

**Integers**:
- Sized: `i8`, `i16`, `i32`, `i64`, `i128`, `u8`, `u16`, `u32`, `u64`, `u128`
- Pointer-sized: `isize`, `usize`
- Aliases: `int` (`i32`), `uint` (`u32`) — fully equivalent in all contexts

**Floats**:
- `f16`, `f32`, `f64`
- Aliases: `half` (`f16`), `float` (`f32`), `double` (`f64`) — fully equivalent in all contexts

**Others**: `bool`, `char`, `()`, `!`

##### Memory & Layout

Implementations **MUST** match target platform pointer width for `isize`, `usize`, and `Ptr`.

| Type                     | Size (bytes)  | Alignment (bytes) | Representation                                                             |
| :----------------------- | :------------ | :---------------- | :------------------------------------------------------------------------- |
| `i8`/`u8`/`bool`         | 1             | 1                 | —                                                                          |
| `i16`/`u16`/`f16`        | 2             | 2                 | —                                                                          |
| `i32`/`u32`/`f32`/`char` | 4             | 4                 | Two's complement (signed); IEEE 754 (float); Unicode Scalar Value (`char`) |
| `i64`/`u64`/`f64`        | 8             | 8                 | —                                                                          |
| `i128`/`u128`            | 16            | 8 or 16 (IDB)     | —                                                                          |
| `isize`/`usize`          | Platform word | Platform word     | —                                                                          |
| `()`                     | 0             | 1                 | Zero-sized type                                                            |
| `!`                      | 0             | 1                 | Uninhabited type                                                           |

##### Dynamic Semantics

**Integer Overflow**

Implementations **MUST** provide a checked mode in which signed and unsigned integer overflow triggers a **panic**. Behavior in release mode is **IDB**.

##### Constraints & Legality

| Code         | Severity | Condition                                     |
| :----------- | :------- | :-------------------------------------------- |
| `W-TYP-1701` | Warning  | `f16` arithmetic emulated on target platform. |

---

### 4.7 Composite Types {Source: Draft 2 §18.1, 18.5-18.7}

#### 4.7.1 Tuples

##### Definition

A **tuple** is an ordered, fixed-length sequence of heterogeneous types. Tuples have **structural** equivalence.

##### Syntax & Declaration

**Type**: `(T1, T2, ..., Tn)`

**Literal**: `(v1, v2, ..., vn)`

**Access**: Index-based only (e.g., `t.0`, `t.1`).

##### Static Semantics

**(T-Equiv-Tuple)** Equivalence:
$$(T_1, \ldots, T_n) \equiv (U_1, \ldots, U_m) \text{ iff } n = m \text{ and } \forall i \in 1..n,\ T_i \equiv U_i$$

##### Memory & Layout

Structural layout. Field order is preserved. Padding is IDB.

---

#### 4.7.2 Arrays

##### Definition

An **array** is a contiguous, fixed-length sequence of homogeneous elements.

##### Syntax & Declaration

**Type**: `[T; N]` where `N` is a compile-time constant expression.

**Literals**:
- List form: `[e1, e2, ..., en]`
- Repeat form: `[e; N]` (value `e` duplicated `N` times)

##### Memory & Layout

Contiguous storage. No inter-element padding. Size = `N * sizeof(T)`.

##### Constraints & Legality

- Index expression **MUST** have type `usize`.
- Runtime bounds checking is **REQUIRED**.

| Code         | Severity    | Condition                  |
| :----------- | :---------- | :------------------------- |
| `E-TYP-1801` | Error/Panic | Array index out of bounds. |

---

#### 4.7.3 Slices

##### Definition

A **slice** is a dynamically-sized view into a contiguous sequence. It is represented as a **fat pointer** containing a data pointer and a length.

##### Memory & Layout

```
struct { ptr: *imm T, len: usize }
```

Two machine words. The slice does not own its data; it borrows from an array or other contiguous storage.

---

#### 4.7.4 Range Types

##### Definition

**Range types** are structural record types produced by range expressions (`..`, `..=`, etc.).

##### Syntax & Declaration

Implementations **MUST** provide the following generic records with **public fields**:

| Expression    | Type                  | Fields               |
| :------------ | :-------------------- | :------------------- |
| `start..end`  | `Range<T>`            | `start: T`, `end: T` |
| `start..=end` | `RangeInclusive<T>`   | `start: T`, `end: T` |
| `start..`     | `RangeFrom<T>`        | `start: T`           |
| `..end`       | `RangeTo<T>`          | `end: T`             |
| `..=end`      | `RangeToInclusive<T>` | `end: T`             |
| `..`          | `RangeFull`           | (none)               |

##### Constraints & Legality

Range types **MUST** implement `Copy` if and only if `T` implements `Copy`.

---

### 4.8 Records {Source: Draft 2 §18.2, §23.3}

##### Definition

A **record** is a nominal product type with named fields.

##### Syntax & Declaration

```ebnf
record_decl ::= [visibility] "record" identifier [generic_params]
                "{" field_list "}" [type_invariant]
field_decl  ::= [visibility] identifier ":" type
field_list  ::= (field_decl ("," field_decl)* ","?)?
type_invariant ::= "where" "{" predicate "}"
```

**Literal**: `TypeName { field1: v1, field2: v2 }`

**Field Shorthand**: `{ field }` is equivalent to `{ field: field }`.

**Type Invariant**: Records may include a `where` clause after the field list to specify invariants that must hold for all valid instances. Within the predicate, `self` refers to the record instance.

##### Examples

```cursive
// Simple record (no invariant)
record Point { x: f64, y: f64 }

// Record with type invariant
record PositiveInt {
    value: i32
} where { self.value > 0 }

// Record with multi-constraint invariant
record BoundedCounter {
    value: i32,
    max: i32
} where { self.value >= 0 && self.value <= self.max }
```

##### Static Semantics

- **Nominal equivalence**: Two record types are equivalent if and only if they refer to the same declaration.
- **Default field visibility**: `private` (module-local) unless explicitly marked `public` or `internal`.
- **Invariant enforcement**: Type invariants are verified at construction time and after any mutating operation. See §7.3 for enforcement details.

##### Memory & Layout

Field ordering and padding are **IDB** unless `[[repr(C)]]` is specified.

---

### 4.9 Enums {Source: Draft 2 §18.3}

##### Definition

**Enum**

An **enum** is a nominal sum type (tagged union) with named variants.

**Discriminant**

A **discriminant** is an integer value identifying the active variant of an enum.

##### Syntax & Declaration

```ebnf
enum_decl ::= [visibility] "enum" identifier [generic_params]
              "{" variant_list "}" [type_invariant]
variant   ::= identifier [payload] ["=" integer_constant]
payload   ::= "(" type_list ")" | "{" field_list "}"
type_invariant ::= "where" "{" predicate "}"
```

**Type Invariant**: Enums may include a `where` clause after the variant list to specify invariants. Within the predicate, `self` refers to the enum instance and may be matched to access variant-specific data.

##### Examples

```cursive
// Simple enum (no invariant)
enum Option<T> {
    None,
    Some(T)
}

// Enum with invariant (all variants must satisfy)
enum NonEmptyResult<T, E> {
    Ok(T),
    Err(E)
} where {
    match self {
        Ok(v) => v~>len() > 0,
        Err(_) => true
    }
}
```

##### Static Semantics

- Explicit discriminants **MUST** be compile-time integer constants.
- Unspecified discriminants are assigned sequentially (0, or previous + 1).
- Duplicate discriminant values are **forbidden**.
- **Invariant enforcement**: Type invariants are verified at construction time and after any mutating operation. See §7.3 for enforcement details.

##### Memory & Layout

**Size**: `sizeof(Discriminant) + sizeof(LargestVariant) + Padding`

**Alignment**: `max(alignof(Discriminant), alignof(LargestVariant))`

**Niche Optimization**: Implementations **SHOULD** apply niche optimization where payload bit-patterns allow elision of the separate discriminant.

---

### 4.10 Union Types {Source: Draft 2 §18.4}

##### Definition

A **union type** is a structural anonymous sum type (`A | B`). Union types are safe and tagged.

##### Static Semantics

**Equivalence**

Set-based (order independent): `A | B ≡ B | A`.

**Subtyping**

$T <: U$ if the set of member types in $T$ is a subset of the member types in $U$.

**Access**

Values of union type **MUST** be accessed via exhaustive `match`.

##### Memory & Layout

Structure: `(tag, payload)`

- **Tag**: Integer discriminant (size IDB, typically `u8` or `u16`).
- **Payload**: Union of fields (overlapping storage).
- **Size**: `sizeof(Tag) + sizeof(LargestMember) + Padding`.
- **Alignment**: `max(alignof(Tag), alignof(LargestMember))`.

---

### 4.11 Modal Types {Source: Draft 2 §19}

##### Definition

**Modal Type**

A **modal type** is a nominal type that embeds a compile-time-validated state machine. It consists of a family of related types:

1. **State-Specific Types (`M@S`)**: Concrete, zero-overhead types containing only the data defined in their specific state payload. They do **not** store a runtime state tag.
2. **The General Modal Type (`M`)**: A sum type (tagged union) capable of holding a value of any of its specific states. It stores the state payload **plus** a runtime discriminant (tag) to identify the current state.

This system allows for "Zero-Cost" states when the state is known at compile time, while supporting safe, dynamic inspection via `match` when the state is erased to the general type.

##### Syntax & Declaration

```ebnf
modal_decl  ::= [visibility] "modal" identifier [generic_params] "{" state_block+ "}"
state_block ::= "@" identifier [state_payload] [state_members]
state_payload ::= "{" (field_decl ("," field_decl)* ","?)? "}"
state_members ::= "{" procedure_declaration* "}"
```

- A `modal` declaration **MUST** contain at least one state block.
- All state names within a `modal` declaration **MUST** be unique.
- Fields defined within a `@State` block are implicitly `protected` (accessible only to the modal type's implementation).

**Transition Signature**:

```ebnf
transition_signature ::= "transition" identifier "(" param_list ")" "->" "@" target_state
```

**Transition Implementation**:

```ebnf
transition_impl ::= "transition" type_name "::" identifier "(" param_list ")" "->" target_state_type block
```

##### Static Semantics

**Incomparability**

Two different state-specific types of the same modal type are incomparable:
$$\frac{S_A \neq S_B}{\Gamma \vdash M@S_A \not<: M@S_B \quad \land \quad \Gamma \vdash M@S_B \not<: M@S_A}$$

**Modal Widening (Coercion)**

An expression of a state-specific type `M@S` **MAY** be implicitly coerced to the general modal type `M`:
$$\frac{}{\Gamma \vdash M@S <: M}$$

**Transition Desugaring**

- The `transition` keyword **MUST** be used (not `procedure`).
- The receiver type is determined by the enclosing state block.
- The return type **MUST** be the target state type.

##### Memory & Layout

- **State-Specific Type (`M@S`)**: Layout equivalent to a `record` with the state payload fields. Zero size if payload is empty.
- **General Modal Type (`M`)**: Layout equivalent to an `enum` (tagged union) where each variant corresponds to one defined state.

**Niche Optimization (MANDATE)**

Implementations **MUST** apply niche optimization when any state payload contains invalid bit patterns that can distinguish states:

- **Example**: `Ptr<T>` has `@Null` (address `0x0`) and `@Valid` (non-zero addresses). The general `Ptr<T>` type **MUST** have the same size/alignment as a raw pointer (one machine word), **NOT** size of pointer + discriminant.

##### Dynamic Semantics

**Pattern Matching**

- **General Type (`M`)**: Runtime dispatch on discriminant/niche. Match **MUST** be exhaustive.
- **Specific Type (`M@S`)**: Irrefutable payload destructuring (state statically known).

##### Constraints & Legality

| Code         | Severity | Condition                                           |
| :----------- | :------- | :-------------------------------------------------- |
| `E-TYP-1910` | Error    | Modal type must declare at least one `@State`.      |
| `E-TYP-1911` | Error    | Duplicate state name in modal type.                 |
| `E-TYP-1912` | Error    | Field access in wrong state.                        |
| `E-TYP-1915` | Error    | Transition procedure must return target state type. |
| `E-TYP-1920` | Error    | Non-exhaustive match on modal type.                 |

---

### 4.12 String Types {Source: Draft 2 §20}

##### Definition

The **string** type is a built-in modal type with two states:

- **`string@Managed`**: An owned, heap-allocated string.
- **`string@View`**: A borrowed slice/view into string data.

All string content **MUST** be valid UTF-8.

##### Memory & Layout

- **`string@View`**: Fat pointer `(ptr: *u8, len: usize)`. Two machine words.
- **Storage**: Not null-terminated. Content **MUST** be valid UTF-8.

##### Constraints & Legality

- **Indexing**: Direct byte indexing (`s[i]`) on any string type is **forbidden**.
- **Slicing**: Indices are interpreted as **byte offsets**. Slice boundaries **MUST** fall on valid UTF-8 character starts; failure triggers a panic.
- **Copy/Clone**: `string@Managed` **MUST NOT** implement `Copy`.

| Code         | Severity | Condition                                    |
| :----------- | :------- | :------------------------------------------- |
| `E-TYP-1901` | Panic    | Slice boundary not on UTF-8 character start. |
| `E-TYP-1902` | Error    | Direct byte indexing on string type.         |

---

### 4.13 Pointer Types {Source: Draft 2 §21}

##### Definition

**Safe Pointer (`Ptr<T>`)**

A modal type with states:
- **`@Valid`**: Points to valid, initialized memory.
- **`@Null`**: The null pointer (address `0`).
- **`@Expired`**: Points to deallocated memory (only reachable via unsafe code).

**Raw Pointers**

- **`*imm T`**: Raw immutable pointer.
- **`*mut T`**: Raw mutable pointer.

Raw pointer operations are **unsafe**.

##### Memory & Layout

- **Size**: One machine word (platform pointer size).
- **Alignment**: Platform word alignment.

##### Constraints & Legality

- Dereferencing `Ptr<T>@Null` or `Ptr<T>@Expired` is a **compile-time error**.
- Dereferencing raw pointers requires an `unsafe` block.

---

### 4.14 Function Types {Source: Draft 2 §22}

##### Definition

**Function Type**

A structural type representing a callable signature: `(T1, ..., Tn) -> R`.

**Sparse (Function Pointer)**

A single machine-word code pointer. FFI-safe.

**Dense (Witness Closure)**

A two-word pointer `(env_ptr, code_ptr)` where `env_ptr` points to the captured environment. **Not** FFI-safe.

##### Syntax & Declaration

```ebnf
function_type   ::= "(" [param_type_list] ")" "->" type
param_type_list ::= param_type ("," param_type)*
param_type      ::= ["move"] type
```

A function type's identity includes:
- The full ordered list of parameter types (including the presence or absence of the `move` modifier).
- The return type.

Parameter names and contract annotations are **not** part of the function type.

##### Static Semantics

**Structural Equivalence**

Two function types are equivalent if and only if:
- They have the same number of parameters.
- Each corresponding pair of parameters has equivalent types and matching `move` modifiers.
- Their return types are equivalent.

**Variance**

- Parameters are **contravariant**.
- Return type is **covariant**.

**Subtyping**

For the same parameter and return types, a sparse function pointer `(T) -> U` is a subtype of a witness closure `witness (T) -> U`. Non-capturing procedures may be used where a witness closure is expected, but not vice versa.

##### Memory & Layout

- **Sparse**: 1 machine word.
- **Dense (witness)**: 2 machine words. **Not** FFI-safe.

---

### 4.15 Static Polymorphism (Generics) {Source: Draft 2 §28.3}

##### Definition

**Monomorphization**

**Monomorphization** is the process of generating specialized code for each concrete instantiation of a generic type or procedure. This is Cursive's "Path 1" polymorphism: zero overhead, polymorphism on *inputs* resolved at compile time.

##### Syntax & Declaration

```ebnf
generic_params ::= "<" generic_param ("," generic_param)* ">"
generic_param  ::= identifier ["<:" trait_bound]
```

**Declaration**: `<T>` (unconstrained) or `<T <: Trait>` (constrained).

**Instantiation**: Implicit at call site; the compiler infers concrete types.

##### Static Semantics

- **Monomorphization**: Implementations **MUST** generate specialized code for each concrete type combination.
- **Zero Overhead**: Generic calls **MUST** resolve to direct static calls; no vtable lookup is permitted.
- **Resolution**: Generic parameters are treated as invariant types within the procedure body unless bounded otherwise.
- **Infinite Recursion Detection**: Infinite monomorphization recursion (e.g., `f<T>() { f<Option<T>>() }`) **MUST** be detected and rejected.

##### Constraints & Legality

- Generic parameters are **PROHIBITED** in `extern` procedure signatures.

---

### 4.16 Type Layout {Source: Draft 2 §15.4}

##### Definition

**Layout**

The **layout** of a type comprises its size (`sizeof`), alignment (`alignof`), and physical representation (padding, field ordering).

##### Memory & Layout

- General layouts are **IDB** (implementation-defined behavior).
- Implementations **MUST** document their type layout strategy for each supported target platform in the conformance dossier (Appendix C).
- The `[[repr(C)]]` attribute, when applied to a `record` or `enum` declaration, **MUST** direct the implementation to produce a C-compatible memory layout as defined by the target platform's C ABI.

**Minimum Guarantees**

The following minimum guarantees **MUST** hold regardless of IDB choices:

- The size of a `record` is at least the sum of the sizes of its fields.
- The size of an `enum` is at least the size of its tag plus the size of its largest variant.
- The size of an array `[T; N]` is at least `N` times the size of `T`.
- The alignment of a composite type is at least as great as the largest alignment of any of its fields.

---

### 4.17 Attributes {Source: Draft 2 §23.5}

##### Definition

An **attribute** is a compile-time annotation attached to a declaration, providing metadata to the compiler.

##### Syntax & Declaration

```ebnf
attribute_list ::= "[[" attribute_spec ("," attribute_spec)* "]]"
attribute_spec ::= identifier ["(" argument_list ")"]
argument_list  ::= argument ("," argument)*
argument       ::= literal | identifier | identifier ":" literal
```

##### Static Semantics

**Normative Attribute Registry**

| Attribute               | Target             | Effect                                       |
| :---------------------- | :----------------- | :------------------------------------------- |
| `[[repr(...)]]`         | `record`, `enum`   | Memory layout control (C, packed, align).    |
| `[[inline(...)]]`       | `procedure`        | Inlining hint (always, never, default).      |
| `[[cold]]`              | `procedure`        | Unlikely-to-execute hint.                    |
| `[[link_name("...")]]`  | `extern` procedure | FFI symbol naming.                           |
| `[[unwind(...)]]`       | `extern` procedure | FFI panic handling.                          |
| `[[deprecated("...")]]` | Any declaration    | Warn on usage.                               |
| `[[verify(...)]]`       | Contract           | Contract verification mode selection (§7.4). |

##### Constraints & Legality

- Unknown attributes **MUST** trigger `E-DEC-2451`.
- Attributes **MUST** match their target declaration type (e.g., `[[repr(C)]]` is valid only on `record` or `enum`).

| Code         | Severity | Condition          |
| :----------- | :------- | :----------------- |
| `E-DEC-2451` | Error    | Unknown attribute. |

---

### 4.18 Bindings {Source: Draft 2 §23.1, §29.2}

##### Definition

A **binding** is a named association between an identifier and a typed value.

**Binding Categories**

| Category         | Keyword        | Storage Duration  | Scope     |
| :--------------- | :------------- | :---------------- | :-------- |
| Local immutable  | `let`          | Automatic (stack) | Block     |
| Local mutable    | `var`          | Automatic (stack) | Block     |
| Global immutable | `static let`   | Static            | Module    |
| Global mutable   | `static var`   | Static            | Module    |
| Parameter        | (in signature) | Automatic (stack) | Procedure |

##### Syntax & Declaration

```ebnf
local_binding  ::= ("let" | "var") identifier [":" type] "=" expression
global_binding ::= [visibility] "static" ("let" | "var") identifier ":" type "=" expression
```

##### Static Semantics

- **`let`**: Immutable binding. Cannot be reassigned after initialization.
- **`var`**: Mutable binding. May be reassigned.
- **Orthogonality**: Binding mutability is orthogonal to permission:
  - A `let` binding may hold `unique` data (data is mutable, but the binding cannot be reassigned).
  - A `var` binding may hold `const` data (binding is reassignable, but the referenced data is immutable).
- Global bindings **MUST** have explicit type annotations.

##### Dynamic Semantics

**Initialization**

- **Static Init**: Compile-time-constant expressions are baked into the data section.
- **Dynamic Init**: Runtime expressions follow the **Acyclic Eager Subgraph** rule defined in §5.5 (Initialization).

##### Constraints & Legality

- Accessing `static var` (mutable global) is **`unsafe`** unless synchronized.
- Type annotations **MAY** be omitted for local bindings when the type is inferable from context.

---

### 4.19 Refinement Types {Source: Draft 2 §27.4}

##### Definition

A **Refinement Type** is a subtype constructed by attaching a predicate constraint to a base type. The refinement type `T where { P }` denotes the subset of values of type `T` for which predicate `P` evaluates to `true`.

**Formal Definition:**
$$
\text{Values}(T \text{ where } \{P\}) = \{ v \in \text{Values}(T) \mid P(v) = \text{true} \}
$$

Refinement types enable **dependent typing** patterns where type validity depends on runtime-checkable properties.

##### Syntax & Declaration

**Grammar:**

```ebnf
refinement_type ::= type "where" "{" predicate "}"
```

**Self Reference:** Within the predicate of a **type alias** or **standalone refinement type**, the keyword `self` refers to the value being constrained.

**Examples:**
- `usize where { self < 256 }` — unsigned integers less than 256
- `i32 where { self > 0 }` — positive 32-bit integers
- `[T] where { self~>len() > 0 }` — non-empty slices

##### Unified Parameter Constraints

When a refinement type appears as a **procedure parameter type**, the `self` keyword is **NOT** used. Instead, the predicate uses the **parameter name** directly:

```ebnf
param_with_constraint ::= identifier ":" type "where" "{" predicate "}"
```

Within the predicate:
- The **parameter name** refers to the parameter value
- Other in-scope parameters may also be referenced (for dependent constraints)

This syntax is unified with procedure `where` clauses (§7.1). A parameter constraint is semantically equivalent to a precondition that constrains only that parameter.

**Desugaring:**

A parameter with inline constraint:
```cursive
procedure get<T>(arr: [T], idx: usize where { idx < arr~>len() }): T
```

Is semantically equivalent to:
```cursive
procedure get<T>(arr: [T], idx: usize): T
    where { idx < arr~>len() }
```

The inline form is preferred when the constraint concerns a single parameter. The procedure `where` clause is preferred for:
- Multi-parameter relationships
- Postconditions
- Complex predicates that benefit from separate formatting

##### Static Semantics

**Subtyping Rule:**

A refinement type is a subtype of its base type and of other refinement types with weaker predicates:

$$
\frac{
    \Gamma \vdash P \implies Q
}{
    \Gamma \vdash (T \text{ where } \{P\}) <: (T \text{ where } \{Q\})
}
\tag{Sub-Refine}
$$

$$
\Gamma \vdash (T \text{ where } \{P\}) <: T
\tag{Sub-Refine-Base}
$$

**Type Equivalence:**

Two refinement types are equivalent if their predicates are logically equivalent:
$$
\Gamma \vdash (T \text{ where } \{P\}) \equiv (T \text{ where } \{Q\}) \iff \Gamma \vdash P \iff Q
$$

**Intersection (Nested Refinements):**

Nested refinement types are equivalent to conjunction:
$$
(T \text{ where } \{P\}) \text{ where } \{Q\} \equiv T \text{ where } \{P \land Q\}
$$

**Predicate Scope:**

The predicate `P` in `T where { P }`:
- **MUST** be a pure expression (see Clause 7 §7.1)
- **MUST** evaluate to type `bool`
- **MAY** reference `self` (the constrained value) in type aliases and standalone refinement types
- **MUST** reference the parameter name (not `self`) in inline parameter constraints
- **MAY** reference other in-scope bindings (for dependent constraints)
- **MAY** reference earlier parameters when used in procedure signatures (for dependent parameter types)

**Unified Syntax Rationale:**

Using the parameter name (instead of `self`) in inline parameter constraints provides:

1. **Consistency**: The same identifier appears in the signature and the predicate
2. **Dependent typing**: Later parameters can reference earlier parameters by name
3. **Unification**: Inline constraints and procedure `where` clauses use identical predicate syntax
4. **Readability**: `idx < arr~>len()` reads more naturally than `self < arr~>len()`

The `self` keyword is reserved for contexts where no parameter name exists:
- Type aliases: `type Positive = i32 where { self > 0 }`
- Record/enum invariants: `record R { x: i32 } where { self.x > 0 }`

**Automatic Coercion:**

The implementation **MUST** automatically coerce a value of type `T` to type `T where { P }` if active Verification Facts (Clause 7 §7.5) prove that `P` holds for that value.

##### Dynamic Semantics

**Verification:**

When a value is assigned to a binding of refinement type, or passed to a parameter of refinement type, the predicate **MUST** be verified according to the active Verification Mode (Clause 7 §7.4):

| Mode      | Behavior                                           |
| :-------- | :------------------------------------------------- |
| `static`  | Predicate must be statically provable              |
| `dynamic` | Runtime check inserted; panic on failure (default) |
| `trusted` | No check; violation is UVB (requires `unsafe`)     |
| (default) | Runtime check with static elision optimization     |

##### Memory & Layout

**Representation:**

A refinement type `T where { P }` has the **same size, alignment, and layout** as its base type `T`. The predicate is a compile-time/runtime constraint only; it does not affect physical representation.

$$
\text{sizeof}(T \text{ where } \{P\}) = \text{sizeof}(T)
$$
$$
\text{alignof}(T \text{ where } \{P\}) = \text{alignof}(T)
$$

##### Constraints & Legality

**Purity:** The predicate **MUST** be pure (no side effects, no I/O, no allocation).

**Decidability:** For `static` verification mode, the implementation **MAY** reject predicates that are not decidable by its proof system. This is not an error in the program but a limitation of the verifier; the programmer may use `dynamic` mode as fallback.

**Recursive Constraints:** Refinement predicates **MUST NOT** create circular type dependencies.

| Code         | Severity | Condition                                                       |
| :----------- | :------- | :-------------------------------------------------------------- |
| `E-TYP-1950` | Error    | `self` used in inline parameter constraint (use parameter name) |

##### Examples

**Valid:**

```cursive
// Bounded array index - parameter name used in predicate
procedure get<T>(arr: [T], idx: usize where { idx < arr~>len() }): T {
    result arr[idx]  // No bounds check needed - statically verified
}

// Positive number type alias - 'self' used (no parameter name available)
type PositiveInt = i32 where { self > 0 }

// Non-empty slice - parameter name used
procedure first<T>(slice: [T] where { slice~>len() > 0 }): T {
    result slice[0]  // Safe - slice is non-empty
}

// Multi-parameter constraint - can use either inline or procedure where
procedure copy_range<T>(
    src: [T],
    dst: [T] where { dst~>len() >= src~>len() },
    count: usize where { count <= src~>len() }
) {
    // ...
}

// Equivalent using procedure where clause
procedure copy_range_alt<T>(src: [T], dst: [T], count: usize)
    where { dst~>len() >= src~>len() && count <= src~>len() }
{
    // ...
}

// Dependent parameter constraint - later param references earlier param
procedure slice_range<T>(
    arr: [T],
    start: usize where { start <= arr~>len() },
    end: usize where { end >= start && end <= arr~>len() }
): [T] {
    // ...
}
```

**Invalid:**

```cursive
// E-CON-2802: Impure predicate (I/O)
type Bad = i32 where { print(self); true }

// Undecidable in static mode (may require dynamic fallback)
procedure collatz(n: i32 where { terminates_collatz(n) }) { ... }

// E-TYP-1950: 'self' used in parameter constraint (should use 'idx')
procedure bad_get<T>(arr: [T], idx: usize where { self < arr~>len() }): T { ... }
```

---

### Clause 4: Cross-Reference Notes

**Terms defined in Clause 4 that MUST NOT be redefined elsewhere:**

| Term                        | Section | Description                                                  |
| :-------------------------- | :------ | :----------------------------------------------------------- |
| Type Context ($\Gamma$)     | §4.1    | Environment mapping identifiers to types.                    |
| Subtype Relation ($<:$)     | §4.2    | Subtyping relation definition.                               |
| Covariant                   | §4.3    | Variance polarity for output positions.                      |
| Contravariant               | §4.3    | Variance polarity for input positions.                       |
| Invariant                   | §4.3    | Variance polarity for mutable positions.                     |
| Bivariant                   | §4.3    | Variance polarity for unused parameters.                     |
| Permission Lattice          | §4.5    | Hierarchy of `unique`, `partitioned`, `concurrent`, `const`. |
| `concurrent`                | §4.5    | Permission for synchronized mutable access.                  |
| Discriminant                | §4.9    | Integer identifying active enum variant.                     |
| Modal Type                  | §4.11   | Type with compile-time state machine.                        |
| Monomorphization            | §4.15   | Generic specialization process.                              |
| Layout                      | §4.16   | Size/alignment/representation of types.                      |
| Attribute                   | §4.17   | Compile-time annotation on declarations.                     |
| Binding                     | §4.18   | Named association between identifier and typed value.        |
| Refinement Type             | §4.19   | Subtype with predicate constraint (`T where { P }`).         |
| Inline Parameter Constraint | §4.19   | Parameter refinement using parameter name in predicate.      |

**Terms referenced from other clauses:**

| Term                | Source   | Usage in Clause 4                                |
| :------------------ | :------- | :----------------------------------------------- |
| UVB                 | Clause 1 | Coercion must not introduce UVB.                 |
| IDB                 | Clause 1 | Layout choices documented as IDB.                |
| Object              | Clause 3 | Types classify objects.                          |
| Provenance          | Clause 3 | Pointer provenance tracking.                     |
| Partitioning System | Clause 3 | Validates `partitioned` access.                  |
| Responsibility      | Clause 3 | Move semantics for bindings.                     |
| `unsafe`            | Clause 1 | Raw pointer dereference; `static var` access.    |
| Initialization      | §5.5     | Acyclic Eager Subgraph rule for globals.         |
| Verification Mode   | §7.4     | Verification strategy for refinement predicates. |
| Verification Fact   | §7.5     | Enables automatic coercion to refinement types.  |
| Pure expression     | §7.1     | Purity requirement for refinement predicates.    |

**Terms deferred to later clauses:**

| Term                   | Deferred To | Reason                                              |
| :--------------------- | :---------- | :-------------------------------------------------- |
| Trait                  | Clause 6    | Trait bounds for generics.                          |
| Witness (dynamic)      | Clause 6    | Dynamic polymorphism mechanism.                     |
| `match` exhaustiveness | Clause 8    | Pattern matching semantics.                         |
| Procedure Declaration  | Clause 8    | Syntactic declaration separate from function types. |

---

## Clause 5: Modules and Name Resolution

This clause defines Cursive's module system: how code is organized into projects, assemblies, and modules; how names are introduced, resolved, and accessed; and how modules are initialized before program execution.

### 5.1 Module System Architecture {Source: Draft 2 §11.1, §8.3, §10.1}

##### Definition

**Project**

A **Project** is the top-level organizational unit, consisting of a collection of source files and a single manifest file (`Cursive.toml`) at its root. A project defines one or more assemblies.

**Assembly**

An **Assembly** is a collection of modules that are compiled and distributed as a single unit. An assembly **MAY** be either a `library` or an `executable`. Each assembly is defined within the project manifest.

**Module**

A **Module** is the fundamental unit of code organization and encapsulation. A module's contents are defined by the `.cursive` source files within a single directory, and its namespace is identified by a unique, filesystem-derived **module path**.

**Compilation Unit**

A **Compilation Unit** is the set of all source files constituting a single module. Each compilation unit defines exactly one module. An empty module (containing only whitespace and comments) is valid.

**Folder-as-Module Principle**

Each directory within a declared source root that contains one or more `.cursive` files **MUST** be treated as a single module. All `.cursive` files located directly within that directory contribute their top-level declarations to that single module's namespace.

##### Syntax & Declaration

**Top-Level Item Grammar**

Only the following declaration kinds are permitted at module scope:

```ebnf
top_level_item ::= import_decl
                 | use_decl
                 | static_decl
                 | procedure_decl
                 | type_decl
                 | trait_decl

static_decl    ::= visibility? "static" ("let" | "var") binding
type_decl      ::= visibility? ("record" | "enum" | "modal" | "type") identifier ...
trait_decl     ::= visibility? "trait" identifier ...
procedure_decl ::= visibility? "procedure" identifier ...
```

**Module Path Grammar**

```ebnf
module_path    ::= identifier ("::" identifier)*
```

##### Static Semantics

**Module Path Derivation**

The module path for a given module **MUST** be derived from its directory path relative to the assembly's source root directory. Directory separators in the path **MUST** be replaced by the scope resolution operator `::`.

**Formal Rule (WF-Module-Path-Derivation):**

Let $P$ be the project context containing assembly definitions.

$$
\frac{P \vdash \text{assembly} \Rightarrow (\text{root\_path}, \_) \quad \text{dir\_path} = \text{absolute}(\text{module\_dir}) \quad \text{rel\_path} = \text{relative}(\text{dir\_path}, \text{root\_path})}{P \vdash \text{dir\_path} \Rightarrow \text{replace}(\text{rel\_path}, \text{PATH\_SEPARATOR}, "::")}
$$

**Module Well-Formedness**

A module is well-formed if:
1. All top-level declarations are valid according to the grammar
2. Each identifier is unique within the module's namespace
3. No control-flow constructs (`if`, `loop`, `match`) appear at top level
4. No expression statements appear at top level

##### Constraints & Legality

**Visibility Default**: Top-level items without an explicit visibility modifier default to `internal`.

**Identifier Uniqueness**: Each identifier **MUST** be unique within module scope. Because Cursive uses a unified namespace (§5.4), a type declaration and a term declaration **MUST NOT** share the same identifier.

**Forbidden Constructs**: Control-flow constructs (`if`, `loop`, `match`) and expression statements **MUST NOT** appear at top level.

| Code         | Severity | Condition                                        |
| :----------- | :------- | :----------------------------------------------- |
| `E-MOD-1106` | Error    | Module path component is not a valid identifier. |
| `E-NAM-1302` | Error    | Duplicate declaration in module scope.           |
| `E-SYN-0501` | Error    | Control-flow construct at module scope.          |

---

### 5.2 Project Manifest {Source: Draft 2 §11.3}

##### Definition

Every project **MUST** be defined by a UTF-8 encoded manifest file named `Cursive.toml` at its root directory. The manifest **MUST** follow the TOML 1.0 syntax and be well-formed according to the rules in this section.

The manifest defines:
- Project metadata (`name`, `version`)
- Language version compatibility
- Source path mappings
- Assembly definitions

##### Syntax & Declaration

**Required Tables**

The manifest **MUST** contain the following tables:

| Table          | Purpose                                    |
| :------------- | :----------------------------------------- |
| `[project]`    | Project name and version                   |
| `[language]`   | Required Cursive language version          |
| `[paths]`      | Symbolic names mapping to source roots     |
| `[[assembly]]` | Assembly definitions (one or more entries) |

**`[project]` Table Schema**

```toml
[project]
name = "<identifier>"      # REQUIRED: Project identifier
version = "<semver>"       # REQUIRED: Project version (SemVer format)
```

**`[language]` Table Schema**

```toml
[language]
version = "<semver>"       # REQUIRED: Minimum language version
```

**`[paths]` Table Schema**

```toml
[paths]
<symbolic_name> = "<relative_path>"  # One or more entries
```

**`[[assembly]]` Table Schema**

```toml
[[assembly]]
name = "<identifier>"      # REQUIRED: Assembly name (unique)
root = "<path_key>"        # REQUIRED: Key from [paths] table
path = "<relative_path>"   # REQUIRED: Path relative to root
type = "library" | "executable"  # OPTIONAL: defaults to "library"
```

##### Static Semantics

**Manifest Well-Formedness Judgment**

The judgment $\vdash M : WF$ holds if the manifest $M$ is well-formed:

$$
\frac{
    M \vdash \text{project} : WF \quad
    M \vdash \text{language} : WF \quad
    M \vdash \text{paths} : WF \quad
    \forall a \in M.\text{assemblies}, M \vdash a : WF
}{
    \vdash M : WF
}
\tag{WF-Manifest}
$$

**`[project]` Table Well-Formedness (WF-Manifest-Project):**

$$
\frac{
    \text{name} \in M.\text{project} \quad \text{version} \in M.\text{project} \quad \vdash \text{name} : \text{Identifier} \quad \vdash \text{version} : \text{SemVer}
}{
    M \vdash \text{project} : WF
}
$$

**`[language]` Table Well-Formedness (WF-Manifest-Language):**

$$
\frac{
    \text{version} \in M.\text{language} \quad \vdash \text{version} : \text{SemVer} \quad \text{is\_compatible}(\text{version}, \text{compiler\_version})
}{
    M \vdash \text{language} : WF
}
$$

**`[paths]` Table Well-Formedness (WF-Manifest-Paths):**

$$
\frac{
    |M.\text{paths}| \ge 1 \quad \forall (k, v) \in M.\text{paths}, \text{is\_valid\_path}(v)
}{
    M \vdash \text{paths} : WF
}
$$

**`[[assembly]]` Well-Formedness (WF-Manifest-Assembly):**

$$
\frac{
    a.\text{name} \in \text{identifiers} \quad a.\text{root} \in \text{dom}(M.\text{paths}) \quad \text{is\_valid\_path}(a.\text{path})
}{
    M \vdash a : WF
}
$$

##### Constraints & Legality

| Code         | Severity | Condition                                                           |
| :----------- | :------- | :------------------------------------------------------------------ |
| `E-MOD-1101` | Error    | Manifest file `Cursive.toml` not found or syntactically malformed.  |
| `E-MOD-1102` | Error    | `[paths]` table is missing, empty, or contains invalid path values. |
| `E-MOD-1103` | Error    | Assembly references a `root` not defined in `[paths]` table.        |
| `E-MOD-1107` | Error    | `[project]` table or required keys (`name`, `version`) are missing. |
| `E-MOD-1108` | Error    | Duplicate assembly name in manifest.                                |
| `E-MOD-1109` | Error    | `[language]` version missing or incompatible with compiler.         |

##### Examples

**Valid Manifest:**

```toml
[project]
name = "acme.cli"
version = "1.2.3"

[language]
version = "1.0.0"

[paths]
core = "src"
vendor = "deps/vendor"

[[assembly]]
name = "core"
root = "core"
path = "."
type = "executable"

[[assembly]]
name = "utils"
root = "core"
path = "utils"
type = "library"
```

---

### 5.3 Module Discovery and Paths {Source: Draft 2 §11.2, §11.4}

##### Definition

**Module Path**

A **Module Path** is a sequence of identifiers separated by `::` that uniquely identifies a module within an assembly. The path is derived from the filesystem structure.

**Path Derivation Algorithm**

1. Compute the absolute path of the module's directory
2. Compute the absolute path of the assembly's source root
3. Compute the relative path from root to module directory
4. Replace all filesystem path separators with `::`

##### Static Semantics

**Path Well-Formedness Judgment**

The judgment $\vdash p : WF_{path}$ holds if a path $p$ is well-formed:

$$
\frac{
    \forall c \in \text{components}(p), (\vdash c : \text{Identifier} \land \vdash c : \text{NotKeyword})
}{
    \vdash p : WF_{path}
}
\tag{WF-Module-Path}
$$

A module path is well-formed if and only if every component is a valid Cursive identifier (§2.2) and is not a reserved keyword (§2.3).

**Case-Sensitivity Collision Detection**

On filesystems that are not case-sensitive, two file or directory names that differ only in case **MUST** be treated as ambiguous if they would resolve to the same module path component.

Let $N(p)$ be the NFC-normalized, case-folded version of a path component $p$:

$$
\frac{
    \exists p_1, p_2 \in \text{project\_paths} \quad p_1 \neq p_2 \quad N(p_1) = N(p_2)
}{
    \text{Collision Error}
}
\tag{WF-Module-Path-Collision}
$$

##### Constraints & Legality

| Code         | Severity | Condition                                                    |
| :----------- | :------- | :----------------------------------------------------------- |
| `E-MOD-1104` | Error    | Module path collision on case-insensitive filesystem.        |
| `E-MOD-1105` | Error    | Module path component is a reserved keyword.                 |
| `E-MOD-1106` | Error    | Module path component is not a valid identifier.             |
| `W-MOD-1101` | Warning  | Potential module path collision on case-insensitive systems. |

---

### 5.4 Scope Context Structure {Source: Draft 2 §13.2}

##### Definition

**Scope**

A **Scope** is a region of source text where a set of names is valid. Scopes are lexically nested.

**Scope Context ($\Gamma$)**

The **Scope Context** is the abstract data structure representing the mapping from identifiers to entities during name resolution. It is an ordered list of scope mappings from innermost to outermost:

$$\Gamma ::= [S_{local}, S_{proc}, S_{module}, S_{universe}]$$

Where each $S$ is a scope that maps identifiers to entities:

$$S : \text{Identifier} \to \text{Entity}$$

**Scope Hierarchy**

| Scope Level    | Contents                                                            |
| :------------- | :------------------------------------------------------------------ |
| $S_{local}$    | Block-local bindings (`let`, `var`, loop variables, match bindings) |
| $S_{proc}$     | Procedure parameters and local type parameters                      |
| $S_{module}$   | Module-level declarations and imported/used names                   |
| $S_{universe}$ | Primitive types and the `cursive.*` namespace                       |

**Universe Scope**

The **Universe Scope** ($S_{universe}$) is the implicit outermost scope that terminates all name resolution chains. It contains:
- All primitive types as defined in §4.6 (e.g., `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`, `f32`, `f64`, `bool`, `char`)
- Core literals (`true`, `false`)
- The `cursive.*` namespace for standard library access

**Unified Namespace**

Cursive **MUST** be implemented with a single, **unified namespace** per scope. An identifier's meaning is determined only by its spelling and the scope in which it is defined, not by the syntactic context of its use.

This single namespace **MUST** be shared by all declaration kinds:
1. **Terms**: Bindings for variables, constants, and procedures
2. **Types**: Bindings for type declarations (`record`, `enum`, `modal`, `type`, `trait`)
3. **Modules**: Bindings for module import aliases

##### Static Semantics

**Scope Nesting**

Scopes form a strict hierarchy. An inner scope $S_i$ is **nested within** an outer scope $S_o$ if $S_i$ appears before $S_o$ in the context list. The innermost scope is always searched first during name lookup.

**Context Extension**

The context $\Gamma$ is extended by adding a binding to the innermost scope:

$$\Gamma, x : T \equiv [S_{local} \cup \{x \mapsto T\}, S_{proc}, S_{module}, S_{universe}]$$

**Name Binding Model**

Each scope $S$ maintains a mapping from identifiers to **entities**. An entity is one of:
- A term (variable, constant, procedure)
- A type (record, enum, modal, type alias)
- A trait
- A module reference

##### Constraints & Legality

Because Cursive uses a unified namespace, declaring a type and a term with the same identifier in the same scope **MUST** be rejected:

| Code         | Severity | Condition                                                  |
| :----------- | :------- | :--------------------------------------------------------- |
| `E-NAM-1302` | Error    | Duplicate name: identifier already declared in this scope. |

---

### 5.5 Visibility and Access Control {Source: Draft 2 §12.1}

##### Definition

**Visibility**

Every top-level declaration has a **visibility level** that controls its accessibility from other modules. If no modifier is specified, visibility defaults to `internal`.

**Visibility Levels**

| Modifier    | Scope of Accessibility                                              |
| :---------- | :------------------------------------------------------------------ |
| `public`    | Visible to any module in any assembly that depends on it            |
| `internal`  | (Default) Visible only to modules within the **same assembly**      |
| `private`   | Visible only within the **defining module** (same directory)        |
| `protected` | Visible only within the **defining type** and trait implementations |

**Accessibility**

An item is **accessible** from a given context if the visibility rules permit the reference.

##### Syntax & Declaration

```ebnf
visibility ::= "public" | "internal" | "private" | "protected"
```

Visibility modifiers **MUST** appear as a prefix on declarations:

```cursive
public procedure foo() { ... }
internal record Bar { ... }
private let secret: i32 = 42
```

##### Static Semantics

**Access Judgment**

Let the context $\Gamma$ contain the accessor module $m_{acc}$ and its assembly $a_{acc}$. Let the target item $i$ be defined in module $m_{def}$ within assembly $a_{def}$. The judgment $\Gamma \vdash \text{can\_access}(i)$ holds if access is permitted.

**(WF-Access-Public):**
$$
\frac{\text{visibility}(i) = \text{public}}{\Gamma \vdash \text{can\_access}(i)}
$$

**(WF-Access-Internal):**
$$
\frac{\text{visibility}(i) = \text{internal} \quad a_{acc} = a_{def}}{\Gamma \vdash \text{can\_access}(i)}
$$

**(WF-Access-Private):**
$$
\frac{\text{visibility}(i) = \text{private} \quad m_{acc} = m_{def}}{\Gamma \vdash \text{can\_access}(i)}
$$

**(WF-Access-Protected-Self):**
$$
\frac{\text{visibility}(item) = \text{protected} \quad \Gamma \subseteq T_{def}}{\Gamma \vdash \text{can\_access}(item)}
$$

**(WF-Access-Protected-Trait):**
$$
\frac{\text{visibility}(item) = \text{protected} \quad \Gamma \subseteq \text{trait } Tr \text{ for } T_{def} \quad A(\Gamma) = A(T_{def})}{\Gamma \vdash \text{can\_access}(item)}
$$

##### Constraints & Legality

**Protected Restriction**: The `protected` modifier **MUST NOT** be used on top-level (module-scope) declarations. It **MUST** only be applied to members (fields or procedures) within a `record`, `enum`, or `modal` declaration.

**Intra-Assembly Access**: Modules within the same assembly are automatically available for qualified name access without requiring an `import` declaration. A declaration `item` in module `mod` within assembly `A` is accessible from another module in assembly `A` via the qualified path `mod::item` if and only if the visibility of `item` is `public` or `internal`.

| Code         | Severity | Condition                                             |
| :----------- | :------- | :---------------------------------------------------- |
| `E-NAM-1305` | Error    | Inaccessible item: visibility does not permit access. |
| `E-DEC-2440` | Error    | `protected` used on top-level declaration.            |

---

### 5.6 Import and Use Declarations {Source: Draft 2 §12.3–§12.5}

##### Definition

**Import Declaration**

An `import` declaration declares a dependency on a module from an **external assembly**, making that module's public items available for qualified access. It is the primary mechanism for building the inter-assembly dependency graph.

**Use Declaration**

A `use` declaration creates a **scope alias** that brings items into the current scope, enabling unqualified access to otherwise qualified names.

**Re-export**

A `public use` declaration **re-exports** an imported item, making it available to modules that depend on the current assembly.

##### Syntax & Declaration

**Grammar:**

```ebnf
import_decl ::= "import" module_path

use_decl    ::= "use" use_path ("as" identifier)?
use_path    ::= module_path ("::" item_name)?

item_name   ::= identifier | "{" identifier ("," identifier)* "}"
```

**Examples:**

```cursive
// Import external assembly module
import core::io

// Use with alias
use core::io::File as FileHandle

// Use multiple items
use core::collections::{Vec, HashMap}

// Re-export
public use core::io::File
```

##### Static Semantics

**Import Semantics**

An `import` declaration:
1. Establishes a dependency edge from the current assembly to the imported assembly
2. Makes the imported module's namespace available for qualified access
3. Does **NOT** introduce any names into the current scope's unqualified namespace

**Use Semantics**

A `use` declaration:
1. Resolves the specified path to an entity
2. Introduces a binding in the current module's scope
3. If `as alias` is specified, the alias becomes the local name; otherwise, the item's original name is used

**Re-export Semantics**

A `public use` declaration:
1. Performs the same resolution as a regular `use`
2. Additionally marks the introduced binding as `public`, making it visible to external dependents

##### Constraints & Legality

**Import Cycle Detection**: The compiler **MUST** detect and reject cyclic import dependencies between assemblies.

**Visibility Enforcement**: Re-exported items **MUST NOT** expose items with more restrictive visibility than the re-export itself. Re-exporting a `private` item via `public use` is forbidden.

| Code         | Severity | Condition                                            |
| :----------- | :------- | :--------------------------------------------------- |
| `E-MOD-1201` | Error    | Circular import dependency between assemblies.       |
| `E-MOD-1202` | Error    | Import of non-existent assembly or module.           |
| `E-NAM-1305` | Error    | Use of inaccessible item (visibility violation).     |
| `E-NAM-1307` | Error    | Re-export exposes item with incompatible visibility. |

##### Examples

```cursive
// Module: app/main.cursive

import core::io          // External dependency
import utils::helpers    // Same-project dependency

use io::File             // Brings File into scope
use helpers::format as fmt  // Alias

public use io::File      // Re-export for dependents

public procedure main(ctx: Context): i32 {
    let f = File::open(ctx.fs, "test.txt")  // Unqualified access
    let s = fmt("Hello")                     // Aliased access
    result 0
}
```

---

### 5.7 Name Resolution {Source: Draft 2 §13.1, §13.3–§13.4}

##### Definition

**Name Introduction**

**Name Introduction** is the process of adding a binding to a scope's namespace. A declaration introduces an identifier into the current scope.

**Shadowing**

**Shadowing** occurs when a declaration in an inner scope introduces a binding that obscures a binding of the same name in an outer scope. Cursive requires explicit acknowledgment of shadowing.

**Name Lookup (Resolution)**

**Name Lookup** is the process of finding the entity to which an identifier refers.

##### Static Semantics

**Name Introduction Rules**

A declaration that introduces an identifier $x$ into the current scope $S_{curr}$ **MUST** be well-formed only if $x$ is not already bound in $S_{curr}$:

$$
\frac{x \notin \text{dom}(S_{curr})}{\Gamma \vdash \text{declare } x : WF}
\tag{WF-Declaration}
$$

**Explicit Shadowing Rules**

A declaration in an inner scope **MAY** shadow a name from an outer scope under these conditions:

**(WF-Shadow-Success):** Explicit shadowing with `shadow` keyword:
$$
\frac{
    x \in \text{dom}(S_{outer}) \quad x \notin \text{dom}(S_{inner})
}{
    \Gamma \vdash \text{shadow declare } x \text{ in } S_{inner} : WF
}
$$

**(WF-Shadow-Unnecessary):** Using `shadow` when no binding is shadowed:
$$
\frac{
    x \notin \text{dom}(S_{outer})
}{
    \Gamma \vdash \text{shadow declare } x \text{ in } S_{inner} : \text{Ill-Formed}
}
$$

**Conformance Mode Behavior:**
- **Strict Mode**: Implicit shadowing (without `shadow` keyword) **MUST** trigger error `E-NAM-1303`
- **Permissive Mode**: Implicit shadowing **MUST** trigger warning `W-NAM-1303`

**Unqualified Name Lookup**

Unqualified lookup searches the scope context from innermost to outermost.

The judgment $\Gamma \vdash x \Rightarrow \text{entity}$ holds if the identifier $x$ resolves to an entity in context $\Gamma$. Let $\Gamma = [S_0, S_1, \dots, S_n]$.

**(QR-Lookup-Local):**
$$
\frac{x \in \text{dom}(S_0)}{\Gamma \vdash x \Rightarrow S_0(x)}
$$

**(QR-Lookup-Outer):**
$$
\frac{x \notin \text{dom}(S_0) \quad [S_1, \dots, S_n] \vdash x \Rightarrow \text{entity}}{\Gamma \vdash x \Rightarrow \text{entity}}
$$

If the context is exhausted and the name is not found, the lookup fails.

**Qualified Name Lookup**

Qualified lookup resolves a path of identifiers, such as `A::B::C`.

The judgment $\Gamma \vdash p::i \Rightarrow \text{entity}$ holds if the qualified path resolves:

**(QR-Lookup-Qualified):**
$$
\frac{\Gamma \vdash p \Rightarrow m \quad m \vdash i \Rightarrow \text{entity} \quad \Gamma \vdash \text{can\_access}(\text{entity})}{\Gamma \vdash p::i \Rightarrow \text{entity}}
$$

To resolve a path `p::i`:
1. The prefix `p` must first resolve to a module `m`
2. The identifier `i` must resolve to a member of `m`
3. That member must be accessible from the current scope (§5.5)

##### Constraints & Legality

| Code         | Severity | Condition                                                      |
| :----------- | :------- | :------------------------------------------------------------- |
| `E-NAM-1301` | Error    | Unresolved name: identifier not found in any accessible scope. |
| `E-NAM-1302` | Error    | Duplicate name: identifier already declared in this scope.     |
| `E-NAM-1303` | Error    | Shadowing without `shadow` keyword (Strict Mode).              |
| `W-NAM-1303` | Warning  | Shadowing without `shadow` keyword (Permissive Mode).          |
| `E-NAM-1304` | Error    | Unresolved module: path prefix did not resolve to a module.    |
| `E-NAM-1305` | Error    | Inaccessible item: item found but visibility forbids access.   |
| `E-NAM-1306` | Error    | Unnecessary `shadow` keyword: no binding is being shadowed.    |

##### Examples

```cursive
let x: i32 = 10

procedure test() {
    // VALID: Explicitly shadows outer 'x'
    shadow let x: string@View = "hello"

    // ERROR in Strict Mode (E-NAM-1303):
    // let x = 20  // Implicit shadowing

    // ERROR (E-NAM-1306): No binding to shadow
    // shadow let y = 30
}
```

---

### 5.8 Module Initialization {Source: Draft 2 §14.1–§14.5}

##### Definition

**Module Dependency Graph (MDG)**

The **Module Dependency Graph** is a directed graph $G = (V, E)$ where:
- $V$ is the set of all modules in the program and its compiled dependencies
- $E$ is a set of directed edges $(A, B)$, where an edge from module $A$ to module $B$ signifies that $A$ depends on $B$

**Edge Classification**

Each edge in $E$ is classified as either **Type-Level** or **Value-Level**:

| Edge Type   | Definition                                                                    |
| :---------- | :---------------------------------------------------------------------------- |
| Value-Level | Module $A$ contains an expression that evaluates a binding defined in $B$     |
| Type-Level  | Module $A$ refers to a type, trait, or constant signature defined in $B$ only |

**Eager vs Lazy Dependencies**

| Dependency Type | Definition                                                                      |
| :-------------- | :------------------------------------------------------------------------------ |
| **Eager**       | A Value-Level edge originating from a **module-level initializer**              |
| **Lazy**        | A Type-Level edge, OR a Value-Level edge occurring only within procedure bodies |

**Initialization Stages**

Module initialization **MUST** be divided into two stages:

1. **Static Initialization**: Initializers that are compile-time constants (literals, `comptime` values) **MUST** be evaluated at compile time and stored in the program's data section.

2. **Dynamic Initialization**: Initializers requiring runtime execution (procedure calls, values requiring capabilities) **MUST** be executed after program startup but before the `main` procedure body executes.

##### Static Semantics

**Edge Classification Judgment (WF-Dep-Value):**

$$
\frac{
    \exists e \in \text{exprs}(A), \text{refers\_to\_value}(e, B)
}{
    (A, B) \in E, \text{class}((A, B)) = \text{Value-Level}
}
$$

**Acyclic Eager Subgraph Requirement**

The initialization order is derived *only* from the subgraph of eager dependencies. The subgraph $G_e = (V, E_e)$, containing all modules $V$ and only the set of eager edges $E_e$, **MUST** be a Directed Acyclic Graph (DAG).

**(WF-Acyclic-Eager-Deps):**

$$
\frac{
    \forall v \in V, \neg \text{is\_reachable}(v, v, E_e)
}{
    \vdash G_e : \text{DAG}
}
$$

Where $\text{is\_reachable}(u, v, E_e)$ is true if there is a path of one or more eager edges from $u$ to $v$.

##### Dynamic Semantics

**Execution Order**

Before the program's `main` procedure is invoked, the runtime **MUST**:

1. Compute a valid topological sort of modules based on the eager dependency graph $G_e$
2. For each module in the sorted list, execute the initializers for all module-level bindings in the order they appear within that module's source files

**Intra-Module Order**: Within a module, initializers **MUST** execute in strictly sequential lexical order.

**Failure Semantics**

If the evaluation of any module-level initializer terminates with a panic:
1. The initialization of that module is considered **failed**
2. The program state is considered **poisoned**
3. Any subsequent attempt to access a binding from that module, or from any module with an eager dependency path to it, **MUST** also result in a panic

##### Constraints & Legality

| Code         | Severity | Condition                                                |
| :----------- | :------- | :------------------------------------------------------- |
| `E-MOD-1401` | Error    | Cyclic module dependency detected in eager initializers. |

---

### 5.9 Program Entry Point {Source: Draft 2 §30.1, §30.2}

##### Definition

**Entry Point**

The **Entry Point** is the `main` procedure that receives the root capabilities and begins program execution after all module initialization is complete.

**No Ambient Authority Principle**

Cursive enforces the principle of **No Ambient Authority**: all procedures that produce observable external effects (I/O, networking, threading, heap allocation) **MUST** receive the required capability explicitly. There are no global functions for these operations.

**Context Record**

The **Context** record is the container for all system capabilities, injected by the runtime at program startup. The full definition of `Context` and its capability fields are specified in §10.2 (System Capabilities).

##### Syntax & Declaration

**Required Signature:**

```cursive
public procedure main(ctx: Context): i32
```

The `main` procedure:
- **MUST** be declared with `public` visibility
- **MUST** accept exactly one parameter of type `Context`
- **MUST** return type `i32`

**Context Record Structure** (normative definition in §10.2):

```cursive
record Context {
    fs: witness FileSystem,
    net: witness Network,
    sys: System,
    heap: witness HeapAllocator
}
```

##### Static Semantics

**Main Procedure Well-Formedness:**

1. Exactly one `main` procedure **MUST** exist in the entry assembly
2. The signature **MUST** match `main(ctx: Context): i32`
3. `main` **MUST NOT** be generic (no type parameters)

**Return Semantics:**

- Return value `0` indicates successful termination
- Non-zero return values indicate failure (specific codes are application-defined)

##### Constraints & Legality

**Global Mutable State Prohibition**: Module-level `var` bindings **MUST NOT** have `public` visibility. Global mutable state accessible across module boundaries is forbidden to ensure capability-based security.

| Code         | Severity | Condition                                |
| :----------- | :------- | :--------------------------------------- |
| `E-DEC-2430` | Error    | Multiple `main` procedures defined.      |
| `E-DEC-2431` | Error    | Invalid `main` signature.                |
| `E-DEC-2432` | Error    | `main` is generic (has type parameters). |
| `E-DEC-2433` | Error    | Public mutable global state.             |

##### Examples

**Valid Entry Point:**

```cursive
import core::io

public procedure main(ctx: Context): i32 {
    // ctx.fs provides filesystem capability
    let file = io::File::open(ctx.fs, "config.txt")

    // ctx.heap provides allocation capability
    let buffer = Vec::new(ctx.heap)

    result 0
}
```

---

### Clause 5: Modules and Resolution Cross-Reference Notes

**Terms defined in Clause 5 that MUST NOT be redefined elsewhere:**

| Term                    | Section | Description                                                       |
| :---------------------- | :------ | :---------------------------------------------------------------- |
| Project                 | §5.1    | Top-level organizational unit with manifest and assemblies        |
| Assembly                | §5.1    | Collection of modules compiled as a single distributable unit     |
| Module                  | §5.1    | Fundamental unit of code organization (folder-as-module)          |
| Compilation Unit        | §5.1    | Source files constituting a single module                         |
| Module Path             | §5.1    | Unique identifier derived from filesystem path                    |
| Folder-as-Module        | §5.1    | Principle: directory = module, files contribute to namespace      |
| Scope                   | §5.4    | Region of source text where a set of names is valid               |
| Scope Context           | §5.4    | Ordered list of scope mappings ($\Gamma$)                         |
| Universe Scope          | §5.4    | Implicit scope containing primitive types and cursive.* namespace |
| Unified Namespace       | §5.4    | Single namespace shared by terms, types, and modules              |
| Visibility              | §5.5    | Access level: public, internal, private, protected                |
| Accessibility           | §5.5    | Whether an item can be referenced from a given context            |
| Import Declaration      | §5.6    | Inter-assembly dependency declaration                             |
| Use Declaration         | §5.6    | Scope alias for qualified paths                                   |
| Re-export               | §5.6    | Making imported items available to dependents via public use      |
| Name Introduction       | §5.7    | Adding a binding to a scope's namespace                           |
| Shadowing               | §5.7    | Inner scope binding obscuring outer scope binding                 |
| Name Lookup             | §5.7    | Process of finding the entity for an identifier                   |
| Module Dependency Graph | §5.8    | Directed graph representing inter-module dependencies             |
| Eager Dependency        | §5.8    | Value-level edge from module initializer                          |
| Lazy Dependency         | §5.8    | Type-level edge or value-level edge only within procedure bodies  |
| Entry Point             | §5.9    | The main procedure receiving root capabilities                    |
| Context Record          | §5.9    | Container for system capabilities passed to main                  |
| No Ambient Authority    | §5.9    | Principle: all effects require explicit capability                |

**Terms referenced from other clauses:**

| Term             | Source             | Usage in Clause 5                                   |
| :--------------- | :----------------- | :-------------------------------------------------- |
| Identifier       | §2.2               | Module paths composed of identifiers                |
| Keyword          | §2.3               | Keywords forbidden in module path components        |
| Conformance Mode | §1.2               | Determines shadowing strictness (Permissive/Strict) |
| Primitive Types  | §4.6               | Universe Scope contents reference                   |
| witness          | §6.3               | Context fields use witness types                    |
| Panic            | §9 (Concurrency)   | Initialization failure triggers panic               |
| FileSystem       | §10 (Capabilities) | System capability in Context record                 |
| Network          | §10 (Capabilities) | System capability in Context record                 |
| HeapAllocator    | §10 (Capabilities) | Heap capability in Context record                   |

**Terms deferred to later clauses:**

| Term               | Deferred To           | Reason                                            |
| :----------------- | :-------------------- | :------------------------------------------------ |
| witness FileSystem | §10.2                 | Capability trait definition                       |
| witness Network    | §10.2                 | Capability trait definition                       |
| System             | §10.2                 | System capability trait                           |
| HeapAllocator      | §10.2                 | Allocator capability trait                        |
| Panic propagation  | §9 (Concurrency)      | Error handling semantics                          |
| comptime           | §11 (Metaprogramming) | Compile-time evaluation for static initialization |

---

## Clause 6: Traits and Polymorphism

This clause defines the Cursive trait system: the unified mechanism for defining interfaces, sharing implementations, and enabling polymorphism. Traits replace separate "behavior" and "contract" declarations from earlier designs, unifying them into a single `trait` keyword.

### 6.1 Introduction to Traits {Source: Draft 2 §28.0}

##### Definition

**Trait**

A **trait** is a declaration that defines an abstract interface consisting of:
- A set of procedure signatures (abstract or concrete)
- A set of associated type declarations (abstract or concrete)

A trait establishes a behavioral contract that implementing types **MUST** satisfy.

**Polymorphism Paths**

Cursive provides three distinct mechanisms for polymorphism, each with different dispatch semantics, performance characteristics, and use cases:

| Path | Name                 | Dispatch                        | Cost              | Use Case                  |
| :--- | :------------------- | :------------------------------ | :---------------- | :------------------------ |
| 1    | Static Polymorphism  | Compile-time (monomorphization) | Zero              | Constrained generics      |
| 2    | Dynamic Polymorphism | Runtime (vtable)                | One indirect call | Heterogeneous collections |
| 3    | Opaque Polymorphism  | Compile-time (type erasure)     | Zero              | Hidden return types       |

---

### 6.2 Trait Declarations {Source: Draft 2 §28.1}

##### Definition

A trait declaration introduces a named interface that types **MAY** implement. Traits define requirements through abstract procedures and **MAY** provide default implementations through concrete procedures.

**Abstract Procedure**

An **abstract procedure** is a procedure signature within a trait that lacks a body. Implementing types **MUST** provide a concrete implementation.

**Concrete Procedure (Default Implementation)**

A **concrete procedure** is a procedure definition within a trait that includes a body. Implementing types automatically inherit this procedure but **MAY** override it using the `override` keyword.

**Associated Type**

An **associated type** is a type declaration within a trait:
- If abstract (no `= T`): implementing types **MUST** provide a concrete type binding
- If concrete (`= T`): provides a default type that **MAY** be overridden

**The `Self` Type**

Within a trait declaration, the identifier `Self` denotes the (unknown) implementing type. `Self` **MUST** be used for the receiver parameter and **MAY** be used in any other type position.

**Generic Trait Parameters**

A trait **MAY** declare generic type parameters that parameterize the entire trait definition. These parameters are distinct from associated types:
- Generic parameters (`trait Foo<T>`) are specified at use-site by the implementer/consumer
- Associated types are specified by the implementer within the type body

**Supertrait (Trait Bounds)**

A trait **MAY** extend one or more supertraits using the `<:` operator. A type implementing a subtrait **MUST** also implement all of its supertraits.

**Trait Alias**

A **trait alias** declares a new name that is equivalent to a combination of one or more trait bounds. Trait aliases enable concise expression of compound bounds.

##### Syntax & Declaration

**Grammar**

```ebnf
trait_declaration ::=
    [ <visibility> ] "trait" <identifier> [ <generic_params> ]
    [ "<:" <supertrait_bounds> ] "{"
        <trait_item>*
    "}"

supertrait_bounds ::= <trait_bound> ( "+" <trait_bound> )*
trait_bound ::= <type_path> [ <generic_args> ]

generic_params ::= "<" <generic_param> ( "," <generic_param> )* ">"
generic_param ::= <identifier> [ "<:" <trait_bounds> ]

trait_item ::=
    <abstract_procedure>
  | <concrete_procedure>
  | <associated_type>

abstract_procedure ::=
    "procedure" <identifier> <signature> [ <procedure_constraint> ] ";"

concrete_procedure ::=
    "procedure" <identifier> <signature> [ <procedure_constraint> ] <block>

procedure_constraint ::= "where" "{" <precondition_expr> [ "=>" <postcondition_expr> ] "}"

associated_type ::=
    "type" <identifier> [ "=" <type> ] ";"

trait_alias ::=
    [ <visibility> ] "type" <identifier> [ <generic_params> ]
    "=" <trait_bound> ( "+" <trait_bound> )* ";"
```

##### Static Semantics

**Well-Formedness (WF-Trait)**

A trait declaration is well-formed if:
1. The identifier is unique within its namespace
2. All procedure signatures are well-formed
3. All associated types have unique names within the trait
4. No procedure name conflicts with an associated type name
5. All supertrait bounds refer to valid traits
6. No cyclic supertrait dependencies exist

**Typing Rule for Self**

$$\frac{\Gamma, \text{Self} : \text{Type} \vdash \text{body} : \text{ok}}{\Gamma \vdash \text{trait } T\ \{\ \text{body}\ \} : \text{Trait}}$$
\tag{WF-Trait-Self}

**Supertrait Inheritance (T-Supertrait)**

$$\frac{\text{trait } A <: B \quad T <: A}{\Gamma \vdash T <: B}$$
\tag{T-Supertrait}

A type implementing subtrait `A` transitively implements all supertraits.

**Generic Trait Instantiation**

For a generic trait `trait Foo<T>`:
- Each use-site `Foo<ConcreteType>` produces a distinct trait bound
- `T` is available within the trait body as a type parameter

**Trait Alias Equivalence (T-Alias-Equiv)**

$$\frac{\text{type } Alias = A + B}{\Gamma \vdash T <: Alias \iff \Gamma \vdash T <: A \land \Gamma \vdash T <: B}$$
\tag{T-Alias-Equiv}

##### Constraints & Legality

| Code         | Severity | Condition                                      |
| :----------- | :------- | :--------------------------------------------- |
| `E-TRS-2900` | Error    | Duplicate procedure name in trait.             |
| `E-TRS-2904` | Error    | Duplicate associated type name in trait.       |
| `E-TRS-2905` | Error    | Procedure name conflicts with associated type. |
| `E-TRS-2908` | Error    | Cyclic supertrait dependency detected.         |
| `E-TRS-2909` | Error    | Supertrait bound refers to undefined trait.    |

##### Examples

**Valid - Basic Trait:**
```cursive
public trait Drawable {
    // Abstract procedure - implementers MUST provide body
    procedure draw(~, ctx: Context);

    // Associated type - implementers MUST specify concrete type
    type Output;

    // Concrete procedure - default implementation provided
    procedure description(~): string@View {
        result "[drawable object]"
    }
}
```

**Valid - Generic Trait:**
```cursive
// Generic trait parameterized by type T
public trait Container<T> {
    procedure get(~, index: usize): T;
    procedure set(~!, index: usize, value: T);
    type Iterator;
}
```

**Valid - Supertrait:**
```cursive
// DebugDrawable requires Drawable as supertrait
public trait DebugDrawable <: Drawable {
    procedure debug_draw(~, ctx: Context, show_bounds: bool);
}

// Implementers MUST implement both DebugDrawable AND Drawable
record Widget <: DebugDrawable {
    // MUST provide: draw(), debug_draw(), and type Output
}
```

**Valid - Trait Alias:**
```cursive
// Trait alias combining multiple bounds
public type Renderable = Drawable + Sizeable + Clone;

// Equivalent to requiring all three traits
procedure render<T <: Renderable>(item: T, ctx: Context) {
    let size = item~>size()
    item~>draw(ctx)
}
```

---

### 6.3 Trait Implementation {Source: Draft 2 §28.2}

##### Definition

**Trait Implementation**

A type implements a trait by:
1. Declaring the trait in its "implements clause" using the `<:` operator
2. Providing implementations for all abstract procedures
3. Providing type bindings for all abstract associated types

**Implementation Site**

Trait implementation **MUST** occur at the type's definition site. Extension implementations (implementing a trait for a type defined elsewhere) are **PROHIBITED**.

##### Syntax & Declaration

**Grammar**

```ebnf
record_declaration ::=
    [ <visibility> ] "record" <identifier> [ <generic_params> ]
    [ "<:" <trait_list> ]
    <record_body>

enum_declaration ::=
    [ <visibility> ] "enum" <identifier> [ <generic_params> ]
    [ "<:" <trait_list> ]
    <enum_body>

modal_declaration ::=
    [ <visibility> ] "modal" <identifier> [ <generic_params> ]
    [ "<:" <trait_list> ]
    <modal_body>

trait_list ::= <type_path> ( "," <type_path> )*
```

##### Static Semantics

**Implementation Completeness (T-Impl-Complete)**

$$\frac{T <: Tr \quad \forall p \in \text{abstract\_procs}(Tr),\ T \text{ defines } p}{\Gamma \vdash T \text{ implements } Tr}$$
\tag{T-Impl-Complete}

**Override Semantics**

- **Abstract procedures**: implementation **MUST NOT** use the `override` keyword
- **Concrete procedures**: replacement **MUST** use the `override` keyword

**Coherence Rule**

A type `T` **MAY** implement a trait `Tr` at most once. Multiple implementations of the same trait for the same type are **forbidden**.

**Orphan Rule**

For `T <: Tr`, at least one of the following **MUST** be true:
- `T` is defined in the current assembly
- `Tr` is defined in the current assembly

This rule prevents external code from creating conflicting implementations.

##### Constraints & Legality

| Code         | Severity | Condition                                               |
| :----------- | :------- | :------------------------------------------------------ |
| `E-TRS-2901` | Error    | `override` used on abstract procedure implementation.   |
| `E-TRS-2902` | Error    | Missing `override` on concrete procedure replacement.   |
| `E-TRS-2903` | Error    | Type does not implement required procedure from trait.  |
| `E-TRS-2906` | Error    | Coherence violation: duplicate trait implementation.    |
| `E-TRS-2907` | Error    | Orphan rule violation: neither type nor trait is local. |

##### Examples

**Valid:**
```cursive
record Point <: Drawable {
    x: f64,
    y: f64,

    // Implements abstract procedure (no 'override')
    procedure draw(~, ctx: Context) {
        ctx~>draw_point(self.x, self.y)
    }

    // Associated type binding
    type Output = ()

    // Replaces concrete procedure (requires 'override')
    override procedure description(~): string@View {
        result "Point"
    }
}
```

**Invalid:**
```cursive
// E-TRS-2903: Missing implementation of 'draw'
record Circle <: Drawable {
    radius: f64,
    type Output = ()
    // ERROR: 'draw' not implemented
}
```

---

### 6.4 Static Polymorphism (Generics) {Source: Draft 2 §28.3}

##### Definition

**Static Polymorphism**

Static polymorphism is zero-cost, compile-time dispatch using generic parameters constrained by traits.

**Constrained Generic**

A **constrained generic** is a generic parameter `T` constrained by trait `Tr` (written `T <: Tr`), which restricts `T` to types implementing `Tr`.

**Monomorphization**

**Monomorphization** is the process by which the compiler generates specialized versions of generic procedures for each concrete type used as an argument. Each instantiation produces distinct machine code with direct (non-virtual) calls.

##### Syntax & Declaration

**Grammar**

```ebnf
generic_param_constrained ::= <identifier> "<:" <trait_list>
trait_list ::= <type_path> ( "," <type_path> )*
```

##### Static Semantics

**Constraint Satisfaction (T-Constraint)**

A call `f<T>(x)` where `f` requires `T <: Tr` is valid if and only if:

$$\frac{\Gamma \vdash T \text{ implements } Tr}{\Gamma \vdash f\langle T \rangle(x) : \text{valid}}$$
\tag{T-Constraint}

**Monomorphization Semantics**

For a generic procedure:
```cursive
procedure process<T <: Trait>(item: T) { item~>method() }
```

A call `process<Point>(p)` **MUST** be compiled as:
1. Generate specialized function `process_Point`
2. Replace `item~>method()` with direct call `Point::method(item)`
3. No vtable lookup; direct static dispatch

**Zero Runtime Overhead**

Static polymorphism **MUST** incur zero runtime overhead compared to non-generic code. The monomorphized code **MUST** be equivalent in performance to hand-written specialized code.

##### Constraints & Legality

| Code         | Severity | Condition                                          |
| :----------- | :------- | :------------------------------------------------- |
| `E-TRS-2930` | Error    | Type argument does not satisfy trait constraint.   |
| `E-TRS-2931` | Error    | Unconstrained type parameter used in trait method. |

##### Examples

**Valid:**
```cursive
// T is constrained to implement Drawable
procedure render_item<T <: Drawable>(item: T, ctx: Context) {
    item~>draw(ctx)  // Direct call to T::draw, not vtable
}

let p: Point = Point { x: 0.0, y: 0.0 }
render_item<Point>(p, ctx)  // Monomorphized to render_item_Point
```

**Invalid:**
```cursive
// E-TRS-2930: i32 does not implement Drawable
render_item<i32>(42, ctx)
```

---

### 6.5 Dynamic Polymorphism (Witnesses) {Source: Draft 2 §28.4}

##### Definition

**Dynamic Polymorphism**

Dynamic polymorphism is opt-in runtime dispatch using trait witnesses. It enables heterogeneous collections and runtime polymorphism at the cost of one vtable lookup per call.

**Witness**

A **witness** (`witness Trait`) is a concrete, sized type representing any value implementing a witness-safe trait. It is implemented as a dense pointer (fat pointer).

**Witness Safety (Object Safety)**

A trait is **witness-safe** if every procedure in the trait (including inherited ones) is either:
1. **VTable-eligible**, OR
2. **Explicitly excluded** via `where Self: Sized`

**VTable Eligibility**

A procedure is **vtable-eligible** if **ALL** of the following are true:
1. Has a receiver parameter (`self`, `~`, `~!`, `~%`)
2. Has **NO** generic type parameters
3. Does **NOT** return `Self` by value (except as `box Self` or `witness Self`)
4. Does **NOT** use `Self` in parameter types (except via pointer indirection)

##### Syntax & Declaration

**Grammar**

```ebnf
witness_type ::= "witness" <trait_path>
```

##### Static Semantics

**Witness Formation (T-Witness-Form)**

A value of concrete type `T` implementing witness-safe trait `Tr` **MAY** be coerced to `witness Tr`:

$$\frac{\Gamma \vdash v : T \quad \Gamma \vdash T <: Tr \quad \text{witness\_safe}(Tr)}{\Gamma \vdash v : \text{witness } Tr}$$
\tag{T-Witness-Form}

**Dynamic Dispatch Semantics**

A call `w~>method(args)` on witness `w: witness Tr` **MUST**:
1. Load vtable pointer from `w`
2. Load function pointer for `method` from vtable
3. Call function pointer, passing data pointer as `self`

##### Memory & Layout

**Dense Pointer Layout**

A witness (`witness Trait`) is represented as a two-word structure:

```
struct WitnessRepr {
    data: *imm (),      // Pointer to concrete instance
    vtable: *imm VTable // Pointer to trait vtable
}
```

- **Size**: `2 * sizeof(usize)` (16 bytes on 64-bit platforms)
- **Alignment**: `alignof(usize)`

**VTable Layout (Stable ABI)**

VTable entries **MUST** appear in this exact order:
1. `size: usize` — Size of concrete type in bytes
2. `align: usize` — Alignment requirement of concrete type
3. `drop: *imm fn` — Destructor function pointer (null if no `Drop`)
4. `methods[..]` — Function pointers in trait declaration order

**Rationale**: Fixed layout enables separate compilation and FFI vtable compatibility.

##### Constraints & Legality

| Code         | Severity | Condition                                                |
| :----------- | :------- | :------------------------------------------------------- |
| `E-TRS-2940` | Error    | Procedure with `where Self: Sized` called on witness.    |
| `E-TRS-2941` | Error    | Witness created from non-witness-safe trait.             |
| `E-TRS-2942` | Error    | Generic procedure in trait without `Self: Sized` clause. |

##### Examples

**Valid:**
```cursive
// Witness-safe trait
public trait Drawable {
    procedure draw(~, ctx: Context);

    // Excluded from vtable - only available in static dispatch
    procedure clone(~): Self where Self: Sized;
}

// Dynamic dispatch via witness
procedure render_dynamic(item: witness Drawable, ctx: Context) {
    item~>draw(ctx)  // VTable lookup + indirect call
}

// Heterogeneous collection
let shapes: [witness Drawable] = [point, circle, rect]
```

**Invalid:**
```cursive
// E-TRS-2940: Cannot call excluded method on witness
procedure bad(item: witness Drawable) {
    let copy = item~>clone()  // ERROR: clone requires Self: Sized
}
```

---

### 6.6 Opaque Polymorphism {Source: Draft 2 §28.5}

##### Definition

**Opaque Polymorphism**

Opaque polymorphism is zero-cost compile-time polymorphism for return types. The concrete type is hidden from callers; only the trait interface is accessible.

**Opaque Type**

An **opaque return type** (`opaque Trait`) exposes only the trait's interface while hiding the concrete implementation type.

##### Syntax & Declaration

**Grammar**

```ebnf
return_type ::= ... | "opaque" <trait_path>
```

##### Static Semantics

**Type Encapsulation**

For a procedure returning `opaque Trait`:
- The callee returns a concrete type implementing `Trait`
- The caller observes only `Trait` members
- Access to concrete type members is **forbidden**

**Zero Overhead**

Opaque types **MUST** incur zero runtime overhead. The returned value is the concrete type, not a dense pointer. Type encapsulation is enforced statically.

##### Constraints & Legality

| Code         | Severity | Condition                                             |
| :----------- | :------- | :---------------------------------------------------- |
| `E-TRS-2910` | Error    | Accessing member not defined on opaque type's trait.  |
| `E-TRS-2911` | Error    | Opaque return type does not implement required trait. |

##### Examples

**Valid:**
```cursive
// Library code
internal record Widget <: Drawable {
    id: u32,
    procedure draw(~, ctx: Context) { /* ... */ }
}

public procedure make_widget(): opaque Drawable {
    result Widget { id: 123 }
}

// User code
let w = make_widget()
w~>draw(ctx)      // OK: draw is on Drawable trait
// w.id          // ERROR E-TRS-2910: id not on Drawable
```

---

### 6.7 Foundational Traits {Source: Draft 2 §28.6, Appendix D.1}

##### Definition

**Foundational Traits**

Foundational traits are traits fundamental to language semantics. The normative definitions reside in **Appendix D.1**. This section provides a summary and cross-reference.

| Trait      | Purpose                      | Key Constraint               |
| :--------- | :--------------------------- | :--------------------------- |
| `Drop`     | Deterministic cleanup (RAII) | Compiler-invoked only        |
| `Copy`     | Implicit bitwise duplication | Mutual exclusion with `Drop` |
| `Clone`    | Explicit deep copy           | `clone(~): Self`             |
| `Iterator` | Iteration protocol           | `next(~!): Option<Item>`     |

##### Static Semantics

**Drop Semantics (Summary)**

- **Signature**: `procedure drop(~!)`
- The compiler automatically invokes `Drop::drop` on responsible bindings at scope exit
- Direct calls to `Drop::drop` are **FORBIDDEN**

**Copy Semantics (Summary)**

- Marker trait (no procedures)
- Enables implicit bitwise duplication instead of move
- A type **MUST NOT** implement both `Copy` and `Drop`
- All fields of a `Copy` type **MUST** also be `Copy`

**Clone Semantics (Summary)**

- **Signature**: `procedure clone(~): Self`
- Explicit duplication (deep copy)
- `Copy` types **SHOULD** also implement `Clone`

**Iterator Semantics (Summary)**

- **Associated type**: `type Item`
- **Signature**: `procedure next(~!): Option<Item>`
- Stateful advancement; used by `loop ... in` construct

##### Constraints & Legality

| Code         | Severity | Condition                               |
| :----------- | :------- | :-------------------------------------- |
| `E-TRS-2920` | Error    | Direct call to `Drop::drop`.            |
| `E-TRS-2921` | Error    | Type implements both `Copy` and `Drop`. |
| `E-TRS-2922` | Error    | `Copy` type contains non-`Copy` field.  |

---

### Clause 6: Traits and Polymorphism Cross-Reference Notes

**Terms defined in Clause 6 that MUST NOT be redefined elsewhere:**

| Term                    | Section | Description                                                                |
| :---------------------- | :------ | :------------------------------------------------------------------------- |
| Trait                   | §6.1    | A declaration defining an abstract interface of procedures and types.      |
| Abstract Procedure      | §6.2    | A procedure signature in a trait without a body.                           |
| Concrete Procedure      | §6.2    | A procedure definition in a trait with a body (default implementation).    |
| Associated Type         | §6.2    | A type declaration within a trait, either abstract or with a default.      |
| Generic Trait Parameter | §6.2    | Type parameters declared on a trait, specified at use-site.                |
| Supertrait              | §6.2    | A trait required by another trait via `<:` in trait declaration.           |
| Trait Alias             | §6.2    | A named alias for a combination of trait bounds.                           |
| Trait Implementation    | §6.3    | The act of a type satisfying a trait's requirements via `<:`.              |
| Coherence Rule          | §6.3    | A type MAY implement a trait at most once.                                 |
| Orphan Rule             | §6.3    | Trait impl requires locality of type or trait to current assembly.         |
| Monomorphization        | §6.4    | Compile-time specialization of generic code for concrete types.            |
| Constrained Generic     | §6.4    | A generic parameter bounded by one or more traits.                         |
| Witness                 | §6.5    | A dense pointer type enabling runtime polymorphism over a trait.           |
| Witness Safety          | §6.5    | Property of a trait permitting witness creation.                           |
| VTable Eligibility      | §6.5    | Criteria for procedure inclusion in a witness vtable.                      |
| Opaque Type             | §6.6    | A return type hiding the concrete implementation behind a trait interface. |

**Terms referenced:**

| Term                    | Source | Usage in Clause 6                                        |
| :---------------------- | :----- | :------------------------------------------------------- |
| Type Context ($\Gamma$) | §4.1   | Used in typing judgments for trait bounds.               |
| Subtype Relation ($<:$) | §4.2   | Trait implementation creates subtype relationship.       |
| Permission              | §4.5   | Receiver shorthands (`~`, `~!`, `~%`) imply permissions. |
| `const`                 | §4.5   | Default permission for `self` in trait methods.          |
| `unique`                | §4.5   | Permission required for `Drop::drop` receiver.           |
| Record                  | §4.8   | Primary type that implements traits.                     |
| Enum                    | §4.9   | May implement traits.                                    |
| Modal Type              | §4.11  | May implement traits.                                    |
| Visibility              | §5.2   | Controls trait and procedure accessibility.              |
| Assembly                | §5.1   | Compilation unit for orphan rule.                        |

**Terms deferred to later clauses/appendices:**

| Term                   | Deferred To  | Reason                                         |
| :--------------------- | :----------- | :--------------------------------------------- |
| `Drop` (full spec)     | Appendix D.1 | Normative definition with all constraints.     |
| `Copy` (full spec)     | Appendix D.1 | Normative definition with all constraints.     |
| `Clone` (full spec)    | Appendix D.1 | Normative definition with all constraints.     |
| `Iterator` (full spec) | Appendix D.1 | Normative definition with protocol details.    |
| `Context`              | Appendix E   | Root capability record referenced in examples. |

---

## Clause 7: Contracts and Verification

This clause defines the formal semantics, syntax, and verification rules for Contracts in Cursive. Contracts are the primary mechanism for specifying behavioral properties of code beyond the type system. They govern the logical validity of procedures, types, and loops through preconditions, postconditions, and invariants.

### 7.1 Contract Fundamentals {Source: Draft 2 §27.1}

##### Definition

A **Contract** is a specification attached to a procedure declaration that asserts logical predicates over program state. Contracts govern the logical validity of procedures through preconditions (caller obligations) and postconditions (callee guarantees).

**Formal Definition:**

A contract $C$ is a pair $(P_{must}, P_{will})$ where:
- $P_{must}$ is a conjunction of boolean predicates representing preconditions
- $P_{will}$ is a conjunction of boolean predicates representing postconditions

**Distinguished from Capabilities:** Contracts control *logical validity* of data; Capabilities (Clause 10) control *authority* to perform effects.

A **Predicate** is a pure boolean expression evaluated in a specific context (see Static Semantics below).

A **Verification Fact** is a compiler-internal guarantee that a predicate $P$ holds at program location $L$. Verification Facts have zero runtime representation and are used solely for static analysis and optimization.

A **Verification Mode** is a strategy for discharging contract obligations. Cursive supports three modes: `static`, `dynamic`, and `trusted`, plus a hybrid default.

##### Syntax & Declaration

**Grammar:**

```ebnf
procedure_constraint ::= "where" "{" precondition_expr [ "=>" postcondition_expr ] "}"

precondition_expr  ::= logical_expr
postcondition_expr ::= logical_expr

logical_expr ::=
    comparison_expr
  | logical_expr "&&" logical_expr
  | logical_expr "||" logical_expr
  | "!" logical_expr
  | "(" logical_expr ")"
  | match_expr
  | procedure_call
```

**Positioning:** A `where` clause **MUST** appear after the return type (or parameter list if no return type) and before the procedure body or terminating semicolon.

**Relation to Inline Parameter Constraints:**

Inline parameter constraints (§4.19) and procedure `where` clauses use identical predicate syntax and are semantically interchangeable for preconditions. The choice is stylistic:

| Style             | Best For                                      |
| :---------------- | :-------------------------------------------- |
| Inline constraint | Single-parameter preconditions                |
| Procedure `where` | Multi-parameter relationships, postconditions |

**Combining Inline and Procedure Constraints:**

When both forms are present, the effective precondition is their conjunction:

```cursive
procedure example(
    x: i32 where { x > 0 },     // Inline constraint
    y: i32
)
    where { x + y < 100 }       // Procedure constraint
{
    // Effective precondition: x > 0 && x + y < 100
}
```

##### Static Semantics

**Predicate Evaluation Context:**

The `where` clause contains two parts separated by `=>`:

1. **Precondition Expression (left of `=>`):**
   - Contains procedure parameters at entry state
   - Receiver shorthand (`~`, `~!`, etc.) if present
   - **MUST NOT** reference `@result` or `@entry()`

2. **Postcondition Expression (right of `=>`):**
   - Contains procedure parameters (post-state for mutable parameters)
   - `@result`: The procedure's return value
   - `@entry(expr)`: Entry-state capture for any expression
   - Mutable parameters refer to post-state unless wrapped in `@entry()`

**Omission:**
- If no `=>` appears, the entire expression is a precondition
- If `=>` appears with no left side, there is no precondition (always `true`)

**Well-Formedness Rule:**

$$
\frac{
    \Gamma_{pre} \vdash P_{pre} : \text{bool} \land \text{pure}(P_{pre}) \\
    \Gamma_{post} \vdash P_{post} : \text{bool} \land \text{pure}(P_{post})
}{
    \Gamma \vdash \text{where } \{ P_{pre} \Rightarrow P_{post} \} : \text{WF}
}
\tag{WF-Contract}
$$

**Logical Operators:** Predicates use standard boolean operators (`&&`, `||`, `!`). Multiple conditions are combined using these operators rather than comma-separated lists.

##### Constraints & Legality

**Purity Constraint:** All expressions within a contract **MUST** be pure:
- **MUST NOT** perform I/O
- **MUST NOT** mutate state
- **MUST NOT** allocate memory
- **MUST NOT** invoke procedures unless marked `pure` or `const`

**Type Constraint:** Every predicate expression **MUST** evaluate to type `bool`.

**Memory Safety:** Contract expressions **MUST** adhere to the permission system (Clause 3). Accessing `partitioned` fields is forbidden unless within a context guaranteeing exclusive access.

| Code         | Severity | Condition                                              |
| :----------- | :------- | :----------------------------------------------------- |
| `E-CON-2802` | Error    | Impure or unsafe expression used in contract predicate |

---

### 7.2 Contract Clauses {Source: Draft 2 §27.2, §27.3}

This section defines preconditions and postconditions within the unified `where` clause syntax.

#### 7.2.1 Preconditions

##### Definition

A **Precondition** is the logical expression appearing to the left of `=>` in a `where` clause. The caller **MUST** ensure this expression evaluates to `true` at the call site, prior to transfer of control to the callee.

**Inline Parameter Constraints as Preconditions:**

An inline parameter constraint `param: T where { P }` is semantically a precondition on that parameter. It is unified with the procedure's `where` clause during semantic analysis.

##### Static Semantics

**Caller Obligation:** When a procedure $f$ with contract `where { P_{pre} => P_{post} }` is called at site $S$, the implementation **MUST** verify that a Verification Fact $F(P_{pre}, L)$ exists where $L$ dominates $S$ in the Control Flow Graph.

**Unified Precondition:** Given:
- Inline parameter constraints $\{P_1, P_2, \ldots, P_n\}$
- Procedure-level precondition $P_{proc}$

The effective precondition is:
$P_{eff} = P_1 \land P_2 \land \cdots \land P_n \land P_{proc}$

**Attribution:** Failure to satisfy a precondition is a critical error attributed to the **caller**.

**Elision:** If no `=>` is present, the entire expression is treated as a precondition. If `=>` appears with nothing to its left (e.g., `where { => post }`), the precondition defaults to `true`.

##### Constraints & Legality

**Scope:** Precondition expressions **MAY** reference:
- All procedure parameters (at entry state)
- Receiver shorthand (`~`, `~!`, etc.)

Precondition expressions **MUST NOT** reference `@result` or `@entry()`.

---

#### 7.2.2 Postconditions

##### Definition

A **Postcondition** is the logical expression appearing to the right of `=>` in a `where` clause. The callee **MUST** ensure this expression evaluates to `true` immediately before returning control to the caller.

##### Static Semantics

**Callee Obligation:** The procedure implementation **MUST** ensure the postcondition holds at every return point.

**Attribution:** Failure to satisfy a postcondition is a critical error attributed to the **callee**.

**Elision:** If no `=>` is present in the `where` clause, there is no postcondition (defaults to `true`).

##### Dynamic Semantics

**Contract Intrinsics:**

1. **`@result`**
   - Available exclusively in postcondition expressions (right of `=>`)
   - Type: The return type of the procedure
   - Value: The value being returned
   - If procedure returns `()`, `@result` has value `()`
   - **MUST NOT** be shadowed (intrinsic, not identifier)

2. **`@entry(expr)`**
   - Available exclusively in postcondition expressions (right of `=>`)
   - Evaluates `expr` in the entry state (after parameter binding, before body execution)
   - Expression `expr` **MUST** be pure
   - Expression `expr` **MUST** depend only on inputs (parameters, receiver)
   - Result type **MUST** implement `Copy` or `Clone`
   - The implementation captures this value at procedure entry

**Mutable Parameter Semantics:**

For mutable parameters (those with `~!` or `~%` receiver):
- On the **left of `=>`** (precondition): refers to **entry state**
- On the **right of `=>`** (postcondition) **without** `@entry()`: refers to **post-state**
- On the **right of `=>`** (postcondition) **with** `@entry()`: refers to **entry state**

##### Constraints & Legality

**Scope:** Postcondition expressions **MAY** reference:
- All procedure parameters (post-state for mutable parameters)
- Receiver shorthand (`~`, `~!`, etc.)
- `@result` intrinsic
- `@entry(expr)` operator

| Code         | Severity | Condition                                                         |
| :----------- | :------- | :---------------------------------------------------------------- |
| `E-CON-2805` | Error    | `@entry` operator used with non-Copy/Clone result type            |
| `E-CON-2806` | Error    | `@result` intrinsic used outside of postcondition (right of `=>`) |

##### Examples

**Valid:**

```cursive
// Precondition only (no =>)
procedure sqrt(x: f64): f64
    where { x >= 0.0 }
{
    // ...
}

// Postcondition only (just =>)
procedure abs(x: i32): i32
    where { => @result >= 0 }
{
    if x < 0 { -x } else { x }
}

// Both precondition and postcondition
procedure divide(a: i32, b: i32): i32
    where { b != 0 => @result * b == a }
{
    a / b
}

// Complex multi-parameter precondition with logical operators
procedure clamp(x: i32, lo: i32, hi: i32): i32
    where {
        lo <= hi
        =>
        @result >= lo && @result <= hi && (@result == x || @result == lo || @result == hi)
    }
{
    if x < lo { lo }
    else if x > hi { hi }
    else { x }
}

// Stateful with @entry
procedure increment(~! counter: Counter)
    where { => counter.value == @entry(counter.value) + 1 }
{
    counter.value += 1
}

// Precondition and stateful postcondition
procedure pop<T>(~! vec: Vec<T>): T
    where {
        vec~>len() > 0  // Precondition: entry state
        =>
        vec~>len() == @entry(vec~>len()) - 1  // Postcondition: need @entry for pre-state
    }
{
    // ...
}

// Combined refinement type + procedure where
procedure insert_sorted<T>(
    ~! vec: Vec<T> where { is_sorted(self) },
    item: T
): ()
    where {
        is_sorted(@entry(vec))  // Precondition: sorted on entry
        =>
        is_sorted(vec) &&  // Postcondition: sorted on exit
        vec~>len() == @entry(vec~>len()) + 1
    }
{
    // ...
}
```

**Unified syntax examples:**

```cursive
// Inline precondition + procedure postcondition
procedure sqrt(x: f64 where { x >= 0.0 }): f64
    where { => @result >= 0.0 && @result * @result == x }
{
    // ...
}

// Mixed inline and procedure preconditions
procedure binary_search<T>(
    arr: [T] where { is_sorted(arr) },
    target: T
): usize | ()
    where {
        arr~>len() > 0
        =>
        match @result {
            idx: usize => arr[idx] == target,
            () => true
        }
    }
{
    // ...
}
```

**Invalid:**

```cursive
// E-CON-2806: @result used in precondition (left of =>)
procedure bad(x: i32): i32 where { @result > 0 } { x }

// E-CON-2805: Buffer is not Copy/Clone
procedure bad2(~! buf: Buffer): () where { => buf == @entry(buf) } { ... }
```

---

### 7.3 Invariants (`where`) {Source: Draft 2 §27.4}

##### Definition

An **Invariant** is a predicate that **MUST** hold true over a defined scope or lifetime. Cursive uses the `where` keyword for invariants. Two forms are defined in this clause:

1. **Type Invariant:** Attached to `record`, `enum`, or `modal` declarations; constrains all instances
2. **Loop Invariant:** Attached to `loop` expressions; maintained across iterations

> **Note:** Refinement Types (anonymous invariants attached to type references, e.g., `usize where { self < len }`) are defined in Clause 4 §4.19 as they constitute a subtype form with type-theoretic semantics. Verification of refinement type predicates uses the mechanisms defined in §7.4 and §7.5.

##### Syntax & Declaration

**Type Invariant:**

Type invariants are expressed as `where` clauses that follow the record/enum body:

```ebnf
record_with_invariant ::= "record" identifier [generic_params] "{" field_list "}" "where" "{" predicate "}"
enum_with_invariant   ::= "enum" identifier [generic_params] "{" variant_list "}" "where" "{" predicate "}"
```

This is syntactically consistent with refinement types: the `where` clause always follows the type expression it refines.

**Loop Invariant:**

```ebnf
loop_expr ::= "loop" [condition] "where" "{" predicate "}" block
```

##### Static Semantics

**Type Invariant Enforcement Points (Boundary Points):**

1. **Post-Construction:** Immediately after constructor or literal initialization completes
2. **Pre-Call (Public):** Before any public method is invoked on the instance
3. **Post-Call (Mutator):** Before any method taking `unique` or `partitioned` `self` returns

**Effective Postcondition Desugaring:**

For a mutator procedure $P$ on type $T$ with invariant $Inv$ and explicit postcondition $Post$:
$$
\text{EffectivePost}(P) = Post \land Inv(self)
$$

**Loop Invariant Enforcement:**

1. **Initialization:** Before the first iteration
2. **Maintenance:** At the start of every subsequent iteration
3. **Termination:** Immediately after loop terminates

##### Constraints & Legality

**Purity:** Invariant predicates **MUST** satisfy the same purity constraints as contract predicates (§7.1).

**Type:** Invariant predicates **MUST** evaluate to type `bool`.

##### Examples

**Type Invariant:**

```cursive
// Sugar form - where clause follows the record body
record PositiveInt {
    value: i32
} where { self.value > 0 }

// Explicit type alias form
type PositiveInt = record {
    value: i32
} where { self.value > 0 }

// Multi-constraint invariant using logical operators
record BoundedCounter {
    value: i32,
    min: i32,
    max: i32
} where {
    self.min <= self.max &&
    self.value >= self.min &&
    self.value <= self.max
}

// Fraction with non-zero denominator
type Fraction = record {
    num: i32,
    denom: i32
} where { self.denom != 0 }
```

**Loop Invariant:**

```cursive
var i = 0
loop i < 10 where { i >= 0 && i <= 10 } {
    i += 1
}
```

---

### 7.4 Verification Modes {Source: Draft 2 §27.6}

##### Definition

A **Verification Mode** specifies the strategy for discharging contract obligations. The mode is selected via the `[[verify(mode)]]` attribute.

**Mode Enumeration:**

| Mode      | Static Proof | Runtime Check | On Failure | Context Required |
| :-------- | :----------- | :------------ | :--------- | :--------------- |
| `static`  | Required     | None          | Ill-formed | Any              |
| `dynamic` | Elision only | Required      | Panic      | Any              |
| `trusted` | None         | None          | UVB        | `unsafe` block   |
| (default) | Elision only | Required      | Panic      | Any              |

**Default Behavior:**

The default verification strategy (when no `[[verify(...)]]` attribute is present) is **dynamic verification with static elision**:

1. Runtime checks are generated for all contract predicates
2. The compiler **MAY** elide checks that are statically proven to always succeed
3. Failed runtime checks trigger a **Panic**

This default prioritizes safety (contracts are always enforced) while allowing optimization (unnecessary checks are removed).

##### Syntax & Declaration

```ebnf
verify_attribute ::= "[[" "verify" "(" mode ")" "]]"
mode             ::= "static" | "dynamic" | "trusted"
```

##### Static Semantics

**`static` Mode:**

- The implementation **MUST** mathematically prove contract satisfaction via static analysis
- If proof succeeds: no runtime code generated
- If proof fails or is inconclusive: program is **ill-formed**
- **Use Case:** Performance-critical code where runtime checks are unacceptable and predicates are statically decidable

**`dynamic` Mode:**

- The implementation **MUST** generate runtime checks at procedure boundaries
- Preconditions checked at procedure prologue
- Postconditions checked at procedure epilogue
- Purity requirements still apply (no side-effects in predicates)
- Implementation **MAY** elide checks statically proven to always succeed
- **Note:** This is the same as default behavior; `[[verify(dynamic)]]` serves as explicit documentation

**`trusted` Mode:**

- No static proofs attempted
- No runtime checks generated
- Violations produce **Unverifiable Behavior (UVB)**
- **MUST** appear within an `unsafe` block or on an `unsafe` procedure
- Usage outside `unsafe` context triggers error `E-CON-2807`
- Shifts responsibility entirely to programmer
- **Use Case:** FFI boundaries where predicates reference external state that cannot be checked

**Default (No Attribute):**

1. Runtime checks are generated for all predicates
2. Compiler analyzes each check site
3. If static proof succeeds for a check: that check is elided (zero overhead)
4. If static proof fails or is inconclusive: runtime check remains

This is equivalent to `[[verify(dynamic)]]` but the attribute is unnecessary for default behavior.

##### Dynamic Semantics

**Runtime Check Failure:**

1. Trigger panic in current thread
2. Poison the current task state
3. Prevent further execution of verified code region

**Fact Injection:** Successful runtime checks synthesize Verification Facts:
- Facts inserted at check location in CFG
- Facts dominate all program points reachable from check
- Enables optimizer assumptions (e.g., dead code elimination)

##### Constraints & Legality

| Code         | Severity | Condition                                           |
| :----------- | :------- | :-------------------------------------------------- |
| `E-CON-2801` | Error    | Static verification failed in `static` mode         |
| `E-CON-2807` | Error    | `[[verify(trusted)]]` used outside `unsafe` context |

##### Examples

```cursive
// Default: runtime checks with static elision
procedure pop(~! stack: Stack)
    where { stack~>len() > 0 }
{
    // Runtime check generated (elided if caller's context proves len > 0)
}

// Trusted verification - REQUIRES unsafe context
unsafe procedure ffi_bridge(~ ctx: Context)
    where { [[verify(trusted)]] external_invariant_holds() }
{
    // NO checks emitted - programmer guarantees invariant
    // UVB if predicate is false
}

// Alternative: trusted within unsafe block
procedure call_ffi(~ ctx: Context) {
    // ... safe setup code ...

    // SAFETY: external_invariant_holds() is guaranteed by the FFI contract
    // established during ctx initialization.
    unsafe {
        [[verify(trusted)]]
        ffi_do_work(ctx)
            where { external_invariant_holds() }
    }
}

// Static verification - must prove at compile time
[[verify(static)]]
procedure get_first<T>(arr: [T])
    where { arr~>len() > 0 }
{
    result arr[0]  // Compiler must prove len > 0 statically
}

// Explicit dynamic (same as default, for documentation)
[[verify(dynamic)]]
procedure bounds_checked_get<T>(arr: [T], idx: usize): T
    where { idx < arr~>len() }
{
    result arr[idx]
}
```

##### Rationale

**Why Dynamic Default?**

The previous "static-first, dynamic-fallback" default created unpredictable behavior:
- Whether a check existed at runtime depended on verifier capabilities
- Code could silently become unsafe when refactored in ways the verifier couldn't follow
- Performance characteristics were unpredictable across compiler versions

The "dynamic with static elision" default provides:
- Predictable safety: contracts are always enforced unless explicitly trusted
- Predictable performance: developers know checks exist and can measure/profile them
- Safe optimization: static elision is purely an optimization, not a semantic change

**Why Require `unsafe` for `trusted`?**

`trusted` mode produces Unverifiable Behavior on violation—the same category as raw pointer dereference and FFI calls. It belongs behind the same safety barrier. This ensures:
- Code review catches all potential UVB sources by searching for `unsafe`
- The `// SAFETY:` documentation convention applies uniformly
- No "hidden" UVB in ostensibly safe code

---

### 7.5 Verification Facts {Source: Draft 2 §27.7}

##### Definition

A **Verification Fact** $F(P, L)$ is a compiler-internal guarantee that predicate $P$ holds at program location $L$. Verification Facts:
- Have **zero runtime size**
- Have **no runtime representation**
- **MUST NOT** be stored in variables, passed as parameters, or returned from procedures
- Exist solely for static analysis and optimization

##### Static Semantics

**Fact Dominance:**

A Verification Fact $F(P, L)$ satisfies a requirement for predicate $P$ at statement $S$ if and only if $L$ strictly **dominates** $S$ in the Control Flow Graph (CFG).

**Fact Generation:**

Control flow structures generate Verification Facts:

| Construct               | Generated Fact                              |
| :---------------------- | :------------------------------------------ |
| `if P { ... }`          | $F(P, \text{then-branch entry})$            |
| `if !P { } else {...}`  | $F(P, \text{else-branch entry})$            |
| `match x { Pat => ...}` | $F(\text{x matches Pat}, \text{arm entry})$ |
| Runtime check for $P$   | $F(P, \text{post-check})$                   |

**Type Narrowing:**

If Fact $F(P, L)$ is active for binding `x` at location $L$, the type of `x` is effectively refined to:
$$
\text{typeof}(x) \text{ where } \{P\}
$$

##### Dynamic Semantics

**Dynamic Fact Injection (for `dynamic` mode):**

When verification mode is `dynamic` (or hybrid fallback):

1. Identify requirement $P$ at call site $S$
2. If no Fact $F(P, L)$ dominates $S$:
   a. Construct check block $C$: `if !P { panic("Contract violation") }`
   b. Insert $C$ into CFG such that $C$ dominates $S$
   c. Successful execution of $C$ establishes $F(P, \text{exit}(C))$

**Optimizer Integration:**

In optimized builds, implementations **SHOULD** use facts as optimization hints:
- Treat $P$ as invariant truth for all instructions dominated by $F(P, L)$
- Eliminate branches dependent on $\neg P$ dominated by $F(P, L)$
- Corresponds to intrinsics like `llvm.assume` in backend IRs

In `trusted` mode (within `unsafe` context), the predicate is assumed true without generating a fact through verification. The optimizer **MAY** use the assumption, but this is UVB if the predicate is false.

---

### 7.6 Behavioral Subtyping (Liskov Substitution) {Source: Draft 2 §27.5}

##### Definition

When a type implements a trait (Clause 6), procedure implementations **MUST** adhere to the **behavioral subtyping principle** (Liskov Substitution Principle) with respect to trait contracts.

##### Static Semantics

**Precondition Weakening:**

An implementation **MAY** weaken (require less) the preconditions defined in the trait. An implementation **MUST NOT** strengthen (require more) the preconditions.

$$P_{trait} \implies P_{impl}$$

*Intuition:* If the trait says "you must provide X", the implementation may accept X or something easier.

**Postcondition Strengthening:**

An implementation **MAY** strengthen (guarantee more) the postconditions defined in the trait. An implementation **MUST NOT** weaken (guarantee less) the postconditions.

$$Q_{impl} \implies Q_{trait}$$

*Intuition:* If the trait promises "you will get Y", the implementation must deliver Y or something better.

##### Constraints & Legality

| Code         | Severity | Condition                                   |
| :----------- | :------- | :------------------------------------------ |
| `E-CON-2803` | Error    | Precondition strengthened in implementation |
| `E-CON-2804` | Error    | Postcondition weakened in implementation    |

##### Examples

```cursive
trait Stack<T> {
    // Trait contract: caller must ensure non-empty
    procedure pop(~!): T where { self~>len() > 0 };
}

record MyStack<T> <: Stack<T> {
    // VALID: Weakened precondition (accepts len >= 0, returns Option)
    procedure pop(~!): Option<T>
        where {
            self~>len() >= 0  // Weakened: accepts empty stacks
            =>
            @result~>is_some() == (@entry(self~>len()) > 0)  // Strengthened: better guarantee
        }
    {
        // ...
    }
}

// INVALID example - would produce E-CON-2803
trait Container<T> {
    procedure get(~, idx: usize): T where { idx < self~>len() };
}

record MyContainer<T> <: Container<T> {
    // ERROR: Precondition strengthened (more restrictive than trait)
    procedure get(~, idx: usize): T
        where { idx < self~>len() && idx > 0 }  // E-CON-2803: idx > 0 not in trait
    {
        // ...
    }
}
```

---

### 7.7 Diagnostics Summary

| Code         | Severity | Description                                                       |
| :----------- | :------- | :---------------------------------------------------------------- |
| `E-CON-2801` | Error    | Static verification failed: Unable to prove contract              |
| `E-CON-2802` | Error    | Impure or unsafe expression used in contract predicate            |
| `E-CON-2803` | Error    | Liskov violation: Precondition strengthened in implementation     |
| `E-CON-2804` | Error    | Liskov violation: Postcondition weakened in implementation        |
| `E-CON-2805` | Error    | `@entry` operator used with non-Copy/Clone result type            |
| `E-CON-2806` | Error    | `@result` intrinsic used outside of postcondition (right of `=>`) |
| `E-CON-2807` | Error    | `[[verify(trusted)]]` used outside `unsafe` context               |

---

### Clause 7: Cross-Reference Notes

**Terms defined in Clause 7 that MUST NOT be redefined elsewhere:**

| Term                 | Section | Description                                                     |
| :------------------- | :------ | :-------------------------------------------------------------- |
| Contract             | §7.1    | Specification attached to procedure declaration                 |
| Predicate            | §7.1    | Pure boolean expression in contract context                     |
| Precondition         | §7.2.1  | Caller obligation (left of `=>` or inline parameter constraint) |
| Postcondition        | §7.2.2  | Callee guarantee (right of `=>` in `where` clause)              |
| `@result`            | §7.2.2  | Intrinsic for return value in postconditions                    |
| `@entry`             | §7.2.2  | Operator for entry-state capture                                |
| Type Invariant       | §7.3    | Predicate constraining all instances of a type                  |
| Loop Invariant       | §7.3    | Predicate maintained across loop iterations                     |
| Verification Mode    | §7.4    | Strategy for discharging contract obligations                   |
| Verification Fact    | §7.5    | Compiler-internal predicate satisfaction proof                  |
| Behavioral Subtyping | §7.6    | Contract inheritance rules (Liskov)                             |

**Terms referenced from other clauses:**

| Term                  | Source     | Usage in Clause 7                                   |
| :-------------------- | :--------- | :-------------------------------------------------- |
| Unverifiable Behavior | Clause 1   | `trusted` mode (in `unsafe`) violations produce UVB |
| Unsafe Block          | Clause 3   | Required context for `[[verify(trusted)]]`          |
| Panic                 | Clause 3   | Runtime contract failure triggers panic             |
| `Copy` / `Clone`      | Clause 6   | Required for `@entry` capture types                 |
| Trait                 | Clause 6   | Behavioral subtyping applies to traits              |
| Permission System     | Clause 3   | Contracts must respect permissions                  |
| Refinement Type       | §4.19      | Type narrowing produces refinement type             |
| Control Flow Graph    | (Implicit) | Fact dominance defined via CFG                      |

**Terms deferred to other clauses:**

| Term             | Deferred To   | Reason                                               |
| :--------------- | :------------ | :--------------------------------------------------- |
| `partition` stmt | Clause 3 §3.7 | Defined in The Partitioning System (primary concern) |
| Capability       | Clause 10     | Distinguished from contracts in §7.1                 |


---

## Clause 8: Expressions & Statements

This clause defines the syntax and semantics of expressions and statements in Cursive. Expressions are syntactic forms that produce typed values; statements are syntactic forms executed for their side effects. This clause establishes the foundational abstractions (the "nouns") before describing the operations on them (the "verbs").

### 8.1 Foundational Definitions {Source: Draft 2 §24.1, §25.1}

##### Definition

**Expression**

An **Expression** is a syntactic form that, when evaluated, produces a typed value. Every expression has:
-   **Type**: A compile-time type $T$ determined by static analysis.
-   **Value Category**: Classification as either a *place expression* or *value expression*.
-   **Evaluation Result**: A runtime value conforming to the expression's type.

**Statement**

A **Statement** is a syntactic form that is executed for its side effects and does not produce a value. Statements form the executable body of procedures, loops, and other control-flow blocks.

**Statement Categories**

1.  **Declaration Statements**: Introduce new bindings (`let`, `var`).
2.  **Assignment Statements**: Modify the value stored in a place.
3.  **Expression Statements**: Execute an expression for its side effects.
4.  **Control Flow Statements**: Alter the flow of execution (`return`, `result`, `break`, `continue`).
5.  **Defer Statements**: Schedule cleanup code.

**Typing Context**

A **Typing Context** $\Gamma$ is a finite ordered sequence of bindings:

$$\Gamma ::= \emptyset \mid \Gamma, x : T$$

where $x$ is an identifier and $T$ is a type. The judgment $\Gamma \vdash e : T$ asserts that expression $e$ has type $T$ under context $\Gamma$.

**Value Categories**

Every expression is classified into exactly one **value category**:

| Category         | Notation    | Definition                                       |
| :--------------- | :---------- | :----------------------------------------------- |
| Place Expression | $e^{place}$ | Denotes a memory location with stable address    |
| Value Expression | $e^{value}$ | Produces a temporary without persistent location |

**Formal Classification Rules:**
- Identifiers bound by `let`/`var`: Place
- Dereferenced pointers (`*p`): Place
- Field access on place (`place.field`): Place
- Indexed access on place (`place[i]`): Place
- Literals, arithmetic results, procedure returns: Value

##### Static Semantics

**Evaluation Order**

Cursive's evaluation order **MUST** be deterministic. For any compound expression, subexpressions **MUST** be evaluated strictly left-to-right, except for short-circuit operators (`&&`, `||`) defined in §8.4.

**Sequential Statement Execution**

Statements within a block **MUST** be executed sequentially in source order. The effects of a statement **MUST** be fully complete before the next statement begins.

**Statement Termination**

Statement termination is governed by §2.11. A statement is terminated by a `<newline>` token unless a continuation condition is met, or by an explicit semicolon (`;`).

##### Constraints & Legality

| Code         | Severity | Condition                                                 |
| :----------- | :------- | :-------------------------------------------------------- |
| `E-EXP-2502` | Error    | Value expression used where place expression is required. |
| `E-EXP-2501` | Error    | Type mismatch (expected type differs from actual type).   |

---

### 8.2 Pattern Matching {Source: Draft 2 §26}

*[RATIONALE: Patterns are placed early because they are used by both declaration statements (§8.7) and match expressions (§8.5). This follows the "nouns before verbs" principle.]*

##### Definition

**Pattern**

A **Pattern** is a syntactic form that:
1.  Tests whether a value has a particular shape (for refutable patterns), and
2.  Binds parts of the value to identifiers.

**Irrefutable Pattern**

An **irrefutable pattern** is a pattern that will match for any value of its expected type. Irrefutable patterns include:
-   Identifier patterns (`x`)
-   Wildcard patterns (`_`)
-   Tuple/record patterns composed entirely of irrefutable sub-patterns
-   Single-variant enum patterns

**Refutable Pattern**

A **refutable pattern** is a pattern that can fail to match a value of its expected type. Refutable patterns include:
-   Literal patterns (`42`, `"hello"`)
-   Multi-variant enum patterns
-   Modal state patterns (`@State`)
-   Range patterns

**Pattern Binding Judgment**

The judgment $\Gamma \vdash p : T \Rightarrow \Gamma'$ asserts that pattern $p$ matches type $T$ and extends context $\Gamma$ with bindings to produce $\Gamma'$.

##### Syntax & Declaration

**Grammar**

```ebnf
pattern ::=
    literal                                    (* Literal pattern *)
  | "_"                                        (* Wildcard pattern *)
  | identifier                                 (* Identifier pattern *)
  | "(" ( pattern ( "," pattern )* ","? )? ")" (* Tuple pattern *)
  | path "{" ( field_pattern ( "," field_pattern )* ","? )? "}"  (* Record pattern *)
  | path                                       (* Unit-like enum variant *)
  | path "(" pattern_list ")"                  (* Tuple-like enum variant *)
  | path "{" field_pattern_list "}"            (* Record-like enum variant *)
  | "@" state_identifier [ "{" field_pattern_list "}" ]  (* Modal state pattern *)
  | pattern ".." pattern                       (* Range pattern *)

field_pattern ::= identifier [ ":" pattern ]
```

**Pattern Summary**

| Pattern Type | Example                       | Refutability  | Description                       |
| :----------- | :---------------------------- | :------------ | :-------------------------------- |
| Literal      | `100`, `"hello"`, `true`      | Refutable     | Matches if value equals literal.  |
| Wildcard     | `_`                           | Irrefutable   | Matches any value; binds nothing. |
| Identifier   | `x`                           | Irrefutable   | Matches any value; binds to `x`.  |
| Tuple        | `(p1, p2, ...)`               | Conditional*  | Destructures a tuple.             |
| Record       | `MyRecord { field: p1, ... }` | Conditional*  | Destructures a record.            |
| Enum         | `MyEnum::Variant`             | Conditional** | Matches a specific enum variant.  |
| Modal State  | `@State { field: p, ... }`    | Refutable     | Matches a specific modal state.   |
| Range        | `0..10`                       | Refutable     | Matches if value is within range. |

*Irrefutable only if all sub-patterns are irrefutable.
**Irrefutable if and only if the enum has exactly one variant.

##### Static Semantics

**Matching Rules**

1.  **Literal Pattern**: Matches if `scrutinee == literal`.
2.  **Wildcard (`_`)**: Matches any value; binds nothing.
3.  **Identifier Pattern (`x`)**: Matches any value; binds value to `x`. Move semantics apply unless `ref` keyword is used.
4.  **Variant Pattern (`Enum::Variant(p...)`)**: Matches if scrutinee's discriminant equals Variant; recursively matches payload fields against sub-patterns.
5.  **Range Pattern (`start..end`)**: Matches if `start <= scrutinee < end`.
6.  **Modal Pattern (`@State { ... }`)**: Matches if scrutinee's active state is `@State`; destructures state payload.

**Pattern Binding Scopes**

-   **Let statement**: Bindings are introduced into the current scope. Requires `shadow` keyword if shadowing an outer binding.
-   **Match arm**: Bindings are introduced into an arm-local scope that encompasses only the arm body. Implicit shadowing is permitted (NO `shadow` keyword required).

**Exhaustiveness**

A `match` expression **MUST** be exhaustive. The set of patterns in its arms, taken together, **MUST** cover every possible value of the scrutinee type.

Exhaustiveness checking is **MANDATORY** for:
-   `enum` types
-   `modal` types
-   `bool` type

**Unreachability**

A match arm **MUST NOT** be unreachable. An arm is unreachable if its pattern covers only values already covered by preceding arms.

##### Constraints & Legality

| Code         | Severity | Condition                                                                |
| :----------- | :------- | :----------------------------------------------------------------------- |
| `E-PAT-2711` | Error    | A refutable pattern was used in an irrefutable context (e.g., `let`).    |
| `E-PAT-2741` | Error    | `match` expression is not exhaustive for its scrutinee type.             |
| `E-PAT-2751` | Error    | `match` arm is unreachable because its pattern is covered by prior arms. |

##### Examples

**Valid: Exhaustive Match**
```cursive
enum Status {
    Running,
    Finished,
}

procedure check(s: Status) {
    match s {
        Status::Running => { /* ... */ }
        Status::Finished => { /* ... */ }
    }
}
```

**Invalid: Non-Exhaustive Match (E-PAT-2741)**
```cursive
match s {
    Status::Running => { /* ... */ }
    // Missing: Status::Finished
}
```

**Invalid: Unreachable Arm (E-PAT-2751)**
```cursive
match s {
    _ => { /* Wildcard covers all */ }
    Status::Running => { /* Unreachable */ }
}
```

---

### 8.3 Operator Precedence and Associativity {Source: Draft 2 §24.2}

##### Definition

**Precedence**

**Operator Precedence** is an integer ranking that determines how tightly an operator binds to its operands. Higher precedence operators bind more tightly than lower precedence operators.

**Associativity**

**Associativity** determines how operators of the same precedence are grouped when parentheses are absent:
-   **Left-associative**: Groups left-to-right. `a op b op c` parses as `(a op b) op c`.
-   **Right-associative**: Groups right-to-left. `a op b op c` parses as `a op (b op c)`.

##### Syntax & Declaration

**Precedence Table** (Highest to Lowest)

| Precedence | Operator(s)                                              | Description                             | Associativity |
| :--------- | :------------------------------------------------------- | :-------------------------------------- | :------------ |
| 1          | `()` `[]` `.` `~>` `::`                                  | Call, Index, Field, Method, Scope       | Left          |
| 2          | `=>`                                                     | Pipeline                                | Left          |
| 3          | `!` `-` `&` `*` `^` `move`                               | Unary Not, Neg, Ref, Deref, Alloc, Move | Right         |
| 4          | `**`                                                     | Power                                   | Right         |
| 5          | `*` `/` `%`                                              | Multiplicative                          | Left          |
| 6          | `+` `-`                                                  | Additive                                | Left          |
| 7          | `<<` `>>`                                                | Bitwise Shift                           | Left          |
| 8          | `&`                                                      | Bitwise AND                             | Left          |
| 9          | `^`                                                      | Bitwise XOR                             | Left          |
| 10         | `\|`                                                     | Bitwise OR                              | Left          |
| 11         | `==` `!=` `<` `<=` `>` `>=`                              | Comparison                              | Left          |
| 12         | `&&`                                                     | Logical AND                             | Left          |
| 13         | `\|\|`                                                   | Logical OR                              | Left          |
| 14         | `=` `+=` `-=` `*=` `/=` `%=` `&=` `\|=` `^=` `<<=` `>>=` | Assignment                              | Right         |

##### Static Semantics

**Maximal Munch**

The lexer **MUST** emit the longest valid token at each position (maximal munch rule). For example, `>>` is tokenized as a single right-shift operator, not two `>` tokens.

**Generic Angle Bracket Exception**

Inside generic argument lists (e.g., `Vec<Option<i32>>`), the token sequence `>>` **MUST** be split into two separate `>` tokens to correctly close nested type parameters.

---

### 8.4 Primary Expressions and Operators {Source: Draft 2 §24.3, §24.4}

*[RATIONALE: All operator categories are consolidated into content blocks within this section, avoiding excessive h4 nesting. The operators share common typing infrastructure.]*

##### Definition

**Primary Expressions** are the fundamental operand forms: literals, identifiers, and parenthesized expressions.

**Postfix Expressions** extend primary expressions with field access, indexing, calls, and method dispatch.

##### Syntax & Declaration

**Grammar**

```ebnf
primary_expr ::=
    literal
  | identifier
  | "(" expression ")"

postfix_expr ::=
    primary_expr
  | postfix_expr "." identifier          (* Field access *)
  | postfix_expr "." integer_literal     (* Tuple access *)
  | postfix_expr "[" expression "]"      (* Indexing *)
  | postfix_expr "(" argument_list? ")"  (* Procedure call *)
  | postfix_expr "~>" identifier "(" argument_list? ")"  (* Instance method *)
  | type_path "::" identifier "(" argument_list? ")"     (* Static method *)
  | postfix_expr "=>" expression         (* Pipeline *)
```

**Literals**

A **literal** (e.g., `123`, `"hello"`, `true`) is a primary expression. Literal typing is defined in §2.5.

**Identifiers**

An **identifier** is a primary expression that **MUST** resolve to a value binding according to the name lookup rules in §5.

**Parenthesized Expressions**

A **parenthesized expression** `(<expression>)` has the same value, type, and value category as the enclosed expression.

**Field Access (`.`)**

A field access expression `record_expr.field` **MUST** be applied to an expression of a `record` type. The `field` **MUST** be a declared field of that record and **MUST** be visible in the current scope.

A tuple access expression `tuple_expr.index` **MUST** be applied to an expression of a tuple type. The `index` **MUST** be a constant, non-negative integer literal that is a valid zero-based index for the tuple.

**Indexing (`[]`)**

An indexing expression `sequence_expr[index_expr]` **MUST** be applied to an expression of an `array` type or a `slice` type. The `index_expr` **MUST** have the type `usize`.

**Procedure Call (`()`)**

A procedure call `callable_expr(arg1, arg2, ...)` **MUST** be applied to an expression of a procedure type.

**Method Calls (`~>` and `::`)**

1.  **Instance Method Call (`~>`):** The **Receiver Dispatch Operator** `~>` **MUST** be used for calling methods on a value instance:
    ```cursive
    receiver_expr~>method_name(arg1, ...)
    ```

2.  **Static/Disambiguated Call (`::`):** The double-colon operator `::` **MUST** be used for static method calls and trait disambiguation:
    ```cursive
    MyType::new()
    Trait::method(receiver)
    ```

**Pipeline (`=>`)**

The pipeline expression `lhs_expr => rhs_expr` is syntactic sugar for a procedure call `rhs_expr(lhs_expr)`.

##### Static Semantics

**Typing Rules**

**T-IDENT**: Identifier Typing
$$\frac{(x : T) \in \Gamma}{\Gamma \vdash x : T}$$

**T-FIELD**: Field Access Typing
$$\frac{\Gamma \vdash e : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \}}{\Gamma \vdash e.f : T}$$

**T-INDEX**: Indexing Typing
$$\frac{\Gamma \vdash e_1 : [T; N] \quad \Gamma \vdash e_2 : \texttt{usize}}{\Gamma \vdash e_1[e_2] : T}$$

*Note: The `move` expression typing rule (T-MOVE) is defined in **§3.5 Responsibility & Move Semantics**, not here.*

**Typing Rules (Operators)**

**Logical Operators** (`!`, `&&`, `||`):
$$\frac{\Gamma \vdash e : \texttt{bool}}{\Gamma \vdash !e : \texttt{bool}} \quad \frac{\Gamma \vdash e_1 : \texttt{bool} \quad \Gamma \vdash e_2 : \texttt{bool}}{\Gamma \vdash e_1 \texttt{ \&\& } e_2 : \texttt{bool}}$$

**Arithmetic Operators** (`+`, `-`, `*`, `/`, `%`, `**`):
$$\frac{\Gamma \vdash e_1 : N \quad \Gamma \vdash e_2 : N \quad N \in \text{Numeric}}{\Gamma \vdash e_1 \oplus e_2 : N}$$
No implicit promotions. Homogeneous operand types required.

**Bitwise Operators** (`&`, `|`, `^`, `<<`, `>>`, `!` on integers):
$$\frac{\Gamma \vdash e_1 : I \quad \Gamma \vdash e_2 : I \quad I \in \text{Integer}}{\Gamma \vdash e_1 \oplus e_2 : I}$$

**Comparison Operators** (`==`, `!=`, `<`, `<=`, `>`, `>=`):
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T}{\Gamma \vdash e_1 \texttt{ cmp } e_2 : \texttt{bool}}$$

**Address-Of (`&`)**:
$$\frac{\Gamma \vdash e : T \quad e^{place}}{\Gamma \vdash \&e : \texttt{Ptr}<T>\texttt{@Valid}}$$

**Dereference (`*`)**:
$$\frac{\Gamma \vdash e : \texttt{Ptr}<T>\texttt{@}S \quad S \neq \texttt{Null}}{\Gamma \vdash *e : T}$$

**Region Allocation (`^`)**: Allocates operand into innermost active `region`. Full semantics defined in **§3.9 Regions and Arena Allocation**.

**Receiver Dispatch Algorithm (`~>`)**

1.  Search for method `m` in the inherent methods of type `T` of receiver `x`.
2.  If not found, search for method `m` in all Traits implemented by `T` that are visible in the current scope.
3.  If ambiguous (multiple traits provide `m`), trigger `E-NAM-1305` (require disambiguation via `Trait::m(x)`).
4.  Auto-dereference and auto-reference of `x` to match method receiver (`self`, `*self`, `&self`) is **NOT** performed. Strict receiver matching is required.

##### Dynamic Semantics

**Short-Circuit Evaluation**

Logical `&&` and `||` **MUST** implement short-circuit evaluation:
-   `e1 && e2`: If `e1` evaluates to `false`, `e2` is NOT evaluated; result is `false`.
-   `e1 || e2`: If `e1` evaluates to `true`, `e2` is NOT evaluated; result is `true`.

**Bounds Checking**

All indexing operations **MUST** be bounds-checked at runtime. An out-of-bounds access **MUST** cause the executing thread to panic.

**Overflow Behavior**

Integer arithmetic operations (`+`, `-`, `*`) **MUST** panic on overflow in `strict` mode. Wrap-around behavior in `release` mode is Implementation-Defined Behavior (IDB).

**Division by Zero**

Division by zero **MUST** cause the executing thread to panic in all modes.

**Pipeline Desugaring**

The expression `x => f` desugars to `f(x)` before type checking.

##### Constraints & Legality

| Code         | Severity | Condition                                                 |
| :----------- | :------- | :-------------------------------------------------------- |
| `E-EXP-2511` | Error    | Identifier resolves to a type or module, not a value.     |
| `E-EXP-2531` | Error    | Invalid field or tuple index, or field is not visible.    |
| `E-EXP-2532` | Error    | Mismatched number of arguments in procedure call.         |
| `E-EXP-2533` | Error    | Method call using `.` instead of `~>`.                    |
| `E-EXP-2535` | Error    | Right-hand side of pipeline `=>` is not a valid callable. |
| `E-EXP-2541` | Error    | Logical operator applied to non-bool operand.             |
| `E-EXP-2542` | Error    | Invalid or mismatched types for arithmetic/bitwise.       |
| `E-EXP-2545` | Error    | Address-of operator `&` applied to non-place expression.  |
| `E-MEM-3021` | Error    | Region allocation `^` outside region scope. (See §3.9)    |
| `E-MEM-3030` | Error    | Pointer arithmetic outside `unsafe` block. (See §3.10)    |
| `E-MEM-3001` | Error    | Use of moved value. (Defined in §3.5)                     |
| `E-NAM-1305` | Error    | Ambiguous method resolution; requires disambiguation.     |

---

### 8.5 Control Flow Expressions {Source: Draft 2 §24.6-§24.8}

##### Definition

**Conditional Expression (`if`)**: Evaluates one of two branches based on boolean condition.

**Match Expression**: Exhaustive pattern matching on a scrutinee.

**Loop Expression**: Iteration construct with three forms (infinite, conditional, iterator).

##### Syntax & Declaration

```ebnf
if_expr   ::= "if" expression block [ "else" ( block | if_expr ) ]
match_expr ::= "match" expression "{" match_arm+ "}"
match_arm  ::= pattern "=>" expression ","

loop_expr ::=
    [ label ":" ] "loop" block                              (* Infinite *)
  | [ label ":" ] "loop" expression block                   (* Conditional *)
  | [ label ":" ] "loop" pattern ":" type "in" expression block  (* Iterator *)

label ::= "'" identifier
```

##### Static Semantics

**T-IF**:
$$\frac{\Gamma \vdash e_1 : \texttt{bool} \quad \Gamma \vdash e_2 : T \quad \Gamma \vdash e_3 : T}{\Gamma \vdash \texttt{if } e_1 \{ e_2 \} \texttt{ else } \{ e_3 \} : T}$$

**T-IF-NO-ELSE**:
$$\frac{\Gamma \vdash e_1 : \texttt{bool} \quad \Gamma \vdash e_2 : ()}{\Gamma \vdash \texttt{if } e_1 \{ e_2 \} : ()}$$

**T-MATCH**:
$$\frac{\Gamma \vdash e : T_s \quad \forall i.\, \Gamma, \text{bindings}(p_i) \vdash e_i : T \quad \text{exhaustive}(\{p_i\}, T_s)}{\Gamma \vdash \texttt{match } e \{ p_1 \Rightarrow e_1, \ldots \} : T}$$

**Loop Types**:
- Infinite loop (`loop { }`): Type `!` unless `break <value>` present.
- Conditional loop (`loop cond { }`): Type `()`.
- Iterator loop: Type `()`.
- With `break <value>`: All `break`s **MUST** provide same type; that type becomes loop type.

##### Dynamic Semantics

**Conditional Desugaring**: `loop cond { body }` → `loop { if !cond { break } body }`

**Iterator Desugaring**: `loop x: T in iter { body }` → match on `iter~>next()`.

**Match Execution**: Patterns tested in declaration order; first match executes.

##### Constraints & Legality

| Code         | Severity | Condition                                 |
| :----------- | :------- | :---------------------------------------- |
| `E-EXP-2561` | Error    | `if` without `else` in non-unit context.  |
| `E-EXP-2571` | Error    | `match` arms have incompatible types.     |
| `E-EXP-2582` | Error    | `break` statements have mismatched types. |

---

### 8.6 Block Expressions {Source: Draft 2 §24.9}

##### Definition

**Block Expression**: A block expression `{ stmt*; [result_expr] }` introduces a new lexical scope and evaluates to a value.

##### Syntax & Declaration

```ebnf
block_expr ::= "{" statement* [ expression ] "}"
```

##### Static Semantics

**Block Type**:
1. Type of explicit `result` statement, OR
2. Type of final unterminated expression, OR
3. `()` if empty or semicolon-terminated.

##### Dynamic Semantics

Statements executed sequentially. Block returns value per Static Semantics rules.

##### Cross-References (Specialized Blocks)

The following block forms have specialized semantics defined in other clauses:

| Block Form                   | Authoritative Section                 |
| :--------------------------- | :------------------------------------ |
| `region { ... }`             | **§3.9** Regions and Arena Allocation |
| `unsafe { ... }`             | **§3.10** Unsafe Memory Operations    |
| `parallel(bindings) { ... }` | **§9.1** Data Parallelism             |
| `comptime { ... }`           | **§11.1** Compile-Time Execution      |

Clause 8 does NOT re-specify these blocks; see the authoritative sections for full syntax and semantics.

---

### 8.7 Declaration Statements {Source: Draft 2 §25.2}

##### Definition

Declaration statements introduce new bindings into the current scope.

##### Syntax & Declaration

```ebnf
let_stmt ::= "let" pattern [ ":" type ] "=" expression
var_stmt ::= "var" pattern [ ":" type ] "=" expression
```

##### Static Semantics

- `let`: Immutable binding (identifier cannot be rebound).
- `var`: Mutable binding (identifier can be rebound).
- **Orthogonality**: Binding mutability is orthogonal to data permission (`unique`, `const`, etc.). See **§3.4 The Binding Model**.
- **Responsibility**: The `=` operator establishes cleanup responsibility per **§3.4** and **§3.5**.
- **Pattern**: **MUST** be irrefutable (see §8.2).

##### Constraints & Legality

| Code         | Severity | Condition                         |
| :----------- | :------- | :-------------------------------- |
| `E-PAT-2711` | Error    | Refutable pattern in `let`/`var`. |
| `E-DEC-2401` | Error    | Reassignment of immutable `let`.  |

---

### 8.8 Assignment Statements {Source: Draft 2 §25.3}

##### Definition

Assignment statements modify the value stored in a place expression.

##### Syntax & Declaration

```ebnf
assignment_stmt ::= place_expr assignment_operator expression
assignment_operator ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
```

**Compound Assignment**

Compound assignment `place op= expr` desugars to `place = place op expr`.

##### Static Semantics

1.  The left-hand side **MUST** be a place expression.
2.  The place expression **MUST** refer to a mutable binding (declared with `var`).
3.  The place expression **MUST** be accessible via a `unique` or `partitioned` permission (NOT `const`).
4.  The right-hand side expression **MUST** be type-compatible with the place.

##### Dynamic Semantics

**Drop on Reassignment**

If the place expression refers to an initialized, responsible binding, the implementation **MUST** invoke the destructor (`Drop::drop`) of the current value. This destruction **MUST** occur deterministically before the new value is installed.

##### Constraints & Legality

| Code         | Severity | Condition                                 |
| :----------- | :------- | :---------------------------------------- |
| `E-STM-2631` | Error    | Assignment target is not a place.         |
| `E-DEC-2401` | Error    | Re-assignment of immutable `let` binding. |

---

### 8.9 Control Flow Statements {Source: Draft 2 §25.6}

##### Definition

**`return`**: Terminates procedure, returns to caller.
**`result`**: Terminates block, yields value as block result.
**`break`**: Terminates innermost `loop` (or labeled loop).
**`continue`**: Skips to next iteration.

##### Syntax & Declaration

```ebnf
return_stmt   ::= "return" [ expression ]
result_stmt   ::= "result" expression
break_stmt    ::= "break" [ label ] [ expression ]
continue_stmt ::= "continue" [ label ]
```

##### Static Semantics

- `return` expression type **MUST** match procedure return type.
- `result` expression type becomes enclosing block type.
- `break` with value: all `break`s for same loop **MUST** provide same type.
- `break`/`continue` **MUST** be within `loop`.

##### Dynamic Semantics

- `return`: Executes deferred actions in LIFO order, then returns.
- `result`: Exits immediate block only (not procedure).
- `break`/`continue`: Target innermost or labeled loop.

##### Constraints & Legality

| Code         | Severity | Condition                  |
| :----------- | :------- | :------------------------- |
| `E-STM-2661` | Error    | `return` type mismatch.    |
| `E-STM-2662` | Error    | `break` outside `loop`.    |
| `E-STM-2663` | Error    | `continue` outside `loop`. |
| `E-STM-2664` | Error    | `result` type mismatch.    |

---

### 8.10 Defer Statement {Source: Draft 2 §25.5}

##### Definition

A `defer` statement schedules code for execution at scope exit.

##### Syntax & Declaration

```ebnf
defer_stmt ::= "defer" block
```

##### Static Semantics

- Block **MUST** have type `()`.
- Block **MUST NOT** contain non-local control flow (`return`, `break`, `continue`).

##### Dynamic Semantics

**LIFO Execution**: On scope exit (normal, `return`, `result`, `break`), deferred blocks execute Last-In, First-Out.

##### Constraints & Legality

| Code         | Severity | Condition                        |
| :----------- | :------- | :------------------------------- |
| `E-STM-2651` | Error    | Defer block returns non-unit.    |
| `E-STM-2652` | Error    | Non-local control flow in defer. |

---

### 8.11 Diagnostics Summary

*Note: The `partition` contract statement is NOT defined in Clause 8. It is fully specified in **§3.7 The Partitioning System** where its PRIMARY concern (aliasing proof) resides.*

---

**Diagnostics Defined in Clause 8:**

| Code         | Severity | Description                               |
| :----------- | :------- | :---------------------------------------- |
| `E-EXP-2501` | Error    | Type mismatch in expression.              |
| `E-EXP-2502` | Error    | Value expression where place required.    |
| `E-EXP-2511` | Error    | Identifier resolves to type/module.       |
| `E-EXP-2531` | Error    | Invalid field/tuple index.                |
| `E-EXP-2532` | Error    | Argument count mismatch.                  |
| `E-EXP-2533` | Error    | Method call using `.` instead of `~>`.    |
| `E-EXP-2535` | Error    | Pipeline RHS not callable.                |
| `E-EXP-2541` | Error    | Logical operator on non-bool.             |
| `E-EXP-2542` | Error    | Invalid types for arithmetic/bitwise.     |
| `E-EXP-2545` | Error    | Address-of on non-place.                  |
| `E-EXP-2561` | Error    | `if` without `else` in non-unit context.  |
| `E-EXP-2571` | Error    | Match arms incompatible types.            |
| `E-EXP-2582` | Error    | Break statements mismatched types.        |
| `E-PAT-2711` | Error    | Refutable pattern in irrefutable context. |
| `E-PAT-2741` | Error    | Match not exhaustive.                     |
| `E-PAT-2751` | Error    | Match arm unreachable.                    |
| `E-DEC-2401` | Error    | Reassignment of immutable binding.        |
| `E-STM-2631` | Error    | Assignment target not a place.            |
| `E-STM-2651` | Error    | Defer block non-unit.                     |
| `E-STM-2652` | Error    | Non-local control in defer.               |
| `E-STM-2661` | Error    | Return type mismatch.                     |
| `E-STM-2662` | Error    | Break outside loop.                       |
| `E-STM-2663` | Error    | Continue outside loop.                    |
| `E-STM-2664` | Error    | Result type mismatch.                     |
| `E-NAM-1305` | Error    | Ambiguous method resolution.              |

**Diagnostics Referenced from Clause 3 (Memory Model):**

| Code         | Source | Description                        |
| :----------- | :----- | :--------------------------------- |
| `E-MEM-3001` | §3.5   | Use of moved value.                |
| `E-MEM-3012` | §3.8   | Partition verification failed.     |
| `E-MEM-3021` | §3.9   | Region allocation outside region.  |
| `E-MEM-3030` | §3.10  | Pointer arithmetic outside unsafe. |

---

### Clause 8 Cross-Reference Notes

**Terms defined in Clause 8 that MUST NOT be redefined elsewhere:**

| Term                | Section | Description                                   |
| :------------------ | :------ | :-------------------------------------------- |
| Expression          | §8.1    | Syntactic form producing typed value.         |
| Statement           | §8.1    | Syntactic form executed for side effects.     |
| Typing Context      | §8.1    | Mapping $\Gamma$ from identifiers to types.   |
| Value Category      | §8.1    | Classification: Place vs Value expression.    |
| Place Expression    | §8.1    | Expression denoting memory location.          |
| Value Expression    | §8.1    | Expression producing temporary value.         |
| Pattern             | §8.2    | Syntactic form for destructuring/testing.     |
| Irrefutable Pattern | §8.2    | Pattern that always matches.                  |
| Refutable Pattern   | §8.2    | Pattern that may fail to match.               |
| Exhaustiveness      | §8.2    | Property: patterns cover all values.          |
| Unreachability      | §8.2    | Property: arm can never execute.              |
| Precedence          | §8.3    | Operator binding strength ranking.            |
| Associativity       | §8.3    | Grouping rule for equal-precedence operators. |
| Short-Circuit Eval  | §8.4    | Second operand conditionally skipped.         |

**Terms referenced from other clauses:**

| Term                      | Source | Usage in Clause 8                                   |
| :------------------------ | :----- | :-------------------------------------------------- |
| Type                      | §4.1   | Expression typing, pattern typing.                  |
| Permission                | §4.5   | Assignment constraints.                             |
| Binding, Responsible      | §3.4   | Declaration statements, binding state.              |
| Move (semantics)          | §3.5   | Move expression references §3.5 for full semantics. |
| Deterministic Destruction | §3.6   | Assignment drop behavior.                           |
| Partitioning System       | §3.7   | Contract statement references §3.7.                 |
| Partition Proof Verifier  | §3.8   | Contract statement verification.                    |
| Region                    | §3.9   | Region allocation operator, region blocks.          |
| Unsafe Operations         | §3.10  | Unsafe block behavior.                              |
| Ptr<T>@State              | §4.13  | Address-of result type (Pointer Types).             |
| Statement Termination     | §2.11  | Statement boundary rules.                           |
| Drop                      | §6.5   | Assignment drop semantics.                          |
| Iterator Protocol         | §6.x   | Iterator loop desugaring.                           |

**Terms deferred to later clauses:**

| Term             | Deferred To | Reason                     |
| :--------------- | :---------- | :------------------------- |
| `parallel` block | §9.1        | Concurrency semantics.     |
| `comptime` block | §11.1       | Metaprogramming semantics. |
| `unsafe` details | §3.10       | Safety check specifics.    |

---

## Clause 9: Concurrency

This clause defines the Cursive concurrency model, specifying two mutually exclusive paths for concurrent execution and the static rules that guarantee data-race freedom.

### 9.1 Concurrency Model Foundations {Source: Draft 2 §31.1}

##### Definition

**The Two-Path Concurrency Model**

Cursive provides exactly two models for concurrent execution:

1. **Path 1 (Deterministic Data Parallelism)**: The `parallel` block enforces Concurrent-Read, Exclusive-Write (CREW) semantics at compile time.

2. **Path 2 (Stateful Coordination)**: The `System` capability provides thread spawning and synchronization primitives (`Mutex`, `Thread`).

**Formal Definition**

Let $\text{ConcurrencyPath}$ be the set $\{\text{CREW}, \text{Coordination}\}$.

A concurrent operation $op$ **MUST** be associated with exactly one path:
$$op \in \text{ConcurrencyPath}$$

**Thread Safety Classification**

Thread safety is derived from the **Permission System** (§4.5), not marker traits:

| Permission    | Share Safety | Transfer Safety |
| :------------ | :----------- | :-------------- |
| `const`       | Safe         | Safe            |
| `unique`      | Unsafe       | Safe            |
| `partitioned` | Unsafe       | Unsafe          |
| `concurrent`  | Safe         | Safe            |

##### Static Semantics

**Typing Rules**

**Rule Safe-Const:** A `const` binding is always safe to share across threads.
$$
\frac{}{\Gamma \vdash \text{IsSafe}(x : \text{const } T, \text{Share})}
$$

**Rule Safe-Unique-Transfer:** A `unique` binding may be transferred (moved) to another thread.
$$
\frac{}{\Gamma \vdash \text{IsSafe}(x : \text{unique } T, \text{Transfer})}
$$

**Rule Unsafe-Partitioned:** A `partitioned` binding is never safe for concurrent access.
$$
\frac{}{\Gamma \nvdash \text{IsSafe}(x : \text{partitioned } T, \_)}
$$

##### Constraints & Legality

| Code         | Severity | Condition                                               |
| :----------- | :------- | :------------------------------------------------------ |
| `E-CON-3201` | Error    | Capture of `partitioned` binding in concurrent closure. |

---

### 9.2 Path 1: Data Parallelism (CREW) {Source: Draft 2 §31.2}

##### Definition

**Parallel Epoch**

A **Parallel Epoch** is the lexical scope of a `parallel` block during which the compiler enforces Concurrent-Read, Exclusive-Write (CREW) semantics.

**CREW Invariant**

Within a parallel epoch:
- Multiple threads **MAY** read shared data concurrently.
- No thread **MAY** mutate shared data.

##### Syntax & Declaration

**Grammar**

```ebnf
parallel_statement
    ::= "parallel" "(" binding_list ")" block

binding_list
    ::= identifier ("," identifier)*

fork_expression
    ::= "fork" closure_expression
```

**Desugaring**

The `parallel` block desugars to:
1. Static invalidation of captured bindings in outer scope
2. Introduction of `const`-permission bindings in inner scope
3. Implicit synchronization barrier at block exit

##### Static Semantics

**Typing Rules**

**Rule WF-Parallel-Epoch:** Permission tightening within parallel scope.

Let $\Gamma_{outer}$ be the environment before the block. Let $x$ be a binding with outer permission $P_{outer}$ and type $T$.

$$
\frac{
    \Gamma_{inner} = \Gamma_{outer} [ x \mapsto \text{const } T ] \quad
    \Gamma_{outer}' = \Gamma_{outer} [ x \mapsto \text{invalidated} ]
}{
    \Gamma_{outer} \vdash \text{parallel}(x) \{ s \} \Rightarrow \Gamma_{outer}
}
$$

**Static Invalidation**

Upon entry to a `parallel` block, bindings in the `binding_list` **MUST** be statically invalidated in the enclosing scope for the duration of the block.

**Permission Tightening**

Inside the block, captured bindings **MUST** have `const` permission, regardless of their outer permission (`unique` or `partitioned`).

**Restoration**

Upon exit from the `parallel` block, outer bindings **MUST** be re-validated with their original permissions.

##### Dynamic Semantics

**Evaluation**

1. **Entry**: The runtime suspends the current execution context.
2. **Execution**: The block body executes with CREW guarantees enforced.
3. **Barrier**: The end of the block acts as a strict synchronization barrier.
4. **Exit**: Control returns to the parent context only after all forked tasks complete.

**Panic Propagation**

If any task within the epoch panics, the parent thread **MUST** panic upon reaching the synchronization barrier.

##### Examples

**Valid:** CREW parallel processing
```cursive
let buffer: unique [i32; 1000] = /* ... */

parallel(buffer) {
    // 'buffer' is 'const [i32; 1000]' inside
    let h1 = fork { process_part(buffer, 0, 500) }
    let h2 = fork { process_part(buffer, 500, 1000) }
    h1.join()
    h2.join()
}
// 'buffer' is 'unique' again
```

---

### 9.3 The `fork` Expression and `JobHandle<T>` {Source: Draft 2 §31.2.3–31.2.5}

##### Definition

**Fork Expression**

The `fork` keyword spawns a logical task within a parallel epoch.

**JobHandle<T>**

A `JobHandle<T>` is a non-modal, linear type representing a pending computation that yields a value of type `T`. Its layout is implementation-defined.

##### Syntax & Declaration

**Grammar**

```ebnf
fork_expression
    ::= "fork" closure_expression
```

##### Static Semantics

**Typing Rules**

**Rule Fork-Type:** The `fork` expression returns `JobHandle<T>`.

$$
\frac{
    \Gamma \vdash e : () \to T \quad
    \text{InParallelScope}(\Gamma)
}{
    \Gamma \vdash \text{fork } e : \text{JobHandle}<T>
}
$$

**Rule Fork-Capture:** Closures capture only `const` bindings from the epoch scope.

**JobHandle Constraints**

- `JobHandle<T>` **MUST NOT** implement `Copy` or `Clone`.
- `JobHandle<T>` **MUST NOT** provide a `detach()` method.
- `JobHandle<T>` **MUST** provide exactly one consuming method: `join(self) -> T`.

**Static Join Requirement (Linear Usage Analysis)**

Every `JobHandle` produced by `fork` **MUST** be consumed by `.join()` within the same `parallel` block:

1. **Usage Count**: Compiler tracks usage of every `JobHandle`.
2. **Consumption**: Calling `join()` consumes the handle (count → 0).
3. **Invariant**: At block exit, usage count of all handles **MUST** be 0.
4. **Branching**: In conditional branches, handle **MUST** be joined in ALL paths or NO paths.

##### Constraints & Legality

| Code         | Severity | Condition                                         |
| :----------- | :------- | :------------------------------------------------ |
| `E-CON-3202` | Error    | `JobHandle` not joined within `parallel` block.   |
| `E-EXP-2592` | Error    | `parallel` result depends on invalidated binding. |

##### Examples

**Invalid:** Unjoined JobHandle
```cursive
parallel(data) {
    let handle = fork { compute(data) }
    // ERROR E-CON-3202: handle not joined
}
```

---

### 9.4 Path 2: Stateful Coordination {Source: Draft 2 §31.3}

##### Definition

**Stateful Coordination Model**

Path 2 manages mutable state across threads using explicit ownership transfer, locking primitives, and the System capability.

**Thread<T> Modal Type**

`Thread<T>` is a modal type (§4.13) representing a system thread:

| State       | Description                                 |
| :---------- | :------------------------------------------ |
| `@Spawned`  | Thread is executing                         |
| `@Joined`   | Thread completed; result `T` available      |
| `@Detached` | Thread continues independently; result lost |

**Mutex<T> Modal Type**

`Mutex<T>` is a modal type providing synchronized mutable access:

| State       | Description                                       |
| :---------- | :------------------------------------------------ |
| `@Unlocked` | Mutex is free; data `T` inaccessible              |
| `@Locked`   | Mutex held by current thread; data `T` accessible |

`Mutex<T>` supports the `concurrent` permission because it provides methods accepting `concurrent self` (e.g., `lock`). See §4.5 for `concurrent` permission semantics.

The modal type system tracks the lock state. When `Mutex<T>@Locked` goes out of scope, `Drop` transitions it back to `@Unlocked`. No separate guard type needed.

##### Syntax & Declaration

**Thread Transitions**

```cursive
transition join(self: unique Thread<T>@Spawned) -> Thread<T>@Joined
transition detach(self: unique Thread<T>@Spawned) -> Thread<T>@Detached
```

**Mutex State-Specific Operations**

```cursive
modal Mutex<T> {
    @Unlocked {
        // Blocks until lock acquired, transitions to @Locked
        transition lock(self: concurrent Mutex<T>@Unlocked) -> Mutex<T>@Locked
    }

    @Locked {
        data: T  // Protected data accessible only in @Locked state

        // Access the protected data
        procedure get(self: unique) -> unique T
        procedure set(self: unique, value: T)

        // Explicit unlock (also happens via Drop)
        transition unlock(self: unique Mutex<T>@Locked) -> Mutex<T>@Unlocked
    }
}
```

**System.spawn**

```cursive
procedure spawn<T>(self: const System, closure: () -> T): Thread<T>@Spawned
```

##### Static Semantics

**Mutex Lock Transition**

The `lock` transition **MUST** accept `concurrent` permission (§4.5) because it performs synchronized internal state mutation:

$$
\frac{
    \Gamma \vdash m : \text{concurrent Mutex}<T>\text{@Unlocked}
}{
    \Gamma \vdash m.\text{lock}() : \text{Mutex}<T>\text{@Locked}
}
$$

**State-Specific Access**

Data field `T` is accessible **only** in `@Locked` state. Attempting to access data on `Mutex<T>@Unlocked` is a compile-time error (`E-TYP-1912`).

**Spawn Capture Constraints**

1. Closure **MUST NOT** capture `partitioned` bindings.
2. `unique` bindings **MUST** be moved into the closure.
3. `const` bindings **MUST** be `Copy` or thread-safe references.

##### Dynamic Semantics

**Lock/Unlock via Modal Transitions**

1. `lock()` blocks until mutex is acquired, returns `Mutex<T>@Locked`.
2. In `@Locked` state, `data` field provides exclusive mutable access.
3. `unlock()` or `Drop::drop` transitions back to `@Unlocked`.

**Memory Ordering**

`lock()` implies acquire barrier; `unlock()` implies release barrier.

**Deadlock Semantics**

Deadlocks are permitted (safe but undesirable). Calling `lock()` on an already-locked mutex blocks the thread indefinitely.

##### Constraints & Legality

| Code         | Severity | Condition                                      |
| :----------- | :------- | :--------------------------------------------- |
| `E-CON-3203` | Error    | Invalid capture in `System.spawn`.             |
| `E-TYP-1912` | Error    | Accessing `data` field on `Mutex<T>@Unlocked`. |

##### Examples

**Valid:** Thread spawning with mutex
```cursive
let counter: Mutex<i32>@Unlocked = Mutex::new(0)
let handle = ctx.sys~>spawn(move || {
    let locked = counter~>lock()   // Returns Mutex<i32>@Locked
    locked.data = locked.data + 1  // Access data field in @Locked state
})                                 // Drop transitions to @Unlocked
handle~>join()
```

---

### 9.5 Diagnostics Summary {Source: Draft 2 §31.4}

| Code         | Severity | Description                                             |
| :----------- | :------- | :------------------------------------------------------ |
| `E-CON-3201` | Error    | Capture of `partitioned` binding in concurrent closure. |
| `E-CON-3202` | Error    | `JobHandle` not joined within `parallel` block.         |
| `E-CON-3203` | Error    | Invalid capture in `System.spawn`.                      |
| `E-EXP-2592` | Error    | `parallel` result depends on invalidated binding.       |

---

### Clause 9: Concurrency Cross-Reference Notes

**Terms defined in Clause 9 that MUST NOT be redefined elsewhere:**

| Term                    | Section | Description                                            |
| :---------------------- | :------ | :----------------------------------------------------- |
| Parallel Epoch          | §9.2    | Lexical scope of `parallel` block with CREW semantics. |
| CREW                    | §9.2    | Concurrent-Read, Exclusive-Write invariant.            |
| Permission Tightening   | §9.2    | Downgrading of permissions within parallel scope.      |
| Static Invalidation     | §9.2    | Compile-time inaccessibility of captured bindings.     |
| JobHandle<T>            | §9.3    | Linear type for pending parallel computation.          |
| Static Join Requirement | §9.3    | Linearity constraint requiring all handles be joined.  |

**Terms referenced:**

| Term         | Source | Usage in Clause 9                                         |
| :----------- | :----- | :-------------------------------------------------------- |
| Permission   | §4.5   | Thread safety classification, capture constraints.        |
| const        | §4.5   | Shared-safe permission in concurrent contexts.            |
| unique       | §4.5   | Transfer-safe permission; move semantics in spawn.        |
| partitioned  | §4.5   | Thread-unsafe permission; prohibited in captures.         |
| concurrent   | §4.5   | Synchronized aliased mutability; required for Mutex.lock. |
| Modal Type   | §4.11  | Thread<T>@State, Mutex<T>@State declarations.             |
| Drop         | §6.7   | Mutex unlock semantics (special compiler permission).     |
| `concurrent` | §4.5   | Permission required for synchronized shared access.       |
| System       | §10.2  | Capability required for thread spawning.                  |
| Closure      | §7.x   | Capture semantics in fork and spawn.                      |

**Terms deferred to later clauses:**

| Term       | Deferred To | Reason                                         |
| :--------- | :---------- | :--------------------------------------------- |
| Channel<T> | Appendix D  | Standard library primitive; not core language. |

**Terms explicitly NOT included (Rust cargo-cult removed):**

| Term          | Reason                                                             |
| :------------ | :----------------------------------------------------------------- |
| Atomic<T>     | No justification in Cursive's model - use Mutex or FFI if needed.  |
| MutexGuard<T> | Not needed due to modal type design with @Locked/@Unlocked states. |
  

---

## Clause 10: The Capability System

This clause defines the Cursive Capability System, which governs all procedures that produce observable **external effects** (e.g., I/O, networking, threading, heap allocation) and enforces the security principle of **No Ambient Authority**.

### 10.1 Foundational Principles {Source: Draft 2 §30.1}

##### Definition

**Capability**

A **Capability** is a first-class value representing unforgeable authority to perform a specific class of external effects. Capabilities are the sole mechanism by which a Cursive program may interact with the external world.

**Formal Definition**

$$\text{Capability} ::= (\text{Authority}, \text{Interface}, \text{Provenance})$$

Where:
- **Authority**: The set of permitted operations (e.g., read files, connect to network)
- **Interface**: The trait defining available methods
- **Provenance**: The derivation chain from the root capability

**No Ambient Authority Principle**

The capability system enforces four core invariants:

1. **No Global State**: A conforming implementation **MUST NOT** provide mutable global variables or global procedures that perform side effects (e.g., no `open()`, `print()`, or `malloc()` in the global namespace).

2. **Capabilities as Values**: Authority to perform a sensitive operation **MUST** be represented by a first-class value (a Capability).

3. **Parameter Injection**: To perform an effect, a procedure **MUST** require the corresponding capability as an explicit parameter.

4. **Unforgeability**: Capabilities cannot be constructed arbitrarily by user code. They **MUST** originate from the runtime root or be derived (attenuated) from an existing capability.

##### Static Semantics

**Authority Derivation Rule**

A procedure $p$ may perform effect $e$ iff $p$ receives a capability $c$ where $e \in \text{Authority}(c)$:

$$\frac{\Gamma \vdash c : \text{witness } T \quad e \in \text{Authority}(T)}{\Gamma \vdash p(c) \text{ may perform } e}$$

##### Constraints & Legality

| Code         | Severity | Condition                                                           |
| :----------- | :------- | :------------------------------------------------------------------ |
| `E-CAP-1001` | Error    | Ambient authority detected: global procedure performs side effects. |

---

### 10.2 The Root of Authority {Source: Draft 2 §30.2}

##### Definition

**The Capability Root**

All system-level capabilities originate from the Cursive runtime and are injected into the program via the `Context` parameter at the entry point.

**Cross-Reference**: The entry point signature `public procedure main(ctx: Context): i32` and the `Context` record structure are normatively defined in §5.6 (Program Entry Point).

**Runtime Injection**

The Cursive runtime constructs the `Context` record before program execution begins. This record contains concrete implementations of all system capability traits, initialized with full authority over system resources.

##### Static Semantics

**Runtime Injection Guarantee**

The runtime **MUST** guarantee that `ctx` is a valid, initialized `Context` record containing root capabilities before `main` begins execution.

**Root Capability Hierarchy**

The `Context` record forms the root of the capability tree:

```
Context (root)
├── fs: witness FileSystem      (filesystem authority)
├── net: witness Network        (network authority)
├── sys: System                 (system primitives)
│   ├── get_env()              (environment access)
│   ├── time()                 (clock access)
│   ├── exit()                 (process termination)
│   ├── spawn()                (thread creation)
│   └── create_mutex()         (synchronization)
└── heap: witness HeapAllocator (dynamic allocation)
```

##### Constraints & Legality

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-DEC-2431` | Error    | `main` signature incorrect (see §5.6 for spec). |

---

### 10.3 Capability Attenuation {Source: Draft 2 §30.4}

##### Definition

**Attenuation**

**Attenuation** is the process of creating a new capability with strictly **less** authority than the source capability. Attenuation is the primary mechanism for implementing the principle of least privilege.

**Formal Definition**

$$\text{Attenuate}(C_{parent}, R) \to C_{child}$$

Where:
- $C_{parent}$: Source capability
- $R$: Restriction specification (e.g., path, quota, host filter)
- $C_{child}$: Derived capability where $\text{Authority}(C_{child}) \subseteq \text{Authority}(C_{parent})$

**Attenuation Invariant**

A child capability **MUST NOT** grant more authority than its parent:

$$\forall c_{child} \in \text{Attenuate}(c_{parent}, \_) : \text{Authority}(c_{child}) \subseteq \text{Authority}(c_{parent})$$

##### Static Semantics

**Attenuation Method Requirements**

Attenuation methods (e.g., `restrict`, `with_quota`) **MUST** return a capability that:

1. Implements the same capability trait as the parent
2. Enforces the specified restrictions on all operations
3. Delegates authorized operations to the parent capability

**Type Preservation Rule**

Attenuation preserves the capability trait:

$$\frac{\Gamma \vdash c : \text{witness } T \quad \Gamma \vdash c.restrict(r) : \text{witness } T'}{\Gamma \vdash T' \equiv T}$$

##### Examples

```cursive
// Attenuate filesystem capability to single directory
let restricted_fs: witness FileSystem = ctx.fs~>restrict("/app/data")
// restricted_fs can only access files under /app/data

// Attenuate heap allocator with quota
let limited_heap: witness HeapAllocator = ctx.heap~>with_quota(1024 * 1024)
// limited_heap fails allocation if cumulative usage exceeds 1 MiB

// Attenuate network capability to specific host
let api_only: witness Network = ctx.net~>restrict_to_host("api.example.com")
// api_only can only connect to api.example.com
```

---

### 10.4 Capability Propagation {Source: Draft 2 §30.5}

##### Definition

**Capability Propagation**

Capabilities travel through the call graph as explicit parameters. A procedure requiring a capability **MUST** accept it as a parameter; capabilities cannot be obtained through any other means in safe code.

**Granular Capability Pattern**

Low-level or reusable procedures **SHOULD** accept only the specific capability traits they require. This enables:
- Maximum code reuse (works with any implementation of the trait)
- Clear documentation of required authority
- Easy testing via mock implementations

**Capability Bundle Pattern**

High-level "manager" procedures **MAY** accept record types ("bundles") that aggregate multiple capabilities. This reduces parameter count for procedures requiring many capabilities.

##### Syntax & Declaration

**Capability Parameter Syntax**

Capability parameters **MUST** be declared using `witness` types to enable polymorphism:

```cursive
// Granular: accepts any FileSystem implementation
procedure read_config(fs: witness FileSystem, path: string@View): string {
    let file = fs~>open(path, Mode::Read)
    file~>read_all()
}
```

**Bundle Declaration**

```cursive
// Bundle: aggregates multiple capabilities
record AppContext {
    fs: witness FileSystem,
    net: witness Network,
    log: witness Logger
}

procedure run_server(ctx: AppContext) {
    // Can use ctx.fs, ctx.net, ctx.log
}
```

##### Static Semantics

**Witness Parameter Rule**

A parameter of type `witness Trait` accepts any concrete type `T` where `T` implements `Trait`.

**Permission Requirements for Capability Methods**

Capability methods **MUST** declare appropriate receiver permissions:

| Operation Type                 | Required Permission  | Rationale                         |
| :----------------------------- | :------------------- | :-------------------------------- |
| Side-effecting I/O             | `concurrent` (`~\|`) | Allows synchronized shared access |
| Pure queries (no state change) | `const` (`~`)        | No state change; safe for sharing |

**Cross-Reference**: `concurrent` permission semantics defined in §4.5.

##### Constraints & Legality

**Implicit Capability Prohibition**

A procedure **MUST NOT** access capabilities not explicitly provided as parameters or derived from provided parameters.

| Code         | Severity | Condition                                                   |
| :----------- | :------- | :---------------------------------------------------------- |
| `E-CAP-1002` | Error    | Effect-producing procedure lacks required capability param. |

---

### 10.5 System Capability Traits {Source: Draft 2 §30.3}

##### Definition

System capabilities are defined by **Traits** (interfaces). This design enables:
- **Attenuation**: Restricted implementations can wrap full-authority implementations
- **Virtualization**: Mock implementations for testing
- **Abstraction**: Code depends on interface, not implementation

**Cross-Reference**: The normative definitions of these traits are provided in **Appendix D.2** (System Capability Traits).

**FileSystem Trait**

Governs access to the host filesystem.

| Requirement        | Description                                     |
| :----------------- | :---------------------------------------------- |
| Operations         | File read, write, open (returning `FileHandle`) |
| Attenuation Method | `restrict(path)` - limits to directory subtree  |

**Network Trait**

Governs access to network sockets.

| Requirement        | Description                                             |
| :----------------- | :------------------------------------------------------ |
| Operations         | `connect` to remote hosts, bind listeners               |
| Attenuation Method | `restrict_to_host(addr)` - limits to specific hosts/IPs |

**HeapAllocator Trait**

Governs dynamic memory allocation.

| Requirement        | Description                                   |
| :----------------- | :-------------------------------------------- |
| Operations         | `alloc` and `free` methods                    |
| Attenuation Method | `with_quota(bytes)` - fails if quota exceeded |

**System Record**

Aggregates miscellaneous system-level primitives. Unlike the traits above, `System` is a concrete record (not a trait) because it bundles unrelated operations.

| Method            | Signature                            | Description                 |
| :---------------- | :----------------------------------- | :-------------------------- |
| `get_env`         | `(~, key: string@View) -> string`    | Environment variable access |
| `time`            | `(~) -> Timestamp`                   | Current timestamp           |
| `exit`            | `(~, code: i32) -> !`                | Process termination         |
| `spawn<T>`        | `(~, action: () -> T) -> Thread<T>`  | Thread creation             |
| `create_mutex<T>` | `(~, value: T) -> Mutex<T>@Unlocked` | Mutex creation              |

##### Static Semantics

**Method Receiver Permissions**

System capability methods **MUST** declare receiver permissions that accurately reflect their side effects:

$$\frac{m : T.\text{method} \quad \text{HasSideEffect}(m)}{\Gamma \vdash m.\text{receiver} = \texttt{concurrent}}$$

**Const Purity Rule**

Methods accepting `const` (`~`) receiver **MUST** guarantee **zero** logical state changes to the external resource:
- `time~>now()` - const (reading clock has no side effect)
- `file~>write(data)` - concurrent (modifies file state)

##### Constraints & Legality

| Code         | Severity | Condition                                             |
| :----------- | :------- | :---------------------------------------------------- |
| `E-CAP-1003` | Error    | Side-effecting capability method has `const` receiver |

---

### 10.6 User-Defined Capabilities {Source: Draft 2 §30.6}

##### Definition

**User-Defined Capability**

Users **MAY** define application-level capability types representing domain-specific authority (e.g., `DatabaseAccess`, `BillingService`, `AuditLog`).

A user-defined capability is typically implemented as a `record` that:

1. Implements a domain-specific trait defining the capability interface
2. Holds system capabilities internally as `private` or `protected` fields
3. Provides methods that use internal capabilities to perform authorized operations

**Encapsulation Principle**

By wrapping system capabilities in `private` fields, the user-defined capability restricts access to the raw system resource, allowing only the application-specific operations defined by its methods.

##### Syntax & Declaration

**Domain Trait Definition**

```cursive
// Define the capability interface
trait DatabaseAccess {
    procedure query(~|, sql: string@View): QueryResult
    procedure execute(~|, sql: string@View): u64
}
```

**Capability Implementation**

```cursive
// Implement the capability wrapping system resources
record PostgresConnection <: DatabaseAccess {
    private net: witness Network,
    private connection_string: string@View,

    procedure query(~|, sql: string@View): QueryResult {
        // Uses self.net internally to communicate with database
        // ...
    }

    procedure execute(~|, sql: string@View): u64 {
        // Uses self.net internally
        // ...
    }
}
```

##### Static Semantics

**Encapsulation Rule**

Private system capability fields **MUST NOT** be directly accessible outside the record's implementation. This ensures the user-defined capability controls all access to the underlying system resource.

**Attenuation for User-Defined Capabilities**

User-defined capabilities **SHOULD** provide attenuation methods appropriate to their domain:

```cursive
record PostgresConnection <: DatabaseAccess {
    // ... fields ...

    // Attenuation: create read-only capability
    procedure restrict_to_read_only(~): witness DatabaseAccess {
        ReadOnlyPostgres { inner: self }
    }
}

record ReadOnlyPostgres <: DatabaseAccess {
    private inner: witness DatabaseAccess,

    procedure query(~|, sql: string@View): QueryResult {
        self.inner~>query(sql)
    }

    procedure execute(~|, sql: string@View): u64 {
        panic("Write operations not permitted on read-only connection")
    }
}
```

##### Examples

**Creating a User-Defined Capability**

```cursive
procedure create_db_connection(net: witness Network): PostgresConnection {
    PostgresConnection {
        net: net,
        connection_string: "postgres://localhost/mydb"
    }
}
```

**Using the Capability Polymorphically**

```cursive
// Accepts any DatabaseAccess implementation
procedure run_reports(db: witness DatabaseAccess) {
    let results = db~>query("SELECT * FROM reports")
    // Process results...
}

// Can be called with full or attenuated capability
public procedure main(ctx: Context): i32 {
    let db = create_db_connection(ctx.net)
    let read_only_db = db~>restrict_to_read_only()

    run_reports(db)           // Full access
    run_reports(read_only_db) // Read-only access

    result 0
}
```

---

### Clause 10: The Capability System Cross-Reference Notes

**Terms defined in Clause 10 that MUST NOT be redefined elsewhere:**

| Term                        | Section | Description                                               |
| :-------------------------- | :------ | :-------------------------------------------------------- |
| Capability                  | §10.1   | First-class unforgeable authority value                   |
| No Ambient Authority        | §10.1   | Core principle prohibiting global side effects            |
| Authority                   | §10.1   | The set of permitted operations for a capability          |
| Attenuation                 | §10.3   | Process of deriving restricted capability from parent     |
| Attenuation Invariant       | §10.3   | Child capability cannot exceed parent authority           |
| Capability Propagation      | §10.4   | Explicit parameter passing of capabilities                |
| Granular Capability Pattern | §10.4   | Accept only the specific capability traits required       |
| Capability Bundle Pattern   | §10.4   | Aggregate multiple capabilities in a record               |
| User-Defined Capability     | §10.6   | Application-level capability wrapping system capabilities |

**Terms referenced:**

| Term                     | Source       | Usage in Clause 10                             |
| :----------------------- | :----------- | :--------------------------------------------- |
| `main` procedure         | §5.6         | Entry point for capability injection           |
| `Context` record         | §5.6         | Root capability container                      |
| Permission types         | §4.5         | `concurrent`, `const` for method receivers     |
| `concurrent` permission  | §4.5         | Required for side-effecting capability methods |
| `witness` types          | §6.3         | Dynamic polymorphism for capability traits     |
| Trait                    | §6.1         | Interface defining capability methods          |
| System Capability Traits | Appendix D.2 | Normative trait definitions                    |

**Terms deferred to later clauses:**

| Term         | Deferred To  | Reason                        |
| :----------- | :----------- | :---------------------------- |
| `Mutex<T>`   | Clause 9     | Synchronization primitive     |
| `Thread<T>`  | Clause 9     | Thread handle type            |
| `FileHandle` | Appendix D.2 | Defined with FileSystem trait |
| `Timestamp`  | Appendix D.2 | Defined with System record    |

---

## Clause 11: Metaprogramming

This clause defines the Cursive metaprogramming system. Cursive provides a deterministic, declarative code generation mechanism based on **compile-time execution**, **type introspection**, **quasiquoting**, and **explicit AST emission**. This system adheres to the **Two-Phase Compilation Model** defined in §2.12. Metaprogramming occurs strictly between the **Parsing** and **Semantic Analysis** phases.

### 11.1 The Metaprogramming Model {Source: Draft 2 §33.1.1}

##### Definition

**The Comptime Environment**

The `comptime` environment $\Gamma_{ct}$ is a sandboxed execution context, distinct from the runtime environment $\Gamma_{rt}$. Code marked with the `comptime` keyword **MUST** execute during the Metaprogramming Phase (Translation Phase 2, as defined in §2.12).

**Formal Definition:**

$$\Gamma_{ct} ::= (\Sigma_{stdlib}, \Sigma_{imports}, \emptyset)$$

Where:
- $\Sigma_{stdlib}$ is the standard library available at compile time
- $\Sigma_{imports}$ contains modules imported via `import`
- The third component (empty set) indicates no shared mutable state with $\Gamma_{rt}$

##### Static Semantics

**Isolation Property:**

Comptime code **MUST NOT** access:
- Runtime memory or heap allocations
- File handles, sockets, or other I/O resources
- Foreign function interfaces (FFI)
- Capabilities other than `ComptimeCodegen`

**Determinism Property:**

Comptime code **MUST** be deterministic. Given the same source input and compiler configuration, comptime execution **MUST** produce the same output.

**Termination Property:**

Implementations **MUST** enforce resource limits (§11.7) to ensure compilation terminates.

##### Constraints & Legality

**Prohibitions:**

The following are **STRICTLY FORBIDDEN** in `comptime` context:
- `unsafe` blocks
- FFI calls (`extern` procedures)
- Access to runtime capabilities

**Caching:**

Implementations **SHOULD** cache the result of `comptime` blocks and procedures annotated with the `[[cache]]` attribute.

- **Cache Key**: **MUST** include the AST of the block, the values of all captured identifiers, and the compiler version.
- **Determinism**: Blocks marked `[[cache]]` **MUST** be purely deterministic. Reliance on non-deterministic compiler state (e.g., memory addresses of AST nodes) in cached blocks is **Unspecified Behavior (USB)**.

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-MET-3402` | Error    | Compile-time execution resource limit exceeded. |

---

### 11.2 AST Representation {Source: Draft 2 §33.3, §33.5}

##### Definition

**The AST Model**

The metaprogramming system operates on Abstract Syntax Tree (AST) node types. The following types **MUST** be available in the `comptime` context:

| Type   | Description                                                             |
| :----- | :---------------------------------------------------------------------- |
| `Node` | Base trait for all syntax elements.                                     |
| `Expr` | Tagged union of expression nodes (Binary, Call, Literal, If, etc.).     |
| `Stmt` | Tagged union of statement nodes (Let, Return, ExprStmt, Loop, etc.).    |
| `Decl` | Tagged union of declaration nodes (Procedure, Record, Enum, etc.).      |
| `Type` | Representation of resolved types (used for type-level metaprogramming). |

**Quoted Code Types**

| Type          | Description                                          |
| :------------ | :--------------------------------------------------- |
| `QuotedBlock` | Opaque type representing a block of quoted code.     |
| `QuotedExpr`  | Opaque type representing a single quoted expression. |

##### Static Semantics

**Deferred Type Checking:**

`QuotedBlock` and `QuotedExpr` are **NOT** type-checked at the quote site. Type checking is deferred until emission into module scope (§11.6).

##### Constraints & Legality

| Code         | Severity | Condition                          |
| :----------- | :------- | :--------------------------------- |
| `E-MET-3404` | Error    | Quote block contains syntax error. |

---

### 11.3 Type Introspection {Source: Draft 2 §33.2}

##### Definition

**TypeInfo Structure**

Implementations **MUST** provide a `TypeInfo` record accessible in `comptime` context:

```cursive
record TypeInfo {
    kind: TypeKind,           // Type category (TypeKind defined in §4.1)
    name: string,             // Fully qualified name (e.g., "mymodule::MyRecord")
    fields: [FieldInfo],      // Non-empty for Records only; empty otherwise
    variants: [VariantInfo],  // Non-empty for Enums only; empty otherwise
    layout: LayoutInfo,       // Size, alignment, and stride
    generic_args: [TypeInfo], // Instantiated generic parameters (empty if non-generic)
    attributes: [Attribute],  // Attached attributes (Attribute defined in §4.17)
}
```

**FieldInfo Structure**

```cursive
record FieldInfo {
    name: string,             // Field identifier
    field_type: TypeInfo,     // Type of the field
    offset: usize,            // Byte offset from record start
    attributes: [Attribute],  // Attached attributes (§4.17)
    visibility: Visibility,   // Visibility level (§5.5)
}
```

**VariantInfo Structure**

```cursive
record VariantInfo {
    name: string,             // Variant identifier
    discriminant: i64,        // Discriminant value
    payload: [FieldInfo],     // Payload fields (empty for unit variants)
    attributes: [Attribute],  // Attached attributes (§4.17)
}
```

**LayoutInfo Structure**

```cursive
record LayoutInfo {
    size: usize,              // sizeof(T) in bytes
    alignment: usize,         // alignof(T) in bytes
    stride: usize,            // Array element stride (size rounded up to alignment)
}
```

##### Syntax & Declaration

**The `reflect_type<T>()` Intrinsic**

```cursive
comptime procedure reflect_type<T>(): TypeInfo
```

##### Static Semantics

- `reflect_type<T>` **MUST** only be invoked on types `T` that are fully defined at the point of invocation.
- Recursive type dependencies that prevent layout resolution **MUST** be diagnosed.

##### Constraints & Legality

| Code         | Severity | Condition                                 |
| :----------- | :------- | :---------------------------------------- |
| `E-MET-3407` | Error    | `reflect_type` called on incomplete type. |

---

### 11.4 Compile-Time Procedures and Blocks {Source: Draft 2 §33.1.2, §33.1.3}

##### Definition

**Comptime Procedure**

A procedure declared with the `comptime` modifier is a **compile-time procedure**. It executes during the Metaprogramming Phase.

**Comptime Block**

A `comptime` block is a block statement prefixed with the `comptime` keyword. It executes immediately when encountered during the Metaprogramming Phase.

##### Syntax & Declaration

**Grammar:**

```ebnf
// NEW productions for Clause 11 (metaprogramming-specific)
comptime_procedure ::= <visibility>? "comptime" "procedure" IDENTIFIER
                       <generic_params>? "(" <param_list>? ")"
                       (":" <type>)? <contract_clause>? <block>

comptime_block     ::= "comptime" <block>

comptime_expr      ::= "comptime" <block>  // When used in expression context
```

**Referenced Productions (defined elsewhere):**
- `<visibility>` → §5.5 Visibility and Access Control
- `<generic_params>` → §4.15 Static Polymorphism
- `<param_list>` → §6.4 Procedure Declarations
- `<contract_clause>` → §7.1 Contract Fundamentals
- `<block>` → §8.x Statements

##### Static Semantics

**Callability Rules:**

- A `comptime` procedure **MAY** be called by: other `comptime` code, `comptime` blocks, or `const` initializers.
- A `comptime` procedure **MUST NOT** be called from runtime context.

**Parameter Type Restrictions:**

Parameters to `comptime` procedures **MUST** be types representable at compile time:
- Primitive types (`iN`, `uN`, `fN`, `bool`, `char`)
- `string` (literals)
- `Type` (introspection handles)
- `Expr`, `Stmt`, `Decl` (AST nodes)
- Composites (tuples, records) of the above

**Prohibited Parameter Types:** Capabilities, Handles, Raw Pointers.

**Scope Rules:**

Variables declared inside a `comptime` block are local to the compile-time environment. They do **NOT** persist to the runtime program.

##### Dynamic Semantics

**Expression Use:**

A `comptime` block **MAY** return a value. If the value is a valid constant type, the block may be used as an expression in runtime code:

```cursive
let x = comptime { 1 + 1 }  // x = 2, computed at compile time
```

##### Constraints & Legality

| Code         | Severity | Condition                                         |
| :----------- | :------- | :------------------------------------------------ |
| `E-MET-3401` | Error    | `comptime` procedure called from runtime context. |

##### Examples

**Valid:**

```cursive
comptime procedure generate_table(size: i32): [i32] {
    let table: [i32] = []
    let i = 0
    loop {
        if i >= size { break }
        table.push(i * i)
        i = i + 1
    }
    result table
}

let SQUARES = comptime { generate_table(16) }
```

---

### 11.5 Quasiquoting and Interpolation {Source: Draft 2 §33.3, §33.4}

##### Definition

**Quote Expression**

A quote expression captures Cursive syntax as AST data rather than executing it:
- `quote { ... }` → `QuotedBlock`
- `quote <expr>` → `QuotedExpr`

**Splice Operator**

The `$(...)` syntax interpolates compile-time values into quote blocks.

##### Syntax & Declaration

**Grammar (NEW productions for Clause 11):**

```ebnf
// Quote expressions - metaprogramming-specific
quote_expr       ::= "quote" quote_body

quote_body       ::= quote_block
                   | <expression>

quote_block      ::= "{" quoted_item* "}"

quoted_item      ::= <statement>
                   | <declaration>
                   | splice_expr

// Splice productions - metaprogramming-specific
splice_expr      ::= "$(" <expression> ")"
```

**Referenced Productions (defined elsewhere):**
- `<expression>` → §8.3 Expressions
- `<statement>` → §8.7 Statements
- `<declaration>` → §6.x / §4.x (procedure, record, enum, etc.)
- `<pattern>` → §8.2 Pattern Matching
- `<type>` → §4.x Types
- `<identifier>` → §2.2 Identifiers

##### Static Semantics

**Quote Validation:**

- Content of a quote expression **MUST** be syntactically valid Cursive.
- Implementations **MUST** parse quoted code at definition time.
- Type checking is **DEFERRED** until emission (§11.6).

**Hygiene Algorithm:**

The implementation **MUST** apply hygienic macro semantics to prevent identifier capture:

1. **Free Identifiers (Capture):** Identifiers not defined within the quote are captured from the lexical scope at the `quote` definition site.
2. **Bound Identifiers (Fresh):** Identifiers introduced within the quote generate fresh, unique names to prevent collision with splice-site bindings.
3. **Hygiene Breaking:** To introduce identifiers into the target scope, use Identifier Splicing.

**Splice Context Table:**

| Splice Position | Required Input Type    | Result                          |
| :-------------- | :--------------------- | :------------------------------ |
| Expression      | Primitive literal      | AST literal node                |
| Expression      | `QuotedExpr`           | Structural AST insertion        |
| Identifier      | `string`               | Identifier (validated per §2.2) |
| Type            | `Type`                 | Type AST node                   |
| Pattern         | `QuotedExpr` (pattern) | Pattern AST insertion           |
| Block           | `QuotedBlock`          | Structural block insertion      |

**Identifier Validation:**

A string used for identifier splicing **MUST** conform to the identifier grammar (§2.2).

##### Constraints & Legality

| Code         | Severity | Condition                                   |
| :----------- | :------- | :------------------------------------------ |
| `E-MET-3403` | Error    | Invalid identifier string in interpolation. |
| `E-MET-3404` | Error    | Quote block contains syntax error.          |

##### Examples

**Value Splicing:**

```cursive
let limit = 100
let check = quote {
    if x > $(limit) { return; }  // Becomes: if x > 100 { return; }
}
```

**Identifier Splicing:**

```cursive
let method_name = "get_value"
let impl = quote {
    procedure $(method_name)(): i32 { result 42 }
    // Becomes: procedure get_value(): i32 { result 42 }
}
```

**AST Splicing:**

```cursive
let inner = quote { x = x + 1; }
let outer = quote {
    loop { $(inner) }  // Injects body of 'inner'
}
```

---

### 11.6 Code Emission {Source: Draft 2 §33.5}

##### Definition

**ComptimeCodegen Capability**

Code emission is a side effect on the compilation unit. It **MUST** require the `ComptimeCodegen` capability. This capability is provided by the compiler environment to `comptime` entry points.

**Emit Operation**

The `emit` method on the `ComptimeCodegen` capability injects a `QuotedBlock` into the current module's scope.

##### Syntax & Declaration

```cursive
// Method signature on ComptimeCodegen capability
procedure emit(~! self: ComptimeCodegen, code: QuotedBlock)
```

##### Static Semantics

**Injection Point:**

Code is emitted into the **module scope** where the `comptime` block resides. It **cannot** be injected into arbitrary scopes.

**Visibility:**

Emitted declarations are visible to all subsequent phases of compilation, exactly as if they had been written in the source file.

**Post-Emission Type Checking:**

After the Metaprogramming Phase completes, the implementation **MUST** perform full type checking on the expanded AST.

##### Dynamic Semantics

**Error Reporting:**

Type errors in emitted code **MUST** be reported with a diagnostic trace pointing to the `emit` call site. Implementations **SHOULD** provide the generated source code in the error message to aid debugging.

##### Constraints & Legality

| Code         | Severity | Condition                                                 |
| :----------- | :------- | :-------------------------------------------------------- |
| `E-MET-3405` | Error    | Emitted code failed type checking (includes trace).       |
| `E-MET-3406` | Error    | `emit` called without valid `ComptimeCodegen` capability. |

##### Examples

```cursive
comptime {
    let cg = compiler.get_codegen()
    let type_name = "DynamicRecord"

    let ast = quote {
        public record $(type_name) {
            val: i32,
        }

        public procedure make_dynamic(): $(type_name) {
            result $(type_name) { val: 0 }
        }
    }

    cg.emit(ast)  // Injects definitions into module
}

// Runtime code uses generated types
public procedure main(ctx: Context): i32 {
    let d = make_dynamic()
    result d.val
}
```

---

### 11.7 Implementation Limits {Source: Draft 2 §33.6}

##### Definition

Conforming implementations **MUST** enforce the following minimum limits for compile-time execution to ensure portability.

##### Constraints & Legality

**Minimum Resource Limits:**

| Resource              | Minimum Limit | Description                                            |
| :-------------------- | :------------ | :----------------------------------------------------- |
| **Recursion Depth**   | 256 frames    | Stack depth for `comptime` procedure calls.            |
| **Evaluation Steps**  | 1,000,000     | Basic operations (fuel) to prevent infinite loops.     |
| **Memory Allocation** | 64 MiB        | Total heap memory available to the `comptime` context. |
| **AST Depth**         | 1024 levels   | Maximum nesting depth of quoted ASTs.                  |

Exceeding these limits **MUST** result in error `E-MET-3402`.

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-MET-3402` | Error    | Compile-time execution resource limit exceeded. |

---

### Clause 11: Metaprogramming Cross-Reference Notes

**Terms defined in Clause 11 that MUST NOT be redefined elsewhere:**

| Term                 | Section | Description                                          |
| :------------------- | :------ | :--------------------------------------------------- |
| Comptime Environment | §11.1   | The sandboxed execution context $\Gamma_{ct}$        |
| AST Node Types       | §11.2   | Node, Expr, Stmt, Decl, Type - syntax tree elements  |
| QuotedBlock          | §11.2   | Opaque type for quoted code blocks                   |
| QuotedExpr           | §11.2   | Opaque type for quoted expressions                   |
| TypeInfo             | §11.3   | Structure describing type metadata for introspection |
| FieldInfo            | §11.3   | Structure describing record field metadata           |
| VariantInfo          | §11.3   | Structure describing enum variant metadata           |
| LayoutInfo           | §11.3   | Structure describing type size/alignment/stride      |
| reflect_type<T>      | §11.3   | Intrinsic returning TypeInfo for type T              |
| Comptime Procedure   | §11.4   | Procedure with `comptime` modifier                   |
| Comptime Block       | §11.4   | Block statement with `comptime` prefix               |
| Quote Expression     | §11.5   | Syntax capturing code as AST data                    |
| Splice Operator      | §11.5   | `$(expr)` syntax for interpolation                   |
| Hygiene Algorithm    | §11.5   | Name resolution rules for quoted code                |
| ComptimeCodegen      | §11.6   | Capability required for code emission                |
| emit                 | §11.6   | Method to inject QuotedBlock into module scope       |

**Terms referenced:**

| Term              | Source | Usage in Clause 11                        |
| :---------------- | :----- | :---------------------------------------- |
| Translation Phase | §2.12  | Metaprogramming phase ordering            |
| Identifier        | §2.2   | Validation for identifier splicing        |
| TypeKind          | §4.1   | Type category in TypeInfo.kind            |
| Attribute         | §4.17  | Attached attributes in TypeInfo/FieldInfo |
| Visibility        | §5.5   | Field visibility in FieldInfo             |
| Module Scope      | §5.x   | Target for code emission                  |
| Capability        | §10.x  | ComptimeCodegen as capability type        |
| Permission        | §4.5   | `unique` receiver on emit method          |

**Terms deferred to later clauses:**

| Term   | Deferred To | Reason                                          |
| :----- | :---------- | :---------------------------------------------- |
| (none) | -           | Clause 11 is self-contained for metaprogramming |

---

## Clause 12: Interoperability

This clause defines the Foreign Function Interface (FFI) mechanisms for interacting with code adhering to C-compatible Application Binary Interfaces (ABIs). It specifies extern declaration syntax, the safety boundary enforced by `unsafe` blocks, memory layout guarantees via representation attributes, and the strict categorization of types permitted across the language boundary.

### 12.1 Foundational Definitions {Source: Draft 2 §32}

##### Definition

**Foreign Function Interface (FFI)**

The **Foreign Function Interface** is the set of language mechanisms enabling Cursive code to:
1. Import and call procedures defined in external (non-Cursive) object files or libraries.
2. Export Cursive procedures for invocation by external code.

**Application Binary Interface (ABI)**

An **Application Binary Interface (ABI)** specifies the low-level conventions governing procedure calls between compiled code modules, including:
- Calling convention (register usage, stack layout, parameter passing)
- Name mangling rules
- Data type representation and alignment

**Calling Convention**

A **Calling Convention** is the subset of an ABI specifying how procedure parameters and return values are passed between caller and callee.

**ABI String Literal**

An **ABI String Literal** is a string literal appearing in an `extern` declaration that selects a calling convention.

| ABI String   | Convention                                            | Support      |
| :----------- | :---------------------------------------------------- | :----------- |
| `"C"`        | Default C calling convention for the target platform  | **REQUIRED** |
| `"system"`   | System call convention (stdcall on Win32, C on POSIX) | **REQUIRED** |
| `"stdcall"`  | Windows stdcall convention                            | OPTIONAL     |
| `"fastcall"` | Windows fastcall convention                           | OPTIONAL     |

##### Constraints & Legality

Implementations **MUST** support `"C"` and `"system"` ABI strings. Unknown ABI strings **MUST** be diagnosed.

| Code         | Severity | Condition                   |
| :----------- | :------- | :-------------------------- |
| `E-FFI-3305` | Error    | Unknown ABI string literal. |

---

### 12.2 FFI-Safe Types {Source: Draft 2 §32.4}

##### Definition

**FFI-Safe Type**

A type $T$ is **FFI-Safe** if and only if the predicate $\text{IsFFISafe}(T)$ holds. FFI-Safe types have well-defined representation compatible with C ABIs.

**Formal Definition**

The predicate $\text{IsFFISafe}(T)$ is defined inductively:

$$
\text{IsFFISafe}(T) \iff T \in \mathcal{F}
$$

Where $\mathcal{F}$ is the smallest set satisfying:

1. **Primitive Integers**: $\{$`i8`, `i16`, `i32`, `i64`, `i128`, `isize`, `u8`, `u16`, `u32`, `u64`, `u128`, `usize`$\} \subseteq \mathcal{F}$

2. **Floating Point**: $\{$`f16`, `f32`, `f64`$\} \subseteq \mathcal{F}$

3. **Unit Type**: `()` $\in \mathcal{F}$ (maps to `void` in return position)

4. **Raw Pointers**: If $\text{IsFFISafe}(U)$ or $U = ()$, then `*imm U` $\in \mathcal{F}$ and `*mut U` $\in \mathcal{F}$

5. **Extern Function Pointers**: If all parameter types $P_i$ and return type $R$ satisfy $\text{IsFFISafe}$, then `extern "ABI" ($P_1$, ..., $P_n$) -> $R$` $\in \mathcal{F}$

6. **Repr-C Records**: If `record R` is annotated `[[repr(C)]]` and $\forall f \in \text{fields}(R), \text{IsFFISafe}(\text{type}(f))$, then $R \in \mathcal{F}$

7. **Repr-C Enums**: If `enum E` is annotated `[[repr(C)]]` or `[[repr(IntType)]]`, then $E \in \mathcal{F}$

**Explicit Exclusions**

The following types are expressly **excluded** from FFI signatures:

- Permission-qualified references (`unique T`, `const T`, `partitioned T`, `concurrent T`)
- Slices (`[T]`)
- Strings (`string`, `string@View`, `string@Owned`)
- Closures (non-extern function types that capture environment)
- Modal types not marked `[[repr(C)]]`
- Generic type parameters (must be monomorphized first)
- Trait witness types (`witness Trait`)

##### Static Semantics

**FFI-Safety Judgment**

$$
\frac{\text{IsFFISafe}(T)}{\Gamma \vdash T : \text{FFISafe}}
\tag{FFI-Safe}
$$

**FFI Signature Well-Formedness**

An extern procedure signature is well-formed if all parameter types and the return type are FFI-Safe:

$$
\frac{
    \forall i \in 1..n, \Gamma \vdash P_i : \text{FFISafe} \quad
    \Gamma \vdash R : \text{FFISafe}
}{
    \Gamma \vdash \texttt{extern "ABI" procedure } f(p_1: P_1, \ldots, p_n: P_n): R : WF
}
\tag{WF-Extern-Sig}
$$

##### Constraints & Legality

| Code         | Severity | Condition                                     |
| :----------- | :------- | :-------------------------------------------- |
| `E-FFI-3301` | Error    | Non-FFI-Safe type used in `extern` signature. |

---

### 12.3 Type Representation {Source: Draft 2 §32.3}

##### Definition

**Default Layout (Unspecified)**

For any `record` or `enum` **not** annotated with `[[repr(C)]]`, the memory layout (field ordering, padding, size, alignment) is **Unspecified Behavior (USB)**. The implementation **MAY** reorder fields, insert arbitrary padding, or optimize layout.

**C-Compatible Layout**

The `[[repr(C)]]` attribute guarantees C ABI-compatible memory layout.

##### Syntax & Declaration

**Grammar**

```ebnf
repr_attribute ::= "[[" "repr" "(" repr_kind ")" "]]"
repr_kind      ::= "C" | int_type
int_type       ::= "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64"
```

##### Static Semantics

**Repr-C Record Layout**

For a `record` marked `[[repr(C)]]`:
1. Fields **MUST** be laid out in declaration order
2. Padding **MUST** be inserted only as required to satisfy alignment constraints per the target C ABI
3. The overall size **MUST** be a multiple of the largest field alignment

**Repr-C Enum Layout**

For an `enum` marked `[[repr(C)]]`:
1. The discriminant **MUST** be represented as a C-compatible integer tag
2. Default tag type is implementation-defined (typically `i32`)
3. `[[repr(IntType)]]` (e.g., `[[repr(u8)]]`) explicitly specifies the tag type

**Layout Applicability**

| Declaration   | `[[repr(C)]]` Effect                           |
| :------------ | :--------------------------------------------- |
| `record`      | C struct layout (declaration-order fields)     |
| `enum`        | C union with integer discriminant              |
| `modal`       | **Forbidden** (E-FFI-3303)                     |
| Generic types | **Forbidden** until monomorphized (E-FFI-3303) |

##### Constraints & Legality

| Code         | Severity | Condition                                                     |
| :----------- | :------- | :------------------------------------------------------------ |
| `E-FFI-3303` | Error    | Invalid `[[repr(C)]]` on modal, generic, or unsupported type. |

---

### 12.4 Extern Declarations {Source: Draft 2 §32.1, §32.5.2}

##### Definition

**Extern Declaration**

An **Extern Declaration** introduces a procedure whose implementation crosses the FFI boundary. It takes two forms:

1. **Import**: Declares a procedure whose definition is provided by a foreign library (no body).
2. **Export**: Defines a Cursive procedure to be callable from foreign code (has body).

##### Syntax & Declaration

**Grammar**

```ebnf
extern_decl    ::= attribute_list? visibility?
                   "extern" string_literal
                   "procedure" identifier
                   "(" param_list? ")" (":" return_type)?
                   (";" | block)
```

**Import Syntax** (semicolon terminator):
```cursive
extern "C" procedure malloc(size: usize): *mut ()
extern "C" procedure free(ptr: *mut ())
```

**Export Syntax** (block body):
```cursive
public extern "C" procedure cursive_init(ctx: *mut ()): i32 {
    // Implementation callable from C
    result 0
}
```

**FFI Attributes**

| Attribute              | Target      | Effect                                              |
| :--------------------- | :---------- | :-------------------------------------------------- |
| `[[link_name("sym")]]` | Extern proc | Overrides linker symbol name                        |
| `[[no_mangle]]`        | Extern proc | Disables name mangling (implicit for `extern "C"`)  |
| `[[unwind(mode)]]`     | Extern proc | Controls panic behavior at FFI boundary (see §12.5) |

##### Static Semantics

**Extern Procedure Typing**

$$
\frac{
    \text{ABI} \in \{\text{"C"}, \text{"system"}, \ldots\} \quad
    \Gamma \vdash (P_1, \ldots, P_n) \to R : \text{FFISafe}
}{
    \Gamma \vdash \texttt{extern "ABI" procedure } f(x_1: P_1, \ldots, x_n: P_n): R : \text{ExternProc}
}
\tag{T-Extern-Proc}
$$

**Import vs Export Determination**

- If the declaration ends with `;`, it is an **import** (foreign-defined).
- If the declaration has a block body `{ ... }`, it is an **export** (Cursive-defined).

##### Constraints & Legality

**Variadic Prohibition**

Cursive does **NOT** support variadic arguments (`...`). Variadic extern declarations are ill-formed.

**Export Constraints**

Procedures exported via FFI **MUST** satisfy:
1. Visibility **MUST** be `public`.
2. **MUST NOT** capture environment (no closures).
3. Signature **MUST** consist exclusively of FFI-Safe types.

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-FFI-3301` | Error    | Non-FFI-Safe type in `extern` signature.        |
| `E-FFI-3304` | Error    | Variadic arguments (`...`) are not supported.   |
| `E-FFI-3306` | Error    | Exported extern procedure is not `public`.      |
| `E-FFI-3307` | Error    | Exported extern procedure captures environment. |

##### Examples

**Valid Import:**
```cursive
extern "C" procedure printf(format: *imm i8): i32
```

**Valid Export:**
```cursive
[[no_mangle]]
public extern "C" procedure my_callback(data: *mut (), len: usize): i32 {
    // Safe wrapper around Cursive logic
    result 0
}
```

**Invalid (Non-FFI-Safe):**
```cursive
// ERROR E-FFI-3301: string is not FFI-Safe
extern "C" procedure bad_sig(s: string): i32
```

---

### 12.5 FFI Safety Boundary {Source: Draft 2 §32.2}

##### Definition

**Unverifiable Behavior (UVB) at FFI**

Invoking an imported `extern` procedure is classified as **Unverifiable Behavior (UVB)** per §1.3. The Cursive compiler cannot verify that foreign code upholds memory safety, type safety, or concurrency guarantees.

##### Static Semantics

**Unsafe Requirement for FFI Calls**

A call to an imported `extern` procedure **MUST** occur within an `unsafe` block (§3.10):

$$
\frac{
    \Gamma \vdash f : \text{ExternProc} \quad
    f \text{ is imported} \quad
    \Gamma \vdash_{\text{unsafe}} \text{context}
}{
    \Gamma \vdash f(e_1, \ldots, e_n) : R
}
\tag{T-Extern-Call}
$$

If the call site is not within an `unsafe` block, the implementation **MUST** diagnose error `E-FFI-3302`.

**Safety Documentation Requirement**

Per Appendix F.0, `unsafe` blocks containing FFI calls **SHOULD** be preceded by a `// SAFETY:` comment documenting the safety justification. Organizations **MAY** enforce this via linting.

##### Dynamic Semantics

**Panic Behavior at FFI Boundary**

The `[[unwind(mode)]]` attribute controls what happens when Cursive code panics during execution of an exported extern procedure or when a panic would propagate across an FFI call:

| Mode    | Behavior                                                                        |
| :------ | :------------------------------------------------------------------------------ |
| `abort` | (DEFAULT) Process **MUST** immediately abort. No unwinding across FFI boundary. |
| `catch` | Panic is caught at boundary. Procedure returns a failure value.                 |

**Abort Mode (Default)**

When `[[unwind(abort)]]` applies (or no attribute is present):
1. If Cursive code panics within an exported extern procedure, the runtime **MUST** call `abort()`.
2. Stack unwinding **MUST NOT** cross the FFI boundary.
3. **Rationale**: Foreign code (C/C++) may not be exception-safe; unwinding through foreign frames may corrupt stack state.

**Catch Mode**

When `[[unwind(catch)]]` is specified:
1. The compiler generates a landing pad at the FFI boundary.
2. If a panic occurs, it is caught and converted to a return value.
3. The procedure **MUST** return a type capable of representing failure (e.g., integer error code, nullable pointer).

**Constraint**: If the return type cannot represent failure, the behavior is **Unverifiable (UVB)**.

##### Constraints & Legality

| Code         | Severity | Condition                                              |
| :----------- | :------- | :----------------------------------------------------- |
| `E-FFI-3302` | Error    | Call to imported `extern` procedure outside `unsafe`.  |
| `W-FFI-3301` | Warning  | `[[unwind(catch)]]` on non-error-representable return. |

##### Examples

**Correct FFI Call:**
```cursive
extern "C" procedure c_function(x: i32): i32

procedure call_c(x: i32): i32 {
    unsafe {
        c_function(x)  // OK: inside unsafe block
    }
}
```

**Incorrect FFI Call:**
```cursive
extern "C" procedure c_function(x: i32): i32

procedure call_c(x: i32): i32 {
    c_function(x)  // ERROR E-FFI-3302: outside unsafe
}
```

**Unwind Catch Example:**
```cursive
[[unwind(catch)]]
public extern "C" procedure safe_callback(): i32 {
    // If this panics, returns -1 instead of aborting
    do_risky_operation()
    result 0
}
```

---

### Clause 12: Interoperability Cross-Reference Notes

**Terms defined in Clause 12 that MUST NOT be redefined elsewhere:**

| Term                               | Section | Description                                                          |
| :--------------------------------- | :------ | :------------------------------------------------------------------- |
| Foreign Function Interface (FFI)   | §12.1   | Mechanisms for calling/exporting procedures across language boundary |
| Application Binary Interface (ABI) | §12.1   | Low-level conventions for compiled code interoperation               |
| Calling Convention                 | §12.1   | ABI subset for parameter/return value passing                        |
| ABI String Literal                 | §12.1   | String literal selecting calling convention in extern declarations   |
| FFI-Safe Type                      | §12.2   | Type with C ABI-compatible representation                            |
| IsFFISafe(T)                       | §12.2   | Predicate determining FFI-safety of type T                           |
| Extern Declaration                 | §12.4   | Declaration of imported or exported FFI procedure                    |
| C-Compatible Layout                | §12.3   | Memory layout guaranteed by `[[repr(C)]]` attribute                  |

**Terms referenced from other clauses:**

| Term                        | Source   | Usage in Clause 12                                  |
| :-------------------------- | :------- | :-------------------------------------------------- |
| Unverifiable Behavior (UVB) | §1.3     | FFI calls are classified as UVB                     |
| Unspecified Behavior (USB)  | §1.3     | Default (non-repr-C) layout is USB                  |
| Unsafe Block                | §3.10    | Required context for imported extern calls          |
| Safety Comment Format       | Appx F.0 | Recommended `// SAFETY:` documentation for FFI code |
| Raw Pointer Types           | §4.13    | `*imm T`, `*mut T` used in FFI signatures           |
| Conformance Mode            | §1.2     | Strict vs Permissive compilation mode               |
| Panic                       | §9       | Exception mechanism; controlled at FFI boundary     |

**Terms deferred to later clauses:**

| Term   | Deferred To | Reason                                         |
| :----- | :---------- | :--------------------------------------------- |
| (none) | -           | Clause 12 is terminal for FFI/interoperability |

---

## Appendices
### Appendix A: Formal Grammar (ANTLR) {Source: Draft 2 Appx A}

  <u>Definition</u>
    Complete normative grammar for Cursive in ANTLR4 format. This grammar defines all lexical and syntactic productions required to parse valid Cursive source code.
  
  <u>Syntax & Declaration</u>
    **Grammar Structure**:
    *   **Lexer Rules**: Keywords, identifiers (XID_START/XID_Continue), literals (integer/float/string/char), operators, comments
    *   **Parser Rules**: All declarations (record, enum, modal, trait, procedure), expressions (precedence-encoded), statements, patterns
    *   **Operator Precedence**: Encoded in grammar hierarchy (14 levels from Postfix to Assignment)
    *   **Comment Nesting**: Block comments (`/* ... */`) MUST nest recursively; lexer maintains nesting counter
    *   **Maximal Munch Exception**: Sequence `>>` inside generic argument list MUST be split into two `>` tokens (context-sensitive lexing)
    
  <u>Constraints & Legality</u>
    *   Keywords are reserved context-insensitively
    *   Receiver shorthands (`~`, `~%`, `~!`) MUST appear only as first parameter in type method declarations
    *   All lexical rules MUST conform to preprocessing pipeline output (§2.1)
  
  <u>Examples</u>
    Complete ANTLR4 grammar suitable for parser generators. Includes all production rules from top-level `sourceFile` to terminal tokens.

### Appendix B: Diagnostic Code Taxonomy {Source: Draft 2 Appx B}

This appendix defines the normative taxonomy for compiler diagnostics. All conforming implementations **MUST** use these codes when reporting the corresponding conditions to ensure consistent error reporting across toolchains.

#### B.1 Diagnostic Code Format

Diagnostic codes follow the format `K-CAT-FFNN`:

*   **K (Kind/Severity):**
    *   `E`: **Error**. A violation of a normative requirement. Compilation cannot proceed to codegen.
    *   `W`: **Warning**. Code is well-formed but contains potential issues (e.g., deprecated usage).
    *   `N`: **Note**. Informational message attached to an error or warning.
*   **CAT (Category):** A three-letter code identifying the language subsystem.
*   **FF (Feature Bucket):** Two digits identifying the specific feature area or chapter within the category.
*   **NN (Number):** Two digits uniquely identifying the specific condition within the bucket.

#### B.2 Feature Buckets (FF Values)

The following tables define the "Feature Bucket" (`FF`) values for each Category (`CAT`). These values generally align with the specification chapters but are grouped for diagnostic utility.

##### B.2.1 Source, Syntax, & Modules

| Category | FF   | Feature Name         | Relevant Chapters |
| :------- | :--- | :------------------- | :---------------- |
| **SRC**  | 01   | Encoding & Structure | §8.1, §8.2        |
|          | 02   | Resource Limits      | §6.5, §8.2.2      |
|          | 03   | Lexical Elements     | §9                |
| **SYN**  | 01   | Recursive Limits     | §10.2             |
|          | 02   | General Grammar      | Appx A            |
| **MOD**  | 11   | Manifest & Projects  | §11               |
|          | 12   | Imports & Visibility | §12               |
|          | 14   | Initialization       | §14               |
| **NAM**  | 13   | Scopes & Shadowing   | §13               |

##### B.2.2 Type System

| Category | FF   | Feature Name               | Relevant Chapters |
| :------- | :--- | :------------------------- | :---------------- |
| **TYP**  | 15   | Foundations                | §15               |
|          | 16   | Permissions                | §16               |
|          | 17   | Primitives                 | §17               |
|          | 18   | Composites                 | §18               |
|          | 19   | Modal Types (incl. String) | §19, §20          |
|          | 20   | Pointers                   | §21               |
|          | 23   | Function Types             | §22               |

##### B.2.3 Logic & Semantics

| Category | FF   | Feature Name     | Relevant Chapters |
| :------- | :--- | :--------------- | :---------------- |
| **DEC**  | 24   | Declarations     | §24               |
| **EXP**  | 25   | Expressions      | §25               |
| **STM**  | 26   | Statements       | §26               |
| **PAT**  | 27   | Pattern Matching | §27               |
| **CON**  | 28   | Contracts        | §28               |
|          | 32   | Concurrency      | §32               |

##### B.2.4 Systems & Metaprogramming

| Category | FF   | Feature Name        | Relevant Chapters |
| :------- | :--- | :------------------ | :---------------- |
| **TRS**  | 29   | Traits              | §29               |
| **MEM**  | 30   | Memory Model        | §30               |
|          | 31   | Object Capabilities | §31               |
| **FFI**  | 33   | FFI & ABI           | §33               |
| **MET**  | 34   | Metaprogramming     | §34               |

#### B.3 Normative Diagnostic Catalog

**IMPORTANT NOTE FOR WRITERS**: This section MUST enumerate EVERY diagnostic code from EVERY chapter's "Diagnostics Summary" section in Draft2. The catalog below is illustrative structure only.

**Requirements**:
- Each code entry MUST include: Code identifier, Severity, Description
- Codes MUST be organized by Category (SRC, SYN, MOD, NAM, TYP, DEC, EXP, STM, PAT, CON, TRS, MEM, FFI, MET)
- Writers MUST cross-reference each chapter's diagnostic summary to ensure no codes omitted
- Every diagnostic triggered in specification text must have entry here

The following tables list all diagnostic codes required by this specification, organized by their Category and Feature Bucket.

##### B.3.1 SRC (Source)

| Code         | Severity | Description                                                                  |
| :----------- | :------- | :--------------------------------------------------------------------------- |
| `E-SRC-0101` | Error    | Invalid UTF-8 byte sequence in source file.                                  |
| `E-SRC-0102` | Error    | Source file exceeds implementation-defined maximum size.                     |
| `E-SRC-0103` | Error    | UTF-8 Byte Order Mark (BOM) found after the first byte.                      |
| `E-SRC-0104` | Error    | Source contains prohibited control character or null byte.                   |
| `E-SRC-0105` | Error    | Maximum logical line count exceeded.                                         |
| `E-SRC-0106` | Error    | Maximum line length exceeded.                                                |
| `W-SRC-0101` | Warning  | Source file begins with UTF-8 BOM (should be stripped).                      |
| `E-SRC-0204` | Error    | String literal length exceeds implementation limit during compile-time eval. |
| `E-SRC-0301` | Error    | Unterminated string literal.                                                 |
| `E-SRC-0302` | Error    | Invalid escape sequence in literal.                                          |
| `E-SRC-0303` | Error    | Invalid character literal (empty, multiple chars).                           |
| `E-SRC-0304` | Error    | Malformed numeric literal.                                                   |
| `E-SRC-0305` | Error    | Reserved keyword used as identifier.                                         |
| `E-SRC-0306` | Error    | Unterminated block comment.                                                  |
| `E-SRC-0307` | Error    | Invalid Unicode in identifier (violates UAX31).                              |
| `E-SRC-0308` | Error    | Lexically sensitive Unicode character in identifier (Strict Mode).           |
| `W-SRC-0301` | Warning  | Integer literal has leading zeros (interpreted as decimal, not octal).       |
| `W-SRC-0308` | Warning  | Lexically sensitive Unicode character in identifier (Permissive Mode).       |

##### B.3.2 SYN (Syntax)

| Code         | Severity | Description                                             |
| :----------- | :------- | :------------------------------------------------------ |
| `E-SYN-0101` | Error    | Block nesting depth exceeded implementation limit.      |
| `E-SYN-0102` | Error    | Expression nesting depth exceeded implementation limit. |
| `E-SYN-0103` | Error    | Delimiter nesting depth exceeded implementation limit.  |

##### B.3.3 MOD (Modules) & NAM (Naming)

| Code         | Severity | Description                                           |
| :----------- | :------- | :---------------------------------------------------- |
| `E-MOD-1101` | Error    | `Cursive.toml` missing or syntactically malformed.    |
| `E-MOD-1102` | Error    | Manifest `[paths]` table missing or invalid.          |
| `E-MOD-1103` | Error    | Assembly references undefined root in `[paths]`.      |
| `E-MOD-1104` | Error    | Module path collision on case-insensitive filesystem. |
| `E-MOD-1105` | Error    | Module path component is a reserved keyword.          |
| `E-MOD-1106` | Error    | Module path component is not a valid identifier.      |
| `E-MOD-1107` | Error    | Manifest `[project]` table missing required keys.     |
| `E-MOD-1108` | Error    | Duplicate assembly name in manifest.                  |
| `E-MOD-1109` | Error    | Incompatible language version in manifest.            |
| `E-MOD-1201` | Error    | `import` path does not resolve to external module.    |
| `E-MOD-1202` | Error    | `use` path does not resolve to accessible module.     |
| `E-MOD-1203` | Error    | Name conflict in `use` or `import as`.                |
| `E-MOD-1204` | Error    | Item in `use` path not found or not visible.          |
| `E-MOD-1205` | Error    | Attempt to `public use` a non-public item.            |
| `E-MOD-1206` | Error    | Duplicate item in `use` list.                         |
| `E-MOD-1207` | Error    | Access to `protected` item denied.                    |
| `E-MOD-1401` | Error    | Cyclic dependency in eager module initialization.     |
| `W-MOD-1101` | Warning  | Potential module path collision (case-sensitivity).   |
| `W-MOD-1201` | Warning  | Wildcard `use` (`*`) usage.                           |
| `E-NAM-1301` | Error    | Unresolved name.                                      |
| `E-NAM-1302` | Error    | Duplicate name declaration in same scope.             |
| `E-NAM-1303` | Error    | Shadowing existing binding without `shadow` keyword.  |
| `E-NAM-1304` | Error    | Unresolved module in qualified path.                  |
| `E-NAM-1305` | Error    | Unresolved or inaccessible member in path.            |
| `E-NAM-1306` | Error    | Unnecessary use of `shadow` keyword.                  |
| `W-NAM-1303` | Warning  | Implicit shadowing (Permissive Mode).                 |

##### B.3.4 TYP (Type System)

| Code         | Severity | Description                                             |
| :----------- | :------- | :------------------------------------------------------ |
| `E-TYP-1501` | Error    | Type mismatch or other type checking failure.           |
| `E-TYP-1505` | Error    | Missing required top-level type annotation.             |
| `E-TYP-1601` | Error    | Attempt to mutate data via a `const` reference.         |
| `E-TYP-1602` | Error    | Violation of `unique` exclusion (aliasing detected).    |
| `E-TYP-1603` | Error    | Partitioning system violation.                          |
| `W-TYP-1701` | Warning  | `f16` arithmetic may be emulated and slow.              |
| `E-TYP-1801` | Error    | Tuple or Array index out of bounds.                     |
| `E-TYP-1901` | Panic    | String slice boundary is not a valid `char` boundary.   |
| `E-TYP-1902` | Error    | Direct indexing of `string` is forbidden.               |
| `E-TYP-1910` | Error    | Modal type declares no states.                          |
| `E-TYP-1911` | Error    | Duplicate state in modal declaration.                   |
| `E-TYP-1912` | Error    | Accessing field existing only in specific modal state.  |
| `E-TYP-1913` | Error    | Accessing method existing only in specific modal state. |
| `E-TYP-1914` | Error    | Missing implementation for modal transition.            |
| `E-TYP-1915` | Error    | Transition body does not return target state type.      |
| `E-TYP-1920` | Error    | Non-exhaustive match on modal type.                     |
| `E-TYP-2001` | Error    | Dereference of `Ptr<T>@Null`.                           |
| `E-TYP-2002` | Error    | Dereference of `Ptr<T>@Expired`.                        |
| `E-TYP-2003` | Error    | Dereference of raw pointer outside `unsafe`.            |
| `E-TYP-2301` | Error    | Function argument count mismatch.                       |
| `E-TYP-2302` | Error    | Type mismatch (general).                                |
| `E-TYP-2304` | Error    | Control flow path missing `result` value.               |

##### B.3.5 DEC (Declarations), EXP (Expressions), STM (Statements)

| Code         | Severity | Description                                           |
| :----------- | :------- | :---------------------------------------------------- |
| `E-DEC-2401` | Error    | Re-assignment of immutable `let` binding.             |
| `E-DEC-2411` | Error    | `move` keyword mismatch at call site.                 |
| `E-DEC-2420` | Error    | Recursive type alias detected.                        |
| `E-DEC-2430` | Error    | Missing or duplicate `main` procedure.                |
| `E-DEC-2431` | Error    | Invalid signature for `main` (must accept `Context`). |
| `E-DEC-2440` | Error    | `protected` used on top-level declaration.            |
| `E-DEC-2451` | Error    | Unknown attribute or unknown field in attribute.      |
| `E-EXP-2501` | Error    | Expression type mismatch.                             |
| `E-EXP-2502` | Error    | `value` expression used where `place` required.       |
| `E-EXP-2511` | Error    | Identifier resolves to type/module in value context.  |
| `E-EXP-2531` | Error    | Invalid field/tuple access (not found or invisible).  |
| `E-EXP-2532` | Error    | Procedure call argument count mismatch.               |
| `E-EXP-2533` | Error    | Method call using `.` instead of `::`.                |
| `E-EXP-2535` | Error    | Invalid pipeline `=>` right-hand side.                |
| `E-EXP-2541` | Error    | Logical operator applied to non-bool.                 |
| `E-EXP-2542` | Error    | Invalid types for arithmetic/bitwise operator.        |
| `E-EXP-2545` | Error    | Address-of `&` applied to non-place.                  |
| `E-EXP-2561` | Error    | `if` without `else` in value context.                 |
| `E-EXP-2571` | Error    | Incompatible types in `match` arms.                   |
| `E-EXP-2582` | Error    | Mismatched types in `loop` `break` values.            |
| `E-EXP-2591` | Error    | Returning region-allocated value from `region` block. |
| `E-EXP-2592` | Error    | `parallel` result depends on invalidated binding.     |
| `E-STM-2631` | Error    | Assignment target is not a place.                     |
| `E-STM-2651` | Error    | `defer` block returns non-unit value.                 |
| `E-STM-2652` | Error    | Non-local control flow (return/break) in `defer`.     |
| `E-STM-2661` | Error    | `return` value type mismatch.                         |
| `E-STM-2662` | Error    | `break` used outside loop.                            |
| `E-STM-2663` | Error    | `continue` used outside loop.                         |
| `E-STM-2664` | Error    | `result` value mismatch with block type.              |
| `E-STM-2671` | Error    | `partition` proof verification failed.                |

##### B.3.6 PAT (Patterns)

| Code         | Severity | Description                                    |
| :----------- | :------- | :--------------------------------------------- |
| `E-PAT-2711` | Error    | Refutable pattern used in irrefutable context. |
| `E-PAT-2741` | Error    | Non-exhaustive pattern match.                  |
| `E-PAT-2751` | Error    | Unreachable match arm.                         |

##### B.3.7 CON (Contracts & Concurrency)

| Code         | Severity | Description                                                              |
| :----------- | :------- | :----------------------------------------------------------------------- |
| `E-CON-2801` | Error    | Static contract verification failed.                                     |
| `E-CON-2802` | Error    | Impure/Unsafe expression in contract.                                    |
| `E-CON-2803` | Error    | Liskov violation: Precondition strengthened.                             |
| `E-CON-2804` | Error    | Liskov violation: Postcondition weakened.                                |
| `E-CON-2805` | Error    | `@entry` applied to non-Copy/Clone type.                                 |
| `E-CON-2806` | Error    | `@result` used outside `will` clause.                                    |
| `E-CON-3201` | Error    | Thread Safety: Capturing `partitioned` binding in concurrency primitive. |
| `E-CON-3202` | Error    | Static Join violation: `JobHandle` not joined in `parallel`.             |
| `E-CON-3203` | Error    | Spawn capture violation (e.g. `unique` not moved).                       |

##### B.3.8 TRS (Traits)

| Code         | Severity | Description                                            |
| :----------- | :------- | :----------------------------------------------------- |
| `E-TRS-2901` | Error    | Abstract implementation incorrectly marked `override`. |
| `E-TRS-2902` | Error    | Concrete override missing `override` keyword.          |
| `E-TRS-2903` | Error    | Missing implementation for required trait procedure.   |
| `E-TRS-2910` | Error    | Accessing non-trait member on opaque type.             |
| `E-TRS-2920` | Error    | Explicit call to `Drop::drop`.                         |
| `E-TRS-2921` | Error    | Type implements both `Copy` and `Drop`.                |
| `E-TRS-2922` | Error    | `Copy` implementation on type with non-Copy fields.    |
| `E-TRS-2940` | Error    | Calling `where Self: Sized` procedure on witness.      |

##### B.3.9 MEM (Memory & Safety)

| Code         | Severity | Description                                 |
| :----------- | :------- | :------------------------------------------ |
| `E-MEM-3001` | Error    | Use of moved value.                         |
| `E-MEM-3005` | Error    | Explicit call to destructor.                |
| `E-MEM-3010` | Error    | Static record partition conflict.           |
| `E-MEM-3012` | Error    | Partition contract proof failed (Static).   |
| `P-MEM-3013` | Panic    | Dynamic partition check failed (Runtime).   |
| `E-MEM-3020` | Error    | Region pointer escape.                      |
| `E-MEM-3021` | Error    | Region allocation `^` outside region scope. |
| `E-MEM-3030` | Error    | Unsafe operation in safe code.              |
| `E-MEM-3031` | Error    | `transmute` size mismatch.                  |

##### B.3.10 FFI (Interoperability)

| Code         | Severity | Description                                        |
| :----------- | :------- | :------------------------------------------------- |
| `E-FFI-3301` | Error    | Non-FFI-Safe type in `extern` signature.           |
| `E-FFI-3302` | Error    | Call to `extern` procedure outside `unsafe` block. |
| `E-FFI-3303` | Error    | Invalid application of `[[repr(C)]]`.              |
| `E-FFI-3304` | Error    | Variadic arguments not supported.                  |

##### B.3.11 MET (Metaprogramming)

| Code         | Severity | Description                                       |
| :----------- | :------- | :------------------------------------------------ |
| `E-MET-3401` | Error    | `comptime` procedure called from runtime context. |
| `E-MET-3402` | Error    | Compile-time resource limit exceeded.             |
| `E-MET-3403` | Error    | Invalid identifier string in interpolation.       |
| `E-MET-3404` | Error    | Syntax error in `quote` block.                    |
| `E-MET-3405` | Error    | Emitted AST failed type checking.                 |
| `E-MET-3406` | Error    | `emit` called without capability.                 |

### Appendix C: Conformance Dossier Schema {Source: Draft 2 Appx C}

This appendix defines the normative JSON Schema for the Conformance Dossier. A conforming implementation **MUST** produce a JSON artifact matching this schema when the `dossier` emission phase is active.

#### C.1 File Format
The dossier **MUST** be a valid JSON document encoded in UTF-8.

#### C.2 Schema Definition
```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Cursive Conformance Dossier",
  "type": "object",
  "required": ["metadata", "configuration", "safety_report", "implementation_limits"],
  "properties": {
    
    "metadata": {
      "type": "object",
      "description": "Information about the compiler and build environment.",
      "required": ["compiler_id", "compiler_version", "target_triple", "build_timestamp"],
      "properties": {
        "compiler_id": { "type": "string", "example": "cursive-ref-impl" },
        "compiler_version": { "type": "string", "pattern": "^\\d+\\.\\d+\\.\\d+$" },
        "target_triple": { "type": "string", "example": "x86_64-unknown-linux-gnu" },
        "build_timestamp": { "type": "string", "format": "date-time" }
      }
    },

    "configuration": {
      "type": "object",
      "description": "The conformance mode and settings used for this compilation.",
      "required": ["mode", "features"],
      "properties": {
        "mode": {
          "type": "string",
          "enum": ["strict", "permissive"],
          "description": "Strict mode treats implicit shadowing and ambiguous Unicode as errors; permissive mode treats them as warnings."
        },
        "features": {
          "type": "array",
          "items": { "type": "string" },
          "description": "List of enabled language feature flags."
        }
      }
    },

    "safety_report": {
      "type": "object",
      "description": "Inventory of unsafe usage.",
      "required": ["unsafe_blocks_count", "ifndr_instances"],
      "properties": {
        "unsafe_blocks_count": { "type": "integer", "minimum": 0 },
        "ifndr_instances": {
          "type": "array",
          "description": "List of all IFNDR (Ill-Formed, No Diagnostic Required) instances encountered during compilation.",
          "items": {
            "type": "object",
            "required": ["location", "category"],
            "properties": {
              "location": {
                "type": "object",
                "properties": {
                  "file": { "type": "string" },
                  "line": { "type": "integer" },
                  "module": { "type": "string" }
                }
              },
              "category": {
                "type": "string",
                "description": "IFNDR category (e.g., 'OOB in Const Eval', 'Recursion Limits')"
              },
              "description": {
                "type": "string",
                "description": "Optional additional context about the IFNDR condition"
              }
            }
          }
        }
      }
    },

    "implementation_defined_behavior": {
      "type": "object",
      "description": "Documentation of IDB choices made by this implementation.",
      "required": ["type_layout", "pointer_width"],
      "properties": {
        "pointer_width": { "type": "integer", "enum": [32, 64] },
        "type_layout": {
          "type": "object",
          "additionalProperties": {
             "type": "object",
             "properties": {
               "size": { "type": "integer" },
               "alignment": { "type": "integer" }
             }
          },
          "description": "Layout map for primitive types (i32, usize, etc)."
        }
      }
    },
    
    "implementation_limits": {
      "type": "object",
      "description": "Actual limits enforced by this implementation.",
      "required": ["max_recursion_depth", "max_identifier_length"],
      "properties": {
        "max_recursion_depth": { "type": "integer" },
        "max_identifier_length": { "type": "integer" },
        "max_source_size": { "type": "integer" }
      }
    }
  }
}
```

### Appendix D: Standard Trait Catalog {Source: Draft 2 Appx D}

This appendix provides normative definitions for foundational traits and system capability traits built into Cursive or its core library.

  <u>Definition</u>
    Normative definitions for foundational traits and system capability traits that are deeply integrated with language mechanics.
  
  <u>Syntax & Declaration</u>
    **Foundational Traits** (§D.1):
    *   `Drop`: `procedure drop(~!)` - RAII cleanup, compiler-invoked only
    *   `Copy`: Marker trait for implicit bitwise duplication
    *   `Clone`: `procedure clone(~): Self` - explicit deep copy

    **Note on `concurrent` Permission**: Thread safety for the `concurrent` permission is not governed by a marker trait. Instead, types opt-in to `concurrent` access by providing methods that accept `concurrent self` (or `~|`). Types like `Mutex<T>` and `Atomic<T>` support `concurrent` because they provide synchronized methods. See §4.5 for full semantics.
    
    **System Capability Traits** (§D.2):
    *   `FileSystem`:
        -   `open(path: string@View, mode: FileMode): FileHandle | IoError`
        -   `exists(path: string@View): bool`
        -   `restrict(path: string@View): witness FileSystem` (Attenuation)
    *   `Network`:
        -   `connect(addr: NetAddr): Stream | NetError`
        -   `bind(addr: NetAddr): Listener | NetError`
        -   `restrict_to_host(addr: NetAddr): witness Network` (Attenuation)
    *   `HeapAllocator`:
        -   `alloc<T>(count: usize): *mut T`
        -   `dealloc<T>(ptr: *mut T, count: usize)`
        -   `with_quota(size: usize): witness HeapAllocator` (Attenuation)
    *   `System`:
        -   `exit(code: i32): !`
        -   `get_env(key: string@View): string`
        -   `spawn<T>(closure: () -> T): Thread<T>@Spawned` (Concurrency Path 2)
        -   `create_mutex<T>(value: T): Mutex<T>@Unlocked` (Concurrency Path 2)
        -   `time(): Timestamp`
    *   `Time`: Monotonic time access with `now(): Timestamp` procedure
  
  <u>Constraints & Legality</u>
    *   `Drop::drop` MUST NOT be called directly by user code (E-TRS-2920)
    *   `Copy` and `Drop` are mutually exclusive on same type (E-TRS-2921)
    *   `Copy` requires all fields implement `Copy` (E-TRS-2922)
    *   `HeapAllocator::alloc` MUST panic on OOM (never return null)
    *   Variadic implementations across all trait types are prohibited
  
  <u>Static Semantics</u>
    *   Compiler automatically inserts `Drop::drop` calls at scope exit for responsible bindings
    *   `Copy` types are duplicated (not moved) on assignment/parameter passing
    *   Static invalidation applies to aliases when Drop is invoked on owner
    *   Capability traits enable attenuation patterns (e.g., `with_quota` on HeapAllocator)
  
  <u>Dynamic Semantics</u>
    *   Drop execution grants temporary exclusive (unique) access to self
    *   Drop during panic unwind: second panic causes process abort
    *   HeapAllocator failures cause panic (no null returns)
  
  <u>Examples</u>
    Complete trait signatures must be provided for: Drop, Copy, Clone, HeapAllocator, FileSystem, Network, Time, System (which implements Time and provides `exit`, `get_env`).

### Appendix E: Core Library Specification {Source: Draft 2 Appx E}

  <u>Definition</u>
    Minimal normative definitions for core types assumed to be available by the language without explicit import.
  
  <u>Syntax & Declaration</u>
    **Context Capability Record**:
    *   `Context` record structure (see §30.1, §30.2):
        - `fs: witness FileSystem`
        - `net: witness Network`
        - `sys: System` (implements Time)
        - `heap: witness HeapAllocator`

    *(Note: `Option` and `Result` are removed. Optionality is handled via Union Types e.g., `T | ()`, and failure via `T | Error`. Pointers handle nullability via `Ptr<T>@Null` states).*
  
  <u>Constraints & Legality</u>
    *   `Context` field types MUST match the capability trait witnesses defined in Appendix D
    *   Core library types (`Option`, `Result`, `Context`, primitives) MUST be available in the universe scope (no import required)
    *   `main` procedure MUST accept `Context` parameter as defined
  
  <u>Static Semantics</u>
    *   All context capabilities are passed explicitly (no ambient authority)
  


### Appendix F: Implementation Guide (Informative) {Source: Draft 2 Appx G}

**Note**: This appendix provides non-normative guidance for compiler implementers.

#### F.0 Canonical Safety Comment Format

  <u>Definition</u>
    This section defines the recommended format for documenting `unsafe` code. This is a **style guideline**, not a language requirement. Enforcement is delegated to linters, CI systems, and organizational policy.

  <u>Syntax</u>
    Safety comments **SHOULD** immediately precede `unsafe` blocks using the following format:

    ```
    // SAFETY: <rationale>
    ```

    For complex justifications spanning multiple lines:

    ```
    // SAFETY: <summary>
    // - <point 1>
    // - <point 2>
    // - <point N>
    ```

    Organizations requiring audit metadata **MAY** extend the format:

    ```
    // SAFETY: <rationale>
    // REVIEWED: <reviewer> (<reference>)
    ```

  <u>Rationale</u>
    The `SAFETY:` prefix enables:
    - Grep/search for all safety justifications in a codebase
    - Linter rules requiring safety comments on unsafe blocks
    - IDE tooling to highlight undocumented unsafe code
    - Audit tooling to extract safety documentation

    This approach delegates enforcement to appropriate tooling rather than embedding organizational process into the language grammar. The compiler cannot verify that a safety justification is correct—only that the syntax is present. Therefore, this responsibility belongs at the tooling layer where it can be customized per organization.

  <u>Examples</u>

    **Simple safety justification:**

    ```cursive
    // SAFETY: `ptr` is non-null and properly aligned because it was obtained
    // from `Vec::as_mut_ptr()` and the Vec is still alive.
    unsafe {
        *ptr = value
    }
    ```

    **FFI boundary documentation:**

    ```cursive
    // SAFETY: The C library contract guarantees:
    // - The returned pointer is valid for the lifetime of the context
    // - The pointer is aligned to 8 bytes
    // - The memory is initialized with zeroes
    // REVIEWED: security-team (AUDIT-2025-0042)
    unsafe {
        let data = ffi_get_buffer(ctx)
        slice::from_raw_parts(data, len)
    }
    ```

    **Transmute justification:**

    ```cursive
    // SAFETY: Transmuting [u8; 4] to u32 is valid because:
    // - Both types have size 4 and alignment ≤ 4
    // - All bit patterns are valid for u32
    // - We accept platform-specific byte order (native endian)
    unsafe {
        transmute::<[u8; 4], u32>(bytes)
    }
    ```

  <u>Recommended Lint Rules</u>
    Implementations **SHOULD** provide the following configurable lint:

    | Lint Name                    | Default | Description                                           |
    | :--------------------------- | :------ | :---------------------------------------------------- |
    | `undocumented_unsafe_blocks` | `warn`  | `unsafe` block without preceding `// SAFETY:` comment |

    Organizations **MAY** configure this lint to `deny` for production builds.

#### F.1 Control Flow Graph (CFG) for Verification

  <u>Static Semantics</u>
    Guidance for implementing Fact Injection (§27.7, Clause 7.2) and Partitioning verification (§29.4, Clause 3.4):
    *   **Dominator Trees**: Use Lengauer-Tarjan algorithm to build dominator tree
    *   **Verification Facts**: Facts are valid only if origin node strictly dominates consumption node
    *   **Loop Headers**: Synthesize entry facts for loop induction variables at loop header node
    *   **Example**: For loop `i in 0..N`, inject facts `i >= 0` and `i < N` dominating loop body
  
#### F.2 Niche Optimization for Modal Types

  <u>Memory & Layout</u>
    Strongly encouraged optimization to reduce modal type size:
    *   Use invalid bit-patterns (niches) in state payloads to encode discriminant
    *   **Example**: `Ptr<T>` uses address `0x0` for `@Null` state, non-zero for `@Valid` state
    *   **Result**: `Ptr<T>` general type occupies same space as raw pointer (64 bits, not 128)
    *   **General Algorithm**: Identify niches in payload types, map empty states to niche patterns
  
#### F.3 Canonical Formatting

  <u>Definition</u>
    Recommended formatting to satisfy syntactic stability goals (§4.8, Clause 1.6):
    *   **Indentation**: 4 spaces (no tabs)
    *   **Brace Style**: 1TBS (One True Brace Style) - opening brace on same line as declaration
    *   **Import Ordering**: Alphabetically sorted
    *   **Declaration Ordering**: public declarations first, then internal, then private within each category

### Appendix G: Behavior Classification Index (Normative) {Source: Draft 2 Appx H}
**H.1 Unverifiable Behavior (UVB)**:
*   FFI Call (§32.2)
*   Raw Deref (§29.6.2)
*   Transmute (§29.6.3)
*   Pointer Arithmetic (§24.4.2)
*   Trusted Contracts within `unsafe` (§7.4)
*   Trusted Partitions (`[[verify(trusted)]]`)

**H.2 Implementation-Defined Behavior (IDB)**:
*   Type Layout (non-C)
*   Integer Overflow (Release)
*   Pointer Width
*   Resource Limits
*   Panic Abort Mechanism

### H.3 Unspecified Behavior (USB) {Source: Draft 2 Appx H.3}

The following behaviors are bounded by the language safety guarantees but are not documented or consistent between executions.

*   **Map Iteration**: Order of iteration for hash-based collections.
*   **Padding Bytes**: The values of padding bytes in non-`[[repr(C)]]` records.

### H.4 Defined Runtime Failure (Panics)

*   Integer Overflow (Checked Mode)
*   Array/Slice Bounds Check
*   **Dynamic Partition Overlap** (`[[verify(dynamic)]]`)

### Appendix H: Formal Core Semantics (Normative) {Source: Draft 2 Appx I}

This appendix defines the **Cursive Core Calculus**, a simplified formal model of the language's memory and permission system. Implementations **MUST** preserve the safety properties defined here.

#### H.1 Syntax of the Core

$$\begin{aligned} v &::= \ell \mid \text{const } \ell \mid \text{null} \\ e &::= v \mid \text{let } x = e \text{ in } e \mid x \mid x.f \mid x.f \leftarrow v \mid \text{fork}(e) \\ \tau &::= \text{const } T \mid \text{unique } T \mid \text{partitioned } T \end{aligned}$$

#### H.2 Operational Semantics (Small-Step)

State is defined as a pair $(H, e)$ where $H$ is the heap mapping locations $\ell$ to values.

**Read Rule:**

$$\frac{H(\ell) = v}{(H, \ell.f) \longrightarrow (H, v.f)}$$  
**Write Rule (Unique):**

$$\frac{H(\ell) \text{ is live}}{(H, \ell.f \leftarrow v') \longrightarrow (H[\ell.f \mapsto v'], ())}$$

#### H.3 Safety Theorems

##### Theorem 1: Progress
If $\vdash e : \tau$ and $e$ is not a value, then there exists a state $(H', e')$ such that $(H, e) \longrightarrow (H', e')$.  
Implication: A well-typed Cursive program never gets "stuck" (segfaults or undefined behavior).

##### Theorem 2: Preservation (Type Safety)
If $\vdash e : \tau$ and $(H, e) \longrightarrow (H', e')$, then $\vdash e' : \tau$.  
Implication: Operations never violate the permission rules defined in Part 4\.  

##### Theorem 3: Data Race Freedom
If $\Gamma \vdash e : \text{well-formed}$, and $e$ contains fork, no two threads can access location $\ell$ simultaneously unless both accesses are Reads.  
_Proof Sketch:_ The unique permission ($\ell$) cannot be duplicated. The const permission ($\text{const } \ell$) allows duplication but removes the Write Rule from the set of valid reductions for that value.

### Appendix I: Implementation Limits {Source: Draft 2 §6.5, Appx F}
**Minimum Guaranteed Limits**:
*   **Source Size**: 1 MiB
*   **Logical Lines**: 65,535
*   **Line Length**: 16,384 chars
*   **Nesting Depth**: 256
*   **Identifier Length**: 1,023 chars
*   **Parameters**: 255
*   **Fields**: 1,024
*   **Recursion Depth**: 256 (Comptime), Implementation-Defined (Runtime)
