#include <common/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/*Macro variables----------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_fputchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifndef CONFIG_CONSOLE_BUFSZ
#define CONFIG_CONSOLE_BUFSZ    100
#endif

PUTCHAR_PROTOTYPE
{
    putchar(ch);
    return ch;
}

int print(const char* fmt, ...)
{
    static char buffer[CONFIG_CONSOLE_BUFSZ];
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = vsnprintf((char*)buffer, sizeof(buffer) - 1, fmt, args);
    va_end(args);
    return ret;
}

