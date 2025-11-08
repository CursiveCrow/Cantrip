# Cursive Language Specification

## Clause 12 — Contracts

**Clause**: 12 — Contracts
**File**: 12-10_Conformance-Diagnostics.md
**Section**: §12.10 Conformance and Diagnostics
**Stable label**: [contract.diagnostics]  
**Forward references**: §12.1-12.9 (all contract subclauses), Annex E §E.5 [implementation.diagnostics]

---

### §12.10 Conformance and Diagnostics [contract.diagnostics]

#### §12.10.1 Overview

[1] This subclause consolidates conformance requirements for the contract system, provides the complete diagnostic code catalog for Clause 12, and specifies integration requirements with other language subsystems.

[2] Conforming implementations shall satisfy all requirements enumerated in §§12.1-12.9 and shall emit the diagnostics defined in this subclause when contract violations occur.

#### §12.10.2 Consolidated Conformance Requirements [contract.diagnostics.conformance]

[3] A conforming implementation of the contract system shall:

**Sequent Support** (§12.2):

1. Parse contractual sequents with semantic brackets `⟦ ⟧` or `[[ ]]`
2. Support complete sequent form: `[[ grants |- must => will ]]`
3. Apply smart defaulting rules for abbreviated forms
4. Recognize turnstile `⊢`/`|-` and implication `⇒`/`=>`
5. Desugar abbreviated sequents deterministically to canonical form

**Grant System** (§12.3): 6. Provide all built-in grants enumerated in §12.3.3 7. Support user-defined grants declared via §5.9 8. Enforce grant visibility rules (public/internal/private) 9. Perform grant checking at call sites using subset inclusion 10. Reject wildcard grant syntax; require explicit grant lists 11. Support grant parameters with substitution and bounds

**Preconditions** (§12.4): 12. Parse must clauses as boolean predicates 13. Enforce caller obligations at call sites 14. Perform static verification when provable 15. Insert dynamic checks when verification mode requires 16. Support flow-sensitive precondition proof

**Postconditions** (§12.5): 17. Parse will clauses with `result` and `@old` support 18. Implement `@old(expression)` operator capturing pre-state 19. Verify postconditions on all non-diverging return paths 20. Insert dynamic checks per verification mode 21. Support multiple independent @old captures

**Invariants** (§12.6): 22. Parse `where` clauses on types and loops 23. Desugar type invariants to postcondition conjunctions 24. Desugar loop invariants to verification points 25. Automatically check invariants at construction and mutation 26. Maintain unified `where` terminology across all contexts

**Checking Flow** (§12.7): 27. Implement grant availability checking algorithm 28. Implement precondition obligation verification 29. Implement postcondition guarantee checking 30. Support compositional verification using callee contracts 31. Integrate with definite assignment analysis

**Verification Modes** (§12.8): 32. Support static, dynamic, trusted modes via attributes 33. Provide build-mode-dependent defaults 34. Enforce static verification requirements (prove or reject) 35. Insert runtime checks in dynamic mode 36. Skip checks in trusted mode 37. Generate witness hooks for dynamic verification

**Grammar** (§12.9): 38. Implement sequent, grant, and predicate grammars per Annex A 39. Support contract declarations with no procedure bodies 40. Integrate contracts with behavior system (Clause 10)

#### §12.10.3 Complete Diagnostic Catalog [contract.diagnostics.catalog]

[4] This section enumerates all diagnostic codes defined in Clause 12. Each diagnostic follows the canonical format `E12-[NNN]` as specified in §1.6.6 [intro.document.diagnostics].

##### §12.10.3.1 Sequent Syntax Diagnostics (E12-001 through E12-010)

| Code    | Description                               | Section    |
| ------- | ----------------------------------------- | ---------- |
| E12-001 | Invalid sequent brackets (not [[]])       | §12.2.3[1] |
| E12-002 | Missing or duplicate turnstile            | §12.2.3[2] |
| E12-003 | Missing or duplicate implication operator | §12.2.3[3] |
| E12-004 | Sequent components in wrong order         | §12.2.3[4] |
| E12-005 | Effectful expression in must/will clause  | §12.2.3[5] |
| E12-006 | Undefined grant in grant clause           | §12.2.3[6] |
| E12-007 | result identifier used in must clause     | §12.2.3[7] |
| E12-008 | @old operator used in must clause         | §12.2.3[8] |
| E12-009 | Nested @old operators                     | §12.2.3[8] |
| E12-010 | Ambiguous abbreviated sequent             | §12.2.3[9] |

##### §12.10.3.2 Grant System Diagnostics (E12-020 through E12-032)

| Code    | Description                              | Section      |
| ------- | ---------------------------------------- | ------------ |
| E12-020 | Comptime grant used in runtime procedure | §12.3.3.9[9] |
| E12-030 | Call site missing required grants        | §12.3.8[21]  |
| E12-031 | Grant visibility violation (non-public)  | §12.3.9[1]   |
| E12-032 | Grant set exceeds implementation limit   | §12.3.9[3]   |

##### §12.10.3.3 Precondition Diagnostics (E12-040 through E12-044)

| Code    | Description                                        | Section     |
| ------- | -------------------------------------------------- | ----------- |
| E12-040 | Precondition expression has non-bool type          | §12.4.3[1]  |
| E12-041 | Precondition performs side effects (not pure)      | §12.4.3[2]  |
| E12-042 | Precondition references local binding (not param)  | §12.4.3[3]  |
| E12-043 | Contract implementation has stronger precondition  | §12.4.6[20] |
| E12-044 | Precondition cannot be evaluated (ill-formed expr) | §12.4.3[4]  |

##### §12.10.3.4 Postcondition Diagnostics (E12-050 through E12-056)

| Code    | Description                                          | Section       |
| ------- | ---------------------------------------------------- | ------------- |
| E12-050 | Effectful expression in @old(...)                    | §12.5.4.1[13] |
| E12-051 | @old references local binding (not parameter)        | §12.5.4.1[14] |
| E12-052 | Contract implementation has weaker postcondition     | §12.5.6[26]   |
| E12-053 | Postcondition expression has non-bool type           | §12.5.7[1]    |
| E12-054 | Postcondition performs side effects (not pure)       | §12.5.7[2]    |
| E12-055 | Postcondition references local binding               | §12.5.7[3]    |
| E12-056 | Postcondition unprovable in static verification mode | §12.8.4[15]   |

##### §12.10.3.5 Invariant Diagnostics (E12-060 through E12-069)

| Code    | Description                                  | Section       |
| ------- | -------------------------------------------- | ------------- |
| E12-060 | Invariant expression has non-bool type       | §12.6.9[1]    |
| E12-061 | Invariant performs side effects (not pure)   | §12.6.9[2]    |
| E12-062 | Invariant references inaccessible binding    | §12.6.9[3]    |
| E12-063 | @old operator used in where clause           | §12.6.9[4]    |
| E12-064 | result identifier used in type invariant     | §12.6.9[5]    |
| E12-065 | Unsatisfiable type invariant (uninhabitable) | §12.6.9[6]    |
| E12-066 | Invariant violated at construction           | §12.6.2.4[10] |
| E12-067 | Invariant violated after mutation            | §12.6.2.4[10] |
| E12-068 | Loop invariant violated at entry             | §12.6.3.3[15] |
| E12-069 | Loop invariant not preserved by iteration    | §12.6.3.3[15] |

##### §12.10.3.6 Checking and Verification Diagnostics (E12-070 through E12-091)

| Code    | Description                                              | Section     |
| ------- | -------------------------------------------------------- | ----------- |
| E12-070 | Grant accumulation exceeds declared grants               | §12.7.7[20] |
| E12-080 | verify attribute misplaced (not before procedure)        | §12.8.10[1] |
| E12-081 | Invalid verification mode (not static/dynamic/trusted)   | §12.8.10[2] |
| E12-090 | Quantifier (forall/exists) unsupported by implementation | §12.9.4[7]  |
| E12-091 | Contract procedure has body (must be abstract)           | §12.9.5[9]  |

#### §12.10.4 Diagnostic Payload Requirements [contract.diagnostics.payloads]

[5] Contract diagnostics shall follow Annex E §E.5 structured payload format. Required fields for each diagnostic family:

**Grant diagnostics** (E12-006, E12-020, E12-030, E12-031):

- Grant identifier (qualified path)
- Required grant set
- Available grant set
- Missing grants (for E12-030)
- Source location

**Precondition diagnostics** (E12-040 through E12-044):

- Precondition expression (source text)
- Substituted arguments
- Violated condition (for runtime failures)
- Call site location
- Procedure definition location

**Postcondition diagnostics** (E12-050 through E12-056):

- Postcondition expression (source text)
- result value (for runtime failures)
- @old captured values (if applicable)
- Return point location
- Procedure definition location

**Invariant diagnostics** (E12-060 through E12-069):

- Invariant expression (source text)
- Violated field or state
- Construction/mutation site
- Type definition location

#### §12.10.5 Integration Summary [contract.diagnostics.integration]

[6] The contract system integrates with the following language subsystems:

**Declarations (Clause 5)**:

- Contractual sequents attach to procedures (§5.4)
- Grant declarations introduce capability tokens (§5.9)
- Contract declarations are type-like bindings

**Type System (Clause 7)**:

- Callable types include grant sets and sequents (§7.4)
- Contract bounds on generic parameters (§7.x)
- Subtyping respects sequent variance

**Expressions (Clause 8)**:

- Grant accumulation through expressions (§8.1.5)
- Predicate expressions type-checked as bool
- result and @old special identifiers

**Generics (Clause 10)**:

- Grant parameters for grant polymorphism (§10.2.5)
- Grant bounds in where clauses (§10.3.7)
- Contract vs behavior distinction (§10.4.1.2)

**Memory Model (Clause 11)**:

- Grant tracking for allocation operations
- Integration with RAII and cleanup
- Permissions orthogonal to contracts

**Witness System (Clause 13)**:

- Dynamic verification generates witnesses
- Polymorphic dispatch via contract witnesses
- Runtime evidence of contract satisfaction

#### §12.10.6 Complete Example [contract.diagnostics.example]

**Example 12.10.6.1 (Comprehensive contract usage)**:

```cursive
// User-defined grants
public grant ledger_read
public grant ledger_write

// Contract declaration
public contract Auditable {
    type AuditLog = string@View

    procedure audit_read(~): Self::AuditLog
        [[ ledger_read |- true => result.len() > 0 ]]

    procedure audit_write(~, entry: Self::AuditLog)
        [[ ledger_write |- entry.len() > 0 => true ]]
}

// Type with invariant implementing contract
record BankAccount: Auditable {
    balance: i64,
    transactions: [Transaction],

    where {
        balance >= 0,
        transactions.len() <= 10_000
    }

    type AuditLog = string@Managed

    procedure audit_read(~): string@Managed
        [[ ledger_read, alloc::heap |- true => result.len() > 0 ]]
    {
        result string.from("audit log")
    }

    procedure audit_write(~, entry: string@Managed)
        [[ ledger_write |- entry.len() > 0 => true ]]
    {
        // Log to audit system
    }

    [[verify(static)]]
    procedure deposit(~!, amount: i64)
        [[
            ledger_write
            |-
            amount > 0
            =>
            self.balance == @old(self.balance) + amount,
            self.balance >= amount
        ]]
    {
        self.balance += amount
        self.audit_write(string.from("deposit"))
        // Invariant automatically checked: balance >= 0, transactions.len() <= 10_000
    }

    [[verify(dynamic)]]
    procedure withdraw(~!, amount: i64): () \/ InsufficientFunds
        [[
            ledger_write
            |-
            amount > 0
            =>
            match result {
                _: () => self.balance == @old(self.balance) - amount,
                _: InsufficientFunds => self.balance == @old(self.balance)
            }
        ]]
    {
        if self.balance >= amount {
            self.balance -= amount
            self.audit_write(string.from("withdrawal"))
            result ()
        } else {
            result InsufficientFunds { requested: amount, available: self.balance }
        }
    }
}

// Grant-polymorphic procedure using contract bound
procedure process_auditable<T, ε>(item: unique T)
    [[
        ε,
        alloc::heap
        |-
        true
        =>
        true
    ]]
    where T: Auditable,
          ε ⊆ {ledger_read, ledger_write}
{
    let log = item.audit_read()
    println("Audit: {}", log)
}
```

#### §12.10.7 Diagnostic Code Summary [contract.diagnostics.summary]

[4] Clause 12 defines diagnostic codes E12-001 through E12-091 covering:

- **Syntax** (001-010): Sequent structure and formatting
- **Grants** (020-032): Grant availability and visibility
- **Preconditions** (040-044): Must clause requirements
- **Postconditions** (050-056): Will clause and @old operator
- **Invariants** (060-069): Where clauses and checking
- **Flow/Verification** (070-091): Checking and verification modes

[5] All diagnostics shall be registered in Annex E §E.5 with:

- Canonical code (E12-[NNN])
- Section reference
- Description
- Severity (all E for Error in current version)
- Structured payload schema

#### §12.10.8 Future Extensions [contract.diagnostics.future]

[6] Future editions of this specification may introduce:

- Quantified predicates (`forall`, `exists`) with verification support
- Contract derivation (automatic contract implementation)
- Refinement types (contracts in type expressions)
- Temporal operators beyond `@old` (e.g., `@eventually`, `@always`)
- Frame conditions (explicit specification of unchanged state)

[7] Such extensions shall be designed to maintain backward compatibility with existing contracts or shall be introduced in new major versions per §1.7 [intro.versioning].

#### §12.10.9 Conformance Testing [contract.diagnostics.testing]

[8] Conformance test suites should verify:

- All smart defaulting forms parse correctly
- Grant checking rejects missing grants
- Precondition violations are detected (statically or dynamically)
- Postcondition violations are detected
- Invariants are checked at all required points
- Verification modes behave as specified
- Contract/behavior distinction is enforced
- All diagnostic codes are emitted for corresponding violations

[9] Test coverage shall include positive cases (valid contracts) and negative cases (each diagnostic code triggered).

#### §12.10.10 Conformance Requirements [contract.diagnostics.requirements]

[10] Implementations claiming conformance to Clause 12 shall:

1. Satisfy all requirements in §12.10.2
2. Emit all diagnostics defined in §12.10.3
3. Provide structured diagnostic payloads per §12.10.4
4. Integrate contracts with all systems listed in §12.10.5
5. Support the complete grammar in §12.9 and Annex A
6. Document build flags and verification behavior
7. Maintain contract metadata for reflection and tooling
8. Pass conformance tests per §12.10.8

---

**Previous**: §12.9 Grammar Reference (§12.9 [contract.grammar]) | **Next**: Clause 13 — Witness System (§13 [witness])
