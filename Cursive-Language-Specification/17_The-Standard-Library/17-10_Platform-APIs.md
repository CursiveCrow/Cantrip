# Cursive Language Specification

## Clause 17 — The Standard Library

**Part**: XVII — The Standard Library  
**File**: 17-10_Platform-APIs.md  
**Section**: §17.10 Platform-Specific APIs  
**Stable label**: [stdlib.os]  
**Forward references**: Clause 15 [interop], Clause 16 §16.4 [comptime.intrinsics]

---

### §17.10 Platform-Specific APIs [stdlib.os]

#### §17.10.1 Overview

[1] The `std::os` module hierarchy provides platform-specific functionality through conditional compilation. Code using these modules remains portable via comptime platform detection.

[2] Module organization:

- `std::os::unix` — Unix/Linux/macOS APIs
- `std::os::windows` — Windows-specific APIs
- `std::os::raw` — Low-level platform types

#### §17.10.2 Environment Variables

##### §17.10.2.1 Cross-Platform API

[3] Environment variable access:

```cursive
public procedure env_var(key: string@View): string@Managed \/ None
    [[ sys::env, alloc::heap |- key.len() > 0 => true ]]
{
    // Platform-specific environment variable lookup
}

public procedure set_env_var(key: string@View, value: string@View): () \/ IoError
    [[ sys::env |- key.len() > 0, value.len() > 0 => true ]]
```

#### §17.10.3 Process Control

##### §17.10.3.1 Exit and Arguments

[4] Process control procedures:

```cursive
public procedure exit(code: i32): !
    [[ sys::exit |- true => false ]]
{
    // Terminate process with exit code
}

public procedure args(): [string@View]
    [[ sys::env |- true => true ]]
{
    // Return command-line arguments
}
```

#### §17.10.4 Unix-Specific Module

##### §17.10.4.1 Unix File Descriptors

[5] Unix-specific I/O:

```cursive
// Module: std::os::unix
comptime {
    if target_os() == "linux" || target_os() == "macos" || target_os() == "freebsd" {
        codegen::emit("""
            public record FileDescriptor {
                fd: i32,
            }

            public procedure FileDescriptor::open_raw(path: string@View, flags: i32): FileDescriptor \/ IoError
                [[ fs::read, unsafe::ptr |- path.len() > 0 => true ]]
        """)
    }
}
```

#### §17.10.5 Windows-Specific Module

##### §17.10.5.1 Windows Handles

[6] Windows-specific I/O:

```cursive
// Module: std::os::windows
comptime {
    if target_os() == "windows" {
        codegen::emit("""
            public record Handle {
                handle: *mut (),
            }

            [[extern(stdcall)]]
            procedure CreateFileW(
                filename: *const u16,
                access: u32,
                share_mode: u32,
                security: *const (),
                creation: u32,
                flags: u32,
                template: *const ()
            ): *mut ()
                [[ ffi::call, unsafe::ptr |- filename != null => true ]]
        """)
    }
}
```

#### §17.10.6 Conformance Requirements

[7] Implementations shall:

1. Provide cross-platform APIs for common operations
2. Use comptime conditionals for platform-specific code
3. Isolate platform-specific types to appropriate modules
4. Document platform availability clearly
5. Provide fallback implementations or clear errors for unsupported platforms

---

**Previous**: §17.9 Networking (§17.9 [stdlib.net]) | **Next**: §17.11 Compile-Time Utilities (§17.11 [stdlib.comptime])
