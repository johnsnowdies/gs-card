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
	THREAD *threads;
	unsigned char threadSize;
	int rate;
	unsigned char visited;
};

OBJECT {
	int x;
	int y;
	int z;
	int r;
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
