# Cursive Language Specification
## Part I: Introduction and Conformance
### Section 1.6: Versioning and Evolution

**File**: `01-6_Versioning-Evolution.md`
**Version**: 1.0
**Status**: Normative
**Previous**: [[REF_TBD]](01-5_Document-Conventions.md) | **Next**: [Part II](../02_Basics-and-Program-Model/02-0_Translation-Phases.md)

---

This section defines how the Cursive language evolves over time through semantic versioning.

## 1.6.1 Semantic Versioning System

### Version Number Format

Cursive uses **semantic versioning** with the format `MAJOR.MINOR.PATCH`:

- **MAJOR** version: Incompatible language changes (breaking changes)
- **MINOR** version: New features in a backward-compatible manner
- **PATCH** version: Backward-compatible bug fixes, clarifications, and errata

**Examples**:
- `1.0.0` — First stable release (this specification)
- `1.1.0` — Adds new features, remains compatible with 1.0
- `1.0.1` — Bug fixes and clarifications to 1.0
- `2.0.0` — Breaking changes from 1.x series

### This Specification Defines Cursive 1.0

This document specifies **Cursive 1.0**, the first stable version of the Cursive programming language.

**Full version identifier**: `1.0.0`
**Short version**: `1.0`

### Version Declaration in Programs

Programs **shall** declare their minimum required language version in a manifest or configuration file:

```toml
# Cursive.toml (project manifest)
[package]
name = "myproject"
version = "0.1.0"

[cursive]
version = "1.0"
```

**Meaning**: This program requires Cursive language version 1.0 or compatible (1.x series).

**Default**: If no version is declared, implementations shall assume version `1.0` for now. Future major versions may change this default.

**Effect**: The declared version determines:
- Which language features are available
- How syntax and semantics are interpreted
- Which deprecated features trigger warnings
- Compatibility requirements

### Version Compatibility

**Backward compatibility within major version**:
- Programs written for Cursive 1.0 shall remain valid in all 1.x versions
- New features in 1.1, 1.2, etc. are additive only
- No breaking changes within the 1.x series

**Forward compatibility**:
- Cursive 1.0 programs may not compile on Cursive 0.x (if any pre-releases existed)
- Cursive 1.x programs will not compile on Cursive 2.0 without migration

**Compatibility rules**:

| Program Version | Compiler Version | Compatible? |
|----------------|------------------|-------------|
| 1.0 | 1.0 | ✅ Yes |
| 1.0 | 1.1 | ✅ Yes (backward compatible) |
| 1.0 | 1.5 | ✅ Yes (backward compatible) |
| 1.0 | 2.0 | ❌ No (major version change) |
| 1.2 | 1.0 | ❌ No (uses newer features) |
| 1.2 | 1.5 | ✅ Yes |

### Version Schedule and Cadence

Cursive follows a **deliberate evolution** strategy:

| Version | Type | Cadence | Breaking Changes |
|---------|------|---------|------------------|
| **PATCH** (1.0.x) | Bug fixes, errata | As needed | No |
| **MINOR** (1.x) | New features | ~6-12 months | No |
| **MAJOR** (x.0) | Breaking changes | ~3-5 years | Yes |

**Planned major versions**:

| Version | Target Release | Status |
|---------|---------------|--------|
| 1.0.0 | 2025 Q1 | **This specification** |
| 2.0.0 | ~2028-2030 | Planned (major revision) |
| 3.0.0 | ~2031-2035 | Future |

**Note**: Exact timing depends on feature maturity and community needs.

### Implementation Requirements

**Conforming implementations**:

- **Shall** support the declared version (e.g., 1.0)
- **Should** support all versions within the same major series (1.0, 1.1, 1.2, etc.)
- **May** support experimental features for future versions (with explicit opt-in, see [REF_TBD]
- **May** support preview versions (e.g., 2.0-preview) before final release

**Version support declaration**:

Implementations shall document which versions they support:

```
Cursive Compiler v1.5.0
Language versions: 1.0, 1.1, 1.2, 1.3 (default: 1.3)
Preview: 2.0-alpha (experimental)
```

**Version selection**:

Implementations should allow users to specify the language version:

```bash
# Use specific language version
cursive build --language-version 1.0

# Use latest compatible version
cursive build  # Defaults to latest 1.x
```

## 1.6.2 Feature Stability

### Stable Features

**Stable features** are those without an `[Experimental]` or `[Preview]` annotation.

**Stability guarantee within major version**:
- **Shall not** have breaking syntax changes
- **Shall not** have breaking semantic changes
- **May** have bug fixes (correcting incorrect behavior)
- **May** have performance improvements
- **May** have new optional capabilities (non-breaking additions)

**Example**: The two-axis memory model (binding categories `let`/`var` and permissions `const`/`unique`/`shared`) is stable in version 1.0.

**Breaking changes only in major versions**: Stable features can only have breaking changes in major version increments (1.x -> 2.0).

### Experimental Features

**Experimental features** are marked with `[Experimental]` throughout this specification.

**Characteristics**:
- May change or be removed in minor or patch versions
- Require explicit opt-in (feature gate)
- Not recommended for production code
- No stability guarantees

**Opt-in mechanism**:

```cursive
// Enable experimental feature
#![feature(experimental_feature_name)]
```

Or in manifest:

```toml
[cursive]
version = "1.0"
features = ["experimental_feature_name"]
```

**Example** (hypothetical):

```cursive
// This hypothetical feature is experimental in 1.0
#![feature(async_await)]  // [Experimental]

async function fetch_data() {
    // Async syntax (experimental)
}
```

**Experimental in 1.x, stable in 2.0**: Features can be experimental in 1.x and stabilize in 2.0.

### Preview Features

**Preview features** are candidates for the next major version.

**Characteristics**:
- More stable than experimental
- Available in current version but designed for next major version
- API mostly stable, details may change
- Marked with `[Preview: v2.0]` annotation

**Example** (hypothetical):

```cursive
// Preview of Cursive 2.0 async/await
#![feature(async_await_preview)]  // [Preview: v2.0]

async function fetch_data() {
    // Will be stable in 2.0
}
```

### Stabilization Process

For an experimental feature to become stable:

1. **Community review**: Gather feedback from early adopters
2. **Formal specification**: Write normative specification text
3. **Implementation testing**: Verify feature in multiple implementations
4. **Migration assessment**: Ensure smooth transition
5. **Stability decision**: Accepted for next **minor** version (if non-breaking) or next **major** version (if breaking)

**Timeline**:
- Experimental -> Stable (non-breaking): Can stabilize in next minor version (e.g., 1.1)
- Experimental -> Stable (breaking): Must wait for next major version (e.g., 2.0)

## 1.6.3 Deprecation Policy

### Deprecation Marking

Features may be **deprecated** when they:
- Are superseded by better alternatives
- Have design flaws discovered after release
- No longer align with language philosophy

**Deprecation annotation**: `[Deprecated: v2.0]` (specifies when it will be removed)

**Example** (hypothetical):

> The `array[index]` syntax is **deprecated** as of version 1.2 [Deprecated: v2.0]. Use `array::get(index)` instead.

### Deprecation Timeline for Major Version Changes

**For breaking changes (removal in major version)**:

1. **Version 1.0**: Feature is stable
2. **Version 1.x**: Feature is **deprecated** (warning issued, still functional)
   - Marked `[Deprecated: v2.0]`
   - Alternatives documented
   - Migration guide provided
3. **Version 2.0**: Feature **removed** (breaking change)

**Minimum deprecation period**: At least one minor version before major version removal.

**Example timeline**:
- v1.0: `array[index]` syntax is stable
- v1.2: `array[index]` is deprecated (`[Deprecated: v2.0]`), warnings issued
- v1.3-1.9: `array[index]` remains deprecated
- v2.0: `array[index]` is removed (compile error)

### Deprecation Timeline for Minor Version Changes (Rare)

**For non-breaking deprecations** (discouraged but not removed):

Features can be deprecated indefinitely without removal if:
- Removal would break backward compatibility
- Replacement is available
- Usage is discouraged but harmless

**Example** (hypothetical):
```
[Deprecated: use new_feature instead]  // No removal version
```

### Migration Guidance

When a feature is deprecated, this specification or companion documents shall provide:

1. **Rationale**: Why the feature is being deprecated
2. **Alternative**: What should be used instead
3. **Migration guide**: Step-by-step instructions for updating code
4. **Automated tooling** (recommended): Tools to automatically migrate code
5. **Timeline**: When the feature will be removed (for major version deprecations)

**Example migration guide** (hypothetical):

> **Migrating from `array[index]` to `array::get(index)`** (Deprecated in 1.2, removed in 2.0)
>
> Old code (1.0-1.x):
> ```cursive
> let value = array[5];
> ```
>
> New code (1.2+):
> ```cursive
> let value = array::get(5);
> ```
>
> **Automated migration**: Run `cursive fix --version 2.0` to automatically update.

### Removal

**Removal conditions**:
- Feature has been deprecated for at least one minor version
- Only occurs at major version boundaries (1.x -> 2.0)
- Migration path is documented
- Automated migration tooling is available (recommended)

**Breaking change**: Removal is a **breaking change** and requires a major version increment.

**Implementations**: After removal in version 2.0, implementations supporting version 2.0:
- Shall reject code using the removed feature
- Should provide helpful error messages pointing to migration guide
- May provide `--allow-deprecated` flag for temporary backward compatibility (quality of implementation)

## 1.6.4 Relationship to ISO Standardization

This specification is designed to be suitable for formal standardization through ISO/IEC JTC 1/SC 22 (Programming Languages) or equivalent standards body.

### ISO Revisions Correspond to Major Versions

If Cursive undergoes ISO standardization:

- **ISO revisions** correspond to Cursive major versions
- Each ISO standard revision represents a well-tested, production-ready version
- ISO standardization provides formal recognition and stability guarantees

**Example** (hypothetical future):
- ISO/IEC 12345:2025 ≡ Cursive 1.0
- ISO/IEC 12345:2028 ≡ Cursive 2.0
- ISO/IEC 12345:2033 ≡ Cursive 3.0

**Naming convention**: ISO standards use publication year, Cursive uses semantic versioning. The mapping is documented.

### Minor Versions Between ISO Revisions

**Advantage of semantic versioning for standards**:

- Community can release minor versions between ISO standardization cycles
- Bug fixes (patch versions) can be released quickly
- ISO revisions focus on major language changes

**Example timeline**:
- 2025: Cursive 1.0 -> ISO standardization -> ISO/IEC 12345:2025
- 2026: Cursive 1.1 released (new features, backward compatible)
- 2027: Cursive 1.2 released
- 2028: Cursive 2.0 -> ISO standardization -> ISO/IEC 12345:2028

**Errata handling**: Patch versions (1.0.1, 1.0.2) can address specification errors without waiting for ISO revision.

### ISO Amendment Process

Between major revisions, ISO may publish:
- **Technical Corrigenda** (corrections) — maps to patch versions
- **Amendments** (additions) — maps to minor versions

## 1.6.5 Implementation Guidance

### Feature Gates for Experimental Features

Implementations providing experimental features should use **feature gates**:

```cursive
#![feature(feature_name)]
```

**Behavior**:
- Without the feature gate, experimental features produce a compile error
- With the feature gate, experimental features are enabled
- Warnings should indicate the feature is unstable

**Benefit**: Prevents accidental use of unstable features in production code.

**Version-based gating** (alternative):

```toml
[cursive]
version = "1.0"
# Experimental features from 1.1-preview
preview = ["async_await"]
```

### Version Migration Tooling

Implementations **should** provide automated tools to migrate code between major versions:

```bash
# Migrate from 1.x to 2.0
$ cursive fix --target-version 2.0
```

**Tool capabilities**:
- **Automated syntax updates**: Mechanically update syntax changes
- **Deprecation fixes**: Replace deprecated features with alternatives
- **Manual review flagging**: Flag areas requiring human attention
- **Idempotence**: Running the tool multiple times is safe

**Example**:

```bash
$ cursive fix --target-version 2.0
Migrating from Cursive 1.3 to 2.0...
✓ Updated 15 uses of deprecated array subscript syntax
✓ Converted 3 deprecated function signatures
⚠ Manual review required: 2 changes to grant system (see migration guide)
Migration complete. Review changes and run tests.
```

### Multi-Version Support

Implementations **may** support multiple language versions simultaneously:

**Version determination** from manifest:

```toml
[cursive]
version = "1.0"

[dependencies]
legacy_lib = { version = "1.0", cursive = "1.0" }
modern_lib = { version = "2.0", cursive = "1.5" }
```

**Benefits**:
- Gradual migration (migrate modules incrementally)
- Interoperability with dependencies using different versions

**Challenges**:
- ABI compatibility across versions (implementation responsibility)
- Potential confusion from mixing versions in one project

**Quality of implementation**: Implementations should clearly report which language version each module uses.

---

## Summary

| Concept | Definition | Stability | Breaking Changes |
|---------|-----------|-----------|------------------|
| **Major version (x.0)** | Incompatible changes | ~3-5 years | Yes (breaking) |
| **Minor version (1.x)** | New features | ~6-12 months | No (additive) |
| **Patch version (1.0.x)** | Bug fixes, errata | As needed | No |
| **Stable Feature** | Production-ready | Stable within major | Only in major++ |
| **Experimental Feature** | Early-access | Unstable | May change anytime |
| **Deprecated Feature** | Phased out | Functional but discouraged | Removed in major++ |

---

## Examples

### Example 1: Version Declaration

**Cursive.toml**:
```toml
[package]
name = "my_application"
version = "0.1.0"

[cursive]
version = "1.0"  # Requires Cursive 1.0 or compatible
```

**Effect**: This project uses Cursive 1.x syntax and semantics.

### Example 2: Using an Experimental Feature (Hypothetical)

```cursive
// Enable experimental async/await feature (hypothetical)
#![feature(async_await)]

async function fetch_data(url: string): string
    grants io::net
{
    // Experimental async syntax
    let response = await http_get(url);
    result response.body
}
```

**Warning**: Experimental features may change without notice. Not recommended for production.

### Example 3: Version Compatibility

**Cursive 1.0 code**:
```cursive
function add(x: i32, y: i32): i32 {
    result x + y
}
```

**Compatibility**:
- ✅ Compiles on Cursive 1.0
- ✅ Compiles on Cursive 1.5 (backward compatible)
- ✅ Compiles on Cursive 1.9
- ❌ May not compile on Cursive 2.0 (if syntax changed)

### Example 4: Migrating Through Versions

**Version 1.0** (original code):
```cursive
let items = [1, 2, 3];
let first = items[0];  // Stable in 1.0
```

**Version 1.2** (deprecated feature):
```cursive
let items = [1, 2, 3];
let first = items[0];  // Warning: 'array[index]' is deprecated, use 'array::get(index)'
```

**Version 2.0** (feature removed):
```cursive
let items = [1, 2, 3];
let first = items::get(0);  // Required syntax in 2.0
```

**Migration**: Run `cursive fix --target-version 2.0` before upgrading.

---

## Version 1.0 Feature Summary

The following major features are **stable** in Cursive 1.0:

- ✅ Lexical Permission System (const, unique, shared)
- ✅ Grant System (grants, grant tracking)
- ✅ Contract System (must, will, where)
- ✅ Modal Types (state machines)
- ✅ Region-based Memory Management
- ✅ Comptime Metaprogramming
- ✅ Predicates and Behavioral Contracts
- ✅ Type Inference with Holes
- ✅ FFI and C Interoperability

**Experimental** features in 1.0:
- (None in this version)

**Future directions** (potential for version 1.x or 2.0):
- Concurrency primitives (likely 2.0 - breaking change)
- Advanced effect handlers
- Refinement types
- Limited dependent types

---

**Previous**: [[REF_TBD]](01-5_Document-Conventions.md) | **Next**: [Part II](../02_Basics-and-Program-Model/02-0_Translation-Phases.md)
