/*
 * Cursive Bootstrap Compiler - Semantic Analysis Interface
 *
 * Combines name resolution, type checking, move analysis, and permission checking.
 * (Implementation in Stages 4-6)
 */

#ifndef CURSIVE_SEMA_H
#define CURSIVE_SEMA_H

#include "parser/ast.h"
#include "common/arena.h"
#include "common/error.h"
#include "common/map.h"
#include "common/string_pool.h"

/* Include scope and types headers */
#include "scope.h"
#include "types.h"

/*
 * Semantic Analysis Context
 */
typedef struct SemaContext {
    Arena *arena;           /* Memory for semantic info */
    DiagContext *diag;      /* Error reporting */
    StringPool *strings;    /* String interning */

    /* Scope management */
    Scope *current_scope;   /* Current lexical scope */
    Scope *universe_scope;  /* Built-in types */

    /* Type system */
    TypeContext type_ctx;   /* Type context */
    Map type_cache;         /* Canonical types */
} SemaContext;

/* Initialize semantic analysis context */
void sema_init(SemaContext *ctx, Arena *arena, DiagContext *diag, StringPool *strings);

/* Run full semantic analysis on a module */
bool sema_analyze(SemaContext *ctx, Module *mod);

/* Individual analysis phases (for testing) */
bool sema_resolve_names(SemaContext *ctx, Module *mod);
bool sema_check_types(SemaContext *ctx, Module *mod);
bool sema_analyze_moves(SemaContext *ctx, Module *mod);
bool sema_check_permissions(SemaContext *ctx, Module *mod);

#endif /* CURSIVE_SEMA_H */
