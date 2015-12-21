/******************************************************************************
 * Filename: main.c
 *
 * Created: 2012/12/14 23:29
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>
#include "nrf.h"
#include "spi.h"
#include "sr.h"
#include "delay.h"
#include "segment.h"
#include "adc.h"
#include "music.h"
#include "globals.h"
#include "beep.h"
#include "radiation.h"

static volatile int second = 0;
volatile unsigned char jiffies = 0;

#define JS (CLOCK/65536)

unsigned char digits = 0;
unsigned char lcdoff = 0;
volatile unsigned char button_sample = 0;
unsigned char button_code = 0;
unsigned char rng = 0;


/* Operation mode */
unsigned char mode = 0;
unsigned char mode_next = 0;
#define MODE_DEFAULT 0
#define MODE_SELECT 1
#define MODE_TIMESUP 2
#define MODE_IMPERIAL 3

volatile unsigned int countdown = 0;

unsigned char _digits = 0;

void mplex() {
	if (flags & FL_DISPLAY) {
		show_digit(_digits & 0xf, 0);
		if (counter_select)
			_sr_conf &= ~DOT;
		else _sr_conf |= DOT;
		sr_update();
		delay_us(1000);
		show_digit((_digits & 0xf0) >> 4, 1);
		sr_update();
		delay_us(1000);
	} else {
		LPM3;
		//delay_ms(2);
	}
	show_digit(0, 2);
	sr_update(0);
}

unsigned int _rand_state = 0;
unsigned int rand() {
	return _rand_state = _rand_state * 49381 + 8643 ;

}

int main() {
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	/* Set ACL to VLO */
	BCSCTL3 = LFXT1S1;
	/* Set clock to MCLK/32768 ~ 244 ticks per second with 8MHz clock */
	// WDTCTL = WDTPW + WDTTMSEL; 
	/* Set clock to ACLK/64 ~ 187 ticks per second with 12kHz clock */
	WDTCTL = WDTPW + WDTTMSEL + WDTIS0 + WDTIS1 + WDTSSEL; 
	IE1 |= WDTIE;

	/* Initialize ADC as soon as possible, dies if battery is low. */
	//adc_init();
	//if (message[MSG_BAT] < 0x8f) {
	
	/* Using XIN/XOUT as I/O */
	P2SEL &= ~(BIT6 + BIT7);

	/* Setup timer for 10ms */
	TACTL |= TASSEL1 | ID0 | ID1 | MC0;
	TACCTL0 = CCIE;
	TACCR0 = 0xff;


#ifdef HWDEBUG
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
#endif

	/* Setup SR ports */
	P2DIR |= SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH;
	P2OUT &= ~(SR_DATA | SR_CLEAR | SR_CLK | SR_LATCH);
	
	/* Configure libraries */
	spi_init();
	nrf_init();
	adc_init();

	/* Setup button on P2.2, internal pull-up. */
	P2DIR &= ~BIT2; /* Input */
	P2REN |= BIT2;	/* Enable pull-up/down */
	P2OUT |= BIT2;	/* Set to pull-up */
	P2IE  |= BIT2;	/* Enable interrupts */
	P2IES |= BIT2;	/* Select high-to-low edge */
	/* TODO: adc_init() breaks those ^^. Why? */
	

	/* Configure as receiver */
	nrf_reg_write(NRF_REG_CONFIG, EN_CRC | PWR_UP | PRIM_RX | CRCO, 1);
	nrf_reg_write(NRF_REG_EN_AA, 0, 1); /* Disable Auto Acknowledgment */
	nrf_reg_write(NRF_REG_RF_CH, RF_CH, 1);
	nrf_reg_write(NRF_REG_RX_PW_P0, MSG_LENGTH, 1);
	//nrf_reg_write(NRF_REG_RX_PW_P0, 1, 1);
	//nrf_reg_write(NRF_REG_EN_RXADDR, ERX_P0 | ERX_P1, 1);
	eint();

	sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
#ifdef HWDEBUG
	for (int i = 0; i < 16; i++) {
		_sr_conf = ~(1<< i);
		sr_update();
		/*beep(1000, 10, 0); */
		delay_ms(100);
	}
#endif
	sr_led(SR_O_YELLOW | SR_O_GREEN | SR_O_RED, 0);
	sr_led(SR_O_GREEN, 1);
	sr_update();

	delay_ms(1000);

	if ((adc_read(0xb) >> 2) > 0x8f) {
		rad_store(0); /* This is backup restore */
	} else {
		/* Light red led and power off. */
		sr_led(SR_O_RED, 1);
		sr_led(SR_O_GREEN, 0);
		sr_update();
		nrf_powerdown();
		LPM4;
	}

	/* Setup speaker. This is done intentionally after the power check to prevent buzzing on low power. */
	P2DIR |= BIT6;

	beep(1000, 10, 0);
	
	
	
	//flags |= FL_DISPLAY;
	//countdown = 500;

	/* Main control loop */
	digits = 0x7a;
	unsigned char jiffies_led[3] = {0,0,0};
	for (;;) {
		/* Handle LED blinking */
		if (evlist & EV_RED_BLINK) {
			sr_led(SR_O_RED, 1);
			jiffies_led[0] = jiffies;
			evlist &= ~EV_RED_BLINK;
		}
		if (evlist & EV_YELLOW_BLINK) {
			sr_led(SR_O_YELLOW, 1);
			jiffies_led[1] = jiffies;
			evlist &= ~EV_YELLOW_BLINK;
		}
		if (evlist & EV_GREEN_BLINK) {
			sr_led(SR_O_GREEN, 1);
			jiffies_led[2] = jiffies;
			evlist &= ~EV_GREEN_BLINK;
		}
		#define BLINK_TIME 8
		for (int led = 0; led < 3; led++) {
			if (((jiffies - jiffies_led[led]) > BLINK_TIME) || ((jiffies_led[led] - jiffies) >= (255-BLINK_TIME))) {
				switch (led) {
					case 0: sr_led(SR_O_RED, 0); break;
					case 1: sr_led(SR_O_YELLOW, 0); break;
					case 2: sr_led(SR_O_GREEN, 0); break;
					default: break;
				}
			}
		}
		sr_update();

		/* Handle listening */
		/* if (evlist & EV_LISTEN) {
			nrf_nolisten();
			evlist &= ~EV_LISTEN;
			msg_receive();
		} */
		
		/* Handle short polling. */
		if (evlist & EV_SHORT_POLL) {
			evlist &= ~EV_SHORT_POLL;
			nrf_powerup();
			nrf_listen();
			mplex();
			nrf_nolisten();
			msg_receive();
			nrf_powerdown();
			evlist |= EV_LISTEN;
			/* Some approximation of dosage */
			unsigned char zone_shift = 0;
			unsigned int zone_incr = 1;
			if (rad_zone > 7) {
				zone_shift = 7;
				zone_incr = rad_zone - 7 + 1 + 4;
			} else
				zone_shift = rad_zone;
			if ((rad_zone) && (rand() < (1 << (zone_shift+8)))) {
				zone_incr = (0x1 << zone_incr);
				if (((rad + zone_incr) ^ rad) & 0xff000 ){
					flags |= FL_DISPLAY;
					countdown = 1000;
				}
				rad += zone_incr;
				if (rad > 0xff000) rad = 0x0ff000;
				rad_zone--;
				evlist |= EV_YELLOW_BLINK;
				if (counter_select)
					beep(500, 10, 0);
			}
			if ((rad >> 20) > 0) rad = 0xffffffff;	/* When this happens, people die.. */
		}

		/* Handle long polling. */
		if (evlist & EV_LONG_POLL) {
			evlist &= ~EV_LONG_POLL;
			message[MSG_BAT] = adc_read(0xb) >> 2;
			/* If voltage is below 2.8V */
			if (message[MSG_BAT] < 0x8f) {
				/* TODO: blink */
				evlist |= EV_RED_BLINK;
			} else {
				/* Store if data differs significantly. This is to prevent flash wearoff. */
				unsigned char d = (rad_zone > 0) ? (rad_zone * 15) : 5;
				if ((rad - rad_get()) > d)
					rad_store(rad);
			}

			message[MSG_FROM] = MYNAME;
			message[MSG_COUNTER4] = rad & 0xff;
			message[MSG_COUNTER3] = (rad & 0xff00) >> 8;
			message[MSG_COUNTER2] = (rad & 0xff0000) >> 16;
			message[MSG_COUNTER1] = (rad & 0xff000000) >> 24;
			message[MSG_RESERVED] = counter_select;

			nrf_settx();
			msg_beacon();
			nrf_setrx();
		}

		/* Simple button presses */
		if (flags & FL_BUTTON) {
			switch (mode) {
				case MODE_TIMESUP:
					countdown = 30*JS;
					flags &= ~FL_BUTTON;
					break;
			}
		}
			
		mplex();

#if 0
		/* 
		 * Delayed button presses for cases when long and short 
		 * press needs to be differentiated 
		 */
		if ((flags & FL_BUTTON) && (jiffies - button_sample <= 5)) {
			/* Button is pressed when P2IN & BUTTON_PIN == 0 */
			#define LONG_PRESSED (!(P2IN & BUTTON_PIN))
			#define SHORT_PRESSED (P2IN & BUTTON_PIN)
			switch (mode) {
				case MODE_DEFAULT:
					button_code <<= 1;
					if (LONG_PRESSED) {
						button_code |= 1;
					}
					if ((button_code & 0x7) == 0b1011) {
						play(MUSIC_SUCCESS);
						digits = mode = mode_next = MODE_SELECT;
					}
					break;
				case MODE_SELECT:
					if (LONG_PRESSED) {
						mode = mode_next;
					} else
						mode_next++;
					break;
				case MODE_IMPERIAL:
					if (LONG_PRESSED) {
						play(MUSIC_IMPERIAL);
					}
					mode = MODE_DEFAULT;
					break;
			}
			flags &= ~FL_BUTTON;
		}
#endif

		/* Multiplex data, this is done inline and does take at most 3ms */
		if ((flags & FL_DISPLAY) || (mode == MODE_TIMESUP) || (mode == MODE_SELECT)) { 
			_digits = digits;
			switch (mode) {
				case MODE_DEFAULT:
					_digits = (rad & 0x0ff000) >> 12;
					break;
				case MODE_SELECT:
					_digits = mode_next;
					break;
#if 0
				case MODE_TIMESUP:
					_digits = countdown / JS;
					_digits = (_digits%10) | (_digits / 10 << 4);
					if (countdown == 1) {
						play(MUSIC_FAILURE);
						countdown = 0;
					}
					if (countdown == 0)
						goto relax;
					break;
#endif
			}
			//mplex();
		} else { 
			//LPM3; 
			_BIS_SR(LPM3_bits + GIE);
		}

		/* TODO: Powersaving via LPM3, could use VLOCLK here. */
		relax:;
	}

}

interrupt(WDT_VECTOR) WDT_ISR(void) {
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

	/* Short polling every ~18ms */
	//if (jiffies % 2 == 0)
		evlist |= EV_SHORT_POLL;

	/* Long polling every second or two */
	if (jiffies == 132) evlist |= EV_LONG_POLL;

	if (countdown == 1) flags &= ~FL_DISPLAY;
	LPM3_EXIT;
}

interrupt(TIMER0_A0_VECTOR) TIMER0_ISR(void) {
	if (flags & FL_BEEP) SPK_OUT ^= SPK;
	rng++;
}

interrupt(PORT2_VECTOR) PORT2_ISR(void) {
	if (P2IFG & BIT2) {
		//flags |= FL_DISPLAY | FL_BUTTON;
		flags |= FL_DISPLAY;
		countdown = 1000;
		//button_sample = jiffies+200;
		P2IFG &= ~BIT2;
	}
	LPM3_EXIT;
}
