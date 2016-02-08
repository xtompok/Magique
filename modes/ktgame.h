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
#define DISP_DEFAULT 0xff

#define TEAM_BIT 0x20
#define DRAGON_BIT 0x40
#define PLAYER_BITS 0xf

#define FLAG_HOLDER_NOONE 2
#define TEAM_RED 0
#define TEAM_GREEN 1

#define MANA_MAX 0x32
#define FLAG_CONQUER_PERIOD 10

#define PLAYER_KING 1
#define PLAYER_KNIGHT 2
#define PLAYER_FAT_KNIGHT 3
#define PLAYER_POLITICAL_KNIGHT 4
#define PLAYER_DRAGON_HELPER 0xf
#define POLITICAL_KNIGHT_MASTER_KILL 16
#define DRAGON_INSTANT_KILL 0x16



void ktgame_init(void);
void ktgame_process(void);
void ktgame_process_flag(void);

#endif
