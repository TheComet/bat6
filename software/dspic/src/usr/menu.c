/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"
#include "usr/solar_panels.h"
#include "drv/lcd.h"
#include "core/event.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef enum menu_state_e
{
    STATE_INIT,
    STATE_NAVIGATE_MANUFACTURERS,
    STATE_NAVIGATE_PANELS
} menu_state_e;

struct item_t
{
    short selected;
    short max;
    short scroll;
};

struct manufacturer_t
{
    short selected;
};

struct menu_t
{
    menu_state_e state;
    struct item_t item;
    struct manufacturer_t manufacturer;
};

struct menu_t menu;

static void handle_menu_switches(unsigned int button);
static void menu_update(void);
static void on_button(unsigned int button);

#ifdef TESTING
int solar_panels_get_manufacturers_count_test();
int solar_panels_get_panel_count_test(int manufacturer);
const char* solar_panels_get_manufacturer_name_test(int manufacturer);
const char* solar_panels_get_model_name_test(int manufacturer, int panel);
void lcd_writeline_test(int line, const char* str);
#   define solar_panels_get_manufacturers_count \
            solar_panels_get_manufacturers_count_test
#   define solar_panels_get_panel_count \
            solar_panels_get_panel_count_test
#   define solar_panels_get_manufacturer_name \
            solar_panels_get_manufacturer_name_test
#   define solar_panels_get_model_name \
            solar_panels_get_model_name_test
#   define lcd_writeline \
            lcd_writeline_test
#endif

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    menu.item.selected = -1;
    menu.item.max = 0;
    menu.item.scroll = 0;
    menu.manufacturer.selected = -1;
    menu.state = STATE_INIT;

    handle_menu_switches(0);
    menu_update();

    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void handle_item_selection(unsigned int button)
{
    if(menu.item.selected == -1)
        return;

    if(button == BUTTON_TWISTED_LEFT)
    {
        --menu.item.selected;

        if(menu.item.selected == -1)
        {
            menu.item.selected = menu.item.max - 1;
            menu.item.scroll = menu.item.max - 4;
            if(menu.item.scroll < 0)
                menu.item.scroll = 0;
        }

        if(menu.item.selected < menu.item.scroll)
            --menu.item.scroll;
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        ++menu.item.selected;

        if(menu.item.selected == menu.item.max)
        {
            menu.item.selected = 0;
            menu.item.scroll = 0;
        }

        if(menu.item.selected - menu.item.scroll >= 4)
            ++menu.item.scroll;
    }
}

/* -------------------------------------------------------------------------- */
static void handle_menu_switches(unsigned int button)
{
#define reset_selection()                                \
        menu.item.selected = (menu.item.max ? 0 : -1);   \
        menu.item.scroll = 0;

    switch(menu.state)
    {
        case STATE_INIT :
            menu.item.max = solar_panels_get_manufacturers_count();
            reset_selection();
            menu.state = STATE_NAVIGATE_MANUFACTURERS;
            break;

        case STATE_NAVIGATE_MANUFACTURERS :

            /* Switch to panel selection of the current manufacturer */
            if(button == BUTTON_RELEASED && menu.item.selected != -1)
            {
                /* abort if there are no panels */
                short panels = solar_panels_get_panel_count(menu.item.selected);
                if(!panels)
                    break;

                /* select current item as selected manufacturer */
                menu.manufacturer.selected = menu.item.selected;
                menu.item.max = panels;
                reset_selection();

                menu.state = STATE_NAVIGATE_PANELS;
            }

            break;

        case STATE_NAVIGATE_PANELS :

            /* Switch back to manufacturer selection */
            if(button == BUTTON_PRESSED_LONGER)
            {
                menu.item.max = solar_panels_get_manufacturers_count();
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
    char buffer[21];
    int i;

    for(i = menu.item.scroll;
        i < menu.item.max && i != menu.item.scroll + 4;
        ++i)
    {
        const char* selection;
        const char* item = NULL;

        /* set selection string */
        if(i == menu.item.selected)
            selection = "> ";
        else
            selection = "  ";

        /* get item to append */
        switch(menu.state)
        {
            case STATE_INIT :
                break;

            case STATE_NAVIGATE_MANUFACTURERS :
                item = solar_panels_get_manufacturer_name(i);
                break;

            case STATE_NAVIGATE_PANELS :
                item = solar_panels_get_model_name(menu.manufacturer.selected, i);
                break;
        }

        /* concatenate and write to LCD */
        snprintf(buffer, 20, "%s%s", selection, item);
        lcd_writeline(i - menu.item.scroll, buffer);
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

std::vector<std::pair<std::string, std::vector<std::string> > > manufacturers;

int solar_panels_get_manufacturers_count_test() {
    return manufacturers.size();
}
int solar_panels_get_panel_count_test(int manufacturer) {
    return manufacturers[manufacturer].second.size();
}
const char* solar_panels_get_manufacturer_name_test(int manufacturer) {
    return manufacturers[manufacturer].first.c_str();
}
const char* solar_panels_get_model_name_test(int manufacturer, int panel) {
    return manufacturers[manufacturer].second[panel].c_str();
}
void lcd_writeline_test(int line, const char* str)
{
}

/* -------------------------------------------------------------------------- */
class oled_menu : public Test
{
    virtual void SetUp()
    {
        // create 5 manufacturers with 5 panels each
        char buf[sizeof(int)*8+1];
        for(int i = 1; i <= 5; ++i)
        {
            std::vector<std::string> panels;
            for(int j = 1; j <= 5; ++j)
            {
                sprintf(buf, "%d", j);
                panels.push_back(std::string("Panel ") + buf);
            }

            sprintf(buf, "%d", i);
            manufacturers.push_back(
                std::make_pair<std::string, std::vector<std::string> >(
                    std::string("Manufacturer ") + buf,
                    panels
                )
            );
        }

        // add manufacturer with no panels
        manufacturers.push_back(std::make_pair<std::string, std::vector<std::string> >(
            "No panels", std::vector<std::string>()
        ));

        event_deinit();
        menu_init();
    }

    virtual void TearDown()
    {
        manufacturers.clear();
    }
};

/* -------------------------------------------------------------------------- */
TEST_F(oled_menu, twisting_right_with_no_items_does_nothing)
{
    menu.item.selected = -1;
    menu.item.max = 0;
    menu.item.scroll = 0;

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(-1));
    EXPECT_THAT(menu.item.max, Eq(0));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, twisting_left_with_no_items_does_nothing)
{
    menu.item.selected = -1;
    menu.item.max = 0;
    menu.item.scroll = 0;

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(-1));
    EXPECT_THAT(menu.item.max, Eq(0));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_right_wraps_correctly)
{
    menu.item.selected = 0;
    menu.item.max = 5;
    menu.item.scroll = 0;

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(1));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(2));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(3));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(4));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));

    on_button(BUTTON_TWISTED_RIGHT);
    EXPECT_THAT(menu.item.selected, Eq(0));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_left_wraps_correctly)
{
    menu.item.selected = 0;
    menu.item.max = 5;
    menu.item.scroll = 0;

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(4));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(3));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(2));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(1));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));

    on_button(BUTTON_TWISTED_LEFT);
    EXPECT_THAT(menu.item.selected, Eq(0));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, dont_go_into_submenu_with_no_items)
{
    on_button(BUTTON_TWISTED_LEFT); // selects last item

    on_button(BUTTON_PRESSED);
    on_button(BUTTON_RELEASED);

    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

#endif /* TESTING */
