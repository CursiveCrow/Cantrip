# The Cursive Language Specification

**Part**: II — Basics and Program Model
**File**: 02-1_Source-Text-Encoding.md
**Section**: 2.1 Source Text Encoding
**Previous**: [Translation Phases](02-0_Translation-Phases.md) | **Next**: [Lexical Elements](02-2_Lexical-Elements.md)

---

## 2.1 Source Text Encoding

This section specifies the character encoding, structure, and processing rules for Cursive source files. All source text processing begins with validation and normalization as specified herein.

---

### 2.1.1 Character Encoding (UTF-8)

**Definition 2.1 (Source File):** A **source file** is a sequence of Unicode scalar values encoded in UTF-8.

**NORMATIVE REQUIREMENT:**

A conforming implementation SHALL accept only UTF-8 encoded source files. Source files encoded in any other character encoding SHALL be rejected with a diagnostic.

**UTF-8 Validation:**

The implementation SHALL validate that the source file constitutes a valid UTF-8 byte sequence before any further processing. Invalid UTF-8 byte sequences SHALL result in a compile-time diagnostic.

**Diagnostic:**

```
error[E2001]: invalid UTF-8 encoding in source file
  --> src/example.cur:1:1
   |
   | <invalid byte sequence>
   | ^^^^^^^^^^^^^^^^^^^^^^^
   |
   = note: source files must be encoded in UTF-8
   = help: verify file encoding and re-save as UTF-8
```

**Formal Specification:**

```
[UTF8-Valid]
source file F
byte sequence B = read(F)
B is valid UTF-8
---------------------------------
F is accepted for further processing

[UTF8-Invalid]
source file F
byte sequence B = read(F)
B contains invalid UTF-8 sequences
---------------------------------
ERROR E2001: Invalid UTF-8 encoding
```

**Unicode Scalar Values:**

After UTF-8 decoding, the source text is represented as a sequence of Unicode scalar values in the range U+0000 to U+D7FF and U+E000 to U+10FFFF (excluding surrogate code points U+D800 to U+DFFF).

**Unicode Version:**

Implementations SHOULD support Unicode 15.0 or later. Implementations MAY support earlier Unicode versions but SHALL document the supported version.

---

### 2.1.2 Source File Structure

**Definition 2.2 (Compilation Unit):** A **compilation unit** is the textual content of a single source file after UTF-8 decoding and line ending normalization.

**File Extension:**

Source files SHOULD use the file extension `.cursive`. Implementations MAY accept source files with other extensions but SHALL document this behavior.

**Example file names:**
- `main.cursive`
- `geometry.cursive`
- `utils.cursive`

**File Size:**

The maximum source file size is implementation-defined. Implementations SHOULD support source files of at least 1 megabyte (1,048,576 bytes). Implementations MAY impose larger or smaller limits but SHALL document the limit.

**NORMATIVE RECOMMENDATION:**

Source files SHOULD be kept under 1 megabyte for optimal compilation performance and maintainability.

**Diagnostic for oversized files:**

```
error[E2002]: source file exceeds maximum size
  --> src/large.cur:1:1
   |
   = note: file size: 2.5 MB
   = note: maximum supported: 1.0 MB
   = help: consider splitting into multiple modules
```

**Unicode Normalization:**

Source text Unicode normalization form is implementation-defined. Implementations SHOULD accept source text in any Unicode normalization form (NFC, NFD, NFKC, NFKD). Implementations MAY normalize source text to NFC during processing but are not required to do so.

**NORMATIVE RECOMMENDATION:**

Source files SHOULD be stored in Unicode Normalization Form C (NFC) for maximum interoperability between implementations and text editors.

**Rationale:**

Different text editors and operating systems may produce source files in different normalization forms. Requiring implementations to accept all normalization forms ensures portability. Recommending NFC provides a standard practice without imposing mandatory normalization overhead.

---

### 2.1.3 Line Endings

**Definition 2.3 (Line Ending):** A **line ending** is a character sequence that terminates a line of source text.

**Accepted Line Ending Sequences:**

The following line ending sequences SHALL be accepted:

| Sequence | Name | Unicode | Byte Sequence (UTF-8) |
|----------|------|---------|----------------------|
| LF | Line Feed | U+000A | 0x0A |
| CRLF | Carriage Return + Line Feed | U+000D U+000A | 0x0D 0x0A |
| CR | Carriage Return | U+000D | 0x0D |

**NORMATIVE REQUIREMENT:**

An implementation SHALL accept source files containing any combination of the above line ending sequences, including mixed line endings within a single file.

**Line Ending Normalization:**

During lexical analysis ([REF_TBD] Lexical Analysis), all line ending sequences SHALL be normalized to a single Line Feed (LF, U+000A) character.

**Normalization Algorithm:**

```
Algorithm: normalize_line_endings(input: string) -> string
Input: input - raw source text with arbitrary line endings
Output: normalized text with all line endings as LF

1. Let result = empty string
2. Let i = 0
3. While i < length(input):
   a. If input[i..i+2] == CRLF (U+000D U+000A):
      - Append LF (U+000A) to result
      - Set i = i + 2
      - Continue to step 3

   b. Else if input[i] == CR (U+000D):
      - Append LF (U+000A) to result
      - Set i = i + 1
      - Continue to step 3

   c. Else:
      - Append input[i] to result
      - Set i = i + 1
      - Continue to step 3

4. Return result
```

**Formal Specification:**

```
[LineEnd-Normalize]
raw source text T
T contains line endings {LF, CRLF, CR}
T' = normalize_line_endings(T)
---------------------------------
All line endings in T' are LF

[LineEnd-Order]
Normalization preserves:
- Character order (except line ending substitution)
- Character count (modulo line ending length changes)
- Line count (number of lines unchanged)
```

**Newline Significance:**

Newlines (after normalization to LF) are **significant tokens** in Cursive. They participate in automatic statement termination rules ([REF_TBD] Statement Termination).

**NORMATIVE STATEMENT:**

> "Line endings, after normalization to LF, are preserved as tokens during lexical analysis and participate in statement termination rules."

**Mixed Line Endings:**

Source files containing mixed line ending types (e.g., some lines ending with LF, others with CRLF) are explicitly permitted and SHALL be processed correctly after normalization.

**Example:**

```cursive
// Source file with mixed line endings (visualized):
let x = 5<CRLF>
let y = 10<LF>
let z = 15<CR>

// After normalization:
let x = 5<LF>
let y = 10<LF>
let z = 15<LF>
```

---

### 2.1.4 Byte Order Mark Handling

**Definition 2.4 (Byte Order Mark):** The **Byte Order Mark (BOM)** is the Unicode character U+FEFF, which may appear at the beginning of a UTF-8 encoded file. The UTF-8 encoding of the BOM is the byte sequence 0xEF 0xBB 0xBF.

**BOM at File Start (Optional):**

If a UTF-8 BOM appears at the beginning of a source file, the implementation SHALL ignore it. The BOM SHALL NOT be considered part of the source text.

**NORMATIVE REQUIREMENT:**

> "If a source file begins with the UTF-8 BOM (0xEF 0xBB 0xBF), the BOM shall be stripped before lexical analysis and shall not contribute to the token stream."

**Formal Specification:**

```
[BOM-Start-Ignore]
source file F
first 3 bytes of F = {0xEF, 0xBB, 0xBF}
---------------------------------
BOM is removed before lexical analysis
Source text begins at byte 3

[BOM-Absent]
source file F
first 3 bytes of F ≠ {0xEF, 0xBB, 0xBF}
---------------------------------
No BOM processing occurs
Source text begins at byte 0
```

**BOM in Non-Initial Position (Error):**

If the UTF-8 BOM appears anywhere other than the beginning of the file, it SHALL be treated as an invalid character and result in a compile-time diagnostic.

**NORMATIVE REQUIREMENT:**

> "A BOM occurring at any position other than the start of the file is an invalid character and shall result in a diagnostic."

**Diagnostic:**

```
error[E2003]: byte order mark in invalid position
  --> src/example.cur:5:1
   |
 5 | let x = 5
   | ^ BOM character (U+FEFF) not allowed here
   |
   = note: BOM is only valid at the start of the file
   = help: remove the BOM or ensure it only appears at file start
```

**Formal Specification:**

```
[BOM-Invalid-Position]
source file F after initial BOM check
character U+FEFF appears at position p > 0
---------------------------------
ERROR E2003: BOM in invalid position
```

**Rationale:**

The BOM is a metadata indicator for text editors and should not appear in source text. Allowing it only at the file start accommodates editors that automatically insert BOMs while preventing accidental BOM characters in source code.

**BOM Recommendation:**

While conforming implementations MUST accept files with or without a BOM, source files SHOULD be stored without a BOM for maximum portability and clarity.

---

### 2.1.5 Character Restrictions

**NUL Byte Prohibition:**

The NUL character (U+0000) SHALL NOT appear in source text at any position. Source files containing NUL bytes SHALL be rejected with a compile-time diagnostic.

**NORMATIVE REQUIREMENT:**

> "Source files shall not contain NUL bytes (U+0000). A NUL byte at any position is a lexical error."

**Diagnostic:**

```
error[E2004]: NUL byte in source text
  --> src/example.cur:10:15
   |
10 | let data = "text"
   |               ^ NUL byte (U+0000) not allowed in source
   |
   = note: NUL bytes are prohibited in Cursive source files
   = help: remove or replace the NUL byte
```

**Formal Specification:**

```
[Char-No-NUL]
source file F after UTF-8 decoding
F contains character U+0000 at any position
---------------------------------
ERROR E2004: NUL byte in source text
```

**Rationale:**

NUL bytes can cause issues with string processing, text editors, and version control systems. Prohibiting them ensures consistent source file handling across tools.

**Control Character Restrictions:**

Control characters (Unicode categories Cc, excluding specified exceptions) SHALL NOT appear in source text outside of string literals and character literals.

**Permitted Control Characters (Outside Literals):**

The following control characters MAY appear in source text outside of literals:

| Character | Name | Unicode | Purpose |
|-----------|------|---------|---------|
| Horizontal Tab | HT | U+0009 | Whitespace, indentation |
| Line Feed | LF | U+000A | Line termination (after normalization) |
| Carriage Return | CR | U+000D | Line termination (before normalization) |
| Form Feed | FF | U+000C | Whitespace (rare) |

**Prohibited Control Characters (Outside Literals):**

All other control characters (U+0001-U+0008, U+000B, U+000E-U+001F, U+007F-U+009F) SHALL NOT appear in source text outside of string and character literals.

**NORMATIVE REQUIREMENT:**

> "Control characters other than horizontal tab (U+0009), line feed (U+000A), carriage return (U+000D), and form feed (U+000C) are disallowed outside string and character literals."

**Diagnostic:**

```
error[E2005]: invalid control character in source text
  --> src/example.cur:7:12
   |
 7 | let value = result
   |            ^ control character U+0007 (BEL) not allowed here
   |
   = note: control characters are only allowed inside string/char literals
   = help: remove the control character or place it in a string literal
```

**Formal Specification:**

```
[Char-Control-Restricted]
source position p outside string literal or character literal
character c at position p
c ∈ ControlCharacters ∧ c ∉ {U+0009, U+000A, U+000D, U+000C}
---------------------------------
ERROR E2005: Invalid control character

where ControlCharacters = {U+0001..U+001F, U+007F..U+009F}
```

**Within String and Character Literals:**

Inside string literals and character literals, all Unicode scalar values (including control characters) MAY appear, either directly or via escape sequences ([REF_TBD] String Literals).

**Example:**

```cursive
// ✅ OK: Control character via escape sequence
let bell = "\x07"  // U+0007 BEL character

// ✅ OK: Control character literal in string
let tab = "\t"  // U+0009 TAB via escape

// ❌ ERROR: Control character outside literal
let x = 5  // (assume U+0007 appears here in source)
```

---

### 2.1.6 Canonical Example

This comprehensive example demonstrates all aspects of source text encoding:

```cursive
// ========== example.cursive ==========
// UTF-8 encoding required
// BOM: Optional at start, stripped if present
// Line endings: LF, CRLF, or CR (all normalized to LF)

// ========== UTF-8 Multi-byte Characters ==========
// Copyright © 2025 Cursive™
// Mathematical symbols: π ≈ 3.14159, ∞, ∑, ∫
// Greek: Δ (delta), Σ (sigma), Ω (omega)
// Emoji: 🚀 ✅ ❌

// ========== Line Ending Demonstration ==========
// This comment ends with LF
let x = 5
// Above line may have LF, CRLF, or CR - all normalized

// ========== Character Restrictions ==========
// ✅ OK: Permitted control characters
let tabs = "\t\t\t"  // Tab characters in string
let newline = "\n"   // LF in string

// ✅ OK: Horizontal tab for indentation
function example() {
	let indented = 42  // Tab character (U+0009) allowed
}

// ❌ ERROR: NUL byte not allowed
// let invalid = ""  // If U+0000 appeared here: E2004

// ❌ ERROR: Control characters outside literals
// let bell = // If U+0007 appeared here: E2005

// ========== Valid Unicode Identifiers ==========
let 変数 = "Japanese"  // XID_Start/XID_Continue
let переменная = "Russian"
let μετάβλητη = "Greek"

// ========== String Literals (Control Chars OK) ==========
let control_chars = "Bell:\x07 Escape:\x1B Null:\x00"  // ✅ OK

// ========== Maximum File Size ==========
// This file should be under 1 MB (recommended)
// If larger: warning or error depending on implementation

// ========== Unicode Normalization ==========
// The following two identifiers are distinct if NFC != NFD:
// "café" (NFC: U+0063 U+0061 U+0066 U+00E9)
// "café" (NFD: U+0063 U+0061 U+0066 U+0065 U+0301)
// Implementations should accept both forms

function main(): i32 {
    // ========== BOM Handling ==========
    // If this file started with UTF-8 BOM (EF BB BF):
    // - BOM stripped before parsing
    // - This comment starts at byte 0 (after BOM removal)

    // ========== Line Ending Normalization ==========
    let multiline = "
        Line 1
        Line 2
        Line 3
    "  // Internal newlines preserved as LF

    println("UTF-8 source encoding: ✅")
    result 0
}

// ========== Error Code Summary ==========
// E2001: Invalid UTF-8 encoding
// E2002: File size exceeds limit (implementation-defined)
// E2003: BOM in invalid position
// E2004: NUL byte in source
// E2005: Invalid control character outside literal

// ========== Processing Pipeline ==========
// 1. Read file as byte sequence
// 2. Validate UTF-8 (E2001 if invalid)
// 3. Check file size (E2002 if over limit)
// 4. Remove BOM if present at start
// 5. Normalize line endings (CRLF/CR -> LF)
// 6. Validate character restrictions (E2004, E2005)
// 7. Proceed to lexical analysis

/* After processing this file:
   - UTF-8 validated ✓
   - BOM removed (if present) ✓
   - Line endings normalized to LF ✓
   - NUL bytes checked ✓
   - Control characters validated ✓
   - Ready for tokenization
*/
```

---

### 2.1.7 Error Code Summary

| Error Code | Description | Context |
|------------|-------------|---------|
| **E2001** | Invalid UTF-8 encoding | Byte sequence is not valid UTF-8 |
| **E2002** | Source file exceeds maximum size | File larger than implementation limit |
| **E2003** | Byte Order Mark in invalid position | BOM appears after file start |
| **E2004** | NUL byte in source text | U+0000 appears anywhere in source |
| **E2005** | Invalid control character | Prohibited control char outside literals |

---

### 2.1.8 Conformance Requirements Summary

A conforming implementation MUST:

1. Accept only UTF-8 encoded source files
2. Validate UTF-8 encoding before further processing
3. Accept all three line ending types (LF, CRLF, CR) and mixed combinations
4. Normalize all line endings to LF during lexical analysis
5. Ignore UTF-8 BOM if present at file start
6. Reject BOM at non-initial positions with diagnostic
7. Reject source files containing NUL bytes (U+0000)
8. Reject prohibited control characters outside literals
9. Accept horizontal tab, line feed, carriage return, and form feed in source text
10. Support source files of at least 1 megabyte (recommended)

A conforming implementation SHOULD:

1. Support Unicode 15.0 or later
2. Accept source files in any Unicode normalization form
3. Recommend NFC for source file storage
4. Use file extension `.cursive` for source files
5. Document implementation-defined maximum file size
6. Document supported Unicode version

A conforming implementation MAY:

1. Accept source files with extensions other than `.cursive`
2. Normalize source text to NFC during processing
3. Support larger source files than the 1 MB recommendation
4. Provide warnings for non-NFC normalized source files

---

**Previous**: [Translation Phases](02-0_Translation-Phases.md) | **Next**: [Lexical Elements](02-2_Lexical-Elements.md)

**END OF SECTION 2.1**
