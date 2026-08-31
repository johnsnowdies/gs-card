/* card.c -- main entry point, event loop, game state
 *
 * Event loop uses enum + switch so that adding a second screen
 * (e.g. cargo / ship management) is a matter of adding a new
 * case branch and a new input-handler function.
 *
 * Game globals are defined here and extern'd where needed.
 */

#include <stdio.h>
#include <graphics.h>
#include <alloc.h>
#include <stdlib.h>

#include "data\structs.h"
#include "data\reader.h"

#include "ui\gui.h"
#include "ui\ad\ad.h"

#include "ui\map\mapnav.h"
#include "ui\menu\menunav.h"
#include "ui\stat\statnav.h"
#include "ui\upgrade\upgrnav.h"
#include "ui\shipyard\syardnav.h"


/* ----------------------------------------------------------------
 * Globals
 * ---------------------------------------------------------------- */
const int DEBUG = 0;

/* Data related global variables */
SYSTEM* sol_list;
OBJECT* obj_list;
BOUND_LINE* bnd_list;
QUEST system_quests[5];
UPGRADE system_upgrades[8];
SHIP system_shipyard[6];

unsigned int sol_size;
unsigned int obj_size;
unsigned int bnd_size;
int system_quests_size = 0;
int system_upgrades_size = 0;
int system_shipyard_size = 0;

WAYPOINT wp;

/* Game state */
GAME_STATE gs;

/* Exit signal */
unsigned char SIG_TERM = 0;

/* Root window */
WND root_wnd = {NULL, 0, 21, 639, 460};

/* Screens */
E_GAME_SCREEN cur_screen = SCR_MAIN_MENU;
E_GAME_SCREEN prev_screen = SCR_MAP;

/* ----------------------------------------------------------------
 * Handlers table
 * ---------------------------------------------------------------- */
typedef int (*key_handler)(int ch, WND* parent);
static key_handler key_handlers[] = {
    gui_map_wnd_key,          /* SCR_MAP */
    gui_menu_main_wnd_key,    /* SCR_MAIN_MENU */
    gui_menu_game_wnd_key,    /* SCR_GAME_MENU */
    gui_status_wnd_key,       /* SCR_STATUS */
    gui_upgrade_wnd_key,       /* SCR_UPGRADE */
    gui_shipyard_wnd_key      /* SCR_SHIPYARD */
};


/* ----------------------------------------------------------------
 * main
 * ---------------------------------------------------------------- */
int main() {
  char c;
  srand((unsigned)time(NULL));

  sol_size = data_reader_load_systems(&sol_list);
  bnd_size = data_reader_load_bounds(&bnd_list);
  obj_size = data_reader_load_objects(&obj_list);

  gui_init();

  if (!DEBUG) {
    /* Splash screen */
    data_reader_draw_bmp("LOGO.BMP", 0, 0);
  }

  core_finder_calc_hyper_threads();
 
  gui_clrscr();

  /* Show ads */
  gui_ad_loading();

  /* Draw Main Menu */
  gui_menu_wnd(&root_wnd, 0, MAIN_MENU);

  while (!SIG_TERM) {
    c = getch();
    if (cur_screen >= 0 && cur_screen < 6) {
      key_handlers[cur_screen](c, &root_wnd);
    }
  }

  /* Cleanup */
  if (sol_list) free(sol_list);
  if (obj_list) free(obj_list);
  if (bnd_list) free(bnd_list);
  closegraph();
  return 0;
}
