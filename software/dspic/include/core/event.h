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
typedef void (*event_listener_func)(unsigned int arg);

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
    EVENT_BUTTON,
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
    BUTTON_TWISTED_RIGHT = 2,
    BUTTON_PRESSED = 3,
    BUTTON_PRESSED_LONGER = 4,
    BUTTON_RELEASED = 5
} event_args_e;

/*!
 * @brief De-initialises the event system and cleans up all listeners.
 * @note Any pending events are lost.
 */
void event_deinit(void);

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

/*!
 * @brief Queues an event. This can be called from interrupts or from the main
 * thread.
 * @param[in] event_id The event ID to post. Event IDs are defined in the event
 * enum in event.h.
 * @param[in] arg Optional event data. The data specified here gets
 * passed to the registered listener callback function.
 */
void event_post(event_id_e event_id, unsigned int arg);

/*!
 * @brief Processes all queued events.
 */
void event_dispatch_all(void);

#ifdef	__cplusplus
}
#endif

#endif	/* EVENT_H */
