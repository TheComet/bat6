/*!
 * @file leds.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/leds.h"
#include "drv/hw.h"
#include "core/event.h"

/* -------------------------------------------------------------------------- */
static void on_update(unsigned int arg)
{

}

/* -------------------------------------------------------------------------- */
void leds_init(void)
{
    /*
     * NOTE: Auxiliary clock configuration is implemented in hw.c. It is clocked
     * at 120 MHz.
     */

    /*
     * - The PWM outputs are configured to use the master time base. This means
     *   PTPER controls the period of PWMxH and STPER controls the period of
     *   PWMxL.
     * - PDCx controls the duty cycle of PWMxH and SDCx controls the duty cycle
     *   of PWMxL.
     * - Period is set to support
     */

    unlock_registers();
        /* configure as digital output */
        TRISC &= ~(BIT3);
        TRISD &= ~(BIT1 | BIT3 | BIT6);
    lock_registers();

    /* Disable PWM module for configuration */
    PTCONbits.PTEN = 0;

    /* True independent PWM mode */
    IOCON4bits.PMOD = 0b11;
    IOCON5bits.PMOD = 0b11;

    PTCONbits.PTEN = 0;

    /* listen to 10 ms update events */
    event_register_listener(EVENT_UPDATE, on_update);
}

/* -------------------------------------------------------------------------- */
void led_set(unsigned char led_id, unsigned char state)
{
    switch(led_id)
    {
        case 0 : LATCbits.LATC3 = state; break;
        case 1 : LATDbits.LATD1 = state; break;
        case 2 : LATDbits.LATD3 = state; break;
        case 3 : LATDbits.LATD6 = state; break;
        default: break;
    };
}

/* -------------------------------------------------------------------------- */
void led_all(unsigned char state)
{
    LATCbits.LATC3 = state;
    if(state)
        LATD = PORTD | (BIT1 | BIT3 | BIT6);
    else
        LATD = PORTD & ~(BIT1 | BIT3 | BIT6);
}
