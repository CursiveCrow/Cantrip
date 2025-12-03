# Cursive Language Compiler Implementation Plan

## Overview

This plan outlines the implementation of a Cursive language compiler using C and LLVM. The goal is a Bootstrap/MVP compiler that implements the full specification progressively, designed for cross-platform targeting.

**Build System:** CMake (excellent LLVM integration, cross-platform)
**Testing:** Custom lightweight test runner
**Implementation Order:** Logical pipeline progression (utilities → lexer → parser → types → sema → IR → codegen)

---

## Implementation Order (Detailed)

The implementation follows the natural compilation pipeline. Each step builds on the previous:

### Step 1: Project Setup & Utilities
1. Create directory structure
2. CMakeLists.txt with LLVM detection
3. Arena allocator (`util/arena.c`)
4. String interner (`util/string_interner.c`)
5. Dynamic array macros (`util/vec.h`)
6. Hash table (`util/hash_table.c`)
7. Custom test runner (`tests/test_runner.c`)

### Step 2: Lexer
1. Token definitions (`include/cursive/token.h`)
2. Source manager for locations (`src/lexer/source.c`)
3. Lexer implementation (`src/lexer/lexer.c`)
4. Lexer tests

### Step 3: AST & Parser
1. AST node definitions (`include/cursive/ast.h`)
2. Parser infrastructure (`src/parser/parser.c`)
3. Expression parsing with Pratt algorithm
4. Statement parsing
5. Declaration parsing (procedures, records, enums, modals, forms)
6. Parser tests

### Step 4: Type System Core
1. Type representation (`include/cursive/types.h`)
2. Permission system
3. Modal state tracking
4. Type equality and compatibility
5. Built-in types (primitives, Ptr, string)

### Step 5: Semantic Analysis
1. Symbol table and scopes (`src/sema/scope.c`)
2. Name resolution (`src/sema/resolve.c`)
3. Type checking (`src/sema/typecheck.c`)
4. Move/liveness analysis (`src/sema/liveness.c`)
5. Permission checking (`src/sema/permissions.c`)
6. Modal type checking (`src/sema/modal.c`)

### Step 6: Cursive IR
1. IR data structures (`include/cursive/ir.h`)
2. IR generation from AST (`src/ir/irgen.c`)
3. IR verification (`src/ir/verify.c`)

### Step 7: LLVM Code Generation
1. LLVM interface wrapper (`src/codegen/llvm_codegen.c`)
2. Type lowering to LLVM
3. Expression/statement codegen
4. Runtime library stubs (`runtime/`)

### Step 8: Driver & Integration
1. Compiler driver (`src/driver/main.c`)
2. Command-line argument parsing
3. End-to-end tests

---

## Project Structure

```
LLVM-Compiler/
├── CMakeLists.txt              # Build configuration
├── include/
│   ├── cursive/
│   │   ├── lexer.h             # Lexer interface
│   │   ├── token.h             # Token definitions
│   │   ├── parser.h            # Parser interface
│   │   ├── ast.h               # AST node definitions
│   │   ├── types.h             # Type system
│   │   ├── sema.h              # Semantic analysis
│   │   ├── ir.h                # Cursive IR (pre-LLVM)
│   │   ├── codegen.h           # LLVM codegen
│   │   ├── diagnostics.h       # Error/warning infrastructure
│   │   └── driver.h            # Compiler driver
│   └── util/
│       ├── arena.h             # Arena allocator
│       ├── string_interner.h   # String interning
│       └── hash_table.h        # Hash table
├── src/
│   ├── lexer/
│   ├── parser/
│   ├── ast/
│   ├── types/
│   ├── sema/
│   ├── ir/
│   ├── codegen/
│   └── driver/
├── tests/
│   ├── lexer/
│   ├── parser/
│   ├── sema/
│   ├── codegen/
│   └── e2e/                    # End-to-end tests
└── runtime/                    # Minimal runtime library
    ├── panic.c
    ├── alloc.c
    └── sync.c                  # Key system runtime
```

---

## Phase 1: Foundation (MVP Core)

### 1.1 Build System & Utilities

**CMake Configuration:**
- C17 standard
- LLVM 17+ integration
- Platform detection (Windows/Linux/macOS)
- Debug/Release configurations

**Core Utilities:**
- Arena allocator for AST/IR nodes
- String interner for identifiers
- Hash table for symbol tables
- Dynamic array implementation

### 1.2 Lexer

**Token Categories:**
```c
typedef enum {
    // Literals
    TOK_INT_LIT, TOK_FLOAT_LIT, TOK_STRING_LIT, TOK_CHAR_LIT,

    // Keywords (per §2.6)
    TOK_KW_LET, TOK_KW_VAR, TOK_KW_PROCEDURE, TOK_KW_RECORD,
    TOK_KW_ENUM, TOK_KW_MODAL, TOK_KW_FORM, TOK_KW_MATCH,
    TOK_KW_IF, TOK_KW_ELSE, TOK_KW_LOOP, TOK_KW_BREAK,
    TOK_KW_CONTINUE, TOK_KW_RETURN, TOK_KW_RESULT, TOK_KW_MOVE,
    TOK_KW_CONST, TOK_KW_UNIQUE, TOK_KW_SHARED, TOK_KW_PARALLEL,
    TOK_KW_SPAWN, TOK_KW_DISPATCH, TOK_KW_REGION, TOK_KW_COMPTIME,
    TOK_KW_QUOTE, TOK_KW_YIELD, TOK_KW_UNSAFE, TOK_KW_DEFER,
    TOK_KW_WITNESS, TOK_KW_TRANSITION, TOK_KW_WIDEN,
    TOK_KW_PUBLIC, TOK_KW_INTERNAL, TOK_KW_PRIVATE, TOK_KW_PROTECTED,
    TOK_KW_IMPORT, TOK_KW_USE, TOK_KW_AS, TOK_KW_WHERE,
    TOK_KW_IMPLEMENT, TOK_KW_FOR, TOK_KW_TYPE, TOK_KW_TRUE, TOK_KW_FALSE,

    // Operators
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_STARSTAR, TOK_AMP, TOK_PIPE, TOK_CARET, TOK_TILDE,
    TOK_BANG, TOK_AMPAMP, TOK_PIPEPIPE,
    TOK_EQ, TOK_EQEQ, TOK_BANGEQ, TOK_LT, TOK_GT, TOK_LTEQ, TOK_GTEQ,
    TOK_PLUSEQ, TOK_MINUSEQ, TOK_STAREQ, TOK_SLASHEQ,
    TOK_LTLT, TOK_GTGT, TOK_DOTDOT, TOK_DOTDOTEQ,
    TOK_QUESTION, TOK_HASH, TOK_AT, TOK_DOLLAR,
    TOK_ARROW, TOK_FATARROW, TOK_TILDEGT, TOK_COLONCOLON,
    TOK_COLONEQ,  // := immovable binding

    // Punctuation
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET, TOK_COMMA, TOK_DOT,
    TOK_COLON, TOK_SEMICOLON,

    // Special
    TOK_IDENT, TOK_NEWLINE, TOK_EOF, TOK_ERROR,
} TokenKind;
```

**Key Implementation Details:**
- UTF-8 source handling
- Newline-sensitive parsing (§2.11)
- Maximal munch with generic argument exception (§2.7)
- String interpolation support (f"..." syntax)
- Attribute syntax `[[...]]`

### 1.3 Parser

**AST Node Design:**
```c
// Base node with source location
typedef struct AstNode {
    AstKind kind;
    SourceSpan span;
} AstNode;

// Expression nodes
typedef struct ExprLiteral { AstNode base; LiteralValue value; } ExprLiteral;
typedef struct ExprIdent { AstNode base; InternedString name; } ExprIdent;
typedef struct ExprBinary { AstNode base; BinaryOp op; Expr *left, *right; } ExprBinary;
typedef struct ExprUnary { AstNode base; UnaryOp op; Expr *operand; } ExprUnary;
typedef struct ExprCall { AstNode base; Expr *callee; ExprList args; } ExprCall;
typedef struct ExprMethodCall { AstNode base; Expr *receiver; InternedString method; ExprList args; } ExprMethodCall;
typedef struct ExprFieldAccess { AstNode base; Expr *base_expr; InternedString field; } ExprFieldAccess;
typedef struct ExprIndex { AstNode base; Expr *base_expr; Expr *index; } ExprIndex;
typedef struct ExprIf { AstNode base; Expr *cond; Block then_branch; Block *else_branch; } ExprIf;
typedef struct ExprMatch { AstNode base; Expr *scrutinee; MatchArmList arms; } ExprMatch;
typedef struct ExprBlock { AstNode base; StmtList stmts; Expr *result; } ExprBlock;
typedef struct ExprMove { AstNode base; Expr *operand; } ExprMove;
typedef struct ExprKeyBlock { AstNode base; PathList paths; KeyMode mode; Block body; } ExprKeyBlock;
// ... more expression types

// Type AST nodes
typedef struct TypePath { AstNode base; PathSegmentList segments; } TypePath;
typedef struct TypeTuple { AstNode base; TypeList elements; } TypeTuple;
typedef struct TypeArray { AstNode base; Type *element; Expr *size; } TypeArray;
typedef struct TypeSlice { AstNode base; Type *element; } TypeSlice;
typedef struct TypePtr { AstNode base; Type *pointee; Permission perm; } TypePtr;
typedef struct TypeModal { AstNode base; Type *base_type; InternedString state; } TypeModal;
typedef struct TypeUnion { AstNode base; TypeList alternatives; } TypeUnion;
// ... more type nodes

// Declaration nodes
typedef struct DeclProcedure {
    AstNode base;
    Visibility vis;
    InternedString name;
    GenericParamList generics;
    ParamList params;
    Type *return_type;
    ContractClause *precond;
    ContractClause *postcond;
    Block *body;
} DeclProcedure;

typedef struct DeclRecord {
    AstNode base;
    Visibility vis;
    InternedString name;
    GenericParamList generics;
    FormBoundList implements;
    FieldList fields;
    ProcedureList methods;
} DeclRecord;

typedef struct DeclModal {
    AstNode base;
    Visibility vis;
    InternedString name;
    GenericParamList generics;
    ModalStateList states;
} DeclModal;

// ... enums, forms, implement blocks, etc.
```

**Parsing Strategy:**
- Recursive descent with Pratt parsing for expressions
- Newline-aware statement termination
- Error recovery with synchronization tokens

### 1.4 Diagnostics Infrastructure

```c
typedef struct Diagnostic {
    DiagnosticKind kind;      // Error, Warning, Note
    const char *code;         // "E-TYP-1601", etc.
    SourceSpan span;
    char *message;
    DiagnosticList notes;     // Related locations
} Diagnostic;

typedef struct DiagnosticEngine {
    DiagnosticList diagnostics;
    SourceManager *sm;
    bool has_errors;
} DiagnosticEngine;

void diag_error(DiagnosticEngine *de, const char *code, SourceSpan span, const char *fmt, ...);
void diag_warning(DiagnosticEngine *de, const char *code, SourceSpan span, const char *fmt, ...);
void diag_note(DiagnosticEngine *de, SourceSpan span, const char *fmt, ...);
```

---

## Phase 2: Type System

### 2.1 Type Representation

```c
typedef enum TypeKind {
    TYPE_VOID,
    TYPE_NEVER,              // ! type
    TYPE_BOOL,
    TYPE_INT,                // i8, i16, i32, i64, i128, isize
    TYPE_UINT,               // u8, u16, u32, u64, u128, usize
    TYPE_FLOAT,              // f16, f32, f64
    TYPE_CHAR,
    TYPE_STRING,             // Modal: @Managed | @View
    TYPE_TUPLE,
    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_PTR,                // Modal: @Valid | @Null | @Expired
    TYPE_RECORD,
    TYPE_ENUM,
    TYPE_MODAL,
    TYPE_UNION,
    TYPE_PROCEDURE,
    TYPE_GENERIC_PARAM,
    TYPE_FORM_BOUND,
    TYPE_ERROR,              // Sentinel for error recovery
} TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct { IntWidth width; bool is_signed; } int_type;
        struct { FloatWidth width; } float_type;
        struct { TypeList elements; } tuple_type;
        struct { Type *element; uint64_t size; } array_type;
        struct { Type *element; } slice_type;
        struct { Type *pointee; Permission perm; ModalState ptr_state; } ptr_type;
        struct { RecordDecl *decl; TypeList type_args; } record_type;
        struct { EnumDecl *decl; TypeList type_args; } enum_type;
        struct { ModalDecl *decl; TypeList type_args; ModalState state; } modal_type;
        struct { TypeList alternatives; } union_type;
        struct { ParamTypeList params; Type *return_type; } proc_type;
        // ...
    };
} Type;
```

### 2.2 Permission System (§4.5)

```c
typedef enum Permission {
    PERM_CONST,    // ~ : Read-only, unlimited aliases
    PERM_UNIQUE,   // ~! : Read-write, exclusive
    PERM_SHARED,   // ~% : Synchronized aliased mutability
} Permission;

// Permission lattice: unique > shared > const
bool permission_allows_write(Permission p);
bool permission_allows_alias(Permission p);
bool permission_compatible(Permission held, Permission required);
```

### 2.3 Modal State Tracking

```c
typedef struct ModalState {
    ModalDecl *modal;
    InternedString state_name;  // e.g., "Valid", "Null", "Managed"
    bool is_general;            // True if general modal type (any state)
} ModalState;

typedef struct BindingState {
    InternedString name;
    Type *type;
    Permission permission;
    ModalState modal_state;     // For modal types
    BindingStatus status;       // Live, Moved, Dropped
    bool is_movable;            // let vs let :=
} BindingState;
```

### 2.4 Type Inference & Unification

```c
typedef struct TypeVar {
    uint32_t id;
    Type *resolved;             // NULL if unresolved
    TypeConstraintList constraints;
} TypeVar;

typedef struct TypeContext {
    Arena *arena;
    TypeVarList type_vars;
    SubstitutionMap substitutions;
} TypeContext;

// Unification
bool unify(TypeContext *ctx, Type *a, Type *b);
Type *substitute(TypeContext *ctx, Type *t);
Type *instantiate_generic(TypeContext *ctx, Type *generic, TypeList args);
```

---

## Phase 3: Semantic Analysis

### 3.1 Name Resolution

```c
typedef struct Scope {
    ScopeKind kind;           // Universe, Module, Procedure, Block
    Scope *parent;
    SymbolTable symbols;      // Unified namespace
} Scope;

typedef struct Symbol {
    SymbolKind kind;          // Variable, Procedure, Type, Module, Form
    InternedString name;
    Visibility visibility;
    Decl *decl;
    Type *type;
} Symbol;

// Resolution
Symbol *resolve_name(Scope *scope, InternedString name);
Symbol *resolve_path(Scope *scope, PathSegmentList path);
```

### 3.2 Move & Liveness Analysis

```c
typedef struct LivenessState {
    BindingStateMap bindings;
    bool in_loop;
    bool after_yield;
} LivenessState;

typedef enum BindingStatus {
    BINDING_LIVE,
    BINDING_MOVED,
    BINDING_PARTIALLY_MOVED,
    BINDING_DROPPED,
} BindingStatus;

// Analysis functions
void check_move(SemaContext *ctx, Expr *expr, LivenessState *state);
void check_use_after_move(SemaContext *ctx, Expr *expr, LivenessState *state);
void check_drop_order(SemaContext *ctx, Block *block);
```

### 3.3 Permission Checking

```c
// Verify permission requirements
void check_permission(SemaContext *ctx, Expr *expr, Permission required);
void check_mutation(SemaContext *ctx, Expr *target);
void check_aliasing(SemaContext *ctx, Expr *expr, LivenessState *state);

// Path permission propagation (§4.5)
Permission path_permission(SemaContext *ctx, Expr *path);
```

### 3.4 Modal Type Checking

```c
// State-specific type checking
void check_modal_state_access(SemaContext *ctx, Expr *expr, InternedString method);
void check_transition(SemaContext *ctx, TransitionExpr *trans);
Type *narrow_modal_type(Type *modal, InternedString state);
void check_exhaustive_match(SemaContext *ctx, MatchExpr *match);
```

### 3.5 Form Implementation Checking

```c
// Form satisfaction
bool type_implements_form(SemaContext *ctx, Type *type, FormDecl *form);
void check_form_implementation(SemaContext *ctx, ImplementDecl *impl);
void check_behavioral_subtyping(SemaContext *ctx, ProcDecl *impl, ProcDecl *form_method);
```

---

## Phase 4: Key System Analysis (§13)

### 4.1 Key Path Computation

```c
typedef struct KeyPath {
    PathSegmentList segments;
    bool has_coarsening;      // # operator present
    uint32_t coarsen_depth;   // Depth at which # appears
} KeyPath;

typedef enum KeyMode {
    KEY_READ,
    KEY_WRITE,
} KeyMode;

typedef struct KeyRequirement {
    KeyPath path;
    KeyMode mode;
    SourceSpan origin;
} KeyRequirement;

KeyPath compute_key_path(SemaContext *ctx, Expr *expr);
KeyMode determine_key_mode(SemaContext *ctx, Expr *expr, ExprContext context);
```

### 4.2 Static Key Analysis

```c
typedef struct KeyState {
    KeyRequirementList held_keys;
    bool in_dynamic_context;  // [[dynamic]] attribute active
} KeyState;

// Disjointness analysis
bool paths_disjoint(KeyPath *a, KeyPath *b);
bool paths_overlap(KeyPath *a, KeyPath *b);
bool key_covers(KeyRequirement *held, KeyRequirement *needed);

// Static safety checks
bool can_prove_static_safety(SemaContext *ctx, KeyRequirementList reqs);
void check_read_then_write(SemaContext *ctx, Stmt *stmt, KeyState *state);
void check_key_escape(SemaContext *ctx, Scope *scope, KeyState *state);
```

### 4.3 Key System Errors

```c
// Diagnose key violations
void emit_key_conflict(SemaContext *ctx, KeyRequirement *a, KeyRequirement *b);
void emit_dynamic_index_conflict(SemaContext *ctx, Expr *a, Expr *b);
void emit_read_then_write_error(SemaContext *ctx, Expr *read, Expr *write);
```

---

## Phase 5: Intermediate Representation

### 5.1 Cursive IR Design

A typed IR between AST and LLVM IR to handle Cursive-specific semantics:

```c
typedef enum IrOpcode {
    // Memory
    IR_ALLOCA,
    IR_LOAD,
    IR_STORE,
    IR_GEP,              // Get element pointer
    IR_REGION_ALLOC,     // ^region expr
    IR_REGION_FREE,

    // Control flow
    IR_BRANCH,
    IR_COND_BRANCH,
    IR_SWITCH,
    IR_RETURN,
    IR_UNREACHABLE,

    // Calls
    IR_CALL,
    IR_CALL_INDIRECT,

    // Modal operations
    IR_STATE_CHECK,      // Modal state test
    IR_TRANSITION,       // Modal state transition
    IR_DISCRIMINANT,     // Get enum/modal discriminant

    // Key operations
    IR_KEY_ACQUIRE,
    IR_KEY_RELEASE,
    IR_KEY_UPGRADE,

    // Concurrency
    IR_SPAWN,
    IR_DISPATCH,
    IR_PARALLEL_BEGIN,
    IR_PARALLEL_END,
    IR_YIELD,
    IR_RESUME,

    // Arithmetic, comparisons, etc.
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_REM,
    IR_AND, IR_OR, IR_XOR, IR_SHL, IR_SHR,
    IR_EQ, IR_NE, IR_LT, IR_LE, IR_GT, IR_GE,
    IR_NEG, IR_NOT,

    // Type operations
    IR_CAST,
    IR_BITCAST,
    IR_WIDEN,            // Modal state widening

    // Drop
    IR_DROP,
    IR_DROP_IN_PLACE,
} IrOpcode;

typedef struct IrInstr {
    IrOpcode opcode;
    Type *type;
    IrValue *dest;
    IrValueList operands;
    SourceSpan span;
} IrInstr;

typedef struct IrBlock {
    InternedString label;
    IrInstrList instrs;
    IrTerminator terminator;
} IrBlock;

typedef struct IrFunction {
    InternedString name;
    IrParamList params;
    Type *return_type;
    IrBlockList blocks;
    IrBlock *entry;
} IrFunction;
```

### 5.2 IR Generation from AST

```c
typedef struct IrGen {
    Arena *arena;
    IrModule *module;
    IrFunction *current_fn;
    IrBlock *current_block;
    ValueMap locals;          // Local variable -> IR value
    ScopeStack scopes;
    DropQueue pending_drops;
} IrGen;

IrValue *irgen_expr(IrGen *gen, Expr *expr);
void irgen_stmt(IrGen *gen, Stmt *stmt);
void irgen_decl(IrGen *gen, Decl *decl);
```

---

## Phase 6: LLVM Code Generation

### 6.1 LLVM Interface

```c
typedef struct LLVMCodegen {
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    LLVMTargetMachineRef target;

    TypeLayoutMap type_layouts;
    ValueMap globals;
    ValueMap functions;

    // Runtime function references
    LLVMValueRef rt_panic;
    LLVMValueRef rt_alloc;
    LLVMValueRef rt_key_acquire;
    LLVMValueRef rt_key_release;
    // ...
} LLVMCodegen;
```

### 6.2 Type Lowering

```c
LLVMTypeRef lower_type(LLVMCodegen *cg, Type *type);

// Modal types -> tagged union
// Record -> struct
// Enum -> tagged union with discriminant
// Union types -> tagged union
// Slice -> { ptr, len }
// String -> modal { ptr, len, cap } or { ptr, len }
```

### 6.3 Key System Runtime

For `[[dynamic]]` contexts, generate runtime synchronization:

```c
// Runtime functions
void cursive_key_acquire(void *root, KeyPath *path, KeyMode mode);
void cursive_key_release(void *root, KeyPath *path);
bool cursive_key_try_acquire(void *root, KeyPath *path, KeyMode mode);

// Speculative blocks use CAS-based approach
bool cursive_spec_begin(void *root, SpecContext *ctx);
bool cursive_spec_commit(SpecContext *ctx);
void cursive_spec_abort(SpecContext *ctx);
```

### 6.4 Async State Machine Generation

Transform async procedures into state machines:

```c
// Async<Out, In, Result, E> becomes:
typedef struct AsyncStateMachine {
    uint32_t state;           // Current resumption point
    union {
        Out suspended_value;
        Result completed_value;
        E failed_value;
    } payload;
    // Captured locals as fields
} AsyncStateMachine;
```

---

## Phase 7: Concurrency & Parallel

### 7.1 Parallel Block Lowering

```c
// parallel ctx.cpu() { spawn { e1 } spawn { e2 } }
// ->
// 1. Create task descriptors
// 2. Submit to thread pool
// 3. Join barrier at block exit
```

### 7.2 Dispatch Lowering

```c
// dispatch i in 0..n { body }
// ->
// 1. Partition range
// 2. Create work items
// 3. Schedule with optional reduction
```

### 7.3 GPU Support (Future)

- SPIR-V generation for compute shaders
- Dispatch to Vulkan/Metal/CUDA backends

---

## Phase 8: Metaprogramming

### 8.1 Comptime Evaluation

```c
typedef struct ComptimeInterp {
    Arena *arena;
    ValueStack stack;
    ComptimeScope *scope;
    EmitQueue emit_queue;
    uint64_t step_limit;      // Resource limit
} ComptimeInterp;

ComptimeValue eval_comptime(ComptimeInterp *interp, Expr *expr);
void exec_comptime_block(ComptimeInterp *interp, Block *block);
```

### 8.2 Quote/Emit

```c
// Quote captures AST without evaluation
QuotedBlock *quote_block(Parser *p, TokenStream *ts);
QuotedExpr *quote_expr(Parser *p, TokenStream *ts);

// Emit injects into module scope
void emit_quoted(ComptimeInterp *interp, QuotedBlock *block);

// Splice interpolates values
AstNode *splice(ComptimeInterp *interp, Expr *splice_expr, SpliceContext ctx);
```

### 8.3 Type Introspection

```c
TypeInfo *reflect_type(Type *type);
ProcInfo *reflect_proc(ProcDecl *proc);
FormInfo *reflect_form(FormDecl *form);
ModalInfo *reflect_modal(ModalDecl *modal);
```

---

## Implementation Milestones

### MVP Milestone 1: Basic Compilation
- Lexer, parser, basic AST
- Primitive types, records, enums
- Procedures, control flow
- Basic type checking
- LLVM codegen for simple programs

### MVP Milestone 2: Memory Model
- Permission system (const/unique/shared)
- Move semantics
- Modal types (Ptr, string)
- Drop/RAII

### MVP Milestone 3: Forms & Generics
- Form declarations
- Form implementations
- Generic types and procedures
- Monomorphization

### MVP Milestone 4: Key System
- Static key analysis
- [[dynamic]] runtime support
- # coarsening operator
- Compound assignment desugaring

### MVP Milestone 5: Concurrency
- parallel blocks
- spawn/dispatch
- Key system integration
- Async state machines

### MVP Milestone 6: Metaprogramming
- comptime blocks
- quote/emit
- Type introspection
- Derive attributes

### MVP Milestone 7: Full Specification
- Contracts (preconditions/postconditions)
- Refinement types
- Region-based allocation
- GPU support
- Complete standard library

---

## Key Design Decisions

### 1. Arena-Based Memory Management
All AST and IR nodes allocated from arenas for fast allocation and bulk deallocation.

### 2. Interned Strings
All identifiers interned for O(1) equality comparison.

### 3. Two-Phase IR
Cursive IR captures high-level semantics (modals, keys, async) before lowering to LLVM IR.

### 4. Incremental Compilation Support
Design symbol tables and caching for eventual incremental compilation.

### 5. Error Recovery
Parser and semantic analysis continue after errors to report multiple diagnostics.

### 6. Cross-Platform from Start
Abstract platform-specific code behind interfaces; use LLVM's target abstraction.

---

## Testing Strategy

### Custom Test Runner Design

```c
// tests/test_runner.h
#define TEST(name) static void test_##name(TestContext *ctx)
#define ASSERT_EQ(a, b) test_assert_eq(ctx, (a), (b), #a, #b, __FILE__, __LINE__)
#define ASSERT_TRUE(x) test_assert_true(ctx, (x), #x, __FILE__, __LINE__)
#define ASSERT_STR_EQ(a, b) test_assert_str_eq(ctx, (a), (b), __FILE__, __LINE__)

typedef struct TestContext {
    const char *test_name;
    int assertions_passed;
    int assertions_failed;
    char *failure_message;
} TestContext;

// Test registration
#define REGISTER_TEST(name) register_test(#name, test_##name)
```

### Test Categories

**Unit Tests:**
- Lexer: Token stream verification for each token type
- Parser: AST structure verification, error recovery
- Type checker: Type inference, permission errors, modal states
- Key analysis: Static safety proofs, conflict detection

**Integration Tests:**
- Full compilation of test programs
- Runtime behavior verification
- Error message quality checks

**Specification Tests:**
- Test case for each diagnostic code in the spec (E-TYP-*, E-MEM-*, E-KEY-*, etc.)
- Test case for each well-formedness rule

**Regression Tests:**
- Directory of .cursive files with expected outputs
- Automatic comparison of compiler output

---

## Dependencies

- **LLVM 17+**: Backend code generation
- **CMake 3.20+**: Build system
- **C17 compiler**: GCC 11+ or Clang 14+

Optional:
- **libffi**: For comptime FFI (if needed)
- **pthreads/Win32 threads**: Runtime threading

---

## Files to Create First

1. `CMakeLists.txt` - Build configuration
2. `include/cursive/token.h` - Token definitions
3. `include/util/arena.h` - Arena allocator
4. `include/util/string_interner.h` - String interning
5. `src/lexer/lexer.c` - Lexer implementation
6. `src/driver/main.c` - Compiler entry point