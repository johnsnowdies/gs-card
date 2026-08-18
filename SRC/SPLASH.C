#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <alloc.h>

void splash() {
	FILE *fp=NULL;

	int x,y,c;
	long int w,h;
    struct palettetype pal;
	long int imgoffset=1078;

	fp=fopen("logo.bmp","rb");

	/* читаем ширину/высоту из BMP-заголовка */
	fseek(fp,18,0);
    w = fgetc(fp) + (fgetc(fp)<<8) + (fgetc(fp)<<16) + (fgetc(fp)<<24);
    h = fgetc(fp) + (fgetc(fp)<<8) + (fgetc(fp)<<16) + (fgetc(fp)<<24);

	fseek(fp,imgoffset,0);
	x=0;
	y=h;

    getpalette(&pal);

	for (c=0; c<16; c++)
		setrgbpalette(pal.colors[c],c*5,0,0);

	/* читаем пиксели блоками через fread вместо fgetc × 64000 */
	{
		unsigned char *row;
		int rowlen = ((w + 3) / 4) * 4;	/* BMP row stride (4-byte aligned) */

		row = malloc(rowlen);
		if (row == NULL) {
			fclose(fp);
			return;
		}

		fseek(fp, imgoffset, SEEK_SET);
		for (y = h - 1; y >= 0; y--) {
			fread(row, 1, rowlen, fp);
			for (x = 0; x < w; x++) {
				putpixel(x, y, row[x] / 32);
			}
		}
		free(row);
	}

    fclose(fp);

	setcolor(8);
	settextstyle(SMALL_FONT,HORIZ_DIR,8);
	outtextxy(10,10, "HYPERSOFT ILTD");
	getch();
	closegraph();
	init();
}