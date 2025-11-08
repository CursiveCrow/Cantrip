# The Cursive Language Specification

## Clause 2 — Lexical Structure and Translation

**Part**: II — Lexical Structure and Translation
**File**: 02-3_Lexical-Elements.md
**Section**: §2.3 Lexical Elements
**Stable label**: [lex.tokens]
**Forward references**: §2.1 [lex.source], §2.4 [lex.terminators], Annex A §A.2 [grammar.lexical]

---

### §2.3.1 Overview

[1] Lexical analysis transforms the normalised character stream (§2.1 [lex.source]) into a sequence of tokens consumed by the parser (§2.2 [lex.phases]). Tokens carry a kind, lexeme, and source location, while whitespace and non-documentation comments are discarded.

[2] Documentation comments beginning with `///` or `//!` are retained for later association with declarations; all other comments are removed during this phase.

### §2.3.2 Syntax

[1] Token categories are defined by Annex A §A.2 [grammar.lexical] and are summarised below:

```ebnf
token
    ::= identifier
     | keyword
     | literal
     | operator
     | punctuator
     | newline
```

[ Note: See Annex A §A.2 [grammar.lexical] for complete lexical grammar.
— end note ]

[2] Identifiers follow Unicode Standard Annex #31 (XID_Start followed by zero or more XID_Continue code points) and shall not collide with reserved keywords.

### §2.3.3 Constraints

[1] _Whitespace._ Space (U+0020), horizontal tab (U+0009), and form feed (U+000C) act as token separators and are not emitted as tokens. Line feed (U+000A) is emitted as a `NEWLINE` token for use in §2.4 [lex.terminators].

[2] _Comments._

- (2.1) Line comments starting with `//` consume characters to the next line terminator and are discarded.
- (2.2) Block comments delimited by `/*` and `*/` shall nest; unclosed block comments raise diagnostic E02-209.

[3] _Documentation comments._ Comments beginning with `///` or `//!` shall be preserved and attached to the following item or module, respectively. They participate in documentation tooling but do not appear in the token stream.

[4] _Keywords._ The following identifiers are reserved and may not be used as ordinary identifiers: `abstract`, `as`, `async`, `await`, `behavior`, `break`, `by`, `case`, `comptime`, `const`, `continue`, `contract`, `defer`, `else`, `enum`, `exists`, `false`, `forall`, `grant`, `if`, `import`, `internal`, `invariant`, `let`, `loop`, `match`, `modal`, `module`, `move`, `must`, `new`, `none`, `private`, `procedure`, `protected`, `public`, `record`, `region`, `result`, `select`, `self`, `Self`, `shadow`, `shared`, `state`, `static`, `true`, `type`, `unique`, `var`, `where`, `will`, `with`, `witness`.

[5] _Literals._ Numeric literals support decimal, hexadecimal (`0x`), octal (`0o`), and binary (`0b`) prefixes. Underscores `_` may separate digits but shall not appear at the start or end of the literal, immediately after a base prefix, or before a type suffix. Integer literals without a suffix default to type `i32`. Violations raise diagnostic E02-206. Floating-point literals consist of an integer part, optional fractional part, optional exponent, and optional suffix (`f32` or `f64`).

[6] _String and character literals._ Strings are delimited by double quotes and support escape sequences `\n`, `\r`, `\t`, `\\`, `\"`, `\'`, `\0`, `\xNN`, and `\u{...}`. Invalid escapes raise diagnostic E02-201. Character literals use single quotes and must correspond to a single Unicode scalar value; empty or multi-character literals raise diagnostic E02-203.

(6.1) _String literal line boundaries._ String literals may not span multiple lines unless escaped newlines (`\n`) are used. An unclosed string literal that reaches end-of-file or encounters a newline without being closed emits diagnostic E02-200 (unterminated string literal). The lexer shall not attempt to recover by inserting a closing quote; the compilation unit is ill-formed.

(6.2) _Nested block comments._ Block comments nest arbitrarily: `/* outer /* inner */ still outer */` is valid and consumes all characters between the outermost `/*` and `*/`. Unclosed nested comments emit diagnostic E02-209. Implementations shall track nesting depth and report the nesting level at the point of failure to aid debugging.

(6.3) _Invalid Unicode in identifiers._ Identifiers must consist of valid Unicode XID_Start and XID_Continue code points. If an identifier contains invalid Unicode sequences (e.g., unpaired surrogates, invalid UTF-8), diagnostic E02-210 (invalid Unicode in identifier) is emitted. The lexer shall not attempt to repair invalid sequences; the identifier is rejected.

[7] _Operators and punctuators._ Multi-character operators (e.g., `==`, `!=`, `=>`, `..=`, `<-`) are recognised using maximal munch (§2.4 [lex.terminators]). The reference-binding operator `<-` participates in the same precedence and continuation rules as `=` so that Clause 5 bindings parse unambiguously. Implementations shall disambiguate closing angle brackets in generic type contexts by treating `>>` as two tokens when syntactically required. The glyph `~` is reserved for procedure receiver shorthand (§5.4 [decl.function]) and is tokenised as an operator so that combinations such as `~%` and `~!` are available.

### §2.3.4 Semantics

[1] Tokens retain source-span metadata (file, line, column) so that later phases can provide precise diagnostics and tooling hooks.

[2] Documentation comments preserved by constraint [3] are associated with the immediately following declaration unless separated by a blank line.

[3] Invalid lexical constructs shall emit the diagnostics listed in Table 2.3.4 and shall not produce tokens; compilation continues so that additional diagnostics can be reported.

### §2.3.5 Examples

**Example 2.3.5.1 (Token Stream):**

```cursive
// comment
let answer = 42
```

[1] Tokens emitted: `NEWLINE`, `KEYWORD("let")`, `IDENTIFIER("answer")`, `OPERATOR("=")`, `INTEGER_LITERAL("42")`, `NEWLINE`.

**Example 2.3.5.2 - invalid (Keyword misuse):**

```cursive
let let = 5  // error[E02-208]
```

[2] The second `let` is rejected because keywords cannot serve as identifiers (constraint [4]).

**Example 2.3.5.3 (Numeric formatting):**

```cursive
let mask = 0b1111_0000u8
let size = 1_024
```

[3] Underscore placement satisfies constraint [5], so both literals are accepted.

### §2.3.6 Diagnostic Summary

| Code    | Condition                                            | Constraint |
| ------- | ---------------------------------------------------- | ---------- |
| E02-208 | Reserved keyword used as identifier                  | [4]        |
| E02-200 | Unterminated string literal                          | [6]        |
| E02-209 | Unterminated block comment                           | [2]        |
| E02-206 | Numeric literal out of range or malformed separators | [5]        |
| E02-201 | Invalid escape sequence                              | [6]        |
| E02-203 | Invalid character literal                            | [6]        |
| E02-210 | Invalid Unicode in identifier                        | [6.3]      |

### §2.3.7 Conformance Requirements

[1] Implementations shall expose token streams (or equivalent APIs) to tooling with location metadata preserved.

[2] Implementations shall detect and report the diagnostics in §2.3.6 at the earliest phase that can do so without suppressing additional diagnostics.

[3] Implementations may extend the set of contextual keywords provided they do not conflict with the reserved set in constraint [4] and provided the contextual keywords remain usable as identifiers outside the specialised contexts that introduce them.
