/*!
 * @file event.c
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 21:50
 */

#include "event.h"
#include "drv/hw.h"
#include "stdlib.h" /* for NULL definition */

/*
 * Currently, we only support one handler function for every event. This maps
 * each event ID in the table defined in event.h to a callback function.
 */
static event_handler_func event_table[EVENT_COUNT] = {0};

#define RING_BUFFER_SIZE 32
struct ring_buffer_data_t
{
    event_handler_func          callback;
    void*                       args;
};
static struct
{
    volatile unsigned char      read;
    volatile unsigned char      write;
    struct ring_buffer_data_t   data[RING_BUFFER_SIZE];
} ring_buffer = {0};

/* -------------------------------------------------------------------------- */
void event_init(void)
{
    unsigned short i = EVENT_COUNT;
    while(i --> 0)
    {
        event_table[i] = NULL;
    }
    
    ring_buffer.read = 0;
    ring_buffer.write = 0;
}

/* -------------------------------------------------------------------------- */
void event_register_handler(event_id_e event, event_handler_func callback)
{
    event_table[event] = callback;
}

/* -------------------------------------------------------------------------- */
void event_post_(event_id_e event, void* args)
{
    unsigned char write;
    
    /* 
     * Aquire unique write position in queue. If the queue is full, ignore this
     * event and return.
     */
    disable_interrupts();
    {
        write = ring_buffer.write;
        if(write == ring_buffer.read)
        {
            enable_interrupts();
            return;
        }
        ++ring_buffer.write;
    }
    enable_interrupts();
    
    /* add callback function and arguments into queue */
    ring_buffer.data[write].callback = event_table[event];
    ring_buffer.data[write].args = args;
}

/* -------------------------------------------------------------------------- */
void event_process_all(void)
{
    unsigned char write, read;
    
    /* 
     * Copy write position, as it could change during event processing.
     */
    read = ring_buffer.read;
    write = ring_buffer.write;
  
    /* process all events up to the write position we acquired */
    while(read != write)
    {
        /* call handler */
        ring_buffer.data[ring_buffer.read].callback( \
                ring_buffer.data[ring_buffer.read].args);
        
        /* increment and wrap read position */
        read = (read == RING_BUFFER_SIZE ? 0 : read + 1);
    }
    
    /* update read position */
    ring_buffer.read = write;
}
