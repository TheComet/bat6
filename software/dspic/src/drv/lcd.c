/*!
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/lcd.h"
#include "drv/hw.h"
#include "core/event.h"

static int timer = 0;

static void on_update(unsigned int arg)
{
    /* remove this shit at some point */
    timer++;
    if(timer > 50)
    {
        LED0_OFF;
        timer = 0;
        
        I2C2CONLbits.SEN = 1;
        while(I2C2CONLbits.SEN); /* fuck */
        I2C2TRN = 0xAA;
    }
}

/* -------------------------------------------------------------------------- */
void lcd_init(void)
{
    unlock_registers();

        /* LCD reset is a 5V output signal, set to open drain and let external
         * pull-ups do their job */
        PORTBbits.RB11 = 0;     /* disable LCD */
        ODCBbits.ODCB11 = 1;    /* open drain for 5V operation */
        TRISBbits.TRISB11 = 0;  /* output */

        /*
         * Communication to the LCD is achieved over I2C. The address of the LCD
         * is defined in hw.h as LCD_ADDRESS.
         */
        
        /* 
         * (1/400kHz - 120ns) * FS/2 - 2
         * FS = 60MHz, see hw.h
         */
        I2C2BRG = 141;
        
        I2C2CONHbits.SCIE = 1;  /* enable start condition interrupt */
        I2C2CONHbits.PCIE = 1;  /* enable stop condition interrupt */
        
        IFS3bits.MI2C2IF = 0;   /* clear master interrupt flag */
        IFS3bits.SI2C2IF = 0;   /* clear slave interrupt flag */
        IEC3bits.MI2C2IE = 1;   /* enable master interrupts */
        IEC3bits.SI2C2IE = 1;   /* enable slave interrupts */
        
        /* enable I2C2 module, master mode*/
        I2C2CONLbits.I2CEN = 1;

    lock_registers();
    
    PORTBbits.RB11 = 1; /* enable LCD */
    
    event_register_listener(EVENT_UPDATE, on_update);
}

/* -------------------------------------------------------------------------- */
/* master event interrupt */
void _ISR_NOPSV _MI2C2Interrupt(void)
{
    IFS3bits.MI2C2IF = 0;  /* clear interrupt flag */
}

/* -------------------------------------------------------------------------- */
/* slave event interrupt */
void _ISR_NOPSV _SI2C2Interrupt(void)
{
    LED0_ON;
    if(I2C2STATbits.ACKSTAT)
        LED0_ON;
    
    IFS3bits.SI2C2IF = 0;  /* clear interrupt flag */
}
