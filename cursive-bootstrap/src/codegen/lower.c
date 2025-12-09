/*
 * Cursive Bootstrap Compiler - Type Lowering
 *
 * Converts Cursive semantic types to LLVM types.
 *
 * Type mapping:
 * - i8..i128 -> i8..i128
 * - u8..u128 -> i8..i128 (same representation, different operations)
 * - f16/f32/f64 -> half/float/double
 * - bool -> i1
 * - char -> i32
 * - () -> void
 * - ! -> void (with noreturn)
 * - record -> named struct
 * - enum -> tagged union
 * - tuple -> anonymous struct
 * - array -> [N x T]
 * - slice -> {ptr, len}
 * - Ptr<T>@Valid -> T*
 * - string@View -> {i8*, i64}
 * - string@Managed -> {i8*, i64, i64}
 */

#include "codegen.h"
#include <string.h>

#ifdef HAVE_LLVM

/*
 * Lower a primitive type to LLVM
 */
static LLVMTypeRef lower_primitive(CodegenContext *ctx, TypeKind kind) {
    switch (kind) {
        case TYPE_PRIM_I8:
        case TYPE_PRIM_U8:
            return LLVMInt8TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_I16:
        case TYPE_PRIM_U16:
            return LLVMInt16TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_I32:
        case TYPE_PRIM_U32:
            return LLVMInt32TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_I64:
        case TYPE_PRIM_U64:
            return LLVMInt64TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_I128:
        case TYPE_PRIM_U128:
            return LLVMInt128TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_ISIZE:
        case TYPE_PRIM_USIZE:
            return LLVMIntPtrTypeInContext(ctx->llvm_ctx, ctx->target_data);

        case TYPE_PRIM_F16:
            return LLVMHalfTypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_F32:
            return LLVMFloatTypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_F64:
            return LLVMDoubleTypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_BOOL:
            return LLVMInt1TypeInContext(ctx->llvm_ctx);

        case TYPE_PRIM_CHAR:
            return LLVMInt32TypeInContext(ctx->llvm_ctx);

        default:
            return LLVMVoidTypeInContext(ctx->llvm_ctx);
    }
}

/*
 * Lower a record type to LLVM struct
 */
static LLVMTypeRef lower_record(CodegenContext *ctx, Type *type) {
    Symbol *sym = type->nominal.sym;
    if (!sym || !sym->decl) {
        return LLVMVoidTypeInContext(ctx->llvm_ctx);
    }

    /* Check cache first */
    uintptr_t cached;
    if (map_get(&ctx->type_cache, (uintptr_t)type, &cached)) {
        return (LLVMTypeRef)cached;
    }

    RecordDecl *record = &sym->decl->record;
    size_t field_count = vec_len(record->fields);

    /* Create opaque struct first (for recursive types) */
    LLVMTypeRef struct_type = LLVMStructCreateNamed(ctx->llvm_ctx, sym->name.data);
    map_put(&ctx->type_cache, (uintptr_t)type, (uintptr_t)struct_type);

    /* Lower field types */
    LLVMTypeRef *field_types = arena_alloc(ctx->arena,
        field_count * sizeof(LLVMTypeRef), _Alignof(LLVMTypeRef));

    for (size_t i = 0; i < field_count; i++) {
        /* Placeholder - would need resolved types from sema */
        field_types[i] = LLVMInt32TypeInContext(ctx->llvm_ctx);
    }

    LLVMStructSetBody(struct_type, field_types, (unsigned)field_count, 0);
    return struct_type;
}

/*
 * Lower an enum type to LLVM tagged union
 */
static LLVMTypeRef lower_enum(CodegenContext *ctx, Type *type) {
    Symbol *sym = type->nominal.sym;
    if (!sym || !sym->decl) {
        return LLVMVoidTypeInContext(ctx->llvm_ctx);
    }

    /* Check cache first */
    uintptr_t cached;
    if (map_get(&ctx->type_cache, (uintptr_t)type, &cached)) {
        return (LLVMTypeRef)cached;
    }

    EnumDecl *enum_decl = &sym->decl->enum_;
    size_t variant_count = vec_len(enum_decl->variants);

    /* Determine tag size based on variant count */
    LLVMTypeRef tag_type;
    if (variant_count <= 256) {
        tag_type = LLVMInt8TypeInContext(ctx->llvm_ctx);
    } else if (variant_count <= 65536) {
        tag_type = LLVMInt16TypeInContext(ctx->llvm_ctx);
    } else {
        tag_type = LLVMInt32TypeInContext(ctx->llvm_ctx);
    }

    /* Find maximum payload size */
    size_t max_payload_size = 0;
    for (size_t i = 0; i < variant_count; i++) {
        EnumVariant *variant = &enum_decl->variants[i];
        if (variant->payload) {
            /* Placeholder size calculation */
            size_t payload_size = 8;
            if (payload_size > max_payload_size) {
                max_payload_size = payload_size;
            }
        }
    }

    /* Create struct: { tag, payload_bytes } */
    LLVMTypeRef struct_type = LLVMStructCreateNamed(ctx->llvm_ctx, sym->name.data);
    map_put(&ctx->type_cache, (uintptr_t)type, (uintptr_t)struct_type);

    if (max_payload_size > 0) {
        LLVMTypeRef payload_type = LLVMArrayType(
            LLVMInt8TypeInContext(ctx->llvm_ctx),
            (unsigned)max_payload_size);
        LLVMTypeRef fields[2] = { tag_type, payload_type };
        LLVMStructSetBody(struct_type, fields, 2, 0);
    } else {
        /* No payloads - just the tag */
        LLVMStructSetBody(struct_type, &tag_type, 1, 0);
    }

    return struct_type;
}

/*
 * Lower a tuple type
 */
static LLVMTypeRef lower_tuple(CodegenContext *ctx, Type *type) {
    size_t elem_count = vec_len(type->tuple.elements);

    LLVMTypeRef *elem_types = arena_alloc(ctx->arena,
        elem_count * sizeof(LLVMTypeRef), _Alignof(LLVMTypeRef));

    for (size_t i = 0; i < elem_count; i++) {
        elem_types[i] = lower_type(ctx, type->tuple.elements[i]);
    }

    return LLVMStructTypeInContext(ctx->llvm_ctx, elem_types,
        (unsigned)elem_count, 0);
}

/*
 * Lower an array type
 */
static LLVMTypeRef lower_array(CodegenContext *ctx, Type *type) {
    LLVMTypeRef elem_type = lower_type(ctx, type->array.element);
    return LLVMArrayType(elem_type, (unsigned)type->array.size);
}

/*
 * Lower a slice type -> { ptr, len }
 */
static LLVMTypeRef lower_slice(CodegenContext *ctx, Type *type) {
    LLVMTypeRef elem_type = lower_type(ctx, type->slice.element);
    LLVMTypeRef ptr_type = LLVMPointerType(elem_type, 0);
    LLVMTypeRef len_type = LLVMInt64TypeInContext(ctx->llvm_ctx);

    LLVMTypeRef fields[2] = { ptr_type, len_type };
    return LLVMStructTypeInContext(ctx->llvm_ctx, fields, 2, 0);
}

/*
 * Lower a function type
 */
static LLVMTypeRef lower_function(CodegenContext *ctx, Type *type) {
    size_t param_count = vec_len(type->function.params);

    LLVMTypeRef *param_types = arena_alloc(ctx->arena,
        param_count * sizeof(LLVMTypeRef), _Alignof(LLVMTypeRef));

    for (size_t i = 0; i < param_count; i++) {
        param_types[i] = lower_type(ctx, type->function.params[i]);
    }

    LLVMTypeRef ret_type = lower_type(ctx, type->function.return_type);

    return LLVMFunctionType(ret_type, param_types, (unsigned)param_count, 0);
}

/*
 * Lower a pointer type
 */
static LLVMTypeRef lower_pointer(CodegenContext *ctx, Type *type) {
    LLVMTypeRef pointee = lower_type(ctx, type->ptr.pointee);
    return LLVMPointerType(pointee, 0);
}

/*
 * Lower a union type -> tagged union with all possible types
 */
static LLVMTypeRef lower_union(CodegenContext *ctx, Type *type) {
    size_t member_count = vec_len(type->union_.members);

    /* Find the maximum member size */
    size_t max_size = 0;
    for (size_t i = 0; i < member_count; i++) {
        LLVMTypeRef member_type = lower_type(ctx, type->union_.members[i]);
        size_t size = LLVMABISizeOfType(ctx->target_data, member_type);
        if (size > max_size) {
            max_size = size;
        }
    }

    /* Determine tag size */
    LLVMTypeRef tag_type;
    if (member_count <= 256) {
        tag_type = LLVMInt8TypeInContext(ctx->llvm_ctx);
    } else {
        tag_type = LLVMInt16TypeInContext(ctx->llvm_ctx);
    }

    /* Create union representation: { tag, data } */
    LLVMTypeRef data_type = LLVMArrayType(
        LLVMInt8TypeInContext(ctx->llvm_ctx),
        (unsigned)max_size);

    LLVMTypeRef fields[2] = { tag_type, data_type };
    return LLVMStructTypeInContext(ctx->llvm_ctx, fields, 2, 0);
}

/*
 * Main type lowering function
 */
LLVMTypeRef lower_type(CodegenContext *ctx, Type *type) {
    if (!type) {
        return LLVMVoidTypeInContext(ctx->llvm_ctx);
    }

    /* Check cache */
    uintptr_t cached;
    if (map_get(&ctx->type_cache, (uintptr_t)type, &cached)) {
        return (LLVMTypeRef)cached;
    }

    LLVMTypeRef result;

    switch (type->kind) {
        /* Primitives */
        case TYPE_PRIM_I8:
        case TYPE_PRIM_I16:
        case TYPE_PRIM_I32:
        case TYPE_PRIM_I64:
        case TYPE_PRIM_I128:
        case TYPE_PRIM_U8:
        case TYPE_PRIM_U16:
        case TYPE_PRIM_U32:
        case TYPE_PRIM_U64:
        case TYPE_PRIM_U128:
        case TYPE_PRIM_ISIZE:
        case TYPE_PRIM_USIZE:
        case TYPE_PRIM_F16:
        case TYPE_PRIM_F32:
        case TYPE_PRIM_F64:
        case TYPE_PRIM_BOOL:
        case TYPE_PRIM_CHAR:
            result = lower_primitive(ctx, type->kind);
            break;

        case TYPE_UNIT:
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        case TYPE_NEVER:
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        case TYPE_STRING:
            /* string@View: { ptr, len } */
            {
                LLVMTypeRef ptr_type = LLVMPointerType(
                    LLVMInt8TypeInContext(ctx->llvm_ctx), 0);
                LLVMTypeRef len_type = LLVMInt64TypeInContext(ctx->llvm_ctx);
                LLVMTypeRef fields[2] = { ptr_type, len_type };
                result = LLVMStructTypeInContext(ctx->llvm_ctx, fields, 2, 0);
            }
            break;

        case TYPE_RECORD:
            result = lower_record(ctx, type);
            break;

        case TYPE_ENUM:
            result = lower_enum(ctx, type);
            break;

        case TYPE_MODAL:
        case TYPE_MODAL_STATE:
            /* Modal types lowered based on concrete state */
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        case TYPE_CLASS:
            /* Classes are constraints, not directly representable */
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        case TYPE_TUPLE:
            result = lower_tuple(ctx, type);
            break;

        case TYPE_ARRAY:
            result = lower_array(ctx, type);
            break;

        case TYPE_SLICE:
            result = lower_slice(ctx, type);
            break;

        case TYPE_UNION:
            result = lower_union(ctx, type);
            break;

        case TYPE_FUNCTION:
            result = lower_function(ctx, type);
            break;

        case TYPE_PTR:
        case TYPE_PTR_VALID:
        case TYPE_PTR_NULL:
            result = lower_pointer(ctx, type);
            break;

        case TYPE_GENERIC_PARAM:
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        case TYPE_GENERIC_INST:
            result = lower_type(ctx, type->generic_inst.base);
            break;

        case TYPE_ERROR:
        case TYPE_INFER:
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;

        default:
            result = LLVMVoidTypeInContext(ctx->llvm_ctx);
            break;
    }

    /* Cache the result */
    map_put(&ctx->type_cache, (uintptr_t)type, (uintptr_t)result);
    return result;
}

#endif /* HAVE_LLVM */
