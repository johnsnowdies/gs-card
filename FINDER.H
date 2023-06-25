void RunWave(int ptrSize, SYSTEM * ptrList, int c, int end);
int RestorePath(int ptrSize, SYSTEM * ptrList, WAYPOINT *wp, int start, int end,int reverse);
int GetWay(int ptrSize, SYSTEM * ptrList, WAYPOINT *wp);
void CalculateHyperThreads(int ptrSize, SYSTEM * ptrList);
void CalculateThreadCosts(int ptrSize, SYSTEM * ptrList,int topCost);
void ClearThreadCosts(int ptrSize, SYSTEM * ptrList);