#include <graphics.h>
#include <stdio.h>
#include <conio.h>

#include "data\structs.h"
#include "data\reader.h"
#include "data\keys.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "core\game.h"

#include "ui\stat\statwnd.h"

WND status_wnd, quest_wnd;
int system_quest_selected = 0;

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern GAME_STATE gs;
extern SYSTEM* sol_list;

extern char* data_factions[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern char* data_ship_names[SHIP_COUNT];
extern int   data_ship_tonnages[SHIP_COUNT];
extern char* data_hyper_names[HYPER_COUNT];

extern char* QUEST_TYPES[];

extern QUEST system_quests[5];

extern WAYPOINT wp;

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;

/* ----------------------------------------------------------------
 * gui_status_bottom_status_line -- bottom shortcut bar
 * ---------------------------------------------------------------- */
void gui_status_bottom_status_line()
{
    WND status_line;
    char *keys[] = { "F1", NULL };
    char *items[] = { "-HELP", NULL };

    status_line.x = 0;
    status_line.y = STATUSBAR_BOTTOM_Y;
    status_line.width = STATUSBAR_WIDTH;
    status_line.height = STATUSBAR_HEIGHT;
    status_line.header = NULL;

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    gui_draw_status_line(&status_line, keys, items);
}

void gui_status_quest_info(WND *quest_wnd, int selected)
{
    char line_1[100], line_2[100], reward[100], photo[10];
    char *genders[] = {
        "M", "F"
    };

    gui_draw_wnd_proto(quest_wnd);

    setcolor(4);
    rectangle(quest_wnd->x + 9, quest_wnd->y + 29, quest_wnd->x + 150, quest_wnd->y + 200);
    settextstyle(SMALL_FONT, HORIZ_DIR, 6);
    outtextxy(quest_wnd->x + 40, quest_wnd->y + 100, "NO PHOTO");

    switch(system_quests[selected].giver.faction){
        case 1:
        case 3:
            sprintf(photo, "NPC/R%s%d.BMP", genders[system_quests[selected].giver.gender], system_quests[selected].giver.portrait + 1);
        break;

        case 0:
            sprintf(photo, "NPC/A%s%d.BMP", genders[system_quests[selected].giver.gender], system_quests[selected].giver.portrait + 1);
        break;

        default:
            sprintf(photo, "NPC/S%s%d.BMP", genders[system_quests[selected].giver.gender], system_quests[selected].giver.portrait + 1);
        break;
    }

    data_reader_draw_bmp(photo, quest_wnd->x + 10, quest_wnd->y + 30);
    
    setcolor(15);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 30, QUEST_TYPES[system_quests[selected].type]);
    setcolor(4);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 50, system_quests[selected].giver.name);
    setcolor(15);

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    if (system_quests[selected].giver.faction == 1)
        outtextxy(quest_wnd->x + 160, quest_wnd->y + 80, LC_QUEST_GREETING_IRISH);
    else if (system_quests[selected].giver.faction == 0)
        outtextxy(quest_wnd->x + 160, quest_wnd->y + 80, LC_QUEST_GREETING_ARAB);
    else
        outtextxy(quest_wnd->x + 160, quest_wnd->y + 80, LC_QUEST_GREETING_COMMON);

    switch(system_quests[selected].type){
    case 1:
        sprintf(line_1, LC_QUEST_TYPE_1_LINE_1, system_quests[selected].target_system, data_sectors[system_quests[selected].target_sector]);
        sprintf(line_2, LC_QUEST_TYPE_1_LINE_2);
        sprintf(reward, LC_QUEST_REWARD_1, system_quests[selected].reward);
        break;
    case 2:
        sprintf(line_1, LC_QUEST_TYPE_2_LINE_1, system_quests[selected].target_system, data_sectors[system_quests[selected].target_sector]);
        sprintf(line_2, LC_QUEST_TYPE_2_LINE_2);
        sprintf(reward, LC_QUEST_REWARD_2, system_quests[selected].reward);
        break;
    case 3: 
        sprintf(line_1, LC_QUEST_TYPE_3_LINE_1, system_quests[selected].target_system, data_sectors[system_quests[selected].target_sector]);
        sprintf(line_2, LC_QUEST_TYPE_3_LINE_2);
        sprintf(reward, LC_QUEST_REWARD_3, system_quests[selected].reward);
        break;
    case 4:
        sprintf(line_1, LC_QUEST_TYPE_4_LINE_1, system_quests[selected].target_system, data_sectors[system_quests[selected].target_sector]);
        sprintf(line_2, LC_QUEST_TYPE_4_LINE_2);
        
        sprintf(reward, LC_QUEST_REWARD_4, system_quests[selected].reward);
        break;
    case 5:
        sprintf(line_1, LC_QUEST_TYPE_5_LINE_1, system_quests[selected].target_system, data_sectors[system_quests[selected].target_sector]);
        sprintf(line_2, LC_QUEST_TYPE_5_LINE_2);
        sprintf(reward, LC_QUEST_REWARD_5, system_quests[selected].reward);
        break;
    }

    outtextxy(quest_wnd->x + 160, quest_wnd->y + 95, line_1);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 110, line_2);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 125, reward);

    setcolor(4);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 140, LC_QUEST_TABLE_6);
    outtextxy(quest_wnd->x + 160, quest_wnd->y + 155, LC_QUEST_TABLE_4);

    setcolor(15);

    sprintf(line_1, "%d $$", system_quests[selected].penalty);
    sprintf(line_2, "%d", system_quests[selected].cargo);

    outtextxy(quest_wnd->x + 210, quest_wnd->y + 140, line_1);
    outtextxy(quest_wnd->x + 210, quest_wnd->y + 155, line_2);
}

void gui_status_quest_list(WND *status_wnd, int selected)
{
    char line[100];
    int i = 0, y_pos, x_pos;

    y_pos = status_wnd->y + 220;
    x_pos = status_wnd->x + 10;

    setfillstyle(SOLID_FILL, BLACK);
    bar(x_pos, y_pos, status_wnd->x + status_wnd->width - 11, status_wnd->y + status_wnd->height - 2);

    setfillstyle(SOLID_FILL, RED);
    bar(status_wnd->x, y_pos - 20, status_wnd->x + status_wnd->width, y_pos - 50);

    setcolor(0);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(status_wnd->x + 10, y_pos - 42, LC_QUEST_NEW_HEAD);

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

    for (i = 0; i < 5; i++) {
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
            bar(x_pos, y_pos + (10 * i), status_wnd->x + status_wnd->width - 10, y_pos + (10 * i) + 10);
        }
        outtextxy(x_pos, y_pos + (10 * i), buf);
    }
}

void draw_player_status(WND *status_wnd)
{
    char captain_name[100];
    char ship_name[100];
    char hyper_class[100];
    char current_system[50];
    char sector[50];

    sprintf(captain_name, "%s: %s", LC_STATUS_WND_CAPTAIN, gs.captain_name);
    sprintf(ship_name, "%s: %s", LC_STATUS_WND_SHIP, data_ship_names[gs.ship_type]);
    sprintf(hyper_class, "%s: %s", LC_STATUS_WND_HYPER, data_hyper_names[gs.hyper_class]);
    sprintf(current_system, "%s: SA.%d", LC_STATUS_WND_SYSTEM, gs.current_system);
    sprintf(sector, "%s: %s", LC_STATUS_WND_SECTOR, data_sectors[sol_list[gs.current_system].sector]);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);

    outtextxy(status_wnd->x + 80, status_wnd->y + 20, captain_name);
    outtextxy(status_wnd->x + 80, status_wnd->y + 40, ship_name);
    outtextxy(status_wnd->x + 80, status_wnd->y + 60, hyper_class);

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);

    outtextxy(status_wnd->x + 10, status_wnd->y + 100, current_system);
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(status_wnd->x + 10, status_wnd->y + 120, data_factions[sol_list[gs.current_system].faction]);
    outtextxy(status_wnd->x + 10, status_wnd->y + 140, sector);
}

void gui_status_wnd()
{
    status_wnd.header = "";
    status_wnd.x = 0;
    status_wnd.y = 21;
    status_wnd.width = 640;
    status_wnd.height = 459;

    quest_wnd.header = QUEST_TYPES[system_quests[system_quest_selected].type];
    quest_wnd.x = 10;
    quest_wnd.y = 60;
    quest_wnd.width = 600;
    quest_wnd.height = 300;

    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, status_wnd.width - 1, status_wnd.height - 1);

    gui_ad_hypersoft();
    draw_player_status(&status_wnd);

    gui_status_quest_list(&status_wnd, system_quest_selected);
    gui_status_bottom_status_line();
}
