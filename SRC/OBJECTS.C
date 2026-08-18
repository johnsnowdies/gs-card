#include <stdio.h>
#include <alloc.h>
#include <math.h>
#include "structs.h"
#include "objects.h"

int loadObjects(OBJECT **list)
{
	FILE *fp;
	char buf[100];
	int count = 0, counter = 0;

	if ((fp = fopen("objects.sol", "r")) == NULL)
		return 0;	/* no objects — not an error */

	/* first pass: count lines */
	while (fgets(buf, sizeof buf, fp) != NULL)
		count++;

	if (count == 0) {
		fclose(fp);
		return 0;
	}

	*list = (OBJECT*) calloc(count, sizeof(OBJECT));
	if (*list == NULL) {
		fclose(fp);
		return 0;
	}

	/* second pass: parse */
	fseek(fp, 0L, SEEK_SET);
	while (fgets(buf, sizeof buf, fp) != NULL)
	{
		int v[5] = {0,0,0,0,0};
		int vi = 0, i, j = 0;
		char num[10] = "";

		for (i = 0; buf[i] != '\0' && buf[i] != '\n' && buf[i] != '\r' && vi < 5; i++)
		{
			if (buf[i] == ';') {
				v[vi++] = atoi(num);
				j = 0;
				num[0] = '\0';
			} else {
				num[j++] = buf[i];
				num[j] = '\0';
			}
		}
		if (vi < 4) continue;	/* malformed line */
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

/* sphereLineIntersect: returns 1 if line segment A-B passes within
 * distance r of point C (sphere intersection) */
int sphereLineIntersect(int ax, int ay, int az, int bx, int by, int bz,
                        int cx, int cy, int cz, int r)
{
	double dx, dy, dz;		/* B - A */
	double wx, wy, wz;		/* C - A */
	double cross_x, cross_y, cross_z;
	double lenSq, distSq;
	double dot, t;

	dx = (double)(bx - ax);
	dy = (double)(by - ay);
	dz = (double)(bz - az);

	wx = (double)(cx - ax);
	wy = (double)(cy - ay);
	wz = (double)(cz - az);

	/* cross product (C-A) × (B-A) */
	cross_x = wy * dz - wz * dy;
	cross_y = wz * dx - wx * dz;
	cross_z = wx * dy - wy * dx;

	/* squared distance from C to the line */
	distSq = cross_x*cross_x + cross_y*cross_y + cross_z*cross_z;
	lenSq = dx*dx + dy*dy + dz*dz;

	if (lenSq == 0.0) return 0;	/* degenerate segment */

	distSq = distSq / lenSq;

	/* check if the closest point is within the segment */
	dot = wx*dx + wy*dy + wz*dz;
	t = dot / lenSq;
	if (t < 0.0 || t > 1.0)
	{
		/* closest point is outside the segment — check endpoints */
		double d1 = wx*wx + wy*wy + wz*wz;
		double d2 = (double)(cx-bx)*(cx-bx) + (double)(cy-by)*(cy-by) + (double)(cz-bz)*(cz-bz);
		if (d1 <= (double)r*r || d2 <= (double)r*r)
			return 1;
		return 0;
	}

	return (distSq <= (double)r*r) ? 1 : 0;
}
