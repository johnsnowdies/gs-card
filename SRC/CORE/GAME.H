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
 *      Entire game logics: NPCs, quests, upgrades and shipyard generation,
 *      game state control (new, save, load);
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _CORE_GAME
#define _CORE_GAME
#include "data/structs.h"

/* IN-GAME SPACE SHIPS */
#define SHIP_ALMIGHTY 0
#define SHIP_BETSY 1
#define SHIP_IMPERIAL 2
#define SHIP_GAE_BULG 3
#define SHIP_NOVA 4
#define SHIP_BULK_CARRIER 5

/* IN-GAME HYPER ENGINES */
#define HYPER_I 0
#define HYPER_II 1
#define HYPER_III 2
#define HYPER_UMBRELLA 3 /* REMOVE UMBRELLA MENTION! */

/* COUNTERS */
#define SHIP_COUNT 6
#define HYPER_COUNT 4
#define CUSTOM_UPGRADES_COUNT 5
#define FACTIONS_COUNT 4
#define SECTORS_COUNT 9
#define NAMES_COUNT 15
#define MALE_PORTRAITS_COUNT 6
#define FEMALE_PORTRAITS_COUNT 3

/* IN-GAME FIXED PRICES */
#define HYPER_UPGRADE_BASE_PRICES {0, 1000, 3000, 8000}
#define CUSTOM_UPGRADE_BASE_PRICES {500, 1500, 1200, 800, 1000}
#define SHIP_UPGRADE_BASE_PRICES {10000, 30000, 70000, 100000, 300000, 500000}

#define E_NPC_TYPE enum npc_type
E_NPC_TYPE{QUEST_NPC, GAS_NPC};

#define E_GENDER enum gender_type

E_GENDER{MALE, FEMALE, RANDOM_GENDER};

/* EXTERNAL GLOBAL VARIABLES */
extern const char* data_ship_names[SHIP_COUNT];
extern const unsigned int data_ship_engines[SHIP_COUNT];
extern const unsigned char data_ship_smuggler_bay[SHIP_COUNT];
extern const unsigned char data_ship_continuous_jump[SHIP_COUNT];
extern const unsigned char data_ship_emergency_jump[SHIP_COUNT];
extern const unsigned int data_ship_tonnages[SHIP_COUNT];
extern const char* data_hyper_names[HYPER_COUNT];
extern const unsigned int data_hyper_fuel[HYPER_COUNT];
extern const char* data_factions[FACTIONS_COUNT];
extern const unsigned int data_factions_colors[FACTIONS_COUNT];
extern const char* data_sectors[SECTORS_COUNT];
extern const long ship_prices[SHIP_COUNT];
extern const long hyper_prices[HYPER_COUNT];
extern const long custom_prices[CUSTOM_UPGRADES_COUNT];
extern const char* data_upgrade_names[CUSTOM_UPGRADES_COUNT];

/* CORE GAME API */
extern void core_game_gen_npc(NPC* ptr_npc, unsigned int faction,
                              E_GENDER gender, E_NPC_TYPE npc_type);
extern void core_game_gen_all();
extern int core_game_is_visited(int system);
extern void core_game_mark_visited();

extern int core_game_accept_quest(unsigned int index);

extern void core_game_new_game(char* name);
extern int core_game_load(char* filename);
extern int core_game_save(char* filename);

#endif
