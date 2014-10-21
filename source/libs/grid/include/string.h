
#ifndef _STRING_H
#define _STRING_H

int memcmp(const void *cs, const void *ct, size_t count);
void *memcpy(void *dst, const void *src, unsigned long len);
void *memset(void *dst, int value, unsigned long count);
char *strcat(char *dest, const char *src);
char * strchr (const char * string, int ch);
int strcmp(const char *src1, const char *src2);
char *strcpy (char *dest, const char *src);
unsigned long strlen(const char * str);
int strncmp (const char *s1, const char *s2, size_t n);
char *strncpy (char *dest, const char *src, int len);
size_t strnlen(const char *s, size_t maxlen);
char *strrchr(const char *s, int c);

#endif
