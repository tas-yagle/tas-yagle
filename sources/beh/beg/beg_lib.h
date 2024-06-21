/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEG_GEN Version 1.00                                        */
/*    Fichier : beg_lib.h                                                   */
/*                                                                          */
/*    (c)  copyright 2002 AVERTEC                                           */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s)  :   Antony PINTO                                           */
/*                                                                          */
/****************************************************************************/

#ifndef BEG_LIB_H
#define BEG_LIB_H

#include MUT_H
#include BEH_H
#include MLO_H

#define BEG_PRE                (long) 0x00000002
#define BEG_REG                (long) 0x00000004
#define BEG_BUS                (long) 0x00000008
#define BEG_CMPLX              (long) 0x00000010
#define BEG_LAST               (long) 0x00000020

#define BEG_MASK_OUT           (long) 0x00000010
#define BEG_MASK_AUX           (long) 0x00000020
#define BEG_MASK_BUS           (long) 0x00000030
#define BEG_MASK_BUX           (long) 0x00000040
#define BEG_MASK_REG           (long) 0x00000050
#define BEG_MASK_POR           (long) 0x00000060
#define BEG_MASK_RIN           (long) 0x00000070

#define BEG_MASK_VECT          (long) 0x00000001
#define BEG_MASK_SING          (long) 0x00000002
#define BEG_MASK_TYPE          (long) 0x0000000F

#define BEG_TYPE_VECTOUT       (long) 0x00000011
#define BEG_TYPE_SINGOUT       (long) 0x00000012
#define BEG_TYPE_VECTAUX       (long) 0x00000021
#define BEG_TYPE_SINGAUX       (long) 0x00000022
#define BEG_TYPE_VECTBUS       (long) 0x00000031
#define BEG_TYPE_SINGBUS       (long) 0x00000032
#define BEG_TYPE_VECTBUX       (long) 0x00000041
#define BEG_TYPE_SINGBUX       (long) 0x00000042
#define BEG_TYPE_VECTREG       (long) 0x00000051
#define BEG_TYPE_SINGREG       (long) 0x00000052
#define BEG_TYPE_VECTPOR       (long) 0x00000061
#define BEG_TYPE_SINGPOR       (long) 0x00000062
#define BEG_TYPE_CONFLIT       (long) 0x00000071

// mode de recherche pour beg_get_vectname
#define BEG_SEARCH_NAME        '('
#define BEG_SEARCH_ABL         ' '
#define BEG_SEARCH_LOOP        'L'
#define BEG_SEARCH_ALOOP       'a'

#define BEG_SAVE               1
#define BEG_NOVERIF            (int)  0xFF00
// verify coherence with bvl

#define BEG_OBJ                ptype_list*
#define BEG_CORRESP_PTYPE      (long) 0xc1c01263
#define BEG_RENAME_LIST        (long) 0xc1c03093

void             beg_init            ( void                     );
void             beg_def_por         ( char          *name      ,
                                       char           direction );
void             beg_def_sig         ( char          *name      ,
                                       char          *expr      ,
                                       unsigned int   time,int,int, long);
biabl_list*      beg_def_mux         ( char          *name      ,
                                       char          *cndexpr   ,
                                       char          *bitstr    ,
                                       char          *valexpr   ,
                                       int            time,
                                        long flags, long biflags);
biabl_list*      beg_def_process     ( char          *name      ,
                                       char          *cndexpr   ,
                                       char          *valexpr   ,
                                       int            time      ,
                                       int            timer     ,
                                       int            timef     ,
                                       long           flag,
                                        long flags, long biflags);
biabl_list*      beg_def_loop        ( char          *name      ,
                                       char          *cndexpr   ,
                                       char          *valexpr   ,
                                       char          *varname   ,
                                       int            time      ,
                                       long           flag,
                                        long flags, long biflags);
biabl_list*      beg_def_biloop      ( char          *name      ,
                                       char          *cndexpr   ,
                                       char          *valexpr   ,
                                       char          *var1      ,
                                       char          *var2      ,
                                       int            time      ,
                                       long           flag,
                                        long flags, long biflags );
void             beg_assertion       ( char          *expr      ,
                                       char          *msg       ,
                                       char           level     ,
                                       char          *label     );

befig_list      *beg_get_befig       ( int            verif     );
void             beg_freeBefig       ( void                     );
void             beg_delBefig        ( befig_list    *fig       );
void             beg_def_befig       ( char          *name      );

void             beg_get_vectname    ( char          *signame   ,
                                       char          **name     ,
                                       long          *left      ,
                                       long          *right     ,
                                       char           mode      );
void             beg_eat_por         ( befig_list    *eaten     );
void             beg_eat_figure      ( befig_list    *eaten     );
befig_list      *beg_get_befigByName ( char          *name      );

void             beg_sort            ( void);
// !!! WARNING beg_compact need beg_sort to be performed before use
void             beg_compact         ( void);

int              beg_isVect          ( BEG_OBJ        obj       );
BEG_OBJ          beg_getObjByName    ( befig_list    *fig       ,
                                       char          *name      ,
                                       BEG_OBJ       *por       );
berin_list      *beg_getBerinByName  ( befig_list    *fig       ,
                                       char          *name      );
int              beg_getLeft         ( BEG_OBJ        obj       );
int              beg_getRight        ( BEG_OBJ        obj       );
void             beg_newRight        ( BEG_OBJ        obj       ,
                                       int            right     );
void             beg_newLeft         ( BEG_OBJ        obj       ,
                                       int            left      );
vectbiabl_list  *beg_getVectBiabl    ( BEG_OBJ        obj       );

chain_list      *beg_str2Abl         ( char          *str       );
char            *beg_boolDelNot      ( chain_list    *expr      ,
                                       chain_list   **res       );
chain_list      *beg_compactAbl      ( chain_list    *expr      );
int              beg_isDeletedBiabl  ( biabl_list    *biabl     );

void             beg_add_pors        ( befig_list    *fig       );

void             beg_terminate       ( befig_list    *fig       );

void             beg_topLevel        ( befig_list   **befig     ,
                                       chain_list    *insbefigs ,
                                       int            compact   ,
                                       loins_list    *loins , ht **renamed, ht **morealiases);
void             beg_addAllInstances ( char *curbefig, befig_list    *befig     ,
                                       loins_list    *loins     ,
                                       ht            *fight     ,
                                       ht            *sight     ,
                                       int            mode      ,
                                       void         (*eatfunc)
                                                    ( loins_list*,
                                                      ht*       ,
                                                      ht*       ,
                                                      ht*       ));
char            *beg_get_currentName ( void                     );
befig_list      *beg_duplicate       ( befig_list    *model     ,
                                       char          *insname   );

#define BEGAPI_HT_BASE       13
#define DISABLE_CREATE_BEH   2
#define DISABLE_CREATE_COR   4
#define DISABLE_COMPACT      8

ht   *begGetCorrespHT(char *name);
void begFreeCorrespHT(befig_list *bf, char *name);
ht   *begGetCorrespHTCreate(char *name);
void begGetCorrespHTUnlink(char *name);
void begGetCorrespHTRelink(char *name, ht *newc);
void beg_forceZSort(void);

#endif
