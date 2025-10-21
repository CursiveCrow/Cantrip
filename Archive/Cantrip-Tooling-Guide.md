# The Cantrip Tooling and Implementation Guide

**Version:** 1.0.0 (compatible with 0.7.x)  
**Date:** October 21, 2025  
**Status:** Implementation Guide  
**Target Audience:** Compiler implementers, tool developers, and application developers

---

## Preface

This document specifies the **Cantrip Tooling Ecosystem**, including the compiler architecture, error reporting, package management, and testing framework.

This guide is a companion to the **Cantrip Language Specification v1.0.0**. For language semantics, type system, effect system, and other core language features, refer to the main language specification.

### Conformance

Implementations of Cantrip tooling SHOULD follow the conventions and formats specified in this document to ensure consistency and interoperability across the ecosystem. However, alternative implementations are permitted as long as they maintain compatibility with the language specification.

### Cross-References

- **Main Specification:** \Cantrip-Language-Spec-1.0.0.md- **Standard Library:** \Cantrip-Standard-Library-Spec.md- **Error Codes:** \Cantrip-Error-Codes.md
---

# Part XIII (Normative): Tooling and Implementation

## 51. Compiler Architecture

### 51.1 Compilation Pipeline

**Definition 51.1 (Compilation Pipeline):** The sequence of transformations from source to executable.

**Phases:**
```
Source Code (.arc)
    ↓
[1. Lexical Analysis]
    ↓
Token Stream
    ↓
[2. Parsing]
    ↓
Abstract Syntax Tree (AST)
    ↓
[3. Name Resolution]
    ↓
Resolved AST
    ↓
[4. Type Checking]
    ↓
Typed AST
    ↓
[5. Effect Checking]
    ↓
Effect-Annotated AST
    ↓
[6. Contract Verification]
    ↓
Verified AST
    ↓
[7. Modal State Analysis]
    ↓
State-Verified AST
    ↓
[8. Borrow/Region Analysis]
    ↓
Lifetime-Annotated AST
    ↓
[9. Optimization]
    ↓
Optimized IR
    ↓
[10. Code Generation]
    ↓
Native Code / Bytecode
```

### 51.2 Compiler Invocation

**Basic compilation:**
```bash
arc compile main.arc -o output
```

**Options:**
```bash
arc compile main.arc \
    --opt=2 \                    # Optimization level (0-3)
    --verify=static \            # Contract verification mode
    --target=x86_64-linux \      # Target platform
    --emit=asm \                 # Emit assembly
    --no-default-features \      # Disable default features
    --features=experimental      # Enable features
```

### 51.3 Optimization Levels

**Optimization levels:**

| Level | Description | Compile Time | Runtime Performance |
|-------|-------------|--------------|---------------------|
| `--opt=0` | No optimization | Fast | Slow |
| `--opt=1` | Basic optimization | Moderate | Good |
| `--opt=2` | Standard (default) | Moderate | Very Good |
| `--opt=3` | Aggressive | Slow | Excellent |

**Optimizations:**
- Level 0: None
- Level 1: Dead code elimination, constant folding
- Level 2: Inlining, loop optimizations, register allocation
- Level 3: Inter-procedural optimization, vectorization, LTO

### 51.4 Verification Modes

**Three verification strategies:**

```bash
# Static verification (must prove or fail compilation)
arc compile --verify=static main.arc

# Runtime verification (default - insert runtime checks)
arc compile --verify=runtime main.arc

# No verification (unsafe - trust programmer)
arc compile --verify=none main.arc
```

**Per-function override:**
```cantrip
#[verify(static)]
function critical() { ... }

#[verify(runtime)]
function normal() { ... }

#[verify(none)]
unsafe function low_level() { ... }
```

### 51.5 Incremental Compilation

**Compiler maintains dependency graph:**

```
Module Dependency Graph:
  main → http → net.tcp
  main → json
  http → json
```

**Recompilation:**
- Only recompile changed modules
- Recompile dependent modules
- Use cached artifacts for unchanged modules

**Cache structure:**
```
target/
├── cache/
│   ├── main.arc.o
│   ├── http.arc.o
│   └── json.arc.o
└── deps/
    └── dependency_graph.json
```

---

## 52. Error Reporting

### 52.1 Error Message Format

**Standard error format:**
```
error[E####]: Error message
  --> file:line:column
   |
## | source code line
   | ^^^^^ explanation
   |
   = note: Additional information
   = help: Suggestion for fix
```

### 52.2 Example Error Messages

**Type error:**
```bash
error[E2001]: Type mismatch
  --> src/main.arc:15:9
   |
15 |     let x: i32 = "string";
   |                  ^^^^^^^^ expected i32, found str
   |
   = note: Types must match exactly in Cantrip
   = help: Consider using: let x: i32 = 42;
```

**Effect error:**
```bash
error[E9001]: Missing effect declaration
  --> src/io.arc:8:9
   |
 8 |         Vec.new()
   |         ^^^^^^^^^ requires effect alloc.heap
   |
note: function declared as pure
  --> src/io.arc:5:5
   |
 5 |     requires:
   |     ^^^^^^^^^ declared with no effects
   |
help: add effect declaration
   |
 5 |     requires<alloc.heap>:
   |             ^^^^^^^^^^^^
```

**Modal state error:**
```bash
error[E3003]: Procedure not available in current state
  --> src/file.arc:42:10
   |
42 |     file.read(buffer);
   |          ^^^^ procedure 'read' not available in state Closed
   |
note: file has type File@Closed
  --> src/file.arc:40:9
   |
40 |     let file = File.new("data.txt");
   |         ^^^^ inferred type: File@Closed
   |
help: call 'open' to transition to Open state
   |
41 |     let file = file.open()?;
   |                ^^^^^^^^^^^
```

### 52.3 Machine-Readable Output

**JSON format:**
```bash
arc compile --output-format=json main.arc
```

**Output:**
```json
{
  "version": "0.4.0",
  "diagnostics": [
    {
      "id": "E2001",
      "severity": "error",
      "message": "Type mismatch",
      "location": {
        "file": "src/main.arc",
        "line": 15,
        "column": 9,
        "span": {
          "start": 245,
          "end": 253
        }
      },
      "fixes": [
        {
          "description": "Change to i32",
          "edits": [
            {
              "location": { "start": 245, "end": 253 },
              "replacement": "42"
            }
          ]
        }
      ]
    }
  ]
}
```

---

## 53. Package Management

### 53.1 Project Structure

**Standard project layout:**
```
myproject/
├── arc.toml              # Package manifest
├── src/
│   ├── main.arc          # Entry point
│   └── lib.arc           # Library root (optional)
├── tests/
│   └── integration.arc   # Integration tests
├── examples/
│   └── simple.arc        # Example programs
├── benches/
│   └── performance.arc   # Benchmarks
└── target/               # Build artifacts (generated)
    ├── debug/
    └── release/
```

### 53.2 Package Manifest

**`arc.toml` format:**
```toml
[package]
name = "myproject"
version = "1.0.0"
authors = ["Your Name <you@example.com>"]
edition = "0.4.0"
description = "A short description"
license = "MIT OR Apache-2.0"
repository = "https://github.com/user/myproject"

[dependencies]
http = "2.0"
json = { version = "1.5", features = ["pretty"] }
local_crate = { path = "../local_crate" }

[dev-dependencies]
test_utils = "0.3"

[features]
default = ["std"]
std = []
experimental = []

[profile.release]
opt-level = 3
verify = "runtime"

[profile.dev]
opt-level = 0
verify = "runtime"
```

### 53.3 Commands

**Package management commands:**

```bash
# Create new project
arc new myproject
arc new --lib mylibrary

# Build project
arc build
arc build --release

# Run project
arc run
arc run --release

# Test project
arc test
arc test --test integration

# Benchmark
arc bench

# Clean build artifacts
arc clean

# Update dependencies
arc update

# Publish to registry
arc publish
```

### 53.4 Dependency Resolution

**Version specification:**
```toml
[dependencies]
# Exact version
exact = "=1.0.0"

# Compatible versions (semver)
compatible = "1.2"         # >=1.2.0, <2.0.0
caret = "^1.2.3"           # >=1.2.3, <2.0.0
tilde = "~1.2.3"           # >=1.2.3, <1.3.0

# Version ranges
range = ">=1.2, <1.5"

# Wildcard
any = "*"
```

**Resolution algorithm:**
1. Parse dependency specifications
2. Fetch package metadata from registry
3. Build dependency graph
4. Check for conflicts
5. Select versions satisfying all constraints
6. Download and cache packages

---

## 54. Testing Framework

### 54.1 Unit Tests

**Test annotation:**
```cantrip
#[test]
function test_addition() {
    assert_eq(2 + 2, 4);
}

#[test]
function test_subtraction() {
    assert_eq(5 - 3, 2);
}
```

**Test macros:**
```cantrip
assert!(condition);
assert_eq!(left, right);
assert_ne!(left, right);
```

### 54.2 Integration Tests

**Integration test file (`tests/integration.arc`):**
```cantrip
import mylib;

#[test]
function test_end_to_end() {
    let result = mylib.process_data(input);
    assert_eq(result, expected);
}
```

### 54.3 Property-Based Testing

**Property tests:**
```cantrip
#[property_test(cases = 1000)]
function test_sort_preserves_length(arr: Vec<i32>) {
    let original_len = arr.length();
    let sorted = arr.sort();
    assert_eq(sorted.length(), original_len);
}

#[property_test(cases = 1000)]
function test_addition_commutative(a: i32, b: i32) {
    assert_eq(a + b, b + a);
}
```

### 54.4 Benchmarks

**Benchmark file (`benches/performance.arc`):**
```cantrip
#[bench]
function bench_vector_push(b: Bencher) {
    b.iter(|| {
        var v = Vec.new();
        for i in 0..1000 {
            v.push(i);
        }
    });
}

#[bench]
function bench_hashmap_insert(b: Bencher) {
    b.iter(|| {
        var map = HashMap.new();
        for i in 0..1000 {
            map.insert(i, i * 2);
        }
    });
}
```

---
