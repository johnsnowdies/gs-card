#define DELIMITER_CHAR ';'

int loadSolarFile(struct system_solar** list);
void saveFile(int ptrSize, struct system_solar* ptrList);

int load_game(struct game_state* state);
int save_game(struct game_state* state);
