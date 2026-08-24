#define DELIMITER_CHAR ';'
int data_reader_load_bounds(BOUND_LINE** list);

int data_reader_load_objects(OBJECT** list);
int data_reader_load_systems(SYSTEM** list);

int data_reader_load_game_file(GAME_STATE* state, char* filename);
int data_reader_save_game_file(GAME_STATE* state, char* filename);

void data_reader_draw_bmp(char *filename, int px, int py);
