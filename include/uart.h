/* uart.h
 * Interface to UART0 on teensy3, using pins 0 and 1. ISR driven, with ring
 * buffer backing TX and RX.
 * Written with the help of shamelessly consulting/stealing from
 * Teensyduino/PJRC serial1.c code.
 * Patrick Schubert 2017
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>

// UART PARITY MODES
#define UART_EVEN_PARITY (2)
#define UART_ODD_PARITY (1)
#define UART_NO_PARITY (0)

// UART ERROR CODES
#define UART_ERROR_PARITY  (1<<0)
#define UART_ERROR_FRAMING (1<<1)
#define UART_ERROR_RXOVER  (1<<2)

void uart_init(uint32_t baud);

// Transmit single byte. Returns 0 on success.
int uart_putc(uint8_t c);

// Transmit null-terminated string. Returns 0 on success.
int uart_puts(uint8_t* s);

// Transmit null-terminated string, blocking until entire string loaded.
int uart_puts_wait(uint8_t* s);

// Set parity mode of UART.
void uart_setparity(uint8_t type);

// Pull a character out of the RX buffer. Returns 0 on success.
int uart_getc(uint8_t* c);

// Returns a bitfield of UART errors experienced since last geterror call.
uint32_t uart_geterror(void);

#endif // UART_H
