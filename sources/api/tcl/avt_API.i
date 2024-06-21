StringList      *avt_PinList            (char *name);
StringList      *avt_SigList            (char *name);
extern void      avt_config             (char *var, char *val); 
extern void      avt_Config             (char *var, char *val); 
char *avt_GetConfig(char *var);
char            *avt_gettcldistpath     ();
void             avt_SetBlackBoxes      (List *argv);
BehavioralFigure *avt_LoadBehavior(char *name, char *format);
void avt_DriveBehavior(BehavioralFigure *befig, char *format);
void             avt_LoadFile           (char *filename, char *format);
void             avt_EncryptSpice(char *inputname, char *outputname);
void             avt_StartWatch         (char *name);
void             avt_StopWatch          (char *name);
char            *avt_PrintWatch         (char *name);
unsigned long    avt_GetMemoryUsage     ();
Netlist         *avt_GetNetlist         (char *name);
void             avt_DriveNetlist       (Netlist *lf, char *filename, char *format);
void             avt_RemoveResistances  (Netlist *lf, char *nameregex);
void             avt_RemoveCapacitances (Netlist *lf, char *nameregex);
void             avt_FlattenNetlist     (Netlist *lf, char *level);
void             avt_ViewNetlist        (char *name);
void avt_ViewSignal(char *name, char *signal);
void             avt_RemoveNetlist      (char *name);
void             avt_DriveSignalInfo    (Netlist*, char*, char* );
void avt_SetSEED(int val);
void             avt_AddRC              (Netlist *lofig, int maxwire, CapaValue minc, CapaValue maxc, double minr, double maxr);
void             avt_AddCC              (Netlist *lofig, CapaValue minc, CapaValue maxc);
StringList      *avt_GetCatalog         ();
void             avt_SetCatalog         (List *argv);
void avt_DisplayNetlistHierarchy(FILE *f, char *netlistname, int maxdepth);
void avt_DisplayResistivePath(FILE *f, Netlist *lf, char *connector1, char *connector2);
void avt_CheckTechno(char *label, char *tn, char *tp);
int avt_BuildID();
int avt_RegexIsMatching(char *nametocheck, char *template);
void *_NULL_();
int avt_CodeName(char *name);

Board *Board_CreateBoard();
void Board_SetSize(Board *B, int col, int size, char align);
BoardColumn *Board_NewLine(Board *B);
void Board_SetValue(Board *B, int col, char *val);
void Board_Display(FILE *f, Board *B, char *LP);
void Board_FreeBoard(Board *B);
void Board_NewSeparation(Board *B);

void avt_SetMainSeed(unsigned int value);
void avt_SetGlobalSeed(unsigned int value);
unsigned int avt_GetMainSeed();
unsigned int avt_GetGlobalSeed();

/*void help (char *subject);*/
void avt_banner (const char *tool, const char *comment, const char *date);

