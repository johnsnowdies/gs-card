#include <alloc.h>
#include <limits.h>
#include <math.h>

#include "data\structs.h"

#include "core\objects.h"
#include "core\finder.h"

#include "ui\gui.h"

#include "ui\locale.h"

#include "ui\map\mapwnd.h"

/* ----------------------------------------------------------------
 * Extern game globals (defined in card.c)
 * ---------------------------------------------------------------- */
extern SYSTEM* sol_list;
extern unsigned int sol_size;
extern OBJECT* obj_list;
extern int obj_size;
extern GAME_STATE gs;
extern WND map_wnd;
extern const int DEBUG;

int S;
unsigned char gotEnd;

int max(int a, int b) {
  if (a > b)
    return a;
  else
    return b;
}

int min(int a, int b) {
  if (a < b)
    return a;
  else
    return b;
}


void core_finder_clear_threads_costs() {
  int i, k;

  for (i = 0; i < sol_size; i++) {
    for (k = 0; k < sol_list[i].threadSize; k++) {
      sol_list[i].threads[k].cost = 0;
    }
  }
}

void core_finder_calc_threads_costs(int topCost) {
  int i, j, n, m, k, e;
  SYSTEM a, b, c, d;
  char wndLabel[50] = "";

  sprintf(wndLabel, "%s: %d", LC_FINDER_LOAD_CALC, topCost);

  for (j = 0; j < sol_size; j++) {
    a = sol_list[j];

    if (j % max(1, sol_size / 50) == 0 || j == sol_size - 1)
      gui_progress_wnd(&map_wnd, "GSCARD 1.5", wndLabel, j, sol_size);

    for (i = 0; i < a.threadSize; i++) {
      k = a.threads[i].value;
      b = sol_list[k];


      for (n = 0; n < sol_size; n++) {
        c = sol_list[n];

        for (m = 0; m < c.threadSize; m++) {
          int p[3][3];
          int cross;

          e = c.threads[m].value;
          d = sol_list[e];

          p[0][0] = (b.x - a.x);
          p[0][1] = (b.y - a.y);
          p[0][2] = (b.z - a.z);

          p[1][0] = (d.x - c.x);
          p[1][1] = (d.y - c.y);
          p[1][2] = (d.z - c.z);

          p[2][0] = (b.x - c.x);
          p[2][1] = (b.y - c.y);
          p[2][2] = (b.z - c.z);

          cross = p[0][0] * p[1][1] * p[2][2] + p[0][1] * p[1][2] * p[2][0] +
                  p[0][2] * p[1][0] * p[2][1] - p[0][0] * p[1][2] * p[2][1] -
                  p[0][1] * p[1][0] * p[2][2] - p[0][2] * p[1][1] * p[2][0];

          if (cross == 0) {
            double dt, abxy, abz, cdxy, cdz, t;
            /* double t, checkLeft, checkRight; */

            SYSTEM target;

            t = ((d.x - c.x) * (a.y - c.y) - (d.y - c.y) * (a.x - c.x));
            dt = ((d.y - c.y) * (b.x - a.x) - (b.y - a.y) * (d.x - c.x));

            if (dt != 0.0) {
              t = (int)t / dt;

              target.x = (b.x - a.x) * t + a.x;
              target.y = (b.y - a.y) * t + a.y;
              target.z = (b.z - a.z) * t + a.z;

              /*
              checkLeft = ((d.y - c.y) * ((b.x - a.x) * t + a.x - c.x));
              checkRight = ((d.x - c.x) * ((b.y - a.y) * t + a.y - c.y));
              */

              abxy = (target.x - a.x) * (b.y - a.y) -
                     (target.y - a.y) * (b.x - a.x);

              if (b.z - a.z != 0)
                abz = (target.z - a.z) / (b.z - a.z);
              else
                abz = 0;

              cdxy = (target.x - c.x) * (d.y - c.y) -
                     (target.y - c.y) * (d.x - c.x);

              if (d.z - c.z != 0)
                cdz = (target.z - c.z) / (d.z - c.z);
              else
                cdz = 0;

              if (abxy == abz && cdxy == cdz) {
                int iq;

                a.threads[i].cost++;
                sol_list[j] = a;

                for (iq = 0; iq < c.threadSize; iq++) {
                  if (c.threads[iq].value == e && c.threads[iq].cost == 0) {
                    c.threads[iq].cost--;
                    sol_list[n] = c;
                  }
                }

                for (iq = 0; iq < d.threadSize; iq++) {
                  if (d.threads[iq].value == n && d.threads[iq].cost == 0) {
                    d.threads[iq].cost--;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  for (j = 0; j < sol_size; j++) {
    int newSize = 0;
    a = sol_list[j];

    if (j % max(1, sol_size / 50) == 0 || j == sol_size - 1)
      gui_progress_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_FINDER_REALOC, j, sol_size);

    for (i = 0; i < a.threadSize; i++) {
      if (a.threads[i].cost < topCost) {
        newSize++;
      }
    }

    if (newSize < a.threadSize && newSize != 0) {
      THREAD* newThreads;
      int index = 0;

      if ((newThreads = (THREAD*)malloc(newSize * sizeof(THREAD))) == NULL) {
        printf("HT: Cant allocate memory!");
        exit(1);
      }

      for (i = 0; i < newSize; i++) {
        while (a.threads[index].cost > topCost) {
          index++;
        }

        newThreads[i] = a.threads[index];
        index++;
      }

      free(a.threads);

      a.threads = newThreads;
      a.threadSize = newSize;
    }

    sol_list[j] = a;
  }
}

int core_finder_get_way(WAYPOINT* wp) {
  char* input;
  char current[5];
  int start, end, i, status;

  for (i = 0; i < sol_size; i++) {
    sol_list[i].rate = 0;
    sol_list[i].visited = 0;
  }

  wp->size = 0;

  S = 1;
  gotEnd = 0;
  sprintf(current, "%d", gs.current_system);
  input = (char*)gui_input_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_FINDER_START_TEXT_1, current);
  if(input[0] == '\0')
    return 0;
  start = atoi(input);

  input = (char*)gui_input_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_FINDER_END_TEXT, NULL);
  if(input[0] == '\0')
    return 0;
  end = atoi(input);

  free(input);

  if (start >= 0 && end >= 0 && start < sol_size && end < sol_size) {
    core_finder_run_wave(start, end);
    if (gotEnd) {
      status = core_finder_restore_path(wp, start, end, 1);
    }

    if (!gotEnd || !status) {
      gui_warning_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_FINDER_ERROR_NOWAY);
      getch();
      return 0;
    }

    return 1;

  } else {
    gui_warning_wnd(&map_wnd, LC_GEN_TITLE_GSCARD, LC_GEN_ERROR_INCORRECT_VALUE);
    getch();
  }

  return 0;
}

int core_finder_restore_path(WAYPOINT* wp, int start, int end, int reverse) {
    int i, j, k, c, cnt = 1, prev;
    int minRate;
    int next;

    for (i = 0; i < 25; i++) wp->way[i] = 0;   /* или 30, если используем весь массив */
    wp->way[0] = end;

    c = end;
    while (c != start && cnt < 25) {
        prev = c;
        minRate = INT_MAX;
        next = -1;

        for (i = 0; i < sol_list[c].threadSize; i++) {
            k = sol_list[c].threads[i].value;
            if (k < 0 || k >= sol_size) continue;
            if (sol_list[k].rate != 0 && sol_list[k].rate < minRate) {
                minRate = sol_list[k].rate;
                next = k;
            }
        }

        if (next == -1) return 0;

        c = next;
        wp->way[cnt] = c;
        cnt++;

        if (prev == c) return 0;
    }

    /* Если цикл завершился из-за cnt >= 25, но мы не дошли до start */
    if (c != start) {
        return 0;   /* путь слишком длинный */
    }

    /* Дальше реверс и установка size */
    if (cnt > 1) {
        int buf[25];
        wp->size = cnt;

        if (reverse) {
            for (i = 0, j = cnt - 1; i < cnt; i++, j--) {
                buf[i] = wp->way[j];
            }
            for (i = 0; i < cnt; i++) {
                wp->way[i] = buf[i];
            }
        }
    }

    return 1;
}

void core_finder_run_wave(int c, int end) {
  int i, k;
  SYSTEM m;

  sol_list[c].rate = S;
  sol_list[c].visited = 1;
  S++;

  if (S > 900) {
    return;
  }

  for (i = 0; i < sol_list[c].threadSize; i++) {
    k = sol_list[c].threads[i].value;
    m = sol_list[k];

    if (m.rate == 0) {
      m.rate = S;
    }
  }

  for (i = 0; i < sol_list[c].threadSize; i++) {
    k = sol_list[c].threads[i].value;
    m = sol_list[k];

    if (k != end && m.visited == 0) {
      core_finder_run_wave(k, end);
    }

    if (k == end) {
      gotEnd = 1;
    }
  }
}

void core_finder_calc_hyper_threads() {
  int i = 0, j = 0, k = 0, cnt = 0;
  int error = 0;
  int buffer[15];
  double dx, dy, dz, distSq;

  SYSTEM a, b;

  for (i = 0; i < sol_size; i++) {
    cnt = 0;
    a = sol_list[i];
    a.threadSize = 0;

    if (i % max(1, sol_size / 50) == 0 || i == sol_size - 1)
      gui_progress_wnd(&map_wnd, "GSCARD", "Processing hyper-threads calculation", i, sol_size);

    for (j = 0; j < sol_size; j++) {
      if (j == i) continue;

      b = sol_list[j];
      error = 0;

      dx = (double)(a.x - b.x);
      dy = (double)(a.y - b.y);
      dz = (double)(a.z - b.z);

      distSq = dx * dx + dy * dy + dz * dz;

      if (distSq > 130.0 * 130.0) error = 1;

      if (!error && cnt < 15) {
        buffer[cnt] = j;
        cnt++;
      }
    }

    if (cnt) {
      sol_list[i].threadSize = cnt;

      if ((sol_list[i].threads = (THREAD*)calloc(cnt, sizeof(THREAD))) == NULL) {
        printf("HT: Memory allocation error!");
        exit(1);
      }

      for (k = 0; k < cnt; k++) {
        int val = buffer[k];
        sol_list[i].threads[k].value = val;
        sol_list[i].threads[k].cost = 0;
      }
    } else {
      sol_list[i].threadSize = 0;
    }
  }
  if (!DEBUG) {
    core_finder_clear_threads_costs();
    core_finder_calc_threads_costs(1);
    }
}

void core_finder_calc_distances(int start, int* distances, int max_dist) {
    int i, head = 0, tail = 0;
    int* queue = (int*)malloc(sol_size * sizeof(int));
    if (!queue) return;

    for (i = 0; i < sol_size; i++) distances[i] = -1;
    distances[start] = 0;
    queue[tail++] = start;

    while (head < tail) {
        SYSTEM* sys;
        int cur = queue[head++];
        int curDist = distances[cur];
        if (curDist >= max_dist) continue;

        sys = &sol_list[cur];
        for (i = 0; i < sys->threadSize; i++) {
            int next = sys->threads[i].value;
            if (distances[next] == -1) {
                distances[next] = curDist + 1;
                if (distances[next] <= max_dist) {
                    queue[tail++] = next;
                }
            }
        }
    }
    free(queue);
}
