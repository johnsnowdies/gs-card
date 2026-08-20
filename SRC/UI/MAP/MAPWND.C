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

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\ad.h"
#include "ui\map\mapwnd.h"
#include "ui\map\pathwnd.h"

/* ----------------------------------------------------------------
 * Screen / viewport layout -- defined here, declared extern in gui.h
 * ---------------------------------------------------------------- */
int MAP_WND_WIDTH  = 639;   /* may shrink to 470 when path panel is open */
int MAP_WND_HEIGHT = 460;

/* Viewport bounds for clipping (map window only) */
static struct map_wnd {
    int x1, y1, x2, y2;
} map_wnd = { 0, 21, 639, 460 };

/* Projection bounds */
#define MAX_VALUE  1400
#define MIN_VALUE -1400

float xmin = MIN_VALUE, xmax = MAX_VALUE;
float ymin = MIN_VALUE, ymax = MAX_VALUE;
float zmin = MIN_VALUE, zmax = MAX_VALUE;

int  offsetX = 0, offsetY = 0, offsetZ = 0;
float xdens, ydens;

/* Colour computed as a side-effect of p() */
static int POINT_COLOR = 0;

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct object* obj_list;
extern int      obj_size;
extern int      render_danger_objects;
extern int      show_danger_hyperthreads;
extern int      show_danger_path_parts;
extern int      dirty_path;
extern int      dirty_topbar;
extern int      dirty_bottombar;

extern char* sectors[9];

/* ----------------------------------------------------------------
 * Clipping helpers (safe_*) -- clip against map_wnd
 * ---------------------------------------------------------------- */
static void safe_putpixel(int x, int y, int c)
{
    if (x >= map_wnd.x1 && x <= map_wnd.x2 &&
        y >= map_wnd.y1 && y <= map_wnd.y2)
        putpixel(x, y, c);
}

static void safe_line(int x1, int y1, int x2, int y2)
{
    int accept = 0, done = 0;
    int code1, code2, code;
    int *xp, *yp;
    float dy, dx;

    /* Quick reject */
    if (x1 < map_wnd.x1 && x2 < map_wnd.x1) return;
    if (x1 > map_wnd.x2 && x2 > map_wnd.x2) return;
    if (y1 < map_wnd.y1 && y2 < map_wnd.y1) return;
    if (y1 > map_wnd.y2 && y2 > map_wnd.y2) return;

    /* Cohen-Sutherland */
    while (!done) {
        code1 = 0; code2 = 0;
        if (y1 < map_wnd.y1) code1 |= 1;
        if (y1 > map_wnd.y2) code1 |= 2;
        if (y2 < map_wnd.y1) code2 |= 1;
        if (y2 > map_wnd.y2) code2 |= 2;
        if (x1 < map_wnd.x1) code1 |= 4;
        if (x1 > map_wnd.x2) code1 |= 8;
        if (x2 < map_wnd.x1) code2 |= 4;
        if (x2 > map_wnd.x2) code2 |= 8;

        if (code1 == 0 && code2 == 0) { accept = 1; done = 1; }
        else if (code1 & code2)        { done = 1; }
        else {
            code = code1 ? code1 : code2;
            xp = (code == code1) ? &x1 : &x2;
            yp = (code == code1) ? &y1 : &y2;
            dy = (float)(y2 - y1);
            dx = (float)(x2 - x1);

            if      (code & 1) { *yp = map_wnd.y1; if (dy) *xp = x1 + dx * (map_wnd.y1 - y1) / dy; }
            else if (code & 2) { *yp = map_wnd.y2; if (dy) *xp = x1 + dx * (map_wnd.y2 - y1) / dy; }
            else if (code & 4) { *xp = map_wnd.x1; if (dx) *yp = y1 + dy * (map_wnd.x1 - x1) / dx; }
            else if (code & 8) { *xp = map_wnd.x2; if (dx) *yp = y1 + dy * (map_wnd.x2 - x1) / dx; }
        }
    }
    if (accept) line(x1, y1, x2, y2);
}

static void safe_outtextxy(int x, int y, char* s)
{
    if (x >= map_wnd.x1 && x <= map_wnd.x2 &&
        y >= map_wnd.y1 && y <= map_wnd.y2 - 60)
        outtextxy(x, y, s);
}

static void safe_circle(int x, int y, int r)
{
    if (x - r >= map_wnd.x1 && x + r <= map_wnd.x2 &&
        y - r >= map_wnd.y1 && y + r <= map_wnd.y2)
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

static struct point p(float x, float y, float z)
{
    float b = 0.8660254F * y;
    float a = y / 2.0F;
    struct point res;

    res.x = (int)(MAP_WND_WIDTH  / 2 + (x - a) / xdens);
    res.y = (int)(MAP_WND_HEIGHT / 2 + (b / ydens) - (z / ydens));

    /* Depth-colouring */
    if      (z <= -200) POINT_COLOR = 1;
    else if (z <= -150) POINT_COLOR = 5;
    else if (z <= -100) POINT_COLOR = 3;
    else if (z <= -50)  POINT_COLOR = 9;
    else if (z < 0)     POINT_COLOR = 7;
    else if (z == 0)    POINT_COLOR = 7;
    else if (z < 50)    POINT_COLOR = 7;
    else if (z < 100)   POINT_COLOR = 8;
    else if (z < 150)   POINT_COLOR = 14;
    else if (z < 200)   POINT_COLOR = 12;
    else                POINT_COLOR = 4;

    return res;
}

/* ----------------------------------------------------------------
 * drawObjects -- gas clouds, black holes, nebulae
 * ---------------------------------------------------------------- */
static void drawObjects(int mode)
{
    int i, color, inner;

    if (!obj_size || !obj_list || !render_danger_objects) return;

    for (i = 0; i < obj_size; i++) {
        int cx, cy, rx, ry;
        int r = obj_list[i].r;

        switch (obj_list[i].type) {
            case OBJ_GASCLOUD:  color = 5;  inner = 13; break;
            case OBJ_BLACKHOLE: color = 4;  inner = 0;  break;
            case OBJ_NEBULA:    color = 9;  inner = 11; break;
            default:            color = 5;  inner = 13; break;
        }

        if (mode == 1) {
            cx = ex(obj_list[i].x + offsetX);
            cy = ey(obj_list[i].y + offsetY);
        } else if (mode == 2) {
            struct point c = p(obj_list[i].x + offsetX,
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

/* ----------------------------------------------------------------
 * draw2dwnd -- XY projection (top-down)
 * ---------------------------------------------------------------- */
static void draw2dwnd(int sol_size, struct system_solar* solar,
                      int isCoord, int isHyper, struct waypoint* wp)
{
    int i, j, o;
    struct system_solar buf, a, b;
    int drawThreads = isHyper;

    xdens = (xmax - xmin) / MAP_WND_WIDTH;
    ydens = (ymax - ymin) / MAP_WND_HEIGHT;

    if (isCoord) {
        int x0 = ex(0 + offsetX);
        int y0 = ey(0 + offsetY);

        setcolor(15);
        setlinestyle(0, 0, 1);
        safe_line(x0, y0, MAP_WND_WIDTH, y0);
        safe_line(x0, y0, x0, 0);

        setlinestyle(1, 0, 1);
        safe_line(0, y0, x0, y0);
        safe_line(x0, MAP_WND_HEIGHT, x0, y0);
    }

    for (i = 0; i < sol_size; i++) {
        p(solar[i].x, solar[i].y, solar[i].z);
        safe_putpixel(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                      POINT_COLOR);

        if (solar[i].threadSize && drawThreads) {
            for (j = 0; j < solar[i].threadSize; j++) {
                if (solar[i].threads[j].cost >= 15) continue;

                setcolor(15);
                setlinestyle(1, 0, 1);
                buf = solar[solar[i].threads[j].value];
                safe_line(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                          ex(buf.x + offsetX), ey(buf.y + offsetY));

                /* unsafe thread */
                if (show_danger_hyperthreads && obj_size) {
                    for (o = 0; o < obj_size; o++) {
                        if (core_objects_sphere_line_intersect(
                                solar[i].x, solar[i].y, solar[i].z,
                                buf.x, buf.y, buf.z,
                                obj_list[o].x, obj_list[o].y,
                                obj_list[o].z, obj_list[o].r)) {
                            setcolor(4);
                            setlinestyle(0, 0, 1);
                            safe_line(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                                      ex(buf.x + offsetX), ey(buf.y + offsetY));
                            setlinestyle(1, 0, 1);
                            break;
                        }
                    }
                }
            }
        }

        /* System label */
        if (xmax <= MAX_VALUE / 10 &&
            ex(solar[i].x + offsetX) < (MAP_WND_WIDTH - 80) &&
            ey(solar[i].y + offsetY) < (MAP_WND_HEIGHT - 15)) {
            char c[50] = "";
            sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
            setcolor(15);
            settextstyle(SMALL_FONT, HORIZ_DIR, 4);
            safe_outtextxy(ex(solar[i].x + offsetX),
                           ey(solar[i].y + offsetY) + 5, c);
        }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = solar[wp->way[i - 1]];
        b = solar[wp->way[i]];
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
                if (core_objects_sphere_line_intersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                        solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    setcolor(4);
                    safe_line(ex(solar[a_idx].x + offsetX), ey(solar[a_idx].y + offsetY),
                              ex(solar[b_idx].x + offsetX), ey(solar[b_idx].y + offsetY));
                    break;
                }
            }
        }
    }

    setlinestyle(0, 0, 1);
    drawObjects(1);

    setcolor(4);
    rectangle(0, 21, MAP_WND_WIDTH, MAP_WND_HEIGHT);
    gui_ad_hypersoft();
}

/* ----------------------------------------------------------------
 * draw3dwnd -- Isometric (XYZ) projection
 * ---------------------------------------------------------------- */
static void draw3dwnd(int sol_size, struct system_solar* solar,
                      int isCoord, int isHyper, struct waypoint* wp)
{
    int i, j, o;
    struct system_solar buf, a, b;
    int drawThreads = isHyper;

    xdens = (xmax - xmin) / MAP_WND_WIDTH;
    ydens = (ymax - ymin) / MAP_WND_HEIGHT;

    if (isCoord) {
        struct point A1 = p(0.0F + offsetX, 0.0F + offsetY, 0.0F + offsetZ);

        /* X axis */
        setcolor(15); setlinestyle(0, 0, 1);
        safe_line(A1.x, A1.y, MAP_WND_WIDTH, A1.y);
        setlinestyle(1, 0, 1);
        safe_line(0, A1.y, A1.x, A1.y);

        /* Z axis */
        setcolor(15); setlinestyle(0, 0, 1);
        safe_line(A1.x, A1.y, A1.x, 0);
        setlinestyle(1, 0, 1);
        safe_line(A1.x, MAP_WND_HEIGHT, A1.x, A1.y);

        /* Y axis (diagonal) */
        {
            float farY = (xmax - xmin) * 10.0F;
            struct point yPos, yNeg;
            setcolor(15); setlinestyle(0, 0, 1);
            yPos = p(0.0F + offsetX, farY + offsetY, 0.0F + offsetZ);
            safe_line(A1.x, A1.y, yPos.x, yPos.y);
            setlinestyle(1, 0, 1);
            yNeg = p(0.0F + offsetX, -farY + offsetY, 0.0F + offsetZ);
            safe_line(A1.x, A1.y, yNeg.x, yNeg.y);
        }
    }

    for (i = 0; i < sol_size; i++) {
        struct point A1 = p(solar[i].x + offsetX, solar[i].y + offsetY,
                            solar[i].z + offsetZ);
        p(solar[i].x, solar[i].y, solar[i].z);
        safe_putpixel(A1.x, A1.y, POINT_COLOR);

        if (solar[i].threadSize && drawThreads) {
            setlinestyle(1, 0, 1);
            for (j = 0; j < solar[i].threadSize; j++) {
                buf = solar[solar[i].threads[j].value];
                {
                    struct point A8 = p(buf.x + offsetX, buf.y + offsetY,
                                        buf.z + offsetZ);
                    if (solar[i].threads[j].cost < 30)
                        safe_line(A1.x, A1.y, A8.x, A8.y);

                    /* unsafe thread */
                    if (show_danger_hyperthreads && obj_size) {
                        int o;
                        for (o = 0; o < obj_size; o++) {
                            if (core_objects_sphere_line_intersect(
                                    solar[i].x, solar[i].y, solar[i].z,
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

        if (xmax <= MAX_VALUE / 10 &&
            A1.x < (MAP_WND_WIDTH - 80) && A1.y < (MAP_WND_HEIGHT - 20)) {
            char c[50] = "";
            setcolor(15);
            settextstyle(SMALL_FONT, HORIZ_DIR, 4);
            sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
            safe_outtextxy(A1.x, A1.y + 5, c);
        }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = solar[wp->way[i - 1]];
        b = solar[wp->way[i]];
        {
            struct point A8 = p(a.x + offsetX, a.y + offsetY, a.z + offsetZ);
            struct point A9 = p(b.x + offsetX, b.y + offsetY, b.z + offsetZ);
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
                if (core_objects_sphere_line_intersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                        solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    struct point A8 = p(solar[a_idx].x + offsetX, solar[a_idx].y + offsetY,
                                        solar[a_idx].z + offsetZ);
                    struct point A9 = p(solar[b_idx].x + offsetX, solar[b_idx].y + offsetY,
                                        solar[b_idx].z + offsetZ);
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
    rectangle(0, 21, MAP_WND_WIDTH, MAP_WND_HEIGHT);
    gui_ad_hypersoft();
}

/* ----------------------------------------------------------------
 * drawyzwnd -- YZ (side) projection
 * ---------------------------------------------------------------- */
static void drawyzwnd(int sol_size, struct system_solar* solar,
                      int isCoord, int isHyper, struct waypoint* wp)
{
    int i, j, o;
    struct system_solar buf, a, b;
    int drawThreads = isHyper;

    xdens = (xmax - xmin) / MAP_WND_WIDTH;
    ydens = (ymax - ymin) / MAP_WND_HEIGHT;

    if (isCoord) {
        int y0 = ey(-1 * (0 + offsetZ));
        int x0 = ex(0 + offsetX);

        setcolor(15);
        setlinestyle(0, 0, 1);
        safe_line(x0, y0, MAP_WND_WIDTH, y0);
        safe_line(x0, y0, x0, 0);

        setlinestyle(1, 0, 1);
        safe_line(0, y0, x0, y0);
        safe_line(x0, MAP_WND_HEIGHT, x0, y0);
    }

    for (i = 0; i < sol_size; i++) {
        p(solar[i].x, solar[i].y, solar[i].z);
        safe_putpixel(ex(solar[i].x + offsetX),
                      ey(-1 * (solar[i].z + offsetZ)), POINT_COLOR);

        if (solar[i].threadSize && drawThreads) {
            for (j = 0; j < solar[i].threadSize; j++) {
                if (solar[i].threads[j].cost >= 15) continue;

                setcolor(15);
                setlinestyle(1, 0, 1);
                buf = solar[solar[i].threads[j].value];
                safe_line(ex(solar[i].x + offsetX),
                          ey(-1 * (solar[i].z + offsetZ)),
                          ex(buf.x + offsetX),
                          ey(-1 * (buf.z + offsetZ)));

                /* unsafe thread */
                if (show_danger_hyperthreads && obj_size) {
                    int o;
                    for (o = 0; o < obj_size; o++) {
                        if (core_objects_sphere_line_intersect(
                                solar[i].x, solar[i].y, solar[i].z,
                                buf.x, buf.y, buf.z,
                                obj_list[o].x, obj_list[o].y,
                                obj_list[o].z, obj_list[o].r)) {
                            setcolor(4);
                            setlinestyle(0, 0, 1);
                            safe_line(ex(solar[i].x + offsetX),
                                      ey(-1 * (solar[i].z + offsetZ)),
                                      ex(buf.x + offsetX),
                                      ey(-1 * (buf.z + offsetZ)));
                            setlinestyle(1, 0, 1);
                            break;
                        }
                    }
                }
            }
        }

        if (xmax <= MAX_VALUE / 10 &&
            ex(solar[i].x + offsetX) < (MAP_WND_WIDTH - 80) &&
            ey(solar[i].y + offsetY) < (MAP_WND_HEIGHT - 100)) {
            char c[50] = "";
            sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
            setcolor(15);
            settextstyle(SMALL_FONT, VERT_DIR, 4);
            safe_outtextxy(ex(solar[i].x + offsetX),
                           ey(-1 * (solar[i].z + offsetZ)) + 5, c);
        }
    }

    /* Waypoint path */
    for (i = 1; i < wp->size; i++) {
        a = solar[wp->way[i - 1]];
        b = solar[wp->way[i]];
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
                if (core_objects_sphere_line_intersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                        solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                        obj_list[o].x, obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
                    setcolor(4);
                    safe_line(ex(solar[a_idx].x + offsetX),
                              ey(-1 * (solar[a_idx].z + offsetZ)),
                              ex(solar[b_idx].x + offsetX),
                              ey(-1 * (solar[b_idx].z + offsetZ)));
                    break;
                }
            }
        }
    }

    setlinestyle(0, 0, 1);
    drawObjects(3);
    setcolor(4);
    rectangle(0, 21, MAP_WND_WIDTH, MAP_WND_HEIGHT);
    gui_ad_hypersoft();
}

/* ----------------------------------------------------------------
 * map_bottom_status_line -- bottom shortcut bar + memory usage
 * ---------------------------------------------------------------- */
void map_bottom_status_line()
{
    unsigned int USED_MEM, FREE_MEM, TOTAL_MEM = 65535;
    int xpos = BAR_LEFT;
    char memMsg[50] = "";

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    FREE_MEM = coreleft();
    USED_MEM = TOTAL_MEM - FREE_MEM;
    sprintf(memMsg, "%u/%u", USED_MEM, TOTAL_MEM);

    setfillstyle(SOLID_FILL, BLACK);
    bar(0, STATUSBAR_Y, MAP_WND_WIDTH, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Shortcut labels */
    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F1"); xpos += 13;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-VIEW"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F2");
    setcolor(TEXT_COLOR); xpos += 15;
    outtextxy(xpos, STATUSBAR_Y + 2, "-AXIS"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F3/F4"); xpos += 40;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-ZOOM"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F5"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-GOTO"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F6"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-THREADS"); xpos += 70;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F7"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-RUN"); xpos += 40;

    /* Separator */
    xpos = 470;
    setcolor(BAR_COLOR);
    line(xpos, STATUSBAR_Y - 1, xpos, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Memory */
    setcolor(BAR_COLOR);
    outtextxy(xpos + 5, STATUSBAR_Y + 2, "MEM:");
    setcolor(TEXT_COLOR);
    outtextxy(xpos + 35, STATUSBAR_Y + 2, memMsg);
}

/* ----------------------------------------------------------------
 * Public: gui_map_wnd_draw() -- main frame dispatcher
 * ---------------------------------------------------------------- */
void gui_map_wnd_draw(int sol_size, struct system_solar* sol_list, int mode,
          int isCoord, int isHyper, struct waypoint* way, int current_point)
{
    /* Shrink main view when path panel is open */
    if (way->size)
        MAP_WND_WIDTH = 470;
    else
        MAP_WND_WIDTH = 639;

    map_wnd.x2 = MAP_WND_WIDTH;

    gui_map_wnd_clear();

    if (mode == 1) draw2dwnd(sol_size, sol_list, isCoord, isHyper, way);
    if (mode == 2) draw3dwnd(sol_size, sol_list, isCoord, isHyper, way);
    if (mode == 3) drawyzwnd(sol_size, sol_list, isCoord, isHyper, way);

    if (way->size && dirty_path) gui_map_path_wnd(way, current_point, sol_list);
    dirty_path = 0;

    if (dirty_topbar) gui_top_status_line();
    if (dirty_bottombar) map_bottom_status_line();
    dirty_topbar = 0;
    dirty_bottombar = 0;
}

/* ----------------------------------------------------------------
 * Public: gui_map_wnd_clear() -- fill map area with black
 * ---------------------------------------------------------------- */
void gui_map_wnd_clear()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 21, MAP_WND_WIDTH - 1, MAP_WND_HEIGHT - 1);
}
