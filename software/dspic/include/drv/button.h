/*! 
 * @file button.h
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#ifndef BUTTON_H
#define	BUTTON_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the button driver. Call this for events related to button
 * twists/presses to work.
 */
void button_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTON_H */
