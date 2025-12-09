#include <stdio.h>
#include <string.h>
#include "common/arena.h"
#include "common/string_pool.h"
#include "common/error.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "sema/sema.h"

int main(void) {
    const char *source =
        "procedure test() -> i32 {\n"
        "    let x: i32 = 42\n"
        "    let y = x\n"
        "    result x + y\n"
        "}\n";

    Arena arena;
    arena_init(&arena);
    StringPool pool;
    string_pool_init(&pool);
    DiagContext diag;
    diag_init(&diag);

    Lexer lexer;
    lexer_init(&lexer, source, strlen(source), 0, &pool, &diag);
    Parser parser;
    parser_init(&parser, &lexer, &arena, &diag);
    Module *mod = parse_module(&parser);

    if (!mod || diag_has_errors(&diag)) {
        printf("Parse failed\n");
        diag_print_all(&diag, stdout);
        return 1;
    }

    SemaContext sema;
    sema_init(&sema, &arena, &diag, &pool);
    bool result = sema_analyze(&sema, mod);

    printf("Analysis result: %s\n", result ? "PASS" : "FAIL");
    if (!result) {
        diag_print_all(&diag, stdout);
    }

    return result ? 0 : 1;
}
