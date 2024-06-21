/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_iterative.h                                      */
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
/*   for loop model to change recursive algorithm to en iterative           */
/****************************************************************************/
extern int continue_search_on(tree_list* tree, foundins_list *circuit_foundins, loins_list *modelins, ptype_list *env);

/****************************************************************************/
/*    Search occurences of model contained in circuit and maybe begin from  */
/*   the rarest                                                             */
/****************************************************************************/
extern int genFindInstances(lofig_list *circuit, model_list *model, char *rarest_model);

extern char *current_variable_name;
extern chain_list *current_variable_values;
