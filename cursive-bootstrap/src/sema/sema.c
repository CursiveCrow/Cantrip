/*
 * Cursive Bootstrap Compiler - Semantic Analysis Main
 *
 * Entry points for semantic analysis phases.
 */

#include "sema.h"

/* Initialize semantic analysis context */
void sema_init(SemaContext *ctx, Arena *arena, DiagContext *diag, StringPool *strings) {
    memset(ctx, 0, sizeof(SemaContext));
    ctx->arena = arena;
    ctx->diag = diag;
    ctx->strings = strings;

    /* Initialize type context */
    type_ctx_init(&ctx->type_ctx, arena, strings);

    /* Initialize type cache */
    map_init(&ctx->type_cache);

    /* Scope context will be created during name resolution */
    ctx->current_scope = NULL;
    ctx->universe_scope = NULL;
}

/* Run full semantic analysis on a module */
bool sema_analyze(SemaContext *ctx, Module *mod) {
    /* Phase 1: Name resolution */
    if (!sema_resolve_names(ctx, mod)) {
        return false;
    }

    /* Phase 2: Type checking */
    if (!sema_check_types(ctx, mod)) {
        return false;
    }

    /* Phase 3: Move analysis */
    if (!sema_analyze_moves(ctx, mod)) {
        return false;
    }

    /* Phase 4: Permission checking */
    if (!sema_check_permissions(ctx, mod)) {
        return false;
    }

    return true;
}

/* Note: sema_check_types is implemented in typecheck.c */
/* Note: sema_analyze_moves is implemented in moves.c */
/* Note: sema_check_permissions is implemented in perms.c */
