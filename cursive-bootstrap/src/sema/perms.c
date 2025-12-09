/*
 * Cursive Bootstrap Compiler - Permission Checking
 *
 * Implements permission system verification.
 * Bootstrap compiler only supports const and unique permissions:
 * - const (default): Read-only access, unlimited aliases
 * - unique: Exclusive read-write, no aliases
 *
 * Key checks:
 * - E-TYP-1601: Mutation through const path
 * - E-TYP-1602: Aliasing of unique path
 *
 * Method receiver compatibility:
 * - ~ (const) methods can be called on any receiver
 * - ~! (unique) methods require unique access
 */

#include "sema.h"
#include <string.h>

/*
 * Permission context for tracking access paths
 */
typedef struct PermContext {
    SemaContext *sema;
    Arena *arena;
    DiagContext *diag;

    /* Current expected permission for receiver */
    Permission receiver_perm;

    /* Are we in an unsafe block? */
    bool in_unsafe;

    /* Unique paths currently borrowed (for aliasing detection) */
    Vec(Expr *) borrowed_unique_paths;
} PermContext;

/* Forward declarations */
static void check_expr(PermContext *ctx, Expr *expr, Permission required);
static void check_stmt(PermContext *ctx, Stmt *stmt);
static Permission get_expr_permission(PermContext *ctx, Expr *expr);
static bool paths_may_alias(Expr *a, Expr *b);
static Symbol *perm_scope_lookup(SemaContext *sema, InternedString name);

/*
 * Initialize permission context
 */
static void perm_ctx_init(PermContext *ctx, SemaContext *sema) {
    memset(ctx, 0, sizeof(PermContext));
    ctx->sema = sema;
    ctx->arena = sema->arena;
    ctx->diag = sema->diag;
    ctx->receiver_perm = PERM_CONST;
    ctx->in_unsafe = false;
    ctx->borrowed_unique_paths = NULL;  /* Vec starts as NULL */
}

/*
 * Wrapper for scope_lookup using SemaContext
 */
static Symbol *perm_scope_lookup(SemaContext *sema, InternedString name) {
    return scope_lookup_from(sema->current_scope, name);
}

/*
 * Get the permission of an expression
 */
static Permission get_expr_permission(PermContext *ctx, Expr *expr) {
    if (!expr) return PERM_CONST;

    switch (expr->kind) {
        case EXPR_IDENT: {
            Symbol *sym = perm_scope_lookup(ctx->sema, expr->ident.name);
            if (sym && sym->type) {
                return sym->type->perm;
            }
            return PERM_CONST;
        }

        case EXPR_FIELD: {
            /* Field access inherits permission from parent */
            Permission parent_perm = get_expr_permission(ctx, expr->field.object);
            /* Can't have more permission than parent */
            return parent_perm;
        }

        case EXPR_INDEX: {
            /* Index access inherits permission from parent */
            return get_expr_permission(ctx, expr->index.object);
        }

        case EXPR_DEREF: {
            /* Dereference gets permission from pointer type */
            /* For now, assume same as operand */
            return get_expr_permission(ctx, expr->deref.operand);
        }

        case EXPR_ADDR_OF: {
            /* Address-of creates a reference with specified permission */
            if (expr->addr_of.is_unique) {
                return PERM_UNIQUE;
            }
            return PERM_CONST;
        }

        default:
            return PERM_CONST;
    }
}

/*
 * Check if two expressions may alias (conservatively)
 */
static bool paths_may_alias(Expr *a, Expr *b) {
    if (!a || !b) return false;

    /* Same identifier? */
    if (a->kind == EXPR_IDENT && b->kind == EXPR_IDENT) {
        return a->ident.name.data == b->ident.name.data;
    }

    /* Field of same base? */
    if (a->kind == EXPR_FIELD && b->kind == EXPR_FIELD) {
        if (paths_may_alias(a->field.object, b->field.object)) {
            /* Same base - might alias if same field or nested */
            return true;
        }
    }

    /* Index of same base? */
    if (a->kind == EXPR_INDEX && b->kind == EXPR_INDEX) {
        if (paths_may_alias(a->index.object, b->index.object)) {
            /* Conservative: any index into same array might alias */
            return true;
        }
    }

    /* One is prefix of other? (e.g., a.b vs a.b.c) */
    if (a->kind == EXPR_FIELD && paths_may_alias(a->field.object, b)) {
        return true;
    }
    if (b->kind == EXPR_FIELD && paths_may_alias(a, b->field.object)) {
        return true;
    }

    return false;
}

/*
 * Check if taking a unique borrow would conflict with existing borrows
 */
static bool check_unique_borrow(PermContext *ctx, Expr *expr) {
    for (size_t i = 0; i < vec_len(ctx->borrowed_unique_paths); i++) {
        Expr *borrowed = ctx->borrowed_unique_paths[i];
        if (paths_may_alias(expr, borrowed)) {
            diag_report(ctx->diag, DIAG_ERROR, E_TYP_1602, expr->span,
                "cannot create unique reference while path is already borrowed");
            return false;
        }
    }
    return true;
}

/*
 * Check mutation through a path
 */
static void check_mutation(PermContext *ctx, Expr *target) {
    Permission perm = get_expr_permission(ctx, target);

    if (perm == PERM_CONST) {
        diag_report(ctx->diag, DIAG_ERROR, E_TYP_1601, target->span,
            "cannot mutate through const path");
        return;
    }

    /* unique permission allows mutation */
}

/*
 * Check a binary expression
 */
static void check_binary(PermContext *ctx, Expr *expr) {
    /* Check for assignment operators */
    if (expr->binary.op >= BINOP_ASSIGN && expr->binary.op <= BINOP_SHR_ASSIGN) {
        /* LHS must be mutable (unique permission) */
        check_mutation(ctx, expr->binary.left);
        /* RHS just needs to be readable */
        check_expr(ctx, expr->binary.right, PERM_CONST);
        return;
    }

    /* Regular binary - both sides just need to be readable */
    check_expr(ctx, expr->binary.left, PERM_CONST);
    check_expr(ctx, expr->binary.right, PERM_CONST);
}

/*
 * Check a method call expression
 */
static void check_method_call(PermContext *ctx, Expr *expr) {
    /* Check receiver permission against method receiver kind */
    /* For now we don't have access to the resolved method to check receiver kind */
    /* This would be done after type resolution fills in method info */

    Permission receiver_perm = get_expr_permission(ctx, expr->method_call.receiver);

    /* Check receiver is valid */
    check_expr(ctx, expr->method_call.receiver, PERM_CONST);

    /* Check arguments */
    for (size_t i = 0; i < vec_len(expr->method_call.args); i++) {
        check_expr(ctx, expr->method_call.args[i], PERM_CONST);
    }
}

/*
 * Check an address-of expression
 */
static void check_addr_of(PermContext *ctx, Expr *expr) {
    if (expr->addr_of.is_unique) {
        /* Taking unique reference - check for aliasing conflicts */
        if (!check_unique_borrow(ctx, expr->addr_of.operand)) {
            return;
        }

        /* The operand must be a mutable path */
        Permission operand_perm = get_expr_permission(ctx, expr->addr_of.operand);
        if (operand_perm == PERM_CONST) {
            diag_report(ctx->diag, DIAG_ERROR, E_TYP_1602, expr->span,
                "cannot take unique reference (&!) to const path");
            return;
        }

        /* Track this unique borrow */
        vec_push(ctx->borrowed_unique_paths, expr->addr_of.operand);
    } else {
        /* Taking const reference - just check operand is valid */
        check_expr(ctx, expr->addr_of.operand, PERM_CONST);
    }
}

/*
 * Check an if expression
 */
static void check_if(PermContext *ctx, Expr *expr) {
    /* Check condition */
    check_expr(ctx, expr->if_.condition, PERM_CONST);

    /* Save borrowed paths */
    size_t saved_len = vec_len(ctx->borrowed_unique_paths);

    /* Check then branch */
    check_expr(ctx, expr->if_.then_branch, PERM_CONST);

    /* Restore for else branch */
    while (vec_len(ctx->borrowed_unique_paths) > saved_len) {
        (void)vec_pop(ctx->borrowed_unique_paths);
    }

    /* Check else branch if present */
    if (expr->if_.else_branch) {
        check_expr(ctx, expr->if_.else_branch, PERM_CONST);
    }
}

/*
 * Check a match expression
 */
static void check_match(PermContext *ctx, Expr *expr) {
    /* Check scrutinee */
    check_expr(ctx, expr->match.scrutinee, PERM_CONST);

    size_t saved_len = vec_len(ctx->borrowed_unique_paths);

    /* Check each arm */
    for (size_t i = 0; i < vec_len(expr->match.arms_bodies); i++) {
        check_expr(ctx, expr->match.arms_bodies[i], PERM_CONST);

        /* Restore borrowed paths for next arm */
        while (vec_len(ctx->borrowed_unique_paths) > saved_len) {
            (void)vec_pop(ctx->borrowed_unique_paths);
        }
    }
}

/*
 * Check a block expression
 */
static void check_block(PermContext *ctx, Expr *expr) {
    size_t saved_len = vec_len(ctx->borrowed_unique_paths);

    /* Check statements */
    for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
        check_stmt(ctx, expr->block.stmts[i]);
    }

    /* Check result expression if present */
    if (expr->block.result) {
        check_expr(ctx, expr->block.result, PERM_CONST);
    }

    /* Borrows end at scope exit */
    while (vec_len(ctx->borrowed_unique_paths) > saved_len) {
        (void)vec_pop(ctx->borrowed_unique_paths);
    }
}

/*
 * Check a loop expression
 */
static void check_loop(PermContext *ctx, Expr *expr) {
    size_t saved_len = vec_len(ctx->borrowed_unique_paths);

    /* Check iterable */
    if (expr->loop.iterable) {
        check_expr(ctx, expr->loop.iterable, PERM_CONST);
    }

    /* Check condition */
    if (expr->loop.condition) {
        check_expr(ctx, expr->loop.condition, PERM_CONST);
    }

    /* Check body */
    if (expr->loop.body) {
        check_expr(ctx, expr->loop.body, PERM_CONST);
    }

    /* Borrows end at scope exit */
    while (vec_len(ctx->borrowed_unique_paths) > saved_len) {
        (void)vec_pop(ctx->borrowed_unique_paths);
    }
}

/*
 * Check an expression
 */
static void check_expr(PermContext *ctx, Expr *expr, Permission required) {
    if (!expr) return;

    switch (expr->kind) {
        case EXPR_INT_LIT:
        case EXPR_FLOAT_LIT:
        case EXPR_STRING_LIT:
        case EXPR_CHAR_LIT:
        case EXPR_BOOL_LIT:
            /* Literals are always readable */
            break;

        case EXPR_IDENT:
            /* Identifier access is valid if permission is sufficient */
            /* Required permission is at least what we need */
            break;

        case EXPR_PATH:
            /* Path expressions are always readable */
            break;

        case EXPR_BINARY:
            check_binary(ctx, expr);
            break;

        case EXPR_UNARY:
            check_expr(ctx, expr->unary.operand, PERM_CONST);
            break;

        case EXPR_CALL:
            check_expr(ctx, expr->call.callee, PERM_CONST);
            for (size_t i = 0; i < vec_len(expr->call.args); i++) {
                check_expr(ctx, expr->call.args[i], PERM_CONST);
            }
            break;

        case EXPR_METHOD_CALL:
            check_method_call(ctx, expr);
            break;

        case EXPR_FIELD:
            check_expr(ctx, expr->field.object, required);
            break;

        case EXPR_INDEX:
            check_expr(ctx, expr->index.object, required);
            check_expr(ctx, expr->index.index, PERM_CONST);
            break;

        case EXPR_TUPLE:
            for (size_t i = 0; i < vec_len(expr->tuple.elements); i++) {
                check_expr(ctx, expr->tuple.elements[i], required);
            }
            break;

        case EXPR_ARRAY:
            if (expr->array.repeat_value) {
                check_expr(ctx, expr->array.repeat_value, PERM_CONST);
                check_expr(ctx, expr->array.repeat_count, PERM_CONST);
            } else {
                for (size_t i = 0; i < vec_len(expr->array.elements); i++) {
                    check_expr(ctx, expr->array.elements[i], PERM_CONST);
                }
            }
            break;

        case EXPR_RECORD:
            for (size_t i = 0; i < vec_len(expr->record.field_values); i++) {
                check_expr(ctx, expr->record.field_values[i], PERM_CONST);
            }
            break;

        case EXPR_IF:
            check_if(ctx, expr);
            break;

        case EXPR_MATCH:
            check_match(ctx, expr);
            break;

        case EXPR_BLOCK:
            check_block(ctx, expr);
            break;

        case EXPR_LOOP:
            check_loop(ctx, expr);
            break;

        case EXPR_MOVE:
            check_expr(ctx, expr->move.operand, PERM_CONST);
            break;

        case EXPR_WIDEN:
            check_expr(ctx, expr->widen.operand, required);
            break;

        case EXPR_CAST:
            check_expr(ctx, expr->cast.operand, PERM_CONST);
            break;

        case EXPR_RANGE:
            if (expr->range.start) check_expr(ctx, expr->range.start, PERM_CONST);
            if (expr->range.end) check_expr(ctx, expr->range.end, PERM_CONST);
            break;

        case EXPR_STATIC_CALL:
            for (size_t i = 0; i < vec_len(expr->static_call.args); i++) {
                check_expr(ctx, expr->static_call.args[i], PERM_CONST);
            }
            break;

        case EXPR_REGION_ALLOC:
            check_expr(ctx, expr->region_alloc.value, PERM_CONST);
            break;

        case EXPR_ADDR_OF:
            check_addr_of(ctx, expr);
            break;

        case EXPR_DEREF:
            check_expr(ctx, expr->deref.operand, required);
            break;

        case EXPR_CLOSURE:
            /* Check closure body */
            check_expr(ctx, expr->closure.body, PERM_CONST);
            break;
    }
}

/*
 * Check a statement
 */
static void check_stmt(PermContext *ctx, Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {
        case STMT_EXPR:
            check_expr(ctx, stmt->expr.expr, PERM_CONST);
            break;

        case STMT_LET:
            if (stmt->let.init) {
                check_expr(ctx, stmt->let.init, PERM_CONST);
            }
            break;

        case STMT_VAR:
            if (stmt->var.init) {
                check_expr(ctx, stmt->var.init, PERM_CONST);
            }
            break;

        case STMT_ASSIGN:
            check_mutation(ctx, stmt->assign.target);
            check_expr(ctx, stmt->assign.value, PERM_CONST);
            break;

        case STMT_RETURN:
            if (stmt->return_.value) {
                check_expr(ctx, stmt->return_.value, PERM_CONST);
            }
            break;

        case STMT_RESULT:
            if (stmt->result.value) {
                check_expr(ctx, stmt->result.value, PERM_CONST);
            }
            break;

        case STMT_BREAK:
            if (stmt->break_.value) {
                check_expr(ctx, stmt->break_.value, PERM_CONST);
            }
            break;

        case STMT_CONTINUE:
            /* No expression to check */
            break;

        case STMT_DEFER:
            check_expr(ctx, stmt->defer.body, PERM_CONST);
            break;

        case STMT_UNSAFE:
            /* Inside unsafe block, some permission checks are relaxed */
            ctx->in_unsafe = true;
            check_expr(ctx, stmt->unsafe.body, PERM_CONST);
            ctx->in_unsafe = false;
            break;
    }
}

/*
 * Check a procedure
 */
static void check_proc(PermContext *ctx, ProcDecl *proc) {
    /* Set receiver permission based on receiver kind */
    switch (proc->receiver) {
        case RECV_NONE:
        case RECV_CONST:
            ctx->receiver_perm = PERM_CONST;
            break;
        case RECV_UNIQUE:
            ctx->receiver_perm = PERM_UNIQUE;
            break;
        case RECV_SHARED:
            /* shared is deferred in bootstrap */
            ctx->receiver_perm = PERM_CONST;
            break;
    }

    /* Clear borrowed paths for this procedure */
    while (vec_len(ctx->borrowed_unique_paths) > 0) {
        (void)vec_pop(ctx->borrowed_unique_paths);
    }

    /* Check body */
    if (proc->body) {
        check_expr(ctx, proc->body, PERM_CONST);
    }
}

/*
 * Check a declaration
 */
static void check_decl(PermContext *ctx, Decl *decl) {
    if (!decl) return;

    switch (decl->kind) {
        case DECL_PROC:
            check_proc(ctx, &decl->proc);
            break;

        case DECL_RECORD:
            /* Check methods */
            for (size_t i = 0; i < vec_len(decl->record.methods); i++) {
                check_proc(ctx, &decl->record.methods[i]);
            }
            break;

        case DECL_ENUM:
            for (size_t i = 0; i < vec_len(decl->enum_.methods); i++) {
                check_proc(ctx, &decl->enum_.methods[i]);
            }
            break;

        case DECL_MODAL:
            /* Check shared methods */
            for (size_t i = 0; i < vec_len(decl->modal.shared_methods); i++) {
                check_proc(ctx, &decl->modal.shared_methods[i]);
            }
            /* Check state-specific methods */
            for (size_t i = 0; i < vec_len(decl->modal.states); i++) {
                ModalState *state = &decl->modal.states[i];
                for (size_t j = 0; j < vec_len(state->methods); j++) {
                    check_proc(ctx, &state->methods[j]);
                }
                /* Check transitions */
                for (size_t j = 0; j < vec_len(state->transitions); j++) {
                    Transition *trans = &state->transitions[j];
                    if (trans->body) {
                        /* Transitions typically take ~! receiver */
                        ctx->receiver_perm = PERM_UNIQUE;
                        check_expr(ctx, trans->body, PERM_CONST);
                    }
                }
            }
            break;

        case DECL_CLASS:
            /* Check default methods */
            for (size_t i = 0; i < vec_len(decl->class_.default_methods); i++) {
                check_proc(ctx, &decl->class_.default_methods[i]);
            }
            break;

        case DECL_TYPE_ALIAS:
        case DECL_EXTERN:
        case DECL_MODULE:
        case DECL_IMPORT:
        case DECL_USE:
            /* No permission checking needed */
            break;
    }
}

/*
 * Main entry point: check permissions in a module
 */
bool sema_check_permissions(SemaContext *ctx, Module *mod) {
    PermContext pctx;
    perm_ctx_init(&pctx, ctx);

    /* Check all declarations */
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        check_decl(&pctx, mod->decls[i]);
    }

    return !diag_has_errors(ctx->diag);
}
