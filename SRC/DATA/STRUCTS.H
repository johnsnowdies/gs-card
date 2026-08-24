#define SYSTEM struct system_solar
#define POINT struct point
#define WAYPOINT struct waypoint
#define THREAD struct thread
#define OBJECT struct object
#define NPC struct npc
#define QUEST struct quest
#define GAMESTATE struct game_state
#define BOUND_LINE struct bound_line

SYSTEM {
  /* POSITIONING */
  int x;
  int y;
  int z;

  /* PATHFINDER DATA */
  THREAD* threads;
  unsigned char threadSize;
  int rate;
  unsigned char visited;

  /* GAME DATA */
  unsigned int faction;
  unsigned int is_shipyard;
  unsigned int is_gas_station;
  unsigned int sector;
};

OBJECT {
  int x;
  int y;
  int z;
  int r;
  int type;
};

POINT {
  int x;
  int y;
};

BOUND_LINE {
    POINT* a;
    POINT* b;
};

THREAD {
  unsigned int value;
  int cost;
};

WAYPOINT {
  int way[25];
  int size;
};

NPC {
  char name[100];
  unsigned int faction;
  unsigned int portrait;
  unsigned int gender;
};

QUEST {
  unsigned int reward;
  unsigned int penalty;
  unsigned int type;  
  unsigned int target_system;
  unsigned int target_sector;
  unsigned int cargo;
  NPC giver;
};

GAMESTATE {
  char captain_name[100];
  long balance;
  int current_system;
  unsigned int ship_type;
  unsigned int tonnage;
  unsigned int current_cargo;
  long cargo_value;
  unsigned int hyper_class;
  unsigned char smuggler_bay;
  unsigned int reputation;
  unsigned int missions_completed;
  unsigned int fuel;
  unsigned int quests_size;
  QUEST quests[5];
  unsigned char *visited;
  int visited_bytes;
};

enum game_screen {
    SCR_MAP,
    SCR_MAIN_MENU,
    SCR_GAME_MENU,
    SCR_STATUS,
    SCR_QUEST_LIST_DETAIL
};
