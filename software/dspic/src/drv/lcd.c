/*! 
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/lcd.h"
#include "drv/hw.h"

/* -------------------------------------------------------------------------- */
void lcd_init(void)
{
    /* LCD reset is a 5V output signal, set to open drain and let external
     * pull-ups do their job */
    ODCBbits.ODCB11 = 1;    /* open drain for 5V operation */
    TRISBbits.TRISB11 = 0;  /* output */
    
    /*
     * Communication to the LCD is achieved over I2C. The address of the LCD is
     * defined in hw.h as LCD_ADDRESS.
     */
    
    /* enable I2C2 module, master mode*/
    I2C2CONLbits.I2CEN = 1;
}

/* -------------------------------------------------------------------------- */
/* master event interrupt */
void _ISR_NOPSV _MI2C2Interrupt(void)
{
    
}

/* -------------------------------------------------------------------------- */
/* slave event interrupt */
void _ISR_NOPSV _SI2C2Interrupt(void)
{
    
}

