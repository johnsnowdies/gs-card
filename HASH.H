int getCounter();
TABLE * CreateHashtable(int size);
int GetHash(TABLE *hashtable, char * key);
ENTRY *NewPair(char *key, int value);
void Set(TABLE *hashtable, char *key, int value);
int Get(TABLE *hashtable, char *key);
