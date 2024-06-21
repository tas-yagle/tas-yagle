/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Fichier : gsp_lib.h                                                   */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/*****************************************************************************/

/*****************************************************************************/
/*     includes                                                              */
/*****************************************************************************/

#ifndef GSP
#define GSP


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include MSL_H
#include RCN_H
#include INF_H
#include CNS_H
#include LOG_H
#include CBH_H
#include ELP_H
#include EFG_H
#include EQT_H

/*****************************************************************************/
/*     defines                                                               */
/*****************************************************************************/
// stucks for the inputs

#define GSP_STUCK_ZERO    ((long) 0x00000000 )
#define GSP_STUCK_ONE     ((long) 0x00000001 )
#define GSP_STUCK_INDPD   ((long) 0x00000002 )

#define GSP_CONE_NUMBER   ((long) 0x10000000 ) /* numerotation des cone     */

#define GSP_SWITCHPAIR    ((long) 0x20000000 ) /* contient la cmd opposee   */

#define GSP_COMPUTE_INITIAL_STATE 1
#define GSP_FORCE_COMPUTE_INITIAL_STATE 2

extern int GSP_QUIET_MODE;
extern int GSP_REAL_ORDER_SET;
extern int GSP_LOOP_MODE;
extern int GSP_FOUND_SOLUTION_NUMBER;

/*****************************************************************************/
/*     structures                                                            */
/*****************************************************************************/
typedef struct 
{
  int MinDate, Date;
  char BeforeTransitionState, AfterTransitionState;
  char *cmd, cmd_state, used_ic;
  int pathtag, precharged, usestate;
} gsp_cone_info;

typedef struct
{
  char *dest, *orig;
  int tog;
} gsp_correl;

/*****************************************************************************/
/*     functions                                                             */
/*****************************************************************************/
extern char           gsp_is_pat_indpd      (ptype_list *patterns);
extern ptype_list    *gsp_get_patterns      ( lofig_list  *lofig,
                                              cnsfig_list *cnsfig,
                                              lofig_list  *figext,
                                              chain_list  *chaincone,
                                              chain_list  *chaininstance,
                                              char,
                                              spisig_list *,
                                              chain_list **
                                            );
// temp
ptype_list *gsp_spisetinputs_from_cone(char *figname, chain_list *ptconelist, char corner, chain_list **allcorrel);
void gsp_new_numbercones (char *figname, chain_list *ptconelist);
void gsp_FreeMarksOnCnsfig (cnsfig_list *cnsfig);
void gsp_FreeMarksOnCnsfigFromChainCone (chain_list *cl);
gsp_cone_info *gsp_get_cone_info(cone_list *cone);
void gsp_delete_cone_info(cone_list *cone);
gsp_cone_info *gsp_create_cone_info(cone_list *cone);
void gsp_freecorrel(chain_list *head);
losig_list *gsp_FindSimpleCorrelationForToBeStuckConnectors(cone_list *cone,int depth,int maxdepth, int *tog, int setonly, chain_list **conelist);

#endif
