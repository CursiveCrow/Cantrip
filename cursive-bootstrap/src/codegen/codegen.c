/*
 * Cursive Bootstrap Compiler - LLVM Code Generation
 *
 * Generates LLVM IR from typed AST.
 * Handles expressions, statements, procedures, and module structure.
 */

#include "codegen.h"
#include <string.h>
#include <stdio.h>

#ifdef HAVE_LLVM
#include <llvm-c/BitWriter.h>
#endif

/*
 * Initialize code generation context
 */
bool codegen_init(CodegenContext *ctx, Arena *arena, SemaContext *sema,
                  DiagContext *diag, const char *module_name) {
    memset(ctx, 0, sizeof(CodegenContext));
    ctx->arena = arena;
    ctx->sema = sema;
    ctx->strings = sema->strings;
    ctx->diag = diag;

    /* Initialize target info */
    target_init_host(&ctx->target);

    /* Initialize maps */
    map_init(&ctx->type_cache);
    map_init(&ctx->func_cache);
    map_init(&ctx->global_cache);
    map_init(&ctx->locals);

#ifdef HAVE_LLVM
    /* Initialize LLVM */
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    /* Create LLVM context and module */
    ctx->llvm_ctx = LLVMContextCreate();
    ctx->module = LLVMModuleCreateWithNameInContext(module_name, ctx->llvm_ctx);
    ctx->builder = LLVMCreateBuilderInContext(ctx->llvm_ctx);

    /* Set up target */
    char *error = NULL;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(ctx->target.triple, &target, &error) != 0) {
        diag_emit(diag, DIAG_ERROR, (SourceSpan){0},
            "Failed to get target: %s", error);
        LLVMDisposeMessage(error);
        return false;
    }

    ctx->target_machine = LLVMCreateTargetMachine(
        target,
        ctx->target.triple,
        "generic",  /* CPU */
        "",         /* Features */
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault,
        LLVMCodeModelDefault);

    ctx->target_data = LLVMCreateTargetDataLayout(ctx->target_machine);
    LLVMSetModuleDataLayout(ctx->module, ctx->target_data);
    LLVMSetTarget(ctx->module, ctx->target.triple);
#endif

    return true;
}

/*
 * Cleanup code generation context
 */
void codegen_destroy(CodegenContext *ctx) {
#ifdef HAVE_LLVM
    if (ctx->builder) {
        LLVMDisposeBuilder(ctx->builder);
    }
    if (ctx->target_data) {
        LLVMDisposeTargetData(ctx->target_data);
    }
    if (ctx->target_machine) {
        LLVMDisposeTargetMachine(ctx->target_machine);
    }
    if (ctx->module) {
        LLVMDisposeModule(ctx->module);
    }
    if (ctx->llvm_ctx) {
        LLVMContextDispose(ctx->llvm_ctx);
    }
#endif
    (void)ctx;
}

#ifdef HAVE_LLVM

/*
 * Forward declarations
 */
static LLVMValueRef codegen_expr_internal(CodegenContext *ctx, Expr *expr);
static void codegen_stmt_internal(CodegenContext *ctx, Stmt *stmt);

/*
 * Generate code for a literal expression
 */
static LLVMValueRef codegen_literal(CodegenContext *ctx, Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LIT:
            return LLVMConstInt(LLVMInt64TypeInContext(ctx->llvm_ctx),
                expr->int_lit.value, 0);

        case EXPR_FLOAT_LIT:
            return LLVMConstReal(LLVMDoubleTypeInContext(ctx->llvm_ctx),
                expr->float_lit.value);

        case EXPR_BOOL_LIT:
            return LLVMConstInt(LLVMInt1TypeInContext(ctx->llvm_ctx),
                expr->bool_lit.value ? 1 : 0, 0);

        case EXPR_CHAR_LIT:
            return LLVMConstInt(LLVMInt32TypeInContext(ctx->llvm_ctx),
                expr->char_lit.value, 0);

        case EXPR_STRING_LIT:
            return LLVMBuildGlobalStringPtr(ctx->builder,
                expr->string_lit.value.data, "str");

        default:
            return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }
}

/*
 * Generate code for an identifier
 */
static LLVMValueRef codegen_ident(CodegenContext *ctx, Expr *expr) {
    /* Look up in locals */
    uintptr_t val;
    InternedString name = expr->ident.name;

    /* Check local variables */
    Symbol *sym = scope_lookup_from(ctx->sema->current_scope, name);
    if (sym) {
        if (map_get(&ctx->locals, (uintptr_t)sym, &val)) {
            /* Load from alloca */
            return LLVMBuildLoad2(ctx->builder,
                LLVMGetAllocatedType((LLVMValueRef)val),
                (LLVMValueRef)val, name.data);
        }

        /* Check function cache */
        if (map_get(&ctx->func_cache, (uintptr_t)sym, &val)) {
            return (LLVMValueRef)val;
        }

        /* Check global cache */
        if (map_get(&ctx->global_cache, (uintptr_t)sym, &val)) {
            return (LLVMValueRef)val;
        }
    }

    /* Not found - return null */
    return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
}

/*
 * Generate code for a binary expression
 */
static LLVMValueRef codegen_binary(CodegenContext *ctx, Expr *expr) {
    LLVMValueRef left = codegen_expr_internal(ctx, expr->binary.left);
    LLVMValueRef right = codegen_expr_internal(ctx, expr->binary.right);

    if (!left || !right) {
        return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }

    /* Get type for signed/unsigned selection */
    LLVMTypeRef left_type = LLVMTypeOf(left);
    bool is_float = (LLVMGetTypeKind(left_type) == LLVMFloatTypeKind ||
                     LLVMGetTypeKind(left_type) == LLVMDoubleTypeKind ||
                     LLVMGetTypeKind(left_type) == LLVMHalfTypeKind);

    switch (expr->binary.op) {
        /* Arithmetic */
        case BINOP_ADD:
            if (is_float) return LLVMBuildFAdd(ctx->builder, left, right, "fadd");
            return LLVMBuildAdd(ctx->builder, left, right, "add");

        case BINOP_SUB:
            if (is_float) return LLVMBuildFSub(ctx->builder, left, right, "fsub");
            return LLVMBuildSub(ctx->builder, left, right, "sub");

        case BINOP_MUL:
            if (is_float) return LLVMBuildFMul(ctx->builder, left, right, "fmul");
            return LLVMBuildMul(ctx->builder, left, right, "mul");

        case BINOP_DIV:
            if (is_float) return LLVMBuildFDiv(ctx->builder, left, right, "fdiv");
            return LLVMBuildSDiv(ctx->builder, left, right, "sdiv");

        case BINOP_MOD:
            if (is_float) return LLVMBuildFRem(ctx->builder, left, right, "frem");
            return LLVMBuildSRem(ctx->builder, left, right, "srem");

        /* Comparison */
        case BINOP_EQ:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealOEQ, left, right, "feq");
            return LLVMBuildICmp(ctx->builder, LLVMIntEQ, left, right, "eq");

        case BINOP_NE:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealONE, left, right, "fne");
            return LLVMBuildICmp(ctx->builder, LLVMIntNE, left, right, "ne");

        case BINOP_LT:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealOLT, left, right, "flt");
            return LLVMBuildICmp(ctx->builder, LLVMIntSLT, left, right, "slt");

        case BINOP_LE:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealOLE, left, right, "fle");
            return LLVMBuildICmp(ctx->builder, LLVMIntSLE, left, right, "sle");

        case BINOP_GT:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealOGT, left, right, "fgt");
            return LLVMBuildICmp(ctx->builder, LLVMIntSGT, left, right, "sgt");

        case BINOP_GE:
            if (is_float) return LLVMBuildFCmp(ctx->builder, LLVMRealOGE, left, right, "fge");
            return LLVMBuildICmp(ctx->builder, LLVMIntSGE, left, right, "sge");

        /* Logical */
        case BINOP_AND:
            return LLVMBuildAnd(ctx->builder, left, right, "and");

        case BINOP_OR:
            return LLVMBuildOr(ctx->builder, left, right, "or");

        /* Bitwise */
        case BINOP_BIT_AND:
            return LLVMBuildAnd(ctx->builder, left, right, "band");

        case BINOP_BIT_OR:
            return LLVMBuildOr(ctx->builder, left, right, "bor");

        case BINOP_BIT_XOR:
            return LLVMBuildXor(ctx->builder, left, right, "bxor");

        case BINOP_SHL:
            return LLVMBuildShl(ctx->builder, left, right, "shl");

        case BINOP_SHR:
            return LLVMBuildAShr(ctx->builder, left, right, "shr");

        /* Power - use llvm.pow intrinsic for floats */
        case BINOP_POW:
            if (is_float) {
                LLVMTypeRef param_types[] = { left_type, left_type };
                LLVMTypeRef fn_type = LLVMFunctionType(left_type, param_types, 2, 0);
                LLVMValueRef pow_fn = LLVMGetIntrinsicDeclaration(ctx->module,
                    LLVMLookupIntrinsicID("llvm.pow", 8), &left_type, 1);
                LLVMValueRef args[] = { left, right };
                return LLVMBuildCall2(ctx->builder, fn_type, pow_fn, args, 2, "pow");
            }
            /* Integer power would need a loop - simplified here */
            return left;

        default:
            return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }
}

/*
 * Generate code for a unary expression
 */
static LLVMValueRef codegen_unary(CodegenContext *ctx, Expr *expr) {
    LLVMValueRef operand = codegen_expr_internal(ctx, expr->unary.operand);
    if (!operand) {
        return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }

    LLVMTypeRef type = LLVMTypeOf(operand);
    bool is_float = (LLVMGetTypeKind(type) == LLVMFloatTypeKind ||
                     LLVMGetTypeKind(type) == LLVMDoubleTypeKind);

    switch (expr->unary.op) {
        case UNOP_NEG:
            if (is_float) return LLVMBuildFNeg(ctx->builder, operand, "fneg");
            return LLVMBuildNeg(ctx->builder, operand, "neg");

        case UNOP_NOT:
            return LLVMBuildNot(ctx->builder, operand, "not");

        case UNOP_BIT_NOT:
            return LLVMBuildNot(ctx->builder, operand, "bitnot");

        case UNOP_DEREF:
            /* Dereference pointer */
            return LLVMBuildLoad2(ctx->builder,
                LLVMGetElementType(type), operand, "deref");

        case UNOP_ADDR:
        case UNOP_ADDR_MUT:
            /* Address-of - operand should already be a pointer (alloca) */
            return operand;

        default:
            return operand;
    }
}

/*
 * Generate code for a call expression
 */
static LLVMValueRef codegen_call(CodegenContext *ctx, Expr *expr) {
    LLVMValueRef callee = codegen_expr_internal(ctx, expr->call.callee);
    if (!callee) {
        return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }

    size_t arg_count = vec_len(expr->call.args);
    LLVMValueRef *args = arena_alloc(ctx->arena,
        arg_count * sizeof(LLVMValueRef), _Alignof(LLVMValueRef));

    for (size_t i = 0; i < arg_count; i++) {
        args[i] = codegen_expr_internal(ctx, expr->call.args[i]);
    }

    LLVMTypeRef fn_type = LLVMGlobalGetValueType(callee);
    return LLVMBuildCall2(ctx->builder, fn_type, callee, args,
        (unsigned)arg_count, "call");
}

/*
 * Generate code for an if expression
 */
static LLVMValueRef codegen_if(CodegenContext *ctx, Expr *expr) {
    LLVMValueRef cond = codegen_expr_internal(ctx, expr->if_.condition);
    if (!cond) {
        return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }

    /* Create basic blocks */
    LLVMBasicBlockRef then_bb = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "then");
    LLVMBasicBlockRef else_bb = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "else");
    LLVMBasicBlockRef merge_bb = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "ifcont");

    LLVMBuildCondBr(ctx->builder, cond, then_bb, else_bb);

    /* Then block */
    LLVMPositionBuilderAtEnd(ctx->builder, then_bb);
    LLVMValueRef then_val = codegen_expr_internal(ctx, expr->if_.then_branch);
    LLVMBuildBr(ctx->builder, merge_bb);
    then_bb = LLVMGetInsertBlock(ctx->builder);

    /* Else block */
    LLVMPositionBuilderAtEnd(ctx->builder, else_bb);
    LLVMValueRef else_val = NULL;
    if (expr->if_.else_branch) {
        else_val = codegen_expr_internal(ctx, expr->if_.else_branch);
    } else {
        else_val = LLVMConstNull(LLVMTypeOf(then_val));
    }
    LLVMBuildBr(ctx->builder, merge_bb);
    else_bb = LLVMGetInsertBlock(ctx->builder);

    /* Merge block */
    LLVMPositionBuilderAtEnd(ctx->builder, merge_bb);

    /* Create PHI node for result */
    if (then_val && else_val &&
        LLVMGetTypeKind(LLVMTypeOf(then_val)) != LLVMVoidTypeKind) {
        LLVMValueRef phi = LLVMBuildPhi(ctx->builder,
            LLVMTypeOf(then_val), "iftmp");
        LLVMValueRef incoming_vals[] = { then_val, else_val };
        LLVMBasicBlockRef incoming_blocks[] = { then_bb, else_bb };
        LLVMAddIncoming(phi, incoming_vals, incoming_blocks, 2);
        return phi;
    }

    return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
}

/*
 * Generate code for a block expression
 */
static LLVMValueRef codegen_block(CodegenContext *ctx, Expr *expr) {
    /* Generate statements */
    for (size_t i = 0; i < vec_len(expr->block.stmts); i++) {
        codegen_stmt_internal(ctx, expr->block.stmts[i]);
    }

    /* Generate result expression */
    if (expr->block.result) {
        return codegen_expr_internal(ctx, expr->block.result);
    }

    return LLVMConstNull(LLVMVoidTypeInContext(ctx->llvm_ctx));
}

/*
 * Generate code for a loop expression
 */
static LLVMValueRef codegen_loop(CodegenContext *ctx, Expr *expr) {
    /* Create blocks */
    LLVMBasicBlockRef loop_header = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "loop");
    LLVMBasicBlockRef loop_body = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "loopbody");
    LLVMBasicBlockRef loop_exit = LLVMAppendBasicBlockInContext(
        ctx->llvm_ctx, ctx->current_func, "loopexit");

    /* Save loop context */
    LLVMBasicBlockRef saved_break = ctx->loop_break_block;
    LLVMBasicBlockRef saved_continue = ctx->loop_continue_block;
    ctx->loop_break_block = loop_exit;
    ctx->loop_continue_block = loop_header;

    /* Jump to loop header */
    LLVMBuildBr(ctx->builder, loop_header);

    /* Loop header - check condition */
    LLVMPositionBuilderAtEnd(ctx->builder, loop_header);
    if (expr->loop.condition) {
        LLVMValueRef cond = codegen_expr_internal(ctx, expr->loop.condition);
        LLVMBuildCondBr(ctx->builder, cond, loop_body, loop_exit);
    } else {
        /* Infinite loop */
        LLVMBuildBr(ctx->builder, loop_body);
    }

    /* Loop body */
    LLVMPositionBuilderAtEnd(ctx->builder, loop_body);
    if (expr->loop.body) {
        codegen_expr_internal(ctx, expr->loop.body);
    }
    LLVMBuildBr(ctx->builder, loop_header);

    /* Loop exit */
    LLVMPositionBuilderAtEnd(ctx->builder, loop_exit);

    /* Restore loop context */
    ctx->loop_break_block = saved_break;
    ctx->loop_continue_block = saved_continue;

    return LLVMConstNull(LLVMVoidTypeInContext(ctx->llvm_ctx));
}

/*
 * Generate code for expression
 */
static LLVMValueRef codegen_expr_internal(CodegenContext *ctx, Expr *expr) {
    if (!expr) return NULL;

    switch (expr->kind) {
        case EXPR_INT_LIT:
        case EXPR_FLOAT_LIT:
        case EXPR_STRING_LIT:
        case EXPR_CHAR_LIT:
        case EXPR_BOOL_LIT:
            return codegen_literal(ctx, expr);

        case EXPR_IDENT:
            return codegen_ident(ctx, expr);

        case EXPR_BINARY:
            return codegen_binary(ctx, expr);

        case EXPR_UNARY:
            return codegen_unary(ctx, expr);

        case EXPR_CALL:
            return codegen_call(ctx, expr);

        case EXPR_IF:
            return codegen_if(ctx, expr);

        case EXPR_BLOCK:
            return codegen_block(ctx, expr);

        case EXPR_LOOP:
            return codegen_loop(ctx, expr);

        case EXPR_MOVE:
            /* Move just passes through the value */
            return codegen_expr_internal(ctx, expr->move.operand);

        case EXPR_WIDEN:
            return codegen_expr_internal(ctx, expr->widen.operand);

        default:
            return LLVMConstNull(LLVMInt32TypeInContext(ctx->llvm_ctx));
    }
}

/*
 * Generate code for statement
 */
static void codegen_stmt_internal(CodegenContext *ctx, Stmt *stmt) {
    if (!stmt) return;

    switch (stmt->kind) {
        case STMT_EXPR:
            codegen_expr_internal(ctx, stmt->expr.expr);
            break;

        case STMT_LET:
        case STMT_VAR: {
            /* Create alloca for binding */
            LLVMTypeRef var_type = LLVMInt32TypeInContext(ctx->llvm_ctx);
            LLVMValueRef alloca = LLVMBuildAlloca(ctx->builder, var_type, "var");

            /* Store initial value if present */
            Expr *init = (stmt->kind == STMT_LET) ? stmt->let.init : stmt->var.init;
            if (init) {
                LLVMValueRef val = codegen_expr_internal(ctx, init);
                if (val) {
                    LLVMBuildStore(ctx->builder, val, alloca);
                }
            }

            /* TODO: Register in locals map based on pattern symbol */
            break;
        }

        case STMT_ASSIGN: {
            LLVMValueRef val = codegen_expr_internal(ctx, stmt->assign.value);
            /* TODO: Get alloca for target and store */
            (void)val;
            break;
        }

        case STMT_RETURN:
            if (stmt->return_.value) {
                LLVMValueRef val = codegen_expr_internal(ctx, stmt->return_.value);
                LLVMBuildRet(ctx->builder, val);
            } else {
                LLVMBuildRetVoid(ctx->builder);
            }
            break;

        case STMT_RESULT:
            if (stmt->result.value) {
                LLVMValueRef val = codegen_expr_internal(ctx, stmt->result.value);
                LLVMBuildRet(ctx->builder, val);
            }
            break;

        case STMT_BREAK:
            if (ctx->loop_break_block) {
                LLVMBuildBr(ctx->builder, ctx->loop_break_block);
            }
            break;

        case STMT_CONTINUE:
            if (ctx->loop_continue_block) {
                LLVMBuildBr(ctx->builder, ctx->loop_continue_block);
            }
            break;

        case STMT_DEFER:
            /* Defer is handled at scope exit */
            break;

        case STMT_UNSAFE:
            if (stmt->unsafe.body) {
                codegen_expr_internal(ctx, stmt->unsafe.body);
            }
            break;
    }
}

/*
 * Generate code for procedure
 */
LLVMValueRef codegen_proc(CodegenContext *ctx, ProcDecl *proc, Symbol *sym) {
    /* Get or create function */
    uintptr_t cached;
    if (map_get(&ctx->func_cache, (uintptr_t)sym, &cached)) {
        return (LLVMValueRef)cached;
    }

    /* Build function type */
    size_t param_count = vec_len(proc->params);
    LLVMTypeRef *param_types = arena_alloc(ctx->arena,
        param_count * sizeof(LLVMTypeRef), _Alignof(LLVMTypeRef));

    for (size_t i = 0; i < param_count; i++) {
        param_types[i] = LLVMInt32TypeInContext(ctx->llvm_ctx);  /* Placeholder */
    }

    LLVMTypeRef ret_type = LLVMInt32TypeInContext(ctx->llvm_ctx);  /* Placeholder */
    LLVMTypeRef fn_type = LLVMFunctionType(ret_type, param_types,
        (unsigned)param_count, 0);

    /* Create function */
    LLVMValueRef fn = LLVMAddFunction(ctx->module, proc->name.data, fn_type);
    map_put(&ctx->func_cache, (uintptr_t)sym, (uintptr_t)fn);

    /* Generate body if present */
    if (proc->body) {
        ctx->current_func = fn;

        /* Create entry block */
        LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
            ctx->llvm_ctx, fn, "entry");
        LLVMPositionBuilderAtEnd(ctx->builder, entry);

        /* Create allocas for parameters */
        for (size_t i = 0; i < param_count; i++) {
            ParamDecl *param = &proc->params[i];
            LLVMValueRef alloca = LLVMBuildAlloca(ctx->builder,
                param_types[i], param->name.data);
            LLVMBuildStore(ctx->builder, LLVMGetParam(fn, (unsigned)i), alloca);

            /* Register in locals */
            Symbol *param_sym = scope_lookup_from(ctx->sema->current_scope,
                param->name);
            if (param_sym) {
                map_put(&ctx->locals, (uintptr_t)param_sym, (uintptr_t)alloca);
            }
        }

        /* Generate body */
        LLVMValueRef result = codegen_expr_internal(ctx, proc->body);

        /* Add return if not already terminated */
        if (!LLVMGetBasicBlockTerminator(LLVMGetInsertBlock(ctx->builder))) {
            if (result && LLVMGetTypeKind(LLVMTypeOf(result)) != LLVMVoidTypeKind) {
                LLVMBuildRet(ctx->builder, result);
            } else {
                LLVMBuildRetVoid(ctx->builder);
            }
        }

        /* Clear locals for next function */
        map_init(&ctx->locals);
    }

    return fn;
}

/*
 * Public expression codegen wrapper
 */
LLVMValueRef codegen_expr(CodegenContext *ctx, Expr *expr) {
    return codegen_expr_internal(ctx, expr);
}

/*
 * Public statement codegen wrapper
 */
void codegen_stmt(CodegenContext *ctx, Stmt *stmt) {
    codegen_stmt_internal(ctx, stmt);
}

#endif /* HAVE_LLVM */

/*
 * Generate code for a module
 */
bool codegen_module(CodegenContext *ctx, Module *mod) {
#ifdef HAVE_LLVM
    /* Generate all declarations */
    for (size_t i = 0; i < vec_len(mod->decls); i++) {
        Decl *decl = mod->decls[i];

        switch (decl->kind) {
            case DECL_PROC: {
                Symbol *sym = scope_lookup_from(ctx->sema->current_scope,
                    decl->proc.name);
                codegen_proc(ctx, &decl->proc, sym);
                break;
            }

            case DECL_RECORD:
            case DECL_ENUM:
            case DECL_MODAL:
                /* Type declarations are handled on demand during lowering */
                break;

            case DECL_EXTERN:
                /* Generate extern declarations */
                for (size_t j = 0; j < vec_len(decl->extern_.funcs); j++) {
                    ExternFuncDecl *ext = &decl->extern_.funcs[j];
                    size_t param_count = vec_len(ext->params);
                    LLVMTypeRef *param_types = arena_alloc(ctx->arena,
                        param_count * sizeof(LLVMTypeRef), _Alignof(LLVMTypeRef));
                    for (size_t k = 0; k < param_count; k++) {
                        param_types[k] = LLVMInt32TypeInContext(ctx->llvm_ctx);
                    }
                    LLVMTypeRef ret_type = LLVMInt32TypeInContext(ctx->llvm_ctx);
                    LLVMTypeRef fn_type = LLVMFunctionType(ret_type, param_types,
                        (unsigned)param_count, 0);
                    LLVMAddFunction(ctx->module, ext->name.data, fn_type);
                }
                break;

            default:
                break;
        }
    }

    /* Verify module */
    char *error = NULL;
    if (LLVMVerifyModule(ctx->module, LLVMReturnStatusAction, &error) != 0) {
        diag_emit(ctx->diag, DIAG_ERROR, (SourceSpan){0},
            "LLVM verification failed: %s", error);
        LLVMDisposeMessage(error);
        return false;
    }

    return true;
#else
    (void)ctx;
    (void)mod;
    return false;
#endif
}

/*
 * Write generated code to object file
 */
bool codegen_write_object(CodegenContext *ctx, const char *filename) {
#ifdef HAVE_LLVM
    char *error = NULL;
    if (LLVMTargetMachineEmitToFile(ctx->target_machine, ctx->module,
            (char *)filename, LLVMObjectFile, &error) != 0) {
        diag_emit(ctx->diag, DIAG_ERROR, (SourceSpan){0},
            "Failed to write object file: %s", error);
        LLVMDisposeMessage(error);
        return false;
    }
    return true;
#else
    (void)ctx;
    (void)filename;
    return false;
#endif
}

/*
 * Write generated code to LLVM IR
 */
bool codegen_write_ir(CodegenContext *ctx, const char *filename) {
#ifdef HAVE_LLVM
    char *error = NULL;
    if (LLVMPrintModuleToFile(ctx->module, filename, &error) != 0) {
        diag_emit(ctx->diag, DIAG_ERROR, (SourceSpan){0},
            "Failed to write IR: %s", error);
        LLVMDisposeMessage(error);
        return false;
    }
    return true;
#else
    (void)ctx;
    (void)filename;
    return false;
#endif
}

/*
 * Write generated code to bitcode
 */
bool codegen_write_bitcode(CodegenContext *ctx, const char *filename) {
#ifdef HAVE_LLVM
    if (LLVMWriteBitcodeToFile(ctx->module, filename) != 0) {
        diag_emit(ctx->diag, DIAG_ERROR, (SourceSpan){0},
            "Failed to write bitcode");
        return false;
    }
    return true;
#else
    (void)ctx;
    (void)filename;
    return false;
#endif
}
