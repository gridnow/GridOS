/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#ifndef UTILS_H
#define	UTILS_H


void* alloc_startup_args_mem(size_t size);

int __strlen(const char *str);
int __strcmp(const char *s1, const char *s2);
void __strcpy(char *dst, char *src);

void cls (void);
void printf (const char *format, ...);

#endif	/* UTILS_H */

