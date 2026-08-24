/* card.c -- main entry point, event loop, game state
 *
 * Event loop uses enum + switch so that adding a second screen
 * (e.g. cargo / ship management) is a matter of adding a new
 * case branch and a new input-handler function.
 *
 * Game globals are defined here and extern'd where needed.
 */

#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>

#include "data\keys.h"
#include "data\structs.h"
#include "data\reader.h"

#include "core\objects.h"
#include "core\finder.h"
#include "core\game.h"

#include "ui\ad.h"
#include "ui\splash.h"
#include "ui\gui.h"
#include "ui\menuwnd.h"
#include "ui\statwnd.h"
#include "ui\map\mapwnd.h"
#include "ui\map\pathwnd.h"
#include "ui\map\nav.h"

#include "ui\locale.h"

unsigned _stklen = 16384;

const int DEBUG = 1;

/* ----------------------------------------------------------------
 * Game globals
 * ---------------------------------------------------------------- */
SYSTEM* sol_list;
OBJECT* obj_list;
BOUND_LINE* bnd_list;
QUEST system_quests[5];

unsigned int sol_size;
unsigned int obj_size;
unsigned int bnd_size;
unsigned int system_quests_size = 0;

WAYPOINT wp;

/* Render flags */
unsigned char render_bounds = 1;
unsigned char render_danger_objects = 0;
unsigned char show_danger_hyperthreads = 0;
unsigned char show_danger_path_parts = 0;

/* Game state */
struct game_state gs;

/* Exit signal */
unsigned char SIG_TERM = 0;


/* ----------------------------------------------------------------
 * Screen enumeration -- add new screens here
 * ---------------------------------------------------------------- */

enum game_screen cur_screen = SCR_MAIN_MENU;
enum game_screen prev_screen = SCR_MAP;

/* ----------------------------------------------------------------
 * Handlers table
 * ---------------------------------------------------------------- */
typedef int (*key_handler)(int ch, WND *parent);
key_handler key_handlers[] = {
    gui_map_wnd_key,        /* SCR_MAP */
    gui_main_menu_key,      /* SCR_MAIN_MENU */
    gui_game_menu_key,      /* SCR_GAME_MENU */
    gui_status_wnd_key,     /* SCR_STATUS */
    gui_quest_detail_key    /* SCR_QUEST_LIST_DETAIL */
};


/* ----------------------------------------------------------------
 * main
 * ---------------------------------------------------------------- */
int main()
{
    char c;
    WND root_wnd = {NULL, 0, 21, 639, 460};

    srand((unsigned)time(NULL));

    sol_size = load_solar(&sol_list);
    bnd_size = load_bounds(&bnd_list);

    gui_init();

    /* Splash screen */
    draw_4bit_bmp("LOGO.BMP",0,0);
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, 640, 480);

    /* Show ads */
    gui_ad_loading();
    
    /* Draw Main Menu */
    gui_menu_wnd(&root_wnd, 0, MAIN_MENU);

    while (!SIG_TERM) {
        c = getch();
        if (cur_screen >= 0 && cur_screen < 5) {
            key_handlers[cur_screen](c, &root_wnd);
        }
    }

    /* Cleanup */
    free(sol_list);
    if (obj_list) free(obj_list);
    closegraph();
    return 0;
}
