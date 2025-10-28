# Chapter 2 (Types) Cleanup Handoff

## 1. Completed Work (latest session)
- Added a tuple/tuple-struct/record terminology snapshot in §2.2.2 so the three constructs are easy to distinguish at a glance.
- Consolidated tuple-struct theorems into a concise “Proven Properties” list (§2.2.2.9) and mirrored that pattern for enums (§2.3.1.3.3), unions (§2.3.2.8), and modals (§2.3.3.5).
- Pruned repeated union guidance by keeping the `unsafe.union` requirement in one authoritative location.
- Preserved the earlier Guidance clean-up (non-normative blocks) and cross-reference fixes from prior passes.
- Added `tools/codex_review.py` so Codex MCP submissions drain streamed events and complete before the harness timeout.
- Normalized pointer usage guidance by folding §2.5.1.2 into a single non-normative contrast that points back to §§2.5.1.3–2.5.1.4.
- Re-read §§2.5.1.1–2.5.1.4 to ensure the surrounding overview, safety model, and cross-reference text stay consistent with the updated guidance.

## 2. Remaining Work / Open Issues
- **Outstanding issues:** TS-1 (tuple-struct constructors), TS-2 (mutable slice alias proofs), TS-3 (lifetime/region parameters), TS-4 (marker traits), TS-5 (example compendium), TS-6 (modal/enum algorithms), TS-7 (trait object safety), TS-8 (associated type bounds), TS-9 (closure capture rules). None addressed in this pass.
- **Numbering alignment:** Several theorems still carry legacy “Theorem 9.x” labels (e.g., in pointer/alias sections). These should be renumbered under the 2.x scheme for consistency once the surrounding sections are stabilized.
- **Examples:** Many sections still refer to ISSUE TS-5 for examples; once the compendium exists, replace the placeholders with canonical samples.
- **Codex review loop:** Direct `mcp__Codex__codex` calls still time out because they ignore streamed `codex/event` traffic. Always route submissions through `python3 tools/codex_review.py`, which drains the stream and returns Codex responses within the window.

## 3. Known Ambiguities / TODOs
- Tuple struct constructors remain partially specified; await resolution of ISSUE TS-1 before finalizing that part of §2.2.2.
- Modal reachability algorithm is summarized but not formally specified; ISSUE TS-6 should deliver the authoritative algorithm and proof.
- String section (§2.1.6) still carries extensive static semantics text; future clean-up may wish to segment it further once examples are in place.
- The spec still references future “protocols” concept in passing (traits vs protocols); once protocols are defined, update the relevant guidance.

## 4. Conversation Summary for Future Agents
- **Initial evaluation:** Identified duplicate structure and non-conforming headings in `Spec/02_Types.md`; introduced 2.0 front matter, Guidance blocks, and cross-reference normalization.
- **Codex issues:** Multiple attempts to submit cleanup plans to Codex MCP failed due to timeouts or missing conversation IDs; user explicitly authorized continuing without Codex review.
- **First cleanup pass:** Removed redundant “when to use” prose, normalized tuple struct IDs, and adjusted cross references.
- **Second cleanup pass (current):** Added explicit terminology snapshot, consolidated proven properties, and trimmed narrative duplication around unions/modals.
- **Artifacts:** Updated `AGENTS.md` with a canonical provenance guardrail and the Codex submission wrapper requirement, plus this cleanup log to hand off context.

## 5. Next Recommended Steps
1. Align Theorem/Definition labels still using legacy numbering (search `Theorem 9.`) with the Chapter 2 numbering scheme.
2. Draft canonical examples (ISSUE TS-5) for primitives, products, sums, pointers, generics, map types, aliases, and Self.
3. Audit remaining Chapter 2 guidance blocks for redundant usage lists and refactor them to match the §2.5.1.2 pattern where needed.
4. Use `python3 tools/codex_review.py` for every Codex submission so the mandated review loop succeeds without timing out.
5. Tackle the major open spec gaps (tuple-struct constructors, trait object safety, closure capture rules) under their respective TS issues.
