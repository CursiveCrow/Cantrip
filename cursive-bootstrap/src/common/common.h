/*
 * Cursive Bootstrap Compiler - Common Definitions
 *
 * Core types and macros used throughout the compiler.
 */

#ifndef CURSIVE_COMMON_H
#define CURSIVE_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* Platform detection */
#if defined(_WIN32) || defined(_WIN64)
    #define CURSIVE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define CURSIVE_PLATFORM_LINUX 1
#elif defined(__APPLE__)
    #define CURSIVE_PLATFORM_MACOS 1
#else
    #error "Unsupported platform"
#endif

/* Compiler detection */
#if defined(_MSC_VER)
    #define CURSIVE_COMPILER_MSVC 1
    #define CURSIVE_INLINE __forceinline
    #define CURSIVE_NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
    #define CURSIVE_COMPILER_GCC 1
    #define CURSIVE_INLINE __attribute__((always_inline)) inline
    #define CURSIVE_NORETURN __attribute__((noreturn))
#else
    #define CURSIVE_INLINE inline
    #define CURSIVE_NORETURN
#endif

/* Utility macros */
#define CURSIVE_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define CURSIVE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define CURSIVE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define CURSIVE_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

/* Memory alignment */
#define CURSIVE_DEFAULT_ALIGN (sizeof(void*))

/* Panic/assertion */
CURSIVE_NORETURN void cursive_panic(const char *msg, const char *file, int line);

#define CURSIVE_PANIC(msg) cursive_panic(msg, __FILE__, __LINE__)
#define CURSIVE_ASSERT(cond) do { if (!(cond)) CURSIVE_PANIC("Assertion failed: " #cond); } while(0)
#define CURSIVE_UNREACHABLE() CURSIVE_PANIC("Unreachable code reached")

/* Source location tracking */
typedef struct SourceLoc {
    uint32_t file_id;   /* Index into file table */
    uint32_t line;      /* 1-indexed line number */
    uint32_t col;       /* 1-indexed column (byte offset in line) */
} SourceLoc;

typedef struct SourceSpan {
    SourceLoc start;
    SourceLoc end;
} SourceSpan;

/* Create a source span from two locations */
static inline SourceSpan span_new(SourceLoc start, SourceLoc end) {
    return (SourceSpan){ .start = start, .end = end };
}

/* Create a single-point span */
static inline SourceSpan span_point(SourceLoc loc) {
    return (SourceSpan){ .start = loc, .end = loc };
}

/* Merge two spans (assumes they're in the same file) */
static inline SourceSpan span_merge(SourceSpan a, SourceSpan b) {
    SourceSpan result;
    result.start = (a.start.line < b.start.line ||
                   (a.start.line == b.start.line && a.start.col < b.start.col))
                   ? a.start : b.start;
    result.end = (a.end.line > b.end.line ||
                 (a.end.line == b.end.line && a.end.col > b.end.col))
                 ? a.end : b.end;
    return result;
}

#endif /* CURSIVE_COMMON_H */