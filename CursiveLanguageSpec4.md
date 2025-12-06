## Clause 0: Preliminaries

### 0.1 Glossary of Terms

##### Definition

The following terms have precise meanings in this specification:

**Lexeme**: The character sequence matched by a lexical production during tokenization. A lexeme is the concrete syntactic representation of a token in source text.

**Linear** (lattice theory): A lattice in which every pair of elements is comparable; also called "totally ordered." A linear lattice has exactly one path between top and bottom.

**Nominal Equivalence**: Two types are nominally equivalent if and only if they refer to the same declaration. Records and enums use nominal equivalence.

**Payload**: Associated data carried by an enum variant or union member. A variant may have no payload (unit-like), a tuple payload (positional fields), or a record payload (named fields).

**Place Expression**: An expression that denotes a memory location (also known as an l-value). Place expressions include:
- Identifiers bound to variables (`x`)
- Field access (`x.field`)
- Indexed access (`x[i]`)
- Dereference (`*ptr`)

Place expressions may appear on the left-hand side of assignments and as operands of address-of (`&`) and move expressions.

**Structural Equivalence**: Two types are structurally equivalent if their component types are equivalent and in the same order. Tuples, arrays, slices, and union types use structural equivalence.

---

### 0.2 Notation and Symbols

##### Definition

**Typing Judgment Notation**

Typing rules use the following notation:

$$\Gamma \vdash e : T \dashv \Gamma'$$

where:
- $\Gamma$ (input context, read "under context Gamma"): The typing context before evaluating $e$, containing bindings and their states.
- $\vdash$ (turnstile): Separates the context from the judgment; read as "entails" or "proves."
- $e : T$: The expression $e$ has type $T$.
- $\dashv$ (reverse turnstile): Separates the judgment from the output context; indicates "producing."
- $\Gamma'$ (output context): The typing context after evaluating $e$, reflecting any state changes (e.g., moved bindings).

When only the input context matters (no state change), the simpler form $\Gamma \vdash e : T$ is used.

**Inference Rule Format**

Inference rules are written as:

$$\frac{\text{premise}_1 \quad \text{premise}_2 \quad \ldots}{\text{conclusion}}$$

The rule states: if all premises above the line hold, then the conclusion below the line holds.

---

## Clause 1: General Principles and Conformance

### 1.1 Conformance Obligations

##### Definition

A **conforming implementation** is a translator (compiler, interpreter, or hybrid execution engine) that satisfies all normative requirements set forth in this specification.

A **conforming program** is a source program that satisfies all syntactic and static-semantic constraints defined in this specification, and whose execution semantics rely solely on Defined Behavior or Implementation-Defined Behavior.

##### Static Semantics

**Implementation Requirements**

A conforming implementation MUST:

1.  Accept every well-formed program and translate it to an executable representation that exhibits the semantics defined in this specification.
2.  Reject every ill-formed program (except those classified as IFNDR per §1.2.1) and issue at least one diagnostic message identifying the violation.
3.  Produce a **Conformance Dossier** as a mandatory build artifact. The dossier MUST be a machine-readable JSON document conforming to the schema defined in Appendix F.
4.  Document every choice made for behaviors classified as Implementation-Defined Behavior (IDB) within the Conformance Dossier.
5.  Record all instances of Ill-Formed, No Diagnostic Required (IFNDR) conditions detected during compilation in the Conformance Dossier.

**Program Requirements**

A program is **well-formed** if and only if it satisfies all of the following:

1.  It complies with the lexical grammar and constraints defined in Clause 2.
2.  It complies with the syntactic grammar defined throughout this specification.
3.  It satisfies all static-semantic typing judgments, permission checks, and safety analysis rules defined in this specification.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                            | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0101` | Error    | Program is ill-formed due to syntactic or static-semantic violation. | Compile-time | Rejection |
| `E-CNF-0102` | Error    | Generation of the Conformance Dossier failed.                        | Compile-time | Rejection |

---

### 1.2 Behavior Classifications

##### Definition

The execution semantics of Cursive programs are partitioned into four mutually exclusive categories.

**Defined Behavior** is behavior for which this specification prescribes exactly one permitted outcome. A conforming implementation MUST produce this outcome for all well-formed programs exercising such behavior.

**Implementation-Defined Behavior (IDB)** is behavior for which this specification permits a finite set of valid outcomes and requires the implementation to select and document one specific outcome. The implementation MUST maintain consistency of the chosen outcome across all compilations sharing the same configuration.

**Unspecified Behavior (USB)** is behavior for which this specification permits a set of valid outcomes but does not require the implementation to document the choice. The implementation MAY select any outcome from the permitted set. The choice MAY vary between executions, compilations, or optimization levels. USB MUST NOT violate memory safety or data integrity invariants.

**Unverifiable Behavior (UVB)** is behavior unrelated to the abstract machine defined by this specification. UVB occurs when the program violates invariants that are not enforced by the language semantics (e.g., within `unsafe` blocks).

##### Static Semantics

**Safety Invariant**

For any well-formed program execution that does not enter an `unsafe` block or invoke Foreign Function Interface (FFI) procedures, the program MUST NOT exhibit Unverifiable Behavior.

**IDB Documentation Mandate**

The implementation MUST list all IDB choices in the Conformance Dossier. The complete index of IDB instances is defined in Appendix I.

---

#### 1.2.1 Ill-Formed, No Diagnostic Required (IFNDR)

##### Definition

A program is **Ill-Formed, No Diagnostic Required (IFNDR)** if it violates a static-semantic rule for which this specification explicitly waives the diagnostic requirement.

##### Static Semantics

**Classification Criteria**

A violation is classified as IFNDR if and only if detecting the violation requires:
1.  Solving a problem known to be undecidable (e.g., the Halting Problem).
2.  Analysis complexity exceeding polynomial time relative to the program size.
3.  Knowledge of runtime values that cannot be statically determined via constant propagation.

**Semantic Status**

The execution of a program containing an IFNDR violation constitutes **Unspecified Behavior (USB)**. The implementation MAY:
1.  Reject the program with a diagnostic.
2.  Compile the program such that it panics deterministically at the point of violation.
3.  Compile the program such that it exhibits arbitrary behavior, provided that the Safety Invariant (§1.2) is maintained.

**Reporting**

If an implementation detects an IFNDR violation, it MUST record the violation instance (file path, line number, and error category) in the Conformance Dossier.

---

### 1.3 Reserved Identifiers

##### Definition

A **reserved identifier** is a character sequence that is syntactically valid as an identifier but is reserved for use by the specification or the implementation.

##### Static Semantics

**Reserved Keywords**

The set of identifiers listed in §2.6 (Keywords) are reserved. They MUST NOT be used as user-defined identifiers.

**Reserved Namespaces**

Identifiers beginning with the sequence `cursive.` are reserved for the language core libraries. User declarations MUST NOT define modules or items within the `cursive` namespace.

**Synthetic Identifier Prefix**

Identifiers beginning with the prefix `gen_` are reserved for compiler-generated entities (e.g., anonymous modal types for async procedures). User declarations MUST NOT define identifiers beginning with `gen_`.

**Universe-Protected Bindings**

The following identifiers are declared in the implicit universe scope ($S_{universe}$) and MUST NOT be shadowed by declarations at module scope ($S_{module}$):

1.  **Primitive Types:** `i8`, `i16`, `i32`, `i64`, `i128`, `u8`, `u16`, `u32`, `u64`, `u128`, `f16`, `f32`, `f64`, `bool`, `char`, `usize`, `isize`.
2.  **Special Types:** `Self`, `string`, `Modal`.
3.  **Async Type Aliases:** `Async`, `Future`, `Sequence`, `Stream`, `Pipe`, `Exchange`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                  | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0401` | Error    | Reserved keyword used as identifier.                       | Compile-time | Rejection |
| `E-CNF-0402` | Error    | User declaration in `cursive.*` namespace.                 | Compile-time | Rejection |
| `E-CNF-0403` | Error    | Primitive type name shadowed at module scope.              | Compile-time | Rejection |
| `E-CNF-0404` | Error    | Shadowing of `Self`, `string`, or `Modal`.                 | Compile-time | Rejection |
| `E-CNF-0405` | Error    | Shadowing of async type aliases (`Async`, `Future`, etc.). | Compile-time | Rejection |
| `E-CNF-0406` | Error    | User declaration uses `gen_` prefix.                       | Compile-time | Rejection |
| `W-CNF-0401` | Warning  | Implementation-reserved pattern used.                      | Compile-time | N/A       |

---

### 1.4 Implementation Limits

##### Definition

**Implementation limits** are the bounded capacities of the translator.

##### Static Semantics

**Guaranteed Minimums**

A conforming implementation MUST support at least the following capacities. Programs exceeding these limits are ill-formed.

1.  **Source File Size:** 1 MiB (1,048,576 bytes).
2.  **Logical Lines per File:** 65,535 lines.
3.  **Characters per Logical Line:** 16,384 characters.
4.  **Syntactic Nesting Depth:** 256 levels (blocks and expressions combined).
5.  **Identifier Length:** 1,023 characters.
6.  **Parameters per Procedure:** 255 parameters.
7.  **Fields per Record:** 1,024 fields.

**Compile-Time Execution Limits**

Implementations MUST support the compile-time execution capacities defined in §17.8.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                      | Detection    | Effect    |
| :----------- | :------- | :----------------------------- | :----------- | :-------- |
| `E-CNF-0301` | Error    | Implementation limit exceeded. | Compile-time | Rejection |

---

### 1.5 Language Evolution

##### Definition

**Language evolution** defines the mechanisms for versioning, feature lifecycle management, and extension.

##### Static Semantics

**Versioning**

The language version MUST be identified by a Semantic Versioning 2.0.0 triple (`MAJOR.MINOR.PATCH`).

**Compatibility**

1.  An implementation MUST reject a program if the program's declared `MAJOR` version differs from the implementation's supported `MAJOR` version.
2.  An implementation MUST accept a program if the program's declared version is less than or equal to the implementation's version within the same `MAJOR` series.

**Feature Stability**

Every language feature MUST be assigned exactly one stability class:

| Class          | Requirements                    | Mutation Policy                       |
| :------------- | :------------------------------ | :------------------------------------ |
| `Stable`       | Enabled by default.             | Immutable within `MAJOR` version.     |
| `Preview`      | Requires explicit feature flag. | MAY change between `MINOR` versions.  |
| `Experimental` | Requires explicit feature flag. | MAY change or be removed at any time. |

**Deprecation**

1.  Usage of a deprecated feature MUST trigger a warning diagnostic.
2.  Deprecated features MUST NOT be removed until the next `MAJOR` version increment.
3.  Removal of a deprecated feature MUST result in an error diagnostic.

**Vendor Extensions**

Vendor-specific extensions MUST use identifiers within a reverse-domain-style namespace (e.g., `com.vendor.feature`). Extensions MUST NOT alter the semantics of conforming programs that do not use those extensions.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CNF-0601` | Error    | Program requires unsupported `MAJOR` version. | Compile-time | Rejection |
| `E-CNF-0602` | Error    | Unknown or invalid feature flag.              | Compile-time | Rejection |
| `E-CNF-0603` | Error    | Feature usage requires disabled flag.         | Compile-time | Rejection |
| `E-CNF-0604` | Error    | Mixed editions within a compilation unit.     | Compile-time | Rejection |
| `E-CNF-0605` | Error    | Usage of removed feature.                     | Compile-time | Rejection |
| `W-CNF-0601` | Warning  | Usage of deprecated feature.                  | Compile-time | N/A       |

---

### 1.6 Foundational Semantic Concepts

##### Definition

**Program Point**

A **Program Point** is a unique location within the control flow graph of a procedure. It is formally defined as the tuple:

$$P = (\text{ProcedureID}, \text{NodeID})$$

where $\text{ProcedureID}$ uniquely identifies the procedure definition and $\text{NodeID}$ uniquely identifies a node in the control flow graph.

**Lexical Scope**

A **Lexical Scope** is a contiguous region of source text defining the visibility and lifetime of bindings. It is formally defined as the tuple:

$$S = (\text{Start}, \text{End}, \text{Parent})$$

where $\text{Start}$ and $\text{End}$ are the program points delimiting the scope, and $\text{Parent}$ is the immediately enclosing scope (or $\emptyset$ for the top-level module scope).

**Dominance**

A program point $A$ **dominates** program point $B$ (written $A\ \text{dom}\ B$) if and only if every path from the procedure entry to $B$ must pass through $A$.

---

## Clause 2: Lexical Structure and Source Text

### 2.1 Source Text Encoding

##### Definition

**Source Text** is a finite sequence of Unicode scalar values that constitutes the input to the translation pipeline. A **Source Byte Stream** is the raw sequence of octets read from an input source. A **Normalized Source File** is the sequence of Unicode scalar values produced by the preprocessing pipeline (§2.3).

**Formal Definition**

Let $\mathcal{U}$ denote the set of all Unicode scalar values. Let $\mathcal{F}$ denote the set of prohibited code points defined in the Static Semantics below. A valid source text $S$ is a sequence:

$$S \in \mathcal{U}^* \setminus \{s : \exists c \in s.\ c \in \mathcal{F}\}$$

##### Static Semantics

**UTF-8 Mandate**

The source byte stream MUST be encoded as UTF-8 (RFC 3629).

**Byte Order Mark (BOM)**

If the source byte stream begins with the UTF-8 Byte Order Mark (U+FEFF), the implementation MUST ignore it. A BOM appearing at any position other than the start of the stream is treated as a zero-width non-breaking space (U+FEFF) if permitted by context, or rejected if prohibited by specific lexical rules.

**Unicode Normalization**

Implementations MUST normalize identifier lexemes and module-path components to Unicode Normalization Form C (NFC) prior to equality comparison, hashing, or symbol lookup. Normalization of source text outside these constructs is Implementation-Defined Behavior (IDB).

Normalization MUST NOT alter logical line boundaries or the byte offsets reported in diagnostics.

**Prohibited Code Points**

The set of prohibited code points $\mathcal{F}$ consists of all characters with the Unicode General Category `Cc` (Control), excluding the permitted whitespace controls:

$$\mathcal{F} = \{c : \text{GeneralCategory}(c) = \texttt{Cc}\} \setminus \{\text{U+0009}, \text{U+000A}, \text{U+000C}, \text{U+000D}\}$$

Prohibited code points MUST NOT appear in source text outside of string and character literals.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0101` | Error    | Invalid UTF-8 byte sequence.                 | Compile-time | Rejection |
| `E-SRC-0103` | Error    | Embedded BOM found after the first position. | Compile-time | Rejection |
| `E-SRC-0104` | Error    | Forbidden control character or null byte.    | Compile-time | Rejection |
| `W-SRC-0101` | Warning  | UTF-8 BOM present at start of file.          | Compile-time | N/A       |

---

### 2.2 Source File Structure

##### Definition

A **Logical Line** is a sequence of Unicode scalar values terminated by a line feed (U+000A) or the end of the input. **Line Ending Normalization** is the transformation of platform-specific line terminators into canonical line feeds.

**Formal Definition**

$$L ::= c^* \cdot (\text{U+000A})? \quad \text{where } c \in \mathcal{U} \setminus (\mathcal{F} \cup \{\text{U+000A}\})$$

A source file is a sequence of logical lines: $S = L_1 \cdot L_2 \cdot \ldots \cdot L_n$.

##### Syntax & Declaration

**Grammar**

```ebnf
source_file     ::= normalized_line*
normalized_line ::= code_point* line_terminator?
line_terminator ::= U+000A
code_point      ::= U - {U+000A} - F
```

##### Static Semantics

**Normalization Rules**

Implementations MUST normalize line endings prior to tokenization according to the following rules:

1. The sequence U+000D U+000A (CR LF) is replaced by a single U+000A (LF).
2. The character U+000D (CR) not followed by U+000A is replaced by a single U+000A (LF).
3. The character U+000A (LF) remains unchanged.

Mixed line endings within a single source file MUST be accepted and normalized.

##### Constraints & Legality

**Implementation Limits**

Implementations MUST enforce the limits defined in §1.4 regarding maximum source file size and logical line count.

**Diagnostic Table**

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-SRC-0102` | Error    | Source file exceeds maximum size.    | Compile-time | Rejection |
| `E-SRC-0105` | Error    | Maximum logical line count exceeded. | Compile-time | Rejection |

---

### 2.3 Preprocessing Pipeline

##### Definition

The **Preprocessing Pipeline** is the ordered sequence of transformations applied to a source byte stream to produce a token stream.

**Formal Definition**

$$\text{Preprocess} : \text{ByteStream} \to \text{TokenStream} \cup \{\bot\}$$

The pipeline is the composition:

$$\text{Preprocess} = \text{Tokenize} \circ \text{ValidateCtrl} \circ \text{NormLines} \circ \text{StripBOM} \circ \text{DecodeUTF8} \circ \text{CheckSize}$$

##### Dynamic Semantics

**Pipeline Execution**

The implementation MUST execute the following steps in order. Failure of any step terminates the pipeline.

1.  **Size Check**: Verify the input size does not exceed the implementation limit (§1.4). Violation triggers `E-SRC-0102`.
2.  **UTF-8 Decoding**: Decode the byte stream into Unicode scalar values. Invalid sequences trigger `E-SRC-0101`.
3.  **BOM Removal**: If the first scalar value is U+FEFF, discard it. If U+FEFF appears elsewhere, trigger `E-SRC-0103`.
4.  **Line Normalization**: Apply line ending normalization (§2.2).
5.  **Control Character Validation**: Check for prohibited code points (§2.1). Violation triggers `E-SRC-0104`.
6.  **Tokenization**: Decompose the normalized source into tokens (§2.4). Failure to classify a sequence triggers `E-SRC-0309`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-SRC-0309` | Error    | Tokenization failed to classify a character sequence. | Compile-time | Rejection |

---

### 2.4 Lexical Vocabulary

##### Definition

A **Token** is the atomic unit of the syntax.

**Formal Definition**

A token is a triple $t = (\kappa, \lambda, \sigma)$ where:
*   $\kappa \in \mathcal{T} = \{\texttt{identifier}, \texttt{keyword}, \texttt{literal}, \texttt{operator}, \texttt{punctuator}, \texttt{newline}\}$ is the token kind.
*   $\lambda \in \mathcal{U}^*$ is the lexeme (the character sequence).
*   $\sigma \in \text{Span}$ is the source location (file, start offset, end offset).

##### Static Semantics

**Classification**

Every maximal subsequence of the normalized source text that is not whitespace or a comment MUST correspond to exactly one token kind in $\mathcal{T}$.

**Determinism**

Tokenization MUST be deterministic for a given normalized source file.

---

### 2.5 Identifiers

##### Definition

An **Identifier** is a token that names a declaration, type, or binding.

**Formal Definition**

$$\text{Ident} = \{s \in \mathcal{U}^+ : s_0 \in \text{XID\_Start} \cup \{\_\} \land \forall i > 0.\, s_i \in \text{XID\_Continue} \cup \{\_\}\}$$

##### Syntax & Declaration

**Grammar**

```ebnf
identifier     ::= ident_start ident_continue*
ident_start    ::= <Unicode XID_Start> | "_"
ident_continue ::= <Unicode XID_Continue> | "_"
```

##### Static Semantics

**Exclusions**

Identifiers MUST NOT contain prohibited code points (§2.1), surrogate code points, or non-characters.

**Keyword Precedence**

A lexeme that matches a reserved keyword (§2.6) MUST be classified as a `<keyword>` token, not an `<identifier>`, unless appearing in a context that explicitly permits keywords as identifiers (e.g., field access).

**Equivalence**

Two identifiers are equivalent if and only if their NFC-normalized forms are identical.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-SRC-0307` | Error    | Invalid Unicode in identifier.               | Compile-time | Rejection |
| `E-CNF-0406` | Error    | User declaration uses reserved `gen_` prefix | Compile-time | Rejection |

---

### 2.6 Keywords

##### Definition

A **Keyword** is a reserved lexeme with special syntactic meaning.

##### Static Semantics

**Reserved List**

The following lexemes are reserved keywords. They MUST NOT be used as identifiers.

```text
and         as          async       atomic      break       class
comptime    const       continue    defer       dispatch    do
drop        else        emit        enum        escape
extern      false       for         gpu         if          import
in          interrupt   let         loop        match       modal
mod         module      move        mut         override    pool
private     procedure   protected   public      quote       record
region      result      return      select      self        Self
set         shared      simd        spawn       sync        then
transition  transmute   true        type        union       unique
unsafe      using       var         volatile    where       while
widen       yield
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-CNF-0401` | Error    | Reserved keyword used as identifier. | Compile-time | Rejection |

---

### 2.7 Operators and Punctuators

##### Definition

An **Operator** is a token denoting an operation. A **Punctuator** is a token used for syntactic structure.

##### Syntax & Declaration

**Lexical Productions**

```text
Operator    ::= + | - | * | / | % | ^ | & | | | ~ | ! | = | < | >
              | << | >> | == | != | <= | >= | && | || | -> | => | ..
              | ..= | ** | :: | := | += | -= | *= | /= | %= | &= | |=
              | ^= | <<= | >>= | ~> | ~! | ~% | r^ | ^

Punctuator  ::= ( | ) | [ | ] | { | } | , | : | ; | . | # | @ | ?
```

##### Static Semantics

**Maximal Munch**

The implementation MUST form the longest possible valid token at any given position.

**Generic Argument Exception**

If the sequence `>>` appears in a context where it would close two nested generic argument lists, it MUST be tokenized as two `>` tokens. Similarly, `>>>` MUST be tokenized as three `>` tokens.

---

### 2.8 Literals

##### Definition

A **Literal** is a token representing a fixed compile-time value.

##### Syntax & Declaration

**Grammar**

```ebnf
literal          ::= integer_lit | float_lit | string_lit | char_lit

integer_lit      ::= (dec_lit | hex_lit | oct_lit | bin_lit) [int_suffix]
dec_lit          ::= digit (digit | "_")*
hex_lit          ::= "0x" hex_digit (hex_digit | "_")*
oct_lit          ::= "0o" oct_digit (oct_digit | "_")*
bin_lit          ::= "0b" bin_digit (bin_digit | "_")*

float_lit        ::= dec_lit "." dec_lit [exponent] [float_suffix]
                   | dec_lit exponent [float_suffix]
exponent         ::= ("e" | "E") ["+" | "-"] dec_lit

string_lit       ::= '"' (string_content | escape)* '"'
char_lit         ::= "'" (char_content | escape) "'"

escape           ::= "\" ("n" | "r" | "t" | "\" | "'" | '"' | "0")
                   | "\x" hex_digit hex_digit
                   | "\u{" hex_digit+ "}"
```

##### Static Semantics

**Numeric Literals**

1.  Underscores MUST NOT appear at the start, end, immediately after a radix prefix, adjacent to `e`/`E`, or before a type suffix.
2.  Based literals (`0x`, `0o`, `0b`) MUST contain at least one digit.
3.  Type suffixes (`u8`, `i32`, `f64`, etc.) MUST match a valid primitive type.

**String Literals**

1.  MUST be delimited by double quotes (`"`).
2.  MUST NOT contain unescaped double quotes or line feeds.
3.  Escape sequences MUST be valid per the grammar.

**Character Literals**

1.  MUST be delimited by single quotes (`'`).
2.  MUST contain exactly one Unicode scalar value.
3.  MUST NOT contain unescaped single quotes or line feeds.

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

**Whitespace** is any sequence of characters that separates tokens but has no semantic meaning. **Comments** are regions of text ignored by the tokenizer.

**Formal Definition**

$$\mathcal{W} = \{\text{U+0020}, \text{U+0009}, \text{U+000C}, \text{U+000A}\}$$

##### Static Semantics

**Whitespace Behavior**

Whitespace characters separate tokens. The Line Feed character (U+000A) is emitted as a `<newline>` token to support statement termination (§2.11). All other whitespace characters are discarded.

**Comment Syntax**

1.  **Line Comment**: Begins with `//` and extends to the next U+000A or end-of-file.
2.  **Block Comment**: Begins with `/*` and ends with `*/`. Block comments nest recursively.
3.  **Doc Comment**: Line comments beginning with `///` or `//!`. These are preserved for documentation generation but are treated as whitespace for tokenization.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                   | Detection    | Effect    |
| :----------- | :------- | :-------------------------- | :----------- | :-------- |
| `E-SRC-0306` | Error    | Unterminated block comment. | Compile-time | Rejection |

---

### 2.10 Lexical Security

##### Definition

**Lexically Sensitive Characters** are Unicode code points that may alter visual appearance without affecting tokenization, potentially creating homoglyph attacks or bidi spoofing.

**Formal Definition**

$$\mathcal{S} = \{\text{U+202A}..\text{U+202E}, \text{U+2066}..\text{U+2069}, \text{U+200C}, \text{U+200D}\}$$

##### Static Semantics

**Enforcement Modes**

Implementations MUST provide a configuration to select the enforcement mode:

1.  **Permissive Mode**: Usage of characters in $\mathcal{S}$ outside of strings/comments triggers warning `W-SRC-0308`.
2.  **Strict Mode**: Usage of characters in $\mathcal{S}$ outside of strings/comments triggers error `E-SRC-0308`.

The default mode is Implementation-Defined (IDB).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `W-SRC-0308` | Warning  | Lexically sensitive character (Permissive). | Compile-time | N/A       |
| `E-SRC-0308` | Error    | Lexically sensitive character (Strict).     | Compile-time | Rejection |

---

### 2.11 Statement Termination

##### Definition

A **Statement Terminator** is a token that signifies the end of a statement. Cursive supports both explicit semicolons and implicit termination via newlines.

##### Static Semantics

**Termination Rules**

A statement is terminated by:
1.  A semicolon (`;`) punctuator.
2.  A `<newline>` token, **UNLESS** the line continuation condition holds.

**Line Continuation Condition**

A newline does NOT terminate a statement if:
1.  The newline is enclosed within matching delimiters `()`, `[]`, or `{}`.
2.  The last token before the newline is a binary operator or comma.
3.  The first token after the newline is a dot (`.`), scope resolution (`::`), or chain operator (`~>`).

**Ambiguity Resolution**

Tokens that function as both unary and binary operators (e.g., `-`, `*`) appearing at the end of a line are treated as binary operators, triggering line continuation.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                              | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------------------- | :----------- | :-------- |
| `E-SYN-0110` | Error    | Missing statement terminator.                                          | Compile-time | Rejection |
| `E-SYN-0111` | Error    | Invalid line continuation (newline required to terminate but ignored). | Compile-time | Rejection |

---

### 2.12 Translation Phases

##### Definition

The **Translation Pipeline** is the ordered sequence of transformations applied to a **Compilation Unit** (§8.1) to produce an executable representation. The pipeline adheres to a **Staged Metaprogramming Model** to ensure determinism and prevent causality paradoxes during compile-time execution.

**Formal Definition**

$$\text{Translate} : \text{CompilationUnit} \to \text{Executable} \cup \{\bot\}$$

The pipeline is the sequential composition of five phases, where $\bot$ indicates failure:

$$\text{Translate} = \text{Parse} \triangleright \text{MetaGen} \triangleright \text{MetaAnalyze} \triangleright \text{TypeCheck} \triangleright \text{CodeGen}$$

where $f \triangleright g$ denotes "apply $f$, then apply $g$". This sequence is strict; phase $N$ MUST complete successfully for all modules in the compilation unit before phase $N+1$ begins.

##### Dynamic Semantics

**Phase 1: Parsing**

The implementation transforms the Token Stream (§2.4) into an Abstract Syntax Tree (AST).
*   **Input:** Normalized Source Files (§2.2).
*   **Output:** Initial AST.
*   **Validation:** Syntactic well-formedness.

**Phase 2: Metaprogramming — Generation**

The implementation executes compile-time logic that modifies the AST. This phase is the *only* phase where the AST structure is mutable.
*   **Scope:** Executes `derive` targets (§20.2) and `comptime` blocks annotated with `[[emit]]`.
*   **Capabilities:**
    *   `TypeEmitter` (§17.5.1): **Available**. Code emission is permitted.
    *   `Introspect` (§17.5.2): **Restricted**. Introspection is limited to the `target` type of a derive macro or types defined in fully resolved dependencies. Global introspection is prohibited to prevent observation of incomplete types.
*   **Ordering:** Execution order is determined by the Derive Dependency Graph (§20.4).
*   **Output:** Expanded AST.

**Phase 3: Metaprogramming — Analysis**

The implementation executes compile-time logic that inspects the AST for validation, static assertions, or computed constants.
*   **Scope:** Executes `comptime` blocks *not* annotated with `[[emit]]`, `comptime` assertions, and `comptime` procedure calls in value positions.
*   **Capabilities:**
    *   `TypeEmitter` (§17.5.1): **Revoked**. Calling `emit` or `acquire_write_key` in this phase constitutes a dynamic capability violation (`E-CTE-0040`).
    *   `Introspect` (§17.5.2): **Available**. The AST is frozen; full reflection is safe and deterministic.
*   **Output:** Validated AST and computed constant values.

**Phase 4: Type Checking**

The implementation performs full semantic analysis, type inference, and safety verification.
*   **Scope:** All declarations and bodies.
*   **Validation:**
    *   Name resolution (§8.7).
    *   Type checking (§4).
    *   Permission analysis (§4.5).
    *   Lifetime and borrow analysis (§3).
    *   Static contract verification (§11.4).
*   **Output:** Typed AST (TAST) or Intermediate Representation (IR).

**Phase 5: Code Generation**

The implementation translates the TAST/IR into machine code or a target-specific object format.
*   **Validation:** Final layout computation and backend-specific constraints.
*   **Output:** Executable artifact or object files.

##### Constraints & Legality

**Phase Barrier Enforcement**

The implementation MUST enforce strict barriers between phases. Information generated in Phase $N$ is visible in Phase $N+1$. Information or mutations attempted in Phase $N$ that belong to Phase $<N$ are ill-formed.

**Negative Constraints**

1.  **Late Emission Prohibition:** Code executing in Phase 3 (Analysis) MUST NOT emit new AST nodes.
2.  **Early Introspection Prohibition:** Code executing in Phase 2 (Generation) MUST NOT perform global introspection on types that are not yet finalized (i.e., types currently pending derivation).
3.  **Forward Reference Resolution:** Name resolution errors (`E-NAM-1301`) are reported during Phase 4 (Type Checking), ensuring that symbols generated in Phase 2 are visible.

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CNF-0201` | Error    | `TypeEmitter` capability usage during Analysis Phase. | Compile-time | Rejection |
| `E-CNF-0202` | Error    | Observation of incomplete type during Generation.     | Compile-time | Rejection |
| `E-NAM-1301` | Error    | Unresolved symbol reference during Type Checking.     | Compile-time | Rejection |

---

### 2.13 Syntactic Nesting Limits

##### Definition

**Nesting Depth** is the count of nested syntactic structures (blocks or expressions).

##### Constraints & Legality

**Limits**

Implementations MUST enforce limits on nesting depth to ensure termination and resource bounds. The minimum guaranteed limits are defined in §1.4.

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-SYN-0101` | Error    | Block nesting depth exceeded.      | Compile-time | Rejection |
| `E-SYN-0102` | Error    | Expression nesting depth exceeded. | Compile-time | Rejection |

---

## Clause 3: The Object and Memory Model

### 3.1 Foundational Principles

##### Definition

**Memory Safety** is the conjunction of two properties:
1.  **Liveness**: Every memory access operates on allocated, initialized storage.
2.  **Aliasing Integrity**: Every memory access adheres to the exclusivity invariants defined by the permission system.

##### Static Semantics

**Enforcement Mechanisms**

A conforming implementation MUST enforce memory safety through the following static analysis mechanisms:
1.  **Lifetime Analysis**: Verifies liveness by tracking provenance (§3.3) and region scopes (§3.7).
2.  **Binding State Analysis**: Verifies liveness by tracking initialization and move states (§3.4, §3.5).
3.  **Permission Checking**: Verifies aliasing integrity per the rules defined in Clause 4.

**Unsafe Exemptions**

The memory safety invariants are enforced for all code except expressions enclosed within an `unsafe` block (§3.8). Within an `unsafe` block, the programmer assumes responsibility for maintaining Liveness and Aliasing Integrity.

---

### 3.2 The Object Model

##### Definition

An **Object** is a discrete unit of typed storage.

**Formal Definition**

An object is a tuple $O = (\Sigma, T, L, V)$ where:
- $\Sigma$: The storage segment occupying a contiguous sequence of bytes.
- $T$: The type of the object.
- $L$: The lifetime of the object (the interval of execution during which $\Sigma$ is valid).
- $V$: The value stored in $\Sigma$, which must be a valid inhabitant of $T$.

**Storage Duration**

Every object has a **storage duration** that determines its lifetime $L$:

| Duration      | Allocation            | Deallocation         | Provenance               |
| :------------ | :-------------------- | :------------------- | :----------------------- |
| **Static**    | Program load          | Program termination  | $\pi_{\text{Global}}$    |
| **Automatic** | Scope entry           | Scope exit           | $\pi_{\text{Stack}}(S)$  |
| **Region**    | Explicit `^` operator | Region scope exit    | $\pi_{\text{Region}}(R)$ |
| **Dynamic**   | Explicit allocator    | Explicit deallocator | $\pi_{\text{Heap}}$      |

##### Dynamic Semantics

**Object Lifecycle**

Every object MUST transition through three distinct phases:
1.  **Allocation**: Storage is reserved.
2.  **Initialization**: A valid value of type $T$ is written to the storage.
3.  **Destruction**: The object's lifetime ends. If $T$ implements `Drop`, the destructor is invoked. Storage is subsequently released.

Accessing an object outside its lifetime constitutes Unverifiable Behavior (UVB).

---

### 3.3 The Provenance Model

##### Definition

**Provenance** is a static property of a reference type that identifies the storage duration of the referent. A **Provenance Tag** ($\pi$) is a compile-time annotation attached to every pointer and reference type.

**Formal Definition**

The set of provenance tags is defined as:

$$\Pi = \{ \pi_{\text{Global}}, \pi_{\text{Heap}}, \pi_{\text{Stack}}(S), \pi_{\text{Region}}(R), \bot \}$$

where:
- $S$ identifies a lexical scope.
- $R$ identifies a named region.
- $\bot$ denotes the absence of provenance (literals).

##### Static Semantics

**Lifetime Partial Order**

The provenance tags form a partial order $(\Pi, \le)$ representing lifetime containment. $\pi_A \le \pi_B$ holds if and only if storage with provenance $\pi_A$ is guaranteed to be deallocated no later than storage with provenance $\pi_B$.

The ordering relations are:

1.  **Nesting**: $\pi_{\text{Stack}}(S_{\text{inner}}) < \pi_{\text{Stack}}(S_{\text{outer}})$ where $S_{\text{inner}}$ is nested within $S_{\text{outer}}$.
2.  **Region**: $\pi_{\text{Region}}(R_{\text{inner}}) < \pi_{\text{Region}}(R_{\text{outer}})$ where region $R_{\text{inner}}$ is nested within $R_{\text{outer}}$.
3.  **Heap/Global**: $\pi_{\text{Stack}}(S) < \pi_{\text{Heap}} \le \pi_{\text{Global}}$.

**The Escape Rule**

A value with provenance $\pi_{src}$ MUST NOT be stored in a location with provenance $\pi_{dst}$ if $\pi_{src} < \pi_{dst}$.

Formally:
$$\frac{\Gamma \vdash e : T, \pi_{src} \quad \Gamma \vdash loc : T, \pi_{dst} \quad \pi_{src} < \pi_{dst}}{\text{ill-formed: E-MEM-3020}}$$

**Provenance Inference**

Implementations MUST infer provenance tags for all expressions according to the following rules:

**(P-Global)**
$$\frac{\Gamma \vdash \texttt{static } x : T}{\Gamma \vdash x : T, \pi_{\text{Global}}}$$

**(P-Local)**
$$\frac{\Gamma \vdash \texttt{let } x : T = e \quad x \text{ declared in scope } S}{\Gamma \vdash x : T, \pi_{\text{Stack}}(S)}$$

**(P-Region)**
$$\frac{\Gamma \vdash e : T \quad R \text{ is the innermost active region}}{\Gamma \vdash \hat{\ } e : T, \pi_{\text{Region}}(R)}$$

**(P-Deref)**
$$\frac{\Gamma \vdash p : \texttt{Ptr}\langle T \rangle @\texttt{Valid}, \pi_p \quad \text{target}(p) \text{ has provenance } \pi_t}{\Gamma \vdash *p : T, \pi_t}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                             | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3020` | Error    | Value with shorter-lived provenance escapes to longer-lived location. | Compile-time | Rejection |

---

### 3.4 The Binding Model

##### Definition

A **Binding** is a named association between an identifier and an object.

A **Responsible Binding** is a binding that possesses ownership of the bound object. When a responsible binding goes out of scope or is reassigned, it triggers the deterministic destruction of the object.

**Binding Properties**

1.  **Mutability**:
    *   `let`: Immutable. The binding cannot be reassigned.
    *   `var`: Mutable. The binding can be reassigned.
2.  **Movability**:
    *   `=`: Movable. Responsibility can be transferred via the `move` operator.
    *   `:=`: Immovable. Responsibility cannot be transferred.

**Binding State**

The state of a binding is tracked statically:
*   **Uninitialized**: Storage allocated but not written.
*   **Valid**: Holds a valid object.
*   **Moved**: Responsibility has been transferred; value is inaccessible.
*   **PartiallyMoved**: One or more fields of the bound record have been moved.

##### Syntax & Declaration

**Grammar**

```ebnf
binding_decl     ::= let_decl | var_decl
binding_op       ::= "=" | ":="
let_decl         ::= "let" pattern [ ":" type ] binding_op expression
var_decl         ::= "var" pattern [ ":" type ] binding_op expression
```

##### Static Semantics

**State Tracking**

The implementation MUST track the state of every binding at every program point.

**Control Flow Merge**

At control flow merge points, the state of a binding is the minimum of its states in all incoming branches, ordered as:
$\text{Valid} > \text{PartiallyMoved} > \text{Moved} > \text{Uninitialized}$.

**Reassignment Rule**

For a mutable binding `var x`:
1.  Assignment `x = e` is permitted if `x` is in `Valid`, `Moved`, or `PartiallyMoved` state.
2.  After assignment, `x` transitions to `Valid` state.
3.  If `x` was `Valid` or `PartiallyMoved` prior to assignment, the previous value (or remaining parts) MUST be destroyed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-MEM-3002` | Error    | Access to binding in `Uninitialized` state. | Compile-time | Rejection |
| `E-MEM-3003` | Error    | Reassignment of immutable `let` binding.    | Compile-time | Rejection |

---

### 3.5 Responsibility and Move Semantics

##### Definition

A **Move** is a semantic operation that transfers responsibility for an object from a source binding to a destination.

##### Syntax & Declaration

**Grammar**

```ebnf
move_expr        ::= "move" place_expr
partial_move     ::= "move" place_expr "." identifier
```

##### Static Semantics

**Typing Rules**

**(T-Move)**
$$\frac{\Gamma \vdash x : T \quad \text{state}(\Gamma, x) = \text{Valid} \quad \text{movable}(\Gamma, x)}{\Gamma \vdash \texttt{move } x : T \dashv \Gamma[x \mapsto \text{Moved}]}$$

The predicate $\text{movable}(\Gamma, x)$ holds if and only if $x$ was declared with the `=` operator.

**(T-Move-Field)**
$$\frac{\Gamma \vdash x : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \} \quad \text{perm}(\Gamma, x) = \text{unique}}{\Gamma \vdash \texttt{move } x.f : T \dashv \Gamma[x \mapsto \text{PartiallyMoved}(f)]}$$

**Binding Invalidation**

Upon evaluation of `move x`:
1.  The expression yields the value of `x`.
2.  The binding `x` transitions to the `Moved` state.
3.  Subsequent read or write access to `x` is prohibited until `x` is reinitialized (if mutable).

**Partial Move Constraints**

Partial moves (`move x.field`) are permitted ONLY if:
1.  The parent binding `x` is movable.
2.  The parent binding `x` has `unique` permission.

**Immovable Bindings**

A binding declared with `:=` is **immovable**. Application of the `move` operator to an immovable binding is ill-formed.

**Parameter Passing**

*   **Transferring Parameter**: Declared with `move T`. The caller MUST apply `move` to the argument. Responsibility transfers to the callee.
*   **Non-Transferring Parameter**: Declared as `T` (or `const T`, `unique T`, `shared T`). The caller MUST NOT apply `move`. Responsibility remains with the caller.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                       | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3001` | Error    | Read or move of a binding in `Moved` or `PartiallyMoved` state. | Compile-time | Rejection |
| `E-MEM-3004` | Error    | Partial move from binding without `unique` permission.          | Compile-time | Rejection |
| `E-MEM-3006` | Error    | Attempt to move from immovable binding (`:=`).                  | Compile-time | Rejection |
| `E-DEC-2411` | Error    | Missing `move` keyword at call site for consuming parameter.    | Compile-time | Rejection |

---

### 3.6 Deterministic Destruction

##### Definition

**Deterministic Destruction** is the guaranteed deallocation of resources at a statically known program point.

##### Dynamic Semantics

**Destruction Sequence**

When a scope exits (via normal control flow, `return`, `break`, or panic), all responsible bindings in that scope MUST be destroyed.

**Drop Order**

Bindings MUST be destroyed in **strict LIFO order** (reverse declaration order) relative to their introduction within the scope.

For a scope introducing bindings $x_1, x_2, \ldots, x_n$:
1.  Destruction proceeds from $x_n$ down to $x_1$.
2.  If $x_i$ is in `Moved` state, no action is taken.
3.  If $x_i$ is in `Valid` state and implements `Drop`, `Drop::drop` is invoked.
4.  If $x_i$ is in `PartiallyMoved` state, the remaining valid fields are destroyed recursively.

**Panic Safety**

If a destructor invoked during panic unwinding itself initiates a panic (double panic), the runtime MUST abort the process immediately.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------ | :----------- | :-------- |
| `E-MEM-3005` | Error    | Explicit call to `Drop::drop` method. | Compile-time | Rejection |

---

### 3.7 Regions

##### Definition

A **Region** is a named lexical scope that provides arena-style memory allocation. Objects allocated in a region have a lifetime bound to the region's execution.

##### Syntax & Declaration

**Grammar**

```ebnf
region_stmt      ::= "region" [ region_options ] [ "as" identifier ] block
alloc_expr       ::= identifier "^" expression
```

##### Static Semantics

**Typing Rules**

**(T-Region-Seq)**
$$\frac{\Gamma, r : \texttt{Region@Active} \vdash \textit{body} : T}{\Gamma \vdash \texttt{region as } r \{ \textit{body} \} : T}$$

**(T-Alloc)**
$$\frac{\Gamma(r) = \texttt{Region@Active} \quad \Gamma \vdash e : T}{\Gamma \vdash r\texttt{\^{}}e : T, \pi_{\text{Region}}(r)}$$

**Region Provenance**

The return type of `r^e` is $T$ with provenance $\pi_{\text{Region}}(r)$. This provenance is strictly less than the provenance of the region handle itself.

##### Dynamic Semantics

**Allocation**

The expression `r^e`:
1.  Evaluates `e` to a value $v$.
2.  Allocates storage for $T$ within the memory arena associated with region $r$.
3.  Writes $v$ to the allocated storage.
4.  Returns a reference to the allocated object.

**Deallocation**

Upon exit from the `region` block:
1.  Destructors are invoked for all live objects allocated in the region, in reverse allocation order (LIFO).
2.  The backing memory for the region is reclaimed in bulk.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-REG-1206` | Error    | Named region not found for allocation.      | Compile-time | Rejection |
| `E-MEM-3021` | Error    | Region allocation `^` outside region scope. | Compile-time | Rejection |

---

### 3.8 Unsafe Memory Operations

##### Definition

An **Unsafe Block** is a lexical scope in which specific memory safety checks are suspended to permit low-level operations.

##### Syntax & Declaration

**Grammar**

```ebnf
unsafe_block     ::= "unsafe" block
transmute_expr   ::= "transmute" "::" "<" type "," type ">" "(" expression ")"
```

##### Static Semantics

**Typing Rules**

**(T-Unsafe)**
$$\frac{\Gamma \vdash_{\text{unsafe}} \textit{body} : T}{\Gamma \vdash \texttt{unsafe } \{ \textit{body} \} : T}$$

**(T-Transmute)**
$$\frac{\Gamma \vdash_{\text{unsafe}} e : S \quad \text{sizeof}(S) = \text{sizeof}(T)}{\Gamma \vdash_{\text{unsafe}} \texttt{transmute}::\langle S, T \rangle(e) : T}$$

**Permitted Operations**

The following operations are permitted ONLY within an `unsafe` block:
1.  Dereferencing raw pointers (`*imm T`, `*mut T`).
2.  Invoking procedures declared in `extern` blocks.
3.  Accessing `extern` global variables.
4.  Invoking `transmute`.
5.  Accessing fields of `packed` records.

**Transmute Constraints**

The source type $S$ and target type $T$ in a `transmute` expression MUST have the same size in bytes.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-MEM-3030` | Error    | Unsafe operation attempted outside `unsafe` block. | Compile-time | Rejection |
| `E-MEM-3031` | Error    | Transmute size mismatch.                           | Compile-time | Rejection |

---

## Clause 4: Type System Foundations

### 4.1 Type System Architecture

##### Definition

The Cursive type system is a static, nominal, strict type system with structural extensions.

**Static Typing**: All expressions MUST have a type determined during the Type Checking phase (§2.12). A program containing any expression for which a type cannot be derived is ill-formed.

**Nominality**: Type equivalence is primarily determined by the declared name of a type. Distinct declarations introduce distinct types, even if their structure is identical.

**Structural Extensions**: Tuple, array, slice, union, and function types use structural equivalence.

**Type Context ($\Gamma$)**: The type context is the ordered mapping of identifiers to types and binding states (§3.4) valid at a specific program point.

$$\Gamma ::= \emptyset \mid \Gamma, x : T \mid \Gamma, x : T^{\text{State}}_{\text{props}}$$

The judgment $\Gamma \vdash e : T$ asserts that expression $e$ has type $T$ under context $\Gamma$.

##### Static Semantics

**Type Equivalence ($\equiv$)**

The type equivalence relation $\equiv$ is an equivalence relation (reflexive, symmetric, transitive) on types. Two types $T$ and $U$ are equivalent if and only if derived via the following rules:

**(T-Equiv-Nominal)**
Two nominal types are equivalent if and only if they refer to the same declaration $D$.
$$\frac{D(T) = D(U)}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Nominal)}$$

**(T-Equiv-Structural)**
Structural types (tuples, arrays, slices, unions, functions) are equivalent if and only if they share the same constructor and their component types are equivalent.

$$\frac{T = (T_1, \ldots, T_n) \quad U = (U_1, \ldots, U_n) \quad \forall i \in 1..n,\ \Gamma \vdash T_i \equiv U_i}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Tuple)}$$

$$\frac{T = [T_e; N] \quad U = [U_e; M] \quad \Gamma \vdash T_e \equiv U_e \quad N = M}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Array)}$$

$$\frac{T = [T_e] \quad U = [U_e] \quad \Gamma \vdash T_e \equiv U_e}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Slice)}$$

$$\frac{\text{multiset}(\text{members}(T)) = \text{multiset}(\text{members}(U))}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Union)}$$

Function type equivalence includes representation kind and parameter modifiers (§6.4).

**(T-Equiv-Permission)**
Permission-qualified types are equivalent if and only if both the permission and the base type are identical.
$$\frac{P_1 = P_2 \quad \Gamma \vdash T \equiv U}{\Gamma \vdash P_1\ T \equiv P_2\ U} \quad \text{(T-Equiv-Perm)}$$

**Alias Resolution**
Type aliases are transparent. Equivalence is determined using the aliased type.
$$\frac{\Gamma \vdash \text{resolve}(T) \equiv \text{resolve}(U)}{\Gamma \vdash T \equiv U} \quad \text{(T-Equiv-Alias)}$$

##### Constraints & Legality

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

**Subtyping ($<:$)**: A binary relation on types defining substitutability. $S <: T$ implies that a value of type $S$ may be used in any context expecting type $T$.

**Coercion**: An implicit conversion applied when a subtype is used where a supertype is expected.

##### Static Semantics

**Relation Properties**
The subtyping relation $<:$ MUST be:
1.  **Reflexive**: $\Gamma \vdash T <: T$
2.  **Transitive**: $\Gamma \vdash S <: T \land \Gamma \vdash T <: U \implies \Gamma \vdash S <: U$
3.  **Antisymmetric**: $\Gamma \vdash S <: T \land \Gamma \vdash T <: S \implies \Gamma \vdash S \equiv T$

**Coercion Rule**
$$\frac{\Gamma \vdash e : S \quad \Gamma \vdash S <: T}{\Gamma \vdash e : T} \quad \text{(T-Coerce)}$$

**Bottom Type**
The never type `!` is a subtype of all types.
$$\frac{T \in \mathcal{T}}{\Gamma \vdash \texttt{!} <: T} \quad \text{(Sub-Never)}$$

**Structural Subtyping**
Subtyping rules for specific constructs are defined in their respective sections:
-   **Tuples**: Covariant in all components (§5.2.1).
-   **Arrays**: Covariant in element type; invariant in length (§5.2.2).
-   **Slices**: Covariant in element type (§5.2.3).
-   **Unions**: Width and depth subtyping (§5.5).
-   **Functions**: Contravariant in parameters, covariant in return type (§4.3).
-   **Permissions**: `unique <: shared <: const` (§4.5).
-   **Modal Types**: Niche-layout-compatible state subtyping (§6.1).
-   **Classes**: Implementation subtyping (§10.1).

##### Constraints & Legality

**Negative Constraints**
1.  Implicit coercion MUST NOT occur if $\Gamma \vdash S \not<: T$.
2.  Coercions MUST NOT introduce Unverifiable Behavior.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-1510` | Error    | Source type is not a subtype of target type. | Compile-time | Rejection |

---

### 4.3 Variance

##### Definition

**Variance** defines the subtyping relationship of a parameterized type $F\langle T \rangle$ with respect to its type argument $T$.

**Variance Categories**:
-   **Covariant ($+$)**: $A <: B \implies F\langle A \rangle <: F\langle B \rangle$
-   **Contravariant ($-$)**: $B <: A \implies F\langle A \rangle <: F\langle B \rangle$
-   **Invariant ($=$)**: $F\langle A \rangle <: F\langle B \rangle \iff A \equiv B$
-   **Bivariant ($\pm$)**: $F\langle A \rangle <: F\langle B \rangle$ is always true (parameter unused).

##### Static Semantics

**Variance Determination**
The variance of a type parameter $T$ in type definition $D$ is determined by the position of $T$ within $D$:

1.  **Output Position** (immutable field, return type): Covariant.
2.  **Input Position** (method parameter): Contravariant.
3.  **Mutable Position** (mutable field, `var` binding): Invariant.
4.  **Both Input and Output**: Invariant.

**Generic Subtyping Rule**
For a generic type constructor $C$ with parameters $P_1, \ldots, P_n$ having variances $v_1, \ldots, v_n$:

$$\frac{\forall i \in 1..n, \text{CheckVariance}(v_i, A_i, B_i)}{\Gamma \vdash C\langle A_1, \ldots, A_n \rangle <: C\langle B_1, \ldots, B_n \rangle} \quad \text{(Sub-Generic)}$$

Where $\text{CheckVariance}(v, A, B)$ is:
-   If $v = +$: $\Gamma \vdash A <: B$
-   If $v = -$: $\Gamma \vdash B <: A$
-   If $v = =$: $\Gamma \vdash A \equiv B$
-   If $v = \pm$: True

**Permission Interaction**
The `const` permission induces covariance on otherwise invariant parameters, as mutation is statically prohibited.

$$\frac{\Gamma \vdash A <: B}{\Gamma \vdash \texttt{const}\ C\langle A \rangle <: \texttt{const}\ C\langle B \rangle} \quad \text{(Sub-Const-Covar)}$$

This rule does NOT apply to `unique` or `shared` permissions.

**Function Type Variance**
Function types are contravariant in parameter types and covariant in return types.

$$\frac{\Gamma \vdash U <: T \quad \Gamma \vdash R_T <: R_U}{\Gamma \vdash (T) \to R_T <: (U) \to R_U} \quad \text{(Sub-Func)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                           | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1520` | Error    | Variance violation in generic type instantiation.   | Compile-time | Rejection |
| `E-TYP-1521` | Error    | Invariant type parameter requires exact type match. | Compile-time | Rejection |

---

### 4.4 Bidirectional Type Inference

##### Definition

**Type Inference** is the process of deriving types for expressions where explicit annotations are omitted. Cursive uses **Bidirectional Type Inference**, consisting of **Synthesis** (deriving a type from an expression) and **Checking** (validating an expression against an expected type).

##### Static Semantics

**Judgment Forms**
-   $\Gamma \vdash e \Rightarrow T$: Expression $e$ synthesizes type $T$.
-   $\Gamma \vdash e \Leftarrow T$: Expression $e$ checks against expected type $T$.

**Inference Rules**

**(Synth-Var)**
$$\frac{(x : T) \in \Gamma}{\Gamma \vdash x \Rightarrow T}$$

**(Check-Sub)**
Synthesis subsumes checking via subtyping.
$$\frac{\Gamma \vdash e \Rightarrow S \quad \Gamma \vdash S <: T}{\Gamma \vdash e \Leftarrow T}$$

**(Synth-Annot)**
Explicit annotation switches mode from Checking to Synthesis.
$$\frac{\Gamma \vdash e \Leftarrow T}{\Gamma \vdash (e : T) \Rightarrow T}$$

**(Check-Lambda)**
Closures check against function types by propagating parameter types.
$$\frac{\Gamma, x_1 : T_1, \ldots, x_n : T_n \vdash e_{\text{body}} \Leftarrow R}{\Gamma \vdash |x_1, \ldots, x_n| \to e_{\text{body}} \Leftarrow (T_1, \ldots, T_n) \to R}$$

**Inference Boundaries**
Type inference is **local**. It operates within a single procedure body.
-   Inference MUST NOT propagate across procedure boundaries.
-   Top-level declarations (constants, globals, procedures) MUST have explicit type annotations.

**Ambiguity**
If synthesis fails and no expected type is provided (e.g., `let x = 5` where literal types are ambiguous without context, or generic calls with undetermined parameters), the program is ill-formed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-1530` | Error    | Type inference failed; annotation required.  | Compile-time | Rejection |
| `E-TYP-1505` | Error    | Missing type annotation on top-level symbol. | Compile-time | Rejection |

---

### 4.5 Permission Types

#### 4.5.1 Definitions

##### Definition

A **Permission** is a type qualifier that restricts aliasing and mutation capabilities for a reference.

**Permission Set**: $\mathcal{P} = \{\texttt{const}, \texttt{unique}, \texttt{shared}\}$.

A **Permission-Qualified Type** $P\ T$ consists of a permission $P \in \mathcal{P}$ and a base type $T$. When no permission is specified, `const` is the default.

##### Static Semantics

**Semantics of Permissions**

1.  **`const`** (Immutable):
    -   Grants: Read-only access.
    -   Aliasing: Unlimited `const` and `shared` aliases permitted.
    -   Mutation: Prohibited.

2.  **`unique`** (Exclusive Mutable):
    -   Grants: Read and Write access.
    -   Aliasing: No other live aliases (of any permission) permitted to the same object or its sub-components.
    -   Mutation: Permitted.

3.  **`shared`** (Synchronized Mutable):
    -   Grants: Read access (always), Write access (conditional via Key System, §14).
    -   Aliasing: Unlimited `const` and `shared` aliases permitted.
    -   Mutation: Permitted only via synchronized mechanisms (keys).

**Exclusion Principle**
The following matrix defines the validity of forming a new reference with permission $P_{new}$ to an object already referenced with permission $P_{exist}$.

| Existing \ New | `unique` | `shared` | `const` |
| :------------- | :------- | :------- | :------ |
| **`unique`**   | No       | No       | No      |
| **`shared`**   | No       | Yes      | Yes     |
| **`const`**    | No       | Yes      | Yes     |

Violation of this principle is ill-formed.

#### 4.5.2 Permission Lattice

##### Definition

Permissions form a linear subtyping lattice ordered by strength of guarantee.

**Lattice Order**: $\texttt{unique} <: \texttt{shared} <: \texttt{const}$

##### Static Semantics

**Subtyping Rules**

**(Sub-Perm-US)**
$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{shared}\ T}$$

**(Sub-Perm-SC)**
$$\frac{}{\Gamma \vdash \texttt{shared}\ T <: \texttt{const}\ T}$$

**(Sub-Perm-Trans)**
$$\frac{}{\Gamma \vdash \texttt{unique}\ T <: \texttt{const}\ T}$$

**No Implicit Upgrade**
Coercion from a weaker permission to a stronger permission is prohibited.
$$\Gamma \nvdash \texttt{const}\ T <: \texttt{unique}\ T$$
$$\Gamma \nvdash \texttt{shared}\ T <: \texttt{unique}\ T$$
$$\Gamma \nvdash \texttt{const}\ T <: \texttt{shared}\ T$$

#### 4.5.3 Binding State Machine

##### Definition

Bindings with `unique` permission are subject to state tracking to enforce the Exclusion Principle during temporary downgrades.

**Binding States**:
-   **Active**: The binding is valid and accessible.
-   **Inactive**: The binding is temporarily inaccessible because a conflicting alias exists.

##### Static Semantics

**Transitions**

1.  **Downgrade (Active $\to$ Inactive)**: When a `unique` binding $b$ is used to create a `shared` or `const` reference (e.g., passed to a function), $b$ transitions to **Inactive**.
    -   Condition: The lifetime of the derived reference must be bounded.

2.  **Restoration (Inactive $\to$ Active)**: When the lifetime of all derived references expires, $b$ transitions to **Active**.

**Constraint**
Accessing a binding in the **Inactive** state is ill-formed.

#### 4.5.4 Method Receiver Compatibility

##### Static Semantics

A method with receiver requirement $P_{req}$ is callable via a reference with permission $P_{ref}$ if and only if $P_{ref} <: P_{req}$.

**Compatibility Matrix**

| Caller ($P_{ref}$) \ Method ($P_{req}$) | `const` (`~`) | `shared` (`~%`) | `unique` (`~!`) |
| :-------------------------------------- | :------------ | :-------------- | :-------------- |
| **`const`**                             | Yes           | No              | No              |
| **`shared`**                            | Yes           | Yes             | No              |
| **`unique`**                            | Yes           | Yes             | Yes             |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-TYP-1601` | Error    | Mutation via `const` reference.                         | Compile-time | Rejection |
| `E-TYP-1602` | Error    | Access to `unique` binding while Inactive (aliased).    | Compile-time | Rejection |
| `E-TYP-1603` | Error    | Violation of Exclusion Principle (aliasing `unique`).   | Compile-time | Rejection |
| `E-TYP-1604` | Error    | Direct mutation of `shared` field without Key.          | Compile-time | Rejection |
| `E-TYP-1605` | Error    | Method receiver permission incompatible with reference. | Compile-time | Rejection |
| `E-TYP-1511` | Error    | Implicit permission upgrade attempted.                  | Compile-time | Rejection |

---

## Clause 5: Data Types

### 5.1 Primitive Types

##### Definition

**Primitive types** are the built-in scalar types representing indivisible units of data with fixed size and representation.

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

##### Static Semantics

**Type Equivalence**

Primitive types use strict name equivalence. Two primitive type expressions are equivalent if and only if they denote the same element of $\mathcal{T}_{\text{prim}}$.

**(T-Equiv-Prim)**
$$\frac{T \in \mathcal{T}_{\text{prim}} \quad U \in \mathcal{T}_{\text{prim}} \quad T = U}{\Gamma \vdash T \equiv U}$$

**Subtyping**

The never type `!` is a subtype of all types. No other subtyping relations exist between distinct primitive types.

**(Sub-Never)**
$$\frac{T \in \mathcal{T}}{\Gamma \vdash \texttt{!} <: T}$$

**(No-Implicit-Prim-Coerce)**
$$\frac{T \neq U \quad T \in \mathcal{T}_{\text{prim}} \quad U \in \mathcal{T}_{\text{prim}} \quad U \neq \texttt{!}}{\Gamma \nvdash T <: U}$$

**Literal Typing**

Literal values are typed according to their lexical form (§2.8) and value range:

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

##### Dynamic Semantics

**Integer Arithmetic**

Evaluation of integer arithmetic operations MUST adhere to the following rules regarding overflow:

1.  **Checked Mode:** In debug configurations, signed and unsigned integer overflow MUST cause a runtime panic (`P-TYP-1720`).
2.  **Release Mode:** In release configurations, overflow behavior is **Implementation-Defined Behavior (IDB)**. The implementation MUST choose one of: Wrap, Panic, or Trap.

Integer division and remainder by zero MUST cause a runtime panic (`P-TYP-1721`) in all modes.

**Floating-Point Arithmetic**

Floating-point operations MUST conform to IEEE 754-2008 semantics.

**Unit and Never Semantics**

*   The unit type `()` has exactly one value, denoted `()`.
*   The never type `!` is uninhabited. Evaluation of an expression of type `!` MUST NOT complete normally (it must diverge, panic, or exit).

##### Memory & Layout

**Representation**

Implementations MUST use the following representations for primitive types:

| Type             | Size (bytes) | Alignment (bytes) | Representation                        |
| :--------------- | :----------- | :---------------- | :------------------------------------ |
| `i8`, `u8`       | 1            | 1                 | Integer (Two's complement for signed) |
| `bool`           | 1            | 1                 | `0x00` (false), `0x01` (true)         |
| `i16`, `u16`     | 2            | 2                 | Integer                               |
| `f16`            | 2            | 2                 | IEEE 754 binary16                     |
| `i32`, `u32`     | 4            | 4                 | Integer                               |
| `f32`            | 4            | 4                 | IEEE 754 binary32                     |
| `char`           | 4            | 4                 | Unicode Scalar Value                  |
| `i64`, `u64`     | 8            | 8                 | Integer                               |
| `f64`            | 8            | 8                 | IEEE 754 binary64                     |
| `i128`, `u128`   | 16           | IDB (8 or 16)     | Integer                               |
| `isize`, `usize` | IDB          | IDB               | Platform pointer width                |
| `()`, `!`        | 0            | 1                 | Empty                                 |

**Representation Invariants**

*   **Boolean:** Valid values are exactly `0x00` and `0x01`.
*   **Character:** Valid values are Unicode Scalar Values (U+0000–U+D7FF, U+E000–U+10FFFF).

Violation of these invariants constitutes **Unverifiable Behavior (UVB)**.

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

**Composite types** are constructed from other types using structural aggregation mechanisms.

**Formal Definition**

$$\mathcal{T}_{\text{composite}} = \mathcal{T}_{\text{tuple}} \cup \mathcal{T}_{\text{array}} \cup \mathcal{T}_{\text{slice}} \cup \mathcal{T}_{\text{range}}$$

Two composite types are equivalent if and only if their structures and component types are equivalent.

#### 5.2.1 Tuples

##### Definition

A **tuple** is a finite, ordered sequence of component types.

**Formal Definition**

$$\mathcal{T}_{\text{tuple}} = \{(T_1, T_2, \ldots, T_n) : n \geq 0 \land \forall i \in 1..n,\ T_i \in \mathcal{T}\}$$

##### Syntax & Declaration

**Grammar**

```ebnf
tuple_type       ::= "(" tuple_elements? ")"
tuple_elements   ::= type ";" | type ("," type)+

tuple_literal    ::= "(" tuple_expr_elements? ")"
tuple_expr_elements ::= expression ";" | expression ("," expression)+

tuple_access     ::= expression "." decimal_literal
```

##### Static Semantics

**Typing Rules**

**(T-Tuple-Type)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash (T_1, \ldots, T_n)\ \text{wf}}$$

**(T-Tuple-Literal)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T_i}{\Gamma \vdash (e_1, \ldots, e_n) : (T_1, \ldots, T_n)}$$

**(T-Tuple-Index)**
$$\frac{\Gamma \vdash e : (T_0, T_1, \ldots, T_{n-1}) \quad 0 \leq i < n}{\Gamma \vdash e.i : T_i}$$

**Equivalence and Subtyping**

Tuple types use structural equivalence. Subtyping is covariant in all components.

**(Sub-Tuple)**
$$\frac{n = m \quad \forall i \in 1..n,\ \Gamma \vdash T_i <: U_i}{\Gamma \vdash (T_1, \ldots, T_n) <: (U_1, \ldots, U_m)}$$

##### Memory & Layout

**Representation**

Tuples MUST be laid out as a contiguous sequence of their components in declaration order. Padding MUST be inserted to satisfy alignment requirements of components.

$$\text{sizeof}((T_1, \ldots, T_n)) = \sum_{i=1}^{n} \text{sizeof}(T_i) + \text{padding}$$

$$\text{alignof}((T_1, \ldots, T_n)) = \max_{i \in 1..n}(\text{alignof}(T_i))$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1801` | Error    | Tuple index out of bounds.                                  | Compile-time | Rejection |
| `E-TYP-1802` | Error    | Tuple index is not a compile-time constant integer literal. | Compile-time | Rejection |
| `E-TYP-1803` | Error    | Tuple arity mismatch in assignment or pattern.              | Compile-time | Rejection |

#### 5.2.2 Arrays

##### Definition

An **array** is a fixed-length sequence of elements of a single type.

**Formal Definition**

$$\mathcal{T}_{\text{array}} = \{[T; N] : T \in \mathcal{T} \land N \in \mathbb{N}\}$$

The length $N$ is a part of the type signature.

##### Syntax & Declaration

**Grammar**

```ebnf
array_type       ::= "[" type ";" const_expression "]"

array_literal    ::= "[" expression_list "]"
                   | "[" expression ";" const_expression "]"

array_access     ::= expression "[" expression "]"
```

##### Static Semantics

**Typing Rules**

**(T-Array-Type)**
$$\frac{\Gamma \vdash T\ \text{wf} \quad N : \texttt{usize} \quad N \geq 0}{\Gamma \vdash [T; N]\ \text{wf}}$$

**(T-Array-Literal-List)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T}{\Gamma \vdash [e_1, \ldots, e_n] : [T; n]}$$

**(T-Array-Literal-Repeat)**
$$\frac{\Gamma \vdash e : T \quad N : \texttt{usize} \quad T <: \texttt{Copy} \lor e \in \text{ConstExpr}}{\Gamma \vdash [e; N] : [T; N]}$$

**(T-Array-Index)**
$$\frac{\Gamma \vdash a : [T; N] \quad \Gamma \vdash i : \texttt{usize}}{\Gamma \vdash a[i] : T}$$

**Equivalence and Subtyping**

Array types use structural equivalence. Subtyping is covariant in the element type and invariant in the length.

**(Sub-Array)**
$$\frac{\Gamma \vdash T <: U \quad N = M}{\Gamma \vdash [T; N] <: [U; M]}$$

##### Dynamic Semantics

**Bounds Checking**

Array indexing operations MUST be checked against the array bounds at runtime. If the index is out of bounds, the operation MUST panic (`P-TYP-2530`).

##### Memory & Layout

**Representation**

Arrays MUST be laid out as a contiguous sequence of elements. Implementations MUST NOT insert padding between elements.

$$\text{sizeof}([T; N]) = N \times \text{sizeof}(T)$$

$$\text{alignof}([T; N]) = \text{alignof}(T)$$

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

A **slice** is a dynamically-sized view into a contiguous sequence of elements.

**Formal Definition**

$$\mathcal{T}_{\text{slice}} = \{[T] : T \in \mathcal{T}\}$$

##### Syntax & Declaration

**Grammar**

```ebnf
slice_type       ::= "[" type "]"
slice_access     ::= expression "[" expression "]"
slice_range      ::= expression "[" range_expression "]"
```

##### Static Semantics

**Typing Rules**

**(T-Slice-Type)**
$$\frac{\Gamma \vdash T\ \text{wf}}{\Gamma \vdash [T]\ \text{wf}}$$

**(T-Slice-From-Array)**
$$\frac{\Gamma \vdash a : P\ [T; N] \quad \Gamma \vdash r : \text{Range}}{\Gamma \vdash a[r] : P\ [T]}$$

**(T-Slice-Index)**
$$\frac{\Gamma \vdash s : [T] \quad \Gamma \vdash i : \texttt{usize}}{\Gamma \vdash s[i] : T}$$

**(Coerce-Array-Slice)**
$$\frac{\Gamma \vdash a : P\ [T; N]}{\Gamma \vdash a : P\ [T]}$$

##### Dynamic Semantics

**Bounds Checking**

Slice indexing and sub-slicing operations MUST be checked against the slice length at runtime. Violation MUST panic (`P-TYP-1822`).

##### Memory & Layout

**Representation**

A slice MUST be represented as a dense pointer containing a pointer to the first element and the length.

| Field | Type    | Description        |
| :---- | :------ | :----------------- |
| `ptr` | `*T`    | Pointer to data    |
| `len` | `usize` | Number of elements |

$$\text{sizeof}([T]) = 2 \times \text{sizeof}(\texttt{usize})$$

$$\text{alignof}([T]) = \text{alignof}(\texttt{usize})$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-1820` | Error    | Slice index expression has non-`usize` type. | Compile-time | Rejection |
| `P-TYP-1822` | Panic    | Slice range out of bounds.                   | Runtime      | Panic     |
| `E-TYP-1823` | Error    | Slice outlives borrowed storage.             | Compile-time | Rejection |

#### 5.2.4 Range Types

##### Definition

**Range types** are intrinsic structural types representing intervals.

**Formal Definition**

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

##### Static Semantics

**Typing Rules**

Expressions evaluate to the corresponding range type. Bounds MUST have identical types.

**(T-Range-Exclusive)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T}{\Gamma \vdash e_1\text{..}e_2 : \texttt{Range}\langle T \rangle}$$

**Copy Semantics**

A range type implements `Copy` if and only if its element type $T$ implements `Copy`. `RangeFull` always implements `Copy`.

##### Memory & Layout

**Representation**

Range types MUST be laid out structurally equivalent to records containing their bounds as public fields (`start`, `end`).

| Type                  | Size                                         |
| :-------------------- | :------------------------------------------- |
| `Range<T>`            | $2 \times \text{sizeof}(T) + \text{padding}$ |
| `RangeInclusive<T>`   | $2 \times \text{sizeof}(T) + \text{padding}$ |
| `RangeFrom<T>`        | $\text{sizeof}(T)$                           |
| `RangeTo<T>`          | $\text{sizeof}(T)$                           |
| `RangeToInclusive<T>` | $\text{sizeof}(T)$                           |
| `RangeFull`           | $0$                                          |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1830` | Error    | Range bound types do not match.                             | Compile-time | Rejection |
| `E-TYP-1831` | Error    | Unbounded range used in context requiring finite iteration. | Compile-time | Rejection |

---

### 5.3 Records

##### Definition

A **record** is a nominal product type with named fields.

**Formal Definition**

A record type $R$ is defined by:

$$R = (\text{Name}, \text{Params}, \text{Fields}, \text{Forms}, \text{Invariant})$$

Two record types are equivalent if and only if they refer to the same declaration.

##### Syntax & Declaration

**Grammar**

```ebnf
record_decl      ::= [visibility] "record" identifier [generic_params] 
                     [implements_clause] "{" record_body "}" [type_invariant]

record_body      ::= field_decl ("," field_decl)* ","?

field_decl       ::= [visibility] identifier ":" type

implements_clause ::= "<:" class_list
class_list       ::= type_path ("," type_path)*

type_invariant   ::= "where" "{" predicate "}"
```

**Record Literal**

```ebnf
record_literal   ::= type_path "{" field_init_list "}"
field_init_list  ::= field_init ("," field_init)* ","?
field_init       ::= identifier ":" expression
                   | identifier
```

##### Static Semantics

**Well-Formedness**

**(T-Record-WF)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf} \quad \forall i \neq j,\ f_i \neq f_j}{\Gamma \vdash \texttt{record}\ R\ \{f_1: T_1, \ldots, f_n: T_n\}\ \text{wf}}$$

**Typing Rules**

**(T-Record-Literal)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash e_i : T_i \quad R = \{f_1: T_1, \ldots, f_n: T_n\}}{\Gamma \vdash R\ \{f_1: e_1, \ldots, f_n: e_n\} : R}$$

**(T-Field-Access)**
$$\frac{\Gamma \vdash e : R \quad R.\text{fields}(f) = T \quad \text{visible}(f, \Gamma)}{\Gamma \vdash e.f : T}$$

**Subtyping**

Record types are subtypes of the classes they explicitly implement.

**(Sub-Record-Class)**
$$\frac{R\ \texttt{<:}\ \textit{Cl}}{\Gamma \vdash R <: \textit{Cl}}$$

##### Memory & Layout

**Representation**

Records MUST be laid out as a contiguous sequence of field values. Field ordering and padding are Implementation-Defined (IDB) unless the `[[layout(C)]]` attribute is present.

**Size and Alignment**

$$\text{alignof}(R) = \max_{f \in \text{fields}(R)}(\text{alignof}(T_f))$$

$$\text{sizeof}(R) \geq \sum_{f \in \text{fields}(R)} \text{sizeof}(T_f)$$

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

An **enum** is a nominal sum type consisting of a finite set of named variants. Each variant may carry an associated payload.

**Formal Definition**

An enum type $E$ is defined by:

$$E = (\text{Name}, \text{Params}, \text{Variants}, \text{Forms}, \text{Invariant})$$

Variants are identified by a unique name and an optional discriminant value.

##### Syntax & Declaration

**Grammar**

```ebnf
enum_decl        ::= [visibility] "enum" identifier [generic_params]
                     [implements_clause] "{" variant_list "}" [type_invariant]

variant_list     ::= variant ("," variant)* ","?

variant          ::= identifier [variant_payload] ["=" integer_constant]

variant_payload  ::= "(" type_list ")"
                   | "{" field_decl_list "}"
```

##### Static Semantics

**Well-Formedness**

**(T-Enum-WF)**
$$\frac{\forall i \in 1..n,\ \Gamma \vdash P_i\ \text{wf} \quad \forall i \neq j,\ v_i \neq v_j}{\Gamma \vdash \texttt{enum}\ E\ \{v_1(P_1), \ldots, v_n(P_n)\}\ \text{wf}}$$

**Discriminant Assignment**

Each variant MUST have a unique integer discriminant.
1.  If explicit: The value MUST be the specified constant.
2.  If implicit (first): The value is `0`.
3.  If implicit (subsequent): The value is `previous_discriminant + 1`.

Overflow of the underlying discriminant type during implicit assignment is ill-formed.

**Typing Rules**

Enum literals are typed according to the variant form (unit, tuple-like, record-like). Access to enum payload data MUST occur via pattern matching.

**(Sub-Enum-Class)**
$$\frac{E\ \texttt{<:}\ \textit{Cl}}{\Gamma \vdash E <: \textit{Cl}}$$

##### Memory & Layout

**Representation**

Enums MUST be represented as a discriminant followed by payload storage. The payload storage MUST be sized to accommodate the largest variant payload.

$$\text{layout}(E) = \text{Discriminant}\ ||\ \text{Payload}\ ||\ \text{Padding}$$

**Niche Optimization**

Implementations MUST apply niche optimization to eliminate the discriminant field when valid payloads contain invalid bit patterns (niches) that can unambiguously encode the variant.

**C-Compatible Layout**

When `[[layout(C)]]` or `[[layout(IntType)]]` is present, the implementation MUST follow the C ABI layout for tagged unions.

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

---

### 5.5 Union Types

##### Definition

A **union type** is a structural anonymous sum type representing a value that may be of one of several distinct types. Union types are safe and tagged.

**Formal Definition**

$$U = \bigcup_{i=1}^{n} T_i \quad \text{where } n \geq 2 \text{ and } T_i \in \mathcal{T}$$

##### Syntax & Declaration

**Grammar**

```ebnf
union_type       ::= type ("|" type)+
```

##### Static Semantics

**Well-Formedness**

**(T-Union-WF)**
$$\frac{n \geq 2 \quad \forall i \in 1..n,\ \Gamma \vdash T_i\ \text{wf}}{\Gamma \vdash T_1 \mid T_2 \mid \cdots \mid T_n\ \text{wf}}$$

**Recursion Detection**

A union type $U$ is **recursive** if $U$ appears in its own member list without intervening pointer indirection. This is ill-formed (`E-TYP-2203`).

Formally, a union is valid if $\text{ValidRecursion}(U, \emptyset)$ holds:

$$
\text{ValidRecursion}(T, \text{Seen}) = \begin{cases}
\text{true} & \text{if } T \in \text{IndirectionTypes} \\
\text{false} & \text{if } T \in \text{Seen} \\
\forall M \in \text{members}(T).\ \text{ValidRecursion}(M, \text{Seen} \cup \{T\}) & \text{if } T \text{ is a union} \\
\text{true} & \text{otherwise}
\end{cases}
$$

Where $\text{IndirectionTypes} = \{\texttt{Ptr}\langle T \rangle, \texttt{*imm } T, \texttt{*mut } T, \texttt{const } T, \texttt{unique } T, \texttt{shared } T\}$.

**Typing Rules**

**(T-Union-Intro)**
$$\frac{\Gamma \vdash e : T \quad T \in_U U}{\Gamma \vdash e : U}$$

**(T-Union-Match)**
Access to union values MUST be performed via exhaustive pattern matching.

**Equivalence**

Union types use structural equivalence based on the multiset of member types.

**Subtyping**

**(Sub-Union-Width)**
$$\frac{\forall T \in_U U_1 : T \in_U U_2}{\Gamma \vdash U_1 <: U_2}$$

**(Sub-Member-Union)**
$$\frac{T \in_U U}{\Gamma \vdash T <: U}$$

##### 5.5.1 Propagation Operator (`?`)

The propagation operator `?` unwraps the success member of a union or returns error members.

**(T-Try-Union)**
$$\frac{
    \Gamma \vdash e : U \quad
    U = T_s \mid T_{e_1} \mid \cdots \mid T_{e_k} \quad
    \forall i \in 1..k,\ T_{e_i} <: R \quad
    T_s \not<: R
}{
    \Gamma \vdash e? : T_s
}$$

The operator is well-formed only if exactly one member type is not propagation-compatible with the enclosing return type.

##### Memory & Layout

**Representation**

Unions MUST be represented as a discriminant followed by payload storage.

$$\text{layout}(U) = \text{Discriminant}\ ||\ \text{Payload}\ ||\ \text{Padding}$$

**Discriminant**

Discriminant values MUST be assigned based on the canonical lexicographical ordering of member type names.

**Niche Optimization**

Implementations MUST apply niche optimization to union types.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2201` | Error    | Union type has fewer than two member types.               | Compile-time | Rejection |
| `E-TYP-2202` | Error    | Direct access on union value without pattern matching.    | Compile-time | Rejection |
| `E-TYP-2203` | Error    | Infinite type: recursive union without indirection.       | Compile-time | Rejection |
| `E-TYP-2204` | Error    | Union type used in `[[layout(C)]]` context.               | Compile-time | Rejection |
| `E-PAT-2205` | Error    | `match` expression is not exhaustive for union type.      | Compile-time | Rejection |
| `W-TYP-2201` | Warning  | Union type contains duplicate member types.               | Compile-time | N/A       |
| `E-TYP-2210` | Error    | `?` applied to non-union type.                            | Compile-time | Rejection |
| `E-TYP-2211` | Error    | Ambiguous success type in `?` expression.                 | Compile-time | Rejection |
| `E-TYP-2212` | Error    | Union member not propagation-compatible with return type. | Compile-time | Rejection |
| `E-TYP-2213` | Error    | `?` used outside procedure body.                          | Compile-time | Rejection |
| `E-TYP-2214` | Error    | No success type candidate (all types propagate).          | Compile-time | Rejection |

---

## Clause 6: Behavioral Types

### 6.1 Modal Types

##### Definition

A **modal type** is a nominal sum type that embeds a finite state machine into the type system. A modal declaration defines a family of types consisting of the **general modal type** and a set of **state-specific types**.

**Formal Definition**

Let $M$ be a modal type name. The declaration of $M$ defines:
1.  A set of state names $\Sigma_M = \{S_1, \ldots, S_n\}$.
2.  For each state $S \in \Sigma_M$, a payload schema $\Pi(S)$.
3.  A set of transition functions $\Delta_M \subseteq \Sigma_M \times \Sigma_M$.

The type family $\mathcal{T}_M$ consists of:
*   The **general type** $M$, capable of holding any state $S \in \Sigma_M$.
*   The **state-specific types** $\{M@S \mid S \in \Sigma_M\}$, each capable of holding exactly one state.

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
state_specific_type ::= type_path "@" state_name
```

The `param_list` production is defined in §9.1. The `implements_clause` is defined in §5.3.

**Syntactic Constraints**

1.  A modal declaration MUST define at least one state.
2.  State names MUST be unique within the modal declaration.
3.  State names MUST NOT match the enclosing modal type name.
4.  Transition definitions MUST appear only within `state_members` blocks.

##### Static Semantics

**Typing Rules**

**(T-Modal-State-Intro)**
A state-specific value is constructed by providing arguments for the state's payload fields.
$$\frac{S \in \Sigma_M \quad \Pi(S) = \{f_1: T_1, \ldots, f_n: T_n\} \quad \forall i.\ \Gamma \vdash e_i : T_i}{\Gamma \vdash M@S\ \{f_1: e_1, \ldots, f_n: e_n\} : M@S}$$

**(T-Modal-Field)**
Fields declared in a state payload are accessible only on the corresponding state-specific type.
$$\frac{\Gamma \vdash e : M@S \quad f \in \Pi(S) \quad \Pi(S).f = T}{\Gamma \vdash e.f : T}$$

**(T-Modal-Method)**
Methods declared in a state block are callable only on the corresponding state-specific type.
$$\frac{\Gamma \vdash e : M@S \quad m \in \text{Methods}(S) \quad m : (M@S, \overline{P}) \to R}{\Gamma \vdash e.m(\overline{a}) : R}$$

**(T-Modal-Transition)**
A transition defined in state $S_{src}$ targeting $S_{tgt}$ consumes the receiver and returns the target state-specific type.
$$\frac{\Gamma \vdash e : \texttt{unique}\ M@S_{src} \quad t \in \text{Transitions}(S_{src}, S_{tgt})}{\Gamma \vdash e.t(\ldots) : M@S_{tgt} \dashv \Gamma[e \mapsto \text{Moved}]}$$

**Subtyping and Widening**

State-specific types are subtypes of the general modal type if and only if they are **niche-layout-compatible** (defined below under Memory & Layout).

1.  **Niche-Compatible:** If $M@S$ is niche-layout-compatible with $M$:
    $$\Gamma \vdash M@S <: M$$
2.  **Standard:** If $M@S$ is not niche-layout-compatible, the subtyping relation does not hold. Conversion requires the explicit `widen` operator (§11.4.5).

Distinct state-specific types are incomparable:
$$\frac{S_i \neq S_j}{\Gamma \vdash M@S_i \not<: M@S_j}$$

**Field Visibility**

Payload fields of a modal state have `protected` visibility by default. They MUST NOT be accessed outside the defining modal declaration (including its methods and transitions) unless explicitly declared `public`.

##### Dynamic Semantics

**Pattern Matching**

The general modal type $M$ MUST be inspected via exhaustive pattern matching (§12.2).
1.  **Discriminant Check:** The runtime evaluates the discriminant of the value.
2.  **Dispatch:** Control transfers to the arm corresponding to the active state.
3.  **Binding:** The pattern binds variables to the payload fields of the active state.

**Transition Execution**

Execution of a transition $t$ on receiver $obj$:
1.  The `unique` receiver $obj$ is consumed (moved).
2.  The transition body executes.
3.  The body MUST return a value of type $M@S_{tgt}$.

##### Memory & Layout

**State-Specific Layout**

A state-specific type $M@S$ is laid out as a record containing the fields defined in $\Pi(S)$. No runtime discriminant is stored in $M@S$.

**General Layout**

The general modal type $M$ is laid out as a tagged union:
$$\text{layout}(M) = \text{Discriminant}\ ||\ \text{Payload}_{\text{max}}\ ||\ \text{Padding}$$

*   **Discriminant:** An integer tag identifying the active state.
*   **Payload:** Storage sufficient for the largest state payload: $\max_{S \in \Sigma_M}(\text{sizeof}(M@S))$.

**Niche Optimization**

Implementations MUST apply niche optimization. If a state payload contains a value with invalid bit patterns (niches), the discriminant MAY be encoded within those bits.

**Niche-Layout-Compatibility**

A type $M@S$ is **niche-layout-compatible** with $M$ if and only if:
1.  $\text{sizeof}(M@S) = \text{sizeof}(M)$
2.  $\text{alignof}(M@S) = \text{alignof}(M)$
3.  The discriminant is fully encoded within the payload niches (no separate tag storage).

**Widening Operation**

The `widen` operation is a **consuming semantic move**. The expression $e$ is evaluated to a value $v$, and the storage for $v$ is consumed to populate the new general instance.

1.  **Consumption:** If $e$ is a Place Expression referring to a `unique` binding, $e$ transitions to the **Moved** state (§3.5).
2.  **Allocation:** Storage is allocated for the general type $M$.
3.  **Transfer:** The payload of $v$ is moved into the payload region of $M$.
4.  **Discriminant:** The discriminant for state $S$ is written to $M$ (unless niche-optimized).

| Type Category               | `widen` Keyword | Implicit Coercion | Subtyping      |
| :-------------------------- | :-------------- | :---------------- | :------------- |
| Non-niche-layout-compatible | Required        | Prohibited        | $M@S \not<: M$ |
| Niche-layout-compatible     | Optional        | Permitted         | $M@S <: M$     |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------- | :----------- | :-------- |
| `E-TYP-2050` | Error    | Modal type declares zero states.                         | Compile-time | Rejection |
| `E-TYP-2051` | Error    | Duplicate state name within modal declaration.           | Compile-time | Rejection |
| `E-TYP-2052` | Error    | Field access on general modal type $M$.                  | Compile-time | Rejection |
| `E-TYP-2053` | Error    | Method/Transition invocation on wrong state.             | Compile-time | Rejection |
| `E-TYP-2055` | Error    | Transition returns incorrect state type.                 | Compile-time | Rejection |
| `E-TYP-2057` | Error    | `widen` required but omitted (layout incompatibility).   | Compile-time | Rejection |
| `E-TYP-2060` | Error    | Non-exhaustive match on modal type.                      | Compile-time | Rejection |
| `W-OPT-4010` | Warning  | Modal widening involves large payload copy (> threshold) | Compile-time | Advisory  |
---

### 6.2 String Types

##### Definition

The **`string`** type is a built-in modal type representing a sequence of UTF-8 encoded Unicode scalar values. It defines two states:
1.  **`@Managed`**: An owning, heap-allocated, mutable string buffer.
2.  **`@View`**: A non-owning, immutable slice (pointer and length).

All `string` values MUST contain valid UTF-8 sequences. This invariant is enforced by constructors and the type system.

##### Syntax & Declaration

**Grammar**

```ebnf
string_type ::= "string" ["@" ("Managed" | "View")]
```

**Literals**

String literals (e.g., `"text"`) are defined in §2.8.

##### Static Semantics

**Typing Rules**

**(T-String-Literal)**
$$\frac{s \in \text{StringLiteral}}{\Gamma \vdash s : \texttt{string@View}}$$

**(T-String-Slice)**
Slicing any string type produces a view.
$$\frac{\Gamma \vdash e : \texttt{string@}S \quad \Gamma \vdash i : \text{Range}\langle \texttt{usize} \rangle}{\Gamma \vdash e[i] : \texttt{string@View}}$$

**Subtyping**

Both states are subtypes of the general type:
$$\Gamma \vdash \texttt{string@Managed} <: \texttt{string}$$
$$\Gamma \vdash \texttt{string@View} <: \texttt{string}$$

**Implements**

*   `string@Managed` implements `Drop`. It does NOT implement `Copy`.
*   `string@View` implements `Copy`.

##### Dynamic Semantics

**Slicing Checks**

Slicing operations ($s[start..end]$) MUST verify at runtime:
1.  $start \le end \le s.\text{len}$ (`P-TYP-2153`, `P-TYP-2154`)
2.  $s[start]$ is a UTF-8 character boundary (`P-TYP-2151`)
3.  $s[end]$ is a UTF-8 character boundary (`P-TYP-2151`)

A byte is a UTF-8 character boundary if it is not a continuation byte (`0b10xxxxxx`).

**Literal Storage**

String literals have static storage duration. Creating a `string@View` from a literal is an $O(1)$ operation.

##### Memory & Layout

**`string@View` Layout**

| Offset | Type      | Description           |
| :----- | :-------- | :-------------------- |
| 0      | `*imm u8` | Pointer to UTF-8 data |
| 1 word | `usize`   | Length in bytes       |

**`string@Managed` Layout**

| Offset  | Type      | Description                |
| :------ | :-------- | :------------------------- |
| 0       | `*mut u8` | Pointer to heap allocation |
| 1 word  | `usize`   | Length in bytes            |
| 2 words | `usize`   | Capacity in bytes          |

**General `string` Layout**

The general `string` type uses a discriminated union layout compatible with the largest state (`@Managed`). Niche optimization MAY be applied.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-TYP-2152` | Error    | Direct byte indexing ($s[i]$) on string. | Compile-time | Rejection |
| `P-TYP-2151` | Panic    | Slice index not on UTF-8 boundary.       | Runtime      | Panic     |
| `P-TYP-2153` | Panic    | Slice start index > end index.           | Runtime      | Panic     |
| `P-TYP-2154` | Panic    | Slice index out of bounds.               | Runtime      | Panic     |

---

### 6.3 Pointer Types

##### Definition

**Pointer types** represent memory addresses. Cursive defines two categories:
1.  **Safe Modal Pointers (`Ptr<T>`)**: A modal type tracking validity state (`@Valid`, `@Null`, `@Expired`).
2.  **Raw Pointers (`*imm T`, `*mut T`)**: Unsafe addresses with no validity tracking.

##### Syntax & Declaration

**Grammar**

```ebnf
safe_ptr_type ::= "Ptr" "<" type ">" ["@" ("Valid" | "Null" | "Expired")]
raw_ptr_type  ::= "*" ("imm" | "mut") type
addr_of_expr  ::= "&" place_expr
```

##### Static Semantics

**Pointer States**

*   **`@Valid`**: Points to initialized, live memory. Dereferenceable.
*   **`@Null`**: Represents the null address. Not dereferenceable.
*   **`@Expired`**: Points to deallocated memory (e.g., exited region). Not dereferenceable.

**Typing Rules**

**(T-Addr-Of)**
$$\frac{\Gamma \vdash e : T \quad \text{IsPlace}(e)}{\Gamma \vdash \&e : \texttt{Ptr}\langle T \rangle\texttt{@Valid}}$$

**(T-Deref-Safe)**
$$\frac{\Gamma \vdash p : \texttt{Ptr}\langle T \rangle\texttt{@Valid}}{\Gamma \vdash *p : T}$$

**(T-Deref-Raw)**
$$\frac{\Gamma \vdash p : *\mu\ T \quad \text{InUnsafeBlock}(\Gamma)}{\Gamma \vdash *p : T}$$

**(T-Cast-Raw)**
$$\frac{\Gamma \vdash p : \texttt{Ptr}\langle T \rangle\texttt{@Valid}}{\Gamma \vdash p \text{ as } *\mu\ T : *\mu\ T}$$

**Provenance and Lifetime**

The `@Expired` state is a compile-time construct enforcing the Escape Rule (§3.3). A pointer derived from a region $R$ transitions to `@Expired` when the scope of $R$ ends.

##### Dynamic Semantics

**Evaluation**

*   `&e`: Evaluates to the address of `e`.
*   `*p`: Accesses memory at the address stored in `p`.
*   `Ptr::null()`: Evaluates to address 0.

##### Memory & Layout

**Safe Pointer Layout**

`Ptr<T>` uses **niche optimization**.
*   `@Valid`: Non-zero address.
*   `@Null`: Zero address.
*   `@Expired`: Non-zero address (distinguished only statically).

Size: `sizeof(usize)`. Alignment: `alignof(usize)`.

**Raw Pointer Layout**

Size: `sizeof(usize)`. Alignment: `alignof(usize)`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-TYP-2101` | Error    | Dereference of `Ptr<T>@Null`.                | Compile-time | Rejection |
| `E-TYP-2102` | Error    | Dereference of `Ptr<T>@Expired`.             | Compile-time | Rejection |
| `E-TYP-2103` | Error    | Raw pointer dereference outside `unsafe`.    | Compile-time | Rejection |
| `E-TYP-2104` | Error    | Address-of operator on value expression.     | Compile-time | Rejection |
| `E-TYP-2105` | Error    | Cast of non-`@Valid` pointer to raw pointer. | Compile-time | Rejection |

---

### 6.4 Function Types

##### Definition

**Function types** describe callable entities. Cursive distinguishes between **sparse function pointers** (code pointer only) and **closures** (code pointer plus environment).

##### Syntax & Declaration

**Grammar**

```ebnf
function_type ::= sparse_type | closure_type
sparse_type   ::= "(" [param_types] ")" "->" return_type
closure_type  ::= "|" [param_types] "|" "->" return_type
param_types   ::= param_type ("," param_type)*
param_type    ::= ["move"] type
```

##### Static Semantics

**Typing Rules**

**(T-Func-Sub)**
Sparse pointers are subtypes of compatible closures.
$$\frac{\Sigma_{sparse} \equiv \Sigma_{closure}}{\Gamma \vdash \Sigma_{sparse} <: \Sigma_{closure}}$$

**(T-Call)**
$$\frac{\Gamma \vdash f : (\overline{P}) \to R \quad \forall i.\ \Gamma \vdash a_i <: P_i}{\Gamma \vdash f(\overline{a}) : R}$$

**Variance**

Function types are contravariant in parameter types and covariant in return type.

**FFI Safety**

*   Sparse function pointers (`(...) -> R`) are **FFI-safe** if all component types are FFI-safe.
*   Closure types (`|...| -> R`) are **NOT FFI-safe**.

##### Memory & Layout

**Sparse Function Pointer**

*   Size: `sizeof(usize)` (1 word).
*   Representation: Address of the function entry point.

**Closure**

*   Size: `2 * sizeof(usize)` (2 words).
*   Representation: `struct { env: *void, code: *fn }`.
*   The `code` pointer expects `env` as an implicit first argument.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-TYP-2220` | Error    | Argument count mismatch.                      | Compile-time | Rejection |
| `E-TYP-2221` | Error    | Argument type incompatibility.                | Compile-time | Rejection |
| `E-TYP-2222` | Error    | Missing `move` at call site for `move` param. | Compile-time | Rejection |
| `E-TYP-2223` | Error    | Closure type used in FFI signature.           | Compile-time | Rejection |

---

## Clause 7: Type Extensions

### 7.1 Static Polymorphism

##### Definition

A **generic declaration** is a declaration parameterized by one or more **type parameters**. Type parameters are abstract placeholders for concrete types supplied at instantiation.

**Monomorphization** is the translation process that generates specialized implementations for each unique sequence of concrete type arguments used in the program.

**Formal Definition**

A generic declaration $D$ is a tuple $(N, \mathcal{P}, B)$ where:
*   $N$ is the declaration identifier.
*   $\mathcal{P} = \langle P_1, \ldots, P_n \rangle$ is an ordered sequence of type parameters.
*   $B$ is the declaration body.

Each type parameter $P_i$ is a pair $(n_i, \mathcal{C}_i)$ where:
*   $n_i$ is the parameter identifier.
*   $\mathcal{C}_i$ is a set of **class bounds** (§10) that constrain valid arguments.

##### Syntax & Declaration

**Grammar**

```ebnf
generic_params     ::= "<" generic_param_list ">"
generic_param_list ::= generic_param (";" generic_param)*
generic_param      ::= identifier [ bound_clause ] [ default_clause ]
bound_clause       ::= "<:" class_bound_list
default_clause     ::= "=" type
class_bound_list   ::= class_bound ("," class_bound)*

where_clause       ::= "where" where_predicates
where_predicates   ::= where_predicate (predicate_sep where_predicate)* [";"]
where_predicate    ::= identifier "<:" class_bound_list
predicate_sep      ::= ";" | NEWLINE

generic_args       ::= "<" type_arg_list ">"
type_arg_list      ::= type ("," type)*
```

**Syntactic Semantics**

1.  **Parameter Separation**: Type parameters within `generic_params` MUST be separated by semicolons (`;`).
2.  **Bound Separation**: Class bounds within a single `bound_clause` MUST be separated by commas (`,`).
3.  **Argument Separation**: Concrete type arguments within `generic_args` MUST be separated by commas (`,`).

##### Static Semantics

**Well-Formedness**

A generic parameter list is well-formed if all parameter names are distinct and all bounds refer to valid classes.

$$\frac{
    \forall i, j \in 1..n.\ i \neq j \implies n_i \neq n_j \quad
    \forall i.\ \forall C \in \mathcal{C}_i.\ \Gamma \vdash C : \text{Class}
}{
    \Gamma \vdash \langle P_1; \ldots; P_n \rangle\ \text{wf}
} \quad \text{(WF-Generic-Params)}$$

**Constraint Satisfaction**

A concrete type argument $A$ satisfies the bounds of parameter $P = (n, \mathcal{C})$ if $A$ implements every class in $\mathcal{C}$.

$$\frac{
    \forall C \in \mathcal{C}.\ \Gamma \vdash A <: C
}{
    \Gamma \vdash A \text{ satisfies } \mathcal{C}
} \quad \text{(Sat-Bounds)}$$

**Instantiation**

A generic instantiation is well-formed if the number of arguments matches the number of parameters and each argument satisfies its corresponding parameter's bounds.

$$\frac{
    D\langle P_1, \ldots, P_n \rangle \text{ declared} \quad
    \forall i \in 1..n.\ \Gamma \vdash A_i \text{ satisfies } \mathcal{C}_i
}{
    \Gamma \vdash D\langle A_1, \ldots, A_n \rangle\ \text{wf}
} \quad \text{(WF-Instantiation)}$$

**Inference**

When type arguments are omitted, the implementation MUST infer them via bidirectional type inference (§4.4). If inference yields multiple solutions or no solution, the program is ill-formed.

**Monomorphization Requirements**

1.  **Specialization**: The implementation MUST generate distinct machine code or data layouts for each unique instantiation tuple $\langle A_1, \ldots, A_n \rangle$.
2.  **Substitution**: The semantics of the instantiated body MUST be identical to the semantics of the generic body with all occurrences of $n_i$ replaced by $A_i$.
3.  **Static Dispatch**: Procedure calls within the specialized body MUST use static dispatch. Virtual table lookup is prohibited for generic resolution.

**Recursion Limits**

The implementation MUST detect and reject infinite monomorphization recursion chains. The maximum recursion depth is Implementation-Defined Behavior (IDB).

##### Memory & Layout

**Layout Independence**

Each instantiation of a generic type has an independent memory layout derived from its concrete type arguments.

$$\text{sizeof}(D\langle \vec{A} \rangle) = \text{sizeof}(D[\vec{A}/\vec{P}])$$
$$\text{alignof}(D\langle \vec{A} \rangle) = \text{alignof}(D[\vec{A}/\vec{P}])$$

##### Constraints & Legality

**Negative Constraints**

1.  Generic parameters MUST NOT appear in `extern` procedure signatures (§21).
2.  A class bound MUST NOT reference a non-class type.
3.  A generic instantiation MUST NOT supply partial arguments.

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-SYN-0701` | Error    | Comma used as type parameter separator (expected `;`). | Compile-time | Rejection |
| `E-TYP-2301` | Error    | Type arguments cannot be inferred.                     | Compile-time | Rejection |
| `E-TYP-2302` | Error    | Type argument does not satisfy class bound.            | Compile-time | Rejection |
| `E-TYP-2303` | Error    | Argument count mismatch in instantiation.              | Compile-time | Rejection |
| `E-TYP-2304` | Error    | Duplicate type parameter name.                         | Compile-time | Rejection |
| `E-TYP-2305` | Error    | Class bound references non-class type.                 | Compile-time | Rejection |
| `E-TYP-2306` | Error    | Generic parameter in `extern` signature.               | Compile-time | Rejection |
| `E-TYP-2307` | Error    | Infinite monomorphization recursion.                   | Compile-time | Rejection |
| `E-TYP-2308` | Error    | Monomorphization depth limit exceeded.                 | Compile-time | Rejection |

---

### 7.2 Attributes

##### Definition

An **attribute** is a compile-time metadata annotation attached to a declaration or expression. Attributes modify static semantics, code generation, or verification behavior.

The **Attribute Registry** $\mathcal{R}$ contains all valid attribute identifiers. $\mathcal{R}$ is the union of specification-defined attributes and vendor-defined attributes.

##### Syntax & Declaration

**Grammar**

```ebnf
attribute_list ::= attribute+
attribute      ::= "[[" attribute_spec ("," attribute_spec)* "]]"
attribute_spec ::= attribute_name [ "(" attribute_args ")" ]
attribute_name ::= identifier | vendor_path
vendor_path    ::= identifier ("." identifier)+
attribute_args ::= attribute_arg ("," attribute_arg)*
attribute_arg  ::= literal | identifier | identifier ":" literal | identifier "(" attribute_args ")"
```

**Placement**

*   **Declaration Attributes**: MUST appear immediately preceding the target declaration.
*   **Expression Attributes**: MUST appear immediately preceding the target expression.

##### Static Semantics

**Processing**

For every attribute $A$ applied to target $T$:
1.  Verify $A.\text{name} \in \mathcal{R}$.
2.  Verify $T$ is a valid target for $A$ per the **Attribute Target Matrix**.
3.  Verify $A.\text{args}$ conform to the attribute's schema.

**Attribute Target Matrix**

| Attribute                  | Valid Targets                   | Reference |
| :------------------------- | :------------------------------ | :-------- |
| `[[layout]]`               | `record`, `enum`                | §7.2.1    |
| `[[inline]]`               | `procedure`                     | §7.2.2    |
| `[[cold]]`                 | `procedure`                     | §7.2.3    |
| `[[deprecated]]`           | Any declaration                 | §7.2.4    |
| `[[reflect]]`              | `record`, `enum`, `modal`       | §7.2.5    |
| `[[link_name]]`            | `procedure` (extern)            | §21.3     |
| `[[no_mangle]]`            | `procedure`                     | §21.5     |
| `[[unwind]]`               | `procedure` (extern)            | §21.3     |
| `[[relaxed]]`              | Memory access expression        | §14.10    |
| `[[acquire]]`              | Memory access expression        | §14.10    |
| `[[release]]`              | Memory access expression        | §14.10    |
| `[[acq_rel]]`              | Memory access expression        | §14.10    |
| `[[seq_cst]]`              | Memory access expression        | §14.10    |
| `[[dynamic]]`              | Any declaration, Any expression | §7.2.9    |
| `[[static_dispatch_only]]` | `procedure` (in class)          | §7.2.10   |

**Vendor Attributes**

Vendor-defined attributes MUST use a reverse-domain namespace (e.g., `[[com.example.attr]]`). The `cursive` top-level namespace is reserved.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                            | Detection    | Effect    |
| :----------- | :------- | :----------------------------------- | :----------- | :-------- |
| `E-DEC-2450` | Error    | Malformed attribute syntax.          | Compile-time | Rejection |
| `E-DEC-2451` | Error    | Unknown attribute name.              | Compile-time | Rejection |
| `E-DEC-2452` | Error    | Attribute applied to invalid target. | Compile-time | Rejection |

---

#### 7.2.1 Layout Attribute

##### Definition

The `[[layout(...)]]` attribute specifies the memory representation strategy for a type.

##### Static Semantics

**Arguments**

The attribute accepts a comma-separated list of layout directives:

*   `C`: Enforces C-compatible layout ordering and padding.
*   `packed`: Removes all inter-field padding. Sets alignment to 1.
*   `align(N)`: Enforces minimum alignment of $N$ bytes. $N$ MUST be a power of two.
*   *IntegerType* (`u8`, `i32`, etc.): Sets the discriminant backing type.

**Validation Rules**

1.  `C` applies to `record` and `enum`.
2.  `packed` applies only to `record`.
3.  `align(N)` applies to `record` and `enum`.
4.  *IntegerType* applies only to `enum`.
5.  `packed` and `align(N)` are mutually exclusive.

**Record Layout Semantics**

*   **Default**: Implementation-Defined field ordering (optimization permitted).
*   **`layout(C)`**: Fields laid out in declaration order. Padding inserted to match target C ABI.
*   **`layout(packed)`**: Fields laid out in declaration order with 0 padding bytes. Field access may be unaligned. Taking a reference to a packed field is **Unverifiable Behavior** (UVB).

**Enum Layout Semantics**

*   **Default**: Implementation-Defined discriminant size.
*   **`layout(IntType)`**: Discriminant MUST be stored as `IntType`.
*   **`layout(C)`**: Discriminant size and union layout match target C ABI.

| Operation                          | Classification         |
| :--------------------------------- | :--------------------- |
| `packed.field = value`             | Direct write (safe)    |
| `let x = packed.field`             | Direct read (safe)     |
| `packed.field.method()`            | Reference-taking (UVB) |
| Pass to `move` parameter           | Direct read (safe)     |
| Pass to `const`/`unique` parameter | Reference-taking (UVB) |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-DEC-2453` | Error    | `align(N)` where N is not a power of two.          | Compile-time | Rejection |
| `E-DEC-2454` | Error    | `packed` applied to non-record type.               | Compile-time | Rejection |
| `E-DEC-2455` | Error    | Conflicting layout arguments (`packed` + `align`). | Compile-time | Rejection |

---

#### 7.2.2 Inline Attribute

##### Definition

The `[[inline(...)]]` attribute provides a compilation hint regarding procedure inlining.

##### Static Semantics

**Arguments**

*   `always`: The implementation SHOULD inline the procedure.
*   `never`: The implementation MUST NOT inline the procedure.
*   `default`: The implementation uses default heuristics.

**Constraints**

If `always` is specified but inlining is impossible (e.g., recursion, virtual call), the implementation MUST emit a warning.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                  | Detection    | Effect |
| :----------- | :------- | :----------------------------------------- | :----------- | :----- |
| `W-DEC-2452` | Warning  | `inline(always)` requested but impossible. | Compile-time | None   |

---

#### 7.2.3 Cold Attribute

##### Definition

The `[[cold]]` attribute hints that a procedure is unlikely to be executed.

##### Static Semantics

The implementation MAY use this hint to optimize code placement (e.g., moving to cold sections) or branch prediction. The attribute has no semantic effect on program behavior.

---

#### 7.2.4 Deprecated Attribute

##### Definition

The `[[deprecated(...)]]` attribute marks a declaration as obsolete.

##### Static Semantics

**Arguments**

*   Optional string literal: Message to include in diagnostic.

**Behavior**

Usage of a marked declaration MUST trigger a warning diagnostic containing the provided message.

---

#### 7.2.5 Reflect Attribute

##### Definition

The `[[reflect]]` attribute enables compile-time introspection metadata for the target type.

##### Static Semantics

Types marked `[[reflect]]` are visible to the `reflect_type` intrinsic during the Metaprogramming Phase. See Clause 18.

---

#### 7.2.9 Dynamic Attribute

##### Definition

The `[[dynamic]]` attribute opts in to runtime verification for safety properties that cannot be proven statically.

##### Static Semantics

**Attribute Scope**

An expression $e$ is in a **dynamic scope** if:
1.  $e$ is lexically enclosed by a declaration marked `[[dynamic]]`.
2.  $e$ is lexically enclosed by an expression marked `[[dynamic]]`.

**Effects**

Within a dynamic scope:
1.  **Key System**: Runtime synchronization is inserted if static key safety cannot be proven (§14.9).
2.  **Contracts**: Runtime checks are inserted if preconditions/postconditions cannot be proven statically (§11.4).
3.  **Refinements**: Runtime checks are inserted if refinement predicates cannot be proven statically (§7.3).

**Static Elision**

Even within a dynamic scope, if a property is statically provable, the implementation MUST NOT emit runtime checks.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-DYN-0010` | Error    | `[[dynamic]]` applied directly to contract clause. | Compile-time | Rejection |
| `E-DYN-0011` | Error    | `[[dynamic]]` applied to type alias.               | Compile-time | Rejection |
| `W-DYN-0001` | Warning  | `[[dynamic]]` present but all proofs succeed.      | Compile-time | None      |

---

#### 7.2.10 Static Dispatch Only Attribute

##### Definition

The `[[static_dispatch_only]]` attribute excludes a procedure from the virtual method table (vtable) of its enclosing class.

##### Static Semantics

**Target**

Valid only on `procedure` declarations within a `class`.

**Effect**

1.  The procedure is NOT required to be vtable-eligible (may be generic or return `Self`).
2.  The procedure is NOT callable via a `$Class` reference.
3.  The enclosing class remains dispatchable if all other methods are vtable-eligible.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-TRS-2940` | Error    | Call to static-only method on `dyn` object. | Compile-time | Rejection |

---

### 7.3 Refinement Types

##### Definition

A **refinement type** $T \text{ where } \{ P \}$ restricts a base type $T$ to the subset of values for which the predicate $P$ holds.

**Formal Definition**

A refinement type $R$ is a pair $(T, P)$ where:
*   $T$ is a base type.
*   $P: T \to \texttt{bool}$ is a pure predicate.

Values of $R$ are $\{ v \in \text{Values}(T) \mid P(v) \Downarrow \texttt{true} \}$.

##### Syntax & Declaration

**Grammar**

```ebnf
refinement_type   ::= type "where" "{" predicate "}"
predicate         ::= expression
```

**Context**

Within `predicate`, the identifier `self` is bound to the value being constrained.

##### Static Semantics

**Well-Formedness**

A refinement type is well-formed if the predicate $P$ is a pure expression of type `bool`.

$$\frac{
    \Gamma \vdash T\ \text{wf} \quad
    \Gamma, \text{self}: T \vdash P : \texttt{bool} \quad
    \text{Pure}(P)
}{
    \Gamma \vdash T \text{ where } \{ P \}\ \text{wf}
} \quad \text{(WF-Refinement)}$$

**Subtyping**

A refinement type is a proper subtype of its base type.

$$\Gamma \vdash (T \text{ where } \{ P \}) <: T$$

A refinement with a stronger predicate is a subtype of a refinement with a weaker predicate.

$$\frac{\Gamma \vdash P \implies Q}{\Gamma \vdash (T \text{ where } \{ P \}) <: (T \text{ where } \{ Q \})}$$

**Type Equivalence**

Two refinement types are equivalent if their base types are equivalent and their predicates are logically equivalent.

**Decidable Subset**

The implementation MUST support static verification for the following predicate subset:
1.  Comparisons with literals.
2.  Linear integer arithmetic inequalities.
3.  Boolean algebra combinations.
4.  Structural equality of expressions.

For predicates outside this subset, static verification is Implementation-Defined.

##### Dynamic Semantics

**Verification Strategy**

When a value $v$ is coerced to $T \text{ where } \{ P \}$:
1.  **Static**: If the compiler proves $P(v)$ is true, no check is emitted.
2.  **Dynamic**: If static proof fails and the context is `[[dynamic]]`, a runtime check `assert(P(v))` is emitted.
3.  **Failure**: If static proof fails and the context is NOT `[[dynamic]]`, the program is ill-formed.

**Runtime Check Failure**

If a runtime check for $P(v)$ evaluates to `false`, the executing thread MUST panic.

##### Constraints & Legality

**Negative Constraints**

1.  The predicate MUST be pure (§11.1.1).
2.  The predicate MUST NOT capture mutable bindings from the environment.
3.  The predicate MUST evaluate to `bool`.

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-TYP-1950` | Error    | `self` used in inline parameter constraint.           | Compile-time | Rejection |
| `E-TYP-1951` | Error    | Predicate is not boolean.                             | Compile-time | Rejection |
| `E-TYP-1952` | Error    | Circular dependency in refinement predicate.          | Compile-time | Rejection |
| `E-TYP-1953` | Error    | Predicate not statically provable (no `[[dynamic]]`). | Compile-time | Rejection |
| `P-TYP-1953` | Panic    | Runtime refinement check failed.                      | Runtime      | Panic     |

---

## Clause 8: Modules and Name Resolution

### 8.1 Module System Architecture

##### Definition

The **Module System** organizes code into hierarchical units for compilation, encapsulation, and distribution.

A **Project** is the root organizational unit defined by a manifest file. A project comprises one or more **Assemblies**.

An **Assembly** is a deployable unit of code consisting of a set of modules. An assembly defines a coherent namespace root and serves as the boundary for `internal` visibility.

A **Module** is the fundamental unit of name resolution and encapsulation. A module corresponds exactly to a filesystem directory containing source files.

A **Compilation Unit** is the aggregate of all source text files contributing to a single module.

##### Static Semantics

**Folder-as-Module Principle**

A directory containing one or more source files MUST be treated as a single module. All source files within the directory contribute to the module's unified scope. A module MUST NOT span multiple directories.

**Source File Processing**

The order in which source files within a compilation unit are processed is Implementation-Defined Behavior (IDB). The semantic interpretation of a module MUST be independent of the processing order of its constituent files.

**Namespace Root**

Every assembly MUST have exactly one root module. The root module serves as the entry point for path resolution into the assembly.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                                 | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------------ | :----------- | :-------- |
| `E-MOD-1100` | Error    | Source file location violates module structure boundaries.                | Compile-time | Rejection |
| `E-MOD-1110` | Error    | Module semantics depend on file processing order (ambiguous definitions). | Compile-time | Rejection |

---

### 8.2 Project Manifest

##### Definition

A **Project Manifest** is a configuration file named `Cursive.toml` located at the project root. It defines project metadata, dependencies, and assembly configurations.

##### Syntax & Declaration

The manifest MUST be a well-formed TOML document encoding the following schema:

**Project Table** (`[project]`)
*   `name` (string): The project identifier.
*   `version` (string): The semantic version of the project.

**Language Table** (`[language]`)
*   `version` (string): The target Cursive language version.

**Paths Table** (`[paths]`)
*   Keys are symbolic identifiers.
*   Values are relative paths to source roots.

**Assembly Table** (`[[assembly]]`)
*   `name` (string): The assembly identifier.
*   `root` (string): A key referencing an entry in the `[paths]` table.
*   `path` (string): The relative path to the assembly root module within the source root.
*   `type` (string, optional): `"library"` (default) or `"executable"`.

##### Static Semantics

**Validation**

A manifest is well-formed if and only if:
1. It contains exactly one `[project]` table with valid `name` and `version` fields.
2. It contains exactly one `[language]` table with a version supported by the implementation.
3. It contains one or more `[[assembly]]` tables.
4. Every assembly's `root` field references a valid key in the `[paths]` table.
5. All file paths specified in `[paths]` and `[[assembly]]` resolve to existing locations.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                            | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :----------- | :-------- |
| `E-MOD-1101` | Error    | Manifest file `Cursive.toml` not found or malformed. | Compile-time | Rejection |
| `E-MOD-1102` | Error    | `[paths]` table missing or invalid.                  | Compile-time | Rejection |
| `E-MOD-1103` | Error    | Assembly references undefined source root.           | Compile-time | Rejection |
| `E-MOD-1107` | Error    | `[project]` table missing required fields.           | Compile-time | Rejection |
| `E-MOD-1108` | Error    | Duplicate assembly name.                             | Compile-time | Rejection |
| `E-MOD-1109` | Error    | Incompatible language version declared.              | Compile-time | Rejection |

---

### 8.3 Module Discovery and Paths

##### Definition

A **Module Path** is a sequence of identifiers uniquely locating a module within the project structure. Module paths are derived from the filesystem hierarchy relative to the assembly root.

##### Syntax & Declaration

**Grammar**

```ebnf
module_path ::= identifier ("::" identifier)*
```

##### Static Semantics

**Path Derivation**

Let $R$ be the filesystem path of the assembly root module.
Let $M$ be the filesystem path of a module directory.
The module path $P$ is derived as follows:

1. Let $S$ be the relative path from $R$ to $M$.
2. The components of $P$ are the path segments of $S$.
3. The separator is `::`.

**Identifier Validation**

Every segment of a derived module path MUST be a valid Cursive identifier (§2.5) and MUST NOT be a reserved keyword (§2.6).

**Case Sensitivity**

Module path resolution MUST be case-sensitive. On filesystems that are not case-sensitive, if two directories differ only in case and map to the same module path, the program is ill-formed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-MOD-1104` | Error    | Module path collision (case-insensitive filesystem).  | Compile-time | Rejection |
| `E-MOD-1105` | Error    | Module path component is a reserved keyword.          | Compile-time | Rejection |
| `E-MOD-1106` | Error    | Module path component is not a valid identifier.      | Compile-time | Rejection |
| `W-MOD-1101` | Warning  | Potential path collision on case-insensitive systems. | Compile-time | N/A       |

---

### 8.4 Scope Context Structure

##### Definition

**Scope** is the textual region where a name binding is valid.
The **Scope Context** ($\Gamma$) is the ordered sequence of nested scopes active at a program point.

##### Static Semantics

**Context Structure**

The scope context is an ordered list:
$$\Gamma = [S_{\text{local}}, \dots, S_{\text{proc}}, S_{\text{module}}, S_{\text{universe}}]$$

1.  **$S_{\text{local}}$**: Block-local bindings (innermost).
2.  **$S_{\text{proc}}$**: Procedure parameters and type parameters.
3.  **$S_{\text{module}}$**: Top-level module declarations and imports.
4.  **$S_{\text{universe}}$**: Built-in types and the `cursive` namespace (outermost).

**Unified Namespace**

Each scope $S$ MUST maintain a single, unified mapping from identifiers to entities. Terms, types, and modules share the same namespace.

$$\text{Entity} ::= \text{Term} \mid \text{Type} \mid \text{Module} \mid \text{Form}$$

**Uniqueness Constraint**

An identifier MUST NOT be bound to more than one entity within the same scope.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-NAM-1302` | Error    | Duplicate identifier definition in scope. | Compile-time | Rejection |

---

### 8.5 Visibility and Access Control

##### Definition

**Visibility** determines the set of modules from which a declaration may be accessed.

##### Syntax & Declaration

**Grammar**

```ebnf
visibility ::= "public" | "internal" | "private" | "protected"
```

**Defaults**

Top-level declarations default to `internal`.
Members of types default to `private`.

##### Static Semantics

**Visibility Levels**

1.  **`public`**: Accessible from any module that can resolve the name.
2.  **`internal`**: Accessible only from modules within the same assembly.
3.  **`private`**: Accessible only from the defining module.
4.  **`protected`**: Accessible from the defining module and within implementations of the defining type or class.

**Access Judgment**

Let $D$ be a declaration with visibility $V$ in module $M_{def}$ of assembly $A_{def}$.
Let $M_{acc}$ be the accessing module in assembly $A_{acc}$.
Access is permitted if $\text{CanAccess}(M_{acc}, D)$ holds:

*   If $V = \texttt{public}$, $\text{CanAccess}$ is true.
*   If $V = \texttt{internal}$, $\text{CanAccess}$ is true iff $A_{acc} = A_{def}$.
*   If $V = \texttt{private}$, $\text{CanAccess}$ is true iff $M_{acc} = M_{def}$.
*   If $V = \texttt{protected}$:
    *   Allowed if $M_{acc} = M_{def}$.
    *   Allowed if access occurs within the scope of a type $T$ such that $T$ implements the type containing $D$.

##### Constraints & Legality

**Negative Constraints**

The `protected` modifier MUST NOT be applied to top-level declarations.

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-NAM-1305` | Error    | Access denied by visibility rules.     | Compile-time | Rejection |
| `E-DEC-2440` | Error    | `protected` applied to top-level item. | Compile-time | Rejection |
| `E-MOD-1207` | Error    | Invalid access to `protected` member.  | Compile-time | Rejection |

---

### 8.6 Import and Use Declarations

##### Definition

An **Import Declaration** establishes a dependency on a module from an external assembly.
A **Use Declaration** creates a local alias for a path, introducing it into the current scope.
A **Re-export** makes an imported item part of the current module's public interface.

##### Syntax & Declaration

**Grammar**

```ebnf
top_level_item ::= import_decl | use_decl | ...

import_decl    ::= "import" module_path [ "as" identifier ]

use_decl       ::= [ "public" ] "use" use_target
use_target     ::= use_path [ "as" identifier ]
                 | use_path "::" use_list
                 | use_path "::" "*"

use_path       ::= module_path [ "::" identifier ]
use_list       ::= "{" use_specifier ( "," use_specifier )* [ "," ] "}"
use_specifier  ::= identifier [ "as" identifier ]
                 | "self" [ "as" identifier ]
```

##### Static Semantics

**Import Semantics**

An `import` declaration adds a module binding to $S_{\text{module}}$.
If `as` is omitted, the binding name is the last component of the module path.
Importing a module from an external assembly is required before using its contents.

**Use Semantics**

A `use` declaration resolves a path to an entity and binds it in $S_{\text{module}}$.
*   **Simple Use**: Binds the target entity to its name or the provided alias.
*   **List Use**: Binds multiple entities from a common parent. `self` refers to the parent path itself.
*   **Wildcard Use (`*`)**: Binds all `public` (and `internal` if intra-assembly) members of the target module.

**Re-export Semantics**

A `use` declaration prefixed with `public` is a re-export. The bound name has `public` visibility in the current module.
The target of a re-export MUST be `public`.

**Resolution**

Paths in `use` declarations are resolved relative to the universe scope.

##### Constraints & Legality

**Negative Constraints**

1.  Circular assembly dependencies introduced by imports are prohibited.
2.  `public use` MUST NOT target an item with `internal` or `private` visibility.
3.  A `use` declaration MUST NOT introduce a name that conflicts with an existing binding in the scope.
4.  Wildcard imports MUST NOT be used in modules that export a public API (Warning only).

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-MOD-1201` | Error    | Circular assembly dependency.      | Compile-time | Rejection |
| `E-MOD-1202` | Error    | Import target not found.           | Compile-time | Rejection |
| `E-MOD-1203` | Error    | Name conflict in import/use.       | Compile-time | Rejection |
| `E-MOD-1204` | Error    | Use path resolution failed.        | Compile-time | Rejection |
| `E-MOD-1205` | Error    | Re-export of non-public item.      | Compile-time | Rejection |
| `E-MOD-1206` | Error    | Duplicate item in use list.        | Compile-time | Rejection |
| `W-MOD-1201` | Warning  | Wildcard use in public API module. | Compile-time | N/A       |

---

### 8.7 Name Resolution

##### Definition

**Name Resolution** is the process of associating an identifier reference with its declaration.

##### Static Semantics

**Unqualified Lookup**

To resolve an identifier $x$:
1.  Search scopes in $\Gamma$ from innermost ($S_{\text{local}}$) to outermost ($S_{\text{universe}}$).
2.  The result is the entity bound to $x$ in the first scope where $x$ is defined.
3.  If no binding is found, the program is ill-formed.

**Qualified Lookup**

To resolve a path $x::y$:
1.  Resolve $x$ to a module or type entity $E$.
2.  Search for $y$ in the namespace of $E$.
3.  Verify visibility per §8.5.

**Shadowing**

A declaration in an inner scope MAY shadow a binding in an outer scope.
*   **Explicit Shadowing**: If the `shadow` keyword is present on the declaration, shadowing is valid.
*   **Implicit Shadowing**: If the `shadow` keyword is absent, the behavior depends on the conformance mode:
    *   **Strict Mode**: Ill-formed.
    *   **Permissive Mode**: Valid with warning.

It is ill-formed to use the `shadow` keyword if no outer binding is shadowed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-NAM-1301` | Error    | Unresolved identifier.                 | Compile-time | Rejection |
| `E-NAM-1303` | Error    | Implicit shadowing (Strict Mode).      | Compile-time | Rejection |
| `W-NAM-1303` | Warning  | Implicit shadowing (Permissive Mode).  | Compile-time | N/A       |
| `E-NAM-1304` | Error    | Resolution failed for path prefix.     | Compile-time | Rejection |
| `E-NAM-1306` | Error    | `shadow` used but no binding shadowed. | Compile-time | Rejection |

---

### 8.8 Module Initialization

##### Definition

**Module Initialization** is the execution of module-level variable definitions.

**Eager Dependency**: A dependency where module $A$ evaluates a value from module $B$ during initialization.
**Lazy Dependency**: A dependency where module $A$ references types or deferred values from module $B$.

##### Static Semantics

**Dependency Graph**

Let $G = (V, E)$ be the dependency graph where $V$ is the set of modules.
An edge $(A, B) \in E$ exists if $A$ contains an eager dependency on $B$.

**Acyclicity**

The graph of eager dependencies MUST be acyclic.

##### Dynamic Semantics

**Execution Order**

1.  Compute the topological sort of the eager dependency graph.
2.  Initialize modules in topological order.
3.  Within a module, initialize definitions in lexical order.

**Failure**

If a module initializer panics, program execution aborts.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                       | Detection    | Effect    |
| :----------- | :------- | :------------------------------ | :----------- | :-------- |
| `E-MOD-1401` | Error    | Cyclic eager module dependency. | Compile-time | Rejection |

---

### 8.9 Program Entry Point

##### Definition

The **Entry Point** is the procedure where program execution begins.

##### Syntax & Declaration

The entry point MUST be a public procedure named `main` in the root module of an executable assembly.

```cursive
public procedure main(ctx: Context) -> i32
```

##### Static Semantics

**Signature Requirements**

The `main` procedure MUST satisfy:
1.  Name is `main`.
2.  Visibility is `public`.
3.  Parameters: Exactly one parameter of type `Context` (see Appendix H).
4.  Return type: `i32`.
5.  Generics: No type parameters.

**No Ambient Authority**

Global bindings MUST NOT be mutable (`var`) if they are `public`. Mutable global state MUST be confined to module-private scope or passed via the `Context` capability.

##### Dynamic Semantics

**Execution**

1.  Runtime initializes the `Context` record with system capabilities.
2.  Runtime performs module initialization (§8.8).
3.  Runtime invokes `main(ctx)`.
4.  Program terminates with the exit code returned by `main`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                       | Detection    | Effect    |
| :----------- | :------- | :------------------------------ | :----------- | :-------- |
| `E-DEC-2430` | Error    | Multiple `main` procedures.     | Compile-time | Rejection |
| `E-DEC-2431` | Error    | Invalid `main` signature.       | Compile-time | Rejection |
| `E-DEC-2432` | Error    | `main` has generic parameters.  | Compile-time | Rejection |
| `E-DEC-2433` | Error    | Public mutable global variable. | Compile-time | Rejection |

---

## Clause 9: Procedures and Methods

### 9.1 Procedure Declarations

##### Definition

A **procedure** is a named, callable unit of computation consisting of a signature, an optional contract, and an executable body. Procedures serve as the primary abstraction for behavior and scope encapsulation.

**Formal Definition**

A procedure declaration $P$ is defined as a tuple:

$$P = (N, V, G, \Sigma, C, B)$$

where:
- $N$ is the procedure identifier.
- $V \in \{\texttt{public}, \texttt{internal}, \texttt{protected}, \texttt{private}\}$ is the visibility modifier.
- $G$ is the sequence of generic parameters (possibly empty).
- $\Sigma = (\Pi, R)$ is the signature, comprising parameter list $\Pi$ and return type $R$.
- $C$ is the set of contract clauses (preconditions and postconditions).
- $B$ is the statement block forming the procedure body.

##### Syntax & Declaration

**Grammar**

```ebnf
procedure_decl   ::= [visibility] "procedure" identifier [generic_params]
                     signature [contract_clauses] block

signature        ::= "(" [param_list] ")" ["->" return_type]

param_list       ::= param ("," param)* [","]

param            ::= [param_mode] identifier ":" type

param_mode       ::= "move" | permission_prefix

permission_prefix ::= "const" | "unique" | "shared"

return_type      ::= type | union_return

union_return     ::= type ("|" type)+
```

The `contract_clauses` production is defined in §11.1. The `block` production is defined in §12.7.

**Parameter Modes**

The `param_mode` determines the permission and responsibility semantics for the parameter binding:

| Mode           | Syntax        | Permission | Responsibility | Description                   |
| :------------- | :------------ | :--------- | :------------- | :---------------------------- |
| Default        | `x: T`        | `const`    | Caller         | Read-only view                |
| Explicit Const | `const x: T`  | `const`    | Caller         | Read-only view (explicit)     |
| Unique         | `unique x: T` | `unique`   | Caller         | Exclusive mutable access      |
| Shared         | `shared x: T` | `shared`   | Caller         | Synchronized mutable access   |
| Move           | `move x: T`   | `unique`   | Callee         | Ownership transfer (consumes) |

**Return Type Defaults**

If the `-> return_type` clause is omitted, the return type is implicitly the unit type `()`.

##### Static Semantics

**Typing Rules**

**(T-Proc)**
A procedure declaration is well-typed if the body $B$ has type $R$ under a context extended with the parameters:

$$\frac{
  \Gamma' = \Gamma, p_1 : T_1, \ldots, p_n : T_n \quad
  \Gamma' \vdash B : R_{actual} \quad
  \Gamma' \vdash R_{actual} <: R \quad
  \text{AllPathsReturn}(B)
}{
  \Gamma \vdash \texttt{procedure } N(p_1: T_1, \ldots, p_n: T_n) \rightarrow R\ \{B\} : \text{ok}
}$$

**Well-Formedness**

A procedure declaration is well-formed if and only if:
1. The identifier $N$ is unique within the enclosing scope.
2. All parameter identifiers $p_i$ are distinct.
3. All parameter types $T_i$ and the return type $R$ are well-formed.
4. The body $B$ satisfies the declared return type on all control flow paths.
5. If the procedure is `public`, all types in $\Sigma$ MUST be `public`.

##### Constraints & Legality

**Control Flow Constraints**

If $R \neq \texttt{()}$ and $R \neq \texttt{!}$, the procedure body MUST NOT implicitly fall through the closing brace. Every control flow path MUST terminate with a `return` statement, a `result` statement (if the body is a block evaluating to $R$), or a diverging expression.

**Diagnostic Table**

| Code          | Severity | Condition                                                   | Detection    | Effect    |
| :------------ | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-PROC-3001` | Error    | Duplicate parameter name in procedure signature.            | Compile-time | Rejection |
| `E-PROC-3002` | Error    | Procedure body type incompatible with declared return type. | Compile-time | Rejection |
| `E-PROC-3003` | Error    | Control flow falls through without returning value.         | Compile-time | Rejection |
| `E-PROC-3005` | Error    | Public procedure exposes non-public type in signature.      | Compile-time | Rejection |

---

### 9.2 Method Definitions and Receivers

##### Definition

A **method** is a procedure associated with a nominal type (Record, Enum, or Modal). Methods are distinguished from standalone procedures by the presence of a **receiver parameter** (`self`) as the first parameter.

##### Syntax & Declaration

**Grammar**

```ebnf
method_def     ::= [visibility] "procedure" identifier "(" receiver ["," param_list] ")"
                   ["->" return_type] [contract_clauses] block

receiver       ::= receiver_shorthand | explicit_receiver

receiver_shorthand ::= "~" | "~!" | "~%"

explicit_receiver  ::= [param_mode] "self" ":" type
```

**Shorthand Expansion**

Receiver shorthands expand to explicit `self` parameters typed as the enclosing type `Self`:

| Shorthand | Expansion           | Permission | Semantics                   |
| :-------- | :------------------ | :--------- | :-------------------------- |
| `~`       | `self: const Self`  | `const`    | Immutable view              |
| `~!`      | `self: unique Self` | `unique`   | Exclusive mutable access    |
| `~%`      | `self: shared Self` | `shared`   | Synchronized mutable access |

**Constraint**: Explicit receiver syntax MUST NOT use a type other than `Self` or the name of the enclosing type.

##### Static Semantics

**Method Typing Context**

Within a method body, the identifier `self` is bound to the receiver with the declared permission. The type `Self` is bound to the enclosing type.

**(T-Method-Body)**
$$\frac{
    \Gamma, \texttt{self} : P_{\text{recv}}\ \texttt{Self} \vdash \text{body} : R
}{
    \Gamma \vdash \text{method declaration is well-typed}
}$$

**Receiver Compatibility**

A method with receiver permission $P_{\text{target}}$ may be invoked on a path with permission $P_{\text{source}}$ if and only if $P_{\text{source}} <: P_{\text{target}}$ according to the permission lattice defined in §4.5.

**Dispatch Matrix**

| Source Permission | Target: `~` (`const`) | Target: `~!` (`unique`) | Target: `~%` (`shared`) |
| :---------------- | :-------------------: | :---------------------: | :---------------------: |
| `const`           |       **Valid**       |         Invalid         |         Invalid         |
| `unique`          |       **Valid**       |        **Valid**        |        **Valid**        |
| `shared`          |       **Valid**       |         Invalid         |        **Valid**        |

**Key Acquisition**

When a method with `shared` receiver (`~%`) is invoked on a `shared` path, the implementation MUST logically acquire a Write key for the receiver path for the duration of the call. When a method with `const` receiver (`~`) is invoked on a `shared` path, the implementation MUST logically acquire a Read key.

##### Constraints & Legality

**Placement**

Methods MUST be defined within the scope of a `record`, `enum`, `modal`, or `class` declaration.

**Diagnostic Table**

| Code          | Severity | Condition                                                   | Detection    | Effect    |
| :------------ | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-PROC-3010` | Error    | Method invocation violates receiver permission constraints. | Compile-time | Rejection |
| `E-PROC-3011` | Error    | Method defined outside of a type or class context.          | Compile-time | Rejection |
| `E-PROC-3012` | Error    | Explicit receiver type does not match enclosing type.       | Compile-time | Rejection |

---

### 9.3 Parameter Passing and Binding

##### Static Semantics

**Evaluation Order**

Arguments in a procedure call MUST be evaluated in strict left-to-right order before the procedure body is entered.

**Binding Semantics**

1.  **By Value**: If the parameter type implements `Copy` and the mode is not `move`, the value is copied.
2.  **By Reference**: If the parameter mode is `const`, `unique`, or `shared`, and the type does not implement `Copy` (or for optimization of large types), the argument is passed by reference. The alias analysis model (§3.1) enforces safety.
3.  **By Move**: If the parameter mode is `move`, the caller transfers responsibility to the callee. The argument expression MUST be a valid candidate for a move operation.

**The `move` Keyword Requirement**

If a parameter is declared with `move`, the corresponding argument at the call site MUST be prefixed with the `move` keyword.

$$\frac{
    \text{param}_i = \texttt{move } x: T \quad \text{arg}_i \neq \texttt{move } e
}{
    \text{ill-formed (E-PROC-3020)}
}$$

If a parameter is NOT declared with `move`, the argument MUST NOT be prefixed with `move`.

$$\frac{
    \text{param}_i \neq \texttt{move } \ldots \quad \text{arg}_i = \texttt{move } e
}{
    \text{ill-formed (E-PROC-3021)}
}$$

##### Dynamic Semantics

**Parameter Initialization**

Upon procedure entry, a new stack frame is allocated. Parameters are initialized with the evaluated argument values (or references) in declaration order.

##### Constraints & Legality

**Diagnostic Table**

| Code          | Severity | Condition                                       | Detection    | Effect    |
| :------------ | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-PROC-3020` | Error    | Missing `move` keyword at call site.            | Compile-time | Rejection |
| `E-PROC-3021` | Error    | Superfluous `move` keyword at call site.        | Compile-time | Rejection |
| `E-PROC-3022` | Error    | Argument type not assignable to parameter type. | Compile-time | Rejection |

---

### 9.4 Procedure Execution Model

##### Definition

The **Procedure Execution Model** defines the runtime behavior of procedure invocation, execution, and termination.

##### Dynamic Semantics

**Invocation Sequence**

1.  **Evaluation**: Evaluate the callee expression to a function pointer or closure.
2.  **Arguments**: Evaluate arguments $a_1 \ldots a_n$ left-to-right.
3.  **Frame**: Allocate a stack frame.
4.  **Binding**: Initialize parameters in the new frame.
5.  **Entry**: Transfer control to the first statement of the body.

**Termination Sequence**

A procedure terminates when it executes `return`, propagates a panic, or reaches the end of its body (if return type is `()`). Termination proceeds as follows:

1.  **Result**: If terminating normally, the return value is constructed in the caller's context.
2.  **Defer**: Execute all `defer` blocks registered in the procedure body in LIFO order (§12.12).
3.  **Drop**: Destroy all local bindings (parameters and variables) in reverse declaration order (§3.6).
4.  **Exit**: Deallocate the stack frame and resume the caller.

**Panic Propagation**

If a panic occurs:
1.  Execution of the current procedure is suspended.
2.  The termination sequence (Defer and Drop steps) is executed.
3.  The panic is re-raised in the caller.

##### Constraints & Legality

**Recursion**

Recursive calls are permitted. Infinite recursion will eventually exhaust the stack, resulting in a runtime panic (implementation limit).

---

### 9.5 Overloading and Signature Matching

##### Definition

**Overloading** allows multiple procedures to share the same name within the same scope, provided their signatures are distinct.

##### Static Semantics

**Overload Set**

The **overload set** for a name $N$ contains all visible procedures named $N$.

**Candidate Selection**

Given a call $N(a_1, \ldots, a_n)$, a procedure $P$ in the overload set is a **candidate** if:
1.  The number of parameters in $P$ equals $n$.
2.  For every argument $a_i$ of type $T_{arg}$, and corresponding parameter $p_i$ of type $T_{param}$, $T_{arg} <: T_{param}$.

**Resolution Algorithm**

1.  **Filter**: Identify all candidates. If the set is empty, emit `E-PROC-3031`.
2.  **Rank**: Determine the **specificity** of each candidate. Candidate $A$ is more specific than $B$ if:
    *   For every parameter $i$, $T_{param, A} <: T_{param, B}$.
    *   And for at least one parameter $j$, $T_{param, B} \not<: T_{param, A}$.
    *   Non-generic procedures are strictly more specific than generic procedures if types match.
3.  **Select**:
    *   If exactly one candidate is strictly more specific than all others, select it.
    *   If multiple candidates are incomparable (neither is more specific), or identical, the call is ambiguous. Emit `E-PROC-3030`.

**Shadowing vs Overloading**

Declarations in inner scopes **shadow** declarations in outer scopes. Overloading only applies to procedures declared in the *same* scope.

##### Constraints & Legality

**Diagnostic Table**

| Code          | Severity | Condition                                          | Detection    | Effect    |
| :------------ | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-PROC-3030` | Error    | Ambiguous overload resolution.                     | Compile-time | Rejection |
| `E-PROC-3031` | Error    | No matching overload found for arguments.          | Compile-time | Rejection |
| `E-PROC-3032` | Error    | Duplicate procedure signature (indistinguishable). | Compile-time | Rejection |

---

## Clause 10: Classes and Polymorphism

### 10.1 Introduction to Classes

##### Definition

A **class** is a named declaration that defines an abstract interface consisting of procedure signatures, associated type declarations, abstract fields, and abstract states. Classes establish structural and behavioral contracts that implementing types MUST satisfy.

**Formal Definition**

A class $Cl$ is defined as a tuple:

$$Cl = (N, G, S, P_{abs}, P_{con}, A_{abs}, A_{con}, F, St)$$

where:

- $N$ is the class name (an identifier).
- $G$ is the (possibly empty) set of generic type parameters.
- $S$ is the (possibly empty) set of superclass bounds.
- $P_{abs}$ is the set of abstract procedure signatures.
- $P_{con}$ is the set of concrete procedure definitions (default implementations).
- $A_{abs}$ is the set of abstract associated type declarations.
- $A_{con}$ is the set of concrete associated type bindings (defaults).
- $F$ is the (possibly empty) set of abstract field declarations.
- $St$ is the (possibly empty) set of abstract state declarations.

A class with $St \neq \emptyset$ is a **modal class**.

The **interface** of class $Cl$ is defined as:

$$\text{interface}(Cl) = P_{abs} \cup P_{con} \cup A_{abs} \cup A_{con} \cup F \cup St$$

A type $T$ **implements** class $Cl$ (written $T <: Cl$) when $T$ provides concrete definitions for all abstract members:

$$T <: Cl \iff \forall p \in P_{abs}.\ T \text{ defines } p\ \land\ \forall a \in A_{abs}.\ T \text{ binds } a\ \land\ \forall f \in F.\ T \text{ has } f\ \land\ \forall s \in St.\ T \text{ has } s$$

##### Static Semantics

Cursive provides three distinct mechanisms for polymorphism:

1.  **Static Polymorphism:** Selected when a generic parameter `T <: Cl` is used. Resolved at compile time via monomorphization.
2.  **Dynamic Polymorphism:** Selected when `$Cl` is used as a type. Resolved at runtime via vtable dispatch.
3.  **Opaque Polymorphism:** Selected when `opaque Cl` is used as a return type. Resolved at compile time via type erasure.

---

### 10.2 Class Declarations

##### Definition

A **class declaration** introduces a named interface that types MAY implement. It defines the contract for implementing types, including required members and optional default implementations.

##### Syntax & Declaration

**Grammar**

```ebnf
class_declaration ::=
    [ visibility ] "class" identifier [ generic_params ]
    [ "<:" superclass_bounds ] "{"
        class_item*
    "}"

superclass_bounds ::= class_bound ( "+" class_bound )*
class_bound       ::= type_path [ generic_args ]

class_item ::=
    abstract_procedure
  | concrete_procedure
  | associated_type
  | abstract_field
  | abstract_state

abstract_procedure ::=
    "procedure" identifier signature [ contract_clause ]

concrete_procedure ::=
    "procedure" identifier signature [ contract_clause ] block

associated_type ::=
    "type" identifier [ "=" type ]

abstract_field ::=
    identifier ":" type

abstract_state ::=
    "@" identifier "{" [ field_list ] "}"

class_alias ::=
    [ visibility ] "type" identifier [ generic_params ]
    "=" class_bound ( "+" class_bound )* ";"
```

**The `Self` Type**

Within a class declaration, the identifier `Self` refers to the implementing type. `Self` MUST be used for the receiver parameter of methods and MAY be used in other type positions.

##### Static Semantics

**Well-Formedness (WF-Class)**

A class declaration is well-formed if and only if:

1.  The identifier $N$ is unique within its namespace.
2.  All procedure signatures are well-formed.
3.  All associated types have unique names within the class.
4.  All abstract fields have unique names within the class.
5.  All abstract states have unique names within the class.
6.  No name conflicts exist among procedures, associated types, fields, and states.
7.  All superclass bounds reference valid class types.
8.  No cyclic superclass dependencies exist.
9.  State references in procedure signatures refer to states declared in the same class.

$$\frac{
  \text{unique}(N) \quad
  \forall p \in P.\ \Gamma, \text{Self} : \text{Type} \vdash p\ \text{wf} \quad
  \forall a \in A.\ \Gamma \vdash a\ \text{wf} \quad
  \forall f \in F.\ \Gamma \vdash f\ \text{wf} \quad
  \forall s \in St.\ \Gamma \vdash s\ \text{wf} \quad
  \neg\text{cyclic}(S) \quad
  \text{names\_disjoint}(P, A, F, St)
}{
  \Gamma \vdash \text{class } N\ [<: S]\ \{P, A, F, St\}\ \text{wf}
} \quad \text{(WF-Class)}$$

**Modal Class Constraint (T-Modal-Class)**

A class containing one or more abstract state declarations ($St \neq \emptyset$) is a **modal class**. Only modal types MAY implement modal classes.

**State Union Return Types (T-State-Union)**

A procedure within a modal class MAY declare a return type as a union of class states. The return type is well-formed if every referenced state is declared in the class.

$$\frac{
  \forall i.\ @S_i \in St(Cl)
}{
  \Gamma \vdash Cl@S_1 \mid Cl@S_2 \mid \ldots \mid Cl@S_n : \text{Type}
} \quad \text{(T-State-Union)}$$

**Class Alias Equivalence (T-Alias-Equiv)**

A class alias defines a constraint equivalent to the conjunction of its bounds.

$$\frac{\text{type } Alias = A + B}{\Gamma \vdash T <: Alias \iff \Gamma \vdash T <: A \land \Gamma \vdash T <: B}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-CLS-2900` | Error    | Duplicate procedure name in class.                | Compile-time | Rejection |
| `E-CLS-2904` | Error    | Duplicate associated type name in class.          | Compile-time | Rejection |
| `E-CLS-2905` | Error    | Name conflict among class members.                | Compile-time | Rejection |
| `E-CLS-2908` | Error    | Cyclic superclass dependency detected.            | Compile-time | Rejection |
| `E-CLS-2909` | Error    | Superclass bound refers to undefined class.       | Compile-time | Rejection |
| `E-CLS-0408` | Error    | Duplicate abstract field name in class.           | Compile-time | Rejection |
| `E-CLS-0409` | Error    | Duplicate abstract state name in class.           | Compile-time | Rejection |
| `E-CLS-0410` | Error    | State reference to undeclared state in signature. | Compile-time | Rejection |
| `E-CLS-0411` | Error    | Duplicate field name within state payload.        | Compile-time | Rejection |

---

### 10.3 Class Implementation

##### Definition

**Class Implementation** is the explicit association of a concrete type with a class contract. An implementation definition MUST satisfy all requirements of the class interface.

##### Syntax & Declaration

**Grammar**

The class implementation clause uses the `<:` operator within type declarations.

```ebnf
implements_clause ::= "<:" class_list
class_list        ::= type_path ("," type_path)*
```

**Override Syntax**

Procedure implementations within the type body follow this grammar:

```ebnf
impl_procedure ::= [ visibility ] [ "override" ] "procedure" identifier signature block
```

##### Static Semantics

**Implementation Well-Formedness (WF-Impl)**

A class implementation `T <: Cl` is well-formed if and only if:

1.  `T` provides a concrete definition for every abstract procedure in `Cl`.
2.  `T` binds every abstract associated type in `Cl`.
3.  `T` declares a field for every abstract field in `Cl` with a matching name and compatible type.
4.  `T` declares a state for every abstract state in `Cl` with a matching name and compatible payload (if `Cl` is modal).
5.  If `Cl` is modal, `T` is a modal type.

$$\frac{
  \Gamma \vdash T\ \text{wf} \quad
  \Gamma \vdash Cl\ \text{wf} \quad
  \forall p \in P_{abs}(Cl).\ T \vdash p\ \text{implemented} \quad
  \forall a \in A_{abs}(Cl).\ T \vdash a\ \text{bound} \quad
  \forall f \in F(Cl).\ T \vdash f\ \text{present} \quad
  \forall s \in St(Cl).\ T \vdash s\ \text{present} \quad
  St(Cl) \neq \emptyset \implies T \text{ is modal}
}{
  \Gamma \vdash T <: Cl\ \text{wf}
} \quad \text{(WF-Impl)}$$

**Field Compatibility (T-Field-Compat)**

A field $f : T_i$ in implementation $T$ satisfies abstract field $f : T_c$ in class $Cl$ if and only if $T_i$ is a subtype of $T_c$.

$$\frac{
  f : T_c \in F(Cl) \quad
  f : T_i \in \text{fields}(T) \quad
  T <: Cl \quad
  T_i <: T_c
}{
  T \vdash f\ \text{present}
} \quad \text{(T-Field-Compat)}$$

**State Compatibility (T-State-Compat)**

A state $@S$ in implementation $T$ satisfies abstract state $@S$ in class $Cl$ if and only if the implementing state contains at least the payload fields required by the abstract state, with compatible types.

$$\frac{
  @S \{ f_1: T_1, \ldots, f_n: T_n \} \in St(Cl) \quad
  @S \{ f_1: T'_1, \ldots, f_n: T'_n, \ldots \} \in \text{states}(T) \quad
  T <: Cl \quad
  \forall i \in 1..n.\ T'_i <: T_i
}{
  T \vdash @S\ \text{present}
} \quad \text{(T-State-Compat)}$$

**Override Semantics**

1.  If implementing an **abstract procedure**, the `override` keyword MUST NOT be used.
2.  If replacing a **concrete procedure** (default implementation), the `override` keyword MUST be used.

**Coherence Rule**

A type `T` MAY implement a class `Cl` at most once.

**Orphan Rule**

For an implementation `T <: Cl`, at least one of `T` or `Cl` MUST be defined in the current assembly.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-CLS-2901` | Error    | `override` used on abstract procedure implementation.   | Compile-time | Rejection |
| `E-CLS-2902` | Error    | Missing `override` on concrete procedure replacement.   | Compile-time | Rejection |
| `E-CLS-2903` | Error    | Type does not implement required procedure from class.  | Compile-time | Rejection |
| `E-CLS-2906` | Error    | Coherence violation: duplicate class implementation.    | Compile-time | Rejection |
| `E-CLS-2907` | Error    | Orphan rule violation: neither type nor class is local. | Compile-time | Rejection |
| `E-CLS-0401` | Error    | Non-modal type implements modal class.                  | Compile-time | Rejection |
| `E-CLS-0402` | Error    | Implementing type missing required field.               | Compile-time | Rejection |
| `E-CLS-0403` | Error    | Implementing modal missing required state.              | Compile-time | Rejection |
| `E-CLS-0404` | Error    | Implementing field has incompatible type.               | Compile-time | Rejection |
| `E-CLS-0405` | Error    | Implementing state missing required payload field.      | Compile-time | Rejection |
| `E-CLS-0406` | Error    | Conflicting field names from multiple classes.          | Compile-time | Rejection |
| `E-CLS-0407` | Error    | Conflicting state names from multiple classes.          | Compile-time | Rejection |

---

### 10.4 Class Constraints in Generics

##### Definition

Class constraints restrict valid generic type arguments to those implementing a specific class.

##### Static Semantics

**Constraint Satisfaction**

A generic instantiation is well-formed only if every constrained parameter `T <: Cl` is instantiated with a type `A` such that `A <: Cl`.

**Method Availability**

Within the scope of a generic parameter `T <: Cl`, methods defined in `Cl` are callable on values of type `T`. Dispatch is static and resolved during monomorphization.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-CLS-2930` | Error    | Type argument does not satisfy class constraint.   | Compile-time | Rejection |
| `E-CLS-2931` | Error    | Unconstrained type parameter used in class method. | Compile-time | Rejection |

---

### 10.5 Dynamic Polymorphism

##### Definition

**Dynamic Polymorphism** allows values of disparate concrete types to be treated uniformly via a common class interface using runtime dispatch.

A **Dynamic Class Type**, denoted `$Cl`, is a concrete, sized type representing a value of some erased type $T$ that implements $Cl$.

##### Syntax & Declaration

**Grammar**

```ebnf
dyn_type ::= "dyn" class_path
```

##### Static Semantics

**Dispatchability**

A class $Cl$ is **dispatchable** if and only if every procedure in $Cl$ (and its superclasses) is either **vtable-eligible** or explicitly excluded via the `[[static_dispatch_only]]` attribute.

A procedure $p$ is **vtable-eligible** if and only if:
1.  It has a receiver parameter.
2.  It has no generic type parameters.
3.  It does not return `Self` by value (except via pointer indirection).
4.  It does not use `Self` in parameter types (except via pointer indirection).

**Formal Definition of Dispatchable**

$$
\text{dispatchable}(Cl) \iff \forall p \in \text{procedures}(Cl).\ \text{vtable\_eligible}(p) \lor \text{has\_static\_dispatch\_attr}(p)
$$

**Dynamic Class Formation (T-Dynamic-Form)**

A value of type $T$ can be coerced to `$Cl` if $T <: Cl$ and $Cl$ is dispatchable.

$$\frac{
  \Gamma \vdash v : T \quad \Gamma \vdash T <: Cl \quad \text{dispatchable}(Cl)
}{
  \Gamma \vdash v : \text{$} Cl
} \quad \text{(T-Dynamic-Form)}$$

##### Dynamic Semantics

**Evaluation**

Evaluation of a `$Cl` creation expression from value `v`:
1.  Construct a pointer to the storage of `v`.
2.  Resolve the static vtable for the pair $(T, Cl)$.
3.  Return the pair `(ptr, vtable)`.

**VTable Dispatch**

A method call `w~>method(args)` on `w: $Cl`:
1.  Extract the vtable pointer `vt` and data pointer `dp` from `w`.
2.  Calculate the offset of `method` in the vtable.
3.  Load the function pointer `fp` from `vt` at the calculated offset.
4.  Invoke `fp(dp, args)`.

##### Memory & Layout

**Representation**

A `$Cl` value is represented as a **dense pointer** consisting of two machine words:

1.  **Data Pointer:** Address of the concrete value (erased type).
2.  **VTable Pointer:** Address of the virtual method table for the concrete type's implementation of the class.

**Size and Alignment**

$$\text{sizeof}(\texttt{$} Cl) = 2 \times \text{sizeof}(\texttt{usize})$$
$$\text{alignof}(\texttt{$} Cl) = \text{alignof}(\texttt{usize})$$

**VTable Layout (Stable ABI)**

The vtable MUST be laid out in the following order:

| Offset | Field       | Type       | Description                                      |
| :----- | :---------- | :--------- | :----------------------------------------------- |
| 0      | `size`      | `usize`    | Size of concrete type in bytes.                  |
| 1      | `align`     | `usize`    | Alignment of concrete type in bytes.             |
| 2      | `drop`      | `*imm fn`  | Destructor function pointer (null if no `Drop`). |
| 3      | `state_map` | `*imm Map` | (Modal classes only) State discriminant mapping. |
| 3+N    | `methods`   | `*imm fn`  | Function pointers in declaration order.          |

For non-modal classes, the `state_map` field is absent, and methods start at offset 3.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------- | :----------- | :-------- |
| `E-CLS-2940` | Error    | Procedure with `[[static_dispatch_only]]` called on dyn. | Compile-time | Rejection |
| `E-CLS-2941` | Error    | Dynamic class type created from non-dispatchable class.  | Compile-time | Rejection |

---

### 10.6 Opaque Polymorphism

##### Definition

**Opaque Polymorphism** allows a procedure to return a concrete type while exposing only a class interface to the caller. The concrete type is erased at the API boundary but preserved for compilation.

##### Syntax & Declaration

**Grammar**

```ebnf
return_type ::= ... | "opaque" class_path
```

##### Static Semantics

**Opaque Return Typing (T-Opaque-Return)**

A procedure is well-typed if its body returns a type $T$ such that $T <: Cl$.

$$\frac{
  \Gamma \vdash \text{body} : T \quad
  \Gamma \vdash T <: Cl \quad
  \text{return\_type}(f) = \text{opaque } Cl
}{
  \Gamma \vdash f : () \to \text{opaque } Cl
} \quad \text{(T-Opaque-Return)}$$

**Opaque Type Projection (T-Opaque-Project)**

Callers treat the return value as an existential type bounded by $Cl$. Only members of $Cl$ are accessible.

**Type Identity**

Two opaque types are equivalent if and only if they originate from the same procedure definition.

$$\frac{
  \Gamma \vdash f : () \to \text{opaque } Cl \quad
  \Gamma \vdash g : () \to \text{opaque } Cl \quad
  f \neq g
}{
  \text{typeof}(f()) \neq \text{typeof}(g())
} \quad \text{(T-Opaque-Distinct)}$$

##### Memory & Layout

**Zero Overhead**

Opaque types MUST have the same runtime representation, size, and alignment as the underlying concrete type. No indirection or vtable is used.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-CLS-2910` | Error    | Accessing member not defined on opaque type's class.  | Compile-time | Rejection |
| `E-CLS-2911` | Error    | Opaque return type does not implement required class. | Compile-time | Rejection |
| `E-CLS-2912` | Error    | Attempting to assign incompatible opaque types.       | Compile-time | Rejection |

---

### 10.7 Foundational Classes

##### Definition

**Foundational classes** are intrinsic interfaces that define core language semantics for destruction, duplication, and iteration. These classes have special interactions with the compiler and runtime.

#### 10.7.1 The Drop Class

##### Definition

The **`Drop` class** defines custom destruction logic. Implementing `Drop` designates a type as requiring deterministic cleanup code execution when its lifetime ends.

##### Syntax & Declaration

```cursive
class Drop {
    /// Executes cleanup logic.
    procedure drop(~!);
}
```

##### Static Semantics

**Mutual Exclusivity**

A type MUST NOT implement both `Drop` and `Copy`. A type that manages resources (requiring `Drop`) cannot be safely duplicated via bitwise copy. Violation of this constraint is ill-formed (`E-TRS-2921`).

**Explicit Call Prohibition**

User code MUST NOT invoke `Drop::drop` directly. Destructors are invoked solely by the implementation during scope exit or cleanup. Explicit calls to `drop` are ill-formed (`E-TRS-2920`). To explicitly destroy a value, user code MUST use `mem::drop` or allow the value to go out of scope.

##### Dynamic Semantics

**Automatic Invocation**

The `drop` procedure is invoked exactly once for a responsible binding when:
1.  The binding goes out of scope.
2.  The binding is overwritten via assignment (destructor runs on the old value).
3.  The binding is a field of a composite type being dropped.
4.  The program unwinds due to a panic.

**Destruction Order**

Destructors execute in **reverse declaration order** (LIFO) within a scope. Fields of a record are dropped in reverse declaration order.

#### 10.7.2 The Copy Class

##### Definition

The **`Copy` class** is a marker interface indicating that a type's values may be duplicated via bitwise copy. Types implementing `Copy` follow value semantics: assignment and parameter passing duplicate the value rather than moving it.

##### Syntax & Declaration

```cursive
class Copy {
    // Marker interface; no members.
}
```

##### Static Semantics

**Field Propagation Requirement**

A type implementing `Copy` MUST ensure that all of its fields also implement `Copy`. Violation of this constraint is ill-formed (`E-TRS-2922`).

**Assignment Semantics**

For a binding `x` of type `T` where `T <: Copy`:
1.  `let y = x` creates a bitwise copy of `x` in `y`.
2.  `x` remains in the **Valid** state (it does not transition to **Moved**).
3.  No destructor is run for the source value.

**Copy-Implies-Clone**

Any type implementing `Copy` MUST also implement `Clone`.

#### 10.7.3 The Clone Class

##### Definition

The **`Clone` class** defines the interface for explicit duplication. Unlike `Copy`, `Clone` may involve arbitrary code execution (e.g., deep copying heap allocations).

##### Syntax & Declaration

```cursive
class Clone {
    /// Returns a duplicate of the receiver.
    procedure clone(~) -> Self;
}
```

##### Static Semantics

**Relationship to Copy**

For types implementing `Copy`, the implementation of `clone` MUST perform an operation semantically equivalent to a bitwise copy.

#### 10.7.4 The Iterator Class

##### Definition

The **`Iterator` class** defines the protocol for sequential iteration. Types implementing `Iterator` may be used as the source expression in `loop ... in` constructs.

##### Syntax & Declaration

```cursive
class Iterator {
    /// The type of elements yielded by the iterator.
    type Item;

    /// Advances the iterator and returns the next value.
    /// Returns `None` when iteration is complete.
    procedure next(~!) -> Item | None;
}
```

##### Dynamic Semantics

**Iteration Protocol**

The `next` procedure advances the internal state of the iterator and returns:
-   The next element (wrapped in the union type), if available.
-   `None`, if the sequence is exhausted.

Once `None` is returned, subsequent calls to `next` MAY return `None` or panic, but MUST NOT return a value.

#### 10.7.5 Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-TRS-2920` | Error    | Explicit call to `Drop::drop` by user code.      | Compile-time | Rejection |
| `E-TRS-2921` | Error    | Type implements both `Drop` and `Copy`.          | Compile-time | Rejection |
| `E-TRS-2922` | Error    | `Copy` type contains fields that are not `Copy`. | Compile-time | Rejection |
| `E-TRS-2923` | Error    | `Copy` type does not implement `Clone`.          | Compile-time | Rejection |

---

## Clause 11: Contracts and Verification

### 11.1 Contract Fundamentals

##### Definition

A **Contract** is a formal specification attached to a procedure declaration consisting of logical predicates that constrain the state of the program at specific execution points. Contracts govern the interface between a caller and a callee.

**Formal Definition**

A contract $C$ is a pair of predicate sets:

$$C = (P_{pre}, P_{post})$$

where:
*   $P_{pre}$ is the set of **preconditions** (caller obligations).
*   $P_{post}$ is the set of **postconditions** (callee guarantees).

A **Predicate** is a pure boolean expression interpreted within a specific evaluation context.

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

**Statement Placement**

A contract clause MUST appear immediately following the procedure signature (parameter list and return type) and immediately preceding the procedure body or terminating semicolon.

##### Static Semantics

**Well-Formedness (WF-Contract)**

A contract clause is well-formed if and only if both the precondition and postcondition expressions are well-formed predicates of type `bool` that satisfy purity constraints:

$$
\frac{
    \Gamma_{pre} \vdash P_{pre} : \texttt{bool} \quad \text{Pure}(P_{pre}) \\
    \Gamma_{post} \vdash P_{post} : \texttt{bool} \quad \text{Pure}(P_{post})
}{
    \Gamma \vdash \texttt{|=}\ P_{pre} \Rightarrow P_{post} : \text{WF}
}
\tag{WF-Contract}
$$

**Purity Constraint**

An expression $e$ satisfies $\text{Pure}(e)$ if and only if:
1.  $e$ contains no calls to procedures accepting capability parameters.
2.  $e$ contains no assignment expressions or mutating method calls.
3.  $e$ contains no `spawn`, `dispatch`, or `yield` expressions.

**Evaluation Contexts**

Predicates are typed and evaluated in specific contexts:

1.  **Precondition Context ($\Gamma_{pre}$)**: Includes all procedure parameters and the receiver (`self`). For mutable parameters (`unique`, `shared`, `var`), the context refers to the value at procedure entry. The `@result` and `@entry` intrinsics are not available.
2.  **Postcondition Context ($\Gamma_{post}$)**: Includes all procedure parameters and the receiver. For mutable parameters, the context refers to the value at procedure exit. The `@result` and `@entry` intrinsics are available.
  
| Location in Contract          | State Referenced |
| :---------------------------- | :--------------- |
| Left of `=>`                  | Entry state      |
| Right of `=>` (bare)          | Post-state       |
| Right of `=>` with `@entry()` | Entry state      |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CON-2802` | Error    | Impure expression used in contract predicate. | Compile-time | Rejection |
| `E-CON-2806` | Error    | `@result` used outside postcondition context. | Compile-time | Rejection |
| `E-CON-2807` | Error    | Predicate expression is not of type `bool`.   | Compile-time | Rejection |

---

### 11.2 Preconditions and Postconditions

##### Definition

A **Precondition** is a predicate that the caller MUST satisfy before invoking the procedure. A **Postcondition** is a predicate that the callee MUST satisfy before returning control to the caller.

##### Static Semantics

**Precondition Verification (Pre-Satisfied)**

For every call site $S$ invoking procedure $f$, the implementation MUST verify that the precondition $P_{pre}$ holds given the caller's context.

$$
\frac{
    \Gamma \vdash f : (T_1, \ldots, T_n) \to R \quad
    \text{precondition}(f) = P_{pre} \quad
    \exists L.\ F(P_{pre}, L) \land L \text{ dominates } S
}{
    \Gamma \vdash f(a_1, \ldots, a_n) @ S : \text{valid}
}
\tag{Pre-Satisfied}
$$

If a Verification Fact $F(P_{pre}, L)$ (see §11.5) cannot be established statically at a dominating location $L$, and the call site is not within a `[[dynamic]]` scope (§7.2.9), the program is ill-formed.

**Postcondition Verification (Post-Valid)**

For every procedure definition $f$, the implementation MUST verify that the postcondition $P_{post}$ holds at every return point $r$.

$$
\frac{
    \text{postcondition}(f) = P_{post} \quad
    \forall r \in \text{ReturnPoints}(f).\ \Gamma_r \vdash P_{post} : \text{satisfied}
}{
    f : \text{postcondition-valid}
}
\tag{Post-Valid}
$$

**Intrinsics**

The following intrinsics are defined only within the postcondition context:

1.  **`@result`**:
    *   **Type**: The return type of the procedure.
    *   **Value**: The value being returned.
    *   **Constraint**: If the return type is `()`, `@result` is `()`.

2.  **`@entry(expr)`**:
    *   **Type**: The type of `expr`.
    *   **Value**: The result of evaluating `expr` in the **entry** state (before procedure body execution).
    *   **Constraint**: The type of `expr` MUST implement `Copy` or `Clone`.

##### Dynamic Semantics

**@entry Evaluation**

When `@entry(e)` is present in a postcondition:
1.  The expression $e$ is evaluated immediately after parameter binding and before the procedure body begins.
2.  If the type of $e$ implements `Clone` but not `Copy`, the `clone()` method is invoked to capture the value.
3.  The captured value is stored in the stack frame for use during postcondition verification.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-CON-2801` | Error    | Contract not statically provable outside `[[dynamic]]`. | Compile-time | Rejection |
| `E-CON-2805` | Error    | `@entry()` result type does not implement Copy/Clone.   | Compile-time | Rejection |

---

### 11.3 Invariants

##### Definition

An **Invariant** is a predicate that MUST hold true throughout a specific scope or lifecycle.

#### 11.3.1 Type Invariants

##### Definition

A **Type Invariant** is a `where` clause attached to a `record`, `enum`, or `modal` definition. It constrains the valid state of all instances of that type.

##### Syntax & Declaration

**Grammar**

```ebnf
type_invariant ::= "where" "{" predicate_expr "}"
```

This production appears as the final element of a type declaration.

##### Static Semantics

**Enforcement Points**

The implementation MUST verify the type invariant at the following program points:

1.  **Post-Construction**: Immediately after any constructor or literal initialization of the type.
2.  **Pre-Call (Public)**: Immediately before any `public` procedure defined on the type is invoked (with the instance as a receiver).
3.  **Post-Call (Mutator)**: Immediately before any procedure taking a `unique` receiver returns.
4.  **Private-to-Public Boundary**: Immediately before a `private` or `internal` procedure returns control to a context outside the defining module.

**Private Exemption**

Procedures with `private` or `internal` visibility are exempt from the Pre-Call enforcement check. This permits internal procedures to operate on instances that are temporarily in an invalid state, provided the invariant is restored before returning to public code.

**Public Field Constraint**

If a type declares a type invariant, it MUST NOT declare any `public` fields that are mutable (`var` or accessible via `unique` reference).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                     | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------ | :----------- | :-------- |
| `E-CON-2820` | Error    | Type invariant violated at construction.                      | Compile-time | Rejection |
| `E-CON-2821` | Error    | Type invariant violated at public procedure entry.            | Compile-time | Rejection |
| `E-CON-2822` | Error    | Type invariant violated at mutator return.                    | Compile-time | Rejection |
| `E-CON-2823` | Error    | Type invariant violated at private-to-public return boundary. | Compile-time | Rejection |
| `E-CON-2824` | Error    | Public mutable field declared on type with invariant.         | Compile-time | Rejection |

---

#### 11.3.2 Loop Invariants

##### Definition

A **Loop Invariant** is a predicate attached to a loop that MUST hold before the first iteration, at the start of every subsequent iteration, and upon loop termination.

##### Syntax & Declaration

**Grammar**

```ebnf
loop_invariant ::= "where" "{" predicate_expr "}"
```

This production appears in `loop` expressions (§12.6).

##### Static Semantics

**Verification Obligations**

Given a loop invariant $Inv$:

1.  **Initialization**: $\Gamma_{pre-loop} \vdash Inv$
2.  **Maintenance**: $\Gamma_{start-body} \vdash Inv \implies \Gamma_{end-body} \vdash Inv$
3.  **Termination**: $\Gamma_{exit} \vdash Inv$

Upon successful verification, the implementation MUST generate a Verification Fact $F(Inv, L_{exit})$ available to code following the loop.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-CON-2830` | Error    | Loop invariant not established at initialization. | Compile-time | Rejection |
| `E-CON-2831` | Error    | Loop invariant not maintained across iteration.   | Compile-time | Rejection |

---

### 11.4 Contract Verification

##### Definition

Contract verification is the process of proving that all contract predicates hold at their required enforcement points. Verification encompasses both static proof and dynamic checking.

##### Static Semantics

**Verification Mode Selection**

The verification mode for a predicate $P$ is determined by the presence of the `[[dynamic]]` attribute in the scope enclosing the enforcement point.

**Default: Static Verification (Contract-Static-OK)**

If the enforcement point is NOT within a `[[dynamic]]` scope:

$$
\frac{
    \text{StaticProof}(\Gamma, P)
}{
    P : \text{verified}
}
\tag{Contract-Static-OK}
$$

$$
\frac{
    \neg\text{StaticProof}(\Gamma, P)
}{
    \text{Error: E-CON-2801}
}
\tag{Contract-Static-Fail}
$$

**Opt-In: Dynamic Verification (Contract-Dynamic-Check)**

If the enforcement point IS within a `[[dynamic]]` scope:

$$
\frac{
    \text{StaticProof}(\Gamma, P)
}{
    P : \text{verified (elided)}
}
\tag{Contract-Dynamic-Elide}
$$

$$
\frac{
    \neg\text{StaticProof}(\Gamma, P)
}{
    \text{EmitRuntimeCheck}(P)
}
\tag{Contract-Dynamic-Check}
$$

**Mandatory Proof Techniques**

A conforming implementation MUST support the following static proof techniques:
1.  **Constant Propagation**: Evaluation of expressions involving only literals and `comptime` constants.
2.  **Linear Integer Arithmetic**: Proving inequalities ($<, \le, >, \ge, =, \ne$) over integer variables where relations are linear.
3.  **Boolean Algebra**: Simplification of logical AND/OR/NOT expressions.
4.  **Control Flow Dominance**: Utilizing predicates established by `if` conditions and `match` arms that strictly dominate the enforcement point.
5.  **Type Constraints**: Utilizing bounds inherent to types (e.g., `u8` value is $\in [0, 255]$).

Support for techniques beyond this set is Implementation-Defined Behavior (IDB).

##### Dynamic Semantics

**Runtime Check Failure**

If a runtime check for predicate $P$ evaluates to `false`:
1.  The implementation MUST initiate a panic with code `P-CON-2850`.
2.  Panic processing proceeds per Clause 3 (Deterministic Destruction).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                               | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------ | :----------- | :-------- |
| `E-CON-2801` | Error    | Contract not statically provable outside `[[dynamic]]`. | Compile-time | Rejection |
| `P-CON-2850` | Panic    | Runtime contract check failed.                          | Runtime      | Panic     |

---

### 11.5 Verification Facts

##### Definition

A **Verification Fact** is a tuple $F(P, L)$ recording that predicate $P$ is statically guaranteed to hold at program location $L$.

##### Static Semantics

**Fact Generation**

Verification Facts are generated by the following constructs:

1.  **Conditionals**:
    *   `if cond { ... }`: Generates $F(cond, L_{entry})$ for the `then` block.
    *   `else { ... }`: Generates $F(!cond, L_{entry})$ for the `else` block.
2.  **Pattern Matching**:
    *   `match x { pat => ... }`: Generates $F(x \text{ matches } pat, L_{entry})$ for the arm.
3.  **Runtime Checks**:
    *   Any runtime check for predicate $P$ at location $L$ generates $F(P, L_{next})$, where $L_{next}$ is the instruction immediately following the check (asserting survival implies success).
4.  **Loop Invariants**:
    *   A verified loop invariant $Inv$ generates $F(Inv, L_{exit})$ at the loop exit point.

**Dominance Utilization**

A requirement for predicate $Q$ at location $S$ is satisfied if there exists a fact $F(P, L)$ such that:
1.  $L$ strictly dominates $S$ in the control flow graph.
2.  $P \implies Q$ is provable using the Mandatory Proof Techniques (§11.4).

##### Constraints & Legality

Verification Facts are ephemeral compiler artifacts and have no runtime representation. They MUST NOT be stored, passed, or returned.

---

### 11.6 Liskov Substitution Principle

##### Definition

Behavioral subtyping requires that a type implementing a class MUST satisfy the behavioral contract of that class.

##### Static Semantics

**Precondition Weakening**

For every procedure $m$ in class $Cl$ implemented by type $T$:
$$ P_{pre}(Cl.m) \implies P_{pre}(T.m) $$
The implementation MUST accept *at least* the inputs accepted by the class contract.

**Postcondition Strengthening**

For every procedure $m$ in class $Cl$ implemented by type $T$:
$$ P_{post}(T.m) \implies P_{post}(Cl.m) $$
The implementation MUST guarantee *at least* the properties guaranteed by the class contract.

**Verification Strategy**

These implications MUST be verified statically during the semantic analysis of the class implementation (`<:` declaration). Failure to prove these implications constitutes an ill-formed program.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-CON-2803` | Error    | Implementation strengthens class precondition. | Compile-time | Rejection |
| `E-CON-2804` | Error    | Implementation weakens class postcondition.    | Compile-time | Rejection |

---

### 11.7 The Standard Verifier Profile

##### Definition

To ensure portable compilation and ergonomic safety, all conforming implementations MUST support a standardized logic profile for static verification. This profile combines linear arithmetic, interval analysis, and symbolic equality.

##### Static Semantics

**Mandatory Logic Capabilities**

The verifier MUST statically prove predicates that are decidable using the combination of the following theories. Failure to prove a predicate that is decidable within this profile constitutes a non-conforming implementation (IFNDR):

1.  **Linear Integer Arithmetic (Presburger):**
    *   Addition, subtraction, and comparison ($<, \le, =, \ne, \ge, >$) of integer variables.
    *   Multiplication and division by **compile-time constants**.
    *   *Example:* Prove `i * 2 + 1 < len` where `i` is bounded.

2.  **Type-Derived Interval Analysis:**
    *   The verifier MUST automatically introduce facts based on the inherent bounds of primitive types.
    *   *Fact Generation:* For a binding `x: u8`, the fact $0 \le x \le 255$ is implicitly available.
    *   *Widening:* The verifier MUST prove safety when a narrower type is indexed into a wider type (e.g., `u8` index into `[T; 500]`).

3.  **Symbolic Distinctness (Enums):**
    *   The verifier MUST treat distinct Enum variants as disjoint symbolic constants.
    *   *Example:* If `status == Status::Ready`, then `status != Status::Busy` is statically proven.

4.  **Congruence Closure (Pure Functions):**
    *   For any procedure `f` marked `[[pure]]` (or implicitly pure/comptime), if the verifier can prove $x = y$, it MUST conclude $f(x) = f(y)$.
    *   This enables refactoring verification logic into helper functions without losing proof context.

5.  **Tuple and Struct Projection:**
    *   The verifier MUST track relationships between composite types and their fields.
    *   *Example:* If `x = (a, b)` and `x.0 < 10`, the fact `a < 10` is available.

**Contextual Facts**

The verifier MUST utilize facts established by:
1.  **Control Flow Dominance:** `if`, `else`, `match` arms, and `loop` invariants.
2.  **Constant Propagation:** Values of `const` bindings and `comptime` expressions.
3.  **Data Flow:** Single Static Assignment (SSA) form or equivalent tracking of immutable values through local variables.

**Exclusions (Implementation-Defined Extensions)**

Unless explicitly enabled via feature flags, the verifier is NOT required to support:
1.  **Non-Linear Arithmetic:** Multiplication/division of two dynamic variables (e.g., `x * y`).
2.  **Bitwise Constraints:** Proving logic based on specific bit-patterns (e.g., `(x & 0xF0) < 16`).
3.  **Heap Shape Analysis:** Proving properties about the graph structure of the heap (e.g., "this linked list is acyclic").
4.  **Floating Point Logic:** All floating point comparisons are treated as runtime checks due to NaN/rounding complexity.

**Fallback**

Predicates that cannot be proven using this profile MUST be rejected with `E-CON-2801`, requiring the user to either:
1.  Simplify the logic to fit the Standard Profile.
2.  Annotate the scope with `[[dynamic]]` to opt-in to runtime checking.

---

## Clause 12: Expressions and Statements

### 12.1 Foundational Semantics

##### Definition

An **expression** is a syntactic construct that evaluates to a value and has a static type. A **statement** is a syntactic construct executed for side effects that does not produce a value within its enclosing block context.

**Value Categories**

Every expression is classified into exactly one **value category**:

1.  **Place Expression**: An expression that denotes a specific memory location. Place expressions permit address-taking (`&`) and assignment.
2.  **Value Expression**: An expression that produces a temporary value not associated with a persistent memory location.

**Formal Classification**

Let $\text{Cat}(e)$ denote the value category of expression $e$.

$$\text{Cat}(e) = \begin{cases}
\text{Place} & \text{if } e \text{ is an identifier bound by } \texttt{let}, \texttt{var}, \text{or parameter} \\
\text{Place} & \text{if } e = (*e') \text{ and } \text{Cat}(e') \in \{\text{Place}, \text{Value}\} \\
\text{Place} & \text{if } e = e'.f \text{ or } e = e'[i] \text{ and } \text{Cat}(e') = \text{Place} \\
\text{Value} & \text{otherwise}
\end{cases}$$

##### Static Semantics

**Evaluation Order**

Unless explicitly specified otherwise by a sub-clause, the operands of an operator or expression MUST be evaluated in strict left-to-right order.

**Expression Typing**

The judgment $\Gamma \vdash e : T$ asserts that expression $e$ evaluates to a value of type $T$ under context $\Gamma$.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2501` | Error    | Type mismatch: expected type differs from actual type.    | Compile-time | Rejection |
| `E-EXP-2502` | Error    | Value expression used where place expression is required. | Compile-time | Rejection |

---

### 12.2 Pattern Matching

##### Definition

**Pattern matching** is a mechanism for destructing values and selecting control flow based on value structure. A **pattern** describes the shape of a value and optionally binds constituent parts to identifiers.

**Refutability**

A pattern is **irrefutable** if it matches every possible value of its expected type. A pattern is **refutable** if there exists at least one value of its expected type that it does not match.

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

tuple_pattern       ::= "(" tuple_pattern_elements? ")"
tuple_pattern_elements ::= pattern ";" | pattern ("," pattern)+

record_pattern      ::= type_path "{" [ field_pattern ( "," field_pattern )* ] "}"

field_pattern       ::= identifier [ ":" pattern ]

enum_pattern        ::= type_path "::" identifier [ enum_payload_pattern ]

enum_payload_pattern ::= "(" tuple_pattern_elements? ")"
                       | "{" [ field_pattern ( "," field_pattern )* ] "}"

modal_pattern       ::= "@" identifier [ "{" [ field_pattern ( "," field_pattern )* ","? ] "}" ]

range_pattern       ::= pattern ( ".." | "..=" ) pattern
```

##### Static Semantics

**Pattern Typing and Binding**

The judgment $\Gamma \vdash p : T \Rightarrow \Delta$ asserts that pattern $p$ is valid for type $T$ and introduces the set of bindings $\Delta$.

**(T-Pat-Wildcard)**
$$\frac{}{\Gamma \vdash \_ : T \Rightarrow \emptyset}$$

**(T-Pat-Ident)**
$$\frac{x \notin \text{dom}(\Gamma)}{\Gamma \vdash x : T \Rightarrow \{x : T\}}$$

**(T-Pat-Literal)**
$$\frac{\Gamma \vdash \text{lit} : T}{\Gamma \vdash \text{lit} : T \Rightarrow \emptyset}$$

**(T-Pat-Tuple)**
$$\frac{\Gamma \vdash p_i : T_i \Rightarrow \Delta_i \quad \forall i \neq j, \text{dom}(\Delta_i) \cap \text{dom}(\Delta_j) = \emptyset}{\Gamma \vdash (p_1, \ldots, p_n) : (T_1, \ldots, T_n) \Rightarrow \bigcup \Delta_i}$$

**(T-Pat-Enum)**
$$\frac{E = \texttt{enum} \{ \ldots, V(T_1, \ldots, T_n), \ldots \} \quad \Gamma \vdash p_i : T_i \Rightarrow \Delta_i}{\Gamma \vdash E::V(p_1, \ldots, p_n) : E \Rightarrow \bigcup \Delta_i}$$

**(T-Pat-Range)**
$$\frac{\Gamma \vdash L : T \quad \Gamma \vdash H : T \quad T \in \mathcal{T}_{ordered} \quad \text{IsConst}(L) \quad \text{IsConst}(H)}{\Gamma \vdash L\texttt{..}H : T \Rightarrow \emptyset}$$

**Refutability Classification**

| Pattern Form     | Classification | Condition                                        |
| :--------------- | :------------- | :----------------------------------------------- |
| Wildcard (`_`)   | Irrefutable    | Always                                           |
| Identifier (`x`) | Irrefutable    | Always                                           |
| Literal          | Refutable      | Always                                           |
| Range            | Refutable      | Always                                           |
| Tuple            | Dependent      | Irrefutable iff all sub-patterns are irrefutable |
| Record           | Dependent      | Irrefutable iff all sub-patterns are irrefutable |
| Enum Variant     | Refutable      | If enum has >1 variant                           |
| Modal State      | Refutable      | If modal has >1 state                            |

**Exhaustiveness**

A pattern match MUST be exhaustive. The set of patterns $P = \{p_1, \ldots, p_n\}$ is exhaustive for type $T$ if every possible value $v$ of type $T$ matches at least one $p_i \in P$.

**Binding Permission Derivation**

When binding an identifier `x` to a value derived from scrutinee `s`:

1.  If `s` has `unique` permission, `x` assumes ownership (move semantics).
2.  If `s` has `shared` permission, `x` has `shared` permission.
3.  If `s` has `const` permission, `x` has `const` permission.

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

### 12.3 Operator Precedence and Associativity

##### Definition

The grammar defines operator precedence implicitly through rule hierarchy. The table below provides the normative reference for precedence and associativity.

##### Static Semantics

| Precedence | Operators                                               | Associativity            |
| :--------- | :------------------------------------------------------ | :----------------------- |
| 1 (High)   | `.` `[` `]` `(` `)` `~>` `?`                            | Left                     |
| 2          | `::`                                                    | Left                     |
| 3          | `!` `-` (unary) `&` `*` (deref) `^` `move` `widen` `as` | Right                    |
| 4          | `**`                                                    | Right                    |
| 5          | `*` `/` `%`                                             | Left                     |
| 6          | `+` `-` (binary)                                        | Left                     |
| 7          | `<<` `>>`                                               | Left                     |
| 8          | `&` (bitwise)                                           | Left                     |
| 9          | `^` (bitwise)                                           | Left                     |
| 10         | `                                                       | ` (bitwise)              | Left  |
| 11         | `==` `!=` `<` `>` `<=` `>=`                             | Left                     |
| 12         | `&&`                                                    | Left                     |
| 13         | `                                                       |                          | `     | Left |
| 14 (Low)   | `=` `+=` `-=` `*=` `/=` `%=` `&=` `                     | =` `^=` `<<=` `>>=` `=>` | Right |

---

### 12.4 Primary Expressions and Operators

#### 12.4.1 Primary Expressions

##### Definition

Primary expressions are the atomic units of evaluation: literals, identifiers, and parenthesized expressions.

##### Syntax & Declaration

```ebnf
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
```

##### Static Semantics

**Literals**

A literal expression has the type determined by its lexical form (§2.8) and type inference context (§5.1). String literals have type `string@View`. Literals are **Value Expressions**.

**Identifiers**

An identifier expression evaluates to the value of the binding $x$ in context $\Gamma$.

**(T-Ident)**
$$\frac{(x : T) \in \Gamma}{\Gamma \vdash x : T}$$

If the identifier resolves to a `let` or `var` binding, it is a **Place Expression**. If it resolves to a constant or function definition, it is a **Value Expression**.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-EXP-2511` | Error    | Identifier resolves to type or module, not value. | Compile-time | Rejection |

#### 12.4.2 Member Access

##### Definition

Member access expressions retrieve components of composite types via named fields or positional indices.

##### Syntax & Declaration

```ebnf
field_access ::= postfix_expr "." identifier
tuple_access ::= postfix_expr "." integer_literal
```

##### Static Semantics

**(T-Field)**
$$\frac{\Gamma \vdash e : R \quad R = \texttt{record} \{ \ldots, f : T, \ldots \} \quad f \text{ visible}}{\Gamma \vdash e.f : T}$$

**(T-Tuple)**
$$\frac{\Gamma \vdash e : (T_0, \ldots, T_n) \quad 0 \leq i < n}{\Gamma \vdash e.i : T_i}$$

Member access preserves the value category of the operand. If $e$ is a **Place Expression**, then $e.f$ and $e.i$ are **Place Expressions**.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-EXP-2521` | Error    | Field access on non-record type.               | Compile-time | Rejection |
| `E-EXP-2522` | Error    | Field does not exist in record type.           | Compile-time | Rejection |
| `E-EXP-2523` | Error    | Field is not visible in current scope.         | Compile-time | Rejection |
| `E-EXP-2524` | Error    | Tuple access on non-tuple type.                | Compile-time | Rejection |
| `E-EXP-2525` | Error    | Tuple index out of bounds.                     | Compile-time | Rejection |
| `E-EXP-2526` | Error    | Tuple index is not a constant integer literal. | Compile-time | Rejection |

#### 12.4.3 Indexing

##### Definition

Indexing expressions access elements of arrays or slices using a computed index.

##### Syntax & Declaration

```ebnf
index_expr ::= postfix_expr "[" expression "]"
```

##### Static Semantics

**(T-Index)**
$$\frac{\Gamma \vdash e_1 : T_{seq} \quad \Gamma \vdash e_2 : \texttt{usize} \quad T_{seq} \in \{[T; N], [T]\}}{\Gamma \vdash e_1[e_2] : T}$$

Indexing preserves the value category of the operand.

##### Dynamic Semantics

**Bounds Checking**

The implementation MUST verify that $0 \le e_2 < \text{len}(e_1)$ at runtime. If the check fails, the program MUST panic.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-EXP-2527` | Error    | Indexing applied to non-indexable type.  | Compile-time | Rejection |
| `E-EXP-2528` | Error    | Index expression is not of type `usize`. | Compile-time | Rejection |
| `P-EXP-2530` | Panic    | Index out of bounds at runtime.          | Runtime      | Panic     |

#### 12.4.4 Calls and Method Dispatch

##### Definition

Call expressions invoke procedures, closures, or methods.

##### Syntax & Declaration

```ebnf
call_expr ::= postfix_expr "(" [ argument_list ] ")"
method_call ::= postfix_expr "~>" identifier "(" [ argument_list ] ")"
static_call ::= type_path "::" identifier "(" [ argument_list ] ")"
```

##### Static Semantics

**(T-Call)**
$$\frac{\Gamma \vdash f : (P_1, \ldots, P_n) \to R \quad \forall i.\, \Gamma \vdash a_i \Leftarrow P_i}{\Gamma \vdash f(a_1, \ldots, a_n) : R}$$

**(T-Method-Instance)**
$$\frac{\Gamma \vdash r : T \quad m \in \text{Methods}(T) \cup \text{Methods}(\text{Forms}(T)) \quad \text{Compatible}(r, m)}{\Gamma \vdash r\text{\textasciitilde>}m(\ldots) : \text{Result}(m)}$$

**Argument Compatibility**

An argument $a_i$ is compatible with parameter $P_i$ if:
1. $\Gamma \vdash \text{Type}(a_i) <: \text{Type}(P_i)$.
2. If $P_i$ is a `move` parameter, $a_i$ MUST be a `move` expression.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2531` | Error    | Callee expression is not of callable type.            | Compile-time | Rejection |
| `E-EXP-2532` | Error    | Argument count mismatch.                              | Compile-time | Rejection |
| `E-EXP-2533` | Error    | Argument type incompatible with parameter type.       | Compile-time | Rejection |
| `E-EXP-2534` | Error    | `move` argument required but not provided.            | Compile-time | Rejection |
| `E-EXP-2535` | Error    | `move` argument provided but parameter is not `move`. | Compile-time | Rejection |
| `E-EXP-2536` | Error    | Method not found for receiver type.                   | Compile-time | Rejection |
| `E-EXP-2537` | Error    | Method call using `.` instead of `~>`.                | Compile-time | Rejection |

#### 12.4.5 Unary Expressions

##### Definition

Unary operators perform operations on a single operand.

##### Syntax & Declaration

```ebnf
unary_expr ::= unary_operator unary_expr | postfix_expr
unary_operator ::= "!" | "-" | "&" | "*" | "^" | "move" | "widen"
```

##### Static Semantics

**(T-Ref)** Address-Of
$$\frac{\Gamma \vdash e : T \quad \text{Cat}(e) = \text{Place}}{\Gamma \vdash \&e : \text{Ptr}\langle T \rangle@\text{Valid}}$$

**(T-Deref)** Dereference
$$\frac{\Gamma \vdash e : \text{Ptr}\langle T \rangle@\text{Valid}}{\Gamma \vdash *e : T}$$
$\text{Cat}(*e) = \text{Place}$.

**(T-Neg)** Numeric Negation
$$\frac{\Gamma \vdash e : T \quad T \in \mathcal{T}_{signed} \cup \mathcal{T}_{float}}{\Gamma \vdash -e : T}$$

**(T-Not)** Logical Negation
$$\frac{\Gamma \vdash e : \text{bool}}{\Gamma \vdash !e : \text{bool}}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-EXP-2541` | Error    | Logical `!` applied to non-bool type.           | Compile-time | Rejection |
| `E-EXP-2542` | Error    | Numeric negation of unsigned integer.           | Compile-time | Rejection |
| `E-EXP-2543` | Error    | Address-of `&` applied to value expression.     | Compile-time | Rejection |
| `E-EXP-2544` | Error    | Dereference of pointer in `@Null` state.        | Compile-time | Rejection |
| `E-EXP-2545` | Error    | Raw pointer dereference outside `unsafe` block. | Compile-time | Rejection |

#### 12.4.6 Binary Expressions

##### Definition

Binary expressions combine two operands using arithmetic, logical, or comparison operators.

##### Syntax & Declaration

```ebnf
binary_expr ::= expression binary_op expression
binary_op ::= "+" | "-" | "*" | "/" | "%" | "**" | 
              "&" | "|" | "^" | "<<" | ">>" | 
              "==" | "!=" | "<" | "<=" | ">" | ">=" | 
              "&&" | "||"
```

##### Static Semantics

**Arithmetic and Bitwise**

Operands MUST be of the same type. The result type is the same as the operand type.

**(T-Arith)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T \quad T \in \mathcal{T}_{numeric}}{\Gamma \vdash e_1 \oplus e_2 : T}$$

**Shift Operations**

The left operand MUST be an integer type $I$. The right operand MUST be `u32`. The result type is $I$.

**(T-Shift)**
$$\frac{\Gamma \vdash e_1 : I \quad \Gamma \vdash e_2 : \text{u32}}{\Gamma \vdash e_1 \ll e_2 : I}$$

**Comparison**

Operands MUST be of the same type. The result type is `bool`.

**(T-Cmp)**
$$\frac{\Gamma \vdash e_1 : T \quad \Gamma \vdash e_2 : T \quad T \text{ implements } Eq/Ord}{\Gamma \vdash e_1 \text{ op } e_2 : \text{bool}}$$

##### Dynamic Semantics

**Short-Circuit Evaluation**

1.  `e1 && e2`: If $e_1$ evaluates to `false`, $e_2$ is NOT evaluated.
2.  `e1 || e2`: If $e_1$ evaluates to `true`, $e_2$ is NOT evaluated.

**Overflow and Division**

1.  **Integer Overflow**: In Checked Mode, overflow triggers a panic (`P-EXP-2560`). In Release Mode, behavior is IDB.
2.  **Division by Zero**: Triggers a panic (`P-EXP-2561`) for both integer and floating-point types.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-EXP-2551` | Error    | Arithmetic operator applied to non-numeric types. | Compile-time | Rejection |
| `E-EXP-2552` | Error    | Mismatched operand types for binary operator.     | Compile-time | Rejection |
| `E-EXP-2556` | Error    | Shift amount is not of type `u32`.                | Compile-time | Rejection |
| `P-EXP-2560` | Panic    | Integer overflow in checked mode.                 | Runtime      | Panic     |
| `P-EXP-2561` | Panic    | Division by zero.                                 | Runtime      | Panic     |

#### 12.4.7 Cast Expressions

##### Definition

A **cast expression** converts a value from one type to another using the `as` operator.

##### Syntax & Declaration

```ebnf
cast_expr ::= unary_expr "as" type
```

##### Static Semantics

A cast is valid if and only if the pair (Source Type, Target Type) appears in the **Permitted Casts Table**. All other casts are ill-formed.

**Permitted Casts Table**

| Source              | Target              | Semantics                                | Condition                      |
| :------------------ | :------------------ | :--------------------------------------- | :----------------------------- |
| `T`                 | `T`                 | Identity                                 | Always valid                   |
| Integer             | Integer             | Widen (sign-extend) or Narrow (truncate) | Always valid                   |
| Float               | Float               | Widen (lossless) or Narrow (round)       | Always valid                   |
| Integer             | Float               | Round to nearest                         | Always valid                   |
| Float               | Integer             | Truncate toward zero                     | `P-EXP-2580` on overflow/NaN   |
| `bool`              | Integer             | false $\to$ 0, true $\to$ 1              | Always valid                   |
| `char`              | `u32`               | Zero-extend                              | Always valid                   |
| `u32`               | `char`              | Value check                              | `P-EXP-2581` if invalid scalar |
| `Ptr<T>@Valid`      | `*mut T` / `*imm T` | Address extraction                       | Always valid                   |
| `*mut T` / `*imm T` | `usize`             | Address to integer                       | **Unsafe Block Only**          |
| `usize`             | `*mut T` / `*imm T` | Integer to address                       | **Unsafe Block Only**          |
| Enum                | Integer             | Discriminant extraction                  | Enum has `[[layout(Int)]]`     |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-EXP-2571` | Error    | Cast between incompatible types.             | Compile-time | Rejection |
| `E-EXP-2572` | Error    | Pointer-integer cast outside `unsafe` block. | Compile-time | Rejection |
| `P-EXP-2580` | Panic    | Float-to-integer cast overflow or NaN.       | Runtime      | Panic     |

---

### 12.5 Closure Expressions

##### Definition

A **closure expression** creates an anonymous function value that may capture variables from its enclosing scope.

##### Syntax & Declaration

```ebnf
closure_expr ::= "|" [ closure_param_list ] "|" [ "->" type ] closure_body
closure_param_list ::= closure_param ( "," closure_param )* ","?
closure_param ::= [ "move" ] identifier [ ":" type ]
closure_body ::= expression | block_expr
```

##### Static Semantics

**Typing**

A closure expression produces a value of function type (§6.4).

1.  **Non-capturing**: If the closure captures no variables, its type is a **sparse function pointer** `(Args) -> Ret`.
2.  **Capturing**: If the closure captures variables, its type is a **closure type** `|Args| -> Ret`.

**Capture Analysis**

Variables referenced in the closure body that are not parameters are **captured**.

1.  `const` bindings are captured by `const` reference.
2.  `shared` bindings are captured by `shared` reference.
3.  `unique` bindings MUST be captured by value. This requires an explicit `move` on the usage or parameter definition if the language allows, otherwise it is an error.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-EXP-2591` | Error    | Closure parameter type cannot be inferred.  | Compile-time | Rejection |
| `E-EXP-2593` | Error    | Capture of `unique` binding without `move`. | Compile-time | Rejection |

---

### 12.6 Control Flow Expressions

#### 12.6.1 Conditional Expression (`if`)

##### Definition

The `if` expression selects one of two branches for evaluation based on a boolean condition.

##### Syntax & Declaration

```ebnf
if_expr ::= "if" expression block_expr [ "else" ( block_expr | if_expr ) ]
```

##### Static Semantics

**(T-If)**
$$\frac{\Gamma \vdash e_c : \text{bool} \quad \Gamma \vdash e_t : T \quad \Gamma \vdash e_f : T}{\Gamma \vdash \texttt{if } e_c \{ e_t \} \texttt{ else } \{ e_f \} : T}$$

If the `else` branch is omitted, the `then` branch MUST have type `()`, and the expression has type `()`.

#### 12.6.2 Match Expression

##### Definition

The `match` expression selects a branch based on pattern matching against a scrutinee.

##### Syntax & Declaration

```ebnf
match_expr ::= "match" expression "{" match_arm+ "}"
match_arm ::= pattern [ "if" expression ] "=>" arm_body ","
```

##### Static Semantics

**(T-Match)**
$$\frac{\Gamma \vdash e_s : T_s \quad \forall i, \Gamma \vdash p_i : T_s \quad \forall i, \Gamma \vdash e_i : T_{res}}{\Gamma \vdash \texttt{match } e_s \{ p_i \Rightarrow e_i \} : T_{res}}$$

The set of patterns MUST be exhaustive for type $T_s$.

#### 12.6.3 Loop Expression

##### Definition

The `loop` expression repeatedly executes a block. It may produce a value upon termination via `break`.

##### Syntax & Declaration

```ebnf
loop_expr ::= [ label ] loop_kind
loop_kind ::=
    "loop" block_expr
  | "loop" expression block_expr
  | "loop" pattern "in" expression block_expr
```

##### Static Semantics

**(T-Loop)**
If every `break` statement in the loop body provides a value of type $T$, the loop expression has type $T$. If there are no `break` statements, the loop has type `!` (Never).

**Desugaring**

1.  `loop cond { body }` $\rightarrow$ `loop { if !cond { break } body }`
2.  `loop pat in iter { body }` $\rightarrow$ `match iter.next()` logic (see §12.6 dynamic semantics in source context).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-EXP-2601` | Error    | `if` condition is not of type `bool`.              | Compile-time | Rejection |
| `E-EXP-2602` | Error    | `if` branches have incompatible types.             | Compile-time | Rejection |
| `E-EXP-2611` | Error    | Match arms have incompatible types.                | Compile-time | Rejection |
| `E-EXP-2621` | Error    | Iterator expression does not implement `Iterator`. | Compile-time | Rejection |

---

### 12.7 Block Expressions

##### Definition

A **block expression** introduces a new lexical scope. It evaluates to the value of its final expression or an explicit `result` statement.

##### Syntax & Declaration

```ebnf
block_expr ::= "{" statement* [ expression ] "}"
```

##### Static Semantics

**(T-Block)**
If the block ends with an expression $e$, the type of the block is $\text{Type}(e)$.
If the block ends with a statement, the type of the block is `()`.
If the block contains `result e`, the type of the block is $\text{Type}(e)$.

##### Dynamic Semantics

**Evaluation**

1.  Statements are executed sequentially.
2.  If `result e` is encountered, the block terminates immediately, returning the value of $e$.
3.  Upon exit (normal or early), destructors for all bindings declared within the block are executed in reverse declaration order.

---

### 12.8 Declaration Statements

##### Definition

A **declaration statement** introduces a new binding into the current scope.

##### Syntax & Declaration

```ebnf
decl_stmt ::= ("let" | "var") pattern [ ":" type ] ("=" | ":=") expression
```

##### Static Semantics

1.  **`let`**: Immutable binding.
2.  **`var`**: Mutable binding.
3.  **`=`**: Movable binding (responsibility transfer permitted).
4.  **`:=`**: Immovable binding (responsibility transfer prohibited).

The pattern MUST be irrefutable.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-DEC-2401` | Error    | Reassignment of immutable `let` binding.         | Compile-time | Rejection |
| `E-DEC-2402` | Error    | Type annotation incompatible with inferred type. | Compile-time | Rejection |

---

### 12.9 Expression Statements

##### Definition

An **expression statement** evaluates an expression for side effects and discards the result.

##### Syntax & Declaration

```ebnf
expr_stmt ::= expression ";"
```

##### Static Semantics

The expression MUST be well-typed. The statement has type `()`.

---

### 12.10 Control Flow Statements

##### Definition

Control flow statements alter the sequential execution order.

##### Syntax & Declaration

```ebnf
return_stmt   ::= "return" [ expression ]
result_stmt   ::= "result" expression
break_stmt    ::= "break" [ label ] [ expression ]
continue_stmt ::= "continue" [ label ]
```

##### Static Semantics

1.  **`return`**: Terminates the enclosing procedure. The expression type MUST match the procedure return type.
2.  **`result`**: Terminates the enclosing block expression. The expression determines the block's value.
3.  **`break`**: Terminates the enclosing (or labeled) loop.
4.  **`continue`**: Skips to the next iteration of the enclosing (or labeled) loop.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-STM-2661` | Error    | `return` type mismatch with procedure. | Compile-time | Rejection |
| `E-STM-2662` | Error    | `break` outside `loop`.                | Compile-time | Rejection |
| `E-STM-2664` | Error    | `result` type mismatch with block.     | Compile-time | Rejection |

---

### 12.11 Assignment Statements

##### Definition

An **assignment statement** replaces the value stored in a place expression.

##### Syntax & Declaration

```ebnf
assignment_stmt ::= place_expr assignment_op expression
assignment_op ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
```

##### Static Semantics

**(T-Assign)**
$$\frac{\Gamma \vdash e_{lhs} : T \quad \Gamma \vdash e_{rhs} : T \quad \text{Cat}(e_{lhs}) = \text{Place} \quad \text{Mut}(e_{lhs})}{\Gamma \vdash e_{lhs} = e_{rhs} : \text{stmt}}$$

The left-hand side MUST be a mutable place expression.

##### Dynamic Semantics

**Drop Semantics**

Assignment to a responsible binding is a **destructive update**.
1.  Evaluate $e_{rhs}$.
2.  Invoke the destructor (`Drop`) of the value currently in $e_{lhs}$.
3.  Move the value of $e_{rhs}$ into $e_{lhs}$.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-STM-2631` | Error    | Assignment target is not a place expression. | Compile-time | Rejection |
| `E-STM-2633` | Error    | Assignment type mismatch.                    | Compile-time | Rejection |

---

### 12.12 Defer Statements

##### Definition

A **defer statement** registers a block of code to be executed when the enclosing scope exits.

##### Syntax & Declaration

```ebnf
defer_stmt ::= "defer" block_expr
```

##### Static Semantics

The block expression MUST have type `()`. The block MUST NOT contain control flow statements that transfer execution outside the block (`return`, `break`, `continue`, `result`).

##### Dynamic Semantics

**Unified LIFO Cleanup**

Upon scope exit, cleanup actions are executed in **Last-In, First-Out (LIFO)** order based on their declaration point. The cleanup stack unifies:
1.  `defer` blocks.
2.  Implicit destructor calls for variables going out of scope.

**Panic Interaction**

If a panic occurs during a defer block, execution proceeds to the next item in the cleanup stack.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-STM-2651` | Error    | Defer block has non-unit type.         | Compile-time | Rejection |
| `E-STM-2652` | Error    | Non-local control flow in defer block. | Compile-time | Rejection |

---

## Clause 13: The Capability System

### 13.1 Foundational Principles

##### Definition

A **Capability** is a first-class value representing unforgeable authority to perform a specific class of external effects or side-effecting operations. The Capability System enforces the principle of **No Ambient Authority**: no procedure may perform an observable effect unless it holds a reference to a capability granting authority for that effect.

**Formal Definition**

A capability $C$ is a triple:

$$C = (\text{Authority}, \text{Interface}, \text{Lineage})$$

where:
- $\text{Authority} \subseteq \mathcal{E}$ is the set of permitted effects (operations on external resources).
- $\text{Interface}$ is the nominal type (form or class) defining the methods available on $C$.
- $\text{Lineage}$ is the derivation chain tracing $C$ back to a root capability provided by the runtime.

##### Static Semantics

**Authority Derivation Rule**

A procedure $P$ is authorized to perform effect $e$ if and only if $P$ possesses a valid binding to a capability $C$ such that $e \in \text{Authority}(C)$.

$$\frac{\Gamma \vdash c : \$T \quad e \in \text{Authority}(T)}{\Gamma \vdash P(c) \text{ authorizes } e}$$

**Ambient Authority Prohibition**

Access to global mutable state or system resources without an explicit capability parameter is prohibited. The implementation MUST verify that the call graph of any effect-performing procedure traces back to a capability introduced via parameters.

A procedure $A$ calls procedure $B$. If $B$ requires capabilities $\text{Caps}(B)$, then $A$ MUST possess capabilities $\text{Caps}(A)$ such that $\text{Caps}(B) \subseteq \text{Caps}(A)$. The set of required capabilities for a procedure is defined as the set of parameters declared with the capability sigil `$`.

**Intrinsic Operations**

Intrinsic operations that perform side effects (e.g., file I/O syscalls, network transmission) MUST be defined as methods on system capability types. They MUST NOT be exposed as global functions.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                           | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------------------ | :----------- | :-------- |
| `E-CAP-1001` | Error    | Ambient authority detected: global procedure performs side effects. | Compile-time | Rejection |

---

### 13.2 The Root of Authority

##### Definition

The **Capability Root** is the set of capabilities created by the runtime environment and injected into the program at startup. This set represents the maximal authority granted to the process by the host environment.

##### Static Semantics

**Runtime Injection**

The Cursive runtime MUST construct a `Context` record (§H.1) prior to the execution of the user program. This record MUST contain concrete implementations of all system capability forms defined in Appendix G, initialized with authority corresponding to the process's operating system privileges.

**Entry Point Requirement**

The program entry point `main` MUST accept the capability root as its sole parameter, as specified in §8.9.

$$\text{signature}(\texttt{main}) = (\texttt{ctx}: \texttt{Context}) \to \texttt{i32}$$

**Context Integrity**

The `Context` record passed to `main` MUST be unique and effectively `unique` (unaliased) at the start of execution.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-DEC-2431` | Error    | `main` signature does not accept `Context` capability root. | Compile-time | Rejection |

---

### 13.3 Capability Parameters and Propagation

##### Definition

**Capability Propagation** is the passing of capability values through the call stack. A procedure declares a requirement for authority by listing capability types in its parameter list.

##### Syntax & Declaration

**Capability Type Syntax**

A capability type is denoted by the sigil `$` prefixing a class name.

```ebnf
capability_type ::= "$" type_path
```

The syntax `$T` is semantically equivalent to a bounded generic parameter constrained to `T`, specialized for capability semantics.

##### Static Semantics

**Polymorphism**

A parameter of type `$T` accepts any concrete type $C$ such that $C$ implements the capability form $T$.

**Permission Semantics**

Capability methods MUST declare receiver permissions consistent with the nature of their operations:

| Operation Category | Required Receiver Permission | Semantics                                                                    |
| :----------------- | :--------------------------- | :--------------------------------------------------------------------------- |
| **Query**          | `const` (`~`)                | Pure observation; no state change; thread-safe.                              |
| **Action**         | `shared` (`~%`)              | Side-effecting operation; internal state mutation; requires synchronization. |
| **Mutation**       | `unique` (`~!`)              | Structural modification of the capability itself.                            |

**Propagation Rule**

A procedure may only pass a capability $C$ to a callee if the procedure holds a valid binding to $C$ with sufficient permissions.

**Principle of Least Authority**

Procedures SHOULD declare parameters for specific capabilities (e.g., `Reactor`, `FileSystem`) rather than accepting the root `Context` record, unless the procedure serves as a top-level application orchestrator. Implementations MUST emit a warning (`W-CAP-1005`) if `Context` is passed to a procedure that utilizes fewer than 3 distinct capabilities derived from it.

##### Constraints & Legality

**Explicit Propagation**

Implicit capture of capabilities from the environment (other than lexical closure capture defined in §13.5) is prohibited. A procedure body MUST NOT access a capability unless it is:
1. A parameter of the procedure.
2. Created locally within the procedure (e.g., via attenuation).
3. A field of a record passed to the procedure.

**Diagnostic Table**

| Code         | Severity | Condition                                                   | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------------- | :----------- | :-------- |
| `E-CAP-1002` | Error    | Effect-producing procedure lacks required capability param. | Compile-time | Rejection |

---

### 13.4 Capability Attenuation

##### Definition

**Attenuation** is the operation of constructing a new capability $C_{child}$ from an existing capability $C_{parent}$ such that the authority of the child is a proper subset of the authority of the parent.

**Formal Definition**

An attenuation function $f$ satisfies:

$$f(C_{parent}, R) \to C_{child} \implies \text{Authority}(C_{child}) \subseteq \text{Authority}(C_{parent})$$

where $R$ specifies the restriction (e.g., a file path prefix, a network subnet, or a resource quota).

##### Static Semantics

**Type Preservation**

Attenuation MUST preserve the capability form. If $C_{parent} : \$T$, then $C_{child}$ MUST implement $T$ or a subtype of $T$.

$$\frac{\Gamma \vdash c : \$T \quad \Gamma \vdash c.\text{restrict}(r) : \$T'}{\Gamma \vdash T' <: T}$$

**Delegation Requirement**

The implementation of an attenuated capability MUST delegate authorized operations to the parent capability. The child capability MUST NOT bypass the parent to access system resources directly.

##### Constraints & Legality

**Amplification Prohibition**

It is ill-formed to derive a capability $C_{child}$ from $C_{parent}$ if $\text{Authority}(C_{child}) \not\subseteq \text{Authority}(C_{parent})$. While exact authority sets may be undecidable at compile time, implementations MUST reject derivations that structurally increase authority (e.g., casting a read-only file capability to a read-write capability).

---

### 13.5 Capability-Concurrency Interaction

##### Definition

This section defines the rules for capturing and using capabilities within concurrent constructs defined in Clause 15 (`spawn`, `dispatch`, `parallel`).

##### Static Semantics

**Capture Rules**

When a closure or concurrent block captures a capability binding from its enclosing scope, the following permission rules apply based on the capture mode:

1.  **`const` Capabilities**: Captured by reference. Multiple concurrent tasks MAY hold `const` references to the same capability.
2.  **`shared` Capabilities**: Captured with `shared` permission. Concurrent access is permitted. The capability implementation MUST ensure thread safety for `shared` methods (typically via internal locking or lock-free structures).
3.  **`unique` Capabilities**: MUST be moved into the closure. Ownership transfers to the spawned task. The capability is no longer accessible in the parent scope.

**Isolation of Key State**

While capabilities may be shared or moved into spawned tasks, the **Key State Context** ($\Gamma_{\text{keys}}$, §14.1) is **NOT** inherited. Each spawned task begins with an empty key state. A task must acquire its own keys to `shared` data, even if the capability to access that data was captured from the parent.

**Capability Delegation**

Spawned tasks inherit the *potential* for authority from their parent. If a parent procedure holds capability $C$, a spawned child MAY capture $C$. This does not violate the No Ambient Authority principle, as the authority is explicitly transferred via closure capture.

##### Dynamic Semantics

**Concurrent Use**

1.  **Read-Only Operations**: Concurrent invocations of `const` methods on the same capability instance MUST proceed without blocking, subject to hardware limits.
2.  **Side-Effecting Operations**: Concurrent invocations of `shared` methods on the same capability instance MUST be synchronized by the capability implementation. The ordering of these effects is Unspecified unless explicitly ordered by synchronization primitives.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                  | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------- | :----------- | :-------- |
| `E-CAP-1003` | Error    | `unique` capability captured by multiple concurrent tasks. | Compile-time | Rejection |

---

### 13.6 Attenuated Capability Lifecycle

##### Definition

Attenuated capabilities maintain a strong lifetime dependency on their parent capability.

##### Static Semantics

**Lifetime Constraint**

For any attenuation $C_{child} = \text{Attenuate}(C_{parent})$, the lifetime of $C_{parent}$ MUST strictly encompass the lifetime of $C_{child}$.

$$\text{Lifetime}(C_{parent}) \supseteq \text{Lifetime}(C_{child})$$

**Drop Ordering**

1.  **Child Drop Independence**: Dropping $C_{child}$ MUST NOT affect the validity of $C_{parent}$.
2.  **Parent Drop Restriction**: $C_{parent}$ MUST NOT be dropped while any $C_{child}$ derived from it remains live. This is enforced by the ownership and borrowing rules of the type system (Clause 4).

##### Dynamic Semantics

**Invalidation**

If a parent capability is explicitly revoked or closed (e.g., closing a file handle), all derived capabilities MUST effectively be invalidated. Subsequent operations on derived capabilities MUST fail (e.g., return an error or panic).

##### Constraints & Legality

**Escape Analysis**

An attenuated capability MUST NOT escape the scope of its parent.

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-CAP-1004` | Error    | Derived capability outlives parent capability. | Compile-time | Rejection |

---

## Clause 14: The Key System

### 14.1 Key Fundamentals

##### Definition

A **key** is a **static proof of access rights** to a specific path within `shared` data. Keys are primarily a compile-time verification mechanism: key state is tracked during type checking to ensure safe access patterns. Runtime synchronization is introduced only when static analysis cannot prove safety.

**Formal Definition**

A key is a triple:

$$\text{Key} ::= (\text{Path}, \text{Mode}, \text{Scope})$$

| Component | Type                            | Description                                     |
| :-------- | :------------------------------ | :---------------------------------------------- |
| Path      | $\text{PathExpr}$               | The memory location being accessed              |
| Mode      | $\{\text{Read}, \text{Write}\}$ | The access grant (§14.1.2)                      |
| Scope     | $\text{LexicalScope}$           | The lexical scope during which the key is valid |

**Key State Context**

Key state MUST be tracked at each program point using a key state context $\Gamma_{\text{keys}}$, which maps the current program point to the set of keys logically held.

##### Static Semantics

**Key Invariants**

1.  **Exclusivity:** If a Write key to path $P$ is held, no other key (Read or Write) to $P$ or any path overlapping $P$ may be held by any other execution context.
2.  **Validity:** Access to a `shared` memory location $L$ is permitted if and only if a key covering the path to $L$ with sufficient mode is present in $\Gamma_{\text{keys}}$.
3.  **Scope-Boundedness:** A key acquired within scope $S$ MUST be released upon exit from $S$.

#### 14.1.1 Path Expressions

##### Definition

A **path expression** identifies a location within a value hierarchy. It serves as the address for key acquisition.

##### Syntax & Declaration

```ebnf
path_expr       ::= root_segment ("." path_segment)*
root_segment    ::= key_marker? identifier index_suffix?
path_segment    ::= key_marker? identifier index_suffix?
key_marker      ::= "#"
index_suffix    ::= "[" expression "]"
```

**Path Root Extraction**

The **root** of an expression, written $\text{Root}(e)$, extracts the base identifier. Pointer dereferences (`*ptr`) establish **Key Boundaries**, meaning the path resets at the dereferenced value (based on runtime identity).

##### Static Semantics

**Path Well-Formedness**

A path expression is well-formed if it resolves to a valid sequence of field or index accesses starting from a bound identifier.

#### 14.1.2 Key Modes

##### Definition

**Key Modes** determine permitted operations and compatibility.

| Mode  | Semantics                             | Compatibility                            |
| :---- | :------------------------------------ | :--------------------------------------- |
| Read  | Shared access; mutation prohibited.   | Compatible with other Read keys.         |
| Write | Exclusive access; mutation permitted. | Incompatible with all keys (Read/Write). |

**Mode Determination**

- **Write Context:** Required for assignment LHS (`x = ...`), `unique` arguments, and mutating methods (`~!`, `~%`).
- **Read Context:** Required for all other accesses.

---

#### 14.1.3 Path Disjointness

##### Definition

Safe concurrent access relies on proving that two paths refer to non-overlapping memory.

##### Static Semantics

**Index Equivalence**

Two index expressions $e_1$ and $e_2$ are **provably equivalent** ($e_1 \equiv_{idx} e_2$) if:
1. Both are the same integer literal.
2. Both refer to the same immutable binding (identifier identity).
3. Both normalize to the same canonical form under constant folding and linear arithmetic simplification.

**Provable Disjointness**

Two paths $P$ and $Q$ are **provably disjoint** ($\text{Disjoint}(P, Q)$) if:
1. They have different roots ($\text{Root}(P) \neq \text{Root}(Q)$).
2. They diverge at a specific segment $i$ such that $p_i$ and $q_i$ are distinct fields.
3. They diverge at a specific segment $i$ such that $p_i = [e_1]$ and $q_i = [e_2]$, and the implementation can prove $e_1 \neq e_2$ via:
    *   **Static Values:** $\text{IsConst}(e_1) \land \text{IsConst}(e_2) \land \text{Val}(e_1) \neq \text{Val}(e_2)$.
    *   **Algebraic Offset:** $e_1 = v + k_1$ and $e_2 = v + k_2$ where $k_1 \neq k_2$.
    *   **Control Flow:** Verification Facts (§11.5) establish disjointness (e.g., `if i != j`).

If two paths are provably disjoint, concurrent access (including Read-Write or Write-Write) is **statically safe** and requires no runtime key acquisition.

---

### 14.2 Automatic Key Acquisition

##### Definition

**Automatic Key Acquisition** is the mechanism by which the implementation implicitly acquires keys for `shared` paths accessed outside of explicit `#` blocks. This ensures that every access to `shared` data is protected by a valid key, enforcing the **Total Coverage Invariant**.

##### Static Semantics

**Implicit Scope Rule**

When a `shared` path $P$ is accessed in an expression $e$ and no covering key is currently held in $\Gamma_{\text{keys}}$:

1. A key for $P$ is implicitly acquired immediately prior to the evaluation of $e$.
2. The mode $M$ is determined by the context of $e$ (Read or Write).
3. The scope $S$ of the key is the **innermost enclosing statement**.

**(K-Implicit-Acquire)**
$$\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    M = \text{RequiredMode}(P) \quad
    \neg\text{Covered}(P, M, \Gamma_{\text{keys}})
}{
    \Gamma_{\text{keys}}' = \Gamma_{\text{keys}} \cup \{(P, M, \text{CurrentStatement})\}
}$$

**Statement-Level Release**

Keys acquired automatically are released at the **statement boundary** (the semicolon or end of control flow condition).

```cursive
// Automatic Acquisition Example
player.health += 10;
// 1. Acquire Write key for `player.health`
// 2. Read value
// 3. Add 10
// 4. Write value
// 5. Release key (at semicolon)
```

**Acquisition Order**

Within a single statement, the implementation MUST evaluate operands in strict left-to-right order. After operand evaluation and before the operation executes, all required keys MUST be acquired atomically in Canonical Order (§14.7).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------- | :----------- | :-------- |
| `W-KEY-0001` | Warning  | Implicit key acquisition in tight loop (perf hazard).    | Compile-time | Warning   |
| `E-KEY-0005` | Error    | Implicit acquisition requires Write, but only Read held. | Compile-time | Rejection |

---

### 14.3 Manual Coarsening

##### Definition

**Manual Coarsening** is the deliberate widening of key granularity using the `#` marker within a path expression. It allows a programmer to acquire a lock on a parent object (the "root" of the coarsening) to cover accesses to multiple child paths, thereby reducing synchronization overhead and ensuring atomicity across multiple operations.

##### Syntax & Declaration

**Grammar**

The `#` marker represents a coarsening point in a path expression (see §14.1.1).

##### Static Semantics

**Coarsening Logic**

When the `#` marker appears before a segment `s` in path $P = p_1.\dots.p_i.\#s.\dots.p_n$:

1. The **Key Path** is truncated at `s`: $\text{KeyPath}(P) = p_1.\dots.p_i.s$.
2. The key acquired covers $P$ and all other paths rooted at $p_1.\dots.p_i.s$.
3. Accesses to children (e.g., $s.x$, $s.y$) are covered by this coarser key.

**Example:**
```cursive
// Path: player.#stats.health
// Key Acquired: player.stats
// Covered: player.stats.health, player.stats.mana
```

**Method Receiver Coarsening**

To coarsen a key for a method call, the `#` marker MUST be placed on the *receiver*, not the method name.

- `#receiver.method()`: Acquires key for `receiver`, then calls `method`.
- `receiver.#method()`: **Ill-formed** (`E-KEY-0030`). Keys apply to data paths, not code.

**(K-Coarsen-Method)**
$$\frac{
    e = \#r.m(\dots)
}{
    \text{KeyPath}(e) = \text{Root}(r) \quad
    \text{Scope}(e) = \text{Statement}
}$$

**Effectiveness Rule**

Manual coarsening is only effective if the coarsened path is a **prefix** of the target data.

##### Constraints & Legality

**Negative Constraints**

1. **Redundant Coarsening:** Using `#` on a path that is already covered by a type-level boundary (§14.5) is valid but triggers a warning (`W-KEY-0003`).
2. **Invalid Placement:** The `#` marker MUST NOT appear on a variable that is not `shared`.
3. **Double Coarsening:** A path MUST NOT contain more than one `#` marker (`E-KEY-0003`).

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-KEY-0003` | Error    | Multiple `#` markers in path.            | Compile-time | Rejection |
| `E-KEY-0030` | Error    | `#` applied to method name.              | Compile-time | Rejection |
| `W-KEY-0003` | Warning  | Redundant `#` (matches type definition). | Compile-time | Warning   |

---

### 14.4 The `#` Key Block

##### Definition

The **`#` Key Block** is the syntactic construct for explicit, scoped key acquisition. It defines a lexical scope during which keys to specific paths are held.

##### Syntax & Declaration

**Grammar**

```ebnf
key_block       ::= "#" path_list mode_modifier* block
path_list       ::= path_expr ("," path_expr)*
mode_modifier   ::= "write" | "read" | release_clause | "ordered" | "speculative"
release_clause  ::= "release" ("write" | "read")
```

##### Static Semantics

**Acquisition Rule (K-Block-Acquire)**

Upon entry to `#` block $B$ listing paths $P_1, \ldots, P_n$:

1.  Sort paths into **Canonical Order** ($Q_1, \ldots, Q_n$) per §14.7.
2.  Acquire keys for all $Q_i$ atomically.
3.  Execute block $B$.
4.  Release all keys.

**Implicit Read Mode**

If the `write` modifier is absent, the block acquires Read keys. Write operations to keyed paths within a Read block are ill-formed (`E-KEY-0070`).

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0031` | Error    | `#` block path references identifier not in scope.     | Compile-time | Rejection |
| `E-KEY-0032` | Error    | `#` block path root is not `shared`.                   | Compile-time | Rejection |
| `E-KEY-0070` | Error    | Write operation in `#` block without `write` modifier. | Compile-time | Rejection |

---

### 14.5 Type-Level Key Boundaries

##### Definition

A **Type-Level Key Boundary** is a permanent granularity constraint declared on a record field using the `#` marker.

##### Syntax & Declaration

```ebnf
field_decl ::= "#"? identifier ":" type
```

##### Static Semantics

**Boundary Propagation**

If a field `f` is declared as `#f: T`, any path traversing `f` (e.g., `x.f.y`) is automatically coarsened to `x.f`. The key acquired is for `x.f`, covering all sub-paths.

**Interaction with Manual Coarsening**

Manual coarsening (`#`) can widen the key further (e.g., to `x` in `#x.f.y`) but CANNOT narrow it below the type-level boundary.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-KEY-0033` | Error    | `#` marker on field of non-record type. | Compile-time | Rejection |

---

### 14.6 Read-Then-Write Prohibition

##### Definition

The **read-then-write prohibition** is a static safety rule that prevents non-atomic read-modify-write sequences on `shared` data within a single statement. A **read-then-write pattern** occurs when a statement reads from a `shared` path and subsequently writes to the same path (or an overlapping path) without holding a covering Write key.

This prohibition forces explicit synchronization for operations that appear atomic but would otherwise execute as distinct, unprotected transactions (a read transaction followed by a write transaction).

**Formal Definition**

Let $\text{Subexpressions}(S)$ denote the set of all subexpressions within a statement $S$.

The predicates $\text{Reads}(e, P)$ and $\text{Writes}(e, P)$ hold when expression $e$ performs a read or write access, respectively, to path $P$ or any path overlapping $P$.

A statement $S$ exhibits a **read-then-write pattern** for path $P$ if:

$$
\exists e_r, e_w \in \text{Subexpressions}(S) : 
\text{Reads}(e_r, P) \land \text{Writes}(e_w, P) \land e_r \text{ is evaluated before } e_w
$$

##### Static Semantics

**Prohibition Rule (K-Read-Write-Reject)**

A statement exhibiting a read-then-write pattern for path $P$ is ill-formed unless a Write key covering $P$ is statically held in the enclosing scope.

$$
\frac{
    \Gamma \vdash P : \texttt{shared}\ T \quad
    \text{ReadThenWrite}(P, S) \quad
    \neg\text{Held}(P, \text{Write}, \Gamma_{\text{keys}})
}{
    \text{Emit}(\texttt{E-KEY-0060})
}
$$

This rule ensures that a read-modify-write sequence within a single statement (e.g., `x = x + 1`) is either atomic (protected by an existing lock) or rejected. It prevents the implicit acquisition of a Read key followed by a distinct implicit Write key, which would leave an atomicity gap.

**Compound Assignment Exception**

Compound assignment operators (`+=`, `-=`, etc.) are exempt from this prohibition because they desugar to an explicit `#` block with `write` mode.

The statement `p += e` desugars to:

```cursive
#p write {
    p = p + e
}
```

This desugaring ensures the Write key is acquired *before* the read occurs, eliminating the atomicity gap.

**Separate Statements Permission**

Reading and writing the same `shared` path in **separate statements** is **permitted**.

Because keys acquired implicitly are released at the end of the statement (§14.2), no key is held across the statement boundary. Therefore, no key upgrade (Read $\to$ Write) occurs, and no atomicity gap exists within a held lock duration.

*   **Statement 1 (Read):** Acquires Read key, reads value, releases Read key.
*   **Statement 2 (Write):** Acquires Write key, writes value, releases Write key.

$$
\frac{
    S_1 \text{ reads } P \quad
    S_2 \text{ writes } P \quad
    S_1 \text{ precedes } S_2
}{
    \text{Valid (Sequentially Consistent)}
} \quad \text{(K-Separate-Stmts)}
$$

> **Note:** While valid, this pattern is not atomic. Other tasks may modify $P$ between $S_1$ and $S_2$. This is a logic race, not a data race.

**Aliasing Assumptions**

When two paths $P_1$ and $P_2$ cannot be statically proven disjoint (§14.1.1), they MUST be assumed to overlap. A statement reading $P_1$ and writing $P_2$ is rejected if $P_1$ and $P_2$ potentially alias and no covering Write key is held.

$$
\frac{
    \text{Reads}(S, P_1) \quad \text{Writes}(S, P_2) \quad \neg\text{ProvablyDisjoint}(P_1, P_2) \quad \neg\text{Held}(P_1, \text{Write})
}{
    \text{Emit}(\texttt{E-KEY-0060})
}
$$

##### Constraints & Legality

**Negative Constraints**

1. A program MUST NOT contain a read-then-write pattern on a `shared` path within a single statement unless a covering Write key is held.
2. A program MUST NOT rely on implicit promotion from Read key to Write key; key modes are static and fixed for the duration of the implicit scope.

**Diagnostic Table**

| Code         | Severity | Condition                                                              | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0060` | Error    | Read-then-write on `shared` path without covering Write key.           | Compile-time | Rejection |
| `W-KEY-0004` | Warning  | Sequential read/write pattern detected (potential logic race).         | Compile-time | Warning   |
| `W-KEY-0006` | Warning  | Explicit read-then-write block used where compound assignment applies. | Compile-time | Warning   |

---

### 14.7 Deadlock Prevention

##### Definition

Deadlock freedom is enforced via **Canonical Ordering** of key acquisition.

##### Static Semantics

**Canonical Order**

Paths in a `#` block MUST be acquired in lexicographical order.

**Dynamic Indices**

If paths involve dynamic indices (`a[i]`, `a[j]`):
1.  If statically comparable (e.g., `i`, `i+1`), use static order.
2.  If incomparable and outside `[[dynamic]]` scope: **Error** (`E-KEY-0011`).
3.  If incomparable and inside [[dynamic]] scope: The usage is ill-formed (E-KEY-0013) unless the block is marked speculative. Blocking acquisition of dynamically ordered resources is prohibited to prevents hidden runtime overhead. Users must employ speculative blocks (§14.11) which utilize optimistic concurrency control, or manually sort indices prior to the key block.

**Dynamic Ordering Properties**
When runtime ordering is used (`[[dynamic]]`), the implementation MUST enforce a total ordering relation $\text{DynOrder}(P, Q)$ over all addressable memory locations. This relation MUST satisfy:
1.  **Totality**: For distinct $P, Q$, exactly one of $\text{DynOrder}(P, Q)$ or $\text{DynOrder}(Q, P)$ holds.
2.  **Transitivity**: $\text{DynOrder}(P, Q) \land \text{DynOrder}(Q, R) \implies \text{DynOrder}(P, R)$.
3.  **Cross-Task Consistency**: All tasks compute the same ordering for the same paths.
4.  **Value-Determinism**: Ordering depends only on runtime values (addresses/indices), not task identity or timing.
5.  **Stability**: The ordering relation $\text{DynOrder}(P, Q)$ MUST remain invariant for the duration of the key block. If the underlying storage of a keyed path is subject to relocation (e.g., via region compaction or garbage collection), the implementation MUST implicitly **pin** the storage in memory prior to computing the order and acquiring keys, and unpin it only after the keys are released.

**Key Release and Reacquisition**
The **Key Release Sequence** is an atomic operation used by suspension constructs (e.g., `yield release`) to temporarily drop authority to prevent deadlocks.
1.  **Release:** The execution context identifies all currently held keys and releases them in **Reverse Canonical Order** (lexicographically descending).
2.  **Reacquisition:** Upon resumption, the context MUST reacquire all previously held keys in **Canonical Order**. If acquisition blocks, the task remains suspended. If acquisition fails (e.g., due to cancellation), the task transitions to `@Failed`.

**Panic Safety and Unwinding**
If the execution context terminates abnormally (e.g., due to a panic or exception), the implementation MUST execute the **Key Release Sequence** (§14.7) for all keys held by the current scope. This release MUST occur **prior** to the execution of `defer` blocks or the destruction of local bindings (§3.6).

1.  **Identify:** The runtime identifies the set of keys $K_{held}$ associated with the unwinding scope.
2.  **Release:** The runtime releases all $k \in K_{held}$ in **Reverse Canonical Order**.
3.  **Proceed:** The runtime proceeds with the standard Destruction Sequence defined in §3.6.
   
---

### 14.8 Nested Key Release

##### Definition

**Nested Key Release** handles mode transitions (Read $\to$ Write) in nested blocks. The `release` modifier is required to acknowledge that the outer key is released before the inner key is acquired, creating an atomicity gap.

##### Syntax & Declaration

```cursive
#path {
    // Read key held
    #path release write {
        // Read released -> Gap -> Write acquired
    }
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-KEY-0012` | Error    | Nested mode change without `release`.          | Compile-time | Rejection |
| `W-KEY-0011` | Warning  | Access to potentially stale binding after gap. | Compile-time | Warning   |

---

### 14.9 Verification and Realization

##### Definition

**Key Verification** is the process of establishing the validity of a key requirement. Verification occurs either statically (via compiler proofs) or dynamically (via runtime checks).

**Realization** is the transformation of abstract static keys into concrete runtime mechanisms within `[[dynamic]]` scopes.

##### Static Semantics

**Verification Mode Selection**

The verification mode for a key requirement is determined by the presence of the `[[dynamic]]` attribute in the enclosing scope.

**Static Verification (Default)**

If the scope is **not** `[[dynamic]]`, the implementation MUST prove disjointness and access rights statically.

$$\frac{
    \neg\text{InDynamicScope}(\Gamma) \quad
    \neg\text{StaticProof}(\Gamma, \text{Disjoint}(P, Q))
}{
    \text{Error: E-KEY-0020}
} \quad \text{(Verify-Static)}$$

**Dynamic Realization (Opt-In)**

If the scope **is** `[[dynamic]]`, the compiler MUST emit runtime instructions to enforce the **Exclusivity Invariant** (§14.1) dynamically using the Key Repository model.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0020` | Error    | Key safety not statically provable outside dynamic scope. | Compile-time | Rejection |

---

#### 14.9.1 Runtime Key Repository

##### Definition

A **Key Repository** is a runtime synchronization primitive associated with a specific addressable `shared` memory location. It serves as the single source of truth for access authority, governing the circulation of keys, the coordination of waiters, and the validation of speculative snapshots.

To support the requirements of Coarsening (§14.3) and Speculative Execution (§14.11), the Repository functions as a **Versioned Hierarchical Reader-Writer Lock**.

**Formal Definition**

Let $\mathcal{R}_L$ be the repository for location $L$. The state of $\mathcal{R}_L$ is defined as the tuple:

$$ \mathcal{R}_L = (\text{LockState}, N_{read}, \text{Version}, \mathcal{Q}_{wait}, \mathcal{R}_{parent}) $$

where:
- **$\text{LockState} \in \{ \texttt{Quiescent}, \texttt{Shared}, \texttt{Exclusive} \}$**: The current locking mode.
- **$N_{read} \in \mathbb{N}$**: The count of active Read Keys (witness tokens) minted from this repository.
- **$\text{Version} \in \mathbb{N}_{64}$**: A monotonically increasing sequence number, incremented upon every release of the Write Key. This supports Seqlock-style speculative validation.
- **$\mathcal{Q}_{wait}$**: A FIFO queue of Execution Contexts suspended while waiting for keys or condition predicates (§16.6.3) on this path.
- **$\mathcal{R}_{parent} \in \text{Option}\langle \text{Ref}\langle \mathcal{R} \rangle \rangle$**: A reference to the repository of the immediate containment parent (if one exists), supporting hierarchical locking checks.

##### Dynamic Semantics

**Repository States**

The implementation MUST maintain the repository in one of three mutually exclusive logic states, mapped to the tuple components as follows:

| State         | LockState   | $N_{read}$ | Description                                                                     |
| :------------ | :---------- | :--------- | :------------------------------------------------------------------------------ |
| **Quiescent** | `Quiescent` | $0$        | No active keys. Version is stable.                                              |
| **Shared**    | `Shared`    | $> 0$      | Read access active. Write Key is **pinned** inside the Repo. Version is stable. |
| **Exclusive** | `Exclusive` | $0$        | Write access active. Write Key is **taken**. Version is potentially unstable.   |

**Hierarchical Consistency**

To support Manual Coarsening (§14.3), an operation on $\mathcal{R}_L$ MUST respect the state of $\mathcal{R}_{parent}$:
1.  **Acquire Write:** Valid only if $\mathcal{R}_{parent}$ is NOT in the `Exclusive` or `Shared` state (unless the current context already holds the parent key).
2.  **Acquire Read:** Valid only if $\mathcal{R}_{parent}$ is NOT in the `Exclusive` state.

**Speculative Invariant**

The `Version` field MUST satisfy the following properties:
1.  **Atomicity:** Updates to `Version` are atomic with respect to the `LockState`.
2.  **Monotonicity:** $\text{Version}_{t+1} \ge \text{Version}_t$.
3.  **Modification:** The `Version` is incremented exactly once per Write Key cycle (specifically, during the **Return Write Key** operation defined in §14.9.2). This allows speculative blocks to validate that no writes occurred by comparing start and end versions.

##### Memory & Layout

**Representation**

The layout of the Key Repository is **Implementation-Defined**. Implementations SHOULD optimize the representation for space efficiency.

*   **Packed Word:** The `LockState`, `N_read`, and `Version` MAY be packed into a single atomic machine word (e.g., using the high bits for the version and low bits for the state/count) to enable atomic snapshotting.
*   **Lazy Inflation:** The `Q_wait` and `R_parent` fields MAY be allocated lazily (inflated) only when contention occurs or hierarchy is explicitly invoked.

**Zero-Size Optimization**

If the compiler can statically prove that a path is never subject to hierarchy (no coarsening) or speculation, those specific fields MAY be elided from the runtime instance.

---

#### 14.9.2 Key Transaction Protocol

##### Definition

The **Key Transaction Protocol** defines the four atomic operations used to mediate access in `[[dynamic]]` scopes. These operations replace "locking" semantics with resource movement semantics.

##### Dynamic Semantics

**Transaction Algorithms**

The implementation MUST execute the following algorithms atomically with respect to the repository state.

1.  **Claim Write Key (Acquire Write)**
    *   **Precondition:** Repository is **Quiescent** ($\text{WriteKeyLoc} = \texttt{InRepo} \land N_{read} = 0$).
    *   **Action:** Transition $\text{WriteKeyLoc} \to \texttt{Taken}$.
    *   **Blocking:** If the Repository is **Shared** or **Exclusive**, the context blocks until the state becomes Quiescent.

2.  **Mint Read Key (Acquire Read)**
    *   **Precondition:** Repository is **Quiescent** or **Shared** ($\text{WriteKeyLoc} = \texttt{InRepo}$).
    *   **Action:** Increment $N_{read}$.
    *   **Blocking:** If the Repository is **Exclusive** ($\text{WriteKeyLoc} = \texttt{Taken}$), the context blocks until the Write Key is returned.

3.  **Return Write Key (Release Write)**
    *   **Action:** Transition $\text{WriteKeyLoc} \to \texttt{InRepo}$.
    *   **Effect:** State transitions from **Exclusive** to **Quiescent**. Waiting contexts are signaled.

4.  **Surrender Read Key (Release Read)**
    *   **Action:** Decrement $N_{read}$.
    *   **Effect:** If $N_{read}$ becomes 0, state transitions from **Shared** to **Quiescent**. Contexts waiting for the Write Key are signaled.

**Semantic Alignment**

This model preserves the static semantic invariants:
1.  **Exclusivity:** A Context cannot hold the Write Key if any other Context holds a key (Read or Write), because the Write Key must be `InRepo` to mint Read Keys.
2.  **Immutability Witness:** Holding a Read Key guarantees the Write Key is trapped in the Repository (`InRepo`), proving that no other Context can mutate the data.

---

#### 14.9.3 Opaque Key Representation

##### Definition

The **Opaque Key Representation** is the runtime manifestation of a key held by an execution context.

##### Static Semantics

**Opacity**

The runtime representation of a Key is **Implementation-Defined** and **Opaque**. User code MUST NOT inspect, serialize, or store a runtime Key outside of the scope in which it was acquired.

**Unit Witness**

From the perspective of the user program, a Dynamic Key has the unit type `()`. It serves purely as a **Witness** to the validity of the access.

##### Memory & Layout

**Zero-Size Optimization**

Implementations MAY represent the held Key as a zero-sized type (ZST) if the necessary state (e.g., reentrancy counters or owner IDs) is maintained within the Repository or the Execution Context.

---

#### 14.9.4 Fallback Protocol

##### Definition

The **Fallback Protocol** (or Canonical Claim Sequence) is the blocking mechanism used when speculative execution (§14.11) fails to commit. It enforces the global ordering invariant to prevent deadlocks during high contention.

##### Dynamic Semantics

**Canonical Claim Sequence**

If the speculative retry limit is exceeded, the implementation MUST execute the following sequence:

1.  **Identify:** Determine the set of Keys $K_{req}$ required for the block.
2.  **Order:** Sort $K_{req}$ according to the Canonical Order defined in §14.7.
3.  **Claim:** Iterate through $k \in K_{req}$ in order:
    *   Inspect the Key Repository for $k$.
    *   Execute **Claim Write Key** (if $k$ is a Write Key).
    *   Execute **Mint Read Key** (if $k$ is a Read Key).
    *   If the operation blocks, the context waits on that specific repository.
4.  **Execute:** Execute the block body with the held Keys.
5.  **Return:** Iterate through $k \in K_{req}$ in **Reverse Canonical Order**:
    *   Execute **Return Write Key** or **Surrender Read Key** as appropriate.

---

### 14.10 Memory Ordering

##### Definition

Key operations use **Acquire/Release** semantics. Data access uses **Sequential Consistency** by default.

##### Syntax & Declaration

Attributes `[[relaxed]]`, `[[acquire]]`, `[[release]]` may be applied to individual expressions to weaken ordering requirements.

**Diagnostic Table**

| Code         | Severity | Condition                                            | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------------- | :----------- | :-------- |
| `E-KEY-0096` | Error    | Memory ordering annotation inside speculative block. | Compile-time | Rejection |

### 14.11 Speculative Execution

##### Definition

**Speculative Execution** allows a block of code to execute optimistically without acquiring blocking locks. The implementation buffers writes and attempts to atomically commit them only if the read set has not changed.

##### Syntax & Declaration

**Grammar**

The `speculative` modifier appears in `#` blocks.

```ebnf
key_block ::= "#" path_list "speculative" "write" block
```

##### Static Semantics

**Restrictions**

A speculative block is **ill-formed** if its body contains:
1.  **Side Effects:** Writes to paths NOT listed in the key block (`E-KEY-0091`).
2.  **Nested Keys:** Any nested `#` block (`E-KEY-0090`).
3.  **Synchronization:** `wait`, `yield`, or `defer` (`E-KEY-0092`, `E-KEY-0093`).
4.  **Ordering:** Explicit memory ordering annotations (`[[relaxed]]`, etc.) (`E-KEY-0096`).

**Typing**

The block has the type of its body expression.

##### Dynamic Semantics

**Execution Model**

Evaluation of `#path speculative write { body }` proceeds as follows:

1.  **Snapshot**: Atomically read the current values of all keyed paths into a local read-set $R$.
    *   For primitives: Atomic load.
    *   For composites: Implementation-defined snapshot (e.g., optimistic read loop or seqlock).
2.  **Execute**: Evaluate `body`.
    *   Redirect reads of keyed paths: check local write-set $W$, then $R$.
    *   Buffer writes to keyed paths into local write-set $W$.
    *   If panic occurs: Discard $W$ and propagate panic.
3.  **Validate & Commit**: Atomically verify that current memory matches $R$.
    *   If match: Apply $W$ to memory. Return result.
    *   If mismatch: Discard $W$. Increment retry counter.
4.  **Retry Strategy**:
    *   If retries < Implementation Limit: Repeat from Step 1.

**Fallback Protocol**
If the retry limit is exceeded, the implementation MUST execute the **Canonical Fallback**:
1.  **Sort:** Establish the Canonical Order (§14.7) for all paths in the read-set $R$ and write-set $W$ of the block.
2.  **Acquire:** Acquire **Write Keys** for all paths in $R \cup W$ in Canonical Order. This operation is blocking.
3.  **Execute:** Re-evaluate the `body` (or apply the buffered write-set $W$ if the implementation can prove atomicity between the last validation and lock acquisition).
4.  **Release:** Release all keys in Reverse Canonical Order.

**Memory Ordering**

*   Snapshot reads use **Acquire** semantics.
*   Successful commit uses **Release** semantics.
*   Failed commit provides no ordering guarantees.

**Snapshot Granularity**
| Data Type             | Permitted Snapshot Mechanism                    |
| :-------------------- | :---------------------------------------------- |
| Primitive (≤ 8 bytes) | Atomic load                                     |
| Small struct          | Seqlock read or atomic load of version + fields |
| Large struct          | Copy under brief lock or seqlock                |

**Commit Atomicity**
| Data Type                  | Permitted Commit Mechanism           |
| :------------------------- | :----------------------------------- |
| Single primitive           | Compare-and-swap                     |
| Single pointer-sized value | Compare-and-swap                     |
| Two pointer-sized values   | Double-word CAS (DCAS) or lock-based |
| Struct with version field  | CAS on version + write fields        |
| Large struct               | Acquire lock, verify, write, release |


**Panic Safety**

If a panic occurs during the **Execute** phase, the write-set $W$ is discarded. No shared state is modified.

---

## Clause 15: Structured Parallelism

### 15.1 Foundational Principles

##### Definition

**Structured Parallelism** is a concurrency model where the lifetime of every concurrent operation is strictly bounded by the lexical scope of its parent construct.

A **Parallel Block** is a lexical scope that coordinates the execution of multiple **Work Items**.

A **Work Item** is a unit of computation submitted for concurrent execution. Work items are created by `spawn` expressions and `dispatch` expressions.

A **Worker** is an execution resource (such as an OS thread or hardware compute unit) provided by an Execution Domain to execute work items.

**Formal Definition**

Let $P$ be a parallel block execution instance. Let $\mathcal{W}_P$ be the set of all work items created within $P$. The **Structured Concurrency Invariant** requires that the lifetime of every work item is strictly contained within the execution lifetime of the parallel block:

$$\forall w \in \mathcal{W}_P.\ \text{Lifetime}(w) \subset \text{Lifetime}(P)$$

This invariant guarantees that when control flow exits a parallel block, all work items associated with that block have terminated (completed, failed, or cancelled).

##### Static Semantics

**Capture Safety**

Because the lifetime of the parallel block strictly exceeds the lifetime of all internal work items, bindings captured by reference from scopes enclosing the parallel block are guaranteed to remain valid during the execution of all work items.

---

### 15.2 Execution Domains

##### Definition

An **Execution Domain** is an abstraction representing a pool of computational resources capable of executing work items.

##### Syntax & Declaration

**Class Declaration**

The `ExecutionDomain` class defines the interface that domain capabilities MUST implement.

```cursive
class ExecutionDomain {
    /// Returns the human-readable identifier of the domain.
    procedure name(~) -> string

    /// Returns the maximum level of concurrency supported.
    procedure max_concurrency(~) -> usize
}
```

##### Static Semantics

**Domain Categories**

Implementations MUST support the following domain categories, provided via the root capability context:

1.  **CPU Domain**: Executes work items on host CPU threads.
2.  **GPU Domain**: Executes work items on graphics processing units.
3.  **Inline Domain**: Executes work items sequentially on the submitting thread.

**GPU Domain Constraints**

Code executing within a GPU domain is subject to the following constraints:

1.  **Memory Isolation**: Access to host memory, including `shared` captures and heap allocations not explicitly managed for device access, is ill-formed (`E-PAR-0050`).
2.  **Capability Restriction**: Use of runtime capabilities (e.g., file system, network) is ill-formed.
3.  **Nesting Prohibition**: A parallel block targeting a GPU domain MUST NOT contain nested GPU parallel blocks (`E-PAR-0052`).

**GPU Intrinsics**

The following intrinsic procedures are available ONLY within parallel blocks targeting a GPU domain:

| Intrinsic         | Signature                                   | Semantics                         |
| :---------------- | :------------------------------------------ | :-------------------------------- |
| `gpu::global_id`  | `procedure(dim: usize) -> usize`            | Global thread index in dimension. |
| `gpu::local_id`   | `procedure(dim: usize) -> usize`            | Workgroup-local index.            |
| `gpu::barrier`    | `procedure()`                               | Workgroup execution barrier.      |
| `gpu::alloc`      | `procedure<T>(count: usize) -> GpuBuf<T>`   | Allocate device-local memory.     |
| `gpu::atomic_add` | `procedure(ptr: *mut u32, val: u32) -> u32` | Atomic add; returns old value.    |
| `gpu::upload`     | `procedure<T>(dst: GpuBuf<T>, src: [T])`    | Copy host to device.              |
| `gpu::download`   | `procedure<T>(dst: [T], src: GpuBuf<T>)`    | Copy device to host.              |

Usage of these intrinsics outside a GPU parallel block is ill-formed.


##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                       | Detection    | Effect    |
| :----------- | :------- | :------------------------------ | :----------- | :-------- |
| `E-PAR-0050` | Error    | Host memory access in GPU code. | Compile-time | Rejection |
| `E-PAR-0052` | Error    | Nested GPU parallel block.      | Compile-time | Rejection |

---

### 15.3 The Parallel Block

##### Definition

The **`parallel`** block is the syntactic construct that delimits a structured parallelism scope. It accepts an execution domain and executes a body containing work creation expressions.

##### Syntax & Declaration

**Grammar**

```ebnf
parallel_block    ::= "parallel" domain_expr [ block_options ] block

domain_expr       ::= expression

block_options     ::= "[" block_option ( "," block_option )* "]"

block_option      ::= "cancel" ":" expression
                    | "name" ":" string_literal
```

##### Static Semantics

**Typing Rules**

The domain expression MUST evaluate to a type implementing `ExecutionDomain` (or `$ExecutionDomain`).

**(T-Parallel)**
$$\frac{
  \Gamma \vdash D : \text{$ExecutionDomain} \quad
  \Gamma_P = \Gamma[\text{parallel\_context} \mapsto D] \quad
  \Gamma_P \vdash B : T
}{
  \Gamma \vdash \texttt{parallel } D\ \{B\} : T
} \quad \text{(T-Parallel)}$$

The result type $T$ is determined by the result aggregation rules defined in §15.6.

**Scope Context**

The parallel block introduces a **parallel context** into the static environment. `spawn` and `dispatch` expressions checks this context to verify they appear within a valid parallel scope.

##### Dynamic Semantics

**Evaluation**

Evaluation of a parallel block proceeds as follows:

1.  Evaluate the `domain_expr` to obtain the execution domain.
2.  Initialize the worker pool and synchronization primitives.
3.  If the `cancel` option is present, register the `CancelToken`.
4.  Execute the statements in the block body sequentially. `spawn` and `dispatch` expressions enqueue work items but do not block.
5.  **Join Barrier**: Upon reaching the end of the block body, the executing thread blocks until all enqueued work items have terminated (completed or failed).
6.  **Panic Propagation**: If one or more work items panicked, the first panic (determined by completion order) is re-raised in the parallel block's context.
7.  **Result Aggregation**: If no panic occurred, the results of the work items are aggregated and returned.

**Ordering**

The execution order of work items is Unspecified.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-PAR-0002` | Error    | Domain expression does not implement ExecutionDomain. | Compile-time | Rejection |

---

### 15.4 Spawn Expressions

##### Definition

A **`spawn`** expression creates a single work item that executes a block of code asynchronously relative to other work items in the same parallel scope.

##### Syntax & Declaration

**Grammar**

```ebnf
spawn_expr      ::= "spawn" [ spawn_attributes ] block

spawn_attributes ::= "[" spawn_attribute ( "," spawn_attribute )* "]"

spawn_attribute  ::= "name" ":" string_literal
                   | "affinity" ":" expression
                   | "priority" ":" expression
```

##### Static Semantics

**Typing Rules**

A `spawn` expression evaluates to a `SpawnHandle<T>`, where `T` is the type of the spawned block body.

**(T-Spawn)**
$$\frac{
  \Gamma[\text{parallel\_context}] \neq \emptyset \quad
  \Gamma \vdash \text{body} : T
}{
  \Gamma \vdash \texttt{spawn}\ \{\text{body}\} : \text{SpawnHandle}\langle T \rangle
} \quad \text{(T-Spawn)}$$

**Capture Rules**

The body of a `spawn` expression captures bindings from the enclosing scope. Capture semantics follow the rules for closures (§12.5), with the following exception:

*   Bindings with `const` or `shared` permission MAY be captured by reference, as the structured concurrency invariant guarantees the referent outlives the work item.
*   Bindings with `unique` permission MUST be moved into the spawn block using the `move` keyword if exclusive access is required.

**SpawnHandle Type**

The `SpawnHandle<T>` is a modal type representing the state of the spawned work.

```cursive
modal SpawnHandle<T> {
    @Pending { }
    @Ready { value: T }
}
```

##### Dynamic Semantics

**Evaluation**

1.  Capture the environment required by the block body.
2.  Construct a work item containing the code and environment.
3.  Submit the work item to the enclosing parallel block's execution domain.
4.  Return a `SpawnHandle<T>` in the `@Pending` state.

**Completion**

When the work item finishes execution, the `SpawnHandle` transitions to `@Ready`, storing the result value.

##### Constraints & Legality

**Negative Constraints**

A `spawn` expression MUST NOT appear outside of a `parallel` block.

**Diagnostic Table**

| Code         | Severity | Condition                                  | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------- | :----------- | :-------- |
| `E-PAR-0001` | Error    | `spawn` expression outside parallel block. | Compile-time | Rejection |
| `E-PAR-0020` | Error    | Implicit capture of `unique` binding.      | Compile-time | Rejection |

---

### 15.14 The Reactor Capability

##### Definition

The **Reactor** is a system capability responsible for driving asynchronous state machines and managing I/O event notification. It provides the mechanism to block the current thread until a `Future` completes.

##### Syntax & Declaration

```cursive
class Reactor {
    /// Drives a future to completion, blocking the current thread.
    /// Polling occurs on the current thread; I/O events are multiplexed.
    procedure run<T; E>(~, future: Future<T, E>) -> T | E

    /// Registers interest in an I/O handle.
    /// Returns a token used to identify completion events.
    procedure register(~, handle: IoHandle, interest: Interest) -> Token
}
```

##### Static Semantics

**Context Availability**
The reactor is available via `ctx.reactor`. It is a required field of the `Context` record (Appendix H).

**Sync Desugaring**
The `sync e` expression (§16.5.3) desugars to:
$$\texttt{ctx.reactor\sim>run}(e)$$

##### Dynamic Semantics

**Run Semantics**
`reactor~>run(f)` executes the following loop:
1.  Poll `f` (`resume(())`).
2.  If `f` returns `@Completed` or `@Failed`, return the result.
3.  If `f` returns `@Suspended`:
    *   Check for I/O events (e.g., `epoll`, `kqueue`, `IOCP`).
    *   Wake tasks associated with ready events.
    *   Repeat step 1.

---

### 15.5 Dispatch Expressions

##### Definition

A **`dispatch`** expression performs data-parallel iteration over a range. The iterations are partitioned into work items and executed concurrently.

##### Syntax & Declaration

**Grammar**

```ebnf
dispatch_expr   ::= "dispatch" pattern "in" range_expr
                    [ key_clause ]
                    [ dispatch_attributes ]
                    block

key_clause      ::= "key" path_expr key_mode

key_mode        ::= "read" | "write"

dispatch_attributes ::= "[" dispatch_attribute ( "," dispatch_attribute )* "]"

dispatch_attribute ::= "reduce" ":" reduce_op
                     | "ordered"
                     | "chunk" ":" expression

reduce_op       ::= "+" | "*" | "min" | "max" | "and" | "or" | identifier
```

##### Static Semantics

**Typing Rules**

**(T-Dispatch)**
Without reduction, the expression has unit type.
$$\frac{
  \Gamma \vdash \text{range} : \text{Range}\langle I \rangle \quad
  \Gamma, i : I \vdash \text{body} : ()
}{
  \Gamma \vdash \texttt{dispatch } i \texttt{ in range } \{\text{body}\} : ()
} \quad \text{(T-Dispatch)}$$

**(T-Dispatch-Reduce)**
With reduction, the expression evaluates to the result of the reduction.
$$\frac{
  \Gamma \vdash \text{range} : \text{Range}\langle I \rangle \quad
  \Gamma, i : I \vdash \text{body} : T \quad
  \Gamma \vdash \oplus : (T, T) \to T
}{
  \Gamma \vdash \texttt{dispatch } i \texttt{ in range [reduce: } \oplus \texttt{] } \{\text{body}\} : T
} \quad \text{(T-Dispatch-Reduce)}$$

**Key Inference and Disjointness**

The implementation MUST analyze the body to determine the access pattern to `shared` data indexed by the iteration variable.

1.  **Disjointness Guarantee**: If the implementation can prove that accesses $A[i]$ and $A[j]$ are disjoint for all $i \neq j$ in the range, then concurrent execution of iterations is statically safe without runtime synchronization.
2.  **Key Inference**: If no `key` clause is provided, the implementation attempts to infer the key path and mode from the body. If inference is ambiguous, the program is ill-formed (`E-PAR-0041`).

##### Dynamic Semantics

**Evaluation**

1.  Evaluate the range expression to determine the iteration space.
2.  Partition the iteration space into chunks.
3.  Submit chunks as work items to the execution domain.
4.  **Wait**: The `dispatch` expression blocks the current thread until all iterations complete.
5.  **Reduction**: If a reduction operator is specified, partial results from chunks are combined using the operator. The combination order is deterministic based on the iteration order.

**Ordering**

If the `ordered` attribute is present, side effects (such as I/O or `shared` mutation) MUST appear to occur in iteration index order.

##### Constraints & Legality

**Negative Constraints**

1.  A `dispatch` expression MUST NOT appear outside of a `parallel` block.
2.  The body of a `dispatch` expression MUST NOT access data from other iterations (`E-PAR-0042`).
3.  The reduction operator MUST be associative.

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-PAR-0040` | Error    | `dispatch` expression outside parallel block. | Compile-time | Rejection |
| `E-PAR-0041` | Error    | Key inference failed; explicit key required.  | Compile-time | Rejection |
| `E-PAR-0042` | Error    | Cross-iteration dependency detected.          | Compile-time | Rejection |
| `E-PAR-0043` | Error    | Non-associative reduction without `ordered`.  | Compile-time | Rejection |

---

### 15.6 Result Aggregation

##### Definition

The return type of a `parallel` block is determined by the composition of the `spawn` and `dispatch` expressions within its body.

##### Static Semantics

**Result Type Rules**

The type of the parallel block is derived from its statements:

1.  **Empty/Statement-Only**: If the block contains no `spawn` or `dispatch` expressions, or only statements that do not produce results (e.g., `dispatch` without reduction), the type is `()`.
2.  **Single Spawn**: If the block contains exactly one `spawn` expression returning $T$, the block type is $T$.
3.  **Multiple Spawns**: If the block contains multiple `spawn` expressions $S_1, \dots, S_n$ returning types $T_1, \dots, T_n$, the block type is the tuple $(T_1, \dots, T_n)$. The tuple elements correspond to the source order of the `spawn` expressions.
4.  **Reducing Dispatch**: A `dispatch` with reduction returning $T$ contributes $T$ to the result tuple.

**(T-Parallel-Result)**
Let $R_1, \dots, R_k$ be the result types of the work-creating expressions in the block body, in lexical order.
$$\frac{
  \text{Types} = (R_1, \dots, R_k)
}{
  \text{TypeOf}(\text{parallel block}) = \text{Types}
}$$

If $k=1$, the type is $R_1$. If $k=0$, the type is `()`.

##### Dynamic Semantics

**Result Collection**

Upon successful completion of all work items:
1.  The results of all `spawn` expressions are collected.
2.  The results of all reducing `dispatch` expressions are collected.
3.  The values are aggregated into the result structure defined by the static semantics and returned.

---

### 15.7 Panic and Cancellation

##### Definition

Structured parallelism enforces consistent error handling and resource cleanup across concurrent tasks.

##### Dynamic Semantics

**Panic Propagation**

If any work item within a parallel block terminates with a panic:

1.  The panic is captured.
2.  **Cancellation Signal**: The runtime signals cancellation to all other active work items in the same parallel block.
3.  **Settling**: The block waits for all work items to terminate (either by completion, cancellation, or panic).
4.  **Propagation**: Once all work has settled, the initial panic is re-raised in the thread executing the `parallel` block. If multiple work items panicked, the panic that occurred first in time is propagated; others are discarded.

**Cancellation Token**

If a `CancelToken` is provided via the `cancel` option, it is associated with the parallel scope. Work items MAY query this token to detect cancellation requests (initiated by the user or by peer panic).

**Cleanup**

Upon termination of a work item (success or panic), all local bindings are destroyed in reverse declaration order. `defer` blocks are executed. This cleanup MUST complete before the parallel block unblocks.

---

### 15.8 Async Integration

##### Definition

Parallel blocks interact with the asynchronous programming model through `wait` expressions and key isolation.

##### Static Semantics

**Wait Expression**

A `wait` expression MAY appear within a `spawn` or `dispatch` body.

**Key Isolation**

Keys obtained via the Key System (§14) MUST NOT be held across a `wait` point within a parallel block. Violating this constraint is ill-formed (`E-ASYNC-0013`).

##### Dynamic Semantics

**Suspension**

When a work item evaluates `wait future`:
1.  The work item is suspended.
2.  The worker executing the item is released to execute other ready work items.
3.  Upon completion of the future, the work item becomes ready and is rescheduled for execution.

**Re-entrancy**

The `parallel` block construct itself is synchronous; it blocks the calling thread until all work completes. To execute a parallel workload asynchronously, the `parallel` block must be wrapped in an `async` procedure or a `spawn` within an outer parallel context.

---

### 15.9 Cancellation Tokens

##### Definition

A **CancelToken** is a modal type used to coordinate cooperative cancellation of parallel work.

##### Syntax & Declaration

```cursive
modal CancelToken {
    @Active { } {
        /// Returns true if cancellation has been requested.
        procedure is_cancelled(~) -> bool
        
        /// Asynchronously waits for cancellation request.
        procedure wait_cancelled(~) -> Future<()>
        
        /// Creates a child token linked to this one.
        procedure child(~) -> unique CancelToken@Active
        
        /// Requests cancellation.
        procedure cancel(~%)
    }
    
    @Cancelled { } {
        procedure is_cancelled(~) -> bool { true }
    }
}
```

**Semantics**

1.  **Propagation:** Cancelling a parent token automatically cancels all child tokens derived from it.
2.  **Scope:** When a `parallel` block is configured with a token, that token is implicitly available to all spawned tasks.

---

### 15.10 Panic Handling

##### Definition

**Panic Handling** in structured parallelism defines the mechanism for capturing, aggregating, and propagating runtime exceptions (panics) occurring within concurrent work items. The structured concurrency invariant requires that a parallel block MUST NOT exit until all internal work items have settled, ensuring that panics do not escape while work is still pending.

##### Dynamic Semantics

**Panic State Lifecycle**

If one or more work items within a parallel block terminate with a panic, the parallel block execution proceeds through the following phases:

1.  **Capture**: The runtime captures the panic payload and the associated work item identifier.
2.  **Cancellation Signal**: The runtime signals cancellation to all other active work items within the same parallel block.
3.  **Settling**: The parallel block execution blocks until all work items have terminated (via completion, cancellation, or panic).
4.  **Propagation**: Once the block is settled, the runtime propagates a panic to the enclosing scope.

**Multiple Panic Resolution**

If multiple work items panic during execution:

1.  The implementation MUST capture all unique panics.
2.  The implementation MUST propagate the panic corresponding to the work item that terminated first in wall-clock time (or an implementation-defined deterministic ordering).
3.  Secondary panics MAY be logged or discarded but MUST NOT obscure the primary panic.

**Interaction with Cancellation**

When a panic occurs, the implementation SHOULD trigger the cancellation mechanism (§15.8) for peer tasks to expedite the settling phase. This behavior is Implementation-Defined Behavior (IDB) to allow for variation in runtime overhead trade-offs.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection | Effect |
| :----------- | :------- | :--------------------------------------------- | :-------- | :----- |
| `P-PAR-0001` | Panic    | Parallel block aggregation of work item panic. | Runtime   | Panic  |

---

### 15.11 Nested Parallelism

##### Definition

**Nested Parallelism** occurs when a `parallel` block is executed within the scope of an outer `parallel` block.

##### Dynamic Semantics

**Homogeneous Nesting (CPU/CPU)**

When a CPU parallel block nests within another CPU parallel block:
1.  The inner block MUST share the worker pool of the outer block. It MUST NOT create a new thread pool.
2.  The `workers` option on the inner block serves as a logical concurrency limit, not a resource reservation.

**Heterogeneous Nesting (CPU/GPU)**

When a GPU parallel block nests within a CPU parallel block:
1.  The GPU block executes on the specified device.
2.  The CPU task invoking the GPU block blocks (or suspends) until the GPU work completes.

##### Constraints & Legality

**Negative Constraints**

1.  A GPU parallel block MUST NOT contain a nested GPU parallel block.
2.  Capture rules apply independently at each nesting level.

**Diagnostic Table**

| Code         | Severity | Condition                                                | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------------- | :----------- | :-------- |
| `E-PAR-0052` | Error    | Nested GPU parallel block (unsupported dynamic nesting). | Compile-time | Rejection |

---

### 15.12 Determinism

##### Definition

**Determinism** is the property that a parallel program produces identical results and side effects for a given input, regardless of the nondeterministic scheduling of underlying workers.

##### Static Semantics

**Deterministic Dispatch Conditions**

A `dispatch` expression guarantees deterministic results if and only if:
1.  The key partitioning algorithm is deterministic.
2.  Iterations mapped to the same key execute in strictly increasing index order.
3.  The reduction operator (if present) is associative and applied in a deterministic tree order.

**Ordered Dispatch**

The `[ordered]` attribute enforces sequential ordering of observable side effects.

1.  **Execution**: Iterations MAY execute concurrently.
2.  **Commit**: Side effects (I/O, shared mutation via `write` keys) MUST become visible in iteration index order.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-PAR-0043` | Error    | Non-associative reduction without `[ordered]`. | Compile-time | Rejection |

---

### 15.13 Memory Allocation in Parallel Blocks

##### Definition

Work items within parallel blocks may perform memory allocation using captured allocator capabilities or region-based allocation.

##### Dynamic Semantics

**Captured Allocators**

Work items MAY allocate memory using a captured `$HeapAllocator` capability. The allocator implementation MUST be thread-safe if the capability is captured with `shared` permission.

**Region Allocation**

Work items MAY allocate memory in a region $R$ defined in an enclosing scope if the region handle is captured.

1.  **LIFO Safety**: The Structured Concurrency Invariant (§15.1) guarantees that the parallel block exits (and thus all work items terminate) before the region $R$ exits.
2.  **Thread Safety**: If region allocation is not thread-safe, the region handle MUST be captured with `unique` permission (limiting allocation to a single task) or protected by a key.

**Region-Local Parallelism**

A parallel block MAY be nested within a region. Objects allocated in that region are accessible to work items subject to standard capture and permission rules.

```cursive
region work_arena {
    parallel ctx.cpu() {
        dispatch i in 0..n {
            // Valid: allocating in enclosing region
            let temp = ^work_arena compute_result(i)
            process(temp)
        }
    }
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-MEM-3021` | Error    | Region allocation outside region scope. | Compile-time | Rejection |
| `E-KEY-0001` | Error    | Unsynchronized access to shared region. | Compile-time | Rejection |

---

## Clause 16: Asynchronous Operations

### 16.1 The Async Modal Class

##### Definition

The **Async class** (`Async<Out, In, Result, E>`) is a modal class that defines the interface for asynchronous state machines. It abstracts over computations that yield intermediate values, accept input upon resumption, produce a final result, or terminate with an error.

**Formal Definition**

The `Async` class is defined as the tuple $(N, G, S, P_{abs}, P_{con}, A_{abs}, A_{con}, F, St)$ where:
- $N = \texttt{Async}$
- $G = \langle \texttt{Out}, \texttt{In}, \texttt{Result}, \texttt{E} \rangle$
- $St = \{ \texttt{@Suspended}, \texttt{@Completed}, \texttt{@Failed} \}$
- $P_{abs} = \{ \texttt{resume} \}$

A type $T$ implements `Async` if and only if $T$ is a modal type providing the required states and implementing the `resume` procedure.

##### Syntax & Declaration

**Class Declaration**

```cursive
class Async<Out; In = (); Result = (); E = !> {
    @Suspended {
        output: Out,
    }

    @Completed {
        value: Result,
    }

    @Failed {
        error: E,
    }

    procedure resume(~!, input: In) -> Async@Suspended | Async@Completed | Async@Failed;
}
```

##### Static Semantics

**State Semantics**

| State        | Condition                                        | Payload Requirements |
| :----------- | :----------------------------------------------- | :------------------- |
| `@Suspended` | The computation is paused and has yielded output | `output: Out`        |
| `@Completed` | The computation has finished successfully        | `value: Result`      |
| `@Failed`    | The computation has terminated with an error     | `error: E`           |

**Uninhabited Error Type**

When the error type argument `E` is the never type `!`, the `@Failed` state is uninhabited. Implementing types MAY omit the `@Failed` state per the uninhabited state omission rule (§9.3).

**Subtyping and Variance**

The `Async` class is covariant in `Out` and `Result`, contravariant in `In`, and covariant in `E`.

$$\frac{
    \Gamma \vdash Out_A <: Out_B \quad
    \Gamma \vdash In_B <: In_A \quad
    \Gamma \vdash Res_A <: Res_B \quad
    \Gamma \vdash E_A <: E_B
}{
    \Gamma \vdash \texttt{Async}\langle Out_A, In_A, Res_A, E_A \rangle <: \texttt{Async}\langle Out_B, In_B, Res_B, E_B \rangle
} \quad \text{(Sub-Async)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                 | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0001` | Error    | `Async` type parameter is not well-formed | Compile-time | Rejection |

---

### 16.2 Common Async Patterns

##### Definition

Standard asynchronous patterns are defined as type aliases to the `Async` class. These aliases are normative and MUST be provided by the implementation.

##### Syntax & Declaration

```cursive
type Sequence<T>      = Async<T, (), (), !>
type Future<T; E = !> = Async<(), (), T, E>
type Pipe<In; Out>    = Async<Out, In, (), !>
type Exchange<T>      = Async<T, T, T, !>
type Stream<T; E>     = Async<T, (), (), E>
```

##### Static Semantics

**Type Equivalence**

These aliases are structurally equivalent to their expansions per §4.1 (T-Equiv-Nominal).

$$\Gamma \vdash \texttt{Future}\langle T, E \rangle \equiv \texttt{Async}\langle (), (), T, E \rangle$$

All typing rules and constraints applicable to `Async` apply identically to these aliases.

---

### 16.3 Async Procedures

##### Definition

An **async procedure** is a procedure that declares `Async` (or an alias thereof) as its return type. The implementation of an async procedure is transformed into a generated modal type implementing the declared `Async` interface.

##### Syntax & Declaration

**Grammar**

```ebnf
async_procedure ::= procedure_decl
```

*(Note: No distinct syntax distinguishes async procedures; the return type is authoritative.)*

##### Static Semantics

**Generated Modal Type**

For every procedure $f$ with return type $R = \texttt{Async}\langle Out, In, Res, E \rangle$, the implementation MUST generate an anonymous modal type $M_f$ such that:

1.  $M_f <: R$
2.  $M_f$ declares states `@Suspended`, `@Completed`, and (if $E \neq !$) `@Failed`.
3.  $M_f$ states contain all payload fields required by $R$.
4.  $M_f$ states contain implementation-defined fields sufficient to store the resumption point (`gen_point`) and all captured local bindings live across suspension points.

**Procedure Type Derivation**

The type of the procedure $f$ is a function type returning the generated modal $M_f$.

$$\frac{
    \Gamma \vdash \text{body} : Res \quad
    \text{ReturnType}(f) = \texttt{Async}\langle Out, In, Res, E \rangle
}{
    \Gamma \vdash f : (\dots) \to M_f
} \quad \text{(T-Async-Proc)}$$

**Body Transformation**

The body of the procedure is transformed into the implementation of the `resume` transition for $M_f$.
-   **Entry:** The initial call to $f$ constructs $M_f$ in its initial state (executing until the first suspension or completion).
-   **Suspension:** `yield` expressions become return points returning $M_f@Suspended$.
-   **Completion:** `result` statements (or fallthrough) become return points returning $M_f@Completed$.
-   **Failure:** `?` operator propagation becomes return points returning $M_f@Failed$.

##### Dynamic Semantics

**Execution**

1.  Calling $f(\dots)$ allocates and initializes $M_f$.
2.  Execution proceeds synchronously until the first `yield`, `result`, or error.
3.  The call returns the $M_f$ value in the corresponding state.
4.  Subsequent calls to $M_f\texttt{\~>resume}(input)$ restore local state and jump to the resumption point defined by `gen_point`.

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                       | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0002` | Error    | `yield` used in non-async procedure             | Compile-time | Rejection |
| `E-ASYNC-0030` | Error    | Error propagation in infallible async procedure | Compile-time | Rejection |

---

### 16.4 Suspension Expressions

#### 16.4.1 The Yield Expression

##### Definition

The **yield expression** suspends the asynchronous computation, emitting an output value and awaiting an input value.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_expr ::= "yield" [ "release" ] expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \text{ReturnType}(\text{ctx}) = \texttt{Async}\langle Out, In, \_, \_ \rangle \quad
    \Gamma \vdash e : Out
}{
    \Gamma \vdash \texttt{yield } [\texttt{release}] \ e : In
} \quad \text{(T-Yield)}$$

The expression $e$ MUST be a subtype of the `Out` parameter of the enclosing async return type. The `yield` expression evaluates to the `In` parameter type.

**Key Holding Constraint**

If the `release` keyword is absent, `yield` MUST NOT appear within a scope where any keys are logically held (§14).

**(K-Yield-No-Keys)**
$$\frac{
    \Gamma_{\text{keys}} \neq \emptyset \quad
    \texttt{release} \notin \text{modifiers}
}{
    \text{Emit}(\texttt{E-ASYNC-0013})
}$$

##### Dynamic Semantics

**Evaluation**

1.  Evaluate $e$ to value $v$.
2.  If `release` is present: execute **Key Release** sequence (§14.7).
3.  Update $M_f$ state to `@Suspended` with `output: v` and `gen_point` set to the resume address.
4.  Return $M_f$ to the caller/resumer.
5.  **On Resumption:**
    -   Receive `input` from `resume(input)`.
    -   If `release` is present: execute **Key Reacquire** sequence (§14.7).
    -   The `yield` expression evaluates to `input`.

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                      | Detection    | Effect    |
| :------------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0010` | Error    | `yield` outside async-returning procedure      | Compile-time | Rejection |
| `E-ASYNC-0011` | Error    | `yield` operand type incompatible with `Out`   | Compile-time | Rejection |
| `E-ASYNC-0013` | Error    | `yield` while keys held (without `release`)    | Compile-time | Rejection |
| `W-KEY-0011`   | Warning  | Potential stale binding access after `release` | Compile-time | Warning   |

---

#### 16.4.2 The Yield From Expression

##### Definition

The **yield from expression** delegates execution to a sub-async computation, forwarding outputs and inputs until the sub-computation completes.

##### Syntax & Declaration

**Grammar**

```ebnf
yield_from_expr ::= "yield" [ "release" ] "from" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \text{ReturnType}(\text{ctx}) = \texttt{Async}\langle Out, In, \_, E_{ctx} \rangle \quad
    \Gamma \vdash e : \texttt{Async}\langle Out, In, Res_{sub}, E_{sub} \rangle \quad
    E_{sub} <: E_{ctx}
}{
    \Gamma \vdash \texttt{yield } [\texttt{release}] \texttt{ from } e : Res_{sub}
} \quad \text{(T-Yield-From)}$$

The sub-computation $e$ MUST have `Out` and `In` types identical to the enclosing procedure, and an error type compatible with the enclosing procedure.

**Key Holding Constraint**

As with `yield`, `yield from` without `release` MUST NOT occur while keys are held.

##### Dynamic Semantics

**Evaluation**

1.  Evaluate $e$ to async value $A$.
2.  **Loop:**
    -   Match state of $A$:
        -   `@Suspended { output: o }`:
            -   Perform `yield o` (including Release/Reacquire logic if `release` specified).
            -   On resumption with `input`, $A \leftarrow A\texttt{\~>resume}(input)$.
        -   `@Completed { value: v }`:
            -   Terminate loop. Result is $v$.
        -   `@Failed { error: err }`:
            -   Propagate `err` (transition enclosing to `@Failed`).

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                        | Detection    | Effect    |
| :------------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0020` | Error    | `yield from` outside async procedure             | Compile-time | Rejection |
| `E-ASYNC-0021` | Error    | Incompatible `Out`/`In` types in delegation      | Compile-time | Rejection |
| `E-ASYNC-0024` | Error    | `yield from` while keys held (without `release`) | Compile-time | Rejection |

---

### 16.5 Consumption Expressions

#### 16.5.1 The Sync Expression

##### Definition

The **sync expression** drives a `Future` (async with unit input/output) to completion synchronously using the system reactor.

##### Syntax & Declaration

**Grammar**

```ebnf
sync_expr ::= "sync" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle (), (), Res, E \rangle
}{
    \Gamma \vdash \texttt{sync } e : Res \mid E
} \quad \text{(T-Sync)}$$

**Context Restriction**

`sync` MUST NOT appear within an async-returning procedure. (Blocking the thread inside an async state machine defeats the purpose of asynchrony).

##### Dynamic Semantics

**Evaluation**

1.  Evaluate $e$ to async value $A$.
2.  Invoke `ctx.reactor~>run(A)`.
3.  The reactor polls $A$ and handles I/O events until $A$ reaches `@Completed` or `@Failed`.
4.  Return the result or propagate the error.

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                 | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0050` | Error    | `sync` used inside async procedure        | Compile-time | Rejection |
| `E-ASYNC-0051` | Error    | `sync` operand has non-unit `Out` or `In` | Compile-time | Rejection |

---

#### 16.5.2 Async Iteration

##### Definition

The `loop ... in` statement iterates over an async value that produces a stream of outputs (where `In = ()`).

##### Syntax & Declaration

**Grammar**

```ebnf
async_loop ::= "loop" pattern "in" expression block
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma \vdash e : \texttt{Async}\langle Out, (), \_, \_ \rangle \quad
    \Gamma \vdash p : Out \Rightarrow \Gamma_{body} \quad
    \Gamma_{body} \vdash \text{block} : ()
}{
    \Gamma \vdash \texttt{loop } p \texttt{ in } e \ \{ \text{block} \} : ()
} \quad \text{(T-Async-Loop)}$$

The async value MUST accept `()` as input.

##### Dynamic Semantics

**Desugaring**

The construct desugars to a loop matching on the async state:

```cursive
var _a = expression;
loop {
    match _a {
        @Suspended { output: o } => {
            let pattern = o;
            block;
            _a = _a~>resume(());
        }
        @Completed { .. } => break,
        @Failed { error: e } => panic(e),
    }
}
```

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                      | Detection    | Effect    |
| :------------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0040` | Error    | Async iteration operand requires non-unit `In` | Compile-time | Rejection |

---

### 16.6 Concurrent Composition

#### 16.6.1 The Race Expression

##### Definition

The **race expression** executes multiple async computations concurrently, completing when the first operand completes (or fails).

##### Syntax & Declaration

**Grammar**

```ebnf
race_expr ::= "race" "{" race_arm ("," race_arm)* [","] "}"
race_arm  ::= expression "->" "|" pattern "|" expression
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \forall i.\ \Gamma \vdash e_i : \texttt{Future}\langle T_i, E_i \rangle \quad
    \forall i.\ \Gamma, p_i : T_i \vdash \text{handler}_i : R
}{
    \Gamma \vdash \texttt{race } \{ \dots \} : R \mid (\bigcup E_i)
} \quad \text{(T-Race)}$$

1.  Operands MUST be `Future`s (unit Out/In).
2.  Handler expressions MUST unify to a single return type $R$.

##### Dynamic Semantics

**Evaluation**

1.  Initialize all $e_i$.
2.  Poll all $e_i$ via the reactor.
3.  When any $e_i$ transitions to `@Completed` or `@Failed`:
    -   If `@Completed`: Bind result to $p_i$, execute $\text{handler}_i$.
    -   If `@Failed`: Propagate error immediately (unless handled).
    -   **Cancel** all other in-flight operations (drop them).
4.  Return the result of the handler (or error).

---

#### 16.6.2 The All Expression

##### Definition

The **all expression** executes multiple async computations concurrently, completing only when **all** operands succeed or **any** operand fails.

##### Syntax & Declaration

**Grammar**

```ebnf
all_expr ::= "all" "{" expression ("," expression)* [","] "}"
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \forall i.\ \Gamma \vdash e_i : \texttt{Future}\langle T_i, E_i \rangle
}{
    \Gamma \vdash \texttt{all } \{ e_1, \dots, e_n \} : (T_1, \dots, T_n) \mid (\bigcup E_i)
} \quad \text{(T-All)}$$

##### Dynamic Semantics

**Evaluation**

1.  Initialize all $e_i$.
2.  Poll all $e_i$ via the reactor.
3.  If any $e_i$ transitions to `@Failed`:
    -   Cancel all other operations.
    -   Propagate the error.
4.  If all $e_i$ transition to `@Completed`:
    -   Construct tuple $(v_1, \dots, v_n)$.
    -   Return tuple.

---

#### 16.6.3 Condition Waiting (`until`)

##### Definition

The **`until`** method allows asynchronous tasks to suspend until a specific predicate on `shared` data becomes true.

##### Syntax & Declaration

Every type `T` used in a `shared` binding implicitly provides:

```cursive
procedure until<R>(
    self: shared T,
    pred: procedure(const T) -> bool,
    action: procedure(unique T) -> R
) -> Future<R>
```

##### Dynamic Semantics

**Evaluation**

1.  **Check:** Acquire Read key. Evaluate `pred(self)`.
2.  **Fast Path:** If `true`, release Read, acquire Write, execute `action(self)`, return result.
3.  **Suspend:** If `false`, release Read. Register a **Waiter** on the path of `self`. Return `@Suspended`.

**Wakeup Protocol**

1.  **Trigger:** When any Write key covering the path of `self` (or a prefix) is **released**.
2.  **Verify:** The runtime wakes the waiter.
3.  **Re-Check:** Waiter acquires Read key, re-evaluates `pred(self)`.
    *   If `true`: Proceed to Fast Path (Acquire Write, Action, Complete).
    *   If `false`: Release Read, suspend again.

**Ordering:** Waiters observe mutations in an order consistent with the Key System's acquisition order.

---

### 16.7 Combinators

##### Definition

The `Async` class MUST provide the following combinator methods. These methods return generated modal types implementing `Async` that wrap the source computation.

##### Static Semantics

**Signatures**

1.  `map<U>(f: Out -> U) -> Async<U, In, Res, E>`
    -   Applies $f$ to each yielded output.
2.  `filter(pred: const Out -> bool) -> Async<Out, In, Res, E>`
    -   Yields output only if $pred$ returns true.
3.  `chain<U>(f: Res -> Future<U, E>) -> Future<U, E>`
    -   When source completes, executes $f(\text{result})$ and delegates to the returned future.

**Typing**

Combinators are generic in the `Async` type parameters. The returned type is a concrete generated modal unique to the combinator invocation.

---

### 16.8 Memory Layout and Generation

##### Definition

This section defines the mandatory memory layout properties for generated async modal types.

##### Memory & Layout

**Generated Structure**

The generated modal type $M_f$ for an async procedure MUST contain:

1.  **Discriminant**: A field distinguishing `@Suspended`, `@Completed`, and `@Failed`.
2.  **Resumption Point**: An integer field tracking the CFG node index for resumption.
3.  **Captures**: Fields for all arguments and local variables whose live ranges span a suspension point.
4.  **Outputs**: Storage for `Out`, `Result`, and `E` as required by the state payloads.

**Size and Alignment**

$$\text{sizeof}(M_f) = \text{sizeof}(\text{Discriminant}) + \max(\text{sizeof}(@\text{Suspended}), \text{sizeof}(@\text{Completed}), \text{sizeof}(@\text{Failed}))$$

Layout uses the standard `enum` layout algorithm (§5.4) with Niche Optimization (§5.4) applied where possible.

---

### 16.9 Cancellation and Cleanup

##### Definition

**Cancellation** is the premature termination of an asynchronous operation before it reaches the `@Completed` or `@Failed` state. Cancellation in Cursive is **drop-based**: releasing the handle to an `Async` value implicitly requests cancellation of the underlying computation.

##### Dynamic Semantics

**Drop-Based Cancellation**

When an `Async` value is dropped while in the `@Suspended` state, the implementation MUST perform the following **Cancellation Sequence**:

1.  **Suspension Check**: If the operation is currently suspended at a `yield` point, execution resumes immediately at that point.
2.  **Control Flow Injection**: The implementation injects a control flow transfer to the nearest enclosing cleanup scope.
3.  **Cleanup Execution**: `defer` blocks registered within the async procedure are executed in reverse declaration order (LIFO).
4.  **Resource Destruction**: Destructors (`Drop::drop`) are invoked for all live bindings captured by or local to the async procedure.
5.  **Termination**: The async state transitions to a terminal state (effectively destroyed).

**In-Flight I/O Behavior**

The behavior of pending I/O operations (e.g., socket reads, file writes) upon cancellation is **Implementation-Defined Behavior (IDB)**.

Conforming implementations MUST choose one of the following strategies and document it in the Conformance Dossier:
1.  **Immediate Cancellation**: The I/O request is aborted at the OS level.
2.  **Completion-then-Drop**: The I/O request completes in the background, but the result is discarded.
3.  **Handle Closure**: The underlying resource handle is closed to force termination.

##### Constraints & Legality

**Negative Constraints**

The cancellation mechanism MUST NOT bypass `defer` blocks or destructors.

---

### 16.10 Error Handling

##### Definition

**Error Handling** in asynchronous operations manages the propagation of failure states through the `Async` state machine. Errors are propagated via the `?` operator or explicit return, transitioning the state machine to `@Failed`.

##### Static Semantics

**Propagation Typing (T-Async-Try)**

The `?` operator is well-typed within an async procedure if the error component of the operand is a subtype of the procedure's declared error type `E`.

$$\frac{
    \text{ReturnType}(\text{ctx}) = \texttt{Async}\langle \_, \_, \_, E_{ctx} \rangle \quad
    \Gamma \vdash e : T \mid E_{expr} \quad
    E_{expr} <: E_{ctx}
}{
    \Gamma \vdash e? : T
} \quad \text{(T-Async-Try)}$$

**Infallibility Constraint**

If an async procedure declares its error type as the never type `!` (infallible), the body MUST NOT contain:
1.  Usage of the `?` operator on fallible expressions.
2.  Explicit construction of the `@Failed` state.
3.  Calls to procedures returning `Result<T, E>` without handling the error.

##### Dynamic Semantics

**Failure Sequence**

When an error propagates (via `?` or explicit `result` of error type):

1.  **Capture**: The error value is captured into the `@Failed` state payload.
2.  **Cleanup**: The **Cancellation Sequence** (§16.9) is executed to clean up local resources.
3.  **Transition**: The async value transitions to the `@Failed` state.
4.  **Notification**: The caller/resumer is notified of the state change.

##### Constraints & Legality

**Diagnostic Table**

| Code           | Severity | Condition                                       | Detection    | Effect    |
| :------------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-ASYNC-0030` | Error    | Error propagation in infallible async procedure | Compile-time | Rejection |

---

### 16.11 Integration with Other Language Features

#### 16.11.3 Parallel Block Composition

##### Definition

**Parallel Block Composition** defines the interaction between the asynchronous model (`Async`) and the structured parallelism model (`parallel`). Async operations may be spawned as concurrent work items, and parallel blocks may suspend on async conditions.

##### Static Semantics

**Async in Spawn**

A `spawn` block body MAY contain async expressions. If a spawned task evaluates a `yield` expression (or `yield from`):
1.  The task suspends execution.
2.  The worker thread is released to the execution domain.
3.  The task is rescheduled when the resumption condition is met.

**Async in Dispatch**

A `dispatch` loop MAY iterate over an async sequence (`Stream` or `Sequence`).
1.  The iteration source MUST implement `Async<T, (), (), E>`.
2.  Each yielded value becomes a work item.

##### Dynamic Semantics

**I/O Integration**

When a spawned task awaits an I/O future (via `yield from` or `sync`):
1.  The task registers interest with the `Reactor`.
2.  The task suspends.
3.  The `ExecutionDomain` scheduler reclaims the worker.
4.  Upon I/O readiness, the `Reactor` notifies the domain, and the task is re-queued.

**Structured Concurrency Invariant**

The Structured Concurrency Invariant (§15.1) applies to async tasks spawned within a parallel block. The parallel block MUST NOT exit until all spawned async tasks have reached `@Completed`, `@Failed`, or have been cancelled.

**Block Exit Sequence:**
1.  Wait for all active tasks to settle.
2.  If the block is exiting due to panic or cancellation, cancel all pending async tasks.
3.  Run cleanup for all tasks.
4.  Return aggregated results or propagate panic.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-PAR-0060` | Error    | Async task escapes parallel block scope. | Compile-time | Rejection |

---

## Clause 17: Compile-Time Execution

### 17.1 The Comptime Environment

##### Definition

**Compile-time execution** is the evaluation of Cursive code during the Metaprogramming Phase of translation, occurring after parsing and prior to full semantic analysis. This execution operates within a sandboxed **comptime environment** ($\Gamma_{ct}$) that is distinct from the runtime environment ($\Gamma_{rt}$).

**Formal Definition**

The comptime environment is defined as a tuple:

$$\Gamma_{ct} ::= (\Sigma_{stdlib}, \Sigma_{imports}, \Sigma_{types}, \Sigma_{caps}, \emptyset)$$

where:
- $\Sigma_{stdlib}$ is the subset of the standard library available at compile time.
- $\Sigma_{imports}$ is the set of modules imported via `import` declarations.
- $\Sigma_{types}$ is the set of type definitions visible at the point of execution.
- $\Sigma_{caps}$ is the set of compile-time capabilities provided to the current context.
- $\emptyset$ denotes the empty set of shared mutable state; $\Gamma_{ct}$ shares no state with $\Gamma_{rt}$.

##### Static Semantics

**Allowlist Mandate**
The `comptime` environment is a strict whitelist environment. Code executing in $\Gamma_{ct}$ MUST NOT access any standard library module unless explicitly marked [[comptime_visible]].
The following standard modules are **Explicitly Prohibited** in $\Gamma_{ct}$:
- std::time (System clocks)
- std::random (Non-deterministic entropy)
- std::thread (Runtime threading)
- std::process (Environment variables and arguments)
- std::fs (File system access is permitted ONLY via the ProjectFiles capability).

**Determinism Property**

Comptime execution MUST be deterministic. For any expression $e$ evaluated in $\Gamma_{ct}$ yielding result $v$, re-evaluation in an identical environment MUST yield an identical result $v$.

$$\forall e, \Gamma_{ct}.\ (\Gamma_{ct} \vdash e \Downarrow v_1) \land (\Gamma_{ct} \vdash e \Downarrow v_2) \implies v_1 = v_2$$

**Termination Property**

Comptime execution MUST terminate. Implementations MUST enforce the resource limits defined in §17.8 to guarantee termination.

**Purity Requirement**

Expressions evaluated in $\Gamma_{ct}$ MUST be **pure** as defined in §11.1.1, with the following exceptions enabled by specific capabilities:
1. Operations authorized by `TypeEmitter` (§17.5.1).
2. Operations authorized by `ProjectFiles` (§17.5.3).
3. Operations authorized by `ComptimeDiagnostics` (§17.5.4).

##### Constraints & Legality

**Negative Constraints**

The following constructs MUST NOT appear in code evaluated within the comptime environment:
1. `unsafe` blocks.
2. Calls to `extern` procedures.
3. Expressions accessing runtime capabilities.
4. Non-deterministic operations (e.g., random number generation, system time).

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-CTE-0001` | Error    | `unsafe` block in comptime context.              | Compile-time | Rejection |
| `E-CTE-0002` | Error    | FFI call in comptime context.                    | Compile-time | Rejection |
| `E-CTE-0003` | Error    | Runtime capability access in comptime context.   | Compile-time | Rejection |
| `E-CTE-0004` | Error    | Non-deterministic operation in comptime context. | Compile-time | Rejection |
| `E-CTE-0005` | Error    | Prohibited I/O operation in comptime context.    | Compile-time | Rejection |

---

### 17.2 Comptime-Available Types

##### Definition

A **comptime-available type** is a type whose values are representable within the comptime environment. Only values of comptime-available types may be computed, stored, or passed as arguments during the Metaprogramming Phase.

**Formal Definition**

The predicate $\text{IsComptimeAvailable}(T)$ holds if and only if $T \in \mathcal{T}_{ct}$, defined inductively:

$$\mathcal{T}_{ct} ::= \text{Primitive} \mid \texttt{string} \mid \texttt{Type} \mid \texttt{Ast} \mid \text{Tuple}(\mathcal{T}_{ct}^*) \mid \text{Array}(\mathcal{T}_{ct}) \mid \text{Record}_{ct} \mid \text{Enum}_{ct}$$

where $\text{Record}_{ct}$ and $\text{Enum}_{ct}$ are types whose constituent fields or payloads are in $\mathcal{T}_{ct}$.

##### Static Semantics

**Typing Rules**

The following inference rules define the set $\mathcal{T}_{ct}$:

**(CT-Prim)**
$$\frac{T \in \{\texttt{bool}, \texttt{char}\} \cup \mathcal{T}_{\text{int}} \cup \mathcal{T}_{\text{float}}}{\text{IsComptimeAvailable}(T)}$$

**(CT-String)**
$$\frac{}{\text{IsComptimeAvailable}(\texttt{string})}$$

**(CT-Meta)**
$$\frac{T \in \{\texttt{Type}, \texttt{Ast}\}}{\text{IsComptimeAvailable}(T)}$$

**(CT-Tuple)**
$$\frac{\forall i \in 1..n.\ \text{IsComptimeAvailable}(T_i)}{\text{IsComptimeAvailable}((T_1, \ldots, T_n))}$$

**(CT-Array)**
$$\frac{\text{IsComptimeAvailable}(T)}{\text{IsComptimeAvailable}([T; n])}$$

**(CT-Record)**
$$\frac{R = \texttt{record} \{ f_1: T_1, \ldots, f_n: T_n \} \quad \forall i.\ \text{IsComptimeAvailable}(T_i)}{\text{IsComptimeAvailable}(R)}$$

**(CT-Enum)**
$$\frac{E = \texttt{enum} \{ V_1(P_1), \ldots, V_n(P_n) \} \quad \forall i.\ \text{IsComptimeAvailable}(P_i)}{\text{IsComptimeAvailable}(E)}$$

##### Constraints & Legality

**Negative Constraints**

The following types are NOT comptime-available and MUST NOT be used in comptime calculations:
1. Reference types (`&T`, `&unique T`, `&shared T`).
2. Pointer types (`Ptr<T>`, `*imm T`, `*mut T`).
3. Capability types (`$Cap`, `$Cap`).
4. Modal types (except those explicitly defined as comptime-available in the standard library).
5. Types containing `dyn` references.

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CTE-0010` | Error    | Non-comptime-available type in comptime expr. | Compile-time | Rejection |
| `E-CTE-0011` | Error    | Reference type in comptime context.           | Compile-time | Rejection |
| `E-CTE-0012` | Error    | Capability type in comptime context.          | Compile-time | Rejection |

---

### 17.3 Comptime Blocks and Expressions

##### Definition

A **comptime block** is a statement block executed during the Metaprogramming Phase for its side effects (such as code emission). A **comptime expression** is an expression evaluated during the Metaprogramming Phase that resolves to a constant value injected into the program AST.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_block ::= "comptime" block
comptime_expr  ::= "comptime" "{" expression "}"
```

##### Static Semantics

**Typing Rules**

**(T-ComptimeBlock)**
$$\frac{\Gamma_{ct} \vdash \textit{stmts} : ()}{\Gamma \vdash \texttt{comptime } \{ \textit{stmts} \} : ()}$$

**(T-ComptimeExpr)**
$$\frac{\Gamma_{ct} \vdash e : T \quad \text{IsComptimeAvailable}(T)}{\Gamma \vdash \texttt{comptime } \{ e \} : T}$$

**Validation**

The body of a comptime block or expression MUST satisfy the isolation, purity, and determinism properties defined in §17.1.

##### Dynamic Semantics

**Evaluation**

1. The implementation evaluates the body within the environment $\Gamma_{ct}$.
2. For a **comptime block**:
   - The block is executed for side effects (e.g., `emit`).
   - The block does not produce a runtime value.
3. For a **comptime expression**:
   - The expression evaluates to a value $v$.
   - The implementation converts $v$ into a literal AST node representing that value.
   - The `comptime` expression in the AST is replaced by this literal node.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CTE-0020` | Error    | Comptime block contains prohibited construct. | Compile-time | Rejection |
| `E-CTE-0021` | Error    | Comptime expression has non-comptime type.    | Compile-time | Rejection |
| `E-CTE-0022` | Error    | Comptime evaluation diverges.                 | Compile-time | Rejection |
| `E-CTE-0023` | Error    | Comptime evaluation panics.                   | Compile-time | Rejection |

---

### 17.4 Comptime Procedures

##### Definition

A **comptime procedure** is a procedure that may be invoked only during the Metaprogramming Phase. It operates on comptime-available types and may utilize comptime capabilities.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_procedure_decl ::= 
    "comptime" "procedure" identifier generic_params? 
    "(" param_list ")" "->" return_type 
    [ requires_clause ] block

requires_clause    ::= "requires" comptime_predicate ("," comptime_predicate)*
comptime_predicate ::= expression
```

##### Static Semantics

**Typing Rules**

**(T-ComptimeProc)**
$$\frac{
    \forall i.\ \text{IsComptimeAvailable}(T_i) \quad
    \text{IsComptimeAvailable}(R) \quad
    \Gamma_{ct}, x_1:T_1, \ldots, x_n:T_n \vdash \textit{body} : R
}{
    \Gamma \vdash \texttt{comptime procedure } f(x_1:T_1, \ldots) \to R\ \{ \textit{body} \} : \text{wf}
}$$

**(T-ComptimeCall)**
$$\frac{
    \Gamma_{ct} \vdash f : (T_1, \ldots, T_n) \to R \quad
    \forall i.\ \Gamma_{ct} \vdash e_i : T_i
}{
    \Gamma_{ct} \vdash f(e_1, \ldots, e_n) : R
}$$

**Validation**

1. All parameter types and the return type MUST be comptime-available.
2. The procedure body MUST satisfy all restrictions of the comptime environment (§17.1).
3. If a `requires` clause is present, all predicates MUST evaluate to `true` at every call site.

##### Constraints & Legality

**Negative Constraints**

Comptime procedures MUST NOT:
1. Be called from runtime procedures.
2. Access runtime capabilities.
3. Perform operations prohibited in §17.1.

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-CTE-0030` | Error    | Parameter type not comptime-available.          | Compile-time | Rejection |
| `E-CTE-0031` | Error    | Return type not comptime-available.             | Compile-time | Rejection |
| `E-CTE-0032` | Error    | Comptime procedure body violates restrictions.  | Compile-time | Rejection |
| `E-CTE-0033` | Error    | Requires clause predicate evaluates to false.   | Compile-time | Rejection |
| `E-CTE-0034` | Error    | Comptime procedure called from runtime context. | Compile-time | Rejection |

---

### 17.5 Comptime Capabilities

##### Definition

**Comptime capabilities** are intrinsic capabilities that authorize specific operations within the comptime environment. They are distinct from runtime capabilities and operate on the compilation context itself.

#### 17.5.1 TypeEmitter

##### Definition

The **TypeEmitter** capability authorizes the injection of generated AST nodes into the compilation unit.

##### Syntax & Declaration

```cursive
capability TypeEmitter {
    procedure emit(~, ast: Ast)
    procedure acquire_write_key(~, target: Type, form: Type) -> WriteKey
    procedure target_type(~) -> Type
}
```

##### Static Semantics

**Provision**

The `TypeEmitter` capability is provided to:
1. Comptime blocks annotated with `[[emit]]`.
2. Derive target procedure bodies (§20.2).

It is accessible via the identifier `emitter`.

**Typing Rules**

**(T-Emit)**
$$\frac{
    \Gamma_{ct} \vdash \textit{emitter} : \texttt{TypeEmitter} \quad
    \Gamma_{ct} \vdash \textit{ast} : \texttt{Ast}
}{
    \Gamma_{ct} \vdash \textit{emitter}\texttt{\textasciitilde>emit}(\textit{ast}) : ()
}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-CTE-0040` | Error    | Emit operation without `TypeEmitter` capability. | Compile-time | Rejection |
| `E-CTE-0041` | Error    | `[[emit]]` attribute on non-comptime block.      | Compile-time | Rejection |
| `E-CTE-0042` | Error    | Emitted AST is ill-formed.                       | Compile-time | Rejection |

---

#### 17.5.2 Introspect

##### Definition

The **Introspect** capability authorizes reflection over the structure of types accessible in the comptime environment.

##### Syntax & Declaration

```cursive
capability Introspect {
    procedure category<T>() -> TypeCategory
    procedure fields<T>() -> [FieldInfo]
        where category::<T>() == TypeCategory::Record
    procedure variants<T>() -> [VariantInfo]
        where category::<T>() == TypeCategory::Enum
    procedure states<T>() -> [StateInfo]
        where category::<T>() == TypeCategory::Modal
    procedure implements_form<T; F>() -> bool
    procedure type_name<T>() -> string
    procedure module_path<T>() -> string
}
```

##### Static Semantics

**Provision**

The `Introspect` capability is implicitly provided to all comptime contexts. It is accessible via the identifier `introspect`.

**Typing Rules**

**(T-Introspect-Category)**
$$\frac{\Gamma_{ct} \vdash T : \texttt{Type}}{\Gamma_{ct} \vdash \textit{introspect}\texttt{\textasciitilde>category::<}T\texttt{>()} : \texttt{TypeCategory}}$$

**(T-Introspect-Fields)**
$$\frac{\Gamma_{ct} \vdash T : \texttt{Type} \quad \text{category}(T) = \texttt{Record}}{\Gamma_{ct} \vdash \textit{introspect}\texttt{\textasciitilde>fields::<}T\texttt{>()} : [\texttt{FieldInfo}]}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                           | Detection    | Effect    |
| :----------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-CTE-0050` | Error    | `fields` called on non-record type. | Compile-time | Rejection |
| `E-CTE-0051` | Error    | `variants` called on non-enum type. | Compile-time | Rejection |
| `E-CTE-0052` | Error    | `states` called on non-modal type.  | Compile-time | Rejection |
| `E-CTE-0053` | Error    | Introspection of incomplete type.   | Compile-time | Rejection |

---

#### 17.5.3 ProjectFiles

##### Definition

The **ProjectFiles** capability authorizes read-only access to files located within the project's source root during compilation.

##### Syntax & Declaration

```cursive
capability ProjectFiles {
    procedure read(~, path: string) -> Result<string, FileError>
    procedure read_bytes(~, path: string) -> Result<[u8], FileError>
    procedure exists(~, path: string) -> bool
    procedure list_dir(~, path: string) -> Result<[string], FileError>
    procedure project_root(~) -> string
}
```

##### Static Semantics

**Provision**

The `ProjectFiles` capability is provided ONLY to comptime blocks annotated with the `[[files]]` attribute. It is accessible via the identifier `files`.

**Path Resolution**

All paths MUST be relative to the project root directory. Paths MUST NOT:
1. Be absolute.
2. Contain `..` components that traverse above the project root.
3. Follow symbolic links that target locations outside the project root.

**Determinism**

The implementation MUST ensure determinism by capturing the state of project files at the start of the Metaprogramming Phase. Subsequent external modifications during compilation MUST NOT be observable via this capability.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-CTE-0060` | Error    | File operation without `ProjectFiles` capability. | Compile-time | Rejection |
| `E-CTE-0061` | Error    | `[[files]]` attribute on non-comptime block.      | Compile-time | Rejection |
| `E-CTE-0062` | Error    | Path escapes project directory.                   | Compile-time | Rejection |
| `E-CTE-0063` | Error    | Absolute path in file operation.                  | Compile-time | Rejection |

---

#### 17.5.4 ComptimeDiagnostics

##### Definition

The **ComptimeDiagnostics** capability authorizes the emission of compiler diagnostics (errors, warnings, notes) from user code.

##### Syntax & Declaration

```cursive
capability ComptimeDiagnostics {
    procedure error(~!, message: string) -> !
    procedure error_at(~!, message: string, span: SourceSpan) -> !
    procedure warning(~, message: string)
    procedure warning_at(~, message: string, span: SourceSpan)
    procedure note(~, message: string)
    procedure note_at(~, message: string, span: SourceSpan)
    procedure current_span(~) -> SourceSpan
    procedure current_module(~) -> string
}
```

##### Static Semantics

**Provision**

The `ComptimeDiagnostics` capability is implicitly provided to all comptime contexts. It is accessible via the identifier `diagnostics`.

**Termination**

The `error` and `error_at` procedures have return type `!`. Invocation of these procedures MUST terminate the Metaprogramming Phase and abort compilation.

##### Dynamic Semantics

**Error Emission**

When `error` or `error_at` is executed:
1. The diagnostic message is recorded.
2. The Metaprogramming Phase is immediately terminated.
3. Compilation fails with diagnostic `E-CTE-0070`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                              | Detection    | Effect    |
| :----------- | :------- | :------------------------------------- | :----------- | :-------- |
| `E-CTE-0070` | Error    | Comptime error emitted by user code.   | Compile-time | Rejection |
| `W-CTE-0071` | Warning  | Comptime warning emitted by user code. | Compile-time | Continue  |

---

### 17.6 Comptime Control Flow

##### Definition

**Comptime control flow** constructs (`comptime if`, `comptime for`) allow conditional compilation and code generation logic to be expressed directly in the program syntax.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_if  ::= "comptime" "if" comptime_expr block ("else" (comptime_if | block))?
comptime_for ::= "comptime" "for" pattern "in" comptime_expr block
```

##### Static Semantics

**Typing Rules**

**(T-ComptimeIf)**
$$\frac{
    \Gamma_{ct} \vdash e : \texttt{bool} \quad
    \Gamma \vdash \textit{then} : T \quad
    \Gamma \vdash \textit{else} : T
}{
    \Gamma \vdash \texttt{comptime if } e\ \textit{then}\ \texttt{else}\ \textit{else} : T
}$$

**(T-ComptimeFor)**
$$\frac{
    \Gamma_{ct} \vdash e : [T] \quad
    \Gamma, x: T \vdash \textit{body} : U
}{
    \Gamma \vdash \texttt{comptime for } x\ \texttt{in } e\ \textit{body} : [U]
}$$

##### Dynamic Semantics

**Evaluation of Comptime If**

1. Evaluate condition $e$ in $\Gamma_{ct}$.
2. If $e$ evaluates to `true`, the `then` block is included in the AST.
3. If $e$ evaluates to `false`, the `else` block (if present) is included in the AST.
4. The non-selected branch is discarded and is NOT subject to semantic analysis or type checking beyond syntactic well-formedness.

**Evaluation of Comptime For**

1. Evaluate iterator expression $e$ in $\Gamma_{ct}$ to produce a sequence $[v_1, \ldots, v_n]$.
2. For each $v_i$, instantiate the loop body with the pattern bound to $v_i$.
3. Concatenate the instantiated bodies in sequence.
4. Replace the `comptime for` node in the AST with the sequence of unrolled statements.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-CTE-0080` | Error    | Comptime if condition not comptime-evaluable. | Compile-time | Rejection |
| `E-CTE-0081` | Error    | Comptime if condition not boolean.            | Compile-time | Rejection |
| `E-CTE-0082` | Error    | Comptime for iterator not comptime-evaluable. | Compile-time | Rejection |
| `E-CTE-0083` | Error    | Comptime for iterator not iterable.           | Compile-time | Rejection |
| `E-CTE-0084` | Error    | Comptime for exceeds iteration limit.         | Compile-time | Rejection |

---

### 17.7 Comptime Assertions

##### Definition

A **comptime assertion** is a static check that enforces a boolean condition during compilation. If the condition evaluates to false, compilation halts.

##### Syntax & Declaration

**Grammar**

```ebnf
comptime_assert ::= "comptime" "assert" "(" comptime_expr ("," string_literal)? ")"
```

##### Static Semantics

**Typing Rule**

**(T-ComptimeAssert)**
$$\frac{
    \Gamma_{ct} \vdash e : \texttt{bool}
}{
    \Gamma \vdash \texttt{comptime assert}(e) : ()
}$$

##### Dynamic Semantics

**Evaluation**

1. Evaluate condition $e$ in $\Gamma_{ct}$.
2. If $e$ evaluates to `true`, compilation proceeds.
3. If $e$ evaluates to `false`, the implementation MUST emit `E-CTE-0090` (referencing the optional message string if provided) and halt compilation.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-CTE-0090` | Error    | Comptime assertion failed.                      | Compile-time | Rejection |
| `E-CTE-0091` | Error    | Comptime assertion condition not boolean.       | Compile-time | Rejection |
| `E-CTE-0092` | Error    | Comptime assertion condition not comptime-eval. | Compile-time | Rejection |

---

### 17.8 Resource Limits

##### Definition

**Resource limits** are the minimum capacities that a conforming implementation MUST support for comptime execution. These limits ensure that comptime programs terminate and do not exhaust system resources indefinitely.

##### Static Semantics

**Minimum Limits**

Implementations MUST support at least the following limits for comptime execution:

| Resource                | Minimum Limit | Diagnostic   |
| :---------------------- | :------------ | :----------- |
| Recursion depth         | 128 frames    | `E-CTE-0100` |
| Loop iterations         | 65,536        | `E-CTE-0101` |
| Memory allocation       | 64 MiB        | `E-CTE-0102` |
| AST nodes generated     | 1,000,000     | `E-CTE-0103` |
| Comptime procedure time | 60 seconds    | `E-CTE-0104` |

Implementations MAY provide higher limits or user-configurable limits.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-CTE-0100` | Error    | Comptime recursion depth exceeded.      | Compile-time | Rejection |
| `E-CTE-0101` | Error    | Comptime iteration limit exceeded.      | Compile-time | Rejection |
| `E-CTE-0102` | Error    | Comptime memory limit exceeded.         | Compile-time | Rejection |
| `E-CTE-0103` | Error    | Comptime AST node limit exceeded.       | Compile-time | Rejection |
| `E-CTE-0104` | Error    | Comptime execution time limit exceeded. | Compile-time | Rejection |

---

## Clause 18: Type Reflection

### 18.1 The Type Metatype

##### Definition

The **Type metatype** (`Type`) is an opaque, compile-time-only scalar type representing a specific type within the compilation unit. A value of type `Type` is a unique handle to the compiler's internal type definition, enabling introspection of the type's structure, properties, and relationships.

**Formal Definition**

Let $\mathcal{T}_{universe}$ be the set of all well-formed types in the program. The type `Type` is the set of handles to elements in $\mathcal{T}_{universe}$.

$$\texttt{Type} = \{ \text{handle}(T) \mid T \in \mathcal{T}_{universe} \}$$

Two values $t_1, t_2 : \texttt{Type}$ are equal if and only if the types they represent are equivalent per the type equivalence rules defined in §4.1.

##### Syntax & Declaration

**Grammar**

```ebnf
type_literal ::= "Type" "::" "<" type ">"
```

##### Static Semantics

**Typing Rule**

A type literal evaluates to a value of type `Type`. The type argument MUST be a well-formed type in the current context.

$$\frac{
    \Gamma \vdash T\ \text{wf}
}{
    \Gamma_{ct} \vdash \texttt{Type::<}T\texttt{>} : \texttt{Type}
} \quad \text{(T-TypeLiteral)}$$

**Comptime Availability**

The `Type` type is **comptime-available** (§17.2). Values of type `Type` MUST NOT exist at runtime. Any attempt to use `Type` in a runtime context is ill-formed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                               | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------- | :----------- | :-------- |
| `E-REF-0010` | Error    | Type argument to `Type::<>` ill-formed. | Compile-time | Rejection |
| `E-REF-0011` | Error    | `Type` used in runtime context.         | Compile-time | Rejection |

---

### 18.2 Type Categories

##### Definition

The **Type Category** partitions the universe of types into mutually exclusive structural classes. Every type belongs to exactly one category.

##### Syntax & Declaration

**Enumeration Definition**

Implementations MUST provide the following definition in the `cursive::reflection` namespace:

```cursive
enum TypeCategory {
    Record,
    Enum,
    Modal,
    Primitive,
    Tuple,
    Array,
    Slice,
    Procedure,
    Reference,
    Generic,
}
```

##### Static Semantics

**Category Mapping**

The mapping from type structures to `TypeCategory` variants is defined as follows:

| Type Structure $T$                                          | $\text{category}(T)$ |
| :---------------------------------------------------------- | :------------------- |
| `record` definitions                                        | `Record`             |
| `enum` definitions                                          | `Enum`               |
| `modal` definitions (including generated async modals)      | `Modal`              |
| `iN`, `uN`, `fN`, `bool`, `char`, `()`, `!`                 | `Primitive`          |
| `(T1, ...)`                                                 | `Tuple`              |
| `[T; N]`                                                    | `Array`              |
| `[T]`                                                       | `Slice`              |
| `(...) -> R` and `\|...\| -> R`                             | `Procedure`          |
| `*imm T`, `*mut T`, `Ptr<T>`, `&T`, `&unique T`, `&const T` | `Reference`          |
| Unsubstituted type parameter $T$                            | `Generic`            |

**Introspection Procedure**

The `category` procedure returns the category of a type:

```cursive
comptime procedure category<T>() -> TypeCategory
```

$$\frac{
    \Gamma_{ct} \vdash T : \texttt{Type}
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>category::<}T\texttt{>()} \Rightarrow \text{category}(T)
} \quad \text{(Reflect-Category)}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-REF-0020` | Error    | Category query on incomplete/opaque type | Compile-time | Rejection |

---

### 18.3 Structural Introspection

##### Definition

Structural introspection provides access to the constituent parts of composite types. The availability of specific introspection procedures is constrained by the target type's category.

#### 18.3.1 Record Introspection

##### Definition

Record introspection exposes the fields of a record type.

##### Syntax & Declaration

**Reflection Types**

Implementations MUST provide the following definitions:

```cursive
record FieldInfo {
    name: string,
    type_id: Type,
    offset: u64,
    visibility: Visibility,
}

enum Visibility {
    Public,
    Internal,
    Protected,
    Private,
}
```

**Introspection Procedure**

```cursive
comptime procedure fields<T>() -> [FieldInfo]
```

##### Static Semantics

**Typing Rule**

The `fields` procedure is well-formed if and only if $T$ is a record type.

$$\frac{
    \text{category}(T) = \texttt{Record}
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>fields::<}T\texttt{>()} : [\texttt{FieldInfo}]
} \quad \text{(Reflect-Fields)}$$

**Evaluation**

The returned array MUST contain one `FieldInfo` element for each field in the record definition, in strict declaration order.

- `name`: The field identifier.
- `type_id`: The `Type` handle for the field's type.
- `offset`: The byte offset of the field relative to the record start. This value depends on the computed layout (including `[[layout(...)]]` attributes).
- `visibility`: The declared visibility of the field.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-REF-0030` | Error    | `fields` called on non-record type | Compile-time | Rejection |

---

#### 18.3.2 Enum Introspection

##### Definition

Enum introspection exposes the variants of an enum type.

##### Syntax & Declaration

**Reflection Types**

```cursive
record VariantInfo {
    name: string,
    discriminant: i64,
    fields: [FieldInfo],
}
```

**Introspection Procedure**

```cursive
comptime procedure variants<T>() -> [VariantInfo]
```

##### Static Semantics

**Typing Rule**

The `variants` procedure is well-formed if and only if $T$ is an enum type.

$$\frac{
    \text{category}(T) = \texttt{Enum}
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>variants::<}T\texttt{>()} : [\texttt{VariantInfo}]
} \quad \text{(Reflect-Variants)}$$

**Evaluation**

The returned array MUST contain one `VariantInfo` element for each variant in the enum definition, in strict declaration order.

- `name`: The variant identifier.
- `discriminant`: The integer value of the variant's discriminant.
- `fields`:
    - For unit variants (`V`), this array is empty.
    - For tuple variants (`V(T1, T2)`), this array contains fields with implementation-defined names (e.g., "0", "1").
    - For record variants (`V{x: T}`), this array contains the named fields.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                           | Detection    | Effect    |
| :----------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-REF-0040` | Error    | `variants` called on non-enum type. | Compile-time | Rejection |

---

#### 18.3.3 Modal Introspection

##### Definition

Modal introspection exposes the states and transitions of a modal type.

##### Syntax & Declaration

**Reflection Types**

```cursive
record StateInfo {
    name: string,
    fields: [FieldInfo],
    transitions: [TransitionInfo],
}

record TransitionInfo {
    name: string,
    target_state: string,
    parameters: [FieldInfo],
}
```

**Introspection Procedure**

```cursive
comptime procedure states<T>() -> [StateInfo]
```

##### Static Semantics

**Typing Rule**

The `states` procedure is well-formed if and only if $T$ is a modal type.

$$\frac{
    \text{category}(T) = \texttt{Modal}
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>states::<}T\texttt{>()} : [\texttt{StateInfo}]
} \quad \text{(Reflect-States)}$$

**Evaluation**

The returned array MUST contain one `StateInfo` element for each state defined in the modal type.

- `name`: The state identifier (without `@`).
- `fields`: The payload fields defined for that state.
- `transitions`: The transition procedures defined within that state block.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                          | Detection    | Effect    |
| :----------- | :------- | :--------------------------------- | :----------- | :-------- |
| `E-REF-0050` | Error    | `states` called on non-modal type. | Compile-time | Rejection |

---

### 18.4 Form Introspection

##### Definition

Form introspection allows querying whether a type implements a specific form (class) and inspecting the requirements of a form.

##### Syntax & Declaration

**Reflection Types**

```cursive
record ProcedureInfo {
    name: string,
    parameters: [ParameterInfo],
    return_type: Type,
    is_abstract: bool,
}

record ParameterInfo {
    name: string,
    type_id: Type,
    mode: ParameterMode,
}

enum ParameterMode {
    Value,      // x: T
    RefConst,   // ~ or x: const T
    RefUnique,  // ~! or x: unique T
    RefShared,  // ~% or x: shared T
    Move,       // move x: T
}
```

**Introspection Procedures**

```cursive
comptime procedure implements_form<T; Form>() -> bool
comptime procedure required_procedures<Form>() -> [ProcedureInfo]
```

##### Static Semantics

**Typing Rules**

**(T-ImplementsForm)**
$$\frac{
    \Gamma_{ct} \vdash T : \texttt{Type} \quad
    \text{IsForm}(F)
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>implements\_form::<}T, F\texttt{>()} : \texttt{bool}
}$$

**(T-RequiredProcs)**
$$\frac{
    \text{IsForm}(F)
}{
    \Gamma_{ct} \vdash \texttt{introspect\textasciitilde>required\_procedures::<}F\texttt{>()} : [\texttt{ProcedureInfo}]
}$$

**Evaluation**

- `implements_form` returns `true` if and only if the judgment $\Gamma \vdash T <: F$ holds.
- `required_procedures` returns descriptions of all procedures in the form $F$ that do not have default implementations.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                        | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------- | :----------- | :-------- |
| `E-REF-0060` | Error    | `required_procedures` called on non-form type.   | Compile-time | Rejection |
| `E-REF-0061` | Error    | Second argument to `implements_form` not a form. | Compile-time | Rejection |

---

### 18.5 Type Predicates

##### Definition

Type predicates are convenience procedures that return boolean values indicating whether a type possesses a specific property.

##### Syntax & Declaration

**Procedures**

```cursive
comptime procedure is_record<T>() -> bool
comptime procedure is_enum<T>() -> bool
comptime procedure is_modal<T>() -> bool
comptime procedure is_primitive<T>() -> bool
comptime procedure is_tuple<T>() -> bool
comptime procedure is_array<T>() -> bool
comptime procedure is_slice<T>() -> bool
comptime procedure is_sized<T>() -> bool
comptime procedure is_copy<T>() -> bool
comptime procedure has_layout<T>() -> bool
```

##### Static Semantics

**Semantics**

These procedures are semantically equivalent to checking the `TypeCategory` or specific trait implementations:

- `is_X<T>()` $\iff \text{category}(T) == X$ (for record, enum, modal, primitive, tuple, array).
- `is_slice<T>()` $\iff \text{category}(T) == \text{Slice}$.
- `is_sized<T>()` holds if $T$ has a constant size known at compile time (i.e., $T$ is not a DST like `[T]` or `$Class`).
- `is_copy<T>()` holds if $\Gamma \vdash T <: \text{Copy}$.
- `has_layout<T>()` holds if $T$ is a concrete type with a fixed memory layout. This excludes `opaque` types, `dyn` types, unsized types, and unmonomorphized generic parameters. This predicate matches the `HasLayout(T)` requirement in §21.1.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------- | :----------- | :-------- |
| `E-REF-0070` | Error    | Type predicate called on incomplete type | Compile-time | Rejection |

---

### 18.6 Name Introspection

##### Definition

Name introspection procedures return string representations of type identifiers and paths.

##### Syntax & Declaration

**Procedures**

```cursive
comptime procedure type_name<T>() -> string
comptime procedure module_path<T>() -> string
comptime procedure full_path<T>() -> string
```

##### Static Semantics

**Return Values**

1.  **`type_name<T>()`**: Returns the unqualified identifier of $T$.
    *   For generic instantiations `Vec<i32>`, returns `"Vec<i32>"`.
    *   For primitives, returns the keyword (e.g., `"i32"`).

2.  **`module_path<T>()`**: Returns the canonical path of the module where $T$ is defined.
    *   Example: `"std::collections"`.
    *   For primitives, returns `"std::core"` (or implementation-defined core module).

3.  **`full_path<T>()`**: Returns the fully qualified path.
    *   Formally: `module_path<T>() + "::" + type_name<T>()`.
    *   Example: `"std::collections::Vec<i32>"`.

**String Lifetime**

The returned strings are constructed during the Metaprogramming Phase. They are essentially string literals allocated in the comptime environment.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------ | :----------- | :-------- |
| `E-REF-0080` | Error    | Name introspection on anonymous type  | Compile-time | Rejection |
| `E-REF-0081` | Error    | Name introspection on incomplete type | Compile-time | Rejection |

---

## Clause 19: Code Generation

### 19.1 The Ast Type

##### Definition

The **`Ast` type** is an opaque, comptime-only sum type representing a fragment of the Cursive Abstract Syntax Tree. Values of type `Ast` represent syntactically valid program constructs.

**Formal Definition**

The `Ast` type is the disjoint union of five variant types:

$$\texttt{Ast} = \texttt{Ast::Expr} \uplus \texttt{Ast::Stmt} \uplus \texttt{Ast::Item} \uplus \texttt{Ast::Type} \uplus \texttt{Ast::Pattern}$$

Each variant corresponds to a specific syntactic category defined in the language grammar:

| Variant        | Syntactic Category | Authoritative Clause |
| :------------- | :----------------- | :------------------- |
| `Ast::Expr`    | Expression         | Clause 12            |
| `Ast::Stmt`    | Statement          | Clause 12            |
| `Ast::Item`    | Declaration        | Clause 8–10          |
| `Ast::Type`    | Type               | Clause 4–6           |
| `Ast::Pattern` | Pattern            | §12.2                |

##### Static Semantics

**Subtyping**

Each specific variant type is a subtype of the general `Ast` type:

$$\forall K \in \{\texttt{Expr}, \texttt{Stmt}, \texttt{Item}, \texttt{Type}, \texttt{Pattern}\}.\ \texttt{Ast::}K <: \texttt{Ast}$$

**Comptime Availability**

The `Ast` type and its variants satisfy the `IsComptimeAvailable` predicate defined in §17.2.

##### Constraints & Legality

**Runtime Prohibition**

The `Ast` type and its variants MUST NOT appear in the type of any runtime storage location, parameter, or return value. `Ast` values exist exclusively within the comptime execution environment.

**Diagnostic Table**

| Code         | Severity | Condition                                          | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0010` | Error    | `Ast` type used in runtime context.                | Compile-time | Rejection |
| `E-GEN-0011` | Error    | `Ast` variant mismatch in assignment or parameter. | Compile-time | Rejection |

---

### 19.2 Quote Expressions

##### Definition

A **quote expression** is a comptime expression that constructs an `Ast` value from a sequence of tokens. The content of a quote expression is parsed but not evaluated; it is captured as a reified syntax tree.

##### Syntax & Declaration

**Grammar**

```ebnf
quote_expr    ::= "quote" [ category_spec ] "{" quoted_content "}"

category_spec ::= "type" | "pattern"

quoted_content ::= /* Balanced token sequence */
```

**Syntactic Disambiguation**

The `category_spec` determines the syntactic category used to parse `quoted_content`:

1.  `quote { ... }`: Parses as an **Item**, **Statement**, or **Expression**. The implementation MUST attempt to parse the content in that order of precedence.
2.  `quote type { ... }`: Parses as a **Type**.
3.  `quote pattern { ... }`: Parses as a **Pattern**.

##### Static Semantics

**Typing Rules**

**(T-Quote-Expr)**
$$\frac{
    \textit{content} \text{ parses as } \texttt{Expr}
}{
    \Gamma_{ct} \vdash \texttt{quote } \{\ \textit{content}\ \} : \texttt{Ast::Expr}
}$$

**(T-Quote-Stmt)**
$$\frac{
    \textit{content} \text{ parses as } \texttt{Stmt}
}{
    \Gamma_{ct} \vdash \texttt{quote } \{\ \textit{content}\ \} : \texttt{Ast::Stmt}
}$$

**(T-Quote-Item)**
$$\frac{
    \textit{content} \text{ parses as } \texttt{Item}
}{
    \Gamma_{ct} \vdash \texttt{quote } \{\ \textit{content}\ \} : \texttt{Ast::Item}
}$$

**(T-Quote-Type)**
$$\frac{
    \textit{content} \text{ parses as } \texttt{Type}
}{
    \Gamma_{ct} \vdash \texttt{quote type } \{\ \textit{content}\ \} : \texttt{Ast::Type}
}$$

**(T-Quote-Pattern)**
$$\frac{
    \textit{content} \text{ parses as } \texttt{Pattern}
}{
    \Gamma_{ct} \vdash \texttt{quote pattern } \{\ \textit{content}\ \} : \texttt{Ast::Pattern}
}$$

**Hygiene Scope**

Identifiers appearing within `quoted_content` that are not introduced by bindings within the quote itself are **captured** from the enclosing comptime scope. See §19.4 (Hygiene).

##### Constraints & Legality

**Parsing Requirement**

The `quoted_content` MUST form a syntactically valid construct of the requested category. Partial or malformed constructs (e.g., unbalanced braces, missing delimiters) result in a compile-time error.

**Diagnostic Table**

| Code         | Severity | Condition                                  | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------- | :----------- | :-------- |
| `E-GEN-0020` | Error    | Quoted content is syntactically malformed. | Compile-time | Rejection |
| `E-GEN-0021` | Error    | Quote expression outside comptime scope.   | Compile-time | Rejection |

---

### 19.3 Splice Expressions

##### Definition

A **splice expression** is a mechanism to insert computed values into a quote expression. A splice evaluates a comptime expression and substitutes its result into the enclosing syntax tree.

##### Syntax & Declaration

**Grammar**

```ebnf
splice_expr  ::= "$" "(" expression ")"
               | "$" identifier
```

The shorthand `$ident` is syntactically equivalent to `$(ident)`.

##### Static Semantics

**Splice Contexts**

Splice expressions are permitted ONLY within the `quoted_content` of a quote expression.

**Allowed Splice Types**

The type of the expression inside the splice, $T_{expr}$, determines the semantics of the insertion:

1.  **AST Splice ($T_{expr} <: \texttt{Ast}$):**
    The AST node is inserted directly into the tree. The variant of the `Ast` value MUST be compatible with the syntactic position of the splice.

2.  **Literal Splice ($T_{expr} \in \text{ComptimeLiteral}$):**
    The value is converted to a literal token (e.g., integer literal, string literal) and inserted. `ComptimeLiteral` includes all primitive types and `string`.

3.  **Identifier Splice ($T_{expr} = \texttt{string}$):**
    If the splice appears in a position where an **identifier** is expected, a string value MAY be spliced. The string content MUST be a valid identifier. This operation constructs a fresh identifier symbol (unhygienic introduction).

**Typing Rules**

**(T-Splice-Ast)**
$$\frac{
    \Gamma_{ct} \vdash e : \texttt{Ast::}K \quad \text{Context expects category } K
}{
    \text{Splice is well-formed}
}$$

**(T-Splice-Lit)**
$$\frac{
    \Gamma_{ct} \vdash e : T \quad T \in \text{ComptimeLiteral}
}{
    \text{Splice converts value to AST literal node}
}$$

**(T-Splice-Ident)**
$$\frac{
    \Gamma_{ct} \vdash e : \texttt{string} \quad \text{Context expects Identifier}
}{
    \text{Splice constructs Identifier from string value}
}$$

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0030` | Error    | Splice expression outside quote context.                  | Compile-time | Rejection |
| `E-GEN-0031` | Error    | Splice expression type incompatible with syntax position. | Compile-time | Rejection |
| `E-GEN-0032` | Error    | String splice contains invalid identifier characters.     | Compile-time | Rejection |

---

### 19.4 Hygiene

##### Definition

**Hygiene** is the preservation of lexical scoping relationships across code generation boundaries. Identifiers in generated code must resolve to the bindings intended by the generator, regardless of the environment at the expansion site.

##### Static Semantics

**Symbol Resolution Environments**

1.  **Definition Site:** The lexical scope where the `quote` expression appears.
2.  **Expansion Site:** The lexical scope where the generated AST is emitted.

**Hygiene Invariants**

1.  **Referential Transparency:** An identifier free in a `quote` expression resolves to the binding visible at the **Definition Site**, not the Expansion Site.
2.  **Binding Safety:** An identifier bound within a `quote` expression (e.g., `let x = ...`) creates a unique binding that does not shadow or conflict with identifiers at the Expansion Site, unless explicitly intended.

**Renaming Mechanism**

The implementation MUST apply a renaming algorithm (e.g., $\alpha$-conversion) to all identifiers bound within generated code to ensure uniqueness.

$$\text{fresh}(id) = \text{gensym}(id)$$

**Unhygienic Introduction**

To introduce an identifier that binds to the **Expansion Site** environment (breaking hygiene), the generator MUST use a **string splice** in an identifier position.

*   `quote { let $x = 1; }` where `x` is `Ast`: Hygienic. `x` refers to a specific symbol handle.
*   `quote { let $("y") = 1; }`: Unhygienic. Introduces the identifier `y` into the expansion scope.

##### Constraints & Legality

**Capture Constraints**

An identifier captured from the Definition Site MUST remain valid (e.g., not dropped or out of scope) if it is referenced in the generated code.

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-GEN-0040` | Error    | Captured binding is invalid at emission time. | Compile-time | Rejection |

---

### 19.5 Emission

##### Definition

**Emission** is the side-effectful operation of inserting an AST fragment into the compilation unit. Emission is performed via the `TypeEmitter` capability.

##### Syntax & Declaration

The `TypeEmitter` capability provides the `emit` procedure.

```cursive
procedure emit(~, ast: Ast::Item)
```

##### Static Semantics

**Capability Requirement**

Emission requires the `TypeEmitter` capability. This capability is provided implicitly to:
1.  **Derive Targets:** Via the `emitter` binding.
2.  **Comptime Blocks:** Only if the block is annotated with `[[emit]]`.

**Insertion Point**

Emitted items are inserted into the **module scope** of the module containing the `[[emit]]` block or the type triggering the derivation. Emitted items are treated as if they were declared in the source text at that location.

**Post-Emission Verification**

Code emitted via `emit` is subject to all standard semantic checks (name resolution, type checking, borrow checking) **after** the Metaprogramming Phase completes.

##### Dynamic Semantics

**Evaluation**

1.  The `emit` procedure appends the `ast` node to the pending injection list for the current module.
2.  Upon completion of the Metaprogramming Phase, pending injections are merged into the module's AST.

##### Constraints & Legality

**Negative Constraints**

1.  Only `Ast::Item` variants may be emitted. Expressions and statements cannot be emitted directly at module scope; they must be wrapped in a procedure or constant declaration.
2.  The `emit` procedure MUST NOT be called without the `TypeEmitter` capability.

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-GEN-0050` | Error    | `emit` called without `TypeEmitter` capability. | Compile-time | Rejection |
| `E-GEN-0051` | Error    | Argument to `emit` is not an `Ast::Item`.       | Compile-time | Rejection |
| `E-GEN-0053` | Error    | Emitted code fails semantic analysis.           | Compile-time | Rejection |

---

### 19.6 Type Representation

##### Definition

**Type Representation** is the conversion of a resolved `Type` metavalue (obtained via introspection) into an `Ast::Type` syntax tree that can be used in a quote.

##### Syntax & Declaration

**Intrinsic**

```cursive
comptime procedure type_repr_of(t: Type) -> Ast::Type
```

##### Static Semantics

**Behavior**

The `type_repr_of` function returns an AST node that, when compiled, resolves to the exact type `t`.

1.  For nominal types, it returns a fully qualified path (e.g., `::pkg::module::Type`).
2.  For concrete generic instantiations, it includes the type arguments.
3.  For primitive types, it returns the standard type name.

This ensures that generated code refers to the correct types regardless of imports or shadowing at the expansion site.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                             | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------------- | :----------- | :-------- |
| `E-GEN-0060` | Error    | `type_repr_of` argument is not a valid concrete type. | Compile-time | Rejection |

---

### 19.7 Write Keys

##### Definition

A **Write Key** is a token representing exclusive permission to generate an implementation of a specific Form for a specific Type. Write Keys enforce the coherence rule (one implementation per form per type) during code generation.

##### Syntax & Declaration

**Intrinsic**

```cursive
comptime procedure acquire_write_key(
    emitter: TypeEmitter, 
    target: Type, 
    form: Type
) -> WriteKey
```

##### Static Semantics

**Exclusivity Invariant**

For any pair $(T, F)$ where $T$ is a target type and $F$ is a form type, the implementation MUST ensure that `acquire_write_key` succeeds exactly once across all derive targets in the compilation session.

**Typing Rule**

**(T-WriteKey)**
$$\frac{
    \Gamma_{ct} \vdash \textit{emitter} : \texttt{TypeEmitter} \quad
    \Gamma_{ct} \vdash T : \texttt{Type} \quad
    \Gamma_{ct} \vdash F : \texttt{Type} \quad
    \text{is\_form}(F)
}{
    \Gamma_{ct} \vdash \textit{emitter}\texttt{\textasciitilde>acquire\_write\_key}(T, F) : \texttt{WriteKey}
}$$

##### Dynamic Semantics

**Acquisition**

1.  The compiler checks the global registry of held write keys.
2.  If the key $(T, F)$ is already held, compilation fails immediately with `E-GEN-0070`.
3.  Otherwise, the key is registered and returned.

**Usage**

The `WriteKey` MUST be presented (conceptually or explicitly via API) when emitting the implementation block for `T <: F`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-GEN-0070` | Error    | Write key for (Type, Form) already acquired.  | Compile-time | Rejection |
| `E-GEN-0071` | Error    | `acquire_write_key` target `F` is not a Form. | Compile-time | Rejection |

---

## Clause 20: Derivation

### 20.1 Derive Attributes

##### Definition

A **derive attribute** is a directive attached to a type declaration that triggers the execution of one or more **derive targets** during the Metaprogramming Phase. The attribute requests the automatic generation of code—typically form implementations—based on the structure of the annotated type.

##### Syntax & Declaration

**Grammar**

```ebnf
derive_attr        ::= "[[" "derive" "(" derive_target_list ")" "]]"
derive_target_list ::= derive_target ("," derive_target)*
derive_target      ::= identifier
```

**Placement**

A derive attribute MUST immediately precede a `record`, `enum`, or `modal` type declaration.

##### Static Semantics

**Typing Rule**

The presence of a derive attribute schedules the associated derive targets for execution:

$$\frac{
    \texttt{[[derive(}D_1, \ldots, D_n\texttt{)]]} \text{ precedes type } T \quad
    \forall i.\; \Gamma_{ct} \vdash D_i : \text{DeriveTarget}
}{
    \text{ScheduleDerive}(T, \{D_1, \ldots, D_n\})
} \quad \text{(T-DeriveAttr)}$$

**Target Resolution**

Each `derive_target` identifier MUST resolve to a visible **derive target declaration** (§20.2) or a **standard derive target** (§20.6). Resolution follows standard name lookup rules (§8.7) within the comptime environment.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-DRV-0010` | Error    | Unknown derive target name.               | Compile-time | Rejection |
| `E-DRV-0011` | Error    | Derive attribute on non-type declaration. | Compile-time | Rejection |
| `E-DRV-0012` | Error    | Duplicate derive target in attribute.     | Compile-time | Rejection |

---

### 20.2 Derive Target Declarations

##### Definition

A **derive target declaration** defines a metaprogramming procedure invoked by the derive attribute. A derive target accepts a `Type` representation of the annotated declaration and emits AST nodes to the compilation unit.

##### Syntax & Declaration

**Grammar**

```ebnf
derive_target_decl ::= "derive" "target" identifier 
                       "(" "target" ":" "Type" ")" 
                       derive_contract?
                       block

derive_contract    ::= "|=" contract_clause ("," contract_clause)*
contract_clause    ::= "emits" class_ref
                     | "requires" class_ref
class_ref          ::= identifier
```

##### Static Semantics

**Typing Rule**

A derive target is well-formed if its body is well-typed in a context extended with the `target` parameter and implicit metaprogramming capabilities:

$$\frac{
    \Gamma_{ct}' = \Gamma_{ct} \cup \{
        \textit{target}: \texttt{Type},
        \textit{emitter}: \texttt{TypeEmitter},
        \textit{introspect}: \texttt{Introspect},
        \textit{diagnostics}: \texttt{ComptimeDiagnostics}
    \} \quad
    \Gamma_{ct}' \vdash \textit{body} : ()
}{
    \Gamma \vdash \texttt{derive target}\; N\;(\texttt{target}: \texttt{Type})\; \{\; \textit{body} \;\} : \text{DeriveTarget}
} \quad \text{(T-DeriveTarget)}$$

**Implicit Bindings**

The following bindings MUST be available within the `block` of a derive target:

| Identifier    | Type                  | Description                                      |
| :------------ | :-------------------- | :----------------------------------------------- |
| `target`      | `Type`                | The type definition annotated with `[[derive]]`. |
| `emitter`     | `TypeEmitter`         | Capability for AST emission (§17.5.1).           |
| `introspect`  | `Introspect`          | Capability for type introspection (§17.5.2).     |
| `diagnostics` | `ComptimeDiagnostics` | Capability for error reporting (§17.5.4).        |

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                   | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------ | :----------- | :-------- |
| `E-DRV-0020` | Error    | Derive target body violates comptime rules. | Compile-time | Rejection |
| `E-DRV-0021` | Error    | Contract references unknown form.           | Compile-time | Rejection |
| `E-DRV-0022` | Error    | Derive target has invalid signature.        | Compile-time | Rejection |

---

### 20.3 Derive Contracts

##### Definition

A **derive contract** formally specifies the dependencies and outputs of a derive target. The contract enables the compiler to schedule derive execution and verify correctness.

**Formal Definition**

A derive contract is a pair $(E, R)$ where:
- $E \subseteq \mathcal{T}_{\text{form}}$ is the set of forms emitted (implemented) by the generated code.
- $R \subseteq \mathcal{T}_{\text{form}}$ is the set of forms required to be implemented by the target type prior to execution.

##### Static Semantics

**Emits Clause (`emits F`)**

The `emits F` clause asserts that the code generated by the derive target implements form $F$ for the target type $T$.

Post-condition: $T <: F$ MUST hold after the Metaprogramming Phase completes.

**Requires Clause (`requires F`)**

The `requires F` clause asserts that the target type $T$ MUST implement form $F$ before the derive target executes.

Pre-condition: $T <: F$ MUST hold before the derive target is invoked.

**Contract Verification**

The implementation MUST verify the contract:
1. **Pre-execution:** Verify that `target` satisfies all `requires` clauses. Failure triggers `E-DRV-0030`.
2. **Post-execution:** Verify that `target` satisfies all `emits` clauses. Failure triggers `E-DRV-0031`.
3. **Unexpected Emission:** Verify that the target does not emit implementations for forms not listed in `emits`. Failure triggers `E-DRV-0032`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                     | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------- | :----------- | :-------- |
| `E-DRV-0030` | Error    | Required form not implemented by target type. | Compile-time | Rejection |
| `E-DRV-0031` | Error    | Emits clause not satisfied after execution.   | Compile-time | Rejection |
| `E-DRV-0032` | Error    | Undeclared form emitted.                      | Compile-time | Rejection |

---

### 20.4 Derive Execution Model

##### Definition

**Derive execution** is the schedule and invocation of derive targets during the Metaprogramming Phase. Execution order is determined by the dependencies declared in derive contracts.

**Formal Definition**

Let $G = (V, E)$ be the **Derive Dependency Graph**:
- $V = \{(T, D) \mid T \text{ is a type} \land D \in \text{derive\_list}(T)\}$
- $E = \{((T, D_1), (T, D_2)) \mid D_1 \texttt{ requires } F \land D_2 \texttt{ emits } F\}$

An edge exists from $D_2$ to $D_1$ when $D_1$ requires a form emitted by $D_2$.

##### Dynamic Semantics

**Execution Algorithm**

The implementation MUST execute derive targets as follows:

1. **Graph Construction:** Build $G$ from all types with `[[derive]]` attributes in the compilation unit.
2. **Cycle Detection:** If $G$ contains a cycle, the program is ill-formed (`E-DRV-0040`).
3. **Scheduling:** Compute a topological ordering $\pi$ of $V$.
4. **Execution:** For each $(T, D)$ in $\pi$:
   a. Verify $T$ satisfies $D.\text{requires}$.
   b. Invoke $D$ with `target` bound to $T$.
   c. Integrate emitted ASTs into the compilation unit.
   d. Verify $T$ satisfies $D.\text{emits}$.

**Parallel Execution**

The implementation MAY execute derive targets in parallel provided that:
1. The partial order defined by $G$ is respected.
2. Access to the `TypeEmitter` capability is synchronized or isolated.
3. The resulting AST is deterministic.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                 | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------- | :----------- | :-------- |
| `E-DRV-0040` | Error    | Cyclic derive dependency.                 | Compile-time | Rejection |
| `E-DRV-0041` | Error    | Derive target execution panics.           | Compile-time | Rejection |
| `E-DRV-0042` | Error    | Derive execution exceeds resource limits. | Compile-time | Rejection |

---

### 20.5 Generated Type Declarations

##### Definition

Derive targets output code by emitting a **replacement type declaration**. The emitted declaration replaces the original source declaration and includes all original members plus the generated implementations.

##### Static Semantics

**Emission Structure**

A derive target MUST emit a complete `Ast::Item` representing the target type. The emitted item MUST:
1. Retain the original type name.
2. Retain all original generic parameters.
3. Retain all original fields/variants/states in declaration order.
4. Include `implements` clauses for all emitted forms.
5. Include concrete procedure definitions for all emitted forms.

**Orphan Rule Enforcement**

Derive targets MUST NOT emit standalone implementation blocks (`implement T <: F { ... }`). All implementations must be inline within the emitted type declaration. This enforces the coherence property that a type and its form implementations are defined as a single unit.

##### Constraints & Legality

**Negative Constraints**

1. A derive target MUST NOT emit a type with a different name than the input `target`.
2. A derive target MUST NOT drop fields present in the input `target`.
3. A derive target MUST NOT emit implementations for forms not declared in `emits`.

**Diagnostic Table**

| Code         | Severity | Condition                                    | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------- | :----------- | :-------- |
| `E-DRV-0050` | Error    | Emitted declaration missing original fields. | Compile-time | Rejection |
| `E-DRV-0051` | Error    | Emitted declaration has wrong type name.     | Compile-time | Rejection |
| `E-DRV-0052` | Error    | Form procedure missing from emission.        | Compile-time | Rejection |

---

### 20.6 Standard Derive Targets

##### Definition

The **standard derive targets** are built-in derive targets provided by the language implementation. They generate implementations for foundational forms.

##### Static Semantics

**Target Catalog**

| Target        | Contract                      | Applicable Categories |
| :------------ | :---------------------------- | :-------------------- |
| `Debug`       | `\|= emits Debug`             | Record, Enum          |
| `Clone`       | `\|= emits Clone`             | Record, Enum          |
| `Eq`          | `\|= emits Eq`                | Record, Enum          |
| `Hash`        | `\|= emits Hash, requires Eq` | Record, Enum          |
| `Default`     | `\|= emits Default`           | Record                |
| `Serialize`   | `\|= emits Serialize`         | Record, Enum          |
| `Deserialize` | `\|= emits Deserialize`       | Record, Enum          |
| `FfiSafe`     | `\|= emits FfiSafe`           | Record, Enum          |

#### 20.6.1 Debug

**Generated Procedure**
```cursive
procedure debug(~) -> string
```

**Generation Logic**
- **Record:** Returns a string formatted as `"TypeName { field1: val1, ... }"`.
- **Enum:** Returns a string formatted as `"VariantName(val1, ...)"` or `"VariantName { ... }"`.
- **Constraint:** All fields MUST implement `Debug`. Violation triggers `E-DRV-0060`.

#### 20.6.2 Clone

**Generated Procedure**
```cursive
procedure clone(~) -> Self
```

**Generation Logic**
- **Record:** Returns a new instance where each field is initialized with `field.clone()`.
- **Enum:** Matches the current variant and returns a new instance of the same variant with cloned payload fields.
- **Constraint:** All fields MUST implement `Clone`. Violation triggers `E-DRV-0061`.

#### 20.6.3 Eq

**Generated Procedure**
```cursive
procedure eq(~, other: ~Self) -> bool
```

**Generation Logic**
- **Record:** Returns `true` if all fields compare equal to the corresponding fields in `other`.
- **Enum:** Returns `true` if `other` is the same variant and all payload fields compare equal.
- **Constraint:** All fields MUST implement `Eq`. Violation triggers `E-DRV-0062`.

#### 20.6.4 Hash

**Generated Procedure**
```cursive
procedure hash(~, hasher: &unique Hasher)
```

**Generation Logic**
- **Record:** Invokes `hasher.write` on each field in declaration order.
- **Enum:** Invokes `hasher.write` on the discriminant, followed by payload fields.
- **Constraint:** All fields MUST implement `Hash`. Violation triggers `E-DRV-0063`.

#### 20.6.5 Default

**Generated Procedure**
```cursive
procedure default() -> Self
```

**Generation Logic**
- **Record:** Returns a new instance where each field is initialized with `FieldType::default()`.
- **Constraint:** All fields MUST implement `Default`. Violation triggers `E-DRV-0064`.
- **Constraint:** Target MUST be a `record`. Violation triggers `E-DRV-0065`.

#### 20.6.6 Serialize and Deserialize

**Generated Procedures**
```cursive
procedure serialize(~, serializer: &unique Serializer) -> Result<(), SerializeError>
procedure deserialize(deserializer: &unique Deserializer) -> Result<Self, DeserializeError>
```

**Generation Logic**
The implementation maps the type structure to the `Serializer` and `Deserializer` interfaces defined in the core library. The serialization format is defined by the `Serializer` implementation.

**Constraints:**
- All fields MUST implement `Serialize` (for `serialize`) or `Deserialize` (for `deserialize`).
- Violations trigger `E-DRV-0066` or `E-DRV-0067`.

#### 20.6.7 FfiSafe

**Generated Procedures**
```cursive
comptime procedure c_size() -> usize
comptime procedure c_alignment() -> usize
```

**Generation Logic**
- Verifies the presence of `[[layout(C)]]`.
- Uses `introspect` to compute the C-compatible size and alignment of the type.
- **Constraint:** All fields MUST be `FfiSafe`. Violation triggers `E-FFI-3302`.
- **Constraint:** Type MUST NOT contain prohibited types (e.g., modals). Violation triggers `E-FFI-3303`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                             | Detection    | Effect    |
| :----------- | :------- | :------------------------------------ | :----------- | :-------- |
| `E-DRV-0060` | Error    | Field does not implement Debug.       | Compile-time | Rejection |
| `E-DRV-0061` | Error    | Field does not implement Clone.       | Compile-time | Rejection |
| `E-DRV-0062` | Error    | Field does not implement Eq.          | Compile-time | Rejection |
| `E-DRV-0063` | Error    | Field does not implement Hash.        | Compile-time | Rejection |
| `E-DRV-0064` | Error    | Field does not implement Default.     | Compile-time | Rejection |
| `E-DRV-0065` | Error    | Default derive on non-record type.    | Compile-time | Rejection |
| `E-DRV-0066` | Error    | Field does not implement Serialize.   | Compile-time | Rejection |
| `E-DRV-0067` | Error    | Field does not implement Deserialize. | Compile-time | Rejection |

---

### 20.7 Type Category Dispatch

##### Definition

**Type category dispatch** is a control flow mechanism within derive targets that executes different logic based on the structural category of the target type (`record`, `enum`, or `modal`).

##### Syntax & Declaration

**Grammar**

```ebnf
category_match ::= "match" "introspect" "~>" "category" "::<" type_var ">" "()" "{" category_arm+ "}"
category_arm   ::= "TypeCategory" "::" category_name "=>" block ","
category_name  ::= "Record" | "Enum" | "Modal" | "Primitive" 
                 | "Tuple" | "Array" | "Procedure" | "Reference" | "Generic"
```

##### Static Semantics

**Typing Rule**

$$\frac{
    \Gamma_{ct} \vdash T : \texttt{Type} \quad
    \forall \text{arm } C_i \Rightarrow e_i.\; \Gamma_{ct} \vdash e_i : R
}{
    \Gamma_{ct} \vdash \texttt{match introspect\textasciitilde>category::<}T\texttt{>()} \{ \ldots \} : R
} \quad \text{(T-CategoryMatch)}$$

**Exhaustiveness**

Unlike standard match expressions, `category_match` is **not** required to be exhaustive.

##### Dynamic Semantics

**Evaluation**

1. The implementation evaluates `introspect~>category::<T>()` to obtain the `TypeCategory`.
2. If a matching arm exists, its body is executed.
3. If no matching arm exists, the derive target execution halts with error `E-DRV-0070`.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                      | Detection    | Effect    |
| :----------- | :------- | :--------------------------------------------- | :----------- | :-------- |
| `E-DRV-0070` | Error    | Derive target not applicable to type category. | Compile-time | Rejection |

---

## Clause 21: Foreign Function Interface

### 21.1 The FfiSafe Class

##### Definition

The **FfiSafe class** is a marker form that classifies types whose runtime representation is compatible with the target platform's C Application Binary Interface (ABI). Only types implementing `FfiSafe` are permitted to cross the Foreign Function Interface (FFI) boundary.

**Formal Definition**

A type $T$ is **FFI-safe** if and only if the judgment $T <: \texttt{FfiSafe}$ holds. The `FfiSafe` class is defined as:

$$Cl_{\text{FfiSafe}} = (\texttt{FfiSafe}, \emptyset, \emptyset, P_{abs}, \emptyset, \emptyset, \emptyset, \emptyset, \emptyset)$$

where $P_{abs}$ contains the compile-time introspection procedures `c_size` and `c_alignment`.

##### Syntax & Declaration

**Class Declaration**

```cursive
class FfiSafe {
    comptime procedure c_size() -> usize
    comptime procedure c_alignment() -> usize
    
    comptime procedure verify_layout(size: usize, align: usize) -> bool {
        Self::c_size() == size && Self::c_alignment() == align
    }
}
```

##### Static Semantics

**Well-Formedness (WF-FfiSafe)**

A type $T$ satisfies the `FfiSafe` constraint if and only if:

1. $T$ explicitly implements `FfiSafe`.
2. $T$ has a complete, fixed memory layout.
3. $T$ does not contain any type listed in the **Prohibited Types** table below.
4. If $T$ is a `record` or `enum`, it is annotated with `[[layout(C)]]` or `[[layout(IntType)]]`.

$$\frac{
  T <: \texttt{FfiSafe} \quad
  \text{HasFixedLayout}(T) \quad
  \neg\text{ContainsProhibited}(T) \quad
  \text{LayoutCompatible}(T)
}{
  \text{FfiSafe}(T)
} \quad \text{(WF-FfiSafe)}$$

**Intrinsic Implementations**

The implementation MUST provide intrinsic `FfiSafe` implementations for the following types:

| Type Category            | Condition                           | Size / Alignment          |
| :----------------------- | :---------------------------------- | :------------------------ |
| Signed Integers          | `i8`..`i128`, `isize`               | Target C ABI              |
| Unsigned Integers        | `u8`..`u128`, `usize`               | Target C ABI              |
| Floating Point           | `f16`, `f32`, `f64`                 | IEEE 754 / Target C ABI   |
| Raw Pointers             | `*imm T`, `*mut T` (any `T`)        | Target Pointer Size       |
| Arrays                   | `[T; N]` where `T <: FfiSafe`       | $N \times \text{size}(T)$ |
| Sparse Function Pointers | All params and return are `FfiSafe` | Target Pointer Size       |

**Prohibited Types**

The following types MUST NOT implement `FfiSafe`. Any attempt to implement `FfiSafe` for these types is ill-formed (`E-FFI-3303`).

| Prohibited Category | Rationale                                     |
| :------------------ | :-------------------------------------------- |
| `bool`              | Representation is implementation-specific     |
| `char`              | Unicode scalar value (not C `char`)           |
| Modal types         | Runtime discriminants incompatible with C ABI |
| `Ptr<T>`            | Safe pointers have modal states               |
| `$Class`            | Dense pointers (data + vtable)                |
| Capability types    | Runtime capabilities are language-internal    |
| `Context`           | Root capability record is language-internal   |
| `opaque` types      | Concrete type hidden                          |
| Tuples              | Layout is unspecified                         |
| Slices `[T]`        | Dense pointers (pointer + length)             |
| String types        | Modal structure (`@Managed`/`@View`)          |
| Closures            | Dense pointers (code + environment)           |

**User-Defined Implementations**

A user-defined `record` or `enum` implements `FfiSafe` by declaring the implementation and satisfying the layout constraints.

1. The declaration MUST include `[[layout(C)]]` (for records/enums) or `[[layout(IntType)]]` (for enums).
2. All fields or variant payloads MUST be of types that implement `FfiSafe`.
3. If the type implements `Drop`, it MUST also be annotated with `[[ffi_pass_by_value]]` to permit by-value passing across FFI boundaries.

**Derivation**

The `[[derive(FfiSafe)]]` attribute generates the required `c_size` and `c_alignment` procedures using compile-time introspection.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                                 | Detection    | Effect    |
| :----------- | :------- | :-------------------------------------------------------- | :----------- | :-------- |
| `E-FFI-3301` | Error    | `FfiSafe` implemented on type without C layout attribute. | Compile-time | Rejection |
| `E-FFI-3302` | Error    | `FfiSafe` type contains non-`FfiSafe` field.              | Compile-time | Rejection |
| `E-FFI-3303` | Error    | `FfiSafe` implemented on prohibited type category.        | Compile-time | Rejection |
| `E-FFI-3304` | Error    | Generic `FfiSafe` type with unconstrained parameter.      | Compile-time | Rejection |
| `E-FFI-3305` | Error    | `FfiSafe` type depends on incomplete type.                | Compile-time | Rejection |
| `E-FFI-3306` | Error    | `Drop` + `FfiSafe` type missing `[[ffi_pass_by_value]]`.  | Compile-time | Rejection |

---

### 21.2 Foreign Procedure Declarations

##### Definition

A **foreign procedure declaration** describes a procedure whose implementation is provided by an external library. These declarations appear within `extern` blocks and define the calling convention, symbol name, and signature required to invoke the foreign code.

##### Syntax & Declaration

**Grammar**

```ebnf
extern_block        ::= attribute_list? "extern" [ string_literal ] "{" extern_item* "}"

extern_item         ::= foreign_procedure | foreign_global

foreign_procedure   ::= attribute_list? [ visibility ] "procedure" identifier 
                        "(" [ param_list ] ")" [ "->" type ]
                        [ variadic_spec ] [ foreign_contract ] ";"

variadic_spec       ::= "..." | "...," type

param_list          ::= param ( "," param )*

param               ::= identifier ":" type

foreign_contract    ::= "|=" foreign_clause ( "|=" foreign_clause )*

foreign_clause      ::= foreign_assumes | foreign_ensures
```

The `attribute_list` and `visibility` productions are defined in §7.2 and §8.5, respectively.

##### Static Semantics

**ABI Specification**

The string literal following `extern` specifies the **calling convention**. If omitted, it defaults to `"C"`. Implementations MUST support the following ABI strings:

| ABI String   | Semantics                               |
| :----------- | :-------------------------------------- |
| `"C"`        | Platform standard C calling convention  |
| `"C-unwind"` | C convention permitting stack unwinding |
| `"system"`   | Platform system call convention         |

Support for other ABI strings is Implementation-Defined (IDB). Unknown ABI strings MUST trigger diagnostic `E-FFI-3311`.

**Signature Constraints (T-Extern-Proc)**

1. All parameter types MUST satisfy `FfiSafe`.
2. The return type MUST satisfy `FfiSafe` or be the unit type `()`.
3. The procedure MUST NOT have a body.
4. The procedure MUST NOT be generic.

$$\frac{
  \texttt{extern}\ \textit{abi}\ \{\ \texttt{procedure}\ f(p_1: T_1, \ldots, p_n: T_n) \to R;\ \}
}{
  \forall i.\ T_i <: \texttt{FfiSafe} \quad R <: \texttt{FfiSafe} \lor R = ()
} \quad \text{(T-Extern-Proc)}$$

**Variadic Procedures**

A foreign procedure declaring `...` is **variadic**.

1. Fixed parameters MUST precede the `...` token.
2. If a type follows `...,` (typed variadic), all variable arguments MUST be of that type.
3. If no type follows `...` (untyped variadic), variable arguments undergo **default argument promotion** at the call site.

**Default Argument Promotion:**

| Source Type               | Promoted Type |
| :------------------------ | :------------ |
| `i8`, `i16`               | `c_int`       |
| `u8`, `u16`               | `c_int`       |
| `f32`                     | `f64`         |
| All other `FfiSafe` types | Unchanged     |

**Linking Attributes**

The `[[link(...)]]` and `[[link_name(...)]]` attributes control symbol resolution.

**`[[link(name: "lib", kind: "type")]]`**
- Applies to `extern` blocks.
- `name`: The library name to link against.
- `kind`: Optional. One of `"dylib"` (default), `"static"`, `"framework"` (macOS), `"raw-dylib"` (Windows).

**`[[link_name("symbol")]]`**
- Applies to individual foreign procedures or globals.
- Overrides the symbol name used for linking. If omitted, the Cursive identifier is used.

##### Dynamic Semantics

**Invocation**

Calling a foreign procedure is an **unsafe operation** (§3.8) and MUST occur within an `unsafe` block.

1. Arguments are evaluated left-to-right.
2. Argument values are converted to the specified ABI representation.
3. Control transfers to the external procedure.
4. Upon return, the result is converted to the Cursive return type.

**Panic Compatibility**

If the ABI is `"C"` (default), unwinding from foreign code into Cursive or vice-versa is Undefined Behavior unless the implementation guarantees compatibility. If the ABI is `"C-unwind"`, unwinding is Defined Behavior.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-FFI-3310` | Error    | Non-`FfiSafe` type in foreign procedure signature.     | Compile-time | Rejection |
| `E-FFI-3311` | Error    | Unknown ABI string in `extern` block.                  | Compile-time | Rejection |
| `E-FFI-3312` | Error    | Variadic indicator `...` not at end of parameter list. | Compile-time | Rejection |
| `E-FFI-3313` | Error    | Foreign procedure declaration contains a body.         | Compile-time | Rejection |
| `E-FFI-3320` | Error    | Foreign procedure call outside `unsafe` block.         | Compile-time | Rejection |
| `E-FFI-3321` | Error    | Non-promotable type used in untyped variadic argument. | Compile-time | Rejection |

---

### 21.3 Foreign Global Declarations

##### Definition

A **foreign global declaration** defines a binding to a global variable stored in external memory.

##### Syntax & Declaration

**Grammar**

```ebnf
foreign_global ::= attribute_list? [ visibility ] [ "mut" ] identifier ":" type ";"
```

##### Static Semantics

**Typing Rules**

1. The declared type $T$ MUST satisfy `FfiSafe`.
2. If `mut` is present, the binding is mutable; otherwise, it is read-only.

**Access restrictions**

Accessing a foreign global (read or write) is an **unsafe operation** and MUST occur within an `unsafe` block.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                         | Detection    | Effect    |
| :----------- | :------- | :------------------------------------------------ | :----------- | :-------- |
| `E-FFI-3330` | Error    | Non-`FfiSafe` type in foreign global declaration. | Compile-time | Rejection |
| `E-FFI-3331` | Error    | Assignment to non-`mut` foreign global.           | Compile-time | Rejection |
| `E-FFI-3332` | Error    | Access to foreign global outside `unsafe` block.  | Compile-time | Rejection |

---

### 21.4 Exported Procedures

##### Definition

An **exported procedure** is a Cursive procedure exposed for invocation by foreign code.

##### Syntax & Declaration

**Grammar**

An exported procedure is a standard procedure declaration annotated with the `[[export]]` attribute.

```ebnf
export_attr ::= "[[" "export" "(" string_literal [ "," export_options ] ")" "]]"
export_options ::= "link_name" ":" string_literal
```

The string literal argument specifies the calling convention (ABI), using the same values as `extern` blocks.

##### Static Semantics

**Well-Formedness**

An exported procedure declaration is well-formed if and only if:

1. It has `public` visibility.
2. All parameter types satisfy `FfiSafe`.
3. The return type satisfies `FfiSafe` or is `()`.
4. It is not generic.
5. It does not accept `Context`, capabilities, or `dyn` types as parameters.

**(Export-Blindness)**

The procedure signature MUST NOT contain any types that carry Cursive runtime authority or capabilities. This is the **Capability Blindness Rule**.

**Symbol Name**

If `link_name` is specified, the procedure is exported with that symbol name. Otherwise, the procedure's identifier is used as the un-mangled symbol name.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                              | Detection    | Effect    |
| :----------- | :------- | :----------------------------------------------------- | :----------- | :-------- |
| `E-FFI-3350` | Error    | `[[export]]` applied to non-`public` procedure.        | Compile-time | Rejection |
| `E-FFI-3351` | Error    | Capability, `Context`, or `dyn` type in export params. | Compile-time | Rejection |
| `E-FFI-3352` | Error    | Non-`FfiSafe` type in export signature.                | Compile-time | Rejection |
| `E-FFI-3354` | Error    | Duplicate exported symbol name.                        | Link-time    | Rejection |

---

### 21.5 Capability Isolation

##### Definition

**Capability Isolation** defines the constraints preventing the leakage of Cursive's authority model into foreign code.

##### Static Semantics

**Isolation Invariants**

1. **Blindness:** Foreign code MUST NOT possess values of capability types.
2. **Context Integrity:** The `Context` record MUST NOT be passed to foreign code.
3. **Region Confinement:** Pointers to region-allocated storage (§3.7) MUST NOT be passed to foreign code, as the foreign code cannot participate in the region's lifetime discipline.

**Verification**

The implementation MUST enforce these invariants via:
1. The **Signature Constraints** defined in §21.4 (prohibiting capability types in exports).
2. **Escape Analysis** on pointers passed to `extern` procedures. If a pointer derives from a region allocation, the call is ill-formed.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                           | Detection    | Effect    |
| :----------- | :------- | :---------------------------------- | :----------- | :-------- |
| `E-FFI-3360` | Error    | Region-local pointer passed to FFI. | Compile-time | Rejection |

---

#### 21.6 Foreign Contracts

##### Definition

Foreign contracts specify preconditions (`@foreign_assumes`) and trusted postconditions (`@foreign_ensures`) for FFI procedures.

##### Syntax & Declaration

```ebnf
foreign_contract ::= "|=" clause+
clause           ::= "@foreign_assumes" "(" pred_list ")" 
                   | "@foreign_ensures" "(" ensure_list ")"

ensure_list      ::= ensure_pred ( "," ensure_pred )*
ensure_pred      ::= predicate
                   | "@error" ":" predicate
                   | "@null_result" ":" predicate
```

##### Static Semantics

**Postconditions (`@foreign_ensures`)**

These predicates are **trusted axioms**. The compiler assumes they are true after the call returns.
*   **`@result`**: Binds to the return value.
*   **`@error`**: Predicate holds only if the return value indicates failure (e.g., is -1 or null).
*   **`@null_result`**: Predicate holds specifically when `@result == null`.

**Verification Modes**

1.  **`[[static]]`** (Default): `@foreign_assumes` MUST be proven at compile time.
2.  **`[[dynamic]]`**: `@foreign_assumes` checked at runtime before call. `@foreign_ensures` checked at runtime after call.

##### Constraints & Legality

**Diagnostic Table**

| Code         | Severity | Condition                                       | Detection    | Effect    |
| :----------- | :------- | :---------------------------------------------- | :----------- | :-------- |
| `E-CON-2853` | Error    | Invalid predicate syntax in foreign contract.   | Compile-time | Rejection |
| `E-CON-2855` | Error    | `@error` predicate on void-returning procedure. | Compile-time | Rejection |

---

### 21.7 Platform Type Aliases

##### Definition

The implementation MUST provide the following type aliases in the `std::ffi` module (or equivalent core library location), mapping to the specific integer types defined by the target platform's C ABI.

| Alias         | Definition Constraint |
| :------------ | :-------------------- |
| `c_char`      | `i8` or `u8`          |
| `c_schar`     | `i8`                  |
| `c_uchar`     | `u8`                  |
| `c_short`     | `i16`                 |
| `c_ushort`    | `u16`                 |
| `c_int`       | `i32`                 |
| `c_uint`      | `u32`                 |
| `c_long`      | IDB (32 or 64 bit)    |
| `c_ulong`     | IDB (32 or 64 bit)    |
| `c_longlong`  | `i64`                 |
| `c_ulonglong` | `u64`                 |
| `c_float`     | `f32`                 |
| `c_double`    | `f64`                 |
| `c_size_t`    | `usize`               |
| `c_ssize_t`   | `isize`               |
| `c_void`      | `opaque c_void`       |

**Note:** `c_char` signedness is Implementation-Defined to match the target platform default.

---

## Appendix A: Formal Grammar (Normative)

##### Definition

This appendix describes the structure of the normative ANTLR4 grammar.

**Grammar Structure**
- **Lexer Rules:** Keywords, identifiers (XID_START/XID_Continue), literals (integer/float/string/char), operators, comments
- **Parser Rules:** All declarations (record, enum, modal, class, procedure), expressions (precedence-encoded), statements, patterns
- **Operator Precedence:** Encoded in grammar hierarchy (14 levels from Postfix to Assignment)
- **Comment Nesting:** Block comments (`/* ... */`) MUST nest recursively; lexer maintains nesting counter
- **Maximal Munch Exception:** Generic argument exception per §2.7 (context-sensitive lexing)

##### Constraints & Legality

- Keywords are reserved per §2.6.
- Receiver shorthands (`~`, `~%`, `~!`) MUST appear only as first parameter in type method declarations.
- All lexical rules MUST conform to preprocessing pipeline output (§2.1).

---

## Appendix B: Diagnostic Code Taxonomy (Normative)

##### Definition

This appendix defines the normative taxonomy for diagnostics and the authoritative allocation of diagnostic code ranges.

#### B.1 Diagnostic Code Format

Codes follow `K-CAT-FFNN`:
*   **K**: `E` (Error), `W` (Warning), `N` (Note), `P` (Panic).
*   **CAT**: Category (e.g., `TYP`, `MEM`).
*   **FF**: Feature Bucket.
*   **NN**: Unique Number.

#### B.2 Code Range Allocation

| Prefix        | Range     | Clause/Domain                           |
| :------------ | :-------- | :-------------------------------------- |
| `E-CNF-`      | 0100–0299 | Clause 1: Conformance (general)         |
| `E-CNF-`      | 0300–0399 | Clause 1: Implementation Limits         |
| `E-SRC-`      | 0100–0399 | Clause 2: Source Text & Lexical         |
| `E-SYN-`      | 0100–0199 | Clause 2: Syntactic Nesting             |
| `E-SYN-`      | 0700–0799 | Clause 7: Generic Parameter Syntax      |
| `E-MEM-`      | 3000–3099 | Clause 3: Memory Model                  |
| `E-TYP-`      | 1500–1599 | Clause 4: Type System Foundations       |
| `E-TYP-`      | 1600–1699 | Clause 4: Permission System             |
| `E-TYP-`      | 1700–1799 | Clause 5: Primitive Types               |
| `E-TYP-`      | 1800–1849 | Clause 5: Composite Types (Tuple/Array) |
| `E-TYP-`      | 1850–1899 | Clause 5: Composite Types (Slice/Range) |
| `E-TYP-`      | 1900–1949 | Clause 5: Records                       |
| `E-TYP-`      | 1950–1999 | Clause 5: Enums                         |
| `E-TYP-`      | 2000–2049 | Clause 5: Union Types                   |
| `E-TYP-`      | 2050–2099 | Clause 6: Modal Types                   |
| `E-TYP-`      | 2100–2149 | Clause 6: Pointer Types                 |
| `E-TYP-`      | 2150–2199 | Clause 6: String Type                   |
| `E-TYP-`      | 2200–2249 | Clause 6: Function Types                |
| `E-TYP-`      | 2300–2399 | Clause 7: Generics                      |
| `E-DEC-`      | 2400–2499 | Clause 8: Declarations                  |
| `E-NAM-`      | 1300–1399 | Clause 9: Name Resolution               |
| `E-EXP-`      | 2500–2599 | Clause 12: Expressions                  |
| `E-STM-`      | 2630–2639 | Clause 12: Assignment Statements        |
| `E-STM-`      | 2650–2659 | Clause 12: Defer Statements             |
| `E-STM-`      | 2660–2669 | Clause 12: Control Flow Statements      |
| `E-PAT-`      | 2710–2759 | Clause 12: Patterns                     |
| `E-TRS-`      | 2900–2999 | Clause 9: Forms                         |
| `E-CON-`      | 3200–3299 | Clause 11: Contracts                    |
| `E-KEY-`      | 0001–0099 | Clause 14: Key System                   |
| `E-SPAWN-`    | 0001–0099 | Clause 15: Task Spawning                |
| `E-ASYNC-`    | 0001–0099 | Clause 16: Async Operations             |
| `E-DISPATCH-` | 0001–0099 | Clause 15: Data Parallelism             |
| `E-WAIT-`     | 0001–0099 | Clause 16: Wait Expressions             |
| `W-KEY-`      | 0001–0099 | Clause 14: Key Warnings                 |
| `W-DYN-`      | 0001–0099 | Clause 7: `[[dynamic]]` Attribute       |

#### B.4 Conflict Resolution

1. Type System (`E-TYP-`) diagnostics are canonical for permission/type violations.
2. Memory Model (`E-MEM-`) diagnostics are canonical for lifetime/move violations.
3. Source (`E-SRC-`) diagnostics are canonical for lexical violations.

---

### Appendix C: C Type Mapping Reference

##### Definition

This appendix provides the normative mapping between C types and their Cursive equivalents for FFI interoperability.

| C Type               | Cursive Type                               | Notes                         |
| :------------------- | :----------------------------------------- | :---------------------------- |
| `void`               | `()` (return) or `opaque c_void` (pointer) |                               |
| `_Bool`              | `CBool`                                    | NOT `bool`                    |
| `char`               | `c_char`                                   | Platform-dependent signedness |
| `signed char`        | `c_schar` / `i8`                           |                               |
| `unsigned char`      | `c_uchar` / `u8`                           |                               |
| `short`              | `c_short` / `i16`                          |                               |
| `unsigned short`     | `c_ushort` / `u16`                         |                               |
| `int`                | `c_int` / `i32`                            |                               |
| `unsigned int`       | `c_uint` / `u32`                           |                               |
| `long`               | `c_long`                                   | IDB: 32 or 64 bits            |
| `unsigned long`      | `c_ulong`                                  | IDB: 32 or 64 bits            |
| `long long`          | `c_longlong` / `i64`                       |                               |
| `unsigned long long` | `c_ulonglong` / `u64`                      |                               |
| `size_t`             | `c_size_t` / `usize`                       |                               |
| `ssize_t`            | `c_ssize_t` / `isize`                      |                               |
| `ptrdiff_t`          | `c_ptrdiff_t` / `isize`                    |                               |
| `float`              | `c_float` / `f32`                          |                               |
| `double`             | `c_double` / `f64`                         |                               |
| `T*`                 | `*mut T` or `*imm T`                       |                               |
| `const T*`           | `*imm T`                                   |                               |
| `T[]` / `T*` (array) | `BufferView<T>` / `BufferMut<T>`           |                               |
| `char*` (string)     | `CStr` / `CString`                         |                               |
| `void*`              | `*mut opaque c_void`                       |                               |
| Function pointer     | Sparse function type                       |                               |

---

### Appendix D: Conformance Dossier Schema

##### Definition

A conforming implementation MUST produce a `dossier` artifact in JSON format.

#### D.1 Required Information

**Metadata:**
- Implementation identifier (vendor name)
- Implementation version (semantic versioning)
- Target triple (architecture-vendor-os)
- Build timestamp

**Configuration:**
- Conformance mode (`strict` or `permissive`)
- List of enabled language feature flags

**Safety Report:**
- Count of `unsafe` blocks
- List of IFNDR instances (file path, line number, category)

**Implementation-Defined Behavior:**
- Pointer width (32 or 64 bits)
- Type layout map for primitive types (size and alignment)

**Implementation Limits:**
- Maximum recursion depth
- Maximum identifier length
- Maximum source file size