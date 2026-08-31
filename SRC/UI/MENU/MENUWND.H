/* menuwnd.h -- main menu window API */
#ifndef _GUI_MENU_WND
#include "ui\gui.h"

#define _GUI_MENU_WND
#define MAIN_MENU 1
#define GAME_MENU 2

void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode);
#endif
