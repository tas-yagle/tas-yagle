/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_gen.h                                                   */
/*                                                                          */
/*    (c); copyright 2002 AVERTEC                                           */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s); :   Antony PINTO                                           */
/*                                                                          */
/****************************************************************************/

#ifndef BEG_GEN_H
#define BEG_GEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

#include AVT_H
#include MUT_H
#include LOG_H
#include EQT_H
#include BEH_H
#include BHL_H
#include BEG_H
#include BEF_H

#define BEG_OBJ                ptype_list*
#define BEG_OBJ_NEW(pt,t)      BEG_WORKLIST = addptype(BEG_WORKLIST,t,pt)
#define BEG_OBJ_TYPE(pt)       pt->TYPE
#define BEG_OBJ_DATA(pt)       pt->DATA

#define BEG_FREEALL            2
#define BEG_RESTORE            4

#define BEG_HTSIZE             50

// error and warning
#define BEG_ERR_SING_N_VECT   "001"
#define BEG_ERR_BETYPE        "002"
#define BEG_ERR_OUT_AS_IN     "003"
#define BEG_ERR_NULCND        "004"
#define BEG_ERR_CONFLICT_VECT "005"
#define BEG_ERR_SIZE          "006"
#define BEG_ERR_PARSER        "007"
#define BEG_ERR_POR           "008"
#define BEG_ERR_BIBUS         "009"
#define BEG_ERR_WAY           "010"
#define BEG_ERR_SELECT        "011"
#define BEG_ERR_EXISTSIG      "012"
#define BEG_ERR_UNKNOWN       "016"
#define BEG_ERR_IN_TO_OUT     "018"

#define BEG_WAR_UNDEF_SIG     "013"
#define BEG_WAR_INOUTSIG      "014"
#define BEG_WAR_WAYENV        "015"
#define BEG_WAR_UNKNOWN       "016"
    
#define BEG_TRA_X             "017"

// structure for loop detection
typedef struct beg_ld
{
  struct beg_ld     *NEXT       ;
  biabl_list        *BASE       ;
  chain_list        *REFCS      ;
  chain_list        *REFVS      ;
  chain_list        *BASCS      ;
  chain_list        *BASVS      ;
  loop_list         *LOOP       ;
  int                CPTINIT    ;
  int                CPT        ;
  int                VECTORIZE  ;
  int                STEP       ;
  int                TIME       ;
  vectbiabl_list    *VBIABL     ;
  vectbiabl_list    *LAST       ;
  char              *LVAR       ;
} beg_ld_list;

// structure for beg
typedef struct beg_obj
{
  struct beg_obj    *NEXT       ;
  befig_list        *FIG        ;
  BEG_OBJ            WORKLIST   ;
  ht                *POR        ;
  ht                *SIG        ;
  ht                *GUE        ;
  ht                *CON        ;
  ptype_list        *CONLIST    ;
  ht                *RIN        ;
  ht                *DEL        ;
  ht                *RINADJUS   ;
  int                RINTREAT   ;
}
beg_obj_list;


//extern    befig_list      *BEG_HEAD_BEFIG;
//extern    befig_list      *BEG_WORK_BEFIG;
//
//extern    BEG_OBJ          BEG_WORKLIST;
//
//extern    ht              *BEG_POR_TABLE;
//extern    ht              *BEG_SIG_TABLE;
//
//extern    chain_list      *BEG_POT_LIST; // potentially-in-port list

// parser function
extern chain_list      *beg_genStr2Abl      (char             *str);
extern chain_list      *beg_genStable       (chain_list       *atm, int attrib);
extern chain_list      *beg_genEquCnd       (chain_list *abl, chain_list *bitstr);


       void             beg_initError       (void);
       void             beg_trace           (char *text, ... );
       BEG_OBJ          beg_getObjByName    (befig_list *fig, char *name, BEG_OBJ *por);
       void             beg_initError       (void);
extern void             beg_error           (char * error, char *text, ... );
extern void             beg_warning         (char * warning, char *text, ...);

extern void             beg_def_befig       (char *name);

extern BEG_OBJ          beg_getObjByName    (befig_list *fig, char *name, BEG_OBJ *por);
extern int              beg_getLeft         (BEG_OBJ obj);
extern int              beg_getRight        (BEG_OBJ obj);
extern void             beg_newRight        (BEG_OBJ obj, int right);
extern void             beg_newLeft         (BEG_OBJ obj, int left);
extern int              beg_isVect          (BEG_OBJ obj);
extern vectbiabl_list  *beg_getVectBiabl    (BEG_OBJ obj);
extern befig_list      *beg_get_befigByName (char *name);
extern void             beg_def_befig       (char *name);
extern void             beg_def_sig         (char *name, char *expr, unsigned int time,int,int,long flags);
extern void             beg_def_por         (char *name, char direction);
extern chain_list      *beg_str2Abl         (char *str);
extern befig_list      *beg_get_befig       (int verif);
extern void             beg_freeBefig       (void);
extern void             beg_eat_figure      (befig_list *eaten);
extern biabl_list*      beg_def_mux         (char *name, char *cndexpr, char *bitstr, char *valexpr, int time, long flags, long biflags);
extern biabl_list*      beg_def_process     (char *name, char *cndexpr, char *valexpr, int time, int timer, int timef, long flag, long flags, long biflags);
extern biabl_list*      beg_def_loop        (char *name, char *cndexpr, char *valexpr, char *var, int   time, long  flag, long flags, long biflags);
extern biabl_list*      beg_def_biloop      (char *name, char *cndexpr, char *valexpr, char *var1, char *var2, int   time, long  flag, long flags, long biflags);

extern char             *beg_getName        (BEG_OBJ obj);
extern char             *beg_genBitStr      (char *text);
extern chain_list       *beg_compactAbl     (chain_list *expr);
extern void              beg_forceZSort     (void);
       void              beg_updateConsDir  (befig_list *befig);

#endif
