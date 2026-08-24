#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "core\objects.h"

#include "ui\gui.h"
#include "ui\menuwnd.h"

#include "ui\locale.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */

extern unsigned int sol_size;

/* SCREEN NAVIGATION */
extern enum game_screen;
extern enum game_screen cur_screen;
extern enum game_screen prev_screen;

extern unsigned char SIG_TERM;
extern struct game_state gs;

char* MAIN_MENU_ITEMS[3] = {
    LC_MENU_NEW_GAME,
    LC_MENU_LOAD,
    LC_MENU_EXIT
};

char* GAME_MENU_ITEMS[3] = {
    LC_MENU_SAVE,
    LC_MENU_LOAD,
    LC_MENU_EXIT
};

int mm_select = 0;


void gui_menu_wnd(WND* ptr_parent, int currentPos, int mode)
{
    WND menu_wnd;
    int i = 0;
    int wx = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    int wy = ((ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2) + 25;
    char **ITEMS;

    if (mode == MAIN_MENU)
        ITEMS = MAIN_MENU_ITEMS;
    else
        ITEMS = GAME_MENU_ITEMS;

    menu_wnd.header = "GS-CARD v1.5";

    menu_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    menu_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;
    menu_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    menu_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    gui_draw_wnd_proto(&menu_wnd);

    setcolor(4);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    
    for (i = 0; i < 3; i++){
        if (i == currentPos){
            setfillstyle(SOLID_FILL, RED);
            bar(wx+10, wy+(20*i), wx + WND_MODAL_DEFAULT_WIDTH - 10, wy+(20*(i+1)));
            setcolor(0);
        }
        else
        {
            setcolor(BAR_COLOR);
        }

        outtextxy(wx+20, wy+5+(20*i), ITEMS[i]);
    }
}

/* ----------------------------------------------------------
 * SCR_MAIN_MENU -- main menu on startup
 * ---------------------------------------------------------- */
int gui_main_menu_key(int ch, WND *parent)
{
    char *text_input;

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
int gui_game_menu_key(int ch, WND *parent)
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
            if (save_game_file(&gs, text_input) == 1) {
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
