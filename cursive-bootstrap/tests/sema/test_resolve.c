/*
 * Cursive Bootstrap Compiler - Name Resolution Tests
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "common/arena.h"
#include "common/string_pool.h"
#include "common/error.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "sema/sema.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("  Testing: %s... ", #name); \
    tests_run++; \
    if (test_##name()) { \
        printf("PASSED\n"); \
        tests_passed++; \
    } else { \
        printf("FAILED\n"); \
    } \
} while (0)

/* Helper to parse and resolve a string */
static bool parse_and_resolve(const char *source, DiagContext *diag) {
    Arena arena;
    arena_init(&arena);

    StringPool pool;
    string_pool_init(&pool);

    Lexer lexer;
    lexer_init(&lexer, source, strlen(source), 0, &pool, diag);

    Parser parser;
    parser_init(&parser, &lexer, &arena, diag);

    Module *mod = parse_module(&parser);
    if (!mod || diag_has_errors(diag)) {
        arena_destroy(&arena);
        string_pool_destroy(&pool);
        return false;
    }

    SemaContext sema;
    sema_init(&sema, &arena, diag, &pool);

    bool result = sema_resolve_names(&sema, mod);

    arena_destroy(&arena);
    string_pool_destroy(&pool);
    return result;
}

/* Test: Built-in types resolve correctly */
static bool test_builtin_types(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    let x: i32 = 42\n"
        "    let y: bool = true\n"
        "    let z: f64 = 3.14\n"
        "    result x\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Undefined variable error */
static bool test_undefined_variable(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    result undefined_var\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);

    /* Should fail - undefined variable */
    return !result && diag_has_errors(&diag);
}

/* Test: Variable shadowing in nested scope */
static bool test_variable_shadowing(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    let x: i32 = 1\n"
        "    {\n"
        "        let x: i32 = 2\n"
        "        x\n"
        "    }\n"
        "    result x\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Record definition and use */
static bool test_record_definition(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "record Point {\n"
        "    x: f64,\n"
        "    y: f64\n"
        "}\n"
        "\n"
        "procedure test() -> f64 {\n"
        "    let p = Point { x: 1.0, y: 2.0 }\n"
        "    result p.x\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Undefined type error */
static bool test_undefined_type(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test(p: UndefinedType) -> i32 {\n"
        "    result 0\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);

    /* Should fail - undefined type */
    return !result;
}

/* Test: Enum definition and use */
static bool test_enum_definition(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "enum Color {\n"
        "    Red,\n"
        "    Green,\n"
        "    Blue\n"
        "}\n"
        "\n"
        "procedure test(c: Color) -> i32 {\n"
        "    result 0\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Generic procedure */
static bool test_generic_procedure(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure identity<T>(x: T) -> T {\n"
        "    result x\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Loop variable binding */
static bool test_loop_binding(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    var sum: i32 = 0\n"
        "    loop i in 0..10 {\n"
        "        sum = sum + i\n"
        "    }\n"
        "    result sum\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Break outside loop error */
static bool test_break_outside_loop(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    break\n"
        "    result 0\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);

    /* Should fail - break outside loop */
    return !result;
}

/* Test: Match expression with bindings */
static bool test_match_bindings(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "enum Option<T> {\n"
        "    Some(T),\n"
        "    None\n"
        "}\n"
        "\n"
        "procedure test(opt: Option<i32>) -> i32 {\n"
        "    match opt {\n"
        "        Option::Some(x) => result x,\n"
        "        Option::None => result 0\n"
        "    }\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Class definition */
static bool test_class_definition(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "class Printable {\n"
        "    procedure print(~)\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Record with class implementation */
static bool test_record_implements_class(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "class Printable {\n"
        "    procedure print(~)\n"
        "}\n"
        "\n"
        "record Point <: Printable {\n"
        "    x: f64,\n"
        "    y: f64,\n"
        "\n"
        "    procedure print(~) {\n"
        "    }\n"
        "}\n";

    bool result = parse_and_resolve(source, &diag);
    diag_destroy(&diag);
    return result;
}

int main(void) {
    printf("Running name resolution tests:\n");

    TEST(builtin_types);
    TEST(undefined_variable);
    TEST(variable_shadowing);
    TEST(record_definition);
    TEST(undefined_type);
    TEST(enum_definition);
    TEST(generic_procedure);
    TEST(loop_binding);
    TEST(break_outside_loop);
    TEST(match_bindings);
    TEST(class_definition);
    TEST(record_implements_class);

    printf("\nResults: %d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
