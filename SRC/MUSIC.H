#define MS_TO_TICKS(ms) ((ms) / 55)

void music_play(int *notes, int *durations, int size);
void music_shutdown();
void music_stop();


void sfx_menu_move();
void sfx_screen_change();
void sfx_window_open();
void sfx_modal();
void sfx_hyperjump();
void sfx_error();