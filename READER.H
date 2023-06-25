#define DELIMITER_CHAR ';'

int getFileSize(char *filename);
void loadSolarFile(struct system_solar *list);
void saveFile(int ptrSize, struct system_solar *ptrList);