#include <graphics.h>
#include <stdio.h>
#include <conio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "core\game.h"

#include "ui\stat\statnav.h"

#include "music.h"

extern WND status_wnd, quest_wnd;
extern int system_quest_selected;

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern GAME_STATE gs;
extern SYSTEM* sol_list;

extern char* data_factions[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern char* data_ship_names[SHIP_COUNT];
extern int   data_ship_tonnages[SHIP_COUNT];
extern char* data_hyper_names[HYPER_COUNT];

extern char* QUEST_TYPES[];

extern QUEST system_quests[5];
extern int system_quests_size;

extern WAYPOINT wp;

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;


/* ----------------------------------------------------------
 * SCR_STATUS -- status window
 * ---------------------------------------------------------- */
int gui_status_wnd_key(int ch, WND *parent)
{
    if (TAB == ch) {
        if (wp.size > 0)
            gui_map_path_wnd();
        cur_screen = SCR_MAP;
        gui_map_bottom_status_line();
        gui_map_wnd_draw();
    }
    if (ESC == ch) {
        prev_screen = cur_screen;
        cur_screen = SCR_GAME_MENU;
        gui_menu_wnd(parent, 0, 2);
    }
    if (UP == ch) {
        if (system_quest_selected > 0){
            system_quest_selected--;
            gui_status_quest_list(&status_wnd, system_quest_selected);
        }
    }
    if (DWN == ch) {
        if (system_quest_selected < system_quests_size-1){
            system_quest_selected++;
            gui_status_quest_list(&status_wnd, system_quest_selected);
        }
    }
    if (ENTER == ch) {
        cur_screen = SCR_QUEST_LIST_DETAIL;
        gui_status_quest_info(system_quest_selected);
    }
    return 0;
}

/* ----------------------------------------------------------
 * SCR_QUEST_LIST_DETAIL -- quest detail view
 * ---------------------------------------------------------- */
int gui_status_quest_wnd_key(int ch, WND *parent)
{
    if (ESC == ch) {
        cur_screen = SCR_STATUS;
        gui_status_wnd();
    }
    return 0;
}
