#include <graphics.h>
#include <conio.h>


#include "data\structs.h"
#include "data\keys.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\map\mapwnd.h"
#include "music.h"

#include "ui\shipyard\syardwnd.h"
#include "ui\shipyard\syardnav.h"

extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;
extern int system_shipyard_size;
extern int ship_selected;

/* ----------------------------------------------------------------
 * SCR_SHIPYARD - shipyard window navigation
 * ---------------------------------------------------------------- */
int gui_shipyard_wnd_key(int ch, WND* parent) {
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
  if (F2 == ch) {
    cur_screen = SCR_UPGRADE;
    gui_status_wnd();
    sfx_screen_change();
  }
  if (UP == ch) {
    if (ship_selected > 0) {
      ship_selected--;
      gui_shipyard_draw_list();
    }
  }
  if (DWN == ch) {
    if (ship_selected < system_shipyard_size - 1) {
      ship_selected++;
      gui_shipyard_draw_list();
    }
  }
  if (ESC == ch) {
    prev_screen = cur_screen;
    cur_screen = SCR_GAME_MENU;
    gui_menu_wnd(parent, 0, 2);
  }
  return 0;
}
