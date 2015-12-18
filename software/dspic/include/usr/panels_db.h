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



void panels_db_init(void);

short panels_db_add_new_manufacturer(const char* name);
short panels_db_add_new_panel(short manufacturer_index,
                                 const char* name);

short panels_db_get_manufacturers_count(void);

const char* panels_db_get_manufacturer_name(short index);

short panels_db_get_panel_count(short manufacturer_index);

const char* panels_db_get_model_name(short manufacturer_index,
                                        short panel_index);

#ifdef	__cplusplus
}
#endif

#endif /* PANELS_DB_H */
