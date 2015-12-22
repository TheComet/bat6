/*!
 * @file uart.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include <libq.h>
#include <stdint.h>
#include "drv/uart.h"
#include "drv/hw.h"
#include "core/event.h"
#include "usr/pv_model.h"
#include "core/string.h"
#include "drv/buck.h"

/* Based on Example 5-1 in UART pdf */
/* Calculates value to write to register for the specified baud rate */
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1

/* Sets the size of the send queue */
#define TRANSMIT_QUEUE_SIZE (unsigned)64

/* Enables or disables transmit interrupt */
#define disable_tx_interrupt() (IEC0bits.U1TXIE = 0)
#define enable_tx_interrupt()  (IEC0bits.U1TXIE = 1)

#define is_number(x) \
        (x >= '0' && x <= '9')

/* When a new cell is created and no config for it is provided, */
#define DEFAULT_CURRENT 3000     /* 3000 milliamps */
#define DEFAULT_VOLTAGE 4000     /* 4000 millivolts */
#define DEFAULT_EXPOSURE 100     /* percent */
#define DEFAULT_TEMPERATURE 2930 /* 1/10ths kelvin */

/* Length of the numbers for UART */
#define VOLTAGE_LENGTH 5
#define CURRENT_LENGTH 5
#define EXPOSURE_LENGTH 3
#define TEMPERATURE_LENGTH 3
#define BUFFER_LENGTH 7
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
static void send_update_to_frontend(unsigned int arg);

typedef enum
{
    STATE_IDLE,
    STATE_SELECT_CELL,
    STATE_AWAIT_CELL_CONFIG,
    STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE,
    STATE_CONFIG_SHORT_CIRCUIT_CURRENT,
    STATE_CONFIG_TEMPERATURE,
    STATE_CONFIG_EXPOSURE,
    STATE_ADD_CELL,
    STATE_GET_CONFIG_DUMP,
    STATE_GET_MEASUREMENTS,
    STATE_REMOVE_CELL,
} state_e;

typedef enum
{
    CASE_SELECT_CELL = 'c',
    CASE_REMOVE_CELL = 'r',
    CASE_ADD_CELL = 'a',
    CASE_DUMP_CONFIG = 'd',
    CASE_GET_MEASUREMENTS = 'm'
} case_e;

struct data_t {
    union
    {
        struct {
            unsigned char selected_cell;  /* number representing the cell */
            union {
                unsigned int voltage;     /* millivolts */
                unsigned int current;     /* milliamperes */
                unsigned int temperature; /* 1/10ths Kelvin */
                unsigned int exposure;    /* percent */
            } param;
            unsigned char was_digit;      /* flag for indicating whether
                                           * received char was digit or not */
        } config_cell;
        struct {
            unsigned char selected_cell;
        } config_dump;
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
    event_register_listener(EVENT_CELL_VALUE_UPDATED, send_update_to_frontend);
}

/* -------------------------------------------------------------------------- */
void uart_send(const char* str)
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

    while(*str)
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
        transmit_queue.data[write] = *str++;
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
static void init_state_vars_config()
{
    state_data.config_cell.selected_cell = 0;
    state_data.config_cell.was_digit = 0;
}

/* -------------------------------------------------------------------------- */

static short convert_milli(_Q16 input)
{
    return (short)((input * 1000) >> 16);
}

static short convert_deci(_Q16 input)
{
    return (short)((input * 10) >> 16);
}

static short convert_unit(_Q16 input)
{
    return (short)((input >> 16));
}
 
/* -------------------------------------------------------------------------- */

static short get_cell_voltage(unsigned char cell_id)
{
    return convert_milli(model_get_open_circuit_voltage(cell_id));
}

static short get_cell_current(unsigned char cell_id)
{
    return convert_milli(model_get_short_circuit_current(cell_id));
}

static short get_cell_exposure(unsigned char cell_id)
{
    return convert_deci(model_get_thermal_voltage(cell_id));
}

static short get_cell_temperature(unsigned char cell_id)
{
    return convert_unit(model_get_relative_solar_irradiation(cell_id));   
}

static void send_cell_config(unsigned char cell_id)
{
    char buffer_str[BUFFER_LENGTH]; /* five digits, sign, '\0' terminator */
    short buffer_num;

    buffer_num = get_cell_voltage(cell_id);
    str_nitoa(buffer_str, VOLTAGE_LENGTH, buffer_num);
    uart_send("U");
    uart_send(buffer_str);

    buffer_num = get_cell_current(cell_id);
    str_nitoa(buffer_str, CURRENT_LENGTH, buffer_num);
    uart_send("I");
    uart_send(buffer_str);

    buffer_num = get_cell_temperature(cell_id);
    str_nitoa(buffer_str, TEMPERATURE_LENGTH, buffer_num);
    uart_send("T");
    uart_send(buffer_str);

    buffer_num = get_cell_exposure(cell_id);
    str_nitoa(buffer_str, EXPOSURE_LENGTH, buffer_num);
    uart_send("E");
    uart_send(buffer_str);
}

static void send_measurements(void)
{
    char buffer_str[7];
    short buffer_num;
    
    buffer_num = convert_milli(buck_get_voltage());
    str_nitoa(buffer_str, VOLTAGE_LENGTH, buffer_num);
    uart_send("U");
    uart_send(buffer_str);
    
    buffer_num = convert_milli(buck_get_current());
    str_nitoa(buffer_str, CURRENT_LENGTH, buffer_num);
    uart_send("I");
    uart_send(buffer_str);
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
                init_state_vars_config();
                state = STATE_SELECT_CELL;
            } else if (data == CASE_REMOVE_CELL) {
                init_state_vars_config();
                state = STATE_REMOVE_CELL;
            } else if (data == CASE_ADD_CELL) {
                init_state_vars_config();
                state = STATE_ADD_CELL;
            } else if (data == CASE_DUMP_CONFIG) {
                state = STATE_GET_CONFIG_DUMP;
            } else if (data == CASE_GET_MEASUREMENTS) {
                state = STATE_GET_MEASUREMENTS;
            }
            break;

        case STATE_SELECT_CELL:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.selected_cell *= 10;
                state_data.config_cell.selected_cell += CHAR_TO_INT(data);

                /*
                 * Indicate if last char received was a digit or not.
                 */
                state_data.config_cell.was_digit = 1;
            } else if (state_data.config_cell.was_digit == 0) {
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
                    state_data.config_cell.was_digit = 0;
                    state_data.config_cell.param.current = 0;
   
                    state = STATE_CONFIG_SHORT_CIRCUIT_CURRENT;
                    break;

                case 'U':
                    state_data.config_cell.was_digit = 0;
                    state_data.config_cell.param.voltage = 0;
                    state = STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE;
                    break;

                case 'T':
                    state_data.config_cell.was_digit = 0;
                    state_data.config_cell.param.temperature = 0;
                    state = STATE_CONFIG_TEMPERATURE;
                    break;

                case 'E':
                    state_data.config_cell.was_digit = 0;
                    state_data.config_cell.param.exposure = 0;
                    state = STATE_CONFIG_EXPOSURE;
                    break;

                default:
                    state = STATE_IDLE;
                    break;
            }
            break;

        case STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.param.voltage *= 10;
                state_data.config_cell.param.voltage += CHAR_TO_INT(data);

                state_data.config_cell.was_digit = 1;
            } else {
                _Q16 voltage = state_data.config_cell.param.voltage / 1000
                    * (((unsigned long)2)<<16)
                    | state_data.config_cell.param.voltage % 1000;
                model_set_open_circuit_voltage(
                    state_data.config_cell.selected_cell,
                    voltage);
                /*
                 * We might want to configure more than just OC voltage.
                 */
                state_data.config_cell.was_digit = 0;
                state = STATE_AWAIT_CELL_CONFIG;
            }
            break;

        case STATE_CONFIG_SHORT_CIRCUIT_CURRENT:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.param.current *= 10;
                state_data.config_cell.param.current += CHAR_TO_INT(data);

                state_data.config_cell.was_digit = 1;
            } else {
                _Q16 current = state_data.config_cell.param.current / 1000
                    * (((unsigned long)2)<<16)
                    | state_data.config_cell.param.current % 1000;
                model_set_short_circuit_current(
                    state_data.config_cell.selected_cell,
                    current);
                /*
                 * We might want to configure more than just OC voltage.
                 */
                state_data.config_cell.was_digit = 0;
                state = STATE_AWAIT_CELL_CONFIG;
            }
            break;

        case STATE_CONFIG_TEMPERATURE:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.param.temperature *= 10;
                state_data.config_cell.param.temperature += CHAR_TO_INT(data);

                state_data.config_cell.was_digit = 1;
            } else {
                _Q16 temperature = state_data.config_cell.param.temperature / 10 
                    * (((unsigned long)2)<<16) 
                    | state_data.config_cell.param.temperature % 10;
                model_set_thermal_voltage(
                    state_data.config_cell.selected_cell,
                    temperature);

                /*
                 * We might want to configure more than just OC voltage.
                 */
                state_data.config_cell.was_digit = 0;
                state = STATE_AWAIT_CELL_CONFIG;
            }
            break;

        case STATE_CONFIG_EXPOSURE:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.param.exposure *= 10;
                state_data.config_cell.param.exposure += CHAR_TO_INT(data);

                state_data.config_cell.was_digit = 1;
            } else {
                _Q16 exposure = state_data.config_cell.param.exposure 
                    * (((unsigned long)2)<<16);
                model_set_relative_solar_irradiation(
                    state_data.config_cell.selected_cell,
                    exposure);
                /*
                 * We might want to configure more than just OC voltage.
                 */
                state_data.config_cell.was_digit = 0;
                state = STATE_AWAIT_CELL_CONFIG;
            }
            break;
            
        case STATE_REMOVE_CELL:
            if (is_number(data))
            {
                /* Process multi-digit cell numbers */
                state_data.config_cell.param.exposure *= 10;
                state_data.config_cell.param.exposure += CHAR_TO_INT(data);

                state_data.config_cell.was_digit = 1;
            } else if (data == 'a' && state_data.config_cell.was_digit == 0) {
                /*
                 * If we've started processing digits and an 'a' appears, we
                 * do not want to enter this case. Only for the sequence 'ra'
                 * should we get here.
                 */
                model_cell_remove_all();
                state = STATE_IDLE;
            } else if (data == 'a' && state_data.config_cell.was_digit == 1) {
                /*
                 * This is an error, revert back to idle state. All variables
                 * will be reset there as well.
                 */
                state = STATE_IDLE;
            } else {
                if (model_cell_remove(state_data.config_cell.selected_cell))
                    uart_send("r1");
                else
                    uart_send("r0");
                state = STATE_IDLE;
            }
            break;
            
        case STATE_ADD_CELL:
            /* First, create a new cell and get its ID */
            state_data.config_cell.selected_cell = model_cell_add();
            /* 
             * After that, we set default values for all its properties.
             * In case of an additional user-provided config, this is redundant,
             * but it keeps the state machine simpler and ensures that when
             * this state is left, there exists a config for any added cell.
             */
            model_set_open_circuit_voltage(
                state_data.config_cell.selected_cell,
                DEFAULT_VOLTAGE);
            model_set_short_circuit_current(
                state_data.config_cell.selected_cell,
                DEFAULT_CURRENT);
            model_set_thermal_voltage(
                state_data.config_cell.selected_cell,
                DEFAULT_EXPOSURE);
            model_set_relative_solar_irradiation(
                state_data.config_cell.selected_cell,
                DEFAULT_EXPOSURE);
            /* 
             * If user has provided a list of config properties, they will be
             * they will be configured from the following state, as usual:
             */
            state = STATE_AWAIT_CELL_CONFIG;
            break;
            
        case STATE_GET_CONFIG_DUMP:
            uart_send("d");
            for (
                state_data.config_dump.selected_cell = model_cell_begin_iteration();
                state_data.config_dump.selected_cell != 0;
                state_data.config_dump.selected_cell = model_cell_get_next())
            {
                uart_send("c");
                send_cell_config(state_data.config_dump.selected_cell);
            }
            state = STATE_IDLE;
            break;
            
        case STATE_GET_MEASUREMENTS:
            uart_send("m");
            send_measurements();
            state = STATE_IDLE;
            break;
            
        default:
            state = STATE_IDLE;
            break;
    }
}
/* -------------------------------------------------------------------------- */
static void send_update_to_frontend(unsigned int arg)
{
#define UPDATE_BUFFER_LENGTH 10
    /*
     * When a value for a cell's configuration has been changed on the
     * device itself via the device's menu, that information is sent to
     * the front-end.
     * 
     * arg has the following structure:
     * 
     * 0000'ETIU xxxx xxxx
     *           |_______|
     *               n      
     *
     * E: Exposure changed
     * T: Temperature changed
     * I: Short-circuit current changed
     * U: Open-circuit voltage changed
     * 
     * n: cell ID for which said value has changed
     * 
     */
    unsigned char cell_id = (arg & 0xFF);
    char buffer[UPDATE_BUFFER_LENGTH];
    *buffer='\0';
    
    str_append(buffer, UPDATE_BUFFER_LENGTH, "dc");
       
    char numerical_buffer_str[BUFFER_LENGTH];

    if (arg & 0x0100) {
        /* voltage */
        str_append(buffer, UPDATE_BUFFER_LENGTH, "U");
        str_nitoa(numerical_buffer_str, VOLTAGE_LENGTH, get_cell_voltage(cell_id));
        str_append(buffer, UPDATE_BUFFER_LENGTH, numerical_buffer_str);
    } else if (arg & 0x0200) {
        /* current */
        str_append(buffer, UPDATE_BUFFER_LENGTH, "I");
        str_nitoa(numerical_buffer_str, CURRENT_LENGTH, get_cell_current(cell_id));
        str_append(buffer, UPDATE_BUFFER_LENGTH, numerical_buffer_str);
    } else if (arg & 0x0400) {
        /* temperature */
        str_append(buffer, UPDATE_BUFFER_LENGTH, "T");
        str_nitoa(numerical_buffer_str, CURRENT_LENGTH, get_cell_temperature(cell_id));
        str_append(buffer, UPDATE_BUFFER_LENGTH, numerical_buffer_str);
    } else if (arg & 0x0800) {
        /* exposure */
        str_append(buffer, UPDATE_BUFFER_LENGTH, "E");
        str_nitoa(numerical_buffer_str, CURRENT_LENGTH, get_cell_exposure(cell_id));
        str_append(buffer, UPDATE_BUFFER_LENGTH, numerical_buffer_str);
    } else
        return
    
    uart_send(buffer);
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
    static const char* data = "a";

    /* some sanity checks */
    EXPECT_THAT(transmit_queue.write, Eq(zero));
    EXPECT_THAT(transmit_queue.read, Eq(zero));

    uart_send(data);

    EXPECT_THAT(transmit_queue.write, Eq(one));
    EXPECT_THAT(transmit_queue.read, Eq(one));
    EXPECT_THAT(U1TXREG, Eq((unsigned char)*data));
}

TEST_F(uart_transmit_queue, inserting_byte_when_tx_buffer_is_busy_queues_byte)
{
    U1STAbits.TRMT = 0; /* TX buffer is busy */

    static const unsigned char zero = 0;
    static const unsigned char one  = 1;
    static const char* data = "c";

    uart_send(data);

    EXPECT_THAT(transmit_queue.write, Eq(one));
    EXPECT_THAT(transmit_queue.read, Eq(zero));
    EXPECT_THAT(U1TXREG, Ne((unsigned char)*data));
}

TEST_F(uart_transmit_queue, buffer_wraps_correctly)
{
    static const char* data = "b";

    for(int i = 0; i != TRANSMIT_QUEUE_SIZE * 2; ++i)
    {
        U1TXREG = 0; /* reset this so we can test it */

        uart_send(data);
        EXPECT_THAT(transmit_queue.write, Ne(TRANSMIT_QUEUE_SIZE));
        EXPECT_THAT(transmit_queue.read, Ne(TRANSMIT_QUEUE_SIZE));
        EXPECT_THAT(U1TXREG, Eq((unsigned char)*data));
    }
}

TEST_F(uart_transmit_queue, strings_are_correctly_copied_into_queue)
{
    const char* str = "Test";

    ASSERT_THAT(strlen(str), Le(TRANSMIT_QUEUE_SIZE));

    uart_send(str);

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
    char* data = new char[TRANSMIT_QUEUE_SIZE + 4];

    {   int i;
        unsigned char d;
        for(i = 0, d = 'a'; i != TRANSMIT_QUEUE_SIZE + 3; i++, d++)
            data[i] = d;
        data[TRANSMIT_QUEUE_SIZE + 3] = '\0';
    }

    /* tell TX buffer to be busy for 10 cycles after queue has been filled */
    send_after_n_cycles = 10;
    U1STAbits.TRMT = 0;

    uart_send(data);

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
    static const char* data = "a";

    uart_send(data);

    tx_send_update(); /* this is what's being tested */

    static const unsigned char one = 1;
    EXPECT_THAT(transmit_queue.read, Eq(one));
    EXPECT_THAT(transmit_queue.write, Eq(one));
}

#endif /* TESTING */
