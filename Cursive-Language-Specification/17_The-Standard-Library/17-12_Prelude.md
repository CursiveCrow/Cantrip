# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-12_Prelude.md  
**Section**: §17.12 Prelude Module  
**Stable label**: [stdlib.prelude]  
**Forward references**: §§17.2-17.7

---

### §17.12 Prelude Module [stdlib.prelude]

#### §17.12.1 Overview

[1] The prelude module (`std::prelude`) contains items automatically imported into every Cursive module. Users need not write explicit `use` statements for prelude items.

[2] The prelude is deliberately minimal to avoid namespace pollution while providing essential functionality.

#### §17.12.2 Prelude Contents

##### §17.12.2.1 Types

[3] Automatically available types:

```cursive
// From std::core
public use std::core::None;
public use std::core::ParseError;
public use std::core::IoError;
public use std::core::AllocationError;

// From std::types
public use std::types::string;  // Modal type with @View/@Managed

// From std::collections
public use std::collections::List;
public use std::collections::Map;
public use std::collections::Set;
```

##### §17.12.2.2 Behaviors

[4] Automatically available behaviors:

```cursive
// Built-in markers (Clause 10 §10.4.5)
public use std::markers::{Copy, Sized, Drop};

// Standard behaviors (§17.4)
public use std::behaviors::{Clone, Eq, Ord, Hash, Display, Debug, Default};

// Conversion behaviors
public use std::behaviors::{Into, From, AsRef};
```

##### §17.12.2.3 Procedures

[5] Automatically available procedures:

```cursive
// Output (§17.6.2)
public use std::io::{print, println, eprint, eprintln};

// Panicking (§17.2.4)
public use std::core::{panic, assert, debug_assert};

// Utilities
public use std::core::{min, max};
```

##### §17.12.2.4 Error Handling Pattern

[6] The prelude enables immediate error handling:

```cursive
// No imports needed - prelude provides everything:

procedure example(text: string): i32 \/ ParseError \/ None
{
    if text.is_empty() {
        result None { }  // None from prelude
    }

    let parsed = parse_number(text)
    match parsed {
        num: i32 => result num,
        err: ParseError => result err,  // ParseError from prelude
    }
}
```

#### §17.12.3 Prelude Customization

##### §17.12.3.1 Shadowing Prelude Items

[7] Modules may shadow prelude items explicitly:

```cursive
// Shadow prelude's List with custom implementation
use custom::collections::List;

// Now List refers to custom::collections::List, not std::collections::List
```

[8] Explicit imports take precedence over prelude imports.

##### §17.12.3.2 Disabling Prelude

[9] Modules may opt out of the prelude:

```cursive
[[no_prelude]]

// This module must explicitly import everything
use std::io::println;
use std::collections::List;
```

[10] The `[[no_prelude]]` attribute is useful for defining the standard library itself or for bare-metal/embedded contexts.

#### §17.12.4 Conformance Requirements

[11] Implementations shall:

1. Automatically import std::prelude::\* into all modules (unless [[no_prelude]])
2. Include all items specified in §17.12.2
3. Allow shadowing of prelude items by explicit imports
4. Support [[no_prelude]] attribute
5. Document prelude contents clearly

---

**Previous**: §17.11 Compile-Time Utilities (§17.11 [stdlib.comptime]) | **Next**: Annex A — Grammar ([grammar])
