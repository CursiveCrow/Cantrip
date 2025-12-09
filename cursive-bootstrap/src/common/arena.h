/*
 * Cursive Bootstrap Compiler - Arena Allocator
 *
 * Region-based memory allocator for AST nodes and other compiler data.
 * Memory is allocated linearly and freed all at once when the arena is destroyed.
 */

#ifndef CURSIVE_ARENA_H
#define CURSIVE_ARENA_H

#include "common.h"

/* Default arena block size: 64KB */
#define ARENA_DEFAULT_BLOCK_SIZE (64 * 1024)

/* A single block in the arena chain */
typedef struct ArenaBlock {
    struct ArenaBlock *next;
    size_t size;
    size_t used;
    char data[];  /* Flexible array member */
} ArenaBlock;

/* Arena allocator */
typedef struct Arena {
    ArenaBlock *current;       /* Current block being allocated from */
    ArenaBlock *first;         /* First block (for iteration/reset) */
    size_t default_block_size; /* Size for new blocks */
    size_t total_allocated;    /* Total bytes allocated (stats) */
} Arena;

/* Initialize an arena with default block size */
void arena_init(Arena *arena);

/* Initialize an arena with custom block size */
void arena_init_sized(Arena *arena, size_t block_size);

/* Destroy an arena, freeing all memory */
void arena_destroy(Arena *arena);

/* Reset an arena, keeping allocated blocks but marking them as empty */
void arena_reset(Arena *arena);

/* Allocate memory from the arena (aligned to default alignment) */
void *arena_alloc(Arena *arena, size_t size);

/* Allocate memory with specific alignment */
void *arena_alloc_aligned(Arena *arena, size_t size, size_t align);

/* Allocate and zero-initialize memory */
void *arena_calloc(Arena *arena, size_t count, size_t size);

/* Duplicate a string into the arena */
char *arena_strdup(Arena *arena, const char *str);

/* Duplicate a string with explicit length (may contain nulls) */
char *arena_strndup(Arena *arena, const char *str, size_t len);

/* Printf-style allocation */
char *arena_sprintf(Arena *arena, const char *fmt, ...);

/* Get total bytes allocated by this arena */
size_t arena_total_allocated(const Arena *arena);

/* Convenience macro for type-safe allocation */
#define ARENA_ALLOC(arena, type) \
    ((type *)arena_alloc_aligned((arena), sizeof(type), _Alignof(type)))

/* Convenience macro for array allocation */
#define ARENA_ALLOC_ARRAY(arena, type, count) \
    ((type *)arena_alloc_aligned((arena), sizeof(type) * (count), _Alignof(type)))

#endif /* CURSIVE_ARENA_H */