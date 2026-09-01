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
#include "ui/shipyard/syardwnd.h"
#include "ui/stat/statnav.h"
#include "ui/stat/statwnd.h"
#include "ui/upgrade/upgrwnd.h"

/* ----------------------------------------------------------
 * EXTERNAL: SCR_STATUS -- STATUS WINDOW KEY HANDLER
 * ---------------------------------------------------------- */
int gui_status_wnd_key(int ch, WND* parent) {
  if (TAB == ch) {
    dispatch_wnd(SCR_MAP, &root_wnd);
  }
  if (F2 == ch) {
    dispatch_wnd(SCR_UPGRADE, &root_wnd);
  }
  if (F3 == ch) {
    if (sol_list[gs.current_system].is_shipyard) {
      dispatch_wnd(SCR_SHIPYARD, &root_wnd);
    } else {
      gui_warning_wnd(&status_wnd, LC_GEN_ERROR_HEAD, LC_SHIPYARD_ERROR,
                      SOUND_ERROR);
    }
  }
  if (ESC == ch) {
    dispatch_wnd(SCR_GAME_MENU, &root_wnd);
  }
  if (UP == ch) {
    if (system_quest_selected > 0) {
      system_quest_selected--;
      gui_status_quest_list(&status_wnd);
    }
  }
  if (DWN == ch) {
    if (system_quest_selected < system_quests_size - 1) {
      system_quest_selected++;
      gui_status_quest_list(&status_wnd);
    }
  }
  if (ENTER == ch) {
    gui_status_quest_info(system_quest_selected);
  }
  return 0;
}
