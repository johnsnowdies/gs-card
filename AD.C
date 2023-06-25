#include <graphics.h>
#include <alloc.h>
#include "structs.h"
#include "gui.h"
#include "reader.h"
#include "keys.h"
#include "ad.h"

void adQuindett()
{
	setcolor(4);
	rectangle (470,300,639,479);

	setfillstyle(BKSLASH_FILL,RED);
	bar(470,300,639,325);

	setfillstyle(SOLID_FILL,RED);
	bar(471,326,638,453);

	setcolor(0);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(475,330,"Become a soldier of...");

	settextstyle(SANS_SERIF_FONT,HORIZ_DIR,4);

	setcolor(15);
	outtextxy(475,340,"Fortune!");

	setcolor(0);
	setlinestyle(1,10,2);
	line(470,374,639,374);

	settextstyle(SMALL_FONT,HORIZ_DIR,4);
	outtextxy(475,380,"Quendett awaits you!");
	outtextxy(475,395,"Just imagine ");

	setcolor(15);
	settextstyle(SANS_SERIF_FONT,HORIZ_DIR,2);
	outtextxy(555,395,"2500 $$");

	rectangle(550,395,638,420);

	setcolor(0);
	settextstyle(SMALL_FONT,HORIZ_DIR,4);
	outtextxy(475,410,"trooper offer");

	outtextxy(475,425,"code:");
	setcolor(15);
	outtextxy(510,425,"\"I WANT TO QUINDETT!\"");

	setfillstyle(SOLID_FILL,BLACK);
	bar(471,453,639,479);
	/*
	settextstyle(SMALL_FONT,HORIZ_DIR,4);
	outtextxy(478,433,"GS:1:234:QA");
	*/
}

void adHypersoft()
{
    /* HYPERSOFT LOGO */

	setcolor(4);


	setfillstyle(SOLID_FILL,BLACK);
	bar(20,20,70,80);

	rectangle (20,20,70,80);
	settextstyle(DEFAULT_FONT, HORIZ_DIR, 6);
	outtextxy( 25,25,"H");
	settextstyle(SMALL_FONT, HORIZ_DIR, 4);
	outtextxy( 25,68,"SOFT TM");

}

void adUpgrade()
{
    /* UPGRADE NAG */
	rectangle(150,20,540,80);

	setcolor(0);

	settextstyle(DEFAULT_FONT, HORIZ_DIR,2);
	setfillstyle(SOLID_FILL,RED);
	bar(405,20,540,80);
	outtextxy(410,25,"CARD");
	outtextxy(410,45,"OUTDATED");

	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	setcolor(4);
	outtextxy(155,25,"This GSCARD is outdated for 5401");
	outtextxy(155,40,"years! Call us at GS:1:3218-HSFT");
	outtextxy(155,55,"to purchase new version!");

	settextstyle(SMALL_FONT,HORIZ_DIR,2);
	outtextxy(150,83,"Hypersoft ILTD takes no responsibility for usaging outdated GSCARDs. ");

}

void adLegion()
{
    /* legion */

	setcolor(0);
	bar(20,315,215,470);

	settextstyle(GOTHIC_FONT,HORIZ_DIR,4);
	outtextxy(25, 318,"Legion awaits");
	outtextxy(25, 350,"you, Soldier!");

	setlinestyle(3,0,1);
	line(20,390,220,390);

	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(25,400,"Start your NEW life with");
	outtextxy(25,420,"Imperial military forces");
	outtextxy(25,440,"recruit today at GS:1:10A");

	setlinestyle(0,0,1);

}

void adPipboy()
{
    /* PIPBOY */

	setcolor(0);
	setfillstyle(SOLID_FILL,2);
	settextstyle(SMALL_FONT,HORIZ_DIR,4);
	bar(420,315,620,470);
	outtextxy(425,316,"Color display? No tricks, just");
	setfillstyle(BKSLASH_FILL,8);
	bar(420,330,620,346);
	setfillstyle(SLASH_FILL,4);
	bar(420,347,620,365);
	setcolor(0);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,4);
	setusercharsize(1,1,1,1);
	setcolor(15);
	outtextxy(425,334,"PIPBOY");
	setcolor(0);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(425,368,"Best wearable hardware in");
	outtextxy(425,378,"whole multiverse!");
	outtextxy(425,388,"Supports all Hypersoft's ");
	outtextxy(425,398,"GSCARD, it can make your");
	outtextxy(425,408,"hyper-run faster!");
	setfillstyle(SOLID_FILL,4);
	bar(420,428,620,470);
	setcolor(15);
	settextstyle(SANS_SERIF_FONT,HORIZ_DIR,4);
	outtextxy(430,430, "GS:14:24-PB");

}

void adFleet()
{
    /* TRADE FLEET */
	setcolor(4);
	rectangle(220, 315, 415, 388);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(225, 320, "  IMPERIAL TRADE FLEET");
	outtextxy(225, 335, "HIRE: engieners, pilots");
	outtextxy(225, 350, "OFFERS: 2000 $$ per year");
	outtextxy(225, 365, "CONTACT: GS:1:15:SF-HR");

}

void adLegals()
{
    /* LEGALS */
	setcolor(4);
	rectangle(220, 392, 415, 470);
    settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(225, 395, " GSCARD, Hypersoft ILTD");
	outtextxy(225, 410, "  All rights reserved");
	outtextxy(225, 425, "Support line: GS:1:3218:S");
	settextstyle(SMALL_FONT,HORIZ_DIR,2);
	outtextxy(225,445,"Warntary void if program launched. Hypersoft ILTD");
	outtextxy(225,450,"is trademark of Imperial Trade Fleet. Illegal copy");
	outtextxy(225,455,"dissasembling and hacking this software disallowed");
	outtextxy(225,460,"          34M315 Terra, Washington DC");

}

void adLoading(){
	adHypersoft();
	adUpgrade();
	adLegion();
	adPipboy();
	adLegals();
	adFleet();
}


