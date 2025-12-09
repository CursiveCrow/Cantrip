/*
 * Cursive Bootstrap Compiler - Semantic Type System Implementation
 */

#include "types.h"
#include "scope.h"
#include <stdio.h>

/* Helper to allocate a type */
static Type *type_alloc(TypeContext *ctx, TypeKind kind) {
    Type *t = ARENA_ALLOC(ctx->arena, Type);
    memset(t, 0, sizeof(Type));
    t->kind = kind;
    t->perm = PERM_CONST;  /* Default permission */
    return t;
}

/* Initialize type context */
void type_ctx_init(TypeContext *ctx, Arena *arena, StringPool *strings) {
    ctx->arena = arena;
    ctx->strings = strings;

    /* Create cached primitive types */
    ctx->type_i8 = type_alloc(ctx, TYPE_PRIM_I8);
    ctx->type_i16 = type_alloc(ctx, TYPE_PRIM_I16);
    ctx->type_i32 = type_alloc(ctx, TYPE_PRIM_I32);
    ctx->type_i64 = type_alloc(ctx, TYPE_PRIM_I64);
    ctx->type_i128 = type_alloc(ctx, TYPE_PRIM_I128);
    ctx->type_isize = type_alloc(ctx, TYPE_PRIM_ISIZE);

    ctx->type_u8 = type_alloc(ctx, TYPE_PRIM_U8);
    ctx->type_u16 = type_alloc(ctx, TYPE_PRIM_U16);
    ctx->type_u32 = type_alloc(ctx, TYPE_PRIM_U32);
    ctx->type_u64 = type_alloc(ctx, TYPE_PRIM_U64);
    ctx->type_u128 = type_alloc(ctx, TYPE_PRIM_U128);
    ctx->type_usize = type_alloc(ctx, TYPE_PRIM_USIZE);

    ctx->type_f16 = type_alloc(ctx, TYPE_PRIM_F16);
    ctx->type_f32 = type_alloc(ctx, TYPE_PRIM_F32);
    ctx->type_f64 = type_alloc(ctx, TYPE_PRIM_F64);

    ctx->type_bool = type_alloc(ctx, TYPE_PRIM_BOOL);
    ctx->type_char = type_alloc(ctx, TYPE_PRIM_CHAR);

    ctx->type_unit = type_alloc(ctx, TYPE_UNIT);
    ctx->type_never = type_alloc(ctx, TYPE_NEVER);
    ctx->type_string = type_alloc(ctx, TYPE_STRING);

    ctx->type_error = type_alloc(ctx, TYPE_ERROR);
}

/* Get primitive type */
Type *type_primitive(TypeContext *ctx, TypeKind kind) {
    switch (kind) {
        case TYPE_PRIM_I8:    return ctx->type_i8;
        case TYPE_PRIM_I16:   return ctx->type_i16;
        case TYPE_PRIM_I32:   return ctx->type_i32;
        case TYPE_PRIM_I64:   return ctx->type_i64;
        case TYPE_PRIM_I128:  return ctx->type_i128;
        case TYPE_PRIM_ISIZE: return ctx->type_isize;
        case TYPE_PRIM_U8:    return ctx->type_u8;
        case TYPE_PRIM_U16:   return ctx->type_u16;
        case TYPE_PRIM_U32:   return ctx->type_u32;
        case TYPE_PRIM_U64:   return ctx->type_u64;
        case TYPE_PRIM_U128:  return ctx->type_u128;
        case TYPE_PRIM_USIZE: return ctx->type_usize;
        case TYPE_PRIM_F16:   return ctx->type_f16;
        case TYPE_PRIM_F32:   return ctx->type_f32;
        case TYPE_PRIM_F64:   return ctx->type_f64;
        case TYPE_PRIM_BOOL:  return ctx->type_bool;
        case TYPE_PRIM_CHAR:  return ctx->type_char;
        case TYPE_UNIT:       return ctx->type_unit;
        case TYPE_NEVER:      return ctx->type_never;
        case TYPE_STRING:     return ctx->type_string;
        default:              return ctx->type_error;
    }
}

/* Create nominal type */
Type *type_nominal(TypeContext *ctx, Symbol *sym, Vec(Type *) type_args) {
    Type *t = type_alloc(ctx, TYPE_RECORD);  /* Will be updated based on symbol */

    if (sym) {
        switch (sym->kind) {
            case SYM_TYPE:
                /* Determine actual kind from declaration */
                if (sym->decl) {
                    switch (sym->decl->kind) {
                        case DECL_RECORD: t->kind = TYPE_RECORD; break;
                        case DECL_ENUM:   t->kind = TYPE_ENUM; break;
                        case DECL_MODAL:  t->kind = TYPE_MODAL; break;
                        default:          t->kind = TYPE_RECORD; break;
                    }
                }
                break;
            case SYM_CLASS:
                t->kind = TYPE_CLASS;
                break;
            default:
                break;
        }
    }

    t->nominal.sym = sym;
    t->nominal.type_args = type_args;
    return t;
}

/* Create modal state type */
Type *type_modal_state(TypeContext *ctx, Type *modal, InternedString state) {
    Type *t = type_alloc(ctx, TYPE_MODAL_STATE);
    t->modal_state.modal_type = modal;
    t->modal_state.state_name = state;
    return t;
}

/* Create tuple type */
Type *type_tuple(TypeContext *ctx, Vec(Type *) elements) {
    /* Unit type for empty tuple */
    if (vec_len(elements) == 0) {
        return ctx->type_unit;
    }

    Type *t = type_alloc(ctx, TYPE_TUPLE);
    t->tuple.elements = elements;
    return t;
}

/* Create array type */
Type *type_array(TypeContext *ctx, Type *element, size_t size) {
    Type *t = type_alloc(ctx, TYPE_ARRAY);
    t->array.element = element;
    t->array.size = size;
    return t;
}

/* Create slice type */
Type *type_slice(TypeContext *ctx, Type *element) {
    Type *t = type_alloc(ctx, TYPE_SLICE);
    t->slice.element = element;
    return t;
}

/* Compare two types for ordering (for union canonicalization) */
static int type_compare(const void *a, const void *b) {
    Type *ta = *(Type **)a;
    Type *tb = *(Type **)b;

    /* Compare by kind first */
    if (ta->kind != tb->kind) {
        return (int)ta->kind - (int)tb->kind;
    }

    /* For nominal types, compare by symbol pointer */
    if (ta->kind >= TYPE_RECORD && ta->kind <= TYPE_CLASS) {
        return (int)((char *)ta->nominal.sym - (char *)tb->nominal.sym);
    }

    /* For other types, just use pointer for now */
    return (int)((char *)ta - (char *)tb);
}

/* Create union type (canonicalizes member order) */
Type *type_union(TypeContext *ctx, Vec(Type *) members) {
    size_t len = vec_len(members);

    /* Single member - just return that type */
    if (len == 1) {
        return members[0];
    }

    /* Sort members for canonical ordering */
    /* Simple bubble sort for small arrays */
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            if (type_compare(&members[j], &members[j + 1]) > 0) {
                Type *tmp = members[j];
                members[j] = members[j + 1];
                members[j + 1] = tmp;
            }
        }
    }

    /* Remove duplicates */
    Vec(Type *) unique = vec_new(Type *);
    Type *prev = NULL;
    for (size_t i = 0; i < len; i++) {
        if (!prev || !type_equals(prev, members[i])) {
            vec_push(unique, members[i]);
            prev = members[i];
        }
    }

    /* Single unique member */
    if (vec_len(unique) == 1) {
        Type *result = unique[0];
        vec_free(unique);
        return result;
    }

    Type *t = type_alloc(ctx, TYPE_UNION);
    t->union_.members = unique;
    return t;
}

/* Create function type */
Type *type_function(TypeContext *ctx, Vec(Type *) params, Type *return_type) {
    Type *t = type_alloc(ctx, TYPE_FUNCTION);
    t->function.params = params;
    t->function.return_type = return_type ? return_type : ctx->type_unit;
    return t;
}

/* Create pointer type */
Type *type_ptr(TypeContext *ctx, Type *pointee, TypeKind ptr_kind) {
    Type *t = type_alloc(ctx, ptr_kind);
    t->ptr.pointee = pointee;
    return t;
}

/* Create generic parameter type */
Type *type_generic_param(TypeContext *ctx, InternedString name, size_t index,
                         Vec(Type *) bounds) {
    Type *t = type_alloc(ctx, TYPE_GENERIC_PARAM);
    t->generic_param.name = name;
    t->generic_param.index = index;
    t->generic_param.bounds = bounds;
    return t;
}

/* Create generic instantiation */
Type *type_generic_inst(TypeContext *ctx, Type *base, Vec(Type *) args) {
    Type *t = type_alloc(ctx, TYPE_GENERIC_INST);
    t->generic_inst.base = base;
    t->generic_inst.args = args;
    return t;
}

/* Apply permission to type */
Type *type_with_permission(TypeContext *ctx, Type *type, Permission perm) {
    if (type->perm == perm) {
        return type;
    }

    /* Create a copy with new permission */
    Type *t = ARENA_ALLOC(ctx->arena, Type);
    *t = *type;
    t->perm = perm;
    return t;
}

/* Type equality */
bool type_equals(Type *a, Type *b) {
    if (a == b) return true;
    if (!a || !b) return false;
    if (a->kind != b->kind) return false;

    switch (a->kind) {
        /* Primitives are equal by kind */
        case TYPE_PRIM_I8:
        case TYPE_PRIM_I16:
        case TYPE_PRIM_I32:
        case TYPE_PRIM_I64:
        case TYPE_PRIM_I128:
        case TYPE_PRIM_ISIZE:
        case TYPE_PRIM_U8:
        case TYPE_PRIM_U16:
        case TYPE_PRIM_U32:
        case TYPE_PRIM_U64:
        case TYPE_PRIM_U128:
        case TYPE_PRIM_USIZE:
        case TYPE_PRIM_F16:
        case TYPE_PRIM_F32:
        case TYPE_PRIM_F64:
        case TYPE_PRIM_BOOL:
        case TYPE_PRIM_CHAR:
        case TYPE_UNIT:
        case TYPE_NEVER:
        case TYPE_STRING:
            return true;

        /* Nominal types: compare by symbol */
        case TYPE_RECORD:
        case TYPE_ENUM:
        case TYPE_MODAL:
        case TYPE_CLASS:
            if (a->nominal.sym != b->nominal.sym) return false;
            /* Compare type arguments */
            if (vec_len(a->nominal.type_args) != vec_len(b->nominal.type_args)) {
                return false;
            }
            for (size_t i = 0; i < vec_len(a->nominal.type_args); i++) {
                if (!type_equals(a->nominal.type_args[i], b->nominal.type_args[i])) {
                    return false;
                }
            }
            return true;

        case TYPE_MODAL_STATE:
            return type_equals(a->modal_state.modal_type, b->modal_state.modal_type) &&
                   a->modal_state.state_name.data == b->modal_state.state_name.data;

        case TYPE_TUPLE:
            if (vec_len(a->tuple.elements) != vec_len(b->tuple.elements)) {
                return false;
            }
            for (size_t i = 0; i < vec_len(a->tuple.elements); i++) {
                if (!type_equals(a->tuple.elements[i], b->tuple.elements[i])) {
                    return false;
                }
            }
            return true;

        case TYPE_ARRAY:
            return a->array.size == b->array.size &&
                   type_equals(a->array.element, b->array.element);

        case TYPE_SLICE:
            return type_equals(a->slice.element, b->slice.element);

        case TYPE_UNION:
            if (vec_len(a->union_.members) != vec_len(b->union_.members)) {
                return false;
            }
            /* Members are canonically ordered */
            for (size_t i = 0; i < vec_len(a->union_.members); i++) {
                if (!type_equals(a->union_.members[i], b->union_.members[i])) {
                    return false;
                }
            }
            return true;

        case TYPE_FUNCTION:
            if (vec_len(a->function.params) != vec_len(b->function.params)) {
                return false;
            }
            for (size_t i = 0; i < vec_len(a->function.params); i++) {
                if (!type_equals(a->function.params[i], b->function.params[i])) {
                    return false;
                }
            }
            return type_equals(a->function.return_type, b->function.return_type);

        case TYPE_PTR:
        case TYPE_PTR_VALID:
        case TYPE_PTR_NULL:
            return type_equals(a->ptr.pointee, b->ptr.pointee);

        case TYPE_GENERIC_PARAM:
            return a->generic_param.name.data == b->generic_param.name.data &&
                   a->generic_param.index == b->generic_param.index;

        case TYPE_GENERIC_INST:
            if (!type_equals(a->generic_inst.base, b->generic_inst.base)) {
                return false;
            }
            if (vec_len(a->generic_inst.args) != vec_len(b->generic_inst.args)) {
                return false;
            }
            for (size_t i = 0; i < vec_len(a->generic_inst.args); i++) {
                if (!type_equals(a->generic_inst.args[i], b->generic_inst.args[i])) {
                    return false;
                }
            }
            return true;

        case TYPE_ERROR:
        case TYPE_INFER:
            return true;

        default:
            return false;
    }
}

/* Subtyping check */
bool type_is_subtype(Type *sub, Type *super) {
    if (type_equals(sub, super)) return true;
    if (!sub || !super) return false;

    /* Never is a subtype of everything */
    if (sub->kind == TYPE_NEVER) return true;

    /* Permission subtyping: unique <: const */
    if (sub->perm == PERM_UNIQUE && super->perm == PERM_CONST) {
        /* Check structural equality ignoring permission */
        Type sub_copy = *sub;
        sub_copy.perm = PERM_CONST;
        return type_equals(&sub_copy, super);
    }

    /* Union subtyping: T <: T | U */
    if (super->kind == TYPE_UNION) {
        for (size_t i = 0; i < vec_len(super->union_.members); i++) {
            if (type_is_subtype(sub, super->union_.members[i])) {
                return true;
            }
        }
    }

    /* Modal state subtyping: M@S <: M (widening) */
    if (sub->kind == TYPE_MODAL_STATE && super->kind == TYPE_MODAL) {
        return type_equals(sub->modal_state.modal_type, super);
    }

    return false;
}

/* Is this type Copy? */
bool type_is_copy(Type *type) {
    if (!type) return false;

    switch (type->kind) {
        /* Primitives are Copy */
        case TYPE_PRIM_I8:
        case TYPE_PRIM_I16:
        case TYPE_PRIM_I32:
        case TYPE_PRIM_I64:
        case TYPE_PRIM_I128:
        case TYPE_PRIM_ISIZE:
        case TYPE_PRIM_U8:
        case TYPE_PRIM_U16:
        case TYPE_PRIM_U32:
        case TYPE_PRIM_U64:
        case TYPE_PRIM_U128:
        case TYPE_PRIM_USIZE:
        case TYPE_PRIM_F16:
        case TYPE_PRIM_F32:
        case TYPE_PRIM_F64:
        case TYPE_PRIM_BOOL:
        case TYPE_PRIM_CHAR:
        case TYPE_UNIT:
        case TYPE_NEVER:
            return true;

        /* Tuples are Copy if all elements are Copy */
        case TYPE_TUPLE: {
            for (size_t i = 0; i < vec_len(type->tuple.elements); i++) {
                if (!type_is_copy(type->tuple.elements[i])) {
                    return false;
                }
            }
            return true;
        }

        /* Arrays are Copy if element is Copy */
        case TYPE_ARRAY:
            return type_is_copy(type->array.element);

        /* string@View is Copy, string@Managed is not */
        case TYPE_STRING:
            /* For bootstrap, treat as non-Copy by default */
            return false;

        /* Pointers: const is Copy, unique is not */
        case TYPE_PTR:
        case TYPE_PTR_VALID:
        case TYPE_PTR_NULL:
            return type->perm == PERM_CONST;

        /* Nominal types need to check class implementation */
        case TYPE_RECORD:
        case TYPE_ENUM:
            /* Would need to check if type implements Copy */
            /* For bootstrap, default to non-Copy */
            return false;

        default:
            return false;
    }
}

/* Is this type sized? */
bool type_is_sized(Type *type) {
    if (!type) return false;

    switch (type->kind) {
        case TYPE_SLICE:
            return false;  /* Slices are unsized */

        case TYPE_NEVER:
            return false;  /* Never is zero-sized */

        default:
            return true;
    }
}

/* Pretty-print type for diagnostics */
const char *type_to_string(Type *type, Arena *arena) {
    if (!type) return "<null>";

    char buf[256];
    const char *perm_str = "";
    if (type->perm == PERM_UNIQUE) perm_str = "unique ";
    else if (type->perm == PERM_SHARED) perm_str = "shared ";

    switch (type->kind) {
        case TYPE_PRIM_I8:    snprintf(buf, sizeof(buf), "%si8", perm_str); break;
        case TYPE_PRIM_I16:   snprintf(buf, sizeof(buf), "%si16", perm_str); break;
        case TYPE_PRIM_I32:   snprintf(buf, sizeof(buf), "%si32", perm_str); break;
        case TYPE_PRIM_I64:   snprintf(buf, sizeof(buf), "%si64", perm_str); break;
        case TYPE_PRIM_I128:  snprintf(buf, sizeof(buf), "%si128", perm_str); break;
        case TYPE_PRIM_ISIZE: snprintf(buf, sizeof(buf), "%sisize", perm_str); break;
        case TYPE_PRIM_U8:    snprintf(buf, sizeof(buf), "%su8", perm_str); break;
        case TYPE_PRIM_U16:   snprintf(buf, sizeof(buf), "%su16", perm_str); break;
        case TYPE_PRIM_U32:   snprintf(buf, sizeof(buf), "%su32", perm_str); break;
        case TYPE_PRIM_U64:   snprintf(buf, sizeof(buf), "%su64", perm_str); break;
        case TYPE_PRIM_U128:  snprintf(buf, sizeof(buf), "%su128", perm_str); break;
        case TYPE_PRIM_USIZE: snprintf(buf, sizeof(buf), "%susize", perm_str); break;
        case TYPE_PRIM_F16:   snprintf(buf, sizeof(buf), "%sf16", perm_str); break;
        case TYPE_PRIM_F32:   snprintf(buf, sizeof(buf), "%sf32", perm_str); break;
        case TYPE_PRIM_F64:   snprintf(buf, sizeof(buf), "%sf64", perm_str); break;
        case TYPE_PRIM_BOOL:  snprintf(buf, sizeof(buf), "%sbool", perm_str); break;
        case TYPE_PRIM_CHAR:  snprintf(buf, sizeof(buf), "%schar", perm_str); break;
        case TYPE_UNIT:       snprintf(buf, sizeof(buf), "()"); break;
        case TYPE_NEVER:      snprintf(buf, sizeof(buf), "!"); break;
        case TYPE_STRING:     snprintf(buf, sizeof(buf), "%sstring", perm_str); break;

        case TYPE_RECORD:
        case TYPE_ENUM:
        case TYPE_MODAL:
        case TYPE_CLASS:
            if (type->nominal.sym) {
                snprintf(buf, sizeof(buf), "%s%.*s", perm_str,
                         (int)type->nominal.sym->name.len,
                         type->nominal.sym->name.data);
            } else {
                snprintf(buf, sizeof(buf), "%s<unknown>", perm_str);
            }
            break;

        case TYPE_MODAL_STATE: {
            const char *modal = type_to_string(type->modal_state.modal_type, arena);
            snprintf(buf, sizeof(buf), "%s%s@%.*s", perm_str, modal,
                     (int)type->modal_state.state_name.len,
                     type->modal_state.state_name.data);
            break;
        }

        case TYPE_TUPLE:
            snprintf(buf, sizeof(buf), "%s(tuple)", perm_str);
            break;

        case TYPE_ARRAY:
            snprintf(buf, sizeof(buf), "%s[%s; %zu]", perm_str,
                     type_to_string(type->array.element, arena),
                     type->array.size);
            break;

        case TYPE_SLICE:
            snprintf(buf, sizeof(buf), "%s[%s]", perm_str,
                     type_to_string(type->slice.element, arena));
            break;

        case TYPE_UNION:
            snprintf(buf, sizeof(buf), "%s(union)", perm_str);
            break;

        case TYPE_FUNCTION:
            snprintf(buf, sizeof(buf), "%sprocedure(...)", perm_str);
            break;

        case TYPE_PTR:
            snprintf(buf, sizeof(buf), "%sPtr<%s>", perm_str,
                     type_to_string(type->ptr.pointee, arena));
            break;

        case TYPE_PTR_VALID:
            snprintf(buf, sizeof(buf), "%sPtr<%s>@Valid", perm_str,
                     type_to_string(type->ptr.pointee, arena));
            break;

        case TYPE_PTR_NULL:
            snprintf(buf, sizeof(buf), "%sPtr<%s>@Null", perm_str,
                     type_to_string(type->ptr.pointee, arena));
            break;

        case TYPE_GENERIC_PARAM:
            snprintf(buf, sizeof(buf), "%.*s",
                     (int)type->generic_param.name.len,
                     type->generic_param.name.data);
            break;

        case TYPE_GENERIC_INST:
            snprintf(buf, sizeof(buf), "%s<...>",
                     type_to_string(type->generic_inst.base, arena));
            break;

        case TYPE_ERROR:
            snprintf(buf, sizeof(buf), "<error>");
            break;

        case TYPE_INFER:
            snprintf(buf, sizeof(buf), "_");
            break;

        default:
            snprintf(buf, sizeof(buf), "<unknown>");
            break;
    }

    /* Copy to arena */
    size_t len = strlen(buf) + 1;
    char *result = arena_alloc_aligned(arena, len, 1);
    memcpy(result, buf, len);
    return result;
}

/* Get the error type */
Type *type_error_type(TypeContext *ctx) {
    return ctx->type_error;
}
