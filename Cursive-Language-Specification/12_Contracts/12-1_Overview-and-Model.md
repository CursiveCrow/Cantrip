# Cursive Language Specification

## Clause 12 — Contracts

**Clause**: 12 — Contracts
**File**: 12-1_Overview-and-Model.md
**Section**: §12.1 Overview and Model
**Stable label**: [contract.overview]  
**Forward references**: §12.2 [contract.sequent], §12.3 [contract.grant], §12.4 [contract.precondition], §12.5 [contract.postcondition], §12.6 [contract.invariant], §12.7 [contract.checking], §12.8 [contract.verification], Clause 5 §5.4 [decl.function], Clause 5 §5.9 [decl.grant], Clause 10 [generic], Clause 13 [witness]

---

### §12.1 Overview and Model [contract.overview]

#### §12.1.1 Overview

[1] The contract system provides mechanisms for specifying and verifying behavioral requirements on procedures and types through _contractual sequents_. A contractual sequent is a formal specification attached to procedure declarations that expresses required capabilities (grants), caller obligations (preconditions), and callee guarantees (postconditions).

[2] Contractual sequents enable:

- **Capability tracking**: Explicit declaration of computational effects and resource access through grants
- **Interface specification**: Abstract procedure signatures with behavioral requirements via contracts
- **Static verification**: Compile-time checking of preconditions and postconditions where provable
- **Dynamic checking**: Runtime assertion insertion for conditions that cannot be proven statically
- **Documentation**: Machine-verifiable specifications that serve as executable documentation
- **Optimization**: Compiler optimization based on proven contract guarantees

[3] This clause specifies the complete contract system: sequent syntax (§12.2), grants (§12.3), preconditions (§12.4), postconditions (§12.5), invariants (§12.6), checking algorithms (§12.7), verification modes (§12.8), grammar integration (§12.9), and conformance requirements (§12.10).

#### §12.1.2 Contracts vs Behaviors [contract.overview.distinction]

[4] Cursive employs two distinct polymorphism mechanisms that serve complementary purposes:

- **Contracts** (this clause): Abstract interface specifications with no implementations
- **Behaviors** (Clause 10 §10.4): Concrete code reuse with default implementations

[5] Table 12.1 summarizes the fundamental distinction:

**Table 12.1 — Contracts vs behaviors**

| Aspect                  | Contracts (Clause 12)                            | Behaviors (Clause 10)                            |
| ----------------------- | ------------------------------------------------ | ------------------------------------------------ |
| Purpose                 | Abstract interface specification                 | Concrete code reuse                              |
| Procedure bodies        | **NO bodies allowed** (purely abstract)          | **ALL procedures MUST have bodies**              |
| Associated types        | Requirements (no defaults)                       | May have defaults                                |
| Dispatch                | Static (monomorphization) or dynamic (witnesses) | Static (monomorphization)                        |
| Attachment syntax       | `record T: Contract`                             | `record T with Behavior`                         |
| Declaration syntax      | `contract Name { ... }`                          | `behavior Name { ... }`                          |
| Implementation location | Inline in type body                              | Inline or standalone `behavior B for T`          |
| Sequents                | Optional (defaults to `[[ |- true => true ]]`)   | Optional (defaults to `[[ |- true => true ]]`)   |
| Primary use case        | Defining polymorphic interfaces                  | Sharing default implementations                  |

[6] Types may attach both contracts and behaviors simultaneously:

```cursive
record Account: Ledgered, Auditable with Serializable, Display {
    // Ledgered, Auditable: contracts (abstract requirements)
    // Serializable, Display: behaviors (concrete code reuse)
}
```

[7] The separation ensures that interface obligations (contracts) remain distinct from implementation sharing (behaviors), preventing confusion about whether a declaration provides code or specifies requirements.

#### §12.1.3 Sequent Calculus Model [contract.overview.sequent]

[8] Cursive's contract system is based on _sequent calculus_, a formal logical framework for reasoning about entailment. A contractual sequent has the form:

$$
\[[ G \vdash P \Rightarrow Q ]\]
$$

where:

- $G$ is the _grant context_ (capability assumptions)
- $P$ is the _antecedent_ (preconditions that must hold)
- $Q$ is the _consequent_ (postconditions that will hold)
- $\vdash$ is the _turnstile_ (entailment operator)
- $\Rightarrow$ is _implication_

[9] In Cursive source syntax, sequents use semantic brackets `⟦ ⟧` (Unicode U+27E6, U+27E7) or ASCII equivalent `[[ ]]` with keywords:

$$
\text{[[} \; \texttt{grants} \; \texttt{|-} \; \texttt{must} \; \texttt{=>} \; \texttt{will} \; \text{]]}
$$

The turnstile `⊢` is written `|-` (ASCII) and implication `⇒` is written `=>` (ASCII).

[10] The semantic interpretation is: "Given grants $G$, if preconditions $P$ hold on entry, then postconditions $Q$ will hold on exit."

#### §12.1.4 Sequent Components [contract.overview.components]

[11] A complete contractual sequent has three components:

**Grants clause** (before turnstile):
[12] The grants clause lists capability tokens that must be available for the procedure to execute. Grants represent permissions to perform observable effects: file I/O, network access, heap allocation, etc. The grant system is specified in detail in §12.3.

**Preconditions** (must clause):
[13] Preconditions specify conditions that must hold when the procedure is called. They represent _caller obligations_: the caller must ensure preconditions are satisfied before invoking the procedure. Preconditions are specified in §12.4.

**Postconditions** (will clause):
[14] Postconditions specify conditions that will hold when the procedure returns normally. They represent _callee guarantees_: the procedure implementation must ensure postconditions are satisfied on all non-diverging exit paths. Postconditions are specified in §12.5.

[15] All three components are optional with smart defaulting (§12.2.4):

- Omitted grants clause: Empty grant set `∅` (pure procedure)
- Omitted preconditions: `true` (no requirements)
- Omitted postconditions: `true` (no guarantees)
- Entirely omitted sequent: `[[ |- true => true ]]`

#### §12.1.5 Invariants and Desugaring [contract.overview.invariants]

[16] Cursive provides `where` clauses for declaring _invariants_: conditions that must always hold at observable program points. Invariants desugar to conjunctions in sequent postconditions.

**Type invariants**:
[17] Type declarations may include `where` clauses specifying type-level invariants. These desugar to implicit postcondition conjunctions on all constructors and mutating methods:

```cursive
record BankAccount {
    balance: i64,

    where {
        balance >= 0
    }

    procedure deposit(~!, amount: i64)
        [[ ledger::post |- amount > 0 => self.balance >= amount ]]
    {
        self.balance += amount
    }
}
```

[18] The `where { balance >= 0 }` invariant automatically extends the postcondition of `deposit` to:

$$
\texttt{will} \; (\texttt{self.balance} \ge \texttt{amount}) \land (\texttt{self.balance} \ge 0)
$$

The compiler inserts the invariant check automatically; programmers need not write it explicitly.

**Loop invariants**:
[19] Loop statements may include `where` clauses specifying loop invariants. These conditions must hold on loop entry, at the start of each iteration, and on loop exit:

```cursive
loop i in 0..array.len() where { i >= 0, i <= array.len() } {
    // Invariant checked: entry, each iteration, exit
}
```

[20] Complete invariant semantics and desugaring rules are specified in §12.6.

#### §12.1.6 Verification Strategy [contract.overview.verification]

[21] Contractual sequents are verified through a combination of static proof and dynamic checking:

**Static verification** (compile-time):
[22] The compiler attempts to prove contracts using dataflow analysis, symbolic execution, and optional SMT solvers. Provable contracts are verified at compile time with zero runtime overhead.

**Dynamic verification** (runtime):
[23] Contracts that cannot be proven statically are converted to runtime assertions. The verification mode (§12.8) controls when dynamic checks are inserted: always, debug-only, release-proven-only, or never.

**Verification modes**:
[24] Procedures may specify verification requirements via attributes:

- `[[verify(static)]]` — Must prove statically or compilation fails
- `[[verify(dynamic)]]` — Always insert runtime checks
- `[[verify(trusted)]]` — Trust contract, no checks (programmer responsibility)
- No attribute — Build-mode default (debug inserts checks, release only proven)

[25] Verification modes are metadata (not part of sequent syntax) and do not affect procedure type signatures. Two procedures with identical sequents but different verification modes have the same type.

#### §12.1.7 Contract Declarations [contract.overview.contracts]

[26] In addition to contractual sequents on procedures, Cursive supports _contract declarations_: abstract interfaces that types may implement for polymorphism.

[27] Contract declarations define procedure signatures without implementations:

```cursive
public contract Serializable {
    type Format = string@View

    procedure serialize(~): Self::Format
        [[ alloc::heap |- true => result.len() > 0 ]]

    procedure deserialize(data: Self::Format): Self \/ ParseError
        [[ alloc::heap |- data.len() > 0 => true ]]
}
```

[28] Types attach contracts using `:` syntax in the type declaration header:

```cursive
record User: Serializable {
    id: u64,
    name: string@Managed,

    type Format = string@Managed

    procedure serialize(~): string@Managed
        [[ alloc::heap |- true => result.len() > 0 ]]
    {
        // Implementation must satisfy contract
    }

    procedure deserialize(data: string@Managed): Self \/ ParseError
        [[ alloc::heap |- data.len() > 0 => true ]]
    {
        // Implementation
    }
}
```

[29] Contract declarations, satisfaction rules, and coherence are integrated with the behavior system defined in Clause 10. This clause focuses on the sequent notation and verification; polymorphic dispatch is specified in Clause 13 (Witness System).

#### §12.1.8 Integration with Other Systems [contract.overview.integration]

[30] The contract system integrates with multiple language features:

**Declarations (Clause 5)**:
[31] Contractual sequents attach to procedure declarations (§5.4) and contract declarations (§5.5). Grant declarations (§5.9) introduce user-defined capability tokens used in grant clauses.

**Type System (Clause 7)**:
[32] Grants and contracts are reflected in callable types (§7.4): procedure types include grant sets and sequent specifications. Subtyping for callable types respects contract variance (contravariance in preconditions, covariance in postconditions).

**Generics (Clause 10)**:
[33] Grant parameters (§10.2.5) enable grant-polymorphic procedures. Contract bounds (§10.3) constrain type parameters to types satisfying specific contracts. Behaviors and contracts are distinguished as described in §10.4.1.2.

**Memory Model (Clause 11)**:
[34] Grants track memory-related capabilities (heap allocation, region allocation). Contracts interact with permissions (§11.4) and RAII cleanup (§11.2).

**Witness System (Clause 13)**:
[35] Contracts enable dynamic polymorphism through witnesses. The witness system provides runtime evidence that contracts are satisfied, supporting dynamic dispatch while maintaining type safety.

#### §12.1.9 Design Philosophy [contract.overview.philosophy]

[36] The contract system embodies Cursive's core design principles:

**Explicit over implicit**:
[37] All grants, preconditions, and postconditions are explicitly written in procedure signatures. There are no hidden effects, implicit preconditions, or unspecified guarantees.

**Local reasoning**:
[38] Contractual sequents provide complete information at the call site. Programmers can understand procedure requirements and guarantees by reading the signature alone, without inspecting the implementation.

**Zero abstraction cost**:
[39] Contracts verified statically impose no runtime overhead. Dynamic checks are inserted only when static proof fails, and verification modes provide explicit control over when checks execute.

**Compositional**:
[40] Contracts compose through procedure calls following sequent calculus rules. Grant sets accumulate via union; precondition obligations transfer to callers; postcondition guarantees transfer to callees.

**LLM-friendly**:
[41] The regular sequent structure, explicit keyword markers (`grants`, `must`, `will`), and deterministic desugaring rules make contracts predictable for AI-assisted development and automated tooling.

#### §12.1.10 Conformance Requirements [contract.overview.requirements]

[42] Implementations shall:

1. Support contractual sequent syntax on procedure declarations with semantic brackets `⟦ ⟧` or ASCII `[[ ]]`
2. Parse and validate grant clauses, preconditions, and postconditions per §§12.2-12.5
3. Implement smart defaulting for omitted sequent components
4. Desugar `where` invariants to postcondition conjunctions per §12.6
5. Perform grant availability checking at call sites per §12.7
6. Support verification modes (static, dynamic, trusted) per §12.8
7. Distinguish contracts from behaviors: contracts have no procedure bodies, behaviors must have bodies
8. Integrate contracts with the type system, generics, and witness system
9. Emit diagnostics for contract violations (E12-xxx family)
10. Maintain contract metadata through compilation phases for reflection and tooling

---

**Previous**: Clause 11 — Memory Model, Regions, and Permissions (§11.8 [memory.unsafe]) | **Next**: §12.2 Sequent: Syntax and Structure (§12.2 [contract.sequent])
