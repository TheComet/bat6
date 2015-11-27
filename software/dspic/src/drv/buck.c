/*! 
 * @file buck.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/buck.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stddef.h>

/* -------------------------------------------------------------------------- */
void buck_init(void)
{
    /* 
     * UVLO is a digital input signal. Because all pins are configured as analog
     * inputs by default, clear analog flag.
     */
    ANSELC &= ~0x0200;      /* bit 9 */
    
    /* buck enable is a digital output */
    TRISAbits.TRISA2 = 0;   /* output */
    
    /* configure BUCK_UVLO to trigger an interrupt on a rising edge */
    RPINR1bits.INT2R = 57;  /* assign INT2 to pin RP57 (BUCK_UVLO) */
    INTCON2bits.INT2EP = 0; /* interrupt on rising edge */
    IFS1bits.INT2IF = 0;    /* clear interrupt flag */
    IEC1bits.INT2IE = 1;    /* enable INT2 interrupt */
}

/* -------------------------------------------------------------------------- */
/* Called when an UVLO event occurs */
void _ISR_NOPSV _INT2Interrupt(void)
{
    buck_disable();
    event_post(EVENT_UVLO, NULL);
    
    /* clear interrupt flag */
    IFS1bits.INT2IF = 0;
}
