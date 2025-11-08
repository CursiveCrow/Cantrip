# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-5_Collections.md  
**Section**: §17.5 Collections  
**Stable label**: [stdlib.collections]  
**Forward references**: §17.4 [stdlib.behaviors], Clause 7 [type], Clause 10 [generic], Clause 11 [memory]

---

### §17.5 Collections [stdlib.collections]

#### §17.5.1 Overview

[1] The `std::collections` module provides growable data structures implemented using Cursive's memory model. All collections use explicit heap allocation with the `alloc::heap` grant.

[2] Standard collections:

- `List<T>` — Growable array
- `Map<K, V>` — Hash-based key-value store
- `Set<T>` — Hash-based unique values
- `Deque<T>` — Double-ended queue
- `LinkedList<T>` — Doubly-linked list

[3] **Design Note**: Collections use Cursive naming (`List` not `Vec`, `Map` not `HashMap`) to avoid Rust-specific terminology.

#### §17.5.2 List<T> — Growable Array

##### §17.5.2.1 Type Definition

[4] Growable array with heap allocation:

```cursive
public record List<T> {
    data: Ptr<T>@Valid,
    length: usize,
    capacity: usize,

    where {
        length <= capacity,
        capacity > 0 => data.is_valid()
    }
}
```

##### §17.5.2.2 Core Procedures

[5] Construction and basic operations:

```cursive
public procedure List::new<T>(): List<T>
    [[ alloc::heap |- true => result.length == 0 ]]
{
    result List {
        data: allocate_array(4),  // Initial capacity
        length: 0,
        capacity: 4,
    }
}

public procedure List::with_capacity<T>(cap: usize): List<T>
    [[ alloc::heap |- cap > 0 => result.capacity >= cap ]]
{
    result List {
        data: allocate_array(cap),
        length: 0,
        capacity: cap,
    }
}

public procedure List.push<T>(~!, item: T)
    [[ alloc::heap |- true => self.length == @old(self.length) + 1 ]]
{
    if self.length == self.capacity {
        self.grow()
    }
    unsafe {
        self.data.offset(self.length).write(item)
    }
    self.length += 1
}

public procedure List.pop<T>(~!): T \/ None
    [[ |- true => true ]]
{
    if self.length == 0 {
        result None { }
    }
    self.length -= 1
    let value = unsafe { self.data.offset(self.length).read() }
    result value
}

public procedure List.get<T>(~, index: usize): T \/ None
    [[ |- true => true ]]
    where T: Copy
{
    if index >= self.length {
        result None { }
    }
    let value = unsafe { self.data.offset(index).read() }
    result value
}
```

##### §17.5.2.3 Iterator Support

[6] List implements iteration:

```cursive
behavior Iterator for List<T> {
    type Item = T

    procedure next(~%): Item \/ None
    {
        // Iterator implementation
    }
}
```

#### §17.5.3 Map<K, V> — Hash Map

##### §17.5.3.1 Type Definition

[7] Hash-based key-value storage:

```cursive
public record Map<K, V> where K: Hash + Eq {
    buckets: List<List<Entry<K, V>>>,
    length: usize,

    where {
        length >= 0
    }
}

record Entry<K, V> {
    key: K,
    value: V,
    hash: u64,
}
```

##### §17.5.3.2 Core Procedures

[8] Map operations:

```cursive
public procedure Map::new<K, V>(): Map<K, V>
    [[ alloc::heap |- true => result.length == 0 ]]
    where K: Hash + Eq

public procedure Map.insert<K, V>(~!, key: K, value: V): () \/ None
    [[ alloc::heap |- true => true ]]
    where K: Hash + Eq
{
    let hash_value = key.hash()
    let bucket_index = hash_value % self.buckets.len()

    // Check for existing key
    let bucket = self.buckets.get(bucket_index)
    // Insert or update logic

    self.length += 1
    result ()
}

public procedure Map.get<K, V>(~, key: K): V \/ None
    [[ |- true => true ]]
    where K: Hash + Eq, V: Copy
{
    let hash_value = key.hash()
    let bucket_index = hash_value % self.buckets.len()

    // Lookup logic
    result found_value_or_none
}
```

#### §17.5.4 Set<T> — Hash Set

##### §17.5.4.1 Type Definition

[9] Hash-based unique value storage:

```cursive
public record Set<T> where T: Hash + Eq {
    map: Map<T, ()>,  // Implemented using Map
}

public procedure Set::new<T>(): Set<T>
    [[ alloc::heap |- true => true ]]
    where T: Hash + Eq

public procedure Set.insert<T>(~!, value: T): bool
    [[ alloc::heap |- true => true ]]
    where T: Hash + Eq
{
    let previous = self.map.insert(value, ())
    match previous {
        _: () => result true,   // Inserted
        _: None => result false,  // Already present
    }
}
```

#### §17.5.5 Deque<T> — Double-Ended Queue

##### §17.5.5.1 Type Definition

[10] Ring buffer implementation:

```cursive
public record Deque<T> {
    buffer: List<T>,
    head: usize,
    tail: usize,

    where {
        head < buffer.capacity,
        tail < buffer.capacity
    }
}

public procedure Deque.push_front<T>(~!, item: T)
    [[ alloc::heap |- true => true ]]

public procedure Deque.push_back<T>(~!, item: T)
    [[ alloc::heap |- true => true ]]

public procedure Deque.pop_front<T>(~!): T \/ None
    [[ |- true => true ]]

public procedure Deque.pop_back<T>(~!): T \/ None
    [[ |- true => true ]]
```

#### §17.5.6 Conformance Requirements

[11] Implementations shall:

1. Provide all collection types specified
2. Use `alloc::heap` grant for all allocations
3. Implement proper cleanup (Drop behavior)
4. Provide efficient implementations (amortized O(1) for List.push)
5. Include complete contractual sequents
6. Support iteration via Iterator behavior

---

**Previous**: §17.4 Standard Behaviors (§17.4 [stdlib.behaviors]) | **Next**: §17.6 Input/Output (§17.6 [stdlib.io])
