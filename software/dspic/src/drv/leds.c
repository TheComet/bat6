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
    PDC4 = 0x8000;
    SDC4 = 0x8000;
    PDC5 = 0x8000;
    SDC5 = 0x8000;
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

        /*
         * Because all pins are configured as analog inputs by default,  clear
         * analog flags.
         */
        ANSELC &= ~(BIT3);
        ANSELD &= ~(BIT1 & BIT3 & BIT6);

        /* configure as digital output */
        TRISC &= ~(BIT3);
        TRISD &= ~(BIT1 & BIT3 & BIT6);

        PORTCbits.RC3 = 1;

    lock_registers();

    /* Disable PWM module for configuration */
    PTCONbits.PTEN = 0;

    /* True independent PWM mode */
    IOCON4bits.PMOD = 0b11;
    IOCON5bits.PMOD = 0b11;

    PTCONbits.PTEN = 1;

    /* listen to 10 ms update events */
    event_register_listener(EVENT_UPDATE, on_update);
}
