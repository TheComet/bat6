/*!
 * @file string.c
 * @author Alex Murray
 *
 * Created on 19. December 2015, 00:38
 */

#include "core/string.h"
#include <string.h>
#include <stdarg.h>

/* -------------------------------------------------------------------------- */
void str_reverse(char* str)
{
    char* end;

    end = str + strlen(str) - 1;
    for(; str < end; ++str, --end)
    {
        char tmp = *str;
        *str = *end;
        *end = tmp;
    }
}

/* -------------------------------------------------------------------------- */
char* str_nstrcat(char* dest, short dest_n, short src_n, ...)
{
    va_list ap;
    short i;

    --dest_n; /* reserve space for null terminator */

    va_start(ap, src_n);
        for(i = 0; i != src_n; ++i)
        {
            const char* str = va_arg(ap, char*);
            while(*str && dest_n --> 0)
                *dest++ = *str++;
        }
    va_end(ap);

    *dest = '\0';
    return dest;
}

/* -------------------------------------------------------------------------- */
char* str_2nstrcat(char* dest, short n, const char* s1, const char* s2)
{
    --n; /* reserve space for null terminator */
    while(*s1 && n --> 0)
        *dest++ = *s1++;
    while(*s2 && n --> 0)
        *dest++ = *s2++;
    *dest = '\0';
    return dest;
}

/* -------------------------------------------------------------------------- */
char* str_append(char* dest, short n, const char* src)
{
    --n; /* null terminator is always written */
    while(*dest && n --> 0)
        ++dest;
    while(*src && n --> 0)
        *dest++ = *src++;
    *dest = '\0';
    return dest;
}

/* -------------------------------------------------------------------------- */
char* str_nitoa(char* dest, short digits, short number)
{
    char buffer[5], *ptr; /* enough to hold a short without null terminator */
    unsigned char is_negative = 0;

    --digits; /* always have space for null terminator */

    /* handle 0 explicitly */
    if(number == 0)
    {
        *dest++ = '0';
        *dest = '\0';
        return dest;
    }

    /* handle negative numbers */
    if(number < 0)
    {
        number = -number;
        is_negative = 1;
    }

    /* do conversion in reverse */
    ptr = buffer;
    while(number != 0)
    {
        short remainder = number % 10;
        number /= 10;
        *ptr++ = remainder + '0';
    }

    /* add "-" character if it is negative */
    if(is_negative)
    {
        *dest++ = '-';
        --digits;
    }

    /* copy buffer into destination */
    while(ptr-- != buffer && digits --> 0)
        *dest++ = *ptr;
    *dest = '\0';
    return dest;
}

/* -------------------------------------------------------------------------- */
char* str_q16itoa(char* dest, short n, _Q16 value)
{
    char* ptr;
    short before_decimal = value >> 16;
    unsigned short after_decimal = (value & 0xFFFF) * 10000 / 65536;

    if(after_decimal && before_decimal < 0)
        ++before_decimal;

    /* convert before decimal, update n */
    ptr = str_nitoa(dest, n, before_decimal);
    n -= ptr - dest;

    /* not enough space for decimal point and at least one number? */
    if(n < 2)
        return ptr;

    *ptr++ = '.'; *ptr = '\0';
    return str_nitoa(ptr, n - 1, after_decimal);
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING
#include "gmock/gmock.h"

using namespace ::testing;

TEST(string, reverse)
{
    char s[15];
    strcpy(s, "this is a test");
    str_reverse(s);
    EXPECT_THAT(s, StrEq("tset a si siht"));
}

TEST(string, reverse_empty_string)
{
    char s[2];
    strcpy(s, "");
    str_reverse(s);
    EXPECT_THAT(s, StrEq(""));
}

TEST(string, nstrcat)
{
    char buffer[10];
    char* ret = str_nstrcat(buffer, 10, 4, "This ", "is ", "a ", "test");
    EXPECT_THAT(ret, Eq(buffer + 9));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("This is a")); /* 9 characters */
}

TEST(string, nstrcat_aliasing_buffers)
{
    char buffer[32];
    char* ret = str_nstrcat(buffer, 32, 3, "this ", "is ", "a ");
    EXPECT_THAT(ret, Eq(buffer + 10));
    ASSERT_THAT(*ret, Eq('\0'));

    ret = str_nstrcat(buffer, 32, 2, buffer, "test");
    EXPECT_THAT(ret, Eq(buffer + 14));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("this is a test"));
}

TEST(string, append_to_empty_string)
{
    char buffer[32];
    buffer[0] = '\0';
    char* ret = str_append(buffer, 32, "test");
    EXPECT_THAT(ret, Eq(buffer + 4));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("test"));
}

TEST(string, append_empty_string)
{
    char buffer[32];
    strcpy(buffer, "this is ");
    char* ret = str_append(buffer, 32, "");
    EXPECT_THAT(ret, Eq(buffer + 8));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("this is "));
}

TEST(string, append_string)
{
    char buffer[32];
    strcpy(buffer, "this is ");
    char* ret = str_append(buffer, 32, "a test");
    EXPECT_THAT(ret, Eq(buffer + 14));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("this is a test"));
}

TEST(string, append_string_with_smaller_target_buffer)
{
    char buffer[5];
    strcpy(buffer, "th");
    char* ret = str_append(buffer, 5, "is a test");
    EXPECT_THAT(ret, Eq(buffer + 4));
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(buffer, StrEq("this"));
}

TEST(string, nitoa_positive_number)
{
    char s[32];
    str_nitoa(s, 5, 8372);
    EXPECT_THAT(s, StrEq("8372"));
}

TEST(string, nitoa_negative_number)
{
    char s[32];
    str_nitoa(s, 6, -4837);
    EXPECT_THAT(s, StrEq("-4837"));
}

TEST(string, nitoa_zero)
{
    char s[32];
    str_nitoa(s, 2, 0);
    EXPECT_THAT(s, StrEq("0"));
}

TEST(string, nitoa_smaller_target_buffer)
{
    char s[32];
    str_nitoa(s, 4, -18478);
    EXPECT_THAT(s, StrEq("-18"));
}

TEST(string, Q16itoa)
{
    char s[32];
    _Q16 value = (_Q16)(float)(11.5 * 65536);
    char* ret = str_q16itoa(s, 5, value);
    ASSERT_THAT(*ret, Eq('\0'));
    EXPECT_THAT(s, StrEq("11.5"));
}

#endif /* TESTING */
