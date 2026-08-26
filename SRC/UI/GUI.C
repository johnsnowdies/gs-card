/* gui.c -- window system, status bars, input dialogs
 *
 * Layout constants (WND_*, BAR_*) replace magic numbers.
 * All coordinates inside a window are expressed relative to
 * the window origin (wx, wy).
 */

#include <alloc.h>
#include <graphics.h>
#include <stdio.h>
#include <string.h>

#include "data\structs.h"
#include "data\keys.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "music.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern GAME_STATE gs;
extern SYSTEM* sol_list;

/* ----------------------------------------------------------------
 * init -- BGI graphics init
 * ---------------------------------------------------------------- */
void gui_init()
{
    int gd = DETECT, gm, result;
    char* msg;

    initgraph(&gd, &gm, "BGI");
    result = graphresult();
    if (result) {
        msg = grapherrormsg(result);
        printf("%s", msg);
    }

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
}

/* ----------------------------------------------------------------
 * Window prototype - size, header
 * ---------------------------------------------------------------- */

void gui_draw_wnd_proto(WND* ptr_wnd)
{    
    int width = ptr_wnd->x + ptr_wnd->width,
        height = ptr_wnd->y + ptr_wnd->height;

    setlinestyle(0, 0, 1);
    
    /* Fill window space with BLACK */
    setfillstyle(SOLID_FILL, BLACK);
    bar(ptr_wnd->x, ptr_wnd->y, width, height);

    /* Window RED border */
    setcolor(RED);
    rectangle(ptr_wnd->x, ptr_wnd->y, width, height);

    /* Title bar */
    if (ptr_wnd->header){
        setfillstyle(SOLID_FILL, RED);
        bar(ptr_wnd->x, ptr_wnd->y, width, ptr_wnd->y + WND_HEADER_HEIGHT);
        settextstyle(WND_HEADER_FONT);
        setcolor(BLACK);
        outtextxy(ptr_wnd->x + WND_HEADER_X_OFFSET, ptr_wnd->y + WND_HEADER_Y_OFFSET, ptr_wnd->header);
        setcolor(RED);   
    }
}
/* ----------------------------------------------------------------
 * Element: Button
 * ---------------------------------------------------------------- */

void gui_draw_btn(BTN *btn)
{
    if (!btn->visible) return;
    setlinestyle(0, 0, 1);
    if (btn->enabled) {
        if (btn->selected) {
            setfillstyle(SOLID_FILL, RED);
            setcolor(RED);
            bar(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
            rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
            setcolor(BLACK);
        } else {
            setcolor(RED);
            setfillstyle(SOLID_FILL, BLACK);
            bar(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
            rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
            setcolor(RED);
        }
        outtextxy(btn->x + (btn->width - textwidth(btn->text)) / 2,
                  btn->y + (btn->height - textheight(btn->text)) / 2,
                  btn->text);
    } else {
        setcolor(DARKGRAY);
        rectangle(btn->x, btn->y, btn->x + btn->width, btn->y + btn->height);
        setcolor(DARKGRAY);
        outtextxy(btn->x + (btn->width - textwidth(btn->text)) / 2,
                  btn->y + (btn->height - textheight(btn->text)) / 2,
                  btn->text);
    }
}

int gui_handle_btn_keys(int num_btns, int *current)
{
    int ch, ext;
    ch = getch();
    if (ch == 0) {
        ext = getch();
        if (ext == LFT || ext == UP) {
            (*current)--;
            if (*current < 0) *current = num_btns - 1;
            return 0;
        }
        if (ext == RHT || ext == DWN) {
            (*current)++;
            if (*current >= num_btns) *current = 0;
            return 0;
        }
    } else if (ch == ENTER) {
        return 1;
    } else if (ch == ESC) {
        return -1;
    }
    return 0;
}

/* ----------------------------------------------------------------
 * Element: Input Field
 * ---------------------------------------------------------------- */
void gui_draw_input_field(INPUT_FIELD *field)
{
    if (!field->visible) return;

    setcolor(field->border_color);
    rectangle(field->x, field->y, field->x + field->width, field->y + field->height);

    if (field->fill_color >= 0) {
        setfillstyle(SOLID_FILL, field->fill_color);
        bar(field->x + 1, field->y + 1, field->x + field->width - 1, field->y + field->height - 1);
    }

    setcolor(field->text_color);
    outtextxy(field->x + 5, field->y + (field->height - textheight("A")) / 2, field->buffer);
}

static int gui_handle_input_key(INPUT_FIELD *field, int ch)
{
    int len = strlen(field->buffer);

    if (ch == 8) {
        if (len > 0) {
            field->buffer[len - 1] = '\0';
        }
        return 0;
    }
    if (ch == ENTER) {
        return 1;
    }
    if (ch == ESC) {
        field->buffer[0] = '\0';
        return 1;
    }
    if (ch >= ' ' && ch <= '~') {
        if (len < field->max_len - 1) {
            field->buffer[len] = (char)ch;
            field->buffer[len + 1] = '\0';
        }
        return 0;
    }
    return 0;
}

void gui_run_input_field(INPUT_FIELD *field)
{
    int done = 0;
    while (!done) {
        gui_draw_input_field(field);
        {
            int ch = getch();
            done = gui_handle_input_key(field, ch);
        }
    }
}

/* ----------------------------------------------------------------
 * Element: Progress Bar
 * ---------------------------------------------------------------- */
void gui_init_progress_bar(PROGRESS_BAR *pb, int x, int y, int width, int height,
                           int total, int border_color, int fill_color, int bg_color)
{
    pb->x = x;
    pb->y = y;
    pb->width = width;
    pb->height = height;
    pb->current = 0;
    pb->total = total;
    pb->border_color = border_color;
    pb->fill_color = fill_color;
    pb->bg_color = bg_color;
    pb->visible = 1;
}

void gui_draw_progress_bar(PROGRESS_BAR *pb)
{
    int fill_width;

    if (!pb->visible) return;
    if (pb->total <= 0) return;

    setcolor(pb->border_color);
    rectangle(pb->x, pb->y, pb->x + pb->width, pb->y + pb->height);

    if (pb->bg_color >= 0) {
        setfillstyle(SOLID_FILL, pb->bg_color);
        bar(pb->x + 1, pb->y + 1, pb->x + pb->width - 1, pb->y + pb->height - 1);
    }

    fill_width = (int)((long)pb->current * pb->width / pb->total);
    if (fill_width > 0) {
        setfillstyle(SOLID_FILL, pb->fill_color);
        bar(pb->x + 1, pb->y + 1, pb->x + fill_width, pb->y + pb->height - 1);
    }
}

void gui_set_progress(PROGRESS_BAR *pb, int current)
{
    if (current < 0) current = 0;
    if (current > pb->total) current = pb->total;
    pb->current = current;
}

/* ----------------------------------------------------------------
 * Modal: simple centred warning dialog
 * ---------------------------------------------------------------- */
void gui_warning_wnd(WND* ptr_parent, char* header, char* text, int play_sound)
{
    WND warning_wnd;
    
    warning_wnd.header = header;
    /* TODO: remove map dependency!*/
    warning_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    warning_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;;
    warning_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    warning_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    gui_draw_wnd_proto(&warning_wnd);

    setcolor(RED);
    outtextxy(warning_wnd.x + 2, warning_wnd.y + 20, text);

    switch(play_sound){
    case NO_SOUND:
        break;
    case SOUND_WARNING:
        sfx_modal();
        break;
    case SOUND_ERROR:
        sfx_error();
        break;
    case SOUND_SUCCESS:
        sfx_hyperjump();
        break;
    }
}

/* ----------------------------------------------------------------
 * Modal: yes/no dialog
 * ---------------------------------------------------------------- */
int gui_confirm_wnd(WND* ptr_parent, char* header, char* text)
{
    WND confirm_wnd;
    BTN btn_yes, btn_no;
    int sound_played = 0;
    int selected = 0;   

    int btn_width = 50, btn_height = 20, btn_gap = 10;
    int btn_y;
    int total = 2 * btn_width + btn_gap;

    confirm_wnd.header = header;
    confirm_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    confirm_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;
    confirm_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    confirm_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    btn_y = confirm_wnd.y  + WND_MODAL_DEFAULT_HEIGHT - btn_height - 10;

    btn_yes.text = LC_GUI_BOOL_YES;
    btn_yes.x = confirm_wnd.x + (WND_MODAL_DEFAULT_WIDTH - total) / 2;
    btn_yes.y = btn_y;
    btn_yes.width = btn_width;
    btn_yes.height = btn_height;
    btn_yes.selected = (selected == 0) ? 1 : 0;
    btn_yes.enabled = 1;
    btn_yes.visible = 1;

    btn_no.text = LC_GUI_BOOL_NO;
    btn_no.x = btn_yes.x + btn_yes.width + btn_gap;
    btn_no.y = btn_y;
    btn_no.width = btn_width;
    btn_no.height = btn_height;
    btn_no.selected = (selected == 1) ? 1 : 0;
    btn_no.enabled = 1;
    btn_no.visible = 1;

    while (1) {
        if (selected == 0)
        {
            btn_yes.selected = 1;
            btn_no.selected = 0;
        }

        if (selected == 1)
        {
            btn_yes.selected = 0;
            btn_no.selected = 1;
        }

        gui_draw_wnd_proto(&confirm_wnd);
        outtextxy(confirm_wnd.x + 5, confirm_wnd.y + 20, text);

        gui_draw_btn(&btn_yes);
        gui_draw_btn(&btn_no);

        if (sound_played == 0){
            sfx_modal();
            sound_played = 1;
        }

        if (gui_handle_btn_keys(2, &selected) == 1){
            return selected;
        }
    }


}

/* ----------------------------------------------------------------
 * Modal: input dialog
 * ---------------------------------------------------------------- */
char* gui_input_wnd(WND* ptr_parent, char* header, char* text, char* defaultValue)
{
    WND input_wnd;
    INPUT_FIELD field;
    char* result;

    input_wnd.header = header;
    input_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    input_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;;
    input_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    input_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    field.max_len = Q_INPUT_LEN;
    field.x = input_wnd.x + 20;
    field.y = input_wnd.y + 50;
    field.width = 280;
    field.height = 18;
    field.border_color = RED;
    field.text_color = BLACK;
    field.fill_color = RED;
    field.active = 1;
    field.visible = 1;

    if (defaultValue != NULL && defaultValue[0] != '\0') {
        strncpy(field.buffer, defaultValue, Q_INPUT_LEN - 1);
        field.buffer[Q_INPUT_LEN - 1] = '\0';
    } else {
        field.buffer[0] = '\0';
    }
    field.cursor_pos = strlen(field.buffer);

    gui_draw_wnd_proto(&input_wnd);
    
    setcolor(RED);
    outtextxy(input_wnd.x + 2, input_wnd.y + 20, text);
    outtextxy(input_wnd.x + 2, input_wnd.y + 88, LC_GUI_INPUT_TEXT);

    sfx_modal();
    gui_run_input_field(&field);
    result = (char*)malloc(Q_INPUT_LEN);

    strcpy(result, field.buffer);
    return result;

}

/* ----------------------------------------------------------------
 * Modal: progress bar dialog
 * ---------------------------------------------------------------- */

void gui_progress_wnd(WND* ptr_parent, char* header, char* text, int current, int total)
{
    WND progress_wnd;
    PROGRESS_BAR pb;

    progress_wnd.header = header;
    progress_wnd.x = (ptr_parent->width - WND_MODAL_DEFAULT_WIDTH) / 2;
    progress_wnd.y = (ptr_parent->height - WND_MODAL_DEFAULT_HEIGHT) / 2;
    progress_wnd.width = WND_MODAL_DEFAULT_WIDTH;
    progress_wnd.height = WND_MODAL_DEFAULT_HEIGHT;

    gui_init_progress_bar(&pb,
                          progress_wnd.x + 20, progress_wnd.y + 50,
                          260, 18,
                          total,
                          RED, RED, BLACK);

    if (current == 0) {
        gui_draw_wnd_proto(&progress_wnd);
        setcolor(RED);
        outtextxy(progress_wnd.x + 2, progress_wnd.y + 20, text);
        setcolor(WHITE);
    }

    gui_set_progress(&pb, current);
    gui_draw_progress_bar(&pb);
}

/* ----------------------------------------------------------------
 * Multiline image window
 * ---------------------------------------------------------------- */
void gui_image_multiline_wnd(WND* ptr_parent, char* image, char* header, char lines[][100], int lines_count, int width, int height, int play_sound)
{
    WND image_wnd;
    int i;

    image_wnd.header = header;
    image_wnd.x = (ptr_parent->width - width) / 2;
    image_wnd.y = (ptr_parent->height - height + 10 + 15 * lines_count) / 2;
    image_wnd.width = width+1;
    image_wnd.height = height + 15 * lines_count + WND_HEADER_HEIGHT;

    gui_draw_wnd_proto(&image_wnd);

    data_reader_draw_bmp(image, image_wnd.x+1, image_wnd.y + WND_HEADER_HEIGHT);

    switch(play_sound){
        case NO_SOUND:
            break;
        case SOUND_WARNING:
            sfx_modal();
            break;
        case SOUND_ERROR:
            sfx_error();
            break;
        case SOUND_SUCCESS:
            sfx_hyperjump();
            break;
    }

    for (i = 0; i < lines_count; i++){
        outtextxy(image_wnd.x + 10, image_wnd.y + height + 10 + (i*15), lines[i]);
    }
}

/* ----------------------------------------------------------------
 * Universal status line
 * ---------------------------------------------------------------- */
void gui_draw_status_line(WND* ptr_wnd, char* keys[], char* items[])
{
    int xpos = ptr_wnd->x + 5;
    int width = ptr_wnd->x + ptr_wnd->width,
        height = ptr_wnd->y + ptr_wnd->height;
    int i = 0;

    setfillstyle(SOLID_FILL, RED);
    bar(ptr_wnd->x, ptr_wnd->y, width, height);

    while (keys[i] != NULL && items[i] != NULL) {
        setcolor(BLACK);
        outtextxy(xpos, ptr_wnd->y + 2, keys[i]);
        xpos += textwidth(keys[i]) + 4;

        setcolor(WHITE);
        outtextxy(xpos, ptr_wnd->y + 2, items[i]);
        xpos += textwidth(items[i]) + 5;

        i++;
    }
}

/* ----------------------------------------------------------------
 * Memory status (fixed)
 * ---------------------------------------------------------------- */
void gui_memory_status()
{
    unsigned int USED_MEM, FREE_MEM, TOTAL_MEM = 65535;
    char memMsg[50] = "";
    
    FREE_MEM = coreleft();
    USED_MEM = TOTAL_MEM - FREE_MEM;
    sprintf(memMsg, "%u/%u", USED_MEM, TOTAL_MEM);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setfillstyle(SOLID_FILL, BLACK);
    bar(470, STATUSBAR_BOTTOM_Y, 640, STATUSBAR_BOTTOM_Y + STATUSBAR_HEIGHT - 1);

    setcolor(RED);
    line(470, STATUSBAR_BOTTOM_Y - 1, 470, STATUSBAR_BOTTOM_Y + STATUSBAR_HEIGHT - 1);

    /* Memory */
    setcolor(RED);
    outtextxy(470 + 5, STATUSBAR_BOTTOM_Y + 2, LC_MAP_STATUS_MEM);
    setcolor(WHITE);
    outtextxy(470 + 35, STATUSBAR_BOTTOM_Y + 2, memMsg);
}


void gui_game_over(WND* parent)
{
    
}
