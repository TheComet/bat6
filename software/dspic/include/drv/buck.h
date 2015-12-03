/*! 
 * @file buck.h
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#ifndef BUCK_H
#define	BUCK_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the buck driver. Call this for the buck to work.
 */
void buck_init(void);

void buck_enable();       
void buck_disable();     

_Q16 buck_get_voltage();
_Q16 buck_get_current();
void buck_set_voltage(_Q16 voltage);
void buck_set_current(_Q16 current);

#ifdef	__cplusplus
}
#endif

#endif	/* BUCK_H */
