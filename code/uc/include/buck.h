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
 * @brief This gets called by the change notification interrupt when the UVLO
 * pin goes high.
 */
void notify_uvlo(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUCK_H */
