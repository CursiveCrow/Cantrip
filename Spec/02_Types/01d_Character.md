# Part II: Type System - §5.4 Character Type

**Section**: §5.4 | **Part**: Type System (Part II)
**Previous**: [Boolean Type](01c_Boolean.md) | **Next**: [Never Type](01e_Never.md)

---

**Definition 5.4.1 (Character Type):** The character type `char` represents a single Unicode scalar value. Formally, `⟦char⟧ = [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]`, excluding the surrogate pair range [U+D800, U+DFFF].

## 5.4 Character Type

### 5.4.1 Overview

**Key innovation/purpose:** `char` provides Unicode-correct character handling by representing Unicode scalar values (not bytes or code units), ensuring text processing correctness for international text.

**When to use char:**
- Single Unicode characters (grapheme clusters may require strings)
- Character-by-character text processing
- Unicode codepoint manipulation
- Character classification and conversion
- Text parsing and lexical analysis

**When NOT to use char:**
- Text strings → use `str` or `String` types
- Byte data → use `u8` type
- ASCII-only data → use `u8` with appropriate validation
- Grapheme clusters (user-perceived characters) → use string libraries

**Relationship to other features:**
- **UTF-32 encoding**: Each `char` is 4 bytes (one Unicode scalar value)
- **No type inference**: Character literals always have type `char`
- **Unicode correctness**: Surrogate pairs (U+D800-U+DFFF) are invalid
- **Copy semantics**: `char` is a `Copy` type

### 5.4.2 Syntax

#### Concrete Syntax

**Type and literal syntax:**
```ebnf
CharType    ::= "char"
CharLiteral ::= "'" CharContent "'"

CharContent ::= AnyChar
              | EscapeSeq
              | UnicodeEscape

EscapeSeq   ::= "\\" | "\'" | "\"" | "\n" | "\r" | "\t" | "\0"
UnicodeEscape ::= "\\u{" HexDigit+ "}"
```

**Examples:**
```cantrip
let letter: char = 'A';           // U+0041 (Latin capital A)
let emoji: char = '🚀';            // U+1F680 (Rocket)
let chinese: char = '中';          // U+4E2D (Chinese character)
let null_char: char = '\0';       // U+0000 (null character, valid!)
let tab: char = '\t';             // U+0009 (tab)
let newline: char = '\n';         // U+000A (line feed)
let unicode: char = '\u{1F600}';  // U+1F600 (grinning face emoji)
```

#### Abstract Syntax

**Type:**
```
τ ::= char
```

**Values:**
```
v ::= 'c'    (character literal)
```

### 5.4.3 Static Semantics

#### 5.4.3.1 Well-Formedness Rules

```
[WF-Char]
──────────────────
Γ ⊢ char : Type
```

#### 5.4.3.2 Type Rules

**Character literal typing:**

```
[T-Char]
c is a valid Unicode scalar value
c ∈ [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]
──────────────────
Γ ⊢ 'c' : char
```

**No type inference:** Character literals always have type `char` with no context sensitivity:

```cantrip
let c1 = 'A';           // Type: char (no inference needed)
let c2: char = '🚀';    // Type: char (explicit annotation)
// No suffixes or context variations like integers
```

**Invalid characters:**
```cantrip
// ERROR E5401: Invalid Unicode scalar value (surrogate pair)
// let invalid: char = '\u{D800}';  // U+D800 is in surrogate range
```

#### 5.4.3.3 Type Properties

**Theorem 5.4.1 (Value Set):**

```
⟦char⟧ = Unicode Scalar Values = [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]

Total valid values: 1,112,064 codepoints
- U+0000 to U+D7FF: 55,296 codepoints
- U+E000 to U+10FFFF: 1,056,768 codepoints
```

**Excluded range (surrogate pairs):**
```
[U+D800, U+DFFF] = 2,048 invalid codepoints
```

**Theorem 5.4.2 (Size and Alignment):**

```
size(char) = 4 bytes
align(char) = 4 bytes
```

**Theorem 5.4.3 (Copy Semantics):**

```
char : Copy
```

Characters are always copied, never moved.

### 5.4.4 Dynamic Semantics

#### Evaluation Rules

**Literal evaluation:**
```
[E-Char]
────────────────
⟨'c', σ⟩ ⇓ ⟨'c', σ⟩
```

**Character comparison:**
```
[E-Char-Comparison]
⟨c₁, σ⟩ ⇓ ⟨v₁, σ'⟩    ⟨c₂, σ'⟩ ⇓ ⟨v₂, σ''⟩
codepoint(v₁) ⊙ codepoint(v₂) = result
where ⊙ ∈ {<, ≤, >, ≥, ==, ≠}
─────────────────────────────────────────
⟨c₁ ⊙ c₂, σ⟩ ⇓ ⟨result, σ''⟩
```

#### Memory Representation

**Character layout:**

```
char memory layout (4 bytes):
┌─────────────────────┐
│ Unicode codepoint   │
│     (UTF-32)        │
└─────────────────────┘
Size: 4 bytes
Alignment: 4 bytes

Valid range: [U+0000, U+D7FF] ∪ [U+E000, U+10FFFF]

Examples:
'A'  → 0x00000041
'€'  → 0x000020AC
'🚀' → 0x0001F680
```

**Byte order:**

Characters follow platform byte order (typically little-endian):

```
'🚀' (U+1F680) on little-endian:
Address: [0]  [1]  [2]  [3]
Value:   0x80 0xF6 0x01 0x00
```

#### Operational Behavior

**Comparison semantics:**

Character comparison uses Unicode scalar value ordering:

```cantrip
'A' < 'Z'              // true (U+0041 < U+005A)
'a' > 'A'              // true (U+0061 > U+0041, lowercase > uppercase)
'0' < '9'              // true (U+0030 < U+0039)
'🚀' == '🚀'            // true (U+1F680 == U+1F680)
```

**Formal property:**
```
∀ c₁, c₂ : char. c₁ < c₂ ⟺ codepoint(c₁) < codepoint(c₂)
```

**Character conversions:**

Characters can be converted to/from `u32` representing the Unicode codepoint:

```cantrip
let c: char = 'A';
let code: u32 = c as u32;                // 65 (U+0041)

let valid: char = char::from_u32(65).unwrap();      // 'A'
let invalid: Option<char> = char::from_u32(0xD800); // None (surrogate)
```

**Type rules for conversions:**
```
[T-Char-To-U32]
Γ ⊢ c : char
────────────────────────────
Γ ⊢ (c as u32) : u32

[T-U32-To-Char]
Γ ⊢ n : u32
────────────────────────────────────────
Γ ⊢ char::from_u32(n) : Option<char>    (runtime validation)
```

### 5.4.5 Examples and Patterns

#### Character Processing

**ASCII checking:**
```cantrip
function is_ascii_letter(ch: char): bool {
    (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
}

function is_ascii_digit(ch: char): bool {
    ch >= '0' && ch <= '9'
}

function is_ascii_whitespace(ch: char): bool {
    ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'
}
```

**Character classification (Unicode-aware):**
```cantrip
function is_letter(ch: char): bool {
    ch.is_alphabetic()
}

function is_numeric(ch: char): bool {
    ch.is_numeric()
}

function is_whitespace(ch: char): bool {
    ch.is_whitespace()
}

// Examples:
assert!(is_letter('A'));      // Latin
assert!(is_letter('中'));     // Chinese
assert!(is_letter('Ω'));      // Greek
assert!(is_numeric('5'));     // ASCII digit
assert!(is_numeric('५'));     // Devanagari digit (U+096B)
assert!(is_whitespace(' '));  // Space
assert!(is_whitespace('\u{00A0}'));  // Non-breaking space
```

#### Case Conversion

**Simple ASCII case conversion:**
```cantrip
function to_uppercase_ascii(ch: char): char {
    if ch >= 'a' && ch <= 'z' {
        char::from_u32((ch as u32) - 32).unwrap()
    } else {
        ch
    }
}

function to_lowercase_ascii(ch: char): char {
    if ch >= 'A' && ch <= 'Z' {
        char::from_u32((ch as u32) + 32).unwrap()
    } else {
        ch
    }
}
```

**Unicode case conversion:**
```cantrip
function to_uppercase_unicode(ch: char): char {
    ch.to_uppercase().next().unwrap_or(ch)
}

// Examples:
assert_eq!(to_uppercase_unicode('a'), 'A');
assert_eq!(to_uppercase_unicode('ß'), 'S');  // German sharp s
assert_eq!(to_uppercase_unicode('i'), 'I');  // But Turkish would be İ
```

#### Character Iteration

**Iterating over string characters:**
```cantrip
function count_vowels(text: str): usize {
    let mut count = 0;
    for ch in text.chars() {
        match ch {
            'a' | 'e' | 'i' | 'o' | 'u' |
            'A' | 'E' | 'I' | 'O' | 'U' -> count += 1,
            _ -> {},
        }
    }
    count
}

function first_uppercase(text: str): Option<char> {
    text.chars().find(|&c| c.is_uppercase())
}
```

#### Codepoint Manipulation

**Working with codepoints:**
```cantrip
function codepoint_range(start: char, end: char): Vec<char> {
    let start_code = start as u32;
    let end_code = end as u32;

    (start_code..=end_code)
        .filter_map(|code| char::from_u32(code))
        .collect()
}

// Generate ASCII letters
let lowercase = codepoint_range('a', 'z');
assert_eq!(lowercase.len(), 26);

function is_emoji(ch: char): bool {
    let code = ch as u32;
    // Simplified emoji range check
    (code >= 0x1F600 && code <= 0x1F64F) ||  // Emoticons
    (code >= 0x1F300 && code <= 0x1F5FF) ||  // Misc Symbols and Pictographs
    (code >= 0x1F680 && code <= 0x1F6FF)     // Transport and Map
}
```

#### Pattern Matching

**Character patterns:**
```cantrip
function classify_char(ch: char): str {
    match ch {
        'a'..='z' -> "lowercase letter",
        'A'..='Z' -> "uppercase letter",
        '0'..='9' -> "digit",
        ' ' | '\t' | '\n' | '\r' -> "whitespace",
        '!' | '?' | '.' | ',' -> "punctuation",
        _ -> "other",
    }
}

function hex_to_digit(ch: char): Option<u8> {
    match ch {
        '0'..='9' -> Some((ch as u32 - '0' as u32) as u8),
        'a'..='f' -> Some((ch as u32 - 'a' as u32 + 10) as u8),
        'A'..='F' -> Some((ch as u32 - 'A' as u32 + 10) as u8),
        _ -> None,
    }
}
```

#### Important Distinctions

**Char vs. Grapheme vs. Byte:**

```cantrip
// char: Single Unicode scalar value
let char_e_acute: char = 'é';  // U+00E9 (single codepoint)

// Grapheme: User-perceived character (may be multiple codepoints)
let grapheme_e_acute: str = "é";  // Could be U+0065 + U+0301 (e + combining acute)

// Byte: Raw UTF-8 byte
let bytes: [u8] = "é".as_bytes();  // [0xC3, 0xA9] in UTF-8

// Important: char != byte count
let rocket: char = '🚀';           // 4 bytes UTF-32, single char
let rocket_str: str = "🚀";        // 4 bytes UTF-8 encoding
assert_eq!(rocket_str.len(), 4);   // Byte length
assert_eq!(rocket_str.chars().count(), 1);  // Character count
```

**When char is insufficient:**

```cantrip
// Combining characters (grapheme clusters)
let flag: str = "🇺🇸";  // Two chars: U+1F1FA + U+1F1F8 (regional indicators)
assert_eq!(flag.chars().count(), 2);  // Two scalar values
// Use grapheme library for user-perceived character count

// Emoji with skin tone modifiers
let hand: str = "👋🏽";  // Two chars: U+1F44B + U+1F3FD
assert_eq!(hand.chars().count(), 2);
```

---

**Previous**: [Boolean Type](01c_Boolean.md) | **Next**: [Never Type](01e_Never.md)
