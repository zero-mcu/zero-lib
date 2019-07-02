#include <common/zero.h>
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
    stdio_putchar(ch);
    return ch;
}

static ze_stdio_t* ze_stdio = NULL;

int stdio_init(ze_stdio_t* stdio)
{
    ze_stdio = stdio;
    return 0;
}

int stdio_putchar(int ch)
{
    if (ze_stdio && ze_stdio->put_char)
        return ze_stdio->put_char(ch);
    return -1;
}


int stdio_getchar(void)
{
    if (ze_stdio && ze_stdio->get_char)
        return ze_stdio->get_char();
    return -1;
}

int stdio_write(const char* wr_data, ze_size_t wr_len)
{
    int i = 0;
    for (i = 0; i < wr_len; i++)
    {
        if (stdio_putchar(wr_data[i]) < 0)
            break;
    }
    return i;
}

int print(const char* fmt, ...)
{
    static char buffer[CONFIG_CONSOLE_BUFSZ];
    int ret = 0;
    va_list args;
    va_start(args, fmt);
    ret = vsnprintf((char*)buffer, sizeof(buffer) - 1, fmt, args);
    va_end(args);
    return stdio_write(buffer, ret);
}

