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
SYSTEM*     sol_list;
int         sol_size;

OBJECT*     obj_list;
int         obj_size;

BOUND_LINE* bnd_list;
int         bnd_size;

WAYPOINT wp;

int current_point;

/* Render flags */
int render_danger_objects      = 0;
int render_bounds              = 0;
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
    SCR_GAME_MENU,
    SCR_STATUS
};

/* ----------------------------------------------------------------
 * Game Data Structures
 * ---------------------------------------------------------------- */
char* data_ship_names[SHIP_COUNT] = {
    "Almighty's Boat", "Betsy Boat", "Old Imperial Trade Ship",
    "Gae Bulg Ship", "Nova Trade Carrier", "Bulk Carrier" 
};

int data_ship_tonnages[SHIP_COUNT] = {
    50, 80, 100, 150, 200, 400
};

char* data_hyper_names[HYPER_COUNT] = {
    "Hyper Engine Mark I", "Hyper Engine Mark II", 
    "Hyper Engine Mark III", "Hyper Engine [UMBRELLA]"
};

int data_hyper_fuel[HYPER_COUNT] = {
    10, 8, 5, 2
};

char* data_factions[FACTIONS_COUNT] = {
    "Crescent and Star Union", "Ireland-Mermaidian Confederation", "Sentinel Coalition Peacemakers", "Pirates"
};

int data_factions_colors[FACTIONS_COUNT] = {
    2, 14, 9, 4
};

char* data_sectors[SECTORS_COUNT] = {
    "Dhat", "Medinat", "Ghabkar", "Buraq", "Ben Vara", "Killoch Vairan", "Cuchulainn", "Danter", "Coalsack"
};

void game_mark_visited(GAMESTATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return;
    gs->visited[system >> 3] |= (1 << (system & 7));
}

int game_is_visited(GAMESTATE *gs, int system) {
    if (system < 0 || system >= sol_size || !gs->visited) return 0;
    return (gs->visited[system >> 3] >> (system & 7)) & 1;
}


/* ----------------------------------------------------------------
 * new_game -- initialise game state
 * ---------------------------------------------------------------- */
static void new_game(char *name, int sol_size)
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

    gs.visited_bytes = (sol_size + 7) / 8;
    gs.visited = (unsigned char*)malloc(gs.visited_bytes);
    if (gs.visited) {
        memset(gs.visited, 0, gs.visited_bytes);
    }

    /* Load ads, calculate hyper-threads, load objects */
    core_finder_calc_hyper_threads(sol_size, sol_list);
    obj_size = loadObjects(&obj_list);

    gui_map_nav_move_screen_to(sol_list, gs.current_system);

    game_mark_visited(&gs, gs.current_system);

    wp.size = 0;
    current_point = -1;

    save_game(&gs, "USER.SAV");
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
    enum game_screen prev_screen = SCR_MAP;
    
    /* New game Player name*/
    char* nameInput;

    srand((unsigned)time(NULL));

    sol_size = loadSolarFile(&sol_list);
    bnd_size = load_bounds(&bnd_list);

    gui_init();

    /* Splash screen */
    gui_splash();

    /* Show ads */
    gui_ad_loading();
    
    /* Draw Main Menu */
    gui_menu_wnd(mm_select, MAIN_MENU);


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

            if (KEY_D == c) {
                render_danger_objects    = render_danger_objects == 1 ? 0 : 1;
                show_danger_hyperthreads = show_danger_hyperthreads == 1 ? 0 : 1;
                show_danger_path_parts   = show_danger_path_parts == 1 ? 0 : 1;
                
                gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
            }

            if (KEY_P == c ){
                render_bounds = render_bounds == 1 ? 0 : 1;
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
                        game_mark_visited(&gs, gs.current_system);
                        gs.fuel -= data_hyper_fuel[gs.hyper_class];
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
                    prev_screen = cur_screen;
                    cur_screen = SCR_GAME_MENU;
                    mm_select = 0;
                    gui_menu_wnd(mm_select, GAME_MENU);
                }
            }

            break;
        /* ============================================================
         * SCR_GAME_MENU -- game menu on ESC
         * ============================================================ */
        case SCR_GAME_MENU:
            if (ESC == c){
                mm_select = 0;
                cur_screen = prev_screen;
                if (cur_screen == SCR_MAP)
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                if (cur_screen == SCR_STATUS)
                    gui_status_wnd();
            }
            /* Save game */
            if (mm_select == 0 && ENTER == c) {
                nameInput = gui_input_wnd("Save game", "Enter save file name", "USER.SAV");

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    if(save_game(&gs, nameInput) == 1)
                    {
                        cur_screen = prev_screen;
                        gui_warning_wnd("SUCCESS", "Game saved!");
                        getch();
                        if (cur_screen == SCR_MAP)
                            gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);

                        if (cur_screen == SCR_STATUS)
                            gui_status_wnd();
                    }
                    else
                    {
                        gui_warning_wnd("ERROR", "Save Error");
                        getch();
                        gui_menu_wnd(mm_select, GAME_MENU);
                    }
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select, GAME_MENU);
                }

            }

             /* Load game */
            if (mm_select == 1 && ENTER == c)
            {
                nameInput = gui_input_wnd("Load game", "Enter save file name", "USER.SAV");

                if (nameInput != NULL && nameInput[0] != '\0') 
                {
                    if(load_game(&gs, nameInput) == 1)
                    {
                        cur_screen = SCR_MAP;
                        dirty_topbar = 1;
                        gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                    }
                    else
                    {
                        gui_warning_wnd("ERROR", "File not found");
                        getch();
                        gui_menu_wnd(mm_select, GAME_MENU);
                    }
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select, GAME_MENU);
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
                    gui_menu_wnd(mm_select, GAME_MENU);
                }
            }

            if (DWN == c){
                if (mm_select < 2)
                {
                    mm_select++;
                    gui_menu_wnd(mm_select, GAME_MENU);
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
                    new_game(nameInput, sol_size);
                    cur_screen = SCR_MAP;
                    gui_map_wnd_draw(sol_size, sol_list, mode, isCoord, isHyper, &wp, current_point);
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select, MAIN_MENU);
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
                        gui_menu_wnd(mm_select, MAIN_MENU);
                    }
                } else {
                    gui_warning_wnd("ERROR", "Wrong value!");
                    getch();
                    gui_menu_wnd(mm_select, MAIN_MENU);
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
                    gui_menu_wnd(mm_select, MAIN_MENU);
                }
            }

            if (DWN == c){
                if (mm_select < 2)
                {
                    mm_select++;
                    gui_menu_wnd(mm_select, MAIN_MENU);
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

            if (ESC == c){
                prev_screen = cur_screen;
                cur_screen = SCR_GAME_MENU;
                mm_select = 0;
                gui_menu_wnd(mm_select, GAME_MENU);
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
