# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-1_Overview.md  
**Section**: §17.1 Standard Library Overview  
**Stable label**: [stdlib.overview]  
**Forward references**: §17.2 [stdlib.core], §17.3 [stdlib.types], §17.4 [stdlib.behaviors], §17.5 [stdlib.collections], §17.6 [stdlib.io], §17.7 [stdlib.concurrency], Clause 7 [type], Clause 10 [generic], Clause 11 [memory], Clause 12 [contract], Clause 14 [concurrency]

---

### §17.1 Standard Library Overview [stdlib.overview]

#### §17.1.1 Overview

[1] The Cursive standard library provides essential types, behaviors, and procedures that complement the core language features specified in Clauses 1-16. While the language defines the syntax and semantics for built-in constructs, the standard library supplies the foundational utilities programmers need for practical development.

[2] This clause specifies the normative standard library components that conforming implementations shall provide. The library is organized into modules reflecting common use cases: core types, collections, I/O, concurrency, and platform-specific functionality.

[3] **Design Principles**:

- **Minimal but Complete**: Provide essential functionality without bloat
- **Zero-Cost**: All abstractions compile to efficient code with no runtime overhead when unused
- **Explicit**: Operations require explicit grants, permissions, and error handling
- **Composable**: Types and behaviors work together predictably
- **Portable**: Platform-specific code isolated to dedicated modules with comptime conditionals

#### §17.1.2 Library Organization

[4] The standard library is organized into the following top-level modules:

**Table 17.1 — Standard library modules**

| Module             | Purpose                                    | Specified in |
| ------------------ | ------------------------------------------ | ------------ |
| `std::core`        | Core types and behaviors                   | §17.2        |
| `std::types`       | Built-in modal types (Arena, String, etc.) | §17.3        |
| `std::behaviors`   | Standard behaviors (Copy, Clone, Eq, etc.) | §17.4        |
| `std::collections` | Collections (Vec, Map, Set, etc.)          | §17.5        |
| `std::io`          | Input/output operations                    | §17.6        |
| `std::concurrency` | Threading and synchronization              | §17.7        |
| `std::fs`          | File system operations                     | §17.8        |
| `std::net`         | Network operations                         | §17.9        |
| `std::os`          | Platform-specific interfaces               | §17.10       |
| `std::comptime`    | Compile-time utilities                     | §17.11       |

[5] Each module is further subdivided into logical submodules. The complete module hierarchy is specified in the relevant sections.

#### §17.1.3 Built-In Types vs Standard Library

[6] **Built-in types** are defined by the language specification and have compiler support:

- Primitive types (§7.2): `i32`, `bool`, `f64`, etc.
- `Ptr<T>` modal pointer type (§7.5)
- `Arena` modal type for regions (§11.3.2)
- `Thread<T>` modal type (§14.1.3)
- `witness<B>` modal type (Clause 13)

[7] **Standard library types** are provided as Cursive source code or minimal compiler intrinsics:

- `String` modal type (specified here, referenced in §7.3.4.3)
- Collections: `Vec<T>`, `Map<K, V>`, `Set<T>`
- Synchronization: `Mutex<T>`, `Channel<T>`, `RwLock<T>` (specified here, referenced in §14.4)
- I/O types: `File`, `Reader`, `Writer`

[8] The distinction ensures the core language remains small while the standard library provides practical functionality.

#### §17.1.4 Standard Behaviors

[9] The standard library provides default implementations for common behaviors:

**Marker behaviors** (built-in, specified in §10.4.5):

- `Copy` — Bitwise copyable types
- `Sized` — Statically known size
- `Drop` — Custom destructor

**Derivable behaviors** (standard library):

- `Clone` — Deep copying
- `Eq` — Equality comparison
- `Ord` — Ordering comparison
- `Hash` — Hashing for collections
- `Display` — Human-readable formatting
- `Debug` — Debug formatting
- `Default` — Default value construction

[10] Complete behavior specifications appear in §17.4.

#### §17.1.5 Grant Requirements

[11] Standard library operations require grants as specified in Clause 12. The library uses the built-in grant namespaces and provides clear grant documentation for each procedure.

[12] **Common grant patterns**:

- Collections: `alloc::heap` for growable types
- I/O operations: `io::read`, `io::write`, `fs::read`, `fs::write`
- Networking: `net::connect`, `net::send`, `net::receive`
- Concurrency: `thread::spawn`, `sync::lock`, `sync::atomic`

#### §17.1.6 Contract Specifications

[13] All standard library procedures include contractual sequents documenting:

- Grant requirements
- Preconditions (caller obligations)
- Postconditions (implementation guarantees)
- Invariants for types with state

[14] The library demonstrates best practices for contract-based programming.

#### §17.1.7 Stability Guarantees

[15] Standard library types, behaviors, and procedures are subject to semantic versioning per §1.7:

- **Minor versions** (1.x): May add new items; existing items maintain compatibility
- **Major versions** (2.0+): May introduce breaking changes with migration guides

[16] Types marked as experimental or unstable shall include `[[stability(preview)]]` attributes and require explicit feature flags.

#### §17.1.8 Platform-Specific Modules

[17] Platform-specific functionality resides in the `std::os` module hierarchy:

- `std::os::unix` — Unix-specific APIs
- `std::os::windows` — Windows-specific APIs
- `std::os::macos` — macOS-specific APIs

[18] Conditional compilation using comptime blocks enables portable code that adapts to target platforms.

#### §17.1.9 Prelude Module

[19] The `std::prelude` module contains commonly used items automatically imported into every module:

```cursive
// Automatically available in all modules (no explicit import needed)
use std::prelude::*;
```

[20] Prelude contents include:

- Common types: `String`, `Vec<T>`, `Option<T>`, `Result<T, E>`
- Essential behaviors: `Copy`, `Clone`, `Eq`, `Ord`
- Common procedures: `print()`, `println()`, `panic()`
- Core operators and conversions

[21] Complete prelude specification appears in §17.12.

#### §17.1.10 Conformance Requirements

[22] Conforming implementations shall:

1. Provide all modules and types specified in §§17.2-17.12
2. Implement all standard behaviors per §17.4
3. Support all specified procedures with documented contracts
4. Maintain semantic compatibility across minor versions
5. Document platform-specific availability
6. Provide source code or equivalent functionality for all specified items
7. Include grant requirements in procedure signatures
8. Follow permission and memory model rules for all types

[23] Implementations may provide additional library modules as extensions, clearly marked as non-standard.

---

**Previous**: Clause 16 — Compile-Time Evaluation and Reflection (§16.9 [comptime.conformance]) | **Next**: §17.2 Core Types and Utilities (§17.2 [stdlib.core])
