/* pathwnd.c -- path side-panel window
 *
 * Private: drawPathWnd(), pathListFlag
 * Public:  pathWnd()
 */

#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"

#include "math\objects.h"

#include "ui\gui.h"
#include "ui\ad.h"
#include "ui\map\pathwnd.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct object* objList;
extern struct game_state gs;
extern int      objSize;
extern int      show_danger_path_parts;

/* ----------------------------------------------------------------
 * One-shot flag: only show the Quindett ad once per path calc
 * ---------------------------------------------------------------- */
static int pathListFlag = 0;

/* ----------------------------------------------------------------
 * Private: drawPathWnd -- background frame of the path panel
 * ---------------------------------------------------------------- */
static void drawPathWnd(struct waypoint* wp, int oy)
{
    extern int WND_WIDTH, WND_HEIGHT;

    setfillstyle(SOLID_FILL, BLACK);
    setcolor(15);
    bar(WND_WIDTH + 1, 21, 638, WND_HEIGHT-1);

    /* Title bar */
    setfillstyle(SOLID_FILL, RED);
    setcolor(0);
    bar(WND_WIDTH + 1, 21, 638, 41);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(WND_WIDTH + 5, 26, "PATH");

    /* Separator + hint */
    setcolor(4);
    setlinestyle(1, 0, 1);
    line(WND_WIDTH + 1, oy, 638, oy);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(WND_WIDTH + 5, oy + 5, "PgUp/PgDn");
    setcolor(15);
    outtextxy(WND_WIDTH + 80, oy + 5, "-SELECT");

    /* Embedded ad (one-shot per path) */
    if (wp->size < 15 && !pathListFlag) {
        adQuindett();
        pathListFlag = 1;
    }

}

/* ----------------------------------------------------------------
 * Public: pathWnd -- draw the path waypoint list
 * ---------------------------------------------------------------- */
void pathWnd(struct waypoint* wp, int currentPoint,
             struct system_solar* ptrList)
{
    int i, j, oy, yStep = 15, jump_possible = 0;
    int o;
    char buf[50];

    extern int WND_WIDTH, WND_HEIGHT;

    pathListFlag = 0;

    oy = 46 + wp->size * yStep;
    drawPathWnd(wp, oy);

    setcolor(15);
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    if (gs.current_system == wp->way[0]){
        jump_possible = 1;
    }

    for (i = 0, j = (wp->size - 1); i < wp->size; i++, j--) {
        if (i == currentPoint) {
            setcolor(0);
            setfillstyle(SOLID_FILL, RED);
            bar(WND_WIDTH + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        } else {
            setcolor(15);
            setfillstyle(SOLID_FILL, BLACK);
            bar(WND_WIDTH + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        }

        if (wp->way[i] == gs.current_system){
            sprintf(buf, "#%d: SA%d << CURRENT", i + 1, wp->way[i]);    
        }
        else if (i == 1 && jump_possible){
            sprintf(buf, "#%d: SA%d << NEXT JUMP", i + 1, wp->way[i]);    
        } else {
            sprintf(buf, "#%d: SA%d", i + 1, wp->way[i]);    
        }

        /* Mark dangerous segments */
        if (i > 0 && show_danger_path_parts && objSize) {
            int prev = wp->way[i - 1];
            int cur  = wp->way[i];
            for (o = 0; o < objSize; o++) {
                if (sphereLineIntersect(
                        ptrList[prev].x, ptrList[prev].y, ptrList[prev].z,
                        ptrList[cur].x,  ptrList[cur].y,  ptrList[cur].z,
                        objList[o].x,  objList[o].y,  objList[o].z,
                        objList[o].r)) {
                    sprintf(buf, "#%d: SA%d [DANGER]", i + 1, wp->way[i]);
                    break;
                }
            }
        }

        outtextxy(WND_WIDTH + 5, 46 + i * yStep, buf);
    }
}
