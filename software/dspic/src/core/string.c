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
void str_nstrcat(char* dest, short dest_n, short src_n, ...)
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
}

/* -------------------------------------------------------------------------- */
void str_2nstrcat(char* dest, short n, const char* s1, const char* s2)
{
    --n; /* reserve space for null terminator */
    while(*s1 && n --> 0)
        *dest++ = *s1++;
    while(*s2 && n --> 0)
        *dest++ = *s2++;
    *dest = '\0';
}

/* -------------------------------------------------------------------------- */
void str_nitoa(char* dest, short digits, short number)
{
    char buffer[5], *ptr; /* enough to hold a short without null terminator */
    unsigned char is_negative = 0;

    /* handle 0 explicitly */
    if(number == 0)
    {
        *dest++ = '0';
        *dest = '\0';
        return;
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
        *dest++ = '-';

    /* copy buffer into destination */
    while(ptr-- != buffer && digits --> 0)
        *dest++ = *ptr;
    *dest = '\0';
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
    str_nstrcat(buffer, 10, 4, "This ", "is ", "a ", "test");
    EXPECT_THAT(buffer, StrEq("This is a")); /* 9 characters */
}

TEST(string, nstrcat_one_string)
{
    char buffer[10];
    str_nstrcat(buffer, 10, 1, "test");
    EXPECT_THAT(buffer, StrEq("test"));
}

TEST(string, nstrcat_empty_strings)
{
    char buffer[10];
    str_nstrcat(buffer, 10, 4, "", "", "", "");
    EXPECT_THAT(buffer, StrEq(""));
}

TEST(string, nitoa_positive_number)
{
    char s[32];
    str_nitoa(s, 4, 8372);
    EXPECT_THAT(s, StrEq("8372"));
}

TEST(string, nitoa_negative_number)
{
    char s[32];
    str_nitoa(s, 4, -4837);
    EXPECT_THAT(s, StrEq("-4837"));
}

TEST(string, nitoa_zero)
{
    char s[32];
    str_nitoa(s, 5, 0);
    EXPECT_THAT(s, StrEq("0"));
}

TEST(string, nitoa_smaller_target_buffer)
{
    char s[32];
    str_nitoa(s, 2, -18478);
    EXPECT_THAT(s, StrEq("-18"));
}

#endif /* TESTING */
