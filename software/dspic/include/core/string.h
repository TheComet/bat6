/*!
 * @file string.c
 * @author Alex Murray
 *
 * Created on 19. December 2015, 00:38
 */

#ifndef STRING_H
#define STRING_H

#include <libq.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*!
 * @brief Reverses the order of a null terminated character array.
 * @note The data is modified in-place. If you wish to keep the original
 * string, make a copy first.
 * @param[in] str The string to reverse the order of.
 */
void str_reverse(char* str);

/*!
 * @brief Concatenates two or more strings and makes sure the target buffer size
 * is not exceeded.
 * @param[out] dest The destination buffer to write into.
 * @param[in] dest_n The destination buffer length, **including** null
 * terminator.
 * @param[in] src_n The number of vararg strings.
 * @param[in] ... src_n number of character arrays to concatenate.
 * @note If the total length of all input strings exceeds **dest_n**, the
 * function returns. In this case the destination buffer will be truncated.
 * @return Returns a pointer into the destination buffer pointing to the null
 * terminator at the end of the string.
 */
char* str_nstrcat(char* dest, short dest_n, short src_n, ...);

/*!
 * @brief Concatenates two strings and makes sure the target buffer size is not
 * exceeded.
 * @param[out] dest The destination buffer to write into.
 * @param[in] n The destination buffer length, **including** null terminator.
 * @param[in] s1 The first string to concatenate.
 * @param[in] s2 The second string to concatenate.
 * @note If the total length of all input strings exceeds **dest_n**, the
 * function returns. In this case the destination buffer will be truncated.
 * @return Returns a pointer into the destination buffer pointing to the null
 * terminator at the end of the string.
 */
char* str_2nstrcat(char* dest, short n, const char* s1, const char* s2);

/*!
 * @brief Appends a string to the end of the specified string and makes sure the
 * target buffer size is not exceeded.
 * @param[out] dest The destination buffer to write into.
 * @param[in] n The destination buffer length, **including** null terminator.
 * @param[in] src The source string to append to the buffer.
 *@note If the total length of the buffer is exceeded, the function returns.
 * This leaves the destination buffer in a truncated state.
 * @return Returns a pointer into the destination buffer pointing to the null
 * terminator at the end of the string.
 */
char* str_append(char* dest, short n, const char* src);

/*!
 * @brief Converts the first n digits of an integer into a string.
 * @param[out] dest The destination buffer to write into.
 * @param[in] digits The number of digits to write to the destination buffer,
 * starting with the most significant digit.
 * @note This is **NOT** the maximum size of the destination buffer.
 * The destination buffer must also account for two extra characters:
 * The minus character "-" and the null terminator.
 * @param[in] number The integer to convert into a string.
 * @return Returns a pointer into the destination buffer pointing to the null
 * terminator at the end of the string.
 */
char* str_nitoa(char* dest, short n, short number);

/*!
 * @brief Converts a _Q16 fixed point type into a string.
 * @param[out] dest The destination buffer to write to. The size of this buffer
 * must be at least
 */
char* str_q16itoa(char* dest, short n, _Q16 value);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
