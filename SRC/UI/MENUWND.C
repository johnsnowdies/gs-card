#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\menuwnd.h"

char* MAIN_MENU_ITEMS[3] = {
    "New Game",
    "Load",
    "Exit to DOS"
};

char* GAME_MENU_ITEMS[3] = {
    "Save Game",
    "Load",
    "Exit to DOS"
};

static void drawgui_menu_wnd()
{
    int wx = (MAP_WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;

    gui_draw_generic_wnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, "GS-CARD v1.5");
    setcolor(BAR_COLOR);
    
    moveto(wx + 5, wy + 40);
    setcolor(TEXT_COLOR);
}

void gui_menu_wnd(int currentPos, int mode)
{
    int i = 0;
    int wx = (MAP_WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y + 25;
    char **ITEMS;
    if (mode == MAIN_MENU)
        ITEMS = MAIN_MENU_ITEMS;
    else
        ITEMS = GAME_MENU_ITEMS;

    drawgui_menu_wnd();

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
