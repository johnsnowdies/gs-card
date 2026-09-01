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
 *  Module  : GUI/NPC
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI NPC Window module - cover up gui_dialog_wnd for NPC
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI_NPC_WND
#define _GUI_NPC_WND
#define E_NPC_WND_TYPE enum npc_wnd_type
#include "data/structs.h"
#include "ui\gui.h"

E_NPC_WND_TYPE{NPC_CHOICE_WND, NPC_DIALOG_WND};

int gui_npc_wnd(WND* ptr_parent, NPC* ptr_npc, int wnd_type, char title[100],
                char lines[][100], int lines_count, char buttons[][100],
                int buttons_count, int buttons_orient);
#endif
