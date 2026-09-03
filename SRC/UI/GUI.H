#pragma comment(compiler)
#pragma message("Compiled for Hypersoft ILTD GS-CARD")
/*
 * ========================================================================
 *               HYPER SOFT ILTD  -  PROPRIETARY SOURCE CODE
 * ========================================================================
 *
 *  (C) COPYRIGHT 19950-34315 HYPERSPORT IMPERIAL LIMITED.
 *  ALL RIGHTS RESERVED.  UNPUBLISHED RIGHTS RESERVED UNDER
 *  IMPERIAL SPACE COPYRIGHT LAWS.
 *
 *  This file contains confidential and proprietary information of
 *  Hypersoft ILTD.  No part of this file may be reproduced, stored
 *  in a retrieval system, or transmitted, in any form or by any means,
 *  electronic, mechanical, photocopying, recording, or otherwise,
 *  without the prior written permission of Hypersoft ILTD.
 *
 *  File    : GUI.H
 *  Module  : GUI
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI Framework. Entire Window UI system based on BGI
 *      Relative windows, inherence, independent UI elements
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI
#define _GUI

/* ----------------------------------------------------------------
 * LAYOUT CONSTANTS
 * ---------------------------------------------------------------- */

/* VGA Screen */
#define SCREEN_W 640
#define SCREEN_H 480

/* Status Bar constants */
#define STATUSBAR_WIDTH 640
#define STATUSBAR_HEIGHT 20
#define STATUSBAR_BOTTOM_Y 461

/* Modal windows width */
#define WND_MODAL_DEFAULT_WIDTH 320
#define WND_MODAL_DEFAULT_HEIGHT 100
#define WND_DIALOG_DEFAULT_HEIGHT 220

/* Window header constants*/
#define WND_HEADER_FONT SMALL_FONT, HORIZ_DIR, 4
#define WND_HEADER_HEIGHT 16
#define WND_HEADER_X_OFFSET 2
#define WND_HEADER_Y_OFFSET 5

/* Input Field Length*/
#define Q_INPUT_LEN 30

/* Game Screens */
#define E_GAME_SCREEN enum game_screen
E_GAME_SCREEN{SCR_MAP,    SCR_MAIN_MENU, SCR_GAME_MENU,
              SCR_STATUS, SCR_UPGRADE,   SCR_SHIPYARD};

/* ----------------------------------------------------------------
 * GUI ELEMENTS
 * ---------------------------------------------------------------- */

#define INPUT_FIELD struct input_field
INPUT_FIELD {
  char buffer[Q_INPUT_LEN];
  int max_len;
  int cursor_pos;
  int x, y, width, height;
  int border_color;
  int text_color;
  int fill_color;
  unsigned char active;
  unsigned char visible;
};

#define PROGRESS_BAR struct progress_bar
PROGRESS_BAR {
  int x, y, width, height;
  int current;
  int total;
  int border_color;
  int fill_color;
  int bg_color;
  unsigned char visible;
};

#define WND struct gui_proto_window
WND {
  E_GAME_SCREEN id;
  WND* ptr_parent;
  char* header;
  int x;
  int y;
  int width;
  int height;
};

#define BTN struct gui_proto_button
BTN {
  char* text;
  int x;
  int y;
  int width;
  int height;
  unsigned char selected;
  unsigned char enabled;
  unsigned char visible;
};

/* ----------------------------------------------------------------
 * UI/GUI API
 * ---------------------------------------------------------------- */
extern void gui_warning_wnd(WND* ptr_parent, char* header, char* text,
                            int play_sound);

extern int gui_confirm_wnd(WND* ptr_parent, char* header, char* text);
extern char* gui_input_wnd(WND* ptr_parent, char* header, char* text,
                           char* defaultValue);

extern void gui_progress_wnd(WND* ptr_parent, char* header, char* text,
                             int current, int total);

extern int gui_dialog_wnd(WND* ptr_parent, char* header, char* title,
                          char* photo_file, char lines[][100], int lines_count,
                          char buttons[][100], int buttons_count,
                          int play_sound, int buttons_orient);

extern void gui_draw_status_line(WND* ptr_wnd, char* keys[], char* items[],
                                 int highlight);

extern void gui_memory_status(void);

extern void gui_draw_section_header(int x, int y, int width, char* title);
extern void gui_draw_section_header_v(int x, int y, int width, int height,
                                      char* title);

extern void gui_clrscr(void);
extern void gui_draw_wnd_proto(WND* ptr_wnd);
extern void gui_init(void);
extern void gui_dbg_out(char line[100]);
extern void gui_splash();

#endif
