/******************************************************************************
 * Filename: uart.h
 *
 * Created: 2013/02/03 17:02
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#ifndef __MSP430_HAS_USCI__
#error "Selected cpu does not have hw UART, please, code your own."
#endif

void uart_init(void);
void uart_set_rx_fn(void (*isr_ptr)(unsigned char c));
unsigned char uart_getc();
unsigned char uart_hasc();
void uart_putc(unsigned char c);
void uart_puts(const char *str);
void uart_putix(unsigned int num, int l);
void uart_putib(unsigned int num, int l);

#endif
