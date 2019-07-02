/**
 * @file string.h
 * @brief This is the difinition of string.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/05/31    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-05-31
 * @license Dual BSD/GPL
 */
#ifndef __COMMON_STRING_H__
#define __COMMON_STRING_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    int (*put_char)(int ch);
    int (*get_char)(void);
} ze_stdio_t;


int stdio_init(ze_stdio_t* stdio);

/**
 * @brief print format string to console.
 * @param fmt format string.
 * @return number of char has been print. if retval < 0,
 * execute error.
 */
int print(const char* fmt, ...);

/**
 * @brief put char to console
 * @param char to be put to console
 * @return 0 for success, < 0 for failed.
 */
int stdio_putchar(int ch);

/**
 * @brief get char from console
 * @return the char has been get from char, < 0 for failed.
 */
int stdio_getchar(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __COMMON_STRING_H__