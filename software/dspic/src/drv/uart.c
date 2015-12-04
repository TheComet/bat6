/*! 
 * @file uart.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/uart.h"
#include "drv/hw.h"
#include "core/event.h"

// based on Example 5-1 in UART pdf
#define FP 60000000 /* 60 MIPS, see hw.c */
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1

static void process_incoming_data(void* data);

typedef enum
{
    STATE_IDLE,
    STATE_SELECT_MODEL,
    /* awaits a selector for what is to be done to a model*/
    STATE_AWAIT_MODEL_CONFIG,
    STATE_CONFIG_OPEN_CIRCUIT_VOLTAGE,
    STATE_CONFIG_SHORT_CIRCUIT_CURRENT,
    STATE_CONFIG_TEMPERATURE,
    STATE_CONFIG_EXPOSURE,
} state_e;

typedef enum
{
    CASE_SELECT_MODEL = 'm',
} case_e;

struct data_t {
    union 
    {
        struct {
            unsigned char decimal;
            unsigned char selected_model;
            unsigned int param;
        } config_model;
    };
};

static state_e state = STATE_IDLE;
static struct data_t state_data;

/* -------------------------------------------------------------------------- */
void uart_init(void)
{
    // TODO: merge with hw.c
    
    // Set RP58 to digital
    ANSELCbits.ANSC10 = 0;
    
    // Set RP59 to digital
    ANSELCbits.ANSC11 = 0;
    
    // Set RP60 to Digital
    ANSELCbits.ANSC12 = 0;
    // RP61 seems to already be digital.
    
    // First draft based on Example 10-2 from I/O Ports pdf.
    
    // unlock registers
    __builtin_write_OSCCONL(OSCCON & ~ (1<<6));
    
    // Assign U1Rx to Pin RP59
    RPINR18bits.U1RXR = 0b00111011;
    
    // Assign U1TCS to Pin RP58
    RPINR18bits.U1CTSR = 0b00111010;
    
    // Assign U1Tx to Pin RP60, p.10-11 I/O Ports documentation
    RPOR14bits.RP60R = 0b000001;
    
    // Assign U1RTS to RP61
    RPOR14bits.RP61R = 0b000010;
    
    /* RX requires pull-up */
    CNPUCbits.CNPUC11 = 1;
    
    // Lock Registers
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    
    // Example 5-1 from UART manual pdf
    U1MODEbits.STSEL = 0;           // 1 stop bit
    //U1MODEbits.PDSEL = 0;           // no parity
    U1MODEbits.PDSEL = 1;           // 8-bit data, even parity
    U1MODEbits.ABAUD = 0;           // Auto-Baud disabled
    U1MODEbits.BRGH = 0;            // Standard-Speech mode
    U1MODEbits.RXINV = 1;           // Invert RX (due to isolating IC)
    U1STAbits.TXINV = 1;            // Invert TX (due to isolating IC)
    
    U1BRG = BRGVAL;                 // baud rate setting, see #defines at top
    
    U1STAbits.UTXISEL0 = 0;         // interrupt after one Tx char is transmitted
    U1STAbits.UTXISEL1 = 0;
    
    U1MODEbits.UARTEN = 1;          // enable UART
    U1STAbits.UTXEN = 1;            // enable UART TX
    
    IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 1;            // enable UART TX interrupt
    
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;            /* enable RX interrupt */

    //U1TXREG = 'a';                  // transmit one character
    
    event_register_listener(EVENT_DATA_RECEIVED, process_incoming_data);
}

static void process_incoming_data(void* args)
{
    unsigned int data = (unsigned int)args;
    
    switch (state)
    {
        case STATE_IDLE:
            if (data == CASE_SELECT_MODEL)
            {
                state_data.config_model.decimal = 1;
                state_data.config_model.selected_model = 0;
                state = STATE_SELECT_MODEL;
            }
            break;
            
        case STATE_SELECT_MODEL:
            if (data >= '0' && data <= '9')
            {
                /* Process multi-digit model numbers*/
                state_data.config_model.selected_model 
                    += data * state_data.config_model.decimal;
                state_data.config_model.decimal *= 10;
            } else if (state_data.config_model.decimal == 1) {
                /* 
                 * Letter directly followed by another letter: error, 
                 * revert back to idle state
                 */
                state = STATE_IDLE;
            } else {
                state = STATE_AWAIT_MODEL_CONFIG;
            }
            break;
            
        case STATE_AWAIT_MODEL_CONFIG:
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
            break;
    }
}


/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1RXInterrupt(void)
{
    //U1TXREG = U1RXREG; /* echo back whatever we receive */
    
    event_post(EVENT_DATA_RECEIVED, (void*)U1RXREG);
    
    /* clear interrupt flag */
    IFS0bits.U1RXIF = 0;
}

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1TXInterrupt(void)
{
    /* clear interrupt flag */
    IFS0bits.U1TXIF = 0;
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

void sendByte(unsigned char byte)
{
    U1RXREG = byte;
    _U1RXInterrupt();
}

TEST(receive_state_machine, model_is_correctly_selected)
{
    sendByte('I');
    sendByte('2');
    event_process_all();
    
    ASSERT_THAT(state_data.model.selected, Eq(2));
}

#endif /* TESTING */