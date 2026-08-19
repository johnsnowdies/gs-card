/* game.c - core gameplay logics API */

/* Quest System */
NPC* generateNPC(int faction);
QUEST* generateQuest(int playerRep, SYSTEM* currentSystem);
bool checkQuestDone(QUEST* activeQuest, SYSTEM* currentSystem);

/* Shopping System */
