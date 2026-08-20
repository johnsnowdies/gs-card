/* mapwnd.h -- map window rendering API */
/* Only draw() and clearWnd() are public.  All coordinate helpers
   and projection functions are static inside mapwnd.c. */

void draw(int ptrSize, struct system_solar* ptrList, int mode,
          int isCoord, int isHyper, struct waypoint* way, int currentPoint);

void clearWnd(void);