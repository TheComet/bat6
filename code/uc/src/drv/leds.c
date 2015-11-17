/*! 
 * @file leds.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/leds.h"
#include "core/event.h"

/* -------------------------------------------------------------------------- */
static void on_update(void* args)
{
    
}

/* -------------------------------------------------------------------------- */
void leds_init(void)
{
    event_register_listener(EVENT_UPDATE, on_update);
}
