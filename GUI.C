#include <graphics.h>
#include <alloc.h>
#include "structs.h"
#include "gui.h"
#include "reader.h"
#include "keys.h"
#include "ad.h"
#include "finder.h"

#define MAX_INPUT_LEN 30
#define MAX_VALUE 1400
#define MIN_VALUE -1400

int WND_WIDTH = 639;
int WND_HEIGHT = 460;

int POINT_COLOR = 0;

float xmin=MIN_VALUE, xmax=MAX_VALUE;
float ymin=MIN_VALUE, ymax=MAX_VALUE;
float zmin=MIN_VALUE, zmax=MAX_VALUE;

int offsetX=0, offsetY = 0, offsetZ = 0;
int pathListFlag = 0;
float xdens, ydens;

void pathwnd(WAYPOINT *wp);

int ex(float x)
{
	int xs;

	xs = (int) ((x-xmin)/xdens);

	if (xs >= WND_WIDTH)
		xs = WND_WIDTH;

	return xs;
}

int ey(float y)
{
	int ys;

	y = -1 * y;
	ys = (int) ((ymax-y)/ydens);

	return ys;
}

POINT p(float x, float y, float z)
{

	float b = 0.8660254 * y;
	float a = (y / 2);

	POINT res;

	res.x = (int)(WND_WIDTH/2 + (x - a)/xdens);
	res.y = (int)(WND_HEIGHT/2 + (b/ydens) - (z/ydens) );

	if(res.x >= WND_WIDTH)
		res.x = WND_WIDTH;

	if (z <= 0)
		POINT_COLOR = 7;

	if (z <= -50)
		POINT_COLOR = 9;

	if (z <= -100)
		POINT_COLOR = 3;

	if (z <= -150)
		POINT_COLOR = 5;

	if (z <= -200)
		POINT_COLOR = 1;

	if (z >= 0)
		POINT_COLOR = 7;

	if (z >= 50)
		POINT_COLOR = 8;

	if (z >= 100)
		POINT_COLOR = 14;

	if (z >= 150)
		POINT_COLOR = 12;

	if (z >= 200)
		POINT_COLOR = 4;

	return res;
}

void draw3dwnd(int ptrSize, SYSTEM *solar, int isCoord, int isHyper, WAYPOINT *wp)
{
	int i,j,k;

	POINT A1,A2,A3,A4,A5,A6,A7,A8,A9;

	SYSTEM buf,a,b;

	xdens=(xmax-xmin)/WND_WIDTH;
	ydens=(ymax-ymin)/WND_HEIGHT;

	if (isCoord == 1){
		setcolor(15);

		A1 = p(0.0 +offsetX,0.0+offsetY,0.0+offsetZ);
		A2 = p(0.0 +offsetX,ymax+offsetY,0.0+offsetZ);
		A3 = p(xmax+offsetX,0.0+offsetY,0.0+offsetZ);

		A4 = p(xmin+offsetX,0.0+offsetY,0.0+offsetZ);
		A5 = p(0.0+offsetX,ymin+offsetY,0.0+offsetZ);
		A6 = p(0.0+offsetX,0.0+offsetY,zmin+offsetZ);

		A7 = p(0.0+offsetX,0.0+offsetY,zmax+offsetZ);

		/* POSITIVE LINES */
		line(A1.x,A1.y,A2.x,A2.y);
		line(A1.x,A1.y,A3.x,A3.y);
		line(A1.x,A1.y,A7.x,A7.y);

		/* NEGATIVE LINES */
		setlinestyle(1,0,1);
		line (A1.x,A1.y,A4.x,A4.y);
		line (A1.x,A1.y,A5.x,A5.y);
		line (A1.x,A1.y,A6.x,A6.y);

	}

	for (i = 0; i< ptrSize; i++)
	{
		A1 = p(solar[i].x + offsetX ,solar[i].y + offsetY,solar[i].z+offsetZ);

		p(solar[i].x, solar[i].y, solar[i].z);
		putpixel(A1.x,A1.y,POINT_COLOR);

		if (solar[i].threadSize !=0 && isHyper)
			{

				setlinestyle(1,0,1);
				for (j = 0; j < solar[i].threadSize; j++){
					buf = solar[solar[i].threads[j].value];
					A8 = p(buf.x + offsetX, buf.y +offsetY, buf.z + offsetZ);
					if (solar[i].threads[j].cost < 30)
						line (A1.x,A1.y,A8.x,A8.y);
				}
			}

		if (xmax <= MAX_VALUE/10 && (A1.x < (WND_WIDTH-80)) && (A1.y < (WND_HEIGHT-20))){
			char c[50] = "";
			setcolor(15);
			settextstyle(SMALL_FONT,HORIZ_DIR,4);
			sprintf(c,"SA.%d(%d)",i,solar[i].threadSize);
			outtextxy(A1.x, A1.y + 5,c);
			/*settextstyle(DEFAULT_FONT,HORIZ_DIR,1);*/
		}
	}

    if (wp->way[0]!=NULL)
	{
		for (i=1; i< wp->size; i++)
		{

		  a = solar[wp->way[i-1]];
		  b = solar[wp->way[i]];

		  A8 = p(a.x + offsetX, a.y + offsetY, a.z + offsetZ);
		  A9 = p(b.x + offsetX, b.y + offsetY, b.z + offsetZ);
		  setcolor(9);
		  setlinestyle(3,0,1);

		  line(A8.x, A8.y, A9.x, A9.y);

		  setcolor(15);
		}
	}

	setcolor(4);
	setlinestyle(0,0,1);
	rectangle(0,0,WND_WIDTH,WND_HEIGHT);
	statusLine();
}

void draw2dwnd(int ptrSize, SYSTEM *solar, int isCoord, int isHyper, WAYPOINT *wp)
{
	int i,j,hashValue,hb;
	char *hashKey;

	SYSTEM buf,a,b;

	xdens=(xmax-xmin)/WND_WIDTH;
	ydens=(ymax-ymin)/WND_HEIGHT;

	if (isCoord == 1){
		setcolor(15);

		/* POSITIVE LINES */
		line (ex(0+offsetX),ey(0+offsetY),ex(xmax+offsetX),ey(0+offsetY));
		line (ex(0+offsetX),ey(0+offsetY),ex(0+offsetX),ey(ymax+offsetY));

		/* NEGATIVE LINES */
		setlinestyle(1,0,1);
		line (ex(xmin+offsetX),ey(0+offsetY),ex(0+offsetX),ey(0+offsetY));
		line (ex(0+offsetX),ey(ymin+offsetY),ex(0+offsetX),ey(0+offsetY));
	}

	for (i = 0; i < ptrSize; i++)
	{

		p(solar[i].x, solar[i].y, solar[i].z);
		putpixel(ex(solar[i].x+offsetX),ey(solar[i].y+offsetY),POINT_COLOR);

        if (solar[i].threadSize!=0 && isHyper)
			{

				for (j = 0; j < solar[i].threadSize; j++)
				{

					setcolor(15);
					setlinestyle(1,0,1);

					if (solar[i].threads[j].cost < 15){
						buf = solar[solar[i].threads[j].value];

						line (ex(solar[i].x + offsetX), ey(solar[i].y + offsetY),
							ex(buf.x + offsetX),ey(buf.y + offsetY));
							}
				}
			}

		if (xmax <= MAX_VALUE/10 && (ex(solar[i].x+offsetX) < (WND_WIDTH-80) )
								&& (ey(solar[i].y+offsetY) < (WND_HEIGHT-15)) ){

			char c[50] = "";

			sprintf(c,"SA.%d(%d)",i,solar[i].threadSize);
			setcolor(15);
			settextstyle(SMALL_FONT,HORIZ_DIR,4);
			outtextxy(ex(solar[i].x+offsetX), ey(solar[i].y+offsetY)+5,c);


		}
	}

	if (wp->size != 0)
	{
		for (i=1; i< wp->size; i++)
		{

		  a = solar[wp->way[i-1]];
		  b = solar[wp->way[i]];

		  setcolor(9);
		  setlinestyle(3,0,1);

		  line(ex(a.x + offsetX),ey(a.y + offsetY),
			   ex(b.x + offsetX),ey(b.y + offsetY));
		  setcolor(15);
		}
	}

	setlinestyle(0,0,1);

	setcolor(4);
	rectangle(0,0,WND_WIDTH,WND_HEIGHT);
	statusLine();
}

void drawyzwnd(int ptrSize, SYSTEM *solar, int isCoord, int isHyper, WAYPOINT *wp)
{
	int i,j,hashValue,hb;
	char *hashKey;

	SYSTEM buf,a,b;

	xdens=(xmax-xmin)/WND_WIDTH;
	ydens=(ymax-ymin)/WND_HEIGHT;

	if (isCoord == 1){
		setcolor(15);

		/* POSITIVE LINES */
		line (ex(0+offsetX),ey(-1*(0+offsetZ)),ex(xmax+offsetX),ey(-1*(0+offsetZ)));
		line (ex(0+offsetX),ey(-1*(0+offsetZ)),ex(0+offsetX),ey(-1*(zmax+offsetZ)));

		/* NEGATIVE LINES */
		setlinestyle(1,0,1);
		line (ex(xmin+offsetX),ey(-1*(0+offsetZ)),ex(0+offsetX),ey(-1*(0+offsetZ)));
		line (ex(0+offsetX),ey(-1*(zmin+offsetZ)),ex(0+offsetX),ey(-1*(0+offsetZ)));
	}

	for (i = 0; i < ptrSize; i++){

		p(solar[i].x, solar[i].y, solar[i].z);
		putpixel(ex(solar[i].x+offsetX),ey(-1 * (solar[i].z+offsetZ)),POINT_COLOR);

        if (solar[i].threadSize!=0 && isHyper)
			{

				for (j = 0; j < solar[i].threadSize; j++)
				{

					setcolor(15);
					setlinestyle(1,0,1);

					if (solar[i].threads[j].cost < 15){
						buf = solar[solar[i].threads[j].value];

						line (ex(solar[i].x + offsetX), ey(-1 * (solar[i].z + offsetZ)),
							ex(buf.x + offsetX),ey(-1 * (buf.z + offsetZ)));
							}
				}
			}

		if (xmax <= MAX_VALUE/10 && (ex(solar[i].x+offsetX) < (WND_WIDTH-80) )
								&& (ey(solar[i].y+offsetY) < (WND_HEIGHT-15)) ){

			char c[50] = "";

			sprintf(c,"SA.%d(%d)",i,solar[i].threadSize);
			setcolor(15);
			settextstyle(SMALL_FONT,VERT_DIR,4);
			outtextxy(ex(solar[i].x+offsetX), ey(-1 * (solar[i].z+offsetZ))+5,c);


		}
	}

	if (wp->size != 0)
	{
		for (i=1; i< wp->size; i++)
		{

		  a = solar[wp->way[i-1]];
		  b = solar[wp->way[i]];

		  setcolor(9);
		  setlinestyle(3,0,1);

		  line(ex(a.x + offsetX),ey(-1*(a.z + offsetZ)),
			   ex(b.x + offsetX),ey(-1*(b.z + offsetZ)));
		  setcolor(15);
		}
	}

	setlinestyle(0,0,1);

	setcolor(4);
	rectangle(0,0,WND_WIDTH,WND_HEIGHT);
	statusLine();
}



void statusLine()
{
	unsigned int USED_MEM;
	unsigned int FREE_MEM;
	unsigned int TOTAL_MEM = 65535;

	int xpos=2;

	char memMsg[50] = "";

	adHypersoft();

	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	FREE_MEM = coreleft();
	USED_MEM = TOTAL_MEM - FREE_MEM;

	sprintf(memMsg,"%u/%u",USED_MEM,TOTAL_MEM);

	setfillstyle(SOLID_FILL,BLACK);

	bar(0,WND_HEIGHT+1,WND_WIDTH,WND_HEIGHT+20);

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT+2,"F1");
	xpos +=13;
	setcolor(15);
	outtextxy(xpos,WND_HEIGHT+2,"-VIEW");
	xpos +=45;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT+2,"F2");
	setcolor(15);
	xpos +=15;
	outtextxy(xpos,WND_HEIGHT+2,"-AXIS");
	xpos += 45;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT+2,"F3/F4");
	xpos += 40;
	setcolor(15);
	outtextxy(xpos,WND_HEIGHT + 2, "-ZOOM");
	xpos += 45;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT +2, "F5");
	xpos += 15;
	setcolor(15);
	outtextxy(xpos,WND_HEIGHT +2, "-GOTO");
	xpos += 45;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT +2, "F6");
	xpos += 15;

	setcolor(15);
	outtextxy(xpos,WND_HEIGHT +2, "-THREADS");
	xpos +=70;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT +2, "F7");
	xpos +=15;

	setcolor(15);
	outtextxy(xpos,WND_HEIGHT +2, "-RUN");
	xpos +=40;

	xpos = 470;
	setcolor(4);
	line(xpos,WND_HEIGHT,xpos,480);
	xpos +=5;

	setcolor(4);
	outtextxy(xpos,WND_HEIGHT +2, "MEM:");
	xpos +=30;
	setcolor(15);
	outtextxy(xpos,WND_HEIGHT +2, memMsg);
}

void scaleMinus()
{
	if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3)
	{
    	xmax = xmax + MAX_VALUE/10;
		ymax = ymax + MAX_VALUE/10;
		zmax = zmax + MAX_VALUE/10;

		ymin = ymin - MAX_VALUE/10;
		xmin = xmin - MAX_VALUE/10;
		zmin = zmin - MAX_VALUE/10;
	}
	else
	{
		warningWnd("ERROR","Minimal scale rate reached!");
		getch();
	}
}

void scalePlus()
{
	if (xmax > MAX_VALUE/10 && xmin < MIN_VALUE/10)
	{
		xmax = xmax - MAX_VALUE/10;
		ymax = ymax - MAX_VALUE/10;
		zmax = zmax - MAX_VALUE/10;

		ymin = ymin + MAX_VALUE/10;
		xmin = xmin + MAX_VALUE/10;
		zmin = zmin + MAX_VALUE/10;
	}
	else
	{
		warningWnd("ERROR","Maximal scale rate reached!");
		getch();
	}
}

void offsetXplus(){	offsetX += xmax/10;}
void offsetXminus(){ offsetX -= xmax/10; }
void offsetYplus() { offsetY += ymax/10; }
void offsetYminus(){ offsetY -= ymax/10; }
void offsetZplus() { offsetZ += zmax/10; }
void offsetZminus(){ offsetZ -= zmax/10; }


void clearWnd()
{
	setfillstyle(SOLID_FILL,BLACK);
	bar(1,1,WND_WIDTH-1,WND_HEIGHT-1);
}

void init()
{
	int gd=DETECT,gm,result;
	char *msg;

	initgraph(&gd,&gm,"BGI");
	result = graphresult();
	if(result){
		msg = grapherrormsg(result);
		printf("%s",msg);
	}

	settextstyle(SMALL_FONT,HORIZ_DIR,4);
}

void draw(int ptrSize, SYSTEM *ptrList, int mode, int isCoord,int isHyper, WAYPOINT *way, int currentPoint)
{
	clearWnd();

	if (way->size){

		WND_WIDTH = 470;
		pathWnd(way,currentPoint,ptrList);
	}
	else{
		WND_WIDTH = 639;
		clearWnd();
	}


	if (mode == 1)
	{
		draw2dwnd(ptrSize,ptrList,isCoord,isHyper,way);
	}

	if (mode == 3)
	{
		drawyzwnd(ptrSize,ptrList,isCoord,isHyper,way);
	}

	if (mode == 2)
	{
		draw3dwnd(ptrSize,ptrList,isCoord,isHyper,way);
	}
}

void moveScreenTo(SYSTEM *solar, int value)
{
	offsetX = -1 * solar[value].x;
	offsetY = -1 * solar[value].y;
	offsetZ = -1 * solar[value].z;

	xmax = MAX_VALUE/10;
	ymax = MAX_VALUE/10;
	zmax = MAX_VALUE/10;

	xmin = MIN_VALUE/10;
	ymin = MIN_VALUE/10;
	zmin = MIN_VALUE/10;
}

void gotoSystem(int ptrSize, SYSTEM *solar)
{
	char *input;

	int size;
	int value;
	int error = 0;

	input = questionWnd("GSCARD","Insert system COORD");

	value = atoi(input);

	if (value != 0)
	{
		if (value > ptrSize || value < 1)
		{
			error = 1;
		}
		else
		{
			moveScreenTo(solar,value);
		}
	}
	else
	{
		error = 1;
	}

	if(error)
	{
		warningWnd("ERROR","Wrong value!");
		getch();
	}

	free(input);
}

char* questionWnd(char *header, char *text)
{
	char inputbuf[MAX_INPUT_LEN]="",c='';
	int input_pos=0,the_end=0;

	drawWnd(160,180,320,100);

	setcolor(0);
	outtextxy(162,185,header);
	setcolor(4);
	outtextxy(162,200,text);
	outtextxy(162,268,"Press [ENTER] to confirm [ESC] to abort");

	moveto(165,220);

	setcolor(15);

	do
	{
		setfillstyle(SOLID_FILL, RED);
		bar(180,230,460,248);
		setcolor(0);
		outtextxy(185,235,inputbuf);

		c = getch();
		switch (c)
		{
			case 8: /* BACKSPACE */
				if (input_pos)
				{
					input_pos--;
					inputbuf[input_pos]=0;
				}
				break;
			case 13: /* RETURN */
				the_end = 1;
				break;
			case ESC: /* ESC */
				inputbuf[0]=0;
				the_end = 1;
				break;
			default:
				if (input_pos < MAX_INPUT_LEN-1 && c >= ' ' && c <= '~')
				{
					inputbuf[input_pos] = c;
					input_pos++;
					inputbuf[input_pos] = 0;
				}

		}

	} while (!the_end);

	return inputbuf;
}

void warningWnd(char *header, char *text)
{
	drawWnd(160,180,320,100);
	setcolor(0);
	outtextxy(162,185,header);
	setcolor(4);
	outtextxy(162,200,text);
	setcolor(15);
}

void systemInfoWnd(char *header)
{
	drawWnd(20,20,430, 400);
	setcolor(15);
	settextstyle(SMALL_FONT,HORIZ_DIR,6);
	outtextxy(40,40,"Привет браток, я умею в русский язык!");
}

void progressWnd(char *header, char *text, int current, int total)
{
	float di;
	float x;
	unsigned int MEM;
	char memMsg[50] = "";

    MEM = coreleft();
	sprintf(memMsg,"M: %d",MEM);


	di = (float) current/total;

	if (di < 0)
		di = -1 * di;

	x = (280 * di) + 180;

	if (current == 0){

	drawWnd(160,180,320,100);
	setcolor(0);
	outtextxy(162,185,header);
	setcolor(4);
	outtextxy(162,200,text);
	setcolor(15);

	setcolor(4);
	rectangle(180,230,460,248);}



	setfillstyle(SOLID_FILL, RED);
	bar(400,185,475,195);
	bar(180,230,x,248);
	outtextxy(400,185,memMsg);

	setcolor(0);

}

void drawWnd(int x, int y, int width, int height)
{
	setfillstyle(SOLID_FILL,BLACK);
	setcolor(4);
	bar(x,y,x+width,y+height);
	rectangle(x,y,x+width,y+height);

	setfillstyle(SOLID_FILL,RED);
	bar(x,y,x+width,y+16);
}

void drawPathWnd(WAYPOINT *wp,int oy)
{
	setfillstyle(SOLID_FILL,BLACK);
	setcolor(15);

	bar(WND_WIDTH,0,639,WND_HEIGHT-165);

	setfillstyle(SOLID_FILL,RED);
	setcolor(0);
	bar(WND_WIDTH,0,639,20);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
	outtextxy(WND_WIDTH+5,5,"PATH");

	setcolor(4);
	setlinestyle(1,0,1);
	line(WND_WIDTH,oy,639,oy);

	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(WND_WIDTH+5,oy+5,"PgUp/PgDn");
	setcolor(15);
	outtextxy(WND_WIDTH+80,oy+5,"-SELECT");

	if (wp->size < 15 && !pathListFlag)
	{
		adQuindett();
		pathListFlag = 1;

	}

}

void pathWnd(WAYPOINT *wp,int currentPoint,SYSTEM *ptrList)
{
	int i,j,oy,yStep = 15;
	char buf[30];

	oy = 25 + wp->size*yStep;
	drawPathWnd(wp,oy);


	setcolor(15);

	settextstyle(SMALL_FONT,HORIZ_DIR,4);

	for (i = 0,j = (wp->size -1); i < wp->size ;i++,j--)
	{
		if(i == currentPoint){
			setcolor(0);
            setfillstyle(SOLID_FILL,RED);
			bar(WND_WIDTH,25+i*yStep,639,25+i*yStep+15);
		}
		else{
			setcolor(15);
			setfillstyle(SOLID_FILL,BLACK);
            bar(WND_WIDTH,25+i*yStep,639,25+i*yStep+15);
		}

		sprintf(buf,"#%d: SA%d [%d;%d;%d]",i+1,wp->way[i],ptrList[i].x,ptrList[i].y,ptrList[i].z);
		outtextxy(WND_WIDTH+5,25+i*yStep,buf);
	}


}
