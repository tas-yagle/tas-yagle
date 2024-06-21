/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_GENIUS.h                                          */
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
/****************************************************************************/
extern chain_list *Get_Losig_Generic(locon_list *locon, chain_list *signal, 
                              chain_list *sigchain);
                              
/****************************************************************************/
/*     return the abl generic function of all created instances in fig      */
/****************************************************************************/
extern tree_list *Get_Loins_Generic(tree_list *tree, tree_list *archi);

/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Signals_Generic(tree_list *tree);

/****************************************************************************/
/*         build one locon of fig (and also its external signal)            */
/****************************************************************************/
extern void Get_Locon_Generic(int dir, tree_list *tree);

/****************************************************************************/
/*         build one locon of fig (and also its external signal)            */
/****************************************************************************/
extern void Get_Locon_Alliance(int dir, tree_list *tree);

/****************************************************************************/
/*   build all the locons of and its signals from tree pointing on port     */
/****************************************************************************/
extern void Get_Port_Generic(tree_list *tree);

/****************************************************************************/
/*           return list of variables contained in tree                     */
/*  result is put on top of var                                             */
/****************************************************************************/
extern chain_list* Get_Generic(tree_list* tree, chain_list *var);

/****************************************************************************/
/*                 put environment to good values                           */
/****************************************************************************/
extern void Env_Generic(lofig_list *lofig);


extern void verif_instance_flags(ptype_list *p, char *file, char *type);
void verif_instance_flags_chainlist(chain_list *p, char *file, char *type);

