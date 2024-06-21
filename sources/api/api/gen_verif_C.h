/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_C.h                                               */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 04/05/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


#define VBE_NEW_FILE namealloc("newfile")   /*variable containing the VBE file flow*/


/***************************************************************************/
/*  change a tree of arguments in a list of ptype                          */
/***************************************************************************/
extern chain_list *args2chain(tree_list *tree, chain_list *var);

/***************************************************************************/
/* return the element of list wich DATA field equals DATA field of ident   */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern ptype_list *get_type(tree_list *ident, ptype_list *list);

/****************************************************************************/
/*          count the number of arguments of a function                     */
/****************************************************************************/
extern int Count_Args(tree_list *tree);

/****************************************************************************/
/*return the type of tree and verify that tree match with type_exp          */
/* env is the declared variables in field DATA and initialization in TYPE   */
/****************************************************************************/
extern int Type_Exp(tree_list *tree, int type_exp, ptype_list *env);

/****************************************************************************/
/*           clean the tree for execution and send messages                 */
/* Var is the list of all declared variables with their initialization      */
/* unused statements as 3+4; is warned and erased                           */
/* uninitialised variables are warned                                       */
/* control double declaration of variables                                  */
/* useless instructions are warned                                          */
/* verify number and type of arguments                                      */
/* illegal operations on types are counted as fatal errors                  */
/* fatal errors are counted                                                 */
/* return new environment                                                   */
/****************************************************************************/
extern ptype_list* Verify_Type(tree_list *tree, ptype_list *Var);

/****************************************************************************/
/*           clean the tree for execution and send messages error           */
/* Var is a list of all generic variables from VHDL                         */
/* unused statements as 3+4; is warned and erased                           */
/* uninitialised variables are warned                                       */
/* control double declaration of variables                                  */
/* useless instructions are warned                                          */
/* verify number and type of arguments                                      */
/* illegal operations on types are counted as fatal errors                  */
/* fatal errors are counted but not returned                                */
/* return new tree                                                          */
/****************************************************************************/
extern tree_list* Verify_C(tree_list *tree, ptype_list *Var);


