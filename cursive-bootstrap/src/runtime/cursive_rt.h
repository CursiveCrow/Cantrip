/*
 * Cursive Bootstrap Compiler - Runtime Library Header
 *
 * Minimal runtime support for compiled Cursive programs.
 * Provides:
 *   - Panic/abort functionality
 *   - Memory allocation wrappers
 *   - String types and operations
 *   - Capability stubs for I/O (bootstrap only)
 */

#ifndef CURSIVE_RT_H
#define CURSIVE_RT_H

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
typedef int ssize_t;
#endif

/* ============================================
 * Panic / Abort
 * ============================================ */

/* Terminate program with error message */
void cursive_panic(const char *msg, const char *file, int line);

/* Abort macro for convenience */
#define CURSIVE_PANIC(msg) cursive_panic((msg), __FILE__, __LINE__)

/* ============================================
 * Memory Allocation
 * ============================================ */

/* Allocate memory with specified alignment */
void *cursive_alloc(size_t size, size_t align);

/* Deallocate memory */
void cursive_dealloc(void *ptr, size_t size, size_t align);

/* Reallocate memory (size only, alignment preserved) */
void *cursive_realloc(void *ptr, size_t old_size, size_t new_size, size_t align);

/* Zero-initialize allocated memory */
void *cursive_alloc_zeroed(size_t size, size_t align);

/* ============================================
 * String Types
 * ============================================ */

/* string@View - non-owning, immutable view of string data */
typedef struct {
    const char *ptr;
    size_t len;
} cursive_string_view;

/* string@Managed - owned, growable string buffer */
typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} cursive_string_managed;

/* ============================================
 * String Operations
 * ============================================ */

/* Create managed string from view (copies data) */
cursive_string_managed cursive_string_from_view(cursive_string_view v);

/* Create managed string from C string */
cursive_string_managed cursive_string_from_cstr(const char *s);

/* Create view from managed string */
cursive_string_view cursive_string_as_view(const cursive_string_managed *s);

/* Create view from C string */
cursive_string_view cursive_string_view_from_cstr(const char *s);

/* Get null-terminated C string (may reallocate) */
const char *cursive_string_to_cstr(cursive_string_managed *s);

/* Drop (deallocate) managed string */
void cursive_string_drop(cursive_string_managed *s);

/* Append to managed string */
void cursive_string_append(cursive_string_managed *s, cursive_string_view suffix);

/* Compare strings for equality */
int cursive_string_eq(cursive_string_view a, cursive_string_view b);

/* ============================================
 * Capability Stubs ($FileSystem)
 *
 * These are minimal wrappers around system calls
 * for the bootstrap compiler. The full implementation
 * would have proper capability tracking.
 * ============================================ */

/* File open flags */
#define CURSIVE_FS_READ     0x01
#define CURSIVE_FS_WRITE    0x02
#define CURSIVE_FS_CREATE   0x04
#define CURSIVE_FS_TRUNCATE 0x08
#define CURSIVE_FS_APPEND   0x10

/* Open a file, returns file descriptor or -1 on error */
int cursive_fs_open(const char *path, int flags);

/* Read from file descriptor */
ssize_t cursive_fs_read(int fd, void *buf, size_t count);

/* Write to file descriptor */
ssize_t cursive_fs_write(int fd, const void *buf, size_t count);

/* Close file descriptor */
int cursive_fs_close(int fd);

/* Write to stdout */
ssize_t cursive_fs_write_stdout(const void *buf, size_t count);

/* Write to stderr */
ssize_t cursive_fs_write_stderr(const void *buf, size_t count);

/* Read from stdin */
ssize_t cursive_fs_read_stdin(void *buf, size_t count);

/* ============================================
 * Slice Type
 * ============================================ */

/* Generic slice (used for arrays/vectors) */
typedef struct {
    void *ptr;
    size_t len;
} cursive_slice;

/* ============================================
 * Integer Operations (overflow-checked)
 * ============================================ */

/* Check for addition overflow */
int cursive_add_overflow_i32(int32_t a, int32_t b, int32_t *result);
int cursive_add_overflow_i64(int64_t a, int64_t b, int64_t *result);

/* Check for subtraction overflow */
int cursive_sub_overflow_i32(int32_t a, int32_t b, int32_t *result);
int cursive_sub_overflow_i64(int64_t a, int64_t b, int64_t *result);

/* Check for multiplication overflow */
int cursive_mul_overflow_i32(int32_t a, int32_t b, int32_t *result);
int cursive_mul_overflow_i64(int64_t a, int64_t b, int64_t *result);

#endif /* CURSIVE_RT_H */
