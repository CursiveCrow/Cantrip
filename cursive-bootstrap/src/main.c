/*
 * Cursive Bootstrap Compiler - Main Entry Point
 *
 * Command-line driver for the bootstrap compiler.
 * Orchestrates the full compilation pipeline:
 *   1. Lexing (source -> tokens)
 *   2. Parsing (tokens -> AST)
 *   3. Name Resolution (resolve symbols)
 *   4. Type Checking (verify types)
 *   5. Move Analysis (verify ownership)
 *   6. Permission Checking (verify access)
 *   7. Code Generation (AST -> LLVM IR -> object code)
 */

#include "common/common.h"
#include "common/arena.h"
#include "common/error.h"
#include "common/string_pool.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"
#include "sema/sema.h"
#include "codegen/codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Command-line options */
typedef struct Options {
    const char *input_file;   /* Input source file */
    const char *output_file;  /* Output file (default: a.out / a.exe) */
    bool emit_tokens;         /* -emit-tokens: print token stream */
    bool emit_ast;            /* -emit-ast: print AST */
    bool emit_llvm;           /* -emit-llvm: print LLVM IR */
    bool emit_obj;            /* -c: compile to object file only */
    bool check_only;          /* -check: type check only, no codegen */
    bool help;                /* -help: print usage */
    bool version;             /* -version: print version */
} Options;

static void print_usage(const char *program) {
    fprintf(stderr, "Usage: %s [options] <input.cur>\n\n", program);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -o <file>       Output file (default: a.out / a.exe)\n");
    fprintf(stderr, "  -c              Compile to object file only (no linking)\n");
    fprintf(stderr, "  -check          Type check only, no code generation\n");
    fprintf(stderr, "  -emit-tokens    Print token stream and exit\n");
    fprintf(stderr, "  -emit-ast       Print AST and exit\n");
    fprintf(stderr, "  -emit-llvm      Print LLVM IR and exit\n");
    fprintf(stderr, "  -help           Print this help message\n");
    fprintf(stderr, "  -version        Print version information\n");
}

static void print_version(void) {
    fprintf(stderr, "Cursive Bootstrap Compiler v0.1.0\n");
    fprintf(stderr, "Target: ");
#if defined(CURSIVE_PLATFORM_WINDOWS)
    fprintf(stderr, "Windows ");
#elif defined(CURSIVE_PLATFORM_LINUX)
    fprintf(stderr, "Linux ");
#elif defined(CURSIVE_PLATFORM_MACOS)
    fprintf(stderr, "macOS ");
#endif
#if defined(__x86_64__) || defined(_M_X64)
    fprintf(stderr, "x86_64\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    fprintf(stderr, "aarch64\n");
#else
    fprintf(stderr, "unknown\n");
#endif
}

static bool parse_args(int argc, char **argv, Options *opts) {
    memset(opts, 0, sizeof(*opts));

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-help") == 0 || strcmp(arg, "--help") == 0) {
            opts->help = true;
        } else if (strcmp(arg, "-version") == 0 || strcmp(arg, "--version") == 0) {
            opts->version = true;
        } else if (strcmp(arg, "-emit-tokens") == 0) {
            opts->emit_tokens = true;
        } else if (strcmp(arg, "-emit-ast") == 0) {
            opts->emit_ast = true;
        } else if (strcmp(arg, "-emit-llvm") == 0) {
            opts->emit_llvm = true;
        } else if (strcmp(arg, "-c") == 0) {
            opts->emit_obj = true;
        } else if (strcmp(arg, "-check") == 0) {
            opts->check_only = true;
        } else if (strcmp(arg, "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o requires an argument\n");
                return false;
            }
            opts->output_file = argv[++i];
        } else if (arg[0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", arg);
            return false;
        } else {
            if (opts->input_file) {
                fprintf(stderr, "Error: Multiple input files not supported\n");
                return false;
            }
            opts->input_file = arg;
        }
    }

    return true;
}

/* Read entire file into memory */
static char *read_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (len < 0) {
        fclose(f);
        fprintf(stderr, "Error: Cannot determine file size '%s'\n", path);
        return NULL;
    }

    char *buf = (char *)malloc((size_t)len + 1);
    if (!buf) {
        fclose(f);
        fprintf(stderr, "Error: Out of memory reading '%s'\n", path);
        return NULL;
    }

    size_t read = fread(buf, 1, (size_t)len, f);
    fclose(f);

    buf[read] = '\0';
    *out_len = read;
    return buf;
}

/* Forward declarations for AST printing */
static void print_ast_module(Module *mod, int indent);
static void print_ast_decl(Decl *decl, int indent);
static void print_ast_expr(Expr *expr, int indent);
static void print_ast_stmt(Stmt *stmt, int indent);
static void print_ast_type(TypeExpr *type);
static void print_ast_pattern(Pattern *pat);

/* Get default output filename */
static const char *get_default_output(const Options *opts) {
    if (opts->emit_obj) {
#ifdef _WIN32
        return "output.obj";
#else
        return "output.o";
#endif
    }
#ifdef _WIN32
    return "a.exe";
#else
    return "a.out";
#endif
}

/* Extract module name from filename */
static const char *get_module_name(const char *path, Arena *arena) {
    /* Find last path separator */
    const char *name = path;
    for (const char *p = path; *p; p++) {
        if (*p == '/' || *p == '\\') {
            name = p + 1;
        }
    }

    /* Find extension and strip it */
    size_t len = strlen(name);
    for (size_t i = len; i > 0; i--) {
        if (name[i - 1] == '.') {
            len = i - 1;
            break;
        }
    }

    char *result = arena_alloc_aligned(arena, len + 1, 1);
    memcpy(result, name, len);
    result[len] = '\0';
    return result;
}

int main(int argc, char **argv) {
    Options opts;
    if (!parse_args(argc, argv, &opts)) {
        print_usage(argv[0]);
        return 1;
    }

    if (opts.help) {
        print_usage(argv[0]);
        return 0;
    }

    if (opts.version) {
        print_version();
        return 0;
    }

    if (!opts.input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Read source file */
    size_t source_len;
    char *source = read_file(opts.input_file, &source_len);
    if (!source) {
        return 1;
    }

    int exit_code = 0;

    /* Initialize compiler context */
    DiagContext diag;
    diag_init(&diag);

    StringPool strings;
    string_pool_init(&strings);

    Arena ast_arena;
    arena_init(&ast_arena);

    /* Add source file to diagnostics */
    uint32_t file_id = diag_add_file(&diag, opts.input_file, source, source_len);

    /* ============================================
     * Stage 1: Lexing
     * ============================================ */
    Lexer lexer;
    lexer_init(&lexer, source, source_len, file_id, &strings, &diag);

    if (opts.emit_tokens) {
        /* Print all tokens */
        Token tok;
        do {
            tok = lexer_next(&lexer);
            token_print(&tok, stdout);
            printf("\n");
        } while (tok.kind != TOK_EOF && tok.kind != TOK_ERROR);

        diag_print_all(&diag);
        exit_code = diag_has_errors(&diag) ? 1 : 0;
        goto cleanup;
    }

    /* ============================================
     * Stage 2: Parsing
     * ============================================ */
    Parser parser;
    parser_init(&parser, &lexer, &ast_arena, &diag);

    Module *mod = parse_module(&parser);
    if (!mod || diag_has_errors(&diag)) {
        fprintf(stderr, "Parsing failed.\n");
        diag_print_all(&diag);
        exit_code = 1;
        goto cleanup;
    }

    if (opts.emit_ast) {
        /* Print AST */
        print_ast_module(mod, 0);
        diag_print_all(&diag);
        exit_code = diag_has_errors(&diag) ? 1 : 0;
        goto cleanup;
    }

    /* ============================================
     * Stage 3-6: Semantic Analysis
     * ============================================ */
    SemaContext sema;
    sema_init(&sema, &ast_arena, &diag, &strings);

    /* Run full semantic analysis */
    if (!sema_analyze(&sema, mod)) {
        fprintf(stderr, "Semantic analysis failed.\n");
        diag_print_all(&diag);
        exit_code = 1;
        goto cleanup;
    }

    if (opts.check_only) {
        /* Type checking only - no codegen */
        fprintf(stderr, "Type checking passed.\n");
        diag_print_all(&diag);
        exit_code = 0;
        goto cleanup;
    }

    /* ============================================
     * Stage 7: Code Generation
     * ============================================ */
#ifdef HAVE_LLVM
    {
        const char *module_name = get_module_name(opts.input_file, &ast_arena);
        CodegenContext codegen;

        if (!codegen_init(&codegen, &ast_arena, &sema, &diag, module_name)) {
            fprintf(stderr, "Code generation initialization failed.\n");
            exit_code = 1;
            goto cleanup;
        }

        if (!codegen_module(&codegen, mod)) {
            fprintf(stderr, "Code generation failed.\n");
            diag_print_all(&diag);
            codegen_destroy(&codegen);
            exit_code = 1;
            goto cleanup;
        }

        /* Determine output file */
        const char *output = opts.output_file ? opts.output_file : get_default_output(&opts);

        if (opts.emit_llvm) {
            /* Write LLVM IR */
            const char *ir_file = output;
            if (!opts.output_file) {
                ir_file = "output.ll";
            }
            if (!codegen_write_ir(&codegen, ir_file)) {
                fprintf(stderr, "Failed to write LLVM IR to '%s'.\n", ir_file);
                exit_code = 1;
            } else {
                fprintf(stderr, "Wrote LLVM IR to '%s'.\n", ir_file);
            }
        } else {
            /* Write object file */
            if (!codegen_write_object(&codegen, output)) {
                fprintf(stderr, "Failed to write object file to '%s'.\n", output);
                exit_code = 1;
            } else {
                fprintf(stderr, "Wrote object file to '%s'.\n", output);
            }
        }

        codegen_destroy(&codegen);
    }
#else
    /* LLVM not available */
    if (opts.emit_llvm || opts.emit_obj || (!opts.check_only && !opts.emit_ast && !opts.emit_tokens)) {
        fprintf(stderr, "Error: Code generation requires LLVM support.\n");
        fprintf(stderr, "The compiler was built without LLVM. Use -check for type checking only.\n");
        exit_code = 1;
        goto cleanup;
    }
#endif

    diag_print_all(&diag);
    if (diag_has_errors(&diag)) {
        exit_code = 1;
    }

cleanup:
    arena_destroy(&ast_arena);
    string_pool_destroy(&strings);
    diag_destroy(&diag);
    free(source);

    return exit_code;
}

/* ============================================
 * AST Printing Utilities
 * ============================================ */

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static const char *visibility_str(Visibility vis) {
    switch (vis) {
        case VIS_PUBLIC: return "public";
        case VIS_INTERNAL: return "internal";
        case VIS_PROTECTED: return "protected";
        case VIS_PRIVATE: return "private";
        default: return "unknown";
    }
}

static const char *receiver_str(ReceiverKind recv) {
    switch (recv) {
        case RECV_NONE: return "";
        case RECV_CONST: return "~";
        case RECV_UNIQUE: return "~!";
        case RECV_SHARED: return "~%";
        default: return "?";
    }
}

static void print_ast_type(TypeExpr *type) {
    if (!type) {
        printf("(inferred)");
        return;
    }

    switch (type->kind) {
        case TEXPR_PRIMITIVE:
            switch (type->primitive) {
                case PRIM_I8: printf("i8"); break;
                case PRIM_I16: printf("i16"); break;
                case PRIM_I32: printf("i32"); break;
                case PRIM_I64: printf("i64"); break;
                case PRIM_I128: printf("i128"); break;
                case PRIM_ISIZE: printf("isize"); break;
                case PRIM_U8: printf("u8"); break;
                case PRIM_U16: printf("u16"); break;
                case PRIM_U32: printf("u32"); break;
                case PRIM_U64: printf("u64"); break;
                case PRIM_U128: printf("u128"); break;
                case PRIM_USIZE: printf("usize"); break;
                case PRIM_F16: printf("f16"); break;
                case PRIM_F32: printf("f32"); break;
                case PRIM_F64: printf("f64"); break;
                case PRIM_BOOL: printf("bool"); break;
                case PRIM_CHAR: printf("char"); break;
                case PRIM_STRING: printf("string"); break;
            }
            break;

        case TEXPR_NAMED:
            printf("%s", type->named.name.data ? type->named.name.data : "(unnamed)");
            break;

        case TEXPR_MODAL_STATE:
            print_ast_type(type->modal_state.base);
            printf("@%s", type->modal_state.state.data ? type->modal_state.state.data : "?");
            break;

        case TEXPR_GENERIC:
            print_ast_type(type->generic.base);
            printf("<");
            for (size_t i = 0; i < vec_len(type->generic.args); i++) {
                if (i > 0) printf(", ");
                print_ast_type(type->generic.args[i]);
            }
            printf(">");
            break;

        case TEXPR_TUPLE:
            printf("(");
            for (size_t i = 0; i < vec_len(type->tuple.elements); i++) {
                if (i > 0) printf(", ");
                print_ast_type(type->tuple.elements[i]);
            }
            printf(")");
            break;

        case TEXPR_ARRAY:
            printf("[");
            print_ast_type(type->array.element);
            printf("; ?]");  /* Size would need expression printing */
            break;

        case TEXPR_SLICE:
            printf("[");
            print_ast_type(type->slice.element);
            printf("]");
            break;

        case TEXPR_FUNCTION:
            printf("procedure(");
            for (size_t i = 0; i < vec_len(type->function.params); i++) {
                if (i > 0) printf(", ");
                print_ast_type(type->function.params[i]);
            }
            printf(") -> ");
            print_ast_type(type->function.return_type);
            break;

        case TEXPR_UNION:
            for (size_t i = 0; i < vec_len(type->union_.members); i++) {
                if (i > 0) printf(" | ");
                print_ast_type(type->union_.members[i]);
            }
            break;

        case TEXPR_PTR:
            printf("Ptr<");
            print_ast_type(type->ptr.pointee);
            printf(">");
            break;

        case TEXPR_REF:
            printf(type->ref.is_unique ? "&!" : "&");
            print_ast_type(type->ref.referent);
            break;

        case TEXPR_NEVER:
            printf("!");
            break;

        case TEXPR_UNIT:
            printf("()");
            break;

        case TEXPR_SELF:
            printf("Self");
            break;

        case TEXPR_INFER:
            printf("_");
            break;
    }
}

static void print_ast_pattern(Pattern *pat) {
    if (!pat) {
        printf("_");
        return;
    }

    switch (pat->kind) {
        case PAT_WILDCARD:
            printf("_");
            break;

        case PAT_BINDING:
            if (pat->binding.is_mutable) printf("var ");
            printf("%s", pat->binding.name.data ? pat->binding.name.data : "(unnamed)");
            if (pat->binding.type) {
                printf(": ");
                print_ast_type(pat->binding.type);
            }
            break;

        case PAT_LITERAL:
            printf("<literal>");
            break;

        case PAT_TUPLE:
            printf("(");
            for (size_t i = 0; i < vec_len(pat->tuple.elements); i++) {
                if (i > 0) printf(", ");
                print_ast_pattern(pat->tuple.elements[i]);
            }
            printf(")");
            break;

        case PAT_RECORD:
            print_ast_type(pat->record.type);
            printf(" { ... }");
            break;

        case PAT_ENUM:
            print_ast_type(pat->enum_.type);
            printf("::%s", pat->enum_.variant.data ? pat->enum_.variant.data : "?");
            if (pat->enum_.payload) {
                printf("(");
                print_ast_pattern(pat->enum_.payload);
                printf(")");
            }
            break;

        case PAT_MODAL:
            printf("@%s { ... }", pat->modal.state.data ? pat->modal.state.data : "?");
            break;

        case PAT_RANGE:
            print_ast_pattern(pat->range.start);
            printf(pat->range.inclusive ? "..=" : "..");
            print_ast_pattern(pat->range.end);
            break;

        case PAT_OR:
            for (size_t i = 0; i < vec_len(pat->or_.alternatives); i++) {
                if (i > 0) printf(" | ");
                print_ast_pattern(pat->or_.alternatives[i]);
            }
            break;

        case PAT_GUARD:
            print_ast_pattern(pat->guard.pattern);
            printf(" if <guard>");
            break;
    }
}

static void print_ast_expr(Expr *expr, int indent) {
    if (!expr) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (expr->kind) {
        case EXPR_INT_LIT:
            printf("IntLit(%llu)\n", (unsigned long long)expr->int_lit.value);
            break;

        case EXPR_FLOAT_LIT:
            printf("FloatLit(%f)\n", expr->float_lit.value);
            break;

        case EXPR_STRING_LIT:
            printf("StringLit(\"%s\")\n",
                   expr->string_lit.value.data ? expr->string_lit.value.data : "");
            break;

        case EXPR_CHAR_LIT:
            printf("CharLit('%c')\n", (char)expr->char_lit.value);
            break;

        case EXPR_BOOL_LIT:
            printf("BoolLit(%s)\n", expr->bool_lit.value ? "true" : "false");
            break;

        case EXPR_IDENT:
            printf("Ident(%s)\n", expr->ident.name.data ? expr->ident.name.data : "(unnamed)");
            break;

        case EXPR_PATH:
            printf("Path(");
            for (size_t i = 0; i < vec_len(expr->path.segments); i++) {
                if (i > 0) printf("::");
                printf("%s", expr->path.segments[i].data ? expr->path.segments[i].data : "?");
            }
            printf(")\n");
            break;

        case EXPR_BINARY:
            printf("Binary(op=%d)\n", expr->binary.op);
            print_ast_expr(expr->binary.left, indent + 1);
            print_ast_expr(expr->binary.right, indent + 1);
            break;

        case EXPR_UNARY:
            printf("Unary(op=%d)\n", expr->unary.op);
            print_ast_expr(expr->unary.operand, indent + 1);
            break;

        case EXPR_CALL:
            printf("Call\n");
            print_indent(indent + 1);
            printf("callee:\n");
            print_ast_expr(expr->call.callee, indent + 2);
            print_indent(indent + 1);
            printf("args: %zu\n", vec_len(expr->call.args));
            for (size_t i = 0; i < vec_len(expr->call.args); i++) {
                print_ast_expr(expr->call.args[i], indent + 2);
            }
            break;

        case EXPR_METHOD_CALL:
            printf("MethodCall(~>%s)\n",
                   expr->method_call.method.data ? expr->method_call.method.data : "?");
            print_indent(indent + 1);
            printf("receiver:\n");
            print_ast_expr(expr->method_call.receiver, indent + 2);
            print_indent(indent + 1);
            printf("args: %zu\n", vec_len(expr->method_call.args));
            for (size_t i = 0; i < vec_len(expr->method_call.args); i++) {
                print_ast_expr(expr->method_call.args[i], indent + 2);
            }
            break;

        case EXPR_FIELD:
            printf("Field(.%s)\n", expr->field.field.data ? expr->field.field.data : "?");
            print_ast_expr(expr->field.object, indent + 1);
            break;

        case EXPR_INDEX:
            printf("Index\n");
            print_ast_expr(expr->index.object, indent + 1);
            print_ast_expr(expr->index.index, indent + 1);
            break;

        case EXPR_TUPLE:
            printf("Tuple(%zu elements)\n", vec_len(expr->tuple.elements));
            for (size_t i = 0; i < vec_len(expr->tuple.elements); i++) {
                print_ast_expr(expr->tuple.elements[i], indent + 1);
            }
            break;

        case EXPR_ARRAY:
            printf("Array\n");
            if (expr->array.repeat_value) {
                print_indent(indent + 1);
                printf("repeat value:\n");
                print_ast_expr(expr->array.repeat_value, indent + 2);
            } else {
                for (size_t i = 0; i < vec_len(expr->array.elements); i++) {
                    print_ast_expr(expr->array.elements[i], indent + 1);
                }
            }
            break;

        case EXPR_RECORD:
            printf("RecordLit(");
            print_ast_type(expr->record.type);
            printf(")\n");
            for (size_t i = 0; i < vec_len(expr->record.field_names); i++) {
                print_indent(indent + 1);
                printf("%s:\n", expr->record.field_names[i].data ?
                       expr->record.field_names[i].data : "?");
                print_ast_expr(expr->record.field_values[i], indent + 2);
            }
            break;

        case EXPR_IF:
            printf("If\n");
            print_indent(indent + 1);
            printf("condition:\n");
            print_ast_expr(expr->if_.condition, indent + 2);
            print_indent(indent + 1);
            printf("then:\n");
            print_ast_expr(expr->if_.then_branch, indent + 2);
            if (expr->if_.else_branch) {
                print_indent(indent + 1);
                printf("else:\n");
                print_ast_expr(expr->if_.else_branch, indent + 2);
            }
            break;

        case EXPR_MATCH:
            printf("Match\n");
            print_indent(indent + 1);
            printf("scrutinee:\n");
            print_ast_expr(expr->match.scrutinee, indent + 2);
            for (size_t i = 0; i < vec_len(expr->match.arms_patterns); i++) {
                print_indent(indent + 1);
                printf("arm %zu: ", i);
                print_ast_pattern(expr->match.arms_patterns[i]);
                printf(" =>\n");
                print_ast_expr(expr->match.arms_bodies[i], indent + 2);
            }
            break;

        case EXPR_BLOCK:
            printf("Block(%zu stmts)\n", vec_len(expr->block.stmts));
            for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
                print_ast_stmt(expr->block.stmts[i], indent + 1);
            }
            if (expr->block.result) {
                print_indent(indent + 1);
                printf("result:\n");
                print_ast_expr(expr->block.result, indent + 2);
            }
            break;

        case EXPR_LOOP:
            printf("Loop");
            if (expr->loop.label.data) {
                printf(" '%s", expr->loop.label.data);
            }
            printf("\n");
            if (expr->loop.binding) {
                print_indent(indent + 1);
                printf("binding: ");
                print_ast_pattern(expr->loop.binding);
                printf(" in\n");
                print_ast_expr(expr->loop.iterable, indent + 2);
            }
            if (expr->loop.condition) {
                print_indent(indent + 1);
                printf("condition:\n");
                print_ast_expr(expr->loop.condition, indent + 2);
            }
            print_indent(indent + 1);
            printf("body:\n");
            print_ast_expr(expr->loop.body, indent + 2);
            break;

        case EXPR_MOVE:
            printf("Move\n");
            print_ast_expr(expr->move.operand, indent + 1);
            break;

        case EXPR_WIDEN:
            printf("Widen\n");
            print_ast_expr(expr->widen.operand, indent + 1);
            break;

        case EXPR_CAST:
            printf("Cast(as ");
            print_ast_type(expr->cast.target_type);
            printf(")\n");
            print_ast_expr(expr->cast.operand, indent + 1);
            break;

        case EXPR_RANGE:
            printf("Range(%s)\n", expr->range.inclusive ? "..=" : "..");
            if (expr->range.start) print_ast_expr(expr->range.start, indent + 1);
            if (expr->range.end) print_ast_expr(expr->range.end, indent + 1);
            break;

        case EXPR_STATIC_CALL:
            printf("StaticCall(");
            print_ast_type(expr->static_call.type);
            printf("::%s)\n", expr->static_call.method.data ? expr->static_call.method.data : "?");
            for (size_t i = 0; i < vec_len(expr->static_call.args); i++) {
                print_ast_expr(expr->static_call.args[i], indent + 1);
            }
            break;

        case EXPR_REGION_ALLOC:
            printf("RegionAlloc(^%s)\n",
                   expr->region_alloc.region.data ? expr->region_alloc.region.data : "?");
            print_ast_expr(expr->region_alloc.value, indent + 1);
            break;

        case EXPR_ADDR_OF:
            printf("AddrOf(%s)\n", expr->addr_of.is_unique ? "&!" : "&");
            print_ast_expr(expr->addr_of.operand, indent + 1);
            break;

        case EXPR_DEREF:
            printf("Deref(*)\n");
            print_ast_expr(expr->deref.operand, indent + 1);
            break;

        case EXPR_CLOSURE:
            printf("Closure\n");
            break;
    }
}

static void print_ast_stmt(Stmt *stmt, int indent) {
    if (!stmt) {
        print_indent(indent);
        printf("(null stmt)\n");
        return;
    }

    print_indent(indent);

    switch (stmt->kind) {
        case STMT_EXPR:
            printf("ExprStmt\n");
            print_ast_expr(stmt->expr.expr, indent + 1);
            break;

        case STMT_LET:
            printf("Let(%s) ", stmt->let.op == BIND_MOVABLE ? "=" : ":=");
            print_ast_pattern(stmt->let.pattern);
            if (stmt->let.type) {
                printf(": ");
                print_ast_type(stmt->let.type);
            }
            printf("\n");
            if (stmt->let.init) {
                print_ast_expr(stmt->let.init, indent + 1);
            }
            break;

        case STMT_VAR:
            printf("Var(%s) ", stmt->var.op == BIND_MOVABLE ? "=" : ":=");
            print_ast_pattern(stmt->var.pattern);
            if (stmt->var.type) {
                printf(": ");
                print_ast_type(stmt->var.type);
            }
            printf("\n");
            if (stmt->var.init) {
                print_ast_expr(stmt->var.init, indent + 1);
            }
            break;

        case STMT_ASSIGN:
            printf("Assign\n");
            print_indent(indent + 1);
            printf("target:\n");
            print_ast_expr(stmt->assign.target, indent + 2);
            print_indent(indent + 1);
            printf("value:\n");
            print_ast_expr(stmt->assign.value, indent + 2);
            break;

        case STMT_RETURN:
            printf("Return\n");
            if (stmt->return_.value) {
                print_ast_expr(stmt->return_.value, indent + 1);
            }
            break;

        case STMT_RESULT:
            printf("Result\n");
            print_ast_expr(stmt->result.value, indent + 1);
            break;

        case STMT_BREAK:
            printf("Break");
            if (stmt->break_.label.data) {
                printf(" '%s", stmt->break_.label.data);
            }
            printf("\n");
            if (stmt->break_.value) {
                print_ast_expr(stmt->break_.value, indent + 1);
            }
            break;

        case STMT_CONTINUE:
            printf("Continue");
            if (stmt->continue_.label.data) {
                printf(" '%s", stmt->continue_.label.data);
            }
            printf("\n");
            break;

        case STMT_DEFER:
            printf("Defer\n");
            print_ast_expr(stmt->defer.body, indent + 1);
            break;

        case STMT_UNSAFE:
            printf("Unsafe\n");
            print_ast_expr(stmt->unsafe.body, indent + 1);
            break;
    }
}

static void print_ast_proc(ProcDecl *proc, int indent) {
    print_indent(indent);
    printf("%s procedure ", visibility_str(proc->vis));
    if (proc->receiver != RECV_NONE) {
        printf("(%s) ", receiver_str(proc->receiver));
    }
    printf("%s", proc->name.data ? proc->name.data : "(unnamed)");

    /* Generics */
    if (vec_len(proc->generics) > 0) {
        printf("<");
        for (size_t i = 0; i < vec_len(proc->generics); i++) {
            if (i > 0) printf(", ");
            printf("%s", proc->generics[i].name.data ? proc->generics[i].name.data : "?");
        }
        printf(">");
    }

    /* Parameters */
    printf("(");
    for (size_t i = 0; i < vec_len(proc->params); i++) {
        if (i > 0) printf(", ");
        ParamDecl *p = &proc->params[i];
        if (p->is_move) printf("move ");
        printf("%s: ", p->name.data ? p->name.data : "?");
        print_ast_type(p->type);
    }
    printf(")");

    /* Return type */
    if (proc->return_type) {
        printf(" -> ");
        print_ast_type(proc->return_type);
    }

    printf("\n");

    /* Contracts */
    for (size_t i = 0; i < vec_len(proc->contracts); i++) {
        print_indent(indent + 1);
        printf("%s <contract>\n", proc->contracts[i].is_precondition ? "|=" : "=>");
    }

    /* Body */
    if (proc->body) {
        print_ast_expr(proc->body, indent + 1);
    }
}

static void print_ast_decl(Decl *decl, int indent) {
    if (!decl) {
        print_indent(indent);
        printf("(null decl)\n");
        return;
    }

    switch (decl->kind) {
        case DECL_PROC:
            print_ast_proc(&decl->proc, indent);
            break;

        case DECL_RECORD:
            print_indent(indent);
            printf("%s record %s", visibility_str(decl->record.vis),
                   decl->record.name.data ? decl->record.name.data : "(unnamed)");
            if (vec_len(decl->record.implements) > 0) {
                printf(" <: ");
                for (size_t i = 0; i < vec_len(decl->record.implements); i++) {
                    if (i > 0) printf(" + ");
                    print_ast_type(decl->record.implements[i]);
                }
            }
            printf(" {\n");
            for (size_t i = 0; i < vec_len(decl->record.fields); i++) {
                FieldDecl *f = &decl->record.fields[i];
                print_indent(indent + 1);
                printf("%s: ", f->name.data ? f->name.data : "?");
                print_ast_type(f->type);
                printf("\n");
            }
            for (size_t i = 0; i < vec_len(decl->record.methods); i++) {
                print_ast_proc(&decl->record.methods[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;

        case DECL_ENUM:
            print_indent(indent);
            printf("%s enum %s {\n", visibility_str(decl->enum_.vis),
                   decl->enum_.name.data ? decl->enum_.name.data : "(unnamed)");
            for (size_t i = 0; i < vec_len(decl->enum_.variants); i++) {
                EnumVariant *v = &decl->enum_.variants[i];
                print_indent(indent + 1);
                printf("%s", v->name.data ? v->name.data : "?");
                if (v->payload) {
                    printf("(");
                    print_ast_type(v->payload);
                    printf(")");
                }
                printf("\n");
            }
            print_indent(indent);
            printf("}\n");
            break;

        case DECL_MODAL:
            print_indent(indent);
            printf("%s modal %s {\n", visibility_str(decl->modal.vis),
                   decl->modal.name.data ? decl->modal.name.data : "(unnamed)");
            for (size_t i = 0; i < vec_len(decl->modal.states); i++) {
                ModalState *s = &decl->modal.states[i];
                print_indent(indent + 1);
                printf("@%s { ... }\n", s->name.data ? s->name.data : "?");
            }
            print_indent(indent);
            printf("}\n");
            break;

        case DECL_TYPE_ALIAS:
            print_indent(indent);
            printf("%s type %s = ", visibility_str(decl->type_alias.vis),
                   decl->type_alias.name.data ? decl->type_alias.name.data : "(unnamed)");
            print_ast_type(decl->type_alias.aliased);
            printf("\n");
            break;

        case DECL_CLASS:
            print_indent(indent);
            printf("%s class %s {\n", visibility_str(decl->class_.vis),
                   decl->class_.name.data ? decl->class_.name.data : "(unnamed)");
            for (size_t i = 0; i < vec_len(decl->class_.methods); i++) {
                print_ast_proc(&decl->class_.methods[i], indent + 1);
            }
            print_indent(indent);
            printf("}\n");
            break;

        case DECL_EXTERN:
            print_indent(indent);
            printf("extern \"%s\" {\n",
                   decl->extern_.abi.data ? decl->extern_.abi.data : "C");
            for (size_t i = 0; i < vec_len(decl->extern_.funcs); i++) {
                ExternFuncDecl *f = &decl->extern_.funcs[i];
                print_indent(indent + 1);
                printf("procedure %s(...) -> ...\n",
                       f->name.data ? f->name.data : "?");
            }
            print_indent(indent);
            printf("}\n");
            break;

        case DECL_MODULE:
            print_indent(indent);
            printf("module (declaration)\n");
            break;

        case DECL_IMPORT:
            print_indent(indent);
            printf("import ");
            for (size_t i = 0; i < vec_len(decl->import.path); i++) {
                if (i > 0) printf("::");
                printf("%s", decl->import.path[i].data ? decl->import.path[i].data : "?");
            }
            printf("\n");
            break;

        case DECL_USE:
            print_indent(indent);
            printf("use ");
            for (size_t i = 0; i < vec_len(decl->use.path); i++) {
                if (i > 0) printf("::");
                printf("%s", decl->use.path[i].data ? decl->use.path[i].data : "?");
            }
            if (decl->use.is_glob) {
                printf("::*");
            } else if (vec_len(decl->use.items) > 0) {
                printf("::{");
                for (size_t i = 0; i < vec_len(decl->use.items); i++) {
                    if (i > 0) printf(", ");
                    printf("%s", decl->use.items[i].data ? decl->use.items[i].data : "?");
                }
                printf("}");
            }
            printf("\n");
            break;
    }
}

static void print_ast_module(Module *mod, int indent) {
    print_indent(indent);
    printf("Module: %s\n", mod->name.data ? mod->name.data : "(unnamed)");
    printf("========================================\n");

    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        print_ast_decl(mod->decls[i], indent);
        printf("\n");
    }
}