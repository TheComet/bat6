/*!
 * @file lcd.h
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the LCD driver. Call this before calling any other LCD
 * related functions.
 */
void lcd_init(void);

/*!
 * @brief Sends a command with optional argument to the LCD.
 * @param[in] command The command to send to the LCD
 * @param[in] argument The argument is command-specific. If a command does not
 * require an argument then LCD_NOARG should be specified.
 */
void lcd_write(lcd_command_e command, lcd_argument_e argument);

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */
