/*
 * Cursive Bootstrap Compiler - Type Checking
 *
 * Type checking pass that:
 * - Infers types for all expressions
 * - Checks type compatibility
 * - Resolves method calls
 * - Validates generic instantiations
 * - Checks modal type states
 */

#include "sema.h"
#include "scope.h"
#include "types.h"
#include "common/error.h"
#include <stdio.h>

/* Type checking context */
typedef struct TypeCheckContext {
    SemaContext *sema;
    Arena *arena;
    DiagContext *diag;
    TypeContext *types;
    StringPool *strings;

    /* Current context */
    Decl *current_type_decl;
    ProcDecl *current_proc;
    Type *current_return_type;

    /* Scope for looking up symbols */
    Scope *scope;
} TypeCheckContext;

/* Forward declarations */
static Type *check_expr(TypeCheckContext *ctx, Expr *expr, Type *expected);
static void check_stmt(TypeCheckContext *ctx, Stmt *stmt);
static Type *resolve_type_expr(TypeCheckContext *ctx, TypeExpr *texpr);
static Type *check_pattern(TypeCheckContext *ctx, Pattern *pat, Type *expected);

/* Error reporting helpers */
static void error_type_mismatch(TypeCheckContext *ctx, SourceSpan span,
                                Type *expected, Type *actual) {
    diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, span,
        "type mismatch: expected '%s', found '%s'",
        type_to_string(expected, ctx->arena),
        type_to_string(actual, ctx->arena));
}

static void error_not_callable(TypeCheckContext *ctx, SourceSpan span, Type *type) {
    diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, span,
        "type '%s' is not callable",
        type_to_string(type, ctx->arena));
}

static void error_no_field(TypeCheckContext *ctx, SourceSpan span,
                          Type *type, InternedString field) {
    diag_report(ctx->diag, DIAG_ERROR, E_TYP_2052, span,
        "type '%s' has no field '%.*s'",
        type_to_string(type, ctx->arena),
        (int)field.len, field.data);
}

static void error_no_method(TypeCheckContext *ctx, SourceSpan span,
                           Type *type, InternedString method) {
    diag_report(ctx->diag, DIAG_ERROR, E_TYP_2053, span,
        "type '%s' has no method '%.*s'",
        type_to_string(type, ctx->arena),
        (int)method.len, method.data);
}

static void error_wrong_arg_count(TypeCheckContext *ctx, SourceSpan span,
                                  size_t expected, size_t actual) {
    diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, span,
        "expected %zu arguments, found %zu",
        expected, actual);
}

/*
 * Convert PrimitiveType (AST) to TypeKind (semantic)
 */
static TypeKind prim_to_type_kind(PrimitiveType prim) {
    switch (prim) {
        case PRIM_I8:     return TYPE_PRIM_I8;
        case PRIM_I16:    return TYPE_PRIM_I16;
        case PRIM_I32:    return TYPE_PRIM_I32;
        case PRIM_I64:    return TYPE_PRIM_I64;
        case PRIM_I128:   return TYPE_PRIM_I128;
        case PRIM_ISIZE:  return TYPE_PRIM_ISIZE;
        case PRIM_U8:     return TYPE_PRIM_U8;
        case PRIM_U16:    return TYPE_PRIM_U16;
        case PRIM_U32:    return TYPE_PRIM_U32;
        case PRIM_U64:    return TYPE_PRIM_U64;
        case PRIM_U128:   return TYPE_PRIM_U128;
        case PRIM_USIZE:  return TYPE_PRIM_USIZE;
        case PRIM_F16:    return TYPE_PRIM_F16;
        case PRIM_F32:    return TYPE_PRIM_F32;
        case PRIM_F64:    return TYPE_PRIM_F64;
        case PRIM_BOOL:   return TYPE_PRIM_BOOL;
        case PRIM_CHAR:   return TYPE_PRIM_CHAR;
        case PRIM_STRING: return TYPE_STRING;
        default:          return TYPE_ERROR;
    }
}

/*
 * Convert AST TypeExpr to semantic Type
 */
static Type *resolve_type_expr(TypeCheckContext *ctx, TypeExpr *texpr) {
    if (!texpr) return ctx->types->type_unit;

    Type *result = NULL;

    switch (texpr->kind) {
        case TEXPR_PRIMITIVE:
            result = type_primitive(ctx->types, prim_to_type_kind(texpr->primitive));
            break;

        case TEXPR_NAMED: {
            /* Look up the type symbol */
            Symbol *sym = scope_lookup_from(ctx->scope, texpr->named.name);
            if (!sym) {
                diag_report(ctx->diag, DIAG_ERROR, E_RES_0200, texpr->span, "unknown type");
                return type_error_type(ctx->types);
            }
            Vec(Type *) args = vec_new(Type *);
            result = type_nominal(ctx->types, sym, args);
            break;
        }

        case TEXPR_MODAL_STATE: {
            Type *base = resolve_type_expr(ctx, texpr->modal_state.base);
            result = type_modal_state(ctx->types, base, texpr->modal_state.state);
            break;
        }

        case TEXPR_GENERIC: {
            Type *base = resolve_type_expr(ctx, texpr->generic.base);
            Vec(Type *) args = vec_new(Type *);
            for (size_t i = 0; i < vec_len(texpr->generic.args); i++) {
                Type *arg = resolve_type_expr(ctx, texpr->generic.args[i]);
                vec_push(args, arg);
            }
            result = type_generic_inst(ctx->types, base, args);
            break;
        }

        case TEXPR_TUPLE: {
            Vec(Type *) elements = vec_new(Type *);
            for (size_t i = 0; i < vec_len(texpr->tuple.elements); i++) {
                Type *elem = resolve_type_expr(ctx, texpr->tuple.elements[i]);
                vec_push(elements, elem);
            }
            result = type_tuple(ctx->types, elements);
            break;
        }

        case TEXPR_ARRAY: {
            Type *elem = resolve_type_expr(ctx, texpr->array.element);
            /* For bootstrap, assume array size is a constant integer literal */
            size_t size = 0;
            if (texpr->array.size && texpr->array.size->kind == EXPR_INT_LIT) {
                size = (size_t)texpr->array.size->int_lit.value;
            }
            result = type_array(ctx->types, elem, size);
            break;
        }

        case TEXPR_SLICE: {
            Type *elem = resolve_type_expr(ctx, texpr->slice.element);
            result = type_slice(ctx->types, elem);
            break;
        }

        case TEXPR_FUNCTION: {
            Vec(Type *) params = vec_new(Type *);
            for (size_t i = 0; i < vec_len(texpr->function.params); i++) {
                Type *param = resolve_type_expr(ctx, texpr->function.params[i]);
                vec_push(params, param);
            }
            Type *ret = resolve_type_expr(ctx, texpr->function.return_type);
            result = type_function(ctx->types, params, ret);
            break;
        }

        case TEXPR_UNION: {
            Vec(Type *) members = vec_new(Type *);
            for (size_t i = 0; i < vec_len(texpr->union_.members); i++) {
                Type *mem = resolve_type_expr(ctx, texpr->union_.members[i]);
                vec_push(members, mem);
            }
            result = type_union(ctx->types, members);
            break;
        }

        case TEXPR_PTR: {
            Type *pointee = resolve_type_expr(ctx, texpr->ptr.pointee);
            result = type_ptr(ctx->types, pointee, TYPE_PTR);
            break;
        }

        case TEXPR_REF: {
            Type *referent = resolve_type_expr(ctx, texpr->ref.referent);
            TypeKind kind = texpr->ref.is_unique ? TYPE_PTR_VALID : TYPE_PTR_VALID;
            result = type_ptr(ctx->types, referent, kind);
            if (texpr->ref.is_unique) {
                result = type_with_permission(ctx->types, result, PERM_UNIQUE);
            }
            break;
        }

        case TEXPR_NEVER:
            result = ctx->types->type_never;
            break;

        case TEXPR_UNIT:
            result = ctx->types->type_unit;
            break;

        case TEXPR_SELF:
            /* Look for Self in enclosing type scope */
            /* For bootstrap, return error type if not found */
            result = type_error_type(ctx->types);
            break;

        case TEXPR_INFER:
            /* Will be inferred later */
            result = type_error_type(ctx->types);
            break;
    }

    /* Apply permission from AST */
    if (texpr->perm != PERM_CONST) {
        result = type_with_permission(ctx->types, result, texpr->perm);
    }

    return result;
}

/*
 * Check if two types are compatible (with implicit conversions)
 */
static bool types_compatible(TypeCheckContext *ctx, Type *expected, Type *actual) {
    if (type_equals(expected, actual)) return true;
    if (type_is_subtype(actual, expected)) return true;

    /* Error type is compatible with anything (error recovery) */
    if (expected->kind == TYPE_ERROR || actual->kind == TYPE_ERROR) return true;

    return false;
}

/*
 * Get the result type of a binary operation
 */
static Type *binary_result_type(TypeCheckContext *ctx, BinaryOp op,
                                Type *left, Type *right) {
    /* Arithmetic operations */
    if (op >= BINOP_ADD && op <= BINOP_POW) {
        /* Both operands must be numeric and same type */
        if (type_equals(left, right) &&
            left->kind >= TYPE_PRIM_I8 && left->kind <= TYPE_PRIM_F64) {
            return left;
        }
        return type_error_type(ctx->types);
    }

    /* Comparison operations return bool */
    if (op >= BINOP_EQ && op <= BINOP_GE) {
        return ctx->types->type_bool;
    }

    /* Logical operations */
    if (op == BINOP_AND || op == BINOP_OR) {
        if (left->kind == TYPE_PRIM_BOOL && right->kind == TYPE_PRIM_BOOL) {
            return ctx->types->type_bool;
        }
        return type_error_type(ctx->types);
    }

    /* Bitwise operations */
    if (op >= BINOP_BIT_AND && op <= BINOP_SHR) {
        if (type_equals(left, right) &&
            left->kind >= TYPE_PRIM_I8 && left->kind <= TYPE_PRIM_USIZE) {
            return left;
        }
        return type_error_type(ctx->types);
    }

    /* Assignment returns unit */
    if (op == BINOP_ASSIGN) {
        return ctx->types->type_unit;
    }

    /* Compound assignment returns unit */
    if (op >= BINOP_ADD_ASSIGN && op <= BINOP_SHR_ASSIGN) {
        return ctx->types->type_unit;
    }

    return type_error_type(ctx->types);
}

/*
 * Get the result type of a unary operation
 */
static Type *unary_result_type(TypeCheckContext *ctx, UnaryOp op, Type *operand) {
    switch (op) {
        case UNOP_NEG:
            /* Numeric types */
            if (operand->kind >= TYPE_PRIM_I8 && operand->kind <= TYPE_PRIM_F64) {
                return operand;
            }
            return type_error_type(ctx->types);

        case UNOP_NOT:
            /* Boolean */
            if (operand->kind == TYPE_PRIM_BOOL) {
                return ctx->types->type_bool;
            }
            return type_error_type(ctx->types);

        case UNOP_BIT_NOT:
            /* Integer types */
            if (operand->kind >= TYPE_PRIM_I8 && operand->kind <= TYPE_PRIM_USIZE) {
                return operand;
            }
            return type_error_type(ctx->types);

        case UNOP_DEREF:
            /* Pointer types */
            if (operand->kind == TYPE_PTR || operand->kind == TYPE_PTR_VALID) {
                return operand->ptr.pointee;
            }
            return type_error_type(ctx->types);

        case UNOP_ADDR:
        case UNOP_ADDR_MUT: {
            /* Create pointer to operand type */
            Type *ptr = type_ptr(ctx->types, operand, TYPE_PTR_VALID);
            if (op == UNOP_ADDR_MUT) {
                ptr = type_with_permission(ctx->types, ptr, PERM_UNIQUE);
            }
            return ptr;
        }

        case UNOP_TRY:
            /* Error propagation - for union types, extracts success type */
            if (operand->kind == TYPE_UNION && vec_len(operand->union_.members) > 0) {
                /* Return first non-error member */
                return operand->union_.members[0];
            }
            return operand;
    }

    return type_error_type(ctx->types);
}

/*
 * Look up a field in a type
 */
static Type *lookup_field(TypeCheckContext *ctx, Type *type, InternedString name,
                          SourceSpan span) {
    if (type->kind == TYPE_ERROR) return type;

    if (type->kind == TYPE_RECORD && type->nominal.sym && type->nominal.sym->decl) {
        Decl *decl = type->nominal.sym->decl;
        if (decl->kind == DECL_RECORD) {
            RecordDecl *rec = &decl->record;
            for (size_t i = 0; i < vec_len(rec->fields); i++) {
                if (rec->fields[i].name.data == name.data) {
                    return resolve_type_expr(ctx, rec->fields[i].type);
                }
            }
        }
    }

    /* Tuple field access */
    if (type->kind == TYPE_TUPLE) {
        /* Field name should be a number like "0", "1", etc. */
        /* For bootstrap, basic handling */
        if (name.len == 1 && name.data[0] >= '0' && name.data[0] <= '9') {
            size_t idx = name.data[0] - '0';
            if (idx < vec_len(type->tuple.elements)) {
                return type->tuple.elements[idx];
            }
        }
    }

    error_no_field(ctx, span, type, name);
    return type_error_type(ctx->types);
}

/*
 * Look up a method in a type
 */
static Type *lookup_method(TypeCheckContext *ctx, Type *type, InternedString name,
                          SourceSpan span, ProcDecl **out_method) {
    *out_method = NULL;

    if (type->kind == TYPE_ERROR) return type;

    if (type->nominal.sym && type->nominal.sym->decl) {
        Decl *decl = type->nominal.sym->decl;

        /* Check record methods */
        if (decl->kind == DECL_RECORD) {
            RecordDecl *rec = &decl->record;
            for (size_t i = 0; i < vec_len(rec->methods); i++) {
                if (rec->methods[i].name.data == name.data) {
                    *out_method = &rec->methods[i];
                    /* Build function type from method signature */
                    Vec(Type *) params = vec_new(Type *);
                    for (size_t j = 0; j < vec_len(rec->methods[i].params); j++) {
                        Type *pt = resolve_type_expr(ctx, rec->methods[i].params[j].type);
                        vec_push(params, pt);
                    }
                    Type *ret = resolve_type_expr(ctx, rec->methods[i].return_type);
                    return type_function(ctx->types, params, ret);
                }
            }
        }

        /* Check enum methods */
        if (decl->kind == DECL_ENUM) {
            EnumDecl *en = &decl->enum_;
            for (size_t i = 0; i < vec_len(en->methods); i++) {
                if (en->methods[i].name.data == name.data) {
                    *out_method = &en->methods[i];
                    Vec(Type *) params = vec_new(Type *);
                    for (size_t j = 0; j < vec_len(en->methods[i].params); j++) {
                        Type *pt = resolve_type_expr(ctx, en->methods[i].params[j].type);
                        vec_push(params, pt);
                    }
                    Type *ret = resolve_type_expr(ctx, en->methods[i].return_type);
                    return type_function(ctx->types, params, ret);
                }
            }
        }

        /* Check modal type methods */
        if (decl->kind == DECL_MODAL) {
            ModalDecl *modal = &decl->modal;

            /* Check shared methods first */
            for (size_t i = 0; i < vec_len(modal->shared_methods); i++) {
                if (modal->shared_methods[i].name.data == name.data) {
                    *out_method = &modal->shared_methods[i];
                    Vec(Type *) params = vec_new(Type *);
                    for (size_t j = 0; j < vec_len(modal->shared_methods[i].params); j++) {
                        Type *pt = resolve_type_expr(ctx, modal->shared_methods[i].params[j].type);
                        vec_push(params, pt);
                    }
                    Type *ret = resolve_type_expr(ctx, modal->shared_methods[i].return_type);
                    return type_function(ctx->types, params, ret);
                }
            }

            /* Check state-specific methods if we have a modal state type */
            if (type->kind == TYPE_MODAL_STATE) {
                for (size_t s = 0; s < vec_len(modal->states); s++) {
                    ModalState *state = &modal->states[s];
                    if (state->name.data == type->modal_state.state_name.data) {
                        /* Found the state, check its methods */
                        for (size_t i = 0; i < vec_len(state->methods); i++) {
                            if (state->methods[i].name.data == name.data) {
                                *out_method = &state->methods[i];
                                Vec(Type *) params = vec_new(Type *);
                                for (size_t j = 0; j < vec_len(state->methods[i].params); j++) {
                                    Type *pt = resolve_type_expr(ctx, state->methods[i].params[j].type);
                                    vec_push(params, pt);
                                }
                                Type *ret = resolve_type_expr(ctx, state->methods[i].return_type);
                                return type_function(ctx->types, params, ret);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    error_no_method(ctx, span, type, name);
    return type_error_type(ctx->types);
}

/*
 * Check a pattern and return its type
 */
static Type *check_pattern(TypeCheckContext *ctx, Pattern *pat, Type *expected) {
    if (!pat) return expected;

    switch (pat->kind) {
        case PAT_WILDCARD:
            return expected;

        case PAT_BINDING: {
            Type *binding_type = expected;
            if (pat->binding.type) {
                binding_type = resolve_type_expr(ctx, pat->binding.type);
                if (expected && !types_compatible(ctx, expected, binding_type)) {
                    error_type_mismatch(ctx, pat->span, expected, binding_type);
                }
            }
            /* Set the type on the resolved symbol for later phases (move analysis) */
            if (pat->binding.resolved && binding_type) {
                pat->binding.resolved->type = binding_type;
            }
            return binding_type;
        }

        case PAT_LITERAL:
            return check_expr(ctx, pat->literal.value, expected);

        case PAT_TUPLE: {
            if (expected && expected->kind == TYPE_TUPLE) {
                if (vec_len(pat->tuple.elements) != vec_len(expected->tuple.elements)) {
                    diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, pat->span,
                        "tuple pattern has %zu elements, expected %zu",
                        vec_len(pat->tuple.elements), vec_len(expected->tuple.elements));
                }
                for (size_t i = 0; i < vec_len(pat->tuple.elements); i++) {
                    Type *elem_expected = i < vec_len(expected->tuple.elements)
                        ? expected->tuple.elements[i] : NULL;
                    check_pattern(ctx, pat->tuple.elements[i], elem_expected);
                }
            }
            return expected;
        }

        case PAT_RECORD:
            /* Type check handled by the type annotation in pattern */
            return expected;

        case PAT_ENUM:
            return expected;

        case PAT_MODAL:
            return expected;

        case PAT_RANGE:
            check_pattern(ctx, pat->range.start, expected);
            check_pattern(ctx, pat->range.end, expected);
            return expected;

        case PAT_OR:
            for (size_t i = 0; i < vec_len(pat->or_.alternatives); i++) {
                check_pattern(ctx, pat->or_.alternatives[i], expected);
            }
            return expected;

        case PAT_GUARD:
            check_pattern(ctx, pat->guard.pattern, expected);
            check_expr(ctx, pat->guard.guard, ctx->types->type_bool);
            return expected;
    }

    return expected;
}

/*
 * Check an expression and return its type
 */
static Type *check_expr(TypeCheckContext *ctx, Expr *expr, Type *expected) {
    if (!expr) return ctx->types->type_unit;

    Type *result = NULL;

    switch (expr->kind) {
        case EXPR_INT_LIT:
            /* Default to i32, or use expected type if numeric */
            if (expected && expected->kind >= TYPE_PRIM_I8 &&
                expected->kind <= TYPE_PRIM_USIZE) {
                result = expected;
            } else {
                result = ctx->types->type_i32;
            }
            break;

        case EXPR_FLOAT_LIT:
            if (expected && expected->kind >= TYPE_PRIM_F16 &&
                expected->kind <= TYPE_PRIM_F64) {
                result = expected;
            } else {
                result = ctx->types->type_f64;
            }
            break;

        case EXPR_STRING_LIT:
            result = ctx->types->type_string;
            break;

        case EXPR_CHAR_LIT:
            result = ctx->types->type_char;
            break;

        case EXPR_BOOL_LIT:
            result = ctx->types->type_bool;
            break;

        case EXPR_IDENT: {
            Symbol *sym = scope_lookup_from(ctx->scope, expr->ident.name);
            if (!sym) {
                result = type_error_type(ctx->types);
            } else if (sym->type) {
                result = sym->type;
            } else if (sym->decl) {
                /* Get type from declaration */
                switch (sym->kind) {
                    case SYM_VAR:
                    case SYM_PARAM:
                        /* Should have been set during pattern checking */
                        result = type_error_type(ctx->types);
                        break;
                    case SYM_PROC:
                        /* Return function type */
                        if (sym->decl->kind == DECL_PROC) {
                            ProcDecl *proc = &sym->decl->proc;
                            Vec(Type *) params = vec_new(Type *);
                            for (size_t i = 0; i < vec_len(proc->params); i++) {
                                Type *pt = resolve_type_expr(ctx, proc->params[i].type);
                                vec_push(params, pt);
                            }
                            Type *ret = resolve_type_expr(ctx, proc->return_type);
                            result = type_function(ctx->types, params, ret);
                        } else {
                            result = type_error_type(ctx->types);
                        }
                        break;
                    case SYM_TYPE:
                        /* Type used as value - not allowed */
                        diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, expr->span,
                            "type name used as value");
                        result = type_error_type(ctx->types);
                        break;
                    default:
                        result = type_error_type(ctx->types);
                        break;
                }
            } else {
                result = type_error_type(ctx->types);
            }
            break;
        }

        case EXPR_PATH:
            /* Path to a nested item - for bootstrap, basic handling */
            result = type_error_type(ctx->types);
            break;

        case EXPR_BINARY: {
            Type *left = check_expr(ctx, expr->binary.left, NULL);
            Type *right = check_expr(ctx, expr->binary.right, left);
            result = binary_result_type(ctx, expr->binary.op, left, right);

            /* Check operands are compatible */
            if (expr->binary.op != BINOP_ASSIGN &&
                expr->binary.op < BINOP_ADD_ASSIGN) {
                if (!types_compatible(ctx, left, right)) {
                    error_type_mismatch(ctx, expr->span, left, right);
                }
            }
            break;
        }

        case EXPR_UNARY: {
            Type *operand = check_expr(ctx, expr->unary.operand, NULL);
            result = unary_result_type(ctx, expr->unary.op, operand);
            break;
        }

        case EXPR_CALL: {
            Type *callee = check_expr(ctx, expr->call.callee, NULL);
            if (callee->kind != TYPE_FUNCTION) {
                error_not_callable(ctx, expr->span, callee);
                result = type_error_type(ctx->types);
            } else {
                /* Check argument count */
                if (vec_len(expr->call.args) != vec_len(callee->function.params)) {
                    error_wrong_arg_count(ctx, expr->span,
                                         vec_len(callee->function.params),
                                         vec_len(expr->call.args));
                }
                /* Check argument types */
                for (size_t i = 0; i < vec_len(expr->call.args); i++) {
                    Type *param_type = i < vec_len(callee->function.params)
                        ? callee->function.params[i] : NULL;
                    Type *arg_type = check_expr(ctx, expr->call.args[i], param_type);
                    if (param_type && !types_compatible(ctx, param_type, arg_type)) {
                        error_type_mismatch(ctx, expr->call.args[i]->span,
                                          param_type, arg_type);
                    }
                }
                result = callee->function.return_type;
            }
            break;
        }

        case EXPR_METHOD_CALL: {
            /* Method dispatch using ~> operator */
            Type *receiver = check_expr(ctx, expr->method_call.receiver, NULL);
            ProcDecl *method = NULL;
            Type *method_type = lookup_method(ctx, receiver, expr->method_call.method,
                                             expr->span, &method);

            if (method_type->kind == TYPE_FUNCTION) {
                /* Check argument count */
                if (vec_len(expr->method_call.args) != vec_len(method_type->function.params)) {
                    error_wrong_arg_count(ctx, expr->span,
                                         vec_len(method_type->function.params),
                                         vec_len(expr->method_call.args));
                }
                /* Check argument types */
                for (size_t i = 0; i < vec_len(expr->method_call.args); i++) {
                    Type *param_type = i < vec_len(method_type->function.params)
                        ? method_type->function.params[i] : NULL;
                    Type *arg_type = check_expr(ctx, expr->method_call.args[i], param_type);
                    if (param_type && !types_compatible(ctx, param_type, arg_type)) {
                        error_type_mismatch(ctx, expr->method_call.args[i]->span,
                                          param_type, arg_type);
                    }
                }
                result = method_type->function.return_type;
            } else {
                result = type_error_type(ctx->types);
            }
            break;
        }

        case EXPR_FIELD: {
            Type *object = check_expr(ctx, expr->field.object, NULL);
            result = lookup_field(ctx, object, expr->field.field, expr->span);
            break;
        }

        case EXPR_INDEX: {
            Type *object = check_expr(ctx, expr->index.object, NULL);
            Type *index = check_expr(ctx, expr->index.index, ctx->types->type_usize);

            if (object->kind == TYPE_ARRAY) {
                result = object->array.element;
            } else if (object->kind == TYPE_SLICE) {
                result = object->slice.element;
            } else {
                diag_report(ctx->diag, DIAG_ERROR, E_TYP_1603, expr->span,
                    "cannot index into this type");
                result = type_error_type(ctx->types);
            }

            /* Index should be usize */
            if (!types_compatible(ctx, ctx->types->type_usize, index)) {
                error_type_mismatch(ctx, expr->index.index->span,
                                  ctx->types->type_usize, index);
            }
            break;
        }

        case EXPR_TUPLE: {
            Vec(Type *) elements = vec_new(Type *);
            for (size_t i = 0; i < vec_len(expr->tuple.elements); i++) {
                Type *elem_expected = NULL;
                if (expected && expected->kind == TYPE_TUPLE &&
                    i < vec_len(expected->tuple.elements)) {
                    elem_expected = expected->tuple.elements[i];
                }
                Type *elem = check_expr(ctx, expr->tuple.elements[i], elem_expected);
                vec_push(elements, elem);
            }
            result = type_tuple(ctx->types, elements);
            break;
        }

        case EXPR_ARRAY: {
            Type *elem_type = NULL;
            if (expected && expected->kind == TYPE_ARRAY) {
                elem_type = expected->array.element;
            }

            if (expr->array.repeat_value) {
                /* [value; count] syntax */
                Type *value_type = check_expr(ctx, expr->array.repeat_value, elem_type);
                check_expr(ctx, expr->array.repeat_count, ctx->types->type_usize);

                size_t count = 0;
                if (expr->array.repeat_count->kind == EXPR_INT_LIT) {
                    count = (size_t)expr->array.repeat_count->int_lit.value;
                }
                result = type_array(ctx->types, value_type, count);
            } else {
                /* [elem1, elem2, ...] syntax */
                for (size_t i = 0; i < vec_len(expr->array.elements); i++) {
                    Type *elem = check_expr(ctx, expr->array.elements[i], elem_type);
                    if (i == 0) {
                        elem_type = elem;
                    } else if (!types_compatible(ctx, elem_type, elem)) {
                        error_type_mismatch(ctx, expr->array.elements[i]->span,
                                          elem_type, elem);
                    }
                }
                result = type_array(ctx->types, elem_type ? elem_type : type_error_type(ctx->types),
                                   vec_len(expr->array.elements));
            }
            break;
        }

        case EXPR_RECORD: {
            Type *record_type = resolve_type_expr(ctx, expr->record.type);
            /* Check field types against declaration */
            if (record_type->nominal.sym && record_type->nominal.sym->decl) {
                Decl *decl = record_type->nominal.sym->decl;
                if (decl->kind == DECL_RECORD) {
                    /* Match fields */
                    for (size_t i = 0; i < vec_len(expr->record.field_values); i++) {
                        InternedString field_name = expr->record.field_names[i];
                        Expr *field_value = expr->record.field_values[i];

                        /* Find field in declaration */
                        Type *field_type = NULL;
                        for (size_t j = 0; j < vec_len(decl->record.fields); j++) {
                            if (decl->record.fields[j].name.data == field_name.data) {
                                field_type = resolve_type_expr(ctx, decl->record.fields[j].type);
                                break;
                            }
                        }

                        if (!field_type) {
                            error_no_field(ctx, field_value->span, record_type, field_name);
                        } else {
                            Type *value_type = check_expr(ctx, field_value, field_type);
                            if (!types_compatible(ctx, field_type, value_type)) {
                                error_type_mismatch(ctx, field_value->span,
                                                  field_type, value_type);
                            }
                        }
                    }
                }
            }
            result = record_type;
            break;
        }

        case EXPR_IF: {
            Type *cond_type = check_expr(ctx, expr->if_.condition, ctx->types->type_bool);
            if (!types_compatible(ctx, ctx->types->type_bool, cond_type)) {
                error_type_mismatch(ctx, expr->if_.condition->span,
                                  ctx->types->type_bool, cond_type);
            }

            Type *then_type = check_expr(ctx, expr->if_.then_branch, expected);
            if (expr->if_.else_branch) {
                Type *else_type = check_expr(ctx, expr->if_.else_branch, then_type);
                if (!types_compatible(ctx, then_type, else_type)) {
                    /* Create union type */
                    Vec(Type *) members = vec_new(Type *);
                    vec_push(members, then_type);
                    vec_push(members, else_type);
                    result = type_union(ctx->types, members);
                } else {
                    result = then_type;
                }
            } else {
                result = ctx->types->type_unit;
            }
            break;
        }

        case EXPR_MATCH: {
            Type *scrutinee_type = check_expr(ctx, expr->match.scrutinee, NULL);
            Type *result_type = NULL;

            for (size_t i = 0; i < vec_len(expr->match.arms_patterns); i++) {
                check_pattern(ctx, expr->match.arms_patterns[i], scrutinee_type);
                Type *arm_type = check_expr(ctx, expr->match.arms_bodies[i], result_type);
                if (result_type == NULL) {
                    result_type = arm_type;
                } else if (!types_compatible(ctx, result_type, arm_type)) {
                    /* Union of arm types */
                    Vec(Type *) members = vec_new(Type *);
                    vec_push(members, result_type);
                    vec_push(members, arm_type);
                    result_type = type_union(ctx->types, members);
                }
            }
            result = result_type ? result_type : ctx->types->type_unit;
            break;
        }

        case EXPR_BLOCK: {
            for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
                check_stmt(ctx, expr->block.stmts[i]);
            }
            if (expr->block.result) {
                /* Trailing expression determines block type */
                result = check_expr(ctx, expr->block.result, expected);
            } else {
                /* Check if last statement is result/return (transfers control) */
                size_t num_stmts = vec_len(expr->block.stmts);
                if (num_stmts > 0) {
                    Stmt *last = expr->block.stmts[num_stmts - 1];
                    if (last->kind == STMT_RESULT || last->kind == STMT_RETURN) {
                        /* Block ends with result/return - type matches expected */
                        result = expected ? expected : ctx->current_return_type;
                    } else {
                        result = ctx->types->type_unit;
                    }
                } else {
                    result = ctx->types->type_unit;
                }
            }
            break;
        }

        case EXPR_LOOP: {
            if (expr->loop.binding) {
                /* loop i in range - check range type */
                if (expr->loop.iterable) {
                    Type *iter_type = check_expr(ctx, expr->loop.iterable, NULL);
                    /* For ranges, element type is the range's bound type */
                    /* For arrays/slices, element type is the element */
                    Type *elem_type = iter_type;
                    if (iter_type->kind == TYPE_ARRAY) {
                        elem_type = iter_type->array.element;
                    } else if (iter_type->kind == TYPE_SLICE) {
                        elem_type = iter_type->slice.element;
                    }
                    check_pattern(ctx, expr->loop.binding, elem_type);
                }
            }
            if (expr->loop.condition) {
                Type *cond_type = check_expr(ctx, expr->loop.condition, ctx->types->type_bool);
                if (!types_compatible(ctx, ctx->types->type_bool, cond_type)) {
                    error_type_mismatch(ctx, expr->loop.condition->span,
                                      ctx->types->type_bool, cond_type);
                }
            }
            check_expr(ctx, expr->loop.body, ctx->types->type_unit);
            result = ctx->types->type_unit;
            break;
        }

        case EXPR_MOVE: {
            result = check_expr(ctx, expr->move.operand, expected);
            break;
        }

        case EXPR_WIDEN: {
            Type *operand = check_expr(ctx, expr->widen.operand, NULL);
            /* Widening removes state specificity */
            if (operand->kind == TYPE_MODAL_STATE) {
                result = operand->modal_state.modal_type;
            } else {
                result = operand;
            }
            break;
        }

        case EXPR_CAST: {
            check_expr(ctx, expr->cast.operand, NULL);
            result = resolve_type_expr(ctx, expr->cast.target_type);
            break;
        }

        case EXPR_RANGE: {
            Type *start_type = NULL;
            Type *end_type = NULL;
            if (expr->range.start) {
                start_type = check_expr(ctx, expr->range.start, NULL);
            }
            if (expr->range.end) {
                end_type = check_expr(ctx, expr->range.end, start_type);
            }
            /* Range type - for bootstrap, treat as the element type */
            result = start_type ? start_type : (end_type ? end_type : ctx->types->type_i32);
            break;
        }

        case EXPR_STATIC_CALL: {
            Type *type = resolve_type_expr(ctx, expr->static_call.type);
            ProcDecl *method = NULL;
            Type *method_type = lookup_method(ctx, type, expr->static_call.method,
                                             expr->span, &method);

            if (method_type->kind == TYPE_FUNCTION) {
                /* Check arguments */
                for (size_t i = 0; i < vec_len(expr->static_call.args); i++) {
                    Type *param_type = i < vec_len(method_type->function.params)
                        ? method_type->function.params[i] : NULL;
                    Type *arg_type = check_expr(ctx, expr->static_call.args[i], param_type);
                    if (param_type && !types_compatible(ctx, param_type, arg_type)) {
                        error_type_mismatch(ctx, expr->static_call.args[i]->span,
                                          param_type, arg_type);
                    }
                }
                result = method_type->function.return_type;
            } else {
                result = type_error_type(ctx->types);
            }
            break;
        }

        case EXPR_REGION_ALLOC: {
            Type *value_type = check_expr(ctx, expr->region_alloc.value, NULL);
            result = type_ptr(ctx->types, value_type, TYPE_PTR_VALID);
            break;
        }

        case EXPR_ADDR_OF: {
            Type *operand = check_expr(ctx, expr->addr_of.operand, NULL);
            result = type_ptr(ctx->types, operand, TYPE_PTR_VALID);
            if (expr->addr_of.is_unique) {
                result = type_with_permission(ctx->types, result, PERM_UNIQUE);
            }
            break;
        }

        case EXPR_DEREF: {
            Type *operand = check_expr(ctx, expr->deref.operand, NULL);
            result = unary_result_type(ctx, UNOP_DEREF, operand);
            break;
        }

        case EXPR_CLOSURE: {
            /* Check closure parameters */
            Vec(Type *) params = vec_new(Type *);
            for (size_t i = 0; i < vec_len(expr->closure.params); i++) {
                Type *param_type = check_pattern(ctx, expr->closure.params[i], NULL);
                vec_push(params, param_type ? param_type : type_error_type(ctx->types));
            }

            /* Check return type */
            Type *ret_type = NULL;
            if (expr->closure.return_type) {
                ret_type = resolve_type_expr(ctx, expr->closure.return_type);
            }

            /* Check body */
            Type *body_type = check_expr(ctx, expr->closure.body, ret_type);
            if (!ret_type) ret_type = body_type;

            result = type_function(ctx->types, params, ret_type);
            break;
        }
    }

    /* Store resolved type in AST */
    /* expr->resolved_type = result; - would need to convert Type* back to TypeExpr* */

    /* Check against expected type */
    if (expected && result && !types_compatible(ctx, expected, result)) {
        /* Don't double-report if we already have an error type */
        if (result->kind != TYPE_ERROR && expected->kind != TYPE_ERROR) {
            error_type_mismatch(ctx, expr->span, expected, result);
        }
    }

    return result ? result : type_error_type(ctx->types);
}

/*
 * Check a statement
 */
static void check_stmt(TypeCheckContext *ctx, Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {
        case STMT_EXPR:
            check_expr(ctx, stmt->expr.expr, NULL);
            break;

        case STMT_LET: {
            Type *type_annot = NULL;
            if (stmt->let.type) {
                type_annot = resolve_type_expr(ctx, stmt->let.type);
            }

            Type *init_type = NULL;
            if (stmt->let.init) {
                init_type = check_expr(ctx, stmt->let.init, type_annot);
            }

            Type *binding_type = type_annot ? type_annot : init_type;
            check_pattern(ctx, stmt->let.pattern, binding_type);
            break;
        }

        case STMT_VAR: {
            Type *type_annot = NULL;
            if (stmt->var.type) {
                type_annot = resolve_type_expr(ctx, stmt->var.type);
            }

            Type *init_type = NULL;
            if (stmt->var.init) {
                init_type = check_expr(ctx, stmt->var.init, type_annot);
            }

            Type *binding_type = type_annot ? type_annot : init_type;
            check_pattern(ctx, stmt->var.pattern, binding_type);
            break;
        }

        case STMT_ASSIGN: {
            Type *target_type = check_expr(ctx, stmt->assign.target, NULL);
            Type *value_type = check_expr(ctx, stmt->assign.value, target_type);
            if (!types_compatible(ctx, target_type, value_type)) {
                error_type_mismatch(ctx, stmt->assign.value->span, target_type, value_type);
            }
            break;
        }

        case STMT_RETURN:
            if (stmt->return_.value) {
                check_expr(ctx, stmt->return_.value, ctx->current_return_type);
            }
            break;

        case STMT_RESULT:
            check_expr(ctx, stmt->result.value, ctx->current_return_type);
            break;

        case STMT_BREAK:
            if (stmt->break_.value) {
                check_expr(ctx, stmt->break_.value, NULL);
            }
            break;

        case STMT_CONTINUE:
            /* No value to check */
            break;

        case STMT_DEFER:
            check_expr(ctx, stmt->defer.body, ctx->types->type_unit);
            break;

        case STMT_UNSAFE:
            check_expr(ctx, stmt->unsafe.body, NULL);
            break;
    }
}

/*
 * Check a procedure declaration
 */
static void check_proc_decl(TypeCheckContext *ctx, ProcDecl *proc) {
    ctx->current_proc = proc;
    ctx->current_return_type = resolve_type_expr(ctx, proc->return_type);

    /* Check contracts */
    for (size_t i = 0; i < vec_len(proc->contracts); i++) {
        check_expr(ctx, proc->contracts[i].condition, ctx->types->type_bool);
    }

    /* Check body */
    if (proc->body) {
        Type *body_type = check_expr(ctx, proc->body, ctx->current_return_type);
        /* Note: Don't require body_type to match return_type
         * Procedure bodies may exit via result/return statements which have already
         * been type-checked against current_return_type in check_stmt.
         * The block itself might have type unit if it ends with a control flow
         * expression (if/match) that has result statements in all branches. */
        if (body_type != ctx->types->type_unit &&
            !types_compatible(ctx, ctx->current_return_type, body_type)) {
            error_type_mismatch(ctx, proc->body->span, ctx->current_return_type, body_type);
        }
    }

    ctx->current_proc = NULL;
    ctx->current_return_type = NULL;
}

/*
 * Check a declaration
 */
static void check_decl(TypeCheckContext *ctx, Decl *decl) {
    ctx->current_type_decl = decl;

    switch (decl->kind) {
        case DECL_PROC:
            check_proc_decl(ctx, &decl->proc);
            break;

        case DECL_RECORD:
            /* Check field default values */
            for (size_t i = 0; i < vec_len(decl->record.fields); i++) {
                FieldDecl *field = &decl->record.fields[i];
                if (field->default_value) {
                    Type *field_type = resolve_type_expr(ctx, field->type);
                    check_expr(ctx, field->default_value, field_type);
                }
            }
            /* Check methods */
            for (size_t i = 0; i < vec_len(decl->record.methods); i++) {
                check_proc_decl(ctx, &decl->record.methods[i]);
            }
            break;

        case DECL_ENUM:
            /* Check variant discriminants */
            for (size_t i = 0; i < vec_len(decl->enum_.variants); i++) {
                EnumVariant *var = &decl->enum_.variants[i];
                if (var->discriminant) {
                    check_expr(ctx, var->discriminant, ctx->types->type_i32);
                }
            }
            /* Check methods */
            for (size_t i = 0; i < vec_len(decl->enum_.methods); i++) {
                check_proc_decl(ctx, &decl->enum_.methods[i]);
            }
            break;

        case DECL_MODAL:
            /* Check state field defaults */
            for (size_t s = 0; s < vec_len(decl->modal.states); s++) {
                ModalState *state = &decl->modal.states[s];
                for (size_t i = 0; i < vec_len(state->fields); i++) {
                    FieldDecl *field = &state->fields[i];
                    if (field->default_value) {
                        Type *field_type = resolve_type_expr(ctx, field->type);
                        check_expr(ctx, field->default_value, field_type);
                    }
                }
                /* Check state methods */
                for (size_t i = 0; i < vec_len(state->methods); i++) {
                    check_proc_decl(ctx, &state->methods[i]);
                }
                /* Check transitions */
                for (size_t i = 0; i < vec_len(state->transitions); i++) {
                    Transition *trans = &state->transitions[i];
                    if (trans->body) {
                        check_expr(ctx, trans->body, NULL);
                    }
                }
            }
            /* Check shared methods */
            for (size_t i = 0; i < vec_len(decl->modal.shared_methods); i++) {
                check_proc_decl(ctx, &decl->modal.shared_methods[i]);
            }
            break;

        case DECL_TYPE_ALIAS:
            /* Nothing to check beyond what resolve did */
            break;

        case DECL_CLASS:
            /* Check method signatures */
            for (size_t i = 0; i < vec_len(decl->class_.methods); i++) {
                check_proc_decl(ctx, &decl->class_.methods[i]);
            }
            /* Check default methods */
            for (size_t i = 0; i < vec_len(decl->class_.default_methods); i++) {
                check_proc_decl(ctx, &decl->class_.default_methods[i]);
            }
            break;

        case DECL_EXTERN:
            /* Extern signatures were validated during resolution */
            break;

        case DECL_IMPORT:
        case DECL_USE:
        case DECL_MODULE:
            /* Nothing to type check */
            break;
    }

    ctx->current_type_decl = NULL;
}

/*
 * Main entry point for type checking
 */
bool sema_check_types(SemaContext *ctx, Module *mod) {
    TypeCheckContext tctx;
    memset(&tctx, 0, sizeof(tctx));
    tctx.sema = ctx;
    tctx.arena = ctx->arena;
    tctx.diag = ctx->diag;
    tctx.types = &ctx->type_ctx;
    tctx.strings = ctx->strings;
    tctx.scope = ctx->current_scope;

    /* Check all declarations */
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        check_decl(&tctx, mod->decls[i]);
    }

    return !diag_has_errors(ctx->diag);
}
