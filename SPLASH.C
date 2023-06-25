#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>

#define MAX 256

void splash() {
	FILE *fp=NULL;

	long int n[MAX]={0};
	int x,y,i,j,c,t, type;
	int r,g,bl;

	/*int gd=VGA,gm=VGAHI,x=0,y;*/

	long int w,h;
    struct palettetype pal;

    char s[5];

	char INPUTFILE[100], OUTPUTFILE[100];
	long int imgoffset=1078;

	fp=fopen("logo.bmp","rb");

	fseek(fp,18,0);
    w=fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);
    h=fgetc(fp)+(fgetc(fp)<<8)+(fgetc(fp)<<16)+(fgetc(fp)<<24);

	fseek(fp,imgoffset,0);
	x=0;
	y=h;

	/*initgraph(&gd,&gm,"c:/turboc/BGI/");*/
    getpalette(&pal);


	for (i=0; i<16; i++)
		setrgbpalette(pal.colors[i],i*5,0,0);


	fseek(fp,imgoffset,0);
    while(!feof(fp)) {
        c=fgetc(fp);
		putpixel(x,y,c/32);
        x=(x+1)%w;
        if(!x){
            --y;
        }
        ++n[c%MAX];
    }
    fclose(fp);








	setcolor(8);
	settextstyle(SMALL_FONT,HORIZ_DIR,8);
	outtextxy(10,10, "HYPERSOFT ILTD");
	getch();
	closegraph();
	init();
}