# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-6_Input-Output.md  
**Section**: §17.6 Input and Output  
**Stable label**: [stdlib.io]  
**Forward references**: §17.2 [stdlib.core], Clause 7 §7.6 [type.modal], Clause 12 [contract]

---

### §17.6 Input and Output [stdlib.io]

#### §17.6.1 Overview

[1] The `std::io` module provides abstractions for reading and writing data to various sources: standard streams, files, network sockets, and buffers.

[2] All I/O operations require appropriate grants (`io::read`, `io::write`, `fs::read`, `fs::write`) and use union types for error handling.

#### §17.6.2 Standard Streams

##### §17.6.2.1 Console Output

[3] Standard output procedures:

```cursive
public procedure print(text: string@View): ()
    [[ io::write |- true => true ]]
{
    write_to_stdout(text)
}

public procedure println(text: string@View): ()
    [[ io::write |- true => true ]]
{
    write_to_stdout(text)
    write_to_stdout("\n")
}

public procedure eprint(text: string@View): ()
    [[ io::write |- true => true ]]
{
    write_to_stderr(text)
}

public procedure eprintln(text: string@View): ()
    [[ io::write |- true => true ]]
{
    write_to_stderr(text)
    write_to_stderr("\n")
}
```

##### §17.6.2.2 Console Input

[4] Standard input procedures:

```cursive
public procedure read_line(): string@Managed \/ IoError
    [[ io::read, alloc::heap |- true => true ]]
{
    // Read from stdin until newline
}
```

#### §17.6.3 File Modal Type

##### §17.6.3.1 File States

[5] File lifecycle as modal type:

```cursive
public modal File {
    @Closed {
        path: string@Managed,
    }

    @Closed::open(path: string@View, mode: OpenMode): File@Open \/ IoError

    @Open {
        path: string@Managed,
        handle: FileHandle,
    }

    @Open::read(~%, buffer: [u8]): usize \/ IoError
    @Open::write(~!, data: [u8]): usize \/ IoError
    @Open::seek(~!, position: u64): () \/ IoError
    @Open::close(~!): File@Closed
}

public enum OpenMode {
    Read,
    Write,
    Append,
    ReadWrite,
}
```

##### §17.6.3.2 File Procedures

[6] File operations with proper grants:

```cursive
public procedure File::open(path: string@View, mode: OpenMode): File@Open \/ IoError
    [[ fs::read, alloc::heap |- path.len() > 0 => true ]]
{
    // Platform-specific file opening
}

public procedure File.read_to_string<T>(~%): string@Managed \/ IoError
    [[ fs::read, alloc::heap |- true => true ]]
{
    // Read entire file to string
}

public procedure File.write_all(~!, data: [u8]): () \/ IoError
    [[ fs::write |- data.len() > 0 => true ]]
{
    // Write all bytes
}
```

#### §17.6.4 Reader and Writer Behaviors

##### §17.6.4.1 Definitions

[7] Abstract I/O behaviors:

```cursive
public behavior Reader {
    procedure read(~%, buffer: [u8]): usize \/ IoError
        [[ io::read |- buffer.len() > 0 => true ]]
    {
        panic("Reader not implemented")
    }
}

public behavior Writer {
    procedure write(~!, data: [u8]): usize \/ IoError
        [[ io::write |- data.len() > 0 => true ]]
    {
        panic("Writer not implemented")
    }

    procedure flush(~!): () \/ IoError
        [[ io::write |- true => true ]]
    {
        result ()  // Default: no-op
    }
}
```

#### §17.6.5 Buffered I/O

##### §17.6.5.1 BufferedReader

[8] Buffered reading for performance:

```cursive
public record BufferedReader<R: Reader> {
    inner: R,
    buffer: List<u8>,
    position: usize,
}

behavior Reader for BufferedReader<R> {
    procedure read(~%, buffer: [u8]): usize \/ IoError
        [[ io::read, alloc::heap |- buffer.len() > 0 => true ]]
    {
        // Buffered read implementation
    }
}
```

#### §17.6.6 Conformance Requirements

[9] Implementations shall:

1. Provide standard stream procedures (print, println, read_line)
2. Implement File modal type with all states
3. Provide Reader and Writer behaviors
4. Include appropriate grants in all signatures
5. Use union types for error returns (not wrapper enums)
6. Support buffered I/O for performance

---

**Previous**: §17.5 Collections (§17.5 [stdlib.collections]) | **Next**: §17.7 Concurrency Support (§17.7 [stdlib.concurrency])
