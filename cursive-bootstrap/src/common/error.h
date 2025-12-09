/*
 * Cursive Bootstrap Compiler - Error Reporting
 *
 * Diagnostic messages with source locations, following Cursive spec error codes.
 */

#ifndef CURSIVE_ERROR_H
#define CURSIVE_ERROR_H

#include "common.h"
#include "vec.h"

/* Diagnostic severity levels */
typedef enum DiagLevel {
    DIAG_NOTE,      /* Informational note */
    DIAG_WARNING,   /* Warning (compilation continues) */
    DIAG_ERROR,     /* Error (compilation may continue, but will fail) */
    DIAG_FATAL      /* Fatal error (compilation stops immediately) */
} DiagLevel;

/* Diagnostic message */
typedef struct Diagnostic {
    DiagLevel level;
    const char *code;    /* Error code like "E-TYP-1601" */
    SourceSpan span;     /* Location in source */
    char *message;       /* Formatted message (owned) */
    char *note;          /* Optional additional note (owned) */
} Diagnostic;

/* Source file info for error reporting */
typedef struct SourceFile {
    const char *path;    /* File path */
    const char *content; /* File contents */
    size_t len;          /* Content length */
    Vec(size_t) lines;   /* Line start offsets */
} SourceFile;

/* Diagnostic context */
typedef struct DiagContext {
    Vec(SourceFile) files;      /* Loaded source files */
    Vec(Diagnostic) diagnostics; /* Collected diagnostics */
    size_t error_count;          /* Number of errors */
    size_t warning_count;        /* Number of warnings */
    bool fatal_occurred;         /* Fatal error occurred */
} DiagContext;

/* Initialize diagnostic context */
void diag_init(DiagContext *ctx);

/* Destroy diagnostic context */
void diag_destroy(DiagContext *ctx);

/* Add a source file and return its file_id */
uint32_t diag_add_file(DiagContext *ctx, const char *path, const char *content, size_t len);

/* Get source file by id */
SourceFile *diag_get_file(DiagContext *ctx, uint32_t file_id);

/* Report a diagnostic */
void diag_report(DiagContext *ctx, DiagLevel level, const char *code,
                 SourceSpan span, const char *fmt, ...);

/* Report with additional note */
void diag_report_note(DiagContext *ctx, DiagLevel level, const char *code,
                      SourceSpan span, const char *note, const char *fmt, ...);

/* Print all diagnostics to stderr */
void diag_print_all(DiagContext *ctx);

/* Print a single diagnostic */
void diag_print(DiagContext *ctx, const Diagnostic *diag);

/* Check if any errors occurred */
static inline bool diag_has_errors(const DiagContext *ctx) {
    return ctx->error_count > 0 || ctx->fatal_occurred;
}

/* Get line/column for a byte offset in a file */
void diag_offset_to_loc(const SourceFile *file, size_t offset,
                        uint32_t *line, uint32_t *col);

/* Get the source line text for a location */
const char *diag_get_line_text(const SourceFile *file, uint32_t line, size_t *len);

/* === Common error codes (from Cursive spec) === */

/* Lexical errors */
#define E_LEX_0001 "E-LEX-0001" /* Invalid character */
#define E_LEX_0002 "E-LEX-0002" /* Unterminated string literal */
#define E_LEX_0003 "E-LEX-0003" /* Invalid escape sequence */
#define E_LEX_0004 "E-LEX-0004" /* Invalid numeric literal */
#define E_LEX_0005 "E-LEX-0005" /* Nested comment not closed */

/* Parser errors */
#define E_SYN_0100 "E-SYN-0100" /* Unexpected token */
#define E_SYN_0101 "E-SYN-0101" /* Expected expression */
#define E_SYN_0102 "E-SYN-0102" /* Expected type */
#define E_SYN_0103 "E-SYN-0103" /* Expected pattern */
#define E_SYN_0104 "E-SYN-0104" /* Expected identifier */
#define E_SYN_0105 "E-SYN-0105" /* Missing closing bracket */

/* Name resolution errors */
#define E_RES_0200 "E-RES-0200" /* Undefined identifier */
#define E_RES_0201 "E-RES-0201" /* Duplicate definition */
#define E_RES_0202 "E-RES-0202" /* Cannot access private member */
#define E_RES_0203 "E-RES-0203" /* Unresolved import */

/* Type errors */
#define E_TYP_1601 "E-TYP-1601" /* Mutation through const path */
#define E_TYP_1602 "E-TYP-1602" /* Unique permission violation (aliasing) */
#define E_TYP_1603 "E-TYP-1603" /* Type mismatch */
#define E_TYP_1604 "E-TYP-1604" /* Missing class implementation */
#define E_TYP_2052 "E-TYP-2052" /* Invalid state field access */
#define E_TYP_2053 "E-TYP-2053" /* Invalid state method invocation */
#define E_TYP_2060 "E-TYP-2060" /* Non-exhaustive modal match */

/* Memory/move errors */
#define E_MEM_3001 "E-MEM-3001" /* Access to moved binding */
#define E_MEM_3003 "E-MEM-3003" /* Reassignment of immutable binding */
#define E_MEM_3006 "E-MEM-3006" /* Move from immovable binding */
#define E_MEM_3007 "E-MEM-3007" /* Use of uninitialized binding */

/* Expression errors */
#define E_EXP_2537 "E-EXP-2537" /* Method call using . instead of ~> */

#endif /* CURSIVE_ERROR_H */