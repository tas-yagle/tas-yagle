
%rename (fopen) myfopen;
%rename (fputs) myfputs;
%rename (fclose) myfclose;
%rename (fflush) myfflush;

extern FILE *myfopen(const char *name, const char *mode);
extern int myfclose(FILE *f);
int myfputs (const char *s, FILE *stream);
void myfflush(FILE *f);

void runStatHiTas_sub( char *numrun, char *script, char *tool, char *resfile, int incremental, char *resdir);
void avt_McPostData( char *msg );
void avt_McInfo( char *msg );
char *avt_McAsk( char *msg );
int avt_McIsSlave();

QuickList *mbk_QuickListCreate();
void mbk_QuickListAppend(QuickList *ql, char *item);
StringListF *mbk_QuickListToTCL(QuickList *ql);
int mbk_QuickListLength(QuickList *ql);
void mbk_QuickListAppendDouble(QuickList *ql, double value);
DoubleList *mbk_QuickListComputeMeanVarMedian(QuickList *ql);
void mbk_QuickListDoubleFree(QuickList *ql);

HashTable *mbk_NewHashTable (int size);
void mbk_AddStringHashItem (HashTable *htable, char *key, char *value);
char *mbk_GetStringHashItem (HashTable *htable, char *key);
void mbk_FreeStringHashTable (HashTable *htable);
StringList *mbk_GetStringHashTableKeys (HashTable *htable);

