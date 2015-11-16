/*! 
 * @file timer.c
 * @author Alex Murray
 *
 * Created on 15 November 2015, 02:10
 */

#include "drv/timer.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stddef.h>

/* -------------------------------------------------------------------------- */
/* 10ms timer interrupt */
void _ISR_NOPSV _T4Interrupt(void)
{
    event_post(EVENT_UPDATE, NULL);
    
    /* clear interrupt flag */
    IFS1bits.T4IF = 0;
}
