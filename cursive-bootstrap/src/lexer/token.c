/*
 * Cursive Bootstrap Compiler - Token Implementation
 */

#include "token.h"

/* Token kind names for debugging */
static const char *token_names[TOK_COUNT] = {
    [TOK_INT_LIT] = "INT_LIT",
    [TOK_FLOAT_LIT] = "FLOAT_LIT",
    [TOK_STRING_LIT] = "STRING_LIT",
    [TOK_CHAR_LIT] = "CHAR_LIT",

    /* Keywords */
    [TOK_AND] = "and",
    [TOK_AS] = "as",
    [TOK_ASYNC] = "async",
    [TOK_ATOMIC] = "atomic",
    [TOK_BREAK] = "break",
    [TOK_CLASS] = "class",
    [TOK_COMPTIME] = "comptime",
    [TOK_CONST] = "const",
    [TOK_CONTINUE] = "continue",
    [TOK_DEFER] = "defer",
    [TOK_DISPATCH] = "dispatch",
    [TOK_DO] = "do",
    [TOK_DROP] = "drop",
    [TOK_ELSE] = "else",
    [TOK_EMIT] = "emit",
    [TOK_ENUM] = "enum",
    [TOK_ESCAPE] = "escape",
    [TOK_EXTERN] = "extern",
    [TOK_FALSE] = "false",
    [TOK_FOR] = "for",
    [TOK_GPU] = "gpu",
    [TOK_IF] = "if",
    [TOK_IMPORT] = "import",
    [TOK_IN] = "in",
    [TOK_INTERRUPT] = "interrupt",
    [TOK_LET] = "let",
    [TOK_LOOP] = "loop",
    [TOK_MATCH] = "match",
    [TOK_MODAL] = "modal",
    [TOK_MOD] = "mod",
    [TOK_MODULE] = "module",
    [TOK_MOVE] = "move",
    [TOK_MUT] = "mut",
    [TOK_OVERRIDE] = "override",
    [TOK_PARALLEL] = "parallel",
    [TOK_POOL] = "pool",
    [TOK_PRIVATE] = "private",
    [TOK_PROCEDURE] = "procedure",
    [TOK_PROTECTED] = "protected",
    [TOK_PUBLIC] = "public",
    [TOK_QUOTE] = "quote",
    [TOK_RECORD] = "record",
    [TOK_REGION] = "region",
    [TOK_RESULT] = "result",
    [TOK_RETURN] = "return",
    [TOK_SELECT] = "select",
    [TOK_SELF] = "self",
    [TOK_SELF_TYPE] = "Self",
    [TOK_SET] = "set",
    [TOK_SHARED] = "shared",
    [TOK_SIMD] = "simd",
    [TOK_SPAWN] = "spawn",
    [TOK_SYNC] = "sync",
    [TOK_THEN] = "then",
    [TOK_TRANSITION] = "transition",
    [TOK_TRANSMUTE] = "transmute",
    [TOK_TRUE] = "true",
    [TOK_TYPE] = "type",
    [TOK_UNION] = "union",
    [TOK_UNIQUE] = "unique",
    [TOK_UNSAFE] = "unsafe",
    [TOK_USING] = "using",
    [TOK_VAR] = "var",
    [TOK_VOLATILE] = "volatile",
    [TOK_WHERE] = "where",
    [TOK_WHILE] = "while",
    [TOK_WIDEN] = "widen",
    [TOK_YIELD] = "yield",

    /* Operators */
    [TOK_EQEQ] = "==",
    [TOK_NE] = "!=",
    [TOK_LE] = "<=",
    [TOK_GE] = ">=",
    [TOK_AMPAMP] = "&&",
    [TOK_PIPEPIPE] = "||",
    [TOK_LTLT] = "<<",
    [TOK_GTGT] = ">>",
    [TOK_LTLTEQ] = "<<=",
    [TOK_GTGTEQ] = ">>=",
    [TOK_DOTDOT] = "..",
    [TOK_DOTDOTEQ] = "..=",
    [TOK_FATARROW] = "=>",
    [TOK_ARROW] = "->",
    [TOK_STARSTAR] = "**",
    [TOK_COLONCOLON] = "::",
    [TOK_COLONEQ] = ":=",
    [TOK_PIPEEQ] = "|=",
    [TOK_TILDEGT] = "~>",
    [TOK_TILDEEXCL] = "~!",
    [TOK_TILDEPCT] = "~%",

    [TOK_PLUSEQ] = "+=",
    [TOK_MINUSEQ] = "-=",
    [TOK_STAREQ] = "*=",
    [TOK_SLASHEQ] = "/=",
    [TOK_PERCENTEQ] = "%=",
    [TOK_AMPEQ] = "&=",
    [TOK_CARETEQ] = "^=",

    [TOK_PLUS] = "+",
    [TOK_MINUS] = "-",
    [TOK_STAR] = "*",
    [TOK_SLASH] = "/",
    [TOK_PERCENT] = "%",
    [TOK_EQ] = "=",
    [TOK_LT] = "<",
    [TOK_GT] = ">",
    [TOK_BANG] = "!",
    [TOK_AMP] = "&",
    [TOK_PIPE] = "|",
    [TOK_CARET] = "^",
    [TOK_TILDE] = "~",
    [TOK_DOT] = ".",
    [TOK_QUESTION] = "?",
    [TOK_HASH] = "#",
    [TOK_AT] = "@",

    /* Punctuators */
    [TOK_LPAREN] = "(",
    [TOK_RPAREN] = ")",
    [TOK_LBRACKET] = "[",
    [TOK_RBRACKET] = "]",
    [TOK_LBRACE] = "{",
    [TOK_RBRACE] = "}",
    [TOK_COMMA] = ",",
    [TOK_COLON] = ":",
    [TOK_SEMI] = ";",

    /* Special */
    [TOK_IDENT] = "IDENT",
    [TOK_NEWLINE] = "NEWLINE",
    [TOK_EOF] = "EOF",
    [TOK_ERROR] = "ERROR",
};

const char *token_kind_name(TokenKind kind) {
    if (kind < 0 || kind >= TOK_COUNT) {
        return "<invalid>";
    }
    return token_names[kind] ? token_names[kind] : "<unknown>";
}

void token_print(const Token *tok, FILE *out) {
    fprintf(out, "%u:%u-%u:%u ",
            tok->span.start.line, tok->span.start.col,
            tok->span.end.line, tok->span.end.col);

    switch (tok->kind) {
        case TOK_INT_LIT:
            fprintf(out, "INT_LIT(%llu)", (unsigned long long)tok->value.int_val);
            break;
        case TOK_FLOAT_LIT:
            fprintf(out, "FLOAT_LIT(%g)", tok->value.float_val);
            break;
        case TOK_STRING_LIT:
            fprintf(out, "STRING_LIT(\"%.*s\")",
                    (int)tok->value.ident.len, tok->value.ident.data);
            break;
        case TOK_CHAR_LIT:
            if (tok->value.char_val < 128 && tok->value.char_val >= 32) {
                fprintf(out, "CHAR_LIT('%c')", (char)tok->value.char_val);
            } else {
                fprintf(out, "CHAR_LIT(U+%04X)", tok->value.char_val);
            }
            break;
        case TOK_IDENT:
            fprintf(out, "IDENT(%.*s)",
                    (int)tok->value.ident.len, tok->value.ident.data);
            break;
        default:
            fprintf(out, "%s", token_kind_name(tok->kind));
            break;
    }
}

bool token_is_keyword(TokenKind kind) {
    return kind >= TOK_AND && kind <= TOK_YIELD;
}

bool token_can_start_expr(TokenKind kind) {
    switch (kind) {
        /* Literals */
        case TOK_INT_LIT:
        case TOK_FLOAT_LIT:
        case TOK_STRING_LIT:
        case TOK_CHAR_LIT:
        case TOK_TRUE:
        case TOK_FALSE:
        /* Identifiers and keywords that start expressions */
        case TOK_IDENT:
        case TOK_SELF:
        case TOK_SELF_TYPE:
        /* Unary operators */
        case TOK_MINUS:
        case TOK_BANG:
        case TOK_AMP:
        case TOK_STAR:
        case TOK_TILDE:
        /* Grouping */
        case TOK_LPAREN:
        case TOK_LBRACKET:
        case TOK_LBRACE:
        /* Special expression starters */
        case TOK_IF:
        case TOK_MATCH:
        case TOK_LOOP:
        case TOK_MOVE:
        case TOK_WIDEN:
        case TOK_UNSAFE:
        case TOK_AT:        /* Modal state @State */
            return true;
        default:
            return false;
    }
}

bool token_can_start_stmt(TokenKind kind) {
    switch (kind) {
        case TOK_LET:
        case TOK_VAR:
        case TOK_IF:
        case TOK_LOOP:
        case TOK_WHILE:
        case TOK_FOR:
        case TOK_MATCH:
        case TOK_RETURN:
        case TOK_RESULT:
        case TOK_BREAK:
        case TOK_CONTINUE:
        case TOK_DEFER:
        case TOK_UNSAFE:
        case TOK_LBRACE:
            return true;
        default:
            /* Expression statements */
            return token_can_start_expr(kind);
    }
}

bool token_is_assignment(TokenKind kind) {
    switch (kind) {
        case TOK_EQ:
        case TOK_PLUSEQ:
        case TOK_MINUSEQ:
        case TOK_STAREQ:
        case TOK_SLASHEQ:
        case TOK_PERCENTEQ:
        case TOK_AMPEQ:
        case TOK_PIPEEQ:
        case TOK_CARETEQ:
        case TOK_LTLTEQ:
        case TOK_GTGTEQ:
            return true;
        default:
            return false;
    }
}

bool token_is_comparison(TokenKind kind) {
    switch (kind) {
        case TOK_EQEQ:
        case TOK_NE:
        case TOK_LT:
        case TOK_GT:
        case TOK_LE:
        case TOK_GE:
            return true;
        default:
            return false;
    }
}