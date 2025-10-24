### 5.6.8 String Operations

String provides a comprehensive API for construction, access, modification, and manipulation of UTF-8 text. Operations are organized by category and permission requirements.

#### 5.6.8.1 Construction Operations

**Empty string creation:**
```cantrip
function String.new(): own String
    needs alloc.heap;
```

Creates an empty String with zero length. May allocate minimal capacity.

**Example:**
```cantrip
function create_buffer(): own String
    needs alloc.heap;
{
    String.new()  // Returns empty String
}
```

---

**From UTF-8 bytes (validated):**
```cantrip
function String.from_utf8(bytes: [u8]): Result<own String, Utf8Error>
    needs alloc.heap;
```

Constructs a String from a byte array, validating UTF-8 encoding. Returns error if bytes are invalid UTF-8.

**Example:**
```cantrip
function parse_bytes(data: [u8]): Result<own String, Utf8Error>
    needs alloc.heap;
{
    String.from_utf8(data)  // Validates UTF-8
}

let valid_bytes: [u8] = [72, 101, 108, 108, 111];  // "Hello"
let s = String.from_utf8(valid_bytes)?;  // Ok(String)

let invalid_bytes: [u8] = [0xFF, 0xFE];  // Invalid UTF-8
let err = String.from_utf8(invalid_bytes);  // Err(Utf8Error)
```

---

**From UTF-8 bytes (unchecked):**
```cantrip
function String.from_utf8_unchecked(bytes: [u8]): own String
    needs alloc.heap;
```

Constructs a String from bytes without validation. **Unsafe**: Violates UTF-8 invariant if bytes are invalid.

**Example:**
```cantrip
// ONLY use if you KNOW bytes are valid UTF-8
function trusted_conversion(validated_bytes: [u8]): own String
    needs alloc.heap;
{
    String.from_utf8_unchecked(validated_bytes)  // No validation overhead
}
```

**Warning:** Using `from_utf8_unchecked` with invalid UTF-8 creates undefined behavior. Use `from_utf8` unless performance is critical and data is pre-validated.

---

**From string slice:**
```cantrip
function String.from(slice: str): own String
    needs alloc.heap;
```

Constructs a String by copying data from a string slice. Allocation required for heap storage.

**Example:**
```cantrip
function duplicate_literal(): own String
    needs alloc.heap;
{
    let literal: str = "Hello, world!";
    String.from(literal)  // Allocates and copies
}
```

---

**With initial capacity:**
```cantrip
function String.with_capacity(cap: usize): own String
    needs alloc.heap;
```

Creates an empty String with pre-allocated capacity. Useful for avoiding reallocations when final size is known.

**Example:**
```cantrip
function build_large_string(parts: [str]): own String
    needs alloc.heap;
{
    let total_len: usize = parts.map(|p| p.len()).sum();
    let mut result: own String = String.with_capacity(total_len);

    for part in parts {
        result.push_str(part);  // No reallocation needed
    }

    result
}
```

---

**Region-allocated string:**
```cantrip
function String.new_in<'r>(): own String
    needs alloc.region;
```

Creates an empty String allocated in the specified region instead of the heap. String lifetime is bound to the region.

**Example:**
```cantrip
function temporary_strings(): own String
    needs alloc.heap, alloc.region;
{
    region temp {
        let s: own String = String.new_in<temp>();
        s.push_str("temporary");

        // Cannot return s (region-bound)
        // Must convert to heap:
        String.from(s.as_str())  // Heap copy
    }  // s deallocated here
}
```

#### 5.6.8.2 Access Operations (Immutable)

**Get byte length:**
```cantrip
function len(self: String): usize;
```

Returns the length of the String in bytes (NOT characters). O(1) operation.

**Example:**
```cantrip
let s: own String = String.from("Hello🦀");
assert!(s.len() == 10);  // 5 ASCII bytes + 4 byte emoji
```

---

**Check if empty:**
```cantrip
function is_empty(self: String): bool;
```

Returns `true` if the String contains zero bytes. Equivalent to `self.len() == 0`.

**Example:**
```cantrip
let empty: own String = String.new();
assert!(empty.is_empty());

let non_empty: own String = String.from("x");
assert!(!non_empty.is_empty());
```

---

**Get capacity:**
```cantrip
function capacity(self: String): usize;
```

Returns the allocated capacity in bytes. Capacity is always greater than or equal to length.

**Example:**
```cantrip
let s: own String = String.with_capacity(100);
assert!(s.capacity() >= 100);
assert!(s.len() == 0);
```

---

**Convert to str slice:**
```cantrip
function as_str(self: String): str;
```

Returns a string slice view of the String's contents. Zero-copy operation.

**Example:**
```cantrip
function print_text(text: str) {
    println(text);
}

function example()
    needs alloc.heap;
{
    let s: own String = String.from("data");
    print_text(s.as_str());  // Explicit conversion
    print_text(s);  // Or use deref coercion
}
```

---

**Get bytes:**
```cantrip
function as_bytes(self: String): [u8];
```

Returns a byte slice view of the String's UTF-8 data. Zero-copy operation.

**Example:**
```cantrip
let s: own String = String.from("ABC");
let bytes: [u8] = s.as_bytes();
assert!(bytes[0] == 65);  // 'A'
assert!(bytes[1] == 66);  // 'B'
assert!(bytes[2] == 67);  // 'C'
```

#### 5.6.8.3 Modification Operations (Requires own or mut)

**Append string:**
```cantrip
function push_str(self: mut String, s: str)
    needs alloc.heap;
```

Appends a string slice to the end of the String. May reallocate if capacity is insufficient.

**Example:**
```cantrip
function concatenate(a: str, b: str): own String
    needs alloc.heap;
{
    let mut result: own String = String.from(a);
    result.push_str(b);
    result
}
```

---

**Append character:**
```cantrip
function push(self: mut String, ch: char)
    needs alloc.heap;
```

Appends a Unicode scalar value to the String. Character is encoded as UTF-8 (1-4 bytes).

**Example:**
```cantrip
let mut s: own String = String.from("Hello");
s.push(' ');
s.push('🦀');
assert!(s.as_str() == "Hello 🦀");
```

---

**Insert string at position:**
```cantrip
function insert_str(self: mut String, idx: usize, s: str)
    needs alloc.heap;
    requires idx <= self.len();
    requires is_char_boundary(self, idx);
```

Inserts a string slice at the specified byte index. May reallocate. Index must be at a UTF-8 character boundary.

**Example:**
```cantrip
let mut s: own String = String.from("HelloWorld");
s.insert_str(5, ", ");  // Index 5 is after "Hello"
assert!(s.as_str() == "Hello, World");

// ERROR: Invalid boundary
// s.insert_str(1, "x");  // Panic if idx=1 is mid-character
```

---

**Remove range:**
```cantrip
function remove(self: mut String, start: usize, end: usize)
    requires start <= end;
    requires end <= self.len();
    requires is_char_boundary(self, start);
    requires is_char_boundary(self, end);
```

Removes characters in the byte range [start, end). Indices must be at UTF-8 character boundaries.

**Example:**
```cantrip
let mut s: own String = String.from("Hello, World!");
s.remove(5, 7);  // Remove ", "
assert!(s.as_str() == "HelloWorld!");
```

---

**Clear all contents:**
```cantrip
function clear(self: mut String);
```

Removes all characters from the String, setting length to 0. Capacity is unchanged.

**Example:**
```cantrip
let mut s: own String = String.from("temporary");
let old_cap: usize = s.capacity();

s.clear();
assert!(s.is_empty());
assert!(s.capacity() == old_cap);  // Capacity preserved
```

---

**Truncate to length:**
```cantrip
function truncate(self: mut String, new_len: usize)
    requires new_len <= self.len();
    requires is_char_boundary(self, new_len);
```

Shortens the String to the specified byte length. `new_len` must be at a character boundary.

**Example:**
```cantrip
let mut s: own String = String.from("Hello, World!");
s.truncate(5);  // Keep only "Hello"
assert!(s.as_str() == "Hello");
```

---

**Reserve additional capacity:**
```cantrip
function reserve(self: mut String, additional: usize)
    needs alloc.heap;
```

Reserves capacity for at least `additional` more bytes. May reallocate if current capacity is insufficient.

**Example:**
```cantrip
function build_string(parts: [str]): own String
    needs alloc.heap;
{
    let mut s: own String = String.new();
    let total: usize = parts.map(|p| p.len()).sum();

    s.reserve(total);  // Pre-allocate

    for part in parts {
        s.push_str(part);  // No reallocation
    }

    s
}
```

#### 5.6.8.4 View Operations (Zero-Copy)

**Slice to str:**
```cantrip
function slice(self: String, start: usize, end: usize): str
    requires start <= end;
    requires end <= self.len();
    requires is_char_boundary(self, start);
    requires is_char_boundary(self, end);
```

Returns a string slice view of the byte range [start, end). Zero-copy operation. Indices must be at UTF-8 character boundaries.

**Example:**
```cantrip
let s: own String = String.from("Hello, World!");
let hello: str = s.slice(0, 5);  // "Hello"
let world: str = s.slice(7, 12); // "World"
```

---

**Split by delimiter:**
```cantrip
function split(self: String, delimiter: str): Iterator<str>
    requires !delimiter.is_empty();
```

Returns an iterator over substrings separated by the delimiter. Delimiter is not included in results.

**Example:**
```cantrip
let s: own String = String.from("one,two,three");
let parts: [str] = s.split(",").collect();
assert!(parts.len() == 3);
assert!(parts[0] == "one");
assert!(parts[1] == "two");
assert!(parts[2] == "three");
```

---

**Get lines:**
```cantrip
function lines(self: String): Iterator<str>;
```

Returns an iterator over lines (substrings separated by `\n` or `\r\n`). Line terminators are not included in results.

**Example:**
```cantrip
let doc: own String = String.from("Line 1\nLine 2\nLine 3");
for line in doc.lines() {
    println(line);
}
// Output:
// Line 1
// Line 2
// Line 3
```

---

**Trim whitespace:**
```cantrip
function trim(self: String): str;
```

Returns a string slice with leading and trailing whitespace removed. Zero-copy operation.

**Example:**
```cantrip
let s: own String = String.from("  hello  ");
let trimmed: str = s.trim();
assert!(trimmed == "hello");
```

#### 5.6.8.5 Iteration Operations

**Iterate bytes:**
```cantrip
function bytes(self: String): Iterator<u8>;
```

Returns an iterator over the String's bytes. Each byte is yielded individually.

**Example:**
```cantrip
let s: own String = String.from("ABC");
for byte in s.bytes() {
    println("{byte}");  // 65, 66, 67
}
```

---

**Iterate characters:**
```cantrip
function chars(self: String): Iterator<char>;
```

Returns an iterator over Unicode scalar values (characters). Multi-byte UTF-8 sequences are decoded.

**Example:**
```cantrip
let s: own String = String.from("Hello🦀");
for ch in s.chars() {
    println("{ch}");  // H, e, l, l, o, 🦀
}
```

#### 5.6.8.6 Comparison Operations

**Equality (bytes):**
```cantrip
function equals(self: String, other: String): bool;
```

Returns `true` if both Strings contain identical byte sequences. Equivalent to `==` operator.

**Example:**
```cantrip
let a: own String = String.from("hello");
let b: own String = String.from("hello");
let c: own String = String.from("world");

assert!(a.equals(b));
assert!(!a.equals(c));
```

---

**Lexicographic comparison:**
```cantrip
function compare(self: String, other: String): Ordering;
```

Compares Strings lexicographically by byte values. Returns `Less`, `Equal`, or `Greater`.

**Example:**
```cantrip
let a: own String = String.from("apple");
let b: own String = String.from("banana");

match a.compare(b) {
    Ordering::Less -> println("apple < banana"),
    Ordering::Equal -> println("equal"),
    Ordering::Greater -> println("apple > banana"),
}
```

---

**Starts with prefix:**
```cantrip
function starts_with(self: String, prefix: str): bool;
```

Returns `true` if the String begins with the specified prefix.

**Example:**
```cantrip
let s: own String = String.from("https://example.com");
assert!(s.starts_with("https://"));
assert!(!s.starts_with("http://"));
```

---

**Ends with suffix:**
```cantrip
function ends_with(self: String, suffix: str): bool;
```

Returns `true` if the String ends with the specified suffix.

**Example:**
```cantrip
let filename: own String = String.from("document.txt");
assert!(filename.ends_with(".txt"));
assert!(!filename.ends_with(".pdf"));
```

### 5.6.9 Indexing and Iteration

#### 5.6.9.1 Why No Direct Indexing

String does NOT support direct indexing via `s[i]` syntax. This is a deliberate design decision based on UTF-8's variable-width encoding.

**Problem with character indexing:**

UTF-8 encodes Unicode scalar values using 1-4 bytes per character. Direct indexing creates ambiguity:

```cantrip
let s: own String = String.from("Hello🦀World");  // Contains emoji (4 bytes)

// If s[6] were allowed, what would it return?
// Option 1: Byte at index 6? (middle of emoji - INVALID UTF-8!)
// Option 2: 6th character? (requires O(n) scan from start)

// ERROR: s[6] is not allowed
// let ch = s[6];  // Compile error: String does not support indexing
```

**Why byte indexing is unsafe:**

```cantrip
// UTF-8 encoding of "Hello🦀":
// H     e     l     l     o     🦀              (characters)
// 0x48  0x65  0x6C  0x6C  0x6F  0xF0 0x9F 0xA6 0x80  (bytes)
// 0     1     2     3     4     5    6    7    8     (indices)

// Accessing index 6 would return 0x9F (middle of 🦀)
// This is NOT a valid UTF-8 sequence!
```

**Why character indexing is avoided:**

Users typically expect `[i]` to be O(1) constant-time. However, UTF-8 character access requires scanning from the start:

```cantrip
// To find the nth character, must scan from beginning
function nth_char_naive(s: String, n: usize): Option<char> {
    let mut count: usize = 0;
    for ch in s.chars() {
        if count == n {
            return Some(ch);
        }
        count = count + 1;
    }
    None
}
// O(n) operation!
```

**Rationale:**

By prohibiting `s[i]`, Cantrip makes performance characteristics explicit:
- Byte iteration: `s.bytes()` - clearly O(1) per byte
- Character iteration: `s.chars()` - clearly requires decoding
- Slicing: `s.slice(start, end)` - requires explicit boundary indices

#### 5.6.9.2 Byte Iteration

For byte-level access (protocols, parsing, low-level operations):

```cantrip
function process_bytes(s: String) {
    for byte in s.bytes() {
        // byte has type u8
        if byte == 0x0A {  // Newline
            println("Found newline");
        }
    }
}
```

**Performance:** O(1) per byte (direct array access).

**Use cases:**
- Binary protocol parsing
- ASCII-only processing
- Byte-level validation
- Low-level file I/O

#### 5.6.9.3 Character Iteration

For Unicode scalar value access:

```cantrip
function count_chars(s: String): usize {
    let mut count: usize = 0;
    for ch in s.chars() {
        // ch has type char (Unicode scalar value)
        count = count + 1;
    }
    count
}

let s: own String = String.from("Hello🦀");
assert!(count_chars(s) == 6);  // 5 ASCII + 1 emoji
```

**Performance:** O(n) to traverse entire string. Getting the nth character requires O(n) scan.

**Example: Finding nth character:**
```cantrip
function nth_char(s: String, n: usize): Option<char> {
    let mut iter = s.chars();
    iter.skip(n).next()  // O(n) operation
}

let s: own String = String.from("Hello");
assert!(nth_char(s, 0) == Some('H'));
assert!(nth_char(s, 4) == Some('o'));
assert!(nth_char(s, 10) == None);
```

#### 5.6.9.4 Grapheme Clusters

**Important caveat:** `.chars()` returns Unicode scalar values (code points), NOT grapheme clusters (user-perceived characters).

```cantrip
// Example: é can be represented two ways
let composed: str = "é";     // U+00E9 (single code point)
let decomposed: str = "é";   // U+0065 U+0301 (e + combining accent)

let s1: own String = String.from(composed);
let s2: own String = String.from(decomposed);

// Character counts differ!
assert!(s1.chars().count() == 1);  // 1 code point
assert!(s2.chars().count() == 2);  // 2 code points (e + accent)

// But they look identical when displayed:
println("{}", s1);  // é
println("{}", s2);  // é
```

**For true grapheme iteration:**

Use the Unicode segmentation library (not in core):

```cantrip
// Hypothetical library usage:
// use std.unicode.graphemes;
//
// for grapheme in s.graphemes() {
//     // grapheme is a user-perceived character
// }
```

#### 5.6.9.5 Safe Slicing

Slicing must respect UTF-8 character boundaries to preserve the UTF-8 invariant:

```cantrip
let s: own String = String.from("Hello🦀World");

// OK: Slices at character boundaries
let hello: str = s.slice(0, 5);  // "Hello"
let world: str = s.slice(9, 14); // "World"

// PANIC: Index 7 is mid-character (inside 🦀 emoji)
// let bad: str = s.slice(0, 7);  // Runtime panic!
// Error: "byte index 7 is not a char boundary"
```

**Character boundary rules:**

A byte index is a valid character boundary if:
1. It equals 0 or `s.len()`, OR
2. The byte at that index is NOT a UTF-8 continuation byte (0x80-0xBF)

```cantrip
// UTF-8 continuation bytes: 10xxxxxx (0x80-0xBF)
function is_char_boundary(s: String, idx: usize): bool {
    if idx > s.len() {
        return false;
    }
    if idx == 0 || idx == s.len() {
        return true;
    }

    let byte: u8 = s.as_bytes()[idx];
    (byte & 0xC0) != 0x80  // NOT a continuation byte
}
```

**Safe slicing pattern:**

When you need to slice at a specific character position:

```cantrip
function slice_first_n_chars(s: String, n: usize): str {
    let mut char_count: usize = 0;
    let mut byte_idx: usize = 0;

    for (idx, ch) in s.char_indices() {
        if char_count >= n {
            break;
        }
        byte_idx = idx + ch.len_utf8();
        char_count = char_count + 1;
    }

    s.slice(0, byte_idx)
}

let s: own String = String.from("Hello🦀World");
let first5: str = slice_first_n_chars(s, 5);
assert!(first5 == "Hello");
```

### 5.6.10 Examples and Patterns

#### 5.6.10.1 Basic String Usage

Simple string creation and manipulation:

```cantrip
function greet(name: str): own String
    needs alloc.heap;
{
    let mut msg: own String = String.from("Hello, ");
    msg.push_str(name);
    msg.push('!');
    msg  // Return owned string
}

function example()
    needs alloc.heap;
{
    let greeting: own String = greet("Alice");
    println(greeting);  // "Hello, Alice!"
}
```

**Explanation:** Build a greeting by starting with a base string, appending the name, and adding punctuation. The function returns owned String for the caller to use.

#### 5.6.10.2 String Building with Regions

Efficient string construction using temporary region allocation:

```cantrip
record Data {
    items: [Item];
}

record Item {
    name_val: str;
}

impl Item {
    function name(self: Item): str {
        self.name_val
    }
}

function format_report(data: Data): own String
    needs alloc.heap, alloc.region;
{
    region temp {
        let mut parts: own Vec<str> = Vec.new_in<temp>();

        // Build parts in temporary region
        for item in data.items {
            let mut s: own String = String.new_in<temp>();
            s.push_str("Item: ");
            s.push_str(item.name());
            parts.push(s.as_str());
        }

        // Join into final heap string
        let mut result: own String = String.new();
        for part in parts {
            result.push_str(part);
            result.push_str("\n");
        }

        result  // Heap-allocated, escapes region
    }  // All temporary strings freed here
}
```

**Explanation:** Temporary strings are allocated in a region to avoid heap fragmentation. The final result is built on the heap before the region ends, allowing it to escape. All temporary allocations are freed in bulk when the region ends.

#### 5.6.10.3 String Parsing Pattern

Parse and validate structured string input:

```cantrip
record Email {
    local: str;
    domain: str;
}

enum ParseError {
    Empty,
    InvalidFormat,
    MissingPart,
}

function parse_email(input: String): Result<Email, ParseError> {
    if input.is_empty() {
        return Err(ParseError::Empty);
    }

    // Split by @ symbol
    let mut parts = input.split("@");
    let parts_vec: [str] = parts.collect();

    if parts_vec.len() != 2 {
        return Err(ParseError::InvalidFormat);
    }

    let local: str = parts_vec[0];
    let domain: str = parts_vec[1];

    if local.is_empty() || domain.is_empty() {
        return Err(ParseError::MissingPart);
    }

    Ok(Email { local, domain })
}

function example()
    needs alloc.heap;
{
    let addr: own String = String.from("user@example.com");

    match parse_email(addr) {
        Ok(email) -> println("Local: {}, Domain: {}", email.local, email.domain),
        Err(ParseError::Empty) -> println("Empty email"),
        Err(ParseError::InvalidFormat) -> println("Invalid format"),
        Err(ParseError::MissingPart) -> println("Missing part"),
    }
}
```

**Explanation:** Validates email format by splitting on `@` and checking component counts. Uses Result type to return either parsed Email or specific error. No allocation needed for parsing (uses str slices).

#### 5.6.10.4 Zero-Copy String Processing

Process strings without allocation using string slices:

```cantrip
function count_words(text: String): usize {
    let mut count: usize = 0;

    for line in text.lines() {
        for word in line.split(" ") {
            let trimmed: str = word.trim();
            if !trimmed.is_empty() {
                count = count + 1;
            }
        }
    }

    count
}

function example()
    needs alloc.heap;
{
    let doc: own String = String.from("Hello world\nHow are you\n\nFine thanks");
    let words: usize = count_words(doc);
    println("Word count: {words}");  // 6
}
```

**Explanation:** All operations (`lines()`, `split()`, `trim()`) return `str` slices that borrow from the original String. No heap allocation occurs during processing, making this efficient for large texts.

#### 5.6.10.5 Common Pitfalls

**Pitfall 1: UTF-8 Boundary Violation**

```cantrip
// WRONG: Slicing mid-character
function bad_slice()
    needs alloc.heap;
{
    let s: own String = String.from("Hello🦀");

    // 🦀 is 4 bytes: 0xF0 0x9F 0xA6 0x80
    // Byte indices: H=0, e=1, l=2, l=3, o=4, 🦀=5-8

    let bad: str = s.slice(0, 7);  // PANIC at runtime!
    // Error: "byte index 7 is not a char boundary"
}

// CORRECT: Use character iteration or valid boundaries
function good_slice()
    needs alloc.heap;
{
    let s: own String = String.from("Hello🦀");

    // Option 1: Slice at known character boundaries
    let hello: str = s.slice(0, 5);  // "Hello"

    // Option 2: Use character iteration
    let first5: own String = String.new();
    for (idx, ch) in s.chars().enumerate() {
        if idx < 5 {
            first5.push(ch);
        }
    }
}
```

**Explanation:** UTF-8 multi-byte characters cannot be split. Index 7 falls inside the 🦀 emoji (4 bytes at indices 5-8), causing a panic. Always slice at character boundaries or use character-based iteration.

---

**Pitfall 2: Multiple Mutable References (Use-After-Realloc)**

```cantrip
// DANGEROUS: Reference invalidation
function dangerous(s: mut String)
    needs alloc.heap;
{
    let bytes: [u8] = s.as_bytes();  // Get reference to byte data
    let first: u8 = bytes[0];        // Read from reference

    s.push_str("x");  // Modifies String, MAY reallocate!

    // bytes pointer is now INVALID if reallocation occurred!
    // Accessing bytes[1] could read freed memory (undefined behavior)
    // let second: u8 = bytes[1];  // USE-AFTER-FREE!
}

// SAFE: Don't hold references across mutations
function safe(s: mut String)
    needs alloc.heap;
{
    // Copy values, not references
    let len: usize = s.len();
    let first: u8 = s.as_bytes()[0];

    s.push_str("x");  // OK: no outstanding references

    // Re-acquire reference after mutation
    let new_len: usize = s.len();
    assert!(new_len == len + 1);
}
```

**Explanation:** Unlike Rust, Cantrip allows multiple mutable references and doesn't prevent invalidation. Programmer must ensure references aren't held across reallocating operations. Copy values instead of holding references.

---

**Pitfall 3: Assuming Character Count = Byte Count**

```cantrip
// WRONG: Confusing bytes and characters
function bad_count()
    needs alloc.heap;
{
    let s: own String = String.from("Hello🦀");
    let char_count: usize = s.len();  // Returns 10 (bytes), not 6!
    println("Characters: {char_count}");  // Wrong: prints 10
}

// CORRECT: Use chars().count()
function good_count()
    needs alloc.heap;
{
    let s: own String = String.from("Hello🦀");

    let byte_count: usize = s.len();           // 10 bytes
    let char_count: usize = s.chars().count(); // 6 characters

    println("Bytes: {byte_count}");      // 10
    println("Characters: {char_count}"); // 6
}
```

**Explanation:** `len()` returns byte length, not character count. Use `chars().count()` for character count, but note this is O(n) because it must decode the entire UTF-8 string.

---

**Pitfall 4: Region Escape Attempt**

```cantrip
// WRONG: Trying to return region-allocated string
function bad_region(): own String
    needs alloc.region;
{
    region temp {
        let s: own String = String.new_in<temp>();
        s.push_str("data");
        s  // ERROR: cannot escape region (lifetime violation)
    }
}

// CORRECT: Convert to heap before returning
function good_region(): own String
    needs alloc.heap, alloc.region;
{
    region temp {
        let s: own String = String.new_in<temp>();
        s.push_str("data");

        // Copy to heap before escaping region
        String.from(s.as_str())  // Heap allocation
    }  // s freed here (region ends)
}
```

**Explanation:** Region-allocated Strings cannot escape their region's scope. To return a region-built string, copy it to the heap using `String.from()` before the region ends.

---

**Pitfall 5: Grapheme vs. Character Count**

```cantrip
// WRONG: Assuming chars() gives grapheme count
function bad_grapheme()
    needs alloc.heap;
{
    // "é" can be U+00E9 (composed) or U+0065 U+0301 (decomposed)
    let s1: own String = String.from("é");  // Composed (1 code point)
    let s2: own String = String.from("é");  // Decomposed (2 code points)

    // Character counts differ!
    assert!(s1.chars().count() == 1);  // Passes
    assert!(s2.chars().count() == 1);  // FAILS! (actually 2)
}

// CORRECT: Be aware of normalization
function good_grapheme()
    needs alloc.heap;
{
    let s: own String = String.from("é");  // Could be 1 or 2 code points

    // For user-perceived character count, need Unicode library:
    // use std.unicode.graphemes;
    // let grapheme_count = s.graphemes().count();

    // Or normalize before counting:
    // let normalized = s.nfc();  // NFC normalization
    // let char_count = normalized.chars().count();
}
```

**Explanation:** Unicode allows multiple representations of the same visual character. `chars()` counts code points, not graphemes (user-perceived characters). For accurate user-visible character counts, use Unicode segmentation libraries.

---

**Previous**: [Never Type](01e_Never.md) | **Next**: [Arrays and Slices](02_ArraysAndSlices.md)
