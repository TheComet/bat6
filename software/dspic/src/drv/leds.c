/*!
 * @file leds.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include <p33EP16GS506.h>

#include "drv/leds.h"
#include "core/event.h"

/* -------------------------------------------------------------------------- */
static void on_update(unsigned int arg)
{

}

/* -------------------------------------------------------------------------- */
void leds_init(void)
{
    PTCONbits.PTEN = 0;

    PTCON2bits.PCLKDIV = 0; /* Don't divide input clock (120 MHz) */

    /* listen to 10 ms update events */
    event_register_listener(EVENT_UPDATE, on_update);
}

