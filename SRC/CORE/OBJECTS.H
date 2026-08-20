/* objects.c -- gas clouds, black holes, nebulae */

#define OBJ_GASCLOUD 0
#define OBJ_BLACKHOLE 1
#define OBJ_NEBULA 2

int core_objects_sphere_line_intersect(int ax, int ay, int az, int bx, int by, int bz, int cx,
                        int cy, int cz, int r);
