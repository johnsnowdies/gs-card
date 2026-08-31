#ifndef _CORE_GLOBALS
#define _CORE_GLOBALS

#include "data/structs.h"
#include "ui/gui.h"

extern const int DEBUG;

/* Data related global variables */
extern SYSTEM* sol_list;
extern OBJECT* obj_list;
extern BOUND_LINE* bnd_list;
extern QUEST system_quests[5];
extern UPGRADE system_upgrades[8];
extern SHIP system_shipyard[6];

extern unsigned int sol_size;
extern unsigned int obj_size;
extern unsigned int bnd_size;
extern int system_quests_size;
extern int system_upgrades_size;
extern int system_shipyard_size;

extern WAYPOINT wp;

/* Game state */
extern GAME_STATE gs;

/* Exit signal */
extern unsigned char SIG_TERM;

extern WND root_wnd;

extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;
#endif
