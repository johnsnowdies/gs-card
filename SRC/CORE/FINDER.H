/* finder.c - main map related math and pathfinder algo API */
void core_finder_run_wave(int sol_size, SYSTEM* sol_list, int c, int end);
void core_finder_calc_hyper_threads(int sol_size, SYSTEM * sol_list);
int core_finder_get_way(int sol_size, SYSTEM * sol_list, WAYPOINT *wp);
/*void core_finder_calc_distances(int start, int* distances, int max_dist);*/
