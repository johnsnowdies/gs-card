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


/* change with 1/2 calculation */
#define WND_MODAL_DEFAULT_Y 180
#define WND_MODAL_DEFAULT_WIDTH 320
#define WND_MODAL_DEFAULT_HEIGHT 100


#define WND_BORDER_COLOR      4     /* highlight colour    */
#define WND_FILL_COLOR 0

#define WND_HEADER_FONT SMALL_FONT, HORIZ_DIR, 4
#define WND_HEADER_HEIGHT 16
#define WND_HEADER_X_OFFSET 2
#define WND_HEADER_Y_OFFSET 5



#define TEXT_COLOR    15     /* normal text colour  */

#define Q_INPUT_LEN 30


#define INPUT_FIELD struct input_field
INPUT_FIELD {
    char buffer[Q_INPUT_LEN];
    int max_len;
    int cursor_pos;
    int x, y, width, height;
    int border_color;
    int text_color;
    int fill_color;
    unsigned char active;
    unsigned char visible;
};

#define PROGRESS_BAR struct progress_bar
PROGRESS_BAR {
    int x, y, width, height;
    int current;
    int total;
    int border_color;
    int fill_color;
    int bg_color;
    unsigned char visible;
};

#define WND struct gui_proto_window
WND {
    char* header;
    int x;
    int y;
    int width;
    int height;
};

#define BTN struct gui_proto_button
BTN {
    char *text;
    int x;
    int y;
    int width;
    int height;
    unsigned char selected;
    unsigned char enabled;
    unsigned char visible;
};


extern int MAP_WND_WIDTH, MAP_WND_HEIGHT;

void gui_init(void);

/* Window prototype - size, header */
void gui_draw_wnd_proto(WND* ptr_wnd);

/* Element: Button */
void gui_draw_btn(BTN *btn);
int gui_handle_btn_keys(int num_btns, int *current);

/* Element: Input Field */
void gui_draw_input_field(INPUT_FIELD *field);
void gui_run_input_field(INPUT_FIELD *field);

/* Element: Progress Bar */
void gui_init_progress_bar(PROGRESS_BAR *pb, int x, int y, int width, int height,
                           int total, int border_color, int fill_color, int bg_color);
void gui_draw_progress_bar(PROGRESS_BAR *pb);

/* Modal: simple centred warning dialog */
void  gui_warning_wnd(char* header, char* text);

/* Modal: yes/no dialog */
int gui_confirm_wnd(char* header, char* text);

/* Modal: input dialog */
char* gui_input_wnd(char* header, char* text, char* defaultValue);

/* Modal: progress bar dialog */
void  gui_progress_wnd(char* header, char* text, int current, int total);

/* Universal status line */
void gui_draw_status_line(WND* ptr_wnd, char* keys[], char* items[]);

/* Memory status (fixed) */
void gui_memory_status(void);
