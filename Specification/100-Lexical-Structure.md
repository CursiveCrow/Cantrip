# 100 — Lexical Structure (Normative)

## 100.1 Source Files
- Encoding: UTF‑8
- Extension: `.arc`
- Line endings: LF or CRLF
- BOM: Optional (ignored)

## 100.2 Comments
```cantrip
// Line comment
/* Block comment */
/// Doc for following item
//! Module-level documentation
```

**EBNF:**
```ebnf
LineComment ::= "//" ~[\n\r]* [\n\r]
BlockComment ::= "/*" (~"*/" any)* "*/"
DocComment ::= "///" ~[\n\r]*
ModuleDoc ::= "//!" ~[\n\r]*
```

## 100.3 Identifiers
```ebnf
Identifier   ::= IdentStart IdentContinue*
IdentStart   ::= [a-zA-Z_]
IdentContinue::= [a-zA-Z0-9_]
```

Restrictions: not a keyword, not starting with a digit, case‑sensitive.
Recommended max length: 255.

## 100.4 Literals
Integer: decimal, hex `0x`, binary `0b`, octal `0o`, underscore separators.  
Float: `d.dd` with optional exponent.  
Boolean: `true`/`false`.  
Char: 32‑bit Unicode scalar.  
String: UTF‑8 with escapes.

## 100.5 Keywords

**Reserved:**
```
abstract actor as async await
break case comptime const continue
defer effect else ensures enum
exists false for forall function
if impl import internal invariant
iso let loop match modal
module move mut needs new
none own private protected procedure
public record ref region requires
result select self Self state
static trait true type var
where while
```

**Contextual (special in context):** `effects`, `pure`

## 100.6 Attributes

```ebnf
Attribute      ::= "#[" AttributeBody "]"
AttributeBody  ::= Ident ( "(" AttrArgs? ")" )?
AttrArgs       ::= AttrArg ( "," AttrArg )*
AttrArg        ::= Ident "=" Literal | Literal | Ident
```

Core attributes include `#[repr(...)]`, `#[verify(...)]`, `#[overflow_checks(true|false)]`.
The `#[alias(...)]` attribute declares alternative *source‑level* names; it has
no ABI impact and MUST NOT change name resolution.
