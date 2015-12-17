/*!
 * @file solar_panels.c
 * @author Alex Murray
 *
 * Created on 17 December 2015, 13:53
 */

#include "usr/solar_panels.h"
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
    struct panel_t panels[2];
};

struct manufacturer_t database[] = {
    { "Generic Manufacturer", {
        { "Generic Panel 1", {
            {24 * 65536, 3 * 65536, 1.5 * 65536, 1 * 65536}
        }},
        { "Generic Panel 2", {
            {12 * 65536, 1 * 65536, 1.5 * 65536, 1 * 65536}
        }}
    }},
    { "Batshit Insane Coorporation", {
        { "Panel of Shit", {
            {24 * 65536, 2 * 65536, 1.2 * 65536, 1 * 65536}
        }},
        { "Panel of Fuck", {
            {24 * 65536, 2 * 65536, 1.2 * 65536, 1 * 65536}
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
