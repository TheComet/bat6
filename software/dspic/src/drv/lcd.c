/*!
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include <stdint.h>
#include <string.h>
#include "drv/lcd.h"
#include "drv/hw.h"
#include "core/event.h"

enum lcd_states{
    lcd_idle = 0,
    lcd_starting,
    lcd_configuring,
    lcd_sending,
    lcd_stopping
};
static enum lcd_states lcd_state = lcd_idle;

#define LCD_FIFO_SIZE 128
#define LCD_FIFO_TYPE short
static LCD_FIFO_TYPE lcd_fifo[LCD_FIFO_SIZE] = {};
static unsigned short lcd_fifo_start = 0;
static volatile unsigned short lcd_fifo_end = 0;

static int lcd_fifo_full(){
    return lcd_fifo_start == ((lcd_fifo_end + 1) % LCD_FIFO_SIZE);
}
static int lcd_fifo_empty(){
    return lcd_fifo_start == lcd_fifo_end;
}
static LCD_FIFO_TYPE lcd_fifo_get(){
    LCD_FIFO_TYPE data = 0;
    if(!lcd_fifo_empty()){
        data = lcd_fifo[lcd_fifo_start++];
        if(lcd_fifo_start >= LCD_FIFO_SIZE){
            lcd_fifo_start = 0;
        }
    }
    return data;
}
static LCD_FIFO_TYPE lcd_fifo_peek(){
    LCD_FIFO_TYPE data = 0;
    if(!lcd_fifo_empty()){
        data = lcd_fifo[lcd_fifo_start];
    }
    return data;
}
static void lcd_fifo_put(LCD_FIFO_TYPE data){
    if(!lcd_fifo_full()){
        lcd_fifo[lcd_fifo_end++] = data;
        if(lcd_fifo_end >= LCD_FIFO_SIZE){
            lcd_fifo_end = 0;
        }
    }
}

static void lcd_send_blocking(char is_command, char data){
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
    I2C2TRN = data;
    while(I2C2STATbits.TRSTAT);
    I2C2CONLbits.PEN = 1;
    while(I2C2CONLbits.PEN);
}


static void lcd_statemachine_tick(){
    static unsigned char current_config = 0;

    switch(lcd_state){
        case lcd_idle:
            if(!lcd_fifo_empty()){
                lcd_state = lcd_starting;
                I2C2CONLbits.SEN = 1;
            }
            break;
        case lcd_starting:
            lcd_state = lcd_configuring;

            I2C2TRN = 0x78; // I2C Address of Display

            break;
        case lcd_configuring:
            lcd_state = lcd_sending;
            current_config = lcd_fifo_peek() >> 8;
            I2C2TRN = current_config << 6;
            break;
        case lcd_sending:
            if(lcd_fifo_empty()){
                I2C2CONLbits.PEN = 1;
                lcd_state = lcd_stopping;
            }else if((lcd_fifo_peek() >> 8) != current_config){
                I2C2CONLbits.PEN = 1;
                lcd_state = lcd_stopping;
            }else{
                I2C2TRN = lcd_fifo_get() & 0xff;
            }

            break;
        case lcd_stopping:
            if(lcd_fifo_empty()){
                lcd_state = lcd_idle;
            }else{
                lcd_state = lcd_starting;
                I2C2CONLbits.SEN = 1;
            }
            break;
        default:
            break;
    }
}

short lcd_send(char is_command, unsigned char data) {
    if(lcd_fifo_full()){
        return 1;
    }
    if(is_command){
        lcd_fifo_put(0x0000 | data);
    }else{
        lcd_fifo_put(0x0100 | data);
    }
    if(lcd_state == lcd_idle){
        lcd_statemachine_tick();
    }
    return 0;
}

void lcd_reset(){

    volatile unsigned int delay = 0;

    PORTBbits.RB11 = 0; /* reset LCD */
    for(delay = 0; delay < 10000; delay++);
    PORTBbits.RB11 = 1; /* enable LCD */
    for(delay = 0; delay < 10000; delay++);

    lcd_send_blocking(0, 0x2a);
    lcd_send_blocking(0, 0x71);
    lcd_send_blocking(1, 0x5c);
    lcd_send_blocking(0, 0x28);
    lcd_send_blocking(0, 0x08);
    lcd_send_blocking(0, 0x2a);
    lcd_send_blocking(0, 0x79);
    lcd_send_blocking(0, 0xd5);
    lcd_send_blocking(0, 0x70);
    lcd_send_blocking(0, 0x78);
    lcd_send_blocking(0, 0x09);
    lcd_send_blocking(0, 0x06);
    lcd_send_blocking(0, 0x72);
    lcd_send_blocking(1, 0x00);
    lcd_send_blocking(0, 0x2a);
    lcd_send_blocking(0, 0x79);
    lcd_send_blocking(0, 0xda);
    lcd_send_blocking(0, 0x10);
    lcd_send_blocking(0, 0xdc);
    lcd_send_blocking(0, 0x00);
    lcd_send_blocking(0, 0x81);
    lcd_send_blocking(0, 0x7f);
    lcd_send_blocking(0, 0xd9);
    lcd_send_blocking(0, 0xf1);
    lcd_send_blocking(0, 0xdb);
    lcd_send_blocking(0, 0x40);
    lcd_send_blocking(0, 0x78);
    lcd_send_blocking(0, 0x28);
    lcd_send_blocking(0, 0x01);
    lcd_send_blocking(0, 0x80);
    lcd_send_blocking(0, 0x0c);
    lcd_send_blocking(0, 0x01);
}

static void on_update(unsigned int arg) {
    /*static int timer = 0;

    if(timer++ > 50){
        timer = 0;
        lcd_send(1, 0x01);
        lcd_writeline(0, "Applejack");
        lcd_writeline(1, "is totally");
        lcd_writeline(0, "gay");
    }*/

}

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
     * (1/100kHz - 120ns) * FS/2 - 2
     * FS = 60MHz, see hw.h
     */
    I2C2BRG = 591;

    I2C2CONHbits.SCIE = 1; /* enable start condition interrupt */
    I2C2CONHbits.PCIE = 1; /* enable stop condition interrupt */

    IFS3bits.MI2C2IF = 0; /* clear master interrupt flag */
    IFS3bits.SI2C2IF = 0; /* clear slave interrupt flag */
    IEC3bits.MI2C2IE = 1; /* enable master interrupts */
    IEC3bits.SI2C2IE = 0; /* disable slave interrupts */

    /* enable I2C2 module, master mode*/
    I2C2CONLbits.I2CEN = 1;

#ifndef TESTING
    lcd_reset();
#endif

    event_register_listener(EVENT_UPDATE, on_update);
}

int lcd_writeline(unsigned char lineN, const char * string){
    int i = 0;
    int n_send = 0;
    char c = 0;
    if(string == NULL){
        return 0;
    }

    lcd_send(1, 0x80 | (lineN << 5));

    while((c = *string++) != '\0'){
        if(lcd_send(0, c)){
            return n_send;
        }
        n_send++;
    }
    for(i = n_send; i < 20; i++){
         if(lcd_send(0, ' ')){
            return n_send;
        }
    }
    return n_send;
}

void _ISR_NOPSV _MI2C2Interrupt(void)
{
    lcd_statemachine_tick();
    IFS3bits.MI2C2IF = 0;  /* clear interrupt flag */
}
