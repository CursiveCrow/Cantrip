/*
 * Cursive Bootstrap Compiler - Parser Implementation
 *
 * Recursive descent parser with Pratt parsing for expressions.
 * Follows the Cursive Language Specification (Draft 3).
 *
 * Key features:
 * - Method dispatch uses ~> operator (NOT .method())
 * - Class implementations are inline via <:
 * - Binding operators: = (movable), := (immovable)
 * - 14 precedence levels per §12.3
 */

#include "parser.h"
#include "common/error.h"
#include <stdarg.h>

/*
 * ============================================
 * Parser Utilities
 * ============================================
 */

void parser_init(Parser *p, Lexer *lexer, Arena *arena, DiagContext *diag) {
    p->lexer = lexer;
    p->ast_arena = arena;
    p->diag = diag;
    p->has_peek = false;
    p->current = lexer_next(lexer);
}

static Token peek(Parser *p) {
    if (!p->has_peek) {
        p->peek = lexer_next(p->lexer);
        p->has_peek = true;
    }
    return p->peek;
}

static Token advance(Parser *p) {
    Token prev = p->current;
    if (p->has_peek) {
        p->current = p->peek;
        p->has_peek = false;
    } else {
        p->current = lexer_next(p->lexer);
    }
    return prev;
}

static bool check(Parser *p, TokenKind kind) {
    return p->current.kind == kind;
}

static bool check_any(Parser *p, int count, ...) {
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        TokenKind kind = va_arg(args, TokenKind);
        if (p->current.kind == kind) {
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}

static bool accept(Parser *p, TokenKind kind) {
    if (check(p, kind)) {
        advance(p);
        return true;
    }
    return false;
}

static Token expect(Parser *p, TokenKind kind, const char *msg) {
    if (check(p, kind)) {
        return advance(p);
    }
    diag_report(p->diag, DIAG_ERROR, E_SYN_0100,
               p->current.span, "Expected %s, found %s",
               msg, token_kind_name(kind));
    /* Return the current token anyway for error recovery */
    return p->current;
}

static void synchronize(Parser *p) {
    /* Skip tokens until we find a synchronization point */
    while (!check(p, TOK_EOF)) {
        if (check(p, TOK_SEMI)) {
            advance(p);
            return;
        }
        switch (p->current.kind) {
            case TOK_PROCEDURE:
            case TOK_RECORD:
            case TOK_ENUM:
            case TOK_MODAL:
            case TOK_CLASS:
            case TOK_LET:
            case TOK_VAR:
            case TOK_IF:
            case TOK_LOOP:
            case TOK_MATCH:
            case TOK_RETURN:
            case TOK_RESULT:
            case TOK_PUBLIC:
            case TOK_PRIVATE:
            case TOK_PROTECTED:
            case TOK_INTERNAL:
                return;
            default:
                advance(p);
        }
    }
}

/*
 * ============================================
 * Operator Precedence (per §12.3)
 * ============================================
 * Level 1 (highest): Postfix - (), [], ., ~>, as
 * Level 2: Unary - !, -, ~, *, &
 * Level 3: Exponent - **
 * Level 4: Multiplicative - *, /, %
 * Level 5: Additive - +, -
 * Level 6: Shift - <<, >>
 * Level 7: Bitwise AND - &
 * Level 8: Bitwise XOR - ^
 * Level 9: Bitwise OR - |
 * Level 10: Comparison - ==, !=, <, <=, >, >=
 * Level 11: Logical AND - &&
 * Level 12: Logical OR - ||
 * Level 13: Range - .., ..=
 * Level 14 (lowest): Assignment - =, +=, -=, etc.
 */

typedef enum Precedence {
    PREC_NONE = 0,
    PREC_ASSIGNMENT,  /* = += -= *= /= %= &= |= ^= <<= >>= */
    PREC_RANGE,       /* .. ..= */
    PREC_OR,          /* || */
    PREC_AND,         /* && */
    PREC_COMPARISON,  /* == != < <= > >= */
    PREC_BITOR,       /* | */
    PREC_BITXOR,      /* ^ */
    PREC_BITAND,      /* & */
    PREC_SHIFT,       /* << >> */
    PREC_ADDITIVE,    /* + - */
    PREC_MULTIPLICATIVE, /* * / % */
    PREC_EXPONENT,    /* ** */
    PREC_UNARY,       /* ! - ~ * & */
    PREC_POSTFIX      /* () [] . ~> as */
} Precedence;

static Precedence get_binary_precedence(TokenKind kind) {
    switch (kind) {
        case TOK_EQ:
        case TOK_PLUSEQ:
        case TOK_MINUSEQ:
        case TOK_STAREQ:
        case TOK_SLASHEQ:
        case TOK_PERCENTEQ:
        case TOK_AMPEQ:
        case TOK_PIPEEQ:
        case TOK_CARETEQ:
        case TOK_LTLTEQ:
        case TOK_GTGTEQ:
            return PREC_ASSIGNMENT;

        case TOK_DOTDOT:
        case TOK_DOTDOTEQ:
            return PREC_RANGE;

        case TOK_PIPEPIPE:
            return PREC_OR;

        case TOK_AMPAMP:
            return PREC_AND;

        case TOK_EQEQ:
        case TOK_NE:
        case TOK_LT:
        case TOK_LE:
        case TOK_GT:
        case TOK_GE:
            return PREC_COMPARISON;

        case TOK_PIPE:
            return PREC_BITOR;

        case TOK_CARET:
            return PREC_BITXOR;

        case TOK_AMP:
            return PREC_BITAND;

        case TOK_LTLT:
        case TOK_GTGT:
            return PREC_SHIFT;

        case TOK_PLUS:
        case TOK_MINUS:
            return PREC_ADDITIVE;

        case TOK_STAR:
        case TOK_SLASH:
        case TOK_PERCENT:
            return PREC_MULTIPLICATIVE;

        case TOK_STARSTAR:
            return PREC_EXPONENT;

        default:
            return PREC_NONE;
    }
}

static BinaryOp token_to_binop(TokenKind kind) {
    switch (kind) {
        case TOK_PLUS: return BINOP_ADD;
        case TOK_MINUS: return BINOP_SUB;
        case TOK_STAR: return BINOP_MUL;
        case TOK_SLASH: return BINOP_DIV;
        case TOK_PERCENT: return BINOP_MOD;
        case TOK_STARSTAR: return BINOP_POW;
        case TOK_EQEQ: return BINOP_EQ;
        case TOK_NE: return BINOP_NE;
        case TOK_LT: return BINOP_LT;
        case TOK_LE: return BINOP_LE;
        case TOK_GT: return BINOP_GT;
        case TOK_GE: return BINOP_GE;
        case TOK_AMPAMP: return BINOP_AND;
        case TOK_PIPEPIPE: return BINOP_OR;
        case TOK_AMP: return BINOP_BIT_AND;
        case TOK_PIPE: return BINOP_BIT_OR;
        case TOK_CARET: return BINOP_BIT_XOR;
        case TOK_LTLT: return BINOP_SHL;
        case TOK_GTGT: return BINOP_SHR;
        case TOK_EQ: return BINOP_ASSIGN;
        case TOK_PLUSEQ: return BINOP_ADD_ASSIGN;
        case TOK_MINUSEQ: return BINOP_SUB_ASSIGN;
        case TOK_STAREQ: return BINOP_MUL_ASSIGN;
        case TOK_SLASHEQ: return BINOP_DIV_ASSIGN;
        case TOK_PERCENTEQ: return BINOP_MOD_ASSIGN;
        case TOK_AMPEQ: return BINOP_BIT_AND_ASSIGN;
        case TOK_CARETEQ: return BINOP_BIT_XOR_ASSIGN;
        case TOK_LTLTEQ: return BINOP_SHL_ASSIGN;
        case TOK_GTGTEQ: return BINOP_SHR_ASSIGN;
        default: CURSIVE_UNREACHABLE();
    }
}

/*
 * ============================================
 * Forward Declarations
 * ============================================
 */

static Expr *parse_expr_prec(Parser *p, Precedence min_prec);
static Expr *parse_primary(Parser *p);
static TypeExpr *parse_type_internal(Parser *p);
static Pattern *parse_pattern_internal(Parser *p);
static Stmt *parse_stmt(Parser *p);
static Decl *parse_decl_internal(Parser *p);

/*
 * ============================================
 * Type Parsing
 * ============================================
 */

/* Parse primitive type from identifier */
static PrimitiveType parse_primitive_type(InternedString name) {
    const char *s = name.data;
    if (strcmp(s, "i8") == 0) return PRIM_I8;
    if (strcmp(s, "i16") == 0) return PRIM_I16;
    if (strcmp(s, "i32") == 0) return PRIM_I32;
    if (strcmp(s, "i64") == 0) return PRIM_I64;
    if (strcmp(s, "i128") == 0) return PRIM_I128;
    if (strcmp(s, "isize") == 0) return PRIM_ISIZE;
    if (strcmp(s, "u8") == 0) return PRIM_U8;
    if (strcmp(s, "u16") == 0) return PRIM_U16;
    if (strcmp(s, "u32") == 0) return PRIM_U32;
    if (strcmp(s, "u64") == 0) return PRIM_U64;
    if (strcmp(s, "u128") == 0) return PRIM_U128;
    if (strcmp(s, "usize") == 0) return PRIM_USIZE;
    if (strcmp(s, "f16") == 0) return PRIM_F16;
    if (strcmp(s, "f32") == 0) return PRIM_F32;
    if (strcmp(s, "f64") == 0) return PRIM_F64;
    if (strcmp(s, "bool") == 0) return PRIM_BOOL;
    if (strcmp(s, "char") == 0) return PRIM_CHAR;
    if (strcmp(s, "string") == 0) return PRIM_STRING;
    return (PrimitiveType)-1;  /* Not a primitive */
}

static TypeExpr *parse_type_internal(Parser *p) {
    SourceLoc start = p->current.span.start;

    /* Never type */
    if (accept(p, TOK_BANG)) {
        return ast_new_type(p->ast_arena, TEXPR_NEVER,
                           span_new(start, p->current.span.end));
    }

    /* Unit type or tuple type */
    if (accept(p, TOK_LPAREN)) {
        if (accept(p, TOK_RPAREN)) {
            /* Unit type () */
            return ast_new_type(p->ast_arena, TEXPR_UNIT,
                               span_new(start, p->current.span.end));
        }

        /* Tuple or parenthesized type */
        TypeExpr *first = parse_type_internal(p);

        if (accept(p, TOK_COMMA)) {
            /* Tuple type */
            TypeExpr *tuple = ast_new_type(p->ast_arena, TEXPR_TUPLE, span_point(start));
            tuple->tuple.elements = vec_new(TypeExpr *);
            vec_push(tuple->tuple.elements, first);

            do {
                if (check(p, TOK_RPAREN)) break;
                vec_push(tuple->tuple.elements, parse_type_internal(p));
            } while (accept(p, TOK_COMMA));

            expect(p, TOK_RPAREN, ")");
            tuple->span.end = p->current.span.end;
            return tuple;
        }

        expect(p, TOK_RPAREN, ")");
        return first;
    }

    /* Array or slice type */
    if (accept(p, TOK_LBRACKET)) {
        TypeExpr *element = parse_type_internal(p);

        if (accept(p, TOK_SEMI)) {
            /* Array: [T; N] */
            Expr *size = parse_expr_prec(p, PREC_NONE);
            expect(p, TOK_RBRACKET, "]");

            TypeExpr *arr = ast_new_type(p->ast_arena, TEXPR_ARRAY,
                                        span_new(start, p->current.span.end));
            arr->array.element = element;
            arr->array.size = size;
            return arr;
        }

        /* Slice: [T] */
        expect(p, TOK_RBRACKET, "]");
        TypeExpr *slice = ast_new_type(p->ast_arena, TEXPR_SLICE,
                                       span_new(start, p->current.span.end));
        slice->slice.element = element;
        return slice;
    }

    /* Reference type */
    if (accept(p, TOK_AMP)) {
        bool is_unique = accept(p, TOK_BANG);
        TypeExpr *referent = parse_type_internal(p);

        TypeExpr *ref = ast_new_type(p->ast_arena, TEXPR_REF,
                                     span_new(start, referent->span.end));
        ref->ref.referent = referent;
        ref->ref.is_unique = is_unique;
        return ref;
    }

    /* Self type */
    if (accept(p, TOK_SELF_TYPE)) {
        return ast_new_type(p->ast_arena, TEXPR_SELF,
                           span_new(start, p->current.span.end));
    }

    /* Named type with optional path and generics */
    if (check(p, TOK_IDENT)) {
        Token name_tok = advance(p);
        InternedString name = name_tok.value.ident;

        /* Check if it's a primitive */
        PrimitiveType prim = parse_primitive_type(name);
        if ((int)prim != -1) {
            TypeExpr *t = ast_new_type(p->ast_arena, TEXPR_PRIMITIVE,
                                       span_new(start, name_tok.span.end));
            t->primitive = prim;
            return t;
        }

        /* Build path if :: follows */
        Vec(InternedString) path = vec_new(InternedString);
        while (accept(p, TOK_COLONCOLON)) {
            vec_push(path, name);
            Token seg = expect(p, TOK_IDENT, "identifier");
            name = seg.value.ident;
        }

        TypeExpr *named = ast_new_type(p->ast_arena, TEXPR_NAMED, span_point(start));
        named->named.name = name;
        named->named.path = path;

        /* Check for generic arguments */
        if (accept(p, TOK_LT)) {
            Vec(TypeExpr *) args = vec_new(TypeExpr *);
            do {
                vec_push(args, parse_type_internal(p));
            } while (accept(p, TOK_COMMA));

            /* Handle >> as two > for nested generics */
            if (check(p, TOK_GTGT)) {
                /* Split >> into > > */
                /* For now, just expect > */
            }
            expect(p, TOK_GT, ">");

            TypeExpr *generic = ast_new_type(p->ast_arena, TEXPR_GENERIC,
                                             span_new(start, p->current.span.end));
            generic->generic.base = named;
            generic->generic.args = args;

            named = generic;
        }

        /* Check for modal state annotation: Type@State */
        if (accept(p, TOK_AT)) {
            Token state_tok = expect(p, TOK_IDENT, "state name");
            TypeExpr *modal = ast_new_type(p->ast_arena, TEXPR_MODAL_STATE,
                                           span_new(start, state_tok.span.end));
            modal->modal_state.base = named;
            modal->modal_state.state = state_tok.value.ident;
            return modal;
        }

        named->span.end = p->current.span.end;
        return named;
    }

    /* Procedure type */
    if (accept(p, TOK_PROCEDURE)) {
        expect(p, TOK_LPAREN, "(");

        Vec(TypeExpr *) params = vec_new(TypeExpr *);
        if (!check(p, TOK_RPAREN)) {
            do {
                vec_push(params, parse_type_internal(p));
            } while (accept(p, TOK_COMMA));
        }
        expect(p, TOK_RPAREN, ")");

        TypeExpr *ret = NULL;
        if (accept(p, TOK_ARROW)) {
            ret = parse_type_internal(p);
        }

        TypeExpr *fn = ast_new_type(p->ast_arena, TEXPR_FUNCTION,
                                    span_new(start, p->current.span.end));
        fn->function.params = params;
        fn->function.return_type = ret;
        return fn;
    }

    diag_report(p->diag, DIAG_ERROR, E_SYN_0102,
               p->current.span, "Expected type");
    return ast_new_type(p->ast_arena, TEXPR_INFER, p->current.span);
}

TypeExpr *parse_type(Parser *p) {
    TypeExpr *base = parse_type_internal(p);

    /* Check for union type: T | U | V */
    if (accept(p, TOK_PIPE)) {
        TypeExpr *union_type = ast_new_type(p->ast_arena, TEXPR_UNION, base->span);
        union_type->union_.members = vec_new(TypeExpr *);
        vec_push(union_type->union_.members, base);

        do {
            vec_push(union_type->union_.members, parse_type_internal(p));
        } while (accept(p, TOK_PIPE));

        union_type->span.end = p->current.span.end;
        return union_type;
    }

    return base;
}

/*
 * ============================================
 * Pattern Parsing
 * ============================================
 */

static Pattern *parse_pattern_internal(Parser *p) {
    SourceLoc start = p->current.span.start;

    /* Wildcard pattern */
    if (check(p, TOK_IDENT) && strcmp(p->current.value.ident.data, "_") == 0) {
        advance(p);
        return ast_new_pattern(p->ast_arena, PAT_WILDCARD,
                              span_new(start, p->current.span.end));
    }

    /* Modal state pattern: @State { ... } */
    if (accept(p, TOK_AT)) {
        Token state_tok = expect(p, TOK_IDENT, "state name");
        Pattern *modal = ast_new_pattern(p->ast_arena, PAT_MODAL, span_point(start));
        modal->modal.state = state_tok.value.ident;
        modal->modal.field_names = vec_new(InternedString);
        modal->modal.field_patterns = vec_new(Pattern *);

        if (accept(p, TOK_LBRACE)) {
            while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                Token field = expect(p, TOK_IDENT, "field name");
                vec_push(modal->modal.field_names, field.value.ident);

                if (accept(p, TOK_COLON)) {
                    vec_push(modal->modal.field_patterns, parse_pattern_internal(p));
                } else {
                    /* Shorthand: just the field name binds to itself */
                    Pattern *bind = ast_new_pattern(p->ast_arena, PAT_BINDING, field.span);
                    bind->binding.name = field.value.ident;
                    bind->binding.is_mutable = false;
                    bind->binding.type = NULL;
                    vec_push(modal->modal.field_patterns, bind);
                }

                if (!accept(p, TOK_COMMA)) break;
            }
            expect(p, TOK_RBRACE, "}");
        }

        modal->span.end = p->current.span.end;
        return modal;
    }

    /* Tuple pattern: (p1, p2, ...) */
    if (accept(p, TOK_LPAREN)) {
        if (accept(p, TOK_RPAREN)) {
            /* Unit pattern */
            Pattern *unit = ast_new_pattern(p->ast_arena, PAT_LITERAL, span_new(start, p->current.span.end));
            /* Create unit literal expression */
            return unit;
        }

        Pattern *first = parse_pattern_internal(p);

        if (accept(p, TOK_COMMA)) {
            Pattern *tuple = ast_new_pattern(p->ast_arena, PAT_TUPLE, span_point(start));
            tuple->tuple.elements = vec_new(Pattern *);
            vec_push(tuple->tuple.elements, first);

            do {
                if (check(p, TOK_RPAREN)) break;
                vec_push(tuple->tuple.elements, parse_pattern_internal(p));
            } while (accept(p, TOK_COMMA));

            expect(p, TOK_RPAREN, ")");
            tuple->span.end = p->current.span.end;
            return tuple;
        }

        expect(p, TOK_RPAREN, ")");
        return first;
    }

    /* Literal patterns */
    if (check(p, TOK_INT_LIT) || check(p, TOK_FLOAT_LIT) ||
        check(p, TOK_STRING_LIT) || check(p, TOK_CHAR_LIT) ||
        check(p, TOK_TRUE) || check(p, TOK_FALSE)) {
        Expr *lit = parse_primary(p);
        Pattern *pat = ast_new_pattern(p->ast_arena, PAT_LITERAL, lit->span);
        pat->literal.value = lit;
        return pat;
    }

    /* Binding pattern or record/enum pattern */
    if (check(p, TOK_IDENT)) {
        Token name_tok = advance(p);
        InternedString name = name_tok.value.ident;

        /* Check for path (enum variant) */
        if (accept(p, TOK_COLONCOLON)) {
            /* Enum variant: EnumType::Variant(payload) */
            TypeExpr *enum_type = ast_new_type(p->ast_arena, TEXPR_NAMED, name_tok.span);
            enum_type->named.name = name;
            enum_type->named.path = vec_new(InternedString);

            /* Read the identifier after the first :: */
            Token seg = expect(p, TOK_IDENT, "variant name");
            InternedString variant_name = seg.value.ident;

            /* Handle longer paths like A::B::C */
            while (accept(p, TOK_COLONCOLON)) {
                vec_push(enum_type->named.path, name);
                name = variant_name;
                seg = expect(p, TOK_IDENT, "identifier");
                variant_name = seg.value.ident;
            }

            Pattern *pat = ast_new_pattern(p->ast_arena, PAT_ENUM, span_point(start));
            pat->enum_.type = enum_type;
            pat->enum_.variant = variant_name;
            pat->enum_.payload = NULL;

            if (accept(p, TOK_LPAREN)) {
                pat->enum_.payload = parse_pattern_internal(p);
                expect(p, TOK_RPAREN, ")");
            }

            pat->span.end = p->current.span.end;
            return pat;
        }

        /* Check for record pattern: RecordName { ... } */
        if (accept(p, TOK_LBRACE)) {
            TypeExpr *rec_type = ast_new_type(p->ast_arena, TEXPR_NAMED, name_tok.span);
            rec_type->named.name = name;
            rec_type->named.path = vec_new(InternedString);

            Pattern *pat = ast_new_pattern(p->ast_arena, PAT_RECORD, span_point(start));
            pat->record.type = rec_type;
            pat->record.field_names = vec_new(InternedString);
            pat->record.field_patterns = vec_new(Pattern *);
            pat->record.has_rest = false;

            while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                if (accept(p, TOK_DOTDOT)) {
                    pat->record.has_rest = true;
                    break;
                }

                Token field = expect(p, TOK_IDENT, "field name");
                vec_push(pat->record.field_names, field.value.ident);

                if (accept(p, TOK_COLON)) {
                    vec_push(pat->record.field_patterns, parse_pattern_internal(p));
                } else {
                    Pattern *bind = ast_new_pattern(p->ast_arena, PAT_BINDING, field.span);
                    bind->binding.name = field.value.ident;
                    bind->binding.is_mutable = false;
                    bind->binding.type = NULL;
                    vec_push(pat->record.field_patterns, bind);
                }

                if (!accept(p, TOK_COMMA)) break;
            }
            expect(p, TOK_RBRACE, "}");
            pat->span.end = p->current.span.end;
            return pat;
        }

        /* Simple binding pattern */
        Pattern *bind = ast_new_pattern(p->ast_arena, PAT_BINDING, name_tok.span);
        bind->binding.name = name;
        bind->binding.is_mutable = false;
        bind->binding.type = NULL;

        /* Optional type annotation */
        if (accept(p, TOK_COLON)) {
            bind->binding.type = parse_type(p);
            bind->span.end = bind->binding.type->span.end;
        }

        return bind;
    }

    diag_report(p->diag, DIAG_ERROR, E_SYN_0103,
               p->current.span, "Expected pattern");
    return ast_new_pattern(p->ast_arena, PAT_WILDCARD, p->current.span);
}

Pattern *parse_pattern(Parser *p) {
    Pattern *pat = parse_pattern_internal(p);

    /* Check for OR pattern: pat1 | pat2 */
    if (accept(p, TOK_PIPE)) {
        Pattern *or_pat = ast_new_pattern(p->ast_arena, PAT_OR, pat->span);
        or_pat->or_.alternatives = vec_new(Pattern *);
        vec_push(or_pat->or_.alternatives, pat);

        do {
            vec_push(or_pat->or_.alternatives, parse_pattern_internal(p));
        } while (accept(p, TOK_PIPE));

        or_pat->span.end = p->current.span.end;
        pat = or_pat;
    }

    /* Check for guard: pat if condition */
    if (accept(p, TOK_IF)) {
        Expr *guard_expr = parse_expr_prec(p, PREC_NONE);
        Pattern *guard = ast_new_pattern(p->ast_arena, PAT_GUARD, pat->span);
        guard->guard.pattern = pat;
        guard->guard.guard = guard_expr;
        guard->span.end = guard_expr->span.end;
        return guard;
    }

    return pat;
}

/*
 * ============================================
 * Expression Parsing
 * ============================================
 */

static Expr *parse_primary(Parser *p) {
    SourceLoc start = p->current.span.start;

    /* Literals */
    if (check(p, TOK_INT_LIT)) {
        Token tok = advance(p);
        Expr *e = ast_new_expr(p->ast_arena, EXPR_INT_LIT, tok.span);
        e->int_lit.value = tok.value.int_val;
        e->int_lit.suffix = tok.int_suffix;
        return e;
    }

    if (check(p, TOK_FLOAT_LIT)) {
        Token tok = advance(p);
        Expr *e = ast_new_expr(p->ast_arena, EXPR_FLOAT_LIT, tok.span);
        e->float_lit.value = tok.value.float_val;
        return e;
    }

    if (check(p, TOK_STRING_LIT)) {
        Token tok = advance(p);
        Expr *e = ast_new_expr(p->ast_arena, EXPR_STRING_LIT, tok.span);
        e->string_lit.value = tok.value.ident;
        return e;
    }

    if (check(p, TOK_CHAR_LIT)) {
        Token tok = advance(p);
        Expr *e = ast_new_expr(p->ast_arena, EXPR_CHAR_LIT, tok.span);
        e->char_lit.value = tok.value.char_val;
        return e;
    }

    if (accept(p, TOK_TRUE)) {
        Expr *e = ast_new_expr(p->ast_arena, EXPR_BOOL_LIT,
                               span_new(start, p->current.span.end));
        e->bool_lit.value = true;
        return e;
    }

    if (accept(p, TOK_FALSE)) {
        Expr *e = ast_new_expr(p->ast_arena, EXPR_BOOL_LIT,
                               span_new(start, p->current.span.end));
        e->bool_lit.value = false;
        return e;
    }

    /* Parenthesized expression or tuple */
    if (accept(p, TOK_LPAREN)) {
        if (accept(p, TOK_RPAREN)) {
            /* Unit literal */
            Expr *e = ast_new_expr(p->ast_arena, EXPR_TUPLE,
                                   span_new(start, p->current.span.end));
            e->tuple.elements = vec_new(Expr *);
            return e;
        }

        Expr *first = parse_expr_prec(p, PREC_NONE);

        if (accept(p, TOK_COMMA)) {
            /* Tuple literal */
            Expr *tuple = ast_new_expr(p->ast_arena, EXPR_TUPLE, span_point(start));
            tuple->tuple.elements = vec_new(Expr *);
            vec_push(tuple->tuple.elements, first);

            do {
                if (check(p, TOK_RPAREN)) break;
                vec_push(tuple->tuple.elements, parse_expr_prec(p, PREC_NONE));
            } while (accept(p, TOK_COMMA));

            expect(p, TOK_RPAREN, ")");
            tuple->span.end = p->current.span.end;
            return tuple;
        }

        expect(p, TOK_RPAREN, ")");
        return first;
    }

    /* Array literal */
    if (accept(p, TOK_LBRACKET)) {
        Expr *arr = ast_new_expr(p->ast_arena, EXPR_ARRAY, span_point(start));
        arr->array.elements = vec_new(Expr *);
        arr->array.repeat_value = NULL;
        arr->array.repeat_count = NULL;

        if (!check(p, TOK_RBRACKET)) {
            Expr *first = parse_expr_prec(p, PREC_NONE);

            if (accept(p, TOK_SEMI)) {
                /* Repeat syntax: [value; count] */
                arr->array.repeat_value = first;
                arr->array.repeat_count = parse_expr_prec(p, PREC_NONE);
            } else {
                vec_push(arr->array.elements, first);
                while (accept(p, TOK_COMMA)) {
                    if (check(p, TOK_RBRACKET)) break;
                    vec_push(arr->array.elements, parse_expr_prec(p, PREC_NONE));
                }
            }
        }

        expect(p, TOK_RBRACKET, "]");
        arr->span.end = p->current.span.end;
        return arr;
    }

    /* Block expression */
    if (accept(p, TOK_LBRACE)) {
        Expr *block = ast_new_expr(p->ast_arena, EXPR_BLOCK, span_point(start));
        block->block.stmts = vec_new(Stmt *);
        block->block.result = NULL;

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            Stmt *stmt = parse_stmt(p);
            vec_push(block->block.stmts, stmt);

            /* Check for final expression (no semicolon) */
            if (check(p, TOK_RBRACE) && stmt->kind == STMT_EXPR) {
                block->block.result = stmt->expr.expr;
                /* Remove from stmts since it's the result */
                VEC_HEADER(block->block.stmts)->len--;
                break;
            }
        }

        expect(p, TOK_RBRACE, "}");
        block->span.end = p->current.span.end;
        return block;
    }

    /* If expression */
    if (accept(p, TOK_IF)) {
        Expr *if_expr = ast_new_expr(p->ast_arena, EXPR_IF, span_point(start));
        if_expr->if_.condition = parse_expr_prec(p, PREC_NONE);

        expect(p, TOK_LBRACE, "{");
        /* Parse then block inline */
        Expr *then_block = ast_new_expr(p->ast_arena, EXPR_BLOCK, p->current.span);
        then_block->block.stmts = vec_new(Stmt *);
        then_block->block.result = NULL;

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            Stmt *stmt = parse_stmt(p);
            vec_push(then_block->block.stmts, stmt);
        }
        expect(p, TOK_RBRACE, "}");
        then_block->span.end = p->current.span.end;
        if_expr->if_.then_branch = then_block;

        if_expr->if_.else_branch = NULL;
        if (accept(p, TOK_ELSE)) {
            if (check(p, TOK_IF)) {
                /* else if */
                if_expr->if_.else_branch = parse_primary(p);
            } else {
                expect(p, TOK_LBRACE, "{");
                Expr *else_block = ast_new_expr(p->ast_arena, EXPR_BLOCK, p->current.span);
                else_block->block.stmts = vec_new(Stmt *);
                else_block->block.result = NULL;

                while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                    Stmt *stmt = parse_stmt(p);
                    vec_push(else_block->block.stmts, stmt);
                }
                expect(p, TOK_RBRACE, "}");
                else_block->span.end = p->current.span.end;
                if_expr->if_.else_branch = else_block;
            }
        }

        if_expr->span.end = p->current.span.end;
        return if_expr;
    }

    /* Match expression */
    if (accept(p, TOK_MATCH)) {
        Expr *match_expr = ast_new_expr(p->ast_arena, EXPR_MATCH, span_point(start));
        match_expr->match.scrutinee = parse_expr_prec(p, PREC_NONE);
        match_expr->match.arms_patterns = vec_new(Pattern *);
        match_expr->match.arms_bodies = vec_new(Expr *);

        expect(p, TOK_LBRACE, "{");

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            Pattern *pat = parse_pattern(p);
            vec_push(match_expr->match.arms_patterns, pat);

            expect(p, TOK_FATARROW, "=>");
            Expr *body = parse_expr_prec(p, PREC_NONE);
            vec_push(match_expr->match.arms_bodies, body);

            if (!accept(p, TOK_COMMA)) break;
        }

        expect(p, TOK_RBRACE, "}");
        match_expr->span.end = p->current.span.end;
        return match_expr;
    }

    /* Loop expression */
    if (accept(p, TOK_LOOP)) {
        Expr *loop = ast_new_expr(p->ast_arena, EXPR_LOOP, span_point(start));
        loop->loop.label = interned_null();
        loop->loop.binding = NULL;
        loop->loop.iterable = NULL;
        loop->loop.condition = NULL;

        /* Check for iterator: loop x in range */
        if (check(p, TOK_IDENT) && peek(p).kind == TOK_IN) {
            loop->loop.binding = parse_pattern(p);
            expect(p, TOK_IN, "in");
            loop->loop.iterable = parse_expr_prec(p, PREC_NONE);
        } else if (!check(p, TOK_LBRACE)) {
            /* Condition loop: loop condition { } */
            loop->loop.condition = parse_expr_prec(p, PREC_NONE);
        }

        expect(p, TOK_LBRACE, "{");
        Expr *body = ast_new_expr(p->ast_arena, EXPR_BLOCK, p->current.span);
        body->block.stmts = vec_new(Stmt *);
        body->block.result = NULL;

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            vec_push(body->block.stmts, parse_stmt(p));
        }
        expect(p, TOK_RBRACE, "}");
        body->span.end = p->current.span.end;

        loop->loop.body = body;
        loop->span.end = p->current.span.end;
        return loop;
    }

    /* Move expression */
    if (accept(p, TOK_MOVE)) {
        Expr *move_expr = ast_new_expr(p->ast_arena, EXPR_MOVE, span_point(start));
        move_expr->move.operand = parse_expr_prec(p, PREC_UNARY);
        move_expr->span.end = move_expr->move.operand->span.end;
        return move_expr;
    }

    /* Widen expression */
    if (accept(p, TOK_WIDEN)) {
        Expr *widen = ast_new_expr(p->ast_arena, EXPR_WIDEN, span_point(start));
        widen->widen.operand = parse_expr_prec(p, PREC_UNARY);
        widen->span.end = widen->widen.operand->span.end;
        return widen;
    }

    /* Identifier or path */
    if (check(p, TOK_IDENT) || check(p, TOK_SELF)) {
        Token name_tok = advance(p);
        InternedString name = check(p, TOK_SELF)
            ? string_pool_intern(p->lexer->strings, "self")
            : name_tok.value.ident;

        /* Check for path */
        if (accept(p, TOK_COLONCOLON)) {
            Expr *path = ast_new_expr(p->ast_arena, EXPR_PATH, span_point(start));
            path->path.segments = vec_new(InternedString);
            vec_push(path->path.segments, name);

            do {
                Token seg = expect(p, TOK_IDENT, "identifier");
                vec_push(path->path.segments, seg.value.ident);
            } while (accept(p, TOK_COLONCOLON));

            path->span.end = p->current.span.end;
            return path;
        }

        /* Check for record literal: Identifier { field: value, ... }
         * Use lookahead to distinguish from match/block: Identifier { pattern => ... }
         * A record literal requires { followed by IDENT then : or , or } */
        if (check(p, TOK_LBRACE)) {
            Token next = peek(p);
            bool is_record_lit = (next.kind == TOK_RBRACE) ||  /* Empty record: {} */
                                 (next.kind == TOK_IDENT);      /* Field: { field... */
            /* Further check: if IDENT, peek more to see if followed by : or , (record) vs => (match) */
            /* For now, use simple heuristic: lowercase = variable, uppercase = type */
            /* TODO: Better disambiguation with 2-token lookahead */
            if (is_record_lit && next.kind == TOK_IDENT) {
                /* Heuristic: type names start with uppercase */
                char first = name.data[0];
                is_record_lit = (first >= 'A' && first <= 'Z');
            }
            if (is_record_lit) {
                accept(p, TOK_LBRACE);
                Expr *rec = ast_new_expr(p->ast_arena, EXPR_RECORD, span_point(start));
                rec->record.type = ast_new_type(p->ast_arena, TEXPR_NAMED, name_tok.span);
                rec->record.type->named.name = name;
                rec->record.field_names = vec_new(InternedString);
                rec->record.field_values = vec_new(Expr *);

                while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                    Token field_name = expect(p, TOK_IDENT, "field name");
                    expect(p, TOK_COLON, ":");
                    Expr *field_value = parse_expr_prec(p, PREC_NONE);
                    vec_push(rec->record.field_names, field_name.value.ident);
                    vec_push(rec->record.field_values, field_value);
                    if (!accept(p, TOK_COMMA)) break;
                }
                expect(p, TOK_RBRACE, "}");
                rec->span.end = p->current.span.end;
                return rec;
            }
        }
        Expr *ident = ast_new_expr(p->ast_arena, EXPR_IDENT, name_tok.span);
        ident->ident.name = name;
        return ident;
    }

    /* Unary operators */
    if (check(p, TOK_MINUS) || check(p, TOK_BANG) || check(p, TOK_TILDE) ||
        check(p, TOK_STAR) || check(p, TOK_AMP)) {
        Token op_tok = advance(p);
        UnaryOp op;

        switch (op_tok.kind) {
            case TOK_MINUS: op = UNOP_NEG; break;
            case TOK_BANG: op = UNOP_NOT; break;
            case TOK_TILDE: op = UNOP_BIT_NOT; break;
            case TOK_STAR: op = UNOP_DEREF; break;
            case TOK_AMP:
                if (accept(p, TOK_BANG)) {
                    op = UNOP_ADDR_MUT;
                } else {
                    op = UNOP_ADDR;
                }
                break;
            default: CURSIVE_UNREACHABLE();
        }

        Expr *operand = parse_expr_prec(p, PREC_UNARY);
        Expr *unary = ast_new_expr(p->ast_arena, EXPR_UNARY,
                                   span_new(start, operand->span.end));
        unary->unary.op = op;
        unary->unary.operand = operand;
        return unary;
    }

    diag_report(p->diag, DIAG_ERROR, E_SYN_0101,
               p->current.span, "Expected expression");
    /* Advance to prevent infinite loop in error recovery */
    advance(p);
    return ast_new_expr(p->ast_arena, EXPR_BOOL_LIT, p->current.span);
}

static Expr *parse_postfix(Parser *p, Expr *left) {
    for (;;) {
        SourceLoc start = left->span.start;

        /* Function call: expr(args) */
        if (accept(p, TOK_LPAREN)) {
            Vec(Expr *) args = vec_new(Expr *);
            if (!check(p, TOK_RPAREN)) {
                do {
                    vec_push(args, parse_expr_prec(p, PREC_NONE));
                } while (accept(p, TOK_COMMA));
            }
            expect(p, TOK_RPAREN, ")");

            Expr *call = ast_new_expr(p->ast_arena, EXPR_CALL,
                                      span_new(start, p->current.span.end));
            call->call.callee = left;
            call->call.args = args;
            left = call;
            continue;
        }

        /* Index: expr[index] */
        if (accept(p, TOK_LBRACKET)) {
            Expr *index_expr = parse_expr_prec(p, PREC_NONE);
            expect(p, TOK_RBRACKET, "]");

            Expr *index = ast_new_expr(p->ast_arena, EXPR_INDEX,
                                       span_new(start, p->current.span.end));
            index->index.object = left;
            index->index.index = index_expr;
            left = index;
            continue;
        }

        /* Field access: expr.field */
        if (accept(p, TOK_DOT)) {
            Token field_tok = expect(p, TOK_IDENT, "field name");

            Expr *field = ast_new_expr(p->ast_arena, EXPR_FIELD,
                                       span_new(start, field_tok.span.end));
            field->field.object = left;
            field->field.field = field_tok.value.ident;
            left = field;
            continue;
        }

        /* Method call: expr~>method(args) - THE CORRECT CURSIVE SYNTAX */
        if (accept(p, TOK_TILDEGT)) {
            Token method_tok = expect(p, TOK_IDENT, "method name");

            Expr *method_call = ast_new_expr(p->ast_arena, EXPR_METHOD_CALL,
                                             span_new(start, p->current.span.end));
            method_call->method_call.receiver = left;
            method_call->method_call.method = method_tok.value.ident;
            method_call->method_call.args = vec_new(Expr *);
            method_call->method_call.type_args = vec_new(TypeExpr *);

            /* Optional turbofish: ~>method::<T>(...) */
            if (accept(p, TOK_COLONCOLON)) {
                expect(p, TOK_LT, "<");
                do {
                    vec_push(method_call->method_call.type_args, parse_type(p));
                } while (accept(p, TOK_COMMA));
                expect(p, TOK_GT, ">");
            }

            /* Arguments */
            expect(p, TOK_LPAREN, "(");
            if (!check(p, TOK_RPAREN)) {
                do {
                    vec_push(method_call->method_call.args, parse_expr_prec(p, PREC_NONE));
                } while (accept(p, TOK_COMMA));
            }
            expect(p, TOK_RPAREN, ")");

            method_call->span.end = p->current.span.end;
            left = method_call;
            continue;
        }

        /* Try operator: expr? */
        if (accept(p, TOK_QUESTION)) {
            Expr *try_expr = ast_new_expr(p->ast_arena, EXPR_UNARY,
                                          span_new(start, p->current.span.end));
            try_expr->unary.op = UNOP_TRY;
            try_expr->unary.operand = left;
            left = try_expr;
            continue;
        }

        /* Cast: expr as Type */
        if (accept(p, TOK_AS)) {
            TypeExpr *target = parse_type(p);
            Expr *cast = ast_new_expr(p->ast_arena, EXPR_CAST,
                                      span_new(start, target->span.end));
            cast->cast.operand = left;
            cast->cast.target_type = target;
            left = cast;
            continue;
        }

        break;
    }

    return left;
}

static Expr *parse_expr_prec(Parser *p, Precedence min_prec) {
    Expr *left = parse_primary(p);
    left = parse_postfix(p, left);

    for (;;) {
        Precedence prec = get_binary_precedence(p->current.kind);
        if (prec < min_prec || prec == PREC_NONE) {
            break;
        }

        Token op_tok = advance(p);

        /* Handle range operators specially */
        if (op_tok.kind == TOK_DOTDOT || op_tok.kind == TOK_DOTDOTEQ) {
            Expr *end = NULL;
            if (token_can_start_expr(p->current.kind)) {
                end = parse_expr_prec(p, prec + 1);
            }

            Expr *range = ast_new_expr(p->ast_arena, EXPR_RANGE,
                                       span_new(left->span.start, p->current.span.end));
            range->range.start = left;
            range->range.end = end;
            range->range.inclusive = (op_tok.kind == TOK_DOTDOTEQ);
            left = range;
            continue;
        }

        /* Right-associative for exponentiation */
        Precedence next_prec = (op_tok.kind == TOK_STARSTAR) ? prec : prec + 1;
        Expr *right = parse_expr_prec(p, next_prec);
        right = parse_postfix(p, right);

        BinaryOp op = token_to_binop(op_tok.kind);
        Expr *binary = ast_new_expr(p->ast_arena, EXPR_BINARY,
                                    span_new(left->span.start, right->span.end));
        binary->binary.op = op;
        binary->binary.left = left;
        binary->binary.right = right;
        left = binary;
    }

    return left;
}

Expr *parse_expr(Parser *p) {
    return parse_expr_prec(p, PREC_ASSIGNMENT);
}

/*
 * ============================================
 * Statement Parsing
 * ============================================
 */

static Stmt *parse_stmt(Parser *p) {
    SourceLoc start = p->current.span.start;

    /* Let binding */
    if (accept(p, TOK_LET)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_LET, span_point(start));
        stmt->let.pattern = parse_pattern(p);

        /* Optional type annotation */
        stmt->let.type = NULL;
        if (accept(p, TOK_COLON)) {
            stmt->let.type = parse_type(p);
        }

        /* Binding operator: = or := */
        if (accept(p, TOK_COLONEQ)) {
            stmt->let.op = BIND_IMMOVABLE;
        } else {
            expect(p, TOK_EQ, "= or :=");
            stmt->let.op = BIND_MOVABLE;
        }

        stmt->let.init = parse_expr(p);
        stmt->span.end = stmt->let.init->span.end;

        /* Consume semicolon if present */
        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Var binding */
    if (accept(p, TOK_VAR)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_VAR, span_point(start));
        stmt->var.pattern = parse_pattern(p);

        stmt->var.type = NULL;
        if (accept(p, TOK_COLON)) {
            stmt->var.type = parse_type(p);
        }

        if (accept(p, TOK_COLONEQ)) {
            stmt->var.op = BIND_IMMOVABLE;
        } else {
            expect(p, TOK_EQ, "= or :=");
            stmt->var.op = BIND_MOVABLE;
        }

        stmt->var.init = parse_expr(p);
        stmt->span.end = stmt->var.init->span.end;

        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Return statement */
    if (accept(p, TOK_RETURN)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_RETURN, span_point(start));
        if (!check(p, TOK_SEMI) && !check(p, TOK_RBRACE)) {
            stmt->return_.value = parse_expr(p);
            stmt->span.end = stmt->return_.value->span.end;
        } else {
            stmt->return_.value = NULL;
        }
        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Result statement */
    if (accept(p, TOK_RESULT)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_RESULT, span_point(start));
        stmt->result.value = parse_expr(p);
        stmt->span.end = stmt->result.value->span.end;
        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Break statement */
    if (accept(p, TOK_BREAK)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_BREAK, span_point(start));
        stmt->break_.label = interned_null();
        stmt->break_.value = NULL;

        /* Optional label: break 'label */
        /* TODO: label parsing */

        if (!check(p, TOK_SEMI) && !check(p, TOK_RBRACE)) {
            stmt->break_.value = parse_expr(p);
            stmt->span.end = stmt->break_.value->span.end;
        }
        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Continue statement */
    if (accept(p, TOK_CONTINUE)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_CONTINUE, span_point(start));
        stmt->continue_.label = interned_null();
        accept(p, TOK_SEMI);
        return stmt;
    }

    /* Defer statement */
    if (accept(p, TOK_DEFER)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_DEFER, span_point(start));
        expect(p, TOK_LBRACE, "{");
        stmt->defer.body = parse_primary(p);  /* Parse block */
        stmt->span.end = stmt->defer.body->span.end;
        return stmt;
    }

    /* Unsafe block */
    if (accept(p, TOK_UNSAFE)) {
        Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_UNSAFE, span_point(start));
        expect(p, TOK_LBRACE, "{");
        /* Back up and re-parse as block */
        stmt->unsafe.body = parse_primary(p);
        stmt->span.end = stmt->unsafe.body->span.end;
        return stmt;
    }

    /* Expression statement */
    Expr *expr = parse_expr(p);
    Stmt *stmt = ast_new_stmt(p->ast_arena, STMT_EXPR, expr->span);
    stmt->expr.expr = expr;

    accept(p, TOK_SEMI);
    return stmt;
}

/*
 * ============================================
 * Declaration Parsing
 * ============================================
 */

static Visibility parse_visibility(Parser *p) {
    if (accept(p, TOK_PUBLIC)) return VIS_PUBLIC;
    if (accept(p, TOK_PRIVATE)) return VIS_PRIVATE;
    if (accept(p, TOK_PROTECTED)) return VIS_PROTECTED;
    if (accept(p, TOK_INTERNAL)) return VIS_INTERNAL;
    return VIS_PRIVATE;  /* Default */
}

static Vec(GenericParam) parse_generic_params(Parser *p) {
    Vec(GenericParam) params = vec_new(GenericParam);

    if (!accept(p, TOK_LT)) {
        return params;
    }

    do {
        GenericParam param = {0};
        param.span = p->current.span;

        Token name_tok = expect(p, TOK_IDENT, "type parameter name");
        param.name = name_tok.value.ident;
        param.bounds = vec_new(TypeExpr *);
        param.default_type = NULL;

        /* Optional bounds: T: Bound1 + Bound2 */
        if (accept(p, TOK_COLON)) {
            do {
                vec_push(param.bounds, parse_type(p));
            } while (accept(p, TOK_PLUS));
        }

        /* Optional default: T = DefaultType */
        if (accept(p, TOK_EQ)) {
            param.default_type = parse_type(p);
        }

        vec_push(params, param);
    } while (accept(p, TOK_COMMA));

    expect(p, TOK_GT, ">");
    return params;
}

static Vec(ParamDecl) parse_params(Parser *p) {
    Vec(ParamDecl) params = vec_new(ParamDecl);

    expect(p, TOK_LPAREN, "(");

    while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
        ParamDecl param = {0};
        param.span = p->current.span;
        param.perm = PERM_CONST;
        param.is_move = false;

        /* Check for move modifier */
        if (accept(p, TOK_MOVE)) {
            param.is_move = true;
        }

        /* Check for permission modifier */
        if (accept(p, TOK_CONST)) {
            param.perm = PERM_CONST;
        } else if (accept(p, TOK_UNIQUE)) {
            param.perm = PERM_UNIQUE;
        } else if (accept(p, TOK_SHARED)) {
            param.perm = PERM_SHARED;
        }

        Token name_tok = expect(p, TOK_IDENT, "parameter name");
        param.name = name_tok.value.ident;

        expect(p, TOK_COLON, ":");
        param.type = parse_type(p);

        vec_push(params, param);

        if (!accept(p, TOK_COMMA)) break;
    }

    expect(p, TOK_RPAREN, ")");
    return params;
}

static ReceiverKind parse_receiver(Parser *p) {
    if (accept(p, TOK_TILDE)) {
        return RECV_CONST;
    }
    if (accept(p, TOK_TILDEEXCL)) {
        return RECV_UNIQUE;
    }
    if (accept(p, TOK_TILDEPCT)) {
        return RECV_SHARED;
    }
    return RECV_NONE;
}

static ProcDecl parse_proc_decl_internal(Parser *p, Visibility vis) {
    ProcDecl proc = {0};
    proc.vis = vis;
    proc.span = p->current.span;
    proc.receiver = RECV_NONE;

    expect(p, TOK_PROCEDURE, "procedure");

    Token name_tok = expect(p, TOK_IDENT, "procedure name");
    proc.name = name_tok.value.ident;

    proc.generics = parse_generic_params(p);

    /* Check for receiver in parameter list */
    expect(p, TOK_LPAREN, "(");

    /* Check for receiver shorthand: ~, ~!, ~% */
    proc.receiver = parse_receiver(p);
    proc.params = vec_new(ParamDecl);

    if (proc.receiver != RECV_NONE) {
        /* Consume comma after receiver if there are more params */
        if (!check(p, TOK_RPAREN)) {
            accept(p, TOK_COMMA);
        }
    }

    /* Parse remaining parameters */
    while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
        ParamDecl param = {0};
        param.span = p->current.span;
        param.perm = PERM_CONST;
        param.is_move = false;

        if (accept(p, TOK_MOVE)) {
            param.is_move = true;
        }

        if (accept(p, TOK_CONST)) {
            param.perm = PERM_CONST;
        } else if (accept(p, TOK_UNIQUE)) {
            param.perm = PERM_UNIQUE;
        } else if (accept(p, TOK_SHARED)) {
            param.perm = PERM_SHARED;
        }

        Token pname_tok = expect(p, TOK_IDENT, "parameter name");
        param.name = pname_tok.value.ident;

        expect(p, TOK_COLON, ":");
        param.type = parse_type(p);

        vec_push(proc.params, param);

        if (!accept(p, TOK_COMMA)) break;
    }
    expect(p, TOK_RPAREN, ")");

    /* Return type */
    proc.return_type = NULL;
    if (accept(p, TOK_ARROW)) {
        proc.return_type = parse_type(p);
    }

    /* Contracts */
    proc.contracts = vec_new(Contract);
    while (check(p, TOK_PIPEEQ) || check(p, TOK_FATARROW)) {
        Contract contract = {0};
        contract.span = p->current.span;
        contract.is_precondition = check(p, TOK_PIPEEQ);
        advance(p);
        contract.condition = parse_expr(p);
        vec_push(proc.contracts, contract);
    }

    /* Where clause */
    proc.where_clauses = vec_new(WhereClause);
    if (accept(p, TOK_WHERE)) {
        /* TODO: Parse where clauses */
    }

    /* Body or semicolon */
    proc.body = NULL;
    if (accept(p, TOK_LBRACE)) {
        /* Parse block body */
        Expr *body = ast_new_expr(p->ast_arena, EXPR_BLOCK, p->current.span);
        body->block.stmts = vec_new(Stmt *);
        body->block.result = NULL;

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            Stmt *stmt = parse_stmt(p);
            vec_push(body->block.stmts, stmt);

            /* Check for final expression (no semicolon) */
            if (check(p, TOK_RBRACE) && stmt->kind == STMT_EXPR) {
                body->block.result = stmt->expr.expr;
                VEC_HEADER(body->block.stmts)->len--;
                break;
            }
        }
        expect(p, TOK_RBRACE, "}");
        body->span.end = p->current.span.end;
        proc.body = body;
    } else {
        accept(p, TOK_SEMI);
    }

    proc.span.end = p->current.span.end;
    return proc;
}

static Decl *parse_decl_internal(Parser *p) {
    SourceLoc start = p->current.span.start;
    Visibility vis = parse_visibility(p);

    /* Procedure declaration */
    if (check(p, TOK_PROCEDURE)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_PROC, span_point(start));
        decl->proc = parse_proc_decl_internal(p, vis);
        decl->span.end = decl->proc.span.end;
        return decl;
    }

    /* Record declaration */
    if (accept(p, TOK_RECORD)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_RECORD, span_point(start));
        decl->record.vis = vis;

        Token name_tok = expect(p, TOK_IDENT, "record name");
        decl->record.name = name_tok.value.ident;

        decl->record.generics = parse_generic_params(p);
        decl->record.implements = vec_new(TypeExpr *);
        decl->record.fields = vec_new(FieldDecl);
        decl->record.methods = vec_new(ProcDecl);
        decl->record.where_clauses = vec_new(WhereClause);

        /* Class implementations: <: Class1 + Class2 */
        if (accept(p, TOK_LT)) {
            expect(p, TOK_COLON, ":");  /* <: */
            do {
                vec_push(decl->record.implements, parse_type(p));
            } while (accept(p, TOK_PLUS));
        }

        /* Body: fields and methods in single block */
        expect(p, TOK_LBRACE, "{");
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            Visibility member_vis = parse_visibility(p);

            if (check(p, TOK_PROCEDURE)) {
                ProcDecl method = parse_proc_decl_internal(p, member_vis);
                vec_push(decl->record.methods, method);
            } else {
                /* Field declaration */
                FieldDecl field = {0};
                field.vis = member_vis;
                field.span = p->current.span;

                Token field_name = expect(p, TOK_IDENT, "field name");
                field.name = field_name.value.ident;

                expect(p, TOK_COLON, ":");
                field.type = parse_type(p);

                if (accept(p, TOK_EQ)) {
                    field.default_value = parse_expr(p);
                } else {
                    field.default_value = NULL;
                }

                vec_push(decl->record.fields, field);

                accept(p, TOK_COMMA);
            }
        }
        expect(p, TOK_RBRACE, "}");

        decl->span.end = p->current.span.end;
        return decl;
    }

    /* Enum declaration */
    if (accept(p, TOK_ENUM)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_ENUM, span_point(start));
        decl->enum_.vis = vis;

        Token name_tok = expect(p, TOK_IDENT, "enum name");
        decl->enum_.name = name_tok.value.ident;

        decl->enum_.generics = parse_generic_params(p);
        decl->enum_.implements = vec_new(TypeExpr *);
        decl->enum_.variants = vec_new(EnumVariant);
        decl->enum_.methods = vec_new(ProcDecl);
        decl->enum_.where_clauses = vec_new(WhereClause);

        if (accept(p, TOK_LT)) {
            expect(p, TOK_COLON, ":");
            do {
                vec_push(decl->enum_.implements, parse_type(p));
            } while (accept(p, TOK_PLUS));
        }

        expect(p, TOK_LBRACE, "{");
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            if (check(p, TOK_PROCEDURE) || check(p, TOK_PUBLIC) ||
                check(p, TOK_PRIVATE) || check(p, TOK_PROTECTED)) {
                Visibility method_vis = parse_visibility(p);
                if (check(p, TOK_PROCEDURE)) {
                    ProcDecl method = parse_proc_decl_internal(p, method_vis);
                    vec_push(decl->enum_.methods, method);
                    continue;
                }
            }

            /* Variant */
            EnumVariant variant = {0};
            variant.span = p->current.span;

            Token var_name = expect(p, TOK_IDENT, "variant name");
            variant.name = var_name.value.ident;
            variant.payload = NULL;
            variant.discriminant = NULL;

            if (accept(p, TOK_LPAREN)) {
                variant.payload = parse_type(p);
                expect(p, TOK_RPAREN, ")");
            }

            if (accept(p, TOK_EQ)) {
                variant.discriminant = parse_expr(p);
            }

            vec_push(decl->enum_.variants, variant);
            accept(p, TOK_COMMA);
        }
        expect(p, TOK_RBRACE, "}");

        decl->span.end = p->current.span.end;
        return decl;
    }

    /* Modal declaration */
    if (accept(p, TOK_MODAL)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_MODAL, span_point(start));
        decl->modal.vis = vis;

        Token name_tok = expect(p, TOK_IDENT, "modal type name");
        decl->modal.name = name_tok.value.ident;

        decl->modal.generics = parse_generic_params(p);
        decl->modal.implements = vec_new(TypeExpr *);
        decl->modal.states = vec_new(ModalState);
        decl->modal.shared_methods = vec_new(ProcDecl);
        decl->modal.where_clauses = vec_new(WhereClause);

        if (accept(p, TOK_LT)) {
            expect(p, TOK_COLON, ":");
            do {
                vec_push(decl->modal.implements, parse_type(p));
            } while (accept(p, TOK_PLUS));
        }

        expect(p, TOK_LBRACE, "{");
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            /* State definition: @StateName { fields, methods, transitions } */
            if (accept(p, TOK_AT)) {
                ModalState state = {0};
                state.span = p->current.span;

                Token state_name = expect(p, TOK_IDENT, "state name");
                state.name = state_name.value.ident;
                state.fields = vec_new(FieldDecl);
                state.methods = vec_new(ProcDecl);
                state.transitions = vec_new(Transition);

                expect(p, TOK_LBRACE, "{");
                while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                    if (accept(p, TOK_TRANSITION)) {
                        /* Transition declaration */
                        Transition trans = {0};
                        trans.span = p->current.span;

                        Token trans_name = expect(p, TOK_IDENT, "transition name");
                        trans.name = trans_name.value.ident;

                        expect(p, TOK_LPAREN, "(");
                        trans.receiver = parse_receiver(p);
                        trans.params = vec_new(ParamDecl);

                        if (trans.receiver != RECV_NONE && !check(p, TOK_RPAREN)) {
                            accept(p, TOK_COMMA);
                        }

                        while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
                            ParamDecl param = {0};
                            param.span = p->current.span;
                            Token pname = expect(p, TOK_IDENT, "parameter name");
                            param.name = pname.value.ident;
                            expect(p, TOK_COLON, ":");
                            param.type = parse_type(p);
                            vec_push(trans.params, param);
                            if (!accept(p, TOK_COMMA)) break;
                        }
                        expect(p, TOK_RPAREN, ")");

                        expect(p, TOK_ARROW, "->");
                        expect(p, TOK_AT, "@");
                        Token target = expect(p, TOK_IDENT, "target state");
                        trans.target_state = target.value.ident;

                        if (accept(p, TOK_LBRACE)) {
                            /* Parse body as block */
                            Expr *body = ast_new_expr(p->ast_arena, EXPR_BLOCK, p->current.span);
                            body->block.stmts = vec_new(Stmt *);
                            body->block.result = NULL;
                            while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                                vec_push(body->block.stmts, parse_stmt(p));
                            }
                            expect(p, TOK_RBRACE, "}");
                            body->span.end = p->current.span.end;
                            trans.body = body;
                        }

                        vec_push(state.transitions, trans);
                    } else if (check(p, TOK_PROCEDURE)) {
                        ProcDecl method = parse_proc_decl_internal(p, VIS_PRIVATE);
                        vec_push(state.methods, method);
                    } else {
                        /* Field */
                        FieldDecl field = {0};
                        field.span = p->current.span;
                        Token fname = expect(p, TOK_IDENT, "field name");
                        field.name = fname.value.ident;
                        expect(p, TOK_COLON, ":");
                        field.type = parse_type(p);
                        vec_push(state.fields, field);
                        accept(p, TOK_COMMA);
                    }
                }
                expect(p, TOK_RBRACE, "}");
                vec_push(decl->modal.states, state);
            } else if (check(p, TOK_PROCEDURE)) {
                /* Shared method */
                ProcDecl method = parse_proc_decl_internal(p, VIS_PRIVATE);
                vec_push(decl->modal.shared_methods, method);
            } else {
                synchronize(p);
            }
        }
        expect(p, TOK_RBRACE, "}");

        decl->span.end = p->current.span.end;
        return decl;
    }

    /* Class declaration */
    if (accept(p, TOK_CLASS)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_CLASS, span_point(start));
        decl->class_.vis = vis;

        Token name_tok = expect(p, TOK_IDENT, "class name");
        decl->class_.name = name_tok.value.ident;

        decl->class_.generics = parse_generic_params(p);
        decl->class_.superclasses = vec_new(TypeExpr *);
        decl->class_.methods = vec_new(ProcDecl);
        decl->class_.default_methods = vec_new(ProcDecl);
        decl->class_.where_clauses = vec_new(WhereClause);

        /* Superclasses: class Foo: Bar + Baz */
        if (accept(p, TOK_COLON)) {
            do {
                vec_push(decl->class_.superclasses, parse_type(p));
            } while (accept(p, TOK_PLUS));
        }

        expect(p, TOK_LBRACE, "{");
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            ProcDecl method = parse_proc_decl_internal(p, VIS_PUBLIC);
            if (method.body != NULL) {
                vec_push(decl->class_.default_methods, method);
            } else {
                vec_push(decl->class_.methods, method);
            }
        }
        expect(p, TOK_RBRACE, "}");

        decl->span.end = p->current.span.end;
        return decl;
    }

    /* Type alias */
    if (accept(p, TOK_TYPE)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_TYPE_ALIAS, span_point(start));
        decl->type_alias.vis = vis;

        Token name_tok = expect(p, TOK_IDENT, "type name");
        decl->type_alias.name = name_tok.value.ident;

        decl->type_alias.generics = parse_generic_params(p);

        expect(p, TOK_EQ, "=");
        decl->type_alias.aliased = parse_type(p);

        decl->span.end = p->current.span.end;
        accept(p, TOK_SEMI);
        return decl;
    }

    /* Import */
    if (accept(p, TOK_IMPORT)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_IMPORT, span_point(start));
        decl->import.path = vec_new(InternedString);

        do {
            Token seg = expect(p, TOK_IDENT, "module name");
            vec_push(decl->import.path, seg.value.ident);
        } while (accept(p, TOK_COLONCOLON));

        decl->span.end = p->current.span.end;
        accept(p, TOK_SEMI);
        return decl;
    }

    /* Use */
    if (accept(p, TOK_USING)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_USE, span_point(start));
        decl->use.path = vec_new(InternedString);
        decl->use.items = vec_new(InternedString);
        decl->use.is_glob = false;
        decl->use.alias = interned_null();

        do {
            Token seg = expect(p, TOK_IDENT, "identifier");
            vec_push(decl->use.path, seg.value.ident);
        } while (accept(p, TOK_COLONCOLON) && !check(p, TOK_LBRACE) && !check(p, TOK_STAR));

        if (accept(p, TOK_STAR)) {
            decl->use.is_glob = true;
        } else if (accept(p, TOK_LBRACE)) {
            do {
                Token item = expect(p, TOK_IDENT, "identifier");
                vec_push(decl->use.items, item.value.ident);
            } while (accept(p, TOK_COMMA));
            expect(p, TOK_RBRACE, "}");
        }

        if (accept(p, TOK_AS)) {
            Token alias = expect(p, TOK_IDENT, "alias");
            decl->use.alias = alias.value.ident;
        }

        decl->span.end = p->current.span.end;
        accept(p, TOK_SEMI);
        return decl;
    }

    /* Extern block */
    if (accept(p, TOK_EXTERN)) {
        Decl *decl = ast_new_decl(p->ast_arena, DECL_EXTERN, span_point(start));

        /* ABI string */
        if (check(p, TOK_STRING_LIT)) {
            Token abi = advance(p);
            decl->extern_.abi = abi.value.ident;
        } else {
            decl->extern_.abi = string_pool_intern(p->lexer->strings, "C");
        }

        decl->extern_.funcs = vec_new(ExternFuncDecl);

        expect(p, TOK_LBRACE, "{");
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            ExternFuncDecl func = {0};
            func.span = p->current.span;

            expect(p, TOK_PROCEDURE, "procedure");
            Token name = expect(p, TOK_IDENT, "function name");
            func.name = name.value.ident;
            func.link_name = interned_null();

            func.params = parse_params(p);

            func.return_type = NULL;
            if (accept(p, TOK_ARROW)) {
                func.return_type = parse_type(p);
            }

            vec_push(decl->extern_.funcs, func);
            accept(p, TOK_SEMI);
        }
        expect(p, TOK_RBRACE, "}");

        decl->span.end = p->current.span.end;
        return decl;
    }

    diag_report(p->diag, DIAG_ERROR, E_SYN_0100,
               p->current.span, "Expected declaration");
    synchronize(p);
    return ast_new_decl(p->ast_arena, DECL_PROC, p->current.span);
}

Decl *parse_decl(Parser *p) {
    return parse_decl_internal(p);
}

/*
 * ============================================
 * Module Parsing
 * ============================================
 */

Module *parse_module(Parser *p) {
    Module *mod = ast_new_module(p->ast_arena);
    mod->span = p->current.span;

    while (!check(p, TOK_EOF)) {
        Decl *decl = parse_decl(p);
        vec_push(mod->decls, decl);
    }

    mod->span.end = p->current.span.end;
    return mod;
}
