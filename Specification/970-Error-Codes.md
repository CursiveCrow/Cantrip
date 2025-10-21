# 970 — Error Codes (Normative)

| Code   | Category | Description |
|--------|----------|-------------|
| E1010  | Syntax   | Removed legacy `requires<effects>` syntax; use `needs` |
| E1011  | Syntax   | Removed `returns<T>`; use `: T` in signature |
| E1012  | Syntax   | Removed `implements:` block marker |
| E2001  | Type     | Type mismatch |
| E3003  | Modal    | Procedure not available in current state |
| E3004  | Modal    | Use of moved value |
| E5001  | Region   | Value allocated in region escapes its lifetime |
| E8201  | Trait    | Overlapping implementations (coherence) |
| E8202  | Trait    | Orphan implementation violation |
| E9001  | Effect   | Missing effect declaration |
| E9002  | Effect   | Forbidden effect used |
| E9010  | Effect   | Redundant forbidden effect (not a wildcard/polymorphic constraint) |
| E9120  | Effect   | Trait impl exceeds trait method's effect bound |
| E9201  | Effect   | Async awaited effect not declared in enclosing function |
| E9501  | Hole     | Unfilled typed hole (static verify) |
| E9502  | Hole     | Unfilled typed hole (runtime verify; elaborated trap) |
| E9503  | Hole     | Typed hole would widen declared effects |
