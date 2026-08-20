/* mapwnd.h -- map window rendering API */
/* Only gui_map_wnd_draw() and gui_map_wnd_clear() are public.  All coordinate helpers
   and projection functions are static inside mapwnd.c. */

void gui_map_wnd_draw(int sol_size, struct system_solar* sol_list, int mode,
          int isCoord, int isHyper, struct waypoint* way, int current_point);

void gui_map_wnd_clear(void);
