# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-2_Core-Types.md  
**Section**: §17.2 Core Types and Utilities  
**Stable label**: [stdlib.core]  
**Forward references**: §17.4 [stdlib.behaviors], Clause 7 [type], Clause 10 [generic], Clause 11 [memory]

---

### §17.2 Core Types and Utilities [stdlib.core]

#### §17.2.1 Overview

[1] The `std::core` module provides fundamental types and utilities that form the foundation of the standard library. These types support error handling, optional values, and common programming patterns using Cursive's native type system.

[2] This module defines:

- Error handling types using union types
- Core utility types
- Conversion behaviors
- Fundamental procedures

[3] **Design Note**: This module uses Cursive's union types (`T \/ U`) rather than wrapper enums. The language's native union support eliminates the need for Rust-style `Option<T>` and `Result<T, E>` types.

#### §17.2.2 Error Handling with Union Types

##### §17.2.2.1 Philosophy

[4] Cursive uses **union types** (`T \/ ErrorType`) for error handling. The type system automatically widens success values and error values to the union type, enabling clean error propagation without wrapper types.

**Example 17.2.2.1** (Error handling pattern):

```cursive
// Procedure returning success value or error
procedure parse_int(text: string@View): i32 \/ ParseError
    [[ |- text.len() > 0 => true ]]
{
    if text.is_empty() {
        result ParseError::Empty  // Auto-widens to i32 \/ ParseError
    }

    // Parsing logic
    result parsed_value  // Auto-widens to i32 \/ ParseError
}

// Pattern matching for error handling
let result = parse_int("42")
match result {
    value: i32 => println("Success: {}", value),
    err: ParseError => println("Error: {}", err.message()),
}
```

##### §17.2.2.2 Common Error Types

[5] The standard library provides common error types for typical failure modes:

**ParseError** — Parsing failures:

```cursive
public enum ParseError {
    Empty,
    InvalidFormat { expected: string@View },
    OutOfRange { min: i64, max: i64, found: i64 },
    UnexpectedCharacter { position: usize, found: char },
}
```

**IoError** — I/O operation failures:

```cursive
public enum IoError {
    NotFound { path: string@View },
    PermissionDenied { path: string@View },
    AlreadyExists { path: string@View },
    EndOfFile,
    Interrupted,
    Other { message: string@Managed },
}
```

**AllocationError** — Memory allocation failures:

```cursive
public enum AllocationError {
    OutOfMemory { requested: usize },
    InvalidSize { size: usize },
}
```

[6] Procedures returning these errors use union types:

```cursive
procedure allocate_buffer(size: usize): Buffer \/ AllocationError
    [[ alloc::heap |- size > 0 => true ]]
```

#### §17.2.3 Optional Values

##### §17.2.3.1 Using Union with Unit Type

[7] Optional values (presence or absence) use union with a unit-like type:

**None** — Represents absence:

```cursive
public record None { }

// Optional value pattern
type Optional<T> = T \/ None
```

**Example 17.2.3.1** (Optional value usage):

```cursive
procedure find_user(id: u64): User \/ None
    [[ database::query |- id > 0 => true ]]
{
    // Search for user
    if found {
        result user  // Widens to User \/ None
    } else {
        result None { }  // Widens to User \/ None
    }
}

// Pattern matching
match find_user(123) {
    user: User => process(user),
    _: None => println("User not found"),
}
```

##### §17.2.3.2 Multiple Error Types

[8] Union types compose naturally for procedures with multiple failure modes:

```cursive
procedure load_config(path: string@View): Config \/ IoError \/ ParseError
    [[ fs::read, alloc::heap |- path.len() > 0 => true ]]
{
    // Read file
    let contents = read_file(path)
    match contents {
        data: string@Managed => {
            // Parse config
            let parsed = parse_config(data)
            match parsed {
                config: Config => result config,  // Widens to 3-way union
                err: ParseError => result err,     // Widens to 3-way union
            }
        }
        err: IoError => result err,  // Widens to 3-way union
    }
}
```

#### §17.2.4 Core Utilities

##### §17.2.4.1 Panic and Abort

[9] Standard panic procedures:

```cursive
public procedure panic(message: string@View): !
    [[ panic |- true => false ]]
{
    // Implementation triggers panic with message
    // Never returns (type: !)
}

public procedure abort(): !
    [[ sys::exit |- true => false ]]
{
    // Immediate process termination without unwinding
}
```

##### §17.2.4.2 Assertion Utilities

[10] Runtime assertions:

```cursive
public procedure assert(condition: bool, message: string@View): ()
    [[ panic |- true => true ]]
{
    if !condition {
        panic(message)
    }
}

public procedure debug_assert(condition: bool, message: string@View): ()
    [[ panic |- true => true ]]
{
    // Only checked in debug builds
    comptime {
        if cfg("debug_assertions") {
            // Compiled to assert
        } else {
            // No-op in release
        }
    }
}
```

##### §17.2.4.3 Min/Max Utilities

[11] Comparison utilities:

```cursive
public procedure min<T: Ord>(a: T, b: T): T
    [[ |- true => result == a \/ result == b ]]
{
    if a.compare(b) <= 0 {
        result a
    } else {
        result b
    }
}

public procedure max<T: Ord>(a: T, b: T): T
    [[ |- true => result == a \/ result == b ]]
{
    if a.compare(b) >= 0 {
        result a
    } else {
        result b
    }
}
```

#### §17.2.5 Conversion Behaviors

##### §17.2.5.1 Into Behavior

[12] Generic type conversion:

```cursive
public behavior Into<T> {
    procedure into(~): T
        [[ alloc::heap |- true => true ]]
    {
        panic("Into not implemented")
    }
}

public behavior From<T> {
    procedure from(value: T): Self
        [[ alloc::heap |- true => true ]]
    {
        panic("From not implemented")
    }
}
```

##### §17.2.5.2 AsRef Behavior

[13] Reference conversion:

```cursive
public behavior AsRef<T> {
    procedure as_ref(~): T
    {
        panic("AsRef not implemented")
    }
}
```

#### §17.2.6 Conformance Requirements

[14] Implementations shall:

1. Provide all error types specified in §17.2.2.2
2. Implement panic and assertion utilities per §17.2.4
3. Provide min/max utilities for Ord types
4. Include all specified behaviors with default implementations
5. Use union types (`T \/ E`) for all error handling, not wrapper enums
6. Document all error types comprehensively

---

**Previous**: §17.1 Standard Library Overview (§17.1 [stdlib.overview]) | **Next**: §17.3 Built-In Modal Types (§17.3 [stdlib.types])
