#include <msp430.h>

#include "uart.h"

void (*uart_rx_fn)(unsigned char c);

void uart_init(void) {
	uart_set_rx_fn(0L);

	P1SEL  = BIT1 + BIT2;
  	P1SEL2 = BIT1 + BIT2;
  	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  	UCA0BR0 = 104;                            // 1MHz 9600
  	UCA0BR1 = 0;                              // 1MHz 9600
  	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  	UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
  	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void uart_set_rx_fn(void (*isr_ptr)(unsigned char c)) {
	uart_rx_fn = isr_ptr;	
}

unsigned char uart_getc() {
	/* Wait for input and read */
    while (!(IFG2&UCA0RXIFG));
	return UCA0RXBUF;
}

unsigned char uart_hasc() {
	return IFG2 & UCA0RXIFG;
}

void uart_putc(unsigned char c) {
	/* Wait until last byte is transmitted */
	while (!(IFG2&UCA0TXIFG));
  	UCA0TXBUF = c;
}

void uart_puts(const char *str) {
     while(*str) uart_putc(*str++);
}

void uart_putix(unsigned int num, int l) {
	for (int i = l-1; i >= 0; i--) {
		int n = (num & (0xf << (i*4))) >> (i*4);
		if (n > 9) uart_putc('A'+n-10);
		else uart_putc('0'+n);
	}
}

void uart_putib(unsigned int num, int l) {
	for (int i = l-1; i >= 0; i--) {
		uart_putc('0'+((num & (1 << i)) >> i));
		if (!(i%4)) uart_putc(' ');
	}
}

void __attribute__((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR(void)
{
	if(uart_rx_fn != 0L) {
		(uart_rx_fn)(UCA0RXBUF);
	}
}
