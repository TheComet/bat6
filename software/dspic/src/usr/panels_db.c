/*!
 * @file panels_db.c
 * @author Alex Murray
 *
 * Created on 17 December 2015, 13:53
 */

#include "usr/panels_db.h"
#include "usr/pv_model.h"
#include <stddef.h>

struct panel_t
{
    const char name[32];
    struct pv_cell_t cells[1];
};

struct manufacturer_t
{
    const char name[32];
    struct panel_t panels[5];
};

#define CELL_PARAM(x) ((unsigned int)(x * 65536))

struct manufacturer_t database[] = {
    { "Generic Manufacturer", {
        { "Generic Panel 1", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Generic Panel 2", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }},
    { "Batshit Coorporation", {
        { "Panel of Shit", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Panel of Fuck", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }},
    { "Actual Trash", {
        { "Panel of Shit", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Panel of Fuck", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Trash Stash", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Compost", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Dick Stuck in it", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }},
    { "Panels? Panls.", {
        { "Panel of Shit", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Panel of Fuck", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }},
    { "Sandals", {
        { "Panel of Shit", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Panel of Fuck", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }}
};

/* -------------------------------------------------------------------------- */
void solar_panels_init(void)
{

}

/* -------------------------------------------------------------------------- */
short solar_panels_add_new_manufacturer(const char* name)
{
    return -1; /* not implemented in this version */
}

/* -------------------------------------------------------------------------- */
short solar_panels_add_new_panel(short manufacturer_index,
                                 const char* name)
{
    return -1; /* not implemented in this version */
}

/* -------------------------------------------------------------------------- */
short solar_panels_get_manufacturers_count(void)
{
    return sizeof(database) / sizeof(*database);
}

/* -------------------------------------------------------------------------- */
const char* solar_panels_get_manufacturer_name(short index)
{
    return database[index].name;
}

/* -------------------------------------------------------------------------- */
short solar_panels_get_panel_count(short manufacturer_index)
{
    return sizeof(database->panels) / sizeof(*database->panels);
}

/* -------------------------------------------------------------------------- */
const char* solar_panels_get_model_name(short manufacturer_index,
                                        short panel_index)
{
    return database[manufacturer_index].panels[panel_index].name;
}
