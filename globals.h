/******************************************************************************
 * Filename: globals.h
 *
 * Created: 2013/02/18 23:35
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/* Command flags */
extern volatile unsigned char flags;
#define FL_DISPLAY BIT0
#define FL_BEEP BIT1
#define FL_BUTTON BIT5
#define FL_GAME_LISTEN BIT6
#define FL_STONE_LISTEN BIT7

/* Event queue */
extern volatile unsigned char evlist;
#define EV_RED_BLINK BIT0
#define EV_YELLOW_BLINK BIT1
#define EV_GREEN_BLINK BIT2
#define EV_SHORT_POLL BIT3
#define EV_LONG_POLL BIT4
#define EV_TICK_POLL BIT5
#define EV_LISTEN BIT6

extern volatile unsigned char _digits;

#endif
