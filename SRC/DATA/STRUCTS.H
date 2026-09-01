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
 *  File    : STRUCTS.H
 *  Module  : DATA
 *  Author  : Hypersoft Engineering Team
 *  Created : 15-JAN-19950
 *  Revised : 12-DEC-34315
 *  Version : 2.0b
 *
 *  Description:
 *      Global structures definitions
 *
 *  WARNING:
 *      This software is provided "AS IS" without warranty of any kind,
 *      express or implied.  Use of this code is at your own risk.
 *      See the Hypersoft ILTD Software License Agreement for full terms.
 *
 * ========================================================================
 */

#ifndef _DATA_STRUCTS
#define _DATA_STRUCTS

#define THREAD struct thread
THREAD {
  unsigned int value;
  int cost;
};

#define SYSTEM struct system_solar
SYSTEM {
  /* POSITIONING */
  int x;
  int y;
  int z;

  /* PATHFINDER DATA */
  THREAD far* threads;
  unsigned char threadSize;
  int rate;
  unsigned char visited;

  /* GAME DATA */
  unsigned int faction;
  unsigned int is_shipyard;
  unsigned int is_gas_station;
  unsigned int sector;
};

#define OBJECT struct object
OBJECT {
  int x;
  int y;
  int z;
  int r;
  int type;
};

#define POINT struct point
POINT {
  int x;
  int y;
};

#define BOUND_LINE struct bound_line
BOUND_LINE {
  POINT* a;
  POINT* b;
};

#define WAYPOINT struct waypoint
WAYPOINT {
  int way[25];
  int size;
};

#define NPC struct npc
NPC {
  char name[100];
  char photo[30];
  unsigned int faction;
  unsigned int gender;
};

#define QUEST struct quest
QUEST {
  unsigned long reward;
  unsigned long penalty;
  unsigned int type;
  unsigned int target_system;
  unsigned int target_sector;
  unsigned int cargo;
  unsigned int jumps;
  NPC giver;
};

#define GAME_STATE struct game_state
GAME_STATE {
  char captain_name[100];
  long balance;

  int current_system;
  int prev_system;

  unsigned int ship_type;
  unsigned int tonnage;
  unsigned int current_cargo;
  unsigned int hyper_class;

  unsigned char upgrade_smuggler_bay;
  unsigned char upgrade_continuous_jump;
  unsigned char upgrade_emergency_jump;

  unsigned char upgrade_objects_map;
  unsigned char upgrade_political_map;

  unsigned int reputation;
  unsigned int missions_completed;
  unsigned int fuel;
  unsigned int quests_size;
  QUEST quests[5];
  unsigned char* visited;
  int visited_bytes;
};

#define UPGRADE struct upgrade
UPGRADE {
  char* name;
  long base_price;
  int type;
  int id;
  NPC giver;
};

#define SHIP struct ship
SHIP {
  char* name;
  long base_price;
  int id;
  NPC giver;
  unsigned int hyper_class;
  unsigned char upgrade_smuggler_bay;
  unsigned char upgrade_continuous_jump;
  unsigned char upgrade_emergency_jump;
};

#endif
