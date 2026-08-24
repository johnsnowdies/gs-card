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

int load_object(OBJECT** list) 
{
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

int load_solar(SYSTEM** list) 
{
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


int load_game_file(GAMESTATE* state, char* filename)
{
    FILE* fp;
    int i;

    if ((fp = fopen(filename, "rb")) == NULL)
        return 0;

    if (state->visited) {
        free(state->visited);
        state->visited = NULL;
    }

    fread(state->captain_name, sizeof(char), 100, fp);
    fread(&state->balance, sizeof(long), 1, fp);
    fread(&state->current_system, sizeof(int), 1, fp);
    fread(&state->ship_type, sizeof(int), 1, fp);
    fread(&state->tonnage, sizeof(int), 1, fp);
    fread(&state->current_cargo, sizeof(int), 1, fp);
    fread(&state->cargo_value, sizeof(long), 1, fp);
    fread(&state->hyper_class, sizeof(int), 1, fp);
    fread(&state->smuggler_bay, sizeof(unsigned char), 1, fp);
    fread(&state->reputation, sizeof(int), 1, fp);
    fread(&state->missions_completed, sizeof(int), 1, fp);
    fread(&state->fuel, sizeof(int), 1, fp);

    /*
    fread(&state->quests_size, sizeof(int), 1, fp);

    if (state->quests_size > 5) {
        state->quests_size = 5;   
    }

    for (i = 0; i < state->quests_size; i++) {
        fread(&state->quests[i], sizeof(QUEST), 1, fp);
    }

    for (i = state->quests_size; i < 5; i++) {
        memset(&state->quests[i], 0, sizeof(QUEST));
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
    }*/

    fclose(fp);
    return 1;
}

int save_game_file(GAMESTATE* state, char* filename)
{
    FILE* fp;
    int i;

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
    fwrite(&state->smuggler_bay, sizeof(unsigned char), 1, fp);
    fwrite(&state->reputation, sizeof(int), 1, fp);
    fwrite(&state->missions_completed, sizeof(int), 1, fp);
    fwrite(&state->fuel, sizeof(int), 1, fp);

    /*

    fwrite(&state->quests_size, sizeof(int), 1, fp);

    for (i = 0; i < state->quests_size; i++) {
        fwrite(&state->quests[i], sizeof(QUEST), 1, fp);
    }

    fwrite(&state->visited_bytes, sizeof(int), 1, fp);
    if (state->visited_bytes > 0 && state->visited != NULL) {
        fwrite(state->visited, sizeof(unsigned char), state->visited_bytes, fp);
    }
*/
    fclose(fp);
    return 1;
}


void draw_4bit_bmp(char *filename, int px, int py)
{
    FILE *fp;
    long offbits;
    int w, h, bitcount;
    int rowlen;
    unsigned char *row;
    int x, y;

    fp = fopen(filename, "rb");
    if (!fp) return;

    fseek(fp, 10, SEEK_SET);
    offbits = fgetc(fp) + (fgetc(fp) << 8) + (fgetc(fp) << 16) + (fgetc(fp) << 24);

    fseek(fp, 18, SEEK_SET);
    w = fgetc(fp) + (fgetc(fp) << 8) + (fgetc(fp) << 16) + (fgetc(fp) << 24);
    h = fgetc(fp) + (fgetc(fp) << 8) + (fgetc(fp) << 16) + (fgetc(fp) << 24);

    fseek(fp, 28, SEEK_SET);
    bitcount = fgetc(fp) + (fgetc(fp) << 8);
    if (bitcount != 4) {
        fclose(fp);
        return;
    }

    rowlen = ((w + 1) / 2 + 3) & ~3;
    row = (unsigned char *)malloc(rowlen);
    if (!row) {
        fclose(fp);
        return;
    }

    fseek(fp, offbits, SEEK_SET);
    for (y = h - 1; y >= 0; y--) {
        fread(row, 1, rowlen, fp);
        for (x = 0; x < w; x++) {
            unsigned char byte = row[x / 2];
            unsigned char idx;
            if (x % 2 == 0)
                idx = byte >> 4;      /* левый пиксель — старший полубайт */
            else
                idx = byte & 0x0F;    /* правый пиксель — младший полубайт */
            putpixel(px + x, py + y, idx);
        }
    }

    free(row);
    fclose(fp);
}
