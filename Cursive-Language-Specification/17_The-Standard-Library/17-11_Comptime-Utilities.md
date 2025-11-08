# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-11_Comptime-Utilities.md  
**Section**: §17.11 Compile-Time Utilities  
**Stable label**: [stdlib.comptime]  
**Forward references**: Clause 16 [comptime]

---

### §17.11 Compile-Time Utilities [stdlib.comptime]

#### §17.11.1 Overview

[1] The `std::comptime` module provides utility procedures for compile-time programming, supplementing the core comptime intrinsics specified in Clause 16.

[2] All procedures in this module execute at compile time and require `comptime::*` grants.

#### §17.11.2 String Manipulation

##### §17.11.2.1 Comptime String Utilities

[3] Additional string operations for code generation:

```cursive
comptime procedure string_join(parts: [string@View], separator: string@View): string@View
    [[ comptime::alloc |- parts.len() > 0 => true ]]
{
    var result: string@View = ""
    var first = true

    loop part: string@View in parts {
        if !first {
            result = string_concat(result, separator)
        }
        result = string_concat(result, part)
        first = false
    }

    result result
}

comptime procedure string_repeat(s: string@View, count: usize): string@View
    [[ comptime::alloc |- count > 0, count <= 1000 => true ]]
{
    var result: string@View = ""
    var i: usize = 0

    loop i < count {
        result = string_concat(result, s)
        i += 1
    }

    result result
}
```

#### §17.11.3 Array Utilities

##### §17.11.3.1 Comptime Array Operations

[4] Compile-time array manipulation:

```cursive
comptime procedure array_map<T, U>(arr: [T], f: (T) -> U): [U]
    [[ comptime::alloc |- arr.len() > 0 => result.len() == arr.len() ]]
{
    var result: [U] = []

    loop item: T in arr {
        let mapped = f(item)
        result = array_push(result, mapped)
    }

    result result
}

comptime procedure array_filter<T>(arr: [T], predicate: (T) -> bool): [T]
    [[ comptime::alloc |- true => true ]]
```

#### §17.11.4 Code Generation Helpers

##### §17.11.4.1 Common Patterns

[5] Helper procedures for frequent code generation patterns:

```cursive
comptime procedure generate_getter(
    type_name: string@View,
    field_name: string@View,
    field_type: string@View
): ()
    [[ comptime::codegen |- true => true ]]
{
    codegen::add_procedure(
        target: TypeRef::Named(type_name),
        spec: ProcedureSpec {
            name: string_concat("get_", field_name),
            visibility: Visibility::Public,
            receiver: ReceiverSpec::Const,
            params: [],
            return_type: TypeRef::Named(field_type),
            sequent: sequent_pure(),
            body: quote {
                result self.$(field_name)
            },
        }
    )
}
```

#### §17.11.5 Conformance Requirements

[6] Implementations shall:

1. Provide string manipulation utilities for comptime
2. Provide array operations for comptime
3. Provide code generation helpers
4. Ensure all utilities use comptime::\* grants only
5. Maintain deterministic evaluation

---

**Previous**: §17.10 Platform-Specific APIs (§17.10 [stdlib.os]) | **Next**: §17.12 Prelude Module (§17.12 [stdlib.prelude])
