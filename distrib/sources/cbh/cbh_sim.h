/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_sim.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef CBH_SIM_H
#define CBH_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/types.h"
#include "sys/time.h"
#include "sys/resource.h"
#include MUT_H
#include LOG_H
#include BEH_H
#include MLO_H
#include CBH_H
#include "cbh_cmp.h"
#include "cbh_error.h"

#define CBH_ONETRANS  1
#define CBH_ALLTRANS  0

#define CBH_SET_TRANS ((int)  0x0001)
#define CBH_CHK_TRANS ((int)  0x0002)

#define CBH_CST_DOWN  ((long) 0xFFFFFFFF)


extern  chain_list  *cbh_propagate          ( chain_list    *toBeTreatedList );
extern  chain_list  *cbh_applyConstraint    ( chain_list    *toBeTreatedList,
                                              locon_list    *locon           );
extern  chain_list  *cbh_getToBeTreatedList ( chain_list    *toBeTreatedList,
                                              lofig_list    *lofig           );
extern  void         cbh_doLoinsToLofig     ( lofig_list    *lofig           );
extern  void         cbh_doMatchLocon       ( lofig_list    *lofig          ,
                                              loins_list    *loins           );
extern  locon_list  *cbh_getMatchLocon      ( locon_list    *locon           );
extern  int          cbh_propageBdd         ( pNode          bdd            ,
                                              pCircuit       cct            ,
                                              chain_list    *lofigInList     );
extern  chain_list  *cbh_getLoconList       ( losig_list    *sig             );
extern  lofig_list  *cbh_getLofigFromLoins  ( loins_list    *loins           );
extern  void         cbh_addSigToTreat      ( chain_list    *tBTL           ,
                                              losig_list    *sig             );
extern  chain_list  *cbh_delTreatedList     ( chain_list    *tBTL           ,
                                              chain_list    *hBT             );
extern  char         cbh_cst                ( int            constraint      );
extern  int          cbh_getLoconCst        ( locon_list    *locon           );
extern  void         cbh_setLoconCst        ( locon_list    *locon          ,
                                              int            cst             );
extern  void         cbh_delLoconCst        ( locon_list    *locon           );
extern  long         cbh_findTrans2         ( lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type           ,
                                              int            mode            );
extern  ptype_list  *cbh_resConf            ( loins_list    *loins          ,
                                              lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out             );
extern  long         cbh_decodeTrans        ( locon_list    *in             ,
                                              locon_list    *out            ,
                                              int            code           ,
                                              int            mode            );
extern  void         cbh_setLoconForTrans   ( locon_list    *in             ,
                                              int            cstin          ,
                                              locon_list    *out            ,
                                              int            cstout          );
extern  pNode        cbh_constraintBdd      ( pNode          bdd            ,
                                              pCircuit       cct            ,
                                              locon_list    *locon           );
extern  chain_list  *cbh_inWithoutA         ( lofig_list    *lofig          ,
                                              locon_list    *a              ,
                                              int           *nb              );
extern  void         cbh_printTrans         ( long           code            );
extern  void         cbh_delCstOnLofig      ( lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out             );
extern  char        *cbh_ltob               ( long           l              ,
                                              char          *b               );
extern  int          cbh_isInput            ( locon_list    *locon           );
extern  void         cbh_setLofigCstOnLoins ( loins_list    *loins           );
extern  int          cbh_setCstWithBdd      ( lofig_list    *lofig          ,
                                              pNode          bdd            ,
                                              pCircuit       cct             );
extern  pNode        cbh_constraintBddWithCst(lofig_list    *lofig          ,
                                              pNode          bdd            ,
                                              pCircuit       cct             );
extern  chain_list  *cbh_delCstedLoconFromList(chain_list   *inList          );

extern  void         cbh_setCstToLofig      ( loins_list    *loins           );
extern  void         cbh_delCstFromLofig    ( loins_list    *loins           );

extern  int          cbh_getLoconCst        ( locon_list    *locon           );
extern  void         cbh_setLoconCst        ( locon_list    *locon           ,
                                              int            cst             );
extern  void         cbh_delLoconCst        ( locon_list    *locon           );

extern  void         cbh_setLofigCstOnLoins ( loins_list    *loins           );
extern  long         cbh_existLoinsTrans    ( loins_list    *loins          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type           ,
                                              ptype_list    *cstList         );
extern  ptype_list  *cbh_confLoinsForTrans  ( loins_list    *loins          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type           ,
                                              ptype_list    *cstList         );
extern  long         cbh_existTrans         ( lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type            );
extern  ptype_list  *cbh_confForTrans       ( lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type            );
extern  void         cbh_printLofigCst      ( lofig_list    *lofig          ,
                                              loins_list    *loins          ,
                                              locon_list    *in             ,
                                              locon_list    *out             );
extern  void         cbh_printPTypeCst      ( ptype_list    *pType           );
extern  lofig_list  *cbh_doMatchLofig       ( loins_list    *loins           );
extern  ptype_list  *cbh_mvResLofigLoins    ( loins_list    *loins          ,
                                              ptype_list    *res             );
extern  ptype_list  *cbh_getHZCstFLoins     ( loins_list    *loins          ,
                                              locon_list    *insOut         ,
                                              ptype_list    *cstList        ,
                                              int            enable          );
extern  ptype_list  *cbh_getHzCst           ( lofig_list    *lofig          ,
                                              locon_list    *locon          ,
                                              int            enable          );
/*
extern  int          cbh_evalBdd            ( pNode          bdd            ,
                                              pCircuit       cct            ,
                                              locon_list    *locon           );
extern  long         cbh_findTrans          ( lofig_list    *lofig          ,
                                              locon_list    *in             ,
                                              locon_list    *out            ,
                                              long           type           ,
                                              int            mode            );
extern  long         cbh_lookCombi          ( pNode          bdd            ,
                                              pCircuit       cct            ,
                                              chain_list    *loconList      ,
                                              long           target          );
extern  locon_list  *cbh_checkAllCstSet     ( locon_list    *locon           );
extern  int          cbh_evalBddUnFull      ( pNode          bdd            ,
                                              pCircuit       cct            ,
                                              locon_list    *locon          ,
                                              int            resOut          );
extern  int          cbh_bddToCst           ( pNode          bdd            ,
                                              int            target          );
extern  int          cbh_checkTrans         ( locon_list    *in             ,
                                              locon_list    *out            ,
                                              int            code           ,
                                              pNode          bdd            ,
                                              pCircuit       cct            ,
                                              lofig_list    *lofig          ,
                                              chain_list    *loconList       );
*/


#endif
