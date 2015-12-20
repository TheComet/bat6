/*!
 * @file menu.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 *
 * Entire menu structure is listed here.
 * + Manufacturer Menu
 *   - Manufacturer 1                    STATE_NAVIGATE_MANUFACTURERS
 *   - Manufacturer 2                    ...
 *   + Manufacturer 3                    ...
 *     - Panel 1                         STATE_NAVIGATE_PANELS
 *     - Panel 2                         ...
 *     + Panel 3                         ...
 *       + Irradiation                   STATE_NAVIGATE_GLOBAL_PARAMETERS
 *         - 100%                        STATE_CONTROL_GLOBAL_IRRADIATION
 *       + Temperature                   STATE_NAVIGATE_GLOBAL_PARAMETERS
 *         - 20.0°                       STATE_CONTROL_GLOBAL_TEMPERATURE
 *       + Individual Cells              STATE_NAVIGATE_GLOBAL_PARAMETERS
 *         - Go back                     STATE_NAVIGATE_PANEL_CELLS
 *         - Cell 1                      ...
 *         - Cell 2                      ...
 *         + Cell 3                      ...
 *           + Irradiation               STATE_NAVIGATE_CELL_PARAMETERS
 *             - 100%                    STATE_CONTROL_CELL_IRRADIATION
 *           + Temperature               STATE_NAVIGATE_CELL_PARAMETERS
 *             - 20.0°                   STATE_CONTROL_CELL_TEMPERATURE
 *           - Global Parameters         STATE_NAVIGATE_CELL_PARAMETERS
 *
 * STATE_NAVIGATE_MANUFACTURERS lists all manufacturers. Selecting an item in
 * this menu brings you to the manufacturer's panel selection menu and
 * switches state to STATE_NAVIGATE_PANELS.
 *   [Manufacturers]
 * > Manufacturer 1
 *   Manufacturer 2
 *   Manufacturer 3
 *
 * STATE_NAVIGATE_PANELS lists all panels from a selected manufacturer.
 * Selecting an item in this menu activates the panel's model and switches
 * state to STATE_CONTROL_GLOBAL_IRRADIATION.
 *   [Manufacturer Name]
 * > Panel 1
 *   Panel 2
 *   Panel 3
 *
 * STATE_NAVIGATE_GLOBAL_PARAMETERS lists the global parameters of the model
 * that can be changed.
 *   14.3V 1.0A 14.3W
 * > Irradiation             Selecting this -> STATE_CONTROL_GLOBAL_IRRADIATION
 *   Temperature             Selecting this -> STATE_CONTROL_GLOBAL_TEMPERATURE
 *   Individual Cells        Selecting this -> STATE_NAVIGATE_PANEL_CELLS
 *
 * STATE_CONTROL_GLOBAL_IRRADIATION modifies the irradiation of all cells
 * equally.
 * STATE_CONTROL_GLOBAL_TEMPERATURE modifies the temperature of all cells
 * equally.
 *
 * STATE_NAVIGATE_PANEL_CELLS lists the model's cells for selection. Selecting
 * a cell switches state to STATE_NAVIGATE_CELL_PARAMETERS.
 *   14.3V 1.0A 14.3W
 * > Go back                 Selecting this -> STATE_NAVIGATE_GLOBAL_PARAMETERS
 *   Cell 1                  Selecting this -> STATE_NAVIGATE_CELL_PARAMETERS
 *   Cell 2
 *
 * STATE_NAVIGATE_CELL_PARAMETERS lists the selected cell's parameters that
 * can be changed.
 *   14.3V 1.0A 14.3W
 * > Irradiation             Selecting this -> STATE_CONTROL_CELL_IRRADIATION
 *   Temperature             Selecting this -> STATE_CONTROL_CELL_TEMPERATURE
 *   Go back                 Selecting this -> STATE_NAVIGATE_PANEL_CELLS
 *
 * STATE_CONTROL_CELL_IRRADIATION
 * STATE_CONTROL_CELL_TEMPERATURE
 */

#include "usr/menu.h"
#include "usr/panels_db.h"
#include "usr/pv_model.h"
#include "drv/lcd.h"
#include "drv/buck.h"
#include "core/event.h"
#include "core/string.h"

#include <stddef.h>

/*!
 *
 */
typedef enum menu_state_e
{
    STATE_NAVIGATE_MANUFACTURERS,
    STATE_NAVIGATE_PANELS,
    STATE_NAVIGATE_GLOBAL_PARAMETERS,
    STATE_CONTROL_GLOBAL_IRRADIATION,
    STATE_CONTROL_GLOBAL_TEMPERATURE,
    STATE_NAVIGATE_PANEL_CELLS,
    STATE_NAVIGATE_CELL_PARAMETERS,
    STATE_CONTROL_CELL_IRRADIATION,
    STATE_CONTROL_CELL_TEMPERATURE
} menu_state_e;

struct menu_navigation_t
{
    short max;
    short scroll;
    short item;
    union
    {
        short manufacturer;
    } selected;
};

struct menu_t
{
    menu_state_e state;
    struct menu_navigation_t navigation;
};

struct menu_t menu;

static void load_menu_manufacturers(void);
static void handle_menu_switches(unsigned int button);
static void menu_update(void);
static void refresh_measurements(void);
static void on_button(unsigned int button);
static void on_update(unsigned int arg);

/*
 * Override some of the external functions used by the menu for unit testing
 * purposes
 */
#ifdef TESTING
int panels_db_get_manufacturers_count_test();
int panels_db_get_panel_count_test(int manufacturer);
const char* panels_db_get_manufacturer_name_test(int manufacturer);
const char* panels_db_get_model_name_test(int manufacturer, int panel);
void lcd_writeline_test(int line, const char* str);
#   define panels_db_get_manufacturers_count       \
           panels_db_get_manufacturers_count_test
#   define panels_db_get_panel_count               \
           panels_db_get_panel_count_test
#   define panels_db_get_manufacturer_name         \
           panels_db_get_manufacturer_name_test
#   define panels_db_get_model_name                \
           panels_db_get_model_name_test
#   define lcd_writeline                           \
           lcd_writeline_test
#endif

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    menu.navigation.item = -1;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;
    menu.navigation.selected.manufacturer = -1;

    load_menu_manufacturers();
    menu_update();

    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void handle_item_selection(unsigned int button)
{
    if(menu.navigation.item == -1)
        return;

    if(button == BUTTON_TWISTED_LEFT)
    {
        if(menu.navigation.item != 0)
            --menu.navigation.item;

        if(menu.navigation.item < menu.navigation.scroll)
            --menu.navigation.scroll;

        return;
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        if(menu.navigation.item != menu.navigation.max - 1)
            ++menu.navigation.item;

        if(menu.navigation.item - menu.navigation.scroll >= 4)
            ++menu.navigation.scroll;
    }
}

/* -------------------------------------------------------------------------- */
#define reset_selection()                                        \
        menu.navigation.item = (menu.navigation.max ? 0 : -1);   \
        menu.navigation.scroll = 0

static void load_menu_manufacturers(void)
{
    /* set selection */
    menu.navigation.max = panels_db_get_manufacturers_count();
    reset_selection();

    /* write menu title to LCD */
    lcd_writeline(0, "[Manufacturers]");

    menu.state = STATE_NAVIGATE_MANUFACTURERS;
}

/* -------------------------------------------------------------------------- */
static void handle_menu_switches(unsigned int button)
{
    /* go back to manufacturers menu at any point */
    if(button == BUTTON_PRESSED_LONGER)
    {
        event_unregister_listener(EVENT_UPDATE, on_update);
        load_menu_manufacturers();
        return;
    }

    switch(menu.state)
    {
        case STATE_NAVIGATE_MANUFACTURERS :

            /* Switch to panel selection of the current manufacturer */
            if(button == BUTTON_RELEASED && menu.navigation.item != -1)
            {
                /* abort if there are no panels */
                short panels = panels_db_get_panel_count(menu.navigation.item);
                if(!panels)
                    break;

                /* select current item as selected manufacturer */
                menu.navigation.selected.manufacturer = menu.navigation.item;
                menu.navigation.max = panels;
                reset_selection();

                /* write menu title to LCD */
                {   char buffer[21];
                    const char* menu_title = panels_db_get_manufacturer_name(
                            menu.navigation.selected.manufacturer);
                    str_nstrcat(buffer, 21, 3, "[", menu_title, "]");
                    lcd_writeline(0, buffer);
                }

                menu.state = STATE_NAVIGATE_PANELS;
            }

            break;

        case STATE_NAVIGATE_PANELS :

            /* activate model */
            if(button == BUTTON_RELEASED)
            {
                short i;

                /* disable buck for this process */
                buck_disable();

                /* clear existing cells */
                model_cell_remove_all();

                /*
                 * Copy parameters for each cell in the db and add them to a
                 * new cell in the active model
                 */
                for(i = 0;
                    i != panels_db_get_cell_count(
                            menu.navigation.selected.manufacturer,
                            menu.navigation.item);
                    ++i)
                {
                    const struct pv_cell_t* cell = panels_db_get_cell(
                            menu.navigation.selected.manufacturer,
                            menu.navigation.item,
                            i);
                    unsigned char cell_id = model_cell_add();
                    model_set_open_circuit_voltage(cell_id, cell->voc);
                    model_set_short_circuit_current(cell_id, cell->isc);
                    model_set_thermal_voltage(cell_id, cell->vt);
                    model_set_relative_solar_irridation(cell_id, cell->g);
                }

                /* buck can now be enabled */
                buck_enable();

                /* set up real time measurements */
                event_register_listener(EVENT_UPDATE, on_update);
                refresh_measurements();

                menu.state = STATE_CONTROL_GLOBAL_IRRADIATION;
            }

            /* Switch back to manufacturer selection */
            if(button == BUTTON_PRESSED_LONGER)
            {
                menu.navigation.max = panels_db_get_manufacturers_count();
                reset_selection();
                menu.state = STATE_NAVIGATE_MANUFACTURERS;
            }

            break;

        case STATE_NAVIGATE_GLOBAL_PARAMETERS:
            break;

        case STATE_CONTROL_GLOBAL_IRRADIATION:
            break;

        case STATE_CONTROL_GLOBAL_TEMPERATURE:
            break;

        case STATE_NAVIGATE_PANEL_CELLS:
            break;

        case STATE_CONTROL_CELL_IRRADIATION:
            break;

        case STATE_CONTROL_CELL_TEMPERATURE:
            break;

        default: break;
    }
}

/* -------------------------------------------------------------------------- */
static void menu_update(void)
{
    char buffer[21];
    int i;

    for(i = 0; i != 3; ++i)
    {
        short current_item = i + menu.navigation.scroll;
        const char* selection;
        const char* item = NULL;

        /* set selection string */
        if(current_item == menu.navigation.item)
            selection = "> ";
        else
            selection = "  ";

        /* get item string */
        switch(menu.state)
        {
            case STATE_NAVIGATE_MANUFACTURERS :
                item = panels_db_get_manufacturer_name(current_item);
                break;

            case STATE_NAVIGATE_PANELS :
                item = panels_db_get_model_name(
                        menu.navigation.selected.manufacturer, current_item);
                break;

            default:
                break;
        }

        /* was item found? */
        if(item == NULL)
            continue;

        /* concatenate and write to LCD */
        str_nstrcat(buffer, 20, 2, selection, item);
        lcd_writeline(i + 1, buffer);
    }
}

/* -------------------------------------------------------------------------- */
static void refresh_measurements(void)
{
    char line[21], *lineptr;
    char buffer[4]; /* enough for 2 digits */

    /* because we're lazy programmers */
#define append_to_line(lineptr, str) do { \
    const char* ptr = str;                \
    while(*ptr)                           \
        *lineptr++ = *ptr++;    }while(0)

    _Q16 voltage = buck_get_voltage();
    _Q16 current = buck_get_current();
    _Q16 power   = _Q16mpy(voltage, current);

    lineptr = line;

    /* Add voltage to line */
    str_nitoa(buffer, 2, voltage >> 16);
    append_to_line(lineptr, buffer);            /* 3 */
    append_to_line(lineptr, ".");               /* 4 */
    str_nitoa(buffer, 2, voltage & 0xFFFF);
    append_to_line(lineptr, buffer);            /* 6 */
    append_to_line(lineptr, "V ");              /* 8 */

    /* Add current to line */
    str_nitoa(buffer, 1, current >> 16);
    append_to_line(lineptr, buffer);            /* 10 */
    append_to_line(lineptr, ".");               /* 11 */
    str_nitoa(buffer, 2, current & 0xFFFF);
    append_to_line(lineptr, buffer);            /* 13 */
    append_to_line(lineptr, "A ");              /* 15 */

    /* Add power to line */
    str_nitoa(buffer, 2, power >> 16);
    append_to_line(lineptr, buffer);            /* 18 */
    append_to_line(lineptr, "W");               /* 19 */

    *lineptr = '\0';
    lcd_writeline(0, line);
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int button)
{
    handle_item_selection(button);
    handle_menu_switches(button);
    menu_update();
}

/* -------------------------------------------------------------------------- */
static void on_update(unsigned int arg)
{
    static unsigned char counter = 0;
    if(counter-- == 0)
    {
        counter = 50;
        refresh_measurements();
    }
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

std::vector<std::pair<std::string, std::vector<std::string> > > manufacturers;
std::string writeline;

int panels_db_get_manufacturers_count_test() {
    return manufacturers.size();
}
int panels_db_get_panel_count_test(int manufacturer) {
    return manufacturers[manufacturer].second.size();
}
const char* panels_db_get_manufacturer_name_test(int manufacturer) {
    return manufacturers[manufacturer].first.c_str();
}
const char* panels_db_get_model_name_test(int manufacturer, int panel) {
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
TEST_F(oled_menu, output)
{
    refresh_measurements();
    std::cout << writeline << std::endl;
}

TEST_F(oled_menu, twisting_right_with_no_items_does_nothing)
{
    menu.navigation.item = -1;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(-1));
    EXPECT_THAT(menu.navigation.max, Eq(0));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
}

TEST_F(oled_menu, twisting_left_with_no_items_does_nothing)
{
    menu.navigation.item = -1;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(-1));
    EXPECT_THAT(menu.navigation.max, Eq(0));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
}

TEST_F(oled_menu, item_selection_right_clamps_correctly)
{
    menu.navigation.item = 0;
    menu.navigation.max = 5;
    menu.navigation.scroll = 0;

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(1));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(2));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(3));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(4));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(4));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, item_selection_left_clamps_correctly)
{
    menu.navigation.item = 4;
    menu.navigation.max = 5;
    menu.navigation.scroll = 1;

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(3));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(2));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(1));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(1));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(0));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(0));
    EXPECT_THAT(menu.navigation.max, Eq(5));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, menu_update_is_called_during_init)
{
    EXPECT_THAT(writeline, StrEq("0: [Manufacturers]\n1: > Manufacturer 1\n2:   Manufacturer 2\n3:   Manufacturer 3\n"));
}

TEST_F(oled_menu, go_from_manufacturer_menu_into_panel_menu)
{
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANELS));
}

TEST_F(oled_menu, dont_go_into_submenu_with_no_items)
{
    menu.navigation.item = menu.navigation.max - 1; // selects last item (which has a submenu with no items)
    press_button();

    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_back_from_panel_menu_to_manufacturer_menu)
{
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANELS));
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_from_panel_menu_into_controlling_global_irradiation)
{
    press_button();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_IRRADIATION));
}

TEST_F(oled_menu, go_back_from_controlling_global_irradiation_to_manufacturer_menu)
{
    press_button();
    press_button();

    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_from_controlling_global_irradiation_to_selecting_global_parameters)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */

    press_button(); /* should get us to selecting global parameters */
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
}

TEST_F(oled_menu, go_back_from_selecting_global_parameters_to_manufacturer_menu)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */

    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_from_selecting_global_parameters_to_global_irradiation)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */

    press_button(); /* go back to global irradiation */
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_IRRADIATION));
}

TEST_F(oled_menu, go_from_selecting_global_parameters_to_global_temperature)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */

    twist_button_right(); /* select global temperature */
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_TEMPERATURE));
}

TEST_F(oled_menu, go_back_from_global_temperature_to_manufacturer_menu)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    press_button();

    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_back_from_global_temperature_to_global_parameters)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    press_button();

    press_button(); /* should go back to global parameters */
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
}

TEST_F(oled_menu, go_from_global_parameters_to_cell_selection)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */

    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANEL_CELLS));
}

TEST_F(oled_menu, go_back_from_cell_selection_to_manufacturer_menu)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */
    press_button();       /* go into cell selection */

    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_back_from_cell_selection_to_global_parameter_selection)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */
    press_button();       /* go into cell selection */

    press_button(); /* top item in menu should be "go back" */
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
}

TEST_F(oled_menu, go_from_cell_selection_into_cell_parameter_selection)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */
    press_button();       /* go into cell selection */
    twist_button_right(); /* select cell 1 */

    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_CELL_PARAMETERS));
}

TEST_F(oled_menu, go_back_from_cell_parameter_selection_to_manufacturer_menu)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */
    press_button();       /* go into cell selection */
    twist_button_right(); /* select cell 1 */
    press_button();       /* go into cell parameters */

    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
}

TEST_F(oled_menu, go_back_from_cell_parameter_selection_to_panel_call_selection)
{
    press_button(); /* gets us to panels */
    press_button(); /* gets us to global irradiation */
    press_button(); /* gets us to global parameters */
    twist_button_right(); /* select global temperature */
    twist_button_right(); /* select individual cells */
    press_button();       /* go into cell selection */
    twist_button_right(); /* select cell 1 */
    press_button();       /* go into cell parameters */

    twist_button_right(); /* select temperature */
    twist_button_right(); /* select "go back" */
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANEL_CELLS));
}

TEST_F(oled_menu, )
{

}

#endif /* TESTING */
