#include <graphics.h>
#include <stdio.h>
#include "ui/ad/ad.h"
#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/locale.h"
#include "ui/menu/menuwnd.h"

static char* MAIN_MENU_ITEMS[3] = {LC_MENU_NEW_GAME, LC_MENU_LOAD, LC_MENU_EXIT};

static char* GAME_MENU_ITEMS[3] = {LC_MENU_SAVE, LC_MENU_LOAD, LC_MENU_EXIT};

int ad_rendered = 0;

int mm_select = 0;

static void gui_menu_draw(WND* ptr_parent, int mode){
  WND menu_wnd;
  int i = 0;
  int wx = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
  int wy = ((ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2) + 25;
  char** ITEMS;

  if (mode == MAIN_MENU)
    ITEMS = MAIN_MENU_ITEMS;
  else
    ITEMS = GAME_MENU_ITEMS;

  menu_wnd.header = "GS-CARD v1.5";
  menu_wnd.id = SCR_MAIN_MENU;

  menu_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
  menu_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;
  menu_wnd.width = WND_MODAL_DEFAULT_WIDTH;
  menu_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

  gui_draw_wnd_proto(&menu_wnd);

  setcolor(4);
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);

  for (i = 0; i < 3; i++) {
    if (i == mm_select) {
      setfillstyle(SOLID_FILL, RED);
      bar(wx + 10, wy + (20 * i), wx + WND_MODAL_DEFAULT_WIDTH - 10,
          wy + (20 * (i + 1)));
      setcolor(0);
    } else {
      setcolor(RED);
    }

    outtextxy(wx + 20, wy + 5 + (20 * i), ITEMS[i]);
  }
}


/* ----------------------------------------------------------
 * EXTERNAL: SCR_MENU -- WINDOW DISPATCHER
 * ---------------------------------------------------------- */
void gui_menu_main_wnd_dispatcher(WND* ptr_parent) {
  if (!ad_rendered){
    gui_ad_loading();
    ad_rendered = 1;
  }
  gui_menu_draw(ptr_parent, MAIN_MENU);
}

void gui_menu_game_wnd_dispatcher(WND* ptr_parent) {
  gui_menu_draw(ptr_parent, GAME_MENU);
}
