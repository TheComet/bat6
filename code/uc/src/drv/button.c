/*! 
 * @file button.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/button.h"
#include "drv/hw.h"
#include "event.h"

static struct
{
    volatile unsigned AB:2;
    volatile unsigned pressed:1;
    volatile unsigned angle:7;
    unsigned :6;
} button = {0};

/* -------------------------------------------------------------------------- */
/* called when the button is pressed (falling edge) */
void _ISR_NOPSV _INT1Interrupt(void)
{
    button.pressed = 1;
    
    /* clear interrupt flag */
    IFS1bits.INT1IF = 0;
}

/* -------------------------------------------------------------------------- */
/* called when the button is twisted (A or B changed) */
void _ISR_NOPSV _CNInterrupt(void)
{
    /*
     * 00
     * 01
     * 11
     * 10
     */

    /* clear interrupt flag */
    IFS1bits.CNIF = 0;
}
