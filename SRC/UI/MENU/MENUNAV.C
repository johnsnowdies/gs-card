#include "ui\menu\menunav.h"

#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "core\objects.h"
#include "core\game.h"


#include "ui\locale.h"

#include "ui\menu\menuwnd.h"

#include "ui\upgrade\upgrwnd.h"
#include "ui\upgrade\upgrnav.h"

#include "music.h"

#include "core/globals.h"

/* ----------------------------------------------------------
 * SCR_MAIN_MENU -- main menu on startup
 * ---------------------------------------------------------- */
int gui_menu_main_wnd_key(int ch, WND *parent)
{
    

    if (mm_select == 0 && ENTER == ch) {
        char* text_input;
        text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_NEW_WND_HEAD,
                                   LC_CARD_MENU_NEW_WND_TEXT, NULL);
        if (text_input != NULL && text_input[0] != '\0') {
            mm_select = 0;
            new_game(text_input, sol_size);
            cur_screen = SCR_MAP;
            gui_map_wnd_draw();
            gui_bars_map_bottom();
            gui_bars_common_top();
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
            getch();
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
        free(text_input);
    }
    if (mm_select == 1 && ENTER == ch) {
        char* text_input;
        text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD, LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (core_game_load(text_input) == 1) {
                mm_select = 0;
                cur_screen = SCR_MAP;
                gui_map_wnd_draw();
                gui_bars_map_bottom();
                gui_bars_common_top();

            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR, SOUND_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, MAIN_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
            getch();
            gui_menu_wnd(parent, mm_select, MAIN_MENU);
        }
        free(text_input);
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
    if (ESC == ch) {
        mm_select = 0;
        cur_screen = prev_screen;
        if (cur_screen == SCR_MAP)
            gui_map_wnd_draw();
        if (cur_screen == SCR_STATUS)
            gui_status_wnd();
        if (cur_screen == SCR_UPGRADE)
            gui_upgrade_wnd();
        if (cur_screen == SCR_SHIPYARD)
            gui_shipyard_wnd();
    }
    if (mm_select == 0 && ENTER == ch) {
        char *text_input;
         text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD, LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (core_game_save(text_input) == 1) {
                cur_screen = prev_screen;
                gui_warning_wnd(parent, LC_GEN_SUCCESS_HEAD, LC_CARD_MENU_SAVE_SUCCESS, SOUND_SUCCESS);
                getch();
                mm_select = 0;
                if (cur_screen == SCR_MAP) {
                    gui_map_wnd_draw();
                    gui_bars_map_bottom();
                    gui_bars_common_top();
                }

                if (cur_screen == SCR_STATUS){
                    gui_bars_common_top();
                    gui_bars_status_bottom();
                    gui_status_wnd();
                }
                
                if (cur_screen == SCR_UPGRADE){
                    gui_bars_common_top();
                    gui_bars_status_bottom();
                    gui_upgrade_wnd();
                }

            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_SAVE_ERROR, SOUND_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, GAME_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
            getch();
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
        free(text_input);
    }
    if (mm_select == 1 && ENTER == ch) {
        char *text_input;
        text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD,
                                   LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
        if (text_input != NULL && text_input[0] != '\0') {
            if (core_game_load(text_input) == 1) {
                mm_select = 0;
                cur_screen = prev_screen;
                if (cur_screen == SCR_MAP) {
                    gui_map_wnd_draw();
                    gui_bars_map_bottom();
                    gui_bars_common_top();
                }

                if (cur_screen == SCR_STATUS){
                    gui_bars_common_top();
                    gui_bars_status_bottom();
                    gui_status_wnd();
                }
                
                if (cur_screen == SCR_UPGRADE){
                    gui_bars_common_top();
                    gui_bars_status_bottom();
                    gui_upgrade_wnd();
                }
            } else {
                gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR, SOUND_ERROR);
                getch();
                gui_menu_wnd(parent, mm_select, GAME_MENU);
            }
        } else {
            gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
            getch();
            gui_menu_wnd(parent, mm_select, GAME_MENU);
        }
        free(text_input);
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
