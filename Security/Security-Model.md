# Safety & Security Model (Informative)

Cantrip prevents use‑after‑free, double‑free, and region leaks by construction.
It does not prevent aliasing races or iterator invalidation; concurrency requires
discipline (use `iso` and synchronization primitives). FFI is `unsafe` and MUST
declare `ffi.call` and `unsafe.ptr` effects.
