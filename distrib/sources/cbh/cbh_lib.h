/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh100.h                                                    */
/*                                                                          */
/*    (c) copyright 2001 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Anthony LESTER                                          */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef FNC
#define FNC

/* Constant definitions */

#define CBH_UNKNOWN   0
#define CBH_LATCH     1
#define CBH_FLIPFLOP  2

#define CBH_ERROR    -1

#define CBH_NONE      0
#define CBH_INVERT    1
#define CBH_NONINVERT 2
#define CBH_BOTH      3

#define CBH_LIB_MODE  1
#define CBH_TLF_MODE  2

#define CBH_TYPE_COMB       ((long) 0xc1c00000)
#define CBH_TYPE_SEQ        ((long) 0xc1c00001)
#define CBH_TYPE_CCT        ((long) 0xc1c00002)
#define CBH_TYPE_CST        ((long) 0xc1c00003)
#define CBH_TYPE_CST2       ((long) 0xc1c00004)
#define CBH_TYPE_INS        ((long) 0xc1c00005)
#define CBH_TYPE_LOCON      ((long) 0xc1c00006)
#define CBH_TYPE_VLOCON     ((long) 0xc1c19231)

#define CBH_DISPLAY         ((int)  0x8000)
#define CBH_MASKDISPLAY     ((int)  0x7FFF)

#define CBH_SEQ             ((int)  0x0000)
#define CBH_COMB            ((int)  0x0001)
#define CBH_NOTYPE          ((int)  0x0002)
#define CBH_CMP             ((int)  0x0003)
#define CBH_MASKTYPE        ((int)  0xFFFC)

#define CBH_DELBDD          ((int)  0x4000)
#define CBH_DOBDD           ((int)  0x2000)
#define CBH_MASKBDD         ((int)  0x9FFF)

#define CBH_FREECLASS       ((int)  0x1000)
#define CBH_MASKFREECLASS   ((int)  0xEFFF)

#define CBH_EXISTBDD        ((int)  0x0000)

#define CBH_NO_CONSTRAINT   ((int)  0x0000) // pas de contrainte
#define CBH_CONSTRAINT_0    ((int)  0x0001) // contrainte 0
#define CBH_CONSTRAINT_1    ((int)  0x0002) // contrainte 1
#define CBH_CONSTRAINT_Z    ((int)  0x0003) // contrainte hz
#define CBH_CONSTRAINT_X    ((int)  0x0004) // contrainte conflict
#define CBH_CONSTRAINT_U    ((int)  0x0005) // contrainte inconnue
#define CBH_CONSTRAINT_N    ((int)  0x0008) // contrainte not-used
#define CBH_CST_FULL        ((int)  0x0006)
#define CBH_NOT_LEAF        ((int)  0x0007)

#define CBH_TRANS_UU        ((long) 0x00000001)
#define CBH_TRANS_UD        ((long) 0x00000002)
#define CBH_TRANS_DU        ((long) 0x00000004)
#define CBH_TRANS_DD        ((long) 0x00000008)
#define CBH_TRANS_ALL       ((long) 0x0000000F)

#define CBH_TRANS_UC        ((long) 0x00000010)
#define CBH_TRANS_UZ        ((long) 0x00000020)
#define CBH_TRANS_UX        ((long) 0x00000030)
#define CBH_TRANS_DC        ((long) 0x00000040)
#define CBH_TRANS_DZ        ((long) 0x00000100)
#define CBH_TRANS_DX        ((long) 0x00000200)
#define CBH_TRANS_CU        ((long) 0x00000400)
#define CBH_TRANS_CD        ((long) 0x00000800)
#define CBH_TRANS_CC        ((long) 0x00001000)
#define CBH_TRANS_CZ        ((long) 0x00002000)
#define CBH_TRANS_CX        ((long) 0x00004000)
#define CBH_TRANS_ZU        ((long) 0x00008000)
#define CBH_TRANS_ZD        ((long) 0x00010000)
#define CBH_TRANS_ZC        ((long) 0x00020000)
#define CBH_TRANS_ZZ        ((long) 0x00040000)
#define CBH_TRANS_ZX        ((long) 0x00080000)
#define CBH_TRANS_XU        ((long) 0x00100000)
#define CBH_TRANS_XD        ((long) 0x00200000)
#define CBH_TRANS_XC        ((long) 0x00400000)
#define CBH_TRANS_XZ        ((long) 0x00800000)
#define CBH_TRANS_XX        ((long) 0x01000000)

#define CBH_GOOD_TRANS      ((ptype_list*) 0xFFFFFFFF)

/* Macros */
#define CBH_PRINT           if (CBH_TRACE) printf
#define CBH_WARN(A,B)       cbh_error((A),(B))

/* Type declarations */

#include LOG_H
#include MLO_H

typedef struct cbhseq {
    struct cbhseq  *NEXT;
    int             SEQTYPE;
    char           *NAME;
    char           *NEGNAME;
    char           *PIN;
    char           *HZPIN;
    char           *NEGPIN;
    char           *HZNEGPIN;
    char           *STATEPIN;
    char           *LATCHNAME; /* name of latch or master */
    char           *SLAVENAME; /* name of slave */
    int             POLARITY; /* polarity of latch variable wrt latch or master */ 
    int             MSPOLARITY; /* polarity of master wrt slave */
    chain_list     *CLOCK;
    chain_list     *SLAVECLOCK;
    chain_list     *DATA;
    chain_list     *RESET;
    chain_list     *SET;
    chain_list     *RSCONF;
    chain_list     *RSCONFNEG;
} cbhseq;

typedef struct cbhcomb {
    char           *NAME;
    chain_list     *FUNCTION;
    chain_list     *HZFUNC;
    chain_list     *CONFLICT;
    float           FANOUT;
} cbhcomb;

typedef struct cbhclass {
  chain_list       *TYPE[3];
  int               EXISTBDD;
} cbh_classifier;

/* Global variables */

extern  cbh_classifier  *CBH_CLASSIFIER;
extern  int              CBH_TRACE;    
extern  int              CBH_BDD;    
extern  int              CBH_DEBUG;
extern  int              CBH_TEST;
extern  int              CBH_PERMUT;


/* Function declarations */

cbhseq         *cbh_getseqfunc (befig_list *ptcellbefig, int mode);
cbhcomb        *cbh_getcombfunc (befig_list *ptcellbefig, cbhseq *ptcbhseq, char *name);

cbhcomb        *cbh_newcomb(void);
cbhseq         *cbh_newseq(void);
void            cbh_delcomb (cbhcomb *ptcbhcomb);
void            cbh_delseq (cbhseq *ptcbhseq);
void            cbh_clearcct (befig_list *ptcellbefig);

void            cbh_writeabl (FILE *ptfile, chain_list *ptabl, char lib_or_tlf);
char           *cbh_abl2str (chain_list *expr, char *buffer, int *size_pnt, char lib_or_tlf);
char           *cbh_oper2char (short oper);
char           *cbh_vectorize (char *name);
int             cbh_suppressaux (befig_list *ptcellbefig);
chain_list     *cbh_getchain(chain_list *pthead, void *ptdata);
chain_list     *cbh_unionchain(chain_list *ptchain1, chain_list *ptchain2);
int             cbh_countchains(chain_list *headchain);
int             cbh_testnegname(char *name);
void            cbh_calchzfunc(pCircuit pC, biabl_list *ptheadbiabl, cbhcomb *ptcbhcomb);
int             cbh_calcsense(befig_list *ptcellbefig, chain_list *ptexpr, char *varname);
int             cbh_calcsense_abl(chain_list *ptexpr, char *varname);
chain_list     *cbh_calccondition(befig_list *ptcellbefig, char *pinname, char *varname, int polarity);

/* gestion des structure dans les lofigs */
extern  void             cbh_addcombtolocon     ( locon_list 	*locon      ,
                                                  cbhcomb       *ptcbhcomb   );
extern  void             cbh_delcombtolocon     ( locon_list	*locon       );
extern  cbhcomb         *cbh_getcombfromlocon   ( locon_list  	*locon       );

extern  void             cbh_addseqtolofig      ( lofig_list  	*lofig      ,
                                                  cbhseq        *ptcbhseq    );
extern  void             cbh_delseqtolofig      ( lofig_list  	*lofig       );
extern  cbhseq          *cbh_getseqfromlofig    ( lofig_list  	*lofig       );

/* classifier */
extern  int              cbh_cmplofig           ( lofig_list    *reflofig   ,
                                                  lofig_list    *newlofig    );
extern  void             cbh_classlofig         ( lofig_list  	*lofig       );
extern  lofig_list      *cbh_getclasslofig      ( lofig_list    *lofig       );
extern  void             cbh_delclasslofig      ( lofig_list    *lofig       );
extern  int              cbh_cmplofig           ( lofig_list    *reflofig   ,
                                                  lofig_list    *newlofig    );
extern  cbh_classifier  *cbh_newclassifier      ( void                       );
extern  void             cbh_freeclassifier     ( void                       );
extern  void             cbh_displayclassifier  ( void                       );
extern  int              cbh_cmplofig           ( lofig_list    *reflofig   ,
                                                  lofig_list    *newlofig    );
extern  void             cbhenv                 ( void                       );
extern  lofig_list      *cbh_behtocbh           ( befig_list    *befig       );
extern  void             cbh_befiglisttocbhlist ( befig_list    *befig       );

extern  void             cbh_addccttolofig      ( lofig_list    *lofig      ,
                                                  pCircuit       ptcct       );
extern  void             cbh_delccttolofig      ( lofig_list    *lofig       );
extern  pCircuit         cbh_getcctfromlofig    ( lofig_list    *lofig       );

/* fonction du propagateur */
extern  void             cbh_sim                ( lofig_list    *lofig       );
extern  int              cbh_getConstraint      ( losig_list    *losig       );
extern  void             cbh_delConstraint      ( losig_list    *losig       );
extern  int              cbh_setConstraint      ( losig_list    *losig      ,
                                                  int            constraint  );
/* fonction du detecteur de transition niveau loins */
extern  long             cbh_existLoinsTrans    ( loins_list    *loins      ,
                                                  locon_list    *in         ,
                                                  locon_list    *out        ,
                                                  long           type       ,
                                                  ptype_list    *cstList     );
extern  ptype_list      *cbh_confLoinsForTrans  ( loins_list    *loins      ,
                                                  locon_list    *in         ,
                                                  locon_list    *out        ,
                                                  long           type       ,
                                                  ptype_list    *cstList     );
/* fonction du detecteur de transition niveau lofig */
extern  long             cbh_existTrans         ( lofig_list    *lofig      ,
                                                  locon_list    *in         ,
                                                  locon_list    *out        ,
                                                  long           type        );
extern  ptype_list      *cbh_confForTrans       ( lofig_list    *lofig      ,
                                                  locon_list    *in         ,
                                                  locon_list    *out        ,
                                                  long           type        );
extern  int              cbh_getLoconCst        ( locon_list    *locon       );
extern  void             cbh_setLoconCst        ( locon_list    *locon       ,
                                                  int            cst         );
extern  void             cbh_delLoconCst        ( locon_list    *locon       );
extern  void             cbh_delCstOnLofig      ( lofig_list    *lofig      ,
                                                  locon_list    *in         ,
                                                  locon_list    *out         );
extern  ptype_list      *cbh_getCstToEnableLoins( loins_list    *loins      ,
                                                  locon_list    *insOut     ,
                                                  ptype_list    *cstList     );
extern  ptype_list      *cbh_getCstToDisableLoins(loins_list    *loins      ,
                                                  locon_list    *insOut     ,
                                                  ptype_list    *cstList     );

extern void             cbh_createcct            ( void );
extern void             cbh_delcct               ( void );
void cbh_vhdlload(char *filename);
void cbh_verilogload(char *filename);
void cbh_spfload(char *filename);
void cbh_spefload(char *filename);
void cbh_infload(char *filename);
void cbh_spiceload(char *filename);
#endif
