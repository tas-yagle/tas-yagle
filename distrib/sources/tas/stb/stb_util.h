/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_util.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/
/* constantes */

#define STB_TABLE   0
#define STB_THZ     1
#define STB_SPECIN  2
#define STB_SPECOUT 3

extern char *STB_TOOLNAME ;
extern char  STB_REPORT ;
extern char  STB_OUT ;
extern char  STB_SILENT ;
extern char  STB_MULTIPLE_COMMAND ;
extern char  STB_OPEN_LATCH_PHASE ;
extern char  STB_ANALYSIS_VAR;
extern char  STB_GRAPH_VAR;
extern char  STB_MODE_VAR;
extern char  STB_CTK_VAR;

extern char  STB_FILE_FORMAT;
extern char  *STB_FOREIGN_TIME_UNIT;
extern char  *STB_FOREIGN_CONSTRAINT_FILE;
extern double STB_LIMITS;
extern double STB_LIMITL;

/* definitions des types */

typedef struct phase {
    ttvevent_list  *EVENT;
    long            TIME;
} phase;

/* declaration des fonctions */

void            stb_env __P((void));
void            stb_exit __P((int code));

void            stb_addstbnode __P((ttvevent_list *ptnode));
void            stb_delstbnode __P((ttvevent_list *ptnode, char nbindex));

stbpair_list  **stb_alloctab __P((int size));

stbpair_list   *stb_addstbpair __P((stbpair_list *head, long d, long u));
stbpair_list   *stb_delstbpair __P((stbpair_list *head, stbpair_list *del));
void            stb_freestbpair __P((stbpair_list *head));
void            stb_freestbtabpair __P((stbpair_list **tab,char size));

void            stb_addstbck __P((stbnode *ptstbnode, long supmin, long supmax, long sdnmin, long sdnmax, long period, long setup, long hold, char index, char active, char verif, char type,ttvevent_list *origclock));
void            stb_chainstbck __P((stbnode *ptstbnode, long supmin, long supmax, long sdnmin, long sdnmax, long period, long setup, long hold, char index, char active, char verif, char type, ttvevent_list *cmd, ttvevent_list *original_clock));
void            stb_delstbck __P((stbnode *ptstbnode));
void            stb_getstbck __P((stbnode *ptstbnode,ttvevent_list *cmd));
stbck *stb_findstbck(stbnode *ptstbnode,ttvevent_list *cmd);

stbfig_list    *stb_addstbfig __P((ttvfig_list *ptttvfig));
int             stb_delstbfig __P((stbfig_list *ptstbfig));
stbfig_list    *stb_getstbfig __P((ttvfig_list *ptttvfig));

stbnode        *stb_getstbnode __P((ttvevent_list *ptnode));

stbpair_list   *stb_dupstbpairlist __P((stbpair_list *ptheadlist));
stbpair_list   *stb_revstbpairlist __P((stbpair_list *head));
chain_list    *stb_getsigfromlist __P((ttvfig_list *ptfig, chain_list *ptlist, char *name));
chain_list     *stb_suppresseqvtck __P((chain_list *clocklist));
chain_list     *stb_geteqvtck __P((ttvsig_list *ptttvsig, chain_list *ptlist));

char            stb_sortphase __P((stbfig_list *ptstbfig, chain_list *clocklist));
ttvevent_list  *stb_getphase __P((stbfig_list *ptstbfig, char *name, char edge));
ttvevent_list  *stb_getclockevent __P((stbfig_list *ptstbfig, char *name, char edge));
char            stb_getphaseindex __P((stbfig_list *ptstbfig, char *name, char edge));
stbck          *stb_getclocknode __P((stbfig_list *ptstbfig, char phase, char *namebuf, ttvevent_list **ptevent, stbck *ck));
stbck          *stb_getclock __P((stbfig_list *ptstbfig, char phase, char *namebuf, char *ptedge, stbck *ck));
long            stb_getperiod __P((stbfig_list *ptstbfig, char phase));

stbpair_list   *stb_buildintervals __P((chain_list *ptstablelist, chain_list *ptunstablelist, long clockperiod, int *errcode));
int             stb_addintervals __P((stbfig_list *ptstbfig, ttvsig_list *ptttvsig, chain_list *ptstablelist, chain_list *ptunstablelist, int table, char slope, int phaseindex, char cktype, char nodemode));

int             stb_countchain __P((chain_list *ptchain));

void            stb_viewstbnode __P((stbfig_list *ptstbfig, ttvevent_list *ptevent));
void            stb_viewstbtab __P((stbpair_list **ptstbtab, int size));
void            stb_viewstbpairlist __P((stbpair_list *ptstbpairlist));
stbdomain_list *stb_getstbdomain __P((stbdomain_list *domain, char index));
stbdomain_list *stb_addstbdomain __P((stbdomain_list *head, char min, char max)) ;
void            stb_adddisable __P((stbfig_list *stbfig, char from, char to));
int             stb_isdisable __P((stbfig_list *stbfig, char from, char to));
void            stb_ckdisable __P((stbfig_list *stbfig, char *ckfrom, char *ckto));
extern void     stb_init_var __P((void));
long stb_calperiode(stbfig_list *stbfig, char u, stbnode *node, ptype_list *phaselatch, char phaseinit, long periode);
void stb_getmulticycleperiod(ttvevent_list *inpute, ttvevent_list *outpute, long inputperiod, long outputperiod, long *setupP, long *holdP, int *nb_setup_cycle, int *nb_hold_cycle);
int stb_sigisonclockpath(ttvevent_list *pevent, ttvevent_list *from);
void stb_mark_data_on_clock_path(ttvpath_list *pth);
void stb_free_data_on_clock_path(ttvpath_list *pth);

void stb_assign_phase_to_stbpair(stbpair_list  *ptheadlist, unsigned char phase);
stbpair_list *stb_addstbpair_and_phase(stbpair_list *head, long d, long u, unsigned char pD, unsigned char pU);
int stb_checkvalidcommand(ttvevent_list *tve, stbck *inputck);
void create_clock_stability(stbnode *clocknode, stbpair_list **RES_STBTAB);
        
typedef struct checkstruct
{
  char type;
  int idx;
} checkstruct;

extern checkstruct sto_cfg[];
extern int sto_cfg_size;

int stb_getfalsepathkey(inffig_list *ifl);
int stb_getmulticyclekey(inffig_list * myfig);
int stb_getdirectivekey(inffig_list * myfig);
int stb_getdelaymarginkey (inffig_list * myfig);
int stb_getlistsectionkey(inffig_list * ifl, char *section);
int stb_getnocheckkey(inffig_list * ifl);
int stb_getfalseslackkey (inffig_list *ifl);
int stb_getswitchingprobakey(inffig_list * ifl);
int stb_getclockuncertaintykey(inffig_list * myfig);

ht *stb_buildquickaccessht(ttvfig_list *tvf, NameAllocator *NA);
void stb_setprobability (stbfig_list *sbf, inffig_list *ifl);

stbpair_list *stb_clippair(stbpair_list *ptstbpair, long min, long max);
