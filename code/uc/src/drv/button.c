/*! 
 * @file button.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/button.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/* called when the button is pressed (falling edge) */
void _ISR_NOPSV _INT1Interrupt(void)
{
    /* button was pressed, trigger event */
    event_post(EVENT_BUTTON_PRESSED, NULL);
    
    /* clear interrupt flag */
    IFS1bits.INT1IF = 0;
}

/* -------------------------------------------------------------------------- */
/* called when the button is twisted (A or B changed) */
void _ISR_NOPSV _CNInterrupt(void)
{
    static unsigned char current_AB = 0;
    
    /* 
     * Read the new values of A and B.
     * 2-bit gray-code can be converted to binary by flipping the last bit,
     * if the number is greater than 1.
     * 00
     * 01 _
     * 11  | Flip these
     * 10 _|
     */
    unsigned char AB = KNOB_AB;  /* read from register */
    AB ^= (AB >> 1); /* flip bit0 if bit1 is set */
    
    /* 
     * Determine the direction the knob was twisted and post an event
     * accordingly.
     */
    if(AB > current_AB || (current_AB == 3 && AB == 0))
        event_post(EVENT_BUTTON_TWISTED, (void*)BUTTON_TWISTED_LEFT);
    else
        event_post(EVENT_BUTTON_TWISTED, (void*)BUTTON_TWISTED_RIGHT);
    
    /* update current AB */
    current_AB = AB;

    /* clear interrupt flag */
    IFS1bits.CNIF = 0;
}
