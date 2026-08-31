#ifndef _MUSIC
#define _MUSIC

#define MS_TO_TICKS(ms) ((ms) / 55)
#define E_SFX_MODAL_SOUND enum sfx_modal_sound
E_SFX_MODAL_SOUND{NO_SOUND, SOUND_WARNING, SOUND_ERROR, SOUND_SUCCESS};

void music_play(int* notes, int* durations, int size);
void music_shutdown();
void music_stop();

void sfx_menu_move();
void sfx_screen_change();
void sfx_window_open();
void sfx_modal();
void sfx_hyperjump();
void sfx_error();

#endif
