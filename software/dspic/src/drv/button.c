/*!
 * @file button.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include "drv/button.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stdlib.h>

#define TIME_THRESHOLD_IN_MILLISECONDS 600

#define TIME_THRESHOLD \
    TIME_THRESHOLD_IN_MILLISECONDS / 10

volatile static unsigned char button_timer = 0;

static void on_update(unsigned int arg);

/* -------------------------------------------------------------------------- */
void button_init(void)
{
    /*
     * Twist/push button (bit 4, 5 and 6) are digital input signals. Because
     * all pins are configured as analog inputs by default,  clear analog
     * flags.
     */
    ANSELC &= ~0x0070;

    /* twist/push button has three wires that need pull-ups */
    CNPUC |= 0x0070;     /* bit 4, 5, 6 */

    /* configure encoder and button to trigger interrupts on change */
    CNENC |= 0x70;       /* enable interrupts for bits 4, 5, and 6 */
    IFS1bits.CNIF = 0;   /* clear interrupt flag for change notifications */
    IEC1bits.CNIE = 1;   /* enable change notification interrupts */

    /* listen to 10ms update events, required for "long press" timings of
     * the button */
    event_register_listener(EVENT_UPDATE, on_update);
}

/* -------------------------------------------------------------------------- */
static void on_update(unsigned int arg)
{
    /* If the timer is active (non-zero) and hasn't reached the max value yet,
     * increment it */
    if(button_timer)
        ++button_timer;
    if(button_timer > TIME_THRESHOLD)
    {
        event_post(EVENT_BUTTON, BUTTON_PRESSED_LONGER);
        button_timer = 0;
    }
}

/* -------------------------------------------------------------------------- */
static void process_press_event(void)
{
    /* Was the button pressed? (falling edge) */
    if(!KNOB_BUTTON)
    {
        event_post(EVENT_BUTTON, BUTTON_PRESSED);
        button_timer = 1; /* start timer - gets incremented on EVENT_UPDATE */
    /* was the button released? (rising edge) */
    } else if(button_timer) {
        event_post(EVENT_BUTTON, BUTTON_RELEASED);
        button_timer = 0; /* stop timer on release */
    }
}

/* -------------------------------------------------------------------------- */
static void process_twist_event(void)
{
    static unsigned char current_AB = 0;
    static unsigned char ticks = 0;

    /*
     * Read the new values of A and B.
     * 2-bit gray-code can be converted to binary by flipping the last bit,
     * if the number is greater than 1.
     * 00
     * 01 _
     * 11  | Flip these
     * 10 _|
     */
    unsigned char AB = KNOB_AB;  /* read from register */
    AB ^= (AB >> 1); /* flip bit0 if bit1 is set */

    /* Was the knob even twisted? */
    if(current_AB == AB)
        return;

    /*
     * Determine the direction the knob was twisted and post an event
     * accordingly.
     */
    if((AB > current_AB ||
         (current_AB == 3 && AB == 0)) &&
        !(current_AB == 0 && AB == 3))
    {
        ticks = (ticks + 1) & 0x3;
        if(ticks == 0)
            event_post(EVENT_BUTTON, BUTTON_TWISTED_RIGHT);
    } else {
        ticks = (ticks - 1) & 0x3;
        if(ticks == 2)
            event_post(EVENT_BUTTON, BUTTON_TWISTED_LEFT);
    }

    /* update current AB */
    current_AB = AB;
}

/* -------------------------------------------------------------------------- */
/* called when the button is twisted (A or B changed) */
void _ISR_NOPSV _CNInterrupt(void)
{
    process_press_event();
    process_twist_event();

    /* clear interrupt flag */
    IFS1bits.CNIF = 0;
}

/* -------------------------------------------------------------------------- */
/* Unit tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

int button_action;
static void test_callback(unsigned int arg)
{
    button_action = (int)arg;
}

/* -------------------------------------------------------------------------- */
class button : public Test
{
    virtual void SetUp()
    {
        /* re-initialise events and button */
        event_deinit();
        button_init();
        button_action = 0;

        /* By default, the button isn't pressed, which means BIT6 is high and
         * all other bits are low */
        PORTC = BIT6;
    }

    virtual void TearDown()
    {
    }
};

static void twist_button_left()
{
    unsigned int AB = KNOB_AB;
    switch(AB) {
        case 0 : PORTC |=  BIT4; break;
        case 1 : PORTC |=  BIT5; break;
        case 3 : PORTC &= ~BIT4; break;
        case 2 : PORTC &= ~BIT5; break;
    }
    _CNInterrupt();
    event_dispatch_all();
}

static void twist_button_right()
{
    unsigned int AB = KNOB_AB;
    switch(AB) {
        case 0 : PORTC |=  BIT5; break;
        case 2 : PORTC |=  BIT4; break;
        case 3 : PORTC &= ~BIT5; break;
        case 1 : PORTC &= ~BIT4; break;
    }
    _CNInterrupt();
    event_dispatch_all();
}

static void press_button()
{
    PORTC &= ~BIT6;
    _CNInterrupt();
    event_dispatch_all();
}

static void release_button()
{
    PORTC |= BIT6;
    _CNInterrupt();
    event_dispatch_all();
}

static void press_button_for(int time_to_pass_in_milliseconds)
{
    press_button();

    /* button uses EVENT_UPDATE to measure time. One EVENT_UPDATE = 10ms */
    int number_of_updates = time_to_pass_in_milliseconds / 10;
    for(int i = 0; i != number_of_updates; ++i)
    {
        event_post(EVENT_UPDATE, 0);
        event_dispatch_all();
    }
}

/* -------------------------------------------------------------------------- */
TEST_F(button, twist_left_posts_correct_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    twist_button_left();
    twist_button_left();
    twist_button_left();
    twist_button_left();

    twist_button_left();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_LEFT));
    twist_button_left();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_LEFT));
    twist_button_left();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_LEFT));
    twist_button_left();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_LEFT));
}

TEST_F(button, twist_right_posts_correct_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    twist_button_right();
    twist_button_right();
    twist_button_right();
    twist_button_right();

    twist_button_right();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_RIGHT));
    twist_button_right();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_RIGHT));
    twist_button_right();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_RIGHT));
    twist_button_right();
    EXPECT_THAT(button_action, Eq(BUTTON_TWISTED_RIGHT));
}

TEST_F(button, pressing_posts_correct_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    press_button();
    EXPECT_THAT(button_action, Eq(BUTTON_PRESSED));

    button_action = 0;
    release_button();
    EXPECT_THAT(button_action, Eq(BUTTON_RELEASED));

    press_button();
    EXPECT_THAT(button_action, Eq(BUTTON_PRESSED));
}

TEST_F(button, pressing_for_1_second_posts_correct_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    press_button_for(1000);

    EXPECT_THAT(button_action, Eq(BUTTON_PRESSED_LONGER));
}

TEST_F(button, pressing_for_1_second_and_releasing_doesnt_post_released_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    press_button_for(1000);
    release_button();

    EXPECT_THAT(button_action, Eq(BUTTON_PRESSED_LONGER));
}

TEST_F(button, pressing_for_100_milliseconds_doesnt_post_event)
{
    event_register_listener(EVENT_BUTTON, test_callback);

    press_button_for(100);
    release_button();

    /* Only expect the button press event, without the longer press */
    EXPECT_THAT(button_action, Eq(BUTTON_RELEASED));
}

#endif /* TESTING */
