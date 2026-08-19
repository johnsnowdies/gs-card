/* gui.h -- window system / status bar API
 *
 * WND_WIDTH / WND_HEIGHT are defined in mapwnd.c.
 */
#define Q_INPUT_LEN 30

extern int WND_WIDTH, WND_HEIGHT;

void init(void);
void topStatusLine(void);
void statusLine(void);
void drawWnd(int x, int y, int width, int height);
void warningWnd(char* header, char* text);
char* questionWnd(char* header, char* text, char* defaultValue);
void progressWnd(char* header, char* text, int current, int total);
