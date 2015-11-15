/*!
 * @file main.c
 * @author Alex Murray
 * 
 * Created on 18 October 2015, 14:34
 */

#include "drv/hw.h"
#include "drv/button.h"
#include "event.h"

/* -------------------------------------------------------------------------- */
int main(void)
{
    hw_init();
    event_init();
    button_init();
    
    while(1)
    {
        event_process_all();
    }
}
