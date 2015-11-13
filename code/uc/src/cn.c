/*! 
 * @file cn.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "hw.h"
#include "button.h"

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _CNInterrupt(void)
{
    notify_button_change();
    /* TODO: UVLO */
    
    /* clear interrupt flag */
    IFS1bits.CNIF = 0;
}
