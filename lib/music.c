/******************************************************************************
 * Filename: music.c
 *
 * Created: 2013/02/18 23:18
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#include "hardware.h"
#include "beep.h"
#include "music.h"

void play(unsigned char tune) {
	#undef D
	#define D 600
	switch (tune) {
		case MUSIC_IMPERIAL:
			beep(NOTE_E, D, 50);
			beep(NOTE_E, D, 50);
			beep(NOTE_E, D, 50);
			beep(NOTE_C, D, 0);
			beep(NOTE_G, D/2, 0);
			beep(NOTE_E, D, 0);
			beep(NOTE_C, D, 0);
			beep(NOTE_G, D/2, 0);
			beep(NOTE_E, D*2, 0);

			beep(NOTE_H, D, 50);
			beep(NOTE_H, D, 50);
			beep(NOTE_H, D, 50);
			beep(NOTE_C1, D, 0);
			beep(NOTE_G, D/2, 0);
			beep(NOTE_E, D, 0);
			beep(NOTE_C, D, 0);
			beep(NOTE_G, D/2, 0);
			beep(NOTE_E, D*2, 0);
			break;
		case MUSIC_SUCCESS:
			beep(NOTE_C, D/2, 0);
			beep(NOTE_E, D/2, 0);
			break;
		case MUSIC_FAILURE:
			beep(NOTE_E, D/2, 20);
			beep(NOTE_E, D/2, 20);
			beep(NOTE_E, D/2, 20);
			beep(NOTE_C, D*2, 0);
			break;
	}
}
