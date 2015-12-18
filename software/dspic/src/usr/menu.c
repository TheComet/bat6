/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include "usr/menu.h"
#include "usr/panels_db.h"
#include "drv/lcd.h"
#include "core/event.h"

#include <stddef.h>
#include <string.h>
#include <stdarg.h>

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

static void cat_strings(char* dest, short dest_n, short src_n, ...);
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

    handle_menu_switches(-1);
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
        if(menu.item.selected != 0)
            --menu.item.selected;

        if(menu.item.selected < menu.item.scroll)
            --menu.item.scroll;
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        if(menu.item.selected != menu.item.max - 1)
            ++menu.item.selected;

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
            /* set selection */
            menu.item.max = solar_panels_get_manufacturers_count();
            reset_selection();

            /* write menu title to LCD */
            lcd_writeline(0, "[Manufacturers]");

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

                {   char buffer[21];
                    const char* menu_title = solar_panels_get_manufacturer_name(
                            menu.manufacturer.selected);
                    cat_strings(buffer, 21, 3, "[", menu_title, "]");
                    lcd_writeline(0, buffer);
                }

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
static void cat_strings(char* dest, short dest_n, short src_n, ...)
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
static void menu_update(void)
{
    char buffer[21];
    int i;

    for(i = 0; i != 3; ++i)
    {
        short current_item = i + menu.item.scroll;
        const char* selection;
        const char* item = NULL;

        /* set selection string */
        if(current_item == menu.item.selected)
            selection = "> ";
        else
            selection = "  ";

        /* get item to append */
        if(current_item < menu.item.max)
        {
            switch(menu.state)
            {
                case STATE_INIT :
                    break;

                case STATE_NAVIGATE_MANUFACTURERS :
                    item = solar_panels_get_manufacturer_name(current_item);
                    break;

                case STATE_NAVIGATE_PANELS :
                    item = solar_panels_get_model_name(
                            menu.manufacturer.selected, current_item);
                    break;
            }
        } else {
            item = "";
        }

        /* concatenate and write to LCD */
        cat_strings(buffer, 20, 2, selection, item);
        lcd_writeline(i + 1, buffer);
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
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

std::vector<std::pair<std::string, std::vector<std::string> > > manufacturers;
std::string writeline;

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
    char buf[sizeof(int)*8+1];
    sprintf(buf, "%d", line);
    writeline.append(std::string(buf) + ": " + str + "\n");
}

static void twist_button_left()
{
    on_button(BUTTON_TWISTED_LEFT);
}

static void twist_button_right()
{
    on_button(BUTTON_TWISTED_RIGHT);
}

static void press_button()
{
    on_button(BUTTON_PRESSED);
    on_button(BUTTON_RELEASED);
}

static void press_button_longer()
{
    on_button(BUTTON_PRESSED_LONGER);
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
        writeline.clear();
    }
};

/* -------------------------------------------------------------------------- */
TEST_F(oled_menu, cat_strings)
{
    char buffer[10];
    cat_strings(buffer, 10, 4, "This ", "is ", "a ", "test");
    EXPECT_THAT(buffer, StrEq("This is a")); /* 9 characters */
}

TEST_F(oled_menu, twisting_right_with_no_items_does_nothing)
{
    menu.item.selected = -1;
    menu.item.max = 0;
    menu.item.scroll = 0;

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(-1));
    EXPECT_THAT(menu.item.max, Eq(0));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, twisting_left_with_no_items_does_nothing)
{
    menu.item.selected = -1;
    menu.item.max = 0;
    menu.item.scroll = 0;

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(-1));
    EXPECT_THAT(menu.item.max, Eq(0));
    EXPECT_THAT(menu.item.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_right_clamps_correctly)
{
    menu.item.selected = 0;
    menu.item.max = 5;
    menu.item.scroll = 0;

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(1));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(2));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(3));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(4));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.item.selected, Eq(4));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, item_selection_left_clamps_correctly)
{
    menu.item.selected = 4;
    menu.item.max = 5;
    menu.item.scroll = 1;

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(3));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(2));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(1));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(0));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.item.selected, Eq(0));
    EXPECT_THAT(menu.item.max, Eq(5));
    EXPECT_THAT(menu.item.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, dont_go_into_submenu_with_no_items)
{
    menu.item.selected = menu.item.max - 1; // selects last item (which has a submenu with no items)
    press_button();

    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_into_a_submenu)
{
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANELS));
}

TEST_F(oled_menu, go_into_a_supermenu)
{
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANELS));
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, menu_update_is_called_during_init)
{
    EXPECT_THAT(writeline, StrEq("0: [Manufacturers]\n1: > Manufacturer 1\n2:   Manufacturer 2\n3:   Manufacturer 3\n"));
}

#endif /* TESTING */
