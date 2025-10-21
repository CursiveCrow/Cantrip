# The Cantrip Standard Library Specification

**Version:** 1.0.0 (compatible with 0.7.x)
**Date:** October 21, 2025
**Status:** Normative Reference
**Target Audience:** Library implementers and application developers

---

## Preface

This document specifies the **Cantrip Standard Library**, which provides core types, collections, I/O operations, networking, and concurrency primitives for the Cantrip programming language.

The standard library is distributed with the Cantrip compiler and is available through the `std` module namespace.

### Conformance

An implementation of the Cantrip standard library conforms to this specification if and only if it provides all types, functions, and procedures specified herein with the documented signatures, effects, contracts, and complexity guarantees.

### Relationship to Language Specification

This document is a companion to the **Cantrip Language Specification v1.0.0**. For language semantics, type system, effect system, and other core language features, refer to the main language specification.

### Document Conventions

- Code blocks use the `cantrip` fence label
- Effect declarations are normative and MUST be respected
- Complexity guarantees are normative
- Implementation details marked with `// Implementation` are informative

---

## Table of Contents

### Core Library
- [The Cantrip Standard Library Specification](#the-cantrip-standard-library-specification)
  - [Preface](#preface)
    - [Conformance](#conformance)
    - [Relationship to Language Specification](#relationship-to-language-specification)
    - [Document Conventions](#document-conventions)
  - [Table of Contents](#table-of-contents)
    - [Core Library](#core-library)
    - [Appendices](#appendices)
- [Part I: Core Library](#part-i-core-library)
  - [1. Core Types and Operations](#1-core-types-and-operations)
    - [1.1 Core Type System](#11-core-type-system)
    - [1.2 Option Type](#12-option-type)
    - [1.3 Result Type](#13-result-type)
    - [1.4 String Type](#14-string-type)
  - [2. Collections](#2-collections)
    - [2.1 Vec](#21-vec)
    - [2.2 HashMap\<K, V\>](#22-hashmapk-v)
    - [2.3 HashSet](#23-hashset)
  - [3. I/O and File System](#3-io-and-file-system)
    - [3.1 File I/O](#31-file-io)
    - [3.2 Standard Streams](#32-standard-streams)
    - [3.3 File System Operations](#33-file-system-operations)
  - [4. Networking](#4-networking)
    - [4.1 HTTP Client](#41-http-client)
    - [4.2 TCP Sockets](#42-tcp-sockets)
  - [5. Concurrency Primitives](#5-concurrency-primitives)
    - [5.1 Mutex](#51-mutex)
    - [5.2 Channels](#52-channels)
    - [5.3 Atomic Types](#53-atomic-types)

### Appendices
- [Appendix A: Module Index](#appendix-a-module-index)
- [Appendix B: Canonical API Names](#appendix-b-canonical-api-names)
- [Appendix C: Effect Families](#appendix-c-effect-families)
- [Appendix D: Standard Effect Definitions](#appendix-d-standard-effect-definitions)

---

# Part I: Core Library

## 1. Core Types and Operations

### 1.1 Core Type System

**Standard library core types (`std`):**

```cantrip
// Fundamental types
std.Option<T>           // Optional values
std.Result<T, E>        // Error handling
std.String              // UTF-8 string
std.Vec<T>              // Dynamic array
std.Box<T>              // Heap-allocated value
```

### 1.2 Option Type

**Definition 1.1 (Option):**

```cantrip
// Module: std
public enum Option<T> {
    Some(T),
    None,
}

impl<T> Option<T> {
    public function is_some(self: Option<T>): bool {
    match self {
                Option.Some(_) -> true,
                Option.None -> false,
            }
    }

    public function is_none(self: Option<T>): bool {
        match self {
            Option.Some(_) -> false,
            Option.None -> true,
        }
    }

    public function unwrap(self: Option<T>): T
    needs panic;
{
    match self {
                Option.Some(value) -> value,
                Option.None -> panic("unwrap on None"),
            }
    }

    public function unwrap_or(self: Option<T>, default: T): T {
        match self {
            Option.Some(value) -> value,
            Option.None -> default,
        }
    }

    public function map<U>(self: Option<T>, f: T -> U): Option<U>
        needs effects(f);
    {
        match self {
            Option.Some(value) -> Option.Some(f(value)),
            Option.None -> Option.None,
        }
    }
}
```

### 1.3 Result Type

**Definition 1.2 (Result):**

```cantrip
// Module: std
public enum Result<T, E> {
    Ok(T),
    Err(E),
}

impl<T, E> Result<T, E> {
    public function is_ok(self: Result<T, E>): bool {
    match self {
                Result.Ok(_) -> true,
                Result.Err(_) -> false,
            }
    }

    public function is_err(self: Result<T, E>): bool {
        match self {
            Result.Ok(_) -> false,
            Result.Err(_) -> true,
        }
    }

    public function unwrap(self: Result<T, E>): T
    needs panic;
{
    match self {
                Result.Ok(value) -> value,
                Result.Err(err) -> panic("unwrap on Err: {}", err),
            }
    }

    public function unwrap_or(self: Result<T, E>, default: T): T {
        match self {
            Result.Ok(value) -> value,
            Result.Err(_) -> default,
        }
    }

    public function map<U>(self: Result<T, E>, f: T -> U): Result<U, E>
        needs effects(f);
    {
        match self {
            Result.Ok(value) -> Result.Ok(f(value)),
            Result.Err(err) -> Result.Err(err),
        }
    }

    public function map_err<F>(self: Result<T, E>, f: E -> F): Result<T, F>
        needs effects(f);
    {
        match self {
            Result.Ok(value) -> Result.Ok(value),
            Result.Err(err) -> Result.Err(f(err)),
        }
    }
}
```

### 1.4 String Type

**Definition 1.3 (String):**

```cantrip
// Module: std
public record String {
    // Internal representation
    data: own Vec<u8>;

    public function new(): own String
    needs alloc.heap;
{
    own String { data: Vec.new() }
    }

    public function from(slice: str): own String
        needs alloc.heap;
        requires !slice.is_empty();
    {
        own String { data: slice.as_bytes().to_vec() }
    }

    public function length(self: String): usize {
    self.data.length()
    }

    public function is_empty(self: String): bool {
        self.data.is_empty()
    }

    public function push_str(self: mut String, s: str)
    needs alloc.heap;
{
    self.data.extend_from_slice(s.as_bytes());
    }

    public function split(self: String, delimiter: str): own Vec<str>
        needs alloc.heap;
        requires !delimiter.is_empty();
    {
        // Implementation
    }
}
```

---

## 2. Collections

### 2.1 Vec<T>

**Definition 2.1 (Vec):** Dynamic array with amortized O(1) push.

**Module:** `std.collections`

```cantrip
public record Vec<T> {
    // Internal fields
    data: *mut T;
    length: usize;
    capacity: usize;

    public function new(): own Vec<T>
        needs alloc.heap;
    {
        own Vec {
            data: null,
                length: 0,
                capacity: 0,
            }
    }

    public function new_in<'r>(): own Vec<T>
        needs alloc.region;
    {
        // Region-allocated vector
    }

    public function with_capacity(capacity: usize): own Vec<T>
        needs alloc.heap;
        requires capacity > 0;
    {
        // Pre-allocate space
    }

    public function push(self: mut Vec<T>, item: T)
    needs alloc.heap;
{
    if self.length == self.capacity {
                self.grow();
            }
            self.data[self.length] = item;
            self.length += 1;
    }

    public function pop(self: mut Vec<T>): Option<T> {
        if self.length == 0 {
            None
        } else {
            self.length -= 1;
            Some(self.data[self.length])
        }
    }

    public function length(self: Vec<T>): usize {
    self.length
    }

    public function capacity(self: Vec<T>): usize {
        self.capacity
    }

    public function is_empty(self: Vec<T>): bool {
        self.length == 0
    }

    public function get(self: Vec<T>, index: usize): Option<T> {
        if index < self.length {
            Some(self.data[index])
        } else {
            None
        }
    }

    public function contains(self: Vec<T>, item: T): bool
        where T: PartialEq;
    {
        for i in 0..self.length {
            if self.data[i] == item {
                return true;
            }
        }
        false
    }

    public function clear(self: mut Vec<T>) {
        self.length = 0;
    }
}
```

**Complexity guarantees:**
- `push`: O(1) amortized
- `pop`: O(1)
- `get`: O(1)
- `contains`: O(n)

### 2.2 HashMap<K, V>

**Definition 2.2 (HashMap):** Hash table with O(1) average access.

**Module:** `std.collections`

```cantrip
public record HashMap<K, V> where K: Hash + Eq {
    // Internal structure
    buckets: Vec<Bucket<K, V>>;
    size: usize;

    public function new(): own HashMap<K, V>
        needs alloc.heap;
    {
        own HashMap {
            buckets: Vec.with_capacity(16),
            size: 0,
        }
    }

    public function insert(self: mut HashMap<K, V>, key: K, value: V): Option<V>
        needs alloc.heap;
    {
        let hash = key.hash();
        let index = hash % self.buckets.length();

        // Check if key exists
        if let Some(old_value) = self.buckets[index].find(key) {
            self.buckets[index].replace(key, value);
            Some(old_value)
        } else {
            self.buckets[index].insert(key, value);
            self.size += 1;
            None
        }
    }

    public function get(self: HashMap<K, V>, key: K): Option<V> {
        let hash = key.hash();
        let index = hash % self.buckets.length();
        self.buckets[index].find(key)
    }

    public function remove(self: mut HashMap<K, V>, key: K): Option<V> {
        let hash = key.hash();
        let index = hash % self.buckets.length();

        if let Some(value) = self.buckets[index].remove(key) {
            self.size -= 1;
            Some(value)
        } else {
            None
        }
    }

    public function contains_key(self: HashMap<K, V>, key: K): bool {
    self.get(key).is_some()
    }

    public function length(self: HashMap<K, V>): usize {
        self.size
    }
}
```

### 2.3 HashSet<T>

**Definition 2.3 (HashSet):** Set implemented as HashMap<T, ()>.

```cantrip
public record HashSet<T> where T: Hash + Eq {
    map: HashMap<T, ()>,

    public function new(): own HashSet<T>
        needs alloc.heap;
    {
        own HashSet { map: HashMap.new() }
    }

    public function insert(self: mut HashSet<T>, value: T): bool
    needs alloc.heap;
{
    self.map.insert(value, ()).is_none()
    }

    public function contains(self: HashSet<T>, value: T): bool {
    self.map.contains_key(value)
    }

    public function remove(self: mut HashSet<T>, value: T): bool {
        self.map.remove(value).is_some()
    }
}
```

---

## 3. I/O and File System

### 3.1 File I/O

**Module:** `std.io`

```cantrip
public record File {
    handle: FileHandle;
    path: String;

    public function open(path: str): Result<own File, Error>
        needs fs.read, alloc.heap;
        requires !path.is_empty();
    {
        match FileSystem.open(path) {
            Ok(handle) -> Ok(own File {
                handle,
                path: String.from(path),
            }),
            Err(err) -> Err(err),
        }
    }

    public function create(path: str): Result<own File, Error>
        needs fs.write, alloc.heap;
        requires !path.is_empty();
    {
        match FileSystem.create(path) {
            Ok(handle) -> Ok(own File {
                handle,
                path: String.from(path),
            }),
            Err(err) -> Err(err),
        }
    }

    public function read(self: mut File, buffer: mut [u8]): Result<usize, Error>
        needs fs.read;
        requires buffer.length() > 0;
    {
        self.handle.read(buffer)
    }

    public function read_all(self: mut File): Result<Vec<u8>, Error>
        needs fs.read, alloc.heap;
    {
        let mut data = Vec.new();
        let mut buffer = [0u8; 4096];

        loop {
            match self.read(mut buffer) {
                Ok(0) -> break,
                Ok(n) -> data.extend_from_slice(buffer[0..n]),
                Err(err) -> return Err(err),
            }
        }

        Ok(data)
    }

    public function write(self: mut File, data: [u8]): Result<usize, Error>
        needs fs.write;
        requires data.length() > 0;
    {
        self.handle.write(data)
    }

    public function close(self: own File) {
        self.handle.close();
    }
}
```

### 3.2 Standard Streams

```cantrip
// Module: std.io

public function print(msg: str)
    needs io.write;
{
    stdout().write(msg.as_bytes());
}

public function println(msg: str)
    needs io.write;
{
    stdout().write(msg.as_bytes());
        stdout().write("\n".as_bytes());
}

public function eprint(msg: str)
    needs io.write;
{
    stderr().write(msg.as_bytes());
}

public function eprintln(msg: str)
    needs io.write;
{
    stderr().write(msg.as_bytes());
        stderr().write("\n".as_bytes());
}
```

### 3.3 File System Operations

**Module:** `std.fs`

```cantrip
public function read_to_string(path: str): Result<String, Error>
    needs fs.read, alloc.heap;
    requires !path.is_empty();
{
    let mut file = File.open(path)?;
    let data = file.read_all()?;
    String.from_utf8(data)
}

public function write(path: str, contents: str): Result<(), Error>
    needs fs.write, alloc.heap;
    requires !path.is_empty();
{
    let mut file = File.create(path)?;
    file.write(contents.as_bytes())?;
    Ok(())
}

public function exists(path: str): bool
    needs fs.metadata;
    requires !path.is_empty();
{
    FileSystem.exists(path)
}

public function remove(path: str): Result<(), Error>
    needs fs.delete;
    requires !path.is_empty();
{
    FileSystem.remove(path)
}
```

---

## 4. Networking

### 4.1 HTTP Client

**Module:** `std.net.http`

```cantrip
public record Client {
    timeout: Duration;

    public function new(): own Client
    needs alloc.heap;
{
    own Client {
                timeout: Duration.from_secs(30),
            }
    }

    public function get(self: Client, url: str): Result<Response, Error>
        needs net.read(outbound), alloc.heap;
        requires !url.is_empty();
    {
        let request = Request.builder()
            .procedure(Procedure.GET)
            .url(url)
            .build()?;

        self.execute(request)
    }

    public function post(self: Client, url: str, body: [u8]): Result<Response, Error>
        needs net.read(outbound), net.write, alloc.heap;
        requires !url.is_empty();
    {
        let request = Request.builder()
            .procedure(Procedure.POST)
            .url(url)
            .body(body)
            .build()?;

            self.execute(request)
    }
}

public record Response {
    status: u16;
    headers: HashMap<String, String>;
    body: Vec<u8>;

    public function status(self: Response): u16 {
    self.status
    }

    public function body(self: Response): Vec<u8> {
        self.body
    }

    public function text(self: Response): Result<String, Error>
        needs alloc.heap;
    {
        String.from_utf8(self.body)
    }
}
```

### 4.2 TCP Sockets

**Module:** `std.net.tcp`

```cantrip
public record TcpListener {
    addr: SocketAddr;

    public function bind(addr: str): Result<own TcpListener, Error>
        needs net.read(inbound), alloc.heap;
    {
        // Implementation
    }

    public function accept(self: TcpListener): Result<TcpStream, Error>
        needs net.read(inbound), alloc.heap;
    {
        // Implementation
    }
}

public record TcpStream {
    socket: Socket;

    public function connect(addr: str): Result<own TcpStream, Error>
        needs net.read(outbound), alloc.heap;
    {
        // Implementation
    }

    public function read(self: mut TcpStream, buffer: mut [u8]): Result<usize, Error>
        needs net.read(outbound);
    {
        self.socket.read(buffer)
    }

    public function write(self: mut TcpStream, data: [u8]): Result<usize, Error>
        needs net.write;
    {
        self.socket.write(data)
    }
}
```

---

## 5. Concurrency Primitives

**Note:** The `std.concurrent.scope` module provides structured concurrency helpers. See the main language specification §35.5 for semantics. Using `scope.spawn` eliminates leaked threads by construction and integrates with the effect system (`thread.spawn`, `thread.join`).

### 5.1 Mutex

**Module:** `std.sync`

```cantrip
public record Mutex<T> {
    data: T;
    lock: AtomicBool;

    public function new(value: T): own Mutex<T>
        needs alloc.heap;
    {
        own Mutex {
            data: value,
            lock: AtomicBool.new(false),
        }
    }

    public function lock(self: Mutex<T>): MutexGuard<T> {
        while !self.lock.compare_exchange(false, true) {
            // Spin
        }
        MutexGuard { mutex: self }
    }
}

public record MutexGuard<T> {
    mutex: Mutex<T>;

    // Automatic unlock on drop
}
```

### 5.2 Channels

**Module:** `std.sync.channel`

```cantrip
public function new<T>(): (Sender<T>, Receiver<T>)
    needs alloc.heap;
{
    let channel = Channel.new();
    (Sender { channel }, Receiver { channel })
}

public record Sender<T> {
    channel: Channel<T>;

    public function send(self: Sender<T>, value: T): Result<(), Error> {
        self.channel.push(value)
    }
}

public record Receiver<T> {
    channel: Channel<T>;

    public function receive(self: Receiver<T>): Result<T, Error> {
        self.channel.pop()
    }

    public function try_receive(self: Receiver<T>): Option<T> {
        self.channel.try_pop()
    }
}
```

### 5.3 Atomic Types

**Module:** `std.sync.atomic`

```cantrip
public record AtomicI32 {
    value: i32;

    public function new(initial: i32): own AtomicI32 {
    own AtomicI32 { value: initial }
    }

    public function load(self: AtomicI32): i32
    needs thread.atomic;
{
    atomic_load(&self.value)
    }

    public function store(self: mut AtomicI32, value: i32)
    needs thread.atomic;
{
    atomic_store(&self.value, value)
    }

    public function fetch_add(self: mut AtomicI32, value: i32): i32
        needs thread.atomic;
    {
        atomic_fetch_add(&self.value, value)
    }

    public function compare_exchange(
        self: mut AtomicI32,
        current: i32,
        new: i32
    ): bool
        needs thread.atomic;
    {
        atomic_compare_exchange(&self.value, current, new)
    }
}
```

---

# Part II: Appendices

## Appendix A: Module Index

### A.1 Core Modules

```
std                     # Core types (Option, Result, String)
std.collections         # Vec, HashMap, HashSet
std.io                  # Input/output
std.fs                  # File system operations
std.net                 # Networking
std.net.http            # HTTP client/server
std.net.tcp             # TCP sockets
std.sync                # Synchronization primitives
std.sync.atomic         # Atomic types
std.sync.channel        # Message passing
std.thread              # Threading
std.time                # Time and duration
std.math                # Mathematical functions
std.string              # String utilities
std.process             # Process management
std.concurrent          # Structured concurrency
std.concurrent.scope    # Scoped thread spawning
```

---

## Appendix B: Canonical API Names

**Consistency is critical for LLM code generation.** The following table specifies the ONLY correct names for common operations. Using alternative names will result in compilation errors.

| Operation | Correct Procedure | ❌ Alternatives (DO NOT USE) |
|-----------|----------------|-----------------|
| Get size | `.length()` | `.size()`, `.count()`, `.len()` |
| Add item | `.push(item)` | `.append()`, `.add()` |
| Remove last | `.pop()` | `.remove_last()` |
| Check empty | `.is_empty()` | `.empty()` |
| Check contains | `.contains(item)` | `.has()` |
| Get element | `.get(index)` | `.at()`, `[]` operator for fallible access |
| Remove element | `.remove(item)` | `.delete()`, `.erase()` |
| Clear all | `.clear()` | `.empty()`, `.reset()` |
| Get capacity | `.capacity()` | `.cap()`, `.max_size()` |
| Reserve space | `.reserve(n)` | `.allocate()`, `.ensure_capacity()` |

**Rationale:** Consistent naming prevents LLM hallucinations and reduces cognitive load. These names are enforced by the compiler.

---

## Appendix C: Effect Families

**Complete effect taxonomy for standard library operations:**

```
alloc.*
  ├─ alloc.heap       # Heap allocation (Vec, HashMap, String)
  ├─ alloc.stack      # Stack allocation (arrays, local variables)
  ├─ alloc.region     # Region allocation (Vec.new_in<'r>())
  └─ alloc.temp       # Temporary allocation (short-lived)

fs.*
  ├─ fs.read          # Read files (File.open, read_to_string)
  ├─ fs.write         # Write files (File.create, write)
  ├─ fs.delete        # Delete files (remove)
  └─ fs.metadata      # File metadata (exists, stat)

net.*
  ├─ net.read(inbound)   # Accept connections (TcpListener.accept)
  ├─ net.read(outbound)  # Connect to servers (TcpStream.connect, HTTP.get)
  └─ net.write           # Send data (TcpStream.write, HTTP.post)

time.*
  ├─ time.read        # Read current time (now, elapsed)
  └─ time.sleep       # Sleep/delay (sleep, timeout)

thread.*
  ├─ thread.spawn     # Spawn threads (thread::spawn, scope.spawn)
  ├─ thread.join      # Join threads (JoinHandle.join)
  └─ thread.atomic    # Atomic operations (AtomicI32.load, store)

io.*
  └─ io.write         # Console output (print, println, eprint)

process.*
  └─ process.spawn    # Spawn processes (Command.spawn)

ffi.*
  └─ ffi.call         # Foreign function calls (extern "C")

unsafe.*
  └─ unsafe.ptr       # Raw pointer operations (FFI, low-level)
```

---

## Appendix D: Standard Effect Definitions

**NOTE:** For comprehensive coverage of standard effect definitions, see the main language specification §21.7.

**Quick reference - Core standard effects:**

```cantrip
// From std.effects module

Pure            // No effects
AllAlloc        // All allocation types
SafeIO          // fs.read + fs.write + alloc.heap
AllIO           // All I/O operations
NetworkIO       // Network I/O with allocation
FileIO          // File I/O with allocation
ConsoleIO       // Console output only
NoAlloc         // Forbid all allocation
NoIO            // Forbid all I/O
Deterministic   // Forbid time and random
GameTick        // Temp allocation only, no I/O
RealTime        // No heap, no I/O
WebService      // Network + file I/O + heap
DatabaseOps     // File I/O + heap, no network
```

**Usage example:**

```cantrip
// Import standard effect definitions
import std.effects;

// Use predefined effect
function handle_request(req: Request): Response
    needs WebService;  // Expands to: net.* + fs.* + alloc.heap
{
    let data = fs.read_to_string("config.json")?;
    let response = http.get(req.url)?;
    Response.new(response.body)
}
```

**For detailed documentation, usage patterns, and examples:**
- See main language specification §21.7: Standard Effect Definitions
- See main language specification §21.8: Effect Composition Best Practices

---

## Appendix E: Complexity Guarantees

All complexity guarantees in this specification are **normative**. Implementations MUST meet or exceed these guarantees.

### E.1 Collections

**Vec<T>:**
- `new()`: O(1)
- `push(item)`: O(1) amortized
- `pop()`: O(1)
- `get(index)`: O(1)
- `contains(item)`: O(n)
- `clear()`: O(1)
- `length()`: O(1)

**HashMap<K, V>:**
- `new()`: O(1)
- `insert(key, value)`: O(1) average, O(n) worst case
- `get(key)`: O(1) average, O(n) worst case
- `remove(key)`: O(1) average, O(n) worst case
- `contains_key(key)`: O(1) average, O(n) worst case
- `length()`: O(1)

**HashSet<T>:**
- Same as HashMap (implemented as HashMap<T, ()>)

### E.2 I/O Operations

**File:**
- `open(path)`: O(1) system call
- `read(buffer)`: O(n) where n = buffer size
- `write(data)`: O(n) where n = data size
- `close()`: O(1) system call

**Network:**
- `TcpStream.connect(addr)`: O(1) system call + network latency
- `TcpStream.read(buffer)`: O(n) where n = buffer size
- `TcpStream.write(data)`: O(n) where n = data size

### E.3 Concurrency

**Mutex:**
- `lock()`: O(1) uncontended, O(n) contended (spin lock)
- `unlock()`: O(1)

**Channel:**
- `send(value)`: O(1) amortized
- `receive()`: O(1) amortized
- `try_receive()`: O(1)

**Atomic:**
- All atomic operations: O(1)

---

## Appendix F: Memory Allocation Patterns

### F.1 Heap Allocation

**When to use:** Dynamic data structures with flexible lifetimes.

```cantrip
function create_user_data(): own Vec<User>
    needs alloc.heap;
{
    let users = Vec.new();  // Heap allocated
    users.push(User.new("Alice"));
    users.push(User.new("Bob"));
    users  // Ownership transferred to caller
}
```

### F.2 Region Allocation

**When to use:** Temporary data with known scope, bulk deallocation.

```cantrip
function process_request(request: Request): Response
    needs alloc.region, alloc.heap;
{
    region temp {
        let parsed = parse_in<temp>(request);  // Region allocated
        let result = process(parsed);
        Response.from_data(result)  // Heap allocated
    }  // O(1) bulk free of all temp allocations
}
```

### F.3 Stack Allocation

**When to use:** Fixed-size data with function-local lifetime.

```cantrip
function calculate(x: i32, y: i32): i32 {
    let buffer = [0i32; 100];  // Stack allocated
    // Use buffer
    buffer[0] + buffer[1]
}  // Automatic cleanup
```

---

## Appendix G: Error Handling Patterns

### G.1 Result Propagation

```cantrip
function read_config(path: str): Result<Config, Error>
    needs fs.read, alloc.heap;
{
    let contents = fs.read_to_string(path)?;  // ? propagates errors
    let config = parse_config(contents)?;
    Ok(config)
}
```

### G.2 Option Handling

```cantrip
function find_user(id: i32, users: Vec<User>): Option<User> {
    for user in users {
        if user.id == id {
            return Some(user);
        }
    }
    None
}

// Usage
match find_user(42, users) {
    Some(user) -> println("Found: {}", user.name),
    None -> println("User not found"),
}
```

### G.3 Combining Result and Option

```cantrip
function get_user_email(id: i32): Result<Option<String>, Error>
    needs fs.read, alloc.heap;
{
    let users = load_users()?;  // Result
    let user = users.find(id);  // Option
    Ok(user.map(|u| u.email))   // Result<Option<String>>
}
```

---

## Appendix H: Concurrency Patterns

### H.1 Structured Concurrency

```cantrip
import std.concurrent.scope;

function parallel_process(items: Vec<Item>): Vec<Result>
    needs thread.spawn, thread.join, alloc.heap;
{
    scope.run(|scope| {
        let mut handles = Vec.new();

        for item in items {
            let handle = scope.spawn(|| process_item(item));
            handles.push(handle);
        }

        // All threads automatically joined at scope end
        handles.map(|h| h.join())
    })
}
```

### H.2 Message Passing

```cantrip
import std.sync.channel;

function producer_consumer()
    needs thread.spawn, thread.join, alloc.heap;
{
    let (sender, receiver) = channel.new<i32>();

    scope.run(|scope| {
        // Producer
        scope.spawn(move || {
            for i in 0..10 {
                sender.send(i);
            }
        });

        // Consumer
        scope.spawn(move || {
            while let Ok(value) = receiver.receive() {
                println("Received: {}", value);
            }
        });
    });
}
```

### H.3 Shared State with Mutex

```cantrip
import std.sync.Mutex;

function shared_counter()
    needs thread.spawn, thread.join, alloc.heap;
{
    let counter = Mutex.new(0);

    scope.run(|scope| {
        for _ in 0..10 {
            scope.spawn(|| {
                let guard = counter.lock();
                *guard += 1;
            });
        }
    });

    println("Final count: {}", counter.lock());
}
```

---

**END OF STANDARD LIBRARY SPECIFICATION**

© 2025 Cantrip Language Project
Licensed under MIT

