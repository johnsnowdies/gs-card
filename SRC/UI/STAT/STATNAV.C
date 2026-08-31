#include "ui\stat\statnav.h"

#include <graphics.h>
#include <stdio.h>
#include <conio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "core\game.h"

#include "ui\upgrade\upgrwnd.h"
#include "ui\shipyard\syardwnd.h"
#include "music.h"


/* ----------------------------------------------------------------
 * Extern game globals
 * ---------------------------------------------------------------- */
extern WND status_wnd;
extern int system_quest_selected;

extern int system_quests_size;
extern WAYPOINT wp;

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;

extern GAME_STATE gs;
extern SYSTEM* sol_list;


/* ----------------------------------------------------------
 * SCR_STATUS -- status window
 * ---------------------------------------------------------- */
int gui_status_wnd_key(int ch, WND *parent)
{
    if (TAB == ch) {
        if (wp.size > 0)
            gui_map_path_wnd();
        cur_screen = SCR_MAP;
        gui_bars_map_bottom();
        gui_map_wnd_draw();
    }
    if (F2 == ch){
        prev_screen = cur_screen;
        cur_screen = SCR_UPGRADE;
        gui_upgrade_wnd();
    }
    if (F3 == ch) {
        if (sol_list[gs.current_system].is_shipyard){
            prev_screen = cur_screen;
            cur_screen = SCR_SHIPYARD;
            gui_shipyard_wnd();
        }else{
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_SHIPYARD_ERROR, SOUND_ERROR);
            getch();
            gui_status_wnd();
        }
    }
    if (ESC == ch) {
        prev_screen = cur_screen;
        cur_screen = SCR_GAME_MENU;
        gui_menu_wnd(parent, 0, 2);
    }
    if (UP == ch) {
        if (system_quest_selected > 0){
            system_quest_selected--;
            gui_status_quest_list(&status_wnd);
        }
    }
    if (DWN == ch) {
        if (system_quest_selected < system_quests_size-1){
            system_quest_selected++;
            gui_status_quest_list(&status_wnd);
        }
    }
    if (ENTER == ch) {
        gui_status_quest_info(system_quest_selected);
    }
    return 0;
}
