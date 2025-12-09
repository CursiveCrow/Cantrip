/*
 * Cursive Bootstrap Compiler - Hash Map
 *
 * Generic hash map using open addressing with linear probing.
 * Keys are InternedStrings (pointer comparison for fast lookup).
 */

#ifndef CURSIVE_MAP_H
#define CURSIVE_MAP_H

#include "common.h"
#include "string_pool.h"

/* Map entry */
typedef struct MapEntry {
    InternedString key;  /* Key (NULL data means empty slot) */
    void *value;         /* Value pointer */
} MapEntry;

/* Hash map */
typedef struct Map {
    MapEntry *entries;
    size_t count;
    size_t capacity;
} Map;

/* Initialize a map */
void map_init(Map *map);

/* Destroy a map */
void map_destroy(Map *map);

/* Clear all entries (but keep capacity) */
void map_clear(Map *map);

/* Get value for key (returns NULL if not found) */
void *map_get(const Map *map, InternedString key);

/* Insert or update key-value pair */
void map_set(Map *map, InternedString key, void *value);

/* Check if key exists */
bool map_contains(const Map *map, InternedString key);

/* Remove a key (returns removed value, or NULL if not found) */
void *map_remove(Map *map, InternedString key);

/* Get number of entries */
static inline size_t map_len(const Map *map) {
    return map ? map->count : 0;
}

/* Iterate over map entries */
typedef void (*MapIterFn)(InternedString key, void *value, void *ctx);
void map_foreach(const Map *map, MapIterFn fn, void *ctx);

/* Pointer-keyed map (for Type* -> X mappings) */
typedef struct PtrMapEntry {
    const void *key;
    void *value;
} PtrMapEntry;

typedef struct PtrMap {
    PtrMapEntry *entries;
    size_t count;
    size_t capacity;
} PtrMap;

void ptr_map_init(PtrMap *map);
void ptr_map_destroy(PtrMap *map);
void *ptr_map_get(const PtrMap *map, const void *key);
void ptr_map_set(PtrMap *map, const void *key, void *value);
bool ptr_map_contains(const PtrMap *map, const void *key);

#endif /* CURSIVE_MAP_H */