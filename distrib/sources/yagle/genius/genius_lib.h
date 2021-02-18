/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS  v1.00                                               */
/*    Fichier : genius100.h                                                 */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#ifndef GENIUS_HEADER
#define GENIUS_HEADER

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include MUT_H
#include MLO_H
#include API_H

/****************************************************************************/
/* Parse from a LIBRARY files user's models and extract them from netlist   */
/****************************************************************************/
extern chain_list* genius_main(lofig_list *netlist, char *celldir, char *libname, char *outname);
extern void geniusExecuteAction(loins_list *loins);


#define UNDEF 0x0FFFFFFF       /*improbable value: variable not defined*/

#define TOKEN(pt) ((pt)->TOKEN)	                    /*to recover the token*/

#define TO_DELETE_MARK_PTYPE 0x1f2e3a


#define INDEX_START 1        /*beginning of indexation for signals in a model*/

/*USER ptype for loins or lotrs*/
#define GEN_VISITED_PTYPE  0xf23460
/*USER ptype for loinsin generic model*/
#define GENERIC_PTYPE  0xf23456
/*USER ptype for locon for model*/
#define GENCONCHAIN    0xFe3671
/*USER ptype for losig for model*/
#define GENSIGCHAIN    0xdf2211
/*USER ptype for losig for searching*/
#define GEN_MARK_PTYPE 0xdf2551
/*USER ptype for loins for searching in phase 1 */
#define GEN_FOUNDINS_PTYPE 0x100532
/*USER ptype for lofig to keep symmetric port information */
#define GEN_SYM_INFO_PTYPE 0x100533
/*USER ptype for loins to keep generic var assignment information toto => titi*2 */
#define GEN_GENERIC_MAP_PTYPE 0x100534
/*USER ptype for loins to keep coupled port information */
#define GEN_COUPLED_INFO_PTYPE 0x100535
// for a connector, indicate that it can't be swapped (or already visited)
#define GEN_FIXED_PTYPE 0x100536 
// for a connector, gives the marked signal in the model
#define GEN_LOCON_MARK_PTYPE 0x100537
// for an instance, gives index in a loop
#define GEN_LOOP_INDEX_PTYPE 0x100538
// for a signal in model, gives the range of the signal, x to y, x downto y or NULL otherwise
#define GEN_REAL_RANGE_PTYPE 0x100539
// for a signal, point to a hash table containing information on the lofigchain (ls->previous)
#define FAST_LOFIGCHAIN_PTYPE 0x100540
// for a transistor mark it has ghost
#define GENIUS_GHOST_PTYPE 0x100542
// for a connector, indicate the lofigchain it is in
#define GENIUS_CON_LOFIG_PTYPE 0x100543
// for a transistor or a signal, modelname
#define GENIUS_MODNAME_PTYPE 0x100544
// for a transistor or a signal, modelname
#define GENIUS_VISITED_LOCON_PTYPE 0x100545
// for a loins, indicate the flags EXCLUDE or WITHOUT
#define GENIUS_LOINS_FLAGS_PTYPE 0x100546
// here are the flags
#define             LOINS_IS_EXCLUDE  0x1
#define             LOINS_IS_WITHOUT  0x2
#define             LOINS_IS_EXCLUDE_AT_END  0x40
  // + those one in the corresp_t
#define             LOINS_IS_BLACKBOX  0x4
#define             LOINS_IS_UPDATED   0x8
#define             LOINS_ALIM_UPDATED 0x10
#define             LOINS_IS_TOPLEVEL  0x20
// for a loins, keeps a count of the number of time it has been hit by a search
#define GENIUS_LOINS_COUNT_PTYPE 0x100547
// for a losig, keeps a hash table with keys for all loins and lotrs attached thru lofigchain
#define GENIUS_LOFIGCHAIN_KEYS_PTYPE 0x100548
#define GEN_LOINS_UNDER_CONST 0x100549
// for a losig, points to a mark_list array for faster accessindicate the flags EXCLUDE or WITHOUT
#define GENIUS_EXP_ARRAY_PTYPE 0x100550
// for a losig, indicate if a lofigchain must be pushed to enable update
#define LAST_TOBE_PENDING_LOFIGCHAIN 0x100551
#define GEN_USER_PTYPE 0x100552
#define GENIUS_SAVED_PNODE 0x10053
#define GEN_ORIG_LOINS_PTYPE 0x10054

// for action netlist
#define GEN_ORIG_CORRESP_PTYPE 0x10055
#define GEN_ORIG_INS_CORRESP_PTYPE 0x10056
#define GEN_TRANS_TO_SUPPRESS_PTYPE 0x10057
#define GEN_TRANS_TO_KEEP_PTYPE 0x10058

#define GEN_PARENT_LOFIG 0x10059

// in case of multiple vdd linked to one
#define GEN_MARK_LOINS_GROUP 0x10060
#define GEN_AUTO_UPDATE_ALIMS 0x10061

//some losig flags
#define GRAB_HAVE_BEEN_DONE 1
#define LOFIGCHAIN_HAS_SYMS 2
#define SIGNAL_HAVE_MORE_INSTANCES_IN_LOFIGCHAIN 4

#define HIER_LOFIG_INFO_PTYPE 0x100553

#define GEN_TRANS_PARALLEL 0x100554
#define GEN_UNUSED_FLAG_PTYPE 0x100555
#define GEN_POST_BBOX_PTYPE 0x100556
#define GEN_STOP_POWER_FLAG_PTYPE 0x100559

#define GEN_HIERLOFIG_LOSIG_HT 0x100557
#define GEN_BBOX_LOINS_MODEL 0x100558

// loins list control flags
#define KEEP_INSTANCE 1

// general ptype
#define GNS_FORCE_MATCH 0xfab90407

/*elem wich refers a loins found in global netlist. This loins isn't 
incorporated in it, so we have to keep its reference outside. it is the aim of
this srtucture*/

typedef struct undoswap
{
  locon_list *lc, *sym;
} undoswap;

typedef struct foundins 
{
  struct foundins *NEXT, *PREVIOUS;
  struct foundins *LOWER;
  struct foundins *HIGHER;
  loins_list      *LOINS;           /*loins found on circuit*/
  ptype_list      *VAR;             /* Generic variables with their values */
  struct foundins *AFTER;           /* if FLAG=HEAD or SELECT :place in list*/
  struct foundins *BEFORE;          /* if FLAG=HEAD or SELECT :place in list*/
  int              SIZE;            /*if FLAG = HEAD  number of foundins */
  /*behind + himself*/
  loins_list      *VISITED;    /*if already visited have the model it matches*/
  char ghost; // tells : don't care this instance, it's from another instance 
  char             NOT_MATCH;        /*non-zero if already tried and failed*/
  char             FLAG;            /*mark passage of search algorithm*/
  char             SAVEDFLAG;            /*mark passage of search algorithm*/
  char             LOCKED;
} foundins_list;



typedef struct model 
{
  char         *NAME;     /*name of lofig*/
  struct model *NEXT;
  char *main_archi_name;
  lofig_list   *LOFIG;    
  chain_list   *VAR;      /*list of generic variables contained in GENERIC*/
  tree_list   *GENERIC;  /*condition tree on instances in LOFIG*/
  char *C;
//tree_list   *C;        /*action to do if model is recognized*/
  char *REGEXP;
} model_list;

/*in locon USER, a ptype GENCONCHAIN pointed to this*/
typedef struct genconchain {
   struct genconchain *NEXT;
   tree_list *LOCON_EXP;        /*source bit number */
   losig_list *LOSIG;            /*destination*/
   tree_list *LOSIG_EXP;        /*destination bit number*/
} genconchain_list;

/*in locon USER, a ptype GENCONCHAIN pointed to this*/
typedef struct gensigchain {
   struct gensigchain *NEXT;
   tree_list *LOSIG_EXP;        /*source bit number */
   locon_list *LOCON;            /*destination*/
   tree_list *LOCON_EXP;        /*destination bit number*/
  int ghost;                    // don't care this gensigchain
} gensigchain_list;

/*in MARK_LIST in USER field of losig*/
typedef struct mark 
{
  struct mark      *NEXT;
  losig_list       *LOSIG;              /*losig in circuit*/
  ptype_list       *ROOT;               /*pointer on ptype of USER*/
  int               BIT;                /*bit number in model*/
  int               VISITED;            /*non zero if already visited*/
  int COUNT;  /* number of connectors who have marked this signal */
  locon_list *one_locon; /* locon who have marked this signal first */
  void **mark_in_ea; // pointer on position in expandable array
} mark_list;

struct corresp_t;

typedef struct subinst_t
{
  struct subinst_t *NEXT;
  char *INSNAME;
  struct corresp_t *CRT;
  long FLAGS;
} subinst_t;

typedef struct corresp_t {
  char       *GENIUS_INSNAME;
  char       *GENIUS_FIGNAME;
//  char       *INSNAME;
  char       *FIGNAME;
  ht         *SIGNALS;
  ht         *TRANSISTORS;
  subinst_t  *SUBINSTS; // type of DATA is corresp_t*
  ptype_list *VAR;
  short FLAGS;
  char ARCHISTART, ARCHILENGTH;
} corresp_t;


/* General Utility Functions */
extern char        GEN_SEPAR;
extern lofig_list *GENIUS_GLOBAL_LOFIG;
extern chain_list *ALL_LOINS;
extern chain_list *ALL_LOTRS;
extern chain_list *ALL_SWAPS;
extern ptype_list *ALL_ENV;
extern lofig_list *current_lofig;
extern chain_list *ALL_HIERARCHY_LOTRS;
extern lofig_list *GEN_HEAD_LOFIG;
extern ht         *GEN_HT_LOFIG;
//extern chain_list *TRANS_TO_SUPPRESS;
extern ptype_list *GEN_USER;
extern char        GEN_SEPAR;
extern chain_list *expansedloins, *originalloins;
extern lofig_list *CUR_HIER_LOFIG;
extern corresp_t  *CUR_CORRESP_TABLE;
extern ht         *NEW_LOINS_NAMES_HT;
extern ht *LOINS_LIST_CONTROL;
extern ht *GNS_TEMPLATE_HT;

extern char       *gen_info                         ();
extern int         gns_FinishedRule                 ();
extern int         gns_ModelVisitedDuringInit       (char *name);
extern void        gns_MarkModelVisitedDuringInit   (char *name);
extern losig_list *getlosigfrommark                 (gensigchain_list* gen, int numbit);
extern int         gen_is_token_to                  (int token);
extern int         gen_is_token_downto              (int token);
extern int         Eval_Exp_VHDL                    (tree_list *tree, ptype_list *env);
extern char       *gen_canonize_trname              (char* trname);
extern void        ComputeWeightsForLoinsConnectors (lofig_list *lf);
extern chain_list *gen_hierarchical_split           (char *name);
char *gen_makesignalname (char *s);
char *gen_makeinstancename (char *s);
//extern char       *gen_makename                     (char *s);
extern lotrs_list *gen_corresp_trs                  (char *name, corresp_t *corresp);
extern losig_list *gen_corresp_sig                  (char *name, corresp_t *corresp);
extern void        gen_update_sigcon                (lofig_list *fig);
extern char       *gen_losigname                    (losig_list *sig);
extern corresp_t  *gen_build_corresp_table          (lofig_list *lf, chain_list *loins, chain_list *lotrs);
extern void        gen_update_corresp_table         (lofig_list *lf, corresp_t *crp_table, loins_list *li);
extern lofig_list *gen_build_netlist                ();
void gen_drive_corresp_htable (FILE *f, char *name, char *path, int top, corresp_t *dico);
extern void        gen_drive_corresp_table             (FILE *f, char *path, int top, subinst_t *dico);
extern void        gen_clean_RC                     (lofig_list *fig);
chain_list *gen_goto(chain_list *arbo, corresp_t **crt, int tag);
subinst_t *gen_GetCorrespondingInstance(char *name, corresp_t *level);
lotrs_list *gen_GetCorrespondingTransistor(char *name, corresp_t * level);
chain_list *gen_GetInstanceWithSameRadical(lofig_list *lf, char *radical);

// trouve une instance dans la lofig hierarchique
loins_list *gen_findinstance(lofig_list *lf, char *name);
void gen_update_alims (lofig_list *lf, corresp_t *root_tbl, chain_list *distrib_sigs);
chain_list *GetModelSignalFromCON(locon_list *ptcon, int *first, int *last, model_list *model, ptype_list *env, ptype_list *livar);

int gen_wasunused();

chain_list *GrabAllConnectorsThruCorresp(char *con_name, losig_list *realconls, corresp_t *CRT, chain_list *Last);
subinst_t *Add_subinst(subinst_t *head, char *name, corresp_t *crt, long flags);

typedef struct
{
  char *FIGNAME;
  lofig_list *GLOBAL_LOFIG;
  ht *HIER_HT_LOFIG;
  lofig_list *HIER_HEAD_LOFIG;
  subinst_t *TOP_LEVEL_SUBINST;
  HeapAlloc corresp_heap;
  HeapAlloc subinst_heap;
  ht *ALL_INSTANCES;
  ht *TEMPLATE_HT;
  chain_list *RETURN;
  mbkContext external_ctx;
  /*
    ht *MODEL_HT_LOFIG;
    lofig_list *MODEL_HEAD_LOFIG;
  */
} ALL_FOR_GNS;

extern ALL_FOR_GNS *LATEST_GNS_RUN;
void gen_fill_subinsts(ALL_FOR_GNS *all);
subinst_t *gen_get_hier_instance(ALL_FOR_GNS *all, char *hiername);
int gen_getvariable(char *name);
char *gen_getarchi();
char *gen_getmodel();
char *gen_getinstancename();
ht *gen_get_losig_ht(lofig_list *lf);
void gen_grab_all_real_corresp(lofig_list *lf, chain_list **lotrs, ptype_list **losig, chain_list **loins);

extern lofig_list *(*external_getlofig)(char *name);
void in_genius_context_of(ALL_FOR_GNS *all, subinst_t *sins);
void out_genius_context_of();
int is_genius_instance(lofig_list *lf, char *name);
mbkContext *genius_external_getcontext();

// 126-33
/*
#define D_BASE 10
#define D_BASE_START '0'
*/
#define D_BASE 220
#define D_BASE_START 33

//void DriveCorresp(FILE *f, ALL_FOR_GNS *all);
void gnsDriveCorrespondanceTables(ALL_FOR_GNS *all, int mode);
ALL_FOR_GNS *gnsParseCorrespondanceTables(char *name);
void gnsApplyConnectorOrientation(ALL_FOR_GNS *all);

typedef struct
{
  char *arc_name;
  APICallFunc *MODEL;
  APICallFunc *SIM;
  APICallFunc *ENV;
  APICallFunc *CTK_ENV;
} ArcInfo;

typedef struct
{
  APICallFunc *BUILD_TTV;
  APICallFunc *BUILD_BEH;
  ht *ARCS;
} HierLofigInfo;

HierLofigInfo *gethierlofiginfo(lofig_list *lf);
ArcInfo *getarcinfo(HierLofigInfo *hli, char *name);

lotrs_list *_gen_GetCorrespondingTransistor(char *name, corresp_t * level, int mode);
void UpdateTransistorsForYagle(ALL_FOR_GNS *all, int fromcns);

int check_instances_connections(lofig_list *lf, char *info);

extern void        gen_update_transistors         (lofig_list *fig, corresp_t *table);
extern void        gen_update_loins               (lofig_list *fig, corresp_t *table);
lofig_list *__gns_GetNetlist ();
void transfert_needed_lofigs(lofig_list *lf, mbkContext *ctx);

#endif
