# Cursive Language Specification
## Clause 1 — Introduction and Conformance

**Part**: I — Introduction and Conformance
**File**: 01-0_Scope.md  
**Section**: 1.1 Scope  
**Stable label**: [intro.scope]  
**Forward references**: §2.1–§2.5 [lex.source]–[lex.units], Clauses 5–10, §11–§14, §16.6 [interop.abi], Annex B.2 [behavior.undefined]

---

### §1.1 Scope [intro.scope]

[1] This clause establishes the boundaries of the Cursive language specification, defines the actors it serves, and identifies complementary material external to the core standard. The scope applies to all implementations that claim adherence to Cursive version 1.0 and later compatible editions.

[2] The specification governs every program element required to compile and execute portable, deterministic Cursive code. The governed areas are:
- lexical structure and translation phases (§2.1–§2.5 [lex.source]–[lex.units]);
- grammar and semantics for declarations, types, expressions, statements, and compile-time constructs (§5–§10);
- the permission, grant, contract, modal, and witness systems that ensure memory safety and semantic guarantees (§11–§14);
- interoperability and concurrency requirements (§15–§16).

[3] The specification intentionally excludes topics whose evolution is decoupled from language conformance. Excluded topics include:
- the normative standard library catalogue (Annex F [library]);
- platform-specific ABI and calling convention details left to implementation policy (§16.6 [interop.abi]);
- concurrency libraries beyond the surface guarantees described in §15 [concurrency.model];
- tooling, build systems, or project layout conventions (informative guidance may appear in companion documents).

[4] Certain behaviours are implementation-defined but remain within the specification’s boundary. Conforming implementations shall document at least integer and floating-point representations, pointer and region sizes, alignment rules, diagnostic formatting, and resource ceilings consistent with §1.5.4 [intro.conformance.impldef].

[5] Undefined behaviour is catalogued centrally in Annex B.2 [behavior.undefined]. Any construct not explicitly marked as undefined shall be diagnosed or defined. Implementers shall ensure that undefined entries referenced in later clauses map back to the Annex catalogue.

#### §1.1.1 Intended Audiences [intro.scope.audience]

[6] Implementers require a precise definition of syntax, typing, evaluation, and diagnostic obligations. Paragraphs marked **Normative** in Clauses 2–16 and Annexes A–B constitute their primary reference set.

[7] Tool developers (formatters, linters, debuggers, analysers) depend on the grammar (Annex A [grammar]), semantic judgments (§1.4 [intro.notation]), and diagnostic contracts (§1.5 [intro.conformance]).

[8] Language designers and researchers rely on the formal notation, meta-theory, and rationale supplied throughout Clause 1, Clause 3 (§3.1 [basic.type]) and Annex C [formal].

[9] Programmers seeking educational material should use the informative Programming Guide; Clause 1 provides orientation but does not substitute for tutorials.

#### §1.1.2 Forward References [intro.scope.forward]

[10] Clause 1 references later material only when necessary to state the conformance boundary. Every forward reference is identified explicitly so that implementers can trace dependencies. Circular references are avoided by delegating operational detail to the destination clause.

---

**Previous**: *(start)* | **Next**: §1.2 Normative References (§1.2 [intro.refs])
