/*
 * Cursive Bootstrap Compiler - AST Construction Helpers
 */

#include "ast.h"
#include "common/arena.h"

Module *ast_new_module(Arena *arena) {
    Module *mod = ARENA_ALLOC(arena, Module);
    memset(mod, 0, sizeof(Module));
    mod->decls = vec_new(Decl *);
    return mod;
}

Decl *ast_new_decl(Arena *arena, DeclKind kind, SourceSpan span) {
    Decl *decl = ARENA_ALLOC(arena, Decl);
    memset(decl, 0, sizeof(Decl));
    decl->kind = kind;
    decl->span = span;
    return decl;
}

Expr *ast_new_expr(Arena *arena, ExprKind kind, SourceSpan span) {
    Expr *expr = ARENA_ALLOC(arena, Expr);
    memset(expr, 0, sizeof(Expr));
    expr->kind = kind;
    expr->span = span;
    return expr;
}

Stmt *ast_new_stmt(Arena *arena, StmtKind kind, SourceSpan span) {
    Stmt *stmt = ARENA_ALLOC(arena, Stmt);
    memset(stmt, 0, sizeof(Stmt));
    stmt->kind = kind;
    stmt->span = span;
    return stmt;
}

TypeExpr *ast_new_type(Arena *arena, TypeExprKind kind, SourceSpan span) {
    TypeExpr *type = ARENA_ALLOC(arena, TypeExpr);
    memset(type, 0, sizeof(TypeExpr));
    type->kind = kind;
    type->span = span;
    type->perm = PERM_CONST;  /* Default permission */
    return type;
}

Pattern *ast_new_pattern(Arena *arena, PatternKind kind, SourceSpan span) {
    Pattern *pat = ARENA_ALLOC(arena, Pattern);
    memset(pat, 0, sizeof(Pattern));
    pat->kind = kind;
    pat->span = span;
    return pat;
}
