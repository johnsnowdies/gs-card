#include <stdlib.h>
#include <math.h>

#include "data/structs.h"
#include "data/reader.h"

#include "core/objects.h"
#include "core/finder.h"
#include "core/game.h"

#include "ui/locale.h" 

#include "ui/gui.h"
#include "ui/npc/npcwnd.h"

#include "music.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern int DEBUG;

extern GAME_STATE gs;

extern WAYPOINT wp;

extern SYSTEM* sol_list;
extern unsigned int sol_size;

extern OBJECT* obj_list;
extern unsigned int obj_size;

extern WND map_wnd;
extern WND root_wnd;

extern QUEST system_quests[5];
extern int system_quests_size;

extern UPGRADE system_upgrades[8];
extern int system_upgrades_size;

extern SHIP system_shipyard[6];
extern int system_shipyard_size;

#define N_SIZE 15


/* ----------------------------------------------------------------
 * Game Data Structures
 * ---------------------------------------------------------------- */
char* data_ship_names[SHIP_COUNT] = {LC_GAME_SHIP_1, LC_GAME_SHIP_2,
                                     LC_GAME_SHIP_3, LC_GAME_SHIP_4,
                                     LC_GAME_SHIP_5, LC_GAME_SHIP_6};

unsigned int data_ship_engines[SHIP_COUNT] = {0, 0, 1, 1, 3, 2};
unsigned char data_ship_smuggler_bay[SHIP_COUNT] = { 0, 1, 1, 1, 0, 0};
unsigned char data_ship_continuous_jump[SHIP_COUNT] = {0, 0, 1, 1, 1, 1};
unsigned char data_ship_emergency_jump[SHIP_COUNT] = {0, 0, 0, 1, 1, 1};


unsigned int data_ship_tonnages[SHIP_COUNT] = {50, 80, 100, 150, 200, 400};

char* data_hyper_names[HYPER_COUNT] = {LC_GAME_ENGINE_1, LC_GAME_ENGINE_2,
                                       LC_GAME_ENGINE_3, LC_GAME_ENGINE_4};

unsigned int data_hyper_fuel[HYPER_COUNT] = {10, 8, 5, 2};

char* data_factions[FACTIONS_COUNT] = {LC_GAME_FACTION_1, LC_GAME_FACTION_2,
                                       LC_GAME_FACTION_3, LC_GAME_FACTION_4};

unsigned int data_factions_colors[FACTIONS_COUNT] = {2, 14, 9, 4};

char* data_sectors[SECTORS_COUNT] = {
    LC_GAME_SECTOR_1, LC_GAME_SECTOR_2, LC_GAME_SECTOR_3,
    LC_GAME_SECTOR_4, LC_GAME_SECTOR_5, LC_GAME_SECTOR_6,
    LC_GAME_SECTOR_7, LC_GAME_SECTOR_8, LC_GAME_SECTOR_9};

long ship_prices[SHIP_COUNT] = SHIP_UPGRADE_BASE_PRICES;
long hyper_prices[HYPER_COUNT] = HYPER_UPGRADE_BASE_PRICES;
long custom_prices[CUSTOM_UPGRADES_COUNT] = CUSTOM_UPGRADE_BASE_PRICES;

/* ----------------------------------------------------------------
 * Visited solar systems tracking
 * ---------------------------------------------------------------- */

void game_mark_visited(GAME_STATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return;
    gs->visited[system >> 3] |= (1 << (system & 7));
}

int game_is_visited(GAME_STATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return 0;
    return (gs->visited[system >> 3] >> (system & 7)) & 1;
}


/* ----------------------------------------------------------------
 * Initialise new game state
 * ---------------------------------------------------------------- */
void new_game(char* name, int sol_size) {
  int i;

  strcpy(gs.captain_name, name);
  gs.balance = DEBUG == 1? 100000: 500;
  gs.current_system = 85; 
  if (gs.current_system < 0) gs.current_system = 0;
  if (gs.current_system >= sol_size) gs.current_system = 0;

  gs.ship_type = 0;
  gs.tonnage = data_ship_tonnages[gs.ship_type];
  gs.current_cargo = 0;
  gs.hyper_class = 0;
  gs.upgrade_smuggler_bay = 0;
  gs.upgrade_continuous_jump = 0;
  gs.upgrade_emergency_jump = 0;
  gs.upgrade_objects_map = 0;
  gs.upgrade_political_map = 0;
  gs.reputation = 0;
  gs.missions_completed = 0;
  gs.fuel = 100;

  gs.quests_size = 0;

  gs.visited_bytes = (sol_size + 7) / 8;
  gs.visited = (unsigned char*)malloc(gs.visited_bytes);
  if (gs.visited) {
    memset(gs.visited, 0, gs.visited_bytes);
  }

  system_quests_size = 0;
  system_upgrades_size = 0;

  core_game_run_event();
  wp.size = 0;

  core_game_save("USER.SAV");

  /* Draw new game GUI */
  gui_map_nav_move_screen_to(sol_list, gs.current_system);
  gui_bars_map_bottom();
  gui_bars_common_top();
}

/* ----------------------------------------------------------------
 * Load and save game
 * ---------------------------------------------------------------- */
int core_game_load(char *filename)
{

    int result = 0;

    result = data_reader_load_game_file(&gs, filename);

    if (result == 1)
    {
        gui_map_nav_move_screen_to(sol_list, gs.current_system);
        core_game_run_event();
        wp.size = 0;
        /* Draw new game GUI */
        gui_map_nav_move_screen_to(sol_list, gs.current_system);
        gui_bars_map_bottom();
        gui_bars_common_top();
    }

    return result;
}

int core_game_save(char *filename)
{
    int result = 0;
    result = data_reader_save_game_file(&gs, filename);
    return result;
}


/* ----------------------------------------------------------------
 * Quests and NPC's
 * ---------------------------------------------------------------- */

static void core_game_gen_npc(NPC* ptr_npc, unsigned int faction, E_GENDER gender,
                       E_NPC_TYPE npc_type) {
  /* Name constants */
  const char* IRISH_MALE_FIRST[] = {
      LC_GEN_FNAME_MALE_IRISH_1,  LC_GEN_FNAME_MALE_IRISH_2,
      LC_GEN_FNAME_MALE_IRISH_3,  LC_GEN_FNAME_MALE_IRISH_4,
      LC_GEN_FNAME_MALE_IRISH_5,  LC_GEN_FNAME_MALE_IRISH_6,
      LC_GEN_FNAME_MALE_IRISH_7,  LC_GEN_FNAME_MALE_IRISH_8,
      LC_GEN_FNAME_MALE_IRISH_9,  LC_GEN_FNAME_MALE_IRISH_10,
      LC_GEN_FNAME_MALE_IRISH_11, LC_GEN_FNAME_MALE_IRISH_12,
      LC_GEN_FNAME_MALE_IRISH_13, LC_GEN_FNAME_MALE_IRISH_14,
      LC_GEN_FNAME_MALE_IRISH_15};

  const char* IRISH_FEMALE_FIRST[] = {
      LC_GEN_FNAME_FEMALE_IRISH_1,  LC_GEN_FNAME_FEMALE_IRISH_2,
      LC_GEN_FNAME_FEMALE_IRISH_3,  LC_GEN_FNAME_FEMALE_IRISH_4,
      LC_GEN_FNAME_FEMALE_IRISH_5,  LC_GEN_FNAME_FEMALE_IRISH_6,
      LC_GEN_FNAME_FEMALE_IRISH_7,  LC_GEN_FNAME_FEMALE_IRISH_8,
      LC_GEN_FNAME_FEMALE_IRISH_9,  LC_GEN_FNAME_FEMALE_IRISH_10,
      LC_GEN_FNAME_FEMALE_IRISH_11, LC_GEN_FNAME_FEMALE_IRISH_12,
      LC_GEN_FNAME_FEMALE_IRISH_13, LC_GEN_FNAME_FEMALE_IRISH_14,
      LC_GEN_FNAME_FEMALE_IRISH_15};

  const char* IRISH_LAST[] = {
      LC_GEN_LNAME_IRISH_1,  LC_GEN_LNAME_IRISH_2,  LC_GEN_LNAME_IRISH_3,
      LC_GEN_LNAME_IRISH_4,  LC_GEN_LNAME_IRISH_5,  LC_GEN_LNAME_IRISH_6,
      LC_GEN_LNAME_IRISH_7,  LC_GEN_LNAME_IRISH_8,  LC_GEN_LNAME_IRISH_9,
      LC_GEN_LNAME_IRISH_10, LC_GEN_LNAME_IRISH_11, LC_GEN_LNAME_IRISH_12,
      LC_GEN_LNAME_IRISH_13, LC_GEN_LNAME_IRISH_14, LC_GEN_LNAME_IRISH_15};

  const char* ARAB_MALE_FIRST[] = {
      LC_GEN_FNAME_MALE_ARAB_1,  LC_GEN_FNAME_MALE_ARAB_2,
      LC_GEN_FNAME_MALE_ARAB_3,  LC_GEN_FNAME_MALE_ARAB_4,
      LC_GEN_FNAME_MALE_ARAB_5,  LC_GEN_FNAME_MALE_ARAB_6,
      LC_GEN_FNAME_MALE_ARAB_7,  LC_GEN_FNAME_MALE_ARAB_8,
      LC_GEN_FNAME_MALE_ARAB_9,  LC_GEN_FNAME_MALE_ARAB_10,
      LC_GEN_FNAME_MALE_ARAB_11, LC_GEN_FNAME_MALE_ARAB_12,
      LC_GEN_FNAME_MALE_ARAB_13, LC_GEN_FNAME_MALE_ARAB_14,
      LC_GEN_FNAME_MALE_ARAB_15};

  const char* ARAB_FEMALE_FIRST[] = {
      LC_GEN_FNAME_FEMALE_ARAB_1,  LC_GEN_FNAME_FEMALE_ARAB_2,
      LC_GEN_FNAME_FEMALE_ARAB_3,  LC_GEN_FNAME_FEMALE_ARAB_4,
      LC_GEN_FNAME_FEMALE_ARAB_5,  LC_GEN_FNAME_FEMALE_ARAB_6,
      LC_GEN_FNAME_FEMALE_ARAB_7,  LC_GEN_FNAME_FEMALE_ARAB_8,
      LC_GEN_FNAME_FEMALE_ARAB_9,  LC_GEN_FNAME_FEMALE_ARAB_10,
      LC_GEN_FNAME_FEMALE_ARAB_11, LC_GEN_FNAME_FEMALE_ARAB_12,
      LC_GEN_FNAME_FEMALE_ARAB_13, LC_GEN_FNAME_FEMALE_ARAB_14,
      LC_GEN_FNAME_FEMALE_ARAB_15};

  const char* ARAB_LAST[] = {
      LC_GEN_LNAME_ARAB_1,  LC_GEN_LNAME_ARAB_2,  LC_GEN_LNAME_ARAB_3,
      LC_GEN_LNAME_ARAB_4,  LC_GEN_LNAME_ARAB_5,  LC_GEN_LNAME_ARAB_6,
      LC_GEN_LNAME_ARAB_7,  LC_GEN_LNAME_ARAB_8,  LC_GEN_LNAME_ARAB_9,
      LC_GEN_LNAME_ARAB_10, LC_GEN_LNAME_ARAB_11, LC_GEN_LNAME_ARAB_12,
      LC_GEN_LNAME_ARAB_13, LC_GEN_LNAME_ARAB_14, LC_GEN_LNAME_ARAB_15};

  const char* COMMON_MALE_FIRST[] = {
      LC_GEN_FNAME_MALE_COMMON_1,  LC_GEN_FNAME_MALE_COMMON_2,
      LC_GEN_FNAME_MALE_COMMON_3,  LC_GEN_FNAME_MALE_COMMON_4,
      LC_GEN_FNAME_MALE_COMMON_5,  LC_GEN_FNAME_MALE_COMMON_6,
      LC_GEN_FNAME_MALE_COMMON_7,  LC_GEN_FNAME_MALE_COMMON_8,
      LC_GEN_FNAME_MALE_COMMON_9,  LC_GEN_FNAME_MALE_COMMON_10,
      LC_GEN_FNAME_MALE_COMMON_11, LC_GEN_FNAME_MALE_COMMON_12,
      LC_GEN_FNAME_MALE_COMMON_13, LC_GEN_FNAME_MALE_COMMON_14,
      LC_GEN_FNAME_MALE_COMMON_15};

  const char* COMMON_FEMALE_FIRST[] = {
      LC_GEN_FNAME_FEMALE_COMMON_1,  LC_GEN_FNAME_FEMALE_COMMON_2,
      LC_GEN_FNAME_FEMALE_COMMON_3,  LC_GEN_FNAME_FEMALE_COMMON_4,
      LC_GEN_FNAME_FEMALE_COMMON_5,  LC_GEN_FNAME_FEMALE_COMMON_6,
      LC_GEN_FNAME_FEMALE_COMMON_7,  LC_GEN_FNAME_FEMALE_COMMON_8,
      LC_GEN_FNAME_FEMALE_COMMON_9,  LC_GEN_FNAME_FEMALE_COMMON_10,
      LC_GEN_FNAME_FEMALE_COMMON_11, LC_GEN_FNAME_FEMALE_COMMON_12,
      LC_GEN_FNAME_FEMALE_COMMON_13, LC_GEN_FNAME_FEMALE_COMMON_14,
      LC_GEN_FNAME_FEMALE_COMMON_15};

  const char* COMMON_LAST[] = {
      LC_GEN_LNAME_COMMON_1,  LC_GEN_LNAME_COMMON_2,  LC_GEN_LNAME_COMMON_3,
      LC_GEN_LNAME_COMMON_4,  LC_GEN_LNAME_COMMON_5,  LC_GEN_LNAME_COMMON_6,
      LC_GEN_LNAME_COMMON_7,  LC_GEN_LNAME_COMMON_8,  LC_GEN_LNAME_COMMON_9,
      LC_GEN_LNAME_COMMON_10, LC_GEN_LNAME_COMMON_11, LC_GEN_LNAME_COMMON_12,
      LC_GEN_LNAME_COMMON_13, LC_GEN_LNAME_COMMON_14, LC_GEN_LNAME_COMMON_15};

  const char GENDER_SYMBOL[] = {'M', 'F'};

  const char FACTION_SYMBOL[] = {'A', 'R', 'S', 'U'};

  int photo_id = 0;

  ptr_npc->faction = faction;

  if (gender == RANDOM_GENDER)
    ptr_npc->gender = rand() % 3 == 2 ? 1 : 0;
  else
    ptr_npc->gender = gender;

  if (ptr_npc->gender == 0)
    photo_id = (rand() % MALE_PORTRAITS_COUNT) + 1;
  else
    photo_id = (rand() % FEMALE_PORTRAITS_COUNT) + 1;

  if (npc_type == QUEST_NPC)
    sprintf(ptr_npc->photo, "NPC/%c%c%d.BMP", FACTION_SYMBOL[ptr_npc->faction],
            GENDER_SYMBOL[ptr_npc->gender], photo_id);
  else if (npc_type == GAS_NPC)
    sprintf(ptr_npc->photo, "NPC/GAS%c.BMP", FACTION_SYMBOL[ptr_npc->faction]);

  switch (faction) {
    case 0:
      if (ptr_npc->gender == 0)
        sprintf(ptr_npc->name, "%s %s", ARAB_MALE_FIRST[rand() % N_SIZE],
                ARAB_LAST[rand() % N_SIZE]);
      else
        sprintf(ptr_npc->name, "%s %s", ARAB_FEMALE_FIRST[rand() % N_SIZE],
                ARAB_LAST[rand() % N_SIZE]);
      break;

    case 1:
      if (ptr_npc->gender == 0)
        sprintf(ptr_npc->name, "%s %s", IRISH_MALE_FIRST[rand() % N_SIZE],
                IRISH_LAST[rand() % N_SIZE]);
      else
        sprintf(ptr_npc->name, "%s %s", IRISH_FEMALE_FIRST[rand() % N_SIZE],
                IRISH_LAST[rand() % N_SIZE]);
      break;

    case 2:
    case 3:
      if (ptr_npc->gender == 0)
        sprintf(ptr_npc->name, "%s %s", COMMON_MALE_FIRST[rand() % N_SIZE],
                COMMON_LAST[rand() % N_SIZE]);
      else
        sprintf(ptr_npc->name, "%s %s", COMMON_FEMALE_FIRST[rand() % N_SIZE],
                COMMON_LAST[rand() % N_SIZE]);
      break;
  }
}

/* ----------------------------------------------------------------
 * Quests generator
 * ---------------------------------------------------------------- */

static int pick_target_system_by_jumps(int current, int min_jumps, int max_jumps) {
    int candidates[1000];
    int count = 0;
    int i, jumps;

    for (i = 0; i < sol_size; i++) {
        gui_progress_wnd(&map_wnd, "GS-CARD 1.5", "Generating Quests", i, sol_size);
        
        if (i == current) continue;
        jumps = core_finder_get_jumps(current, i);
        if (jumps == 0) continue;
        if (jumps >= min_jumps && jumps <= max_jumps) {
            candidates[count++] = i;
        }
    }

    if (count == 0) {
        /* fallback: any reachable system */
        for (i = 0; i < sol_size; i++) {
            if (i != current && core_finder_get_jumps(current, i) > 0) {
                candidates[count++] = i;
            }
        }
        if (count == 0) return -1;
    }

    return candidates[rand() % count];
}


static int generate_cargo(void) {
  double r = (double)rand() / RAND_MAX;
  if (r < 0.70) {
    return 1 + rand() % 50;
  } else if (r < 0.90) {
    return 51 + rand() % 100;
  } else {
    return 151 + rand() % 250;
  }
}

static long calc_reward(int type, int cargo, int jumps) {
    long base;
    double cargo_factor;
    double jump_factor;

    switch (type) {
        case 1:
            base = 100 + rand() % 201;
            break;
        case 2:
            base = 250 + rand() % 251;
            break;
        case 3:
            base = 400 + rand() % 401;
            break;
        case 4:
        case 5:
            base = 200 + rand() % 251;
            break;
        default:
            base = 100;
            break;
    }

    if (type <= 3) {
        cargo_factor = 1.0 + (double)cargo / 200.0;
        jump_factor = 1.0 + (double)jumps / 10.0;
        return (long)(base * cargo_factor * jump_factor);
    } else {
        jump_factor = 1.0 + (double)jumps / 10.0;
        return (long)(base * jump_factor);
    }
}

void core_game_gen_quest(QUEST* ptr_quest, int player_rep,
                         unsigned int faction) {
    int type, target, cargo, jumps, reward, penalty, r;

    if (faction == 3) {
        system_quests_size = 0;
        return;
    }

    r = rand() % 100;
    if (r < 50)
        type = 1;
    else if (r < 75)
        type = 2;
    else if (r < 90)
        type = 3;
    else if (r < 95)
        type = 4;
    else
        type = 5;

    if (type == 1)
        target = pick_target_system_by_jumps(gs.current_system, 1, 5);
    else if (type == 2)
        target = pick_target_system_by_jumps(gs.current_system, 6, 10);
    else  /* type 3, 4, 5 – дальняя */
        target = pick_target_system_by_jumps(gs.current_system, 11, 999);

    if (target == -1) {
        /* совсем нет подходящих – выбираем любую достижимую */
        target = pick_target_system_by_jumps(gs.current_system, 1, 999);
        if (target == -1) {
            system_quests_size = 0;
            return;
        }
    }

    if (type <= 3)
        cargo = generate_cargo();
    else
        cargo = 1;

    jumps = core_finder_get_jumps(gs.current_system, target);
    if (jumps < 1) jumps = 1;

    reward = calc_reward(type, cargo, jumps);

    penalty = (long)(reward * (0.5 - player_rep * 0.0004));
    if (penalty < (long)(reward * 0.1)) penalty = (int)(reward * 0.1);
    if (penalty < 0) penalty = 0;

    core_game_gen_npc(&ptr_quest->giver, faction, RANDOM_GENDER, QUEST_NPC);

    ptr_quest->reward = reward;
    ptr_quest->penalty = penalty;
    ptr_quest->target_system = target;
    ptr_quest->target_sector = sol_list[target].sector;
    ptr_quest->cargo = cargo;
    ptr_quest->type = type;
    ptr_quest->jumps = jumps;
}

int core_game_accept_quest(unsigned int index) {
  int i;

  if (gs.quests_size >= 5) {
    return 0;
  }
  if (index >= system_quests_size) {
    return 0;
  }

  if (gs.current_cargo + system_quests[index].cargo > gs.tonnage) return 0;

  gs.quests[gs.quests_size] = system_quests[index];
  gs.quests_size++;

  gs.current_cargo += system_quests[index].cargo;

  for (i = index; i < system_quests_size - 1; i++) {
    system_quests[i] = system_quests[i + 1];
  }

  system_quests_size = system_quests_size - 1;

  return 1;
}

static int core_game_event_quest_done() {
  int i, j;
  char lines[2][100];

  for (i = 0; i < gs.quests_size; i++) {
    if (gs.quests[i].target_system == gs.current_system) {
      /* Set quest done */
      gs.current_cargo -= gs.quests[i].cargo;
      gs.balance += gs.quests[i].reward;

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
                  LC_QUEST_COMPLETE_HEAD, lines, 2, NULL, 0);

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
 * Game Events System
 * ---------------------------------------------------------------- */
static int core_game_check_fuel_gone() {
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
    sprintf(lines[4], LC_GAME_OVER_STATS_TEXT_1, *gs.visited);
    sprintf(lines[5], LC_GAME_OVER_STATS_TEXT_2, gs.missions_completed);
    sprintf(lines[6], LC_GAME_OVER_STATS_TEXT_3, gs.balance);
    sprintf(lines[7], "   ");
    sprintf(lines[8], LC_GAME_OVER_STATS_TEXT_4);
    gui_dialog_wnd(&map_wnd, LC_GAME_OVER_HEAD, LC_GAME_OVER_HEAD, NULL, lines,
                   9, NULL, 0, SOUND_ERROR);

    return 1;
  }
  return 0;
}

void core_game_event_gas_station() {
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
                  LC_GAME_GAS_STATION_HEAD, lines, 3, NULL, 0);

    } else {
      sprintf(lines[2], LC_GAME_GAS_STATION_TEXT_3, total);
      choice = gui_npc_wnd(&map_wnd, &gas_worker, NPC_CHOICE_WND,
                           LC_GAME_GAS_STATION_HEAD, lines, 3, buttons, 2);

      if (choice == 0) {
        gs.fuel = 100;
        gs.balance -= total;
      }
    }
  }
}

static void core_game_gen_shipyard(void) {
    int i, count = 0, faction;

    faction = sol_list[gs.current_system].faction;
    system_shipyard_size = 0;

    if (!sol_list[gs.current_system].is_shipyard) {
      return;
    }

    for (i=0; i < SHIP_COUNT; i++){
        /* Almighty's vessel only cor CSU */
        if (i == 1 && faction != 0)
            continue;

        /* Ga-Bolg only for IMC */
        if (i == 3 && faction != 1)
            continue;

        /* NOVA and Sashok only for Sentinels */
        if ((i == 4 || i == 5) && faction != 2)
            continue;

        system_shipyard[count].name = data_ship_names[i];
        system_shipyard[count].base_price = ship_prices[i];

        /* Preinstalled upgrades */
        system_shipyard[count].hyper_class = data_ship_engines[i];
        system_shipyard[count].upgrade_smuggler_bay = data_ship_smuggler_bay[i];
        system_shipyard[count].upgrade_continuous_jump = data_ship_continuous_jump[i];
        system_shipyard[count].upgrade_emergency_jump = data_ship_emergency_jump[i];
        system_shipyard[count].id = i;

        core_game_gen_npc(&system_shipyard[count].giver, faction, RANDOM_GENDER, QUEST_NPC);
        count++;
    }

    system_shipyard_size = count;
}

static void core_game_gen_upgrades(void) {
  int i, count = 0;

  if (!sol_list[gs.current_system].is_shipyard) {
    system_upgrades_size = 0;
    return;
  }

  /* hiper drives for sale */
  if (gs.hyper_class < 3) {
    system_upgrades[count].name = data_hyper_names[gs.hyper_class + 1];
    system_upgrades[count].base_price = hyper_prices[gs.hyper_class + 1];
    system_upgrades[count].type = 0;
    system_upgrades[count].id = gs.hyper_class + 1;
    core_game_gen_npc(&system_upgrades[count].giver, sol_list[gs.current_system].faction,
                    RANDOM_GENDER, QUEST_NPC);
    count++;
  }

  /* custom upgrades */
  {
    static char* names[] = {LC_UPGRADE_SMUGGLER_BAY,
                            LC_UPGRADE_CONTIN_JUMP_SYSTEM,
                            LC_UPGRADE_EMERGENCY_JUMP_SYSTEM,
                            LC_UPGRADE_OBJECTS_MAP, LC_UPGRADE_POLITICAL_MAP};
    unsigned char* flags[] = {
        &gs.upgrade_smuggler_bay, &gs.upgrade_continuous_jump,
        &gs.upgrade_emergency_jump, &gs.upgrade_objects_map,
        &gs.upgrade_political_map};

    for (i = 0; i < 5; i++) {
      if (*(flags[i]) == 0 && (rand() % 100) < 70) {
        /* No smuggler bay in Sentinel system */
        if (sol_list[gs.current_system].faction == 2 && i == 0)
            continue;

        /* No emergency jump in Irish systems, yo-ho-ho */
        if (sol_list[gs.current_system].faction == 0 && i == 2)
            continue;

        system_upgrades[count].name = names[i];
        system_upgrades[count].base_price = custom_prices[i];
        system_upgrades[count].type = 1;
        system_upgrades[count].id = i;
        core_game_gen_npc(&system_upgrades[count].giver, sol_list[gs.current_system].faction,
                    RANDOM_GENDER, QUEST_NPC);
        count++;
      }
    }
  }

  system_upgrades_size = count; 
}


int core_game_run_event() {
  int i = 0, game_over = 0;
  char buf[50];

  game_mark_visited(&gs, gs.current_system);
  gs.fuel -= data_hyper_fuel[gs.hyper_class];

  game_over = core_game_check_fuel_gone();

  if (!game_over){
      system_quests_size = 5;
      /* System quests generator */
      for (i = 0; i < system_quests_size; i++) {
        core_game_gen_quest(&system_quests[i], gs.reputation,
                            sol_list[gs.current_system].faction);
      }

      for (i = 0; i < gs.quests_size; i++)
      {
        gui_progress_wnd(&map_wnd, "GS-CARD 1.5", "Updating Quests", i, gs.quests_size);
        gs.quests[i].jumps = core_finder_get_jumps(gs.current_system, gs.quests[i].target_system);
      }

      /* System upgrades market generator */
      core_game_gen_upgrades();

      core_game_gen_shipyard();

      core_game_event_quest_done();
  }

  return game_over;
}

void core_game_event_quest_failed(int index)
{

}

void core_game_event_hijack()
{

}

void core_game_event_customs()
{

}

void core_game_event_kidnapping()
{

}

void core_game_danger_object()
{
    
}
