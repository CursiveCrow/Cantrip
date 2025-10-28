# Part II: Type System - §12. Modals: State Machines as Types

**Section**: §12 | **Part**: Type System (Part II)
**Previous**: [Generics](07_Generics.md) | **Next**: [Functions](../03_Functions/01_FunctionBasics.md)

---

**Definition 12.1 (Modal):** A modal is a type with an explicit finite state machine, where each state @Sᵢ can have different data fields and the compiler enforces valid state transitions. Formally, a modal M = (S, Σ, δ, s₀) where S is a finite set of states, Σ is the alphabet of procedures, δ: S × Σ → S is the transition function, and s₀ ∈ S is the initial state. Modals bring state machine verification into the type system, ensuring compile-time correctness for stateful resources and business logic.

## 12. Modals: State Machines as Types

### 12.1 Overview

**Key innovation/purpose:** Modals make state machines first-class types with compile-time verification, ensuring that state-dependent operations are only called in valid states and all state transitions are explicit and type-checked.

**When to use modals:**
- Resources with lifecycles (files, database connections, network sockets)
- Stateful business logic (orders, shopping carts, payment flows)
- Protocol implementations (TCP handshakes, authentication state)
- Types where "state matters" for correctness
- API wrappers requiring initialization/cleanup sequences
- Workflow engines and state-based processes
- Transaction management (pending, committed, rolled back)

**When NOT to use modals:**
- Simple data with no state machine → use records (§8)
- Discrete alternatives without transitions → use enums (§9)
- Mathematical types (vectors, matrices, points) → use records
- Configuration objects → use records
- Collections → use Vec, HashMap, etc.
- When state doesn't affect available operations

**Relationship to other features:**
- **Records**: Modals extend records with state-dependent fields and procedures
- **Enums**: Modals are like enums but with explicit transitions and state-dependent data
- **Procedures**: Modal procedures specify state transitions via `@State` annotations
- **Contracts**: State invariants verified via `requires`/`ensures`
- **Linear types**: Modal values use linear state transfer (no copying state)
- **Verification**: Reachability and exhaustiveness checked statically

### 12.2 Syntax

#### 12.2.1 Concrete Syntax

**Modal declaration:**
```ebnf
ModalDecl    ::= "modal" Ident GenericParams? "{" ModalItem* "}"
ModalItem    ::= StateDecl | CommonDecl | ModalProcedure
StateDecl    ::= "state" Ident "{" FieldList "}"
CommonDecl   ::= "common" "{" FieldList "}"
ModalProcedure ::= "procedure" Ident "@" StatePattern "(" ParamList ")" "->" "@" Ident ContractClauses? Block
StatePattern ::= Ident
               | "(" Ident ("|" Ident)* ")"    // Union of states
```

**State annotations:**
```ebnf
StateAnnot   ::= Type "@" Ident
```

**Examples:**
```cantrip
modal File {
    state Closed {
        path: String
    }

    state Open {
        path: String
        handle: FileHandle
        position: usize
    }

    procedure open@Closed() => @Open
        uses fs.read
    {
        let handle = FileSystem.open(self.path)?
        Open {
            path: self.path,
            handle: handle,
            position: 0,
        }
    }

    procedure close@Open() => @Closed
        uses fs.close
    {
        self.handle.close()
        Closed { path: self.path }
    }
}
```

#### 12.2.2 Abstract Syntax

**Modal types:**
```
τ ::= modal M { @S₁, ..., @Sₙ }    (modal type with states)
    | M@S                           (modal in specific state)
```

**State machine:**
```
M = (S, Σ, δ, s₀, Fields)
where:
  S = {@S₁, ..., @Sₙ}              (finite set of states)
  Σ = {m₁, ..., mₖ}                (procedures)
  δ : S × Σ → S                     (transition function)
  s₀ ∈ S                            (initial state)
  Fields: S → (Name × Type)*        (state-dependent fields)
```

**Transition syntax:**
```
δ ::= m@Sᵢ(...) => @Sⱼ { e }      (transition from Sᵢ to Sⱼ via m)
```

#### 12.2.3 Basic Examples

**Simple lifecycle:**
```cantrip
modal Connection {
    state Disconnected {
        host: String
        port: u16
    }

    state Connected {
        host: String
        port: u16
        socket: TcpSocket
    }

    procedure connect@Disconnected() => @Connected
        uses net.tcp
    {
        let socket = TcpSocket.connect(self.host, self.port)?
        Connected {
            host: self.host,
            port: self.port,
            socket: socket,
        }
    }

    procedure disconnect@Connected() => @Disconnected
        uses net.tcp
    {
        self.socket.close()
        Disconnected {
            host: self.host,
            port: self.port,
        }
    }
}
```

**Usage:**
```cantrip
let conn = Connection.new("localhost", 8080)  // Connection@Disconnected
let conn = conn.connect()?                     // Connection@Connected
let data = conn.read(1024)?                    // OK in Connected state
let conn = conn.disconnect()                   // Back to Disconnected
```

### 12.3 Static Semantics

#### 12.3.1 Well-Formedness Rules

**Modal declaration:**
```
[WF-Modal]
∀i. state @Sᵢ { fields } well-formed
∀j. procedure mⱼ@S_src => @S_tgt well-formed
@S_src, @S_tgt ∈ S
────────────────────────────────────────────
modal M { @S₁, ..., @Sₙ; m₁, ..., mₖ } well-formed
```

**State declaration:**
```
[WF-State]
Γ ⊢ T₁ : Type    ...    Γ ⊢ Tₙ : Type
field names distinct
────────────────────────────────────────
state @S { f₁: T₁; ...; fₙ: Tₙ } well-formed
```

**Modal procedure:**
```
[WF-Modal-Procedure]
modal M { ..., state @S₁, ..., state @S₂, ... }
Γ, self: M@S₁ ⊢ body : M@S₂
────────────────────────────────────────────────
procedure m@S₁(...) => @S₂ { body } well-formed
```

**Reachability:**
```
[WF-Reachability]
modal M = (S, Σ, δ, s₀, Fields)
∀ @S ∈ S. ∃ path from s₀ to @S in transition graph
────────────────────────────────────────────────
M satisfies reachability
```

**Determinism:**
```
[WF-Determinism]
∀ @S ∈ S, m ∈ Σ. |{@S' | δ(@S, m) = @S'}| ≤ 1
────────────────────────────────────────────────
M is deterministic
```

#### 12.3.2 Type Rules

**State-annotated type:**
```
[T-State-Type]
Γ ⊢ M : modal
@S ∈ states(M)
────────────────────────────────
Γ ⊢ M@S : Type
```

**Procedure invocation:**
```
[T-Modal-Procedure]
Γ ⊢ self : M@S₁
procedure m@S₁(...) => @S₂ in M
Γ ⊢ args : T
────────────────────────────────────────
Γ ⊢ self.m(args) : M@S₂
```

**State transition:**
```
[T-State-Transition]
Γ, Σ ⊢ e : M@S₁
Σ ⊢ (@S₁, m, @S₂) ∈ δ
Γ ⊢ args : T
────────────────────────────────────────
Γ, Σ ⊢ e.m(args) : M@S₂
```

**Invalid transition:**
```
[T-Invalid-Transition]
Γ ⊢ self : M@S₁
procedure m@S₂(...) => @S₃ in M
S₁ ≠ S₂
────────────────────────────────────────
ERROR E10020: Procedure m not available in state @S₁
```

**Field access (state-dependent):**
```
[T-Modal-Field]
Γ ⊢ self : M@S
field f : T in Fields(@S)
────────────────────────────────────────
Γ ⊢ self.f : T

[T-Modal-Field-Invalid]
Γ ⊢ self : M@S
field f ∉ Fields(@S)
────────────────────────────────────────
ERROR E10021: Field f not available in state @S
```

#### 12.3.3 State Invariants

**State invariant declaration:**
```
[T-State-Invariant]
state @S {
    fields...
    invariant P(fields)
}
────────────────────────────────────────
invariant(@S) = P
```

**Invariant preservation:**
```
[T-Invariant-Preservation]
procedure m@S₁(...) => @S₂
    must P
    will Q
P ∧ invariant(@S₁) ⟹ Q ∧ invariant(@S₂)
────────────────────────────────────────
Transition preserves invariants
```

**Example:**
```cantrip
modal Account {
    state Active {
        balance: i64
        invariant self.balance >= 0
    }

    state Frozen {
        balance: i64
        reason: String
        invariant self.balance >= 0
    }

    procedure withdraw@Active(amount: i64) => @Active
        must amount > 0
        must self.balance >= amount  // Preserves invariant
        will self.balance == old(self.balance) - amount
    {
        Active { balance: self.balance - amount }
    }
}
```

### 12.4 Dynamic Semantics

#### 12.4.1 State Transitions

**Transition evaluation:**
```
[E-Modal-Transition]
⟨self, σ⟩ ⇓ ⟨v_self : M@S₁, σ₁⟩
procedure m@S₁(...) => @S₂ in M
⟨body[self ↦ v_self], σ₁⟩ ⇓ ⟨v_result : M@S₂, σ₂⟩
────────────────────────────────────────────────────
⟨self.m(...), σ⟩ ⇓ ⟨v_result, σ₂⟩
```

**State construction:**
```
[E-State-Construction]
⟨e₁, σ⟩ ⇓ ⟨v₁, σ₁⟩    ...    ⟨eₙ, σₙ₋₁⟩ ⇓ ⟨vₙ, σₙ⟩
────────────────────────────────────────────────────
⟨@S { f₁: e₁, ..., fₙ: eₙ }, σ⟩ ⇓ ⟨M@S { f₁: v₁, ..., fₙ: vₙ }, σₙ⟩
```

#### 12.4.2 Linear State Transfer

**Linearity rule:**

Modal values use linear semantics—each value must be used exactly once:

```
[Linear-Use]
Γ ⊢ x : M@S
x used exactly once in scope
────────────────────────────────
Well-formed

[Linear-Use-Error]
Γ ⊢ x : M@S
x used zero or multiple times
────────────────────────────────
ERROR E10022: Linear value must be used exactly once
```

**Example:**
```cantrip
let file = File.new("data.txt")  // File@Closed
let file2 = file   // Moves ownership
// let x = file.open()  // ERROR E10022: file already moved

let conn = Connection.new("localhost", 8080)
// conn must be used exactly once
let conn = conn.connect()?
```

**Rebinding pattern:**

The common pattern is to rebind the same variable name:

```cantrip
let file = File.new("data.txt")  // File@Closed
let file = file.open()?           // Rebind: now File@Open
let data = file.read(1024)?       // Still File@Open (returns (data, file))
let file = file.close()           // Back to File@Closed
```

#### 12.4.3 Memory Layout

**Modal memory representation:**

A modal value consists of:
1. **Discriminant** (which state)
2. **Payload** (fields for that state)

```
Modal layout (similar to enum):
┌──────────────┬─────────────────────────┐
│ discriminant │  state payload          │
│  (which @S)  │  (max size of all states)│
└──────────────┴─────────────────────────┘
```

**Size calculation:**
```
size(modal M { @S₁, ..., @Sₙ }) =
    size(discriminant) + max(size(@S₁), ..., size(@Sₙ)) + padding

align(M) = max(align(discriminant), align(@S₁), ..., align(@Sₙ))
```

**Example:**
```cantrip
modal Example {
    state Small { x: u8; }           // 1 byte
    state Large { data: [u8; 100]; } // 100 bytes
}
// Total size ≈ 104 bytes (discriminant + largest state)
```

### 12.5 Verification

#### 12.5.1 Verification Conditions

**State reachability:**

Compiler verifies all states are reachable from initial state:

```
VC-Reachability:
∀ @S ∈ states(M). ∃ path s₀ →* @S
```

**Diagnostic:** `E10023` — State @S is unreachable from initial state

**Transition validity:**

All transitions must target valid states:

```
VC-Valid-Target:
∀ (procedure m@S₁ => @S₂). @S₁, @S₂ ∈ states(M)
```

**Diagnostic:** `E10024` — Transition targets non-existent state @S

#### 12.5.2 Static Checking

**Exhaustiveness checking:**

When matching on modal states, all states must be covered:

```cantrip
function describe(conn: Connection) => str {
    match conn {
        Connection@Disconnected => "disconnected",
        Connection@Connected => "connected",
        // Compiler will exhaustiveness
    }
}
```

**Type rule:**
```
[T-Modal-Match]
Γ ⊢ e : M
patterns cover all states in M
∀i. Γ, pᵢ ⊢ eᵢ : T
────────────────────────────────────────
Γ ⊢ match e { p₁ => e₁, ..., pₙ => eₙ } : T
```

#### 12.5.3 Runtime Checks

**Invariant checking:**

When enabled, runtime checks verify state invariants:

```cantrip
#[runtime_checks(true)]
modal SafeAccount {
    state Active {
        balance: i64
        invariant self.balance >= 0
    }

    procedure withdraw@Active(amount: i64) => @Active {
        Active { balance: self.balance - amount }
        // Runtime check: balance >= 0
    }
}
```

### 12.6 Advanced Features

#### 12.6.1 State Unions

**Syntax:**

Procedures can accept multiple source states using union syntax:

```cantrip
modal OrderProcessor {
    state Draft { items: Vec<Item>; }
    state Submitted { order_id: u64; items: Vec<Item>; }
    state Cancelled { order_id: u64; reason: String; }

    // Can cancel from either Draft or Submitted
    procedure cancel@(Draft | Submitted)(reason: String) => @Cancelled {
        let order_id = match self {
            Draft { .. } => generate_id(),
            Submitted { order_id, .. } => order_id,
        }
        Cancelled { order_id, reason }
    }
}
```

**Type rule:**
```
[T-State-Union]
procedure m@(@S₁ | ... | @Sₙ)(...) => @S
Γ ⊢ self : M@Sᵢ    where Sᵢ ∈ {@S₁, ..., @Sₙ}
────────────────────────────────────────────────
Γ ⊢ self.m(...) : M@S
```

#### 12.6.2 Common Fields

**Purpose:** Fields that exist in ALL states can be declared once in a `common` block.

**Syntax:**
```cantrip
modal HttpRequest {
    common {
        url: String
        headers: HashMap<String, String>
    }

    state Building { }

    state Sent {
        response_code: u16
    }

    state Completed {
        response_code: u16
        body: Vec<u8>
    }

    procedure send@Building() => @Sent
        uses net.http
    {
        let code = http_send(self.url, self.headers)?
        Sent { response_code: code }
    }
}
```

**Access:**

Common fields accessible in all states:

```cantrip
function log_url(req: HttpRequest@Building) {
    println(req.url)  // OK: url is common
}

function log_url2(req: HttpRequest@Sent) {
    println(req.url)  // OK: url is common
}
```

**Formal semantics:**
```
common fields ⊆ Fields(@S) for all @S ∈ states(M)
```

#### 12.6.3 Reachability Analysis

**Algorithm:**

```
reachability(M = (S, Σ, δ, s₀)):
1. Initialize reached = {s₀}
2. Initialize worklist = {s₀}
3. While worklist ≠ ∅:
     a. Remove state @S from worklist
     b. For each procedure m where (@S, m, @S') ∈ δ:
        i.  If @S' ∉ reached:
            - Add @S' to reached
            - Add @S' to worklist
4. Return unreachable = S \ reached
```

**Example:**
```cantrip
modal Protocol {
    state Init { }
    state Ready { }
    state Working { }
    state Done { }
    state Error { }       // Unreachable!
    state Timeout { }     // Unreachable!

    procedure start@Init() => @Ready
    procedure process@Ready() => @Working
    procedure finish@Working() => @Done
    // No transitions to Error or Timeout
}
```

**Diagnostic:** `W10025` — States Error and Timeout are unreachable

### 12.7 Examples and Patterns

#### 12.7.1 Resource Lifecycle Pattern

**File management:**
```cantrip
modal File {
    common {
        path: String
    }

    state Closed { }

    state Open {
        handle: FileHandle
        mode: FileMode
    }

    procedure open@Closed(mode: FileMode) => @Open
        uses fs.open
        must self.path.is_valid()
        will result.mode == mode
    {
        let handle = fs::open(self.path, mode)?
        Open { handle, mode }
    }

    procedure read@Open(buf: [mut u8]) => @Open
        uses fs.read
        must self.mode.allows_read()
    {
        self.handle.read(buf)?
        self
    }

    procedure write@Open(data: [u8]) => @Open
        uses fs.write
        must self.mode.allows_write()
    {
        self.handle.write(data)?
        self
    }

    procedure close@Open() => @Closed
        uses fs.close
    {
        self.handle.close()
        Closed { }
    }
}
```

#### 12.7.2 Request-Response Pattern

**HTTP client:**
```cantrip
modal HttpClient {
    common {
        url: String
        timeout: Duration
    }

    state Ready { }

    state Sent {
        request_id: u64
        sent_at: DateTime
    }

    state Completed {
        request_id: u64
        status: u16
        body: Vec<u8>
    }

    state Failed {
        error: Error
    }

    procedure send@Ready(body: Vec<u8>) => @Sent
        uses net.http
    {
        let id = http_send(self.url, body)?
        Sent {
            request_id: id,
            sent_at: DateTime::now(),
        }
    }

    procedure poll@Sent() => @(Sent | Completed | Failed)
        uses net.http
    {
        match http_poll(self.request_id) {
            Poll::Pending => self,  // Stay in Sent
            Poll::Ready(Ok(response)) => Completed {
                request_id: self.request_id,
                status: response.status,
                body: response.body,
            },
            Poll::Ready(Err(e)) => Failed { error: e },
        }
    }
}
```

#### 12.7.3 Multi-Stage Pipeline Pattern

**Data processing:**
```cantrip
modal DataProcessor {
    state Raw {
        data: Vec<u8>
    }

    state Validated {
        data: Vec<u8>
        schema: Schema
    }

    state Transformed {
        records: Vec<Record>
    }

    state Aggregated {
        summary: Summary
    }

    procedure validate@Raw(schema: Schema) => @Validated
        must !self.data.is_empty()
    {
        schema.validate(&self.data)?
        Validated {
            data: self.data,
            schema,
        }
    }

    procedure transform@Validated() => @Transformed {
        let records = self.schema.parse(&self.data)?
        Transformed { records }
    }

    procedure aggregate@Transformed() => @Aggregated {
        let summary = compute_summary(&self.records)
        Aggregated { summary }
    }
}
```

#### 12.7.4 State Recovery Pattern

**Error recovery:**
```cantrip
modal Transaction {
    common {
        id: u64
        conn: DbConnection
    }

    state Active {
        operations: Vec<Operation>
    }

    state Committed {
        timestamp: DateTime
    }

    state RolledBack {
        error: Error
    }

    procedure add_operation@Active(op: Operation) => @Active {
        self.operations.push(op)
        self
    }

    procedure commit@Active() => @(Committed | RolledBack) {
        match self.conn.commit(&self.operations) {
            Ok(timestamp) => Committed { timestamp },
            Err(error) => {
                self.conn.rollback()
                RolledBack { error }
            }
        }
    }

    // Recovery: retry after rollback
    procedure retry@RolledBack(new_ops: Vec<Operation>) => @Active {
        Active { operations: new_ops }
    }
}
```

---

**Previous**: [Generics](07_Generics.md) | **Next**: [Functions](../03_Functions/01_FunctionBasics.md)
