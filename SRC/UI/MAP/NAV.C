/* nav.c -- navigation controller
 *
 * All functions here manipulate the viewport globals (offsetX/Y/Z,
 * xmin/xmax/ymin/ymax/zmin/zmax) that are defined in mapwnd.c.
 *
 * Public API declared in nav.h.
 */

#include <stdio.h>

#include "data\structs.h"

#include "ui\gui.h"
#include "ui\map\nav.h"

/* ----------------------------------------------------------------
 * Extern viewport globals (defined in mapwnd.c)
 * ---------------------------------------------------------------- */
extern int  MAP_WND_WIDTH, MAP_WND_HEIGHT;
extern int  offsetX, offsetY, offsetZ;
extern float xmin, xmax, ymin, ymax, zmin, zmax;
extern float xdens, ydens;

/* ----------------------------------------------------------------
 * Scale
 * ---------------------------------------------------------------- */
#define MAX_VALUE 1400
#define MIN_VALUE -1400

void gui_map_nav_scale_minus()
{
    if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3) {
        xmax = xmax + MAX_VALUE / 10;
        ymax = ymax + MAX_VALUE / 10;
        zmax = zmax + MAX_VALUE / 10;

        xmin = xmin - MAX_VALUE / 10;
        ymin = ymin - MAX_VALUE / 10;
        zmin = zmin - MAX_VALUE / 10;
    } else {
        gui_warning_wnd("ERROR", "Minimal scale rate reached!");
        getch();
    }
}

void gui_map_nav_scale_plus()
{
    if (xmax > MAX_VALUE / 10 && xmin < MIN_VALUE / 10) {
        xmax = xmax - MAX_VALUE / 10;
        ymax = ymax - MAX_VALUE / 10;
        zmax = zmax - MAX_VALUE / 10;

        xmin = xmin + MAX_VALUE / 10;
        ymin = ymin + MAX_VALUE / 10;
        zmin = zmin + MAX_VALUE / 10;
    } else {
        gui_warning_wnd("ERROR", "Maximal scale rate reached!");
        getch();
    }
}

/* ----------------------------------------------------------------
 * Offset
 * ---------------------------------------------------------------- */
void gui_map_nav_offset_x_plus()  { offsetX += xmax / 10; }
void gui_map_nav_offset_x_minus() { offsetX -= xmax / 10; }
void gui_map_nav_offset_y_plus()  { offsetY += ymax / 10; }
void gui_map_nav_offset_y_minus() { offsetY -= ymax / 10; }
void gui_map_nav_offset_z_plus()  { offsetZ += zmax / 10; }
void gui_map_nav_offset_z_minus() { offsetZ -= zmax / 10; }

/* ----------------------------------------------------------------
 * moveScreenTo -- centre view on a system
 * ---------------------------------------------------------------- */
void gui_map_nav_move_screen_to(struct system_solar* solar, int value)
{
    offsetX = -1 * solar[value].x;
    offsetY = -1 * solar[value].y;
    offsetZ = -1 * solar[value].z;

    xmax = MAX_VALUE / 10;
    ymax = MAX_VALUE / 10;
    zmax = MAX_VALUE / 10;

    xmin = MIN_VALUE / 10;
    ymin = MIN_VALUE / 10;
    zmin = MIN_VALUE / 10;
}

/* ----------------------------------------------------------------
 * gotoSystem -- prompt for system number, then jump
 * ---------------------------------------------------------------- */
void gui_map_nav_goto_system(int sol_size, struct system_solar* solar)
{
    char* input;
    int value;
    int error = 0;

    input = gui_input_wnd("GSCARD", "Insert system COORD", NULL);
    value = atoi(input);

    if (value != 0) {
        if (value > sol_size || value < 1) {
            error = 1;
        } else {
            gui_map_nav_move_screen_to(solar, value);
        }
    } else {
        error = 1;
    }

    if (error) {
        gui_warning_wnd("ERROR", "Wrong value!");
        getch();
    }
}
