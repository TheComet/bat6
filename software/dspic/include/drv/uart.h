/*! 
 * @file uart.h
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#ifndef UART_H
#define	UART_H

// based on Example 5-1 in UART pdf
#define FP 40000000
#define BAUDRATE 9600
#define BRGVAL ((FP/BAUDRATE)/16)-1
#define DELAY_105uS asm volatile ("REPEAT, #4201");Nop(); // 105uS delay

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the UART driver. Call this for communication to work.
 */
void uart_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */
