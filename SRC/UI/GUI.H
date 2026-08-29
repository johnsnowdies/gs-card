/* ----------------------------------------------------------------
 * Layout constants
 * ---------------------------------------------------------------- */

/* VGA Screen */
#define SCREEN_W 640
#define SCREEN_H 480

/* Status Bar constants */
#define STATUSBAR_WIDTH 640
#define STATUSBAR_HEIGHT 20
#define STATUSBAR_BOTTOM_Y 461

/* Modal windows width */
#define WND_MODAL_DEFAULT_WIDTH 320
#define WND_MODAL_DEFAULT_HEIGHT 100
#define WND_DIALOG_DEFAULT_HEIGHT 220

/* Window header constants*/
#define WND_HEADER_FONT SMALL_FONT, HORIZ_DIR, 4
#define WND_HEADER_HEIGHT 16
#define WND_HEADER_X_OFFSET 2
#define WND_HEADER_Y_OFFSET 5

/* Input Field Length*/
#define Q_INPUT_LEN 30

/* ----------------------------------------------------------------
 * GUI Elements
 * ---------------------------------------------------------------- */

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
  char* text;
  int x;
  int y;
  int width;
  int height;
  unsigned char selected;
  unsigned char enabled;
  unsigned char visible;
};

/* ----------------------------------------------------------------
 * GUI API
 * ---------------------------------------------------------------- */

void gui_init(void);

/* Window prototype - size, header */
void gui_draw_wnd_proto(WND* ptr_wnd);

/* Element: Button */
void gui_draw_btn(BTN* btn);
int gui_handle_btn_keys(int num_btns, int* current);

/* Element: Input Field */
void gui_draw_input_field(INPUT_FIELD* field);
void gui_run_input_field(INPUT_FIELD* field);

/* Element: Progress Bar */
void gui_init_progress_bar(PROGRESS_BAR* pb, int x, int y, int width,
                           int height, int total, int border_color,
                           int fill_color, int bg_color);
void gui_draw_progress_bar(PROGRESS_BAR* pb);

/* Modal: simple centred warning dialog */
void gui_warning_wnd(WND* ptr_parent, char* header, char* text, int play_sound);

/* Modal: yes/no dialog */
int gui_confirm_wnd(WND* ptr_parent, char* header, char* text);

/* Modal: input dialog */
char* gui_input_wnd(WND* ptr_parent, char* header, char* text,
                    char* defaultValue);

/* Modal: progress bar dialog */
void gui_progress_wnd(WND* ptr_parent, char* header, char* text, int current,
                      int total);

/* Multiline dialog window with portrait */
int gui_dialog_wnd(WND* parent, char* header, char* title, char* photo_file,
                   char lines[][100], int lines_count, char buttons[][100],
                   int buttons_count, int play_sound);

/* Universal status line */
void gui_draw_status_line(WND* ptr_wnd, char* keys[], char* items[], int highlight);

/* Memory status (fixed) */
void gui_memory_status(void);

/* Draw section header: red bar, title, pattern fill and transition triangle.
   x, y     - top-left corner of the window (y is baseline for 30px high bar)
   width    - full width of the bar
   title    - header text (ASCII)
*/
void gui_draw_section_header(int x, int y, int width, char *title);
void gui_draw_section_header_v(int x, int y, int width, int height, char *title);