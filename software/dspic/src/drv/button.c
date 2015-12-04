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
void button_init(void)
{
    /* 
     * Twist/push button (bit 4, 5 and 6) are digital input signals. Because all
     * pins are configured as analog inputs by default,  clear analog flags.
     */
    ANSELC &= ~0x0070;      /* bit 4, 5, 6 */
    
    /* twist/push button has three wires that need pull-ups */
    CNPUC |= 0x0070;        /* bit 4, 5, 6 */
    
    /* configure KNOB_BTN to trigger an interrupt when pressed */
    RPINR0bits.INT1R = 54;  /* assign INT1 to pin RP54 */
    INTCON2bits.INT1EP = 1; /* interrupt on falling edge (pressed) */
    IFS1bits.INT1IF = 0;    /* clear interrupt flag */
    IEC1bits.INT1IE = 1;    /* enable INT1 interrupt */
    
    /* configure encoder to trigger interrupts whenever A or B changes */
    CNENC |= 0x30;          /* enable interrupts for bits 4 and 5 */
    IFS1bits.CNIF = 0;      /* clear interrupt flag for change notifications */
    IEC1bits.CNIE = 1;      /* enable change notification interrupts */
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
