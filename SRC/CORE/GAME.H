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

/* NPC CONSTANTS AND ENUMS */
#define MALE_PORTRAITS_COUNT 9
#define FEMALE_PORTRAITS_COUNT 4

#define E_NPC_TYPE enum npc_type
E_NPC_TYPE{QUEST_NPC, GAS_NPC};

#define E_GENDER enum gender_type

E_GENDER{MALE, FEMALE, RANDOM_GENDER};

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
#define SYSTEM_QUESTS_MAX_COUNT 5
/* 4 + hyper engine */
#define SYSTEM_UPGRADES_MAX_COUNT 4
#define PLAYER_QUESTS_MAX_COUNT 5

/* ----------------------------------------------------------------
 *
 *                      GAME MATH CONSTANTS
 *
 * ---------------------------------------------------------------- */


/* HYPER ENGINE UPGRADE PRICES */
#define HYPER_UPGRADE_BASE_PRICES {0, 1000, 3000, 8000}

/* UPGRADE PRICES:
 *  - smuggler bay
 *  - continuous jump
 *  - emergency jump
 *  - danger objects map 
 *  - political map 
 */
#define CUSTOM_UPGRADE_BASE_PRICES {500, 1500, 1200, 800, 1000}

/* SHIP PRICES: Betsy, Almighty, Imperial, Gae Bulg, Nova, Bulk Carrier */
#define SHIP_UPGRADE_BASE_PRICES {10000, 30000, 70000, 100000, 300000, 500000}

/* SHIPYARD DEFAULT SHIP UPGRADES INSTALLED */
#define SHIPS_ENGINES_INSTALLED {0, 0, 1, 1, 3, 2}
#define SHIPS_SMUGGLER_BAY_INSTALLED {0, 1, 1, 1, 0, 0}
#define SHIPS_CONT_JUMP_INSTALLED  {0, 0, 1, 1, 1, 1}
#define SHIPS_EMERG_JUMP_INSTALLED {0, 0, 0, 1, 1, 1}
#define SHIP_TONNAGES {50, 80, 100, 150, 200, 400}

/* HYPER ENGINE FUEL CONSUME PER JUMP */
#define HYPER_ENGINE_FUEL_CONSUME {10, 8, 5, 2}

/* REPUTATION */
#define MAX_REPUTATION 1000
#define MIN_REPUTATION 0

/* NEW GAME CONSTANTS */
#define NEW_GAME_BALANCE 500

/* RAND() CHANCES */
#define SYSTEM_UPGRADE_AVAIL_CHANCE 70
#define NPC_IS_MALE_CHANCE 70


/* ------------------------------------------------------------------
   QUEST GENERATOR CONSTANTS
   ------------------------------------------------------------------ */

/* QUEST CHANCES - INVERTED, 4 and 5 are same */
#define QUEST_TYPE_1_CHANCE 50
#define QUEST_TYPE_2_CHANCE 75
#define QUEST_TYPE_3_CHANCE 90
#define QUEST_TYPE_4_CHANCE 95

/* Base reward ranges for different quest types.
   The formula is: base = MIN + rand() % RANGE
   Effective range: MIN .. MIN + RANGE - 1
*/
#define QUEST_TYPE1_BASE_MIN    100
#define QUEST_TYPE1_BASE_RANGE  201   /* 100..300 */

#define QUEST_TYPE2_BASE_MIN    250
#define QUEST_TYPE2_BASE_RANGE  251   /* 250..500 */

#define QUEST_TYPE3_BASE_MIN    400
#define QUEST_TYPE3_BASE_RANGE  401   /* 400..800 */

#define QUEST_TYPE45_BASE_MIN   200
#define QUEST_TYPE45_BASE_RANGE 251   /* 200..450 */

#define QUEST_DEFAULT_BASE      100

/* Multipliers applied to base reward.
   For quest types 1..3:
       reward = base * (1 + cargo / QUEST_CARGO_DIVISOR)
                     * (1 + jumps / QUEST_JUMP_DIVISOR)
   For quest types 4..5:
       reward = base * (1 + jumps / QUEST_JUMP_DIVISOR)
   cargo and jumps are cast to double before division.
*/
#define QUEST_CARGO_DIVISOR     200.0
#define QUEST_JUMP_DIVISOR      10.0

/* ------------------------------------------------------------------
   CARGO GENERATION CONSTANTS
   ------------------------------------------------------------------ */

/* Probability thresholds for cargo rarity (0.0 .. 1.0).
   A random number r in [0,1) selects the category:
       r < QUEST_CARGO_COMMON_PROB       -> common cargo
       r < QUEST_CARGO_UNCOMMON_PROB     -> uncommon cargo
       otherwise                         -> rare cargo
*/
#define QUEST_CARGO_COMMON_PROB      0.70
#define QUEST_CARGO_UNCOMMON_PROB    0.90

/* Common cargo: 1..50 */
#define QUEST_CARGO_COMMON_MIN       1
#define QUEST_CARGO_COMMON_RANGE     50

/* Uncommon cargo: 51..150 */
#define QUEST_CARGO_UNCOMMON_MIN     51
#define QUEST_CARGO_UNCOMMON_RANGE   100

/* Rare cargo: 151..400 */
#define QUEST_CARGO_RARE_MIN         151
#define QUEST_CARGO_RARE_RANGE       250

/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */


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
