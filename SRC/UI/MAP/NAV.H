/* nav.h -- navigation controller API */

void scaleMinus(void);
void scalePlus(void);
void offsetXplus(void);
void offsetXminus(void);
void offsetYplus(void);
void offsetYminus(void);
void offsetZplus(void);
void offsetZminus(void);
void moveScreenTo(struct system_solar* solar, int value);
void gotoSystem(int ptrSize, struct system_solar* solar);