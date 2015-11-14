/*!
 * @file main.c
 * @author Alex Murray
 * 
 * Created on 18 October 2015, 14:34
 */

#include "drv/init_hw.h"
#include "drv/button.h"
#include "event.h"

/* -------------------------------------------------------------------------- */
int main(void)
{
    init_device();
    event_init();
    button_init();
    
    while(1)
    {
        event_process_all();
    }
}
