#include "core\objects.h"

#include <alloc.h>
#include <math.h>
#include <stdio.h>

#include "data\structs.h"


/* sphereLineIntersect: returns 1 if line segment A-B passes within
 * distance r of point C (sphere intersection) */
int core_objects_sphere_line_intersect(int ax, int ay, int az, int bx, int by, int bz, int cx,
                        int cy, int cz, int r) {
  double dx, dy, dz; /* B - A */
  double wx, wy, wz; /* C - A */
  double cross_x, cross_y, cross_z;
  double lenSq, distSq;
  double dot, t;

  dx = (double)(bx - ax);
  dy = (double)(by - ay);
  dz = (double)(bz - az);

  wx = (double)(cx - ax);
  wy = (double)(cy - ay);
  wz = (double)(cz - az);

  /* cross product (C-A) x (B-A) */
  cross_x = wy * dz - wz * dy;
  cross_y = wz * dx - wx * dz;
  cross_z = wx * dy - wy * dx;

  /* squared distance from C to the line */
  distSq = cross_x * cross_x + cross_y * cross_y + cross_z * cross_z;
  lenSq = dx * dx + dy * dy + dz * dz;

  if (lenSq == 0.0) return 0; /* degenerate segment */

  distSq = distSq / lenSq;

  /* check if the closest point is within the segment */
  dot = wx * dx + wy * dy + wz * dz;
  t = dot / lenSq;
  if (t < 0.0 || t > 1.0) {
    /* closest point is outside the segment -- check endpoints */
    double d1 = wx * wx + wy * wy + wz * wz;
    double d2 = (double)(cx - bx) * (cx - bx) + (double)(cy - by) * (cy - by) +
                (double)(cz - bz) * (cz - bz);
    if (d1 <= (double)r * r || d2 <= (double)r * r) return 1;
    return 0;
  }

  return (distSq <= (double)r * r) ? 1 : 0;
}
