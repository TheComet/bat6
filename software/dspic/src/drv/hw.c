/*!
 * @file init_hw.c
 * @author Alex Murray
 *
 * Created on 18 October 2015, 14:34
 */

#include "drv/hw.h"
#include "core/event.h"
#include "drv/buck.h"
#include "drv/button.h"
#include "drv/lcd.h"
#include "drv/leds.h"
#include "drv/timer.h"
#include "drv/uart.h"

/* -------------------------------------------------------------------------- */
/* Disable watchdog timer */
_FWDT(WDTEN_OFF)

/* -------------------------------------------------------------------------- */
/*
 * Oscillator setup - We have to boot using a clock without PLL. We switch
 * to PLL during initialisation.
 */
_FOSCSEL(
    FNOSC_FRC & /* Use Internal Fast RC oscillator -- approx. 7.37 MHz */
    IESO_OFF)   /* Start up with user selected oscillator source*/

_FOSC(
    FCKSM_CSECMD &  /* enable clock switching for the switch to PLL to work */
    OSCIOFNC_ON  &  /* OSC2 is a general purpose IO pin */
    IOL1WAY_ON &    /* allow only one reconfiguration of clock */
    POSCMD_NONE)    /* primary oscillator is disabled (we're using internal) */

static void init_sysclk60mips(void)
{
    /*
     * Configure PLL - Target execution speed is Fcy = 60 MIPS
     *   Fpllo = 2*Fcy = 120 MHz (15 MHz < Fpllo < 120 MHz)
     *   Fvco = 2*Fpllo = 240 MHz (120 MHz < Fvco < 340 MHz)
     *   Fplli = 7.37 MHz / 2 = 3.685 MHz
     *      --> M = 65
     */
    PLLFBD = 63;             /* Feedback divider M = 65 */
    CLKDIVbits.PLLPOST = 0;  /* N2 = 2 (can't go lower) */
    CLKDIVbits.PLLPRE = 0;   /* N1 = 2 (can't go lower) */

    /* initiate clock switch */
    __builtin_write_OSCCONH(0x01);          /* 0x01 = FRC oscillator with PLL */
    __builtin_write_OSCCONL(OSCCON | 0x01); /* requests oscillator switch to
                                             * the selection specified above */

    /* wait for clock switch to occur */
    while(OSCCONbits.COSC != 0x01);

    /* wait for PLL to lock */
    while(OSCCONbits.LOCK != 1);
}

/* -------------------------------------------------------------------------- */
static void init_ports(void)
{
    /*
     * Set unused pins as output and drive low.
     * For TRISx: "1" means input, "0" means output. Default is input.
     */
    /* drive low */
    PORTA = 0x00;
    PORTB = 0x0000;
    PORTC = 0x0000;
    PORTD = 0x0000;
    /*        FEDCBA9876543210 */
    TRISA =            0b00111;
    TRISB = 0b1000100100001100;
    //TRISC = 0b0000101001111110; // RP58 and RP61 on Dout
    TRISC = 0b0000111001111110; // RP58 on Din, RP61 on Dout
    //TRISC = 0b0010101001111110; // RP58 on Dout, RP61 on Din
    //TRISC = 0b0010111001111110; // RP58 and RP61 on Din
    TRISD = 0b0010000001001010;
}

static void init_auxiliary_clock(void)
{
    /*
     * The auxiliary clock is the only clock source the PWM modules can use.
     * We can go up to 120 MHz with this.
     */
    ACLKCONbits.SELACLK = 0;    /* primary PLL provides clock source (120MHz) */
    ACLKCONbits.APSTSCLR = 0x7; /* divide by 1 */
}

/* -------------------------------------------------------------------------- */
void hw_init(void)
{
    disable_interrupts();

    /* system clock and initial port config */
    init_sysclk60mips();
    init_auxiliary_clock();
    init_ports();

    enable_interrupts();
}

/* -------------------------------------------------------------------------- */
void drivers_init(void)
{
    disable_interrupts();

    /* initialise all drivers here */
    buck_init();
    button_init();
    lcd_init();
    leds_init();
    timer_init();
    uart_init();

    enable_interrupts();
}

/* -------------------------------------------------------------------------- */
void drivers_deinit(void)
{
    /* de-initialise all drivers here */

    /* de-initialise event system */
    event_deinit();
}
