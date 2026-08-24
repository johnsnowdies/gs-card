#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "ui\menu\menuwnd.h"


/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */

extern unsigned int sol_size;

char* MAIN_MENU_ITEMS[3] = {
    LC_MENU_NEW_GAME,
    LC_MENU_LOAD,
    LC_MENU_EXIT
};

char* GAME_MENU_ITEMS[3] = {
    LC_MENU_SAVE,
    LC_MENU_LOAD,
    LC_MENU_EXIT
};

int mm_select = 0;


void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode)
{
    WND menu_wnd;
    int i = 0;
    int wx = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    int wy = ((ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2) + 25;
    char **ITEMS;

    if (mode == MAIN_MENU)
        ITEMS = MAIN_MENU_ITEMS;
    else
        ITEMS = GAME_MENU_ITEMS;

    menu_wnd.header = "GS-CARD v1.5";

    menu_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    menu_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;
    menu_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    menu_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    gui_draw_wnd_proto(&menu_wnd);

    setcolor(4);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    
    for (i = 0; i < 3; i++){
        if (i == currentPos){
            setfillstyle(SOLID_FILL, RED);
            bar(wx+10, wy+(20*i), wx + WND_MODAL_DEFAULT_WIDTH - 10, wy+(20*(i+1)));
            setcolor(0);
        }
        else
        {
            setcolor(RED);
        }

        outtextxy(wx+20, wy+5+(20*i), ITEMS[i]);
    }
}
