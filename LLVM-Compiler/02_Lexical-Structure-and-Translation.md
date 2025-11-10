# Cursive Language Specification

## Chapter 2: Lexical Structure and Translation

**Part**: II — Lexical Structure and Translation
**Stable label**: [lex]

**Forward references**: Chapter 3 [basic], Chapter 4 [module], Chapter 5 [decl], Chapter 6 [name], Chapter 7 [type], Chapter 8 [expr], Chapter 9 [stmt], Chapter 10 [generic], Chapter 11 [memory], Chapter 12 [contract], Chapter 13 [witness], Chapter 16 [comptime], Annex A [grammar], Annex B.2 [behavior.undefined], Annex D [limits], Annex E [implementation]

---

### §2.1 Source text and encoding [lex.source]

#### §2.1.1 Overview [lex.source.overview]

Cursive source input is a sequence of Unicode scalar values that must be encoded as UTF-8. All subsequent translation phases (§2.2 [lex.phases]) assume that the implementation has normalized line endings and removed optional metadata such as byte order marks before tokenization (§2.3 [lex.tokens]).

A _source file_ is the decoded Unicode scalar stream after UTF-8 validation and line-ending normalization. A _compilation unit_ is the textual content of a single source file after this preprocessing step (§2.5 [lex.units]).

#### §2.1.2 Syntax [lex.source.syntax]

Source files conform to the following lexical structure prior to tokenization:

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

The symbol `code_point` denotes any Unicode scalar value other than control characters disallowed by the constraints below.

[ Note: See Annex A §A.2 [grammar.lexical] for the complete lexical grammar including source file structure. — end note ]

#### §2.1.3 Constraints [lex.source.constraints]

**UTF-8 validation.** Implementations must accept only byte streams that decode to legal UTF-8 sequences. Invalid byte sequences must elicit diagnostic E02-001.

**Byte order mark handling.** If the byte stream begins with U+FEFF, the implementation must strip the BOM before any further analysis. Any occurrence of U+FEFF after the first decoded scalar value must trigger diagnostic E02-003.

**Line endings.** Implementations must recognize LF (U+000A), CR (U+000D), and CRLF (CR followed by LF) sequences and normalize each to a single LF code point before tokenization. Mixed line endings are permitted; the normalization process preserves the number of logical lines.

**Prohibited code points.** Outside string and character literals, only horizontal tab (U+0009), line feed (U+000A), carriage return (U+000D), and form feed (U+000C) are permitted from Unicode category Cc (control characters). The null character (U+0000) is forbidden everywhere and must raise diagnostic E02-004.

**File size.** Implementations must document an implementation-defined maximum source size; they must accept files of at least 1 MiB. Files exceeding the implementation limit must raise diagnostic E02-002.

#### §2.1.4 Semantics [lex.source.semantics]

After constraint checks succeed, the normalized LF characters become significant tokens that participate in automatic statement termination (§2.4 [lex.terminators]). Horizontal tabs, spaces, and form feeds serve as token separators but are otherwise ignored by lexical analysis.

Unicode normalization is implementation-defined. Implementations should accept any of NFC, NFD, NFKC, or NFKD input; if additional normalization is performed, it must not alter source line boundaries or byte offsets used for diagnostics.

#### §2.1.5 Examples [lex.source.examples]

**Example 2.1.5.1** (Valid source structure):

```cursive
//! Module overview — documentation comment
let greeting = "Hello, world"  // UTF-8 literal
let delta = "Δ"                // Non-ASCII scalar value
```

This example uses LF line endings, no BOM, and only permitted control characters, satisfying all constraints.

**Example 2.1.5.2** (Embedded BOM — invalid):

```cursive
let x = 1
\uFEFFlet y = 2  // ERROR E02-003: BOM after start of file
```

The second line embeds U+FEFF after decoding, violating the BOM constraint.

#### §2.1.6 Conformance [lex.source.conformance]

Implementations must document the maximum supported source file size and the Unicode version they validate against.

Diagnostics E02-001 through E02-004 must be emitted at the location of the offending byte sequence or code point. Suggestions in diagnostic text are informative; the failure remains normative.

Implementations may issue warnings when large files or mixed normalization forms are detected, but such warnings must not replace the mandatory diagnostics described above.

---

### §2.2 Translation phases [lex.phases]

#### §2.2.1 Overview [lex.phases.overview]

Cursive compilation proceeds through a deterministic pipeline of translation phases that convert validated UTF-8 source (§2.1 [lex.source]) into executable artifacts. The phases are designed to expose explicit intermediate products so that diagnostics, tooling, and contracts can observe well-defined boundaries.

Cursive deliberately omits textual preprocessing. The pipeline begins immediately with lexical analysis (§2.3 [lex.tokens]), ensuring that all transformations are performed with full syntactic and semantic context rather than token substitution.

The pipeline follows a **two-phase compilation model**: the parsing phase records the complete set of declarations and their structural metadata without enforcing semantic requirements, and the semantic phases (compile-time execution, type checking, and code generation) run only after parsing succeeds. This model guarantees that declarations may appear in any order within a compilation unit—forward references are resolved during the semantic phases rather than by separate stub declarations.

A program is _well-formed_ precisely when every compilation unit completes each translation phase without producing a fatal diagnostic.

#### §2.2.2 Syntax [lex.phases.syntax]

The sequencing of phases is described by the following grammar; the symbols correspond to the sections in §2.2.4.

```ebnf
phase_pipeline
    ::= parsing
     "→" comptime_execution
     "→" type_checking
     "→" code_generation
     "→" lowering
```

Implementations may internally refactor work, but they must present the externally observable phase boundaries in the order above. Type checking therefore gates code generation: no target code is produced until the program has passed parsing, compile-time execution, and type checking.

#### §2.2.3 Constraints [lex.phases.constraints]

**Phase ordering.** Implementations must execute phases strictly in the order defined by `phase_pipeline`. A phase that emits a diagnostic with severity _error_ must terminate the pipeline for the affected compilation unit; later phases must not observe partially processed artifacts.

**Parsing boundary.** Parsing must complete (successfully or with diagnostics) before compile-time execution begins. Subsequent phases are prohibited from mutating the parse tree in ways that would invalidate the recorded declaration inventory; they may annotate the tree with semantic metadata only.

**Type-checking gate.** Type checking must complete (successfully or with diagnostics) before code generation begins. Generated code is therefore guaranteed to correspond to a well-typed program that satisfies all permission, grant, and contract requirements available at that point in the pipeline.

**Determinism.** The observable results of a phase (diagnostics, generated declarations, lowered IR) must be deterministic with respect to the input compilation unit and compilation configuration.

**Compile-time evaluation limits.** The compile-time execution phase must enforce resource limits at least as permissive as the following minima:

| Resource               | Minimum                     | Diagnostic |
| ---------------------- | --------------------------- | ---------- |
| Recursion depth        | 256 frames                  | E02-101    |
| Evaluation steps       | 1,000,000 per block         | E02-102    |
| Memory allocation      | 64 MiB per compilation unit | E02-103    |
| String size            | 1 MiB                       | E02-104    |
| Collection cardinality | 10,000 elements             | E02-105    |

**Grant safety.** Compile-time execution must refuse any grant that is not listed in the grants clause of the executing item's contractual sequent. Runtime-only capabilities (e.g., file system or network grants as cataloged in §12.3 [contract.grant]) are forbidden and must raise diagnostic E02-106. Compile-time blocks declare their capability requirements explicitly through the same sequent mechanism used by procedures; complete grant semantics are specified in Chapter 12 [contract].

**Generated symbol hygiene.** Code generation must ensure that generated declarations do not collide with declarations already present in the AST. Name collisions must be diagnosed as E02-107.

#### §2.2.4 Semantics [lex.phases.semantics]

##### §2.2.4.1 Parsing [lex.phases.parsing]

The parsing phase consumes the normalized token stream (§2.3 [lex.tokens]) and constructs an abstract syntax tree (AST) that preserves source spans needed for diagnostics and tooling.

Parsing records every declaration, its identifier, structural metadata (e.g., parameter lists, field declarations), and source scope. The AST produced at this stage is the authoritative inventory of declarations for the compilation unit.

Parsing does not attempt semantic validation. All name lookup, type checking, permission and grant checks, and diagnostic enforcement occur during the semantic-analysis phases described below.

Forward references are therefore permitted: declarations may appear after their uses in the source, and mutual recursion is resolved by the later semantic phases.

##### §2.2.4.2 Compile-time execution [lex.phases.comptime]

The compile-time execution phase evaluates `comptime` blocks in dependency order. Implementations build a dependency graph whose nodes represent compile-time blocks and whose edges reflect value or type dependencies discovered during parsing. The graph is constructed only after the parser has completed the compilation unit, ensuring all referenced declarations are known.

Before executing a block, the compiler verifies that all predecessors in the dependency graph have succeeded. Cycles are ill-formed and must be reported as diagnostic E02-100.

Compile-time code executes with access only to explicitly-granted capabilities (`comptime.alloc`, `comptime.codegen`, `comptime.config`, `comptime.diag`). All other grants are rejected per the grant safety constraint.

Complete compile-time execution semantics appear in Chapter 16 [comptime].

##### §2.2.4.3 Type checking [lex.phases.typecheck]

The type-checking phase resolves names, validates type constraints, enforces grant clauses, and checks contracts. Because parsing has already recorded every declaration, name resolution always observes a complete declaration inventory.

[ Note: Module scope formation (§4.3 [module.scope]) occurs during this phase, before name lookup. Wildcard imports (`use module::*`) expand at scope-formation time, after parsing has completed for all modules in the dependency graph. This ensures that the set of exported items is stable before expansion proceeds. — end note ]

Permission checks, modal verification, and contract evaluation are delegated to their respective chapters (memory model, modals, contracts) but are orchestrated from this phase to guarantee that only semantically sound programs proceed to code generation and lowering.

##### §2.2.4.4 Code generation [lex.phases.codegen]

Code generation materializes declarations requested by compile-time execution (e.g., via `codegen::declare_procedure`) and produces backend-specific artifacts only after type checking succeeds. Generated declarations are appended to the AST and annotated so that diagnostics can reference both the generated item and the originating compile-time site.

Hygiene utilities such as `gensym` must be used to avoid collisions; generated code is subject to the same visibility and validation rules as user-authored code.

##### §2.2.4.5 Lowering [lex.phases.lowering]

Lowering transforms the fully-validated AST into an intermediate representation (IR) suitable for target-specific code generation. Compile-time constructs are eliminated, generic constructs are monomorphized, and deterministic cleanup paths (RAII) are made explicit.

Implementations may perform optimization-friendly rewrites during lowering provided the externally observable behavior remains unchanged.

#### §2.2.5 Examples [lex.phases.examples]

**Example 2.2.5.1** (Phase interaction):

```cursive
// Phase 1: Parse comptime block and procedure skeleton
comptime {
    let size = config::get_usize("buffer_size", default: 4096)
    codegen::declare_constant(
        name: "BUFFER_SIZE",
        ty: codegen::type_named("usize"),
        value: quote { #(size) }
    )
}

public procedure create_buffer(): Buffer
    [[ |- true => true ]]
{
    result Buffer { data: make_zeroes(len: BUFFER_SIZE) }
}
```

Parsing records a compile-time block and an incomplete procedure. During compile-time execution the configuration query runs, contributing a constant. Code generation publishes the constant via `declare_constant`; type checking resolves `BUFFER_SIZE` and validates the call to `make_zeroes`; lowering erases the compile-time block and emits IR containing the numeric literal produced at compile time.

#### §2.2.6 Conformance [lex.phases.conformance]

Implementations must provide diagnostics cited in this section and must guarantee that fatal diagnostics terminate the compilation of the affected compilation unit.

Implementations must make the outputs of each phase observable to tools (for example through logs or compiler APIs) so that external tooling can integrate with the pipeline.

Implementations may parallelize translation phases across independent compilation units, but the phases for any single compilation unit must respect the deterministic pipeline defined in this section.

---

### §2.3 Lexical elements [lex.tokens]

#### §2.3.1 Overview [lex.tokens.overview]

Lexical analysis transforms the normalized character stream (§2.1 [lex.source]) into a sequence of tokens consumed by the parser (§2.2 [lex.phases]). Tokens carry a kind, lexeme, and source location, while whitespace and non-documentation comments are discarded.

Documentation comments beginning with `///` or `//!` are retained for later association with declarations; all other comments are removed during this phase.

#### §2.3.2 Syntax [lex.tokens.syntax]

Token categories are defined by Annex A §A.2 [grammar.lexical] and are summarized below:

```ebnf
token
    ::= identifier
     | keyword
     | literal
     | operator
     | punctuator
     | newline
```

Identifiers follow Unicode Standard Annex #31 (XID_Start followed by zero or more XID_Continue code points) and must not collide with reserved keywords.

[ Note: See Annex A §A.2 [grammar.lexical] for complete lexical grammar. — end note ]

#### §2.3.3 Constraints [lex.tokens.constraints]

**Whitespace.** Space (U+0020), horizontal tab (U+0009), and form feed (U+000C) act as token separators and are not emitted as tokens. Line feed (U+000A) is emitted as a `NEWLINE` token for use in statement termination (§2.4 [lex.terminators]).

**Comments.** Line comments starting with `//` consume characters to the next line terminator and are discarded. Block comments delimited by `/*` and `*/` must nest; unclosed block comments raise diagnostic E02-209.

Block comments nest arbitrarily: `/* outer /* inner */ still outer */` is valid and consumes all characters between the outermost `/*` and `*/`. Unclosed nested comments emit diagnostic E02-209. Implementations must track nesting depth and report the nesting level at the point of failure to aid debugging.

**Documentation comments.** Comments beginning with `///` or `//!` must be preserved and attached to the following item or module, respectively. They participate in documentation tooling but do not appear in the token stream.

**Keywords.** The following identifiers are reserved and may not be used as ordinary identifiers:

`abstract`, `as`, `async`, `await`, `behavior`, `break`, `by`, `case`, `comptime`, `const`, `continue`, `contract`, `defer`, `else`, `enum`, `exists`, `false`, `forall`, `grant`, `if`, `import`, `internal`, `invariant`, `let`, `loop`, `match`, `modal`, `module`, `move`, `must`, `new`, `none`, `private`, `procedure`, `protected`, `public`, `record`, `region`, `result`, `select`, `self`, `Self`, `shadow`, `shared`, `state`, `static`, `true`, `type`, `unique`, `var`, `where`, `will`, `with`, `witness`

**Literals.** Numeric literals support decimal, hexadecimal (`0x`), octal (`0o`), and binary (`0b`) prefixes. Underscores `_` may separate digits but must not appear at the start or end of the literal, immediately after a base prefix, or before a type suffix. Integer literals without a suffix default to type `i32`. Violations raise diagnostic E02-206.

Floating-point literals consist of an integer part, optional fractional part, optional exponent, and optional suffix (`f32` or `f64`).

**String and character literals.** Strings are delimited by double quotes and support escape sequences `\n`, `\r`, `\t`, `\\`, `\"`, `\'`, `\0`, `\xNN`, and `\u{...}`. Invalid escapes raise diagnostic E02-201.

Character literals use single quotes and must correspond to a single Unicode scalar value; empty or multi-character literals raise diagnostic E02-203.

String literals may not span multiple lines unless escaped newlines (`\n`) are used. An unclosed string literal that reaches end-of-file or encounters a newline without being closed emits diagnostic E02-200 (unterminated string literal). The lexer must not attempt to recover by inserting a closing quote; the compilation unit is ill-formed.

**Invalid Unicode in identifiers.** Identifiers must consist of valid Unicode XID_Start and XID_Continue code points. If an identifier contains invalid Unicode sequences (e.g., unpaired surrogates, invalid UTF-8), diagnostic E02-210 (invalid Unicode in identifier) is emitted. The lexer must not attempt to repair invalid sequences; the identifier is rejected.

**Operators and punctuators.** Multi-character operators (e.g., `==`, `!=`, `=>`, `..=`, `<-`) are recognized using maximal munch (§2.4 [lex.terminators]). The reference-binding operator `<-` participates in the same precedence and continuation rules as `=` so that binding declarations (§5.2 [decl.variable]) parse unambiguously.

Implementations must disambiguate closing angle brackets in generic type contexts by treating `>>` as two tokens when syntactically required. The glyph `~` is reserved for procedure receiver shorthand (§5.4 [decl.procedure]) and is tokenized as an operator so that combinations such as `~%` and `~!` are available.

#### §2.3.4 Semantics [lex.tokens.semantics]

Tokens retain source-span metadata (file, line, column) so that later phases can provide precise diagnostics and tooling hooks.

Documentation comments preserved by the constraints above are associated with the immediately following declaration unless separated by a blank line.

Invalid lexical constructs must emit the diagnostics listed in §2.3.6 and must not produce tokens; compilation continues so that additional diagnostics can be reported.

#### §2.3.5 Examples [lex.tokens.examples]

**Example 2.3.5.1** (Token stream):

```cursive
// comment
let answer = 42
```

Tokens emitted: `NEWLINE`, `KEYWORD("let")`, `IDENTIFIER("answer")`, `OPERATOR("=")`, `INTEGER_LITERAL("42")`, `NEWLINE`.

**Example 2.3.5.2** (Keyword misuse — invalid):

```cursive
let let = 5  // ERROR E02-208: Reserved keyword used as identifier
```

The second `let` is rejected because keywords cannot serve as identifiers.

**Example 2.3.5.3** (Numeric formatting):

```cursive
let mask = 0b1111_0000u8
let size = 1_024
```

Underscore placement satisfies the constraints, so both literals are accepted.

#### §2.3.6 Diagnostic summary [lex.tokens.diagnostics]

| Code    | Condition                                            | Reference |
| ------- | ---------------------------------------------------- | --------- |
| E02-200 | Unterminated string literal                          | §2.3.3    |
| E02-201 | Invalid escape sequence                              | §2.3.3    |
| E02-203 | Invalid character literal                            | §2.3.3    |
| E02-206 | Numeric literal out of range or malformed separators | §2.3.3    |
| E02-208 | Reserved keyword used as identifier                  | §2.3.3    |
| E02-209 | Unterminated block comment                           | §2.3.3    |
| E02-210 | Invalid Unicode in identifier                        | §2.3.3    |

#### §2.3.7 Conformance [lex.tokens.conformance]

Implementations must expose token streams (or equivalent APIs) to tooling with location metadata preserved.

Implementations must detect and report the diagnostics in §2.3.6 at the earliest phase that can do so without suppressing additional diagnostics.

Implementations may extend the set of contextual keywords provided they do not conflict with the reserved set and provided the contextual keywords remain usable as identifiers outside the specialized contexts that introduce them.

---

### §2.4 Tokenization and statement termination [lex.terminators]

#### §2.4.1 Overview [lex.terminators.overview]

This section refines the behavior of the lexer around newline handling, statement continuation, and the maximal-munch rule used to recognize multi-character operators. It builds on the token categories introduced in §2.3 [lex.tokens] and defines when newline tokens terminate statements implicitly.

#### §2.4.2 Syntax [lex.terminators.syntax]

Statement termination operates on the following abstract grammar:

```ebnf
statement_sequence
    ::= statement (separator)*

separator
    ::= newline
     | semicolon

continuation
    ::= trailing_operator
     | leading_dot
     | leading_pipeline
     | unclosed_delimiter
```

A `newline` that satisfies `continuation` does not terminate the preceding statement; otherwise it behaves as a separator.

[ Note: See Annex A §A.2 [grammar.lexical] for the complete lexical grammar including statement termination rules. — end note ]

#### §2.4.3 Constraints [lex.terminators.constraints]

**Implicit termination.** A newline must terminate the current statement unless one of the continuation predicates in §2.4.4 evaluates to true. The same rule applies at end-of-file.

When multiple continuation rules apply simultaneously (e.g., a trailing operator followed by a leading dot on the next line), the statement continues. The rules are evaluated independently; any matching rule prevents termination. This allows flexible formatting while maintaining predictable behavior.

If end-of-file occurs while a statement is incomplete (unclosed delimiters, trailing operator, or other continuation condition), diagnostic E02-211 (unexpected end of file) is emitted. The lexer must report the location where the statement began and which continuation condition was active.

**Semicolons.** Semicolons may be used to separate multiple statements on a single line but are never required at line breaks.

**Maximal munch.** When multiple tokenizations are possible at a character position, the lexer must emit the longest valid token. In generic type contexts the parser may reinterpret `>>` as two closing angle brackets to satisfy the grammar (§7 [type]).

**Delimiter nesting depth.** Implementations must support delimiter nesting to at least depth 256. Nested delimiters beyond this limit may be rejected with diagnostic E02-300 (delimiter nesting too deep). This limit prevents stack overflow in pathological cases while accommodating realistic code structures.

#### §2.4.4 Semantics [lex.terminators.semantics]

##### §2.4.4.1 Unclosed delimiters [lex.terminators.delimiters]

If the lexer encounters a newline while an opening delimiter `(`, `[`, `{`, or `<` remains unmatched, the newline is treated as whitespace and the statement continues until the delimiter stack is balanced. Diagnostic context highlights the location of the unmatched delimiter when exhaustion occurs.

##### §2.4.4.2 Trailing operators [lex.terminators.trailing]

When a line ends with a binary or assignment operator (`+`, `-`, `*`, `/`, `%`, `**`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`, `&`, `|`, `^`, `<<`, `>>`, `..`, `..=`, `=>`, `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`), the following newline is ignored and the expression continues on the next line.

##### §2.4.4.3 Leading dot [lex.terminators.dot]

A line beginning with `.` is treated as a continuation of the previous expression to support fluent member access. The token `::` does not activate this rule; code using the scope operator must remain on the same line or employ explicit parentheses.

##### §2.4.4.4 Leading pipeline [lex.terminators.pipeline]

A line beginning with the pipeline operator `=>` continues the preceding expression. Pipelines are commonly chained with one entry per line for readability.

##### §2.4.4.5 Maximal munch [lex.terminators.munch]

The lexer applies the maximal-munch rule globally. Examples include recognizing `<<=` as a single left-shift assignment operator and `..=` as an inclusive range operator. When maximal munch conflicts with generic parsing, the parser reinterprets tokens without altering original lexemes, preserving tooling fidelity.

#### §2.4.5 Examples [lex.terminators.examples]

**Example 2.4.5.1** (Continuation rules):

```cursive
let sum = calculate(
    input
)  // newline ignored because '(' was unclosed

let transformed = source
    => normalize
    => render  // leading pipeline

let value = builder
    .step_one()
    .step_two()  // leading dot
```

**Example 2.4.5.2** (Maximal munch vs generics):

```cursive
let shift = value >> 3        // lexer emits single '>>'
let ptr: Ptr<Ptr<i32>> = make_ptr()  // parser splits '>>' into two closers
```

#### §2.4.6 Diagnostic summary [lex.terminators.diagnostics]

| Code    | Condition                                | Reference |
| ------- | ---------------------------------------- | --------- |
| E02-211 | Unexpected end of file during statement  | §2.4.3    |
| E02-300 | Delimiter nesting too deep (exceeds 256) | §2.4.3    |

#### §2.4.7 Conformance [lex.terminators.conformance]

Implementations must expose diagnostics that identify the continuation rule responsible when statement termination behaves unexpectedly (e.g., pointing to a trailing operator or unclosed delimiter).

Implementations must preserve original lexemes even when the parser reinterprets tokens for generic closings so tooling can reconstruct the exact source text.

---

### §2.5 Compilation units and top-level forms [lex.units]

#### §2.5.1 Overview [lex.units.overview]

A compilation unit is the syntactic content of a single source file after preprocessing (§2.1 [lex.source]). This chapter governs the translation of those files; Chapter 4 [module] derives module paths from the file system layout. This section enumerates the declarations permitted at module scope and describes module-level initialization.

#### §2.5.2 Syntax [lex.units.syntax]

Top-level structure is constrained by the following grammar, with non-terminals defined in later chapters:

```ebnf
compilation_unit
    ::= top_level_item*

top_level_item
    ::= import_declaration
     | use_declaration
     | variable_declaration
     | procedure_declaration
     | type_declaration
     | predicate_declaration
     | contract_declaration
     | grant_declaration
```

Expression statements, control-flow constructs (`if`, `match`, `loop`, etc.), and local bindings are not permitted at module scope.

[ Note: Constants are expressed via type qualifiers on variable declarations (`let x: const Y = 0`) rather than separate declaration forms. Grant declarations (§5.9 [decl.grant]) introduce user-defined capability tokens for use in procedure contractual sequent specifications. — end note ]

#### §2.5.3 Constraints [lex.units.constraints]

**Visibility.** Declarations without an explicit visibility modifier default to `internal`. The modifiers `public`, `internal`, `private`, and `protected` control accessibility across modules and packages; see Chapter 4 [module] for re-exporting behavior and §5.6 [decl.visibility] for complete visibility rules.

**Uniqueness.** The names introduced by top-level items must be unique within the compilation unit. Redeclaration without explicit `shadow` is diagnosed as E02-400.

**Forward references.** Forward references to declarations in the same compilation unit are permitted because the parser records declarations before type checking (§2.2 [lex.phases]).

**Disallowed forms.** Expression statements, control-flow constructs, and block scopes at module level must raise diagnostic E02-301.

**Empty compilation units.** A compilation unit may contain zero top-level items. An empty compilation unit defines a valid module with no exports. Such modules may be imported for their side effects (e.g., module-level initialization) but contribute no bindings to the importing module's namespace.

A compilation unit containing only whitespace, line comments, block comments, or documentation comments (with no top-level items) is treated as an empty compilation unit. No diagnostic is emitted for such files.

#### §2.5.4 Semantics [lex.units.semantics]

##### §2.5.4.1 Module-level initialization [lex.units.init]

Module-level `let` and `var` bindings are initialized before the program entry point executes. Implementations construct a dependency graph between initializers and evaluate bindings in topological order. Cycles are rejected with diagnostic E02-401.

Initializers execute exactly once per program instance. Mutable `var` bindings retain their state for the life of the program unless explicitly mutated or reset by user code.

##### §2.5.4.2 Entry point [lex.units.entry]

Executable programs must provide exactly one procedure named `main`. The canonical forms and required diagnostics are specified in §5.8 [decl.entry], which owns entry-point validation (codes E05-801 through E05-804).

#### §2.5.5 Examples [lex.units.examples]

**Example 2.5.5.1** (Valid compilation unit):

```cursive
use std::io::println

public record Point { x: f64, y: f64 }

let ORIGIN: Point = Point { x: 0.0, y: 0.0 }

public procedure distance(a: Point, b: Point): f64
    [[ |- true => true ]]
{
    let dx = b.x - a.x
    let dy = b.y - a.y
    result (dx * dx + dy * dy).sqrt()
}
```

This example contains only permitted top-level items and relies on the implicit initialization rules.

**Example 2.5.5.2** (Expression statement at module scope — invalid):

```cursive
let value = 5
value + 1  // ERROR E02-301: Expression statement at module scope
```

Expression statements are not permitted at module scope.

#### §2.5.6 Conformance [lex.units.conformance]

Implementations must expose APIs or metadata that identify the module path corresponding to each compilation unit so tooling can correlate files with module identifiers.

Diagnostically, implementations must detect redeclarations (E02-400), prohibited constructs (E02-301), and entry-point violations (E05-801 through E05-804, see §5.8 [decl.entry]) during or before type checking.

Implementations may lazily evaluate module-level initializers provided that observable behavior matches the eager semantics described above.

---

**Previous**: Chapter 1 — Introduction and Conformance (§1 [intro]) | **Next**: Chapter 3 — Basic Concepts (§3 [basic])
