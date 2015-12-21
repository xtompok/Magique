/******************************************************************************
 * Filename: music.h
 *
 * Created: 2013/02/18 23:19
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __MUSIC_H__
#define __MUSIC_H__

void play(unsigned char tune);

/* Available tunes */
#define MUSIC_IMPERIAL 1
#define MUSIC_SUCCESS 2
#define MUSIC_FAILURE 3

/* Beeps and sounds */
#define FREQ(A) (CLOCK/(A*2))
#define NOTE_C FREQ(261)
#define NOTE_D FREQ(293)
#define NOTE_E FREQ(329)
#define NOTE_F FREQ(349)
#define NOTE_G FREQ(392)
#define NOTE_A FREQ(493)
#define NOTE_H FREQ(440)

#define NOTE_C1 FREQ(523)
#define NOTE_D1 FREQ(587)
#define NOTE_E1 FREQ(659)
#define NOTE_F1 FREQ(698)
#define NOTE_G1 FREQ(783)
#define NOTE_A1 FREQ(880)
#define NOTE_H1 FREQ(987)

#endif
