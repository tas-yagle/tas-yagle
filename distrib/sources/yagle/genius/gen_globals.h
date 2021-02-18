#ifndef GENIUS_GLOBALS
#define GENIUS_GLOBALS

#define CASHS 4
#define MAX_SYMLIST 1024

typedef struct biinfo
{
  struct SymInfoItem *syms, *coupl;
  int index;
} biinfo;

typedef struct
{
  // gen_corresp.c
  int CORRESP_DEBUG;
  int NO_UPDATE;
  lofig_list *GEN_HEAD_LOFIG;
  ht *GEN_HT_LOFIG;
//  chain_list *TRANS_TO_SUPPRESS;

  HeapAlloc corresp_heap;
  HeapAlloc subinst_heap;

  int nbalim, nbins;
  char *GENIUS_TO_SPY, *GENIUS_SPY;

  // gen_env.c
  char       *GENIUS_LIB_NAME;              /* file with all models names */
  char       *GENIUS_LIB_PATH;              /* directory */
  struct tree  *GENIUS_TREE;      /* syntaxical result of model files */
  struct lib_entry  *GENIUS_PRIORITY;      /* syntaxical result of library file*/
  FILE       *GENIUS_OUTPUT;          /* file for verbose disassembly */
  int GEN_DEBUG_LEVEL;
  int GEN_OPTIONS_PACK;
  ExecutionContext *genius_ec;

  // gen_execute_VHDL.c
  int VAR_UNKNOWN;     /*flag if variable isn't found in environment*/
  int VAR_UNDEF;       /*flag if var. with no value*/

  // gen_model_utils.c
  char GEN_SEPAR;
  chain_list *blocks_to_free;
  model_list *HEAD_MODEL;                /*list of all models*/
  /*to accelerate memory allocation*/
  mark_list *HEAD_MARK;                   /*list of allocated marks*/

  lofig_list *GENIUS_HEAD_LOFIG;
  ht *GENIUS_HT_LOFIG;
  chain_list *ModelTrace; // list of models marked as visited by the user

  //gen_new_king.c
  HeapAlloc ea_heap;

  //gen_optimization.c
  HeapAlloc lcu_heap;
  HeapAlloc stat_heap;

  //gen_search_global.c
  lofig_list *GENIUS_GLOBAL_LOFIG;

  // gen_search_utils.c
  char tabs[200];
  lofig_list *current_lofig;
  ptype_list *ALL_ENV, *GEN_USER;
  ht *NEW_LOINS_NAMES_HT;
  ht *LOINS_LIST_CONTROL;
  chain_list *ALL_KIND_OF_MODELS;
  struct all_loins_heap_struct *ALL_LOINS_FOUND;
  lofig_list *CUR_HIER_LOFIG;
  corresp_t *CUR_CORRESP_TABLE;
  HeapAlloc all_loins_heap;
  HeapAlloc loconmark_heap;

  //gen_symmetric.c
  HeapAlloc swap_heap;
  ht *radindexht;
  char **radtable;
  int curradindex;
  ht *scht;
  chain_list *myallocs;

  biinfo *ALL_SYM_INFO[MAX_SYMLIST];
  int CUR_SYM_INDEX;

  struct localcash
  {
    int count;
    char *model, *con;
    biinfo *info;
  } entry[CASHS];

  struct foundmodel *FOUND_MODEL;
  ht *TEMPLATE_HT;
} GeniusContext;


extern lofig_list *GEN_HEAD_LOFIG;
extern ht *GEN_HT_LOFIG;
//extern chain_list *TRANS_TO_SUPPRESS;
extern struct foundmodel *FOUND_MODEL;
extern HeapAlloc corresp_heap;
extern HeapAlloc subinst_heap;

extern int nbalim, nbins;
//extern char *GENIUS_TO_SPY, *GENIUS_SPY;

// gen_env.c
extern  char       *GENIUS_LIB_NAME;              /* file with all models names */
extern  char       *GENIUS_LIB_PATH;              /* directory */
extern  struct tree  *GENIUS_TREE;      /* syntaxical result of model files */
extern  struct lib_entry  *GENIUS_PRIORITY;      /* syntaxical result of library file*/
extern  FILE       *GENIUS_OUTPUT;          /* file for verbose disassembly */
extern  int GEN_DEBUG_LEVEL;
extern  int GEN_OPTIONS_PACK;
extern  ExecutionContext *genius_ec;

// gen_execute_VHDL.c
extern int VAR_UNKNOWN;     /*flag if variable isn't found in environment*/
extern int VAR_UNDEF;       /*flag if var. with no value*/

// gen_model_utils.c
extern   char GEN_SEPAR;

extern  chain_list *blocks_to_free;

extern model_list *HEAD_MODEL;                /*list of all models*/
/*to accelerate memory allocation*/
extern mark_list *HEAD_MARK;                   /*list of allocated marks*/

extern  lofig_list *GENIUS_HEAD_LOFIG;
extern  ht *GENIUS_HT_LOFIG;

extern chain_list *ModelTrace; // list of models marked as visited by the user

//gen_new_king.c
extern HeapAlloc ea_heap;

//gen_optimization.c
extern HeapAlloc lcu_heap;
extern HeapAlloc stat_heap;
//gen_search_global.c
extern  lofig_list *GENIUS_GLOBAL_LOFIG;

// gen_search_utils.c
extern  char tabs[200];
extern  lofig_list *current_lofig;
extern  ptype_list *ALL_ENV, *GEN_USER;
extern  ht *NEW_LOINS_NAMES_HT;
extern  ht *LOINS_LIST_CONTROL;
extern  chain_list *ALL_KIND_OF_MODELS;
extern  struct all_loins_heap_struct *ALL_LOINS_FOUND;
extern  lofig_list *CUR_HIER_LOFIG;
extern  corresp_t *CUR_CORRESP_TABLE;

extern  HeapAlloc all_loins_heap;
extern  HeapAlloc loconmark_heap;

//gen_symmetric.c
extern  HeapAlloc swap_heap;
extern  ht *radindexht;
extern char **radtable;
extern  int curradindex;
extern  ht *scht;
extern  chain_list *myallocs;

extern  biinfo *ALL_SYM_INFO[MAX_SYMLIST];
extern  int CUR_SYM_INDEX;

extern  struct localcash entry[CASHS];

lofig_list *genius_external_getlofig(char *name);
mbkContext *genius_external_getcontext();
void genius_external_setcontext(mbkContext *ctx);
extern ht *GNS_TEMPLATE_HT;
#endif
