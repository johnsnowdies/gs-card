#include "core\finder.h"

#include <alloc.h>
#include <limits.h>
#include <math.h>

#include "core\objects.h"
#include "ui\gui.h"
#include "ui\locale.h"
#include "ui\map\mapwnd.h"
#include "music.h"

extern SYSTEM* sol_list;
extern unsigned int sol_size;
extern OBJECT* obj_list;
extern int obj_size;
extern GAME_STATE gs;
extern WND map_wnd;
extern const int DEBUG;

/* BFS data (far pointers, allocated in far heap) */
static int far* bfs_queue = NULL;
static int far* bfs_parent = NULL;
static int far* bfs_rates = NULL;
static int bfs_queue_capacity = 0;
static int bfs_last_start = -1;
static int bfs_valid = 0;

#define EPSILON 1e-6

/* ----------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------- */
int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }

/* ----------------------------------------------------------------
 * Allocate BFS arrays in far memory
 * ---------------------------------------------------------------- */
static int core_finder_ensure_bfs_arrays(void) {
  int need_size = sol_size;

  if (need_size <= 0) return -1;

  if (bfs_queue == NULL || bfs_queue_capacity < need_size) {
    if (bfs_queue != NULL) farfree(bfs_queue);
    if (bfs_parent != NULL) farfree(bfs_parent);
    if (bfs_rates != NULL) farfree(bfs_rates);

    bfs_queue = (int far*)farmalloc(need_size * sizeof(int));
    bfs_parent = (int far*)farmalloc(need_size * sizeof(int));
    bfs_rates = (int far*)farmalloc(need_size * sizeof(int));

    if (bfs_queue == NULL || bfs_parent == NULL || bfs_rates == NULL) {
      if (bfs_queue != NULL) {
        farfree(bfs_queue);
        bfs_queue = NULL;
      }
      if (bfs_parent != NULL) {
        farfree(bfs_parent);
        bfs_parent = NULL;
      }
      if (bfs_rates != NULL) {
        farfree(bfs_rates);
        bfs_rates = NULL;
      }
      bfs_queue_capacity = 0;
      return -1;
    }

    bfs_queue_capacity = need_size;
    bfs_valid = 0;
  }
  return 0;
}

/* ----------------------------------------------------------------
 * Cleanup: free all permanent allocations
 * ---------------------------------------------------------------- */
void core_finder_cleanup(void) {
  if (bfs_queue != NULL) {
    farfree(bfs_queue);
    bfs_queue = NULL;
  }
  if (bfs_parent != NULL) {
    farfree(bfs_parent);
    bfs_parent = NULL;
  }
  if (bfs_rates != NULL) {
    farfree(bfs_rates);
    bfs_rates = NULL;
  }
  bfs_queue_capacity = 0;
  bfs_last_start = -1;
  bfs_valid = 0;
}

/* ----------------------------------------------------------------
 * Run BFS (uses far arrays)
 * ---------------------------------------------------------------- */
static int core_finder_run_bfs(int start) {
  int head, tail, i, cur, neighbor;
  SYSTEM* sys;

  if (start < 0 || start >= sol_size) return -1;

  if (bfs_valid && bfs_last_start == start) {
    return 0;
  }

  if (core_finder_ensure_bfs_arrays() != 0) return -1;

  for (i = 0; i < sol_size; i++) {
    bfs_rates[i] = 0;
    bfs_parent[i] = -1;
  }

  bfs_rates[start] = 1;
  bfs_parent[start] = -1;
  head = tail = 0;
  bfs_queue[tail++] = start;

  while (head < tail) {
    cur = bfs_queue[head++];
    sys = &sol_list[cur];
    for (i = 0; i < sys->threadSize; i++) {
      neighbor = sys->threads[i].value;
      if (neighbor < 0 || neighbor >= sol_size) continue;
      if (bfs_rates[neighbor] == 0) {
        bfs_rates[neighbor] = bfs_rates[cur] + 1;
        bfs_parent[neighbor] = cur;
        bfs_queue[tail++] = neighbor;
      }
    }
  }

  bfs_last_start = start;
  bfs_valid = 1;
  return 0;
}

/* ----------------------------------------------------------------
 * Restore path (same as before, but uses far arrays)
 * ---------------------------------------------------------------- */
int core_finder_restore_path(WAYPOINT* wp, int start, int end, int reverse) {
  int i, j, cnt, cur;
  int temp_path[25];

  if (start < 0 || end < 0 || start >= sol_size || end >= sol_size) return 0;

  if (!bfs_valid || bfs_last_start != start) {
    if (core_finder_run_bfs(start) != 0) return 0;
  }

  if (bfs_rates[end] == 0) return 0;
  if (bfs_rates[end] > 25) return 0;

  cnt = 0;
  cur = end;
  while (cur != -1 && cnt < 25) {
    temp_path[cnt++] = cur;
    if (cur == start) break;
    cur = bfs_parent[cur];
  }

  if (cur != start || cnt == 0) return 0;

  wp->size = cnt;
  if (reverse) {
    for (i = 0, j = cnt - 1; i < cnt; i++, j--) wp->way[i] = temp_path[j];
  } else {
    for (i = 0; i < cnt; i++) wp->way[i] = temp_path[i];
  }
  return 1;
}

/* ----------------------------------------------------------------
 * Input and get way (unchanged logic)
 * ---------------------------------------------------------------- */
int core_finder_get_way(WAYPOINT* wp) {
  char* input;
  int start, end, status;

  start = gs.current_system;

  input = (char*)gui_input_wnd(&map_wnd, LC_GEN_TITLE_GSCARD,
                               LC_FINDER_END_TEXT, NULL);
  if (input[0] == '\0') return 0;
  end = atoi(input);
  free(input);

  if (start < 0 || end < 0 || start >= sol_size || end >= sol_size) {
    gui_warning_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_GEN_ERROR_INCORRECT_VALUE,
                    SOUND_ERROR);
    getch();
    return 0;
  }

  status = core_finder_restore_path(wp, start, end, 1);
  if (!status) {
    gui_warning_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_FINDER_ERROR_NOWAY,
                    SOUND_ERROR);
    getch();
  }
  return status;
}

/* ----------------------------------------------------------------
 * Get jumps (uses BFS)
 * ---------------------------------------------------------------- */
int core_finder_get_jumps(int start, int end) {
  int result;

  if (start < 0 || end < 0 || start >= sol_size || end >= sol_size) return 0;

  if (core_finder_run_bfs(start) != 0) return 0;
  if (bfs_rates[end] == 0) return 0;
  if (bfs_rates[end] > 25) return 0;

  result = bfs_rates[end];
  return result;
}

/* ----------------------------------------------------------------
 * Geometric intersection test (no large local arrays)
 * ---------------------------------------------------------------- */
static int segments_intersect_3d(int ax1, int ay1, int az1, int ax2, int ay2,
                                 int az2, int bx1, int by1, int bz1, int bx2,
                                 int by2, int bz2) {
  double d1x, d1y, d1z, d2x, d2y, d2z;
  double wx, wy, wz, a, b, c, d, e, denominator, sc, tc;
  double cx1, cy1, cz1, cx2, cy2, cz2, dist_sq, epsilon;

  epsilon = EPSILON;

  if ((ax1 == bx1 && ay1 == by1 && az1 == bz1) ||
      (ax1 == bx2 && ay1 == by2 && az1 == bz2) ||
      (ax2 == bx1 && ay2 == by1 && az2 == bz1) ||
      (ax2 == bx2 && ay2 == by2 && az2 == bz2))
    return 0;

  d1x = ax2 - ax1;
  d1y = ay2 - ay1;
  d1z = az2 - az1;
  d2x = bx2 - bx1;
  d2y = by2 - by1;
  d2z = bz2 - bz1;

  wx = ax1 - bx1;
  wy = ay1 - by1;
  wz = az1 - bz1;

  a = d1x * d1x + d1y * d1y + d1z * d1z;
  b = d1x * d2x + d1y * d2y + d1z * d2z;
  c = d2x * d2x + d2y * d2y + d2z * d2z;
  d = d1x * wx + d1y * wy + d1z * wz;
  e = d2x * wx + d2y * wy + d2z * wz;

  denominator = a * c - b * b;
  if (denominator < epsilon) {
    sc = 0.0;
    tc = (b > c ? d / b : e / c);
  } else {
    sc = (b * e - c * d) / denominator;
    tc = (a * e - b * d) / denominator;
  }

  if (sc < 0.0)
    sc = 0.0;
  else if (sc > 1.0)
    sc = 1.0;
  if (tc < 0.0)
    tc = 0.0;
  else if (tc > 1.0)
    tc = 1.0;

  cx1 = ax1 + sc * d1x;
  cy1 = ay1 + sc * d1y;
  cz1 = az1 + sc * d1z;
  cx2 = bx1 + tc * d2x;
  cy2 = by1 + tc * d2y;
  cz2 = bz1 + tc * d2z;

  dist_sq = (cx1 - cx2) * (cx1 - cx2) + (cy1 - cy2) * (cy1 - cy2) +
            (cz1 - cz2) * (cz1 - cz2);
  return (dist_sq < epsilon * epsilon);
}

/* ----------------------------------------------------------------
 * Build hyper-thread graph without intersecting edges
 * All temporary large arrays are allocated in FAR memory
 * ---------------------------------------------------------------- */
void core_finder_calc_hyper_threads() {
  int i, j;
  int total_possible = 0;
  int far* cand1 = NULL;
  int far* cand2 = NULL;
  double far* cand_len = NULL;
  int cand_count = 0;
  int far* added1 = NULL;
  int far* added2 = NULL;
  int added_count = 0;
  SYSTEM a, b;
  double dx, dy, dz, dist_sq;
  double tmp_len;
  int tmp1, tmp2;
  int intersect;
  int far* degree = NULL;
  int far* fill_pos = NULL;
  int v1, v2;

  total_possible = 0;
  for (i = 0; i < sol_size; i++) {
    a = sol_list[i];
    for (j = i + 1; j < sol_size; j++) {
      b = sol_list[j];
      dx = (double)(a.x - b.x);
      dy = (double)(a.y - b.y);
      dz = (double)(a.z - b.z);
      dist_sq = dx * dx + dy * dy + dz * dz;
      if (dist_sq <= 130.0 * 130.0) total_possible++;
    }
  }

  if (total_possible == 0) {
    for (i = 0; i < sol_size; i++) {
      sol_list[i].threadSize = 0;
      if (sol_list[i].threads != NULL) {
        free(sol_list[i].threads);
        sol_list[i].threads = NULL;
      }
    }
    return;
  }

  cand1 = (int far*)farmalloc(total_possible * sizeof(int));
  cand2 = (int far*)farmalloc(total_possible * sizeof(int));
  cand_len = (double far*)farmalloc(total_possible * sizeof(double));

  if (cand1 == NULL || cand2 == NULL || cand_len == NULL) {
    if (cand1) farfree(cand1);
    if (cand2) farfree(cand2);
    if (cand_len) farfree(cand_len);
    exit(1);
  }

  cand_count = 0;
  for (i = 0; i < sol_size; i++) {
    a = sol_list[i];
    for (j = i + 1; j < sol_size; j++) {
      b = sol_list[j];
      dx = (double)(a.x - b.x);
      dy = (double)(a.y - b.y);
      dz = (double)(a.z - b.z);
      dist_sq = dx * dx + dy * dy + dz * dz;
      if (dist_sq <= 130.0 * 130.0) {
        cand1[cand_count] = i;
        cand2[cand_count] = j;
        cand_len[cand_count] = sqrt(dist_sq);
        cand_count++;
      }
    }
  }

  for (i = 0; i < cand_count - 1; i++) {
    for (j = 0; j < cand_count - i - 1; j++) {
      if (cand_len[j] > cand_len[j + 1]) {
        tmp_len = cand_len[j];
        cand_len[j] = cand_len[j + 1];
        cand_len[j + 1] = tmp_len;
        tmp1 = cand1[j];
        cand1[j] = cand1[j + 1];
        cand1[j + 1] = tmp1;
        tmp2 = cand2[j];
        cand2[j] = cand2[j + 1];
        cand2[j + 1] = tmp2;
      }
    }
    if (i % 10 == 0)
      gui_progress_wnd(&map_wnd, "GSCARD", "Sorting edges", i, cand_count);
  }

  added1 = (int far*)farmalloc(cand_count * sizeof(int));
  added2 = (int far*)farmalloc(cand_count * sizeof(int));
  if (added1 == NULL || added2 == NULL) {
    farfree(cand1);
    farfree(cand2);
    farfree(cand_len);
    if (added1) farfree(added1);
    if (added2) farfree(added2);
    exit(1);
  }

  added_count = 0;
  for (i = 0; i < cand_count; i++) {
    intersect = 0;
    if (i % max(1, cand_count / 100) == 0)
      gui_progress_wnd(&map_wnd, "GSCARD", "Checking intersections", i,
                       cand_count);

    for (j = 0; j < added_count; j++) {
      if (segments_intersect_3d(sol_list[cand1[i]].x, sol_list[cand1[i]].y,
                                sol_list[cand1[i]].z, sol_list[cand2[i]].x,
                                sol_list[cand2[i]].y, sol_list[cand2[i]].z,
                                sol_list[added1[j]].x, sol_list[added1[j]].y,
                                sol_list[added1[j]].z, sol_list[added2[j]].x,
                                sol_list[added2[j]].y, sol_list[added2[j]].z)) {
        intersect = 1;
        break;
      }
    }

    if (!intersect) {
      added1[added_count] = cand1[i];
      added2[added_count] = cand2[i];
      added_count++;
    } else {
    }
  }

  for (i = 0; i < sol_size; i++) {
    if (sol_list[i].threads != NULL) {
      free(sol_list[i].threads);
      sol_list[i].threads = NULL;
    }
    sol_list[i].threadSize = 0;
  }

  degree = (int far*)farmalloc(sol_size * sizeof(int));
  fill_pos = (int far*)farmalloc(sol_size * sizeof(int));
  if (degree == NULL || fill_pos == NULL) {
    farfree(cand1);
    farfree(cand2);
    farfree(cand_len);
    farfree(added1);
    farfree(added2);
    if (degree) farfree(degree);
    if (fill_pos) farfree(fill_pos);
    exit(1);
  }

  for (i = 0; i < sol_size; i++) {
    degree[i] = 0;
    fill_pos[i] = 0;
  }

  for (i = 0; i < added_count; i++) {
    degree[added1[i]]++;
    degree[added2[i]]++;
  }

  for (i = 0; i < sol_size; i++) {
    if (degree[i] > 0) {
      sol_list[i].threads = (THREAD far*)farcalloc(degree[i], sizeof(THREAD));
      if (sol_list[i].threads == NULL) {
        farfree(degree);
        farfree(fill_pos);
        farfree(cand1);
        farfree(cand2);
        farfree(cand_len);
        farfree(added1);
        farfree(added2);
        exit(1);
      }
    } else {
      sol_list[i].threads = NULL;
    }
    sol_list[i].threadSize = degree[i];
  }

  for (i = 0; i < added_count; i++) {
    v1 = added1[i];
    v2 = added2[i];
    sol_list[v1].threads[fill_pos[v1]].value = v2;
    sol_list[v1].threads[fill_pos[v1]].cost = 0;
    fill_pos[v1]++;
    sol_list[v2].threads[fill_pos[v2]].value = v1;
    sol_list[v2].threads[fill_pos[v2]].cost = 0;
    fill_pos[v2]++;
  }

  farfree(cand1);
  farfree(cand2);
  farfree(cand_len);
  farfree(added1);
  farfree(added2);
  farfree(degree);
  farfree(fill_pos);

  bfs_valid = 0;
  bfs_last_start = -1;
}
