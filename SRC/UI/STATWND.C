#include <graphics.h>
#include <stdio.h>

#include "data\structs.h"
#include "data\reader.h"

#include "ui\gui.h"

#include "ui\locale.h"

#include "core\game.h"

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

extern char* QUEST_TYPES[];

/* Viewport bounds for clipping (status window only) */
static struct status_wnd {
    int x1, y1, x2, y2;
} status_wnd = { 0, 21, 639, 460 };


/* ----------------------------------------------------------------
 * gui_status_bottom_status_line -- bottom shortcut bar + memory usage
 * ---------------------------------------------------------------- */
void gui_status_bottom_status_line()
{
    unsigned int USED_MEM, FREE_MEM, TOTAL_MEM = 65535;
    int xpos = BAR_LEFT;
    char memMsg[50] = "";

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    FREE_MEM = coreleft();
    USED_MEM = TOTAL_MEM - FREE_MEM;
    sprintf(memMsg, "%u/%u", USED_MEM, TOTAL_MEM);

    setfillstyle(SOLID_FILL, BLACK);
    bar(0, STATUSBAR_Y, MAP_WND_WIDTH, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Shortcut labels */
    setcolor(BAR_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "F1"); xpos += 13;
    setcolor(TEXT_COLOR); outtextxy(xpos, STATUSBAR_Y + 2, "-HELP"); xpos += 45;

    
    /* Separator */
    xpos = 470;
    setcolor(BAR_COLOR);
    line(xpos, STATUSBAR_Y - 1, xpos, STATUSBAR_Y + STATUSBAR_H - 1);

    /* Memory */
    setcolor(BAR_COLOR);
    outtextxy(xpos + 5, STATUSBAR_Y + 2, LC_MAP_STATUS_MEM);
    setcolor(TEXT_COLOR);
    outtextxy(xpos + 35, STATUSBAR_Y + 2, memMsg);
}

void draw_player_status()
{
    int i=0;
    char captain_name[100];
    char ship_name[100];
    char hyper_class[100];
    char current_system[50];
    char sector[50];
    int line_y, j;
    char line[100];

    sprintf(captain_name, "%s: %s",LC_STATUS_WND_CAPTAIN, gs.captain_name);
    sprintf(ship_name, "%s: %s",LC_STATUS_WND_SHIP, data_ship_names[gs.ship_type]);
    sprintf(hyper_class, "%s: %s",LC_STATUS_WND_HYPER, data_hyper_names[gs.hyper_class]);
    sprintf(current_system, "%s: SA.%d",LC_STATUS_WND_SYSTEM, gs.current_system);
    sprintf(sector, "%s: %s", LC_STATUS_WND_SECTOR, data_sectors[sol_list[gs.current_system].sector]);

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

    sprintf(line, "%-25.25s %-15.15s %-6.6s %6s %6s %6s",  
            "Заgача", 
            "Сектор", 
            "Система",
            "Груз", 
            "Награgа", 
            "Штраф");
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 200, line);
    for (i=0; i<5; i++){
        char buf[128], system[16];
        QUEST current;

        core_game_gen_quest(&current, gs.reputation, gs.current_system);

        sprintf(system, "SA.%d", current.target_system);
        sprintf(buf, 
            "%-25.25s %-15.15s %-6.6s %6d %6d %6d",
                QUEST_TYPES[current.type],
                data_sectors[current.target_sector],
                system,
                current.cargo,
                current.reward,
                current.penalty
            );
        
        outtextxy(status_wnd.x1 + 10, status_wnd.y1 + 220 + (15*i), buf);
    }

    gui_status_bottom_status_line();
    gui_memory_status();
}

void gui_status_wnd()
{
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, STATUS_WND_WIDTH - 1, STATUS_WND_HEIGHT - 1);

    gui_ad_hypersoft();
    draw_player_status();

    gui_top_status_line();
}

