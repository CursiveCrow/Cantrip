# Cursive Language Specification

## Clause 4 — Modules

**Clause**: 4 — Modules
**File**: 04-6_Cycles-Initialization.md
**Section**: §4.6 Cycles, Initialization Order, and Diagnostics
**Stable label**: [module.initialization]  
**Forward references**: §2.5 [lex.units], Clause 5 §5.7 [decl.initialization], Annex E §E.2.1 [implementation.algorithms], Annex E §E.5 [implementation.diagnostics]

---

### §4.6 Cycles, Initialization Order, and Diagnostics [module.initialization]

#### §4.6.1 Overview

[1] This subclause defines how module dependencies form a directed graph, how module-level initialization order is derived, and which diagnostics apply to cyclic or ill-formed dependency structures.

[2] It refines §2.5 (module initialisers) and §4.1–§4.5 (module identity, scope, exports) by establishing deterministic rules for dependency classification, initialization execution, failure handling, and diagnostic reporting.

[3] Forward references: Clause 5 §5.7 [decl.initialization], Annex E §E.2.1 [implementation.algorithms], Annex E §E.5 [implementation.diagnostics].

#### §4.6.2 Constraints

[1] **Dependency graph.** Implementations shall construct a directed graph $G = (V, E)$ whose vertices are modules and whose edges record inter-module dependencies introduced by `import` and `public use` declarations as well as module-level references to external bindings.

[2] **Eager edge criterion.** An edge `m → n` is _eager_ when any of the following hold: - a module-level binding or `comptime` block in `m` directly reads a binding exported by `n` while initialising; - `m` performs `public use` of a binding in `n` whose declaration requires initialization (e.g., `let`, `var`, `comptime`-generated constant); - `m` re-exports `n::*` and the expansion includes any eager binding; - a diagnostic obligation in `m` requires evaluating a contract or effect in `n` during module initialization.

[2.1] **Eager/lazy classification algorithm.** Implementations shall classify edges using the following deterministic procedure: 1. Initialize all edges as _lazy_. 2. For each module `m` and each dependency `m → n`:
a. If `m` contains a module-level `let` or `var` binding whose initializer expression references any binding exported by `n`, mark `m → n` as _eager_.
b. If `m` contains a `comptime` block that references any binding exported by `n` during module initialization, mark `m → n` as _eager_.
c. If `m` performs `public use n::item` where `item` is a `let`, `var`, or `comptime`-generated constant, mark `m → n` as _eager_.
d. If `m` performs `public use n::*` and the wildcard expansion includes any binding that would make an edge eager per steps (a)–(c), mark `m → n` as _eager_. 3. Transitive closure: if `m → n` is eager and `n → p` is eager, then `m → p` inherits eager status for initialization ordering purposes (though the direct edge classification remains unchanged). 4. All edges not marked eager remain _lazy_.

[3] **Lazy edge criterion.** An edge `m → n` is _lazy_ when the only interactions are references to declarations that do not require immediate initialization—procedures, types, predicates, or contracts whose bodies are not executed during module initialization. Lazy edges may form cycles.

[3.1] **Lazy edge examples.** The following interactions create lazy edges: - `m` imports `n` and references only procedure signatures (not bodies) - `m` references type declarations from `n` without reading their values - `m` references predicate or contract declarations from `n` without instantiating them - `m` performs `public use n::item` where `item` is a procedure, type, predicate, or contract declaration

[4] **Acyclic eager subgraph.** The subgraph containing all eager edges shall be acyclic. Detection of a cycle in this subgraph triggers diagnostic E04-500.

[5] **Initialization order.** Modules that participate in eager edges shall be initialised according to any topological ordering of the eager subgraph that preserves predecessor relationships.

[6] **Failure propagation.** If a module's initialization fails, every module that depends on it through an eager edge is blocked from initializing. Diagnostic E04-501 shall identify the blocked modules and the failing predecessor.

[7] **Deferred evaluation fidelity.** Implementations may evaluate module initializers lazily provided the observable behaviour matches eager evaluation: each initializer runs at most once, dependencies remain respected, and no additional side effects occur.

[8] **Uninitialised access.** A module shall not read an eager binding from another module before that binding's initializer has completed successfully. Violations emit diagnostic E04-502 referencing the dependency chain.

[9] **Retry safety.** After an initialization failure, implementations may permit a retry during the same compilation provided they reset all side effects of the failed module and its blocked dependents. Retried modules shall re-execute in the established topological order.

#### §4.6.3 Formal Judgments

[ Given: Dependency graph $G = (V, E)$, eager edge set $E_e \subseteq E$ ]

$$
\frac{\exists C \subseteq V : C \neq \emptyset \land \text{Cycle}(C, E_e)}{G \vdash \text{RejectCycle}(C)}\tag{WF-Cycle}
$$

[1] Rule WF-Cycle rejects any set of modules forming a cycle using eager edges.

[ Given: Eager DAG $G_e = (V, E_e)$, total order $\prec$ ]

$$
\frac{v_1 \prec v_2 \prec \dots \prec v_n}{G_e \vdash \text{InitOrder}(v_1,\ldots,v_n)}\tag{WF-InitOrder}
$$

[2] Rule WF-InitOrder establishes a valid initialization order over the eager subgraph.

[ Given: Module $m$, predecessors $\operatorname{Pred}_e(m)$ ]

$$
\frac{\forall p \in \operatorname{Pred}_e(m).\, \text{Initialised}(p)}{\text{SafeInit}(m)}\tag{WF-SafeInit}
$$

[3] Rule WF-SafeInit ensures module $m$ may initialise only after all eager predecessors have completed successfully.

#### §4.6.4 Semantics

##### §4.6.4.1 Graph Construction

[1] The compiler analyses `import` and `public use` statements together with module-level expressions to classify edges per §4.6.2[2]–[3]. Dependencies discovered during `comptime` execution contribute eager edges when their results are required at initialization time.

[2] Lazy edges are retained for completeness but do not influence initialization ordering; they remain available for tooling and diagnostics.

##### §4.6.4.2 Cycle Handling

[3] When WF-Cycle fails, the implementation shall emit diagnostic E04-500. The diagnostic shall include a machine-readable list of modules in the cycle (Annex E §E.5 format) and a textual explanation naming each edge.

[4] Suggested refactorings may accompany the diagnostic as informative notes, but compilation fails for the affected modules until the cycle is removed.

##### §4.6.4.3 Initialization Execution

[5] Initialization executes modules in the order produced by WF-InitOrder. Each module records whether its initializer succeeded, failed, or was skipped due to upstream failure.

[6] On failure, the compiler emits E04-501 for each blocked module, referencing the original error and providing machine-readable dependency metadata. Implementations that support retries shall roll back side effects before reattempting initialization.

[7] Successful initialization marks all eager bindings defined in the module as ready for use. Repeated execution of the same initializer is prohibited.

##### §4.6.4.4 Diagnostics

[8] Diagnostics associated with this clause include:

- **E04-500** — "Module dependency cycle detected." Payload: ordered list of modules forming the cycle, each edge annotated with its eager classification reason.
- **E04-501** — "Module initialisation blocked." Payload: identifier of the blocked module, failing predecessor, and dependency path.
- **E04-502** — "Access to uninitialised module binding." Payload: requesting module, target binding, outstanding predecessors.
- **E04-503** — "Misclassified lazy dependency." Payload: module pair and evidence that the interaction required eagerly initialised data.

[9] Structured payloads shall follow Annex E §E.5 so that diagnostics are machine-readable and suitable for tooling consumption.

#### §4.6.5 Examples

**Example 4.6.5.1 (Valid ordering):**

```text
module analytics::core       // no eager initialisers
module analytics::config     // eager: const CONFIG = load_config()
module analytics::reporting  // eager: reads CONFIG during initialisation
```

[1] The eager subgraph forms the order `analytics::config → analytics::reporting`; `analytics::core` has only lazy edges and can initialise at any point.

**Example 4.6.5.2 - invalid (Cycle):**

```text
module input::parser      // eager edge: builds table using output::formatter
module output::formatter  // eager edge: initialises inverse map using input::parser
```

[2] The eager cycle `parser ↔ formatter` triggers E04-500 with the cycle list `[input::parser, output::formatter]`.

**Example 4.6.5.3 - invalid (Uninitialised access):**

```text
module data::cache
import data::tables
let DEFAULT = tables::DEFAULT_TABLE  // eager edge
```

[3] If `data::tables` fails to initialise `DEFAULT_TABLE`, `data::cache` receives E04-501 and any attempt to read `DEFAULT` produces E04-502.

#### §4.6.6 Integration

[1] Annex E §E.2.1 shall define algorithms for constructing the eager dependency graph, detecting cycles, and orchestrating initialization retries.

[2] Clause 5 §5.7 shall treat module-level bindings as defined only after WF-SafeInit succeeds for their module.

[3] Annex E §E.5 shall include structured diagnostic schemas for E04-500–E04-503 to ensure consistent reporting across tooling.

#### §4.6.7 Interaction with Type Checking [module.type.integration]

[4] Type-checking obligations for exported types, generic instantiation, and contract verification are specified in Clause 7 (§7.1.5). This subclause records only the dependency ordering: module initialization (including eager bindings) SHALL complete before Clause 7 begins semantic analysis so that exported declarations have concrete definitions. Diagnostic responsibilities for cross-module visibility, internal-type access (E04-700), and contract readiness follow the rules in §7.1.5 and §7.3.

**Previous**: §4.5 Qualified Name Resolution (§4.5 [module.qualified]) | **Next**: §4.A Diagnostics and Indices (§4.A [module.index])
