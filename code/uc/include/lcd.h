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
 * @brief List of commands that can be sent to the LCD.
 * 
 * The following describes what each command does and what arguments can be
 * supplied in addition to the command, if any.
 *  + LCD_CLEAR_DISPLAY**
 *    - Clears all text from the display
 *  + LCD_RETURN_HOME**
 *    - Returns the cursor to its original position
 *  + LCD_ENTRY_MODE_SET
 *    - LCD_ENTRY_MODE_MOVE_CURSOR_RIGHT moves the cursor right
 *    - LCD_ENTRY_MODE_MOVE_CURSOR_LEFT moves the cursor left
 *    - LCD_ENTRY_MODE_ENABLE_ENTRY_SHIFT enables cursor shifting
 *    - LCD_ENTRY_MODE_DISABLE_ENTRY_SHFIT disables cursor shifting
 *  + LCD_ON_OFF_CTRL
 *    - LCD_ON_OFF_CTRL_DISPLAY_ON turns the display on
 *    - LCD_ON_OFF_CTRL_DISPLAY_OFF turns the display off
 *    - LCD_ON_OFF_CTRL_CURSOR_ON turns the cursor on
 *    - LCD_ON_OFF_CTRL_CURSOR_OFF turns the cursor off
 *    - LCD_ON_OFF_CTRL_BLINK_ON enables cursor blinking
 *    - LCD_ON_OFF_CTRL_BLINK_OFF disables cursor blinking
 */
typedef enum
{
    LCD_CLEAR_DISPLAY           = 0x01,
    LCD_RETURN_HOME             = 0x02,
    LCD_ENTRY_MODE_SET          = 0x04,
    LCD_ON_OFF_CTRL             = 0x08,
    LCD_SELECT_CHAR_GEN         = 0x72,
    LCD_ENABLE_OLED_CMD_SET     = 0x78,
    LCD_DISABLE_OLED_CMD_SET    = 0X79,
    
    LCD_CONTRAST                = 0x81,
    LCD_CFG_OSCILLATOR          = 0xD5,
    LCD_PHASE_LENGTH            = 0xD9
    
} lcd_command_e;

typedef enum
{
    LCD_NOARG                   = 0x00,

    LCD_MOVE_CURSOR_RIGHT       = 0x00,
    LCD_MOVE_CURSOR_LEFT        = 0x02,
    LCD_ENABLE_ENTRY_SHIFT      = 0x01,
    LCD_DISABLE_ENTRY_SHFIT     = 0x00,

    LCD_DISPLAY_ON              = 0x04,
    LCD_DISPLAY_OFF             = 0x00,
    LCD_CURSOR_ON               = 0x02,
    LCD_CURSOR_OFF              = 0x00,
    LCD_BLINK_ON                = 0x01,
    LCD_BLINK_OFF               = 0x00,

    LCD_CGROM240_CGRAM8         = 0x00,
    LCD_CGROM248_CGRAM8         = 0x01,
    LCD_CGROM250_CGRAM6         = 0x02,
    LCD_CGROM256_CGRAM0         = 0x03,
    LCD_ROM_A                   = 0x00,
    LCD_ROM_B                   = 0x04,
    LCD_ROM_C                   = 0x08
} lcd_argument_e;

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
