# The Cursive Language Specification (Draft 3)

# Part I: General Principles and Conformance

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

$$\text{Conforming}_{\mathcal{P}}(p) \iff \text{WellFormed}(p)$$

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

Since well-formed programs satisfy all lexical, syntactic, and static-semantic constraints, a conforming implementation MUST accept them without errors.

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
| `E-CNF-0301` | Error    | Implementation limit exceeded (see §1.4).                            | Compile-time | Rejection |

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

**Diagnostic Coordination**

Multiple diagnostic codes across different clauses may be triggered by the same underlying program violation when detected at different compilation phases. See Appendix J for a cross-reference matrix mapping related diagnostics.

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

Let $\text{IllFormed}(p)$ denote that program $p$ violates at least one static-semantic rule. Let $\text{DetectionInfeasible}(p)$ hold when at least one of the classification criteria below applies to the violation in $p$. Define:

$$\text{IFNDR}(p) \iff \text{IllFormed}(p) \land \text{DetectionInfeasible}(p)$$

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

The observable effects of executing an IFNDR program are Unspecified Behavior (USB). The Safety Invariant (§1.2) still applies: when the IFNDR condition occurs in safe code, the observable effects MUST NOT exhibit Unverifiable Behavior (UVB).

**Formal Statement**

$$\text{IFNDR}(p) \land \text{Safe}(p) \implies \text{Effects}(p) \subseteq \mathcal{B}_{\text{USB}}$$

Permitted outcomes include:

- Compilation failure (if detected)
- Deterministic panic at runtime
- Non-deterministic but safe execution

Prohibited outcomes include:

- Memory corruption
- Data races
- Arbitrary code execution

**Tracking and Reporting**

IFNDR instances MUST be recorded in the Conformance Dossier per §1.1. See the Instance Data table below for required fields.

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

The identifiers listed in the keyword table (Clause 2, §2.6) are reserved keywords. They MUST NOT be used as user-defined identifiers.

**Reserved Namespaces**

The `cursive.*` namespace prefix is reserved for specification-defined features. User programs and vendor extensions MUST NOT use this namespace.

**Implementation Reservations**

Implementations MAY reserve additional identifier patterns (e.g., identifiers beginning with `__` or `_[A-Z]`). Such reservations:

1. MUST be documented in the implementation's Conformance Dossier.
2. SHOULD follow common conventions to avoid collision with user code.

Conforming programs SHOULD NOT use patterns that are commonly reserved by implementations.

**Universe-Protected Bindings**

**A Universe-Protected Binding is an identifier that is pre-declared in the implicit universal scope and MUST NOT be shadowed by user declarations at module scope.**

The following identifiers are universe-protected:

1. **Primitive type names:** `i8`, `i16`, `i32`, `i64`, `i128`, `u8`, `u16`, `u32`, `u64`, `u128`, `f16`, `f32`, `f64`, `bool`, `char`, `usize`, `isize`

2. **Special types:** `Self`, `string`, `Modal`

Attempting to shadow a universe-protected binding at module scope MUST be diagnosed as an error.

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

Conforming implementations MUST support at least the minimum limits specified in Appendix I. The complete limit table with units and descriptions is normatively defined in Appendix I.

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

### Clause 1 Cross-Reference Notes `[INFORMATIVE]`

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

**Terms Deferred to Later Clauses:**

| Term                  | Deferred To | Reason                                      |
| :-------------------- | :---------- | :------------------------------------------ |
| Lexical constraints   | Clause 2    | Detailed token and identifier rules         |
| Syntactic constraints | Clause 2    | Grammar and parse rules                     |
| Type Layout           | Clause 5    | Size, alignment, representation details     |
| Permission System     | Clause 4    | `unique`, `const`, `shared` rules           |
| Module System         | Clause 9    | Visibility, imports, compilation units      |
| `unsafe` semantics    | Clause 3    | Full rules for unsafe blocks and operations |
| `comptime` semantics  | Clause 10   | Compile-time evaluation rules               |
| FFI                   | Clause 12   | Foreign function interface rules            |

---

## Clause 2: Lexical Structure and Source Text

This clause defines the lexical structure of Cursive: how source text is encoded, preprocessed, and transformed into a sequence of tokens.

---

### 2.1 Source Text Encoding

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

$$\mathcal{F} = \{c : \text{GeneralCategory}(c) = \texttt{Cc}\} \setminus \{\text{U+0009}, \text{U+000A}, \text{U+000C}, \text{U+000D}\}$$

> **Note:** U+0000 (NULL) is included in the Cc (Control) general category and is therefore prohibited.

A source file containing a prohibited code point outside of a literal MUST be rejected with diagnostic `E-SRC-0104`.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0101` | Error    | Invalid UTF-8 byte sequence.                 | Compile-time | Rejection |
| `E-SRC-0103` | Error    | Embedded BOM found after the first position. | Compile-time | Rejection |
| `E-SRC-0104` | Error    | Forbidden control character or null byte.    | Compile-time | Rejection |
| `W-SRC-0101` | Warning  | UTF-8 BOM present at the start of the file.  | Compile-time | N/A       |

---

### 2.2 Source File Structure

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

### 2.3 Preprocessing Pipeline

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

### 2.4 Lexical Vocabulary

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

> **Note:** Newlines are tokens (not discarded whitespace) to support optional statement termination.

Tokenization MUST be deterministic for a given normalized source file.

##### Constraints & Legality

Any maximal character sequence that cannot be classified MUST trigger diagnostic `E-SRC-0309`.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0309` | Error    | Unclassifiable character sequence in source. | Compile-time | Rejection |

---

### 2.5 Identifiers

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

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-CNF-0401` | Error    | Reserved keyword used as identifier (see §1.3). | Compile-time | Rejection |
| `E-SRC-0307` | Error    | Invalid Unicode in identifier.                  | Compile-time | Rejection |

---

### 2.6 Keywords

##### Definition

A **Keyword** is a reserved lexeme that has special meaning in the language grammar and MUST NOT be used where an identifier is expected.

**Formal Definition**

Let $\mathcal{K}$ denote the set of reserved keywords. The complete enumeration is provided in the Static Semantics block below.

##### Static Semantics

The reserved keywords are:

```
and         as          async       atomic      break       comptime
shared      const       continue    defer       dispatch    do
drop        else        emit        enum        escape      extern
false       for         gpu         if          import
in          interrupt   let         loop        match       modal
mod         module      move        mut         override    pool
private     procedure   protected   public      quote       record
region      result      return      select      self        Self
set         simd        spawn       sync        then        form
transition  transmute   true        type        union       unique
unsafe      using       var         volatile    where       while
witness
```

Implementations MUST tokenize these as `<keyword>`, not `<identifier>`. The keyword set MUST be identical across conforming implementations for a given language version.

> **Note:** The keyword `mut` is used as a qualifier in raw pointer type syntax: `*mut T` (mutable raw pointer to `T`). See §4.13 for the complete grammar and semantics of pointer types.

**Contextual Keywords**

Cursive does not employ contextual keywords. All keywords listed above are **unconditionally reserved** in all syntactic contexts. An identifier matching a reserved keyword MUST be tokenized as `<keyword>`, never as `<identifier>`, regardless of the surrounding syntactic context.

> **Note:** Cursive has no contextual keywords. This simplifies tooling and AI-assisted code generation.

##### Constraints & Legality

Using a reserved keyword where an identifier is expected MUST trigger diagnostic `E-CNF-0401` (§1.3).

---

### 2.7 Operators and Punctuators

##### Definition

An **Operator** is a symbol or symbol sequence denoting an operation. A **Punctuator** is a symbol or symbol sequence used for syntactic structure.

**Formal Definition**

$$\mathcal{O}_\text{multi} = \{\texttt{==}, \texttt{!=}, \texttt{<=}, \texttt{>=}, \texttt{\&\&}, \texttt{||}, \texttt{<<}, \texttt{>>}, \texttt{..}, \texttt{..=}, \texttt{=>}, \texttt{->}, \texttt{**}, \texttt{::}, \texttt{:=}, \texttt{|=}, \texttt{\~>}, \texttt{\~!}, \texttt{\~%}\}$$

$$\mathcal{O}_\text{single} = \{+, -, *, /, \%, <, >, =, !, \&, |, \hat{}, \sim, .\}$$

$$\mathcal{P} = \{(, ), [, ], \{, \}, ,, :, ;\}$$

##### Syntax & Declaration

**Multi-Character Tokens**

| Category          | Tokens                   |
| :---------------- | :----------------------- |
| Comparison        | `==`, `!=`, `<=`, `>=`   |
| Logical           | `&&`, `\|\|`             |
| Bitwise Shift     | `<<`, `>>`, `<<=`, `>>=` |
| Range             | `..`, `..=`              |
| Arrow             | `=>`, `->`               |
| Binding           | `:=`                     |
| Contract          | `\|=`                    |
| Other             | `**`, `::`               |
| Receiver/Dispatch | `~`, `~>`, `~!`, `~%`    |

**Single-Character Tokens**

```
+  -  *  /  %  <  >  =  !  &  |  ^  ~  .  ,  :  ;  (  )  [  ]  {  }
```

**Receiver Token Lexemes**

| Token | Lexical Role                             |
| :---- | :--------------------------------------- |
| `~`   | Receiver reference in method context     |
| `~>`  | Lens operator / method dispatch operator |
| `~!`  | Permission-qualified receiver (unique)   |
| `~%`  | Permission-qualified receiver (shared)   |

**Region Operators**

| Token | Lexical Role                                  |
| :---- | :-------------------------------------------- |
| `r^`  | Region-prefixed allocation (e.g., `r^data()`) |
| `^`   | Implicit region allocation (innermost named)  |

> **Note:** Full semantic definitions are in §4.5.6. See Appendix F.4 for design rationale.

**Binding Operator Semantics**

| Token | Semantic Meaning                                                       |
| :---- | :--------------------------------------------------------------------- |
| `=`   | Establishes movable responsibility (ownership may transfer via `move`) |
| `:=`  | Establishes immovable responsibility (ownership cannot transfer)       |

**Contract Operator Semantics**

| Token | Semantic Meaning                                                  |
| :---- | :---------------------------------------------------------------- |
| `\|=` | Introduces procedure contract clause (semantic entailment symbol) |

##### Static Semantics

**Maximal Munch Rule**

When multiple tokenizations are possible, the lexer MUST emit the longest valid token.

**Generic Argument Exception**

When parsing generic type arguments, the implementation MAY split `>>` into two `>` tokens while preserving the original lexeme for diagnostics.

---

### 2.8 Literals

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

### 2.9 Whitespace and Comments

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

Comments beginning with `///` or `//!` are documentation comments. Implementations MUST preserve documentation comments and associate them with their following declaration (`///`) or enclosing module (`//!`). Documentation comments MUST NOT appear in the token stream.

> **Note:** Documentation extraction and rendering are implementation-defined. See Appendix F.5 for recommended documentation formats.

**Block Comments**

A block comment is delimited by `/*` and `*/`. Block comments MUST nest: each `/*` increases nesting depth, each `*/` decreases it. End-of-file with non-zero nesting depth MUST trigger `E-SRC-0306`.

Characters within comments MUST NOT contribute to token formation, delimiter nesting, or statement termination.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------- | :----------- | :-------- |
| `E-SRC-0306` | Error    | Unterminated block comment. | Compile-time | Rejection |

---

### 2.10 Lexical Security

##### Definition

**Lexically Sensitive Characters** are Unicode code points that may alter the visual appearance of source code without changing its tokenization.

**Formal Definition**

$$\mathcal{S} = \{\text{U+202A–U+202E}\} \cup \{\text{U+2066–U+2069}\} \cup \{\text{U+200C}, \text{U+200D}\}$$

##### Static Semantics

**Sensitive Character Categories**

The following Unicode categories contain lexically sensitive characters:

| Category              | Unicode Property       | Risk                              |
| :-------------------- | :--------------------- | :-------------------------------- |
| Bidirectional Control | Bidi_Control           | Text reordering attacks           |
| Zero-Width Characters | General_Category=Cf    | Invisible identifier differences  |
| Homoglyphs            | Confusable mappings    | Visual spoofing of identifiers    |
| Combining Characters  | General_Category=Mn/Mc | Stacking attacks on rendered text |

**Enforcement Modes**

When a character in $\mathcal{S}$ appears unescaped in an identifier, operator/punctuator, or adjacent to token boundaries outside literals and comments, the implementation MUST emit a diagnostic. Severity depends on conformance mode:

| Mode       | Behavior                                  | Diagnostic |
| :--------- | :---------------------------------------- | :--------- |
| Permissive | Issue warning `W-SRC-0308`; accept source | Warning    |
| Strict     | Issue error `E-SRC-0308`; reject source   | Error      |

The default mode is implementation-defined (IDB). Implementations MUST document their default in the Conformance Dossier and MUST provide a mechanism to select either mode.

Lexically sensitive characters within string or character literals MUST NOT affect well-formedness.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `W-SRC-0308` | Warning  | Lexically sensitive Unicode character (Permissive). | Compile-time | N/A       |
| `E-SRC-0308` | Error    | Lexically sensitive Unicode character (Strict).     | Compile-time | Rejection |

---

### 2.11 Statement Termination

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

**Ambiguity Resolution for Operators**

When a token sequence could be parsed as either a trailing binary operator (triggering continuation) or a leading unary operator on the next line, the following disambiguation rule applies:

If the last non-whitespace, non-comment token on line $L$ is in the set $\{+, -, *, \&, |\}$ (tokens that may be either binary or unary), and the first non-whitespace, non-comment token on the following line $L+1$ is an operand (identifier, literal, or opening delimiter), then:

1. The token on line $L$ is treated as a **binary operator** (continuation).
2. Line $L$ does NOT terminate.

To force statement termination before a unary operator, insert an explicit semicolon.

**Example — Ambiguous minus:**

```cursive
let a = b
-c              // Parsed as: let a = b - c (continuation)

let a = b;
-c              // Parsed as: let a = b; followed by expression -c
```

---

### 2.12 Translation Phases

##### Definition

A **Translation Phase** is a discrete stage in the compilation pipeline. A **Compilation Unit** is the collection of all normalized source files that contribute to a single module.

**Formal Definition**

$$\text{Translate} : \text{CompilationUnit} \to \text{Executable} \cup \{\bot\}$$

The translation pipeline is the sequential composition of four phases, where $\bot$ indicates failure:

$$\text{Translate} = \text{Parse} \triangleright \text{ComptimeExec} \triangleright \text{TypeCheck} \triangleright \text{CodeGen}$$

where $f \triangleright g$ denotes "apply $f$, then apply $g$" (left-to-right sequencing).

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

### 2.13 Syntactic Nesting Limits

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

### Clause 2 Cross-Reference Notes `[INFORMATIVE]`

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

**Terms deferred to later clauses:**

| Term               | Deferred To    | Reason                     |
| :----------------- | :------------- | :------------------------- |
| Module             | Clause 6       | Requires module system     |
| `comptime` Block   | Clause 8       | Requires metaprogramming   |
| `self`, `Self`     | Clause 4       | Requires type definitions  |
| `unique`, `shared` | Clause 4, §4.5 | Requires permission system |

---

## Clause 3: The Object and Memory Model

This clause defines the Cursive memory model: how objects are represented, how their lifetimes are tracked, and how memory safety is enforced.

---

### 3.1 Foundational Principles

##### Definition

**Memory Safety Axes**

Cursive's memory safety is decomposed into two orthogonal concerns:

1.  **Liveness**: The property that all pointers and bindings refer to allocated, initialized memory.

2.  **Aliasing**: The property that shared or reentrant access to the same memory location does not violate data integrity.

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

Aliasing is enforced through permission types (§4.5) and the key-based concurrency model (§13).

##### Constraints & Legality

**The Explicit-Over-Implicit Principle**

Memory operations affecting ownership, allocation, or synchronization MUST be syntactically explicit in source code:

| Operation               | Required Syntax                                             |
| :---------------------- | :---------------------------------------------------------- |
| Region Allocation       | The `^` operator                                            |
| Heap Allocation         | Explicit allocator method call with `witness HeapAllocator` |
| Responsibility Transfer | The `move` keyword at call sites for consuming parameters   |

**The Zero Runtime Overhead Principle**

All memory safety checks, except for dynamic array/slice bounds checks and explicitly requested dynamic contract verification (`[[verify(dynamic)]]`), MUST be resolved at compile time.

Conforming implementations MUST NOT insert reference counting, garbage collection barriers, dynamic lifetime tracking metadata, or hidden synchronization primitives in code paths that do not explicitly request such mechanisms.

---

#### 3.1.1 Aliasing Enforcement

##### Definition

The **Exclusion Principle** governs which permission-qualified paths may coexist for the same object at any given program point. A **path** is an expression denoting a location in memory (a binding, field access chain, or pointer dereference).

##### Static Semantics

**Coexistence Rules**

The complete permission system—including the coexistence matrix, subtyping lattice, and method receiver compatibility—is defined normatively in **§4.5 (Permission Types)**.

**Summary:**

| Permission | Permits Aliasing? | Permits Mutation?  | Coexistence                 | Key Required? |
| :--------- | :---------------- | :----------------- | :-------------------------- | :------------ |
| `const`    | Yes               | No                 | Any number of `const` paths | No            |
| `unique`   | No                | Yes                | Excludes all other paths    | No            |
| `shared`   | Yes               | Yes (synchronized) | With `shared` or `const`    | Yes           |

> **Note:** The `shared` permission uses a key-based access model where keys are
> implicitly acquired on access and released after use. Disjoint paths to `shared`
> data may be accessed in parallel without contention. See §13 (Concurrency) for
> the complete key system specification.

See §4.5 for formal definitions, the coexistence matrix, subtyping rules, and method receiver compatibility.

> **Canonical Diagnostics:** Type system diagnostics (`E-TYP-16xx`) in §4.5 are authoritative for permission violations. Memory model diagnostics (`E-MEM-30xx`) in this section are secondary per Appendix K.

**Temporal Permission Transitions**

The coexistence matrix above specifies which permissions may **simultaneously** exist for paths to the same object. This section clarifies the rules for **sequential** permission changes.

**Transition Rule:** When all paths of a given permission to an object cease to exist (go out of scope, are moved, or are explicitly released), the object returns to a state where any permission may be established, subject to the coexistence rules.

**Sequencing Examples:**

| Initial State        | Action                    | Permitted Next State                               |
| :------------------- | :------------------------ | :------------------------------------------------- |
| `unique` path active | Path goes out of scope    | Any permission                                     |
| `const` paths active | All paths go out of scope | Any permission                                     |
| `shared` path active | Path goes out of scope    | Any permission (if no other `shared` paths remain) |

**Non-Overlapping Temporal Windows:**

The following sequence is **well-formed**:

```cursive
let data: unique Buffer = Buffer::new()
{
    let view: const Buffer = data    // 'data' inactive; 'view' is const
}                                     // 'view' ends; 'data' reactivates as unique
data.mutate()                         // OK: 'data' is unique again
```

The coexistence rules apply to paths that exist **at the same program point**. Paths in disjoint temporal scopes do not conflict.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3040` | Error    | `unique` path created while other paths are active. | Compile-time | Rejection |

> **Note:** Per Appendix K, type system diagnostics (`E-TYP-`) are canonical; memory model diagnostics (`E-MEM-`) are secondary. Implementations MUST emit the canonical diagnostic and MAY additionally emit secondary diagnostics when they provide distinct information.

---

### 3.2 The Object Model

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
3.  **Destruction and Deallocation**: If the object's type implements `Drop` (§9.7), its destructor is invoked. Storage is then released.

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

### 3.3 The Provenance Model

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

The distinction between strict ordering ($<$) and non-strict ordering ($\le$) is significant:

- **Strict ordering ($<$):** The left provenance is *guaranteed* to be deallocated *before* the right provenance. Values with the shorter-lived provenance MUST NOT escape to locations with longer-lived provenance.

- **Non-strict ordering ($\le$):** The provenances may have equivalent lifetimes. Specifically, $\pi_{\text{Heap}} \le \pi_{\text{Global}}$ because heap-allocated storage with static lifetime (e.g., intentional memory leaks, process-lifetime allocations) may persist until program termination, matching global storage duration.

> **Note:** The ordering $\pi_{\text{Heap}} \le \pi_{\text{Global}}$ rather than $\pi_{\text{Heap}} < \pi_{\text{Global}}$ permits heap pointers to be stored in global variables without violating the escape rule.

**Constraint:** Heap-allocated storage whose address has been stored in a binding with $\pi_{\text{Global}}$ provenance MUST NOT be deallocated during program execution. Deallocating such storage constitutes Unverifiable Behavior (UVB).

**⚠️ Critical Implementation Note**

This constraint is **not statically enforceable** in the general case because:
1. The decision to store a heap pointer in a global binding may occur at runtime.
2. Tracking all possible aliases to heap storage through global bindings is undecidable.

**Enforcement Model:**

| Code Context  | Enforcement Mechanism                 | Responsibility                |
| :------------ | :------------------------------------ | :---------------------------- |
| Safe code     | Safe allocator API design (see below) | Standard library implementers |
| `unsafe` code | Programmer assertion                  | Application developer         |

**Standard Library Requirement:** Allocators in the standard library that return heap pointers MUST be designed such that:
- Either the returned pointer cannot be stored in global bindings (enforced by provenance typing), OR
- The allocator does not support explicit deallocation (arena/leaked allocation pattern).

This requirement is **normative for standard library implementations**. Third-party allocators operating in safe code MUST maintain equivalent guarantees.

Violation in `unsafe` code constitutes UVB (§1.2).

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

### 3.4 The Binding Model

##### Definition

A **Binding** is a named association between an identifier and an object. Bindings are introduced by `let` and `var` declarations, pattern matching, and procedure parameters.

A **Responsible Binding** is a binding that manages the lifecycle of its associated object. When a responsible binding goes out of scope, it triggers destruction of the object (§3.6).

A **Movable Binding** is a binding established with the `=` operator. Responsibility for the bound object MAY be transferred via `move`.

An **Immovable Binding** is a binding established with the `:=` operator. Responsibility for the bound object CANNOT be transferred. Attempting to `move` from an immovable binding is a compile-time error.

**Binding State**

$$\text{BindingState} ::= \text{Uninitialized} \mid \text{Valid} \mid \text{Moved} \mid \text{PartiallyMoved}$$

A **Temporary Value** is an object resulting from expression evaluation that is not immediately bound to a named identifier.

##### Syntax & Declaration

```ebnf
<binding_decl>     ::= <let_decl> | <var_decl>
<binding_op>       ::= "=" | ":="
<let_decl>         ::= "let" <pattern> (":" <type>)? <binding_op> <expression>
<var_decl>         ::= "var" <pattern> (":" <type>)? <binding_op> <expression>
```

##### Static Semantics

**Binding Mutability**

`let` establishes an immutable binding; `var` establishes a mutable binding. Binding mutability is orthogonal to data permission (§4.5) and responsibility transferability.

**Binding Movability**

The binding operator determines whether responsibility may be transferred:

| Declaration  | Reassignable | Movable | Responsibility                      |
| :----------- | :----------- | :------ | :---------------------------------- |
| `let x = v`  | No           | Yes     | Transferable via `move`             |
| `let x := v` | No           | No      | Permanently fixed to `x`            |
| `var x = v`  | Yes          | Yes     | Transferable via `move`             |
| `var x := v` | Yes          | No      | Fixed; reassignment drops old value |

For `var x := v`: reassignment is permitted but `move` is not. Each reassignment drops the previous value and fixes the new value's responsibility to `x`.

**Binding Annotation Notation**

The typing rules below use a decorated type notation to track binding state and properties:

$$T^{\text{State}}_{\text{properties}}$$

where:

- **Superscript** ($^{\text{State}}$) denotes the **binding state** from the BindingState enumeration defined above:
  - $^{\text{Valid}}$: The binding holds an initialized, accessible value.
  - $^{\text{Moved}}$: The binding's value has been moved; access is statically prohibited.
  - $^{\text{PartiallyMoved}}$: Some fields have been moved; the binding is partially accessible.

- **Subscript** ($_{\text{properties}}$) denotes **binding properties** as a comma-separated list:
  - $_{\text{mov}}$: The binding is movable (established with `=`).
  - $_{\text{immov}}$: The binding is immovable (established with `:=`).
  - $_{\text{mut}}$: The binding is mutable (declared with `var`).

For example, $T^{\text{Valid}}_{\text{mut,mov}}$ denotes a type $T$ with a Valid state, mutable binding, and movable responsibility.

**Typing Rules**

**(T-Let-Movable)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{let } x = e : () \dashv \Gamma, x : T^{\text{Valid}}_{\text{mov}}}$$

**(T-Let-Immovable)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{let } x := e : () \dashv \Gamma, x : T^{\text{Valid}}_{\text{immov}}}$$

**(T-Var-Movable)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{var } x = e : () \dashv \Gamma, x : T^{\text{Valid}}_{\text{mut,mov}}}$$

**(T-Var-Immovable)**
$$\frac{\Gamma \vdash e : T}{\Gamma, x : T \vdash \texttt{var } x := e : () \dashv \Gamma, x : T^{\text{Valid}}_{\text{mut,immov}}}$$

**State Tracking**

The compiler MUST track binding state through control flow. A binding's state is determined by its declaration, any `move` expressions consuming the binding, partial moves consuming fields, and reassignment of `var` bindings.

**Responsibility and Move Semantics**

Both binding operators establish cleanup responsibility; the mechanics of responsibility transfer via `move` are defined in §3.5. The key distinction:

- **`=` (movable)**: Responsibility may be transferred per §3.5.
- **`:=` (immovable)**: Responsibility is permanently fixed; `move` is prohibited.

**Temporary Lifetime**

Temporaries have a lifetime extending from their creation to the end of the innermost enclosing statement. If a temporary is used to initialize a `let` or `var` binding, its lifetime is promoted to match the binding's scope. Temporaries that are not moved or bound MUST be destroyed at statement end, in reverse order of creation.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-MEM-3002` | Error    | Access to binding in Uninitialized state. | Compile-time | Rejection |
| `E-MEM-3003` | Error    | Reassignment of immutable binding.        | Compile-time | Rejection |

---

### 3.5 Responsibility and Move Semantics

> **Cross-Reference:** This section defines the mechanics of ownership transfer. The distinction between movable and immovable bindings is established in §3.4.

##### Definition

A **Move** is an operation that transfers responsibility for an object from one binding to another. After a move, the source binding is invalidated and the destination binding becomes responsible for the object's lifecycle.

A move operation is only permitted from **movable bindings** (§3.4). Attempting to move from an immovable binding MUST be rejected with diagnostic `E-MEM-3006`.

**Formal Definition**

$$\text{move} : (\Gamma, x : T^{\text{Valid}}_{\text{mov}}) \to (\Gamma[x \mapsto T^{\text{Moved}}], T)$$

The move operation takes a context $\Gamma$ containing a valid, movable binding $x$ of type $T$, and produces:
1. An updated context where $x$ is in the Moved state
2. The value of type $T$ (now available for binding elsewhere)

The subscript $_{\text{mov}}$ indicates the binding must be movable (established with `=`, not `:=`).

##### Syntax & Declaration

```ebnf
<move_expr>        ::= "move" <place_expr>
<partial_move>     ::= "move" <place_expr> "." <identifier>
```

##### Static Semantics

**Typing Rules**

**(T-Move)**
$$\frac{\Gamma \vdash x : T \quad \text{state}(\Gamma, x) = \text{Valid} \quad \text{movable}(\Gamma, x)}{\Gamma \vdash \texttt{move } x : T \dashv \Gamma[x \mapsto \text{Moved}]}$$

Where $\text{movable}(\Gamma, x)$ holds iff $x$ was bound with `=`, not `:=`.

**(T-Move-Field)**
$$\frac{\Gamma \vdash x : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \} \quad \text{perm}(\Gamma, x) = \text{unique}}{\Gamma \vdash \texttt{move } x.f : T \dashv \Gamma[x \mapsto \text{PartiallyMoved}(f)]}$$

**Move Semantics**

`move x` transitions the source binding `x` to the Moved state. Access to a Moved binding is forbidden. A `var` binding in Moved state MAY be reassigned, restoring it to Valid state. A `let` binding in Moved state MUST NOT be reassigned.

**Partial Move Constraints**

Partial moves are permitted ONLY when both of the following conditions are satisfied:

1. The binding is **movable** (established with `=`, not `:=`). Immovable bindings prohibit all `move` operations, including partial moves.
2. The binding has **`unique` permission**, providing exclusive access to the record's fields.

After a partial move, the parent binding enters the PartiallyMoved state. If the binding is a mutable `var`, reassignment may restore it to Valid state (§3.4). A `let` binding in PartiallyMoved state cannot be restored and remains invalid for the remainder of its scope.

**Parameter Responsibility**

A **Transferring Parameter** is declared with the `move` modifier. The callee assumes responsibility; the caller MUST apply `move` at the call site; the source binding is invalidated after the call.

A **Non-Transferring Parameter** is declared without `move`. The callee receives a view; responsibility remains with the caller; the source binding remains valid.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3001` | Error    | Access to a binding in Moved or PartiallyMoved state.        | Compile-time | Rejection |
| `E-MEM-3004` | Error    | Partial move from `const` binding.                           | Compile-time | Rejection |
| `E-MEM-3006` | Error    | Attempt to move from immovable binding (`:=`).               | Compile-time | Rejection |
| `E-DEC-2411` | Error    | Missing `move` keyword at call site for consuming parameter. | Compile-time | Rejection |

---

### 3.6 Deterministic Destruction

##### Definition

**RAII (Resource Acquisition Is Initialization)** is the pattern where resources are acquired during object initialization and released during object destruction. Destruction in Cursive is deterministic—it occurs at a statically known program point.

**Drop Order** specifies that bindings MUST be destroyed in strict LIFO order relative to their declaration within a scope.

##### Dynamic Semantics

**Normal Destruction**

At scope exit, the compiler MUST generate code to destroy all responsible bindings in reverse declaration order:

1.  If the binding's type implements `Drop` (§9.7), invoke `Drop::drop`.
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

---

### 3.7 Regions

##### Definition

A **Region** is a lexical scope that provides arena-style memory allocation with
deterministic bulk deallocation. Data allocated in a region lives until the region
exits.

**Region** provides **memory lifetime** management:
- Arena-style bulk allocation with deterministic deallocation
- LIFO lifetime ordering relative to nested regions

Task execution is managed separately by the `parallel` block (§13.3). While regions
and parallel blocks are orthogonal, they compose naturally: region-allocated data
can be shared across tasks within a `parallel` block because structured concurrency
guarantees all tasks complete before the enclosing scope (and thus any enclosing
region) exits.

##### Syntax & Declaration

**Grammar**

```ebnf
<region_stmt>      ::= "region" <region_options>? <region_alias>? <block>

<region_options>   ::= <region_option>*
<region_option>    ::= "." "stack_size" "(" <expr> ")"
                     | "." "name" "(" <expr> ")"

<region_alias>     ::= "as" <identifier>

<alloc_expr>       ::= <identifier>? "^" <expression>
<named_alloc>      ::= <identifier> "^" <expression>
```

> **Note:** Execution modifiers (`pool`, `async`, `simd`, `gpu`, `interrupt`) are
> specified on `parallel` blocks, not regions. See §13.3 for the `parallel` block
> specification.

**Region Allocation**

The `^` operator allocates storage in a region:
- `^expr` allocates in the innermost enclosing named region
- `r^expr` allocates in the explicitly named region `r`

##### Static Semantics

**Typing Rules**

**(T-Region-Seq)**
$$\frac{\Gamma, r : \texttt{Region@Active} \vdash \textit{body} : T}{\Gamma \vdash \texttt{region as } r \{ \textit{body} \} : T}$$

**(T-Alloc-Named)**
$$\frac{\Gamma(r) = \texttt{Region@Active} \quad \Gamma \vdash e : T}{\Gamma \vdash r\texttt{\^{}}e : T^{\pi_r}}$$

**(T-Alloc-Implicit)**
$$\frac{\text{InnermostNamedRegion}(\Gamma) = r \quad \Gamma \vdash e : T}{\Gamma \vdash \texttt{\^{}}e : T^{\pi_r}}$$

**Named Region Requirement**

The unprefixed `^` operator requires a named region in lexical scope:
- `^expr` allocates in the innermost enclosing named region
- `r^expr` allocates in the explicitly named region `r`
- Using `^` with no named region in scope is error `E-REG-1205`

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      |
| ------------ | -------- | ---------------------------------------------- |
| `E-REG-1205` | Error    | `^` with no named region in scope              |
| `E-REG-1206` | Error    | Named region not found for prefixed allocation |

> **Note:** Task-related diagnostics (`E-SPAWN-xxxx`, `E-ASYNC-xxxx`) are defined
> in §13 (Concurrency).

---

### 3.8 Unsafe Memory Operations

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

Within an `unsafe` block, the compiler SHALL NOT enforce liveness, aliasing exclusivity, or key disjointness.

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

---

### Clause 3 Cross-Reference Notes `[INFORMATIVE]`

**Terms defined in Clause 3 that MUST NOT be redefined elsewhere:**

| Term                   | Section | Description                                               |
| :--------------------- | :------ | :-------------------------------------------------------- |
| Exclusion Principle    | §3.1.1  | Rules governing permission-qualified path coexistence     |
| Path                   | §3.1.1  | Expression denoting a memory location                     |
| Object                 | §3.2    | Fundamental unit of typed storage                         |
| Storage Duration       | §3.2    | Classification of object lifetime                         |
| Provenance Tag         | §3.3    | Compile-time annotation indicating address origin         |
| Lifetime Partial Order | §3.3    | Ordering relation on provenance tags                      |
| Escape Rule            | §3.3    | Constraint preventing assignment across provenance bounds |
| Binding                | §3.4    | Named association between identifier and object           |
| Responsible Binding    | §3.4    | Binding managing lifecycle of its object                  |
| Movable Binding        | §3.4    | Binding established with `=`; responsibility transferable |
| Immovable Binding      | §3.4    | Binding established with `:=`; responsibility fixed       |
| Define-As Operator     | §3.4    | The `:=` operator establishing immovable responsibility   |
| Binding State          | §3.4    | Valid, Moved, PartiallyMoved, or Uninitialized            |
| Temporary Value        | §3.4    | Unnamed intermediate expression result                    |
| Move                   | §3.5    | Operation transferring responsibility between bindings    |
| Transferring Parameter | §3.5    | Parameter declared with `move` modifier                   |
| RAII                   | §3.6    | Resource Acquisition Is Initialization pattern            |
| Drop Order             | §3.6    | LIFO destruction order                                    |
| Region                 | §3.7    | Lexical scope with associated Arena                       |
| Key                    | §13.2   | Runtime proof of access rights to a shared path           |
| Key Mode               | §13.2   | Read or Write access grant                                |
| Key Coarsening         | §13.2   | The `#` annotation for explicit key granularity control   |
| Arena                  | §3.9    | Modal type managing region memory lifecycle               |
| Unsafe Block           | §3.10   | Lexical scope with suspended safety enforcement           |
| `transmute`            | §3.10   | Bitwise reinterpretation intrinsic                        |

**Terms referenced from other clauses:**

| Term             | Source | Usage in Clause 3                          |
| :--------------- | :----- | :----------------------------------------- |
| Permission Types | §4.5   | Define type annotations for aliasing rules |
| Modal Types      | §4.8   | Arena is a modal type                      |
| Pointer Types    | §4.13  | Raw pointers defined there; semantics here |
| Drop Form        | §9.7   | Destructor mechanism                       |
| Place Expression | §8.1   | Required for `move` operand                |
| HeapAllocator    | §9.3   | Capability for dynamic allocation          |

**Terms deferred to later clauses:**

| Term    | Deferred To | Reason                        |
| :------ | :---------- | :---------------------------- |
| Panic   | Clause 10   | Error handling semantics      |
| Abort   | Clause 10   | Process termination semantics |
| FFI     | Clause 12   | Foreign function interface    |
| `defer` | Clause 8    | Deferred execution statement  |

---

# Part 2: Types

## Clause 4: Type System Foundations

This clause defines the foundational machinery of the Cursive type system: the abstract framework governing type equivalence, subtyping, variance, inference, and the permission system. These concepts underpin all concrete type definitions in subsequent clauses.

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

Permissions are part of the type. `unique T` is not equivalent to `const T`. See §4.5 for the complete permission system specification.

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

> **Note:** The `string` type (§4.12) is a built-in Modal type with states `@Managed`, `@View`, and `@Static`. It belongs to the Modal category but is documented separately due to its special syntax and standard library integration.

##### Constraints & Legality

Top-level declarations (procedures, module-scope bindings, type definitions) MUST include explicit type annotations. Local bindings within procedure bodies MAY omit type annotations when the type is inferable (§4.4).

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1501` | Error    | Type mismatch in expression or assignment.        | Compile-time | Rejection |
| `E-TYP-1502` | Error    | No valid type derivable for expression.           | Compile-time | Rejection |
| `E-TYP-1503` | Error    | Operation not supported for operand type.         | Compile-time | Rejection |
| `E-TYP-1505` | Error    | Missing required type annotation at module scope. | Compile-time | Rejection |

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

Permission types form a linear subtype lattice. The complete lattice structure, formal subtyping rules, and the prohibition on implicit upgrades are all defined in §4.5.

**Form Implementation Subtyping**

A concrete type that implements a form is a subtype of that form. See §6.3 for the formal rule (Sub-Form).

**Additional Subtyping Rules**

The following subtyping relationships are defined in their respective sections:

- Modal widening ($M@S <: M$, where $M$ is the general modal type): §6.1 (includes storage cost documentation)
- Union member subtyping: §4.10
- Refinement type subtyping: §4.19
- Permission subtyping: §4.5

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

This rule applies because `const` prohibits mutation, eliminating the safety concern that motivates invariance. The rule does not apply to `unique` or `shared` permissions:

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

When type arguments cannot be inferred, explicit type arguments may be supplied using turbofish syntax (`::<>`). See §7.1 for the full specification.

---

### 4.5 Permission Types

This section defines the permission system that governs data access, mutation, and aliasing.

---

#### 4.5.0 Permission System Overview

The permission system governs data access through three complementary mechanisms:

1. **Coexistence Rules (§4.5.4):** Specify which permission-qualified paths may exist *simultaneously* for the same object. These rules prevent aliasing violations at any program point.

2. **Subtyping Rules (§4.5.5):** Specify which permissions may be *implicitly converted* to other permissions. These rules govern type compatibility in assignment, argument passing, and return contexts.

3. **Receiver Compatibility (§4.5.6):** Specify which methods are callable through a path of a given permission. These rules ensure that method calls respect permission constraints.

**Quick Reference:**

| Question                                              | Answer | Rule        | Section |
| :---------------------------------------------------- | :----- | :---------- | :------ |
| Can I have both a `const` and `unique` path to X?     | No     | Coexistence | §4.5.4  |
| Can I pass a `unique T` where `const T` is expected?  | Yes    | Subtyping   | §4.5.5  |
| Can I convert `const T` to `unique T`?                | No     | Subtyping   | §4.5.5  |
| Can I call a `~!` method through a `const` path?      | No     | Receiver    | §4.5.6  |
| Can I call a `~` method through a `unique` path?      | Yes    | Receiver    | §4.5.6  |
| Can multiple `shared` paths coexist?                  | Yes    | Coexistence | §4.5.4  |
| Does accessing `shared` data require synchronization? | Yes    | Key System  | §13.2   |
| Can I directly mutate fields through a `shared` path? | No*    | Key System  | §13.2   |

> *Direct field mutation through `shared` requires implicit key acquisition. The
> compiler automatically inserts key operations; field access syntax is unchanged.
> See §13.2 for key semantics.

**Permission Lattice (Visual Summary):**

```
    unique        (exclusive read-write)
       |
  shared      (synchronized shared mutable)
       |
     const        (shared read-only)
```

All formal rules are defined in the subsections below. §3.1.1 provides memory model context.

---

#### 4.5.1 Permission Definitions

##### Definition

A **permission** is a type qualifier that governs how the data referenced by a binding may be accessed, mutated, and aliased. Permissions are fundamental to the language's memory safety guarantees, providing compile-time enforcement of aliasing rules without runtime overhead. When no permission is explicitly specified, `const` is the default.

**Formal Definition**

Let $\mathcal{P}$ denote the set of permissions:

$$\mathcal{P} = \{\texttt{const}, \texttt{unique}, \texttt{shared}\}$$

A **permission-qualified type** is a pair $(P, T)$ where $P \in \mathcal{P}$ and $T$ is a base type. The notation $P\ T$ denotes this qualification (e.g., `unique Buffer`, `const Config`).

**Permission Equivalence**

Two permission-qualified types are equivalent if and only if both their base types and their permissions are identical. The formal rule (T-Equiv-Permission) is defined in §4.1.

---

#### 4.5.2 The Permission Lattice

The three permissions form a linear lattice ordered by the subtype relation:

$$\texttt{unique} <: \texttt{shared} <: \texttt{const}$$

The lattice structure is:

```
    unique        (exclusive read-write)
       |
  shared      (synchronized shared mutable)
       |
     const        (shared read-only)
```

---

#### 4.5.3 Orthogonality with Binding Mutability and Responsibility

The permission system is **strictly orthogonal** to two other language concepts:

1. **Binding Mutability** (§4.18): Whether a binding can be reassigned to a different value.
   - Controlled by `let` (immutable binding) versus `var` (mutable binding)
   - A `let` binding may hold `unique` data; a `var` binding may hold `const` data

2. **Cleanup Responsibility** (§3.4–§3.5): Which binding is responsible for destroying an object.
   - Controlled **exclusively** by the `move` keyword
   - Permissions do **not** determine cleanup responsibility

These orthogonal axes produce four distinct parameter forms:

| Parameter Declaration  | Permission        | Cleanup Responsibility |
| :--------------------- | :---------------- | :--------------------- |
| `param: unique T`      | Exclusive mutable | Caller retains         |
| `move param: unique T` | Exclusive mutable | Callee takes           |
| `param: const T`       | Read-only         | Caller retains         |
| `move param: const T`  | Read-only         | Callee takes           |

---

#### 4.5.4 Detailed Permission Semantics

The following definitions specify the semantic meaning of each permission.

**`const` — Immutable Access (Default)**

The `const` permission grants read-only access to data and permits unlimited aliasing. Mutation through a `const` path is forbidden; any number of `const` paths to the same data may coexist simultaneously. Data accessible only through `const` paths is inherently safe for shared read access across threads.

**`unique` — Exclusive Mutable Access**

The `unique` permission grants exclusive read-write access to data, guaranteeing that no other live path to the same data exists. The existence of a live `unique` path to an object statically precludes the existence of any other path to that object or its sub-components. A `unique` path may be temporarily downgraded to a weaker permission for a bounded scope; during this period, the original path is **inactive** and the binding becomes unusable until the scope ends. The `unique` permission does **not** imply cleanup responsibility; a procedure parameter of type `unique T` (without `move`) grants exclusive access while the caller retains responsibility for cleanup.

**`shared` — Key-Synchronized Mutable Access**

The `shared` permission grants mutable access to data through implicit key acquisition.
A `shared` value is conceptually "locked by default"—access requires a key, which is
automatically acquired when accessing the data and released after use.

**Key Properties:**

- **Path-specific:** Keys are acquired at the granularity of the accessed path
- **Implicitly acquired:** Accessing a `shared` path acquires the necessary key automatically
- **Minimal scope:** Keys are held for the minimal duration preserving correctness
- **Reentrant:** If a covering key is already held, nested access succeeds without additional acquisition

The `shared` permission does **not** imply cleanup responsibility; a procedure parameter
of type `shared T` (without `move`) grants synchronized access while the caller retains
responsibility for cleanup.

The following table specifies which operations are permitted through a `shared` path:

| Operation                   | Permitted | Key Mode  | Rationale                                |
| :-------------------------- | :-------- | :-------- | :--------------------------------------- |
| Field read                  | Yes       | Read key  | Multiple readers may proceed in parallel |
| Field mutation              | Yes       | Write key | Exclusive access during mutation         |
| Method call (`~` receiver)  | Yes       | Read key  | `shared <: const`                        |
| Method call (`~%` receiver) | Yes       | Write key | Method controls internal synchronization |
| Method call (`~!` receiver) | No        | N/A       | `shared` is not `<: unique`              |

**Key Modes:**

| Mode  | Grants       | Multiple Holders | Acquired By  |
| :---- | :----------- | :--------------- | :----------- |
| Read  | Read         | Yes              | Read access  |
| Write | Read + Write | No               | Write access |

> **Cross-Reference:** The complete key system specification, including acquisition, release, deadlock prevention, and compile-time verification, is defined in §13. See §4.5.6 for the receiver compatibility matrix.

> **Rationale:** The key system provides fine-grained, path-based synchronization with
> zero syntactic overhead for the common case. Disjoint paths to the same `shared`
> object may be accessed in parallel without contention.

**Exclusion Principle (Normative)**

The rules governing which permission-qualified paths may coexist for the same object are defined in this section. The **coexistence matrix** specifies which permissions may exist simultaneously:

| Active Permission | May Add `unique` | May Add `shared` | May Add `const` |
| :---------------- | :--------------- | :--------------- | :-------------- |
| `unique`          | No               | No               | No              |
| `shared`          | No               | Yes              | Yes             |
| `const`           | No               | Yes              | Yes             |

> **Cross-Reference:** See §3.1.1 for memory model context (temporal transitions, non-overlapping windows). Diagnostic `E-TYP-1602` is canonical for coexistence violations.

**Complete Permission Reference:**

This section (§4.5) is the authoritative source for all permission rules:
- **§4.5.1:** Permission definitions
- **§4.5.2:** Permission lattice structure
- **§4.5.3:** Orthogonality with binding mutability and responsibility
- **§4.5.4:** Coexistence rules (this section)
- **§4.5.5:** Subtyping rules
- **§4.5.6:** Method receiver compatibility

##### Syntax & Declaration

**Grammar**

```ebnf
permission       ::= "const" | "unique" | "shared"
permission_type  ::= [permission] type
```

When no permission is specified, `const` is implied.

**Shorthand Notation**

The receiver shorthand notations (`~`, `~!`, `~%`) are defined in §2.7 (Operators and Punctuators). The semantic mapping to permission-qualified `self` parameters is:

- `~` expands to `self: const Self`
- `~!` expands to `self: unique Self`
- `~%` expands to `self: shared Self`

These shorthands are syntactic sugar. A conforming implementation MUST treat them as equivalent to their expanded forms.

---

#### 4.5.5 Permission Subtyping Rules

##### Static Semantics

**Subtyping Rules**

The following rules define the subtype relation for permission-qualified types:

$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{shared}\ T} \quad \text{(Sub-Perm-UC)}$$

$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{const}\ T} \quad \text{(Sub-Perm-UK)}$$

$$\frac{}{\Gamma \vdash \texttt{shared}\ T <: \texttt{const}\ T} \quad \text{(Sub-Perm-CK)}$$

**Permission Lattice Summary**

The subtyping rules above formalize the linear lattice structure defined in §4.5.2. For the complete lattice diagram and permission definitions, see §4.5.2.

**No Implicit Upgrade**

Coercion from a weaker permission to a stronger permission is **not derivable**:

$$\Gamma \nvdash \texttt{const}\ T <: \texttt{unique}\ T$$

$$\Gamma \nvdash \texttt{const}\ T <: \texttt{shared}\ T$$

$$\Gamma \nvdash \texttt{shared}\ T <: \texttt{unique}\ T$$

Implicit upgrade attempts are rejected per §4.2 (`E-TYP-1511`).

**shared-Safe Method Rule**

A method $m$ on type $T$ is callable through a `shared` path if and only if the method's receiver accepts `shared` permission or a weaker permission (`const`):

$$\frac{\Gamma \vdash m : T.\text{method} \quad m.\text{receiver} \in \{\texttt{shared}, \texttt{const}\}}{\Gamma \vdash (\texttt{shared}\ T).m\ \text{callable}} \quad \text{(shared-Method)}$$

**shared Access to Types Without Synchronized Methods**

A type that provides no methods accepting `shared Self` (or `~%`) receiver is **read-only through `shared` paths**. Such types may still be useful in shared contexts when:

1. Only field reads are required, OR
2. The type implements `const`-compatible methods sufficient for the use case.

**Example — Read-only shared access:**

```cursive
record Config {
    timeout_ms: u32,
    max_retries: u32,
    // No methods accepting ~% receiver
}

let config: shared Config = load_config()
let timeout = config.timeout_ms    // OK: field read
// config.timeout_ms = 5000        // ERROR E-TYP-1604: field mutation forbidden
```

> **Design Note:** Types intended for shared mutation SHOULD provide methods with `~%` receivers that encapsulate synchronization. Types without such methods are implicitly read-only when accessed through `shared` paths. This is intentional: the absence of synchronized methods indicates the type was not designed for shared mutation.

---

#### 4.5.6 Method Receiver Permissions

**Receiver Compatibility Matrix**

A method with receiver permission $P_{\text{method}}$ is callable through a path with permission $P_{\text{caller}}$ if and only if $P_{\text{caller}} <: P_{\text{method}}$ in the permission subtyping lattice (§4.5.5).

| Caller's Permission | `~` (const) | `~!` (unique) | `~%` (shared) |
| :------------------ | :---------: | :-----------: | :-----------: |
| `const`             |      ✓      |       ✗       |       ✗       |
| `unique`            |      ✓      |       ✓       |       ✓       |
| `shared`            |      ✓      |       ✗       |       ✓       |

**Reading the matrix:** Row = permission of the caller's path to the receiver. Column = permission required by the method's receiver parameter. ✓ = method is callable. ✗ = method is not callable (error `E-TYP-1605`).

**Key observations** (derived from the permission lattice in §4.5.2)**:**

1. **`unique` callers can call any method.** The `unique` permission is the lattice top and subtypes all other permissions.

2. **`const` callers can only call `const` methods.** The `const` permission is the lattice bottom; it cannot be upgraded.

3. **`shared` callers can call `const` and `shared` methods.** The `shared` permission is in the middle of the linear lattice.

4. **All callers can call `const` methods.** Every permission subtypes `const`, so `~` methods are universally accessible.

**Formal Rule (Receiver-Compat):**

$$\frac{
    \Gamma \vdash e : P_{\text{caller}}\ T \quad
    m \in \text{Methods}(T) \quad
    m.\text{receiver} = P_{\text{method}}\ \texttt{Self} \quad
    \Gamma \vdash P_{\text{caller}}\ T <: P_{\text{method}}\ T
}{
    \Gamma \vdash e.m(\ldots)\ \text{well-typed}
} \quad \text{(Receiver-Compat)}$$

**Examples:**

```cursive
record Counter {
    value: i32

    procedure get(~) -> i32 { self.value }           // const receiver
    procedure set(~!, new_value: i32) { self.value = new_value }  // unique receiver
    procedure increment(~%) { /* atomic increment */ }  // shared receiver
}

let c1: const Counter = Counter { value: 0 }
c1.get()           // OK: const caller, const method
// c1.set(5)       // ERROR E-TYP-1605: const caller, unique method
// c1.increment()  // ERROR E-TYP-1605: const caller, shared method

let c2: unique Counter = Counter { value: 0 }
c2.get()           // OK: unique caller, const method (unique <: const)
c2.set(5)          // OK: unique caller, unique method
c2.increment()     // OK: unique caller, shared method (unique <: shared)

let c3: shared Counter = Counter { value: 0 }
c3.get()           // OK: shared caller, const method (shared <: const)
// c3.set(5)       // ERROR E-TYP-1605: shared caller, unique method
c3.increment()     // OK: shared caller, shared method
```

**Key Acquisition in Method Calls**

When a method with `~%` receiver is called through a `shared` path, the key is
acquired for the duration of the method call:

```cursive
let counter: shared Counter = Counter { value: 0 }

// Key acquisition sequence:
counter.increment()
// 1. Acquire write key to `counter` (method has ~% receiver)
// 2. Execute increment method body
// 3. Release key

// Parallel access to disjoint shared data:
parallel {
    spawn { counter_a.increment() }  // Key to counter_a
    spawn { counter_b.increment() }  // Key to counter_b — parallel, no contention
}
```

For explicit control over key scope, see the `#` coarsening annotation in §13.2.

---

**Downgrading Scope and Inactive Bindings**

A binding is **inactive** when its permission has been temporarily downgraded for a bounded scope.

When a `unique` path is passed to a procedure expecting a weaker permission, the original binding enters the inactive state for the duration of the call:

1. During the inactive period, the original `unique` binding MUST NOT be read, written, or moved.
2. Any attempt to use an inactive binding MUST be rejected with diagnostic `E-TYP-1602`.
3. When the downgrade scope ends, the original binding returns to active status with `unique` permission restored.

**Inactive Binding Examples**

The following examples illustrate the inactive binding mechanism:

**Example 1 — Function call downgrade:**

```cursive
procedure read_only(data: const Buffer) { /* ... */ }

let buf: unique Buffer = Buffer::new()
read_only(buf)          // 'buf' inactive during call (downgraded to const)
buf.write(42)           // OK: 'buf' reactivated as unique after call returns
```

**Example 2 — Nested downgrade scope:**

```cursive
let buf: unique Buffer = Buffer::new()
{
    let view: const Buffer = buf    // 'buf' inactive in this block
    // buf.write(42)                // ERROR E-TYP-1602: 'buf' is inactive
    view.read()                     // OK: 'view' is const
}
buf.write(42)                       // OK: 'buf' reactivated
```

**Example 3 — Inactive binding error:**

```cursive
procedure process(a: const Data, b: unique Data) { /* ... */ }

let d: unique Data = Data::new()
process(d, d)           // ERROR E-TYP-1602: 'd' is inactive (passed as const)
                        // when attempting second use (as unique)
```

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to permission-qualified types:

1. A `const` path MUST NOT be used to mutate data.
2. A `shared` path MUST NOT be used for direct field mutation.
3. Multiple paths violating the Exclusion Principle MUST NOT coexist.

**Diagnostic Table**

| Code         | Severity | Condition                                                                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1601` | Error    | Attempt to mutate data via a `const` path.                                            | Compile-time | Rejection |
| `E-TYP-1602` | Error    | Violation of `unique` exclusion (aliasing detected or inactive use).                  | Compile-time | Rejection |
| `E-TYP-1604` | Error    | Direct field mutation through `shared` path.                                          | Compile-time | Rejection |
| `E-TYP-1605` | Error    | Method receiver permission incompatible with caller's permission (see §4.5.6 matrix). | Compile-time | Rejection |

**Canonical Diagnostic Codes**

See Appendix J (§J.1) for the cross-reference matrix mapping permission diagnostics to memory model diagnostics. Per Appendix K, type system diagnostics (`E-TYP-`) are canonical; memory model diagnostics (`E-MEM-`) are secondary.

> **Note:** Implicit permission upgrade (`E-TYP-1511`) diagnostic is defined in §4.2.

---

### Clause 4 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 4 That MUST NOT Be Redefined Elsewhere:**

| Term                    | Section | Description                                                   |
| :---------------------- | :------ | :------------------------------------------------------------ |
| Type                    | §4.1    | Classification of values determining representation and usage |
| Type Equivalence        | §4.1    | Relation determining when two types classify the same values  |
| Type Context (Γ)        | §4.1    | Environment mapping identifiers to types                      |
| Subtype                 | §4.2    | Partial order relation on types for safe substitution         |
| Coercion                | §4.2    | Implicit conversion from subtype to supertype                 |
| Variance                | §4.3    | How subtyping of type arguments affects parameterized types   |
| Bidirectional Inference | §4.4    | Type inference combining synthesis and checking modes         |
| Permission              | §4.5    | Type qualifier governing access, mutation, and aliasing       |
| `const`                 | §4.5    | Permission for immutable, freely aliasable access             |
| `unique`                | §4.5    | Permission for exclusive read-write access                    |
| `shared`                | §4.5    | Permission for synchronized temporal aliased mutability       |
| Permission Lattice      | §4.5    | Linear order on permissions defining subtyping                |

**Terms Deferred to Later Clauses:**

| Term           | Deferred To | Reason                     |
| :------------- | :---------- | :------------------------- |
| Primitive Type | §5.1        | Concrete type definitions  |
| Record Type    | §5.3        | Concrete type definitions  |
| Enum Type      | §5.4        | Concrete type definitions  |
| Modal Type     | §5.5        | State-machine types        |
| String Type    | §5.6        | Built-in modal string      |
| Pointer Type   | §6.2        | Safe and raw pointer types |
| Generic Type   | §7.1        | Static polymorphism        |
| Form           | Clause 8    | Polymorphism system        |

---

## Clause 5: Data Types

This clause defines the concrete data-carrying types in Cursive: primitive scalar types, composite structural types (tuples, arrays, slices, ranges), nominal product types (records), nominal sum types (enums), structural sum types (unions), and the built-in string type. These types represent the "nouns" of the type system—the fundamental building blocks for representing data.

---

### 5.1 Primitive Types

##### Definition

**Primitive types** are the built-in scalar types that form the foundation of the Cursive type system. These types represent indivisible units of data with fixed size and well-defined representation. All composite types are ultimately constructed from primitive types.

**Formal Definition**

Let $\mathcal{T}_{\text{prim}}$ denote the set of primitive types:

$$\mathcal{T}_{\text{prim}} = \mathcal{T}_{\text{int}} \cup \mathcal{T}_{\text{float}} \cup \{\texttt{bool}, \texttt{char}, \texttt{()}, \texttt{!}\}$$

where:

$$\mathcal{T}_{\text{int}} = \{\texttt{i8}, \texttt{i16}, \texttt{i32}, \texttt{i64}, \texttt{i128}, \texttt{u8}, \texttt{u16}, \texttt{u32}, \texttt{u64}, \texttt{u128}, \texttt{isize}, \texttt{usize}\}$$

$$\mathcal{T}_{\text{float}} = \{\texttt{f16}, \texttt{f32}, \texttt{f64}\}$$

##### Syntax & Declaration

**Grammar**

```ebnf
primitive_type     ::= integer_type | float_type | "bool" | "char" | unit_type | never_type

integer_type       ::= signed_int | unsigned_int | pointer_int 
signed_int         ::= "i8" | "i16" | "i32" | "i64" | "i128"
unsigned_int       ::= "u8" | "u16" | "u32" | "u64" | "u128"
pointer_int        ::= "isize" | "usize"

float_type         ::= "f16" | "f32" | "f64" 

unit_type          ::= "(" ")"
never_type         ::= "!"
```


**Literal Syntax**

Integer and floating-point literals are defined in Clause 2 (Lexical Structure). The following summary specifies the mapping from literal forms to types:

| Literal Form              | Default Type | Type Suffix       |
| :------------------------ | :----------- | :---------------- |
| `42`, `-7`                | `i32`        | `i8`, `u64`, etc. |
| `0x1F`, `0o77`, `0b1010`  | `i32`        | `i8`, `u64`, etc. |
| `3.14`, `-0.5`            | `f64`        | `f32`, `f16`      |
| `1e10`, `2.5e-3`          | `f64`        | `f32`, `f16`      |
| `true`, `false`           | `bool`       | (none)            |
| `'a'`, `'\n'`, `'\u0041'` | `char`       | (none)            |
| `()`                      | `()`         | (none)            |

##### Static Semantics

**Type Equivalence**

Primitive types have **strict name equivalence**: two primitive type expressions are equivalent if and only if, after alias resolution, they denote the same type:

$$\frac{T \in \mathcal{T}_{\text{prim}} \quad U \in \mathcal{T}_{\text{prim}} \quad T = U}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Prim)}$$

**Never Type as Bottom**

The never type `!` is the **bottom type** of the type system. It is a subtype of all other types:

$$\frac{T \in \mathcal{T}}{\Gamma \vdash \texttt{!} <: T} \quad \text{(Sub-Never)}$$

This rule permits expressions of type `!` (such as `panic()` or infinite loops) to appear in any context regardless of the expected type.

**Literal Typing Rules**

**(T-Int-Literal)**
$$\frac{v \in \text{IntLiteral} \quad T \in \mathcal{T}_{\text{int}} \quad \text{InRange}(v, T)}{\Gamma \vdash v : T}$$

**(T-Float-Literal)**
$$\frac{v \in \text{FloatLiteral} \quad T \in \mathcal{T}_{\text{float}}}{\Gamma \vdash v : T}$$

**(T-Bool-Literal)**
$$\frac{v \in \{\texttt{true}, \texttt{false}\}}{\Gamma \vdash v : \texttt{bool}}$$

**(T-Char-Literal)**
$$\frac{v \in \text{CharLiteral} \quad \text{IsUSV}(v)}{\Gamma \vdash v : \texttt{char}}$$

**(T-Unit-Literal)**
$$\frac{}{\Gamma \vdash () : ()}$$

where $\text{InRange}(v, T)$ holds when the numeric value $v$ is representable in type $T$, and $\text{IsUSV}(v)$ holds when $v$ is a valid Unicode Scalar Value.

**Operator Typing**

The following operators are defined for integer types:

| Category    | Operators                        | Signature                                      |
| :---------- | :------------------------------- | :--------------------------------------------- |
| Arithmetic  | `+`, `-`, `*`, `/`, `%`          | $(T, T) \to T$ where $T \in \mathcal{T}_{int}$ |
| Bitwise     | `&`, `\|`, `^`, `<<`, `>>`       | $(T, T) \to T$ where $T \in \mathcal{T}_{int}$ |
| Comparison  | `==`, `!=`, `<`, `<=`, `>`, `>=` | $(T, T) \to \texttt{bool}$                     |
| Negation    | `-` (prefix)                     | $T \to T$ where $T \in \mathcal{T}_{int}$      |
| Bitwise NOT | `!` (prefix)                     | $T \to T$ where $T \in \mathcal{T}_{int}$      |

Floating-point types support arithmetic (`+`, `-`, `*`, `/`, `%`) and comparison operators with analogous signatures. The `%` operator on floating-point types computes the IEEE 754 remainder.

The following operators are defined for boolean types:

| Operator   | Name        | Signature                                          | Semantics            |
| :--------- | :---------- | :------------------------------------------------- | :------------------- |
| `&&`       | Logical AND | $(\texttt{bool}, \texttt{bool}) \to \texttt{bool}$ | Short-circuiting     |
| `\|\|`     | Logical OR  | $(\texttt{bool}, \texttt{bool}) \to \texttt{bool}$ | Short-circuiting     |
| `!`        | Logical NOT | $\texttt{bool} \to \texttt{bool}$                  | Prefix unary         |
| `==`, `!=` | Equality    | $(\texttt{bool}, \texttt{bool}) \to \texttt{bool}$ | Non-short-circuiting |

> **Note:** The logical operators `&&` and `||` evaluate their right operand only when necessary to determine the result. Short-circuit evaluation semantics are defined in Clause 8 (Expressions).

The character type supports equality and ordering comparisons based on Unicode code point value:

| Operator                         | Signature                                          | Semantics             |
| :------------------------------- | :------------------------------------------------- | :-------------------- |
| `==`, `!=`, `<`, `<=`, `>`, `>=` | $(\texttt{char}, \texttt{char}) \to \texttt{bool}$ | Code point comparison |

**Implicit Numeric Conversions**

There are **no implicit conversions** between distinct primitive types. A conforming implementation MUST reject any expression where a value of one primitive type is used in a context requiring a different primitive type without an explicit cast.

$$\frac{T \neq U \quad T \in \mathcal{T}_{\text{prim}} \quad U \in \mathcal{T}_{\text{prim}} \quad U \neq \texttt{!}}{\Gamma \nvdash T <: U} \quad \text{(No-Implicit-Prim-Coerce)}$$

> **Rationale:** Implicit numeric conversions are a common source of bugs, including silent precision loss, sign-related errors, and platform-dependent behavior. Cursive requires explicit casts to make all conversions visible in source code.

##### Dynamic Semantics

**Integer Overflow**

Arithmetic operations on integer types (`+`, `-`, `*`) may overflow when the mathematical result is not representable in the result type.

A conforming implementation MUST provide a **checked mode** in which signed and unsigned integer overflow causes the executing thread to **panic**. This mode MUST be the default for debug builds.

The behavior of integer overflow in release (optimized) builds is **implementation-defined (IDB)**. Conforming implementations MAY choose one of the following behaviors for release mode:

1. **Wrap** — The result is the mathematical result modulo $2^N$ where $N$ is the bit width.
2. **Panic** — Same behavior as checked mode.
3. **Trap** — Invoke a platform-specific trap or signal.

The implementation MUST document its release-mode overflow behavior in the Conformance Dossier (§1.1). The chosen behavior MUST apply uniformly to all integer overflow operations of the same category (signed vs. unsigned); an implementation MUST NOT vary overflow behavior on a per-operation or per-type basis within a single compilation.

> **Note:** An implementation MAY choose different behaviors for signed and unsigned overflow (e.g., wrap for unsigned, panic for signed), but MUST apply each choice consistently across all operations of that signedness category.

> **Note:** Explicit wrapping, saturating, and checked arithmetic operations are available as methods on integer types (e.g., `i32::wrapping_add`, `i32::saturating_mul`, `i32::checked_sub`) and are not affected by build mode.

**Integer Division and Remainder**

Division (`/`) and remainder (`%`) operations on integer types MUST trigger a **panic** when the divisor is zero. Implementations MUST NOT optimize away division-by-zero checks.

**Floating-Point Semantics**

Floating-point arithmetic MUST conform to **IEEE 754** for the corresponding precision:

| Type  | IEEE 754 Format | Exponent Bits | Significand Bits |
| :---- | :-------------- | :------------ | :--------------- |
| `f16` | binary16        | 5             | 10               |
| `f32` | binary32        | 8             | 23               |
| `f64` | binary64        | 11            | 52               |

All IEEE 754 special values and behaviors are supported:

- **NaN (Not a Number)** — Propagates through arithmetic operations per IEEE 754.
- **Infinity** — Positive and negative infinity (`+inf`, `-inf`).
- **Signed zero** — Positive and negative zero are distinct bit patterns but compare equal.
- **Division by zero** — Produces infinity (for non-zero dividend) or NaN (for zero dividend).

**Unit Type Semantics**

The unit type `()` has exactly one value, also written `()`. The unit value is the implicit result of:

- Procedures without an explicit return type
- Block expressions whose final statement is not an expression
- The `result` statement with no expression

**Never Type Semantics**

The never type `!` is **uninhabited**: no value of type `!` can be created. Expressions of type `!` represent computations that do not return normally. Such expressions include:

- Invocations of diverging procedures (e.g., `panic()`, `sys::exit()`)
- Infinite loops with no `break` expression
- The `unreachable!()` intrinsic

Because `!` is uninhabited, code following an expression of type `!` is unreachable.

##### Memory & Layout

**Primitive Type Layout**

Primitive types have defined layouts. Implementations MUST use the following sizes and alignments:

| Type   | Size (bytes) | Alignment (bytes) | Representation                                               |
| :----- | :----------- | :---------------- | :----------------------------------------------------------- |
| `i8`   | 1            | 1                 | Two's complement signed integer                              |
| `u8`   | 1            | 1                 | Unsigned integer                                             |
| `bool` | 1            | 1                 | `0x00` = `false`, `0x01` = `true`; other values are invalid  |
| `i16`  | 2            | 2                 | Two's complement signed integer                              |
| `u16`  | 2            | 2                 | Unsigned integer                                             |
| `f16`  | 2            | 2                 | IEEE 754 binary16                                            |
| `i32`  | 4            | 4                 | Two's complement signed integer                              |
| `u32`  | 4            | 4                 | Unsigned integer                                             |
| `f32`  | 4            | 4                 | IEEE 754 binary32                                            |
| `char` | 4            | 4                 | Unicode scalar value (U+0000–U+10FFFF, excluding surrogates) |
| `i64`  | 8            | 8                 | Two's complement signed integer                              |
| `u64`  | 8            | 8                 | Unsigned integer                                             |
| `f64`  | 8            | 8                 | IEEE 754 binary64                                            |
| `i128` | 16           | IDB               | Two's complement signed integer                              |
| `u128` | 16           | IDB               | Unsigned integer                                             |

The alignment of `i128` and `u128` is implementation-defined (IDB). Conforming implementations MUST choose an alignment of 8 or 16 bytes and document the choice in the Conformance Dossier.

**Platform-Dependent Types**

The pointer-sized integer types have sizes and alignments determined by the target platform:

| Type    | Size                   | Alignment              | Representation                  |
| :------ | :--------------------- | :--------------------- | :------------------------------ |
| `usize` | Platform pointer width | Platform pointer width | Unsigned integer                |
| `isize` | Platform pointer width | Platform pointer width | Two's complement signed integer |

On 32-bit platforms, `usize` and `isize` are 4 bytes with 4-byte alignment. On 64-bit platforms, they are 8 bytes with 8-byte alignment. The exact pointer width is implementation-defined (IDB) and MUST be documented in the Conformance Dossier.

**Unit and Never Types**

| Type | Size (bytes) | Alignment (bytes) |
| :--- | :----------- | :---------------- |
| `()` | 0            | 1                 |
| `!`  | 0            | 1                 |

The unit type `()` and never type `!` are zero-sized types (ZSTs). Values of these types do not occupy storage.

**Representation Invariants**

The following invariants define valid bit patterns for specific primitive types. Constructing a value that violates these invariants is **Unverifiable Behavior (UVB)** and MUST occur only within an `unsafe` block.

*Boolean:* A valid `bool` value MUST be represented by exactly one of two bit patterns: `0x00` (false) or `0x01` (true). Any other bit pattern is invalid.

*Character:* A valid `char` value MUST contain a **Unicode Scalar Value (USV)**, defined as any Unicode code point in the ranges `U+0000`–`U+D7FF` or `U+E000`–`U+10FFFF`. Surrogate code points (`U+D800`–`U+DFFF`) and values above `U+10FFFF` are invalid.

##### Constraints & Legality

**Negative Constraints**

See diagnostic table below for constraint violations. Key constraints:

1. Integer literals MUST be representable in their target type.
2. Character literals MUST represent valid Unicode Scalar Values.
3. Implicit conversions between distinct primitive types are forbidden.

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `W-TYP-1701` | Warning  | `f16` arithmetic emulated on target platform.          | Compile-time | Warning   |
| `E-TYP-1710` | Error    | Integer literal out of range for target type.          | Compile-time | Rejection |
| `E-TYP-1711` | Error    | Character literal is not a valid Unicode Scalar Value. | Compile-time | Rejection |
| `E-TYP-1712` | Error    | Implicit conversion between distinct primitive types.  | Compile-time | Rejection |
| `E-TYP-1713` | Error    | Shadowing of primitive type alias identifier.          | Compile-time | Rejection |
| `P-TYP-1720` | Panic    | Integer overflow in checked mode.                      | Runtime      | Panic     |
| `P-TYP-1721` | Panic    | Integer division or remainder by zero.                 | Runtime      | Panic     |

---

### 5.2 Composite Types

##### Definition

**Composite types** are types constructed by aggregating other types into structured collections. This section defines the structural composite types: tuples, arrays, slices, and ranges. These types share the property of **structural equivalence**: two composite type expressions are equivalent if their structure matches, regardless of where or how they are declared.

**Formal Definition**

Let $\mathcal{T}_{\text{composite}}$ denote the set of structural composite types defined in this section:

$$\mathcal{T}_{\text{composite}} = \mathcal{T}_{\text{tuple}} \cup \mathcal{T}_{\text{array}} \cup \mathcal{T}_{\text{slice}} \cup \mathcal{T}_{\text{range}}$$

> **Note:** Nominal composite types (records and enums) are defined separately in §5.3 and §5.4 respectively. Union types are defined in §5.5.

#### 5.2.1 Tuples

##### Definition

A **tuple** is an ordered, fixed-length, heterogeneous sequence of values. Tuples are anonymous product types with **structural equivalence**.

**Formal Definition**

A tuple type is an ordered sequence of component types:

$$\mathcal{T}_{\text{tuple}} = \{(T_1, T_2, \ldots, T_n) : n \geq 0 \land \forall i \in 1..n,\ T_i \in \mathcal{T}\}$$

The **arity** of a tuple type $(T_1, \ldots, T_n)$ is $n$. The **unit type** `()` is the tuple of arity zero.

##### Syntax & Declaration

**Grammar**

```ebnf
tuple_type       ::= "(" type_list? ")"
type_list        ::= type ("," type)* ","?

tuple_literal    ::= "(" expression_list? ")"
expression_list  ::= expression ("," expression)* ","?

tuple_access     ::= expression "." decimal_literal
```

**Type Expression**

A tuple type is written as a parenthesized, comma-separated list of component types: `(T₁, T₂, ..., Tₙ)`.

**Literal Syntax**

A tuple value is constructed with a corresponding literal: `(e₁, e₂, ..., eₙ)`.

**Element Access**

Individual elements of a tuple `t` are accessed by a constant, zero-based integer index using dot notation: `t.0`, `t.1`, etc. The index MUST be a decimal integer literal; it MUST NOT be a variable or computed expression.

##### Static Semantics

**Typing Rules**

**(T-Tuple-Type)**
A tuple type is well-formed when all component types are well-formed:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash (T_1, \ldots, T_n)\ \text{wf}}$$

**(T-Tuple-Literal)**
A tuple literal has a tuple type when each component expression has the corresponding component type:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T_i}{\Gamma \vdash (e_1, \ldots, e_n) : (T_1, \ldots, T_n)}$$

**(T-Tuple-Index)**
Accessing element $i$ of a tuple yields the $i$-th component type (zero-indexed):
$$\frac{\Gamma \vdash e : (T_0, T_1, \ldots, T_{n-1}) \quad 0 \leq i < n}{\Gamma \vdash e.i : T_i} \quad \text{(T-Tuple-Index)}$$

**Type Equivalence**

Tuple types have structural equivalence. Two tuple types are equivalent if and only if they have the same arity and all corresponding component types are equivalent:

$$\frac{n = m \quad \forall i \in 1..n,\ \Gamma \vdash T_i \equiv U_i}{\Gamma \vdash (T_1, \ldots, T_n) \equiv (U_1, \ldots, U_m)} \quad \text{(T-Equiv-Tuple)}$$

**Subtyping**

Tuple subtyping is covariant in all component positions:

$$\frac{n = m \quad \forall i \in 1..n,\ \Gamma \vdash T_i <: U_i}{\Gamma \vdash (T_1, \ldots, T_n) <: (U_1, \ldots, U_m)} \quad \text{(Sub-Tuple)}$$

**Permission Propagation**

The permission of a tuple value propagates to element access. If `t` has permission $P$, then `t.i` has permission $P$ for all valid indices $i$.

##### Memory & Layout

**Representation**

A tuple is laid out as a contiguous sequence of its component values. Components are stored in declaration order (left to right).

**Size and Alignment**

$$\text{sizeof}((T_1, \ldots, T_n)) = \sum_{i=1}^{n} \text{sizeof}(T_i) + \text{padding}$$

$$\text{alignof}((T_1, \ldots, T_n)) = \max_{i \in 1..n}(\text{alignof}(T_i))$$

For the unit type: $\text{sizeof}(()) = 0$ and $\text{alignof}(()) = 1$.

**Padding**

The amount and placement of inter-element padding is **implementation-defined (IDB)**. Implementations MUST document their tuple padding strategy in the Conformance Dossier.

> **Note:** Padding is typically inserted to satisfy alignment requirements of subsequent elements, but implementations MAY choose different strategies.

##### Constraints & Legality

**Constraints**

Tuple indices MUST be compile-time constant integers within bounds. Single-element tuples require trailing comma: `(T,)` (1-tuple) vs. `(T)` (parenthesized expression).

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1801` | Error    | Tuple index out of bounds.                                  | Compile-time | Rejection |
| `E-TYP-1802` | Error    | Tuple index is not a compile-time constant integer literal. | Compile-time | Rejection |
| `E-TYP-1803` | Error    | Tuple arity mismatch in assignment or pattern.              | Compile-time | Rejection |


#### 5.2.2 Arrays

##### Definition

An **array** is a contiguous, fixed-length, homogeneous sequence of elements. The length of an array is part of its type and MUST be known at compile time.

**Formal Definition**

An array type is parameterized by an element type and a compile-time constant length:

$$\mathcal{T}_{\text{array}} = \{[T; N] : T \in \mathcal{T} \land N \in \mathbb{N}\}$$

where $N$ is a non-negative integer constant expression of type `usize`.

##### Syntax & Declaration

**Grammar**

```ebnf
array_type       ::= "[" type ";" const_expression "]"

array_literal    ::= "[" expression_list "]"
                   | "[" expression ";" const_expression "]"

array_access     ::= expression "[" expression "]"
```

**Type Expression**

An array type is written as `[T; N]`, where `T` is the element type and `N` is a compile-time constant expression of type `usize` specifying the array length.

**Literal Syntax**

Array values are constructed using one of two literal forms:

1. **List form:** `[e₁, e₂, ..., eₙ]` — Creates an array of length $n$ with each element initialized to the corresponding expression.

2. **Repeat form:** `[e; N]` — Creates an array of length $N$ with all elements initialized to copies of `e`. The expression `e` MUST have a type that implements `Copy`, or MUST be a constant expression.

**Element Access**

Elements of an array `a` are accessed via indexing: `a[i]`, where `i` is an expression of type `usize`.

##### Static Semantics

**Typing Rules**

**(T-Array-Type)**
An array type is well-formed when the element type is well-formed and the length is a valid constant:
$$\frac{\Gamma \vdash T\ \text{wf} \quad N : \texttt{usize} \quad N \geq 0}{\Gamma \vdash [T; N]\ \text{wf}}$$

**(T-Array-Literal-List)**
A list-form array literal has an array type when all element expressions have the element type:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T}{\Gamma \vdash [e_1, \ldots, e_n] : [T; n]}$$

**(T-Array-Literal-Repeat)**
A repeat-form array literal has an array type when the initializer has the element type:
$$\frac{\Gamma \vdash e : T \quad N : \texttt{usize} \quad T : \texttt{Copy} \lor e \in \text{ConstExpr}}{\Gamma \vdash [e; N] : [T; N]}$$

**(T-Array-Index)**
Indexing an array yields the element type:
$$\frac{\Gamma \vdash a : [T; N] \quad \Gamma \vdash i : \texttt{usize}}{\Gamma \vdash a[i] : T}$$

**Type Equivalence**

Array types have structural equivalence. Two array types are equivalent if and only if their element types are equivalent and their lengths are equal:

$$\frac{\Gamma \vdash T \equiv U \quad N = M}{\Gamma \vdash [T; N] \equiv [U; M]} \quad \text{(T-Equiv-Array)}$$

**Subtyping**

Array subtyping is covariant in the element type and invariant in length:

$$\frac{\Gamma \vdash T <: U \quad N = M}{\Gamma \vdash [T; N] <: [U; M]} \quad \text{(Sub-Array)}$$

**Permission Propagation**

The permission of an array value propagates to element access. If `a` has permission $P$, then `a[i]` has permission $P$ for all valid indices $i$.

##### Dynamic Semantics

**Bounds Checking**

All array indexing MUST be bounds-checked at runtime. An attempt to access an index $i$ where $i \geq N$ MUST cause the executing thread to panic with diagnostic `P-TYP-1811`.

**Bounds Check Elision**

Implementations MAY elide bounds checks only when they can **statically prove** that the access is always within bounds. The key-based concurrency model (§12) provides one mechanism for such proofs via disjointness analysis.

**Initialization**

For list-form literals, elements are initialized in left-to-right order. For repeat-form literals, the initialization order is unspecified.

##### Memory & Layout

**Representation**

An array is laid out as a contiguous sequence of elements with no inter-element padding:

$$\text{layout}([T; N]) = T\ ||\ T\ ||\ \cdots\ ||\ T \quad (N \text{ times})$$

where $||$ denotes contiguous concatenation.

**Size and Alignment**

$$\text{sizeof}([T; N]) = N \times \text{sizeof}(T)$$

$$\text{alignof}([T; N]) = \text{alignof}(T)$$

For zero-length arrays: $\text{sizeof}([T; 0]) = 0$ and $\text{alignof}([T; 0]) = \text{alignof}(T)$.

**ABI Guarantee**

The array layout formula above is **defined behavior**. Implementations MUST NOT insert padding between array elements.

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to array types and values:

1. The array length expression MUST be a compile-time constant of type `usize`.
2. The array length MUST NOT be negative.
3. The index expression in an array access MUST have type `usize`.
4. A repeat-form literal with a non-Copy element type MUST use a constant expression for the initializer.

**Diagnostic Table**

| Code         | Severity | Condition                                                     | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1810` | Error    | Array length is not a compile-time constant.                  | Compile-time | Rejection |
| `P-TYP-1811` | Panic    | Array index out of bounds.                                    | Runtime      | Panic     |
| `E-TYP-1812` | Error    | Array index expression has non-`usize` type.                  | Compile-time | Rejection |
| `E-TYP-1813` | Error    | Array repeat literal requires `Copy` or constant initializer. | Compile-time | Rejection |
| `E-TYP-1814` | Error    | Array length mismatch in assignment or pattern.               | Compile-time | Rejection |

#### 5.2.3 Slices

##### Definition

A **slice** is a dynamically-sized view into a contiguous sequence of elements. A slice does not own its data; it borrows from an array or other contiguous storage.

**Formal Definition**

A slice type is parameterized by an element type:

$$\mathcal{T}_{\text{slice}} = \{[T] : T \in \mathcal{T}\}$$

A slice value is a **dense pointer** consisting of a data pointer and a length.

##### Syntax & Declaration

**Grammar**

```ebnf
slice_type       ::= "[" type "]"

slice_access     ::= expression "[" expression "]"
slice_range      ::= expression "[" range_expression "]"
```

**Type Expression**

A slice type is written as `[T]`, where `T` is the element type. Note the absence of a length parameter, distinguishing slices from arrays.

**Slice Creation**

Slices are created by borrowing a range from an array or another slice:

```cursive
let arr: [i32; 5] = [10, 20, 30, 40, 50]
let slice: const [i32] = arr[1..4]    // borrows elements [20, 30, 40]
```

**Element Access**

Elements of a slice `s` are accessed via indexing: `s[i]`, where `i` is an expression of type `usize`.

##### Static Semantics

**Typing Rules**

**(T-Slice-Type)**
A slice type is well-formed when the element type is well-formed:
$$\frac{\Gamma \vdash T\ \text{wf}}{\Gamma \vdash [T]\ \text{wf}}$$

**(T-Slice-From-Array)**
A range expression on an array produces a slice:
$$\frac{\Gamma \vdash a : P\ [T; N] \quad \Gamma \vdash r : \text{Range}}{\Gamma \vdash a[r] : P\ [T]}$$

where $P$ is the permission of the array and Range is any range type from §4.7.4.

**(T-Slice-Index)**
Indexing a slice yields the element type:
$$\frac{\Gamma \vdash s : [T] \quad \Gamma \vdash i : \texttt{usize}}{\Gamma \vdash s[i] : T}$$

**Type Equivalence**

Slice types have structural equivalence based on their element type:

$$\frac{\Gamma \vdash T \equiv U}{\Gamma \vdash [T] \equiv [U]} \quad \text{(T-Equiv-Slice)}$$

**Subtyping**

Slice subtyping is covariant in the element type:

$$\frac{\Gamma \vdash T <: U}{\Gamma \vdash [T] <: [U]} \quad \text{(Sub-Slice)}$$

**Coercion from Array**

An array type coerces to a slice type with the same element type:

$$\frac{\Gamma \vdash a : P\ [T; N]}{\Gamma \vdash a : P\ [T]} \quad \text{(Coerce-Array-Slice)}$$

This coercion is implicit and creates a slice viewing the entire array.

**Permission Propagation**

The permission of a slice value propagates to element access and sub-slicing. If `s` has permission $P$, then `s[i]` and `s[r]` have permission $P$.

##### Dynamic Semantics

**Bounds Checking**

All slice indexing MUST be bounds-checked at runtime. An attempt to access an index $i$ where $i \geq \text{len}(s)$ MUST cause the executing thread to panic with diagnostic `P-TYP-1821`.

**Slice Range Bounds**

Creating a sub-slice with a range MUST verify that the range bounds are within the slice. For a slice `s` of length $L$ and range `start..end`:
- `start` MUST satisfy $0 \leq \text{start} \leq L$
- `end` MUST satisfy $\text{start} \leq \text{end} \leq L$

Violation of either condition MUST cause a panic with diagnostic `P-TYP-1822`.

**Length Accessor**

The `len()` method on slices returns the number of elements:
$$\text{len}(s) : \texttt{usize}$$

##### Memory & Layout

**Representation**

A slice is represented as a **dense pointer** containing two machine words:

| Field | Type    | Description                     |
| :---- | :------ | :------------------------------ |
| `ptr` | `*T`    | Pointer to the first element    |
| `len` | `usize` | Number of elements in the slice |

**Size and Alignment**

$$\text{sizeof}([T]) = 2 \times \text{sizeof}(\texttt{usize})$$

$$\text{alignof}([T]) = \text{alignof}(\texttt{usize})$$

On a 64-bit platform, a slice occupies 16 bytes (two 8-byte words).

**Ownership**

A slice does not own its data. The underlying storage MUST remain valid for the lifetime of the slice. The slice's permission determines whether it may read or mutate the underlying elements.

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to slice types and values:

1. The index expression in a slice access MUST have type `usize`.
2. A slice MUST NOT outlive the storage it borrows from.
3. A mutable slice (`unique [T]`) MUST NOT coexist with any other path to the same elements.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-1820` | Error    | Slice index expression has non-`usize` type. | Compile-time | Rejection |
| `P-TYP-1821` | Panic    | Slice index out of bounds.                   | Runtime      | Panic     |
| `P-TYP-1822` | Panic    | Slice range out of bounds.                   | Runtime      | Panic     |
| `E-TYP-1823` | Error    | Slice outlives borrowed storage.             | Compile-time | Rejection |

#### 5.2.4 Range Types

##### Definition

**Range types** are structural record types produced by range expressions. Ranges represent bounded or unbounded intervals and are primarily used for iteration and slicing operations.

**Formal Definition**

The set of range types comprises six distinct structural record types:

$$\mathcal{T}_{\text{range}} = \{\texttt{Range}\langle T \rangle, \texttt{RangeInclusive}\langle T \rangle, \texttt{RangeFrom}\langle T \rangle, \texttt{RangeTo}\langle T \rangle, \texttt{RangeToInclusive}\langle T \rangle, \texttt{RangeFull}\}$$

##### Syntax & Declaration

**Grammar**

```ebnf
range_expression ::= exclusive_range | inclusive_range | from_range
                   | to_range | to_inclusive_range | full_range

exclusive_range     ::= expression ".." expression
inclusive_range     ::= expression "..=" expression
from_range          ::= expression ".."
to_range            ::= ".." expression
to_inclusive_range  ::= "..=" expression
full_range          ::= ".."
```

**Range Type Definitions**

Implementations MUST provide the following generic record definitions with **public fields**:

| Expression    | Type                  | Fields                             |
| :------------ | :-------------------- | :--------------------------------- |
| `start..end`  | `Range<T>`            | `public start: T`, `public end: T` |
| `start..=end` | `RangeInclusive<T>`   | `public start: T`, `public end: T` |
| `start..`     | `RangeFrom<T>`        | `public start: T`                  |
| `..end`       | `RangeTo<T>`          | `public end: T`                    |
| `..=end`      | `RangeToInclusive<T>` | `public end: T`                    |
| `..`          | `RangeFull`           | (none)                             |

##### Static Semantics

**Typing Rules**

**(T-Range-Exclusive)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T}{\Gamma \vdash e_1\text{..}e_2 : \texttt{Range}\langle T \rangle}$$

**(T-Range-Inclusive)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T}{\Gamma \vdash e_1\text{..=}e_2 : \texttt{RangeInclusive}\langle T \rangle}$$

**(T-Range-From)**
$$\frac{\Gamma \vdash e : T}{\Gamma \vdash e\text{..} : \texttt{RangeFrom}\langle T \rangle}$$

**(T-Range-To)**
$$\frac{\Gamma \vdash e : T}{\Gamma \vdash \text{..}e : \texttt{RangeTo}\langle T \rangle}$$

**(T-Range-To-Inclusive)**
$$\frac{\Gamma \vdash e : T}{\Gamma \vdash \text{..=}e : \texttt{RangeToInclusive}\langle T \rangle}$$

**(T-Range-Full)**
$$\frac{}{\Gamma \vdash \text{..} : \texttt{RangeFull}}$$

**Type Equivalence**

Range types have structural equivalence based on their element type:

$$\frac{\Gamma \vdash T \equiv U}{\Gamma \vdash \texttt{Range}\langle T \rangle \equiv \texttt{Range}\langle U \rangle} \quad \text{(T-Equiv-Range)}$$

Analogous rules apply to all other range type constructors.

**Copy Semantics**

A range type implements `Copy` if and only if its element type `T` implements `Copy`:

$$\frac{T : \texttt{Copy}}{\texttt{Range}\langle T \rangle : \texttt{Copy}} \quad \text{(Copy-Range)}$$

This rule applies to all generic range types. The type `RangeFull` unconditionally implements `Copy`.

**Iteration**

Range types `Range<T>` and `RangeInclusive<T>` implement the `Iterator` form when `T` implements `Step`. This enables their use in `for` loops:

```cursive
for i in 0..10 { /* i: i32, values 0 through 9 */ }
for i in 0..=10 { /* i: i32, values 0 through 10 */ }
```

##### Dynamic Semantics

**Inclusive vs. Exclusive Semantics**

The exclusive range `start..end` represents values $v$ where $\text{start} \leq v < \text{end}$.

The inclusive range `start..=end` represents values $v$ where $\text{start} \leq v \leq \text{end}$.

**Empty Ranges**

A range `start..end` where $\text{start} \geq \text{end}$ is **empty** and produces no elements when iterated.

A range `start..=end` where $\text{start} > \text{end}$ is **empty**.

**Unbounded Ranges**

The types `RangeFrom`, `RangeTo`, `RangeToInclusive`, and `RangeFull` represent unbounded ranges. These types are valid for slicing operations but MUST NOT be used directly in `for` loops without an explicit bound, as iteration would be unbounded.

##### Memory & Layout

**Representation**

Range types are laid out as their equivalent record definitions. Each generic range type containing element type `T` has:

| Type                  | Size                                         | Alignment           |
| :-------------------- | :------------------------------------------- | :------------------ |
| `Range<T>`            | $2 \times \text{sizeof}(T) + \text{padding}$ | $\text{alignof}(T)$ |
| `RangeInclusive<T>`   | $2 \times \text{sizeof}(T) + \text{padding}$ | $\text{alignof}(T)$ |
| `RangeFrom<T>`        | $\text{sizeof}(T)$                           | $\text{alignof}(T)$ |
| `RangeTo<T>`          | $\text{sizeof}(T)$                           | $\text{alignof}(T)$ |
| `RangeToInclusive<T>` | $\text{sizeof}(T)$                           | $\text{alignof}(T)$ |
| `RangeFull`           | $0$                                          | $1$                 |

Padding within range types follows the same implementation-defined rules as records (§4.8).

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to range types:

1. Both bounds of a two-bound range expression (`..` or `..=`) MUST have the same type.
2. Unbounded ranges (`RangeFrom`, `RangeTo`, `RangeToInclusive`, `RangeFull`) MUST NOT be used in contexts requiring a finite iterator without explicit bounds.

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1830` | Error    | Range bound types do not match.                             | Compile-time | Rejection |
| `E-TYP-1831` | Error    | Unbounded range used in context requiring finite iteration. | Compile-time | Rejection |

---

### 5.3 Records

##### Definition

A **record** is a nominal product type with named fields. A record aggregates zero or more typed values into a single composite value, where each constituent value is identified by a field name rather than by position.

**Formal Definition**

Let $\mathcal{R}$ denote the set of all record types. A record type $R$ is defined by:

$$R = (\text{Name}, \text{Params}, \text{Fields}, \text{Forms}, \text{Invariant})$$

where:

- $\text{Name}$ is the unique type identifier within its declaring module
- $\text{Params}$ is an optional list of generic type parameters
- $\text{Fields} = \{(f_1, V_1, T_1), \ldots, (f_n, V_n, T_n)\}$ is an ordered sequence of field declarations, where $f_i$ is the field name, $V_i$ is the field visibility, and $T_i$ is the field type
- $\text{Forms}$ is the set of forms the record implements (possibly empty)
- $\text{Invariant}$ is an optional predicate that constrains valid instances

Two record types are **equivalent** if and only if they refer to the same declaration:

$$\frac{D(R_1) = D(R_2)}{\Gamma \vdash R_1 \equiv R_2} \quad \text{(T-Equiv-Record)}$$

##### Syntax & Declaration

**Grammar**

```ebnf
record_decl      ::= [visibility] "record" identifier [generic_params] 
                     [implements_clause] "{" record_body "}" [type_invariant]

record_body      ::= (partition_decl | field_decl) ("," (partition_decl | field_decl))* ","?

field_decl       ::= [visibility] identifier ":" type

partition_decl   ::= "partition" identifier "{" field_decl ("," field_decl)* ","? "}"

implements_clause ::= "<:" form_list
form_list       ::= type_path ("," type_path)*

type_invariant   ::= "where" "{" predicate "}"
```

**Record Literal**

A record value is constructed using a literal that specifies the type name and provides values for all fields:

```ebnf
record_literal   ::= type_path "{" field_init_list "}"
field_init_list  ::= field_init ("," field_init)* ","?
field_init       ::= identifier ":" expression
                   | identifier
```

**Field Shorthand**

When the initializing expression is a variable with the same name as the field, the shorthand syntax `{ field }` is equivalent to `{ field: field }`.

**Field Access**

Fields of a record instance are accessed using dot notation:

```ebnf
field_access     ::= expression "." identifier
```

**Type Invariant**

A record declaration MAY include a `where` clause after the closing brace to specify an invariant that MUST hold for all valid instances. Within the predicate, the identifier `self` refers to the record instance being constrained.

##### Static Semantics

**Typing Rules**

**(T-Record-Type)**
A record type is well-formed when all field types are well-formed and all fields have distinct names:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf} \quad \forall i \neq j,\ f_i \neq f_j}{\Gamma \vdash \texttt{record}\ R\ \{f_1: T_1, \ldots, f_n: T_n\}\ \text{wf}} \quad \text{(T-Record-WF)}$$

**(T-Record-Literal)**
A record literal has the declared record type when each field initializer has the corresponding field type:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T_i \quad R = \{f_1: T_1, \ldots, f_n: T_n\}}{\Gamma \vdash R\ \{f_1: e_1, \ldots, f_n: e_n\} : R} \quad \text{(T-Record-Lit)}$$

**(T-Field-Access)**
Accessing field $f$ of a record yields the field's type:
$$\frac{\Gamma \vdash e : R \quad R.\text{fields}(f) = T \quad \text{visible}(f, \Gamma)}{\Gamma \vdash e.f : T} \quad \text{(T-Field)}$$

**Nominal Equivalence**

Record types have nominal equivalence. Two record types are equivalent if and only if they refer to the same declaration. Records with identical structure but different names are distinct types:

```cursive
record Point2D { x: f64, y: f64 }
record Vector2D { x: f64, y: f64 }
// Point2D and Vector2D are NOT equivalent despite identical structure
```

**Field Visibility**

The default visibility for record fields is `private` (accessible only within the declaring module). Each field MAY be annotated with an explicit visibility modifier:

| Visibility  | Accessible From                             |
| :---------- | :------------------------------------------ |
| `private`   | Declaring module only (default)             |
| `internal`  | Declaring package only                      |
| `protected` | Declaring type and its implementations only |
| `public`    | Any module that can access the record type  |

Field visibility MUST NOT exceed the visibility of the enclosing record type.

**Form Implementation**

A record declaration MAY specify the forms it implements using the subtype operator (`<:`) followed by a comma-separated list of form identifiers. The record body MUST provide implementations for all required form procedures. Form implementation semantics are defined in §6.3.

**Invariant Enforcement**

When a record declares a type invariant via a `where` clause, the implementation MUST verify that the invariant holds at the following boundary points:

1. **Post-Construction:** Immediately after any constructor or literal initialization completes.
2. **Pre-Call (Public):** Before any `public` procedure is invoked on the instance.
3. **Post-Call (Mutating):** Before any procedure taking `unique` access to `self` returns.

The invariant predicate is implicitly conjoined with the postconditions of all mutating procedures. Invariant enforcement modes and verification strategies are defined in §7.3.

**Permission Propagation**

The permission of a record value propagates to field access. If binding `r` has permission $P$, then `r.f` has permission $P$ for all accessible fields $f$.

**Subtyping**

Record types do not participate in structural subtyping. A record type $R$ is a subtype of a form type $\textit{Tr}$ if and only if $R$ implements $\textit{Tr}$:

$$\frac{R\ \texttt{<:}\ \textit{Tr}}{\Gamma \vdash R <: \textit{Tr}} \quad \text{(Sub-Record-Form)}$$

**Parallel Field Access**

Record fields with disjoint paths may be accessed in parallel through `shared` references.
The key system automatically acquires fine-grained keys to individual fields, enabling
parallel access without explicit synchronization:

```cursive
let player: shared Player = ...

parallel {
    spawn { player.health = 100 }   // Key to player.health
    spawn { player.mana = 50 }      // Key to player.mana — parallel
}
```

When fields are moved into parallel tasks, the compiler tracks the partial move state
and restores the parent record when all fields return to their original scope.

##### Memory & Layout

**Representation**

A record is laid out as a contiguous sequence of its field values. Each field occupies storage sufficient for its type, plus any padding required for alignment.

**Field Ordering**

By default, the physical ordering of fields in memory is **implementation-defined (IDB)**. Implementations MAY reorder fields to minimize padding or improve cache locality. Implementations MUST document their default field ordering strategy in the Conformance Dossier.

**Size and Alignment**

$$\text{alignof}(R) = \max_{f \in \text{fields}(R)}(\text{alignof}(T_f))$$

$$\text{sizeof}(R) \geq \sum_{f \in \text{fields}(R)} \text{sizeof}(T_f)$$

The actual size includes implementation-defined padding to satisfy alignment constraints.

**Padding**

The amount and placement of inter-field padding is **implementation-defined (IDB)**. The values of padding bytes are **unspecified behavior (USB)**.

**C-Compatible Layout**

When the `[[layout(C)]]` attribute is applied to a record declaration, the implementation MUST produce a C-compatible memory layout:

1. Fields MUST be stored in declaration order.
2. Padding MUST be inserted to satisfy alignment requirements of subsequent fields.
3. The total size MUST be a multiple of the record's alignment.
4. The layout MUST conform to the target platform's C ABI.

**Zero-Field Records**

A record with no fields has size 0 and alignment 1:

$$\text{sizeof}(\texttt{record}\ R\ \{\}) = 0 \qquad \text{alignof}(\texttt{record}\ R\ \{\}) = 1$$

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to record types and values:

1. A record declaration MUST contain zero or more field declarations; duplicate field names within a single record are forbidden.
2. A record literal MUST provide initializers for all fields of the record type; missing or duplicate field initializers are forbidden.
3. A field access expression MUST name a field that exists and is visible in the current scope.
4. A record field visibility MUST NOT exceed the visibility of the enclosing record.
5. A partition name MUST be unique within its enclosing record declaration.
6. A field MUST NOT appear in more than one explicit partition block.

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-TYP-1901` | Error    | Duplicate field name in record declaration.     | Compile-time | Rejection |
| `E-TYP-1902` | Error    | Missing field initializer in record literal.    | Compile-time | Rejection |
| `E-TYP-1903` | Error    | Duplicate field initializer in record literal.  | Compile-time | Rejection |
| `E-TYP-1904` | Error    | Access to nonexistent field.                    | Compile-time | Rejection |
| `E-TYP-1905` | Error    | Access to field not visible in current scope.   | Compile-time | Rejection |
| `E-TYP-1906` | Error    | Field visibility exceeds record visibility.     | Compile-time | Rejection |
| `E-TYP-1907` | Error    | Duplicate partition name in record declaration. | Compile-time | Rejection |
| `E-TYP-1908` | Error    | Field appears in multiple partition blocks.     | Compile-time | Rejection |
| `P-TYP-1909` | Panic    | Type invariant violated at construction.        | Runtime      | Panic     |
| `P-TYP-1910` | Panic    | Type invariant violated at procedure boundary.  | Runtime      | Panic     |

---

### 5.4 Enums

##### Definition

An **enum** (enumerated type) is a nominal sum type, also known as a tagged union. An enum value is exactly one of several defined **variants**, each of which may carry a payload of associated data. Unlike union types (§4.10), which are structural and anonymous, enums are nominal: two enum types with identical variants but different names are distinct types.

A **variant** is a named alternative within an enum. Each variant is identified by name and may optionally contain associated data in one of three forms: unit-like (no data), tuple-like (positional data), or record-like (named fields).

A **discriminant** is a compile-time-assigned integer value that uniquely identifies each variant within an enum. The discriminant enables runtime determination of which variant is active and is stored alongside the payload in the enum's memory representation.

**Formal Definition**

Let $\mathcal{E}$ denote the set of all enum types. An enum type $E$ is defined by:

$$E = (\text{Name}, \text{Params}, \text{Variants}, \text{Forms}, \text{Invariant})$$

where:

- $\text{Name}$ is the unique type identifier within its declaring module
- $\text{Params}$ is an optional list of generic type parameters
- $\text{Variants} = \{(v_1, D_1, P_1), \ldots, (v_n, D_n, P_n)\}$ is a non-empty sequence of variant declarations, where $v_i$ is the variant name, $D_i$ is the discriminant value, and $P_i$ is the optional payload type
- $\text{Forms}$ is the set of forms the enum implements (possibly empty)
- $\text{Invariant}$ is an optional predicate that constrains valid instances

Two enum types are **equivalent** if and only if they refer to the same declaration:

$$\frac{D(E_1) = D(E_2)}{\Gamma \vdash E_1 \equiv E_2} \quad \text{(T-Equiv-Enum)}$$

##### Syntax & Declaration

**Grammar**

```ebnf
enum_decl        ::= [visibility] "enum" identifier [generic_params]
                     [implements_clause] "{" variant_list "}" [type_invariant]

variant_list     ::= variant ("," variant)* ","?

variant          ::= identifier [variant_payload] ["=" integer_constant]

variant_payload  ::= "(" type_list ")"
                   | "{" field_decl_list "}"

type_list        ::= type ("," type)* ","?

field_decl_list  ::= field_decl ("," field_decl)* ","?

field_decl       ::= identifier ":" type

implements_clause ::= "<:" form_list

form_list       ::= type_path ("," type_path)*

type_invariant   ::= "where" "{" predicate "}"
```

**Variant Forms**

Variants may take one of three forms:

| Form        | Syntax                       | Payload Structure                    |
| :---------- | :--------------------------- | :----------------------------------- |
| Unit-like   | `Variant`                    | No associated data                   |
| Tuple-like  | `Variant(T₁, T₂, ...)`       | Positional fields, accessed by index |
| Record-like | `Variant { f₁: T₁, f₂: T₂ }` | Named fields, accessed by name       |

**Value Construction**

An enum value is constructed by qualifying the variant name with the enum's type path:

```ebnf
enum_literal     ::= type_path "::" identifier [variant_args]

variant_args     ::= "(" expression_list ")"
                   | "{" field_init_list "}"
```

For tuple-like variants, the arguments are positional expressions. For record-like variants, the arguments are named field initializers following the same rules as record literals (§4.8), including support for field shorthand.

**Explicit Discriminant Assignment**

A variant MAY specify an explicit discriminant value using the `=` syntax followed by a compile-time integer constant:

```cursive
enum Status {
    Pending = 0,
    Active = 1,
    Completed = 100
}
```

**Type Invariant**

An enum declaration MAY include a `where` clause after the closing brace to specify an invariant that MUST hold for all valid instances. Within the predicate, the identifier `self` refers to the enum instance being constrained. When the predicate requires access to variant-specific data, it MUST use a `match` expression:

```cursive
enum NonEmpty<T> {
    Single(T),
    Multiple([T])
} where {
    match self {
        NonEmpty::Single(_) => true,
        NonEmpty::Multiple(items) => items.len() > 1
    }
}
```

##### Static Semantics

**Typing Rules**

**(T-Enum-Type)**
An enum type is well-formed when all variant payload types are well-formed and all variant names are distinct:
$$\frac{\forall i \in 1..n,\ \Gamma \vdash P_i\ \text{wf} \quad \forall i \neq j,\ v_i \neq v_j}{\Gamma \vdash \texttt{enum}\ E\ \{v_1(P_1), \ldots, v_n(P_n)\}\ \text{wf}} \quad \text{(T-Enum-WF)}$$

**(T-Enum-Variant-Unit)**
A unit-like variant has the enum type:
$$\frac{E \text{ declares variant } V \text{ with no payload}}{\Gamma \vdash E::V : E} \quad \text{(T-Variant-Unit)}$$

**(T-Enum-Variant-Tuple)**
A tuple-like variant has the enum type when each argument has the corresponding payload type:
$$\frac{E \text{ declares variant } V(T_1, \ldots, T_n) \quad \forall i,\ \Gamma \vdash e_i : T_i}{\Gamma \vdash E::V(e_1, \ldots, e_n) : E} \quad \text{(T-Variant-Tuple)}$$

**(T-Enum-Variant-Record)**
A record-like variant has the enum type when each field initializer has the corresponding field type:
$$\frac{E \text{ declares variant } V\{f_1: T_1, \ldots, f_n: T_n\} \quad \forall i,\ \Gamma \vdash e_i : T_i}{\Gamma \vdash E::V\{f_1: e_1, \ldots, f_n: e_n\} : E} \quad \text{(T-Variant-Record)}$$

**Nominal Equivalence**

Enum types have nominal equivalence. Two enum types are equivalent if and only if they refer to the same declaration. Enums with identical variants but different names are distinct types:

```cursive
enum Result<T, E> { Ok(T), Err(E) }
enum Outcome<T, E> { Ok(T), Err(E) }
// Result<T, E> and Outcome<T, E> are NOT equivalent despite identical structure
```

**Discriminant Assignment**

Discriminant values are assigned as follows:

1. If a variant specifies an explicit discriminant value, that value is assigned to the variant.
2. If a variant does not specify an explicit discriminant and is the first variant, it is assigned the value `0`.
3. If a variant does not specify an explicit discriminant and is not the first variant, it is assigned the value of the previous variant's discriminant plus one.

Discriminant values MUST be unique within an enum. The discriminant type is implementation-defined but MUST be an integer type large enough to represent all discriminant values.

**Discriminant Overflow**

If the implicit discriminant value of a variant would exceed the maximum value of the discriminant type (due to sequential assignment following an explicit discriminant), the program is ill-formed.

**Variant Access**

Accessing the data stored within an enum variant MUST be performed using a `match` expression (§8.4). Direct field access on an enum value is forbidden; the active variant must first be determined through pattern matching.

**Exhaustiveness**

A `match` expression on an enum type MUST be exhaustive: the set of patterns in its arms, taken together, MUST cover every variant of the enum. Exhaustiveness checking is mandatory for all enum types. See §8.4 for pattern matching semantics.

**Form Implementation**

An enum declaration MAY specify the forms it implements using the subtype operator (`<:`) followed by a comma-separated list of form identifiers. The enum body MUST provide implementations for all required form procedures. Form implementation semantics are defined in §6.3.

**Invariant Enforcement**

When an enum declares a type invariant via a `where` clause, the implementation MUST verify that the invariant holds at the following boundary points:

1. **Post-Construction:** Immediately after any variant constructor is invoked.
2. **Pre-Call (Public):** Before any `public` procedure is invoked on the instance.
3. **Post-Call (Mutating):** Before any procedure taking `unique` access to `self` returns.

Invariant enforcement modes and verification strategies are defined in §7.3.

**Subtyping**

Enum types do not participate in structural subtyping. An enum type $E$ is a subtype of a form type $\textit{Tr}$ if and only if $E$ implements $\textit{Tr}$:

$$\frac{E\ \texttt{<:}\ \textit{Tr}}{\Gamma \vdash E <: \textit{Tr}} \quad \text{(Sub-Enum-Form)}$$

##### Memory & Layout

**Representation**

An enum is represented as a discriminant followed by a payload region large enough to hold any variant's data:

$$\text{layout}(E) = \text{Discriminant}\ ||\ \text{Payload}\ ||\ \text{Padding}$$

where $||$ denotes contiguous concatenation.

The **discriminant** is an integer value identifying the active variant. The **payload** is storage for the variant's associated data. The payload region is sized to accommodate the largest variant.

**Size and Alignment**

$$\text{sizeof}(E) = \text{sizeof}(\text{Discriminant}) + \max_{v \in \text{Variants}}(\text{sizeof}(P_v)) + \text{Padding}$$

$$\text{alignof}(E) = \max(\text{alignof}(\text{Discriminant}), \max_{v \in \text{Variants}}(\text{alignof}(P_v)))$$

Padding is inserted as necessary to satisfy alignment requirements.

**Discriminant Size**

For enums without a `[[layout(...)]]` attribute, the discriminant size is implementation-defined but MUST be sufficient to represent all discriminant values. Implementations SHOULD use the smallest integer type that can represent all discriminants.

**Niche Optimization**

Implementations SHOULD apply **niche optimization** when variant payloads contain invalid bit patterns (niches) that can be repurposed to encode the discriminant. When niche optimization is applied, the discriminant storage may be elided entirely.

> **Note:** A common example is `Option<Ptr<T>>`, where the null pointer value (`0x0`) can represent the `None` variant, allowing `Option<Ptr<T>>` to occupy the same space as a raw pointer.

**C-Compatible Layout**

When an enum is annotated with `[[layout(C)]]` or `[[layout(IntType)]]` (e.g., `[[layout(u8)]]`), the implementation MUST use a C-compatible layout:

1. The discriminant MUST be stored first, using the specified integer type (or `i32` for `[[layout(C)]]`).
2. The payload MUST follow the discriminant.
3. Field ordering within record-like variants follows declaration order.

C-compatible enum layout semantics are fully specified in §9.4.

**Layout Guarantees**

| Aspect            | Default Layout         | `[[layout(C)]]` Layout  |
| :---------------- | :--------------------- | :---------------------- |
| Discriminant size | Implementation-defined | Specified type or `i32` |
| Field order       | Unspecified            | Declaration order       |
| Padding           | Implementation-defined | C ABI rules             |

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to enum declarations and values:

1. An enum declaration MUST contain at least one variant.
2. All variant names within an enum MUST be unique.
3. All discriminant values within an enum MUST be unique.
4. Explicit discriminant values MUST be compile-time integer constants.
5. Direct field access on an enum value is forbidden; use `match` to access variant data.
6. An enum type MUST NOT be recursive without indirection (pointer or reference).
7. Visibility of variants MUST NOT exceed the visibility of the enclosing enum type.

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2001` | Error    | Enum declaration contains no variants.                      | Compile-time | Rejection |
| `E-TYP-2002` | Error    | Duplicate variant name in enum declaration.                 | Compile-time | Rejection |
| `E-TYP-2003` | Error    | Duplicate discriminant value in enum declaration.           | Compile-time | Rejection |
| `E-TYP-2004` | Error    | Discriminant value is not a compile-time constant.          | Compile-time | Rejection |
| `E-TYP-2005` | Error    | Direct field access on enum value without pattern matching. | Compile-time | Rejection |
| `E-TYP-2006` | Error    | Infinite type: recursive enum without indirection.          | Compile-time | Rejection |
| `E-TYP-2007` | Error    | Unknown variant name in enum construction.                  | Compile-time | Rejection |
| `E-TYP-2008` | Error    | Variant payload arity mismatch.                             | Compile-time | Rejection |
| `E-TYP-2009` | Error    | Missing field initializer in record-like variant.           | Compile-time | Rejection |
| `E-TYP-2010` | Error    | Discriminant overflow during implicit assignment.           | Compile-time | Rejection |
| `E-PAT-2741` | Error    | `match` expression is not exhaustive for enum type.         | Compile-time | Rejection |
| `P-TYP-2011` | Panic    | Type invariant violated at construction.                    | Runtime      | Panic     |
| `P-TYP-2012` | Panic    | Type invariant violated at procedure boundary.              | Runtime      | Panic     |

---

### 5.5 Union Types

##### Definition

A **union type** is a structural anonymous sum type representing a value that may be one of several distinct types. Union types are written using the pipe operator (`|`) to combine member types. Unlike enums (§4.9), which are nominal and require explicit declaration, union types are structural: two union types are equivalent if they have the same member types, regardless of the order in which those members appear.

A **member type** is one of the component types that constitutes the union. The set of member types defines the possible runtime values that an expression of the union type may hold.

Union types in Cursive are **safe** and **tagged**. Every union value carries a runtime discriminant that identifies the currently active member type, enabling type-safe access through pattern matching.

**Formal Definition**

Let $\mathcal{T}$ denote the universe of all types. A union type $U$ is defined by a non-empty multiset of member types:

$$U = \bigcup_{i=1}^{n} T_i \quad \text{where } n \geq 2 \text{ and } T_i \in \mathcal{T}$$

The union type is denoted syntactically as $T_1 \mid T_2 \mid \cdots \mid T_n$.

The **membership relation** $\in_U$ defines which types are members of a union:

$$T \in_U (T_1 \mid T_2 \mid \cdots \mid T_n) \iff \exists i \in 1..n : T \equiv T_i$$

Two union types are **equivalent** if and only if they have the same member type multisets:

$$\frac{\text{multiset}(\text{members}(U_1)) = \text{multiset}(\text{members}(U_2))}{\Gamma \vdash U_1 \equiv U_2} \quad \text{(T-Equiv-Union)}$$

##### Syntax & Declaration

**Grammar**

```ebnf
union_type       ::= type ("|" type)+

type             ::= union_type
                   | non_union_type

non_union_type   ::= primitive_type
                   | nominal_type
                   | tuple_type
                   | array_type
                   | slice_type
                   | function_type
                   | pointer_type
                   | "(" union_type ")"
```

**Precedence and Associativity**

The union type operator `|` has the following properties:

1. **Precedence:** The `|` operator has lower precedence than function type arrows (`->`), array brackets, and pointer constructors. Parentheses are required to use a union type as a function parameter or return type without ambiguity.

2. **Associativity:** The `|` operator is **non-associative** because union type equivalence is defined over multisets, making associativity semantically irrelevant.

**Parsing Disambiguation**

When a `|` appears in a type position, it MUST be interpreted as the union type operator. The following contexts disambiguate union types:

| Context               | Interpretation               | Example                     |
| :-------------------- | :--------------------------- | :-------------------------- |
| Type annotation       | Union type                   | `let x: i32 \| bool`        |
| Function parameter    | Requires parentheses         | `proc(x: (i32 \| bool))`    |
| Function return       | Requires parentheses         | `-> (i32 \| bool)`          |
| Generic type argument | Union type                   | `Vec<i32 \| string>`        |
| Pattern match binding | Not a type; use member types | `match x { i: i32 => ... }` |

**Value Construction**

A value of union type is constructed by assigning a value of any member type:

```ebnf
union_value      ::= expression
```

No explicit constructor or wrapping syntax is required. When an expression of type $T$ appears in a context expecting a union type $U$ where $T \in_U U$, the value is implicitly wrapped.

##### Static Semantics

**Typing Rules**

**(T-Union-Intro)** Union Introduction:
A value of a member type may be used where a union type is expected:
$$\frac{\Gamma \vdash e : T \quad T \in_U U}{\Gamma \vdash e : U} \quad \text{(T-Union-Intro)}$$

**(T-Union-Member)** Member Type Check:
A type $T$ is a member of union $U = T_1 \mid T_2 \mid \cdots \mid T_n$ if:
$$\frac{\exists i \in 1..n : \Gamma \vdash T \equiv T_i}{\Gamma \vdash T \in_U U} \quad \text{(T-Union-Member)}$$

**(T-Union-WF)** Union Well-Formedness:
A union type is well-formed when all member types are well-formed and there are at least two members:
$$\frac{n \geq 2 \quad \forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash T_1 \mid T_2 \mid \cdots \mid T_n\ \text{wf}} \quad \text{(T-Union-WF)}$$

**(T-Union-Match)** Union Elimination:
A union value MUST be accessed through exhaustive pattern matching:
$$\frac{\Gamma \vdash e : U \quad U = T_1 \mid \cdots \mid T_n \quad \forall i,\ \Gamma, x_i : T_i \vdash e_i : R}{\Gamma \vdash \texttt{match } e\ \{x_1 : T_1 \Rightarrow e_1, \ldots, x_n : T_n \Rightarrow e_n\} : R} \quad \text{(T-Union-Match)}$$

**Type Equivalence**

Union type equivalence follows the rule (T-Equiv-Union) defined above. Equivalence is **set-based and order-independent**:

$$\Gamma \vdash (A \mid B) \equiv (B \mid A)$$

$$\Gamma \vdash (A \mid B \mid C) \equiv (C \mid A \mid B) \equiv (B \mid C \mid A)$$

**Duplicate Handling**

Duplicate member types within a union are semantically distinct entries in the multiset:

$$\Gamma \vdash (A \mid A) \not\equiv A$$

$$\Gamma \vdash (A \mid A \mid B) \not\equiv (A \mid B)$$

> **Note:** While syntactically permitted, duplicate member types are diagnostically discouraged (see `W-TYP-2201`).

**Subtyping**

Union types participate in the subtype relation in two ways:

**(Sub-Union-Width)** A union type is a subtype of another union type if the first's member types form a subset of the second's:
$$\frac{\forall T \in_U U_1 : T \in_U U_2}{\Gamma \vdash U_1 <: U_2} \quad \text{(Sub-Union-Width)}$$

**(Sub-Union-Depth)** A union type is a subtype of another union type if corresponding members are subtypes:
$$\frac{U_1 = T_1 \mid \cdots \mid T_n \quad U_2 = S_1 \mid \cdots \mid S_n \quad \forall i,\ \exists j : \Gamma \vdash T_i <: S_j}{\Gamma \vdash U_1 <: U_2} \quad \text{(Sub-Union-Depth)}$$

**(Sub-Member-Union)** A single type is a subtype of any union containing it as a member:
$$\frac{T \in_U U}{\Gamma \vdash T <: U} \quad \text{(Sub-Member-Union)}$$

**Coercion**

Implicit coercion from a member type to its containing union type occurs at assignment, argument passing, and return, per the coercion rule (T-Coerce) defined in §4.2. This coercion injects the value into the union representation, setting the appropriate discriminant.

**Access Restriction**

Direct field access, method invocation, or operator application on a union value is forbidden. The active member type MUST first be determined through pattern matching:

$$\Gamma \vdash e : U \implies \Gamma \nvdash e.f : T \quad \text{(Union-No-Direct-Access)}$$



**Exhaustiveness Requirement**

A `match` expression over a union type MUST be exhaustive. The set of patterns MUST cover all member types of the union. Exhaustiveness checking is defined in §8.4.

**Nested Unions**

When a union type appears as a member of another union, the nested union is **not** automatically flattened. The types `(A | B) | C` and `A | B | C` are distinct:

$$\Gamma \vdash ((A \mid B) \mid C) \not\equiv (A \mid B \mid C)$$

To match a value of type `(A | B) | C`, the outer union must first be matched, then the inner union if applicable.

##### Memory & Layout

**Representation**

A union value is represented as a discriminant followed by a payload region:

$$\text{layout}(U) = \text{Discriminant}\ ||\ \text{Payload}\ ||\ \text{Padding}$$

where $||$ denotes contiguous concatenation.

The **discriminant** (tag) is an integer value identifying which member type is currently active. The **payload** is storage for the active member's data, sized to accommodate the largest member.

**Size and Alignment**

$$\text{sizeof}(U) = \text{sizeof}(\text{Discriminant}) + \max_{T \in_U U}(\text{sizeof}(T)) + \text{Padding}$$

$$\text{alignof}(U) = \max(\text{alignof}(\text{Discriminant}), \max_{T \in_U U}(\text{alignof}(T)))$$

Padding is inserted as necessary to satisfy alignment requirements.

**Discriminant Encoding**

The discriminant value for each member type is assigned based on the **canonical ordering** of member types. The canonical ordering is determined by:

1. Sorting member types lexicographically by their fully-qualified type name.
2. Assigning discriminant values starting from `0` in sorted order.

This deterministic assignment ensures that equivalent union types have identical discriminant mappings.

**Discriminant Size**

The discriminant size is implementation-defined but MUST be sufficient to represent all member types. Implementations SHOULD use the smallest integer type capable of representing all discriminants:

| Member Count | Minimum Discriminant Size |
| :----------- | :------------------------ |
| 2–256        | 1 byte (`u8`)             |
| 257–65,536   | 2 bytes (`u16`)           |
| > 65,536     | 4 bytes (`u32`)           |

**Niche Optimization**

Implementations SHOULD apply **niche optimization** when a member type contains invalid bit patterns (niches) that can encode discriminant information. When niche optimization is applicable, the discriminant storage may be partially or fully elided.

> **Note:** A common example is `i32 | Ptr<T>`, where the null pointer value (`0x0`) can represent a discriminant, potentially reducing the union's size.

**Layout Guarantees**

| Aspect             | Classification         |
| :----------------- | :--------------------- |
| Discriminant size  | Implementation-defined |
| Member ordering    | Defined (canonical)    |
| Padding            | Implementation-defined |
| Niche optimization | Implementation-defined |

**Zero-Sized Types**

If a union contains zero-sized types (ZSTs) as members, those members occupy no payload space but still require a discriminant value. The union's payload size is determined by the largest non-ZST member.

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to union type declarations and values:

1. A union type MUST have at least two member types. A single type `T | T` where both are equivalent does not satisfy this requirement semantically, though it is syntactically valid and results in a warning.

2. Direct field access, method calls, or operator application on a union value is forbidden without prior pattern matching.

3. A `match` expression over a union type MUST be exhaustive.

4. Union types MUST NOT be recursive without indirection. A type `T | U` where `T` or `U` contains the union type itself is ill-formed unless the recursion passes through a pointer type.

5. Union types MUST NOT appear in `[[layout(C)]]` contexts. C-compatible code requiring sum types MUST use enums with explicit discriminants.

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2201` | Error    | Union type has fewer than two member types.            | Compile-time | Rejection |
| `E-TYP-2202` | Error    | Direct access on union value without pattern matching. | Compile-time | Rejection |
| `E-TYP-2203` | Error    | Infinite type: recursive union without indirection.    | Compile-time | Rejection |
| `E-TYP-2204` | Error    | Union type used in `[[layout(C)]]` context.            | Compile-time | Rejection |
| `E-PAT-2205` | Error    | `match` expression is not exhaustive for union type.   | Compile-time | Rejection |
| `W-TYP-2201` | Warning  | Union type contains duplicate member types.            | Compile-time | N/A       |

#### 5.5.1 Propagation Operator (`?`)

##### Definition

The **propagation operator** (`?`) provides concise syntax for early return of error-like union members. When applied to a union-typed expression, it either unwraps the "success" member or immediately returns the "error" member to the caller.

**Formal Definition**

Let $e$ be an expression of union type $U = T_1 \mid T_2 \mid \cdots \mid T_n$. Let $R$ be the return type of the enclosing procedure, where $R = S_1 \mid S_2 \mid \cdots \mid S_m$ (or $R$ is a single type).

The expression $e?$ is well-formed when:

1. **Exactly one success type exists:** There exists exactly one member $T_s \in U$ such that $T_s \not<: R$ (the success type).
2. **All other members propagate:** For all $T_i \in U$ where $i \neq s$, either $T_i <: R$ or $T_i$ is a member of $R$ (the error types).

The result type of $e?$ is $T_s$ (the success type).

##### Syntax & Declaration

**Grammar**

```ebnf
try_expr ::= postfix_expr "?"
```

The `?` operator is a postfix operator with precedence 2 (highest, same as field access and method calls).

##### Static Semantics

**Typing Rules**

**(T-Try-Union)** Propagation on Union Type:

$$\frac{
    \Gamma \vdash e : U \quad
    U = T_s \mid T_{e_1} \mid \cdots \mid T_{e_k} \quad
    \forall i \in 1..k,\ T_{e_i} <: R \quad
    T_s \not<: R
}{
    \Gamma \vdash e? : T_s
} \quad \text{(T-Try-Union)}$$

Where $R$ is the return type of the enclosing procedure.

**(T-Try-Single-Error)** Common Case (Single Error Type):

For the common pattern $T \mid E$ where $E <: R$:

$$\frac{
    \Gamma \vdash e : T \mid E \quad
    E <: R \quad
    T \not<: R
}{
    \Gamma \vdash e? : T
} \quad \text{(T-Try-Single)}$$

**Success Type Inference**

The success type $T_s$ is determined by:

1. If exactly one member type is NOT a subtype of (or member of) the return type, that type is the success type.
2. If multiple candidates exist, the program is ill-formed (ambiguous propagation).
3. If no candidates exist (all types propagate), the program is ill-formed.

**Propagation Compatibility**

A union member $T_e$ is **propagation-compatible** with return type $R$ when:

| Return Type Form                      | Compatibility Condition                      |
| :------------------------------------ | :------------------------------------------- |
| Single type $S$                       | $T_e <: S$                                   |
| Union type $S_1 \mid \cdots \mid S_m$ | $\exists j : T_e <: S_j$ OR $T_e \equiv S_j$ |

##### Dynamic Semantics

**Evaluation**

The expression $e?$ evaluates as follows:

1. Evaluate $e$ to obtain value $v$ of union type $U$.
2. Determine the active member type $T_v$ of $v$.
3. **If $T_v$ is the success type $T_s$:** Extract the payload and continue with type $T_s$.
4. **If $T_v$ is a propagating type $T_{e_i}$:** Execute `return v` (coerced to return type $R$ if necessary).

**Desugaring**

The expression $e?$ is semantically equivalent to:

```cursive
match e {
    success: T_s => success,
    error: T_e => { return error }
}
```

Where $T_s$ is the success type and $T_e$ ranges over all propagating types.

##### Constraints & Legality

**Negative Constraints**

1. The `?` operator MUST NOT be applied outside a procedure body.
2. The `?` operator MUST NOT be applied to non-union types.
3. The success type MUST be unambiguously determinable.
4. All non-success types MUST be propagation-compatible with the return type.

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2210` | Error    | `?` applied to non-union type.                            | Compile-time | Rejection |
| `E-TYP-2211` | Error    | Ambiguous success type in `?` expression.                 | Compile-time | Rejection |
| `E-TYP-2212` | Error    | Union member not propagation-compatible with return type. | Compile-time | Rejection |
| `E-TYP-2213` | Error    | `?` used outside procedure body.                          | Compile-time | Rejection |
| `E-TYP-2214` | Error    | No success type candidate (all types propagate).          | Compile-time | Rejection |

---

### Rationale

The `?` operator addresses a significant ergonomic gap identified in the review. Without it, every fallible operation requires verbose pattern matching. This addition:

1. **Aligns with ONE_CORRECT_WAY** — Establishes `?` as the canonical error propagation pattern
2. **Supports LLM_FRIENDLY_SYNTAX** — Reduces token count for common operations
3. **Maintains LOCAL_REASONING** — `?` clearly marks potential early-return points
4. **Preserves type safety** — Propagation compatibility is statically verified

---

### 5.7 Volatile Type

##### Definition

**Volatile** is a type for memory-mapped I/O that prevents compiler optimizations
on memory operations. Unlike `Atomic`, `Volatile` provides no memory barriers
or cache coherence guarantees.

**Formal Definition**

```cursive
modal Volatile<T> where T: Copy {
    @Valid { address: usize }
}
```

##### Comparison with Atomic

| Aspect          | `Volatile<T>`    | `Atomic<T>`            |
| --------------- | ---------------- | ---------------------- |
| Purpose         | Hardware I/O     | Thread synchronization |
| Memory barriers | None             | Per ordering parameter |
| Cache coherence | Not assumed      | Assumed                |
| shared access   | Unsafe           | Safe                   |
| Typical use     | Device registers | Shared variables       |

##### Syntax & Declaration

```cursive
impl<T: Copy> Volatile<T> {
    /// Create from raw address. UNSAFE: caller ensures validity.
    unsafe procedure from_address(addr: usize) -> Volatile<T>

    /// Read value. Compiler will not cache, reorder, or elide.
    procedure read(~) -> T

    /// Write value. Compiler will not reorder or elide.
    procedure write(~, value: T)

    /// Modify via read-modify-write (NOT atomic).
    procedure modify(~, f: |T| -> T)
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                         |
| ------------ | -------- | ------------------------------------------------- |
| `E-VOL-6001` | Error    | Volatile type parameter not Copy                  |
| `W-VOL-6002` | Warning  | Volatile used for apparent thread synchronization |

---

### Clause 5 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 5 That MUST NOT Be Redefined Elsewhere:**

| Term             | Section | Description                                    |
| :--------------- | :------ | :--------------------------------------------- |
| Primitive Type   | §5.1    | Built-in scalar type with fixed representation |
| Never Type (`!`) | §5.1    | Bottom type; subtype of all types              |
| Unit Type (`()`) | §5.1    | Zero-sized type with single value              |
| Tuple            | §5.2.1  | Structural product type with positional access |
| Array            | §5.2.2  | Fixed-length homogeneous sequence              |
| Slice            | §5.2.3  | Dynamically-sized view into contiguous storage |
| Range            | §5.2.4  | Half-open interval type                        |
| Record           | §5.3    | Nominal product type with named fields         |
| Enum             | §5.4    | Nominal sum type with variants                 |
| Union Type       | §5.5    | Structural anonymous sum type                  |
| String Type      | §5.6    | Built-in text type with modal states           |
| Volatile Type    | §5.7    | Memory-mapped I/O type without barriers        |

**Terms Referenced from Other Clauses:**

| Term             | Source | Usage in Clause 5                  |
| :--------------- | :----- | :--------------------------------- |
| Permission Types | §4.5   | Applied to all data types          |
| Type Equivalence | §4.1   | Structural vs. nominal rules       |
| Subtyping        | §4.2   | Covariance rules for composites    |
| Variance         | §4.3   | Container type variance            |
| Copy Form        | §9.2   | Required for array repeat literals |
| Drop Form        | §9.3   | Destructor typing                  |

**Terms Deferred to Later Clauses:**

| Term             | Deferred To | Reason                                |
| :--------------- | :---------- | :------------------------------------ |
| Modal Type       | Clause 6    | String is modal; full semantics there |
| Pointer Type     | Clause 6    | Slice internal representation         |
| Pattern Matching | Clause 11   | Destructuring records/enums           |

---

## Clause 6: Behavioral Types

This clause defines types with state, behavior, or indirection: modal types (state machines embedded in the type system), pointer types (safe and raw), and function types (callable signatures). These types share the property of having internal state transitions, lifetime management, or representing callable abstractions.

---

### 6.1 Modal Types

> **Cross-Reference Note:** This section is forward-referenced from §4.2 (modal widening), §6.2 (safe pointer states), and other sections. Modal types are a foundational concept for Cursive's state-tracking capabilities. Readers encountering forward references to this section should consult the definitions below.

##### Definition

A **modal type** is a nominal type that embeds a compile-time-validated state machine into the type system. Modal types statically prevent operations from being performed in an incorrect state, such as dereferencing a null pointer or reading from a closed file. A modal type defines a family of related types:

1. **State-Specific Types (`M@S`)**: Concrete types containing only the data defined in their state's payload. State-specific types do not store a runtime discriminant; the state is tracked statically by the type system.

2. **General Modal Type (`M`)**: A sum type (tagged union) capable of holding a value in any of the declared states. The general type stores the state payload together with a runtime discriminant that identifies the currently active state.

This dual representation enables zero-cost state tracking when the state is statically known while supporting safe, dynamic state inspection via pattern matching when the state has been erased to the general type.

**Formal Definition**

Let $\mathcal{M}$ denote the set of modal type names and let $\text{States}(M)$ denote the finite, non-empty set of state names declared for modal type $M$. For each $M \in \mathcal{M}$ and $S \in \text{States}(M)$:

- $M@S$ denotes the state-specific type for state $S$ of modal $M$.
- $M$ denotes the general modal type.

The type family induced by a modal declaration is:

$$\mathcal{T}_M = \{M\} \cup \{M@S : S \in \text{States}(M)\}$$

A **state transition** is a procedure that consumes a value of one state-specific type and produces a value of another (or the same) state-specific type. Transitions define the edges of the modal state graph:

$$\text{Transitions}(M) \subseteq \text{States}(M) \times \text{States}(M)$$

##### Syntax & Declaration

**Grammar**

```ebnf
modal_decl        ::= [visibility] "modal" identifier [generic_params] "{" state_block+ "}"

state_block       ::= "@" state_name [state_payload] [state_members]

state_name        ::= identifier

state_payload     ::= "{" (field_decl ("," field_decl)* ","?)? "}"

state_members     ::= "{" (method_def | transition_def)* "}"

method_def        ::= "procedure" identifier "(" param_list ")" ["->" return_type] block

transition_def    ::= "transition" identifier "(" param_list ")" "->" "@" target_state block

target_state      ::= identifier
```

Methods and transitions MUST be defined inline within their state block. Separate declaration and definition is not supported.

**State-Specific Type Syntax**

A state-specific type is written as the modal type name followed by `@` and the state name:

```ebnf
state_specific_type ::= modal_type_name "@" state_name
```

**Syntactic Constraints**

The following syntactic constraints apply to modal declarations:

1. A `modal` declaration MUST contain at least one state block.
2. All state names within a single `modal` declaration MUST be unique.
3. State names MUST NOT collide with the modal type name itself.
4. Within `state_members`, the `self` parameter type is implicitly the enclosing state-specific type.

**Transition Resolution**

A `transition` defined in state `@S` targeting `@T` has:
- Implicit receiver type: `Self` = `M@S` (the enclosing state)
- Return type: `M@T` (the target state-specific type)

The transition `connect` in `@Disconnected` targeting `@Connecting` resolves to the function type: `(unique Connection@Disconnected, Duration) → Connection@Connecting`

##### Static Semantics

**Well-Formedness**

A modal type declaration is well-formed when:

$$\frac{n \geq 1 \quad \forall i \in 1..n,\ S_i \text{ unique} \quad \forall i,\ \text{Payload}(S_i) \text{ wf}}{\Gamma \vdash \texttt{modal } M\ \{@S_1\ \ldots\ @S_n\}\ \text{wf}} \quad \text{(Modal-WF)}$$

**State-Specific Type Formation**

A state-specific type $M@S$ is well-formed when $S$ is a declared state of modal $M$:

$$\frac{S \in \text{States}(M)}{\Gamma \vdash M@S\ \text{wf}} \quad \text{(State-Specific-WF)}$$

**Typing Rules**

**(T-Modal-State-Intro)** State-Specific Value Construction:

A state-specific value is constructed by providing values for all payload fields:

$$\frac{M@S \text{ has payload fields } f_1 : T_1, \ldots, f_k : T_k \quad \forall i,\ \Gamma \vdash e_i : T_i}{\Gamma \vdash M@S\ \{f_1: e_1, \ldots, f_k: e_k\} : M@S} \quad \text{(T-Modal-State-Intro)}$$

**(T-Modal-Field)** State Payload Field Access:

Payload fields are accessible only when the binding has the corresponding state-specific type:

$$\frac{\Gamma \vdash e : M@S \quad f \in \text{Payload}(S) \quad \text{Payload}(S).f : T}{\Gamma \vdash e.f : T} \quad \text{(T-Modal-Field)}$$

Field access through a general modal type $M$ or a different state-specific type $M@S'$ where $S' \neq S$ is ill-formed.

**(T-Modal-Method)** State-Specific Method Invocation:

A method declared in state block `@S` is callable only on bindings of type `M@S`:

$$\frac{\Gamma \vdash e : M@S \quad m \in \text{Methods}(S) \quad m : (M@S, T_1, \ldots, T_n) \to R}{\Gamma \vdash e.m(a_1, \ldots, a_n) : R} \quad \text{(T-Modal-Method)}$$

**Incomparability**

Two distinct state-specific types of the same modal type are **incomparable**. Neither is a subtype of the other:

$$\frac{S_A \neq S_B}{\Gamma \vdash M@S_A \not<: M@S_B \quad \land \quad \Gamma \vdash M@S_B \not<: M@S_A} \quad \text{(Modal-Incomparable)}$$

This rule prohibits implicit conversion between states; state changes MUST occur through explicit transition procedures.

**Modal Widening**

Modal widening converts a state-specific type `M@S` to the general modal type `M`. This section describes the general widening mechanism; see **Niche-Induced Layout-Compatible Modal Subtyping** below for an exception that permits implicit widening for certain modal types.

For modal types that are **not** niche-layout-compatible, widening is **explicit only**; the `widen` keyword MUST be used at every widening site:

$$\frac{\Gamma \vdash e : M@S \quad S \in \text{States}(M)}{\Gamma \vdash \texttt{widen } e : M} \quad \text{(T-Modal-Widen)}$$

**No Implicit Subtyping (Non-Niche-Layout-Compatible):** For modal types that are not niche-layout-compatible, state-specific types are NOT subtypes of general modal types. The following does NOT hold:

$$M@S \not<: M \quad \text{(when } M@S \text{ is not niche-layout-compatible with } M \text{)}$$

For such types, implicit coercion from `M@S` to `M` is prohibited. The `widen` keyword MUST appear at every site where a state-specific value is used in a context expecting the general modal type. The converse also does not hold: the general type `M` is not a subtype of any state-specific type `M@S`.

**Widen Expression Constraints:**

1. The operand of `widen` MUST have a state-specific modal type `M@S` (diagnostic `E-TYP-2071` if non-modal).
2. The result type is the general modal type `M`.
3. Applying `widen` to an already-general modal type is ill-formed (diagnostic `E-TYP-2072`).
4. For non-niche-layout-compatible modal types, the `widen` keyword is required regardless of payload size. For niche-layout-compatible types, `widen` is optional (see below).

**Usage Contexts:**

| Context             | Syntax                                  |
| ------------------- | --------------------------------------- |
| Variable binding    | `let conn: Connection = widen specific` |
| Procedure argument  | `process_connection(widen specific)`    |
| Return value        | `result widen specific`                 |
| Expression position | `match widen specific { ... }`          |

**Niche-Induced Layout-Compatible Modal Subtyping**

A state-specific type $M@S$ is **niche-layout-compatible** with general type $M$ when:

1. **Niche Optimization Applies:** The implementation applies niche optimization to modal type $M$ (per §4.9 Niche Optimization).
2. **Layout Compatibility:** $\text{sizeof}(M@S) = \text{sizeof}(M)$ and the representation of $M@S$ is a valid representation of $M$.

For niche-layout-compatible state-specific types, implicit subtyping is permitted:

$$\frac{M@S \text{ is niche-layout-compatible with } M}{\Gamma \vdash M@S <: M} \quad \text{(Sub-NicheLayout)}$$

The `widen` keyword is optional for niche-layout-compatible types. It MAY be used for explicitness but is not required.

For the complete specification of niche-layout-compatibility criteria, storage costs, optimization requirements, and detailed examples comparing niche-layout-compatible types (e.g., `Ptr<T>`) with non-compatible types (e.g., `Connection`), see **Modal Widening Operation** under Dynamic Semantics below.

**Transition Typing**

A transition procedure consumes a value of the source state and produces a value of the target state:

$$\frac{\Gamma \vdash e_{\text{self}} : P_{\text{src}}\ M@S_{\text{src}} \quad (S_{\text{src}}, S_{\text{tgt}}) \in \text{Transitions}(M) \quad \forall i,\ \Gamma \vdash a_i : T_i}{\Gamma \vdash e_{\text{self}}.t(a_1, \ldots, a_n) : M@S_{\text{tgt}}} \quad \text{(T-Modal-Transition)}$$

where $P_{\text{src}}$ is the permission qualifier on the receiver (typically `unique` for consuming transitions).

The implementation of a transition procedure MUST return a value of exactly the declared target state-specific type:

$$\frac{\Gamma \vdash \text{body} : M@S_{\text{tgt}}}{\text{transition } M\text{::}t(\ldots) \to M@S_{\text{tgt}}\ \{\text{body}\}\ \text{well-typed}}$$

**Visibility of Payload Fields**

Fields declared within a state payload are implicitly `protected`. They are accessible only from:

1. Procedures declared within the same modal type's state blocks.
2. Transition implementations for the modal type.
3. Associated form implementations for the modal type.

External code MUST NOT directly access payload fields; access is mediated through methods and pattern matching.

##### Dynamic Semantics

**State Transition Evaluation**

When a transition procedure is invoked:

1. The receiver value (in the source state) is consumed.
2. The transition body executes, constructing a new value of the target state.
3. The resulting value is returned with the target state-specific type.

After a consuming transition, the original binding is statically invalid; the source state's data has been moved into the transition.

**Pattern Matching**

Pattern matching provides the mechanism for safely narrowing a general modal type to a state-specific type.

**(Match on General Type)** When the scrutinee has general modal type $M$:

1. The runtime discriminant (or niche encoding) is inspected to determine the active state.
2. Control transfers to the arm matching the active state.
3. Within that arm, the bound variable has the corresponding state-specific type.

The match MUST be exhaustive: all declared states MUST be covered by match arms.

**(Match on State-Specific Type)** When the scrutinee has state-specific type $M@S$:

The state is statically known. A match expression on $M@S$ is treated as irrefutable payload destructuring, analogous to matching on a record type. Coverage of other states is neither required nor permitted.

**Pattern Syntax**

```ebnf
modal_pattern     ::= "@" state_name ["{" payload_pattern "}"]

payload_pattern   ::= (field_name [":" pattern] ("," field_name [":" pattern])* ","?)?
```

When a field pattern omits the `: pattern` suffix, the field name binds a variable of the field's type.

##### Memory & Layout

**State-Specific Type Layout**

The layout of a state-specific type $M@S$ is equivalent to a `record` containing the fields declared in the payload of state $@S$:

$$\text{layout}(M@S) \equiv \text{layout}(\texttt{record}\ \{\text{Payload}(S)\})$$

If the state payload is empty, the state-specific type is a zero-sized type (ZST):

$$\text{Payload}(S) = \emptyset \implies \text{sizeof}(M@S) = 0$$

State-specific types carry no runtime discriminant; the type system tracks the state statically.

**General Modal Type Layout**

The layout of the general modal type $M$ is equivalent to a tagged union (`enum`) where each variant corresponds to a declared state:

$$\text{layout}(M) \equiv \text{layout}(\texttt{enum}\ \{S_1(\text{Payload}(S_1)),\ \ldots,\ S_n(\text{Payload}(S_n))\})$$

The general type stores:

1. A **discriminant** (tag) identifying the currently active state.
2. A **payload region** sized to accommodate the largest state payload.
3. **Padding** as required for alignment.

$$\text{sizeof}(M) = \text{sizeof}(\text{Discriminant}) + \max_{S \in \text{States}(M)}(\text{sizeof}(M@S)) + \text{Padding}$$

$$\text{alignof}(M) = \max(\text{alignof}(\text{Discriminant}),\ \max_{S \in \text{States}(M)}(\text{alignof}(M@S)))$$

**Niche Optimization (Normative)**

Implementations MUST apply **niche optimization** when any state payload contains invalid bit patterns that can unambiguously distinguish between states.

When niche optimization is applicable:

1. The separate discriminant storage MAY be elided partially or entirely.
2. State identification is performed by inspecting the payload bits directly.
3. The resulting general type MUST have the same size as the largest state-specific type (not larger).

> **Rationale:** Niche optimization is mandatory (not merely recommended) to ensure that common modal types like `Ptr<T>` achieve their intended zero-overhead representation. Without this guarantee, programs could not rely on `Ptr<T>` having pointer-sized layout.

**Normative Example:** For the built-in `Ptr<T>` type:

- `@Null` state is represented by address `0x0`.
- `@Valid` state is represented by non-zero addresses.

The general `Ptr<T>` type MUST occupy one machine word (the size of `Ptr<T>@Valid`), NOT one machine word plus a discriminant byte. The null address serves as an implicit discriminant.

**Modal Widening Operation**

Modal widening converts a state-specific type $M@S$ to the general modal type $M$ via the **explicit `widen` keyword**. This section specifies the semantic operation, storage implications, and optimization requirements.

**Semantic Operation**

When `widen e` is evaluated where $e : M@S$:

1. Storage for the general representation is allocated (stack or destination).
2. The payload from $e$ is moved into the corresponding storage region of the general representation.
3. The discriminant (or niche encoding) for state $@S$ is written.
4. The resulting value has type $M$.

**Storage Cost Classification**

| Expression                | Result Type | Storage Change | Runtime Cost                                 |
| :------------------------ | :---------- | :------------- | :------------------------------------------- |
| `e : M@S`                 | `M@S`       | None           | Zero (no conversion)                         |
| `widen e` where `e : M@S` | `M`         | May increase   | Payload copy + discriminant write            |
| `match e` where `e : M`   | `M@S`       | N/A            | Requires pattern match (narrowing)           |
| `M@S₁` to `M@S₂`          | N/A         | N/A            | Requires explicit transition (not permitted) |

**Size Relationships:**

$\text{sizeof}(M@S) \leq \text{sizeof}(M)$

The inequality is strict when:
- Multiple states exist with different payload sizes, AND
- Niche optimization does not fully eliminate the discriminant

The inequality is an equality when:
- Only one state exists, OR
- Niche optimization fully absorbs the discriminant into payload bits

**Widening Cost Formula:**

$\text{Cost}_{\text{widen}}(M@S \to M) = \begin{cases}
0 & \text{if } \text{sizeof}(M@S) = \text{sizeof}(M) \text{ and layout-compatible} \\
O(\text{sizeof}(M@S)) & \text{otherwise (payload copy + discriminant write)}
\end{cases}$

**Optimization Requirements**

Implementations MUST apply the following optimizations to `widen` expressions:

1. **In-Place Widening at Call Sites:** When `widen e` appears as a procedure argument and the source binding is not used after the call, the implementation SHOULD perform widening in-place at the callee's stack frame rather than creating an intermediate copy in the caller.

2. **Elision for Niche-Optimized Types:** When niche optimization results in $\text{sizeof}(M) = \text{sizeof}(M@S)$ and layout compatibility, the `widen` operation MUST be a no-op (zero cost). The discriminant is implicit in the payload bits.

3. **Return Value Optimization:** When `result widen e` appears in a procedure, the implementation SHOULD construct the widened representation directly in the return slot.

**Example — Widening Costs:**

```cursive
modal Connection {
    @Disconnected { }                     // sizeof = 0 (ZST)
    @Connecting { timeout: Duration }     // sizeof = 8
    @Connected { socket: Socket }         // sizeof = 16
}

// sizeof(Connection) = 1 (discriminant) + 16 (max payload) + padding
// sizeof(Connection@Disconnected) = 0
// sizeof(Connection@Connecting) = 8
// sizeof(Connection@Connected) = 16

let c1: Connection@Disconnected = Connection@Disconnected {}
let c2: Connection = widen c1  // Widening: 0 bytes copied, discriminant written
                               // Cost: O(1) — discriminant write only

let c3: Connection@Connected = establish_connection()
let c4: Connection = widen c3  // Widening: 16 bytes copied, discriminant written
                               // Cost: O(sizeof(Socket))
```

**Example — Niche-Layout-Compatible Type (Zero-Cost Implicit Widening):**

```cursive
modal Ptr<T> {
    @Null { }                    // sizeof = 0, represented by address 0x0
    @Valid { address: usize }    // sizeof = 8 (pointer-sized)
}

// sizeof(Ptr<T>) = 8 (niche optimization: null address is discriminant)
// sizeof(Ptr<T>@Null) = 0
// sizeof(Ptr<T>@Valid) = 8
// Ptr<T> is niche-layout-compatible — implicit widening permitted

let p1: Ptr<Buffer>@Valid = allocate_buffer()
let p2: Ptr<Buffer> = p1        // Implicit widening: ZERO COST (niche-layout-compatible)
                                // No copy, no discriminant write — layout identical
                                // `widen` keyword is optional

let p3: Ptr<Buffer> = widen p1  // Explicit `widen` also permitted (for clarity)
```

**Narrowing via Pattern Match**

The converse of widening—narrowing from $M$ to $M@S$—is **not** an implicit coercion. Narrowing requires explicit pattern matching, which performs a runtime discriminant check:

```cursive
let conn: Connection = get_connection()

// Narrowing requires pattern match
match conn {
    @Connected { socket } => {
        // Here: socket has type Socket
        // Type of matched value: Connection@Connected
        socket.send(data)
    }
    @Connecting { timeout } => { /* ... */ }
    @Disconnected => { /* ... */ }
}
```

**Diagnostic Table Addition:**

| Code         | Severity | Condition                                                                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-2070` | Error    | Implicit modal widening on non-niche-layout-compatible type (missing `widen` keyword) | Compile-time | Rejection |
| `E-TYP-2071` | Error    | `widen` applied to non-modal type                                                     | Compile-time | Rejection |
| `E-TYP-2072` | Error    | `widen` applied to already-general modal type                                         | Compile-time | Rejection |
| `W-OPT-4010` | Warning  | Modal widening involves large payload copy (> threshold)                              | Compile-time | Advisory  |

**Error Message Format (E-TYP-2070):**

```
error[E-TYP-2070]: implicit modal widening prohibited

  --> src/main.cursive:15:5
   |
15 |     let conn: Connection = specific_conn
   |                            ^^^^^^^^^^^^^

   State-specific type `Connection@Connected` cannot implicitly convert to
   general type `Connection` because `Connection` is not niche-layout-compatible.

   Use explicit `widen` to perform this conversion:

       let conn: Connection = widen specific_conn

   note: Implicit widening is only permitted for niche-layout-compatible modal
         types such as `Ptr<T>`. See §6.1 for details.
```

> **Note:** The threshold for `W-OPT-4010` is Implementation-Defined but SHOULD be configurable. A reasonable default is 256 bytes.

**Layout Classification**

| Aspect                 | Classification                 |
| :--------------------- | :----------------------------- |
| State-specific layout  | Defined (equivalent to record) |
| General type layout    | Defined (equivalent to enum)   |
| Discriminant size      | Implementation-Defined         |
| Niche optimization     | MUST apply when applicable     |
| Payload field ordering | Implementation-Defined         |
| Padding                | Implementation-Defined         |

##### Constraints & Legality

**Structural Constraints**

The following structural constraints apply to modal type declarations:

1. A modal type MUST declare at least one state.
2. State names within a modal type MUST be unique.

**Access Constraints**

The following access constraints apply to modal values:

1. Payload field access on a value of general modal type $M$ is forbidden; the state MUST first be determined via pattern matching.
2. Payload field access on a state-specific type $M@S$ for a field not in state $@S$'s payload is forbidden.
3. Method invocation on a value of general modal type $M$ is forbidden for methods declared in specific states; the state MUST first be determined via pattern matching.
4. Method invocation on a state-specific type $M@S$ for a method not declared in state $@S$ is forbidden.

**Match Constraints**

1. A `match` expression on a general modal type $M$ MUST be exhaustive; all states in $\text{States}(M)$ MUST be covered.
2. A `match` expression on a state-specific type $M@S$ is irrefutable and MUST NOT include arms for other states.

**Transition Constraints**

1. A transition implementation MUST return a value of exactly the declared target state-specific type.
2. A transition MUST be invoked only on a value of the declared source state-specific type.

**Diagnostic Table**

| Code         | Severity | Condition                                                                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-2050` | Error    | Modal type declares zero states.                                                      | Compile-time | Rejection |
| `E-TYP-2051` | Error    | Duplicate state name within modal type declaration.                                   | Compile-time | Rejection |
| `E-TYP-2052` | Error    | Field access for field not present in the current state's payload.                    | Compile-time | Rejection |
| `E-TYP-2053` | Error    | Method invocation for method not available in the current state.                      | Compile-time | Rejection |
| `E-TYP-2055` | Error    | Transition body returns a value not matching the declared target state-specific type. | Compile-time | Rejection |
| `E-TYP-2056` | Error    | Transition invoked on value not of the declared source state-specific type.           | Compile-time | Rejection |
| `E-TYP-2057` | Error    | Direct field or method access on general modal type without pattern matching.         | Compile-time | Rejection |
| `E-TYP-2060` | Error    | Non-exhaustive match on general modal type; missing states must be listed.            | Compile-time | Rejection |

---

### 6.2 String Types

##### Definition

The **`string`** type is a built-in modal type representing sequences of Unicode scalar values encoded as UTF-8. The `string` type has two states:

1. **`string@Managed`**: An owned, heap-allocated, mutable string buffer responsible for its underlying memory allocation.

2. **`string@View`**: A non-owning, immutable slice into string data, referencing either a `string@Managed` buffer or a statically allocated string literal.

All string content MUST be valid UTF-8. The type system enforces this invariant statically for literals and at runtime for slicing operations.

**Formal Definition**

The `string` type is defined as a member of the modal type family (§4.11):

$$\text{States}(\texttt{string}) = \{\ \texttt{@Managed},\ \texttt{@View}\ \}$$

The type family induced by the `string` modal type is:

$$\mathcal{T}_{\texttt{string}} = \{\ \texttt{string},\ \texttt{string@Managed},\ \texttt{string@View}\ \}$$

As established by the modal incomparability rule (§4.11), the state-specific types `string@Managed` and `string@View` are incomparable:

$$\Gamma \nvdash \texttt{string@Managed} <: \texttt{string@View} \qquad \Gamma \nvdash \texttt{string@View} <: \texttt{string@Managed}$$

Conversion between states requires explicit operations (see Dynamic Semantics).

##### Syntax & Declaration

**Conceptual Modal Declaration** `[INFORMATIVE]`

The `string` type is a built-in primitive; it is not user-declarable. The following declaration illustrates its conceptual structure but is not valid Cursive source code:

```cursive
// Conceptual built-in declaration (not valid user code)
modal string {
    @Managed {
        pointer: Ptr<u8>@Valid,
        length: usize,
        capacity: usize,
    } {
        // Creates an immutable view of this managed string's data.
        procedure as_view(self: const Self) -> string@View {
            result string@View {
                pointer: self.pointer,
                length: self.length,
            }
        }
    }

    @View {
        pointer: Ptr<const u8>@Valid,
        length: usize,
    }
}
```

**Grammar**

```ebnf
string_type         ::= "string" ["@" string_state]

string_state        ::= "Managed" | "View"
```

String literal syntax and escape sequences are defined in §2.4.2.

##### Static Semantics

**Typing Rules**

**(T-String-Literal)** String Literal Typing:

A string literal has type `string@View`. The literal content MUST be valid UTF-8:

$$\frac{\Gamma \vdash s \text{ is a valid string literal}}{\Gamma \vdash s : \texttt{string@View}} \quad \text{(T-String-Literal)}$$

**(T-String-Slice)** Slice Typing:

A slice operation on any string type produces a `string@View`:

$$\frac{\Gamma \vdash e : \texttt{string@}S \quad S \in \{\texttt{Managed}, \texttt{View}\} \quad \Gamma \vdash a : \texttt{usize} \quad \Gamma \vdash b : \texttt{usize}}{\Gamma \vdash e[a..b] : \texttt{string@View}} \quad \text{(T-String-Slice)}$$

**Modal Widening**

Both state-specific types are subtypes of the general modal type `string` per the modal widening rule (§4.11):

$$\frac{S \in \{\texttt{@Managed}, \texttt{@View}\}}{\Gamma \vdash \texttt{string@}S <: \texttt{string}} \quad \text{(Sub-Modal-Widen)}$$

A procedure accepting the general type `string` may receive either state; the active state is determined via pattern matching.

**Form Implementations**

| State            | `Copy` | `Clone` | `Drop` |
| :--------------- | :----- | :------ | :----- |
| `string@Managed` | No     | No      | Yes    |
| `string@View`    | Yes    | Yes     | No     |

The following constraints apply per §6.7:

1. `string@Managed` MUST NOT implement `Copy` because it owns a heap allocation.
2. `string@Managed` MUST NOT implement `Clone` because cloning requires heap allocation; duplication requires explicit `clone_with` (see Dynamic Semantics).
3. `string@Managed` MUST implement `Drop` to deallocate its buffer when responsibility ends.
4. `string@View` MUST implement `Copy` because it is a non-owning pointer-length pair.
5. `string@View` MUST implement `Clone` (implied by `Copy` per §6.7).

##### Dynamic Semantics

**String Literal Evaluation**

When a string literal is evaluated:

1. The implementation allocates the literal content in static, read-only memory during compilation.
2. At runtime, a `string@View` value is constructed containing a pointer to this static memory and the byte length.

String literals have static storage duration; their backing memory is never deallocated.

**State Conversion**

Conversion between `string@Managed` and `string@View` requires explicit operations:

**(Managed to View)** The `as_view` method creates an immutable view of a managed string's buffer:

```cursive
procedure as_view(self: const string@Managed) -> string@View
```

This operation has complexity $O(1)$. The resulting view borrows from the managed string; the view's lifetime MUST NOT exceed the managed string's lifetime.

**(View to Managed)** The `to_managed` method creates a new managed string by copying a view's data:

```cursive
procedure to_managed(self: const string@View, heap: witness HeapAllocator) -> string@Managed
```

This operation has complexity $O(n)$ where $n$ is the view's byte length.

**`string@Managed` Operations**

Operations on `string@Managed` that may allocate or reallocate the buffer MUST receive a `HeapAllocator` capability parameter (§10.5):

- **Construction:** `string::from(source: string@View, heap: witness HeapAllocator): string@Managed`
- **Mutation:** `append(self: unique, data: string@View, heap: witness HeapAllocator)`
- **Duplication:** `clone_with(self: const, heap: witness HeapAllocator): string@Managed`

> **Rationale:** Explicit allocator parameters enforce the Object-Capability model (§10.1).

**`string@View` Operations**

The `string@View` state provides non-mutating methods:

- `length(self: const): usize` — Returns the byte length.
- `is_empty(self: const): bool` — Returns `true` if the length is zero.
- `chars(self: const): CharIterator` — Returns an iterator over Unicode scalar values.

**Slicing Evaluation**

When a slice expression `s[a..b]` is evaluated:

1. Let `start` be the value of `a`.
2. Let `end` be the value of `b`.
3. If `start > end`, the operation MUST panic (`E-TYP-2153`).
4. If `end > s.length()`, the operation MUST panic (`E-TYP-2154`).
5. If `start` does not fall on a valid UTF-8 character boundary, the operation MUST panic (`E-TYP-2151`).
6. If `end` does not fall on a valid UTF-8 character boundary, the operation MUST panic (`E-TYP-2151`).
7. A new `string@View` is constructed with pointer `s.pointer + start` and length `end - start`.

**UTF-8 Character Boundary Definition**

A byte offset `i` within a UTF-8 string is a **valid character boundary** if and only if one of the following holds:

1. `i == 0` (start of string).
2. `i == length` (end of string).
3. The byte at offset `i` is NOT a UTF-8 continuation byte (the two high bits are NOT `10`).

##### Memory & Layout

**`string@Managed` Representation**

| Field      | Type            | Offset  | Description                            |
| :--------- | :-------------- | :------ | :------------------------------------- |
| `pointer`  | `Ptr<u8>@Valid` | 0       | Pointer to heap-allocated UTF-8 buffer |
| `length`   | `usize`         | 1 word  | Number of bytes of valid content       |
| `capacity` | `usize`         | 2 words | Total allocated buffer size in bytes   |

$$\text{sizeof}(\texttt{string@Managed}) = 3 \times \text{sizeof}(\texttt{usize})$$

$$\text{alignof}(\texttt{string@Managed}) = \text{alignof}(\texttt{usize})$$

**`string@View` Representation**

| Field     | Type                  | Offset | Description                           |
| :-------- | :-------------------- | :----- | :------------------------------------ |
| `pointer` | `Ptr<const u8>@Valid` | 0      | Pointer to the first byte of the view |
| `length`  | `usize`               | 1 word | Number of bytes in the view           |

$$\text{sizeof}(\texttt{string@View}) = 2 \times \text{sizeof}(\texttt{usize})$$

$$\text{alignof}(\texttt{string@View}) = \text{alignof}(\texttt{usize})$$

**General `string` Type Representation**

The general modal type `string` uses standard modal type layout (§4.11): a discriminant plus a payload region sized to the largest state.

**Storage Characteristics**

- String content is NOT null-terminated; length is tracked explicitly.
- All content MUST be valid UTF-8.
- The `pointer` field of `string@View` for string literals points into static, read-only memory.
- The `pointer` field of `string@Managed` points into heap-allocated memory.

**Layout Classification**

| Aspect                  | Classification         |
| :---------------------- | :--------------------- |
| `string@View` size      | Defined (2 words)      |
| `string@Managed` size   | Defined (3 words)      |
| Field ordering          | Defined                |
| Buffer null-termination | Defined (not present)  |
| Content encoding        | Defined (UTF-8)        |
| Pointer alignment       | Implementation-Defined |

##### Constraints & Legality

**Indexing Prohibition**

Direct byte indexing on any string type using the subscript operator is forbidden:

$$\Gamma \vdash e : \texttt{string@}S \implies e[i] \text{ is ill-formed}$$

where `i` is a single index expression (not a range).

> **Rationale:** A byte index into a UTF-8 string does not necessarily correspond to a character boundary. Forbidding `s[i]` prevents accidental mid-character access.

**Slicing Requirements**

1. Slice indices are interpreted as **byte offsets**, not character indices.
2. Both boundaries MUST fall on valid UTF-8 character boundaries.
3. Boundary violations MUST cause a panic at runtime.

**Responsibility Constraints**

The `string@Managed` state follows standard responsibility semantics (§3.5):

1. A `string@Managed` binding MUST have exactly one responsible owner.
2. Transfer of responsibility MUST use explicit `move`.
3. When responsibility ends, the `Drop` implementation deallocates the buffer.

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-TYP-2151` | Error    | Slice boundary not on UTF-8 char boundary.    | Runtime      | Panic     |
| `E-TYP-2152` | Error    | Direct byte indexing (`s[i]`) on string type. | Compile-time | Rejection |
| `E-TYP-2153` | Error    | Slice end index less than start index.        | Runtime      | Panic     |
| `E-TYP-2154` | Error    | Slice index exceeds string length.            | Runtime      | Panic     |

---

### 6.3 Pointer Types

##### Definition

A **pointer type** is a type whose values are memory addresses. Cursive provides two families of pointer types designed for distinct purposes:

1. **Safe Modal Pointers (`Ptr<T>`)**: The primary pointer type for safe code. `Ptr<T>` is a built-in modal type with compile-time-tracked states that statically prevent null-pointer dereferences and use-after-free errors.

2. **Raw Pointers (`*imm T`, `*mut T`)**: Unsafe, C-style pointers providing no safety guarantees. Raw pointers are intended for `unsafe` blocks and Foreign Function Interface (FFI) interoperability.

The safe pointer type integrates with the modal type system (§4.11) to encode pointer validity as a static property. The raw pointer types bypass this system entirely, delegating all safety responsibility to the programmer.

**Formal Definition**

The `Ptr<T>` type is defined as a member of the modal type family (§4.11):

$$\text{States}(\texttt{Ptr<T>}) = \{\ \texttt{@Valid},\ \texttt{@Null},\ \texttt{@Expired}\ \}$$

The type family induced by the `Ptr<T>` modal type is:

$$\mathcal{T}_{\texttt{Ptr<T>}} = \{\ \texttt{Ptr<T>},\ \texttt{Ptr<T>@Valid},\ \texttt{Ptr<T>@Null},\ \texttt{Ptr<T>@Expired}\ \}$$

As established by the modal incomparability rule (§4.11), the state-specific types are pairwise incomparable:

$$\forall S_A, S_B \in \text{States}(\texttt{Ptr<T>}),\ S_A \neq S_B \implies \Gamma \nvdash \texttt{Ptr<T>@}S_A <: \texttt{Ptr<T>@}S_B$$

The raw pointer types form a separate type family:

$$\mathcal{T}_{\text{RawPtr}} = \{\ \texttt{*imm T},\ \texttt{*mut T}\ :\ T \in \mathcal{T}\ \}$$

where $\mathcal{T}$ is the set of all types.

##### Syntax & Declaration

**Conceptual Modal Declaration** `[INFORMATIVE]`

The `Ptr<T>` type is a built-in primitive; it is not user-declarable. The following declaration illustrates its conceptual structure but is not valid Cursive source code:

```cursive
// Conceptual built-in declaration (not valid user code)
modal Ptr<T> {
    // State: The pointer is guaranteed to be non-null
    // and point to live, initialized memory.
    // This is the ONLY state that permits dereferencing.
    @Valid {
        // (Internal representation: non-null address)
    }

    // State: The pointer is guaranteed to be null.
    // This is Cursive's safe, explicit replacement for null pointers.
    @Null {
        // (Internal representation: address 0x0)
    }

    // State: The pointer was valid, but the memory region it
    // pointed to has been deallocated.
    // This state is assigned by the compiler when a region exits.
    @Expired {
        // (Internal representation: formerly-valid address)
    }
}
```

**Grammar**

```ebnf
safe_pointer_type     ::= "Ptr" "<" type ">" ["@" pointer_state]

pointer_state         ::= "Valid" | "Null" | "Expired"

raw_pointer_type      ::= "*" raw_pointer_qual type

raw_pointer_qual      ::= "imm" | "mut"
```

> **Design Note — Vocabulary Distinction:** Raw pointer qualifiers use `imm` (immutable) and `mut` (mutable) rather than the permission keywords `const` and `unique`. This distinction is intentional:
>
> 1. **Semantic Separation:** Permission keywords (`const`, `unique`, `shared`) denote the language's verified aliasing model with compile-time guarantees. Raw pointer qualifiers denote unverified mutability intent with no compiler enforcement.
>
> 2. **FFI Alignment:** The `imm`/`mut` vocabulary aligns with C's `const`/non-`const` pointer semantics, facilitating FFI reasoning without conflating C's shallow `const` with Cursive's deep `const` permission.
>
> 3. **Visual Distinction:** The abbreviated forms `*imm` and `*mut` are visually distinct from permission-qualified types like `const Buffer`, reducing the risk of confusion between safe and unsafe pointer operations.

**Address-Of Operator**

The `&` operator creates a safe pointer from a valid memory location:

```ebnf
address_of_expr       ::= "&" place_expr
```

**Null Pointer Constructor**

A null pointer is created using the built-in constructor:

```ebnf
null_ptr_expr         ::= "Ptr" "::" "null" "()"
```

**Raw Pointer Cast**

A safe pointer in the `@Valid` state may be cast to a raw pointer:

```ebnf
raw_ptr_cast_expr     ::= safe_ptr_expr "as" raw_pointer_type
```

##### Static Semantics

**Pointer States**

The `Ptr<T>` type has three compile-time states that track pointer validity:

| State      | Description                                                                           | Dereferenceable |
| :--------- | :------------------------------------------------------------------------------------ | :-------------- |
| `@Valid`   | The pointer is guaranteed to be non-null and to point to live, accessible memory.     | Yes             |
| `@Null`    | The pointer is guaranteed to be null (address `0x0`).                                 | No              |
| `@Expired` | The pointer was valid but now references deallocated memory (e.g., an exited region). | No              |

**Typing Rules**

**(T-Addr-Of)** Address-Of Operator:

The `&` operator applied to a valid place expression produces a pointer in the `@Valid` state:

$$\frac{\Gamma \vdash e : T \quad e\ \text{is a valid place expression}}{\Gamma \vdash \texttt{\&}e : \texttt{Ptr<T>@Valid}} \quad \text{(T-Addr-Of)}$$

A **place expression** is an expression that denotes a memory location. Place expressions include:
- Variable bindings
- Field access expressions on place expressions
- Indexed access into arrays or slices

**(T-Null-Ptr)** Null Pointer Construction:

The `Ptr::null()` constructor produces a pointer in the `@Null` state:

$$\frac{}{\Gamma \vdash \texttt{Ptr::null()} : \texttt{Ptr<T>@Null}} \quad \text{(T-Null-Ptr)}$$

The type parameter `T` is inferred from context or explicitly annotated.

**(T-Deref)** Dereference Operator:

The dereference operator `*` applied to a safe pointer MUST only be applied to a pointer in the `@Valid` state:

$$\frac{\Gamma \vdash p : \texttt{Ptr<T>@Valid}}{\Gamma \vdash \texttt{*}p : T} \quad \text{(T-Deref)}$$

This rule statically enforces memory safety. An attempt to dereference a pointer in the `@Null` or `@Expired` state is ill-formed.

**(T-Raw-Ptr-Cast)** Raw Pointer Cast:

A safe pointer in the `@Valid` state may be cast to a raw pointer of compatible mutability:

$$\frac{\Gamma \vdash p : \texttt{Ptr<T>@Valid}}{\Gamma \vdash p\ \texttt{as *imm T} : \texttt{*imm T}} \quad \text{(T-Raw-Ptr-Cast-Imm)}$$

$$\frac{\Gamma \vdash p : \texttt{Ptr<T>@Valid}}{\Gamma \vdash p\ \texttt{as *mut T} : \texttt{*mut T}} \quad \text{(T-Raw-Ptr-Cast-Mut)}$$

**(T-Raw-Deref)** Unsafe Raw Pointer Dereference:

Dereferencing a raw pointer is permitted only within an `unsafe` block:

$$\frac{\Gamma \vdash p : \texttt{*imm T} \quad \text{context is } \texttt{unsafe}}{\Gamma \vdash \texttt{*}p : T} \quad \text{(T-Raw-Deref-Imm)}$$

$$\frac{\Gamma \vdash p : \texttt{*mut T} \quad \text{context is } \texttt{unsafe}}{\Gamma \vdash \texttt{*}p : T} \quad \text{(T-Raw-Deref-Mut)}$$

**Modal Subtyping**

The `Ptr<T>` modal type is **niche-layout-compatible**: all runtime-observable states (`@Valid`, `@Null`) can be distinguished via niche encoding (null vs non-null address), and state-specific types share identical layout with the general type. Per the Niche-Induced Layout-Compatible Modal Subtyping rule (§6.1):

$$\frac{S \in \{\texttt{@Valid}, \texttt{@Null}\}}{\Gamma \vdash \texttt{Ptr<T>@}S <: \texttt{Ptr<T>}} \quad \text{(Sub-Ptr-Widen)}$$

Implicit widening is permitted; a procedure accepting `Ptr<T>` may receive any state-specific pointer directly:

```cursive
procedure process_ptr(p: Ptr<Buffer>) { /* ... */ }

let valid_ptr: Ptr<Buffer>@Valid = &buffer
process_ptr(valid_ptr)  // Implicit widening — no `widen` keyword required
```

> **Note:** The `@Expired` state is compile-time only (see Dynamic Semantics below) and cannot be widened to the general type; the compiler statically prevents use of expired pointers before they could reach widening contexts.

**Form Implementations**

| State            | `Copy` | `Clone` | `Drop` |
| :--------------- | :----- | :------ | :----- |
| `Ptr<T>@Valid`   | Yes    | Yes     | No     |
| `Ptr<T>@Null`    | Yes    | Yes     | No     |
| `Ptr<T>@Expired` | Yes    | Yes     | No     |
| `*imm T`         | Yes    | Yes     | No     |
| `*mut T`         | Yes    | Yes     | No     |

The following constraints apply per §6.7:

1. All pointer types (safe and raw) MUST implement `Copy` because they are address values.
2. All pointer types MUST implement `Clone` (implied by `Copy` per §6.7).
3. No pointer type implements `Drop`. Pointers do not own the memory they reference; the referenced data's lifetime is managed by the responsibility system (§3.5) or region system (§3.9).

##### Dynamic Semantics

**Address-Of Evaluation**

When an address-of expression `&e` is evaluated:

1. Let `loc` be the memory location denoted by place expression `e`.
2. Construct a `Ptr<T>@Valid` value containing the address of `loc`.
3. Return this pointer value.

The resulting pointer's validity is tied to the lifetime of the referenced storage. When the storage is deallocated, pointers into that storage transition to the `@Expired` state.

**Null Pointer Evaluation**

When `Ptr::null()` is evaluated:

1. Construct a `Ptr<T>@Null` value containing address `0x0`.
2. Return this pointer value.

**Dereference Evaluation**

When a dereference expression `*p` is evaluated where `p : Ptr<T>@Valid`:

1. Let `addr` be the address stored in `p`.
2. Read the value of type `T` stored at address `addr`.
3. Return this value.

**Region Exit State Transition**

When a region block (§3.9) exits:

1. For every pointer `p : Ptr<T>@Valid` whose referent was allocated in the exiting region:
2. The compiler transitions `p` to state `Ptr<T>@Expired`.
3. Subsequent attempts to dereference `p` are statically rejected.

This state transition is not a runtime operation; it is a compile-time type refinement. The physical pointer value is unchanged, but the type system prevents its use.

> **Note:** The `@Expired` state is reachable only through the region system's compile-time tracking. Safe code cannot construct an `@Expired` pointer directly. The state exists to provide precise error messages distinguishing null pointer access from use-after-free.

**Raw Pointer Dereference Evaluation**

When a raw pointer dereference `*p` is evaluated within an `unsafe` block:

1. Let `addr` be the address stored in `p`.
2. Read the value of type `T` stored at address `addr`.
3. Return this value.

The implementation provides no safety guarantees. If `addr` is null, dangling, misaligned, or points to uninitialized memory, the behavior is Unverifiable Behavior (UVB) per §1.2.

##### Memory & Layout

**Safe Pointer Representation**

The safe pointer type `Ptr<T>` uses niche optimization (§4.11) to achieve a single-word representation:

| State      | Representation                    |
| :--------- | :-------------------------------- |
| `@Valid`   | Non-zero address (pointer to `T`) |
| `@Null`    | Address `0x0`                     |
| `@Expired` | Formerly-valid address            |

$$\text{sizeof}(\texttt{Ptr<T>}) = \text{sizeof}(\texttt{usize})$$

$$\text{alignof}(\texttt{Ptr<T>}) = \text{alignof}(\texttt{usize})$$

The general modal type `Ptr<T>` MUST occupy exactly one machine word. The null address (`0x0`) serves as an implicit discriminant for the `@Null` state. Non-zero addresses indicate either `@Valid` or `@Expired`; the distinction between these states is tracked statically, not at runtime.

> **Rationale:** This representation ensures zero-overhead abstraction. A `Ptr<T>` has identical size and alignment to a C pointer, enabling efficient FFI interoperability when cast to a raw pointer.

**Raw Pointer Representation**

| Type     | Size            | Alignment        |
| :------- | :-------------- | :--------------- |
| `*imm T` | `sizeof(usize)` | `alignof(usize)` |
| `*mut T` | `sizeof(usize)` | `alignof(usize)` |

Raw pointers have identical representation regardless of mutability qualifier. The `imm`/`mut` distinction is a compile-time property only.

**Layout Classification**

| Aspect                  | Classification         |
| :---------------------- | :--------------------- |
| Pointer size            | Implementation-Defined |
| Pointer alignment       | Implementation-Defined |
| `@Null` representation  | Defined (`0x0`)        |
| `@Valid` representation | Defined (non-zero)     |
| Niche optimization      | MUST apply             |

##### Constraints & Legality

**Dereference Constraints**

The following constraints govern pointer dereferencing:

1. The dereference operator `*` applied to `Ptr<T>@Null` is ill-formed.
2. The dereference operator `*` applied to `Ptr<T>@Expired` is ill-formed.
3. The dereference operator `*` applied to a raw pointer (`*imm T` or `*mut T`) outside an `unsafe` block is ill-formed.

**Address-Of Constraints**

The following constraints govern the address-of operator:

1. The operand of `&` MUST be a place expression (a memory location).
2. The operand of `&` MUST be initialized.
3. The resulting pointer MUST NOT escape the lifetime of the referenced storage (see §3.3 Escape Rule).

**Cast Constraints**

The following constraints govern pointer casts:

1. Only `Ptr<T>@Valid` may be cast to a raw pointer.
2. Casting `Ptr<T>@Null` or `Ptr<T>@Expired` to a raw pointer is ill-formed.

**FFI Constraints**

Raw pointers are the only FFI-safe (Clause 12) pointer types:

1. The `Ptr<T>@State` modal type MUST NOT appear in `extern` procedure signatures.
2. Procedures accepting or returning pointers across the FFI boundary MUST use raw pointer types.

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2101` | Error    | Dereference of pointer in `@Null` state.                     | Compile-time | Rejection |
| `E-TYP-2102` | Error    | Dereference of pointer in `@Expired` state (use-after-free). | Compile-time | Rejection |
| `E-TYP-2103` | Error    | Dereference of raw pointer outside `unsafe` block.           | Compile-time | Rejection |
| `E-TYP-2104` | Error    | Address-of operator applied to non-place expression.         | Compile-time | Rejection |
| `E-TYP-2105` | Error    | Cast of non-`@Valid` pointer to raw pointer.                 | Compile-time | Rejection |
| `E-TYP-2106` | Error    | Modal pointer type in `extern` procedure signature.          | Compile-time | Rejection |


---


### 6.4 Function Types

##### Definition

A **function type** is a structural type representing a callable signature. Function types describe the interface of callable entities—procedures and closures—as mappings from parameter types to a return type. A function type is defined by its ordered parameter types (including any `move` responsibility modifiers) and its return type.

Cursive distinguishes two representations of function types based on whether the callable captures state from its environment:

1. **Sparse Function Pointer (`(T) -> U`)**: A direct pointer to executable code. Sparse function pointers occupy a single machine word and are FFI-safe. Non-capturing procedures and non-capturing closure expressions have sparse function pointer types.

2. **Closure (`|T| -> U`)**: A dense pointer containing both a code pointer and an environment pointer. Closures occupy two machine words and are NOT FFI-safe. Closure expressions that capture bindings from their enclosing scope have closure types.

This distinction ensures that the representation cost of each callable form is explicit in the type system and that FFI boundary constraints are enforced statically.

**Formal Definition**

Let $\mathcal{F}$ denote the set of all function types. A function type is defined by the tuple:

$$F = (P_1, \ldots, P_n, R, \kappa)$$

where:
- $P_i = (m_i, T_i)$ is a parameter descriptor consisting of an optional `move` modifier $m_i \in \{\epsilon, \texttt{move}\}$ and a type $T_i$
- $R$ is the return type
- $\kappa \in \{\texttt{sparse}, \texttt{closure}\}$ is the representation kind

The type families are:

$$\mathcal{F}_{\text{sparse}} = \{(m_1\ T_1, \ldots, m_n\ T_n) \to R : \forall i,\ T_i \in \mathcal{T},\ R \in \mathcal{T}\}$$

$$\mathcal{F}_{\text{closure}} = \{|m_1\ T_1, \ldots, m_n\ T_n| \to R : \forall i,\ T_i \in \mathcal{T},\ R \in \mathcal{T}\}$$

where $\mathcal{T}$ is the universe of all types.

**Function Type vs. Procedure Declaration**

A function type is distinct from a procedure declaration. A `procedure` declaration (Clause 7) is a named, top-level item that *has* a corresponding function type. The function type captures the procedure's complete callable interface without its name or contract annotations.

| Aspect                | Procedure Declaration           | Function Type                |
| :-------------------- | :------------------------------ | :--------------------------- |
| Nature                | Named, top-level item           | Structural, anonymous type   |
| Name                  | Yes                             | No                           |
| Contract (`[[...]]`)  | Yes (as metadata, per Clause 7) | No                           |
| Capability parameters | Yes (as parameters)             | Yes (as parameter types)     |
| `move` modifiers      | Yes (on parameters)             | Yes (part of parameter type) |

##### Syntax & Declaration

**Grammar**

```ebnf
function_type         ::= sparse_function_type | closure_type

sparse_function_type  ::= "(" [param_type_list] ")" "->" type

closure_type          ::= "|" [param_type_list] "|" "->" type

param_type_list       ::= param_type ("," param_type)*

param_type            ::= ["move"] type
```

**Syntactic Disambiguation**

The `|` token serves as both a closure type delimiter and the bitwise OR operator. Disambiguation is context-dependent:

1. In type position, `|` followed by a type list and `|` forms a closure type.
2. In expression position, `|` is the bitwise OR operator.
3. The parser resolves ambiguity by syntactic context (type vs. expression position).

**Syntactic Constraints**

The following syntactic constraints apply to function types:

1. The `param_type_list` is a comma-separated sequence of zero or more parameter types. Each parameter type MAY be prefixed with the `move` keyword.
2. Parameter types include capability types (e.g., `FileSystem`, `HeapAllocator`) on equal footing with data types.
3. The `->` token separates the parameter list from the return type.
4. The return type MUST be a single type. Procedures returning no meaningful value use the unit type `()`.
5. Parameter names are not part of function type syntax; only types appear.

**Type Identity**

A function type's identity is determined by the tuple $(P_1, \ldots, P_n, R, \kappa)$ as defined in the Formal Definition above. Specifically:

1. Representation kind (sparse or closure) is part of identity; `(T) -> U` and `|T| -> U` are distinct types.
2. The `move` modifier is part of identity; `(T) -> U` and `(move T) -> U` are distinct types.
3. Contract annotations and parameter names are NOT part of function type identity.

##### Static Semantics

**Well-Formedness (T-Func-WF)**

A function type is well-formed if and only if all its constituent types are well-formed:

$$\frac{\Gamma \vdash R\ \text{wf} \quad \forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash (m_1\ T_1, \ldots, m_n\ T_n) \to R\ \text{wf}} \quad \text{(T-Func-WF)}$$

where $m_i \in \{\epsilon, \texttt{move}\}$ is the optional `move` modifier for parameter $i$.

The same rule applies to closure types:

$$\frac{\Gamma \vdash R\ \text{wf} \quad \forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash |m_1\ T_1, \ldots, m_n\ T_n| \to R\ \text{wf}} \quad \text{(T-Closure-WF)}$$

**Type Equivalence (T-Equiv-Func)**

Function types are structural. Two function types are equivalent if and only if:
- They have the same representation kind.
- They have the same number of parameters.
- Each corresponding parameter pair has equivalent types and matching `move` modifiers.
- Their return types are equivalent.

$$\frac{\begin{gathered}
\kappa_F = \kappa_G \quad n = k \\
\forall i \in 1..n,\ (m_i = m'_i \land \Gamma \vdash T_i \equiv U_i) \\
\Gamma \vdash R_F \equiv R_G
\end{gathered}}{\Gamma \vdash F \equiv G} \quad \text{(T-Equiv-Func)}$$

where $F = (\kappa_F,\ m_1\ T_1, \ldots, m_n\ T_n,\ R_F)$ and $G = (\kappa_G,\ m'_1\ U_1, \ldots, m'_k\ U_k,\ R_G)$.

> **Note:** The types `(move Buffer) -> ()` and `(Buffer) -> ()` are NOT equivalent. The types `(i32) -> bool` and `|i32| -> bool` are NOT equivalent.

**Variance**

Function types exhibit contravariant parameters and covariant return types as defined by rule `(Var-Func)` in §4.3. This variance applies to both sparse function pointer types and closure types independently.

> **Note:** The variance rules in §4.3 govern subtyping relationships arising from type parameter substitution. The subtyping rules below govern relationships arising from representation kind and parameter responsibility modifiers.

**Representation Subtyping (T-Sparse-Sub-Closure)**

A sparse function pointer is a subtype of the corresponding closure type with the same parameter and return types:

$$\frac{\Gamma \vdash (T_1, \ldots, T_n) \to R\ \text{wf}}{\Gamma \vdash (T_1, \ldots, T_n) \to R <: |T_1, \ldots, T_n| \to R} \quad \text{(T-Sparse-Sub-Closure)}$$

This rule permits a non-capturing function to be used where a capturing closure is expected. The implicit coercion constructs a closure value with a null environment pointer (see Memory & Layout). The converse does NOT hold: a closure type is NOT a subtype of a sparse function pointer type.

**Move Modifier Subtyping (T-NonMove-Sub-Move)**

A function type with a non-`move` parameter is a subtype of a function type with a `move` parameter for the same underlying type:

$$\frac{\Gamma \vdash T\ \text{wf}}{\Gamma \vdash (T) \to R <: (\texttt{move } T) \to R} \quad \text{(T-NonMove-Sub-Move)}$$

This rule permits a procedure that does not take responsibility for an argument to substitute for a procedure that does. The caller prepares for the argument's responsibility to be transferred, but the callee declines to accept responsibility, which is always safe. The reverse substitution is NOT permitted: a procedure that requires responsibility transfer cannot substitute for one that does not.

This rule extends to corresponding parameters in multi-parameter function types and applies equally to sparse and closure representations.

**Combined Subtyping**

The subtyping rules compose via transitivity. A sparse non-`move` function is a subtype of a closure `move` function with compatible types:

$$\Gamma \vdash (T) \to R <: |\texttt{move } T| \to R$$

**Typing Rules**

**(T-Proc-As-Value)** Procedure Reference:

When a procedure name appears in value position (not immediately followed by an argument list), the expression has the sparse function type corresponding to the procedure's signature:

$$\frac{\text{procedure } f(m_1\ x_1 : T_1, \ldots, m_n\ x_n : T_n) \to R\ \text{declared}}{\Gamma \vdash f : (m_1\ T_1, \ldots, m_n\ T_n) \to R} \quad \text{(T-Proc-As-Value)}$$

**(T-Closure-Sparse)** Non-Capturing Closure Expression:

A closure expression that captures no bindings from its environment has a sparse function type:

$$\frac{\Gamma \vdash |p_1, \ldots, p_n| \to e : (T_1, \ldots, T_n) \to R \quad \text{captures}(|p_1, \ldots, p_n| \to e) = \emptyset}{\Gamma \vdash |p_1, \ldots, p_n| \to e : (T_1, \ldots, T_n) \to R} \quad \text{(T-Closure-Sparse)}$$

**(T-Closure-Capturing)** Capturing Closure Expression:

A closure expression that captures one or more bindings from its environment has a closure type:

$$\frac{\Gamma \vdash |p_1, \ldots, p_n| \to e : (T_1, \ldots, T_n) \to R \quad \text{captures}(|p_1, \ldots, p_n| \to e) \neq \emptyset}{\Gamma \vdash |p_1, \ldots, p_n| \to e : |T_1, \ldots, T_n| \to R} \quad \text{(T-Closure-Capturing)}$$

> **Note:** Closure expression syntax and capture semantics are defined in Clause 8. This section defines only the typing of the resulting callable values.

**(T-Call)** Function Invocation:

A value of function type may be invoked with arguments matching the parameter types:

$$\frac{\Gamma \vdash f : (m_1\ T_1, \ldots, m_n\ T_n) \to R \quad \forall i \in 1..n,\ \Gamma \vdash a_i : T_i}{\Gamma \vdash f(a_1, \ldots, a_n) : R} \quad \text{(T-Call)}$$

When $m_i = \texttt{move}$, the argument $a_i$ MUST be passed via an explicit `move` expression, and the source binding becomes invalid after the call (per §3.5).

The same rule applies to closure type values.

##### Memory & Layout

**Sparse Function Pointer Representation**

A sparse function pointer is represented as a single machine word containing the address of the callable code:

$$\text{sizeof}((T_1, \ldots, T_n) \to R) = \text{sizeof}(\texttt{usize})$$

$$\text{alignof}((T_1, \ldots, T_n) \to R) = \text{alignof}(\texttt{usize})$$

The representation is a code pointer:

```
┌─────────────────────────────────┐
│          code_ptr               │  ← pointer to executable code
└─────────────────────────────────┘
         (1 machine word)
```

**Closure Representation**

A closure is represented as a two-word structure containing an environment pointer and a code pointer:

$$\text{sizeof}(|T_1, \ldots, T_n| \to R) = 2 \times \text{sizeof}(\texttt{usize})$$

$$\text{alignof}(|T_1, \ldots, T_n| \to R) = \text{alignof}(\texttt{usize})$$

The representation is:

```
┌─────────────────────────────────┬─────────────────────────────────┐
│          env_ptr                │          code_ptr               │
└─────────────────────────────────┴─────────────────────────────────┘
         (1 machine word)                  (1 machine word)
```

where:
- `env_ptr` points to the captured environment (or is null for non-capturing callables coerced to closure type).
- `code_ptr` points to the executable code, which expects `env_ptr` as an implicit first argument.

**Sparse-to-Closure Coercion**

When a sparse function pointer value is coerced to a closure type (via the subtyping rule T-Sparse-Sub-Closure):

1. A closure value is constructed.
2. The `env_ptr` field is set to null.
3. The `code_ptr` field is set to a thunk that ignores the environment parameter and calls the original code.

Implementations MAY optimize this coercion to avoid the thunk when the calling convention permits.

**Layout Classification**

| Aspect                    | Classification         |
| :------------------------ | :--------------------- |
| Sparse function pointer   | Defined (1 word)       |
| Closure type              | Defined (2 words)      |
| Field ordering in closure | Defined (env, code)    |
| Environment layout        | Implementation-Defined |

> **Note:** The internal layout of a closure's captured environment is implementation-defined. Implementations determine field ordering, padding, and whether captures are stored inline or via indirection.

##### Constraints & Legality

**Structural Constraints**

The following structural constraints apply to function types:

1. A function type MUST have zero or more parameters and exactly one return type.
2. The return type `()` (unit) MUST be used when a callable returns no meaningful value.
3. The `move` modifier MUST NOT appear on the return type; it applies only to parameters.

**FFI Constraints**

Closure types are NOT FFI-safe:

1. Closure types (`|T| -> U`) MUST NOT appear in `extern` procedure signatures (Clause 12).
2. Only sparse function pointer types are permitted at FFI boundaries.
3. Sparse function pointers used in FFI contexts MUST NOT have generic type parameters.

**Invocation Constraints**

The following constraints apply when invoking a function type value:

1. The number of arguments MUST equal the number of parameters.
2. Each argument type MUST be a subtype of the corresponding parameter type.
3. For parameters with the `move` modifier, the argument MUST be an explicit `move` expression.

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2201` | Error    | Argument count mismatch: expected $n$ arguments, found $m$.  | Compile-time | Rejection |
| `E-TYP-2202` | Error    | Type mismatch in function argument or return position.       | Compile-time | Rejection |
| `E-TYP-2203` | Error    | Missing `move` on argument to `move` parameter.              | Compile-time | Rejection |
| `E-TYP-2204` | Error    | Closure type in `extern` procedure signature.                | Compile-time | Rejection |
| `E-TYP-2205` | Error    | Assignment of closure value to sparse function pointer type. | Compile-time | Rejection |
| `E-TYP-2206` | Error    | `move` modifier on return type.                              | Compile-time | Rejection |

---

### Clause 6 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 6 That MUST NOT Be Redefined Elsewhere:**

| Term                             | Section | Description                                                  |
| :------------------------------- | :------ | :----------------------------------------------------------- |
| Modal Type                       | §6.1    | State-machine type with compile-time state tracking          |
| General Modal Type               | §6.1    | Sum type holding any state of a modal                        |
| State-Specific Type              | §6.1    | Concrete type for a single modal state (`M@S`)               |
| Modal Widening                   | §6.1    | Coercion from state-specific to general modal type           |
| State Transition                 | §6.1    | Method consuming one state and producing another             |
| Safe Pointer (`Ptr<T>`)          | §6.2    | Modal pointer type with `@Valid`, `@Null`, `@Expired` states |
| Raw Pointer (`*imm T`, `*mut T`) | §6.2    | Untracked pointer type for unsafe code                       |
| Procedure Type                   | §6.3    | First-class callable signature type                          |
| Closure Type                     | §6.3    | Anonymous procedure with captured environment                |

**Terms Referenced from Other Clauses:**

| Term             | Source | Usage in Clause 6                   |
| :--------------- | :----- | :---------------------------------- |
| Permission Types | §4.5   | Pointer permission qualifiers       |
| Provenance       | §3.3   | Pointer lifetime tracking           |
| Unsafe Block     | §3.10  | Required for raw pointer operations |
| Region           | §3.9   | Pointer provenance source           |
| Drop Form        | §9.3   | Destructor for pointer cleanup      |

**Terms Deferred to Later Clauses:**

| Term               | Deferred To | Reason                        |
| :----------------- | :---------- | :---------------------------- |
| Generic Parameters | Clause 7    | Pointer type parameterization |
| FFI                | Clause 15   | Raw pointer interop           |
| Closure Captures   | Clause 11   | Expression semantics          |

---

## Clause 7: Type Extensions

This clause defines mechanisms that extend or constrain the type system: static polymorphism (generics), attributes (compile-time metadata), and refinement types (types with predicate constraints). These features do not define new types per se but add parameterization, metadata, or constraints to existing types.

---

### 7.1 Static Polymorphism (Generics)

##### Definition

**Static polymorphism** (also called **generics**) is the mechanism by which a single type or procedure definition operates uniformly over multiple concrete types. A **generic declaration** introduces one or more **type parameters** that serve as placeholders for concrete types supplied at instantiation. Generic declarations are resolved entirely at compile time via **monomorphization**: the compiler generates specialized code for each distinct combination of type arguments.

Static polymorphism provides zero-overhead polymorphism on *inputs* resolved at compile time. It contrasts with dynamic polymorphism (§9.5), which uses runtime dispatch via form objects.

**Formal Definition**

A generic declaration $D$ is defined by:

$$D = (\text{Name}, \text{Params}, \text{Body})$$

where:

- $\text{Name}$ is the declaration's identifier
- $\text{Params} = \langle P_1, P_2, \ldots, P_n \rangle$ is an ordered sequence of type parameters
- $\text{Body}$ is the declaration body (type definition or procedure body)

Each type parameter $P_i$ is defined by:

$$P_i = (\text{name}_i, \text{Bounds}_i)$$

where:

- $\text{name}_i$ is an identifier serving as the parameter name
- $\text{Bounds}_i \subseteq \mathcal{T}_{\text{form}}$ is a (possibly empty) set of form bounds that constrain valid type arguments

A type parameter with $\text{Bounds}_i = \emptyset$ is **unconstrained**; any type may be substituted. A type parameter with $\text{Bounds}_i \neq \emptyset$ is **constrained**; only types implementing all forms in $\text{Bounds}_i$ may be substituted.

**Monomorphization**

**Monomorphization** is the process of generating specialized code for each concrete instantiation of a generic type or procedure. Given a generic declaration $D\langle T_1, \ldots, T_n \rangle$ and concrete type arguments $A_1, \ldots, A_n$, monomorphization produces a specialized declaration $D[A_1/T_1, \ldots, A_n/T_n]$ where each occurrence of $T_i$ in the body is replaced with $A_i$.

##### Syntax & Declaration

**Grammar**

```ebnf
generic_params     ::= "<" generic_param_list ">"
generic_param_list ::= generic_param ("," generic_param)*
generic_param      ::= identifier [bound_clause]
bound_clause       ::= "<:" form_bound_list
form_bound_list   ::= form_bound ("," form_bound)*
form_bound        ::= type_path

where_clause       ::= "where" where_predicate_list
where_predicate_list ::= where_predicate ("," where_predicate)*
where_predicate    ::= identifier "<:" form_bound_list

generic_args       ::= "<" type_arg_list ">"
type_arg_list      ::= type ("," type)*

turbofish          ::= "::" generic_args
```

**Generic Parameter Declaration**

A generic parameter list appears after the name in type and procedure declarations:

```cursive
record Container<T> { ... }
procedure sort<T <: Ord>(arr: unique [T]) { ... }
```

**Bound Clause Syntax**

A type parameter MAY be followed by a bound clause using the `<:` operator. Multiple form bounds are separated by commas:

```cursive
<T>                         // Unconstrained
<T <: Display>              // Single bound
<T <: Display, Ord>         // Multiple bounds (T must implement both)
<T, U <: Clone>             // Multiple parameters, one constrained
```

**Where Clause Syntax**

As an alternative to inline bounds, constraints MAY be specified in a `where` clause following the parameter list:

```cursive
procedure compare<T>(a: T, b: T) -> Ordering
where T <: Ord
{ ... }

procedure process<T, U>(x: T, y: U) -> string
where T <: Display, Clone,
      U <: Hash
{ ... }
```

When both inline bounds and a `where` clause specify constraints for the same parameter, the effective constraint is the union of all specified bounds.

**Parsing Disambiguation**

The comma character serves as a separator for both form bounds (within a single parameter's bound list) and generic parameters. Parsing is disambiguated as follows:

1. After parsing `<:` and a form bound, if a comma is encountered:
   - If the token following the comma is an identifier followed by `<:`, the identifier begins a new bounded parameter.
   - Otherwise, the identifier is another form bound for the current parameter.

2. Identifiers NOT preceded by `<:` in the current parse state are unconstrained type parameters.

This rule implies:
- `<T, U>` — Two unconstrained parameters
- `<T <: A, B>` — One parameter `T` bounded by both `A` and `B`
- `<T <: A, U <: B>` — Parameter `T` bounded by `A`, parameter `U` bounded by `B`
- `<T <: A, U>` — Parameter `T` bounded by both `A` and `U` (NOT `T` bounded by `A` with unconstrained `U`)

> **⚠️ Parsing Pitfall:** The comma-separated bound syntax can surprise developers familiar with other languages. The declaration `<T <: A, U>` does NOT declare two parameters. To declare a bounded parameter followed by an unconstrained parameter, use a `where` clause:
> ```cursive
> // INCORRECT interpretation: T bounded by A, U unconstrained
> // ACTUAL interpretation: T bounded by both A and U
> procedure wrong<T <: A, U>(x: T, y: U) { ... }  // COMPILE ERROR if U is not a form
>
> // CORRECT: Use where clause
> procedure correct<T, U>(x: T, y: U) where T <: A { ... }
> ```

To declare bounded parameters followed by unconstrained parameters, use a `where` clause:

```cursive
procedure example<T, U>(x: T, y: U) where T <: Display { ... }
```

**Summary Table — Generic Parameter Syntax:**

| Syntax                | Meaning                                                    |
| :-------------------- | :--------------------------------------------------------- |
| `<T>`                 | One unconstrained parameter `T`                            |
| `<T, U>`              | Two unconstrained parameters `T` and `U`                   |
| `<T <: A>`            | Parameter `T` bounded by form `A`                          |
| `<T <: A, B>`         | Parameter `T` bounded by forms `A` AND `B`                 |
| `<T <: A, U <: B>`    | Parameter `T` bounded by `A`, parameter `U` bounded by `B` |
| `<T, U> where T <: A` | Equivalent to above; preferred for readability             |

**Best Practice:** When mixing bounded and unconstrained parameters, use a `where` clause for clarity.

**Type Argument Syntax**

Generic types are instantiated by supplying type arguments in angle brackets:

```cursive
let opt: Option<i32> = Option::Some(42)
let map: HashMap<string, i64> = HashMap::new()
```

For generic procedure calls, type arguments are typically inferred from the arguments and return context. When explicit type arguments are required, the **turbofish** syntax (`::<>`) disambiguates type arguments from comparison operators:

```cursive
let values = collect::<Vec<i32>>(iter)
let default_val = Default::default::<Config>()
```

##### Static Semantics

**Well-Formedness of Generic Parameters (WF-Generic-Param)**

A generic parameter list is well-formed if all parameter names are distinct and all form bounds reference valid form types:

$$\frac{
    \forall i \neq j,\ \text{name}_i \neq \text{name}_j \quad
    \forall i,\ \forall B \in \text{Bounds}_i,\ \Gamma \vdash B : \text{Form}
}{
    \Gamma \vdash \langle P_1, \ldots, P_n \rangle\ \text{wf}
} \quad \text{(WF-Generic-Param)}$$

**Generic Type Well-Formedness (WF-Generic-Type)**

A generic type declaration is well-formed if its parameter list is well-formed and its body is well-formed under a context extended with the type parameters:

$$\frac{
    \Gamma \vdash \langle P_1, \ldots, P_n \rangle\ \text{wf} \quad
    \Gamma, T_1 : P_1, \ldots, T_n : P_n \vdash \text{Body}\ \text{wf}
}{
    \Gamma \vdash \texttt{type}\ \textit{Name}\langle P_1, \ldots, P_n \rangle\ \text{Body}\ \text{wf}
} \quad \text{(WF-Generic-Type)}$$

**Generic Procedure Well-Formedness (WF-Generic-Proc)**

A generic procedure declaration is well-formed if its parameter list is well-formed and its signature and body are well-formed under a context extended with the type parameters:

$$\frac{
    \Gamma \vdash \langle P_1, \ldots, P_n \rangle\ \text{wf} \quad
    \Gamma' = \Gamma, T_1 : P_1, \ldots, T_n : P_n \quad
    \Gamma' \vdash \text{signature}\ \text{wf} \quad
    \Gamma' \vdash \text{body}\ \text{wf}
}{
    \Gamma \vdash \texttt{procedure}\ f\langle P_1, \ldots, P_n \rangle(\ldots) \to R\ \{\ldots\}\ \text{wf}
} \quad \text{(WF-Generic-Proc)}$$

**Constraint Satisfaction (T-Constraint-Sat)**

A type argument $A$ satisfies a constraint set $\text{Bounds}$ if $A$ implements all forms in $\text{Bounds}$:

$$\frac{
    \forall B \in \text{Bounds},\ \Gamma \vdash A <: B
}{
    \Gamma \vdash A\ \text{satisfies}\ \text{Bounds}
} \quad \text{(T-Constraint-Sat)}$$

**Generic Instantiation (T-Generic-Inst)**

A generic type instantiation is well-formed if all type arguments satisfy their corresponding constraints:

$$\frac{
    \text{Name}\langle P_1, \ldots, P_n \rangle\ \text{declared} \quad
    \forall i \in 1..n,\ \Gamma \vdash A_i\ \text{satisfies}\ \text{Bounds}(P_i)
}{
    \Gamma \vdash \text{Name}\langle A_1, \ldots, A_n \rangle\ \text{wf}
} \quad \text{(T-Generic-Inst)}$$

**Generic Procedure Call (T-Generic-Call)**

A call to a generic procedure is well-typed if the inferred or explicit type arguments satisfy all constraints and the value arguments match the specialized parameter types:

$$\frac{
    \begin{gathered}
    f\langle P_1, \ldots, P_n \rangle(x_1 : S_1, \ldots, x_m : S_m) \to R\ \text{declared} \\
    \forall i \in 1..n,\ \Gamma \vdash A_i\ \text{satisfies}\ \text{Bounds}(P_i) \\
    \forall j \in 1..m,\ \Gamma \vdash e_j : S_j[A_1/T_1, \ldots, A_n/T_n]
    \end{gathered}
}{
    \Gamma \vdash f\langle A_1, \ldots, A_n \rangle(e_1, \ldots, e_m) : R[A_1/T_1, \ldots, A_n/T_n]
} \quad \text{(T-Generic-Call)}$$

**Type Argument Inference**

When type arguments are not explicitly provided, the implementation MUST infer them using bidirectional type inference (§4.4). Type arguments are inferred from:

1. The types of value arguments at the call site
2. The expected return type from the surrounding context

If type arguments cannot be uniquely determined, the program is ill-formed with diagnostic `E-TYP-2301`.

**Variance of Generic Parameters**

The variance of each type parameter is determined by its usage within the type definition, as specified in §4.3. Variance governs the subtyping relationship between instantiations of the same generic type with different type arguments.

**Monomorphization Requirements**

Implementations MUST generate specialized code for each distinct combination of concrete type arguments used in the program. The following requirements apply:

1. **Specialization:** For each instantiation $D\langle A_1, \ldots, A_n \rangle$, the implementation MUST produce code equivalent to manually substituting each type argument for its corresponding parameter throughout the declaration body.

2. **Zero Overhead:** Calls to generic procedures MUST compile to direct static calls to the specialized instantiation. Virtual dispatch (vtable lookup) is prohibited for static polymorphism.

3. **Independent Instantiation:** Each distinct instantiation is an independent type or procedure. `Container<i32>` and `Container<i64>` are distinct types with no implicit relationship.

**Recursion Depth Limit**

Monomorphization MAY produce recursive instantiations (e.g., `List<Option<List<T>>>`). Implementations MUST detect and reject infinite monomorphization recursion.

The maximum instantiation depth is **implementation-defined (IDB)**. Implementations MUST support a minimum depth of 128 levels and MUST document their limit in the Conformance Dossier.

> **Note:** Infinite recursion typically arises from patterns like `procedure f<T>() { f::<Option<T>>() }` where each instantiation triggers another with a strictly larger type.

##### Memory & Layout

**Layout Independence**

Each monomorphized instantiation has an independent memory layout. The layout of `Container<i32>` is computed using the rules for `i32` as the element type; the layout of `Container<i64>` is computed independently using `i64`.

**Size and Alignment**

The size and alignment of a generic type instantiation are determined by substituting the concrete type arguments and applying the layout rules for the resulting concrete type (§4.16).

$$\text{sizeof}(\text{Name}\langle A_1, \ldots, A_n \rangle) = \text{sizeof}(\text{Name}[A_1/T_1, \ldots, A_n/T_n])$$

$$\text{alignof}(\text{Name}\langle A_1, \ldots, A_n \rangle) = \text{alignof}(\text{Name}[A_1/T_1, \ldots, A_n/T_n])$$

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to generic declarations and instantiations:

1. A generic parameter list MUST NOT contain duplicate parameter names.
2. A form bound MUST reference a valid form type; bounding by non-form types is forbidden.
3. A generic instantiation MUST provide exactly the number of type arguments declared by the generic; partial application is not permitted.
4. Type arguments MUST satisfy all constraints (form bounds) declared for their corresponding parameters.
5. Generic parameters are **prohibited** in `extern` procedure signatures. FFI boundaries require monomorphic types.
6. Infinite monomorphization recursion MUST be detected and rejected.
7. The monomorphization depth MUST NOT exceed the implementation's documented limit.

**Diagnostic Table**

| Code         | Severity | Condition                                                           | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-2301` | Error    | Type arguments cannot be inferred; explicit instantiation required. | Compile-time | Rejection |
| `E-TYP-2302` | Error    | Type argument does not satisfy form bound.                          | Compile-time | Rejection |
| `E-TYP-2303` | Error    | Wrong number of type arguments for generic instantiation.           | Compile-time | Rejection |
| `E-TYP-2304` | Error    | Duplicate type parameter name in generic declaration.               | Compile-time | Rejection |
| `E-TYP-2305` | Error    | Form bound references a non-form type.                              | Compile-time | Rejection |
| `E-TYP-2306` | Error    | Generic parameter in `extern` procedure signature.                  | Compile-time | Rejection |
| `E-TYP-2307` | Error    | Infinite monomorphization recursion detected.                       | Compile-time | Rejection |
| `E-TYP-2308` | Error    | Monomorphization depth limit exceeded.                              | Compile-time | Rejection |

---


### 7.2 Attributes

##### Definition

An **attribute** is a compile-time annotation attached to a declaration that provides metadata to the compiler. Attributes influence code generation, memory layout, diagnostics, verification strategies, and interoperability without altering the core semantics of the annotated declaration except where this specification explicitly defines such alteration.

**Formal Definition**

An attribute $A$ is defined by:

$$A = (\text{Name}, \text{Args})$$

where:

- $\text{Name}$ is an identifier designating the attribute
- $\text{Args}$ is a (possibly empty) sequence of arguments

An **attribute list** is a sequence of one or more attributes applied to a declaration:

$$\text{AttributeList} = \langle A_1, A_2, \ldots, A_n \rangle$$

The **attribute registry** $\mathcal{R}$ is the set of all attributes recognized by a conforming implementation. The registry partitions into:

$$\mathcal{R} = \mathcal{R}_{\text{spec}} \uplus \mathcal{R}_{\text{vendor}}$$

where $\mathcal{R}_{\text{spec}}$ contains specification-defined attributes and $\mathcal{R}_{\text{vendor}}$ contains vendor-defined extensions.

##### Syntax & Declaration

**Grammar**

```ebnf
attribute_list     ::= attribute+
attribute          ::= "[[" attribute_spec ("," attribute_spec)* "]]"
attribute_spec     ::= attribute_name ["(" attribute_args ")"]
attribute_name     ::= identifier | vendor_prefix "::" identifier
vendor_prefix      ::= identifier ("." identifier)*
attribute_args     ::= attribute_arg ("," attribute_arg)*
attribute_arg      ::= literal
                     | identifier
                     | identifier ":" literal
                     | identifier "(" attribute_args ")"
```

**Placement**

An attribute list MUST appear immediately before the declaration it modifies. The attribute list and its target declaration MUST NOT be separated by other declarations or statements.

**Attribute Block Equivalence**

Multiple attributes MAY be specified in a single block or in separate blocks. The following forms are semantically equivalent:

```cursive
[[attr1, attr2]]
declaration

[[attr1]]
[[attr2]]
declaration
```

The order of attribute application within equivalent forms is Unspecified Behavior (USB).

##### Static Semantics

**Attribute Processing**

Attribute processing occurs during semantic analysis. For each attribute $A$ applied to declaration $D$:

1. The implementation MUST verify that $A.\text{Name} \in \mathcal{R}$.
2. The implementation MUST verify that $D$ is a valid target for $A$ per the target constraints defined below.
3. The implementation MUST verify that $A.\text{Args}$ conforms to the argument specification for $A$.
4. If all verifications succeed, the implementation applies the effect defined for $A$.

**Normative Attribute Registry**

The following table defines $\mathcal{R}_{\text{spec}}$, the complete set of specification-defined attributes. Valid targets for each attribute are specified in the Attribute Target Constraints table below.

| Attribute             | Arguments                            | Effect                                  | Full Semantics |
| :-------------------- | :----------------------------------- | :-------------------------------------- | :------------- |
| `[[layout(...)]]`     | `C`, `packed`, `align(N)`, `IntType` | Controls memory representation          | §4.16.1        |
| `[[inline(...)]]`     | `always`, `never`, `default`         | Inlining directive                      | §4.16.2        |
| `[[cold]]`            | (none)                               | Marks procedure as unlikely to execute  | §4.16.3        |
| `[[deprecated(...)]]` | Optional string message              | Emits warning on usage                  | §4.16.4        |
| `[[reflect]]`         | (none)                               | Enables compile-time introspection      | §4.16.5        |
| `[[verify(...)]]`     | `static`, `dynamic`, `trusted`       | Contract verification mode              | §7.4           |
| `[[link_name(...)]]`  | String literal                       | Overrides linker symbol name            | §12.4          |
| `[[no_mangle]]`       | (none)                               | Disables symbol name mangling           | §12.4          |
| `[[unwind(...)]]`     | `abort`, `catch`                     | Controls panic behavior at FFI boundary | §12.5          |

**Attribute Target Constraints**

Each attribute is valid only on specific declaration kinds. Applying an attribute to an invalid target is ill-formed:

| Attribute             | `record` | `enum` | `modal` | `procedure` | `extern` proc | Contract | Other |
| :-------------------- | :------: | :----: | :-----: | :---------: | :-----------: | :------: | :---: |
| `[[layout(...)]]`     |    ✓     |   ✓    |    ✗    |      ✗      |       ✗       |    ✗     |   ✗   |
| `[[inline(...)]]`     |    ✗     |   ✗    |    ✗    |      ✓      |       ✓       |    ✗     |   ✗   |
| `[[cold]]`            |    ✗     |   ✗    |    ✗    |      ✓      |       ✓       |    ✗     |   ✗   |
| `[[deprecated(...)]]` |    ✓     |   ✓    |    ✓    |      ✓      |       ✓       |    ✗     |   ✓   |
| `[[verify(...)]]`     |    ✗     |   ✗    |    ✗    |      ✗      |       ✗       |    ✓     |   ✗   |
| `[[reflect]]`         |    ✓     |   ✓    |    ✓    |      ✗      |       ✗       |    ✗     |   ✗   |
| `[[link_name(...)]]`  |    ✗     |   ✗    |    ✗    |      ✗      |       ✓       |    ✗     |   ✗   |
| `[[no_mangle]]`       |    ✗     |   ✗    |    ✗    |      ✗      |       ✓       |    ✗     |   ✗   |
| `[[unwind(...)]]`     |    ✗     |   ✗    |    ✗    |      ✗      |       ✓       |    ✗     |   ✗   |

**Vendor Extension Namespacing**

Vendor-defined attributes MUST use a namespaced identifier to prevent collision with specification-defined attributes and other vendors' extensions:

1. Vendor attributes MUST use reverse-domain-style prefixes: `[[com.vendor.attribute_name]]`
2. The `cursive` namespace and all `cursive::*` prefixes are reserved for specification-defined attributes and MUST NOT be used by vendor extensions.
3. Implementations MUST document all vendor-defined attributes in the Conformance Dossier.

An implementation encountering an unknown attribute (one not in $\mathcal{R}$) MUST diagnose error `E-DEC-2451`.

---

#### 7.2.1 The `[[layout(...)]]` Attribute

##### Definition

The `[[layout(...)]]` attribute controls the memory representation of composite types. It specifies field ordering, padding behavior, alignment requirements, and discriminant representation.

> **Rationale:** The attribute name `layout` was chosen over `repr` (used in some other systems languages) because it more directly describes the attribute's purpose: controlling memory layout. This aligns with Cursive's design principle of self-documenting syntax.

##### Syntax & Declaration

**Grammar**

```ebnf
layout_attribute ::= "[[" "layout" "(" layout_args ")" "]]"
layout_args      ::= layout_kind ("," layout_kind)*
layout_kind      ::= "C"
                   | "packed"
                   | "align" "(" integer_literal ")"
                   | int_type
int_type         ::= "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64"
```

##### Static Semantics

**`layout(C)` — C-Compatible Layout**

For a `record` marked `[[layout(C)]]`:
1. Fields MUST be laid out in declaration order.
2. Padding MUST be inserted only as required to satisfy alignment constraints per the target platform's C ABI.
3. The total size MUST be a multiple of the record's alignment.

For an `enum` marked `[[layout(C)]]`:
1. The discriminant MUST be represented as a C-compatible integer tag.
2. The default tag type is implementation-defined (IDB), typically `i32`.
3. The enum is laid out as a tagged union per the target C ABI.

**`layout(IntType)` — Explicit Discriminant Type**

For an `enum` marked `[[layout(IntType)]]` where `IntType` is one of `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`:
1. The discriminant MUST be represented using the specified integer type.
2. The discriminant value for each variant MUST be representable in the specified type.
3. This form is valid only on `enum` declarations. Applying `[[layout(IntType)]]` to a `record` is ill-formed (`E-DEC-2452`).

**`layout(packed)` — Packed Layout**

For a `record` marked `[[layout(packed)]]`:
1. All inter-field padding is removed.
2. Each field is laid out with alignment 1, regardless of its natural alignment.
3. The record's overall alignment becomes 1.

Packed layout has the following implications:
- Field access MAY require unaligned memory operations.
- Taking a reference to a packed field is Unverifiable Behavior (UVB) because the resulting reference may be misaligned. Such operations MUST occur within an `unsafe` block.
- Direct field reads and writes remain safe (not UVB).

The `packed` layout kind is valid only on `record` declarations. Applying `packed` to an `enum` is ill-formed (`E-DEC-2454`).

**`layout(align(N))` — Minimum Alignment**

The `align(N)` layout kind sets a minimum alignment for the type:
1. $N$ MUST be a positive integer that is a power of two. Non-power-of-two values are ill-formed (`E-DEC-2453`).
2. The effective alignment of the type is $\max(N, \text{natural alignment})$.
3. If $N$ is less than the natural alignment, the natural alignment is used and a warning (`W-DEC-2451`) is emitted.
4. The type's size is padded to be a multiple of the effective alignment.

**Valid Combinations**

Multiple layout kinds MAY be combined in a single attribute:

| Combination                | Validity | Effect                                         |
| :------------------------- | :------- | :--------------------------------------------- |
| `layout(C)`                | Valid    | C-compatible layout                            |
| `layout(packed)`           | Valid    | Packed layout (records only)                   |
| `layout(align(N))`         | Valid    | Minimum alignment $N$                          |
| `layout(C, packed)`        | Valid    | C-compatible packed layout                     |
| `layout(C, align(N))`      | Valid    | C-compatible layout with minimum alignment $N$ |
| `layout(u8)` (on enum)     | Valid    | 8-bit unsigned discriminant                    |
| `layout(packed, align(N))` | Invalid  | Conflicting directives (`E-DEC-2455`)          |

**Applicability Constraints**

| Declaration Kind          |  `C`  | `packed` | `align(N)` | `IntType` |
| :------------------------ | :---: | :------: | :--------: | :-------: |
| `record`                  |   ✓   |    ✓     |     ✓      |     ✗     |
| `enum`                    |   ✓   |    ✗     |     ✓      |     ✓     |
| `modal`                   |   ✗   |    ✗     |     ✗      |     ✗     |
| Generic (unmonomorphized) |   ✗   |    ✗     |     ✗      |     ✗     |

Applying `[[layout(...)]]` to a `modal` type or an unmonomorphized generic type is ill-formed (`E-FFI-3303`, as defined in §12.3).

---

#### 7.2.2 The `[[inline(...)]]` Attribute

##### Definition

The `[[inline(...)]]` attribute provides directives to the compiler regarding procedure inlining.

##### Static Semantics

**`inline(always)`**

The implementation SHOULD inline the procedure at all call sites. If inlining is not possible (e.g., the procedure is recursive, exceeds implementation limits, or its address is taken), the implementation MUST emit warning `W-DEC-2452`.

**`inline(never)`**

The implementation MUST NOT inline the procedure. The procedure body MUST be emitted as a separate callable unit, and all calls MUST use the standard calling convention.

**`inline(default)`**

The implementation applies its default inlining heuristics. This is semantically equivalent to omitting the `[[inline(...)]]` attribute entirely.

**Default Behavior (No Attribute)**

When no `[[inline(...)]]` attribute is present, the implementation MAY inline or not inline the procedure at its discretion based on optimization heuristics.

---

#### 7.2.3 The `[[cold]]` Attribute

##### Definition

The `[[cold]]` attribute marks a procedure as unlikely to be executed during typical program runs. This hint enables the implementation to optimize for the common case by:

1. Placing the procedure's code in a separate section to improve instruction cache locality of hot code.
2. Reducing optimization effort on the cold procedure.
3. Biasing branch prediction at call sites toward not calling the procedure.

##### Static Semantics

The `[[cold]]` attribute takes no arguments. It is valid only on procedure declarations.

The attribute is a hint; implementations MAY ignore it without diagnostic.

---

#### 7.2.4 The `[[deprecated(...)]]` Attribute

##### Definition

The `[[deprecated(...)]]` attribute marks a declaration as deprecated. Usage of a deprecated declaration triggers a warning diagnostic.

##### Syntax & Declaration

```ebnf
deprecated_attribute ::= "[[" "deprecated" ["(" string_literal ")"] "]]"
```

The optional string argument provides a deprecation message that SHOULD be included in the warning diagnostic.

##### Static Semantics

When a program references a declaration marked `[[deprecated(...)]]`:

1. The implementation MUST emit warning `W-CNF-0601` (as defined in §1.5).
2. If a message argument is present, the diagnostic SHOULD include the message text.
3. The deprecated declaration remains fully functional; only a warning is emitted.

---

#### 7.2.5 The `[[reflect]]` Attribute

##### Definition

The `[[reflect]]` attribute enables compile-time introspection for a type declaration. Types marked with `[[reflect]]` have their metadata accessible to the `reflect_type<T>()` intrinsic during compile-time evaluation.

##### Static Semantics

1. The `[[reflect]]` attribute takes no arguments.
2. It is valid on `record`, `enum`, and `modal` type declarations.
3. The attribute has no effect on runtime behavior or memory layout.
4. Full semantics for compile-time introspection are defined in Clause 10 (Metaprogramming).

---

#### 7.2.6 FFI Attributes

##### Definition

**FFI Attributes** are attributes specific to Foreign Function Interface declarations. These attributes control linking behavior and panic propagation across the language boundary.

##### Static Semantics

The following FFI attributes are recognized. Complete semantics are defined in Clause 12 (Interoperability):

| Attribute              | Reference | Effect                                                       |
| :--------------------- | :-------- | :----------------------------------------------------------- |
| `[[link_name("sym")]]` | §12.4     | Overrides the linker symbol name for an extern procedure     |
| `[[no_mangle]]`        | §12.4     | Disables name mangling (implicit for `extern "C"`)           |
| `[[unwind(mode)]]`     | §12.5     | Controls panic behavior at FFI boundary (`abort` or `catch`) |

These attributes are valid only on `extern` procedure declarations.

---

#### 7.2.7 The `[[verify(...)]]` Attribute

##### Definition

The `[[verify(...)]]` attribute controls the verification strategy for contract clauses. It specifies whether contracts are checked statically, dynamically, or trusted without verification.

##### Static Semantics

The `[[verify(...)]]` attribute accepts one of three modes. Complete semantics are defined in §7.4 (Verification Modes):

| Mode      | Effect                                                        |
| :-------- | :------------------------------------------------------------ |
| `static`  | Requires compile-time proof; failure is ill-formed            |
| `dynamic` | Generates runtime checks; failure triggers panic              |
| `trusted` | No verification; violation is UVB (requires `unsafe` context) |

This attribute is valid only on contract clauses (`where`, `must`, `ensure`).

---

#### 7.2.8 Consolidated Constraints and Diagnostics

##### Constraints & Legality

**Negative Constraints**

1. An attribute name not in $\mathcal{R}$ (the attribute registry) MUST be diagnosed as unknown.
2. An attribute applied to an invalid target declaration kind MUST be diagnosed.
3. Malformed attribute syntax (e.g., missing brackets, invalid argument types) MUST be diagnosed.
4. Conflicting layout arguments (`packed` with `align`) MUST be diagnosed.
5. The `align(N)` argument MUST be a power of two.
6. The `packed` layout kind MUST NOT be applied to `enum` declarations.

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-DEC-2450` | Error    | Malformed attribute syntax.                           | Compile-time | Rejection |
| `E-DEC-2451` | Error    | Unknown attribute name.                               | Compile-time | Rejection |
| `E-DEC-2452` | Error    | Attribute not valid on target declaration kind.       | Compile-time | Rejection |
| `E-DEC-2453` | Error    | `align(N)` where N is not a power of two.             | Compile-time | Rejection |
| `E-DEC-2454` | Error    | `packed` applied to non-record declaration.           | Compile-time | Rejection |
| `E-DEC-2455` | Error    | Conflicting layout arguments (`packed` with `align`). | Compile-time | Rejection |
| `W-DEC-2451` | Warning  | `align(N)` where N < natural alignment (no effect).   | Compile-time | N/A       |
| `W-DEC-2452` | Warning  | `inline(always)` but inlining not possible.           | Compile-time | N/A       |

---


### 7.3 Refinement Types

##### Definition

A **refinement type** is a type constructed by attaching a predicate constraint to a base type. The refinement type `T where { P }` denotes the subset of values of type `T` for which predicate `P` evaluates to `true`. Refinement types enable dependent typing patterns where type validity depends on runtime-checkable value properties.

**Formal Definition**

Let $\mathcal{T}$ denote the set of all types and $\mathcal{P}$ denote the set of all pure boolean predicates. A refinement type $R$ is defined by:

$$R = (T_{\text{base}}, P)$$

where:

- $T_{\text{base}} \in \mathcal{T}$ is the base type being refined
- $P : T_{\text{base}} \to \texttt{bool}$ is a pure predicate constraining the value set

The value set of a refinement type is the subset of base type values satisfying the predicate:

$$\text{Values}(T \text{ where } \{P\}) = \{ v \in \text{Values}(T) \mid P(v) = \texttt{true} \}$$

A refinement type is a **proper subtype** of its base type; a value of type `T where { P }` may be used wherever a value of type `T` is expected.

##### Syntax & Declaration

**Grammar**

```ebnf
refinement_type       ::= type "where" "{" predicate "}"

type_alias_refine     ::= "type" identifier "=" type "where" "{" predicate "}"

param_with_constraint ::= identifier ":" type "where" "{" predicate "}"

predicate             ::= expression
```

**Self Reference**

Within the predicate of a **type alias** or **standalone refinement type expression**, the keyword `self` refers to the value being constrained. The identifier `self` is implicitly bound within the predicate scope with the base type `T`.

**Inline Notation Examples:**

The following notations are valid refinement type expressions:

- `usize where { self < 256 }` — unsigned integers less than 256
- `i32 where { self > 0 }` — positive 32-bit integers
- `[T] where { self~>len() > 0 }` — non-empty slices

**Parameter Constraint Syntax**

When a refinement type appears as a **procedure parameter type**, the predicate uses the **parameter name** directly rather than `self`. This syntax integrates with procedure contract clauses (§7.2).

Within the predicate of a parameter constraint, the **parameter name** refers to the parameter value being constrained. Other in-scope parameters declared earlier in the parameter list MAY be referenced, enabling dependent constraints between parameters.

**Desugaring**

A parameter with an inline constraint is semantically equivalent to an unrefined parameter with a procedure-level precondition. The following declarations are equivalent:

```cursive
// Inline constraint form (type refinement)
procedure get<T>(arr: [T], idx: usize where { idx < arr~>len() }) -> T

// Procedure contract form (equivalent)
procedure get<T>(arr: [T], idx: usize) -> T
    |= idx < arr~>len()
```

The inline form is preferred when the constraint concerns a single parameter. The procedure contract clause (`|=`) is preferred for multi-parameter relationships, postconditions, or complex predicates that benefit from separate formatting.

> **Rationale:** Using the parameter name (instead of `self`) in inline parameter constraints provides consistency (the same identifier appears in the signature and predicate), enables dependent typing (later parameters can reference earlier parameters by name), unifies inline constraints with procedure contract clause syntax, and improves readability (`idx < arr~>len()` reads more naturally than `self < arr~>len()`). The `self` keyword is reserved for contexts where no parameter name exists: type aliases (`type Positive = i32 where { self > 0 }`) and record/enum invariants (`record R { x: i32 } where { self.x > 0 }`).

##### Static Semantics

**Well-Formedness (WF-Refine-Type)**

A refinement type `T where { P }` is well-formed when the base type is well-formed and the predicate is a valid pure expression of type `bool` under a context extended with the constrained value:

$$\frac{
    \Gamma \vdash T\ \text{wf} \quad
    \Gamma, \texttt{self} : T \vdash P : \texttt{bool} \quad
    \text{Pure}(P)
}{
    \Gamma \vdash (T \text{ where } \{P\})\ \text{wf}
} \quad \text{(WF-Refine-Type)}$$

**Well-Formedness of Parameter Constraint (WF-Param-Constraint)**

A parameter constraint `x: T where { P }` is well-formed when the base type is well-formed, the predicate is a valid pure expression of type `bool`, and the predicate references the parameter name (not `self`):

$$\frac{
    \Gamma \vdash T\ \text{wf} \quad
    \Gamma, x : T \vdash P : \texttt{bool} \quad
    \text{Pure}(P) \quad
    \texttt{self} \notin \text{FreeVars}(P)
}{
    \Gamma \vdash (x : T \text{ where } \{P\})\ \text{wf}
} \quad \text{(WF-Param-Constraint)}$$

**Typing Rules**

**(T-Refine-Intro)**

A value of base type `T` has refinement type `T where { P }` when a Verification Fact (§7.5) establishes that `P` holds for that value:

$$\frac{
    \Gamma \vdash e : T \quad
    \Gamma \vdash F(P[e/\texttt{self}], L) \quad
    L \text{ dominates current location}
}{
    \Gamma \vdash e : T \text{ where } \{P\}
} \quad \text{(T-Refine-Intro)}$$

**(T-Refine-Elim)**

A value of refinement type may be used as a value of its base type without any runtime check:

$$\frac{
    \Gamma \vdash e : T \text{ where } \{P\}
}{
    \Gamma \vdash e : T
} \quad \text{(T-Refine-Elim)}$$

**Subtyping Rules**

A refinement type is a subtype of its base type:

$$\Gamma \vdash (T \text{ where } \{P\}) <: T \quad \text{(Sub-Refine-Base)}$$

A refinement type with a stronger predicate is a subtype of one with a weaker predicate when both share the same base type:

$$\frac{
    \Gamma \vdash P \implies Q
}{
    \Gamma \vdash (T \text{ where } \{P\}) <: (T \text{ where } \{Q\})
} \quad \text{(Sub-Refine)}$$

**Type Equivalence**

Two refinement types with the same base type are equivalent if and only if their predicates are logically equivalent:

$$\Gamma \vdash (T \text{ where } \{P\}) \equiv (T \text{ where } \{Q\}) \iff \Gamma \vdash P \iff Q$$

**Nested Refinements**

Nested refinement types flatten to a single refinement with conjoined predicates:

$$(T \text{ where } \{P\}) \text{ where } \{Q\} \equiv T \text{ where } \{P \land Q\}$$

**Predicate Scope and Purity**

The predicate `P` in a refinement type `T where { P }`:

1. MUST be a pure expression as defined in §7.1 (no side effects, no I/O, no allocation, no capability invocation).
2. MUST evaluate to type `bool`.
3. In type aliases and standalone refinement type expressions, MUST use `self` to refer to the constrained value.
4. In inline parameter constraints, MUST use the parameter name to refer to the constrained value; use of `self` is forbidden.
5. MAY reference other in-scope bindings, including earlier parameters in procedure signatures.

**Automatic Coercion**

The implementation MUST automatically coerce a value of type `T` to type `T where { P }` when active Verification Facts (§7.5) prove that `P` holds for that value. This coercion is implicit and requires no runtime check.

**Variance Interaction**

Refinement types participate in the standard subtyping relation defined in §4.2. Because `T where { P }` is a subtype of `T`, refinement types interact with generic type constructors according to the variance rules defined in §4.3. For a type constructor `C<T>` with covariant parameter `T`, if `S where { P } <: S` then `C<S where { P }> <: C<S>` follows from transitivity.

##### Dynamic Semantics

**Verification**

When a value is assigned to a binding of refinement type, or passed to a parameter of refinement type, and no Verification Fact establishes that the predicate holds, the predicate MUST be verified according to the active Verification Mode (§7.4):

| Mode      | Behavior                                                                                       |
| :-------- | :--------------------------------------------------------------------------------------------- |
| `static`  | The predicate MUST be statically provable; failure to prove is a compile-time error.           |
| `dynamic` | A runtime check is inserted; if the check fails, the thread MUST panic.                        |
| `trusted` | No verification is performed; predicate violation is Unverifiable Behavior (UVB).              |
| (default) | The implementation first attempts static proof; if inconclusive, falls back to `dynamic` mode. |

**Fact Synthesis**

Successful runtime verification of a refinement predicate synthesizes a Verification Fact that MAY be used by the implementation to elide subsequent checks on the same value for the same or implied predicates.

##### Memory & Layout

**Representation**

A refinement type `T where { P }` has identical size, alignment, and layout to its base type `T`. The predicate is a compile-time and runtime constraint only; it does not affect physical representation.

$$\text{sizeof}(T \text{ where } \{P\}) = \text{sizeof}(T)$$

$$\text{alignof}(T \text{ where } \{P\}) = \text{alignof}(T)$$

The memory representation of a value of type `T where { P }` is indistinguishable from a value of type `T`.

##### Constraints & Legality

**Negative Constraints**

The following constraints apply to refinement types:

1. A refinement predicate MUST be a pure expression. Impure expressions (those performing I/O, allocation, mutation, or capability invocation) are forbidden.

2. A refinement predicate MUST evaluate to type `bool`. Predicates of other types are forbidden.

3. In inline parameter constraints, the `self` keyword MUST NOT appear in the predicate; the parameter name MUST be used instead.

4. Refinement predicates MUST NOT create circular type dependencies. A predicate that references a type alias containing the refinement being defined is ill-formed.

5. For `static` verification mode, if the implementation's proof system cannot decide the predicate, the program is ill-formed. The programmer MAY use `dynamic` mode as a fallback.

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1950` | Error    | `self` used in inline parameter constraint.           | Compile-time | Rejection |
| `E-TYP-1951` | Error    | Refinement predicate is not of type `bool`.           | Compile-time | Rejection |
| `E-TYP-1952` | Error    | Circular type dependency in refinement predicate.     | Compile-time | Rejection |
| `E-CON-2801` | Error    | Static verification failed in `static` mode.          | Compile-time | Rejection |
| `E-CON-2802` | Error    | Impure expression in refinement predicate.            | Compile-time | Rejection |
| `P-TYP-1953` | Panic    | Refinement predicate evaluated to `false` at runtime. | Runtime      | Panic     |

---

### Clause 7 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 7 That MUST NOT Be Redefined Elsewhere:**

| Term               | Section | Description                                            |
| :----------------- | :------ | :----------------------------------------------------- |
| Generic Type       | §7.1    | Type parameterized by type variables                   |
| Type Parameter     | §7.1    | Placeholder for a concrete type in generic definitions |
| Type Constraint    | §7.1    | Form bound on a type parameter                         |
| Monomorphization   | §7.1    | Instantiation of generics to concrete types            |
| Attribute          | §7.2    | Compile-time metadata annotation                       |
| Built-in Attribute | §7.2    | Language-defined attribute with special semantics      |
| Refinement Type    | §7.3    | Type with predicate constraint (`T where {P}`)         |
| Predicate          | §7.3    | Boolean expression constraining refinement type        |

**Terms Referenced from Other Clauses:**

| Term      | Source | Usage in Clause 7                  |
| :-------- | :----- | :--------------------------------- |
| Variance  | §4.3   | Generic parameter variance         |
| Subtyping | §4.2   | Constrained type relationships     |
| Form      | §9.1   | Type constraints use forms         |
| Contract  | §10.1  | Refinement predicates as contracts |
| Comptime  | §14.1  | Attribute evaluation context       |

**Terms Deferred to Later Clauses:**

| Term                  | Deferred To | Reason                        |
| :-------------------- | :---------- | :---------------------------- |
| Form Definition       | Clause 9    | Constraint semantics          |
| Contract Verification | Clause 10   | Predicate verification modes  |
| Comptime Evaluation   | Clause 14   | Attribute argument evaluation |

---

# Part 3: Program Behavior

## Clause 8: Modules and Name Resolution

This clause defines Cursive's module system: how code is organized into projects, assemblies, and modules; how names are introduced, resolved, and accessed; and how modules are initialized before program execution.

### 8.1 Module System Architecture

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

**Multi-File Processing Order**

When a module comprises multiple `.cursive` files, the order in which files are processed is **Implementation-Defined Behavior (IDB)**. Implementations **MUST** document the file processing order in the Conformance Dossier.

Regardless of file processing order:
1. All files in the directory contribute to a single unified namespace
2. Identifier uniqueness constraints apply across all files
3. The resulting module semantics **MUST** be independent of file processing order for well-formed programs

##### Syntax & Declaration

**Top-Level Item Grammar**

Only the following declaration kinds are permitted at module scope:

```ebnf
top_level_item ::= import_decl
                 | use_decl
                 | static_decl
                 | procedure_decl
                 | type_decl
                 | form_decl

static_decl    ::= visibility? ("let" | "var") binding
type_decl      ::= visibility? ("record" | "enum" | "modal" | "type") identifier ...
form_decl     ::= visibility? "form" identifier ...
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

**Identifier Uniqueness**: Each identifier **MUST** be unique within module scope. Because Cursive uses a unified namespace (§8.4), a type declaration and a term declaration **MUST NOT** share the same identifier.

**Forbidden Constructs**: Control-flow constructs (`if`, `loop`, `match`) and expression statements **MUST NOT** appear at top level.

| Code         | Severity | Condition                                        |
| :----------- | :------- | :----------------------------------------------- |
| `E-MOD-1106` | Error    | Module path component is not a valid identifier. |
| `E-NAM-1302` | Error    | Duplicate declaration in module scope.           |
| `E-SYN-0501` | Error    | Control-flow construct at module scope.          |

---

### 8.2 Project Manifest

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

---

### 8.3 Module Discovery and Paths

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

### 8.4 Scope Context Structure

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
2. **Types**: Bindings for type declarations (`record`, `enum`, `modal`, `type`, `form`)
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
- A form
- A module reference

##### Constraints & Legality

Because Cursive uses a unified namespace, declaring a type and a term with the same identifier in the same scope **MUST** be rejected:

| Code         | Severity | Condition                                                  |
| :----------- | :------- | :--------------------------------------------------------- |
| `E-NAM-1302` | Error    | Duplicate name: identifier already declared in this scope. |

---

### 8.5 Visibility and Access Control

##### Definition

**Visibility**

Every top-level declaration has a **visibility level** that controls its accessibility from other modules. If no modifier is specified, visibility defaults to `internal`.

**Visibility Levels**

| Modifier    | Scope of Accessibility                                             |
| :---------- | :----------------------------------------------------------------- |
| `public`    | Visible to any module in any assembly that depends on it           |
| `internal`  | (Default) Visible only to modules within the **same assembly**     |
| `private`   | Visible only within the **defining module** (same directory)       |
| `protected` | Visible only within the **defining type** and form implementations |

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

**(WF-Access-Protected-Form):**
$$
\frac{\text{visibility}(item) = \text{protected} \quad \Gamma \subseteq \text{form } Tr \text{ for } T_{def} \quad A(\Gamma) = A(T_{def})}{\Gamma \vdash \text{can\_access}(item)}
$$

##### Constraints & Legality

**Protected Restriction**: The `protected` modifier **MUST NOT** be used on top-level (module-scope) declarations. It **MUST** only be applied to members (fields or procedures) within a `record`, `enum`, or `modal` declaration.

**Intra-Assembly Access**: Modules within the same assembly are automatically available for qualified name access without requiring an `import` declaration. A declaration `item` in module `mod` within assembly `A` is accessible from another module in assembly `A` via the qualified path `mod::item` if and only if the visibility of `item` is `public` or `internal`.

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-NAM-1305` | Error    | Inaccessible item: visibility does not permit access. | Compile-time | Rejection |
| `E-DEC-2440` | Error    | `protected` used on top-level declaration.            | Compile-time | Rejection |
| `E-MOD-1207` | Error    | Cannot access a `protected` item from this scope.     | Compile-time | Rejection |

---

### 8.6 Import and Use Declarations

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
import_decl ::= "import" module_path ("as" identifier)?

use_decl    ::= visibility? "use" use_clause
use_clause  ::= use_path ("as" identifier)?
              | use_path "::" use_list
              | use_path "::" "*"

use_path    ::= module_path ("::" identifier)?
use_list    ::= "{" use_specifier ("," use_specifier)* ","? "}"
use_specifier ::= identifier ("as" identifier)?
                | "self" ("as" identifier)?
```

**Examples:**

```cursive
// Import external assembly module
import core::io

// Import with alias
import core::collections as col

// Use single item
use core::io::File

// Use with alias
use core::io::File as FileHandle

// Use multiple items (list form)
use core::collections::{Vec, HashMap}

// Use with self (brings module AND items into scope)
use core::io::{self, File, Reader}
// After this: `io::write()` and `File` both work

// Use with aliased self
use core::collections::{self as col, Vec}

// Wildcard use (brings all accessible items into scope)
use core::prelude::*

// Re-export
public use core::io::File
```

##### Static Semantics

**Import Semantics**

An `import` declaration:

1. Establishes a dependency edge from the current assembly to the imported assembly
2. Makes the imported module's namespace available for qualified access
3. Does **NOT** introduce any names into the current scope's unqualified namespace
4. If `as alias` is specified, the alias becomes the local name for the module; otherwise, the last component of the module path is used

**Import Requirement for External Access**

For modules in **external assemblies**, an `import` declaration **MUST** precede any `use` declaration that references items from that assembly. The `import` declaration establishes the inter-assembly dependency; the `use` declaration then brings specific items into scope.

For modules in the **same assembly** (intra-assembly access), `use` declarations **MAY** directly reference module paths without a preceding `import`.

**Use Semantics**

A `use` declaration resolves a path and introduces bindings into the current scope:

1. **Item Use** (`use path::Item`): Resolves `Item` and introduces a binding with its original name (or the alias if `as alias` is specified).

2. **List Use** (`use path::{A, B, C}`): Resolves each item in the list and introduces bindings for all of them. Each item in the list **MUST** be unique.

3. **Self Use** (`use path::{self, A}`): The special specifier `self` in a use list introduces a binding for the module path itself, enabling qualified access via the module name alongside unqualified access to other items.

4. **Wildcard Use** (`use path::*`): Brings all accessible (public or internal, depending on assembly relationship) items from the target module into the current scope. Each item retains its original name.

**Formal Rule (WF-Use-Item):**

$$
\frac{
    \Gamma \vdash \text{resolve\_item}(\text{path}::i) \Rightarrow \text{item} \quad \Gamma \vdash \text{can\_access}(\text{item}) \quad \text{name} \notin \text{dom}(\Gamma)
}{
    \Gamma \vdash \text{use path}::i \text{ as name;}: WF
}
$$

**Formal Rule (WF-Use-Wildcard):**

$$
\frac{
    \Gamma \vdash \text{resolve\_module}(\text{path}) \Rightarrow m \quad \forall i \in \text{accessible}(m), \text{name}(i) \notin \text{dom}(\Gamma)
}{
    \Gamma \vdash \text{use path}::*: WF
}
$$

**Re-export Semantics**

A `public use` declaration re-exports an item, making it part of the current module's public API:

1. Performs the same resolution as a regular `use`
2. Additionally marks the introduced binding as `public`, making it visible to external dependents
3. The source item **MUST** itself be `public`; re-exporting `internal` or `private` items is forbidden

**Formal Rule (WF-Use-Public):**

$$
\frac{
    \Gamma \vdash \text{use path}::i \text{ as name;}: WF \quad \text{visibility}(\text{item}) = \text{public}
}{
    \Gamma \vdash \text{public use path}::i \text{ as name;}: WF
}
$$

##### Constraints & Legality

**Import Cycle Detection**: The compiler **MUST** detect and reject cyclic import dependencies between assemblies.

**Import Requirement**: For external assemblies, an `import` declaration **MUST** precede any `use` that references that assembly. Attempting to `use` an item from an external assembly without a prior `import` **MUST** be diagnosed as an error.

**Visibility Enforcement**: Re-exported items **MUST NOT** expose items with more restrictive visibility than the re-export itself. Re-exporting a `private` or `internal` item via `public use` is forbidden.

**Uniqueness in Use Lists**: Each identifier in a use list **MUST** be unique. Duplicate items in a single `use` declaration **MUST** be diagnosed as an error.

**Name Conflict Prevention**: Names introduced by `import as` or `use` declarations **MUST NOT** conflict with existing names in the same scope.

**Wildcard Use Warning**: Wildcard use declarations (`use path::*`) in modules that expose a `public` API **SHOULD** trigger a warning. This warning is informational and does not affect program validity.

**Diagnostic Table**

| Code         | Severity | Condition                                                        | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------------- | :----------- | :-------- |
| `E-MOD-1201` | Error    | Circular import dependency between assemblies.                   | Compile-time | Rejection |
| `E-MOD-1202` | Error    | Import of non-existent assembly or module.                       | Compile-time | Rejection |
| `E-MOD-1203` | Error    | Name introduced by `use` or `import as` conflicts with existing. | Compile-time | Rejection |
| `E-MOD-1204` | Error    | Item in `use` path not found or not accessible.                  | Compile-time | Rejection |
| `E-MOD-1205` | Error    | Attempt to `public use` a non-public item.                       | Compile-time | Rejection |
| `E-MOD-1206` | Error    | Duplicate item in a `use` list.                                  | Compile-time | Rejection |
| `E-NAM-1305` | Error    | Use of inaccessible item (visibility violation).                 | Compile-time | Rejection |
| `W-MOD-1201` | Warning  | Wildcard `use` (`*`) in a module exposing public API.            | Compile-time | N/A       |

---

### 8.7 Name Resolution

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
3. That member must be accessible from the current scope (§8.5)

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

---

### 8.8 Module Initialization

##### Definition

**Module Dependency Graph (MDG)**

The **Module Dependency Graph** is a directed graph $G = (V, E)$ where:
- $V$ is the set of all modules in the program and its compiled dependencies
- $E$ is a set of directed edges $(A, B)$, where an edge from module $A$ to module $B$ signifies that $A$ depends on $B$

**Edge Classification**

Each edge in $E$ is classified as either **Type-Level** or **Value-Level**:

| Edge Type   | Definition                                                                   |
| :---------- | :--------------------------------------------------------------------------- |
| Value-Level | Module $A$ contains an expression that evaluates a binding defined in $B$    |
| Type-Level  | Module $A$ refers to a type, form, or constant signature defined in $B$ only |

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

**Intra-Module Order**: Within a module, initializers **MUST** execute in strictly sequential lexical order within each source file. When a module comprises multiple source files, the order in which files are processed for initialization is **Implementation-Defined Behavior (IDB)** consistent with the file processing order documented per §8.1. Within each file, lexical order is preserved.

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

### 8.9 Program Entry Point

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
public procedure main(ctx: Context) -> i32
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
    heap: witness HeapAllocator,
    exec: witness Executor
}
```

##### Static Semantics

**Main Procedure Well-Formedness:**

1. Exactly one `main` procedure **MUST** exist in the entry assembly
2. The signature **MUST** match `main(ctx: Context) -> i32`
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

---

### Clause 8: Modules and Resolution Cross-Reference Notes `[INFORMATIVE]`

**Terms defined in Clause 8 that MUST NOT be redefined elsewhere:**

| Term                    | Section | Description                                                       |
| :---------------------- | :------ | :---------------------------------------------------------------- |
| Project                 | §8.1    | Top-level organizational unit with manifest and assemblies        |
| Assembly                | §8.1    | Collection of modules compiled as a single distributable unit     |
| Module                  | §8.1    | Fundamental unit of code organization (folder-as-module)          |
| Compilation Unit        | §8.1    | Source files constituting a single module                         |
| Module Path             | §8.3    | Unique identifier derived from filesystem path                    |
| Folder-as-Module        | §8.1    | Principle: directory = module, files contribute to namespace      |
| Scope                   | §8.4    | Region of source text where a set of names is valid               |
| Scope Context           | §8.4    | Ordered list of scope mappings ($\Gamma$)                         |
| Universe Scope          | §8.4    | Implicit scope containing primitive types and cursive.* namespace |
| Unified Namespace       | §8.4    | Single namespace shared by terms, types, and modules              |
| Visibility              | §8.5    | Access level: public, internal, private, protected                |
| Accessibility           | §8.5    | Whether an item can be referenced from a given context            |
| Import Declaration      | §8.6    | Inter-assembly dependency declaration                             |
| Use Declaration         | §8.6    | Scope alias for qualified paths                                   |
| Re-export               | §8.6    | Making imported items available to dependents via public use      |
| Name Introduction       | §8.7    | Adding a binding to a scope's namespace                           |
| Shadowing               | §8.7    | Inner scope binding obscuring outer scope binding                 |
| Name Lookup             | §8.7    | Process of finding the entity for an identifier                   |
| Module Dependency Graph | §8.8    | Directed graph representing inter-module dependencies             |
| Eager Dependency        | §8.8    | Value-level edge from module initializer                          |
| Lazy Dependency         | §8.8    | Type-level edge or value-level edge only within procedure bodies  |
| Entry Point             | §8.9    | The main procedure receiving root capabilities                    |
| Context Record          | §8.9    | Container for system capabilities passed to main                  |
| No Ambient Authority    | §8.9    | Principle: all effects require explicit capability                |

**Terms referenced from other clauses:**

| Term             | Source             | Usage in Clause 8                                   |
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
| witness FileSystem | §10.2                 | Capability form definition                        |
| witness Network    | §10.2                 | Capability form definition                        |
| System             | §10.2                 | System capability form                            |
| HeapAllocator      | §10.2                 | Allocator capability form                         |
| Panic propagation  | §9 (Concurrency)      | Error handling semantics                          |
| comptime           | §11 (Metaprogramming) | Compile-time evaluation for static initialization |

---

## Clause 9: Forms and Polymorphism

This clause defines the Cursive form system: the unified mechanism for defining interfaces, sharing implementations, and enabling polymorphism. Forms replace separate "behavior" and "contract" declarations from earlier designs, unifying them into a single `form` keyword.

### 9.1 Introduction to Forms

##### Definition

A **form** is a named declaration that defines an abstract interface consisting of procedure signatures and associated type declarations. Forms establish behavioral contracts that implementing types MUST satisfy.

**Formal Definition**

A form $Tr$ is defined as a tuple:

$$Tr = (N, G, S, P_{abs}, P_{con}, A_{abs}, A_{con})$$

where:

- $N$ is the form name (an identifier)
- $G$ is the (possibly empty) set of generic type parameters
- $S$ is the (possibly empty) set of superform bounds
- $P_{abs}$ is the set of abstract procedure signatures
- $P_{con}$ is the set of concrete procedure definitions (default implementations)
- $A_{abs}$ is the set of abstract associated type declarations
- $A_{con}$ is the set of concrete associated type bindings (defaults)

The **interface** of form $Tr$ is defined as:

$$\text{interface}(Tr) = P_{abs} \cup P_{con} \cup A_{abs} \cup A_{con}$$

A type $T$ **implements** form $Tr$ (written $T <: Tr$) when $T$ provides concrete definitions for all abstract members:

$$T <: Tr \iff \forall p \in P_{abs}.\ T \text{ defines } p\ \land\ \forall a \in A_{abs}.\ T \text{ binds } a$$

##### Static Semantics

**Polymorphism Dispatch Classification**

Cursive provides three distinct mechanisms for polymorphism, each with different dispatch semantics, performance characteristics, and use cases:

| Path | Name                 | Dispatch                        | Cost              | Use Case                  |
| :--- | :------------------- | :------------------------------ | :---------------- | :------------------------ |
| 1    | Static Polymorphism  | Compile-time (monomorphization) | Zero              | Constrained generics      |
| 2    | Dynamic Polymorphism | Runtime (vtable)                | One indirect call | Heterogeneous collections |
| 3    | Opaque Polymorphism  | Compile-time (type erasure)     | Zero              | Hidden return types       |

**Path Selection**

The polymorphism path is determined by usage context:

1. **Static (Path 1):** Selected when a generic parameter `T <: Tr` is used in a procedure signature. The compiler monomorphizes the procedure for each concrete type argument.

2. **Dynamic (Path 2):** Selected when `witness Tr` is used as a type. A vtable-based dense pointer enables runtime dispatch.

3. **Opaque (Path 3):** Selected when `opaque Tr` is used as a return type. The concrete type is hidden from callers while maintaining zero-cost dispatch within the defining module.

---

### 9.2 Form Declarations

##### Definition

A form declaration introduces a named interface that types MAY implement. Forms define requirements through abstract procedures and MAY provide default implementations through concrete procedures.

**Abstract Procedure**

An **abstract procedure** is a procedure signature within a form that lacks a body. Implementing types MUST provide a concrete implementation.

**Concrete Procedure (Default Implementation)**

A **concrete procedure** is a procedure definition within a form that includes a body. Implementing types automatically inherit this procedure but MAY override it using the `override` keyword.

**Associated Type**

An **associated type** is a type declaration within a form:
- If abstract (no `= T`): implementing types MUST provide a concrete type binding
- If concrete (`= T`): provides a default type that MAY be overridden

**The `Self` Type**

Within a form declaration, the identifier `Self` denotes the (unknown) implementing type. `Self` MUST be used for the receiver parameter and MAY be used in any other type position.

**Generic Form Parameters**

A form MAY declare generic type parameters that parameterize the entire form definition. These parameters are distinct from associated types:
- Generic parameters (`form Foo<T>`) are specified at use-site by the implementer/consumer
- Associated types are specified by the implementer within the type body

**Superform (Form Bounds)**

A form MAY extend one or more superforms using the `<:` operator. A type implementing a subform MUST also implement all of its superforms.

**Form Alias**

A **form alias** declares a new name that is equivalent to a combination of one or more form bounds. Form aliases enable concise expression of compound bounds.

##### Syntax & Declaration

**Grammar**

```ebnf
form_declaration ::=
    [ <visibility> ] "form" <identifier> [ <generic_params> ]
    [ "<:" <superform_bounds> ] "{"
        <form_item>*
    "}"

superform_bounds ::= <form_bound> ( "+" <form_bound> )*
form_bound ::= <type_path> [ <generic_args> ]

generic_params ::= "<" <generic_param> ( "," <generic_param> )* ">"
generic_param ::= <identifier> [ "<:" <form_bounds> ]

form_item ::=
    <abstract_procedure>
  | <concrete_procedure>
  | <associated_type>

abstract_procedure ::=
    "procedure" <identifier> <signature> [ <contract_clause> ] ";"

concrete_procedure ::=
    "procedure" <identifier> <signature> [ <contract_clause> ] <block>

contract_clause ::= "|=" <precondition_expr> [ "=>" <postcondition_expr> ]

associated_type ::=
    "type" <identifier> [ "=" <type> ] ";"

form_alias ::=
    [ <visibility> ] "type" <identifier> [ <generic_params> ]
    "=" <form_bound> ( "+" <form_bound> )* ";"
```

##### Static Semantics

**Well-Formedness (WF-Form)**

A form declaration is well-formed if:
1. The identifier is unique within its namespace
2. All procedure signatures are well-formed
3. All associated types have unique names within the form
4. No procedure name conflicts with an associated type name
5. All superform bounds refer to valid forms
6. No cyclic superform dependencies exist

**Formal Well-Formedness Rule**

$$\frac{
  \text{unique}(N) \quad
  \forall p \in P.\ \Gamma, \text{Self} : \text{Type} \vdash p\ \text{wf} \quad
  \forall a \in A.\ \Gamma \vdash a\ \text{wf} \quad
  \neg\text{cyclic}(S)
}{
  \Gamma \vdash \text{form } N\ [<: S]\ \{P, A\}\ \text{wf}
} \quad \text{(WF-Form)}$$

where:
- $N$ is the form name
- $P$ is the set of procedure declarations
- $A$ is the set of associated type declarations
- $S$ is the set of superform bounds

**Typing Rule for Self**

$$\frac{\Gamma, \text{Self} : \text{Type} \vdash \text{body} : \text{ok}}{\Gamma \vdash \text{form } T\ \{\ \text{body}\ \} : \text{Form}}$$
\tag{WF-Form-Self}

**Superform Inheritance (T-Superform)**

$$\frac{\text{form } A <: B \quad T <: A}{\Gamma \vdash T <: B}$$
\tag{T-Superform}

A type implementing subform `A` transitively implements all superforms.

**Generic Form Instantiation**

For a generic form `form Foo<T>`:
- Each use-site `Foo<ConcreteType>` produces a distinct form bound
- `T` is available within the form body as a type parameter

**Form Alias Equivalence (T-Alias-Equiv)**

$$\frac{\text{type } Alias = A + B}{\Gamma \vdash T <: Alias \iff \Gamma \vdash T <: A \land \Gamma \vdash T <: B}$$
\tag{T-Alias-Equiv}

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-TRS-2900` | Error    | Duplicate procedure name in form.              | Compile-time | Rejection |
| `E-TRS-2904` | Error    | Duplicate associated type name in form.        | Compile-time | Rejection |
| `E-TRS-2905` | Error    | Procedure name conflicts with associated type. | Compile-time | Rejection |
| `E-TRS-2908` | Error    | Cyclic superform dependency detected.          | Compile-time | Rejection |
| `E-TRS-2909` | Error    | Superform bound refers to undefined form.      | Compile-time | Rejection |

---

### 9.3 Form Implementation

##### Definition

**Form Implementation**

A type implements a form by:
1. Declaring the form in its "implements clause" using the `<:` operator
2. Providing implementations for all abstract procedures
3. Providing type bindings for all abstract associated types

**Implementation Site**

Form implementation MUST occur at the type's definition site. Extension implementations (implementing a form for a type defined elsewhere) are PROHIBITED.

##### Syntax & Declaration

**Grammar**

```ebnf
record_declaration ::=
    [ <visibility> ] "record" <identifier> [ <generic_params> ]
    [ "<:" <form_list> ]
    <record_body>

enum_declaration ::=
    [ <visibility> ] "enum" <identifier> [ <generic_params> ]
    [ "<:" <form_list> ]
    <enum_body>

modal_declaration ::=
    [ <visibility> ] "modal" <identifier> [ <generic_params> ]
    [ "<:" <form_list> ]
    <modal_body>

form_list ::= <type_path> ( "," <type_path> )*
```

##### Static Semantics

**Implementation Completeness (T-Impl-Complete)**

$$\frac{T <: Tr \quad \forall p \in \text{abstract\_procs}(Tr),\ T \text{ defines } p}{\Gamma \vdash T \text{ implements } Tr}$$
\tag{T-Impl-Complete}

**Implementation Well-Formedness (WF-Impl)**

A form implementation `T <: Tr` is well-formed if:

$$\frac{
  \Gamma \vdash T\ \text{wf} \quad
  \Gamma \vdash Tr\ \text{wf} \quad
  \forall p \in \text{abstract}(Tr).\ T \vdash p\ \text{implemented} \quad
  \forall a \in \text{abstract\_types}(Tr).\ T \vdash a\ \text{bound}
}{
  \Gamma \vdash T <: Tr\ \text{wf}
} \quad \text{(WF-Impl)}$$

**Override Semantics**

- **Abstract procedures**: implementation MUST NOT use the `override` keyword
- **Concrete procedures**: replacement MUST use the `override` keyword

**Coherence Rule**

A type `T` MAY implement a form `Tr` at most once. Multiple implementations of the same form for the same type are forbidden.

**Orphan Rule**

For `T <: Tr`, at least one of the following MUST be true:
- `T` is defined in the current assembly
- `Tr` is defined in the current assembly

This rule prevents external code from creating conflicting implementations.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-TRS-2901` | Error    | `override` used on abstract procedure implementation.  | Compile-time | Rejection |
| `E-TRS-2902` | Error    | Missing `override` on concrete procedure replacement.  | Compile-time | Rejection |
| `E-TRS-2903` | Error    | Type does not implement required procedure from form.  | Compile-time | Rejection |
| `E-TRS-2906` | Error    | Coherence violation: duplicate form implementation.    | Compile-time | Rejection |
| `E-TRS-2907` | Error    | Orphan rule violation: neither type nor form is local. | Compile-time | Rejection |

---

### 9.4 Static Polymorphism (Generics)

##### Definition

**Static Polymorphism**

Static polymorphism is zero-cost, compile-time dispatch using generic parameters constrained by forms.

**Constrained Generic**

A **constrained generic** is a generic parameter `T` constrained by form `Tr` (written `T <: Tr`), which restricts `T` to types implementing `Tr`.

**Monomorphization**

**Monomorphization** is the process by which the compiler generates specialized versions of generic procedures for each concrete type used as an argument. Each instantiation produces distinct machine code with direct (non-virtual) calls.

##### Syntax & Declaration

**Grammar**

```ebnf
generic_param_constrained ::= <identifier> "<:" <form_list>
form_list ::= <type_path> ( "," <type_path> )*
```

##### Static Semantics

**Constraint Satisfaction (T-Constraint)**

A call `f<T>(x)` where `f` requires `T <: Tr` is valid if and only if:

$$\frac{\Gamma \vdash T \text{ implements } Tr}{\Gamma \vdash f\langle T \rangle(x) : \text{valid}}$$
\tag{T-Constraint}

**Generic Instantiation (T-Generic-Inst)**

When a generic procedure is instantiated with a concrete type argument, the type of the instantiated procedure is derived by substituting the concrete type for the type parameter:

$$\frac{
  \Gamma \vdash f : \forall T <: Tr.\ (T, P_1, \ldots, P_n) \to R \quad
  \Gamma \vdash C <: Tr
}{
  \Gamma \vdash f\langle C \rangle : (C, P_1, \ldots, P_n) \to R[C/T]
} \quad \text{(T-Generic-Inst)}$$

Where:
- $f$ is a generic procedure with type parameter $T$ constrained by form $Tr$
- $C$ is a concrete type satisfying the constraint $C <: Tr$
- $R[C/T]$ denotes the return type with all occurrences of $T$ substituted by $C$

**Multiple Constraints (T-Multi-Constraint)**

When a type parameter has multiple form bounds, all constraints MUST be satisfied:

$$\frac{
  \Gamma \vdash f : \forall T <: Tr_1, Tr_2, \ldots, Tr_n.\ (T) \to R \quad
  \forall i \in 1..n.\ \Gamma \vdash C <: Tr_i
}{
  \Gamma \vdash f\langle C \rangle : (C) \to R[C/T]
} \quad \text{(T-Multi-Constraint)}$$

**Monomorphization Semantics**

For a generic procedure:
```cursive
procedure process<T <: Form>(item: T) { item~>method() }
```

A call `process<Point>(p)` MUST be compiled as:
1. Generate specialized function `process_Point`
2. Replace `item~>method()` with direct call `Point::method(item)`
3. No vtable lookup; direct static dispatch

**Zero Runtime Overhead**

Static polymorphism MUST incur zero runtime overhead compared to non-generic code. The monomorphized code MUST be equivalent in performance to hand-written specialized code.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-TRS-2930` | Error    | Type argument does not satisfy form constraint.   | Compile-time | Rejection |
| `E-TRS-2931` | Error    | Unconstrained type parameter used in form method. | Compile-time | Rejection |

---

### 9.5 Dynamic Polymorphism (Witnesses)

##### Definition

**Dynamic Polymorphism**

Dynamic polymorphism is opt-in runtime dispatch using form witnesses. It enables heterogeneous collections and runtime polymorphism at the cost of one vtable lookup per call.

**Witness**

A **witness** (`witness Form`) is a concrete, sized type representing any value implementing a witness-safe form. It is implemented as a dense pointer (dense pointer).

**Witness Safety (Object Safety)**

A form is **witness-safe** if every procedure in the form (including inherited ones) is either:
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
witness_type ::= "witness" <form_path>
```

##### Static Semantics

**Witness Formation (T-Witness-Form)**

A value of concrete type `T` implementing witness-safe form `Tr` MAY be coerced to `witness Tr`:

$$\frac{\Gamma \vdash v : T \quad \Gamma \vdash T <: Tr \quad \text{witness\_safe}(Tr)}{\Gamma \vdash v : \text{witness } Tr}$$
\tag{T-Witness-Form}

##### Dynamic Semantics

**Evaluation**

**Witness Creation**

Formation of a witness from a concrete value proceeds as follows:

1. Let `v` be a value of concrete type `T` where `T <: Tr` and `witness_safe(Tr)`.
2. Let `dp` be a pointer to the storage of `v`.
3. Let `vt` be the static vtable for the `(T, Tr)` type-form pair.
4. Construct the witness value as the pair `(dp, vt)`.

**Formal Witness Formation Rule**

$$\frac{
  \Gamma \vdash v : T \quad
  T <: Tr \quad
  \text{witness\_safe}(Tr)
}{
  v \Rightarrow_{\text{witness}} (\text{ptr}(v), \text{vtable}(T, Tr))
} \quad \text{(E-Witness-Form)}$$

**VTable Dispatch Algorithm**

A procedure call `w~>method(args)` on witness `w: witness Tr` executes as follows:

1. Let `(dp, vt)` be the data pointer and vtable pointer components of `w`.
2. Let `offset` be the vtable offset for `method` (determined at compile time from form declaration order, offset by header size).
3. Let `fp` be the function pointer at `vt + 3 * sizeof(usize) + offset * sizeof(usize)`.
4. Return the result of calling `fp(dp, args)`.

**Formal Dispatch Rule**

$$\frac{
  w = (dp, vt) \quad
  \text{method} \in \text{interface}(Tr) \quad
  vt[\text{offset}(\text{method})] = fp
}{
  w\text{\textasciitilde>method}(args) \to fp(dp, args)
} \quad \text{(E-Witness-Dispatch)}$$

##### Memory & Layout

**Dense Pointer Layout**

A witness (`witness Form`) is represented as a two-word structure:

```
struct WitnessRepr {
    data: *imm (),      // Pointer to concrete instance
    vtable: *imm VTable // Pointer to form vtable
}
```

- **Size**: `2 * sizeof(usize)` (16 bytes on 64-bit platforms)
- **Alignment**: `alignof(usize)`

**VTable Layout (Stable ABI)**

VTable entries MUST appear in this exact order:
1. `size: usize` — Size of concrete type in bytes
2. `align: usize` — Alignment requirement of concrete type
3. `drop: *imm fn` — Destructor function pointer (null if no `Drop`)
4. `methods[..]` — Function pointers in form declaration order

> **Rationale:** Fixed layout enables separate compilation and FFI vtable compatibility.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-TRS-2940` | Error    | Procedure with `where Self: Sized` called on witness.   | Compile-time | Rejection |
| `E-TRS-2941` | Error    | Witness created from non-witness-safe form.             | Compile-time | Rejection |
| `E-TRS-2942` | Error    | Generic procedure in form without `Self: Sized` clause. | Compile-time | Rejection |

---

### 9.6 Opaque Polymorphism

##### Definition

**Opaque Polymorphism**

Opaque polymorphism is zero-cost compile-time polymorphism for return types. The concrete type is hidden from callers; only the form interface is accessible.

**Opaque Type**

An **opaque return type** (`opaque Form`) exposes only the form's interface while hiding the concrete implementation type.

##### Syntax & Declaration

**Grammar**

```ebnf
return_type ::= ... | "opaque" <form_path>
```

##### Static Semantics

**Type Encapsulation**

For a procedure returning `opaque Form`:
- The callee returns a concrete type implementing `Form`
- The caller observes only `Form` members
- Access to concrete type members is forbidden

**Opaque Return Typing (T-Opaque-Return)**

A procedure body returning a concrete type is well-typed against an opaque return type declaration:

$$\frac{
  \Gamma \vdash \text{body} : T \quad
  \Gamma \vdash T <: Tr \quad
  \text{return\_type}(f) = \text{opaque } Tr
}{
  \Gamma \vdash f : () \to \text{opaque } Tr
} \quad \text{(T-Opaque-Return)}$$

Where:
- The procedure body evaluates to a value of concrete type $T$
- The concrete type $T$ implements form $Tr$
- The declared return type is `opaque Tr`

**Opaque Type Projection (T-Opaque-Project)**

At call sites, the opaque type is treated as an existential; callers may invoke only form methods:

$$\frac{
  \Gamma \vdash f() : \text{opaque } Tr \quad
  m \in \text{interface}(Tr) \quad
  \text{signature}(Tr.m) = (S_1, \ldots, S_n) \to R_m
}{
  \Gamma \vdash f().\text{~>}m(a_1, \ldots, a_n) : R_m
} \quad \text{(T-Opaque-Project)}$$

Where:
- $f()$ returns an opaque type with form bound $Tr$
- $m$ is a procedure declared in form $Tr$ (or its superforms)
- The call is well-typed if arguments match the procedure signature

**Opaque Type Equality**

Two opaque types `opaque Tr` are considered equivalent types if and only if they originate from the same procedure definition. Different procedures returning `opaque Tr` produce distinct, incompatible types:

$$\frac{
  \Gamma \vdash f : () \to \text{opaque } Tr \quad
  \Gamma \vdash g : () \to \text{opaque } Tr \quad
  f \neq g
}{
  \text{typeof}(f()) \neq \text{typeof}(g())
} \quad \text{(T-Opaque-Distinct)}$$

**Zero Overhead**

Opaque types MUST incur zero runtime overhead. The returned value is the concrete type, not a dense pointer. Type encapsulation is enforced statically.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                            | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :----------- | :-------- |
| `E-TRS-2910` | Error    | Accessing member not defined on opaque type's form.  | Compile-time | Rejection |
| `E-TRS-2911` | Error    | Opaque return type does not implement required form. | Compile-time | Rejection |
| `E-TRS-2912` | Error    | Attempting to assign incompatible opaque types.      | Compile-time | Rejection |

---

### 9.7 Foundational Forms

##### Definition

**Foundational Forms**

Foundational forms are forms fundamental to language semantics. The normative definitions reside in **Appendix D.1**. This section provides a summary and cross-reference.

| Form       | Purpose                      | Key Constraint               |
| :--------- | :--------------------------- | :--------------------------- |
| `Drop`     | Deterministic cleanup (RAII) | Compiler-invoked only        |
| `Copy`     | Implicit bitwise duplication | Mutual exclusion with `Drop` |
| `Clone`    | Explicit deep copy           | `clone(~) -> Self`           |
| `Iterator` | Iteration protocol           | `next(~!) -> Option<Item>`   |

##### Static Semantics

**Drop Semantics (Summary)**

- **Signature**: `procedure drop(~!)`
- The compiler automatically invokes `Drop::drop` on responsible bindings at scope exit
- Direct calls to `Drop::drop` are FORBIDDEN

**Copy Semantics (Summary)**

- Marker form (no procedures)
- Enables implicit bitwise duplication instead of move
- A type MUST NOT implement both `Copy` and `Drop`
- All fields of a `Copy` type MUST also be `Copy`

**Clone Semantics (Summary)**

- **Signature**: `procedure clone(~) -> Self`
- Explicit duplication (deep copy)
- `Copy` types SHOULD also implement `Clone`

**Iterator Semantics (Summary)**

- **Associated type**: `type Item`
- **Signature**: `procedure next(~!): Option<Item>`
- Stateful advancement; used by `loop ... in` construct

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-TRS-2920` | Error    | Direct call to `Drop::drop`.            | Compile-time | Rejection |
| `E-TRS-2921` | Error    | Type implements both `Copy` and `Drop`. | Compile-time | Rejection |
| `E-TRS-2922` | Error    | `Copy` type contains non-`Copy` field.  | Compile-time | Rejection |

---

### Clause 9: Forms and Polymorphism Cross-Reference Notes `[INFORMATIVE]`

**Terms defined in Clause 9 that MUST NOT be redefined elsewhere:**

| Term                   | Section | Description                                                                |
| :--------------------- | :------ | :------------------------------------------------------------------------- |
| Form                   | §9.1    | A declaration defining an abstract interface of procedures and types.      |
| Abstract Procedure     | §9.2    | A procedure signature in a form without a body.                            |
| Concrete Procedure     | §9.2    | A procedure definition in a form with a body (default implementation).     |
| Associated Type        | §9.2    | A type declaration within a form, either abstract or with a default.       |
| Generic Form Parameter | §9.2    | Type parameters declared on a form, specified at use-site.                 |
| Superform              | §9.2    | A form required by another form via `<:` in form declaration.              |
| Form Alias             | §9.2    | A named alias for a combination of form bounds.                            |
| Form Implementation    | §9.3    | The act of a type satisfying a form's requirements via `<:`.               |
| Coherence Rule         | §9.3    | A type MAY implement a form at most once.                                  |
| Orphan Rule            | §9.3    | Form implementation requires locality of type or form to current assembly. |
| Monomorphization       | §9.4    | Compile-time specialization of generic code for concrete types.            |
| Constrained Generic    | §9.4    | A generic parameter bounded by one or more forms.                          |
| Witness                | §9.5    | A dense pointer type enabling runtime polymorphism over a form.            |
| Witness Safety         | §9.5    | Property of a form permitting witness creation.                            |
| VTable Eligibility     | §9.5    | Criteria for procedure inclusion in a witness vtable.                      |
| Opaque Type            | §9.6    | A return type hiding the concrete implementation behind a form interface.  |

**Terms referenced:**

| Term                    | Source | Usage in Clause 9                                 |
| :---------------------- | :----- | :------------------------------------------------ |
| Type Context ($\Gamma$) | §4.1   | Used in typing judgments for form bounds.         |
| Subtype Relation ($<:$) | §4.2   | Form implementation creates subtype relationship. |
| Permission              | §4.5   | Receiver shorthands (`~`, `~!`, `~                | `) imply permissions. |
| `const`                 | §4.5   | Default permission for `self` in form methods.    |
| `unique`                | §4.5   | Permission required for `Drop::drop` receiver.    |
| Record                  | §4.8   | Primary type that implements forms.               |
| Enum                    | §4.9   | May implement forms.                              |
| Modal Type              | §4.11  | May implement forms.                              |
| Visibility              | §5.2   | Controls form and procedure accessibility.        |
| Assembly                | §5.1   | Compilation unit for orphan rule.                 |

**Terms deferred to later clauses/appendices:**

| Term                   | Deferred To  | Reason                                         |
| :--------------------- | :----------- | :--------------------------------------------- |
| `Drop` (full spec)     | Appendix D.1 | Normative definition with all constraints.     |
| `Copy` (full spec)     | Appendix D.1 | Normative definition with all constraints.     |
| `Clone` (full spec)    | Appendix D.1 | Normative definition with all constraints.     |
| `Iterator` (full spec) | Appendix D.1 | Normative definition with protocol details.    |
| `Context`              | Appendix E   | Root capability record referenced in examples. |

---

## Clause 10: Contracts and Verification

This clause defines the formal semantics, syntax, and verification rules for Contracts in Cursive. Contracts are the primary mechanism for specifying behavioral properties of code beyond the type system. They govern the logical validity of procedures, types, and loops through preconditions, postconditions, and invariants.

---

### 10.1 Contract Fundamentals

##### Definition

A **Contract** is a specification attached to a procedure declaration that asserts logical predicates over program state. Contracts govern the logical validity of procedures through preconditions (caller obligations) and postconditions (callee guarantees).

**Formal Definition**

A contract $C$ is a pair $(P_{pre}, P_{post})$ where:
- $P_{pre}$ is a conjunction of boolean predicates representing preconditions
- $P_{post}$ is a conjunction of boolean predicates representing postconditions

$$C = (P_{pre}, P_{post}) \quad \text{where} \quad P_{pre}, P_{post} \in \text{Predicate}^*$$

A **Predicate** is a pure boolean expression evaluated in a specific evaluation context (see §10.1.2).

**Distinguished from Capabilities**

Contracts and Capabilities (Clause 13) serve orthogonal purposes:
- **Contracts** control the *logical validity* of data and operations
- **Capabilities** control the *authority* to perform external effects

A procedure may have both contracts (specifying what values are valid) and capability requirements (specifying what effects it may perform).

##### Syntax & Declaration

**Grammar**

```ebnf
contract_clause
    ::= "|=" contract_body

contract_body
    ::= precondition_expr
      | precondition_expr "=>" postcondition_expr
      | "=>" postcondition_expr

precondition_expr
    ::= predicate_expr

postcondition_expr
    ::= predicate_expr

predicate_expr
    ::= logical_or_expr

logical_or_expr
    ::= logical_and_expr
      | logical_or_expr "||" logical_and_expr

logical_and_expr
    ::= logical_not_expr
      | logical_and_expr "&&" logical_not_expr

logical_not_expr
    ::= "!" logical_not_expr
      | primary_predicate

primary_predicate
    ::= comparison_expr
      | "(" predicate_expr ")"
      | identifier
      | procedure_call
      | field_access
      | contract_intrinsic

contract_intrinsic
    ::= "@result"
      | "@entry" "(" expression ")"
```

**Positioning**

A contract clause (introduced by `|=`) MUST appear after the return type annotation (or after the parameter list if no return type is specified) and before the procedure body or terminating semicolon.

```cursive
// Contract after return type, before body
procedure divide(a: i32, b: i32) -> i32
    |= b != 0 => @result * b == a
{
    a / b
}

// Contract on procedure without return type annotation
procedure assert_positive(x: i32)
    |= x > 0
{
    // ...
}
```

**Rationale: The `|=` Symbol**

The `|=` symbol (semantic entailment) is borrowed from mathematical logic, where $\Gamma \models P$ means "in all models where $\Gamma$ holds, $P$ also holds." For procedure contracts:

- The procedure signature and precondition form the context $\Gamma$
- The postcondition is the entailed proposition $P$
- The contract asserts: "in all executions satisfying the precondition, the postcondition holds"

This symbol was chosen to distinguish **contractual assertions** (obligations about program behavior) from **definitional constraints** (restrictions on value sets), which use `where`. The distinction clarifies:

| Syntax          | Keyword | Meaning                         | Semantic Category |
| :-------------- | :------ | :------------------------------ | :---------------- |
| `T where { P }` | `where` | "values of T such that P"       | Definitional      |
| `\|= P => Q`    | `\|=`   | "if P at entry, then Q at exit" | Contractual       |

##### Static Semantics

**Well-Formedness Rule (WF-Contract)**

A contract clause is well-formed if and only if both its precondition and postcondition expressions are well-formed predicates:

$$
\frac{
    \Gamma_{pre} \vdash P_{pre} : \texttt{bool} \quad \text{pure}(P_{pre}) \\
    \Gamma_{post} \vdash P_{post} : \texttt{bool} \quad \text{pure}(P_{post})
}{
    \Gamma \vdash \texttt{|=}\ P_{pre} \Rightarrow P_{post} : \text{WF}
}
\tag{WF-Contract}
$$

where:
- $\Gamma_{pre}$ is the precondition evaluation context (§10.1.2)
- $\Gamma_{post}$ is the postcondition evaluation context (§10.1.2)
- $\text{pure}(e)$ holds iff $e$ satisfies all purity constraints (§10.1.1)

**Logical Operators**

Predicates use standard boolean operators with the following precedence (highest to lowest):
1. `!` (logical NOT) — right-associative
2. `&&` (logical AND) — left-associative
3. `||` (logical OR) — left-associative

Short-circuit evaluation applies: `&&` does not evaluate its right operand if the left is `false`; `||` does not evaluate its right operand if the left is `true`.

##### Constraints & Legality

A syntactically malformed contract clause causes the program to be ill-formed; the implementation issues a diagnostic per §2.12 (grammar enforcement).

---

#### 10.1.1 Purity Constraints

##### Definition

A **pure expression** is an expression whose evaluation produces no observable side effects. All expressions within a contract MUST be pure.

##### Static Semantics

**Purity via the Capability System**

In Cursive, procedures are **pure by default**. A procedure becomes impure only when it accepts capability parameters (Clause 13). This design means that purity is not a separate annotation but a natural consequence of the capability system.

An expression $e$ satisfies $\text{pure}(e)$ if and only if all of the following hold:

1. **No Capability Use**: $e$ MUST NOT invoke any procedure that accepts capability parameters. Procedures without capability parameters are inherently pure.

2. **No Mutation Through Effects**: $e$ MUST NOT mutate state observable outside the expression's evaluation.

3. **Built-in Operations**: Built-in operators on primitive types are always pure.

4. **Compile-Time Procedures**: `comptime` procedures are pure by construction (they cannot access capabilities).

**Determining Purity**

A procedure call in a contract predicate is valid if and only if the callee's signature contains no capability parameters (`witness` bindings):

```cursive
// Pure: no capability parameters
procedure len(~) -> usize { /* ... */ }

// Impure: requires FileSystem capability
procedure read_file(path: string, fs: witness FileSystem) -> string { /* ... */ }
```

The first procedure may be called in contracts; the second may not.

##### Constraints & Legality

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CON-2802` | Error    | Impure expression used in contract predicate. | Compile-time | Rejection |

---

#### 10.1.2 Evaluation Contexts

##### Definition

An **Evaluation Context** defines the set of bindings available for reference within a predicate expression. Preconditions and postconditions have distinct evaluation contexts.

##### Static Semantics

**Precondition Evaluation Context ($\Gamma_{pre}$)**

The precondition evaluation context includes:
- All procedure parameters at their entry state
- The receiver binding (if present), accessible via receiver shorthand (`~`, `~!`, `~%`)
- All bindings visible in the enclosing scope that are accessible without side effects

The precondition context MUST NOT include:
- The `@result` intrinsic
- The `@entry` operator
- Any binding introduced within the procedure body

**Postcondition Evaluation Context ($\Gamma_{post}$)**

The postcondition evaluation context includes:
- All procedure parameters:
  - Immutable parameters (`const`, `~`): same value as at entry
  - Mutable parameters (`unique`, `shared`): post-state value
- The receiver binding (post-state for mutable receivers)
- The `@result` intrinsic (§10.2.2)
- The `@entry` operator (§10.2.2)
- All bindings visible in the enclosing scope

**Mutable Parameter State Semantics**

For parameters with mutable permission (`~!`, `~%`):

| Location in Contract          | State Referenced |
| :---------------------------- | :--------------- |
| Left of `=>`                  | Entry state      |
| Right of `=>` (bare)          | Post-state       |
| Right of `=>` with `@entry()` | Entry state      |

```cursive
record Counter {
    value: i32,

    procedure increment(~!)
        |= self.value >= 0 => self.value == @entry(self.value) + 1
    {
        self.value += 1;
    }
}
```

In this example:
- `self.value >= 0` (left of `=>`) references the entry state (precondition)
- `self.value == @entry(self.value) + 1` (right of `=>`) asserts post-state equals entry + 1

---

#### 10.1.3 Inline Parameter Constraints

##### Definition

An **Inline Parameter Constraint** is a `where` clause attached directly to a parameter declaration, specifying a precondition on that parameter.

##### Syntax & Declaration

```ebnf
parameter_with_constraint
    ::= identifier ":" type_expr "where" "{" predicate_expr "}"
```

**Example**

```cursive
procedure sqrt(x: f64 where { x >= 0.0 }) -> f64
    |= => @result >= 0.0 && @result * @result == x
{
    // ...
}
```

Note: The inline `where` on the parameter is **type refinement** (definitional), while the procedure contract uses `|=` (contractual).

##### Static Semantics

**Semantic Equivalence**

Inline parameter constraints are semantically equivalent to procedure-level preconditions. Given:
- Inline constraints $\{P_1, P_2, \ldots, P_n\}$ on parameters
- Procedure-level precondition $P_{proc}$

The **effective precondition** is:

$$P_{eff} = P_1 \land P_2 \land \cdots \land P_n \land P_{proc}$$

**Evaluation Order**

Inline constraints are evaluated left-to-right in parameter declaration order, followed by the procedure-level precondition. Short-circuit semantics apply within each predicate but not across the conjunction of predicates.

**Scope Within Inline Constraints**

An inline constraint on parameter $p_i$ MAY reference:
- Parameters $p_1, \ldots, p_{i-1}$ (earlier parameters)
- The parameter $p_i$ itself (via `self` within the constraint)

An inline constraint MUST NOT reference parameters declared after it.

```cursive
// Valid: y's constraint references x
procedure example(
    x: i32 where { x > 0 },
    y: i32 where { y > x }   // References earlier parameter x
) { /* ... */ }

// Invalid: x's constraint references y (declared later)
procedure invalid(
    x: i32 where { x < y },  // Error: y not yet in scope
    y: i32
) { /* ... */ }
```

##### Constraints & Legality

An inline constraint MUST reference only parameters declared to its left. Referencing a parameter not yet in scope causes the program to be ill-formed (standard name resolution rules apply per §8.3).

---

### 10.2 Contract Clauses

This section defines the semantics of preconditions and postconditions within the `|=` contract clause syntax.

---

#### 10.2.1 Preconditions

##### Definition

A **Precondition** is the logical expression appearing to the left of `=>` in a `|=` contract clause, or the entire expression if `=>` is absent. The caller MUST ensure this expression evaluates to `true` at the call site prior to transfer of control to the callee.

##### Static Semantics

**Caller Obligation Rule**

When a procedure $f$ with precondition $P_{pre}$ is called at site $S$, the implementation MUST verify that a Verification Fact (§10.5) $F(P_{pre}, L)$ exists where $L$ dominates $S$ in the Control Flow Graph.

$$
\frac{
    \Gamma \vdash f : (T_1, \ldots, T_n) \to R \quad
    \text{precondition}(f) = P_{pre} \quad
    \exists L.\ F(P_{pre}, L) \land L \text{ dom } S
}{
    \Gamma \vdash f(a_1, \ldots, a_n) @ S : \text{valid}
}
\tag{Pre-Satisfied}
$$

**Attribution**

Failure to satisfy a precondition is attributed to the **caller**. The diagnostic MUST identify the call site, not the callee's declaration.

**Elision Rules**

| Contract Form        | Precondition              |
| :------------------- | :------------------------ |
| `\|= P`              | $P$                       |
| `\|= P => Q`         | $P$                       |
| `\|= => Q`           | `true` (always satisfied) |
| (no contract clause) | `true` (always satisfied) |

##### Constraints & Legality

**Scope Restrictions**

Precondition expressions:
- MAY reference all procedure parameters (at entry state)
- MAY reference receiver shorthand (`~`, `~!`, etc.)
- MUST NOT reference `@result` (diagnosed as `E-CON-2806`)
- MUST NOT reference `@entry()` (no entry state exists to compare against)
- MUST NOT reference bindings introduced in the procedure body

Precondition verification follows the strategy specified by `[[verify(...)]]` (§10.4). A precondition that cannot be statically verified in `static` mode is diagnosed as `E-CON-2801`. In `dynamic` mode, a runtime check is inserted; failure causes a panic.

---

#### 10.2.2 Postconditions

##### Definition

A **Postcondition** is the logical expression appearing to the right of `=>` in a `|=` contract clause. The callee MUST ensure this expression evaluates to `true` immediately before returning control to the caller.

##### Static Semantics

**Callee Obligation Rule**

The procedure implementation MUST ensure the postcondition holds at every return point:

$$
\frac{
    \text{postcondition}(f) = P_{post} \quad
    \forall r \in \text{ReturnPoints}(f).\ \Gamma_r \vdash P_{post} : \text{satisfied}
}{
    f : \text{postcondition-valid}
}
\tag{Post-Valid}
$$

**Attribution**

Failure to satisfy a postcondition is attributed to the **callee**. The diagnostic MUST identify the return point within the procedure body.

**Elision Rules**

| Contract Form        | Postcondition             |
| :------------------- | :------------------------ |
| `\|= P`              | `true` (no postcondition) |
| `\|= P => Q`         | $Q$                       |
| `\|= => Q`           | $Q$                       |
| (no contract clause) | `true` (no postcondition) |

##### Dynamic Semantics

**Contract Intrinsics**

Two intrinsics are available exclusively in postcondition expressions:

**1. `@result` Intrinsic**

| Property     | Specification                                                             |
| :----------- | :------------------------------------------------------------------------ |
| Availability | Postcondition expressions only (right of `=>`)                            |
| Type         | The return type of the enclosing procedure                                |
| Value        | The value being returned from the procedure                               |
| Unit Returns | If procedure returns `()`, `@result` has value `()`                       |
| Shadowing    | `@result` is an intrinsic keyword; it MUST NOT be shadowed by any binding |

```cursive
procedure abs(x: i32) -> i32
    |= => @result >= 0 && (@result == x || @result == -x)
{
    if x >= 0 { x } else { -x }
}
```

**2. `@entry(expr)` Operator**

| Property              | Specification                                          |
| :-------------------- | :----------------------------------------------------- |
| Availability          | Postcondition expressions only (right of `=>`)         |
| Semantics             | Evaluates `expr` in the entry state of the procedure   |
| Evaluation Point      | After parameter binding, before body execution         |
| Expression Constraint | `expr` MUST be pure                                    |
| Expression Scope      | `expr` MUST reference only parameters and receiver     |
| Type Constraint       | Result type of `expr` MUST implement `Copy` or `Clone` |
| Capture Mechanism     | Implementation captures the value at procedure entry   |

**Capture Semantics**

When `@entry(expr)` appears in a postcondition:
1. The implementation evaluates `expr` immediately after parameter binding
2. The result is captured (copied or cloned) and stored
3. In the postcondition, `@entry(expr)` refers to this captured value

```cursive
record Vec<T> {
    // ...

    procedure push<T: Copy>(~!, item: T)
        |= => self.len() == @entry(self.len()) + 1
    {
        // Implementation...
    }
}
```

**Non-Copyable Resources**

Direct capture of non-copyable resources is prohibited. To reference properties of non-copyable resources, capture a copyable projection:

```cursive
// Invalid: Buffer is not Copy
procedure process(buf: unique Buffer)
    |= => buf == @entry(buf)  // Error: Buffer does not implement Copy

// Valid: Capture the length (usize is Copy)
procedure process(buf: unique Buffer)
    |= => buf.len() == @entry(buf.len())  // OK
```

##### Constraints & Legality

**Scope Permissions**

Postcondition expressions:
- MAY reference all procedure parameters (post-state for mutable)
- MAY reference receiver shorthand (`~`, `~!`, etc.)
- MAY reference `@result`
- MAY reference `@entry(expr)` where `expr` references only parameters/receiver

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CON-2805` | Error    | `@entry()` result type does not implement Copy/Clone. | Compile-time | Rejection |
| `E-CON-2806` | Error    | `@result` used outside postcondition context.         | Compile-time | Rejection |

Postcondition verification follows the strategy specified by `[[verify(...)]]` (§10.4). A postcondition that cannot be statically verified in `static` mode is diagnosed as `E-CON-2801`. In `dynamic` mode, a runtime check is inserted; failure causes a panic.

---

### 10.3 Invariants

##### Definition

An **Invariant** is a predicate that MUST hold true over a defined scope or lifetime. Cursive defines two forms of invariants: Type Invariants and Loop Invariants.

> **Cross-Reference:** Refinement Types (§4.19) provide anonymous invariants attached to type references (e.g., `usize where { self < len }`). Refinement types are defined in Clause 4 as they constitute a subtype form. Verification of refinement type predicates uses the mechanisms defined in §10.4 and §10.5.

---

#### 10.3.1 Type Invariants

##### Definition

A **Type Invariant** is a `where` clause attached to a `record`, `enum`, or `modal` type declaration. The invariant constrains all instances of the type: every instance MUST satisfy the invariant at designated enforcement points.

##### Syntax & Declaration

**Grammar**

```ebnf
record_declaration
    ::= [ visibility ] "record" identifier [ generic_params ] 
        "{" field_list "}" [ type_invariant ]

enum_declaration
    ::= [ visibility ] "enum" identifier [ generic_params ]
        "{" variant_list "}" [ type_invariant ]

modal_declaration
    ::= [ visibility ] "modal" identifier [ generic_params ]
        "{" state_list "}" [ type_invariant ]

type_invariant
    ::= "where" "{" predicate_expr "}"
```

**Example**

```cursive
record NonEmptyList<T> {
    head: T,
    tail: Vec<T>,
}
where { self.len() >= 1 }

record BoundedCounter {
    value: u32,
    max: u32,
}
where { self.value <= self.max }
```

##### Static Semantics

**Invariant Predicate Context**

Within a type invariant predicate:
- `self` refers to an instance of the type being defined
- Field access on `self` is permitted
- Method calls on `self` are permitted if the method accepts no capability parameters (i.e., is pure)

**Enforcement Points (Boundary Points)**

A type invariant MUST be verified at the following program points:

| Enforcement Point   | Description                                                          |
| :------------------ | :------------------------------------------------------------------- |
| Post-Construction   | Immediately after a constructor or literal initialization completes  |
| Pre-Call (Public)   | Before any public procedure is invoked with the instance as receiver |
| Post-Call (Mutator) | Before any procedure taking `unique` `self` returns                  |

**Effective Postcondition Desugaring**

For a procedure $P$ on type $T$ with:
- Type invariant $Inv$
- Explicit postcondition $Post$
- Mutable receiver (`unique`)

The **effective postcondition** is:

$$\text{EffectivePost}(P) = Post \land Inv(\texttt{self})$$

The implementation MUST verify both the explicit postcondition and the type invariant before return.

**Private Procedure Exemption**

Private procedures (visibility `internal` or less) are exempt from the Pre-Call enforcement point. This permits temporary invariant violations during internal state transitions:

```cursive
record Counter {
    value: u32,
    max: u32,

    // Public: invariant checked before entry
    public procedure increment(~!) {
        self.unsafe_increment();  // May temporarily violate
        self.clamp();             // Restores invariant
    }  // Invariant checked at return (Post-Call)

    // Private: invariant NOT checked before entry
    internal procedure unsafe_increment(~!) {
        self.value += 1;  // May violate: value > max
    }

    internal procedure clamp(~!) {
        if self.value > self.max {
            self.value = self.max;
        }
    }
}
where { self.value <= self.max }
```

##### Constraints & Legality

| Code         | Severity | Condition                                            | Detection | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :-------- | :-------- |
| `E-CON-2820` | Error    | Type invariant violated at construction.             | See §10.4 | See §10.4 |
| `E-CON-2821` | Error    | Type invariant violated at public procedure entry.   | See §10.4 | See §10.4 |
| `E-CON-2822` | Error    | Type invariant violated at mutator procedure return. | See §10.4 | See §10.4 |

---

#### 10.3.2 Loop Invariants

##### Definition

A **Loop Invariant** is a `where` clause attached to a `loop` expression. The invariant specifies a predicate that MUST hold at the beginning of every iteration and after the loop terminates.

##### Syntax & Declaration

**Grammar**

```ebnf
loop_expression
    ::= "loop" [ loop_condition ] [ loop_invariant ] block

loop_invariant
    ::= "where" "{" predicate_expr "}"

loop_condition
    ::= expression
```

**Example**

```cursive
var sum = 0;
var i = 0;

loop i < n
    where { sum == i * (i - 1) / 2 && i <= n }
{
    sum += i;
    i += 1;
}
// After loop: sum == n * (n - 1) / 2
```

##### Static Semantics

**Enforcement Points**

A loop invariant $Inv$ MUST be verified at three points:

| Point          | Description                               | Formal                                                 |
| :------------- | :---------------------------------------- | :----------------------------------------------------- |
| Initialization | Before the first iteration begins         | $\Gamma_0 \vdash Inv$                                  |
| Maintenance    | At the start of each subsequent iteration | $\Gamma_i \vdash Inv \implies \Gamma_{i+1} \vdash Inv$ |
| Termination    | Immediately after loop terminates         | $\Gamma_{exit} \vdash Inv$                             |

**Verification Fact Generation**

Upon successful verification of a loop invariant at the Termination point, the implementation MUST generate a Verification Fact $F(Inv, L_{exit})$ where $L_{exit}$ is the program location immediately after the loop.

This enables the invariant to be used as a postcondition of the loop construct:

```cursive
var i = 0;
loop i < 10
    where { i >= 0 && i <= 10 }
{
    i += 1;
}
// Fact available: i >= 0 && i <= 10
// Combined with loop exit condition !(i < 10): i == 10
```

##### Constraints & Legality

| Code         | Severity | Condition                                         | Detection | Effect    |
| :----------- | :------- | :------------------------------------------------ | :-------- | :-------- |
| `E-CON-2830` | Error    | Loop invariant not established at initialization. | See §10.4 | See §10.4 |
| `E-CON-2831` | Error    | Loop invariant not maintained across iteration.   | See §10.4 | See §10.4 |

---

### 10.4 Verification Modes

##### Definition

A **Verification Mode** specifies the strategy for discharging contract obligations. The mode determines whether contracts are verified statically, dynamically, or assumed without verification.

##### Syntax & Declaration

**Grammar**

```ebnf
verify_attribute
    ::= "[[" "verify" "(" verification_mode ")" "]]"

verification_mode
    ::= "static"
      | "dynamic"
      | "trusted"
```

**Attribute Placement**

The `[[verify(...)]]` attribute MAY be placed on:
- Procedure declarations (applies to that procedure's contracts)
- Module declarations (applies to all contracts in the module)
- Crate root (applies to all contracts in the crate)

More specific placements override less specific ones.

##### Static Semantics

**Mode Enumeration**

| Mode      | Static Proof | Runtime Check | On Failure | Context Restriction |
| :-------- | :----------- | :------------ | :--------- | :------------------ |
| `static`  | Required     | None          | Ill-formed | Any                 |
| `dynamic` | Elision only | Required      | Panic      | Any                 |
| `trusted` | None         | None          | UVB        | `unsafe` only       |
| (default) | Elision only | Required      | Panic      | Any                 |

---

#### 10.4.1 Static Verification Mode

##### Definition

In `static` mode, the implementation MUST mathematically prove that the contract holds using static analysis techniques.

##### Static Semantics

**Proof Obligation**

For a contract predicate $P$ in `static` mode:

$$
\frac{
    \text{mode}(P) = \texttt{static} \quad
    \text{StaticProof}(\Gamma, P)
}{
    P : \text{verified}
}
\tag{Static-OK}
$$

$$
\frac{
    \text{mode}(P) = \texttt{static} \quad
    \neg\text{StaticProof}(\Gamma, P)
}{
    \text{program is ill-formed}
}
\tag{Static-Fail}
$$

**Proof Requirements**

- If proof succeeds: no runtime code is generated for the contract
- If proof fails or is inconclusive: the program is **ill-formed** with diagnostic `E-CON-2801`

**Use Case**

Static mode is appropriate when:
- Runtime overhead is unacceptable
- The predicate is decidable with available static analysis
- Formal verification is required for certification

```cursive
[[verify(static)]]
procedure array_access(arr: const [i32; 10], idx: usize) -> i32
    |= idx < 10
{
    arr[idx]
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CON-2801` | Error    | Static verification failed: unable to prove contract. | Compile-time | Rejection |

---

#### 10.4.2 Dynamic Verification Mode

##### Definition

In `dynamic` mode, the implementation MUST generate executable code to evaluate contract predicates at runtime.

##### Static Semantics

**Static Elision**

The implementation MAY elide a runtime check for predicate $P$ if and only if the implementation can statically prove that $P$ always holds at the check site:

$$
\frac{
    \text{mode}(P) = \texttt{dynamic} \quad
    \text{StaticProof}(\Gamma, P)
}{
    \text{check elided (zero overhead)}
}
\tag{Dynamic-Elide}
$$

When static proof is not possible, a runtime check MUST be generated.

##### Dynamic Semantics

**Check Insertion Points**

| Contract Type  | Check Location                                            |
| :------------- | :-------------------------------------------------------- |
| Precondition   | Procedure prologue (after parameter binding, before body) |
| Postcondition  | Procedure epilogue (after body, before return)            |
| Type Invariant | At each enforcement point (§10.3.1)                       |
| Loop Invariant | At each enforcement point (§10.3.2)                       |

**Runtime Check Failure**

When a dynamically checked predicate evaluates to `false`:

1. The runtime MUST trigger a **Panic** in the current thread
2. The panic payload MUST include:
   - The predicate text (if available)
   - Source location of the contract
   - Whether it was a precondition or postcondition failure
3. Normal panic propagation rules apply (Clause 3)

**Explicit Dynamic Mode**

`[[verify(dynamic)]]` is equivalent to default behavior. Its purpose is documentation:

```cursive
[[verify(dynamic)]]  // Explicit: "this contract is runtime-checked"
procedure user_input(s: string) -> i32
    |= s.len() > 0 => @result >= 0
```

---

#### 10.4.3 Trusted Verification Mode

##### Definition

In `trusted` mode, the implementation assumes the contract holds without generating any verification code. Violation of a trusted contract produces **Unverifiable Behavior (UVB)**.

##### Static Semantics

**Context Restriction**

`[[verify(trusted)]]` MUST appear within one of the following contexts:
- Inside an `unsafe` block
- On an `unsafe` procedure declaration

Use of `[[verify(trusted)]]` outside an `unsafe` context MUST be diagnosed as error `E-CON-2807`.

**UVB Classification**

Violation of a trusted contract is classified as Unverifiable Behavior (§1.2). The safety guarantees of this specification do not apply to programs that violate trusted contracts.

##### Dynamic Semantics

**No Code Generation**

For a predicate $P$ in `trusted` mode:
- No static proofs are attempted
- No runtime checks are generated
- The optimizer MAY assume $P$ holds (equivalent to `llvm.assume`)

**Responsibility Transfer**

By using `[[verify(trusted)]]`, the programmer asserts:
1. The contract will be satisfied by external means
2. Responsibility for correctness is transferred from the language to the programmer
3. Standard safety guarantees do not apply to violation

**Use Case**

Trusted mode is appropriate for:
- FFI boundaries where predicates reference external state
- Performance-critical code where checks are prohibitive and correctness is assured by external analysis
- Interfacing with formally verified components

```cursive
// FFI: external C function guarantees non-null return
unsafe {
    [[verify(trusted)]]
    let ptr = external_alloc(size);
    // Contract: ptr != null is trusted, not checked
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                            | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :----------- | :-------- |
| `E-CON-2807` | Error    | `[[verify(trusted)]]` used outside `unsafe` context. | Compile-time | Rejection |

---

#### 10.4.4 Default Verification Strategy

##### Definition

When no `[[verify(...)]]` attribute is present, the implementation MUST apply the **default verification strategy**: dynamic verification with static elision.

##### Static Semantics

**Default Strategy Algorithm**

For each contract predicate $P$ without an explicit verification mode:

1. **Generate Runtime Check**: The implementation initially plans to emit a runtime check for $P$

2. **Attempt Static Proof**: The implementation analyzes whether $P$ can be statically proven at the check site

3. **Elision Decision**:
   - If static proof succeeds: the runtime check is elided (zero overhead)
   - If static proof fails or is inconclusive: the runtime check is retained

4. **Runtime Failure**: If the check executes and $P$ evaluates to `false`, trigger Panic

**Equivalence**

The default strategy is semantically equivalent to `[[verify(dynamic)]]`. The attribute is unnecessary for default behavior but serves as explicit documentation.

##### Rationale

**Why Dynamic Default?**

The default prioritizes safety:
- Contracts are always enforced unless explicitly trusted
- Developers can reason about program behavior knowing contracts hold
- Static elision provides optimization without semantic change
- No "hidden" paths where contracts are silently ignored

**Why Require `unsafe` for `trusted`?**

Trusted mode produces UVB on violation—the same category as raw pointer dereference and FFI. Requiring `unsafe`:
- Ensures code review catches all potential UVB sources
- Maintains the `// SAFETY:` documentation convention
- Prevents accidental use of trusted mode in safe code

---

### 10.5 Verification Facts

##### Definition

A **Verification Fact** is a compiler-internal guarantee that a predicate $P$ holds at program location $L$. Verification Facts are used for static analysis, contract elision, and type narrowing.

**Formal Definition**

A Verification Fact is a triple $F(P, L, S)$ where:
- $P$ is a predicate expression
- $L$ is a program location (CFG node)
- $S$ is the source of the fact (control flow, runtime check, or assumption)

##### Static Semantics

**Zero-Size Property**

Verification Facts:
- Have **zero runtime size**
- Have **no runtime representation**
- MUST NOT be stored in variables
- MUST NOT be passed as parameters
- MUST NOT be returned from procedures
- Exist solely for static analysis

**Fact Dominance**

A Verification Fact $F(P, L)$ satisfies a requirement for predicate $P$ at statement $S$ if and only if $L$ **strictly dominates** $S$ in the Control Flow Graph (CFG):

$$
\frac{
    F(P, L) \in \text{Facts} \quad
    L \text{ dom } S \quad
    L \neq S
}{
    P \text{ satisfied at } S
}
\tag{Fact-Dominate}
$$

**Fact Generation Rules**

The following constructs generate Verification Facts:

| Construct                    | Generated Fact                  | Location                 |
| :--------------------------- | :------------------------------ | :----------------------- |
| `if P { ... }`               | $F(P, \_)$                      | Entry of then-branch     |
| `if !P { } else { ... }`     | $F(P, \_)$                      | Entry of else-branch     |
| `match x { Pat => ... }`     | $F(x \text{ matches } Pat, \_)$ | Entry of match arm       |
| Runtime check for $P$        | $F(P, \_)$                      | Immediately after check  |
| `[[verify(trusted)]]` on $P$ | $F(P, \_)$                      | Point of trust assertion |
| Loop invariant $Inv$ at exit | $F(Inv, \_)$                    | Immediately after loop   |

**Type Narrowing**

When a Verification Fact $F(P, L)$ is active for binding $x$ at location $L$, the type of $x$ is effectively refined:

$$\text{typeof}(x) \xrightarrow{F(P, L)} \text{typeof}(x) \texttt{ where } \{P\}$$

This enables flow-sensitive typing:

```cursive
procedure example(x: i32 | null) {
    if x != null {
        // Fact: x != null
        // Type of x narrowed to: i32
        let y: i32 = x;  // Valid: x is known non-null
    }
}
```

##### Dynamic Semantics

**Dynamic Fact Injection**

When verification mode is `dynamic` and no static fact dominates the check site:

1. Identify requirement $P$ at program point $S$
2. Construct check block $C$:
   ```cursive
   if !P { panic("Contract violation: {P}"); }
   ```
3. Insert $C$ into CFG such that $C$ dominates $S$
4. Successful execution of $C$ establishes $F(P, \text{exit}(C))$

**Optimizer Integration**

Implementations SHOULD use Verification Facts as optimization hints:

| Optimization             | Condition                                                            |
| :----------------------- | :------------------------------------------------------------------- |
| Assumption injection     | Treat $P$ as invariant truth for instructions dominated by $F(P, L)$ |
| Dead code elimination    | Eliminate branches dependent on $\neg P$ dominated by $F(P, L)$      |
| Bounds check elimination | Elide array bounds checks when index constraints are established     |

> **Implementation Note:** Fact-based assumptions correspond to intrinsics like `llvm.assume` in backend IRs.

---

### 10.6 Behavioral Subtyping (Liskov Substitution)

##### Definition

When a type implements a form (Clause 9), procedure implementations MUST adhere to the **Behavioral Subtyping Principle** (Liskov Substitution Principle) with respect to form-defined contracts.

**Formal Statement**

For form $T$ with procedure $f$ and implementing type $S$ with implementation $f'$:

$$S <: T \implies \forall x.\ (\text{Pre}_T(x) \implies \text{Pre}_S(x)) \land (\text{Post}_S(x) \implies \text{Post}_T(x))$$

##### Static Semantics

**Precondition Weakening**

An implementation MAY weaken (require less than) the preconditions defined in the form. An implementation MUST NOT strengthen (require more than) the preconditions.

$$P_{form} \implies P_{impl}$$

*Intuition:* If the form says "caller must provide X", the implementation may accept X or something weaker (easier to satisfy).

**Formal Rule**

$$
\frac{
    \text{form } T \text{ defines } f \text{ with precondition } P_T \\
    \text{type } S \text{ implements } f \text{ with precondition } P_S \\
    \nvdash P_T \implies P_S
}{
    \text{program is ill-formed}
}
\tag{Liskov-Pre}
$$

**Postcondition Strengthening**

An implementation MAY strengthen (guarantee more than) the postconditions defined in the form. An implementation MUST NOT weaken (guarantee less than) the postconditions.

$$Q_{impl} \implies Q_{form}$$

*Intuition:* If the form promises "callee will provide Y", the implementation must deliver Y or something stronger (more informative).

**Formal Rule**

$$
\frac{
    \text{form } T \text{ defines } f \text{ with postcondition } Q_T \\
    \text{type } S \text{ implements } f \text{ with postcondition } Q_S \\
    \nvdash Q_S \implies Q_T
}{
    \text{program is ill-formed}
}
\tag{Liskov-Post}
$$

**Example**

```cursive
form Container {
    procedure get(~, idx: usize) -> i32
        |= idx < self.len() => @result != 0;
}

// Valid: Postcondition strengthened (guarantees more)
record MyList <: Container {
    data: [i32; 100],
    length: usize,

    procedure get(~, idx: usize) -> i32
        |= idx < self.len() => @result > 0  // > 0 implies != 0
    {
        self.data[idx]
    }
}

// Invalid: Postcondition weakened
record BadList <: Container {
    data: [i32; 100],
    length: usize,

    procedure get(~, idx: usize) -> i32
        |= idx < self.len()  // No guarantee about @result
    {
        self.data[idx]
    }  // Error: E-CON-2804
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CON-2803` | Error    | Implementation strengthens form precondition. | Compile-time | Rejection |
| `E-CON-2804` | Error    | Implementation weakens form postcondition.    | Compile-time | Rejection |

---

### Clause 10 Cross-Reference Notes `[INFORMATIVE]`

**Terms defined in Clause 10 that MUST NOT be redefined elsewhere:**

| Term                        | Section | Description                                               |
| :-------------------------- | :------ | :-------------------------------------------------------- |
| Contract                    | §10.1   | Specification attached to procedure declaration           |
| Predicate                   | §10.1   | Pure boolean expression in contract context               |
| Pure Expression             | §10.1.1 | Expression with no observable side effects                |
| Evaluation Context          | §10.1.2 | Set of bindings available in predicate expressions        |
| Inline Parameter Constraint | §10.1.3 | `where` clause on parameter declaration (type refinement) |
| Contract Clause             | §10.1   | `\|=` clause on procedure declaration                     |
| Precondition                | §10.2.1 | Caller obligation (left of `=>`)                          |
| Postcondition               | §10.2.2 | Callee guarantee (right of `=>`)                          |
| `@result`                   | §10.2.2 | Intrinsic for return value in postconditions              |
| `@entry`                    | §10.2.2 | Operator for entry-state capture in postconditions        |
| Type Invariant              | §10.3.1 | Predicate constraining all instances of a type            |
| Enforcement Point           | §10.3.1 | Program point where type invariant is verified            |
| Loop Invariant              | §10.3.2 | Predicate maintained across loop iterations               |
| Verification Mode           | §10.4   | Strategy for discharging contract obligations             |
| Verification Fact           | §10.5   | Compiler-internal predicate satisfaction guarantee        |
| Fact Dominance              | §10.5   | CFG-based validity of verification facts                  |
| Behavioral Subtyping        | §10.6   | Contract inheritance rules (Liskov Substitution)          |

**Terms referenced from other clauses:**

| Term                  | Source     | Usage in Clause 10                                    |
| :-------------------- | :--------- | :---------------------------------------------------- |
| Unverifiable Behavior | §1.2       | `trusted` mode violations produce UVB                 |
| Unsafe Block          | §3.10      | Required context for `[[verify(trusted)]]`            |
| Panic                 | §3.6       | Runtime contract failure triggers panic               |
| `Copy` / `Clone`      | §9.7       | Required forms for `@entry` capture types             |
| Form                  | §9.1       | Behavioral subtyping applies to form implementations  |
| Permission System     | §4.5       | Contracts must respect permission constraints         |
| Refinement Type       | §4.19      | Type narrowing produces refinement types              |
| Control Flow Graph    | (Implicit) | Fact dominance defined via CFG structure              |
| Capability            | §13        | Distinguished from contracts (authority vs. validity) |

**Terms deferred to other clauses:**

| Term             | Deferred To | Reason                                                 |
| :--------------- | :---------- | :----------------------------------------------------- |
| `parallel` block | §12.2       | Defined in the Concurrency Model                       |
| Capability       | §13         | Distinguished from contracts; controls effects         |
| Refinement Type  | §4.19       | Subtype form; verification uses §10.4/§10.5 mechanisms |

---

## Clause 11: Expressions & Statements

This clause defines the syntax and semantics of expressions and statements in Cursive. Expressions are syntactic forms that produce typed values; statements are syntactic forms executed for their side effects. This clause establishes evaluation rules, pattern matching, operators, control flow, and the binding of values to identifiers.

> **Diagnostic Code Allocation:** This clause uses diagnostic codes with prefixes `E-EXP-`, `E-STM-`, `E-PAT-`, and `P-EXP-` within the range 2500–2799. See the Appendix K Update section at the end of this clause for detailed allocations.

---

### 11.1 Foundational Definitions

##### Definition

**Expression**

An **expression** is a syntactic form that, when evaluated, produces a typed value. Every expression has:

1. **Type**: A compile-time type $T$ determined by static analysis.
2. **Value Category**: Classification as either a *place expression* or *value expression*.
3. **Evaluation Result**: A runtime value conforming to the expression's type.

**Statement**

A **statement** is a syntactic form that is executed for its side effects and does not produce a value. Statements form the executable body of procedures, loops, and other control-flow blocks.

**Statement Categories**

Statements are classified into the following categories:

1. **Declaration Statements**: Introduce new bindings (`let`, `var`). See §11.8.
2. **Assignment Statements**: Modify the value stored in a place. See §11.11.
3. **Expression Statements**: Execute an expression for its side effects. See §11.9.
4. **Control Flow Statements**: Alter the flow of execution (`return`, `result`, `break`, `continue`). See §11.10.
5. **Defer Statements**: Schedule cleanup code for scope exit. See §11.12.

**Typing Context**

A **typing context** $\Gamma$ is a finite ordered sequence of bindings:

$$\Gamma ::= \emptyset \mid \Gamma, x : T$$

where $x$ is an identifier and $T$ is a type. The judgment $\Gamma \vdash e : T$ asserts that expression $e$ has type $T$ under context $\Gamma$.

**Value Categories**

Every expression is classified into exactly one **value category**:

| Category         | Notation    | Definition                                               |
| :--------------- | :---------- | :------------------------------------------------------- |
| Place Expression | $e^{place}$ | Denotes a memory location with a stable address          |
| Value Expression | $e^{value}$ | Produces a temporary value without a persistent location |

**Formal Classification Rules**

The value category of an expression is determined by its syntactic form:

| Expression Form                       | Value Category |
| :------------------------------------ | :------------- |
| Identifier bound by `let` or `var`    | Place          |
| Dereferenced pointer (`*p`)           | Place          |
| Field access on place (`place.field`) | Place          |
| Tuple access on place (`place.0`)     | Place          |
| Indexed access on place (`place[i]`)  | Place          |
| Literals                              | Value          |
| Arithmetic and logical results        | Value          |
| Procedure and closure call results    | Value          |
| Constructor expressions               | Value          |
| Block expression results              | Value          |

##### Static Semantics

**Evaluation Order**

Cursive's evaluation order MUST be deterministic. For any compound expression, subexpressions MUST be evaluated strictly left-to-right, with the following exception:

- Short-circuit operators (`&&`, `||`) conditionally skip evaluation of their right operand as specified in §11.4.

**Sequential Statement Execution**

Statements within a block MUST be executed sequentially in source order. The effects of a statement MUST be fully complete before the next statement begins execution.

**Statement Termination**

Statement termination is governed by Clause 2, §2.11. A statement is terminated by:

1. A `<newline>` token, unless a continuation condition defined in §2.11 is met, OR
2. An explicit semicolon (`;`).

Multiple statements MAY appear on a single line when separated by semicolons.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2501` | Error    | Type mismatch: expected type differs from actual type.    | Compile-time | Rejection |
| `E-EXP-2502` | Error    | Value expression used where place expression is required. | Compile-time | Rejection |

---

### 11.2 Pattern Matching

##### Definition

**Pattern**

A **pattern** is a syntactic form that:

1. Tests whether a value has a particular shape (for refutable patterns), and
2. Binds parts of the value to identifiers.

**Irrefutable Pattern**

An **irrefutable pattern** is a pattern that matches any value of its expected type. Irrefutable patterns include:

- Identifier patterns (`x`)
- Wildcard patterns (`_`)
- Tuple patterns composed entirely of irrefutable sub-patterns
- Record patterns composed entirely of irrefutable sub-patterns
- Single-variant enum patterns

**Refutable Pattern**

A **refutable pattern** is a pattern that can fail to match a value of its expected type. Refutable patterns include:

- Literal patterns (`42`, `"hello"`, `true`)
- Multi-variant enum patterns
- Modal state patterns (`@State`)
- Range patterns (`0..10`)

**Pattern Binding Judgment**

The judgment $\Gamma \vdash p : T \Rightarrow \Gamma'$ asserts that pattern $p$ matches type $T$ and extends context $\Gamma$ with bindings to produce $\Gamma'$.

##### Syntax & Declaration

**Grammar**

```ebnf
pattern ::=
    literal_pattern
  | wildcard_pattern
  | identifier_pattern
  | tuple_pattern
  | record_pattern
  | enum_pattern
  | modal_pattern
  | range_pattern

literal_pattern     ::= integer_literal | float_literal | string_literal 
                      | char_literal | "true" | "false"

wildcard_pattern    ::= "_"

identifier_pattern  ::= identifier

tuple_pattern       ::= "(" [ pattern ( "," pattern )* ","? ] ")"

record_pattern      ::= type_path "{" [ field_pattern ( "," field_pattern )* ","? ] "}"

field_pattern       ::= identifier [ ":" pattern ]

enum_pattern        ::= type_path "::" identifier [ enum_payload_pattern ]

enum_payload_pattern ::= "(" [ pattern ( "," pattern )* ","? ] ")"
                       | "{" [ field_pattern ( "," field_pattern )* ","? ] "}"

modal_pattern       ::= "@" identifier [ "{" [ field_pattern ( "," field_pattern )* ","? ] "}" ]

range_pattern       ::= pattern ".." pattern
```

**Pattern Summary**

| Pattern Type | Example                  | Refutability  | Description                      |
| :----------- | :----------------------- | :------------ | :------------------------------- |
| Literal      | `100`, `"hello"`, `true` | Refutable     | Matches if value equals literal  |
| Wildcard     | `_`                      | Irrefutable   | Matches any value; binds nothing |
| Identifier   | `x`                      | Irrefutable   | Matches any value; binds to `x`  |
| Tuple        | `(a, b, c)`              | Conditional*  | Destructures a tuple             |
| Record       | `Point { x: px, y: py }` | Conditional*  | Destructures a record            |
| Enum         | `Option::Some(v)`        | Conditional** | Matches a specific enum variant  |
| Modal State  | `@Valid { ptr }`         | Refutable     | Matches a specific modal state   |
| Range        | `0..10`                  | Refutable     | Matches if value is within range |

\* Irrefutable only if all sub-patterns are irrefutable.

\** Irrefutable if and only if the enum has exactly one variant.

##### Static Semantics

**Matching Rules**

1. **Literal Pattern**: Matches if `scrutinee == literal`. The literal type MUST be compatible with the scrutinee type.

2. **Wildcard Pattern (`_`)**: Matches any value. No binding is introduced. The matched value is not consumed.

3. **Identifier Pattern (`x`)**: Matches any value. The value is bound to `x` using move semantics: responsibility for the value transfers to the new binding unless the scrutinee is accessed through a `const` permission, in which case the binding receives a `const` view.

4. **Tuple Pattern (`(p₁, p₂, ..., pₙ)`)**: Matches if the scrutinee is a tuple of arity $n$ and each component matches the corresponding sub-pattern. Sub-patterns are matched left-to-right.

5. **Record Pattern (`Type { f₁: p₁, ... }`)**: Matches if the scrutinee is of the named record type and each named field matches the corresponding sub-pattern. Field shorthand (`{ x }`) is equivalent to (`{ x: x }`). Fields not mentioned are ignored.

6. **Enum Pattern (`Enum::Variant(p...)`)**: Matches if the scrutinee's discriminant equals the specified variant. Payload fields are recursively matched against sub-patterns.

7. **Modal Pattern (`@State { ... }`)**: Matches if the scrutinee's active modal state is `@State`. State payload fields are recursively matched.

8. **Range Pattern (`start..end`)**: Matches if $\text{start} \le \text{scrutinee} < \text{end}$. Both `start` and `end` MUST be compile-time constant expressions of the same ordered type as the scrutinee.

**Typing Rules**

**(T-Pat-Wildcard)**
$$\frac{}{\Gamma \vdash \_ : T \Rightarrow \Gamma}$$

**(T-Pat-Ident)**
$$\frac{x \notin \text{dom}(\Gamma)}{\Gamma \vdash x : T \Rightarrow \Gamma, x : T}$$

**(T-Pat-Tuple)**
$$\frac{\Gamma_0 = \Gamma \quad \forall i \in 1..n,\ \Gamma_{i-1} \vdash p_i : T_i \Rightarrow \Gamma_i}{\Gamma \vdash (p_1, \ldots, p_n) : (T_1, \ldots, T_n) \Rightarrow \Gamma_n}$$

**(T-Pat-Record)**
$$\frac{R = \texttt{record} \{ f_1 : T_1, \ldots, f_k : T_k \} \quad \Gamma_0 = \Gamma \quad \forall j \in J,\ \Gamma_{j-1} \vdash p_j : T_{f_j} \Rightarrow \Gamma_j}{\Gamma \vdash R \{ f_{j_1} : p_1, \ldots \} : R \Rightarrow \Gamma_{|J|}}$$

where $J \subseteq \{1, \ldots, k\}$ is the set of matched field indices.

**(T-Pat-Enum)**
$$\frac{E = \texttt{enum} \{ \ldots, V(T_1, \ldots, T_m), \ldots \} \quad \Gamma_0 = \Gamma \quad \forall i \in 1..m,\ \Gamma_{i-1} \vdash p_i : T_i \Rightarrow \Gamma_i}{\Gamma \vdash E::V(p_1, \ldots, p_m) : E \Rightarrow \Gamma_m}$$

**Pattern Binding Scopes**

- **Let/Var Statement**: Bindings are introduced into the enclosing scope. The pattern MUST be irrefutable.

- **Match Arm**: Bindings are introduced into an arm-local scope encompassing only the arm body. Shadowing of outer bindings is implicitly permitted within match arms.

**Exhaustiveness**

A `match` expression MUST be exhaustive. The set of patterns in its arms, taken together, MUST cover every possible value of the scrutinee type.

Exhaustiveness checking is MANDATORY for:

- `enum` types: All variants MUST be covered.
- `modal` types: All states MUST be covered.
- `bool` type: Both `true` and `false` MUST be covered.
- Integer types with range patterns: The union of ranges MUST cover all values, OR a wildcard/identifier pattern MUST be present.

**Exhaustiveness Algorithm**

Implementations MUST use a sound exhaustiveness algorithm. The algorithm:

1. Constructs the set of all possible value shapes for the scrutinee type.
2. For each pattern arm in order, removes the shapes covered by that pattern.
3. If any shapes remain uncovered after processing all arms, the match is non-exhaustive.

**Unreachability**

A match arm MUST NOT be unreachable. An arm is unreachable if its pattern covers only values already covered by preceding arms.

Implementations MUST detect unreachable arms and issue diagnostic `E-PAT-2751`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                            | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------- | :----------- | :-------- |
| `E-PAT-2711` | Error    | Refutable pattern used in irrefutable context (e.g., `let` binding). | Compile-time | Rejection |
| `E-PAT-2712` | Error    | Pattern type incompatible with scrutinee type.                       | Compile-time | Rejection |
| `E-PAT-2713` | Error    | Duplicate binding identifier within a single pattern.                | Compile-time | Rejection |
| `E-PAT-2721` | Error    | Range pattern bounds are not compile-time constants.                 | Compile-time | Rejection |
| `E-PAT-2722` | Error    | Range pattern start exceeds end (empty range).                       | Compile-time | Rejection |
| `E-PAT-2731` | Error    | Record pattern references non-existent field.                        | Compile-time | Rejection |
| `E-PAT-2732` | Error    | Record pattern missing required field (when exhaustive matching).    | Compile-time | Rejection |
| `E-PAT-2741` | Error    | Match expression is not exhaustive for scrutinee type.               | Compile-time | Rejection |
| `E-PAT-2751` | Error    | Match arm is unreachable; pattern covered by preceding arms.         | Compile-time | Rejection |

---

### 11.3 Operator Precedence and Associativity

##### Definition

**Precedence**

**Operator precedence** is an integer ranking that determines how tightly an operator binds to its operands. Operators with higher precedence bind more tightly than operators with lower precedence.

**Associativity**

**Associativity** determines how operators of equal precedence are grouped when parentheses are absent:

- **Left-associative**: Groups left-to-right. `a op b op c` parses as `(a op b) op c`.
- **Right-associative**: Groups right-to-left. `a op b op c` parses as `a op (b op c)`.

##### Syntax & Declaration

**Precedence Table**

The following table lists all Cursive operators from highest precedence (1) to lowest precedence (15). Operators in the same row share the same precedence level.

| Precedence | Operator(s)                                              | Description                             | Associativity |
| :--------- | :------------------------------------------------------- | :-------------------------------------- | :------------ |
| 1          | `()` `[]` `.` `~>` `::`                                  | Call, Index, Field, Method, Scope       | Left          |
| 2          | `=>`                                                     | Pipeline                                | Left          |
| 3          | `!` `-` `&` `*` `^` `move`                               | Unary Not, Neg, Ref, Deref, Alloc, Move | Right         |
| 4          | `as`                                                     | Type Cast                               | Left          |
| 5          | `**`                                                     | Exponentiation                          | Right         |
| 6          | `*` `/` `%`                                              | Multiplicative                          | Left          |
| 7          | `+` `-`                                                  | Additive                                | Left          |
| 8          | `<<` `>>`                                                | Bitwise Shift                           | Left          |
| 9          | `&`                                                      | Bitwise AND                             | Left          |
| 10         | `^`                                                      | Bitwise XOR                             | Left          |
| 11         | `\|`                                                     | Bitwise OR                              | Left          |
| 12         | `==` `!=` `<` `<=` `>` `>=`                              | Comparison                              | Left          |
| 13         | `&&`                                                     | Logical AND                             | Left          |
| 14         | `\|\|`                                                   | Logical OR                              | Left          |
| 15         | `=` `+=` `-=` `*=` `/=` `%=` `&=` `\|=` `^=` `<<=` `>>=` | Assignment                              | Right         |

##### Static Semantics

**Maximal Munch Rule**

The lexer MUST emit the longest valid token at each position. For example:

- `>>` is tokenized as a single right-shift operator, not two `>` tokens.
- `&&` is tokenized as logical AND, not two `&` tokens.
- `::` is tokenized as the scope resolution operator, not two `:` tokens.

**Generic Angle Bracket Exception**

Inside generic argument lists (following `<` in type position), the token sequence `>>` MUST be split into two separate `>` tokens to correctly close nested type parameters:

```cursive
let x: Vec<Option<i32>> = ...  // >> split into > > to close Option and Vec
```

**Operator Ambiguity Resolution**

Certain operators serve multiple roles depending on context:

| Operator | Unary Role (Precedence 3) | Binary Role              |
| :------- | :------------------------ | :----------------------- |
| `-`      | Numeric negation          | Subtraction (Prec. 7)    |
| `&`      | Address-of                | Bitwise AND (Prec. 9)    |
| `*`      | Dereference               | Multiplication (Prec. 6) |
| `^`      | Region allocation         | Bitwise XOR (Prec. 10)   |

Disambiguation is syntactic: if the operator appears after a complete operand expression, it is binary; otherwise, it is unary.

---

### 11.4 Primary Expressions and Operators

##### Definition

**Primary Expressions** are the fundamental operand forms from which all other expressions are built: literals, identifiers, parenthesized expressions, and constructor expressions.

**Postfix Expressions** extend primary expressions with operations that follow the operand: field access, indexing, procedure calls, and method dispatch.

**Unary Expressions** apply a prefix operator to a single operand.

**Binary Expressions** combine two operands with an infix operator.

##### Syntax & Declaration

**Grammar**

```ebnf
expression ::= assignment_expr

assignment_expr ::= logical_or_expr [ assignment_operator assignment_expr ]

logical_or_expr ::= logical_and_expr ( "||" logical_and_expr )*

logical_and_expr ::= comparison_expr ( "&&" comparison_expr )*

comparison_expr ::= bitor_expr ( comparison_operator bitor_expr )*

bitor_expr ::= bitxor_expr ( "|" bitxor_expr )*

bitxor_expr ::= bitand_expr ( "^" bitand_expr )*

bitand_expr ::= shift_expr ( "&" shift_expr )*

shift_expr ::= additive_expr ( shift_operator additive_expr )*

additive_expr ::= multiplicative_expr ( additive_operator multiplicative_expr )*

multiplicative_expr ::= power_expr ( multiplicative_operator power_expr )*

power_expr ::= cast_expr [ "**" power_expr ]

cast_expr ::= unary_expr [ "as" type ]

unary_expr ::= unary_operator unary_expr | postfix_expr

postfix_expr ::= primary_expr postfix_operation*

postfix_operation ::=
    "." identifier
  | "." integer_literal
  | "[" expression "]"
  | "(" [ argument_list ] ")"
  | "~>" identifier "(" [ argument_list ] ")"
  | "=>" expression

primary_expr ::=
    literal
  | identifier
  | "(" expression ")"
  | tuple_expr
  | array_expr
  | block_expr
  | if_expr
  | match_expr
  | loop_expr
  | closure_expr

argument_list ::= argument ( "," argument )* ","?

argument ::= [ "move" ] expression

assignment_operator ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" 
                      | "&=" | "|=" | "^=" | "<<=" | ">>="

comparison_operator ::= "==" | "!=" | "<" | "<=" | ">" | ">="

shift_operator ::= "<<" | ">>"

additive_operator ::= "+" | "-"

multiplicative_operator ::= "*" | "/" | "%"

unary_operator ::= "!" | "-" | "&" | "*" | "^" | "move" | "widen"
```

##### Static Semantics

**Literals**

A **literal** (integer, float, string, character, or boolean) is a primary expression whose type is determined by Clause 2, §2.8 (lexical form) and Clause 5 (type inference and suffixes). Literals are value expressions.

**Identifiers**

An **identifier** in expression position MUST resolve to a value binding according to the name resolution rules in Clause 8. If the identifier resolves to a type, module, or form (rather than a value), the program is ill-formed.

**(T-Ident)** Identifier Typing:
$$\frac{(x : T) \in \Gamma}{\Gamma \vdash x : T}$$

An identifier expression is a place expression if it refers to a `let` or `var` binding, and a value expression otherwise.

**Parenthesized Expressions**

A **parenthesized expression** `(e)` has the same type, value, and value category as the enclosed expression `e`. Parentheses serve only to override precedence; they introduce no semantic effect.

**Constructor Expressions**

Constructor expressions for composite types (tuples, arrays, records, and enum variants) are defined in Clause 5:

| Constructor Form               | Authoritative Section |
| :----------------------------- | :-------------------- |
| Tuple: `(e₁, e₂, ..., eₙ)`     | §5.2                  |
| Array: `[e₁, e₂, ..., eₙ]`     | §5.3                  |
| Record: `Type { f₁: e₁, ... }` | §5.4                  |
| Enum: `Enum::Variant(e₁, ...)` | §5.5                  |

This clause does not redefine constructor expression syntax or semantics; see the authoritative sections.

---

#### 11.4.1 Field and Tuple Access

##### Definition

**Field access** retrieves a named field from a record value. **Tuple access** retrieves an element from a tuple by positional index.

##### Syntax & Declaration

```ebnf
field_access ::= postfix_expr "." identifier
tuple_access ::= postfix_expr "." integer_literal
```

##### Static Semantics

**(T-Field)** Field Access Typing:
$$\frac{\Gamma \vdash e : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \} \quad f \text{ visible}}{\Gamma \vdash e.f : T}$$

The field `f` MUST be:
1. A declared field of record type $R$.
2. Visible in the current scope per the visibility rules of Clause 8.

**(T-Tuple-Index)** Tuple Access Typing:
$$\frac{\Gamma \vdash e : (T_0, T_1, \ldots, T_{n-1}) \quad 0 \le i < n}{\Gamma \vdash e.i : T_i}$$

The index MUST be:
1. A non-negative decimal integer literal (not a variable or computed expression).
2. Within bounds: $0 \le i < n$ where $n$ is the tuple arity.

**Value Category Propagation**

If the base expression is a place expression, the field/tuple access is also a place expression. If the base is a value expression, the access is a value expression.

##### Constraints & Legality

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-EXP-2511` | Error    | Identifier resolves to type or module, not value. | Compile-time | Rejection |
| `E-EXP-2521` | Error    | Field access on non-record type.                  | Compile-time | Rejection |
| `E-EXP-2522` | Error    | Field does not exist in record type.              | Compile-time | Rejection |
| `E-EXP-2523` | Error    | Field is not visible in current scope.            | Compile-time | Rejection |
| `E-EXP-2524` | Error    | Tuple access on non-tuple type.                   | Compile-time | Rejection |
| `E-EXP-2525` | Error    | Tuple index out of bounds.                        | Compile-time | Rejection |
| `E-EXP-2526` | Error    | Tuple index is not a constant integer literal.    | Compile-time | Rejection |

---

#### 11.4.2 Indexing

##### Definition

An **indexing expression** retrieves an element from an array or slice by computed index.

##### Syntax & Declaration

```ebnf
index_expr ::= postfix_expr "[" expression "]"
```

##### Static Semantics

**(T-Index-Array)** Array Indexing:
$$\frac{\Gamma \vdash e_1 : [T; N] \quad \Gamma \vdash e_2 : \texttt{usize}}{\Gamma \vdash e_1[e_2] : T}$$

**(T-Index-Slice)** Slice Indexing:
$$\frac{\Gamma \vdash e_1 : [T] \quad \Gamma \vdash e_2 : \texttt{usize}}{\Gamma \vdash e_1[e_2] : T}$$

The index expression MUST have type `usize`. No implicit integer conversions are performed.

**Value Category**

If the base expression is a place expression, the indexing result is also a place expression (enabling assignment to array elements). If the base is a value expression, the result is a value expression.

##### Dynamic Semantics

**Bounds Checking**

All indexing operations MUST be bounds-checked at runtime. If $\text{index} \ge \text{length}$, the executing thread MUST panic with diagnostic `P-EXP-2530`.

**Optimization Note**

Implementations MAY elide bounds checks when static analysis can prove the index is within bounds (e.g., via contract verification per Clause 10 or loop invariants). Such elision MUST NOT alter observable behavior for well-formed programs.

##### Constraints & Legality

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-EXP-2527` | Error    | Indexing applied to non-indexable type.  | Compile-time | Rejection |
| `E-EXP-2528` | Error    | Index expression is not of type `usize`. | Compile-time | Rejection |
| `P-EXP-2530` | Panic    | Index out of bounds at runtime.          | Runtime      | Panic     |

---

#### 11.4.3 Procedure and Method Calls

##### Definition

A **procedure call** invokes a callable value with a list of arguments. A **method call** invokes a procedure associated with a type, using either instance dispatch (`~>`) or static/qualified dispatch (`::`).

##### Syntax & Declaration

```ebnf
call_expr ::= postfix_expr "(" [ argument_list ] ")"

method_call ::= postfix_expr "~>" identifier "(" [ argument_list ] ")"

static_call ::= type_path "::" identifier "(" [ argument_list ] ")"

argument_list ::= argument ( "," argument )* ","?

argument ::= [ "move" ] expression
```

##### Static Semantics

**(T-Call)** Procedure Call Typing:
$$\frac{\Gamma \vdash f : (m_1\ T_1, \ldots, m_n\ T_n) \to R \quad \forall i \in 1..n,\ \Gamma \vdash a_i : T_i}{\Gamma \vdash f(a_1, \ldots, a_n) : R}$$

**Argument Passing Rules**

1. The number of arguments MUST equal the number of parameters.
2. Each argument type MUST be compatible with the corresponding parameter type per the subtyping rules of Clause 4.
3. If a parameter has the `move` modifier, the corresponding argument MUST be passed via an explicit `move` expression. After the call, the source binding enters the Moved state (§3.5).
4. Arguments are evaluated left-to-right before control transfers to the callee.

**Instance Method Dispatch (`~>`)**

The receiver dispatch operator `~>` invokes a method on a value instance:

```cursive
receiver~>method_name(args)
```

**(T-Method-Instance)** Instance Method Typing:
$$\frac{\Gamma \vdash r : T \quad \text{method } m(self : T', \ldots) \to R \in \text{methods}(T) \quad T <: T'}{\Gamma \vdash r \mathord{\sim>} m(\ldots) : R}$$

**Receiver Dispatch Algorithm**

1. Search for method `m` in the inherent methods of the receiver's type $T$.
2. If not found, search for method `m` in all forms implemented by $T$ that are visible in the current scope.
3. If multiple forms provide method `m`, the call is ambiguous. Disambiguation via qualified syntax `Form::m(receiver, ...)` is required.
4. **Strict Receiver Matching**: The receiver type MUST match the method's `self` parameter type exactly. Auto-dereference and auto-reference are NOT performed.

**Static/Qualified Dispatch (`::`)**

The scope resolution operator `::` invokes a method without an instance receiver, or disambiguates form methods:

```cursive
Type::method(args)           // Static method (no self parameter)
Form::method(receiver, ...) // Disambiguated form method
```

##### Constraints & Legality

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2531` | Error    | Callee expression is not of callable type.            | Compile-time | Rejection |
| `E-EXP-2532` | Error    | Argument count mismatch.                              | Compile-time | Rejection |
| `E-EXP-2533` | Error    | Argument type incompatible with parameter type.       | Compile-time | Rejection |
| `E-EXP-2534` | Error    | `move` argument required but not provided.            | Compile-time | Rejection |
| `E-EXP-2535` | Error    | `move` argument provided but parameter is not `move`. | Compile-time | Rejection |
| `E-EXP-2536` | Error    | Method not found for receiver type.                   | Compile-time | Rejection |
| `E-EXP-2537` | Error    | Method call using `.` instead of `~>`.                | Compile-time | Rejection |
| `E-NAM-1305` | Error    | Ambiguous method resolution; disambiguation required. | Compile-time | Rejection |

---

#### 11.4.4 Pipeline Expressions

##### Definition

A **pipeline expression** provides left-to-right function composition syntax, where the left operand becomes the first argument to the right operand.

##### Syntax & Declaration

```ebnf
pipeline_expr ::= postfix_expr "=>" expression
```

##### Static Semantics

**Desugaring**

The pipeline expression `x => f` desugars to `f(x)` before type checking. The desugaring is:

$$e_1 \Rightarrow e_2 \equiv e_2(e_1)$$

The right-hand expression MUST evaluate to a callable that accepts at least one argument, where the first parameter type is compatible with the left-hand expression type.

**(T-Pipeline)**
$$\frac{\Gamma \vdash e_1 : T_1 \quad \Gamma \vdash e_2 : (T_1, \ldots) \to R}{\Gamma \vdash e_1 \Rightarrow e_2 : R}$$

**Chaining**

Pipelines are left-associative and may be chained:

```cursive
x => f => g => h   // Equivalent to: h(g(f(x)))
```

##### Constraints & Legality

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-EXP-2538` | Error    | Right-hand side of pipeline is not callable. | Compile-time | Rejection |
| `E-EXP-2539` | Error    | Pipeline target has no parameters.           | Compile-time | Rejection |

---

#### 11.4.5 Unary Operators

##### Definition

Unary operators apply a single-operand operation. All unary operators have precedence 3 and are right-associative.

##### Syntax & Declaration

```ebnf
unary_expr ::= unary_operator unary_expr | postfix_expr

unary_operator ::= "!" | "-" | "&" | "*" | "^" | "move" | "widen"
```

##### Static Semantics

**Logical Negation (`!`)**

**(T-Not-Bool)**
$$\frac{\Gamma \vdash e : \texttt{bool}}{\Gamma \vdash !e : \texttt{bool}}$$

**(T-Not-Int)** Bitwise complement on integers:
$$\frac{\Gamma \vdash e : I \quad I \in \mathcal{T}_{\text{int}}}{\Gamma \vdash !e : I}$$

**Numeric Negation (`-`)**

**(T-Neg)**
$$\frac{\Gamma \vdash e : N \quad N \in \mathcal{T}_{\text{signed}} \cup \mathcal{T}_{\text{float}}}{\Gamma \vdash -e : N}$$

Negation is defined for signed integer types and floating-point types. Negation of unsigned integers is ill-formed.

**Address-Of (`&`)**

**(T-Ref)**
$$\frac{\Gamma \vdash e : T \quad e^{place}}{\Gamma \vdash \&e : \texttt{Ptr}\langle T \rangle @\texttt{Valid}}$$

The operand MUST be a place expression. The result is a safe pointer in the `@Valid` modal state. The permission of the resulting pointer is determined by the permission of the source place (§4.5).

**Dereference (`*`)**

**(T-Deref)**
$$\frac{\Gamma \vdash e : \texttt{Ptr}\langle T \rangle @S \quad S \ne \texttt{Null}}{\Gamma \vdash *e : T}$$

The operand MUST be a pointer in a dereferenceable state (not `@Null`). Dereferencing a raw pointer (`*imm T` or `*mut T`) is permitted only within an `unsafe` block (§3.10).

**Region Allocation (`^`)**

**(T-Alloc)**
$$\frac{\Gamma \vdash e : T \quad \text{region}(R) \in \text{scope}}{\Gamma \vdash \texttt{\^{}}e : \texttt{Ptr}\langle T \rangle @\texttt{Valid}}$$

The `^` operator allocates the operand value into the innermost active region. Full semantics are defined in §3.9 (Regions and Arena Allocation).

**Move (`move`)**

**(T-Move)**
$$\frac{\Gamma \vdash x : T \quad \text{state}(\Gamma, x) = \text{Valid} \quad \text{movable}(\Gamma, x)}{\Gamma \vdash \texttt{move } x : T \dashv \Gamma[x \mapsto \text{Moved}]}$$

The `move` operator transfers responsibility for a value from its source binding. Full semantics are defined in §3.5 (Responsibility and Move Semantics).

##### Constraints & Legality

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2541` | Error    | Logical `!` applied to non-bool, non-integer type. | Compile-time | Rejection |
| `E-EXP-2542` | Error    | Numeric negation of unsigned integer.              | Compile-time | Rejection |
| `E-EXP-2543` | Error    | Address-of `&` applied to value expression.        | Compile-time | Rejection |
| `E-EXP-2544` | Error    | Dereference of pointer in `@Null` state.           | Compile-time | Rejection |
| `E-EXP-2545` | Error    | Raw pointer dereference outside `unsafe` block.    | Compile-time | Rejection |
| `E-MEM-3021` | Error    | Region allocation `^` outside region scope.        | Compile-time | Rejection |
| `E-MEM-3001` | Error    | Move from binding in Moved state.                  | Compile-time | Rejection |
| `E-MEM-3006` | Error    | Move from immovable binding (`:=`).                | Compile-time | Rejection |

---

#### 11.4.6 Binary Operators

##### Definition

Binary operators combine two operands with an infix operator. This section covers arithmetic, bitwise, comparison, and logical operators.

##### Static Semantics

**Arithmetic Operators (`+`, `-`, `*`, `/`, `%`, `**`)**

**(T-Arith)**
$$\frac{\Gamma \vdash e_1 : N \quad \Gamma \vdash e_2 : N \quad N \in \mathcal{T}_{\text{numeric}}}{\Gamma \vdash e_1 \oplus e_2 : N}$$

where $\oplus \in \{+, -, *, /, \%, **\}$.

Arithmetic operators require homogeneous operand types. No implicit numeric promotions or conversions are performed. If operand types differ, the program is ill-formed.

**Bitwise Operators (`&`, `|`, `^`, `<<`, `>>`)**

**(T-Bitwise)**
$$\frac{\Gamma \vdash e_1 : I \quad \Gamma \vdash e_2 : I \quad I \in \mathcal{T}_{\text{int}}}{\Gamma \vdash e_1 \oplus e_2 : I}$$

where $\oplus \in \{\&, |, \texttt{\^{}}\}$.

**(T-Shift)**
$$\frac{\Gamma \vdash e_1 : I \quad \Gamma \vdash e_2 : \texttt{u32} \quad I \in \mathcal{T}_{\text{int}}}{\Gamma \vdash e_1 \ll e_2 : I} \quad \frac{\Gamma \vdash e_1 : I \quad \Gamma \vdash e_2 : \texttt{u32} \quad I \in \mathcal{T}_{\text{int}}}{\Gamma \vdash e_1 \gg e_2 : I}$$

Shift operators require the right operand to be `u32`. The result type is the type of the left operand.

**Comparison Operators (`==`, `!=`, `<`, `<=`, `>`, `>=`)**

**(T-Compare)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T}{\Gamma \vdash e_1\ \text{cmp}\ e_2 : \texttt{bool}}$$

Comparison operators require homogeneous operand types. The result type is always `bool`.

Equality operators (`==`, `!=`) are defined for all types implementing the `Eq` form.

Ordering operators (`<`, `<=`, `>`, `>=`) are defined for types implementing the `Ord` form.

**Logical Operators (`&&`, `||`)**

**(T-Logic)**
$$\frac{\Gamma \vdash e_1 : \texttt{bool} \quad \Gamma \vdash e_2 : \texttt{bool}}{\Gamma \vdash e_1 \land\land e_2 : \texttt{bool}} \quad \frac{\Gamma \vdash e_1 : \texttt{bool} \quad \Gamma \vdash e_2 : \texttt{bool}}{\Gamma \vdash e_1 \lor\lor e_2 : \texttt{bool}}$$

##### Dynamic Semantics

**Short-Circuit Evaluation**

Logical operators MUST implement short-circuit evaluation:

- `e₁ && e₂`: If `e₁` evaluates to `false`, `e₂` is NOT evaluated; the result is `false`.
- `e₁ || e₂`: If `e₁` evaluates to `true`, `e₂` is NOT evaluated; the result is `true`.

This is the sole exception to strict left-to-right evaluation: the right operand may be skipped entirely.

**Integer Overflow**

Integer arithmetic operations (`+`, `-`, `*`) MUST panic on overflow in `strict` mode. Behavior in `release` mode is Implementation-Defined (IDB); implementations MUST document whether overflow wraps or panics.

**Division and Remainder**

Division by zero MUST cause the executing thread to panic.

For signed integer division, the result is truncated toward zero. The remainder operation `a % b` satisfies the identity: `a == (a / b) * b + (a % b)`.

**Shift Operations**

If the right operand of a shift operation is greater than or equal to the bit width of the left operand type, the behavior is Implementation-Defined. Implementations MUST document whether this condition panics or produces a defined result.

##### Constraints & Legality

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-EXP-2551` | Error    | Arithmetic operator applied to non-numeric types. | Compile-time | Rejection |
| `E-EXP-2552` | Error    | Mismatched operand types for arithmetic operator. | Compile-time | Rejection |
| `E-EXP-2553` | Error    | Bitwise operator applied to non-integer types.    | Compile-time | Rejection |
| `E-EXP-2554` | Error    | Comparison of incompatible types.                 | Compile-time | Rejection |
| `E-EXP-2555` | Error    | Logical operator applied to non-bool operands.    | Compile-time | Rejection |
| `E-EXP-2556` | Error    | Shift amount is not of type `u32`.                | Compile-time | Rejection |
| `P-EXP-2560` | Panic    | Integer overflow in strict mode.                  | Runtime      | Panic     |
| `P-EXP-2561` | Panic    | Division by zero.                                 | Runtime      | Panic     |

---

#### 11.4.7 Cast Expressions

##### Definition

A **cast expression** explicitly converts a value from one type to another using the `as` operator.

##### Syntax & Declaration

```ebnf
cast_expr ::= unary_expr "as" type
```

The `as` operator has precedence 4 (between unary operators and exponentiation) and is left-associative. Multiple casts may be chained: `x as i32 as f64` parses as `(x as i32) as f64`.

##### Static Semantics

**(T-Cast)**
$$\frac{\Gamma \vdash e : S \quad \text{CastValid}(S, T)}{\Gamma \vdash e\ \texttt{as}\ T : T}$$

A cast is well-formed if and only if the source-target type pair is in one of the categories defined below. All other combinations are ill-formed.

**Cast Categories**

| Category              | Source Type         | Target Type         | Semantics                   |
| :-------------------- | :------------------ | :------------------ | :-------------------------- |
| **Numeric Widening**  | `iN`                | `iM` where `M > N`  | Sign-extended               |
|                       | `uN`                | `uM` where `M > N`  | Zero-extended               |
|                       | `fN`                | `fM` where `M > N`  | Precision-extended          |
| **Numeric Narrowing** | `iN`                | `iM` where `M < N`  | Truncated (low-order bits)  |
|                       | `uN`                | `uM` where `M < N`  | Truncated (low-order bits)  |
|                       | `fN`                | `fM` where `M < N`  | Rounded to nearest          |
| **Sign Conversion**   | `iN`                | `uN`                | Bit reinterpretation        |
|                       | `uN`                | `iN`                | Bit reinterpretation        |
| **Integer ↔ Float**   | `iN` / `uN`         | `fM`                | Nearest representable value |
|                       | `fM`                | `iN` / `uN`         | Truncate toward zero†       |
| **Pointer ↔ Integer** | `usize`             | `*imm T` / `*mut T` | Address interpretation‡     |
|                       | `*imm T` / `*mut T` | `usize`             | Address extraction‡         |
| **Safe → Raw Ptr**    | `Ptr<T>@Valid`      | `*imm T` / `*mut T` | Extract address             |
| **Enum → Integer**    | `enum E`            | Integer type        | Discriminant value§         |
| **Bool → Integer**    | `bool`              | Any integer         | `false`→0, `true`→1         |
| **Char ↔ Integer**    | `char`              | `u32`               | Unicode scalar value        |
|                       | `u32`               | `char`              | Validated conversion††      |
|                       | `char`              | `u8`                | Validated conversion††      |
|                       | `u8`                | `char`              | Always valid (ASCII subset) |

† Float-to-integer casts that overflow or produce NaN trigger a panic.

‡ Pointer-integer casts are permitted only within `unsafe` blocks.

§ Enum-to-integer casts are valid only when the enum has an explicit `[[repr(intN)]]` attribute.

†† Panics if the value is not a valid Unicode scalar value or exceeds the target range.

**Forbidden Casts**

The following conversions are never permitted:

| Source           | Target                | Reason                                 |
| :--------------- | :-------------------- | :------------------------------------- |
| `bool`           | `float`               | Use integer intermediate               |
| `char`           | `bool`                | Semantically meaningless               |
| `string`         | Any numeric           | Use parsing methods                    |
| Any numeric      | `string`              | Use formatting methods                 |
| Record/Enum      | Different Record/Enum | Use explicit conversion functions      |
| `Ptr<T>@Null`    | `*imm T` / `*mut T`   | Only `@Valid` pointers may cast to raw |
| `Ptr<T>@Expired` | `*imm T` / `*mut T`   | Only `@Valid` pointers may cast to raw |

##### Dynamic Semantics

**Widening Casts**

Widening casts are always lossless:
- Signed integers: Sign-extended to fill additional bits.
- Unsigned integers: Zero-extended to fill additional bits.
- Floats: Exactly represented in the larger format.

**Narrowing Casts**

Narrowing casts may lose information:
- Integer narrowing: Low-order bits are preserved; high-order bits are discarded. No panic occurs; information loss is silent.
- Float narrowing: Rounded to nearest representable value per IEEE 754.

**Float-to-Integer Casts**

Float-to-integer casts truncate toward zero. The following conditions trigger a panic:

| Condition                  | Example                |
| :------------------------- | :--------------------- |
| Value exceeds target range | `1e20_f64 as i32`      |
| Value is NaN               | `f64::NAN as i32`      |
| Value is ±Infinity         | `f64::INFINITY as i32` |

**Integer-to-Float Casts**

Large integers that cannot be exactly represented are rounded to the nearest representable value per IEEE 754 "round to nearest, ties to even."

##### Constraints & Legality

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-EXP-2571` | Error    | Cast between incompatible types.                 | Compile-time | Rejection |
| `E-EXP-2572` | Error    | Pointer-integer cast outside `unsafe` block.     | Compile-time | Rejection |
| `E-EXP-2573` | Error    | Enum cast without `[[repr]]` attribute.          | Compile-time | Rejection |
| `E-EXP-2574` | Error    | Cast of non-`@Valid` pointer to raw pointer.     | Compile-time | Rejection |
| `P-EXP-2580` | Panic    | Float-to-integer cast overflow or NaN.           | Runtime      | Panic     |
| `P-EXP-2581` | Panic    | `u32 as char` with invalid Unicode scalar value. | Runtime      | Panic     |
| `P-EXP-2582` | Panic    | `char as u8` with value > 255.                   | Runtime      | Panic     |

---

### 11.5 Closure Expressions

##### Definition

A **closure expression** creates an anonymous callable value. Closures may capture bindings from their enclosing lexical scope.

**Capturing Closure**

A closure that references bindings from its enclosing scope is a **capturing closure**. The captured bindings form the closure's **environment**.

**Non-Capturing Closure**

A closure that references no bindings from its enclosing scope is a **non-capturing closure**. Non-capturing closures have sparse function pointer types and are FFI-compatible.

##### Syntax & Declaration

**Grammar**

```ebnf
closure_expr ::= "|" [ closure_param_list ] "|" [ "->" type ] closure_body

closure_param_list ::= closure_param ( "," closure_param )* ","?

closure_param ::= [ "move" ] identifier [ ":" type ]

closure_body ::= expression | block_expr
```

**Syntactic Notes**

1. The `|` delimiters distinguish closure expressions from other uses of `|` (bitwise OR). Context disambiguates: `|` at expression start, followed by an optional parameter list and another `|`, forms a closure.

2. Parameter types MAY be omitted when the closure is checked against a known function type (see Bidirectional Type Checking).

3. The return type MAY be omitted; it is inferred from the closure body.

4. The closure body is either a single expression (for simple closures) or a block expression (for multi-statement closures).

**Examples**

```cursive
// No parameters, expression body
|| 42

// Single parameter, expression body
|x| x + 1

// Multiple parameters with types, block body
|a: i32, b: i32| -> i32 {
    let sum = a + b
    result sum * 2
}

// Move parameter
|move buffer| process(buffer)
```

##### Static Semantics

**Closure Typing**

A closure expression has a function type determined by its parameters, return type, and captures:

**(T-Closure-Sparse)** Non-Capturing Closure:
$$\frac{\Gamma, p_1 : T_1, \ldots, p_n : T_n \vdash e : R \quad \text{captures} = \emptyset}{\Gamma \vdash |p_1, \ldots, p_n| \to e : (T_1, \ldots, T_n) \to R}$$

**(T-Closure-Capturing)** Capturing Closure:
$$\frac{\Gamma, p_1 : T_1, \ldots, p_n : T_n \vdash e : R \quad \text{captures} \neq \emptyset}{\Gamma \vdash |p_1, \ldots, p_n| \to e : |T_1, \ldots, T_n| \to R}$$

Non-capturing closures have sparse function pointer types `(T) -> R`. Capturing closures have closure types `|T| -> R`. See §6.3 for the full specification of function types.

**Parameter Typing**

1. If a parameter has an explicit type annotation, that type is used.
2. If a parameter lacks a type annotation and the closure is checked against an expected function type, the parameter type is inferred from the expected type.
3. If a parameter lacks a type annotation and no expected type is available, the program is ill-formed.

**Return Type Inference**

1. If the return type is explicitly annotated, the body is checked against that type.
2. If the return type is omitted, it is inferred from the body expression type.

**Capture Semantics**

Closures capture bindings from their enclosing scope according to the following rules:

| Binding Permission | Capture Mode  | Closure Environment Contains        |
| :----------------- | :------------ | :---------------------------------- |
| `const`            | By reference  | `const` reference to binding        |
| `unique`           | Move required | Owned value (binding becomes Moved) |
| `shared`           | By reference  | `shared` reference to binding       |

**Move Captures**

A binding with `unique` permission MUST be captured via an explicit `move` in the closure parameter list or by using `move` when referencing the binding in the closure body:

```cursive
let buffer: unique Buffer = ...
let closure = |x| process(move buffer, x)  // 'buffer' moved into closure
// 'buffer' is now in Moved state
```

**Capture Analysis**

The compiler MUST perform capture analysis to determine which bindings are referenced by the closure body. A binding is captured if:

1. It is referenced by name within the closure body, AND
2. It is not a parameter of the closure, AND
3. It is not shadowed by a local binding within the closure body.

**Closure Subtyping**

Per §6.3, sparse function pointers are subtypes of the corresponding closure types:

$$(T_1, \ldots, T_n) \to R <: |T_1, \ldots, T_n| \to R$$

A non-capturing closure may be used where a capturing closure is expected.

##### Dynamic Semantics

**Closure Construction**

When a closure expression is evaluated:

1. All captured bindings are evaluated and stored in the closure's environment.
2. For `const` and `shared` captures, references to the original bindings are stored.
3. For `move` captures, the values are moved into the closure environment; the source bindings become invalid.
4. A closure value is constructed containing the code pointer and environment pointer.

**Closure Invocation**

When a closure is invoked:

1. Arguments are bound to parameters.
2. The closure body is evaluated with access to both parameters and captured environment.
3. The result is returned to the caller.

**Environment Lifetime**

The closure's environment has a lifetime tied to the closure value. When the closure value is dropped, captured owned values are destroyed.

##### Constraints & Legality

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-EXP-2591` | Error    | Closure parameter type cannot be inferred.  | Compile-time | Rejection |
| `E-EXP-2593` | Error    | Capture of `unique` binding without `move`. | Compile-time | Rejection |
| `E-EXP-2594` | Error    | Closure return type mismatch.               | Compile-time | Rejection |

---

### 11.6 Control Flow Expressions

##### Definition

Control flow expressions alter the sequential flow of evaluation and may produce values.

**Conditional Expression (`if`)**: Evaluates one of two branches based on a boolean condition.

**Match Expression**: Performs exhaustive pattern matching on a scrutinee value.

**Loop Expression**: Iterates until a termination condition is met.

##### Syntax & Declaration

**Grammar**

```ebnf
if_expr ::= "if" expression block_expr [ "else" ( block_expr | if_expr ) ]

match_expr ::= "match" expression "{" match_arm+ "}"

match_arm ::= pattern [ "if" expression ] "=>" arm_body ","

arm_body ::= expression | block_expr

loop_expr ::= [ label ] loop_kind

label ::= "'" identifier ":"

loop_kind ::=
    "loop" block_expr
  | "loop" expression block_expr
  | "loop" pattern ":" type "in" expression block_expr
```

##### Static Semantics

**Conditional Expression (`if`)**

**(T-If)**
$$\frac{\Gamma \vdash e_c : \texttt{bool} \quad \Gamma \vdash e_t : T \quad \Gamma \vdash e_f : T}{\Gamma \vdash \texttt{if } e_c\ \{ e_t \}\ \texttt{else}\ \{ e_f \} : T}$$

Both branches MUST have the same type. The result type is the common branch type.

**(T-If-No-Else)**
$$\frac{\Gamma \vdash e_c : \texttt{bool} \quad \Gamma \vdash e_t : ()}{\Gamma \vdash \texttt{if } e_c\ \{ e_t \} : ()}$$

An `if` without `else` has type `()`. The then-branch MUST also have type `()`.

**Match Expression**

**(T-Match)**
$$\frac{\Gamma \vdash e : T_s \quad \forall i,\ \Gamma, \text{bindings}(p_i) \vdash e_i : T \quad \text{exhaustive}(\{p_i\}, T_s) \quad \text{reachable}(p_i)}{\Gamma \vdash \texttt{match } e\ \{ p_1 \Rightarrow e_1, \ldots \} : T}$$

1. All arm bodies MUST have the same type.
2. The pattern set MUST be exhaustive for the scrutinee type (§11.2).
3. No arm MUST be unreachable (§11.2).

**Match Guards**

A match arm MAY include a guard clause `if guard_expr`. The guard:

1. MUST have type `bool`.
2. Is evaluated only if the pattern matches.
3. If the guard evaluates to `false`, matching continues with the next arm.
4. Guard expressions MAY reference bindings introduced by the pattern.

Guards do not affect exhaustiveness checking; patterns with guards are considered to cover their full range for exhaustiveness purposes.

**Loop Expressions**

**(T-Loop-Infinite)**
$$\frac{\Gamma \vdash e : T}{\Gamma \vdash \texttt{loop}\ \{ e \} : !}$$

An infinite loop with no `break` has type `!` (never). If a `break value` is present, the loop has the type of the break values.

**(T-Loop-Conditional)**
$$\frac{\Gamma \vdash e_c : \texttt{bool} \quad \Gamma \vdash e_b : ()}{\Gamma \vdash \texttt{loop } e_c\ \{ e_b \} : ()}$$

A conditional loop (`loop condition { ... }`) has type `()`.

**(T-Loop-Iterator)**
$$\frac{\Gamma \vdash e_{iter} : I \quad I : \texttt{Iterator}\langle\texttt{Item} = T\rangle \quad \Gamma, x : T \vdash e_b : ()}{\Gamma \vdash \texttt{loop } x : T\ \texttt{in } e_{iter}\ \{ e_b \} : ()}$$

An iterator loop has type `()`. The iterator expression MUST implement the `Iterator` form.

**Loop with Break Value**

When a loop contains `break value` statements, all such breaks for the same loop MUST provide values of the same type. That type becomes the loop's result type.

##### Dynamic Semantics

**Conditional Evaluation**

1. The condition expression is evaluated.
2. If true, the then-branch is evaluated and its value is the result.
3. If false and an else-branch exists, the else-branch is evaluated and its value is the result.
4. If false and no else-branch exists, the result is `()`.

**Match Evaluation**

1. The scrutinee expression is evaluated.
2. Patterns are tested in declaration order.
3. For each pattern that matches structurally, the guard (if any) is evaluated.
4. The first arm whose pattern matches and whose guard (if any) evaluates to `true` is selected.
5. Pattern bindings are introduced into scope, and the arm body is evaluated.
6. The arm body's value is the match result.

**Loop Desugaring**

Conditional loops desugar to infinite loops:

```cursive
loop condition { body }
// Equivalent to:
loop {
    if !condition { break }
    body
}
```

Iterator loops desugar to match on `Iterator::next`:

```cursive
loop x: T in iter { body }
// Equivalent to:
{
    var iterator = iter
    loop {
        match iterator~>next() {
            Option::Some(x) => { body },
            Option::None => { break },
        }
    }
}
```

##### Constraints & Legality

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2601` | Error    | `if` condition is not of type `bool`.              | Compile-time | Rejection |
| `E-EXP-2602` | Error    | `if` branches have incompatible types.             | Compile-time | Rejection |
| `E-EXP-2603` | Error    | `if` without `else` used in non-unit context.      | Compile-time | Rejection |
| `E-EXP-2611` | Error    | Match arms have incompatible types.                | Compile-time | Rejection |
| `E-EXP-2612` | Error    | Match guard is not of type `bool`.                 | Compile-time | Rejection |
| `E-EXP-2621` | Error    | Iterator expression does not implement `Iterator`. | Compile-time | Rejection |
| `E-EXP-2622` | Error    | `break` values have incompatible types.            | Compile-time | Rejection |

---

### 11.7 Block Expressions

##### Definition

A **block expression** introduces a new lexical scope and evaluates to a value. Blocks are delimited by braces and contain zero or more statements followed by an optional result expression.

##### Syntax & Declaration

```ebnf
block_expr ::= "{" statement* [ expression ] "}"
```

##### Static Semantics

**Block Type**

The type of a block expression is determined by:

1. The type of an explicit `result` statement, if present, OR
2. The type of the final unterminated expression, if present, OR
3. The unit type `()` if the block is empty or ends with a terminated statement.

**(T-Block-Result)**
$$\frac{\Gamma \vdash s_1; \ldots; s_n \quad \Gamma \vdash e : T}{\Gamma \vdash \{ s_1; \ldots; s_n;\ e \} : T}$$

**(T-Block-Unit)**
$$\frac{\Gamma \vdash s_1; \ldots; s_n}{\Gamma \vdash \{ s_1; \ldots; s_n; \} : ()}$$

**Scope Rules**

1. Bindings introduced within a block are visible only within that block.
2. Bindings shadow outer bindings of the same name for the duration of the block.
3. When a block exits, all bindings introduced within it are destroyed in reverse declaration order.

##### Dynamic Semantics

1. Statements are executed sequentially in source order.
2. If a `result` statement is executed, the block immediately evaluates to that value.
3. If the block ends with an unterminated expression, that expression's value is the block result.
4. If the block ends with a terminated statement, the block result is `()`.
5. Upon block exit (normal or via control flow), destructors are invoked for all bindings in reverse declaration order.

**Specialized Block Forms**

The following block forms have specialized semantics defined in other clauses:

| Block Form              | Authoritative Section             |
| :---------------------- | :-------------------------------- |
| `region { ... }`        | §3.9 Regions and Arena Allocation |
| `unsafe { ... }`        | §3.10 Unsafe Memory Operations    |
| `parallel(...) { ... }` | §13.3 Structured Concurrency      |
| `comptime { ... }`      | §14.1 Compile-Time Execution      |

This clause does not redefine these block forms; see the authoritative sections.

---

### 11.8 Declaration Statements

##### Definition

A **declaration statement** introduces new bindings into the current scope.

##### Syntax & Declaration

```ebnf
decl_stmt ::= let_decl | var_decl

let_decl ::= "let" pattern [ ":" type ] binding_op expression

var_decl ::= "var" pattern [ ":" type ] binding_op expression

binding_op ::= "=" | ":="
```

##### Static Semantics

**Binding Keywords**

| Keyword | Mutability | Description                  |
| :------ | :--------- | :--------------------------- |
| `let`   | Immutable  | Binding cannot be reassigned |
| `var`   | Mutable    | Binding may be reassigned    |

**Binding Operators**

| Operator | Movability | Description                                  |
| :------- | :--------- | :------------------------------------------- |
| `=`      | Movable    | Responsibility may be transferred via `move` |
| `:=`     | Immovable  | Responsibility cannot be transferred         |

**Orthogonality**

Binding mutability (`let` vs. `var`) is orthogonal to:

1. **Permission**: The permission of the bound value (`const`, `unique`, `shared`) is determined by the value's type and how it is accessed.
2. **Movability**: The binding operator (`=` vs. `:=`) determines whether responsibility may be transferred.

**Type Inference**

If the type annotation is omitted, the type is inferred from the initializer expression. If the type cannot be inferred, the program is ill-formed.

**Pattern Requirements**

The pattern in a declaration statement MUST be irrefutable (§11.2). Refutable patterns are diagnosed as errors.

**Responsibility**

Both `=` and `:=` establish cleanup responsibility per §3.4 and §3.5. The binding becomes responsible for invoking the destructor when it goes out of scope.

##### Dynamic Semantics

1. The initializer expression is evaluated.
2. The value is bound to the pattern, introducing bindings into scope.
3. For destructuring patterns, components are bound to their respective identifiers.

##### Constraints & Legality

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-PAT-2711` | Error    | Refutable pattern in `let`/`var`.                | Compile-time | Rejection |
| `E-DEC-2401` | Error    | Reassignment of immutable `let` binding.         | Compile-time | Rejection |
| `E-DEC-2402` | Error    | Type annotation incompatible with inferred type. | Compile-time | Rejection |

---

### 11.9 Expression Statements

##### Definition

An **expression statement** evaluates an expression for its side effects. The resulting value is discarded.

##### Syntax & Declaration

```ebnf
expr_stmt ::= expression terminator

terminator ::= ";" | <newline>
```

##### Static Semantics

The type of an expression statement is the unit type `()`. The expression's value is discarded after evaluation.

##### Dynamic Semantics

1. The expression is evaluated.
2. If the expression produces a value, that value is discarded.
3. If the expression's result is a value with a destructor and the value is not moved elsewhere, the destructor is invoked at statement end.

---

### 11.10 Control Flow Statements

##### Definition

Control flow statements alter the normal sequential execution of statements.

##### Syntax & Declaration

```ebnf
return_stmt   ::= "return" [ expression ]

result_stmt   ::= "result" expression

break_stmt    ::= "break" [ "'" identifier ] [ expression ]

continue_stmt ::= "continue" [ "'" identifier ]
```

##### Static Semantics

**Return Statement**

`return` terminates the current procedure and returns control to the caller.

1. If an expression is provided, its type MUST match the procedure's declared return type.
2. If no expression is provided, the procedure's return type MUST be `()`.
3. `return` MUST NOT appear at module scope.

**Result Statement**

`result` terminates the current block and yields a value as the block's result.

1. The expression type becomes (or must match) the enclosing block's type.
2. `result` exits only the immediately enclosing block, not the procedure.
3. `result` is the primary mechanism for yielding values from blocks.

**Break Statement**

`break` terminates the innermost (or labeled) loop.

1. `break` MUST appear within a `loop` body.
2. If a label is provided, the labeled loop is terminated.
3. If an expression is provided, the value becomes the loop's result.
4. All `break` statements for the same loop MUST provide values of the same type (or all omit values).

**Continue Statement**

`continue` skips to the next iteration of the innermost (or labeled) loop.

1. `continue` MUST appear within a `loop` body.
2. If a label is provided, the labeled loop's next iteration begins.

##### Dynamic Semantics

**Return Execution**

1. The return expression (if any) is evaluated.
2. Deferred actions in all scopes between the `return` and procedure entry are executed in LIFO order.
3. Control transfers to the caller.

**Result Execution**

1. The result expression is evaluated.
2. Deferred actions in the exited block (only) are executed in LIFO order.
3. The block completes with the yielded value.

**Break/Continue Execution**

1. Deferred actions in scopes between the statement and the target loop are executed in LIFO order.
2. For `break`: the loop terminates with the break value (if any).
3. For `continue`: the loop's next iteration begins.

##### Constraints & Legality

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-STM-2661` | Error    | `return` type mismatch with procedure.  | Compile-time | Rejection |
| `E-STM-2665` | Error    | `return` at module scope.               | Compile-time | Rejection |
| `E-STM-2664` | Error    | `result` type mismatch with block.      | Compile-time | Rejection |
| `E-STM-2662` | Error    | `break` outside `loop`.                 | Compile-time | Rejection |
| `E-STM-2663` | Error    | `continue` outside `loop`.              | Compile-time | Rejection |
| `E-STM-2666` | Error    | Unknown loop label.                     | Compile-time | Rejection |
| `E-STM-2667` | Error    | `break` values have incompatible types. | Compile-time | Rejection |

---

### 11.11 Assignment Statements

##### Definition

An **assignment statement** modifies the value stored in a place expression.

##### Syntax & Declaration

```ebnf
assignment_stmt ::= place_expr assignment_operator expression

assignment_operator ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" 
                      | "&=" | "|=" | "^=" | "<<=" | ">>="
```

##### Static Semantics

**Requirements**

1. The left-hand side MUST be a place expression (§11.1).
2. The place MUST refer to a mutable binding (declared with `var`).
3. The place MUST be accessible via a `unique` permission. Assignment through `const` or `shared` permission is forbidden.
4. The right-hand side type MUST be compatible with the place's type.

**Compound Assignment Desugaring**

Compound assignment `place op= expr` desugars to `place = place op expr`, except that `place` is evaluated only once.

**Movability Preservation**

The movability property of a binding is determined at declaration and preserved across reassignments:

- A `var x = v` binding remains movable after reassignment.
- A `var x := v` binding remains immovable after reassignment; the new value is also immovable.

Reassignment uses the plain `=` operator regardless of movability. The `:=` operator appears only at binding declaration.

##### Dynamic Semantics

**Drop on Reassignment**

If the place refers to an initialized, responsible binding, the implementation MUST invoke the destructor (`Drop::drop`) of the current value before installing the new value. This destruction is deterministic:

1. The new value expression is evaluated.
2. The old value's destructor is invoked.
3. The new value is installed.

##### Constraints & Legality

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-STM-2631` | Error    | Assignment target is not a place expression. | Compile-time | Rejection |
| `E-DEC-2401` | Error    | Assignment to immutable `let` binding.       | Compile-time | Rejection |
| `E-STM-2632` | Error    | Assignment through `const` permission.       | Compile-time | Rejection |
| `E-STM-2633` | Error    | Assignment type mismatch.                    | Compile-time | Rejection |

---

### 11.12 Defer Statements

##### Definition

A **defer statement** schedules a block of code for execution when the enclosing scope exits.

##### Syntax & Declaration

```ebnf
defer_stmt ::= "defer" block_expr
```

##### Static Semantics

1. The deferred block MUST have type `()`.
2. The deferred block MUST NOT contain non-local control flow (`return`, `break`, `continue`) that would transfer control outside the defer block.
3. The deferred block MAY reference bindings from the enclosing scope that are in scope at the `defer` statement.

##### Dynamic Semantics

**Registration**

When a `defer` statement is executed, the block is not evaluated immediately. Instead, it is pushed onto a per-scope stack of deferred actions.

**Execution Order (LIFO)**

When a scope exits—by normal completion, `return`, `result`, `break`, or panic—all deferred blocks for that scope are executed in Last-In, First-Out (LIFO) order.

```cursive
procedure example() {
    defer { log("first defer") }   // Executes second
    defer { log("second defer") }  // Executes first
    // ... body ...
}  // Output: "second defer", "first defer"
```

**Nested Scopes**

Each block has its own defer stack. When a nested block exits, only its deferred actions execute. When the outer scope exits, its deferred actions execute.

**Panics in Defer**

If a panic occurs during execution of a deferred block:

1. The remaining deferred blocks in that scope are still executed.
2. After all deferred blocks complete, the panic propagates.
3. If multiple panics occur, behavior is Implementation-Defined.

##### Constraints & Legality

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-STM-2651` | Error    | Defer block has non-unit type.         | Compile-time | Rejection |
| `E-STM-2652` | Error    | Non-local control flow in defer block. | Compile-time | Rejection |

---

### Clause 11 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 11 That MUST NOT Be Redefined Elsewhere**

| Term                | Section | Description                                        |
| :------------------ | :------ | :------------------------------------------------- |
| Expression          | §11.1   | Syntactic form producing typed value               |
| Statement           | §11.1   | Syntactic form executed for side effects           |
| Typing Context      | §11.1   | Mapping $\Gamma$ from identifiers to types         |
| Value Category      | §11.1   | Classification: Place vs. Value expression         |
| Place Expression    | §11.1   | Expression denoting memory location                |
| Value Expression    | §11.1   | Expression producing temporary value               |
| Pattern             | §11.2   | Syntactic form for destructuring and testing       |
| Irrefutable Pattern | §11.2   | Pattern that always matches                        |
| Refutable Pattern   | §11.2   | Pattern that may fail to match                     |
| Exhaustiveness      | §11.2   | Property: patterns cover all possible values       |
| Unreachability      | §11.2   | Property: arm can never execute                    |
| Precedence          | §11.3   | Operator binding strength ranking                  |
| Associativity       | §11.3   | Grouping rule for equal-precedence operators       |
| Short-Circuit Eval  | §11.4.6 | Second operand conditionally skipped               |
| Closure Expression  | §11.5   | Anonymous callable with optional captures          |
| Capture             | §11.5   | Binding referenced by closure from enclosing scope |

**Terms Referenced From Other Clauses**

| Term                      | Source | Usage in Clause 11                              |
| :------------------------ | :----- | :---------------------------------------------- |
| Type                      | §4.1   | Expression typing, pattern typing               |
| Permission                | §4.5   | Assignment constraints, closure captures        |
| Subtyping                 | §4.3   | Type compatibility in assignments and arguments |
| Function Type             | §6.3   | Closure typing, procedure call typing           |
| Binding, Responsible      | §3.4   | Declaration statements, binding state           |
| Move Semantics            | §3.5   | Move expression, closure captures               |
| Deterministic Destruction | §3.6   | Assignment drop behavior, scope exit            |
| Region                    | §3.9   | Region allocation operator                      |
| Unsafe Operations         | §3.10  | Unsafe block, raw pointer dereference           |
| Ptr<T>@State              | §6.2   | Address-of result type, dereference constraints |
| Statement Termination     | §2.11  | Statement boundary rules                        |
| Drop Form                 | §9.7   | Assignment drop semantics, scope exit           |
| Iterator Form             | §9.x   | Iterator loop desugaring                        |
| Name Resolution           | §8.4   | Identifier lookup                               |

**Terms Deferred to Other Clauses**

| Term               | Clause | Reason                      |
| :----------------- | :----- | :-------------------------- |
| `parallel` block   | §13.3  | Concurrency semantics       |
| `comptime` block   | §14.1  | Metaprogramming semantics   |
| `unsafe` details   | §3.10  | Safety system specification |
| `region` block     | §3.9   | Memory region allocation    |
| Contract statement | §10.x  | Verification system         |

---

## Clause 12: The Capability System

This clause defines the Cursive Capability System, which governs all procedures that produce observable **external effects** (e.g., I/O, networking, threading, heap allocation) and enforces the security principle of **No Ambient Authority**.

### 12.1 Foundational Principles

##### Definition

**Capability**

A **Capability** is a first-class value representing unforgeable authority to perform a specific class of external effects. Capabilities are the sole mechanism by which a Cursive program may interact with the external world.

**Formal Definition**

$$\text{Capability} ::= (\text{Authority}, \text{Interface}, \text{Provenance})$$

Where:
- **Authority**: The set of permitted operations (e.g., read files, connect to network)
- **Interface**: The form defining available methods
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

### 12.2 The Root of Authority

##### Definition

**The Capability Root**

All system-level capabilities originate from the Cursive runtime and are injected into the program via the `Context` parameter at the entry point.

**Cross-Reference**: The entry point signature `public procedure main(ctx: Context) -> i32` and the `Context` record structure are normatively defined in §5.6 (Program Entry Point).

**Runtime Injection**

The Cursive runtime constructs the `Context` record before program execution begins. This record contains concrete implementations of all system capability forms, initialized with full authority over system resources.

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
└── heap: witness HeapAllocator (dynamic allocation)
```

##### Constraints & Legality

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-DEC-2431` | Error    | `main` signature incorrect (see §5.6 for spec). |

---

### 12.3 Capability Attenuation

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

1. Implements the same capability form as the parent
2. Enforces the specified restrictions on all operations
3. Delegates authorized operations to the parent capability

**Type Preservation Rule**

Attenuation preserves the capability form:

$$\frac{\Gamma \vdash c : \text{witness } T \quad \Gamma \vdash c.restrict(r) : \text{witness } T'}{\Gamma \vdash T' \equiv T}$$

---

### 12.4 Capability Propagation

##### Definition

**Capability Propagation**

Capabilities travel through the call graph as explicit parameters. A procedure requiring a capability **MUST** accept it as a parameter; capabilities cannot be obtained through any other means in safe code.

**Granular Capability Pattern**

Low-level or reusable procedures **SHOULD** accept only the specific capability forms they require. This enables:
- Maximum code reuse (works with any implementation of the form)
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

A parameter of type `witness Form` accepts any concrete type `T` where `T` implements `Form`.

**Permission Requirements for Capability Methods**

Capability methods **MUST** declare appropriate receiver permissions:

| Operation Type                 | Required Permission | Rationale                         |
| :----------------------------- | :------------------ | :-------------------------------- |
| Side-effecting I/O             | `shared` (`~%`)     | Allows synchronized shared access |
| Pure queries (no state change) | `const` (`~`)       | No state change; safe for sharing |

**Cross-Reference**: `shared` permission semantics defined in §4.5.

##### Constraints & Legality

**Implicit Capability Prohibition**

A procedure **MUST NOT** access capabilities not explicitly provided as parameters or derived from provided parameters.

| Code         | Severity | Condition                                                   |
| :----------- | :------- | :---------------------------------------------------------- |
| `E-CAP-1002` | Error    | Effect-producing procedure lacks required capability param. |

---

### 12.5 System Capability Forms

##### Definition

System capabilities are defined by **Forms** (interfaces). This design enables:
- **Attenuation**: Restricted implementations can wrap full-authority implementations
- **Virtualization**: Mock implementations for testing
- **Abstraction**: Code depends on interface, not implementation

**Cross-Reference**: The normative definitions of these forms are provided in **Appendix D.2** (System Capability Forms).

**FileSystem Form**

Governs access to the host filesystem.

| Requirement        | Description                                     |
| :----------------- | :---------------------------------------------- |
| Operations         | File read, write, open (returning `FileHandle`) |
| Attenuation Method | `restrict(path)` - limits to directory subtree  |

**Network Form**

Governs access to network sockets.

| Requirement        | Description                                             |
| :----------------- | :------------------------------------------------------ |
| Operations         | `connect` to remote hosts, bind listeners               |
| Attenuation Method | `restrict_to_host(addr)` - limits to specific hosts/IPs |

**HeapAllocator Form**

Governs dynamic memory allocation.

| Requirement        | Description                                   |
| :----------------- | :-------------------------------------------- |
| Operations         | `alloc` and `free` methods                    |
| Attenuation Method | `with_quota(bytes)` - fails if quota exceeded |

**System Record**

Aggregates miscellaneous system-level primitives. Unlike the forms above, `System` is a concrete record (not a form) because it bundles unrelated operations.

| Method    | Signature                         | Description                 |
| :-------- | :-------------------------------- | :-------------------------- |
| `get_env` | `(~, key: string@View) -> string` | Environment variable access |
| `time`    | `(~) -> Timestamp`                | Current timestamp           |
| `exit`    | `(~, code: i32) -> !`             | Process termination         |

##### Static Semantics

**Method Receiver Permissions**

System capability methods **MUST** declare receiver permissions that accurately reflect their side effects:

$$\frac{m : T.\text{method} \quad \text{HasSideEffect}(m)}{\Gamma \vdash m.\text{receiver} = \texttt{shared}}$$

**Const Purity Rule**

Methods accepting `const` (`~`) receiver **MUST** guarantee **zero** logical state changes to the external resource:
- `time~>now()` - const (reading clock has no side effect)
- `file~>write(data)` - shared (modifies file state)

##### Constraints & Legality

| Code         | Severity | Condition                                             |
| :----------- | :------- | :---------------------------------------------------- |
| `E-CAP-1003` | Error    | Side-effecting capability method has `const` receiver |

---

### 12.6 User-Defined Capabilities

##### Definition

**User-Defined Capability**

Users **MAY** define application-level capability types representing domain-specific authority (e.g., `DatabaseAccess`, `BillingService`, `AuditLog`).

A user-defined capability is typically implemented as a `record` that:

1. Implements a domain-specific form defining the capability interface
2. Holds system capabilities internally as `private` or `protected` fields
3. Provides methods that use internal capabilities to perform authorized operations

**Encapsulation Principle**

By wrapping system capabilities in `private` fields, the user-defined capability restricts access to the raw system resource, allowing only the application-specific operations defined by its methods.

##### Syntax & Declaration

**Domain Form Definition**

```cursive
// Define the capability interface
form DatabaseAccess {
    procedure query(~%, sql: string@View): QueryResult
    procedure execute(~%, sql: string@View): u64
}
```

**Capability Implementation**

```cursive
// Implement the capability wrapping system resources
record PostgresConnection <: DatabaseAccess {
    private net: witness Network,
    private connection_string: string@View,

    procedure query(~%, sql: string@View): QueryResult {
        // Uses self.net internally to communicate with database
        // ...
    }

    procedure execute(~%, sql: string@View): u64 {
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

    procedure query(~%, sql: string@View): QueryResult {
        self.inner~>query(sql)
    }

    procedure execute(~%, sql: string@View): u64 {
        panic("Write operations not permitted on read-only connection")
    }
}
```

---

### Clause 12: The Capability System Cross-Reference Notes `[INFORMATIVE]`

**Terms defined in Clause 10 that MUST NOT be redefined elsewhere:**

| Term                        | Section | Description                                               |
| :-------------------------- | :------ | :-------------------------------------------------------- |
| Capability                  | §10.1   | First-class unforgeable authority value                   |
| No Ambient Authority        | §10.1   | Core principle prohibiting global side effects            |
| Authority                   | §10.1   | The set of permitted operations for a capability          |
| Attenuation                 | §10.3   | Process of deriving restricted capability from parent     |
| Attenuation Invariant       | §10.3   | Child capability cannot exceed parent authority           |
| Capability Propagation      | §10.4   | Explicit parameter passing of capabilities                |
| Granular Capability Pattern | §10.4   | Accept only the specific capability forms required        |
| Capability Bundle Pattern   | §10.4   | Aggregate multiple capabilities in a record               |
| User-Defined Capability     | §10.6   | Application-level capability wrapping system capabilities |

**Terms referenced:**

| Term                    | Source       | Usage in Clause 10                             |
| :---------------------- | :----------- | :--------------------------------------------- |
| `main` procedure        | §5.6         | Entry point for capability injection           |
| `Context` record        | §5.6         | Root capability container                      |
| Permission types        | §4.5         | `shared`, `const` for method receivers         |
| `shared` permission     | §4.5         | Required for side-effecting capability methods |
| `witness` types         | §6.3         | Dynamic polymorphism for capability forms      |
| Form                    | §6.1         | Interface defining capability methods          |
| System Capability Forms | Appendix D.2 | Normative form definitions                     |

**Terms deferred to later clauses:**

| Term         | Deferred To  | Reason                       |
| :----------- | :----------- | :--------------------------- |
| `FileHandle` | Appendix D.2 | Defined with FileSystem form |
| `Timestamp`  | Appendix D.2 | Defined with System record   |

---
# Part 4: Concurrency 

This clause defines the Cursive concurrency model, which provides safe concurrent and parallel programming through a key-based synchronization system, structured concurrency primitives, and zero-cost abstractions for shared mutable state.

The Cursive concurrency model is built on five principles:

1. **Shared data is locked by default.** A `shared` value is opaque—you cannot access its contents without a key.

2. **Keys unlock, not lock.** The mental model is a locked box requiring a key to open, not a guard blocking access.

3. **Keys are implicit and fine-grained.** Accessing a path implicitly acquires a key to exactly that path. Maximum parallelism emerges naturally.

4. **No ambient concurrency.** Spawning tasks requires an explicit `parallel` block. There is no global thread pool or hidden parallelism.

5. **Structured concurrency by default.** Tasks are scoped to `parallel` blocks and complete before the block exits.

---

## Clause 13: The Key System

This clause defines the key system, which provides safe access to `shared` data through static verification with runtime synchronization as a fallback.

---

### 13.1 Key Fundamentals

##### Definition

A **key** is a **static proof of access rights** to a specific path within `shared` data. Keys are primarily a compile-time verification mechanism: the compiler tracks key state during type checking to ensure safe access patterns. Runtime synchronization is introduced only when static analysis cannot prove safety.

**Formal Definition**

A key is a triple:

$$\text{Key} ::= (\text{Path}, \text{Mode}, \text{Scope})$$

| Component | Type                            | Description                                     |
| :-------- | :------------------------------ | :---------------------------------------------- |
| Path      | $\text{PathExpr}$               | The memory location being accessed              |
| Mode      | $\{\text{Read}, \text{Write}\}$ | The access grant (see §13.1.2)                  |
| Scope     | $\text{LexicalScope}$           | The lexical scope during which the key is valid |

**Program Point**

A **Program Point** is a unique location in the abstract control flow representation of a procedure body at which the compiler tracks state.

$$\text{ProgramPoint} ::= (\text{ProcedureId}, \text{CFGNodeId})$$

where:
- $\text{ProcedureId}$ uniquely identifies the enclosing procedure
- $\text{CFGNodeId}$ uniquely identifies a node in that procedure's control flow graph

Program points are partially ordered by control flow reachability: $p_1 \leq_{cf} p_2$ if and only if $p_2$ is reachable from $p_1$ via zero or more control flow edges.

**Lexical Scope**

A **Lexical Scope** is a contiguous region of source text that defines a lifetime boundary for keys.

$$\text{LexicalScope} ::= (\text{EntryPoint}, \text{ExitPoint}, \text{Parent}?)$$

where:
- $\text{EntryPoint}$ is the program point where the scope begins
- $\text{ExitPoint}$ is the program point where the scope ends
- $\text{Parent}$ is the immediately enclosing scope (absent for procedure-level scope)

**Scope Hierarchy**

Lexical scopes form a forest of trees, with one tree per procedure, rooted at procedure scope. A scope $S_1$ is **nested within** $S_2$ (written $S_1 \sqsubset S_2$) if $S_2$ is an ancestor of $S_1$ in this tree.

**Scope-Introducing Constructs**

| Construct         | Scope Entry              | Scope Exit                |
| :---------------- | :----------------------- | :------------------------ |
| Procedure body    | Opening `{`              | Closing `}` or `return`   |
| Block expression  | Opening `{`              | Closing `}`               |
| `if` branch       | Branch entry             | Branch exit               |
| `match` arm       | Arm entry                | Arm exit                  |
| `loop` body       | Body entry               | Body exit (per iteration) |
| `#path { }` block | Opening `{`              | Closing `}`               |
| `defer` block     | Deferred execution start | Deferred execution end    |

**Key State Context**

The compiler MUST track key state at each program point using a key state context:

$$\Gamma_{\text{keys}} : \text{ProgramPoint} \to \mathcal{P}(\text{Key})$$

This mapping associates each program point with the set of keys logically held at that point.

**Held Predicate**

A key is **held** at a program point if it is a member of the key state context at that point:

$$\text{Held}(P, M, S, \Gamma_{\text{keys}}, p) \iff (P, M, S) \in \Gamma_{\text{keys}}(p)$$

When the program point is clear from context, we write $\text{Held}(P, M, \Gamma_{\text{keys}})$ or simply $\text{Held}(P, M)$.

**Compile-Time vs. Runtime**

| Aspect                  | Compile-Time                   | Runtime                              |
| :---------------------- | :----------------------------- | :----------------------------------- |
| **Key tracking**        | Always performed               | N/A                                  |
| **Safety verification** | Always attempted               | Fallback when static proof fails     |
| **Synchronization**     | None (zero cost)               | Emitted only when necessary          |
| **Representation**      | Abstract; no runtime footprint | Implementation-defined when required |

**Key Invariants**

The following invariants are enforced by the compiler and, when necessary, by runtime mechanisms:

1. **Path-specificity:** A key to path $P$ grants access only to $P$ and paths for which $P$ is a prefix. A key to `player.health` does not grant access to `player.mana`.

2. **Implicit acquisition:** Accessing a `shared` path logically acquires the necessary key. Keys are not exposed to user code as values; there is no syntax to explicitly acquire, release, store, or pass keys.

3. **Scoped lifetime:** Keys are valid for a bounded lexical scope and become invalid when that scope exits.

4. **Reentrancy:** If a key covering path $P$ is already held by the current task, nested access to $P$ or any path prefixed by $P$ succeeds without conflict.

5. **Task locality:** Keys are associated with tasks (§14.1), not procedures or bindings. A key held by a task remains valid until its scope exits, even across procedure calls within that task.

##### Syntax & Declaration

**Path Expression Grammar**

A **path expression** identifies a location within a value:

```ebnf
path_expr       ::= root_segment ("." path_segment)*

root_segment    ::= key_marker? IDENTIFIER index_suffix?

path_segment    ::= key_marker? IDENTIFIER index_suffix?

key_marker      ::= "#"

index_suffix    ::= "[" expression "]"
```

**Terminology:**

- The **root** of a path is the first identifier (e.g., `player` in `player.stats.health`)
- A **segment** is a single field access or indexed access
- The **depth** of a path is the number of segments (e.g., `player.stats.health` has depth 3)

**Path Root Extraction**

The **root** of an expression, written $\text{Root}(e)$, extracts the base identifier from which key path analysis begins. The operation is defined recursively:

**Formal Definition**

$$\text{Root}(e) ::= \begin{cases}
x & \text{if } e = x \text{ (identifier)} \\
\text{Root}(e') & \text{if } e = e'.f \text{ (field access)} \\
\text{Root}(e') & \text{if } e = e'[i] \text{ (index access)} \\
\text{Root}(e') & \text{if } e = e'.m(\ldots) \text{ (method call)} \\
\bot_{\text{boundary}} & \text{if } e = (*e') \text{ (dereference)}
\end{cases}$$

where $\bot_{\text{boundary}}$ indicates a **key boundary**—the dereferenced value establishes an independent key context (see "Pointer Indirection and Key Boundaries" below).

**Key Boundary Semantics**

When $\text{Root}(e) = \bot_{\text{boundary}}$, the expression $e$ introduces a new key path rooted at the runtime identity of the dereferenced value:

$$\frac{
    e = (*e').p \quad
    \text{Root}(e) = \bot_{\text{boundary}}
}{
    \text{KeyPath}(e) = \text{id}(*e').p
}$$

The $\text{id}$ operation is defined in §13.3.3.

**Examples:**

```cursive
player                      // Depth 1: root only
player.health               // Depth 2: root + one field
player.stats.health         // Depth 3: root + two fields
player.inventory[5]         // Depth 2: root + indexed field
team.players[0].health      // Depth 3: root + indexed field + field
```

**Pointer Indirection and Key Boundaries**

A pointer dereference creates a **key boundary**: the dereferenced value establishes a new, independent key context. Key paths do not extend across pointer indirections.

For path `(*e).p` where `e : shared Ptr<T>@Valid`:
- A key is first acquired for `e` (to safely dereference)
- A separate key is then acquired for `(*e).p`, rooted at the dereferenced value

This rule ensures that concurrent access to different nodes in recursive structures (e.g., linked lists, trees) can proceed in parallel without contention, provided the nodes themselves are distinct allocations.

**Key Boundary Rules:**

1. **Direct field paths:** `a.b.c` is a single key path (no indirection)
2. **Pointer field access:** Accessing a pointer field (e.g., `list.next`) acquires a fine-grained key to that field path (`list.next`), following normal key acquisition behavior
3. **Pointer dereference:** `(*ptr).field` starts a new key path rooted at the dereferenced value
4. **Coarsening applies:** The `#` operator can coarsen pointer field access to the containing node (e.g., `#list.next` acquires key to `list`)

**(K-Deref-Boundary)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ \text{Ptr}\langle T \rangle@\text{Valid} \quad
    \Gamma \vdash (*e).p : \texttt{shared}\ U
}{
    \text{KeyPath}((*e).p) = \text{id}(*e).p \quad
    \text{(independent of KeyPath}(e)\text{)}
}$$

**Examples:**

```cursive
// Linked list traversal
let list: shared List<i32> = ...

list.value          // Key: list.value (path within first node)
(*list.next).value  // Key: (*list.next).value (new key context)

// Each node access is independent
parallel {
    spawn { list.value = 1 }           // Key to list.value
    spawn { (*list.next).value = 2 }   // Key to (*list.next).value — DISJOINT
}
```

##### Static Semantics

**Path Well-Formedness**

A path expression $P = p_1.p_2.\ldots.p_n$ is **well-formed** if and only if:

1. The root identifier $p_1$ is bound in scope $\Gamma$ with type $T_0$
2. For each subsequent segment $p_i$ ($i > 1$):
   - If $p_i$ is a field access, $T_{i-1}$ has a field named $p_i$ with type $T_i$
   - If $p_i$ is an index access $[e]$, $T_{i-1}$ is an array or slice type with element type $T_i$, and $e$ has type `usize`
3. At most one `#` marker appears in the entire path (see §13.4)

**(WF-Path)**
$$\frac{
    \Gamma \vdash p_1 : T_0 \quad
    \forall i \in 2..n,\ \Gamma \vdash T_{i-1}.p_i : T_i
}{
    \Gamma \vdash p_1.p_2.\ldots.p_n : T_n\ \text{wf}
}$$

**Path Typing for Key Determination**

A path expression has an associated **permission** derived from the root binding:

**(Path-Permission)**
$$\frac{
    \Gamma \vdash p_1 : P\ T_0 \quad P \in \{\texttt{const}, \texttt{unique}, \texttt{shared}\}
}{
    \Gamma \vdash p_1.p_2.\ldots.p_n : P\ T_n
}$$

**Key Analysis Applicability**

Key analysis is performed if and only if the path permission is `shared`:

$$\text{RequiresKeyAnalysis}(P) \iff \text{Permission}(P) = \texttt{shared}$$

Paths with `const` or `unique` permission require no key analysis (they are statically safe by the permission system defined in §4.5).

**Key Path Determination for Method Calls**

When a path expression includes a method call, the key path is determined by the method's return type and receiver:

1. If the method returns a reference into the receiver's data (e.g., a field accessor), the key path extends through the method call to the returned reference's target.

2. If the method returns an independent value (not a reference into the receiver), the returned value is a fresh binding and key analysis applies to accesses on that binding independently.

3. For method chaining on `shared` data, each method call in the chain is analyzed for its receiver permission. If a method requires `unique` or write access to its receiver, the key path MUST include the receiver with Write mode.

**(K-Method-Call-Path)**
$$\frac{
    \Gamma \vdash e.\text{method}() : \texttt{shared}\ T \quad
    \text{method returns reference into } e
}{
    \text{KeyPath}(e.\text{method}().f) = \text{KeyPath}(e) \cup \{f\}
}$$

> **Note:** Method calls that return owned values (not references) create new bindings. Key analysis for subsequent accesses on such values is independent of the original receiver's key state.

##### Memory & Layout

**Compile-Time Representation**

Keys have no runtime representation when statically proven safe. The compiler tracks key state in $\Gamma_{\text{keys}}$ during type checking as an abstract data structure with no physical manifestation.

**Runtime Representation (When Required)**

When runtime synchronization is necessary, key metadata representation is Implementation-Defined Behavior (IDB). Implementations MUST document in the Conformance Dossier:

1. Whether synchronization metadata is stored inline within `shared` values or in external data structures
2. Size overhead per `shared` value, if any
3. Lock table or hash table organization, if used
4. Memory ordering guarantees for metadata access

**ABI Guarantees**

| Aspect                          | Guarantee                                           |
| :------------------------------ | :-------------------------------------------------- |
| Key metadata presence           | IDB — may or may not exist at runtime               |
| Key metadata layout             | IDB — not observable by conforming programs         |
| `shared T` vs `unique T` layout | MAY be identical when statically safe               |
| `shared T` size                 | MAY differ from `T` size when runtime sync required |

##### Constraints & Legality

**Negative Constraints**

1. A program MUST NOT access a `shared` path outside a valid key context.
2. A program MUST NOT apply the `#` annotation to a non-`shared` path.
3. A program MUST NOT include multiple `#` markers in a single path expression.
4. A program MUST NOT cause a key to escape its defining scope.

**Key Escape**

A key **escapes** its defining scope $S$ if any of the following conditions hold:

1. A reference to a `shared` path $P$ for which a key is held in $S$ is stored in a binding whose lifetime exceeds $S$
2. A reference to such a path is returned from a procedure whose body contains $S$
3. A reference to such a path is captured by a closure that outlives $S$
4. A reference to such a path is passed to a `spawn` expression (transferring to another task)

**(K-No-Escape)**
$\frac{
    \text{Held}(P, M, S) \quad
    \text{Ref}(P) \text{ stored in } B \quad
    \text{Lifetime}(B) > \text{Lifetime}(S)
}{
    \text{Emit}(\texttt{E-KEY-0004})
}$

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-KEY-0001` | Error    | Access to `shared` path outside valid key context | Compile-time | Rejection |
| `E-KEY-0002` | Error    | `#` annotation on non-`shared` path               | Compile-time | Rejection |
| `E-KEY-0003` | Error    | Multiple `#` markers in single path expression    | Compile-time | Rejection |
| `E-KEY-0004` | Error    | Key escapes its defining scope                    | Compile-time | Rejection |

**Diagnostic Examples**

`E-KEY-0001` is emitted when the compiler cannot establish a valid key context for a `shared` access:

```cursive
// Example: E-KEY-0001 — shared access without key acquisition capability
unsafe {
    let raw_ptr: *shared Player = get_raw_shared_ptr()
    (*raw_ptr).health = 100   // ERROR E-KEY-0001: unsafe dereference bypasses key system
}
```

> **Note:** In safe code, key acquisition is implicit and E-KEY-0001 is rare. It typically indicates a violation of the key system's invariants, such as accessing `shared` data through raw pointers in `unsafe` blocks where key acquisition has been bypassed.

---

#### 13.1.1 Path Prefix and Disjointness

##### Definition

The **prefix relation** and **disjointness relation** on paths are used by the compiler to determine key coverage and to prove static safety of concurrent accesses.

**Formal Definition**

**Path Prefix:**

Path $P$ is a **prefix** of path $Q$ (written $\text{Prefix}(P, Q)$) if $P$ is an initial subsequence of $Q$:

$$\text{Prefix}(p_1.\ldots.p_m,\ q_1.\ldots.q_n) \iff m \leq n \land \forall i \in 1..m,\ p_i \equiv_{\text{seg}} q_i$$

**Path Disjointness:**

Two paths $P$ and $Q$ are **disjoint** (written $\text{Disjoint}(P, Q)$) if neither is a prefix of the other:

$$\text{Disjoint}(P, Q) \iff \neg\text{Prefix}(P, Q) \land \neg\text{Prefix}(Q, P)$$

**Path Overlap**

Two paths $P$ and $Q$ **overlap** (written $\text{Overlaps}(P, Q)$) if they are not disjoint—that is, if one is a prefix of the other:

$$\text{Overlaps}(P, Q) \iff \neg\text{Disjoint}(P, Q)$$

Equivalently:

$$\text{Overlaps}(P, Q) \iff \text{Prefix}(P, Q) \lor \text{Prefix}(Q, P)$$

**Overlap Semantics**

| Relationship         | Overlaps? | Implication for Keys                |
| :------------------- | :-------- | :---------------------------------- |
| $P$ is prefix of $Q$ | Yes       | Key to $P$ covers access to $Q$     |
| $Q$ is prefix of $P$ | Yes       | Key to $Q$ covers access to $P$     |
| Neither is prefix    | No        | Keys to $P$ and $Q$ are independent |
| $P = Q$              | Yes       | Same path                           |

**Segment Equivalence:**

Two segments are equivalent ($\equiv_{\text{seg}}$) if they have the same identifier and, for indexed segments, provably equivalent index expressions:

$$p_i \equiv_{\text{seg}} q_i \iff \text{name}(p_i) = \text{name}(q_i) \land \text{IndexEquiv}(p_i, q_i)$$

where $\text{IndexEquiv}$ is defined as:

$$\text{IndexEquiv}(p, q) \iff \begin{cases}
\text{true} & \text{if neither } p \text{ nor } q \text{ is indexed} \\
e_p \equiv_{\text{idx}} e_q & \text{if both are indexed with expressions } e_p, e_q \\
\text{false} & \text{otherwise}
\end{cases}$$

**Index Expression Equivalence:**

Two index expressions $e_1$ and $e_2$ are **provably equivalent** ($e_1 \equiv_{\text{idx}} e_2$) if and only if one of the following conditions holds:

1. Both are the same integer literal
2. Both are references to the same `const` binding
3. Both are references to the same generic const parameter
4. Both are references to the same variable binding in scope
5. Both normalize to the same canonical form under constant folding

**Canonical Form Normalization**

The implementation MUST normalize index expressions using the following transformations. Two expressions that normalize to the same canonical form are **provably equivalent**.

**Required Transformations (MUST)**

| Transformation          | Before                     | After                   |
| :---------------------- | :------------------------- | :---------------------- |
| Additive identity       | $x + 0$, $0 + x$           | $x$                     |
| Subtractive identity    | $x - 0$                    | $x$                     |
| Multiplicative identity | $x \times 1$, $1 \times x$ | $x$                     |
| Constant folding        | $2 + 3$                    | $5$                     |
| Associative regrouping  | $(a + b) + c$              | $a + (b + c)$           |
| Commutative reordering  | $b + a$                    | $a + b$ (lexicographic) |

**Optional Transformations (MAY)**

The implementation MAY apply additional algebraic simplifications beyond the required set. Common optional transformations include:

- Distribution: $a \times (b + c) \to (a \times b) + (a \times c)$
- Factoring: $(a \times b) + (a \times c) \to a \times (b + c)$
- Cancellation: $a - a \to 0$, $a + (-a) \to 0$

**Conservative Fallback**

If normalization exceeds implementation limits (§1.4), or if optional transformations would be required to prove equivalence, the expressions MUST be treated as **not provably equivalent**. In such cases:

1. If the expressions appear in the same statement accessing the same array, the implementation MUST emit `E-KEY-0010` (potential conflict on dynamic indices).
2. If the expressions appear in different tasks, runtime synchronization MUST be emitted.

**Implementation-Defined Documentation**

The implementation MUST document in the Conformance Dossier:

1. Which optional transformations are applied
2. The maximum complexity of expressions for which normalization is attempted
3. Examples of expressions that normalize to equivalent forms

**Provable Disjointness for Indices:**

Two index expressions $e_1$ and $e_2$ are **provably disjoint** if:
1. Both are statically resolvable (per §13.6), AND
2. Their static values differ

$$\text{ProvablyDisjointIdx}(e_1, e_2) \iff \text{StaticResolvable}(e_1) \land \text{StaticResolvable}(e_2) \land \text{StaticValue}(e_1) \neq \text{StaticValue}(e_2)$$

##### Static Semantics

**Disjointness for Static Safety**

When the compiler can prove two paths are disjoint, concurrent accesses to those paths are statically safe and require no runtime synchronization:

**(K-Disjoint-Safe)**
$$\frac{
    \text{Disjoint}(P, Q)
}{
    \text{ConcurrentAccess}(P, Q) \text{ is statically safe}
}$$

**Prefix for Coverage**

When a held key's path is a prefix of an access path, the held key covers the access:

**(K-Prefix-Coverage)**
$$\frac{
    \text{Prefix}(P, Q) \quad \text{Held}(P, M, \Gamma_{\text{keys}})
}{
    \text{Covers}((P, M), Q)
}$$

##### Examples

| Path P          | Path Q          | Prefix(P,Q) | Prefix(Q,P) | Disjoint | Concurrent Access   |
| :-------------- | :-------------- | :---------- | :---------- | :------- | :------------------ |
| `player`        | `player.health` | true        | false       | false    | Conflicts           |
| `player.health` | `player.mana`   | false       | false       | true     | **Statically safe** |
| `arr[0]`        | `arr[1]`        | false       | false       | true     | **Statically safe** |
| `arr[0]`        | `arr[0].field`  | true        | false       | false    | Conflicts           |
| `a.b.c`         | `a.b`           | false       | true        | false    | Conflicts           |
| `arr[i]`        | `arr[i]`        | true        | true        | false    | Same path           |
| `arr[i]`        | `arr[j]`        | unknown     | unknown     | unknown  | See §13.6           |

---

#### 13.1.2 Key Modes

##### Definition

A key's **mode** determines what operations it permits and how it interacts with other keys to the same or overlapping paths.

**Formal Definition**

$$\text{Mode} ::= \text{Read} \mid \text{Write}$$

| Mode  | Permitted Operations       | Exclusivity                                                       |
| :---- | :------------------------- | :---------------------------------------------------------------- |
| Read  | Read the value at the path | Shared: multiple Read keys to overlapping paths MAY coexist       |
| Write | Read and write the value   | Exclusive: Write key excludes all other keys to overlapping paths |

**Read Context and Write Context**

The predicates $\text{ReadContext}(e)$ and $\text{WriteContext}(e)$ classify expression positions by their access requirements.

**Formal Definition:**

$$\text{ReadContext}(e) \iff e \text{ appears in a syntactic position requiring only read access}$$

$$\text{WriteContext}(e) \iff e \text{ appears in a syntactic position requiring write access}$$

**Classification Rules:**

| Syntactic Position                                   | Context | Rationale                                |
| :--------------------------------------------------- | :------ | :--------------------------------------- |
| Right-hand side of `let`/`var` initializer           | Read    | Value is read for initialization         |
| Right-hand side of assignment (`=`)                  | Read    | Value is read for assignment             |
| Operand of arithmetic/logical operator               | Read    | Value is read for computation            |
| Argument to `const` or `shared` parameter            | Read    | Callee does not require exclusive access |
| Condition expression (`if`, `while`, `match`)        | Read    | Value is read for branching              |
| Left-hand side of assignment (`=`)                   | Write   | Target location is mutated               |
| Left-hand side of compound assignment (`+=`, etc.)   | Write   | Target is read and mutated               |
| Receiver of method with `~!` (unique) receiver       | Write   | Method requires exclusive access         |
| Receiver of method with `~%` (shared-write) receiver | Write   | Method requires write access             |
| Argument to `unique` parameter                       | Write   | Callee requires exclusive access         |
| Receiver of method with `~` (const) receiver         | Read    | Method requires only read access         |

**Disambiguation Rule:**

If an expression appears in multiple contexts (e.g., compound assignment target), the more restrictive context (Write) applies.

##### Static Semantics

**Mode Determination**

The required key mode is determined by the syntactic context of the access:

**(K-Mode-Read)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ T \quad
    \text{ReadContext}(e)
}{
    \text{RequiredMode}(e) = \text{Read}
}$$

**(K-Mode-Write)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ T \quad
    \text{WriteContext}(e)
}{
    \text{RequiredMode}(e) = \text{Write}
}$$

**Key Compatibility**

Two keys $K_1 = (P_1, M_1, S_1)$ and $K_2 = (P_2, M_2, S_2)$ are **compatible** (may coexist) if and only if:

$$\text{Compatible}(K_1, K_2) \iff \text{Disjoint}(P_1, P_2) \lor (M_1 = \text{Read} \land M_2 = \text{Read})$$

**Compatibility Matrix:**

| Key A Mode | Key B Mode | Paths Overlap? | Compatible? |
| :--------- | :--------- | :------------- | :---------- |
| Read       | Read       | Yes            | Yes         |
| Read       | Write      | Yes            | No          |
| Write      | Read       | Yes            | No          |
| Write      | Write      | Yes            | No          |
| Any        | Any        | No (disjoint)  | Yes         |

##### Dynamic Semantics

**When Runtime Synchronization Is Required**

If the compiler cannot statically prove that concurrent accesses are compatible, runtime synchronization MUST enforce the compatibility rules:

- **Compatible keys:** May proceed concurrently
- **Incompatible keys:** One task blocks until the other releases

**Blocking Semantics (Runtime Only)**

When runtime synchronization is active and a task attempts to acquire an incompatible key:

1. The task MUST block until the conflicting key is released
2. Blocked tasks MUST be queued in a manner that ensures eventual acquisition
3. The blocking mechanism is Implementation-Defined Behavior (IDB)

**Progress Guarantee**

Implementations MUST guarantee **eventual progress**: any task blocked waiting for a key MUST eventually acquire that key, provided the holder eventually releases it.

Formally, for any task $t$ blocked on key $K$ held by task $t'$:

$$\text{Blocked}(t, K) \land \text{Held}(K, t') \land \Diamond\text{Released}(K, t') \implies \Diamond\text{Acquired}(K, t)$$

where $\Diamond$ is the temporal "eventually" operator.

> **Note (Informative):** Implementations MAY satisfy the progress guarantee via FIFO ordering of waiters, priority inheritance, bounded retry with timeout, or other mechanisms. The specific mechanism MUST be documented in the Conformance Dossier.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-KEY-0005` | Error    | Write access required but only Read available | Compile-time | Rejection |

---

### 13.2 Key Acquisition and Release

##### Definition

Keys are logically acquired **on demand** during expression evaluation and released **at scope exit**. The compiler tracks this lifecycle statically; runtime synchronization occurs only when static safety cannot be proven.

**Formal Definition**

Let $\text{KeySet}(s)$ denote the set of keys logically acquired during evaluation of statement or block $s$. The key lifecycle has three phases:

1. **Acquisition Phase:** Keys are logically acquired as `shared` paths are evaluated, in evaluation order
2. **Execution Phase:** The statement or block body executes with all keys logically held
3. **Release Phase:** All keys in $\text{KeySet}(s)$ become invalid when $s$'s scope exits

##### Formal Operation Definitions

**Mode Ordering and Sufficiency**

Key modes form a total order (§13.1.2): $\text{Read} < \text{Write}$.

A held mode is **sufficient** for a required mode when the held mode is at least as strong:

$$\text{ModeSufficient}(M_{\text{held}}, M_{\text{required}}) \iff M_{\text{required}} \leq M_{\text{held}}$$

**Key Set Operations**

Let $\Gamma_{\text{keys}}$ be the current key set, a collection of $(P, M, S)$ triples 
where $P$ is a path, $M$ is a mode, and $S$ is the scope identifier.

**(Acquire)**
$$\text{Acquire}(P, M, S, \Gamma_{\text{keys}}) = \Gamma_{\text{keys}} \cup \{(P, M, S)\}$$

**(Release)**
$$\text{Release}(P, \Gamma_{\text{keys}}) = \Gamma_{\text{keys}} \setminus \{(P, M, S) : (P, M, S) \in \Gamma_{\text{keys}}\}$$

**(Release by Scope)**
$$\text{ReleaseScope}(S, \Gamma_{\text{keys}}) = \Gamma_{\text{keys}} \setminus \{(P, M, S') : S' = S\}$$

**(Upgrade)**

The upgrade operation replaces a Read key with a Write key for the same path and scope. This is a compile-time transformation of the key state context; runtime behavior follows release-and-reacquire semantics (§13.7.2).

$$\text{Upgrade}(P, \Gamma_{\text{keys}}) = (\Gamma_{\text{keys}} \setminus \{(P, \text{Read}, S)\}) \cup \{(P, \text{Write}, S)\}$$

where $S$ is the scope of the existing Read key.

**Precondition**: A Read key to path $P$ MUST exist in $\Gamma_{\text{keys}}$:

$$\text{Upgrade}(P, \Gamma_{\text{keys}}) \text{ is defined} \iff \exists S : (P, \text{Read}, S) \in \Gamma_{\text{keys}}$$

**Dynamic Realization**

At runtime, key upgrade follows the release-and-reacquire protocol defined in §13.7.2. The compile-time `Upgrade` operation reflects the intended final state; the runtime achieves this state through the five-step sequence.

**Panic Release Semantics**

When a panic occurs in scope $S$:

$$\text{PanicRelease}(S, \Gamma_{\text{keys}}) = \Gamma_{\text{keys}} \setminus \{(P, M, S') : S' \leq_{\text{nest}} S\}$$

where $S' \leq_{\text{nest}} S$ indicates $S'$ is $S$ or any scope nested within $S$.

All keys held by the panicking scope and its nested scopes are released atomically 
before panic unwinding proceeds. Keys held by enclosing scopes are unaffected.

##### Static Semantics

**Compile-Time Key Tracking**

The compiler MUST track key state at each program point. For each `shared` path access, the compiler:

1. Computes the key path (per §13.4)
2. Determines the required mode (per §13.1.2)
3. Checks for coverage by an existing key
4. Adds the key to $\Gamma_{\text{keys}}$ if not covered
5. Marks the key for release at scope exit

**Covered Predicate**

An access to path $Q$ requiring mode $M_Q$ is **covered** by the current key state if there exists a held key whose path is a prefix of $Q$ and whose mode is sufficient:

$$\text{Covered}(Q, M_Q, \Gamma_{\text{keys}}) \iff \exists (P, M_P, S) \in \Gamma_{\text{keys}} : \text{Prefix}(P, Q) \land \text{ModeSufficient}(M_P, M_Q)$$

Mode Sufficiency is defined in §13.2 (Formal Operation Definitions). The truth table is:

| Held Mode | Required Mode | Sufficient?           |
| :-------- | :------------ | :-------------------- |
| Write     | Write         | Yes                   |
| Write     | Read          | Yes                   |
| Read      | Read          | Yes                   |
| Read      | Write         | No (upgrade required) |

**Acquisition Rules**

**(K-Acquire-New)**

If an access requires a key and no covering key exists, a new key is acquired:

$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    M = \text{RequiredMode}(P) \quad
    \neg\text{Covered}(P, M, \Gamma_{\text{keys}}) \quad
    S = \text{CurrentScope}
}{
    \Gamma'_{\text{keys}} = \Gamma_{\text{keys}} \cup \{(P, M, S)\}
}$$

**(K-Acquire-Covered)**

If an access is covered by an existing key with sufficient mode, no acquisition occurs:

$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    M = \text{RequiredMode}(P) \quad
    \text{Covered}(P, M, \Gamma_{\text{keys}})
}{
    \Gamma'_{\text{keys}} = \Gamma_{\text{keys}}
}$$

**(K-Acquire-Upgrade)**

If an access is covered by an existing key but requires a stronger mode, the covering key is upgraded:

$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    \text{RequiredMode}(P) = \text{Write} \quad
    \exists (Q, \text{Read}, S) \in \Gamma_{\text{keys}} : \text{Prefix}(Q, P)
}{
    \text{Upgrade}(Q, \Gamma_{\text{keys}})
}$$

**Release Rules**

**(K-Release-Scope)**

When a scope $S$ exits, all keys with that scope are released:

$$\frac{
    \text{ScopeExit}(S)
}{
    \Gamma'_{\text{keys}} = \Gamma_{\text{keys}} \setminus \{(P, M, S') : S' = S\}
}$$

**(K-Release-Order)**

Keys acquired within a scope are released in reverse acquisition order (LIFO). This mirrors the destruction order for RAII bindings.

**Evaluation Order**

Subexpressions MUST be evaluated **left-to-right, depth-first**. Key acquisition follows evaluation order. This order is Defined Behavior.

**(K-Eval-Order)** For expression $e_1 \oplus e_2$:

$$\text{eval}(e_1 \oplus e_2) \longrightarrow \text{eval}(e_1);\ \text{eval}(e_2);\ \text{apply}(\oplus)$$

**Destructuring Patterns**

When a `shared` value is destructured via pattern matching, a single key is acquired to the root of the matched expression:

```cursive
let (a, b) = shared_pair   // Key to shared_pair (Read mode)
```

The key is held for the duration of pattern evaluation and binding initialization. All bindings introduced by the pattern receive copies/moves of the destructured values; they do not retain key requirements.

**(K-Destructure)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ (T_1, \ldots, T_n) \quad
    \texttt{let}\ (p_1, \ldots, p_n) = e
}{
    \text{KeyPath} = \text{Root}(e) \quad
    \text{KeyMode} = \text{Read}
}$$

**Pattern Destructuring Key Behavior:**

| Pattern                       | Expression             | Key Path                   | Mode     |
| :---------------------------- | :--------------------- | :------------------------- | :------- |
| `let (a, b) = tuple`          | `tuple: shared (T, U)` | `tuple`                    | Read     |
| `let Point { x, y } = point`  | `point: shared Point`  | `point`                    | Read     |
| `match shared_enum { ... }`   | scrutinee              | Root of scrutinee          | Read     |
| `let (a, b) = (expr1, expr2)` | tuple literal          | Each expression separately | Per-expr |

> **Note:** When the RHS is a tuple literal (not a binding), each component is a separate expression and keys are acquired per component.

##### Dynamic Semantics

**Runtime Realization (When Required)**

If static safety cannot be proven for a key acquisition, the implementation MUST emit runtime synchronization that:

1. Blocks if an incompatible key is held by another task
2. Proceeds once compatibility is established
3. Releases the key when the scope exits

**Release Guarantee**

Keys MUST be released (logically, and at runtime if synchronized) when their scope exits, regardless of exit mechanism:

| Exit Mechanism    | Keys Released? |
| :---------------- | :------------- |
| Normal completion | Yes            |
| `return`          | Yes            |
| `break`           | Yes            |
| `continue`        | Yes            |
| Panic propagation | Yes            |
| Task cancellation | Yes            |

**Panic Release Semantics**

If a panic occurs while keys are held:

1. All keys held by the panicking task MUST be released before unwinding propagates to the caller.
2. If panic occurs during key acquisition, any partially-acquired keys MUST be released.
3. If panic occurs during mode upgrade—whether while blocked waiting for conflicting keys to be released or during the atomic state transition—the key MUST be released entirely (not left in original mode).
4. Key release during panic MUST occur **before** any `Drop::drop` calls for bindings in the same scope.

**Upgrade Panic Clarification**

"During mode upgrade" encompasses:
- The period while the task is blocked waiting for conflicting keys to be released
- The atomic state transition from Read to Write mode

In either case, the original Read key (if any) is released, and no Write key is acquired.

**Defer Interaction**

Keys acquired within a `defer` block are scoped to that block:

```cursive
{
    defer { player.health = 100 }   // Key acquired when defer executes
    // ... other code ...
}  // defer executes here: key acquired, body runs, key released
```

Keys held when a scope exits are released **before** `defer` blocks execute:

```cursive
{
    player.health = 50              // Key K1 acquired
    defer { player.mana = 100 }     // Defer registered (not yet executed)
}  // K1 released, then defer executes with fresh key K2 for player.mana
```

##### Constraints & Legality

**Scope Definitions**

| Construct                      | Key Scope                  | Release Point          |
| :----------------------------- | :------------------------- | :--------------------- |
| Expression statement (`expr;`) | The statement              | Semicolon              |
| `let`/`var` declaration        | The declaration            | Semicolon              |
| Assignment statement           | The statement              | Semicolon              |
| `if` condition                 | The condition only         | Before entering branch |
| `match` scrutinee              | The scrutinee only         | Before entering arm    |
| `loop` condition               | Each iteration's condition | Before entering body   |
| `#path { ... }` block          | The entire block           | Closing brace          |
| Procedure body                 | Callee's body              | Procedure return       |
| `defer` body                   | The defer block            | Defer completion       |

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `W-KEY-0001` | Warning  | Fine-grained keys in tight loop (performance hint) | Compile-time | Warning   |
| `W-KEY-0002` | Warning  | Redundant key acquisition (already covered)        | Compile-time | Warning   |
| `E-KEY-0006` | Error    | Key acquisition in `defer` escapes to outer scope  | Compile-time | Rejection |

---

### 13.3 Key Acquisition at Procedure Boundaries

##### Definition

When a `shared` value is passed to a procedure, key analysis occurs at the **point of access** within the callee, not at the call site.

##### Static Semantics

**(K-Procedure-Boundary)**

Passing a `shared` value as a procedure argument does not constitute key acquisition. The callee's accesses are analyzed independently:

$$\frac{
    \Gamma \vdash f : (\texttt{shared}\ T) \to R \quad
    \Gamma \vdash v : \texttt{shared}\ T
}{
    \text{call}(f, v) \longrightarrow \text{no key acquisition at call site}
}$$

**Interprocedural Analysis**

The compiler MAY perform interprocedural analysis to prove static safety across procedure boundaries, subject to the following constraints:

**Soundness Requirement:** If interprocedural analysis concludes that no runtime synchronization is needed for a callee's accesses, then concurrent execution of that callee MUST be safe. The analysis MUST be sound—false negatives (missing synchronization) are forbidden.

**Conservatism Requirement:** If interprocedural analysis cannot prove safety, the implementation MUST emit runtime synchronization for the callee's accesses.

**Separate Compilation:** For procedures in separate compilation units whose source is not available:
1. The implementation MUST assume the callee may access **any** path reachable from its `shared` parameters
2. The implementation MUST assume the callee requires **Write** access unless the parameter is declared `const`

**Inlining Interaction:** If a procedure is inlined, the inlined body MUST be analyzed as if its statements were written at the call site, with full visibility of the caller's key context.

##### Dynamic Semantics

**Fine-Grained Parallelism**

When interprocedural analysis proves that callees access disjoint paths, the implementation MAY execute those callees concurrently without synchronization:
```cursive
procedure increment_health(p: shared Player) {
    p.health += 1              // Key analysis: p.health (Write)
}

procedure increment_mana(p: shared Player) {
    p.mana += 1                // Key analysis: p.mana (Write)
}

parallel {
    spawn { increment_health(player) }
    spawn { increment_mana(player) }
}
// Compiler proves disjoint paths: both may execute without synchronization
```

**Callee-Determined Granularity**

The callee determines key granularity through its access patterns. A callee MAY use explicit `#` blocks to coarsen granularity or rely on implicit fine-grained key acquisition:
```cursive
procedure update_all(p: shared Player) {
    #p {
        p.health = 100
        p.mana = 100
    }
}
// Callee chooses coarse granularity (entire player)

procedure update_selective(p: shared Player) {
    p.health = 100             // Fine-grained: p.health
    p.mana = 100               // Fine-grained: p.mana
}
// Callee chooses fine granularity (individual fields)
```

##### Constraints & Legality

**Reentrancy**

If a caller holds a key covering the callee's access paths, the callee's accesses are covered (reentrant):

```cursive
procedure process(p: shared Player) {
    #p {
        helper(p)              // Callee's accesses covered by caller's key
    }
}

procedure helper(p: shared Player) {
    p.health += 1              // Covered by caller's key to p
}
```

**(K-Reentrant)**
$$\frac{
    \text{Held}(P, M, \Gamma_{\text{keys}}) \quad
    \text{Prefix}(P, Q) \quad
    \text{CalleeAccesses}(Q)
}{
    \text{CalleeCovered}(Q)
}$$

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect  |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :------ |
| `W-KEY-0005` | Warning  | Callee access pattern unknown; assuming full access | Compile-time | Warning |

#### 13.3.1 Witness Types and Shared Permission

##### Definition

A `shared witness Form` type permits polymorphic read access to shared data. 
Because dynamic dispatch erases the concrete type, only forms with exclusively read-only methods (`~` receiver) are compatible with `shared` permission.

##### Static Semantics

**(K-Witness-Shared-WF)**
$$\frac{
    \forall m \in \text{AllMethods}(Tr) : m.\text{receiver} = \texttt{\sim}
}{
    \Gamma \vdash \texttt{shared witness } Tr\ \text{wf}
}$$

where $\text{AllMethods}(Tr)$ includes inherited methods from superforms.

When invoking a method on `shared witness Form`:

**(K-Witness-Call)**
$$\frac{
    \Gamma \vdash e : \texttt{shared witness } Tr \quad
    m \in \text{Methods}(Tr)
}{
    \text{KeyPath}(e.m(\ldots)) = \text{Root}(e) \quad
    \text{KeyMode} = \text{Read}
}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-KEY-0083` | Error    | `shared witness Form` where Form has `~%` method | Compile-time | Rejection |

> **Note:** To mutate through a polymorphic interface, use concrete types or 
> restructure to pass the witness with `unique` permission and downgrade within 
> a controlled scope.

#### 13.3.2 Modal Type Transitions

##### Definition

A state transition modifies the discriminant of a modal value, which constitutes mutation. When a transition is invoked on a `shared` modal value, a Write key MUST be acquired.

##### Static Semantics

**(K-Modal-Transition)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ M@S_1 \quad
    t : (M@S_1) \to M@S_2 \in \text{Transitions}(M)
}{
    \text{KeyPath}(e.t(\ldots)) = \text{Root}(e) \quad
    \text{KeyMode} = \text{Write}
}$$

State-specific field access follows standard rules:
- Reading a payload field requires a Read key
- Writing a payload field requires a Write key
- The key path is the modal value's root (not the individual field)

> **Rationale:** A transition may change which payload fields exist. Acquiring a key to the root ensures exclusive access during the structural change.

#### 13.3.3 Closure Capture of Shared Bindings

##### Definition

Closures capturing `shared` bindings are subject to **escape-based classification**. 
The analysis and type requirements differ based on whether the closure escapes its 
defining scope.

**Classification**

A closure is **local** if it appears in one of the following contexts:
1. Argument position of a procedure or method call
2. Body of a `spawn` expression
3. Body of a `dispatch` expression
4. Operand of immediate invocation

A closure is **escaping** if it is:
1. Bound to a `let` or `var` binding
2. Returned from a procedure
3. Stored in a data structure field

**Object Identity**

The **identity** of a reference $r$, written $\text{id}(r)$, is a unique value representing the storage location to which $r$ refers at runtime.

**Formal Definition**

$$\text{id} : \text{Reference} \to \text{Identity}$$

Two references have equal identity if and only if they refer to the same storage location:

$$\text{id}(r_1) = \text{id}(r_2) \iff r_1 \text{ and } r_2 \text{ refer to the same storage location}$$

**Properties**

| Property      | Guarantee                                                                      |
| :------------ | :----------------------------------------------------------------------------- |
| Uniqueness    | Distinct storage locations produce distinct identity values                    |
| Stability     | The identity of a reference remains constant for the reference's lifetime      |
| Comparability | Identity values support equality comparison                                    |
| Non-forgeable | Identity values cannot be constructed except by taking identity of a reference |

**Usage in Key Analysis**

Identity values serve as key path roots when the original binding name is not available—specifically, for:

1. Dereferenced pointers: `(*ptr).field` uses `id(*ptr)` as root
2. Escaping closures: captured references use their identity as key roots

**Shared Dependency Type Annotation**

An escaping closure that captures `shared` bindings carries a **shared dependency set** in its type. This annotation enables key analysis at invocation sites where the original binding names are not in scope.

**Grammar**

```ebnf
closure_type        ::= "|" param_list "|" "->" return_type shared_deps?

shared_deps         ::= "[" "shared" ":" dep_list "]"

dep_list            ::= "{" dep_entry ("," dep_entry)* "}"

dep_entry           ::= IDENTIFIER ":" type
```

**Type Notation**

The type of an escaping closure with shared dependencies is written:

$$|\vec{T}| \to R\ [\texttt{shared}: \{x_1 : T_1, \ldots, x_n : T_n\}]$$

where:
- $|\vec{T}|$ is the parameter type list
- $R$ is the return type
- $\{x_1 : T_1, \ldots, x_n : T_n\}$ is the shared dependency set

**Well-Formedness**

**(WF-Closure-SharedDeps)**
$$\frac{
    \forall i \in 1..n : \Gamma \vdash T_i\ \text{wf} \quad
    \forall i \in 1..n : x_i \text{ is a valid identifier}
}{
    \Gamma \vdash |\vec{T}| \to R\ [\texttt{shared}: \{x_1 : T_1, \ldots, x_n : T_n\}]\ \text{wf}
}$$

**Subtyping**

A closure type with shared dependencies is a subtype of the corresponding closure type without dependencies when the dependency set is empty:

$$|\vec{T}| \to R\ [\texttt{shared}: \{\}] \equiv |\vec{T}| \to R$$

Closure types with non-empty shared dependencies are invariant in their dependency sets—no subtyping relationship exists between closures with different dependencies.

##### Static Semantics

**Local Closure Analysis**

For local closures, key analysis uses the **defining scope's paths**:

**(K-Closure-Local)**
$$\frac{
    C \text{ is local} \quad
    x \in \text{SharedCaptures}(C) \quad
    x : \texttt{shared}\ T \in \Gamma_{\text{def}}
}{
    \text{KeyPath}(C, x.p) = x.p
}$$

Key acquisition occurs when the closure is invoked. The defining scope's binding 
serves as the key path root. Fine-grained path analysis is preserved.

**Escaping Closure Type Requirement**

For escaping closures, the type MUST include a **shared dependency set**:

**(K-Closure-Escape-Type)**
$$\frac{
    C \text{ is escaping} \quad
    \text{SharedCaptures}(C) = \{x_1, \ldots, x_n\}
}{
    \text{Type}(C) = |\vec{T}| \to R\ [\texttt{shared}: \{x_1 : T_1, \ldots, x_n : T_n\}]
}$$

**Escaping Closure Key Analysis**

For escaping closures, key analysis uses **object identity**:

**(K-Closure-Escape-Keys)**
$$\frac{
    C : |\vec{T}| \to R\ [\texttt{shared}: \{x : T\}] \quad
    \text{Access}(x.p, M) \in C.\text{body}
}{
    \text{KeyPath}(C, x.p) = \text{id}(C.x).p \quad \text{KeyMode} = M
}$$

At invocation, keys are acquired using the captured reference's identity as the 
root, preserving fine-grained path analysis.

##### Dynamic Semantics

**Local Closure Invocation**

1. Analyze accessed paths using defining scope's bindings
2. Acquire keys for accessed paths
3. Execute closure body
4. Release keys

**Escaping Closure Invocation**

1. For each shared dependency $(x, T)$ in the closure's type:
   - Let $r$ be the captured reference to $x$
   - Acquire keys for paths in $\text{AccessedPaths}(C, x)$ using $\text{id}(r)$ as root
2. Execute closure body
3. Release keys

##### Constraints & Legality

**Negative Constraints**

1. An escaping closure capturing `shared` bindings MUST have an explicitly annotated 
   type or be assigned to a binding with inferred dependency type.
2. An escaping closure MUST NOT outlive any of its captured `shared` bindings.

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0085` | Error    | Escaping closure with `shared` capture lacks type annotation | Compile-time | Rejection |
| `E-KEY-0086` | Error    | Escaping closure outlives captured `shared` binding          | Compile-time | Rejection |
| `W-KEY-0009` | Warning  | Closure captures `shared` data                               | Compile-time | Warning   |

> **Note:** The warning `W-KEY-0009` is advisory. It alerts programmers to shared 
> captures that may affect concurrent behavior, but does not indicate an error.

---

### 13.4 The `#` Key Block

##### Definition

The **`#` key block** is the explicit key acquisition construct. It acquires a key to one or more paths at block entry and holds those keys until block exit.

The `#` operator serves three purposes:

1. **Key coarsening:** Acquire a key to a path prefix rather than the full access path, reducing key granularity for atomicity or performance.
2. **Consistent snapshots:** Hold a Read key across multiple reads to ensure atomicity.
3. **Read-modify-write enablement:** With the `write` modifier, permit read-then-write patterns that would otherwise be prohibited.

**Mode Semantics**

Following the language principle that immutability is the default and mutability requires explicit opt-in:

| Block Form        | Key Mode | Mutation Permitted |
| :---------------- | :------- | :----------------- |
| `#path { }`       | Read     | No                 |
| `#path write { }` | Write    | Yes                |

> **Note:** This mirrors other Cursive constructs where immutability is the default:
> - Bindings: `let` (const) vs `var` (mutable)
> - Receivers: `~` (const) vs `~!`, `~%` (mutable)
> - Key blocks: `#path { }` (Read) vs `#path write { }` (Write)

##### Syntax & Declaration

**Grammar**

```ebnf
key_block       ::= "#" path_list mode_modifier* block

path_list       ::= path_expr ("," path_expr)*

mode_modifier   ::= "write" | "upgrade" | "ordered" | "speculative"
```

**Modifier Constraints:**
- `upgrade` MUST be followed by `write` (either immediately or with `ordered` between)
- `ordered` MAY combine with any other modifiers
- `speculative` MUST be followed by `write`; speculative read-only blocks are ill-formed
- `speculative` MUST NOT combine with `upgrade`
- Modifier order is not significant: `ordered write` ≡ `write ordered`

The `path_expr`, `key_marker`, and `index_suffix` productions are defined in §13.1.

The `upgrade` modifier is used for nested mode escalation; see §13.7.2 for semantics.

**Desugaring**

The `#` block without explicit mode modifier desugars to Read mode. The `write` modifier specifies Write mode:

$$\texttt{\#}P\ \{\ B\ \} \quad \equiv \quad \texttt{\#}P\ \texttt{with mode=Read}\ \{\ B\ \}$$

$$\texttt{\#}P\ \texttt{write}\ \{\ B\ \} \quad \equiv \quad \texttt{\#}P\ \texttt{with mode=Write}\ \{\ B\ \}$$

For multiple paths, mode applies uniformly:

$$\texttt{\#}P_1, P_2\ \texttt{write}\ \{\ B\ \} \quad \equiv \quad \texttt{\#}P_1\ \texttt{write},\ P_2\ \texttt{write}\ \{\ B\ \}$$

**Read Mode (Default)**

When no modifier is specified, the `#` block acquires Read keys. The body MUST NOT contain writes to keyed paths:

```cursive
#player {
    let h = player.health
    let m = player.mana
    // Consistent snapshot; concurrent readers permitted
}
```

**Write Mode (Explicit)**

The `write` modifier acquires Write keys, enabling mutation and read-modify-write patterns:

```cursive
#player write {
    let h = player.health
    player.health = h + heal_amount    // Mutation permitted
}
```

**Multiple Paths**

Both modes support multiple paths. Keys are acquired in canonical order (§13.7):

```cursive
#player, enemy {
    let diff = player.health - enemy.health
}

#player, enemy write {
    player.health -= enemy.damage
    enemy.health -= player.damage
}
```

**Ordered Mode**

The `ordered` modifier forces runtime evaluation and comparison of dynamic indices, establishing a deterministic acquisition order across all tasks. This is necessary when multiple tasks may execute `#` blocks with incomparable dynamic indices on overlapping paths.

```cursive
// Without ordered: acquisition order is USB when i and j cannot be statically compared
#arr[i], arr[j] { ... }           // HAZARD: potential cross-task deadlock

// With ordered: indices evaluated at runtime, keys acquired in sorted order
#arr[i], arr[j] ordered {
    let a = arr[i]
    let b = arr[j]
}

// Ordered with write mode
#arr[i], arr[j] ordered write {
    arr[i] = arr[j] + 1
}

// Ordered with upgrade (from enclosing Read block)
#arr[i], arr[j] ordered upgrade write {
    // Release-and-reacquire with deterministic ordering
}
```

For paths with the same array base and dynamic indices:
- Indices are evaluated to values $v_1, \ldots, v_n$
- Paths are acquired in ascending order of index value
- If two indices have equal values, the paths are identical (single acquisition)

> **Note:** The `ordered` modifier incurs runtime overhead (index evaluation and comparison). Use it only when cross-task deadlock prevention is required without sacrificing fine-grained parallelism. When indices are statically comparable, canonical ordering applies automatically and `ordered` is unnecessary.

**Speculative Mode**

The `speculative write` modifier requests optimistic concurrency semantics. Instead of blocking until an exclusive key is available, the block executes immediately and attempts an atomic commit at block exit. If the commit fails due to concurrent modification, the block re-executes.

```cursive
#counter speculative write {
    let v = counter
    counter = v + 1
}
```

Speculative blocks are suitable for contended paths where blocking would cause unacceptable latency and the block body is cheap to re-execute. See §13.7.3 for complete semantics.

**Inline Coarsening**

The `#` marker MAY appear inline within a path expression to specify the key granularity. Inline coarsening acquires keys in the mode determined by syntactic context (read context → Read key; write context → Write key):

```cursive
let x = #player.stats.health    // Read key to `player`
#player.stats.health = 100      // Write key to `player`
```

**Desugaring: Compound Assignment**

Compound assignment operators desugar to `#` blocks with `write` modifier:

$$P\ \oplus\texttt{=}\ e \quad \Longrightarrow \quad \#P\ \texttt{write}\ \{\ \texttt{let}\ v = P;\ P = v \oplus e\ \}$$

| Source                    | Desugared Form                                                               |
| :------------------------ | :--------------------------------------------------------------------------- |
| `x += 1`                  | `#x write { let v = x; x = v + 1 }`                                          |
| `player.health -= damage` | `#player.health write { let v = player.health; player.health = v - damage }` |

**Desugaring: Increment and Decrement**

$$\texttt{++}P \quad \Longrightarrow \quad \#P\ \texttt{write}\ \{\ \texttt{let}\ v = P;\ P = v + 1;\ v + 1\ \}$$
$$P\texttt{++} \quad \Longrightarrow \quad \#P\ \texttt{write}\ \{\ \texttt{let}\ v = P;\ P = v + 1;\ v\ \}$$

| Source | Desugared Form                             | Result Value |
| :----- | :----------------------------------------- | :----------- |
| `++x`  | `#x write { let v = x; x = v + 1; v + 1 }` | New value    |
| `x++`  | `#x write { let v = x; x = v + 1; v }`     | Old value    |
| `--x`  | `#x write { let v = x; x = v - 1; v - 1 }` | New value    |
| `x--`  | `#x write { let v = x; x = v - 1; v }`     | Old value    |

**Desugaring Phase**

Compound assignment desugaring (as defined above) occurs during the Parsing phase (Translation Phase 1, per §2.12), before key analysis. Key analysis operates on the desugared form. Consequently, the expression `p += e` is analyzed as `#p write { let v = p; p = v + e }`, and a covering Write key is always present for the read-then-write pattern.

##### Static Semantics

**Block Key Acquisition Rule**

**(K-Block-Acquire)**
$$\frac{
    \Gamma \vdash P_1, \ldots, P_m : \texttt{shared}\ T_i \quad
    M = \text{BlockMode}(B) \quad
    (Q_1, \ldots, Q_m) = \text{CanonicalSort}(P_1, \ldots, P_m) \quad
    S = \text{NewScope}
}{
    \Gamma_{\text{keys}}' = \Gamma_{\text{keys}} \cup \{(Q_i, M, S) : i \in 1..m\}
}$$

where $\text{BlockMode}(B) = \text{Write}$ if `write` modifier is present, otherwise $\text{Read}$.

**Read Mode Restriction**

A `#` block without `write` modifier MUST NOT contain write accesses to keyed paths:

**(K-Read-Block-No-Write)**
$$\frac{
    \text{BlockMode}(B) = \text{Read} \quad
    P \in \text{KeyedPaths}(B) \quad
    \text{WriteOf}(P) \in \text{Body}(B)
}{
    \text{Emit}(\texttt{E-KEY-0070})
}$$

**Read-Then-Write Permission**

The read-then-write prohibition (`E-KEY-0060`) does not apply when a covering Write key is statically held:

**(K-RMW-Permitted)**
$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    \text{ReadThenWrite}(P, S) \quad
    \exists (Q, \text{Write}, S') \in \Gamma_{\text{keys}} : \text{Prefix}(Q, P)
}{
    \text{Permitted}
}$$

**Inline Coarsening Rule**

**(K-Coarsen-Inline)**
$$\frac{
    P = p_1.\ldots.p_{k-1}.\#p_k.\ldots.p_n \quad
    \Gamma \vdash P : \texttt{shared}\ T \quad
    M = \text{RequiredMode}(P)
}{
    \text{AcquireKey}(p_1.\ldots.p_k, M, \Gamma_{\text{keys}})
}$$

##### Dynamic Semantics

**Execution Order**

When a `#` block executes:

1. Acquire keys (mode per block specification) to all listed paths in canonical order
2. Execute the block body
3. Release all keys in reverse acquisition order

**Concurrent Access**

| Block A        | Block B        | Same/Overlapping Path | Result                          |
| :------------- | :------------- | :-------------------- | :------------------------------ |
| `#p { }`       | `#p { }`       | Yes                   | Both proceed concurrently       |
| `#p { }`       | `#p write { }` | Yes                   | One blocks until other releases |
| `#p write { }` | `#p write { }` | Yes                   | One blocks until other releases |

**Runtime Implementation**

When runtime synchronization is required:

| Block Mode | Typical Implementation                         |
| :--------- | :--------------------------------------------- |
| Read       | Shared/reader lock; atomic load for primitives |
| Write      | Exclusive/writer lock; CAS for primitives      |

The implementation strategy is Implementation-Defined Behavior (IDB).

##### Constraints & Legality

**Negative Constraints**

1. A `#` block without `write` modifier MUST NOT contain write operations to keyed paths.
2. A program MUST NOT place `#` immediately before a method name.
3. A program MUST NOT use `#` block syntax with a non-`shared` path.

**Method Receiver Coarsening**

To coarsen the key path when accessing `shared` data through a method call, place the `#` marker on the receiver expression, not on the method name:

| Expression                | Key Path       | Validity               |
| :------------------------ | :------------- | :--------------------- |
| `#player.get_health()`    | `player`       | Valid                  |
| `player.#get_health()`    | —              | Invalid (`E-KEY-0020`) |
| `#player.stats.compute()` | `player`       | Valid                  |
| `player.#stats.compute()` | `player.stats` | Valid                  |

The `#` marker coarsens from that point in the path; subsequent segments (including method calls) are covered by the coarsened key.

**Diagnostic Table**

| Code         | Severity | Condition                                                  | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0014` | Error    | `ordered` modifier on paths with different array bases     | Compile-time | Rejection |
| `E-KEY-0020` | Error    | `#` immediately before method name                         | Compile-time | Rejection |
| `E-KEY-0031` | Error    | `#` block path not in scope                                | Compile-time | Rejection |
| `E-KEY-0032` | Error    | `#` block path is not `shared`                             | Compile-time | Rejection |
| `E-KEY-0070` | Error    | Write operation in `#` block without `write` modifier      | Compile-time | Rejection |
| `W-KEY-0003` | Warning  | `#` redundant (matches type boundary)                      | Compile-time | Warning   |
| `W-KEY-0013` | Warning  | `ordered` modifier used with statically-comparable indices | Compile-time | Warning   |

---

### 13.5 Type-Level Key Boundaries

##### Definition

A **type-level key boundary** is a permanent granularity constraint declared on a record field. All access paths through that field use the field as the minimum key granularity.

##### Syntax & Declaration

```ebnf
field_decl      ::= "#"? visibility? IDENTIFIER ":" type
```

**Example:**

```cursive
record Player {
    health: i32,              // Fine-grained
    mana: i32,                // Fine-grained
    #inventory: [Item; 20],   // Boundary: always keys to player.inventory
}
```

##### Static Semantics

**Boundary Propagation Rule**

**(K-Type-Boundary)**

Let $R$ be a record type with field $f$ declared as `#f: U`. For any path $P = r.f.q_1.\ldots.q_n$ where $r : R$:

$$\text{KeyPath}(P) = r.f$$

regardless of the suffix $q_1.\ldots.q_n$.

**Formal Rule:**

$$\frac{
    \Gamma \vdash r : R \quad
    R.\text{fields} \ni (\#f : U) \quad
    P = r.f.q_1.\ldots.q_n
}{
    \text{KeyPath}(P) = r.f
}$$

**Boundary Enforcement Examples:**

| Expression                 | Key Path           | Reason        |
| :------------------------- | :----------------- | :------------ |
| `player.health`            | `player.health`    | No boundary   |
| `player.inventory[5]`      | `player.inventory` | Type boundary |
| `player.inventory[5].name` | `player.inventory` | Type boundary |

**Coarsening Beyond Boundaries**

Expression-level `#` MAY coarsen beyond a type boundary:

| Expression             | Key Path                       |
| :--------------------- | :----------------------------- |
| `#player.inventory[5]` | `player`                       |
| `player.#inventory[5]` | `player.inventory` (redundant) |

**(K-Boundary-Override)**

If path $P$ contains both explicit `#` at position $k$ and type-level boundary at position $b$:

$$\text{KeyPath}(P) = p_1.\ldots.p_{\min(k, b)}$$

The explicit `#` can coarsen further but cannot refine past a type boundary.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-KEY-0033` | Error    | `#` on field of non-record type                  | Compile-time | Rejection |
| `W-KEY-0003` | Warning  | Expression `#` redundant (matches type boundary) | Compile-time | Warning   |

---

### 13.6 Static Index Resolution and Disjointness Proofs

##### Definition

An index expression's **static resolvability** and the availability of **disjointness proofs** determine whether the compiler can reason about path disjointness at compile time. This section defines the mechanisms by which the compiler proves that two dynamic indices refer to distinct array elements within the **same statement**.

When multiple dynamic indices access the same array within a single statement, the compiler MUST either prove they are disjoint or reject the program. This is because same-statement accesses cannot be serialized by runtime key acquisition—they occur as part of one atomic key-holding scope. Cross-task access to `shared` data (where runtime synchronization may apply) is addressed in §13.9.

**Formal Definition**

An index expression is **statically resolvable** if and only if it is:

1. An integer literal
2. A reference to a `const` binding with statically resolvable initializer
3. A generic const parameter
4. An arithmetic expression (`+`, `-`, `*`, `/`, `%`) where all operands are statically resolvable

All other index expressions are **dynamic**.

**Disjointness Classification**

Two index expressions $e_1$ and $e_2$ accessing the same array are classified as:

| Classification          | Condition                                                                  | Compiler Action              |
| :---------------------- | :------------------------------------------------------------------------- | :--------------------------- |
| Statically Disjoint     | Both statically resolvable with different values                           | Fine-grained keys permitted  |
| Provably Disjoint       | Dynamic, but disjointness provable via contracts, refinements, or analysis | Fine-grained keys permitted  |
| Potentially Overlapping | Disjointness cannot be proven                                              | **Rejection** (`E-KEY-0010`) |

##### Static Semantics

**Static Disjointness**

| Expression             | Classification        | Reason                      |
| :--------------------- | :-------------------- | :-------------------------- |
| `arr[0]`               | Statically resolvable | Integer literal             |
| `arr[BUFFER_SIZE - 1]` | Statically resolvable | Const binding in arithmetic |
| `arr[N]` (generic)     | Statically resolvable | Generic const parameter     |
| `arr[i]`               | Dynamic               | Runtime variable            |
| `arr[get_index()]`     | Dynamic               | Function call               |

Two statically resolvable indices are disjoint if their compile-time values differ:

$$\text{StaticDisjoint}(a[e_1], a[e_2]) \iff \text{StaticValue}(e_1) \neq \text{StaticValue}(e_2)$$

**Provable Disjointness for Dynamic Indices**

Two dynamic index expressions $e_1$ and $e_2$ are **provably disjoint** ($\text{ProvablyDisjoint}(e_1, e_2)$) if any of the following conditions hold:

$$\text{ProvablyDisjoint}(e_1, e_2) \iff \text{StaticDisjoint}(e_1, e_2) \lor \text{DynamicDisjoint}(e_1, e_2)$$

where $\text{DynamicDisjoint}(e_1, e_2)$ is established by one or more of the following proof mechanisms:

---

#### 13.6.1 Proof Mechanism: Verification Facts from Control Flow

A **Verification Fact** (§10.5) established by control flow analysis proves disjointness when it establishes $e_1 \neq e_2$:

**(K-Disjoint-Fact)**
$$\frac{
    F(e_1 \neq e_2, L) \in \Gamma_{\text{facts}} \quad
    L \text{ dom } S \quad
    \text{Access}(a[e_1], a[e_2]) \text{ at } S
}{
    \text{ProvablyDisjoint}(e_1, e_2)
}$$

Verification Facts arise from conditional guards:

```cursive
procedure safe_swap(arr: shared [i32; N], i: usize, j: usize) {
    if i != j {
        // Fact F(i != j, _) generated at then-branch entry
        #arr {
            let tmp = arr[i]
            arr[i] = arr[j]     // OK: i != j proven by control flow
            arr[j] = tmp
        }
    }
}
```

---

#### 13.6.2 Proof Mechanism: Contract-Based Disjointness

Procedure contracts (§10.2) establish Verification Facts at procedure entry that can prove index disjointness. When a precondition asserts $e_1 \neq e_2$, the compiler generates a Verification Fact that dominates the entire procedure body.

**(K-Disjoint-Contract)**
$$\frac{
    \text{Procedure } P \text{ has precondition } e_1 \neq e_2 \quad
    \text{Access}(a[e_1], a[e_2]) \text{ in body of } P
}{
    \text{ProvablyDisjoint}(e_1, e_2)
}$$

**Example — Explicit Precondition:**

```cursive
procedure swap(arr: shared [i32; N], i: usize, j: usize)
    |= i != j
{
    // Precondition i != j generates Verification Fact at procedure entry
    // Fact dominates all statements in body
    let tmp = arr[i]
    arr[i] = arr[j]         // OK: disjointness proven via contract
    arr[j] = tmp
}
```

**Example — Inline Parameter Constraint (Refinement Type):**

```cursive
procedure swap(
    arr: shared [i32; N], 
    i: usize, 
    j: usize where { j != i }   // Inline constraint: type refinement (§7.3)
) {
    // Constraint j != i desugars to precondition, generates Verification Fact
    let tmp = arr[i]
    arr[i] = arr[j]         // OK: disjointness proven via refinement
    arr[j] = tmp
}
```

Per §10.1.3, the inline constraint `j: usize where { j != i }` is semantically equivalent to a procedure-level precondition `|= j != i`. Both generate the same Verification Fact.

---

#### 13.6.3 Proof Mechanism: Refinement Type Propagation

When an index binding has a refinement type (§7.3) that constrains its relationship to another index, that constraint propagates as a Verification Fact throughout the binding's scope.

**(K-Disjoint-Refine)**
$$\frac{
    \Gamma \vdash j : \texttt{usize where } \{ j \neq i \} \quad
    \text{Access}(a[i], a[j]) \text{ in scope}
}{
    \text{ProvablyDisjoint}(i, j)
}$$

**Example — Refinement from Type Alias:**

```cursive
type DistinctIndex<other: usize> = usize where { self != other }

procedure process(arr: shared [i32; N], i: usize, j: DistinctIndex<i>) {
    // j has type usize where { self != i }
    // Refinement predicate establishes Verification Fact
    arr[i] = arr[j] * 2     // OK: disjointness proven via refinement type
}
```

**Example — Refinement from Runtime Check:**

```cursive
procedure maybe_swap(arr: shared [i32; N], i: usize, j: usize) {
    // Runtime check establishes refinement
    if i == j { return }
    
    // After check: j effectively has type usize where { j != i }
    // Verification Fact F(i != j, _) is active
    let tmp = arr[i]
    arr[i] = arr[j]         // OK: disjointness proven
    arr[j] = tmp
}
```

---

#### 13.6.4 Proof Mechanism: Algebraic Analysis

The compiler performs algebraic analysis to prove disjointness for index expressions that differ by a statically-known offset:

**(K-Disjoint-Offset)**
$$\frac{
    e_1 = v + c_1 \quad e_2 = v + c_2 \quad c_1, c_2 \text{ statically resolvable} \quad c_1 \neq c_2
}{
    \text{ProvablyDisjoint}(e_1, e_2)
}$$

**Example — Constant Offset:**

```cursive
procedure adjacent_update(arr: shared [i32; N], i: usize)
    |= i + 1 < N
{
    arr[i] = 0
    arr[i + 1] = 1          // OK: i and i+1 are provably distinct
}
```

**Canonical Form Normalization**

Index expressions MUST be normalized according to the Canonical Form Normalization rules defined in §13.1.1 before comparison.

If two normalized expressions share a common subexpression but differ by constant terms, they are provably disjoint.

---

#### 13.6.5 Proof Mechanism: Dispatch Iteration Variables

Within a `dispatch` block (§14.3), accesses indexed by the iteration variable are automatically proven disjoint across iterations:

**(K-Disjoint-Dispatch)**
$$\frac{
    \texttt{dispatch}\ v\ \texttt{in}\ r\ \{\ \ldots\ a[v]\ \ldots\ \}
}{
    \forall v_1, v_2 \in r,\ v_1 \neq v_2 \implies \text{ProvablyDisjoint}(a[v_1], a[v_2])
}$$

```cursive
dispatch i in 0..arr.len() {
    arr[i] *= 2             // Proven disjoint: each iteration touches distinct element
}
```

**Nested Dispatch:**

```cursive
dispatch i in 0..rows {
    dispatch j in 0..cols {
        matrix[i][j] = 0    // Proven disjoint: (i,j) pairs are unique
    }
}
```

**Cross-Iteration Dependency Detection:**

Accesses that reference a **different** iteration's index are rejected:

```cursive
dispatch i in 0..arr.len() - 1 {
    arr[i] = arr[i + 1]     // ERROR E-DISPATCH-0001: cross-iteration dependency
}
```

---

#### 13.6.6 Proof Mechanism: Loop Iteration Variables from Disjoint Ranges

When two index expressions reference iteration variables from nested loops with **non-overlapping ranges**, they are provably disjoint:

**(K-Disjoint-Range)**
$$\frac{
    \texttt{loop}\ v_1\ \texttt{in}\ r_1\ \{ \ldots \} \quad
    \texttt{loop}\ v_2\ \texttt{in}\ r_2\ \{ \ldots \} \quad
    r_1 \cap r_2 = \emptyset
}{
    \text{ProvablyDisjoint}(v_1, v_2)
}$$

**Example:**

```cursive
// Process first half and second half in nested loops
loop i in 0..N/2 {
    loop j in N/2..N {
        // i ∈ [0, N/2) and j ∈ [N/2, N) — ranges are disjoint
        arr[i] = arr[j]     // OK: i and j cannot be equal
    }
}
```

---

#### 13.6.7 Dynamic Index Conflict Detection

If a statement accesses the same array through multiple dynamic indices and disjointness **cannot** be proven by any of the mechanisms above, the implementation MUST emit diagnostic `E-KEY-0010`:

**(K-Dynamic-Index-Conflict)**
$$\frac{
    P_1 = a[e_1] \quad P_2 = a[e_2] \quad
    \text{SameStatement}(P_1, P_2) \quad
    (\text{Dynamic}(e_1) \lor \text{Dynamic}(e_2)) \quad
    \neg\text{ProvablyDisjoint}(e_1, e_2)
}{
    \text{Emit}(\texttt{E-KEY-0010})
}$$

##### Constraints & Legality

**Resolution Strategies**

When `E-KEY-0010` is emitted, the programmer may resolve it by:

1. **Add a precondition:** Use a contract clause to assert disjointness
   ```cursive
   procedure update(arr: shared [i32; N], i: usize, j: usize)
       |= i != j                    // Contract proves disjointness
   { ... }
   ```

2. **Use inline refinement:** Apply a refinement type constraint to a parameter
   ```cursive
   procedure update(arr: shared [i32; N], i: usize, j: usize where { j != i })
   { ... }
   ```

3. **Add a runtime guard:** Insert a conditional that establishes a Verification Fact
   ```cursive
   if i != j {
       // Verification Fact: i != j
       arr[i] = arr[j]              // OK: disjointness proven
   }
   ```

4. **Use explicit coarsening:** Acquire a key to the entire array, serializing access
   ```cursive
   #arr {
       arr[i] = arr[j]              // OK: single key to entire array
   }
   ```

5. **Separate into distinct statements:** Move accesses to separate statements where each acquires its own key
   ```cursive
   let tmp = arr[j]                 // Key acquired and released
   arr[i] = tmp                     // Separate key acquired
   ```

**Diagnostic Table**

| Code         | Severity | Condition                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------ | :----------- | :-------- |
| `E-KEY-0010` | Error    | Potential conflict on dynamic indices | Compile-time | Rejection |

---

##### Cross-Reference Notes

**Related Sections:**

| Concept              | Section | Relationship                                              |
| :------------------- | :------ | :-------------------------------------------------------- |
| Refinement Types     | §7.3    | Provides `where { P }` syntax for type-level constraints  |
| Procedure Contracts  | §10.2   | Provides `\|= P => Q` syntax for preconditions            |
| Inline Constraints   | §10.1.3 | Desugars to preconditions, generates Verification Facts   |
| Verification Facts   | §10.5   | Compiler-internal proof mechanism for predicate validity  |
| Verification Modes   | §10.4   | Determines static vs. dynamic verification of constraints |
| Dispatch Parallelism | §14.3   | Automatic disjointness proof for iteration variables      |
| Path Disjointness    | §13.1.1 | General definition of path disjointness for keys          |

**Proof Mechanism Summary:**

| Mechanism                   | Source                                       | Verification Fact Generated At   |
| :-------------------------- | :------------------------------------------- | :------------------------------- |
| Control Flow Guard          | `if i != j { ... }`                          | Entry of then-branch             |
| Procedure Precondition      | `\|= i != j`                                 | Procedure entry (post-binding)   |
| Inline Parameter Constraint | `j: usize where { j != i }`                  | Procedure entry (post-binding)   |
| Refinement Type             | `j: DistinctIndex<i>`                        | Binding introduction             |
| Algebraic Analysis          | `arr[i]` vs `arr[i + 1]`                     | Point of analysis                |
| Dispatch Iteration          | `dispatch i in r { ... }`                    | Implicit per-iteration guarantee |
| Disjoint Loop Ranges        | Nested loops with $r_1 \cap r_2 = \emptyset$ | Loop entry                       |

---

### 13.7 Deadlock Prevention

##### Definition

Cursive prevents deadlock through:

1. **Scoped keys:** Keys are released at scope exit, limiting hold duration
2. **Canonical ordering:** `#` blocks acquire keys in deterministic order
3. **Await prohibition:** Keys cannot be held across suspension points

##### Static Semantics

**Canonical Order**

A **canonical order** on paths ensures deterministic acquisition order to prevent circular wait conditions.

For paths $P = p_1.\ldots.p_m$ and $Q = q_1.\ldots.q_n$:

$$P <_{\text{canon}} Q \iff \exists k \geq 1 : (\forall i < k,\ p_i =_{\text{seg}} q_i) \land (p_k <_{\text{seg}} q_k \lor (k > m \land m < n))$$

**Segment Ordering ($<_{\text{seg}}$):**

1. **Identifiers:** Lexicographic by UTF-8 byte sequence
2. **Indexed segments:** By index value for statically resolvable indices ($a[i] <_{\text{seg}} a[j] \iff \text{StaticValue}(i) < \text{StaticValue}(j)$)
3. **Bare vs indexed:** Bare identifier precedes any indexed form ($a <_{\text{seg}} a[i]$ for all $i$)

**Dynamic Index Ordering**

If indices $i$ and $j$ in segments $a[i]$ and $a[j]$ cannot be statically compared:

1. If $i$ and $j$ are the same binding: $a[i] =_{\text{seg}} a[i]$ (same segment)
2. Otherwise: ordering is **Unspecified Behavior (USB)**

**Deadlock Safety Under USB Ordering**

When acquisition order is unspecified, deadlock remains impossible within a single task because:
1. A task acquires all keys in a `#` block atomically before executing the block body
2. USB affects only the order of acquisition, not whether acquisition eventually succeeds
3. Deadlock requires circular wait across multiple tasks; a single task cannot deadlock with itself

However, when multiple tasks execute `#` blocks with incomparable dynamic indices on overlapping paths, USB ordering MAY cause different tasks to attempt acquisition in different orders, potentially creating circular wait conditions.

**Mitigation Requirement**

A program SHOULD use explicit coarsening when `#` blocks contain dynamically-indexed paths to the same array where indices cannot be statically compared:
```cursive
// HAZARD: Unspecified ordering across tasks
#arr[i], arr[j] { ... }  // i and j are different dynamic variables

// SAFE: Coarsen to array level
#arr { ... }
```

The implementation MUST emit diagnostic `W-KEY-0011` when this pattern is detected.

**Atomic Key Acquisition**

When a `#` block lists multiple paths, the implementation acquires keys to all listed paths before executing the block body. This acquisition is **atomic** in the following sense:

**Formal Definition**

Let $\text{Paths} = \{P_1, \ldots, P_n\}$ be the paths listed in a `#` block, sorted into canonical order as $Q_1, \ldots, Q_n$. Atomic acquisition proceeds as follows:

1. For $i = 1$ to $n$:
   - Attempt to acquire key $(Q_i, M, S)$
   - If a conflicting key is held by another task, **block** until the conflict is resolved
   - Keys $Q_1, \ldots, Q_{i-1}$ remain held during blocking

2. Once all keys are acquired, execute the block body.

**Properties**

| Property             | Guarantee                                                          |
| :------------------- | :----------------------------------------------------------------- |
| No rollback          | Partially-acquired keys are retained during blocking, not released |
| Eventual acquisition | Each key is eventually acquired (per progress guarantee, §13.1.2)  |
| Order preservation   | Keys are acquired in canonical order                               |
| Body precondition    | Block body executes only after all keys are held                   |

**Deadlock Avoidance**

Canonical ordering ensures that all tasks acquiring the same set of keys do so in the same order, preventing circular wait conditions. A task holding keys $Q_1, \ldots, Q_{i-1}$ while blocking on $Q_i$ cannot be blocked by another task that needs $Q_j$ where $j < i$, because canonical ordering guarantees that task would have acquired $Q_j$ before $Q_i$.

**Single-Task Guarantee**

A single task cannot deadlock with itself. All keys in a `#` block are acquired sequentially by one task; the task either:
1. Acquires all keys and proceeds, or
2. Blocks on a key held by another task (not itself)

**Statement Keys: Evaluation Order**

Keys within a single statement are acquired in **evaluation order** (left-to-right, depth-first). This is Defined Behavior.

**`#` Block Keys: Canonical Order**

Keys in a `#` block MUST be acquired in canonical order, regardless of the order listed:

```cursive
#zebra, alpha, mango {
    // Acquired: alpha, mango, zebra (canonical order)
}
```

**(K-Block-Canonical)**
$$\frac{
    \text{Paths} = \{P_1, \ldots, P_n\} \quad
    (Q_1, \ldots, Q_n) = \text{CanonicalSort}(\text{Paths})
}{
    \text{AcquireSequence} = [Q_1, Q_2, \ldots, Q_n]
}$$

**Nested `#` Block Semantics**

Nested `#` blocks acquire keys independently. The outer block's keys remain held while the inner block executes:

```cursive
#alpha {                    // Acquires: alpha
    // alpha held
    #beta {                 // Acquires: beta (alpha still held)
        // Both alpha and beta held
    }                       // Releases: beta
    // alpha still held
}                           // Releases: alpha
```

**Nested `#` Block on Same or Overlapping Path**

When a nested `#` block specifies a path that overlaps with an outer `#` block's path:

1. **Identical path, same or weaker mode:** The inner block's key request is covered by the outer block's key. No additional acquisition occurs.

2. **Identical path, stronger mode (Read → Write):** If the outer block holds a Read key and the inner block requests Write, the inner block MUST use the `upgrade write` modifier. This triggers release-and-reacquire semantics as defined in §13.7.2. The `upgrade` keyword is required to acknowledge that interleaving may occur between the release of the Read key and acquisition of the Write key.

   Omitting the `upgrade` keyword when escalating mode is a compile-time error (`E-KEY-0012`).

3. **Prefix relationship:** If the inner path is a prefix of the outer path (coarser granularity), the inner block acquires a separate, coarser key. If the outer path is a prefix of the inner path (finer granularity), the inner block's access is covered by the outer key.

**(K-Nested-Same-Path)**
$$\frac{
    \text{Held}(P, M_{\text{outer}}, \Gamma_{\text{keys}}) \quad
    \#P\ M_{\text{inner}}\ \{\ \ldots\ \}
}{
    \begin{cases}
    \text{Covered (no acquisition)} & \text{if } M_{\text{inner}} \leq M_{\text{outer}} \\
    \text{Release-and-Reacquire per §13.7.2} & \text{if } M_{\text{inner}} > M_{\text{outer}} \land \texttt{upgrade} \in \text{modifiers} \\
    \text{Emit}(\texttt{E-KEY-0012}) & \text{if } M_{\text{inner}} > M_{\text{outer}} \land \texttt{upgrade} \notin \text{modifiers}
    \end{cases}
}$$

where $\text{Read} < \text{Write}$.
```cursive
// Example: Outer Read, Inner Write (release-and-reacquire)
#player {
    let h = player.health          // Read access, covered
    #player upgrade write {
        // WARNING: h may be stale after reacquisition
        player.health = h + 10     // Write access via upgrade
    }
    let m = player.mana            // Read key reacquired after inner block
}

// ERROR: Missing `upgrade` keyword
#player {
    let h = player.health
    #player write {                // E-KEY-0012: nested escalation requires `upgrade`
        player.health = h + 10
    }
}
```

**Nested Block Ordering:**

Each `#` block orders its own listed paths canonically. The relative order of keys between nesting levels is determined by nesting structure (outer acquired before inner).

**Deadlock Hazard from Inconsistent Nesting:**

Nested `#` blocks that acquire overlapping keys in different orders across tasks create potential deadlock:

```cursive
// HAZARD: Potential deadlock
// Task 1:              // Task 2:
#alpha {                #beta {
    #beta { }               #alpha { }
}                       }
```

The programmer MUST ensure consistent nesting order across tasks to avoid deadlock. The compiler SHOULD detect this pattern when both blocks are visible:

**(K-Nested-Cycle-Detection)**
$$\frac{
    \text{Block}_1 : \#P_1\ \{\ \#P_2\ \{\ \ldots\ \}\ \} \quad
    \text{Block}_2 : \#P_2\ \{\ \#P_1\ \{\ \ldots\ \}\ \} \quad
    \neg\text{Disjoint}(P_1, P_2)
}{
    \text{Emit}(\texttt{W-KEY-0012})
}$$

**Resolution:** Combine into a single `#` block:

```cursive
#alpha, beta { ... }    // Canonical order: alpha, beta — no deadlock
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-KEY-0011` | Error    | Detectable key ordering cycle within procedure  | Compile-time | Rejection |
| `W-KEY-0011` | Warning  | `#` block contains incomparable dynamic indices | Compile-time | Warning   |
| `W-KEY-0012` | Warning  | Nested `#` blocks with potential order cycle    | Compile-time | Warning   |


#### 13.7.1 Read-Then-Write Prohibition

##### Definition

The **read-then-write prohibition** prevents patterns where a `shared` path is read and then written within the same statement without holding a covering Write key. Such patterns risk upgrade deadlock when multiple tasks attempt simultaneous read-to-write upgrades.

**Formal Definition**

Let $\text{Subexpressions}(S)$ denote the set of all subexpressions within statement $S$. Define:

$$\text{ReadsPath}(e, P) \iff \text{Accesses}(e, P) \land \text{ReadContext}(e)$$

$$\text{WritesPath}(e, P) \iff \text{Accesses}(e, P) \land \text{WriteContext}(e)$$

where $\text{Accesses}(e, P)$ holds when expression $e$ accesses path $P$ (directly or through a prefix), and $\text{ReadContext}$ and $\text{WriteContext}$ are defined in §13.1.2.

A **read-then-write pattern** occurs when a statement contains both a read and a write to the same `shared` path:

$$\text{ReadThenWrite}(P, S) \iff \exists e_r, e_w \in \text{Subexpressions}(S) : \text{ReadsPath}(e_r, P) \land \text{WritesPath}(e_w, P)$$

##### Static Semantics

**Prohibition Rule**

The compiler MUST reject read-then-write patterns when no covering Write key is statically held:

**(K-Read-Write-Reject)**
$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    \text{ReadThenWrite}(P, S) \quad
    \neg\exists (Q, \text{Write}, S') \in \Gamma_{\text{keys}} : \text{Prefix}(Q, P)
}{
    \text{Emit}(\texttt{E-KEY-0060})
}$$

**Pattern Classification**

| Pattern            | Covering Write Key?         | Classification      | Action                |
| :----------------- | :-------------------------- | :------------------ | :-------------------- |
| `p += e`           | Yes (desugars to `#` block) | Compound assignment | Permitted             |
| `#p { p = p + e }` | Yes                         | Explicit `#` block  | Permitted             |
| `p = p + e`        | No                          | Read-then-write     | **Reject E-KEY-0060** |
| `p = f(p)`         | No                          | Read-then-write     | **Reject E-KEY-0060** |
| `p.a = p.b + 1`    | No, but disjoint paths      | Disjoint fields     | Permitted             |

**Separate Statements**

Reading and writing the **same** `shared` path in **separate statements** is permitted. Keys are released between statements:

```cursive
let current = player.health    // Read key acquired, released at semicolon
// ... other code ...
player.health = current + 10   // Write key acquired fresh — no conflict
```

This is safe because no key is held across statements; no upgrade occurs.

> **Note:** The implementation SHOULD emit `W-KEY-0004` when a read-then-write pattern in separate statements occurs within a procedure that captures `shared` data and may later be invoked from a `parallel` context. This warns developers of potential contention if the code is parallelized.

**Aliasing Considerations**

When the compiler cannot prove that two paths are distinct, it MUST assume they may alias:

```cursive
procedure update(a: shared Player, b: shared Player) {
    a.health = b.health + 10   // Potentially a == b — E-KEY-0060
}

procedure update_distinct(a: shared Player, b: shared Player)
    |= a != b                  // Contract proves distinctness
{
    a.health = b.health + 10   // OK — proven distinct
}
```

##### Constraints & Legality

**Negative Constraints**

1. A program MUST NOT read and write the same `shared` path in the same statement without a covering Write key.
2. A program MUST NOT rely on implicit read-to-write key upgrade semantics; no such operation exists.

**Resolution Strategies**

When `E-KEY-0060` is emitted, the programmer MUST resolve it by one of:

1. **Use compound assignment:** `p += e` instead of `p = p + e`
2. **Use explicit `#` block:** `#p { p = p + e }`
3. **Separate into distinct statements:** `let tmp = p; p = tmp + e`
4. **Prove path distinctness:** Add contract or use disjoint paths

**Advisory Warnings**

The implementation SHOULD emit `W-KEY-0006` when a programmer writes an explicit `#path write { let v = path; path = v + e }` block that could be expressed as compound assignment (`path += e`). This warning is advisory; the explicit form is valid but unnecessarily verbose.

**Diagnostic Table**

| Code         | Severity | Condition                                                                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0060` | Error    | Read-then-write on same `shared` path without covering Write key            | Compile-time | Rejection |
| `W-KEY-0004` | Warning  | Read-then-write in sequential context; may cause contention if parallelized | Compile-time | Warning   |
| `W-KEY-0006` | Warning  | Explicit read-then-write form used; compound assignment available           | Compile-time | Warning   |

---

#### 13.7.2 Nested Block Mode Escalation

##### Definition

A nested `#` block that requests a stronger mode than its enclosing block performs a **key upgrade**. Because true atomic upgrade risks deadlock when multiple tasks attempt simultaneous upgrades, Cursive uses **release-and-reacquire** semantics.

The `upgrade` keyword MUST be present to indicate the programmer understands and accepts potential interleaving between the release and reacquisition.

##### Syntax & Declaration

**Grammar**

The `upgrade write` modifier is part of the `key_block` grammar defined in §13.4.

**Usage**
```cursive
#player {                          // Acquires Read key
    let h = player.health
    if should_heal(h) {
        #player upgrade write {    // Release Read, acquire Write
            player.health = h + 10
        }                          // Release Write, reacquire Read
    }
    let m = player.mana            // Continues with Read key
}
```

##### Static Semantics

**(K-Upgrade-Required)**
$$\frac{
    \text{Held}(P, M_{\text{outer}}, \Gamma_{\text{keys}}) \quad
    \#Q\ M_{\text{inner}}\ \{\ \ldots\ \} \quad
    \text{Overlaps}(P, Q) \quad
    M_{\text{inner}} > M_{\text{outer}} \quad
    \texttt{upgrade} \notin \text{modifiers}
}{
    \text{Emit}(\texttt{E-KEY-0012})
}$$

**(K-Upgrade-Prohibited)**
$$\frac{
    \#Q\ \texttt{upgrade}\ M\ \{\ \ldots\ \} \quad
    \neg\exists (P, M', \_) \in \Gamma_{\text{keys}} : \text{Overlaps}(P, Q) \land M > M'
}{
    \text{Emit}(\texttt{E-KEY-0013})
}$$

##### Dynamic Semantics

**Release-and-Reacquire Sequence**

When entering `#path upgrade write { body }` while holding a Read key to an 
overlapping path:

1. **Release:** Release the Read key held by the enclosing block
2. **Acquire:** Acquire a Write key to `path` (blocking if contended)
3. **Execute:** Evaluate `body`
4. **Release:** Release the Write key
5. **Reacquire:** Acquire a Read key for the enclosing block's remaining scope

**(K-Upgrade-Sequence)**
$$\frac{
    \text{Held}(P, \text{Read}, S_{\text{outer}}) \quad
    \#P\ \texttt{upgrade write}\ \{\ B\ \}
}{
    \begin{array}{l}
    \text{Release}(P, \Gamma_{\text{keys}});\
    \text{Acquire}(P, \text{Write}, S_{\text{inner}});\
    \text{Eval}(B);\ \\
    \text{Release}(P, \Gamma_{\text{keys}});\
    \text{Acquire}(P, \text{Read}, S_{\text{outer}})
    \end{array}
}$$

**Reacquisition Semantics**

Step 5 (Reacquire) follows normal key acquisition semantics:

1. If another task holds a Write key to an overlapping path, the current task blocks until the key is released.
2. Multiple tasks MAY hold Read keys concurrently after reacquisition.
3. An interleaving window exists between steps 4 and 5, during which other tasks MAY acquire and release keys.

**Optimization: Eliding Reacquisition**

The implementation MAY elide step 5 if static analysis proves that no statements in the enclosing block's remaining scope access paths covered by the original Read key. This is an optimization; observable behavior MUST be equivalent to performing the reacquisition.

**(K-Upgrade-Reacquire-Elision)**
$$\frac{
    \text{RemainingStatements}(S_{\text{outer}}) \cap \text{AccessPaths}(P) = \emptyset
}{
    \text{Step 5 MAY be elided}
}$$

**Interleaving Window**

Between steps 1 and 2, other tasks MAY:
- Acquire keys to the same path
- Modify the shared data
- Complete their operations

The programmer MUST account for this interleaving. Values read before the `upgrade` 
block MAY be stale after it completes.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-KEY-0012` | Error    | Nested mode escalation without `upgrade` keyword | Compile-time | Rejection |
| `E-KEY-0013` | Error    | `upgrade` keyword on non-escalating block        | Compile-time | Rejection |
| `W-KEY-0010` | Warning  | `upgrade` block permits interleaving             | Compile-time | Warning   |

> **Note:** If atomicity between read and write operations is required, declare `write` mode on the outermost `#` block. The `upgrade` keyword is for cases where reduced exclusion duration is worth the interleaving trade-off.

---

#### 13.7.3 Speculative Block Semantics

##### Definition

A **speculative key block** executes without acquiring an exclusive key, instead relying on optimistic concurrency control. The block body executes against a snapshot of the keyed paths; at block exit, the implementation attempts to atomically commit all writes if and only if the snapshot values remain unchanged.

**Formal Definition**

A speculative block `#P speculative write { B }` has three components:

1. **Read set** $R(B)$: The set of $(path, value)$ pairs read during evaluation of $B$
2. **Write set** $W(B)$: The set of $(path, value)$ pairs to be written by $B$
3. **Commit predicate**: $\bigwedge_{(p,v) \in R(B)} (\text{current}(p) = v)$

$$\text{SpeculativeCommit}(R, W) \iff \text{Atomic}\left(\bigwedge_{(p,v) \in R} p = v \implies \bigwedge_{(q,w) \in W} q := w\right)$$

##### Syntax & Declaration

**Grammar**

The `speculative write` modifier is part of the `key_block` grammar defined in §13.4.

**Usage Patterns**

```cursive
// Single-value atomic update
#counter speculative write {
    let v = counter
    counter = v + 1
}

// Multi-field atomic update
#point speculative write {
    let x = point.x
    let y = point.y
    point.x = y
    point.y = x
}

// With computation
#balance speculative write {
    let current = balance
    if current >= amount {
        balance = current - amount
    }
}
```

##### Static Semantics

**Well-Formedness Rules**

A speculative block is well-formed if and only if all of the following conditions hold:

**(K-Spec-Write-Required)**
$$\frac{
    \#P\ \texttt{speculative}\ M\ \{B\} \quad M \neq \texttt{write}
}{
    \text{Emit}(\texttt{E-KEY-0095})
}$$

**(K-Spec-Pure-Body)**

The block body MUST be **effect-pure** with respect to paths outside the keyed set. Formally:

$$\frac{
    \#P\ \texttt{speculative write}\ \{B\} \quad
    \text{Writes}(B) \not\subseteq \text{CoveredPaths}(P)
}{
    \text{Emit}(\texttt{E-KEY-0091})
}$$

where $\text{CoveredPaths}(P)$ is the set of all paths for which $P$ is a prefix.

**Permitted operations** within a speculative block:
- Read from keyed paths
- Write to keyed paths
- Pure computation (arithmetic, logic, local bindings)
- Procedure calls to `const` receiver methods on keyed data
- Procedure calls to pure procedures (no `shared` or `unique` parameters)

**Prohibited operations** within a speculative block:
- Write to paths outside the keyed set
- Nested key blocks (speculative or blocking)
- `wait` expressions
- Procedure calls with side effects
- `defer` statements
- Panic-inducing operations (bounds checks are permitted; panic aborts the speculation)

**(K-Spec-No-Nested-Key)**
$$\frac{
    \#P\ \texttt{speculative write}\ \{B\} \quad
    \#Q\ \_\ \{\ldots\} \in \text{Subexpressions}(B)
}{
    \text{Emit}(\texttt{E-KEY-0090})
}$$

**(K-Spec-No-Wait)**
$$\frac{
    \#P\ \texttt{speculative write}\ \{B\} \quad
    \texttt{wait}\ e \in \text{Subexpressions}(B)
}{
    \text{Emit}(\texttt{E-KEY-0092})
}$$

**(K-Spec-No-Defer)**
$$\frac{
    \#P\ \texttt{speculative write}\ \{B\} \quad
    \texttt{defer}\ \{\ldots\} \in \text{Statements}(B)
}{
    \text{Emit}(\texttt{E-KEY-0093})
}$$

**(K-Spec-No-Upgrade)**
$$\frac{
    \#P\ \texttt{speculative upgrade write}\ \{B\}
}{
    \text{Emit}(\texttt{E-KEY-0094})
}$$

**Type of Speculative Block**

A speculative block has the same type as its body expression:

$$\frac{
    \Gamma \vdash B : T
}{
    \Gamma \vdash \#P\ \texttt{speculative write}\ \{B\} : T
}$$

The block is an expression and may produce a value. The value is that of the successful execution.

##### Dynamic Semantics

**Execution Model**

Evaluation of `#path speculative write { body }` proceeds as follows:

1. **Initialize**: Set $\text{retries} := 0$
2. **Snapshot**: Read all paths in the keyed set, recording $(path, value)$ pairs in the read set $R$
3. **Execute**: Evaluate $body$, collecting writes in the write set $W$
4. **Commit**: Atomically verify $\bigwedge_{(p,v) \in R} (\text{current}(p) = v)$ and, if true, apply all writes in $W$
5. **On success**: Return the value of $body$
6. **On failure**:
   - Increment $\text{retries}$
   - If $\text{retries} < \text{MAX\_SPECULATIVE\_RETRIES}$: goto step 2
   - Otherwise: proceed to fallback (step 7)
7. **Fallback**: Acquire a blocking Write key (per §13.2), execute $body$, release key, return value

**Retry Limit**

The constant $\text{MAX\_SPECULATIVE\_RETRIES}$ is Implementation-Defined Behavior (IDB). Implementations MUST document this value in the Conformance Dossier. A typical value is 3–10 retries.

**Snapshot Granularity**

The snapshot (step 2) captures the state of keyed paths at a single instant. The mechanism is Implementation-Defined:

| Data Type             | Typical Snapshot Mechanism                      |
| :-------------------- | :---------------------------------------------- |
| Primitive (≤ 8 bytes) | Atomic load                                     |
| Small struct          | Seqlock read or atomic load of version + fields |
| Large struct          | Copy under brief lock or seqlock                |

**Commit Atomicity**

The commit operation (step 4) MUST be atomic with respect to all other key operations (speculative or blocking) on overlapping paths. The mechanism is Implementation-Defined:

| Data Type                  | Typical Commit Mechanism             |
| :------------------------- | :----------------------------------- |
| Single primitive           | Compare-and-swap                     |
| Single pointer-sized value | Compare-and-swap                     |
| Two pointer-sized values   | Double-word CAS (DCAS) or lock-based |
| Struct with version field  | CAS on version + write fields        |
| Large struct               | Acquire lock, verify, write, release |

**Interaction with Blocking Keys**

Speculative blocks and blocking key blocks operate on the same underlying synchronization state:

- A blocking Write key acquisition waits for in-flight speculative commits to complete
- A speculative commit fails if a blocking Write key is held
- Multiple concurrent speculative blocks may race; at most one succeeds per commit attempt

| Concurrent Operation      | Speculative Block Behavior                  |
| :------------------------ | :------------------------------------------ |
| Another speculative block | Race; one commits, others retry             |
| Blocking Read key held    | Speculative may commit (compatible)         |
| Blocking Write key held   | Speculative commit fails, retry or fallback |

**Panic During Speculation**

If a panic occurs during step 3 (Execute):

1. The write set $W$ is discarded (no writes are committed)
2. No key is held (nothing to release)
3. The panic propagates normally

This ensures speculative execution has no observable effect on failure.

**Memory Ordering**

Speculative blocks use the following memory ordering:

- Snapshot reads: Acquire semantics
- Successful commit: Release semantics
- Failed commit: No ordering guarantees (writes discarded)

Memory ordering annotations (`[[relaxed]]`, etc.) MUST NOT appear inside speculative blocks. The implementation controls ordering to ensure correctness.

##### Memory & Layout

**Version Counters**

To implement speculative commits on non-primitive data, implementations MAY add hidden version counters to `shared` values. This is Implementation-Defined Behavior:

| Aspect                         | Specification                       |
| :----------------------------- | :---------------------------------- |
| Presence of version field      | IDB                                 |
| Size of version field          | IDB; if present, MUST be documented |
| Location (inline vs. external) | IDB                                 |

When version counters are used:
- Increment version on every successful Write key release or speculative commit
- Speculative snapshot records version
- Speculative commit verifies version unchanged

**No-Overhead Elision**

If the compiler proves a `shared` value is never accessed speculatively (no `speculative write` blocks target it), the implementation MAY omit version counter overhead. This analysis is Implementation-Defined.

##### Constraints & Legality

**Negative Constraints**

1. A speculative block MUST include the `write` modifier.
2. A speculative block MUST NOT contain nested key blocks.
3. A speculative block MUST NOT contain `wait` expressions.
4. A speculative block MUST NOT contain `defer` statements.
5. A speculative block MUST NOT write to paths outside the keyed set.
6. A speculative block MUST NOT combine with the `upgrade` modifier.
7. Memory ordering annotations MUST NOT appear inside speculative blocks.

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0090` | Error    | Nested key block inside speculative block              | Compile-time | Rejection |
| `E-KEY-0091` | Error    | Write to path outside keyed set in speculative block   | Compile-time | Rejection |
| `E-KEY-0092` | Error    | `wait` expression inside speculative block             | Compile-time | Rejection |
| `E-KEY-0093` | Error    | `defer` statement inside speculative block             | Compile-time | Rejection |
| `E-KEY-0094` | Error    | `speculative` combined with `upgrade`                  | Compile-time | Rejection |
| `E-KEY-0095` | Error    | `speculative` without `write` modifier                 | Compile-time | Rejection |
| `E-KEY-0096` | Error    | Memory ordering annotation inside speculative block    | Compile-time | Rejection |
| `W-KEY-0020` | Warning  | Speculative block on large struct (may be inefficient) | Compile-time | Warning   |
| `W-KEY-0021` | Warning  | Speculative block body may be expensive to re-execute  | Compile-time | Warning   |

> **Note:** `W-KEY-0020` is emitted when `sizeof(T) > 2 * sizeof(usize)` for the keyed type. Speculative operations on large types may degrade to lock-based fallback frequently. Consider using blocking keys or restructuring data.

> **Note:** `W-KEY-0021` is emitted when the block body contains loops, recursive calls, or procedure calls that are not provably cheap. Speculative blocks work best with small, deterministic bodies.

---

### 13.8 Keys and Async Suspension

##### Definition

Keys MUST NOT be held across async suspension points (`wait` expressions). This is enforced at compile time.

##### Static Semantics

**(K-No-Wait-Holding)**

$$\frac{
    \Gamma_{\text{keys}}(p) \neq \emptyset \quad
    \text{IsWait}(p)
}{
    \text{Emit}(\texttt{E-KEY-0050})
}$$

The compiler tracks key state and MUST reject programs where `wait` occurs while keys are logically held.

**Scope Interaction**

The prohibition applies to keys held at the `wait` program point, not to keys in enclosing scopes that have already been released:

```cursive
// VALID: Key released before wait
player.health += 1             // Key acquired and released at semicolon
wait ctx.io~>save(player)      // No keys held at wait point

// INVALID: Key held at wait point
#player {
    player.health += 1
    wait ctx.io~>save(player)  // ERROR E-KEY-0050: key to player still held
}
```

##### Dynamic Semantics

**Resolution Patterns**

When a key is held at a `wait` point, the programmer MUST restructure the code to release the key before suspension. Valid patterns include:

1. **Release before wait:** Move the `wait` expression outside the key-holding scope
2. **Separate scopes:** Use explicit block boundaries to release keys before the `wait`
```cursive
// INVALID:
#player {
    player.health += 1
    wait ctx.io~>save(player)  // ERROR E-KEY-0050
}

// VALID — Release before wait:
player.health += 1             // Key released at semicolon
wait ctx.io~>save(player)      // No keys held

// VALID — Separate scopes:
{
    #player { player.health += 1 }
}   // Key released at block exit
wait ctx.io~>save(player)      // No keys held
```

> **Rationale:** Prohibiting keys across suspension points prevents two classes of concurrency bugs:
>
> | Problem    | Description                                                                                       |
> | :--------- | :------------------------------------------------------------------------------------------------ |
> | Deadlock   | Task holds key, suspends; other task needs key, blocks; first task's completion depends on second |
> | Starvation | Long async operation blocks all other tasks needing the key                                       |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                | Detection    | Effect    |
| :----------- | :------- | :----------------------- | :----------- | :-------- |
| `E-KEY-0050` | Error    | `wait` while key is held | Compile-time | Rejection |

---

### 13.9 Compile-Time Verification and Runtime Realization

##### Definition

Keys are a **compile-time abstraction**. The compiler performs static key analysis to verify safe access patterns. Runtime synchronization is emitted **only when** static verification is insufficient.

**The Verification Hierarchy**

| Level                       | Mechanism                     | Runtime Cost |
| :-------------------------- | :---------------------------- | :----------- |
| **Static Proof**            | Compiler proves safety        | Zero         |
| **Runtime Synchronization** | Static proof fails; emit sync | Non-zero     |

##### Static Semantics

**Compile-Time Key Analysis**

The compiler MUST perform key analysis for all `shared` path accesses:

1. Compute the key path (per §13.4, §13.5)
2. Determine the required mode (per §13.1.2)
3. Track key state through control flow
4. Verify no conflicting accesses
5. Determine if static safety can be proven

**Static Safety Conditions**

An access is **statically safe** if the compiler proves one of the following:

| Condition              | Description                                                       | Rule   |
| :--------------------- | :---------------------------------------------------------------- | :----- |
| **No escape**          | `shared` value never escapes to another task                      | K-SS-1 |
| **Disjoint paths**     | Concurrent accesses target provably disjoint paths                | K-SS-2 |
| **Sequential context** | No `parallel` block encloses the access                           | K-SS-3 |
| **Unique origin**      | Value is `unique` at origin, temporarily viewed as `shared`       | K-SS-4 |
| **Dispatch-indexed**   | Access indexed by `dispatch` iteration variable                   | K-SS-5 |
| **Speculative-only**   | All accesses are within speculative blocks and fallback permitted | K-SS-6 |

**(K-Static-Safe)**
$$\frac{
    \text{Access}(P, M) \quad
    \text{StaticallySafe}(P)
}{
    \text{NoRuntimeSync}(P)
}$$

**When Runtime Synchronization Is Required**

| Situation                          | Why Static Proof Fails        |
| :--------------------------------- | :---------------------------- |
| `shared` escapes to multiple tasks | Cannot track all access sites |
| Dynamic indices to same array      | Cannot prove disjointness     |
| External `shared` parameter        | Caller context unknown        |
| `shared` captured from outer scope | May be accessed concurrently  |
| Cross-compilation-unit call        | Callee access pattern unknown |

##### Dynamic Semantics

**Runtime Realization**

When runtime synchronization is required, the implementation MUST provide:

1. Mutual exclusion per key compatibility rules (§13.1.2)
2. Blocking when incompatible keys are held
3. Release on scope exit (including panic)
4. Progress guarantee (no indefinite starvation)

**Implementation Strategies (IDB)**

| Data Characteristic         | Typical Implementation       |
| :-------------------------- | :--------------------------- |
| Small primitive (≤ 8 bytes) | Atomic operations            |
| Record/struct               | Reader-writer lock           |
| Array (static indices)      | Per-element or segment locks |
| Array (dynamic indices)     | Address-keyed lock table     |
| `#` boundary                | Single lock for subtree      |
| Speculative on primitive    | Compare-and-swap loop        |
| Speculative on struct       | Seqlock or versioned commit  |

Implementations MUST document their strategy in the Conformance Dossier.

**Observational Equivalence**

Program behavior MUST be identical whether safety is proven statically or enforced at runtime. The only permitted difference is performance:

$$\forall P : \text{Observable}(\text{StaticSafe}(P)) = \text{Observable}(\text{RuntimeSync}(P))$$

##### Memory & Layout

**Static Case — No Overhead**

When all accesses to a `shared` value are statically safe:
- No synchronization metadata is required
- Layout MAY be identical to `unique` equivalent
- No runtime code is generated for key operations

**Runtime Case — Implementation-Defined**

When runtime synchronization is required:

| Aspect              | Specification                             |
| :------------------ | :---------------------------------------- |
| Metadata location   | IDB: inline or external                   |
| Metadata size       | IDB: MUST be documented                   |
| Lock granularity    | IDB: per-value, per-field, or per-element |
| Lock implementation | IDB: spinlock, mutex, RwLock, etc.        |



##### Constraints & Legality

**Requirements Summary**

| Requirement                                          | Level |
| :--------------------------------------------------- | :---- |
| Perform static key analysis                          | MUST  |
| Prove safety when possible                           | MUST  |
| Emit runtime sync when static proof fails            | MUST  |
| Runtime sync must conform to key semantics           | MUST  |
| Static proof must not change observable behavior     | MUST  |
| Document realization strategy in Conformance Dossier | MUST  |

**Generic Code and Key Analysis**

Key analysis operates on monomorphized code. For generic procedures parameterized over permission:

1. **Monomorphization:** When a generic procedure is instantiated with `shared` as the permission parameter, key analysis applies to the instantiated procedure body.

2. **Permission-Polymorphic Code:** A procedure with signature `procedure f<P: Permission>(x: P T)` is analyzed separately for each instantiation. When `P = shared`, key analysis rules apply; when `P = const` or `P = unique`, key analysis is not required.

3. **Form Bounds:** If a generic procedure requires `P: Shareable` or similar bounds, the implementation MUST perform key analysis for all instantiations satisfying those bounds.

**(K-Generic-Instantiation)**
$$\frac{
    \text{Generic } f\langle P \rangle \text{ instantiated with } P = \texttt{shared} \quad
    \Gamma \vdash \text{body}(f) : \texttt{shared}\ T
}{
    \text{KeyAnalysis applies to } f\langle\texttt{shared}\rangle
}$$

**Generic Procedures and Form Bounds**

When a generic procedure has a form bound `T: Form` and is instantiated with a `shared` type, key analysis proceeds as follows:

1. Each form method call on the `shared` parameter is analyzed for key requirements.
2. The key mode is determined by the method's receiver permission: `~` requires Read, `~%` requires Write.
3. Form methods with `~!` receivers are not callable through `shared` paths (per §4.5.6).

**(K-Generic-Form-Method)**
$$\frac{
    \Gamma \vdash e : \texttt{shared}\ T \quad
    T : Tr \quad
    m \in \text{Methods}(Tr) \quad
    m.\text{receiver} = P\ \texttt{Self}
}{
    \text{KeyMode}(e.m(\ldots)) = \begin{cases}
    \text{Read} & \text{if } P = \texttt{const} \\
    \text{Write} & \text{if } P = \texttt{shared} \\
    \text{Reject} & \text{if } P = \texttt{unique}
    \end{cases}
}$$

**Conservative Fallback for Opaque Types**

When the concrete type is unknown (e.g., `witness Form`), the implementation MUST assume the most restrictive key requirements compatible with the form's method signatures:

- If any method in the form has a `~%` receiver, assume Write mode may be required
- If all methods have `~` receivers, Read mode is sufficient

---

### 13.10 Cross-Reference Notes `[INFORMATIVE]`

**Terms Defined in Clause 13**

| Term                    | Section | Description                                               |
| :---------------------- | :------ | :-------------------------------------------------------- |
| Key                     | §13.1   | Static proof of access rights to `shared` path            |
| Program Point           | §13.1   | Unique location in control flow for state tracking        |
| Lexical Scope           | §13.1   | Bounded code region defining key lifetime                 |
| Held (predicate)        | §13.1   | Key membership in state context                           |
| Path Prefix             | §13.1.1 | Relation for coverage determination                       |
| Path Disjointness       | §13.1.1 | Relation for static safety proofs                         |
| Segment Equivalence     | §13.1.1 | Equality of path segments including index equivalence     |
| Index Equivalence       | §13.1.1 | Provable equality of index expressions                    |
| Key Mode                | §13.1.2 | Read or Write access grant                                |
| Read Context            | §13.1.2 | Syntactic position requiring read access                  |
| Write Context           | §13.1.2 | Syntactic position requiring write access                 |
| Key Compatibility       | §13.1.2 | When two keys may coexist                                 |
| Read-Then-Write Pattern | §13.7.1 | Read and write to same path within single statement       |
| Covered (predicate)     | §13.2   | When held key subsumes finer access                       |
| Mode Sufficiency        | §13.2   | When held mode satisfies required mode                    |
| Key Path                | §13.4   | Path used for key analysis after coarsening               |
| Type-Level Boundary     | §13.5   | Permanent granularity constraint on field                 |
| Statically Resolvable   | §13.6   | Index evaluable at compile time                           |
| Provably Disjoint       | §13.6   | Index expressions with statically provable inequality     |
| Canonical Order         | §13.7   | Deterministic path ordering for deadlock prevention       |
| Key Upgrade             | §13.7.2 | Mode escalation from Read to Write                        |
| Release-and-Reacquire   | §13.7.2 | Non-atomic upgrade semantics avoiding deadlock            |
| Speculative Block       | §13.7.3 | Optimistic concurrency with validation and rollback       |
| Conflict Detection      | §13.7.3 | Detection of concurrent modification during commit        |
| Validated Commit        | §13.7.3 | Atomic verification and application of speculative writes |
| Fallback Execution      | §13.7.3 | Pessimistic retry after speculative conflict              |
| Static Safety           | §13.9   | Compiler-proven access safety                             |

**Terms Referenced from Other Clauses**

| Term                    | Source | Usage in Clause 13                                  |
| :---------------------- | :----- | :-------------------------------------------------- |
| `shared` permission     | §4.5.4 | Data requiring key analysis                         |
| `const` permission      | §4.5.4 | Immutable, no keys needed                           |
| `unique` permission     | §4.5.4 | Exclusive, no keys needed                           |
| Scope (name resolution) | §8.4   | Distinct from LexicalScope; used for binding lookup |
| `wait` expression       | §15    | Async suspension point                              |
| `dispatch`              | §14.3  | Data-parallel iteration                             |
| `parallel` block        | §14.1  | Structured concurrency primitive                    |
| `defer` statement       | §11.12 | Deferred execution at scope exit                    |
| Drop                    | §9.7   | Destructor form for cleanup                         |
| Panic                   | §11.15 | Unwinding error propagation                         |

**Diagnostic Code Summary**

| Code         | Severity | Section | Condition                                                                   |
| :----------- | :------- | :------ | :-------------------------------------------------------------------------- |
| `E-KEY-0001` | Error    | §13.1   | Access to `shared` path outside valid key context                           |
| `E-KEY-0002` | Error    | §13.1   | `#` annotation on non-`shared` path                                         |
| `E-KEY-0003` | Error    | §13.1   | Multiple `#` markers in single path expression                              |
| `E-KEY-0004` | Error    | §13.1   | Key escapes its defining scope                                              |
| `E-KEY-0005` | Error    | §13.1.2 | Write access required but only Read available                               |
| `E-KEY-0006` | Error    | §13.2   | Key acquisition in `defer` escapes to outer scope                           |
| `E-KEY-0010` | Error    | §13.6   | Potential conflict on dynamic indices                                       |
| `E-KEY-0011` | Error    | §13.7   | Detectable key ordering cycle within procedure                              |
| `E-KEY-0012` | Error    | §13.7.2 | Nested mode escalation without `upgrade` keyword                            |
| `E-KEY-0013` | Error    | §13.7.2 | `upgrade` keyword on non-escalating block                                   |
| `E-KEY-0020` | Error    | §13.4   | `#` immediately before method name                                          |
| `E-KEY-0031` | Error    | §13.4   | `#` block path not in scope                                                 |
| `E-KEY-0032` | Error    | §13.4   | `#` block path is not `shared`                                              |
| `E-KEY-0033` | Error    | §13.5   | `#` on field of non-record type                                             |
| `E-KEY-0050` | Error    | §13.8   | `wait` while key is held                                                    |
| `W-KEY-0001` | Warning  | §13.2   | Fine-grained keys in tight loop (performance hint)                          |
| `W-KEY-0002` | Warning  | §13.2   | Redundant key acquisition (already covered)                                 |
| `W-KEY-0003` | Warning  | §13.4   | `#` redundant (matches type boundary)                                       |
| `W-KEY-0004` | Warning  | §13.7.1 | Read-then-write in sequential context; may cause contention if parallelized |
| `W-KEY-0005` | Warning  | §13.3   | Callee access pattern unknown; assuming full access                         |
| `W-KEY-0011` | Warning  | §13.7   | `#` block contains incomparable dynamic indices                             |
| `W-KEY-0012` | Warning  | §13.7   | Nested `#` blocks with potential order cycle                                |
| `E-KEY-0060` | Error    | §13.7.1 | Read-then-write on same `shared` path without covering Write key            |
| `E-KEY-0070` | Error    | §13.4   | Write operation in `#` block without `write` modifier                       |
| `E-KEY-0083` | Error    | §13.3.1 | `shared witness Form` where Form has `~%` method                            |
| `E-KEY-0085` | Error    | §13.3.3 | Escaping closure with `shared` capture lacks type annotation                |
| `E-KEY-0086` | Error    | §13.3.3 | Escaping closure outlives captured `shared` binding                         |
| `W-KEY-0006` | Warning  | §13.7.1 | Explicit read-then-write form; compound assignment available                |
| `W-KEY-0009` | Warning  | §13.3.3 | Closure captures `shared` data                                              |
| `W-KEY-0010` | Warning  | §13.7.2 | `upgrade` block permits interleaving                                        |
| `E-KEY-0090` | Error    | §13.7.3 | Missing `else` block for speculative `#` block                              |
| `E-KEY-0091` | Error    | §13.7.3 | `else` block on non-speculative `#` block                                   |
| `E-KEY-0092` | Error    | §13.7.3 | Speculative block body contains non-local control flow                      |
| `E-KEY-0093` | Error    | §13.7.3 | Nested speculative blocks on overlapping paths                              |
| `E-KEY-0094` | Error    | §13.7.3 | `wait` expression inside speculative block                                  |
| `E-KEY-0095` | Error    | §13.7.3 | Procedure call with `shared` capture inside speculative block               |
| `E-KEY-0096` | Error    | §13.7.3 | Observable side effect (I/O, allocation) inside speculative block           |
| `E-KEY-0097` | Error    | §13.7.3 | Speculative block value escapes to non-speculative context                  |
| `W-KEY-0020` | Warning  | §13.7.3 | High conflict rate detected for speculative block (profile-guided)          |

---

## Clause 14: Structured Parallelism

This clause defines Cursive's structured parallelism model. Parallelism enables concurrent execution of code across multiple workers within a bounded scope. The model guarantees that all spawned work completes before the parallel scope exits, ensuring deterministic resource cleanup and composable concurrency.

---

### 14.1 Parallelism Overview

##### Definition

**Structured parallelism** is a concurrency model in which all concurrent work spawned within a scope completes before that scope exits. This guarantee enables safe capture of stack-local bindings, deterministic destruction ordering, and composable parallel regions.

Cursive's parallelism model comprises three orthogonal concepts:

| Concept           | Mechanism           | Purpose                              |
| :---------------- | :------------------ | :----------------------------------- |
| Execution Domain  | `parallel` block    | Specifies where work executes        |
| Work Distribution | `spawn`, `dispatch` | Creates concurrent work items        |
| Coordination      | Key system (§13)    | Synchronizes access to `shared` data |

**Formal Definition**

Let $\mathcal{W}$ denote the set of work items spawned within a parallel block $P$. The **structured concurrency invariant** states:

$$\forall w \in \mathcal{W}.\ \text{lifetime}(w) \subseteq \text{lifetime}(P)$$

A **work item** is a unit of computation queued for execution by a worker. Work items are created by `spawn` and `dispatch` expressions. A **worker** is an execution context (typically an OS thread or GPU compute unit) that executes work items.

**Task** is the runtime representation of a work item during execution. A task may suspend (at `wait` expressions) and resume. Tasks are associated with key state per §13.1.

---

### 14.2 The Parallel Block

##### Definition

A **parallel block** establishes a scope within which work may execute concurrently across multiple workers. The block specifies an execution domain and contains `spawn` and `dispatch` expressions that create work items. All work items MUST complete before execution proceeds past the block's closing brace.

**Formal Definition**

A parallel block $P$ is defined as a tuple:

$$P = (D, A, B)$$

where:
- $D$ is the execution domain expression
- $A$ is the (possibly empty) set of block attributes
- $B$ is the block body containing statements and work-creating expressions

##### Syntax & Declaration

**Grammar**

```ebnf
parallel_block    ::= "parallel" domain_expr attribute_list? block

domain_expr       ::= expression

attribute_list    ::= "[" attribute ("," attribute)* "]"

attribute         ::= "cancel" ":" expression
                    | "name" ":" string_literal

block             ::= "{" statement* "}"
```

The `domain_expr` MUST evaluate to a type that implements the `ExecutionDomain` form (§14.6).

**Attribute Semantics**

| Attribute | Type          | Purpose                                      |
| :-------- | :------------ | :------------------------------------------- |
| `cancel`  | `CancelToken` | Provides cooperative cancellation capability |
| `name`    | `string`      | Labels the block for debugging and profiling |

##### Static Semantics

**Typing Rule**

$$\frac{
  \Gamma \vdash D : \text{witness ExecutionDomain} \quad
  \Gamma_P = \Gamma[\text{parallel\_context} \mapsto D] \quad
  \Gamma_P \vdash B : T
}{
  \Gamma \vdash \texttt{parallel } D\ \{B\} : T
} \quad \text{(T-Parallel)}$$

The type of a parallel block is determined by its contents per §14.9.

**Well-Formedness**

A parallel block is well-formed if and only if:

1. The domain expression evaluates to a type implementing `ExecutionDomain`.
2. All `spawn` and `dispatch` expressions within the block body reference the enclosing parallel block.
3. No `spawn` or `dispatch` expression appears outside a parallel block.
4. All captured bindings satisfy the permission requirements of §14.3.

##### Dynamic Semantics

**Evaluation**

Evaluation of `parallel D [attrs] { B }` proceeds as follows:

1. Let $d$ be the result of evaluating domain expression $D$.
2. Initialize the worker pool according to $d$'s configuration.
3. If the `cancel` attribute is present, associate its token with the block.
4. Evaluate statements in $B$ sequentially; `spawn` and `dispatch` expressions enqueue work items to the worker pool.
5. Block at the closing brace until all enqueued work items complete.
6. If any work item panicked, propagate the first panic (by completion order) after all work settles.
7. Release workers back to the domain.
8. Return the collected results (per §14.9).

**Completion Ordering**

Work items complete in Unspecified order. The parallel block guarantees only that ALL work completes before the block exits.

##### Constraints & Legality

**Negative Constraints**

1. A `spawn` expression MUST NOT appear outside a parallel block.
2. A `dispatch` expression MUST NOT appear outside a parallel block.
3. Parallel blocks MUST NOT be nested within `async` procedure bodies without an intervening `wait` expression.

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-PAR-0001` | Error    | `spawn` or `dispatch` outside parallel | Compile-time | Rejection |
| `E-PAR-0002` | Error    | Domain expression not ExecutionDomain  | Compile-time | Rejection |
| `E-PAR-0003` | Error    | Invalid domain parameter type          | Compile-time | Rejection |

---

### 14.3 Capture Semantics

##### Definition

When a `spawn` or `dispatch` body references bindings from enclosing scopes, those bindings are **captured** into the work item. Capture rules ensure memory safety by preventing data races and use-after-free errors.

**Formal Definition**

Let $\text{FreeVars}(e)$ denote the set of free variables in expression $e$. For each $v \in \text{FreeVars}(e)$ where $e$ is a spawn or dispatch body, the **capture mode** is determined by $v$'s permission:

| Source Permission | Capture Mode | Work Item Permission | Source After Capture |
| :---------------- | :----------- | :------------------- | :------------------- |
| `const`           | Reference    | `const`              | Unchanged            |
| `shared`          | Reference    | `shared`             | Unchanged            |
| `unique`          | Move         | `unique`             | Moved (invalid)      |

##### Static Semantics

**Capture Analysis Rules**

For `const` and `shared` bindings, capture occurs by reference:

$$\frac{
  \Gamma \vdash v : P\ T \quad
  P \in \{\texttt{const}, \texttt{shared}\}
}{
  \Gamma_{\text{capture}} \vdash v : P\ T
} \quad \text{(T-Capture-Ref)}$$

For `unique` bindings, capture requires explicit `move`:

$$\frac{
  \Gamma \vdash v : \texttt{unique}\ T \quad
  \texttt{move}\ v \in \text{body}
}{
  \Gamma_{\text{capture}} \vdash v : \texttt{unique}\ T \quad
  \Gamma' = \Gamma[v \mapsto \text{Moved}]
} \quad \text{(T-Capture-Move)}$$

Implicit capture of `unique` bindings is forbidden:

$$\frac{
  \Gamma \vdash v : \texttt{unique}\ T \quad
  v \in \text{FreeVars}(\text{body}) \quad
  \texttt{move}\ v \notin \text{body}
}{
  \text{Emit}(\texttt{E-PAR-0020})
} \quad \text{(T-Capture-Unique-Error)}$$

**Lifetime Safety**

Captured references are guaranteed valid for the duration of all work items because structured concurrency ensures all work completes before the parallel block exits. The parallel block's lifetime contains all work item lifetimes.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-PAR-0020` | Error    | Implicit capture of `unique` binding      | Compile-time | Rejection |
| `E-PAR-0021` | Error    | Move of already-moved binding             | Compile-time | Rejection |
| `E-PAR-0022` | Error    | Move of binding from outer parallel scope | Compile-time | Rejection |

---

### 14.4 The Spawn Expression

##### Definition

A **spawn expression** creates a work item for concurrent execution within the enclosing parallel block. The work item executes independently and produces a result that may be collected.

##### Syntax & Declaration

**Grammar**

```ebnf
spawn_expr      ::= "spawn" attribute_list? block

attribute_list  ::= "[" attribute ("," attribute)* "]"

attribute       ::= "name" ":" string_literal
                  | "affinity" ":" expression
                  | "priority" ":" expression
```

**Attribute Semantics**

| Attribute  | Type       | Default            | Purpose                        |
| :--------- | :--------- | :----------------- | :----------------------------- |
| `name`     | `string`   | Anonymous          | Labels for debugging/profiling |
| `affinity` | `CpuSet`   | Domain default     | CPU core affinity hint         |
| `priority` | `Priority` | `Priority::Normal` | Scheduling priority hint       |

##### Static Semantics

**Typing Rule**

$$\frac{
  \Gamma[\text{parallel\_context}] = D \quad
  \Gamma_{\text{capture}} \vdash e : T
}{
  \Gamma \vdash \texttt{spawn}\ \{e\} : \text{SpawnHandle}\langle T \rangle
} \quad \text{(T-Spawn)}$$

A spawn expression returns a `SpawnHandle<T>` representing the pending result.

**SpawnHandle Type**

```cursive
modal SpawnHandle<T> {
    @Pending { }
    
    @Ready {
        value: T,
    }
}
```

The `SpawnHandle<T>` type supports the `wait` expression (§15.4) for result retrieval.

##### Dynamic Semantics

**Evaluation**

Evaluation of `spawn [attrs] { e }` proceeds as follows:

1. Capture free variables from enclosing scope per §14.3.
2. Package the captured environment with expression $e$ into a work item.
3. Enqueue the work item to the parallel block's worker pool.
4. Return a `SpawnHandle<T>@Pending` immediately (non-blocking).
5. A worker eventually dequeues and evaluates $e$.
6. Upon completion, the handle transitions to `@Ready` with the result value.

**Result Collection**

Spawn results are collected through three mechanisms:

1. **Implicit collection**: When a parallel block contains only spawn expressions, results form a tuple in declaration order.
2. **Explicit wait**: Use `wait handle` to retrieve a specific result.
3. **Ignored**: Results not collected are discarded; work still completes.

##### Constraints & Legality

**Negative Constraints**

1. A `spawn` expression MUST appear directly within a parallel block body.
2. A `spawn` expression MUST NOT appear within an escaping closure.

**Diagnostic Table**

| Code         | Severity | Condition                    | Detection    | Effect    |
| :----------- | :------- | :--------------------------- | :----------- | :-------- |
| `E-PAR-0030` | Error    | Invalid spawn attribute type | Compile-time | Rejection |
| `E-PAR-0031` | Error    | `spawn` in escaping closure  | Compile-time | Rejection |

---

### 14.5 The Dispatch Expression

##### Definition

A **dispatch expression** expresses data-parallel iteration where each iteration may execute concurrently. The key system (§13) determines which iterations can safely parallelize based on their access patterns to `shared` data.

##### Syntax & Declaration

**Grammar**

```ebnf
dispatch_expr   ::= "dispatch" pattern "in" range_expr 
                    key_clause?
                    attribute_list?
                    block

key_clause      ::= "key" path_expr key_mode

key_mode        ::= "read" | "write"

attribute_list  ::= "[" attribute ("," attribute)* "]"

attribute       ::= "reduce" ":" reduce_op
                  | "ordered"
                  | "chunk" ":" expression

reduce_op       ::= "+" | "*" | "min" | "max" | "and" | "or" | identifier
```

**Key Clause**

The optional key clause explicitly declares the key path and mode for each iteration. When omitted, the compiler infers key requirements from the body's access patterns.

**Attribute Semantics**

| Attribute | Type         | Purpose                                       |
| :-------- | :----------- | :-------------------------------------------- |
| `reduce`  | Reduction op | Combines iteration results using specified op |
| `ordered` | (flag)       | Forces sequential side-effect ordering        |
| `chunk`   | `usize`      | Groups iterations into chunks for granularity |

##### Static Semantics

**Typing Rule (Without Reduction)**

$$\frac{
  \Gamma \vdash \text{range} : \text{Range}\langle I \rangle \quad
  \Gamma, i : I \vdash B : T
}{
  \Gamma \vdash \texttt{dispatch } i \texttt{ in range } \{B\} : ()
} \quad \text{(T-Dispatch)}$$

**Typing Rule (With Reduction)**

$$\frac{
  \Gamma \vdash \text{range} : \text{Range}\langle I \rangle \quad
  \Gamma, i : I \vdash B : T \quad
  \Gamma \vdash \oplus : (T, T) \to T
}{
  \Gamma \vdash \texttt{dispatch } i \texttt{ in range [reduce: } \oplus \texttt{] } \{B\} : T
} \quad \text{(T-Dispatch-Reduce)}$$

**Key Inference**

When no key clause is provided, the compiler analyzes the body to infer key paths and modes:

| Body Access Pattern       | Inferred Key Path        | Inferred Mode   |
| :------------------------ | :----------------------- | :-------------- |
| `data[i] = ...`           | `data[i]`                | `write`         |
| `... = data[i]`           | `data[i]`                | `read`          |
| `data[i] = data[i] + ...` | `data[i]`                | `write`         |
| `result[i] = source[j]`   | `result[i]`, `source[j]` | `write`, `read` |

If inference fails or is ambiguous, diagnostic `E-PAR-0041` is emitted.

**Disjointness Guarantee**

Within a dispatch expression, accesses indexed by the iteration variable to the same array are proven disjoint across iterations:

$$\frac{
  \texttt{dispatch}\ v\ \texttt{in}\ r\ \{\ \ldots\ a[v]\ \ldots\ \}
}{
  \forall v_1, v_2 \in r,\ v_1 \neq v_2 \implies \text{ProvablyDisjoint}(a[v_1], a[v_2])
} \quad \text{(K-Disjoint-Dispatch)}$$

Per §13.6.5, this enables full parallelization when iterations access disjoint array elements.

##### Dynamic Semantics

**Evaluation**

Evaluation of `dispatch i in range [attrs] { B }` proceeds as follows:

1. Evaluate `range` to determine iteration count $n$.
2. Analyze key patterns to partition iterations into conflict-free groups.
3. For each group, enqueue iterations as work items to the worker pool.
4. Workers execute iterations, acquiring keys as needed per §13.2.
5. If `[reduce: op]` is present, combine partial results using `op`.
6. Block until all iterations complete.
7. Return the reduced value (if reduction) or unit.

**Parallelism Determination**

The key system partitions iterations based on key paths:

| Key Pattern   | Keys Generated      | Parallelism Degree    |
| :------------ | :------------------ | :-------------------- |
| `data[i]`     | $n$ distinct keys   | Full parallel         |
| `data[i / 2]` | $n/2$ distinct keys | Pairs serialize       |
| `data[i % k]` | $k$ distinct keys   | $k$-way parallel      |
| `data[f(i)]`  | Unknown at compile  | Runtime serialization |

**Reduction Semantics**

Reduction operators MUST be associative:

$$\forall a, b, c.\ (a \oplus b) \oplus c = a \oplus (b \oplus c)$$

For non-associative operations, the `[ordered]` attribute is required, which forces sequential execution.

Parallel reduction proceeds as follows:

1. Partition iterations across workers.
2. Each worker reduces its partition to a partial result.
3. Combine partial results in a deterministic tree pattern.
4. Return the final result.

The tree combination order is deterministic based on iteration indices, ensuring reproducible results for non-commutative associative operators.

##### Constraints & Legality

**Cross-Iteration Dependency**

Accesses that reference a different iteration's data are rejected:

```cursive
dispatch i in 0..arr.len() - 1 {
    arr[i] = arr[i + 1]     // ERROR E-PAR-0042: cross-iteration dependency
}
```

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-PAR-0040` | Error    | Dispatch outside parallel block               | Compile-time | Rejection |
| `E-PAR-0041` | Error    | Key inference failed; explicit key required   | Compile-time | Rejection |
| `E-PAR-0042` | Error    | Cross-iteration dependency detected           | Compile-time | Rejection |
| `E-PAR-0043` | Error    | Non-associative reduction without `[ordered]` | Compile-time | Rejection |
| `W-PAR-0040` | Warning  | Dynamic key pattern; runtime serialization    | Compile-time | Warning   |

---

### 14.6 Execution Domains

##### Definition

An **execution domain** is a capability that provides access to computational resources. Domains are accessed through the `Context` record and implement the `ExecutionDomain` form.

**Formal Definition**

```cursive
form ExecutionDomain {
    /// Human-readable name for debugging and profiling.
    procedure name(~) -> string
    
    /// Maximum concurrent work items supported.
    procedure max_concurrency(~) -> usize
}
```

This form is witness-safe, enabling heterogeneous domain handling.

#### 14.6.1 CPU Domain

##### Definition

The **CPU domain** executes work items on operating system threads managed by the runtime.

##### Syntax & Declaration

**Access**

```cursive
ctx.cpu(workers: n, affinity: cpus, priority: p, stack_size: s)
```

**Parameters**

| Parameter    | Type       | Default                | Purpose                  |
| :----------- | :--------- | :--------------------- | :----------------------- |
| `workers`    | `usize`    | System CPU count       | Number of worker threads |
| `affinity`   | `CpuSet`   | All CPUs               | CPU core affinity mask   |
| `priority`   | `Priority` | `Priority::Normal`     | Thread scheduling hint   |
| `stack_size` | `usize`    | Implementation-defined | Stack size per worker    |

**Supporting Types**

```cursive
record CpuSet {
    mask: [bool; MAX_CPUS],

    procedure all() -> CpuSet
    procedure cores(indices: const [usize]) -> CpuSet
    procedure range(start: usize, end: usize) -> CpuSet
    procedure numa_node(node: usize) -> CpuSet
}

enum Priority {
    Idle,
    Low,
    Normal,
    High,
    Realtime,
}
```

The `Realtime` priority requires elevated OS privileges. Attempting to use `Realtime` without sufficient privileges results in Implementation-Defined Behavior; implementations SHOULD fall back to `High` and emit a warning.

##### Dynamic Semantics

**Thread Pool Management**

1. On first parallel block entry with a given configuration, worker threads are created.
2. Worker threads execute work items from a shared queue.
3. Threads MAY be reused across parallel blocks with compatible configurations.
4. Thread creation and destruction timing is Implementation-Defined.

#### 14.6.2 GPU Domain

##### Definition

The **GPU domain** executes work items on graphics processing unit compute shaders.

##### Syntax & Declaration

**Access**

```cursive
ctx.gpu(device: d, queue: q)
```

**Parameters**

| Parameter | Type    | Default | Purpose             |
| :-------- | :------ | :------ | :------------------ |
| `device`  | `usize` | 0       | GPU device index    |
| `queue`   | `usize` | 0       | Command queue index |

##### Static Semantics

**GPU Capture Rules**

Code within GPU dispatch may capture only:

| Capturable Type       | Reason                       |
| :-------------------- | :--------------------------- |
| `GpuBuffer<T>`        | Device-accessible memory     |
| Primitive constants   | Embedded in shader constants |
| `const` small records | Uniform buffer compatible    |

The following are NOT capturable:

| Forbidden Type       | Reason                        |
| :------------------- | :---------------------------- |
| `shared` types       | Key system unavailable on GPU |
| Host pointers        | Different address space       |
| Heap-allocated types | Host memory inaccessible      |

##### Dynamic Semantics

**Memory Model**

GPU and CPU have separate memory spaces. Data transfer is explicit:

```cursive
parallel ctx.gpu() {
    let d_input: GpuBuffer<f32> = gpu::alloc(n)
    let d_output: GpuBuffer<f32> = gpu::alloc(n)
    
    gpu::upload(host_input, d_input)
    
    dispatch i in 0..n {
        d_output[i] = d_input[i] * 2.0
    }
    
    gpu::download(d_output, host_output)
}
```

**GPU Intrinsics**

The following intrinsics are available within GPU parallel blocks:

| Intrinsic                        | Type           | Purpose                       |
| :------------------------------- | :------------- | :---------------------------- |
| `gpu::alloc<T>(count: usize)`    | `GpuBuffer<T>` | Allocate device buffer        |
| `gpu::upload(src, dst)`          | `()`           | Copy host to device           |
| `gpu::download(src, dst)`        | `()`           | Copy device to host           |
| `gpu::global_id()`               | `usize`        | Global thread index           |
| `gpu::local_id()`                | `usize`        | Workgroup-local index         |
| `gpu::workgroup_id()`            | `usize`        | Workgroup index               |
| `gpu::barrier()`                 | `()`           | Workgroup sync barrier        |
| `gpu::atomic_add(ptr, val)`      | `T`            | Atomic add; returns old value |
| `gpu::atomic_min(ptr, val)`      | `T`            | Atomic min; returns old value |
| `gpu::atomic_max(ptr, val)`      | `T`            | Atomic max; returns old value |
| `gpu::atomic_cas(ptr, cmp, new)` | `T`            | Compare-and-swap; returns old |

**Dispatch Dimensions**

```cursive
// 1D dispatch
dispatch i in 0..n { ... }

// 2D dispatch
dispatch (x, y) in (0..width, 0..height) { ... }

// 3D dispatch
dispatch (x, y, z) in (0..w, 0..h, 0..d) { ... }

// Explicit workgroup size
dispatch i in 0..n [workgroup: 256] { ... }
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                        | Detection    | Effect    |
| :----------- | :------- | :------------------------------- | :----------- | :-------- |
| `E-PAR-0050` | Error    | Host memory access in GPU code   | Compile-time | Rejection |
| `E-PAR-0051` | Error    | `shared` capture in GPU dispatch | Compile-time | Rejection |
| `E-PAR-0052` | Error    | Nested GPU parallel block        | Compile-time | Rejection |

#### 14.6.3 Inline Domain

##### Definition

The **inline domain** executes work items sequentially on the current thread. This domain is useful for testing, debugging, and single-threaded execution contexts.

##### Syntax & Declaration

**Access**

```cursive
ctx.inline()
```

##### Dynamic Semantics

**Evaluation**

- `spawn { e }` evaluates $e$ immediately and blocks until complete.
- `dispatch i in range { e }` executes as a sequential loop.
- No actual parallelism occurs.
- All capture rules and permission requirements remain enforced.

#### 14.6.4 Domain in Context Record

##### Definition

Execution domains are accessed through factory methods on the `Context` record:

```cursive
record Context {
    // ... other capabilities ...
    
    cpu: CpuDomainFactory,
    gpu: GpuDomainFactory | None,
    inline: InlineDomainFactory,
}

record CpuDomainFactory {
    sys: witness System,

    procedure call(~,
        workers: usize = cpu_count(),
        affinity: CpuSet = CpuSet::all(),
        priority: Priority = Priority::Normal,
        stack_size: usize = default_stack_size()
    ) -> CpuDomain
}

record InlineDomainFactory { 
    procedure call(~) -> InlineDomain
}
```

The `gpu` field is `None` when no GPU is available.

---

### 14.7 Async Integration

##### Definition

Async operations (`wait`, `Async<T>` per §15) compose with parallel blocks. Workers may suspend on async operations, yielding to other work items while waiting.

##### Dynamic Semantics

**Wait Inside Parallel**

When a worker evaluates a `wait` expression:

1. The worker suspends the current work item.
2. The worker picks up another queued work item (if available).
3. When the async operation completes, the suspended work item becomes ready.
4. Some worker resumes the work item.

This enables efficient I/O and compute overlap without blocking threads.

**Key Prohibition**

Per §13.8, keys MUST NOT be held across `wait` suspension points:

```cursive
// INVALID
parallel ctx.cpu() {
    spawn {
        #data write {
            wait ctx.io~>read(file)  // ERROR E-KEY-0050
        }
    }
}

// VALID
parallel ctx.cpu() {
    spawn {
        let content = wait ctx.io~>read(file)
        #data write {
            data.update(content)
        }
    }
}
```

**SpawnHandle and Wait**

`SpawnHandle<T>` supports the `wait` expression:

```cursive
parallel ctx.cpu() {
    let handle = spawn { expensive_compute() }
    
    other_work()
    
    let result = wait handle
}
```

---

### 14.8 Cancellation

##### Definition

**Cancellation** is the cooperative mechanism by which in-progress parallel work may be requested to stop early. Work items MUST explicitly check for cancellation; the runtime does not forcibly terminate work.

##### Syntax & Declaration

**CancelToken Type**

```cursive
modal CancelToken {
    @Active { } {
        procedure cancel(~%)
        procedure is_cancelled(~) -> bool
        procedure wait_cancelled(~) -> Async<()>
        procedure child(~) -> CancelToken@Active
    }
    
    @Cancelled { } {
        procedure is_cancelled(~) -> bool { result true }
    }

    procedure new() -> CancelToken@Active
}
```

**Usage**

```cursive
let token = CancelToken::new()

parallel ctx.cpu() [cancel: token] {
    dispatch i in 0..n {
        if token.is_cancelled() {
            return
        }
        
        expensive_work(i)
        
        if token.is_cancelled() {
            return
        }
        
        more_work(i)
    }
}
```

##### Dynamic Semantics

**Propagation**

When a cancel token is attached to a parallel block via the `cancel` attribute, the token is implicitly available within all `spawn` and `dispatch` bodies.

**Cancellation vs Completion**

Cancellation is a request, not a guarantee:

| Scenario                       | Behavior                          |
| :----------------------------- | :-------------------------------- |
| Work checks and returns early  | Iteration completes immediately   |
| Work ignores cancellation      | Iteration runs to completion      |
| Work is queued but not started | MAY be dequeued without executing |
| Work is mid-execution          | Continues until next check point  |

---

### 14.9 Parallel Block Result Types

##### Definition

A parallel block is an expression that yields a value. The result type depends on the block's contents.

##### Static Semantics

**Result Type Determination**

| Block Contents                     | Result Type          |
| :--------------------------------- | :------------------- |
| No `spawn` or `dispatch`           | `()`                 |
| Single `spawn { e }` where $e : T$ | $T$                  |
| Multiple spawns $e_1, \ldots, e_n$ | $(T_1, \ldots, T_n)$ |
| `dispatch` without `[reduce]`      | `()`                 |
| `dispatch ... [reduce: op] { e }`  | $T$ where $e : T$    |
| Mixed spawns and reducing dispatch | Tuple of all results |

**Typing Rules**

$$\frac{
  \Gamma \vdash \texttt{parallel } D\ \{\}\ : ()
}{} \quad \text{(T-Parallel-Empty)}$$

$$\frac{
  \Gamma \vdash \texttt{spawn } \{e\} : \text{SpawnHandle}\langle T \rangle
}{
  \Gamma \vdash \texttt{parallel } D\ \{\texttt{spawn } \{e\}\} : T
} \quad \text{(T-Parallel-Single)}$$

$$\frac{
  \Gamma \vdash \texttt{spawn } \{e_i\} : \text{SpawnHandle}\langle T_i \rangle \quad \forall i \in 1..n
}{
  \Gamma \vdash \texttt{parallel } D\ \{\texttt{spawn } \{e_1\}; \ldots; \texttt{spawn } \{e_n\}\} : (T_1, \ldots, T_n)
} \quad \text{(T-Parallel-Multi)}$$

---

### 14.10 Panic Handling

##### Definition

When a work item panics within a parallel block, the panic is captured and propagated after all work settles.

##### Dynamic Semantics

**Single Panic**

1. The panicking work item captures panic information.
2. Other work items continue to completion (or cancellation if a token is attached).
3. After all work settles, the panic is re-raised at the block boundary.

**Multiple Panics**

1. Each panic is captured independently.
2. All work completes or is cancelled.
3. The first panic (by completion order) is raised.
4. Other panics are discarded; implementations MAY log them.

**Panic and Cancellation**

Implementations MAY request cancellation on first panic to expedite block completion. This behavior is Implementation-Defined.

**Catching Panics**

To handle panics within the block without propagation:

```cursive
let results = parallel ctx.cpu() {
    spawn { catch_panic(|| risky_work_a()) }
    spawn { catch_panic(|| risky_work_b()) }
}
// results: (Result<A, PanicInfo>, Result<B, PanicInfo>)
```

---

### 14.11 Nested Parallelism

##### Definition

Parallel blocks may be nested. Inner blocks execute within the context established by outer blocks.

##### Dynamic Semantics

**CPU Nesting**

Inner CPU parallel blocks share the worker pool with outer blocks. The `workers` parameter on inner blocks is a hint or limit, not additional workers.

```cursive
parallel ctx.cpu(workers: 8) {
    dispatch i in 0..4 {
        parallel ctx.cpu(workers: 2) {
            dispatch j in 0..100 {
                #data[i][j] write {
                    data[i][j] = compute(i, j)
                }
            }
        }
    }
}
```

**Heterogeneous Nesting**

CPU and GPU blocks may be nested:

```cursive
parallel ctx.cpu(workers: 4) {
    spawn {
        let preprocessed = cpu_preprocess(data)
        
        parallel ctx.gpu() {
            dispatch i in 0..n {
                gpu_process(preprocessed, i)
            }
        }
        
        cpu_postprocess(preprocessed)
    }
}
```

##### Constraints & Legality

**Nesting Constraints**

1. GPU parallel blocks MUST NOT be nested inside other GPU parallel blocks.
2. Inner CPU blocks share the outer block's worker pool.
3. Capture rules apply independently at each nesting level.

---

### 14.12 Determinism

##### Definition

**Determinism** guarantees that given identical inputs and parallel structure, execution produces identical results.

##### Static Semantics

**Sources of Non-Determinism**

| Source                       | Mitigation                              |
| :--------------------------- | :-------------------------------------- |
| Spawn execution order        | Results collected in declaration order  |
| Dispatch iteration order     | Key partitioning is deterministic       |
| Reduction combination order  | Tree-based deterministic reduction      |
| Floating-point reassociation | `[ordered]` forces sequential execution |

**Deterministic Dispatch**

Dispatch is deterministic when:

1. Key patterns produce identical partitioning across runs.
2. Iterations with the same key execute in index order.
3. Reduction uses deterministic tree combination.

**Ordered Dispatch**

The `[ordered]` attribute forces sequential side-effect ordering:

```cursive
parallel ctx.cpu() {
    dispatch i in 0..n [ordered] {
        println(f"Processing {i}")  // Prints 0, 1, 2, ... in order
    }
}
```

Iterations MAY execute in parallel, but side effects (I/O, shared mutation) are buffered and applied in index order.

---

### 14.13 Memory Allocation in Parallel Blocks

##### Definition

Work items may allocate memory using captured allocator capabilities.

##### Dynamic Semantics

**Captured Allocator**

```cursive
parallel ctx.cpu() {
    spawn {
        let buffer = ctx.heap~>alloc::<[u8; 1024]>()
        use(buffer)
    }
}
```

**Region Allocation**

```cursive
region work_arena {
    parallel ctx.cpu() {
        dispatch i in 0..n {
            let temp = ^work_arena compute_result(i)
            process(temp)
        }
    }
}
```

---

### 14.14 Diagnostic Summary

##### Definition

This section consolidates all diagnostic codes defined in Clause 14.

**Error Codes**

| Code         | Section | Condition                                     |
| :----------- | :------ | :-------------------------------------------- |
| `E-PAR-0001` | §14.2   | `spawn` or `dispatch` outside parallel block  |
| `E-PAR-0002` | §14.2   | Domain expression not ExecutionDomain         |
| `E-PAR-0003` | §14.2   | Invalid domain parameter type                 |
| `E-PAR-0020` | §14.3   | Implicit capture of `unique` binding          |
| `E-PAR-0021` | §14.3   | Move of already-moved binding                 |
| `E-PAR-0022` | §14.3   | Move of binding from outer parallel scope     |
| `E-PAR-0030` | §14.4   | Invalid spawn attribute type                  |
| `E-PAR-0031` | §14.4   | `spawn` in escaping closure                   |
| `E-PAR-0040` | §14.5   | Dispatch outside parallel block               |
| `E-PAR-0041` | §14.5   | Key inference failed                          |
| `E-PAR-0042` | §14.5   | Cross-iteration dependency detected           |
| `E-PAR-0043` | §14.5   | Non-associative reduction without `[ordered]` |
| `E-PAR-0050` | §14.6.2 | Host memory access in GPU code                |
| `E-PAR-0051` | §14.6.2 | `shared` capture in GPU dispatch              |
| `E-PAR-0052` | §14.6.2 | Nested GPU parallel block                     |

**Warning Codes**

| Code         | Section | Condition                                  |
| :----------- | :------ | :----------------------------------------- |
| `W-PAR-0040` | §14.5   | Dynamic key pattern; runtime serialization |
| `W-PAR-0041` | §14.5   | Large capture in dispatch iteration        |
| `W-PAR-0042` | §14.4   | Spawn result not used                      |

---

### 14.15 Cross-Reference Notes [INFORMATIVE]

**Terms Defined in This Clause**

| Term                   | Section | Description                                         |
| :--------------------- | :------ | :-------------------------------------------------- |
| Structured parallelism | §14.1   | Concurrency model with bounded work lifetime        |
| Work item              | §14.1   | Unit of computation queued for execution            |
| Worker                 | §14.1   | Execution context that executes work items          |
| Task                   | §14.1   | Runtime representation of executing work item       |
| Parallel block         | §14.2   | Scoped concurrent execution region                  |
| Capture                | §14.3   | Binding reference from enclosing scope in work item |
| Spawn                  | §14.4   | Work item creation expression                       |
| SpawnHandle            | §14.4   | Handle to pending spawn result                      |
| Dispatch               | §14.5   | Data-parallel iteration expression                  |
| Execution domain       | §14.6   | Capability providing computational resources        |
| CancelToken            | §14.8   | Cooperative cancellation mechanism                  |

**Terms Referenced from Other Clauses**

| Term                | Source | Usage in This Clause                |
| :------------------ | :----- | :---------------------------------- |
| Key                 | §13.1  | Synchronization for `shared` access |
| Key prohibition     | §13.8  | Keys cannot span `wait` expressions |
| `shared` permission | §4.5   | Concurrent access permission        |
| `const` permission  | §4.5   | Immutable shared access             |
| `unique` permission | §4.5   | Exclusive access requiring move     |
| `Async<T>`          | §15.2  | Async operation type                |
| `wait`              | §15.4  | Suspension until async completion   |
| Region              | §3.3   | Scoped memory allocation            |
| Context             | §8.9   | Root capability record              |
| `#` key block       | §13.4  | Explicit key acquisition construct  |

---

### 14.16 Examples [INFORMATIVE]

**Basic Parallel Computation**

```cursive
procedure parallel_sum(data: const [f64], ctx: Context) -> f64 {
    parallel ctx.cpu(workers: 4) {
        dispatch i in 0..data.len() [reduce: +] {
            data[i]
        }
    }
}
```

**Game Frame Update**

```cursive
procedure game_frame(game: shared GameState, ctx: Context) {
    let input = gather_input(ctx.input)
    
    parallel ctx.cpu(workers: 6) {
        spawn [affinity: CpuSet::cores([0, 1])] {
            #game.physics write {
                step_physics(game.physics, input, dt: 1.0 / 60.0)
            }
        }
        
        spawn [affinity: CpuSet::cores([2, 3])] {
            dispatch i in 0..game.ai_agents.len() {
                #game.ai_agents[i] write {
                    game.ai_agents[i].think(game.world)
                }
            }
        }
        
        spawn [affinity: CpuSet::cores([4, 5])] {
            dispatch i in 0..game.entities.len() {
                #game.entities[i] write {
                    game.entities[i].update(input)
                }
            }
        }
    }
    
    ctx.display~>present()
}
```

**Data Processing Pipeline**

```cursive
procedure process_files(files: const [Path], ctx: Context) -> [Result | Error] {
    let results: shared [Result | Error] = ctx.heap~>alloc_slice(files.len())
    
    parallel ctx.cpu(workers: 8) {
        dispatch i in 0..files.len() {
            let raw = match wait ctx.fs~>read(files[i]) {
                data: [u8] => data,
                err: IoError => {
                    #results[i] write { results[i] = Error::Io(err) }
                    return
                }
            }
            
            let parsed = parse(raw)
            
            let enhanced = parallel ctx.gpu() {
                let d_data: GpuBuffer<f32> = gpu::alloc(parsed.len())
                gpu::upload(parsed, d_data)
                
                dispatch j in 0..parsed.len() {
                    d_data[j] = gpu_enhance(d_data[j])
                }
                
                let result = ctx.heap~>alloc_slice(parsed.len())
                gpu::download(d_data, result)
                result
            }
            
            #results[i] write { results[i] = Result::Ok(enhanced) }
        }
    }
    
    result results
}
```

**Cancellable Search with Timeout**

```cursive
procedure search_with_timeout(
    query: Query,
    shards: const [Shard],
    timeout: Duration,
    ctx: Context
) -> SearchResult | Timeout {
    let token = CancelToken::new()
    let results: shared [SearchResult | None] = ctx.heap~>alloc_slice(shards.len())
    
    spawn {
        wait ctx.time~>after(timeout)
        token.cancel()
    }
    
    parallel ctx.cpu(workers: 4) [cancel: token] {
        dispatch i in 0..shards.len() {
            if token.is_cancelled() {
                #results[i] write { results[i] = None }
                return
            }
            
            let r = search_shard(shards[i], query)
            
            #results[i] write { results[i] = Some(r) }
        }
    }
    
    if token.is_cancelled() {
        result Timeout
    } else {
        result merge(results)
    }
}
```

---

## Clause 15: Asynchronous Operations

### 15.1 Overview [INFORMATIVE]

##### Definition

Cursive provides asynchronous programming through the `Async<T>` modal type and the `wait` expression. This model unifies I/O waiting and condition-waiting into a single coherent abstraction.

> **Rationale:** Traditional async models (futures, promises, async/await) introduce complexity through poll-based state machines, waker contexts, and special syntax. Cursive's approach leverages its existing modal type system to represent async state explicitly, and unifies all forms of waiting—whether for I/O completion or condition satisfaction—under a single `wait` expression.

**Design Principles:**

1. **Unified waiting** — The `wait` expression operates exclusively on `Async<T>` values. Condition-waiting is expressed via methods that return `Async<T>`, unifying it with I/O async.
2. **Explicit state** — The `Async<T>` modal type makes pending/ready/failed states explicit in the type system.
3. **Composable** — Async operations compose via the `=>` pipeline operator, `wait race` for racing, and `wait all` for concurrent collection.
4. **Cancellation by drop** — Dropping an `Async@Pending` value cancels the underlying operation.
5. **Key system integration** — Keys MUST NOT be held across `wait` suspension points (§13.8).

---

### 15.2 The Async<T> Modal Type

##### Definition

An **Async<T>** is a modal type representing an operation that produces a value of type `T` upon completion. The operation may be in progress, completed successfully, or failed.

**Formal Definition:**

$$\text{Async}\langle T \rangle ::= @\text{Pending} \mid @\text{Ready}\{T\} \mid @\text{Failed}\{\text{Error}\}$$

##### Syntax & Declaration

**Grammar:**

```ebnf
async_type       ::= 'Async' '<' type '>'
async_state_type ::= async_type '@' async_state
async_state      ::= 'Pending' | 'Ready' | 'Failed'
```

**Declaration:**

```cursive
modal Async<T> {
    @Pending { }

    @Ready {
        value: T,
    }

    @Failed {
        error: IoError,
    }
}
```

##### Static Semantics

**State Semantics:**

| State      | Meaning                                        | Accessible Fields |
| :--------- | :--------------------------------------------- | :---------------- |
| `@Pending` | Operation in progress; value not yet available | (none)            |
| `@Ready`   | Operation completed successfully               | `value: T`        |
| `@Failed`  | Operation failed with error                    | `error: IoError`  |

**Typing Rules:**

$$\frac{\Gamma \vdash T : \text{Type}}{\Gamma \vdash \text{Async}\langle T \rangle : \text{Type}} \quad \text{(T-Async-Type)}$$

$$\frac{\Gamma \vdash e : \text{Async}\langle T \rangle@\text{Ready}}{\Gamma \vdash e.\text{value} : T} \quad \text{(T-Async-Ready-Access)}$$

$$\frac{\Gamma \vdash e : \text{Async}\langle T \rangle@\text{Failed}}{\Gamma \vdash e.\text{error} : \text{IoError}} \quad \text{(T-Async-Failed-Access)}$$

**State Transitions:**

An `Async<T>` value transitions exactly once from `@Pending` to either `@Ready` or `@Failed`. Once in a terminal state, no further transitions occur.

$$@\text{Pending} \xrightarrow{\text{success}} @\text{Ready}\{v\}$$
$$@\text{Pending} \xrightarrow{\text{failure}} @\text{Failed}\{e\}$$

##### Dynamic Semantics

**Evaluation:**

When an I/O operation or condition-wait method is invoked, the implementation:

1. Initiates the underlying operation (I/O request, condition registration, etc.)
2. Returns an `Async<T>@Pending` value immediately
3. The operation progresses independently of program execution
4. Upon completion, the runtime transitions the value to `@Ready` or `@Failed`

**Cancellation:**

When an `Async<T>@Pending` value is dropped (goes out of scope without being waited on), the implementation MUST cancel the underlying operation if cancellation is possible. The implementation MAY allow the operation to complete if cancellation is not feasible.

##### Memory & Layout

**Representation:**

An `Async<T>` value consists of:

1. A discriminant indicating the current state (Pending, Ready, or Failed)
2. State-specific payload:
   - `@Pending`: Implementation-defined handle to the pending operation
   - `@Ready`: The value of type `T`
   - `@Failed`: The error value

**Size:** `sizeof(Async<T>)` is implementation-defined but MUST be at least `max(sizeof(T), sizeof(IoError)) + sizeof(discriminant)`.

##### Constraints & Legality

**Negative Constraints:**

1. Direct field access on `Async<T>` without state qualification is ill-formed.
2. Accessing `.value` on `@Pending` or `@Failed` is ill-formed.
3. Accessing `.error` on `@Pending` or `@Ready` is ill-formed.

**Diagnostic Table:**

| Code           | Severity | Condition                              | Detection    | Effect    |
| :------------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0010` | Error    | Field access on unqualified `Async<T>` | Compile-time | Rejection |
| `E-ASYNC-0011` | Error    | `.value` access on non-`@Ready` state  | Compile-time | Rejection |
| `E-ASYNC-0012` | Error    | `.error` access on non-`@Failed` state | Compile-time | Rejection |

---

### 15.3 Creating Async Values

##### Definition

Async values are created by invoking I/O operations on capability objects or by calling condition-wait methods on shared data. The operation begins immediately upon invocation; the returned `Async<T>@Pending` value represents a handle to the in-progress operation.

##### Syntax & Declaration

**I/O Operations:**

I/O capability methods return `Async<T>`:

```cursive
let read_op: Async<Data> = ctx.io~>read(file)
let fetch_op: Async<Response> = ctx.net~>get(url)
let timer_op: Async<()> = ctx.time~>after(5.seconds)
```

**Condition-Wait Methods:**

Shared types provide the `when` method that returns `Async<T>`:

```cursive
let item: Async<Task> = queue~>when(
    |q| !q.items~>is_empty(),
    |q| q.items~>pop()
)
```

##### Static Semantics

**Typing Rules:**

For I/O operations:

$$\frac{\Gamma \vdash \text{cap} : \text{Capability} \quad \Gamma \vdash \text{op} : \text{Op}\langle T \rangle}{\Gamma \vdash \text{cap}{\sim}{>}\text{op}(\ldots) : \text{Async}\langle T \rangle@\text{Pending}} \quad \text{(T-IO-Async)}$$

For condition-wait methods (see §15.7):

$$\frac{\Gamma \vdash e : \text{shared } T \quad \Gamma \vdash p : T \to \text{bool} \quad \Gamma \vdash a : T \to R}{\Gamma \vdash e{\sim}{>}\text{when}(p, a) : \text{Async}\langle R \rangle@\text{Pending}} \quad \text{(T-When-Async)}$$

##### Dynamic Semantics

**I/O Operation Initiation:**

1. The runtime initiates the I/O request with the operating system or external service
2. An `Async<T>@Pending` handle is returned immediately
3. The operation progresses concurrently with program execution
4. Upon I/O completion, the handle transitions to `@Ready` or `@Failed`

**Condition-Wait Initiation:**

1. The runtime acquires the key for the shared data
2. The predicate is evaluated
3. If true: the action executes atomically, the key is released, and the handle transitions to `@Ready`
4. If false: the runtime registers the handle as a waiter, releases the key, and the handle remains `@Pending`
5. When any task releases the key, registered waiters are notified to re-evaluate

---

### 15.4 The wait Expression

##### Definition

The `wait` expression suspends the current task until an `Async<T>` value transitions out of the `@Pending` state. Upon completion, `wait` returns the result as `T | ErrorType`, where `ErrorType` is the error type from `@Failed`.

##### Syntax & Declaration

**Grammar:**

```ebnf
wait_expr ::= 'wait' expression
```

**Forms:**

```cursive
// Basic wait — returns T | IoError
let result = wait ctx.io~>read(file)

// With error propagation
let data = wait ctx.io~>read(file)?

// With pattern matching
match wait ctx.io~>read(file) {
    data: Data => process(data),
    err: IoError => handle(err),
}
```

##### Static Semantics

**Typing Rule:**

$$\frac{\Gamma \vdash e : \text{Async}\langle T \rangle \quad \text{ErrorType} = \text{error\_type}(e)}{\Gamma \vdash \texttt{wait } e : T \mid \text{ErrorType}} \quad \text{(T-Wait)}$$

Where `error_type(e)` extracts the error type from the `@Failed` state of the Async type (typically `IoError`).

**Key Prohibition:**

The `wait` expression is a suspension point. Keys MUST NOT be held when `wait` is evaluated. This constraint is enforced by the key system (§13.8).

$$\frac{\Gamma \vdash e : \text{Async}\langle T \rangle \quad \text{held\_keys}(\Gamma) = \emptyset}{\Gamma \vdash \texttt{wait } e : T \mid \text{ErrorType}} \quad \text{(T-Wait-No-Keys)}$$

##### Dynamic Semantics

**Evaluation:**

Let $e$ be an expression of type `Async<T>`. Evaluation of `wait e` proceeds as follows:

1. Evaluate $e$ to obtain an `Async<T>` value $a$
2. If $a$ is in state `@Ready{value: v}`: return $v$
3. If $a$ is in state `@Failed{error: err}`: return $err$
4. If $a$ is in state `@Pending`:
   a. Suspend the current task
   b. Register the task to be resumed when $a$ transitions
   c. Upon transition, resume and proceed to step 2 or 3

**Suspension Semantics:**

When a task suspends at a `wait` expression:

1. The task's execution state is saved
2. Control returns to the runtime scheduler
3. Other tasks may execute while this task is suspended
4. When the awaited `Async` completes, the runtime schedules the task for resumption

##### Constraints & Legality

**Negative Constraints:**

1. `wait` MUST NOT be used on non-`Async` types
2. Keys MUST NOT be held at `wait` suspension points (see §13.8)

**Diagnostic Table:**

| Code           | Severity | Condition                | Detection    | Effect    |
| :------------- | :------- | :----------------------- | :----------- | :-------- |
| `E-ASYNC-0020` | Error    | `wait` on non-Async type | Compile-time | Rejection |
| `E-KEY-0050`   | Error    | Key held across `wait`   | Compile-time | Rejection |

---

### 15.5 Async Pipelines with =>

##### Definition

The pipeline operator `=>` chains asynchronous operations. When applied to an `Async<T>` value, `=>` creates a new `Async<U>` that, upon the original's completion with `@Ready`, applies the provided closure to produce the next async operation.

##### Syntax & Declaration

**Grammar:**

```ebnf
async_pipeline ::= expression '=>' closure_expr
closure_expr   ::= '|' pattern '|' expression
```

**Example:**

```cursive
let pipeline = ctx.io~>read(file)
    => |data| ctx.io~>parse(data)
    => |parsed| ctx.db~>save(parsed)

let result = wait pipeline?
```

##### Static Semantics

**Typing Rule:**

$$\frac{\Gamma \vdash e_1 : \text{Async}\langle T \rangle \quad \Gamma, x : T \vdash e_2 : \text{Async}\langle U \rangle}{\Gamma \vdash e_1 \Rightarrow |x| \; e_2 : \text{Async}\langle U \rangle} \quad \text{(T-Async-Pipeline)}$$

**Error Type Propagation:**

The error type of the resulting `Async` is the union of error types from all stages:

$$\text{error\_type}(e_1 \Rightarrow |x| \; e_2) = \text{error\_type}(e_1) \cup \text{error\_type}(e_2)$$

##### Dynamic Semantics

**Evaluation:**

Given `a => |x| f(x)` where `a : Async<T>` and `f : T -> Async<U>`:

1. An `Async<U>@Pending` is returned immediately
2. When `a` transitions:
   - If `@Ready{value: v}`: evaluate `f(v)`, chain to the result
   - If `@Failed{error: e}`: short-circuit to `@Failed{error: e}`
3. When the chained operation completes, the outer `Async` transitions accordingly

**Short-Circuit Behavior:**

If any stage in a pipeline fails, the entire pipeline immediately transitions to `@Failed` with that error. Subsequent stages are not executed.

##### Constraints & Legality

**Negative Constraints:**

1. The left operand of `=>` MUST be an `Async<T>` type
2. The closure MUST return an `Async<U>` type

**Diagnostic Table:**

| Code           | Severity | Condition                          | Detection    | Effect    |
| :------------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-ASYNC-0030` | Error    | `=>` left operand not Async        | Compile-time | Rejection |
| `E-ASYNC-0031` | Error    | `=>` closure does not return Async | Compile-time | Rejection |

---

### 15.6 Racing with `wait race`

##### Definition

The `wait race` expression concurrently waits on multiple `Async` values and proceeds with the first to complete. Operations that do not complete first are cancelled.

##### Syntax & Declaration

**Grammar:**

```ebnf
wait_race_expr ::= 'wait' 'race' '{' wait_race_arm (',' wait_race_arm)* '}'
wait_race_arm  ::= expression '=>' '|' pattern '|' expression
```

**Example:**

```cursive
wait race {
    ctx.net~>get(url) => |response| Ok(response),
    ctx.time~>after(5.seconds) => |_| Err(Timeout),
}
```

##### Static Semantics

**Typing Rule:**

All arms MUST produce the same result type:

$$\frac{\Gamma \vdash e_i : \text{Async}\langle T_i \rangle \quad \Gamma, x_i : T_i \vdash r_i : R \quad \forall i}{\Gamma \vdash \texttt{wait race } \{ e_1 \Rightarrow |x_1| \; r_1, \ldots \} : R} \quad \text{(T-Wait-First)}$$

##### Dynamic Semantics

**Evaluation:**

1. All async expressions $e_1, \ldots, e_n$ are evaluated, initiating their operations concurrently
2. The runtime monitors all operations for completion
3. When the first operation completes (say $e_k$ with result $v_k$):
   a. All other operations are cancelled
   b. The corresponding handler $r_k$ is evaluated with $x_k = v_k$
   c. The result of the handler becomes the result of `wait race`
4. If an operation fails, it is treated as completing with `@Failed`

**Cancellation:**

Operations that do not complete first MUST be cancelled. The implementation SHALL make a best-effort attempt to abort in-flight I/O. Condition-wait operations are deregistered from their wait lists.

##### Constraints & Legality

**Negative Constraints:**

1. `wait race` MUST have at least two arms
2. All handler expressions MUST produce the same type

**Diagnostic Table:**

| Code           | Severity | Condition                                | Detection    | Effect    |
| :------------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0040` | Error    | `wait race` with fewer than 2 arms       | Compile-time | Rejection |
| `E-ASYNC-0041` | Error    | `wait race` arms have incompatible types | Compile-time | Rejection |

---

### 15.7 Concurrent Collection with wait all

##### Definition

The `wait all` expression concurrently waits on multiple `Async` values and returns all results as a tuple when all complete. If any operation fails, the entire expression fails.

##### Syntax & Declaration

**Grammar:**

```ebnf
wait_all_expr ::= 'wait' 'all' '{' expression (',' expression)* '}'
```

**Example:**

```cursive
let (a, b) = wait all {
    ctx.io~>read(file_a),
    ctx.io~>read(file_b),
}?
```

##### Static Semantics

**Typing Rule:**

$$\frac{\Gamma \vdash e_1 : \text{Async}\langle T_1 \rangle \quad \cdots \quad \Gamma \vdash e_n : \text{Async}\langle T_n \rangle}{\Gamma \vdash \texttt{wait all } \{ e_1, \ldots, e_n \} : (T_1, \ldots, T_n) \mid \text{ErrorType}} \quad \text{(T-Wait-All)}$$

Where `ErrorType` is the union of all error types.

##### Dynamic Semantics

**Evaluation:**

1. All async expressions are evaluated, initiating operations concurrently
2. The runtime waits for all operations to complete
3. If all complete with `@Ready`: return a tuple of all values
4. If any completes with `@Failed`: cancel remaining operations and return the error

**Failure Semantics:**

On first failure, remaining in-progress operations are cancelled. The error from the first failing operation is returned.

##### Constraints & Legality

**Negative Constraints:**

1. `wait all` MUST have at least one expression

**Diagnostic Table:**

| Code           | Severity | Condition                        | Detection    | Effect    |
| :------------- | :------- | :------------------------------- | :----------- | :-------- |
| `E-ASYNC-0050` | Error    | `wait all` with zero expressions | Compile-time | Rejection |

---

### 15.8 Condition-Waiting

##### Definition

Condition-waiting is the pattern of waiting until a predicate on shared data becomes true, then executing an action atomically. In Cursive, condition-waiting is expressed via the `when` method, which returns an `Async<R>`. This unifies condition-waiting with I/O async under the same `wait` abstraction.

##### Syntax & Declaration

**The `when` Method:**

Shared types provide the `when` method:

```cursive
procedure when<R>(~%, predicate: (const Self) -> bool, action: (unique Self) -> R) -> Async<R>
```

**Example:**

```cursive
// Wait for queue non-empty, pop atomically
let item = wait queue~>when(
    |q| !q.items~>is_empty(),
    |q| q.items~>pop()
)?
```

**Common Helpers:**

Types MAY provide convenience methods that wrap `when`:

```cursive
// Equivalent to queue~>when(|q| !q.items~>is_empty(), |q| q.items~>pop())
let item = wait queue~>pop_async()?

// Wait for a boolean flag to become true
wait shutdown~>until_true()
```

##### Static Semantics

**Typing Rule:**

$$\frac{\Gamma \vdash e : \text{shared } T \quad \Gamma \vdash p : (\text{const } T) \to \text{bool} \quad \Gamma \vdash a : (\text{unique } T) \to R}{\Gamma \vdash e{\sim}{>}\text{when}(p, a) : \text{Async}\langle R \rangle@\text{Pending}} \quad \text{(T-When)}$$

##### Dynamic Semantics

**Evaluation of `when(predicate, action)`:**

1. Return an `Async<R>@Pending` immediately
2. The runtime executes the following loop:
   a. Acquire the key for the shared data
   b. Evaluate `predicate(self)` with `const` access
   c. If true:
      - Evaluate `action(self)` with `unique` access
      - Release the key
      - Transition to `@Ready{value: result}`
      - Exit loop
   d. If false:
      - Register this `Async` handle as a waiter for the key
      - Release the key
      - Suspend until notified
      - On notification, goto step (a)

**Wake Notification:**

Waiters are notified when any task releases the key for the shared data. Upon notification, the waiter re-acquires the key and re-evaluates the predicate. If still false, it re-registers and suspends again.

> **Note:** This is not busy-waiting. The runtime efficiently suspends the operation until a relevant key release occurs.

##### Constraints & Legality

**Negative Constraints:**

1. The receiver MUST have `shared` permission
2. The predicate closure MUST NOT mutate the data
3. The action closure receives `unique` access and MAY mutate

**Diagnostic Table:**

| Code           | Severity | Condition                     | Detection    | Effect    |
| :------------- | :------- | :---------------------------- | :----------- | :-------- |
| `E-ASYNC-0060` | Error    | `when` on non-shared receiver | Compile-time | Rejection |
| `E-ASYNC-0061` | Error    | Predicate mutates shared data | Compile-time | Rejection |

---

### 15.9 Cancellation Semantics

##### Definition

Cancellation is the mechanism by which an in-progress async operation is aborted. Cursive uses **drop-based cancellation**: when an `Async<T>@Pending` value goes out of scope without being waited on, the underlying operation is cancelled.

##### Dynamic Semantics

**Drop Behavior:**

When `drop` is invoked on an `Async<T>@Pending`:

1. The runtime attempts to cancel the underlying operation
2. For I/O operations: the OS request is aborted if possible
3. For condition-waits: the waiter is deregistered from the wait list
4. Resources associated with the operation are released

**Racing Cancellation:**

In `wait race`, when one operation completes:

1. All other operations are immediately cancelled
2. Their resources are cleaned up
3. Only the winning operation's result is used

**Best-Effort Cancellation:**

Cancellation is best-effort. Some operations (e.g., a write that has already been submitted to the OS) may complete despite cancellation. The result is discarded.

##### Constraints & Legality

**No Explicit Cancel Method:**

Cursive does not provide an explicit `cancel()` method on `Async`. Cancellation is achieved by dropping the handle:

```cursive
let op = ctx.net~>get(url)
if should_cancel {
    return  // op is dropped here, operation cancelled
}
wait op?
```

---

### 15.10 Async Procedures

##### Definition

A procedure that performs asynchronous operations may either:

1. Return `Async<T>` for the caller to wait
2. Wait internally and return the final result

##### Syntax & Declaration

**Returning Async<T>:**

```cursive
procedure fetch_data(url: Url, ctx: Context) -> Async<Data> {
    result ctx.net~>get(url)
        => |r| ctx.io~>parse(r.body)
}

// Caller waits:
let data = wait fetch_data(url, ctx)?
```

**Waiting Internally:**

```cursive
procedure fetch_data(url: Url, ctx: Context) -> Data | IoError {
    let response = wait ctx.net~>get(url)?
    let parsed = wait ctx.io~>parse(response.body)?
    result parsed
}
```

##### Static Semantics

**No Special `async` Keyword:**

Cursive does not use an `async` keyword for procedure declarations. A procedure that returns `Async<T>` is an async procedure by virtue of its return type. A procedure that uses `wait` internally is synchronous from the caller's perspective (it blocks until all internal waits complete).

> **Rationale:** The `Async<T>` return type makes async behavior explicit. Internal `wait` expressions are suspension points within the procedure but do not affect the procedure's signature.

---

### 15.11 Cross-Reference Notes [INFORMATIVE]

##### Terms Defined in This Clause

| Term                    | Section |
| :---------------------- | :------ |
| Async<T> modal type     | §15.2   |
| wait expression         | §15.4   |
| Async pipeline (=>)     | §15.5   |
| wait race               | §15.6   |
| wait all                | §15.7   |
| when method             | §15.8   |
| Drop-based cancellation | §15.9   |

##### Terms Referenced from Other Clauses

| Term                              | Defined In |
| :-------------------------------- | :--------- |
| Modal type                        | §5.6       |
| shared permission                 | §9.4       |
| Key                               | §13.1      |
| Key prohibition across suspension | §13.8      |
| parallel block                    | §14.1      |
| IoError                           | §18.2      |

##### Diagnostic Codes Defined in This Clause

| Code         | Condition                              |
| :----------- | :------------------------------------- |
| E-ASYNC-0010 | Field access on unqualified Async<T>   |
| E-ASYNC-0011 | .value access on non-@Ready state      |
| E-ASYNC-0012 | .error access on non-@Failed state     |
| E-ASYNC-0020 | wait on non-Async type                 |
| E-ASYNC-0030 | => left operand not Async              |
| E-ASYNC-0031 | => closure does not return Async       |
| E-ASYNC-0040 | wait race with fewer than 2 arms       |
| E-ASYNC-0041 | wait race arms have incompatible types |
| E-ASYNC-0050 | wait all with zero expressions         |
| E-ASYNC-0060 | when on non-shared receiver            |
| E-ASYNC-0061 | Predicate mutates shared data          |

##### Related Diagnostic Codes from Other Clauses

| Code       | Clause | Condition                             |
| :--------- | :----- | :------------------------------------ |
| E-KEY-0050 | §13.8  | Key held across wait suspension point |

---

## Clause 16: Coroutines

## Clause 17: Channels

### 17.1 Condition-Waiting [INFORMATIVE]

##### Definition

Condition-waiting—the pattern of waiting until a predicate on shared data becomes true, then executing an action atomically—is now expressed via the `when` method, which returns `Async<R>`. This is specified in §15.8.

> **Note:** Previous versions of Cursive used a `wait <condition> then { body }` syntax. This has been unified with I/O async under the `Async<T>` modal type and `wait` expression. See §15.8 for the current specification.

**Example (current syntax):**

```cursive
let queue: shared Queue<T> = ...

// Wait for non-empty, pop atomically
let item = wait queue~>when(
    |q| !q.items~>is_empty(),
    |q| q.items~>pop()
)?

// Or using a convenience method
let item = wait queue~>pop_async()?
```

**With Timeout:**

```cursive
wait race {
    queue~>pop_async() => |item| Ok(item),
    ctx.time~>after(5.seconds) => |_| Err(Timeout),
}
```

---

### 17.2 Channels

##### Definition

A **channel** provides ownership transfer between tasks without shared access. Data moves from producer to consumer; the data is `unique` throughout and never requires key synchronization.

Channels are distinct from the key-based synchronization model:

| Mechanism  | Access Pattern                    | Synchronization         |
| :--------- | :-------------------------------- | :---------------------- |
| Keys (§13) | Multiple tasks access shared data | Key acquisition/release |
| Channels   | Data moves between tasks          | Ownership transfer      |

##### Syntax & Declaration

**Grammar:**

```ebnf
channel_create    ::= 'Channel' '::' 'new' '<' type '>' '(' [capacity] ')'
oneshot_create    ::= 'Oneshot' '::' 'new' '<' type '>' '(' ')'
```

**Channel Creation:**

```cursive
// Unbounded channel — grows as needed, send never blocks
let (tx, rx) = Channel::new::<Data>()

// Bounded channel — send blocks when full
let (tx, rx) = Channel::new::<Data>(capacity)

// Oneshot channel — single value, single send
let (tx, rx) = Oneshot::new::<Data>()
```

##### Static Semantics

**Channel Types:**

```cursive
record Sender<T> { /* implementation-defined */ }
record Receiver<T> { /* implementation-defined */ }
```

**Typing Rules:**

$$\frac{}{\Gamma \vdash \text{Channel::new}\langle T \rangle() : (\text{Sender}\langle T \rangle, \text{Receiver}\langle T \rangle)} \quad \text{(T-Channel-Create)}$$

$$\frac{\Gamma \vdash tx : \text{Sender}\langle T \rangle \quad \Gamma \vdash v : \text{unique } T}{\Gamma \vdash tx{\sim}{>}\text{send}(\texttt{move } v) : \text{Async}\langle () \rangle} \quad \text{(T-Channel-Send)}$$

$$\frac{\Gamma \vdash rx : \text{Receiver}\langle T \rangle}{\Gamma \vdash rx{\sim}{>}\text{recv}() : \text{Async}\langle T \rangle} \quad \text{(T-Channel-Recv)}$$

**Ownership Transfer:**

The `send` operation consumes the value (requires `move`). The `recv` operation produces a `unique` value. Data is never shared between sender and receiver.

##### Dynamic Semantics

**Channel Operations:**

All channel operations return `Async<T>` and integrate with the unified `wait` model:

```cursive
// Send — suspends until space available (bounded) or enqueues (unbounded)
wait tx~>send(move value)?

// Receive — suspends until value available
let data = wait rx~>recv()?
```

**Non-Blocking Variants:**

```cursive
// Try send — returns immediately
match tx~>try_send(move value) {
    Ok(()) => /* sent */,
    Err(ChannelFull(value)) => /* channel full, value returned */,
}

// Try receive — returns immediately
match rx~>try_recv() {
    Ok(value) => /* received */,
    Err(ChannelEmpty) => /* no value available */,
}
```

**Close Semantics:**

When a `Sender` is dropped:
1. The channel is marked as closed
2. Pending `recv` operations return `ChannelClosed` error
3. Existing buffered values remain available for receive

When a `Receiver` is dropped:
1. The channel is marked as disconnected
2. Pending `send` operations return `ChannelDisconnected` error
3. Buffered values are dropped

##### Memory & Layout

**Representation:**

Channel implementation is implementation-defined. A conforming implementation MUST provide:

1. FIFO ordering for buffered values
2. Thread-safe access without external synchronization
3. Proper ownership transfer semantics (no aliasing of transferred data)

##### Constraints & Legality

**Negative Constraints:**

1. `send` MUST receive a `move` parameter (unique ownership transfer)
2. Sending on a closed channel is an error
3. Receiving from a disconnected channel with no buffered values is an error

**Diagnostic Table:**

| Code          | Severity | Condition                             | Detection    | Effect      |
| :------------ | :------- | :------------------------------------ | :----------- | :---------- |
| `E-CHAN-0010` | Error    | `send` without `move`                 | Compile-time | Rejection   |
| `E-CHAN-0020` | Error    | Send on closed channel                | Runtime      | Error value |
| `E-CHAN-0021` | Error    | Receive on disconnected empty channel | Runtime      | Error value |

---


## Clause 18: Memory Ordering

##### Definition

**Default: Sequential Consistency**

Key acquisition uses **acquire** semantics. Key release uses **release** semantics. Memory accesses default to **sequentially consistent** ordering.

This provides the strongest guarantees—all tasks observe operations in a consistent global order.

##### Syntax & Declaration

**Relaxed Ordering**

For performance-critical code, weaker orderings are available via annotation:

```cursive
[[relaxed]]
counter += 1

[[acquire]]
let ready = flag

[[release]]
flag = true
```

**Ordering Levels**

| Ordering  | Guarantee                             |
| :-------- | :------------------------------------ |
| `relaxed` | Atomicity only—no ordering            |
| `acquire` | Subsequent reads see prior writes     |
| `release` | Prior writes visible to acquire reads |
| `acq_rel` | Both acquire and release              |
| `seq_cst` | Total global order (default)          |

The annotation affects the memory operation, not the key synchronization. Keys always use acquire/release semantics.

**Fence Operations**

```cursive
fence(acquire)       // Acquire fence
fence(release)       // Release fence
fence(seq_cst)       // Full barrier
```

---

### 18.1 Implementation Model

##### Definition

**Zero-Cost Abstractions**

Keys are a compile-time concept. Runtime cost occurs only when necessary:

| Condition                              | Cost                          |
| :------------------------------------- | :---------------------------- |
| No concurrent access possible          | Zero—elided                   |
| Single-threaded context                | Zero—elided                   |
| Provably disjoint access               | Zero—parallel without sync    |
| `const` permission                     | Zero—immutable                |
| `unique` permission                    | Zero—exclusive                |
| `dispatch` iteration-indexed access    | Zero—proven disjoint          |
| Small primitive + potential contention | Atomic operation              |
| Compound type + potential contention   | Reader-writer synchronization |

**Key Implementation Strategy**

When runtime synchronization is needed:

| Data Type                   | Implementation               |
| :-------------------------- | :--------------------------- |
| Small primitive (≤ 8 bytes) | Atomic CAS / fetch-and-op    |
| Struct field                | Per-field or single RwLock   |
| Array (static index)        | Per-element or segment locks |
| Array (dynamic index)       | Lock keyed by address        |
| `#`-annotated boundary      | Single lock for subtree      |

**Key state storage is implementation-defined.** Implementations document their strategy in the Conformance Dossier.

##### Static Semantics

**Key Elision Analysis**

The compiler uses several analyses to prove key elision is safe:

*Escape Analysis:*
```cursive
procedure local_only() {
    let data: shared Data = Data { value: 0 }
    data.value = 100
}
// data never escapes—elide all key operations
```

*Sequential Region Analysis:*
```cursive
region as r {
    // No parallel block in this region
    let data: shared Data = r^Data::new()
    data.value = 100
}
// No concurrency possible—elide all key operations
```

*Disjoint Path Analysis:*
```cursive
parallel {
    spawn { player.health = 100 }
    spawn { player.mana = 50 }
}
// Paths are disjoint—elide key operations for both
```

---

# Part 5: Advanced Features

## Clause 19: Metaprogramming

This clause defines the Cursive metaprogramming system. Cursive provides a deterministic, declarative code generation mechanism based on **compile-time execution**, **type introspection**, **quasiquoting**, and **explicit AST emission**. This system adheres to the **Two-Phase Compilation Model** defined in §2.12. Metaprogramming occurs strictly between the **Parsing** and **Semantic Analysis** phases.

### 19.1 The Metaprogramming Model

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

### 19.2 AST Representation

##### Definition

**The AST Model**

The metaprogramming system operates on Abstract Syntax Tree (AST) node types. The following types **MUST** be available in the `comptime` context:

| Type   | Description                                                             |
| :----- | :---------------------------------------------------------------------- |
| `Node` | Base form for all syntax elements.                                      |
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

### 19.3 Type Introspection

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

### 19.4 Compile-Time Procedures and Blocks

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

---

### 19.5 Quasiquoting and Interpolation

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

---

### 19.6 Code Emission

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

---

### 19.7 Implementation Limits

##### Definition

Conforming implementations **MUST** enforce the following minimum limits for compile-time execution to ensure portability. These compile-time specific limits supplement the general implementation limits defined in §1.4; see Appendix I for the complete normative limit values.

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

### Clause 19: Metaprogramming Cross-Reference Notes

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

| Term              | Source | Usage in Clause 19                        |
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
| (none) | -           | Clause 19 is self-contained for metaprogramming |

---

## Clause 20: Interoperability

This clause defines the Foreign Function Interface (FFI) mechanisms for interacting with code adhering to C-compatible Application Binary Interfaces (ABIs). It specifies extern declaration syntax, the safety boundary enforced by `unsafe` blocks, memory layout guarantees via representation attributes, and the strict categorization of types permitted across the language boundary.

### 20.1 Foundational Definitions

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

### 20.2 FFI-Safe Types

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

6. **Repr-C Records**: If `record R` is annotated `[[layout(C)]]` and $\forall f \in \text{fields}(R), \text{IsFFISafe}(\text{type}(f))$, then $R \in \mathcal{F}$

7. **Repr-C Enums**: If `enum E` is annotated `[[layout(C)]]` or `[[layout(IntType)]]`, then $E \in \mathcal{F}$

**Explicit Exclusions**

The following types are expressly **excluded** from FFI signatures:

- Permission-qualified references (`unique T`, `const T`, `shared T`)
- Slices (`[T]`)
- Strings (`string`, `string@View`, `string@Owned`)
- Closures (non-extern function types that capture environment)
- Modal types not marked `[[layout(C)]]`
- Generic type parameters (must be monomorphized first)
- Form witness types (`witness Form`)

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
### 20.2.1 FFI String Types

##### Definition

**CString**

`CString` is a null-terminated, heap-allocated byte sequence for C interoperability. It provides safe construction from Cursive strings and extraction of raw pointers for FFI calls.

**CStr**

`CStr` is a borrowed view of a null-terminated byte sequence, analogous to `string@View` but for C-compatible strings.

##### Syntax & Declaration

**Type Definitions**

```cursive
/// Owned null-terminated string for FFI.
/// Implements Drop to deallocate when responsibility ends.
record CString <: Drop [[layout(C)]] {
    private ptr: *mut u8,
    private len: usize,       // Length excluding null terminator
    private cap: usize,       // Allocated capacity
    
    /// Creates a CString from a Cursive string view.
    /// Allocates memory and copies content with null terminator.
    /// Returns error if the string contains interior null bytes.
    procedure from_string(
        s: string@View,
        heap: witness HeapAllocator
    ) -> CString | NullByteError {
        // Scan for interior null bytes
        for byte in s~>bytes() {
            if byte == 0u8 {
                return NullByteError { position: /* index */ }
            }
        }
        // Allocate len + 1 bytes for null terminator
        let buffer = heap~>alloc::<u8>(s~>len() + 1)
        // Copy string data and append null terminator
        copy_bytes(s~>as_ptr(), buffer, s~>len())
        buffer[s~>len()] = 0u8
        CString { ptr: buffer, len: s~>len(), cap: s~>len() + 1 }
    }
    
    /// Creates a CString from a Cursive string, replacing interior
    /// nulls with a replacement character.
    procedure from_string_lossy(
        s: string@View,
        heap: witness HeapAllocator
    ) -> CString {
        // Implementation: replace interior nulls with '?'
    }
    
    /// Returns a raw pointer to the null-terminated content.
    /// Valid for the lifetime of the CString.
    procedure as_ptr(~) -> *imm u8 {
        self.ptr as *imm u8
    }
    
    /// Returns a mutable raw pointer.
    /// Valid for the lifetime of the CString.
    procedure as_mut_ptr(~!) -> *mut u8 {
        self.ptr
    }
    
    /// Converts to CStr view.
    procedure as_cstr(~) -> CStr {
        CStr { ptr: self.ptr as *imm u8, len: self.len }
    }
    
    /// Consumes the CString and transfers ownership of the raw pointer.
    /// Caller assumes responsibility for deallocation.
    /// Drop is suppressed via intrinsic.
    procedure into_raw(move self, heap: witness HeapAllocator) -> *mut u8 {
        let ptr = self.ptr
        intrinsic::forget(move self)  // Suppress destructor
        ptr
    }
    
    /// Reconstructs a CString from a raw pointer.
    /// UNSAFE: ptr must have been created by into_raw() with matching len/cap.
    unsafe procedure from_raw(ptr: *mut u8, len: usize, cap: usize) -> CString {
        CString { ptr, len, cap }
    }
    
    /// Returns the length in bytes (excluding null terminator).
    procedure len(~) -> usize {
        self.len
    }
    
    /// Drop implementation: deallocates the underlying buffer.
    procedure drop(~!, heap: witness HeapAllocator) {
        heap~>dealloc::<u8>(self.ptr, self.cap)
    }
}

/// Borrowed view of null-terminated C string.
/// Does not own memory; no Drop implementation needed.
record CStr [[layout(C)]] <: Copy {
    private ptr: *imm u8,
    private len: usize,       // Length excluding null terminator
    
    /// Creates a CStr from a raw pointer.
    /// UNSAFE: ptr must point to a valid null-terminated string.
    unsafe procedure from_ptr(ptr: *imm u8) -> CStr {
        // Scan for null terminator to determine length
        let len = c_strlen(ptr)
        CStr { ptr, len }
    }
    
    /// Creates a CStr from a pointer with known length.
    /// UNSAFE: ptr must point to valid memory of at least len+1 bytes,
    /// with a null terminator at position len.
    unsafe procedure from_ptr_with_len(ptr: *imm u8, len: usize) -> CStr {
        CStr { ptr, len }
    }
    
    /// Returns the raw pointer.
    procedure as_ptr(~) -> *imm u8 {
        self.ptr
    }
    
    /// Converts to a Cursive string view.
    /// Returns error if content is not valid UTF-8.
    procedure to_string(~) -> string@View | Utf8Error {
        // Validate UTF-8 and create view
    }
    
    /// Converts to a Cursive string, replacing invalid UTF-8.
    procedure to_string_lossy(~) -> string@View {
        // Implementation with replacement
    }
    
    /// Returns the length in bytes (excluding null terminator).
    procedure len(~) -> usize {
        self.len
    }
}
```

**FFI-Safety**

`*imm u8` and `*mut u8` are FFI-safe. The `CString` and `CStr` record types are **not** directly FFI-safe; use `as_ptr()` to obtain the raw pointer for FFI boundaries.


##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection | Effect      |
| :----------- | :------- | :------------------------------------------------ | :-------- | :---------- |
| `E-FFI-3310` | Error    | Interior null byte in CString::from_string input. | Runtime   | Panic/Error |
| `E-FFI-3311` | Error    | Invalid UTF-8 in CStr::to_string.                 | Runtime   | Panic/Error |

---

### Rationale

The review correctly identified the lack of FFI string helpers as a gap. This addition:

1. **Provides safe abstractions** — `CString`/`CStr` handle the complexity of null termination
2. **Maintains capability discipline** — Allocation requires `HeapAllocator` witness
3. **Follows FFI patterns** — Similar to Rust's `std::ffi::CString`/`CStr`

---


### 20.3 Type Representation

##### Definition

**Default Layout (Unspecified)**

For any `record` or `enum` **not** annotated with `[[layout(C)]]`, the memory layout (field ordering, padding, size, alignment) is **Unspecified Behavior (USB)**. The implementation **MAY** reorder fields, insert arbitrary padding, or optimize layout.

**C-Compatible Layout**

The `[[layout(C)]]` attribute guarantees C ABI-compatible memory layout.

##### Syntax & Declaration

**Grammar**

```ebnf
repr_attribute ::= "[[" "repr" "(" repr_kind ")" "]]"
repr_kind      ::= "C" | int_type
int_type       ::= "i8" | "i16" | "i32" | "i64" | "u8" | "u16" | "u32" | "u64"
```

##### Static Semantics

**Repr-C Record Layout**

For a `record` marked `[[layout(C)]]`:
1. Fields **MUST** be laid out in declaration order
2. Padding **MUST** be inserted only as required to satisfy alignment constraints per the target C ABI
3. The overall size **MUST** be a multiple of the largest field alignment

**Repr-C Enum Layout**

For an `enum` marked `[[layout(C)]]`:
1. The discriminant **MUST** be represented as a C-compatible integer tag
2. Default tag type is implementation-defined (typically `i32`)
3. `[[layout(IntType)]]` (e.g., `[[layout(u8)]]`) explicitly specifies the tag type

**Layout Applicability**

| Declaration   | `[[layout(C)]]` Effect                         |
| :------------ | :--------------------------------------------- |
| `record`      | C struct layout (declaration-order fields)     |
| `enum`        | C union with integer discriminant              |
| `modal`       | **Forbidden** (E-FFI-3303)                     |
| Generic types | **Forbidden** until monomorphized (E-FFI-3303) |

##### Constraints & Legality

| Code         | Severity | Condition                                                       |
| :----------- | :------- | :-------------------------------------------------------------- |
| `E-FFI-3303` | Error    | Invalid `[[layout(C)]]` on modal, generic, or unsupported type. |

---

### 20.4 Extern Declarations

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
                   "(" param_list? ")" ("->" return_type)?
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

---

### 20.5 FFI Safety Boundary

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

---

### Clause 20: Interoperability Cross-Reference Notes

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
| C-Compatible Layout                | §12.3   | Memory layout guaranteed by `[[layout(C)]]` attribute                |

**Terms referenced from other clauses:**

| Term                        | Source   | Usage in Clause 20                                  |
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
| (none) | -           | Clause 20 is terminal for FFI/interoperability |

---

# Appendices
## Appendix A: Formal Grammar (ANTLR)

  <u>Definition</u>
    Complete normative grammar for Cursive in ANTLR4 format. This grammar defines all lexical and syntactic productions required to parse valid Cursive source code.
  
  <u>Syntax & Declaration</u>
    **Grammar Structure**:
    *   **Lexer Rules**: Keywords, identifiers (XID_START/XID_Continue), literals (integer/float/string/char), operators, comments
    *   **Parser Rules**: All declarations (record, enum, modal, form, procedure), expressions (precedence-encoded), statements, patterns
    *   **Operator Precedence**: Encoded in grammar hierarchy (14 levels from Postfix to Assignment)
    *   **Comment Nesting**: Block comments (`/* ... */`) MUST nest recursively; lexer maintains nesting counter
    *   **Maximal Munch Exception**: Sequence `>>` inside generic argument list MUST be split into two `>` tokens (context-sensitive lexing)
    
  <u>Constraints & Legality</u>
    *   Keywords are reserved context-insensitively
    *   Receiver shorthands (`~`, `~%`, `~!`) MUST appear only as first parameter in type method declarations
    *   All lexical rules MUST conform to preprocessing pipeline output (§2.1)
  
  <u>Examples</u>
    Complete ANTLR4 grammar suitable for parser generators. Includes all production rules from top-level `sourceFile` to terminal tokens.

## Appendix B: Diagnostic Code Taxonomy

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
| **TRS**  | 29   | Forms               | §29               |
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
| `E-TYP-1603` | Error    | Key disjointness violation.                             |
| `W-TYP-1701` | Warning  | `f16` arithmetic may be emulated and slow.              |
| `E-TYP-1801` | Error    | Tuple or Array index out of bounds.                     |
| `E-TYP-2151` | Panic    | String slice boundary is not a valid `char` boundary.   |
| `E-TYP-2152` | Error    | Direct indexing of `string` is forbidden.               |
| `E-TYP-2050` | Error    | Modal type declares no states.                          |
| `E-TYP-2051` | Error    | Duplicate state in modal declaration.                   |
| `E-TYP-2052` | Error    | Accessing field existing only in specific modal state.  |
| `E-TYP-2053` | Error    | Accessing method existing only in specific modal state. |
| `E-TYP-2054` | Error    | Missing implementation for modal transition.            |
| `E-TYP-2055` | Error    | Transition body does not return target state type.      |
| `E-TYP-2060` | Error    | Non-exhaustive match on modal type.                     |
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
| `E-EXP-2592` | Error    | Pool region result depends on invalidated binding.    |
| `E-STM-2631` | Error    | Assignment target is not a place.                     |
| `E-STM-2651` | Error    | `defer` block returns non-unit value.                 |
| `E-STM-2652` | Error    | Non-local control flow (return/break) in `defer`.     |
| `E-STM-2661` | Error    | `return` value type mismatch.                         |
| `E-STM-2662` | Error    | `break` used outside loop.                            |
| `E-STM-2663` | Error    | `continue` used outside loop.                         |
| `E-STM-2664` | Error    | `result` value mismatch with block type.              |
| `E-STM-2671` | Error    | Lens disjointness proof verification failed.          |

##### B.3.6 PAT (Patterns)

| Code         | Severity | Description                                    |
| :----------- | :------- | :--------------------------------------------- |
| `E-PAT-2711` | Error    | Refutable pattern used in irrefutable context. |
| `E-PAT-2741` | Error    | Non-exhaustive pattern match.                  |
| `E-PAT-2751` | Error    | Unreachable match arm.                         |

##### B.3.7 CON (Contracts & Concurrency)

| Code              | Severity | Description                                             |
| :---------------- | :------- | :------------------------------------------------------ |
| `E-CON-2801`      | Error    | Static contract verification failed.                    |
| `E-CON-2802`      | Error    | Impure/Unsafe expression in contract.                   |
| `E-CON-2803`      | Error    | Liskov violation: Precondition strengthened.            |
| `E-CON-2804`      | Error    | Liskov violation: Postcondition weakened.               |
| `E-CON-2805`      | Error    | `@entry` applied to non-Copy/Clone type.                |
| `E-CON-2806`      | Error    | `@result` used outside `will` clause.                   |
| `E-KEY-0001`      | Error    | Access to shared path without key context.              |
| `E-KEY-0010`      | Error    | Potential key conflict on dynamic indices.              |
| `E-KEY-0020`      | Error    | `#` annotation on method call.                          |
| `E-KEY-0030`      | Error    | Redundant `#` (already at boundary).                    |
| `E-SPAWN-0001`    | Error    | `spawn` outside `parallel` block.                       |
| `E-SPAWN-0002`    | Error    | Captured reference outlives task.                       |
| `E-SPAWN-0003`    | Error    | Cannot capture downgraded reference outside structured. |
| `E-SPAWN-0004`    | Error    | `detach` prohibited inside `parallel` block.            |
| `E-ASYNC-0020`    | Error    | `wait` on non-Async type.                               |
| `E-ASYNC-0030`    | Error    | `=>` left operand not Async.                            |
| `E-ASYNC-0040`    | Error    | `wait race` with fewer than 2 arms.                     |
| `E-ASYNC-0050`    | Error    | `wait all` with zero expressions.                       |
| `E-ASYNC-0060`    | Error    | `when` on non-shared receiver.                          |
| `E-DISPATCH-0001` | Error    | Cross-iteration dependency in dispatch.                 |
| `W-KEY-0001`      | Warning  | Fine-grained keys in tight loop.                        |
| `W-KEY-0002`      | Warning  | Redundant key acquisition (covered by held key).        |

##### B.3.8 TRS (Forms)

| Code         | Severity | Description                                            |
| :----------- | :------- | :----------------------------------------------------- |
| `E-TRS-2901` | Error    | Abstract implementation incorrectly marked `override`. |
| `E-TRS-2902` | Error    | Concrete override missing `override` keyword.          |
| `E-TRS-2903` | Error    | Missing implementation for required form procedure.    |
| `E-TRS-2910` | Error    | Accessing non-form member on opaque type.              |
| `E-TRS-2920` | Error    | Explicit call to `Drop::drop`.                         |
| `E-TRS-2921` | Error    | Type implements both `Copy` and `Drop`.                |
| `E-TRS-2922` | Error    | `Copy` implementation on type with non-Copy fields.    |
| `E-TRS-2940` | Error    | Calling `where Self: Sized` procedure on witness.      |

##### B.3.9 MEM (Memory & Safety)

| Code         | Severity | Description                                    |
| :----------- | :------- | :--------------------------------------------- |
| `E-MEM-3001` | Error    | Use of moved value.                            |
| `E-MEM-3002` | Error    | Access to binding in Uninitialized state.      |
| `E-MEM-3003` | Error    | Reassignment of immutable binding.             |
| `E-MEM-3004` | Error    | Partial move from `const` binding.             |
| `E-MEM-3005` | Error    | Explicit call to destructor.                   |
| `E-MEM-3006` | Error    | Attempt to move from immovable binding (`:=`). |
| `E-MEM-3020` | Error    | Region pointer escape.                         |
| `E-MEM-3021` | Error    | Region allocation `^` outside region scope.    |
| `E-MEM-3022` | Error    | Caret depth exceeds region nesting depth.      |
| `E-MEM-3030` | Error    | Unsafe operation in safe code.                 |
| `E-MEM-3031` | Error    | `transmute` size mismatch.                     |

##### B.3.10 FFI (Interoperability)

| Code         | Severity | Description                                        |
| :----------- | :------- | :------------------------------------------------- |
| `E-FFI-3301` | Error    | Non-FFI-Safe type in `extern` signature.           |
| `E-FFI-3302` | Error    | Call to `extern` procedure outside `unsafe` block. |
| `E-FFI-3303` | Error    | Invalid application of `[[layout(C)]]`.            |
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

## Appendix C: Conformance Dossier Schema

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

## Appendix D: Standard Form Catalog

This appendix provides normative definitions for foundational forms and system capability forms built into Cursive or its core library.

  <u>Definition</u>
    Normative definitions for foundational forms and system capability forms that are deeply integrated with language mechanics.
  
  <u>Syntax & Declaration</u>
    **Foundational Forms** (§D.1):
    *   `Drop`: `procedure drop(~!)` - RAII cleanup, compiler-invoked only
    *   `Copy`: Marker form for implicit bitwise duplication
    *   `Clone`: `procedure clone(~): Self` - explicit deep copy

    
    **System Capability Forms** (§D.2):
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
        -   `time(): Timestamp`
    *   `Time`: Monotonic time access with `now(): Timestamp` procedure
  
  <u>Constraints & Legality</u>
    *   `Drop::drop` MUST NOT be called directly by user code (E-TRS-2920)
    *   `Copy` and `Drop` are mutually exclusive on same type (E-TRS-2921)
    *   `Copy` requires all fields implement `Copy` (E-TRS-2922)
    *   `HeapAllocator::alloc` MUST panic on OOM (never return null)
    *   Variadic implementations across all form types are prohibited
  
  <u>Static Semantics</u>
    *   Compiler automatically inserts `Drop::drop` calls at scope exit for responsible bindings
    *   `Copy` types are duplicated (not moved) on assignment/parameter passing
    *   Static invalidation applies to aliases when Drop is invoked on owner
    *   Capability forms enable attenuation patterns (e.g., `with_quota` on HeapAllocator)
  
  <u>Dynamic Semantics</u>
    *   Drop execution grants temporary exclusive (unique) access to self
    *   Drop during panic unwind: second panic causes process abort
    *   HeapAllocator failures cause panic (no null returns)
  
  <u>Examples</u>
    Complete form signatures must be provided for: Drop, Copy, Clone, HeapAllocator, FileSystem, Network, Time, System (which implements Time and provides `exit`, `get_env`).

## Appendix E: Core Library Specification

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
    *   `Context` field types MUST match the capability form witnesses defined in Appendix D
    *   Core library types (`Option`, `Result`, `Context`, primitives) MUST be available in the universe scope (no import required)
    *   `main` procedure MUST accept `Context` parameter as defined
  
  <u>Static Semantics</u>
    *   All context capabilities are passed explicitly (no ambient authority)
  


## Appendix F: Implementation Guide (Informative)

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

#### F.4 Receiver Token Design Rationale

  <u>Design Note</u>

  The tilde (`~`) was chosen for receiver syntax based on its mnemonic association with "home" in Unix systems—representing the method's "home" (the receiver instance). The shorthand forms (`~!`, `~%`) extend this metaphor while providing visual distinction for permission-qualified receivers.

## Appendix G: Behavior Classification Index (Normative)

This appendix provides an index of behaviors by classification. Definitions for each classification (Defined, IDB, USB, UVB) are in §1.2.

**G.1 Unverifiable Behavior (UVB) Instances**:
*   FFI Call (§32.2)
*   Raw Deref (§29.6.2)
*   Transmute (§29.6.3)
*   Pointer Arithmetic (§24.4.2)
*   Trusted Contracts within `unsafe` (§7.4)
*   Trusted Partitions (`[[verify(trusted)]]`)

**G.2 Implementation-Defined Behavior (IDB) Instances**:
*   Type Layout (non-C)
*   Integer Overflow (Release)
*   Pointer Width
*   Resource Limits
*   Panic Abort Mechanism

### G.3 Unspecified Behavior (USB) Instances

*   **Map Iteration**: Order of iteration for hash-based collections.
*   **Padding Bytes**: The values of padding bytes in non-`[[layout(C)]]` records.

### G.4 Defined Runtime Panics

*   Integer Overflow (Checked Mode)
*   Array/Slice Bounds Check
*   **Dynamic Lens Overlap** (`[[verify(dynamic)]]`)

## Appendix H: Formal Core Semantics (Normative)

This appendix defines the **Cursive Core Calculus**, a simplified formal model of the language's memory and permission system. Implementations **MUST** preserve the safety properties defined here.


#### H.1 Syntax of the Core

$$\begin{aligned} v &::= \ell \mid \text{const } \ell \mid \text{null} \\ e &::= v \mid \text{let } x = e \text{ in } e \mid x \mid x.f \mid x.f \leftarrow v \mid \text{spawn}(e) \\ \tau &::= \text{const } T \mid \text{unique } T \quad \text{(shared omitted; see Scope Note)} \end{aligned}$$

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
If $\Gamma \vdash e : \text{well-formed}$, and $e$ contains spawn, no two threads can access location $\ell$ simultaneously unless both accesses are Reads.  
_Proof Sketch:_ The unique permission ($\ell$) cannot be duplicated. The const permission ($\text{const } \ell$) allows duplication but removes the Write Rule from the set of valid reductions for that value.

## Appendix I: Implementation Limits (Normative)

This appendix defines the minimum guaranteed capacities that all conforming implementations MUST support. Programs exceeding these limits are ill-formed (§1.4).

**Minimum Guaranteed Limits**:
*   **Source Size**: 1 MiB
*   **Logical Lines**: 65,535
*   **Line Length**: 16,384 chars
*   **Nesting Depth**: 256
*   **Identifier Length**: 1,023 chars
*   **Parameters**: 255
*   **Fields**: 1,024
*   **Recursion Depth**: 256 (Comptime), Implementation-Defined (Runtime)

---

## Appendix J: Diagnostic Code Cross-Reference (Informative)

This appendix provides a cross-reference of diagnostic codes that may be triggered by related or identical program violations across different specification clauses.

#### J.1 Permission and Memory Model Diagnostics

| Clauses 4-6 (Types) | Clause 3 (Memory) | Clause 13 (Concurrency) | Violation Category                      |
| :------------------ | :---------------- | :---------------------- | :-------------------------------------- |
| `E-TYP-1601`        | `E-MEM-3003`      | —                       | Mutation through immutable binding/path |
| `E-TYP-1602`        | `E-MEM-3040`      | —                       | Unique permission exclusion             |
| `E-TYP-1604`        | —                 | `E-KEY-0001`            | shared access without key               |
| —                   | —                 | `E-KEY-0010`            | Dynamic index key conflict              |
| —                   | —                 | `E-DISPATCH-0001`       | Cross-iteration dependency              |
| `E-TYP-1511`        | —                 | —                       | Implicit permission upgrade             |
| `E-TYP-1512`        | `E-MEM-3042`      | —                       | Sibling permission coercion             |

#### J.1.1 Modal Type Diagnostics

| Clause 6 (Modal) | Related Diagnostic | Violation Category                  |
| :--------------- | :----------------- | :---------------------------------- |
| `E-TYP-2052`     | —                  | Invalid state field access          |
| `E-TYP-2053`     | —                  | Invalid state method invocation     |
| `E-TYP-2057`     | —                  | Direct access on general modal type |
| `E-TYP-2060`     | —                  | Non-exhaustive modal match          |

#### J.1.2 Pointer Type Diagnostics

| Clause 6 (Pointers) | Related Diagnostic | Violation Category                     |
| :------------------ | :----------------- | :------------------------------------- |
| `E-TYP-2001`        | `E-MEM-3030`       | Dereference of non-Valid pointer       |
| `E-TYP-2002`        | —                  | Raw pointer dereference outside unsafe |

#### J.2 Move and Binding State Diagnostics

| Clause 3 (Memory) | Clauses 4-7 (Types) | Violation Category          |
| :---------------- | :------------------ | :-------------------------- |
| `E-MEM-3001`      | —                   | Access to moved binding     |
| `E-MEM-3006`      | —                   | Move from immovable binding |
| `E-MEM-3004`      | —                   | Partial move from const     |

#### J.3 Implementation Guidance

When multiple diagnostic codes apply to the same violation:

1. Implementations MUST emit the **canonical** diagnostic. Type system diagnostics (`E-TYP-xxxx`) are canonical; memory model diagnostics (`E-MEM-xxxx`) are secondary and MAY be emitted in addition to, but not instead of, the canonical diagnostic.
2. Implementations MAY additionally emit secondary diagnostics if they provide distinct information (e.g., different source location granularity).
3. Implementations SHOULD prefer more specific diagnostics within a clause (e.g., `E-TYP-1604` over `E-TYP-1601` for shared field mutation).

#### J.4 Diagnostic Severity Legend

| Prefix | Severity | Effect               |
| :----- | :------- | :------------------- |
| `E-`   | Error    | Compilation rejected |
| `W-`   | Warning  | Compilation proceeds |
| `P-`   | Panic    | Runtime termination  |

---

## Appendix K: Diagnostic Code Registry (Normative)

This appendix provides the authoritative allocation of diagnostic code ranges to prevent conflicts across specification clauses.

#### K.1 Code Range Allocation

| Prefix        | Range         | Clause/Domain                           |
| :------------ | :------------ | :-------------------------------------- |
| `E-CNF-`      | 0100–0299     | Clause 1: Conformance (general)         |
| `E-CNF-`      | 0300–0399     | Clause 1: Implementation Limits         |
| `E-SRC-`      | 0100–0399     | Clause 2: Source Text & Lexical         |
| `E-SYN-`      | 0100–0199     | Clause 2: Syntactic Nesting             |
| `E-MEM-`      | 3000–3099     | Clause 3: Memory Model                  |
| `E-TYP-`      | 1500–1599     | Clause 4: Type System Foundations       |
| `E-TYP-`      | 1600–1699     | Clause 4: Permission System             |
| `E-TYP-`      | 1700–1799     | Clause 5: Primitive Types               |
| `E-TYP-`      | 1800–1849     | Clause 5: Composite Types (Tuple/Array) |
| `E-TYP-`      | 1850–1899     | Clause 5: Composite Types (Slice/Range) |
| `E-TYP-`      | 1900–1949     | Clause 5: Records                       |
| `E-TYP-`      | 1950–1999     | Clause 5: Enums                         |
| `E-TYP-`      | 2000–2049     | Clause 5: Union Types                   |
| `E-TYP-`      | 2050–2099     | Clause 6: Modal Types                   |
| `E-TYP-`      | 2100–2149     | Clause 6: Pointer Types                 |
| `E-TYP-`      | 2150–2199     | Clause 6: String Type                   |
| `E-TYP-`      | 2200–2249     | Clause 6: Function Types                |
| `E-TYP-`      | 2300–2399     | Clause 7: Generics                      |
| `E-DEC-`      | 2400–2499     | Clause 8: Declarations                  |
| `E-NAM-`      | 1300–1399     | Clause 9: Name Resolution               |
| `E-EXP-`      | 2500–2599     | Clause 11: Expressions                  |
| `E-STM-`      | 2630–2639     | Clause 11: Assignment Statements        |
| `E-STM-`      | 2650–2659     | Clause 11: Defer Statements             |
| `E-STM-`      | 2660–2669     | Clause 11: Control Flow Statements      |
| `E-PAT-`      | 2710–2759     | Clause 11: Patterns                     |
| `E-TRS-`      | 2900–2999     | Clause 9: Forms                         |
| `E-CON-`      | 3200–3299     | Clause 12: Contracts                    |
| `E-KEY-`      | 0001–0099     | Clause 13: Key System                   |
| `E-SPAWN-`    | 0001–0099     | Clause 13: Task Spawning                |
| `E-ASYNC-`    | 0001–0099     | Clause 13: Async Operations             |
| `E-DISPATCH-` | 0001–0099     | Clause 13: Data Parallelism             |
| `E-WAIT-`     | 0001–0099     | Clause 13: Wait Expressions             |
| `W-KEY-`      | 0001–0099     | Clause 13: Key Warnings                 |
| `P-`          | (same ranges) | Runtime Panics (use domain prefix)      |
| `W-`          | (same ranges) | Warnings (use domain prefix)            |

#### K.2 Reserved Ranges

The following ranges are reserved for future specification expansion:

| Range     | Reserved For            |
| :-------- | :---------------------- |
| 2500–2899 | Clauses 10–12           |
| 4000–4999 | Standard Library        |
| 5000–5999 | Implementation-Specific |

#### K.3 Conflict Resolution

When a program violation is detectable by multiple specification subsystems (e.g., both the type system and the memory model), the **canonical diagnostic** is determined by the following precedence:

1. Type System diagnostics (`E-TYP-`) are canonical for permission and type violations
2. Memory Model diagnostics (`E-MEM-`) are canonical for lifetime and move violations
3. Source diagnostics (`E-SRC-`) are canonical for lexical violations

Implementations MAY emit secondary diagnostics in addition to the canonical diagnostic when doing so provides distinct information.
