/*!
 * @file uart.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/uart.h"
#include "drv/hw.h"
#include "core/event.h"

/* Based on Example 5-1 in UART pdf */
/* Calculates value to write to register for the specified baud rate */
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1

/* Sets the size of the send queue */
#define TRANSMIT_QUEUE_SIZE (unsigned)64

/* Enables or disables transmit interrupt */
#define disable_tx_interrupt() (IEC0bits.U1TXIE = 0)
#define enable_tx_interrupt()  (IEC0bits.U1TXIE = 1)

/*
 * In order to test some of the concurrency situations present in the transmit
 * queue, we need to call an "update" function while uart_send() is blocking.
 */
#ifdef TESTING
static void tx_send_update(); /* is implemented in the tests below */
#   define TX_SEND_UPDATE() \
            (tx_send_update())
#else
#   define TX_SEND_UPDATE()
#endif

static void process_incoming_data(unsigned int data);
static void configure_pins(void);
static void configure_uart(void);
static void send_next_byte(void);

typedef enum
{
    STATE_IDLE,
    STATE_SELECT_CELL,
    /* awaits a selector for what is to be done to a cell*/
    STATE_AWAIT_CELL_CONFIG,
    STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE,
    STATE_CONFIG_SHORT_CIRCUIT_CURRENT,
    STATE_CONFIG_TEMPERATURE,
    STATE_CONFIG_EXPOSURE,
} state_e;

typedef enum
{
    CASE_SELECT_CELL = 'm',
} case_e;

struct data_t {
    union
    {
        struct {
            unsigned char selected_cell;  /* number representing the cell */
            unsigned char selected_param; /* defines whether param holds value
                                           * for Current, Voltage, Temperature
                                           * or Exposure */
            unsigned int param;           /* holds numerical value
                                           * UNITS:
                                           * I: milli ampere
                                           * U: milli volt
                                           * T: degrees celsius
                                           * E: percent */
        } config_cell;
    };
};

struct ring_buffer_t
{
    volatile unsigned char read;
    unsigned char write;
    unsigned char data[TRANSMIT_QUEUE_SIZE];
};

static state_e              state       = STATE_IDLE;
static struct data_t        state_data;
static struct ring_buffer_t transmit_queue  = {};

/* -------------------------------------------------------------------------- */
void uart_init(void)
{
    configure_pins();
    configure_uart();

    event_register_listener(EVENT_DATA_RECEIVED, process_incoming_data);
}

/* -------------------------------------------------------------------------- */
void uart_send(const unsigned char* bytes, unsigned short len)
{
    /*
     * Note: The assumption is that this never gets called from an interrupt,
     * only from event handlers.
     */

    /* sets "write" to the next write position in the ring buffer */
#define get_next_write_position(write) do {                         \
        write = transmit_queue.write + 1;                               \
        write = (write == TRANSMIT_QUEUE_SIZE ? 0 : write); } while(0)

    /* sends the first byte in the queue if the TX buffer is idle */
#define try_send_first_byte() do {        \
        disable_tx_interrupt();           \
            if(U1STAbits.TRMT)            \
                send_next_byte();         \
        enable_tx_interrupt(); } while(0)

    while(len --> 0)
    {
        /* increment and wrap write position */
        unsigned char write;
        get_next_write_position(write);

        /* if queue is full, block until space frees up */
        while(write == transmit_queue.read)
        {
            try_send_first_byte(); /* send, so buffer is emptied */
            while(write == transmit_queue.read) { TX_SEND_UPDATE(); }
            get_next_write_position(write);
        }

        /* add data to queue and update write position */
        transmit_queue.data[write] = *bytes++;
        transmit_queue.write = write;
    }

    /*
     * At this point the data is in the queue, but we don't know if the TX
     * interrupt is in the process of reading the buffer or not. If the
     * transmit buffer is empty, it should be safe to assume it isn't, in
     * which case we must manually trigger it to send.
     */
    try_send_first_byte();
}

/* -------------------------------------------------------------------------- */
static void configure_pins(void)
{
    /* See also Example 10-2 from I/O Ports pdf.*/

    ANSELCbits.ANSC10 = 0;    /* Set RP58 to digital */
    ANSELCbits.ANSC11 = 0;    /* Set RP59 to digital */
    ANSELCbits.ANSC12 = 0;    /* set RP60 to digital */
                              /* RP61 seems to already be digital. */

    unlock_registers();

        RPINR18bits.U1RXR = 59;   /* Assign U1Rx to Pin RP59 */
        RPINR18bits.U1CTSR = 58;  /* Assign U1TCS to Pin RP58 */

        RPOR14bits.RP60R = 1;     /* Assign U1Tx to Pin RP60, see p.10-11 I/O
                                   * ports documentation */
        RPOR14bits.RP61R = 3;     /* Assign U1RTS to RP61 */
        CNPUCbits.CNPUC11 = 1;    /* RX requires pull-up */

    lock_registers();
}

/* -------------------------------------------------------------------------- */
static void configure_uart(void)
{
    /* Example 5-1 from UART manual pdf */
    U1MODEbits.STSEL = 0;     /* 1 stop bit */
    U1MODEbits.PDSEL = 1;     /* 8-bit data, even parity */
    U1MODEbits.ABAUD = 0;     /* Auto-Baud disabled */
    U1MODEbits.BRGH = 0;      /* Standard-Speech mode */
    U1MODEbits.RXINV = 1;     /* Invert RX (due to isolating IC) */
    U1STAbits.TXINV = 1;      /* Invert TX (due to isolating IC) */

    U1BRG = BRGVAL;           /* baud rate setting, see #defines at top */

    U1STAbits.UTXISEL0 = 0;   /* interrupt after one Tx char is transmitted */
    U1STAbits.UTXISEL1 = 0;

    U1MODEbits.UARTEN = 1;    /* enable UART */
    U1STAbits.UTXEN = 1;      /* enable UART TX */

    IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 1;      /* enable TX interrupt */

    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;      /* enable RX interrupt */
}

/* -------------------------------------------------------------------------- */
static void process_incoming_data(unsigned int data)
{

#define CHAR_TO_INT(x) ((unsigned short)(x - '0'))

    switch (state)
    {
        case STATE_IDLE:
            if (data == CASE_SELECT_CELL)
            {
                state_data.config_cell.selected_cell = 0;
                state_data.config_cell.param = 0;
                state = STATE_SELECT_CELL;
            }
            break;

        case STATE_SELECT_CELL:
            if (data >= '0' && data <= '9')
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.selected_cell *= 10;
                state_data.config_cell.selected_cell += CHAR_TO_INT(data);

                /*
                 * Hijack param as a flag to indicate if a number was received
                 * or not so we can detect the error case when two non-numbers
                 * are sent consecutively.
                 */
                state_data.config_cell.param = 1;
            } else if (state_data.config_cell.param == 0) {
                /*
                 * Letter directly followed by another letter: error,
                 * revert back to idle state
                 */
                state = STATE_IDLE;
            } else {
                state = STATE_AWAIT_CELL_CONFIG;
            }
            break;

        case STATE_AWAIT_CELL_CONFIG:
            /*
             * We can configure current, voltage, temp or exposure. Anything
             * else is an error and results in reverting back to idle state.
             */
            switch (data)
            {
                case 'I':
                    state_data.config_cell.selected_param = data;
                    state = STATE_CONFIG_SHORT_CIRCUIT_CURRENT;
                    break;

                case 'U':
                    state_data.config_cell.selected_param = data;
                    state = STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE;
                    break;

                case 'T':
                    state_data.config_cell.selected_param = data;
                    state = STATE_CONFIG_TEMPERATURE;
                    break;

                case 'E':
                    state_data.config_cell.selected_param = data;
                    state = STATE_CONFIG_EXPOSURE;
                    break;

                default:
                    state = STATE_IDLE;
                    break;
            }
            break;

        case STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE:
            break;

        case STATE_CONFIG_SHORT_CIRCUIT_CURRENT:
            break;

        case STATE_CONFIG_TEMPERATURE:
            break;

        case STATE_CONFIG_EXPOSURE:
            break;

        default:
            state = STATE_IDLE;
            break;
    }
}


/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1RXInterrupt(void)
{
    event_post(EVENT_DATA_RECEIVED, U1RXREG);

    /* clear interrupt flag */
    IFS0bits.U1RXIF = 0;
}

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1TXInterrupt(void)
{
    send_next_byte();

    /* clear interrupt flag */
    IFS0bits.U1TXIF = 0;
}

/* -------------------------------------------------------------------------- */
static void send_next_byte(void)
{
    /* send the next queued byte if available, otherwise stop sending */
    if(transmit_queue.read == transmit_queue.write)
        return;

    /* increment and wrap read position */
    ++transmit_queue.read;
    if(transmit_queue.read == TRANSMIT_QUEUE_SIZE)
        transmit_queue.read = 0;

    /* send */
    U1TXREG = transmit_queue.data[transmit_queue.read];
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

/* -------------------------------------------------------------------------- */
/* Test fixture for UART receive state machine */
class uart_rx_fss : public Test
{
    virtual void SetUp()
    {
        /* free all listeners and destroy pending events */
        event_deinit();

        /* initialise uart so our listeners are registered */
        uart_init();

        /* set initial state */
        state = STATE_IDLE;
    }

    virtual void TearDown() {}
};

/* -------------------------------------------------------------------------- */
/* Test fixture for UART send buffer */
static int send_after_n_cycles = 0;

class uart_transmit_queue : public Test
{
    virtual void SetUp()
    {
        event_deinit();
        uart_init();

        transmit_queue.read = 0;
        transmit_queue.write = 0;

        /* TX buffer is idle (nothing being sent) */
        U1TXREG = 0;
        U1STAbits.TRMT = 1;

        /* Used in the blocking test when the buffer is full */
        send_after_n_cycles = 0;
    }

    virtual void TearDown() {}
};

/* -------------------------------------------------------------------------- */
/* Simulates a byte being received */
void sendByte(unsigned char byte)
{
    U1RXREG = byte;
    _U1RXInterrupt();
    event_dispatch_all();
}
void sendString(const char* str)
{
    while(*str)
        sendByte(*(unsigned char*)const_cast<char*>(str++));
}

/* -------------------------------------------------------------------------- */
/* Simulates a byte being sent */
static void tx_send_update()
{
    if(send_after_n_cycles)
        send_after_n_cycles--;
    else
    {
        U1STAbits.TRMT = 1; /* TX buffer is now idle */
        _U1TXInterrupt();   /* send byte */
    }
}
static void tx_send_update_send_all()
{
    while(transmit_queue.read != transmit_queue.write)
        _U1TXInterrupt();
}

/* -------------------------------------------------------------------------- */
TEST_F(uart_rx_fss, selected_cell_and_param_is_reset_correctly_when_switching_to_STATE_SELECT_CELL)
{
    /* set some garbage values */
    state_data.config_cell.param = 88;
    state_data.config_cell.selected_cell = 5;

    sendString("m");

    EXPECT_THAT(state, Eq(STATE_SELECT_CELL));
    EXPECT_THAT(state_data.config_cell.param, Eq((unsigned int)0));
    EXPECT_THAT(state_data.config_cell.selected_cell, Eq((unsigned int)0));
}

TEST_F(uart_rx_fss, cell_is_correctly_selected)
{
    sendString("m2");

    EXPECT_THAT(state, Eq(STATE_SELECT_CELL));
    EXPECT_THAT(state_data.config_cell.selected_cell, Eq(2));
}

TEST_F(uart_rx_fss, cell_with_multiple_digits_is_correctly_selected)
{
    sendString("m195");

    EXPECT_THAT(state, Eq(STATE_SELECT_CELL));
    EXPECT_THAT(state_data.config_cell.selected_cell, Eq(195));
}

TEST_F(uart_rx_fss, abort_cell_selection_if_no_number_is_sent)
{
    sendString("ma");

    EXPECT_THAT(state, Eq(STATE_IDLE));
}

/* -------------------------------------------------------------------------- */
TEST_F(uart_transmit_queue, inserting_byte_when_tx_buffer_is_idle_sends_byte)
{
    U1STAbits.TRMT = 1; /* TX buffer is idle */

    static const unsigned char zero = 0;
    static const unsigned char one  = 1;
    static const unsigned char data = 'a';

    /* some sanity checks */
    EXPECT_THAT(transmit_queue.write, Eq(zero));
    EXPECT_THAT(transmit_queue.read, Eq(zero));

    uart_send(&data, 1);

    EXPECT_THAT(transmit_queue.write, Eq(one));
    EXPECT_THAT(transmit_queue.read, Eq(one));
    EXPECT_THAT(U1TXREG, Eq(data));
}

TEST_F(uart_transmit_queue, inserting_byte_when_tx_buffer_is_busy_queues_byte)
{
    U1STAbits.TRMT = 0; /* TX buffer is busy */

    static const unsigned char zero = 0;
    static const unsigned char one  = 1;
    static const unsigned char data = 'c';

    uart_send(&data, 1);

    EXPECT_THAT(transmit_queue.write, Eq(one));
    EXPECT_THAT(transmit_queue.read, Eq(zero));
    EXPECT_THAT(U1TXREG, Ne(data));
}

TEST_F(uart_transmit_queue, buffer_wraps_correctly)
{
    static const unsigned char data = 'b';

    for(int i = 0; i != TRANSMIT_QUEUE_SIZE * 2; ++i)
    {
        U1TXREG = 0; /* reset this so we can test it */

        uart_send(&data, 1);
        EXPECT_THAT(transmit_queue.write, Ne(TRANSMIT_QUEUE_SIZE));
        EXPECT_THAT(transmit_queue.read, Ne(TRANSMIT_QUEUE_SIZE));
        EXPECT_THAT(U1TXREG, Eq(data));
    }
}

TEST_F(uart_transmit_queue, strings_are_correctly_copied_into_queue)
{
    const char* str = "Test";

    ASSERT_THAT(strlen(str), Le(TRANSMIT_QUEUE_SIZE));

    uart_send((const unsigned char*)str, strlen(str));

    EXPECT_THAT((unsigned char)U1TXREG, Eq(str[0]));
    tx_send_update();
    EXPECT_THAT((unsigned char)U1TXREG, Eq(str[1]));
    tx_send_update();
    EXPECT_THAT((unsigned char)U1TXREG, Eq(str[2]));
    tx_send_update();
    EXPECT_THAT((unsigned char)U1TXREG, Eq(str[3]));
}

TEST_F(uart_transmit_queue, sending_blocks_until_buffer_has_space)
{
    /* build ourselves a string (warning: not null-terminated) */
    unsigned char* data = new unsigned char[TRANSMIT_QUEUE_SIZE + 3];

    {   int i;
        unsigned char d;
        for(i = 0, d = 'a'; i != TRANSMIT_QUEUE_SIZE + 3; i++, d++)
            data[i] = d;
    }

    /* tell TX buffer to be busy for 10 cycles after queue has been filled */
    send_after_n_cycles = 10;
    U1STAbits.TRMT = 0;

    uart_send(data, TRANSMIT_QUEUE_SIZE + 3);

    /* If uart_send blocked, it should have called tx_send_update()
     * 10 times before unblocking */
    EXPECT_THAT(send_after_n_cycles, Eq(0));

    /* After unblocking, the three bytes that wouldn't fit into the buffer
     * should now be in the buffer, placing the write pointer at 3 */
    static const unsigned char three = 3;
    EXPECT_THAT(transmit_queue.write, Eq(three));

    /* sending all bytes should place the read pointer at 3 too */
    tx_send_update_send_all();
    EXPECT_THAT(transmit_queue.read, Eq(three));

    delete[] data;
}

TEST_F(uart_transmit_queue, tx_interrupt_does_nothing_if_queue_is_empty)
{
    static const unsigned char data = 'a';

    uart_send(&data, 1);

    tx_send_update(); /* this is what's being tested */

    static const unsigned char one = 1;
    EXPECT_THAT(transmit_queue.read, Eq(one));
    EXPECT_THAT(transmit_queue.write, Eq(one));
}

#endif /* TESTING */
