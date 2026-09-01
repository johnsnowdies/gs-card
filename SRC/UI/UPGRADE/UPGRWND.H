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
 *  File    : UPGRWND.H
 *  Module  : GUI/UPGRADE
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI Upgrade window
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI_UPGR_WND
#define _GUI_UPGR_WND

#include "ui/gui.h"

#define UPGRADE_WND_DEFAULT_X 0
#define UPGRADE_WND_DEFAULT_Y 21
#define UPGRADE_WND_DEFAULT_WIDTH 639
#define UPGRADE_WND_DEFAULT_HEIGHT 439

extern WND upgrade_wnd;
extern int upgrade_selected;

/* UP/UPGRADE/UPGRWND API */
extern void gui_upgrade_wnd(void);
extern void gui_upgrade_draw_list(void);
extern void gui_upgrade_show_info(int selected);
extern void gui_upgrade_wnd_dispatch(WND* parent);

#endif
