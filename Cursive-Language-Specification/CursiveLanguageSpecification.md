# The Cursive Language Specification (Draft 3)

# Part I: General Principles and Conformance

## Clause 1: General Principles and Conformance

This clause establishes the foundational framework for the Cursive language specification. It defines the normative requirements for conforming implementations and programs, the classification of program behaviors, the minimum capacities implementations MUST support, and the rules governing language evolution.

---

### 1.1 Conformance Obligations

##### Definition

A **conforming implementation** is a translator (compiler, interpreter, or hybrid) that satisfies all normative requirements of this specification applicable to implementations. A **conforming program** is a Cursive source program that satisfies all normative requirements of this specification applicable to programs.

##### Static Semantics

**Implementation Requirements**

A conforming implementation:

1. MUST accept all well-formed programs and translate them to an executable representation.

2. MUST reject all ill-formed programs (§1.2.1) (except those classified as IFNDR) with at least one error diagnostic identifying the violation.

3. MUST generate a **Conformance Dossier** as a build artifact. The dossier MUST be a machine-readable JSON document conforming to the schema defined in Appendix C.

4. MUST document all Implementation-Defined Behavior (IDB) choices in the Conformance Dossier.

5. MUST record all IFNDR instances per §1.2.1.

> **Note:** Throughout this specification, statements that an implementation choice "is IDB" or "MUST be documented" implicitly refer to documentation in the Conformance Dossier per requirement 4 above. This documentation requirement is not restated for each IDB instance.

**Program Requirements**

A **well-formed program** satisfies the following conditions:

1. Satisfies all lexical constraints defined in Clause 2.
2. Satisfies all syntactic constraints defined in Clause 3.
3. Satisfies all static-semantic constraints defined throughout this specification.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                            | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0101` | Error    | Program is ill-formed due to syntactic or static-semantic violation. | Compile-time | Rejection |
| `E-CNF-0102` | Error    | Conformance Dossier generation failed.                               | Compile-time | Rejection |

Implementation limit violations are diagnosed per §1.4.

---

### 1.2 Behavior Classifications

##### Definition

**Behavior classifications** partition all program behaviors into four mutually exclusive categories based on how this specification constrains outcomes and what guarantees implementations provide.

##### Static Semantics

**Defined Behavior** is program behavior for which this specification prescribes exactly one permitted outcome. Conforming implementations MUST produce this outcome for all well-formed programs exercising such behavior.

**Implementation-Defined Behavior (IDB)** occurs when this specification permits multiple valid outcomes and requires the implementation to document which outcome it chooses. The implementation MUST select exactly one outcome from the permitted set, document its choice in the Conformance Dossier, and maintain consistency across all compilations with the same configuration. Programs relying on a specific IDB choice are conforming but non-portable. See Appendix F for the complete IDB index.

**Unspecified Behavior (USB)** occurs when this specification permits a set of valid outcomes but does not require the implementation to document which outcome is chosen. The choice MAY vary between executions, compilations, or within a single execution. USB in safe code MUST NOT introduce memory unsafety or data races. See Appendix F for the complete USB index.

**Unverifiable Behavior (UVB)** comprises operations whose runtime correctness depends on properties external to the language's semantic model. UVB is permitted only within `unsafe` blocks and Foreign Function Interface (FFI) calls. By using an `unsafe` block, the programmer asserts responsibility for upholding an external contract that the compiler cannot verify. See Appendix F for the complete UVB index.

**Diagnostic Coordination**

Multiple diagnostic codes across different clauses may be triggered by the same underlying program violation when detected at different compilation phases. See Appendix B.4 for conflict resolution rules.

##### Constraints & Legality

**Safety Invariant**

Safe code cannot exhibit Unverifiable Behavior. For any well-formed program where all expressions are outside of `unsafe` blocks, the program MUST NOT exhibit any Unverifiable Behavior. This invariant is enforced by the type system (Clause 4), the permission system (§4.5), and the module system (Clause 6).

---

#### 1.2.1 Ill-Formed, No Diagnostic Required (IFNDR)

##### Definition

A program that violates a static-semantic rule of the language is **ill-formed**. When detecting such a violation is computationally undecidable or infeasible, an implementation is not required to issue a diagnostic. Such a program is classified as **Ill-Formed, No Diagnostic Required (IFNDR)**.

##### Static Semantics

**Classification Criteria**

A violation is classified as IFNDR when at least one of the following conditions holds:

1. The violation involves a property that is undecidable in general (e.g., halting, unbounded symbolic execution).
2. The violation requires analysis that exceeds polynomial complexity in the size of the program.
3. The violation depends on runtime values that cannot be statically determined.

**IFNDR Categories**

The following table classifies IFNDR conditions by category. This is a classification table for organizing IFNDR instances, not a diagnostic table; IFNDR conditions by definition do not require diagnostics.

| Category            | Description                                                                         |
| :------------------ | :---------------------------------------------------------------------------------- |
| OOB in Const Eval   | Index out of bounds during constant evaluation where index is not a static constant |
| Recursion Limits    | Exceeding implementation limits in complex metaprogramming that cannot be bounded   |
| Infinite Type       | Type definitions that would require infinite expansion                              |
| Unbounded Unrolling | Loop unrolling in `comptime` context that cannot be statically bounded              |

##### Constraints & Legality

**Safety Boundary**

The observable effects of executing an IFNDR program are Unspecified Behavior (USB). The Safety Invariant (§1.2) still applies: when the IFNDR condition occurs in safe code, the observable effects MUST NOT exhibit Unverifiable Behavior (UVB).

Permitted outcomes include:

- Compilation failure (if detected)
- Deterministic panic at runtime
- Non-deterministic but safe execution

Prohibited outcomes include:

- Memory corruption
- Data races
- Arbitrary code execution

**Tracking and Reporting**

IFNDR instances MUST be recorded in the Conformance Dossier per §1.1. The dossier MUST record the source file path, line number, and error category for every IFNDR instance.

---

### 1.3 Reserved Identifiers

##### Definition

A **reserved identifier** is an identifier that is reserved for use by this specification or by implementations, and MUST NOT be used as a user-defined identifier in conforming programs.

##### Static Semantics

**Reserved Keywords**

The identifiers listed in the keyword table (§2.6) are reserved keywords per the constraints defined therein.

**Reserved Namespaces**

The `cursive.*` namespace prefix is reserved for specification-defined features. User programs and vendor extensions MUST NOT use this namespace.

**Implementation Reservations**

Implementations MAY reserve additional identifier patterns (e.g., identifiers beginning with `__` or `_[A-Z]`). Such reservations are IDB and SHOULD follow common conventions to avoid collision with user code.

Conforming programs SHOULD NOT use patterns that are commonly reserved by implementations.

**Universe-Protected Bindings**

**A Universe-Protected Binding is an identifier that is pre-declared in the implicit universal scope and MUST NOT be shadowed by user declarations at module scope.**

The following identifiers are universe-protected:

1. **Primitive type names:** `i8`, `i16`, `i32`, `i64`, `i128`, `u8`, `u16`, `u32`, `u64`, `u128`, `f16`, `f32`, `f64`, `bool`, `char`, `usize`, `isize`

2. **Special types:** `Self`, `string`, `Modal`

3. **Async type aliases (§15.3):** `Async`, `Future`, `Sequence`, `Stream`, `Pipe`, `Exchange`

> **Forward Reference:** `Self` is the type-relative self-reference (§4.2). `string` is a modal type for text (§6.2). `Modal` is the metatype for modal declarations (§6.1). Async type aliases are defined in §15.3.

Attempting to shadow a universe-protected binding at module scope MUST be diagnosed as an error.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0401` | Error    | Reserved keyword used as identifier.                                                         | Compile-time | Rejection |
| `E-CNF-0402` | Error    | Reserved namespace `cursive.*` used by user code.                                            | Compile-time | Rejection |
| `E-CNF-0403` | Error    | Primitive type name shadowed at module scope.                                                | Compile-time | Rejection |
| `E-CNF-0404` | Error    | Shadowing of `Self`, `string`, or `Modal`.                                                   | Compile-time | Rejection |
| `E-CNF-0405` | Error    | Shadowing of async type alias (`Async`, `Future`, `Sequence`, `Stream`, `Pipe`, `Exchange`). | Compile-time | Rejection |
| `W-CNF-0401` | Warning  | Implementation-reserved pattern used.                                                        | Compile-time | N/A       |

---

### 1.4 Implementation Limits

##### Definition

**Implementation limits** are minimum guaranteed capacities that all conforming implementations MUST support. Programs exceeding these limits are ill-formed.

##### Static Semantics

Minimum guaranteed limits are defined in Appendix G. Representative limits include:

- **Nesting Depth**: 256 levels (block, control-flow, and expression nesting)
- **Identifier Length**: 1,023 characters
- **Parameters per Procedure**: 255

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                      | Detection    | Effect    |
| :----------- | :------- | :----------------------------- | :----------- | :-------- |
| `E-CNF-0301` | Error    | Implementation limit exceeded. | Compile-time | Rejection |

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

2. The `cursive.*` namespace is reserved (§1.3).

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

### 1.6 Foundational Semantic Concepts

##### Definition

This section defines foundational concepts used throughout the specification for static analysis and scoping.

**Program Point**

A **Program Point** is a unique location in the abstract control flow representation of a procedure body at which the compiler tracks state.

$$\text{ProgramPoint} ::= (\text{ProcedureId}, \text{CFGNodeId})$$

where:
- $\text{ProcedureId}$ uniquely identifies the enclosing procedure
- $\text{CFGNodeId}$ uniquely identifies a node in that procedure's control flow graph

Program points are partially ordered by control flow reachability: $p_1 \leq_{cf} p_2$ if and only if $p_2$ is reachable from $p_1$ via zero or more control flow edges.

**Lexical Scope**

A **Lexical Scope** is a contiguous region of source text that defines a lifetime boundary for bindings, keys, and deferred actions.

$$\text{LexicalScope} ::= (\text{EntryPoint}, \text{ExitPoint}, \text{Parent}?)$$

where:
- $\text{EntryPoint}$ is the program point where the scope begins
- $\text{ExitPoint}$ is the program point where the scope ends
- $\text{Parent}$ is the immediately enclosing scope (absent for procedure-level scope)

**Scope Hierarchy**

Lexical scopes form a forest of trees, with one tree per procedure, rooted at procedure scope. A scope $S_1$ is **nested within** $S_2$ (written $S_1 \sqsubset S_2$) if $S_2$ is an ancestor of $S_1$ in this tree.

> **Note:** This definition of Lexical Scope describes the lifetime-tracking structure used for deterministic destruction, deferred execution, and key validity. For the name resolution hierarchy (the four-level Scope Context used for identifier lookup), see §8.4 (Scope Context Structure).

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

Unicode normalization of source text outside identifiers and module-path components is Implementation-Defined Behavior (IDB).

For identifier lexemes and module-path components, implementations MUST normalize the corresponding scalar sequences to Unicode Normalization Form C (NFC) prior to equality comparison, hashing, or name lookup.

Any normalization performed MUST NOT change logical line boundaries or the byte offsets used for diagnostic locations.

##### Constraints & Legality

**Prohibited Code Points**

Source files MUST NOT contain prohibited code points outside of string and character literals (§2.8). The prohibited code points $\mathcal{F}$ are:

$$\mathcal{F} = \{c : \text{GeneralCategory}(c) = \texttt{Cc}\} \setminus \{\text{U+0009}, \text{U+000A}, \text{U+000C}, \text{U+000D}\}$$

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

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-SRC-0101` | Error    | UTF-8 decoding failed during preprocessing.             | Compile-time | Rejection |
| `E-SRC-0102` | Error    | Source size exceeds implementation limit during step 1. | Compile-time | Rejection |
| `E-SRC-0103` | Error    | Embedded or misplaced BOM encountered in step 3.        | Compile-time | Rejection |
| `E-SRC-0104` | Error    | Prohibited control character encountered in step 5.     | Compile-time | Rejection |
| `E-SRC-0309` | Error    | Tokenization failed to classify a character sequence.   | Compile-time | Rejection |

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

The lexer MUST classify every non-comment, non-whitespace fragment as exactly one token kind from the set $\mathcal{T}$ defined above.

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

| Code         | Severity | Condition                      | Detection    | Effect    |
| :----------- | :------- | :----------------------------- | :----------- | :-------- |
| `E-SRC-0307` | Error    | Invalid Unicode in identifier. | Compile-time | Rejection |

Reserved keyword violations are diagnosed per §1.3 (`E-CNF-0401`).

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
widen       witness     yield
```

Implementations MUST tokenize these as `<keyword>`, not `<identifier>`. The keyword set MUST be identical across conforming implementations for a given language version. All keywords are unconditionally reserved in all syntactic contexts.

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

When the token `>>` appears in a context where it would close two or more nested generic parameter lists (e.g., `Vec<Vec<i32>>`), the implementation MUST split `>>` into two separate `>` tokens. The implementation MUST preserve the original lexeme span for diagnostic purposes. This exception applies recursively: `>>>` MUST be split into three `>` tokens when closing three nested generic parameter lists.

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
<suffix>        ::= "f16" | "f32" | "f64"
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

A decimal integer with a leading zero followed by additional digits MUST be accepted and interpreted as a decimal (base-10) value; the leading zeros have no semantic effect. For example, `007` is parsed as the decimal integer 7, not as an octal literal. Implementations SHOULD emit warning `W-SRC-0301` to alert programmers accustomed to C-style octal notation.

A string literal MUST begin with `"` and end with a matching unescaped `"`. A line feed or end-of-file before the closing quote MUST trigger `E-SRC-0301`.

A character literal MUST represent exactly one Unicode scalar value. Empty or multi-value character literals MUST trigger `E-SRC-0303`.

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

Comments beginning with `///` or `//!` are documentation comments. Implementations MUST preserve documentation comments and associate them with their following declaration (`///`) or enclosing module (`//!`). Documentation comments MUST NOT appear in the token stream. Documentation extraction and rendering are implementation-defined.

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

| Category              | Unicode Property       |
| :-------------------- | :--------------------- |
| Bidirectional Control | Bidi_Control           |
| Zero-Width Characters | General_Category=Cf    |
| Homoglyphs            | Confusable mappings    |
| Combining Characters  | General_Category=Mn/Mc |

**Enforcement Modes**

When a character in $\mathcal{S}$ appears unescaped in an identifier, operator/punctuator, or adjacent to token boundaries outside literals and comments, the implementation MUST emit a diagnostic. Severity depends on conformance mode:

| Mode       | Behavior                                  | Diagnostic |
| :--------- | :---------------------------------------- | :--------- |
| Permissive | Issue warning `W-SRC-0308`; accept source | Warning    |
| Strict     | Issue error `E-SRC-0308`; reject source   | Error      |

The default mode is IDB. Implementations MUST provide a mechanism to select either mode.

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

**Example — Ambiguous plus:**

```cursive
let sum = value
+increment      // Parsed as: let sum = value + increment (continuation)

let sum = value;
+increment      // Parsed as: let sum = value; followed by unary +increment
```

**Example — Ambiguous asterisk (multiply vs dereference):**

```cursive
let result = ptr
*offset         // Parsed as: let result = ptr * offset (multiplication)

let result = ptr;
*offset         // Parsed as: let result = ptr; followed by dereference *offset
```

**Example — Ambiguous ampersand (bitwise AND vs address-of):**

```cursive
let bits = flags
&mask           // Parsed as: let bits = flags & mask (bitwise AND)

let bits = flags;
&mask           // Parsed as: let bits = flags; followed by address-of &mask
```

**Example — Ambiguous pipe (bitwise OR):**

```cursive
let combined = base
|modifier       // Parsed as: let combined = base | modifier (bitwise OR)

let combined = base;
|modifier       // Parsed as: let combined = base; followed by |modifier expression
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------------------------------------------- | :----------- | :-------- |
| `E-SYN-0110` | Error    | Missing statement terminator (no `;` and newline is not a terminator by rules in this section).  | Compile-time | Rejection |
| `E-SYN-0111` | Error    | Line continuation used where none of the continuation conditions apply (newline must terminate). | Compile-time | Rejection |

---

### 2.12 Translation Phases

##### Definition

A **Translation Phase** is a discrete stage in the compilation pipeline. Compilation operates on **Compilation Units** as defined in §8.1 (Module System Architecture).

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

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-SYN-0101` | Error    | Block nesting depth exceeded.      | Compile-time | Rejection |
| `E-SYN-0102` | Error    | Expression nesting depth exceeded. | Compile-time | Rejection |

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

A program is **memory-safe** if and only if it satisfies both the Liveness property (all accesses target live memory) and the Aliasing property (no write conflicts occur).

##### Static Semantics

**Liveness Enforcement**

Liveness is enforced through RAII and deterministic destruction (§3.6), modal pointer states (§6.3), and region escape analysis (§3.3).

**Aliasing Enforcement**

Aliasing is enforced through permission types (§4.5) and the key-based concurrency model (§13).

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
3.  **Destruction and Deallocation**: If the object's type implements `Drop` (§9.7), its destructor is invoked. Storage is then released. (`Drop` is a form that defines custom destruction logic; see §9.7 for the complete specification.)

Storage duration categories correspond to provenance tags (§3.3).

##### Memory & Layout

The size, alignment, and internal layout of objects are governed by their types. Universal layout principles are defined in §5 (Data Types), and the `[[layout(...)]]` attribute is specified in §7.2.1.

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

> **Informative Note:** A heap allocation achieves global provenance when it is intentionally never deallocated. For example, a logger singleton allocated at startup and used throughout program execution has $\pi_{\text{Heap}}$ storage but effectively $\pi_{\text{Global}}$ lifetime. The non-strict ordering $\pi_{\text{Heap}} \le \pi_{\text{Global}}$ permits storing such heap addresses in static bindings without triggering escape analysis errors.

**Constraint:** Heap-allocated storage whose address has been stored in a binding with global provenance MUST NOT be deallocated during program execution. Deallocating such storage constitutes Unverifiable Behavior (UVB). Violation in `unsafe` code constitutes UVB (§1.2).

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

After a partial move, the parent binding enters the PartiallyMoved state. If the binding is a mutable `var`, reassignment may restore it to Valid state (§3.4). A `let` binding in PartiallyMoved state cannot be restored and remains invalid for the remainder of its scope. Any use of a PartiallyMoved binding—whether read, write, or move—is a compile-time error diagnosed as `E-MEM-3001`.

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

Execution modifiers are specified on `parallel` blocks (§13.3), not regions.

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

The unprefixed `^` operator requires a named region in lexical scope. Using `^` with no named region in scope is error `E-REG-1205`.

##### Dynamic Semantics

**Region Entry**

1. Entering `region as r { body }` allocates a region frame `r` and marks it Active for the dynamic extent of `body`.
2. The Active region pointer for implicit `^` allocation is set to `r` on entry and restored to its previous value on exit.

**Allocation Lifetime**

1. `^expr` allocates storage from the Active region; `r^expr` allocates from the explicitly named region `r`.
2. Each allocation returns an initialized object whose lifetime is bounded by the lifetime of the region that supplied the storage.

**Region Exit and Deallocation Order**

1. On normal exit from the region block, all objects allocated in the region are destroyed, then the region storage is released in bulk.
2. Destruction order within a region is LIFO with respect to allocation order. Nested regions are destroyed before their parent region resumes destruction (structured stack discipline).
3. On unwinding (panic), the same destruction and bulk deallocation sequence occurs before unwinding continues to the enclosing region or scope.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-REG-1205` | Error    | `^` with no named region in scope              | Compile-time | Rejection |
| `E-REG-1206` | Error    | Named region not found for prefixed allocation | Compile-time | Rejection |

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

The following operations are permitted ONLY within an `unsafe` block: raw pointer dereference, unsafe procedure calls, `transmute`, and pointer arithmetic. Raw pointer types (`*imm T`, `*mut T`) are defined in §6.3.

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

# Part 2: Types

## Clause 4: Type System Foundations

This clause defines the foundational machinery of the Cursive type system: the abstract framework governing type equivalence, subtyping, variance, inference, and the permission system. These concepts underpin all concrete type definitions in subsequent clauses.

---

### 4.1 Type System Architecture

##### Definition

The **type system** is the static semantic framework that assigns a type to every well-formed expression and validates the consistency of those types across the program. A **type** is a classification of values that determines their representation, the operations applicable to them, and the constraints governing their use. Two types are **equivalent** if and only if they classify the same values under the equivalence rules defined in this section.

##### Static Semantics

**Static Type Checking**

Cursive MUST be implemented as a statically typed language. All type checking MUST be performed at compile time. A program that fails type checking MUST be diagnosed as ill-formed and rejected.

**Nominal vs. Structural Typing**

The type system is primarily **nominal**. Two types with different names are never equivalent, even if their structure is identical. The following constructs are nominal:

- Record types (§5.3)
- Enum types (§5.4)
- Modal types (§6.1)

The following constructs are **structural**. Their equivalence is determined by their composition:

- Tuple types (§5.2.1)
- Anonymous union types (§5.5)
- Function types (§6.4)
- Array and slice types (§5.2.2, §5.2.3)

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

This rule defines equivalence for the abstract calling signature. The complete function type identity—including representation kind (sparse function pointer vs. closure) and `move` parameter modifiers—is defined in §6.4 (T-Equiv-Func-Extended).

**(T-Equiv-Union)** Union Type Equivalence:
$$\frac{\text{multiset\_equiv}(\text{members}(T), \text{members}(U))}{\Gamma \vdash T \equiv U}$$

Union types are equivalent if their member type multisets are equal (order-independent).

**(T-Equiv-Permission)** Permission Equivalence:
$$\frac{P_1 = P_2 \quad \Gamma \vdash T \equiv U}{\Gamma \vdash P_1\ T \equiv P_2\ U}$$

The complete permission system is defined in §4.5.

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

A concrete type that implements a form is a subtype of that form. See §9.3 for form implementation and the formal rule (Sub-Form).

**Additional Subtyping Rules**

The following subtyping relationships are defined in their respective sections:

- Modal widening ($M@S <: M$, where $M$ is the general modal type): §6.1 (includes storage cost documentation)
- Union member subtyping: §5.5
- Refinement type subtyping: §7.3
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

The `const` permission enables covariant treatment of otherwise invariant container types (see §4.5.3 for the `const` permission semantics that guarantee immutability):

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

The formal subtyping rules are defined in §4.5.4.

---

#### 4.5.3 Detailed Permission Semantics

The following definitions specify the semantic meaning of each permission.

**`const` — Immutable Access (Default)**

The `const` permission grants read-only access to data and permits unlimited aliasing. Mutation through a `const` path is forbidden; any number of `const` paths to the same data may coexist simultaneously.

**`unique` — Exclusive Mutable Access**

The `unique` permission grants exclusive read-write access to data. The existence of a live `unique` path to an object statically precludes the existence of any other path to that object or its sub-components. A `unique` path may be temporarily downgraded to a weaker permission for a bounded scope; during this period, the original path is **inactive** and the binding becomes unusable until the scope ends. The `unique` permission does **not** imply cleanup responsibility; a procedure parameter of type `unique T` (without `move`) grants exclusive access while the caller retains responsibility for cleanup.

**`shared` — Key-Synchronized Mutable Access**

The `shared` permission grants mutable access to data through implicit key acquisition. Access requires a key, which is automatically acquired when accessing the data and released after use.

**Key Properties:**

- **Path-specific:** Keys are acquired at the granularity of the accessed path
- **Implicitly acquired:** Accessing a `shared` path acquires the necessary key automatically
- **Minimal scope:** Keys are held for the minimal duration preserving correctness
- **Reentrant:** If a covering key is already held, nested access succeeds without additional acquisition

The `shared` permission does **not** imply cleanup responsibility; a procedure parameter
of type `shared T` (without `move`) grants synchronized access while the caller retains
responsibility for cleanup.

The following table specifies which operations are permitted through a `shared` path:

| Operation                   | Permitted | Key Mode  |
| :-------------------------- | :-------- | :-------- |
| Field read                  | Yes       | Read key  |
| Field mutation              | Yes       | Write key |
| Method call (`~` receiver)  | Yes       | Read key  |
| Method call (`~%` receiver) | Yes       | Write key |
| Method call (`~!` receiver) | No        | N/A       |

**Key Modes:**

| Mode  | Grants       | Multiple Holders | Acquired By  |
| :---- | :----------- | :--------------- | :----------- |
| Read  | Read         | Yes              | Read access  |
| Write | Read + Write | No               | Write access |

> **Cross-Reference:** See §13 (Key System) and §4.5.5 (Receiver Compatibility).

**Exclusion Principle (Normative)**

The rules governing which permission-qualified paths may coexist for the same object are defined in this section. The **coexistence matrix** specifies which permissions may exist simultaneously:

| Active Permission | May Add `unique` | May Add `shared` | May Add `const` |
| :---------------- | :--------------- | :--------------- | :-------------- |
| `unique`          | No               | No               | No              |
| `shared`          | No               | Yes              | Yes             |
| `const`           | No               | Yes              | Yes             |

##### Syntax & Declaration

**Grammar**

```ebnf
permission       ::= "const" | "unique" | "shared"
permission_type  ::= [permission] type
```

When no permission is specified, `const` is implied.

**Shorthand Notation**

Receiver shorthand notation is defined in §2.7.

---

#### 4.5.4 Permission Subtyping Rules

##### Static Semantics

**Subtyping Rules**

The following rules formalize the permission subtyping lattice ($\texttt{unique} <: \texttt{shared} <: \texttt{const}$) defined in §4.5.2:

$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{shared}\ T} \quad \text{(Sub-Perm-US)}$$

$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{const}\ T} \quad \text{(Sub-Perm-UC)}$$

$$\frac{}{\Gamma \vdash \texttt{shared}\ T <: \texttt{const}\ T} \quad \text{(Sub-Perm-SC)}$$

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

---

#### 4.5.5 Method Receiver Permissions

**Receiver Compatibility Matrix**

A method with receiver permission $P_{\text{method}}$ is callable through a path with permission $P_{\text{caller}}$ if and only if $P_{\text{caller}} <: P_{\text{method}}$ in the permission subtyping lattice (§4.5.4).

> **Cross-Reference:** See §9.3 for complete method dispatch semantics, including form-based polymorphism and resolution rules.

| Caller's Permission | `~` (const) | `~!` (unique) | `~%` (shared) |
| :------------------ | :---------: | :-----------: | :-----------: |
| `const`             |      ✓      |       ✗       |       ✗       |
| `unique`            |      ✓      |       ✓       |       ✓       |
| `shared`            |      ✓      |       ✗       |       ✓       |

**Reading the matrix:** Row = permission of the caller's path to the receiver. Column = permission required by the method's receiver parameter. ✓ = method is callable. ✗ = method is not callable (error `E-TYP-1605`).

**Formal Rule (Receiver-Compat):**

$$\frac{
    \Gamma \vdash e : P_{\text{caller}}\ T \quad
    m \in \text{Methods}(T) \quad
    m.\text{receiver} = P_{\text{method}}\ \texttt{Self} \quad
    \Gamma \vdash P_{\text{caller}}\ T <: P_{\text{method}}\ T
}{
    \Gamma \vdash e.m(\ldots)\ \text{well-typed}
} \quad \text{(Receiver-Compat)}$$

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

A binding is **inactive** when its permission has been temporarily downgraded for a bounded scope. The binding state transitions are governed by the following formal rules:

**Binding State Machine**

A binding $b$ with `unique` permission exists in one of two states:

| State    | Definition                                        | Operations Permitted         |
| :------- | :------------------------------------------------ | :--------------------------- |
| Active   | No downgraded references to $b$ are live          | Read, write, move, downgrade |
| Inactive | One or more downgraded references to $b$ are live | None                         |

**State Transitions**

$$\text{Active} \xrightarrow{\text{downgrade begins}} \text{Inactive} \xrightarrow{\text{downgrade scope ends}} \text{Active}$$

The transitions are defined as follows:

**(Inactive-Enter)** When a `unique` binding $b$ is passed to a context expecting a weaker permission ($\texttt{const}$ or $\texttt{shared}$), $b$ transitions from Active to Inactive:

$$\frac{b : \texttt{unique}\ T \quad b\ \text{is Active} \quad \text{downgrade to}\ P\ \text{where}\ P \in \{\texttt{const}, \texttt{shared}\}}{b\ \text{becomes Inactive}}$$

**(Inactive-Exit)** When the scope containing the downgraded reference ends, $b$ transitions from Inactive to Active:

$$\frac{b\ \text{is Inactive} \quad \text{all downgraded references to}\ b\ \text{go out of scope}}{b\ \text{becomes Active with}\ \texttt{unique}\ \text{permission restored}}$$

**Invariants:**

1. During the inactive period, the original `unique` binding MUST NOT be read, written, or moved.
2. Any attempt to use an inactive binding MUST be rejected with diagnostic `E-TYP-1602`.
3. The transition back to Active occurs deterministically when the downgrade scope ends.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1601` | Error    | Attempt to mutate data via a `const` path.                                            | Compile-time | Rejection |
| `E-TYP-1602` | Error    | Violation of `unique` exclusion (aliasing detected or inactive use).                  | Compile-time | Rejection |
| `E-TYP-1604` | Error    | Direct field mutation through `shared` path.                                          | Compile-time | Rejection |
| `E-TYP-1605` | Error    | Method receiver permission incompatible with caller's permission (see §4.5.5 matrix). | Compile-time | Rejection |

**Canonical Diagnostic Codes**

Per Appendix B.4, type system diagnostics (`E-TYP-`) are canonical; memory model diagnostics (`E-MEM-`) are secondary.

---

## Clause 5: Data Types

**Universal Layout Principles**

The following principles apply to all composite types unless explicitly overridden:

1. **Padding**: The amount and placement of padding is implementation-defined (IDB).
2. **Alignment**: Type alignment is the maximum alignment of constituent parts unless overridden by layout attributes.
3. **Field/Element Ordering**: Physical ordering may differ from declaration order unless `[[layout(C)]]` is applied.
4. **Padding Byte Values**: The values of padding bytes are unspecified behavior (USB).

Individual type sections specify only type-specific layout rules.

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

Literal syntax is defined in §2.8 (Literals). The following specifies the default type inference for each literal category:

| Literal Category | Default Type | Type Suffix Available   |
| :--------------- | :----------- | :---------------------- |
| Integer          | `i32`        | Any integer type suffix |
| Floating-point   | `f64`        | `f32`, `f16`            |
| Boolean          | `bool`       | —                       |
| Character        | `char`       | —                       |
| Unit             | `()`         | —                       |

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

**Implicit Numeric Conversions**

There are **no implicit conversions** between distinct primitive types. A conforming implementation MUST reject any expression where a value of one primitive type is used in a context requiring a different primitive type without an explicit cast.

$$\frac{T \neq U \quad T \in \mathcal{T}_{\text{prim}} \quad U \in \mathcal{T}_{\text{prim}} \quad U \neq \texttt{!}}{\Gamma \nvdash T <: U} \quad \text{(No-Implicit-Prim-Coerce)}$$

##### Dynamic Semantics

**Integer Overflow**

Arithmetic operations on integer types (`+`, `-`, `*`) may overflow when the mathematical result is not representable in the result type.

A conforming implementation MUST provide a **checked mode** in which signed and unsigned integer overflow causes the executing thread to **panic**. This mode MUST be the default for debug builds.

The behavior of integer overflow in release (optimized) builds is **implementation-defined (IDB)**. Conforming implementations MAY choose one of the following behaviors for release mode:

1. **Wrap** — The result is the mathematical result modulo $2^N$ where $N$ is the bit width.
2. **Panic** — Same behavior as checked mode.
3. **Trap** — Invoke a platform-specific trap or signal.

Release-mode overflow behavior is IDB. The chosen behavior MUST apply uniformly to all integer overflow operations of the same category (signed vs. unsigned); an implementation MUST NOT vary overflow behavior on a per-operation or per-type basis within a single compilation.

> **Note:** Implementations MAY choose different behaviors for signed and unsigned overflow but MUST apply each choice consistently. Explicit wrapping, saturating, and checked arithmetic methods (e.g., `wrapping_add`, `saturating_mul`) are unaffected by build mode.

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

The alignment of `i128` and `u128` is implementation-defined (IDB). Conforming implementations MUST choose an alignment of 8 or 16 bytes.

**Platform-Dependent Types**

The pointer-sized integer types have sizes and alignments determined by the target platform:

| Type    | Size                   | Alignment              | Representation                  |
| :------ | :--------------------- | :--------------------- | :------------------------------ |
| `usize` | Platform pointer width | Platform pointer width | Unsigned integer                |
| `isize` | Platform pointer width | Platform pointer width | Two's complement signed integer |

On 32-bit platforms, `usize` and `isize` are 4 bytes with 4-byte alignment. On 64-bit platforms, they are 8 bytes with 8-byte alignment. The exact pointer width is implementation-defined (IDB).

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

Tuple type equivalence is structural, per rule (T-Equiv-Tuple) in §4.1.

**Subtyping**

Tuple subtyping is covariant in all component positions:

$$\frac{n = m \quad \forall i \in 1..n,\ \Gamma \vdash T_i <: U_i}{\Gamma \vdash (T_1, \ldots, T_n) <: (U_1, \ldots, U_m)} \quad \text{(Sub-Tuple)}$$

Permission propagates per §4.5.

##### Memory & Layout

**Representation**

A tuple is laid out as a contiguous sequence of its component values. Components are stored in declaration order (left to right).

**Size and Alignment**

$$\text{sizeof}((T_1, \ldots, T_n)) = \sum_{i=1}^{n} \text{sizeof}(T_i) + \text{padding}$$

$$\text{alignof}((T_1, \ldots, T_n)) = \max_{i \in 1..n}(\text{alignof}(T_i))$$

For the unit type: $\text{sizeof}(()) = 0$ and $\text{alignof}(()) = 1$.

Padding follows Universal Layout Principles (Clause 5 introduction).

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

Permission propagates per §4.5.

##### Dynamic Semantics

**Bounds Checking**

**Bounds Checking**: All array indexing is bounds-checked at runtime per §11.4.2.

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

**Diagnostic Table**

| Code         | Severity | Condition                                                     | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1810` | Error    | Array length is not a compile-time constant.                  | Compile-time | Rejection |
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

where $P$ is the permission of the array and Range is any range type from §5.2.4.

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

Permission propagates per §4.5.

##### Dynamic Semantics

**Bounds Checking**: All slice indexing is bounds-checked at runtime per §11.4.2.

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

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-1820` | Error    | Slice index expression has non-`usize` type. | Compile-time | Rejection |
| `P-TYP-1822` | Panic    | Slice range out of bounds.                   | Runtime      | Panic     |
| `E-TYP-1823` | Error    | Slice outlives borrowed storage.             | Compile-time | Rejection |

#### 5.2.4 Range Types

##### Definition

**Range types** are compiler-intrinsic types with record-like syntax produced by range expressions. Ranges represent bounded or unbounded intervals and are primarily used for iteration and slicing operations. Although range types have structural equivalence semantics and expose public fields like records, they are built-in to the compiler rather than user-definable. Implementations MUST provide these types with the specified field structure and semantics.

**Formal Definition**

The set of range types comprises six distinct compiler-intrinsic types:

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

Padding within range types follows the same implementation-defined rules as records (§5.3).

##### Constraints & Legality

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

record_body      ::= field_decl ("," field_decl)* ","?

field_decl       ::= [visibility] identifier ":" type

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

A record declaration MAY include a `where` clause to specify a type invariant. The syntax, semantics, predicate context, and enforcement points are defined in §10.3.1 (Type Invariants).

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

Record types have nominal equivalence per §4.1 (T-Equiv-Nominal). Two records with identical structure but different names are distinct types.

**Field Visibility**

Record fields use the same visibility modifiers defined in §8.5 (Visibility and Access Control). The default visibility for record fields is `private`. Field visibility MUST NOT exceed the visibility of the enclosing record type.

**Form Implementation**

A record declaration MAY implement forms using the `<:` syntax. See §9.3 for form implementation syntax and semantics.

**Invariant Enforcement**

Invariant enforcement modes and verification strategies are defined in §10.4.

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

A record is laid out as a contiguous sequence of its field values. Layout follows Universal Layout Principles (Clause 5 introduction).

**Size and Alignment**

$$\text{alignof}(R) = \max_{f \in \text{fields}(R)}(\text{alignof}(T_f))$$

$$\text{sizeof}(R) \geq \sum_{f \in \text{fields}(R)} \text{sizeof}(T_f)$$

**C-Compatible Layout**

When the `[[layout(C)]]` attribute is applied to a record declaration, the implementation MUST produce a C-compatible memory layout as specified in §7.2.1 (The `[[layout(...)]]` Attribute).

**Zero-Field Records**

A record with no fields has size 0 and alignment 1:

$$\text{sizeof}(\texttt{record}\ R\ \{\}) = 0 \qquad \text{alignof}(\texttt{record}\ R\ \{\}) = 1$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-TYP-1901` | Error    | Duplicate field name in record declaration.    | Compile-time | Rejection |
| `E-TYP-1902` | Error    | Missing field initializer in record literal.   | Compile-time | Rejection |
| `E-TYP-1903` | Error    | Duplicate field initializer in record literal. | Compile-time | Rejection |
| `E-TYP-1904` | Error    | Access to nonexistent field.                   | Compile-time | Rejection |
| `E-TYP-1905` | Error    | Access to field not visible in current scope.  | Compile-time | Rejection |
| `E-TYP-1906` | Error    | Field visibility exceeds record visibility.    | Compile-time | Rejection |
| `P-TYP-1909` | Panic    | Type invariant violated at construction.       | Runtime      | Panic     |
| `P-TYP-1910` | Panic    | Type invariant violated at procedure boundary. | Runtime      | Panic     |

---

### 5.4 Enums

##### Definition

An **enum** (enumerated type) is a nominal sum type, also known as a tagged union. An enum value is exactly one of several defined **variants**, each of which may carry a payload of associated data. Unlike union types (§5.5), which are structural and anonymous, enums are nominal: two enum types with identical variants but different names are distinct types.

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
```

The `field_decl`, `implements_clause`, and `type_invariant` productions are defined in §5.3.

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

For tuple-like variants, the arguments are positional expressions. For record-like variants, the arguments are named field initializers following the same rules as record literals (§5.3), including support for field shorthand.

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

An enum declaration MAY include a `where` clause to specify a type invariant. The syntax, semantics, predicate context, and enforcement points are defined in §10.3.1 (Type Invariants).

When the invariant predicate requires access to variant-specific data, it MUST use a `match` expression:

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

Enum types have nominal equivalence per §4.1 (T-Equiv-Nominal). Two enums with identical variants but different names are distinct types.

**Discriminant Assignment**

Discriminant values are assigned as follows:

1. If a variant specifies an explicit discriminant value, that value is assigned to the variant.
2. If a variant does not specify an explicit discriminant and is the first variant, it is assigned the value `0`.
3. If a variant does not specify an explicit discriminant and is not the first variant, it is assigned the value of the previous variant's discriminant plus one.

Discriminant values MUST be unique within an enum. The discriminant type is implementation-defined but MUST be an integer type large enough to represent all discriminant values.

**Discriminant Overflow**

If the implicit discriminant value of a variant would exceed the maximum value of the discriminant type (due to sequential assignment following an explicit discriminant), the program is ill-formed.

**Variant Access**

Accessing the data stored within an enum variant MUST be performed using a `match` expression (§11.2). Direct field access on an enum value is forbidden; the active variant must first be determined through pattern matching.

**Exhaustiveness**

A `match` expression on an enum type MUST be exhaustive: the set of patterns in its arms, taken together, MUST cover every variant of the enum. Exhaustiveness checking is mandatory for all enum types. See §11.2 for pattern matching semantics.

**Form Implementation**

An enum declaration MAY implement forms using the `<:` syntax. See §9.3 for form implementation syntax and semantics.

**Invariant Enforcement**

Invariant enforcement modes and verification strategies are defined in §10.4.

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

**Niche Optimization (Canonical Definition)**

A **niche** is an invalid bit pattern within a type that cannot represent a valid value. **Niche optimization** is an implementation technique that repurposes niches to encode discriminant information, potentially eliding separate discriminant storage.

Implementations SHOULD apply niche optimization to enums when variant payloads contain invalid bit patterns (niches) that can unambiguously encode the discriminant.

> **Cross-Reference:** See §5.5 for union types (SHOULD apply) and §6.1 for modal types (MUST apply).

**C-Compatible Layout**

When an enum is annotated with `[[layout(C)]]` or `[[layout(IntType)]]` (e.g., `[[layout(u8)]]`), the implementation MUST use a C-compatible layout as specified in §7.2.1 (The `[[layout(...)]]` Attribute).

##### Constraints & Legality

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
| `P-TYP-2011` | Panic    | Type invariant violated at construction.                    | Runtime      | Panic     |
| `P-TYP-2012` | Panic    | Type invariant violated at procedure boundary.              | Runtime      | Panic     |

See §11.2 for pattern exhaustiveness diagnostics (`E-PAT-2741`).

---

### 5.5 Union Types

##### Definition

A **union type** is a structural anonymous sum type representing a value that may be one of several distinct types. Union types are written using the pipe operator (`|`) to combine member types. Unlike enums (§5.4), which are nominal and require explicit declaration, union types are structural: two union types are equivalent if they have the same member types, regardless of the order in which those members appear.

A **member type** is one of the component types that constitutes the union. The set of member types defines the possible runtime values that an expression of the union type may hold.

Union types in Cursive are **safe** and **tagged**. Every union value carries a runtime discriminant that identifies the currently active member type, enabling type-safe access through pattern matching.

**Formal Definition**

Let $\mathcal{T}$ denote the universe of all types. A union type $U$ is defined by a non-empty multiset of member types:

$$U = \bigcup_{i=1}^{n} T_i \quad \text{where } n \geq 2 \text{ and } T_i \in \mathcal{T}$$

The union type is denoted syntactically as $T_1 \mid T_2 \mid \cdots \mid T_n$.

The **membership relation** $\in_U$ defines which types are members of a union:

$$T \in_U (T_1 \mid T_2 \mid \cdots \mid T_n) \iff \exists i \in 1..n : T \equiv T_i$$

Two union types are **equivalent** if and only if they have the same member type multisets, per rule (T-Equiv-Union) defined in §4.1.

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

**Recursion Detection**

A union type $U$ is **recursive** if $U$ appears in its own member list without intervening pointer indirection. Formally:

$$\text{Recursive}(U) \iff U \in_U U \land \neg\text{IndirectedOccurrence}(U, U)$$

where $\text{IndirectedOccurrence}(T, U)$ holds when every occurrence of $T$ within the member types of $U$ appears behind a pointer constructor. The following type constructors provide indirection:

- `Ptr<T>` — the safe pointer type
- `const T`, `unique T`, `shared T` — permission-qualified pointer types

A recursive union type without indirection has infinite size and is ill-formed (diagnostic `E-TYP-2203`).

**Example of well-formed recursive union:**
```cursive
type Node = i32 | Ptr<Node>  // OK: recursion behind Ptr
```

**Example of ill-formed recursive union:**
```cursive
type Bad = i32 | Bad         // ERROR E-TYP-2203: infinite type
```

**(T-Union-Match)** Union Elimination:
A union value MUST be accessed through exhaustive pattern matching:
$$\frac{\Gamma \vdash e : U \quad U = T_1 \mid \cdots \mid T_n \quad \forall i,\ \Gamma, x_i : T_i \vdash e_i : R}{\Gamma \vdash \texttt{match } e\ \{x_1 : T_1 \Rightarrow e_1, \ldots, x_n : T_n \Rightarrow e_n\} : R} \quad \text{(T-Union-Match)}$$

**Type Equivalence**

Union type equivalence follows (T-Equiv-Union). Equivalence is order-independent:

$$\Gamma \vdash (A \mid B) \equiv (B \mid A)$$

Duplicate members are distinct entries:

$$\Gamma \vdash (A \mid A) \not\equiv A$$

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

A `match` expression over a union type MUST be exhaustive. The set of patterns MUST cover all member types of the union (§11.2).

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

Niche optimization (see §5.4, Niche Optimization under Memory & Layout) SHOULD be applied to union types.

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

## Clause 6: Behavioral Types

---

### 6.1 Modal Types

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
modal_decl        ::= [visibility] "modal" identifier [generic_params]
                      [implements_clause] "{" state_block+ "}"

state_block       ::= "@" state_name [state_payload] [state_members]

state_name        ::= identifier

state_payload     ::= "{" (field_decl ("," field_decl)* ","?)? "}"

state_members     ::= "{" (method_def | transition_def)* "}"

method_def        ::= "procedure" identifier "(" param_list ")" ["->" return_type] block

transition_def    ::= "transition" identifier "(" param_list ")" "->" "@" target_state block

target_state      ::= identifier
```

The `implements_clause` production is defined in §5.3 (Records).

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

Modal widening converts a state-specific type `M@S` to the general modal type `M`:

$$\frac{\Gamma \vdash e : M@S \quad S \in \text{States}(M)}{\Gamma \vdash \texttt{widen } e : M} \quad \text{(T-Modal-Widen)}$$

**Widening Rules Summary:**

| Type Category               | `widen` Keyword | Implicit Coercion | Subtyping      |
| :-------------------------- | :-------------- | :---------------- | :------------- |
| Non-niche-layout-compatible | Required        | Prohibited        | $M@S \not<: M$ |
| Niche-layout-compatible     | Optional        | Permitted         | $M@S <: M$     |

A state-specific type $M@S$ is **niche-layout-compatible** with $M$ when all of the following conditions hold:

1. **Niche Optimization Applies:** The modal type uses niche encoding (see §5.4, Niche Optimization under Memory & Layout) rather than an explicit discriminant field. This requires at least one payload field with an unused bit pattern that can serve as a state discriminant.

2. **Size Equality:** The state-specific type has the same size as the general type: $\text{sizeof}(M@S) = \text{sizeof}(M)$.

3. **Alignment Equality:** The state-specific type has the same alignment as the general type: $\text{alignof}(M@S) = \text{alignof}(M)$.

4. **No Discriminant Storage Overhead:** The discriminant is encoded entirely within niche bits of the payload; no additional storage is required for the state tag.

When these conditions hold, converting from $M@S$ to $M$ is a no-op at runtime—the bit representation is identical, and only the type system's view changes.

**Widen Expression Constraints:**

1. The operand of `widen` MUST have a state-specific modal type `M@S` (diagnostic `E-TYP-2071` if non-modal).
2. The result type is the general modal type `M`.
3. Applying `widen` to an already-general modal type is ill-formed (diagnostic `E-TYP-2072`).

For storage costs and runtime widening semantics, see **Modal Widening Operation** under Dynamic Semantics below.

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

**Niche Optimization**

Niche optimization (see §5.4, Niche Optimization under Memory & Layout) MUST be applied to modal types. When applicable, the general type has the same size as the largest state-specific type.

**Modal Widening Operation**

Modal widening converts a state-specific type $M@S$ to the general modal type $M$ via the **explicit `widen` keyword**.

**Semantic Operation**

When `widen e` is evaluated where $e : M@S$:

1. Storage for the general representation is allocated (stack or destination).
2. The payload from $e$ is moved into the corresponding storage region of the general representation.
3. The discriminant (or niche encoding) for state $@S$ is written.
4. The resulting value has type $M$.

**Size Relationships:**

$\text{sizeof}(M@S) \leq \text{sizeof}(M)$

The inequality is strict when multiple states exist with different payload sizes AND niche optimization does not fully eliminate the discriminant. The inequality is an equality when only one state exists OR niche optimization fully absorbs the discriminant into payload bits.

**Example — Niche-Layout-Compatible Type:**

For niche-layout-compatible types like `Ptr<T>` (defined in §6.3), implicit widening is zero-cost:

```cursive
let p1: Ptr<Buffer>@Valid = &buffer
let p2: Ptr<Buffer> = p1        // Implicit widening: zero cost (niche-layout-compatible)
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

The threshold for `W-OPT-4010` is Implementation-Defined but SHOULD be configurable. A reasonable default is 256 bytes.

##### Constraints & Legality

Modal type constraints are enforced by typing rules (T-Modal-*). Key constraints:

1. A modal type MUST declare at least one state.
2. Direct access on general modal type $M$ requires pattern matching.
3. Match expressions on $M$ MUST be exhaustive.

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

The `string` type is defined as a member of the modal type family (§6.1):

$$\text{States}(\texttt{string}) = \{\ \texttt{@Managed},\ \texttt{@View}\ \}$$

The type family induced by the `string` modal type is:

$$\mathcal{T}_{\texttt{string}} = \{\ \texttt{string},\ \texttt{string@Managed},\ \texttt{string@View}\ \}$$

Per the modal incomparability rule (§6.1), `string@Managed` and `string@View` are pairwise incomparable. Conversion between states requires explicit operations (see Dynamic Semantics).

##### Syntax & Declaration

The `string` type is a built-in primitive; it is not user-declarable.

**Grammar**

```ebnf
string_type         ::= "string" ["@" string_state]

string_state        ::= "Managed" | "View"
```

String literal syntax and escape sequences are defined in §2.8.

##### Static Semantics

**Typing Rules**

**(T-String-Literal)** String Literal Typing:

A string literal has type `string@View`. The literal content MUST be valid UTF-8:

$$\frac{\Gamma \vdash s \text{ is a valid string literal}}{\Gamma \vdash s : \texttt{string@View}} \quad \text{(T-String-Literal)}$$

**(T-String-Slice)** Slice Typing:

A slice operation on any string type produces a `string@View`:

$$\frac{\Gamma \vdash e : \texttt{string@}S \quad S \in \{\texttt{Managed}, \texttt{View}\} \quad \Gamma \vdash a : \texttt{usize} \quad \Gamma \vdash b : \texttt{usize}}{\Gamma \vdash e[a..b] : \texttt{string@View}} \quad \text{(T-String-Slice)}$$

**Modal Widening**

Both state-specific types are subtypes of the general modal type `string` per the modal widening rule (§6.1):

$$\frac{S \in \{\texttt{@Managed}, \texttt{@View}\}}{\Gamma \vdash \texttt{string@}S <: \texttt{string}} \quad \text{(Sub-Modal-Widen)}$$

A procedure accepting the general type `string` may receive either state; the active state is determined via pattern matching.

**Form Implementations**

| State            | `Copy` | `Clone` | `Drop` |
| :--------------- | :----- | :------ | :----- |
| `string@Managed` | No     | No      | Yes    |
| `string@View`    | Yes    | Yes     | No     |

The following constraints apply per Appendix D.1:

1. `string@Managed` MUST NOT implement `Copy` because it owns a heap allocation.
2. `string@Managed` MUST NOT implement `Clone`. This is a design choice, not a technical limitation: since cloning a managed string requires heap allocation, the `Clone` form would need access to a `HeapAllocator` capability, which the standard `Clone::clone` signature does not provide. Duplication MUST use the explicit `clone_with(heap)` method to inject the required capability (see Dynamic Semantics).
3. `string@Managed` MUST implement `Drop` to deallocate its buffer when responsibility ends.
4. `string@View` MUST implement `Copy` because it is a non-owning pointer-length pair.
5. `string@View` MUST implement `Clone` (implied by `Copy` per Appendix D.1).

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

The general modal type `string` uses standard modal type layout (§6.1): a discriminant plus a payload region sized to the largest state.

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

The safe pointer type integrates with the modal type system (§6.1) to encode pointer validity as a static property. The raw pointer types bypass this system entirely, delegating all safety responsibility to the programmer.

**Formal Definition**

The `Ptr<T>` type is defined as a member of the modal type family (§6.1):

$$\text{States}(\texttt{Ptr<T>}) = \{\ \texttt{@Valid},\ \texttt{@Null},\ \texttt{@Expired}\ \}$$

The type family induced by the `Ptr<T>` modal type is:

$$\mathcal{T}_{\texttt{Ptr<T>}} = \{\ \texttt{Ptr<T>},\ \texttt{Ptr<T>@Valid},\ \texttt{Ptr<T>@Null},\ \texttt{Ptr<T>@Expired}\ \}$$

Per §6.1, `Ptr<T>` state-specific types are pairwise incomparable.

The raw pointer types form a separate type family:

$$\mathcal{T}_{\text{RawPtr}} = \{\ \texttt{*imm T},\ \texttt{*mut T}\ :\ T \in \mathcal{T}\ \}$$

where $\mathcal{T}$ is the set of all types.

##### Syntax & Declaration

The `Ptr<T>` type is a built-in primitive; it is not user-declarable.

**Grammar**

```ebnf
safe_pointer_type     ::= "Ptr" "<" type ">" ["@" pointer_state]

pointer_state         ::= "Valid" | "Null" | "Expired"

raw_pointer_type      ::= "*" raw_pointer_qual type

raw_pointer_qual      ::= "imm" | "mut"
```

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

The following constraints apply per Appendix D.1:

1. All pointer types (safe and raw) MUST implement `Copy` because they are address values.
2. All pointer types MUST implement `Clone` (implied by `Copy` per Appendix D.1).
3. No pointer type implements `Drop`. Pointers do not own the memory they reference; the referenced data's lifetime is managed by the responsibility system (§3.5) or region system (§3.7).

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

> **Cross-Reference:** The evaluation semantics of the dereference operator `*` are defined in §11.4.5 (Unary Operators).

**Region Exit State Transition**

When a region block (§3.7) exits:

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

The safe pointer type `Ptr<T>` uses niche optimization (§5.4) to achieve a single-word representation:

| State      | Representation                    |
| :--------- | :-------------------------------- |
| `@Valid`   | Non-zero address (pointer to `T`) |
| `@Null`    | Address `0x0`                     |
| `@Expired` | Formerly-valid address            |

$$\text{sizeof}(\texttt{Ptr<T>}) = \text{sizeof}(\texttt{usize})$$

$$\text{alignof}(\texttt{Ptr<T>}) = \text{alignof}(\texttt{usize})$$

The general modal type `Ptr<T>` MUST occupy exactly one machine word. The null address (`0x0`) serves as an implicit discriminant for the `@Null` state. Non-zero addresses indicate either `@Valid` or `@Expired`; the distinction between these states is tracked statically, not at runtime.

**The `@Expired` State is Compile-Time Only:** The `@Expired` state has no runtime representation and no discriminant value. At runtime, an expired pointer is indistinguishable from a valid pointer—both hold a non-zero address. The `@Expired` state exists purely within the type system to enable precise static error messages (distinguishing use-after-free from null-pointer dereference). The compiler statically prevents any code path where an `@Expired` pointer could be dereferenced or widened to the general type; consequently, no runtime check is needed or performed.

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

**Type Equivalence (T-Equiv-Func-Extended)**

Function type equivalence is governed by rule **(T-Equiv-Func)** defined in §4.1. For the full function type representation defined in this section, equivalence additionally requires matching representation kinds and `move` modifiers:

$$\frac{\begin{gathered}
\kappa_F = \kappa_G \quad n = k \\
\forall i \in 1..n,\ (m_i = m'_i \land \Gamma \vdash T_i \equiv U_i) \\
\Gamma \vdash R_F \equiv R_G
\end{gathered}}{\Gamma \vdash F \equiv G} \quad \text{(T-Equiv-Func-Extended)}$$

where $F = (\kappa_F,\ m_1\ T_1, \ldots, m_n\ T_n,\ R_F)$ and $G = (\kappa_G,\ m'_1\ U_1, \ldots, m'_k\ U_k,\ R_G)$.

This rule extends §4.1's **(T-Equiv-Func)** by incorporating representation kind ($\kappa$) and `move` modifiers ($m_i$) into the equivalence check.

> **Note:** The types `(move Buffer) -> ()` and `(Buffer) -> ()` are NOT equivalent. The types `(i32) -> bool` and `|i32| -> bool` are NOT equivalent.

**When Representation Kind Matters**

The distinction between sparse function pointers `(T) -> U` and closures `|T| -> U` affects type checking in the following contexts:

1. **Storage and Passing:** Closures require two machine words (code pointer + environment pointer), while sparse function pointers require one. A binding of type `|T| -> U` cannot hold a value of type `(T) -> U` without implicit coercion (which is permitted via subtyping, see below).

2. **FFI Boundaries:** Only sparse function pointer types are FFI-safe (§12). A procedure accepting a callback from foreign code MUST declare that parameter with sparse function pointer type `(T) -> U`, not closure type `|T| -> U`.

3. **Environment Capture:** A closure type indicates that the callable may hold references to captured bindings from its defining scope. Procedures may require this property (accepting `|T| -> U`) when the callback must close over external state, or prohibit it (accepting `(T) -> U`) when callbacks must be stateless.

4. **Coercion Direction:** A sparse function pointer coerces to a closure (one direction only). Code accepting `|T| -> U` can receive either representation; code accepting `(T) -> U` can receive only non-capturing callables.

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

By the transitivity of subtyping (§4.2), the rules above compose. For example, a sparse non-`move` function is a subtype of a closure `move` function with compatible types:

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
| `E-TYP-2220` | Error    | Argument count mismatch: expected $n$ arguments, found $m$.  | Compile-time | Rejection |
| `E-TYP-2221` | Error    | Type mismatch in function argument or return position.       | Compile-time | Rejection |
| `E-TYP-2222` | Error    | Missing `move` on argument to `move` parameter.              | Compile-time | Rejection |
| `E-TYP-2223` | Error    | Closure type in `extern` procedure signature.                | Compile-time | Rejection |
| `E-TYP-2224` | Error    | Assignment of closure value to sparse function pointer type. | Compile-time | Rejection |
| `E-TYP-2225` | Error    | `move` modifier on return type.                              | Compile-time | Rejection |

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
form_bound_list    ::= form_bound ("," form_bound)*

where_clause       ::= "where" where_predicate_list
where_predicate_list ::= where_predicate ("," where_predicate)*
where_predicate    ::= identifier "<:" form_bound_list

generic_args       ::= "<" type_arg_list ">"
type_arg_list      ::= type ("," type)*

turbofish          ::= "::" generic_args
```

The `form_bound` production is defined in §9.2 (Form Declarations).

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

> **Warning:** The final example above demonstrates a common source of confusion. The syntax `<T <: A, U>` does **not** declare two parameters where `T` is bounded and `U` is unconstrained. Instead, it declares a single parameter `T` bounded by both `A` and `U`. This is because identifiers following a comma after `<:` are parsed as additional bounds unless they are themselves followed by `<:`. To declare bounded parameters followed by unconstrained parameters, use a `where` clause as shown below.

To declare bounded parameters followed by unconstrained parameters, use a `where` clause:

```cursive
procedure example<T, U>(x: T, y: U) where T <: Display { ... }
```

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

The variance of each type parameter is determined by its usage within the type definition. See §4.3 for the complete variance specification and subtyping rules for generic types.

**Monomorphization Requirements**

Implementations MUST generate specialized code for each distinct combination of concrete type arguments used in the program. The following requirements apply:

1. **Specialization:** For each instantiation $D\langle A_1, \ldots, A_n \rangle$, the implementation MUST produce code equivalent to manually substituting each type argument for its corresponding parameter throughout the declaration body.

2. **Zero Overhead:** Calls to generic procedures MUST compile to direct static calls to the specialized instantiation. Virtual dispatch (vtable lookup) is prohibited for static polymorphism.

3. **Independent Instantiation:** Each distinct instantiation is an independent type or procedure. `Container<i32>` and `Container<i64>` are distinct types with no implicit relationship.

**Recursion Depth Limit**

Monomorphization MAY produce recursive instantiations (e.g., `List<Option<List<T>>>`). Implementations MUST detect and reject infinite monomorphization recursion.

The maximum instantiation depth is IDB. Implementations MUST support a minimum depth of 128 levels.

> **Note:** Infinite recursion typically arises from patterns like `procedure f<T>() { f::<Option<T>>() }` where each instantiation triggers another with a strictly larger type.

##### Memory & Layout

**Layout Independence**

Each monomorphized instantiation has an independent memory layout. The layout of `Container<i32>` is computed using the rules for `i32` as the element type; the layout of `Container<i64>` is computed independently using `i64`.

**Size and Alignment**

The size and alignment of a generic type instantiation are determined by substituting the concrete type arguments and applying the layout rules for the resulting concrete type (see Universal Layout Principles in §5 and §7.2.1).

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

An **attribute** is a compile-time annotation attached to a declaration or expression that provides metadata to the compiler. Attributes influence code generation, memory layout, diagnostics, verification strategies, and interoperability without altering the core semantics of the annotated construct except where this specification explicitly defines such alteration. Attributes may also be attached to expressions, enabling fine-grained control over compiler behavior at specific program points.

**Formal Definition**

An attribute $A$ is defined by:

$$A = (\text{Name}, \text{Args})$$

where:

- $\text{Name}$ is an identifier designating the attribute
- $\text{Args}$ is a (possibly empty) sequence of arguments

An **attribute list** is a sequence of one or more attributes applied to a declaration or expression:

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

expression         ::= attributed_expr | unattributed_expr
attributed_expr    ::= attribute_list expression
```

**Placement**

**Declaration-Level Attributes**

An attribute list MUST appear immediately before the declaration it modifies. The attribute list and its target declaration MUST NOT be separated by other declarations or statements.

**Expression-Level Attributes**

An attribute list MAY appear immediately before an expression. The attributed expression has the same type as the underlying expression; the attribute affects only compiler behavior (e.g., verification strategy) not the expression's value or type.

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

The complete set of specification-defined attributes ($\mathcal{R}_{\text{spec}}$) and their full semantics are defined in subsections §7.2.1–§7.2.9 below.

**Attribute Target Matrix**

The following matrix defines which targets are valid for each specification-defined attribute. Applying an attribute to an invalid target is ill-formed (`E-DEC-2452`).

| Attribute             | `record` | `enum` | `modal` | `procedure` | Field | Parameter | `expression` |
| :-------------------- | :------: | :----: | :-----: | :---------: | :---: | :-------: | :----------: |
| `[[layout(...)]]`     |   Yes    |  Yes   |   No    |     No      |  No   |    No     |      No      |
| `[[inline(...)]]`     |    No    |   No   |   No    |     Yes     |  No   |    No     |      No      |
| `[[cold]]`            |    No    |   No   |   No    |     Yes     |  No   |    No     |      No      |
| `[[deprecated(...)]]` |   Yes    |  Yes   |   Yes   |     Yes     |  Yes  |    No     |      No      |
| `[[reflect]]`         |   Yes    |  Yes   |   Yes   |     No      |  No   |    No     |      No      |
| `[[link_name(...)]]`  |    No    |   No   |   No    |  Yes (FFI)  |  No   |    No     |      No      |
| `[[no_mangle]]`       |    No    |   No   |   No    |  Yes (FFI)  |  No   |    No     |      No      |
| `[[unwind(...)]]`     |    No    |   No   |   No    |  Yes (FFI)  |  No   |    No     |      No      |
| `[[dynamic]]`         |   Yes    |  Yes   |   Yes   |     Yes     |  No   |    No     |     Yes      |
| `[[relaxed]]`         |    No    |   No   |   No    |     No      |  No   |    No     |     Yes      |
| `[[acquire]]`         |    No    |   No   |   No    |     No      |  No   |    No     |     Yes      |
| `[[release]]`         |    No    |   No   |   No    |     No      |  No   |    No     |     Yes      |
| `[[acq_rel]]`         |    No    |   No   |   No    |     No      |  No   |    No     |     Yes      |
| `[[seq_cst]]`         |    No    |   No   |   No    |     No      |  No   |    No     |     Yes      |

> **Note:** "Yes (FFI)" indicates the attribute is valid only on `extern` procedure declarations. Memory ordering attributes and `[[dynamic]]` may target expressions; memory ordering attributes are valid only on expressions performing memory access to `shared` data.

**Vendor Extension Namespacing**

Vendor-defined attributes MUST use a namespaced identifier to prevent collision with specification-defined attributes and other vendors' extensions:

1. Vendor attributes MUST use reverse-domain-style prefixes: `[[com.vendor.attribute_name]]`
2. The `cursive.*` namespace is reserved (§1.3).
3. Vendor-defined attributes are IDB.

An implementation encountering an unknown attribute (one not in $\mathcal{R}$) MUST diagnose error `E-DEC-2451`.

---

#### 7.2.1 The `[[layout(...)]]` Attribute

##### Definition

The `[[layout(...)]]` attribute controls the memory representation of composite types. It specifies field ordering, padding behavior, alignment requirements, and discriminant representation.

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
4. Full semantics for compile-time introspection are defined in Clause 17 (Metaprogramming).

---

#### 7.2.6 FFI Attributes

##### Definition

**FFI Attributes** are attributes specific to Foreign Function Interface declarations. These attributes control linking behavior and panic propagation across the language boundary.

##### Static Semantics

The following FFI attributes are recognized. Complete semantics are defined in Clause 18 (Interoperability):

| Attribute              | Reference | Effect                                                       |
| :--------------------- | :-------- | :----------------------------------------------------------- |
| `[[link_name("sym")]]` | §18.4     | Overrides the linker symbol name for an extern procedure     |
| `[[no_mangle]]`        | §18.4     | Disables name mangling (implicit for `extern "C"`)           |
| `[[unwind(mode)]]`     | §18.5     | Controls panic behavior at FFI boundary (`abort` or `catch`) |

These attributes are valid only on `extern` procedure declarations.

---

#### 7.2.7 Memory Ordering Attributes

##### Definition

**Memory Ordering Attributes** control the memory ordering semantics of individual memory operations in concurrent code. These attributes allow relaxation of the default sequential consistency ordering for performance-critical operations.

##### Static Semantics

The following memory ordering attributes are recognized. Complete semantics are defined in §13.10 (Memory Ordering):

| Attribute     | Target        | Effect                                                          |
| :------------ | :------------ | :-------------------------------------------------------------- |
| `[[relaxed]]` | Memory access | Atomicity only—no ordering guarantees                           |
| `[[acquire]]` | Memory access | Subsequent reads see prior writes                               |
| `[[release]]` | Memory access | Prior writes visible to acquire reads                           |
| `[[acq_rel]]` | Memory access | Both acquire and release semantics                              |
| `[[seq_cst]]` | Memory access | Total global order (default; explicit annotation rarely needed) |

These attributes are valid only on expressions that perform memory access to `shared` data.

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

> **Note:** Attribute diagnostics use the `E-DEC-` (Declarations) prefix because attribute validation is performed as part of declaration processing. Although attributes are defined in this clause (Clause 7), their diagnostics fall under the Declarations category per Appendix B.2.

---

#### 7.2.9 The `[[dynamic]]` Attribute

##### Definition

The **`[[dynamic]]` attribute** marks a declaration or expression as requiring **runtime verification** when static verification is insufficient. This attribute is the explicit opt-in mechanism for runtime checks, reflecting the language's philosophy of **static safety by default**.

**Design Rationale**

Cursive's verification model is "static by default": the compiler must prove safety properties (key safety, contracts, refinement predicates) at compile time, or the program is ill-formed. The `[[dynamic]]` attribute provides an explicit escape hatch when:

1. Static analysis cannot prove safety due to inherent undecidability
2. Dynamic information is required (user input, external data)
3. Performance trade-offs favor runtime checks over complex static proofs

##### Syntax & Declaration

**Grammar**

```ebnf
dynamic_attribute     ::= "[[" "dynamic" "]]"
attributed_decl       ::= dynamic_attribute declaration
attributed_expr       ::= dynamic_attribute expression
```

**Valid Targets**

| Target       | Effect                                                     |
| :----------- | :--------------------------------------------------------- |
| `record`     | All operations on instances may use runtime verification   |
| `enum`       | All operations on instances may use runtime verification   |
| `modal`      | All state transitions may use runtime verification         |
| `procedure`  | Contracts and body operations may use runtime verification |
| `expression` | The specific expression may use runtime verification       |

##### Static Semantics

**Propagation Rules**

The `[[dynamic]]` attribute establishes a **dynamic context** that propagates inward:

1. **Declaration-Level Propagation**: When `[[dynamic]]` is applied to a type or procedure declaration, all expressions within that declaration are within a dynamic context.

2. **Expression-Level Scope**: When `[[dynamic]]` is applied to an expression, only that expression and its sub-expressions are within a dynamic context.

3. **Lexical Scoping**: The dynamic context is determined lexically, not dynamically. A procedure call from a dynamic context does not make the callee dynamic.

4. **No Transitive Propagation**: The `[[dynamic]]` attribute does not propagate through procedure calls. Each procedure's verification context is determined by its own attributes.

**Effect on Key System (§13)**

Within a `[[dynamic]]` context:

- If static key safety analysis fails, the compiler MAY insert runtime synchronization
- The rules K-Static-Required and K-Dynamic-Permitted (§13.9) govern this behavior
- Same-statement conflicts (§13.6.1) remain compile-time errors regardless of `[[dynamic]]`

**Effect on Contract System (§10)**

Within a `[[dynamic]]` context:

- If a contract predicate cannot be proven statically, a runtime check is inserted
- Failed runtime checks result in panic (`P-CON-2850`)
- The rules Contract-Static-OK, Contract-Static-Fail, Contract-Dynamic-Elide, Contract-Dynamic-Check (§10.4) govern this behavior

**Effect on Refinement Types (§7.3)**

Within a `[[dynamic]]` context:

- If a refinement predicate cannot be proven statically, a runtime check is inserted
- Failed runtime checks result in panic (`P-TYP-1953`)

##### Dynamic Semantics

**Runtime Behavior**

When `[[dynamic]]` is present and static verification fails:

1. **Key Operations**: Runtime synchronization primitives are inserted to ensure safe concurrent access
2. **Contract Checks**: Runtime assertion code is generated at the appropriate verification point (call site for preconditions, return site for postconditions)
3. **Refinement Validation**: Runtime predicate evaluation is inserted at type coercion points

**Static Elision**

Even within a `[[dynamic]]` context, if the compiler can prove a property statically, no runtime code is generated. The `[[dynamic]]` attribute permits but does not require runtime verification.

##### Constraints & Legality

**Negative Constraints**

1. `[[dynamic]]` MUST NOT be applied to contract clauses directly (apply to the enclosing procedure instead).
2. `[[dynamic]]` MUST NOT be applied to type alias declarations.
3. `[[dynamic]]` on a field declaration is ill-formed; apply to the containing record instead.

**Diagnostic Table**

| Code         | Severity | Condition                                                                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0020` | Error    | Key safety not statically provable outside `[[dynamic]]` context.           | Compile-time | Rejection |
| `E-CON-2801` | Error    | Contract predicate not statically provable outside `[[dynamic]]` context.   | Compile-time | Rejection |
| `E-TYP-1953` | Error    | Refinement predicate not statically provable outside `[[dynamic]]` context. | Compile-time | Rejection |
| `W-DYN-0001` | Warning  | `[[dynamic]]` present but all proofs succeed statically.                    | Compile-time | N/A       |

##### Examples

**Procedure-Level Dynamic**

```cursive
// Static verification required (default)
procedure static_divide(a: i32, b: i32) -> i32
    |= b != 0
{
    result a / b
}

// Runtime verification permitted
[[dynamic]]
procedure dynamic_divide(a: i32, b: i32) -> i32
    |= b != 0
{
    result a / b  // Runtime check inserted if b != 0 not proven
}
```

**Expression-Level Dynamic**

```cursive
procedure process_index(data: shared [i32], idx: usize) {
    // Static proof required for most operations
    let first = data[0]  // OK: constant index

    // Runtime sync permitted for this specific access
    let value = [[dynamic]] data[idx]  // Runtime sync if needed
}
```

**Type-Level Dynamic**

```cursive
[[dynamic]]
record DynamicBuffer {
    data: shared [u8],
    len: usize,
}
// All operations on DynamicBuffer instances may use runtime verification
```

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

A parameter with an inline constraint is semantically equivalent to an unrefined parameter with a procedure-level precondition. The desugaring rules are defined in §10.1.3 (Inline Parameter Constraints). The following example illustrates the equivalence:

```cursive
// Inline constraint form (type refinement)
procedure get<T>(arr: [T], idx: usize where { idx < arr~>len() }) -> T

// Procedure contract form (equivalent)
procedure get<T>(arr: [T], idx: usize) -> T
    |= idx < arr~>len()
```

The inline form is preferred when the constraint concerns a single parameter. The procedure contract clause (`|=`) is preferred for multi-parameter relationships, postconditions, or complex predicates that benefit from separate formatting.

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

1. MUST be a pure expression (§10.1.1).
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

When a value is assigned to a binding of refinement type, or passed to a parameter of refinement type, and no Verification Fact (§10.5) establishes that the predicate holds, the predicate MUST be verified.

**Static Verification by Default**

Refinement predicates require static proof by default:

1. The implementation attempts static verification of the predicate
2. If static verification succeeds, no runtime code is generated
3. If static verification fails and the expression is not within a `[[dynamic]]` context, the program is ill-formed (`E-TYP-1953`)
4. If static verification fails and the expression is within a `[[dynamic]]` context, a runtime check is generated

**Examples**

```cursive
type Positive = i32 where { self > 0 }
type Percentage = f64 where { self >= 0.0 && self <= 100.0 }

procedure example(raw: i32, pct: f64) {
    // Static proof required
    let a: Positive = 42                    // OK: trivially provable
    let b: Positive = raw                   // Error E-TYP-1953: cannot prove raw > 0
    let c: Positive = raw.abs() + 1         // OK: abs() + 1 > 0 provable

    // With [[dynamic]]: runtime check permitted
    let d: Positive = [[dynamic]] raw       // OK: runtime check

    // Control flow establishes fact
    if raw > 0 {
        let e: Positive = raw               // OK: guarded by if
    }

    // Block-level [[dynamic]]
    [[dynamic]] {
        let f: Positive = raw               // OK: runtime check
        let g: Percentage = pct             // OK: runtime check
    }
}
```

**Fact Synthesis**

Successful runtime verification synthesizes a Verification Fact per the rules defined in §10.5 (Verification Facts). This fact MAY be used by the implementation to elide subsequent checks on the same value for the same or implied predicates.

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

5. If static verification fails and the expression is not within a `[[dynamic]]` context, the program is ill-formed (`E-TYP-1953`).

**Diagnostic Table**

| Code         | Severity | Condition                                                           | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1950` | Error    | `self` used in inline parameter constraint.                         | Compile-time | Rejection |
| `E-TYP-1951` | Error    | Refinement predicate is not of type `bool`.                         | Compile-time | Rejection |
| `E-TYP-1952` | Error    | Circular type dependency in refinement predicate.                   | Compile-time | Rejection |
| `E-TYP-1953` | Error    | Refinement predicate not statically provable outside `[[dynamic]]`. | Compile-time | Rejection |
| `E-CON-2802` | Error    | Impure expression in refinement predicate.                          | Compile-time | Rejection |
| `P-TYP-1953` | Panic    | Runtime refinement validation failed.                               | Runtime      | Panic     |

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

When a module comprises multiple `.cursive` files, the order in which files are processed is IDB.

Regardless of file processing order:
1. All files in the directory contribute to a single unified namespace
2. Identifier uniqueness constraints apply across all files
3. The resulting module semantics **MUST** be independent of file processing order for well-formed programs

The IDB file processing order affects only compilation artifacts (error message ordering, intermediate file timestamps, symbol table construction order) but **MUST NOT** affect the observable semantics of the compiled program. Any program whose semantics would change based on file processing order is ill-formed.

##### Syntax & Declaration

**Top-Level Item Grammar**

Only the following declaration kinds are permitted at module scope:

```ebnf
top_level_item ::= import_decl
                 | use_decl
                 | static_decl
                 | procedure_decl
                 | type_decl
                 | form_declaration

static_decl    ::= visibility? ("let" | "var") binding
type_decl      ::= visibility? ("record" | "enum" | "modal" | "type") identifier ...
procedure_decl ::= visibility? "procedure" identifier ...
```

The `form_declaration` production is defined in §9.2 (Form Declarations). The semantics of `let` and `var` bindings in `static_decl` (mutability and movability) are defined in §3.4 (The Binding Model).

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

A **Module Path** is a sequence of identifiers separated by `::` that uniquely identifies a module within an assembly. Module paths are derived from filesystem structure per the algorithm defined in §8.1 (WF-Module-Path-Derivation).

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

A module path is well-formed if and only if every component is a valid Cursive identifier (§2.5) and is not a reserved keyword (§2.6).

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

> **Note:** This Scope Context ($\Gamma$) is used for name resolution and has a four-level hierarchical structure. For the simpler typing judgment context used in expression typing, see §11.1.

**Scope Hierarchy**

| Scope Level    | Contents                                                            |
| :------------- | :------------------------------------------------------------------ |
| $S_{local}$    | Block-local bindings (`let`, `var`, loop variables, match bindings) |
| $S_{proc}$     | Procedure parameters and local type parameters                      |
| $S_{module}$   | Module-level declarations and imported/used names                   |
| $S_{universe}$ | Primitive types and the `cursive.*` namespace                       |

**Universe Scope**

The **Universe Scope** ($S_{universe}$) is the implicit outermost scope that terminates all name resolution chains. It contains:
- All primitive types (§5.1)
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

Every top-level declaration has a **visibility level** that controls its accessibility from other modules.

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

**Summary: Import vs Use**

| Declaration | Purpose                                   | Effect                                                                                              | When Required                                              |
| :---------- | :---------------------------------------- | :-------------------------------------------------------------------------------------------------- | :--------------------------------------------------------- |
| `import`    | Establish dependency on external assembly | Makes module namespace available for qualified access; adds edge to inter-assembly dependency graph | Before any `use` referencing an external assembly          |
| `use`       | Create scope aliases                      | Introduces bindings into current scope for unqualified access                                       | Optional; for convenience when qualified access is verbose |

For intra-assembly access (modules within the same assembly), `use` declarations may reference module paths directly without a preceding `import`. For external assemblies, `import` establishes the dependency relationship and `use` then brings specific items into scope.

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
| `W-MOD-1201` | Warning  | Wildcard `use` (`*`) in a module exposing public API.            | Compile-time | N/A       |

See §8.5 for visibility violation diagnostics (`E-NAM-1305`).

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

A declaration that introduces an identifier into the current scope MUST be well-formed only if that identifier is not already bound in the same scope.

**Explicit Shadowing Rules**

A declaration in an inner scope MAY shadow a name from an outer scope, but:
- Explicit shadowing with the `shadow` keyword is well-formed when the name exists in an outer scope but not in the current scope.
- Using `shadow` when no binding is being shadowed is ill-formed.

**Conformance Mode Behavior:**
- **Strict Mode**: Implicit shadowing (without `shadow` keyword) MUST trigger error `E-NAM-1303`
- **Permissive Mode**: Implicit shadowing MUST trigger warning `W-NAM-1303`

**Unqualified Name Lookup**

Unqualified lookup searches the scope context from innermost to outermost. If the identifier is found in the innermost scope, that binding is returned. Otherwise, lookup proceeds to the next outer scope. If the context is exhausted and the name is not found, the lookup fails.

**Qualified Name Lookup**

Qualified lookup resolves a path of identifiers, such as `A::B::C`. To resolve a path `p::i`:
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
| `E-NAM-1306` | Error    | Unnecessary `shadow` keyword: no binding is being shadowed.    |

See §8.5 for visibility violation diagnostics (`E-NAM-1305`).

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

A dependency is **eager** if its resolution is required during module initialization. This includes:
- Constant evaluation (`comptime` expressions) referencing external bindings
- Module-level `let` or `var` initializers that invoke procedures or access values from other modules
- Type definitions that require evaluating constant expressions from other modules

A dependency is **lazy** if it is only required during procedure execution. Lazy dependencies include:
- Type-only references (using a type from another module without evaluating its values)
- Value-level references that occur exclusively within procedure bodies (not at module scope)

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

The `Context` record is defined in Appendix E.

##### Syntax & Declaration

**Required Signature:**

```cursive
public procedure main(ctx: Context) -> i32
```

The `main` procedure:
- **MUST** be declared with `public` visibility
- **MUST** accept exactly one parameter of type `Context`
- **MUST** return type `i32`

##### Static Semantics

**Main Procedure Well-Formedness:**

1. Exactly one `main` procedure **MUST** exist in the entry assembly
2. The signature **MUST** match `main(ctx: Context) -> i32`
3. `main` **MUST NOT** be generic (no type parameters)

**Return Semantics:**

- Return value `0` indicates successful termination
- Non-zero return values indicate failure (specific codes are application-defined)

##### Constraints & Legality

**Global State Definition**

**Global state** comprises any mutable storage with module lifetime. In Cursive, this includes:
- Module-level `var` bindings
- Any other mutable storage accessible at module scope without requiring a capability parameter

**Global Mutable State Prohibition**: Module-level `var` bindings **MUST NOT** have `public` visibility. Global mutable state accessible across module boundaries is forbidden to ensure capability-based security.

| Code         | Severity | Condition                                |
| :----------- | :------- | :--------------------------------------- |
| `E-DEC-2430` | Error    | Multiple `main` procedures defined.      |
| `E-DEC-2431` | Error    | Invalid `main` signature.                |
| `E-DEC-2432` | Error    | `main` is generic (has type parameters). |
| `E-DEC-2433` | Error    | Public mutable global state.             |

---

## Clause 9: Forms and Polymorphism

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

A form MAY extend one or more superforms using the `<:` operator. A type implementing a subform MUST also implement all of its superforms. A subform declaration (`form Sub <: A + B`) establishes a **nominal** subtyping relationship: `Sub` is a distinct form with its own identity, and implementing `Sub` requires implementing both `A` and `B`.

**Form Alias**

A **form alias** declares a new name that is equivalent to a combination of one or more form bounds. Form aliases enable concise expression of compound bounds.

> **Note on Form Alias vs Superform:** A form alias (`type Alias = A + B`) creates **structural equivalence**—`Alias` is simply another name for the compound bound `A + B`, and `T <: Alias` is identical to `T <: A + B`. In contrast, a subform declaration (`form Sub <: A + B`) creates a **nominal** relationship—`Sub` is a new form that happens to require its superforms, but `T <: Sub` is not equivalent to `T <: A + B`. A type must explicitly declare `T <: Sub` to satisfy the `Sub` bound.

##### Syntax & Declaration

**Grammar**

```ebnf
form_declaration ::=
    [ <visibility> ] "form" <identifier> [ <generic_params> ]
    [ "<:" <superform_bounds> ] "{"
        <form_item>*
    "}"

superform_bounds ::= <form_bound> ( "+" <form_bound> )*
form_bound       ::= <type_path> [ <generic_args> ]

form_item ::=
    <abstract_procedure>
  | <concrete_procedure>
  | <associated_type>

abstract_procedure ::=
    "procedure" <identifier> <signature> [ <contract_clause> ] ";"

concrete_procedure ::=
    "procedure" <identifier> <signature> [ <contract_clause> ] <block>

associated_type ::=
    "type" <identifier> [ "=" <type> ] ";"

form_alias ::=
    [ <visibility> ] "type" <identifier> [ <generic_params> ]
    "=" <form_bound> ( "+" <form_bound> )* ";"
```

The `generic_params` and `generic_args` productions are defined in §7.1 (Static Polymorphism). The `contract_clause` production is defined in §10.1 (Contract Fundamentals).

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

> **Rationale:** This restriction, enforced by the Orphan Rule below, prevents conflicting implementations when multiple assemblies implement the same form for the same type. By requiring that either the type or the form be defined locally, Cursive guarantees global coherence: every `(Type, Form)` pair has at most one implementation visible to any compilation unit.

##### Syntax & Declaration

**Grammar**

The form implementation clause is integrated into type declarations using the `<:` operator followed by a comma-separated list of form paths.

> **Cross-Reference:** The complete grammar for type declarations with form implementation is defined in:
> - §5.3 (`record_decl` with `implements_clause`)
> - §5.4 (`enum_decl` with `implements_clause`)
> - §6.1 (`modal_decl` with `implements_clause`)
>
> The `implements_clause` and `form_list` productions are defined in §5.3.

**Form List**

The `form_list` production (defined in §5.3) specifies one or more forms to implement:

```ebnf
implements_clause ::= "<:" form_list
form_list         ::= type_path ("," type_path)*
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

The `override` keyword distinguishes between two implementation scenarios:

- **Implementing an abstract procedure**: When a type provides the first implementation of a procedure that has no body in the form, the `override` keyword MUST NOT be used. This is an *initial implementation*, not an override.
- **Overriding a concrete procedure**: When a type provides its own implementation of a procedure that already has a default body in the form, the `override` keyword MUST be used. This signals that the type is intentionally replacing the inherited behavior.

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

### 9.4 Form Constraints in Generics

This section describes how forms integrate with Cursive's generic type system to provide constrained polymorphism.

> **Cross-Reference:** The complete specification of static polymorphism (generics), including syntax, grammar, typing rules, monomorphization semantics, and constraint satisfaction, is defined in **§7.1 Static Polymorphism (Generics)**. This section documents form-specific aspects of constraint usage.

##### Overview

Forms serve as **type constraints** in generic declarations. A generic parameter `T <: Tr` restricts valid type arguments to types implementing form `Tr`. This enables:

1. **Type-safe abstraction**: Generic code can call form methods on values of type `T`, knowing any valid instantiation provides those methods.
2. **Zero-cost dispatch**: All method calls are resolved at monomorphization time to direct static calls (no vtable lookup).
3. **Compile-time verification**: Constraint violations are detected during type checking, not at runtime.

**Example**

```cursive
procedure summarize<T <: Display>(items: const [T], output: witness FileSystem) {
    loop item in items {
        item~>display(output)  // Valid: T <: Display guarantees display method
    }
}
```

The constraint `T <: Display` ensures that any type instantiating `T` provides the `display` method. The call `item~>display(output)` is compiled to a direct static call to the concrete type's implementation.

##### Diagnostic Table

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-TRS-2930` | Error    | Type argument does not satisfy form constraint.   | Compile-time | Rejection |
| `E-TRS-2931` | Error    | Unconstrained type parameter used in form method. | Compile-time | Rejection |

> **Note:** Additional diagnostics for generic type parameter well-formedness, instantiation, and inference are defined in §7.1.

---

### 9.5 Dynamic Polymorphism (Witnesses)

##### Definition

**Dynamic Polymorphism**

Dynamic polymorphism is opt-in runtime dispatch using form witnesses. It enables heterogeneous collections and runtime polymorphism at the cost of one vtable lookup per call.

**Witness**

A **witness** (`witness Form`) is a concrete, sized type representing any value implementing a witness-safe form. It is implemented as a dense pointer.

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

**The `Self: Sized` Exclusion Clause**

A procedure that is not vtable-eligible MAY be excluded from witness safety requirements by adding `where Self: Sized` to its signature. This clause restricts the procedure to contexts where the concrete type is statically known, making it unavailable through witness dispatch while preserving the form's overall witness safety.

**Formal Definition of `witness_safe`**

$$
\text{witness\_safe}(Tr) \iff \forall p \in \text{procedures}(Tr).\ \text{vtable\_eligible}(p) \lor \text{has\_sized\_bound}(p)
$$

where:
- $\text{procedures}(Tr)$ includes all procedures declared in $Tr$ and its superforms
- $\text{vtable\_eligible}(p)$ holds if $p$ satisfies all four criteria above
- $\text{has\_sized\_bound}(p)$ holds if $p$ declares `where Self: Sized`

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

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-TRS-2940` | Error    | Procedure with `where Self: Sized` called on witness.   | Compile-time | Rejection |
| `E-TRS-2941` | Error    | Witness created from non-witness-safe form.             | Compile-time | Rejection |
| `E-TRS-2942` | Error    | Generic procedure in form without `Self: Sized` clause. | Compile-time | Rejection |

**Permission Interaction**

Witness types may be combined with permissions per §4.5. The `shared` permission imposes additional constraints on the form; see §13.3.1 (Witness Types and Shared Permission) for the well-formedness rule.

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

> **API Design Implication:** Because each procedure returning `opaque Tr` produces a distinct type, values from different procedures cannot be stored together in a homogeneous collection. For example, if `create_handler_a()` and `create_handler_b()` both return `opaque Handler`, their return values cannot be placed in the same `Vec<opaque Handler>`. When heterogeneous storage is required, use `witness Tr` instead, accepting the vtable dispatch overhead.

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

The language defines foundational forms (`Drop`, `Copy`, `Clone`, `Iterator`) with special compiler semantics. Their normative signatures, constraints, and complete semantics are defined in **Appendix D.1**.

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

An expression $e$ satisfies $\text{pure}(e)$ if and only if:

1. $e$ MUST NOT invoke any procedure that accepts capability parameters (`witness` bindings).
2. $e$ MUST NOT mutate state observable outside the expression's evaluation.
3. Built-in operators on primitive types and `comptime` procedures are always pure.

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

An **Inline Parameter Constraint** is a `where` clause attached directly to a parameter declaration. This syntax is sugar for a refinement type (§7.3) that is also added as a precondition.

##### Syntax & Declaration

```ebnf
parameter_with_constraint
    ::= identifier ":" type_expr "where" "{" predicate_expr "}"
```

##### Static Semantics

**Desugaring**

A parameter declaration `x: T where { P }` is semantically equivalent to:
1. Declaring `x` with refinement type `T where { P }` (per §7.3)
2. Adding `P` as a conjunct to the procedure's effective precondition

The refinement type semantics (scope rules, evaluation, validity) are defined in §7.3.

**Effective Precondition**

Given inline constraints $\{P_1, P_2, \ldots, P_n\}$ and procedure-level precondition $P_{proc}$:

$$P_{eff} = P_1 \land P_2 \land \cdots \land P_n \land P_{proc}$$

Constraints are conjoined left-to-right in parameter declaration order.

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

Precondition scope is defined by $\Gamma_{pre}$ (§10.1.2). Precondition verification follows the strategy specified by `[[verify(...)]]` (§10.4). A precondition that cannot be statically verified in `static` mode is diagnosed as `E-CON-2801`. In `dynamic` mode, a runtime check is inserted; failure causes a panic.

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
| Expression Constraint | `expr` MUST be pure (§10.1.1)                          |
| Expression Scope      | `expr` MUST reference only parameters and receiver     |
| Type Constraint       | Result type of `expr` MUST implement `Copy` or `Clone` |
| Capture Mechanism     | Implementation captures the value at procedure entry   |

**Capture Semantics**

When `@entry(expr)` appears in a postcondition:
1. The implementation evaluates `expr` immediately after parameter binding
2. The result is captured and stored according to the type's copy semantics:
   - **Copy types**: A bitwise copy is performed (zero runtime cost beyond the copy itself)
   - **Clone types**: The `clone()` procedure is invoked to produce the captured value
3. In the postcondition, `@entry(expr)` refers to this captured value

> **Performance Note:** For `Clone` types, the `clone()` invocation occurs unconditionally at procedure entry, regardless of whether the postcondition will ultimately be evaluated (e.g., in `dynamic` mode where the procedure may panic before return). Authors should prefer capturing `Copy` projections (e.g., `@entry(self.len())`) over cloning entire structures when possible.

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

> **Cross-Reference:** Refinement Types (§7.3) provide anonymous invariants attached to type references (e.g., `usize where { self < len }`). Verification of refinement type predicates uses the mechanisms defined in §10.4 and §10.5.

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

At each enforcement point:
- If the expression is within a `[[dynamic]]` context: runtime check is emitted when static proof fails
- If the expression is outside `[[dynamic]]` context: static proof is required; failure is ill-formed (`E-CON-2801`)

```cursive
type Positive = i32 where { self > 0 }

// Type declaration makes enforcement points for Positive require static proof
// unless the specific expression is marked [[dynamic]]

procedure example() {
    let a: Positive = 5                    // OK: trivially provable
    let b: Positive = compute()            // Error: cannot prove compute() > 0
    let c: Positive = [[dynamic]] compute() // OK: runtime check
}
```

**Effective Postcondition Desugaring**

For a procedure $P$ on type $T$ with:
- Type invariant $Inv$
- Explicit postcondition $Post$
- Mutable receiver (`unique`)

The **effective postcondition** is:

$$\text{EffectivePost}(P) = Post \land Inv(\texttt{self})$$

The implementation MUST verify both the explicit postcondition and the type invariant before return.

**Private Procedure Exemption**

Private procedures (visibility `internal` or less) are exempt from the Pre-Call enforcement point. This exemption enables internal helper procedures to temporarily violate the type invariant during complex state transitions.

**Invariant Restoration Requirement**

When invariants are temporarily violated within private procedures:
1. The type invariant MAY be violated at any point during private procedure execution
2. The type invariant MUST be restored before any return path that directly or indirectly reaches a public caller
3. This restoration is the **type author's responsibility**; the compiler does not track invariant states across private procedure boundaries

Failure to restore the invariant before returning to public callers results in observable invariant violations at the next public enforcement point. This constitutes a logic error in the type's implementation.

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

### 10.4 Contract Verification

##### Definition

Contract verification determines how the compiler ensures contract predicates hold. By default, **static verification is required**: the compiler must prove the contract holds, or the program is ill-formed. The `[[dynamic]]` attribute permits runtime verification as an explicit opt-in when static proof is not achievable.

##### Static Semantics

**Default: Static Verification Required**

For any contract predicate $P$ without enclosing `[[dynamic]]` context:

$$
\frac{
    \text{StaticProof}(\Gamma, P)
}{
    P : \text{verified (zero overhead)}
}
\tag{Contract-Static-OK}
$$

$$
\frac{
    \neg\text{StaticProof}(\Gamma, P) \quad
    \neg\text{InDynamicContext}
}{
    \text{program is ill-formed (E-CON-2801)}
}
\tag{Contract-Static-Fail}
$$

**With `[[dynamic]]`: Runtime Verification Permitted**

For any contract predicate $P$ within a `[[dynamic]]` context:

$$
\frac{
    \text{StaticProof}(\Gamma, P)
}{
    P : \text{verified (zero overhead)}
}
\tag{Contract-Dynamic-Elide}
$$

$$
\frac{
    \neg\text{StaticProof}(\Gamma, P) \quad
    \text{InDynamicContext}
}{
    \text{emit runtime check for } P
}
\tag{Contract-Dynamic-Check}
$$

**Proof Techniques**

The specific proof techniques supported by $\text{StaticProof}$ are Implementation-Defined Behavior. However, for portable programs, implementations SHOULD support at minimum:

| Technique                | Description                                           | Example                     |
| :----------------------- | :---------------------------------------------------- | :-------------------------- |
| Constant propagation     | Evaluate expressions involving compile-time constants | `SIZE < 100` where SIZE = 5 |
| Linear integer reasoning | Prove inequalities over bounded integer arithmetic    | `i < n && n <= 10`          |
| Boolean algebra          | Simplify and prove boolean expressions                | `a && (a \|\| b)`           |
| Control flow analysis    | Track predicates established by conditionals          | `if x > 0 { ... }`          |
| Type-derived bounds      | Use type constraints (e.g., `usize >= 0`)             | Non-negativity of unsigned  |
| Verification facts       | Use facts established by prior checks                 | §10.5                       |

**Precondition vs Postcondition Verification Location**

| Contract Type  | Verified Where    | `[[dynamic]]` Context Determined By  |
| :------------- | :---------------- | :----------------------------------- |
| Precondition   | Call site         | The call expression's context        |
| Postcondition  | Definition site   | The procedure's `[[dynamic]]` status |
| Type invariant | Enforcement point | The expression's context             |
| Loop invariant | Enforcement point | The enclosing scope's context        |

**Precondition Verification at Call Sites**

A precondition is verified at each call site. The `[[dynamic]]` status of the call site determines whether runtime checking is permitted:

```cursive
procedure require_positive(x: i32)
    |= x > 0
{ ... }

procedure caller(n: i32) {
    require_positive(5)              // OK: trivially provable
    require_positive(n)              // Error E-CON-2801: cannot prove n > 0
    [[dynamic]] require_positive(n)  // OK: runtime check on n > 0

    if n > 0 {
        require_positive(n)          // OK: guarded, provable
    }
}
```

**Postcondition Verification at Definition Site**

A postcondition is verified within the procedure body. The procedure's own `[[dynamic]]` status determines whether runtime checking is permitted:

```cursive
// Without [[dynamic]]: postcondition must be statically provable
procedure abs(x: i32) -> i32
    => @result >= 0
{
    if x >= 0 { result x }
    else { result -x }
    // Compiler must prove both paths yield @result >= 0
}

// With [[dynamic]]: runtime check permitted
[[dynamic]]
procedure complex_computation(x: f64) -> f64
    => @result.is_finite()
{
    // Complex logic where finiteness is hard to prove statically
    // Runtime check emitted for postcondition
}
```

##### Dynamic Semantics

**Runtime Check Insertion Points**

When `[[dynamic]]` permits and static proof fails:

| Contract Type  | Check Location                         |
| :------------- | :------------------------------------- |
| Precondition   | Immediately before call (at call site) |
| Postcondition  | Immediately before return (in callee)  |
| Type invariant | At each enforcement point (§10.3.1)    |
| Loop invariant | At each enforcement point (§10.3.2)    |

**Runtime Check Failure**

When a runtime-checked predicate evaluates to `false`:

1. The runtime MUST trigger a **Panic** (`P-CON-2850`)
2. The panic payload MUST include:
   - The predicate text (if available)
   - Source location of the contract
   - Whether it was a precondition or postcondition failure
3. Normal panic propagation rules apply (Clause 3)

**Fact Synthesis After Runtime Check**

Successful execution of a runtime check synthesizes a Verification Fact per §10.5:

```cursive
[[dynamic]]
procedure example(x: i32)
    |= x > 0
{
    // If we reach here, x > 0 is a Verification Fact
    // Subsequent code may rely on this fact for static proofs
    let y = x - 1
    if y > 0 {
        // Can prove y > 0 using control flow
    }
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-CON-2801` | Error    | Contract not statically provable outside `[[dynamic]]` | Compile-time | Rejection |
| `P-CON-2850` | Panic    | Runtime contract check failed                          | Runtime      | Panic     |

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

| Construct                    | Generated Fact                  | Location                |
| :--------------------------- | :------------------------------ | :---------------------- |
| `if P { ... }`               | $F(P, \_)$                      | Entry of then-branch    |
| `if !P { } else { ... }`     | $F(P, \_)$                      | Entry of else-branch    |
| `match x { Pat => ... }`     | $F(x \text{ matches } Pat, \_)$ | Entry of match arm      |
| Runtime check for $P$        | $F(P, \_)$                      | Immediately after check |
| Loop invariant $Inv$ at exit | $F(Inv, \_)$                    | Immediately after loop  |

> **Note:** The `[[verify(trusted)]]` mechanism has been removed. Verification Facts are now established only through control flow analysis or successful runtime checks.

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

When a `[[dynamic]]` context requires a runtime check and no static fact dominates the check site:

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

##### Static Semantics

**Precondition Weakening**

An implementation MAY weaken (require less than) the preconditions defined in the form. An implementation MUST NOT strengthen (require more than) the preconditions. If the form says "caller must provide X", the implementation may accept X or something weaker (easier to satisfy).

**Postcondition Strengthening**

An implementation MAY strengthen (guarantee more than) the postconditions defined in the form. An implementation MUST NOT weaken (guarantee less than) the postconditions. If the form promises "callee will provide Y", the implementation must deliver Y or something stronger (more informative).

**Verification Strategy**

Behavioral subtyping constraints are verified **statically at compile-time**. The implementation performs the following checks when a type declares form implementation (`<:`):

1. **Precondition Check**: For each procedure in the form, verify that the implementation's precondition logically implies the form's precondition (i.e., the implementation accepts at least what the form requires).

2. **Postcondition Check**: For each procedure in the form, verify that the form's postcondition logically implies the implementation's postcondition (i.e., the implementation guarantees at least what the form promises).

These checks use the same proof infrastructure as `[[verify(static)]]` (§10.4.1). If the implication cannot be proven, the program is ill-formed. No runtime checks are generated for behavioral subtyping; violations are structural errors in the type definition.

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

## Clause 11: Expressions & Statements

This clause defines the syntax and semantics of expressions and statements in Cursive. Expressions are syntactic forms that produce typed values; statements are syntactic forms executed for their side effects. This clause establishes evaluation rules, pattern matching, operators, control flow, and the binding of values to identifiers.

> **Diagnostic Code Allocation:** This clause uses diagnostic codes with prefixes `E-EXP-`, `E-STM-`, `E-PAT-`, and `P-EXP-` within the range 2500–2799. See Appendix B for detailed allocations.

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

Expression type judgments use the **typing context** $\Gamma$ as defined in §4.1. The judgment $\Gamma \vdash e : T$ asserts that expression $e$ has type $T$ under context $\Gamma$.

> **Note:** For the more detailed scope context used in name resolution, see §8.4.

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

- Short-circuit operators (`&&`, `||`) conditionally skip evaluation of their right operand as specified in §11.4.6 (Binary Operators, Short-Circuit Evaluation).

**Sequential Statement Execution**

Statements within a block MUST be executed sequentially in source order. The effects of a statement MUST be fully complete before the next statement begins execution.

**Statement Termination**

Statement termination rules, including implicit newline termination and continuation conditions, are defined in §2.11.

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

**Precedence Table (Informative)**

The following table summarizes operator precedence and associativity. The normative definition of precedence is encoded in the expression grammar hierarchy (§11.4).

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

**Lexical Tokenization**

The normative rules for lexical tokenization of operators (Maximal Munch Rule, Generic Argument Exception) are defined in §2.7 (Operators and Punctuators).

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

A **literal** (integer, float, string, character, or boolean) is a primary expression whose type is determined by Clause 2, §2.8 (lexical form) and Clause 5 (type inference and suffixes). String literals are typed as `string@View` per the authoritative definition in §6.2 (String Types). Literals are value expressions.

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
| Tuple: `(e₁, e₂, ..., eₙ)`     | §5.2.1                |
| Array: `[e₁, e₂, ..., eₙ]`     | §5.2.2                |
| Record: `Type { f₁: e₁, ... }` | §5.3                  |
| Enum: `Enum::Variant(e₁, ...)` | §5.4                  |

This clause does not redefine constructor expression syntax or semantics; see the authoritative sections.

**Range Expressions**

Range expressions (e.g., `start..end`, `start..=end`, `start..`, `..end`, `..=end`, `..`) produce compiler-intrinsic range types. The syntax, typing rules, and semantics of range expressions are authoritatively defined in §5.2.4 (Range Types).

> **Cross-Reference:** See §5.2.4 for the complete grammar of range expressions and their associated types (`Range<T>`, `RangeInclusive<T>`, `RangeFrom<T>`, `RangeTo<T>`, `RangeToInclusive<T>`, `RangeFull`).

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

**Value Category**

Value category propagates per §11.1: field/tuple access on a place expression yields a place expression; on a value expression, a value expression.

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

> **Cross-Reference:** Range indexing (slicing) for arrays and slices is defined in §5.2.3 (T-Slice-From-Array). For string types, indexing and slicing have distinct semantics: direct byte indexing is prohibited (E-TYP-2152), and range slicing produces `string@View` with UTF-8 boundary validation—see §6.2 for the authoritative rules (T-String-Slice).

**Value Category**

Value category propagates per §11.1: indexing a place expression yields a place expression (enabling element assignment); indexing a value expression yields a value expression.

##### Dynamic Semantics

**Bounds Checking**

All indexing operations MUST be bounds-checked at runtime. If $\text{index} \ge \text{length}$, the executing thread MUST panic with diagnostic `P-EXP-2530`.

**Optimization Note**

Implementations MAY elide bounds checks when static analysis can prove the index is within bounds. See §10.5 (Verification Facts) for the mechanism by which bounds proofs are established and propagated; common sources include contract verification, loop invariants, and conditional guards that dominate the access site. Such elision MUST NOT alter observable behavior for well-formed programs.

**Relationship to `[[dynamic]]`**

Bounds checking is a **safety mechanism** and is not affected by the `[[dynamic]]` attribute. The `[[dynamic]]` attribute controls:
- Key synchronization for `shared` data
- Contract verification
- Refinement type validation

Bounds checks are always performed (with static elision when provable) regardless of `[[dynamic]]` status. This is because an out-of-bounds access is a program bug that should be caught, not a performance tradeoff.

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

The `&` operator takes the address of a place expression, producing a safe pointer in the `@Valid` modal state. The authoritative typing rule **(T-Addr-Of)** is defined in §6.3 (Pointer Types).

> **Cross-Reference:** See §6.3 for the normative typing judgment, place expression requirements, and the complete modal pointer state model.

The permission of the resulting pointer is determined by the permission of the source place (§4.5).

**Dereference (`*`)**

The `*` operator dereferences a pointer to access the value it points to. The authoritative typing rule **(T-Deref)** for safe pointers is defined in §6.3 (Pointer Types).

> **Cross-Reference:** See §6.3 for the normative typing judgment requiring `@Valid` state for safe pointer dereference, and **(T-Raw-Deref)** for raw pointer dereference rules.

Dereferencing a raw pointer (`*imm T` or `*mut T`) is permitted only within an `unsafe` block (§3.8).

**Region Allocation (`^`)**

The `^` operator allocates the operand value into an active region. The authoritative typing rules **(T-Alloc-Named)** and **(T-Alloc-Implicit)** are defined in §3.7 (Regions).

> **Cross-Reference:** See §3.7 for the normative typing judgments distinguishing named region allocation (`r^e`) from implicit allocation (`^e`), provenance assignment, and escape analysis rules.

**Move (`move`)**

The `move` operator transfers responsibility for a value from its source binding. The typing rule **(T-Move)** and full semantics are authoritatively defined in §3.5 (Responsibility and Move Semantics).

> **Cross-Reference:** See §3.5 for the normative typing judgment, post-move state tracking, and parameter responsibility rules.

**Modal Widening (`widen`)**

The `widen` operator converts a state-specific modal type `M@S` to the general modal type `M`. The authoritative typing rule **(T-Modal-Widen)** is defined in §6.1 (Modal Types).

> **Cross-Reference:** See §6.1 for the normative typing judgment, storage cost implications, and conditions under which explicit `widen` is required versus implicit coercion.

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

> **Note:** Explicit casts are distinct from implicit coercions (§4.2). While coercion applies automatically for subtype relationships, explicit `as` casts are required for conversions that change representation. In particular, §5.1 prohibits implicit numeric conversions, requiring explicit casts for all numeric type changes.

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
| **Safe → Raw Ptr**    | `Ptr<T>@Valid`      | `*imm T` / `*mut T` | Extract address‡‡           |
| **Enum → Integer**    | `enum E`            | Integer type        | Discriminant value§         |
| **Bool → Integer**    | `bool`              | Any integer         | `false`→0, `true`→1         |
| **Char ↔ Integer**    | `char`              | `u32`               | Unicode scalar value        |
|                       | `u32`               | `char`              | Validated conversion††      |
|                       | `char`              | `u8`                | Validated conversion††      |
|                       | `u8`                | `char`              | Always valid (ASCII subset) |

† Float-to-integer casts that overflow or produce NaN trigger a panic.

‡ Pointer-integer casts are permitted only within `unsafe` blocks. See §6.3 for the authoritative typing rules (T-Raw-Ptr-Cast-Imm, T-Raw-Ptr-Cast-Mut).

§ Enum-to-integer casts are valid only when the enum has an explicit `[[layout(IntType)]]` attribute (see §7.2.1).

†† Panics if the value is not a valid Unicode scalar value or exceeds the target range.

‡‡ See §6.3 for the authoritative typing rules (T-Raw-Ptr-Cast-Imm, T-Raw-Ptr-Cast-Mut).

Any cast not listed in the Cast Categories table above is ill-formed.

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

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2571` | Error    | Cast between incompatible types.                   | Compile-time | Rejection |
| `E-EXP-2572` | Error    | Pointer-integer cast outside `unsafe` block.       | Compile-time | Rejection |
| `E-EXP-2573` | Error    | Enum cast without `[[layout(IntType)]]` attribute. | Compile-time | Rejection |
| `E-EXP-2574` | Error    | Cast of non-`@Valid` pointer to raw pointer.       | Compile-time | Rejection |
| `P-EXP-2580` | Panic    | Float-to-integer cast overflow or NaN.             | Runtime      | Panic     |
| `P-EXP-2581` | Panic    | `u32 as char` with invalid Unicode scalar value.   | Runtime      | Panic     |
| `P-EXP-2582` | Panic    | `char as u8` with value > 255.                     | Runtime      | Panic     |

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

Closure expressions have function types (§6.4). Non-capturing closures have sparse function pointer types; capturing closures have closure types.

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

Per §6.4 (T-Sparse-Sub-Closure), sparse function pointers are subtypes of the corresponding closure types. A non-capturing closure may be used where a capturing closure is expected.

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

| Block Form              | Authoritative Section         |
| :---------------------- | :---------------------------- |
| `region { ... }`        | §3.7 Regions                  |
| `unsafe { ... }`        | §3.8 Unsafe Memory Operations |
| `parallel(...) { ... }` | §13.3 Structured Concurrency  |
| `comptime { ... }`      | §14.1 Compile-Time Execution  |

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
```

The `binding_op` production is defined in §3.4.

##### Static Semantics

**Binding Semantics**

The semantics of binding keywords (`let`/`var`) and binding operators (`=`/`:=`) are defined in §3.4 (The Binding Model). The key distinctions are:

- `let` vs. `var`: Determines mutability (whether the binding can be reassigned)
- `=` vs. `:=`: Determines movability (whether responsibility can be transferred via `move`)

These properties are orthogonal to each other and to permission types.

**Type Inference**

If the type annotation is omitted, the type is inferred from the initializer expression. If the type cannot be inferred, the program is ill-formed.

**Pattern Requirements**

The pattern in a declaration statement MUST be irrefutable (§11.2). Refutable patterns are diagnosed as errors.

##### Dynamic Semantics

1. The initializer expression is evaluated.
2. The value is bound to the pattern, introducing bindings into scope.
3. For destructuring patterns, components are bound to their respective identifiers.

##### Constraints & Legality

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-DEC-2401` | Error    | Reassignment of immutable `let` binding.         | Compile-time | Rejection |
| `E-DEC-2402` | Error    | Type annotation incompatible with inferred type. | Compile-time | Rejection |

See §11.2 for refutable pattern diagnostics (`E-PAT-2711`).

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

Reassignment of a responsible binding triggers deterministic destruction of the prior value as specified in §3.4 (The Binding Model) and §3.6 (Deterministic Destruction).

> **Cross-Reference:** See §3.4 for the normative statement "Each reassignment drops the previous value" and §3.6 for the destruction sequence and LIFO ordering guarantees.

The execution order is:
1. The new value expression is evaluated.
2. The old value's destructor is invoked per §3.6.
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

##### Static Semantics

**Authority Derivation Rule**

A procedure $p$ may perform effect $e$ iff $p$ receives a capability $c$ where $e \in \text{Authority}(c)$:

$$\frac{\Gamma \vdash c : \text{witness } T \quad e \in \text{Authority}(T)}{\Gamma \vdash p(c) \text{ may perform } e}$$

##### Constraints & Legality

| Code         | Severity | Condition                                                           | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------ | :----------- | :-------- |
| `E-CAP-1001` | Error    | Ambient authority detected: global procedure performs side effects. | Compile-time | Rejection |

---

### 12.2 The Root of Authority

##### Definition

**The Capability Root**

All system-level capabilities originate from the Cursive runtime and are injected into the program via the `Context` parameter at the entry point.

**Cross-Reference**: Entry point signature (§8.9); `Context` record (Appendix E).

**Runtime Injection**

The Cursive runtime constructs the `Context` record before program execution begins. This record contains concrete implementations of all system capability forms, initialized with full authority over system resources.

##### Static Semantics

**Runtime Injection Guarantee**

The runtime **MUST** guarantee that `ctx` is a valid, initialized `Context` record containing root capabilities before `main` begins execution.

##### Constraints & Legality

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-DEC-2431` | Error    | `main` signature incorrect (see §8.9 for spec). | Compile-time | Rejection |

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

##### Syntax & Declaration

**Capability Parameter Syntax**

Capability parameters **MUST** be declared using `witness` types to enable polymorphism:

```cursive
procedure read_config(fs: witness FileSystem, path: string@View): string {
    let file = fs~>open(path, Mode::Read)
    file~>read_all()
}
```

##### Static Semantics

**Witness Parameter Rule**

A parameter of type `witness Form` accepts any concrete type `T` where `T` implements `Form`.

**Permission Requirements for Capability Methods**

Capability methods **MUST** declare appropriate receiver permissions per the semantics defined in §4.5:

| Operation Type                 | Required Permission |
| :----------------------------- | :------------------ |
| Side-effecting I/O             | `shared` (`~%`)     |
| Pure queries (no state change) | `const` (`~`)       |

##### Constraints & Legality

**Implicit Capability Prohibition**

A procedure **MUST NOT** access capabilities not explicitly provided as parameters or derived from provided parameters.

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-CAP-1002` | Error    | Effect-producing procedure lacks required capability param. | Compile-time | Rejection |

---

### 12.5 System Capability Forms

##### Definition

The language defines the following system capability forms: `FileSystem`, `Network`, `HeapAllocator`, and the `System` record. Their normative signatures and complete semantics are provided in **Appendix D.2**.

##### Constraints & Legality

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CAP-1003` | Error    | Side-effecting capability method has `const` receiver | Compile-time | Rejection |

---

# Part 4: Concurrency

This clause defines the Cursive concurrency model, which provides safe concurrent and parallel programming through a key-based synchronization system, structured concurrency primitives, and zero-cost abstractions for shared mutable state.

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

The definitions of **Program Point** and **Lexical Scope** are provided in **§1.6 (Foundational Semantic Concepts)**.

**Key State Context**

The compiler MUST track key state at each program point using a key state context:

$$\Gamma_{\text{keys}} : \text{ProgramPoint} \to \mathcal{P}(\text{Key})$$

This mapping associates each program point with the set of keys logically held at that point.

**Held Predicate**

A key is **held** at a program point if it is a member of the key state context at that point:

$$\text{Held}(P, M, S, \Gamma_{\text{keys}}, p) \iff (P, M, S) \in \Gamma_{\text{keys}}(p)$$

When the program point is clear from context, we write $\text{Held}(P, M, \Gamma_{\text{keys}})$ or simply $\text{Held}(P, M)$.

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

When runtime synchronization is necessary, key metadata representation is IDB. The following aspects are implementation-defined:

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

Implementations MUST perform constant folding and canonicalization of linear integer expressions. Two expressions that normalize to the same canonical form are **provably equivalent**.

Implementations MAY apply additional algebraic simplifications to prove index equivalence. If normalization exceeds implementation limits (§1.4), expressions MUST be treated as **not provably equivalent**. In such cases, runtime synchronization MUST be emitted or `E-KEY-0010` MUST be diagnosed.

**Provable Disjointness for Indices:**

Two index expressions $e_1$ and $e_2$ are **provably disjoint** ($\text{ProvablyDisjoint}(e_1, e_2)$) if any of the following conditions hold:

1. **Static Disjointness:** Both are statically resolvable with different values:
   $$\text{StaticResolvable}(e_1) \land \text{StaticResolvable}(e_2) \land \text{StaticValue}(e_1) \neq \text{StaticValue}(e_2)$$

2. **Control Flow Facts:** A Verification Fact (§10.5) establishes $e_1 \neq e_2$ via conditional guard (e.g., `if i != j { ... }`).

3. **Contract-Based:** A procedure precondition or inline parameter constraint asserts $e_1 \neq e_2$.

4. **Refinement Types:** An index has a refinement type (§7.3) constraining its relationship to another index.

5. **Algebraic Offset:** Index expressions share a common base but differ by statically-known constant offsets ($e_1 = v + c_1$, $e_2 = v + c_2$ where $c_1 \neq c_2$).

6. **Dispatch Iteration:** Within a `dispatch` block (§14.5), accesses indexed by the iteration variable are automatically disjoint across iterations.

7. **Disjoint Loop Ranges:** Index expressions reference iteration variables from loops with non-overlapping ranges.

An index expression is **statically resolvable** if it is: an integer literal, a reference to a `const` binding, a generic const parameter, or an arithmetic expression where all operands are statically resolvable.

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

> **Note (Informative):** Implementations MAY satisfy the progress guarantee via FIFO ordering of waiters, priority inheritance, bounded retry with timeout, or other mechanisms. The specific mechanism is IDB.

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

Mode Sufficiency ($\text{ModeSufficient}$) is defined above in §13.2 Formal Operation Definitions.

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

**Callee `[[dynamic]]` Independence**

The `[[dynamic]]` status of a callee is independent of the caller's status:

```cursive
[[dynamic]]
procedure dynamic_caller(data: shared [Record], idx: usize) {
    static_callee(data)      // Callee enforces its own static requirements
    dynamic_callee(data)     // Callee permits its own runtime sync
}

procedure static_callee(data: shared [Record]) {
    data[0].process()        // Must be statically safe (not [[dynamic]])
}

[[dynamic]]
procedure dynamic_callee(data: shared [Record]) {
    data[0].process()        // May use runtime sync
}
```

**Interprocedural Analysis**

The compiler MAY perform interprocedural analysis to prove static safety across procedure boundaries, subject to the following constraints:

**Soundness Requirement:** If interprocedural analysis concludes that no runtime synchronization is needed for a callee's accesses, then concurrent execution of that callee MUST be safe. The analysis MUST be sound—false negatives (missing synchronization) are forbidden.

**Conservatism Requirement:** If interprocedural analysis cannot prove safety and the callee is not `[[dynamic]]`, the program is ill-formed.

**Separate Compilation:** For procedures in separate compilation units whose source is not available:
1. The implementation MUST assume the callee may access **any** path reachable from its `shared` parameters
2. The implementation MUST assume the callee requires **Write** access unless the parameter is declared `const`
3. If safety cannot be proven and the call site is not in a `[[dynamic]]` context, the program is ill-formed

**Inlining Interaction:** If a procedure is inlined, the inlined body MUST be analyzed as if its statements were written at the call site, with full visibility of the caller's key context and the caller's `[[dynamic]]` status.

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

A `shared witness Form` type is the composition of the `shared` permission (§4.5) applied to a witness type (§9.5). This combination permits polymorphic read access to shared data.

Because dynamic dispatch erases the concrete type, only forms with exclusively read-only methods (`~` receiver) are compatible with `shared` permission. This constraint follows from the receiver compatibility rules defined in §4.5.4–§4.5.5.

> **Note:** This is not a distinct syntactic construct. The type `shared witness Form` is parsed as the permission modifier `shared` applied to the witness type `witness Form`, following the standard permission type composition rules in §4.5.

##### Static Semantics

The following well-formedness rule constrains which forms can appear in `shared witness` types. This rule derives from §4.5's receiver compatibility matrix and §9.5's witness safety requirements:

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

See §13.7.3 for complete semantics.

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

**Increment and Decrement Operators**

The increment (`++`) and decrement (`--`) operators are defined in §11.4.5 (Unary Operators). When applied to `shared` paths, they implicitly acquire keys using the compound assignment pattern above.

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

This section specifies the application of path disjointness rules (§13.1.1) to array accesses within the **same statement**.

When multiple dynamic indices access the same array within a single statement, the compiler MUST either prove they are disjoint (per §13.1.1) or reject the program with `E-KEY-0010`. Same-statement accesses cannot be serialized by runtime key acquisition—they occur as part of one atomic key-holding scope.

> **Cross-Reference:** The authoritative definitions for static resolvability, index expression equivalence, and provable disjointness are in §13.1.1 (Path Prefix and Disjointness). This section applies those definitions to same-statement conflict detection.

##### Static Semantics

**Same-Statement Conflict Detection**

For two index expressions $e_1$ and $e_2$ accessing the same array within a single statement, the compiler classifies them using the provable disjointness rules from §13.1.1:

| Classification          | Condition (per §13.1.1)                                  | Compiler Action              |
| :---------------------- | :------------------------------------------------------- | :--------------------------- |
| Statically Disjoint     | $\text{ProvablyDisjoint}(e_1, e_2)$ via static values    | Fine-grained keys permitted  |
| Provably Disjoint       | $\text{ProvablyDisjoint}(e_1, e_2)$ via any §13.1.1 rule | Fine-grained keys permitted  |
| Potentially Overlapping | $\neg\text{ProvablyDisjoint}(e_1, e_2)$                  | **Rejection** (`E-KEY-0010`) |

---

#### 13.6.1 Dynamic Index Conflict Detection

##### Definition

When multiple dynamic indices access the same array within a single statement, the compiler MUST either prove they are disjoint (per §13.1.1) or reject the program with `E-KEY-0010`. Same-statement accesses cannot be serialized by runtime key acquisition—they occur as part of one atomic key-holding scope.

> **Note:** The `[[dynamic]]` attribute does NOT permit same-statement conflicts. These remain compile-time errors because runtime synchronization cannot serialize accesses within a single atomic statement.

##### Static Semantics

**Same-Statement Conflict Detection**

For two index expressions $e_1$ and $e_2$ accessing the same array within a single statement, the compiler classifies them using the provable disjointness rules from §13.1.1:

| Classification          | Condition (per §13.1.1)                                  | Compiler Action              |
| :---------------------- | :------------------------------------------------------- | :--------------------------- |
| Statically Disjoint     | $\text{ProvablyDisjoint}(e_1, e_2)$ via static values    | Fine-grained keys permitted  |
| Provably Disjoint       | $\text{ProvablyDisjoint}(e_1, e_2)$ via any §13.1.1 rule | Fine-grained keys permitted  |
| Potentially Overlapping | $\neg\text{ProvablyDisjoint}(e_1, e_2)$                  | **Rejection** (`E-KEY-0010`) |

This table applies regardless of `[[dynamic]]` context. Same-statement conflicts are always ill-formed.

**(K-Dynamic-Index-Conflict)**
$$\frac{
    P_1 = a[e_1] \quad P_2 = a[e_2] \quad
    \text{SameStatement}(P_1, P_2) \quad
    (\text{Dynamic}(e_1) \lor \text{Dynamic}(e_2)) \quad
    \neg\text{ProvablyDisjoint}(e_1, e_2)
}{
    \text{Emit}(\texttt{E-KEY-0010})
}$$

**Cross-Statement Dynamic Conflicts**

For dynamic index accesses in *different* statements, `[[dynamic]]` permits runtime serialization:

```cursive
[[dynamic]]
procedure swap(arr: shared [i32], i: usize, j: usize) {
    let temp = arr[i]    // Statement 1: runtime key on arr[i]
    arr[i] = arr[j]      // Statement 2: runtime keys, serialized
    arr[j] = temp        // Statement 3: runtime key on arr[j]
    // OK: different statements, runtime serialization works
}

procedure same_statement_conflict(arr: shared [i32], i: usize, j: usize) {
    arr[i] = arr[j]      // Error E-KEY-0010: same statement, cannot serialize
    // [[dynamic]] does not help here
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0010` | Error    | Potential conflict on dynamic indices (same statement) | Compile-time | Rejection |

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

**USB Ordering Hazard**

When multiple tasks execute `#` blocks with incomparable dynamic indices on overlapping paths, USB ordering MAY create circular wait conditions across tasks.

The implementation MUST emit diagnostic `W-KEY-0011` when `#` blocks contain dynamically-indexed paths to the same array where indices cannot be statically compared.

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

The constant $\text{MAX\_SPECULATIVE\_RETRIES}$ is IDB. A typical value is 3–10 retries.

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

Keys MUST NOT be held across async suspension points. This is enforced at compile time.

> **Cross-Reference:** Async suspension points are defined by the `yield` and `yield from` expressions in §15.4.2 and §15.4.3. The normative prohibition rule is specified in §15.11.2 (Permission and Capture Rules).

##### Static Semantics

The diagnostic `E-ASYNC-0013` is emitted when `yield` or `yield from` occurs while keys are logically held.

**Scope Interaction**

The prohibition applies to keys held at the suspension program point, not to keys in enclosing scopes that have already been released:

```cursive
// VALID: Key released before yield
let health = #player { player.health }  // Key acquired and released
yield health                             // No keys held at yield point

// INVALID: Key held at yield point
#player {
    yield player.health    // ERROR E-ASYNC-0013
}
```

> **Rationale:** Keys represent synchronization primitives. Holding a key across suspension could cause deadlock or priority inversion when other tasks contend for the same key.

##### Constraints & Legality

> **Note:** The diagnostic for holding keys at async suspension points is `E-ASYNC-0013`, defined in §15.13.

---

### 13.9 Compile-Time Verification and Runtime Realization

##### Definition

Keys are a **compile-time abstraction**. The compiler performs static key analysis to verify safe access patterns. Runtime synchronization is emitted **only when** the `[[dynamic]]` attribute is present and static verification fails.

**The Verification Hierarchy**

| Level                       | Mechanism                 | Runtime Cost | Requirement                |
| :-------------------------- | :------------------------ | :----------- | :------------------------- |
| **Static Proof**            | Compiler proves safety    | Zero         | **Default (mandatory)**    |
| **Runtime Synchronization** | Static proof insufficient | Non-zero     | **Requires `[[dynamic]]`** |

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

**Default: Static Proof Required**

When static safety cannot be proven and the access is not within a `[[dynamic]]` context, the program is ill-formed:

**(K-Static-Required)**
$$\frac{
    \neg\text{StaticallySafe}(P) \quad
    \neg\text{InDynamicContext}
}{
    \text{Emit}(\texttt{E-KEY-0020})
}$$

**With `[[dynamic]]`: Runtime Synchronization Permitted**

When static safety cannot be proven but the access is within a `[[dynamic]]` context, runtime synchronization is emitted:

**(K-Dynamic-Permitted)**
$$\frac{
    \neg\text{StaticallySafe}(P) \quad
    \text{InDynamicContext}
}{
    \text{EmitRuntimeSync}(P)
}$$

**When `[[dynamic]]` Is Needed**

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

The specific strategy is IDB.

**Observational Equivalence**

Program behavior MUST be identical whether safety is proven statically or enforced at runtime. The only permitted difference is performance:

$$\forall P : \text{Observable}(\text{StaticSafe}(P)) = \text{Observable}(\text{RuntimeSync}(P))$$

##### Memory & Layout

**Static Case — No Overhead**

When all accesses to a `shared` value are statically safe:
- No synchronization metadata is required
- Layout MAY be identical to `unique` equivalent
- No runtime code is generated for key operations

**Runtime Case — With `[[dynamic]]`**

When runtime synchronization is permitted via `[[dynamic]]`:

| Aspect              | Specification                             |
| :------------------ | :---------------------------------------- |
| Metadata location   | IDB: inline or external                   |
| Metadata size       | IDB: MUST be documented                   |
| Lock granularity    | IDB: per-value, per-field, or per-element |
| Lock implementation | IDB: spinlock, mutex, RwLock, etc.        |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                        | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0020` | Error    | Key safety not statically provable outside `[[dynamic]]`         | Compile-time | Rejection |
| `W-KEY-0021` | Warning  | `[[dynamic]]` present but all key operations are statically safe | Compile-time | Advisory  |

**Examples**

```cursive
procedure static_only(player: shared Player) {
    player.health = 100        // OK: sequential context
    player.mana = 50           // OK: disjoint path
}

procedure needs_dynamic(players: shared [Player], idx: usize) {
    players[0].health = 100               // OK: static index
    players[idx].health = 50              // Error E-KEY-0020
    [[dynamic]] players[idx].health = 50  // OK: runtime sync
}

[[dynamic]]
procedure all_dynamic(players: shared [Player], idx: usize) {
    players[idx].health = 100  // OK: runtime sync
    players[idx].mana = 50     // OK: runtime sync
}
```

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
3. Form methods with `~!` receivers are not callable through `shared` paths (per §4.5.5).

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

### 13.10 Memory Ordering

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

## Clause 14: Structured Parallelism

This clause defines Cursive's structured parallelism model. Parallelism enables concurrent execution of code across multiple workers within a bounded scope. The model guarantees that all spawned work completes before the parallel scope exits, ensuring deterministic resource cleanup and composable concurrency.

---

### 14.1 Parallelism Overview

##### Definition

**Structured parallelism** is a concurrency model in which all concurrent work spawned within a scope completes before that scope exits. This guarantee enables safe capture of stack-local bindings, deterministic destruction ordering, and composable parallel regions.

**Formal Definition**

Let $\mathcal{W}$ denote the set of work items spawned within a parallel block $P$. The **structured concurrency invariant** states:

$$\forall w \in \mathcal{W}.\ \text{lifetime}(w) \subseteq \text{lifetime}(P)$$

A **work item** is a unit of computation queued for execution by a worker. Work items are created by `spawn` and `dispatch` expressions. A **worker** is an execution context (typically an OS thread or GPU compute unit) that executes work items.

**Task** is the runtime representation of a work item during execution. A task may suspend (at `wait` expressions) and resume. Tasks are associated with key state per §13.1.

---

### 14.2 The Parallel Block

##### Definition

A **parallel block** establishes a scope within which work may execute concurrently across multiple workers. The block specifies an execution domain and contains `spawn` and `dispatch` expressions that create work items. Per the structured concurrency invariant (§14.1), all work items complete before execution proceeds past the block's closing brace.

**Formal Definition**

A parallel block $P$ is defined as a tuple:

$$P = (D, A, B)$$

where:
- $D$ is the execution domain expression
- $A$ is the (possibly empty) set of block options
- $B$ is the block body containing statements and work-creating expressions

##### Syntax & Declaration

**Grammar**

```ebnf
parallel_block    ::= "parallel" domain_expr block_options? block

domain_expr       ::= expression

block_options     ::= "[" block_option ("," block_option)* "]"

block_option      ::= "cancel" ":" expression
                    | "name" ":" string_literal

block             ::= "{" statement* "}"
```

The `domain_expr` MUST evaluate to a type that implements the `ExecutionDomain` form (§14.6).

> **Note:** Block options use single-bracket syntax `[...]` and are distinct from attributes (which use double-bracket syntax `[[...]]`). Block options are specific to parallel blocks and are not part of the general Attribute Registry (§7.2).

**Block Option Semantics**

| Option   | Type          | Purpose                                      |
| :------- | :------------ | :------------------------------------------- |
| `cancel` | `CancelToken` | Provides cooperative cancellation capability |
| `name`   | `string`      | Labels the block for debugging and profiling |

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

Evaluation of `parallel D [opts] { B }` proceeds as follows:

1. Let $d$ be the result of evaluating domain expression $D$.
2. Initialize the worker pool according to $d$'s configuration.
3. If the `cancel` option is present, associate its token with the block.
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

When a `spawn` or `dispatch` body references bindings from enclosing scopes, those bindings are **captured** into the work item. The capture rules for `spawn` and `dispatch` bodies follow the same semantics as closure capture, defined in **§11.5 (Closure Expressions)**.

##### Static Semantics

**Lifetime Safety**

Captured references are guaranteed valid because structured concurrency (§14.1) ensures all work completes before the parallel block exits. This distinguishes parallel capture from closure capture: closures may escape their defining scope, requiring move semantics for all non-`Copy` captures. Parallel work items enable safe reference capture for `const` and `shared` bindings.

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

The `SpawnHandle<T>` type supports the `wait` expression (§15.3) for result retrieval.

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

Per §13.6, this enables full parallelization when iterations access disjoint array elements.

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

> **Cross-Reference:** The specific domain implementations (`ctx.cpu`, `ctx.gpu`, `ctx.inline`) and their APIs are library interfaces defined in **Appendix E**.

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

Execution domains are accessed through factory fields on the `Context` record (see **Appendix E** for the normative `Context` definition). The `Context` record includes `cpu`, `gpu`, and `inline` domain factories.

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

Per §13.8, keys MUST NOT be held across `wait` suspension points.

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

When a cancel token is attached to a parallel block via the `cancel` option, the token is implicitly available within all `spawn` and `dispatch` bodies.

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

## Clause 15: Asynchronous Operations

This clause defines Cursive's model for asynchronous computation. Asynchronous operations are computations that may suspend execution and resume later, producing values incrementally, completing after external events, or interleaving execution with other computations.

---

### 15.1 Overview and Design Principles

##### Definition

An **asynchronous operation** is a computation with addressable resumption points. Unlike a standard procedure that runs to completion in a single invocation, an asynchronous operation may pause and resume multiple times, exchanging values with its caller at each resumption point.

**Formal Definition**

An asynchronous operation $A$ is characterized by the tuple:

$$A = (\mathcal{S}, s_0, \delta, \omega)$$

where:
- $\mathcal{S}$ is a finite set of internal states (captured local bindings)
- $s_0 \in \mathcal{S}$ is the initial state
- $\delta : \mathcal{S} \times In \to \mathcal{S} \times (Out \mid Result \mid E)$ is the transition function
- $\omega \subseteq \mathcal{S}$ is the set of terminal states

Each invocation of `resume` applies $\delta$, transitioning the operation to a new state and producing either an intermediate output, a final result, or an error.

##### Static Semantics

**Design Principles**

The asynchronous model adheres to the following principles:

**Unified Abstraction**: All asynchronous patterns—sequences, futures, coroutines, streams—are instances of a single parameterized modal type. There are no separate mechanisms for different asynchronous patterns.

**Explicit Suspension**: Every point at which execution may pause is syntactically visible via the `yield` expression. There is no implicit suspension.

**No Function Coloring**: Procedures are not marked with special modifiers in their declaration. The return type indicates asynchronous behavior. A procedure returning `Async<...>` may be called from any context; the caller decides how to consume the result.

**Pull-Based Execution**: Asynchronous operations advance only when explicitly resumed by their consumer. This provides natural backpressure and resource control.

**Capability Integration**: External events (I/O, timers) require capabilities. Pure computation sequences require no capabilities.

> **Note:** The `yield` keyword MUST be added to the reserved keywords table in §2.6.

---

### 15.2 The Async Modal Type

##### Definition

`Async<Out, In, Result, E>` is a modal type representing an asynchronous computation. The type parameters specify the data flow between the computation and its consumer.

| Parameter | Meaning                                          | Default  |
| :-------- | :----------------------------------------------- | :------- |
| `Out`     | Type of values produced at each suspension point | Required |
| `In`      | Type of values received when resumed             | `()`     |
| `Result`  | Type of the final completion value               | `()`     |
| `E`       | Type of the error on failure                     | `!`      |

When `E` is the never type (`!`), the async operation cannot fail.

##### Syntax & Declaration

**Grammar**

```ebnf
async_type ::= "Async" "<" type_arg ("," type_arg)* ">"
```

**Modal Declaration**

```cursive
modal Async<Out, In = (), Result = (), E = !> {
    @Suspended {
        output: Out,
    } {
        /// Advance the computation by providing input.
        /// Returns the new state after one step.
        transition resume(~!, input: In) -> @Suspended | @Completed | @Failed
    }
    
    @Completed {
        value: Result,
    }
    
    @Failed {
        error: E,
    }
}
```

##### Static Semantics

**State Semantics**

| State        | Meaning                                          | Payload         |
| :----------- | :----------------------------------------------- | :-------------- |
| `@Suspended` | Computation paused, intermediate value available | `output: Out`   |
| `@Completed` | Computation finished successfully                | `value: Result` |
| `@Failed`    | Computation terminated with error                | `error: E`      |

**State Transition Diagram**

```
              resume(input)           resume(input)
    ┌────────────────────┐   ┌────────────────────┐
    │                    ▼   │                    ▼
@Suspended ──────────► @Suspended ──────────► @Completed
    │                                               
    │           resume(input)                       
    └──────────────────────────────────────────► @Failed
```

**Initial State**

When a procedure returning `Async<Out, In, Result, E>` is called:

1. The procedure body executes until the first `yield` expression or completion.
2. If `yield v` is reached, the result is `Async@Suspended { output: v }`.
3. If the procedure completes without `yield`, the result is `Async@Completed { value: r }` where `r` is the result value.
4. If an error propagates before any `yield`, the result is `Async@Failed { error: e }`.

**Well-Formedness**

$$\frac{
    \Gamma \vdash Out\ \text{wf} \quad
    \Gamma \vdash In\ \text{wf} \quad
    \Gamma \vdash Result\ \text{wf} \quad
    \Gamma \vdash E\ \text{wf}
}{
    \Gamma \vdash \texttt{Async}\langle Out, In, Result, E \rangle\ \text{wf}
} \quad \text{(Async-WF)}$$

**Default Parameters**

When fewer than four type arguments are provided:
- `Async<T>` ≡ `Async<T, (), (), !>`
- `Async<T, U>` ≡ `Async<T, U, (), !>`
- `Async<T, U, R>` ≡ `Async<T, U, R, !>`

**Variance**

The type parameters of `Async<Out, In, Result, E>` have the following variance:

| Parameter | Variance          | Rationale                           |
| :-------- | :---------------- | :---------------------------------- |
| `Out`     | Covariant (+)     | Output values flow out of the async |
| `In`      | Contravariant (−) | Input values flow into the async    |
| `Result`  | Covariant (+)     | Final result flows out              |
| `E`       | Covariant (+)     | Errors flow out                     |

**Subtyping Rule**

$$\frac{
    \Gamma \vdash Out_1 <: Out_2 \quad
    \Gamma \vdash In_2 <: In_1 \quad
    \Gamma \vdash Result_1 <: Result_2 \quad
    \Gamma \vdash E_1 <: E_2
}{
    \Gamma \vdash \texttt{Async}\langle Out_1, In_1, Result_1, E_1 \rangle <: 
                  \texttt{Async}\langle Out_2, In_2, Result_2, E_2 \rangle
} \quad \text{(Sub-Async)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                         | Detection    | Effect    |
| :------------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-ASYNC-0001` | Error    | `Async` type parameter is not well-formed         | Compile-time | Rejection |
| `E-ASYNC-0002` | Error    | Error type `E` does not satisfy error constraints | Compile-time | Rejection |

---

### 15.3 Type Aliases for Common Patterns

##### Definition

The following type aliases provide convenient names for common asynchronous patterns. These are type aliases, not distinct types; all share the same underlying machinery.

##### Syntax & Declaration

```cursive
/// Produces multiple values, no input needed, trivial completion
type Sequence<T> = Async<T, (), (), !>

/// No intermediate values, completes with result or error
type Future<T, E = !> = Async<(), (), T, E>

/// Receives input, produces transformed output
type Pipe<In, Out> = Async<Out, In, (), !>

/// Symmetric bidirectional communication
type Exchange<T> = Async<T, T, T, !>

/// Stream with possible failure
type Stream<T, E> = Async<T, (), (), E>
```

##### Static Semantics

**Equivalence**

Type aliases are structurally equivalent to their expanded forms:

$$\texttt{Sequence}\langle T \rangle \equiv \texttt{Async}\langle T, (), (), ! \rangle$$

$$\texttt{Future}\langle T, E \rangle \equiv \texttt{Async}\langle (), (), T, E \rangle$$

$$\texttt{Pipe}\langle In, Out \rangle \equiv \texttt{Async}\langle Out, In, (), ! \rangle$$

$$\texttt{Stream}\langle T, E \rangle \equiv \texttt{Async}\langle T, (), (), E \rangle$$

**Variance Inheritance**

Type aliases inherit variance from their expansion:

| Alias           | Expansion               | Variance                              |
| :-------------- | :---------------------- | :------------------------------------ |
| `Sequence<T>`   | `Async<T, (), (), !>`   | Covariant in T                        |
| `Future<T, E>`  | `Async<(), (), T, E>`   | Covariant in T, E                     |
| `Pipe<In, Out>` | `Async<Out, In, (), !>` | Contravariant in In, Covariant in Out |
| `Stream<T, E>`  | `Async<T, (), (), E>`   | Covariant in T, E                     |

---

### 15.4 Producing Async Values

#### 15.4.1 Async-Returning Procedures

##### Definition

A procedure returns an `Async` value by declaring the appropriate return type. The compiler transforms the procedure body into a state machine.

##### Syntax & Declaration

**Grammar**

```ebnf
async_procedure ::= procedure_decl
                    (* where return type is Async<...> *)
```

No special syntax modifier is required. The return type determines async behavior.

##### Static Semantics

**Transformation Rules**

When a procedure's return type is `Async<Out, In, Result, E>`:

1. The compiler transforms the procedure body into a state machine.
2. Local bindings become fields of the state object.
3. Each `yield` expression becomes a suspension point with an associated resumption point identifier.
4. The `result` expression produces the final `@Completed` value.
5. Returning without `result` in a procedure with `Result = ()` implicitly completes with `()`.
6. Error propagation via `?` transitions to `@Failed`.

**(T-Async-Proc)**
$$\frac{
    \Gamma \vdash \text{body} : Result \quad
    \text{yields in body have type } Out \quad
    \text{yields receive type } In
}{
    \Gamma \vdash \texttt{procedure}\ f(\ldots) \to \texttt{Async}\langle Out, In, Result, E \rangle\ \{\ \text{body}\ \}\ \text{wf}
}$$

##### Dynamic Semantics

**Normal Completion**

A procedure completes by executing `result value` or by reaching the end of its body (for `Result = ()`):

```cursive
procedure countdown(n: i32) -> Async<i32, (), i32, !> {
    var i = n
    loop i > 0 {
        yield i
        i -= 1
    }
    result 0
}
```

**Failure**

An `Async` fails when an error propagates via `?` or explicit error return:

```cursive
procedure read_lines(path: string, fs: witness FileSystem) -> Stream<string, IoError> {
    let file = fs~>open(path)?
    loop {
        let line = yield from fs~>read_line(file)?
        if line~>is_empty() {
            return
        }
        yield line
    }
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                       | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0030` | Error    | Error propagation in infallible async procedure | Compile-time | Rejection |

---

#### 15.4.2 The `yield` Expression

##### Definition

The `yield` expression suspends an asynchronous computation, producing an intermediate value and awaiting resumption with an input value.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_expr ::= "yield" expression
```

##### Static Semantics

**Typing Rule**

Within a procedure returning `Async<Out, In, Result, E>`:

$$\frac{
    \text{ReturnType}(\text{enclosing}) = \texttt{Async}\langle Out, In, Result, E \rangle \quad
    \Gamma \vdash e : Out
}{
    \Gamma \vdash \texttt{yield}\ e : In
} \quad \text{(T-Yield)}$$

The operand expression MUST have type `Out`. The `yield` expression evaluates to type `In`—the value provided when the computation is resumed.

**Context Requirement**

The `yield` expression is valid only within a procedure whose return type is `Async<Out, In, Result, E>` for some `Out`, `In`, `Result`, `E`.

**Key Interaction**

When `yield` is combined with a `#` key block in the same expression, the key is released BEFORE suspension.

**Desugaring**

```cursive
yield #path { expr }

// Desugars to:
{
    let __temp = #path { expr }  // Key acquired, expr evaluated, key released
    yield __temp                  // Yield occurs with no keys held
}
```

##### Dynamic Semantics

**Evaluation**

Evaluation of `yield e`:

1. Let $v$ be the result of evaluating $e$.
2. Transition the `Async` to `@Suspended { output: v }`.
3. Return control to the caller.
4. When `resume(input)` is called, bind $input$ as the result of the `yield` expression.
5. Continue execution from the point immediately after `yield`.

##### Constraints & Legality

**Negative Constraints**

1. `yield` MUST NOT appear outside an async-returning procedure.
2. `yield` MUST NOT appear inside a `sync` expression.
3. `yield` MUST NOT appear while any keys are logically held (§13).

**Diagnostic Table**

| Code           | Severity | Condition                                 | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0010` | Error    | `yield` outside async-returning procedure | Compile-time | Rejection |
| `E-ASYNC-0011` | Error    | `yield` operand type does not match `Out` | Compile-time | Rejection |
| `E-ASYNC-0012` | Error    | `yield` inside `sync` expression          | Compile-time | Rejection |
| `E-ASYNC-0013` | Error    | `yield` while key is held                 | Compile-time | Rejection |

---

#### 15.4.3 The `yield from` Expression

##### Definition

The `yield from` expression delegates to another asynchronous computation, forwarding all its outputs and inputs until completion, then evaluating to the delegated computation's result.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_from_expr ::= "yield" "from" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \text{ReturnType}(\text{enclosing}) = \texttt{Async}\langle Out, In, \_, E_1 \rangle \quad
    \Gamma \vdash e : \texttt{Async}\langle Out, In, R, E_2 \rangle \quad
    E_2 <: E_1
}{
    \Gamma \vdash \texttt{yield from}\ e : R
} \quad \text{(T-Yield-From)}$$

**Compatibility Requirements**

The source expression MUST be an `Async` with:
- Identical `Out` parameter to the enclosing procedure
- Identical `In` parameter to the enclosing procedure
- Error type `E_2` that is a subtype of the enclosing error type `E_1`

The expression evaluates to the source's `Result` type.

##### Dynamic Semantics

**Delegation Loop**

Evaluation of `yield from source`:

1. Let $s$ be the result of evaluating $source$.
2. Loop:
   - Match $s$:
     - `@Suspended { output }`: Execute `yield output` in the enclosing computation. When resumed with $input$, let $s$ := $s$~>resume$(input)$.
     - `@Completed { value }`: The `yield from` expression evaluates to $value$. Exit loop.
     - `@Failed { error }`: Propagate $error$ to the enclosing computation. Exit loop.

##### Constraints & Legality

**Negative Constraints**

1. `yield from` MUST NOT appear outside an async-returning procedure.
2. `yield from` MUST NOT appear inside a `sync` expression.
3. `yield from` MUST NOT appear while any keys are logically held.
4. The delegated `Async` MUST have compatible `Out` and `In` parameters.

**Diagnostic Table**

| Code           | Severity | Condition                                    | Detection    | Effect    |
| :------------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0020` | Error    | `yield from` outside async-returning proc    | Compile-time | Rejection |
| `E-ASYNC-0021` | Error    | Incompatible `Out` parameter in `yield from` | Compile-time | Rejection |
| `E-ASYNC-0022` | Error    | Incompatible `In` parameter in `yield from`  | Compile-time | Rejection |
| `E-ASYNC-0023` | Error    | `yield from` inside `sync` expression        | Compile-time | Rejection |
| `E-ASYNC-0024` | Error    | `yield from` while key is held               | Compile-time | Rejection |
| `E-ASYNC-0025` | Error    | Error type not compatible in `yield from`    | Compile-time | Rejection |

---

### 15.5 Consuming Async Values

##### Definition

An async value may be consumed through three patterns: iteration over outputs, manual stepping with explicit state matching, or synchronous execution via `sync` until completion. The appropriate pattern depends on the async type parameters and the consumption context.

---

#### 15.5.1 Iteration (`loop...in`)

##### Definition

The `loop...in` construct iterates over an `Async<T, (), R, E>`, consuming each yielded value until completion or failure. Iteration is available only for async values with `In = ()`.

##### Syntax & Declaration

**Grammar**

```ebnf
async_loop ::= "loop" pattern "in" expression block
```

**Desugaring**

```cursive
loop item in source {
    body
}

// Desugars to:
{
    var __s = source
    loop {
        match __s {
            @Suspended { output: item } => {
                body
                __s = __s~>resume(())
            }
            @Completed { .. } => break,
            @Failed { error } => panic(error),
        }
    }
}
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle T, (), R, E \rangle \quad
    \Gamma, x : T \vdash \text{body} : ()
}{
    \Gamma \vdash \texttt{loop}\ x\ \texttt{in}\ e\ \{ \text{body} \} : ()
} \quad \text{(T-Async-Loop)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                           | Detection    | Effect    |
| :------------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-ASYNC-0040` | Error    | Iteration over async with `In ≠ ()` | Compile-time | Rejection |

---

#### 15.5.2 Manual Stepping

##### Definition

Direct interaction with the `Async` state machine via pattern matching and explicit `resume` transition invocation. This pattern is required for async values with non-unit `In` types.

##### Dynamic Semantics

**Pattern Matching on State**

The consumer matches on the async value's modal state and invokes the `resume` transition to advance:

```cursive
var acc = accumulator(0)

match acc {
    @Suspended { output } => {
        assert(output == 0)
        acc = acc~>resume(5)
    }
    _ => panic("expected suspended"),
}

match acc {
    @Suspended { output } => {
        assert(output == 5)
        acc = acc~>resume(3)
    }
    _ => panic("expected suspended"),
}

match acc {
    @Completed { value } => assert(value == 8),
    _ => panic("expected completed"),
}
```

**Chained Resumption**

Multiple resumptions may be chained when the intermediate states are not inspected:

```cursive
// Step through three iterations, ignoring intermediate outputs
var seq = some_sequence()
seq = seq~>resume(())
seq = seq~>resume(())
seq = seq~>resume(())
```

---

#### 15.5.3 Synchronous Execution (`sync`)

##### Definition

The `sync` expression runs a `Future<T, E>` (equivalently `Async<(), (), T, E>`) to completion synchronously, halting the current execution context until the result is available.

##### Syntax & Declaration

**Grammar**

```ebnf
sync_expr ::= "sync" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle (), (), T, E \rangle
}{
    \Gamma \vdash \texttt{sync}\ e : T \mid E
} \quad \text{(T-Sync)}$$

The operand MUST be an `Async` with `Out = ()` and `In = ()`.

**Context Restriction**

The `sync` expression is permitted only in non-async contexts (procedures not returning `Async`).

##### Dynamic Semantics

**Evaluation**

Evaluation of `sync e`:

1. Let $a$ be the result of evaluating $e$.
2. Loop:
   - Match $a$:
     - `@Suspended { output: () }`: Let $a$ := $a$~>resume$(())$.
     - `@Completed { value }`: Return $value$.
     - `@Failed { error }`: Propagate $error$.

**Reactor Integration**

The `sync` expression desugars to use the reactor capability (§15.11.1):

```cursive
sync future

// Semantically equivalent to:
ctx.reactor~>run(future)
```

The reactor polls the future and any dependent I/O operations until completion.

##### Constraints & Legality

**Negative Constraints**

1. `sync` MUST NOT appear inside an async-returning procedure.
2. The operand MUST have `Out = ()` and `In = ()`.

**Diagnostic Table**

| Code           | Severity | Condition                          | Detection    | Effect    |
| :------------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-ASYNC-0050` | Error    | `sync` inside async-returning proc | Compile-time | Rejection |
| `E-ASYNC-0051` | Error    | `sync` operand has `Out ≠ ()`      | Compile-time | Rejection |
| `E-ASYNC-0052` | Error    | `sync` operand has `In ≠ ()`       | Compile-time | Rejection |

---

### 15.6 Concurrent Composition

##### Definition

Concurrent composition allows multiple async operations to execute simultaneously. The `race` expression returns when the first operation completes; the `all` expression returns when all operations complete; the `until` method suspends until a condition becomes true.

---

#### 15.6.1 The `race` Expression

##### Definition

The `race` expression initiates multiple asynchronous operations concurrently and returns when the first completes. When handler arms use `yield`, the expression produces a streaming result.

##### Syntax & Declaration

**Grammar**

```ebnf
race_expr    ::= "race" "{" race_arm ("," race_arm)* [","] "}"
race_arm     ::= expression "->" "|" pattern "|" race_handler
race_handler ::= expression | "yield" expression
```

##### Static Semantics

**First-Completion Typing (Non-Streaming)**

$$\frac{
    \forall i \in 1..n.\ \Gamma \vdash e_i : \texttt{Async}\langle (), (), T_i, E_i \rangle \quad
    \forall i \in 1..n.\ \Gamma, x_i : T_i \vdash r_i : R \quad
    n \geq 2 \quad
    \text{no handler uses yield}
}{
    \Gamma \vdash \texttt{race}\ \{ e_1 \to |x_1|\ r_1, \ldots, e_n \to |x_n|\ r_n \} : R \mid (E_1 | \ldots | E_n)
} \quad \text{(T-Race)}$$

All handler arms MUST produce the same result type. The error type is the union of all constituent error types.

**Streaming Typing**

$$\frac{
    \forall i \in 1..n.\ \Gamma \vdash e_i : \texttt{Async}\langle T_i, (), R_i, E_i \rangle \quad
    \forall i \in 1..n.\ \Gamma, x_i : T_i \vdash u_i : U \quad
    \exists j.\ T_j \neq () \quad
    \text{all handlers use yield}
}{
    \Gamma \vdash \texttt{race}\ \{ \ldots \} : \texttt{Sequence}\langle U, E_1 | \ldots | E_n \rangle
} \quad \text{(T-Race-Stream)}$$

When all arms use `yield` in their handlers, the result is a `Sequence` that yields each handled value.

##### Dynamic Semantics

**First-Completion Evaluation**

1. Initiate all async expressions concurrently.
2. When any operation reaches `@Completed` or `@Failed`:
   - Execute the corresponding handler arm (binding the result or error to the pattern).
   - Cancel all other operations (drop them, invoking cleanup per §15.9).
   - Return the handler's result.
3. If an operation fails, the failure propagates unless explicitly handled in the arm.

**Streaming Evaluation**

1. Initiate all async expressions concurrently.
2. When any arm yields a value (transitions to `@Suspended`):
   - Execute the corresponding handler, binding the output to the pattern.
   - Yield the handler's result from the streaming race.
   - Resume the arm that produced the value.
3. When an arm completes (`@Completed`): remove it from the race.
4. When all arms complete: the streaming race completes.
5. If any arm fails: propagate error, cancel remaining arms.

**Cancellation**

When one arm completes (non-streaming) or fails:
1. All other in-flight operations are dropped.
2. `Drop` implementations execute per §15.9.
3. Resources are released before the race expression completes.

##### Constraints & Legality

**Negative Constraints**

1. `race` MUST have at least 2 arms.
2. All arms MUST have compatible result types.
3. All operand expressions in non-streaming race MUST have `Out = ()` and `In = ()`.
4. All arms MUST use the same handler form (all `yield` or all non-`yield`).

**Diagnostic Table**

| Code           | Severity | Condition                                   | Detection    | Effect    |
| :------------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-ASYNC-0060` | Error    | `race` with fewer than 2 arms               | Compile-time | Rejection |
| `E-ASYNC-0061` | Error    | `race` arms have incompatible types         | Compile-time | Rejection |
| `E-ASYNC-0062` | Error    | Non-streaming `race` operand has `Out ≠ ()` | Compile-time | Rejection |
| `E-ASYNC-0063` | Error    | Mixed yield/non-yield handlers in race      | Compile-time | Rejection |

---

#### 15.6.2 The `all` Expression

##### Definition

The `all` expression initiates multiple asynchronous operations concurrently and returns when all complete successfully, or when any fails.

##### Syntax & Declaration

**Grammar**

```ebnf
all_expr ::= "all" "{" expression ("," expression)* [","] "}"
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \forall i \in 1..n.\ \Gamma \vdash e_i : \texttt{Async}\langle (), (), T_i, E_i \rangle
}{
    \Gamma \vdash \texttt{all}\ \{ e_1, \ldots, e_n \} : (T_1, \ldots, T_n) \mid (E_1 | \ldots | E_n)
} \quad \text{(T-All)}$$

The result is a tuple of all completion values. The error type is the union of all constituent error types.

##### Dynamic Semantics

**All-Completion Evaluation**

1. Initiate all async expressions concurrently.
2. Wait for all operations to complete.
3. If all succeed: return a tuple of results in declaration order.
4. If any fails: cancel remaining operations, propagate the first error.

**First-Failure Semantics**

When any operation transitions to `@Failed`:
1. Record the error.
2. Cancel all remaining in-flight operations.
3. Wait for cancellation to complete (cleanup runs).
4. Propagate the recorded error.

##### Constraints & Legality

**Negative Constraints**

1. All operand expressions MUST have `Out = ()` and `In = ()`.

**Diagnostic Table**

| Code           | Severity | Condition                    | Detection    | Effect    |
| :------------- | :------- | :--------------------------- | :----------- | :-------- |
| `E-ASYNC-0070` | Error    | `all` operand has `Out ≠ ()` | Compile-time | Rejection |
| `E-ASYNC-0071` | Error    | `all` operand has `In ≠ ()`  | Compile-time | Rejection |

---

#### 15.6.3 Condition Waiting (`until`)

##### Definition

Condition waiting suspends an async operation until a predicate on shared data becomes true. The `until` method is provided on all `shared` types.

##### Syntax & Declaration

**Method Signature**

Every `shared` type provides a compiler-generated `until` method:

```cursive
procedure until<T, R>(
    self: shared T,
    predicate: procedure(const T) -> bool,
    action: procedure(unique T) -> R,
) -> Future<R>
```

##### Dynamic Semantics

**Predicate Registration and Wake**

Evaluation of `shared_value~>until(pred, action)`:

1. If `pred(shared_value)` is true immediately:
   - Acquire Write key to `shared_value`.
   - Execute `action(shared_value)`.
   - Complete the Future with the result.
2. Otherwise:
   - Register a waiter record: $(path, pred, action, continuation)$.
   - Transition the Future to `@Suspended { output: () }`.
   - The waiter does NOT hold any keys while suspended.

**Notification on Key Release**

When a Write key to path $P$ is released (per §13.2):

1. The runtime checks for waiters registered on $P$ or any prefix of $P$.
2. Each matching waiter is marked **potentially ready**.
3. Potentially ready waiters re-evaluate their predicates.
4. Waiters whose predicates return `true` are transitioned to **ready** and scheduled for execution.

**Notification Granularity**

| Write Key Released | Waiters Notified                          |
| :----------------- | :---------------------------------------- |
| `player.health`    | Waiters on `player.health`, `player`      |
| `player`           | All waiters on `player` or any `player.*` |
| `arr[i]`           | Waiters on `arr[i]`, `arr`                |

**Predicate Re-evaluation**

When a waiter is notified:

1. Acquire a Read key to the waiter's path.
2. Evaluate $pred(value)$.
3. If true: upgrade to Write key, execute $action(value)$, complete the Future.
4. If false: release Read key, remain in wait table.

**Ordering Guarantees**

1. A waiter that observes a mutation's effect is guaranteed to see all prior mutations by the same task (program order).
2. Across tasks, waiters see mutations in an order consistent with key acquisition order.
3. Spurious wakeups are permitted.

---

### 15.7 Transformations and Combinators

##### Definition

The `Async` type provides methods for transforming and composing asynchronous computations. These combinators enable declarative pipeline construction.

##### Static Semantics

> **Invocation Syntax**
>
> Combinator methods are invoked via the `~>` dispatch operator:
>
> ```cursive
> let doubled = sequence~>map(|x| x * 2)
> let first_ten = sequence~>take(10)
> let total = sequence~>fold(0, |acc, x| acc + x)
> ```

**Core Transformation Signatures**

**`map`**: Transform output values.

```cursive
procedure map<Out, In, Result, E, U>(
    self: Async<Out, In, Result, E>,
    f: procedure(Out) -> U,
) -> Async<U, In, Result, E>
```

$$\frac{
    \Gamma \vdash a : \texttt{Async}\langle Out, In, R, E \rangle \quad
    \Gamma \vdash f : Out \to U
}{
    \Gamma \vdash a\texttt{~>map}(f) : \texttt{Async}\langle U, In, R, E \rangle
} \quad \text{(T-Async-Map)}$$

**`filter`**: Conditionally yield values.

```cursive
procedure filter<T, E>(
    self: Async<T, (), (), E>,
    predicate: procedure(const T) -> bool,
) -> Async<T, (), (), E>
```

**`take`**: Limit number of outputs.

```cursive
procedure take<T, E>(
    self: Async<T, (), (), E>,
    count: usize,
) -> Async<T, (), (), E>
```

**`fold`**: Reduce to single value.

```cursive
procedure fold<T, A, E>(
    self: Async<T, (), (), E>,
    initial: A,
    combine: procedure(A, T) -> A,
) -> Future<A, E>
```

**`chain`**: Sequence dependent async operations.

```cursive
procedure chain<T, U, E>(
    self: Future<T, E>,
    next: procedure(T) -> Future<U, E>,
) -> Future<U, E>
```

$$\frac{
    \Gamma \vdash a : \texttt{Future}\langle T, E \rangle \quad
    \Gamma \vdash f : T \to \texttt{Future}\langle U, E \rangle
}{
    \Gamma \vdash a\texttt{~>chain}(f) : \texttt{Future}\langle U, E \rangle
} \quad \text{(T-Async-Chain)}$$

##### Dynamic Semantics

**Lazy Evaluation Model**

Combinators do not eagerly evaluate their source async. Instead, they return a new async that, when resumed, applies the transformation:

1. `map(f)`: When the source yields $v$, yield $f(v)$.
2. `filter(p)`: When the source yields $v$, yield $v$ only if $p(v)$ is true; otherwise, resume source.
3. `take(n)`: Yield the first $n$ values from source, then complete.
4. `fold(init, f)`: Consume all source values, accumulating via $f$, then complete with final accumulator.
5. `chain(f)`: When source completes with $v$, evaluate $f(v)$ and delegate to the resulting async.

---

### 15.8 Memory Model and State Representation

##### Definition

The compiler transforms an async procedure's local state into a state object that persists across suspensions. This section specifies the memory representation.

##### Memory & Layout

**State Object Structure**

An async procedure is transformed into a state object containing:
- All local bindings that are live across any suspension point
- A discriminant indicating the current resumption point
- The output/result/error payload (unioned, as only one is active)

```cursive
// Source
procedure range(start: i32, end: i32) -> Sequence<i32> {
    var i = start
    loop i < end {
        yield i
        i += 1
    }
}

// Conceptual transformation (not user-visible)
record __RangeState {
    i: i32,
    end: i32,
    __resumption_point: u8,
}
```

**Size Formula**

$$\text{sizeof}(\texttt{Async}\langle Out, In, Result, E \rangle) = \text{sizeof}(\text{State}) + \max(\text{sizeof}(Out), \text{sizeof}(Result), \text{sizeof}(E)) + \text{sizeof}(\text{Discriminant})$$

**ABI Classification**

| Property              | Classification         |
| :-------------------- | :--------------------- |
| State object layout   | Implementation-Defined |
| Discriminant encoding | Implementation-Defined |
| Alignment             | Implementation-Defined |

##### Dynamic Semantics

**Region Allocation**

Async state may be allocated in regions (§12.3):

```cursive
region task_arena {
    let ops = [
        ^task_arena range(0, 100),
        ^task_arena range(100, 200),
    ]
    
    loop op in ops {
        loop value in op {
            process(value)
        }
    }
}    // All async states freed here
```

**Heap Escape**

When async values escape their defining scope, they MUST be heap-allocated:

```cursive
procedure create_counter(
    start: i32, 
    heap: witness HeapAllocator,
) -> Ptr<Sequence<i32>>@Valid {
    heap~>alloc(range(start, i32::MAX))
}
```

**Structured Lifetime**

An async operation allocated in a region MUST NOT escape that region:

$$\text{lifetime}(A) \leq \text{lifetime}(\text{region}(A))$$

---

### 15.9 Cancellation and Cleanup

##### Definition

Cancellation terminates an async operation before completion, ensuring proper cleanup of resources. Cursive uses drop-based cancellation.

##### Dynamic Semantics

**Drop-Based Cancellation**

When an `Async@Suspended` value is dropped, the operation is cancelled:

```cursive
{
    let op = ctx.net~>fetch(url)
    // op dropped here without being consumed
}    // Fetch cancelled
```

**Explicit Cancellation**

For controlled cancellation with a signal, use `race` with a cancellation condition:

```cursive
procedure cancellable_fetch(
    url: string,
    cancel: shared CancelFlag,
    net: witness Network,
) -> Future<Response | Cancelled, NetError> {
    race {
        net~>fetch(url) -> |response| result response,
        cancel~>until(|c| c.is_set, |_| ()) -> |_| result Cancelled,
    }
}
```

**Cleanup Order**

When an async operation is cancelled:

1. Execution does not continue past the current suspension point.
2. `defer` blocks are executed in reverse declaration order.
3. `Drop` implementations are called for all captured resources.
4. Cleanup proceeds from innermost scope outward.

##### Constraints & Legality

**In-Flight I/O Behavior**

The behavior of in-flight I/O operations on cancellation is Implementation-Defined Behavior (IDB):

| I/O State          | Permitted Behaviors                    |
| :----------------- | :------------------------------------- |
| Pending read/write | Cancel, complete, or abandon           |
| Connected socket   | Close immediately or graceful shutdown |
| Open file          | Flush and close, or abandon            |

Implementations MUST document their cancellation behavior in the Conformance Dossier.

---

### 15.10 Error Handling

##### Definition

Errors propagate through async boundaries via the `?` operator or explicit pattern matching on the `@Failed` state.

##### Static Semantics

**Error Propagation via `?`**

The `?` operator propagates errors through async boundaries:

$$\frac{
    \text{ReturnType}(\text{enclosing}) = \texttt{Async}\langle Out, In, R, E_1 \rangle \quad
    \Gamma \vdash e : T \mid E_2 \quad
    E_2 <: E_1
}{
    \Gamma \vdash e? : T
} \quad \text{(T-Async-Try)}$$

**Error Type Constraints**

A procedure returning `Async<Out, In, Result, E>` where `E = !` MUST NOT contain error-propagating expressions for fallible operations.

##### Dynamic Semantics

**Cleanup on Failure**

When an async operation fails:

1. The error is captured in the `@Failed` state.
2. `defer` blocks execute in reverse order.
3. `Drop` implementations run for all live bindings.
4. The async transitions to `@Failed { error }`.

```cursive
procedure with_temp_file(fs: witness FileSystem) -> Stream<string, IoError> {
    let temp = fs~>create_temp()?
    defer { fs~>delete(temp) }    // Runs on completion, failure, or drop

    loop line in lines {
        fs~>write(temp, line)?
        yield line
    }

    result fs~>finalize(temp)?
}
```

---

### 15.11 Integration with Other Language Features

##### Definition

This section specifies how async operations interact with capabilities, the permission system, and parallel blocks.

---

#### 15.11.1 Capability Requirements

##### Static Semantics

**Pure vs I/O Async**

| Async Category | Capability Required | Example                |
| :------------- | :------------------ | :--------------------- |
| Pure sequence  | None                | `range(0, 100)`        |
| I/O operation  | Specific witness    | `fs~>read(file)`       |
| Timing         | `System` witness    | `sys~>after(duration)` |
| Network        | `Network` witness   | `net~>fetch(url)`      |

**Capability Propagation**

Capabilities captured by an async procedure remain valid across suspensions. The capability witness is stored in the async state object.

**Reactor Capability**

The `Reactor` capability manages the lifecycle of suspended async operations awaiting external events:

```cursive
form Reactor {
    /// Run a Future to completion, polling until done.
    procedure run<T, E>(~, future: Future<T, E>) -> T | E
    
    /// Register a future for concurrent execution.
    procedure register<T, E>(~, future: Future<T, E>) -> FutureHandle<T, E>
}
```

The `sync` expression implicitly uses the reactor from `Context`.

**I/O Capability Methods**

Capability forms provide methods returning `Async`:

```cursive
form FileSystem {
    procedure open(~, path: string) -> FileHandle | IoError
    procedure read(~, file: FileHandle) -> Future<Data, IoError>
    procedure write(~, file: FileHandle, data: const [u8]) -> Future<usize, IoError>
    procedure close(~, file: FileHandle) -> Future<(), IoError>
}

form Network {
    procedure connect(~, addr: NetAddr) -> Future<Connection, NetError>
    procedure send(~, conn: Connection, data: const [u8]) -> Future<usize, NetError>
    procedure receive(~, conn: Connection) -> Future<[u8], NetError>
    procedure fetch(~, url: string) -> Future<Response, NetError>
}

form Time {
    procedure now(~) -> Timestamp
    procedure after(~, duration: Duration) -> Future<()>
}
```

---

#### 15.11.2 Permission and Capture Rules

##### Static Semantics

**Capture Table**

| Permission | Capturable | Behavior                                |
| :--------- | :--------- | :-------------------------------------- |
| `const T`  | Yes        | Referenced data must outlive the async  |
| `unique T` | Yes        | Exclusive access for async's lifetime   |
| `shared T` | Yes        | Subject to key rules at each suspension |
| `move T`   | Yes        | Ownership transfers to async's state    |

**`[[dynamic]]` Inheritance in Parallel Contexts**

When a `spawn` or `dispatch` block captures `shared` data, the `[[dynamic]]` status is inherited from the enclosing scope:

```cursive
[[dynamic]]
procedure parallel_updates(data: shared [Record], indices: [usize]) {
    parallel {
        loop idx in indices {
            spawn {
                data[idx].process()    // Inherits [[dynamic]]: runtime sync permitted
            }
        }
    }
}

procedure static_parallel(data: shared [Record]) {
    parallel {
        spawn { data[0].process() }    // Must be statically safe
        spawn { data[1].process() }    // Must be statically safe
        // spawn { data[i].process() } // Would be Error E-KEY-0020
    }
}
```

**Capture with `[[dynamic]]` on Spawn**

The `[[dynamic]]` attribute may be applied to individual spawn expressions:

```cursive
procedure mixed_parallel(data: shared [Record], idx: usize) {
    parallel {
        spawn { data[0].process() }           // Static: must be provable
        [[dynamic]] spawn { data[idx].update() }  // Dynamic: runtime sync
    }
}
```

**Lifetime Constraint**

For any binding $b$ with permission $p$ captured by an async procedure:

$$\text{lifetime}(b) \geq \text{lifetime}(\texttt{Async})$$

The captured data MUST remain valid for the entire lifetime of the `Async` value.

##### Constraints & Legality

**Key Prohibition at Suspension**

A `yield` or `yield from` expression MUST NOT occur while any keys are logically held.

$$\frac{
    \Gamma_{\text{keys}} \neq \emptyset \quad
    \text{IsYield}(e) \lor \text{IsYieldFrom}(e)
}{
    \text{Emit}(\texttt{E-ASYNC-0013})
} \quad \text{(A-No-Keys)}$$

> **Rationale:** Keys represent synchronization primitives. Holding a key across suspension could cause deadlock or priority inversion.

**Valid Pattern**

```cursive
procedure read_player_stats(player: shared Player, ctx: Context) -> Sequence<i32> {
    loop {
        let health = #player { player.health }    // Key acquired and released
        yield health                               // No key held
        yield from ctx.sys~>after(1.second)
    }
}
```

**Invalid Pattern**

```cursive
procedure bad(player: shared Player) -> Sequence<i32> {
    #player {
        yield player.health    // ERROR E-ASYNC-0013: key held at yield
    }
}
```

**Diagnostic Table**

| Code           | Severity | Condition                               | Detection    | Effect    |
| :------------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0080` | Error    | Captured binding does not outlive async | Compile-time | Rejection |
| `E-ASYNC-0081` | Error    | Async operation escapes its region      | Compile-time | Rejection |
| `W-ASYNC-0001` | Warning  | Large captured state (performance)      | Compile-time | Warning   |

---

#### 15.11.3 Parallel Block Composition

##### Static Semantics

**Async in `spawn` Blocks**

`spawn` blocks may contain async operations:

```cursive
parallel ctx.cpu {
    spawn {
        let data = yield from ctx.net~>fetch(url)
        process(data)
    }
    spawn {
        let config = yield from ctx.fs~>read("/config")
        apply(config)
    }
}
```

When a spawned task encounters `yield from` on an I/O Future:
1. The task suspends.
2. The reactor registers the I/O operation.
3. The worker picks up other work items.
4. When I/O is ready, the reactor notifies and the task resumes.

**Async in `dispatch`**

`dispatch` may iterate over async sequences:

```cursive
parallel ctx.cpu {
    dispatch item in async_producer() {
        process(item)
    }
}
```

Each yielded item is processed as a separate work item.

**Structured Concurrency**

All async operations spawned within a `parallel` block MUST complete or be cancelled before the block exits.

When a `parallel` block reaches its closing brace:
1. Wait for all `spawn` tasks to complete or fail.
2. If the block exits early (panic, break), cancel all pending tasks.
3. Run `defer` and `Drop` for all task-local state.
4. Propagate the first panic (if any) after cleanup.

---

# Part 5: Metaprogramming

---

## Clause 16: Compile-Time Execution

This clause defines the compile-time execution model for Cursive. Compile-time execution enables computation during the Metaprogramming Phase (Translation Phase 2, as defined in §2.12), producing values, types, and code that are incorporated into the final program. This clause establishes the foundational execution environment, type restrictions, and capability model upon which type reflection (§17), code generation (§18), and derivation (§19) depend.

---

### 16.1 The Comptime Environment

##### Definition

**Compile-time execution** is the evaluation of Cursive code during the Metaprogramming Phase of translation, prior to semantic analysis of the complete program. Code executed at compile time operates within the **comptime environment**.

The **comptime environment** is a sandboxed execution context, denoted $\Gamma_{ct}$, that is distinct from the runtime environment $\Gamma_{rt}$. The comptime environment provides access to a restricted subset of language features sufficient for type introspection, code construction, and program transformation.

**Formal Definition**

$$\Gamma_{ct} ::= (\Sigma_{stdlib}, \Sigma_{imports}, \Sigma_{types}, \Sigma_{caps}, \emptyset)$$

The components of $\Gamma_{ct}$ are:

| Component          | Description                                                              |
| :----------------- | :----------------------------------------------------------------------- |
| $\Sigma_{stdlib}$  | The subset of the standard library available at compile time             |
| $\Sigma_{imports}$ | Modules imported via `import` declarations visible at the comptime block |
| $\Sigma_{types}$   | Type definitions visible at the point of comptime execution              |
| $\Sigma_{caps}$    | Compile-time capabilities provided to the current context (§16.5)        |
| $\emptyset$        | Empty set indicating no shared mutable state with $\Gamma_{rt}$          |

The fifth component ($\emptyset$) enforces the critical invariant that compile-time execution cannot observe or modify runtime state.

##### Static Semantics

**Isolation Property**

The comptime environment is isolated from the runtime environment. Comptime code MUST NOT access:

1. Runtime memory or heap allocations from $\Gamma_{rt}$
2. File handles, network sockets, or other I/O resources (except via the `ProjectFiles` capability defined in §16.5.3)
3. Foreign function interfaces (FFI) as defined in §17
4. Runtime capabilities as defined in §12

**Determinism Property**

Comptime code MUST be deterministic. Given identical source input and identical compiler configuration, comptime execution MUST produce identical output.

**Formal Determinism Requirement**

For any comptime expression $e$ evaluated in environment $\Gamma_{ct}$:

$$\Gamma_{ct} \vdash e \Downarrow v_1 \land \Gamma_{ct} \vdash e \Downarrow v_2 \implies v_1 = v_2$$

**Termination Property**

Comptime execution MUST terminate. Implementations MUST enforce resource limits as specified in §16.8 to guarantee termination.

**Purity Requirement**

All expressions evaluated in comptime context MUST be pure as defined in §10.1.1, with the following exceptions:

1. Operations on compile-time capabilities (§16.5) constitute controlled side effects on the compilation unit.
2. File reads via the `ProjectFiles` capability (§16.5.3) are permitted.
3. Diagnostic emissions via the `ComptimeDiagnostics` capability (§16.5.4) are permitted.

##### Constraints & Legality

The following constructs are prohibited in comptime context:

| Construct                       | Rationale                                                           |
| :------------------------------ | :------------------------------------------------------------------ |
| `unsafe` blocks                 | Comptime execution cannot verify memory safety of unsafe operations |
| FFI calls (`extern` procedures) | Foreign code may have side effects or non-determinism               |
| Runtime capability access       | Runtime capabilities are not available during compilation           |
| Random number generation        | Violates determinism property                                       |
| System time queries             | Violates determinism property                                       |
| I/O operations                  | Violates isolation property (except `ProjectFiles`)                 |

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-CTE-0001` | Error    | `unsafe` block in comptime context              | Compile-time | Rejection |
| `E-CTE-0002` | Error    | FFI call in comptime context                    | Compile-time | Rejection |
| `E-CTE-0003` | Error    | Runtime capability access in comptime context   | Compile-time | Rejection |
| `E-CTE-0004` | Error    | Non-deterministic operation in comptime context | Compile-time | Rejection |
| `E-CTE-0005` | Error    | Prohibited I/O operation in comptime context    | Compile-time | Rejection |

---

### 16.2 Comptime-Available Types

##### Definition

A **comptime-available type** is a type whose values can exist within the comptime environment. Only comptime-available types may appear as parameter types, return types, or local variable types in comptime procedures and blocks.

**Formal Definition**

A type $T$ is comptime-available if and only if the predicate $\text{IsComptimeAvailable}(T)$ holds:

$$\text{IsComptimeAvailable}(T) \iff T \in \mathcal{C}$$

The set $\mathcal{C}$ is the smallest set satisfying the following inductive rules:

**Primitive Types**

$$\{i8, i16, i32, i64, i128, u8, u16, u32, u64, u128, isize, usize, f32, f64, bool, char\} \subseteq \mathcal{C}$$

**String Type**

$$\texttt{string} \in \mathcal{C}$$

**Unit and Never Types**

$$() \in \mathcal{C} \quad \text{and} \quad ! \in \mathcal{C}$$

**Compile-Time Specific Types**

The following types, defined in §17 and §18, are comptime-available:

$$\{\texttt{TypePattern}, \texttt{TypeRepr}, \texttt{FormRepr}, \texttt{Ast}\langle T \rangle@S\} \subseteq \mathcal{C}$$

for any comptime-available $T$ and any state $S$ of the `Ast` modal type.

**Introspection Types**

The following introspection record types, defined in §17.4, are comptime-available:

$$\{\texttt{FieldInfo}, \texttt{VariantInfo}, \texttt{StateInfo}, \texttt{TransitionInfo}, \texttt{ParamInfo}, \texttt{LayoutInfo}, \texttt{Attribute}, \texttt{AttributeArg}\} \subseteq \mathcal{C}$$

**Source Location Types**

$$\{\texttt{SourceSpan}, \texttt{TypeError}\} \subseteq \mathcal{C}$$

**Composite Type Rules**

The following rules define comptime-availability for composite types:

$$\frac{\forall i \in 1..n.\ T_i \in \mathcal{C}}{(T_1, T_2, \ldots, T_n) \in \mathcal{C}} \quad \text{(C-Tuple)}$$

$$\frac{T \in \mathcal{C} \quad N : \texttt{usize}}{[T; N] \in \mathcal{C}} \quad \text{(C-Array)}$$

$$\frac{T \in \mathcal{C}}{[T] \in \mathcal{C}} \quad \text{(C-Slice)}$$

$$\frac{T \in \mathcal{C}}{\texttt{Option}\langle T \rangle \in \mathcal{C}} \quad \text{(C-Option)}$$

$$\frac{T \in \mathcal{C} \quad E \in \mathcal{C}}{\texttt{Result}\langle T, E \rangle \in \mathcal{C}} \quad \text{(C-Result)}$$

$$\frac{T \in \mathcal{C}}{\texttt{Sequence}\langle T \rangle \in \mathcal{C}} \quad \text{(C-Sequence)}$$

**Record Types**

$$\frac{\text{record } R\ \{\ f_1: T_1, \ldots, f_n: T_n\ \} \quad \forall i.\ T_i \in \mathcal{C}}{R \in \mathcal{C}} \quad \text{(C-Record)}$$

**Enum Types**

$$\frac{\text{enum } E\ \{\ V_1(P_1), \ldots, V_n(P_n)\ \} \quad \forall i.\ P_i \in \mathcal{C} \lor P_i = ()}{E \in \mathcal{C}} \quad \text{(C-Enum)}$$

**Procedure Types**

Procedure types are comptime-available when all parameter types and the return type are comptime-available:

$$\frac{\forall i.\ T_i \in \mathcal{C} \quad R \in \mathcal{C}}{(T_1, \ldots, T_n) \to R \in \mathcal{C}} \quad \text{(C-Proc)}$$

##### Static Semantics

**Excluded Types**

The following type categories are explicitly excluded from $\mathcal{C}$:

| Type Category                                   | Rationale                                   |
| :---------------------------------------------- | :------------------------------------------ |
| Runtime capabilities (§12)                      | Require runtime context                     |
| Handles and resources                           | Represent runtime system state              |
| Raw pointers (`*imm T`, `*mut T`)               | Cannot verify memory safety at compile time |
| References with `unique` or `shared` permission | Require runtime borrow tracking             |
| Modal types other than `Ast`                    | Require runtime state machines              |
| Types containing excluded types                 | Compositional closure                       |

**Const References**

References with `const` permission to comptime-available types are comptime-available:

$$\frac{T \in \mathcal{C}}{\texttt{const}\ T \in \mathcal{C}} \quad \text{(C-Const-Ref)}$$

**Validation**

The implementation MUST verify that all types used in comptime context satisfy $\text{IsComptimeAvailable}(T)$. This verification occurs during semantic analysis of comptime procedures and blocks.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                        | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------------- | :----------- | :-------- |
| `E-CTE-0010` | Error    | Non-comptime-available type used in comptime context             | Compile-time | Rejection |
| `E-CTE-0011` | Error    | Parameter type of comptime procedure not comptime-available      | Compile-time | Rejection |
| `E-CTE-0012` | Error    | Return type of comptime procedure not comptime-available         | Compile-time | Rejection |
| `E-CTE-0013` | Error    | Local variable in comptime block has non-comptime-available type | Compile-time | Rejection |

---

### 16.3 Comptime Procedures

##### Definition

A **comptime procedure** is a procedure declared with the `comptime` modifier. A comptime procedure executes during the Metaprogramming Phase and MUST NOT be called from runtime context.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_procedure ::= attribute_list? visibility? "comptime" "procedure" IDENTIFIER
                       generic_params? "(" param_list? ")"
                       (":" return_type)? contract_clause?
                       block
```

The productions `attribute_list`, `visibility`, `generic_params`, `param_list`, `return_type`, `contract_clause`, and `block` are defined in §7.2, §8.5, §7.1, §5.5, §4, §10.2, and §11.2 respectively.

**Example**

```cursive
comptime procedure factorial(n: u64): u64 {
    match n {
        0 | 1 => result 1,
        _ => result n * factorial(n - 1),
    }
}
```

##### Static Semantics

**Callability Rules**

A comptime procedure MAY be called from:

1. Other comptime procedures
2. Comptime blocks (§16.4)
3. `const` initializers (§4.3)
4. Derive target procedures (§19.3)
5. Conditional implementation bodies (§19.5)

A comptime procedure MUST NOT be called from runtime context. An attempted call from runtime context is ill-formed.

**Parameter Type Restrictions**

All parameter types of a comptime procedure MUST be comptime-available as defined in §16.2:

$$\frac{
    \texttt{comptime procedure } f(x_1: T_1, \ldots, x_n: T_n): R \quad
    \exists i.\ \neg\text{IsComptimeAvailable}(T_i)
}{
    \text{ill-formed}
} \quad \text{(Comptime-Param-Check)}$$

**Return Type Restrictions**

The return type of a comptime procedure MUST be comptime-available:

$$\frac{
    \texttt{comptime procedure } f(\ldots): R \quad
    \neg\text{IsComptimeAvailable}(R)
}{
    \text{ill-formed}
} \quad \text{(Comptime-Return-Check)}$$

**Typing Rule**

$$\frac{
    \forall i.\ \text{IsComptimeAvailable}(T_i) \quad
    \text{IsComptimeAvailable}(R) \quad
    \Gamma_{ct}, x_1: T_1, \ldots, x_n: T_n \vdash \textit{body} : R
}{
    \Gamma_{ct} \vdash \texttt{comptime procedure } f(x_1: T_1, \ldots, x_n: T_n): R\ \{\ \textit{body}\ \}
} \quad \text{(T-Comptime-Proc)}$$

**Generic Comptime Procedures**

Comptime procedures MAY have generic type parameters. Type parameters are instantiated at compile time when the procedure is called. All instantiated types MUST be comptime-available.

**Contract Clauses**

Comptime procedures MAY have contract clauses (preconditions and postconditions) as defined in §10.2. Contracts on comptime procedures are evaluated at compile time during each invocation.

##### Dynamic Semantics

**Execution**

When a comptime procedure is called during the Metaprogramming Phase:

1. Arguments are evaluated in the comptime environment.
2. Parameter bindings are established in a new scope.
3. The procedure body executes in the comptime environment.
4. The result value is returned to the caller.

**Recursion**

Comptime procedures MAY be recursive. The implementation MUST enforce a recursion depth limit as specified in §16.8.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                  | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------- | :----------- | :-------- |
| `E-CTE-0020` | Error    | Comptime procedure called from runtime context             | Compile-time | Rejection |
| `E-CTE-0021` | Error    | Non-comptime-available parameter type                      | Compile-time | Rejection |
| `E-CTE-0022` | Error    | Non-comptime-available return type                         | Compile-time | Rejection |
| `E-CTE-0023` | Error    | Comptime procedure body contains prohibited construct      | Compile-time | Rejection |
| `E-CTE-0024` | Error    | Generic instantiation produces non-comptime-available type | Compile-time | Rejection |

---

### 16.4 Comptime Blocks and Expressions

##### Definition

A **comptime block** is a block statement prefixed with the `comptime` keyword. A comptime block executes immediately when encountered during the Metaprogramming Phase.

A **comptime expression** is a comptime block used in expression position. The result value of a comptime expression is substituted into the runtime program as a compile-time constant.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_block ::= attribute_list? "comptime" block

comptime_expr  ::= "comptime" block
```

The `block` production is defined in §11.2. The `attribute_list` production is defined in §7.2.

**Examples**

```cursive
// Comptime block at statement level
comptime {
    static_assert(size_of::<Player>() <= 64, "Player must fit in cache line")
}

// Comptime expression
let array_size = comptime { compute_optimal_size() }

// Comptime block with file access attribute
[[files]]
comptime {
    let schema = files~>read("schema.json")?
    generate_types_from_schema(schema)
}
```

##### Static Semantics

**Typing Rule for Comptime Blocks (Statement Position)**

$$\frac{
    \Gamma_{ct} \vdash \textit{body} : T
}{
    \Gamma \vdash \texttt{comptime}\ \{\ \textit{body}\ \} : ()
} \quad \text{(T-Comptime-Block-Stmt)}$$

When a comptime block appears in statement position, its result value is discarded and the block has type `()` in the enclosing runtime context.

**Typing Rule for Comptime Expressions**

$$\frac{
    \Gamma_{ct} \vdash \textit{body} : T \quad
    \text{IsComptimeAvailable}(T) \quad
    \text{IsRuntimeRepresentable}(T)
}{
    \Gamma \vdash \texttt{comptime}\ \{\ \textit{body}\ \} : T
} \quad \text{(T-Comptime-Expr)}$$

When a comptime block appears in expression position, the result type $T$ MUST be both comptime-available and representable as a runtime constant.

**Runtime Representability**

A type $T$ is runtime-representable if a value of type $T$ can be encoded as a literal in the runtime program. The following types are runtime-representable:

1. All primitive types
2. `string`
3. Tuples of runtime-representable types
4. Arrays of runtime-representable types
5. Records with all fields runtime-representable
6. Enums with all variant payloads runtime-representable

Types that are comptime-available but NOT runtime-representable include: `TypePattern`, `TypeRepr`, `Ast<T>@S`, and `Sequence<T>`.

**Scope Rules**

Variables declared inside a comptime block are local to the comptime environment. These bindings:

1. Do NOT persist after the comptime block completes.
2. Do NOT exist in the runtime program.
3. MUST NOT be referenced from runtime code outside the block.

**Attribute Handling**

The `[[files]]` attribute on a comptime block enables the `ProjectFiles` capability (§16.5.3) within that block.

##### Dynamic Semantics

**Execution Timing**

Comptime blocks execute during Translation Phase 2 (Metaprogramming Phase). The execution order of comptime blocks respects the dependency graph defined in §16.7.

**Expression Substitution**

When a comptime block is used as an expression:

1. The block body is evaluated at compile time.
2. The result value is converted to an AST literal node.
3. The literal node replaces the comptime expression in the program AST.

```cursive
let x = comptime { 1 + 1 }
// After comptime execution, equivalent to:
let x = 2
```

**Side Effects**

Comptime blocks may have side effects on the compilation unit through capabilities:

1. Code emission via `ModuleEmitter` (§16.5.2)
2. Diagnostic emission via `ComptimeDiagnostics` (§16.5.4)
3. File reads via `ProjectFiles` (§16.5.3)

These side effects are the only observable effects of comptime execution.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-CTE-0030` | Error    | Comptime expression has non-runtime-representable type | Compile-time | Rejection |
| `E-CTE-0031` | Error    | Comptime block references runtime binding              | Compile-time | Rejection |
| `E-CTE-0032` | Error    | Runtime code references comptime-local binding         | Compile-time | Rejection |

---

### 16.5 Compile-Time Capabilities

##### Definition

**Compile-time capabilities** are witnesses that authorize specific operations during the Metaprogramming Phase. Compile-time capabilities follow the capability model defined in §12 but are available only during compile-time execution.

Each compile-time capability authorizes a specific category of compile-time operations. The capability system ensures that comptime code cannot perform unauthorized operations on the compilation unit.

---

#### 16.5.1 The Introspect Capability

##### Definition

The **Introspect capability** authorizes reflection operations on types. It provides access to type structure, field information, variant information, and layout metadata.

##### Syntax & Declaration

```cursive
capability Introspect {
    /// Returns the fields of a record type as a sequence
    procedure fields<T>(~) -> Sequence<FieldInfo>
        where T: Record
    
    /// Returns the variants of an enum type as a sequence
    procedure variants<T>(~) -> Sequence<VariantInfo>
        where T: Enum
    
    /// Returns the states of a modal type as a sequence
    procedure states<T>(~) -> Sequence<StateInfo>
        where T: Modal
    
    /// Returns layout information for a type
    procedure layout<T>(~) -> LayoutInfo
    
    /// Returns the fully-qualified name of a type
    procedure type_name<T>(~) -> string
    
    /// Returns the module path containing a type
    procedure module_path<T>(~) -> string
    
    /// Returns the attributes attached to a type
    procedure attributes<T>(~) -> Sequence<Attribute>
}
```

> **Note:** The `Record`, `Enum`, and `Modal` constraints are marker forms that restrict the type parameter to the appropriate type kind. These are built-in forms not user-definable.

##### Static Semantics

**Provision**

The `Introspect` capability is implicitly provided to:

1. All comptime blocks
2. All comptime procedures
3. All derive target procedures (§19.3)

The capability witness is available via the identifier `introspect`.

**Type Parameter Constraints**

Operations with type kind constraints MUST be invoked with type arguments satisfying those constraints:

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect} \quad
    \text{IsRecord}(T)
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>fields::<}T\texttt{>()} : \texttt{Sequence}\langle\texttt{FieldInfo}\rangle
} \quad \text{(T-Introspect-Fields)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-CTE-0040` | Error    | `fields` called on non-record type             | Compile-time | Rejection |
| `E-CTE-0041` | Error    | `variants` called on non-enum type             | Compile-time | Rejection |
| `E-CTE-0042` | Error    | `states` called on non-modal type              | Compile-time | Rejection |
| `E-CTE-0043` | Error    | `layout` called on type with incomplete layout | Compile-time | Rejection |

---

#### 16.5.2 The ModuleEmitter Capability

##### Definition

The **ModuleEmitter capability** authorizes emission of generated code into a specific module's scope. Code emission is the mechanism by which comptime execution produces declarations visible to the runtime program.

##### Syntax & Declaration

```cursive
capability ModuleEmitter {
    /// Returns the module path this emitter targets
    procedure target_module(~) -> string
    
    /// Emits a typed AST declaration into the module
    /// Requires unique permission (consumes capability for this emission)
    procedure emit(~!, ast: Ast<()>@Typed)
    
    /// Emits a form implementation for a type
    procedure emit_implement(~!,
        target_type: TypeRepr,
        form: FormRepr,
        body: Ast<()>@Typed)
}
```

The `Ast` modal type is defined in §18.2. The `TypeRepr` and `FormRepr` types are defined in §17.5.

##### Static Semantics

**Provision**

The `ModuleEmitter` capability is provided to:

1. Derive target procedures (§19.3), targeting the module where the derived type is declared
2. Comptime blocks containing `emit` calls, targeting the enclosing module

The capability witness is available via the identifier `emitter`.

**Emission Target**

Each `ModuleEmitter` instance is bound to a specific module. Emitted declarations become visible in that module after the Metaprogramming Phase completes.

**Permission Requirements**

The `emit` and `emit_implement` methods require `unique` permission on the capability witness. This ensures each emission is an explicit, trackable operation.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-CTE-0050` | Error    | `emit` called without `ModuleEmitter` capability        | Compile-time | Rejection |
| `E-CTE-0051` | Error    | `emit` called on `Ast` not in `@Typed` state            | Compile-time | Rejection |
| `E-CTE-0052` | Error    | Emitted declaration conflicts with existing declaration | Compile-time | Rejection |
| `E-CTE-0053` | Error    | Duplicate form implementation emitted                   | Compile-time | Rejection |

---

#### 16.5.3 The ProjectFiles Capability

##### Definition

The **ProjectFiles capability** authorizes read access to files within the project directory during compile-time execution. This capability enables schema-driven code generation and build-time resource embedding.

##### Syntax & Declaration

```cursive
capability ProjectFiles {
    /// Reads a file's contents as a UTF-8 string
    procedure read(~, path: string) -> Result<string, FileError>
    
    /// Reads a file's contents as raw bytes
    procedure read_bytes(~, path: string) -> Result<[u8], FileError>
    
    /// Checks whether a file exists
    procedure exists(~, path: string) -> bool
    
    /// Lists the contents of a directory
    procedure list_dir(~, path: string) -> Result<[string], FileError>
    
    /// Returns the project root directory path
    procedure project_root(~) -> string
}

enum FileError {
    NotFound { path: string },
    PermissionDenied { path: string },
    NotUtf8 { path: string },
    IoError { message: string },
}
```

##### Static Semantics

**Provision**

The `ProjectFiles` capability is provided only to comptime blocks annotated with the `[[files]]` attribute:

```cursive
[[files]]
comptime {
    let content = files~>read("data/schema.json")?
    // ...
}
```

The capability witness is available via the identifier `files`.

**Path Resolution**

All paths are interpreted relative to the project root directory. The project root is the directory containing the project manifest file.

**Path Restrictions**

The following path restrictions apply:

1. Paths MUST be relative (not absolute).
2. Paths MUST NOT contain `..` components that would traverse above the project root.
3. Paths MUST NOT follow symbolic links that escape the project directory.
4. Only regular files and directories are accessible.

**Determinism**

File contents read via `ProjectFiles` are captured at the start of the Metaprogramming Phase. Subsequent modifications to files during compilation do not affect the values returned by `read` operations.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-CTE-0060` | Error    | File operation without `ProjectFiles` capability | Compile-time | Rejection |
| `E-CTE-0061` | Error    | `[[files]]` attribute on non-comptime block      | Compile-time | Rejection |
| `E-CTE-0062` | Error    | Path escapes project directory                   | Compile-time | Rejection |
| `E-CTE-0063` | Error    | Absolute path in file operation                  | Compile-time | Rejection |
| `E-CTE-0064` | Error    | File not found                                   | Compile-time | Rejection |

---

#### 16.5.4 The ComptimeDiagnostics Capability

##### Definition

The **ComptimeDiagnostics capability** authorizes emission of compile-time diagnostics including errors, warnings, and informational messages. This capability enables comptime code to report meaningful error messages when code generation fails or invalid configurations are detected.

##### Syntax & Declaration

```cursive
capability ComptimeDiagnostics {
    /// Emits a compile-time error and halts compilation
    /// The never type (!) indicates this procedure does not return
    procedure error(~!, message: string) -> !
    
    /// Emits a compile-time error at a specific source location
    procedure error_at(~!, message: string, span: SourceSpan) -> !
    
    /// Emits a compile-time warning; compilation continues
    procedure warning(~, message: string)
    
    /// Emits a compile-time warning at a specific source location
    procedure warning_at(~, message: string, span: SourceSpan)
    
    /// Emits an informational note
    procedure note(~, message: string)
    
    /// Emits an informational note at a specific source location
    procedure note_at(~, message: string, span: SourceSpan)
    
    /// Returns the source span of the current comptime block
    procedure current_span(~) -> SourceSpan
    
    /// Returns the module path of the current comptime block
    procedure current_module(~) -> string
}
```

The `SourceSpan` type is defined as:

```cursive
record SourceSpan {
    file: string,
    start_line: u32,
    start_column: u32,
    end_line: u32,
    end_column: u32,
}
```

##### Static Semantics

**Provision**

The `ComptimeDiagnostics` capability is implicitly provided to:

1. All comptime blocks
2. All comptime procedures
3. All derive target procedures (§19.3)

The capability witness is available via the identifier `diagnostics`.

**Never Type**

The `error` and `error_at` procedures have return type `!` (never). These procedures do not return; they terminate the Metaprogramming Phase.

##### Dynamic Semantics

**Error Emission**

When `error` or `error_at` is invoked:

1. The diagnostic message is recorded with severity Error.
2. The Metaprogramming Phase terminates immediately.
3. Compilation fails with diagnostic code `E-CTE-0070`.

**Warning Emission**

When `warning` or `warning_at` is invoked:

1. The diagnostic message is recorded with severity Warning.
2. Execution of the comptime block continues.
3. Compilation proceeds unless other errors occur.

**Note Emission**

When `note` or `note_at` is invoked:

1. The diagnostic message is recorded with severity Note.
2. Notes are attached to the most recent error or warning.
3. If no prior diagnostic exists, the note is emitted standalone.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                  | Detection    | Effect    |
| :----------- | :------- | :------------------------- | :----------- | :-------- |
| `E-CTE-0070` | Error    | Comptime `error` invoked   | Compile-time | Rejection |
| `W-CTE-0071` | Warning  | Comptime `warning` invoked | Compile-time | Continue  |

---

### 16.6 Compile-Time Diagnostic Intrinsics

##### Definition

In addition to the `ComptimeDiagnostics` capability, Cursive provides **compile-time diagnostic intrinsics** as standalone procedures. These intrinsics provide convenient access to diagnostic functionality without explicit capability threading.

##### Syntax & Declaration

```cursive
/// Emits a compile-time error and halts compilation
comptime procedure compile_error(message: string) -> !

/// Emits a compile-time error at a specific source location
comptime procedure compile_error_at(message: string, span: SourceSpan) -> !

/// Emits a compile-time warning
comptime procedure compile_warning(message: string)

/// Emits a compile-time warning at a specific source location
comptime procedure compile_warning_at(message: string, span: SourceSpan)

/// Prints a debug message during compilation
comptime procedure compile_log(message: string)

/// Asserts a condition at compile time
comptime procedure static_assert(condition: bool, message: string)
```

##### Static Semantics

**Typing Rules**

$$\frac{
    \Gamma_{ct} \vdash m : \texttt{string}
}{
    \Gamma_{ct} \vdash \texttt{compile\_error}(m) : !
} \quad \text{(T-Compile-Error)}$$

$$\frac{
    \Gamma_{ct} \vdash c : \texttt{bool} \quad
    \Gamma_{ct} \vdash m : \texttt{string}
}{
    \Gamma_{ct} \vdash \texttt{static\_assert}(c, m) : ()
} \quad \text{(T-Static-Assert)}$$

**Desugaring**

The intrinsics desugar to operations on an implicitly-provided `ComptimeDiagnostics` capability:

| Intrinsic             | Desugaring                        |
| :-------------------- | :-------------------------------- |
| `compile_error(m)`    | `diagnostics~>error(m)`           |
| `compile_warning(m)`  | `diagnostics~>warning(m)`         |
| `static_assert(c, m)` | `if !c { diagnostics~>error(m) }` |

##### Dynamic Semantics

**compile_error**

1. Emits diagnostic `E-CTE-0070` with the provided message.
2. Terminates the Metaprogramming Phase immediately.
3. Compilation fails.

**compile_warning**

1. Emits diagnostic `W-CTE-0071` with the provided message.
2. Compilation continues.

**compile_log**

1. Writes the message to the compiler's diagnostic output stream (stderr or equivalent).
2. Has no effect on compilation success.
3. Output format is Implementation-Defined.

**static_assert**

1. Evaluates `condition` at compile time.
2. If `condition` is `true`, has no effect.
3. If `condition` is `false`, behaves as `compile_error(message)`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-CTE-0080` | Error    | `static_assert` condition is false | Compile-time | Rejection |

---

### 16.7 Dependency Resolution and Execution Order

##### Definition

**Dependency resolution** determines the order in which comptime blocks and derive invocations execute. The implementation constructs a dependency graph to ensure that declarations are available when referenced.

**Formal Definition**

A **comptime dependency graph** is a directed graph $G = (V, E)$ where:

- $V$ is the set of all comptime blocks, comptime procedures, and derive invocations in the compilation unit
- $(A, B) \in E$ if and only if $A$ references a declaration that is emitted by $B$

##### Static Semantics

**Dependency Detection**

An edge $(A, B) \in E$ exists when:

1. Comptime block $A$ calls a comptime procedure $B$.
2. Comptime block $A$ references a type that is emitted by comptime block $B$.
3. Comptime block $A$ references a procedure that is emitted by comptime block $B$.
4. Derive invocation $A$ depends on a form implementation emitted by $B$.

**Cycle Detection**

If the dependency graph $G$ contains a cycle, the program is ill-formed:

$$\frac{
    \exists\ v_1, v_2, \ldots, v_n \in V.\ (v_1, v_2), (v_2, v_3), \ldots, (v_{n-1}, v_n), (v_n, v_1) \in E
}{
    \text{ill-formed: cyclic dependency}
} \quad \text{(Comptime-Cycle)}$$

The implementation MUST report `E-CTE-0090` and identify at least one edge in the cycle.

**Topological Order**

The implementation MUST execute comptime blocks in a topological order of the dependency graph. If multiple valid topological orders exist, the implementation MAY choose any valid order.

**Determinism Requirement**

For reproducible builds, the implementation MUST choose the same topological order given identical source input. The specific order is Implementation-Defined but MUST be deterministic.

##### Dynamic Semantics

**Execution Order**

Comptime blocks execute according to the following algorithm:

1. Construct the dependency graph $G$ for all comptime blocks in the compilation unit.
2. Verify $G$ is acyclic; if not, emit `E-CTE-0090` and terminate.
3. Compute a topological ordering $[v_1, v_2, \ldots, v_n]$ of $G$.
4. For each $v_i$ in order:
   a. Execute the comptime block or derive invocation.
   b. Process any emitted declarations.
   c. If execution panics, emit `E-CTE-0091` and terminate.

**Panic Behavior**

If a comptime block or procedure panics during execution:

1. The panic message is captured.
2. The source location of the panicking expression is recorded.
3. Diagnostic `E-CTE-0091` is emitted with the panic message.
4. The Metaprogramming Phase terminates.
5. Compilation fails.

**Partial Execution**

If a comptime block fails, other comptime blocks that do not depend on the failed block MAY still execute to provide additional diagnostics. This behavior is Implementation-Defined.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CTE-0090` | Error    | Cyclic dependency in comptime execution               | Compile-time | Rejection |
| `E-CTE-0091` | Error    | Comptime execution panicked                           | Compile-time | Rejection |
| `E-CTE-0092` | Error    | Comptime block references declaration not yet emitted | Compile-time | Rejection |

---

### 16.8 Implementation Limits

##### Definition

Conforming implementations MUST enforce resource limits on compile-time execution to guarantee termination. The limits specified in this section are **minimum guaranteed capacities** that all conforming implementations MUST support. Implementations MAY support higher limits.

##### Static Semantics

**Resource Limit Table**

| Resource                   | Minimum Limit        | Diagnostic on Exceed |
| :------------------------- | :------------------- | :------------------- |
| Comptime recursion depth   | 256 stack frames     | `E-CTE-0100`         |
| Comptime evaluation steps  | 10,000,000 steps     | `E-CTE-0101`         |
| Comptime memory allocation | 256 MiB              | `E-CTE-0102`         |
| Comptime string length     | 16 MiB               | `E-CTE-0103`         |
| Comptime sequence length   | 1,000,000 elements   | `E-CTE-0104`         |
| Total emitted declarations | 100,000 declarations | `E-CTE-0105`         |

**Evaluation Step**

An **evaluation step** is one of the following operations:

1. Evaluation of a single expression node
2. Execution of a single statement
3. One iteration of a loop body
4. One procedure call (entering or returning)

**Memory Accounting**

Comptime memory allocation includes:

1. Local variables in comptime blocks and procedures
2. Heap allocations for strings and sequences
3. AST nodes constructed via quoting (§18)
4. Introspection data structures

**Limit Configuration**

Implementations MAY provide command-line options or configuration to adjust resource limits above the minimums. Such configuration is Implementation-Defined.

##### Dynamic Semantics

**Limit Enforcement**

The implementation MUST track resource usage during comptime execution. When any limit is exceeded:

1. The appropriate diagnostic is emitted.
2. Comptime execution terminates immediately.
3. Compilation fails.

**Caching**

Implementations SHOULD cache the results of comptime blocks and procedures annotated with the `[[cache]]` attribute to avoid redundant computation:

```cursive
[[cache]]
comptime procedure expensive_computation(): [TypeInfo] {
    // Result is cached across compilation units
}
```

**Cache Key**

When caching is implemented, the cache key MUST include:

1. The complete AST of the comptime block or procedure
2. The values of all captured bindings
3. The compiler version identifier
4. All configuration that affects comptime execution

**Cache Invalidation**

Cached results MUST be invalidated when any component of the cache key changes.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-CTE-0100` | Error    | Comptime recursion depth exceeded        | Compile-time | Rejection |
| `E-CTE-0101` | Error    | Comptime evaluation step limit exceeded  | Compile-time | Rejection |
| `E-CTE-0102` | Error    | Comptime memory limit exceeded           | Compile-time | Rejection |
| `E-CTE-0103` | Error    | Comptime string length limit exceeded    | Compile-time | Rejection |
| `E-CTE-0104` | Error    | Comptime sequence length limit exceeded  | Compile-time | Rejection |
| `E-CTE-0105` | Error    | Emitted declaration count limit exceeded | Compile-time | Rejection |

---

### 16.9 Summary of Diagnostic Codes

This section provides a consolidated reference of all diagnostic codes defined in this clause.

| Code         | Severity | Section | Condition                                                        |
| :----------- | :------- | :------ | :--------------------------------------------------------------- |
| `E-CTE-0001` | Error    | §16.1   | `unsafe` block in comptime context                               |
| `E-CTE-0002` | Error    | §16.1   | FFI call in comptime context                                     |
| `E-CTE-0003` | Error    | §16.1   | Runtime capability access in comptime context                    |
| `E-CTE-0004` | Error    | §16.1   | Non-deterministic operation in comptime context                  |
| `E-CTE-0005` | Error    | §16.1   | Prohibited I/O operation in comptime context                     |
| `E-CTE-0010` | Error    | §16.2   | Non-comptime-available type used in comptime context             |
| `E-CTE-0011` | Error    | §16.2   | Parameter type of comptime procedure not comptime-available      |
| `E-CTE-0012` | Error    | §16.2   | Return type of comptime procedure not comptime-available         |
| `E-CTE-0013` | Error    | §16.2   | Local variable in comptime block has non-comptime-available type |
| `E-CTE-0020` | Error    | §16.3   | Comptime procedure called from runtime context                   |
| `E-CTE-0021` | Error    | §16.3   | Non-comptime-available parameter type                            |
| `E-CTE-0022` | Error    | §16.3   | Non-comptime-available return type                               |
| `E-CTE-0023` | Error    | §16.3   | Comptime procedure body contains prohibited construct            |
| `E-CTE-0024` | Error    | §16.3   | Generic instantiation produces non-comptime-available type       |
| `E-CTE-0030` | Error    | §16.4   | Comptime expression has non-runtime-representable type           |
| `E-CTE-0031` | Error    | §16.4   | Comptime block references runtime binding                        |
| `E-CTE-0032` | Error    | §16.4   | Runtime code references comptime-local binding                   |
| `E-CTE-0040` | Error    | §16.5.1 | `fields` called on non-record type                               |
| `E-CTE-0041` | Error    | §16.5.1 | `variants` called on non-enum type                               |
| `E-CTE-0042` | Error    | §16.5.1 | `states` called on non-modal type                                |
| `E-CTE-0043` | Error    | §16.5.1 | `layout` called on type with incomplete layout                   |
| `E-CTE-0050` | Error    | §16.5.2 | `emit` called without `ModuleEmitter` capability                 |
| `E-CTE-0051` | Error    | §16.5.2 | `emit` called on `Ast` not in `@Typed` state                     |
| `E-CTE-0052` | Error    | §16.5.2 | Emitted declaration conflicts with existing declaration          |
| `E-CTE-0053` | Error    | §16.5.2 | Duplicate form implementation emitted                            |
| `E-CTE-0060` | Error    | §16.5.3 | File operation without `ProjectFiles` capability                 |
| `E-CTE-0061` | Error    | §16.5.3 | `[[files]]` attribute on non-comptime block                      |
| `E-CTE-0062` | Error    | §16.5.3 | Path escapes project directory                                   |
| `E-CTE-0063` | Error    | §16.5.3 | Absolute path in file operation                                  |
| `E-CTE-0064` | Error    | §16.5.3 | File not found                                                   |
| `E-CTE-0070` | Error    | §16.5.4 | Comptime `error` invoked                                         |
| `W-CTE-0071` | Warning  | §16.5.4 | Comptime `warning` invoked                                       |
| `E-CTE-0080` | Error    | §16.6   | `static_assert` condition is false                               |
| `E-CTE-0090` | Error    | §16.7   | Cyclic dependency in comptime execution                          |
| `E-CTE-0091` | Error    | §16.7   | Comptime execution panicked                                      |
| `E-CTE-0092` | Error    | §16.7   | Comptime block references declaration not yet emitted            |
| `E-CTE-0100` | Error    | §16.8   | Comptime recursion depth exceeded                                |
| `E-CTE-0101` | Error    | §16.8   | Comptime evaluation step limit exceeded                          |
| `E-CTE-0102` | Error    | §16.8   | Comptime memory limit exceeded                                   |
| `E-CTE-0103` | Error    | §16.8   | Comptime string length limit exceeded                            |
| `E-CTE-0104` | Error    | §16.8   | Comptime sequence length limit exceeded                          |
| `E-CTE-0105` | Error    | §16.8   | Emitted declaration count limit exceeded                         |

---

## Clause 17: Type Reflection

This clause defines the type reflection system for Cursive. Type reflection enables compile-time inspection of type structure, providing access to fields, variants, states, layout information, and metadata. Type reflection operates within the comptime environment defined in §16.1 and provides the foundation for code generation (§18) and derivation (§19).

---

### 17.1 Overview

##### Definition

**Type reflection** is the compile-time inspection of type structure and metadata. Type reflection enables comptime code to examine the constituents of types—fields of records, variants of enums, states of modal types—and to generate code based on that structure.

Cursive provides two complementary reflection mechanisms:

1. **Type patterns** — A pattern-matching syntax that deconstructs types based on their structure, binding type variables and structural information.

2. **Introspection operations** — Procedures on the `Introspect` capability (§16.5.1) that return sequences of metadata records describing type constituents.

Type patterns are declarative and integrate with Cursive's pattern matching syntax. Introspection operations are procedural and return data structures suitable for iteration and transformation.

##### Static Semantics

**Context Restriction**

All type reflection operations MUST occur within comptime context as defined in §16.1. Type patterns and introspection operations are not available in runtime code.

**Capability Requirement**

Introspection operations require the `Introspect` capability witness. Type patterns do not require an explicit capability; they are a language construct available in any comptime context.

**Relationship to Other Clauses**

| Clause                       | Relationship                                             |
| :--------------------------- | :------------------------------------------------------- |
| §16 (Compile-Time Execution) | Type reflection operates within the comptime environment |
| §18 (Code Generation)        | Code generation uses reflection to construct typed AST   |
| §19 (Derivation)             | Derive targets use reflection to inspect target types    |

---

### 17.2 Type Patterns

##### Definition

A **type pattern** is a pattern form that matches against type structure at compile time. Type patterns deconstruct types into their constituent parts, binding type variables and structural metadata for use in comptime code.

Type patterns extend the pattern matching system defined in §11.5 to operate on types rather than values.

##### Syntax & Declaration

**Grammar**

```ebnf
type_match_expr        ::= "match" "type" type_expr "{" type_match_arm+ "}"

type_match_arm         ::= type_pattern ("," type_pattern)* "=>" expression ","?

type_pattern           ::= wildcard_type_pattern
                         | binding_type_pattern
                         | record_type_pattern
                         | enum_type_pattern
                         | modal_type_pattern
                         | array_type_pattern
                         | slice_type_pattern
                         | tuple_type_pattern
                         | generic_type_pattern
                         | permission_type_pattern
                         | primitive_type_pattern
                         | named_type_pattern

wildcard_type_pattern  ::= "type" "_"

binding_type_pattern   ::= "type" IDENTIFIER

record_type_pattern    ::= "Record" "{" field_pattern_list? "}"

field_pattern_list     ::= ".."
                         | field_pattern ("," field_pattern)* ("," "..")?

field_pattern          ::= IDENTIFIER ":" type_pattern
                         | IDENTIFIER

enum_type_pattern      ::= "Enum" "{" variant_pattern_list? "}"

variant_pattern_list   ::= ".."
                         | variant_pattern ("," variant_pattern)* ("," "..")?

variant_pattern        ::= IDENTIFIER
                         | IDENTIFIER "(" type_pattern ")"

modal_type_pattern     ::= "Modal" "{" state_pattern_list? "}"

state_pattern_list     ::= ".."
                         | state_pattern ("," state_pattern)* ("," "..")?

state_pattern          ::= "@" IDENTIFIER
                         | "@" IDENTIFIER "{" field_pattern_list? "}"

array_type_pattern     ::= "[" type_pattern ";" IDENTIFIER "]"
                         | "[" type_pattern ";" INTEGER_LITERAL "]"

slice_type_pattern     ::= "[" type_pattern "]"

tuple_type_pattern     ::= "(" type_pattern "," type_pattern_list ")"

type_pattern_list      ::= type_pattern ("," type_pattern)*

generic_type_pattern   ::= IDENTIFIER "<" type_pattern_list ">"

permission_type_pattern ::= permission_qualifier type_pattern

permission_qualifier   ::= "unique" | "shared" | "const"

primitive_type_pattern ::= "Primitive"

named_type_pattern     ::= type_path
```

The `type_expr` and `type_path` productions are defined in §4.

**Pattern Categories**

| Pattern Form               | Matches                | Bindings Produced                                     |
| :------------------------- | :--------------------- | :---------------------------------------------------- |
| `type _`                   | Any type               | None                                                  |
| `type T`                   | Any type               | `T` bound to matched type                             |
| `Record { .. }`            | Any record type        | None                                                  |
| `Record { f: type F, .. }` | Record with field `f`  | `F` bound to field type, `f` bound to `FieldInfo`     |
| `Enum { .. }`              | Any enum type          | None                                                  |
| `Enum { V(type P) }`       | Enum with variant `V`  | `P` bound to payload type, `V` bound to `VariantInfo` |
| `Modal { .. }`             | Any modal type         | None                                                  |
| `Modal { @S { .. } }`      | Modal with state `S`   | `S` bound to `StateInfo`                              |
| `[type E; n]`              | Array type             | `E` bound to element type, `n` bound to length        |
| `[type E]`                 | Slice type             | `E` bound to element type                             |
| `(type A, type B)`         | Tuple type             | `A`, `B` bound to component types                     |
| `Name<type A>`             | Generic instantiation  | `A` bound to type argument                            |
| `unique type T`            | Unique permission type | `T` bound to inner type                               |
| `Primitive`                | Any primitive type     | None                                                  |
| `TypeName`                 | Specific named type    | None                                                  |

##### Static Semantics

**Typing Rule for Type Match**

$$\frac{
    \Gamma_{ct} \vdash T : \texttt{Type} \quad
    \forall i.\ \text{PatternWF}(P_i) \quad
    \forall i.\ \text{Matches}(T, P_i) \implies \Gamma_{ct}, \sigma_i \vdash e_i : R \quad
    \text{Exhaustive}(\{P_1, \ldots, P_n\}, \mathcal{K})
}{
    \Gamma_{ct} \vdash \texttt{match type } T\ \{\ P_1 \Rightarrow e_1, \ldots, P_n \Rightarrow e_n\ \} : R
} \quad \text{(T-Type-Match)}$$

Where:
- $\sigma_i$ is the binding environment produced by matching $T$ against $P_i$
- $\mathcal{K}$ is the set of all type kinds
- $\text{Exhaustive}(P, \mathcal{K})$ holds when the patterns cover all type kinds

**Context Restriction**

Type match expressions MUST appear within comptime context:

$$\frac{
    \texttt{match type } T\ \{ \ldots \} \text{ outside comptime context}
}{
    \text{ill-formed}
} \quad \text{(Type-Match-Context)}$$

**Result Type Consistency**

All arms of a type match expression MUST produce values of the same type:

$$\frac{
    \Gamma_{ct}, \sigma_i \vdash e_i : R_i \quad
    \exists j.\ R_j \not\equiv R_1
}{
    \text{ill-formed}
} \quad \text{(Type-Match-Consistent)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-REF-0001` | Error    | Type match expression outside comptime context | Compile-time | Rejection |
| `E-REF-0002` | Error    | Non-exhaustive type pattern match              | Compile-time | Rejection |
| `E-REF-0003` | Error    | Type match arms have inconsistent result types | Compile-time | Rejection |
| `E-REF-0004` | Error    | Invalid type pattern syntax                    | Compile-time | Rejection |

---

### 17.3 Type Pattern Matching

##### Definition

**Type pattern matching** is the process of testing whether a type matches a type pattern and, if successful, producing bindings for pattern variables. Type pattern matching occurs at compile time during evaluation of `match type` expressions.

##### Static Semantics

**Matching Judgment**

The judgment $\text{Matches}(T, P, \sigma)$ holds when type $T$ matches pattern $P$, producing binding environment $\sigma$.

**Wildcard Pattern**

$$\frac{}{
    \text{Matches}(T, \texttt{type \_}, \emptyset)
} \quad \text{(M-Wildcard)}$$

**Binding Pattern**

$$\frac{}{
    \text{Matches}(T, \texttt{type } X, \{X \mapsto T\})
} \quad \text{(M-Binding)}$$

**Record Pattern (Structural)**

$$\frac{
    \text{IsRecord}(T) \quad
    \text{fields}(T) = \{f_1: T_1, \ldots, f_n: T_n\} \quad
    \forall (f_i: P_i) \in \text{FieldPatterns}.\ \exists j.\ f_i = f_j \land \text{Matches}(T_j, P_i, \sigma_i)
}{
    \text{Matches}(T, \texttt{Record \{} \text{FieldPatterns}, \texttt{..\}}, \bigcup_i \sigma_i \cup \{f_i \mapsto \text{FieldInfo}(f_i, T)\})
} \quad \text{(M-Record)}$$

**Enum Pattern (Structural)**

$$\frac{
    \text{IsEnum}(T) \quad
    \text{variants}(T) = \{V_1(P_1), \ldots, V_n(P_n)\} \quad
    \forall V_i(Q_i) \in \text{VariantPatterns}.\ \exists j.\ V_i = V_j \land \text{Matches}(P_j, Q_i, \sigma_i)
}{
    \text{Matches}(T, \texttt{Enum \{} \text{VariantPatterns}, \texttt{..\}}, \bigcup_i \sigma_i \cup \{V_i \mapsto \text{VariantInfo}(V_i, T)\})
} \quad \text{(M-Enum)}$$

**Modal Pattern (Structural)**

$$\frac{
    \text{IsModal}(T) \quad
    \text{states}(T) = \{@S_1\{\ldots\}, \ldots, @S_n\{\ldots\}\} \quad
    \forall @S_i \in \text{StatePatterns}.\ \exists j.\ S_i = S_j
}{
    \text{Matches}(T, \texttt{Modal \{} \text{StatePatterns}, \texttt{..\}}, \{S_i \mapsto \text{StateInfo}(S_i, T)\})
} \quad \text{(M-Modal)}$$

**Array Pattern**

$$\frac{
    T = [E; N]
}{
    \text{Matches}(T, \texttt{[type } X\texttt{; } n\texttt{]}, \{X \mapsto E, n \mapsto N\})
} \quad \text{(M-Array)}$$

**Slice Pattern**

$$\frac{
    T = [E]
}{
    \text{Matches}(T, \texttt{[type } X\texttt{]}, \{X \mapsto E\})
} \quad \text{(M-Slice)}$$

**Tuple Pattern**

$$\frac{
    T = (T_1, \ldots, T_n) \quad
    \forall i.\ \text{Matches}(T_i, P_i, \sigma_i)
}{
    \text{Matches}(T, \texttt{(}P_1\texttt{, } \ldots\texttt{, } P_n\texttt{)}, \bigcup_i \sigma_i)
} \quad \text{(M-Tuple)}$$

**Generic Pattern**

$$\frac{
    T = G\langle A_1, \ldots, A_n \rangle \quad
    \text{name}(G) = \text{name}(G') \quad
    \forall i.\ \text{Matches}(A_i, P_i, \sigma_i)
}{
    \text{Matches}(T, G'\texttt{<}P_1\texttt{, } \ldots\texttt{, } P_n\texttt{>}, \bigcup_i \sigma_i)
} \quad \text{(M-Generic)}$$

**Permission Pattern**

$$\frac{
    T = \pi\ U \quad
    \text{Matches}(U, P, \sigma)
}{
    \text{Matches}(T, \pi\ P, \sigma)
} \quad \text{(M-Permission)}$$

where $\pi \in \{\texttt{unique}, \texttt{shared}, \texttt{const}\}$.

**Primitive Pattern**

$$\frac{
    T \in \{i8, i16, i32, i64, i128, u8, u16, u32, u64, u128, isize, usize, f32, f64, bool, char\}
}{
    \text{Matches}(T, \texttt{Primitive}, \emptyset)
} \quad \text{(M-Primitive)}$$

**Named Type Pattern**

$$\frac{
    T \equiv U
}{
    \text{Matches}(T, U, \emptyset)
} \quad \text{(M-Named)}$$

**Binding Scope**

Bindings produced by pattern matching are available in the corresponding match arm body. Bindings shadow any existing bindings with the same name in the enclosing scope.

**Binding Types**

| Binding Source                | Binding Type  |
| :---------------------------- | :------------ |
| `type X` pattern              | `TypeRepr`    |
| Field name in record pattern  | `FieldInfo`   |
| Variant name in enum pattern  | `VariantInfo` |
| State name in modal pattern   | `StateInfo`   |
| Array length in array pattern | `usize`       |

##### Dynamic Semantics

**Pattern Matching Algorithm**

Pattern matching at compile time proceeds as follows:

1. Let $T$ be the scrutinee type of the `match type` expression.
2. For each arm $(P_i \Rightarrow e_i)$ in order:
   a. Attempt to match $T$ against $P_i$.
   b. If matching succeeds with bindings $\sigma_i$:
      - Evaluate $e_i$ in environment $\Gamma_{ct} \cup \sigma_i$.
      - Return the result.
3. If no pattern matches, the program is ill-formed (non-exhaustive match).

**Exhaustiveness Checking**

The implementation MUST verify that type patterns are exhaustive. A set of patterns is exhaustive if, for every possible type, at least one pattern matches.

The following pattern sets are exhaustive:

1. Any set containing `type _` or `type X` (for any identifier $X$).
2. A set covering all type kinds: `Record { .. }`, `Enum { .. }`, `Modal { .. }`, `Primitive`, tuple patterns, array patterns, slice patterns, and named type patterns for all remaining types.

##### Constraints & Legality

**Shadowing**

Type pattern bindings MAY shadow existing bindings. The implementation SHOULD emit a warning when shadowing occurs.

**Duplicate Bindings**

A single pattern MUST NOT bind the same identifier twice:

$$\frac{
    X \in \text{Bindings}(P) \quad
    |\{b \in \text{Bindings}(P) \mid \text{name}(b) = X\}| > 1
}{
    \text{ill-formed}
} \quad \text{(No-Duplicate-Bindings)}$$

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-REF-0010` | Error    | Duplicate binding in type pattern             | Compile-time | Rejection |
| `E-REF-0011` | Error    | Pattern requires field not present in type    | Compile-time | Rejection |
| `E-REF-0012` | Error    | Pattern requires variant not present in type  | Compile-time | Rejection |
| `E-REF-0013` | Error    | Pattern requires state not present in type    | Compile-time | Rejection |
| `E-REF-0014` | Error    | Tuple pattern arity mismatch                  | Compile-time | Rejection |
| `E-REF-0015` | Error    | Generic pattern arity mismatch                | Compile-time | Rejection |
| `W-REF-0010` | Warning  | Type pattern binding shadows existing binding | Compile-time | Continue  |

---

### 17.4 Introspection Types

##### Definition

**Introspection types** are record and enum types that represent metadata about type structure. Introspection types are comptime-available (§16.2) and are returned by introspection operations (§17.6).

---

#### 17.4.1 FieldInfo

##### Definition

The `FieldInfo` record represents metadata about a single field of a record type.

##### Syntax & Declaration

```cursive
record FieldInfo {
    /// The name of the field
    name: string,
    
    /// The type of the field as a TypeRepr handle
    field_type: TypeRepr,
    
    /// The byte offset of the field within the record
    offset: usize,
    
    /// Attributes attached to the field
    attributes: [Attribute],
    
    /// The visibility of the field
    visibility: Visibility,
    
    /// Whether the field has a default value
    has_default: bool,
}
```

##### Static Semantics

**Field Ordering**

The sequence of `FieldInfo` values returned by `introspect~>fields::<T>()` is in declaration order.

**Offset Validity**

The `offset` field contains the byte offset from the start of the record to the start of the field. For records with `[[layout(C)]]`, offsets follow C struct layout rules. For records without explicit layout, offsets are Implementation-Defined.

---

#### 17.4.2 VariantInfo

##### Definition

The `VariantInfo` record represents metadata about a single variant of an enum type.

##### Syntax & Declaration

```cursive
record VariantInfo {
    /// The name of the variant
    name: string,
    
    /// The discriminant value for this variant
    discriminant: i128,
    
    /// The payload fields of the variant (empty for unit variants)
    payload: [FieldInfo],
    
    /// Attributes attached to the variant
    attributes: [Attribute],
}
```

##### Static Semantics

**Variant Ordering**

The sequence of `VariantInfo` values returned by `introspect~>variants::<T>()` is in declaration order.

**Discriminant Values**

The `discriminant` field contains the discriminant value assigned to this variant. Discriminant assignment follows the rules in §5.6.

**Unit Variants**

For unit variants (variants with no payload), the `payload` field is an empty array.

---

#### 17.4.3 StateInfo

##### Definition

The `StateInfo` record represents metadata about a single state of a modal type.

##### Syntax & Declaration

```cursive
record StateInfo {
    /// The name of the state (without the @ prefix)
    name: string,
    
    /// The payload fields of the state
    payload: [FieldInfo],
    
    /// Transitions available from this state
    transitions: [TransitionInfo],
    
    /// Attributes attached to the state
    attributes: [Attribute],
}
```

##### Static Semantics

**State Ordering**

The sequence of `StateInfo` values returned by `introspect~>states::<T>()` is in declaration order.

---

#### 17.4.4 TransitionInfo

##### Definition

The `TransitionInfo` record represents metadata about a transition method declared within a modal state.

##### Syntax & Declaration

```cursive
record TransitionInfo {
    /// The name of the transition method
    method_name: string,
    
    /// Parameters of the transition (excluding self)
    params: [ParamInfo],
    
    /// Names of possible target states
    target_states: [string],
    
    /// Whether this transition consumes the value
    is_consuming: bool,
}
```

##### Static Semantics

**Target States**

The `target_states` field contains the names of all states that this transition may produce. For deterministic transitions, this array contains exactly one element. For non-deterministic transitions (using `|` in the return type), this array contains all possible target states.

---

#### 17.4.5 ParamInfo

##### Definition

The `ParamInfo` record represents metadata about a parameter of a procedure or transition.

##### Syntax & Declaration

```cursive
record ParamInfo {
    /// The name of the parameter
    name: string,
    
    /// The type of the parameter
    param_type: TypeRepr,
    
    /// The permission qualifier on the parameter
    permission: Permission,
    
    /// Whether this parameter is a self parameter
    is_self: bool,
    
    /// Whether this parameter has a default value
    has_default: bool,
}
```

---

#### 17.4.6 LayoutInfo

##### Definition

The `LayoutInfo` record represents memory layout information for a type.

##### Syntax & Declaration

```cursive
record LayoutInfo {
    /// The size of the type in bytes
    size: usize,
    
    /// The alignment requirement of the type in bytes
    alignment: usize,
    
    /// The stride of the type in bytes (size rounded up to alignment)
    stride: usize,
}
```

##### Static Semantics

**Layout Invariants**

The following invariants hold for all `LayoutInfo` values:

1. `alignment` is a power of two.
2. `size <= stride`.
3. `stride` is a multiple of `alignment`.
4. `stride` is the smallest multiple of `alignment` that is at least `size`.

**Incomplete Types**

Types with incomplete layout (such as recursive types without indirection) cannot be reflected. Attempting to obtain `LayoutInfo` for such types results in `E-CTE-0043`.

---

#### 17.4.7 Attribute

##### Definition

The `Attribute` record represents a single attribute attached to a declaration or field.

##### Syntax & Declaration

```cursive
record Attribute {
    /// The name of the attribute (without brackets)
    name: string,
    
    /// The arguments to the attribute
    arguments: [AttributeArg],
    
    /// The source location of the attribute
    span: SourceSpan,
}

enum AttributeArg {
    /// A bare identifier argument
    Identifier(string),
    
    /// A literal value argument
    Literal(LiteralValue),
    
    /// A key-value pair argument
    KeyValue { key: string, value: AttributeArg },
    
    /// A nested list of arguments
    List([AttributeArg]),
}

enum LiteralValue {
    String(string),
    Integer(i128),
    Float(f64),
    Bool(bool),
    Char(char),
}
```

---

#### 17.4.8 Supporting Enums

##### Definition

The following enum types support the introspection system.

##### Syntax & Declaration

**Visibility**

```cursive
enum Visibility {
    /// Visible everywhere
    Public,
    
    /// Visible within the same package
    Internal,
    
    /// Visible only within the defining module
    Private,
}
```

**Permission**

```cursive
enum Permission {
    /// Immutable shared access
    Const,
    
    /// Exclusive mutable access
    Unique,
    
    /// Shared mutable access (requires key system)
    Shared,
}
```

> **Note:** The `Permission` enum represents permission qualifiers as defined in §4.5.

##### Static Semantics

All introspection types defined in this section are comptime-available per §16.2. They satisfy the predicate $\text{IsComptimeAvailable}(T)$ by rules (C-Record) and (C-Enum).

---

### 17.5 Type Representation

##### Definition

A **type representation** is an opaque handle representing a resolved type within the compiler's type system. Type representations are used to splice types into generated code and to compare type identity.

---

#### 17.5.1 TypeRepr

##### Definition

The `TypeRepr` type is an opaque handle representing a fully resolved type.

##### Syntax & Declaration

```cursive
record TypeRepr {
    // Internal representation is opaque
}
```

The internal structure of `TypeRepr` is not accessible to comptime code. `TypeRepr` values may only be:

1. Obtained via introspection operations
2. Passed to code generation operations
3. Compared for equality
4. Used in splice expressions within quotes

##### Static Semantics

**Equality**

Two `TypeRepr` values are equal if and only if they represent the same type:

$$\frac{
    r_1 : \texttt{TypeRepr} \quad r_2 : \texttt{TypeRepr} \quad
    \text{type}(r_1) \equiv \text{type}(r_2)
}{
    r_1 = r_2
} \quad \text{(TypeRepr-Eq)}$$

**Obtaining TypeRepr**

`TypeRepr` values are obtained via:

1. The `type_repr::<T>()` intrinsic, which returns the representation of type `T`.
2. The `field_type` field of `FieldInfo`.
3. The `param_type` field of `ParamInfo`.
4. Bindings from type patterns (bound as `TypeRepr`).

**Intrinsic Signature**

```cursive
comptime procedure type_repr<T>(): TypeRepr
```

##### Constraints & Legality

**Opacity**

Field access and pattern matching on `TypeRepr` are prohibited:

$$\frac{
    e : \texttt{TypeRepr} \quad
    e.f \text{ or } \texttt{match } e \{ \ldots \}
}{
    \text{ill-formed}
} \quad \text{(TypeRepr-Opaque)}$$

**Diagnostic Table**

| Code         | Severity | Condition                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------- | :----------- | :-------- |
| `E-REF-0020` | Error    | Field access on `TypeRepr`  | Compile-time | Rejection |
| `E-REF-0021` | Error    | Pattern match on `TypeRepr` | Compile-time | Rejection |

---

#### 17.5.2 FormRepr

##### Definition

The `FormRepr` type is an opaque handle representing a form (trait).

##### Syntax & Declaration

```cursive
record FormRepr {
    // Internal representation is opaque
}
```

##### Static Semantics

**Obtaining FormRepr**

`FormRepr` values are obtained via:

1. The `form_repr::<F>()` intrinsic, which returns the representation of form `F`.

**Intrinsic Signature**

```cursive
comptime procedure form_repr<F>(): FormRepr
    where F: Form
```

The type parameter `F` MUST be a form type.

**Equality**

Two `FormRepr` values are equal if and only if they represent the same form.

##### Constraints & Legality

**Opacity**

Field access and pattern matching on `FormRepr` are prohibited, following the same rules as `TypeRepr`.

**Diagnostic Table**

| Code         | Severity | Condition                           | Detection    | Effect    |
| :----------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-REF-0022` | Error    | `form_repr` called on non-form type | Compile-time | Rejection |
| `E-REF-0023` | Error    | Field access on `FormRepr`          | Compile-time | Rejection |
| `E-REF-0024` | Error    | Pattern match on `FormRepr`         | Compile-time | Rejection |

---

### 17.6 Introspection Operations

##### Definition

**Introspection operations** are procedures on the `Introspect` capability that return metadata about types. Introspection operations complement type patterns by providing programmatic access to type structure.

The `Introspect` capability is defined in §16.5.1.

---

#### 17.6.1 Field Introspection

##### Definition

The `fields` operation returns the fields of a record type as a sequence.

##### Syntax & Declaration

```cursive
procedure fields<T>(~) -> Sequence<FieldInfo>
    where T: Record
```

##### Static Semantics

**Type Constraint**

The type parameter `T` MUST be a record type. Invoking `fields` on a non-record type is ill-formed.

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect} \quad
    \text{IsRecord}(T)
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>fields::<}T\texttt{>()} : \texttt{Sequence}\langle\texttt{FieldInfo}\rangle
} \quad \text{(T-Fields)}$$

##### Dynamic Semantics

The returned sequence contains one `FieldInfo` for each field of `T`, in declaration order. The sequence includes:

1. All declared fields of `T`.
2. Fields inherited from embedded records (if any), in embedding order.

Private fields are included in the result regardless of the visibility relationship between the calling code and the record definition.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-REF-0030` | Error    | `fields` called on non-record type | Compile-time | Rejection |

---

#### 17.6.2 Variant Introspection

##### Definition

The `variants` operation returns the variants of an enum type as a sequence.

##### Syntax & Declaration

```cursive
procedure variants<T>(~) -> Sequence<VariantInfo>
    where T: Enum
```

##### Static Semantics

**Type Constraint**

The type parameter `T` MUST be an enum type. Invoking `variants` on a non-enum type is ill-formed.

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect} \quad
    \text{IsEnum}(T)
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>variants::<}T\texttt{>()} : \texttt{Sequence}\langle\texttt{VariantInfo}\rangle
} \quad \text{(T-Variants)}$$

##### Dynamic Semantics

The returned sequence contains one `VariantInfo` for each variant of `T`, in declaration order.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-REF-0031` | Error    | `variants` called on non-enum type | Compile-time | Rejection |

---

#### 17.6.3 State Introspection

##### Definition

The `states` operation returns the states of a modal type as a sequence.

##### Syntax & Declaration

```cursive
procedure states<T>(~) -> Sequence<StateInfo>
    where T: Modal
```

##### Static Semantics

**Type Constraint**

The type parameter `T` MUST be a modal type as defined in §6.1. Invoking `states` on a non-modal type is ill-formed.

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect} \quad
    \text{IsModal}(T)
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>states::<}T\texttt{>()} : \texttt{Sequence}\langle\texttt{StateInfo}\rangle
} \quad \text{(T-States)}$$

##### Dynamic Semantics

The returned sequence contains one `StateInfo` for each state of `T`, in declaration order. Each `StateInfo` includes the transitions available from that state.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                         | Detection    | Effect    |
| :----------- | :------- | :-------------------------------- | :----------- | :-------- |
| `E-REF-0032` | Error    | `states` called on non-modal type | Compile-time | Rejection |

---

#### 17.6.4 Layout Introspection

##### Definition

The `layout` operation returns the memory layout of a type.

##### Syntax & Declaration

```cursive
procedure layout<T>(~) -> LayoutInfo
```

##### Static Semantics

**Type Constraint**

The type parameter `T` MUST have a known, complete layout. Types with incomplete layout include:

1. Recursive types without indirection
2. Types containing opaque foreign types with unknown size
3. Generic types with unsized type parameters

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect} \quad
    \text{HasLayout}(T)
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>layout::<}T\texttt{>()} : \texttt{LayoutInfo}
} \quad \text{(T-Layout)}$$

##### Dynamic Semantics

The returned `LayoutInfo` contains:

1. `size`: The size of `T` in bytes.
2. `alignment`: The alignment requirement of `T` in bytes.
3. `stride`: The array stride of `T` in bytes.

For types with `[[layout(C)]]`, the layout follows C ABI rules. For other types, the layout is Implementation-Defined but deterministic.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-REF-0033` | Error    | `layout` called on type with incomplete layout | Compile-time | Rejection |

---

#### 17.6.5 Name Introspection

##### Definition

The `type_name` operation returns the fully-qualified name of a type as a string.

##### Syntax & Declaration

```cursive
procedure type_name<T>(~) -> string
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect}
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>type\_name::<}T\texttt{>()} : \texttt{string}
} \quad \text{(T-TypeName)}$$

##### Dynamic Semantics

The returned string contains the fully-qualified name of `T`, including:

1. The module path (separated by `::`)
2. The type name
3. Generic arguments (if any), enclosed in `<` and `>`

**Examples**

| Type                         | Result                          |
| :--------------------------- | :------------------------------ |
| `i32`                        | `"i32"`                         |
| `std::collections::Vec<i32>` | `"std::collections::Vec<i32>"`  |
| `Option<string>`             | `"std::option::Option<string>"` |
| `MyModule::Record`           | `"MyModule::Record"`            |

The exact format of the string is Implementation-Defined, but MUST be consistent within a compilation and MUST uniquely identify the type.

---

#### 17.6.6 Module Path Introspection

##### Definition

The `module_path` operation returns the module path containing a type.

##### Syntax & Declaration

```cursive
procedure module_path<T>(~) -> string
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect}
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>module\_path::<}T\texttt{>()} : \texttt{string}
} \quad \text{(T-ModulePath)}$$

##### Dynamic Semantics

The returned string contains the module path where `T` is defined, with path components separated by `::`.

For primitive types, the result is `"std::primitive"`.

For generic instantiations, the result is the module path of the generic type definition, not the instantiation site.

---

#### 17.6.7 Attribute Introspection

##### Definition

The `attributes` operation returns the attributes attached to a type.

##### Syntax & Declaration

```cursive
procedure attributes<T>(~) -> Sequence<Attribute>
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash w : \texttt{witness Introspect}
}{
    \Gamma_{ct} \vdash w\texttt{\~{}>attributes::<}T\texttt{>()} : \texttt{Sequence}\langle\texttt{Attribute}\rangle
} \quad \text{(T-Attributes)}$$

##### Dynamic Semantics

The returned sequence contains one `Attribute` for each attribute attached to the type declaration, in declaration order.

For primitive types and built-in types, the result is an empty sequence.

---

### 17.7 Summary of Diagnostic Codes

This section provides a consolidated reference of all diagnostic codes defined in this clause.

| Code         | Severity | Section | Condition                                      |
| :----------- | :------- | :------ | :--------------------------------------------- |
| `E-REF-0001` | Error    | §17.2   | Type match expression outside comptime context |
| `E-REF-0002` | Error    | §17.2   | Non-exhaustive type pattern match              |
| `E-REF-0003` | Error    | §17.2   | Type match arms have inconsistent result types |
| `E-REF-0004` | Error    | §17.2   | Invalid type pattern syntax                    |
| `E-REF-0010` | Error    | §17.3   | Duplicate binding in type pattern              |
| `E-REF-0011` | Error    | §17.3   | Pattern requires field not present in type     |
| `E-REF-0012` | Error    | §17.3   | Pattern requires variant not present in type   |
| `E-REF-0013` | Error    | §17.3   | Pattern requires state not present in type     |
| `E-REF-0014` | Error    | §17.3   | Tuple pattern arity mismatch                   |
| `E-REF-0015` | Error    | §17.3   | Generic pattern arity mismatch                 |
| `W-REF-0010` | Warning  | §17.3   | Type pattern binding shadows existing binding  |
| `E-REF-0020` | Error    | §17.5.1 | Field access on `TypeRepr`                     |
| `E-REF-0021` | Error    | §17.5.1 | Pattern match on `TypeRepr`                    |
| `E-REF-0022` | Error    | §17.5.2 | `form_repr` called on non-form type            |
| `E-REF-0023` | Error    | §17.5.2 | Field access on `FormRepr`                     |
| `E-REF-0024` | Error    | §17.5.2 | Pattern match on `FormRepr`                    |
| `E-REF-0030` | Error    | §17.6.1 | `fields` called on non-record type             |
| `E-REF-0031` | Error    | §17.6.2 | `variants` called on non-enum type             |
| `E-REF-0032` | Error    | §17.6.3 | `states` called on non-modal type              |
| `E-REF-0033` | Error    | §17.6.4 | `layout` called on type with incomplete layout |

---

## Clause 18: Code Generation

This clause defines the code generation system for Cursive. Code generation enables the construction of program fragments at compile time through quasiquoting, their transformation through splice operations, and their injection into the program through code emission. Code generation uses type reflection (§17) to inspect type structure when constructing AST fragments and operates within the comptime environment defined in §16.1. The derivation system (§19) builds upon code generation to automate implementation synthesis.

---

### 18.1 Overview

##### Definition

**Code generation** is the programmatic construction and emission of program fragments during the Metaprogramming Phase. Code generation enables comptime procedures to produce declarations, implementations, and expressions that become part of the compiled program.

Cursive provides four complementary code generation mechanisms:

1. **The Ast modal type** — A modal type representing syntax fragments with tracked validation state, enabling safe progression from parsed syntax to type-checked code.

2. **Quote expressions** — Syntax that captures Cursive code as `Ast` values without evaluating it, supporting both deferred and immediate type checking.

3. **Splice operators** — Interpolation operators that inject compile-time values into quoted code, enabling dynamic code construction.

4. **Code emission** — Operations that inject generated AST into module scope, making generated code visible to the runtime program.

##### Static Semantics

**Context Restriction**

All code generation operations MUST occur within comptime context as defined in §16.1. Quote expressions, splice operators, and emission operations are not available in runtime code.

**Capability Requirements**

Code emission requires the `ModuleEmitter` capability (§16.5.2). Quote expressions and splice operators do not require explicit capabilities beyond the implicit comptime context.

**Relationship to Other Clauses**

| Clause                       | Relationship                                                    |
| :--------------------------- | :-------------------------------------------------------------- |
| §16 (Compile-Time Execution) | Code generation operates within the comptime environment        |
| §17 (Type Reflection)        | Reflection provides type information for constructing typed AST |
| §19 (Derivation)             | Derivation uses code generation to synthesize implementations   |

---

### 18.2 The Ast Modal Type

##### Definition

The **Ast modal type** represents a syntax fragment with tracked validation state. `Ast` is a modal type as defined in §6.1, with states representing the progression from parsed syntax to type-checked code ready for emission.

**Formal Definition**

```cursive
modal Ast<T = ()> {
    @Parsed {
        span: SourceSpan,
    } {
        /// Attempt to type-check the AST fragment
        transition check(~!, context: TypeContext) -> @Typed | @Invalid
    }
    
    @Typed {
        result_type: TypeRepr,
    } {
        /// Emit into module scope; consumes the AST
        transition emit(~!, target: witness ModuleEmitter)
        
        /// Transform the result type
        procedure map<U>(~, f: procedure(T) -> U) -> Ast<U>@Typed
    }
    
    @Invalid {
        errors: [TypeError],
        span: SourceSpan,
    } {
        /// Extract errors for custom handling
        procedure errors(~) -> [TypeError]
    }
}
```

##### Static Semantics

**Type Parameter Semantics**

The type parameter `T` represents the result type of the AST fragment:

| Instantiation | Meaning                                      |
| :------------ | :------------------------------------------- |
| `Ast<()>`     | Statement or declaration (no result value)   |
| `Ast<T>`      | Expression that produces a value of type `T` |

The default type parameter is `()`, so `Ast` without explicit parameter represents statements or declarations.

**State Transition Rules**

The `Ast` modal type follows the state machine:

$$\texttt{@Parsed} \xrightarrow{\texttt{check}} \texttt{@Typed} \mid \texttt{@Invalid}$$

$$\texttt{@Typed} \xrightarrow{\texttt{emit}} \text{(consumed)}$$

**Typing Rule for Check Transition**

$$\frac{
    \Gamma_{ct} \vdash a : \texttt{Ast}\langle T \rangle\texttt{@Parsed} \quad
    \Gamma_{ct} \vdash c : \texttt{TypeContext}
}{
    \Gamma_{ct} \vdash a\texttt{\~{}>check}(c) : \texttt{Ast}\langle T \rangle\texttt{@Typed} \mid \texttt{Ast}\langle T \rangle\texttt{@Invalid}
} \quad \text{(T-Ast-Check)}$$

**Typing Rule for Emit Transition**

The `emit` transition is available only on `Ast@Typed`. This statically ensures that only type-checked code can be emitted:

$$\frac{
    \Gamma_{ct} \vdash a : \texttt{Ast}\langle T \rangle\texttt{@Typed} \quad
    \Gamma_{ct} \vdash m : \texttt{witness ModuleEmitter}
}{
    \Gamma_{ct} \vdash a\texttt{\~{}>emit}(m) : ()
} \quad \text{(T-Ast-Emit)}$$

**Typing Rule for Map Operation**

$$\frac{
    \Gamma_{ct} \vdash a : \texttt{Ast}\langle T \rangle\texttt{@Typed} \quad
    \Gamma_{ct} \vdash f : T \to U
}{
    \Gamma_{ct} \vdash a\texttt{.map}(f) : \texttt{Ast}\langle U \rangle\texttt{@Typed}
} \quad \text{(T-Ast-Map)}$$

**TypeContext**

The `TypeContext` record represents the compile-time type environment used for checking quoted code:

```cursive
record TypeContext {
    /// Bindings from identifiers to their types
    bindings: [(string, TypeRepr)],
    
    /// Type parameters in scope
    type_params: [TypeParamInfo],
}
```

A `TypeContext` is obtained from the current comptime environment via the `comptime_context()` intrinsic.

**Comptime Availability**

The `Ast` modal type is comptime-available per §16.2 rule 5: $\texttt{Ast}\langle T \rangle\texttt{@}S \in \mathcal{C}$ for any $T$ and state $S$.

##### Dynamic Semantics

**Check Transition Behavior**

When `check` is invoked on `Ast@Parsed`:

1. The syntax node is type-checked against the provided type context.
2. Spliced expressions within the AST are evaluated and their types are verified.
3. If type-checking succeeds, the AST transitions to `@Typed` with the inferred result type stored in `result_type`.
4. If type-checking fails, the AST transitions to `@Invalid` with diagnostic information in `errors`.

**Emit Transition Behavior**

When `emit` is invoked on `Ast@Typed`:

1. The typed AST is appended to the target module's emission queue.
2. The `Ast` value is consumed (the `unique` permission on self is required).
3. The emitted declaration becomes visible after the Metaprogramming Phase completes.

**Error Extraction**

When `errors` is invoked on `Ast@Invalid`:

1. The error list is returned for inspection.
2. The `Ast@Invalid` value remains usable for further error handling.

##### Constraints & Legality

**Emission State Requirement**

The `emit` transition MUST NOT be invoked on `Ast@Parsed` or `Ast@Invalid`. The modal type system statically prevents this.

**Consumption Requirement**

After `emit` is invoked, the `Ast` value is consumed. Subsequent use of the value is ill-formed and detected by the permission system.

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-GEN-0200` | Error    | `emit` called on `Ast` not in `@Typed` state  | Compile-time | Rejection |
| `E-GEN-0201` | Error    | `check` called with incompatible type context | Compile-time | Rejection |
| `E-GEN-0202` | Error    | `Ast` value used after consumption by `emit`  | Compile-time | Rejection |
| `E-GEN-0203` | Error    | `Ast@Invalid` used where `Ast@Typed` required | Compile-time | Rejection |

---

### 18.3 Quote Expressions

##### Definition

A **quote expression** captures Cursive syntax as an `Ast` value without evaluating it. The resulting value may be manipulated, transformed, type-checked, and later emitted into the program.

##### Syntax & Declaration

**Grammar**

```ebnf
quote_expr       ::= "quote" quote_body
                   | "quote" "[" type "]" expression

quote_body       ::= quote_block
                   | "(" expression ")"

quote_block      ::= "{" quoted_item* "}"

quoted_item      ::= statement
                   | declaration
                   | splice_expr
```

The `statement` and `declaration` productions are defined in §11.2 and §5.1 respectively. The `splice_expr` production is defined in §18.4.

**Quote Forms**

Cursive provides three quote forms with different type checking behavior:

| Syntax               | Result Type      | Type Checking           |
| :------------------- | :--------------- | :---------------------- |
| `quote { items... }` | `Ast<()>@Parsed` | Deferred until `check`  |
| `quote (expr)`       | `Ast<_>@Parsed`  | Deferred until `check`  |
| `quote[T] expr`      | `Ast<T>@Typed`   | Immediate at quote site |

**Disambiguation Rule**

When the token immediately following `quote` is `{`, the parser selects `quote_block`. To quote an expression that is a block, enclose the block in parentheses: `quote ({ ... })`.

**Examples**

```cursive
// Quote a block of declarations (deferred type checking)
let decl_ast = quote {
    record Point { x: i32, y: i32 }
}

// Quote an expression (deferred type checking)
let expr_ast = quote (1 + 2 * 3)

// Quote with immediate type checking
let typed_expr = quote[i32] (1 + 2)
```

##### Static Semantics

**Syntactic Validation**

The content of a quote expression MUST be syntactically valid Cursive. Implementations MUST parse quoted code at definition time and report syntax errors immediately:

$$\frac{
    \text{Parse}(\textit{content}) = \text{Err}(e)
}{
    \text{Emit}(\texttt{E-GEN-0210}, e)
} \quad \text{(Quote-Syntax-Check)}$$

**Deferred Type Checking**

For `quote { ... }` and `quote (...)`, type checking is deferred. The quote expression produces `Ast@Parsed`:

$$\frac{
    \text{Parse}(\textit{items}) = \text{Ok}
}{
    \Gamma_{ct} \vdash \texttt{quote}\ \{ \textit{items} \} : \texttt{Ast}\langle () \rangle\texttt{@Parsed}
} \quad \text{(T-Quote-Block)}$$

$$\frac{
    \text{Parse}(e) = \text{Ok}
}{
    \Gamma_{ct} \vdash \texttt{quote}\ (e) : \texttt{Ast}\langle \_ \rangle\texttt{@Parsed}
} \quad \text{(T-Quote-Expr)}$$

The underscore `_` in `Ast<_>@Parsed` indicates that the result type is not yet determined; it is inferred when `check` is invoked.

**Immediate Type Checking**

For `quote[T] expr`, type checking occurs immediately at the quote site:

$$\frac{
    \Gamma_{ct} \vdash e : T
}{
    \Gamma_{ct} \vdash \texttt{quote}[T]\ e : \texttt{Ast}\langle T \rangle\texttt{@Typed}
} \quad \text{(T-Quote-Typed)}$$

The typed quote form is useful when the expected type is known and immediate feedback is desired.

**Splice Resolution**

Splice expressions within quotes are resolved at quote construction time. The spliced values are captured into the AST structure. See §18.4 for splice semantics.

##### Constraints & Legality

**Context Restriction**

Quote expressions MUST appear within comptime context:

$$\frac{
    \texttt{quote}\ \ldots \text{ outside comptime context}
}{
    \text{Emit}(\texttt{E-GEN-0211})
} \quad \text{(Quote-Context-Check)}$$

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0210` | Error    | Quote block contains syntax error                  | Compile-time | Rejection |
| `E-GEN-0211` | Error    | Quote expression outside comptime context          | Compile-time | Rejection |
| `E-GEN-0212` | Error    | Typed quote expression does not have expected type | Compile-time | Rejection |

---

### 18.4 Splice Operators

##### Definition

The **splice operator** `$(...)` interpolates compile-time values into quoted code. The expression inside the splice is evaluated at compile time, and its result is substituted into the surrounding quote.

The **identifier splice** `$ident` is shorthand for `$(ident)`.

##### Syntax & Declaration

**Grammar**

```ebnf
splice_expr     ::= "$(" expression ")"
                  | "$" IDENTIFIER
```

**Examples**

```cursive
comptime {
    let field_name = "value"
    let field_type = type_repr::<i32>()
    
    let ast = quote {
        record Generated {
            $(field_name): $(field_type),
        }
    }
}
```

##### Static Semantics

**Context-Sensitive Splice Resolution**

The behavior of a splice depends on its syntactic context within the quote:

| Context              | Required Splice Type                                 | Result                  |
| :------------------- | :--------------------------------------------------- | :---------------------- |
| Expression position  | `Ast<T>@Typed`, `Ast<T>@Parsed`, or comptime literal | AST node insertion      |
| Type position        | `TypeRepr`                                           | Type AST insertion      |
| Identifier position  | `string`                                             | Identifier construction |
| Statement position   | `Ast<()>@Typed` or `Ast<()>@Parsed`                  | Statement insertion     |
| Declaration position | `Ast<()>@Typed` or `Ast<()>@Parsed`                  | Declaration insertion   |

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash e : T \quad
    \text{SpliceCompatible}(T, \textit{context})
}{
    \Gamma_{ct} \vdash \texttt{\$(}e\texttt{)} : \text{SpliceResult}(\textit{context})
} \quad \text{(T-Splice)}$$

**Splice Compatibility**

The predicate $\text{SpliceCompatible}(T, \textit{context})$ holds when type $T$ is valid for the given syntactic context:

| Context              | Compatible Types                                              |
| :------------------- | :------------------------------------------------------------ |
| Expression position  | `Ast<T>@Typed`, `Ast<T>@Parsed`, or any comptime literal type |
| Type position        | `TypeRepr`                                                    |
| Identifier position  | `string`                                                      |
| Statement position   | `Ast<()>@Typed`, `Ast<()>@Parsed`                             |
| Declaration position | `Ast<()>@Typed`, `Ast<()>@Parsed`                             |

**Comptime Literal Types**

A comptime literal type is any type whose values can be converted to AST literal nodes: primitive types, `string`, tuples of comptime literal types, and arrays of comptime literal types.

**Identifier Validation**

A string used for identifier splicing MUST conform to the identifier grammar defined in §2.5. If the string does not form a valid identifier:

$$\frac{
    \Gamma_{ct} \vdash e : \texttt{string} \quad
    \textit{context} = \text{Identifier} \quad
    \neg\text{IsValidIdentifier}(\text{value}(e))
}{
    \text{Emit}(\texttt{E-GEN-0221})
} \quad \text{(Splice-Ident-Check)}$$

**Identifier Splice Shorthand**

The identifier splice `$ident` is desugared to `$(ident)`:

$$\texttt{\$}x \equiv \texttt{\$(}x\texttt{)}$$

##### Constraints & Legality

**Splice Context Restriction**

Splice expressions MUST appear within a quote expression:

$$\frac{
    \texttt{\$(}e\texttt{)} \text{ outside quote context}
}{
    \text{Emit}(\texttt{E-GEN-0222})
} \quad \text{(Splice-Context-Check)}$$

**Splice Evaluation**

The expression inside a splice MUST be evaluable at compile time:

$$\frac{
    \Gamma_{ct} \vdash e : T \quad
    \neg\text{IsComptimeEvaluable}(e)
}{
    \text{Emit}(\texttt{E-GEN-0223})
} \quad \text{(Splice-Comptime-Check)}$$

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-GEN-0220` | Error    | Splice type incompatible with syntactic context | Compile-time | Rejection |
| `E-GEN-0221` | Error    | Invalid identifier string in splice             | Compile-time | Rejection |
| `E-GEN-0222` | Error    | Splice expression outside quote context         | Compile-time | Rejection |
| `E-GEN-0223` | Error    | Splice expression not evaluable at compile time | Compile-time | Rejection |

---

### 18.5 Hygiene

##### Definition

Cursive implements **hygienic macro semantics** to prevent unintended identifier capture during code generation. Hygiene ensures that identifiers introduced by code generation do not accidentally bind or shadow identifiers at the emission site, and that identifiers referenced in generated code resolve to their intended bindings.

##### Static Semantics

**Capture Set**

For a quoted expression $q$, the **capture set** is the set of identifiers referenced in $q$ that are not defined within $q$:

$$\text{Captures}(q) = \text{FreeVars}(q) \cap \text{Dom}(\Gamma_{ct})$$

Each identifier in $\text{Captures}(q)$ is resolved at the quote definition site. The binding to which each captured identifier refers is recorded and preserved through emission.

**Fresh Name Set**

The **fresh name set** for a quoted expression $q$ is the set of identifiers bound within $q$:

$$\text{Fresh}(q) = \text{BoundVars}(q)$$

Each identifier in $\text{Fresh}(q)$ is renamed at emission time to avoid collision with bindings at the emission site:

$$\text{Rename}(x) = \text{gensym}(x, \text{EmissionContext})$$

The `gensym` function generates a unique identifier distinct from any identifier in both the quote scope and the emission scope.

**Hygiene Preservation**

The hygiene system maintains the following invariant: for any identifier $x$ appearing in emitted code:

1. If $x \in \text{Captures}(q)$, then $x$ resolves to the same binding it referenced at the quote site.
2. If $x \in \text{Fresh}(q)$, then $x$ does not capture any binding at the emission site.

**Hygiene Breaking**

To introduce identifiers into the emission scope intentionally, use identifier splicing with a string value:

| Splice Input Type           | Hygiene Behavior                       |
| :-------------------------- | :------------------------------------- |
| `string`                    | **Unhygienic**—binds in emission scope |
| `Ast` containing identifier | **Hygienic**—preserves original scope  |

This distinction enables intentional introduction of bindings visible to surrounding code:

```cursive
comptime {
    let user_name = "my_var"  // string value
    
    // Unhygienic: introduces binding visible at emission site
    let ast = quote {
        let $(user_name): i32 = 42
    }
}
```

**Captured Identifier Validity**

All identifiers in $\text{Captures}(q)$ MUST resolve to bindings that remain valid at emission time. If a captured identifier refers to a binding that is no longer in scope when the quoted code is emitted, the program is ill-formed:

$$\frac{
    x \in \text{Captures}(q) \quad
    \neg\text{ValidAtEmission}(x)
}{
    \text{Emit}(\texttt{E-GEN-0230})
} \quad \text{(Capture-Validity-Check)}$$

##### Dynamic Semantics

**Name Generation**

Fresh names are generated using a deterministic algorithm that incorporates:

1. The original identifier name
2. A unique counter for the emission context
3. The source location of the quote

This ensures that fresh names are reproducible across compilations with identical input.

**Example**

```cursive
comptime {
    // Hygienic: 'temp' will be renamed to avoid capture
    let ast = quote {
        let temp = compute()
        process(temp)
    }
    
    emitter~>emit(ast)
}

// At emission site, 'temp' might be renamed to 'temp_42_:L10C5'
// ensuring it doesn't conflict with any 'temp' at the emission site
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0230` | Error    | Captured identifier no longer in scope at emission | Compile-time | Rejection |
| `E-GEN-0231` | Error    | Hygiene renaming collision (implementation limit)  | Compile-time | Rejection |

---

### 18.6 Comptime Iteration

##### Definition

A **comptime for loop** iterates over a compile-time sequence and unrolls the loop body at compile time. Each iteration produces generated code, with the complete set of iterations combined into the result.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_for ::= "comptime" "for" pattern "in" comptime_expr block
```

The `pattern` production is defined in §11.5. The `comptime_expr` production is defined in §16.4.

**Example**

```cursive
comptime {
    let fields = introspect~>fields::<Player>()
    
    let ast = quote {
        comptime for field in $(fields) {
            self.$(field.name)~>process()
        }
    }
}
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash \textit{seq} : \texttt{Sequence}\langle E \rangle \quad
    \Gamma_{ct}, x : E \vdash \textit{body} : T
}{
    \Gamma_{ct} \vdash \texttt{comptime for } x \texttt{ in } \textit{seq}\ \textit{body} : [T]
} \quad \text{(T-Comptime-For)}$$

**Unrolling Semantics**

A comptime for loop over sequence $[e_1, e_2, \ldots, e_n]$ is semantically equivalent to:

$$\texttt{comptime for } x \texttt{ in } [e_1, \ldots, e_n]\ \{ B \} \equiv [B[x := e_1], B[x := e_2], \ldots, B[x := e_n]]$$

Where $B[x := e_i]$ denotes the body $B$ with all occurrences of $x$ replaced by $e_i$.

**Context-Sensitive Unrolling**

The result of unrolling depends on the syntactic context in which the comptime for loop appears:

| Context                           | Unrolled Result          |
| :-------------------------------- | :----------------------- |
| Statement position within quote   | Sequence of statements   |
| Match arm position within quote   | Sequence of match arms   |
| Declaration position within quote | Sequence of declarations |
| Expression position               | Array literal            |

**Quote Context Integration**

When `comptime for` appears within a `quote` block, the loop is unrolled and each iteration's body is spliced into the quoted AST as a sequence of items:

```cursive
quote {
    comptime for field in $(fields) {
        self.$(field.name)~>process()
    }
}
// For fields = [a, b, c], produces AST equivalent to:
// quote {
//     self.a~>process()
//     self.b~>process()
//     self.c~>process()
// }
```

**Sequence Type Requirement**

The expression following `in` MUST have type `Sequence<E>` for some element type `E`:

$$\frac{
    \Gamma_{ct} \vdash e : T \quad
    T \neq \texttt{Sequence}\langle E \rangle \text{ for any } E
}{
    \text{Emit}(\texttt{E-GEN-0240})
} \quad \text{(Comptime-For-Seq-Check)}$$

##### Dynamic Semantics

**Unroll Evaluation**

Comptime for loops are evaluated as follows:

1. The sequence expression is evaluated to produce a concrete sequence $[e_1, \ldots, e_n]$.
2. For each element $e_i$ in order:
   a. The loop variable is bound to $e_i$.
   b. The loop body is evaluated.
   c. The result is collected.
3. The collected results form the unrolled output.

**Static Iteration Count**

The iteration count of a comptime for loop MUST be statically determinable. If the sequence length depends on runtime values, the program is ill-formed:

$$\frac{
    \text{length}(\textit{seq}) \text{ depends on runtime values}
}{
    \text{Emit}(\texttt{E-GEN-0241})
} \quad \text{(Comptime-For-Static-Check)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                    | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0240` | Error    | Comptime for expression is not a compile-time sequence       | Compile-time | Rejection |
| `E-GEN-0241` | Error    | Comptime for body produces runtime-dependent iteration count | Compile-time | Rejection |
| `W-GEN-0240` | Warning  | Comptime for loop over empty sequence                        | Compile-time | Continue  |

---

### 18.7 Code Emission

##### Definition

**Code emission** is the process of injecting generated AST into module scope. Emission is performed via the `ModuleEmitter` capability (§16.5.2), making generated code visible to the rest of the program.

##### Syntax & Declaration

**Emission Operations**

The `ModuleEmitter` capability provides two emission operations:

```cursive
// Emit a declaration AST
emitter~>emit(ast)

// Emit a form implementation for a specific type
emitter~>emit_implement(target_type, form, body)
```

**Examples**

```cursive
[[derive_target]]
comptime procedure GenerateDebug(
    target: TypePattern,
    introspect: witness Introspect,
    emitter: witness ModuleEmitter,
) {
    let impl_ast = quote {
        implement Debug for $(type_repr_of(target)) {
            procedure debug(~) -> string {
                // Generated debug implementation
            }
        }
    }
    
    match impl_ast~>check(comptime_context()) {
        @Typed as typed => typed~>emit(emitter),
        @Invalid as invalid => {
            diagnostics~>error("Debug derive failed", invalid.span)
        }
    }
}
```

##### Static Semantics

**Typing Rule for emit**

$$\frac{
    \Gamma_{ct} \vdash m : \texttt{witness ModuleEmitter} \quad
    \Gamma_{ct} \vdash a : \texttt{Ast}\langle () \rangle\texttt{@Typed}
}{
    \Gamma_{ct} \vdash m\texttt{\~{}>emit}(a) : ()
} \quad \text{(T-Emit)}$$

**Typing Rule for emit_implement**

$$\frac{
    \Gamma_{ct} \vdash m : \texttt{witness ModuleEmitter} \quad
    \Gamma_{ct} \vdash t : \texttt{TypeRepr} \quad
    \Gamma_{ct} \vdash f : \texttt{FormRepr} \quad
    \Gamma_{ct} \vdash b : \texttt{Ast}\langle () \rangle\texttt{@Typed}
}{
    \Gamma_{ct} \vdash m\texttt{\~{}>emit\_implement}(t, f, b) : ()
} \quad \text{(T-Emit-Implement)}$$

**Injection Point**

Code is emitted into the module scope associated with the `ModuleEmitter` capability. For derive targets (§19.3), this is the module where the type being derived is declared.

**Capability Requirement**

The `emit` and `emit_implement` operations require a `ModuleEmitter` capability witness. This capability is provided to derive target procedures and comptime blocks with the appropriate authorization.

##### Dynamic Semantics

**Emission Ordering**

Emitted declarations are ordered as follows:

1. Each `emit` call appends the AST to the module's emission queue.
2. After all comptime execution completes, queued emissions are spliced into the module AST.
3. Emissions from the same comptime block appear in call order.
4. Emissions from different comptime blocks appear in execution order as determined by the dependency graph (§16.7).

**Post-Emission Type Checking**

After the Metaprogramming Phase completes, the implementation MUST perform full type checking on the expanded AST. Type errors in emitted code MUST be reported with a diagnostic trace including:

1. The source location of the `emit` call.
2. The generated source text (or a representative portion if large).
3. The specific error within the generated code.

**Conflict Detection**

If emitted declarations conflict with existing declarations (same name in same namespace), the implementation MUST emit a diagnostic:

$$\frac{
    \text{emit}(d) \quad
    \text{Name}(d) = \text{Name}(d') \quad
    d' \in \text{Declarations}(\text{Module})
}{
    \text{Emit}(\texttt{E-GEN-0252})
} \quad \text{(Emit-Conflict-Check)}$$

**Duplicate Implementation Detection**

If multiple form implementations are emitted for the same (type, form) pair, the implementation MUST emit a diagnostic:

$$\frac{
    \text{emit\_implement}(T, F, \_) \quad
    \text{emit\_implement}(T, F, \_)
}{
    \text{Emit}(\texttt{E-GEN-0253})
} \quad \text{(Emit-Duplicate-Impl-Check)}$$

##### Constraints & Legality

**State Requirement**

The `emit` operation MUST be invoked with `Ast@Typed`. Invoking `emit` on `Ast@Parsed` or `Ast@Invalid` is prevented by the modal type system.

**Capability Requirement**

Invoking `emit` without a `ModuleEmitter` capability is ill-formed:

$$\frac{
    \texttt{emit}(a) \quad
    \texttt{ModuleEmitter} \notin \Gamma_{ct}
}{
    \text{Emit}(\texttt{E-GEN-0251})
} \quad \text{(Emit-Cap-Check)}$$

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-GEN-0250` | Error    | Emitted code failed type checking                       | Compile-time | Rejection |
| `E-GEN-0251` | Error    | `emit` called without `ModuleEmitter` capability        | Compile-time | Rejection |
| `E-GEN-0252` | Error    | Emitted declaration conflicts with existing declaration | Compile-time | Rejection |
| `E-GEN-0253` | Error    | Duplicate form implementation emitted for (type, form)  | Compile-time | Rejection |

---

### 18.8 Compile-Time Keys

##### Definition

A **compile-time key** is a key acquired during the Metaprogramming Phase that governs access to type and declaration state. Compile-time keys use the same path-based model as runtime keys (§13) but operate on compilation state rather than runtime memory.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_key_block ::= "comptime" "#" comptime_path_list key_mode? block

comptime_path_list ::= comptime_path ("," comptime_path)*

comptime_path      ::= "type" type_path
                     | "module" module_path
                     | form_ref "for" type_path

key_mode           ::= "write"
```

The `type_path`, `module_path`, and `form_ref` productions are defined in §4, §8.1, and §9.1 respectively.

**Path Semantics**

| Path Form         | Meaning                                 |
| :---------------- | :-------------------------------------- |
| `type Foo`        | The definition of type `Foo`            |
| `module bar::baz` | The module `bar::baz`                   |
| `Form for Type`   | The implementation of `Form` for `Type` |

**Key Modes**

| Mode    | Meaning                  |
| :------ | :----------------------- |
| (none)  | Read access (default)    |
| `write` | Write access (exclusive) |

**Example**

```cursive
// Acquire read key to type Point definition
comptime #type Point {
    let fields = introspect~>fields::<Point>()
    // ... use fields
}

// Acquire write key to Debug implementation for Player
comptime #Debug for Player write {
    // Generate Debug implementation with exclusive access
}
```

##### Static Semantics

**Key Compatibility**

Compile-time keys follow the same compatibility rules as runtime keys (§13.1.2):

| Key A Mode | Key B Mode | Overlapping Paths | Compatible |
| :--------- | :--------- | :---------------- | :--------- |
| Read       | Read       | Yes               | Yes        |
| Read       | Write      | Yes               | No         |
| Write      | Write      | Yes               | No         |

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash P_1, \ldots, P_n : \text{ComptimePath} \quad
    \text{KeysAcquirable}(P_1, \ldots, P_n, \textit{mode}) \quad
    \Gamma_{ct}, \text{Keys}(P_1, \ldots, P_n, \textit{mode}) \vdash \textit{body} : T
}{
    \Gamma_{ct} \vdash \texttt{comptime \#}P_1, \ldots, P_n\ \textit{mode}\ \{ \textit{body} \} : T
} \quad \text{(T-Comptime-Key)}$$

**Ordering Guarantee**

If comptime block $A$ holds a Write key to path $P$ and comptime block $B$ attempts to acquire any key to $P$:

1. If $A$ and $B$ are in the same compilation unit, execution order is determined by textual order.
2. If $A$ and $B$ are in different compilation units, the implementation determines a deterministic order (Implementation-Defined).

**Path Existence**

The paths in a comptime key block MUST refer to declarations that exist at the point of key acquisition:

$$\frac{
    \texttt{comptime \#}P\ \{ \ldots \} \quad
    \neg\text{Exists}(P)
}{
    \text{Emit}(\texttt{E-GEN-0261})
} \quad \text{(Comptime-Key-Exists-Check)}$$

##### Dynamic Semantics

**Key Acquisition**

When a comptime key block is entered:

1. Keys to the specified paths are requested.
2. If all keys can be acquired without violating compatibility rules, execution proceeds.
3. If a key cannot be acquired (due to conflict with another key holder), execution blocks until the key is available.

**Key Release**

Keys are released when the comptime key block exits (normally or via panic).

**Cycle Prevention**

The canonical ordering rules (§13.7) apply. If a cycle is detected in compile-time key acquisition, the program is ill-formed:

$$\frac{
    \text{CycleDetected}(\text{KeyGraph})
}{
    \text{Emit}(\texttt{E-GEN-0260})
} \quad \text{(Comptime-Key-Cycle-Check)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                            | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0260` | Error    | Circular dependency in compile-time key acquisition  | Compile-time | Rejection |
| `E-GEN-0261` | Error    | Compile-time key path does not exist                 | Compile-time | Rejection |
| `W-GEN-0260` | Warning  | Unnecessary compile-time key (no conflicting access) | Compile-time | Continue  |

---

### 18.9 Summary of Diagnostic Codes

This section provides a consolidated reference of all diagnostic codes defined in this clause.

| Code         | Severity | Section | Condition                                               |
| :----------- | :------- | :------ | :------------------------------------------------------ |
| `E-GEN-0200` | Error    | §18.2   | `emit` called on `Ast` not in `@Typed` state            |
| `E-GEN-0201` | Error    | §18.2   | `check` called with incompatible type context           |
| `E-GEN-0202` | Error    | §18.2   | `Ast` value used after consumption by `emit`            |
| `E-GEN-0203` | Error    | §18.2   | `Ast@Invalid` used where `Ast@Typed` required           |
| `E-GEN-0210` | Error    | §18.3   | Quote block contains syntax error                       |
| `E-GEN-0211` | Error    | §18.3   | Quote expression outside comptime context               |
| `E-GEN-0212` | Error    | §18.3   | Typed quote expression does not have expected type      |
| `E-GEN-0220` | Error    | §18.4   | Splice type incompatible with syntactic context         |
| `E-GEN-0221` | Error    | §18.4   | Invalid identifier string in splice                     |
| `E-GEN-0222` | Error    | §18.4   | Splice expression outside quote context                 |
| `E-GEN-0223` | Error    | §18.4   | Splice expression not evaluable at compile time         |
| `E-GEN-0230` | Error    | §18.5   | Captured identifier no longer in scope at emission      |
| `E-GEN-0231` | Error    | §18.5   | Hygiene renaming collision (implementation limit)       |
| `E-GEN-0240` | Error    | §18.6   | Comptime for expression is not a compile-time sequence  |
| `E-GEN-0241` | Error    | §18.6   | Comptime for produces runtime-dependent iteration count |
| `W-GEN-0240` | Warning  | §18.6   | Comptime for loop over empty sequence                   |
| `E-GEN-0250` | Error    | §18.7   | Emitted code failed type checking                       |
| `E-GEN-0251` | Error    | §18.7   | `emit` called without `ModuleEmitter` capability        |
| `E-GEN-0252` | Error    | §18.7   | Emitted declaration conflicts with existing declaration |
| `E-GEN-0253` | Error    | §18.7   | Duplicate form implementation emitted for (type, form)  |
| `E-GEN-0260` | Error    | §18.8   | Circular dependency in compile-time key acquisition     |
| `E-GEN-0261` | Error    | §18.8   | Compile-time key path does not exist                    |
| `W-GEN-0260` | Warning  | §18.8   | Unnecessary compile-time key (no conflicting access)    |

---

## Clause 19: Derivation

This clause defines the derivation system for Cursive. Derivation automates the synthesis of form implementations by combining type reflection (§17) and code generation (§18). The derive system enables types to acquire implementations through compile-time procedures that inspect type structure and emit appropriate code. This clause specifies derive attributes, derive target procedures, conditional implementations, and generation contracts.

---

### 19.1 Overview

##### Definition

**Derivation** is the automated synthesis of form implementations based on type structure. Derivation eliminates repetitive boilerplate code by generating implementations at compile time according to declarative specifications.

Cursive provides three complementary derivation mechanisms:

1. **Derive attributes** — Annotations on type declarations that invoke derive target procedures to generate implementations.

2. **Conditional implementations** — Parameterized implementation blocks that apply to all types matching a structural pattern and satisfying specified constraints.

3. **Generation contracts** — Postconditions on derive targets that constrain and verify the emitted code.

##### Static Semantics

**Context Restriction**

Derivation occurs during the Metaprogramming Phase (§16.1). Derive target procedures execute in comptime context and emit code using the mechanisms defined in §18.

**Capability Requirements**

Derive target procedures receive the following capabilities:

| Capability            | Purpose                                           |
| :-------------------- | :------------------------------------------------ |
| `Introspect`          | Inspect target type structure (§17)               |
| `ModuleEmitter`       | Emit generated implementations (§18.7)            |
| `ComptimeDiagnostics` | Report compile-time errors and warnings (§16.5.4) |

**Relationship to Other Clauses**

| Clause                       | Relationship                                               |
| :--------------------------- | :--------------------------------------------------------- |
| §16 (Compile-Time Execution) | Derivation operates within the comptime environment        |
| §17 (Type Reflection)        | Derive targets use reflection to inspect target types      |
| §18 (Code Generation)        | Derive targets use code generation to emit implementations |

---

### 19.2 Derive Attributes

##### Definition

A **derive attribute** is an attribute of the form `[[derive(Name, ...)]]` attached to a type declaration. A derive attribute requests that the implementation invoke one or more derive target procedures to generate code for the annotated type.

##### Syntax & Declaration

**Grammar**

```ebnf
derive_attribute        ::= "[[" "derive" "(" derive_list ")" "]]"

derive_list             ::= derive_item ("," derive_item)*

derive_item             ::= derive_name
                          | derive_name "(" derive_config ")"

derive_name             ::= IDENTIFIER
                          | qualified_path

derive_config           ::= derive_arg ("," derive_arg)*

derive_arg              ::= IDENTIFIER ":" expression
```

The `qualified_path` production is defined in §8.1. The `expression` production is defined in §11.1.

**Examples**

```cursive
// Simple derive without configuration
[[derive(Debug, Clone, Eq)]]
record Point {
    x: i32,
    y: i32,
}

// Derive with configuration
[[derive(Serialize(format: Json), Hash(algorithm: Sha256))]]
record Config {
    name: string,
    value: i32,
}

// Qualified derive target
[[derive(mylib::custom::MyDerive)]]
enum Status { Active, Inactive }
```

**Derive Item Forms**

| Form                    | Meaning                                        |
| :---------------------- | :--------------------------------------------- |
| `Name`                  | Invoke derive target `Name` with defaults      |
| `Name(key: value, ...)` | Invoke derive target `Name` with configuration |

##### Static Semantics

**Attribute Placement**

Derive attributes MAY be attached to:

1. Record declarations
2. Enum declarations
3. Modal type declarations

Derive attributes MUST NOT be attached to:

1. Procedure declarations
2. Form declarations
3. Module declarations
4. Type aliases

**Multiple Derive Attributes**

Multiple derive attributes on a single declaration are permitted and are processed in declaration order:

```cursive
[[derive(Debug)]]
[[derive(Clone)]]
record Foo { ... }

// Equivalent to:
[[derive(Debug, Clone)]]
record Foo { ... }
```

**Typing Rule**

$$\frac{
    \text{IsTypeDecl}(D) \quad
    \forall d_i \in \text{DeriveItems}(A).\ \text{WellFormedDeriveItem}(d_i)
}{
    A \text{ is well-formed on } D
} \quad \text{(WF-Derive-Attr)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-DRV-0400` | Error    | Derive attribute on non-type declaration | Compile-time | Rejection |
| `E-DRV-0401` | Error    | Duplicate derive item in same attribute  | Compile-time | Rejection |
| `E-DRV-0402` | Error    | Invalid derive configuration syntax      | Compile-time | Rejection |

---

### 19.3 Derive Targets

##### Definition

A **derive target** is a comptime procedure annotated with `[[derive_target]]` that generates code based on type introspection. Derive targets receive information about the type being derived and capabilities for introspection and code emission.

##### Syntax & Declaration

**Grammar**

```ebnf
derive_target_decl      ::= "[[" "derive_target" "]]"
                            "comptime" "procedure" IDENTIFIER
                            "(" derive_target_params ")"
                            generation_contract?
                            block

derive_target_params    ::= target_param "," introspect_param "," emitter_param
                            ("," config_param)*

target_param            ::= IDENTIFIER ":" type_pattern_type

introspect_param        ::= IDENTIFIER ":" "witness" "Introspect"

emitter_param           ::= IDENTIFIER ":" "witness" "ModuleEmitter"

config_param            ::= IDENTIFIER ":" type ("=" expression)?
```

The `type_pattern_type`, `type`, `expression`, and `block` productions are defined in §17.2, §4, §11.1, and §11.2 respectively. The `generation_contract` production is defined in §19.6.

**Required Signature Pattern**

A derive target procedure MUST conform to the following signature pattern:

```cursive
[[derive_target]]
comptime procedure DeriveName(
    target: TypePattern,                    // REQUIRED: matched type
    introspect: witness Introspect,         // REQUIRED: introspection capability
    emitter: witness ModuleEmitter,         // REQUIRED: emission capability
    // OPTIONAL: configuration parameters with defaults
    config1: Type1 = default1,
    config2: Type2 = default2,
)
```

The first three parameters are mandatory and are provided by the implementation. Additional parameters with default values MAY be specified; these receive values from the derive attribute configuration.

**Example**

```cursive
[[derive_target]]
comptime procedure Debug(
    target: TypePattern,
    introspect: witness Introspect,
    emitter: witness ModuleEmitter,
    verbose: bool = false,
) {
    match type target {
        Record { .. } => generate_record_debug(target, introspect, emitter, verbose),
        Enum { .. } => generate_enum_debug(target, introspect, emitter, verbose),
        type _ => {
            diagnostics~>error(
                "Debug can only be derived for records and enums",
                current_span()
            )
        }
    }
}
```

##### Static Semantics

**Attribute Requirement**

A comptime procedure referenced in a derive attribute MUST have the `[[derive_target]]` attribute:

$$\frac{
    \texttt{[[derive(}D\texttt{)]]} \quad
    \text{resolve}(D) = p \quad
    \neg\text{HasAttribute}(p, \texttt{derive\_target})
}{
    \text{Emit}(\texttt{E-DRV-0410})
} \quad \text{(Derive-Target-Attr-Check)}$$

**Signature Validation**

The derive target procedure MUST have the required signature pattern:

$$\frac{
    \text{HasAttribute}(p, \texttt{derive\_target}) \quad
    \neg\text{ValidDeriveSignature}(p)
}{
    \text{Emit}(\texttt{E-DRV-0411})
} \quad \text{(Derive-Target-Sig-Check)}$$

**Valid Derive Signature**

A procedure has a valid derive signature if:

1. The first parameter has type `TypePattern`.
2. The second parameter has type `witness Introspect`.
3. The third parameter has type `witness ModuleEmitter`.
4. All subsequent parameters have default values.
5. The procedure is declared with the `comptime` modifier.

**Capability Provision**

The implementation provides capability witnesses to derive target procedures:

| Parameter Position | Capability Provided | Scope                               |
| :----------------- | :------------------ | :---------------------------------- |
| Second             | `Introspect`        | Type being derived and dependencies |
| Third              | `ModuleEmitter`     | Module containing type declaration  |

Additionally, the `ComptimeDiagnostics` capability is implicitly available via the `diagnostics` identifier.

**Visibility Requirements**

A derive target referenced in a derive attribute MUST be visible from the scope containing the type declaration:

$$\frac{
    \texttt{[[derive(}D\texttt{)]]} \text{ on type } T \quad
    \neg\text{Visible}(D, \text{Scope}(T))
}{
    \text{Emit}(\texttt{E-DRV-0412})
} \quad \text{(Derive-Target-Vis-Check)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `E-DRV-0410` | Error    | Derive target not found                             | Compile-time | Rejection |
| `E-DRV-0411` | Error    | Derive target has incorrect signature               | Compile-time | Rejection |
| `E-DRV-0412` | Error    | Derive target not visible from declaration scope    | Compile-time | Rejection |
| `E-DRV-0413` | Error    | Derive target is not a comptime procedure           | Compile-time | Rejection |
| `E-DRV-0414` | Error    | Derive target missing `[[derive_target]]` attribute | Compile-time | Rejection |

---

### 19.4 Derive Execution

##### Definition

**Derive execution** is the process by which derive target procedures are invoked during the Metaprogramming Phase to generate implementations for annotated types.

##### Static Semantics

**Resolution Rules**

For each derive item $D$ in a `[[derive(D)]]` attribute:

1. Resolve the identifier or qualified path $D$ to a procedure declaration.
2. Verify the procedure has the `[[derive_target]]` attribute.
3. Verify the procedure has a valid derive signature.
4. If resolution fails, emit `E-DRV-0410`.

**Typing Rule for Derive Resolution**

$$\frac{
    \text{resolve}(D) = p \quad
    \text{HasAttribute}(p, \texttt{derive\_target}) \quad
    \text{ValidDeriveSignature}(p)
}{
    \texttt{[[derive(}D\texttt{)]]}\ \text{resolves to } p
} \quad \text{(T-Derive-Resolve)}$$

**Configuration Passing**

When a derive item includes configuration `[[derive(Name(key: value))]]`:

1. Each `key: value` pair is matched to a parameter of the derive target by name.
2. The expression `value` is evaluated at compile time.
3. The result is passed to the corresponding parameter.
4. Parameters not specified in configuration receive their default values.

**Configuration Type Checking**

$$\frac{
    \text{Param}(p, k) = (k, T, \_) \quad
    \Gamma_{ct} \vdash v : T
}{
    \text{ConfigArg}(k: v) \text{ valid for } p
} \quad \text{(T-Derive-Config)}$$

**Execution Order**

Derive procedures execute in declaration order within a single `[[derive(...)]]` attribute:

```cursive
[[derive(A, B, C)]]  // A executes first, then B, then C
record Foo { ... }
```

For multiple derive attributes on the same type, attributes are processed in textual order:

```cursive
[[derive(A)]]        // A executes first
[[derive(B, C)]]     // Then B, then C
record Foo { ... }
```

**Order Guarantee**

For derive items $d_1, d_2, \ldots, d_n$ on type $T$:

$$\forall i < j.\ \text{Execute}(d_i) \prec \text{Execute}(d_j)$$

Where $\prec$ denotes "happens before" in the Metaprogramming Phase.

##### Dynamic Semantics

**Derive Execution Process**

When a type $T$ with `[[derive(D)]]` is encountered:

1. The type declaration is fully processed and its `TypePattern` representation is computed.
2. The derive target procedure $D$ is invoked with:
   - The type as a `TypePattern` value
   - An `Introspect` capability witness scoped to $T$ and its dependencies
   - A `ModuleEmitter` capability witness targeting $T$'s module
   - Configuration values for any additional parameters (or defaults)
3. The derive target procedure executes in comptime context.
4. Any code emitted by $D$ is associated with $T$'s declaration site.
5. After all derives for $T$ complete, emitted code is type-checked.

**Error Handling**

If a derive target procedure:

- **Panics:** The panic is caught and reported as `E-DRV-0420`. Compilation fails.
- **Emits invalid code:** Type checking of emitted code fails with `E-DRV-0421`. Compilation fails.
- **Emits conflicting declarations:** Conflict is detected per §18.7, reported with source trace.

**Partial Execution**

If a derive target fails, subsequent derives on the same type MAY still execute to provide additional diagnostics. This behavior is Implementation-Defined.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-DRV-0420` | Error    | Derive procedure panicked               | Compile-time | Rejection |
| `E-DRV-0421` | Error    | Derive procedure emitted invalid code   | Compile-time | Rejection |
| `E-DRV-0422` | Error    | Invalid derive configuration argument   | Compile-time | Rejection |
| `E-DRV-0423` | Error    | Missing required configuration argument | Compile-time | Rejection |
| `E-DRV-0424` | Error    | Unknown configuration key               | Compile-time | Rejection |

---

### 19.5 Conditional Implementations

##### Definition

A **conditional implementation** is an `implement` block that applies to all types satisfying a structural predicate expressed as a type pattern. Conditional implementations generalize implementations beyond specific named types to structural type categories.

##### Syntax & Declaration

**Grammar**

```ebnf
conditional_implement   ::= "implement" form_ref "for" type_pattern
                            ("where" constraint_list)?
                            implement_body

constraint_list         ::= constraint ("," constraint)*

constraint              ::= type_pattern ":" form_ref
                          | "fields" "(" IDENTIFIER ")" ":" form_ref
                          | "variants" "(" IDENTIFIER ")" ":" form_ref
                          | "comptime" expression
```

The `form_ref`, `type_pattern`, and `implement_body` productions are defined in §9.1, §17.2, and §9.2 respectively.

**Constraint Forms**

| Constraint Form      | Meaning                                                        |
| :------------------- | :------------------------------------------------------------- |
| `type X : Form`      | Bound type variable `X` implements form `Form`                 |
| `fields(f) : Form`   | All field types of the matched record implement `Form`         |
| `variants(v) : Form` | All variant payload types of the matched enum implement `Form` |
| `comptime expr`      | Expression `expr` evaluates to `true` at compile time          |

**Examples**

```cursive
// Debug for all records where all fields are Debug
implement Debug for Record { .. }
where fields(f) : Debug
{
    procedure debug(~) -> string {
        comptime {
            let fields = introspect~>fields::<Self>()
            quote {
                let mut s = introspect~>type_name::<Self>() + " { "
                comptime for field in $(fields) {
                    s = s + $(field.name) + ": " + self.$(field.name)~>debug() + ", "
                }
                s + "}"
            }
        }
    }
}

// Clone for all enums where all variant payloads are Clone
implement Clone for Enum { .. }
where variants(v) : Clone
{
    procedure clone(~) -> Self {
        // Generated match over all variants
    }
}

// Conditional on comptime expression
implement Serialize for Record { .. }
where comptime { introspect~>layout::<Self>().size <= 1024 }
{
    // Only for records with size <= 1024 bytes
}
```

##### Static Semantics

**Resolution Algorithm**

When the compiler checks whether type $T$ implements form $F$:

1. Search for explicit `implement F for T` declarations.
2. Search for conditional implementations `implement F for P where C`.
3. For each conditional implementation:
   a. Attempt to match $T$ against pattern $P$ using the rules in §17.3.
   b. If the match succeeds with binding environment $\sigma$:
      - Evaluate constraints $C$ with bindings from $\sigma$.
      - If all constraints are satisfied, the conditional implementation applies.
4. Collect all applicable implementations.
5. Determine the result:
   - If zero implementations apply, type $T$ does not implement form $F$.
   - If exactly one implementation applies, use it.
   - If multiple implementations apply, emit `E-DRV-0430` (ambiguous implementation).

**Typing Rule for Conditional Implementation**

$$\frac{
    \text{Matches}(T, P, \sigma) \quad
    \sigma \vdash C_1 \land \cdots \land C_n \quad
    \sigma \vdash \textit{body} : \text{ValidImpl}(F, T)
}{
    T : F \text{ via conditional implementation}
} \quad \text{(T-Cond-Impl)}$$

**Constraint Evaluation**

Each constraint form is evaluated as follows:

**Type Constraint** `type X : Form`:

$$\frac{
    \sigma(X) = T' \quad
    T' : F
}{
    \sigma \vdash X : F
} \quad \text{(C-Type)}$$

**Fields Constraint** `fields(f) : Form`:

$$\frac{
    \text{IsRecord}(\sigma(\text{target})) \quad
    \forall f \in \text{fields}(\sigma(\text{target})).\ \text{type}(f) : F
}{
    \sigma \vdash \texttt{fields}(f) : F
} \quad \text{(C-Fields)}$$

**Variants Constraint** `variants(v) : Form`:

$$\frac{
    \text{IsEnum}(\sigma(\text{target})) \quad
    \forall v \in \text{variants}(\sigma(\text{target})).\ \text{payload}(v) : F
}{
    \sigma \vdash \texttt{variants}(v) : F
} \quad \text{(C-Variants)}$$

**Comptime Constraint** `comptime expr`:

$$\frac{
    \Gamma_{ct}, \sigma \vdash e : \texttt{bool} \quad
    \text{eval}(e) = \texttt{true}
}{
    \sigma \vdash \texttt{comptime}\ e
} \quad \text{(C-Comptime)}$$

**Associated Type Computation**

Associated types in conditional implementations MAY be computed at compile time using `comptime` blocks:

```cursive
implement Container for Record { element: type E, .. }
where E : Sized
{
    type Element = E
    
    procedure size(~) -> usize {
        comptime { introspect~>layout::<E>().size }
    }
}
```

**Overlap Rules**

Conditional implementations MUST NOT overlap ambiguously. Two conditional implementations for the same form overlap if there exists a type $T$ that:

1. Matches both type patterns, AND
2. Satisfies both constraint sets.

Formally, implementations $I_1 = (P_1, C_1)$ and $I_2 = (P_2, C_2)$ overlap if:

$$\exists T.\ \text{Matches}(T, P_1) \land \text{Matches}(T, P_2) \land \text{Satisfies}(T, C_1) \land \text{Satisfies}(T, C_2)$$

Overlapping implementations MUST be detected at the point of declaration and diagnosed with `E-DRV-0433`.

**Specificity Ordering**

When multiple conditional implementations could apply but do not violate the overlap rules (due to mutually exclusive constraints), the implementation selects based on specificity:

1. Explicit implementations (`implement F for T`) are more specific than conditional implementations.
2. Conditional implementations with more specific type patterns are preferred.
3. Conditional implementations with more constraints are preferred.

If specificity does not determine a unique implementation, the implementations overlap and `E-DRV-0430` is emitted.

##### Dynamic Semantics

**Implementation Instantiation**

When a conditional implementation is selected for type $T$:

1. The type pattern bindings $\sigma$ are computed.
2. The implementation body is instantiated with $\sigma$.
3. Associated types are computed by evaluating their defining expressions.
4. The instantiated implementation is cached for $T$.

**Constraint Failure**

If a constraint fails during resolution:

1. The conditional implementation does not apply.
2. Resolution continues with remaining implementations.
3. No diagnostic is emitted for constraint failure alone (the type may have another implementation).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-DRV-0430` | Error    | Ambiguous conditional implementation    | Compile-time | Rejection |
| `E-DRV-0431` | Error    | Constraint not satisfiable              | Compile-time | Rejection |
| `E-DRV-0432` | Error    | Associated type computation failed      | Compile-time | Rejection |
| `E-DRV-0433` | Error    | Overlapping conditional implementations | Compile-time | Rejection |
| `E-DRV-0434` | Error    | Invalid constraint syntax               | Compile-time | Rejection |

---

### 19.6 Generation Contracts

##### Definition

A **generation contract** is a postcondition on a derive target or comptime procedure that constrains the emitted code. Generation contracts extend the contract system (§10) to compile-time code generation, enabling verification that derive targets produce expected outputs.

##### Syntax & Declaration

**Grammar**

```ebnf
generation_contract     ::= "|=" "emits" emit_constraint

emit_constraint         ::= emit_impl_constraint
                          | emit_property_constraint
                          | emit_universal_constraint

emit_impl_constraint    ::= "implement" form_ref "for" type_expr

emit_property_constraint ::= emit_constraint "." property_name

emit_universal_constraint ::= "forall" IDENTIFIER ":" type_expr "." expression
```

The `form_ref`, `type_expr`, `property_name`, and `expression` productions are defined in §9.1, §4, §2.5, and §11.1 respectively.

**Contract Forms**

| Contract Form                   | Meaning                                                    |
| :------------------------------ | :--------------------------------------------------------- |
| `emits implement Form for Type` | Exactly one implementation of `Form` for `Type` is emitted |
| `emits.methods_are_total`       | All emitted methods are provably total (no panics)         |
| `emits.size <= N`               | Layout of emitted types has size at most `N` bytes         |
| `forall x: T. P(x)`             | Universal property `P` holds for all values of type `T`    |

**Examples**

```cursive
// Contract: emits exactly one Debug implementation
[[derive_target]]
comptime procedure Debug(
    target: TypePattern,
    introspect: witness Introspect,
    emitter: witness ModuleEmitter,
)
    |= emits implement Debug for target
{
    // Implementation guaranteed to emit Debug for target
}

// Contract: emitted implementation has total methods
[[derive_target]]
comptime procedure SafeClone(
    target: TypePattern,
    introspect: witness Introspect,
    emitter: witness ModuleEmitter,
)
    |= emits implement Clone for target
    |= emits.methods_are_total
{
    // Implementation guaranteed to emit Clone with no panics
}
```

##### Static Semantics

**Contract Attachment**

Generation contracts are attached to derive target procedures and are verified after the procedure completes execution.

**Typing Rule for Generation Contract**

$$\frac{
    \Gamma_{ct} \vdash p : \text{ComptimeProc} \quad
    \text{HasContract}(p, C) \quad
    \text{Emitted}(p) \models C
}{
    p \text{ satisfies contract } C
} \quad \text{(T-Gen-Contract)}$$

**Contract Verification**

Generation contracts are verified at compile time after the derive target procedure completes:

1. Collect all AST nodes emitted by the procedure.
2. For each `|= emits` constraint, verify the constraint holds against the emitted AST.
3. If verification succeeds, continue compilation.
4. If verification fails, emit `E-DRV-0440` with the failing constraint.

**Implementation Emission Contract**

For `|= emits implement Form for Type`:

$$\frac{
    \text{Emitted}(p) = \{d_1, \ldots, d_n\} \quad
    |\{d_i \mid \text{IsImplement}(d_i, F, T)\}| = 1
}{
    \text{Emitted}(p) \models \texttt{emits implement } F \texttt{ for } T
} \quad \text{(V-Emit-Impl)}$$

The constraint requires exactly one implementation of the specified form for the specified type.

**Property Constraints**

Property constraints refine emission contracts with additional requirements:

| Property            | Verification Requirement                                  |
| :------------------ | :-------------------------------------------------------- |
| `methods_are_total` | All procedure bodies in emitted code are provably total   |
| `no_panics`         | Emitted code contains no panic paths                      |
| `size <= N`         | Emitted type definitions have layout size at most N bytes |
| `deterministic`     | Emitted code produces deterministic results               |

**Universal Constraints**

For `forall x: T. P(x)`:

$$\frac{
    \forall v : T.\ \Gamma_{ct}, x := v \vdash P(x) = \texttt{true}
}{
    \text{Emitted}(p) \models \texttt{forall } x: T.\ P(x)
} \quad \text{(V-Universal)}$$

Universal constraints assert properties that hold for all values of a type.

**Verifiability**

Not all generation contracts are statically verifiable. The implementation MUST verify contracts involving:

1. Implementation emission counts
2. Type layout bounds
3. Simple structural properties

The implementation MAY:

1. Defer verification of complex universal properties to runtime (with `[[dynamic]]` annotation).
2. Conservatively reject contracts it cannot verify, emitting `E-DRV-0441`.

##### Dynamic Semantics

**Verification Timing**

Contract verification occurs:

1. After the derive target procedure returns normally.
2. Before emitted code is added to the module.

If verification fails, the emitted code is discarded and compilation fails.

**Deferred Verification**

For contracts that cannot be fully verified statically, the implementation MAY generate runtime checks:

```cursive
[[derive_target]]
comptime procedure BoundedSerialize(
    target: TypePattern,
    introspect: witness Introspect,
    emitter: witness ModuleEmitter,
    max_size: usize = 1024,
)
    |= [[dynamic]] emits.serialized_size <= max_size
{
    // Runtime check inserted if static verification is not possible
}
```

The `[[dynamic]]` annotation on a contract clause permits runtime verification with `W-DRV-0440`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-DRV-0440` | Error    | Generation contract violated                            | Compile-time | Rejection |
| `E-DRV-0441` | Error    | Generation contract not statically verifiable           | Compile-time | Rejection |
| `W-DRV-0440` | Warning  | Generation contract requires `[[dynamic]]` verification | Compile-time | Continue  |
| `E-DRV-0442` | Error    | Invalid generation contract syntax                      | Compile-time | Rejection |
| `E-DRV-0443` | Error    | Contract references undefined type or form              | Compile-time | Rejection |

---

### 19.7 Standard Derive Targets

##### Definition

This section specifies the derive targets that conforming implementations MUST provide as part of the standard library.

##### Static Semantics

**Required Derive Targets**

Conforming implementations MUST provide the following derive targets:

| Derive Target | Form Implemented | Applicable Types                                       |
| :------------ | :--------------- | :----------------------------------------------------- |
| `Debug`       | `Debug`          | Records, Enums, Modal types                            |
| `Clone`       | `Clone`          | Records, Enums where fields/variants implement `Clone` |
| `Copy`        | `Copy`           | Records, Enums where fields/variants implement `Copy`  |
| `Eq`          | `Eq`             | Records, Enums where fields/variants implement `Eq`    |
| `Ord`         | `Ord`            | Records, Enums where fields/variants implement `Ord`   |
| `Hash`        | `Hash`           | Records, Enums where fields/variants implement `Hash`  |
| `Default`     | `Default`        | Records where all fields implement `Default`           |

**Debug Derive**

The `Debug` derive target generates a human-readable string representation:

```cursive
[[derive(Debug)]]
record Point { x: i32, y: i32 }

// Generates:
implement Debug for Point {
    procedure debug(~) -> string {
        "Point { x: " + self.x~>debug() + ", y: " + self.y~>debug() + " }"
    }
}
```

**Clone Derive**

The `Clone` derive target generates a deep copy implementation:

```cursive
[[derive(Clone)]]
record Point { x: i32, y: i32 }

// Generates:
implement Clone for Point {
    procedure clone(~) -> Point {
        Point { x: self.x~>clone(), y: self.y~>clone() }
    }
}
```

**Eq Derive**

The `Eq` derive target generates structural equality:

```cursive
[[derive(Eq)]]
record Point { x: i32, y: i32 }

// Generates:
implement Eq for Point {
    procedure eq(~, other: const Point) -> bool {
        self.x == other.x and self.y == other.y
    }
}
```

**Enum Derive Behavior**

For enums, derive targets generate match expressions over all variants:

```cursive
[[derive(Debug)]]
enum Option<T> {
    Some(T),
    None,
}

// Generates:
implement<T> Debug for Option<T>
where T: Debug
{
    procedure debug(~) -> string {
        match self {
            Some(v) => "Some(" + v~>debug() + ")",
            None => "None",
        }
    }
}
```

##### Constraints & Legality

**Applicability Constraints**

Each standard derive target has applicability constraints:

| Derive Target | Constraint                                                |
| :------------ | :-------------------------------------------------------- |
| `Clone`       | All fields/variants must implement `Clone`                |
| `Copy`        | All fields/variants must implement `Copy`; no `Drop` impl |
| `Eq`          | All fields/variants must implement `Eq`                   |
| `Ord`         | All fields/variants must implement `Ord`                  |
| `Hash`        | All fields/variants must implement `Hash`                 |
| `Default`     | All fields must implement `Default`                       |

Violation of these constraints results in `E-DRV-0421` (derive emitted invalid code) because the generated code fails type checking.

---

### 19.8 Summary of Diagnostic Codes

This section provides a consolidated reference of all diagnostic codes defined in this clause.

| Code         | Severity | Section | Condition                                               |
| :----------- | :------- | :------ | :------------------------------------------------------ |
| `E-DRV-0400` | Error    | §19.2   | Derive attribute on non-type declaration                |
| `E-DRV-0401` | Error    | §19.2   | Duplicate derive item in same attribute                 |
| `E-DRV-0402` | Error    | §19.2   | Invalid derive configuration syntax                     |
| `E-DRV-0410` | Error    | §19.3   | Derive target not found                                 |
| `E-DRV-0411` | Error    | §19.3   | Derive target has incorrect signature                   |
| `E-DRV-0412` | Error    | §19.3   | Derive target not visible from declaration scope        |
| `E-DRV-0413` | Error    | §19.3   | Derive target is not a comptime procedure               |
| `E-DRV-0414` | Error    | §19.3   | Derive target missing `[[derive_target]]` attribute     |
| `E-DRV-0420` | Error    | §19.4   | Derive procedure panicked                               |
| `E-DRV-0421` | Error    | §19.4   | Derive procedure emitted invalid code                   |
| `E-DRV-0422` | Error    | §19.4   | Invalid derive configuration argument                   |
| `E-DRV-0423` | Error    | §19.4   | Missing required configuration argument                 |
| `E-DRV-0424` | Error    | §19.4   | Unknown configuration key                               |
| `E-DRV-0430` | Error    | §19.5   | Ambiguous conditional implementation                    |
| `E-DRV-0431` | Error    | §19.5   | Constraint not satisfiable                              |
| `E-DRV-0432` | Error    | §19.5   | Associated type computation failed                      |
| `E-DRV-0433` | Error    | §19.5   | Overlapping conditional implementations                 |
| `E-DRV-0434` | Error    | §19.5   | Invalid constraint syntax                               |
| `E-DRV-0440` | Error    | §19.6   | Generation contract violated                            |
| `E-DRV-0441` | Error    | §19.6   | Generation contract not statically verifiable           |
| `W-DRV-0440` | Warning  | §19.6   | Generation contract requires `[[dynamic]]` verification |
| `E-DRV-0442` | Error    | §19.6   | Invalid generation contract syntax                      |
| `E-DRV-0443` | Error    | §19.6   | Contract references undefined type or form              |


---


# Part 6: Interoperability and ABI

## Clause 20: Interoperability

This clause defines the Foreign Function Interface (FFI) mechanisms for interacting with code adhering to C-compatible Application Binary Interfaces (ABIs). It specifies extern declaration syntax, the safety boundary enforced by `unsafe` blocks, memory layout guarantees via representation attributes, and the strict categorization of types permitted across the language boundary.

### 17.1 Foundational Definitions

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

### 17.2 FFI-Safe Types

##### Definition

**FFI-Safe Type**

A type $T$ is **FFI-Safe** if and only if the predicate $\text{IsFFISafe}(T)$ holds. FFI-Safe types have well-defined representation compatible with C ABIs.

**Formal Definition**

The predicate $\text{IsFFISafe}(T)$ is defined inductively:

$$
\text{IsFFISafe}(T) \iff T \in \mathcal{F}
$$

Where $\mathcal{F}$ is the smallest set satisfying:

1. **Primitive Integers**: $\mathcal{T}_{\text{int}} \subseteq \mathcal{F}$ (as defined in §5.1)

2. **Floating Point**: $\mathcal{T}_{\text{float}} \subseteq \mathcal{F}$ (as defined in §5.1)

3. **Unit Type**: `()` $\in \mathcal{F}$ (maps to `void` in return position)

4. **Raw Pointers**: If $\text{IsFFISafe}(U)$ or $U = ()$, then `*imm U` $\in \mathcal{F}$ and `*mut U` $\in \mathcal{F}$

5. **Extern Function Pointers**: If all parameter types $P_i$ and return type $R$ satisfy $\text{IsFFISafe}$, then `extern "ABI" ($P_1$, ..., $P_n$) -> $R$` $\in \mathcal{F}$

6. **Repr-C Records**: If `record R` is annotated `[[layout(C)]]` and $\forall f \in \text{fields}(R), \text{IsFFISafe}(\text{type}(f))$, then $R \in \mathcal{F}$

7. **Repr-C Enums**: If `enum E` is annotated `[[layout(C)]]` or `[[layout(IntType)]]`, then $E \in \mathcal{F}$

Types not satisfying the $\text{IsFFISafe}$ predicate above are excluded from FFI signatures. Common exclusions include: permission-qualified references, slices, non-repr-C strings, closures, unmonomorphized generics, and form witnesses.

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

### 17.3 Type Representation

##### Definition

**Default Layout**

For any `record` or `enum` **not** annotated with `[[layout(C)]]`, the memory layout follows the Universal Layout Principles defined in Clause 5 introduction.

**C-Compatible Layout**

The `[[layout(...)]]` attribute controls memory representation for FFI interoperability. The complete definition of layout attribute syntax, semantics, and constraints is provided in §7.2.1 (The `[[layout(...)]]` Attribute).

##### Static Semantics

For FFI purposes, the key layout requirements are:

| Declaration   | `[[layout(C)]]` Effect                         | Reference |
| :------------ | :--------------------------------------------- | :-------- |
| `record`      | C struct layout (declaration-order fields)     | §7.2.1    |
| `enum`        | C union with integer discriminant              | §7.2.1    |
| `modal`       | **Forbidden** (E-FFI-3303)                     | §7.2.1    |
| Generic types | **Forbidden** until monomorphized (E-FFI-3303) | §7.2.1    |

##### Constraints & Legality

| Code         | Severity | Condition                                                       |
| :----------- | :------- | :-------------------------------------------------------------- |
| `E-FFI-3303` | Error    | Invalid `[[layout(C)]]` on modal, generic, or unsupported type. |

---

### 17.4 Extern Declarations

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

> **Cross-Reference:** FFI attributes are registered in the Attribute Registry (§7.2.6). The table below summarizes their application in extern declarations.

| Attribute              | Target      | Effect                                              |
| :--------------------- | :---------- | :-------------------------------------------------- |
| `[[link_name("sym")]]` | Extern proc | Overrides linker symbol name                        |
| `[[no_mangle]]`        | Extern proc | Disables name mangling (implicit for `extern "C"`)  |
| `[[unwind(mode)]]`     | Extern proc | Controls panic behavior at FFI boundary (see §18.5) |

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
3. Signature must satisfy FFI-Safety requirements (§18.2).

| Code         | Severity | Condition                                       |
| :----------- | :------- | :---------------------------------------------- |
| `E-FFI-3301` | Error    | Non-FFI-Safe type in `extern` signature.        |
| `E-FFI-3304` | Error    | Variadic arguments (`...`) are not supported.   |
| `E-FFI-3306` | Error    | Exported extern procedure is not `public`.      |
| `E-FFI-3307` | Error    | Exported extern procedure captures environment. |

---

### 17.5 FFI Safety Boundary

##### Definition

Invoking an imported `extern` procedure is classified as Unverifiable Behavior (UVB) as defined in §1.2. The Cursive compiler cannot verify that foreign code upholds memory safety, type safety, or concurrency guarantees.

##### Static Semantics

**Unsafe Requirement for FFI Calls**

A call to an imported `extern` procedure **MUST** occur within an `unsafe` block (§3.8):

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
    *   **Maximal Munch Exception**: Generic argument exception per §2.7 (context-sensitive lexing)
    
  <u>Constraints & Legality</u>
    *   Keywords are reserved per §2.6
    *   Receiver shorthands (`~`, `~%`, `~!`) MUST appear only as first parameter in type method declarations
    *   All lexical rules MUST conform to preprocessing pipeline output (§2.1)
  
  <u>Examples</u>
    Complete ANTLR4 grammar suitable for parser generators. Includes all production rules from top-level `sourceFile` to terminal tokens.

## Appendix B: Diagnostics (Normative)

This appendix defines the normative taxonomy for compiler diagnostics and the authoritative allocation of diagnostic code ranges. All conforming implementations **MUST** use these codes when reporting the corresponding conditions to ensure consistent error reporting across toolchains.

#### B.1 Diagnostic Code Format

Diagnostic codes follow the format `K-CAT-FFNN`:

*   **K (Kind/Severity):**
    *   `E`: **Error**. A violation of a normative requirement. Compilation cannot proceed to codegen.
    *   `W`: **Warning**. Code is well-formed but contains potential issues (e.g., deprecated usage).
    *   `N`: **Note**. Informational message attached to an error or warning.
*   **CAT (Category):** A three-letter code identifying the language subsystem.
*   **FF (Feature Bucket):** Two digits identifying the specific feature area or chapter within the category.
*   **NN (Number):** Two digits uniquely identifying the specific condition within the bucket.

#### B.2 Code Range Allocation

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
| `W-DYN-`      | 0001–0099     | Clause 7: `[[dynamic]]` Attribute       |
| `P-`          | (same ranges) | Runtime Panics (use domain prefix)      |
| `W-`          | (same ranges) | Warnings (use domain prefix)            |

#### B.3 Reserved Ranges

The following ranges are reserved for future specification expansion:

| Range     | Reserved For            |
| :-------- | :---------------------- |
| 2500–2899 | Clauses 10–12           |
| 4000–4999 | Standard Library        |
| 5000–5999 | Implementation-Specific |

#### B.4 Conflict Resolution

When a program violation is detectable by multiple specification subsystems (e.g., both the type system and the memory model), the **canonical diagnostic** is determined by the following precedence:

1. Type System diagnostics (`E-TYP-`) are canonical for permission and type violations
2. Memory Model diagnostics (`E-MEM-`) are canonical for lifetime and move violations
3. Source diagnostics (`E-SRC-`) are canonical for lexical violations

Implementations MAY emit secondary diagnostics in addition to the canonical diagnostic when doing so provides distinct information.

## Appendix C: Conformance Dossier Schema

This appendix defines the normative requirements for the Conformance Dossier. A conforming implementation MUST produce a dossier artifact when the `dossier` emission phase is active.

#### C.1 File Format
The dossier MUST be a valid JSON document encoded in UTF-8.

#### C.2 Required Information

The Conformance Dossier MUST include the following information:

**Metadata:**
- Compiler identifier (vendor name)
- Compiler version (semantic versioning)
- Target triple (architecture-vendor-os)
- Build timestamp

**Configuration:**
- Conformance mode (`strict` or `permissive`)
- List of enabled language feature flags

**Safety Report:**
- Count of `unsafe` blocks in the compiled program
- List of IFNDR instances (file path, line number, category)

**Implementation-Defined Behavior:**
- Pointer width (32 or 64 bits)
- Type layout map for primitive types (size and alignment)

**Implementation Limits:**
- Maximum recursion depth
- Maximum identifier length
- Maximum source file size

## Appendix D: Standard Form Catalog

This appendix provides normative definitions for foundational forms and system capability forms built into Cursive or its core library.

  <u>Definition</u>
    Normative definitions for foundational forms and system capability forms that are deeply integrated with language mechanics.
  
  <u>Syntax & Declaration</u>
    **Foundational Forms** (§D.1):
    *   `Drop`: `procedure drop(~!)` - RAII cleanup, compiler-invoked only
    *   `Copy`: Marker form for implicit bitwise duplication
    *   `Clone`: `procedure clone(~): Self` - explicit deep copy
    *   `Iterator`: `type Item; procedure next(~!): Self::Item | None` - iteration protocol for `loop ... in`


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
        -   `after(duration: Duration): Future<()>` (async timer, §15.6.1)
    *   `Reactor` (§15.11.1):
        -   `run<T, E>(async_val: Async<T, (), T, E>): T | E` - Drive async to completion
        -   `register(handle: IoHandle, interest: Interest): Token` - Register I/O source
        -   `poll_ready(token: Token): Readiness` - Check I/O readiness
    *   `Time`: Monotonic time access with `now(): Timestamp` procedure
  
  <u>Constraints & Legality</u>
    *   `Drop::drop` MUST NOT be called directly by user code (E-TRS-2920)
    *   `Copy` and `Drop` are mutually exclusive on same type (E-TRS-2921)
    *   `Copy` requires all fields implement `Copy` (E-TRS-2922)
    *   `HeapAllocator::alloc` MUST panic on OOM (never return null)
    *   Variadic implementations across all form types are prohibited
    *   Any type implementing `Copy` MUST also implement `Clone`; for `Copy` types, `clone()` performs a bitwise copy identical to the implicit copy operation (Copy-Implies-Clone)

  <u>Static Semantics</u>
    *   Drop behavior: See §3.6 (Deterministic Destruction) for complete semantics
    *   `Copy` types are duplicated (not moved) on assignment/parameter passing
    *   Static invalidation applies to aliases when Drop is invoked on owner
    *   Capability forms enable attenuation patterns (e.g., `with_quota` on HeapAllocator)

  <u>Dynamic Semantics</u>
    *   Drop execution and panic handling: See §3.6
    *   HeapAllocator failures cause panic (no null returns)
  
  <u>Examples</u>
    Complete form signatures must be provided for: Drop, Copy, Clone, HeapAllocator, FileSystem, Network, Time, System (which implements Time and provides `exit`, `get_env`).

## Appendix E: Core Library Specification

##### Definition

Minimal normative definitions for core types assumed to be available by the language without explicit import.

##### Syntax & Declaration

**Context Capability Record (Normative Definition)**

```cursive
record Context {
    // System capabilities
    fs: witness FileSystem,
    net: witness Network,
    sys: System,
    heap: witness HeapAllocator,

    // Async runtime (§15.11.1)
    reactor: Reactor,

    // Execution domains (§14.6)
    cpu: CpuDomainFactory,
    gpu: GpuDomainFactory | None,
    inline: InlineDomainFactory,
}
```

**Field Descriptions:**

| Field     | Type                       | Purpose                          |
| :-------- | :------------------------- | :------------------------------- |
| `fs`      | `witness FileSystem`       | Filesystem authority             |
| `net`     | `witness Network`          | Network authority                |
| `sys`     | `System`                   | System primitives (env, time)    |
| `heap`    | `witness HeapAllocator`    | Dynamic allocation               |
| `reactor` | `Reactor`                  | Async runtime (§15.11.1)         |
| `cpu`     | `CpuDomainFactory`         | CPU parallel execution (§14.6.1) |
| `gpu`     | `GpuDomainFactory \| None` | GPU execution (§14.6.2)          |
| `inline`  | `InlineDomainFactory`      | Inline execution (§14.6.3)       |

*(Note: `Option` and `Result` are removed. Optionality is handled via Union Types e.g., `T | None`, and failure via `T | Error`. Pointers handle nullability via `Ptr<T>@Null` states.)*

##### Constraints & Legality

*   `Context` field types MUST match the capability form witnesses defined in Appendix D
*   Core library types (`Context`, primitives) MUST be available in the universe scope (no import required)
*   `main` procedure MUST accept `Context` parameter as defined in §8.9

##### Static Semantics

*   All context capabilities are passed explicitly (no ambient authority)
  


## Appendix F: Behavior Classification Index (Normative)

This appendix provides an index of behaviors by classification. Definitions for each classification (Defined, IDB, USB, UVB) are in §1.2.

**F.1 Unverifiable Behavior (UVB) Instances**:
*   FFI Calls (§17)
*   Raw Pointer Dereference (§6.3)
*   Transmute Operations
*   Pointer Arithmetic

**F.2 Implementation-Defined Behavior (IDB) Instances**:
*   Type Layout (non-C)
*   Integer Overflow (Release)
*   Pointer Width
*   Resource Limits
*   Panic Abort Mechanism
*   Async State Layout (§15.3)
*   Async Discriminant Encoding (§15.3)
*   Async Cancellation I/O Behavior (§15.9)
*   Condition Wake Mechanism (§15.6.3)

### F.3 Unspecified Behavior (USB) Instances

*   **Map Iteration**: Order of iteration for hash-based collections.
*   **Padding Bytes**: The values of padding bytes in non-`[[layout(C)]]` records.

### F.4 Defined Runtime Panics

*   Integer Overflow (Checked Mode)
*   Array/Slice Bounds Check
*   **Dynamic Key Conflict** — occurs only in `[[dynamic]]` contexts where runtime key acquisition blocks indefinitely or detects deadlock (see §13.7)
*   **Contract Check Failure** — occurs in `[[dynamic]]` contexts when a runtime-checked contract predicate evaluates to false (`P-CON-2850`)
*   **Refinement Validation Failure** — occurs in `[[dynamic]]` contexts when a runtime-checked refinement predicate evaluates to false (`P-TYP-1953`)

## Appendix G: Implementation Limits (Normative)

This appendix enumerates the minimum guaranteed capacities defined in §1.4 that all conforming implementations MUST support.

**Minimum Guaranteed Limits**:
*   **Source Size**: 1 MiB
*   **Logical Lines**: 65,535
*   **Line Length**: 16,384 chars
*   **Nesting Depth**: 256
*   **Identifier Length**: 1,023 chars
*   **Parameters**: 255
*   **Fields**: 1,024

**Compile-Time Execution Limits**:

The minimum guaranteed capacities for compile-time execution are defined in §16.9. For reference:

*   **Comptime Recursion Depth**: 256 frames
*   **Comptime Evaluation Steps**: 1,000,000
*   **Comptime Memory Allocation**: 64 MiB
*   **AST Depth**: 1,024 levels
*   **Total Emitted Declarations**: 10,000

---

## Appendix H: Migration Guide for `[[dynamic]]` (Informative)

This appendix provides guidance for migrating code from the previous specification (which used `[[verify(...)]]` and implicit runtime synchronization) to the current specification.

### H.1 Key Differences

| Aspect             | Previous Specification           | Current Specification   |
| :----------------- | :------------------------------- | :---------------------- |
| Key system default | Runtime sync with static elision | Static proof required   |
| Contract default   | `[[verify(dynamic)]]` (runtime)  | Static proof required   |
| Refinement default | Runtime check                    | Static proof required   |
| Opt-in runtime     | N/A (was default)                | `[[dynamic]]` attribute |
| Trusted mode       | `[[verify(trusted)]]`            | **Removed**             |

### H.2 Migration Patterns

**Pattern 1: Implicit Runtime Key Synchronization**

Before:
```cursive
procedure update(data: shared [Record], idx: usize) {
    data[idx].process()    // Implicit runtime sync
}
```

After:
```cursive
// Option A: Add [[dynamic]] to permit runtime sync
[[dynamic]]
procedure update(data: shared [Record], idx: usize) {
    data[idx].process()
}

// Option B: Restructure to enable static proof
procedure update(data: shared [Record], idx: usize)
    |= idx < data.len()
{
    // With precondition, may be statically provable in some contexts
    data[idx].process()
}
```

**Pattern 2: Dynamic Contract Verification**

Before:
```cursive
procedure sqrt(x: f64) -> f64
    |= x >= 0.0
{
    // Runtime check on precondition by default
}
```

After:
```cursive
// Static proof now required at call sites
// Callers must prove x >= 0.0 or use [[dynamic]]

// Option A: Caller proves statically
if x >= 0.0 {
    sqrt(x)    // Provable
}

// Option B: Caller opts into runtime check
[[dynamic]] sqrt(x)
```

**Pattern 3: `[[verify(static)]]`**

Before:
```cursive
[[verify(static)]]
procedure critical(x: i32)
    |= x > 0
{ ... }
```

After:
```cursive
// [[verify(static)]] is now default; remove attribute
procedure critical(x: i32)
    |= x > 0
{ ... }
```

**Pattern 4: `[[verify(trusted)]]`**

Before:
```cursive
unsafe {
    [[verify(trusted)]]
    let ptr = external_alloc(size)
    // Assumed: ptr != null
}
```

After:
```cursive
// Option A: Remove the contract entirely
unsafe {
    let ptr = external_alloc(size)
    // Document assumption in comment: ptr is assumed non-null
}

// Option B: Use [[dynamic]] if runtime check is acceptable
unsafe {
    let ptr = [[dynamic]] external_alloc_checked(size)
    // Runtime check: panics if null
}
```

### H.3 Compiler Migration Assistance

Implementations SHOULD provide a migration flag that:

1. **`--warn-implicit-dynamic`**: Identifies all locations that would require `[[dynamic]]` under the new specification
2. **`--migrate-dynamic`**: Automatically inserts `[[dynamic]]` at required locations

Example workflow:

```bash
# Step 1: Identify required changes
cursive build --warn-implicit-dynamic

# Step 2: Review warnings and decide:
#   - Add [[dynamic]] where runtime cost is acceptable
#   - Restructure code to enable static proof where possible
#   - Remove contracts that were only documentation (use comments instead)

# Step 3: Build with new semantics
cursive build
```
