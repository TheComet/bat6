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

#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */
