/*
 * Cursive Bootstrap Compiler - Scope Management
 *
 * Implements lexical scoping for name resolution.
 * Scope hierarchy: Universe -> Module -> Block/Type
 */

#ifndef CURSIVE_SCOPE_H
#define CURSIVE_SCOPE_H

#include "common/common.h"
#include "common/arena.h"
#include "common/map.h"
#include "common/string_pool.h"
#include "parser/ast.h"

/* Forward declarations */
typedef struct Scope Scope;
typedef struct Symbol Symbol;
typedef struct Type Type;

/*
 * Scope kinds matching the Cursive specification
 */
typedef enum ScopeKind {
    SCOPE_UNIVERSE,   /* Built-in types (i32, bool, etc.) */
    SCOPE_MODULE,     /* Top-level module declarations */
    SCOPE_BLOCK,      /* Local block (procedure body, if/loop, etc.) */
    SCOPE_TYPE,       /* Type scope (Self, generic parameters) */
    SCOPE_LOOP        /* Loop scope (for break/continue labels) */
} ScopeKind;

/*
 * Symbol kinds
 */
typedef enum SymbolKind {
    SYM_TYPE,         /* Record, Enum, Modal, TypeAlias */
    SYM_PROC,         /* Procedure */
    SYM_CLASS,        /* Class (interface) */
    SYM_VAR,          /* Local variable (let/var binding) */
    SYM_PARAM,        /* Procedure parameter */
    SYM_FIELD,        /* Record/Modal field */
    SYM_VARIANT,      /* Enum variant */
    SYM_STATE,        /* Modal state */
    SYM_GENERIC,      /* Generic type parameter */
    SYM_MODULE,       /* Module */
    SYM_LOOP_LABEL    /* Loop label */
} SymbolKind;

/*
 * Symbol definition
 */
struct Symbol {
    SymbolKind kind;
    InternedString name;
    Visibility vis;
    Decl *decl;              /* Declaration AST node (may be NULL for built-ins) */
    Type *type;              /* Resolved type (filled by type checker) */
    Scope *defining_scope;   /* Scope where this symbol is defined */

    /* For variables/parameters: binding information */
    bool is_mutable;         /* var vs let */
    BindingOp binding_op;    /* = vs := */

    /* For generic parameters */
    size_t generic_index;    /* Index in generic parameter list */

    SourceSpan span;
};

/*
 * Scope definition
 */
struct Scope {
    ScopeKind kind;
    Scope *parent;           /* Enclosing scope (NULL for universe) */
    Map symbols;             /* InternedString -> Symbol* */

    /* For module scopes */
    InternedString module_name;

    /* For type scopes */
    Symbol *self_type;       /* The Self type symbol */

    /* For loop scopes */
    InternedString loop_label;

    /* For tracking imports */
    Vec(Scope *) imported_modules;
};

/*
 * Scope context for resolution
 */
typedef struct ScopeContext {
    Arena *arena;
    Scope *universe;         /* Root scope with built-ins */
    Scope *current;          /* Current active scope */
    StringPool *strings;     /* For interning names */
} ScopeContext;

/* Initialize scope context with universe scope */
void scope_ctx_init(ScopeContext *ctx, Arena *arena, StringPool *strings);

/* Create a new scope */
Scope *scope_new(ScopeContext *ctx, ScopeKind kind);

/* Push/pop scopes */
void scope_push(ScopeContext *ctx, Scope *scope);
void scope_pop(ScopeContext *ctx);

/* Create and push a new scope */
Scope *scope_enter(ScopeContext *ctx, ScopeKind kind);

/* Pop current scope and return to parent */
void scope_exit(ScopeContext *ctx);

/* Define a symbol in current scope */
Symbol *scope_define(ScopeContext *ctx, InternedString name, SymbolKind kind,
                     Visibility vis, Decl *decl, SourceSpan span);

/* Look up a symbol in current scope chain */
Symbol *scope_lookup(ScopeContext *ctx, InternedString name);

/* Look up a symbol only in current scope (no parent search) */
Symbol *scope_lookup_local(Scope *scope, InternedString name);

/* Look up a symbol in a specific scope chain starting from given scope */
Symbol *scope_lookup_from(Scope *scope, InternedString name);

/* Check if a name is defined in current scope (not parents) */
bool scope_is_defined_locally(Scope *scope, InternedString name);

/* Create a symbol without adding to scope (for building) */
Symbol *symbol_new(Arena *arena);

/* Add a symbol to a scope */
bool scope_add_symbol(Scope *scope, Symbol *sym);

/* Visibility check: is sym visible from the current scope? */
bool scope_is_visible(Scope *from_scope, Symbol *sym);

/* Get the enclosing module scope */
Scope *scope_enclosing_module(Scope *scope);

/* Get the enclosing type scope (for Self resolution) */
Scope *scope_enclosing_type(Scope *scope);

/* Get enclosing loop scope with optional label */
Scope *scope_enclosing_loop(Scope *scope, InternedString label);

/* Populate universe scope with built-in types */
void scope_populate_universe(ScopeContext *ctx);

#endif /* CURSIVE_SCOPE_H */
