# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-4_Standard-Behaviors.md  
**Section**: §17.4 Standard Behaviors  
**Stable label**: [stdlib.behaviors]  
**Forward references**: Clause 10 §10.4 [generic.behavior], Clause 11 [memory]

---

### §17.4 Standard Behaviors [stdlib.behaviors]

#### §17.4.1 Overview

[1] This section specifies standard behaviors provided by the library. These behaviors extend the built-in marker behaviors (Copy, Sized, Drop from §10.4.5) with commonly needed functionality.

[2] All behaviors specified here follow Clause 10 requirements: **all procedures must have concrete bodies** (behaviors provide code, not abstractions).

#### §17.4.2 Clone Behavior

##### §17.4.2.1 Definition

[3] The `Clone` behavior provides explicit deep copying for types that cannot use bitwise Copy:

```cursive
public behavior Clone {
    procedure clone(~): Self
        [[ alloc::heap |- true => true ]]
    {
        panic("Clone not implemented for this type")
    }
}
```

[4] Types implement Clone for complex resources requiring allocation or deep copying.

**Example 17.4.2.1** (Clone implementation):

```cursive
record Buffer {
    data: [u8],
    length: usize,
}

behavior Clone for Buffer {
    procedure clone(~): Self
        [[ alloc::heap |- true => result.length == self.length ]]
    {
        let new_data = allocate_array(self.length)
        copy_memory(new_data, self.data, self.length)
        result Buffer { data: new_data, length: self.length }
    }
}
```

#### §17.4.3 Eq Behavior

##### §17.4.3.1 Definition

[5] Equality comparison behavior:

```cursive
public behavior Eq {
    procedure eq(~, other: Self): bool
    {
        panic("Eq not implemented")
    }

    procedure ne(~, other: Self): bool
    {
        result !self.eq(other)
    }
}
```

[6] Primitive types automatically satisfy Eq. Composite types derive Eq when all fields satisfy Eq.

**Example 17.4.3.1** (Eq for record):

```cursive
record Point {
    x: f64,
    y: f64,
}

behavior Eq for Point {
    procedure eq(~, other: Self): bool
    {
        result self.x == other.x && self.y == other.y
    }
}
```

#### §17.4.4 Ord Behavior

##### §17.4.4.1 Definition

[7] Ordering comparison behavior:

```cursive
public behavior Ord with Eq {
    procedure compare(~, other: Self): i32
    {
        panic("Ord not implemented")
    }

    procedure lt(~, other: Self): bool
    {
        result self.compare(other) < 0
    }

    procedure le(~, other: Self): bool
    {
        result self.compare(other) <= 0
    }

    procedure gt(~, other: Self): bool
    {
        result self.compare(other) > 0
    }

    procedure ge(~, other: Self): bool
    {
        result self.compare(other) >= 0
    }
}
```

[8] The `compare` procedure returns:

- Negative value if `self < other`
- Zero if `self == other`
- Positive value if `self > other`

#### §17.4.5 Hash Behavior

##### §17.4.5.1 Definition

[9] Hashing behavior for hash-based collections:

```cursive
public behavior Hash {
    procedure hash(~): u64
    {
        panic("Hash not implemented")
    }
}
```

[10] Hash implementations should provide good distribution and consistency (equal values produce equal hashes).

#### §17.4.6 Display Behavior

##### §17.4.6.1 Definition

[11] Human-readable string representation:

```cursive
public behavior Display {
    procedure show(~): string@View
    {
        result type_name::<Self>()
    }

    procedure to_string(~): string@Managed
        [[ alloc::heap |- true => true ]]
    {
        result self.show().to_managed()
    }
}
```

[12] Default implementation uses type name. Types override for meaningful output.

#### §17.4.7 Debug Behavior

##### §17.4.7.1 Definition

[13] Debug-oriented string representation:

```cursive
public behavior Debug {
    procedure debug(~): string@Managed
        [[ alloc::heap |- true => true ]]
    {
        result type_name::<Self>()
    }
}
```

[14] Debug provides detailed output suitable for logging and diagnostics.

#### §17.4.8 Default Behavior

##### §17.4.8.1 Definition

[15] Default value construction:

```cursive
public behavior Default {
    procedure default(): Self
        [[ alloc::heap |- true => true ]]
    {
        panic("Default not implemented")
    }
}
```

[16] Primitive numeric types implement Default with zero values. Other types provide appropriate defaults.

**Example 17.4.8.1** (Default for numeric types):

```cursive
behavior Default for i32 {
    procedure default(): Self
    {
        result 0
    }
}

behavior Default for bool {
    procedure default(): Self
    {
        result false
    }
}
```

#### §17.4.9 Conformance Requirements

[17] Implementations shall:

1. Provide all behaviors specified in §§17.4.2-17.4.8
2. Ensure all behavior procedures have concrete bodies
3. Implement default behaviors for primitive types
4. Support blanket implementations where specified
5. Maintain consistency with Clause 10 behavior requirements

---

**Previous**: §17.3 Built-In Modal Types (§17.3 [stdlib.types]) | **Next**: §17.5 Collections (§17.5 [stdlib.collections])
