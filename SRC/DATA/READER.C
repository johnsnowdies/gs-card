#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data\structs.h"
#include "data\reader.h"

int load_bounds(BOUND_LINE** list)
{
    FILE* f;
    char buf[256];
    int count = 0;
    BOUND_LINE* arr = NULL;

    f = fopen("BOUNDS.SOL", "r");
    if (!f)
        return 0;

    while (fgets(buf, sizeof(buf), f)) {
        char *p, *q;
        int x1, y1, x2, y2, i;
        BOUND_LINE* tmp;
        BOUND_LINE line;
        POINT *p1, *p2;

        for (p = buf; *p == ' ' || *p == '\t'; p++) ;
        if (*p == '\0' || *p == '\n' || *p == '#')
            continue;

        q = strchr(p, ';');
        if (!q)
            continue;

        *q = '\0';
        if (sscanf(p, "%d,%d", &x1, &y1) != 2)
            continue;
        if (sscanf(q + 1, "%d,%d", &x2, &y2) != 2)
            continue;

        p1 = (POINT*)malloc(sizeof(POINT));
        p2 = (POINT*)malloc(sizeof(POINT));
        if (!p1 || !p2) {
            free(p1); free(p2);
            for (i = 0; i < count; i++) {
                free(arr[i].a);
                free(arr[i].b);
            }
            free(arr);
            fclose(f);
            return 0;
        }

        p1->x = x1; p1->y = y1;
        p2->x = x2; p2->y = y2;

        line.a = p1;
        line.b = p2;

        tmp = (BOUND_LINE*)realloc(arr, (count + 1) * sizeof(BOUND_LINE));
        if (!tmp) {
            free(p1); free(p2);
            for (i = 0; i < count; i++) {
                free(arr[i].a);
                free(arr[i].b);
            }
            free(arr);
            fclose(f);
            return 0;
        }
        arr = tmp;
        arr[count] = line;
        count++;
    }

    fclose(f);
    *list = arr;
    return count;
}

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

int load_game(GAMESTATE* state, char* filename)
{
    FILE* fp;
    int has_quest;
    unsigned char* tmp_visited;

    if ((fp = fopen(filename, "rb")) == NULL)
        return 0;

    free_quest(state);

    fread(state->captain_name, sizeof(char), 100, fp);
    fread(&state->balance, sizeof(long), 1, fp);
    fread(&state->current_system, sizeof(int), 1, fp);
    fread(&state->ship_type, sizeof(int), 1, fp);
    fread(&state->tonnage, sizeof(int), 1, fp);
    fread(&state->current_cargo, sizeof(int), 1, fp);
    fread(&state->cargo_value, sizeof(long), 1, fp);
    fread(&state->hyper_class, sizeof(int), 1, fp);
    fread(&state->smuggler_bay, sizeof(int), 1, fp);
    fread(&state->reputation, sizeof(int), 1, fp);
    fread(&state->missions_completed, sizeof(int), 1, fp);
    fread(&state->fuel, sizeof(int), 1, fp);

    fread(&has_quest, sizeof(int), 1, fp);

    if (has_quest) {
        state->quest = (QUEST*)malloc(sizeof(QUEST));
        if (!state->quest) {
            fclose(fp);
            return 0;
        }
        state->quest->giver = (NPC*)malloc(sizeof(NPC));
        if (!state->quest->giver) {
            free(state->quest);
            state->quest = NULL;
            fclose(fp);
            return 0;
        }

        fread(state->quest->name, sizeof(char), 100, fp);
        fread(&state->quest->reward, sizeof(int), 1, fp);
        fread(&state->quest->penalty, sizeof(int), 1, fp);
        fread(&state->quest->type, sizeof(int), 1, fp);

        fread(state->quest->giver->name, sizeof(char), 100, fp);
        fread(&state->quest->giver->faction, sizeof(int), 1, fp);
        fread(&state->quest->giver->portrait, sizeof(int), 1, fp);
    } else {
        state->quest = NULL;
    }

    fread(&state->visited_bytes, sizeof(int), 1, fp);

    if (state->visited_bytes > 0) {
        state->visited = (unsigned char*)malloc(state->visited_bytes);
        if (!state->visited) {
            fclose(fp);
            return 0;
        }
        fread(state->visited, sizeof(unsigned char), state->visited_bytes, fp);
    } else {
        state->visited = NULL;
    }

    fclose(fp);
    return 1;
}

int save_game(GAMESTATE* state, char* filename)
{
    FILE* fp;
    int has_quest;

    if ((fp = fopen(filename, "wb")) == NULL)
        return 0;

    fwrite(state->captain_name, sizeof(char), 100, fp);
    fwrite(&state->balance, sizeof(long), 1, fp);
    fwrite(&state->current_system, sizeof(int), 1, fp);
    fwrite(&state->ship_type, sizeof(int), 1, fp);
    fwrite(&state->tonnage, sizeof(int), 1, fp);
    fwrite(&state->current_cargo, sizeof(int), 1, fp);
    fwrite(&state->cargo_value, sizeof(long), 1, fp);
    fwrite(&state->hyper_class, sizeof(int), 1, fp);
    fwrite(&state->smuggler_bay, sizeof(int), 1, fp);
    fwrite(&state->reputation, sizeof(int), 1, fp);
    fwrite(&state->missions_completed, sizeof(int), 1, fp);
    fwrite(&state->fuel, sizeof(int), 1, fp);

    has_quest = (state->quest != NULL) ? 1 : 0;
    fwrite(&has_quest, sizeof(int), 1, fp);

    if (has_quest) {
        fwrite(state->quest->name, sizeof(char), 100, fp);
        fwrite(&state->quest->reward, sizeof(int), 1, fp);
        fwrite(&state->quest->penalty, sizeof(int), 1, fp);
        fwrite(&state->quest->type, sizeof(int), 1, fp);

        fwrite(state->quest->giver->name, sizeof(char), 100, fp);
        fwrite(&state->quest->giver->faction, sizeof(int), 1, fp);
        fwrite(&state->quest->giver->portrait, sizeof(int), 1, fp);
    }

    fwrite(&state->visited_bytes, sizeof(int), 1, fp);
    if (state->visited_bytes > 0 && state->visited != NULL) {
        fwrite(state->visited, sizeof(unsigned char), state->visited_bytes, fp);
    }

    fclose(fp);
    return 1;
}