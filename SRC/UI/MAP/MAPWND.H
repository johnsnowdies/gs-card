/* mapwnd.h -- map window rendering API */
/* Only gui_map_wnd_draw() and gui_map_wnd_clear() are public.  All coordinate helpers
   and projection functions are static inside mapwnd.c. */

extern WND map_wnd;
void gui_map_wnd_draw();
void gui_map_wnd_clear(void);
int gui_map_wnd_key(int ch, WND *parent);