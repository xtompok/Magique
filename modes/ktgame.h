/******************************************************************************
 * Filename: ktgame.h
 *
 * Created: 2016/01/24 23:42
 * Author: Ladislav Láska
 * e-mail: laska@kam.mff.cuni.cz
 *
 ******************************************************************************/

#ifndef __KTGAME_H__
#define __KTGAME_H__

#define DISP_MANA 0
#define DISP_ATTACK 1
#define DISP_DEFENSE 2
#define DISP_RED 10
#define DISP_GREEN 11

#define FLAG_HOLDER_NOONE 2
#define TEAM_RED 0
#define TEAM_GREEN 1

#define MANA_MAX 200
#define FLAG_CONQUER_PERIOD 6

void ktgame_init(void);
void ktgame_process(void);
void ktgame_process_flag(void);

#endif
