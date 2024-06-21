/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_vector.h                                          */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/***************************************************************************/
/* compare if arrays of instance and model are equal                       */
/* if different return 1 else 0                                            */
/* a signal without any dimension matches with all vector model (cf. VHDL) */
/***************************************************************************/
extern int Compare_Vector(tree_list *instance, tree_list *model, 
                           ptype_list *env_instance, ptype_list *env_model);

/***************************************************************************/
/* verify that instance vector is contained by model vector                */
/***************************************************************************/
extern int Bound_Vector(tree_list *instance, tree_list *model,
                        ptype_list *env);
                        
/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
extern int Verif_Vector(tree_list *tree, ptype_list *env);

/***************************************************************************/
/*               verify the direction of a vector array                    */
/* return size of vector                                                   */
/* env is the list of known variables                                      */
/***************************************************************************/
extern int Size_Vector(tree_list *tree, ptype_list *env);

/* return number of vars in vector expression, updated after using Size_Vector*/
int Nb_Vector_Vars();
