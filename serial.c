/*
 * The following code is adapted from
 * https://www.appelsiini.net/2011/simple-usart-with-avr-libc
 * and
 * http://nongnu.org/avr-libc/user-manual/group__avr__stdio.html#stdio_without_malloc
 */

// Needed for setbaud.h
#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>
#include <avr/io.h>
#include <stdio.h>

static void uart_putchar(char c, FILE *stream);
static char uart_getchar(FILE *stream);

FILE serial_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

FILE serial_in = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void serial_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X
    UCSR0A |= _BV(U2X0);
    #else
    UCSR0A &= ~(_BV(U2X0));
    #endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8 bit data
    UCSR0B = _BV(RXEN0) | _BV(TXEN0); // Enable RX and TX
}

static void uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0); // Wait until data register is empty
    UDR0 = c;
}

static char uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0); // Wait until data exists
    return UDR0;
}