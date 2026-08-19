#define SYSTEM struct system_solar
#define POINT struct point
#define WAYPOINT struct waypoint
#define THREAD struct thread
#define OBJECT struct object

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
  unsigned int fraction;
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

THREAD {
  unsigned int value;
  int cost;
};

WAYPOINT {
  int way[30];
  int size;
};

#define GAMESTATE struct game_state

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
};
