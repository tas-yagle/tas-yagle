/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_exclude.h                                         */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/
/*  return the list of instances excluded contained in tree and put on the  */
/* top of exclude.verify double exclusion                                   */
/****************************************************************************/
extern ptype_list *Verify_Exclude(tree_list *tree, 
                                         ptype_list *exclude);

/****************************************************************************/
/* free the last list of exclude and build another from tree                */
/* must be done before all other functions below                            */
/****************************************************************************/
extern void Init_Exclude(tree_list *tree);

/***************************************************************************/
/*    control if loins is Excluded return line number if true else 0       */
/***************************************************************************/
extern int Is_Exclude(char *loins);

/***************************************************************************/
/*       control if all loins named in Excluded are in architecture        */
/***************************************************************************/
extern void Verif_Alone_Exclude(tree_list *archi);

/***************************************************************************/
/*             comparison between type of tree and type of instance        */
/* an error is dumped if discrepancy                                       */
/***************************************************************************/
extern void Compare_Type(tree_list *tree,
                         tree_list *instance, int type_instance);


extern void Expand_Exclude(ptype_list *pt);
