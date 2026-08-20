/* gui.h -- window system / status bar API
 *
 * MAP_WND_WIDTH / MAP_WND_HEIGHT are defined in mapwnd.c.
 */
/* ----------------------------------------------------------------
 * Layout constants
 * ---------------------------------------------------------------- */

#define SCREEN_W      640
#define SCREEN_H      480

#define WND_W         320    /* default window width  */
#define WND_H         100    /* default window height */
#define WND_TITLE_H   16     /* title bar height      */
#define WND_DEFAULT_Y 180    /* default window Y      */

#define TOPBAR_H      20     /* top status bar height  */
#define STATUSBAR_H   20     /* bottom status bar height */
#define STATUSBAR_Y   461    /* bottom bar Y = MAP_WND_HEIGHT + 1 */

#define BAR_LEFT       2     /* left margin in bars */
#define BAR_COLOR      4     /* highlight colour    */
#define TEXT_COLOR    15     /* normal text colour  */

#define Q_INPUT_LEN 30

extern int MAP_WND_WIDTH, MAP_WND_HEIGHT;

void  gui_init(void);
void  gui_top_status_line(void);
void  gui_draw_generic_wnd(int x, int y, int width, int height);
void  gui_warning_wnd(char* header, char* text);
char* gui_input_wnd(char* header, char* text, char* defaultValue);
void  gui_progress_wnd(char* header, char* text, int current, int total);
int   gui_bool_wnd(char* header, char* text);