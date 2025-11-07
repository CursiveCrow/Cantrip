# The Cursive Language Specification

**Part**: II — Lexical Structure and Translation
**File**: 02-1_Source-Text-Encoding.md  
**Section**: 2.1 Source Text and Encoding  
**Stable label**: [lex.source]  
**Forward references**: §2.2 [lex.phases], §2.3 [lex.tokens], Annex A §A.2 [grammar.lexical]

---

### §2.1.1 Overview

[1] Cursive source input is a sequence of Unicode scalar values that shall be encoded as UTF-8. All subsequent phases (§2.2 [lex.phases]) assume that the implementation has normalised line endings and removed optional metadata such as byte order marks before tokenisation (§2.3 [lex.tokens]).

[2] A _source file_ is the decoded Unicode scalar stream after UTF-8 validation and line-ending normalisation. A _compilation unit_ is the textual content of a single source file after this preprocessing step.

### §2.1.2 Syntax

[1] Source files conform to the following lexical skeleton prior to tokenisation:

```ebnf
source_file
    ::= bom? normalized_line*

normalized_line
    ::= code_point* line_terminator?

line_terminator
    ::= "\n"

bom
    ::= "\uFEFF"
```

[ Note: See Annex A §A.2 [grammar.lexical] for the complete lexical grammar including source file structure.
— end note ]

[2] `code_point` denotes any Unicode scalar value other than control characters disallowed by constraint [4].

### §2.1.3 Constraints

[1] _UTF-8 validation._ Implementations shall accept only byte streams that decode to legal UTF-8 sequences. Invalid byte sequences shall elicit diagnostic E02-001.

[2] _Byte order mark handling._ If the byte stream begins with U+FEFF, the implementation shall strip the BOM before any further analysis. Any occurrence of U+FEFF after the first decoded scalar value shall trigger diagnostic E02-003.

[3] _Line endings._ Implementations shall recognise LF, CR, and CRLF sequences and normalise each to a single LF code point before tokenisation. Mixed line endings are permitted; the normalisation process preserves the number of logical lines.

[4] _Prohibited code points._ Outside string and character literals, only horizontal tab (U+0009), line feed (U+000A), carriage return (U+000D), and form feed (U+000C) are permitted from Unicode category Cc. U+0000 is forbidden everywhere and shall raise diagnostic E02-004.

[5] _File size._ Implementations shall document an implementation-defined maximum source size; they shall accept files of at least 1 MiB. Files exceeding the implementation limit shall raise diagnostic E02-002.

### §2.1.4 Semantics

[1] After constraint checks succeed, the normalised LF characters become significant tokens that participate in automatic statement termination (§2.4 [lex.terminators]). Horizontal tabs and spaces serve as token separators but are otherwise ignored by lexical analysis.

[2] Unicode normalisation is implementation-defined. Implementations should accept any of NFC, NFD, NFKC, or NFKD input; if additional normalisation is performed, it shall not alter source line boundaries or byte offsets used for diagnostics.

### §2.1.5 Examples

**Example 2.1.5.1 (Valid Source Structure):**

```cursive
//! Module overview — documentation comment
let greeting = "Hello, world"  // UTF-8 literal
let delta = "Δ"                // Non-ASCII scalar value
```

[1] The example uses LF line endings, no BOM, and only permitted control characters, so it satisfies constraints [1]–[4].

**Example 2.1.5.2 - invalid (Embedded BOM):**

```cursive
let x = 1
\uFEFFlet y = 2  // error[E02-003]
```

[2] The second line embeds U+FEFF after decoding, violating constraint [2].

### §2.1.6 Conformance Requirements

[1] Implementations shall document the maximum supported source-file size and the Unicode version they validate against.

[2] Diagnostics E02-001–E02-004 shall be emitted at the location of the offending byte sequence or code point. Suggestions in diagnostic text are informative; the failure remains normative.

[3] Implementations may issue warnings when large files or mixed normalisation forms are detected, but such warnings shall not replace the mandatory diagnostics described above.
