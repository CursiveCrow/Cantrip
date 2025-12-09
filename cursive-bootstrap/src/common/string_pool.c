/*
 * Cursive Bootstrap Compiler - String Pool Implementation
 */

#include "string_pool.h"

/* Initial hash table capacity */
#define POOL_INITIAL_CAP 256

/* Load factor threshold for rehashing (0.75) */
#define POOL_LOAD_THRESHOLD 3/4

/* FNV-1a hash parameters */
#define FNV_OFFSET 14695981039346656037ULL
#define FNV_PRIME  1099511628211ULL

uint64_t string_hash(const char *str, size_t len) {
    uint64_t hash = FNV_OFFSET;
    for (size_t i = 0; i < len; i++) {
        hash ^= (uint8_t)str[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

void string_pool_init(StringPool *pool) {
    pool->capacity = POOL_INITIAL_CAP;
    pool->count = 0;
    pool->entries = (InternedString *)calloc(pool->capacity, sizeof(InternedString));
    if (!pool->entries) {
        CURSIVE_PANIC("Out of memory allocating string pool");
    }
    arena_init(&pool->arena);
}

void string_pool_destroy(StringPool *pool) {
    free(pool->entries);
    pool->entries = NULL;
    pool->count = 0;
    pool->capacity = 0;
    arena_destroy(&pool->arena);
}

/* Find entry in table (returns pointer to slot) */
static InternedString *pool_find(StringPool *pool, const char *str, size_t len, uint64_t hash) {
    size_t mask = pool->capacity - 1;
    size_t idx = hash & mask;

    for (;;) {
        InternedString *entry = &pool->entries[idx];

        /* Empty slot - not found */
        if (entry->data == NULL) {
            return entry;
        }

        /* Check if this is the string we're looking for */
        if (entry->hash == hash && entry->len == len &&
            memcmp(entry->data, str, len) == 0) {
            return entry;
        }

        /* Linear probing */
        idx = (idx + 1) & mask;
    }
}

/* Grow and rehash the table */
static void pool_rehash(StringPool *pool) {
    size_t old_capacity = pool->capacity;
    InternedString *old_entries = pool->entries;

    pool->capacity *= 2;
    pool->entries = (InternedString *)calloc(pool->capacity, sizeof(InternedString));
    if (!pool->entries) {
        CURSIVE_PANIC("Out of memory growing string pool");
    }

    /* Reinsert all entries */
    for (size_t i = 0; i < old_capacity; i++) {
        InternedString *old = &old_entries[i];
        if (old->data != NULL) {
            InternedString *slot = pool_find(pool, old->data, old->len, old->hash);
            *slot = *old;
        }
    }

    free(old_entries);
}

InternedString string_pool_intern_len(StringPool *pool, const char *str, size_t len) {
    if (str == NULL || len == 0) {
        return interned_null();
    }

    uint64_t hash = string_hash(str, len);

    /* Check if already interned */
    InternedString *slot = pool_find(pool, str, len, hash);
    if (slot->data != NULL) {
        return *slot; /* Already exists */
    }

    /* Check if we need to grow */
    if (pool->count >= pool->capacity * POOL_LOAD_THRESHOLD) {
        pool_rehash(pool);
        slot = pool_find(pool, str, len, hash);
    }

    /* Copy string into arena and insert */
    char *copy = arena_strndup(&pool->arena, str, len);
    slot->data = copy;
    slot->len = len;
    slot->hash = hash;
    pool->count++;

    return *slot;
}

InternedString string_pool_intern(StringPool *pool, const char *str) {
    if (str == NULL) {
        return interned_null();
    }
    return string_pool_intern_len(pool, str, strlen(str));
}

bool interned_eq_str(InternedString a, const char *str) {
    if (a.data == NULL) {
        return str == NULL || str[0] == '\0';
    }
    if (str == NULL) {
        return false;
    }
    return strcmp(a.data, str) == 0;
}