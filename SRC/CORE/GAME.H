#ifndef _CORE_GAME
#define _CORE_GAME
#include "data/structs.h"

/* game.c - core gameplay logics API */
#define SHIP_ALMIGHTY 0
#define SHIP_BETSY 1
#define SHIP_IMPERIAL 2
#define SHIP_GAE_BULG 3
#define SHIP_NOVA 4
#define SHIP_BULK_CARRIER 5

#define HYPER_I 0
#define HYPER_II 1
#define HYPER_III 2
#define HYPER_UMBRELLA 3

/* Counters */
#define SHIP_COUNT 6
#define HYPER_COUNT 4
#define CUSTOM_UPGRADES_COUNT 5
#define FACTIONS_COUNT 4
#define SECTORS_COUNT 9
#define NAMES_COUNT 15
#define MALE_PORTRAITS_COUNT 6
#define FEMALE_PORTRAITS_COUNT 3

#define HYPER_UPGRADE_BASE_PRICES {0, 1000, 3000, 8000}
#define CUSTOM_UPGRADE_BASE_PRICES {500, 1500, 1200, 800, 1000}
#define SHIP_UPGRADE_BASE_PRICES {10000, 30000, 70000, 100000, 300000, 500000}


#define E_NPC_TYPE enum npc_type
E_NPC_TYPE {
    QUEST_NPC,
    GAS_NPC
};


#define E_GENDER enum gender_type

E_GENDER {
    MALE,
    FEMALE,
    RANDOM_GENDER
};

extern char* data_ship_names[SHIP_COUNT];
extern unsigned int data_ship_engines[SHIP_COUNT];
extern unsigned char data_ship_smuggler_bay[SHIP_COUNT];
extern unsigned char data_ship_continuous_jump[SHIP_COUNT];
extern unsigned char data_ship_emergency_jump[SHIP_COUNT];
extern unsigned int data_ship_tonnages[SHIP_COUNT];
extern char* data_hyper_names[HYPER_COUNT];
extern unsigned int data_hyper_fuel[HYPER_COUNT];
extern char* data_factions[FACTIONS_COUNT];
extern unsigned int data_factions_colors[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern long ship_prices[SHIP_COUNT];
extern long hyper_prices[HYPER_COUNT];
extern long custom_prices[CUSTOM_UPGRADES_COUNT];


void core_game_gen_quest(QUEST* ptr_quest, int player_rep, unsigned int faction);
int core_game_run_event(int fuel_consume);
int core_game_accept_quest(unsigned int index);

int core_game_load(char *filename);
int core_game_save(char *filename);

int core_game_accept_quest(unsigned int index);

void core_game_event_gas_station();

void core_game_event_quest_failed(int index);

int core_game_event_hijack();

int core_game_event_customs();

int core_game_event_kidnapping(int quest_index);

int core_game_event_danger_object();

#endif
