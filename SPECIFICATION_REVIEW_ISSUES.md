# Specification Review: Issues Found

## Critical Issues to Fix

### 1. Rust-isms in Option/Result Specification

**Problem:** The Option/Result specification (§2.1.7) contains Rust-specific patterns that don't align with Cantrip:

#### Issue 1.1: `impl<T>` blocks
```cantrip
impl<T> Option<T> {  // ❌ WRONG - This is Rust syntax
    procedure is_some(self: Option<T>): bool
```

**Cantrip doesn't use `impl` blocks.** Methods are defined:
- Directly in record/enum definitions, OR
- Via trait attachments

**Fix:** Remove `impl` blocks, specify methods as part of stdlib API (not in type spec)

#### Issue 1.2: Methods like `unwrap()`, `map()`, `and_then()`
These are **Rust standard library** methods, not Cantrip language features.

**Problem:** The spec says "These are library methods, but documented here" - this blurs the line between:
- Language-level type specification (belongs in Part II)
- Standard library API (belongs in Part XII)

**Fix:** Option/Result type specification should focus on:
- Type construction syntax
- Pattern matching (the Cantrip way to use them)
- Memory representation
- Type rules

Library methods belong in Part XII (Standard Library), NOT Part II (Type System).

#### Issue 1.3: `.unwrap()` shown in examples
```cantrip
let valid: char = char::from_u32(65).unwrap()  // ❌ Rust pattern
```

**Cantrip pattern:** Use pattern matching, not method chaining:
```cantrip
// ✓ Cantrip way
match char::from_u32(65) {
    Option.Some(c) => c,
    Option.None => panic("invalid codepoint"),
}
```

### 2. Structure Violations

#### Issue 2.1: Missing formal definitions at section starts

Some consolidated sections don't have **Definition** boxes at the very beginning.

**Required structure:**
```markdown
## X.Y Section Title

**Definition X.Y.N (Name):** Formal mathematical/set-theoretic definition...

### X.Y.1 Overview
...
```

#### Issue 2.2: Inconsistent subsection organization

The spec structure requires:
1. Header & Definition
2. Overview
3. Syntax (Concrete, Abstract, Basic Examples)
4. Static Semantics (Well-formedness, Type Rules, Properties)
5. Dynamic Semantics (Evaluation, Memory, Behavior)
6. Examples & Patterns
7. Cross-References & Notes

Some sections mix these or have different orderings.

### 3. Cantrip vs Rust Distinctions Not Clear

#### Issue 3.1: Permission system differences

Rust uses:
- `&T` (shared reference)
- `&mut T` (mutable reference)  
- Borrow checker with lifetimes

Cantrip uses:
- `T` (default, immutable reference)
- `mut T` (mutable reference)
- `own T` (owned)
- `iso T` (isolated)
- **Regions** for lifetime management (NOT lifetime parameters)
- **No borrow checker** - programmer responsibility

The Option/Result spec mentions "borrow checking" which is **Rust-specific**.

#### Issue 3.2: Method call syntax

The spec uses:
```cantrip
self.is_some()  // Method call
```

But doesn't clarify:
- Is this actually `.is_some($)` with `$` sugar?
- Or is this a standard method call?

**Cantrip uses:** `$.field` and `$.method()` where `$` is the self parameter.

### 4. Missing Cantrip-Specific Integration

#### Issue 4.1: No contract integration shown

Option/Result spec should show integration with Cantrip's **contract system**:

```cantrip
procedure safe_get<T>(arr: [T], index: usize): Option<T>
    will match result {
        Option.Some(v) => index < arr.len() && v == arr[index],
        Option.None => index >= arr.len()
    }
```

This is shown in examples but not formalized in Static Semantics.

#### Issue 4.2: No effect system integration

Result is used with effects (I/O, network, etc.) but this isn't specified:

```cantrip
procedure read_file(path: String): Result<String, Error>
    uses io.read, alloc.heap  // Effect integration
```

Should be in type rules.

#### Issue 4.3: No region integration

Cantrip has **regions** for memory management:

```cantrip
region temp {
    let result: Result<Data, Error> = parse(input)
    // result allocated in region
}
```

This should be mentioned in memory representation.

## Recommendations

### Priority 1: Remove Rust-isms from Option/Result

1. **Remove `impl<T>` blocks** - they don't exist in Cantrip
2. **Remove method specifications** - move to Part XII (stdlib)
3. **Replace `.unwrap()` examples** with pattern matching
4. **Focus on:**
   - Type syntax (enum-based)
   - Pattern matching (primary way to use)
   - Memory representation
   - Type rules and properties

### Priority 2: Fix Structure Adherence

1. **Add Definition boxes** where missing
2. **Reorganize sections** to match the 7-part structure:
   - Header & Definition
   - Overview
   - Syntax
   - Static Semantics
   - Dynamic Semantics
   - Examples & Patterns
   - Cross-References

3. **Use consistent subsection numbering**

### Priority 3: Emphasize Cantrip-Specific Features

1. **Contracts:** Show postconditions with Option/Result
2. **Effects:** Show effect declarations with Result
3. **Permissions:** Show `own`, `mut` with Option/Result
4. **Regions:** Show region allocation if applicable
5. **Pattern matching:** Primary mechanism (not method chaining)

### Priority 4: Clean Up Other Files

Review consolidated files for:
- Rust syntax (`impl`, lifetime parameters `'a`)
- Method specifications that belong in stdlib
- Incorrect Cantrip patterns

## Specific Files to Fix

1. **PART_A_Primitives/01_PrimitiveTypes.md**
   - §2.1.7: Remove `impl` blocks, refocus on type specification
   - §2.1.4: Replace `.unwrap()` with pattern matching
   - §2.1.5: Check for Rust-isms in Never type

2. **PART_B_Composite/02_ProductTypes.md**
   - Check for proper structure adherence
   - Verify Definition boxes present

3. **PART_B_Composite/03_SumTypes.md**
   - Ensure enum spec doesn't reference Rust patterns
   - Check modal/union specs for correctness

## Example: How Option Should Be Specified

```markdown
### 2.1.7.1 Option<T>

**Definition 2.1.7.1 (Option Type):** The `Option<T>` type represents an optional value: either `Some(T)` containing a value of type `T`, or `None` representing absence of a value. Formally: `⟦Option<T>⟧ = T + {None}`.

#### 2.1.7.1.1 Overview

**Key innovation:** Type-safe null safety through explicit optional values.

**When to use:** Values that may be absent, search results, configuration.

**Relationship to other features:**
- **Pattern matching (§X):** Primary way to handle Option values
- **Contracts (Part IV):** Postconditions can specify Option conditions
- **Pointer types (§2.5):** Niche optimization for `Option<Ptr<T>>`

#### 2.1.7.1.2 Syntax

**Concrete syntax:**
```ebnf
OptionType  ::= "Option" "<" Type ">"
OptionValue ::= "Option" "." "Some" "(" Expr ")"
              | "Option" "." "None"
```

**Construction:**
```cantrip
let some: Option<i32> = Option.Some(42)
let none: Option<i32> = Option.None
```

**Pattern matching (primary usage):**
```cantrip
match value {
    Option.Some(x) => process(x),
    Option.None => handle_absence(),
}
```

#### 2.1.7.1.3 Static Semantics

**Type rules:**
[Show type rules WITHOUT method specifications]

**Contract integration:**
[Show how postconditions work with Option]

#### 2.1.7.1.4 Dynamic Semantics

**Memory representation:**
[Show layout with niche optimization]

**Evaluation rules:**
[Show pattern matching evaluation]

#### 2.1.7.1.5 Examples

[Show real Cantrip code, not Rust patterns]
```

## Action Items

- [ ] Fix §2.1.7 Option/Result to remove Rust-isms
- [ ] Move method specs to Part XII notes
- [ ] Add proper Definition boxes
- [ ] Verify structure adherence in all consolidated files
- [ ] Replace `.unwrap()` with pattern matching
- [ ] Add Cantrip-specific integrations (contracts, effects, regions)
- [ ] Review all files for `impl`, lifetime params, Rust stdlib methods
