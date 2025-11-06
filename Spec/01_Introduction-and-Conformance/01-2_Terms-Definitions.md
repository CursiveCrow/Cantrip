# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.2: Terms and Definitions

**File**: `01-2_Terms-Definitions.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-1_Normative-References.md) | **Next**: [[REF_TBD]](01-3_Notation-Conventions.md)

---

For the purposes of this specification, the following terms and definitions apply. Additional domain-specific terms are defined in their respective sections.

## 1.2.1 General Terms

### binding
association between an identifier and a value, type, or other entity within a scope

**Example**: In `let x = 42;`, the binding associates identifier `x` with value `42`.

### compilation unit
source file or set of source files processed together to produce an output

### conforming implementation
implementation that satisfies all requirements of this specification (see [REF_TBD]

### conforming program
well-formed program that uses only features defined in this specification (see [REF_TBD]

### declaration
program construct that introduces a name and associates it with an entity (variable, function, type, etc.)

### definition
declaration that fully specifies an entity

**Note**: In Cursive, most declarations are also definitions. Forward declarations are limited to type names and contracts (see [REF_TBD], [REF_TBD]

### diagnostic
message produced by an implementation to inform about violations of language rules

**Note**: Diagnostics include errors (violations requiring rejection) and warnings (informative messages).

### entity
value, object, function, procedure, type, module, or other language construct that can be named

### evaluation
process of computing the value of an expression

### execution
process of carrying out the operations specified by a program

### expression
syntactic construct that computes a value and may have side effects

### identifier
sequence of characters used to name an entity

### implementation
compiler, interpreter, or other system that processes Cursive programs

### program
one or more compilation units constituting a complete application

### scope
region of program text within which a binding is valid

### statement
syntactic construct that performs an action but does not directly produce a value

**Note**: In Cursive, many control flow constructs are expressions (e.g., `if`, `match`, `loop`) rather than statements.

## 1.2.2 Language-Specific Terms

### callable
**Generic term** for any function or procedure (collectively, callables are invocable declarations with parameters and return types)

**Note**: Use this term when referring to functions and procedures without distinction. See Part IX.

### contract
abstract behavioral specification with procedure signatures and clauses (`grants`, `must`, `will`)

**Cross-reference**: [REF_TBD]

### contract clause
one of `grants`, `must`, `will` attached to a callable signature

### grant
capability token representing a side effect or resource access (e.g., `fs::write`, `alloc::heap`)

### grant set
finite set of grants required or provided by an expression or callable

**Cross-reference**: Part IX (Grant System)

### function
callable declared without a `self` parameter

Functions may be declared at module scope or as associated functions within a type's scope (invoked via `::`). Functions cannot directly access instance fields. Declared with the `function` keyword.

**Cross-reference**: [REF_TBD]

**Example**:
```cursive
function add(x: i32, y: i32): i32 {
    result x + y
}
```

### modal state
value of a modal type annotated with a state indicator

**Cross-reference**: [REF_TBD]

**Example**: For modal type `FileHandle`, values can be in states like `FileHandle@Open` or `FileHandle@Closed`.

### modal type
type with compile-time state machine semantics

**Cross-reference**: [REF_TBD]

### binding category
classification of variable bindings determining cleanup responsibility and rebindability

Cursive defines two binding categories (`let` and `var`) combined with two assignment operators (`=` for owning, `<-` for reference of value), creating four binding forms:

- `let x = ...`: non-rebindable owning binding, transferable via `move`
- `var x = ...`: rebindable owning binding, cannot transfer ownership
- `let x <- ...`: non-rebindable non-owning binding (reference of value)
- `var x <- ...`: rebindable non-owning binding (reference of value)

Non-owning bindings (using `<-`) do not call destructors and cannot be transferred.

**Note**: Mutability is determined by permissions (`const`, `unique`, `shared`), not by binding category. A `let` binding can be immutable (with `const` permission) or mutable (with `unique` or `shared` permission).

**Cross-reference**: [REF_TBD]

**Note**: Binding categories are orthogonal to permissions in Cursive's two-axis memory model.

### permission
annotation (`const`, `unique`, `shared`) controlling memory access and aliasing constraints

Cursive's permission system operates on the second axis of the memory model, orthogonal to binding categories. Permissions determine what operations are allowed on a value and what aliasing constraints apply.

**Cross-reference**: [REF_TBD]

**Note**: The three permissions form a lattice with `const` at the bottom: both `unique` and `shared` can coerce to `const`, but `unique` and `shared` cannot coerce to each other. Coercion is always weakening (unique→const, shared→const), never strengthening.

### pipeline stage
expression of the form `expr => stage: Type` chaining transformations

**Cross-reference**: [REF_TBD]

### procedure
callable with an explicit `self` parameter that operates on a receiver value

Procedures are invoked using the scope operator `::` (e.g., `value::method(args)`). Procedures can access instance fields through `self`. Declared with the `procedure` keyword.

**Cross-reference**: [REF_TBD]

**Example**:
```cursive
type Point = record {
    x: f64,
    y: f64,
};

procedure distance(self: const Point): f64 {
    result (self.x * self.x + self.y * self.y).sqrt()
}
```

### region
lexically-scoped allocation arena released in LIFO order

**Cross-reference**: [REF_TBD]

### predicate
collection of procedure signatures with default implementations for code reuse

All procedures in a predicate MUST have bodies. Distinct from contracts (which have no bodies).

**Cross-reference**: [REF_TBD]

**Note**: Predicates enable mixin-style composition patterns. In code, the keyword `predicate` is used to declare predicates.

### witness
runtime evidence of contract satisfaction

**Cross-reference**: [REF_TBD]

## 1.2.3 Technical Terms

### abstract syntax tree (AST)
tree representation of the syntactic structure of a program

**Note**: AST structure is informative; implementations may use alternative internal representations. See Annex D.1.

### aliasing
situation where multiple references or pointers refer to the same memory location

**Cross-reference**: [REF_TBD]

### coercion
implicit type conversion applied at specific program points

**Cross-reference**: [REF_TBD]

### elaboration
process of resolving holes and inserting inferred information into the program

**Cross-reference**: [REF_TBD]

### hole
placeholder for inferred information (type, permission, grant, or modal state)

**Syntax**: `_?` (type/permission hole), `@_?` (modal state hole)

**Cross-reference**: [REF_TBD]

### ill-formed program
program that violates a static semantic rule requiring a diagnostic

**Cross-reference**: [REF_TBD]

### implementation-defined behavior
behavior that varies between conforming implementations and must be documented

**Cross-reference**: [REF_TBD]

**Example**: Size of integer types beyond minimum requirements.

### invariant
condition that must hold at specific program points

**Cross-reference**: [REF_TBD]

### lvalue
expression that designates a memory location and can appear on the left-hand side of an assignment

**Cross-reference**: [REF_TBD]

### monomorphization
process of generating specialized code for each instantiation of a generic function or type

**Cross-reference**: [REF_TBD]

### place
expression that refers to a memory location

**Cross-reference**: [REF_TBD]

### postcondition
condition that must hold after execution of a callable

**Keyword**: `will`

**Cross-reference**: [REF_TBD]

### precondition
condition that must hold before execution of a callable

**Keyword**: `must`

**Cross-reference**: [REF_TBD]

### rvalue
expression that computes a value but does not designate a memory location

**Cross-reference**: [REF_TBD]

### static semantics
compile-time rules governing well-formedness, types, and other properties

### subtyping
relationship where values of one type can be used where another type is expected

**Cross-reference**: [REF_TBD]

### type environment
mapping from identifiers to their types, used in typing rules

**Notation**: Γ (Greek letter Gamma)

**Cross-reference**: [REF_TBD]

### undefined behavior
behavior for which this specification imposes no requirements

**Abbreviation**: UB

**Cross-reference**: [REF_TBD], Annex B.2

**Warning**: Programs with undefined behavior are invalid, even if they appear to execute successfully.

### unspecified behavior
behavior that may vary between executions or implementations, but is limited to a set of valid choices

**Cross-reference**: [REF_TBD]

### well-formed program
program that satisfies all syntactic and static semantic rules

**Note**: Well-formed programs may still exhibit undefined behavior at runtime if they violate dynamic constraints.

## 1.2.4 Symbols and Abbreviated Terms

### ABI
Application Binary Interface

**Cross-reference**: [REF_TBD]

### EBNF
Extended Backus-Naur Form (grammar notation)

**Cross-reference**: [REF_TBD]

### FFI
Foreign Function Interface

**Cross-reference**: [REF_TBD]

### IFNDR
Ill-Formed, No Diagnostic Required

**Cross-reference**: [REF_TBD]

### LPS
Lexical Permission System

**Cross-reference**: [REF_TBD]

### NFC, NFD, NFKC, NFKD
Unicode Normalization Forms (Canonical/Compatibility, Composed/Decomposed)

**Cross-reference**: [REF_TBD]

### RAII
Resource Acquisition Is Initialization

**Cross-reference**: [REF_TBD]

**Note**: In Cursive, RAII is achieved through region cleanup and drop semantics.

### UB
Undefined Behavior

**Cross-reference**: [REF_TBD], Annex B.2

### UTF-8
Unicode Transformation Format, 8-bit

**Cross-reference**: [REF_TBD]

---

## Index of Defined Terms

For convenience, a complete alphabetical index of all technical terms is provided in **Annex F: Glossary**.

---

**Previous**: [[REF_TBD]](01-1_Normative-References.md) | **Next**: [[REF_TBD]](01-3_Notation-Conventions.md)
