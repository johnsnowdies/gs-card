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

/* ----------------------------------------------------------------
 * EXTERNAL: SCR_UPGRADES -- UPGRADE SCREEN KEY HANDLER
 * ---------------------------------------------------------------- */
int gui_upgrade_wnd_key(int ch, WND* parent) {
  if (TAB == ch) {
    if (wp.size > 0) gui_map_path_wnd();
    cur_screen = SCR_MAP;
    gui_bars_map_bottom();
    gui_map_wnd_draw();
  }
  if (F1 == ch) {
    cur_screen = SCR_STATUS;
    gui_status_wnd();
    sfx_screen_change();
  }
  if (F3 == ch) {
    if (sol_list[gs.current_system].is_shipyard) {
      prev_screen = cur_screen;
      cur_screen = SCR_SHIPYARD;
      gui_shipyard_wnd();
    } else {
      gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_SHIPYARD_ERROR,
                      SOUND_ERROR);
      getch();
      gui_upgrade_wnd();
    }
  }
  if (UP == ch) {
    if (upgrade_selected > 0) {
      upgrade_selected--;
      gui_upgrade_draw_list();
    }
  }
  if (DWN == ch) {
    if (upgrade_selected < system_upgrades_size - 1) {
      upgrade_selected++;
      gui_upgrade_draw_list();
    }
  }
  if (ENTER == ch) {
    if (system_upgrades_size > 0) {
      gui_upgrade_show_info(upgrade_selected);
    }
  }
  if (ESC == ch) {
    prev_screen = cur_screen;
    cur_screen = SCR_GAME_MENU;
    gui_menu_wnd(parent, 0, 2);
  }
  return 0;
}
