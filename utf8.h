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

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

int utf8_isutf8(const char *bytes, int bytelen);
int utf8_toutf8(char *utf8, int unicode);
int utf8_tounicode(const char *utf8, int *unicode);
int utf8_charlen(int ch);
int utf8_strlen(const char *utf8, int bytelen);
int utf8_index(const char *utf8, int index);
int utf8_iswidechar(int unicode);
int utf8_iscombiningchar(int unicode);
int utf8_charwidth(int ch);
int utf8_strwidth(const char *str, int charlen);

int utf8_utf8tounicode(const char *utf8str, wchar_t *wcstr);
int utf8_utf8toansi(const char *utf8str, char *ansistr);
int utf8_unicodetoutf8(const wchar_t *wcstr, char *utf8str);
int utf8_unicodetoansi(const wchar_t *wcstr, char *ansistr);
int utf8_ansitoutf8(const char *ansistr, char *utf8str);
int utf8_ansitounicode(const char *ansistr, wchar_t *wcstr);

#ifdef __cplusplus
};
#endif

#endif /* ___UTF8_H__ */
