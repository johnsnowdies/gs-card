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

extern QUEST system_quests[5];
extern unsigned int system_quest_selected;

/* Viewport bounds for clipping (status window only) */
static struct status_wnd {
    int x1, y1, x2, y2;
} status_wnd = { 0, 21, 639, 460 };

static struct quest_info_wnd
{
    int x1, y1, x2, y2;
} quest_info_wnd = { 10, 30, 610, 650};

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

void gui_status_quest_info(int selected)
{
    char* prequest[100], reward[100];
    char* system[10];
    sprintf(system, "SA.%d", system_quests[selected].target_system);

    gui_draw_generic_wnd(quest_info_wnd.x1, quest_info_wnd.y1, 610, 340);
    setcolor(4);
    rectangle(quest_info_wnd.x1 + 10, quest_info_wnd.y1 + 30, quest_info_wnd.x1 + 150, quest_info_wnd.y1 + 200);
    settextstyle(SMALL_FONT, HORIZ_DIR, 6);
    outtextxy(quest_info_wnd.x1+40, quest_info_wnd.y1 + 100, "NO PHOTO");

    draw_4bit_bmp("1.bmp", 20, 60);
    
    setcolor(15);
    outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 30, QUEST_TYPES[system_quests[selected].type]);
    setcolor(4);
    outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 50, system_quests[selected].giver->name);
    setcolor(15);

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    if (system_quests[selected].giver->faction == 1)
        outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 80, LC_QUEST_GREETING_IRISH);
    else if (system_quests[selected].giver->faction == 0)
        outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 80, LC_QUEST_GREETING_ARAB);
    else
        outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 80, LC_QUEST_GREETING_COMMON);

    switch(system_quests[selected].type){
    case 1:
        sprintf(prequest, LC_QUEST_TYPE_1_LINE_1, system, data_sectors[system_quests[selected].target_sector]);
        sprintf(reward, LC_QUEST_REWARD_1, system_quests[selected].reward);
        break;
    case 2:
        sprintf(prequest, LC_QUEST_TYPE_2_LINE_1, system, data_sectors[system_quests[selected].target_sector]);
        sprintf(reward, LC_QUEST_REWARD_2, system_quests[selected].reward);
        break;
    case 3: 
        sprintf(prequest, LC_QUEST_TYPE_3_LINE_1, system, data_sectors[system_quests[selected].target_sector]);
        sprintf(reward, LC_QUEST_REWARD_3, system_quests[selected].reward);
        break;
    case 4:
        sprintf(prequest, LC_QUEST_TYPE_4_LINE_1, system, data_sectors[system_quests[selected].target_sector]);
        sprintf(reward, LC_QUEST_REWARD_4, system_quests[selected].reward);
        break;
    case 5:
        sprintf(prequest, LC_QUEST_TYPE_5_LINE_1, system, data_sectors[system_quests[selected].target_sector]);
        sprintf(reward, LC_QUEST_REWARD_5, system_quests[selected].reward);
        break;
    }

    outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 95, prequest);
    outtextxy(quest_info_wnd.x1+160, quest_info_wnd.y1 + 110, reward);

    
}

void gui_status_quest_wnd(int selected)
{
    char line[100];
    int i=0, y_pos, x_pos;

    y_pos = status_wnd.y1 + 220;
    x_pos = status_wnd.x1 + 10;

    setfillstyle(SOLID_FILL, BLACK);
    bar(x_pos, y_pos, 630, 459);

    setfillstyle(SOLID_FILL, RED);
    bar(0, y_pos-20, 640, y_pos - 50);

    setcolor(0);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(10, y_pos-42, LC_QUEST_NEW_HEAD);

    sprintf(line, "%-25.25s %-15.15s %-6.6s %6s %6s %6s",  
            LC_QUEST_TABLE_1,
            LC_QUEST_TABLE_2,
            LC_QUEST_TABLE_3,
            LC_QUEST_TABLE_4,
            LC_QUEST_TABLE_5,
            LC_QUEST_TABLE_6);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(4);
    outtextxy(x_pos, y_pos - 15, line);
    setcolor(15);


    
    for (i=0; i<5; i++){
        char buf[128], system[16];
        sprintf(system, "SA.%d", system_quests[i].target_system);
        sprintf(buf, 
            "%-25.25s %-15.15s %-6.6s %6d %6d %6d",
                QUEST_TYPES[system_quests[i].type],
                data_sectors[system_quests[i].target_sector],
                system,
                system_quests[i].cargo,
                system_quests[i].reward,
                system_quests[i].penalty
            );
        
        
        if (i == selected) {
            setfillstyle(SOLID_FILL, RED);
            bar(x_pos, y_pos + (10*i), 630,  y_pos + (10*i) + 10);
        }
        outtextxy(x_pos, y_pos + (10*i), buf);

    }
}

void draw_player_status()
{
    char captain_name[100];
    char ship_name[100];
    char hyper_class[100];
    char current_system[50];
    char sector[50];
    int line_y, j;

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

    gui_status_quest_wnd(system_quest_selected);

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

