#ifndef _CORE_FINDER
#define _CORE_FINDER

#include "data/structs.h"

void core_finder_calc_hyper_threads();
int core_finder_get_way(WAYPOINT *wp);
int core_finder_get_jumps(int start, int end);

#endif
