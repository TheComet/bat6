/*! 
 * @file pv_model.h
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#ifndef PV_MODEL_H
#define	PV_MODEL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct{
    _Q16 voc; /* open circuit voltage Q5.11*/
    _Q16 isc; /* short circuit current Q3.13*/
    _Q16 vt; /* thermal voltage Q3.13*/
    _Q16 g; /* relative solar iridation Q0.16*/
}pv_cell_t;

_Q16 calc_voltage(const pv_cell_t* cell, const _Q16 v_is, const _Q16 i_is);


#ifdef	__cplusplus
}
#endif

#endif	/* PV_MODEL_H */
