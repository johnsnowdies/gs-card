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

#include "music.h"

#include "ui\npc\npcwnd.h"

WND status_wnd;
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
extern int   data_hyper_fuel[HYPER_COUNT];

extern char* QUEST_TYPES[];

extern QUEST system_quests[5];

extern WAYPOINT wp;

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;

extern int system_quests_size;

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

void gui_status_quest_info(int selected)
{
    int result, i;

    char lines[5][100];
    for (i = 0; i < 5; i++) {
        lines[i][0] = '\0';
    }

    strcpy(lines[0], QUEST_TYPES[system_quests[selected].type]);

    if (system_quests[selected].giver.faction == 1)
        strcpy(lines[1], LC_QUEST_GREETING_IRISH);
    else if (system_quests[selected].giver.faction == 0)
        strcpy(lines[1], LC_QUEST_GREETING_ARAB);
    else
        strcpy(lines[1],LC_QUEST_GREETING_COMMON);

    switch (system_quests[selected].type) {
      case 1:
        sprintf(lines[2], LC_QUEST_TYPE_1_LINE_1,
                system_quests[selected].target_system,
                data_sectors[system_quests[selected].target_sector]);
        sprintf(lines[3], LC_QUEST_TYPE_1_LINE_2);
        sprintf(lines[4], LC_QUEST_REWARD_1, system_quests[selected].reward);
        break;
      case 2:
        sprintf(lines[2], LC_QUEST_TYPE_2_LINE_1,
                system_quests[selected].target_system,
                data_sectors[system_quests[selected].target_sector]);
        sprintf(lines[3], LC_QUEST_TYPE_2_LINE_2);
        sprintf(lines[4], LC_QUEST_REWARD_2, system_quests[selected].reward);
        break;
      case 3:
        sprintf(lines[2], LC_QUEST_TYPE_3_LINE_1,
                system_quests[selected].target_system,
                data_sectors[system_quests[selected].target_sector]);
        sprintf(lines[3], LC_QUEST_TYPE_3_LINE_2);
        sprintf(lines[4], LC_QUEST_REWARD_3, system_quests[selected].reward);
        break;
      case 4:
        sprintf(lines[2], LC_QUEST_TYPE_4_LINE_1,
                system_quests[selected].target_system,
                data_sectors[system_quests[selected].target_sector]);
        sprintf(lines[3], LC_QUEST_TYPE_4_LINE_2);

        sprintf(lines[4], LC_QUEST_REWARD_4, system_quests[selected].reward);
        break;
      case 5:
        sprintf(lines[2], LC_QUEST_TYPE_5_LINE_1,
                system_quests[selected].target_system,
                data_sectors[system_quests[selected].target_sector]);
        sprintf(lines[3], LC_QUEST_TYPE_5_LINE_2);
        sprintf(lines[4], LC_QUEST_REWARD_5, system_quests[selected].reward);
        break;
    }

    result = gui_npc_wnd(&status_wnd, &system_quests[selected].giver, NPC_CHOICE_WND, lines, 5);

    /*
     * QUEST INFO CONTROLLER 
     */
    
    if(result == 0){
        if(core_game_accept_quest(selected)){
            cur_screen = SCR_STATUS;
            system_quest_selected = 0;                    
            gui_status_wnd();
            gui_map_top_status_line();
            sfx_hyperjump();
        }
        else{
            gui_warning_wnd(&status_wnd, LC_GEN_ERROR_HEAD, LC_QUEST_ERROR, SOUND_ERROR);
            getch();
            cur_screen = SCR_STATUS;
            system_quest_selected = 0;
            gui_status_wnd();
            gui_map_top_status_line();
            sfx_screen_change();
        }
    }
    else
    {
        cur_screen = SCR_STATUS;
        gui_status_wnd();
        sfx_screen_change();
    }
}

void gui_status_quest_list(WND *status_wnd, int selected)
{
    char line[100];
    int i = 0, y_pos, x_pos;

    y_pos = status_wnd->y + 220;
    x_pos = status_wnd->x + 10;

    setfillstyle(SOLID_FILL, BLACK);
    bar(x_pos, y_pos, status_wnd->x + status_wnd->width - 11, y_pos + 50);

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

    for (i = 0; i < system_quests_size; i++) {
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

        if (i == system_quest_selected) {
            setfillstyle(SOLID_FILL, RED);
            bar(x_pos, y_pos + (10 * i), status_wnd->x + status_wnd->width - 11, y_pos + (10 * i) + 10);
        }
        outtextxy(x_pos, y_pos + (10 * i), buf);
    }

    y_pos += 120;

    /*
     * my quests
     */

    setfillstyle(SOLID_FILL, RED);
    bar(status_wnd->x, y_pos - 20, status_wnd->x + status_wnd->width, y_pos - 50);
    
    setcolor(0);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(status_wnd->x + 10, y_pos - 42, LC_QUEST_MY_HEAD);

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

    for (i = 0; i < gs.quests_size; i++) {
        char buf[128], system[16];
        sprintf(system, "SA.%d", gs.quests[i].target_system);
        sprintf(buf, 
            "%-25.25s %-15.15s %-6.6s %6d %6d %6d",
                QUEST_TYPES[gs.quests[i].type],
                data_sectors[gs.quests[i].target_sector],
                system,
                gs.quests[i].cargo,
                gs.quests[i].reward,
                gs.quests[i].penalty
            );

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

    char ship_image[50];

    sprintf(captain_name, "%s: %s", LC_STATUS_WND_CAPTAIN, gs.captain_name);
    sprintf(ship_name, "%s",  data_ship_names[gs.ship_type]);
    sprintf(hyper_class, "%s", data_hyper_names[gs.hyper_class]);
    sprintf(current_system, "%s: SA.%d", LC_STATUS_WND_SYSTEM, gs.current_system);
    sprintf(sector, "%s: %s", LC_STATUS_WND_SECTOR, data_sectors[sol_list[gs.current_system].sector]);
    sprintf(ship_image, "SHIPS/SHIP_%u.BMP", gs.ship_type+1);

    data_reader_draw_bmp(ship_image, 339,22);

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
    status_wnd.width = 639;
    status_wnd.height = 459;

    setfillstyle(SOLID_FILL, BLACK);
    bar(1, 22, status_wnd.width - 1, status_wnd.height - 1);

    gui_ad_hypersoft();
    draw_player_status(&status_wnd);

    gui_status_quest_list(&status_wnd, system_quest_selected);
    gui_status_bottom_status_line();
}
