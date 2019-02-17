/*
 *  test.c
 *
 *  copyright (c) 2019 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 *
 *  https://github.com/shixiongfei/utf8
 */

#include "utf8.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define BUFSIZE 64

int main(int argc, char *argv[]) {
  const char *string = {"HelloWorld, 你好世界！"};

  char str[BUFSIZE] = {0};
  char ansistr[BUFSIZE] = {0};
  wchar_t wcstr[BUFSIZE] = {0};
  char ustr1[BUFSIZE] = {0};
  char ustr2[BUFSIZE] = {0};

  /* `setlocale` is required! */
  setlocale(LC_ALL, "");

  /* check this file is utf8? */
  if (utf8_isutf8(string, -1))
    strcpy(str, string);
  else
    utf8_ansitoutf8(string, str);

  printf("test string: %s\n", str);
  printf("is utf8? %s\n", utf8_isutf8(str, -1) ? "yes" : "no");
  printf("strlen: %d\n", (int)strlen(str));
  printf("utf8_strlen: %d\n", utf8_strlen(str, -1));
  printf("utf8_strwidth: %d\n", utf8_strwidth(str, utf8_strlen(str, -1)));
  printf("utf8 to unicode: %d\n", utf8_utf8tounicode(str, wcstr));
  printf("utf8 to ansi: %d\n", utf8_utf8toansi(str, ansistr));
  wprintf(L"unicode string: %ls\n", wcstr);
  printf("unicode to utf8: %d\n", utf8_unicodetoutf8(wcstr, ustr1));
  printf("ansi to utf8: %d\n", utf8_ansitoutf8(ansistr, ustr2));
  printf("compare: %s, %s\n",
         (0 == memcmp(str, ustr1, strlen(ustr1))) ? "str eq ustr1"
                                                  : "str neq ustr1",
         (0 == memcmp(str, ustr2, strlen(ustr2))) ? "str eq ustr2"
                                                  : "str neq ustr2");

  return 0;
}
