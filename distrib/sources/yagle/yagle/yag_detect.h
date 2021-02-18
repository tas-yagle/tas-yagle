/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_detect.h                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagDetectBadBranches(cone_list *ptcone);
void            yagDetectParaTrans (cone_list *ptcone);
void            yagAddSwitchInversion(cnsfig_list *ptcnsfig);
void            yagCorrectSwitch (cone_list *ptcone);
void            yagDetectSwitch (cone_list *ptcone);
void            yagDetectParallelBranches (cone_list *ptcone);
void            yagDetectPullup (cone_list *ptcone);
void            yagDetectPulldown (cone_list *ptcone);
void            yagDetectLoops (cone_list *ptcone);
int             yagCountLoops (cone_list *ptcone, int warning);
int             yagDetectTransfer (cone_list *ptcone);
void            yagDetectDegraded (cone_list *ptcone);
int             yagDetectFalseConf (cone_list *ptcone);
int             yagCheckFalseConfBranch (branch_list *ptbranch, losig_list *ptfirstsig);
void            yagDetectLoopConf (cone_list *ptcone);
void            yagRemoveRedundantBranches (cone_list *ptcone);
void            yagTransferParallel (lofig_list *ptlofig);
int             yagDetectGlitcher (branch_list *ptbranch, lotrs_list *pttrans, int mark);
void            yagDetectClockLatch (inffig_list *ifl, cnsfig_list *ptcnsfig);
void            yagDetectClockPrech (inffig_list *ifl, cnsfig_list *ptcnsfig);
int             yagPropagateStuck (cone_list *ptcone);
int             yagDetectStuck (cone_list *ptcone);
void            yagUnmarkLatch (cone_list *ptcone, int cutloop, int warn);
void            yagInfUnmarkLatches (inffig_list *ifl, cnsfig_list *ptcnsfig);
void            yagDetectNotClockLatch(inffig_list *ifl, cnsfig_list *ptcnsfig);
void            yagDetectClockGating(inffig_list *ifl, cnsfig_list *ptcnsfig);
void            yagDetectDelayedRS(cnsfig_list *ptcnsfig);
