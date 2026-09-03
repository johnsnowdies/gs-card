#include <stdio.h>

#include "core/events.h"
#include "core/game.h"
#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/locale.h"
#include "ui/map/mapnav.h"
#include "ui/map/mapwnd.h"
#include "ui/menu/menuwnd.h"

/* ----------------------------------------------------------------
 * SCALE DOWN MAP
 * ---------------------------------------------------------------- */
static void gui_map_nav_scale_minus() {
  if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3) {
    xmax = xmax + MAX_VALUE / 10;
    ymax = ymax + MAX_VALUE / 10;
    zmax = zmax + MAX_VALUE / 10;

    xmin = xmin - MAX_VALUE / 10;
    ymin = ymin - MAX_VALUE / 10;
    zmin = zmin - MAX_VALUE / 10;
  } else {
    gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_NAV_ERROR_1, SOUND_ERROR);
  }
}

/* ----------------------------------------------------------------
 * SCALE UP MAP
 * ---------------------------------------------------------------- */
static void gui_map_nav_scale_plus() {
  if (xmax > MAX_VALUE / 10 && xmin < MIN_VALUE / 10) {
    xmax = xmax - MAX_VALUE / 10;
    ymax = ymax - MAX_VALUE / 10;
    zmax = zmax - MAX_VALUE / 10;

    xmin = xmin + MAX_VALUE / 10;
    ymin = ymin + MAX_VALUE / 10;
    zmin = zmin + MAX_VALUE / 10;
  } else {
    gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_NAV_ERROR_2, SOUND_ERROR);
  }
}

/* ----------------------------------------------------------------
 * OFFSET MAP
 * ---------------------------------------------------------------- */
static void gui_map_nav_offset_x_plus() { offsetX += xmax / 10; }
static void gui_map_nav_offset_x_minus() { offsetX -= xmax / 10; }
static void gui_map_nav_offset_y_plus() { offsetY += ymax / 10; }
static void gui_map_nav_offset_y_minus() { offsetY -= ymax / 10; }
static void gui_map_nav_offset_z_plus() { offsetZ += zmax / 10; }
static void gui_map_nav_offset_z_minus() { offsetZ -= zmax / 10; }

/* ----------------------------------------------------------------
 * PROMPT FOR SYSTEM NUMBER, THEN JUMP
 * ---------------------------------------------------------------- */
static void gui_map_nav_goto_system() {
  char* input;
  int value;
  int error = 0;
  char buf[50];

  /* Current system is default in patch 1.5 */
  sprintf(buf, "%d", gs.current_system);

  input = (char*)gui_input_wnd(&map_wnd, LC_NAV_NORMAL_HEAD, LC_NAV_INPUT_COORD,
                               buf);
  value = atoi(input);
  free(input);

  if (value != 0) {
    if (value > sol_size || value < 1) {
      error = 1;
    } else {
      gui_map_nav_move_screen_to(value);
    }
  } else {
    error = 1;
  }

  if (error) {
    gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE,
                    SOUND_ERROR);
  }
}

static void gui_map_nav_hyperjump() {
  char buf[128];
  sprintf(buf, LC_CARD_READY_TO_JUMP, wp.way[0], wp.way[1]);
  if (gui_confirm_wnd(&map_wnd, LC_CARD_JUMP_WND_HEAD, buf) == 0) {
    /* JUMP CONFIRMED */
    int game_over = 0;
    int i;

    gs.prev_system = gs.current_system;
    gs.current_system = wp.way[1];

    /* Keep Path Window open until reached end */
    if (wp.size != 2 && gs.upgrade_continuous_jump) {
      for (i = 0; i <= wp.size - 1; i++) wp.way[i] = wp.way[i + 1];
      wp.size--;
    } else {
      wp.size = 0;
    }

    dispatch_wnd(SCR_MAP);

    /* Run game new system events */
    game_over = core_events(1);
    gui_map_nav_move_screen_to(gs.current_system);

    if (!game_over) {
      dispatch_wnd(SCR_MAP);
    } else {
      dispatch_wnd(SCR_MAIN_MENU);
    }
  } else {
    wp.size = 0;
    dispatch_wnd(SCR_MAP);
  }
}

/* ----------------------------------------------------------------
 *
 *                      EXTERNAL FUNCTIONS
 *
 * ---------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * CENTRE VIEW ON A SYSTEM
 * ---------------------------------------------------------------- */
void gui_map_nav_move_screen_to(int value) {
  offsetX = -1 * sol_list[value].x;
  offsetY = -1 * sol_list[value].y;
  offsetZ = -1 * sol_list[value].z;

  xmax = MAX_VALUE / 10;
  ymax = MAX_VALUE / 10;
  zmax = MAX_VALUE / 10;

  xmin = MIN_VALUE / 10;
  ymin = MIN_VALUE / 10;
  zmin = MIN_VALUE / 10;
}

/* ----------------------------------------------------------------
 * SCR_MAP -- MAP WINDOW KEY HANDLER
 * ---------------------------------------------------------------- */

int gui_map_wnd_key(int ch) {
  if (F1 == ch) {
    mode = (mode < 3) ? mode + 1 : 1;
    gui_map_wnd_refresh();
  }
  if (F2 == ch) {
    is_coord = !is_coord;
    gui_map_wnd_refresh();
  }
  if (F4 == ch) {
    gui_map_nav_scale_plus();
    gui_map_wnd_refresh();
  }
  if (F3 == ch) {
    gui_map_nav_scale_minus();
    gui_map_wnd_refresh();
  }
  if (F5 == ch) {
    gui_map_nav_goto_system(sol_size, sol_list);
    gui_map_wnd_refresh();
  }
  if (F6 == ch) {
    is_hyper = !is_hyper;
    gui_map_wnd_refresh();
  }
  if (F7 == ch) {
    if (core_finder_get_way(&wp)) {
      gui_map_wnd_refresh();
      gui_map_path_wnd();
    } else {
      gui_map_wnd_refresh();
    }
  }
  if (LFT == ch) {
    gui_map_nav_offset_x_plus();
    gui_map_wnd_refresh();
  }
  if (RHT == ch) {
    gui_map_nav_offset_x_minus();
    gui_map_wnd_refresh();
  }
  if (UP == ch) {
    if (mode == 3 || mode == 2) gui_map_nav_offset_z_minus();
    if (mode == 1 || mode == 2) gui_map_nav_offset_y_plus();
    gui_map_wnd_refresh();
  }
  if (DWN == ch) {
    if (mode == 3 || mode == 2) gui_map_nav_offset_z_plus();
    if (mode == 1 || mode == 2) gui_map_nav_offset_y_minus();
    gui_map_wnd_refresh();
  }
  if (PUP == ch) {
    if (wp.size) {
      if (path_wnd_index == -1 || path_wnd_index == 0)
        path_wnd_index = (wp.size - 1);
      else
        path_wnd_index--;
      gui_map_nav_move_screen_to(wp.way[path_wnd_index]);
      gui_map_path_wnd();
      gui_map_wnd_refresh();
      sfx_menu_move();
    }
  }
  if (PDWN == ch) {
    if (wp.size) {
      if (path_wnd_index == -1 || path_wnd_index == (wp.size - 1))
        path_wnd_index = 0;
      else
        path_wnd_index++;
      gui_map_nav_move_screen_to(wp.way[path_wnd_index]);
      gui_map_path_wnd();
      gui_map_wnd_refresh();
      sfx_menu_move();
    }
  }

  if (TAB == ch) {
    dispatch_wnd(SCR_STATUS);
  }
  if (ENTER == ch) {
    /* ----------------------------------------------------------------
     * WARNING: HYPER JUMP INITIATED!
     * ---------------------------------------------------------------- */
    if (wp.size > 1 && wp.way[0] == gs.current_system) {
      gui_map_nav_hyperjump();
    }
  }
  if (ESC == ch) {
    if (wp.size) {
      wp.size = 0;
      gui_map_wnd_refresh();
    } else {
      dispatch_wnd(SCR_GAME_MENU);
    }
  }
  return 0;
}
