#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "data/structs.h"
#include "core/objects.h"
#include "core/finder.h"
#include "ui/locale.h" 
#include "core/game.h"       

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern SYSTEM* sol_list;
extern int sol_size;
extern struct game_state gs;
extern char* data_sectors[SECTORS_COUNT];

extern QUEST system_quests[5];

#define N_SIZE 10


char* QUEST_TYPES[] = {
    "",
    LC_QUEST_TYPE_1,
    LC_QUEST_TYPE_2,
    LC_QUEST_TYPE_3,
    LC_QUEST_TYPE_4,
    LC_QUEST_TYPE_5
};

/*
 * Quests
 */

void core_game_gen_npc(NPC* npc_ptr, int faction)
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
        LC_GEN_FNAME_MALE_IRISH_10
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
        LC_GEN_LNAME_IRISH_10
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
        LC_GEN_FNAME_MALE_ARAB_10
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
        LC_GEN_LNAME_ARAB_10
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
        LC_GEN_FNAME_MALE_COMMON_10
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
        LC_GEN_LNAME_COMMON_10
    };

    npc_ptr->faction = faction;
    npc_ptr->portrait = 0;

    switch(faction){
        case 1:
        case 3:
            sprintf(npc_ptr->name, "%s %s", IRISH_MALE_FIRST[rand()%15], IRISH_LAST[rand()%15]);
        break;

        case 0:
            sprintf(npc_ptr->name, "%s %s", ARAB_MALE_FIRST[rand()%15], ARAB_LAST[rand()%15]);
        break;

        default:
            sprintf(npc_ptr->name, "%s %s", COMMON_MALE_FIRST[rand()%15], COMMON_LAST[rand()%15]);
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

void core_game_gen_quest(QUEST* quest_ptr, int player_rep, SYSTEM* current_system)
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
        core_finder_calc_distances(gs.current_system, dist_arr, 999);
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

    quest_ptr->giver = (NPC*)malloc(sizeof(NPC));
    core_game_gen_npc(quest_ptr->giver, current_system->faction);

    quest_ptr->reward = reward;
    quest_ptr->penalty = penalty;
    quest_ptr->target_system = target;
    quest_ptr->target_sector = sol_list[target].sector;
    quest_ptr->cargo = cargo;
    quest_ptr->type = type;

}


/*
 * Arriving to new system events
 */

void core_game_run_event()
{
    int i = 0;
    /* System quests generator */
    for (i = 0; i < 5; i++) {
        
        core_game_gen_quest(&system_quests[i], gs.reputation, &sol_list[gs.current_system]);
    }
}