#include <graphics.h>
#include <stdio.h>
#include <string.h>

#include "core/game.h"
#include "core/globals.h"
#include "data/structs.h"
#include "sound/sound.h"
#include "ui/gui.h"
#include "ui/locale.h"
#include "ui/npc/npcwnd.h"
#include "ui/shipyard/syardwnd.h"
#include "ui/menu/menuwnd.h"
#include "ui/ad/ad.h"

WND shipyard_wnd;
int ship_selected = 0;



/* -----------------------------------------------------------------
 * SHIP UPGRADES INSTALLED
 * ---------------------------------------------------------------- */
static void draw_ship_upgrades_status_table(void) {
  int x = shipyard_wnd.x + 10;
  int y_start = shipyard_wnd.y + 240;
  int i, max_label_width = 0;
  int step = 11;

  char* labels[4];
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
    if (i == 0) {
      setcolor(RED);
    } else {
      setcolor(15);
    }
    outtextxy(x, y, labels[i]);

    if (i != 0) {
      setcolor(values[i] ? 2 : 4);
      outtextxy(x + max_label_width + 10, y,
                values[i] ? LC_UPGRADE_YES : LC_UPGRADE_NO);
    }
  }
}

/* -----------------------------------------------------------------
 * DRAW SHIPYARD WND
 * ---------------------------------------------------------------- */
static void gui_shipyard_wnd() {
  shipyard_wnd.id = SCR_SHIPYARD;
  shipyard_wnd.x = SYARD_WND_DEFAULT_X;
  shipyard_wnd.y = SYARD_WND_DEFAULT_Y;
  shipyard_wnd.width = SYARD_WND_DEFAULT_WIDTH;
  shipyard_wnd.height = SYARD_WND_DEFAULT_HEIGHT;
  shipyard_wnd.header = NULL;

  gui_draw_wnd_proto(&shipyard_wnd);

  setfillstyle(SOLID_FILL, BLACK);
  bar(1, shipyard_wnd.y + 1, shipyard_wnd.width - 1,
      shipyard_wnd.y + shipyard_wnd.height - 1);

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  setcolor(15);

  gui_shipyard_draw_list();
}


/* ----------------------------------------------------------------
 *
 *                      EXTERNAL FUNCTIONS
 *
 * ---------------------------------------------------------------- */

/* -----------------------------------------------------------------
 * DEAL WINDOW DISPATCHER
 * ---------------------------------------------------------------- */
void gui_shipyard_deal_wnd(void) {
  if (ship_selected < 0 || ship_selected >= system_shipyard_size) return;

  {
    char lines[20][100];
    char buttons[2][100] = {LC_SHIPYARD_YES_BTN, LC_SHIPYARD_NO_BTN};
    int i, line_cnt = 0;

    long price = system_shipyard[ship_selected].base_price;
    long discount = (price * gs.reputation) / 1000;

    long ship_sellout_discount = ship_prices[gs.ship_type] / 2;
    long hyper_sellout_discount = hyper_prices[gs.hyper_class] / 2;
    long upgrade_smuggler_bay_discount =
        gs.upgrade_smuggler_bay ? custom_prices[0] / 2 : 0;
    long upgrade_continuous_jump_discount =
        gs.upgrade_continuous_jump ? custom_prices[1] / 2 : 0;
    long upgrade_emergency_jump_discount =
        gs.upgrade_emergency_jump ? custom_prices[2] / 2 : 0;

    long final_price = price - discount - ship_sellout_discount -
                       hyper_sellout_discount - upgrade_smuggler_bay_discount -
                       upgrade_continuous_jump_discount -
                       upgrade_emergency_jump_discount;

    if (final_price <= 0) {
      gui_warning_wnd(&shipyard_wnd, LC_GEN_ERROR_HEAD,
                      LC_SHIPYARD_SELL_TEXT_10, SOUND_ERROR);
      return;
    }

    for (i = 0; i < 20; i++) lines[i][0] = '\0';

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_1,
            system_shipyard[ship_selected].giver.name);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_2, gs.captain_name);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_3, gs.current_system);
    line_cnt++;

    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_4);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_5);
    line_cnt++;
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_6);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_DISC_SHIP,
            data_ship_names[gs.ship_type], ship_sellout_discount);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_DISC_HYPER,
            data_hyper_names[gs.hyper_class], hyper_sellout_discount);
    line_cnt++;

    if (upgrade_smuggler_bay_discount > 0) {
      sprintf(lines[line_cnt], LC_SHIPYARD_DISC_SMUGG,
              upgrade_smuggler_bay_discount);
      line_cnt++;
    }

    if (upgrade_continuous_jump_discount > 0) {
      sprintf(lines[line_cnt], LC_SHIPYARD_DISC_CJUMP,
              upgrade_continuous_jump_discount);
      line_cnt++;
    }

    if (upgrade_emergency_jump_discount > 0) {
      sprintf(lines[line_cnt], LC_SHIPYARD_DISC_EJUMP,
              upgrade_emergency_jump_discount);
      line_cnt++;
    }

    sprintf(lines[line_cnt], LC_SHIPYARD_DISC_REP, discount);
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TEXT_7);
    line_cnt++;
    line_cnt++;

    sprintf(lines[line_cnt], LC_SHIPYARD_SELL_TOTAL, final_price);
    line_cnt++;

    {
      int result = gui_dialog_wnd(&shipyard_wnd, LC_SHIPYARD_SELL_HEAD,
                                  LC_SHIPYARD_SELL_HEAD, NULL, lines, line_cnt,
                                  buttons, 2, NO_SOUND, 1);

      if (result == 0) {
        int confirm = gui_confirm_wnd(&shipyard_wnd, LC_SHIPYARD_SELL_HEAD,
                                      LC_SHIPYARD_CONFIRM);

        if (confirm == 0) {
          if (gs.balance >= final_price) {
            gs.balance -= final_price;
            gs.ship_type = system_shipyard[ship_selected].id;
            gs.tonnage = data_ship_tonnages[gs.ship_type];
            gs.hyper_class = system_shipyard[ship_selected].hyper_class;

            gs.upgrade_smuggler_bay =
                system_shipyard[ship_selected].upgrade_smuggler_bay;
            gs.upgrade_continuous_jump =
                system_shipyard[ship_selected].upgrade_continuous_jump;
            gs.upgrade_emergency_jump =
                system_shipyard[ship_selected].upgrade_emergency_jump;

            {
              int idx = ship_selected;
              for (i = idx; i < system_shipyard_size - 1; i++) {
                system_shipyard[i] = system_shipyard[i + 1];
              }
              system_shipyard_size--;
              ship_selected = 0;
            }
            dispatch_wnd(SCR_SHIPYARD);

            sfx_success();
          } else {
            gui_warning_wnd(&shipyard_wnd, LC_GEN_ERROR_HEAD,
                            LC_UPGRADE_NO_MONEY, SOUND_ERROR);
          }
        } else {
          sfx_error();
        }
      }
    }
  }
}

/* -----------------------------------------------------------------
 * SHIPS LIST WINDOW DISPATCHER
 * ---------------------------------------------------------------- */
void gui_shipyard_draw_list(void) {
  int x_pos, y_pos, i;
  int list_item_height = 60;
  int list_item_width = 0;
  WND l_wnd, l_holder;

  char* ship_description[6][6] = {
      {LC_GAME_SHIP_1_TEXT_1, LC_GAME_SHIP_1_TEXT_2, LC_GAME_SHIP_1_TEXT_3,
       LC_GAME_SHIP_1_TEXT_4, LC_GAME_SHIP_1_TEXT_5, LC_GAME_SHIP_1_TEXT_6},

      {LC_GAME_SHIP_2_TEXT_1, LC_GAME_SHIP_2_TEXT_2, LC_GAME_SHIP_2_TEXT_3,
       LC_GAME_SHIP_2_TEXT_4, LC_GAME_SHIP_2_TEXT_5, LC_GAME_SHIP_2_TEXT_6},

      {LC_GAME_SHIP_3_TEXT_1, LC_GAME_SHIP_3_TEXT_2, LC_GAME_SHIP_3_TEXT_3,
       LC_GAME_SHIP_3_TEXT_4, LC_GAME_SHIP_3_TEXT_5, LC_GAME_SHIP_3_TEXT_6},

      {LC_GAME_SHIP_4_TEXT_1, LC_GAME_SHIP_4_TEXT_2, LC_GAME_SHIP_4_TEXT_3,
       LC_GAME_SHIP_4_TEXT_4, LC_GAME_SHIP_4_TEXT_5, LC_GAME_SHIP_4_TEXT_6},

      {LC_GAME_SHIP_5_TEXT_1, LC_GAME_SHIP_5_TEXT_2, LC_GAME_SHIP_5_TEXT_3,
       LC_GAME_SHIP_5_TEXT_4, LC_GAME_SHIP_5_TEXT_5, LC_GAME_SHIP_5_TEXT_6},

      {LC_GAME_SHIP_6_TEXT_1, LC_GAME_SHIP_6_TEXT_2, LC_GAME_SHIP_6_TEXT_3,
       LC_GAME_SHIP_6_TEXT_4, LC_GAME_SHIP_6_TEXT_5, LC_GAME_SHIP_6_TEXT_6}};

  /* Determinate list width */
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  for (i = 0; i < SHIP_COUNT; i++) {
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

  for (i = 0; i < system_shipyard_size; i++) {
    char line_1[100], line_2[100];
    sprintf(line_1, LC_UPGRADE_PRICE_L_TEXT, system_shipyard[i].base_price);
    sprintf(line_2, LC_UPGRADE_TONNAGE,
            data_ship_tonnages[system_shipyard[i].id]);

    if (i == ship_selected) {
      setfillstyle(SOLID_FILL, RED);
      bar(l_holder.x, y_pos, l_holder.x + l_holder.width,
          y_pos + list_item_height);
      setcolor(BLACK);
    } else {
      setcolor(RED);
      rectangle(l_holder.x, y_pos, l_holder.x + l_holder.width,
                y_pos + list_item_height);
      setcolor(WHITE);
    }
    settextstyle(SMALL_FONT, HORIZ_DIR, 5);
    outtextxy(l_holder.x + 10, y_pos + 10,
              data_ship_names[system_shipyard[i].id]);
    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
    outtextxy(l_holder.x + 10, y_pos + 25, line_1);
    outtextxy(l_holder.x + 10, y_pos + 40, line_2);
    y_pos += list_item_height;
  }

  gui_draw_section_header_v(l_wnd.x - 40, l_wnd.y, 40, l_wnd.height,
                            LC_UPGRADE_SHIP_VERT);

  {
    char ship_image[50];
    sprintf(ship_image, "SHIPS/SHIP_%u.BMP",
            system_shipyard[ship_selected].id + 1);
    data_reader_draw_bmp(ship_image, 45, 22);
  }

  y_pos = 200;

  setfillstyle(SOLID_FILL, BLACK);
  bar(shipyard_wnd.x + 1, y_pos, l_wnd.x - 41, shipyard_wnd.height - 1);

  setcolor(WHITE);
  settextstyle(SMALL_FONT, HORIZ_DIR, 8);
  outtextxy(shipyard_wnd.x + 10, y_pos,
            data_ship_names[system_shipyard[ship_selected].id]);

  y_pos += 30;

  {
    char buf[100];

    settextstyle(SMALL_FONT, HORIZ_DIR, 4);
    sprintf(buf, "%s",
            data_hyper_names[system_shipyard[ship_selected].hyper_class]);
    outtextxy(shipyard_wnd.x + 10, y_pos, buf);

    y_pos += 10;

    sprintf(buf, "%s: %d", LC_UPGRADE_FUEL_PER_JUMP,
            data_hyper_fuel[system_shipyard[ship_selected].hyper_class]);
    outtextxy(shipyard_wnd.x + 10, y_pos, buf);
  }

  draw_ship_upgrades_status_table();

  y_pos += 70;
  setcolor(WHITE);

  settextstyle(SMALL_FONT, HORIZ_DIR, 4);

  for (i = 0; i < 6; i++) {
    outtextxy(shipyard_wnd.x + 10, y_pos,
              ship_description[system_shipyard[ship_selected].id][i]);
    y_pos += 15;
  }

  {
    int rect_left = shipyard_wnd.x + 10;
    int rect_right = l_wnd.x - 50;
    int text_x =
        (rect_left + rect_right - textwidth(LC_UPGRADE_SHIP_ENTER)) / 2;
    setlinestyle(3, 0, 1);
    setcolor(RED);
    rectangle(shipyard_wnd.x + 10, shipyard_wnd.height - 25, l_wnd.x - 50,
              shipyard_wnd.height + 10);
    outtextxy(text_x, shipyard_wnd.height - 15, LC_UPGRADE_SHIP_ENTER);
    setlinestyle(0, 0, 1);
  }
  gui_ad_warning(&l_wnd);
}

/* -----------------------------------------------------------------
 * SCR_SHIPYARD -- WINDOW DISPATCHER
 * ---------------------------------------------------------------- */

void gui_shipyard_wnd_dispatch(){
  shipyard_wnd.ptr_parent = &root_wnd;
  gui_shipyard_wnd();
  gui_bars_common_top();
  gui_bars_status_bottom();
}
