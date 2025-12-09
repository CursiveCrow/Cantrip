/*
 * Cursive Bootstrap Compiler - Target Configuration
 *
 * Target platform detection and configuration.
 * Supports Windows x86-64 and Linux x86-64.
 */

#include "codegen.h"
#include <string.h>

#ifdef _WIN32
#define HOST_OS TARGET_OS_WINDOWS
#else
#define HOST_OS TARGET_OS_LINUX
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define HOST_ARCH TARGET_ARCH_X86_64
#elif defined(__aarch64__) || defined(_M_ARM64)
#define HOST_ARCH TARGET_ARCH_AARCH64
#else
#define HOST_ARCH TARGET_ARCH_X86_64  /* Default to x86-64 */
#endif

/*
 * Initialize target info for the host platform
 */
void target_init_host(TargetInfo *target) {
    target_init(target, HOST_OS, HOST_ARCH);
}

/*
 * Initialize target info for a specific platform
 */
void target_init(TargetInfo *target, TargetOS os, TargetArch arch) {
    memset(target, 0, sizeof(TargetInfo));
    target->os = os;
    target->arch = arch;

    /* Set pointer size based on architecture */
    switch (arch) {
        case TARGET_ARCH_X86_64:
        case TARGET_ARCH_AARCH64:
            target->pointer_size = 8;
            target->max_align = 16;
            break;
    }

    /* Set target triple */
    target->triple = target_get_triple(target);
}

/*
 * Get the LLVM target triple for a target
 */
const char *target_get_triple(TargetInfo *target) {
    switch (target->os) {
        case TARGET_OS_WINDOWS:
            switch (target->arch) {
                case TARGET_ARCH_X86_64:
                    return "x86_64-pc-windows-msvc";
                case TARGET_ARCH_AARCH64:
                    return "aarch64-pc-windows-msvc";
            }
            break;

        case TARGET_OS_LINUX:
            switch (target->arch) {
                case TARGET_ARCH_X86_64:
                    return "x86_64-unknown-linux-gnu";
                case TARGET_ARCH_AARCH64:
                    return "aarch64-unknown-linux-gnu";
            }
            break;
    }

    /* Fallback */
    return "x86_64-unknown-linux-gnu";
}
