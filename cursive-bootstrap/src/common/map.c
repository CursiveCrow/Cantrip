/*
 * Cursive Bootstrap Compiler - Hash Map Implementation
 */

#include "map.h"

#define MAP_INITIAL_CAP 16
#define MAP_LOAD_THRESHOLD 3/4

void map_init(Map *map) {
    map->capacity = MAP_INITIAL_CAP;
    map->count = 0;
    map->entries = (MapEntry *)calloc(map->capacity, sizeof(MapEntry));
    if (!map->entries) {
        CURSIVE_PANIC("Out of memory allocating map");
    }
}

void map_destroy(Map *map) {
    free(map->entries);
    map->entries = NULL;
    map->count = 0;
    map->capacity = 0;
}

void map_clear(Map *map) {
    memset(map->entries, 0, map->capacity * sizeof(MapEntry));
    map->count = 0;
}

/* Find entry slot for key */
static MapEntry *map_find(const Map *map, InternedString key) {
    if (map->capacity == 0) return NULL;

    size_t mask = map->capacity - 1;
    size_t idx = key.hash & mask;

    for (;;) {
        MapEntry *entry = &map->entries[idx];

        /* Empty slot */
        if (interned_is_null(entry->key)) {
            return entry;
        }

        /* Found matching key (pointer comparison for interned strings) */
        if (interned_eq(entry->key, key)) {
            return entry;
        }

        idx = (idx + 1) & mask;
    }
}

/* Grow and rehash */
static void map_rehash(Map *map) {
    size_t old_capacity = map->capacity;
    MapEntry *old_entries = map->entries;

    map->capacity *= 2;
    map->entries = (MapEntry *)calloc(map->capacity, sizeof(MapEntry));
    if (!map->entries) {
        CURSIVE_PANIC("Out of memory growing map");
    }

    for (size_t i = 0; i < old_capacity; i++) {
        MapEntry *old = &old_entries[i];
        if (!interned_is_null(old->key)) {
            MapEntry *slot = map_find(map, old->key);
            *slot = *old;
        }
    }

    free(old_entries);
}

void *map_get(const Map *map, InternedString key) {
    if (map->count == 0 || interned_is_null(key)) {
        return NULL;
    }
    MapEntry *entry = map_find(map, key);
    if (entry && !interned_is_null(entry->key)) {
        return entry->value;
    }
    return NULL;
}

void map_set(Map *map, InternedString key, void *value) {
    if (interned_is_null(key)) {
        return;
    }

    if (map->count >= map->capacity * MAP_LOAD_THRESHOLD) {
        map_rehash(map);
    }

    MapEntry *entry = map_find(map, key);
    if (interned_is_null(entry->key)) {
        /* New entry */
        entry->key = key;
        map->count++;
    }
    entry->value = value;
}

bool map_contains(const Map *map, InternedString key) {
    if (map->count == 0 || interned_is_null(key)) {
        return false;
    }
    MapEntry *entry = map_find(map, key);
    return entry && !interned_is_null(entry->key);
}

void *map_remove(Map *map, InternedString key) {
    if (map->count == 0 || interned_is_null(key)) {
        return NULL;
    }

    MapEntry *entry = map_find(map, key);
    if (!entry || interned_is_null(entry->key)) {
        return NULL;
    }

    void *value = entry->value;

    /* Mark as deleted (tombstone) - for now, just clear */
    entry->key = interned_null();
    entry->value = NULL;
    map->count--;

    /* Note: This simple deletion can break linear probing chains.
     * A production implementation would use tombstones properly.
     * For the bootstrap compiler, we rarely remove entries. */

    return value;
}

void map_foreach(const Map *map, MapIterFn fn, void *ctx) {
    if (!map || !fn) return;

    for (size_t i = 0; i < map->capacity; i++) {
        MapEntry *entry = &map->entries[i];
        if (!interned_is_null(entry->key)) {
            fn(entry->key, entry->value, ctx);
        }
    }
}

/* === Pointer-keyed map === */

static inline size_t ptr_hash(const void *ptr) {
    /* Simple pointer hash: mix bits */
    uintptr_t x = (uintptr_t)ptr;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

void ptr_map_init(PtrMap *map) {
    map->capacity = MAP_INITIAL_CAP;
    map->count = 0;
    map->entries = (PtrMapEntry *)calloc(map->capacity, sizeof(PtrMapEntry));
    if (!map->entries) {
        CURSIVE_PANIC("Out of memory allocating ptr_map");
    }
}

void ptr_map_destroy(PtrMap *map) {
    free(map->entries);
    map->entries = NULL;
    map->count = 0;
    map->capacity = 0;
}

static PtrMapEntry *ptr_map_find(const PtrMap *map, const void *key) {
    if (map->capacity == 0) return NULL;

    size_t mask = map->capacity - 1;
    size_t idx = ptr_hash(key) & mask;

    for (;;) {
        PtrMapEntry *entry = &map->entries[idx];
        if (entry->key == NULL || entry->key == key) {
            return entry;
        }
        idx = (idx + 1) & mask;
    }
}

static void ptr_map_rehash(PtrMap *map) {
    size_t old_capacity = map->capacity;
    PtrMapEntry *old_entries = map->entries;

    map->capacity *= 2;
    map->entries = (PtrMapEntry *)calloc(map->capacity, sizeof(PtrMapEntry));
    if (!map->entries) {
        CURSIVE_PANIC("Out of memory growing ptr_map");
    }

    for (size_t i = 0; i < old_capacity; i++) {
        PtrMapEntry *old = &old_entries[i];
        if (old->key != NULL) {
            PtrMapEntry *slot = ptr_map_find(map, old->key);
            *slot = *old;
        }
    }

    free(old_entries);
}

void *ptr_map_get(const PtrMap *map, const void *key) {
    if (map->count == 0 || key == NULL) return NULL;
    PtrMapEntry *entry = ptr_map_find(map, key);
    return (entry && entry->key != NULL) ? entry->value : NULL;
}

void ptr_map_set(PtrMap *map, const void *key, void *value) {
    if (key == NULL) return;

    if (map->count >= map->capacity * MAP_LOAD_THRESHOLD) {
        ptr_map_rehash(map);
    }

    PtrMapEntry *entry = ptr_map_find(map, key);
    if (entry->key == NULL) {
        entry->key = key;
        map->count++;
    }
    entry->value = value;
}

bool ptr_map_contains(const PtrMap *map, const void *key) {
    if (map->count == 0 || key == NULL) return false;
    PtrMapEntry *entry = ptr_map_find(map, key);
    return entry && entry->key != NULL;
}