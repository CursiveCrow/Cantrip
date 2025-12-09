/*
 * Cursive Bootstrap Compiler - Lexer Implementation
 */

#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* Keyword lookup table */
typedef struct Keyword {
    const char *name;
    TokenKind kind;
} Keyword;

static const Keyword keywords[] = {
    {"and", TOK_AND},
    {"as", TOK_AS},
    {"async", TOK_ASYNC},
    {"atomic", TOK_ATOMIC},
    {"break", TOK_BREAK},
    {"class", TOK_CLASS},
    {"comptime", TOK_COMPTIME},
    {"const", TOK_CONST},
    {"continue", TOK_CONTINUE},
    {"defer", TOK_DEFER},
    {"dispatch", TOK_DISPATCH},
    {"do", TOK_DO},
    {"drop", TOK_DROP},
    {"else", TOK_ELSE},
    {"emit", TOK_EMIT},
    {"enum", TOK_ENUM},
    {"escape", TOK_ESCAPE},
    {"extern", TOK_EXTERN},
    {"false", TOK_FALSE},
    {"for", TOK_FOR},
    {"gpu", TOK_GPU},
    {"if", TOK_IF},
    {"import", TOK_IMPORT},
    {"in", TOK_IN},
    {"internal", TOK_INTERNAL},
    {"interrupt", TOK_INTERRUPT},
    {"let", TOK_LET},
    {"loop", TOK_LOOP},
    {"match", TOK_MATCH},
    {"modal", TOK_MODAL},
    {"mod", TOK_MOD},
    {"module", TOK_MODULE},
    {"move", TOK_MOVE},
    {"mut", TOK_MUT},
    {"override", TOK_OVERRIDE},
    {"parallel", TOK_PARALLEL},
    {"pool", TOK_POOL},
    {"private", TOK_PRIVATE},
    {"procedure", TOK_PROCEDURE},
    {"protected", TOK_PROTECTED},
    {"public", TOK_PUBLIC},
    {"quote", TOK_QUOTE},
    {"record", TOK_RECORD},
    {"region", TOK_REGION},
    {"result", TOK_RESULT},
    {"return", TOK_RETURN},
    {"select", TOK_SELECT},
    {"self", TOK_SELF},
    {"Self", TOK_SELF_TYPE},
    {"set", TOK_SET},
    {"shared", TOK_SHARED},
    {"simd", TOK_SIMD},
    {"spawn", TOK_SPAWN},
    {"sync", TOK_SYNC},
    {"then", TOK_THEN},
    {"transition", TOK_TRANSITION},
    {"transmute", TOK_TRANSMUTE},
    {"true", TOK_TRUE},
    {"type", TOK_TYPE},
    {"union", TOK_UNION},
    {"unique", TOK_UNIQUE},
    {"unsafe", TOK_UNSAFE},
    {"using", TOK_USING},
    {"var", TOK_VAR},
    {"volatile", TOK_VOLATILE},
    {"where", TOK_WHERE},
    {"while", TOK_WHILE},
    {"widen", TOK_WIDEN},
    {"yield", TOK_YIELD},
    {NULL, TOK_ERROR} /* Sentinel */
};

void lexer_init(Lexer *lex, const char *source, size_t len,
                uint32_t file_id, StringPool *strings, DiagContext *diag) {
    lex->source = source;
    lex->source_len = len;
    lex->pos = 0;
    lex->line = 1;
    lex->col = 1;
    lex->line_start = 0;
    lex->file_id = file_id;
    lex->strings = strings;
    lex->diag = diag;
    lex->has_peeked = false;
    lex->at_line_start = true;
    lex->last_token = TOK_ERROR;
    lex->bracket_depth = 0;

    /* Skip UTF-8 BOM if present */
    if (len >= 3 &&
        (uint8_t)source[0] == 0xEF &&
        (uint8_t)source[1] == 0xBB &&
        (uint8_t)source[2] == 0xBF) {
        lex->pos = 3;
        lex->col = 4;
    }
}

/* Get current character (returns 0 at EOF) */
static inline char peek_char(Lexer *lex) {
    if (lex->pos >= lex->source_len) return '\0';
    return lex->source[lex->pos];
}

/* Get character at offset from current position */
static inline char peek_char_n(Lexer *lex, size_t n) {
    if (lex->pos + n >= lex->source_len) return '\0';
    return lex->source[lex->pos + n];
}

/* Advance by one byte */
static inline void advance(Lexer *lex) {
    if (lex->pos < lex->source_len) {
        if (lex->source[lex->pos] == '\n') {
            lex->line++;
            lex->col = 1;
            lex->line_start = lex->pos + 1;
        } else {
            lex->col++;
        }
        lex->pos++;
    }
}

/* Advance by n bytes */
static inline void advance_n(Lexer *lex, size_t n) {
    for (size_t i = 0; i < n; i++) {
        advance(lex);
    }
}

SourceLoc lexer_loc(const Lexer *lex) {
    return (SourceLoc){
        .file_id = lex->file_id,
        .line = lex->line,
        .col = lex->col
    };
}

bool lexer_at_eof(const Lexer *lex) {
    return lex->pos >= lex->source_len;
}

/* Skip whitespace and comments */
static void skip_whitespace(Lexer *lex) {
    while (lex->pos < lex->source_len) {
        char c = peek_char(lex);

        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lex);
            continue;
        }

        /* Check for comments */
        if (c == '/' && peek_char_n(lex, 1) == '/') {
            /* Line comment - skip to end of line */
            advance_n(lex, 2);
            while (!lexer_at_eof(lex) && peek_char(lex) != '\n') {
                advance(lex);
            }
            continue;
        }

        if (c == '/' && peek_char_n(lex, 1) == '*') {
            /* Block comment - handle nesting */
            advance_n(lex, 2);
            int depth = 1;
            SourceLoc start = lexer_loc(lex);

            while (!lexer_at_eof(lex) && depth > 0) {
                if (peek_char(lex) == '/' && peek_char_n(lex, 1) == '*') {
                    depth++;
                    advance_n(lex, 2);
                } else if (peek_char(lex) == '*' && peek_char_n(lex, 1) == '/') {
                    depth--;
                    advance_n(lex, 2);
                } else {
                    advance(lex);
                }
            }

            if (depth > 0) {
                diag_report(lex->diag, DIAG_ERROR, E_LEX_0005,
                           span_point(start), "Unterminated block comment");
            }
            continue;
        }

        break; /* Not whitespace or comment */
    }
}

/* Look up keyword */
static TokenKind lookup_keyword(const char *str, size_t len) {
    for (size_t i = 0; keywords[i].name != NULL; i++) {
        if (strlen(keywords[i].name) == len &&
            memcmp(keywords[i].name, str, len) == 0) {
            return keywords[i].kind;
        }
    }
    return TOK_IDENT;
}

/* Scan identifier or keyword */
static Token scan_ident(Lexer *lex) {
    SourceLoc start = lexer_loc(lex);
    size_t start_pos = lex->pos;

    /* First character already validated as XID_Start */
    size_t old_pos = lex->pos;
    utf8_decode(lex->source, lex->source_len, &lex->pos);
    lex->col += (uint32_t)(lex->pos - old_pos);

    /* Continue with XID_Continue characters */
    while (!lexer_at_eof(lex)) {
        size_t save_pos = lex->pos;
        uint32_t cp = utf8_decode(lex->source, lex->source_len, &lex->pos);
        if (!unicode_is_xid_continue(cp)) {
            lex->pos = save_pos; /* Put it back */
            break;
        }
        lex->col += (uint32_t)(lex->pos - save_pos);
    }

    size_t len = lex->pos - start_pos;
    const char *text = lex->source + start_pos;

    Token tok;
    tok.span.start = start;
    tok.span.end = lexer_loc(lex);

    /* Check for keyword */
    tok.kind = lookup_keyword(text, len);
    if (tok.kind == TOK_IDENT) {
        tok.value.ident = string_pool_intern_len(lex->strings, text, len);
    }

    return tok;
}

/* Scan numeric literal */
static Token scan_number(Lexer *lex) {
    SourceLoc start = lexer_loc(lex);
    size_t start_pos = lex->pos;
    bool is_float = false;
    int base = 10;

    /* Check for prefix */
    if (peek_char(lex) == '0') {
        char next = peek_char_n(lex, 1);
        if (next == 'x' || next == 'X') {
            base = 16;
            advance_n(lex, 2);
        } else if (next == 'o' || next == 'O') {
            base = 8;
            advance_n(lex, 2);
        } else if (next == 'b' || next == 'B') {
            base = 2;
            advance_n(lex, 2);
        }
    }

    size_t num_start = lex->pos;

    /* Scan digits */
    while (!lexer_at_eof(lex)) {
        char c = peek_char(lex);
        bool valid = false;

        if (c == '_') {
            advance(lex);
            continue; /* Skip underscores */
        }

        switch (base) {
            case 2:  valid = (c == '0' || c == '1'); break;
            case 8:  valid = (c >= '0' && c <= '7'); break;
            case 10: valid = (c >= '0' && c <= '9'); break;
            case 16: valid = is_hex_digit((uint32_t)c); break;
        }

        if (!valid) break;
        advance(lex);
    }

    /* Check for decimal point (only for base 10) */
    if (base == 10 && peek_char(lex) == '.' && peek_char_n(lex, 1) != '.') {
        /* Make sure it's not a method call like 123.method */
        char after_dot = peek_char_n(lex, 1);
        if (is_ascii_digit((uint32_t)after_dot)) {
            is_float = true;
            advance(lex); /* Skip '.' */

            /* Scan fractional part */
            while (!lexer_at_eof(lex)) {
                char c = peek_char(lex);
                if (c == '_') { advance(lex); continue; }
                if (!is_ascii_digit((uint32_t)c)) break;
                advance(lex);
            }
        }
    }

    /* Check for exponent (only for base 10) */
    if (base == 10) {
        char c = peek_char(lex);
        if (c == 'e' || c == 'E') {
            is_float = true;
            advance(lex);

            c = peek_char(lex);
            if (c == '+' || c == '-') {
                advance(lex);
            }

            while (!lexer_at_eof(lex)) {
                c = peek_char(lex);
                if (c == '_') { advance(lex); continue; }
                if (!is_ascii_digit((uint32_t)c)) break;
                advance(lex);
            }
        }
    }

    Token tok;
    tok.span.start = start;
    tok.span.end = lexer_loc(lex);
    tok.int_suffix = INT_SUFFIX_NONE;

    /* Parse suffix for integer types */
    if (!is_float) {
        const char *p = lex->source + lex->pos;
        size_t remaining = lex->source_len - lex->pos;

        #define CHECK_SUFFIX(s, suf) \
            if (remaining >= strlen(s) && memcmp(p, s, strlen(s)) == 0 && \
                (remaining == strlen(s) || !unicode_is_xid_continue((uint32_t)p[strlen(s)]))) { \
                tok.int_suffix = suf; \
                advance_n(lex, strlen(s)); \
            }

        CHECK_SUFFIX("i128", INT_SUFFIX_I128)
        else CHECK_SUFFIX("i64", INT_SUFFIX_I64)
        else CHECK_SUFFIX("i32", INT_SUFFIX_I32)
        else CHECK_SUFFIX("i16", INT_SUFFIX_I16)
        else CHECK_SUFFIX("i8", INT_SUFFIX_I8)
        else CHECK_SUFFIX("isize", INT_SUFFIX_ISIZE)
        else CHECK_SUFFIX("u128", INT_SUFFIX_U128)
        else CHECK_SUFFIX("u64", INT_SUFFIX_U64)
        else CHECK_SUFFIX("u32", INT_SUFFIX_U32)
        else CHECK_SUFFIX("u16", INT_SUFFIX_U16)
        else CHECK_SUFFIX("u8", INT_SUFFIX_U8)
        else CHECK_SUFFIX("usize", INT_SUFFIX_USIZE)

        #undef CHECK_SUFFIX
    }

    tok.span.end = lexer_loc(lex);

    /* Parse the number value */
    size_t num_len = lex->pos - num_start;
    char *num_buf = (char *)malloc(num_len + 1);
    size_t j = 0;
    for (size_t i = num_start; i < lex->pos && j < num_len; i++) {
        if (lex->source[i] != '_') {
            num_buf[j++] = lex->source[i];
        }
    }
    num_buf[j] = '\0';

    if (is_float) {
        tok.kind = TOK_FLOAT_LIT;
        tok.value.float_val = strtod(num_buf, NULL);
    } else {
        tok.kind = TOK_INT_LIT;
        tok.value.int_val = strtoull(num_buf, NULL, base);
    }

    free(num_buf);
    return tok;
}

/* Scan string literal */
static Token scan_string(Lexer *lex) {
    SourceLoc start = lexer_loc(lex);
    char quote = peek_char(lex);
    advance(lex); /* Skip opening quote */

    size_t buf_cap = 256;
    size_t buf_len = 0;
    char *buf = (char *)malloc(buf_cap);

    while (!lexer_at_eof(lex)) {
        char c = peek_char(lex);

        if (c == quote) {
            advance(lex);
            break;
        }

        if (c == '\n') {
            diag_report(lex->diag, DIAG_ERROR, E_LEX_0002,
                       span_point(start), "Unterminated string literal");
            break;
        }

        if (c == '\\') {
            advance(lex);
            if (lexer_at_eof(lex)) {
                diag_report(lex->diag, DIAG_ERROR, E_LEX_0003,
                           span_point(lexer_loc(lex)), "Unexpected end of file in escape sequence");
                break;
            }

            c = peek_char(lex);
            char escaped;
            switch (c) {
                case 'n':  escaped = '\n'; break;
                case 'r':  escaped = '\r'; break;
                case 't':  escaped = '\t'; break;
                case '\\': escaped = '\\'; break;
                case '\'': escaped = '\''; break;
                case '"':  escaped = '"'; break;
                case '0':  escaped = '\0'; break;
                default:
                    diag_report(lex->diag, DIAG_ERROR, E_LEX_0003,
                               span_point(lexer_loc(lex)),
                               "Unknown escape sequence '\\%c'", c);
                    escaped = c;
                    break;
            }
            advance(lex);

            /* Add to buffer */
            if (buf_len + 1 >= buf_cap) {
                buf_cap *= 2;
                buf = (char *)realloc(buf, buf_cap);
            }
            buf[buf_len++] = escaped;
        } else {
            /* Regular character */
            if (buf_len + 1 >= buf_cap) {
                buf_cap *= 2;
                buf = (char *)realloc(buf, buf_cap);
            }
            buf[buf_len++] = c;
            advance(lex);
        }
    }

    Token tok;
    tok.kind = TOK_STRING_LIT;
    tok.span.start = start;
    tok.span.end = lexer_loc(lex);
    tok.value.ident = string_pool_intern_len(lex->strings, buf, buf_len);

    free(buf);
    return tok;
}

/* Scan character literal */
static Token scan_char(Lexer *lex) {
    SourceLoc start = lexer_loc(lex);
    advance(lex); /* Skip opening quote */

    Token tok;
    tok.kind = TOK_CHAR_LIT;
    tok.span.start = start;

    if (lexer_at_eof(lex)) {
        diag_report(lex->diag, DIAG_ERROR, E_LEX_0002,
                   span_point(start), "Unterminated character literal");
        tok.value.char_val = 0;
        tok.span.end = lexer_loc(lex);
        return tok;
    }

    char c = peek_char(lex);
    uint32_t cp;

    if (c == '\\') {
        advance(lex);
        if (lexer_at_eof(lex)) {
            diag_report(lex->diag, DIAG_ERROR, E_LEX_0003,
                       span_point(lexer_loc(lex)), "Unexpected end of file in escape sequence");
            tok.value.char_val = 0;
            tok.span.end = lexer_loc(lex);
            return tok;
        }

        c = peek_char(lex);
        switch (c) {
            case 'n':  cp = '\n'; break;
            case 'r':  cp = '\r'; break;
            case 't':  cp = '\t'; break;
            case '\\': cp = '\\'; break;
            case '\'': cp = '\''; break;
            case '"':  cp = '"'; break;
            case '0':  cp = '\0'; break;
            default:
                diag_report(lex->diag, DIAG_ERROR, E_LEX_0003,
                           span_point(lexer_loc(lex)),
                           "Unknown escape sequence '\\%c'", c);
                cp = (uint32_t)c;
                break;
        }
        advance(lex);
    } else {
        /* Decode UTF-8 character */
        size_t save_pos = lex->pos;
        cp = utf8_decode(lex->source, lex->source_len, &lex->pos);
        lex->col += (uint32_t)(lex->pos - save_pos);
    }

    tok.value.char_val = cp;

    /* Expect closing quote */
    if (!lexer_at_eof(lex) && peek_char(lex) == '\'') {
        advance(lex);
    } else {
        diag_report(lex->diag, DIAG_ERROR, E_LEX_0002,
                   span_point(lexer_loc(lex)), "Unterminated character literal");
    }

    tok.span.end = lexer_loc(lex);
    return tok;
}

/* Make a simple token */
static Token make_token(Lexer *lex, TokenKind kind, size_t len) {
    SourceLoc start = lexer_loc(lex);
    advance_n(lex, len);

    Token tok;
    tok.kind = kind;
    tok.span.start = start;
    tok.span.end = lexer_loc(lex);
    memset(&tok.value, 0, sizeof(tok.value));
    tok.int_suffix = INT_SUFFIX_NONE;
    return tok;
}

/* Scan the next raw token */
static Token scan_token(Lexer *lex) {
    skip_whitespace(lex);

    if (lexer_at_eof(lex)) {
        return make_token(lex, TOK_EOF, 0);
    }

    char c = peek_char(lex);
    char c2 = peek_char_n(lex, 1);
    char c3 = peek_char_n(lex, 2);

    /* Newline handling */
    if (c == '\n') {
        Token tok = make_token(lex, TOK_NEWLINE, 1);
        lex->at_line_start = true;
        return tok;
    }

    lex->at_line_start = false;

    /* Identifiers and keywords */
    if (c == '_' || unicode_is_xid_start((uint32_t)(uint8_t)c)) {
        size_t save_pos = lex->pos;
        uint32_t cp = utf8_decode(lex->source, lex->source_len, &lex->pos);
        lex->pos = save_pos; /* Reset for scan_ident */

        if (unicode_is_xid_start(cp)) {
            return scan_ident(lex);
        }
    }

    /* Numbers */
    if (is_ascii_digit((uint32_t)c)) {
        return scan_number(lex);
    }

    /* String literals */
    if (c == '"') {
        return scan_string(lex);
    }

    /* Character literals */
    if (c == '\'') {
        return scan_char(lex);
    }

    /* Three-character operators */
    if (c == '<' && c2 == '<' && c3 == '=') return make_token(lex, TOK_LTLTEQ, 3);
    if (c == '>' && c2 == '>' && c3 == '=') return make_token(lex, TOK_GTGTEQ, 3);
    if (c == '.' && c2 == '.' && c3 == '=') return make_token(lex, TOK_DOTDOTEQ, 3);

    /* Two-character operators */
    if (c == '=' && c2 == '=') return make_token(lex, TOK_EQEQ, 2);
    if (c == '!' && c2 == '=') return make_token(lex, TOK_NE, 2);
    if (c == '<' && c2 == '=') return make_token(lex, TOK_LE, 2);
    if (c == '>' && c2 == '=') return make_token(lex, TOK_GE, 2);
    if (c == '&' && c2 == '&') return make_token(lex, TOK_AMPAMP, 2);
    if (c == '|' && c2 == '|') return make_token(lex, TOK_PIPEPIPE, 2);
    if (c == '<' && c2 == '<') return make_token(lex, TOK_LTLT, 2);
    if (c == '>' && c2 == '>') return make_token(lex, TOK_GTGT, 2);
    if (c == '.' && c2 == '.') return make_token(lex, TOK_DOTDOT, 2);
    if (c == '=' && c2 == '>') return make_token(lex, TOK_FATARROW, 2);
    if (c == '-' && c2 == '>') return make_token(lex, TOK_ARROW, 2);
    if (c == '*' && c2 == '*') return make_token(lex, TOK_STARSTAR, 2);
    if (c == ':' && c2 == ':') return make_token(lex, TOK_COLONCOLON, 2);
    if (c == ':' && c2 == '=') return make_token(lex, TOK_COLONEQ, 2);
    if (c == '|' && c2 == '=') return make_token(lex, TOK_PIPEEQ, 2);
    if (c == '~' && c2 == '>') return make_token(lex, TOK_TILDEGT, 2);
    if (c == '~' && c2 == '!') return make_token(lex, TOK_TILDEEXCL, 2);
    if (c == '~' && c2 == '%') return make_token(lex, TOK_TILDEPCT, 2);
    if (c == '+' && c2 == '=') return make_token(lex, TOK_PLUSEQ, 2);
    if (c == '-' && c2 == '=') return make_token(lex, TOK_MINUSEQ, 2);
    if (c == '*' && c2 == '=') return make_token(lex, TOK_STAREQ, 2);
    if (c == '/' && c2 == '=') return make_token(lex, TOK_SLASHEQ, 2);
    if (c == '%' && c2 == '=') return make_token(lex, TOK_PERCENTEQ, 2);
    if (c == '&' && c2 == '=') return make_token(lex, TOK_AMPEQ, 2);
    if (c == '^' && c2 == '=') return make_token(lex, TOK_CARETEQ, 2);

    /* Single-character tokens */
    switch (c) {
        case '+': return make_token(lex, TOK_PLUS, 1);
        case '-': return make_token(lex, TOK_MINUS, 1);
        case '*': return make_token(lex, TOK_STAR, 1);
        case '/': return make_token(lex, TOK_SLASH, 1);
        case '%': return make_token(lex, TOK_PERCENT, 1);
        case '=': return make_token(lex, TOK_EQ, 1);
        case '<': return make_token(lex, TOK_LT, 1);
        case '>': return make_token(lex, TOK_GT, 1);
        case '!': return make_token(lex, TOK_BANG, 1);
        case '&': return make_token(lex, TOK_AMP, 1);
        case '|': return make_token(lex, TOK_PIPE, 1);
        case '^': return make_token(lex, TOK_CARET, 1);
        case '~': return make_token(lex, TOK_TILDE, 1);
        case '.': return make_token(lex, TOK_DOT, 1);
        case '?': return make_token(lex, TOK_QUESTION, 1);
        case '#': return make_token(lex, TOK_HASH, 1);
        case '@': return make_token(lex, TOK_AT, 1);
        case '(': return make_token(lex, TOK_LPAREN, 1);
        case ')': return make_token(lex, TOK_RPAREN, 1);
        case '[': return make_token(lex, TOK_LBRACKET, 1);
        case ']': return make_token(lex, TOK_RBRACKET, 1);
        case '{': return make_token(lex, TOK_LBRACE, 1);
        case '}': return make_token(lex, TOK_RBRACE, 1);
        case ',': return make_token(lex, TOK_COMMA, 1);
        case ':': return make_token(lex, TOK_COLON, 1);
        case ';': return make_token(lex, TOK_SEMI, 1);
    }

    /* Invalid character */
    SourceLoc loc = lexer_loc(lex);
    diag_report(lex->diag, DIAG_ERROR, E_LEX_0001,
               span_point(loc), "Invalid character '%c' (0x%02X)", c, (uint8_t)c);
    advance(lex);

    Token tok;
    tok.kind = TOK_ERROR;
    tok.span.start = loc;
    tok.span.end = lexer_loc(lex);
    return tok;
}

/* Should this token be followed by an automatic semicolon?
 * Per §2.11, newline terminates statements but not declarations.
 * We exclude TOK_RBRACE because:
 *   - After declaration bodies (procedure, record, etc.), no semicolon needed
 *   - After block expressions, the containing statement handles termination
 *   - After match arms with blocks, the match itself handles termination
 */
static bool needs_semicolon_after(TokenKind kind) {
    switch (kind) {
        case TOK_IDENT:
        case TOK_INT_LIT:
        case TOK_FLOAT_LIT:
        case TOK_STRING_LIT:
        case TOK_CHAR_LIT:
        case TOK_TRUE:
        case TOK_FALSE:
        case TOK_SELF:
        case TOK_SELF_TYPE:
        case TOK_BREAK:
        case TOK_CONTINUE:
        case TOK_RETURN:
        case TOK_RESULT:
        case TOK_RPAREN:
        case TOK_RBRACKET:
        /* Note: TOK_RBRACE excluded - blocks don't need implicit semicolons */
            return true;
        default:
            return false;
    }
}

Token lexer_next(Lexer *lex) {
    if (lex->has_peeked) {
        lex->has_peeked = false;
        lex->last_token = lex->peeked.kind;
        return lex->peeked;
    }

    for (;;) {
        Token tok = scan_token(lex);

        /* Track bracket depth for §2.11 Open Delimiter rule */
        switch (tok.kind) {
            case TOK_LPAREN:
            case TOK_LBRACKET:
            case TOK_LBRACE:
                lex->bracket_depth++;
                break;
            case TOK_RPAREN:
            case TOK_RBRACKET:
            case TOK_RBRACE:
                if (lex->bracket_depth > 0) lex->bracket_depth--;
                break;
            default:
                break;
        }

        /* Handle newline-as-semicolon per §2.11 */
        if (tok.kind == TOK_NEWLINE) {
            /* §2.11: newlines inside (), [], {} do NOT terminate statements */
            if (lex->bracket_depth == 0 && needs_semicolon_after(lex->last_token)) {
                tok.kind = TOK_SEMI;
                lex->last_token = TOK_SEMI;
                return tok;
            }
            /* Skip non-significant newlines */
            continue;
        }

        lex->last_token = tok.kind;
        return tok;
    }
}

Token lexer_peek(Lexer *lex) {
    if (!lex->has_peeked) {
        lex->peeked = lexer_next(lex);
        lex->has_peeked = true;
    }
    return lex->peeked;
}