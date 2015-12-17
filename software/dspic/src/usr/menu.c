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

struct selection_t
{
    short item;
    short max;
    short scroll;
};

struct menu_t
{
    menu_state_e state;
    struct selection_t selection;
};

struct menu_t menu = {};

static void on_button(unsigned int arg);

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void handle_item_selection(unsigned int button)
{
    if(menu.selection.item == -1)
        return;

    if(button == BUTTON_TWISTED_LEFT)
    {
        --menu.selection.item;

        if(menu.selection.item == -1)
        {
            menu.selection.item = menu.selection.max - 1;
            menu.selection.scroll = menu.selection.max - 4;
            if(menu.selection.scroll < 0)
                menu.selection.scroll = 0;
        }

        if(menu.selection.item < menu.selection.scroll)
            --menu.selection.scroll;
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        ++menu.selection.item;

        if(menu.selection.item == menu.selection.max)
        {
            menu.selection.item = 0;
            menu.selection.scroll = 0;
        }

        if(menu.selection.item - menu.selection.scroll >= 4)
            ++menu.selection.scroll;
    }
}

/* -------------------------------------------------------------------------- */
static void handle_menu_switches(unsigned int button)
{
#define reset_selection()                                      \
        menu.selection.item = (menu.selection.max ? 0 : -1);   \
        menu.selection.scroll = 0;

    switch(menu.state)
    {
        case STATE_NAVIGATE_MANUFACTURERS :

            /* Switch to panel selection of the current manufacturer */
            if(button == BUTTON_RELEASED && menu.selection.item != -1)
            {
                menu.selection.max =
                        solar_panels_get_panel_count(menu.selection.item);
                reset_selection();
                menu.state = STATE_NAVIGATE_PANELS;
            }

            break;

        case STATE_NAVIGATE_PANELS :

            /* Switch back to manufacturer selection */
            if(button == BUTTON_PRESSED_LONGER)
            {
                menu.selection.max = solar_panels_get_manufacturers_count();
                reset_selection();
                menu.state = STATE_NAVIGATE_MANUFACTURERS;
            }
            break;

        default: break;
    }
}

/* -------------------------------------------------------------------------- */
static void menu_update(void)
{
    //char buffer[40];
    int i;

    for(i = menu.selection.scroll;
        i < menu.selection.max && i != menu.selection.scroll + 4;
        ++i)
    {
        switch(menu.state)
        {
            case STATE_NAVIGATE_MANUFACTURERS :
                break;

            case STATE_NAVIGATE_PANELS :
                break;
        }
    }
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int button)
{
    handle_item_selection(button);
    handle_menu_switches(button);
    menu_update();
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifndef TESTING
#define TESTING
#error fuck you
#endif

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

static void reset_menu_state()
{
    menu.state = STATE_NAVIGATE_MANUFACTURERS;
    menu.selection.item = -1;
    menu.selection.max = 0;
    menu.selection.scroll = 0;
}

/* -------------------------------------------------------------------------- */
class oled_menu : public Test
{
    virtual void SetUp()
    {
        reset_menu_state();
    }

    virtual void TearDown() {}
};

/* -------------------------------------------------------------------------- */
TEST_F(oled_menu, twisting_right_with_no_items_does_nothing)
{
    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(-1));
    EXPECT_THAT(menu.selection.max, Eq(0));
    EXPECT_THAT(menu.selection.scroll, Eq(0));
}

TEST_F(oled_menu, twisting_left_with_no_items_does_nothing)
{
    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(-1));
    EXPECT_THAT(menu.selection.max, Eq(0));
    EXPECT_THAT(menu.selection.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_right_wraps_correctly)
{
    menu.selection.item = 0;
    menu.selection.max = 5;
    menu.selection.scroll = 0;

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(1));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(2));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(3));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(4));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(1));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.selection.item, Eq(0));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_left_wraps_correctly)
{
    menu.selection.item = 0;
    menu.selection.max = 5;
    menu.selection.scroll = 0;

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(4));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(3));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(2));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(1));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.selection.item, Eq(0));
    EXPECT_THAT(menu.selection.max, Eq(5));
    EXPECT_THAT(menu.selection.scroll, Eq(0));
}

#endif /* TESTING */
