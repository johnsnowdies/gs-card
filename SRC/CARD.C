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
 *  File    : CARD.C
 *  Module  : MAIN
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GS-CARD main file, global variables initialization, screen change
 *      system with keyboard handlers system
 *
 *  =====================================================================
 *  !!  LEAD DEVELOPER NOTICE  !!
 *  =====================================================================
 *  UNDER NO CIRCUMSTANCES MENTION "UMBRELLA" IN ANY COMMENT, STRING,
 *  OR DOCUMENTATION.  HYPERSOFT IS NOT UMBRELLA.  THIS IS NOT PUBLIC
 *  INFORMATION.  REPEAT: DO NOT REFER TO UMBRELLA.  ANY VIOLATION
 *  WILL RESULT IN IMMEDIATE TERMINATION OF YOUR CONTRACT WITH
 *  HYPERSPORT!.
 *  =====================================================================
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#include <alloc.h>
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include "data/reader.h"
#include "data/structs.h"
#include "ui/ad/ad.h"
#include "ui/gui.h"
#include "ui/map/mapnav.h"
#include "ui/map/mapwnd.h"
#include "ui/menu/menunav.h"
#include "ui/menu/menuwnd.h"
#include "ui/shipyard/syardnav.h"
#include "ui/shipyard/syardwnd.h"
#include "ui/stat/statnav.h"
#include "ui/stat/statwnd.h"
#include "ui/upgrade/upgrnav.h"
#include "ui/upgrade/upgrwnd.h"

/* ----------------------------------------------------------------
 * GLOBALS
 * ---------------------------------------------------------------- */
const int DEBUG = 0;

/* Data related global variables */
SYSTEM* sol_list;
OBJECT* obj_list;
BOUND_LINE* bnd_list;
QUEST system_quests[5];
UPGRADE system_upgrades[8];
SHIP system_shipyard[6];

/* Sizes */
unsigned int sol_size;
unsigned int obj_size;
unsigned int bnd_size;
int system_quests_size = 0;
int system_upgrades_size = 0;
int system_shipyard_size = 0;

/* Pathfinder waypoints */
WAYPOINT wp;

/* Game state */
GAME_STATE gs;

/* Exit signal */
unsigned char SIG_TERM = 0;

/* Root window */
WND root_wnd = {SCR_MAIN_MENU, NULL, NULL, 0, 21, 639, 460};

/* Screens */
E_GAME_SCREEN cur_screen = SCR_MAIN_MENU;

/* ----------------------------------------------------------------
 * HANDLERS TABLE
 * ---------------------------------------------------------------- */
typedef int (*key_handler)(int ch);
static key_handler key_handlers[] = {
    gui_map_wnd_key,       /* SCR_MAP */
    gui_menu_main_wnd_key, /* SCR_MAIN_MENU */
    gui_menu_game_wnd_key, /* SCR_GAME_MENU */
    gui_status_wnd_key,    /* SCR_STATUS */
    gui_upgrade_wnd_key,   /* SCR_UPGRADE */
    gui_shipyard_wnd_key   /* SCR_SHIPYARD */
};

typedef int (*wnd_dispatcher)();
wnd_dispatcher wnd_dispatchers[] = {
    gui_map_wnd_dispatch,         /* SCR_MAP */
    gui_menu_main_wnd_dispatcher, /* SCR_MAIN_MENU */
    gui_menu_game_wnd_dispatcher, /* SCR_GAME_MENU */
    gui_status_wnd_dispatch,      /* SCR_STATUS */
    gui_upgrade_wnd_dispatch,     /* SCR_UPGRADE */
    gui_shipyard_wnd_dispatch     /* SCR_SHIPYARD */
};

WND* windows[] = {
    &map_wnd,       /* SCR_MAP */
    &main_menu_wnd, /* SCR_MAIN_MENU */
    &game_menu_wnd, /* SCR_GAME_MENU */
    &status_wnd,    /* SCR_STATUS */
    &upgrade_wnd,   /* SCR_UPGRADE */
    &shipyard_wnd   /* SCR_SHIPYARD */
};

void dispatch_wnd(E_GAME_SCREEN id) {
  if (cur_screen == SCR_MAIN_MENU)
    root_wnd.id = SCR_MAIN_MENU;
  if (cur_screen == SCR_GAME_MENU)
    root_wnd.id = SCR_GAME_MENU;
  cur_screen = id;
  wnd_dispatchers[id]();
}

/* ----------------------------------------------------------------
 * MAIN
 * ---------------------------------------------------------------- */
int main() {
  char c;
  srand((unsigned)time(NULL));

  sol_size = data_reader_load_systems(&sol_list);
  bnd_size = data_reader_load_bounds(&bnd_list);
  obj_size = data_reader_load_objects(&obj_list);

  gui_init();

  gui_splash();
  
  core_finder_calc_hyper_threads();

  gui_clrscr();

  main_menu_wnd.ptr_parent = &root_wnd;
  /* Draw Main Menu */
  dispatch_wnd(SCR_MAIN_MENU);

  while (!SIG_TERM) {
    c = getch();
    if (cur_screen >= 0 && cur_screen < 6) {
      key_handlers[cur_screen](c);
    }
  }

  /* Cleanup */
  if (sol_list) free(sol_list);
  if (obj_list) free(obj_list);
  if (bnd_list) free(bnd_list);
  closegraph();
  return 0;
}
