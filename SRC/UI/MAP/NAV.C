/* nav.c -- navigation controller
 *
 * All functions here manipulate the viewport globals (offsetX/Y/Z,
 * xmin/xmax/ymin/ymax/zmin/zmax) that are defined in mapwnd.c.
 *
 * Public API declared in nav.h.
 */

#include <alloc.h>
#include <stdio.h>

#include "data\structs.h"

#include "ui\gui.h"
#include "ui\map\nav.h"

/* ----------------------------------------------------------------
 * Extern viewport globals (defined in mapwnd.c)
 * ---------------------------------------------------------------- */
extern int  WND_WIDTH, WND_HEIGHT;
extern int  offsetX, offsetY, offsetZ;
extern float xmin, xmax, ymin, ymax, zmin, zmax;
extern float xdens, ydens;

/* ----------------------------------------------------------------
 * Scale
 * ---------------------------------------------------------------- */
#define MAX_VALUE 1400
#define MIN_VALUE -1400

void scaleMinus()
{
    if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3) {
        xmax = xmax + MAX_VALUE / 10;
        ymax = ymax + MAX_VALUE / 10;
        zmax = zmax + MAX_VALUE / 10;

        xmin = xmin - MAX_VALUE / 10;
        ymin = ymin - MAX_VALUE / 10;
        zmin = zmin - MAX_VALUE / 10;
    } else {
        warningWnd("ERROR", "Minimal scale rate reached!");
        getch();
    }
}

void scalePlus()
{
    if (xmax > MAX_VALUE / 10 && xmin < MIN_VALUE / 10) {
        xmax = xmax - MAX_VALUE / 10;
        ymax = ymax - MAX_VALUE / 10;
        zmax = zmax - MAX_VALUE / 10;

        xmin = xmin + MAX_VALUE / 10;
        ymin = ymin + MAX_VALUE / 10;
        zmin = zmin + MAX_VALUE / 10;
    } else {
        warningWnd("ERROR", "Maximal scale rate reached!");
        getch();
    }
}

/* ----------------------------------------------------------------
 * Offset
 * ---------------------------------------------------------------- */
void offsetXplus()  { offsetX += xmax / 10; }
void offsetXminus() { offsetX -= xmax / 10; }
void offsetYplus()  { offsetY += ymax / 10; }
void offsetYminus() { offsetY -= ymax / 10; }
void offsetZplus()  { offsetZ += zmax / 10; }
void offsetZminus() { offsetZ -= zmax / 10; }

/* ----------------------------------------------------------------
 * moveScreenTo -- centre view on a system
 * ---------------------------------------------------------------- */
void moveScreenTo(struct system_solar* solar, int value)
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
void gotoSystem(int ptrSize, struct system_solar* solar)
{
    char* input;
    int value;
    int error = 0;

    input = questionWnd("GSCARD", "Insert system COORD", NULL);
    value = atoi(input);

    if (value != 0) {
        if (value > ptrSize || value < 1) {
            error = 1;
        } else {
            moveScreenTo(solar, value);
        }
    } else {
        error = 1;
    }

    if (error) {
        warningWnd("ERROR", "Wrong value!");
        getch();
    }
}
