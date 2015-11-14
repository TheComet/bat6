/*!
 * @file init_hw.h
 * @author Alex Murray
 * 
 * Created on 18 October 2015, 14:34
 */

#ifndef INIT_H
#define	INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises and configures the hardware. Must be the first thing
 * called.
 */
void init_device(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */
