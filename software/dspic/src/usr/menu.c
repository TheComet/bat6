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
 *           + Irradiation               STATE_NAVIGATE_Q16_PARAMETERS
 *             - 100%                    STATE_CONTROL_CELL_IRRADIATION
 *           + Temperature               STATE_NAVIGATE_Q16_PARAMETERS
 *             - 20.0°                   STATE_CONTROL_CELL_TEMPERATURE
 *           - Global Parameters         STATE_NAVIGATE_Q16_PARAMETERS
 *
 * STATE_NAVIGATE_MANUFACTURERS lists all manufacturers. Selecting an item in
 * this menu brings you to the manufacturer's panel selection menu and
 * switches state to STATE_NAVIGATE_PANELS.
 *   -Manufacturers-
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
 * a cell switches state to STATE_NAVIGATE_Q16_PARAMETERS.
 *   14.3V 1.0A 14.3W
 * > Go back                 Selecting this -> STATE_NAVIGATE_GLOBAL_PARAMETERS
 *   Cell 1                  Selecting this -> STATE_NAVIGATE_Q16_PARAMETERS
 *   Cell 2
 *
 * STATE_NAVIGATE_Q16_PARAMETERS lists the selected cell's parameters that
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
#include "drv/hw.h"
#include "drv/buck.h"
#include "drv/leds.h"
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
    STATE_NAVIGATE_PANEL_CELLS,
    STATE_NAVIGATE_CELL_PARAMETERS,
    STATE_CONTROL_GLOBAL_IRRADIATION,
    STATE_CONTROL_GLOBAL_TEMPERATURE,
    STATE_CONTROL_CELL_IRRADIATION,
    STATE_CONTROL_CELL_TEMPERATURE
} menu_state_e;

typedef enum menu_item_e
{
    /* global parameter menu item indices */
    ITEM_IRRADIATION = 0,
    ITEM_TEMPERATURE = 1,
    ITEM_INDIVIDUAL_CELLS = 2,
    /* cell parameter menu item indices */
    /* same as above, except for last item */
    ITEM_GO_BACK = 2
} menu_item_e;

struct menu_navigation_t
{
    short max;
    short scroll;
    short item;
};

struct menu_cell_t
{
    unsigned char active_id;
    unsigned char count;
};

struct menu_t
{
    menu_state_e state;
    struct menu_navigation_t navigation;
    union {
        struct menu_cell_t cell;
        short manufacturer;
    };
};

struct menu_t menu;

static void load_menu_navigate_manufacturers(void);
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
const char* panels_db_get_panel_name_test(int manufacturer, int panel);
void lcd_writeline_test(int line, const char* str);
int panels_db_get_cell_count_test(int manufacturer, int panel);
const struct pv_cell_t* panels_db_get_cell_test(int manufacturer, int panel, int cell);
unsigned char model_cell_add_test(void);
unsigned char model_cell_begin_iteration_test();
unsigned char model_cell_get_next_test();
void model_set_open_circuit_voltage_test(unsigned char cell_id, _Q16 value);
void model_set_short_circuit_current_test(unsigned char cell_id, _Q16 value);
void model_set_thermal_voltage_test(unsigned char cell_id, _Q16 value);
void model_set_relative_solar_irradiation_test(unsigned char cell_id, _Q16 value);
_Q16 model_get_open_circuit_voltage_test(unsigned char cell_id);
_Q16 model_get_short_circuit_current_test(unsigned char cell_id);
_Q16 model_get_thermal_voltage_test(unsigned char cell_id);
_Q16 model_get_relative_solar_irradiation_test(unsigned char cell_id);
_Q16 buck_get_voltage_test();
_Q16 buck_get_current_test();
#   define panels_db_get_manufacturers_count       \
           panels_db_get_manufacturers_count_test
#   define panels_db_get_panel_count               \
           panels_db_get_panel_count_test
#   define panels_db_get_manufacturer_name         \
           panels_db_get_manufacturer_name_test
#   define panels_db_get_panel_name                \
           panels_db_get_panel_name_test
#   define panels_db_get_cell_count                \
           panels_db_get_cell_count_test
#   define panels_db_get_cell                      \
           panels_db_get_cell_test
#   define model_cell_begin_iteration              \
           model_cell_begin_iteration_test
#   define model_cell_get_next                     \
           model_cell_get_next_test
#   define lcd_writeline                           \
           lcd_writeline_test
#   define model_cell_add                          \
           model_cell_add_test
#   define model_set_open_circuit_voltage          \
           model_set_open_circuit_voltage_test
#   define model_set_short_circuit_current         \
           model_set_short_circuit_current_test
#   define model_set_thermal_voltage               \
           model_set_thermal_voltage_test
#   define model_set_relative_solar_irradiation    \
           model_set_relative_solar_irradiation_test
#   define model_get_open_circuit_voltage          \
           model_get_open_circuit_voltage_test
#   define model_get_short_circuit_current         \
           model_get_short_circuit_current_test
#   define model_get_thermal_voltage               \
           model_get_thermal_voltage_test
#   define model_get_relative_solar_irradiation    \
           model_get_relative_solar_irradiation_test
#   define buck_get_voltage                        \
           buck_get_voltage_test
#   define buck_get_current                        \
           buck_get_current_test
#endif

/* -------------------------------------------------------------------------- */
void menu_init(void)
{
    menu.navigation.item = 0;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;
    menu.manufacturer = 0;

    lcd_reset();
    load_menu_navigate_manufacturers();
    menu_update();

    event_register_listener(EVENT_BUTTON, on_button);
}

/* -------------------------------------------------------------------------- */
static void handle_item_selection(unsigned int button)
{
    /* no items to select? */
    if(menu.navigation.max == 0)
        return;

    if(button == BUTTON_TWISTED_LEFT)
    {
        /* clamp selection at item 0 */
        if(menu.navigation.item != 0)
            --menu.navigation.item;

        /* scroll menu if we selected an item less than the scroll value */
        if(menu.navigation.item < menu.navigation.scroll)
            --menu.navigation.scroll;
        
        menu_update();
        return;
    }

    if(button == BUTTON_TWISTED_RIGHT)
    {
        /* clamp selection at max item - 1*/
        if(menu.navigation.item != menu.navigation.max - 1)
            ++menu.navigation.item;

        /* scroll menu if we selected the third item relative to the scroll value */
        if(menu.navigation.item - menu.navigation.scroll >= 3)
            ++menu.navigation.scroll;
        
        menu_update();
    }
}

/* -------------------------------------------------------------------------- */
static void load_menu_navigate_manufacturers(void)
{
    /* set selection */
    menu.navigation.max = panels_db_get_manufacturer_count();
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    /* write menu title to LCD */
    lcd_writeline(0, "-Manufacturers-");

    menu.state = STATE_NAVIGATE_MANUFACTURERS;
}

static void load_menu_navigate_panels(void)
{
    /* abort if there are no panels */
    short panels = panels_db_get_panel_count(menu.navigation.item);
    if(!panels)
        return;

    /* select current item as selected manufacturer */
    menu.manufacturer = menu.navigation.item;
    menu.navigation.max = panels;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    /* write menu title to LCD */
    {   char buffer[21];
        const char* menu_title = panels_db_get_manufacturer_name(
                menu.manufacturer);
        str_nstrcat(buffer, 21, 3, "-", menu_title, "-");
        lcd_writeline(0, buffer);
    }

    menu.state = STATE_NAVIGATE_PANELS;
}

static void activate_selected_panel()
{
    unsigned char cell_count, i;

    /* disable buck for this process */
    buck_disable();

    /* clear existing cells */
    model_cell_remove_all();

    /* store cell count in menu state, as it's required by submenus */
    cell_count = panels_db_get_cell_count(menu.manufacturer,
                                          menu.navigation.item);
    
    /* reset global parameters */
    model_set_global_thermal_voltage((_Q16)(293 * 65536));
    model_set_global_relative_solar_irradiation((_Q16)(100 * 65536));

    /*
     * Copy parameters for each cell in the db and add them to a
     * new cell in the active model
     */
    for(i = 0; i != cell_count; ++i)
    {
        const struct pv_cell_t* cell = panels_db_get_cell(
                menu.manufacturer,
                menu.navigation.item,
                i);
        unsigned char cell_id = model_cell_add();
        model_set_open_circuit_voltage(cell_id, cell->voc);
        model_set_short_circuit_current(cell_id, cell->isc);
        model_set_thermal_voltage(cell_id, cell->vt);
        model_set_relative_solar_irradiation(cell_id, cell->g);
    }

    /* buck can now be enabled */
    buck_enable();
    
    /* 
     * Set up cell count and active ID. Warning: This overwrites the
     * "manufacturers" field in the struct.
     */
    menu.cell.count = cell_count;
    menu.cell.active_id = 0;
}

static void load_menu_navigate_global_parameters(void)
{
    /* when navigating the global parameters, select an invalid cell ID */
    menu.cell.active_id = 0;

    menu.navigation.max = 3;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    menu.state = STATE_NAVIGATE_GLOBAL_PARAMETERS;
}

static void load_menu_control_global_irradiation(void)
{
    load_menu_navigate_global_parameters();
    menu.navigation.max = 0;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    menu.state = STATE_CONTROL_GLOBAL_IRRADIATION;
}

static void load_menu_control_global_temperature(void)
{
    load_menu_navigate_global_parameters();
    menu.navigation.max = 0;
    menu.navigation.item = 1;
    menu.navigation.scroll = 0;

    menu.state = STATE_CONTROL_GLOBAL_TEMPERATURE;
}

static void load_menu_navigate_panel_cells(void)
{
    /* There is an additional "Go Back" option in this menu */
    menu.navigation.max = menu.cell.count + 1;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    menu.state = STATE_NAVIGATE_PANEL_CELLS;
}

static void select_active_cell_from_active_menu(void)
{
    unsigned char i = 0;

    /* Get the cell ID of the selected cell. */
    for(menu.cell.active_id = model_cell_begin_iteration();
        menu.cell.active_id != 0;
        menu.cell.active_id = model_cell_get_next(), i++)
    {
        if(i == menu.navigation.item - 1)
            break;
    }
}

static void load_menu_navigate_cell_parameters(void)
{
    /* set up parameter menu. Options are "irradiation", "temperature", and
     * "go back" */
    menu.navigation.max = 3;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    menu.state = STATE_NAVIGATE_CELL_PARAMETERS;
}

static void load_menu_control_cell_irradiation(void)
{
    menu.navigation.max = 0;
    menu.navigation.item = 0;
    menu.navigation.scroll = 0;

    menu.state = STATE_CONTROL_CELL_IRRADIATION;
}

static void load_menu_control_cell_temperature(void)
{
    menu.navigation.max = 0;     /* restrict scrolling around */
    menu.navigation.item = 1;    /* keep "temperature" selected */
    menu.navigation.scroll = 0;
    menu.state = STATE_CONTROL_CELL_TEMPERATURE;
}

/* -------------------------------------------------------------------------- */
static void handle_menu_switches(unsigned int button)
{
    /* go back to manufacturers menu at any point */
    if(button == BUTTON_PRESSED_LONGER)
    {
        event_unregister_listener(EVENT_UPDATE, on_update);
        load_menu_navigate_manufacturers();
        menu_update();
        return;
    }

    /* menu switches only occur on button released */
    if(button != BUTTON_RELEASED)
        return;

    switch(menu.state)
    {
        case STATE_NAVIGATE_MANUFACTURERS :
            load_menu_navigate_panels();
            menu_update();
            break;

        case STATE_NAVIGATE_PANELS :
            /* Loads a new model from the db and sets it as the active model. */
            activate_selected_panel();

            /*
             * Jump straight into controlling the global irradiation, because
             * that is the most common used.
             */
            load_menu_control_global_irradiation();

            /* set up display of real time measurements */
            event_register_listener(EVENT_UPDATE, on_update);
            refresh_measurements();
            menu_update();

            break;

        case STATE_NAVIGATE_GLOBAL_PARAMETERS:
            if(menu.navigation.item == ITEM_IRRADIATION)
            {
                load_menu_control_global_irradiation();
                menu_update();
                break;
            }
            if(menu.navigation.item == ITEM_TEMPERATURE)
            {
                load_menu_control_global_temperature();
                menu_update();
                break;
            }
            if(menu.navigation.item == ITEM_INDIVIDUAL_CELLS)
            {
                load_menu_navigate_panel_cells();
                menu_update();
                break;
            }

            break;

        case STATE_CONTROL_GLOBAL_IRRADIATION:
            load_menu_navigate_global_parameters();
            menu_update();
            break;

        case STATE_CONTROL_GLOBAL_TEMPERATURE:
            load_menu_navigate_global_parameters();
            menu.navigation.item = 1;  /* select item we returned from */
            menu_update();
            break;

        case STATE_NAVIGATE_PANEL_CELLS:
            /* The first item in the list of cells is "Go Back" */
            if(menu.navigation.item == 0)
                load_menu_navigate_global_parameters();
            /* Every other item is a cell */
            else {
                select_active_cell_from_active_menu();
                load_menu_navigate_cell_parameters();
            }
            menu_update();
            break;

        case STATE_NAVIGATE_CELL_PARAMETERS:
            if(menu.navigation.item == ITEM_IRRADIATION)
            {
                load_menu_control_cell_irradiation();
                menu_update();
                break;
            }
            if(menu.navigation.item == ITEM_TEMPERATURE)
            {
                load_menu_control_cell_temperature();
                menu_update();
                break;
            }
            if(menu.navigation.item == ITEM_GO_BACK)
            {
                load_menu_navigate_panel_cells();
                menu_update();
                break;
            }

            break;

        case STATE_CONTROL_CELL_IRRADIATION:
            load_menu_navigate_cell_parameters();
            menu_update();
            break;

        case STATE_CONTROL_CELL_TEMPERATURE:
            load_menu_navigate_cell_parameters();
            menu.navigation.item = 1; /* select item we returned from */
            menu_update();
            break;

        default: break;
    }
}

/* -------------------------------------------------------------------------- */
static _Q16 modify_relative_solar_irradiation(unsigned int button, _Q16 param)
{
    if(button == BUTTON_TWISTED_LEFT)
        param += (_Q16)(65536);
    else if(button == BUTTON_TWISTED_RIGHT)
        param -= (_Q16)(65536);

    if(param > (_Q16)(100 * 65536))
        param = (_Q16)(100 * 65536);
    if(param < 0)
        param = 0;
    
    return param;
}

/* -------------------------------------------------------------------------- */
static _Q16 modify_thermal_voltage(unsigned int button, _Q16 param)
{
    if(button == BUTTON_TWISTED_LEFT)
        param += (_Q16)(32768);
    else if(button == BUTTON_TWISTED_RIGHT)
        param -= (_Q16)(32768);
    
    return param;
}

/* -------------------------------------------------------------------------- */
static void handle_parameter_editing(unsigned int button)
{
    switch(menu.state)
    {
        case STATE_CONTROL_GLOBAL_IRRADIATION:
            model_set_global_relative_solar_irradiation(
                modify_relative_solar_irradiation(button,
                    model_get_global_relative_solar_irradiation()));
            menu_update();
            break;

        case STATE_CONTROL_CELL_IRRADIATION:
            model_set_relative_solar_irradiation(menu.cell.active_id,
                modify_relative_solar_irradiation(button,
                    model_get_relative_solar_irradiation(menu.cell.active_id)));
            menu_update();
            break;

        case STATE_CONTROL_GLOBAL_TEMPERATURE:
            model_set_global_thermal_voltage(
                modify_thermal_voltage(button,
                    model_get_global_thermal_voltage()));
            menu_update();
            break;

        case STATE_CONTROL_CELL_TEMPERATURE:
            model_set_thermal_voltage(menu.cell.active_id,
                modify_thermal_voltage(button,
                    model_get_thermal_voltage(menu.cell.active_id)));
            menu_update();
            break;

        default: break;
    }
}

/* -------------------------------------------------------------------------- */
static void append_temperature_of_selected_cell(char* buffer)
{
    char* ptr = buffer;
    
    /* find insertion point in buffer */
    while(*ptr)
        ++ptr;
    
    /* global parameter */
    if(menu.cell.active_id == 0)
    {
        ptr = str_q16itoa(ptr, 5, model_get_global_thermal_voltage() -
                (_Q16)(273 * 65536));
    } else
    {
        ptr = str_q16itoa(ptr, 5, model_get_thermal_voltage(menu.cell.active_id) - 
            (_Q16)(273 * 65536));
    }

    str_append(ptr, 21 + buffer - ptr, "C");
}

/* -------------------------------------------------------------------------- */
static void append_irradiation_of_selected_cell(char* buffer)
{
    char* ptr = buffer;

    /* find insertion point in buffer */
    while(*ptr)
        ++ptr;
    
    /* Global parameter */
    if(menu.cell.active_id == 0)
    {
        ptr = str_q16itoa(ptr, 4, model_get_global_relative_solar_irradiation());
    } else
    {
        ptr = str_q16itoa(ptr, 4, model_get_relative_solar_irradiation(menu.cell.active_id));
    }

    str_append(ptr, 21 + buffer - ptr, "%");
}

/* -------------------------------------------------------------------------- */
static void menu_update(void)
{
    char buffer[21];
    int i;

    for(i = 0; i != 3; ++i)
    {
        short current_item = i + menu.navigation.scroll;

        /* initialise buffer as empty string */
        buffer[0] = '\0';

        /*
         * Set selection string.
         * If the item is not selected we want two spaces.
         * If the item is selected we want "> ".
         * If the item is selected and being manipulated, we want "= ".
         */
        if(current_item == menu.navigation.item)
            if(menu.state < STATE_CONTROL_GLOBAL_IRRADIATION)
                str_append(buffer, 21, "> ");
            else
                str_append(buffer, 21, "= ");
        else
            str_append(buffer, 21, "  ");

        /* get item string */
        switch(menu.state)
        {
            case STATE_NAVIGATE_MANUFACTURERS : {
                const char* manufacturer;
                if(current_item >= panels_db_get_manufacturer_count())
                    break;
                manufacturer = panels_db_get_manufacturer_name(current_item);
                str_append(buffer, 21, manufacturer);
                break;
            }

            case STATE_NAVIGATE_PANELS : {
                const char* panel;
                if(current_item >= panels_db_get_panel_count(menu.manufacturer))
                    break;
                panel = panels_db_get_panel_name(menu.manufacturer,
                                                 current_item);
                str_append(buffer, 21, panel);
                break;
            }

            case STATE_CONTROL_GLOBAL_IRRADIATION:
            case STATE_CONTROL_GLOBAL_TEMPERATURE:
            case STATE_NAVIGATE_GLOBAL_PARAMETERS:
                if(i == 0)
                {
                    str_append(buffer, 21, "Exposure ");
                    append_irradiation_of_selected_cell(buffer);
                } else if(i == 1)
                {
                    str_append(buffer, 21, "Temp ");
                    append_temperature_of_selected_cell(buffer);
                } else {
                    str_append(buffer, 21, "Individual Cells");
                }

                break;

            case STATE_CONTROL_CELL_IRRADIATION:
            case STATE_CONTROL_CELL_TEMPERATURE:
            case STATE_NAVIGATE_CELL_PARAMETERS:
                if(i == 0)
                {
                    str_append(buffer, 21, "Exposure ");
                    append_irradiation_of_selected_cell(buffer);
                } else if(i == 1)
                {
                    str_append(buffer, 21, "Temp ");
                    append_temperature_of_selected_cell(buffer);
                } else {
                    str_append(buffer, 21, "Go Back");
                }

                break;

            case STATE_NAVIGATE_PANEL_CELLS: {
                if(current_item > menu.cell.count)
                    break;
                if(current_item == 0)
                {
                    str_append(buffer, 21, "Go Back");
                } else
                {
                    char* ptr = str_append(buffer, 21, "Cell ");
                    str_nitoa(ptr, 21 + buffer - ptr, current_item);
                }

                break;
            }

            default:
                break;
        }

        /* write to LCD */
        lcd_writeline(i + 1, buffer);
    }
}

/* -------------------------------------------------------------------------- */
static void refresh_measurements(void)
{
    char line[21], *lineptr;

    _Q16 voltage = buck_get_voltage();
    _Q16 current = buck_get_current();
    _Q16 power   = _Q16mpy(voltage, current);

    lineptr = line;

    /* Add voltage to line */
    lineptr = str_q16itoa(lineptr, 6, voltage); /* 5 */
    lineptr = str_append(lineptr, 3, "V ");     /* 7 */
    /* Add current to line */
    lineptr = str_q16itoa(lineptr, 7, current); /* 13 */
    lineptr = str_append(lineptr, 3, "A ");     /* 15 */
    /* Add power to line */
    lineptr = str_q16itoa(lineptr, 5, power);   /* 19 */
    lineptr = str_append(lineptr, 2, "W");      /* 20 */

    lcd_writeline(0, line);
}

/* -------------------------------------------------------------------------- */
static void on_button(unsigned int button)
{
    handle_item_selection(button);
    handle_menu_switches(button);
    handle_parameter_editing(button);
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

/* -------------------------------------------------------------------------- */
/* set up a DB that we can control for testing */
std::vector<std::pair<std::string, std::vector<std::string> > > manufacturers;
int panels_db_get_manufacturers_count_test() {
    return manufacturers.size();
}
int panels_db_get_panel_count_test(int manufacturer) {
    return manufacturers[manufacturer].second.size();
}
const char* panels_db_get_manufacturer_name_test(int manufacturer) {
    return manufacturers[manufacturer].first.c_str();
}
const char* panels_db_get_panel_name_test(int manufacturer, int panel) {
    return manufacturers[manufacturer].second[panel].c_str();
}
int panels_db_get_cell_count_test(int manufacturer, int panel) {
    return 3;
}

/* -------------------------------------------------------------------------- */
/* set up active model API for testing. We use one default cell. */
#define Q16_PARAM(x) ((_Q16)(x * 65536))
static struct pv_cell_t default_cell = {
    Q16_PARAM(6), Q16_PARAM(2), Q16_PARAM(99.99), Q16_PARAM(100)
};

const struct pv_cell_t* panels_db_get_cell_test(int manufacturer, int panel, int cell) {
    return &default_cell;
}

unsigned char model_cell_add() { return 1; }
unsigned char model_cell_begin_iteration_test() { return 1; }
unsigned char model_cell_get_next_test() { return 0; }
void model_set_open_circuit_voltage_test(unsigned char cell_id, _Q16 value)       { if(cell_id == 1) default_cell.voc = value;  }
void model_set_short_circuit_current_test(unsigned char cell_id, _Q16 value)      { if(cell_id == 1) default_cell.isc = value;  }
void model_set_thermal_voltage_test(unsigned char cell_id, _Q16 value)            { if(cell_id == 1) default_cell.vt  = value;  }
void model_set_relative_solar_irradiation_test(unsigned char cell_id, _Q16 value) { if(cell_id == 1) default_cell.g   = value;  }
_Q16 model_get_open_circuit_voltage(unsigned char cell_id)                        { if(cell_id == 1) return default_cell.voc; else return 0; }
_Q16 model_get_short_circuit_current(unsigned char cell_id)                       { if(cell_id == 1) return default_cell.isc; else return 0; }
_Q16 model_get_thermal_voltage(unsigned char cell_id)                             { if(cell_id == 1) return default_cell.vt;  else return 0; }
_Q16 model_get_relative_solar_irradiation(unsigned char cell_id)                  { if(cell_id == 1) return default_cell.g;   else return 0; }

/* -------------------------------------------------------------------------- */
/* re-implement buck API to return measurement values we can test */
_Q16 buck_get_voltage_test(void) { return Q16_PARAM(-24.5); }
_Q16 buck_get_current_test(void) { return Q16_PARAM(-5.5); }

/* -------------------------------------------------------------------------- */
/* adds all lines written to the LCD to a string instead */
std::string lcd_string;
void lcd_writeline_test(int line, const char* str)
{
    char buf[sizeof(int)*8+1];
    sprintf(buf, "%d", line);
    lcd_string.append(std::string(buf) + ": " + str + "\n");
}

const char* MANUFACTURER_SELECTION_STRING = "\
0: -Manufacturers-\n\
1: > Manufacturer 1\n\
2:   Manufacturer 2\n\
3:   Manufacturer 3\n";
const char* PANEL_SELECTION_STRING = "\
0: -Manufacturer 1-\n\
1: > Panel 1\n\
2:   Panel 2\n\
3:   Panel 3\n";
const char* GLOBAL_PARAMETER_SELECTION_STRING1 = "\
0: -24.5V -5.500A 134W\n\
1: > Exposure 100%\n\
2:   Temp 99.9°C\n\
3:   Individual Cells\n";
const char* GLOBAL_PARAMETER_SELECTION_STRING2 = "\
0: -24.5V -5.500A 134W\n\
1:   Exposure 100%\n\
2: > Temp 99.9°C\n\
3:   Individual Cells\n";
const char* GLOBAL_IRRADIATION_STRING = "\
0: -24.5V -5.500A 134W\n\
1: = Exposure 100%\n\
2:   Temp 99.9°C\n\
3:   Individual Cells\n";
const char* GLOBAL_TEMPERATURE_STRING = "\
0: -24.5V -5.500A 134W\n\
1:   Exposure 100%\n\
2: = Temp 99.9°C\n\
3:   Individual Cells\n";
const char* CELL_SELECTION_STRING = "\
0: -24.5V -5.500A 134W\n\
1: > Go Back\n\
2:   Cell 1\n\
3:   Cell 2\n";
const char* CELL_PARAMETER_SELECTION_STRING1 = "\
0: -24.5V -5.500A 134W\n\
1: > Exposure 100%\n\
2:   Temp 99.9°C\n\
3:   Go Back\n";
const char* CELL_PARAMETER_SELECTION_STRING2 = "\
0: -24.5V -5.500A 134W\n\
1:   Exposure 100%\n\
2: > Temp 99.9°C\n\
3:   Go Back\n";
const char* CELL_IRRADIATION_STRING = "\
0: -24.5V -5.500A 134W\n\
1: = Exposure 100%\n\
2:   Temp 99.9°C\n\
3:   Go Back\n";
const char* CELL_TEMPERATURE_STRING = "\
0: -24.5V -5.500A 134W\n\
1:   Exposure 100%\n\
2: = Temp 99.9°C\n\
3:   Go Back\n";

/* -------------------------------------------------------------------------- */
/* easier button control */
void twist_button_left() { on_button(BUTTON_TWISTED_LEFT); }
void twist_button_right() { on_button(BUTTON_TWISTED_RIGHT); }
void press_button() { on_button(BUTTON_PRESSED); on_button(BUTTON_RELEASED); }
void press_button_longer() { on_button(BUTTON_PRESSED_LONGER); }

/* -------------------------------------------------------------------------- */
/* These functions help navigate the menu more easily */
void navigate_to_panel_selection()
{
    /*
     * -Manufacturers-
     * > Manufacturer 1
     *   Manufacturer 2
     *   Manufacturer 3
     */
    press_button();
}

void navigate_to_global_irradiation()
{
    navigate_to_panel_selection();
    /*
     * [Manufacturer Name]
     * > Panel 1
     *   Panel 2
     *   Panel 3
     */
    press_button();
}

void navigate_to_global_parameter_selection()
{
    navigate_to_global_irradiation();
    /*
     * 14.3V 1.0A 14.3W
     * = Irradiation
     *   Temperature
     *   Individual Cells
     */
    press_button();
}

void navigate_to_global_temperature()
{
    navigate_to_global_parameter_selection();
    twist_button_right();
    /*
     * 14.3V 1.0A 14.3W
     *   Irradiation
     * > Temperature
     *   Individual Cells
     */
    press_button();
}

void navigate_to_cell_selection()
{
    navigate_to_global_parameter_selection();
    twist_button_right();
    twist_button_right();
    /*
     * 14.3V 1.0A 14.3W
     *   Irradiation
     *   Temperature
     * > Individual Cells
     */
    press_button();
}

void navigate_to_cell_parameters()
{
    navigate_to_cell_selection();
    twist_button_right();
    /*
     * 14.3V 1.0A 14.3W
     *   Go back              Selecting this -> STATE_NAVIGATE_GLOBAL_PARAMETERS
     * > Cell 1               Selecting this -> STATE_NAVIGATE_Q16_PARAMETERS
     *   Cell 2
     */
    press_button();
}

void navigate_to_cell_irradiation()
{
    navigate_to_cell_parameters();
    /*
     * 14.3V 1.0A 14.3W
     * > Irradiation            Selecting this -> STATE_CONTROL_CELL_IRRADIATION
     *   Temperature            Selecting this -> STATE_CONTROL_CELL_TEMPERATURE
     *   Go back                Selecting this -> STATE_NAVIGATE_PANEL_CELLS
     */
    press_button();
}

void navigate_to_cell_temperature()
{
    navigate_to_cell_parameters();
    twist_button_right();
    /*
     * 14.3V 1.0A 14.3W
     *   Irradiation            Selecting this -> STATE_CONTROL_CELL_IRRADIATION
     * > Temperature            Selecting this -> STATE_CONTROL_CELL_TEMPERATURE
     *   Go back                Selecting this -> STATE_NAVIGATE_PANEL_CELLS
     */
    press_button();
}

/* -------------------------------------------------------------------------- */
/* Test fixture */
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
        lcd_string.clear();
    }
};

/* -------------------------------------------------------------------------- */
TEST_F(oled_menu, output)
{
    refresh_measurements();
    std::cout << lcd_string << std::endl;
}

/* -------------------------------------------------------------------------- */
/* Tests for menu navigation */
/* -------------------------------------------------------------------------- */
TEST_F(oled_menu, twisting_right_with_no_items_does_nothing)
{
    menu.navigation.item = 0;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;

    twist_button_right();
    EXPECT_THAT(menu.navigation.item, Eq(0));
    EXPECT_THAT(menu.navigation.max, Eq(0));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
}

TEST_F(oled_menu, twisting_left_with_no_items_does_nothing)
{
    menu.navigation.item = 0;
    menu.navigation.max = 0;
    menu.navigation.scroll = 0;

    twist_button_left();
    EXPECT_THAT(menu.navigation.item, Eq(0));
    EXPECT_THAT(menu.navigation.max, Eq(0));
    EXPECT_THAT(menu.navigation.scroll, Eq(0));
}

TEST_F(oled_menu, dont_go_into_submenu_with_no_items)
{
    menu.navigation.item = menu.navigation.max - 1; // selects last item (which has a submenu with no items)
    press_button();

    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
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

/* -------------------------------------------------------------------------- */
/* Tests for state machine integrity */
/* -------------------------------------------------------------------------- */

TEST_F(oled_menu, go_from_manufacturer_menu_into_panel_menu)
{
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    lcd_string.clear();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANELS));
    EXPECT_THAT(lcd_string, StrEq(PANEL_SELECTION_STRING));
}

TEST_F(oled_menu, go_from_panel_menu_into_controlling_global_irradiation)
{
    navigate_to_panel_selection();
    lcd_string.clear();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_IRRADIATION));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_IRRADIATION_STRING));
}

TEST_F(oled_menu, go_from_controlling_global_irradiation_to_selecting_global_parameters)
{
    navigate_to_global_irradiation();
    lcd_string.clear();
    refresh_measurements();
    press_button(); // should get us to selecting global parameters
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_PARAMETER_SELECTION_STRING1));
}

TEST_F(oled_menu, go_from_selecting_global_parameters_to_global_irradiation)
{
    navigate_to_global_parameter_selection();
    lcd_string.clear();
    refresh_measurements();
    press_button(); // go back to global irradiation
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_IRRADIATION));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_IRRADIATION_STRING));
}

TEST_F(oled_menu, go_from_selecting_global_parameters_to_global_temperature)
{
    navigate_to_global_parameter_selection();
    twist_button_right(); // select global temperature
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_GLOBAL_TEMPERATURE));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_TEMPERATURE_STRING));
}

TEST_F(oled_menu, go_back_from_global_temperature_to_global_parameters)
{
    navigate_to_global_temperature();
    lcd_string.clear();
    refresh_measurements();
    press_button(); // should go back to global parameters
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_PARAMETER_SELECTION_STRING2));
}

TEST_F(oled_menu, go_from_global_parameters_to_cell_selection)
{
    navigate_to_global_parameter_selection();
    twist_button_right();
    twist_button_right(); /* select "Individual Cells" */
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANEL_CELLS));
    EXPECT_THAT(lcd_string, StrEq(CELL_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_cell_selection_to_global_parameter_selection)
{
    navigate_to_cell_selection();
    lcd_string.clear();
    refresh_measurements();
    press_button(); // top item in menu should be "go back"
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_GLOBAL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(GLOBAL_PARAMETER_SELECTION_STRING1));
}

TEST_F(oled_menu, go_from_cell_selection_into_cell_parameter_selection)
{
    navigate_to_cell_selection();
    twist_button_right(); // select first cell
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_CELL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(CELL_PARAMETER_SELECTION_STRING1));
}

TEST_F(oled_menu, go_back_from_cell_parameter_selection_to_panel_cell_selection)
{
    navigate_to_cell_parameters();
    twist_button_right(); // select temperature
    twist_button_right(); // select "go back"
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_PANEL_CELLS));
    EXPECT_THAT(lcd_string, StrEq(CELL_SELECTION_STRING));
}

TEST_F(oled_menu, go_from_cell_parameter_selection_to_controlling_cell_irradiation)
{
    navigate_to_cell_parameters();
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_CELL_IRRADIATION));
    EXPECT_THAT(lcd_string, StrEq(CELL_IRRADIATION_STRING));
}

TEST_F(oled_menu, go_back_from_controlling_cell_irradiation_to_cell_parameter_selection)
{
    navigate_to_cell_irradiation();
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_CELL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(CELL_PARAMETER_SELECTION_STRING1));
}

TEST_F(oled_menu, go_from_cell_parameter_selection_to_controlling_cell_temperature)
{
    navigate_to_cell_parameters();
    twist_button_right();
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_CONTROL_CELL_TEMPERATURE));
    EXPECT_THAT(lcd_string, StrEq(CELL_TEMPERATURE_STRING));
}

TEST_F(oled_menu, go_back_from_controlling_cell_temperature_to_cell_parameter_selection)
{
    navigate_to_cell_temperature();
    lcd_string.clear();
    refresh_measurements();
    press_button();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_CELL_PARAMETERS));
    EXPECT_THAT(lcd_string, StrEq(CELL_PARAMETER_SELECTION_STRING2));
}

TEST_F(oled_menu, go_back_from_panel_menu_to_manufacturer_menu)
{
    navigate_to_panel_selection();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_controlling_global_irradiation_to_manufacturer_menu)
{
    navigate_to_global_irradiation();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_selecting_global_parameters_to_manufacturer_menu)
{
    navigate_to_global_parameter_selection();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_global_temperature_to_manufacturer_menu)
{
    navigate_to_global_parameter_selection();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_cell_selection_to_manufacturer_menu)
{
    navigate_to_cell_selection();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_cell_parameter_selection_to_manufacturer_menu)
{
    navigate_to_cell_parameters();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_controlling_cell_irradiation_to_manufacturer_menu)
{
    navigate_to_cell_irradiation();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, go_back_from_controlling_cell_temperature_to_manufacturer_menu)
{
    navigate_to_cell_temperature();
    lcd_string.clear();
    press_button_longer();
    EXPECT_THAT(menu.state, Eq(STATE_NAVIGATE_MANUFACTURERS));
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

TEST_F(oled_menu, menu_update_is_called_during_init)
{
    EXPECT_THAT(lcd_string, StrEq(MANUFACTURER_SELECTION_STRING));
}

#endif /* TESTING */
