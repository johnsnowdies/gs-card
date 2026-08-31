/* mapwnd.h -- map window rendering API */
/* Only gui_map_wnd_draw() and gui_map_wnd_clear() are public.  All coordinate helpers
   and projection functions are static inside mapwnd.c. */
#ifndef _GUI_MAP_WND
#define _GUI_MAP_WND

#include "ui/gui.h"

#define MAX_VALUE  1400
#define MIN_VALUE -1400

/* ----------------------------------------------------------------
 * Extern viewport globals (defined in mapwnd.c)
 * ---------------------------------------------------------------- */
extern int offsetX, offsetY, offsetZ;
extern float xmin, xmax, ymin, ymax, zmin, zmax;
extern float xdens, ydens;

extern int is_coord, is_hyper, mode;
extern int path_wnd_index;

extern float xmin, xmax;
extern float ymin, ymax;
extern float zmin, zmax;
extern int offsetX, offsetY, offsetZ;
extern float xdens, ydens;

extern int path_wnd_index;
extern WND map_wnd;


void gui_map_wnd_draw();

#endif
