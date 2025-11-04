# The Cursive Language Specification

**Part**: II — Basics and Program Model
**File**: 02-2_Lexical-Elements.md
**Section**: 2.2 Lexical Elements
**Previous**: [Source Text Encoding](02-1_Source-Text-Encoding.md) | **Next**: [Syntax Notation](02-3_Syntax-Notation.md)

---

## 2.2 Lexical Elements

This section specifies the lexical structure of Cursive source text, defining the token types, lexical analysis process, and token formation rules. Lexical analysis transforms source text into a token stream for syntactic analysis.

---

### 2.2.1 Lexical Analysis Overview

**Definition 2.5 (Lexical Analysis):** **Lexical analysis** (or tokenization) is the process of converting a sequence of characters into a sequence of tokens.

**Definition 2.6 (Token):** A **token** is an atomic lexical unit with a type, lexeme (textual representation), and source location.

**Lexical Analysis Process:**

```
Input:  Character stream (after UTF-8 decoding and line ending normalization)
Output: Token stream

Process:
1. Skip whitespace (except newlines, which become tokens)
2. Strip non-documentation comments
3. Identify next token using maximal munch rule
4. Record token type, lexeme, and source location
5. Repeat until end of input
```

**Token Properties:**

Each token has three essential properties:

| Property | Description | Example |
|----------|-------------|---------|
| **Type** | Token classification | `IDENTIFIER`, `KEYWORD`, `INTEGER_LITERAL` |
| **Lexeme** | Source text representation | `"example"`, `"let"`, `"42"` |
| **Location** | Source position (file, line, column) | `src/main.cur:15:8` |

**Relationship to Syntax:**

> "Lexical analysis produces the token stream consumed by syntactic analysis ([REF_TBD] Translation Phases). The lexical grammar is independent of the syntactic grammar."

---

### 2.2.2 Input Elements and Token Classification

**Token Categories:**

Cursive tokens are classified into the following categories:

```ebnf
Token ::=
    Identifier
  | Keyword
  | Literal
  | Operator
  | Punctuator
  | Newline
```

**Note:** For the complete lexical grammar, see Annex A.1.

**Token Category Descriptions:**

| Category | Description | Examples |
|----------|-------------|----------|
| **Identifier** | Names for entities | `variable`, `myFunction`, `Point` |
| **Keyword** | Reserved words | `let`, `function`, `if`, `match` |
| **Literal** | Constant values | `42`, `3.14`, `"hello"`, `'x'`, `true` |
| **Operator** | Operation symbols | `+`, `-`, `*`, `/`, `==`, `&&` |
| **Punctuator** | Structural symbols | `(`, `)`, `{`, `}`, `,`, `;`, `:` |
| **Newline** | Line terminator (significant) | `\n` (after normalization) |

**Whitespace and Comments:**

Whitespace (excluding newlines) and non-documentation comments are **not** tokens. They are discarded during lexical analysis but serve as token separators.

**NORMATIVE STATEMENT:**

> "Whitespace characters other than newline, and non-documentation comments, are not tokens. They separate tokens but do not appear in the token stream."

---

### 2.2.3 Whitespace Characters

**Definition 2.7 (Whitespace):** **Whitespace** consists of characters that separate tokens but carry no semantic meaning (except newlines).

**Whitespace Characters:**

The following characters are classified as whitespace:

| Character | Name | Unicode | Purpose |
|-----------|------|---------|---------|
| Space | SP | U+0020 | General whitespace |
| Horizontal Tab | HT | U+0009 | Indentation, alignment |
| Form Feed | FF | U+000C | Page break (rare) |

**Newline Special Status:**

The newline character (U+000A) after line ending normalization ([REF_TBD] Line Endings) is **not** discarded. Newlines are preserved as tokens for statement termination ([REF_TBD] Statement Termination).

**NORMATIVE REQUIREMENT:**

> "Horizontal tab (U+0009), space (U+0020), and form feed (U+000C) shall be recognized as whitespace and shall separate tokens but shall not themselves be tokens."

**Whitespace as Token Separator:**

Whitespace is required to separate tokens that would otherwise merge:

```cursive
// ✅ Correct: Whitespace separates tokens
let x = 5

// ❌ Incorrect: Would be parsed as one token "letx"
letx=5
```

**Whitespace Within Tokens:**

Whitespace SHALL NOT appear within a token (except within string literals and character literals).

**Formal Specification:**

```
[Lex-Whitespace-Separator]
character sequence C₁C₂
C₁ ends token T₁
C₂ begins token T₂
T₁ and T₂ would merge without separator
---------------------------------
Whitespace required between T₁ and T₂

[Lex-Whitespace-Optional]
character sequence C₁C₂
C₁ ends token T₁
C₂ begins token T₂
T₁ and T₂ cannot merge (e.g., punctuation)
---------------------------------
Whitespace optional between T₁ and T₂
```

---

### 2.2.4 Comment Syntax and Preservation

**Definition 2.8 (Comment):** A **comment** is source text intended for human readers that does not affect program semantics.

**Comment Categories:**

Cursive supports three comment types:

1. **Line comments**: `//` to end of line
2. **Block comments**: `/* ... */` with nesting
3. **Documentation comments**: `///` (item docs) and `//!` (module docs)

#### 2.2.4.1 Line Comments

**Syntax:**

```ebnf
LineComment ::= "//" ~[\n]* "\n"?
```

**Semantics:**

A line comment begins with `//` and extends to the end of the line or end of file. The terminating newline is **not** part of the comment.

**Examples:**

```cursive
// This is a line comment
let x = 5  // Inline comment

// Comment at end of file (no newline after)
```

**NORMATIVE REQUIREMENT:**

> "A line comment beginning with exactly two forward slashes (`//`) shall extend from the `//` to the end of the line, not including the line ending."

#### 2.2.4.2 Block Comments

**Syntax:**

```ebnf
BlockComment ::= "/*" (BlockComment | ~[*/])* "*/"
```

**Semantics:**

Block comments begin with `/*` and end with `*/`. Block comments **nest**: inner `/* */` pairs must be balanced.

**Nesting Example:**

```cursive
/* Outer comment
   /* Inner comment (nested) */
   Still outer comment
*/
```

**NORMATIVE REQUIREMENT:**

> "Block comments shall nest. Each `/*` must have a matching `*/`, and inner `/* */` pairs must be properly balanced."

**Invalid Example:**

```cursive
/* Unclosed block comment
   /* Nested but outer not closed
*/ // ❌ ERROR E2008: Unclosed block comment
```

**Diagnostic:**

```
error[E2008]: unterminated block comment
  --> src/example.cur:10:1
   |
10 | /* This comment is never closed
   | ^^ block comment starts here
   |
   = note: block comments must have matching */ delimiter
   = help: add */ to close the block comment
```

**Formal Specification:**

```
[Comment-Block-Nested]
Block comment B
B contains "/*" at position p
B contains matching "*/" at position q > p
---------------------------------
Inner block comment recognized

[Comment-Block-Unclosed]
Block comment begins at position p
No matching "*/" before end of file
---------------------------------
ERROR E2008: Unterminated block comment
```

#### 2.2.4.3 Documentation Comments

**Item Documentation Comments:**

```ebnf
DocComment ::= "///" ~[\n]* "\n"?
```

Item documentation comments begin with `///` (three slashes) and document the immediately following item (function, type, etc.).

**Example:**

```cursive
/// Calculates the absolute value of an integer.
///
/// Returns the non-negative value of `x`.
function abs(x: i32): i32 {
    result if x < 0 { -x } else { x }
}
```

**Module Documentation Comments:**

```ebnf
ModuleDocComment ::= "//!" ~[\n]* "\n"?
```

Module documentation comments begin with `//!` and document the containing module or file.

**Example:**

```cursive
//! This module provides mathematical utilities.
//!
//! Includes functions for common operations like
//! absolute value, maximum, minimum, etc.

public function abs(x: i32): i32 { ... }
```

**Documentation Preservation:**

**NORMATIVE REQUIREMENT:**

> "Documentation comments (beginning with `///` or `//!`) shall be preserved during lexical analysis and made available for documentation generation. Non-documentation comments shall be discarded."

**Formal Specification:**

```
[Comment-Doc-Preserve]
comment C begins with "///" or "//!"
---------------------------------
C is preserved for documentation generation

[Comment-Non-Doc-Discard]
comment C begins with "//" (not "///" or "//!")
---------------------------------
C is discarded during lexical analysis
```

#### 2.2.4.4 Comments and Statement Termination

**NORMATIVE STATEMENT:**

> "Comments do not affect statement continuation rules. A newline following a comment is a statement terminator unless continuation rules apply."

**Example:**

```cursive
let x = 5  // Comment here
// Newline after comment terminates statement (x = 5)
let y = 10  // New statement
```

**Continuation Example:**

```cursive
let sum = a +  // Trailing operator: statement continues
    // Comment on next line
    b +  // Still continuing
    c
```

---

### 2.2.5 Identifier Syntax and Restrictions

**Definition 2.9 (Identifier):** An **identifier** is a sequence of characters that names a program entity (variable, function, type, module, etc.).

**Syntax:**

```ebnf
Identifier ::= XID_Start XID_Continue*

(* XID_Start and XID_Continue defined by Unicode Standard Annex #31 *)
```

**Unicode Support:**

Cursive identifiers support Unicode as specified in UAX #31 (Unicode Identifier and Pattern Syntax):

- **XID_Start**: Characters that can begin an identifier
- **XID_Continue**: Characters that can continue an identifier

**NORMATIVE REQUIREMENT:**

> "Identifiers shall conform to Unicode Standard Annex #31 (UAX#31) identifier syntax, using the XID_Start and XID_Continue character classes."

**Examples:**

```cursive
// ✅ Valid identifiers
variable
myFunction
_underscore
Point2D
user_name
counter123

// Unicode identifiers
변수        // Korean
переменная  // Russian
μετάβλητη   // Greek
変数        // Japanese
```

**Case Sensitivity:**

**NORMATIVE REQUIREMENT:**

> "Identifiers are case-sensitive. The identifiers `Variable` and `variable` are distinct."

**Example:**

```cursive
let variable = 5
let Variable = 10  // ✅ Different identifier
```

**Maximum Length:**

The maximum identifier length is implementation-defined. Implementations SHOULD support identifiers of at least 255 characters.

**NORMATIVE RECOMMENDATION:**

> "Identifiers should be kept under 255 characters for portability across implementations."

**Identifier Restrictions:**

1. **Cannot be a keyword** ([REF_TBD] Keywords)
2. **Cannot start with a digit** (enforced by XID_Start)
3. **Cannot contain whitespace** (except in raw identifiers, if supported)

**Reserved Keyword Conflict:**

If an identifier matches a reserved keyword, it is a lexical error.

**Diagnostic:**

```
error[E2006]: keyword used as identifier
  --> src/example.cur:5:5
   |
 5 | let let = 5
   |     ^^^ 'let' is a reserved keyword
   |
   = note: keywords cannot be used as identifiers
   = help: choose a different identifier name
```

**Formal Specification:**

```
[Ident-Valid]
character sequence S
S[0] ∈ XID_Start
S[1..n] ∈ XID_Continue*
S ∉ Keywords
---------------------------------
S is a valid identifier

[Ident-Keyword-Conflict]
character sequence S
S matches reserved keyword K
---------------------------------
ERROR E2006: Keyword used as identifier
```

---

### 2.2.6 Reserved and Contextual Keywords

**Definition 2.10 (Keyword):** A **keyword** is a reserved identifier with special syntactic meaning that cannot be used as an identifier.

**Definition 2.11 (Contextual Keyword):** A **contextual keyword** is an identifier with special meaning only in specific syntactic contexts.

#### 2.2.6.1 Reserved Keywords

The following identifiers are reserved keywords and SHALL NOT be used as identifiers in any context:

```
abstract    as          async       await       break
by          case        comptime    continue
defer       effect      else        enum        exists
false       forall      function    if          import
internal    invariant   let         loop
match       modal       module      move        must
new         none        owned       private     procedure
protected   public      readonly    record      ref
region      result      select      self        Self
shadow      shared      state       static      predicate
true        type        unique      uses        var
where       will        with
```

**Keyword Count:** 59 reserved keywords

**NORMATIVE REQUIREMENT:**

> "The keywords listed above shall be reserved. They shall not be used as identifiers for variables, functions, types, or other entities."

**Keyword Usage:**

Reserved keywords may only appear in their designated syntactic positions as defined by the grammar ([REF_TBD] Grammar).

**Violation Example:**

```cursive
let function = 5  // ❌ ERROR E2006: 'function' is a keyword
```

#### 2.2.6.2 Contextual Keywords

**Contextual keywords** have special meaning only in specific contexts:

| Keyword | Context | Meaning |
|---------|---------|---------|
| `effects` | Contract/predicate declarations | Effect list specification |
| `pure` | Function declarations | Pure function marker |

**NORMATIVE STATEMENT:**

> "Contextual keywords have special meaning in their designated contexts but may be used as identifiers in other contexts."

**Example:**

```cursive
// ✅ OK: 'pure' as contextual keyword
pure function compute(x: i32): i32 { ... }

// ✅ OK: 'pure' as identifier (different context)
let pure = "not a keyword here"

// ✅ OK: 'effects' as contextual keyword
contract Example {
    effects: [io.read, io.write]
}

// ✅ OK: 'effects' as identifier
let effects = vec![effect1, effect2]
```

#### 2.2.6.3 Deprecated Keywords

The following keywords are DEPRECATED and SHOULD NOT be used:

| Deprecated | Replacement | Reason |
|------------|-------------|--------|
| `needs` | `must` | Contract clause renaming |
| `requires` | `must` | Contract clause renaming |
| `ensures` | `will` | Contract clause renaming |

**NORMATIVE RECOMMENDATION:**

> "Deprecated keywords should not be used. Implementations may issue warnings when deprecated keywords are encountered."

**Diagnostic (Warning):**

```
warning[W2001]: deprecated keyword 'requires'
  --> src/example.cur:8:5
   |
 8 |     requires x > 0
   |     ^^^^^^^^ use 'must' instead
   |
   = note: 'requires' is deprecated, replaced by 'must'
   = help: change to: must x > 0
```

---

### 2.2.7 Literal Syntax and Semantics

**Definition 2.12 (Literal):** A **literal** is a token that directly represents a constant value in source text.

**Literal Categories:**

```ebnf
Literal ::=
    IntegerLiteral
  | FloatingPointLiteral
  | StringLiteral
  | CharacterLiteral
  | BooleanLiteral
```

**Note:** For complete literal grammar, see Annex A.1.

#### 2.2.7.1 Integer Literals

**Syntax (Informal):**

```ebnf
IntegerLiteral ::= DecimalLiteral | HexLiteral | OctalLiteral | BinaryLiteral

DecimalLiteral ::= [0-9] ([0-9] | "_")* IntegerSuffix?
HexLiteral     ::= "0x" [0-9a-fA-F] ([0-9a-fA-F] | "_")* IntegerSuffix?
OctalLiteral   ::= "0o" [0-7] ([0-7] | "_")* IntegerSuffix?
BinaryLiteral  ::= "0b" [01] ([01] | "_")* IntegerSuffix?

IntegerSuffix  ::= "i8" | "i16" | "i32" | "i64" | "i128"
                 | "u8" | "u16" | "u32" | "u64" | "u128"
                 | "isize" | "usize"
```

**Numeric Bases:**

| Base | Prefix | Digits | Example |
|------|--------|--------|---------|
| Decimal | (none) | 0-9 | `42`, `1000` |
| Hexadecimal | `0x` | 0-9, a-f, A-F | `0xFF`, `0x1A2B` |
| Octal | `0o` | 0-7 | `0o755`, `0o644` |
| Binary | `0b` | 0-1 | `0b1010`, `0b1111_0000` |

**Underscore Separators:**

Underscores (`_`) MAY appear between digits for readability. They are ignored during value computation.

**NORMATIVE REQUIREMENT:**

> "Underscore characters in numeric literals shall be ignored during value computation. They serve only to improve readability."

**Restrictions on Underscores:**

- SHALL NOT appear at the start or end of the literal
- SHALL NOT appear adjacent to the base prefix
- SHALL NOT appear adjacent to the type suffix

**Valid Examples:**

```cursive
1_000_000    // ✅ OK: Grouping thousands
0xFF_FF      // ✅ OK: Grouping hex bytes
0b1111_0000  // ✅ OK: Grouping binary nibbles
```

**Invalid Examples:**

```cursive
_42          // ❌ ERROR: Leading underscore
42_          // ❌ ERROR: Trailing underscore
0x_FF        // ❌ ERROR: Underscore after prefix
42_i32       // ❌ ERROR: Underscore before suffix
```

**Type Suffixes:**

Integer literals MAY have a type suffix specifying the integer type. If no suffix is provided, the default type is `i32`.

**Examples:**

```cursive
42           // Type: i32 (default)
42i8         // Type: i8
42u64        // Type: u64
0xFFu32      // Type: u32
0b1010_1111i128  // Type: i128
```

**Integer Literal Overflow:**

If an integer literal exceeds the range of its type (either explicitly suffixed or default `i32`), it is a compile-time error.

**Diagnostic:**

```
error[E2009]: integer literal out of range
  --> src/example.cur:3:9
   |
 3 | let x = 999999999999999i32
   |         ^^^^^^^^^^^^^^^^^ value too large for type i32
   |
   = note: maximum i32 value is 2147483647
   = help: use a larger type (i64, i128) or check the value
```

**Formal Specification:**

```
[IntLit-InRange]
integer literal L with type T
value V = parse(L)
V ∈ range(T)
---------------------------------
L is valid with type T

[IntLit-Overflow]
integer literal L with type T
value V = parse(L)
V ∉ range(T)
---------------------------------
ERROR E2009: Integer literal out of range
```

#### 2.2.7.2 Floating-Point Literals

**Syntax (Informal):**

```ebnf
FloatingPointLiteral ::= DecimalDigits "." DecimalDigits? Exponent? FloatSuffix?
                       | DecimalDigits Exponent FloatSuffix?

DecimalDigits ::= [0-9] ([0-9] | "_")*
Exponent      ::= ("e" | "E") ("+" | "-")? DecimalDigits
FloatSuffix   ::= "f32" | "f64"
```

**Components:**

| Component | Required | Example |
|-----------|----------|---------|
| Integer part | Yes | `3` in `3.14` |
| Decimal point | Yes | `.` in `3.14` |
| Fractional part | No | `14` in `3.14` |
| Exponent | No | `e10` in `1.5e10` |
| Type suffix | No | `f32` in `3.14f32` |

**Examples:**

```cursive
3.14         // Type: f64 (default)
3.14f32      // Type: f32
0.5          // Type: f64
2.0e10       // 2.0 × 10^10, Type: f64
1.5e-5f32    // 1.5 × 10^-5, Type: f32
6.022e23     // Avogadro's number
```

**Default Type:**

Floating-point literals without a type suffix default to `f64`.

**Underscore Separators:**

Underscores MAY appear between digits in floating-point literals with the same restrictions as integer literals.

**Examples:**

```cursive
1_000.500_25     // ✅ OK: Underscores in both parts
6.022_141_29e23  // ✅ OK: Avogadro's number (readable)
```

**Special Floating-Point Values:**

Special values (infinity, NaN) are not representable as literals. They must be obtained via library functions:

```cursive
let inf = f64::INFINITY
let nan = f64::NAN
let neg_inf = f64::NEG_INFINITY
```

#### 2.2.7.3 String Literals

**Syntax (Informal):**

```ebnf
StringLiteral ::= '"' StringCharacter* '"'

StringCharacter ::= ~["\\\n] | EscapeSequence
```

**String Properties:**

- Delimited by double quotes (`"`)
- May span multiple lines (newlines preserved)
- Escape sequences processed during lexing
- Represent sequences of Unicode scalar values

**Escape Sequences:**

| Sequence | Description | Unicode |
|----------|-------------|---------|
| `\n` | Newline (line feed) | U+000A |
| `\r` | Carriage return | U+000D |
| `\t` | Horizontal tab | U+0009 |
| `\\` | Backslash | U+005C |
| `\'` | Single quote | U+0027 |
| `\"` | Double quote | U+0022 |
| `\0` | Null character | U+0000 |
| `\xNN` | ASCII character (00-7F) | U+00NN |
| `\u{N...}` | Unicode scalar (1-6 hex digits) | U+N... |

**Examples:**

```cursive
"hello, world"           // Simple string
"line 1\nline 2"         // String with newline
"tab\tseparated\tvalues" // String with tabs
"quote: \"hello\""       // String with quotes
"unicode: \u{1F680}"     // Unicode emoji 🚀
"path\\to\\file"         // Backslashes (Windows path)
```

**Multi-line Strings:**

Strings MAY span multiple lines. The newline characters in the source are preserved in the string value.

**Example:**

```cursive
let multiline = "
    Line 1
    Line 2
    Line 3
"  // Contains newlines
```

**Unterminated String Error:**

If a string literal is not terminated before the end of the file, it is a lexical error.

**Diagnostic:**

```
error[E2007]: unterminated string literal
  --> src/example.cur:15:9
   |
15 | let s = "This string is never closed
   |         ^ string literal starts here
   |
   = note: string literals must be terminated with "
   = help: add " to close the string literal
```

**Invalid Escape Sequence Error:**

If a string contains an invalid escape sequence, it is a lexical error.

**Diagnostic:**

```
error[E2010]: invalid escape sequence
  --> src/example.cur:8:13
   |
 8 | let s = "invalid: \q"
   |                   ^^ unknown escape sequence
   |
   = note: valid escape sequences: \n \r \t \\ \' \" \0 \xNN \u{...}
   = help: use \\ for literal backslash, or remove the backslash
```

**Formal Specification:**

```
[StringLit-Valid]
string literal S begins with "
S ends with " before end of file
all escape sequences in S are valid
---------------------------------
S is a valid string literal

[StringLit-Unterminated]
string literal S begins with "
no closing " before end of file
---------------------------------
ERROR E2007: Unterminated string literal

[StringLit-InvalidEscape]
string literal S contains "\c"
c ∉ {n, r, t, \, ', ", 0, x, u}
---------------------------------
ERROR E2010: Invalid escape sequence
```

#### 2.2.7.4 Character Literals

**Syntax (Informal):**

```ebnf
CharacterLiteral ::= "'" (CharacterChar | EscapeSequence) "'"

CharacterChar ::= ~['\\]
```

**Character Properties:**

- Delimited by single quotes (`'`)
- Must contain exactly one Unicode scalar value
- Same escape sequences as string literals
- Represent a single `char` value

**Examples:**

```cursive
'a'          // ASCII character
'x'          // ASCII character
'9'          // Digit character
'\n'         // Newline via escape
'\t'         // Tab via escape
'\''         // Single quote via escape
'\u{1F4A1}'  // Unicode emoji 💡
'😀'         // Unicode emoji (direct)
```

**Single Character Requirement:**

A character literal MUST contain exactly one character (or one escape sequence representing one character).

**Invalid Examples:**

```cursive
''           // ❌ ERROR: Empty character literal
'ab'         // ❌ ERROR: Multiple characters
'\n\t'       // ❌ ERROR: Multiple escape sequences
```

**Diagnostic:**

```
error[E2011]: invalid character literal
  --> src/example.cur:10:9
   |
10 | let c = 'ab'
   |         ^^^^ character literal must contain exactly one character
   |
   = note: character literals represent a single Unicode scalar value
   = help: use a string literal for multiple characters: "ab"
```

**Type:**

Character literals have type `char`, which represents a Unicode scalar value.

#### 2.2.7.5 Boolean Literals

**Syntax:**

```ebnf
BooleanLiteral ::= "true" | "false"
```

**Boolean Properties:**

- Two values: `true` and `false`
- Keywords (cannot be used as identifiers)
- Type: `bool`

**Examples:**

```cursive
let flag = true
let active = false

if true {  // Always true
    println("Always executes")
}
```

**Type:**

Boolean literals have type `bool`.

---

### 2.2.8 Operators and Punctuators

**Definition 2.13 (Operator):** An **operator** is a token representing an operation on one or more operands.

**Definition 2.14 (Punctuator):** A **punctuator** is a token providing syntactic structure (delimiters, separators).

**NORMATIVE NOTE:**

> "For the complete and authoritative list of operators and punctuators, see Annex A.1 (Lexical Grammar)."

**Operator Categories:**

| Category | Operators | Description |
|----------|-----------|-------------|
| **Arithmetic** | `+`, `-`, `*`, `/`, `%`, `**` | Numeric operations |
| **Comparison** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Relational tests |
| **Logical** | `&&`, `||`, `!` | Boolean operations |
| **Bitwise** | `&`, `|`, `^`, `~`, `<<`, `>>` | Bit manipulation |
| **Assignment** | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, etc. | Variable assignment |
| **Range** | `..`, `..=` | Range construction |
| **Pipeline** | `=>` | Function composition |
| **Other** | `.`, `::`, `?`, `@` | Member access, scope, etc. |

**Punctuator Categories:**

| Category | Punctuators | Purpose |
|----------|-------------|---------|
| **Delimiters** | `(`, `)`, `{`, `}`, `[`, `]`, `<<`, `>>` | Grouping, scoping, partitions |
| **Separators** | `,`, `;` | List items, statements |
| **Special** | `:`, `::`, `.`, `->` | Type, scope, field, arrow |

**Note on `<<` and `>>`:**

The tokens `<<` and `>>` serve dual purposes depending on context:
- **In expressions**: Left shift and right shift operators
- **In record field declarations**: Partition directive delimiters (see [Field-Level Partitioning](../../06_Memory-Model-and-Permissions/Field-Level-Partitioning.md))

Context disambiguates their meaning during parsing.

**Multi-Character Operators:**

Many operators consist of multiple characters. The lexer uses **maximal munch** ([REF_TBD] Token Formation) to recognize the longest matching operator.

**Examples:**

```cursive
<=      // Comparison operator (not < followed by =)
>>=     // Right shift assignment (not >> followed by =)
..=     // Inclusive range (not .. followed by =)
```

**Operator Precedence:**

Operator precedence and associativity are defined in [REF_TBD] Operator Precedence.

---

### 2.2.9 Statement Termination and Token Formation

#### 2.2.9.1 Statement Termination Rules

**Primary Rule:**

**NORMATIVE REQUIREMENT:**

> "A newline terminates a statement unless a continuation rule applies."

**Definition 2.15 (Statement Termination):** A newline character (U+000A, after normalization) acts as a statement terminator, except when specific continuation rules apply.

**Semicolon as Optional Separator:**

Semicolons (`;`) MAY be used to separate multiple statements on a single line. Semicolons are **not** required at the end of lines.

**Example:**

```cursive
// ✅ Newline terminates statements
let x = 5
let y = 10

// ✅ Semicolons separate statements on same line
let a = 1; let b = 2; let c = 3
```

**Continuation Rules:**

A statement continues across newlines in the following cases:

**Rule 2.15.1 — Unclosed Delimiters:**

If a line contains an unclosed delimiter (`(`, `[`, `{`, or `<`), the statement continues until the delimiter is closed.

**Example:**

```cursive
let result = long_function_name(
    argument1,
    argument2,
    argument3
)  // Statement ends here
```

**Formal Specification:**

```
[Continue-Unclosed-Delim]
line L contains opening delimiter D ∈ {(, [, {, <}
D not closed on line L
---------------------------------
Statement continues to next line

Continuation terminates when:
- Matching closing delimiter found
- All nested delimiters balanced
```

**Rule 2.15.2 — Trailing Operator:**

If a line ends with a binary or assignment operator, the statement continues to the next line.

**Trailing Operators:**

- **Binary operators**: `+`, `-`, `*`, `/`, `%`, `**`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`, `&`, `|`, `^`, `<<`, `>>`, `..`, `..=`, `=>`
- **Assignment operators**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`

**Example:**

```cursive
let sum = a +
    b +
    c
// Statement ends after c

let value = compute() *
    factor
// Statement ends after factor
```

**Formal Specification:**

```
[Continue-Trailing-Op]
line L ends with operator O
O ∈ BinaryOperators ∪ AssignmentOperators
---------------------------------
Statement continues to next line

Continuation terminates when:
- Line ends without trailing operator
- Complete expression formed
```

**Rule 2.15.3 — Leading Dot:**

If a line begins with a dot (`.`), it continues the previous statement (for method chaining).

**Example:**

```cursive
let result = object
    .method1()
    .method2()
    .field
// Statement ends after field
```

**NORMATIVE NOTE:**

> "The double colon (`::`) is NOT a chaining operator and does not trigger continuation. Use the pipeline operator (`=>`) for function chaining."

**Example:**

```cursive
// ❌ :: does NOT continue
let value = Module::function()
    ::another()  // ❌ ERROR: Unexpected ::

// ✅ Use pipeline for chaining
let result = value
    => transform1
    => transform2
```

**Formal Specification:**

```
[Continue-Leading-Dot]
line L+1 begins with "."
line L contains expression E
---------------------------------
L+1 continues statement from L

Continuation terminates when:
- Line does not begin with "."
- Complete expression formed
```

**Rule 2.15.4 — Leading Pipeline:**

If a line begins with the pipeline operator (`=>`), it continues the previous statement.

**Example:**

```cursive
let result = input
    => process
    => transform
    => finalize
// Statement ends after finalize
```

**Formal Specification:**

```
[Continue-Leading-Pipeline]
line L+1 begins with "=>"
line L contains expression E
---------------------------------
L+1 continues statement from L

Continuation terminates when:
- Line does not begin with "=>"
- Complete expression formed
```

#### 2.2.9.2 Token Formation (Maximal Munch)

**Definition 2.16 (Maximal Munch):** The **maximal munch rule** (also called longest match rule) states that the lexer shall consume the longest sequence of characters that forms a valid token.

**NORMATIVE REQUIREMENT:**

> "The lexer shall use the maximal munch rule: when multiple token matches are possible, the longest matching token shall be selected."

**Formal Specification:**

```
[Lex-Maximal-Munch]
Input stream I at position p
Multiple token matches possible:
  T₁ matches characters p...(p+n₁)
  T₂ matches characters p...(p+n₂)
  ...
  Tₖ matches characters p...(p+nₖ)
n₁ > n₂ > ... > nₖ
---------------------------------
Select T₁ (longest match)
Advance position to p+n₁
```

**Examples:**

```cursive
// "<<=" is one token (left shift assignment)
x <<= 2  // Not: x << = 2 (three tokens)

// "..." could be ".." followed by ".", but "..." is not a token
// So it's parsed as ".." followed by "."
let range = 1..10  // ".." is range operator
```

**Ambiguity Resolution:**

Maximal munch prevents ambiguities in token recognition.

**Example:**

```cursive
// ">>" vs ">", ">"
// Maximal munch: ">>" (right shift operator)
let shifted = value >> 2  // One ">>" token, not two ">" tokens

// ">>=" vs ">>", "="
// Maximal munch: ">>=" (right shift assignment)
value >>= 2  // One ">>=" token
```

**Interaction with Generics:**

In generic type contexts, `>>` may need to be parsed as two separate `>` tokens (closing nested type parameters).

**Example:**

```cursive
Ptr<Ptr<i32>@Valid>@Valid  // The ">>" is two closing angle brackets, not right shift
```

**NORMATIVE NOTE:**

> "In generic type parameter contexts, the lexer shall recognize closing angle brackets `>` even when they appear consecutively as `>>`, overriding maximal munch."

---

### 2.2.10 Canonical Examples

#### Example 1: Complete Tokenization

**Source Code:**

```cursive
// This demonstrates tokenization
let x = 42 + 3.14
```

**Token Stream:**

```
1. COMMENT("// This demonstrates tokenization")  [Discarded]
2. NEWLINE
3. KEYWORD("let")
4. IDENTIFIER("x")
5. OPERATOR("=")
6. INTEGER_LITERAL("42", None)  // Default: i32
7. OPERATOR("+")
8. FLOAT_LITERAL("3.14", None)  // Default: f64
9. NEWLINE
10. EOF
```

#### Example 2: Comments (All Types)

```cursive
// Line comment - discarded

/* Block comment - discarded
   /* Nested block comment */
   Still in outer comment
*/

/// Documentation comment for the function below
/// - Preserved for documentation generation
function example() {}

//! Module documentation comment
//! Describes this module
```

**Token Stream (Comments):**

```
- Line comments: Discarded
- Block comments: Discarded
- Doc comments (///): Preserved
- Module docs (//!): Preserved
```

#### Example 3: Identifiers (Valid and Invalid)

```cursive
// ✅ Valid identifiers
let variable = 5
let _underscore = 10
let camelCase = 15
let snake_case = 20
let with123numbers = 25
let 変数 = 30  // Unicode
let μετάβλητη = 35  // Greek

// ❌ Invalid identifiers
// let 123invalid = 40  // ERROR: Starts with digit
// let let = 45  // ERROR: Keyword
```

#### Example 4: Literals (All Types)

```cursive
// Integer literals
let dec = 42
let hex = 0xFF
let octal = 0o755
let binary = 0b1010_1111
let typed = 100u64

// Floating-point literals
let pi = 3.14159
let sci = 6.022e23
let small = 1.5e-10f32

// String literals
let simple = "hello"
let escaped = "line1\nline2"
let unicode = "\u{1F680}"  // 🚀

// Character literals
let ch = 'x'
let newline_ch = '\n'
let emoji_ch = '😀'

// Boolean literals
let flag = true
let active = false
```

#### Example 5: Statement Termination and Continuation

```cursive
// ✅ Newline terminates
let x = 5
let y = 10

// ✅ Continuation: Unclosed delimiter
let sum = calculate(
    a, b, c,
    d, e, f
)

// ✅ Continuation: Trailing operator
let total = value1 +
    value2 +
    value3

// ✅ Continuation: Leading dot
let result = object
    .method1()
    .method2()
    .field

// ✅ Continuation: Leading pipeline
let processed = input
    => validate
    => transform
    => finalize

// ✅ Semicolons separate on same line
let a = 1; let b = 2; let c = 3
```

#### Example 6: Maximal Munch

```cursive
// "<<=" recognized as single token (not "<<" + "=")
x <<= 2  // Left shift assignment

// ">>" recognized as single token in expressions
let shifted = value >> 4  // Right shift

// But ">>" split in generic context
let nested: Ptr<Ptr<i32>@Valid>@Valid = Ptr::new(&inner_ptr)  // Two ">" tokens

// "..=" recognized as single token (not ".." + "=")
let range = 0..=10  // Inclusive range
```

---

### 2.2.11 Error Code Summary

| Error Code | Description | Example |
|------------|-------------|---------|
| **E2006** | Keyword used as identifier | `let let = 5` |
| **E2007** | Unterminated string literal | `let s = "unclosed` |
| **E2008** | Unterminated block comment | `/* never closed` |
| **E2009** | Integer literal out of range | `999999999999i32` |
| **E2010** | Invalid escape sequence | `"invalid: \q"` |
| **E2011** | Invalid character literal | `'ab'` (multiple chars) |

---

### 2.2.12 Conformance Requirements Summary

A conforming implementation MUST:

1. Recognize all token categories (identifiers, keywords, literals, operators, punctuators)
2. Discard non-documentation comments during lexical analysis
3. Preserve documentation comments (`///`, `//!`) for documentation generation
4. Support Unicode identifiers per UAX#31 (XID_Start, XID_Continue)
5. Treat identifiers as case-sensitive
6. Reserve all specified keywords and prohibit their use as identifiers
7. Support all literal forms (integer, float, string, character, boolean)
8. Support underscore separators in numeric literals
9. Recognize all numeric bases (decimal, hexadecimal, octal, binary)
10. Process escape sequences in string and character literals
11. Support nested block comments
12. Use maximal munch rule for token formation
13. Recognize newlines as statement terminators (with continuation rules)
14. Support all four continuation rules (unclosed delimiters, trailing operators, leading dot, leading pipeline)

A conforming implementation SHOULD:

1. Support identifiers of at least 255 characters
2. Issue warnings for deprecated keywords
3. Provide clear diagnostics for lexical errors

A conforming implementation MAY:

1. Support longer identifiers than 255 characters
2. Provide warnings for unusual identifier patterns (all uppercase, single letter, etc.)

---

**Previous**: [Source Text Encoding](02-1_Source-Text-Encoding.md) | **Next**: [Syntax Notation](02-3_Syntax-Notation.md)

**END OF SECTION 2.2**
