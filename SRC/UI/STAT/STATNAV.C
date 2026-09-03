#include <conio.h>
#include <graphics.h>
#include <stdio.h>

#include "core/game.h"
#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/gui.h"
#include "ui/locale.h"
#include "ui/menu/menuwnd.h"
#include "ui/shipyard/syardwnd.h"
#include "ui/stat/statnav.h"
#include "ui/stat/statwnd.h"
#include "ui/upgrade/upgrwnd.h"

/* ----------------------------------------------------------
 * EXTERNAL: SCR_STATUS -- STATUS WINDOW KEY HANDLER
 * ---------------------------------------------------------- */
int gui_status_wnd_key(int ch) {
  if (TAB == ch) {
    dispatch_wnd(SCR_MAP);
  }
  if (F2 == ch) {
    dispatch_wnd(SCR_UPGRADE);
  }
  if (F3 == ch) {
    if (sol_list[gs.current_system].is_shipyard) {
      dispatch_wnd(SCR_SHIPYARD);
    } else {
      gui_warning_wnd(&status_wnd, LC_GEN_ERROR_HEAD, LC_SHIPYARD_ERROR,
                      SOUND_ERROR);
    }
  }
  if (UP == ch) {
    if (system_quest_selected > 0) {
      system_quest_selected--;
      gui_status_quest_list(&status_wnd);
      sfx_menu_move();
    }
  }
  if (DWN == ch) {
    if (system_quest_selected < system_quests_size - 1) {
      system_quest_selected++;
      gui_status_quest_list(&status_wnd);
      sfx_menu_move();
    }
  }

  if (ENTER == ch) {
    gui_status_quest_info(system_quest_selected);
  }

  if (ESC == ch) {
    dispatch_wnd(SCR_GAME_MENU);
  }
  return 0;
}
