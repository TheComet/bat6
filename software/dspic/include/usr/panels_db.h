/*!
 * @file panels_db.h
 * @author Alex Murray
 *
 * Created on 17 December 2015, 13:53
 */

#ifndef PANELS_DB_H
#define PANELS_DB_H

#ifdef	__cplusplus
extern "C" {
#endif

struct pv_cell_t;

/*!
 * @brief Initialises panel database. Call this before calling any other
 * db related functions.
 */
void panels_db_init(void);

/*!
 * @brief Adds a new manufacturer.
 */
short panels_db_add_new_manufacturer(const char* name);

/*!
 * @brief Adds a new
 */
short panels_db_add_new_panel(short manufacturer_id,
                              const char* name);

short panels_db_get_manufacturer_count(void);

const char* panels_db_get_manufacturer_name(short index);

short panels_db_get_panel_count(short manufacturer_id);

const char* panels_db_get_panel_name(short manufacturer_id,
                                     short panel_id);

short panels_db_get_cell_count(short manufacturer_id,
                               short panel_id);

const struct pv_cell_t* panels_db_get_cell(short manufacturer_id,
                                           short panel_id,
                                           short cell_id);

#ifdef	__cplusplus
}
#endif

#endif /* PANELS_DB_H */
