/*! 
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    /* initialise menu and register event listeners here */
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

/* -------------------------------------------------------------------------- */
class menu : public Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/* -------------------------------------------------------------------------- */
TEST_F(menu, example_test)
{
    EXPECT_THAT(1, Eq(1));
}

#endif /* TESTING */

