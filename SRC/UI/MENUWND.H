/* menuwnd.h -- main menu window API */
#define MAIN_MENU 1
#define GAME_MENU 2

void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode);
int gui_main_menu_key(int ch, WND *parent);
int gui_game_menu_key(int ch, WND *parent);