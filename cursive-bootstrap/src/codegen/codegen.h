/*
 * Cursive Bootstrap Compiler - Code Generation
 *
 * LLVM-based code generation for the Cursive compiler.
 * Translates typed AST to LLVM IR for native code generation.
 */

#ifndef CURSIVE_CODEGEN_H
#define CURSIVE_CODEGEN_H

#include "common/common.h"
#include "common/arena.h"
#include "common/map.h"
#include "common/string_pool.h"
#include "parser/ast.h"
#include "sema/sema.h"

#ifdef HAVE_LLVM
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#endif

/*
 * Target platform configuration
 */
typedef enum TargetOS {
    TARGET_OS_WINDOWS,
    TARGET_OS_LINUX
} TargetOS;

typedef enum TargetArch {
    TARGET_ARCH_X86_64,
    TARGET_ARCH_AARCH64
} TargetArch;

typedef struct TargetInfo {
    TargetOS os;
    TargetArch arch;
    const char *triple;           /* LLVM target triple */
    size_t pointer_size;          /* Pointer size in bytes */
    size_t max_align;             /* Maximum alignment */
} TargetInfo;

/*
 * Code generation context
 */
typedef struct CodegenContext {
#ifdef HAVE_LLVM
    LLVMContextRef llvm_ctx;      /* LLVM context */
    LLVMModuleRef module;         /* Current module */
    LLVMBuilderRef builder;       /* IR builder */
    LLVMTargetDataRef target_data; /* Target data layout */
    LLVMTargetMachineRef target_machine; /* Target machine */
#endif

    Arena *arena;                 /* Memory arena */
    SemaContext *sema;            /* Semantic context */
    StringPool *strings;          /* String pool */

    Map type_cache;               /* Type* -> LLVMTypeRef */
    Map func_cache;               /* Symbol* -> LLVMValueRef */
    Map global_cache;             /* Symbol* -> LLVMValueRef */

    /* Current function context */
#ifdef HAVE_LLVM
    LLVMValueRef current_func;    /* Current function being generated */
    LLVMBasicBlockRef entry_block; /* Function entry block */
    LLVMBasicBlockRef return_block; /* Return block for cleanup */
    LLVMValueRef return_value;    /* Alloca for return value */
#endif

    /* Local variables map: Symbol* -> LLVMValueRef (stack alloca) */
    Map locals;

    /* Loop context for break/continue */
#ifdef HAVE_LLVM
    LLVMBasicBlockRef loop_break_block;
    LLVMBasicBlockRef loop_continue_block;
#endif

    /* Target information */
    TargetInfo target;

    /* Diagnostic context */
    DiagContext *diag;
} CodegenContext;

/*
 * Initialize code generation context
 */
bool codegen_init(CodegenContext *ctx, Arena *arena, SemaContext *sema,
                  DiagContext *diag, const char *module_name);

/*
 * Cleanup code generation context
 */
void codegen_destroy(CodegenContext *ctx);

/*
 * Generate code for a module
 */
bool codegen_module(CodegenContext *ctx, Module *mod);

/*
 * Write generated code to file
 */
bool codegen_write_object(CodegenContext *ctx, const char *filename);
bool codegen_write_ir(CodegenContext *ctx, const char *filename);
bool codegen_write_bitcode(CodegenContext *ctx, const char *filename);

/*
 * Type lowering - convert Cursive types to LLVM types
 */
#ifdef HAVE_LLVM
LLVMTypeRef lower_type(CodegenContext *ctx, Type *type);

/*
 * Expression code generation
 */
LLVMValueRef codegen_expr(CodegenContext *ctx, Expr *expr);

/*
 * Statement code generation
 */
void codegen_stmt(CodegenContext *ctx, Stmt *stmt);

/*
 * Procedure code generation
 */
LLVMValueRef codegen_proc(CodegenContext *ctx, ProcDecl *proc, Symbol *sym);
#endif

/*
 * Target detection and configuration
 */
void target_init_host(TargetInfo *target);
void target_init(TargetInfo *target, TargetOS os, TargetArch arch);
const char *target_get_triple(TargetInfo *target);

#endif /* CURSIVE_CODEGEN_H */
