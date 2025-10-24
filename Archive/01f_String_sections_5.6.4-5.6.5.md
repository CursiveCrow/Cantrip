### 5.6.4 Dynamic Semantics

#### 5.6.4.1 Evaluation Rules

**String literal evaluation:**

String literals evaluate to String values pointing to static read-only memory (.rodata section). No heap allocation occurs during evaluation.

```
[E-String-Literal]
"text" stored at static address addr
len = byte_length("text")
─────────────────────────────────────────────────────────────
⟨"text", σ⟩ ⇓ ⟨String{ptr: addr, len: len, cap: len}, σ⟩
```

**Explanation:** String literals are embedded in the executable's read-only data section. The resulting String value is a fat pointer to this static memory. Since no heap allocation is performed, the store σ remains unchanged.

**String heap allocation:**

Creating a new owned string allocates an empty buffer on the heap with default capacity.

```
[E-String-New]
alloc_heap(DEFAULT_CAP) = addr
─────────────────────────────────────────────────────────────
⟨String.new(), σ⟩ ⇓ ⟨String{ptr: addr, len: 0, cap: DEFAULT_CAP}, σ'⟩
    where σ' = σ ∪ {addr ↦ [0u8; DEFAULT_CAP]}
```

**Explanation:** String.new() allocates a heap buffer and returns an owned String with length 0. The capacity (DEFAULT_CAP) is implementation-defined but typically 8-16 bytes. The store σ' includes the new allocation.

**String from UTF-8 validation:**

Constructing a string from untrusted bytes requires validation to ensure UTF-8 correctness.

```
[E-String-From-UTF8-Valid]
⟨bytes, σ⟩ ⇓ ⟨data, σ'⟩
validate_utf8(data) = true
alloc_heap(data.len) = addr
memcpy(addr, data.ptr, data.len)
─────────────────────────────────────────────────────────────
⟨String.from_utf8(bytes), σ⟩ ⇓ ⟨Ok(String{ptr: addr, len: data.len, cap: data.len}), σ''⟩
    where σ'' = σ' ∪ {addr ↦ data}

[E-String-From-UTF8-Invalid]
⟨bytes, σ⟩ ⇓ ⟨data, σ'⟩
validate_utf8(data) = false
─────────────────────────────────────────────────────────────
⟨String.from_utf8(bytes), σ⟩ ⇓ ⟨Err(Utf8Error), σ'⟩
```

**Explanation:** UTF-8 validation is performed in O(n) time where n = byte length. Valid sequences result in heap allocation and Ok(String). Invalid sequences return Err without allocating.

**String concatenation (if included):**

Concatenating two strings may require reallocation if capacity is exceeded.

```
[E-String-Concat-No-Realloc]
⟨s1, σ⟩ ⇓ ⟨String{ptr: p1, len: l1, cap: c1}, σ'⟩
⟨s2, σ'⟩ ⇓ ⟨String{ptr: p2, len: l2, cap: c2}, σ''⟩
l1 + l2 ≤ c1
memcpy(p1 + l1, p2, l2)
─────────────────────────────────────────────────────────────
⟨s1.push_str(s2), σ⟩ ⇓ ⟨(), σ'''⟩
    where σ'''(p1).len = l1 + l2

[E-String-Concat-Realloc]
⟨s1, σ⟩ ⇓ ⟨String{ptr: p1, len: l1, cap: c1}, σ'⟩
⟨s2, σ'⟩ ⇓ ⟨String{ptr: p2, len: l2, cap: c2}, σ''⟩
l1 + l2 > c1
new_cap = max(c1 × 2, l1 + l2)
alloc_heap(new_cap) = addr
memcpy(addr, p1, l1)
memcpy(addr + l1, p2, l2)
dealloc_heap(p1)
─────────────────────────────────────────────────────────────
⟨s1.push_str(s2), σ⟩ ⇓ ⟨(), σ'''⟩
    where σ''' = (σ'' \ {p1 ↦ _}) ∪ {addr ↦ [l1 + l2 bytes]}
          s1 = String{ptr: addr, len: l1 + l2, cap: new_cap}
```

**Explanation:** If the existing capacity is sufficient, bytes are appended in-place. Otherwise, a new buffer is allocated with doubled capacity (or exact size if doubling is insufficient), existing data is copied, new data is appended, and the old buffer is freed.

**String slicing:**

Slicing a string creates a new string view (borrows the underlying data).

```
[E-String-Slice]
⟨s, σ⟩ ⇓ ⟨String{ptr: p, len: l, cap: c}, σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b ≤ l
is_char_boundary(p, a) ∧ is_char_boundary(p, b)
─────────────────────────────────────────────────────────────
⟨s[start..end], σ⟩ ⇓ ⟨String{ptr: p + a, len: b - a, cap: b - a}, σ'''⟩

[E-String-Slice-Invalid-Boundary]
⟨s, σ⟩ ⇓ ⟨String{ptr: p, len: l, cap: c}, σ'⟩
⟨start, σ'⟩ ⇓ ⟨a, σ''⟩
⟨end, σ''⟩ ⇓ ⟨b, σ'''⟩
a ≤ b ≤ l
¬(is_char_boundary(p, a) ∧ is_char_boundary(p, b))
─────────────────────────────────────────────────────────────
⟨s[start..end], σ⟩ ⇓ panic("byte index not at char boundary")
```

**Explanation:** String slicing must occur at UTF-8 character boundaries. A byte index is a character boundary if it is 0, equal to the string length, or points to a byte that is not a UTF-8 continuation byte (i.e., not in range 0x80-0xBF). Invalid boundaries cause runtime panic.

**Character boundary predicate:**
```
is_char_boundary(ptr, idx) ⟺
    idx = 0 ∨
    idx = len ∨
    (ptr[idx] & 0xC0) ≠ 0x80
```

#### 5.6.4.2 Memory Representation

**Record Layout:**

The String type is represented as a fat pointer with three fields:

```
String record layout (24 bytes on 64-bit):
┌─────────────────┬──────────────┬──────────────┐
│ ptr: *u8        │ len: usize   │ cap: usize   │
│ 8 bytes         │ 8 bytes      │ 8 bytes      │
└─────────────────┴──────────────┴──────────────┘
Offset:  0              8               16

Field Semantics:
• ptr:  Pointer to UTF-8 encoded byte array
• len:  Number of BYTES currently used (NOT character count)
• cap:  Total capacity in BYTES (for owned strings only)
```

**Size and alignment:**
```
sizeof(String)  = 24 bytes  (on 64-bit platforms)
                = 12 bytes  (on 32-bit platforms)
alignof(String) = 8 bytes   (on 64-bit platforms)
                = 4 bytes   (on 32-bit platforms)
```

**Important distinctions:**
- `len` represents byte length, not character count
- `cap` represents allocated byte capacity
- Character count requires O(n) iteration over UTF-8 sequences

**String literal storage:**

String literals are stored in the executable's read-only data section:

```
.rodata section (static memory):
┌─────────────────────────────────┐
│ string_0: "Hello, World!\0"     │ ← addr_0
│ string_1: "Cantrip\0"           │ ← addr_1
│ string_2: "🦀 Rust inspired\0"  │ ← addr_2
└─────────────────────────────────┘

String literal expression "Hello, World!" evaluates to:
String {
    ptr: addr_0,
    len: 13,      // byte length (no null terminator in len)
    cap: 13,
}
```

**Properties of literal strings:**
- Immutable (stored in read-only memory)
- Zero allocation cost (no heap usage)
- Lifetime: 'static (valid for entire program execution)
- Null-terminated in .rodata for C FFI, but null not included in len

**Heap-allocated strings:**

Owned strings allocate a contiguous buffer on the heap:

```
Heap memory layout:
┌──────────────────────────────────────┐
│ UTF-8 byte array                     │
│ [capacity bytes allocated]           │
│ [first 'len' bytes used]             │
│ [remaining bytes unused/uninitialized]│
└──────────────────────────────────────┘
          ↑
          │ (points to start)
          │
String { ptr: ─┘, len: used_bytes, cap: total_allocated }
```

**Example:**
```cantrip
let mut s: own String = String.new();  // Allocate with DEFAULT_CAP
s.push_str("Hello");                   // Append 5 bytes

Heap state:
┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
│H│e│l│l│o│?│?│?│?│?│?│?│?│?│?│?│  (assuming DEFAULT_CAP = 16)
└─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
 ↑ len = 5              ↑ cap = 16
 ptr
```

**UTF-8 byte encoding in memory:**

Characters are stored as UTF-8 byte sequences:

```
Character: 'A'     → [0x41]                     (1 byte, ASCII)
Character: '©'     → [0xC2, 0xA9]               (2 bytes)
Character: '中'    → [0xE4, 0xB8, 0xAD]         (3 bytes)
Character: '🦀'    → [0xF0, 0x9F, 0xA6, 0x80]   (4 bytes, emoji)

String "A©中🦀" memory layout:
┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
│0x41│0xC2│0xA9│0xE4│0xB8│0xAD│0xF0│0x9F│0xA6│0x80│
└────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
  A      ©           中              🦀
 1 byte  2 bytes    3 bytes         4 bytes

Total: 10 bytes, 4 characters
len = 10 (byte length)
s.chars().count() = 4 (character count, requires iteration)
```

**Memory layout theorem:**

**Theorem 5.6.1 (String Memory Invariants):**

For any String value `s = String{ptr: p, len: l, cap: c}`, the following invariants hold:

1. **Capacity constraint:** `0 ≤ l ≤ c`
2. **UTF-8 validity:** `validate_utf8(p[0..l]) = true`
3. **Allocation validity:** If `c > 0`, then `p` points to valid heap allocation of size `c`
4. **Null-pointer invariant:** If `c = 0`, then `p` may be null or dangling (zero-capacity strings need not allocate)
5. **Alignment:** `p` is aligned to 1-byte boundary (no alignment requirement for u8 arrays)

**Proof sketch:**
- Invariant (1) enforced by all String operations (construction, push, etc.)
- Invariant (2) enforced by UTF-8 validation in String.from_utf8() and maintained by all operations
- Invariants (3-4) enforced by allocator interface and String.new() semantics
- Invariant (5) follows from u8 alignment requirements (trivially satisfied)

#### 5.6.4.3 Operational Behavior

**Allocation strategy:**

**String literals (zero-cost):**
- Storage: Read-only data section (.rodata)
- Allocation: None (embedded in executable)
- Mutability: Immutable (any attempt to modify causes compiler error or runtime fault)
- Lifetime: 'static (valid for entire program execution)
- Cost: Zero runtime overhead

**Owned strings (heap-allocated):**
- Storage: Heap memory
- Allocation: Dynamic via allocator (requires `needs alloc.heap`)
- Mutability: Mutable if declared with `mut`
- Lifetime: Lexically scoped (freed when owner goes out of scope)
- Cost: Allocation/deallocation overhead, possible reallocation on growth

**Growth strategy (capacity management):**

When appending data exceeds current capacity, strings employ an amortized doubling strategy:

```
new_capacity = max(old_capacity × 2, required_size)
```

**Example growth sequence:**
```
Initial:     cap = 0,  len = 0
After "Hi":  cap = 8,  len = 2   (allocated with DEFAULT_CAP)
After 6 more bytes (len=8):  cap = 8,  len = 8   (fits, no realloc)
After 1 more byte (len=9):   cap = 16, len = 9   (realloc: 8 × 2 = 16)
After 7 more bytes (len=16): cap = 16, len = 16  (fits, no realloc)
After 1 more byte (len=17):  cap = 32, len = 17  (realloc: 16 × 2 = 32)
```

**Amortized analysis:**
- Individual push operations: O(1) amortized
- Worst case (reallocation): O(n) where n = current length
- Total cost of n pushes: O(n) amortized

**Deallocation behavior:**

Owned strings (`own String`) are automatically deallocated when their owner's scope ends:

```cantrip
function example()
    needs alloc.heap;
{
    let s: own String = String.new();  // Allocate heap buffer
    s.push_str("data");                // Append data (may realloc)
    // Automatic deallocation occurs here
}
```

**Deallocation rule:**
```
[E-String-Dealloc]
⟨{ let s: own String = e; body }, σ⟩ ⇓ ⟨v, σ'⟩
s = String{ptr: p, len: l, cap: c}
c > 0
─────────────────────────────────────────────────────────────
final σ'' = σ' \ {p ↦ _}  (heap buffer freed)
```

**Exception:** Zero-capacity strings (empty string constants) may not allocate, hence no deallocation.

**Permission transitions:**

Unlike Rust, Cantrip allows multiple mutable references through region-based lifetime management:

```
Ownership transitions:
own String ────→ String      (pass as immutable reference)
own String ────→ mut String  (pass as mutable reference)

Region guarantees:
• Multiple `mut String` references can coexist
• Region system (not borrow checker) ensures lifetime safety
• All references must not outlive the region containing the owned data
```

**Example:**
```cantrip
function process(s1: mut String, s2: mut String)
    needs alloc.heap;
{
    s1.push_str("A");
    s2.push_str("B");  // OK: multiple mutable references allowed
}

let mut owner: own String = String.new();
process(owner, owner);  // OK in Cantrip (unlike Rust)
                        // Regions ensure owner outlives both references
```

**Performance characteristics:**

```
Operation              Time Complexity    Space Complexity
──────────────────────────────────────────────────────────
String literal         O(1)               O(1) - static memory
String.new()           O(1)               O(cap) - heap allocation
push_str(s)            O(m) amortized     O(m) worst case (realloc)
                       where m = s.len
s[a..b]                O(1)               O(1) - fat pointer creation
s.chars()              O(n)               O(1) - iterator creation
s.chars().count()      O(n)               O(1)
validate_utf8(s)       O(n)               O(1)
──────────────────────────────────────────────────────────
where n = byte length of string
```

**Memory overhead:**
- String struct: 24 bytes (on 64-bit)
- Heap allocation: len + (cap - len) bytes
- Minimum overhead: 24 bytes + 0-16 bytes (DEFAULT_CAP)

**Cache efficiency:**
- UTF-8 data stored contiguously for excellent cache locality
- Sequential iteration exhibits near-optimal cache behavior
- Random character access requires O(n) scan (UTF-8 is variable-width)

### 5.6.5 UTF-8 Encoding

**Definition 5.6.2 (UTF-8 Encoding):** UTF-8 is a variable-width character encoding for Unicode, using 1-4 bytes per Unicode scalar value. All String values in Cantrip MUST contain valid UTF-8 byte sequences.

#### 5.6.5.1 UTF-8 Format Specification

UTF-8 encodes Unicode scalar values (U+0000 to U+10FFFF, excluding surrogates U+D800-U+DFFF) using 1-4 bytes:

**1-byte sequence (ASCII range: U+0000 to U+007F):**
```
Byte pattern: 0xxxxxxx
Range:        U+0000 to U+007F (0 to 127 decimal)
Bits:         7 bits for codepoint
```

**Examples:**
```
'A'   (U+0041) → [01000001]        → 0x41
'0'   (U+0030) → [00110000]        → 0x30
'\n'  (U+000A) → [00001010]        → 0x0A
' '   (U+0020) → [00100000]        → 0x20
```

**2-byte sequence (U+0080 to U+07FF):**
```
Byte pattern: 110xxxxx 10xxxxxx
Range:        U+0080 to U+07FF (128 to 2,047 decimal)
Bits:         11 bits for codepoint (5 + 6)
```

**Examples:**
```
'©'   (U+00A9) → [11000010 10101001] → 0xC2 0xA9
'é'   (U+00E9) → [11000011 10101001] → 0xC3 0xA9
'Ω'   (U+03A9) → [11001110 10101001] → 0xCE 0xA9
```

**3-byte sequence (U+0800 to U+FFFF, excluding surrogates):**
```
Byte pattern: 1110xxxx 10xxxxxx 10xxxxxx
Range:        U+0800 to U+FFFF (2,048 to 65,535 decimal)
              EXCLUDING U+D800 to U+DFFF (surrogate pairs)
Bits:         16 bits for codepoint (4 + 6 + 6)
```

**Examples:**
```
'中'  (U+4E2D) → [11100100 10111000 10101101] → 0xE4 0xB8 0xAD
'€'   (U+20AC) → [11100010 10000010 10101100] → 0xE2 0x82 0xAC
'—'   (U+2014) → [11100010 10000000 10010100] → 0xE2 0x80 0x94
```

**4-byte sequence (U+10000 to U+10FFFF):**
```
Byte pattern: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
Range:        U+10000 to U+10FFFF (65,536 to 1,114,111 decimal)
Bits:         21 bits for codepoint (3 + 6 + 6 + 6)
```

**Examples:**
```
'🦀'  (U+1F980) → [11110000 10011111 10100110 10000000] → 0xF0 0x9F 0xA6 0x80
'🚀'  (U+1F680) → [11110000 10011111 10011010 10000000] → 0xF0 0x9F 0x9A 0x80
'😀'  (U+1F600) → [11110000 10011111 10011000 10000000] → 0xF0 0x9F 0x98 0x80
```

#### 5.6.5.2 Encoding Algorithm

**Encoding a Unicode scalar value to UTF-8:**

```
encode_utf8(codepoint: u32) → [u8]:
    if codepoint ≤ 0x7F:
        return [codepoint]
    else if codepoint ≤ 0x7FF:
        return [
            0xC0 | (codepoint >> 6),
            0x80 | (codepoint & 0x3F)
        ]
    else if codepoint ≤ 0xFFFF:
        if 0xD800 ≤ codepoint ≤ 0xDFFF:
            error("surrogate codepoint invalid")
        return [
            0xE0 | (codepoint >> 12),
            0x80 | ((codepoint >> 6) & 0x3F),
            0x80 | (codepoint & 0x3F)
        ]
    else if codepoint ≤ 0x10FFFF:
        return [
            0xF0 | (codepoint >> 18),
            0x80 | ((codepoint >> 12) & 0x3F),
            0x80 | ((codepoint >> 6) & 0x3F),
            0x80 | (codepoint & 0x3F)
        ]
    else:
        error("codepoint out of range")
```

#### 5.6.5.3 Invalid Sequences

**Normative requirement:** The following byte sequences are INVALID in UTF-8 and MUST be rejected by validation:

**1. Overlong encodings:**

Encoding a codepoint with more bytes than necessary.

```
Invalid: 'A' (U+0041) as [11000001 10000001] → 0xC1 0x81  (2-byte overlong)
Valid:   'A' (U+0041) as [01000001]           → 0x41      (1-byte correct)

Invalid: '©' (U+00A9) as [11100000 10000010 10101001] → 0xE0 0x82 0xA9  (3-byte overlong)
Valid:   '©' (U+00A9) as [11000010 10101001]           → 0xC2 0xA9      (2-byte correct)
```

**Detection rule:** A sequence is overlong if the codepoint value could be encoded in fewer bytes.

**2. UTF-16 surrogate halves (U+D800 to U+DFFF):**

These codepoints are reserved for UTF-16 encoding and are invalid in UTF-8.

```
Invalid: [11101101 10100000 10000000] → 0xED 0xA0 0x80  (U+D800, high surrogate)
Invalid: [11101101 10111111 10111111] → 0xED 0xBF 0xBF  (U+DFFF, low surrogate)
```

**Rationale:** UTF-8 encodes Unicode scalar values directly; surrogate pairs are a UTF-16 encoding artifact.

**3. Codepoints beyond U+10FFFF:**

Unicode defines codepoints only up to U+10FFFF.

```
Invalid: [11110100 10010000 10000000 10000000] → 0xF4 0x90 0x80 0x80  (U+110000)
Invalid: [11111000 10000000 10000000 10000000] → 0xF8 0x80 0x80 0x80  (5-byte sequence)
```

**Maximum valid codepoint:** U+10FFFF

**4. Incomplete sequences:**

Missing continuation bytes.

```
Invalid: [11000010]                 → 0xC2      (2-byte lead, missing continuation)
Invalid: [11100100 10111000]        → 0xE4 0xB8 (3-byte lead, only 1 continuation)
Invalid: [11110000 10011111 10100110] → 0xF0 0x9F 0xA6 (4-byte lead, only 2 continuations)
```

**5. Invalid continuation bytes:**

Continuation bytes outside the range 0x80-0xBF.

```
Invalid: [11000010 01000001]        → 0xC2 0x41 (second byte not in 0x80-0xBF)
Invalid: [11100100 11000000 10101101] → 0xE4 0xC0 0xAD (second byte not continuation)
```

**6. Unexpected continuation bytes:**

Continuation bytes without a leading byte.

```
Invalid: [10101001]                 → 0xA9      (continuation byte with no lead)
Invalid: [01000001 10101001]        → 0x41 0xA9 (continuation after ASCII)
```

**7. Invalid leading bytes:**

Bytes 0xC0, 0xC1, 0xF5-0xFF are never valid UTF-8 leading bytes.

```
Invalid: [11000000 10000000]        → 0xC0 0x80 (invalid leading byte 0xC0)
Invalid: [11111000 10000000 ...]    → 0xF8 ...  (5-byte sequence, invalid)
```

#### 5.6.5.4 Validation

**Validation requirement:** All String construction from untrusted bytes MUST perform UTF-8 validation.

**Validation algorithm complexity:** O(n) where n = byte length

**Validation predicate:**

```
validate_utf8(bytes: [u8]) → bool:
    let i = 0
    while i < bytes.len:
        let b0 = bytes[i]

        // 1-byte sequence (ASCII)
        if b0 < 0x80:
            i += 1
            continue

        // Invalid leading bytes
        if b0 < 0xC2 || b0 > 0xF4:
            return false

        // 2-byte sequence
        if b0 < 0xE0:
            if i + 1 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]):
                return false
            let cp = ((b0 & 0x1F) << 6) | (bytes[i + 1] & 0x3F)
            if cp < 0x80:
                return false  // overlong
            i += 2
            continue

        // 3-byte sequence
        if b0 < 0xF0:
            if i + 2 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]) || !is_continuation(bytes[i + 2]):
                return false
            let cp = ((b0 & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F)
            if cp < 0x800:
                return false  // overlong
            if 0xD800 ≤ cp ≤ 0xDFFF:
                return false  // surrogate
            i += 3
            continue

        // 4-byte sequence
        if b0 < 0xF5:
            if i + 3 >= bytes.len:
                return false  // incomplete
            if !is_continuation(bytes[i + 1]) || !is_continuation(bytes[i + 2]) || !is_continuation(bytes[i + 3]):
                return false
            let cp = ((b0 & 0x07) << 18) | ((bytes[i + 1] & 0x3F) << 12) | ((bytes[i + 2] & 0x3F) << 6) | (bytes[i + 3] & 0x3F)
            if cp < 0x10000:
                return false  // overlong
            if cp > 0x10FFFF:
                return false  // out of range
            i += 4
            continue

        return false  // invalid leading byte

    return true

is_continuation(byte: u8) → bool:
    return (byte & 0xC0) == 0x80
```

**Safe construction API:**

```cantrip
function String.from_utf8(bytes: [u8]): Result<own String, Utf8Error>
    needs alloc.heap;
{
    if validate_utf8(bytes) {
        let mut s = String.new();
        s.reserve(bytes.len);
        memcpy(s.ptr, bytes.ptr, bytes.len);
        s.len = bytes.len;
        Ok(s)
    } else {
        Err(Utf8Error::InvalidSequence)
    }
}
```

#### 5.6.5.5 Guarantees and Invariants

**Invariant 5.6.1 (UTF-8 Validity):** All String values MUST contain valid UTF-8 at all times.

**Formally:**
```
∀s : String. validate_utf8(s.as_bytes()) = true
```

**Operations that maintain invariant:**

1. **String literals (compiler validated):**
   ```cantrip
   let s = "Hello, 世界";  // Compiler ensures UTF-8 validity
   ```

2. **String.new() (creates empty valid string):**
   ```cantrip
   let s = String.new();  // Empty string is trivially valid UTF-8
   ```

3. **String.from_utf8() (validates or errors):**
   ```cantrip
   let s = String.from_utf8(bytes)?;  // Returns Err if invalid
   ```

4. **push_str(valid_string) (concatenation preserves validity):**
   ```cantrip
   s1.push_str(s2);  // If s1 and s2 valid, result is valid
   ```

5. **Slicing at character boundaries:**
   ```cantrip
   let slice = s[0..5];  // Panics if not at char boundary
   ```

**Unsafe operations (programmer responsibility):**

Unsafe operations bypass validation and place the burden on the programmer:

```cantrip
// UNSAFE: Programmer must ensure `bytes` contains valid UTF-8
function String.from_utf8_unchecked(bytes: [u8]): own String
    needs alloc.heap, unsafe.memory;
{
    // No validation performed
    let mut s = String.new();
    s.reserve(bytes.len);
    memcpy(s.ptr, bytes.ptr, bytes.len);
    s.len = bytes.len;
    s
}
```

**Theorem 5.6.2 (UTF-8 Invariant Preservation):**

If `s1` and `s2` are valid UTF-8 strings, then:
1. `s1.push_str(s2)` produces a valid UTF-8 string
2. `s1 + s2` (if concatenation operator exists) produces a valid UTF-8 string
3. `s[a..b]` produces a valid UTF-8 string if `a` and `b` are character boundaries

**Proof sketch:**
1. Valid UTF-8 sequences concatenated produce a valid UTF-8 sequence
2. Slicing at character boundaries preserves sequence validity
3. No operation modifies the byte content except by appending valid UTF-8

#### 5.6.5.6 Character Iteration

**Iterating over characters (not bytes):**

String provides an iterator that yields `char` values by decoding UTF-8:

```cantrip
function String.chars(self: String): CharIterator {
    CharIterator { ptr: self.ptr, end: self.ptr + self.len }
}

impl Iterator<char> for CharIterator {
    function next(mut self): Option<char> {
        if self.ptr >= self.end {
            return None;
        }

        let (ch, byte_len) = decode_utf8_char(self.ptr);
        self.ptr += byte_len;
        Some(ch)
    }
}
```

**UTF-8 decoding algorithm:**

```
decode_utf8_char(ptr: *u8) → (char, usize):
    let b0 = *ptr

    if b0 < 0x80:  // 1-byte (ASCII)
        return (b0 as char, 1)

    if b0 < 0xE0:  // 2-byte
        let b1 = *(ptr + 1)
        let cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F)
        return (char::from_u32_unchecked(cp), 2)

    if b0 < 0xF0:  // 3-byte
        let b1 = *(ptr + 1)
        let b2 = *(ptr + 2)
        let cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F)
        return (char::from_u32_unchecked(cp), 3)

    // 4-byte
    let b1 = *(ptr + 1)
    let b2 = *(ptr + 2)
    let b3 = *(ptr + 3)
    let cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F)
    return (char::from_u32_unchecked(cp), 4)
```

**Note:** `from_u32_unchecked` is safe here because the String invariant guarantees valid UTF-8.

**Example:**
```cantrip
let s = "A©中🦀";
for ch in s.chars() {
    // Yields: 'A', '©', '中', '🦀'
}

// Byte iteration (raw UTF-8 bytes)
for byte in s.as_bytes() {
    // Yields: 0x41, 0xC2, 0xA9, 0xE4, 0xB8, 0xAD, 0xF0, 0x9F, 0xA6, 0x80
}
```

---

**Previous**: [Static Semantics (§5.6.3)](01f_String.md#563-static-semantics) | **Next**: [Examples and Patterns (§5.6.6)](01f_String.md#566-examples-and-patterns)
