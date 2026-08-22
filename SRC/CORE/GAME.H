/* game.c - core gameplay logics API */

/* Quest System */

void core_game_gen_npc(NPC* npc_ptr, int faction);
void core_game_gen_quest(QUEST* quest_ptr, int player_rep, SYSTEM* current_system);
/*
void core_game_accept_quest(QUEST* quest);
int core_game_check_quest_done(QUEST* active_quest, SYSTEM* current_system);
*/
/* Shopping System */
