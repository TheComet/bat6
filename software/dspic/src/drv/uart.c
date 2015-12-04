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
}



/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1RXInterrupt(void)
{
    U1TXREG = U1RXREG; /* echo back whatever we receive */
    
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

TEST(test, foo)
{
    ASSERT_THAT(1, Eq(1));
}

#endif /* TESTING */
