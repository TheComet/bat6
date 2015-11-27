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
    uint16_t voc; /* open circuit voltage Q5.11*/
    uint16_t isc; /* short circuit current Q3.13*/
    uint16_t vt; /* thermal voltage Q3.13*/
    uint16_t g; /* relative solar iridation Q0.16*/
}pv_cell_t;

#ifdef	__cplusplus
}
#endif

#endif	/* PV_MODEL_H */
