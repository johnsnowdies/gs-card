/* game.c - core gameplay logics API */
#define SHIP_ALMIGHTY       0
#define SHIP_BETSY          1
#define SHIP_IMPERIAL       2
#define SHIP_GAE_BULG       3
#define SHIP_NOVA           4
#define SHIP_BULK_CARRIER   5

#define HYPER_I         0
#define HYPER_II        1
#define HYPER_III       2
#define HYPER_UMBRELLA  3

#define SHIP_COUNT     6
#define HYPER_COUNT    4
#define FACTIONS_COUNT 4
#define SECTORS_COUNT  9

#define MALE_PORTRAITS_COUNT 6
#define FEMALE_PORTRAITS_COUNT 3

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


void core_game_gen_npc(NPC* ptr_npc, unsigned int faction, E_GENDER gender, E_NPC_TYPE npc_type);

void core_game_gen_quest(QUEST* ptr_quest, int player_rep, unsigned int faction);
int core_game_run_event();
int core_game_accept_quest(unsigned int index);

int core_game_load(char *filename);
int core_game_save(char *filename);

int core_game_accept_quest(unsigned int index);
int core_game_check_quest_done();
int core_game_check_fuel_gone();
void core_game_check_gas_station();
