/*! 
 * @file timer.h
 * @author Alex Murray
 *
 * Created on 15 November 2015, 02:10
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the 10ms timer. Call this for EVENT_UPDATE events to be
 * posted every 10ms.
 */
void timer_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */
