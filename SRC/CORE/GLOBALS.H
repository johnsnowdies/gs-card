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
 *  File    : GLOBALS.H
 *  Module  : CORE
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      Global variables initialized in CARD.C
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _CORE_GLOBALS
#define _CORE_GLOBALS

#include "data/structs.h"
#include "ui/gui.h"

extern const int DEBUG;

/* Data related global variables */
extern SYSTEM* sol_list;
extern OBJECT* obj_list;
extern BOUND_LINE* bnd_list;
extern QUEST system_quests[5];
extern UPGRADE system_upgrades[8];
extern SHIP system_shipyard[6];

extern unsigned int sol_size;
extern unsigned int obj_size;
extern unsigned int bnd_size;
extern int system_quests_size;
extern int system_upgrades_size;
extern int system_shipyard_size;

extern WAYPOINT wp;

/* Game state */
extern GAME_STATE gs;

/* Exit signal */
extern unsigned char SIG_TERM;

extern WND root_wnd;

extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;
#endif
