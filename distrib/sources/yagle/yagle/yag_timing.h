/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_timing.h                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* includes for tas timing */

#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include TAS_H
#include MCC_H

void            yagPrepTiming(lofig_list *ptlofig);
void            yagAddTiming(cnsfig_list *ptcnsfig, lofig_list *ptlofig);
int             yagCalcStmResPair(chain_list *uplist, chain_list *downlist, cone_list *ptcone, float *ptupresistance, float *ptdownresistance);
int             yagCalcStmResistance(chain_list *ptbranchlist, cone_list *ptcone, long type, float *ptresistance);
int             yagStmBranchCurrent(branch_list *ptbranch, float vin, float vout, float vsupply, float *ptcurrent);
void            yagPrepStmSolver(cone_list *ptcone);
