/*
 * Cursive Bootstrap Compiler - Lexer
 *
 * Tokenizes Cursive source code according to §2 of the specification.
 */

#ifndef CURSIVE_LEXER_H
#define CURSIVE_LEXER_H

#include "token.h"
#include "unicode.h"
#include "common/string_pool.h"
#include "common/error.h"

/* Lexer state */
typedef struct Lexer {
    /* Source text */
    const char *source;
    size_t source_len;

    /* Current position */
    size_t pos;          /* Byte offset in source */
    uint32_t line;       /* Current line (1-indexed) */
    uint32_t col;        /* Current column (1-indexed, in bytes) */
    size_t line_start;   /* Byte offset of current line start */

    /* File ID for source locations */
    uint32_t file_id;

    /* String interning */
    StringPool *strings;

    /* Error reporting */
    DiagContext *diag;

    /* Peeked token (for lookahead) */
    Token peeked;
    bool has_peeked;

    /* State for newline-as-semicolon */
    bool at_line_start;      /* Are we at the start of a line? */
    TokenKind last_token;    /* Last non-newline token emitted */
    int bracket_depth;       /* Nesting depth of (), [], {} - newlines dont terminate inside */
} Lexer;

/* Initialize a lexer */
void lexer_init(Lexer *lex, const char *source, size_t len,
                uint32_t file_id, StringPool *strings, DiagContext *diag);

/* Get the next token */
Token lexer_next(Lexer *lex);

/* Peek at the next token without consuming it */
Token lexer_peek(Lexer *lex);

/* Get current source location */
SourceLoc lexer_loc(const Lexer *lex);

/* Check if at end of file */
bool lexer_at_eof(const Lexer *lex);

#endif /* CURSIVE_LEXER_H */