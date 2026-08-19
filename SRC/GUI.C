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

#include "structs.h"
#include "gui.h"
#include "keys.h"

/* ----------------------------------------------------------------
 * Layout constants
 * ---------------------------------------------------------------- */
#define WND_W         320    /* default window width  */
#define WND_H         100    /* default window height */
#define WND_TITLE_H   16     /* title bar height      */
#define WND_DEFAULT_Y 180    /* default window Y      */

#define TOPBAR_H      20     /* top status bar height  */
#define STATUSBAR_H   20     /* bottom status bar height */
#define STATUSBAR_Y   461    /* bottom bar Y = WND_HEIGHT + 1 */

#define BAR_LEFT       2     /* left margin in bars */
#define BAR_COLOR      4     /* highlight colour    */
#define TEXT_COLOR    15     /* normal text colour  */

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct game_state   gs;
extern struct system_solar* ptrList;

/* ----------------------------------------------------------------
 * init -- BGI graphics init
 * ---------------------------------------------------------------- */
void init()
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
 * topStatusLine -- system name, faction, balance
 * ---------------------------------------------------------------- */
void topStatusLine()
{
    int xpos = BAR_LEFT;
    char buf[100] = "";

    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, WND_WIDTH, TOPBAR_H - 1);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);

    /* TAB-MODE */
    setcolor(BAR_COLOR);
    outtextxy(xpos, 2, "TAB");
    xpos += 25;
    setcolor(TEXT_COLOR);
    outtextxy(xpos, 2, "-MODE");

    xpos += 50;
    
    setcolor(BAR_COLOR);
    outtextxy(xpos, 2, "INFO:");

    xpos += 40;
    sprintf(buf, "SA.%d (%d) | Balance: %ld$$ | Fuel: %d%% | Cargo: %d/%d = %ld$$", gs.current_system, 
        ptrList[gs.current_system].threadSize,
        gs.balance,
        gs.fuel,
        gs.current_cargo,
        gs.tonnage,
        gs.cargo_value);
    
    setcolor(TEXT_COLOR);
    outtextxy(xpos, 2, buf);
  
}

/* ----------------------------------------------------------------
 * statusLine -- bottom shortcut bar + memory usage
 * ---------------------------------------------------------------- */
void statusLine()
{
    unsigned int USED_MEM, FREE_MEM, TOTAL_MEM = 65535;
    int xpos = BAR_LEFT;
    char memMsg[50] = "";

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    FREE_MEM = coreleft();
    USED_MEM = TOTAL_MEM - FREE_MEM;
    sprintf(memMsg, "%u/%u", USED_MEM, TOTAL_MEM);

    setfillstyle(SOLID_FILL, BLACK);
    bar(0, STATUSBAR_Y, WND_WIDTH, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Shortcut labels */
    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F1"); xpos += 13;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-VIEW"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F2");
    setcolor(TEXT_COLOR); xpos += 15;
    outtextxy(xpos, STATUSBAR_Y + 2, "-AXIS"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F3/F4"); xpos += 40;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-ZOOM"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F5"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-GOTO"); xpos += 45;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F6"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-THREADS"); xpos += 70;

    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F7"); xpos += 15;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-RUN"); xpos += 40;

    /* Separator */
    xpos = 470;
    setcolor(BAR_COLOR);
    line(xpos, STATUSBAR_Y - 1, xpos, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Memory */
    setcolor(BAR_COLOR);
    outtextxy(xpos + 5, STATUSBAR_Y + 2, "MEM:");
    setcolor(TEXT_COLOR);
    outtextxy(xpos + 35, STATUSBAR_Y + 2, memMsg);
}

/* ----------------------------------------------------------------
 * drawWnd -- generic window frame
 * ---------------------------------------------------------------- */
void drawWnd(int x, int y, int width, int height)
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
void warningWnd(char* header, char* text)
{
    int wx = (WND_WIDTH - WND_W) / 2;
    int wy = WND_DEFAULT_Y;

    drawWnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, header);
    setcolor(BAR_COLOR);
    outtextxy(wx + 2, wy + 20, text);
    setcolor(TEXT_COLOR);
}

/* ----------------------------------------------------------------
 * questionWnd -- text input dialog
 *
 * Returns a pointer to a static buffer (valid until next call).
 * ---------------------------------------------------------------- */
char* questionWnd(char* header, char* text, char* defaultValue)
{
    static char inputbuf[Q_INPUT_LEN];
    int i = 0, c = 0, input_pos = 0, the_end = 0;
    int wx = (WND_WIDTH - WND_W) / 2;
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

    drawWnd(wx, wy, WND_W, WND_H);

    setcolor(0);
    outtextxy(wx + 2, wy + 5, header);
    setcolor(BAR_COLOR);
    outtextxy(wx + 2, wy + 20, text);
    outtextxy(wx + 2, wy + 88, "Press [ENTER] to confirm [ESC] to abort");

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
void progressWnd(char* header, char* text, int current, int total)
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
        int wx = (WND_WIDTH - WND_W) / 2;
        drawWnd(wx, WND_DEFAULT_Y, WND_W, WND_H);

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