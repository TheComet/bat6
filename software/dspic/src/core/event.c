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
 * to iterate over the listeners and call the associated callback functions.
 */

#include "core/event.h"
#include "drv/hw.h"
#include <stdlib.h>

/*
 * An object of this type is dynamically allocated when a listener registers.
 */
struct listener_t
{
    struct listener_t*  next;       /* intrusive linked list */
    event_listener_func callback;
};

/*
 * For every event ID that exists, a list object is created.
 */
struct listener_list_t
{
    struct listener_t*  head;
    struct listener_t*  tail;
};
static struct listener_list_t event_table[EVENT_COUNT] = {{0}};

/*
 * These are the type of objects stored at each index in the ring buffer.
 */
struct ring_buffer_data_t
{
    event_id_e                  event_id;
    unsigned int                arg;
};

/*
 * The actual ring buffer holds an array of ring_buffer_t objects and is
 * allocated statically. The buffer size should allow for the maximum number of
 * events that can be posted between each dispatch to fit.
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
void event_deinit(void)
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
void event_unregister_listener(event_id_e event_id,
        event_listener_func callback)
{
    struct listener_list_t* list;
    struct listener_t *listener, *previous_listener = NULL;

    /* search for matching item in linked list */
    list = (event_table + event_id);
    for(listener = list->head;
        listener;
        previous_listener = listener, listener = listener->next)
    {
        if(listener->callback == callback)
        {
            /* unlink item */
            if(previous_listener)  /* nothing to unlink if the item is first */
                previous_listener->next = listener->next;

            /* update list object head and tail */
            if(list->head == listener)          /* item was first in list */
                list->head = listener->next;    /* set next item as first */
            if(list->tail == listener)          /* item was last in list */
                list->tail = previous_listener; /* set previous item as last */

            /* listener is unlinked, safe to destroy */
            free(listener);

            return;
        }
    }
}

/* -------------------------------------------------------------------------- */
void event_post(event_id_e event_id, unsigned int arg)
{
    unsigned char write;

    /*
     * Acquire unique write position in queue. If the queue is full, ignore this
     * event and return.
     */
    disable_interrupts();
        write = ring_buffer.write + 1;
        write = (write == RING_BUFFER_SIZE ? 0 : write);

        if(write == ring_buffer.read)
        {
            /* buffer is full, discard this event */
            enable_interrupts();
            return;
        }

        ring_buffer.write = write;
    enable_interrupts();

    /* add event ID and arguments into queue */
    ring_buffer.data[write].event_id = event_id;
    ring_buffer.data[write].arg = arg;
}

/* -------------------------------------------------------------------------- */
void event_dispatch_all(void)
{
    unsigned char write, read;
    struct listener_t* listener;
    struct ring_buffer_data_t* data;
    struct listener_list_t* list;

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
		/* increment and wrap read position */
        ++read;
        read = (read == RING_BUFFER_SIZE ? 0 : read);

        /* iterate list of listeners for the current event ID */
        data = (ring_buffer.data + read);
        list = (event_table + data->event_id);
        for(listener = list->head; listener; listener = listener->next)
        {
            listener->callback(data->arg);
        }
    }

    /* update read position */
    ring_buffer.read = write;
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifndef TESTING
#define TESTING
#endif

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

void test_listener1(unsigned int arg)
{
}

void test_listener2(unsigned int arg)
{
}

void test_listener3(unsigned int arg)
{
}

static int times_called;
static int last_argument_received;
void counting_listener(unsigned int arg)
{
	times_called++;
	last_argument_received = arg;
}

class event : public Test
{
	virtual void SetUp()
	{
		/* Reset counters */
		times_called = 0;
		last_argument_received = 0;
	}

	virtual void TearDown()
	{
		/* clears all listeners and destroys pending events */
		event_deinit();
	}
};

TEST_F(event, listeners_are_cleared_on_deinit)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_deinit();

	EXPECT_THAT(event_table[EVENT_UPDATE].head, IsNull());
}

TEST_F(event, ring_buffer_is_cleared_on_deinit)
{
	event_post(EVENT_UPDATE, 0);
	event_post(EVENT_UPDATE, 0);

	EXPECT_THAT(ring_buffer.write, Eq(2));

	event_deinit();

	EXPECT_THAT(ring_buffer.read, Eq(0));
	EXPECT_THAT(ring_buffer.write, Eq(0));
}

TEST_F(event, items_are_correctly_linked_in_linked_list)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_register_listener(EVENT_UPDATE, test_listener2);
	event_register_listener(EVENT_UPDATE, test_listener3);

	struct listener_list_t* list = (event_table + EVENT_UPDATE);
	ASSERT_THAT(list->head, NotNull());
	EXPECT_THAT(list->head->callback, Eq(test_listener1));
	ASSERT_THAT(list->head->next, NotNull());
	EXPECT_THAT(list->head->next->callback, Eq(test_listener2));
	ASSERT_THAT(list->head->next->next, NotNull());
	EXPECT_THAT(list->head->next->next->callback, Eq(test_listener3));
	EXPECT_THAT(list->head->next->next->next, IsNull());
	ASSERT_THAT(list->tail, NotNull());
	EXPECT_THAT(list->tail->callback, Eq(test_listener3));
}

TEST_F(event, items_are_correctly_removed_from_middle_of_linked_list)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_register_listener(EVENT_UPDATE, test_listener2);
	event_register_listener(EVENT_UPDATE, test_listener3);
	event_unregister_listener(EVENT_UPDATE, test_listener2);

	struct listener_list_t* list = (event_table + EVENT_UPDATE);
	ASSERT_THAT(list->head, NotNull());
	EXPECT_THAT(list->head->callback, Eq(test_listener1));
	ASSERT_THAT(list->head->next, NotNull());
	EXPECT_THAT(list->head->next->callback, Eq(test_listener3));
	ASSERT_THAT(list->tail, NotNull());
	EXPECT_THAT(list->tail->callback, Eq(test_listener3));
}

TEST_F(event, items_are_correctly_removed_from_head_of_linked_list)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_register_listener(EVENT_UPDATE, test_listener2);
	event_register_listener(EVENT_UPDATE, test_listener3);
	event_unregister_listener(EVENT_UPDATE, test_listener1);

	struct listener_list_t* list = (event_table + EVENT_UPDATE);
	ASSERT_THAT(list->head, NotNull());
	EXPECT_THAT(list->head->callback, Eq(test_listener2));
	ASSERT_THAT(list->head->next, NotNull());
	EXPECT_THAT(list->head->next->callback, Eq(test_listener3));
	ASSERT_THAT(list->tail, NotNull());
	EXPECT_THAT(list->tail->callback, Eq(test_listener3));
}

TEST_F(event, items_are_correctly_removed_from_tail_of_linked_list)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_register_listener(EVENT_UPDATE, test_listener2);
	event_register_listener(EVENT_UPDATE, test_listener3);
	event_unregister_listener(EVENT_UPDATE, test_listener3);

	struct listener_list_t* list = (event_table + EVENT_UPDATE);
	ASSERT_THAT(list->head, NotNull());
	EXPECT_THAT(list->head->callback, Eq(test_listener1));
	ASSERT_THAT(list->head->next, NotNull());
	EXPECT_THAT(list->head->next->callback, Eq(test_listener2));
	ASSERT_THAT(list->tail, NotNull());
	EXPECT_THAT(list->tail->callback, Eq(test_listener2));
}

TEST_F(event, last_item_is_correctly_removed_from_linked_list)
{
	event_register_listener(EVENT_UPDATE, test_listener1);
	event_unregister_listener(EVENT_UPDATE, test_listener1);

	struct listener_list_t* list = (event_table + EVENT_UPDATE);
	EXPECT_THAT(list->head, IsNull());
	EXPECT_THAT(list->tail, IsNull());
}

TEST_F(event, listener_gets_called_on_dispatch)
{
	event_register_listener(EVENT_UPDATE, counting_listener);
	event_post(EVENT_UPDATE, 8);
	event_dispatch_all();

	EXPECT_THAT(times_called, Eq(1));
	EXPECT_THAT(last_argument_received, Eq(8));
}

TEST_F(event, multiple_listeners_get_called_on_dispatch)
{
	event_register_listener(EVENT_UPDATE, counting_listener);
	event_register_listener(EVENT_UPDATE, counting_listener);
	event_register_listener(EVENT_UPDATE, counting_listener);
	event_register_listener(EVENT_UPDATE, counting_listener);
	event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	EXPECT_THAT(times_called, Eq(4));
}

TEST_F(event, ring_buffer_overflow_discards_incoming_events_when_posted)
{
	/* post some events and dispatch so the ring buffer wrap is included in
	 * this test */
	for(int i = 0; i != RING_BUFFER_SIZE / 4; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	/* test begins here */
	event_register_listener(EVENT_UPDATE, counting_listener);
	for(int i = 0; i != RING_BUFFER_SIZE + 2; ++i) /* 2 more than what can be stored */
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	/* One slot doesn't get filled due to the way an overflow is detected */
	EXPECT_THAT(times_called, Eq(RING_BUFFER_SIZE - 1));
}

TEST_F(event, ring_buffer_wraps_correctly)
{
	event_register_listener(EVENT_UPDATE, counting_listener);

	for(int i = 0; i != RING_BUFFER_SIZE / 2; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	for(int i = 0; i != RING_BUFFER_SIZE / 4; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	for(int i = 0; i != RING_BUFFER_SIZE - 1; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	for(int i = 0; i != RING_BUFFER_SIZE / 3; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	for(int i = 0; i != RING_BUFFER_SIZE * 2 / 3; ++i)
		event_post(EVENT_UPDATE, 0);
	event_dispatch_all();

	int number_of_posts = 0;
	number_of_posts += RING_BUFFER_SIZE / 2;
	number_of_posts += RING_BUFFER_SIZE / 4;
	number_of_posts += RING_BUFFER_SIZE - 1;
	number_of_posts += RING_BUFFER_SIZE / 3;
	number_of_posts += RING_BUFFER_SIZE * 2 / 3;
	EXPECT_THAT(times_called, Eq(number_of_posts));
}

#endif /* TESTING */
