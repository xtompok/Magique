/******************************************************************************
 * Filename: main.c
 *
 * Created: 2012/12/14 23:29
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include "nrf.h"
#include "spi.h"
#include "sr.h"
#include "delay.h"
#include "segment.h"
#include "adc.h"
#include "music.h"
#include "globals.h"
#include "beep.h"
#include "network.h"
#include "magique.h"

#include "modes/ktgame.h"

static volatile int second = 0;
volatile unsigned char jiffies = 0;

#define JS (CLOCK/65536)

unsigned char lcdoff = 0;
volatile unsigned char button_sample = 0;
unsigned char button_code = 0;
unsigned char rng = 0;

struct node my_info;

/* Operation mode */
unsigned char mode = 0;
unsigned char mode_next = 0;
#define MODE_DEFAULT 0
#define MODE_SELECT 1
#define MODE_TIMESUP 2
#define MODE_IMPERIAL 3

volatile unsigned int countdown = 0;

unsigned char _digits = 0;

/* Change current digit multiplex */
void mplex() {
	/* Show first digit */
	show_digit(_digits & 0xf, 0);
	sr_update();
	delay_us(1000);

	/* Show second digit */
	show_digit((_digits & 0xf0) >> 4, 1);
	sr_update();
	delay_us(1000);

	/* Clear display */
	show_digit(0, 2);
	sr_update(0);
}

unsigned int _rand_state = 0;
unsigned int rand() {
	return _rand_state = _rand_state * 49381 + 8643 ;

}

void magique_hw_init(void) {
	/* This function initializes all the hardware IO/peripherals */
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	/* Set ACL to VLO */
	BCSCTL3 = LFXT1S1;

	/* Set clock to ACLK/64 ~ 187 ticks per second with 12kHz clock */
	WDTCTL = WDTPW + WDTTMSEL + WDTIS0 + WDTIS1 + WDTSSEL;
	IE1 |= WDTIE;

	/* Using XIN/XOUT as I/O */
	P2SEL &= ~(BIT6 + BIT7);

	/* Setup timer for 10ms; initially off. Add MC0 to start it up. */
	TACTL |= TASSEL1 | ID0 | ID1;
	TACCTL0 = CCIE;
	TACCR0 = 0xff;

#ifdef HWDEBUG
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
#endif

	/* Setup SR ports */
	P2DIR |= SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH;
	P2OUT &= ~(SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH);

	/* Setup button on P2.2, internal pull-up. */
	P2DIR &= ~BIT2; /* Input */
	P2REN |= BIT2;	/* Enable pull-up/down */
	P2OUT |= BIT2;	/* Set to pull-up */
	P2IE  |= BIT2;	/* Enable interrupts */
	P2IES |= BIT2;	/* Select high-to-low edge */

	__enable_interrupt();
}

void magique_self_test(void) {
	sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
#ifdef HWDEBUG
	for (int i = 0; i < 16; i++) {
		_sr_conf = ~(1<< i);
		sr_update();
		/*beep(1000, 10, 0); */
		delay_ms(200);
	}
#endif
	sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
	sr_update();
}

int main() {
	/* Setup clock, timers, interrupts and IOs */
	magique_hw_init();

	/* Configure libraries */
	spi_init();
	adc_init();
	nrf_init();

	/* Setup network communication pipelines, frequencies & stuff */
	network_init( RF_ROLE_TX );

	magique_self_test();

	//delay_ms(1000);

#if 0
	if ((adc_read(0xb) >> 2) < 0x8f) {
		/* Battery is low; Flash with red LED once and power down. */
		sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
		sr_led(SR_O_RED, 1);
		sr_update();
		delay_ms(500);
		sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
		sr_update();
		nrf_powerdown();
		LPM4; /* In LPM4, all clocks are off, we won't wake up. */
	}
#endif

	/* Beep, signaling we're ready and entering the control loop */
	beep(1000, 10, 0);

	_digits = 0;

	/* Main control loop */
	unsigned char jiffies_led[3] = {0,0,0};
	for (;;) {
		/* Go to sleep mode, if no events are planned and no flags raised */
		if (!(flags || evlist)) {
			LPM3;
			continue;
		}

		/* Handle LED blinking */
		if (evlist & EV_RED_BLINK) {
			sr_led(SR_O_RED, 1);
			jiffies_led[0] = jiffies;
		}
		if (evlist & EV_YELLOW_BLINK) {
			sr_led(SR_O_YELLOW, 1);
			jiffies_led[1] = jiffies;
		}
		if (evlist & EV_GREEN_BLINK) {
			sr_led(SR_O_GREEN, 1);
			jiffies_led[2] = jiffies;
		}
		#define BLINK_TIME 8
		for (unsigned int led = 0; led < 3; led++) {
			if (((jiffies - jiffies_led[led]) > BLINK_TIME) || ((jiffies_led[led] - jiffies) >= (255-BLINK_TIME))) {
				switch (led) {
					case 0: sr_led(SR_O_RED, 0); break;
					case 1: sr_led(SR_O_YELLOW, 0); break;
					case 2: sr_led(SR_O_GREEN, 0); break;
					default: break;
				}
			}
		}

		/* Handle short polling (16 times per second). */
		if (evlist & EV_SHORT_POLL) {
		}

		/* Handle long polling (once per second). */
		if (evlist & EV_LONG_POLL) {
		}

		switch (my_info.mode) {
			case MODE_KTGAME:
				ktgame_process();
			default:;
		}

		/* All events _should_ have benn handled */
		evlist = 0;

		if (flags & FL_DISPLAY) 
			mplex();
		else
			sr_update();
	}

}

void __attribute__((interrupt(WDT_VECTOR))) WDT_ISR(void) {
	P1OUT ^= 1;
	//WDTCTL = WDTPW + WDTSSEL;
	jiffies++;
	if (countdown > 1) countdown--;
	#ifdef SWDEBUG
	if (jiffies == 0) evlist |= EV_RED_BLINK;
	if (jiffies == 30) evlist |= EV_YELLOW_BLINK;
	if (jiffies == 60) evlist |= EV_GREEN_BLINK;
	if (jiffies == 90) evlist |= EV_YELLOW_BLINK;
	if (jiffies == 120) evlist |= EV_RED_BLINK;
	#endif

	/* Short polling every once in a while */
	if ((jiffies & 0x0f) == 0)
		evlist |= EV_SHORT_POLL;

	/* Long polling every second or two */
	if (jiffies == 132) evlist |= EV_LONG_POLL;

	if (countdown == 1) flags &= ~FL_DISPLAY;

	//sr_update();
	LPM3_EXIT;
}

void __attribute((interrupt(TIMER0_A0_VECTOR))) TIMER0_ISR(void) {
	/* TACCR0 overflowed is reserved for the buzzer */
	//if (TAIV & TA0IV_TAIFG) {
		if (flags & FL_BEEP) SPK_OUT ^= SPK;
//	}
}

void __attribute((interrupt(PORT2_VECTOR))) PORT2_ISR(void) {
	if (P2IFG & BIT2) {
		//flags |= FL_DISPLAY | FL_BUTTON;
		flags |= FL_DISPLAY;
		countdown = 1000;
		//button_sample = jiffies+200;
		P2IFG &= ~BIT2;
	}
	LPM3_EXIT;
}
