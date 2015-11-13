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
 * @brief This gets called by the change notification interrupt when any of the
 * pins related to the push/twist-button change.
 * 
 * The state of each pin is read from the appropriate registers in this
 * function.
 */
void notify_button_change(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTON_H */
