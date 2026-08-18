#include <alloc.h>
#include <graphics.h>

#include "structs.h"
#include "gui.h"
#include "reader.h"
#include "keys.h"
#include "ad.h"
#include "finder.h"

/* Map window -- viewport bounds for clipping */
struct map_wnd {
  int x1, y1, x2, y2;
} map_wnd = {0, 21, 639, 460};

void safe_putpixel(int x, int y, int c) {
  if (x >= map_wnd.x1 && x <= map_wnd.x2 && y >= map_wnd.y1 && y <= map_wnd.y2)
    putpixel(x, y, c);
}

void safe_line(int x1, int y1, int x2, int y2) {
  int accept = 0, done = 0;
  int code1, code2, code;
  int *xp, *yp;
  float dy, dx;

  /* Quick reject: entirely outside */
  if (x1 < map_wnd.x1 && x2 < map_wnd.x1) return;
  if (x1 > map_wnd.x2 && x2 > map_wnd.x2) return;
  if (y1 < map_wnd.y1 && y2 < map_wnd.y1) return;
  if (y1 > map_wnd.y2 && y2 > map_wnd.y2) return;

  /* Cohen-Sutherland clipping against map_wnd */

  while (!done) {
    code1 = 0;
    code2 = 0;
    if (y1 < map_wnd.y1) code1 |= 1;
    if (y1 > map_wnd.y2) code1 |= 2;
    if (y2 < map_wnd.y1) code2 |= 1;
    if (y2 > map_wnd.y2) code2 |= 2;
    if (x1 < map_wnd.x1) code1 |= 4;
    if (x1 > map_wnd.x2) code1 |= 8;
    if (x2 < map_wnd.x1) code2 |= 4;
    if (x2 > map_wnd.x2) code2 |= 8;

    if (code1 == 0 && code2 == 0) {
      accept = 1;
      done = 1;
    } else if (code1 & code2) {
      done = 1;
    } else {
      code = code1 ? code1 : code2;
      xp = (code == code1) ? &x1 : &x2;
      yp = (code == code1) ? &y1 : &y2;
      dy = (float)(y2 - y1);
      dx = (float)(x2 - x1);

      if (code & 1) {
        *yp = map_wnd.y1;
        if (dy) *xp = x1 + dx * (map_wnd.y1 - y1) / dy;
      } else if (code & 2) {
        *yp = map_wnd.y2;
        if (dy) *xp = x1 + dx * (map_wnd.y2 - y1) / dy;
      } else if (code & 4) {
        *xp = map_wnd.x1;
        if (dx) *yp = y1 + dy * (map_wnd.x1 - x1) / dx;
      } else if (code & 8) {
        *xp = map_wnd.x2;
        if (dx) *yp = y1 + dy * (map_wnd.x2 - x1) / dx;
      }
    }
  }

  if (accept) line(x1, y1, x2, y2);
}

void safe_outtextxy(int x, int y, char* s) {
  if (x >= map_wnd.x1 && x <= map_wnd.x2 && y >= map_wnd.y1 &&
      y <= map_wnd.y2 - 60)
    outtextxy(x, y, s);
}

void safe_circle(int x, int y, int r) {
  /* Only draw if entire circle is within map_wnd */
  if (x - r >= map_wnd.x1 && x + r <= map_wnd.x2 && y - r >= map_wnd.y1 &&
      y + r <= map_wnd.y2)
    circle(x, y, r);
}

/* objects */
#include "objects.h"
extern OBJECT* objList;
extern int objSize;

/* game flags */
extern int render_danger_objects;
extern int show_danger_hyperthreads;
extern int show_danger_path_parts;
extern int dirty_path;
extern int dirty_topbar;
extern int dirty_bottombar;

/* game state */
extern GAMESTATE gs;
extern SYSTEM* ptrList;

#define MAX_INPUT_LEN 30
#define MAX_VALUE 1400
#define MIN_VALUE -1400

int WND_WIDTH = 639;
int WND_HEIGHT = 460;

int POINT_COLOR = 0;

float xmin = MIN_VALUE, xmax = MAX_VALUE;
float ymin = MIN_VALUE, ymax = MAX_VALUE;
float zmin = MIN_VALUE, zmax = MAX_VALUE;

int offsetX = 0, offsetY = 0, offsetZ = 0;
int pathListFlag = 0;
float xdens, ydens;

void pathwnd(WAYPOINT* wp);

int ex(float x) {
  int xs;

  xs = (int)((x - xmin) / xdens);

  return xs;
}

int ey(float y) {
  int ys;

  y = -1 * y;
  ys = (int)((ymax - y) / ydens);

  return ys;
}

POINT p(float x, float y, float z) {
  float b = 0.8660254 * y;
  float a = (y / 2);

  POINT res;

  res.x = (int)(WND_WIDTH / 2 + (x - a) / xdens);
  res.y = (int)(WND_HEIGHT / 2 + (b / ydens) - (z / ydens));

  if (z <= 0) POINT_COLOR = 7;

  if (z <= -50) POINT_COLOR = 9;

  if (z <= -100) POINT_COLOR = 3;

  if (z <= -150) POINT_COLOR = 5;

  if (z <= -200) POINT_COLOR = 1;

  if (z >= 0) POINT_COLOR = 7;

  if (z >= 50) POINT_COLOR = 8;

  if (z >= 100) POINT_COLOR = 14;

  if (z >= 150) POINT_COLOR = 12;

  if (z >= 200) POINT_COLOR = 4;

  return res;
}

/* drawObjects: renders gas clouds, black holes, nebulae in all 3 modes */
void drawObjects(int mode) {
  int i;
  int color, inner;

  if (!objSize || !objList || !render_danger_objects) return;

  for (i = 0; i < objSize; i++) {
    int cx, cy, rx, ry;
    int r = objList[i].r;

    switch (objList[i].type) {
      case OBJ_GASCLOUD:
        color = 5;
        inner = 13;
        break;
      case OBJ_BLACKHOLE:
        color = 4;
        inner = 0;
        break;
      case OBJ_NEBULA:
        color = 9;
        inner = 11;
        break;
      default:
        color = 5;
        inner = 13;
        break;
    }

    if (mode == 1) {
      cx = ex(objList[i].x + offsetX);
      cy = ey(objList[i].y + offsetY);
    } else if (mode == 2) {
      POINT c = p(objList[i].x + offsetX, objList[i].y + offsetY,
                  objList[i].z + offsetZ);
      cx = c.x;
      cy = c.y;
    } else {
      cx = ex(objList[i].x + offsetX);
      cy = ey(-1 * (objList[i].z + offsetZ));
    }

    rx = (int)(r / xdens);
    ry = (int)(r / ydens);
    if (rx < 2) rx = 2;
    if (ry < 2) ry = 2;

    /* dashed circle outline instead of filled ellipse */
    setlinestyle(1, 0, 1);
    setcolor(color);
    safe_circle(cx, cy, rx > ry ? rx : ry);
    setlinestyle(0, 0, 1);
  }
}

void draw3dwnd(int ptrSize, SYSTEM* solar, int isCoord, int isHyper,
               WAYPOINT* wp) {
  int i, j;
  int drawThreads;
  int o;

  POINT A1, A8, A9;

  SYSTEM buf, a, b;

  xdens = (xmax - xmin) / WND_WIDTH;
  ydens = (ymax - ymin) / WND_HEIGHT;

  drawThreads = isHyper;

  if (isCoord == 1) {
    /* Full-screen axis lines (grid), zoom-independent */
    A1 = p(0.0 + offsetX, 0.0 + offsetY, 0.0 + offsetZ);

    /* X axis -- horizontal through origin, full width */
    setcolor(15);
    safe_line(A1.x, A1.y, WND_WIDTH, A1.y);
    setlinestyle(1, 0, 1);
    safe_line(0, A1.y, A1.x, A1.y);

    /* Z axis -- vertical through origin, full height */
    setcolor(15);
    setlinestyle(0, 0, 1);
    safe_line(A1.x, A1.y, A1.x, 0);
    setlinestyle(1, 0, 1);
    safe_line(A1.x, WND_HEIGHT, A1.x, A1.y);

    /* Y axis -- diagonal, extend to screen edges via far endpoints */
    {
      float farY = (xmax - xmin) * 10.0;
      POINT yPos, yNeg;
      setcolor(15);
      setlinestyle(0, 0, 1);
      yPos = p(0.0 + offsetX, farY + offsetY, 0.0 + offsetZ);
      safe_line(A1.x, A1.y, yPos.x, yPos.y);
      setlinestyle(1, 0, 1);
      yNeg = p(0.0 + offsetX, -farY + offsetY, 0.0 + offsetZ);
      safe_line(A1.x, A1.y, yNeg.x, yNeg.y);
    }
  }

  for (i = 0; i < ptrSize; i++) {
    A1 = p(solar[i].x + offsetX, solar[i].y + offsetY, solar[i].z + offsetZ);

    p(solar[i].x, solar[i].y, solar[i].z);
    safe_putpixel(A1.x, A1.y, POINT_COLOR);

    if (solar[i].threadSize != 0 && drawThreads) {
      setlinestyle(1, 0, 1);
      for (j = 0; j < solar[i].threadSize; j++) {
        buf = solar[solar[i].threads[j].value];
        A8 = p(buf.x + offsetX, buf.y + offsetY, buf.z + offsetZ);
        if (solar[i].threads[j].cost < 30) safe_line(A1.x, A1.y, A8.x, A8.y);
        /* unsafe thread via object intersection */
        if (show_danger_hyperthreads && objSize) {
          int o;
          for (o = 0; o < objSize; o++) {
            if (sphereLineIntersect(solar[i].x, solar[i].y, solar[i].z, buf.x,
                                    buf.y, buf.z, objList[o].x, objList[o].y,
                                    objList[o].z, objList[o].r)) {
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

    if (xmax <= MAX_VALUE / 10 && (A1.x < (WND_WIDTH - 80)) &&
        (A1.y < (WND_HEIGHT - 20))) {
      char c[50] = "";
      setcolor(15);
      settextstyle(SMALL_FONT, HORIZ_DIR, 4);
      sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
      safe_outtextxy(A1.x, A1.y + 5, c);
    }
  }

  if (wp->way[0] != NULL) {
    for (i = 1; i < wp->size; i++) {
      a = solar[wp->way[i - 1]];
      b = solar[wp->way[i]];

      A8 = p(a.x + offsetX, a.y + offsetY, a.z + offsetZ);
      A9 = p(b.x + offsetX, b.y + offsetY, b.z + offsetZ);
      setcolor(9);
      setlinestyle(3, 0, 1);

      safe_line(A8.x, A8.y, A9.x, A9.y);

      setcolor(15);
    }
  }

  /* Redraw dangerous path segments in red */
  if (show_danger_path_parts && objSize && wp->size) {
    for (i = 1; i < wp->size; i++) {
      int a_idx = wp->way[i - 1];
      int b_idx = wp->way[i];
      for (o = 0; o < objSize; o++) {
        if (sphereLineIntersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                objList[o].x, objList[o].y, objList[o].z,
                                objList[o].r)) {
          A8 = p(solar[a_idx].x + offsetX, solar[a_idx].y + offsetY,
                 solar[a_idx].z + offsetZ);
          A9 = p(solar[b_idx].x + offsetX, solar[b_idx].y + offsetY,
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
  rectangle(0, 21, WND_WIDTH, WND_HEIGHT);
  adHypersoft();
}

void draw2dwnd(int ptrSize, SYSTEM* solar, int isCoord, int isHyper,
               WAYPOINT* wp) {
  int i, j;
  int drawThreads;
  int o;

  SYSTEM buf, a, b;

  xdens = (xmax - xmin) / WND_WIDTH;
  ydens = (ymax - ymin) / WND_HEIGHT;

  drawThreads = isHyper;

  if (isCoord == 1) {
    /* Full-screen axis lines, zoom-independent */
    int x0 = ex(0 + offsetX);
    int y0 = ey(0 + offsetY);

    setcolor(15);
    setlinestyle(0, 0, 1);
    /* X axis -- full width at y0 */
    safe_line(x0, y0, WND_WIDTH, y0);
    /* Y axis -- full height at x0 */
    safe_line(x0, y0, x0, 0);

    setlinestyle(1, 0, 1);
    safe_line(0, y0, x0, y0);
    safe_line(x0, WND_HEIGHT, x0, y0);
  }

  for (i = 0; i < ptrSize; i++) {
    p(solar[i].x, solar[i].y, solar[i].z);
    safe_putpixel(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                  POINT_COLOR);

    if (solar[i].threadSize != 0 && drawThreads) {
      for (j = 0; j < solar[i].threadSize; j++) {
        setcolor(15);
        setlinestyle(1, 0, 1);

        if (solar[i].threads[j].cost < 15) {
          buf = solar[solar[i].threads[j].value];

          safe_line(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                    ex(buf.x + offsetX), ey(buf.y + offsetY));
        }

        /* unsafe thread (intersects object) -- red */
        /* unsafe thread via object intersection */
        if (show_danger_hyperthreads && objSize) {
          int o;
          for (o = 0; o < objSize; o++) {
            if (sphereLineIntersect(solar[i].x, solar[i].y, solar[i].z,
                                    solar[solar[i].threads[j].value].x,
                                    solar[solar[i].threads[j].value].y,
                                    solar[solar[i].threads[j].value].z,
                                    objList[o].x, objList[o].y, objList[o].z,
                                    objList[o].r)) {
              setcolor(4);
              safe_line(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
                        ex(solar[solar[i].threads[j].value].x + offsetX),
                        ey(solar[solar[i].threads[j].value].y + offsetY));
              setcolor(15);
              break;
            }
          }
        }
      }
    }

    if (xmax <= MAX_VALUE / 10 &&
        (ex(solar[i].x + offsetX) < (WND_WIDTH - 80)) &&
        (ey(solar[i].y + offsetY) < (WND_HEIGHT - 15))) {
      char c[50] = "";

      sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
      setcolor(15);
      settextstyle(SMALL_FONT, HORIZ_DIR, 4);
      safe_outtextxy(ex(solar[i].x + offsetX), ey(solar[i].y + offsetY) + 5, c);
    }
  }

  if (wp->size != 0) {
    for (i = 1; i < wp->size; i++) {
      a = solar[wp->way[i - 1]];
      b = solar[wp->way[i]];

      setcolor(9);
      setlinestyle(3, 0, 1);

      safe_line(ex(a.x + offsetX), ey(a.y + offsetY), ex(b.x + offsetX),
                ey(b.y + offsetY));
      setcolor(15);
    }
  }
  /* Redraw dangerous path segments in red */
  if (show_danger_path_parts && objSize && wp->size) {
    for (i = 1; i < wp->size; i++) {
      int a_idx = wp->way[i - 1];
      int b_idx = wp->way[i];
      for (o = 0; o < objSize; o++) {
        if (sphereLineIntersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                objList[o].x, objList[o].y, objList[o].z,
                                objList[o].r)) {
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
  rectangle(0, 21, WND_WIDTH, WND_HEIGHT);
  adHypersoft();
}

void drawyzwnd(int ptrSize, SYSTEM* solar, int isCoord, int isHyper,
               WAYPOINT* wp) {
  int i, j;
  int drawThreads;
  int o;

  SYSTEM buf, a, b;

  xdens = (xmax - xmin) / WND_WIDTH;
  ydens = (ymax - ymin) / WND_HEIGHT;

  drawThreads = isHyper;

  if (isCoord == 1) {
    /* Full-screen axis lines, zoom-independent */
    int y0 = ey(-1 * (0 + offsetZ));
    int x0 = ex(0 + offsetX);

    setcolor(15);
    setlinestyle(0, 0, 1);
    /* Horizontal (Y) axis -- full width at Z=0 */
    safe_line(x0, y0, WND_WIDTH, y0);
    /* Vertical (Z) axis -- full height at Y=0 */
    safe_line(x0, y0, x0, 0);

    setlinestyle(1, 0, 1);
    safe_line(0, y0, x0, y0);
    safe_line(x0, WND_HEIGHT, x0, y0);
  }

  for (i = 0; i < ptrSize; i++) {
    p(solar[i].x, solar[i].y, solar[i].z);
    safe_putpixel(ex(solar[i].x + offsetX), ey(-1 * (solar[i].z + offsetZ)),
                  POINT_COLOR);

    if (solar[i].threadSize != 0 && drawThreads) {
      for (j = 0; j < solar[i].threadSize; j++) {
        setcolor(15);
        setlinestyle(1, 0, 1);

        if (solar[i].threads[j].cost < 15) {
          buf = solar[solar[i].threads[j].value];

          safe_line(ex(solar[i].x + offsetX), ey(-1 * (solar[i].z + offsetZ)),
                    ex(buf.x + offsetX), ey(-1 * (buf.z + offsetZ)));
        }

        /* unsafe thread (intersects object) -- red */
        /* unsafe thread via object intersection */
        if (show_danger_hyperthreads && objSize) {
          int o;
          for (o = 0; o < objSize; o++) {
            if (sphereLineIntersect(solar[i].x, solar[i].y, solar[i].z, buf.x,
                                    buf.y, buf.z, objList[o].x, objList[o].y,
                                    objList[o].z, objList[o].r)) {
              setcolor(4);
              safe_line(ex(solar[i].x + offsetX),
                        ey(-1 * (solar[i].z + offsetZ)), ex(buf.x + offsetX),
                        ey(-1 * (buf.z + offsetZ)));
              setcolor(15);
              break;
            }
          }
        }
      }
    }

    if (xmax <= MAX_VALUE / 10 &&
        (ex(solar[i].x + offsetX) < (WND_WIDTH - 80)) &&
        (ey(solar[i].y + offsetY) < (WND_HEIGHT - 100))) {
      char c[50] = "";

      sprintf(c, "SA.%d(%d)", i, solar[i].threadSize);
      setcolor(15);
      settextstyle(SMALL_FONT, VERT_DIR, 4);
      safe_outtextxy(ex(solar[i].x + offsetX),
                     ey(-1 * (solar[i].z + offsetZ)) + 5, c);
    }
  }

  if (wp->size != 0) {
    for (i = 1; i < wp->size; i++) {
      a = solar[wp->way[i - 1]];
      b = solar[wp->way[i]];

      setcolor(9);
      setlinestyle(3, 0, 1);
      safe_line(ex(a.x + offsetX), ey(-1 * (a.z + offsetZ)), ex(b.x + offsetX),
                ey(-1 * (b.z + offsetZ)));
      setcolor(15);
    }
  }
  /* Redraw dangerous path segments in red */
  if (show_danger_path_parts && objSize && wp->size) {
    for (i = 1; i < wp->size; i++) {
      int a_idx = wp->way[i - 1];
      int b_idx = wp->way[i];
      for (o = 0; o < objSize; o++) {
        if (sphereLineIntersect(solar[a_idx].x, solar[a_idx].y, solar[a_idx].z,
                                solar[b_idx].x, solar[b_idx].y, solar[b_idx].z,
                                objList[o].x, objList[o].y, objList[o].z,
                                objList[o].r)) {
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
  rectangle(0, 21, WND_WIDTH, WND_HEIGHT);
  adHypersoft();
}

/* topStatusLine -- top bar: system, name, balance */
void topStatusLine() {
  int xpos = 2;
  char buf[50] = "";

  setfillstyle(SOLID_FILL, BLACK);
  bar(0, 0, WND_WIDTH, 20);

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);

  /* TAB-Mode */
  setcolor(4);
  outtextxy(xpos, 2, "TAB");
  xpos += 25;
  setcolor(15);
  outtextxy(xpos, 2, "-MODE");

  /* center: current system */
  xpos = WND_WIDTH / 2 - 60;
  if (xpos < 80) xpos = 80;
  sprintf(buf, "SA.%d (%d)", gs.current_system,
          ptrList[gs.current_system].threadSize);
  setcolor(15);
  outtextxy(xpos, 2, buf);

  /* right: name + balance */
  xpos = WND_WIDTH - 160;
  if (xpos < 300) xpos = 300;
  sprintf(buf, "%s $$ %ld", gs.captain_name, gs.balance);
  setcolor(15);
  outtextxy(xpos, 2, buf);
}

void statusLine() {
  unsigned int USED_MEM;
  unsigned int FREE_MEM;
  unsigned int TOTAL_MEM = 65535;

  int xpos = 2;

  char memMsg[50] = "";

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  FREE_MEM = coreleft();
  USED_MEM = TOTAL_MEM - FREE_MEM;

  sprintf(memMsg, "%u/%u", USED_MEM, TOTAL_MEM);

  setfillstyle(SOLID_FILL, BLACK);

  bar(0, WND_HEIGHT + 1, WND_WIDTH, WND_HEIGHT + 20);

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F1");
  xpos += 13;
  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, "-VIEW");
  xpos += 45;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F2");
  setcolor(15);
  xpos += 15;
  outtextxy(xpos, WND_HEIGHT + 2, "-AXIS");
  xpos += 45;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F3/F4");
  xpos += 40;
  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, "-ZOOM");
  xpos += 45;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F5");
  xpos += 15;
  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, "-GOTO");
  xpos += 45;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F6");
  xpos += 15;

  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, "-THREADS");
  xpos += 70;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "F7");
  xpos += 15;

  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, "-RUN");
  xpos += 40;

  xpos = 470;
  setcolor(4);
  line(xpos, WND_HEIGHT, xpos, 480);
  xpos += 5;

  setcolor(4);
  outtextxy(xpos, WND_HEIGHT + 2, "MEM:");
  xpos += 30;
  setcolor(15);
  outtextxy(xpos, WND_HEIGHT + 2, memMsg);
}

void scaleMinus() {
  if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3) {
    xmax = xmax + MAX_VALUE / 10;
    ymax = ymax + MAX_VALUE / 10;
    zmax = zmax + MAX_VALUE / 10;

    ymin = ymin - MAX_VALUE / 10;
    xmin = xmin - MAX_VALUE / 10;
    zmin = zmin - MAX_VALUE / 10;
  } else {
    warningWnd("ERROR", "Minimal scale rate reached!");
    getch();
  }
}

void scalePlus() {
  if (xmax > MAX_VALUE / 10 && xmin < MIN_VALUE / 10) {
    xmax = xmax - MAX_VALUE / 10;
    ymax = ymax - MAX_VALUE / 10;
    zmax = zmax - MAX_VALUE / 10;

    ymin = ymin + MAX_VALUE / 10;
    xmin = xmin + MAX_VALUE / 10;
    zmin = zmin + MAX_VALUE / 10;
  } else {
    warningWnd("ERROR", "Maximal scale rate reached!");
    getch();
  }
}

void offsetXplus() { offsetX += xmax / 10; }
void offsetXminus() { offsetX -= xmax / 10; }
void offsetYplus() { offsetY += ymax / 10; }
void offsetYminus() { offsetY -= ymax / 10; }
void offsetZplus() { offsetZ += zmax / 10; }
void offsetZminus() { offsetZ -= zmax / 10; }

void clearWnd() {
  setfillstyle(SOLID_FILL, BLACK);
  bar(1, 21, WND_WIDTH - 1, WND_HEIGHT - 1);
}

void init() {
  int gd = DETECT, gm, result;
  char* msg;

  initgraph(&gd, &gm, "BGI");
  result = graphresult();
  if (result) {
    msg = grapherrormsg(result);
    printf("%s", msg);
  }

  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
}

void draw(int ptrSize, SYSTEM* ptrList, int mode, int isCoord, int isHyper,
          WAYPOINT* way, int currentPoint) {
  if (way->size)
    WND_WIDTH = 470;
  else
    WND_WIDTH = 639;

  /* Update map window bounds */
  map_wnd.x2 = WND_WIDTH;

  clearWnd();

  if (mode == 1) draw2dwnd(ptrSize, ptrList, isCoord, isHyper, way);

  if (mode == 3) drawyzwnd(ptrSize, ptrList, isCoord, isHyper, way);

  if (mode == 2) draw3dwnd(ptrSize, ptrList, isCoord, isHyper, way);

  if (way->size && dirty_path) pathWnd(way, currentPoint, ptrList);
  dirty_path = 0;

  if (dirty_topbar) topStatusLine();
  if (dirty_bottombar) statusLine();
  dirty_topbar = 0;
  dirty_bottombar = 0;
}

void moveScreenTo(SYSTEM* solar, int value) {
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

void gotoSystem(int ptrSize, SYSTEM* solar) {
  char* input;

  int size;
  int value;
  int error = 0;

  input = questionWnd("GSCARD", "Insert system COORD");

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

char* questionWnd(char* header, char* text) {
  char inputbuf[MAX_INPUT_LEN] = "";
  int c = 0;
  int input_pos = 0, the_end = 0;
  int wx = (WND_WIDTH - 320) / 2, wy = 180;

  drawWnd(wx, wy, 320, 100);

  setcolor(0);
  outtextxy(wx + 2, wy + 5, header);
  setcolor(4);
  outtextxy(wx + 2, wy + 20, text);
  outtextxy(wx + 2, wy + 88, "Press [ENTER] to confirm [ESC] to abort");

  moveto(wx + 5, wy + 40);

  setcolor(15);

  do {
    setfillstyle(SOLID_FILL, RED);
    bar(wx + 20, wy + 50, wx + 300, wy + 68);
    setcolor(0);
    outtextxy(wx + 25, wy + 55, inputbuf);

    c = getch();
    switch (c) {
      case 8: /* BACKSPACE */
        if (input_pos) {
          input_pos--;
          inputbuf[input_pos] = 0;
        }
        break;
      case 13: /* RETURN */
        the_end = 1;
        break;
      case ESC: /* ESC */
        inputbuf[0] = 0;
        the_end = 1;
        break;
      default:
        if (input_pos < MAX_INPUT_LEN - 1 && c >= ' ' && c <= '~') {
          inputbuf[input_pos] = c;
          input_pos++;
          inputbuf[input_pos] = 0;
        }
    }

  } while (!the_end);

  return inputbuf;
}

void warningWnd(char* header, char* text) {
  drawWnd((WND_WIDTH - 320) / 2, 180, 320, 100);
  setcolor(0);
  outtextxy(162, 185, header);
  setcolor(4);
  outtextxy(162, 200, text);
  setcolor(15);
}

void systemInfoWnd(char* header) {
  drawWnd(20, 20, 430, 400);
  setcolor(15);
  settextstyle(SMALL_FONT, HORIZ_DIR, 6);
  outtextxy(40, 40, "? ?,  ?  ? !");
}

void progressWnd(char* header, char* text, int current, int total) {
  float di;
  float x;
  unsigned int MEM;
  char memMsg[50] = "";

  MEM = coreleft();
  sprintf(memMsg, "M: %d", MEM);

  di = (float)current / total;

  if (di < 0) di = -1 * di;

  x = (280 * di) + 180;

  if (current == 0) {
    drawWnd((WND_WIDTH - 320) / 2, 180, 320, 100);
    setcolor(0);
    outtextxy(162, 185, header);
    setcolor(4);
    outtextxy(162, 200, text);
    setcolor(15);

    setcolor(4);
    rectangle(180, 230, 460, 248);
  }

  setfillstyle(SOLID_FILL, RED);
  bar(400, 185, 475, 195);
  bar(180, 230, x, 248);
  outtextxy(400, 185, memMsg);

  setcolor(0);
}

void drawWnd(int x, int y, int width, int height) {
  setfillstyle(SOLID_FILL, BLACK);
  setcolor(4);
  bar(x, y, x + width, y + height);
  rectangle(x, y, x + width, y + height);

  setfillstyle(SOLID_FILL, RED);
  bar(x, y, x + width, y + 16);

  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
}

void drawPathWnd(WAYPOINT* wp, int oy) {
  setfillstyle(SOLID_FILL, BLACK);
  setcolor(15);

  bar(WND_WIDTH + 1, 21, 639, WND_HEIGHT + 20);

  setfillstyle(SOLID_FILL, RED);
  setcolor(0);
  bar(WND_WIDTH + 1, 21, 639, 41);
  settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
  outtextxy(WND_WIDTH + 5, 26, "PATH");

  setcolor(4);
  setlinestyle(1, 0, 1);
  line(WND_WIDTH + 1, oy, 639, oy);

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(WND_WIDTH + 5, oy + 5, "PgUp/PgDn");
  setcolor(15);
  outtextxy(WND_WIDTH + 80, oy + 5, "-SELECT");

  if (wp->size < 15 && !pathListFlag) {
    adQuindett();
    pathListFlag = 1;
  }
}

void pathWnd(WAYPOINT* wp, int currentPoint, SYSTEM* ptrList) {
  int i, j, oy, yStep = 15;
  int o;
  char buf[50];

  pathListFlag = 0;

  oy = 46 + wp->size * yStep;
  drawPathWnd(wp, oy);

  setcolor(15);

  settextstyle(SMALL_FONT, HORIZ_DIR, 4);

  for (i = 0, j = (wp->size - 1); i < wp->size; i++, j--) {
    if (i == currentPoint) {
      setcolor(0);
      setfillstyle(SOLID_FILL, RED);
      bar(WND_WIDTH + 1, 46 + i * yStep, 639, 46 + i * yStep + 15);
    } else {
      setcolor(15);
      setfillstyle(SOLID_FILL, BLACK);
      bar(WND_WIDTH + 1, 46 + i * yStep, 639, 46 + i * yStep + 15);
    }

    sprintf(buf, "#%d: SA%d", i + 1, wp->way[i]);

    /* check if the segment leading TO this waypoint is dangerous */
    if (i > 0 && show_danger_path_parts && objSize) {
      int prev = wp->way[i - 1];
      int cur = wp->way[i];
      for (o = 0; o < objSize; o++) {
        if (sphereLineIntersect(ptrList[prev].x, ptrList[prev].y,
                                ptrList[prev].z, ptrList[cur].x, ptrList[cur].y,
                                ptrList[cur].z, objList[o].x, objList[o].y,
                                objList[o].z, objList[o].r)) {
          sprintf(buf, "#%d: SA%d [DANGER]", i + 1, wp->way[i]);
          break;
        }
      }
    }

    outtextxy(WND_WIDTH + 5, 46 + i * yStep, buf);
  }
}
