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

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */
