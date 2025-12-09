/*
 * Cursive Bootstrap Compiler - Move Analysis Tests
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

/* Helper to parse, resolve, typecheck, and analyze moves */
static bool full_analysis(const char *source, DiagContext *diag) {
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

    bool result = sema_analyze(&sema, mod);

    arena_destroy(&arena);
    string_pool_destroy(&pool);
    return result;
}

/* Test: Simple valid move */
static bool test_simple_move(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "record Resource { id: i32 }\n"
        "\n"
        "procedure consume(move r: Resource) -> i32 {\n"
        "    result r.id\n"
        "}\n"
        "\n"
        "procedure test() -> i32 {\n"
        "    let r = Resource { id: 42 }\n"
        "    result consume(move r)\n"
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Use after move should fail */
static bool test_use_after_move(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "record Resource { id: i32 }\n"
        "\n"
        "procedure consume(move r: Resource) -> i32 {\n"
        "    result r.id\n"
        "}\n"
        "\n"
        "procedure test() -> i32 {\n"
        "    let r = Resource { id: 42 }\n"
        "    let x = consume(move r)\n"
        "    result r.id\n"  /* Error: use after move */
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);

    /* Should fail - use after move */
    return !result;
}

/* Test: Move from immovable binding should fail */
static bool test_immovable_binding(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "record Resource { id: i32 }\n"
        "\n"
        "procedure consume(move r: Resource) -> i32 {\n"
        "    result r.id\n"
        "}\n"
        "\n"
        "procedure test() -> i32 {\n"
        "    let r := Resource { id: 42 }\n"  /* := makes it immovable */
        "    result consume(move r)\n"  /* Error: cannot move from immovable */
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);

    /* Should fail - cannot move from immovable binding */
    return !result;
}

/* Test: Reassignment of mutable binding revalidates */
static bool test_reassign_after_move(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "record Resource { id: i32 }\n"
        "\n"
        "procedure consume(move r: Resource) -> i32 {\n"
        "    result r.id\n"
        "}\n"
        "\n"
        "procedure test() -> i32 {\n"
        "    var r = Resource { id: 42 }\n"
        "    let x = consume(move r)\n"
        "    r = Resource { id: 100 }\n"  /* Reassignment revalidates */
        "    result r.id\n"  /* Valid - r is valid again */
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Uninitialized binding use should fail */
static bool test_uninitialized_use(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    var x: i32\n"
        "    result x\n"  /* Error: use of uninitialized */
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);

    /* Should fail - uninitialized use */
    return !result;
}

/* Test: Assignment to immutable should fail */
static bool test_immutable_assignment(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    let x: i32 = 42\n"
        "    x = 100\n"  /* Error: cannot assign to immutable */
        "    result x\n"
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);

    /* Should fail - assignment to immutable */
    return !result;
}

/* Test: Copy types don't move */
static bool test_copy_types(void) {
    DiagContext diag;
    diag_init(&diag);

    const char *source =
        "procedure test() -> i32 {\n"
        "    let x: i32 = 42\n"
        "    let y = x\n"  /* i32 is Copy, doesn't move */
        "    result x + y\n"  /* Both are still valid */
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);
    return result;
}

/* Test: Match moves scrutinee */
static bool test_match_moves_scrutinee(void) {
    DiagContext diag;
    diag_init(&diag);

    /* Use a simple enum - proper Cursive style (no generics) */
    const char *source =
        "enum Status {\n"
        "    Active(i32),\n"
        "    Inactive\n"
        "}\n"
        "\n"
        "procedure test(move s: Status) -> i32 {\n"
        "    result match s {\n"
        "        Status::Active(x) => x,\n"
        "        Status::Inactive => 0\n"
        "    }\n"
        "}\n";

    bool result = full_analysis(source, &diag);
    diag_destroy(&diag);
    return result;
}

int main(void) {
    printf("Running move analysis tests:\n");

    TEST(simple_move);
    TEST(use_after_move);
    TEST(immovable_binding);
    TEST(reassign_after_move);
    TEST(uninitialized_use);
    TEST(immutable_assignment);
    TEST(copy_types);
    TEST(match_moves_scrutinee);

    printf("\nResults: %d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
