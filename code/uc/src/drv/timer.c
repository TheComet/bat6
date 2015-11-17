/*! 
 * @file timer.c
 * @author Alex Murray
 *
 * Created on 15 November 2015, 02:10
 */

#include "drv/timer.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stddef.h>

/* -------------------------------------------------------------------------- */
void timer_init(void)
{
    /*
     * Target interrupt frequency is 100Hz
     * 
     * Fcy = 7.37 * 65 / 8 = 59.88125 MHz
     * Prescale 1:64 ~ 936 kHz
     * Using 16-bit timer type B: count to 9356 for 100 Hz
     * 
     * We'll be using a timer type B, specifically timer 4, so we don't
     * potentially clash with the timer required for ADC conversions.
     * 
     * Notes on config:
     *  + Clock source select by default is Fosc / 2
     *  + Default mode is 16-bit mode
     */
    T4CONbits.TON = 0;      /* disable timer during config */
    T4CONbits.TCKPS = 0x02; /* prescale 1:64 */
    PR4 = 9356;             /* period match, divide the 936 kHz by 9356 to 
                             * reach 10ms */
    IFS1bits.T4IF = 0;      /* clear interrupt flag */
    IEC1bits.T4IE = 1;      /* enable timer 4 interrupts */
    
    T4CONbits.TON = 1;      /* start timer */
}

/* -------------------------------------------------------------------------- */
/* 10ms timer interrupt */
void _ISR_NOPSV _T4Interrupt(void)
{
    event_post(EVENT_UPDATE, NULL);
    
    /* clear interrupt flag */
    IFS1bits.T4IF = 0;
}
