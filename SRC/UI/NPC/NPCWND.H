#ifndef _GUI_NPC_WND
#define _GUI_NPC_WND
#define E_NPC_WND_TYPE enum npc_wnd_type
#include "data/structs.h"
#include "ui\gui.h"

E_NPC_WND_TYPE{NPC_CHOICE_WND, NPC_DIALOG_WND};

int gui_npc_wnd(WND* ptr_parent, NPC* ptr_npc, int wnd_type, char title[100],
                char lines[][100], int lines_count, char buttons[][100],
                int buttons_count, int buttons_orient);
#endif
