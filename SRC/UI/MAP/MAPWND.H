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
 *  File    : MAPWND.H
 *  Module  : GUI/MAP
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      GUI Map Window - application main UI will be used in 90% of time.
 *      Galaxy representation in projections:
 *      - xy top-down
 *      - xyz isometric
 *      - yz side
 *      Zooming, panning, navigating; Dangerous objects render,
 *      political map bounds, solar systems labels;
 *      Hyper-threads renderer, path render
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI_MAP_WND
#define _GUI_MAP_WND

#include "ui/gui.h"

#define MAX_VALUE  1400
#define MIN_VALUE -1400

/* ----------------------------------------------------------------
 * EXTERN VIEWPORT GLOBALS
 * ---------------------------------------------------------------- */
extern int offsetX, offsetY, offsetZ;
extern float xmin, xmax, ymin, ymax, zmin, zmax;
extern float xdens, ydens;

extern int is_coord, is_hyper, mode;
extern int path_wnd_index;

extern float xmin, xmax;
extern float ymin, ymax;
extern float zmin, zmax;
extern int offsetX, offsetY, offsetZ;
extern float xdens, ydens;

extern int path_wnd_index;
extern WND map_wnd;

/* UI/MAP/MAPWND API */

/* Use only in MAPNAV */
extern void gui_map_wnd_refresh();

extern void gui_map_wnd_dispatch();

#endif
