#include <graphics.h>
#include <stdio.h>
#include <string.h>


#include "data\structs.h"
#include "data\reader.h"
#include "core\game.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\upgrade\upgrwnd.h"
#include "ui\npc\npcwnd.h"
#include "ui\stat\statwnd.h"
#include "music.h"

WND upgrade_wnd;
int upgrade_selected = 0;

extern GAME_STATE gs;
extern SYSTEM* sol_list;
extern char* data_ship_names[SHIP_COUNT];
extern int   data_ship_tonnages[SHIP_COUNT];
extern char* data_hyper_names[HYPER_COUNT];
extern int   data_hyper_fuel[HYPER_COUNT];
extern UPGRADE system_upgrades[8];
extern int system_upgrades_size;
extern NPC black_market_npc;

static void draw_ship_info(void)
{
    char buf[100];
    sprintf(buf, "%s", data_ship_names[gs.ship_type]);
    outtextxy(upgrade_wnd.x + 80, upgrade_wnd.y + 20, buf);

    sprintf(buf, "%s: %d", LC_UPGRADE_TONNAGE, data_ship_tonnages[gs.ship_type]);
    outtextxy(upgrade_wnd.x + 80, upgrade_wnd.y + 40, buf);
}

static void draw_engine_info(void)
{
    char buf[100];
    sprintf(buf, "%s", data_hyper_names[gs.hyper_class]);
    outtextxy(upgrade_wnd.x + 80, upgrade_wnd.y + 60, buf);

    sprintf(buf, "%s: %d", LC_UPGRADE_FUEL_PER_JUMP, data_hyper_fuel[gs.hyper_class]);
    outtextxy(upgrade_wnd.x + 10, upgrade_wnd.y + 90, buf);
}

static void draw_upgrade_status_table(void)
{
    int x = upgrade_wnd.x + 10;
    int y_start = upgrade_wnd.y + 105;
    int i, max_label_width = 0;
    int step = 15;  /* вертикальный шаг между строками */

    char *labels[5];
    unsigned char values[5];

    labels[0] = LC_UPGRADE_SMUGGLER_BAY;
    labels[1] = LC_UPGRADE_CONTIN_JUMP_SYSTEM;
    labels[2] = LC_UPGRADE_EMERGENCY_JUMP_SYSTEM;
    labels[3] = LC_UPGRADE_OBJECTS_MAP;
    labels[4] = LC_UPGRADE_POLITICAL_MAP;

    values[0] = gs.upgrade_smuggler_bay;
    values[1] = gs.upgrade_continuous_jump;
    values[2] = gs.upgrade_emergency_jump;
    values[3] = gs.upgrade_objects_map;
    values[4] = gs.upgrade_political_map;

    /* Вычисляем максимальную ширину названий */
    for (i = 0; i < 5; i++) {
        int w = textwidth(labels[i]);
        if (w > max_label_width) max_label_width = w;
    }

    /* Отрисовка строк с выравниванием значений по одному X */
    for (i = 0; i < 5; i++) {
        int y = y_start + i * step;
        setcolor(15);  /* белый для названия */
        outtextxy(x, y, labels[i]);

        setcolor(values[i] ? 2 : 4);  /* 2 = GREEN, 4 = RED */
        outtextxy(x + max_label_width + 10, y,
                  values[i] ? LC_UPGRADE_YES : LC_UPGRADE_NO);
    }
}

void gui_upgrade_draw_list(void)
{
    int i, y_pos;
    int points[6];
    int tx, ty;
    char line[100];

    y_pos = upgrade_wnd.y + 300;  

    
    setfillstyle(SOLID_FILL, BLACK);
    bar(upgrade_wnd.x+1, y_pos - 50, upgrade_wnd.x + upgrade_wnd.width-1, y_pos + system_upgrades_size * 10 + 10);

    
    setfillstyle(SOLID_FILL, RED);
    bar(upgrade_wnd.x, y_pos - 20, upgrade_wnd.x + upgrade_wnd.width, y_pos - 50);
    setcolor(0);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(upgrade_wnd.x + 10, y_pos - 42, LC_UPGRADE_BLACK_MARKET);

    setfillstyle(BKSLASH_FILL, RED);
    tx = upgrade_wnd.x + textwidth(LC_UPGRADE_BLACK_MARKET) + 20;
    ty = y_pos - 20;
    bar(tx, ty, upgrade_wnd.x + upgrade_wnd.width, y_pos - 50);

  points[0] = tx;        points[1] = ty - 30;
  points[2] = tx + 30;   points[3] = ty;
  points[4] = tx;        points[5] = ty;

    setcolor(RED);             
  setfillstyle(SOLID_FILL, RED);
  fillpoly(3, points);       

    
    sprintf(line, "%-40.40s %10s", LC_UPGRADE_TABLE_1, LC_UPGRADE_TABLE_2);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    setcolor(4);
    outtextxy(upgrade_wnd.x + 10, y_pos - 15, line);
    setcolor(15);

    
    for (i = 0; i < system_upgrades_size; i++) {
        char buf[120];
        int price = system_upgrades[i].base_price;
        int discount = (price * gs.reputation) / 1000;
        int final_price = price - discount;

        sprintf(buf, "%-40.40s %10d", system_upgrades[i].name, final_price);

        if (i == upgrade_selected) {
            setfillstyle(SOLID_FILL, RED);
            bar(upgrade_wnd.x + 10, y_pos + (10 * i),
                upgrade_wnd.x + upgrade_wnd.width - 11,
                y_pos + (10 * i) + 10);
            setcolor(BLACK);
        } else {
            setcolor(15);
        }
        outtextxy(upgrade_wnd.x + 10, y_pos + (10 * i), buf);
        setcolor(15);
    }
}

void gui_upgrade_wnd(void)
{
    upgrade_wnd.x = UPGRADE_WND_DEFAULT_X;
    upgrade_wnd.y = UPGRADE_WND_DEFAULT_Y;
    upgrade_wnd.width = UPGRADE_WND_DEFAULT_WIDTH;
    upgrade_wnd.height = UPGRADE_WND_DEFAULT_HEIGHT;
    upgrade_wnd.header = NULL;

    gui_draw_wnd_proto(&upgrade_wnd);

    
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, upgrade_wnd.y, upgrade_wnd.width - 1, upgrade_wnd.y + upgrade_wnd.height - 1);

    gui_ad_hypersoft();

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);
    draw_ship_info();
    draw_engine_info();
    draw_upgrade_status_table();

    {
        char ship_image[50];
        sprintf(ship_image, "SHIPS/SHIP_%u.BMP", gs.ship_type + 1);
        data_reader_draw_bmp(ship_image, 339, 22);
    }

    gui_upgrade_draw_list();
    gui_bars_status_bottom();
}

void gui_upgrade_show_info(int selected)
{
    if (selected < 0 || selected >= system_upgrades_size) return;

    {
        char lines[4][100];
        char buttons[2][100] = { LC_GUI_BOOL_YES, LC_GUI_BOOL_NO };
        int i;
        int price = system_upgrades[selected].base_price;
        int discount = (price * gs.reputation) / 1000;
        int final_price = price - discount;

        for (i = 0; i < 4; i++) lines[i][0] = '\0';

        if (system_upgrades[selected].type == 0) {
            int level = system_upgrades[selected].id;
            sprintf(lines[0], LC_UPGRADE_ENGINE_TEXT_1, data_hyper_names[level]);
            sprintf(lines[1], LC_UPGRADE_ENGINE_TEXT_2, data_hyper_fuel[level]);
            sprintf(lines[2], LC_UPGRADE_ENGINE_TEXT_3);
            sprintf(lines[3], LC_UPGRADE_PRICE_TEXT, final_price);
        } else {
            int id = system_upgrades[selected].id;
            switch (id) {
                case 0: 
                    strcpy(lines[0], LC_UPGRADE_SMUGGLER_BAY_TEXT_1);
                    strcpy(lines[1], LC_UPGRADE_SMUGGLER_BAY_TEXT_2);
                    strcpy(lines[2], LC_UPGRADE_SMUGGLER_BAY_TEXT_3);
                    strcpy(lines[3], LC_UPGRADE_SMUGGLER_BAY_TEXT_4);
                    break;
                case 1: 
                    strcpy(lines[0], LC_UPGRADE_CONTIN_JUMP_TEXT_1);
                    strcpy(lines[1], LC_UPGRADE_CONTIN_JUMP_TEXT_2);
                    strcpy(lines[2], LC_UPGRADE_CONTIN_JUMP_TEXT_3);
                    strcpy(lines[3], LC_UPGRADE_CONTIN_JUMP_TEXT_4);
                    break;
                case 2: 
                    strcpy(lines[0], LC_UPGRADE_EMERGENCY_JUMP_TEXT_1);
                    strcpy(lines[1], LC_UPGRADE_EMERGENCY_JUMP_TEXT_2);
                    strcpy(lines[2], LC_UPGRADE_EMERGENCY_JUMP_TEXT_3);
                    strcpy(lines[3], LC_UPGRADE_EMERGENCY_JUMP_TEXT_4);
                    break;
                case 3: 
                    strcpy(lines[0], LC_UPGRADE_OBJECTS_MAP_TEXT_1);
                    strcpy(lines[1], LC_UPGRADE_OBJECTS_MAP_TEXT_2);
                    strcpy(lines[2], LC_UPGRADE_OBJECTS_MAP_TEXT_3);
                    strcpy(lines[3], LC_UPGRADE_OBJECTS_MAP_TEXT_4);
                    break;
                case 4: 
                    strcpy(lines[0], LC_UPGRADE_POLITICAL_MAP_TEXT_1);
                    strcpy(lines[1], LC_UPGRADE_POLITICAL_MAP_TEXT_2);
                    strcpy(lines[2], LC_UPGRADE_POLITICAL_MAP_TEXT_3);
                    strcpy(lines[3], LC_UPGRADE_POLITICAL_MAP_TEXT_4);
                    break;
            }
        }

        {
            int result = gui_npc_wnd(&upgrade_wnd, &black_market_npc, NPC_CHOICE_WND,
                                     system_upgrades[selected].name, lines, 4, buttons, 2);

            if (result == 0) { 
                if (gs.balance >= final_price) {
                    gs.balance -= final_price;
                    if (system_upgrades[selected].type == 0) {
                        gs.hyper_class = system_upgrades[selected].id;
                    } else {
                        switch (system_upgrades[selected].id) {
                            case 0: gs.upgrade_smuggler_bay = 1; break;
                            case 1: gs.upgrade_continuous_jump = 1; break;
                            case 2: gs.upgrade_emergency_jump = 1; break;
                            case 3: gs.upgrade_objects_map = 1; break;
                            case 4: gs.upgrade_political_map = 1; break;
                        }
                    }

                    {
                        int idx = selected;
                        for (i = idx; i < system_upgrades_size - 1; i++) {
                            system_upgrades[i] = system_upgrades[i + 1];
                        }
                        system_upgrades_size--;
                        upgrade_selected = 0;
                    }

                    gui_upgrade_wnd();
                    sfx_hyperjump();
                } else {
                    gui_warning_wnd(&upgrade_wnd, LC_GEN_ERROR_HEAD, LC_UPGRADE_NO_MONEY, SOUND_ERROR);
                    getch();
                    gui_upgrade_wnd();
                }
            } else {
                gui_upgrade_wnd();
            }
        }
    }
}
