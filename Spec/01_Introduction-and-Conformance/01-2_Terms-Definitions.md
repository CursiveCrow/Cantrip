# Cursive Language Specification

## Clause 1 — Introduction and Conformance

**Part**: I — Introduction and Conformance
**File**: 01-2_Terms-Definitions.md  
**Section**: 1.3 Terms and Definitions  
**Stable label**: [intro.terms]  
**Forward references**: §2.5 [lex.units], Clause 5 [decl], Clause 6 [name], Clause 8 [expr], §12.3 [memory.region], §12.4 [memory.permission], Clause 13 [contract], Clause 14 [witness]

---

### §1.3 Terms and Definitions [intro.terms]

[1] The vocabulary below is normative. When a term is defined in this clause it shall carry the same meaning throughout Clauses 2–16 and the annexes. Paragraph numbering resets within each subclause per the organisational template described in §1.6 [intro.document].

#### §1.3.1 General Language Terms [intro.terms.general]

[2] **binding** — the association between an identifier and an entity within a particular scope.

[3] **compilation unit** — the smallest source artifact processed as a whole during translation (§2.5 [lex.units]).

[4] **conforming implementation** — any compiler, interpreter, or analysis tool that meets the requirements of §1.5.3 [intro.conformance.impl] and documents its implementation-defined behaviour.

[5] **conforming program** — a program that satisfies the grammar, static semantics, and behavioural requirements outlined in §1.5.4 [intro.conformance.program]; conforming programs shall not rely on undefined behaviour.

[6] **declaration** — a syntactic form that introduces a name and determines its category (§5 [decl]).

[7] **diagnostic** — a message issued when a program violates a rule that requires detection (§1.5.5 [intro.conformance.diagnostics]). Diagnostics include errors (rejecting) and warnings (permissive).

[8] **entity** — any value, type, module, predicate, contract, region, or grant that may be named or referenced.

[9] **expression** — a syntactic form that yields a value or place (§8 [expr]).

[10] **statement** — a syntactic form that performs an effect without directly yielding a value (§9 [stmt]).

[11] **scope** — the syntactic region where a binding is visible (§6.2 [name.scope]).

#### §1.3.2 Cursive-Specific Concepts [intro.terms.language]

[12] **binding category** — the axis that distinguishes owning (`=`) and referring (`<-`) initialisation for `let`/`var` bindings (§5.2 [decl.variable]).

[13] **permission** — a capability (`const`, `shared`, `unique`) describing how a value may be accessed (§12.4 [memory.permission]).

[14] **grant** — a token representing authority to perform an effect such as file I/O (§13.3 [contract.grant]).

[15] **grant set** — the finite set of grants required or produced by an expression, preserved throughout typing (§8.8 [expr.typing]).

[16] **contract** — a user-defined interface declaration that specifies required procedure signatures and associated types (§11.4 [generic.contract]). Contracts are distinct from contractual sequents.

[17] **contractual sequent** — a sequent calculus specification attached to a function or procedure declaration that describes grants (capabilities), preconditions (`must`), and postconditions (`will`) (§13.2 [contract.sequent]). Contractual sequents use the form `{| grants |- must => will |}` and are part of the contracts system but distinct from contract declarations.

[18] **predicate (type)** — a collection of callable signatures with shared obligations; predicates differ from contracts by providing bodies (§11.4 [generic.predicate]).

[19] **modal type** — a type whose values transition through named states validated at compile time (§14.1 [witness.overview]).

[20] **witness** — runtime evidence that a contract or predicate obligation has been satisfied (§14.2 [witness.kind]).

[21] **region** — a lexically-scoped allocation arena released in strict LIFO order (§12.3 [memory.region]).

[22] **unsafe block** — an explicit region where the programmer assumes responsibility for safety constraints while interacting with raw operations (§12.8 [memory.unsafe]).

[23] **hole** — a placeholder for an inferred type, permission, or grant inserted by programmers or tools (§3.4 [basic.binding]).

#### §1.3.3 Behavioural Classifications [intro.terms.behaviour]

[23] **implementation-defined behaviour** — behaviour where the specification provides a set of allowed outcomes and requires documentation (§1.5.4 [intro.conformance.impldef]).

[24] **unspecified behaviour** — behaviour for which multiple outcomes are permitted without documentation, yet each outcome is well-defined (§1.5.6 [intro.conformance.unspecified]).

[25] **undefined behaviour (UB)** — behaviour for which the specification imposes no requirements; UB entries are enumerated in Annex B.2 [behavior.undefined].

[26] **ill-formed program** — a program that violates a static semantic rule requiring diagnosis (§1.5.5 [intro.conformance.diagnostics]).

[27] **ill-formed, no diagnostic required (IFNDR)** — rare violations that implementations are not obligated to detect (§1.5.5 [intro.conformance.ifndr]).

#### §1.3.4 Symbols and Abbreviations [intro.terms.abbrev]

[28] **ABI** — Application Binary Interface.

[29] **AST** — Abstract Syntax Tree.

[30] **EBNF** — Extended Backus–Naur Form.

[31] **FFI** — Foreign Function Interface.

[32] **IFNDR** — Ill-Formed, No Diagnostic Required.

[33] **LPS** — Lexical Permission System.

[34] **RAII** — Resource Acquisition Is Initialisation.

[35] **UB** — Undefined Behaviour.

[36] **UTF-8** — Unicode Transformation Format, 8-bit.

[37] Numerical and symbolic notations used in inference rules are catalogued in §1.4.3 [intro.notation.math].

---

**Previous**: §1.2 Normative References (§1.2 [intro.refs]) | **Next**: §1.4 Notation and Conventions (§1.4 [intro.notation])
