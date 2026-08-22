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
#include "ui\ad.h"
#include "ui\map\pathwnd.h"

#include "ui\locale.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct object* obj_list;
extern struct game_state gs;
extern unsigned int obj_size;
extern unsigned char show_danger_path_parts;

/* ----------------------------------------------------------------
 * One-shot flag: only show the Quindett ad once per path calc
 * ---------------------------------------------------------------- */
static int pathListFlag = 0;

/* ----------------------------------------------------------------
 * Private: drawPathWnd -- background frame of the path panel
 * ---------------------------------------------------------------- */
static void drawgui_map_path_wnd(struct waypoint* wp, int oy)
{
    extern int MAP_WND_WIDTH, MAP_WND_HEIGHT;

    setfillstyle(SOLID_FILL, BLACK);
    setcolor(15);
    bar(MAP_WND_WIDTH + 1, 21, 638, MAP_WND_HEIGHT-1);

    /* Title bar */
    setfillstyle(SOLID_FILL, RED);
    setcolor(0);
    bar(MAP_WND_WIDTH + 1, 21, 638, 41);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(MAP_WND_WIDTH + 5, 26, LC_PATH_WND_HEAD);

    /* Separator + hint */
    setcolor(4);
    setlinestyle(1, 0, 1);
    line(MAP_WND_WIDTH + 1, oy, 638, oy);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(MAP_WND_WIDTH + 5, oy + 5, "PgUp/PgDn");
    setcolor(15);
    outtextxy(MAP_WND_WIDTH + 80, oy + 5, LC_PATH_WND_SELECT);

    /* Embedded ad (one-shot per path) */
    if (wp->size < 15 && !pathListFlag) {
        gui_ad_quindett();
        pathListFlag = 1;
    }
}

/* ----------------------------------------------------------------
 * Public: pathWnd -- draw the path waypoint list
 * ---------------------------------------------------------------- */
void gui_map_path_wnd(struct waypoint* wp, int current_point,
             struct system_solar* sol_list)
{
    int i, j, oy, yStep = 15, jump_possible = 0;
    int o;
    char buf[50];

    extern int MAP_WND_WIDTH, MAP_WND_HEIGHT;

    pathListFlag = 0;

    oy = 46 + wp->size * yStep;
    drawgui_map_path_wnd(wp, oy);

    setcolor(15);
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    if (gs.current_system == wp->way[0]){
        jump_possible = 1;
    }

    for (i = 0, j = (wp->size - 1); i < wp->size; i++, j--) {
        if (i == current_point) {
            setcolor(0);
            setfillstyle(SOLID_FILL, RED);
            bar(MAP_WND_WIDTH + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        } else {
            setcolor(15);
            setfillstyle(SOLID_FILL, BLACK);
            bar(MAP_WND_WIDTH + 1, 46 + i * yStep,
                638, 46 + i * yStep + 15);
        }

        if (wp->way[i] == gs.current_system){
            sprintf(buf, "#%d: SA%d %s", i + 1, wp->way[i], LC_PATH_WND_CURRENT);    
        }
        else if (i == 1 && jump_possible){
            sprintf(buf, "#%d: SA%d %s", i + 1, wp->way[i], LC_PATH_WND_NEXT_JUMP);    
        } else {
            sprintf(buf, "#%d: SA%d", i + 1, wp->way[i]);    
        }

        /* Mark dangerous segments */
        if (i > 0 && show_danger_path_parts && obj_size) {
            int prev = wp->way[i - 1];
            int cur  = wp->way[i];
            for (o = 0; o < obj_size; o++) {
                if (core_objects_sphere_line_intersect(
                        sol_list[prev].x, sol_list[prev].y, sol_list[prev].z,
                        sol_list[cur].x,  sol_list[cur].y,  sol_list[cur].z,
                        obj_list[o].x,  obj_list[o].y,  obj_list[o].z,
                        obj_list[o].r)) {
                    sprintf(buf, "#%d: SA%d [%s]", i + 1, wp->way[i], LC_PATH_WND_DANGER);
                    break;
                }
            }
        }

        outtextxy(MAP_WND_WIDTH + 5, 46 + i * yStep, buf);
    }
}
