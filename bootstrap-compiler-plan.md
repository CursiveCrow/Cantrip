# Cursive Bootstrap Compiler Plan

## Specification Validation Summary

This plan was validated against the Cursive Language Specification (Draft 3) at:
`C:\Dev\Cursive\Cursive-Language-Specification\CursiveLanguageSpecification.md`

**Verified Against Specification:**
- Keywords (§2.6): 68 keywords per official list (lines 695-706) - verified
- Operators (§2.7): Multi-char and single-char operators - verified
- Method dispatch syntax (§9.2, §12.4): Uses `~>` operator, NOT `.method()` - verified
- Class implementation (§10.3): Inline with type definition via `<:`, single block contains BOTH fields AND methods (see example at spec line 8501-8510) - verified (§5.3 EBNF corrected to include `method_def`)
- Operator precedence (§12.3): 14 levels (not 17) - verified
- Entry point (§8.9): `public procedure main(ctx: Context) -> i32` - verified
- Modal types (§6.1): State syntax `@State { state_member* }` where `state_member` can be `field_decl | method_def | transition_def` - single block with fields/methods/transitions freely intermixed (verified against EBNF at lines 3546-3556)
- String states (§6.2): `@Managed`, `@View` - verified
- Ptr<T> states (§6.3): `@Valid`, `@Null`, `@Expired` - verified
- Pattern syntax (§12.2): Modal patterns use `@State { fields }` - verified
- Binding syntax (§12.8, §3.4): `let`/`var` with `=`/`:=` - verified
- Receiver shorthand (§9.2): `~` (const), `~!` (unique), `~%` (shared) - verified
- Parameter modes (§9.1): default (const), `unique`, `shared`, `move` - verified

---

## Executive Summary

Building a minimal bootstrap compiler for Cursive using C and LLVM. The compiler implements a subset sufficient to write a self-hosting Cursive compiler. Once self-hosted, the full language can be implemented in Cursive.

**Target Platforms**: Windows x86-64, Linux x86-64
**Testing Strategy**: Bootstrap-focused (tests designed around compiling the Cursive compiler)
**Approach**: Minimal self-hosting - defer concurrency, async, metaprogramming to Phase 2

## Key Design Decisions

### Permission System Simplification
The full Cursive permission system has three levels: `const`, `unique`, `shared`.

- **`shared` permission** requires the **key system** (Clause 14) for synchronized concurrent access
- The key system is only needed for **concurrent** code (parallel blocks, spawn)
- **For a single-threaded bootstrap compiler**, we can **omit `shared` entirely**

**Bootstrap Permission Model**:
- `const` - Read-only access, unlimited aliasing (default)
- `unique` - Exclusive read-write access, no aliasing
- `shared` - **DEFERRED** to Phase 2 (requires key system + concurrency)

This eliminates: Key system, parallel blocks, spawn, dispatch, all of Clause 14-15.

### Deferred Features (Phase 2)
The following are deferred until the compiler is self-hosting:
- `shared` permission and key system (Clauses 14)
- Parallel blocks, spawn, dispatch (Clause 15)
- Async/await (Clause 16)
- Comptime execution and metaprogramming (Clauses 17-20)
- Contract verification beyond simple assertions (Clause 11)
- Refinement types (§7.3)
- Dynamic polymorphism `dyn` (§10.5)
- Opaque types (§10.6)

## Specification Overview

The Cursive Language Specification (Draft 3) defines a comprehensive systems programming language with:

### Core Language Features (21 Clauses)
1. **Lexical Structure** (Clause 2): UTF-8 source, Unicode identifiers, 68 keywords, operators, literals
2. **Object/Memory Model** (Clause 3): Provenance tracking, RAII, move semantics, regions, unsafe blocks
3. **Type System** (Clauses 4-7): Nominal/structural types, permission types, variance, generics, attributes
4. **Modal Types** (Clause 6.1): State machines in the type system with compile-time state tracking
5. **Modules** (Clause 8): Module system with visibility (public/private/protected/internal)
6. **Procedures** (Clause 9): Procedures, methods, receivers (~, ~!), overloading
7. **Classes** (Clause 10): Classes/interfaces with static polymorphism (monomorphization)
8. **Contracts** (Clause 11): Preconditions (|=), postconditions (=>), invariants - *simplified for bootstrap*
9. **Expressions/Statements** (Clause 12): Pattern matching, operators, control flow, defer
10. **Capabilities** (Clause 13): No ambient authority - all I/O requires explicit capabilities
11. **~~Concurrency~~** (Clauses 14-15): *DEFERRED* - key system, parallel blocks, spawn, dispatch
12. **~~Async~~** (Clause 16): *DEFERRED* - Async/Future/Stream patterns
13. **~~Metaprogramming~~** (Clauses 17-20): *DEFERRED* - comptime, reflection, quote/emit, derive
14. **FFI** (Clause 21): C interop with FfiSafe class, extern blocks

---

## Bootstrap Compiler Architecture

### Bootstrap Feature Set

The bootstrap compiler implements a **subset** of Cursive sufficient to write a self-hosting compiler:

| Feature | Status | Notes |
|---------|--------|-------|
| Lexer (full) | **INCLUDE** | UTF-8, all tokens, keywords |
| Parser (full syntax) | **INCLUDE** | All syntactic constructs |
| Modules & imports | **INCLUDE** | Full module system |
| Records & Enums | **INCLUDE** | Nominal types |
| Tuples, Arrays, Slices | **INCLUDE** | Structural types |
| Union types | **INCLUDE** | Error handling pattern |
| Modal types | **INCLUDE** | Ptr<T>, string, user-defined |
| Generics | **INCLUDE** | Monomorphization |
| Classes (static) | **INCLUDE** | Static polymorphism only |
| `const` permission | **INCLUDE** | Default, read-only |
| `unique` permission | **INCLUDE** | Exclusive mutable |
| `shared` permission | **DEFER** | Requires key system |
| Move semantics | **INCLUDE** | Binding states, RAII |
| Pattern matching | **INCLUDE** | Full pattern syntax |
| Capabilities | **INCLUDE** | Context, $FileSystem, etc. |
| FFI | **INCLUDE** | extern "C" blocks |
| Contracts | **PARTIAL** | Runtime assertions only |
| Concurrency | **DEFER** | Key system, parallel, spawn |
| Async | **DEFER** | Future, await |
| Comptime | **DEFER** | Metaprogramming |

### Component Breakdown

#### 1. Lexer
**Specification Reference**: Clause 2

**Requirements**:
- UTF-8 decoding (§2.1) with BOM handling
- Line normalization: CR/LF/CRLF -> LF (§2.2)
- Token types from §2.4-2.9:
  - Identifiers (Unicode XID_Start/XID_Continue)
  - Keywords (68 per §2.6): `and`, `as`, `async`, `atomic`, `break`, `class`, `comptime`, `const`, `continue`, `defer`, `dispatch`, `do`, `drop`, `else`, `emit`, `enum`, `escape`, `extern`, `false`, `for`, `gpu`, `if`, `import`, `in`, `interrupt`, `let`, `loop`, `match`, `modal`, `mod`, `module`, `move`, `mut`, `override`, `parallel`, `pool`, `private`, `procedure`, `protected`, `public`, `quote`, `record`, `region`, `result`, `return`, `select`, `self`, `Self`, `set`, `shared`, `simd`, `spawn`, `sync`, `then`, `transition`, `transmute`, `true`, `type`, `union`, `unique`, `unsafe`, `using`, `var`, `volatile`, `where`, `while`, `widen`, `yield`
  - Multi-char operators: `==`, `!=`, `<=`, `>=`, `&&`, `||`, `<<`, `>>`, `<<=`, `>>=`, `..`, `..=`, `=>`, `->`, `**`, `::`, `:=`, `|=`, `~>`, `~!`, `~%`
  - Single-char operators: `+`, `-`, `*`, `/`, `%`, `<`, `>`, `=`, `!`, `&`, `|`, `^`, `~`, `.`, `?`, `#`, `@`
  - Punctuators: `(`, `)`, `[`, `]`, `{`, `}`, `,`, `:`, `;`
  - Literals: integers (decimal, hex, octal, binary), floats, strings, chars, bool
  - Receiver tokens: `~` (const), `~!` (unique), `~%` (shared)
  - Region allocation: `^` (implicit), `r^` (named region)
- Comment handling: `//`, `///`, `//!`, nested `/* */`
- Statement termination rules (§2.11)
- Source spans for error reporting

**Output**: Token stream with source locations

#### 2. Parser
**Specification Reference**: Clause 3 (syntax), Clause 12 (expressions)

**AST Node Types**:
```
Module, Import, Use
TypeDecl: Record, Enum, Modal, TypeAlias
ProcDecl, MethodDecl (methods defined within type blocks)
ClassDecl (no separate ClassImpl - implementations are inline with <:)
LetBinding, VarBinding
Expr: Literal, Identifier, Binary, Unary, Call (function call: f(args)),
      MethodCall (uses ~>: receiver~>method(args)),
      FieldAccess (uses .: expr.field),
      Index, Tuple, Array, RecordLit, Match, If, Block,
      Move, Widen, Cast, Range, StaticCall (Type::method)
Stmt: Expr, Let, Var, Assign, Return, Result, Break, Continue,
      Loop, If, Match, Block, Defer, Unsafe
Pattern: Literal, Binding, Wildcard, Tuple, Record, Enum, Modal, Or, Guard
Type: Primitive, Named, Generic, Tuple, Array, Slice, Function, Union, Modal(@State)
```

**Key Parsing Challenges**:
- Operator precedence (§12.3): 14 precedence levels (Postfix to Assignment)
- Generic argument disambiguation: `>>` as two `>` in generic context
- Modal type syntax: `Type@State`
- Method dispatch uses `~>` operator (NOT `.method()`): `receiver~>method(args)`
- Distinguish: `.identifier` (field access) vs `~>identifier(args)` (method call)
- Pattern matching exhaustiveness (static check deferred)
- Union return types: `T | E1 | E2`
- Compound assignment operators: `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`

#### 3. Name Resolution
**Specification Reference**: Clause 8

**Scope Management**:
- Universe scope (primitives, built-in types)
- Module scope (top-level declarations)
- Block scope (local bindings)
- Type scope (for Self, generics)

**Visibility Rules** (§8.5):
- `public` - visible everywhere
- `internal` - visible within crate
- `protected` - visible in module and submodules
- `private` - visible only in declaring module

**Import Resolution**:
- `import module::path`
- `use module::path::{a, b, c}`
- `use module::path::*` (glob)

#### 4. Type System
**Specification Reference**: Clauses 4-6

**Type Representation**:
```c
typedef struct Type {
    TypeKind kind;
    union {
        PrimitiveKind prim;      // i32, bool, etc.
        struct Record *record;   // nominal record
        struct Enum *enum_;      // nominal enum
        struct Modal *modal;     // modal type
        struct {                 // M@State
            struct Modal *modal;
            Symbol state;
        } modal_state;
        Type **tuple;            // (T1, T2, ...)
        struct { Type *elem; size_t len; } array;  // [T; N]
        Type *slice;             // [T]
        struct { Type **members; size_t count; } union_;  // T | U | V
        struct {                 // (T1, ...) -> R
            Type **params;
            size_t param_count;
            Type *return_type;
        } function;
        struct {                 // Generic instantiation
            Type *base;
            Type **args;
            size_t arg_count;
        } generic;
    };
    Permission perm;             // const, unique (no shared in bootstrap)
} Type;
```

**Type Checking Rules**:
- Nominal equivalence for records, enums, modals
- Structural equivalence for tuples, arrays, unions, functions
- Subtyping: `!` (never) <: all types
- Permission subtyping: `unique` <: `const` (simplified lattice)
- Union member subtyping
- Generic instantiation and constraint checking

#### 5. Permission Checking
**Specification Reference**: §4.5 (simplified)

**Bootstrap Permission Model**:
- `const` (default): Read-only access, unlimited aliases
- `unique`: Exclusive read-write, no aliases

**Checks**:
- Mutation through `const` path -> E-TYP-1601
- Aliasing of `unique` path -> E-TYP-1602
- Method receiver compatibility (~ vs ~!)

#### 6. Move Semantics & Binding Analysis
**Specification Reference**: §3.4-3.6

**Binding State Machine**:
```
Uninitialized -> Valid (via initialization)
Valid -> Moved (via move expression)
Valid -> PartiallyMoved (via field move)
Moved -> Valid (via reassignment for var only)
```

**Data Flow Analysis**:
- Track binding states at each program point
- Control flow merge: take least-valid state
- Ensure all paths initialize before use
- Ensure no access after move
- Track partial moves from records

**Drop Insertion**:
- Insert drop calls at scope exits
- LIFO order (reverse declaration order)
- Skip moved bindings
- Handle early returns and panics

#### 7. Modal Type Checking
**Specification Reference**: §6.1-6.3

**Built-in Modal Types**:
- `Ptr<T>`: @Valid, @Null, @Expired
- `string`: @Managed, @View

**Modal Type Checks**:
- State-specific field access: only fields in current state's payload
- State-specific method calls: only methods in current state
- Transition typing: consumes source state, produces target state
- Exhaustive match on general modal type
- Widening: M@S -> M (explicit `widen` or implicit if niche-compatible)

#### 8. Code Generation (LLVM)
**Specification Reference**: Clause 21 (for ABI), general semantics throughout

**LLVM-C API Usage**:
```c
// Module and context
LLVMContextRef ctx = LLVMContextCreate();
LLVMModuleRef mod = LLVMModuleCreateWithNameInContext("main", ctx);
LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

// Type lowering
LLVMTypeRef lower_type(Type *t);  // Cursive type -> LLVM type

// Function generation
LLVMValueRef codegen_proc(ProcDecl *proc);

// Expression codegen
LLVMValueRef codegen_expr(Expr *e, LLVMBuilderRef b);

// Statement codegen
void codegen_stmt(Stmt *s, LLVMBuilderRef b);
```

**Type Lowering**:
| Cursive Type | LLVM Type |
|--------------|-----------|
| i8..i128 | i8..i128 |
| u8..u128 | i8..i128 (same, unsigned ops) |
| f16/f32/f64 | half/float/double |
| bool | i1 |
| char | i32 |
| () | void |
| ! | void (noreturn functions) |
| record | named struct |
| enum | tagged union (i8/i16/i32 tag + payload) |
| tuple | anonymous struct |
| array | [N x T] |
| slice | {ptr, len} struct |
| Ptr<T>@Valid | T* |
| Ptr<T>@Null | T* (guaranteed null) |
| string@View | {i8*, i64} |
| string@Managed | {i8*, i64, i64} |

**Control Flow**:
- if/else: conditional branch
- match: switch or chained conditionals
- loop: phi nodes, back edges
- break/continue: branch to loop exit/header
- return/result: ret instruction

#### 9. Runtime Library (Minimal)
**Files**: `runtime/cursive_rt.c`, `runtime/cursive_rt.h`

**Required Functions**:
```c
// Panic/abort
void cursive_panic(const char *msg, const char *file, int line);

// Memory (wrapper for system allocator)
void *cursive_alloc(size_t size, size_t align);
void cursive_dealloc(void *ptr, size_t size, size_t align);

// String operations
typedef struct { const char *ptr; size_t len; } cursive_string_view;
typedef struct { char *ptr; size_t len; size_t cap; } cursive_string_managed;
cursive_string_managed cursive_string_from_view(cursive_string_view v);
void cursive_string_drop(cursive_string_managed *s);
```

**Capability Stubs**:
For bootstrap, capabilities can be thin wrappers around system calls:
```c
// $FileSystem capability stub
int cursive_fs_open(const char *path, int flags);
ssize_t cursive_fs_read(int fd, void *buf, size_t count);
ssize_t cursive_fs_write(int fd, const void *buf, size_t count);
int cursive_fs_close(int fd);
```

---

## Implementation Stages

### Stage 1: Project Setup & Core Infrastructure

**Directory Structure**:
```
cursive-bootstrap/
├── CMakeLists.txt
├── src/
│   ├── main.c              # Entry point
│   ├── common/
│   │   ├── arena.h/.c      # Arena allocator for AST
│   │   ├── string.h/.c     # String interning
│   │   ├── vec.h/.c        # Dynamic arrays
│   │   ├── map.h/.c        # Hash map
│   │   └── error.h/.c      # Error reporting
│   ├── lexer/
│   │   ├── lexer.h/.c
│   │   ├── token.h/.c
│   │   └── unicode.h/.c    # UTF-8 handling
│   ├── parser/
│   │   ├── ast.h/.c
│   │   ├── parser.h/.c
│   │   └── pretty.h/.c     # AST printer
│   ├── sema/                # Semantic analysis
│   │   ├── scope.h/.c
│   │   ├── types.h/.c
│   │   ├── resolve.h/.c    # Name resolution
│   │   ├── typecheck.h/.c
│   │   ├── moves.h/.c      # Move analysis
│   │   └── perms.h/.c      # Permission checking
│   ├── codegen/
│   │   ├── lower.h/.c      # Type lowering
│   │   ├── codegen.h/.c    # LLVM IR gen
│   │   └── target.h/.c     # Target configuration
│   └── runtime/
│       ├── cursive_rt.h
│       └── cursive_rt.c
├── tests/
│   ├── lexer/
│   ├── parser/
│   ├── sema/
│   ├── codegen/
│   └── e2e/                # End-to-end tests
└── bootstrap-lib/          # Minimal Cursive stdlib for bootstrap
```

**Core Infrastructure**:
```c
// Arena allocator for AST memory
typedef struct Arena {
    char *base;
    size_t used;
    size_t capacity;
} Arena;

void *arena_alloc(Arena *a, size_t size, size_t align);

// String interning
typedef struct StringPool {
    // Hash table of interned strings
} StringPool;

typedef struct InternedString {
    const char *data;
    size_t len;
    uint64_t hash;
} InternedString;

// Source location tracking
typedef struct SourceLoc {
    uint32_t file_id;
    uint32_t line;
    uint32_t col;
} SourceLoc;

typedef struct SourceSpan {
    SourceLoc start;
    SourceLoc end;
} SourceSpan;

// Error reporting
typedef struct Diagnostic {
    DiagLevel level;  // Error, Warning, Note
    const char *code; // E-TYP-1601, etc.
    SourceSpan span;
    char *message;
} Diagnostic;
```

### Stage 2: Lexer
**Files**: `src/lexer/*`

**Token Definition**:
```c
typedef enum TokenKind {
    // Literals
    TOK_INT_LIT, TOK_FLOAT_LIT, TOK_STRING_LIT, TOK_CHAR_LIT,
    TOK_TRUE, TOK_FALSE,

    // Keywords (67) - partial list, see §2.6 for complete set
    TOK_LET, TOK_VAR, TOK_PROCEDURE, TOK_RECORD, TOK_ENUM,
    TOK_MODAL, TOK_CLASS, TOK_IF, TOK_ELSE, TOK_MATCH,
    TOK_LOOP, TOK_BREAK, TOK_CONTINUE, TOK_RETURN, TOK_RESULT,
    TOK_MOVE, TOK_CONST, TOK_UNIQUE, TOK_SHARED, TOK_UNSAFE,
    TOK_EXTERN, TOK_IMPORT, TOK_USE, TOK_PUBLIC, TOK_PRIVATE,
    TOK_PROTECTED, TOK_INTERNAL, TOK_TYPE, TOK_WHERE,
    TOK_TRANSITION, TOK_WIDEN, TOK_DEFER, TOK_REGION,
    TOK_SPAWN, TOK_PARALLEL, TOK_DISPATCH, TOK_OVERRIDE,
    TOK_AND, TOK_AS, TOK_ASYNC, TOK_ATOMIC, TOK_COMPTIME,
    TOK_DO, TOK_DROP, TOK_EMIT, TOK_ESCAPE, TOK_FOR, TOK_GPU,
    TOK_IN, TOK_INTERRUPT, TOK_MOD, TOK_MODULE, TOK_MUT,
    TOK_POOL, TOK_QUOTE, TOK_SELECT, TOK_SELF, TOK_SELF_TYPE,
    TOK_SET, TOK_SIMD, TOK_SYNC, TOK_THEN, TOK_TRANSMUTE,
    TOK_UNION, TOK_USING, TOK_VOLATILE, TOK_WHILE, TOK_YIELD,

    // Multi-char operators
    TOK_EQEQ,      // ==
    TOK_NE,        // !=
    TOK_LE,        // <=
    TOK_GE,        // >=
    TOK_AMPAMP,    // &&
    TOK_PIPEPIPE,  // ||
    TOK_LTLT,      // <<
    TOK_GTGT,      // >>
    TOK_LTLTEQ,    // <<=
    TOK_GTGTEQ,    // >>=
    TOK_DOTDOT,    // ..
    TOK_DOTDOTEQ,  // ..=
    TOK_FATARROW,  // =>
    TOK_ARROW,     // ->
    TOK_STARSTAR,  // **
    TOK_COLONCOLON,// ::
    TOK_COLONEQ,   // :=
    TOK_PIPEEQ,    // |= (contract precondition)
    TOK_TILDEGT,   // ~> (method dispatch)
    TOK_TILDEEXCL, // ~! (unique receiver)
    TOK_TILDEPCT,  // ~% (shared receiver)

    // Compound assignment
    TOK_PLUSEQ, TOK_MINUSEQ, TOK_STAREQ, TOK_SLASHEQ, TOK_PERCENTEQ,
    TOK_AMPEQ, TOK_CARETEQ,  // |= is also compound assignment in context

    // Single-char operators
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_EQ, TOK_LT, TOK_GT, TOK_BANG, TOK_AMP, TOK_PIPE,
    TOK_CARET, TOK_TILDE, TOK_DOT, TOK_QUESTION, TOK_HASH, TOK_AT,

    // Punctuators
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET,
    TOK_LBRACE, TOK_RBRACE, TOK_COMMA, TOK_COLON, TOK_SEMI,

    // Special
    TOK_IDENT, TOK_NEWLINE, TOK_EOF, TOK_ERROR
} TokenKind;

typedef struct Token {
    TokenKind kind;
    SourceSpan span;
    union {
        InternedString ident;
        uint64_t int_val;
        double float_val;
        InternedString string_val;
        uint32_t char_val;
    };
} Token;
```

**Lexer State Machine**:
```c
typedef struct Lexer {
    const char *source;
    size_t source_len;
    size_t pos;
    uint32_t line;
    uint32_t col;
    StringPool *strings;
    Vec(Diagnostic) diagnostics;
} Lexer;

Token lexer_next(Lexer *l);
Token lexer_peek(Lexer *l);
```

### Stage 3: Parser & AST
**Files**: `src/parser/*`

**AST Core Types**:
```c
// Forward declarations
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Type Type;
typedef struct Pattern Pattern;
typedef struct Decl Decl;

// Expression nodes
typedef enum ExprKind {
    EXPR_LITERAL, EXPR_IDENT, EXPR_BINARY, EXPR_UNARY,
    EXPR_CALL, EXPR_METHOD_CALL, EXPR_FIELD, EXPR_INDEX,
    EXPR_TUPLE, EXPR_ARRAY, EXPR_RECORD_LIT, EXPR_MATCH,
    EXPR_IF, EXPR_BLOCK, EXPR_MOVE, EXPR_WIDEN, EXPR_CAST,
    EXPR_RANGE, EXPR_CLOSURE
} ExprKind;

struct Expr {
    ExprKind kind;
    SourceSpan span;
    Type *resolved_type;  // Filled in by type checker
    // ... per-kind fields
};

// Pattern nodes
typedef enum PatternKind {
    PAT_WILDCARD, PAT_BINDING, PAT_LITERAL, PAT_TUPLE,
    PAT_RECORD, PAT_ENUM, PAT_MODAL, PAT_OR, PAT_GUARD
} PatternKind;

// Declaration nodes
typedef enum DeclKind {
    DECL_PROC,        // Standalone procedure
    DECL_RECORD,      // Record type (may implement classes via <:)
    DECL_ENUM,        // Enum type (may implement classes via <:)
    DECL_MODAL,       // Modal type (may implement classes via <:)
    DECL_TYPE_ALIAS,  // type Foo = Bar;
    DECL_CLASS,       // Class declaration (interface)
    DECL_EXTERN,      // extern "C" { ... } block
    DECL_MODULE,      // module declaration
    DECL_IMPORT,      // import statement
    DECL_USE          // use statement
} DeclKind;

// Note: Class implementations are INLINE with type declarations.
// There is no separate "impl" block in Cursive.
// A single block contains BOTH fields AND methods:
// Example: record Point <: Printable { x: f64, y: f64, procedure print(~) { ... } }
// The Record/Enum/Modal AST nodes include:
//   - implements: list of class bounds (from <:)
//   - fields: field declarations
//   - methods: procedure definitions (including class implementations)
```

**Parser API**:
```c
typedef struct Parser {
    Lexer *lexer;
    Token current;
    Token peek;
    Arena *ast_arena;
    Vec(Diagnostic) diagnostics;
} Parser;

// Top-level parsing
Module *parse_module(Parser *p);

// Declaration parsing
Decl *parse_decl(Parser *p);
ProcDecl *parse_proc_decl(Parser *p);
RecordDecl *parse_record_decl(Parser *p);
EnumDecl *parse_enum_decl(Parser *p);
ModalDecl *parse_modal_decl(Parser *p);

// Expression parsing (Pratt parser)
Expr *parse_expr(Parser *p);
Expr *parse_expr_prec(Parser *p, int min_prec);

// Type parsing
TypeExpr *parse_type(Parser *p);

// Pattern parsing
Pattern *parse_pattern(Parser *p);
```

### Stage 4: Name Resolution
**Files**: `src/sema/scope.c`, `src/sema/resolve.c`

**Scope Structure**:
```c
typedef enum ScopeKind {
    SCOPE_UNIVERSE,  // Built-in types
    SCOPE_MODULE,    // Top-level module
    SCOPE_BLOCK,     // Local block
    SCOPE_TYPE,      // Type parameters, Self
} ScopeKind;

typedef struct Scope {
    ScopeKind kind;
    struct Scope *parent;
    Map symbols;  // InternedString -> Symbol*
} Scope;

typedef struct Symbol {
    SymbolKind kind;
    InternedString name;
    Decl *decl;
    Visibility vis;
} Symbol;
```

### Stage 5: Type System & Checking
**Files**: `src/sema/types.c`, `src/sema/typecheck.c`

**Core Type Checking Functions**:
```c
// Type checking main entry
void typecheck_module(Module *m, TypeContext *ctx);

// Expression type checking
Type *typecheck_expr(Expr *e, TypeContext *ctx, Type *expected);

// Statement type checking
void typecheck_stmt(Stmt *s, TypeContext *ctx);

// Type equivalence
bool types_equal(Type *a, Type *b);

// Subtyping
bool is_subtype(Type *sub, Type *super);

// Generic instantiation
Type *instantiate_generic(Type *generic, Type **args, size_t count);
```

### Stage 6: Move Analysis & Permission Checking
**Files**: `src/sema/moves.c`, `src/sema/perms.c`

**Binding State Analysis**:
```c
typedef enum BindingState {
    BIND_UNINITIALIZED,
    BIND_VALID,
    BIND_MOVED,
    BIND_PARTIALLY_MOVED
} BindingState;

typedef struct BindingInfo {
    Symbol *sym;
    BindingState state;
    bool is_movable;  // = vs :=
    bool is_mutable;  // var vs let
    Permission perm;
} BindingInfo;

// Data flow analysis for moves
void analyze_moves(ProcDecl *proc, MoveContext *ctx);

// Permission checking
void check_permissions(Expr *e, PermContext *ctx, Permission required);
```

### Stage 7: Code Generation
**Files**: `src/codegen/*`

**LLVM Codegen Structure**:
```c
typedef struct CodegenContext {
    LLVMContextRef llvm_ctx;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMTargetDataRef target_data;

    Map type_cache;     // Type* -> LLVMTypeRef
    Map func_cache;     // Symbol* -> LLVMValueRef
    Map local_values;   // Symbol* -> LLVMValueRef (stack slots)

    // Current function context
    LLVMValueRef current_func;
    LLVMBasicBlockRef entry_block;
    LLVMBasicBlockRef exit_block;  // For cleanups
    Vec(DropInfo) drop_stack;      // Pending drops
} CodegenContext;

// Main codegen entry
void codegen_module(Module *m, CodegenContext *ctx);

// Procedure codegen
void codegen_proc(ProcDecl *p, CodegenContext *ctx);

// Expression codegen
LLVMValueRef codegen_expr(Expr *e, CodegenContext *ctx);

// Statement codegen
void codegen_stmt(Stmt *s, CodegenContext *ctx);

// Type lowering
LLVMTypeRef lower_type(Type *t, CodegenContext *ctx);
```

### Stage 8: Driver & Integration
**Files**: `src/main.c`

**Compiler Driver**:
```c
int main(int argc, char **argv) {
    // 1. Parse command line arguments
    CompilerOptions opts = parse_args(argc, argv);

    // 2. Initialize LLVM
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();

    // 3. For each source file:
    for (size_t i = 0; i < opts.source_count; i++) {
        // 3a. Read source
        char *source = read_file(opts.sources[i]);

        // 3b. Lex
        Lexer lexer = lexer_new(source);

        // 3c. Parse
        Parser parser = parser_new(&lexer);
        Module *ast = parse_module(&parser);

        // 3d. Name resolution
        resolve_names(ast);

        // 3e. Type checking
        typecheck_module(ast);

        // 3f. Move analysis
        analyze_moves(ast);

        // 3g. Code generation
        codegen_module(ast, &codegen_ctx);
    }

    // 4. Link and output
    output_executable(codegen_ctx.module, opts.output);

    return 0;
}
```

---

## Key Technical Challenges

### 1. Permission System (Simplified for Bootstrap)
With only `const` and `unique`:
- Track permission at each path (e.g., `x.field.subfield`)
- Mutation through `const` path is always an error
- `unique` path cannot be aliased - detect any attempts to create second reference
- Downgrade: `unique` can be passed where `const` expected (implicit coercion)

### 2. Modal Type State Tracking
- Each binding with modal type has a known state
- State changes only via transition methods
- Pattern match narrows general type to specific state
- Exhaustive match checking: all states must be covered

### 3. Move Semantics Implementation
- Build control flow graph for each procedure
- Forward data flow: track binding states
- At merge points: state = min(incoming states)
- Drop insertion pass after all analysis complete

### 4. Generic Monomorphization
- Delay instantiation until call site
- Cache instantiated types and functions
- Handle recursive generics (limit depth)
- Class constraints checked at instantiation

### 5. Union Type Representation
- Canonicalize member order (sort by type hash)
- Tagged union: discriminant + max-size payload
- Niche optimization where applicable

---

## Build & Dependencies

### CMake Configuration
```cmake
cmake_minimum_required(VERSION 3.16)
project(cursive-bootstrap C)

set(CMAKE_C_STANDARD 17)

# Find LLVM
find_package(LLVM 15 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Compiler sources
add_executable(cursivec
    src/main.c
    src/common/arena.c
    src/common/string.c
    # ... all source files
)

# Link LLVM
llvm_map_components_to_libnames(llvm_libs core support native)
target_link_libraries(cursivec ${llvm_libs})

# Runtime library
add_library(cursive_rt STATIC
    src/runtime/cursive_rt.c
)
```

### Platform-Specific Considerations

**Windows (MSVC or MinGW)**:
- Use `_aligned_malloc`/`_aligned_free` for aligned allocation
- Handle path separators (\ vs /)
- UTF-16 filesystem APIs

**Linux (GCC/Clang)**:
- Use `aligned_alloc` or `posix_memalign`
- POSIX filesystem APIs

---

## Testing Strategy

### Bootstrap-Focused Tests

**Level 1: Component Tests**
- Lexer: tokenize sample inputs, verify token sequences
- Parser: parse + pretty-print round-trip
- Type checker: type small programs, verify diagnostics

**Level 2: Integration Tests**
- Compile small programs, verify output
- Test each language feature in isolation

**Level 3: Bootstrap Test**
- Compile a simplified Cursive compiler written in Cursive
- Verify it produces correct output
- Eventually: self-compile the bootstrap compiler

### Test File Format
```cursive
// test/e2e/fibonacci.cur
// RUN: %cursivec %s -o %t && %t
// CHECK: 55

public procedure main(ctx: Context) -> i32 {
    let n = fib(10)
    // Note: method dispatch uses ~> operator, field access uses .
    ctx.fs~>write_stdout(f"{n}\n")
    result 0
}

procedure fib(n: i32) -> i32 {
    if n <= 1 {
        result n
    }
    result fib(n - 1) + fib(n - 2)
}
```

---

## Next Steps After Bootstrap

Once the bootstrap compiler can compile itself:

1. **Rewrite in Cursive**: Implement full compiler in Cursive using bootstrap
2. **Add deferred features**:
   - `shared` permission + key system
   - Parallel blocks, spawn, dispatch
   - Async/await
   - Comptime execution
   - Quote/emit code generation
   - Derive macros
3. **Optimize**: Self-hosting enables aggressive optimization passes
4. **Standard Library**: Build out full stdlib in Cursive
