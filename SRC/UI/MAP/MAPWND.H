/* mapwnd.h -- map window rendering API */
/* Only gui_map_wnd_draw() and gui_map_wnd_clear() are public.  All coordinate helpers
   and projection functions are static inside mapwnd.c. */
#ifndef _GUI_MAP_WND
#define _GUI_MAP_WND

#define MAX_VALUE  1400
#define MIN_VALUE -1400

void gui_map_wnd_draw();

#endif