/* menuwnd.h -- main menu window API */
#ifndef _GUI_MENU_WND
#include "ui/gui.h"

#define _GUI_MENU_WND

extern int mm_select;

void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode);
#endif
