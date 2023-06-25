void draw3dwnd(int ptrSize, SYSTEM *solar, int isCoord, int isHyper, WAYPOINT *wp);
void draw2dwnd(int ptrSize, SYSTEM *solar, int isCoord, int isHyper, WAYPOINT *wp);
void draw(int ptrSize, SYSTEM *ptrList, int mode, int isCoord, int isHyper, WAYPOINT *wp, int currentPoint);
void statusLine();
void init();
void scaleMinus();
void scalePlus();
void clearWnd();

void moveScreenTo(SYSTEM * solar, int value);
void gotoSystem(int ptrSize, SYSTEM *solar);

void offsetXplus();
void offsetXminus();
void offsetYpuls();
void offsetYminus();
void offsetZplus();
void offsetZminus();


void spin(float *a, float *b, int f);
void spinX();
void spinY();
void spinZ();

void drawPathWnd(WAYPOINT *wp, int oy);
void pathWnd(WAYPOINT *wp, int currentPoint,SYSTEM *ptrList);
char* questionWnd(char *header, char *text);
void warningWnd(char *header, char *text);
void progressWnd(char *header, char *text, int current, int total);
void drawWnd(int x, int y, int width, int height);


