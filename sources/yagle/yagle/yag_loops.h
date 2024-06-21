/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_loops.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define YAG_LOOP_IGNORE 0
#define YAG_LOOP_INVERT 1
#define YAG_LOOP_NONINVERT 2

void yagAnalyseLoop (cone_list *ptcone, int fIgnoreFalse);
pNode yagAnalyseElectric (cone_list *ptcone, pCircuit coneCct, int fIgnoreFalse);
int yagIsInversion(pCircuit circuit, char *input, char *output);
void yagMarkMemory (pCircuit ptcircuit, cone_list *ptcone, chain_list *commandlist, chain_list *async_command_list, chain_list *looplist, pNode memorize, pNode write, pNode conflict, pNode async_write, int fIgnoreFalse);
chain_list *yagGetCommands (pCircuit globalCct, cone_list *ptcone, pNode condition, int strict, chain_list **ptdatacommands);
void yagBuildDirectInputBdds (cone_list *ptcone);
pCircuit yagBuildDirectCct (cone_list *ptcone);
chain_list *yagGetCorrelatedWriteSupport (pCircuit globalCct, pCircuit latchCct, pNode write, chain_list *ptallsupport);
chain_list *yagGetWriteSupport (pCircuit globalCct, pCircuit latchCct, pNode write, chain_list *ptallsupport);
void yagDisableRSArcs(cone_list * ptcone1, cone_list *ptcone2, cone_list * ptloopcone1, cone_list *ptloopcone2, long rstype, short cuttype);

