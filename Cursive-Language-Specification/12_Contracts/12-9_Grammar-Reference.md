# Cursive Language Specification

## Clause 12 — Contracts

**Clause**: 12 — Contracts
**File**: 12-9_Grammar-Reference.md
**Section**: §12.9 Grammar Reference
**Stable label**: [contract.grammar]  
**Forward references**: Annex A §A.7 [grammar.sequent], Annex A §A.8 [grammar.assertion], Annex A §A.9 [grammar.grant]

---

### §12.9 Grammar Reference [contract.grammar]

#### §12.9.1 Overview

[1] This subclause consolidates the complete grammar for contractual sequents, grant specifications, predicate expressions, and contract declarations. The authoritative grammar appears in Annex A; this section provides a readable summary with cross-references.

[2] All grammar productions use EBNF notation as defined in §1.4.1 [intro.notation.grammar]. For the authoritative ANTLR-style grammar suitable for parser generation, see Annex A §A.7 [grammar.sequent].

#### §12.9.2 Sequent Grammar Summary [contract.grammar.sequent]

[3] Complete contractual sequent grammar (simplified notation):

```ebnf
SequentClause ::= "[[" SequentSpec "]]"

SequentSpec ::= GrantClause "|-" MustClause "=>" WillClause
             | GrantClause "|-" MustClause
             | GrantClause "|-" "=>" WillClause
             | GrantClause "|-"
             | "|-" MustClause "=>" WillClause
             | "|-" MustClause
             | "|-" "=>" WillClause
             | MustClause "=>" WillClause
             | GrantClause

GrantClause ::= GrantReference ("," GrantReference)*
             | ε

MustClause ::= PredicateExpression
            | PredicateList
            | "true"

WillClause ::= PredicateExpression
            | PredicateList
            | "true"

PredicateList ::= PredicateExpression ("," PredicateExpression)*

PredicateExpression ::= Expression
```

[ Note: See Annex A §A.7 [grammar.sequent] for complete authoritative grammar.
— end note ]

#### §12.9.3 Grant Grammar Summary [contract.grammar.grant]

[4] Grant reference grammar (simplified notation):

```ebnf
GrantReference ::= QualifiedGrantPath
                | GrantParameter

QualifiedGrantPath ::= Identifier ("::" Identifier)*

GrantParameter ::= Identifier
```

[5] Built-in grant namespaces (§12.3.3):

- `alloc::` — Allocation grants (heap, region, global)
- `fs::` — File system grants (read, write, delete, metadata, create)
- `net::` — Network grants (connect, listen, send, receive, dns)
- `io::` — I/O grants (read, write)
- `thread::` — Threading grants (spawn, join, sleep)
- `sync::` — Synchronization grants (atomic, lock)
- `sys::` — System grants (env, time, exit)
- `unsafe::` — Unsafe operation grants (ptr, transmute, asm)
- `ffi::` — FFI grants (call)
- `panic` — Top-level panic grant
- `comptime::` — Compile-time grants (alloc, codegen, config, diag)

[ Note: See Annex A §A.9 [grammar.grant] for complete grant grammar.
— end note ]

#### §12.9.4 Predicate Expression Grammar Summary [contract.grammar.predicate]

[6] Predicate expressions in must and will clauses (simplified notation):

```ebnf
PredicateExpression ::= LogicalExpression

LogicalExpression ::= ComparisonExpression
                   | LogicalExpression "&&" LogicalExpression
                   | LogicalExpression "||" LogicalExpression
                   | "!" LogicalExpression
                   | "(" LogicalExpression ")"

ComparisonExpression ::= Expression CompOp Expression
                      | "true"
                      | "false"
                      | ResultExpression
                      | OldExpression
                      | QuantifiedExpression

CompOp ::= "==" | "!=" | "<" | "<=" | ">" | ">="

ResultExpression ::= "result" (FieldAccess | IndexAccess)*

OldExpression ::= "@old" "(" Expression ")"

QuantifiedExpression ::= "forall" "(" Pattern "in" Expression ")" "{" PredicateExpression "}"
                      | "exists" "(" Pattern "in" Expression ")" "{" PredicateExpression "}"
```

[7] Quantifiers (`forall`, `exists`) are optional extensions for formal verification. Implementations without verification support may reject quantified predicates with diagnostic E12-090.

[ Note: See Annex A §A.8 [grammar.assertion] for complete predicate grammar.
— end note ]

#### §12.9.5 Contract Declaration Grammar Summary [contract.grammar.contract]

[8] Contract declaration grammar (simplified notation):

```ebnf
ContractDecl ::= Visibility? "contract" Identifier GenericParams? ExtendsClause? ContractBody

ExtendsClause ::= ":" ContractReference ("," ContractReference)*

ContractBody ::= "{" ContractItem* "}"

ContractItem ::= AssociatedTypeRequirement
              | ProcedureSignature

AssociatedTypeRequirement ::= "type" Identifier (":" BehaviorBounds)?

ProcedureSignature ::= "procedure" Identifier "(" ParamList? ")" (":" Type)? SequentClause?
```

[9] Contract procedures have signatures but no bodies. Including a body produces diagnostic E12-091 (contract procedures shall not have bodies).

[ Note: See Annex A §A.6 [grammar.declaration] for complete contract grammar.
— end note ]

#### §12.9.6 Invariant Grammar Summary [contract.grammar.invariant]

[10] Invariant clause grammar (simplified notation):

```ebnf
WhereClause ::= "where" "{" InvariantList "}"

InvariantList ::= InvariantExpression ("," InvariantExpression)*

InvariantExpression ::= PredicateExpression
```

[11] Invariant clauses appear on:

- Type declarations (records, enums, modals)
- Loop statements
- (Future: other scoping constructs)

#### §12.9.7 Complete Example [contract.grammar.example]

**Example 12.9.7.1 (All grammar elements)**:

```cursive
// Grant declaration
public grant database_modify

// Contract with sequents
public contract Repository<T: Serializable> {
    type Storage = string@View

    procedure save(~, item: T): Self::Storage \/ Error
        [[ database_modify, alloc::heap |- true => true ]]

    procedure load(data: Self::Storage): T \/ Error
        [[ alloc::heap |- data.len() > 0 => true ]]
}

// Type with invariant implementing contract
record UserRepository: Repository<User> {
    data: [User],

    where {
        data.len() <= 10_000
    }

    type Storage = string@Managed

    procedure save(~, item: User): string@Managed \/ Error
        [[
            database_modify,
            alloc::heap
            |-
            item.id > 0
            =>
            match result {
                storage: string@Managed => storage.len() > 0,
                _: Error => true
            }
        ]]
    {
        // Implementation
    }

    procedure load(data: string@Managed): User \/ Error
        [[ alloc::heap |- data.len() > 0 => true ]]
    {
        // Implementation
    }
}

// Procedure using all features
[[verify(static)]]
procedure process_users(repo: unique UserRepository, count: usize)
    [[
        database_modify,
        alloc::heap
        |-
        count > 0,
        count <= 100
        =>
        repo.data.len() >= @old(repo.data.len())
    ]]
    where count <= 100  // Type invariant ensures this
{
    var i: usize = 0
    loop i < count where { i >= 0, i <= count } {
        let user = create_user(i)
        match repo.save(user) {
            _: string@Managed => { },
            err: Error => { panic("Save failed") }
        }
        i = i + 1
    }
}
```

#### §12.9.8 Conformance Requirements [contract.grammar.requirements]

[12] Implementations shall:

1. Parse sequent clauses using semantic brackets `⟦ ⟧` or `[[ ]]`
2. Support all smart defaulting forms listed in §12.2.2.2
3. Parse grant clauses with qualified paths and grant parameters
4. Parse predicate expressions with logical operators, comparisons, result, @old
5. Support quantifiers (forall, exists) if verification enabled
6. Parse contract declarations with procedure signatures (no bodies)
7. Parse where clauses on types and loops
8. Integrate grammar with Annex A authoritative productions
9. Emit syntax diagnostics for malformed sequents, grants, or predicates
10. Maintain AST representation of contracts for type checking and verification

---

**Previous**: §12.8 Verification Modes (§12.8 [contract.verification]) | **Next**: §12.10 Conformance and Diagnostics (§12.10 [contract.diagnostics])
