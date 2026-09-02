#include <conio.h>
#include <graphics.h>

#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/locale.h"
#include "ui/shipyard/syardnav.h"
#include "ui/shipyard/syardwnd.h"
#include "ui/menu/menuwnd.h"

/* ----------------------------------------------------------------
 * EXTERNAL: SCR_SHIPYARD -- SHIPYARD WINDOW KEY HANDLER
 * ---------------------------------------------------------------- */
int gui_shipyard_wnd_key(int ch) {
  if (TAB == ch) {
    dispatch_wnd(SCR_MAP);
  }
  if (F1 == ch) {
    dispatch_wnd(SCR_STATUS);
  }
  if (F2 == ch) {
    dispatch_wnd(SCR_UPGRADE);
  }
  if (ENTER == ch) {
    gui_shipyard_deal_wnd();
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
    dispatch_wnd(SCR_GAME_MENU);
  }
  return 0;
}
