# Appendix A: Grammar Reference

This appendix is the single normative source for all Cursive grammar productions. Every other chapter references these productions instead of re-declaring them.

## A.1 Lexical Grammar

```ebnf
Identifier      ::= IdentStart IdentContinue*
Ident           ::= Identifier
IdentStart      ::= [a-zA-Z_]
IdentContinue   ::= [a-zA-Z0-9_]

NEWLINE         ::= "\n" | "\r\n"
Separator       ::= NEWLINE | ";"

LineComment     ::= "//" ~[\n\r]* (NEWLINE | EOF)
BlockComment    ::= "/*" (~"*/" | BlockComment)* "*/"
DocComment      ::= "///" ~[\n\r]*
ModuleDoc       ::= "//!" ~[\n\r]*

Attribute       ::= "[[" AttributeBody "]]"
AttributeBody   ::= Ident ("(" AttrArgs? ")")?
AttrArgs        ::= AttrArg ("," AttrArg)*
AttrArg         ::= Ident "=" Literal
                 | Literal
                 | Ident

Literal         ::= IntegerLiteral
                 | FloatLiteral
                 | BooleanLiteral
                 | CharLiteral
                 | StringLiteral

Nat             ::= Digit (Digit | "_")*

IntegerLiteral  ::= DecLiteral IntegerSuffix?
                 | HexLiteral IntegerSuffix?
                 | OctLiteral IntegerSuffix?
                 | BinLiteral IntegerSuffix?

DecLiteral      ::= Digit (Digit | "_")*
HexLiteral      ::= "0x" HexDigit (HexDigit | "_")*
OctLiteral      ::= "0o" OctDigit (OctDigit | "_")*
BinLiteral      ::= "0b" BinDigit (BinDigit | "_")*

IntegerSuffix   ::= "i8" | "i16" | "i32" | "i64" | "isize"
                  | "u8" | "u16" | "u32" | "u64" | "usize"

FloatLiteral    ::= DecLiteral "." DecLiteral? Exponent? FloatSuffix?
                 | DecLiteral Exponent FloatSuffix?
                 | "." DecLiteral Exponent? FloatSuffix?

Exponent        ::= ("e" | "E") ("+" | "-")? DecLiteral
FloatSuffix     ::= "f32" | "f64"

BooleanLiteral  ::= "true" | "false"

CharLiteral     ::= "'" CharBody "'"
CharBody        ::= CharEscape
                  | ~["'\\\n\r]
CharEscape      ::= "\\" ["nrt\\'"0]
                  | "\\x" HexDigit HexDigit
                  | "\\u{" HexDigit+ "}"

StringLiteral   ::= "\"" StringChar* "\""
StringChar      ::= StringEscape
                  | ~["\\]
StringEscape    ::= "\\" ["nrt\\'"0]
                  | "\\x" HexDigit HexDigit
                  | "\\u{" HexDigit+ "}"

Digit           ::= [0-9]
HexDigit        ::= [0-9a-fA-F]
OctDigit        ::= [0-7]
BinDigit        ::= [0-1]
```

(Newline-sensitive statement termination rules are specified in Foundations §2.7.)

## A.2 Type Grammar

```ebnf
Type            ::= PrimitiveType
                 | CompoundType
                 | PermissionType
                 | PointerType
                 | SafePtrType
                 | ModalType
                 | FunctionType
                 | GrantPolyType
                 | GenericType
                 | UnionType
                 | SelfType
                 | PermissionHoleType
                 | TypeHole

PrimitiveType   ::= "i8" | "i16" | "i32" | "i64" | "isize"
                 |  "u8" | "u16" | "u32" | "u64" | "usize"
                 |  "f32" | "f64"
                 |  "bool" | "char" | "string" | "!" | "()"

CompoundType    ::= ArrayType | SliceType | TupleType
ArrayType       ::= "[" Type ";" IntegerLiteral "]"
SliceType       ::= "[" Type "]"
TupleType       ::= "(" Type ("," Type)+ ")"

PermissionType  ::= "owned" Type
                 | "unique" Type
                 | "shared" Type
                 | "readonly" Type

// Deprecated keywords (Version 1.0, removed in 2.0):
// PermissionType ::= "own" Type | "mut" Type | "imm" Type  // DEPRECATED

// Context-sensitive permission defaults:
// - Bindings (let/var): bare T means owned T
// - Parameters: bare T means readonly T
// - Returns: bare T means readonly T
// - Fields: bare T means owned T (only valid permission for fields)


PointerType     ::= "*" Type | "*mut" Type

SafePtrType     ::= "Ptr" "<" Type ">"
                 | "Ptr" "<" Type ">" "@" Ident
                 ; Standard modal states for Ptr<T>: @Null, @Valid, @Weak, @Expired

ModalType       ::= Ident "@" ModalState
ModalState      ::= Ident | "_?"

FunctionType    ::= "(" TypeList? ")" "->" Type GrantAnnotation?

GrantPolyType  ::= "forall" "G" GrantBound? "." FunctionType
GrantBound     ::= "where" "G" "<:" "{" GrantSet "}"

GenericType     ::= Ident "<" TypeArgs ">"
TypeArgs        ::= Type ("," Type)*
TypeList        ::= Type ("," Type)*

UnionType       ::= Type "\\/" Type

SelfType        ::= "Self"

GrantAnnotation::= "!" GrantSet

TypeHole            ::= "_?"
PermissionHoleType  ::= "_?" Type
```

Note (normative; see Part VIII §8.2 and §8.7):
- `PermissionHoleType` appears only where a permission wrapper is expected; elaboration chooses `perm⋆ ∈ {readonly, unique, shared, owned}` and yields `perm⋆ Type`.
- When `_?` is immediately followed by a `Type` token in a position that admits permission wrappers, the parser recognizes `PermissionHoleType` rather than `TypeHole`.

Range type constructors (`Range<T>`, `RangeInclusive<T>`, etc.) are defined in Part II §2.4.0.4.

## A.3 Pattern Grammar

```ebnf
Pattern         ::= WildcardPattern
                 | LiteralPattern
                 | IdentPattern
                 | TuplePattern
                 | RecordPattern
                 | EnumPattern
                 | ModalPattern
                 | Pattern "as" Ident

WildcardPattern ::= "_"
LiteralPattern  ::= Literal
IdentPattern    ::= Identifier

TuplePattern    ::= "(" Pattern ("," Pattern)+ ")"

RecordPattern   ::= Ident "{" FieldPatternList? "}"
FieldPatternList::= FieldPattern ("," FieldPattern)* ","?
FieldPattern    ::= Ident (":" Pattern)?

EnumPattern     ::= Ident ("::" Ident)? PatternPayload?
PatternPayload  ::= "(" PatternList? ")"
                 | "{" FieldPatternList "}"
PatternList     ::= Pattern ("," Pattern)* ","?

ModalPattern    ::= Ident "@" ModalPatternState PatternPayload?
ModalPatternState ::= Ident | "_?"
```

## A.4 Expression Grammar

```ebnf
Expr            ::= SeqExpr
SeqExpr         ::= AssignmentExpr
                 | AssignmentExpr Separator Expr
AssignmentExpr  ::= PipelineExpr (AssignOp PipelineExpr)*
PipelineExpr    ::= RangeExpr ("=>" RangeExpr ":" Type)*
RangeExpr       ::= LogicalOrExpr RangeSuffix
                 | RangePrefix LogicalOrExpr
                 | ".."
                 | LogicalOrExpr
RangeSuffix     ::= ".." LogicalOrExpr?
                 | "..=" LogicalOrExpr
RangePrefix     ::= ".." | "..="
LogicalOrExpr   ::= LogicalAndExpr ("||" LogicalAndExpr)*
LogicalAndExpr  ::= BitwiseOrExpr ("&&" BitwiseOrExpr)*
BitwiseOrExpr   ::= BitwiseXorExpr ("|" BitwiseXorExpr)*
BitwiseXorExpr  ::= BitwiseAndExpr ("^" BitwiseAndExpr)*
BitwiseAndExpr  ::= EqualityExpr ("&" EqualityExpr)*
EqualityExpr    ::= RelationalExpr (EqOp RelationalExpr)?
RelationalExpr  ::= ShiftExpr (RelOp ShiftExpr)?
ShiftExpr       ::= AdditiveExpr (ShiftOp AdditiveExpr)*
AdditiveExpr    ::= MultiplicativeExpr (AddOp MultiplicativeExpr)*
MultiplicativeExpr
                 ::= PowerExpr (MulOp PowerExpr)*
PowerExpr       ::= UnaryExpr ("**" UnaryExpr)*
UnaryExpr       ::= CaretPrefix UnaryExpr
                 | UnaryOp UnaryExpr
                 | PostfixExpr
CaretPrefix     ::= "^"+
PostfixExpr     ::= PrimaryExpr PostfixSuffix*
PostfixSuffix   ::= "." Ident
                 | "::" Ident "(" ArgumentList? ")"
                 | "." IntegerLiteral
                 | "[" Expr "]"
                 | "[" RangeSuffix "]"
                 | "(" ArgumentList? ")"
                 | "?"
PrimaryExpr     ::= Literal
                 | Identifier
                 | ExprHole
                 | TupleExpr
                 | RecordExpr
                 | EnumExpr
                 | ArrayExpr
                 | UnitExpr
                 | "(" Expr ")"
                 | BlockExpr
                 | IfExpr
                 | MatchExpr
                 | LoopExpr
                 | ClosureExpr
                 | RegionExpr
                 | ComptimeExpr
                 | "move" Expr
                 | "contract" "(" Expr "," Expr "," Expr ")"
                 | "transition" "(" Expr "," "@" Ident ")"
                 | "return" Expr?
                 | "defer" BlockExpr
                 | "continue" "(" Expr ")"
                 | "abort" "(" Expr ")"

UnitExpr        ::= "()"

TupleExpr       ::= "(" Expr "," Expr ("," Expr)* ","? ")"

RecordExpr      ::= Ident "{" FieldInitList? "}"
FieldInitList   ::= FieldInit ("," FieldInit)* ","?
FieldInit       ::= Ident ":" Expr
                 | Ident

TupleStructExpr ::= Ident "(" Expr ("," Expr)* ","? ")"

EnumExpr        ::= Ident "::" Ident EnumPayload?
EnumPayload     ::= "(" ArgumentList? ")"
                 | "{" FieldInitList "}"

ArrayExpr       ::= "[" Expr ("," Expr)* ","? "]"
                 | "[" Expr ";" Expr "]"

SliceExpr       ::= Expr "[" Range "]"

Range           ::= Expr ".." Expr          (exclusive end)
                 | Expr "..=" Expr         (inclusive end)
                 | ".." Expr               (from start)
                 | "..=" Expr              (from start, inclusive end)
                 | Expr ".."               (to end)
                 | ".."                    (full slice)

ArgumentList    ::= Expr ("," Expr)* ","?

IfExpr          ::= "if" IfHead BlockExpr ("else" (IfExpr | BlockExpr))?
IfHead          ::= Expr
                 | "let" Pattern "=" Expr

MatchExpr       ::= "match" Expr "{" MatchArm+ "}"
MatchArm        ::= Pattern ("if" Expr)? "=>" Expr ","

LoopExpr        ::= "loop" LoopHead? LoopVerif? LoopBlock
LoopHead        ::= Pattern ":" Type "in" Expr
                 | Expr
LoopVerif       ::= ("with" "{" AssertionList "}")? ("by" Expr)?
LoopBlock       ::= BlockExpr

BlockExpr       ::= BlockStmt

ClosureExpr     ::= "|" ParamList? "|" "->" Expr
                 | "|" ParamList? "|" BlockExpr

AssignOp        ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
UnaryOp         ::= "-" | "!" | "*" | "&"
                 ; Note: ^, &, * are position-disambiguated:
                 ;  - Prefix position: unary (region alloc, address-of, deref)
                 ;  - Infix position: binary (XOR, AND, multiply)
AddOp           ::= "+" | "-"
MulOp           ::= "*" | "/" | "%"
ShiftOp         ::= "<<" | ">>"
EqOp            ::= "==" | "!="
RelOp           ::= "<" | ">" | "<=" | ">="
; Hole production for expressions (Part VIII §8.5)
ExprHole        ::= "_?" (":" Type)?
```

`LoopVerif` and `AssertionList` reuse the assertion grammar in Appendix A.8.

## A.5 Statement Grammar

```ebnf
Statement       ::= BlockStmt
                 | VarDeclStmt
                 | AssignStmt
                 | UseDecl
                 | ExprStmt
                 | LabeledStmt
                 | ReturnStmt
                 | BreakStmt
                 | ContinueStmt
                 | DeferStmt
                 | EmptyStmt
                 | GrantGatedBranch
                 | LoopWithRegion
                 | WithBlock

BlockStmt       ::= "{" Statement* "}"
                 | "{" Statement* "result" Expr "}"

VarDeclStmt     ::= "let" Pattern (":" Type)? "=" Expr
                 | "var" Pattern (":" Type)? "=" Expr
                 | "alias" Identifier (":" Type)? "=" Expr
                 | "shadow" VarDeclStmt

AssignStmt      ::= LValue AssignOp Expr
LValue          ::= Identifier
                 | Expr "." Ident
                 | Expr "." IntegerLiteral
                 | Expr "[" Expr "]"
                 | "*" Expr

ExprStmt        ::= Expr

LabeledStmt     ::= "'" Ident ":" Statement

ReturnStmt      ::= "return" Expr?
BreakStmt       ::= "break" LabelRef? Expr?
ContinueStmt    ::= "continue" LabelRef?
LabelRef        ::= "'" Ident

DeferStmt       ::= "defer" BlockStmt
EmptyStmt       ::= ";"

GrantGatedBranch
                 ::= "comptime" "if" GrantPredicate BlockStmt ("else" BlockStmt)?
GrantPredicate  ::= "grants_include" "(" GrantSet ")"
                 | "grants_exclude" "(" GrantSet ")"

(* Deprecated: Use GrantGatedBranch instead *)
EffectGatedBranch
                 ::= "comptime" "if" EffectPredicate BlockStmt ("else" BlockStmt)?
EffectPredicate ::= "effects_include" "(" GrantSet ")"
                 | "effects_exclude" "(" GrantSet ")"

LoopWithRegion  ::= "loop" Pattern ":" Type "in" Expr "region" Ident BlockStmt

WithBlock       ::= "with" GrantList "{" ImplementationList "}" BlockStmt
GrantList       ::= GrantPath ("," GrantPath)*
ImplementationList
                ::= Implementation ("," Implementation)*
Implementation  ::= GrantPath "." Ident "(" ParamList? ")" "=>" Expr

```

`LoopWithRegion` is syntactic sugar for the explicit region pattern described in §5.12.

`WithBlock` provides custom implementations for effect operations within a specific scope (§7.3.6).

## A.6 Declaration Grammar

```ebnf
TopLevel        ::= (Decl | UseDecl)*
UseDecl         ::= "use" QualifiedName
                 | "use" QualifiedName "::" "{" IdentList "}"
                 | "import" ModulePath ("as" Ident)?

Decl            ::= VarDeclStmt
                 | TypeDecl
                 | FunctionDecl
                 | ProcedureDecl
                 | ContractDecl
                 | PredicateDecl

TypeDecl        ::= "type" Ident GenericParams? "=" Type WhereClause?
                 | RecordDecl
                 | TupleStructDecl
                 | EnumDecl
                 | UnionDecl
                 | ModalDecl

RecordDecl      ::= Attribute* Visibility? "record" Ident GenericParams?
                    ImplementsClause?
                    RecordBody
                    TypeConstraint?
ImplementsClause::= ":" ContractList
ContractList    ::= Ident ("," Ident)*
TypeConstraint  ::= "where" "{" Invariant ("," Invariant)* "}"
Invariant       ::= Assertion
RecordBody      ::= "{" RecordItem* "}"
RecordItem      ::= PartitionDirective | RecordMember
RecordMember    ::= RecordField | ProcedureDecl | FunctionDecl
RecordField     ::= Visibility? Ident ":" Type

PartitionDirective ::= "<<" (Ident | "_") ">>"

TupleStructDecl ::= Attribute* Visibility? "record" Ident "(" FieldType ("," FieldType)* ","? ")"
                    ("{" ProcedureDecl* "}")?
FieldType       ::= Visibility? Type

EnumDecl        ::= Attribute* Visibility? "enum" Ident GenericParams? EnumBody
EnumBody        ::= "{" EnumVariant ("," EnumVariant)* ","? "}"
EnumVariant     ::= Ident PatternPayload?

UnionDecl       ::= Attribute* Visibility? "union" Ident GenericParams? UnionBody
UnionBody       ::= "{" UnionField+ "}"
UnionField      ::= Ident ":" Type

ModalDecl       ::= Attribute* Visibility? "modal" Ident GenericParams? ModalBody
ModalBody       ::= "{" ModalState+ StateCoercion* ProcedureDecl* "}"
ModalState      ::= "@" Ident RecordBody WhereClause?

StateCoercion   ::= "coerce" "@" Ident "<:" "@" Ident CoercionConstraint?
CoercionConstraint ::= "{"
                       ("cost" ":" Cost ","?)?
                       ("requires" ":" Permission ","?)?
                       ("grants" ":" GrantSet ","?)?         (* Canonical form *)
                       "}"
Cost            ::= "O(1)" | "O(n)" | IntegerLiteral

FunctionDecl    ::= Attribute* Visibility? "function" Ident GenericParams?
                    "(" ParamList? ")" (":" Type)?
                    WhereClause?
                    ContractClause*
                    BlockStmt

ProcedureDecl   ::= Attribute* Visibility? "procedure" TransitionSpec?
                    Ident GenericParams?
                    "(" ParamList? ")" (":" Type)?
                    WhereClause?
                    ContractClause*
                    BlockStmt

TransitionSpec  ::= "@" Ident "->" "@" Ident

ParamList       ::= Param ("," Param)*
Param           ::= Ident ":" Type
                 | SelfParam

SelfParam       ::= "self" ":" SelfType
                 | "self" ":" Permission SelfType

ContractDecl    ::= Attribute* Visibility? "contract" Ident GenericParams?
                    ExtendsClause? ContractBody
ExtendsClause   ::= "extends" Ident ("," Ident)*
ContractBody    ::= "{" ContractItem* "}"
ContractItem    ::= AssociatedTypeDecl
                 | ProcedureSignature

AssociatedTypeDecl
                 ::= "type" Ident TypeBound?
TypeBound       ::= ":" PredicateBounds

ProcedureSignature
                 ::= "procedure" Ident "(" ParamList? ")" (":" Type)?
                    WhereClause?
                    ContractClause*

TypeProjection  ::= Ident "::" Ident
                 | "Self" "::" Ident

PredicateDecl       ::= Attribute* Visibility? "predicate" Ident GenericParams? (":" PredicateBounds)?
                    "{" PredicateItem* "}"
PredicateItem       ::= FunctionDecl | ProcedureDecl | TypeDecl

QualifiedName   ::= Ident ("::" Ident)*
ModulePath      ::= Ident ("." Ident)*
IdentList       ::= Ident ("," Ident)* ","?

Visibility      ::= "public" | "internal" | "private" | "protected"
GenericParams   ::= "<" GenericParam ("," GenericParam)* ">"
GenericParam    ::= TypeParam | ConstParam | GrantParam
TypeParam       ::= Ident (":" PredicateBounds)?
ConstParam      ::= "const" Ident ":" Type
GrantParam      ::= Ident                             (* Grant parameter, e.g., G *)
PredicateBounds     ::= PredicateRef ("+" PredicateRef)*
PredicateRef        ::= Ident ("<" TypeArgs ">")?

WhereClause     ::= "where" WherePredicate ("," WherePredicate)*
WherePredicate  ::= Ident ":" PredicateBounds
                 | Type ":" PredicateBounds
                 | Ident "<:" "{" GrantSet "}"        (* Grant parameter bounds *)
Permission      ::= "owned" | "unique" | "shared" | "readonly"
```

## A.7 Contract Grammar

```ebnf
ContractClause  ::= SequentClause

SequentClause   ::= "sequent" "{" SequentSpec "}"

SequentSpec     ::= "[" GrantSet "]" "|-" Antecedent "=>" Consequent
                 | "[" GrantSet "]" "|-" Antecedent
                 | "|-" Antecedent "=>" Consequent
                 | Antecedent "=>" Consequent

Antecedent      ::= PredicateBlock
Consequent      ::= PredicateBlock

PredicateBlock  ::= Assertion
                 | AssertionList
AssertionList   ::= Assertion ("&&" Assertion)*
```

**Explanation:**
- `SequentClause` — Unified contract specification in sequent calculus form
- `SequentSpec` — The sequent proper: `[ε] ⊢ P ⇒ Q`
  - `[ε]` — Grant context (capability assumptions)
  - `|-` — Turnstile (entailment), ASCII representation of `⊢`
  - `P` — Antecedent (preconditions)
  - `=>` — Implication, ASCII representation of `⇒`
  - `Q` — Consequent (postconditions)
- Sequent components may be omitted:
  - No grants: omit `[GrantSet]` portion
  - No preconditions: omit Antecedent (or use `true`)
  - No postconditions: omit `=> Consequent` portion

**Note:** Conjunction within antecedents and consequents uses explicit `&&` operator, not comma separation.

`Assertion` is defined in Appendix A.8.

`GrantSet` is defined in Appendix A.9 (Grant Grammar).

## A.8 Assertion Grammar

```ebnf
Assertion       ::= LogicExpr

LogicExpr       ::= LogicExpr "&&" LogicExpr
                 | LogicExpr "||" LogicExpr
                 | "!" LogicExpr
                 | LogicTerm

LogicTerm       ::= Expr CompareOp Expr
                 | "forall" "(" Ident "in" Expr ")" "{" Assertion "}"
                 | "exists" "(" Ident "in" Expr ")" "{" Assertion "}"
                 | "@old" "(" Expr ")"
                 | "result"
                 | Expr

CompareOp       ::= "==" | "!=" | "<" | ">" | "<=" | ">="
```

## A.9 Grant Grammar

**Normative**: This section defines the grammar for grant specifications, which track compile-time capabilities and restrictions on procedure operations.

```ebnf
GrantSet        ::= GrantRef ("," GrantRef)* ","?

GrantRef        ::= GrantPath
                 | "grants" "<" Ident ">"     (* Grant parameter reference *)
                 | "_?"                        (* Grant inference hole *)
                 | "!" GrantRef                (* Grant exclusion (future) *)

GrantPath       ::= Ident ("::" Ident)*       (* e.g., alloc::heap, fs::write *)

(* Built-in grant categories *)
AllocGrant      ::= "alloc" "::" ("heap" | "region" | "stack" | "temp" | "*")
FileSystemGrant ::= "fs" "::" ("read" | "write" | "delete" | "metadata" | "*")
NetworkGrant    ::= "net" "::" ("read" | "write" | "connect" | "listen" | "*")
TimeGrant       ::= "time" "::" ("read" | "sleep" | "monotonic" | "*")
ThreadGrant     ::= "thread" "::" ("spawn" | "join" | "atomic" | "*")
FFIGrant        ::= "ffi" "::" ("call" | "*")
UnsafeGrant     ::= "unsafe" "::" ("ptr" | "transmute" | "*")
PanicGrant      ::= "panic"
```

**Grant Polymorphism**: Grant parameters are declared in generic parameter lists (see §A.6) and referenced in grant sets using `grants<G>` syntax.

**Example (informative)**:
```cursive
procedure process<T, G>(data: T): Result<T>
    grants<G>, alloc::heap
    where G <: {fs::read, net::read}
{
    // Procedure body can use grants<G> and alloc::heap
}
```

**Grant Inference**: The `_?` hole allows the compiler to infer required grants based on procedure body (see Part VIII §8.x and Part IX §9.x).

**Forward Reference**: Complete grant semantics, propagation rules, and verification are specified in Part IX (Grants and Capabilities).


```

