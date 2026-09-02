#include <math.h>
#include <stdlib.h>

#include "core/events.h"
#include "core/game.h"
#include "core/globals.h"
#include "core/objects.h"
#include "sound/sound.h"
#include "ui/gui.h"
#include "ui/locale.h"
#include "ui/map/mapwnd.h"
#include "ui/npc/npcwnd.h"

/* ----------------------------------------------------------------
 *
 *                      GAME EVENTS SYSTEM
 *
 * ----------------------------------------------------------------

/* ----------------------------------------------------------------
 * GAME OVER EVENT - FUEL GONE
 * ---------------------------------------------------------------- */
static int core_events_check_fuel_gone(void) {
  if (gs.fuel <= 0) {
    char lines[9][100];
    int i;

    for (i = 0; i < 9; i++) {
      lines[i][0] = '\0';
    }

    sprintf(lines[0], LC_GAME_OVER_FUEL_TEXT_1);
    sprintf(lines[1], LC_GAME_OVER_FUEL_TEXT_2);
    sprintf(lines[2], "   ");
    sprintf(lines[3], LC_GAME_OVER_STATS_HEAD);
    sprintf(lines[4], LC_GAME_OVER_STATS_TEXT_2, gs.missions_completed);
    sprintf(lines[5], LC_GAME_OVER_STATS_TEXT_3, gs.balance);
    sprintf(lines[6], "   ");
    sprintf(lines[7], LC_GAME_OVER_STATS_TEXT_4);
    gui_dialog_wnd(&map_wnd, LC_GAME_OVER_HEAD, LC_GAME_OVER_HEAD, NULL, lines,
                   8, NULL, 0, SOUND_ERROR, 1);

    return 1;
  }
  return 0;
}

/* ----------------------------------------------------------------
 * GAME OVER EVENT - MONEY GONE
 * ---------------------------------------------------------------- */
static int core_events_check_money_gone(void) {
  if (gs.balance <= 0) {
    char lines[9][100];
    int i;

    for (i = 0; i < 9; i++) {
      lines[i][0] = '\0';
    }

    sprintf(lines[0], LC_GAME_OVER_MONEY_TEXT_1);
    sprintf(lines[1], LC_GAME_OVER_MONEY_TEXT_2);
    sprintf(lines[2], "   ");
    sprintf(lines[3], LC_GAME_OVER_STATS_HEAD);
    sprintf(lines[4], LC_GAME_OVER_STATS_TEXT_2, gs.missions_completed);
    sprintf(lines[5], LC_GAME_OVER_STATS_TEXT_3, gs.balance);
    sprintf(lines[6], "   ");
    sprintf(lines[7], LC_GAME_OVER_STATS_TEXT_4);
    gui_dialog_wnd(&map_wnd, LC_GAME_OVER_HEAD, LC_GAME_OVER_HEAD, NULL, lines,
                   8, NULL, 0, SOUND_ERROR, 1);

    return 1;
  }
  return 0;
}

/* ----------------------------------------------------------------
 * GAME OVER EVENT - PLAYER WINS!
 * ---------------------------------------------------------------- */
static int core_events_check_win(void) {
  if (gs.balance >= 1000000) {
    char lines[9][100];
    int i;

    for (i = 0; i < 9; i++) {
      lines[i][0] = '\0';
    }

    sprintf(lines[0], LC_GAME_OVER_WIN_TEXT_1);
    sprintf(lines[1], LC_GAME_OVER_WIN_TEXT_2);
    sprintf(lines[2], "   ");
    sprintf(lines[3], LC_GAME_OVER_STATS_HEAD);
    sprintf(lines[4], LC_GAME_OVER_STATS_TEXT_2, gs.missions_completed);
    sprintf(lines[5], LC_GAME_OVER_STATS_TEXT_3, gs.balance);
    sprintf(lines[6], "   ");
    sprintf(lines[7], LC_GAME_OVER_STATS_TEXT_4);
    gui_dialog_wnd(&map_wnd, LC_GAME_OVER_HEAD, LC_GAME_OVER_HEAD, NULL, lines,
                   8, NULL, 0, SOUND_ERROR, 1);

    return 1;
  }
  return 0;
}

/* ----------------------------------------------------------------
 * CHECK ALL GAME OVER CONDITIONS
 * ---------------------------------------------------------------- */
static int core_events_check_game_over() {
  if (core_events_check_fuel_gone()) return 1;
  if (core_events_check_money_gone()) return 1;
  if (core_events_check_win()) return 1;

  return 0;
}

/* ----------------------------------------------------------------
 * EVENT - QUEST DONE
 * ---------------------------------------------------------------- */
static int core_events_quest_done(void) {
  int i, j;
  char lines[2][100];

  for (i = 0; i < gs.quests_size; i++) {
    if (gs.quests[i].target_system == gs.current_system) {
      /* Set quest done */
      gs.current_cargo -= gs.quests[i].cargo;
      gs.balance += gs.quests[i].reward;
      if (gs.reputation < MAX_REPUTATION){
        /* add reward/10 as reputation */
        gs.reputation += gs.quests[i].reward / 10;
        if (gs.reputation > MAX_REPUTATION)
          gs.reputation = MAX_REPUTATION;
      }


      switch (gs.quests[i].type) {
        case 1:
          sprintf(lines[0], LC_QUEST_TYPE_1_DONE_1);
          sprintf(lines[1], LC_QUEST_TYPE_1_DONE_2, gs.quests[i].reward);
          break;
        case 2:
          sprintf(lines[0], LC_QUEST_TYPE_2_DONE_1);
          sprintf(lines[1], LC_QUEST_TYPE_2_DONE_2, gs.quests[i].reward);
          break;
        case 3:
          sprintf(lines[0], LC_QUEST_TYPE_3_DONE_1);
          sprintf(lines[1], LC_QUEST_TYPE_3_DONE_2, gs.quests[i].reward);
          break;
        case 4:
          sprintf(lines[0], LC_QUEST_TYPE_4_DONE_1);
          sprintf(lines[1], LC_QUEST_TYPE_4_DONE_2, gs.quests[i].reward);
          break;
        case 5:
          sprintf(lines[0], LC_QUEST_TYPE_5_DONE_1);
          sprintf(lines[1], LC_QUEST_TYPE_5_DONE_2, gs.quests[i].reward);
          break;
      }

      gui_npc_wnd(&map_wnd, &gs.quests[i].giver, NPC_DIALOG_WND,
                  LC_QUEST_COMPLETE_HEAD, lines, 2, NULL, 0, 1);

      /* Remove Quest from user log */
      for (j = i; j < gs.quests_size; j++) {
        gs.quests[j] = gs.quests[j + 1];
      }

      gs.quests_size--;
      gs.missions_completed++;
      if (i >= gs.quests_size) break;
    }
  }
}

/* ----------------------------------------------------------------
 * EVENT - GAS STATION
 * ---------------------------------------------------------------- */
static void core_events_gas_station(void) {
  if (sol_list[gs.current_system].is_gas_station && gs.fuel < 100) {
    unsigned long percent_price, amount, total;

    int i, j;
    char* text[100];

    char lines[3][100];
    char buttons[2][100] = {LC_GUI_BOOL_YES, LC_GUI_BOOL_NO};

    int choice = 0, gender = 0, faction = 0;
    NPC gas_worker;

    for (i = 0; i < 3; i++) {
      lines[i][0] = '\0';
    }

    percent_price = (rand() % 3) + 1;
    amount = 100 - gs.fuel;
    total = amount * percent_price;
    faction = sol_list[gs.current_system].faction;
    gender = faction == 1 ? 1 : 0;

    core_game_gen_npc(&gas_worker, faction, gender, GAS_NPC);

    sprintf(lines[0], LC_GAME_GAS_STATION_TEXT_1, gs.current_system);
    sprintf(lines[1], LC_GAME_GAS_STATION_TEXT_2, percent_price);

    if (total > gs.balance) {
      /* Not enough money! */
      sprintf(lines[2], LC_GAME_GAS_STATION_NO_MONEY_TEXT, total);
      gui_npc_wnd(&map_wnd, &gas_worker, NPC_DIALOG_WND,
                  LC_GAME_GAS_STATION_HEAD, lines, 3, NULL, 0, 1);

    } else {
      sprintf(lines[2], LC_GAME_GAS_STATION_TEXT_3, total);
      choice = gui_npc_wnd(&map_wnd, &gas_worker, NPC_CHOICE_WND,
                           LC_GAME_GAS_STATION_HEAD, lines, 3, buttons, 2, 1);

      if (choice == 0) {
        gs.fuel = 100;
        gs.balance -= total;
      }
    }
  }
}

/* -----------------------------------------------------------------
 * EVENT - QUEST FAILED, SHOW PENALTY, DEDUCT BALANCE, REMOVE QUEST
 * ---------------------------------------------------------------- */
static core_events_quest_failed(int index) {
  int type, i;
  char lines[2][100];
  char header[80];

  if (index < 0 || index >= gs.quests_size) return;

  type = gs.quests[index].type;

  /* Select failure text by quest type */
  switch (type) {
    case 1:
      strcpy(lines[0], LC_QUEST_TYPE_1_FAIL_1);
      sprintf(lines[1], LC_QUEST_TYPE_1_FAIL_2, gs.quests[index].penalty);
      break;
    case 2:
      strcpy(lines[0], LC_QUEST_TYPE_2_FAIL_1);
      sprintf(lines[1], LC_QUEST_TYPE_2_FAIL_2, gs.quests[index].penalty);
      break;
    case 3:
      strcpy(lines[0], LC_QUEST_TYPE_3_FAIL_1);
      sprintf(lines[1], LC_QUEST_TYPE_3_FAIL_2, gs.quests[index].penalty);
      break;
    case 4:
      strcpy(lines[0], LC_QUEST_TYPE_4_FAIL_1);
      sprintf(lines[1], LC_QUEST_TYPE_4_FAIL_2, gs.quests[index].penalty);
      break;
    case 5:
      strcpy(lines[0], LC_QUEST_TYPE_5_FAIL_1);
      sprintf(lines[1], LC_QUEST_TYPE_5_FAIL_2, gs.quests[index].penalty);
      break;
    default:
      strcpy(lines[0], "Quest failed.");
      sprintf(lines[1], "Penalty: %ld", gs.quests[index].penalty);
      break;
  }

  strcpy(header, LC_QUEST_FAILED_HEAD);

  gui_npc_wnd(&map_wnd, &gs.quests[index].giver, NPC_DIALOG_WND, header, lines,
              2, NULL, 0, 1);

  /* Apply penalty */
  gs.balance -= gs.quests[index].penalty;

  if (gs.reputation > MIN_REPUTATION) {
    /* substract penalty/10 as reputation */
    gs.reputation -= gs.quests[index].penalty / 10;
    if (gs.reputation < MIN_REPUTATION) gs.reputation = MIN_REPUTATION;
  }

  gui_bars_common_top();

  /* Remove quest by shifting array left */
  for (i = index; i < gs.quests_size - 1; i++) {
    gs.quests[i] = gs.quests[i + 1];
  }
  gs.quests_size--;
}

/* -----------------------------------------------------------------
 * EVENT - PIRACY
 * ---------------------------------------------------------------- */
static int core_events_piracy(void) {
  int ship_type, faction, i;
  long request;
  NPC npc_pirate;
  char lines[2][100];
  char buttons[3][100];
  int btn_count = 0;
  int pay_idx = -1, drop_idx = -1, ejump_idx = -1;
  int choice;
  int droppable_cargo = 0;

  ship_type = gs.ship_type;

  /* Ransom amount based on ship type */
  switch (ship_type) {
    case 0:
      request = 1000 + rand() % 1001;
      break; /* 1000..2000 */
    case 1:
      request = 1000 + rand() % 2001;
      break; /* 1000..3000 */
    case 2:
      request = 1000 + rand() % 4001;
      break; /* 1000..5000 */
    case 3:
      request = 1000 + rand() % 4001;
      break; /* same as 2 */
    case 4:
      request = 1000 + rand() % 9001;
      break; /* 1000..10000 */
    case 5:
      request = 1000 + rand() % 19001;
      break; /* 1000..20000 */
    default:
      request = 1000 + rand() % 1001;
  }

  for (i = 0; i < gs.quests_size; i++) {
    if (gs.quests[i].type == 1 || gs.quests[i].type == 2)
      droppable_cargo += gs.quests[i].cargo;
  }

  /* Generate npc_pirate: 90% Irish */
  if ((rand() % 100) < 90)
    faction = 1;
  else
    faction = rand() % 4;
  core_game_gen_npc(&npc_pirate, faction, RANDOM_GENDER, QUEST_NPC);

  strcpy(lines[0], LC_EVENT_PIRACY_TEXT_1);
  sprintf(lines[1], LC_EVENT_PIRACY_TEXT_2, request);

  /* Build active buttons */
  /*if (gs.balance >= request) {*/
  pay_idx = btn_count;
  strcpy(buttons[btn_count], LC_EVENT_PIRACY_PAY_BTN);
  btn_count++;
  /*}*/

  if (droppable_cargo > 0) {
    drop_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_PIRACY_DROP_BTN);
    btn_count++;
  }

  if (gs.upgrade_emergency_jump) {
    ejump_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_EJUMP_BTN);
    btn_count++;
  }

  choice = gui_npc_wnd(&map_wnd, &npc_pirate, NPC_CHOICE_WND,
                       LC_EVENT_PIRACY_HEAD, lines, 2, buttons, btn_count, 2);

  if (choice == pay_idx) {
    /* Pay ransom */
    gs.balance -= request;
    gui_bars_common_top();
  } else if (choice == drop_idx) {
    /* Drop all cargo and fail corresponding quests */
    gs.current_cargo = 0;
    for (i = gs.quests_size - 1; i >= 0; i--) {
      if (gs.quests[i].type == 1 || gs.quests[i].type == 2 ||
          (gs.quests[i].type == 3 && !gs.upgrade_smuggler_bay)) {
        core_events_quest_failed(i);
      }
    }
  } else if (choice == ejump_idx) {
    /* Emergency jump */
    int current = gs.current_system;
    int thread_count = sol_list[current].threadSize;
    if (thread_count > 0) {
      int selected = rand() % thread_count;
      gs.prev_system = gs.current_system;
      gs.current_system = sol_list[current].threads[selected].value;
      wp.size = 0;
      return core_events(1);
    }
  }
  return 0;
}

/* -----------------------------------------------------------------
 * EVENT - CUSTOMS
 * ---------------------------------------------------------------- */
static int core_events_customs(void) {
  int faction, i;
  long bribe;
  NPC customs_officer;
  char lines[2][100];
  char buttons[3][100];
  int btn_count = 0;
  int bribe_idx = -1, allow_idx = -1, ejump_idx = -1;
  int has_contraband = 0;
  int choice;

  faction = sol_list[gs.current_system].faction;
  core_game_gen_npc(&customs_officer, faction, RANDOM_GENDER, QUEST_NPC);

  /* Bribe amount 500..2000 */
  bribe = 500 + rand() % 1501;

  sprintf(lines[0], LC_EVENT_CUSTOMS_TEXT_1);
  sprintf(lines[1], LC_EVENT_CUSTOMS_TEXT_2, gs.current_system,
          data_sectors[sol_list[gs.current_system].sector]);

  /* Check for contraband (type 3) quests */
  for (i = 0; i < gs.quests_size; i++) {
    if (gs.quests[i].type == 3) {
      has_contraband = 1;
      break;
    }
  }

  /* Build buttons */
  if (has_contraband) {
    bribe_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_CUSTOM_BARB_BTN);
    btn_count++;
  }
  allow_idx = btn_count;
  strcpy(buttons[btn_count], LC_EVENT_CUSTOM_ALLOW_BTN);
  btn_count++;
  if (gs.upgrade_emergency_jump) {
    ejump_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_EJUMP_BTN);
    btn_count++;
  }

  choice = gui_npc_wnd(&map_wnd, &customs_officer, NPC_CHOICE_WND,
                       LC_EVENT_CUSTOMS_HEAD, lines, 2, buttons, btn_count, 2);

  if (choice == bribe_idx) {
    /* Pay bribe */
    gs.balance -= bribe;
    gui_bars_common_top();
    strcpy(lines[0], LC_EVENT_CUSTOMS_TEXT_3);
    gui_npc_wnd(&map_wnd, &customs_officer, NPC_DIALOG_WND,
                LC_EVENT_CUSTOMS_HEAD, lines, 1, NULL, 0, 1);
  } else if (choice == allow_idx) {
    /* Allow inspection */
    if (!has_contraband || gs.upgrade_smuggler_bay) {
      strcpy(lines[0], LC_EVENT_CUSTOMS_TEXT_4);
      gui_npc_wnd(&map_wnd, &customs_officer, NPC_DIALOG_WND,
                  LC_EVENT_CUSTOMS_HEAD, lines, 1, NULL, 0, 1);
    } else {
      /* Confiscate first contraband quest */
      for (i = 0; i < gs.quests_size; i++) {
        if (gs.quests[i].type == 3) {
          strcpy(lines[0], LC_EVENT_CUSTOMS_TEXT_4);
          gui_npc_wnd(&map_wnd, &customs_officer, NPC_DIALOG_WND,
                      LC_EVENT_CUSTOMS_HEAD, lines, 1, NULL, 0, 1);
          core_events_quest_failed(i);
          break;
        }
      }
    }
  } else if (choice == ejump_idx) {
    /* Emergency jump */
    int current = gs.current_system;
    int thread_count = sol_list[current].threadSize;
    if (thread_count > 0) {
      int selected = rand() % thread_count;
      gs.prev_system = gs.current_system;
      gs.current_system = sol_list[current].threads[selected].value;
      wp.size = 0;
      return core_events(1);
    }
  }

  return 0;
}

/* -----------------------------------------------------------------
 * EVENT - KIDNAPPING
 * ---------------------------------------------------------------- */
static int core_events_kidnapping(int quest_index) {
  NPC kidnapper;
  int faction;
  char lines[2][100];
  char msg[100];
  char buttons[3][100];
  int btn_count = 0;
  int hide_idx = -1, give_idx = -1, ejump_idx = -1;
  int choice;
  QUEST* quest;

  if (quest_index < 0 || quest_index >= gs.quests_size) return 0;

  quest = &gs.quests[quest_index];

  /* Generate kidnapper: 90% Irish */
  if ((rand() % 100) < 90)
    faction = 1;
  else
    faction = rand() % 4;
  core_game_gen_npc(&kidnapper, faction, RANDOM_GENDER, QUEST_NPC);

  sprintf(lines[0], LC_EVENT_NAP_TEXT_1, quest->giver.name);
  strcpy(lines[1], LC_EVENT_NAP_TEXT_2);

  /* Build buttons */
  if (gs.upgrade_smuggler_bay) {
    hide_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_NAP_HIDE_BTN);
    btn_count++;
  }
  give_idx = btn_count;
  strcpy(buttons[btn_count], LC_EVENT_NAP_GIVE_BTN);
  btn_count++;
  if (gs.upgrade_emergency_jump) {
    ejump_idx = btn_count;
    strcpy(buttons[btn_count], LC_EVENT_EJUMP_BTN);
    btn_count++;
  }
  choice = gui_npc_wnd(&map_wnd, &kidnapper, NPC_CHOICE_WND, LC_EVENT_NAP_HEAD,
                       lines, 2, buttons, btn_count, 2);

  if (choice == hide_idx) {
    /* Hide NPC, reward half */
    long half_reward = quest->reward / 2;
    char single_line[1][100];

    sprintf(single_line[0], LC_EVENT_NAP_THANK, half_reward);
    gui_npc_wnd(&map_wnd, &quest->giver, NPC_DIALOG_WND, LC_EVENT_NAP_HEAD,
                single_line, 1, NULL, 0, 1);
    gs.balance += half_reward;
  } else if (choice == give_idx) {
    /* Give NPC, fail quest */
    char single_line[1][100];

    strcpy(single_line[0], LC_EVENT_NAP_FAIL);
    gui_npc_wnd(&map_wnd, &kidnapper, NPC_DIALOG_WND, LC_EVENT_NAP_HEAD,
                single_line, 1, NULL, 0, 1);
    core_events_quest_failed(quest_index);
  } else if (choice == ejump_idx) {
    /* Emergency jump */
    int current = gs.current_system;
    int thread_count = sol_list[current].threadSize;
    if (thread_count > 0) {
      int selected = rand() % thread_count;
      long half_reward = quest->reward / 2;
      char single_line[1][100];
      sprintf(single_line[0], LC_EVENT_NAP_THANK, half_reward);
      gui_npc_wnd(&map_wnd, &quest->giver, NPC_DIALOG_WND, LC_EVENT_NAP_HEAD,
                  single_line, 1, NULL, 0, 1);
      gs.balance += half_reward;
      gs.prev_system = gs.current_system;
      gs.current_system = sol_list[current].threads[selected].value;
      wp.size = 0;
      return core_events(1);
    }
  }
  return 0;
}

/* -----------------------------------------------------------------
 * EVENT - DANGER OBJECTS
 * ---------------------------------------------------------------- */
static int core_events_danger_object(void) {
  int cur = gs.current_system;
  int prev = gs.prev_system;
  int o = 0, i;

  if (gs.current_system == gs.prev_system) return 0;
  for (o = 0; o < obj_size; o++) {
    if (core_objects_sphere_line_intersect(
            sol_list[prev].x, sol_list[prev].y, sol_list[prev].z,
            sol_list[cur].x, sol_list[cur].y, sol_list[cur].z, obj_list[o].x,
            obj_list[o].y, obj_list[o].z, obj_list[o].r)) {
      int thread_count = sol_list[cur].threadSize;

      char lines[8][100];

      for (i = 0; i < 8; i++) {
        lines[i][0] = '\0';
      }

      if (obj_list[o].type == OBJ_GASCLOUD) {
        int quest_selected = -1;

        for (i = 0; i < gs.quests_size; i++) {
          if (gs.quests[i].type == 1 || gs.quests[i].type == 2 ||
              (gs.quests[i].type == 3 && !gs.upgrade_smuggler_bay)) {
            quest_selected = i;
            break;
          }
        }
        sprintf(lines[0], LC_EVENT_DANGER_TEXT, prev, cur, LC_EVENT_DANGER_GAS);

        /* Cargo damaged */
        if (quest_selected >= 0) {
          sprintf(lines[1], LC_EVENT_DANGER_GAS_TEXT,
                  gs.quests[quest_selected].cargo,
                  gs.quests[quest_selected].target_system);
          gs.quests[quest_selected].reward /= 2;
          sprintf(lines[2], LC_EVENT_DANGER_GAS_TEXT_2,
                  gs.quests[quest_selected].reward);
          gui_dialog_wnd(&map_wnd, LC_EVENT_DANGER_HEAD, LC_EVENT_DANGER_HEAD,
                         NULL, lines, 3, NULL, 0, SOUND_ERROR, 1);
        } else {
          sprintf(lines[1], LC_EVENT_DANGER_GAS_NO);
          gui_dialog_wnd(&map_wnd, LC_EVENT_DANGER_HEAD, LC_EVENT_DANGER_HEAD,
                         NULL, lines, 2, NULL, 0, SOUND_ERROR, 1);
        }
      } else if (obj_list[o].type == OBJ_NEBULA) {
        int selected = -1, found = 0;
        selected = (rand() % 4) + 1;

        /* Upgrade damaged if installed */
        switch (selected) {
          case 1:
            if (gs.upgrade_continuous_jump) found = 1;
            gs.upgrade_continuous_jump = 0;
            break;
          case 2:
            if (gs.upgrade_emergency_jump) found = 1;
            gs.upgrade_emergency_jump = 0;
            break;
          case 3:
            if (gs.upgrade_objects_map) found = 1;
            gs.upgrade_objects_map = 0;
            break;
          case 4:
            if (gs.upgrade_political_map) found = 1;
            gs.upgrade_political_map = 0;
            break;
        }

        sprintf(lines[0], LC_EVENT_DANGER_TEXT, prev, cur, LC_EVENT_DANGER_NEB);

        if (found)
          sprintf(lines[1], LC_EVENT_DANGER_NEB_TEXT,
                  data_upgrade_names[selected]);
        else
          sprintf(lines[1], LC_EVENT_DANGER_NEB_NO);

        gui_dialog_wnd(&map_wnd, LC_EVENT_DANGER_HEAD, LC_EVENT_DANGER_HEAD,
                       NULL, lines, 2, NULL, 0, SOUND_ERROR, 1);
      } else if (obj_list[o].type == OBJ_BLACKHOLE) {
        int selected = rand() % thread_count;
        gs.prev_system = gs.current_system;
        gs.current_system = sol_list[cur].threads[selected].value;
        wp.size = 0;
        sprintf(lines[0], LC_EVENT_DANGER_TEXT, prev, cur, LC_EVENT_DANGER_BH);
        sprintf(lines[1], LC_EVENT_DANGER_BH_TEXT, gs.current_system);
        gui_dialog_wnd(&map_wnd, LC_EVENT_DANGER_HEAD, LC_EVENT_DANGER_HEAD,
                       NULL, lines, 2, NULL, 0, SOUND_ERROR, 1);
        return core_events(0);
      }
    }
  }

  return 0;
}

/* ----------------------------------------------------------------
 *
 *                      EXTERNAL FUNCTIONS
 *
 * ---------------------------------------------------------------- */

/* -----------------------------------------------------------------
 * EVENT SYSTEM MAIN FUNCTIONS
 * ---------------------------------------------------------------- */
int core_events(int fuel_consume) {
  int i = 0, game_over = 0;
  char buf[50];
  int start_system = gs.current_system;

  core_game_mark_visited(gs.current_system);
  if (fuel_consume) gs.fuel -= data_hyper_fuel[gs.hyper_class];

  if (core_events_check_game_over()) return 1;

  if (!game_over) {
    /* Run new system arrival events */

    /* Target system changed by black hole */
    {
      int nested_over = core_events_danger_object();
      if (gs.current_system != start_system || nested_over) {
        game_over = nested_over;
        return game_over;
      }
    }

    /* Piracy Event */
    if (rand() % 100 < (sol_list[gs.current_system].faction == 1 ? 30 : 10)) {
      int nested_over = core_events_piracy();
      if (gs.current_system != start_system || nested_over) {
        game_over = nested_over;
        return game_over;
      }
    }
    /* Customs Event */
    if (rand() % 100 < (sol_list[gs.current_system].faction == 2 ? 70 : 20) &&
        sol_list[gs.current_system].is_shipyard) {
      int nested_over = core_events_customs();
      if (gs.current_system != start_system || nested_over) {
        game_over = nested_over;
        return game_over;
      }
    }

    /* Kidnapping Event */
    for (i = 0; i < gs.quests_size; i++) {
      if (gs.quests[i].type == 4) {
        if (rand() % 100 < 10) {
          int nested_over = core_events_kidnapping(i);
          if (gs.current_system != start_system || nested_over) {
            game_over = nested_over;
            return game_over;
          }
        }
        break; /* only first type 4 quest triggers kidnapping */
      }
    }

    /* Gas Station Event */
    core_events_gas_station();

    /* Quest Done Event */
    core_events_quest_done();

    core_game_gen_all();
  }

  return core_events_check_game_over();
}
