#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data\structs.h"
#include "data\reader.h"

int loadObjects(OBJECT** list) {
  FILE* fp;
  char buf[100];
  int count = 0, counter = 0;

  if ((fp = fopen("objects.sol", "r")) == NULL)
    return 0; /* no objects -- not an error */

  /* first pass: count lines */
  while (fgets(buf, sizeof buf, fp) != NULL) count++;

  if (count == 0) {
    fclose(fp);
    return 0;
  }

  *list = (OBJECT*)calloc(count, sizeof(OBJECT));
  if (*list == NULL) {
    fclose(fp);
    return 0;
  }

  /* second pass: parse */
  fseek(fp, 0L, SEEK_SET);
  while (fgets(buf, sizeof buf, fp) != NULL) {
    int v[5] = {0, 0, 0, 0, 0};
    int vi = 0, i, j = 0;
    char num[10] = "";

    for (i = 0; buf[i] != '\0' && buf[i] != '\n' && buf[i] != '\r' && vi < 5;
         i++) {
      if (buf[i] == ';') {
        v[vi++] = atoi(num);
        j = 0;
        num[0] = '\0';
      } else {
        num[j++] = buf[i];
        num[j] = '\0';
      }
    }
    if (vi < 4) continue; /* malformed line */
    if (j > 0) v[vi] = atoi(num);

    (*list)[counter].x = v[0];
    (*list)[counter].y = v[1];
    (*list)[counter].z = v[2];
    (*list)[counter].r = v[3];
    (*list)[counter].type = (vi >= 4) ? v[4] : 0;
    counter++;
  }

  fclose(fp);
  return counter;
}

int loadSolarFile(SYSTEM** list) {
  FILE* fp;
  char buf[100];
  int count = 0, counter = 0;

  if ((fp = fopen("system.sol", "r")) == NULL) {
    printf("ERROR: NO SOLAR FILE\n");
    exit(1);
  }

  while (fgets(buf, sizeof buf, fp) != NULL) count++;

  *list = (SYSTEM*)calloc(count, sizeof(SYSTEM));
  if (*list == NULL) {
    printf("ERROR: Cant allocate memory for %d systems\n", count);
    exit(1);
  }

  fseek(fp, 0L, SEEK_SET);

  while (fgets(buf, sizeof buf, fp) != NULL) {
    int x, y, z;
    unsigned int faction = 0, is_shipyard = 0, is_gas_station = 0, sector = 0;
    int n = sscanf(buf, "%d;%d;%d;%u;%u;%u;%u",
                   &x, &y, &z,
                   &faction, &is_shipyard, &is_gas_station, &sector);

    if (n < 3) continue;  /* garbage line, skip */

    (*list)[counter].x = x;
    (*list)[counter].y = y;
    (*list)[counter].z = z;
    (*list)[counter].faction      = (n >= 4) ? faction : 0;
    (*list)[counter].is_shipyard   = (n >= 5) ? is_shipyard : 0;
    (*list)[counter].is_gas_station = (n >= 6) ? is_gas_station : 0;
    (*list)[counter].sector        = (n >= 7) ? sector : 0;

    counter++;
  }

  fclose(fp);
  return counter;
}


static void free_quest(GAMESTATE* state) {
    if (state->quest) {
        free(state->quest->giver);
        free(state->quest);
        state->quest = NULL;
    }
}

static long parse_long(const char* token) {
    return token ? strtol(token, NULL, 10) : 0;
}

static int parse_int(const char* token) {
    return token ? atoi(token) : 0;
}

static void copy_str(char* dest, const char* src, size_t size) {
    if (src) {
        strncpy(dest, src, size - 1);
        dest[size - 1] = '\0';
    } else {
        dest[0] = '\0';
    }
}

int load_game(GAMESTATE* state, char* filename) {
    FILE* fp;
    char buf[512];
    char* token;
    int has_quest;

    if ((fp = fopen(filename, "r")) == NULL)
        return 0;

    if (fgets(buf, sizeof(buf), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    free_quest(state);

    token = strtok(buf, ";");

    if (!token) goto error;
    copy_str(state->captain_name, token, sizeof(state->captain_name));

    if (!(token = strtok(NULL, ";"))) goto error;
    state->balance = parse_long(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->current_system = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->ship_type = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->tonnage = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->current_cargo = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->cargo_value = parse_long(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->hyper_class = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->smuggler_bay = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->reputation = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->missions_completed = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    state->fuel = parse_int(token);

    if (!(token = strtok(NULL, ";"))) goto error;
    has_quest = parse_int(token);

    if (has_quest) {
        state->quest = malloc(sizeof(QUEST));
        if (!state->quest) goto error;
        state->quest->giver = malloc(sizeof(NPC));
        if (!state->quest->giver) {
            free(state->quest);
            state->quest = NULL;
            goto error;
        }

        if (!(token = strtok(NULL, ";"))) goto error;
        copy_str(state->quest->name, token, sizeof(state->quest->name));

        if (!(token = strtok(NULL, ";"))) goto error;
        state->quest->reward = parse_int(token);

        if (!(token = strtok(NULL, ";"))) goto error;
        state->quest->penalty = parse_int(token);

        if (!(token = strtok(NULL, ";"))) goto error;
        state->quest->type = parse_int(token);

        if (!(token = strtok(NULL, ";"))) goto error;
        copy_str(state->quest->giver->name, token, sizeof(state->quest->giver->name));

        if (!(token = strtok(NULL, ";"))) goto error;
        state->quest->giver->faction = parse_int(token);

        if (!(token = strtok(NULL, ";"))) goto error;
        state->quest->giver->portrait = parse_int(token);
    } else {
        state->quest = NULL;
    }

    fclose(fp);
    return 1;

error:
    free_quest(state);
    fclose(fp);
    return 0;
}

int save_game(GAMESTATE* state) {
    FILE* fp;

    if ((fp = fopen("USER.SAV", "w")) == NULL)
        return 0;

    fprintf(fp, "%s;%ld;%d;%d;%d;%d;%ld;%d;%d;%d;%d;%d",
        state->captain_name,
        state->balance,
        state->current_system,
        state->ship_type,
        state->tonnage,
        state->current_cargo,
        state->cargo_value,
        state->hyper_class,
        state->smuggler_bay,
        state->reputation,
        state->missions_completed,
        state->fuel);

    if (state->quest) {
        fprintf(fp, ";%d;%s;%d;%d;%d;%s;%d;%d\n",
            1,
            state->quest->name,
            state->quest->reward,
            state->quest->penalty,
            state->quest->type,
            state->quest->giver->name,
            state->quest->giver->faction,
            state->quest->giver->portrait);
    } else {
        fprintf(fp, ";0\n");
    }

    fclose(fp);
    return 1;
}