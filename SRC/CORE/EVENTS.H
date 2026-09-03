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
 *  File    : GAME.H
 *  Module  : CORE
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      Game events system after hyperspace jump
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */
#ifndef _CORE_EVENTS
#define _CORE_EVENTS

#define GAS_PERCENT_PRICE (rand() % 3) + 1

/* -----------------------------------------------------------------
 * EVENT - PIRACY CONSTANTS
 * ---------------------------------------------------------------- */

/* -----------------------------------------------------------------
   PIRACY ransom ranges
   Each range = MIN + rand() % RANGE, so effective range is
   MIN .. MIN + RANGE - 1.
   Values are chosen as 1%..5% of the ship base price.
   Ship prices: 10000, 30000, 70000, 100000, 300000, 500000
   ---------------------------------------------------------------- */
#define EVENT_PIRACY_RANSOM_MIN_TYPE0      100
#define EVENT_PIRACY_RANSOM_RANGE_TYPE0    401   /* 100..500 */

#define EVENT_PIRACY_RANSOM_MIN_TYPE1      300
#define EVENT_PIRACY_RANSOM_RANGE_TYPE1    1201  /* 300..1500 */

#define EVENT_PIRACY_RANSOM_MIN_TYPE2      700
#define EVENT_PIRACY_RANSOM_RANGE_TYPE2    2801  /* 700..3500 */

#define EVENT_PIRACY_RANSOM_MIN_TYPE3      1000
#define EVENT_PIRACY_RANSOM_RANGE_TYPE3    4001  /* 1000..5000 */

#define EVENT_PIRACY_RANSOM_MIN_TYPE4      3000
#define EVENT_PIRACY_RANSOM_RANGE_TYPE4    12001 /* 3000..15000 */

#define EVENT_PIRACY_RANSOM_MIN_TYPE5      5000
#define EVENT_PIRACY_RANSOM_RANGE_TYPE5    20001 /* 5000..25000 */

#define EVENT_PIRACY_RANSOM_MIN_DEFAULT    100
#define EVENT_PIRACY_RANSOM_RANGE_DEFAULT  401   /* same as type0 */

/* Pirate faction generation */
#define EVENT_PIRACY_IRISH_PROB 90       /* % chance of faction 1 */
#define EVENT_PIRACY_OTHER_FACTION_MAX 4 /* random faction 0..3 */

/* -----------------------------------------------------------------
 * EVENT - CUSTOMS CONSTANTS
 * ---------------------------------------------------------------- */

#define EVENT_CUSTOMS_BRIBE 500 + rand() % 1501 /* 500..2000 */

/* -----------------------------------------------------------------
 *  EVENT - KIDNAPPING CONSTANTS
 * ---------------------------------------------------------------- */

#define EVENT_CUSTOMS_IRISH_PROB 90 /* % chance of faction 1 */

/* -----------------------------------------------------------------
 *
 *                         EVENT SYSTEM CORE
 *                  PROBABILITIES (PERCENTAGE, 0..99)
 *
 *  ---------------------------------------------------------------- */

/* Piracy event chances in faction related systems:
   faction 1 (Irish) -> 30%
   other factions    -> 10%
*/
#define EVENT_PIRACY_PROB_FACTION_1 10
#define EVENT_PIRACY_PROB_OTHER 5

/* Customs event chances (only in shipyard systems):
   faction 2 (Sentinels) -> 70%
   other     -> 20%
*/
#define EVENT_CUSTOMS_PROB_FACTION_2 50
#define EVENT_CUSTOMS_PROB_OTHER 10

/* Kidnapping event chance when a type 4 quest is active */
#define EVENT_KIDNAPPING_PROB 10

/* ---------------------------------------------------------------- */

/* CORE/EVENTS API */
extern void core_events_new_game(void);
extern int core_events(int fuel_consume);

#endif
