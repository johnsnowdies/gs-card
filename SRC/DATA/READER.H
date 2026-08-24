#define DELIMITER_CHAR ';'
int load_bounds(BOUND_LINE** list);

int load_object(OBJECT** list);
int load_solar(SYSTEM** list);

int load_game_file(GAME_STATE* state, char* filename);
int save_game_file(GAME_STATE* state, char* filename);

void draw_4bit_bmp(char *filename, int px, int py);
