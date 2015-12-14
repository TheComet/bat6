/*!
 * @file main.c
 * @author Alex Murray
 *
 * Created on 18 October 2015, 14:34
 */

#include "drv/hw.h"
#include "drv/leds.h"
#include "core/event.h"
#include "drv/buck.h"

/* -------------------------------------------------------------------------- */
#ifdef TESTING
void dspic_main(void)
#else
int main(void)
#endif
{
    hw_init();
    drivers_init();

    while(1)
    {
        event_dispatch_all();
    }
}
