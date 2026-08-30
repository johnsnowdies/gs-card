/* pathwnd.c -- path side-panel window
 *
 * Private: drawgui_map_path_wnd(), pathListFlag
 * Public:  gui_map_path_wnd()
 */

#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "core\objects.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "ui\ad\ad.h"
#include "ui\map\pathwnd.h"
#include "ui\map\mapwnd.h"


/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct object* obj_list;
extern unsigned int obj_size;

extern struct game_state gs;

extern SYSTEM* sol_list;
extern unsigned int sol_size;

extern WAYPOINT wp;

/* ----------------------------------------------------------------
 * One-shot flag: only show the Quindett ad once per path calc
 * ---------------------------------------------------------------- */
static int pathListFlag = 0;

extern int path_wnd_index;

extern WND map_wnd;

/* ----------------------------------------------------------------
 * Private: drawPathWnd -- background frame of the path panel
 * ---------------------------------------------------------------- */
static void draw_gui_map_path_wnd(int oy)
{
    setfillstyle(SOLID_FILL, BLACK);
    setcolor(15);
    bar(map_wnd.width + 1, 21, 638, map_wnd.height-1);

    /* Title bar */
    setfillstyle(SOLID_FILL, RED);
    setcolor(0);
    bar(map_wnd.width + 1, 21, 638, 41);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(map_wnd.width + 5, 26, LC_PATH_WND_HEAD);

    /* Separator + hint */
    setcolor(4);
    setlinestyle(1, 0, 1);
    line(map_wnd.width + 1, oy, 638, oy);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(map_wnd.width + 5, oy + 5, "PgUp/PgDn");
    setcolor(15);
    outtextxy(map_wnd.width + 80, oy + 5, LC_PATH_WND_SELECT);

    /* Embedded ad (one-shot per path) */
    if (wp.size < 15 && !pathListFlag) {
        gui_ad_quindett();
        pathListFlag = 1;
    }
}

/* ----------------------------------------------------------------
 * Public: pathWnd -- draw the path waypoint list
 * ---------------------------------------------------------------- */
void gui_map_path_wnd()
{
    int i, j, oy, yStep = 15, jump_possible = 0;
    int o;
    char buf[50];


    pathListFlag = 0;

    oy = 46 + wp.size * yStep;
    draw_gui_map_path_wnd( oy);

    setcolor(15);
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    if (gs.current_system == wp.way[0]){
        jump_possible = 1;
    }

    for (i = 0, j = (wp.size - 1); i < wp.size; i++, j--) {
        if (i == path_wnd_index) {
            setcolor(0);
            setfillstyle(SOLID_FILL, RED);
            bar(map_wnd.width + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        } else {
            setcolor(15);
            setfillstyle(SOLID_FILL, BLACK);
            bar(map_wnd.width + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        }

        if (wp.way[i] == gs.current_system){
            sprintf(buf, "#%d: SA%d %s", i + 1, wp.way[i], LC_PATH_WND_CURRENT);    
        }
        else if (i == 1 && jump_possible){
            sprintf(buf, "#%d: SA%d %s", i + 1, wp.way[i], LC_PATH_WND_NEXT_JUMP);    
        } else {
            sprintf(buf, "#%d: SA%d", i + 1, wp.way[i]);    
        }

        /* Mark dangerous segments */
        if (i > 0 && gs.upgrade_objects_map && obj_size) {
            int prev = wp.way[i - 1];
            int cur  = wp.way[i];
            for (o = 0; o < obj_size; o++) {
                if (core_objects_sphere_line_intersect(
                        sol_list[prev].x, sol_list[prev].y, sol_list[prev].z,
                        sol_list[cur].x,  sol_list[cur].y,  sol_list[cur].z,
                        obj_list[o].x,  obj_list[o].y,  obj_list[o].z,
                        obj_list[o].r)) {
                    sprintf(buf, "#%d: SA%d [%s]", i + 1, wp.way[i], LC_PATH_WND_DANGER);
                    break;
                }
            }
        }

        outtextxy(map_wnd.width + 5, 46 + i * yStep, buf);
    }
}
