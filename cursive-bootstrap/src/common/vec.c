/*
 * Cursive Bootstrap Compiler - Dynamic Vector Implementation
 */

#include "vec.h"

/* Initial capacity for new vectors */
#define VEC_INITIAL_CAP 8

/* Growth factor (multiply by 2) */
#define VEC_GROW(cap) ((cap) ? (cap) * 2 : VEC_INITIAL_CAP)

void *vec_new_internal(size_t elem_size) {
    size_t total = sizeof(VecHeader) + elem_size * VEC_INITIAL_CAP;
    VecHeader *header = (VecHeader *)malloc(total);
    if (!header) {
        CURSIVE_PANIC("Out of memory allocating vector");
    }
    header->len = 0;
    header->cap = VEC_INITIAL_CAP;
    header->elem_size = elem_size;
    return (char *)header + sizeof(VecHeader);
}

void vec_free_internal(void *v) {
    if (v) {
        free(VEC_HEADER(v));
    }
}

void *vec_reserve_internal(void *v, size_t n, size_t elem_size) {
    if (!v) {
        v = vec_new_internal(elem_size);
    }

    VecHeader *header = VEC_HEADER(v);
    if (n <= header->cap) {
        return v; /* Already have enough capacity */
    }

    /* Grow to at least n */
    size_t new_cap = header->cap;
    while (new_cap < n) {
        new_cap = VEC_GROW(new_cap);
    }

    size_t total = sizeof(VecHeader) + elem_size * new_cap;
    VecHeader *new_header = (VecHeader *)realloc(header, total);
    if (!new_header) {
        CURSIVE_PANIC("Out of memory growing vector");
    }
    new_header->cap = new_cap;
    return (char *)new_header + sizeof(VecHeader);
}

void *vec_push_internal(void *v, const void *elem, size_t elem_size) {
    if (!v) {
        v = vec_new_internal(elem_size);
    }

    VecHeader *header = VEC_HEADER(v);
    if (header->len >= header->cap) {
        v = vec_reserve_internal(v, header->len + 1, elem_size);
        header = VEC_HEADER(v);
    }

    memcpy((char *)v + header->len * elem_size, elem, elem_size);
    header->len++;
    return v;
}

void vec_pop_internal(void *v) {
    if (v) {
        VecHeader *header = VEC_HEADER(v);
        if (header->len > 0) {
            header->len--;
        }
    }
}

void vec_clear_internal(void *v) {
    if (v) {
        VEC_HEADER(v)->len = 0;
    }
}