#include <graphics.h>
#include <stdio.h>

#include "structs.h"
#include "objects.h"
#include "gui.h"
#include "reader.h"

/* ----------------------------------------------------------------
 * Layout constants
 * ---------------------------------------------------------------- */
#define WND_W         320    /* default window width  */
#define WND_H         100    /* default window height */
#define WND_DEFAULT_Y 180    /* default window Y      */

#define BAR_COLOR      4     /* highlight colour    */
#define TEXT_COLOR    15     /* normal text colour  */


const char *ITEMS[3] = {
    "New Game",
    "Load",
    "Exit to DOS"
};

static void drawMainMenuWnd()
{
    int wx = (WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;

    drawWnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, "GS-CARD v1.5");
    setcolor(BAR_COLOR);
    
    moveto(wx + 5, wy + 40);
    setcolor(TEXT_COLOR);
}

void mainMenuWnd(int currentPos)
{
    int i = 0;
    int wx = (WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y + 25;

    drawMainMenuWnd();

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
