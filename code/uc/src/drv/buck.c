/*! 
 * @file buck.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/buck.h"
#include "drv/hw.h"

/* -------------------------------------------------------------------------- */
/* Called when an UVLO event occurs */
void _ISR_NOPSV _INT2Interrupt(void)
{
    buck_disable();
    
    /* clear interrupt flag */
    IFS1bits.INT2IF = 0;
}