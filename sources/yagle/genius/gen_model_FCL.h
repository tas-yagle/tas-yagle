/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_FCL.h                                             */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 23/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/



/****************************************************************************/
/*   build and return the list of losig in tree put on top of sigchain      */
/* build also the generic of locon list and losig list returned             */
/* signal from tree                                                         */
/****************************************************************************/
extern chain_list *Get_Losig_Alliance(locon_list *locon, chain_list *signal, 
                              chain_list *sigchain);

/****************************************************************************/
/*     return the abl generic function of all created instances in fig      */
/****************************************************************************/
extern tree_list *Get_Loins_Alliance(tree_list *tree);

/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Signals_Alliance(tree_list *tree);

/****************************************************************************/
/*         build one locon of fig (and also its external signal)            */
/****************************************************************************/
extern void Get_Locon_Alliance(int dir, tree_list *tree);

/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Port_Alliance(tree_list *tree);

/****************************************************************************/
/*                 put environment to good values                           */
/****************************************************************************/
extern void Env_Alliance(lofig_list *lofig);


