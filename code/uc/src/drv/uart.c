/*! 
 * @file uart.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/uart.h"
#include "drv/hw.h"

int beenhere=0;
/* -------------------------------------------------------------------------- */
void uart_init(void)
{
    // First draft based on Example 10-2 from I/O Ports pdf.
    
    // unlock registers
    __builtin_write_OSCCONL(OSCCON & ~ (1<<6));
    
    // Assign U1RXx to Pin RP59
    RPINR18bits.U1RXR = 59;
    
    // Assign U1TCS to Pin RP58
    RPINR18bits.U1CTSR = 58;
    
    // Assign U1Tx to Pin RP60
    RPOR14bits.RP60R = 1;
    
    // Assign U1RTS to RP61
    RPOR14bits.RP61R = 2;
    
    // Lock Registers
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    
    
    // Example 5-1 from UART manual pdf
    U1MODEbits.STSEL = 0;           // 1 stop bit
    U1MODEbits.PDSEL = 0;           // no parity
    //U1MODEbits.PDSEL = 1;           // 8-bit data, even parity
    U1MODEbits.ABAUD = 0;           // Auto-Baud disabled
    U1MODEbits.BRGH = 0;            // Standard-Speech mode
    
    U1BRG = BRGVAL;                 // baud rate setting for 9600, check uart.h
    
    U1STAbits.UTXISEL0 = 0;         // interrupt after one Tx char is transmitted
    U1STAbits.UTXISEL1 = 0;
    
    IEC0bits.U1TXIE = 1;            // enable UART TX interrupt
    
    U1MODEbits.UARTEN = 1;          // enable UART
    U1STAbits.UTXEN = 1;            // enable UART TX
    
    /* wait at least 105 microseconds (1/9600) before transmitting first char*/
    DELAY_105uS
            
    U1TXREG = 'a';                  // transmit one character
}

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1RXInterrupt(void)
{
    /* clear interrupt flag */
    IFS0bits.U1RXIF = 0;
}

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _U1TXInterrupt(void)
{
    /* clear interrupt flag */
    IFS0bits.U1TXIF = 0;
    U1TXREG = 'a';
    beenhere++;
}
