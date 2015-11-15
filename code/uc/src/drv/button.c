/*! 
 * @file button.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/button.h"
#include "drv/hw.h"
#include "event.h"
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
static void button_pressed_handler(void* args)
{
    
}

/* -------------------------------------------------------------------------- */
static void button_twisted_handler(void* args)
{
    event_args_e direction = (event_args_e)args;
    switch(direction)
    {
        case BUTTON_TWISTED_LEFT:
            break;
        case BUTTON_TWISTED_RIGHT:
            break;
        default: break;
    }
}

/* -------------------------------------------------------------------------- */
void button_init(void)
{
    event_register_listener(EVENT_BUTTON_PRESSED, button_pressed_handler);
    event_register_listener(EVENT_BUTTON_TWISTED, button_twisted_handler);
}

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
    unsigned char AB = KNOB_AB;
    if(AB & 0x02)
        AB ^= 0x01;
    
    /* 
     * Determine the direction the knob was twisted and post an event
     * accordingly.
     */
    if(AB > current_AB)
        event_post(EVENT_BUTTON_TWISTED, (void*)BUTTON_TWISTED_LEFT);
    else
        event_post(EVENT_BUTTON_TWISTED, (void*)BUTTON_TWISTED_RIGHT);
    
    /* update current AB */
    current_AB = AB;

    /* clear interrupt flag */
    IFS1bits.CNIF = 0;
}
