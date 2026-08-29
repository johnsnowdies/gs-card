#include <graphics.h>
#include <stdio.h>
#include <string.h>


#include "data\structs.h"
#include "data\reader.h"
#include "core\game.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\shipyard\syardwnd.h"
#include "ui\ad\ad.h"
#include "music.h"

WND shipyard_wnd;
int ship_selected = 0;

extern char* data_ship_names[SHIP_COUNT];
extern unsigned int data_ship_tonnages[SHIP_COUNT];
extern char * data_hyper_names[SHIP_COUNT];
extern unsigned int data_hyper_fuel[SHIP_COUNT];
extern int system_shipyard_size;

extern SHIP system_shipyard[6];

static void draw_ship_upgrades_status_table(void)
{
    int x = shipyard_wnd.x + 10;
    int y_start = shipyard_wnd.y + 240;
    int i, max_label_width = 0;
    int step = 11;

    char *labels[4];
    char empty = '\0';
    unsigned char values[7];

    labels[0] = LC_UPGRADE_SHIP_HEAD;
    labels[1] = LC_UPGRADE_SMUGGLER_BAY;
    labels[2] = LC_UPGRADE_EMERGENCY_JUMP_SYSTEM;
    labels[3] = LC_UPGRADE_CONTIN_JUMP_SYSTEM;

    values[0] = empty;
    values[1] = system_shipyard[ship_selected].upgrade_smuggler_bay;
    values[2] = system_shipyard[ship_selected].upgrade_continuous_jump;
    values[3] = system_shipyard[ship_selected].upgrade_emergency_jump;
    
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    
    for (i = 0; i < 4; i++) {
        int w = textwidth(labels[i]);
        if (w > max_label_width) max_label_width = w;
    }

    
    for (i = 0; i < 4; i++) {
        int y = y_start + i * step;
        if (i == 0){
            setcolor(RED);
        }
        else{
            setcolor(15);  
        }
        outtextxy(x, y, labels[i]);

        if (i != 0){
            setcolor(values[i] ? 2 : 4);  
            outtextxy(x + max_label_width + 10, y,
                      values[i] ? LC_UPGRADE_YES : LC_UPGRADE_NO);
        }
    }
}

void gui_shipyard_draw_list()
{
    int x_pos, y_pos, i;
    int list_item_height = 60;
    int list_item_width = 0;
    WND l_wnd, l_holder;

    char *ship_description[6][6] = {
        { LC_GAME_SHIP_1_TEXT_1, LC_GAME_SHIP_1_TEXT_2, LC_GAME_SHIP_1_TEXT_3,
          LC_GAME_SHIP_1_TEXT_4, LC_GAME_SHIP_1_TEXT_5, LC_GAME_SHIP_1_TEXT_6 },

        { LC_GAME_SHIP_2_TEXT_1, LC_GAME_SHIP_2_TEXT_2, LC_GAME_SHIP_2_TEXT_3,
          LC_GAME_SHIP_2_TEXT_4, LC_GAME_SHIP_2_TEXT_5, LC_GAME_SHIP_2_TEXT_6 },

        { LC_GAME_SHIP_3_TEXT_1, LC_GAME_SHIP_3_TEXT_2, LC_GAME_SHIP_3_TEXT_3,
          LC_GAME_SHIP_3_TEXT_4, LC_GAME_SHIP_3_TEXT_5, LC_GAME_SHIP_3_TEXT_6 },

        { LC_GAME_SHIP_4_TEXT_1, LC_GAME_SHIP_4_TEXT_2, LC_GAME_SHIP_4_TEXT_3,
          LC_GAME_SHIP_4_TEXT_4, LC_GAME_SHIP_4_TEXT_5, LC_GAME_SHIP_4_TEXT_6 },

        { LC_GAME_SHIP_5_TEXT_1, LC_GAME_SHIP_5_TEXT_2, LC_GAME_SHIP_5_TEXT_3,
          LC_GAME_SHIP_5_TEXT_4, LC_GAME_SHIP_5_TEXT_5, LC_GAME_SHIP_5_TEXT_6 },

        { LC_GAME_SHIP_6_TEXT_1, LC_GAME_SHIP_6_TEXT_2, LC_GAME_SHIP_6_TEXT_3,
          LC_GAME_SHIP_6_TEXT_4, LC_GAME_SHIP_6_TEXT_5, LC_GAME_SHIP_6_TEXT_6 }
    };

    /* Determinate list width */
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    for (i = 0; i < SHIP_COUNT; i++)
    {
        if (list_item_width < textwidth(data_ship_names[i]) + 20)
            list_item_width = textwidth(data_ship_names[i]) + 20;
    }


    l_wnd.x = shipyard_wnd.width - list_item_width;
    l_wnd.y = shipyard_wnd.y;
    l_wnd.width = list_item_width;
    l_wnd.height = shipyard_wnd.height;
    l_wnd.header = NULL;

    gui_draw_wnd_proto(&l_wnd);

    l_holder.x = l_wnd.x;
    l_holder.y = l_wnd.y;
    l_holder.width = l_wnd.width;
    l_holder.height = l_wnd.height - 1;

    y_pos = l_holder.y;
    
    for (i = 0; i < system_shipyard_size; i++)
    {
        char line_1[100], line_2[100];
        sprintf(line_1, LC_UPGRADE_PRICE_L_TEXT, system_shipyard[i].base_price);
        sprintf(line_2, LC_UPGRADE_TONNAGE, data_ship_tonnages[system_shipyard[i].id]);

        if (i == ship_selected){
          setfillstyle(SOLID_FILL, RED);
          bar(l_holder.x, y_pos, l_holder.x + l_holder.width, y_pos + list_item_height);
          setcolor(BLACK);
        } else {
          setcolor(RED);
          rectangle(l_holder.x, y_pos, l_holder.x + l_holder.width, y_pos + list_item_height);
          setcolor(WHITE);
        }
          settextstyle(SMALL_FONT, HORIZ_DIR, 5);
          outtextxy(l_holder.x + 10, y_pos + 10, data_ship_names[i]);
          settextstyle(SMALL_FONT, HORIZ_DIR, 4);
          outtextxy(l_holder.x + 10, y_pos + 25, line_1);
          outtextxy(l_holder.x + 10, y_pos + 40, line_2);
          y_pos += list_item_height;
    }

    gui_draw_section_header_v(l_wnd.x - 40, l_wnd.y, 40, l_wnd.height, LC_UPGRADE_SHIP_VERT);

    {
        char ship_image[50];
        sprintf(ship_image, "SHIPS/SHIP_%u.BMP", system_shipyard[ship_selected].id + 1);
        data_reader_draw_bmp(ship_image, 20, 22);
    }

    y_pos = 200;

    setfillstyle(SOLID_FILL, BLACK);
    bar(shipyard_wnd.x + 1, y_pos, l_wnd.x - 41, shipyard_wnd.height-1);
    
    setcolor(WHITE);
    settextstyle(SMALL_FONT, HORIZ_DIR, 8);
    outtextxy(shipyard_wnd.x + 10, y_pos, data_ship_names[system_shipyard[ship_selected].id]);

    y_pos += 30;

    {
        char buf[100];
    
        settextstyle(SMALL_FONT, HORIZ_DIR, 4);
        sprintf(buf, "%s", data_hyper_names[system_shipyard[ship_selected].hyper_class]);
        outtextxy(shipyard_wnd.x + 10, y_pos, buf);

        y_pos += 10;

        sprintf(buf, "%s: %d", LC_UPGRADE_FUEL_PER_JUMP, data_hyper_fuel[system_shipyard[ship_selected].hyper_class]);
        outtextxy(shipyard_wnd.x + 10, y_pos, buf);
    }

    draw_ship_upgrades_status_table();

    y_pos += 70;
    setcolor(WHITE);

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);

    for (i = 0; i < 6; i++) {
        outtextxy(shipyard_wnd.x + 10, y_pos, ship_description[system_shipyard[ship_selected].id][i]);
        y_pos += 15;
    }


    {
        int rect_left  = shipyard_wnd.x + 10;
        int rect_right = l_wnd.x - 50;
        int text_x = (rect_left + rect_right - textwidth(LC_UPGRADE_SHIP_ENTER)) / 2;
        setlinestyle(3, 0, 1);
        setcolor(RED);
        rectangle(shipyard_wnd.x + 10, shipyard_wnd.height - 25, l_wnd.x - 50, shipyard_wnd.height + 10);
        outtextxy(text_x, shipyard_wnd.height - 15, LC_UPGRADE_SHIP_ENTER);
        setlinestyle(1, 0, 1);
    }

}

void gui_shipyard_wnd()
{
    shipyard_wnd.x = SYARD_WND_DEFAULT_X;
    shipyard_wnd.y = SYARD_WND_DEFAULT_Y;
    shipyard_wnd.width = SYARD_WND_DEFAULT_WIDTH;
    shipyard_wnd.height = SYARD_WND_DEFAULT_HEIGHT;
    shipyard_wnd.header = NULL;

    gui_draw_wnd_proto(&shipyard_wnd);

    
    setfillstyle(SOLID_FILL, BLACK);
    bar(1, shipyard_wnd.y+1, shipyard_wnd.width - 1, shipyard_wnd.y + shipyard_wnd.height - 1);
    
    gui_ad_hypersoft();

    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    setcolor(15);


    gui_shipyard_draw_list();

    gui_bars_status_bottom();
}
