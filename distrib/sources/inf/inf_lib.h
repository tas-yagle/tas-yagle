/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : INF  v1.09                                                  */
/*    Fichier : inf109.h                                                    */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#ifndef INF
#define INF

#define INF_DONTLOG 2

#define INF_JOK '*'
#define INF_ANY_VALUES ((char *)0xabcdef01)

/* TAS info type constants */

#define INF_LL_STUCKVSS        0
#define INF_LL_STUCKVDD        1
#define INF_LL_PRECHARGE       2
#define INF_LL_NOTPRECHARGE    3
#define INF_LL_BYPASSIN        4
#define INF_LL_BYPASSOUT       5
#define INF_LL_BYPASS          6
#define INF_LL_INTER           7
#define INF_LL_RC             10
#define INF_LL_NORC           11
#define INF_LL_FLIPFLOP       12
//#define INF_LL_RCN            13
#define INF_LL_NORISING       15
#define INF_LL_NOFALLING      16
#define INF_LL_BREAK          17
#define INF_LL_ONLYEND        18
//#define INF_LL_CAPAOUT        19
//#define INF_LL_SLOPEIN        20
#define INF_LL_CLOCK          21
#define INF_LL_ASYNCHRON      22

// INF TYPES
#define INF_ASSOCLIST      "{Association list}"
#define INF_LISTOFLIST     "{List of list}"
#define INF_MISCDATA       "{Misc data}"

// operation condition
#define INF_OPERATING_CONDITION "+Operating Conditions+"
#define INF_TEMPERATURE         "Temperature"

// power definition
#define INF_POWER               "Supply"

// ckprech
#define INF_CKPRECH             "CkPrech"

#define INF_SENSITIVE           "Sensitive Timing Point"
#define INF_SUPPRESS            "Suppress From Behaviour"
#define INF_FLIPFLOP            "FlipFlop"

// stb informations
#define INF_STBOUT_SPECIN       "Result Input Stability"
#define INF_STBOUT_SPECOUT      "Result Output Stability"
#define INF_STBOUT_SPECMEM      "Result Memory Stability"
#define INF_STBOUT_SPECINODE    "Result Internal Node Stability"

#define INF_VERIFUP                  "Up"
#define INF_VERIFDOWN                "Down"
#define INF_VERIFRISE                "Rising"
#define INF_VERIFFALL                "Falling"
#define INF_NOVERIF                  "No verification"
#define INF_DISABLE_PATH             "+Disabled Paths+"
//#define INF_FALSE_PATH             "+False Paths+"
#define INF_MULTICYCLE_PATH          "+Multicycle Paths+"

#define INF_NOCHECK                  "No Stb Error Check"

// other inf types
#define INF_YES            "Yes"
#define INF_NO             "No"
#define INF_ALL            "All"
#define INF_IN             "In"
#define INF_OUT            "Out"
#define INF_ONLYEND        "Only End"


// some infsiglist elements
//#define INF_RCN            "RCN"
//#define INF_ELM            "ELM"
#define INF_DONTCROSS      "Don't Cross"
#define INF_TRANSPARENT    "Transparent"

#define INF_DELAY          "+Path Delay+"

#define INF_NB_SIGTYPE     24
/* si ajout d'un define pour les siglist
 * ne pas oublier de modifier le driver :
 * dans la fonction inf_driveSiglist
 * la boucle va de 0 a la valeur du dernier define ci-dessus */


#define INF_UP             170
#define INF_DOWN           180
#define INF_UPDOWN         190
#define INF_CK             200
#define INF_NOTHING        201

#define INF_PERIOD          210

#define INF_INIT_VAL (float) -1e31
#define INF_MIN_VAL  (float) -1e30

//
// added sections of documentation for each token
//  Hitas Reference Guide 2.7 (08/18/2005)
//
// x.x.x.x  [I|II|III|??|--]
// doc id    level of verification
//

// ??
#define INF_IGNORE_INSTANCE    "+Ignore Instances+"
#define INF_IGNORE_TRANSISTOR  "+Ignore Transistors+"
#define INF_IGNORE_CAPACITANCE "+Ignore Capacitances+"
#define INF_IGNORE_RESISTANCE  "+Ignore Resistances+"
#define INF_IGNORE_PARASITICS  "+Ignore Parasitics+"
#define INF_IGNORE_DIODE       "+Ignore Diodes+"
#define INF_IGNORE_NAMES       "+Ignore Names+"
    
// I
#define INF_STUCK              "Stuck To Value"
#define INF_DISABLE_GATE_DELAY "Disabled Gate Delay"

// I
#define INF_MUXU           "+Mux UP MUTEX+"
#define INF_MUXD           "+Mux DOWN MUTEX+"
#define INF_CMPU           "+Cmp UP MUTEX+"
#define INF_CMPD           "+Cmp DOWN MUTEX+"

// NM
#define INF_RENAME         "+Signal Renaming+"

// I
#define INF_INPUTS              "Circuit Input"

// II
#define INF_STOP                "Functional Analysis Stop Point"

// II
#define INF_DIROUT              "Transistor Orientation"

// III
#define INF_DLATCH              "Dynamic Latch"
#define INF_MEMSYM              "Memsyms"
#define INF_PRECHARGE      "Precharge"
#define INF_MODELLOOP      "Model Latch Loop"

// II
#define INF_CKLATCH             "CkLatch"

// II
#define INF_NOTLATCH             "NotLatch"
#define INF_KEEP_TRISTATE_BEHAVIOUR "KeepTristateBehaviour"

// II
#define INF_BREAK          "Break Point"
#define INF_STRICT_SETUP   "Strict Setup"

// III
#define INF_BYPASS         "Bypass"

// III
#define INF_INTER          "Inter"

// I
#define INF_PIN_RISING_SLEW   "Rising slope"
#define INF_PIN_FALLING_SLEW  "Falling slope"
#define INF_PIN_LOW_VOLTAGE   "Low Voltage"
#define INF_PIN_HIGH_VOLTAGE  "High Voltage"

// III
#define INF_LL_PATHSIGS       14

// III
#define INF_PATHIN              "Input of path"
#define INF_PATHOUT             "Output of path"

// III
#define INF_PATHDELAYMARGINPREFIX "Path Delay Margin"
#define INF_PATHDELAYMARGINCLOCK "Clock"
#define INF_PATHDELAYMARGINDATA "Data"
#define INF_PATHDELAYMARGINMIN "Min"
#define INF_PATHDELAYMARGINMAX "Max"
#define INF_PATHDELAYMARGINRISE "Rise"
#define INF_PATHDELAYMARGINFALL "Fall"

// III
#define INF_NORISING       "No Rising"
#define INF_NOFALLING      "No Falling"

// I
#define INF_CONNECTOR_DIRECTION "Connector Direction"

// II
#define INF_RC             "RC"

// 1.4.6.a --

// I
#define INF_SLOPEIN        "SlopeIN"

// I
#define INF_CAPAOUT        "CapaOUT"
#define INF_CAPAOUT_LW     "CapaOUT_LW"
#define INF_CAPAOUT_L      "CapaOUT_L"

#define INF_OUTPUT_CAPACITANCE "Output Capacitance"

// I
#define INF_ASYNCHRON      "Asynchron"

// II
#define INF_CLOCK_TYPE          "Clock"
#define INF_MIN_RISE_TIME       "Min rise time"
#define INF_MAX_RISE_TIME       "Max rise time"
#define INF_MIN_FALL_TIME       "Min fall time"
#define INF_MAX_FALL_TIME       "Max fall time"
#define INF_CLOCK_PERIOD        "Clock period"
#define INF_MASTER_CLOCK        "Master Clock"
#define INF_MASTER_CLOCK_EDGES  "Master Clock Edges"
#define INF_LATENCY_RISE_MIN    "Min rise Latency"
#define INF_LATENCY_RISE_MAX    "Max rise Latency"
#define INF_LATENCY_FALL_MIN    "Min fall Latency"
#define INF_LATENCY_FALL_MAX    "Max fall Latency"


#define INF_STB_HEADER          "+STB header+"
#define INF_DEFAULT_PERIOD      "Default period"
#define INF_SETUPMARGIN         "Setup margin"
#define INF_HOLDMARGIN          "Hold margin"

#define INF_VERIF_STATE              "Verification State"

#define INF_ASYNC_CLOCK_GROUP        "Asynchronous clock group"
#define INF_ASYNC_CLOCK_GROUP_PERIOD "Asynchronous clock group period"

#define INF_EQUIV_CLOCK_GROUP        "Equivalent clock group"

#define INF_PREFERED_CLOCK           "Prefered clock"

// I
#define INF_SPECIN              "SpecIn Stability"

// I
#define INF_SPECOUT             "SpecOut Stability"

// III
#define INF_CROSSTALKMUXU  "+Mux UP Crosstalk MUTEX+"
#define INF_CROSSTALKMUXD  "+Mux DOWN Crosstalk MUTEX+"

#define INF_RISE_SETUP    "Rise Setup"
#define INF_RISE_HOLD     "Rise Hold"
#define INF_FALL_SETUP    "Fall Setup"
#define INF_FALL_HOLD     "Fall Hold"

#define INF_SWITCHING_PROBABILITY "Switching Proba"
#define INF_STB_NODE_FLAGS "StbNode flags"

// Marking of FCL types by INF
#define INF_MARKSIG              "Signal Marking"
#define INF_MARKTRANS            "Transistor Marking"
#define INF_MARKRS               "RS Marking"

/* Transferred signal types */

#define INF_NET_LATCH            ((int) 0x80000000 )
#define INF_NET_MEMSYM           ((int) 0x40000000 )
#define INF_NET_RS               ((int) 0x20000000 )
#define INF_NET_FLIPFLOP         ((int) 0x10000000 )
#define INF_NET_MASTER           ((int) 0x08000000 )
#define INF_NET_SLAVE            ((int) 0x04000000 )
#define INF_NET_BLOCKER          ((int) 0x00000001 )
#define INF_NET_NOFALSEBRANCH    ((int) 0x00000008 )
#define INF_NET_VDD              ((int) 0x00000010 )
#define INF_NET_VSS              ((int) 0x00000020 )

/* Transferred transistor types */

#define INF_TRANS_BLEEDER          ((int) 0x80000000 )
#define INF_TRANS_FEEDBACK         ((int) 0x40000000 )
#define INF_TRANS_COMMAND          ((int) 0x20000000 )
#define INF_TRANS_NOT_FUNCTIONAL   ((int) 0x10000000 )
#define INF_TRANS_BLOCKER          ((int) 0x00000001 )
#define INF_TRANS_SHORT            ((int) 0x00000010 )
#define INF_TRANS_UNUSED           ((int) 0x00000020 )

// characteristics
#define INF_CHARAC_DATA "Is a Data"

#define INF_RS_ILLEGAL     1
#define INF_RS_LEGAL       2
#define INF_RS_MARKONLY    3

#define INF_CONDITIONS         "+Config/Conditions+"

// clock uncertainty
#define INF_CLOCK_UNCERTAINTY "Clock Uncertainty"
#define INF_CLOCK_UNCERTAINTY_SETUP       0x1
#define INF_CLOCK_UNCERTAINTY_HOLD        0x2
#define INF_CLOCK_UNCERTAINTY_END_FALL    0x4
#define INF_CLOCK_UNCERTAINTY_END_RISE    0x8
#define INF_CLOCK_UNCERTAINTY_START_FALL  0x10
#define INF_CLOCK_UNCERTAINTY_START_RISE  0x20


typedef union
{
  int ival;
  struct {
    char a, b, c, d;
  } cval;
} splitint;

// parser related structure
typedef struct 
{
  char *nom;
  char *nom1;
  int edgetype;
  int reltype;
  chain_list *chain;
  double val;
  double val1;
  double val2;
  double val3;
  double val4;
} misc_type;

/* generic list structure */

typedef struct list {
  struct list     *NEXT;
  long             TYPE;
  void            *DATA;
  void            *USER;
  void            *SUPDATA, *COMMON;
} list_list;

typedef struct inf_carac {
  double *VALUES ;
  int     NVALUES ;
  double  LW ; /* LW < 0 if values are absolute */
  double  L; /* add for driving the INF file */
} inf_carac ; 

#define INF_DEFAULT_LOCATION 1
#define INF_LOADED_LOCATION  2
#define INF_IS_DIRTY         0
#define INF_LOADED_FROM_DISK 1

#define INF_FALSEPATH_INFO  14
#define INF_SIGLIST_INFO    18

// delay margin
#define INF_MARGIN_ON_LATCH     0x001
#define INF_MARGIN_ON_BREAK     0x002
#define INF_MARGIN_ON_CONNECTOR 0x004
#define INF_MARGIN_ON_PRECHARGE 0x008
#define INF_MARGIN_ON_CLOCKPATH 0x010
#define INF_MARGIN_ON_DATAPATH  0x020
#define INF_MARGIN_ON_MINDELAY  0x040
#define INF_MARGIN_ON_MAXDELAY  0x080
#define INF_MARGIN_ON_RISEDELAY 0x100
#define INF_MARGIN_ON_FALLDELAY 0x200
#define INF_MARGIN_ON_CMD       0x400
#define INF_MARGIN_ON_FLIPFLOP  0x800

#define INF_MARGIN_ON_ALL (INF_MARGIN_ON_LATCH|INF_MARGIN_ON_BREAK|INF_MARGIN_ON_CONNECTOR|INF_MARGIN_ON_PRECHARGE|INF_MARGIN_ON_FLIPFLOP)


#define INF_MULTICYCLE_SETUP    0x001
#define INF_MULTICYCLE_HOLD     0x002
#define INF_MULTICYCLE_RISE     0x004
#define INF_MULTICYCLE_FALL     0x008
#define INF_MULTICYCLE_START    0x010
#define INF_MULTICYCLE_END      0x020

#define INF_CLOCK_INVERTED      0x001
#define INF_CLOCK_VIRTUAL       0x002
#define INF_CLOCK_IDEAL         0x004

#define INF_NOCHECK_SETUP       0x1
#define INF_NOCHECK_HOLD        0x2


#define INF_CHANGED  1
#define INF_CHANGED_RECOMP  2

#define INF_DIRECTIVES          "STB Directives"
#define INF_DIRECTIVE_UP       1
#define INF_DIRECTIVE_DOWN     2
#define INF_DIRECTIVE_RISING   4
#define INF_DIRECTIVE_FALLING  8
#define INF_DIRECTIVE_CLOCK    16
#define INF_DIRECTIVE_DELAY    32
#define INF_DIRECTIVE_BEFORE   1
#define INF_DIRECTIVE_AFTER    2
#define INF_DIRECTIVE_FILTER   1

#define INF_FALSESLACK_HZ      8
#define INF_FALSESLACK_NOTHZ   4
#define INF_FALSESLACK_UP      1
#define INF_FALSESLACK_DOWN    2
#define INF_FALSESLACK_SETUP   16
#define INF_FALSESLACK_HOLD    32
#define INF_FALSESLACK_LATCH   64
#define INF_FALSESLACK_PRECH   128

#define INF_NODE_FLAG_STABCORRECT       0x1

typedef struct INF_INFOSET
{
  chain_list      *INF_FALSEPATH;
  chain_list      *INF_FALSESLACK;
  list_list       *INF_SIGLIST;
  ht *REGISTRY;
} INF_INFOSET;

typedef struct InfValue
{
  char *infotype;
  char datatype;
  char *pointertype;
  int match;
  union 
  {
    char *string;
    double dvalue;
    int ivalue;  
    void *pointer;
  } VAL;
} InfValue;

typedef struct
{
  char *orig;
  char *dest;
  long lval;
  double dval;
  double dval1;
} inf_assoc;

typedef inf_assoc inf_miscdata;

typedef struct inffig_list
{
  char *NAME;
  INF_INFOSET DEFAULT;
  INF_INFOSET LOADED;
  ptype_list *USER;
  int changed;
} inffig_list;

//extern ht *INF_HT;

extern int INF_ERROR, inf_ignorename;
extern inffig_list *INF_FIG;

inf_assoc *inf_createassoc();

inffig_list *_infRead(char *figname, char *filename, char silent);
inffig_list *infRead(char *filename, char lang);
void infClean(inffig_list *ifl, int location);
extern list_list *infAddList(inffig_list *ifl, int location, char *data,long level,char prg, void *user, char *where);

void    infDrive_filtered (inffig_list *myfig, char *filename, int locations, FILE *outputfile, char *section);
void    infDrive (inffig_list *myfig, char *filename, int locations, FILE *outputfile);
//extern char * infTasVectName(char *name);

inffig_list *getloadedinffig(char *name);
inffig_list *addinffig(char *name);
void delinffig(char *name);
inffig_list *getinffig(char *name);
void infclone(char *name, inffig_list *ifig);

chain_list *infGetInfo(inffig_list *myfig, int type);
int infHasInfo(inffig_list *myfig, int type);

// que viva la revolución
void inf_RemoveKey(inffig_list *ifl, int location, char *key, char *type);

chain_list *inf_GetEntriesByType(inffig_list *ifl, char *type, char *val);
chain_list *inf_GetLocatedEntriesByType(inffig_list *ifl, int location, char *type, char *val);
chain_list *inf_GetValuesByType(inffig_list *ifl, char *type);

int inf_GetInt(inffig_list *ifl, char *key, char *type, int *val);
int inf_GetDouble(inffig_list *ifl, char *key, char *type, double *val);
int inf_GetString(inffig_list *ifl, char *key, char *type, char **val);
int inf_GetPointer(inffig_list *ifl, char *key, char *type, void **val);

void inf_AddInt(inffig_list *ifl, int location, char *key, char *type, int val, char *where);
void inf_AddDouble(inffig_list *ifl, int location, char *key, char *type, double val, char *where);
void inf_AddString(inffig_list *ifl, int location, char *key, char *type, char *val, char *where);
void inf_AddPointer(inffig_list *ifl, int location, char *key, char *type, char *pointertype, void *val, char *where);
void inf_AddMiscData(inffig_list *ifl, int location, char *key, char *type, char *val0, char *val1, long lval, double dval, double dval1, char *where);
void inf_AddAssociation(inffig_list *ifl, int location, char *key, char *type, char *val0, char *val1, long lval, double dval, char *where);
void inf_AddList(inffig_list *ifl, int location, char *key, char *type, chain_list *val, char *where);

void inf_DumpRegistry (FILE *f, inffig_list *ifl, int location);
void inf_CheckRegistry (FILE *f, inffig_list *ifl, int level, chain_list *data);
int inf_StuckSection(int val);
void inf_Dirty(char *name);
int inf_DisplayLoad(int val);

list_list *infaddll(list_list *head, void *data, long level, ptype_list *user);
void inffreell(list_list *ptlist);

list_list *inf_create_INFSIGLIST(inffig_list *ifl);
char *inf_GetDefault_AVT_INF_Value();
char *infTasVectName(char *name);

void inf_PushAndHideInf(char *name);
void inf_PopInf();

int inf_getinffig_state(inffig_list *ifl);
void inf_resetinffig_state(inffig_list *ifl);
chain_list *inf_SortEntries(chain_list *cl);
chain_list *inf_mergeclist (chain_list * l1, chain_list * l2);
void inf_set_print_mode(int val);

int inf_code_marksig(char *string);
int inf_code_marktrans(char *string);
int inf_code_markRS(char *string);

char *inf_reVectName (char *name);
void infDriveStbSpecSection (FILE * f, inffig_list * myfig, char *section, char *header);
void inf_renameinffig(char *orig, char *newname);
void inf_buildmatchrule (inffig_list * ifl, char *section, mbk_match_rules * mr, int canbenonnameallocated);

#endif


