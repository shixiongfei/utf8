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

#define BUFSIZE 64

int main(int argc, char *argv[]) {
  const char string[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x57, 0x6f, 0x72,
                         0x6c, 0x64, 0x2c, 0x20, 0xe4, 0xbd, 0xa0, 0xe5,
                         0xa5, 0xbd, 0xe4, 0xb8, 0x96, 0xe7, 0x95, 0x8c,
                         0xef, 0xbc, 0x81, 0xF0, 0x9F, 0x98, 0x80, 0x00};

  ucs4_t ucstr[BUFSIZE] = {0};
  char mbstr[BUFSIZE] = {0};
  char ustr1[BUFSIZE] = {0};
  char ustr2[BUFSIZE] = {0};
  int i, n;

  printf("test string is utf8? %s\n", utf8_isutf8(string, -1) ? "yes" : "no");

  printf("test string:");
  for (i = 0; i < (int)strlen(string); ++i)
    printf(" 0x%0x", (unsigned char)string[i]);
  printf("\n");

  printf("strlen: %d\n", (int)strlen(string));
  printf("utf8_strlen: %d\n", utf8_strlen(string, -1));
  printf("utf8_strwidth: %d\n", utf8_strwidth(string, utf8_strlen(string, -1)));

  printf("utf8 decode: %d\n", (n = utf8_decode(string, ucstr)));

  for (i = 0; i < n; ++i)
    printf(" 0x%0x", ucstr[i]);
  printf("\n");

  printf("utf8 encode: %d\n", utf8_encode(ucstr, ustr1));

  for (i = 0; i < (int)strlen(ustr1); ++i)
    printf(" 0x%0x", (unsigned char)ustr1[i]);
  printf("\n");

  printf("utf8 to gbk: %d\n", utf8_tomultibyte("GBK", string, mbstr));

  for (i = 0; i < (int)strlen(mbstr); ++i)
    printf(" 0x%0x", (unsigned char)mbstr[i]);
  printf("\n");

  printf("utf8 from gbk: %d\n", utf8_frommultibyte("GBK", mbstr, ustr2));

  for (i = 0; i < (int)strlen(ustr2); ++i)
    printf(" 0x%0x", (unsigned char)ustr2[i]);
  printf("\n");

  printf("compare: %s, %s\n",
         (0 == memcmp(string, ustr1, strlen(string))) ? "str eq ustr1"
                                                      : "str neq ustr1",
         (0 == memcmp(string, ustr2, strlen(string))) ? "str eq ustr2"
                                                      : "str neq ustr2");
  return 0;
}
