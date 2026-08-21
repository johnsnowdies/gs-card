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


/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct game_state   gs;
extern struct system_solar* sol_list;

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

int count_digits_loop(long n) {
    int count = 0;
    if (n == 0) return 1;
    if (n < 0) n = -n;

    while (n) { ++count; n /= 10; }
    return count;
}

/* ----------------------------------------------------------------
 * gui_top_status_line -- system name, faction, balance
 * ---------------------------------------------------------------- */
void gui_top_status_line()
{
    int xpos = BAR_LEFT;
    char buf[100] = "";

    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, MAP_WND_WIDTH, TOPBAR_H - 1);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);

    /* TAB-MODE */
    setcolor(BAR_COLOR);
    outtextxy(xpos, 2, "TAB");
    xpos += 25;
    setcolor(TEXT_COLOR);
    outtextxy(xpos, 2, LC_GUI_STATUS_MODE);

    xpos += 55;
    
    setcolor(BAR_COLOR);
    outtextxy(xpos, 2, LC_GUI_STATUS_INFO);

    xpos += 40;
    sprintf(buf, "SA.%d (%d) | %s: %ld$$ | %s: %d%% | %s: %d/%d = %ld$$",
        gs.current_system,
        sol_list[gs.current_system].threadSize,
        LC_GUI_STATUS_BALANCE, gs.balance,
        LC_GUI_STATUS_FUEL, gs.fuel,
        LC_GUI_STATUS_CARGO, gs.current_cargo, gs.tonnage, gs.cargo_value);
    
    setcolor(TEXT_COLOR);
    outtextxy(xpos, 2, buf);
  
}


/* ----------------------------------------------------------------
 * drawWnd -- generic window frame
 * ---------------------------------------------------------------- */
void gui_draw_generic_wnd(int x, int y, int width, int height)
{
    setfillstyle(SOLID_FILL, BLACK);
    setcolor(BAR_COLOR);

    /* Body */
    bar(x, y, x + width, y + height);
    rectangle(x, y, x + width, y + height);

    /* Title bar */
    setfillstyle(SOLID_FILL, RED);
    bar(x, y, x + width, y + WND_TITLE_H);

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
}

/* ----------------------------------------------------------------
 * warningWnd -- simple centred warning dialog
 * ---------------------------------------------------------------- */
void gui_warning_wnd(char* header, char* text)
{
    int wx = (MAP_WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;

    gui_draw_generic_wnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, header);
    setcolor(BAR_COLOR);
    outtextxy(wx + 2, wy + 20, text);
    setcolor(TEXT_COLOR);
}

int gui_bool_wnd(char* header, char* text)
{
    int wx = (MAP_WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;
    int selected = 0;         
    int ch, ext;

    int btnW = 50, btnH = 20, gap = 10;
    int btnY = wy + WND_H - btnH - 10;           
    int total = 2 * btnW + gap;
    int btnX1 = wx + (WND_W - total) / 2;        
    int btnX2 = btnX1 + btnW + gap;   

    setlinestyle(0, 0, 1);          

    while (1) {
        gui_draw_generic_wnd(wx, wy, WND_W, WND_H);

        setcolor(0);                             
        outtextxy(wx + 2, wy + 5, header);
        setcolor(BAR_COLOR);                
        outtextxy(wx + 2, wy + 20, text);
        setcolor(TEXT_COLOR);

        if (selected == 0) {
            setfillstyle(SOLID_FILL, RED);
            setcolor(RED);
            bar(btnX1, btnY, btnX1 + btnW, btnY + btnH);
            rectangle(btnX1, btnY, btnX1 + btnW, btnY + btnH);
            setcolor(BLACK);
            outtextxy(btnX1 + (btnW - textwidth(LC_GUI_BOOL_YES)) / 2,
                      btnY + (btnH - textheight(LC_GUI_BOOL_YES)) / 2, LC_GUI_BOOL_YES);
        } else {
            
            setcolor(RED);
            rectangle(btnX1, btnY, btnX1 + btnW, btnY + btnH);
            setcolor(RED);
            outtextxy(btnX1 + (btnW - textwidth(LC_GUI_BOOL_YES)) / 2,
                      btnY + (btnH - textheight(LC_GUI_BOOL_YES)) / 2, LC_GUI_BOOL_YES);
        }

        if (selected == 1) {
            setfillstyle(SOLID_FILL, RED);
            setcolor(RED);
            bar(btnX2, btnY, btnX2 + btnW, btnY + btnH);
            rectangle(btnX2, btnY, btnX2 + btnW, btnY + btnH);
            setcolor(BLACK);
            outtextxy(btnX2 + (btnW - textwidth(LC_GUI_BOOL_NO)) / 2,
                      btnY + (btnH - textheight(LC_GUI_BOOL_NO)) / 2, LC_GUI_BOOL_NO);
        } else {
            setcolor(RED);
            rectangle(btnX2, btnY, btnX2 + btnW, btnY + btnH);
            setcolor(RED);
            outtextxy(btnX2 + (btnW - textwidth(LC_GUI_BOOL_NO)) / 2,
                      btnY + (btnH - textheight(LC_GUI_BOOL_NO)) / 2, LC_GUI_BOOL_NO);
        }

        ch = getch();
        if (ch == 0) {             
            ext = getch();
            if (ext == LFT) {
                if (selected == 1) selected = 0;  
            } else if (ext == RHT) {
                if (selected == 0) selected = 1; 
            }
        } else if (ch == ENTER) {
            return (selected == 0) ? 1 : 0;     
        }
    }
}

/* ----------------------------------------------------------------
 * questionWnd -- text input dialog
 *
 * Returns a pointer to a static buffer (valid until next call).
 * ---------------------------------------------------------------- */
char* gui_input_wnd(char* header, char* text, char* defaultValue)
{
    static char inputbuf[Q_INPUT_LEN];
    int i = 0, c = 0, input_pos = 0, the_end = 0;
    int wx = (MAP_WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;

    if (defaultValue != NULL && defaultValue[0] != '\0')
    {
        for (i = 0; i < Q_INPUT_LEN - 1 && defaultValue[i] != '\0'; i++)
        {
            inputbuf[i] = defaultValue[i];
        }
        inputbuf[i] = '\0';
    }
    else
    {
        inputbuf[0] = '\0';
    }

    gui_draw_generic_wnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, header);
    setcolor(BAR_COLOR);
    outtextxy(wx + 2, wy + 20, text);
    outtextxy(wx + 2, wy + 88, LC_GUI_INPUT_TEXT);

    moveto(wx + 5, wy + 40);
    setcolor(TEXT_COLOR);

    do {
        /* Redraw input field */
        setfillstyle(SOLID_FILL, RED);
        bar(wx + 20, wy + 50, wx + 300, wy + 68);
        setcolor(0);
        outtextxy(wx + 25, wy + 55, inputbuf);

        c = getch();
        switch (c) {
            case 8:                     /* BACKSPACE */
                if (input_pos) {
                    input_pos--;
                    inputbuf[input_pos] = '\0';
                }
                break;
            case 13:                    /* RETURN */
                the_end = 1;
                break;
            case ESC:                   /* ESC */
                inputbuf[0] = '\0';
                the_end = 1;
                break;
            default:
                if (input_pos < Q_INPUT_LEN - 1 && c >= ' ' && c <= '~') {
                    inputbuf[input_pos] = c;
                    input_pos++;
                    inputbuf[input_pos] = '\0';
                }
        }
    } while (!the_end);

    return inputbuf;
#undef Q_INPUT_LEN
}

/* ----------------------------------------------------------------
 * progressWnd -- progress bar dialog
 * ---------------------------------------------------------------- */
void gui_progress_wnd(char* header, char* text, int current, int total)
{
    float di, x;
    unsigned int MEM;
    char memMsg[50] = "";

    MEM = coreleft();
    sprintf(memMsg, "M: %d", MEM);

    if (total == 0) return;
    di = (float)current / total;
    if (di < 0) di = -di;
    x = 280.0F * di + 180.0F;

    if (current == 0) {
        int wx = (MAP_WND_WIDTH - WND_W) / 2;
        gui_draw_generic_wnd(wx, WND_DEFAULT_Y, WND_W, WND_H);

        setcolor(0);
        outtextxy(wx + 2, WND_DEFAULT_Y + 5, header);
        setcolor(BAR_COLOR);
        outtextxy(wx + 2, WND_DEFAULT_Y + 20, text);
        setcolor(TEXT_COLOR);

        setcolor(BAR_COLOR);
        rectangle(180, 230, 460, 248);
    }

    setfillstyle(SOLID_FILL, RED);
    bar(400, 185, 475, 195);
    bar(180, 230, (int)x, 248);
    outtextxy(400, 185, memMsg);

    setcolor(0);
}
