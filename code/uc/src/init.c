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

static void init_sysclk60mips(void)
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
    TRISC = 0b0001101001111110;
    TRISD = 0b0010000001001010;
    
    /* LCD reset is a 5V output signal, set to open drain */
    _TRISB11 = 0; /* output */
    _ODCB11 = 1;  /* open drain for 5V operation */
    
    /* buck enable pin */
    _TRISA2 = 0; /* output */
    
    /*
     * Configure change notification module for:
     *  + push/twist button (bit 4, 5, 6)
     *  + UVLO (bit 9)
     */
    CNPUC = 0x0070;    /* pull-ups for bit 4, 5, and 6 (button connections) */
    CNENC = 0x0170;    /* enable interrupts for bits 4, 5, 6 and 9 */
    IFS1bits.CNIF = 0; /* clear interrupt flag for change notifications */
    IEC1bits.CNIE = 1; /* enable change notification interrupts */
}

/* -------------------------------------------------------------------------- */
static void init_uart(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void init_leds(void)
{
    
}

/* -------------------------------------------------------------------------- */
static void init_lcd(void)
{
    /*
     * Communication to the LCD is achieved over I2C. The address of the LCD is
     * defined in hw.h as LCD_ADDRESS.
     */
    
    /* enable I2C2 module, master mode*/
    I2C2CONLbits.I2CEN = 1;
}

/* -------------------------------------------------------------------------- */
void init_device(void)
{
    disable_interrupts();
    
    init_sysclk60mips();
    init_ports();
    init_uart();
    init_leds();
    init_lcd();
    
    enable_interrupts();
}
