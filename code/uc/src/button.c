/*! 
 * @file button.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "button.h"
#include "hw.h"

/* -------------------------------------------------------------------------- */
/* called when the button is pressed (falling edge) */
void _ISR_NOPSV _INT1Interrupt(void)
{
    /* clear interrupt flag */
    _INT1IF = 0;
}

/* -------------------------------------------------------------------------- */
/* called when the button is twisted (A or B changed) */
void _ISR_NOPSV _CNInterrupt(void)
{
    /* clear interrupt flag */
    _CNIF = 0;
}
