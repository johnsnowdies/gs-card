#include <graphics.h>
#include <stdio.h>

#include "structs.h"
#include "gui.h"

/* ----------------------------------------------------------------
 * Screen / viewport layout -- defined here, declared extern in gui.h
 * ---------------------------------------------------------------- */
int STATUS_WND_WIDTH  = 639;   /* may shrink to 470 when path panel is open */
int STATUS_WND_HEIGHT = 460;

/* Viewport bounds for clipping (status window only) */
static struct status_wnd {
    int x1, y1, x2, y2;
} status_wnd = { 0, 21, 639, 460 };



void statusWnd()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, STATUS_WND_WIDTH - 1, STATUS_WND_HEIGHT - 1);

    topStatusLine();
}
