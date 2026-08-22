#define SYSTEM struct system_solar
#define POINT struct point
#define WAYPOINT struct waypoint
#define THREAD struct thread
#define OBJECT struct object
#define NPC struct npc
#define QUEST struct quest
#define GAMESTATE struct game_state
#define BOUND_LINE struct bound_line

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
  int way[30];
  int size;
};

NPC {
  char name[100];
  int faction;
  int portrait;
};

QUEST {
  int reward;
  int penalty;
  int type;  
  int target_system;
  int target_sector;
  int cargo;
  NPC* giver;
};

GAMESTATE {
  char captain_name[100];
  long balance;
  int current_system;
  int ship_type;
  int tonnage;
  int current_cargo;
  long cargo_value;
  int hyper_class;
  int smuggler_bay;
  int reputation;
  int missions_completed;
  int fuel;
  QUEST* quest;
  unsigned char *visited;
  int visited_bytes;
};
