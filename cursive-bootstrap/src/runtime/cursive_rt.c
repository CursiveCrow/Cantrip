/*
 * Cursive Bootstrap Compiler - Runtime Library Implementation
 *
 * Minimal runtime support for compiled Cursive programs.
 */

#include "cursive_rt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#define aligned_alloc(align, size) _aligned_malloc((size), (align))
#define aligned_free(ptr) _aligned_free(ptr)
#define open _open
#define read _read
#define write _write
#define close _close
#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_APPEND _O_APPEND
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#else
#include <unistd.h>
#include <fcntl.h>
#define aligned_free(ptr) free(ptr)
#endif

/* ============================================
 * Panic / Abort
 * ============================================ */

void cursive_panic(const char *msg, const char *file, int line) {
    fprintf(stderr, "PANIC at %s:%d: %s\n", file, line, msg);
    fflush(stderr);
    abort();
}

/* ============================================
 * Memory Allocation
 * ============================================ */

void *cursive_alloc(size_t size, size_t align) {
    if (size == 0) return NULL;

    /* Ensure alignment is at least sizeof(void*) and power of 2 */
    if (align < sizeof(void*)) {
        align = sizeof(void*);
    }

    /* Round up size to alignment */
    size = (size + align - 1) & ~(align - 1);

    void *ptr = aligned_alloc(align, size);
    if (ptr == NULL) {
        cursive_panic("Out of memory", __FILE__, __LINE__);
    }
    return ptr;
}

void cursive_dealloc(void *ptr, size_t size, size_t align) {
    (void)size;
    (void)align;
    if (ptr != NULL) {
        aligned_free(ptr);
    }
}

void *cursive_realloc(void *ptr, size_t old_size, size_t new_size, size_t align) {
    if (new_size == 0) {
        cursive_dealloc(ptr, old_size, align);
        return NULL;
    }

    if (ptr == NULL) {
        return cursive_alloc(new_size, align);
    }

    /* Allocate new block and copy */
    void *new_ptr = cursive_alloc(new_size, align);
    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    cursive_dealloc(ptr, old_size, align);

    return new_ptr;
}

void *cursive_alloc_zeroed(size_t size, size_t align) {
    void *ptr = cursive_alloc(size, align);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

/* ============================================
 * String Operations
 * ============================================ */

cursive_string_managed cursive_string_from_view(cursive_string_view v) {
    cursive_string_managed s;

    if (v.len == 0 || v.ptr == NULL) {
        s.ptr = NULL;
        s.len = 0;
        s.cap = 0;
        return s;
    }

    s.cap = v.len + 1; /* Add space for null terminator */
    s.ptr = (char *)cursive_alloc(s.cap, 1);
    memcpy(s.ptr, v.ptr, v.len);
    s.ptr[v.len] = '\0';
    s.len = v.len;

    return s;
}

cursive_string_managed cursive_string_from_cstr(const char *cstr) {
    cursive_string_view v;
    v.ptr = cstr;
    v.len = cstr ? strlen(cstr) : 0;
    return cursive_string_from_view(v);
}

cursive_string_view cursive_string_as_view(const cursive_string_managed *s) {
    cursive_string_view v;
    v.ptr = s->ptr;
    v.len = s->len;
    return v;
}

cursive_string_view cursive_string_view_from_cstr(const char *cstr) {
    cursive_string_view v;
    v.ptr = cstr;
    v.len = cstr ? strlen(cstr) : 0;
    return v;
}

const char *cursive_string_to_cstr(cursive_string_managed *s) {
    if (s->ptr == NULL) {
        return "";
    }
    /* Ensure null terminated */
    if (s->len >= s->cap) {
        /* Need to reallocate to add null terminator */
        s->cap = s->len + 1;
        s->ptr = (char *)cursive_realloc(s->ptr, s->len, s->cap, 1);
    }
    s->ptr[s->len] = '\0';
    return s->ptr;
}

void cursive_string_drop(cursive_string_managed *s) {
    if (s->ptr != NULL) {
        cursive_dealloc(s->ptr, s->cap, 1);
        s->ptr = NULL;
        s->len = 0;
        s->cap = 0;
    }
}

void cursive_string_append(cursive_string_managed *s, cursive_string_view suffix) {
    if (suffix.len == 0) return;

    size_t new_len = s->len + suffix.len;
    if (new_len + 1 > s->cap) {
        /* Grow capacity (at least double) */
        size_t new_cap = s->cap * 2;
        if (new_cap < new_len + 1) {
            new_cap = new_len + 1;
        }
        s->ptr = (char *)cursive_realloc(s->ptr, s->cap, new_cap, 1);
        s->cap = new_cap;
    }

    memcpy(s->ptr + s->len, suffix.ptr, suffix.len);
    s->len = new_len;
    s->ptr[s->len] = '\0';
}

int cursive_string_eq(cursive_string_view a, cursive_string_view b) {
    if (a.len != b.len) return 0;
    if (a.len == 0) return 1;
    return memcmp(a.ptr, b.ptr, a.len) == 0;
}

/* ============================================
 * Capability Stubs ($FileSystem)
 * ============================================ */

int cursive_fs_open(const char *path, int flags) {
    int os_flags = 0;

    if (flags & CURSIVE_FS_READ) {
        if (flags & CURSIVE_FS_WRITE) {
            os_flags |= O_RDWR;
        } else {
            os_flags |= O_RDONLY;
        }
    } else if (flags & CURSIVE_FS_WRITE) {
        os_flags |= O_WRONLY;
    }

    if (flags & CURSIVE_FS_CREATE) {
        os_flags |= O_CREAT;
    }
    if (flags & CURSIVE_FS_TRUNCATE) {
        os_flags |= O_TRUNC;
    }
    if (flags & CURSIVE_FS_APPEND) {
        os_flags |= O_APPEND;
    }

#ifdef _WIN32
    os_flags |= _O_BINARY;
#endif

    return open(path, os_flags, 0644);
}

ssize_t cursive_fs_read(int fd, void *buf, size_t count) {
    return read(fd, buf, (unsigned int)count);
}

ssize_t cursive_fs_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, (unsigned int)count);
}

int cursive_fs_close(int fd) {
    return close(fd);
}

ssize_t cursive_fs_write_stdout(const void *buf, size_t count) {
    return write(STDOUT_FILENO, buf, (unsigned int)count);
}

ssize_t cursive_fs_write_stderr(const void *buf, size_t count) {
    return write(STDERR_FILENO, buf, (unsigned int)count);
}

ssize_t cursive_fs_read_stdin(void *buf, size_t count) {
    return read(STDIN_FILENO, buf, (unsigned int)count);
}

/* ============================================
 * Integer Operations (overflow-checked)
 * ============================================ */

#if defined(__GNUC__) || defined(__clang__)
/* Use compiler builtins when available */

int cursive_add_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    return __builtin_add_overflow(a, b, result);
}

int cursive_add_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    return __builtin_add_overflow(a, b, result);
}

int cursive_sub_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    return __builtin_sub_overflow(a, b, result);
}

int cursive_sub_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    return __builtin_sub_overflow(a, b, result);
}

int cursive_mul_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    return __builtin_mul_overflow(a, b, result);
}

int cursive_mul_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    return __builtin_mul_overflow(a, b, result);
}

#else
/* Portable implementations */

int cursive_add_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    int64_t r = (int64_t)a + (int64_t)b;
    *result = (int32_t)r;
    return r < INT32_MIN || r > INT32_MAX;
}

int cursive_add_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    /* Check for overflow before computing */
    if (b > 0 && a > INT64_MAX - b) {
        *result = 0;
        return 1;
    }
    if (b < 0 && a < INT64_MIN - b) {
        *result = 0;
        return 1;
    }
    *result = a + b;
    return 0;
}

int cursive_sub_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    int64_t r = (int64_t)a - (int64_t)b;
    *result = (int32_t)r;
    return r < INT32_MIN || r > INT32_MAX;
}

int cursive_sub_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    if (b < 0 && a > INT64_MAX + b) {
        *result = 0;
        return 1;
    }
    if (b > 0 && a < INT64_MIN + b) {
        *result = 0;
        return 1;
    }
    *result = a - b;
    return 0;
}

int cursive_mul_overflow_i32(int32_t a, int32_t b, int32_t *result) {
    int64_t r = (int64_t)a * (int64_t)b;
    *result = (int32_t)r;
    return r < INT32_MIN || r > INT32_MAX;
}

int cursive_mul_overflow_i64(int64_t a, int64_t b, int64_t *result) {
    /* Handle special cases */
    if (a == 0 || b == 0) {
        *result = 0;
        return 0;
    }
    if (a == 1) {
        *result = b;
        return 0;
    }
    if (b == 1) {
        *result = a;
        return 0;
    }

    /* Check for overflow */
    if (a > 0) {
        if (b > 0) {
            if (a > INT64_MAX / b) { *result = 0; return 1; }
        } else {
            if (b < INT64_MIN / a) { *result = 0; return 1; }
        }
    } else {
        if (b > 0) {
            if (a < INT64_MIN / b) { *result = 0; return 1; }
        } else {
            if (a != 0 && b < INT64_MAX / a) { *result = 0; return 1; }
        }
    }

    *result = a * b;
    return 0;
}

#endif /* __GNUC__ */
