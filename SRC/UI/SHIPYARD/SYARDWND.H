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
 *  File    : SYARDWND.H
 *  Module  : GUI/SHIPYARD
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI Shipyard's Window - list of ships for sale at shipyard.
 *      Detailed offer information, deal window
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI_SYARD_WND
#define _GUI_SYARD_WND

#define SYARD_WND_DEFAULT_X 0
#define SYARD_WND_DEFAULT_Y 21
#define SYARD_WND_DEFAULT_WIDTH 639
#define SYARD_WND_DEFAULT_HEIGHT 439

extern WND shipyard_wnd;;
extern int ship_selected;

/* UI/SHIPYARD/SYARDWND API */
extern void gui_shipyard_deal_wnd(void);
extern void gui_shipyard_draw_list(void);
extern void gui_shipyard_wnd_dispatch(WND* parent);

#endif
