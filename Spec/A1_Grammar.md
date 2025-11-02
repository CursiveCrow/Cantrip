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
                 | EffectPolyType
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

PermissionType  ::= "own" Type
                 | "mut" Type
                 | "imm" Type

PointerType     ::= "*" Type | "*mut" Type

SafePtrType     ::= "Ptr" "<" Type ">"
                 | "Ptr" "<" Type ">" "@" Ident

ModalType       ::= Ident "@" ModalState
ModalState      ::= Ident | "_?"

FunctionType    ::= "(" TypeList? ")" "→" Type EffectAnnotation?

EffectPolyType  ::= "∀ε" EffectBound? "." FunctionType
EffectBound     ::= "where" "ε" "⊆" "{" EffectSet "}"

GenericType     ::= Ident "<" TypeArgs ">"
TypeArgs        ::= Type ("," Type)*
TypeList        ::= Type ("," Type)*

UnionType       ::= Type "∨" Type

SelfType        ::= "Self"

EffectAnnotation::= "!" EffectSet

TypeHole            ::= "_?"
PermissionHoleType  ::= "_?" Type
```

Note (normative; see Part VIII §8.2 and §8.7):
- `PermissionHoleType` appears only where a permission wrapper is expected; elaboration chooses `perm⋆ ∈ {imm, mut, own}` and yields `perm⋆ Type`.
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
UnaryExpr       ::= UnaryOp UnaryExpr
                 | PostfixExpr
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
                 | TransitionArm
TransitionArm   ::= ModalPattern "=>" Expr "(" ArgumentList? ")" "=>" Expr ","

LoopExpr        ::= "loop" LoopHead? LoopVerif? LoopBlock
LoopHead        ::= Pattern ":" Type "in" Expr
                 | Expr
LoopVerif       ::= ("with" "{" AssertionList "}")? ("by" Expr)?
LoopBlock       ::= BlockExpr

BlockExpr       ::= BlockStmt

ClosureExpr     ::= "|" ParamList? "|" "→" Expr
                 | "|" ParamList? "|" BlockExpr

AssignOp        ::= "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^=" | "<<=" | ">>="
UnaryOp         ::= "-" | "!" | "*" | "&"
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
                 | EffectGatedBranch
                 | LoopWithRegion
                 | WithBlock

BlockStmt       ::= "{" Statement* "}"
                 | "{" Statement* "result" Expr "}"

VarDeclStmt     ::= "let" Pattern (":" Type)? "=" Expr
                 | "var" Pattern (":" Type)? "=" Expr
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

EffectGatedBranch
                 ::= "comptime" "if" EffectPredicate BlockStmt ("else" BlockStmt)?
EffectPredicate ::= "effects_include" "(" EffectSet ")"
                 | "effects_exclude" "(" EffectSet ")"

LoopWithRegion  ::= "loop" Pattern ":" Type "in" Expr "region" Ident BlockStmt

WithBlock       ::= "with" EffectList "{" ImplementationList "}" BlockStmt
EffectList      ::= EffectPath ("," EffectPath)*
ImplementationList
                ::= Implementation ("," Implementation)*
Implementation  ::= EffectPath "." Ident "(" ParamList? ")" "=>" Expr
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
                 | EffectDecl
                 | ContractDecl
                 | TraitDecl

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
RecordBody      ::= "{" RecordMember* "}"
RecordMember    ::= RecordField | ProcedureDecl | FunctionDecl
RecordField     ::= Visibility? Ident ":" Type

TupleStructDecl ::= Attribute* Visibility? "record" Ident "(" FieldType ("," FieldType)* ","? ")"
                    ("{" MethodDecl* "}")?
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
                       ("uses" ":" EffectSet ","?)?
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

MethodDecl      ::= Attribute* Visibility? "procedure" Ident
                    "(" ParamList ")" (":" Type)?
                    WhereClause?
                    ContractClause*
                    BlockStmt

ParamList       ::= Param ("," Param)*
Param           ::= Ident ":" Type
                 | SelfParam

SelfParam       ::= "$"
                 | "self" ":" SelfType
                 | "self" ":" Permission SelfType

ContractDecl    ::= Attribute* Visibility? "contract" Ident GenericParams?
                    ExtendsClause? ContractBody
ExtendsClause   ::= "extends" Ident ("," Ident)*
ContractBody    ::= "{" ContractItem* "}"
ContractItem    ::= AssociatedTypeDecl
                 | ProcedureSignature

AssociatedTypeDecl
                 ::= "type" Ident TypeBound?
TypeBound       ::= ":" TraitBounds

ProcedureSignature
                 ::= "procedure" Ident "(" ParamList? ")" (":" Type)?
                    WhereClause?
                    ContractClause*

TypeProjection  ::= Ident "::" Ident
                 | "Self" "::" Ident

TraitDecl       ::= Attribute* Visibility? "trait" Ident GenericParams? (":" TraitBounds)?
                    "{" TraitItem* "}"
TraitItem       ::= FunctionDecl | ProcedureDecl | TypeDecl

QualifiedName   ::= Ident ("::" Ident)*
ModulePath      ::= Ident ("." Ident)*
IdentList       ::= Ident ("," Ident)* ","?

Visibility      ::= "public" | "internal" | "private" | "protected"
GenericParams   ::= "<" GenericParam ("," GenericParam)* ">"
GenericParam    ::= TypeParam | ConstParam | EffectParam
TypeParam       ::= Ident (":" TraitBounds)?
ConstParam      ::= "const" Ident ":" Type
EffectParam     ::= Ident
TraitBounds     ::= TraitRef ("+" TraitRef)*
TraitRef        ::= Ident ("<" TypeArgs ">")?

WhereClause     ::= "where" WherePredicate ("," WherePredicate)*
WherePredicate  ::= Ident ":" TraitBounds
                 | Type ":" TraitBounds
                 | Ident "⊆" "{" EffectSet "}"

Permission      ::= "own" | "mut" | "imm"

EffectDecl      ::= "effect" EffectPath EffectBody?
EffectPath      ::= Ident ("." Ident)*
EffectBody      ::= "{" EffectItem* "}"
EffectItem      ::= EffectOperation
EffectOperation ::= Ident "(" ParamList? ")" ":" Type EffectImplBody?
EffectImplBody  ::= "{" Expr "}"
```

## A.7 Contract Grammar

```ebnf
ContractClause  ::= UsesClause
                 | MustClause
                 | WillClause

UsesClause      ::= "uses" EffectSet
MustClause      ::= "must" PredicateBlock
WillClause      ::= "will" PredicateBlock

PredicateBlock  ::= Assertion
                 | "{" AssertionList "}"
AssertionList   ::= Assertion ("," Assertion)* ","?
```

`Assertion` is defined in Appendix A.8.

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

## A.9 Effect Grammar

```ebnf
EffectSet       ::= EffectRef ("," EffectRef)* ","?

EffectRef       ::= EffectPath ("::" Ident)?
                 | "effects" "(" Ident ")"
                 | "!" EffectRef
                 | "_?"

EffectPath      ::= Ident ("." Ident)*

(* Legacy effect atoms for backward compatibility *)
EffectAtom      ::= "alloc" "." AllocEffect
                 | "fs" "." FileSystemEffect
                 | "net" "." NetworkEffect
                 | "time" "." TimeEffect
                 | "thread" "." ThreadEffect
                 | "ffi" "." FFIEffect
                 | "unsafe" "." UnsafeEffect
                 | "panic"

AllocEffect         ::= "heap" | "region" | "stack" | "temp" | "*"
FileSystemEffect    ::= "read" | "write" | "delete"
NetworkEffect       ::= "read" | "write"
TimeEffect          ::= "read" | "sleep"
ThreadEffect        ::= "spawn" | "join" | "atomic"
FFIEffect           ::= "call"
UnsafeEffect        ::= "ptr" | "transmute"
```
