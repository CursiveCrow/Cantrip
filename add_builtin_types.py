#!/usr/bin/env python3
"""
Add §2.1.7 Built-In Sum Types to the primitive types file
"""

import re

def main():
    filepath = 'Spec/02_Types/PART_A_Primitives/01_PrimitiveTypes.md'
    
    print(f'Reading {filepath}...')
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Find the end of §2.1.6 (String Types) section
    # We'll insert before the final navigation footer
    
    builtin_section = '''

---

## 2.1.7 Built-In Sum Types

Cantrip provides two fundamental sum types that are so pervasive they are considered built-in primitives despite being implemented as enums. These types provide the foundation for Cantrip's error handling and null safety.

**Definition 2.1.7.1 (Built-In Sum Types):** The types `Option<T>` and `Result<T, E>` are built-in generic sum types that provide type-safe alternatives to null pointers and exception-based error handling. While structurally implemented as enums, they receive special treatment in the type system and integrate deeply with Cantrip's contract and effect systems.

### 2.1.7.1 Option<T>

#### Overview

**Key innovation/purpose:** `Option<T>` provides type-safe optional values, eliminating null pointer errors by making the absence of a value explicit in the type system.

**When to use Option<T>:**
- Values that may or may not be present
- Function returns that may have no meaningful result
- Nullable fields in data structures
- Search operations that may fail to find a result
- Configuration values with defaults

**When NOT to use Option<T>:**
- Error conditions with diagnostic information → use `Result<T, E>`
- Boolean flags → use `bool` directly
- Collections that may be empty → use empty collection
- Values that are always present → use `T` directly

**Relationship to other features:**
- **Pattern matching**: Option must be matched exhaustively
- **Pointer types**: `Option<Ptr<T>>` uses niche optimization (null = None)
- **Permissions**: Option wraps permissions: `Option<own T>`, `Option<mut T>`
- **Contracts**: Postconditions can specify Option return conditions

#### Syntax

**Type definition (conceptual - actually built-in):**
```cantrip
enum Option<T> {
    Some(T),
    None,
}
```

**Construction:**
```cantrip
// Create Some variant
let some_value: Option<i32> = Option.Some(42)

// Create None variant
let no_value: Option<i32> = Option.None
```

**Pattern matching:**
```cantrip
match some_value {
    Option.Some(x) => println("Got value: {x}"),
    Option.None => println("No value"),
}
```

#### Static Semantics

**Type rules:**

```
[T-Option-Some]
Γ ⊢ e : T
──────────────────────────
Γ ⊢ Option.Some(e) : Option<T>

[T-Option-None]
──────────────────────────
Γ ⊢ Option.None : Option<T>
    (T inferred from context)
```

**Pattern matching rule:**

```
[T-Option-Match]
Γ ⊢ e : Option<T>
Γ, x : T ⊢ e_some : U
Γ ⊢ e_none : U
──────────────────────────────────────────────
Γ ⊢ match e {
      Option.Some(x) => e_some,
      Option.None => e_none
    } : U
```

#### Memory Representation

**Standard layout:**
```
Option<T>:
┌──────────────┬──────────────────────┐
│ Discriminant │    Payload (T)       │
│  (1-8 bytes) │  (sizeof(T) bytes)   │
└──────────────┴──────────────────────┘

Size: max(sizeof(T) + discriminant_size, pointer_size)
Alignment: max(alignof(T), alignof(discriminant))
```

**Niche optimization (for non-nullable pointers):**
```
Option<Ptr<T>@Exclusive>:
┌──────────────┐
│   Pointer    │  Some = valid pointer, None = null (0x0)
│   8 bytes    │  No discriminant needed!
└──────────────┘

Size: 8 bytes (same as Ptr<T>)
Alignment: 8 bytes
```

**Theorem 2.1.7.1 (Niche Optimization):** For types T with invalid bit patterns (e.g., non-nullable pointers), `sizeof(Option<T>) = sizeof(T)`. The compiler uses the invalid pattern to represent `None`.

#### Common Methods

**Note:** These are library methods, but documented here due to fundamental nature.

```cantrip
impl<T> Option<T> {
    // Query methods
    procedure is_some(self: Option<T>): bool
    procedure is_none(self: Option<T>): bool

    // Unwrapping (requires precondition or panics)
    procedure unwrap(self: Option<T>): T
        must self.is_some()
        will match self { Option.Some(v) => result == v }

    // Safe unwrapping with default
    procedure unwrap_or(self: Option<T>, default: T): T
        will match self {
            Option.Some(v) => result == v,
            Option.None => result == default
        }

    // Mapping
    procedure map<U>(self: Option<T>, f: map(T) -> U): Option<U>
        will match self {
            Option.Some(v) => result == Option.Some(f(v)),
            Option.None => result == Option.None
        }

    // Chaining
    procedure and_then<U>(self: Option<T>, f: map(T) -> Option<U>): Option<U>
        will match self {
            Option.Some(v) => result == f(v),
            Option.None => result == Option.None
        }
}
```

#### Examples

**Search returning Option:**
```cantrip
procedure find_first_positive(numbers: [i32]): Option<i32> {
    for n in numbers {
        if n > 0 {
            return Option.Some(n)
        }
    }
    Option.None
}

// Usage
let result = find_first_positive([−1, −2, 3, 4])
match result {
    Option.Some(value) => println("Found: {value}"),
    Option.None => println("No positive numbers"),
}
```

**Optional fields:**
```cantrip
record Config {
    port: u16,
    host: String,
    tls_cert: Option<String>,  // May or may not have TLS
}

let config = Config {
    port: 8080,
    host: "localhost",
    tls_cert: Option.None,  // No TLS
}
```

### 2.1.7.2 Result<T, E>

#### Overview

**Key innovation/purpose:** `Result<T, E>` provides type-safe error handling without exceptions, making errors explicit in function signatures and enabling exhaustive error checking through pattern matching.

**When to use Result<T, E>:**
- Operations that can fail with error information
- I/O operations (file, network)
- Parsing and validation
- Resource allocation that may fail
- Any fallible computation needing error details

**When NOT to use Result<T, E>:**
- Logic errors that should never happen → use `must` preconditions or `panic`
- Simple presence/absence → use `Option<T>`
- Control flow without errors → use enums
- Errors that should terminate → use `panic` or `exit`

**Relationship to other features:**
- **Contract system**: Result integrates with effects and postconditions
- **Pattern matching**: Result must be matched exhaustively
- **Error propagation**: Natural composition through match or helper methods
- **Type aliases**: Common to alias `Result<T, E>` with fixed error type

#### Syntax

**Type definition (conceptual - actually built-in):**
```cantrip
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

**Construction:**
```cantrip
// Success case
let success: Result<i32, String> = Result.Ok(42)

// Error case
let failure: Result<i32, String> = Result.Err("computation failed")
```

**Pattern matching:**
```cantrip
match success {
    Result.Ok(value) => println("Success: {value}"),
    Result.Err(error) => println("Error: {error}"),
}
```

#### Static Semantics

**Type rules:**

```
[T-Result-Ok]
Γ ⊢ e : T
──────────────────────────
Γ ⊢ Result.Ok(e) : Result<T, E>
    (E inferred from context)

[T-Result-Err]
Γ ⊢ e : E
──────────────────────────
Γ ⊢ Result.Err(e) : Result<T, E>
    (T inferred from context)
```

**Pattern matching rule:**

```
[T-Result-Match]
Γ ⊢ e : Result<T, E>
Γ, x : T ⊢ e_ok : U
Γ, err : E ⊢ e_err : U
──────────────────────────────────────────────
Γ ⊢ match e {
      Result.Ok(x) => e_ok,
      Result.Err(err) => e_err
    } : U
```

**Effect integration:**

```
[Effect-Result]
procedure p(...): Result<T, E> uses effects
──────────────────────────────────────────────
Effects include all potential error-producing effects
```

#### Memory Representation

**Standard layout:**
```
Result<T, E>:
┌──────────────┬──────────────────────────────────┐
│ Discriminant │  Payload (max(T, E))             │
│  (1-8 bytes) │  (max(sizeof(T), sizeof(E)))     │
└──────────────┴──────────────────────────────────┘

Size: discriminant_size + max(sizeof(T), sizeof(E))
Alignment: max(alignof(T), alignof(E), alignof(discriminant))
```

**Example sizes:**
```
Result<i32, String>:
  - Discriminant: 1 byte
  - i32: 4 bytes
  - String: 24 bytes (ptr + len + cap)
  - Total: ~32 bytes (with padding)

Result<(), ErrorCode>:  // Common pattern for operations without meaningful return
  - Discriminant: 1 byte
  - (): 0 bytes
  - ErrorCode: depends on error type
```

#### Common Methods

```cantrip
impl<T, E> Result<T, E> {
    // Query methods
    procedure is_ok(self: Result<T, E>): bool
    procedure is_err(self: Result<T, E>): bool

    // Unwrapping (requires precondition or panics)
    procedure unwrap(self: Result<T, E>): T
        must self.is_ok()
        will match self { Result.Ok(v) => result == v }

    procedure unwrap_err(self: Result<T, E>): E
        must self.is_err()
        will match self { Result.Err(e) => result == e }

    // Safe unwrapping with default
    procedure unwrap_or(self: Result<T, E>, default: T): T
        will match self {
            Result.Ok(v) => result == v,
            Result.Err(_) => result == default
        }

    // Mapping (transform success value, keep error)
    procedure map<U>(self: Result<T, E>, f: map(T) -> U): Result<U, E>
        will match self {
            Result.Ok(v) => result == Result.Ok(f(v)),
            Result.Err(e) => result == Result.Err(e)
        }

    // Mapping error (keep success, transform error)
    procedure map_err<F>(self: Result<T, E>, f: map(E) -> F): Result<T, F>
        will match self {
            Result.Ok(v) => result == Result.Ok(v),
            Result.Err(e) => result == Result.Err(f(e))
        }

    // Chaining (monadic bind)
    procedure and_then<U>(self: Result<T, E>, f: map(T) -> Result<U, E>): Result<U, E>
        will match self {
            Result.Ok(v) => result == f(v),
            Result.Err(e) => result == Result.Err(e)
        }
}
```

#### Examples

**File operations:**
```cantrip
procedure read_config(path: String): Result<Config, Error>
    uses io.read, alloc.heap
{
    match std.fs.read_to_string(path) {
        Result.Ok(contents) => {
            match parse_config(contents) {
                Result.Ok(config) => Result.Ok(config),
                Result.Err(e) => Result.Err(e),
            }
        },
        Result.Err(e) => Result.Err(e),
    }
}
```

**Parsing with Result:**
```cantrip
procedure parse_port(s: String): Result<u16, ParseError>
    will match result {
        Result.Ok(port) => port >= 0 && port <= 65535,
        Result.Err(_) => true
    }
{
    match s.parse::<u16>() {
        Result.Ok(n) if n <= 65535 => Result.Ok(n),
        Result.Ok(_) => Result.Err(ParseError.OutOfRange),
        Result.Err(_) => Result.Err(ParseError.InvalidFormat),
    }
}
```

**Type aliases for domain-specific Results:**
```cantrip
type IoResult<T> = Result<T, IoError>
type ParseResult<T> = Result<T, ParseError>

procedure read_file(path: String): IoResult<String>
    uses io.read, alloc.heap
{
    std.fs.read_to_string(path)
}
```

#### Integration with Contracts

**Postconditions with Result:**
```cantrip
procedure safe_divide(a: f64, b: f64): Result<f64, String>
    will match result {
        Result.Ok(v) => v == a / b && b != 0.0,
        Result.Err(_) => b == 0.0
    }
{
    if b == 0.0 {
        Result.Err("division by zero")
    } else {
        Result.Ok(a / b)
    }
}
```

**Effects with Result:**
```cantrip
procedure fetch_data(url: String): Result<Data, NetworkError>
    uses network.http, alloc.heap
    will match result {
        Result.Ok(data) => data.is_valid(),
        Result.Err(_) => true
    }
{
    // Implementation
}
```

### 2.1.7.3 Design Rationale

**Why are Option and Result primitives?**

1. **Pervasive use**: Every Cantrip program uses these types extensively
2. **Compiler integration**: Niche optimization, pattern exhaustiveness checking
3. **Effect system**: Result integrates with Cantrip's effect tracking
4. **Error handling foundation**: Result is Cantrip's primary error mechanism
5. **Null safety**: Option eliminates entire class of null pointer bugs

**Why not exceptions?**

Cantrip uses Result<T, E> instead of exceptions because:
- **Explicit in types**: Function signatures show they can fail
- **Exhaustive handling**: Compiler enforces error checking
- **Composable**: Easy to chain fallible operations
- **Zero overhead**: No exception unwinding machinery
- **Permission-safe**: Works naturally with permission system

**Why not built-in `?` operator?**

Cantrip doesn't have Rust's `?` operator because:
- **Explicit over implicit**: Pattern matching makes control flow visible
- **Simpler language**: One less special case operator
- **Methods suffice**: `and_then` and `map` provide composition
- **Clarity**: Error propagation is explicit at each step

However, library or macro support for error propagation patterns is expected.

### 2.1.7.4 Comparison with Other Approaches

**Cantrip vs Rust:**
- **Same**: Option and Result as primary mechanisms
- **Different**: No `?` operator (yet), explicit pattern matching
- **Different**: Permission system (not borrow checker) affects ownership transfer

**Cantrip vs Java/C++:**
- **Cantrip**: Type-safe Option/Result with exhaustiveness checking
- **Java/C++**: Exceptions with no compile-time checking, nullable types

**Cantrip vs Go:**
- **Cantrip**: Result<T, E> with type-safe errors
- **Go**: (T, error) tuple with convention-based checking

**Cantrip vs Haskell:**
- **Similar**: Maybe/Either monads map to Option/Result
- **Different**: Cantrip uses explicit match, Haskell uses `do` notation

---

'''
    
    # Insert before final navigation footer
    footer_pattern = r'\n---\n\n\*\*Previous\*\*:.*?\*\*Next\*\*:.*?\n$'
    match = re.search(footer_pattern, content, re.DOTALL)
    
    if match:
        insertion_point = match.start()
        new_content = content[:insertion_point] + builtin_section + content[insertion_point:]
    else:
        # If no footer found, append at end
        new_content = content + builtin_section
    
    print(f'Writing updated file...')
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    print('Done! Added §2.1.7 Built-In Sum Types')

if __name__ == '__main__':
    main()
