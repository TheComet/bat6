/*! 
 * @file buck.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "buck.h"
#include "hw.h"

/* -------------------------------------------------------------------------- */
/* Called when an UVLO event occurs */
void _ISR_NOPSV _INT2Interrupt(void)
{
    /* clear interrupt flag */
    IFS1bits.INT2IF = 0;
}
