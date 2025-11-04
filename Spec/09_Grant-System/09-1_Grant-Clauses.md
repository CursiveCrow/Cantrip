# Part IX: Grant System
## Section 9.1: Grant Clauses

**File**: `09-1_Grant-Clauses.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [09-0_Grant-System-Overview.md](09-0_Grant-System-Overview.md) | **Next**: [09-2_Grant-Tracking.md](09-2_Grant-Tracking.md)

---

## 9.1.1 Grant Clause Syntax

### 9.1.1.1 Grant Clause Declaration

**Syntax**:

```ebnf
GrantClause ::= "grants" GrantSet
GrantSet    ::= GrantRef ("," GrantRef)* ","?
GrantRef    ::= GrantPath
             | "grants" "(" Ident ")"
             | "_?"
```

**Normative Statement 9.1.1**: A grant clause shall be introduced by the `grants` keyword followed by a grant set.

**Placement**: Grant clauses shall appear in procedure signatures after parameters and return type, before contract clauses.

**Example (informative)**:
```cursive
procedure write_log(message: string)
    grants fs::write
{
    append_to_file("log.txt", message)
}
```

### 9.1.1.2 Empty Grant Set

**Normative Statement 9.1.2**: A procedure with no grant clause has an empty grant set.

**Equivalence**: The following are equivalent:
```cursive
procedure compute(x: i32): i32 {
    result x * 2
}

// Equivalent (explicit empty grant set - discouraged)
procedure compute(x: i32): i32
    grants  /* none */
{
    result x * 2
}
```

**Convention**: Omit the grant clause for procedures requiring no grants.

### 9.1.1.3 Multiple Grants

**Syntax**: Multiple grants are separated by commas.

**Example (informative)**:
```cursive
procedure process_file(input_path: string, output_path: string)
    grants fs::read, fs::write, alloc::heap
{
    let data = read_file_bytes(input_path)
    let buffer = heap_allocate_array<u8>(data.length * 2)
    write_file_bytes(output_path, buffer)
}
```

**Normative Statement 9.1.3**: The order of grants in a grant set is not significant. Duplicate grants shall be rejected as ill-formed.

### 9.1.1.4 Trailing Commas

**Normative Statement 9.1.4**: Implementations may permit an optional trailing comma after the final grant in a grant set.

**Example (informative)**:
```cursive
procedure example()
    grants alloc::heap, fs::write,  // Trailing comma optional
{
    // ...
}
```

---

## 9.1.2 Grant Sets

### 9.1.2.1 Grant Set Definition

**Definition 9.2 (Grant Set)**: A grant set is a finite collection of grants. Grant sets are unordered and contain no duplicates.

**Formal Representation**: Grant sets are represented as sets in the mathematical sense:

```
G = {g₁, g₂, ..., gₙ}
```

Where each `gᵢ` is a grant path.

### 9.1.2.2 Grant Set Operations

**Subset Relation**: Grant set G₁ is a subset of grant set G₂ (written G₁ <: G₂) if every element of G₁ is also an element of G₂.

**Formal Definition**:
```
G₁ <: G₂  ≡  ∀g ∈ G₁ : g ∈ G₂
```

**Union**: The union of grant sets G₁ and G₂ (written G₁ ∪ G₂) contains all grants from both sets.

**Example (informative)**:
```
{fs::read, alloc::heap} ∪ {fs::write, alloc::heap} = {fs::read, fs::write, alloc::heap}
```

### 9.1.2.3 Grant Set Equivalence

**Normative Statement 9.1.5**: Two grant sets are equivalent if and only if they contain exactly the same grants, regardless of order.

**Example (informative)**:
```cursive
// These are equivalent:
grants fs::read, fs::write, alloc::heap
grants alloc::heap, fs::read, fs::write
```

---

## 9.1.3 Grant Paths

### 9.1.3.1 Grant Path Syntax

**Syntax**:
```ebnf
GrantPath ::= Ident ("::" Ident)*
```

**Normative Statement 9.1.6**: A grant path shall consist of one or more identifiers separated by double-colons.

**Hierarchical Structure**: Grant paths form a hierarchical namespace.

**Examples**:
- `alloc::heap` - Two-level path
- `fs::read` - Two-level path
- `panic` - Single-level path

### 9.1.3.2 Grant Path Resolution

**Normative Statement 9.1.7**: Grant paths shall be resolved against both the built-in grant namespace and user-defined grants. Unknown grant paths shall be rejected as ill-formed.

**Built-In Grants**: The set of valid built-in grant paths is defined in §9.4 (Built-In Grants).

**User-Defined Grants**: User-defined grants are specified in §9.5 (User-Defined Grants).

**Example (informative - INVALID)**:
```cursive
// ERROR: Unknown grant path
procedure example()
    grants custom::operation  // ERROR unless custom::operation is defined
{
    // ...
}
```

### 9.1.3.3 Wildcard Grants

**Syntax**: A wildcard grant is a grant path ending in `*`.

**Semantics**: A wildcard grant `category::*` grants all operations in the specified category.

**Example (informative)**:
```cursive
procedure unrestricted_io()
    grants fs::*  // Grants fs::read, fs::write, fs::delete, fs::metadata
{
    // Can perform any file system operation
}
```

**Normative Statement 9.1.8**: A wildcard grant `category::*` shall be equivalent to the union of all specific grants in that category.

**Formal Definition**:
```
category::* ≡ {category::op₁, category::op₂, ..., category::opₙ}
```

Where `category::op₁, ..., category::opₙ` are all built-in grants with prefix `category::`.

---

## 9.1.4 Procedure Grant Requirements

### 9.1.4.1 Grant Declaration

**Normative Statement 9.1.9**: A procedure's grant requirements shall be declared in its signature using a grant clause.

**Scope**: The declared grants are available for the procedure body and all procedures it calls (subject to subset rules).

**Example (informative)**:
```cursive
procedure create_buffer(size: usize): [u8]
    grants alloc::heap
{
    // alloc::heap is available in this body
    result heap_allocate_array<u8>(size)
}
```

### 9.1.4.2 Grant Availability

**Normative Statement 9.1.10**: Within a procedure body, only the grants declared in the procedure's grant clause (and grants available to all procedures) are available.

**No Implicit Grants**: A procedure cannot perform operations requiring grants it has not declared.

**Example (informative - INVALID)**:
```cursive
procedure bad_example()
    grants alloc::heap
{
    // ERROR: fs::write not granted
    write_file("data.txt", [1, 2, 3])  // ERROR
}
```

### 9.1.4.3 Grant Exhaustiveness

**Requirement**: A procedure shall declare all grants required by:
1. Operations performed directly in its body
2. Procedures called from its body (transitive requirement)

**Example (informative)**:
```cursive
procedure helper()
    grants fs::read
{
    result read_file_bytes("config.txt")
}

procedure caller()
    grants fs::read, alloc::heap  // Must include fs::read for helper
{
    let config = helper()        // Requires fs::read
    let buffer = allocate(1024)  // Requires alloc::heap
    result process(config, buffer)
}
```

**Normative Statement 9.1.11**: Implementations shall compute the transitive closure of grant requirements and verify that all required grants are declared.

---

## 9.1.5 Grant Clause Composition

### 9.1.5.1 Multiple Procedures

**Independent Grant Sets**: Each procedure declares its own independent grant set.

**Example (informative)**:
```cursive
procedure reader()
    grants fs::read
{
    result read_file_bytes("input.txt")
}

procedure writer()
    grants fs::write
{
    write_file_bytes("output.txt", [0; 100])
}

procedure both()
    grants fs::read, fs::write
{
    let data = reader()  // OK: fs::read <: {fs::read, fs::write}
    writer()             // OK: fs::write <: {fs::read, fs::write}
}
```

### 9.1.5.2 Override Prohibition

**Normative Statement 9.1.12**: Grant clauses cannot be overridden or shadowed. Each procedure's grant set is determined solely by its signature.

**No Dynamic Grants**: Grant sets are static and determined at compile time.

---

## 9.1.6 Deprecated Syntax

### 9.1.6.1 Uses Clause

**Deprecated Form**: The `uses` keyword is deprecated in favor of `grants`.

**Syntax (deprecated)**:
```cursive
// Deprecated:
procedure old_style()
    uses alloc::heap
{
    // ...
}

// Canonical:
procedure new_style()
    grants alloc::heap
{
    // ...
}
```

**Normative Statement 9.1.13**: Implementations may accept the `uses` keyword for backward compatibility but shall issue a deprecation warning. The `uses` and `grants` keywords are semantically identical.

**Migration**: Replace `uses` with `grants` in all new code.

---

## 9.1.7 Grammar Reference

The complete grammar for grant clauses is specified in Appendix A.7 (Contract Grammar) and Appendix A.9 (Grant Grammar).

**Productions**:
```ebnf
GrantClause ::= "grants" GrantSet
GrantSet    ::= GrantRef ("," GrantRef)* ","?
GrantRef    ::= GrantPath
             | "grants" "(" Ident ")"
             | "_?"
GrantPath   ::= Ident ("." Ident)*
```

**Cross-Reference**: See Appendix A §A.7 and §A.9 for authoritative grammar.

---

## 9.1.8 Well-Formedness Constraints

A grant clause is well-formed if and only if:

1. **Valid Grant Paths**: All grant paths resolve to either built-in grants (§9.4) or user-defined grants (§9.5)
2. **No Duplicates**: The grant set contains no duplicate grants
3. **No Contradictions**: The grant set contains no contradictory grants (e.g., cannot grant and deny the same operation)
4. **Grant Parameters Bound**: Grant parameter references (`grants<G>`) refer to declared grant parameters

**Ill-Formed Examples** (informative):
```cursive
// ERROR: Unknown grant path
procedure bad1()
    grants unknown::operation  // ERROR
{
    // ...
}

// ERROR: Duplicate grant
procedure bad2()
    grants fs::read, fs::read  // ERROR
{
    // ...
}

// ERROR: Unbound grant parameter
procedure bad3()
    grants<G>  // ERROR: G not declared
{
    // ...
}
```

---

## 9.1.9 Interaction with Functions

**Normative Statement 9.1.14**: Functions shall not have grant clauses. Grant clauses are only permitted on procedures.

**Rationale**: Functions are pure and side-effect-free by definition. Grants authorize side effects, which functions cannot perform.

**Example (informative - INVALID)**:
```cursive
// ERROR: Functions cannot have grants
function bad_function(x: i32): i32
    grants alloc::heap  // ERROR
{
    result x * 2
}
```

---

## 9.1.10 Conformance Requirements

A conforming implementation shall:

1. **Parse grant clauses** (§9.1.1): Accept grant clause syntax in procedure signatures
2. **Validate grant paths** (§9.1.3): Reject unknown grant paths
3. **Enforce uniqueness** (§9.1.2): Reject grant sets with duplicate grants
4. **Support wildcards** (§9.1.3.3): Expand wildcard grants to full grant sets
5. **Compute transitive closure** (§9.1.4.3): Verify grant requirements propagate correctly
6. **Reject function grants** (§9.1.9): Diagnose grant clauses on functions as ill-formed

---

**Previous**: [09-0_Grant-System-Overview.md](09-0_Grant-System-Overview.md) | **Next**: [09-2_Grant-Tracking.md](09-2_Grant-Tracking.md)
