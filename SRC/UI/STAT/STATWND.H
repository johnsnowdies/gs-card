/* statuswnd.h -- current solar system status window API */
#ifndef _GUI_STAT_WND
#define _GUI_STAT_WND

#include "ui/gui.h"

extern WND status_wnd;
extern int system_quest_selected;

void gui_status_wnd();
void gui_status_quest_wnd();

#endif
