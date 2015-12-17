/*!
 * @file solar_panels.h
 * @author Alex Murray
 *
 * Created on 17 December 2015, 13:53
 */

#ifndef SOLAR_PANELS_H
#define SOLAR_PANELS_H

#ifdef	__cplusplus
extern "C" {
#endif



void solar_panels_init(void);

short solar_panels_add_new_manufacturer(const char* name);
short solar_panels_add_new_panel(short manufacturer_index,
                                 const char* name);

short solar_panels_get_manufacturers_count(void);

const char* solar_panels_get_manufacturer_name(short index);

short solar_panels_get_panel_count(short manufacturer_index);

const char* solar_panels_get_model_name(short manufacturer_index,
                                        short panel_index);

#ifdef	__cplusplus
}
#endif

#endif /* SOLAR_PANELS_H */
