#include <stdio.h>

#include "core/globals.h"
#include "data/structs.h"
#include "ui/bars/bars.h"
#include "ui/gui.h"
#include "ui/locale.h"

/* ----------------------------------------------------------------
 * STATUS LINES
 * ---------------------------------------------------------------- */
void gui_bars_common_top() {
  WND status_line;
  char buf[100];
  char* keys[3];
  char* items[3];

  sprintf(buf, "SA.%d (%d) | %s: %ld$$ | %s: %d%% | %s: %d/%d",
          gs.current_system, sol_list[gs.current_system].threadSize,
          LC_GUI_STATUS_BALANCE, gs.balance, LC_GUI_STATUS_FUEL, gs.fuel,
          LC_GUI_STATUS_CARGO, gs.current_cargo, gs.tonnage);

  keys[0] = "TAB";
  items[0] = LC_GUI_STATUS_MODE;
  keys[1] = LC_GUI_STATUS_INFO;
  items[1] = buf;
  keys[2] = NULL;
  items[2] = NULL;

  status_line.x = 0;
  status_line.y = 0;
  status_line.width = STATUSBAR_WIDTH;
  status_line.height = STATUSBAR_HEIGHT;
  status_line.header = NULL;

  gui_draw_status_line(&status_line, keys, items, 0);
}

void gui_bars_status_bottom() {
  WND status_line;
  char* keys[] = {"F1", "F2", "F3", NULL};
  char* items[] = {LC_STATUS_F1, LC_STATUS_F2, LC_STATUS_F3, NULL};
  int highlight = -1;

  status_line.x = 0;
  status_line.y = STATUSBAR_BOTTOM_Y;
  status_line.width = STATUSBAR_WIDTH;
  status_line.height = STATUSBAR_HEIGHT;
  status_line.header = NULL;

  if (cur_screen == SCR_STATUS) highlight = 0;
  if (cur_screen == SCR_UPGRADE) highlight = 1;
  if (cur_screen == SCR_SHIPYARD) highlight = 2;

  gui_draw_status_line(&status_line, keys, items, highlight);
}

void gui_bars_map_bottom() {
  WND status_line;
  char* keys[] = {"F1", "F2", "F3/F4", "F5", "F6", "F7", NULL};
  char* items[] = {LC_MAP_STATUS_VIEW,
                   LC_MAP_STATUS_AXIS,
                   LC_MAP_STATUS_ZOOM,
                   LC_MAP_STATUS_GOTO,
                   LC_MAP_STATUS_THREADS,
                   LC_MAP_STATUS_RUN,
                   NULL};

  status_line.x = 0;
  status_line.y = STATUSBAR_BOTTOM_Y;
  status_line.width = STATUSBAR_WIDTH;
  status_line.height = STATUSBAR_HEIGHT;
  status_line.header = NULL;

  gui_draw_status_line(&status_line, keys, items, -1);
}
