#include <conio.h>
#include <math.h>
#include <stdio.h>

#include "structs.h"
#include "reader.h"

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
    unsigned int fraction = 0, is_shipyard = 0, is_gas_station = 0, sector = 0;
    int n = sscanf(buf, "%d;%d;%d;%u;%u;%u;%u",
                   &x, &y, &z,
                   &fraction, &is_shipyard, &is_gas_station, &sector);

    if (n < 3) continue;  /* garbage line, skip */

    (*list)[counter].x = x;
    (*list)[counter].y = y;
    (*list)[counter].z = z;
    (*list)[counter].fraction      = (n >= 4) ? fraction : 0;
    (*list)[counter].is_shipyard   = (n >= 5) ? is_shipyard : 0;
    (*list)[counter].is_gas_station = (n >= 6) ? is_gas_station : 0;
    (*list)[counter].sector        = (n >= 7) ? sector : 0;

    counter++;
  }

  fclose(fp);
  return counter;
}


int load_game(GAMESTATE* state) {
  FILE* fp;
  char buf[256];

  if ((fp = fopen("USER.SAV", "r")) == NULL)
    return 0;

  if (fgets(buf, sizeof buf, fp) == NULL) {
    fclose(fp);
    return 0;
  }

  sscanf(buf, "%99[^;];%ld;%d;%d;%d;%d;%ld;%d;%d;%d;%d;%d",
    state->captain_name,
    &state->balance,
    &state->current_system,
    &state->ship_type,
    &state->tonnage,
    &state->current_cargo,
    &state->cargo_value,
    &state->hyper_class,
    &state->smuggler_bay,
    &state->reputation,
    &state->missions_completed,
    &state->fuel);

  fclose(fp);
  return 1;
}

int save_game(GAMESTATE* state) {
  FILE* fp;

  if ((fp = fopen("USER.SAV", "w")) == NULL)
    return 0;

  fprintf(fp, "%s;%ld;%d;%d;%d;%d;%ld;%d;%d;%d;%d;%d\n",
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

  fclose(fp);
  return 1;
}
