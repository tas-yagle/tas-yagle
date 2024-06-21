/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_recursive.h                                      */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 12/08/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/



/****************************************************************************/
/*  begin of recursive search on circuit_ins, if phase=1 a generic          */
/* instances list is generate wich can be read by get_Generic_foundins()    */
/* phase=0 environment stays at the same place, just look if match          */
/* phase=2 environment moves                                                */
/****************************************************************************/
extern int Search_Phase(int phase, foundins_list* circuit_foundins, 
                        loins_list* model_ins, ptype_list *env);

/****************************************************************************/
/* take the generic instances produced by searching for PHASE 1             */
/****************************************************************************/
extern chain_list* get_Generic_foundins();

/****************************************************************************/
/*  erase all generic instances from an internal list for PHASE 1           */
/****************************************************************************/
extern void free_Generic_foundins();

extern losig_list *getlosigfrommark(gensigchain_list* gen, int numbit);

extern foundins_list *first_instance_in_for;
extern chain_list *signal_to_grab_instance_from;

extern int GOTO_MODE, DECISION_CHANGE_MODE;
extern void AddDecisionSnapshot(loins_list *li, loins_list *forwho);
extern int IMustChangeDecision();
extern int ThereAreDecisionToChange();
extern void ClearAllDecisions();
extern void ResetBackTrackCounter();
extern void IncreaseBackTrackCounter();
