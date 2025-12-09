/*
 * Cursive Bootstrap Compiler - AST Definitions
 *
 * Abstract Syntax Tree node types following the Cursive Language Specification.
 * Key Cursive features reflected in the AST:
 * - Class implementations are INLINE with type definitions via `<:`
 * - Method dispatch uses `~>` operator (NOT `.method()`)
 * - Binding operators: `=` (movable), `:=` (immovable)
 * - Receiver shorthands: `~` (const), `~!` (unique), `~%` (shared)
 */

#ifndef CURSIVE_AST_H
#define CURSIVE_AST_H

#include "common/common.h"
#include "common/string_pool.h"
#include "common/vec.h"
#include "lexer/token.h"

/* Forward declarations */
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Decl Decl;
typedef struct TypeExpr TypeExpr;
typedef struct Pattern Pattern;
typedef struct Module Module;
typedef struct GenericParam GenericParam;
typedef struct WhereClause WhereClause;
typedef struct Scope Scope;  /* For storing resolved scope on procedures */

/*
 * ============================================
 * Visibility
 * ============================================
 */

typedef enum Visibility {
    VIS_PRIVATE,    /* Default - visible in declaring module only */
    VIS_PROTECTED,  /* Visible in module and submodules */
    VIS_INTERNAL,   /* Visible within crate */
    VIS_PUBLIC      /* Visible everywhere */
} Visibility;

/*
 * ============================================
 * Permissions (simplified for bootstrap)
 * ============================================
 */

typedef enum Permission {
    PERM_CONST,     /* Default - read-only, unlimited aliases */
    PERM_UNIQUE,    /* Exclusive read-write, no aliases */
    PERM_SHARED     /* Synchronized aliased mutability (deferred) */
} Permission;

/*
 * ============================================
 * Receiver Kind (for method declarations)
 * ============================================
 */

typedef enum ReceiverKind {
    RECV_NONE,      /* Not a method (standalone procedure) */
    RECV_CONST,     /* ~ (const Self) */
    RECV_UNIQUE,    /* ~! (unique Self) */
    RECV_SHARED     /* ~% (shared Self) */
} ReceiverKind;

/*
 * ============================================
 * Binding Operator
 * ============================================
 */

typedef enum BindingOp {
    BIND_MOVABLE,   /* = (responsibility can transfer via move) */
    BIND_IMMOVABLE  /* := (responsibility permanently fixed) */
} BindingOp;

/*
 * ============================================
 * Type Expressions
 * ============================================
 */

typedef enum TypeExprKind {
    /* Primitive types */
    TEXPR_PRIMITIVE,

    /* Named type (possibly generic) */
    TEXPR_NAMED,

    /* Modal type with state: Type@State */
    TEXPR_MODAL_STATE,

    /* Generic instantiation: Type<Args> */
    TEXPR_GENERIC,

    /* Tuple: (T1, T2, ...) */
    TEXPR_TUPLE,

    /* Array: [T; N] */
    TEXPR_ARRAY,

    /* Slice: [T] */
    TEXPR_SLICE,

    /* Function: procedure(Args) -> Ret */
    TEXPR_FUNCTION,

    /* Union: T | U | V */
    TEXPR_UNION,

    /* Pointer: Ptr<T> (modal - has states) */
    TEXPR_PTR,

    /* Reference: &T or &!T (syntactic sugar for Ptr) */
    TEXPR_REF,

    /* Never type: ! */
    TEXPR_NEVER,

    /* Unit type: () */
    TEXPR_UNIT,

    /* Self type */
    TEXPR_SELF,

    /* Inferred type (for let without annotation) */
    TEXPR_INFER
} TypeExprKind;

typedef enum PrimitiveType {
    PRIM_I8, PRIM_I16, PRIM_I32, PRIM_I64, PRIM_I128, PRIM_ISIZE,
    PRIM_U8, PRIM_U16, PRIM_U32, PRIM_U64, PRIM_U128, PRIM_USIZE,
    PRIM_F16, PRIM_F32, PRIM_F64,
    PRIM_BOOL, PRIM_CHAR, PRIM_STRING
} PrimitiveType;

struct TypeExpr {
    TypeExprKind kind;
    SourceSpan span;
    Permission perm;  /* Permission modifier if any */

    union {
        PrimitiveType primitive;

        struct {
            InternedString name;
            Vec(InternedString) path;  /* Module path segments */
        } named;

        struct {
            TypeExpr *base;           /* The modal type */
            InternedString state;     /* The state name */
        } modal_state;

        struct {
            TypeExpr *base;
            Vec(TypeExpr *) args;
        } generic;

        struct {
            Vec(TypeExpr *) elements;
        } tuple;

        struct {
            TypeExpr *element;
            Expr *size;  /* Compile-time constant */
        } array;

        struct {
            TypeExpr *element;
        } slice;

        struct {
            Vec(TypeExpr *) params;
            TypeExpr *return_type;
        } function;

        struct {
            Vec(TypeExpr *) members;
        } union_;

        struct {
            TypeExpr *pointee;
        } ptr;

        struct {
            TypeExpr *referent;
            bool is_unique;  /* & vs &! */
        } ref;
    };
};

/*
 * ============================================
 * Patterns
 * ============================================
 */

typedef enum PatternKind {
    PAT_WILDCARD,       /* _ */
    PAT_BINDING,        /* ident or mut ident */
    PAT_LITERAL,        /* 42, "hello", true */
    PAT_TUPLE,          /* (p1, p2, ...) */
    PAT_RECORD,         /* RecordName { field: pat, ... } */
    PAT_ENUM,           /* EnumName::Variant(pat) */
    PAT_MODAL,          /* @State { field: pat, ... } */
    PAT_RANGE,          /* start..end or start..=end */
    PAT_OR,             /* pat1 | pat2 */
    PAT_GUARD           /* pat if condition */
} PatternKind;

struct Pattern {
    PatternKind kind;
    SourceSpan span;

    union {
        /* PAT_WILDCARD: no data */

        struct {
            InternedString name;
            bool is_mutable;  /* var vs let in pattern */
            TypeExpr *type;   /* Optional type annotation */
            struct Symbol *resolved;  /* Filled by name resolution */
        } binding;

        struct {
            Expr *value;  /* Literal expression */
        } literal;

        struct {
            Vec(Pattern *) elements;
        } tuple;

        struct {
            TypeExpr *type;  /* The record type */
            Vec(InternedString) field_names;
            Vec(Pattern *) field_patterns;
            bool has_rest;  /* .. to ignore remaining */
        } record;

        struct {
            TypeExpr *type;  /* The enum type */
            InternedString variant;
            Pattern *payload;  /* NULL for unit variants */
        } enum_;

        struct {
            InternedString state;
            Vec(InternedString) field_names;
            Vec(Pattern *) field_patterns;
        } modal;

        struct {
            Pattern *start;
            Pattern *end;
            bool inclusive;  /* ..= vs .. */
        } range;

        struct {
            Vec(Pattern *) alternatives;
        } or_;

        struct {
            Pattern *pattern;
            Expr *guard;  /* Condition expression */
        } guard;
    };
};

/*
 * ============================================
 * Expressions
 * ============================================
 */

typedef enum ExprKind {
    /* Literals */
    EXPR_INT_LIT,
    EXPR_FLOAT_LIT,
    EXPR_STRING_LIT,
    EXPR_CHAR_LIT,
    EXPR_BOOL_LIT,

    /* Identifiers and paths */
    EXPR_IDENT,
    EXPR_PATH,          /* a::b::c */

    /* Operators */
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL,          /* f(args) - function/procedure call */
    EXPR_METHOD_CALL,   /* receiver~>method(args) - uses ~> operator */
    EXPR_FIELD,         /* expr.field - field access */
    EXPR_INDEX,         /* expr[index] */

    /* Constructors */
    EXPR_TUPLE,         /* (e1, e2, ...) */
    EXPR_ARRAY,         /* [e1, e2, ...] or [e; n] */
    EXPR_RECORD,        /* RecordName { field: value, ... } */

    /* Control flow */
    EXPR_IF,
    EXPR_MATCH,
    EXPR_BLOCK,
    EXPR_LOOP,

    /* Move/widen */
    EXPR_MOVE,          /* move expr */
    EXPR_WIDEN,         /* widen expr (modal type widening) */

    /* Type operations */
    EXPR_CAST,          /* expr as Type */

    /* Range */
    EXPR_RANGE,         /* start..end or start..=end */

    /* Static method call */
    EXPR_STATIC_CALL,   /* Type::method(args) */

    /* Region allocation */
    EXPR_REGION_ALLOC,  /* ^region expr */

    /* Address-of */
    EXPR_ADDR_OF,       /* &expr or &!expr */

    /* Dereference */
    EXPR_DEREF,         /* *expr */

    /* Lambda/closure (if supported in bootstrap) */
    EXPR_CLOSURE
} ExprKind;

typedef enum BinaryOp {
    /* Arithmetic */
    BINOP_ADD, BINOP_SUB, BINOP_MUL, BINOP_DIV, BINOP_MOD, BINOP_POW,

    /* Comparison */
    BINOP_EQ, BINOP_NE, BINOP_LT, BINOP_LE, BINOP_GT, BINOP_GE,

    /* Logical */
    BINOP_AND, BINOP_OR,

    /* Bitwise */
    BINOP_BIT_AND, BINOP_BIT_OR, BINOP_BIT_XOR, BINOP_SHL, BINOP_SHR,

    /* Assignment (compound) */
    BINOP_ASSIGN,
    BINOP_ADD_ASSIGN, BINOP_SUB_ASSIGN, BINOP_MUL_ASSIGN,
    BINOP_DIV_ASSIGN, BINOP_MOD_ASSIGN,
    BINOP_BIT_AND_ASSIGN, BINOP_BIT_OR_ASSIGN, BINOP_BIT_XOR_ASSIGN,
    BINOP_SHL_ASSIGN, BINOP_SHR_ASSIGN
} BinaryOp;

typedef enum UnaryOp {
    UNOP_NEG,       /* - */
    UNOP_NOT,       /* ! */
    UNOP_BIT_NOT,   /* ~ */
    UNOP_DEREF,     /* * */
    UNOP_ADDR,      /* & */
    UNOP_ADDR_MUT,  /* &! */
    UNOP_TRY        /* ? (error propagation) */
} UnaryOp;

struct Expr {
    ExprKind kind;
    SourceSpan span;
    TypeExpr *resolved_type;  /* Filled by type checker */

    union {
        struct {
            uint64_t value;
            IntSuffix suffix;
        } int_lit;

        struct {
            double value;
        } float_lit;

        struct {
            InternedString value;
        } string_lit;

        struct {
            uint32_t value;
        } char_lit;

        struct {
            bool value;
        } bool_lit;

        struct {
            InternedString name;
            struct Symbol *resolved;  /* Filled by name resolution */
        } ident;

        struct {
            Vec(InternedString) segments;
        } path;

        struct {
            BinaryOp op;
            Expr *left;
            Expr *right;
        } binary;

        struct {
            UnaryOp op;
            Expr *operand;
        } unary;

        struct {
            Expr *callee;
            Vec(Expr *) args;
        } call;

        struct {
            Expr *receiver;
            InternedString method;
            Vec(Expr *) args;
            Vec(TypeExpr *) type_args;  /* Turbofish: obj~>method::<T>(args) */
        } method_call;

        struct {
            Expr *object;
            InternedString field;
        } field;

        struct {
            Expr *object;
            Expr *index;
        } index;

        struct {
            Vec(Expr *) elements;
        } tuple;

        struct {
            Vec(Expr *) elements;
            Expr *repeat_value;   /* For [expr; count] syntax */
            Expr *repeat_count;
        } array;

        struct {
            TypeExpr *type;
            Vec(InternedString) field_names;
            Vec(Expr *) field_values;
        } record;

        struct {
            Expr *condition;
            Expr *then_branch;
            Expr *else_branch;  /* NULL if no else */
        } if_;

        struct {
            Expr *scrutinee;
            Vec(Pattern *) arms_patterns;
            Vec(Expr *) arms_bodies;
        } match;

        struct {
            Vec(Stmt *) stmts;
            Expr *result;  /* Final expression (may be NULL) */
        } block;

        struct {
            InternedString label;  /* Optional loop label */
            Pattern *binding;      /* loop x in range pattern */
            Expr *iterable;        /* Range or collection */
            Expr *condition;       /* loop condition { } - condition-only */
            Expr *body;
        } loop;

        struct {
            Expr *operand;
        } move;

        struct {
            Expr *operand;
        } widen;

        struct {
            Expr *operand;
            TypeExpr *target_type;
        } cast;

        struct {
            Expr *start;
            Expr *end;
            bool inclusive;
        } range;

        struct {
            TypeExpr *type;
            InternedString method;
            Vec(Expr *) args;
            Vec(TypeExpr *) type_args;
        } static_call;

        struct {
            InternedString region;
            Expr *value;
        } region_alloc;

        struct {
            Expr *operand;
            bool is_unique;  /* &! vs & */
        } addr_of;

        struct {
            Expr *operand;
        } deref;

        struct {
            Vec(Pattern *) params;
            TypeExpr *return_type;  /* May be NULL */
            Expr *body;
        } closure;
    };
};

/*
 * ============================================
 * Statements
 * ============================================
 */

typedef enum StmtKind {
    STMT_EXPR,      /* Expression statement */
    STMT_LET,       /* let binding */
    STMT_VAR,       /* var binding (mutable) */
    STMT_ASSIGN,    /* Assignment (not compound) */
    STMT_RETURN,    /* return expr */
    STMT_RESULT,    /* result expr (tail return) */
    STMT_BREAK,     /* break ['label] [expr] */
    STMT_CONTINUE,  /* continue ['label] */
    STMT_DEFER,     /* defer { ... } */
    STMT_UNSAFE     /* unsafe { ... } */
} StmtKind;

struct Stmt {
    StmtKind kind;
    SourceSpan span;

    union {
        struct {
            Expr *expr;
        } expr;

        struct {
            Pattern *pattern;
            TypeExpr *type;    /* May be NULL (inferred) */
            Expr *init;        /* May be NULL (uninitialized) */
            BindingOp op;      /* = or := */
        } let;

        struct {
            Pattern *pattern;
            TypeExpr *type;
            Expr *init;
            BindingOp op;
        } var;

        struct {
            Expr *target;
            Expr *value;
        } assign;

        struct {
            Expr *value;  /* May be NULL for bare return */
        } return_;

        struct {
            Expr *value;
        } result;

        struct {
            InternedString label;  /* Optional loop label */
            Expr *value;           /* May be NULL */
        } break_;

        struct {
            InternedString label;
        } continue_;

        struct {
            Expr *body;  /* Block expression */
        } defer;

        struct {
            Expr *body;  /* Block expression */
        } unsafe;
    };
};

/*
 * ============================================
 * Declarations
 * ============================================
 */

typedef enum DeclKind {
    DECL_PROC,          /* Standalone procedure */
    DECL_RECORD,        /* Record type (may implement classes) */
    DECL_ENUM,          /* Enum type (may implement classes) */
    DECL_MODAL,         /* Modal type (state machine) */
    DECL_TYPE_ALIAS,    /* type Foo = Bar */
    DECL_CLASS,         /* Class (interface) declaration */
    DECL_EXTERN,        /* extern "C" block */
    DECL_MODULE,        /* module declaration */
    DECL_IMPORT,        /* import statement */
    DECL_USE            /* use statement */
} DeclKind;

/* Generic parameter */
struct GenericParam {
    InternedString name;
    Vec(TypeExpr *) bounds;  /* Class bounds */
    TypeExpr *default_type;  /* Optional default */
    SourceSpan span;
};

/* Where clause constraint */
struct WhereClause {
    TypeExpr *type;
    Vec(TypeExpr *) bounds;
    SourceSpan span;
};

/* Contract (precondition/postcondition) */
typedef struct Contract {
    Expr *condition;
    bool is_precondition;  /* |= vs => */
    SourceSpan span;
} Contract;

/* Parameter declaration */
typedef struct ParamDecl {
    InternedString name;
    TypeExpr *type;
    Permission perm;
    bool is_move;     /* move parameter */
    struct Symbol *resolved;  /* Filled by name resolution */
    SourceSpan span;
} ParamDecl;

/* Procedure declaration (also used for methods) */
typedef struct ProcDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    ReceiverKind receiver;        /* RECV_NONE for standalone procedures */
    Vec(ParamDecl) params;
    TypeExpr *return_type;        /* NULL for procedures returning unit */
    Vec(Contract) contracts;
    Vec(WhereClause) where_clauses;
    Expr *body;                   /* NULL for extern declarations */
    Scope *scope;                 /* Scope with parameters/locals (filled by resolver) */
    SourceSpan span;
} ProcDecl;

/* Field declaration */
typedef struct FieldDecl {
    Visibility vis;
    InternedString name;
    TypeExpr *type;
    Expr *default_value;  /* Optional default */
    SourceSpan span;
} FieldDecl;

/* Enum variant */
typedef struct EnumVariant {
    InternedString name;
    TypeExpr *payload;  /* NULL for unit variants */
    Expr *discriminant; /* Optional explicit discriminant */
    SourceSpan span;
} EnumVariant;

/* Modal state definition */
typedef struct ModalState {
    InternedString name;          /* State name (e.g., "Valid", "Null") */
    Vec(FieldDecl) fields;        /* Payload fields */
    Vec(ProcDecl) methods;        /* State-specific methods */
    Vec(struct Transition) transitions;  /* Forward declaration needed */
    SourceSpan span;
} ModalState;

/* Transition declaration */
typedef struct Transition {
    InternedString name;
    ReceiverKind receiver;
    Vec(ParamDecl) params;
    InternedString target_state;  /* @TargetState */
    Expr *body;
    SourceSpan span;
} Transition;

/* Record declaration */
typedef struct RecordDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    Vec(TypeExpr *) implements;   /* Classes implemented via <: */
    Vec(FieldDecl) fields;
    Vec(ProcDecl) methods;        /* Methods defined inline */
    Vec(WhereClause) where_clauses;
    SourceSpan span;
} RecordDecl;

/* Enum declaration */
typedef struct EnumDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    Vec(TypeExpr *) implements;
    Vec(EnumVariant) variants;
    Vec(ProcDecl) methods;
    Vec(WhereClause) where_clauses;
    SourceSpan span;
} EnumDecl;

/* Modal type declaration */
typedef struct ModalDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    Vec(TypeExpr *) implements;
    Vec(ModalState) states;
    Vec(ProcDecl) shared_methods;  /* Methods available in all states */
    Vec(WhereClause) where_clauses;
    SourceSpan span;
} ModalDecl;

/* Type alias declaration */
typedef struct TypeAliasDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    TypeExpr *aliased;
    SourceSpan span;
} TypeAliasDecl;

/* Class (interface) declaration */
typedef struct ClassDecl {
    Visibility vis;
    InternedString name;
    Vec(GenericParam) generics;
    Vec(TypeExpr *) superclasses;  /* Inherited classes */
    Vec(ProcDecl) methods;         /* Required method signatures */
    Vec(ProcDecl) default_methods; /* Methods with default implementations */
    Vec(WhereClause) where_clauses;
    SourceSpan span;
} ClassDecl;

/* Extern function declaration */
typedef struct ExternFuncDecl {
    InternedString name;
    InternedString link_name;  /* Optional different C name */
    Vec(ParamDecl) params;
    TypeExpr *return_type;
    SourceSpan span;
} ExternFuncDecl;

/* Extern block */
typedef struct ExternBlock {
    InternedString abi;        /* "C" */
    Vec(ExternFuncDecl) funcs;
    SourceSpan span;
} ExternBlock;

/* Import statement */
typedef struct ImportDecl {
    Vec(InternedString) path;
    SourceSpan span;
} ImportDecl;

/* Use statement */
typedef struct UseDecl {
    Vec(InternedString) path;
    Vec(InternedString) items;  /* Specific items, or empty for * */
    bool is_glob;               /* use path::* */
    InternedString alias;       /* use path as alias */
    SourceSpan span;
} UseDecl;

/* Top-level declaration */
struct Decl {
    DeclKind kind;
    SourceSpan span;

    union {
        ProcDecl proc;
        RecordDecl record;
        EnumDecl enum_;
        ModalDecl modal;
        TypeAliasDecl type_alias;
        ClassDecl class_;
        ExternBlock extern_;
        ImportDecl import;
        UseDecl use;
    };
};

/*
 * ============================================
 * Module
 * ============================================
 */

struct Module {
    InternedString name;
    Vec(Decl *) decls;
    SourceSpan span;
};

/*
 * ============================================
 * AST Construction Helpers
 * ============================================
 */

/* Create AST nodes using arena allocation */
Module *ast_new_module(Arena *arena);
Decl *ast_new_decl(Arena *arena, DeclKind kind, SourceSpan span);
Expr *ast_new_expr(Arena *arena, ExprKind kind, SourceSpan span);
Stmt *ast_new_stmt(Arena *arena, StmtKind kind, SourceSpan span);
TypeExpr *ast_new_type(Arena *arena, TypeExprKind kind, SourceSpan span);
Pattern *ast_new_pattern(Arena *arena, PatternKind kind, SourceSpan span);

#endif /* CURSIVE_AST_H */
