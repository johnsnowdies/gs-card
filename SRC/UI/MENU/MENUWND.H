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
 *  File    : MENUWND.H
 *  Module  : GUI/MENU
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI Menu Window, main menu and ingame menu modes. New game,
 *      Save game and Load game options.
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */
#ifndef _GUI_MENU_WND
#include "ui/gui.h"

#define _GUI_MENU_WND

/* Main menu */
#define MAIN_MENU 1
#define GAME_MENU 2

extern WND main_menu_wnd;
extern WND game_menu_wnd;

extern int mm_select;

/* UI/MENU/MENUWND API */
extern void gui_menu_main_wnd_dispatcher();
extern void gui_menu_game_wnd_dispatcher();
#endif
