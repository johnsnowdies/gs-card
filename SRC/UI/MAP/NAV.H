/* nav.h -- navigation controller API */

void gui_map_nav_scale_minus(void);
void gui_map_nav_scale_plus(void);
void gui_map_nav_offset_x_plus(void);
void gui_map_nav_offset_x_minus(void);
void gui_map_nav_offset_y_plus(void);
void gui_map_nav_offset_y_minus(void);
void gui_map_nav_offset_z_plus(void);
void gui_map_nav_offset_z_minus(void);
void gui_map_nav_move_screen_to(struct system_solar* solar, int value);
void gui_map_nav_goto_system(int sol_size, struct system_solar* solar);
