/* mapwnd.c -- map viewport rendering, projection, clipping
 *
 * Private (static): safe_putpixel, safe_line, safe_outtextxy, safe_circle,
 *   p(), ex(), ey(), drawObjects, draw2dwnd, draw3dwnd, drawyzwnd, POINT_COLOR
 *
 * Public: gui_map_wnd_draw(), gui_map_wnd_clear()
 */

#include <alloc.h>
#include <graphics.h>

#include "data\structs.h"
#include "data\keys.h"

#include "core\objects.h"
#include "core\game.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "ui\ad\ad.h"
#include "ui\map\mapwnd.h"
#include "ui\map\pathwnd.h"

/* ----------------------------------------------------------------
 * Globals
 * ---------------------------------------------------------------- */

WND map_wnd = {NULL, 0, 21, 639, 460};
int is_coord = 1, is_hyper = 0, mode = 1; 
int path_wnd_index = 0;

unsigned char render_danger_objects;
unsigned char render_bounds = 1;
unsigned char show_danger_hyperthreads;
unsigned char show_danger_path_parts;

/* Projection bounds */
float xmin = MIN_VALUE, xmax = MAX_VALUE;
float ymin = MIN_VALUE, ymax = MAX_VALUE;
float zmin = MIN_VALUE, zmax = MAX_VALUE;

int  offsetX = 0, offsetY = 0, offsetZ = 0;
float xdens, ydens;


/* Colour computed as a side-effect of p() */
static int POINT_COLOR = 0;

/* Viewport bounds for clipping (map window only) */
static struct map_bounds {
    int x1, y1, x2, y2;
} map_bounds = { 0, 21, 639, 460 };


/* ----------------------------------------------------------------
 * Extern globals
 * ---------------------------------------------------------------- */

extern WAYPOINT wp;

extern SYSTEM* sol_list;
extern unsigned int sol_size;

extern OBJECT* obj_list;
extern unsigned int obj_size;

extern BOUND_LINE* bnd_list;
extern unsigned int bnd_size;

extern GAME_STATE gs;

extern char* data_factions[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern unsigned int data_factions_colors[FACTIONS_COUNT];

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;



/* ----------------------------------------------------------------
 * Clipping helpers (safe_*) -- clip against map_bounds
 * ---------------------------------------------------------------- */
static void safe_putpixel(int x, int y, int c)
{
    if (x >= map_bounds.x1 && x <= map_bounds.x2 &&
        y >= map_bounds.y1 && y <= map_bounds.y2)
        putpixel(x, y, c);
}

static void safe_line(int x1, int y1, int x2, int y2)
{
    int accept = 0, done = 0;
    int code1, code2, code;
    int *xp, *yp;
    float dy, dx;

    /* Quick reject */
    if (x1 < map_bounds.x1 && x2 < map_bounds.x1) return;
    if (x1 > map_bounds.x2 && x2 > map_bounds.x2) return;
    if (y1 < map_bounds.y1 && y2 < map_bounds.y1) return;
    if (y1 > map_bounds.y2 && y2 > map_bounds.y2) return;

    /* Cohen-Sutherland */
    while (!done) {
        code1 = 0; code2 = 0;
        if (y1 < map_bounds.y1) code1 |= 1;
        if (y1 > map_bounds.y2) code1 |= 2;
        if (y2 < map_bounds.y1) code2 |= 1;
        if (y2 > map_bounds.y2) code2 |= 2;
        if (x1 < map_bounds.x1) code1 |= 4;
        if (x1 > map_bounds.x2) code1 |= 8;
        if (x2 < map_bounds.x1) code2 |= 4;
        if (x2 > map_bounds.x2) code2 |= 8;

        if (code1 == 0 && code2 == 0) { accept = 1; done = 1; }
        else if (code1 & code2)        { done = 1; }
        else {
            code = code1 ? code1 : code2;
            xp = (code == code1) ? &x1 : &x2;
            yp = (code == code1) ? &y1 : &y2;
            dy = (float)(y2 - y1);
            dx = (float)(x2 - x1);

            if      (code & 1) { *yp = map_bounds.y1; if (dy) *xp = x1 + dx * (map_bounds.y1 - y1) / dy; }
            else if (code & 2) { *yp = map_bounds.y2; if (dy) *xp = x1 + dx * (map_bounds.y2 - y1) / dy; }
            else if (code & 4) { *xp = map_bounds.x1; if (dx) *yp = y1 + dy * (map_bounds.x1 - x1) / dx; }
            else if (code & 8) { *xp = map_bounds.x2; if (dx) *yp = y1 + dy * (map_bounds.x2 - x1) / dx; }
        }
    }
    if (accept) line(x1, y1, x2, y2);
}

static void safe_outtextxy(int x, int y, char* s)
{
    if (x >= map_bounds.x1 && x <= map_bounds.x2 &&
        x < map_bounds.x2 - textwidth(s) &&
        y >= map_bounds.y1 && y <= map_bounds.y2 - 60 &&
        y < map_bounds.y2 - textheight(s) )
        outtextxy(x, y, s);
}

static void safe_circle(int x, int y, int r)
{
    if (x - r >= map_bounds.x1 && x + r <= map_bounds.x2 &&
        y - r >= map_bounds.y1 && y + r <= map_bounds.y2)
        circle(x, y, r);
}

/* ----------------------------------------------------------------
 * Projection helpers
 * ---------------------------------------------------------------- */
static int ex(float x)
{
    return (int)((x - xmin) / xdens);
}

static int ey(float y)
{
    y = -1 * y;
    return (int)((ymax - y) / ydens);
}

static int get_color_by_z(float z)
{
    /* Depth-colouring */
    if      (z <= -200) return 1;
    else if (z <= -150) return 5;
    else if (z <= -100) return 3;
    else if (z <= -50)  return 9;
    else if (z < 0)     return 7;
    else if (z == 0)    return 7;
    else if (z < 50)    return 7;
    else if (z < 100)   return 8;
    else if (z < 150)   return 14;
    else if (z < 200)   return 12;
    else                return 4;
}

static POINT p(float x, float y, float z)
{
    float b = 0.8660254F * y;
    float a = y / 2.0F;
    POINT res;

    res.x = (int)(map_wnd.width  / 2 + (x - a) / xdens);
    res.y = (int)(map_wnd.height / 2 + (b / ydens) - (z / ydens));

    POINT_COLOR = get_color_by_z(z);

    return res;
}

/* ----------------------------------------------------------------
 * drawObjects -- gas clouds, black holes, nebulae
 * ---------------------------------------------------------------- */
static void drawObjects(int mode)
{
    int i, color;

    if (!obj_size || !obj_list || !render_danger_objects) return;

    for (i = 0; i < obj_size; i++) {
        int cx, cy, rx, ry;
        int r = obj_list[i].r;

        switch (obj_list[i].type) {
            case OBJ_GASCLOUD:  color = 5; break;
            case OBJ_BLACKHOLE: color = 4; break;
            case OBJ_NEBULA:    color = 9; break;
            default:            color = 5; break;
        }

        if (mode == 1) {
            cx = ex(obj_list[i].x + offsetX);
            cy = ey(obj_list[i].y + offsetY);
        } else if (mode == 2) {
            POINT c = p(obj_list[i].x + offsetX,
                               obj_list[i].y + offsetY,
                               obj_list[i].z + offsetZ);
            cx = c.x;  cy = c.y;
        } else {
            cx = ex(obj_list[i].x + offsetX);
            cy = ey(-1 * (obj_list[i].z + offsetZ));
        }

        rx = (int)(r / xdens);  if (rx < 2) rx = 2;
        ry = (int)(r / ydens);  if (ry < 2) ry = 2;

        setlinestyle(1, 0, 1);
        setcolor(color);
        safe_circle(cx, cy, rx > ry ? rx : ry);
        setlinestyle(0, 0, 1);
    }
}

static void draw_bounds()
{
    int i;
    if (!bnd_size || !bnd_list || !render_bounds) return;

    setlinestyle(1, 0, 1);
    setcolor(6);

    for (i = 0; i < bnd_size; i++) {
        int x1 = ex(bnd_list[i].a->x + offsetX);
        int y1 = ey(bnd_list[i].a->y + offsetY);
        int x2 = ex(bnd_list[i].b->x + offsetX);
        int y2 = ey(bnd_list[i].b->y + offsetY);
        safe_line(x1, y1, x2, y2);
    }

    setlinestyle(0, 0, 1);
}

/* ----------------------------------------------------------------
 * draw2dwnd -- XY projection (top-down)
 * ---------------------------------------------------------------- */
static void draw2dwnd(int isCoord, int isHyper, WAYPOINT* wp)
{
    int i, j, o;
    SYSTEM buf, a, b;
    int drawThreads = isHyper;
    char c[50] = "";

    xdens = (xmax - xmin) / map_wnd.width;
    ydens = (ymax - ymin) / map_wnd.height;

    if (isCoord) {
        int x0 = ex(0 + offsetX);
        int y0 = ey(0 + offsetY);

        setcolor(15);
        setlinestyle(0, 0, 1);
        safe_line(x0, y0, map_wnd.width, y0);
        safe_line(x0, y0, x0, 0);

        setlinestyle(1, 0, 1);
        safe_line(0, y0, x0, y0);
        safe_line(x0, map_wnd.height, x0, y0);
    }

    for (i = 0; i < sol_size; i++) {
        p(sol_list[i].x, sol_list[i].y, sol_list[i].z);
        safe_putpixel(ex(sol_list[i].x + offsetX), ey(sol_list[i].y + offsetY),
                      POINT_COLOR);

        if (sol_list[i].threadSize && drawThreads) {
            for (j = 0; j < sol_list[i].threadSize; j++) {
                if (sol_list[i].threads[j].cost >= 15) continue;

                setcolor(get_color_by_z(sol_list[i].z));
                setlinestyle(1, 0, 1);
                buf = sol_list[sol_list[i].threads[j].value];
                safe_line(ex(sol_list[i].x + offsetX), ey(sol_list[i].y + offsetY),
                          ex(buf.x + offsetX), ey(buf.y + offsetY));

                /* unsafe thread */
                if (show_danger_hyperthreads && obj_size) {
                    for (o = 0; o < obj_size; o++) {
                        if (core_objects_sphere_line_intersect(
                                sol_list[i].x, sol_list[i].y, sol_list[i].z,
                                buf.x, buf.y, buf.z,
                                obj_list[o].x, obj_list[o].y,
                                obj_list[o].z, obj_list[o].r)) {
                            setcolor(4);
                            setlinestyle(0, 0, 1);
                            safe_line(ex(sol_list[i].x + offsetX), ey(sol_list[i].y + offsetY),
                                      ex(buf.x + offsetX), ey(buf.y + offsetY));
                            setlinestyle(1, 0, 1);
                            break;
                        }
                    }
                }
            }
        }

        /* System label */
        if (((xmax <= MAX_VALUE / 10) || i == gs.current_system ) &&
            ex(sol_list[i].x + offsetX) < (map_wnd.width - 80) &&
            ey(sol_list[i].y + offsetY) < (map_wnd.height - 15)) {
            

            if (sol_list[i].is_shipyard){
                setcolor(3);
                sprintf(c, "SA.%d(%d) [S][F]", i, sol_list[i].threadSize);
            } else if (sol_list[i].is_gas_station && game_is_visited(&gs, i)){
                setcolor(1);
                sprintf(c, "SA.%d(%d) [F]", i, sol_list[i].threadSize);
            } else if (game_is_visited(&gs, i)){
                setcolor(15);
                sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
            } else {
                setcolor(8);
                sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
            }

            settextstyle(SMALL_FONT, HORIZ_DIR, 4);
            safe_outtextxy(ex(sol_list[i].x + offsetX),
            ey(sol_list[i].y + offsetY) + 5, c);
        }

        /** SECTOR NAMES **/
            if (sol_list[i].is_shipyard && sol_list[i].is_gas_station && render_bounds){
                    setcolor(data_factions_colors[sol_list[i].faction]);
                    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
                    

                    /* Manual politic map aligment*/
                    /* "Dhat", "Medinat", "Ghabkar", "Buraq", "Ben Vara", "Killoch Vairan", "Cuchulainn", "Danter", "Coalsack" */
                    switch(sol_list[i].sector){
                        case 0:
                            /* Dhat */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) + 30,
                                ey(sol_list[i].y + offsetY) + 5 - 30, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 1:
                            /* Medinat */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) - 20,
                                ey(sol_list[i].y + offsetY) + 5 - 24, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 2:
                            /* Ghabkar */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) - 60,
                                ey(sol_list[i].y + offsetY) + 5 - 20, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 3:
                            /* Buraq */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) + 27,
                                ey(sol_list[i].y + offsetY) + 5 - 50, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 4:
                            /* Ben Vara */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX),
                                ey(sol_list[i].y + offsetY)  + 5, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 5:
                            /* Killoch Vairan */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX),
                                ey(sol_list[i].y + offsetY) + 5 + 25, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 6:
                            /* Cuchulainn */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) - 115,
                                ey(sol_list[i].y + offsetY) + 5 + 40, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 7:
                            /* Danter */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX),
                                ey(sol_list[i].y + offsetY)  + 5 - 27, 
                                data_sectors[sol_list[i].sector]);
                        break;

                        case 8:
                            /* Coalsack */
                            safe_outtextxy(
                                ex(sol_list[i].x + offsetX) - 16,
                                ey(sol_list[i].y + offsetY) + 5 - 13, 
                                data_sectors[sol_list[i].sector]);
                        break;
                    }
            }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = sol_list[wp->way[i - 1]];
        b = sol_list[wp->way[i]];
        setcolor(9);
        setlinestyle(3, 0, 1);
        safe_line(ex(a.x + offsetX), ey(a.y + offsetY),
                  ex(b.x + offsetX), ey(b.y + offsetY));
    }

    /* Dangerous path segments */
    if (show_danger_path_parts && obj_size && wp->size) {
        for (i = 1; i < wp->size; i++) {
            int a_idx = wp->way[i - 1];
            int b_idx = wp->way[i];
            for (o = 0; o < obj_size; o++) {
                if (core_objects_sphere_line_intersect(sol_list[a_idx].x, sol_list[a_idx].y, sol_list[a_idx].z,
                                        sol_list[b_idx].x, sol_list[b_idx].y, sol_list[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    setcolor(4);
                    safe_line(ex(sol_list[a_idx].x + offsetX), ey(sol_list[a_idx].y + offsetY),
                              ex(sol_list[b_idx].x + offsetX), ey(sol_list[b_idx].y + offsetY));
                    break;
                }
            }
        }
    }

    setlinestyle(0, 0, 1);
    drawObjects(1);
    draw_bounds();

    setcolor(4);
    rectangle(0, 21, map_wnd.width, map_wnd.height);
}

/* ----------------------------------------------------------------
 * draw3dwnd -- Isometric (XYZ) projection
 * ---------------------------------------------------------------- */
static void draw3dwnd(int isCoord, int isHyper, WAYPOINT* wp)
{
    int i, j, o;
    SYSTEM buf, a, b;
    int drawThreads = isHyper;
    char c[50] = "";

    xdens = (xmax - xmin) / map_wnd.width;
    ydens = (ymax - ymin) / map_wnd.height;

    if (isCoord) {
        POINT A1 = p(0.0F + offsetX, 0.0F + offsetY, 0.0F + offsetZ);

        /* X axis */
        setcolor(15); setlinestyle(0, 0, 1);
        safe_line(A1.x, A1.y, map_wnd.width, A1.y);
        setlinestyle(1, 0, 1);
        safe_line(0, A1.y, A1.x, A1.y);

        /* Z axis */
        setcolor(15); setlinestyle(0, 0, 1);
        safe_line(A1.x, A1.y, A1.x, 0);
        setlinestyle(1, 0, 1);
        safe_line(A1.x, map_wnd.height, A1.x, A1.y);

        /* Y axis (diagonal) */
        {
            float farY = (xmax - xmin) * 10.0F;
            POINT yPos, yNeg;
            setcolor(15); setlinestyle(0, 0, 1);
            yPos = p(0.0F + offsetX, farY + offsetY, 0.0F + offsetZ);
            safe_line(A1.x, A1.y, yPos.x, yPos.y);
            setlinestyle(1, 0, 1);
            yNeg = p(0.0F + offsetX, -farY + offsetY, 0.0F + offsetZ);
            safe_line(A1.x, A1.y, yNeg.x, yNeg.y);
        }
    }

    for (i = 0; i < sol_size; i++) {
        POINT A1 = p(sol_list[i].x + offsetX, sol_list[i].y + offsetY,
                            sol_list[i].z + offsetZ);
        p(sol_list[i].x, sol_list[i].y, sol_list[i].z);
        safe_putpixel(A1.x, A1.y, POINT_COLOR);

        if (sol_list[i].threadSize && drawThreads) {
            setlinestyle(1, 0, 1);
            setcolor(get_color_by_z(sol_list[i].z));
            for (j = 0; j < sol_list[i].threadSize; j++) {
                buf = sol_list[sol_list[i].threads[j].value];
                {
                    POINT A8 = p(buf.x + offsetX, buf.y + offsetY,
                                        buf.z + offsetZ);
                    if (sol_list[i].threads[j].cost < 30)
                        safe_line(A1.x, A1.y, A8.x, A8.y);

                    /* unsafe thread */
                    if (show_danger_hyperthreads && obj_size) {
                        int o;
                        for (o = 0; o < obj_size; o++) {
                            if (core_objects_sphere_line_intersect(
                                    sol_list[i].x, sol_list[i].y, sol_list[i].z,
                                    buf.x, buf.y, buf.z,
                                    obj_list[o].x, obj_list[o].y,
                                    obj_list[o].z, obj_list[o].r)) {
                                setcolor(4);
                                setlinestyle(0, 0, 1);
                                safe_line(A1.x, A1.y, A8.x, A8.y);
                                setlinestyle(1, 0, 1);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (((xmax <= MAX_VALUE / 10) || i == gs.current_system )  &&
            A1.x < (map_wnd.width - 80) && A1.y < (map_wnd.height - 20)) {
            setcolor(15);
            settextstyle(SMALL_FONT, HORIZ_DIR, 4);
            
            if (game_is_visited(&gs, i)){
                if (sol_list[i].is_shipyard){
                    setcolor(3);
                    sprintf(c, "SA.%d(%d) [S][F]", i, sol_list[i].threadSize);
                } else if (sol_list[i].is_gas_station){
                    setcolor(1);
                    sprintf(c, "SA.%d(%d) [F]", i, sol_list[i].threadSize);
                } else {
                    setcolor(15);
                    sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
                }
            } else {
                setcolor(8);
                sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
            }


            safe_outtextxy(A1.x, A1.y + 5, c);
        }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = sol_list[wp->way[i - 1]];
        b = sol_list[wp->way[i]];
        {
            POINT A8 = p(a.x + offsetX, a.y + offsetY, a.z + offsetZ);
            POINT A9 = p(b.x + offsetX, b.y + offsetY, b.z + offsetZ);
            setcolor(9);
            setlinestyle(3, 0, 1);
            safe_line(A8.x, A8.y, A9.x, A9.y);
        }
    }

    /* Dangerous path segments */
    if (show_danger_path_parts && obj_size && wp->size) {
        for (i = 1; i < wp->size; i++) {
            int a_idx = wp->way[i - 1];
            int b_idx = wp->way[i];
            for (o = 0; o < obj_size; o++) {
                if (core_objects_sphere_line_intersect(sol_list[a_idx].x, sol_list[a_idx].y, sol_list[a_idx].z,
                                        sol_list[b_idx].x, sol_list[b_idx].y, sol_list[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    POINT A8 = p(sol_list[a_idx].x + offsetX, sol_list[a_idx].y + offsetY,
                                        sol_list[a_idx].z + offsetZ);
                    POINT A9 = p(sol_list[b_idx].x + offsetX, sol_list[b_idx].y + offsetY,
                                        sol_list[b_idx].z + offsetZ);
                    setcolor(4);
                    setlinestyle(0, 0, 1);
                    safe_line(A8.x, A8.y, A9.x, A9.y);
                    break;
                }
            }
        }
    }

    drawObjects(2);
    setlinestyle(0, 0, 1);
    setcolor(4);
    rectangle(0, 21, map_wnd.width, map_wnd.height);
}

/* ----------------------------------------------------------------
 * drawyzwnd -- YZ (side) projection
 * ---------------------------------------------------------------- */
static void drawyzwnd(int isCoord, int isHyper, WAYPOINT* wp)
{
    int i, j, o;
    struct system_solar buf, a, b;
    int drawThreads = isHyper;
    char c[50] = "";

    xdens = (xmax - xmin) / map_wnd.width;
    ydens = (ymax - ymin) / map_wnd.height;

    if (isCoord) {
        int y0 = ey(-1 * (0 + offsetZ));
        int x0 = ex(0 + offsetX);

        setcolor(15);
        setlinestyle(0, 0, 1);
        safe_line(x0, y0, map_wnd.width, y0);
        safe_line(x0, y0, x0, 0);

        setlinestyle(1, 0, 1);
        safe_line(0, y0, x0, y0);
        safe_line(x0, map_wnd.height, x0, y0);
    }

    for (i = 0; i < sol_size; i++) {
        p(sol_list[i].x, sol_list[i].y, sol_list[i].z);
        safe_putpixel(ex(sol_list[i].x + offsetX),
                      ey(-1 * (sol_list[i].z + offsetZ)), POINT_COLOR);

        if (sol_list[i].threadSize && drawThreads) {
            for (j = 0; j < sol_list[i].threadSize; j++) {
                if (sol_list[i].threads[j].cost >= 15) continue;

                setcolor(get_color_by_z(sol_list[i].z));
                setlinestyle(1, 0, 1);
                buf = sol_list[sol_list[i].threads[j].value];
                safe_line(ex(sol_list[i].x + offsetX),
                          ey(-1 * (sol_list[i].z + offsetZ)),
                          ex(buf.x + offsetX),
                          ey(-1 * (buf.z + offsetZ)));

                /* unsafe thread */
                if (show_danger_hyperthreads && obj_size) {
                    int o;
                    for (o = 0; o < obj_size; o++) {
                        if (core_objects_sphere_line_intersect(
                                sol_list[i].x, sol_list[i].y, sol_list[i].z,
                                buf.x, buf.y, buf.z,
                                obj_list[o].x, obj_list[o].y,
                                obj_list[o].z, obj_list[o].r)) {
                            setcolor(4);
                            setlinestyle(0, 0, 1);
                            safe_line(ex(sol_list[i].x + offsetX),
                                      ey(-1 * (sol_list[i].z + offsetZ)),
                                      ex(buf.x + offsetX),
                                      ey(-1 * (buf.z + offsetZ)));
                            setlinestyle(1, 0, 1);
                            break;
                        }
                    }
                }
            }
        }

        if (((xmax <= MAX_VALUE / 10) || i == gs.current_system ) &&
            ex(sol_list[i].x + offsetX) < (map_wnd.width - 80) &&
            ey(sol_list[i].y + offsetY) < (map_wnd.height - 100)) {
            
            if (game_is_visited(&gs, i)){
                if (sol_list[i].is_shipyard){
                    setcolor(3);
                    sprintf(c, "SA.%d(%d) [S][F]", i, sol_list[i].threadSize);
                } else if (sol_list[i].is_gas_station){
                    setcolor(1);
                    sprintf(c, "SA.%d(%d) [F]", i, sol_list[i].threadSize);
                } else {
                    setcolor(15);
                    sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
                }
            } else {
                setcolor(8);
                sprintf(c, "SA.%d(%d)", i, sol_list[i].threadSize);
            }

            settextstyle(SMALL_FONT, VERT_DIR, 4);
            safe_outtextxy(ex(sol_list[i].x + offsetX),
                           ey(-1 * (sol_list[i].z + offsetZ)) + 5, c);
        }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = sol_list[wp->way[i - 1]];
        b = sol_list[wp->way[i]];
        setcolor(9);
        setlinestyle(3, 0, 1);
        safe_line(ex(a.x + offsetX), ey(-1 * (a.z + offsetZ)),
                  ex(b.x + offsetX), ey(-1 * (b.z + offsetZ)));
    }

    /* Dangerous path segments */
    if (show_danger_path_parts && obj_size && wp->size) {
        for (i = 1; i < wp->size; i++) {
            int a_idx = wp->way[i - 1];
            int b_idx = wp->way[i];
            for (o = 0; o < obj_size; o++) {
                if (core_objects_sphere_line_intersect(sol_list[a_idx].x, sol_list[a_idx].y, sol_list[a_idx].z,
                                        sol_list[b_idx].x, sol_list[b_idx].y, sol_list[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    setcolor(4);
                    safe_line(ex(sol_list[a_idx].x + offsetX),
                              ey(-1 * (sol_list[a_idx].z + offsetZ)),
                              ex(sol_list[b_idx].x + offsetX),
                              ey(-1 * (sol_list[b_idx].z + offsetZ)));
                    break;
                }
            }
        }
    }

    setlinestyle(0, 0, 1);
    drawObjects(3);
    setcolor(4);
    rectangle(0, 21, map_wnd.width, map_wnd.height);
}

/* ----------------------------------------------------------------
 * Status lines
 * ---------------------------------------------------------------- */
void gui_map_top_status_line()
{
    WND status_line;
    char buf[100];
    char *keys[3];
    char *items[3];

    sprintf(buf, "SA.%d (%d) | %s: %ld$$ | %s: %d%% | %s: %d/%d",
        gs.current_system,
        sol_list[gs.current_system].threadSize,
        LC_GUI_STATUS_BALANCE, gs.balance,
        LC_GUI_STATUS_FUEL, gs.fuel,
        LC_GUI_STATUS_CARGO, gs.current_cargo, gs.tonnage);

    keys[0] = "TAB";
    items[0] = LC_GUI_STATUS_MODE;
    keys[1] = "INFO";
    items[1] = buf;
    keys[2] = NULL;
    items[2] = NULL;

    status_line.x = 0;
    status_line.y = 0;
    status_line.width = STATUSBAR_WIDTH;
    status_line.height = STATUSBAR_HEIGHT;
    status_line.header = NULL;

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    gui_draw_status_line(&status_line, keys, items);
}

void gui_map_bottom_status_line()
{
    WND status_line;
    char *keys[] = { "F1", "F2", "F3/F4", "F5", "F6", "F7", NULL };
    char *items[] = {
        LC_MAP_STATUS_VIEW,
        LC_MAP_STATUS_AXIS,
        LC_MAP_STATUS_ZOOM,
        LC_MAP_STATUS_GOTO,
        LC_MAP_STATUS_THREADS,
        LC_MAP_STATUS_RUN,
        NULL
    };

    status_line.x = 0;
    status_line.y = STATUSBAR_BOTTOM_Y;
    status_line.width = STATUSBAR_WIDTH;
    status_line.height = STATUSBAR_HEIGHT;
    status_line.header = NULL;

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    gui_draw_status_line(&status_line, keys, items);
}

void gui_map_wnd_clear()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 21, map_wnd.width - 1, map_wnd.height - 1);
}


/* ----------------------------------------------------------------
 * Public: gui_map_wnd_draw() -- main frame dispatcher
 * ---------------------------------------------------------------- */
void gui_map_wnd_draw()
{
    /* Shrink main view when path panel is open */

    if (wp.size)
        map_wnd.width = 470;
    else
        map_wnd.width = 639;

    map_bounds.x2 = map_wnd.width;

    gui_map_wnd_clear();

    if (mode == 1) draw2dwnd(is_coord, is_hyper, &wp);
    if (mode == 2) draw3dwnd(is_coord, is_hyper, &wp);
    if (mode == 3) drawyzwnd(is_coord, is_hyper, &wp);

    gui_ad_hypersoft();
    gui_memory_status();
}

