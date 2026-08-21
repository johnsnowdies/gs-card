#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "data/structs.h"
#include "core/objects.h"
#include "core/finder.h"
#include "ui/locale.h" 
#include "core/game.h"       

extern SYSTEM* sol_list;
extern int sol_size;
extern struct game_state gs;
extern char* data_sectors[SECTORS_COUNT];
/*
 * Name constants
 */

#define N_SIZE 15

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

/*
 * Quests
 */

void core_game_gen_npc(NPC* npc_ptr, int faction)
{
    int gender = (rand() % 3 == 0)? 0: 1;

    npc_ptr->faction = faction;
    npc_ptr->portrait = 0;

    switch(faction){
        case 0:
        case 3:
            if (gender == 0){
                sprintf(npc_ptr->name, "%s %s", IRISH_FEMALE_FIRST[rand()%15], IRISH_LAST[rand()%15]);
            }else{
                sprintf(npc_ptr->name, "%s %s", IRISH_MALE_FIRST[rand()%15], IRISH_LAST[rand()%15]);
            }
        break;

        case 1:
            if (gender == 0){
                sprintf(npc_ptr->name, "%s %s", ARAB_FEMALE_FIRST[rand()%15], ARAB_LAST[rand()%15]);
            }else{
                sprintf(npc_ptr->name, "%s %s", ARAB_MALE_FIRST[rand()%15], ARAB_LAST[rand()%15]);
            }
        break;

        default:
            if (gender == 0){
                sprintf(npc_ptr->name, "%s %s", COMMON_FEMALE_FIRST[rand()%15], COMMON_LAST[rand()%15]);
            }else{
                sprintf(npc_ptr->name, "%s %s", COMMON_MALE_FIRST[rand()%15], COMMON_LAST[rand()%15]);
            }
        break;
    }
}

/*


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

   
}*/
