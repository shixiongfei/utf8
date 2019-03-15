/*
 *  utf8.c
 *
 *  copyright (c) 2019 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 *
 *  https://github.com/shixiongfei/utf8
 */

#include "utf8.h"
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#include <malloc.h>
#define dynarray(type, name, size)                                             \
  type *name = (type *)_alloca((size) * sizeof(type))
#else
#define dynarray(type, name, size) type name[size]
#endif

#define arraysize(a) ((int)(sizeof(a) / sizeof((a)[0])))

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

int utf8_isutf8(const char *bytes, int bytelen) {
  const char *eof;
  int n;

  if (bytelen < 0)
    bytelen = (int)strlen(bytes);

  eof = bytes + bytelen;

  while (bytes < eof) {
    n = utf8_charlen(*bytes);

    if (n <= 0)
      return 0;

    bytes += n;
  }

  return 1;
}

int utf8_fromunicode(char *utf8, int unicode) {
  if (unicode <= 0x7f) {
    *utf8 = (char)unicode;
    return 1;
  } else if (unicode <= 0x7ff) {
    *utf8++ = 0xc0 | ((unicode & 0x7c0) >> 6);
    *utf8 = 0x80 | (unicode & 0x3f);
    return 2;
  } else if (unicode <= 0xffff) {
    *utf8++ = 0xe0 | ((unicode & 0xf000) >> 12);
    *utf8++ = 0x80 | ((unicode & 0xfc0) >> 6);
    *utf8 = 0x80 | (unicode & 0x3f);
    return 3;
  } else { /* Note: We silently truncate to 21 bits here: 0x1fffff */
    *utf8++ = 0xf0 | ((unicode & 0x1c0000) >> 18);
    *utf8++ = 0x80 | ((unicode & 0x3f000) >> 12);
    *utf8++ = 0x80 | ((unicode & 0xfc0) >> 6);
    *utf8 = 0x80 | (unicode & 0x3f);
    return 4;
  }
  return 0;
}

int utf8_tounicode(const char *utf8, int *unicode) {
  const unsigned char *s = (const unsigned char *)utf8;

  if (s[0] < 0xc0) {
    *unicode = s[0];
    return 1;
  }

  if (s[0] < 0xe0) {
    if (0x80 == (s[1] & 0xc0)) {
      *unicode = ((s[0] & ~0xc0) << 6) | (s[1] & ~0x80);
      return 2;
    }
  } else if (s[0] < 0xf0) {
    if ((0x80 == (utf8[1] & 0xc0)) && (0x80 == (utf8[2] & 0xc0))) {
      *unicode =
          ((s[0] & ~0xe0) << 12) | ((s[1] & ~0x80) << 6) | (s[2] & ~0x80);
      return 3;
    }
  } else if (s[0] < 0xf8) {
    if ((0x80 == (utf8[1] & 0xc0)) && (0x80 == (utf8[2] & 0xc0)) &&
        (0x80 == (utf8[3] & 0xc0))) {
      *unicode = ((s[0] & ~0xf0) << 18) | ((s[1] & ~0x80) << 12) |
                 ((s[2] & ~0x80) << 6) | (s[3] & ~0x80);
      return 4;
    }
  }

  /* Invalid sequence, so just return the byte */
  *unicode = *s;
  return 1;
}

int utf8_charlen(int ch) {
  /* 0xxxxxxx */
  if (0 == (ch & 0x80))
    return 1;

  /* 110xxxxx 10xxxxxx */
  if (0xc0 == (ch & 0xe0))
    return 2;

  /* 1110xxxx 10xxxxxx 10xxxxxx */
  if (0xe0 == (ch & 0xf0))
    return 3;

  /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
  if (0xf0 == (ch & 0xf8))
    return 4;

  return 0;
}

int utf8_strlen(const char *utf8, int bytelen) {
  int charlen = 0;
  int ch, n;

  if (bytelen < 0)
    bytelen = (int)strlen(utf8);

  while (bytelen > 0) {
    n = utf8_tounicode(utf8, &ch);

    charlen += 1;
    utf8 += n;
    bytelen -= n;
  }

  return charlen;
}

int utf8_index(const char *utf8, int index) {
  const char *s = utf8;
  int ch;

  if (index < 0)
    index += utf8_strlen(utf8, -1);

  while ((index--) > 0)
    s += utf8_tounicode(s, &ch);

  return (int)(s - utf8);
}

typedef struct utf8_range_s {
  int lower; /* lower inclusive */
  int upper; /* upper exclusive */
} utf8_range_t;

static const utf8_range_t unicode_range_combining[] = {
    {0x0300, 0x0370}, {0x0483, 0x048a}, {0x0591, 0x05d0}, {0x0610, 0x061b},
    {0x064b, 0x0660}, {0x0670, 0x0671}, {0x06d6, 0x06dd}, {0x06df, 0x06e5},
    {0x06e7, 0x06ee}, {0x0711, 0x0712}, {0x0730, 0x074d}, {0x07a6, 0x07b1},
    {0x07eb, 0x07f4}, {0x0816, 0x0830}, {0x0859, 0x085e}, {0x08d4, 0x0904},
    {0x093a, 0x0958}, {0x0962, 0x0964}, {0x0981, 0x0985}, {0x09bc, 0x09ce},
    {0x09d7, 0x09dc}, {0x09e2, 0x09e6}, {0x0a01, 0x0a05}, {0x0a3c, 0x0a59},
    {0x0a70, 0x0a72}, {0x0a75, 0x0a85}, {0x0abc, 0x0ad0}, {0x0ae2, 0x0ae6},
    {0x0afa, 0x0b05}, {0x0b3c, 0x0b5c}, {0x0b62, 0x0b66}, {0x0b82, 0x0b83},
    {0x0bbe, 0x0bd0}, {0x0bd7, 0x0be6}, {0x0c00, 0x0c05}, {0x0c3e, 0x0c58},
    {0x0c62, 0x0c66}, {0x0c81, 0x0c85}, {0x0cbc, 0x0cde}, {0x0ce2, 0x0ce6},
    {0x0d00, 0x0d05}, {0x0d3b, 0x0d4e}, {0x0d57, 0x0d58}, {0x0d62, 0x0d66},
    {0x0d82, 0x0d85}, {0x0dca, 0x0de6}, {0x0df2, 0x0df4}, {0x0e31, 0x0e32},
    {0x0e34, 0x0e3f}, {0x0e47, 0x0e4f}, {0x0eb1, 0x0eb2}, {0x0eb4, 0x0ebd},
    {0x0ec8, 0x0ed0}, {0x0f18, 0x0f1a}, {0x0f35, 0x0f3a}, {0x0f3e, 0x0f40},
    {0x0f71, 0x0f88}, {0x0f8d, 0x0fbe}, {0x0fc6, 0x0fc7}, {0x102b, 0x103f},
    {0x1056, 0x105a}, {0x105e, 0x1065}, {0x1067, 0x106e}, {0x1071, 0x1075},
    {0x1082, 0x1090}, {0x109a, 0x109e}, {0x135d, 0x1360}, {0x1712, 0x1720},
    {0x1732, 0x1735}, {0x1752, 0x1760}, {0x1772, 0x1780}, {0x17b4, 0x17d4},
    {0x17dd, 0x17e0}, {0x180b, 0x180e}, {0x1885, 0x1887}, {0x18a9, 0x18aa},
    {0x1920, 0x1940}, {0x1a17, 0x1a1e}, {0x1a55, 0x1a80}, {0x1ab0, 0x1b05},
    {0x1b34, 0x1b45}, {0x1b6b, 0x1b74}, {0x1b80, 0x1b83}, {0x1ba1, 0x1bae},
    {0x1be6, 0x1bfc}, {0x1c24, 0x1c3b}, {0x1cd0, 0x1ce9}, {0x1ced, 0x1cee},
    {0x1cf2, 0x1cf5}, {0x1cf7, 0x1d00}, {0x1dc0, 0x1e00}, {0x20d0, 0x2100},
    {0x2cef, 0x2cf2}, {0x2d7f, 0x2d80}, {0x2de0, 0x2e00}, {0x302a, 0x3030},
    {0x3099, 0x309b}, {0xa66f, 0xa67e}, {0xa69e, 0xa6a0}, {0xa6f0, 0xa6f2},
    {0xa802, 0xa803}, {0xa806, 0xa807}, {0xa80b, 0xa80c}, {0xa823, 0xa828},
    {0xa880, 0xa882}, {0xa8b4, 0xa8ce}, {0xa8e0, 0xa8f2}, {0xa926, 0xa92e},
    {0xa947, 0xa95f}, {0xa980, 0xa984}, {0xa9b3, 0xa9c1}, {0xa9e5, 0xa9e6},
    {0xaa29, 0xaa40}, {0xaa43, 0xaa44}, {0xaa4c, 0xaa50}, {0xaa7b, 0xaa7e},
    {0xaab0, 0xaab5}, {0xaab7, 0xaab9}, {0xaabe, 0xaac2}, {0xaaeb, 0xaaf0},
    {0xaaf5, 0xab01}, {0xabe3, 0xabf0}, {0xfb1e, 0xfb1f}, {0xfe00, 0xfe10},
    {0xfe20, 0xfe30},
};

static const utf8_range_t unicode_range_wide[] = {
    {0x1100, 0x115f},   {0x231a, 0x231b},   {0x2329, 0x232a},
    {0x23e9, 0x23ec},   {0x23f0, 0x23f0},   {0x23f3, 0x23f3},
    {0x25fd, 0x25fe},   {0x2614, 0x2615},   {0x2648, 0x2653},
    {0x267f, 0x267f},   {0x2693, 0x2693},   {0x26a1, 0x26a1},
    {0x26aa, 0x26ab},   {0x26bd, 0x26be},   {0x26c4, 0x26c5},
    {0x26ce, 0x26ce},   {0x26d4, 0x26d4},   {0x26ea, 0x26ea},
    {0x26f2, 0x26f3},   {0x26f5, 0x26f5},   {0x26fa, 0x26fa},
    {0x26fd, 0x26fd},   {0x2705, 0x2705},   {0x270a, 0x270b},
    {0x2728, 0x2728},   {0x274c, 0x274c},   {0x274e, 0x274e},
    {0x2753, 0x2755},   {0x2757, 0x2757},   {0x2795, 0x2797},
    {0x27b0, 0x27b0},   {0x27bf, 0x27bf},   {0x2b1b, 0x2b1c},
    {0x2b50, 0x2b50},   {0x2b55, 0x2b55},   {0x2e80, 0x2e99},
    {0x2e9b, 0x2ef3},   {0x2f00, 0x2fd5},   {0x2ff0, 0x2ffb},
    {0x3001, 0x303e},   {0x3041, 0x3096},   {0x3099, 0x30ff},
    {0x3105, 0x312e},   {0x3131, 0x318e},   {0x3190, 0x31ba},
    {0x31c0, 0x31e3},   {0x31f0, 0x321e},   {0x3220, 0x3247},
    {0x3250, 0x32fe},   {0x3300, 0x4dbf},   {0x4e00, 0xa48c},
    {0xa490, 0xa4c6},   {0xa960, 0xa97c},   {0xac00, 0xd7a3},
    {0xf900, 0xfaff},   {0xfe10, 0xfe19},   {0xfe30, 0xfe52},
    {0xfe54, 0xfe66},   {0xfe68, 0xfe6b},   {0x16fe0, 0x16fe1},
    {0x17000, 0x187ec}, {0x18800, 0x18af2}, {0x1b000, 0x1b11e},
    {0x1b170, 0x1b2fb}, {0x1f004, 0x1f004}, {0x1f0cf, 0x1f0cf},
    {0x1f18e, 0x1f18e}, {0x1f191, 0x1f19a}, {0x1f200, 0x1f202},
    {0x1f210, 0x1f23b}, {0x1f240, 0x1f248}, {0x1f250, 0x1f251},
    {0x1f260, 0x1f265}, {0x1f300, 0x1f320}, {0x1f32d, 0x1f335},
    {0x1f337, 0x1f37c}, {0x1f37e, 0x1f393}, {0x1f3a0, 0x1f3ca},
    {0x1f3cf, 0x1f3d3}, {0x1f3e0, 0x1f3f0}, {0x1f3f4, 0x1f3f4},
    {0x1f3f8, 0x1f43e}, {0x1f440, 0x1f440}, {0x1f442, 0x1f4fc},
    {0x1f4ff, 0x1f53d}, {0x1f54b, 0x1f54e}, {0x1f550, 0x1f567},
    {0x1f57a, 0x1f57a}, {0x1f595, 0x1f596}, {0x1f5a4, 0x1f5a4},
    {0x1f5fb, 0x1f64f}, {0x1f680, 0x1f6c5}, {0x1f6cc, 0x1f6cc},
    {0x1f6d0, 0x1f6d2}, {0x1f6eb, 0x1f6ec}, {0x1f6f4, 0x1f6f8},
    {0x1f910, 0x1f93e}, {0x1f940, 0x1f94c}, {0x1f950, 0x1f96b},
    {0x1f980, 0x1f997}, {0x1f9c0, 0x1f9c0}, {0x1f9d0, 0x1f9e6},
    {0x20000, 0x2fffd}, {0x30000, 0x3fffd},
};

static int unicode_rangecmp(const void *key, const void *cm) {
  const utf8_range_t *range = (const utf8_range_t *)cm;
  int ch = *(int *)key;

  return (ch < range->lower) ? -1 : (ch >= range->upper) ? 1 : 0;
}

static int utf8_inrange(const utf8_range_t *range, int num, int ch) {
  const utf8_range_t *r = (const utf8_range_t *)bsearch(
      &ch, range, num, sizeof(*range), unicode_rangecmp);
  return r ? 1 : 0;
}

int utf8_iswidechar(int unicode) {
  return utf8_inrange(unicode_range_wide, arraysize(unicode_range_wide),
                      unicode);
}

int utf8_iscombiningchar(int unicode) {
  return utf8_inrange(unicode_range_combining,
                      arraysize(unicode_range_combining), unicode);
}

int utf8_charwidth(int ch) {
  /* short circuit for common case */
  if (isascii(ch))
    return 1;

  if (utf8_inrange(unicode_range_combining, arraysize(unicode_range_combining),
                   ch))
    return 0;

  if (utf8_inrange(unicode_range_wide, arraysize(unicode_range_wide), ch))
    return 2;

  return 1;
}

int utf8_strwidth(const char *str, int charlen) {
  int l, ch, width = 0;

  while (charlen) {
    l = utf8_tounicode(str, &ch);
    width += utf8_charwidth(ch);
    str += l;
    charlen -= 1;
  }
  return width;
}

int utf8_utf8tounicode(const char *utf8str, wchar_t *wcstr) {
  int len = (int)strlen(utf8str);
  wchar_t *ptr = wcstr;
  int ch, n;

  while (len > 0) {
    n = utf8_tounicode(utf8str, &ch);

    if (n <= 0)
      break;

    *ptr++ = (wchar_t)ch;
    utf8str += n;
    len -= n;
  }

  return (int)(ptr - wcstr);
}

int utf8_utf8toansi(const char *utf8str, char *ansistr) {
  int utf8len = (int)strlen(utf8str);
  int utf8strlen = utf8_strlen(utf8str, utf8len) + 1;

  dynarray(wchar_t, wcstr, utf8strlen);
  memset(wcstr, 0, utf8strlen * sizeof(wchar_t));

  utf8_utf8tounicode(utf8str, wcstr);
  return utf8_unicodetoansi(wcstr, ansistr);
}

int utf8_unicodetoutf8(const wchar_t *wcstr, char *utf8str) {
  char *ptr = utf8str;
  const wchar_t *wcsptr = wcstr;
  const wchar_t *end = wcstr + wcslen(wcstr);
  int n;

  while (wcsptr < end) {
    n = utf8_fromunicode(ptr, *wcsptr++);

    if (n <= 0)
      break;
    ptr += n;
  }

  return (int)(ptr - utf8str);
}

int utf8_unicodetoansi(const wchar_t *wcstr, char *ansistr) {
  char *ptr = ansistr;
  const wchar_t *wcsptr = wcstr;
  const wchar_t *end = wcstr + wcslen(wcstr);
  int n;

  mbstate_t mbstate;
  memset(&mbstate, 0, sizeof(mbstate_t));

  while (wcsptr != end) {
    n = (int)wcrtomb(ptr, *wcsptr++, &mbstate);

    if (n <= 0)
      break;
    ptr += n;
  }

  return (int)(ptr - ansistr);
}

int utf8_ansitoutf8(const char *ansistr, char *utf8str) {
  int ansilen = (int)strlen(ansistr) + 1;

  dynarray(wchar_t, wcstr, ansilen);
  memset(wcstr, 0, ansilen * sizeof(wchar_t));

  utf8_ansitounicode(ansistr, wcstr);
  return utf8_unicodetoutf8(wcstr, utf8str);
}

int utf8_ansitounicode(const char *ansistr, wchar_t *wcstr) {
  int len = (int)strlen(ansistr);
  wchar_t *ptr = wcstr;
  wchar_t ch;
  int n;

  mbstate_t mbstate;
  memset(&mbstate, 0, sizeof(mbstate_t));

  while (len > 0) {
    n = (int)mbrtowc(&ch, ansistr, min(len, MB_LEN_MAX), &mbstate);

    if (n <= 0)
      break;

    *ptr++ = ch;
    ansistr += n;
    len -= n;
  }

  return (int)(ptr - wcstr);
}
