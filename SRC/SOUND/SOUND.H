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
 *  File    : SOUND.H
 *  Module  : SOUND
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      Beep-boop
 *
 *  Revisor note:
 *      John, this isn't funny, this program DID NOT NEED SOUND. Why?
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */
#ifndef _MUSIC
#define _MUSIC

#define MS_TO_TICKS(ms) ((ms) / 55)
#define E_SFX_MODAL_SOUND enum sfx_modal_sound
E_SFX_MODAL_SOUND{NO_SOUND, SOUND_WARNING, SOUND_ERROR, SOUND_SUCCESS};

/* SOUND API */
extern void sfx_menu_move();
extern void sfx_screen_change();
extern void sfx_window_open();
extern void sfx_modal();
extern void sfx_success();
extern void sfx_error();

#endif
