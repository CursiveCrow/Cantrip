/*
 * Cursive Bootstrap Compiler - Dynamic Vector
 *
 * Type-safe dynamic arrays using macros.
 */

#ifndef CURSIVE_VEC_H
#define CURSIVE_VEC_H

#include "common.h"

/* Internal vector header (stored before data) */
typedef struct VecHeader {
    size_t len;      /* Number of elements */
    size_t cap;      /* Capacity (elements) */
    size_t elem_size; /* Size of each element */
} VecHeader;

/* Get the header from a vec pointer */
#define VEC_HEADER(v) ((VecHeader *)((char *)(v) - sizeof(VecHeader)))

/* Type alias for a vector of type T: Vec(T) */
#define Vec(T) T*

/* Initialize a new empty vector */
void *vec_new_internal(size_t elem_size);

/* Free a vector */
void vec_free_internal(void *v);

/* Ensure capacity for at least n elements */
void *vec_reserve_internal(void *v, size_t n, size_t elem_size);

/* Push an element (returns new vec pointer) */
void *vec_push_internal(void *v, const void *elem, size_t elem_size);

/* Pop an element */
void vec_pop_internal(void *v);

/* Clear all elements (but keep capacity) */
void vec_clear_internal(void *v);

/* Get length */
static inline size_t vec_len_internal(const void *v) {
    return v ? VEC_HEADER(v)->len : 0;
}

/* Get capacity */
static inline size_t vec_cap_internal(const void *v) {
    return v ? VEC_HEADER(v)->cap : 0;
}

/* Check if empty */
static inline bool vec_is_empty_internal(const void *v) {
    return vec_len_internal(v) == 0;
}

/* === Type-safe macros === */

/* Create a new vector of type T */
#define vec_new(T) ((T *)vec_new_internal(sizeof(T)))

/* Free a vector */
#define vec_free(v) (vec_free_internal(v), (v) = NULL)

/* Get length */
#define vec_len(v) vec_len_internal(v)

/* Get capacity */
#define vec_cap(v) vec_cap_internal(v)

/* Check if empty */
#define vec_is_empty(v) vec_is_empty_internal(v)

/* Reserve capacity for n elements */
#define vec_reserve(v, n) ((v) = vec_reserve_internal((v), (n), sizeof(*(v))))

/* Push an element (pass by value) */
#ifdef CURSIVE_COMPILER_MSVC
/* MSVC doesn't have __typeof__, so we rely on the vector element type */
#define vec_push(v, elem) do { \
    (v) = vec_reserve_internal((v), vec_len_internal(v) + 1, sizeof(*(v))); \
    (v)[vec_len_internal(v)] = (elem); \
    VEC_HEADER(v)->len++; \
} while (0)
#else
#define vec_push(v, elem) do { \
    __typeof__(elem) _tmp = (elem); \
    (v) = vec_push_internal((v), &_tmp, sizeof(_tmp)); \
} while (0)
#endif

/* Pop the last element */
#define vec_pop(v) vec_pop_internal(v)

/* Clear all elements */
#define vec_clear(v) vec_clear_internal(v)

/* Get last element */
#define vec_last(v) ((v)[vec_len(v) - 1])

/* Get element at index (with bounds checking in debug) */
#define vec_at(v, i) ((v)[(i)])

/* Iterate over vector */
#define vec_foreach(v, var) \
    for (size_t _i = 0; _i < vec_len(v) && ((var) = (v)[_i], 1); _i++)

/* Iterate with index */
#define vec_foreach_idx(v, i, var) \
    for (size_t i = 0; i < vec_len(v) && ((var) = (v)[i], 1); i++)

#endif /* CURSIVE_VEC_H */