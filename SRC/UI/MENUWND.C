#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\menuwnd.h"

#include "ui\locale.h"


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


void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode)
{
    WND menu_wnd;
    int i = 0;
    int wx = (ptr_parent->width - WND_W) / 2;
    int wy = WND_DEFAULT_Y + 25;
    char **ITEMS;

    if (mode == MAIN_MENU)
        ITEMS = MAIN_MENU_ITEMS;
    else
        ITEMS = GAME_MENU_ITEMS;

    menu_wnd.header = "GS-CARD v1.5";

    menu_wnd.x = (ptr_parent->width - WND_W) / 2;
    menu_wnd.y = WND_MODAL_DEFAULT_Y;
    menu_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    menu_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    gui_draw_wnd_proto(&menu_wnd);

    setcolor(4);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    
    for (i = 0; i < 3; i++){
        if (i == currentPos){
            setfillstyle(SOLID_FILL, RED);
            bar(wx+10, wy+(20*i), wx + WND_W - 10, wy+(20*(i+1)));
            setcolor(0);
        }
        else
        {
            setcolor(BAR_COLOR);
        }

        outtextxy(wx+20, wy+5+(20*i), ITEMS[i]);
    }
}
