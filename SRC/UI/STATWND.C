#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"

#include "ui\gui.h"

/* ----------------------------------------------------------------
 * Screen / viewport layout -- defined here, declared extern in gui.h
 * ---------------------------------------------------------------- */
int STATUS_WND_WIDTH  = 639;   /* may shrink to 470 when path panel is open */
int STATUS_WND_HEIGHT = 460;

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern struct game_state gs;
extern struct system_solar* sol_list;
extern int      render_danger_objects;
extern int      show_danger_hyperthreads;
extern int      show_danger_path_parts;

extern char* data_factions[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern char* data_ship_names[SHIP_COUNT];
extern int   data_ship_tonnages[SHIP_COUNT];
extern char* data_hyper_names[HYPER_COUNT];

/* Viewport bounds for clipping (status window only) */
static struct status_wnd {
    int x1, y1, x2, y2;
} status_wnd = { 0, 21, 639, 460 };


void draw_player_status()
{
    char captain_name[100];
    char ship_name[100];
    char hyper_class[100];
    char current_system[50];
    char sector[50];

    sprintf(captain_name, "Software registered for: %s", gs.captain_name);
    sprintf(ship_name, "Spacevessel: %s", data_ship_names[gs.ship_type]);
    sprintf(hyper_class, "Engine: %s", data_hyper_names[gs.hyper_class]);
    sprintf(current_system, "Current system: SA.%d", gs.current_system);
    sprintf(sector, "Sector %s", data_sectors[sol_list[gs.current_system].sector]);

    setcolor(4);
    setlinestyle(0, 0, 1);
    rectangle(0, 21, 639, MAP_WND_HEIGHT);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);

    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 20, captain_name);
    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 40, ship_name);
    outtextxy(status_wnd.x1 + 80, status_wnd.y1 + 60, hyper_class);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);

    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 100, current_system);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 120, data_factions[sol_list[gs.current_system].faction]);
    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 140, sector);
}

void gui_status_wnd()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, STATUS_WND_WIDTH - 1, STATUS_WND_HEIGHT - 1);

    gui_ad_hypersoft();
    draw_player_status();

    gui_top_status_line();
}
