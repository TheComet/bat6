/*! 
 * @file pv_model.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include <stdint.h>
#include <libq.h>
#include "usr/pv_model.h"

/* -------------------------------------------------------------------------- */

static _Q16 Io(const pv_cell_t* cell, const _Q16 vd){
    return _Q16exp(_Q16div(vd - cell->voc, cell->vt));
}

static _Q16 Id(const pv_cell_t* cell, const _Q16 vd){ 
    return _Q16mpy(cell->isc, cell->g - Io(cell, vd));
}

_Q16 calc_voltage(const pv_cell_t* cell, const _Q16 v_is, const _Q16 i_is){
    const _Q16 rl = _Q16div(v_is, i_is);
    
    _Q16 v_min = 0;
    _Q16 v_max = cell->voc;
    
    int i;
    for(i = 0; i<32; i++){
        _Q16 v_next = (v_min + v_max) / 2;
        
        if(Id(cell, v_next) > _Q16div(v_next, rl)){
            v_min = v_next;
        }else{
            v_max = v_next;
        }
    }
    
    return (v_min + v_max) / 2;
}