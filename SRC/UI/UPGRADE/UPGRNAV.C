#include <conio.h>

#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/locale.h"
#include "ui/map/mapwnd.h"
#include "ui/shipyard/syardwnd.h"
#include "ui/stat/statwnd.h"
#include "ui/upgrade/upgrnav.h"
#include "ui/upgrade/upgrwnd.h"
#include "ui/menu/menuwnd.h"

/* ----------------------------------------------------------------
 * EXTERNAL: SCR_UPGRADES -- UPGRADE SCREEN KEY HANDLER
 * ---------------------------------------------------------------- */
int gui_upgrade_wnd_key(int ch) {
  if (TAB == ch) {
    dispatch_wnd(SCR_MAP);
  }
  if (F1 == ch) {
    dispatch_wnd(SCR_STATUS);
  }
  if (F3 == ch) {
    if (sol_list[gs.current_system].is_shipyard) {
      dispatch_wnd(SCR_SHIPYARD);
    } else {
      gui_warning_wnd(&upgrade_wnd, LC_GEN_ERROR_HEAD, LC_SHIPYARD_ERROR,
                      SOUND_ERROR);
    }
  }
  if (UP == ch) {
    if (upgrade_selected > 0) {
      upgrade_selected--;
      gui_upgrade_draw_list();
      sfx_menu_move();
    }
  }
  if (DWN == ch) {
    if (upgrade_selected < system_upgrades_size - 1) {
      upgrade_selected++;
      gui_upgrade_draw_list();
      sfx_menu_move();
    }
  }
  if (ENTER == ch) {
    if (system_upgrades_size > 0) {
      gui_upgrade_show_info(upgrade_selected);
    }
  }
  if (ESC == ch) {
    dispatch_wnd(SCR_GAME_MENU);
  }
  return 0;
}
