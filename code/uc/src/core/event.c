/*!
 * @file event.c
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 21:50
 * 
 * Here, a simple event queue is implemented using a static event table and
 * a dynamic linked list for storing event listeners.
 * 
 * Each event has a globally unique "event ID", which is defined in event.h in
 * the enum "event_id_e". A static table "event_table" is created, mapping each
 * event ID to a list object.
 * 
 * During run-time, listeners can register themselves to any of the list
 * objects by providing a callback function and the event ID they wish to
 * register to. A listener_t object is allocated and linked into the list, thus
 * storing the callback function pointer.
 * 
 * The event queue is implemented using a ring buffer. Each entry in the ring
 * buffer stores the event ID that was posted and optional arguments that were
 * passed at the time of posting.
 * 
 * When the time comes to process all events, we iterate over the ring buffer
 * and extract the event ID + arguments. We use the event ID to look up the
 * associated list object in the static event table. Said list object contains a
 * linked list of all listeners interested in postings of the event. We proceed
 * to iterate over the listeners and call the associated callback function.
 */

#include "core/event.h"
#include "drv/hw.h"
#include <stdlib.h>

/* 
 * An object of this type is dynamically allocated when a listener registers.
 */
struct listener_t
{
    struct listener_t*          next; /* intrusive linked list */
    event_listener_func         callback;
};

/* 
 * For every event ID that exists, a list object is created.
 */
struct listener_list_t
{
    struct listener_t* head;
    struct listener_t* tail;
};
static struct listener_list_t event_table[EVENT_COUNT] = {{0}};

/* 
 * These are the type of objects stored at each index in the ring buffer.
 */
struct ring_buffer_data_t
{
    event_id_e                  event_id;
    void*                       args;
};

/* 
 * The actual ring buffer. It tracks the read and write positions and statically
 * allocates an array of ring_buffer_data_t objects. The size should be set
 * so the maximum number of events that can be posted between each dispatch
 * still fit.
 */
#define RING_BUFFER_SIZE 64
static struct
{
    volatile unsigned char      read;
    volatile unsigned char      write;
    struct ring_buffer_data_t   data[RING_BUFFER_SIZE];
} ring_buffer = {0};

/* -------------------------------------------------------------------------- */
static void destroy_listeners(struct listener_list_t* list)
{
    struct listener_t* next;
    struct listener_t* listener = list->head;
    while(listener)
    {
        next = listener->next;
        free(listener);
        listener = next;
    }
    
    list->head = NULL;
    list->tail = NULL;
}

/* -------------------------------------------------------------------------- */
void event_init(void)
{
    /* clear all lists of listeners */
    unsigned short i = EVENT_COUNT;
    while(i --> 0)
    {
        destroy_listeners(event_table + i);
    }
    
    ring_buffer.read = 0;
    ring_buffer.write = 0;
}

/* -------------------------------------------------------------------------- */
void event_register_listener(event_id_e event_id, event_listener_func callback)
{
    /* allocate listener object - if no more memory is available, return */
    struct listener_list_t* list;
    struct listener_t* listener = (struct listener_t*)malloc(sizeof *listener);
    if(!listener)
        return;
    
    listener->next = NULL;
    listener->callback = callback;
    
    /* link in to list */
    list = event_table + event_id;
    if(list->tail) /* set "next" of current tail to new item, if it exists */
        list->tail->next = listener; 
    else  /* if list is empty, set head to current item */
        list->head = listener;
    list->tail = listener; /* update tail */
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
        write = ring_buffer.write + 1;
        write = (write == RING_BUFFER_SIZE ? 0 : write);
        if(write == ring_buffer.read)
        {
            /* buffer is full, discard this event */
            enable_interrupts();
            return;
        }
        ring_buffer.write = write;
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
    struct listener_list_t* listener_list;
    
    /* 
     * Copy write position, as it could change during event processing.
     * Read position is only ever changed in this function, however,
     * it is declared volatile which makes incrementing it correctly more
     * complicated.
     */
    read = ring_buffer.read;
    write = ring_buffer.write;

    /* process all events up to the write position we acquired */
    while(read != write)
    {
        /* look up data in ring buffer according to read position */
        data = (ring_buffer.data + read);
        
        /* look up list of listeners associated with event ID */
        listener_list = event_table + data->event_id;
        
        /* iterate list of listeners */
        for(listener = listener_list->head; listener; listener = listener->next)
        {
            listener->callback(data->args);
        }
        
        /* increment and wrap read position */
        ++read;
        read = (read == RING_BUFFER_SIZE ? 0 : read);
    }
    
    /* update read position */
    ring_buffer.read = write;
}
