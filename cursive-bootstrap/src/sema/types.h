/*
 * Cursive Bootstrap Compiler - Semantic Type System
 *
 * Canonical type representation for type checking.
 * These are resolved types, distinct from AST TypeExpr nodes.
 */

#ifndef CURSIVE_TYPES_H
#define CURSIVE_TYPES_H

#include "common/common.h"
#include "common/arena.h"
#include "common/vec.h"
#include "common/string_pool.h"
#include "parser/ast.h"

/* Forward declarations */
typedef struct Type Type;
typedef struct Symbol Symbol;

/*
 * Type kinds
 */
typedef enum TypeKind {
    /* Primitive types */
    TYPE_PRIM_I8, TYPE_PRIM_I16, TYPE_PRIM_I32, TYPE_PRIM_I64, TYPE_PRIM_I128,
    TYPE_PRIM_U8, TYPE_PRIM_U16, TYPE_PRIM_U32, TYPE_PRIM_U64, TYPE_PRIM_U128,
    TYPE_PRIM_ISIZE, TYPE_PRIM_USIZE,
    TYPE_PRIM_F16, TYPE_PRIM_F32, TYPE_PRIM_F64,
    TYPE_PRIM_BOOL, TYPE_PRIM_CHAR,

    /* Special types */
    TYPE_UNIT,        /* () */
    TYPE_NEVER,       /* ! */
    TYPE_STRING,      /* string (modal) */

    /* Nominal types */
    TYPE_RECORD,      /* Record type */
    TYPE_ENUM,        /* Enum type */
    TYPE_MODAL,       /* Modal type (general, any state) */
    TYPE_MODAL_STATE, /* Modal type in specific state */
    TYPE_CLASS,       /* Class (interface) type */

    /* Structural types */
    TYPE_TUPLE,       /* (T1, T2, ...) */
    TYPE_ARRAY,       /* [T; N] */
    TYPE_SLICE,       /* [T] */
    TYPE_UNION,       /* T | U | V */
    TYPE_FUNCTION,    /* procedure(T1, ...) -> R */

    /* Pointer types */
    TYPE_PTR,         /* Ptr<T> (general) */
    TYPE_PTR_VALID,   /* Ptr<T>@Valid */
    TYPE_PTR_NULL,    /* Ptr<T>@Null */

    /* Generic */
    TYPE_GENERIC_PARAM,  /* Generic type parameter (T) */
    TYPE_GENERIC_INST,   /* Generic instantiation (Foo<i32, bool>) */

    /* Error/unknown */
    TYPE_ERROR,       /* Type error placeholder */
    TYPE_INFER        /* Type to be inferred */
} TypeKind;

/*
 * Type structure
 */
struct Type {
    TypeKind kind;
    Permission perm;           /* Permission modifier */

    union {
        /* TYPE_RECORD, TYPE_ENUM, TYPE_MODAL, TYPE_CLASS */
        struct {
            Symbol *sym;                /* Symbol for this type */
            Vec(Type *) type_args;      /* Generic arguments if any */
        } nominal;

        /* TYPE_MODAL_STATE */
        struct {
            Type *modal_type;           /* The underlying modal type */
            InternedString state_name;  /* The specific state */
        } modal_state;

        /* TYPE_TUPLE */
        struct {
            Vec(Type *) elements;
        } tuple;

        /* TYPE_ARRAY */
        struct {
            Type *element;
            size_t size;               /* Array size */
        } array;

        /* TYPE_SLICE */
        struct {
            Type *element;
        } slice;

        /* TYPE_UNION */
        struct {
            Vec(Type *) members;       /* Canonical order */
        } union_;

        /* TYPE_FUNCTION */
        struct {
            Vec(Type *) params;
            Type *return_type;
        } function;

        /* TYPE_PTR, TYPE_PTR_VALID, TYPE_PTR_NULL */
        struct {
            Type *pointee;
        } ptr;

        /* TYPE_GENERIC_PARAM */
        struct {
            InternedString name;
            size_t index;              /* Index in generic parameter list */
            Vec(Type *) bounds;        /* Class bounds */
        } generic_param;

        /* TYPE_GENERIC_INST */
        struct {
            Type *base;                /* The generic type definition */
            Vec(Type *) args;          /* Type arguments */
        } generic_inst;
    };
};

/*
 * Type context for managing canonical types
 */
typedef struct TypeContext {
    Arena *arena;
    StringPool *strings;

    /* Cached primitive types */
    Type *type_i8, *type_i16, *type_i32, *type_i64, *type_i128;
    Type *type_u8, *type_u16, *type_u32, *type_u64, *type_u128;
    Type *type_isize, *type_usize;
    Type *type_f16, *type_f32, *type_f64;
    Type *type_bool, *type_char;
    Type *type_unit, *type_never;
    Type *type_string;
    Type *type_error;
} TypeContext;

/* Initialize type context */
void type_ctx_init(TypeContext *ctx, Arena *arena, StringPool *strings);

/* Get primitive type */
Type *type_primitive(TypeContext *ctx, TypeKind kind);

/* Create nominal type */
Type *type_nominal(TypeContext *ctx, Symbol *sym, Vec(Type *) type_args);

/* Create modal state type */
Type *type_modal_state(TypeContext *ctx, Type *modal, InternedString state);

/* Create tuple type */
Type *type_tuple(TypeContext *ctx, Vec(Type *) elements);

/* Create array type */
Type *type_array(TypeContext *ctx, Type *element, size_t size);

/* Create slice type */
Type *type_slice(TypeContext *ctx, Type *element);

/* Create union type (canonicalizes member order) */
Type *type_union(TypeContext *ctx, Vec(Type *) members);

/* Create function type */
Type *type_function(TypeContext *ctx, Vec(Type *) params, Type *return_type);

/* Create pointer type */
Type *type_ptr(TypeContext *ctx, Type *pointee, TypeKind ptr_kind);

/* Create generic parameter type */
Type *type_generic_param(TypeContext *ctx, InternedString name, size_t index,
                         Vec(Type *) bounds);

/* Create generic instantiation */
Type *type_generic_inst(TypeContext *ctx, Type *base, Vec(Type *) args);

/* Apply permission to type */
Type *type_with_permission(TypeContext *ctx, Type *type, Permission perm);

/* Type equality (structural for structural types, nominal for nominal) */
bool type_equals(Type *a, Type *b);

/* Subtyping check: is `sub` a subtype of `super`? */
bool type_is_subtype(Type *sub, Type *super);

/* Is this type Copy? */
bool type_is_copy(Type *type);

/* Is this type sized? */
bool type_is_sized(Type *type);

/* Pretty-print type for diagnostics */
const char *type_to_string(Type *type, Arena *arena);

/* Get the error type (for error recovery) */
Type *type_error_type(TypeContext *ctx);

#endif /* CURSIVE_TYPES_H */
