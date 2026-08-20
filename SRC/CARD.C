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

#include "data\keys.h"
#include "data\structs.h"
#include "data\tables.h"
#include "data\reader.h"

#include "core\objects.h"
#include "core\finder.h"

#include "ui\ad.h"
#include "ui\splash.h"
#include "ui\gui.h"
#include "ui\menuwnd.h"
#include "ui\statwnd.h"
#include "ui\map\mapwnd.h"
#include "ui\map\pathwnd.h"
#include "ui\map\nav.h"

/* ----------------------------------------------------------------
 * Game globals
 * ---------------------------------------------------------------- */
struct system_solar* sol_list;
int sol_size;

struct object*       obj_list;
int    obj_size;

struct waypoint     wp;

int current_point;

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
enum game_screen {
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
    gs.balance            = 100;
    gs.current_system     = rand() % sol_size;
    if (gs.current_system < 0) gs.current_system = 0;
    if (gs.current_system >= sol_size) gs.current_system = 0;

    gs.ship_type         = 0;
    gs.tonnage           = 50;
    gs.current_cargo     = 0;
    gs.cargo_value       = 0;
    gs.hyper_class       = 0;
    gs.smuggler_bay      = 0;
    gs.reputation        = 0;
    gs.missions_completed = 0;
    gs.fuel              = 100;

    /* Load ads, calculate hyper-threads, load objects */
    core_finder_calc_hyper_threads(sol_size, sol_list);
    obj_size = loadObjects(&obj_list);

    gui_map_nav_move_screen_to(sol_list, gs.current_system);

    wp.size = 0;
    current_point = -1;

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
        core_finder_calc_hyper_threads(sol_size, sol_list);
        obj_size = loadObjects(&obj_list);

        gui_map_nav_move_screen_to(sol_list, gs.current_system);

        wp.size = 0;
        current_point = -1;
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
    char* buf[50];
    int isCoord = 1, isHyper = 0, mode = 1;  /* 1 = 2D, 2 = 3D, 3 = YZ */
    enum game_screen cur_screen = SCR_MAIN_MENU;
    
    /* New game Player name*/
    char* nameInput;

    srand((unsigned)time(NULL));

    sol_size = loadSolarFile(&sol_list);

    gui_init();

    /* Splash screen */
    gui_splash();

    /* Show ads */
    gui_ad_loading();
    
    /* Draw Main Menu */
    gui_menu_wnd(mm_select);


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
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F2 == c) {
                isCoord = !isCoord;
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F4 == c) {
                gui_map_nav_scale_plus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F3 == c) {
                gui_map_nav_scale_minus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F5 == c) {
                gui_map_nav_goto_system(sol_size, sol_list);
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F6 == c) {
                isHyper = !isHyper;
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (F7 == c) {
                current_point = -1;
                core_finder_get_way(sol_size, sol_list, &wp);
                dirty_path = 1;
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (LFT == c) {
                gui_map_nav_offset_x_plus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (RHT == c) {
                gui_map_nav_offset_x_minus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (UP == c) {
                if (mode == 3 || mode == 2) gui_map_nav_offset_z_minus();
                if (mode == 1 || mode == 2) gui_map_nav_offset_y_plus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (DWN == c) {
                if (mode == 3 || mode == 2) gui_map_nav_offset_z_plus();
                if (mode == 1 || mode == 2) gui_map_nav_offset_y_minus();
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (PUP == c) {
                if (wp.size) {
                    if (current_point == -1 || current_point == 0)
                        current_point = (wp.size - 1);
                    else
                        current_point--;

                    gui_map_nav_move_screen_to(sol_list, wp.way[current_point]);
                    dirty_path = 1;
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                }
            }

            if (PDWN == c) {
                if (wp.size) {
                    if (current_point == -1 || current_point == (wp.size - 1))
                        current_point = 0;
                    else
                        current_point++;

                    gui_map_nav_move_screen_to(sol_list, wp.way[current_point]);
                    dirty_path = 1;
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                }
            }

            if (KEY_D == c || KEY_D_UPPER == c) {
                render_danger_objects    = 1;
                show_danger_hyperthreads = 1;
                show_danger_path_parts   = 1;
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (TAB == c){
                cur_screen = SCR_STATUS;
                gui_status_wnd();
            }

            if (ENTER == c){
                if (wp.size > 1 && wp.way[0] == gs.current_system){
                    sprintf(buf, "Ready to jump from SA.%d to SA.%d ?", wp.way[0], wp.way[1]);
                    if (gui_bool_wnd("JUMP INITIATED", buf)) {
                        gs.current_system = wp.way[1];
                        gs.fuel -= hyper_fuel[gs.hyper_class];
                        sprintf(buf, "Welcome to SA.%d", wp.way[1]);
                        wp.size = 0;
                        dirty_topbar = 1;
                        gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);    
                        gui_warning_wnd("JUMP RESULT", buf);
                        getch();
                        gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);

                    }
                    else{
                        gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);    
                    }
                }
            }

            if (ESC == c){
                if (wp.size){
                    wp.size = 0;
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                }
                else{
                    SIG_TERM = 1;    
                }
                
            }

            break;

        /* ============================================================
         * SCR_MAIN_MENU -- game main menu after load
         * ============================================================ */
        case SCR_MAIN_MENU:
            /* Init new game */
            if (mm_select == 0 && ENTER == c)
            {
                nameInput = gui_input_wnd("New game", "Enter captain name:", NULL);

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    new_game(nameInput);
                    cur_screen = SCR_MAP;
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select);
                }
            }

            /* Load game */
            if (mm_select == 1 && ENTER == c)
            {
                nameInput = gui_input_wnd("Load game", "Enter save file name", "USER.SAV");

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    if(load_save(nameInput) == 1)
                    {
                        cur_screen = SCR_MAP;
                        gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                    }
                    else
                    {
                        gui_warning_wnd("ERROR", "File not found");
                        getch();
                        gui_menu_wnd(mm_select);
                    }
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select);
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
                    gui_menu_wnd(mm_select);
                }
            }

            if (DWN == c){
                if (mm_select < 2)
                {
                    mm_select++;
                    gui_menu_wnd(mm_select);
                }
            }

        break;

        case SCR_STATUS:
            if (TAB == c){
                if (wp.size > 0)
                    dirty_path = 1;
                cur_screen = SCR_MAP;
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }


        break;
        } /* switch (cur_screen) */
    }

    /* Cleanup */
    free(sol_list);
    if (obj_list) free(obj_list);
    closegraph();
    return 0;
}
