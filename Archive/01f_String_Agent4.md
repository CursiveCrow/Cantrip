### 5.6.6 Permission Integration

**Definition 5.6.6.1 (String Permissions):** The `String` type interacts with Cantrip's Lexical Permission System through three permission modes: immutable reference (default), mutable reference (`mut`), and owned (`own`).

**Philosophy:** Cantrip follows the Cyclone model, not Rust. Permissions control *access rights*, while regions control *lifetimes*. Multiple mutable references are allowed—programmer's responsibility to prevent aliasing bugs.

---

#### Immutable Reference (Default)

**Syntax:** `String` or `text: String`

**Type rule:**
```
[T-String-Ref]
Γ ⊢ s : String    immutable(s)
─────────────────────────────────
Γ ⊢ function f(x: String) accepts reference to s
```

**Semantics:**
- Can read string contents
- Cannot modify contents
- Cannot take ownership
- Many references can coexist (no borrow checker)

**Operations allowed:**
```cantrip
function get_length(s: String): usize {
    s.len()              // OK: reading
}

function get_char(s: String, idx: usize): char {
    s.char_at(idx)       // OK: indexing
}

function compare(s1: String, s2: String): bool {
    s1 == s2             // OK: comparison
}
```

**Operations prohibited:**
```cantrip
function illegal_modify(s: String) {
    s.push_str("!");     // ERROR E3002: Cannot modify immutable reference
    s.clear();           // ERROR E3002: Cannot modify immutable reference
}
```

**Example - Multiple references allowed:**
```cantrip
function process_multiple(text: String)
    needs io.write;
{
    let len1 = get_length(text);      // OK: first reference
    let len2 = get_length(text);      // OK: can pass again
    compare(text, text);              // OK: two references simultaneously

    // text still accessible here
    std.io.println(text);
}
```

**Formal semantics:**
```
⟦String⟧ = { reference to UTF-8 byte sequence }
Operations: Read-only (len, char_at, slice, ==, <, etc.)
Aliasing: Unlimited
```

---

#### Mutable Reference

**Syntax:** `mut String` or `text: mut String`

**Type rule:**
```
[T-String-Mut]
Γ ⊢ s : String    mutable(s) = true
─────────────────────────────────────
Γ ⊢ s : mut String
```

**Semantics:**
- Can read and modify string contents
- Cannot take ownership (remains with caller)
- Multiple `mut` references CAN coexist (programmer's responsibility!)

**Operations allowed:**
```cantrip
function modify_string(s: mut String) {
    s.push_str("!");           // OK: can append
    s.clear();                 // OK: can clear
    s[0] = 'X';                // OK: can modify bytes (if valid UTF-8)
    let len = s.len();         // OK: can also read
}
```

**Example - Basic mutation:**
```cantrip
function append_exclamation(s: mut String) {
    s.push_str("!");
}

function example()
    needs alloc.heap;
{
    var text: own String = String.from("Hello");
    append_exclamation(mut text);
    // text now contains "Hello!"
    std.io.println(text);  // Prints: Hello!
}
```

**CRITICAL DIFFERENCE FROM RUST:** Multiple mutable references allowed:

```cantrip
function modify1(s: mut String) {
    s.push_str(" world");
}

function modify2(s: mut String) {
    s.push_str("!");
}

function example()
    needs alloc.heap;
{
    var text: own String = String.from("Hello");

    // Both allowed simultaneously - NO borrow checker
    modify1(mut text);
    modify2(mut text);

    // Result: "Hello world!"
    // Programmer must ensure correct ordering!
}
```

**Aliasing responsibility:**
```cantrip
// UNSAFE PATTERN - Programmer's responsibility to avoid!
function dangerous_pattern()
    needs alloc.heap;
{
    var text: own String = String.from("data");

    let ref1: mut String = mut text;
    let ref2: mut String = mut text;

    // Both ref1 and ref2 can modify - could cause bugs
    // Cantrip allows this, but programmer must be careful!
}
```

**Formal semantics:**
```
⟦mut String⟧ = { mutable reference to UTF-8 byte sequence }
Operations: Read, Write (push_str, clear, insert, remove, etc.)
Aliasing: Unlimited (programmer ensures safety)
Lifetime: Lexical scope
```

---

#### Owned Permission

**Syntax:** `own String`

**Type rule:**
```
[T-String-Own]
Γ ⊢ e : String    copyable(String) = false
─────────────────────────────────────────────
Γ ⊢ e : own String
```

**Semantics:**
- Full ownership of the string
- Can read, modify, move, destroy
- Automatically destroyed when scope ends (no memory leaks)
- Exactly one owner (enforced by move semantics)

**Operations - Full control:**
```cantrip
function example()
    needs alloc.heap;
{
    let s: own String = String.new();

    s.push_str("data");     // Can modify (owner has all rights)
    let len = s.len();      // Can read
    consume(move s);        // Can transfer ownership
    // s no longer accessible here
}
```

**Creating owned strings:**
```cantrip
function create_greeting(): own String
    needs alloc.heap;
{
    var s: own String = String.new();
    s.push_str("Hello, ");
    s.push_str("World!");
    s  // Return ownership to caller
}
```

**Automatic destruction:**
```cantrip
function auto_cleanup()
    needs alloc.heap;
{
    let temp: own String = String.from("temporary");
    temp.push_str(" data");
    // temp automatically freed here (no explicit drop needed)
}
```

**Move semantics:**
```cantrip
function consume_string(text: own String) {
    std.io.println(text);
    // text destroyed here
}

function example()
    needs alloc.heap;
{
    let message: own String = String.from("Hello");

    consume_string(move message);  // Explicit transfer
    // message.len();  // ERROR E3004: Value moved
}
```

**Formal semantics:**
```
⟦own String⟧ = { unique pointer to UTF-8 byte sequence }
Operations: Read, Write, Move, Destroy
Aliasing: Exactly one owner
Lifetime: Lexical scope (automatic destruction)
Destruction: RAII-style cleanup
```

---

#### Permission Transitions

**Subtyping hierarchy:**
```
[Sub-String-Permission]
own String <: mut String <: String
```

**Explicit transitions:**

```cantrip
let s: own String = String.from("data");

// 1. Pass as immutable reference (keep ownership)
read(s);           // s: String (reference)

// 2. Pass as mutable reference (keep ownership)
modify(mut s);     // s: mut String (mutable reference)

// 3. Transfer ownership (lose access)
consume(move s);   // s: own String (moved)
// s no longer accessible
```

**Complex example showing all transitions:**
```cantrip
function demonstrate_permissions()
    needs alloc.heap, io.write;
{
    var text: own String = String.from("initial");

    // Transition 1: own → reference (temporary, keep ownership)
    let len1 = get_length(text);
    std.io.println(format!("Length: {}", len1));

    // Transition 2: own → mut reference (temporary, keep ownership)
    append_data(mut text);

    // Transition 3: own → reference again
    let len2 = get_length(text);

    // Transition 4: own → own (permanent move)
    send_to_logger(move text);

    // text no longer accessible
}
```

---

#### Safety Guarantees

**What Cantrip PREVENTS:**

1. **Use-after-free** (regions enforce lifetime):
```cantrip
function safe_example(): String {
    region temp {
        let s = String.new_in<temp>();
        s.push_str("data");
        // return s;  // ERROR E8301: Cannot escape region
    }
}
```

2. **Double-free** (one owner only):
```cantrip
function safe_example()
    needs alloc.heap;
{
    let s = String.from("data");
    consume(move s);
    // consume(move s);  // ERROR E3004: Value already moved
}
```

3. **Memory leaks** (automatic destruction):
```cantrip
function safe_example()
    needs alloc.heap;
{
    let s = String.from("data");
    // No need for explicit free - automatic cleanup
}  // s destroyed here, memory reclaimed
```

**What Cantrip does NOT prevent (programmer's responsibility):**

1. **Aliasing bugs** (multiple `mut` refs allowed):
```cantrip
// ALLOWED but potentially dangerous
var text = String.from("data");
modify1(mut text);
modify2(mut text);  // Could conflict with modify1
```

2. **Data races** (no automatic synchronization):
```cantrip
// Programmer must use proper synchronization for threads
thread1.spawn(|| modify(mut shared_string));
thread2.spawn(|| modify(mut shared_string));  // Race condition!
```

3. **Modification during iteration** (no iterator invalidation protection):
```cantrip
var text = String.from("data");
for c in text.chars() {
    text.push_str("x");  // ALLOWED but dangerous!
}
```

**Error codes:**
- `E3002` — Attempted modification of immutable reference
- `E3004` — Use of moved value
- `E8301` — Region-allocated value cannot escape scope

**Design philosophy:**
> Cantrip provides memory safety guarantees (no use-after-free, no leaks) without borrow checker complexity. Aliasing bugs are programmer's responsibility, trading strictness for simplicity.

---

### 5.6.7 Region Integration

**Definition 5.6.7.1 (Region-Allocated String):** A `String` allocated within a memory region for fast bulk deallocation. Region-allocated strings cannot escape their region scope.

**Philosophy:** Regions control WHEN memory is freed (lexical scope), not WHO can access it. Perfect for temporary string processing.

---

#### Region Allocation Syntax

**Basic syntax:**
```cantrip
region name {
    let s: own String = String.new_in<name>();
    // s allocated in region 'name'
}  // s freed here (O(1) bulk deallocation)
```

**Type rule:**
```
[T-String-Region]
Γ ⊢ region r active
─────────────────────────────────────
Γ ⊢ String.new_in<r>() : own String
    needs alloc.region
```

**Complete example:**
```cantrip
function parse_file(path: String): Result<Data, Error>
    needs alloc.region, io.read;
{
    region temp {
        // Allocate parsing buffer in region
        let contents: own String = String.new_in<temp>();

        // Read file into region-allocated buffer
        std.fs.read_to_string(path, mut contents)?;

        // Parse (may use more temp strings)
        let parsed = parse_json(contents)?;

        // Must return heap-allocated data
        Ok(parsed.to_heap())
    }  // All temp strings freed in O(1)
}
```

---

#### String Region Methods

**Standard allocation methods:**

| Method | Signature | Allocation |
|--------|-----------|------------|
| `String.new()` | `() -> own String` | Heap |
| `String.new_in<'r>()` | `() -> own String` | Region `'r` |
| `String.from(s)` | `(str) -> own String` | Heap |
| `String.from_in<'r>(s)` | `(str) -> own String` | Region `'r` |
| `String.with_capacity(n)` | `(usize) -> own String` | Heap |
| `String.with_capacity_in<'r>(n)` | `(usize) -> own String` | Region `'r` |

**Example usage:**
```cantrip
region temp {
    // Empty string in region
    let s1 = String.new_in<temp>();

    // From literal
    let s2 = String.from_in<temp>("hello");

    // With capacity
    let s3 = String.with_capacity_in<temp>(1024);
}
```

---

#### Lifetime Enforcement

**Cannot escape region:**
```cantrip
function bad_example(): own String
    needs alloc.region;
{
    region temp {
        let s = String.new_in<temp>();
        s.push_str("data");
        s  // ERROR E8301: Cannot return region-allocated value
    }
}
```

**Compiler error:**
```
Error E8301: Value allocated in region 'temp' cannot escape region scope
  --> example.ct:5:9
   |
3  |     region temp {
   |     ----------- region 'temp' declared here
4  |         let s = String.new_in<temp>();
   |                 --------------------- allocated in region 'temp'
5  |         s  // ERROR
   |         ^ cannot return region-allocated value
   |
   = note: Region-allocated values are freed when region ends
   = help: Convert to heap: `s.to_heap()` or `s.clone()`
```

**Correct pattern - convert to heap:**
```cantrip
function safe_example(): own String
    needs alloc.region, alloc.heap;
{
    region temp {
        var s = String.new_in<temp>();
        s.push_str("data");
        s.to_heap()  // OK: Deep copy to heap
    }
}
```

**Type rule for escape prevention:**
```
[Region-Escape]
Γ ⊢ region r { e }    e : T    alloc(e, r)    e escapes r
──────────────────────────────────────────────────────────
ERROR E8301: Cannot return region-allocated value
```

---

#### Performance Characteristics

**Allocation comparison:**

| Operation | Heap | Region | Speedup |
|-----------|------|--------|---------|
| Allocation | 50-100 cycles | 3-5 cycles | 10-20× |
| Deallocation | O(n) frees | O(1) bulk | 100-1000× |
| Cache locality | Scattered | Contiguous | 2-4× |

**Region allocation algorithm:**
```
1. Check: current_page has space?
2. Yes: bump pointer (3-5 cycles)
3. No: allocate new page, update list
4. Return: pointer to allocation
5. Region end: free entire page list (O(1))
```

**Memory layout:**
```
Region: temp
├── Page 1 (4 KB)    [String1][String2][String3]▒▒▒▒
├── Page 2 (8 KB)    [String4][String5]▒▒▒▒▒▒▒▒▒▒▒▒▒
└── Page 3 (16 KB)   [String6]▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒
                      ↑
                      allocation_ptr
```

---

#### Region vs Heap Decision

**Use heap allocation (`String.new()`)** when:
- String needs to outlive current function
- String lifetime is unpredictable
- Building long-lived data structures
- String needs to be stored in persistent collections

**Use region allocation (`String.new_in<r>()`)** when:
- String only needed temporarily (within function/phase)
- Batch processing many short-lived strings
- Performance-critical code with many allocations
- Parsing, lexing, or compilation phases

**Decision flowchart:**
```
Does string need to escape function?
├─ Yes → Use heap allocation
└─ No → Is this performance-critical?
    ├─ Yes → Use region allocation
    └─ No → Either (prefer heap for simplicity)
```

---

#### Nested Regions

**Regions support nesting (LIFO order):**
```cantrip
function nested_example()
    needs alloc.region;
{
    region outer {
        let s1 = String.from_in<outer>("outer");

        region inner {
            let s2 = String.from_in<inner>("inner");

            // Can access outer from inner
            s1.push_str(" modified");

            // Cannot return s2 (would escape inner)
        }  // s2 freed here

        // s1 still valid
        s1.push_str(" again");
    }  // s1 freed here
}
```

**Region hierarchy rules:**
1. Inner regions deallocate before outer (LIFO)
2. Inner regions CAN access outer region values
3. Outer regions CANNOT access inner region values
4. Cannot return inner-region values to outer scopes

**Formal semantics:**
```
[Region-LIFO]
r₂ nested in r₁
──────────────────────────────────────
dealloc(r₂) <ʜᴀᴘᴘᴇɴs-ʙᴇғᴏʀᴇ> dealloc(r₁)

[Region-Nesting]
Γ, r₁ : Region ⊢ region r₂ { e₂ }    parent(r₂) = r₁
──────────────────────────────────────────────────────
Γ ⊢ e₂ can reference values in r₁
```

---

#### Common Patterns

**Pattern 1: Parsing with regions**
```cantrip
function tokenize(source: String): Vec<Token>
    needs alloc.heap, alloc.region;
{
    var tokens = Vec.new();  // Heap (return value)

    region temp {
        for line in source.lines() {
            // Temporary string for processing
            var trimmed = String.new_in<temp>();
            trimmed.push_str(line.trim());

            let token = parse_token(trimmed);
            tokens.push(token);  // token copied to heap

            // trimmed stays in region (will be freed)
        }
    }  // All temp strings freed (O(1))

    tokens  // Return heap-allocated result
}
```

**Pattern 2: Request processing**
```cantrip
function handle_request(request: Request): Response
    needs alloc.region, alloc.heap;
{
    region request_scope {
        // Parse request (many temp strings)
        var body = String.new_in<request_scope>();
        request.read_body(mut body)?;

        var headers = parse_headers_in<request_scope>(request);
        let data = extract_data(body, headers)?;

        // Return heap-allocated response
        Response.from_data(data)
    }  // All request temps freed instantly
}
```

**Pattern 3: Batch string processing**
```cantrip
function process_batch(items: Vec<String>): Vec<Result>
    needs alloc.region, alloc.heap;
{
    var results = Vec.new();

    region batch {
        for item in items {
            // Temporary processing strings
            var normalized = String.new_in<batch>();
            normalized.push_str(item.to_lowercase());

            var cleaned = String.new_in<batch>();
            cleaned.push_str(normalized.trim());

            let result = process(cleaned);
            results.push(result);  // result copied to heap

            // normalized, cleaned stay in region
        }
    }  // O(1) bulk free - perfect for batch jobs

    results
}
```

**Pattern 4: Compilation phases**
```cantrip
function compile(source: String): Program
    needs alloc.region, alloc.heap;
{
    region parsing {
        var tokens = lex_into_strings<parsing>(source);

        region analysis {
            let ast = parse_in<analysis>(tokens);
            let optimized = optimize(ast);

            codegen(optimized)  // Return heap-allocated Program
        }  // Analysis temps freed
    }  // Parsing temps freed
}
```

---

#### Converting Between Heap and Region

**Methods:**

```cantrip
impl String {
    // Clone from heap to region
    function clone_in<'r>(self: String): own String
        needs alloc.region;

    // Convert region → heap (deep copy)
    function to_heap(self: own String): own String
        needs alloc.heap;
}
```

**Example:**
```cantrip
function example()
    needs alloc.heap, alloc.region;
{
    let heap_str = String.from("data");

    region temp {
        // Clone heap string into region
        let region_str = heap_str.clone_in<temp>();

        // Process in region
        region_str.push_str(" processed");

        // Convert back to heap if needed
        let result = region_str.to_heap();

        // result escapes, region_str freed
    }
}
```

---

#### Permissions Still Apply

**Regions control lifetime, not access:**
```cantrip
function example()
    needs alloc.region;
{
    region temp {
        var text = String.new_in<temp>();

        // Permissions work as normal:
        read(text);          // Immutable reference
        modify(mut text);    // Mutable reference
        read(text);          // Can reference again

        // Multiple mut refs still allowed
        modify1(mut text);
        modify2(mut text);
    }  // text freed (region determines WHEN)
}
```

**Key insight:**
- **Permissions** control WHO can do WHAT (read/write/move)
- **Regions** control WHEN memory is freed (scope-based)
- These are orthogonal concerns in Cantrip

---

#### Error Codes

- `E8301` — Region-allocated value cannot escape region scope
- `E8302` — Region reference lifetime violation
- `E8303` — Invalid region nesting

**Diagnostic examples:**

```cantrip
// E8301: Escape attempt
function bad() {
    region temp {
        let s = String.new_in<temp>();
        s  // ERROR E8301
    }
}

// Fix: Convert to heap
function good() {
    region temp {
        let s = String.new_in<temp>();
        s.to_heap()  // OK
    }
}
```

---

**Summary:**

Region-allocated strings provide 10-20× faster allocation and O(1) bulk deallocation, perfect for temporary string processing in parsers, compilers, request handlers, and batch processors. The lifetime enforcement prevents use-after-free while maintaining Cantrip's simple permission model.