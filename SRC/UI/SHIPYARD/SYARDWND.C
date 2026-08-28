#include <graphics.h>
#include <stdio.h>
#include <string.h>


#include "data\structs.h"
#include "data\reader.h"
#include "core\game.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\shipyard\syardwnd.h"
#include "ui\ad\ad.h"
#include "music.h"

WND shipyard_wnd;
int ship_selected;


void gui_shipyard_draw_list()
{

}

void gui_shipyard_wnd()
{
    shipyard_wnd.x = SYARD_WND_DEFAULT_X;
    shipyard_wnd.y = SYARD_WND_DEFAULT_Y;
    shipyard_wnd.width = SYARD_WND_DEFAULT_WIDTH;
    shipyard_wnd.height = SYARD_WND_DEFAULT_HEIGHT;
    shipyard_wnd.header = NULL;

    gui_draw_wnd_proto(&shipyard_wnd);

    
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, shipyard_wnd.y, shipyard_wnd.width - 1, shipyard_wnd.y + shipyard_wnd.height - 1);

    gui_ad_hypersoft();

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);

    outtextxy(80,25, "SHIPYARD");

    gui_bars_status_bottom();
}
