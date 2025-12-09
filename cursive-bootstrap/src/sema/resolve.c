/*
 * Cursive Bootstrap Compiler - Name Resolution Pass
 *
 * Walks the AST and resolves all name references to their definitions.
 * Builds symbol tables and checks for undefined/redefined names.
 */

#include "scope.h"
#include "types.h"
#include "sema.h"
#include "common/error.h"

/* Name resolution context */
typedef struct ResolveContext {
    Arena *arena;
    DiagContext *diag;
    ScopeContext scope_ctx;
    TypeContext type_ctx;
    StringPool *strings;

    /* Current context */
    Decl *current_type_decl;      /* Current record/enum/modal being resolved */
    ProcDecl *current_proc;       /* Current procedure being resolved */
} ResolveContext;

/* Forward declarations */
static void resolve_decl(ResolveContext *ctx, Decl *decl);
static void resolve_stmt(ResolveContext *ctx, Stmt *stmt);
static void resolve_expr(ResolveContext *ctx, Expr *expr);
static void resolve_type_expr(ResolveContext *ctx, TypeExpr *type);
static void resolve_pattern(ResolveContext *ctx, Pattern *pat, bool is_definition);

/* Error reporting helpers */
static void error_undefined(ResolveContext *ctx, InternedString name, SourceSpan span) {
    diag_report(ctx->diag, DIAG_ERROR, E_RES_0200, span,
        "undefined name '%.*s'", (int)name.len, name.data);
}

static void error_redefined(ResolveContext *ctx, InternedString name,
                           SourceSpan new_span, SourceSpan old_span) {
    (void)old_span;  /* TODO: add note about previous definition */
    diag_report(ctx->diag, DIAG_ERROR, E_RES_0201, new_span,
        "redefinition of '%.*s'", (int)name.len, name.data);
}

static void error_not_visible(ResolveContext *ctx, InternedString name, SourceSpan span) {
    diag_report(ctx->diag, DIAG_ERROR, E_RES_0202, span,
        "'%.*s' is not visible from this scope", (int)name.len, name.data);
}

/*
 * Phase 1: Register all top-level declarations
 * This creates symbols for types/procedures before resolving bodies
 */
static void register_top_level_decls(ResolveContext *ctx, Module *mod) {
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        Decl *decl = mod->decls[i];

        switch (decl->kind) {
            case DECL_RECORD: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->record.name,
                                          SYM_TYPE, decl->record.vis, decl,
                                          decl->record.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->record.name);
                    error_redefined(ctx, decl->record.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_ENUM: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->enum_.name,
                                          SYM_TYPE, decl->enum_.vis, decl,
                                          decl->enum_.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->enum_.name);
                    error_redefined(ctx, decl->enum_.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_MODAL: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->modal.name,
                                          SYM_TYPE, decl->modal.vis, decl,
                                          decl->modal.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->modal.name);
                    error_redefined(ctx, decl->modal.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_TYPE_ALIAS: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->type_alias.name,
                                          SYM_TYPE, decl->type_alias.vis, decl,
                                          decl->type_alias.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->type_alias.name);
                    error_redefined(ctx, decl->type_alias.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_CLASS: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->class_.name,
                                          SYM_CLASS, decl->class_.vis, decl,
                                          decl->class_.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->class_.name);
                    error_redefined(ctx, decl->class_.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_PROC: {
                Symbol *sym = scope_define(&ctx->scope_ctx, decl->proc.name,
                                          SYM_PROC, decl->proc.vis, decl,
                                          decl->proc.span);
                if (!sym) {
                    Symbol *existing = scope_lookup(&ctx->scope_ctx, decl->proc.name);
                    error_redefined(ctx, decl->proc.name, decl->span,
                                   existing ? existing->span : (SourceSpan){0});
                }
                break;
            }

            case DECL_IMPORT:
            case DECL_USE:
                /* Handled separately */
                break;

            case DECL_EXTERN:
                /* Register extern functions */
                for (size_t j = 0; j < vec_len(decl->extern_.funcs); j++) {
                    ExternFuncDecl *func = &decl->extern_.funcs[j];
                    Symbol *sym = scope_define(&ctx->scope_ctx, func->name,
                                              SYM_PROC, VIS_PUBLIC, decl,
                                              func->span);
                    if (!sym) {
                        Symbol *existing = scope_lookup(&ctx->scope_ctx, func->name);
                        error_redefined(ctx, func->name, func->span,
                                       existing ? existing->span : (SourceSpan){0});
                    }
                }
                break;

            case DECL_MODULE:
                /* Nested modules - TODO */
                break;
        }
    }
}

/*
 * Resolve a generic parameter list
 */
static void resolve_generic_params(ResolveContext *ctx, Vec(GenericParam) params) {
    for (size_t i = 0; i < vec_len(params); i++) {
        GenericParam *param = &params[i];

        /* Define the generic parameter in current scope */
        Symbol *sym = scope_define(&ctx->scope_ctx, param->name, SYM_GENERIC,
                                  VIS_PRIVATE, NULL, param->span);
        if (sym) {
            sym->generic_index = i;
        }

        /* Resolve bounds */
        for (size_t j = 0; j < vec_len(param->bounds); j++) {
            resolve_type_expr(ctx, param->bounds[j]);
        }

        /* Resolve default */
        if (param->default_type) {
            resolve_type_expr(ctx, param->default_type);
        }
    }
}

/*
 * Resolve a where clause
 */
static void resolve_where_clauses(ResolveContext *ctx, Vec(WhereClause) clauses) {
    for (size_t i = 0; i < vec_len(clauses); i++) {
        WhereClause *clause = &clauses[i];
        resolve_type_expr(ctx, clause->type);
        for (size_t j = 0; j < vec_len(clause->bounds); j++) {
            resolve_type_expr(ctx, clause->bounds[j]);
        }
    }
}

/*
 * Resolve a type expression
 */
static void resolve_type_expr(ResolveContext *ctx, TypeExpr *type) {
    if (!type) return;

    switch (type->kind) {
        case TEXPR_PRIMITIVE:
            /* Built-in, nothing to resolve */
            break;

        case TEXPR_NAMED: {
            /* Look up the named type */
            Symbol *sym = scope_lookup(&ctx->scope_ctx, type->named.name);
            if (!sym) {
                error_undefined(ctx, type->named.name, type->span);
            } else if (!scope_is_visible(ctx->scope_ctx.current, sym)) {
                error_not_visible(ctx, type->named.name, type->span);
            }
            /* TODO: store resolved symbol in AST node */
            break;
        }

        case TEXPR_MODAL_STATE:
            resolve_type_expr(ctx, type->modal_state.base);
            /* State name will be validated against the modal type */
            break;

        case TEXPR_GENERIC:
            resolve_type_expr(ctx, type->generic.base);
            for (size_t i = 0; i < vec_len(type->generic.args); i++) {
                resolve_type_expr(ctx, type->generic.args[i]);
            }
            break;

        case TEXPR_TUPLE:
            for (size_t i = 0; i < vec_len(type->tuple.elements); i++) {
                resolve_type_expr(ctx, type->tuple.elements[i]);
            }
            break;

        case TEXPR_ARRAY:
            resolve_type_expr(ctx, type->array.element);
            if (type->array.size) {
                resolve_expr(ctx, type->array.size);
            }
            break;

        case TEXPR_SLICE:
            resolve_type_expr(ctx, type->slice.element);
            break;

        case TEXPR_FUNCTION:
            for (size_t i = 0; i < vec_len(type->function.params); i++) {
                resolve_type_expr(ctx, type->function.params[i]);
            }
            resolve_type_expr(ctx, type->function.return_type);
            break;

        case TEXPR_UNION:
            for (size_t i = 0; i < vec_len(type->union_.members); i++) {
                resolve_type_expr(ctx, type->union_.members[i]);
            }
            break;

        case TEXPR_PTR:
            resolve_type_expr(ctx, type->ptr.pointee);
            break;

        case TEXPR_REF:
            resolve_type_expr(ctx, type->ref.referent);
            break;

        case TEXPR_SELF: {
            /* Self must be within a type scope */
            Scope *type_scope = scope_enclosing_type(ctx->scope_ctx.current);
            if (!type_scope || !type_scope->self_type) {
                diag_report(ctx->diag, DIAG_ERROR, E_RES_0200, type->span,
                    "'Self' can only be used within a type definition");
            }
            break;
        }

        case TEXPR_NEVER:
        case TEXPR_UNIT:
        case TEXPR_INFER:
            /* Nothing to resolve */
            break;
    }
}

/*
 * Resolve a pattern
 */
static void resolve_pattern(ResolveContext *ctx, Pattern *pat, bool is_definition) {
    if (!pat) return;

    switch (pat->kind) {
        case PAT_WILDCARD:
            /* Nothing to resolve */
            break;

        case PAT_BINDING:
            if (is_definition) {
                /* Define the binding in current scope */
                Symbol *sym = scope_define(&ctx->scope_ctx, pat->binding.name,
                                          SYM_VAR, VIS_PRIVATE, NULL, pat->span);
                if (!sym) {
                    Symbol *existing = scope_lookup_local(ctx->scope_ctx.current,
                                                         pat->binding.name);
                    error_redefined(ctx, pat->binding.name, pat->span,
                                   existing ? existing->span : (SourceSpan){0});
                } else {
                    sym->is_mutable = pat->binding.is_mutable;
                }
                /* Store resolved symbol on pattern for later phases */
                pat->binding.resolved = sym;
            } else {
                /* In match patterns, check if identifier is a constant */
                /* For now, treat as new binding */
                Symbol *sym = scope_define(&ctx->scope_ctx, pat->binding.name,
                                          SYM_VAR, VIS_PRIVATE, NULL, pat->span);
                if (sym) {
                    sym->is_mutable = pat->binding.is_mutable;
                }
                /* Store resolved symbol on pattern for later phases */
                pat->binding.resolved = sym;
            }
            if (pat->binding.type) {
                resolve_type_expr(ctx, pat->binding.type);
            }
            break;

        case PAT_LITERAL:
            resolve_expr(ctx, pat->literal.value);
            break;

        case PAT_TUPLE:
            for (size_t i = 0; i < vec_len(pat->tuple.elements); i++) {
                resolve_pattern(ctx, pat->tuple.elements[i], is_definition);
            }
            break;

        case PAT_RECORD:
            resolve_type_expr(ctx, pat->record.type);
            for (size_t i = 0; i < vec_len(pat->record.field_patterns); i++) {
                resolve_pattern(ctx, pat->record.field_patterns[i], is_definition);
            }
            break;

        case PAT_ENUM:
            resolve_type_expr(ctx, pat->enum_.type);
            if (pat->enum_.payload) {
                resolve_pattern(ctx, pat->enum_.payload, is_definition);
            }
            break;

        case PAT_MODAL:
            /* State name checked later */
            for (size_t i = 0; i < vec_len(pat->modal.field_patterns); i++) {
                resolve_pattern(ctx, pat->modal.field_patterns[i], is_definition);
            }
            break;

        case PAT_RANGE:
            resolve_pattern(ctx, pat->range.start, is_definition);
            resolve_pattern(ctx, pat->range.end, is_definition);
            break;

        case PAT_OR:
            for (size_t i = 0; i < vec_len(pat->or_.alternatives); i++) {
                resolve_pattern(ctx, pat->or_.alternatives[i], is_definition);
            }
            break;

        case PAT_GUARD:
            resolve_pattern(ctx, pat->guard.pattern, is_definition);
            resolve_expr(ctx, pat->guard.guard);
            break;
    }
}

/*
 * Resolve an expression
 */
static void resolve_expr(ResolveContext *ctx, Expr *expr) {
    if (!expr) return;

    switch (expr->kind) {
        case EXPR_INT_LIT:
        case EXPR_FLOAT_LIT:
        case EXPR_STRING_LIT:
        case EXPR_CHAR_LIT:
        case EXPR_BOOL_LIT:
            /* Literals don't need resolution */
            break;

        case EXPR_IDENT: {
            Symbol *sym = scope_lookup(&ctx->scope_ctx, expr->ident.name);
            if (!sym) {
                error_undefined(ctx, expr->ident.name, expr->span);
            } else if (!scope_is_visible(ctx->scope_ctx.current, sym)) {
                error_not_visible(ctx, expr->ident.name, expr->span);
            }
            /* Store resolved symbol in AST for later phases */
            expr->ident.resolved = sym;
            break;
        }

        case EXPR_PATH:
            /* Path resolution - module::item::subitem */
            /* TODO: resolve full path through module hierarchy */
            break;

        case EXPR_BINARY:
            resolve_expr(ctx, expr->binary.left);
            resolve_expr(ctx, expr->binary.right);
            break;

        case EXPR_UNARY:
            resolve_expr(ctx, expr->unary.operand);
            break;

        case EXPR_CALL:
            resolve_expr(ctx, expr->call.callee);
            for (size_t i = 0; i < vec_len(expr->call.args); i++) {
                resolve_expr(ctx, expr->call.args[i]);
            }
            break;

        case EXPR_METHOD_CALL:
            resolve_expr(ctx, expr->method_call.receiver);
            /* Method name resolved during type checking */
            for (size_t i = 0; i < vec_len(expr->method_call.args); i++) {
                resolve_expr(ctx, expr->method_call.args[i]);
            }
            for (size_t i = 0; i < vec_len(expr->method_call.type_args); i++) {
                resolve_type_expr(ctx, expr->method_call.type_args[i]);
            }
            break;

        case EXPR_FIELD:
            resolve_expr(ctx, expr->field.object);
            /* Field name resolved during type checking */
            break;

        case EXPR_INDEX:
            resolve_expr(ctx, expr->index.object);
            resolve_expr(ctx, expr->index.index);
            break;

        case EXPR_TUPLE:
            for (size_t i = 0; i < vec_len(expr->tuple.elements); i++) {
                resolve_expr(ctx, expr->tuple.elements[i]);
            }
            break;

        case EXPR_ARRAY:
            for (size_t i = 0; i < vec_len(expr->array.elements); i++) {
                resolve_expr(ctx, expr->array.elements[i]);
            }
            if (expr->array.repeat_value) {
                resolve_expr(ctx, expr->array.repeat_value);
                resolve_expr(ctx, expr->array.repeat_count);
            }
            break;

        case EXPR_RECORD:
            resolve_type_expr(ctx, expr->record.type);
            for (size_t i = 0; i < vec_len(expr->record.field_values); i++) {
                resolve_expr(ctx, expr->record.field_values[i]);
            }
            break;

        case EXPR_IF:
            resolve_expr(ctx, expr->if_.condition);
            resolve_expr(ctx, expr->if_.then_branch);
            if (expr->if_.else_branch) {
                resolve_expr(ctx, expr->if_.else_branch);
            }
            break;

        case EXPR_MATCH: {
            resolve_expr(ctx, expr->match.scrutinee);
            for (size_t i = 0; i < vec_len(expr->match.arms_patterns); i++) {
                /* Each arm gets its own scope for bindings */
                scope_enter(&ctx->scope_ctx, SCOPE_BLOCK);
                resolve_pattern(ctx, expr->match.arms_patterns[i], true);
                resolve_expr(ctx, expr->match.arms_bodies[i]);
                scope_exit(&ctx->scope_ctx);
            }
            break;
        }

        case EXPR_BLOCK: {
            scope_enter(&ctx->scope_ctx, SCOPE_BLOCK);
            for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
                resolve_stmt(ctx, expr->block.stmts[i]);
            }
            if (expr->block.result) {
                resolve_expr(ctx, expr->block.result);
            }
            scope_exit(&ctx->scope_ctx);
            break;
        }

        case EXPR_LOOP: {
            Scope *loop_scope = scope_enter(&ctx->scope_ctx, SCOPE_LOOP);
            loop_scope->loop_label = expr->loop.label;

            if (expr->loop.binding) {
                resolve_pattern(ctx, expr->loop.binding, true);
            }
            if (expr->loop.iterable) {
                resolve_expr(ctx, expr->loop.iterable);
            }
            if (expr->loop.condition) {
                resolve_expr(ctx, expr->loop.condition);
            }
            resolve_expr(ctx, expr->loop.body);

            scope_exit(&ctx->scope_ctx);
            break;
        }

        case EXPR_MOVE:
            resolve_expr(ctx, expr->move.operand);
            break;

        case EXPR_WIDEN:
            resolve_expr(ctx, expr->widen.operand);
            break;

        case EXPR_CAST:
            resolve_expr(ctx, expr->cast.operand);
            resolve_type_expr(ctx, expr->cast.target_type);
            break;

        case EXPR_RANGE:
            if (expr->range.start) resolve_expr(ctx, expr->range.start);
            if (expr->range.end) resolve_expr(ctx, expr->range.end);
            break;

        case EXPR_STATIC_CALL:
            resolve_type_expr(ctx, expr->static_call.type);
            /* Method name resolved during type checking */
            for (size_t i = 0; i < vec_len(expr->static_call.args); i++) {
                resolve_expr(ctx, expr->static_call.args[i]);
            }
            for (size_t i = 0; i < vec_len(expr->static_call.type_args); i++) {
                resolve_type_expr(ctx, expr->static_call.type_args[i]);
            }
            break;

        case EXPR_REGION_ALLOC: {
            /* Look up region name */
            Symbol *sym = scope_lookup(&ctx->scope_ctx, expr->region_alloc.region);
            if (!sym) {
                error_undefined(ctx, expr->region_alloc.region, expr->span);
            }
            resolve_expr(ctx, expr->region_alloc.value);
            break;
        }

        case EXPR_ADDR_OF:
            resolve_expr(ctx, expr->addr_of.operand);
            break;

        case EXPR_DEREF:
            resolve_expr(ctx, expr->deref.operand);
            break;

        case EXPR_CLOSURE: {
            scope_enter(&ctx->scope_ctx, SCOPE_BLOCK);
            for (size_t i = 0; i < vec_len(expr->closure.params); i++) {
                resolve_pattern(ctx, expr->closure.params[i], true);
            }
            if (expr->closure.return_type) {
                resolve_type_expr(ctx, expr->closure.return_type);
            }
            resolve_expr(ctx, expr->closure.body);
            scope_exit(&ctx->scope_ctx);
            break;
        }
    }
}

/*
 * Resolve a statement
 */
static void resolve_stmt(ResolveContext *ctx, Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {
        case STMT_EXPR:
            resolve_expr(ctx, stmt->expr.expr);
            break;

        case STMT_LET: {
            /* Resolve initializer first (in outer scope) */
            if (stmt->let.init) {
                resolve_expr(ctx, stmt->let.init);
            }
            if (stmt->let.type) {
                resolve_type_expr(ctx, stmt->let.type);
            }
            /* Then define the pattern bindings */
            resolve_pattern(ctx, stmt->let.pattern, true);
            break;
        }

        case STMT_VAR: {
            /* Same as let */
            if (stmt->var.init) {
                resolve_expr(ctx, stmt->var.init);
            }
            if (stmt->var.type) {
                resolve_type_expr(ctx, stmt->var.type);
            }
            resolve_pattern(ctx, stmt->var.pattern, true);
            break;
        }

        case STMT_ASSIGN:
            resolve_expr(ctx, stmt->assign.target);
            resolve_expr(ctx, stmt->assign.value);
            break;

        case STMT_RETURN:
            if (stmt->return_.value) {
                resolve_expr(ctx, stmt->return_.value);
            }
            break;

        case STMT_RESULT:
            resolve_expr(ctx, stmt->result.value);
            break;

        case STMT_BREAK: {
            /* Check that we're in a loop */
            Scope *loop = scope_enclosing_loop(ctx->scope_ctx.current,
                                               stmt->break_.label);
            if (!loop) {
                if (stmt->break_.label.data) {
                    diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                        "no loop with label '%.*s' in scope",
                        (int)stmt->break_.label.len, stmt->break_.label.data);
                } else {
                    diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                        "'break' outside of loop");
                }
            }
            if (stmt->break_.value) {
                resolve_expr(ctx, stmt->break_.value);
            }
            break;
        }

        case STMT_CONTINUE: {
            Scope *loop = scope_enclosing_loop(ctx->scope_ctx.current,
                                               stmt->continue_.label);
            if (!loop) {
                if (stmt->continue_.label.data) {
                    diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                        "no loop with label '%.*s' in scope",
                        (int)stmt->continue_.label.len, stmt->continue_.label.data);
                } else {
                    diag_report(ctx->diag, DIAG_ERROR, E_SYN_0100, stmt->span,
                        "'continue' outside of loop");
                }
            }
            break;
        }

        case STMT_DEFER:
            resolve_expr(ctx, stmt->defer.body);
            break;

        case STMT_UNSAFE:
            resolve_expr(ctx, stmt->unsafe.body);
            break;
    }
}

/*
 * Resolve a procedure declaration
 */
static void resolve_proc_decl(ResolveContext *ctx, ProcDecl *proc) {
    ctx->current_proc = proc;

    /* Create scope for generic parameters and procedure body */
    Scope *proc_scope = scope_enter(&ctx->scope_ctx, SCOPE_BLOCK);

    /* Resolve generic parameters */
    resolve_generic_params(ctx, proc->generics);

    /* Resolve where clauses */
    resolve_where_clauses(ctx, proc->where_clauses);

    /* Resolve parameter types and define parameters */
    for (size_t i = 0; i < vec_len(proc->params); i++) {
        ParamDecl *param = &proc->params[i];
        resolve_type_expr(ctx, param->type);

        Symbol *sym = scope_define(&ctx->scope_ctx, param->name, SYM_PARAM,
                                  VIS_PRIVATE, NULL, param->span);
        if (sym) {
            sym->is_mutable = false;  /* Parameters are immutable by default */
        }
        /* Store resolved symbol on parameter for later phases */
        param->resolved = sym;
    }

    /* Resolve return type */
    resolve_type_expr(ctx, proc->return_type);

    /* Resolve contracts */
    for (size_t i = 0; i < vec_len(proc->contracts); i++) {
        resolve_expr(ctx, proc->contracts[i].condition);
    }

    /* Resolve body */
    if (proc->body) {
        resolve_expr(ctx, proc->body);
    }

    /* Store scope on procedure for later phases (move analysis, etc.) */
    proc->scope = proc_scope;

    scope_exit(&ctx->scope_ctx);
    ctx->current_proc = NULL;
}

/*
 * Resolve a record declaration
 */
static void resolve_record_decl(ResolveContext *ctx, RecordDecl *rec) {
    /* Create type scope for Self and generics */
    Scope *type_scope = scope_enter(&ctx->scope_ctx, SCOPE_TYPE);

    /* Set up Self type */
    Symbol *self_sym = symbol_new(ctx->arena);
    self_sym->kind = SYM_TYPE;
    self_sym->name = string_pool_intern(ctx->strings, "Self");
    self_sym->vis = VIS_PRIVATE;
    type_scope->self_type = self_sym;
    scope_add_symbol(type_scope, self_sym);

    /* Resolve generics */
    resolve_generic_params(ctx, rec->generics);

    /* Resolve implemented classes */
    for (size_t i = 0; i < vec_len(rec->implements); i++) {
        resolve_type_expr(ctx, rec->implements[i]);
    }

    /* Resolve field types */
    for (size_t i = 0; i < vec_len(rec->fields); i++) {
        FieldDecl *field = &rec->fields[i];
        resolve_type_expr(ctx, field->type);
        if (field->default_value) {
            resolve_expr(ctx, field->default_value);
        }
    }

    /* Resolve methods (inline with type definition) */
    for (size_t i = 0; i < vec_len(rec->methods); i++) {
        resolve_proc_decl(ctx, &rec->methods[i]);
    }

    /* Resolve where clauses */
    resolve_where_clauses(ctx, rec->where_clauses);

    scope_exit(&ctx->scope_ctx);
}

/*
 * Resolve an enum declaration
 */
static void resolve_enum_decl(ResolveContext *ctx, EnumDecl *en) {
    /* Create type scope */
    Scope *type_scope = scope_enter(&ctx->scope_ctx, SCOPE_TYPE);

    /* Set up Self type */
    Symbol *self_sym = symbol_new(ctx->arena);
    self_sym->kind = SYM_TYPE;
    self_sym->name = string_pool_intern(ctx->strings, "Self");
    self_sym->vis = VIS_PRIVATE;
    type_scope->self_type = self_sym;
    scope_add_symbol(type_scope, self_sym);

    /* Resolve generics */
    resolve_generic_params(ctx, en->generics);

    /* Resolve implemented classes */
    for (size_t i = 0; i < vec_len(en->implements); i++) {
        resolve_type_expr(ctx, en->implements[i]);
    }

    /* Resolve variants */
    for (size_t i = 0; i < vec_len(en->variants); i++) {
        EnumVariant *var = &en->variants[i];
        if (var->payload) {
            resolve_type_expr(ctx, var->payload);
        }
        if (var->discriminant) {
            resolve_expr(ctx, var->discriminant);
        }
    }

    /* Resolve methods */
    for (size_t i = 0; i < vec_len(en->methods); i++) {
        resolve_proc_decl(ctx, &en->methods[i]);
    }

    /* Resolve where clauses */
    resolve_where_clauses(ctx, en->where_clauses);

    scope_exit(&ctx->scope_ctx);
}

/*
 * Resolve a modal type declaration
 */
static void resolve_modal_decl(ResolveContext *ctx, ModalDecl *modal) {
    /* Create type scope */
    Scope *type_scope = scope_enter(&ctx->scope_ctx, SCOPE_TYPE);

    /* Set up Self type */
    Symbol *self_sym = symbol_new(ctx->arena);
    self_sym->kind = SYM_TYPE;
    self_sym->name = string_pool_intern(ctx->strings, "Self");
    self_sym->vis = VIS_PRIVATE;
    type_scope->self_type = self_sym;
    scope_add_symbol(type_scope, self_sym);

    /* Resolve generics */
    resolve_generic_params(ctx, modal->generics);

    /* Resolve implemented classes */
    for (size_t i = 0; i < vec_len(modal->implements); i++) {
        resolve_type_expr(ctx, modal->implements[i]);
    }

    /* Register state names first */
    for (size_t i = 0; i < vec_len(modal->states); i++) {
        ModalState *state = &modal->states[i];
        Symbol *state_sym = scope_define(&ctx->scope_ctx, state->name, SYM_STATE,
                                        VIS_PUBLIC, NULL, state->span);
        (void)state_sym;
    }

    /* Resolve each state */
    for (size_t i = 0; i < vec_len(modal->states); i++) {
        ModalState *state = &modal->states[i];

        /* Resolve fields */
        for (size_t j = 0; j < vec_len(state->fields); j++) {
            FieldDecl *field = &state->fields[j];
            resolve_type_expr(ctx, field->type);
            if (field->default_value) {
                resolve_expr(ctx, field->default_value);
            }
        }

        /* Resolve state-specific methods */
        for (size_t j = 0; j < vec_len(state->methods); j++) {
            resolve_proc_decl(ctx, &state->methods[j]);
        }

        /* Resolve transitions */
        for (size_t j = 0; j < vec_len(state->transitions); j++) {
            Transition *trans = &state->transitions[j];

            /* Create scope for transition */
            scope_enter(&ctx->scope_ctx, SCOPE_BLOCK);

            /* Resolve parameters */
            for (size_t k = 0; k < vec_len(trans->params); k++) {
                ParamDecl *param = &trans->params[k];
                resolve_type_expr(ctx, param->type);
                scope_define(&ctx->scope_ctx, param->name, SYM_PARAM,
                           VIS_PRIVATE, NULL, param->span);
            }

            /* Check target state exists */
            Symbol *target = scope_lookup(&ctx->scope_ctx, trans->target_state);
            if (!target || target->kind != SYM_STATE) {
                diag_report(ctx->diag, DIAG_ERROR, E_RES_0200, trans->span,
                    "unknown state '@%.*s'",
                    (int)trans->target_state.len, trans->target_state.data);
            }

            /* Resolve body */
            if (trans->body) {
                resolve_expr(ctx, trans->body);
            }

            scope_exit(&ctx->scope_ctx);
        }
    }

    /* Resolve shared methods */
    for (size_t i = 0; i < vec_len(modal->shared_methods); i++) {
        resolve_proc_decl(ctx, &modal->shared_methods[i]);
    }

    /* Resolve where clauses */
    resolve_where_clauses(ctx, modal->where_clauses);

    scope_exit(&ctx->scope_ctx);
}

/*
 * Resolve a class declaration
 */
static void resolve_class_decl(ResolveContext *ctx, ClassDecl *cls) {
    /* Create type scope */
    Scope *type_scope = scope_enter(&ctx->scope_ctx, SCOPE_TYPE);

    /* Set up Self type */
    Symbol *self_sym = symbol_new(ctx->arena);
    self_sym->kind = SYM_TYPE;
    self_sym->name = string_pool_intern(ctx->strings, "Self");
    self_sym->vis = VIS_PRIVATE;
    type_scope->self_type = self_sym;
    scope_add_symbol(type_scope, self_sym);

    /* Resolve generics */
    resolve_generic_params(ctx, cls->generics);

    /* Resolve superclasses */
    for (size_t i = 0; i < vec_len(cls->superclasses); i++) {
        resolve_type_expr(ctx, cls->superclasses[i]);
    }

    /* Resolve required method signatures */
    for (size_t i = 0; i < vec_len(cls->methods); i++) {
        resolve_proc_decl(ctx, &cls->methods[i]);
    }

    /* Resolve default method implementations */
    for (size_t i = 0; i < vec_len(cls->default_methods); i++) {
        resolve_proc_decl(ctx, &cls->default_methods[i]);
    }

    /* Resolve where clauses */
    resolve_where_clauses(ctx, cls->where_clauses);

    scope_exit(&ctx->scope_ctx);
}

/*
 * Resolve a declaration
 */
static void resolve_decl(ResolveContext *ctx, Decl *decl) {
    ctx->current_type_decl = decl;

    switch (decl->kind) {
        case DECL_RECORD:
            resolve_record_decl(ctx, &decl->record);
            break;

        case DECL_ENUM:
            resolve_enum_decl(ctx, &decl->enum_);
            break;

        case DECL_MODAL:
            resolve_modal_decl(ctx, &decl->modal);
            break;

        case DECL_TYPE_ALIAS: {
            /* Create scope for generics */
            scope_enter(&ctx->scope_ctx, SCOPE_TYPE);
            resolve_generic_params(ctx, decl->type_alias.generics);
            resolve_type_expr(ctx, decl->type_alias.aliased);
            scope_exit(&ctx->scope_ctx);
            break;
        }

        case DECL_CLASS:
            resolve_class_decl(ctx, &decl->class_);
            break;

        case DECL_PROC:
            resolve_proc_decl(ctx, &decl->proc);
            break;

        case DECL_EXTERN:
            /* Resolve extern function signatures */
            for (size_t i = 0; i < vec_len(decl->extern_.funcs); i++) {
                ExternFuncDecl *func = &decl->extern_.funcs[i];
                for (size_t j = 0; j < vec_len(func->params); j++) {
                    resolve_type_expr(ctx, func->params[j].type);
                }
                resolve_type_expr(ctx, func->return_type);
            }
            break;

        case DECL_IMPORT:
            /* TODO: Import resolution */
            break;

        case DECL_USE:
            /* TODO: Use resolution */
            break;

        case DECL_MODULE:
            /* TODO: Nested module resolution */
            break;
    }

    ctx->current_type_decl = NULL;
}

/*
 * Main entry point for name resolution
 */
bool sema_resolve_names(SemaContext *ctx, Module *mod) {
    ResolveContext rctx;
    memset(&rctx, 0, sizeof(rctx));
    rctx.arena = ctx->arena;
    rctx.diag = ctx->diag;
    rctx.strings = NULL;  /* TODO: pass string pool */

    /* Initialize scope context */
    /* Use existing string pool from somewhere */
    StringPool pool;
    string_pool_init(&pool);
    rctx.strings = &pool;
    scope_ctx_init(&rctx.scope_ctx, ctx->arena, rctx.strings);

    /* Initialize type context */
    type_ctx_init(&rctx.type_ctx, ctx->arena, rctx.strings);

    /* Create module scope */
    Scope *mod_scope = scope_enter(&rctx.scope_ctx, SCOPE_MODULE);
    mod_scope->module_name = mod->name;

    /* Phase 1: Register all top-level declarations */
    register_top_level_decls(&rctx, mod);

    /* Phase 2: Resolve all declaration bodies */
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        resolve_decl(&rctx, mod->decls[i]);
    }

    /* Store scope context for later phases - BEFORE exiting module scope */
    ctx->current_scope = mod_scope;  /* Module scope with all declarations */
    ctx->universe_scope = rctx.scope_ctx.universe;

    /* Note: Don't call scope_exit here - we want to preserve mod_scope for type checking */

    /* Return success if no errors */
    return !diag_has_errors(rctx.diag);
}
