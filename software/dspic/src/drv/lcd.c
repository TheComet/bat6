/*!
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include <stdint.h>
#include "drv/lcd.h"
#include "drv/hw.h"
#include "core/event.h"

#define NULL ((void*)0)

static int timer = 0;

static enum{
    lcd_idle = 0,
    lcd_starting,
    lcd_addressing,
    lcd_setting,
    lcd_sending,
    lcd_stopping
}lcd_state = lcd_idle;

static unsigned char c_or_d = 0;
static char *lcd_data = NULL; 
static unsigned char lcd_length = 0;

static void lcd_single_byte(char is_command, char byte){
    I2C2CONLbits.SEN = 1;
    while(I2C2CONLbits.SEN);
    I2C2TRN = 0x78;
    while(I2C2STATbits.TRSTAT);
    if(is_command){
        I2C2TRN = 0xC0;
    }else{
        I2C2TRN = 0x80;
    }
    while(I2C2STATbits.TRSTAT);
    I2C2TRN = byte;
    while(I2C2STATbits.TRSTAT);
    I2C2CONLbits.PEN = 1;
    while(I2C2CONLbits.PEN);
}

int lcd_send(char is_command, char * data, unsigned char length) {
    if(lcd_state != lcd_idle){
        return 1;
    }
    if(data == NULL){
        return 1;
    }
    if(length == 0){
        return 1;
    }
    
    LED0_ON;
    c_or_d = !!is_command;
    lcd_data = data;
    lcd_length = length;
    lcd_state = lcd_starting;
    I2C2CONLbits.SEN = 1;
    return 0;
}

/* -------------------------------------------------------------------------- */

static void lcd_statemachine_tick(){
    
    IFS3bits.MI2C2IF = 0; /* clear interrupt flag */    
}

static void lcd_reset(){
    volatile unsigned int delay = 0;
 
    PORTBbits.RB11 = 0; /* reset LCD */
    for(delay = 0; delay < 10000; delay++);
    PORTBbits.RB11 = 1; /* enable LCD */
    for(delay = 0; delay < 10000; delay++);

    lcd_single_byte(0, 0x2a);
    lcd_single_byte(0, 0x71);
    lcd_single_byte(1, 0x5c);
    lcd_single_byte(0, 0x28);
    lcd_single_byte(0, 0x08);
    lcd_single_byte(0, 0x2a);
    lcd_single_byte(0, 0x79);
    lcd_single_byte(0, 0xd5);
    lcd_single_byte(0, 0x70);
    lcd_single_byte(0, 0x78);
    lcd_single_byte(0, 0x09);
    lcd_single_byte(0, 0x06);
    lcd_single_byte(0, 0x72);
    lcd_single_byte(1, 0x00);
    lcd_single_byte(0, 0x2a);
    lcd_single_byte(0, 0x79);
    lcd_single_byte(0, 0xda);
    lcd_single_byte(0, 0x10);
    lcd_single_byte(0, 0xdc);
    lcd_single_byte(0, 0x00);
    lcd_single_byte(0, 0x81);
    lcd_single_byte(0, 0x7f);
    lcd_single_byte(0, 0xd9);
    lcd_single_byte(0, 0xf1);
    lcd_single_byte(0, 0xdb);
    lcd_single_byte(0, 0x40);
    lcd_single_byte(0, 0x78);
    lcd_single_byte(0, 0x28);
    lcd_single_byte(0, 0x01);
    lcd_single_byte(0, 0x80);
    lcd_single_byte(0, 0x0c);
    lcd_single_byte(0, 0x01);
}

static void on_update(unsigned int arg) {
    /* remove this shit at some point */
    timer++;
    if (timer > 100) {
        timer = 0;
        lcd_send(1, "SuperGay!", 9);
    }
}

/* -------------------------------------------------------------------------- */
void lcd_init(void) {
    
    unlock_registers();

    /* LCD reset is a 5V output signal, set to open drain and let external
     * pull-ups do their job */
    PORTBbits.RB11 = 0; /* disable LCD */
    ODCBbits.ODCB11 = 1; /* open drain for 5V operation */
    TRISBbits.TRISB11 = 0; /* output */

    lock_registers();
    /*
     * Communication to the LCD is achieved over I2C. The address of the LCD
     * is defined in hw.h as LCD_ADDRESS.
     */

    /* 
     * (1/400kHz - 120ns) * FS/2 - 2
     * FS = 60MHz, see hw.h
     */
    I2C2BRG = 141;

    I2C2CONHbits.SCIE = 1; /* enable start condition interrupt */
    I2C2CONHbits.PCIE = 1; /* enable stop condition interrupt */

    IFS3bits.MI2C2IF = 0; /* clear master interrupt flag */
    IFS3bits.SI2C2IF = 0; /* clear slave interrupt flag */
    IEC3bits.MI2C2IE = 1; /* enable master interrupts */
    IEC3bits.SI2C2IE = 0; /* disable slave interrupts */

    /* enable I2C2 module, master mode*/
    I2C2CONLbits.I2CEN = 1;

    lcd_reset();
       
    event_register_listener(EVENT_UPDATE, on_update);
}
