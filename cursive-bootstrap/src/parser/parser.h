/*
 * Cursive Bootstrap Compiler - Parser Interface
 *
 * Recursive descent parser for Cursive syntax.
 * (Implementation in Stage 3)
 */

#ifndef CURSIVE_PARSER_H
#define CURSIVE_PARSER_H

#include "ast.h"
#include "lexer/lexer.h"
#include "common/arena.h"
#include "common/error.h"

typedef struct Parser {
    Lexer *lexer;
    Token current;
    Token peek;
    bool has_peek;
    Arena *ast_arena;
    DiagContext *diag;
} Parser;

/* Initialize parser */
void parser_init(Parser *p, Lexer *lexer, Arena *arena, DiagContext *diag);

/* Parse a complete module */
Module *parse_module(Parser *p);

/* Parse individual declarations (for testing) */
Decl *parse_decl(Parser *p);
Expr *parse_expr(Parser *p);
TypeExpr *parse_type(Parser *p);
Pattern *parse_pattern(Parser *p);

#endif /* CURSIVE_PARSER_H */
