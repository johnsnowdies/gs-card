#include <graphics.h>
#include <stdio.h>

#include "structs.h"
#include "gui.h"
#include "reader.h"

/* ----------------------------------------------------------------
 * Screen / viewport layout -- defined here, declared extern in gui.h
 * ---------------------------------------------------------------- */
int STATUS_WND_WIDTH  = 639;   /* may shrink to 470 when path panel is open */
int STATUS_WND_HEIGHT = 460;

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct game_state gs;
extern struct system_solar* ptrList;
extern int      render_danger_objects;
extern int      show_danger_hyperthreads;
extern int      show_danger_path_parts;
extern char* factions[4];
extern char* sectors[9];

/* Viewport bounds for clipping (status window only) */
static struct status_wnd {
    int x1, y1, x2, y2;
} status_wnd = { 0, 21, 639, 460 };


/*
    strcpy(gs.captain_name, name);
    gs.balance            = 1000;
    gs.current_system     = rand() % ptrSize;
    if (gs.current_system < 0) gs.current_system = 0;
    if (gs.current_system >= ptrSize) gs.current_system = 0;

    gs.ship_type         = 0;
    gs.tonnage           = 50;
    gs.current_cargo     = 0;
    gs.cargo_value       = 0;
    gs.hyper_class       = 0;
    gs.smuggler_bay      = 0;
    gs.reputation        = 0;
    gs.missions_completed = 0;
    gs.fuel              = 100;
*/

void drawPlayerStatus()
{
    char* captain_name[100];
    char* ship_name[100];
    char* hyper_class[100];
    char* current_system[50];
    char* sector[50];

    sprintf(captain_name, "Captain: %s", gs.captain_name);
    sprintf(ship_name, "Spacevessel: %s - Cargo: %d / %d - Cargo Value: %d $$", ship_names[gs.ship_type], gs.current_cargo, gs.tonnage, gs.cargo_value);
    sprintf(hyper_class, "Engine: %s - Fuel: %d%%", hyper_names[gs.hyper_class], gs.fuel);
    sprintf(current_system, "Current system: SA.%d", gs.current_system);
    sprintf(sector, "Sector %s", sectors[ptrList[gs.current_system].sector]);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);

    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 20, captain_name);
    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 40, ship_name);
    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 60, hyper_class);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);

    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 100, current_system);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 120, factions[ptrList[gs.current_system].faction]);
    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 140, sector);

}

void statusWnd()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, STATUS_WND_WIDTH - 1, STATUS_WND_HEIGHT - 1);

    adHypersoft();
    drawPlayerStatus();

    topStatusLine();
}
