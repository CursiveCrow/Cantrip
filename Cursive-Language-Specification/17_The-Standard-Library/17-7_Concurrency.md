# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-7_Concurrency.md  
**Section**: §17.7 Concurrency Support  
**Stable label**: [stdlib.concurrency]  
**Forward references**: Clause 14 [concurrency], Clause 11 §11.4 [memory.permission]

---

### §17.7 Concurrency Support [stdlib.concurrency]

#### §17.7.1 Overview

[1] The `std::concurrency` module provides the standard library implementations for concurrent programming. Core types (`Thread<T>`, `Mutex<T>`, `Channel<T>`, `RwLock<T>`) are specified in Clause 14 and implemented here.

[2] All concurrency operations follow Cursive's permission-based thread safety model (§14.1.2) without separate Send/Sync markers.

#### §17.7.2 Thread Spawning

##### §17.7.2.1 Spawn Procedure

[3] Thread creation procedure (specified in §14.1.4):

```cursive
public procedure spawn<T, ε>(action: () -> T ! ε): Thread<T>@Spawned
    [[ thread::spawn, ε |- true => true ]]
    where T: const
{
    // Platform-specific thread creation
    // Returns Thread<T>@Spawned
}
```

[4] The return type constraint `T: const` ensures thread-safe return values without requiring Send markers.

#### §17.7.3 Mutex Implementation

##### §17.7.3.1 Mutex Modal Type

[5] Mutual exclusion lock (specified in §14.4.2):

```cursive
public modal Mutex<T> {
    @Unlocked {
        data: T,
    }

    @Unlocked::lock(~!): Mutex<T>@Locked
    @Unlocked::try_lock(~!): Mutex<T>@Locked \/ LockError

    @Locked {
        data: T,
    }

    @Locked::unlock(~!): Mutex<T>@Unlocked
}

public enum LockError {
    WouldBlock,
    Poisoned,
}
```

##### §17.7.3.2 MutexGuard Helper

[6] RAII guard for automatic unlocking:

```cursive
public record MutexGuard<T> {
    mutex: unique Mutex<T>@Locked,

    procedure data(~!): unique T
    {
        result self.mutex.data
    }
}

behavior Drop for MutexGuard<T> {
    procedure drop(~!)
        [[ sync::lock |- true => true ]]
    {
        self.mutex.unlock()
    }
}
```

#### §17.7.4 Channel Implementation

##### §17.7.4.1 Channel Modal Type

[7] Message passing channel (specified in §14.4.3):

```cursive
public modal Channel<T> where T: const {
    @Open { /* internal queue */ }

    @Open::send(~%, value: T): () \/ ChannelError
    @Open::receive(~%): T \/ ChannelError
    @Open::close(~!): Channel<T>@Closed

    @Closed { }
}

public enum ChannelError {
    Disconnected,
    Full,
    Empty,
}
```

##### §17.7.4.2 Bounded and Unbounded Channels

[8] Channel constructors:

```cursive
public procedure Channel::unbounded<T>(): Channel<T>@Open
    [[ alloc::heap, sync::atomic |- true => true ]]
    where T: const

public procedure Channel::bounded<T>(capacity: usize): Channel<T>@Open
    [[ alloc::heap, sync::atomic |- capacity > 0 => true ]]
    where T: const
```

#### §17.7.5 Atomic Types

##### §17.7.5.1 Atomic Wrappers

[9] Convenient atomic type wrappers:

```cursive
public record AtomicI32 {
    value: shared i32,
}

public procedure AtomicI32::new(initial: i32): AtomicI32
{
    result AtomicI32 { value: initial }
}

public procedure AtomicI32.load(~, order: Ordering): i32
    [[ sync::atomic |- true => true ]]
{
    result atomic::load(self.value, order)
}

public procedure AtomicI32.store(~%, new_value: i32, order: Ordering): ()
    [[ sync::atomic |- true => true ]]
{
    atomic::store(self.value, new_value, order)
}

public procedure AtomicI32.fetch_add(~%, delta: i32, order: Ordering): i32
    [[ sync::atomic |- true => true ]]
{
    result atomic::fetch_add(self.value, delta, order)
}
```

#### §17.7.6 Conformance Requirements

[10] Implementations shall:

1. Provide complete Thread<T> modal type implementation
2. Implement Mutex<T>, Channel<T>, RwLock<T> per §14.4
3. Provide atomic wrapper types for common primitives
4. Use permission-based thread safety (no Send/Sync)
5. Include all required grants in signatures
6. Support all memory orderings (§14.3.2)

---

**Previous**: §17.6 Input/Output (§17.6 [stdlib.io]) | **Next**: §17.8 File System (§17.8 [stdlib.fs])
