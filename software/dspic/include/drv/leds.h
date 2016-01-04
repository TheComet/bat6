/*!
 * @file leds.h
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialise LED driver. Call this for LEDs to work.
 */
void leds_init(void);

/*!
 * @brief Turns an LED on or off.
 * @param[in] led_id A number from 0 to 3.
 * @param[in] state Either 0 (for off) or 1 (for on).
 */
void led_set(unsigned char led_id, unsigned char state);

/*!
 * @brief Turns all LEDs either on or off.
 * @param[in] state Either 0 (for off) or 1 (for on).
 */
void led_all(unsigned char state);

#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */
