/*
 *  utf8.h
 *
 *  copyright (c) 2019 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 *
 *  https://github.com/shixiongfei/utf8
 */

#ifndef ___UTF8_H__
#define ___UTF8_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ucs4_t
#define ucs4_t int
#endif

int utf8_isutf8(const char *bytes, int bytelen);
int utf8_fromunicode(char *utf8, ucs4_t unicode);
int utf8_tounicode(const char *utf8, ucs4_t *unicode);
int utf8_charlen(ucs4_t ch);
int utf8_strlen(const char *utf8, int bytelen);
int utf8_index(const char *utf8, int index);
int utf8_iswidechar(ucs4_t unicode);
int utf8_iscombiningchar(ucs4_t unicode);
int utf8_charwidth(ucs4_t ch);
int utf8_strwidth(const char *str, int charlen);

int utf8_encode(const ucs4_t *ucstr, char *utf8str);
int utf8_decode(const char *utf8str, ucs4_t *ucstr);

int utf8_frommultibyte(const char *codepage, const char *mbstr, char *utf8str);
int utf8_tomultibyte(const char *codepage, const char *utf8str, char *mbstr);

#ifdef __cplusplus
};
#endif

#endif /* ___UTF8_H__ */
