# Cursive Language Specification
## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-7_Initialization-and-Definite-Assignment.md
**Section**: §5.7 Initialization and Definite Assignment
**Stable label**: [decl.initialization]  
**Forward references**: §4.6 [module.initialization], §5.2 [decl.variable], Clause 9 [stmt], Clause 12 [memory]

---

### §5.7 Initialization and Definite Assignment [decl.initialization]

#### §5.7.1 Overview

[1] This subclause specifies how declarations acquire initial values and how the compiler ensures every binding is definitely assigned before use. Cursive provides two assignment operators for bindings: `=` for value assignment and `<-` for reference assignment.

[2] Module-scope initialization interacts with the dependency model in §4.6; block-scoped initialization and reassignment rules integrate with permissions in Clause 12.

#### §5.7.2 Module-Scope Initialization

[1] Module-scope `let`/`var` bindings are initialized exactly once before code in the module executes. Implementations construct the eager dependency graph described in §4.6 and evaluate initializers in topological order.

[2] Cycles in the eager dependency graph are ill-formed (diagnostic E05-701). Implementations shall report every binding in the cycle.

[3] If an initializer fails (e.g., panics), dependent bindings are not evaluated. Each blocked binding receives diagnostic E05-702, identifying the failing dependency.

[4] Module-scope `let` bindings whose initializers are compile-time evaluable become compile-time constants; Clause 7 treats them as `ConstExpr`.

[5] Module-scope `var` bindings evaluate their initializers at module initialization. Subsequent assignments follow the rules in Clause 12.

#### §5.7.3 Block-Scoped Initialization

[1] Block-scoped bindings (`let`/`var`) are initialized when execution reaches the declaration. Control flow shall not use a binding before it has been assigned.

[2] The compiler diagnoses uses of unassigned bindings as E05-703 (“binding may be unassigned here”). This check applies across branches and loop iterations.

#### §5.7.4 Definite Assignment Rules

[1] A binding declared with `let` or `var` must be assigned on every control-flow path before each use. `let` forbids reassignment; `var` permits it.

[2] For conditionals (`if`, `match`), each branch must assign the binding before it is used afterwards.

[3] Before entering a loop, the binding must already have a value. Assignments that occur only inside the loop body do not satisfy definite assignment for code that executes before the first iteration.

[4] Pattern bindings (§5.2–§5.3) shall bind every identifier in the pattern. Omitting a field produces E05-704.

[5] With reference bindings (`<-`), the compiler enforces permissions from Clause 12. Assignments that violate permission rules are diagnosed in that clause.

#### §5.7.5 Reference Assignment

[1] A reference assignment (`<-`) creates a binding without cleanup responsibility. The binding refers to the source expression without acquiring responsibility for invoking its destructor. In contrast, value assignment (`=`) transfers cleanup responsibility to the binding.

[2] Reference assignment combines with `let` or `var`:
```cursive
let x <- expr      // non-responsible, non-rebindable
var x <- expr      // non-responsible, rebindable
```

[3] Bindings created with `<-` shall not invoke destructors when the binding goes out of scope. The referenced value retains its original cleanup responsibility; destructor invocation occurs when the responsible binding exits scope.

[4] Reference bindings shall specify permissions and region annotations consistent with the source expression's type. The definite assignment rules in §5.7.4 apply to both `=` and `<-` forms.

[5] A `var` binding initialized with `<-` may be reassigned to reference a different value using subsequent `<-` operations. Each reassignment replaces the reference without invoking destructors.

[6] Implementations shall enforce permission compatibility between the binding's declared type and the referenced value's permissions. Violations produce diagnostics as specified in Clause 12.

**Example 5.7.5.1 (Value and reference bindings):**
```cursive
var buffer = allocate_buffer()  // value binding with cleanup responsibility
let data <- buffer              // reference binding without cleanup responsibility
// buffer destructor executes at scope exit; data destructor does not execute
```

**Example 5.7.5.2 (Rebindable reference):**
```cursive
var config <- get_default_config()  // reference binding
if use_custom {
    var custom = load_custom_config()
    config <- custom                  // rebind reference
}
// config has no cleanup responsibility; no destructor executes for config
```

**Example 5.7.5.3 (Permission constraints):**
```cursive
let responsible = make_buffer()
let view: Buffer@View <- responsible     // well-formed
var mut_ref: unique Buffer <- responsible // ill-formed: permission mismatch
```

[7] Complete semantics for reference assignment, including permission checking, region lifetime validation, and memory model integration, are specified in Clause 12 [memory].

#### §5.7.6 Reassignment and Drop Order

[1] Reassigning a `let` binding is ill-formed (E05-705). `var` bindings may be reassigned, subject to permission checks.

[2] When a scope exits, bindings are dropped in reverse declaration order. Drop behavior and destructor semantics are governed by Clause 12.

#### §5.7.7 Examples (Informative)

**Example 5.7.7.1 (Module-level constants):**
```cursive
let VERSION: string = "1.2.0"        // compile-time constant
var current_locale = detect_locale()  // runtime initialization
```

**Example 5.7.7.2 (Definite assignment across branches):**
```cursive
var total: i64
if config.has_value {
    total = config.value
} else {
    total = 0
}
use_total(total)  // OK: both branches assign total
```

**Example 5.7.7.3 - invalid (Unassigned variable):**
```cursive
var data: Buffer
if should_allocate {
    data = make_buffer()
}
consume(data)  // error[E05-703]: data may be unassigned
```

**Example 5.7.7.4 - invalid (Pattern mismatch):**
```cursive
let {x, y}: Point = make_point()   // OK
let {x}: Point = make_point()      // error[E05-704]: field y missing
```

### §5.7.8 Conformance Requirements [decl.initialization.requirements]

[1] Implementations shall construct the module initialisation dependency graph of §4.6, diagnose cycles (E05-701), and report blocked bindings when an initialiser fails (E05-702).

[2] Compilers shall enforce definite-assignment analysis for block-scoped bindings, emitting E05-703 when a binding might be uninitialised and E05-705 when a `let` binding is reassigned.

[3] Pattern bindings shall bind every identifier declared in the pattern; omissions shall be diagnosed with E05-704 and prevent the program from compiling.
