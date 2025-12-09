/*
 * Cursive Bootstrap Compiler - Arena Allocator Implementation
 */

#include "arena.h"
#include <stdarg.h>

#ifdef CURSIVE_PLATFORM_WINDOWS
    #include <malloc.h>
    #define aligned_alloc(align, size) _aligned_malloc(size, align)
    #define aligned_free(ptr) _aligned_free(ptr)
#else
    #define aligned_free(ptr) free(ptr)
#endif

/* Create a new block with given size */
static ArenaBlock *arena_new_block(size_t size) {
    /* Ensure minimum size for block header */
    size_t total_size = sizeof(ArenaBlock) + size;
    ArenaBlock *block = (ArenaBlock *)malloc(total_size);
    if (!block) {
        CURSIVE_PANIC("Out of memory allocating arena block");
    }
    block->next = NULL;
    block->size = size;
    block->used = 0;
    return block;
}

void arena_init(Arena *arena) {
    arena_init_sized(arena, ARENA_DEFAULT_BLOCK_SIZE);
}

void arena_init_sized(Arena *arena, size_t block_size) {
    arena->default_block_size = block_size;
    arena->total_allocated = 0;
    arena->first = arena_new_block(block_size);
    arena->current = arena->first;
}

void arena_destroy(Arena *arena) {
    ArenaBlock *block = arena->first;
    while (block) {
        ArenaBlock *next = block->next;
        free(block);
        block = next;
    }
    arena->first = NULL;
    arena->current = NULL;
    arena->total_allocated = 0;
}

void arena_reset(Arena *arena) {
    /* Reset all blocks to empty */
    ArenaBlock *block = arena->first;
    while (block) {
        block->used = 0;
        block = block->next;
    }
    arena->current = arena->first;
    arena->total_allocated = 0;
}

void *arena_alloc_aligned(Arena *arena, size_t size, size_t align) {
    if (size == 0) {
        return NULL;
    }

    ArenaBlock *block = arena->current;

    /* Align the current position */
    size_t aligned_used = CURSIVE_ALIGN_UP(block->used, align);

    /* Check if allocation fits in current block */
    if (aligned_used + size > block->size) {
        /* Need a new block */
        size_t new_block_size = CURSIVE_MAX(arena->default_block_size, size + align);
        ArenaBlock *new_block = arena_new_block(new_block_size);
        block->next = new_block;
        arena->current = new_block;
        block = new_block;
        aligned_used = CURSIVE_ALIGN_UP(0, align);
    }

    void *ptr = block->data + aligned_used;
    block->used = aligned_used + size;
    arena->total_allocated += size;

    return ptr;
}

void *arena_alloc(Arena *arena, size_t size) {
    return arena_alloc_aligned(arena, size, CURSIVE_DEFAULT_ALIGN);
}

void *arena_calloc(Arena *arena, size_t count, size_t size) {
    size_t total = count * size;
    void *ptr = arena_alloc(arena, total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

char *arena_strdup(Arena *arena, const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char *copy = (char *)arena_alloc(arena, len + 1);
    memcpy(copy, str, len + 1);
    return copy;
}

char *arena_strndup(Arena *arena, const char *str, size_t len) {
    if (!str) return NULL;
    char *copy = (char *)arena_alloc(arena, len + 1);
    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

char *arena_sprintf(Arena *arena, const char *fmt, ...) {
    va_list args1, args2;
    va_start(args1, fmt);
    va_copy(args2, args1);

    /* First pass: determine size needed */
    int size = vsnprintf(NULL, 0, fmt, args1);
    va_end(args1);

    if (size < 0) {
        va_end(args2);
        return NULL;
    }

    /* Allocate and format */
    char *buf = (char *)arena_alloc(arena, (size_t)size + 1);
    vsnprintf(buf, (size_t)size + 1, fmt, args2);
    va_end(args2);

    return buf;
}

size_t arena_total_allocated(const Arena *arena) {
    return arena->total_allocated;
}

/* Panic implementation */
void cursive_panic(const char *msg, const char *file, int line) {
    fprintf(stderr, "PANIC at %s:%d: %s\n", file, line, msg);
    abort();
}