/*
 * Cursive Bootstrap Compiler - Unicode Utilities Implementation
 *
 * Simplified Unicode property tables for identifier validation.
 * A production compiler would use full Unicode data tables.
 */

#include "unicode.h"

uint32_t utf8_decode(const char *s, size_t len, size_t *pos) {
    if (*pos >= len) {
        return UNICODE_INVALID;
    }

    uint8_t b0 = (uint8_t)s[*pos];

    /* ASCII fast path */
    if (b0 < 0x80) {
        (*pos)++;
        return b0;
    }

    /* Multi-byte sequences */
    uint32_t cp;
    size_t need;

    if ((b0 & 0xE0) == 0xC0) {
        /* 2-byte: 110xxxxx 10xxxxxx */
        cp = b0 & 0x1F;
        need = 2;
    } else if ((b0 & 0xF0) == 0xE0) {
        /* 3-byte: 1110xxxx 10xxxxxx 10xxxxxx */
        cp = b0 & 0x0F;
        need = 3;
    } else if ((b0 & 0xF8) == 0xF0) {
        /* 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        cp = b0 & 0x07;
        need = 4;
    } else {
        /* Invalid leading byte */
        (*pos)++;
        return UNICODE_INVALID;
    }

    if (*pos + need > len) {
        (*pos)++;
        return UNICODE_INVALID;
    }

    for (size_t i = 1; i < need; i++) {
        uint8_t b = (uint8_t)s[*pos + i];
        if ((b & 0xC0) != 0x80) {
            /* Invalid continuation byte */
            (*pos)++;
            return UNICODE_INVALID;
        }
        cp = (cp << 6) | (b & 0x3F);
    }

    *pos += need;

    /* Check for overlong encoding and surrogates */
    if (cp > UNICODE_MAX ||
        (cp >= 0xD800 && cp <= 0xDFFF) ||
        (need == 2 && cp < 0x80) ||
        (need == 3 && cp < 0x800) ||
        (need == 4 && cp < 0x10000)) {
        return UNICODE_INVALID;
    }

    return cp;
}

size_t utf8_char_len(const char *s, size_t remaining) {
    if (remaining == 0) return 0;

    uint8_t b = (uint8_t)s[0];
    if (b < 0x80) return 1;
    if ((b & 0xE0) == 0xC0) return (remaining >= 2) ? 2 : 0;
    if ((b & 0xF0) == 0xE0) return (remaining >= 3) ? 3 : 0;
    if ((b & 0xF8) == 0xF0) return (remaining >= 4) ? 4 : 0;
    return 0; /* Invalid */
}

size_t utf8_encode(uint32_t cp, char *buf) {
    if (cp < 0x80) {
        buf[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        buf[0] = (char)(0xC0 | (cp >> 6));
        buf[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        buf[0] = (char)(0xE0 | (cp >> 12));
        buf[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        buf[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    if (cp <= UNICODE_MAX) {
        buf[0] = (char)(0xF0 | (cp >> 18));
        buf[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
        buf[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
        buf[3] = (char)(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0;
}

/* Simplified XID_Start check.
 * Full Unicode support would use derived properties from UCD.
 * This handles ASCII + Latin-1 Supplement + common scripts. */
bool unicode_is_xid_start(uint32_t cp) {
    /* ASCII letters */
    if ((cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z')) {
        return true;
    }

    /* Underscore */
    if (cp == '_') {
        return true;
    }

    /* Latin-1 Supplement letters (simplified) */
    if (cp >= 0x00C0 && cp <= 0x00D6) return true;  /* À-Ö */
    if (cp >= 0x00D8 && cp <= 0x00F6) return true;  /* Ø-ö */
    if (cp >= 0x00F8 && cp <= 0x00FF) return true;  /* ø-ÿ */

    /* Latin Extended-A */
    if (cp >= 0x0100 && cp <= 0x017F) return true;

    /* Latin Extended-B */
    if (cp >= 0x0180 && cp <= 0x024F) return true;

    /* Greek and Coptic */
    if (cp >= 0x0370 && cp <= 0x03FF) {
        /* Exclude non-letters */
        if (cp == 0x0374 || cp == 0x0375 || cp == 0x037E ||
            cp == 0x0384 || cp == 0x0385 || cp == 0x0387) {
            return false;
        }
        return true;
    }

    /* Cyrillic */
    if (cp >= 0x0400 && cp <= 0x04FF) return true;

    /* Hebrew */
    if (cp >= 0x05D0 && cp <= 0x05EA) return true;

    /* Arabic */
    if (cp >= 0x0621 && cp <= 0x064A) return true;

    /* Devanagari */
    if (cp >= 0x0904 && cp <= 0x0939) return true;

    /* CJK Unified Ideographs */
    if (cp >= 0x4E00 && cp <= 0x9FFF) return true;

    /* Hiragana */
    if (cp >= 0x3040 && cp <= 0x309F) return true;

    /* Katakana */
    if (cp >= 0x30A0 && cp <= 0x30FF) return true;

    /* Hangul Syllables */
    if (cp >= 0xAC00 && cp <= 0xD7AF) return true;

    /* Other Letter categories would be added for full Unicode support */

    return false;
}

/* Simplified XID_Continue check */
bool unicode_is_xid_continue(uint32_t cp) {
    /* XID_Start characters are also XID_Continue */
    if (unicode_is_xid_start(cp)) {
        return true;
    }

    /* ASCII digits */
    if (cp >= '0' && cp <= '9') {
        return true;
    }

    /* Combining Diacritical Marks */
    if (cp >= 0x0300 && cp <= 0x036F) return true;

    /* Arabic combining marks */
    if (cp >= 0x064B && cp <= 0x0652) return true;

    /* Devanagari dependent vowel signs and other marks */
    if (cp >= 0x093E && cp <= 0x094D) return true;

    /* Other Nd, Mn, Mc, Pc categories would be added for full Unicode support */

    return false;
}