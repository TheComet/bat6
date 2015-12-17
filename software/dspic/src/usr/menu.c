/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"
#include "usr/solar_panels.h"
#include "core/event.h"
#include <stddef.h>

typedef enum menu_state_e
{
    STATE_NAVIGATE_MANUFACTURERS,
    STATE_NAVIGATE_PANELS
} menu_state_e;

struct menu_t
{
    menu_state_e state;
    short selected_item;
    short max_items;
};

static struct menu_t menu = {};

static void on_button(unsigned int arg);

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void menu_update(void)
{
    /*char buffer[40];*/

    switch(menu.state)
    {
        case STATE_NAVIGATE_MANUFACTURERS :
            menu.max_items = solar_panels_get_manufacturers_count();
            menu.selected_item = 0;
            break;

        case STATE_NAVIGATE_PANELS :
            menu.max_items = solar_panels_get_panel_count(menu.selected_item);
            menu.selected_item = 0;
            break;
    }
}

/* -------------------------------------------------------------------------- */
static void handle_button_twist(unsigned int button)
{
    if(button == BUTTON_TWISTED_LEFT)
    {
        ++menu.selected_item;
        menu.selected_item = (menu.selected_item == menu.max_items ?
                0 : menu.selected_item);
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        --menu.selected_item;
        menu.selected_item = (menu.selected_item == -1 ?
                menu.max_items -1 : menu.selected_item);
    }
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int button)
{
    handle_button_twist(button);
    menu_update();

    switch(menu.state)
    {
        case STATE_NAVIGATE_MANUFACTURERS :
            if(button == BUTTON_RELEASED)
            break;

        case STATE_NAVIGATE_PANELS :
            break;

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
