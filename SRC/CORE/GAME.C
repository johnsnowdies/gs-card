#include <stdlib.h>
#include <math.h>

#include "data/structs.h"
#include "data/reader.h"

#include "core/objects.h"
#include "core/finder.h"
#include "core/game.h"


#include "ui/locale.h" 

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern GAME_STATE gs;
extern unsigned int system_quests_size;
extern QUEST system_quests[5];
extern WAYPOINT wp;

extern SYSTEM* sol_list;
extern unsigned int sol_size;

extern OBJECT* obj_list;
extern unsigned int obj_size;

#define N_SIZE 15

/* ----------------------------------------------------------------
 * Game Data Structures
 * ---------------------------------------------------------------- */


char* data_ship_names[SHIP_COUNT] = {
    LC_GAME_SHIP_1,
    LC_GAME_SHIP_2,
    LC_GAME_SHIP_3,
    LC_GAME_SHIP_4,
    LC_GAME_SHIP_5,
    LC_GAME_SHIP_6
};

unsigned int data_ship_tonnages[SHIP_COUNT] = {
    50, 80, 100, 150, 200, 400
};

char* data_hyper_names[HYPER_COUNT] = {
    LC_GAME_ENGINE_1,
    LC_GAME_ENGINE_2,
    LC_GAME_ENGINE_3,
    LC_GAME_ENGINE_4
};

unsigned int data_hyper_fuel[HYPER_COUNT] = {
    10, 8, 5, 2
};

char* data_factions[FACTIONS_COUNT] = {
    LC_GAME_FACTION_1,
    LC_GAME_FACTION_2,
    LC_GAME_FACTION_3,
    LC_GAME_FACTION_4
};

unsigned int data_factions_colors[FACTIONS_COUNT] = {
    2, 14, 9, 4
};

char* data_sectors[SECTORS_COUNT] = {
    LC_GAME_SECTOR_1,
    LC_GAME_SECTOR_2,
    LC_GAME_SECTOR_3,
    LC_GAME_SECTOR_4,
    LC_GAME_SECTOR_5,
    LC_GAME_SECTOR_6,
    LC_GAME_SECTOR_7,
    LC_GAME_SECTOR_8,
    LC_GAME_SECTOR_9
};


char* QUEST_TYPES[] = {
    "",
    LC_QUEST_TYPE_1,
    LC_QUEST_TYPE_2,
    LC_QUEST_TYPE_3,
    LC_QUEST_TYPE_4,
    LC_QUEST_TYPE_5
};

void game_mark_visited(GAME_STATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return;
    gs->visited[system >> 3] |= (1 << (system & 7));
}

int game_is_visited(GAME_STATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return 0;
    return (gs->visited[system >> 3] >> (system & 7)) & 1;
}



/* ----------------------------------------------------------------
 * new_game -- initialise game state
 * ---------------------------------------------------------------- */
void new_game(char *name, int sol_size)
{
    int i;

    strcpy(gs.captain_name, name);
    gs.balance            = 100;
    gs.current_system     = 87; /*rand() % sol_size;*/
    if (gs.current_system < 0) gs.current_system = 0;
    if (gs.current_system >= sol_size) gs.current_system = 0;

    gs.ship_type         = 0;
    gs.tonnage           = 50;
    gs.current_cargo     = 0;
    gs.cargo_value       = 0;
    gs.hyper_class       = 0;
    gs.smuggler_bay      = 0;
    gs.reputation        = 0;
    gs.missions_completed = 0;
    gs.fuel              = 100;

    gs.quests_size = 0;

    gs.visited_bytes = (sol_size + 7) / 8;
    gs.visited = (unsigned char*)malloc(gs.visited_bytes);
    if (gs.visited) {
        memset(gs.visited, 0, gs.visited_bytes);
    }

    /* Load ads, calculate hyper-threads, load objects */
    core_finder_calc_hyper_threads();
    obj_size = data_reader_load_objects(&obj_list);

    gui_map_nav_move_screen_to(sol_list, gs.current_system);
    gui_map_bottom_status_line();
    gui_map_top_status_line();

    game_mark_visited(&gs, gs.current_system);
    core_game_run_event();

    wp.size = 0;

    system_quests_size = 0;

    core_game_save("USER.SAV");
}

/* ----------------------------------------------------------------
 * load_game -- initialise game state from file
 * ---------------------------------------------------------------- */
int core_game_load(char *filename)
{

    int result = 0;
    char* debug_buf;
   

    result = data_reader_load_game_file(&gs, filename);

    sprintf(debug_buf, "Result: %d Filename [%s]", result, filename );


     clrscr();
    setcolor(15);
    outtextxy(0,0, debug_buf);
        getch();

    if (result == 1)
    {
        /* Load ads, calculate hyper-threads, load objects */
        core_finder_calc_hyper_threads();
        obj_size = data_reader_load_objects(&obj_list);

        gui_map_nav_move_screen_to(sol_list, gs.current_system);
        core_game_run_event();

        wp.size = 0;
    }


    return result;
}

int core_game_save(char *filename)
{
    int result = 0;
    result = data_reader_save_game_file(&gs, filename);
    return result;
}


/*
 * Quests
 */

void core_game_gen_npc(NPC* npc_ptr, unsigned int faction)
{
/*
    * Name constants
    */

    const char* IRISH_MALE_FIRST[] = {
        LC_GEN_FNAME_MALE_IRISH_1,
        LC_GEN_FNAME_MALE_IRISH_2,
        LC_GEN_FNAME_MALE_IRISH_3,
        LC_GEN_FNAME_MALE_IRISH_4,
        LC_GEN_FNAME_MALE_IRISH_5,
        LC_GEN_FNAME_MALE_IRISH_6,
        LC_GEN_FNAME_MALE_IRISH_7,
        LC_GEN_FNAME_MALE_IRISH_8,
        LC_GEN_FNAME_MALE_IRISH_9,
        LC_GEN_FNAME_MALE_IRISH_10,
        LC_GEN_FNAME_MALE_IRISH_11,
        LC_GEN_FNAME_MALE_IRISH_12,
        LC_GEN_FNAME_MALE_IRISH_13,
        LC_GEN_FNAME_MALE_IRISH_14,
        LC_GEN_FNAME_MALE_IRISH_15
    };

    const char* IRISH_FEMALE_FIRST[] = {
        LC_GEN_FNAME_FEMALE_IRISH_1,
        LC_GEN_FNAME_FEMALE_IRISH_2,
        LC_GEN_FNAME_FEMALE_IRISH_3,
        LC_GEN_FNAME_FEMALE_IRISH_4,
        LC_GEN_FNAME_FEMALE_IRISH_5,
        LC_GEN_FNAME_FEMALE_IRISH_6,
        LC_GEN_FNAME_FEMALE_IRISH_7,
        LC_GEN_FNAME_FEMALE_IRISH_8,
        LC_GEN_FNAME_FEMALE_IRISH_9,
        LC_GEN_FNAME_FEMALE_IRISH_10,
        LC_GEN_FNAME_FEMALE_IRISH_11,
        LC_GEN_FNAME_FEMALE_IRISH_12,
        LC_GEN_FNAME_FEMALE_IRISH_13,
        LC_GEN_FNAME_FEMALE_IRISH_14,
        LC_GEN_FNAME_FEMALE_IRISH_15
    };

    const char* IRISH_LAST[] = {
        LC_GEN_LNAME_IRISH_1,
        LC_GEN_LNAME_IRISH_2,
        LC_GEN_LNAME_IRISH_3,
        LC_GEN_LNAME_IRISH_4,
        LC_GEN_LNAME_IRISH_5,
        LC_GEN_LNAME_IRISH_6,
        LC_GEN_LNAME_IRISH_7,
        LC_GEN_LNAME_IRISH_8,
        LC_GEN_LNAME_IRISH_9,
        LC_GEN_LNAME_IRISH_10,
        LC_GEN_LNAME_IRISH_11,
        LC_GEN_LNAME_IRISH_12,
        LC_GEN_LNAME_IRISH_13,
        LC_GEN_LNAME_IRISH_14,
        LC_GEN_LNAME_IRISH_15
    };


    const char* ARAB_MALE_FIRST[] = {
        LC_GEN_FNAME_MALE_ARAB_1,
        LC_GEN_FNAME_MALE_ARAB_2,
        LC_GEN_FNAME_MALE_ARAB_3,
        LC_GEN_FNAME_MALE_ARAB_4,
        LC_GEN_FNAME_MALE_ARAB_5,
        LC_GEN_FNAME_MALE_ARAB_6,
        LC_GEN_FNAME_MALE_ARAB_7,
        LC_GEN_FNAME_MALE_ARAB_8,
        LC_GEN_FNAME_MALE_ARAB_9,
        LC_GEN_FNAME_MALE_ARAB_10,
        LC_GEN_FNAME_MALE_ARAB_11,
        LC_GEN_FNAME_MALE_ARAB_12,
        LC_GEN_FNAME_MALE_ARAB_13,
        LC_GEN_FNAME_MALE_ARAB_14,
        LC_GEN_FNAME_MALE_ARAB_15

    };

    const char* ARAB_FEMALE_FIRST[] = {
        LC_GEN_FNAME_FEMALE_ARAB_1,
        LC_GEN_FNAME_FEMALE_ARAB_2,
        LC_GEN_FNAME_FEMALE_ARAB_3,
        LC_GEN_FNAME_FEMALE_ARAB_4,
        LC_GEN_FNAME_FEMALE_ARAB_5,
        LC_GEN_FNAME_FEMALE_ARAB_6,
        LC_GEN_FNAME_FEMALE_ARAB_7,
        LC_GEN_FNAME_FEMALE_ARAB_8,
        LC_GEN_FNAME_FEMALE_ARAB_9,
        LC_GEN_FNAME_FEMALE_ARAB_10,
        LC_GEN_FNAME_FEMALE_ARAB_11,
        LC_GEN_FNAME_FEMALE_ARAB_12,
        LC_GEN_FNAME_FEMALE_ARAB_13,
        LC_GEN_FNAME_FEMALE_ARAB_14,
        LC_GEN_FNAME_FEMALE_ARAB_15

    };


    const char* ARAB_LAST[] = {
        LC_GEN_LNAME_ARAB_1,
        LC_GEN_LNAME_ARAB_2,
        LC_GEN_LNAME_ARAB_3,
        LC_GEN_LNAME_ARAB_4,
        LC_GEN_LNAME_ARAB_5,
        LC_GEN_LNAME_ARAB_6,
        LC_GEN_LNAME_ARAB_7,
        LC_GEN_LNAME_ARAB_8,
        LC_GEN_LNAME_ARAB_9,
        LC_GEN_LNAME_ARAB_10,
        LC_GEN_LNAME_ARAB_11,
        LC_GEN_LNAME_ARAB_12,
        LC_GEN_LNAME_ARAB_13,
        LC_GEN_LNAME_ARAB_14,
        LC_GEN_LNAME_ARAB_15
    };

    const char* COMMON_MALE_FIRST[] = {
        LC_GEN_FNAME_MALE_COMMON_1,
        LC_GEN_FNAME_MALE_COMMON_2,
        LC_GEN_FNAME_MALE_COMMON_3,
        LC_GEN_FNAME_MALE_COMMON_4,
        LC_GEN_FNAME_MALE_COMMON_5,
        LC_GEN_FNAME_MALE_COMMON_6,
        LC_GEN_FNAME_MALE_COMMON_7,
        LC_GEN_FNAME_MALE_COMMON_8,
        LC_GEN_FNAME_MALE_COMMON_9,
        LC_GEN_FNAME_MALE_COMMON_10,
        LC_GEN_FNAME_MALE_COMMON_11,
        LC_GEN_FNAME_MALE_COMMON_12,
        LC_GEN_FNAME_MALE_COMMON_13,
        LC_GEN_FNAME_MALE_COMMON_14,
        LC_GEN_FNAME_MALE_COMMON_15
    };

    const char* COMMON_FEMALE_FIRST[] = {
        LC_GEN_FNAME_FEMALE_COMMON_1,
        LC_GEN_FNAME_FEMALE_COMMON_2,
        LC_GEN_FNAME_FEMALE_COMMON_3,
        LC_GEN_FNAME_FEMALE_COMMON_4,
        LC_GEN_FNAME_FEMALE_COMMON_5,
        LC_GEN_FNAME_FEMALE_COMMON_6,
        LC_GEN_FNAME_FEMALE_COMMON_7,
        LC_GEN_FNAME_FEMALE_COMMON_8,
        LC_GEN_FNAME_FEMALE_COMMON_9,
        LC_GEN_FNAME_FEMALE_COMMON_10,
        LC_GEN_FNAME_FEMALE_COMMON_11,
        LC_GEN_FNAME_FEMALE_COMMON_12,
        LC_GEN_FNAME_FEMALE_COMMON_13,
        LC_GEN_FNAME_FEMALE_COMMON_14,
        LC_GEN_FNAME_FEMALE_COMMON_15
    };

    const char* COMMON_LAST[] = {
        LC_GEN_LNAME_COMMON_1,
        LC_GEN_LNAME_COMMON_2,
        LC_GEN_LNAME_COMMON_3,
        LC_GEN_LNAME_COMMON_4,
        LC_GEN_LNAME_COMMON_5,
        LC_GEN_LNAME_COMMON_6,
        LC_GEN_LNAME_COMMON_7,
        LC_GEN_LNAME_COMMON_8,
        LC_GEN_LNAME_COMMON_9,
        LC_GEN_LNAME_COMMON_10,
        LC_GEN_LNAME_COMMON_11,
        LC_GEN_LNAME_COMMON_12,
        LC_GEN_LNAME_COMMON_13,
        LC_GEN_LNAME_COMMON_14,
        LC_GEN_LNAME_COMMON_15
    };

    npc_ptr->faction = faction;
        /* 0 - Male 1 - Female */
    npc_ptr->gender = rand() % 3 == 2 ? 1: 0;

    if (npc_ptr->gender == 0)
        npc_ptr->portrait = rand() % 6;
    else
        npc_ptr->portrait = rand() % 3;
    

    switch(faction){
        case 1:
        case 3:
            if (npc_ptr->gender == 0)
                sprintf(npc_ptr->name, "%s %s", IRISH_MALE_FIRST[rand()%N_SIZE], IRISH_LAST[rand()%N_SIZE]);
            else
                sprintf(npc_ptr->name, "%s %s", IRISH_FEMALE_FIRST[rand()%N_SIZE], IRISH_LAST[rand()%N_SIZE]);
        break;

        case 0:
            if (npc_ptr->gender == 0)
                sprintf(npc_ptr->name, "%s %s", ARAB_MALE_FIRST[rand()%N_SIZE], ARAB_LAST[rand()%N_SIZE]);
            else
                sprintf(npc_ptr->name, "%s %s", ARAB_FEMALE_FIRST[rand()%N_SIZE], ARAB_LAST[rand()%N_SIZE]);
        break;

        default:
            if (npc_ptr->gender == 0)
                sprintf(npc_ptr->name, "%s %s", COMMON_MALE_FIRST[rand()%N_SIZE], COMMON_LAST[rand()%N_SIZE]);
            else
                sprintf(npc_ptr->name, "%s %s", COMMON_FEMALE_FIRST[rand()%N_SIZE], COMMON_LAST[rand()%N_SIZE]);
        break;
    }
}

static int pick_target_system(int current, int min_dist, int max_dist) {
    int candidates[1000]; 
    int count = 0, d = 0, i;
    int* distances = (int*)malloc(sol_size * sizeof(int));
    if (!distances) return -1;

    core_finder_calc_distances(current, distances, 999);

    for (i = 0; i < sol_size; i++) {
        if (i == current) continue;
        d = distances[i];
        if (d >= min_dist && d <= max_dist) {
            candidates[count++] = i;
        }
    }
    free(distances);

    if (count == 0) {
        for (i = 0; i < sol_size; i++) {
            if (i != current) candidates[count++] = i;
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


static int calc_reward(int type, int cargo, int distance) {
    int base;
    switch (type) {
        case 1: base = 100 + rand() % 201; break;  
        case 2: base = 250 + rand() % 251; break;  
        case 3: base = 400 + rand() % 401; break; 
        case 4: 
        case 5: base = 200 + rand() % 251; break; 
        default: base = 100; break;
    }

    if (type <= 3) {
        double cargo_factor = 1.0 + (double)cargo / 200.0;
        double dist_factor = 1.0 + (double)distance / 20.0;
        return (int)(base * cargo_factor * dist_factor);
    } else {
        double dist_factor = 1.0 + (double)distance / 20.0;
        return (int)(base * dist_factor);
    }
}

void core_game_gen_quest(QUEST* quest_ptr, int player_rep, unsigned int faction)
{
    int type, target, cargo, distance, reward, penalty, r;
    int* dist_arr;

    r = rand() % 100;
    if (r < 50) type = 1;
    else if (r < 75) type = 2;
    else if (r < 90) type = 3;
    else if (r < 95) type = 4;
    else type = 5;

    if (type == 1)
        target = pick_target_system(gs.current_system, 1, 10);
    else if (type == 2)
        target = pick_target_system(gs.current_system, 11, 50);
    else
        target = pick_target_system(gs.current_system, 1, 50);

    if (type <= 3) 
        cargo = generate_cargo();
    else 
        cargo = 1;

    dist_arr = (int*)malloc(sol_size * sizeof(int));
    if (dist_arr) {
        core_finder_calc_distances(gs.current_system, dist_arr, 50);
        distance = dist_arr[target];
        free(dist_arr);
    } else {
        distance = 1;
    }
    if (distance < 1) distance = 1;

    reward = calc_reward(type, cargo, distance);

    penalty = (int)(reward * (0.5 - player_rep * 0.0004));
    if (penalty < (int)(reward * 0.1)) penalty = (int)(reward * 0.1);
    if (penalty < 0) penalty = 0;

    core_game_gen_npc(&quest_ptr->giver, faction);

    quest_ptr->reward = reward;
    quest_ptr->penalty = penalty;
    quest_ptr->target_system = target;
    quest_ptr->target_sector = sol_list[target].sector;
    quest_ptr->cargo = cargo;
    quest_ptr->type = type;

}

int core_game_accept_quest(unsigned int index)
{
    int i;

    if (gs.quests_size >= 5) {
        return 0;
    }
    if (index >= system_quests_size) {
        return 0;
    }

    gs.quests[gs.quests_size] = system_quests[index];
    gs.quests_size++;

    for (i = index; i < system_quests_size - 1; i++) {
        system_quests[i] = system_quests[i + 1];
    }

    system_quests_size--;

    return 1;
}


/*
 * Arriving to new system events
 */

void core_game_run_event()
{
    int i = 0;
    char buf[50];
    game_mark_visited(&gs, gs.current_system);
    gs.fuel -= data_hyper_fuel[gs.hyper_class];

    system_quests_size = 5;
    /* System quests generator */
    for (i = 0; i < 5; i++) {
        core_game_gen_quest(&system_quests[i], gs.reputation, sol_list[gs.current_system].faction);
    }
}
