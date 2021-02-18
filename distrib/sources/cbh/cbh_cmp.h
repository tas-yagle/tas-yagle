/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_cmp.h                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef CBH_CMP_H
#define CBH_CMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include MLO_H
#include CBH_H
#include "cbh_util.h"

#define CBH_GET         ((int) 0x0000)
#define CBH_CLASS       ((int) 0x0001)
#define CBH_DEL         ((int) 0x0002)

#define CBH_IN          ((int) 0x0000)
#define CBH_OUT         ((int) 0x0001)
#define CBH_Q           ((int) 0x0000)
#define CBH_NQ          ((int) 0x0001)

#define CBH_MAXNBFUNC   ((int)     10)

#define CBH_FUNCTION    ((int) 0x0000)
#define CBH_HZFUNC      ((int) 0x0001)
#define CBH_CONFLICT    ((int) 0x0002)
#define CBH_CLOCK       ((int) 0x0003)
#define CBH_SLAVECLOCK  ((int) 0x0004)
#define CBH_DATA        ((int) 0x0005)
#define CBH_RESET       ((int) 0x0006)
#define CBH_SET         ((int) 0x0007)
#define CBH_RSCONF      ((int) 0x0008)
#define CBH_RSCONFNEG   ((int) 0x0009)

#define CBH_REF         ((int) 0x0000)
#define CBH_NEW         ((int) 0x0001)



extern  chain_list  *cbh_lookbdd        ( chain_list  *input	    ,
                                          int         *nbinout	    ,
                                          lofig_list  *newlofig	    , 
                                          int         *loop	        ,
                                          char       **newnamein     );
extern  chain_list  *cbh_lookpermut     ( chain_list  *reschain	    ,
                                          chain_list  *tmpchain	    ,
                                          int          n	        ,
                                          lofig_list  *newlofig	    ,
                                          int         *loop	        ,
                                          char       **newnamein	,
                                          int         *nbinout       );
extern  chain_list *cbh_circularchain   ( chain_list  *chain         );
extern  void        cbh_uncircularchain ( chain_list  *tail          );
extern  chain_list *cbh_masksndhead     ( chain_list  *chain         );
extern  void        cbh_unmasksndhead   ( chain_list  *chain	    ,
                                          chain_list  *sndhead);
extern  void        cbh_freechain       ( chain_list  *chain         );
extern  pCircuit    cbh_cmpbdd          ( chain_list  *newin	    ,
                                          lofig_list  *newlofig	    ,
                                          int         *loop	        ,
                                          char       **newnamein	,
                                          int         *nbinout       );
extern  int         cbh_cmpbdd_abl      ( lofig_list  *newlofig	    ,
                                          lofig_list  *reflofig	    ,
                                          chain_list  **refinout	,
                                          int         *loop	        ,
                                          char       **refnamein	,
                                          char       **newnamein	,
                                          chain_list **newinout	    ,
                                          int         *nbinout       );
extern  int         cbh_cmplofig_IO     ( lofig_list  *reflofig	    ,
                                          lofig_list  *newlofig	    ,
                                          int          func	        ,
                                          chain_list **newinout	    ,
                                          int         *nbinout       );
extern  float       cbh_cmpfanout       ( lofig_list  *lofig1	    ,
                                          float        fanout        );
extern  int         cbh_equalVarExpr    ( chain_list  *expr1	    ,
                                          chain_list  *expr2         );
extern  int         cbh_createabllist   ( lofig_list  *lofig	    ,
                                          int          func	        ,
                                          chain_list **mainabllist	,
                                          chain_list  *out           );
extern  void        cbh_addseq2abllist  ( void);      
extern  void        cbh_addabl          ( chain_list **mainabllist	,
                                          chain_list  *abllist	    ,
                                          char        *name	        ,
                                          int          where         );
extern  int         cbh_cmpabl          ( void);
extern  int         cbh_existloop       ( lofig_list  *lofig	    ,
                                          int         *loop	        ,
                                          char       **name          );
extern  pCircuit    cbh_initcct         ( lofig_list  *lofig	    ,
                                          int          nbin	        ,
                                          int          nbout	    ,
                                          int         *loop	        ,
                                          int          nbabl         );
extern  void        cbh_addincct        ( pCircuit     cct	        ,
                                          chain_list  *loconin	    ,
                                          int         *loop	        ,
                                          char       **name          );
extern  void        cbh_addoutcct       ( pCircuit     cct	        ,
                                          pNode        bdd	        ,
                                          char        *name	        ,
                                          int          where         );
extern  int         cbh_freeMainAblList ( int          ret          ,
                                          chain_list  **refinout	 );
        void        cbh_freeabllist     ( chain_list  **mainabllist  );
#endif
