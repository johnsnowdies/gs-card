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
  unsigned int reward;
  unsigned int penalty;
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
  unsigned char *visited;
  int visited_bytes;
};

#define E_GAME_SCREEN enum game_screen
E_GAME_SCREEN {
    SCR_MAP,
    SCR_MAIN_MENU,
    SCR_GAME_MENU,
    SCR_STATUS,
    SCR_UPGRADE,
    SCR_SHIPYARD
};


#define UPGRADE struct upgrade
UPGRADE {
    char *name;
    int base_price;
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
