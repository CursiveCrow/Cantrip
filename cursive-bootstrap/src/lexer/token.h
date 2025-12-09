/*
 * Cursive Bootstrap Compiler - Token Definitions
 *
 * Defines all token types according to Cursive Language Specification §2.
 */

#ifndef CURSIVE_TOKEN_H
#define CURSIVE_TOKEN_H

#include "common/common.h"
#include "common/string_pool.h"

/* Token kinds */
typedef enum TokenKind {
    /* === Literals === */
    TOK_INT_LIT,        /* Integer literal (decimal, hex, octal, binary) */
    TOK_FLOAT_LIT,      /* Floating-point literal */
    TOK_STRING_LIT,     /* String literal */
    TOK_CHAR_LIT,       /* Character literal */

    /* === Keywords (68 per §2.6) === */
    TOK_AND,            /* and */
    TOK_AS,             /* as */
    TOK_ASYNC,          /* async */
    TOK_ATOMIC,         /* atomic */
    TOK_BREAK,          /* break */
    TOK_CLASS,          /* class */
    TOK_COMPTIME,       /* comptime */
    TOK_CONST,          /* const */
    TOK_CONTINUE,       /* continue */
    TOK_DEFER,          /* defer */
    TOK_DISPATCH,       /* dispatch */
    TOK_DO,             /* do */
    TOK_DROP,           /* drop */
    TOK_ELSE,           /* else */
    TOK_EMIT,           /* emit */
    TOK_ENUM,           /* enum */
    TOK_ESCAPE,         /* escape */
    TOK_EXTERN,         /* extern */
    TOK_FALSE,          /* false */
    TOK_FOR,            /* for */
    TOK_GPU,            /* gpu */
    TOK_IF,             /* if */
    TOK_IMPORT,         /* import */
    TOK_IN,             /* in */
    TOK_INTERNAL,       /* internal */
    TOK_INTERRUPT,      /* interrupt */
    TOK_LET,            /* let */
    TOK_LOOP,           /* loop */
    TOK_MATCH,          /* match */
    TOK_MODAL,          /* modal */
    TOK_MOD,            /* mod */
    TOK_MODULE,         /* module */
    TOK_MOVE,           /* move */
    TOK_MUT,            /* mut */
    TOK_OVERRIDE,       /* override */
    TOK_PARALLEL,       /* parallel */
    TOK_POOL,           /* pool */
    TOK_PRIVATE,        /* private */
    TOK_PROCEDURE,      /* procedure */
    TOK_PROTECTED,      /* protected */
    TOK_PUBLIC,         /* public */
    TOK_QUOTE,          /* quote */
    TOK_RECORD,         /* record */
    TOK_REGION,         /* region */
    TOK_RESULT,         /* result */
    TOK_RETURN,         /* return */
    TOK_SELECT,         /* select */
    TOK_SELF,           /* self */
    TOK_SELF_TYPE,      /* Self (type) */
    TOK_SET,            /* set */
    TOK_SHARED,         /* shared */
    TOK_SIMD,           /* simd */
    TOK_SPAWN,          /* spawn */
    TOK_SYNC,           /* sync */
    TOK_THEN,           /* then */
    TOK_TRANSITION,     /* transition */
    TOK_TRANSMUTE,      /* transmute */
    TOK_TRUE,           /* true */
    TOK_TYPE,           /* type */
    TOK_UNION,          /* union */
    TOK_UNIQUE,         /* unique */
    TOK_UNSAFE,         /* unsafe */
    TOK_USING,          /* using */
    TOK_VAR,            /* var */
    TOK_VOLATILE,       /* volatile */
    TOK_WHERE,          /* where */
    TOK_WHILE,          /* while */
    TOK_WIDEN,          /* widen */
    TOK_YIELD,          /* yield */

    /* === Multi-character operators === */
    TOK_EQEQ,           /* == */
    TOK_NE,             /* != */
    TOK_LE,             /* <= */
    TOK_GE,             /* >= */
    TOK_AMPAMP,         /* && */
    TOK_PIPEPIPE,       /* || */
    TOK_LTLT,           /* << */
    TOK_GTGT,           /* >> */
    TOK_LTLTEQ,         /* <<= */
    TOK_GTGTEQ,         /* >>= */
    TOK_DOTDOT,         /* .. */
    TOK_DOTDOTEQ,       /* ..= */
    TOK_FATARROW,       /* => */
    TOK_ARROW,          /* -> */
    TOK_STARSTAR,       /* ** */
    TOK_COLONCOLON,     /* :: */
    TOK_COLONEQ,        /* := */
    TOK_PIPEEQ,         /* |= (precondition / compound assign) */
    TOK_TILDEGT,        /* ~> (method dispatch) */
    TOK_TILDEEXCL,      /* ~! (unique receiver) */
    TOK_TILDEPCT,       /* ~% (shared receiver) */

    /* === Compound assignment operators === */
    TOK_PLUSEQ,         /* += */
    TOK_MINUSEQ,        /* -= */
    TOK_STAREQ,         /* *= */
    TOK_SLASHEQ,        /* /= */
    TOK_PERCENTEQ,      /* %= */
    TOK_AMPEQ,          /* &= */
    TOK_CARETEQ,        /* ^= */

    /* === Single-character operators === */
    TOK_PLUS,           /* + */
    TOK_MINUS,          /* - */
    TOK_STAR,           /* * */
    TOK_SLASH,          /* / */
    TOK_PERCENT,        /* % */
    TOK_EQ,             /* = */
    TOK_LT,             /* < */
    TOK_GT,             /* > */
    TOK_BANG,           /* ! */
    TOK_AMP,            /* & */
    TOK_PIPE,           /* | */
    TOK_CARET,          /* ^ */
    TOK_TILDE,          /* ~ (also const receiver shorthand) */
    TOK_DOT,            /* . */
    TOK_QUESTION,       /* ? */
    TOK_HASH,           /* # (coarsening operator) */
    TOK_AT,             /* @ (state prefix) */

    /* === Punctuators === */
    TOK_LPAREN,         /* ( */
    TOK_RPAREN,         /* ) */
    TOK_LBRACKET,       /* [ */
    TOK_RBRACKET,       /* ] */
    TOK_LBRACE,         /* { */
    TOK_RBRACE,         /* } */
    TOK_COMMA,          /* , */
    TOK_COLON,          /* : */
    TOK_SEMI,           /* ; */

    /* === Special === */
    TOK_IDENT,          /* Identifier */
    TOK_NEWLINE,        /* Significant newline (for statement termination) */
    TOK_EOF,            /* End of file */
    TOK_ERROR,          /* Lexer error */

    TOK_COUNT           /* Number of token kinds */
} TokenKind;

/* Token value union */
typedef union TokenValue {
    InternedString ident;      /* For TOK_IDENT, TOK_STRING_LIT */
    uint64_t int_val;          /* For TOK_INT_LIT */
    double float_val;          /* For TOK_FLOAT_LIT */
    uint32_t char_val;         /* For TOK_CHAR_LIT (Unicode codepoint) */
} TokenValue;

/* Integer literal suffix (if any) */
typedef enum IntSuffix {
    INT_SUFFIX_NONE,
    INT_SUFFIX_I8,  INT_SUFFIX_I16, INT_SUFFIX_I32, INT_SUFFIX_I64, INT_SUFFIX_I128,
    INT_SUFFIX_U8,  INT_SUFFIX_U16, INT_SUFFIX_U32, INT_SUFFIX_U64, INT_SUFFIX_U128,
    INT_SUFFIX_ISIZE, INT_SUFFIX_USIZE
} IntSuffix;

/* Token structure */
typedef struct Token {
    TokenKind kind;
    SourceSpan span;
    TokenValue value;
    IntSuffix int_suffix;   /* For integer literals */
} Token;

/* Get string name of token kind */
const char *token_kind_name(TokenKind kind);

/* Get string representation of a token (for debugging) */
void token_print(const Token *tok, FILE *out);

/* Check if token is a keyword */
bool token_is_keyword(TokenKind kind);

/* Check if token can start an expression */
bool token_can_start_expr(TokenKind kind);

/* Check if token can start a statement */
bool token_can_start_stmt(TokenKind kind);

/* Check if token is an assignment operator */
bool token_is_assignment(TokenKind kind);

/* Check if token is a comparison operator */
bool token_is_comparison(TokenKind kind);

#endif /* CURSIVE_TOKEN_H */