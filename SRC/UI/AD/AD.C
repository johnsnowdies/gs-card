#include <graphics.h>

#include "ui\ad\ad.h"
#include "ui\locale.h"

void gui_ad_quindett() {
  setcolor(4);
  rectangle(471, 300, 639, 459);

  setfillstyle(BKSLASH_FILL, RED);
  bar(471, 300, 639, 325);

  setfillstyle(SOLID_FILL, RED);
  bar(472, 326, 638, 459);

  setcolor(0);
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(475, 330, LC_AD_QUINDETT_HEADER);

  settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);

  setcolor(15);
  outtextxy(475, 340, LC_AD_QUINDETT_BIG);

  setcolor(0);
  setlinestyle(1, 10, 2);
  line(471, 374, 639, 374);

  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
  outtextxy(475, 380, LC_AD_QUINDETT_TEXT_1);
  outtextxy(475, 395, LC_AD_QUINDETT_TEXT_2);

  setcolor(15);
  settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 2);
  outtextxy(555, 395, LC_AD_QUINDETT_MONEY);

  rectangle(550, 395, 638, 420);

  setcolor(0);
  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
  outtextxy(475, 410, LC_AD_QUINDETT_TEXT_3);

  outtextxy(475, 425, LC_AD_QUINDETT_TEXT_4);
  setcolor(15);
  outtextxy(510, 425, LC_AD_QUINDETT_CODE);

  setfillstyle(SOLID_FILL, BLACK);
  bar(471, 453, 639, 459);
  
  settextstyle(SMALL_FONT,HORIZ_DIR,4);
  outtextxy(478,440, LC_AD_QUINDETT_GS);
  
}

void gui_ad_hypersoft() {
  /* HYPERSOFT LOGO */

  setcolor(4);
  setlinestyle(0, 0, 1);

  setfillstyle(SOLID_FILL, BLACK);
  bar(20, 40, 70, 100);

  rectangle(20, 40, 70, 100);
  settextstyle(DEFAULT_FONT, HORIZ_DIR, 6);
  outtextxy(25, 45, "H");
  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
  outtextxy(25, 88, "SOFT TM");
}

void gui_ad_upgrade() {
  /* UPGRADE NAG */
  rectangle(150, 21, 540, 80);

  setcolor(0);

  settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
  setfillstyle(SOLID_FILL, RED);
  bar(405, 21, 540, 80);
  outtextxy(410, 25, LC_AD_OUTDATED_HEAD_1);
  outtextxy(410, 45, LC_AD_OUTDATED_HEAD_2);

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  setcolor(4);
  outtextxy(155, 25, LC_AD_OUTDATED_TEXT_1);
  outtextxy(155, 40, LC_AD_OUTDATED_TEXT_2);
  outtextxy(155, 55, LC_AD_OUTDATED_TEXT_3);

  settextstyle(SMALL_FONT, HORIZ_DIR, 2);
  outtextxy(150, 83, LC_AD_UPGRADE_TEXT);
}

void gui_ad_legion() {
  /* legion */

  setcolor(0);
  bar(20, 315, 215, 470);

  settextstyle(GOTHIC_FONT, HORIZ_DIR, 4);
  outtextxy(25, 318, LC_AD_LEGION_HEAD_1);
  outtextxy(25, 350, LC_AD_LEGION_HEAD_2);

  setlinestyle(3, 0, 1);
  line(20, 390, 220, 390);

  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(25, 400, LC_AD_LEGION_TEXT_1);
  outtextxy(25, 420, LC_AD_LEGION_TEXT_2);
  outtextxy(25, 440, LC_AD_LEGION_TEXT_3);

  setlinestyle(0, 0, 1);
}

void gui_ad_pipboy() {
  /* PIPBOY */

  setcolor(0);
  setfillstyle(SOLID_FILL, 2);
  settextstyle(SMALL_FONT, HORIZ_DIR, 4);
  bar(420, 315, 620, 470);
  outtextxy(425, 316, LC_AD_PIPBOY_PRE);
  setfillstyle(BKSLASH_FILL, 8);
  bar(420, 330, 620, 346);
  setfillstyle(SLASH_FILL, 4);
  bar(420, 347, 620, 365);
  setcolor(0);
  settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
  setusercharsize(1, 1, 1, 1);
  setcolor(15);
  outtextxy(425, 334, LC_AD_PIPBOY_HEAD);
  setcolor(0);
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(425, 368, LC_AD_PIPBOY_TEXT_1);
  outtextxy(425, 378, LC_AD_PIPBOY_TEXT_2);
  outtextxy(425, 388, LC_AD_PIPBOY_TEXT_3);
  outtextxy(425, 398, LC_AD_PIPBOY_TEXT_4);
  outtextxy(425, 408, LC_AD_PIPBOY_TEXT_5);
  setfillstyle(SOLID_FILL, 4);
  bar(420, 428, 620, 470);
  setcolor(15);
  settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 4);
  outtextxy(430, 430, LC_AD_PIPBOY_GS);
}

void gui_ad_fleet() {
  /* TRADE FLEET */
  setcolor(4);
  rectangle(220, 315, 415, 388);
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(225, 320, LC_AD_FLEET_TEXT_1);
  outtextxy(225, 335, LC_AD_FLEET_TEXT_2);
  outtextxy(225, 350, LC_AD_FLEET_TEXT_3);
  outtextxy(225, 365, LC_AD_FLEET_TEXT_4);
}

void gui_ad_legals() {
  /* LEGALS */
  setcolor(4);
  rectangle(220, 392, 415, 470);
  settextstyle(SMALL_FONT, HORIZ_DIR, 5);
  outtextxy(225, 395, LC_AD_LEGALS_TEXT_1);
  outtextxy(225, 410, LC_AD_LEGALS_TEXT_2);
  outtextxy(225, 425, LC_AD_LEGALS_TEXT_3);
  settextstyle(SMALL_FONT, HORIZ_DIR, 2);
  outtextxy(225, 445, LC_AD_LEGALS_TEXT_4);
  outtextxy(225, 450, LC_AD_LEGALS_TEXT_5);
  outtextxy(225, 455, LC_AD_LEGALS_TEXT_6);
  outtextxy(225, 460, LC_AD_LEGALS_TEXT_7);
}

void gui_ad_loading() {
    setfillstyle(SOLID_FILL, BLACK);
    bar(0,0,640, 480);
  
  gui_ad_hypersoft();
  gui_ad_upgrade();
  gui_ad_legion();
  gui_ad_pipboy();
  gui_ad_legals();
  gui_ad_fleet();
}
