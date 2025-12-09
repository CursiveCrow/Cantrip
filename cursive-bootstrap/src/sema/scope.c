/*
 * Cursive Bootstrap Compiler - Scope Management Implementation
 */

#include "scope.h"
#include "types.h"

/* Initialize scope context with universe scope */
void scope_ctx_init(ScopeContext *ctx, Arena *arena, StringPool *strings) {
    ctx->arena = arena;
    ctx->strings = strings;

    /* Create universe scope */
    ctx->universe = scope_new(ctx, SCOPE_UNIVERSE);
    ctx->current = ctx->universe;

    /* Populate with built-in types */
    scope_populate_universe(ctx);
}

/* Create a new scope */
Scope *scope_new(ScopeContext *ctx, ScopeKind kind) {
    Scope *scope = ARENA_ALLOC(ctx->arena, Scope);
    memset(scope, 0, sizeof(Scope));
    scope->kind = kind;
    scope->parent = NULL;
    map_init(&scope->symbols);
    scope->imported_modules = vec_new(Scope *);
    return scope;
}

/* Push a scope onto the scope stack */
void scope_push(ScopeContext *ctx, Scope *scope) {
    scope->parent = ctx->current;
    ctx->current = scope;
}

/* Pop current scope */
void scope_pop(ScopeContext *ctx) {
    if (ctx->current && ctx->current->parent) {
        ctx->current = ctx->current->parent;
    }
}

/* Create and push a new scope */
Scope *scope_enter(ScopeContext *ctx, ScopeKind kind) {
    Scope *scope = scope_new(ctx, kind);
    scope_push(ctx, scope);
    return scope;
}

/* Pop current scope and return to parent */
void scope_exit(ScopeContext *ctx) {
    scope_pop(ctx);
}

/* Create a symbol without adding to scope */
Symbol *symbol_new(Arena *arena) {
    Symbol *sym = ARENA_ALLOC(arena, Symbol);
    memset(sym, 0, sizeof(Symbol));
    return sym;
}

/* Add a symbol to a scope */
bool scope_add_symbol(Scope *scope, Symbol *sym) {
    if (scope_is_defined_locally(scope, sym->name)) {
        return false;  /* Already defined */
    }
    sym->defining_scope = scope;
    map_set(&scope->symbols, sym->name, sym);
    return true;
}

/* Define a symbol in current scope */
Symbol *scope_define(ScopeContext *ctx, InternedString name, SymbolKind kind,
                     Visibility vis, Decl *decl, SourceSpan span) {
    /* Check for redefinition in current scope */
    if (scope_is_defined_locally(ctx->current, name)) {
        return NULL;  /* Error: redefinition */
    }

    Symbol *sym = symbol_new(ctx->arena);
    sym->kind = kind;
    sym->name = name;
    sym->vis = vis;
    sym->decl = decl;
    sym->span = span;
    sym->defining_scope = ctx->current;

    map_set(&ctx->current->symbols, name, sym);
    return sym;
}

/* Look up a symbol only in a specific scope */
Symbol *scope_lookup_local(Scope *scope, InternedString name) {
    if (!scope) return NULL;
    return (Symbol *)map_get(&scope->symbols, name);
}

/* Look up a symbol in a scope chain */
Symbol *scope_lookup_from(Scope *scope, InternedString name) {
    while (scope) {
        Symbol *sym = scope_lookup_local(scope, name);
        if (sym) return sym;

        /* Also check imported modules for module scopes */
        if (scope->kind == SCOPE_MODULE) {
            Scope *imported;
            vec_foreach(scope->imported_modules, imported) {
                sym = scope_lookup_local(imported, name);
                if (sym && scope_is_visible(scope, sym)) {
                    return sym;
                }
            }
        }

        scope = scope->parent;
    }
    return NULL;
}

/* Look up a symbol in current scope chain */
Symbol *scope_lookup(ScopeContext *ctx, InternedString name) {
    return scope_lookup_from(ctx->current, name);
}

/* Check if a name is defined in current scope (not parents) */
bool scope_is_defined_locally(Scope *scope, InternedString name) {
    return scope_lookup_local(scope, name) != NULL;
}

/* Visibility check: is sym visible from the current scope? */
bool scope_is_visible(Scope *from_scope, Symbol *sym) {
    if (!sym) return false;

    switch (sym->vis) {
        case VIS_PUBLIC:
            /* Always visible */
            return true;

        case VIS_INTERNAL:
            /* Visible within same crate - for bootstrap, always true */
            return true;

        case VIS_PROTECTED: {
            /* Visible in defining module and submodules */
            Scope *def_mod = scope_enclosing_module(sym->defining_scope);
            Scope *cur_mod = scope_enclosing_module(from_scope);

            /* Same module? */
            if (def_mod == cur_mod) return true;

            /* cur_mod is a submodule of def_mod?
             * For bootstrap, we use simple name prefix check */
            /* TODO: implement proper submodule checking */
            return false;
        }

        case VIS_PRIVATE: {
            /* Only visible in defining module */
            Scope *def_mod = scope_enclosing_module(sym->defining_scope);
            Scope *cur_mod = scope_enclosing_module(from_scope);
            return def_mod == cur_mod;
        }

        default:
            return false;
    }
}

/* Get the enclosing module scope */
Scope *scope_enclosing_module(Scope *scope) {
    while (scope) {
        if (scope->kind == SCOPE_MODULE) return scope;
        scope = scope->parent;
    }
    return NULL;
}

/* Get the enclosing type scope (for Self resolution) */
Scope *scope_enclosing_type(Scope *scope) {
    while (scope) {
        if (scope->kind == SCOPE_TYPE) return scope;
        scope = scope->parent;
    }
    return NULL;
}

/* Get enclosing loop scope with optional label */
Scope *scope_enclosing_loop(Scope *scope, InternedString label) {
    while (scope) {
        if (scope->kind == SCOPE_LOOP) {
            if (label.data == NULL) {
                /* No label specified, return innermost loop */
                return scope;
            }
            if (scope->loop_label.data && scope->loop_label.data == label.data) {
                /* Label matches (interned string pointer comparison) */
                return scope;
            }
        }
        scope = scope->parent;
    }
    return NULL;
}

/* Helper to create and add a built-in type symbol */
static Symbol *add_builtin_type(ScopeContext *ctx, const char *name) {
    InternedString iname = string_pool_intern_len(ctx->strings, name, strlen(name));
    Symbol *sym = symbol_new(ctx->arena);
    sym->kind = SYM_TYPE;
    sym->name = iname;
    sym->vis = VIS_PUBLIC;
    sym->decl = NULL;  /* Built-in, no declaration */
    sym->defining_scope = ctx->universe;
    sym->span = (SourceSpan){0};
    map_set(&ctx->universe->symbols, iname, sym);
    return sym;
}

/* Populate universe scope with built-in types */
void scope_populate_universe(ScopeContext *ctx) {
    /* Integer types */
    add_builtin_type(ctx, "i8");
    add_builtin_type(ctx, "i16");
    add_builtin_type(ctx, "i32");
    add_builtin_type(ctx, "i64");
    add_builtin_type(ctx, "i128");
    add_builtin_type(ctx, "isize");

    add_builtin_type(ctx, "u8");
    add_builtin_type(ctx, "u16");
    add_builtin_type(ctx, "u32");
    add_builtin_type(ctx, "u64");
    add_builtin_type(ctx, "u128");
    add_builtin_type(ctx, "usize");

    /* Float types */
    add_builtin_type(ctx, "f16");
    add_builtin_type(ctx, "f32");
    add_builtin_type(ctx, "f64");

    /* Other primitives */
    add_builtin_type(ctx, "bool");
    add_builtin_type(ctx, "char");
    add_builtin_type(ctx, "string");

    /* Built-in modal types */
    add_builtin_type(ctx, "Ptr");

    /* Built-in classes (interfaces) */
    Symbol *copy = add_builtin_type(ctx, "Copy");
    copy->kind = SYM_CLASS;

    Symbol *clone = add_builtin_type(ctx, "Clone");
    clone->kind = SYM_CLASS;

    Symbol *drop = add_builtin_type(ctx, "Drop");
    drop->kind = SYM_CLASS;

    Symbol *eq = add_builtin_type(ctx, "Eq");
    eq->kind = SYM_CLASS;

    Symbol *ord = add_builtin_type(ctx, "Ord");
    ord->kind = SYM_CLASS;

    Symbol *hash = add_builtin_type(ctx, "Hash");
    hash->kind = SYM_CLASS;

    Symbol *default_ = add_builtin_type(ctx, "Default");
    default_->kind = SYM_CLASS;

    /* Context type for capabilities */
    add_builtin_type(ctx, "Context");

    /* Capability types */
    add_builtin_type(ctx, "$FileSystem");
    add_builtin_type(ctx, "$Network");
    add_builtin_type(ctx, "$HeapAllocator");
    add_builtin_type(ctx, "System");
}
