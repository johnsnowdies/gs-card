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

/* Quest System */

void core_game_gen_npc(NPC* npc_ptr, unsigned int faction);
void core_game_gen_quest(QUEST* quest_ptr, int player_rep, unsigned int faction);
void core_game_run_event();
int core_game_accept_quest(unsigned int index);
/*
void core_game_core_game_accept_quest(QUEST* quest);
int core_game_check_quest_done(QUEST* active_quest, SYSTEM* current_system);
*/
/* Shopping System */
