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

static _Q16 Idiff(const pv_cell_t* cell, const _Q16 rl, const _Q16 vd){
    _Q16 id = Id(cell, vd);
    
    return id - _Q16div(vd, rl);
}

static _Q16 deltaIdiff(const pv_cell_t* cell, const _Q16 rl, const _Q16 vd){
    return _Q16neg(_Q16div(_Q16mpy(cell->isc, Io(cell, vd)), cell->vt))
            - _Q16reciprocalQ16(rl);
}

_Q16 calc_voltage(const pv_cell_t* cell, const _Q16 v_is, const _Q16 i_is){
    const _Q16 rl = _Q16div(v_is, i_is);
    
    _Q16 v_next = v_is;
    
    int i;
    for(i = 0; i<32; i++){
        _Q16 I_diff = Idiff(cell,rl,v_next);
        _Q16 delta_I_diff = deltaIdiff(cell, rl, v_next);
        _Q16 delta_U = _Q16div(I_diff, delta_I_diff);
        v_next -= delta_U;
    }
    
    return v_next;
}