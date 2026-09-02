#include <graphics.h>
#include <stdio.h>

#include "core/game.h"
#include "core/globals.h"
#include "data/keys.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/locale.h"
#include "ui/map/mapwnd.h"
#include "ui/menu/menunav.h"
#include "ui/menu/menuwnd.h"

/* ----------------------------------------------------------
 * EXTERN: SCR_MAIN_MENU -- MAIN MENU KEY HANDLER
 * ---------------------------------------------------------- */
int gui_menu_main_wnd_key(int ch, WND* parent) {
  /* MAIN MENU -- NEW GAME */
  if (mm_select == 0 && ENTER == ch) {
    char* text_input;
    text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_NEW_WND_HEAD,
                                      LC_CARD_MENU_NEW_WND_TEXT, NULL);
    if (text_input != NULL && text_input[0] != '\0') {
      mm_select = 0;
      core_game_new_game(text_input);
      dispatch_wnd(SCR_MAP);
    } else {
      gui_warning_wnd(parent, LC_GEN_ERROR_HEAD,
                      LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
    }
    free(text_input);
  }
  /* MAIN MENU -- LOAD GAME */
  if (mm_select == 1 && ENTER == ch) {
    char* text_input;
    text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD,
                                      LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
    if (text_input != NULL && text_input[0] != '\0') {
      if (core_game_load(text_input) == 1) {
        dispatch_wnd(SCR_MAP);
      } else {
        gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR,
                        SOUND_ERROR);
      }
    } else {
      gui_warning_wnd(parent, LC_GEN_ERROR_HEAD,
                      LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
    }
    free(text_input);
  }
  /* MAIN MENU -- EXIT */
  if (mm_select == 2 && ENTER == ch) {
    SIG_TERM = 1;
  }
  if (UP == ch) {
    if (mm_select > 0) {
      mm_select--;
      dispatch_wnd(SCR_MAIN_MENU);
    }
  }
  if (DWN == ch) {
    if (mm_select < 2) {
      mm_select++;
      dispatch_wnd(SCR_MAIN_MENU);
    }
  }
  return 0;
}

/* ----------------------------------------------------------
 * EXTERN: SCR_GAME_MENU -- IN-GAME MENU KEY HANDLER
 * ---------------------------------------------------------- */
int gui_menu_game_wnd_key(int ch, WND* parent) {
  if (ESC == ch) {
    mm_select = 0;
    dispatch_wnd(parent->id);
  }
  /* GAME MENU -- SAVE GAME */
  if (mm_select == 0 && ENTER == ch) {
    char* text_input;
    text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD,
                                      LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
    if (text_input != NULL && text_input[0] != '\0') {
      if (core_game_save(text_input) == 1) {
        gui_warning_wnd(parent, LC_GEN_SUCCESS_HEAD, LC_CARD_MENU_SAVE_SUCCESS,
                        SOUND_SUCCESS);
      } else {
        gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_SAVE_ERROR,
                        SOUND_ERROR);
      }
    } else {
      gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE,
                      SOUND_ERROR);
    }
    free(text_input);
  }
  /* GAME MENU -- LOAD GAME */
  if (mm_select == 1 && ENTER == ch) {
    char* text_input;
    text_input = (char*)gui_input_wnd(parent, LC_CARD_MENU_LOAD_WND_HEAD,
                                      LC_CARD_MENU_SAVE_WND_TEXT, "USER.SAV");
    if (text_input != NULL && text_input[0] != '\0') {
      if (core_game_load(text_input) == 1) {
        dispatch_wnd(SCR_MAP);
      } else {
        gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_CARD_MENU_LOAD_ERROR,
                        SOUND_ERROR);
      }
    } else {
      gui_warning_wnd(parent, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE,
                      SOUND_ERROR);
    }
    free(text_input);
  }
  /* GAME MENU -- EXIT */
  if (mm_select == 2 && ENTER == ch) {
    SIG_TERM = 1;
  }

  if (UP == ch) {
    if (mm_select > 0) {
      mm_select--;
      dispatch_wnd(SCR_GAME_MENU);
    }
  }

  if (DWN == ch) {
    if (mm_select < 2) {
      mm_select++;
      dispatch_wnd(SCR_GAME_MENU);
    }
  }
  return 0;
}
