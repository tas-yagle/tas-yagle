/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_constrain.h                                             */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagInitConstraints (inffig_list *ifl);
void            yagDeleteConstraints (void);
chain_list     *yagGetConstraint (chain_list *ptcontraints);
void            yagTagConstraint (void *object);
void            yagRemoveConstraintTags (void);
chain_list     *yagGetOppositeConstraintVars (char *name);
