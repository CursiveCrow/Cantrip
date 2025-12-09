/*
 * Cursive Bootstrap Compiler - Unicode Utilities
 *
 * UTF-8 decoding and Unicode property checking for identifiers.
 * Per §2.1 and §2.4 of the Cursive specification.
 */

#ifndef CURSIVE_UNICODE_H
#define CURSIVE_UNICODE_H

#include "common/common.h"

/* Invalid codepoint marker */
#define UNICODE_INVALID 0xFFFD

/* Maximum codepoint */
#define UNICODE_MAX 0x10FFFF

/* Decode a UTF-8 codepoint from a byte sequence.
 * Returns the codepoint and advances *pos by the number of bytes consumed.
 * Returns UNICODE_INVALID on error. */
uint32_t utf8_decode(const char *s, size_t len, size_t *pos);

/* Get the byte length of a UTF-8 encoded codepoint starting at s[0].
 * Returns 0 for invalid leading byte. */
size_t utf8_char_len(const char *s, size_t remaining);

/* Encode a codepoint as UTF-8 into buffer (must have at least 4 bytes).
 * Returns number of bytes written. */
size_t utf8_encode(uint32_t codepoint, char *buf);

/* Check if codepoint is a valid identifier start (XID_Start).
 * Per Unicode UAX #31, this includes:
 * - Letters (Lu, Ll, Lt, Lm, Lo, Nl)
 * - Underscore (_) */
bool unicode_is_xid_start(uint32_t cp);

/* Check if codepoint is valid in identifier continuation (XID_Continue).
 * Per Unicode UAX #31, this includes:
 * - XID_Start characters
 * - Digits (Nd)
 * - Combining marks (Mn, Mc)
 * - Connector punctuation (Pc) */
bool unicode_is_xid_continue(uint32_t cp);

/* Check if codepoint is an ASCII digit 0-9 */
static inline bool is_ascii_digit(uint32_t cp) {
    return cp >= '0' && cp <= '9';
}

/* Check if codepoint is an ASCII hex digit */
static inline bool is_hex_digit(uint32_t cp) {
    return (cp >= '0' && cp <= '9') ||
           (cp >= 'a' && cp <= 'f') ||
           (cp >= 'A' && cp <= 'F');
}

/* Convert hex digit to value (0-15) */
static inline int hex_value(uint32_t cp) {
    if (cp >= '0' && cp <= '9') return (int)(cp - '0');
    if (cp >= 'a' && cp <= 'f') return (int)(cp - 'a' + 10);
    if (cp >= 'A' && cp <= 'F') return (int)(cp - 'A' + 10);
    return -1;
}

/* Check if codepoint is ASCII whitespace */
static inline bool is_ascii_whitespace(uint32_t cp) {
    return cp == ' ' || cp == '\t' || cp == '\r' || cp == '\n';
}

/* Check if codepoint is ASCII alpha */
static inline bool is_ascii_alpha(uint32_t cp) {
    return (cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z');
}

/* Check if codepoint is ASCII alphanumeric */
static inline bool is_ascii_alnum(uint32_t cp) {
    return is_ascii_alpha(cp) || is_ascii_digit(cp);
}

#endif /* CURSIVE_UNICODE_H */