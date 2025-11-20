Migrating content from §§14.1–14.6, 23.4, 29.1.5, 29.2.5, 30.2–30.3, 31.3, and 32.5 of the original draft to §6.5 Program Execution using the **System Definition** template (with §6.5.1 using the **Language Construct** template).

---

## 6.5 Program Execution

<u>**Definition**</u>

A **program execution** is the evolution of a well-formed, link-closed Cursive program image under the Cursive Abstract Machine (CAM), starting from the invocation of the entry procedure `main` with a runtime-constructed `Context` value, and ending either in termination (with an exit status) or divergence.

Formally, let:

* $\mathcal{P}$ be the set of **well-formed programs** as defined by the declaration, type, and module rules in Parts 3–5.
* $\mathcal{E}$ be the set of **execution environments** (host OS process environment: command-line arguments, environment variables, underlying OS process, ABI, etc.).
* $\mathcal{C}$ be the set of **abstract machine configurations**, as defined by the core small-step semantics in Annex I. 
* $\mathcal{O}$ be the set of **observable outcomes**:

  * $\textsf{Exit}(c)$, where $c \in \mathbb{Z}_{32}$ is the exit status returned by `main` or by an explicit `System.exit` call.
  * $\textsf{Abort}(r)$, an abnormal termination due to a panic-abort or other fail-fast mechanism.
  * $\textsf{Diverge}$, non-termination (infinite execution).

For each **conforming program** $P \in \mathcal{P}$ and execution environment $E \in \mathcal{E}$, program execution is modeled as a (possibly infinite) sequence of abstract machine configurations:
[
C_0(P, E), C_1, C_2, \dots
]
with $C_0(P, E)$ the **initial configuration**, and for each $i$, a transition relation
[
C_i \longrightarrow C_{i+1}
]
given by the core operational semantics. 

We define the **outcome set** of $(P, E)$ as:
[
\mathsf{Outcome}(P, E) \subseteq \mathcal{O}
]
such that:

* $\textsf{Exit}(c) \in \mathsf{Outcome}(P, E)$ iff there exists a finite maximal execution trace ending in a configuration where:

  * The `main` procedure returns an `i32` value $c$ (§23.4.3) or `ctx.sys.exit(code)` selects a `code` value, and
  * Control does not proceed to any further user code.
* $\textsf{Abort}(r) \in \mathsf{Outcome}(P, E)$ iff execution reaches a configuration in which the implementation is required to **abort the process**, e.g.:

  * Double-panic during unwinding (§29.2.5.3). 
  * Panic crossing an FFI boundary marked `[[unwind(abort)]]` (§32.5.2). 
* $\textsf{Diverge} \in \mathsf{Outcome}(P, E)$ if there exists an infinite sequence of transitions, i.e. execution never reaches a terminal configuration.

The **entry configuration** $C_0(P, E)$ is determined as follows:

1. A link-closed image of $P$ is constructed according to the translation phases of §8 and the module system in §11.
2. All static storage objects (§29.1.5) are allocated. 
3. The dynamic module initialization schedule is computed and executed as specified in §14; all dynamic initializers that are reachable in the eager dependency graph are run before `main` is invoked.
4. A fresh `Context` record value is constructed by the runtime, containing the root capability implementations for the process (§30.2.2). 
5. The initial thread’s call stack is seeded with a call frame for:
   [
   \textsf{main}(ctx : Context) : i32
   ]
   as defined in §6.5.1.

<u>**Static Semantics**</u>

[Implicitly derived]

Let $WF(P)$ denote the predicate “program $P$ is well-formed” in the sense of the typing, declaration, and module rules (including all safety constraints). 

Define the predicate $\mathsf{HasMain}(P)$ as:

* There exists exactly one **procedure declaration with a body** at module scope named `main` with the type `(Context) -> i32`, `public` visibility, and no `comptime` modifier (§23.4.1, §23.2.1, §30.2.1).

Formally:
[
\mathsf{HasMain}(P) \iff
\exists! d.; d \in \mathsf{Decls}(P) \wedge \mathsf{name}(d) = \texttt{"main"} \wedge \mathsf{sig}(d) = \texttt{public procedure main(ctx: Context): i32} \wedge \mathsf{hasBody}(d)
]

A program $P$ is **executable** iff:
[
WF(P) \wedge \mathsf{HasMain}(P)
]

The typing judgment for `main` is:
[
\Gamma_0 \vdash \textsf{main} : Context \to i32
]
for the root environment $\Gamma_0$ at the top level of the root module.

The implementation **MUST** check, during semantic analysis or link-time, that any artifact being built as an **executable** satisfies $\mathsf{HasMain}(P)$. 

If an artifact is explicitly configured as a **library** (non-executable), the implementation **MUST NOT** require $\mathsf{HasMain}(P)$ and **MUST NOT** emit `E-DEC-2430` solely due to the absence of `main`. [Implicitly derived]

<u>**Dynamic Semantics**</u>

[Implicitly derived, integrating §§14, 23.4, 29.1.5, 29.2.5, 30.2–30.3, 31.3, 32.5]

Define an **execution judgment**:
[
P, E \Downarrow o
]
where $o \in \mathcal{O}$.

The dynamic semantics of program execution are constrained as follows:

1. **Initialization Phase.**
   Before the first user instruction in `main` is executed, the runtime **MUST**:

   * Allocate all objects with static storage duration, as per §29.1.5. 
   * Evaluate all **static initializers** at compile time and embed their values in the image (§14.1). 
   * Evaluate all **dynamic initializers** in a topological order of the eager dependency graph $G_e$ (§14.3–14.4).

   If a dynamic initializer panics, the semantics of initialization failure in §14.5 (module poisoning) apply; the process **MAY** still continue and invoke `main`, but any subsequent access to a poisoned module or any module with an eager dependency path from it **MUST** panic. 

2. **Root Context Construction.**
   The runtime constructs a `Context` value `ctx` such that:

   * Each field (`fs`, `net`, `sys`, `heap`) contains a valid implementation of the corresponding capability (`FileSystem`, `Network`, `System`, `HeapAllocator`) as defined in §30.3 and Appendix D.
   * `ctx` is the unique *root* of capability for this process: all other capabilities reachable in the program at runtime **MUST** be derived (attenuated) from this value or from values supplied by FFI or host tooling, in accordance with the **No Ambient Authority** rules of §30.1. 

3. **Entry to `main`.**
   The initial thread’s first call frame is:

   [
   \textsf{main}(ctx)
   ]

   The body of `main` executes according to the expression, statement, responsibility, and capability semantics in Parts 3–6. In particular, RAII and `Drop` semantics govern destruction of all responsible bindings in `main` and any procedures it calls (§29.2.5).

4. **Termination Paths.**
   Execution can terminate in the following defined ways (details in §6.5.2):

   * **Normal Return from `main`.**
     `main` evaluates to an `i32` value $c$ and returns; the process exits with an exit status derived from $c$.
   * **Explicit `System.exit`.**
     A call `ctx.sys.exit(code)` is evaluated on some thread; this call has type `!` and **does not return** (§30.3.3). The process is terminated immediately with an exit status derived from `code`. 
   * **Panic Termination.**
     A panic propagates to the top of the main thread without being handled; the runtime unwinds stacks as specified in §29.2.5.3, then terminates the process with an implementation-defined non-zero exit status (§32.5.2 for FFI boundaries).

5. **Concurrency.**

   * Path 1 (`parallel`) provides structured parallelism with **static join**; no `JobHandle` can remain unjoined when control leaves the `parallel` block (§31.2). This ensures that all Path 1 work is complete before the surrounding scope (including `main`) can exit.
   * Path 2 (`System.spawn`) creates additional threads whose lifetime may extend beyond the lexical scope that spawned them. A spawned `Thread<T>@Spawned` can be joined or detached via the `Thread` API (§31.3). At process termination, any remaining live threads are terminated as part of the host OS process teardown; the exact mechanism is implementation-defined but MUST NOT violate the safety properties of Annex I (no undefined behavior in safe code).

<u>**Memory & Layout**</u>

Program execution interacts with memory according to the storage duration categories in §29.1.5. 

Let $\mathsf{Objs}*{static}(P)$ denote the set of objects with static storage duration and $\mathsf{Objs}*{auto}(P)$, $\mathsf{Objs}*{region}(P)$, $\mathsf{Objs}*{heap}(P)$ the sets of automatic, region, and dynamic-storage objects, respectively.

The following conditions hold for any execution trace:

1. **Static Storage Lifetime Invariant.**
   [
   \forall o \in \mathsf{Objs}_{static}(P).; \textsf{allocated}(o, C_0(P,E)) \land \textsf{live}(o, C_i) \text{ for all finite } i
   ]
   i.e., static objects are allocated before `main` and remain allocated until after process termination. Their cleanup is performed as part of program exit according to the RAII rules, but the memory underlying them is conceptually deallocated only after the final configuration.

2. **Stack (Automatic) Storage.**
   For any automatic object $o$ allocated in a frame $F$:
   [
   \forall i.; \textsf{live}(o, C_i) \implies \textsf{frame}(o) = F \land F \in \textsf{stack}(C_i)
   ]
   and $o$ is destroyed (via `Drop`) when $F$ is popped, in strict LIFO order (§29.2.5.2).

3. **Region and Heap Storage.**
   Region and heap objects follow the lifetime and provenance invariants in §§29.5 and 29.6; program execution **MUST** not produce a state that violates provenance constraints (e.g., region pointer escape), except as part of UVB inside an `unsafe` block or FFI calls.

<u>**Concurrency & Safety**</u>

Program execution MUST preserve the global safety guarantees:

* **No Undefined Behavior in Safe Code.**
  For any execution trace of a well-typed program that does not perform UVB operations (§6.1, Appendix I), the state never reaches a “stuck” configuration; all transitions are defined by the operational semantics.

* **Data-Race Freedom.**
  The concurrency model (Path 1 `parallel`, Path 2 `System.spawn` with `Thread` and `Mutex`) is constrained so that well-typed code cannot exhibit data races (§31, Appendix I).

* **Capability Safety.**
  All effectful operations (I/O, threading, heap allocation, environment inspection) **MUST** be mediated by capabilities reachable from the root `Context`, preserving the **No Ambient Authority** principles in §30.1.

<u>**Complexity & Limits**</u>

[Implicitly derived from Appendix H & F]

Implementations **MAY** enforce resource limits on program execution, including but not limited to:

* Maximum stack depth or recursion depth.
* Maximum heap allocation.
* Maximum number of concurrently running threads.
* Maximum size of input source files (affecting compilation, not runtime).

These limits are classified as **Implementation-Defined Behavior** (§6.1.3, Appendix H). A conforming implementation **MUST**:

1. Document its concrete choices in the Conformance Dossier (§6.4, Appendix C).
2. Ensure that exceeding a resource limit results in a **defined** outcome (e.g., panic and process abort or clean termination with a documented non-zero exit status), not undefined behavior.

<u>**Verification & Invariants**</u>

Let $\mathsf{ExecStates}(P, E)$ denote the set of all states reachable during any execution of $(P, E)$.

Key invariants:

1. **Root Capability Origin.**
   [
   \forall s \in \mathsf{ExecStates}(P, E).; \forall cap.; \textsf{isCapability}(cap, s) \Rightarrow \textsf{origin}(cap) \in {ctx} \cup \textsf{FFIOrigins}
   ]
   where `ctx` is the root `Context` passed to `main` and `FFIOrigins` denotes capabilities imported from FFI or test harnesses in ways allowed by the specification. This formalizes the “No Ambient Authority” requirement.

2. **Entrypoint Uniqueness.**
   [
   WF(P) \Rightarrow |{ d \in \mathsf{Decls}(P) \mid \mathsf{name}(d) = \texttt{"main"} \wedge \mathsf{isEntryPoint}(d)}| \leq 1
   ]
   where $\mathsf{isEntryPoint}(d)$ holds iff $d$ matches the signature constraints in §6.5.1.

3. **Progress (High-Level).**
   For any conforming program $P$ and environment $E$:
   [
   WF(P) \Rightarrow \forall s \in \mathsf{ExecStates}(P, E).; s \text{ is not stuck}
   ]
   i.e., either there exists $s'$ with $s \longrightarrow s'$ or $s$ is a terminal configuration corresponding to some $o \in \mathsf{Outcome}(P, E)$, per the Progress theorem in Annex I. 

<u>**Examples**</u>

**Example 1 (Minimal Executable Program).**

```cursive
// main.cursive
import std::io

public procedure main(ctx: Context): i32 {
    let out = ctx.fs.open("log.txt", WriteOnly)
    out.write_line("Hello from Cursive!")
    result 0
}
```

**Example 2 (Program Using `System.exit`).**

```cursive
public procedure main(ctx: Context): i32 {
    let code = ctx.sys.get_env("EXIT_CODE")

    if code == "fail" {
        // Terminates process immediately; does not return.
        ctx.sys.exit(1)
    }

    // If we get here, exit with success.
    result 0
}
```

**Example 3 (CLI Session and Exit Status).**

A typical host shell session illustrating program execution and exit status propagation:

```text
$ cursivec build main.cursive -o main
[info] Parsing...
[info] Type checking...
[info] Linking...
[ok] Built executable: ./main

$ ./main
$ echo $?
0

$ EXIT_CODE=fail ./main
$ echo $?
1
```

**Example 4 (Dynamic Initialization Before `main`).**

```cursive
// config.cursive
import std::fs

// Dynamic initializer that uses capabilities from Context
var LOG_PATH: string@Managed = init_log_path()

procedure init_log_path(): string@Managed {
    // In dynamic initialization, the runtime injects a Context internally,
    // so this procedure can call capability-bearing functions.
    result "/var/log/app.log"
}

// main.cursive
import config

public procedure main(ctx: Context): i32 {
    let out = ctx.fs.open(config.LOG_PATH, WriteOnly)
    out.write_line("Initialized correctly")
    result 0
}
```

The runtime will:

1. Construct the `Context` and perform dynamic initialization of `config.LOG_PATH`.
2. Only after all dynamic initializers complete successfully does it call `main(ctx)`.

**Example 5 (Memory Diagram – High Level).**

At the moment `main` begins executing, the abstract memory picture is:

```text
+------------------------+
| Static Data            |
| - config.LOG_PATH      |  (static storage)
+------------------------+
| Heap (ctx.heap)        |  (dynamic allocations)
+------------------------+
| Stack (main thread)    |
| - main call frame      |
|     arg: ctx: Context  |
+------------------------+
```

---

### 6.5.1 Program Entry (`main`)

[Language Construct Archetype]

<u>**Definition**</u>

The **program entry** is a distinguished procedure declaration named `main` with a specific capability-bearing signature. It is a specialized instance of a `procedureDeclaration` (§23.2.1, Appendix A).

The syntactic form of an entrypoint declaration is given in W3C-style EBNF:

```ebnf
main_declaration ::=
    "public" "procedure" "main"
    "(" "ctx" ":" "Context" ")"
    ":" "i32"
    [ <contract_clause> ]
    <main_body>

main_body ::=
      <block_stmt>
    | "=" <expression> ";"
```

Where:

* `<contract_clause>` is the optional contract sequent `[[ ... ]]` as in §23.2.2 and §27.1.
* `<block_stmt>` and `<expression>` are as defined in the statements and expressions chapters (§24–25, Appendix A).

A `main_declaration` is thus a `procedureDeclaration` with the following fixed properties:

* Visibility: `public` (no other visibility is permitted).
* Name: `main`.
* Parameters: exactly one parameter `ctx: Context`.
* Return type: `i32`.
* No `comptime` modifier.
* Not `extern`.

<u>**Constraints & Legality**</u>

The following static constraints are **normative**:

1. **Uniqueness Constraint.**
   For any program artifact being compiled as an **executable**, there **MUST** exist exactly one `main_declaration` in the link-closed program image.

   Formally:
   [
   \mathsf{Mains}(P) =
   { d \in \mathsf{Decls}(P) \mid \mathsf{matchesMainSyntax}(d)}
   ]
   then:
   [
   |\mathsf{Mains}(P)| = 1
   ]

   If $|\mathsf{Mains}(P)| \neq 1$, the program is ill-formed as an executable and **MUST** be diagnosed with `E-DEC-2430`.

2. **Signature Exactness Constraint.**
   The unique `main` entrypoint **MUST** have exactly the type and modifiers implied by:

   ```cursive
   public procedure main(ctx: Context): i32
   ```

   This entails:

   * No additional parameters.
   * No missing `ctx: Context` parameter.
   * The parameter name **MAY** differ from `ctx` in the concrete syntax, but the parameter type **MUST** be exactly `Context`.
   * The return type **MUST** be exactly `i32`.
   * The declaration **MUST NOT** be marked `comptime`.
   * The declaration **MUST NOT** be `extern`.
   * The declaration **MUST** have a body (either block or expression body); a semicolon-only declaration is not permitted for `main`.

   Any violation of these constraints **MUST** be diagnosed as `E-DEC-2431`.

3. **Context Parameter Role Constraint.**
   The parameter of type `Context` is the **root of capability** for the program. The implementation **MUST NOT** treat any other value as the root of capability, and **MUST** construct this parameter value at runtime as specified in §30.2.2.

**Diagnostics**

| Code         | Severity | Description                                                                                       |
| :----------- | :------- | :------------------------------------------------------------------------------------------------ |
| `E-DEC-2430` | Error    | Missing or duplicate `main` procedure in an executable artifact (violates uniqueness constraint). |
| `E-DEC-2431` | Error    | Invalid `main` signature. Must be equivalent to `public procedure main(ctx: Context): i32`.       |

(Existing codes unified and clarified for Draft 3; sourced from §§23.4.1–23.4.4 and Appendix B.3.5.)

<u>**Static Semantics**</u>

Let $\Gamma_0$ be the top-level typing environment for the root module of the program.

If `main` is declared in the root module:

[
\Gamma_0 \vdash \textsf{main} : Context \to i32
]

The typing rule for the entrypoint is:

[
\frac{
\Gamma_0 \vdash \textsf{main} : Context \to i32
}{
\Gamma_0 \vdash \textsf{EntryPoint}(P, \textsf{main})
}
\tag{T-Main-Entry}
]

Furthermore, the body of `main` is type-checked as a normal procedure body:

[
\Gamma_0,, ctx : Context \vdash \textsf{body(main)} : i32
]

i.e., all paths through the body must produce an `i32` value (via `result` or tail expression), subject to the general control-flow and type rules (§22, §24–25).

<u>**Dynamic Semantics**</u>

At runtime, after initialization (§14), the Abstract Machine invokes the entrypoint:

[
P, E \longrightarrow^* C_{\textsf{main}} \quad\text{with}\quad C_{\textsf{main}} = \textsf{pushFrame}(\textsf{main}, ctx)
]

The `main` procedure behaves like any other procedure:

* Entry: parameters are bound; local variables allocated as needed.
* Execution: statements and expressions in the body are evaluated as per §§24–25.
* Exit:

  * On `result e`, the expression `e` is evaluated to a value $c$ of type `i32` which becomes the **program exit code** for the normal-return path (see §6.5.2).
  * If control reaches the end of the body without an explicit `result`, the semantics are the same as `result 0` (implicit success), unless prohibited by the control-flow rules of §22.3. [Implicitly derived]

<u>**Concurrency & Safety**</u>

The entrypoint `main` executes on the **initial thread**. All capabilities needed for concurrency (`ctx.sys.spawn`, `Mutex`, etc.) are reachable from the `Context` parameter (§31.3).

Safety invariants:

* No effectful operation in `main` may occur without a corresponding capability argument, enforcing No Ambient Authority.
* Any `unsafe` operations in `main` (e.g., FFI calls, raw pointer dereferences) remain subject to the UVB rules of §6.1 and must be enclosed in `unsafe` blocks (§29.6, §32.2).

<u>**Verification & Invariants**</u>

A conforming implementation **MUST** ensure:

1. **Uniqueness Invariant.**
   [
   WF(P) \wedge \mathsf{Executable}(P) \Rightarrow |\mathsf{Mains}(P)| = 1
   ]

2. **Type Invariant.**
   [
   \forall d \in \mathsf{Mains}(P).; \Gamma_0 \vdash d : Context \to i32
   ]

3. **Capability Root Invariant.**
   Let `ctx` be the parameter bound at the entry of `main`. Then at any runtime state $s$ reachable from the entrypoint:
   [
   \textsf{isRootContext}(ctx, s)
   ]
   and:
   [
   \forall cap.; \textsf{isCapability}(cap, s) \Rightarrow \textsf{origin}(cap) \preceq ctx
   ]
   where $\preceq$ is the “attenuation” relation between capabilities (§30.4). 

<u>**Examples**</u>

**Example 1 (Valid `main`).**

```cursive
public procedure main(ctx: Context): i32 {
    ctx.sys.time()        // uses System capability
    ctx.fs.get_metadata(".")
    result 0
}
```

**Example 2 (Invalid Signature – Missing Context).**

```cursive
// COMPILE-TIME ERROR (E-DEC-2431)
public procedure main(): i32 {
    result 0
}
```

**Example 3 (Invalid – Multiple `main` Declarations).**

```cursive
// file a.cursive
public procedure main(ctx: Context): i32 {
    result 0
}

// file b.cursive
public procedure main(ctx: Context): i32 {
    result 1
}

// Linking as executable:
// ERROR (E-DEC-2430): Missing or duplicate `main` procedure.
```

**Example 4 (CLI Build Failure).**

```text
$ cursivec build lib.cursive -o app
E-DEC-2431: Invalid signature for `main`. Must be `public procedure main(ctx: Context): i32`.
 --> lib.cursive:12:1
  |
12| public procedure main(): i32 { ... }
  | ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
```

---

### 6.5.2 Termination

<System Definition Archetype>

<u>**Definition**</u>

**Program termination** is the transition from a live execution of the Cursive Abstract Machine to a terminal state in which no further user code executes and control is returned to the host environment with an exit status or an abort reason.

Termination outcomes are:

* **Normal Termination**: $\textsf{Exit}(c)$ for some $c \in \mathbb{Z}_{32}$ derived from `main`’s return value (§23.4.3) or explicit `System.exit`.
* **Panic Termination**: $\textsf{Abort}(r)$ for some reason $r$ (e.g., contract failure, panic abort across FFI).
* **Divergence**: $\textsf{Diverge}$ (not a termination, but a possible outcome).

<u>**Dynamic Semantics**</u>

1. **Normal Return from `main`.**

   If execution of `main(ctx)` returns a value $c$ of type `i32` without panic:

   [
   P, E \Downarrow \textsf{Exit}(c)
   ]

   The program **MUST**:

   * Execute all remaining destructors for local and temporary objects in `main` and any still-active frames (if the implementation performs a structured shutdown) according to §29.2.5. 
   * Map the internal `i32` exit status $c$ to the host environment’s process exit code as an **Implementation-Defined Behavior** (IDB). The mapping function $\mathsf{MapExit}: \mathbb{Z}_{32} \to \textsf{HostExitCode}$ **MUST** satisfy:
     [
     \mathsf{MapExit}(0) = 0 \quad\text{and}\quad c \neq 0 \Rightarrow \mathsf{MapExit}(c) \neq 0
     ]
     but may otherwise be arbitrary and **MUST** be documented in the Conformance Dossier.

2. **Explicit `System.exit`.**

   The method:

   ```cursive
   procedure exit(self: const, code: i32): !;
   ```

   on the `System` record (§30.3.3) provides a **process control** primitive. 

   When a call `ctx.sys.exit(code_expr)` is evaluated and `code_expr` evaluates to an `i32` value $c$, the implementation **MUST**:

   * Immediately begin process termination; no further user code in the calling thread executes after the call site.
   * The runtime **MAY** perform limited cleanup (flushing I/O buffers, releasing OS handles), but is not required to run user-level destructors for all live objects. This is a *fail-fast* operation and should be treated as semantically equivalent to an immediate termination with exit status derived from $c$. [Implicitly derived]
   * The resulting outcome is:
     [
     P, E \Downarrow \textsf{Exit}(c)
     ]
     with the exit code subject to the same mapping $\mathsf{MapExit}$ as in normal termination.

   In particular, `System.exit` is **not** required to unwind stacks or run `Drop` for every live binding; code that requires RAII cleanup on shutdown **SHOULD** return normally from `main` instead of calling `System.exit`. [Implicitly derived]

3. **Panic Termination.**

   When a panic occurs during execution:

   * The runtime **MUST** perform stack unwinding, invoking destructors for all responsible bindings in each frame as it unwinds (§29.2.5.3). 
   * If a destructor panics during unwinding (double panic), the runtime **MUST** immediately abort the process, resulting in outcome:
     [
     P, E \Downarrow \textsf{Abort}(r_{\text{double-panic}})
     ]
   * If a panic reaches the boundary of an exported FFI function marked `[[unwind(abort)]]`, the runtime **MUST** abort the process per §32.5.2; if marked `[[unwind(catch)]]`, the panic is caught and translated into a fallback value and does **not** terminate the process. 

   For a panic that propagates to the top of the main thread without being caught:

   * The implementation **MUST** terminate the process with a **non-zero** exit code.
   * The specific exit code **MAY** be implementation-defined (IDB) but **MUST** be documented in the Conformance Dossier (e.g., “uncaught panic exit code = 101”).

4. **Divergence.**

   If execution never reaches a terminal configuration (e.g., infinite loop in `main`, or threads that never join or exit), the outcome is:
   [
   \textsf{Diverge} \in \mathsf{Outcome}(P, E)
   ]

   Divergence is defined behavior; the runtime is permitted to continue running indefinitely. If an external watchdog or OS resource limit terminates the process, that termination is outside the Cursive semantic model and is classified as Implementation-Defined Behavior or environmental behavior, not undefined behavior in the language.

<u>**Concurrency & Safety**</u>

At the point of termination:

* All safety guarantees continue to hold until the final state; there is no “undefined behavior at shutdown” in safe code.
* For Path 1 concurrency, static join semantics guarantee that all jobs spawned by `parallel` expressions have completed before the surrounding scope (including `main`) can exit; no special termination logic is required.
* For Path 2 threads:

  * If a thread is joined, it terminates independently and returns its result to the joining thread.
  * If a thread is detached, it may still be running at process termination. The host OS teardown is responsible for reclaiming its resources; the language guarantees that no type or memory invariants are violated by such teardown. The ordering between thread completion and process exit for detached threads is unspecified but bounded by OS guarantees (USB).

<u>**Verification & Invariants**</u>

Let $o \in \mathsf{Outcome}(P, E)$.

1. **Exit Status Invariant.**
   [
   o = \textsf{Exit}(c) \Rightarrow \left( c = v_{\text{main}} \lor c = v_{\text{sys.exit}} \right)
   ]
   where $v_{\text{main}}$ is the value returned by `main` and $v_{\text{sys.exit}}$ is the value passed to `System.exit` on the terminating call.

2. **Panic Non-Zero Exit Invariant.**
   If $o = \textsf{Abort}(r)$ due to an uncaught panic on the main thread, the mapped host exit status $\mathsf{MapExit}(c_r)$ **MUST** be non-zero.

3. **Destructor Execution Invariant.**
   For any scope that is exited via normal control-flow or panic unwinding (not via `System.exit`):

   * All responsible bindings in that scope are eventually dropped exactly once, in LIFO order, as required by §29.2.5.1–2.

<u>**Examples**</u>

**Example 1 (Normal Return vs. Panic).**

```cursive
public procedure main(ctx: Context): i32 {
    if ctx.sys.get_env("MODE") == "panic" {
        panic("fatal error")      // will unwind and then abort process
    }

    result 0                      // normal termination
}
```

Host session (illustrative):

```text
$ MODE=normal ./app
$ echo $?
0

$ MODE=panic ./app
thread 'main' panicked at 'fatal error', src/main.cursive:4:9
$ echo $?
101    # Implementation-defined non-zero panic code
```

**Example 2 (`System.exit` Bypassing Destructors).**

```cursive
record Resource {
    procedure drop(~!) {
        // flush logs, close file handles, etc.
        ctx.sys.get_env("...") // uses capabilities
    }
}

public procedure main(ctx: Context): i32 {
    let r = Resource { /* ... */ }

    // LOGIC ERROR: This bypasses r's drop, by design.
    ctx.sys.exit(42)
}
```

A conforming implementation may log or flush some runtime-managed resources, but is **not** required to run user `drop` logic for `r` after `System.exit(42)` is called.

**Example 3 (Termination with Background Threads).**

```cursive
public procedure main(ctx: Context): i32 {
    let worker = ctx.sys.spawn(() -> i32 {
        // long-running work
        loop {
            // ...
        }
    })

    // Do some quick work, then exit successfully.
    result 0
}
```

At process termination:

* The spawned worker thread is still running.
* The OS tears down the process and all associated threads.
* Within the language model, no memory unsafety occurs in safe code; the exact timing of worker termination relative to process exit is unspecified but bounded by OS semantics.

**Example 4 (Conformance Dossier Entry for Termination Behavior).**

An implementation’s Conformance Dossier might record:

```json
{
  "termination": {
    "panic_exit_code": 101,
    "double_panic_behavior": "abort",
    "system_exit_cleanup": "os_handles_only",
    "exit_code_mapping": "host_exit = (code & 0xFF)"
  }
}
```

This artifact formally documents the implementation-defined aspects of termination behavior required by §6.1.3 and this clause.

---

This completes the migration and formalization of **§6.5 Program Execution**, including **§6.5.1 Program Entry (`main`)** and **§6.5.2 Termination**, into the Draft 3 ISO/IEC-conformant structure.
