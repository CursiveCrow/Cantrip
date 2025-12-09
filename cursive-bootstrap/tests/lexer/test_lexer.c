/*
 * Cursive Bootstrap Compiler - Lexer Tests
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common/arena.h"
#include "common/string_pool.h"
#include "common/error.h"
#include "lexer/lexer.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    static void test_##name(void); \
    static void run_test_##name(void) { \
        tests_run++; \
        printf("  Testing: %s ... ", #name); \
        test_##name(); \
        tests_passed++; \
        printf("PASS\n"); \
    } \
    static void test_##name(void)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            printf("FAIL\n    Assertion failed: %s\n    at %s:%d\n", \
                   #cond, __FILE__, __LINE__); \
            exit(1); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_STR_EQ(a, b) ASSERT(strcmp((a), (b)) == 0)

/* Helper to tokenize a string and return first token */
static Token tokenize_one(const char *source, Arena *arena, StringPool *pool, DiagContext *diag) {
    Lexer lex;
    lexer_init(&lex, source, strlen(source), 0, pool, diag);
    return lexer_next(&lex);
}

/* Helper to tokenize and return all tokens */
static void tokenize_all(const char *source, Token *tokens, size_t max_tokens, size_t *count,
                         Arena *arena, StringPool *pool, DiagContext *diag) {
    Lexer lex;
    lexer_init(&lex, source, strlen(source), 0, pool, diag);

    *count = 0;
    while (*count < max_tokens) {
        Token tok = lexer_next(&lex);
        tokens[(*count)++] = tok;
        if (tok.kind == TOK_EOF) break;
    }
}

/* ============================================ */
/* Test cases                                   */
/* ============================================ */

TEST(empty_source) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok = tokenize_one("", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_EOF);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(keywords) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    /* Test a selection of keywords */
    ASSERT_EQ(tokenize_one("let", &arena, &pool, &diag).kind, TOK_LET);
    ASSERT_EQ(tokenize_one("var", &arena, &pool, &diag).kind, TOK_VAR);
    ASSERT_EQ(tokenize_one("procedure", &arena, &pool, &diag).kind, TOK_PROCEDURE);
    ASSERT_EQ(tokenize_one("record", &arena, &pool, &diag).kind, TOK_RECORD);
    ASSERT_EQ(tokenize_one("enum", &arena, &pool, &diag).kind, TOK_ENUM);
    ASSERT_EQ(tokenize_one("modal", &arena, &pool, &diag).kind, TOK_MODAL);
    ASSERT_EQ(tokenize_one("class", &arena, &pool, &diag).kind, TOK_CLASS);
    ASSERT_EQ(tokenize_one("if", &arena, &pool, &diag).kind, TOK_IF);
    ASSERT_EQ(tokenize_one("else", &arena, &pool, &diag).kind, TOK_ELSE);
    ASSERT_EQ(tokenize_one("match", &arena, &pool, &diag).kind, TOK_MATCH);
    ASSERT_EQ(tokenize_one("loop", &arena, &pool, &diag).kind, TOK_LOOP);
    ASSERT_EQ(tokenize_one("break", &arena, &pool, &diag).kind, TOK_BREAK);
    ASSERT_EQ(tokenize_one("continue", &arena, &pool, &diag).kind, TOK_CONTINUE);
    ASSERT_EQ(tokenize_one("return", &arena, &pool, &diag).kind, TOK_RETURN);
    ASSERT_EQ(tokenize_one("result", &arena, &pool, &diag).kind, TOK_RESULT);
    ASSERT_EQ(tokenize_one("move", &arena, &pool, &diag).kind, TOK_MOVE);
    ASSERT_EQ(tokenize_one("const", &arena, &pool, &diag).kind, TOK_CONST);
    ASSERT_EQ(tokenize_one("unique", &arena, &pool, &diag).kind, TOK_UNIQUE);
    ASSERT_EQ(tokenize_one("shared", &arena, &pool, &diag).kind, TOK_SHARED);
    ASSERT_EQ(tokenize_one("unsafe", &arena, &pool, &diag).kind, TOK_UNSAFE);
    ASSERT_EQ(tokenize_one("transition", &arena, &pool, &diag).kind, TOK_TRANSITION);
    ASSERT_EQ(tokenize_one("parallel", &arena, &pool, &diag).kind, TOK_PARALLEL);
    ASSERT_EQ(tokenize_one("spawn", &arena, &pool, &diag).kind, TOK_SPAWN);
    ASSERT_EQ(tokenize_one("true", &arena, &pool, &diag).kind, TOK_TRUE);
    ASSERT_EQ(tokenize_one("false", &arena, &pool, &diag).kind, TOK_FALSE);
    ASSERT_EQ(tokenize_one("self", &arena, &pool, &diag).kind, TOK_SELF);
    ASSERT_EQ(tokenize_one("Self", &arena, &pool, &diag).kind, TOK_SELF_TYPE);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(identifiers) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    tok = tokenize_one("foo", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_IDENT);
    ASSERT_STR_EQ(tok.value.ident.data, "foo");

    tok = tokenize_one("_underscore", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_IDENT);
    ASSERT_STR_EQ(tok.value.ident.data, "_underscore");

    tok = tokenize_one("camelCase", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_IDENT);
    ASSERT_STR_EQ(tok.value.ident.data, "camelCase");

    tok = tokenize_one("snake_case", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_IDENT);
    ASSERT_STR_EQ(tok.value.ident.data, "snake_case");

    tok = tokenize_one("x123", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_IDENT);
    ASSERT_STR_EQ(tok.value.ident.data, "x123");

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(integer_literals) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    /* Decimal */
    tok = tokenize_one("0", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 0);

    tok = tokenize_one("42", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);

    tok = tokenize_one("1_000_000", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 1000000);

    /* Hexadecimal */
    tok = tokenize_one("0xFF", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 255);

    tok = tokenize_one("0xDEAD_BEEF", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 0xDEADBEEF);

    /* Octal */
    tok = tokenize_one("0o777", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 511);

    /* Binary */
    tok = tokenize_one("0b1010", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 10);

    tok = tokenize_one("0b1111_0000", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 240);

    /* With type suffixes */
    tok = tokenize_one("42i32", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);
    ASSERT_EQ(tok.int_suffix, INT_SUFFIX_I32);

    tok = tokenize_one("255u8", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 255);
    ASSERT_EQ(tok.int_suffix, INT_SUFFIX_U8);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(float_literals) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    tok = tokenize_one("3.14", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_FLOAT_LIT);
    ASSERT(tok.value.float_val > 3.13 && tok.value.float_val < 3.15);

    tok = tokenize_one("1.0", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_FLOAT_LIT);
    ASSERT(tok.value.float_val > 0.99 && tok.value.float_val < 1.01);

    tok = tokenize_one("1e10", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_FLOAT_LIT);
    ASSERT(tok.value.float_val > 9e9 && tok.value.float_val < 2e10);

    tok = tokenize_one("1.5e-3", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_FLOAT_LIT);
    ASSERT(tok.value.float_val > 1e-3 && tok.value.float_val < 2e-3);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(string_literals) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    tok = tokenize_one("\"hello\"", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_STRING_LIT);
    ASSERT_STR_EQ(tok.value.ident.data, "hello");

    tok = tokenize_one("\"hello world\"", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_STRING_LIT);
    ASSERT_STR_EQ(tok.value.ident.data, "hello world");

    tok = tokenize_one("\"with\\nnewline\"", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_STRING_LIT);
    ASSERT_STR_EQ(tok.value.ident.data, "with\nnewline");

    tok = tokenize_one("\"tab\\there\"", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_STRING_LIT);
    ASSERT_STR_EQ(tok.value.ident.data, "tab\there");

    tok = tokenize_one("\"quote\\\"here\"", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_STRING_LIT);
    ASSERT_STR_EQ(tok.value.ident.data, "quote\"here");

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(char_literals) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    tok = tokenize_one("'a'", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_CHAR_LIT);
    ASSERT_EQ(tok.value.char_val, 'a');

    tok = tokenize_one("'\\n'", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_CHAR_LIT);
    ASSERT_EQ(tok.value.char_val, '\n');

    tok = tokenize_one("'\\t'", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_CHAR_LIT);
    ASSERT_EQ(tok.value.char_val, '\t');

    tok = tokenize_one("'\\''", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_CHAR_LIT);
    ASSERT_EQ(tok.value.char_val, '\'');

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(operators) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    /* Single-character operators */
    ASSERT_EQ(tokenize_one("+", &arena, &pool, &diag).kind, TOK_PLUS);
    ASSERT_EQ(tokenize_one("-", &arena, &pool, &diag).kind, TOK_MINUS);
    ASSERT_EQ(tokenize_one("*", &arena, &pool, &diag).kind, TOK_STAR);
    ASSERT_EQ(tokenize_one("/", &arena, &pool, &diag).kind, TOK_SLASH);
    ASSERT_EQ(tokenize_one("%", &arena, &pool, &diag).kind, TOK_PERCENT);
    ASSERT_EQ(tokenize_one("=", &arena, &pool, &diag).kind, TOK_EQ);
    ASSERT_EQ(tokenize_one("<", &arena, &pool, &diag).kind, TOK_LT);
    ASSERT_EQ(tokenize_one(">", &arena, &pool, &diag).kind, TOK_GT);
    ASSERT_EQ(tokenize_one("!", &arena, &pool, &diag).kind, TOK_BANG);
    ASSERT_EQ(tokenize_one("&", &arena, &pool, &diag).kind, TOK_AMP);
    ASSERT_EQ(tokenize_one("|", &arena, &pool, &diag).kind, TOK_PIPE);
    ASSERT_EQ(tokenize_one("^", &arena, &pool, &diag).kind, TOK_CARET);
    ASSERT_EQ(tokenize_one("~", &arena, &pool, &diag).kind, TOK_TILDE);
    ASSERT_EQ(tokenize_one(".", &arena, &pool, &diag).kind, TOK_DOT);
    ASSERT_EQ(tokenize_one("?", &arena, &pool, &diag).kind, TOK_QUESTION);
    ASSERT_EQ(tokenize_one("#", &arena, &pool, &diag).kind, TOK_HASH);
    ASSERT_EQ(tokenize_one("@", &arena, &pool, &diag).kind, TOK_AT);

    /* Two-character operators */
    ASSERT_EQ(tokenize_one("==", &arena, &pool, &diag).kind, TOK_EQEQ);
    ASSERT_EQ(tokenize_one("!=", &arena, &pool, &diag).kind, TOK_NE);
    ASSERT_EQ(tokenize_one("<=", &arena, &pool, &diag).kind, TOK_LE);
    ASSERT_EQ(tokenize_one(">=", &arena, &pool, &diag).kind, TOK_GE);
    ASSERT_EQ(tokenize_one("&&", &arena, &pool, &diag).kind, TOK_AMPAMP);
    ASSERT_EQ(tokenize_one("||", &arena, &pool, &diag).kind, TOK_PIPEPIPE);
    ASSERT_EQ(tokenize_one("<<", &arena, &pool, &diag).kind, TOK_LTLT);
    ASSERT_EQ(tokenize_one(">>", &arena, &pool, &diag).kind, TOK_GTGT);
    ASSERT_EQ(tokenize_one("..", &arena, &pool, &diag).kind, TOK_DOTDOT);
    ASSERT_EQ(tokenize_one("=>", &arena, &pool, &diag).kind, TOK_FATARROW);
    ASSERT_EQ(tokenize_one("->", &arena, &pool, &diag).kind, TOK_ARROW);
    ASSERT_EQ(tokenize_one("**", &arena, &pool, &diag).kind, TOK_STARSTAR);
    ASSERT_EQ(tokenize_one("::", &arena, &pool, &diag).kind, TOK_COLONCOLON);
    ASSERT_EQ(tokenize_one(":=", &arena, &pool, &diag).kind, TOK_COLONEQ);
    ASSERT_EQ(tokenize_one("|=", &arena, &pool, &diag).kind, TOK_PIPEEQ);

    /* Receiver operators */
    ASSERT_EQ(tokenize_one("~>", &arena, &pool, &diag).kind, TOK_TILDEGT);
    ASSERT_EQ(tokenize_one("~!", &arena, &pool, &diag).kind, TOK_TILDEEXCL);
    ASSERT_EQ(tokenize_one("~%", &arena, &pool, &diag).kind, TOK_TILDEPCT);

    /* Compound assignment */
    ASSERT_EQ(tokenize_one("+=", &arena, &pool, &diag).kind, TOK_PLUSEQ);
    ASSERT_EQ(tokenize_one("-=", &arena, &pool, &diag).kind, TOK_MINUSEQ);
    ASSERT_EQ(tokenize_one("*=", &arena, &pool, &diag).kind, TOK_STAREQ);
    ASSERT_EQ(tokenize_one("/=", &arena, &pool, &diag).kind, TOK_SLASHEQ);
    ASSERT_EQ(tokenize_one("%=", &arena, &pool, &diag).kind, TOK_PERCENTEQ);
    ASSERT_EQ(tokenize_one("&=", &arena, &pool, &diag).kind, TOK_AMPEQ);
    ASSERT_EQ(tokenize_one("^=", &arena, &pool, &diag).kind, TOK_CARETEQ);

    /* Three-character operators */
    ASSERT_EQ(tokenize_one("<<=", &arena, &pool, &diag).kind, TOK_LTLTEQ);
    ASSERT_EQ(tokenize_one(">>=", &arena, &pool, &diag).kind, TOK_GTGTEQ);
    ASSERT_EQ(tokenize_one("..=", &arena, &pool, &diag).kind, TOK_DOTDOTEQ);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(punctuators) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    ASSERT_EQ(tokenize_one("(", &arena, &pool, &diag).kind, TOK_LPAREN);
    ASSERT_EQ(tokenize_one(")", &arena, &pool, &diag).kind, TOK_RPAREN);
    ASSERT_EQ(tokenize_one("[", &arena, &pool, &diag).kind, TOK_LBRACKET);
    ASSERT_EQ(tokenize_one("]", &arena, &pool, &diag).kind, TOK_RBRACKET);
    ASSERT_EQ(tokenize_one("{", &arena, &pool, &diag).kind, TOK_LBRACE);
    ASSERT_EQ(tokenize_one("}", &arena, &pool, &diag).kind, TOK_RBRACE);
    ASSERT_EQ(tokenize_one(",", &arena, &pool, &diag).kind, TOK_COMMA);
    ASSERT_EQ(tokenize_one(":", &arena, &pool, &diag).kind, TOK_COLON);
    ASSERT_EQ(tokenize_one(";", &arena, &pool, &diag).kind, TOK_SEMI);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(comments) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tok;

    /* Line comment should be skipped */
    tok = tokenize_one("// this is a comment\n42", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);

    /* Block comment should be skipped */
    tok = tokenize_one("/* comment */ 42", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);

    /* Nested block comments */
    tok = tokenize_one("/* outer /* inner */ still outer */ 42", &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(newline_semicolon) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tokens[16];
    size_t count;

    /* Newline after identifier should insert semicolon */
    tokenize_all("foo\nbar", tokens, 16, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_IDENT);
    ASSERT_EQ(tokens[1].kind, TOK_SEMI);  /* Auto-inserted */
    ASSERT_EQ(tokens[2].kind, TOK_IDENT);

    /* Newline after number should insert semicolon */
    tokenize_all("42\n0", tokens, 16, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_INT_LIT);
    ASSERT_EQ(tokens[1].kind, TOK_SEMI);
    ASSERT_EQ(tokens[2].kind, TOK_INT_LIT);

    /* Newline after operator should NOT insert semicolon */
    tokenize_all("x +\ny", tokens, 16, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_IDENT);
    ASSERT_EQ(tokens[1].kind, TOK_PLUS);
    ASSERT_EQ(tokens[2].kind, TOK_IDENT);
    

    /* Newline after closing paren/bracket/brace should insert semicolon */
    tokenize_all(")\nfoo", tokens, 16, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_RPAREN);
    ASSERT_EQ(tokens[1].kind, TOK_SEMI);
    ASSERT_EQ(tokens[2].kind, TOK_IDENT);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(multi_token_sequence) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    Token tokens[32];
    size_t count;

    /* A simple let binding */
    tokenize_all("let x = 42", tokens, 32, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_LET);
    ASSERT_EQ(tokens[1].kind, TOK_IDENT);
    ASSERT_EQ(tokens[2].kind, TOK_EQ);
    ASSERT_EQ(tokens[3].kind, TOK_INT_LIT);
    ASSERT_EQ(tokens[4].kind, TOK_EOF);

    /* Method dispatch */
    tokenize_all("obj~>method()", tokens, 32, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_IDENT);
    ASSERT_EQ(tokens[1].kind, TOK_TILDEGT);
    ASSERT_EQ(tokens[2].kind, TOK_IDENT);
    ASSERT_EQ(tokens[3].kind, TOK_LPAREN);
    ASSERT_EQ(tokens[4].kind, TOK_RPAREN);
    ASSERT_EQ(tokens[5].kind, TOK_EOF);

    /* Modal type annotation */
    tokenize_all("Ptr<T>@Valid", tokens, 32, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_IDENT); /* Ptr */
    ASSERT_EQ(tokens[1].kind, TOK_LT);
    ASSERT_EQ(tokens[2].kind, TOK_IDENT); /* T */
    ASSERT_EQ(tokens[3].kind, TOK_GT);
    ASSERT_EQ(tokens[4].kind, TOK_AT);
    ASSERT_EQ(tokens[5].kind, TOK_IDENT); /* Valid */

    /* Immovable binding operator */
    tokenize_all("let x := value", tokens, 32, &count, &arena, &pool, &diag);
    ASSERT_EQ(tokens[0].kind, TOK_LET);
    ASSERT_EQ(tokens[1].kind, TOK_IDENT);
    ASSERT_EQ(tokens[2].kind, TOK_COLONEQ);
    

    arena_destroy(&arena);
    diag_destroy(&diag);
}

TEST(bom_handling) {
    Arena arena;
    StringPool pool;
    DiagContext diag;

    arena_init(&arena);
    string_pool_init(&pool);
    diag_init(&diag);

    /* UTF-8 BOM followed by code */
    const char source_with_bom[] = "\xEF\xBB\xBF" "42";

    Token tok = tokenize_one(source_with_bom, &arena, &pool, &diag);
    ASSERT_EQ(tok.kind, TOK_INT_LIT);
    ASSERT_EQ(tok.value.int_val, 42);

    arena_destroy(&arena);
    diag_destroy(&diag);
}

/* ============================================ */
/* Main test runner                             */
/* ============================================ */

int main(void) {
    printf("Running lexer tests...\n\n");

    run_test_empty_source();
    run_test_keywords();
    run_test_identifiers();
    run_test_integer_literals();
    run_test_float_literals();
    run_test_string_literals();
    run_test_char_literals();
    run_test_operators();
    run_test_punctuators();
    run_test_comments();
    run_test_newline_semicolon();
    run_test_multi_token_sequence();
    run_test_bom_handling();

    printf("\n%d/%d tests passed.\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
