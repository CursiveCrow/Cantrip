# Appendix A: Complete EBNF Grammar

This appendix collects all grammar productions from the specification into a single normative reference.

## A.1 Lexical Grammar

### A.1.1 Comments
```ebnf
LineComment ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" any)* "*/"
DocComment ::= "///" ~[\n\r]*
ModuleDoc ::= "//!" ~[\n\r]*
```

### A.1.2 Identifiers
```ebnf
Identifier ::= IdentStart IdentContinue*
IdentStart ::= [a-zA-Z_]
IdentContinue ::= [a-zA-Z0-9_]
```

### A.1.3 Literals
```ebnf
IntegerLiteral ::= DecimalLiteral
                 | HexLiteral
                 | BinaryLiteral
                 | OctalLiteral

DecimalLiteral ::= [0-9] [0-9_]*
HexLiteral ::= "0x" [0-9a-fA-F] [0-9a-fA-F_]*
BinaryLiteral ::= "0b" [01] [01_]*
OctalLiteral ::= "0o" [0-7] [0-7_]*

FloatLiteral ::= DecimalLiteral "." DecimalLiteral Exponent?
Exponent ::= [eE] [+-]? DecimalLiteral

BooleanLiteral ::= "true" | "false"

CharLiteral ::= "'" (EscapeSequence | ~['\\]) "'"
EscapeSequence ::= "\\" [nrt\\'"0]
                 | "\\x" HexDigit HexDigit
                 | "\\u{" HexDigit+ "}"

StringLiteral ::= '"' (EscapeSequence | ~["\\])* '"'
```

### A.1.4 Attributes
```ebnf
Attribute ::= "#[" AttributeBody "]"
AttributeBody ::= Ident ( "(" AttrArgs? ")" )?
AttrArgs ::= AttrArg ( "," AttrArg )*
AttrArg ::= Ident "=" Literal | Literal | Ident
```

## A.2 Type Grammar

```ebnf
Type ::= PrimitiveType
       | CompoundType
       | UserType
       | PermissionType
       | ModalType
       | FunctionType
       | GenericType

PrimitiveType ::= "i8" | "i16" | "i32" | "i64" | "isize"
                | "u8" | "u16" | "u32" | "u64" | "usize"
                | "f32" | "f64"
                | "bool" | "char" | "str"

CompoundType ::= ArrayType | TupleType | SliceType

ArrayType ::= "[" Type ";" IntegerLiteral "]"
SliceType ::= "[" Type "]"
TupleType ::= "(" Type ("," Type)* ")"

PermissionType ::= "own" Type
                 | "mut" Type
                 | "iso" Type

ModalType ::= Type "@" StateIdent

FunctionType ::= "fn" "(" TypeList? ")" (":" Type)?
ProcedureType ::= "proc" "(" SelfParam "," TypeList? ")" (":" Type)?

TypeList ::= Type ("," Type)*

GenericType ::= Identifier "<" TypeArgs ">"
TypeArgs ::= Type ("," Type)*
```

## A.3 Expression Grammar

```ebnf
Expr ::= Literal
       | Identifier
       | BinaryExpr
       | UnaryExpr
       | CallExpr
       | IfExpr
       | LoopExpr
       | MatchExpr
       | BlockExpr
       | LetExpr
       | PipelineExpr
       | LambdaExpr
       | RegionExpr
       | ComptimeExpr
       | TypedHoleExpr

Literal ::= IntegerLiteral
          | FloatLiteral
          | BooleanLiteral
          | CharLiteral
          | StringLiteral

BinaryExpr ::= Expr BinOp Expr
BinOp ::= "+" | "-" | "*" | "/" | "%"
        | "==" | "!=" | "<" | ">" | "<=" | ">="
        | "&&" | "||"
        | "&" | "|" | "^" | "<<" | ">>"
        | "=>" | "=" | "+=" | "-=" | "*=" | "/="

UnaryExpr ::= UnaryOp Expr
UnaryOp ::= "-" | "!" | "*" | "&"

CallExpr ::= Expr "(" ArgList? ")"
ArgList ::= Expr ("," Expr)*

IfExpr ::= "if" Expr BlockExpr ("else" (IfExpr | BlockExpr))?

LoopExpr ::= "loop" LoopHead? LoopVerification? BlockExpr

LoopHead ::= LoopPattern
           | LoopCondition

LoopPattern ::= Pattern "in" Expr
LoopCondition ::= Expr

LoopVerification ::= LoopVariant? LoopInvariant?

LoopVariant ::= "by" Expr

LoopInvariant ::= "with" PredicateBlock ";"

MatchExpr ::= "match" Expr "{" MatchArm* "}"
MatchArm ::= Pattern ("if" Expr)? "=>" Expr ","

BlockExpr ::= "{" Statement* Expr? "}"

LetExpr ::= "let" Pattern (":" Type)? "=" Expr

PipelineExpr ::= Expr "=>" Expr

LambdaExpr ::= "|" ParamList? "|" Expr
             | "|" ParamList? "|" BlockExpr

RegionExpr ::= "region" Identifier BlockExpr

ComptimeExpr ::= "comptime" BlockExpr

TypedHoleExpr ::= "?" Identifier? (":" Type)?
                | "???"
```

## A.4 Pattern Grammar

```ebnf
Pattern ::= IdentPattern
          | WildcardPattern
          | LiteralPattern
          | TuplePattern
          | EnumPattern
          | RecordPattern

IdentPattern ::= Identifier
WildcardPattern ::= "_"
LiteralPattern ::= IntegerLiteral | BooleanLiteral | CharLiteral

TuplePattern ::= "(" Pattern ("," Pattern)* ")"

EnumPattern ::= Identifier ("::" Identifier)? ("(" Pattern ")")?

RecordPattern ::= Identifier "{" FieldPatterns? "}"
FieldPatterns ::= FieldPattern ("," FieldPattern)*
FieldPattern ::= Identifier (":" Pattern)?
```

## A.5 Statement Grammar

```ebnf
Statement ::= LetStmt
            | VarStmt
            | ExprStmt
            | AssignStmt
            | ReturnStmt
            | BreakStmt
            | ContinueStmt

LetStmt ::= "let" Pattern (":" Type)? "=" Expr ";"
VarStmt ::= "var" Pattern (":" Type)? "=" Expr ";"
ExprStmt ::= Expr ";"
AssignStmt ::= Expr "=" Expr ";"
ReturnStmt ::= "return" Expr? ";"
BreakStmt ::= "break" Label? Expr? ";"
ContinueStmt ::= "continue" Label? ";"
Label ::= "'" Identifier
```

## A.6 Declaration Grammar

### A.6.1 Function and Procedure Declarations
```ebnf
FunctionDecl ::= Attribute* Visibility? "function" Identifier GenericParams?
                 "(" ParamList? ")" (":" Type)?
                 ContractClause*
                 FunctionBody

ProcedureDecl ::= Attribute* Visibility? "procedure" Identifier GenericParams?
                  "(" SelfParam ("," ParamList)? ")" (":" Type)?
                  StateTransition?
                  ContractClause*
                  FunctionBody

ParamList ::= Param ("," Param)*
Param ::= Identifier ":" Type

SelfParam ::= "self" (":" SelfType)?
SelfType ::= "Self" | "mut" "Self" | "own" "Self"

StateTransition ::= "@" Identifier "->" "@" Identifier

ContractClause ::= MustClause | WillClause | UsesClause | InvariantClause

MustClause ::= "must" PredicateBlock ";"
WillClause ::= "will" PredicateBlock ";"
UsesClause ::= "uses" EffectList ";"
InvariantClause ::= "invariant" ":" Assertion (";" Assertion)* ";"

PredicateBlock ::= Assertion                    // Single predicate
                 | "{" PredicateList "}"         // Multiple predicates

PredicateList  ::= Assertion ("," Assertion)* ","?

FunctionBody ::= BlockExpr | ";"
```

### A.6.2 Type Declarations
```ebnf
RecordDecl ::= Attribute* Visibility? "record" Identifier GenericParams?
               "{" RecordField* "}"

RecordField ::= Visibility? Identifier ":" Type ";"

EnumDecl ::= Attribute* Visibility? "enum" Identifier GenericParams?
             "{" EnumVariant ("," EnumVariant)* ","? "}"

EnumVariant ::= Identifier ("(" Type ")")? ("=" IntegerLiteral)?

ModalDecl ::= Attribute* Visibility? "modal" Identifier GenericParams?
              "{" ModalState+ "}"

ModalState ::= "state" Identifier "{" RecordField* "}"

TraitDecl ::= Attribute* Visibility? "trait" Identifier GenericParams? (":" TraitBounds)?
              "{" TraitItem* "}"

TraitItem ::= FunctionDecl | TypeDecl

ImplDecl ::= "impl" GenericParams? Type ("for" Type)? WhereClause?
             "{" ImplItem* "}"

ImplItem ::= FunctionDecl | ProcedureDecl
```

### A.6.3 Generic Parameters
```ebnf
GenericParams ::= "<" GenericParam ("," GenericParam)* ">"
GenericParam ::= TypeParam | ConstParam

TypeParam ::= Identifier (":" TraitBounds)?
ConstParam ::= "const" Identifier ":" Type

TraitBounds ::= Trait ("+" Trait)*

WhereClause ::= "where" WherePredicate ("," WherePredicate)*
WherePredicate ::= Type ":" TraitBounds
```

## A.7 Module Grammar

```ebnf
ModuleItem ::= ImportDecl
             | FunctionDecl
             | ProcedureDecl
             | RecordDecl
             | EnumDecl
             | ModalDecl
             | TraitDecl
             | ImplDecl
             | EffectDecl
             | TypeAlias

ImportDecl ::= "import" ImportPath ("as" Identifier)? ";"
ImportPath ::= Identifier ("::" Identifier)*

TypeAlias ::= Visibility? "type" Identifier GenericParams? "=" Type ";"

EffectDecl ::= "effect" Identifier ("{" EffectAtom* "}")? ";"
EffectAtom ::= Identifier ("(" EffectParam ("," EffectParam)* ")")?

Visibility ::= "public" | "internal" | "private"
```

## A.8 Contract Grammar

```ebnf
Assertion ::= LogicExpr

LogicExpr ::= LogicTerm
            | LogicExpr "&&" LogicExpr
            | LogicExpr "||" LogicExpr
            | "!" LogicExpr
            | LogicExpr "=>" LogicExpr

LogicTerm ::= Expr CompareOp Expr
            | "forall" "(" Identifier "in" Expr ")" "{" Assertion "}"
            | "exists" "(" Identifier "in" Expr ")" "{" Assertion "}"
            | "@old" "(" Expr ")"
            | "result"
            | Expr

CompareOp ::= "==" | "!=" | "<" | ">" | "<=" | ">="
```

## A.9 Effect Grammar

```ebnf
EffectList ::= Effect ("," Effect)* ","?

Effect ::= EffectAtom
         | "effects" "(" Identifier ")"
         | "!" Effect
         | "_?"

EffectAtom ::= "alloc" "." AllocEffect
             | "fs" "." FileSystemEffect
             | "net" "." NetworkEffect
             | "time" "." TimeEffect
             | "thread" "." ThreadEffect
             | "render" "." RenderEffect
             | "audio" "." AudioEffect
             | "input" "." InputEffect
             | "ffi" "." FFIEffect
             | "unsafe" "." UnsafeEffect
             | "panic"
             | Identifier

AllocEffect ::= "heap" | "region" | "stack" | "temp" | "*"
FileSystemEffect ::= "read" | "write" | "delete"
NetworkEffect ::= "read" "(" Direction ")" | "write"
Direction ::= "inbound" | "outbound"
TimeEffect ::= "read" | "sleep"
ThreadEffect ::= "spawn" | "join" | "atomic"
RenderEffect ::= "draw" | "compute"
AudioEffect ::= "play"
InputEffect ::= "read"
FFIEffect ::= "call"
UnsafeEffect ::= "ptr"
```

---
