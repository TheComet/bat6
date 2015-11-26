/*!
 * @file main.c
 * @author Alex Murray
 * 
 * Created on 18 October 2015, 14:34
 */

#include <libq.h>
#include <stdio.h>
#include "usr/pv_model.h"
#include "drv/hw.h"
#include "drv/leds.h"
#include "core/event.h"

/* -------------------------------------------------------------------------- */
int main(void)
{
    /* 
     * Event system must be initialised before hardware, because the hardware
     * drivers need to register listeners.
     */
    
    pv_cell_t cell = { _Q16ftoi(21.5), _Q16ftoi(3.5), _Q16ftoi(1.5522),
        _Q16ftoi(1.0)};
    
    _Q16 v_is = _Q16ftoi(15.0);
    _Q16 i_step = _Q16ftoi(0.1);
    _Q16 i_is = i_step;
    _Q16 i_max = _Q16ftoi(4);

    event_init();
    hw_init();
    
    while(1)
    {
        _Q16 v = calc_voltage(&cell, v_is, i_is);
        printf("%ld, %ld;\n", i_is, v);
        i_is += i_step;
        if(i_is > i_max)
            i_is = i_step;
        event_process_all();
    }
}
