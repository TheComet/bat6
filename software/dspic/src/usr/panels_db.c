/*!
 * @file panels_db.c
 * @author Alex Murray
 *
 * Created on 17 December 2015, 13:53
 */

#include "usr/panels_db.h"
#include "usr/pv_model.h"
#include "drv/hw.h"
#include <stddef.h>

struct panel_t
{
    const char name[19];
    struct pv_cell_t cells[1];
};

struct manufacturer_t
{
    const char name[19];
    struct panel_t panels[2];
};

#define CELL_PARAM(x) ((_Q16)(x * 65536))

struct manufacturer_t database[] = {
    { "Generic", {
        { "Generic Panel 1", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }},
        { "Generic Panel 2", {
            {CELL_PARAM(24), CELL_PARAM(3), CELL_PARAM(1.5), CELL_PARAM(1)}
        }}
    }}
};

/* -------------------------------------------------------------------------- */
void panels_db_init(void)
{

}

/* -------------------------------------------------------------------------- */
short panels_db_add_new_manufacturer(const char* name)
{
    return -1; /* not implemented in this version */
}

/* -------------------------------------------------------------------------- */
short panels_db_add_new_panel(short manufacturer_id,
                                 const char* name)
{
    return -1; /* not implemented in this version */
}

/* -------------------------------------------------------------------------- */
short panels_db_get_manufacturers_count(void)
{
    return sizeof(database) / sizeof(*database);
}

/* -------------------------------------------------------------------------- */
const char* panels_db_get_manufacturer_name(short index)
{
    return database[index].name;
}

/* -------------------------------------------------------------------------- */
short panels_db_get_panel_count(short manufacturer_id)
{
    short i;
    short count = 0;
    for(i = 0; i != sizeof(database->panels) / sizeof(*database->panels); ++i)
        if(*database[manufacturer_id].panels[i].name)
            ++count;
    return count;
}

/* -------------------------------------------------------------------------- */
const char* panels_db_get_panel_name(short manufacturer_id,
                                     short panel_id)
{
    return database[manufacturer_id].panels[panel_id].name;
}

/* -------------------------------------------------------------------------- */
short panels_db_get_cell_count(short manufacturer_id,
                               short panel_id)
{
    short i;
    short count = 0;

    for(i = 0;
        i != sizeof(database->panels->cells) / sizeof(*database->panels->cells);
        ++i)
    {
        if(database[manufacturer_id].panels[panel_id].cells[i].voc)
            ++count;
    }

    return count;
}

/* -------------------------------------------------------------------------- */
const struct pv_cell_t* panels_db_get_cell(short manufacturer_id,
                                           short panel_id,
                                           short cell_id)
{
    return &database[manufacturer_id].panels[panel_id].cells[cell_id];
}

/* -------------------------------------------------------------------------- */
/* UNIT TESTS */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

TEST(panels_db, get_manufacturers_count)
{
    EXPECT_THAT(panels_db_get_manufacturers_count(), Eq(1));
}

TEST(panels_db, get_panel_count)
{
    EXPECT_THAT(panels_db_get_panel_count(0), Eq(2));
}

TEST(panels_db, get_cell_count)
{
    EXPECT_THAT(panels_db_get_cell_count(0, 0), Eq(1));
}

#endif /* TESTING */
