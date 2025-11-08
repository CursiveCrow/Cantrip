# Cursive Language Specification

## Clause 7 — Types

**Clause**: 7 — Types
**File**: 07-1_Type-Foundations.md
**Section**: §7.1 Type System Overview
**Stable label**: [type.overview]  
**Forward references**: §7.2 [type.primitive], §7.3 [type.composite], §7.4 [type.function], §7.5 [type.pointer], §7.6 [type.modal], §7.7 [type.relation], §7.8 [type.introspection], Clause 8 [expr], Clause 10 [generic], Clause 11 [memory], Clause 12 [contract], Clause 13 [witness]

---

### §7.1 Type System Overview [type.overview]

[1] This clause specifies the static type system that governs every well-formed Cursive program. The type system classifies values, constrains expressions, coordinates permissions and grants, and ensures that well-typed programs avoid undefined behavior attributable to type errors.

[2] Cursive’s type discipline is primarily nominal, augmented with structural forms where doing so preserves zero abstraction cost and local reasoning. Bidirectional type inference is an intrinsic design goal: the type checker flows information both from expressions to their contexts and from contexts into expressions so that most local declarations do not require redundant annotations while still keeping inference predictable.

[3] Safe union types are first-class citizens. The operator `\/` forms discriminated unions whose runtime representation carries a tag; each component type is automatically a subtype of the union, enabling control-flow joins without manual wrapper declarations. §7.3 [type.composite] defines the formation, pattern-matching rules, and subtype relationships for these unions.

#### §7.1.1 Design Objectives [type.overview.objectives]

[4] The type system shall satisfy the following objectives:

- **Safety**: Typing judgments preclude use-after-free, type confusion, and contract violations when combined with the permission (§12) and contract (§13) systems.
- **Determinism**: Type checking is deterministic with respect to the source program, the available grants, and the compilation environment.
- **Explicitness**: Grants, modal states, and region usage are reflected in type constructors, ensuring that each obligation is visible in source.
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
| Introspection              | §7.8 [type.introspection] | `typeof`, compile-time queries, behavior reflection                        |

[6] Each family subclause follows the standard structure (overview → syntax → constraints → semantics → examples) mandated by SpecificationOnboarding.md. Concrete grammars for individual constructors appear in their respective subclauses; §7.1 provides only categorical organization.

#### §7.1.3 Permission-Qualified Types [type.overview.permissions]

[6.1] **Syntax.** Permission qualifiers attach to types at the binding site, following the grammar:

```ebnf
variable_binding
    ::= binding_head identifier ":" permission? type_expression "=" expression

permission
    ::= "const"
     | "shared"
     | "unique"
```

[ Note: See Annex A §A.7 [grammar.declaration] for complete binding grammar.
— end note ]

[6.2] **Formation and Composition.** Permission qualifiers bind more tightly than modal state selectors. The syntax `unique Self@Open` is parsed as `(unique Self)@Open`, where `unique` modifies the base type `Self` and `@Open` selects the modal state variant of the permission-qualified type.

[6.3] **Semantics.** Permission qualifiers determine what operations are permitted on values:

- `const`: Read-only access, may be aliased freely
- `shared`: Concurrent read access, requires synchronization for writes
- `unique`: Exclusive mutable access, enforces uniqueness

Permission semantics, the permission lattice (`unique <: shared <: const`), checking rules, and integration with the memory model are fully specified in §11.4 [memory.permission].

[6.4] **Examples.**

```cursive
let immutable: const i32 = 42
let concurrent: shared Buffer = get_shared_buffer()
let exclusive: unique Handle = acquire_resource()

// Permission composes with modal states
procedure transition(file: unique FileHandle@Closed): FileHandle@Open
    [[ fs::open |- true => true ]]
{ ... }
```

#### §7.1.4 Typing Judgments and Notation [type.overview.judgments]

[7] The core judgments reused throughout Clause 7 are:

- $\Gamma \vdash \tau : \text{Type}$ — $\tau$ is a well-formed type under environment $\Gamma$.
- $\Gamma \vdash e : \tau \; ! \varepsilon$ — expression $e$ has type $\tau$ and requires grant set $\varepsilon$.
- $\tau_1 <: \tau_2$ — subtyping relation defined in §7.7 [type.relation].
- $\tau_1 \simeq \tau_2$ — type equivalence relation that respects aliases and structural normalization (§7.7.2).

[8] Type environments bind term variables to types, type variables to kinds (with optional behavior bounds), grant parameters to grant sets, and modal parameters to state constraints. Environment formation and lookup rules follow §1.3 [intro.terms] conventions; §7.7 restates any additional obligations introduced by subtype or modal analysis.

#### §7.1.5 Guarantees [type.overview.guarantees]

[9] A program that satisfies all typing, permission, and contract judgments shall not:

- Invoke undefined behavior listed in Annex B due to type mismatches.
- Access memory through an invalid pointer representation (permissions enforce aliasing rules).
- Invoke a procedure without possessing the grants named in its signature.
- Transition a modal value to a state not permitted by its modal type.

[10] Bidirectional inference is sound and complete with respect to the declarative typing rules stated in §§7.2–7.6. When local annotations are omitted, inference succeeds if and only if a unique principal type exists under the rules of this clause. Situations requiring programmer guidance (e.g., ambiguous unions or polymorphic recursion) shall elicit diagnostics that point to the relevant obligation.

#### §7.1.6 Integration Points [type.overview.integration]

[11] Clause 7 interacts tightly with other parts of the specification:

- **Expressions (Clause 8)**: Expression forms consume and produce types; §8.8 links directly to the typing rules defined here.
- **Generics (Clause 10)**: Type parameters, bounds, and instantiation are defined in the generic system and referenced by each type constructor.
- **Memory model (Clause 11)**: Region annotations, permissions, and move semantics enrich pointer and composite types.
- **Contracts and witnesses (Clauses 12–13)**: Procedure types embed sequents; witness construction depends on type information to ensure modal and contract obligations are satisfied.
- **Modules (Clause 4)**: Type checking executes after module initialization, so exported declarations are fully defined (§4.6.7). Only `public` types may be referenced across module boundaries—referencing an internal type emits E07-750—and generic instantiations must supply type arguments that satisfy both visibility and grant requirements. Contract implementations attached to exported types shall succeed before the type becomes visible, and diagnostics that mention cross-module types SHALL include the module path in their payloads (Annex E §E.5).

[12] Annex A (§A.3) provides the consolidated grammar for all type forms. Annex C formalizes the typing judgments and proves progress/preservation theorems that rely on the rules introduced in this clause. Annex E supplies implementation guidance and diagnostic catalogs; the most relevant entries include definite-assignment checks, subtype cycle detection, and union exhaustiveness validation.

#### §7.1.7 Integration with Memory Model [type.memory.integration]

[13] The type system integrates with the memory model to ensure memory safety without runtime overhead. Permission qualifiers (`const`, `shared`, `unique`) specified in bindings (§7.1.3) affect type formation and subtyping. A binding `name: perm T` associates the identifier with a permission-qualified type, which **shall** participate in the permission subtyping rules defined in §11.4 [memory.permission].

**Permission System Integration:**

(13.1) Implementations **shall** enforce that permission-qualified types (`const T`, `shared T`, `unique T`) are well-formed only when:

1. The base type `T` is well-formed under $\Gamma \vdash T : \text{Type}$
2. The permission qualifier is compatible with the type's usage context
3. Permission downgrades follow the lattice: `unique <: shared <: const` (see Clause 11)

(13.2) Permission checking **shall** occur during type checking (§2.2.4.3). Implementations **shall** reject programs that:

- Attempt to mutate through `const` permissions (diagnostic defined in Clause 11)
- Violate unique access requirements (diagnostic defined in Clause 11)
- Create permission upgrades without explicit operations (diagnostic defined in Clause 11)

[14] Permission annotations on pointer types (§7.5) determine what operations are permitted on the pointed-to value. The type system **shall** enforce that:

1. `const` pointers allow only read operations
2. `shared` pointers allow concurrent read access but require synchronization for writes
3. `unique` pointers allow exclusive mutable access

Permission violations **shall** be caught at compile time with diagnostics specified in Clause 11 and §7.5.

**Region System Integration:**

[15] Region annotations affect type formation and lifetime analysis. Implementations **shall** ensure that:

1. Region-allocated types are associated with exactly one region scope (§11.3)
2. Region-allocated values do not escape their region (escape analysis in §7.5.3.2)
3. Region lifetimes are validated during type checking

Violations **shall** produce diagnostics specified in Clause 11 (region escape errors).

**Ownership and Move Semantics:**

[16] Ownership semantics (§11.4) are reflected in the type system through move semantics and copy behaviors. Implementations **shall**:

1. Track which types satisfy the `Copy` predicate
2. Enforce move semantics for types without `Copy`
3. Diagnose use-after-move violations during definite assignment analysis (§5.7)

Types that implement the copy behavior can be copied; types without copy behaviors **shall** be moved when assigned or passed to procedures. Type checking **shall** enforce move semantics by tracking responsibility transfers through procedure calls and assignments, ensuring that moved values are not used after move (diagnostic specified in Clause 11).

**Pointer Safety Integration:**

[17] Pointer types (§7.5) integrate with the memory model to ensure memory safety. Implementations **shall** enforce:

1. Safe modal pointers (`Ptr<T>@State`) track pointer state through the type system
2. Null pointers (`Ptr<T>@Null`) **shall not** be dereferenced (diagnostic E07-301)
3. Weak pointers (`Ptr<T>@Weak`) **shall** be upgraded before use
4. State transitions **shall** follow modal type rules (§7.6)

Violations **shall** produce diagnostics that identify both the type error and the memory safety issue.

**Layout and ABI Integration:**

[18] Composite types (§7.3) interact with the memory model to determine layout, alignment, and padding. Implementations **shall**:

1. Compute field offsets according to alignment rules
2. Insert padding to satisfy alignment constraints
3. Document layout choices for each target platform

Layout calculations affect stack and heap allocation, and **shall** be ABI-compatible for interoperation (Clause 15).

**Generic Type Memory Integration:**

[19] Generic types (Clause 10) interact with the memory model through type parameters and bounds. Implementations **shall**:

1. Validate that type arguments satisfy permission constraints
2. Verify region scope nesting when instantiating generic types
3. Propagate memory model constraints from type arguments to instantiated types

When a generic type is instantiated, the memory model constraints **shall** be checked and validated before code generation proceeds.

**Diagnostic Integration:**

[20] Clause 11 (Memory Model, Regions, and Permissions) relies on type information to perform permission checking, region analysis, and ownership verification. Implementations **shall** emit diagnostics that:

1. Include both type and memory model information when both systems are violated
2. Follow the structured payload format (Annex E §E.5)
3. Provide actionable guidance for resolving permission, region, or ownership errors

#### §7.1.8 Examples (Informative) [type.overview.examples]

**Example 7.1.7.1 (Bidirectional inference with unions):**

```cursive
procedure normalize(input: stream::Line): string \/ io::Error
    [[ io::read |- input.is_open() => result is string \/ io::Error ]]
{
    let trimmed = input.text().trim();        // Context infers `string`
    if trimmed.is_empty() {
        result io::Error::invalid_data("empty line")
    } else {
        result trimmed
    }
}
```

[21] The return type annotation supplies the expected union. The `result` statements rely on automatic widening (`string <: string \/ io::Error`) and on the contractual sequent to document the grant requirements.

**Example 7.1.7.2 (Pointer state integrated with permissions):**

```cursive
procedure swap_current(~!, nodes: list::Iterator@Active, value: Node)
    [[ allocator::unique |- nodes.has_current() => nodes.current() == value ]]
{
    let target: Ptr<Node>@Valid = nodes.current_ptr();
    target.write(value);   // Pointer type ensures state validity and unique access
}
```

[22] The pointer type records both the region (`PointerType`, §7.5) and the modal state (`@Valid`). The contract and grant annotations rely on the type system to guarantee that the write is safe.

---

**Previous**: _(start of clause)_ | **Next**: §7.2 Primitive Types (§7.2 [type.primitive])
