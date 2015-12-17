/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"
#include "core/event.h"

static void on_button(unsigned int arg);

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int arg)
{
    
}

/* -------------------------------------------------------------------------- */

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
