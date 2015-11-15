/*!
 * @file event.h
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 21:50
 */

#ifndef EVENT_H
#define	EVENT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "util/static_assert.h"

/*!
 * @brief Callback function type signature.
 */
typedef void (*event_handler_func)(void* args);

/*!
 * @brief List of global events.
 * 
 * How to add your own event:
 *  1) Add your event name into the enum below.
 *  2) Write a handler function for the event
 *  3) Call event_register_handler(YOUR_EVENT, handler_function)
 */
typedef enum
{
    EVENT_BUTTON_TWISTED = 0,
    EVENT_BUTTON_PRESSED = 1,
    EVENT_COUNT
} event_id_e;

/*!
 * @brief Additional constants that can be passed as arguments.
 */
typedef enum
{
    BUTTON_TWISTED_LEFT = 0,
    BUTTON_TWISTED_RIGHT = 1
} event_args_e;

/*!
 * @brief Initialises the event system. Call before using any other event
 * related functions.
 */
void event_init(void);

/*!
 * @brief Assigns a callback function to one of the entries in event_e
 * @param[in] event An event in the event table to map.
 * @param[in] callback The callback function that should be called when the
 * specified event gets posted.
 */
void event_register_handler(event_id_e event, event_handler_func callback);

/* see macro below for doc */
void event_post_(event_id_e event, void* args);

/*!
 * @brief Queues an event.
 * @param[in] event Must be one of the events specified in the event table.
 * @param[in] event_data Optional event data. The data specified here gets
 * passed to the registered handler function.
 */
#define event_post(event, event_data) do {              \
    static_assert(sizeof(event_data) <= sizeof(void*)); \
    event_post_(event, event_data);                     \
} while(0)

/*!
 * @brief Processes all queued events.
 */
void event_process_all(void);

#ifdef	__cplusplus
}
#endif

#endif	/* EVENT_H */