#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "ui\menu\menuwnd.h"

#include "ui\menu\menunav.h"

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;

extern unsigned char SIG_TERM;
extern GAME_STATE gs;

extern mm_select;

extern unsigned int sol_size;

/* ----------------------------------------------------------
 * SCR_MAIN_MENU -- main menu on startup
 * ---------------------------------------------------------- */
int gui_menu_main_wnd_key(int ch, WND *parent)
{
    char* text_input;

    if (mm_select == 0 && ENTER == ch) {
        text_input = gui_input_wnd(parent, LC_CARD_MENU_NEW_WND_HEAD,
                                   LC_CARD_MENU_NEW_WND_TEXT, NULL);
        if (text_input != NULL && text_input[0] != '\0') {
            new_game(text_input, sol_size);
            cur_screen = SCR_MAP;
            gui_map_wnd_draw();
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE);
            getch();
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
    }
    if (mm_select == 1 && ENTER == ch) {
        text_input = gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD, LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (load_game(&gs, "USER.SAV") == 1) {
                cur_screen = SCR_MAP;
                gui_map_wnd_draw();
            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, MAIN_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE);
            getch();
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
    }
    if (mm_select == 2 && ENTER == ch) {
        SIG_TERM = 1;
    }
    if (UP == ch) {
        if (mm_select > 0) {
            mm_select--;
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
    }
    if (DWN == ch) {
        if (mm_select < 2) {
            mm_select++;
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
    }
    return 0;
}

/* ----------------------------------------------------------
 * SCR_GAME_MENU -- in-game menu on ESC
 * ---------------------------------------------------------- */
int gui_menu_game_wnd_key(int ch, WND *parent)
{
    char *text_input;

    if (ESC == ch) {
        mm_select = 0;
        cur_screen = prev_screen;
        if (cur_screen == SCR_MAP)
            gui_map_wnd_draw();
        if (cur_screen == SCR_STATUS)
            gui_status_wnd();
    }
    if (mm_select == 0 && ENTER == ch) {
        text_input = gui_input_wnd(parent, LC_CARD_MENU_SAVE_WND_HEAD,
                                   LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (data_reader_save_game_file(&gs, text_input) == 1) {
                cur_screen = prev_screen;
                gui_warning_wnd(parent, LC_GEN_SUCCESS_HEAD, LC_CARD_MENU_SAVE_SUCCESS);
                getch();
                if (cur_screen == SCR_MAP)
                    gui_map_wnd_draw();
                if (cur_screen == SCR_STATUS)
                    gui_status_wnd();
            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_SAVE_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, GAME_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE);
            getch();
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
    }
    if (mm_select == 1 && ENTER == ch) {
        text_input = gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD,
                                   LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (load_game(&gs, "USER.SAV") == 1) {
                cur_screen = SCR_MAP;
                gui_map_top_status_line();
                gui_map_wnd_draw();
            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, GAME_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE);
            getch();
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
    }
    if (mm_select == 2 && ENTER == ch) {
        SIG_TERM = 1;
    }
    if (UP == ch) {
        if (mm_select > 0) {
            mm_select--;
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
    }
    if (DWN == ch) {
        if (mm_select < 2) {
            mm_select++;
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
    }
    return 0;
}
