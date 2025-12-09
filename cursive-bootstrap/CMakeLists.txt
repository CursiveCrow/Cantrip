cmake_minimum_required(VERSION 3.16)
project(cursive-bootstrap C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(MSVC)
    # /W4 = high warning level, /WX = warnings as errors
    # Disable specific warnings that are noisy but not critical:
    # C4200: zero-sized array (flexible array member, standard C99)
    # C4996: deprecated POSIX names (strdup -> _strdup, etc.)
    add_compile_options(/W4 /wd4200 /wd4996)
    # Define _CRT_SECURE_NO_WARNINGS to suppress secure function warnings
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Find LLVM
find_package(LLVM 15 CONFIG)
if(NOT LLVM_FOUND)
    find_package(LLVM 16 CONFIG)
endif()
if(NOT LLVM_FOUND)
    find_package(LLVM 17 CONFIG)
endif()
if(NOT LLVM_FOUND)
    find_package(LLVM 18 CONFIG)
endif()

if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(llvm_libs core support native)
else()
    message(WARNING "LLVM not found - codegen will be disabled")
    set(llvm_libs "")
endif()

# Common library (utilities shared across all components)
add_library(cursive_common STATIC
    src/common/arena.c
    src/common/string_pool.c
    src/common/vec.c
    src/common/map.c
    src/common/error.c
)
target_include_directories(cursive_common PUBLIC src)

# Lexer library
add_library(cursive_lexer STATIC
    src/lexer/token.c
    src/lexer/lexer.c
    src/lexer/unicode.c
)
target_link_libraries(cursive_lexer cursive_common)
target_include_directories(cursive_lexer PUBLIC src)

# Parser library
add_library(cursive_parser STATIC
    src/parser/ast.c
    src/parser/parser.c
    src/parser/pretty.c
)
target_link_libraries(cursive_parser cursive_lexer cursive_common)
target_include_directories(cursive_parser PUBLIC src)

# Semantic analysis library
add_library(cursive_sema STATIC
    src/sema/sema.c
    src/sema/scope.c
    src/sema/types.c
    src/sema/resolve.c
    src/sema/typecheck.c
    src/sema/moves.c
    src/sema/perms.c
)
target_link_libraries(cursive_sema cursive_parser cursive_common)
target_include_directories(cursive_sema PUBLIC src)

# Code generation library (only if LLVM found)
if(LLVM_FOUND)
    add_library(cursive_codegen STATIC
        src/codegen/lower.c
        src/codegen/codegen.c
        src/codegen/target.c
    )
    target_link_libraries(cursive_codegen cursive_sema ${llvm_libs})
    target_include_directories(cursive_codegen PUBLIC src)
endif()

# Runtime library
add_library(cursive_rt STATIC
    src/runtime/cursive_rt.c
)
target_include_directories(cursive_rt PUBLIC src/runtime)

# Main compiler executable
add_executable(cursivec
    src/main.c
)
if(LLVM_FOUND)
    target_link_libraries(cursivec cursive_codegen cursive_sema cursive_parser cursive_lexer cursive_common)
    target_compile_definitions(cursivec PRIVATE HAVE_LLVM=1)
else()
    target_link_libraries(cursivec cursive_sema cursive_parser cursive_lexer cursive_common)
endif()

# Tests
enable_testing()

# Lexer tests
add_executable(test_lexer tests/lexer/test_lexer.c)
target_link_libraries(test_lexer cursive_lexer cursive_common)
add_test(NAME lexer_tests COMMAND test_lexer)

# Parser tests
add_executable(test_parser tests/parser/test_parser.c)
target_link_libraries(test_parser cursive_parser cursive_lexer cursive_common)
add_test(NAME parser_tests COMMAND test_parser)

# Semantic analysis tests
add_executable(test_resolve tests/sema/test_resolve.c)
target_link_libraries(test_resolve cursive_sema cursive_parser cursive_lexer cursive_common)
add_test(NAME resolve_tests COMMAND test_resolve)

# Move analysis tests
add_executable(test_moves tests/sema/test_moves.c)
target_link_libraries(test_moves cursive_sema cursive_parser cursive_lexer cursive_common)
add_test(NAME moves_tests COMMAND test_moves)
