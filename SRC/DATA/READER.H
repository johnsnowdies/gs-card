#define DELIMITER_CHAR ';'
int loadObjects(struct object** list);
int loadSolarFile(struct system_solar** list);

int load_game(struct game_state* state, char* filename);
int save_game(struct game_state* state);
