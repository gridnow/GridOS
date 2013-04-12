/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#include <types.h>
#include <stddef.h>
#include <arch/boot.h>

#include "utils.h"

static void *startup_args_base = (void *)STARTUP_ARGS_ADDR;
static off_t alloc_offset = 0;
void* alloc_startup_args_mem(size_t size)
{
    char *ret = (char *)startup_args_base + alloc_offset;
    /* size must align 4 bytes */
    size = (size & 0x3) ? (size + (4 - (size & 0x3)) ) : size;
    alloc_offset += size;
    
    return (void *)ret;
}

int __strlen(const char *str)
{
    char *s = (char *)str;
    while(*s++) /* nothing here */;
    
    return (int)((unsigned long)s - (unsigned long)str);
}

int __strcmp(const char *s1, const char *s2)
{
    char *str1 = (char*)s1;
    char *str2 = (char*)s2;
    
    while(*str1++ == *str2++);
    
    return (int)(*str1 - *str2);
}

void __strcpy(char *dst, char *src)
{
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
}

#ifdef __i386__
/*
 * Standard VGA
 * mode: 80 x 25, 16 colors
 * base address: 0xb8000
 */
#define VID_BASEADDR    0xB8000
#define VID_WIDTH       80
#define VID_HEIGHT      25

static unsigned char *video;
/* 下面的代码偷懒了，直接copy
 * 来在于grub中的实例代码
 */
int xpos, ypos;
/* Clear the screen and initialize VIDEO, XPOS and YPOS.  */
void cls (void)
{
    int i;

    video = (unsigned char *) VID_BASEADDR;
  
    for (i = 0; i < VID_WIDTH * VID_HEIGHT * 2; i++)
        *(video + i) = 0;

    xpos = 0;
    ypos = 0;
}

/* Put the character C on the screen.  */
static
void putchar (int c)
{
    if (c == '\n' || c == '\r')
    {
    newline:
        xpos = 0;
        ypos++;
        if (ypos >= VID_HEIGHT)
            ypos = 0;
        return;
    }

    *(video + (xpos + ypos * VID_WIDTH) * 2) = c & 0xFF;
    *(video + (xpos + ypos * VID_WIDTH) * 2 + 1) = 0x7;

    xpos++;
    if (xpos >= VID_WIDTH)
        goto newline;
}

#endif
/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
static
void itoa (char *buf, int base, int d)
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;
  
    /* If %d is specified and D is minus, put `-' in the head.  */
    if (base == 'd' && d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    }
    else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0.  */
    do
    {
        int remainder = ud % divisor;
      
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    /* Terminate BUF.  */
    *p = 0;
  
    /* Reverse BUF.  */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

/* Format a string and print it on the screen, just like the libc
   function printf.  */
void printf (const char *format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[20];

    arg++;

    while ((c = *format++) != 0)
    {
        if (c != '%')
            putchar (c);
        else
        {
            char *p;

            c = *format++;
            switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
                itoa (buf, c, *((int *) arg++));
                p = buf;
                goto string;
                break;

            case 's':
                p = *arg++;
                if (! p)
                    p = "(null)";
    string:
                while (*p)
                    putchar (*p++);
                break;

            default:
                putchar (*((int *) arg++));
            break;
            }
        }
    }
}

