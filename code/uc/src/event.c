/*!
 * @file event.c
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 21:50
 */

#include "event.h"
#include "drv/hw.h"
#include "util/linked_list.h"
#include <stdlib.h>

struct listener_t
{
    struct listener_t*          next; /* intrusive linked list */
    event_listener_func         callback;
};
static struct linked_list_t* event_table[EVENT_COUNT] = {0};

struct ring_buffer_data_t
{
    event_id_e                  event_id;
    void*                       args;
};

#define RING_BUFFER_SIZE 32
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
        if(event_table[i])
            linked_list_destroy(event_table[i]);
        
        event_table[i] = linked_list_create();
    }
    
    ring_buffer.read = 0;
    ring_buffer.write = 0;
}

/* -------------------------------------------------------------------------- */
void event_register_listener(event_id_e event, event_listener_func callback)
{
    struct listener_t* listener = (struct listener_t*)malloc(sizeof *listener);
    if(!listener)
        return;
    
    listener->callback = callback;
    linked_list_push(event_table[event], (struct linked_list_node_t*)listener);
}

/* -------------------------------------------------------------------------- */
void event_post_(event_id_e event_id, void* args)
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
    
    /* add event ID and arguments into queue */
    ring_buffer.data[write].event_id = event_id;
    ring_buffer.data[write].args = args;
}

/* -------------------------------------------------------------------------- */
void event_process_all(void)
{
    unsigned char write, read;
    struct listener_t* listener;
    struct ring_buffer_data_t* data;
    
    /* 
     * Copy write position, as it could change during event processing.
     */
    read = ring_buffer.read;
    write = ring_buffer.write;
  
    /* process all events up to the write position we acquired */
    while(read != write)
    {
        /* get data from ring buffer */
        data = (ring_buffer.data + ring_buffer.read);
        
        /* look up linked list of listeners associated with event ID */
        struct linked_list_t* listeners = event_table[data->event_id];
        
        /* iterate list of listeners*/
        for(listener = (struct listener_t*)listeners->head;
                listener;
                listener = listener->next)
        {
            listener->callback(data->args);
        }
        
        /* increment and wrap read position */
        read = (read == RING_BUFFER_SIZE ? 0 : read + 1);
    }
    
    /* update read position */
    ring_buffer.read = write;
}
