/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"
#include "core/event.h"

typedef enum menu_state_e
{
    STATE_MANUFACTURERS
} menu_state_e;

struct menu_t
{
    menu_state_e state;
};

static struct menu_t menu;

static void on_button(unsigned int arg);

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    menu.state = STATE_MANUFACTURERS;
    
    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void menu_next_item(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void menu_previous_item(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void menu_select_item(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void menu_go_back(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int arg)
{
    switch(arg)
    {
        case BUTTON_PRESSED        : menu_select_item();   break;
        case BUTTON_PRESSED_LONGER : menu_go_back();       break;
        case BUTTON_TWISTED_LEFT   : menu_next_item();     break;
        case BUTTON_TWISTED_RIGHT  : menu_previous_item(); break;
        default: break;
    }
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
