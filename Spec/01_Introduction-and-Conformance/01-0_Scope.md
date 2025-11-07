# Cursive Language Specification

## Clause 1 — Introduction and Conformance

**Part**: I — Introduction and Conformance
**File**: 01-0_Scope.md  
**Section**: 1.1 Scope  
**Stable label**: [intro.scope]  
**Forward references**: §2.1–§2.5 [lex.source]–[lex.units], Clause 3 [basic], Clauses 4–10, §11–§14, §16.6 [interop.abi], Annex B.2 [behavior.undefined]

---

### §1.1 Scope [intro.scope]

[1] This clause establishes the boundaries of the Cursive language specification, defines the actors it serves, and identifies complementary material external to the core standard. The scope applies to all implementations that claim adherence to Cursive version 1.0 and later compatible editions.

[2] The specification governs every program element required to compile and execute portable, deterministic Cursive code. The governed areas are:

- lexical structure and translation phases (§2.1–§2.5 [lex.source]–[lex.units]);
- foundational concepts: objects, values, storage duration, alignment, and name binding categories (Clause 3 [basic]);
- modules, declarations, names and scopes, types, and expressions (Clauses 4–8);
- statements and control flow (Clause 9);
- the generics, permission, contract, and witness systems that ensure memory safety and semantic guarantees (Clauses 10–13) [ Note: Clauses 10 (Generics and Predicates), 11 (Memory Model, Regions, and Permissions), 12 (Contracts), and 13 (Witness System) are not yet written. Forward references to these clauses will be fully specified when those clauses are authored.
  — end note ];
- concurrency, interoperability, and compile-time evaluation (Clauses 14–16).

[3] The specification intentionally excludes topics whose evolution is decoupled from language conformance. Excluded topics include:

- the normative standard library catalogue (Annex F [library]);
- platform-specific ABI and calling convention details left to implementation policy (§15.6 [interop.abi]);
- concurrency libraries beyond the surface guarantees described in §14.1 [concurrency.model];
- tooling, build systems, or project layout conventions (informative guidance may appear in companion documents).

[4] Certain behaviours are implementation-defined but remain within the specification’s boundary. Conforming implementations shall document at least integer and floating-point representations, pointer and region sizes, alignment rules, diagnostic formatting, and resource ceilings consistent with §1.5.4 [intro.conformance.impldef].

[5] Undefined behaviour is catalogued centrally in Annex B.2 [behavior.undefined]. Any construct not explicitly marked as undefined shall be diagnosed or defined. Implementers shall ensure that undefined entries referenced in later clauses map back to the Annex catalogue.

#### §1.1.1 Intended Audiences [intro.scope.audience]

[6] Implementers require a precise definition of syntax, typing, evaluation, and diagnostic obligations. Paragraphs marked **Normative** in Clauses 2–16 and Annexes A–B constitute their primary reference set.

[7] Tool developers (formatters, linters, debuggers, analysers) depend on the grammar (Annex A [grammar]), semantic judgments (§1.4 [intro.notation]), and diagnostic contracts (§1.5 [intro.conformance]).

[8] Language designers and researchers rely on the formal notation, meta-theory, and rationale supplied throughout Clause 1, Clause 3 [basic] and Annex C [formal].

[9] Programmers seeking educational material should use the informative Programming Guide; Clause 1 provides orientation but does not substitute for tutorials.

#### §1.1.2 Forward References [intro.scope.forward]

[10] Clause 1 references later material only when necessary to state the conformance boundary. Every forward reference is identified explicitly so that implementers can trace dependencies. Circular references are avoided by delegating operational detail to the destination clause.

---

**Previous**: _(start)_ | **Next**: §1.2 Normative References (§1.2 [intro.refs])
