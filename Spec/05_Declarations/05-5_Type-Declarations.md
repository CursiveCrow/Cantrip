# Cursive Language Specification

## Clause 5 — Declarations

**Clause**: 5 — Declarations
**File**: 05-5_Type-Declarations.md
**Section**: §5.5 Type Declarations
**Stable label**: [decl.type]  
**Forward references**: Clause 6 [name], Clause 7 [type], Clause 10 [generic], Clause 11 [memory], Clause 12 [contract], Clause 13 [witness]

---

### §5.5 Type Declarations [decl.type]

#### §5.5.1 Overview

[1] Type declarations introduce nominal and transparent types into a module’s lexical namespace. The language provides records (named product types), tuple records (product types with positional fields), enums (tagged sum types), modal types (compile-time state machines), and transparent type aliases.

[2] This subclause defines the syntactic forms of these declarations, their visibility rules, and the interaction points with other clauses. Semantic properties and typing rules reside in Clause 7 (Type System) and Clause 12 (Permissions and Memory).

#### §5.5.2 Syntax

```ebnf
type_declaration
    ::= record_declaration
     | record_tuple_declaration
     | enum_declaration
     | modal_declaration
     | type_alias

record_declaration
    ::= attribute_list? visibility_modifier? "record" identifier generic_params?
        contract_clause? predicate_clause? record_body

record_body
    ::= "{" record_member_list? "}"

record_member_list
    ::= record_member ("," record_member)*

record_member
    ::= visibility_modifier? identifier ":" type_expression
     | procedure_declaration

record_tuple_declaration
    ::= attribute_list? visibility_modifier? "record" identifier generic_params?
        contract_clause? predicate_clause? "(" record_tuple_field_list? ")" record_body?

record_tuple_field_list
    ::= record_tuple_field ("," record_tuple_field)*

record_tuple_field
    ::= visibility_modifier? type_expression

enum_declaration
    ::= attribute_list? visibility_modifier? "enum" identifier generic_params?
        contract_clause? predicate_clause? enum_body

enum_body
    ::= "{" enum_variant_list? "}"

enum_variant_list
    ::= enum_variant ("," enum_variant)*

enum_variant
    ::= visibility_modifier? identifier
        ( "(" type_list? ")"
        | "{" enum_field_list? "}"
        )?

enum_field_list
    ::= enum_field ("," enum_field)*

enum_field
    ::= visibility_modifier? identifier ":" type_expression

modal_declaration
    ::= attribute_list? visibility_modifier? "modal" identifier generic_params?
        contract_clause? predicate_clause? modal_body

modal_body
    ::= "{" modal_state_block+ "}"

modal_state_block
    ::= state_identifier state_payload? state_transition*

state_identifier
    ::= "@" identifier

state_payload
    ::= "{" state_field_list? "}"

state_field_list
    ::= state_field ("," state_field)*

state_field
    ::= visibility_modifier? identifier ":" type_expression

state_transition
    ::= state_identifier "::" identifier "(" parameter_list? ")" "->" state_identifier

type_alias
    ::= visibility_modifier? "type" identifier generic_params? "=" type_expression

contract_clause
    ::= ":" contract_reference ("," contract_reference)*

behavior_clause
    ::= "with" behavior_reference ("," behavior_reference)*

contract_reference
    ::= type_expression

behavior_reference
    ::= type_expression
```

[1] `attribute_list` follows §1.4.3; representation attributes such as `[[repr(packed)]]` or `[[repr(transparent)]]` attach to records, tuple records, enums, or modal types.

[2] `procedure_declaration` embeds member procedures (§5.4) within records.

[3] Tuple records reuse the `record` keyword with a positional field list and may include an optional body containing member procedures.

[4] **Modal state transitions**: Modal types use two distinct syntactic forms with semantically meaningful operators:

- **Transition signatures** (`@SourceState::transition_name(params) -> @TargetState`): Declare valid state transitions within the modal body. These lightweight declarations define the state machine graph and use the **mapping operator `->` (reads as "transitions to" or "maps to")** to indicate the state-to-state relationship.

- **Procedure implementations**: Provide the actual transition logic using standard procedure syntax (§5.4) with the **type operator `:` (reads as "is of type")** for return type annotations. Each transition signature must have a corresponding procedure implementation.

[ Note: The semantic distinction between operators clarifies intent:
- **`->` (mapping operator)**: Declares a transition relationship between states (`@Source` transitions to `@Target`)
- **`:` (type operator)**: Annotates that a value or return is of a particular type (`result : Self@Target`)

Transition signatures use `->` because they declare state graph edges (mappings between states). Procedure implementations use `:` because they follow standard procedure syntax where return types are type annotations. This distinction is grammatically unambiguous: `->` appears only in transition signatures within modal bodies; `:` appears in return type positions following parameter lists.
— end note ]

#### §5.5.3 Constraints

[1] _Visibility defaults._ Type declarations at module scope default to `internal`. Member visibility defaults to the containing declaration’s visibility unless overridden. Tuple record fields default to the declaration’s visibility.

[2] _Namespace uniqueness._ Type names share the lexical namespace with other identifiers (§5.1.3) and shall not collide within the same scope.

[3] _Field requirements._ Record and enum field names must be unique within their declaration. Field type annotations are mandatory. Tuple record fields specify types but no identifiers.

[4] _Member procedures._ Procedures declared inside records or modal states follow §5.4. Receiver shorthand (`~`, `~%`, `~!`) binds the implicit `self` parameter to the enclosing type. Member procedures shall include a contractual sequent; placing it on the line immediately following the signature is recommended for readability.

[5] _Enum variants._ Variants may be unit-style, tuple-style (`Variant(T₁, …, Tₙ)`), or record-style (`Variant { field: Type, … }`). Variants inherit the enum’s visibility unless overridden. Variant identifiers must be unique.

[6] _Contract clause._ When a type declaration includes a contract clause, each referenced contract shall be visible at the point of declaration and the type shall provide implementations for every required item as specified in Clause 12. Duplicate contract references are diagnosed as E05-503.

[7] _Behavior clause._ When a type declaration includes a behavior clause, each referenced behavior shall be visible and applicable to the type's kind. Behaviors shall be implemented according to Clause 10; duplicate behavior references are diagnosed as E05-504.

[8] _Modal structure._ A modal type shall declare at least one state using the `@State` notation. Each state may include a payload block `{ fields }` (record-style) followed by zero or more transition signatures. Transition signatures use the **mapping operator `->` (reads as "transitions to")** in the form `@SourceState::name(params) -> @TargetState` where `@SourceState` matches the state containing the signature. The source state qualifier makes explicit which state the transition originates from. Parameters may include receiver shorthand (`~`, `~%`, `~!`) as the first parameter.

[9] _Transition implementations._ Each transition signature `@Source::name(params) -> @Target` shall have a corresponding procedure implementation. The procedure shall be named `ModalType.name`, and follow standard procedure syntax (§5.4):

- Receiver matches the signature's shorthand: `~` → `self: const Self@Source`, `~%` → `self: shared Self@Source`, `~!` → `self: unique Self@Source`
- Return type is `Self@Target` using the **type operator `:`** (reads as "is of type")
- Includes contractual sequent specification per §5.4.3[3.1]

Procedure implementations may be declared at module scope or within the state body as complete procedure declarations. The semantic distinction between operators is:
- **`->` (mapping operator)**: Declares a state transition (state A maps to state B)
- **`:` (type operator)**: Declares a type annotation (return value is of type T)

This distinction clarifies intent: signatures declare the state machine graph (using mapping semantics), while implementations provide executable procedures (using type annotation semantics).

[10] _Attributes._ Representation attributes (`[[repr(packed)]]`, `[[repr(transparent)]]`, `[[repr(align(N))]]`) are optional and shall comply with layout rules in Clause 7. Unsupported attributes are ill-formed (diagnostic E05-502).

[11] _Type alias cycles._ Type aliases form transparent names. Direct or indirect cyclic aliases are prohibited (diagnostic E05-501).

[12] _Generics._ Generic parameter lists may include type, const, and grant parameters. Bounds are specified via `where` clauses (§10.3 [generic.bounds]) and validated in Clause 7.

[13] _Empty type declarations._ Records, enums, and modal types may have empty bodies (no fields, variants, or states). Empty records represent unit-like types; empty enums represent uninhabited types (never types); empty modal types are ill-formed (diagnostic E05-505) as they provide no states. Empty tuple records are well-formed and represent zero-element tuples.

[14] _Multiple contract implementations._ When a type implements multiple contracts via the contract clause, all required items from all contracts must be satisfied. If two contracts require procedures with the same name but incompatible signatures, the type declaration is ill-formed (diagnostic E05-506). If signatures are compatible (subtype relationship), a single implementation may satisfy both contracts.

[15] _Recursive type definitions._ Types may reference themselves directly or indirectly in field types, variant payloads, or generic bounds. Recursive definitions are well-formed provided they do not create infinite-size types (e.g., `record R { field: R }` is ill-formed, but `record R { field: Ptr<R> }` is well-formed). Implementations shall detect infinite-size recursive types and emit diagnostic E05-507 (infinite-size recursive type).

#### §5.5.4 Semantics

[1] Records and tuple records introduce nominal product types; field layout, initialization, and destruction semantics are governed by Clause 7 and Clause 12.

[2] Enums introduce nominal sum types with tagged variants. Variant constructors become callable expressions (§8.4) and participate in exhaustiveness checking (§7.3).

[3] Modal types define compile-time verified state machines. Each state corresponds to a distinct type, and transitions are enforced through the modal transition signatures in Clause 7.

[4] When a type lists contracts using the contract clause, the declaration establishes an implementation obligation. Clause 12 specifies how each referenced contract contributes required procedures, associated types, and clauses; §5.5.3[6] enforces that every obligation is fulfilled within the declaration.

[5] Behavior clauses attach behavior implementations to the type. Clause 10 governs behavior formation, coherence, and conflict detection. The behavior clause is equivalent to writing separate behavior implementation blocks whose receiver is the declaring type.

[6] Type aliases provide transparent renaming. Aliases do not create new nominal identities and are interchangeable with the aliased type in all contexts.

#### §5.5.5 Examples (Informative)

**Example 5.5.5.1 (Record with contracts and behaviors):**

```cursive
public record Account: Ledgered with UserStorage {
    public id: u64,
    private balance: i64,

    procedure deposit(~!, amount: i64)
        [[ ledger::post |- amount > 0 => self.balance >= amount ]]
    {
        self.balance += amount
    }
}
```

[1] `Ledgered` is a contract that contributes obligations satisfied by the record's procedures; `UserStorage` is a behavior implemented for `Account` via the behavior clause.

**Example 5.5.5.2 (Tuple record):**

```cursive
record Velocity(f64, f64) {
    procedure magnitude(~): f64
    {
        result (self.0 * self.0 + self.1 * self.1).sqrt()
    }
}
```

**Example 5.5.5.3 (Enum with mixed variants):**

```cursive
enum Message {
    Ping,
    Pong,
    Data(i32, string),
    Log { level: u8, text: string }
}
```

**Example 5.5.5.4 (Modal type):**

```cursive
modal File {
    @Closed { path: Path }
    @Closed::open(~!, path: Path) -> @Open

    @Open { path: Path, handle: Handle }
    @Open::read(~%, buffer: BufferView) -> @Open
    @Open::close(~!) -> @Closed
}

// Procedure implementations (can be at module scope or inline in state body)
procedure File.open(self: unique Self@Closed, path: Path): Self@Open
    [[ fs::open |- path.exists() => result.state() == @Open ]]
{
    // ... implementation
}

procedure File.read(self: shared Self@Open, buffer: BufferView): Self@Open
    [[ fs::read |- buffer.capacity() > 0 => buffer.filled() ]]
{
    // ... implementation
}

procedure File.close(self: unique Self@Open): Self@Closed
    [[ fs::close |- true => result.state() == @Closed ]]
{
    // ... implementation
}
```

**Example 5.5.5.5 (Type alias):**

```cursive
public type UserId = u64
```

### §5.5.6 Conformance Requirements [decl.type.requirements]

[1] Implementations shall enforce uniqueness of type names, record fields, enum variants, and modal state identifiers within their respective scopes, issuing diagnostics E05-501–E05-502 where applicable.

[2] Compilers shall verify contract and behavior clauses: referenced interfaces must be visible, duplicates are rejected (E05-503–E05-504), and the declared type shall supply every required item defined by Clause 12 and Clause 10 respectively.

[3] Compilers shall validate modal transition declarations against the requirements of §5.5.3[8]–[9], ensuring that every transition declaration has a corresponding procedure implementation with matching signature (receiver type, parameter types, and return type) and that representation attributes obey Clause 7 layout constraints.

[4] Type aliases shall be transparent; implementations shall reject cycles (E05-501) and substitute the aliased type wherever the alias appears during type analysis.
