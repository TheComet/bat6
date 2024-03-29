/*!
 * @file hw.h
 * @author Alex Murray
 *
 * Created on 18 October 2015, 14:50
 */

#ifndef HW_H
#define	HW_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Other files should include hw.h instead of the following header */
#include "p33EP16GS506.h"

/*
 * _____________________________________________________________________________
 *                              Port Mappings
 * -------+--------------+-----------+------------------------------------------
 *  Port  |  HW name     | Function  | Description
 * _______|______________|___________|__________________________________________
 *  RA.0  | BUCK_IMEAS_N | PGA1P1    | Negative differential input from current
 *        |              |           | sense resistor on output.
 *        |              |           | Biased to 3.3/2V.
 * -------+--------------+-----------+------------------------------------------
 *  RA.1  | BUCK_UMEAS   | AN1       | Feedback voltage from output voltage
 *        |              |           | divider. Scaled to 0..1.5V. Use VREF from
 *        |              |           | AN8 to derive an accurate value of the
 *        |              |           | output voltage, as the 3.3 rail is
 *        |              |           | unstable.
 * -------+--------------+-----------+------------------------------------------
 *  RA.2  | BUCK_EN      | GPIO/O    | When high, the buck is enabled. If the
 *        |              |           | 36V rail sinks below 25V, the buck is
 *        |              |           | automatically disabled and an interrupt
 *        |              |           | is generated over BUCK_UVLO.
 * -------+--------------+-----------+------------------------------------------
 *  RB.2  | BUCK_IMEAS_P | PGA1N2    | Positive differential input from current
 *        |              |           | sense resistor on output. Biased to
 *        |              |           | 3.3V/2V. Will change linearly in function
 *        |              |           | with output current where 3A = 15mV.
 * -------+--------------+-----------+------------------------------------------
 *  RB.3  | BUCK_USET    | DACOUT1   | Sets the regulated output voltage.
 *        |              |           | Use EXTREF1 for higher accuracy.
 * -------+--------------+-----------+------------------------------------------
 *  RB.8  | LCD_SDA      | SDA2      | I2C SDA line for LCD
 * -------+--------------+-----------+------------------------------------------
 *  RB.11 | LCD_RESET    | GPIO/O    | Active low reset
 * -------+--------------+-----------+------------------------------------------
 *  RB.15 | LCD_SCL      | SCL2      | I2C SCL line for LCD
 * -------+--------------+-----------+------------------------------------------
 *  RC.1  | VREF         | AN8       | 1.5V reference voltage can be sampled
 * -------+--------------+-----------+------------------------------------------
 *  RC.2  | VREF         | EXTREF1   | 1.5V reference voltage input for DACs
 * -------+--------------+-----------+------------------------------------------
 *  RC.3  | LED0         | PWM5L     | PWM controlled LED
 * -------+--------------+-----------+------------------------------------------
 *  RC.4  | KNOB_A       | CNPUC4    | Interrupt for rotary encoder, line A
 * -------+--------------+-----------+------------------------------------------
 *  RC.5  | KNOB_B       | CNPUC5    | Interrupt for rotary encoder, line B
 * -------+--------------+-----------+------------------------------------------
 *  RC.6  | KNOB_BTN     | CNPUC6    | Interrupt for button press/release
 * -------+--------------+-----------+------------------------------------------
 *  RC.9  | BUCK_UVLO    | CNPUC9    | Goes high during power failure.
 * -------+--------------+-----------+------------------------------------------
 *  RC.11 | RX           | RP59/UART | UART receive
 * -------+--------------+-----------+------------------------------------------
 *  RC.12 | TX           | RP60/UART | UART transmit
 * -------+--------------+-----------+------------------------------------------
 *  RC.10 | CTS          | RP58/UART | UART Clear to Send TODO: Fix
 * -------+--------------+-----------+------------------------------------------
 *  RC.10 | RTS          | RP61/UART | UART Clear to Receive TODO: Fix
 * -------+--------------+-----------+------------------------------------------
 *  RD.1  | LED3         | PWM4H     | PWM controlled LED
 * -------+--------------+-----------+------------------------------------------
 *  RD.3  | LED2         | PWM4L     | PWM controlled LED
 * -------+--------------+-----------+------------------------------------------
 *  RD.6  | LED1         | PWM5H     | PWM controlled LED
 * -------+--------------+-----------+------------------------------------------
 *  RD.13 | BUCK_ISET    | DACOUT2   | Sets the maximum regulated current.
 *        |              |           | Use EXTREF1 for higher accuracy.
 * -------+--------------+-----------+------------------------------------------
 */

#define BIT0  (1 << 0 )
#define BIT1  (1 << 1 )
#define BIT2  (1 << 2 )
#define BIT3  (1 << 3 )
#define BIT4  (1 << 4 )
#define BIT5  (1 << 5 )
#define BIT6  (1 << 6 )
#define BIT7  (1 << 7 )
#define BIT8  (1 << 8 )
#define BIT9  (1 << 9 )
#define BIT10 (1 << 10)
#define BIT11 (1 << 11)
#define BIT12 (1 << 12)
#define BIT13 (1 << 13)
#define BIT14 (1 << 14)
#define BIT15 (1 << 15)

/* mappings */
#define LCD_RESET           PORTBbits.RB11
#define LCD_ADDRESS         0x00
#define BUCK_EN             PORTAbits.RA2
#define KNOB_AB             ((PORTC & 0x30) >> 4)
#define KNOB_BUTTON         ((PORTC & BIT6) >> 6)

/* macros */
#define lcd_enable()        (LCD_RESET = 1)
#define lcd_disable()       (LCD_RESET = 0)
#define unlock_registers()  (__builtin_write_OSCCONL(OSCCON & ~BIT6))
#define lock_registers()    (__builtin_write_OSCCONL(OSCCON |  BIT6))

/* interrupt related things */
#define _ISR_NOPSV __attribute__((__interrupt__,no_auto_psv))
#define enable_interrupts() (_GIE = 1)
#define disable_interrupts()(_GIE = 0)

/* Among other things, required for UART
 * Fosc = 120 MHz, FP = Fosc / 2, according to Fig 1-1 in Oscillator Module
 * Documentation. See also init_sysclk60mips() in hw.c */
#define FCY 60000000

/*!
 * @brief Initialises and configures the hardware. Must be the first thing
 * called.
 */
void hw_init(void);

/*!
 * @brief Initialises all drivers.
 */
void drivers_init(void);

/*!
 * @brief De-initialises all drivers.
 */
void drivers_deinit(void);

#ifdef	__cplusplus
}
#endif

#endif	/* HW_H */
