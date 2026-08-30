/* nav.c -- navigation controller
 *
 * All functions here manipulate the viewport globals (offsetX/Y/Z,
 * xmin/xmax/ymin/ymax/zmin/zmax) that are defined in mapwnd.c.
 *
 * Public API declared in nav.h.
 */

#include <stdio.h>

#include "data\structs.h"
#include "data\keys.h"

#include "core\game.h"

#include "ui\gui.h"
#include "ui\locale.h"

#include "ui\map\mapnav.h"
#include "ui\map\mapwnd.h"
#include "ui\menu\menuwnd.h"

#include "music.h"

/* ----------------------------------------------------------------
 * Extern viewport globals (defined in mapwnd.c)
 * ---------------------------------------------------------------- */
extern int offsetX, offsetY, offsetZ;
extern float xmin, xmax, ymin, ymax, zmin, zmax;
extern float xdens, ydens;

extern int is_coord, is_hyper, mode;
extern int path_wnd_index;

extern float xmin, xmax;
extern float ymin, ymax;
extern float zmin, zmax;
extern int offsetX, offsetY, offsetZ;
extern float xdens, ydens;


extern WAYPOINT wp;

extern SYSTEM* sol_list;
extern unsigned int sol_size;

extern OBJECT* obj_list;
extern unsigned int obj_size;

extern BOUND_LINE* bnd_list;
extern unsigned int bnd_size;

extern GAME_STATE gs;

extern char* data_factions[FACTIONS_COUNT];
extern char* data_sectors[SECTORS_COUNT];
extern unsigned int data_factions_colors[FACTIONS_COUNT];

/* SCREEN NAVIGATION */
extern E_GAME_SCREEN cur_screen;
extern E_GAME_SCREEN prev_screen;

extern WND map_wnd;
extern WND root_wnd;


/* ----------------------------------------------------------------
 * Scale
 * ---------------------------------------------------------------- */

void gui_map_nav_scale_minus()
{
    if (xmax < MAX_VALUE * 3 && xmin > MIN_VALUE * 3) {
        xmax = xmax + MAX_VALUE / 10;
        ymax = ymax + MAX_VALUE / 10;
        zmax = zmax + MAX_VALUE / 10;

        xmin = xmin - MAX_VALUE / 10;
        ymin = ymin - MAX_VALUE / 10;
        zmin = zmin - MAX_VALUE / 10;
    } else {
        gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_NAV_ERROR_1, SOUND_ERROR);
        getch();
    }
}

void gui_map_nav_scale_plus()
{
    if (xmax > MAX_VALUE / 10 && xmin < MIN_VALUE / 10) {
        xmax = xmax - MAX_VALUE / 10;
        ymax = ymax - MAX_VALUE / 10;
        zmax = zmax - MAX_VALUE / 10;

        xmin = xmin + MAX_VALUE / 10;
        ymin = ymin + MAX_VALUE / 10;
        zmin = zmin + MAX_VALUE / 10;
    } else {
        gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_NAV_ERROR_2, SOUND_ERROR);
        getch();
    }
}

/* ----------------------------------------------------------------
 * Offset
 * ---------------------------------------------------------------- */
void gui_map_nav_offset_x_plus()  { offsetX += xmax / 10; }
void gui_map_nav_offset_x_minus() { offsetX -= xmax / 10; }
void gui_map_nav_offset_y_plus()  { offsetY += ymax / 10; }
void gui_map_nav_offset_y_minus() { offsetY -= ymax / 10; }
void gui_map_nav_offset_z_plus()  { offsetZ += zmax / 10; }
void gui_map_nav_offset_z_minus() { offsetZ -= zmax / 10; }

/* ----------------------------------------------------------------
 * moveScreenTo -- centre view on a system
 * ---------------------------------------------------------------- */
void gui_map_nav_move_screen_to(struct system_solar* solar, int value)
{
    offsetX = -1 * solar[value].x;
    offsetY = -1 * solar[value].y;
    offsetZ = -1 * solar[value].z;

    xmax = MAX_VALUE / 10;
    ymax = MAX_VALUE / 10;
    zmax = MAX_VALUE / 10;

    xmin = MIN_VALUE / 10;
    ymin = MIN_VALUE / 10;
    zmin = MIN_VALUE / 10;
}

/* ----------------------------------------------------------------
 * gotoSystem -- prompt for system number, then jump
 * ---------------------------------------------------------------- */
void gui_map_nav_goto_system(int sol_size, struct system_solar* solar)
{
    char* input;
    int value;
    int error = 0;
    char buf[50];

    sprintf(buf, "%d", gs.current_system);

    input = (char*)gui_input_wnd(&map_wnd, LC_NAV_NORMAL_HEAD, LC_NAV_INPUT_COORD, buf);
    value = atoi(input);
    free(input);

    if (value != 0) {
        if (value > sol_size || value < 1) {
            error = 1;
        } else {
            gui_map_nav_move_screen_to(solar, value);
        }
    } else {
        error = 1;
    }

    if (error) {
        gui_warning_wnd(&map_wnd, LC_GEN_ERROR_HEAD, LC_GEN_ERROR_INCORRECT_VALUE, SOUND_ERROR);
        getch();
    }
}

/* ----------------------------------------------------------------
 * KEY LISTENER HANDLER
 * ---------------------------------------------------------------- */

int gui_map_wnd_key(int ch, WND *parent)
{
    char buf[128];   /* ¤«ï á®®¡é¥­¨© */

    if (F1 == ch) {
        mode = (mode < 3) ? mode + 1 : 1;
        gui_map_wnd_draw();
    }
    if (F2 == ch) {
        is_coord = !is_coord;
        gui_map_wnd_draw();
    }
    if (F4 == ch) {
        gui_map_nav_scale_plus();
        gui_map_wnd_draw();
    }
    if (F3 == ch) {
        gui_map_nav_scale_minus();
        gui_map_wnd_draw();
    }
    if (F5 == ch) {
        gui_map_nav_goto_system(sol_size, sol_list);
        gui_map_wnd_draw();
    }
    if (F6 == ch) {
        is_hyper = !is_hyper;
        gui_map_wnd_draw();
    }
    if (F7 == ch) {
        if (core_finder_get_way(&wp)) {
            gui_map_wnd_draw();
            gui_map_path_wnd();
        }else{
            gui_map_wnd_draw();
        }
    }
    if (LFT == ch) {
        gui_map_nav_offset_x_plus();
        gui_map_wnd_draw();
    }
    if (RHT == ch) {
        gui_map_nav_offset_x_minus();
        gui_map_wnd_draw();
    }
    if (UP == ch) {
        if (mode == 3 || mode == 2) gui_map_nav_offset_z_minus();
        if (mode == 1 || mode == 2) gui_map_nav_offset_y_plus();
        gui_map_wnd_draw();
    }
    if (DWN == ch) {
        if (mode == 3 || mode == 2) gui_map_nav_offset_z_plus();
        if (mode == 1 || mode == 2) gui_map_nav_offset_y_minus();
        gui_map_wnd_draw();
    }
    if (PUP == ch) {
        if (wp.size) {
            if (path_wnd_index == -1 || path_wnd_index == 0)
                path_wnd_index = (wp.size - 1);
            else
                path_wnd_index--;
            gui_map_nav_move_screen_to(sol_list, wp.way[path_wnd_index]);
            gui_map_path_wnd();
            gui_map_wnd_draw();
        }
    }
    if (PDWN == ch) {
        if (wp.size) {
            if (path_wnd_index == -1 || path_wnd_index == (wp.size - 1))
                path_wnd_index = 0;
            else
                path_wnd_index++;
            gui_map_nav_move_screen_to(sol_list, wp.way[path_wnd_index]);
            gui_map_path_wnd();
            gui_map_wnd_draw();
        }
    }

    if (TAB == ch) {
        cur_screen = SCR_STATUS;
        gui_status_wnd();
    }
    if (ENTER == ch) {
        if (wp.size > 1 && wp.way[0] == gs.current_system) {

            sprintf(buf, LC_CARD_READY_TO_JUMP, wp.way[0], wp.way[1]);
            if (gui_confirm_wnd(&map_wnd, LC_CARD_JUMP_WND_HEAD, buf) == 0) {
                int game_over = 0;
                int i;
                
                gs.prev_system = gs.current_system;
                gs.current_system = wp.way[1];
                                
                /* Keep Path Window open until reached end */
                if (wp.size != 2 && gs.upgrade_continuous_jump
                    ){
                    for(i = 0; i <= wp.size - 1; i++)
                        wp.way[i] = wp.way[i+1];
                    wp.size--;
                    gui_map_path_wnd();
                }else{
                    wp.size = 0;
                    gui_map_wnd_draw();
                }

                game_over = core_game_run_event(1);
                gui_map_nav_move_screen_to(sol_list, gs.current_system);

                if (!game_over){
                    char lines[1][100];
                    gui_bars_common_top();
                    core_game_event_gas_station();
                    gui_bars_common_top();
                    gui_map_wnd_draw();
                } else {
                    gui_bars_common_top();
                    gui_ad_loading();
                    cur_screen = SCR_MAIN_MENU;
                    gui_menu_wnd(&root_wnd, 0, MAIN_MENU);
                }
            } else {
                wp.size = 0;
                gui_bars_common_top();
                gui_map_wnd_draw();
            }
        }
    }
    if (ESC == ch) {
        if (wp.size) {
            wp.size = 0;
            gui_map_wnd_draw();
        } else {
            prev_screen = cur_screen;
            cur_screen = SCR_GAME_MENU;
            gui_menu_wnd(&map_wnd, 0, 2);
        }
    }
    return 0;
}
