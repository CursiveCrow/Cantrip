# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-8_File-System.md  
**Section**: §17.8 File System Operations  
**Stable label**: [stdlib.fs]  
**Forward references**: §17.2 [stdlib.core], §17.6 [stdlib.io], Clause 12 [contract]

---

### §17.8 File System Operations [stdlib.fs]

#### §17.8.1 Overview

[1] The `std::fs` module provides file system operations: reading, writing, directory management, and metadata queries. All operations require appropriate `fs::*` grants.

#### §17.8.2 Path Handling

##### §17.8.2.1 Path Type

[2] File system path representation:

```cursive
public record Path {
    path: string@Managed,

    where {
        path.len() > 0
    }
}

public procedure Path::new(s: string@View): Path
    [[ alloc::heap |- s.len() > 0 => true ]]
{
    result Path { path: s.to_managed() }
}

public procedure Path.join(~, other: string@View): Path
    [[ alloc::heap |- true => true ]]
{
    let separator = comptime {
        if target_os() == "windows" {
            result "\\"
        } else {
            result "/"
        }
    }

    result Path {
        path: string_concat(
            string_concat(self.path.view(), separator),
            other
        )
    }
}
```

#### §17.8.3 File Operations

##### §17.8.3.1 Reading Files

[3] Convenience procedures for file reading:

```cursive
public procedure read_to_string(path: string@View): string@Managed \/ IoError
    [[ fs::read, alloc::heap |- path.len() > 0 => true ]]
{
    let file = File::open(path, OpenMode::Read)
    match file {
        f: File@Open => {
            let contents = f.read_to_string()
            f.close()
            result contents
        }
        err: IoError => result err,
    }
}

public procedure read_bytes(path: string@View): List<u8> \/ IoError
    [[ fs::read, alloc::heap |- path.len() > 0 => true ]]
```

##### §17.8.3.2 Writing Files

[4] Convenience procedures for file writing:

```cursive
public procedure write_string(path: string@View, contents: string@View): () \/ IoError
    [[ fs::write, alloc::heap |- path.len() > 0 => true ]]
{
    let file = File::open(path, OpenMode::Write)
    match file {
        f: File@Open => {
            f.write_all(contents.as_bytes())
            f.close()
            result ()
        }
        err: IoError => result err,
    }
}
```

#### §17.8.4 Directory Operations

##### §17.8.4.1 Directory Listing

[5] Directory enumeration:

```cursive
public record DirEntry {
    path: Path,
    is_file: bool,
    is_directory: bool,
}

public procedure read_dir(path: string@View): List<DirEntry> \/ IoError
    [[ fs::metadata, alloc::heap |- path.len() > 0 => true ]]
{
    // Platform-specific directory reading
}

public procedure create_dir(path: string@View): () \/ IoError
    [[ fs::create |- path.len() > 0 => true ]]

public procedure remove_file(path: string@View): () \/ IoError
    [[ fs::delete |- path.len() > 0 => true ]]
```

#### §17.8.5 Metadata Operations

##### §17.8.5.1 File Metadata

[6] Querying file information:

```cursive
public record Metadata {
    size: u64,
    is_file: bool,
    is_directory: bool,
    modified_time: u64,
}

public procedure metadata(path: string@View): Metadata \/ IoError
    [[ fs::metadata |- path.len() > 0 => true ]]
{
    // Platform-specific metadata query
}

public procedure exists(path: string@View): bool
    [[ fs::metadata |- path.len() > 0 => true ]]
{
    match metadata(path) {
        _: Metadata => result true,
        _: IoError => result false,
    }
}
```

#### §17.8.6 Conformance Requirements

[7] Implementations shall:

1. Provide Path type for cross-platform path handling
2. Implement file reading/writing procedures
3. Support directory operations
4. Provide metadata queries
5. Use appropriate `fs::*` grants
6. Return union types (`T \/ IoError`) for fallible operations
7. Handle platform differences via comptime conditionals

---

**Previous**: §17.7 Concurrency Support (§17.7 [stdlib.concurrency]) | **Next**: §17.9 Networking (§17.9 [stdlib.net])
