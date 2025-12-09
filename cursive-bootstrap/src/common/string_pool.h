/*
 * Cursive Bootstrap Compiler - String Pool (Interning)
 *
 * Interns strings to enable fast equality comparison via pointer comparison.
 * All interned strings are stored in a hash table and deduplicated.
 */

#ifndef CURSIVE_STRING_POOL_H
#define CURSIVE_STRING_POOL_H

#include "common.h"
#include "arena.h"

/* An interned string - compare by pointer equality */
typedef struct InternedString {
    const char *data;   /* Null-terminated string data */
    size_t len;         /* Length (not including null terminator) */
    uint64_t hash;      /* Precomputed hash */
} InternedString;

/* String pool using open addressing hash table */
typedef struct StringPool {
    InternedString *entries;  /* Hash table entries */
    size_t count;             /* Number of strings interned */
    size_t capacity;          /* Table capacity */
    Arena arena;              /* Storage for string data */
} StringPool;

/* Initialize a string pool */
void string_pool_init(StringPool *pool);

/* Destroy a string pool */
void string_pool_destroy(StringPool *pool);

/* Intern a null-terminated string */
InternedString string_pool_intern(StringPool *pool, const char *str);

/* Intern a string with explicit length */
InternedString string_pool_intern_len(StringPool *pool, const char *str, size_t len);

/* Check if two interned strings are equal (fast - pointer comparison) */
static inline bool interned_eq(InternedString a, InternedString b) {
    return a.data == b.data;
}

/* Check if interned string equals a C string (slower) */
bool interned_eq_str(InternedString a, const char *str);

/* Get null (empty) interned string */
static inline InternedString interned_null(void) {
    return (InternedString){ .data = NULL, .len = 0, .hash = 0 };
}

/* Check if interned string is null/empty */
static inline bool interned_is_null(InternedString s) {
    return s.data == NULL;
}

/* Compute hash of a string (FNV-1a) */
uint64_t string_hash(const char *str, size_t len);

#endif /* CURSIVE_STRING_POOL_H */