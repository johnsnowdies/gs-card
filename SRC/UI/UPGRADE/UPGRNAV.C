#include <graphics.h>
#include <conio.h>

#include "data\structs.h"
#include "data\keys.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\stat\statwnd.h"
#include "ui\upgrade\upgrwnd.h"
#include "ui\map\mapwnd.h"
#include "music.h"

extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;
extern int system_upgrades_size;
extern int upgrade_selected;

/* ----------------------------------------------------------------
 * SCR_UPGRADES -- upgrades screen controller
 * ---------------------------------------------------------------- */
int gui_upgrade_wnd_key(int ch, WND* parent) {
  if (TAB == ch) {
    cur_screen = SCR_MAP;
    gui_bars_common_top();
    gui_map_wnd_draw();
    sfx_screen_change();
  }
  if (F1 == ch) {
    cur_screen = SCR_STATUS;
    gui_status_wnd();
    sfx_screen_change();
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
