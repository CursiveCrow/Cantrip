# Part I: Foundations - §2. Lexical Structure

**Section**: §2 | **Part**: Foundations (Part I)
**Previous**: [Mathematical Foundations](01_MathFoundations.md) | **Next**: [Abstract Syntax](03_AbstractSyntax.md)

---

**Definition 2.1 (Lexical Structure):** The lexical structure defines the syntax of tokens—the smallest elements of Cantrip source text—including identifiers, literals, keywords, operators, and delimiters.

## 2. Lexical Structure

### 2.1 Overview

**Key innovation/purpose:** Defines the atomic elements (tokens) of Cantrip source code, forming the first phase of compilation that transforms character streams into structured token sequences.

**When to use this section:**
- When implementing a lexer/tokenizer
- When defining concrete syntax for new language features
- When understanding character-level source code rules
- When resolving ambiguities in token formation

**When NOT to use this section:**
- For understanding program structure (see §3 Abstract Syntax)
- For type system rules (see Part II: Type System)
- For runtime semantics (see Part X: Operational Semantics)
- For practical programming examples (see feature-specific sections)

**Relationship to other features:**
- **§3 (Abstract Syntax)**: Tokens are parsed into abstract syntax trees
- **§4+ (Type System)**: Type inference assigns types to literal tokens
- **Compilation Pipeline**: Lexical analysis is the first phase
- **Error Reporting (§52)**: Lexical errors are the first class of errors detected

**Phase in compilation pipeline:**
```
Source Text → [Lexer] → Token Stream → [Parser] → AST → [Type Checker] → ...
```

**Token categories:**
- **Identifiers**: Names for variables, functions, types (§2.2.3)
- **Literals**: Integer, float, boolean, character, string constants (§2.2.4)
- **Keywords**: Reserved words with special meaning (§2.2.5)
- **Operators**: Symbolic operators (+, -, *, /, etc.)
- **Delimiters**: Punctuation ({}, [], (), etc.)
- **Comments**: Discarded during tokenization (§2.2.2)

**Scope:** This section defines what tokens LOOK LIKE (their lexical form). Type inference and type checking for literals are defined in §4 (Primitives) static semantics.

### 2.2 Syntax

#### 2.2.1 Concrete Syntax

**Source Files:**

**Definition 2.2 (Source File):** A Cantrip source file is a sequence of Unicode characters encoded in UTF-8.

**Grammar:**
```ebnf
SourceFile ::= Utf8Bom? Item*
Utf8Bom    ::= #xEF #xBB #xBF
```

**Formal properties:**
- Encoding: UTF-8 (REQUIRED)
- Line endings: LF (`\n`), CRLF (`\r\n`), or CR (`\r`) (all accepted, normalized to LF during lexing)
- Newlines are significant tokens: Newlines are preserved as tokens during lexical analysis (not discarded as whitespace)
- BOM: Optional UTF-8 BOM (U+FEFF) is ignored if present
- File extension: `.cantrip` (RECOMMENDED)
- Maximum file size: Implementation-defined (RECOMMENDED: 1MB)
- Normalization: Unicode normalization form NFC is RECOMMENDED but not required

**Comments:**

**Grammar:**
```ebnf
Comment      ::= LineComment | BlockComment | DocComment | ModuleDoc
LineComment  ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" | BlockComment)* "*/"
DocComment   ::= "///" ~[\n\r]*
ModuleDoc    ::= "//!" ~[\n\r]*
```

**Semantic rules:**
- Line comments extend from `//` to end of line
- Block comments nest: `/* outer /* inner */ outer */` is valid
- Doc comments (`///`) document the item that follows
- Module docs (`//!`) document the containing module
- Doc comments are preserved for documentation generation
- Non-doc comments are stripped before parsing
- Comments do not affect statement continuation (newlines after comments are significant)

**Identifiers:**

**Definition 2.3 (Identifier):** An identifier is a sequence of characters used to name variables, functions, types, modules, and other program entities.

**Grammar:**
```ebnf
Identifier    ::= IdentStart IdentContinue*
IdentStart    ::= [a-zA-Z_]
IdentContinue ::= [a-zA-Z0-9_]
```

**Restrictions:**
- Cannot be a reserved keyword (§2.2.5)
- Cannot start with a digit
- Case-sensitive: `Variable` ≠ `variable`
- Maximum length: Implementation-defined (RECOMMENDED: 255 characters)

**Integer Literals:**

**Grammar:**
```ebnf
IntegerLiteral ::= DecimalLiteral IntegerSuffix?
                 | HexLiteral IntegerSuffix?
                 | BinaryLiteral IntegerSuffix?
                 | OctalLiteral IntegerSuffix?

DecimalLiteral ::= [0-9] [0-9_]*
HexLiteral     ::= "0x" [0-9a-fA-F] [0-9a-fA-F_]*
BinaryLiteral  ::= "0b" [01] [01_]*
OctalLiteral   ::= "0o" [0-7] [0-7_]*

IntegerSuffix  ::= "i8" | "i16" | "i32" | "i64" | "isize"
                 | "u8" | "u16" | "u32" | "u64" | "usize"
```

**Underscore separators:**
- Underscores (`_`) may appear between digits for readability
- Underscores are ignored during value computation
- Cannot appear at the start or end of the literal
- Cannot appear adjacent to the base prefix (`0x`, `0b`, `0o`)

**Floating-Point Literals:**

**Grammar:**
```ebnf
FloatLiteral ::= DecimalLiteral "." DecimalLiteral Exponent? FloatSuffix?
               | DecimalLiteral Exponent FloatSuffix?

Exponent     ::= [eE] [+-]? DecimalLiteral
FloatSuffix  ::= "f32" | "f64"
```

**Boolean Literals:**

**Grammar:**
```ebnf
BooleanLiteral ::= "true" | "false"
```

**Character Literals:**

**Grammar:**
```ebnf
CharLiteral    ::= "'" (EscapeSequence | ~['\\]) "'"
EscapeSequence ::= "\\" [nrt\\'"0]
                 | "\\x" HexDigit HexDigit
                 | "\\u{" HexDigit+ "}"
```

**Escape sequences:**
- `\n` — newline (U+000A)
- `\r` — carriage return (U+000D)
- `\t` — tab (U+0009)
- `\\` — backslash (U+005C)
- `\'` — single quote (U+0027)
- `\"` — double quote (U+0022)
- `\0` — null character (U+0000)
- `\xNN` — ASCII character (00-7F)
- `\u{N...}` — Unicode scalar value (up to 6 hex digits)

**String Literals:**

**Grammar:**
```ebnf
StringLiteral ::= '"' (EscapeSequence | ~["\\])* '"'
```

**String properties:**
- Strings are sequences of Unicode scalar values (not code points or grapheme clusters)
- Escape sequences are processed during lexing
- Multi-line strings are supported (newlines in source → newlines in string)

**Keywords:**

**Definition 2.4 (Keyword):** A keyword is a reserved identifier with special syntactic meaning that cannot be used as an identifier.

**Reserved keywords (MUST NOT be used as identifiers):**
```
abstract    as          async       await       break
by          case        comptime    const       continue
defer       effect      else        enum        exists
false       forall      function    if          import
internal    invariant   iso         let         loop
match       modal       module      move        must
mut         new         none        own         private
procedure   protected   public      record      ref
region      result      select      self        Self
state       static      trait       true        type
uses        var         where       will        with
```

**Contextual keywords (special meaning in specific contexts):**
```
effects     pure
```

**Statement Terminators:**

**Definition 2.5 (Statement Termination):** Cantrip uses newlines as primary statement terminators. Semicolons are optional and may be used to separate multiple statements on a single line.

**Grammar:**
```ebnf
Separator ::= NEWLINE | ";"
Whitespace ::= " " | "\t" | "\r"  (not newline)
```

**Termination Rules:**
- **Primary rule:** A newline terminates a statement unless a continuation rule applies
- **Optional separator:** Semicolons `;` may be used to separate statements on the same line
- **Whitespace:** Spaces, tabs, and carriage returns are discarded (not significant)
- **Newlines:** Newlines are preserved as tokens (significant for statement boundaries)

**Statement Continuation Rules:**

A statement continues across newlines in exactly four cases:

**Rule 1: Unclosed Delimiters**
```
Statement continues when `(`, `[`, or `<` remains unclosed.
```

**Rule 2: Trailing Operator**
```
Statement continues when line ends with a binary or assignment operator.
Binary operators: +, -, *, /, %, **, ==, !=, <, <=, >, >=, &&, ||, &, |, ^, <<, >>, .., ..=, =>
Assignment operators: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
```

**Rule 3: Leading Dot**
```
Statement continues when next line begins with `.` (method/field access).
```

**Rule 4: Leading Pipeline**
```
Statement continues when next line begins with `=>` (pipeline operator).
```

**Formal Continuation Predicate:**
```
is_continuation(current: Token, state: ParserState) -> bool:
    // Rule 1: Unclosed delimiters
    if state.paren_depth > 0 || state.bracket_depth > 0 || state.angle_depth > 0:
        return true

    // Rule 2: Trailing operator
    if state.previous_token in BINARY_OPERATORS ∪ ASSIGNMENT_OPERATORS:
        return true

    // Rule 3: Leading dot
    if state.next_token == DOT:
        return true

    // Rule 4: Leading pipeline
    if state.next_token == PIPELINE:
        return true

    return false
```

**Examples:**
```cantrip
// Simple statements (newline terminates)
let x = 1
let y = 2

// Multiple statements on one line (semicolon separates)
let x = 1; let y = 2

// Rule 1: Unclosed parentheses
let result = calculate(
    arg1,
    arg2
)

// Rule 2: Trailing operator
let total = base +
    modifier +
    bonus

// Rule 3: Leading dot (method chaining)
result
    .validate()
    .process()

// Rule 4: Leading pipeline
let result = input
    => validate
    => transform
```

#### 2.2.2 Abstract Syntax

**Token Abstract Representation:**

Tokens are represented internally by the compiler as tagged unions:

```
Token ::= Identifier(name: String)
        | IntLiteral(value: Integer, suffix: Option<IntegerType>)
        | FloatLiteral(value: Float, suffix: Option<FloatType>)
        | BoolLiteral(value: Boolean)
        | CharLiteral(value: Char)
        | StringLiteral(value: String)
        | Keyword(word: KeywordKind)
        | Operator(op: OperatorKind)
        | Delimiter(delim: DelimiterKind)
        | Newline                               (significant token)
        | Semicolon                             (optional separator)
        | EOF

KeywordKind ::= Function | Procedure | Record | Enum | Modal | ...
OperatorKind ::= Plus | Minus | Star | Slash | ...
DelimiterKind ::= LParen | RParen | LBrace | RBrace | ...

IntegerType ::= I8 | I16 | I32 | I64 | ISize | U8 | U16 | U32 | U64 | USize
FloatType ::= F32 | F64
```

**Key changes from traditional lexers:**
- **Newline is preserved:** Unlike most C-family languages, newlines are not discarded as whitespace
- **Semicolon is optional:** Semicolons are tokens but not required for statement termination
- **Whitespace handling:** Only spaces, tabs, and carriage returns are discarded; newlines are kept

**Token Stream:**

A token stream is a sequence of tokens:
```
TokenStream ::= Token*
```

**Formal properties:**
```
∀ source_text ∈ SourceText. ∃! token_stream ∈ TokenStream.
    tokenize(source_text) = token_stream

Reading: "For any source text, there exists a unique token stream produced by tokenization."
```

**Token position tracking:**

Each token carries source location metadata:
```
PositionedToken ::= Token × SourcePosition

SourcePosition ::= {
    file: FilePath,
    line: LineNumber,
    column: ColumnNumber,
    offset: ByteOffset
}
```

#### 2.2.3 Basic Examples

**Example 1: Simple function tokenization**

**Source:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y
}
```

**Token Stream:**
```
Keyword(Function)
Identifier("add")
Delimiter(LParen)
Identifier("x")
Delimiter(Colon)
Identifier("i32")
Delimiter(Comma)
Identifier("y")
Delimiter(Colon)
Identifier("i32")
Delimiter(RParen)
Delimiter(Colon)
Identifier("i32")
Delimiter(LBrace)
Identifier("x")
Operator(Plus)
Identifier("y")
Delimiter(RBrace)
EOF
```

**Observations:**
- Comments are discarded
- Whitespace is not in the token stream
- Keywords are distinct token types
- Identifiers include both variable names (`x`, `y`) and type names (`i32`)

**Example 2: Literal tokenization**

**Source:**
```cantrip
42
0xFF
"hello"
'a'
true
```

**Token Stream:**
```
IntLiteral(42, None)
IntLiteral(255, None)
StringLiteral("hello")
CharLiteral('a')
BoolLiteral(true)
EOF
```

**Example 3: Maximal munch**

**Source:**
```cantrip
x++
0x10
while_loop
42u32
```

**Token Stream:**
```
Identifier("x")
Operator(PlusPlus)
IntLiteral(16, None)
Identifier("while_loop")
IntLiteral(42, Some(U32))
EOF
```

**Explanation:** The lexer always forms the longest possible token (maximal munch principle).

### 2.3 Static Semantics

#### 2.3.1 Well-Formedness Rules

**Reserved Word Rule:**
```
[WF-Reserved]
w ∈ Keywords
────────────────────
w cannot be used as identifier
```

**Identifier Validity:**
```
[WF-Identifier]
id = IdentStart IdentContinue*
id ∉ Keywords
────────────────────────────────
id is a valid identifier
```

**Literal Suffix Validity:**
```
[WF-Int-Suffix]
suffix ∈ {i8, i16, i32, i64, isize, u8, u16, u32, u64, usize}
────────────────────────────────────────────────────────────────
IntegerLiteral suffix valid

[WF-Float-Suffix]
suffix ∈ {f32, f64}
────────────────────────────────
FloatLiteral suffix valid
```

**Character Escape Validation:**
```
[WF-Escape-Simple]
c ∈ {n, r, t, \\, ', ", 0}
────────────────────────────
\c is valid escape sequence

[WF-Escape-Hex]
0x00 ≤ NN ≤ 0x7F
────────────────────────────
\xNN is valid ASCII escape

[WF-Escape-Unicode]
0x0000 ≤ NNNNNN ≤ 0x10FFFF
NNNNNN ∉ [0xD800, 0xDFFF]    (surrogate range)
────────────────────────────────────────────────
\u{NNNNNN} is valid Unicode escape
```

**Underscore Separator Validity:**
```
[WF-Underscore]
'_' appears between two digits
─────────────────────────────────
underscore placement valid
```

#### 2.3.2 Tokenization Rules

**Formal tokenization function:**
```
tokenize: SourceText → TokenStream

tokenize(ε) = []
tokenize(ws · rest) = tokenize(rest)  where ws ∈ Whitespace
tokenize(comment · rest) = tokenize(rest)  where comment ∈ Comment
tokenize(keyword · rest) = Keyword(keyword) :: tokenize(rest)
tokenize(literal · rest) = Literal(literal) :: tokenize(rest)
tokenize(identifier · rest) = Identifier(identifier) :: tokenize(rest)
```

**Maximal Munch Principle:**
```
Property 2.1 (Maximal Munch):
∀ text ∈ SourceText, token₁, token₂ ∈ Token.
    text starts with token₁ ∧
    text starts with token₂ ∧
    length(token₁) > length(token₂)
    ⇒ tokenize(text) begins with token₁

Reading: "When multiple tokens match, the lexer always chooses the longest match."
```

**Examples:**
```
"0xFF" → IntLiteral(255) NOT IntLiteral(0), Identifier("xFF")
"x++" → Identifier("x"), Operator(PlusPlus) NOT Identifier("x"), Operator(Plus), Operator(Plus)
"while_loop" → Identifier("while_loop") NOT Keyword("while"), Identifier("_loop")
```

#### 2.3.3 Type Properties

**Formal property (identity):**
```
Property 2.2 (Identifier Equality):
∀ x, y ∈ Identifier. x = y ⟺ string(x) = string(y)

Reading: "Identifiers are compared as exact character sequences.
         No Unicode normalization is performed during comparison."
```

**Value computation semantics:**
```
Property 2.3 (Integer Literal Value):
⟦n⟧ᵢₙₜ = value of n in base-10
⟦0xN⟧ᵢₙₜ = value of N in base-16
⟦0bN⟧ᵢₙₜ = value of N in base-2
⟦0oN⟧ᵢₙₜ = value of N in base-8

Property 2.4 (Float Literal Value):
⟦d₁.d₂⟧_float = d₁ + d₂ × 10^(-|d₂|)
⟦x eᵏ⟧_float = x × 10^k

Property 2.5 (Boolean Literal Value):
⟦true⟧_bool = ⊤
⟦false⟧_bool = ⊥
```

### 2.4 Dynamic Semantics

**No special runtime behavior:** Lexical analysis is a compile-time phase only. Tokens do not exist at runtime.

**Tokenization is pure:**
```
Property 2.6 (Deterministic Tokenization):
∀ source ∈ SourceText. tokenize(source) = tokenize(source)

Reading: "Tokenization is deterministic and produces the same result every time."
```

**Tokenization never modifies source:**
```
Property 2.7 (No Side Effects):
tokenize: SourceText → TokenStream is a pure function

Reading: "Tokenization has no side effects and does not modify the input."
```

### 2.5 Additional Properties

#### 2.5.1 Ambiguity Resolution

**Maximal munch in action:**

```cantrip
// Example 1: Operators
x++         → [Identifier("x"), PlusPlus]
            NOT [Identifier("x"), Plus, Plus]

// Example 2: Number literals
0x10        → [IntLiteral(16)]
            NOT [IntLiteral(0), Identifier("x10")]

// Example 3: Keywords
while_loop  → [Identifier("while_loop")]
            NOT [Keyword("while"), Identifier("_loop")]

// Example 4: Type suffix
42u32       → [IntLiteral(42, Some(u32))]
            NOT [IntLiteral(42), Identifier("u32")]
```

**Operator precedence:** Resolved in the parser (§3), not the lexer. The token `++` is lexically distinct from two `+` tokens.

#### 2.5.2 Error Conditions

**Invalid tokens:**
```cantrip
@symbol     // ERROR E2001: '@' is not a valid token start character
0xFF_       // ERROR E2002: trailing underscore in integer literal
'\xAB'      // ERROR E2003: hex escape exceeds ASCII range (0x7F)
'\u{D800}'  // ERROR E2004: surrogate code point in Unicode escape
let while = 5  // ERROR E2005: 'while' is reserved keyword
```

**Valid lexically but type-incorrect (type errors detected in §4):**
```cantrip
let x: u8 = 256        // Lexically valid, type error (overflow)
let y: i32 = 3.14      // Lexically valid, type error (mismatch)
```

### 2.6 Examples and Patterns

#### 2.6.1 Complete Tokenization Example

**Input:**
```cantrip
function add(x: i32, y: i32): i32 {
    x + y  // Simple addition
}
```

**Token Stream:**
```
[
  Keyword("function"),
  Identifier("add"),
  LParen,
  Identifier("x"),
  Colon,
  Identifier("i32"),
  Comma,
  Identifier("y"),
  Colon,
  Identifier("i32"),
  RParen,
  Colon,
  Identifier("i32"),
  LBrace,
  Identifier("x"),
  Plus,
  Identifier("y"),
  RBrace
]
```

**Observations:**
- Comments are discarded
- Whitespace is not in the token stream
- Keywords are distinct token types
- Identifiers include both variable names (`x`, `y`) and type names (`i32`)

#### 2.6.2 Literal Parsing Examples

**Integer literals:**
```cantrip
42          → IntLiteral(42, None)           // No suffix, default i32
42u64       → IntLiteral(42, Some(u64))      // Explicit u64
0xFF        → IntLiteral(255, None)          // Hexadecimal
0b1010      → IntLiteral(10, None)           // Binary
1_000_000   → IntLiteral(1000000, None)      // Underscores ignored
```

**Floating-point literals:**
```cantrip
3.14        → FloatLiteral(3.14, None)       // No suffix, default f64
3.14f32     → FloatLiteral(3.14, Some(f32))  // Explicit f32
1.0e10      → FloatLiteral(1.0e10, None)     // Scientific notation
```

**String and character literals:**
```cantrip
'a'         → CharLiteral('a')               // ASCII character
'\n'        → CharLiteral('\n')              // Escape sequence
'\u{1F600}' → CharLiteral('😀')              // Unicode escape

"hello"     → StringLiteral("hello")
"line 1\nline 2" → StringLiteral("line 1\nline 2")
```

#### 2.6.3 Statement Termination Examples

**Simple statements:**
```cantrip
let x = 10
let y = 20
let z = x + y
```

**Multi-line expressions:**
```cantrip
let result = base_value +
    adjustment_factor *
    scale_factor

let condition = x > 0 &&
    y < 100 &&
    z != 0
```

**Method chaining:**
```cantrip
let result = builder
    .with_capacity(100)
    .with_name("example")
    .build()
```

**Unclosed delimiters:**
```cantrip
let array = [
    1, 2, 3,
    4, 5, 6
]

let result = compute(
    arg1,
    arg2,
    arg3
)
```

---

**Previous**: [Mathematical Foundations](01_MathFoundations.md) | **Next**: [Abstract Syntax](03_AbstractSyntax.md)
