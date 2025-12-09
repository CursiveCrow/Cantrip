/*
 * Cursive Bootstrap Compiler - Error Reporting Implementation
 */

#include "error.h"
#include <stdarg.h>

/* ANSI color codes */
#ifdef CURSIVE_PLATFORM_WINDOWS
    #include <windows.h>
    static bool colors_enabled = false;
    static void init_colors(void) {
        HANDLE hOut = GetStdHandle(STD_ERROR_HANDLE);
        DWORD mode;
        if (GetConsoleMode(hOut, &mode)) {
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            colors_enabled = true;
        }
    }
#else
    static bool colors_enabled = true;
    static void init_colors(void) {}
#endif

#define COLOR_RESET   (colors_enabled ? "\033[0m" : "")
#define COLOR_RED     (colors_enabled ? "\033[31m" : "")
#define COLOR_YELLOW  (colors_enabled ? "\033[33m" : "")
#define COLOR_BLUE    (colors_enabled ? "\033[34m" : "")
#define COLOR_CYAN    (colors_enabled ? "\033[36m" : "")
#define COLOR_BOLD    (colors_enabled ? "\033[1m" : "")

void diag_init(DiagContext *ctx) {
    init_colors();
    ctx->files = vec_new(SourceFile);
    ctx->diagnostics = vec_new(Diagnostic);
    ctx->error_count = 0;
    ctx->warning_count = 0;
    ctx->fatal_occurred = false;
}

void diag_destroy(DiagContext *ctx) {
    /* Free diagnostic messages */
    for (size_t i = 0; i < vec_len(ctx->diagnostics); i++) {
        free(ctx->diagnostics[i].message);
        free(ctx->diagnostics[i].note);
    }
    vec_free(ctx->diagnostics);

    /* Free file line tables */
    for (size_t i = 0; i < vec_len(ctx->files); i++) {
        vec_free(ctx->files[i].lines);
    }
    vec_free(ctx->files);
}

/* Build line offset table for a file */
static void build_line_table(SourceFile *file) {
    file->lines = vec_new(size_t);

    /* First line starts at offset 0 */
    vec_push(file->lines, (size_t)0);

    for (size_t i = 0; i < file->len; i++) {
        if (file->content[i] == '\n') {
            vec_push(file->lines, i + 1);
        }
    }
}

uint32_t diag_add_file(DiagContext *ctx, const char *path, const char *content, size_t len) {
    uint32_t id = (uint32_t)vec_len(ctx->files);

    SourceFile file = {
        .path = path,
        .content = content,
        .len = len,
        .lines = NULL
    };
    build_line_table(&file);

    vec_push(ctx->files, file);
    return id;
}

SourceFile *diag_get_file(DiagContext *ctx, uint32_t file_id) {
    if (file_id >= vec_len(ctx->files)) {
        return NULL;
    }
    return &ctx->files[file_id];
}

void diag_offset_to_loc(const SourceFile *file, size_t offset,
                        uint32_t *line, uint32_t *col) {
    if (!file || !file->lines || vec_len(file->lines) == 0) {
        *line = 1;
        *col = 1;
        return;
    }

    /* Binary search for line */
    size_t lo = 0;
    size_t hi = vec_len(file->lines);

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (file->lines[mid] <= offset) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    *line = (uint32_t)lo; /* 1-indexed */
    size_t line_start = file->lines[lo - 1];
    *col = (uint32_t)(offset - line_start + 1); /* 1-indexed */
}

const char *diag_get_line_text(const SourceFile *file, uint32_t line, size_t *len) {
    if (!file || line == 0 || line > vec_len(file->lines)) {
        *len = 0;
        return "";
    }

    size_t start = file->lines[line - 1];
    size_t end;

    if (line < vec_len(file->lines)) {
        end = file->lines[line];
        /* Remove trailing newline */
        if (end > start && file->content[end - 1] == '\n') {
            end--;
        }
    } else {
        end = file->len;
    }

    *len = end - start;
    return file->content + start;
}

void diag_report(DiagContext *ctx, DiagLevel level, const char *code,
                 SourceSpan span, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    /* Format message */
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *message = (char *)malloc((size_t)size + 1);
    vsnprintf(message, (size_t)size + 1, fmt, args_copy);
    va_end(args_copy);

    Diagnostic diag = {
        .level = level,
        .code = code,
        .span = span,
        .message = message,
        .note = NULL
    };

    vec_push(ctx->diagnostics, diag);

    switch (level) {
        case DIAG_WARNING:
            ctx->warning_count++;
            break;
        case DIAG_ERROR:
            ctx->error_count++;
            break;
        case DIAG_FATAL:
            ctx->error_count++;
            ctx->fatal_occurred = true;
            break;
        default:
            break;
    }
}

void diag_report_note(DiagContext *ctx, DiagLevel level, const char *code,
                      SourceSpan span, const char *note, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *message = (char *)malloc((size_t)size + 1);
    vsnprintf(message, (size_t)size + 1, fmt, args_copy);
    va_end(args_copy);

    Diagnostic diag = {
        .level = level,
        .code = code,
        .span = span,
        .message = message,
        .note = note ? strdup(note) : NULL
    };

    vec_push(ctx->diagnostics, diag);

    switch (level) {
        case DIAG_WARNING:
            ctx->warning_count++;
            break;
        case DIAG_ERROR:
        case DIAG_FATAL:
            ctx->error_count++;
            if (level == DIAG_FATAL) ctx->fatal_occurred = true;
            break;
        default:
            break;
    }
}

void diag_print(DiagContext *ctx, const Diagnostic *diag) {
    SourceFile *file = diag_get_file(ctx, diag->span.start.file_id);
    const char *path = file ? file->path : "<unknown>";

    /* Level color and prefix */
    const char *level_str;
    const char *color;
    switch (diag->level) {
        case DIAG_NOTE:
            level_str = "note";
            color = COLOR_CYAN;
            break;
        case DIAG_WARNING:
            level_str = "warning";
            color = COLOR_YELLOW;
            break;
        case DIAG_ERROR:
        case DIAG_FATAL:
            level_str = "error";
            color = COLOR_RED;
            break;
        default:
            level_str = "unknown";
            color = "";
            break;
    }

    /* Print header: path:line:col: level[code]: message */
    fprintf(stderr, "%s%s:%u:%u:%s %s%s%s[%s]%s: %s\n",
            COLOR_BOLD, path,
            diag->span.start.line, diag->span.start.col,
            COLOR_RESET,
            COLOR_BOLD, color, level_str, diag->code, COLOR_RESET,
            diag->message);

    /* Print source line with caret */
    if (file && diag->span.start.line > 0) {
        size_t line_len;
        const char *line_text = diag_get_line_text(file, diag->span.start.line, &line_len);

        /* Line number gutter */
        fprintf(stderr, " %5u | ", diag->span.start.line);

        /* Print line content */
        fwrite(line_text, 1, line_len, stderr);
        fprintf(stderr, "\n");

        /* Caret line */
        fprintf(stderr, "       | ");
        for (uint32_t i = 1; i < diag->span.start.col; i++) {
            fputc(' ', stderr);
        }
        fprintf(stderr, "%s^%s\n", color, COLOR_RESET);
    }

    /* Print note if present */
    if (diag->note) {
        fprintf(stderr, "       = %snote%s: %s\n", COLOR_CYAN, COLOR_RESET, diag->note);
    }

    fprintf(stderr, "\n");
}

void diag_print_all(DiagContext *ctx) {
    for (size_t i = 0; i < vec_len(ctx->diagnostics); i++) {
        diag_print(ctx, &ctx->diagnostics[i]);
    }

    /* Print summary */
    if (ctx->error_count > 0 || ctx->warning_count > 0) {
        fprintf(stderr, "%s%zu error(s), %zu warning(s)%s\n",
                ctx->error_count > 0 ? COLOR_RED : COLOR_YELLOW,
                ctx->error_count, ctx->warning_count,
                COLOR_RESET);
    }
}