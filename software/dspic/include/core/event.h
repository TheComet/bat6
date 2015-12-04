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

#include "core/static_assert.h"

/*!
 * @brief Callback function type. All listeners need to implement this
 * signature.
 */
typedef void (*event_listener_func)(void* args);

/*!
 * @brief List of global events.
 * 
 * How to add your own event:
 *  1) Add your event name into the enum below.
 *  2) Call event_register_listener(YOUR_EVENT, listener_function) to add as
 *     many callbacks as you need.
 */
typedef enum
{
    /*! Gets posted every 10ms. Useful for time-critical things. */
    EVENT_UPDATE = 0,
    /*! Gets posted when the twist button is twisted, left or right. The
     *  direction is passed as an argument */
    EVENT_BUTTON_TWISTED,
    /*! Gets posted when the twist button is pressed (falling edge). */
    EVENT_BUTTON_PRESSED,
    /*! Gets posted when an undervoltage lockout is in progress. This usually
     *  means the device was unplugged, in which case the device has a short
     *  time to prepare for total power off. UVLO is triggered when the 36V
     *  rail sinks below 25V. The 3.3V rail remains stable until the 36V rail
     *  reaches ~4V */
    EVENT_UVLO,
    EVENT_DATA_RECEIVED,
    /* ---------------------------------------------------------------------- */
    /*! The number of event IDs. Used to size the static table. 
     *  NOTE: Keep this at the end of the enum! */
    EVENT_COUNT
} event_id_e;

/*!
 * @brief Additional constants that can be passed as arguments.
 */
typedef enum
{
    BUTTON_TWISTED_LEFT = 1,
    BUTTON_TWISTED_RIGHT = 2
} event_args_e;

/*!
 * @brief Initialises the event system. Call before using any other event
 * related functions.
 */
void event_init(void);

/*!
 * @brief Adds a callback function to the specified event's callback list.
 * @param[in] event_id The event ID to register to.
 * @param[in] callback The callback function that should get called when the
 * specified event gets posted.
 */
void event_register_listener(event_id_e event, event_listener_func callback);

/*!
 * @brief Removes a callback function from the specified event's callback list.
 * @param[in] event_id The event ID to unregister from.
 * @param[in] callback The callback function pointer to unregister.
 */
void event_unregister_listener(event_id_e event_id,
        event_listener_func callback);

/* see macro below for doc */
void event_post_(event_id_e event, void* args);

/*!
 * @brief Queues an event. This can be called from interrupts or from the main
 * thread.
 * @param[in] event The event ID to post. Event IDs are defined in the event
 * enum in event.h.
 * @param[in] event_data Optional event data. The data specified here gets
 * passed to the registered listener callback function.
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
