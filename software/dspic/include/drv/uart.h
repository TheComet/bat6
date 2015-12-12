/*!
 * @file uart.h
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

/*!
 * @brief Initialises the UART driver. Call this for communication to work.
 */
void uart_init(void);

/*!
 * @brief Queues a byte for sending.
 * @param bytes An array of bytes to send.
 * @param len The length of the array. If you are sending a string, you do not
 * need to include the null terminator (strlen() returns the correct value).
 */
void uart_send(const unsigned char* byte, unsigned short len);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */
