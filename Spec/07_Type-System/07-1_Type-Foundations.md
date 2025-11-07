# Cursive Language Specification

## Clause 7 — Types

**Clause**: 7 — Types
**File**: 07-1_Type-Foundations.md
**Section**: §7.1 Type System Overview
**Stable label**: [type.overview]  
**Forward references**: §7.2 [type.primitive], §7.3 [type.composite], §7.4 [type.function], §7.5 [type.pointer], §7.6 [type.relation], §7.7 [type.introspection], Clause 8 [expr], Clause 11 [generic], Clause 12 [memory], Clause 13 [contract], Clause 14 [witness]

---

### §7.1 Type System Overview [type.overview]

[1] This clause specifies the static type system that governs every well-formed Cursive program. The type system classifies values, constrains expressions, coordinates permissions and effects, and ensures that well-typed programs avoid undefined behavior attributable to type errors.

[2] Cursive’s type discipline is primarily nominal, augmented with structural forms where doing so preserves zero abstraction cost and local reasoning. Bidirectional type inference is an intrinsic design goal: the type checker flows information both from expressions to their contexts and from contexts into expressions so that most local declarations do not require redundant annotations while still keeping inference predictable.

[3] Safe union types are first-class citizens. The operator `\/` forms discriminated unions whose runtime representation carries a tag; each component type is automatically a subtype of the union, enabling control-flow joins without manual wrapper declarations. §7.3 [type.composite] defines the formation, pattern-matching rules, and subtype relationships for these unions.

#### §7.1.1 Design Objectives [type.overview.objectives]

[4] The type system shall satisfy the following objectives:

- **Safety**: Typing judgments preclude use-after-free, type confusion, and contract violations when combined with the permission (§12) and contract (§13) systems.
- **Determinism**: Type checking is deterministic with respect to the source program, the available grants, and the compilation environment.
- **Explicitness**: Effects, grants, modal states, and region usage are reflected in type constructors, ensuring that each obligation is visible in source.
- **Composability**: Types compose predictably through generics, unions, and composite constructors without hidden coercions or implicit special cases.
- **LLM-friendly regularity**: Type formation rules, inference heuristics, and rule names follow consistent patterns so that automated tooling can reason locally.

#### §7.1.2 Type Families [type.overview.families]

[5] Clause 7 partitions the universe of types across the subclauses listed below. Collections (arrays, slices, strings, ranges) are treated as composite structures and specified within §7.3 [type.composite].

| Family                     | Defined in                | Highlights                                                                 |
| -------------------------- | ------------------------- | -------------------------------------------------------------------------- |
| Primitive types            | §7.2 [type.primitive]     | Fixed-width scalars, unit, never                                           |
| Composite types            | §7.3 [type.composite]     | Tuples, records, enums, discriminated unions, collection constructs        |
| Procedure (callable) types | §7.4 [type.function]      | Call signatures with grant and contract annotations, closure capture model |
| Pointer & reference types  | §7.5 [type.pointer]       | Region-aware pointers, modal state projections                             |
| Type relations             | §7.6 [type.relation]      | Equivalence, subtyping, compatibility, variance                            |
| Introspection              | §7.7 [type.introspection] | `typeof`, compile-time queries, predicate reflection                       |

[6] Each family subclause follows the standard structure (overview → syntax → constraints → semantics → examples) mandated by SpecificationOnboarding.md. Concrete grammars for individual constructors appear in their respective subclauses; §7.1 provides only categorical organization.

#### §7.1.3 Typing Judgments and Notation [type.overview.judgments]

[7] The core judgments reused throughout Clause 7 are:

- $\Gamma \vdash \tau : \text{Type}$ — $\tau$ is a well-formed type under environment $\Gamma$.
- $\Gamma \vdash e : \tau \; ! \varepsilon$ — expression $e$ has type $\tau$ and requires grant set $\varepsilon$.
- $\tau_1 <: \tau_2$ — subtyping relation defined in §7.6 [type.relation].
- $\tau_1 \simeq \tau_2$ — type equivalence relation that respects aliases and structural normalization (§7.6.3).

[8] Type environments bind term variables to types, type variables to kinds (with optional predicate bounds), grant parameters to grant sets, and modal parameters to state constraints. Environment formation and lookup rules follow §1.3 [intro.terms] conventions; §7.6 restates any additional obligations introduced by subtype or modal analysis.

#### §7.1.4 Guarantees [type.overview.guarantees]

[9] A program that satisfies all typing, permission, and contract judgments shall not:

- Invoke undefined behavior listed in Annex B due to type mismatches.
- Access memory through an invalid pointer representation (permissions enforce aliasing rules).
- Invoke a procedure without possessing the grants named in its signature.
- Transition a modal value to a state not permitted by its modal type.

[10] Bidirectional inference is sound and complete with respect to the declarative typing rules stated in §§7.2–7.5. When local annotations are omitted, inference succeeds if and only if a unique principal type exists under the rules of this clause. Situations requiring programmer guidance (e.g., ambiguous unions or polymorphic recursion) shall elicit diagnostics that point to the relevant obligation.

#### §7.1.5 Integration Points [type.overview.integration]

[11] Clause 7 interacts tightly with other parts of the specification:

- **Expressions (§8)**: Expression forms consume and produce types; §8.8 links directly to the typing rules defined here.
- **Generics (§11)**: Type parameters, bounds, and instantiation are defined in the generic system and referenced by each type constructor.
- **Memory model (§12)**: Region annotations, permissions, and move semantics enrich pointer and composite types.
- **Contracts and witnesses (§§13–14)**: Procedure types embed sequents; witness construction depends on type information to ensure modal and contract obligations are satisfied.
- **Modules (§4)**: Type checking executes after module initialization, so exported declarations are fully defined (§4.6.7). Only `public` types may be referenced across module boundaries—referencing an internal type emits E04-700—and generic instantiations must supply type arguments that satisfy both visibility and grant requirements. Contract implementations attached to exported types shall succeed before the type becomes visible, and diagnostics that mention cross-module types SHALL include the module path in their payloads (Annex E §E.5).

[12] Annex A (§A.3) provides the consolidated grammar for all type forms. Annex C formalizes the typing judgments and proves progress/preservation theorems that rely on the rules introduced in this clause. Annex E supplies implementation guidance and diagnostic catalogs; the most relevant entries include definite-assignment checks, subtype cycle detection, and union exhaustiveness validation.

#### §7.1.6 Integration with Memory Model [type.memory.integration]

[13] The type system integrates with the memory model to ensure memory safety without runtime overhead. Permission qualifiers (`const`, `shared`, `unique`) attached to types (§12.4) affect type formation and subtyping. A type `T` with permission `perm` forms the type `perm T`, which participates in the permission subtyping rules defined in Clause 12.

[14] Permission annotations on pointer types (§7.5) determine what operations are permitted on the pointed-to value. The type system enforces that `const` pointers allow only read operations, `shared` pointers allow concurrent read access but require synchronization for writes, and `unique` pointers allow exclusive mutable access. Permission checking occurs during type checking (§2.2.4.3), ensuring that permission violations are caught at compile time.

[15] Region annotations (`^T` for region-allocated types) affect type formation and lifetime analysis. Region-allocated types are associated with a specific region scope (§12.3), and the type system ensures that region-allocated values do not escape their region. Escape analysis (§7.5.3.2) uses type information to determine whether pointers can safely escape their allocation region.

[16] Ownership semantics (§12.4) are reflected in the type system through move semantics and copy predicates. Types that implement the copy predicate (§7.5.4) can be copied; types without copy predicates must be moved. Type checking enforces move semantics by tracking ownership transfers through procedure calls and assignments, ensuring that moved values are not used after move.

[17] Pointer types (§7.5) integrate closely with the memory model to ensure memory safety. Safe modal pointers (`Ptr<T>@State`) track pointer state through the type system, preventing use-after-free and null pointer dereferences. The type system enforces that pointer state transitions (§7.6) are valid according to the modal type system, null pointers (`Ptr<T>@Null`) cannot be dereferenced, and weak pointers (`Ptr<T>@Weak`) must be upgraded before use.

[18] Composite types (§7.3) interact with the memory model to determine layout, alignment, and padding. Record types, tuple types, and array types all have memory layout requirements that affect field offsets, alignment, size calculations for stack and heap allocation, and ABI compatibility for interoperation (§16).

[19] Generic types (§11) interact with the memory model through type parameters and bounds. Generic type parameters may carry permission or region annotations, affecting how the generic type is instantiated. When a generic type is instantiated, the memory model constraints from the type arguments are propagated to the instantiated type, ensuring that permission requirements are satisfied, region scopes are properly nested, and ownership semantics are preserved.

[20] Clause 12 (Memory Model) relies on type information to perform permission checking, region analysis, and ownership verification. Type errors that involve memory model violations shall include both type and memory model information in diagnostic payloads (Annex E §E.5).

#### §7.1.7 Examples (Informative) [type.overview.examples]

**Example 7.1.7.1 (Bidirectional inference with unions):**

```cursive
procedure normalize(input: stream::Line): string \/ io::Error
    {| io::read |- input.is_open() => result is string \/ io::Error |}
{
    let trimmed = input.text().trim();        // Context infers `string`
    if trimmed.is_empty() {
        result io::Error::invalid_data("empty line")
    } else {
        result trimmed
    }
}
```

[21] The return type annotation supplies the expected union. The `result` statements rely on automatic widening (`string <: string \/ io::Error`) and on the contractual sequent to document the effect requirements.

**Example 7.1.7.2 (Pointer state integrated with permissions):**

```cursive
procedure swap_current(~!, nodes: list::Iterator@Active, value: Node)
    {| allocator::unique |- nodes.has_current() => nodes.current() == value |}
{
    let target: Ptr<Node>@Valid = nodes.current_ptr();
    target.write(value);   // Pointer type ensures state validity and unique access
}
```

[22] The pointer type records both the region (`PointerType`, §7.5) and the modal state (`@Valid`). The contract and grant annotations rely on the type system to guarantee that the write is safe.

---

**Previous**: _(start of clause)_ | **Next**: §7.2 Primitive Types (§7.2 [type.primitive])
