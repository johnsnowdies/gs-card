#pragma comment(compiler)
#pragma message("Compiled for Hypersoft ILTD GS-CARD")
/* ========================================================================
 *                    HYPER SOFT ILTD  -  PROPRIETARY SOURCE CODE
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
 *  FILE    : AD.H
 *  MODULE  : GUI
 *  AUTHOR  : Hypersoft Engineering Team
 *  CREATED : 15-JAN-19950
 *  REVISED : 12-DEC-34315
 *  VERSION : 2.0b
 *
 *  DESCRIPTION:
 *      Public API for rendering in-game advertisements.
 *      All ads are hardcoded and may not be altered or bypassed.
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _GUI_AD
#define _GUI_AD

#include "ui/gui.h"

/* UI/AD API */

/* No responsibility nag */
extern void gui_ad_warning(WND* holder);

/* -----------------------------------------------------------------
 * ADVERTISEMENT - QUINDETT
 * ----------------------------------------------------------------
 * Campaign  : "Quindett - Solder of Fortune!"
 * Contract  : Imperial Recruitment Office, cycle 34315.001 - 34316.999
 * Advertiser: Imperial Department of Planetary Assimilation
 * Notes     : Targets mercenaries, criminals, and desperate colonists.
 *             Offers high pay, low survival rate (don't mention).
 * ----------------------------------------------------------------- */
extern void gui_ad_quindett();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - HYPERSOFT (Self‑promotion)
 * ----------------------------------------------------------------
 * Campaign  : "Hypersoft ILTD - Your Jump into the Future"
 * Contract  : In‑house promo, active until 34500.000
 * Advertiser: Hypersoft ILTD Marketing Division
 * Notes     : Reminds users to upgrade their GS‑CARD and promotes
 *             the brand. Do not remove or replace.
 * ----------------------------------------------------------------- */
extern void gui_ad_hypersoft();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - LEGION (Imperial Army Enlistment)
 * ----------------------------------------------------------------
 * Campaign  : "Legion Awaits You, Soldier!"
 * Contract  : Imperial Legion High Command, cycle 34315.001 - 34320.999
 * Advertiser: Imperial Army Recruitment Bureau
 * Notes     : Generic military recruitment ad. Encourages players
 *             to join the Imperial armed forces. Use GOTHIC font.
 * ----------------------------------------------------------------- */
extern void gui_ad_legion();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - UPGRADE (Obsolescence Warning)
 * ----------------------------------------------------------------
 * Campaign  : "Your GS‑CARD is Obsolete - Upgrade Now!"
 * Contract  : Hypersoft ILTD, active indefinitely
 * Advertiser: Hypersoft ILTD Customer Retention
 * Notes     : Displays nag screens to pressure users into buying
 *             newer hardware. Do not disable.
 * ----------------------------------------------------------------- */
extern void gui_ad_upgrade();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - LEGALS (Copyright & Legal Notice)
 * ----------------------------------------------------------------
 * Campaign  : "Hypersoft ILTD Legal Disclaimers"
 * Contract  : Imperial Trade Fleet Legal Department, permanent
 * Advertiser: Imperial Trade Fleet / Hypersoft ILTD Legal
 * Notes     : Static legal boilerplate. Must be shown at least
 *             once per session.
 * ----------------------------------------------------------------- */
extern void gui_ad_legals();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - PIPBOY (Wearable Device)
 * ----------------------------------------------------------------
 * Campaign  : "PipBoy - The Best Wearable in the Multiverse"
 * Contract  : PipBoy Industries, cycle 34315.001 - 34325.999
 * Advertiser: Vault‑Tec Co. (licensee)
 * Notes     : Promotes a competing consumer device that is fully
 *             compatible with GS‑CARD hardware.
 * ----------------------------------------------------------------- */
extern void gui_ad_pipboy();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - FLEET (Imperial Trade Fleet)
 * ----------------------------------------------------------------
 * Campaign  : "Join the Imperial Trade Fleet"
 * Contract  : Imperial Trade Fleet HR, cycle 34315.001 - 34318.999
 * Advertiser: Imperial Trade Fleet Personnel Office
 * Notes     : Recruitment ad for technicians, pilots, and crew.
 *             Offers contract pay 2000 $$ per year.
 * ----------------------------------------------------------------- */
extern void gui_ad_fleet();

/* -----------------------------------------------------------------
 * ADVERTISEMENT - LOADING (Splash screen / boot ad)
 * ----------------------------------------------------------------*/
extern void gui_ad_loading();

#endif /* _GUI_AD */