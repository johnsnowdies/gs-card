/* card.c -- main entry point, event loop, game state
 *
 * Event loop uses enum + switch so that adding a second screen
 * (e.g. cargo / ship management) is a matter of adding a new
 * case branch and a new input-handler function.
 *
 * Game globals are defined here and extern'd where needed.
 */

#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "keys.h"
#include "reader.h"
#include "objects.h"
#include "finder.h"
#include "ad.h"
#include "gui.h"
#include "mapwnd.h"
#include "nav.h"
#include "pathwnd.h"

/* ----------------------------------------------------------------
 * Game globals
 * ---------------------------------------------------------------- */
struct system_solar* ptrList;
struct object*       objList;
int    objSize;
struct waypoint     wp;

int ptrSize;
int currentPoint;

/* Render flags */
int render_danger_objects      = 0;
int show_danger_hyperthreads   = 0;
int show_danger_path_parts     = 0;
int dirty_path                 = 0;
int dirty_topbar               = 1;
int dirty_bottombar            = 1;

/* Game state */
struct game_state gs;

/* Exit signal */
int SIG_TERM = 0;

/* ----------------------------------------------------------------
 * Screen enumeration -- add new screens here
 * ---------------------------------------------------------------- */
enum GameScreen {
    SCR_MAP,
    SCR_MAIN_MENU,
    SCR_STATUS
};

/* ----------------------------------------------------------------
 * new_game -- initialise game state
 * ---------------------------------------------------------------- */
static void new_game(char *name)
{
    int i;

    strcpy(gs.captain_name, name);
    gs.balance            = 1000;
    gs.current_system     = rand() % ptrSize;
    if (gs.current_system < 0) gs.current_system = 0;
    if (gs.current_system >= ptrSize) gs.current_system = 0;

    gs.ship_type         = 1;
    gs.tonnage           = 100;
    gs.current_cargo     = 0;
    gs.cargo_value       = 0;
    gs.hyper_class       = 1;
    gs.smuggler_bay      = 0;
    gs.reputation        = 0;
    gs.missions_completed = 0;
    gs.fuel              = 100;

    /* Load ads, calculate hyper-threads, load objects */
    CalculateHyperThreads(ptrSize, ptrList);
    objSize = loadObjects(&objList);

    moveScreenTo(ptrList, gs.current_system);

    wp.size = 0;
    currentPoint = -1;

    save_game(&gs);
}

/* ----------------------------------------------------------------
 * load_game -- initialise game state from file
 * ---------------------------------------------------------------- */
static int load_save(char *filename)
{
    int result = 0;
    result = load_game(&gs, filename);

    if (result == 1)
    {
        /* Load ads, calculate hyper-threads, load objects */
        CalculateHyperThreads(ptrSize, ptrList);
        objSize = loadObjects(&objList);

        moveScreenTo(ptrList, gs.current_system);

        wp.size = 0;
        currentPoint = -1;
    }


    return result;
}


/* ----------------------------------------------------------------
 * main
 * ---------------------------------------------------------------- */
int main()
{
    int c = 0;
    int mm_select = 0;
    int isCoord = 1, isHyper = 0, mode = 1;  /* 1 = 2D, 2 = 3D, 3 = YZ */
    enum GameScreen cur_screen = SCR_MAIN_MENU;
    
    /* New game Player name*/
    char* nameInput;

    ptrSize = loadSolarFile(&ptrList);

    init();

    /* Splash screen */
    splash();

    adLoading();
    /* Draw Main Menu */
    mainMenuWnd(mm_select);


    /* ----------------------------------------------------------------
     * Main event loop
     * ---------------------------------------------------------------- */
    while (!SIG_TERM) {
        c = getch();

        switch (cur_screen) {

        /* ============================================================
         * SCR_MAP -- star map with navigation
         * ============================================================ */
        case SCR_MAP:
            if (F1 == c) {
                mode = (mode < 3) ? mode + 1 : 1;
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F2 == c) {
                isCoord = !isCoord;
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F4 == c) {
                scalePlus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F3 == c) {
                scaleMinus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F5 == c) {
                gotoSystem(ptrSize, ptrList);
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F6 == c) {
                isHyper = !isHyper;
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (F7 == c) {
                currentPoint = -1;
                GetWay(ptrSize, ptrList, &wp);
                dirty_path = 1;
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (LFT == c) {
                offsetXplus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (RHT == c) {
                offsetXminus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (UP == c) {
                if (mode == 3 || mode == 2) offsetZminus();
                if (mode == 1 || mode == 2) offsetYplus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (DWN == c) {
                if (mode == 3 || mode == 2) offsetZplus();
                if (mode == 1 || mode == 2) offsetYminus();
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (PUP == c) {
                if (wp.size) {
                    if (currentPoint == -1 || currentPoint == 0)
                        currentPoint = (wp.size - 1);
                    else
                        currentPoint--;

                    moveScreenTo(ptrList, wp.way[currentPoint]);
                    dirty_path = 1;
                    draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
                }
            }

            if (PDWN == c) {
                if (wp.size) {
                    if (currentPoint == -1 || currentPoint == (wp.size - 1))
                        currentPoint = 0;
                    else
                        currentPoint++;

                    moveScreenTo(ptrList, wp.way[currentPoint]);
                    dirty_path = 1;
                    draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
                }
            }

            if (KEY_D == c || KEY_D_UPPER == c) {
                render_danger_objects    = 1;
                show_danger_hyperthreads = 1;
                show_danger_path_parts   = 1;
                draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
            }

            if (ESC == c){
                SIG_TERM = 1;
            }

            break;

        /* ============================================================
         * SCR_MAIN_MENU -- game main menu after load
         * ============================================================ */
        case SCR_MAIN_MENU:
            /* Init new game */
            if (mm_select == 0 && ENTER == c)
            {
                nameInput = questionWnd("New game", "Enter captain name:", NULL);

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    new_game(nameInput);
                    cur_screen = SCR_MAP;
                    draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
                } else {
                    warningWnd("ERROR", "Wrong value!");
                    getch();
                    mainMenuWnd(mm_select);
                }
            }

            /* Load game */
            if (mm_select == 1 && ENTER == c)
            {
                nameInput = questionWnd("Load game", "Enter save file name", "USER.SAV");

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    if(load_save(nameInput) == 1)
                    {
                        cur_screen = SCR_MAP;
                        draw(ptrSize, ptrList, mode, isCoord, isHyper, &wp, currentPoint);
                    }
                    else
                    {
                        warningWnd("ERROR", "File not found");
                        getch();
                        mainMenuWnd(mm_select);
                    }
                } else {
                    warningWnd("ERROR", "Wrong value!");
                    getch();
                    mainMenuWnd(mm_select);
                }

                
            }
            

            /* Exit to DOS */
            if (mm_select == 2 && ENTER == c)
            {
                SIG_TERM = 1;
            }

            /* UP */
            if (UP == c){
                if (mm_select > 0)
                {
                    mm_select--;
                    mainMenuWnd(mm_select);
                }
            }

            if (DWN == c){
                if (mm_select < 2)
                {
                    mm_select++;
                    mainMenuWnd(mm_select);
                }
            }

        break;        
        } /* switch (cur_screen) */
    }

    /* Cleanup */
    free(ptrList);
    if (objList) free(objList);
    closegraph();
    return 0;
}
