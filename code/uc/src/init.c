/*!
 * @file init.c
 * @author Alex Murray
 * 
 * Created on 18 October 2015, 14:34
 */

#include "init.h"
#include "hw.h"

/* -------------------------------------------------------------------------- */
/* Disable watchdog timer */
_FWDT(WDTEN_OFF);

/* -------------------------------------------------------------------------- */
/* 
 * Oscillator setup - We have to boot using a clock without PLL. We switch
 * to PLL during initialisation.
 */

_FOSCSEL(
    FNOSC_FRC & /* Use Internal Fast RC oscillator -- approx. 7.37 MHz */
    IESO_OFF);  /* Start up with user selected oscillator source*/

_FOSC(
    FCKSM_CSECMD &  /* enable clock switching for the switch to PLL to work */
    OSCIOFNC_ON  &  /* OSC2 is a general purpose IO pin */
    IOL1WAY_ON &    /* allow only one reconfiguration of clock */
    POSCMD_NONE);   /* primary oscillator is disabled (we're using internal) */

static void sysclk60mips(void)
{
    /* 
     * Configure PLL - Target execution speed is Fcy = 60 MIPS
     *   Fpllo = 2*Fcy = 120 MHz (15 MHz < Fpllo < 120 MHz)
     *   Fvco = 2*Fpllo = 240 MHz (120 MHz < Fvco < 340 MHz)
     *   Fplli = 7.37 MHz / 2 = 3.685 MHz
     *      --> M = 65
     */
    PLLFBD = 63;   /* Feedback divider M = 65 */
    _PLLPOST = 0;  /* N2 = 2 (can't go lower) */
    _PLLPRE = 0;   /* N1 = 2 (can't go lower) */
    
    /* initiate clock switch to 0x01 = FRC oscillator with PLL */
    __builtin_write_OSCCONH(0x01);          /* New oscillator selection bits */
    __builtin_write_OSCCONL(OSCCON | 0x01); /* requests oscillator switch to
                                             * the selection specified above */
    
    /* wait for clock switch to occur */
    while(OSCCONbits.COSC != 0x01);
    
    /* wait for PLL to lock */
    while(OSCCONbits.LOCK != 1);
}

/* -------------------------------------------------------------------------- */
static void ports(void)
{
    /* 
     * Set unused pins as output and drive low.
     * For TRISx: "1" means input, "0" means output. Default is input.
     */
    /* lower byte ------------ */
    TRISA &= ~(                        BIT4  | BIT3);
    TRISB &= ~(BIT7  | BIT6  | BIT5  | BIT4  |                 BIT1  | BIT0);
    TRISC &= ~(BIT7  |                                                 BIT0);
    TRISD &= ~(BIT7  |       | BIT5  | BIT4  |         BIT2  |         BIT0);
    /* upper byte ------------ */
    TRISB &= ~(        BIT14 | BIT13 | BIT12 |         BIT10 | BIT9);
    TRISC &= ~(BIT15 | BIT14 | BIT13 |                 BIT10);
    TRISD &= ~(BIT15 | BIT14 |         BIT12 | BIT11 | BIT10 | BIT9 | BIT8);
    
    /* LCD reset is a 5V output signal, set to open drain */
    _TRISB11 = 0; /* output */
    _ODCB11 = 1;  /* open drain for 5V operation */
    
    /* buck enable pin */
    _TRISA2 = 0; /* output */
}

/* -------------------------------------------------------------------------- */
void init_device(void)
{
    sysclk60mips();
    ports();
}
