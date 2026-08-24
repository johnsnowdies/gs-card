#define DELIMITER_CHAR ';'
int load_bounds(BOUND_LINE** list);

int load_object(struct object** list);
int load_solar(struct system_solar** list);

int load_game_file(struct game_state* state, char* filename);
int save_game_file(struct game_state* state, char* filename);

void draw_4bit_bmp(char *filename, int px, int py);
