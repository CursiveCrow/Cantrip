/*
 * Cursive Bootstrap Compiler - Move Analysis
 *
 * Implements move semantics and binding state analysis.
 * Tracks binding states through control flow:
 * - UNINITIALIZED: Not yet assigned a value
 * - VALID: Has a valid value
 * - MOVED: Value has been moved out
 * - PARTIALLY_MOVED: Some fields have been moved (records)
 *
 * Per the Cursive spec:
 * - `let x = v` / `var x = v`: Movable bindings (= operator)
 * - `let x := v` / `var x := v`: Immovable bindings (:= operator)
 * - `move expr`: Explicit move, transfers responsibility
 */

#include "sema.h"
#include <string.h>

/*
 * ============================================
 * Binding State
 * ============================================
 */

typedef enum BindingState {
    BIND_STATE_UNINITIALIZED,     /* Never assigned */
    BIND_STATE_VALID,             /* Has valid value */
    BIND_STATE_MOVED,             /* Moved out */
    BIND_STATE_PARTIALLY_MOVED    /* Some fields moved (records) */
} BindingState;

/*
 * Binding info tracked during analysis
 */
typedef struct BindingInfo {
    Symbol *sym;               /* The binding symbol */
    BindingState state;        /* Current state */
    bool is_movable;           /* = vs := */
    bool is_mutable;           /* var vs let */
    Permission perm;           /* Permission */
    SourceSpan last_use_span;  /* Location of last use (for diagnostics) */
    Vec(InternedString) moved_fields; /* For partial moves */
} BindingInfo;

/*
 * Move analysis context
 */
typedef struct MoveContext {
    SemaContext *sema;
    Arena *arena;
    DiagContext *diag;

    /* Current binding state map: Symbol* -> BindingInfo* */
    PtrMap bindings;

    /* Stack of scopes for drop tracking (simplified - just track count) */
    size_t scope_depth;

    /* Current procedure return type (for result checking) */
    Type *return_type;

    /* Current procedure's scope (for symbol lookup) */
    Scope *proc_scope;

    /* Are we in a loop? (for break/continue analysis) */
    int loop_depth;

    /* Deferred expressions to run at scope exit */
    Vec(Expr *) defers;
} MoveContext;

/* Forward declarations */
static void analyze_expr(MoveContext *ctx, Expr *expr, bool is_move_context);
static void analyze_stmt(MoveContext *ctx, Stmt *stmt);
static void analyze_pattern(MoveContext *ctx, Pattern *pat, bool creates_binding);
static BindingInfo *get_binding_info(MoveContext *ctx, Symbol *sym);
static void set_binding_state(MoveContext *ctx, Symbol *sym, BindingState state, SourceSpan span);
static Symbol *move_scope_lookup(MoveContext *ctx, InternedString name);

/*
 * Check if a TypeExpr represents a Copy type.
 * Primitives (integers, floats, bool, char) are always Copy.
 * References are Copy. Pointers depend on state.
 */
static bool type_expr_is_copy(TypeExpr *texpr) {
    if (!texpr) return false;

    switch (texpr->kind) {
        case TEXPR_PRIMITIVE:
            /* All primitives are Copy */
            return true;

        case TEXPR_TUPLE: {
            /* Tuples are Copy if all elements are Copy */
            for (size_t i = 0; i < vec_len(texpr->tuple.elements); i++) {
                if (!type_expr_is_copy(texpr->tuple.elements[i])) {
                    return false;
                }
            }
            return true;
        }

        case TEXPR_REF:
            /* References are Copy */
            return true;

        case TEXPR_PTR:
            /* Raw pointers are Copy (but Ptr<T> modal is not by default) */
            return true;

        case TEXPR_ARRAY:
            /* Arrays of Copy types are Copy */
            return type_expr_is_copy(texpr->array.element);

        case TEXPR_NAMED:
        case TEXPR_GENERIC:
        case TEXPR_MODAL_STATE:
        case TEXPR_SLICE:
        case TEXPR_FUNCTION:
        case TEXPR_UNION:
        case TEXPR_NEVER:
        case TEXPR_UNIT:
        case TEXPR_INFER:
            /* These need more context to determine - assume not Copy */
            /* Named types could be records (not Copy) or type aliases */
            return false;

        default:
            return false;
    }
}

/*
 * Initialize move context
 */
static void move_ctx_init(MoveContext *ctx, SemaContext *sema) {
    memset(ctx, 0, sizeof(MoveContext));
    ctx->sema = sema;
    ctx->arena = sema->arena;
    ctx->diag = sema->diag;
    ptr_map_init(&ctx->bindings);
    ctx->defers = NULL;  /* Vec starts as NULL */
    ctx->loop_depth = 0;
    ctx->scope_depth = 0;
}

/*
 * Create binding info for a symbol
 */
static BindingInfo *create_binding_info(MoveContext *ctx, Symbol *sym, bool initialized) {
    BindingInfo *info = arena_alloc_aligned(ctx->arena, sizeof(BindingInfo), _Alignof(BindingInfo));
    memset(info, 0, sizeof(BindingInfo));
    info->sym = sym;
    info->state = initialized ? BIND_STATE_VALID : BIND_STATE_UNINITIALIZED;
    info->is_movable = (sym->binding_op == BIND_MOVABLE);
    info->is_mutable = sym->is_mutable;
    info->perm = sym->type ? sym->type->perm : PERM_CONST;
    info->moved_fields = NULL;  /* Vec starts as NULL */

    /* Add to bindings map */
    ptr_map_set(&ctx->bindings, sym, info);

    return info;
}

/*
 * Get binding info for a symbol
 */
static BindingInfo *get_binding_info(MoveContext *ctx, Symbol *sym) {
    return (BindingInfo *)ptr_map_get(&ctx->bindings, sym);
}

/*
 * Set binding state
 */
static void set_binding_state(MoveContext *ctx, Symbol *sym, BindingState state, SourceSpan span) {
    BindingInfo *info = get_binding_info(ctx, sym);
    if (info) {
        info->state = state;
        info->last_use_span = span;
    }
}

/*
 * Enter a new scope
 */
static void enter_scope(MoveContext *ctx) {
    ctx->scope_depth++;
}

/*
 * Exit scope - check for drops (simplified)
 */
static void exit_scope(MoveContext *ctx) {
    if (ctx->scope_depth == 0) return;
    ctx->scope_depth--;
    /* Drop tracking is simplified - actual drop insertion in codegen */
}

/*
 * Register a binding in current scope (for drop tracking)
 * Note: Simplified - actual tracking deferred to codegen
 */
static void register_binding(MoveContext *ctx, Symbol *sym) {
    (void)ctx;
    (void)sym;
    /* Drop tracking simplified - codegen handles drop insertion */
}

/*
 * Check if binding is in valid state for use
 */
static bool check_binding_valid(MoveContext *ctx, Symbol *sym, SourceSpan span) {
    BindingInfo *info = get_binding_info(ctx, sym);
    if (!info) {
        /* Not a local binding - parameter or global */
        return true;
    }

    switch (info->state) {
        case BIND_STATE_VALID:
            return true;

        case BIND_STATE_UNINITIALIZED:
            diag_report(ctx->diag, DIAG_ERROR, E_MEM_3007, span,
                "use of uninitialized binding '%s'",
                info->sym->name.data);
            return false;

        case BIND_STATE_MOVED:
            diag_report(ctx->diag, DIAG_ERROR, E_MEM_3001, span,
                "use of moved binding '%s'",
                info->sym->name.data);
            diag_report(ctx->diag, DIAG_NOTE, NULL, info->last_use_span,
                "value was moved here");
            return false;

        case BIND_STATE_PARTIALLY_MOVED:
            diag_report(ctx->diag, DIAG_ERROR, E_MEM_3001, span,
                "use of partially moved binding '%s'",
                info->sym->name.data);
            return false;
    }

    return true;
}

/*
 * Analyze an identifier expression
 */
static void analyze_ident(MoveContext *ctx, Expr *expr, bool is_move_context) {
    /* Use resolved symbol from name resolution instead of looking up */
    Symbol *sym = expr->ident.resolved;
    if (!sym) return;  /* Name resolution should have caught this */

    if (sym->kind != SYM_VAR && sym->kind != SYM_PARAM) {
        return;  /* Not a binding */
    }

    /* Check if binding is valid for use */
    if (!check_binding_valid(ctx, sym, expr->span)) {
        return;
    }

    /* If this is in a move context and the type is not Copy, mark as moved */
    if (is_move_context) {
        BindingInfo *info = get_binding_info(ctx, sym);
        if (info) {
            /* Check if type is Copy - use expression's resolved type */
            bool is_copy = false;
            if (expr->resolved_type) {
                is_copy = type_expr_is_copy(expr->resolved_type);
            } else if (sym->type) {
                is_copy = type_is_copy(sym->type);
            }

            if (!is_copy) {
                /* Check if binding is movable */
                if (!info->is_movable) {
                    diag_report(ctx->diag, DIAG_ERROR, E_MEM_3006, expr->span,
                        "cannot move from immovable binding '%s' (uses := operator)",
                        sym->name.data);
                    return;
                }

                set_binding_state(ctx, sym, BIND_STATE_MOVED, expr->span);
            }
        }
    }
}

/*
 * Analyze a field access expression
 */
static void analyze_field(MoveContext *ctx, Expr *expr, bool is_move_context) {
    /* First analyze the base object */
    analyze_expr(ctx, expr->field.object, false);

    /* If moving a field, need to track partial move */
    if (is_move_context && expr->field.object->kind == EXPR_IDENT) {
        /* Use resolved symbol from the identifier expression */
        Symbol *sym = expr->field.object->ident.resolved;
        if (sym && (sym->kind == SYM_VAR || sym->kind == SYM_PARAM)) {
            BindingInfo *info = get_binding_info(ctx, sym);
            if (info) {
                /* Check if the field's type is Copy */
                /* For now, simplified - treat as full move if any field moved */
                bool is_copy = false;  /* TODO: Check field type */

                if (!is_copy && !info->is_movable) {
                    diag_report(ctx->diag, DIAG_ERROR, E_MEM_3006, expr->span,
                        "cannot move field from immovable binding '%s'",
                        sym->name.data);
                    return;
                }

                if (!is_copy) {
                    /* Track partial move */
                    vec_push(info->moved_fields, expr->field.field);
                    info->state = BIND_STATE_PARTIALLY_MOVED;
                    info->last_use_span = expr->span;
                }
            }
        }
    }
}

/*
 * Analyze a move expression
 */
static void analyze_move(MoveContext *ctx, Expr *expr) {
    /* The operand is used in a move context */
    analyze_expr(ctx, expr->move.operand, true);
}

/*
 * Analyze an assignment expression
 */
static void analyze_assignment(MoveContext *ctx, Expr *target, Expr *value) {
    /* Check if target is a mutable binding */
    if (target->kind == EXPR_IDENT) {
        /* Use resolved symbol from the identifier expression */
        Symbol *sym = target->ident.resolved;
        if (sym && (sym->kind == SYM_VAR || sym->kind == SYM_PARAM)) {
            BindingInfo *info = get_binding_info(ctx, sym);
            if (info) {
                if (!info->is_mutable) {
                    diag_report(ctx->diag, DIAG_ERROR, E_MEM_3003, target->span,
                        "cannot assign to immutable binding '%s'",
                        sym->name.data);
                }

                /* Analyze value - might move from it */
                analyze_expr(ctx, value, true);

                /* Reassignment revalidates the binding */
                if (info->state == BIND_STATE_MOVED ||
                    info->state == BIND_STATE_UNINITIALIZED) {
                    if (info->is_mutable) {
                        set_binding_state(ctx, sym, BIND_STATE_VALID, target->span);
                        vec_clear(info->moved_fields);
                    }
                }
                return;
            }
        }
    }

    /* For field assignments and other targets */
    analyze_expr(ctx, target, false);
    analyze_expr(ctx, value, true);
}

/*
 * Analyze a call expression
 */
static void analyze_call(MoveContext *ctx, Expr *expr) {
    /* Analyze callee */
    analyze_expr(ctx, expr->call.callee, false);

    /* Analyze arguments - each argument might be moved depending on parameter mode */
    /* For now, conservatively treat all non-Copy arguments as moves */
    for (size_t i = 0; i < vec_len(expr->call.args); i++) {
        Expr *arg = expr->call.args[i];

        /* Check if the argument is a move expression */
        bool is_move = (arg->kind == EXPR_MOVE);

        analyze_expr(ctx, arg, is_move);
    }
}

/*
 * Analyze a method call expression
 */
static void analyze_method_call(MoveContext *ctx, Expr *expr) {
    /* Analyze receiver */
    analyze_expr(ctx, expr->method_call.receiver, false);

    /* Analyze arguments */
    for (size_t i = 0; i < vec_len(expr->method_call.args); i++) {
        Expr *arg = expr->method_call.args[i];
        bool is_move = (arg->kind == EXPR_MOVE);
        analyze_expr(ctx, arg, is_move);
    }
}

/*
 * Analyze an if expression
 */
static void analyze_if(MoveContext *ctx, Expr *expr) {
    /* Analyze condition */
    analyze_expr(ctx, expr->if_.condition, false);

    /* Save binding states before branches */
    /* For now, simplified - we don't merge states from branches */
    /* A more complete implementation would track states per branch */

    /* Analyze then branch */
    enter_scope(ctx);
    analyze_expr(ctx, expr->if_.then_branch, false);
    exit_scope(ctx);

    /* Analyze else branch if present */
    if (expr->if_.else_branch) {
        enter_scope(ctx);
        analyze_expr(ctx, expr->if_.else_branch, false);
        exit_scope(ctx);
    }
}

/*
 * Analyze a match expression
 */
static void analyze_match(MoveContext *ctx, Expr *expr) {
    /* Analyze scrutinee - might be moved by the match */
    analyze_expr(ctx, expr->match.scrutinee, true);

    /* Analyze each arm */
    for (size_t i = 0; i < vec_len(expr->match.arms_patterns); i++) {
        Pattern *pat = expr->match.arms_patterns[i];
        Expr *body = expr->match.arms_bodies[i];

        enter_scope(ctx);

        /* Pattern creates bindings */
        analyze_pattern(ctx, pat, true);

        /* Analyze body */
        analyze_expr(ctx, body, false);

        exit_scope(ctx);
    }
}

/*
 * Analyze a block expression
 */
static void analyze_block(MoveContext *ctx, Expr *expr) {
    enter_scope(ctx);

    /* Analyze statements */
    for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
        analyze_stmt(ctx, expr->block.stmts[i]);
    }

    /* Analyze result expression if present */
    if (expr->block.result) {
        analyze_expr(ctx, expr->block.result, false);
    }

    exit_scope(ctx);
}

/*
 * Analyze a loop expression
 */
static void analyze_loop(MoveContext *ctx, Expr *expr) {
    ctx->loop_depth++;

    enter_scope(ctx);

    /* Analyze binding pattern if present (loop x in ...) */
    if (expr->loop.binding) {
        analyze_pattern(ctx, expr->loop.binding, true);
    }

    /* Analyze iterable if present */
    if (expr->loop.iterable) {
        analyze_expr(ctx, expr->loop.iterable, false);
    }

    /* Analyze condition if present */
    if (expr->loop.condition) {
        analyze_expr(ctx, expr->loop.condition, false);
    }

    /* Analyze body */
    if (expr->loop.body) {
        analyze_expr(ctx, expr->loop.body, false);
    }

    exit_scope(ctx);

    ctx->loop_depth--;
}

/*
 * Analyze a binary expression
 */
static void analyze_binary(MoveContext *ctx, Expr *expr) {
    /* Check for assignment operators */
    if (expr->binary.op >= BINOP_ASSIGN && expr->binary.op <= BINOP_SHR_ASSIGN) {
        analyze_assignment(ctx, expr->binary.left, expr->binary.right);
        return;
    }

    /* Regular binary - analyze both sides */
    analyze_expr(ctx, expr->binary.left, false);
    analyze_expr(ctx, expr->binary.right, false);
}

/*
 * Analyze an expression
 */
static void analyze_expr(MoveContext *ctx, Expr *expr, bool is_move_context) {
    if (!expr) return;

    switch (expr->kind) {
        case EXPR_INT_LIT:
        case EXPR_FLOAT_LIT:
        case EXPR_STRING_LIT:
        case EXPR_CHAR_LIT:
        case EXPR_BOOL_LIT:
            /* Literals are always valid */
            break;

        case EXPR_IDENT:
            analyze_ident(ctx, expr, is_move_context);
            break;

        case EXPR_PATH:
            /* Path expressions - analyze as identifier of last segment */
            /* Module paths don't have move semantics */
            break;

        case EXPR_BINARY:
            analyze_binary(ctx, expr);
            break;

        case EXPR_UNARY:
            analyze_expr(ctx, expr->unary.operand,
                expr->unary.op == UNOP_DEREF ? is_move_context : false);
            break;

        case EXPR_CALL:
            analyze_call(ctx, expr);
            break;

        case EXPR_METHOD_CALL:
            analyze_method_call(ctx, expr);
            break;

        case EXPR_FIELD:
            analyze_field(ctx, expr, is_move_context);
            break;

        case EXPR_INDEX:
            analyze_expr(ctx, expr->index.object, false);
            analyze_expr(ctx, expr->index.index, false);
            break;

        case EXPR_TUPLE:
            for (size_t i = 0; i < vec_len(expr->tuple.elements); i++) {
                analyze_expr(ctx, expr->tuple.elements[i], is_move_context);
            }
            break;

        case EXPR_ARRAY:
            if (expr->array.repeat_value) {
                analyze_expr(ctx, expr->array.repeat_value, false);
                analyze_expr(ctx, expr->array.repeat_count, false);
            } else {
                for (size_t i = 0; i < vec_len(expr->array.elements); i++) {
                    analyze_expr(ctx, expr->array.elements[i], is_move_context);
                }
            }
            break;

        case EXPR_RECORD:
            for (size_t i = 0; i < vec_len(expr->record.field_values); i++) {
                analyze_expr(ctx, expr->record.field_values[i], true);
            }
            break;

        case EXPR_IF:
            analyze_if(ctx, expr);
            break;

        case EXPR_MATCH:
            analyze_match(ctx, expr);
            break;

        case EXPR_BLOCK:
            analyze_block(ctx, expr);
            break;

        case EXPR_LOOP:
            analyze_loop(ctx, expr);
            break;

        case EXPR_MOVE:
            analyze_move(ctx, expr);
            break;

        case EXPR_WIDEN:
            analyze_expr(ctx, expr->widen.operand, is_move_context);
            break;

        case EXPR_CAST:
            analyze_expr(ctx, expr->cast.operand, false);
            break;

        case EXPR_RANGE:
            if (expr->range.start) analyze_expr(ctx, expr->range.start, false);
            if (expr->range.end) analyze_expr(ctx, expr->range.end, false);
            break;

        case EXPR_STATIC_CALL:
            for (size_t i = 0; i < vec_len(expr->static_call.args); i++) {
                Expr *arg = expr->static_call.args[i];
                bool is_move = (arg->kind == EXPR_MOVE);
                analyze_expr(ctx, arg, is_move);
            }
            break;

        case EXPR_REGION_ALLOC:
            analyze_expr(ctx, expr->region_alloc.value, true);
            break;

        case EXPR_ADDR_OF:
            /* Taking address doesn't move */
            analyze_expr(ctx, expr->addr_of.operand, false);
            break;

        case EXPR_DEREF:
            analyze_expr(ctx, expr->deref.operand, false);
            break;

        case EXPR_CLOSURE:
            /* Analyze closure body in its own scope */
            enter_scope(ctx);
            for (size_t i = 0; i < vec_len(expr->closure.params); i++) {
                analyze_pattern(ctx, expr->closure.params[i], true);
            }
            analyze_expr(ctx, expr->closure.body, false);
            exit_scope(ctx);
            break;
    }
}

/*
 * Analyze a pattern (creates bindings when used in let/match/etc.)
 */
static void analyze_pattern(MoveContext *ctx, Pattern *pat, bool creates_binding) {
    if (!pat) return;

    switch (pat->kind) {
        case PAT_WILDCARD:
            /* No binding created */
            break;

        case PAT_BINDING:
            if (creates_binding) {
                /* Use resolved symbol from name resolution */
                Symbol *sym = pat->binding.resolved;
                if (sym) {
                    /* Create binding info */
                    BindingInfo *info = create_binding_info(ctx, sym, true);
                    info->is_mutable = pat->binding.is_mutable;
                    register_binding(ctx, sym);
                }
            }
            break;

        case PAT_LITERAL:
            /* No binding, analyze the literal expression */
            if (pat->literal.value) {
                analyze_expr(ctx, pat->literal.value, false);
            }
            break;

        case PAT_TUPLE:
            for (size_t i = 0; i < vec_len(pat->tuple.elements); i++) {
                analyze_pattern(ctx, pat->tuple.elements[i], creates_binding);
            }
            break;

        case PAT_RECORD:
            for (size_t i = 0; i < vec_len(pat->record.field_patterns); i++) {
                analyze_pattern(ctx, pat->record.field_patterns[i], creates_binding);
            }
            break;

        case PAT_ENUM:
            if (pat->enum_.payload) {
                analyze_pattern(ctx, pat->enum_.payload, creates_binding);
            }
            break;

        case PAT_MODAL:
            for (size_t i = 0; i < vec_len(pat->modal.field_patterns); i++) {
                analyze_pattern(ctx, pat->modal.field_patterns[i], creates_binding);
            }
            break;

        case PAT_RANGE:
            analyze_pattern(ctx, pat->range.start, false);
            analyze_pattern(ctx, pat->range.end, false);
            break;

        case PAT_OR:
            /* Each alternative must create the same bindings */
            for (size_t i = 0; i < vec_len(pat->or_.alternatives); i++) {
                analyze_pattern(ctx, pat->or_.alternatives[i], creates_binding);
            }
            break;

        case PAT_GUARD:
            analyze_pattern(ctx, pat->guard.pattern, creates_binding);
            analyze_expr(ctx, pat->guard.guard, false);
            break;
    }
}

/*
 * Analyze a statement
 */
static void analyze_stmt(MoveContext *ctx, Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {
        case STMT_EXPR:
            analyze_expr(ctx, stmt->expr.expr, false);
            break;

        case STMT_LET: {
            /* Analyze initializer first (might move from existing bindings) */
            if (stmt->let.init) {
                analyze_expr(ctx, stmt->let.init, true);
            }

            /* Create binding */
            if (stmt->let.pattern) {
                /* Analyze pattern - creates binding info */
                analyze_pattern(ctx, stmt->let.pattern, true);

                /* Set binding properties from statement */
                if (stmt->let.pattern->kind == PAT_BINDING) {
                    Symbol *sym = stmt->let.pattern->binding.resolved;
                    if (sym) {
                        BindingInfo *info = get_binding_info(ctx, sym);
                        if (info) {
                            /* Set movability from binding operator (= vs :=) */
                            info->is_movable = (stmt->let.op == BIND_MOVABLE);
                            if (!stmt->let.init) {
                                info->state = BIND_STATE_UNINITIALIZED;
                            }
                        }
                    }
                }
            }
            break;
        }

        case STMT_VAR: {
            /* Same as let but mutable */
            if (stmt->var.init) {
                analyze_expr(ctx, stmt->var.init, true);
            }

            if (stmt->var.pattern) {
                analyze_pattern(ctx, stmt->var.pattern, true);

                /* Set binding properties from statement */
                if (stmt->var.pattern->kind == PAT_BINDING) {
                    Symbol *sym = stmt->var.pattern->binding.resolved;
                    if (sym) {
                        BindingInfo *info = get_binding_info(ctx, sym);
                        if (info) {
                            /* Set movability from binding operator (= vs :=) */
                            info->is_movable = (stmt->var.op == BIND_MOVABLE);
                            info->is_mutable = true;  /* var is always mutable */
                            if (!stmt->var.init) {
                                info->state = BIND_STATE_UNINITIALIZED;
                            }
                        }
                    }
                }
            }
            break;
        }

        case STMT_ASSIGN:
            analyze_assignment(ctx, stmt->assign.target, stmt->assign.value);
            break;

        case STMT_RETURN:
            if (stmt->return_.value) {
                analyze_expr(ctx, stmt->return_.value, true);
            }
            break;

        case STMT_RESULT:
            if (stmt->result.value) {
                analyze_expr(ctx, stmt->result.value, true);
            }
            break;

        case STMT_BREAK:
            if (ctx->loop_depth == 0) {
                diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                    "'break' outside of loop");
            }
            if (stmt->break_.value) {
                analyze_expr(ctx, stmt->break_.value, true);
            }
            break;

        case STMT_CONTINUE:
            if (ctx->loop_depth == 0) {
                diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                    "'continue' outside of loop");
            }
            break;

        case STMT_DEFER:
            /* Defer body is analyzed but executed later */
            vec_push(ctx->defers, stmt->defer.body);
            break;

        case STMT_UNSAFE:
            analyze_expr(ctx, stmt->unsafe.body, false);
            break;
    }
}

/*
 * Analyze a procedure body
 */
static void analyze_proc(MoveContext *ctx, ProcDecl *proc) {
    ctx->return_type = NULL;  /* TODO: Get from type checking */
    ctx->proc_scope = proc->scope;  /* Use procedure's resolved scope for lookups */

    enter_scope(ctx);

    /* Register parameters as bindings */
    for (size_t i = 0; i < vec_len(proc->params); i++) {
        ParamDecl *param = &proc->params[i];
        /* Use resolved symbol from name resolution */
        Symbol *sym = param->resolved;
        if (sym) {
            BindingInfo *info = create_binding_info(ctx, sym, true);
            info->is_movable = param->is_move;  /* move parameters are movable */
            info->perm = param->perm;
            register_binding(ctx, sym);
        }
    }

    /* Analyze body */
    if (proc->body) {
        analyze_expr(ctx, proc->body, false);
    }

    /* Check defers were all executed */
    /* Defers run at scope exit, which is handled by exit_scope */

    exit_scope(ctx);
    ctx->proc_scope = NULL;  /* Clear procedure scope */
}

/*
 * Analyze a declaration
 */
static void analyze_decl(MoveContext *ctx, Decl *decl) {
    if (!decl) return;

    switch (decl->kind) {
        case DECL_PROC:
            analyze_proc(ctx, &decl->proc);
            break;

        case DECL_RECORD:
            /* Analyze methods */
            for (size_t i = 0; i < vec_len(decl->record.methods); i++) {
                analyze_proc(ctx, &decl->record.methods[i]);
            }
            break;

        case DECL_ENUM:
            for (size_t i = 0; i < vec_len(decl->enum_.methods); i++) {
                analyze_proc(ctx, &decl->enum_.methods[i]);
            }
            break;

        case DECL_MODAL:
            /* Analyze shared methods */
            for (size_t i = 0; i < vec_len(decl->modal.shared_methods); i++) {
                analyze_proc(ctx, &decl->modal.shared_methods[i]);
            }
            /* Analyze state-specific methods */
            for (size_t i = 0; i < vec_len(decl->modal.states); i++) {
                ModalState *state = &decl->modal.states[i];
                for (size_t j = 0; j < vec_len(state->methods); j++) {
                    analyze_proc(ctx, &state->methods[j]);
                }
                /* Transitions are like methods */
                for (size_t j = 0; j < vec_len(state->transitions); j++) {
                    Transition *trans = &state->transitions[j];
                    if (trans->body) {
                        enter_scope(ctx);
                        /* Register parameters */
                        for (size_t k = 0; k < vec_len(trans->params); k++) {
                            ParamDecl *param = &trans->params[k];
                            Symbol *sym = move_scope_lookup(ctx, param->name);
                            if (sym) {
                                create_binding_info(ctx, sym, true);
                                register_binding(ctx, sym);
                            }
                        }
                        analyze_expr(ctx, trans->body, false);
                        exit_scope(ctx);
                    }
                }
            }
            break;

        case DECL_CLASS:
            /* Default methods */
            for (size_t i = 0; i < vec_len(decl->class_.default_methods); i++) {
                analyze_proc(ctx, &decl->class_.default_methods[i]);
            }
            break;

        case DECL_TYPE_ALIAS:
        case DECL_EXTERN:
        case DECL_MODULE:
        case DECL_IMPORT:
        case DECL_USE:
            /* No move analysis needed */
            break;
    }
}

/*
 * Look up a symbol in the current procedure's scope
 */
static Symbol *move_scope_lookup(MoveContext *ctx, InternedString name) {
    if (ctx->proc_scope) {
        return scope_lookup_from(ctx->proc_scope, name);
    }
    /* Fallback to sema's scope if no proc scope set */
    return scope_lookup_from(ctx->sema->current_scope, name);
}

/*
 * Main entry point: analyze moves in a module
 */
bool sema_analyze_moves(SemaContext *ctx, Module *mod) {
    MoveContext mctx;
    move_ctx_init(&mctx, ctx);

    /* Analyze all declarations */
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        analyze_decl(&mctx, mod->decls[i]);
    }

    return !diag_has_errors(ctx->diag);
}
