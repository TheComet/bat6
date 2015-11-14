/*!
 * @file event.c
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 21:50
 */

#include "event.h"
#include "drv/hw.h"

/*
 * Currently, we only support one handler function for every event. This maps
 * each event ID in the table defined in event.h to a callback function.
 */
static event_handler_func event_table[EVENT_COUNT] = {0};

/* -------------------------------------------------------------------------- */
void event_register_handler(event_table_e event, event_handler_func callback)
{
    event_table[event] = callback;
}

/* -------------------------------------------------------------------------- */
void event_post_(event_table_e event, void* args)
{
    disable_interrupts();
    enable_interrupts();
}
