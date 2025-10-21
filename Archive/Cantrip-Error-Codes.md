# The Cantrip Error Codes Reference

**Version:** 1.0.0 (compatible with 0.7.x)  
**Date:** October 21, 2025  
**Status:** Reference Guide  
**Target Audience:** All Cantrip developers

---

## Preface

This document provides a comprehensive reference of all error codes in the Cantrip compiler. Each error code includes a description, examples, and suggested fixes.

### Error Code Format

Error codes follow the format E#### where the first digit indicates the category:
- **E1xxx:** Syntax errors
- **E2xxx:** Type errors
- **E3xxx:** Modal state errors
- **E4xxx:** Contract errors
- **E5xxx:** Region/Lifetime errors
- **E9xxx:** Effect system errors

### Cross-References

- **Main Specification:** Cantrip-Language-Spec-1.0.0.md
- **Tooling Guide:** Cantrip-Tooling-Guide.md
- **Standard Library:** Cantrip-Standard-Library-Spec.md

---

## Appendix C: Error Codes

### C.1 Syntax Errors (E1xxx)

| Code | Description |
|------|-------------|
| E1001 | Procedure does not exist |
| E1006 | Invalid modal state transition syntax |
| E1007 | Missing semicolon after needs clause |
| E1008 | Missing semicolon after requires clause |
| E1009 | Missing semicolon after ensures clause |
| E1010 | Removed syntax: requires<effects> (use 'needs effects;' instead) |
| E1011 | Removed syntax: returns<Type> (use ': Type' in signature instead) |
| E1012 | Removed syntax: implements keyword (use direct body instead) |

### C.2 Type Errors (E2xxx)

| Code | Description |
|------|-------------|
| E2001 | Type mismatch |
| E2002 | Item is private |
| E2003 | Module not found |
| E2004 | Import cycle detected |
| E2005 | Ambiguous import |
| E2010 | Non-exhaustive match pattern |
| E2020 | Invalid state annotation |
| E2021 | Modal state not declared |
| E2022 | Record field not found |
| E2023 | Enum variant not found |

### C.3 Modal State Errors (E3xxx)

| Code | Description |
|------|-------------|
| E3001 | State invariant violated |
| E3002 | Invalid state transition |
| E3003 | Procedure requires different state |
| E3004 | Use of moved value |
| E3005 | Unreachable state declared |
| E3006 | Missing initial state |

### C.4 Contract Errors (E4xxx)

| Code | Description |
|------|-------------|
| E4001 | Precondition violation |
| E4002 | Postcondition violation |

### C.5 Region/Lifetime Errors (E5xxx)

| Code | Description |
|------|-------------|
| E5001 | Cannot return region data |
| E5002 | Reference escapes region scope |
| E5003 | Region outlived by reference |

### C.6 Effect System Errors (E9xxx)

| Code | Description |
|------|-------------|
| E9001 | Missing effect declaration |
| E9002 | Effect not declared |
| E9003 | Effect budget exceeded |
| E9004 | Forbidden effect used |
| E9005 | Unknown effect name in definition |
| E9006 | Circular effect definition |
| E9007 | Effect already defined |
| E9008 | Invalid effect operation |
| E9009 | Effect visibility violation |
| E9010 | Redundant forbidden effect |

**Examples:**

**E9005: Unknown effect name**
```cantrip
effect MyOps = fs.read + undefined.effect;
//                       ^^^^^^^^^^^^^^^^
// error[E9005]: Unknown effect 'undefined.effect'
```

**E9006: Circular definition**
```cantrip
effect A = B + fs.read;
effect B = A + fs.write;
// error[E9006]: Circular effect definition: A -> B -> A
```

**E9007: Already defined**
```cantrip
effect WebOps = fs.read;
effect WebOps = net.read;  // Different definition
// error[E9007]: Effect 'WebOps' already defined at line 1
```

**E9008: Invalid operation**
```cantrip
effect Invalid = fs.read * fs.write;  // Invalid operator
// error[E9008]: Invalid effect operation '*', expected '+', '-', or '!'
```

**E9009: Visibility violation**
```cantrip
// In module a.arc
effect PrivateOps = fs.read;

// In module b.arc
import a.PrivateOps;  // Error: PrivateOps is private
// error[E9009]: Effect 'PrivateOps' is private and cannot be imported
```

**E9010: Redundant forbidden effect**
```cantrip
function example()
    needs alloc.heap, !fs.*;
//                    ^^^^^^ error[E9010]: Redundant forbidden effect
//
// fs.* is already forbidden by deny-by-default semantics.
// Forbidden effects (!) are only valid in two contexts:
//   1. Wildcard restriction: needs fs.*, !fs.delete
//   2. Polymorphic constraint: needs effects(F), !time.read
```

---

