/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_execute_VHDL.h                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 27/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/




/***************************************************************************/
/*                    Evaluate a scalar expression in VHDL                 */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Eval_Exp_VHDL(tree_list *tree, ptype_list *env);

/***************************************************************************/
/*                   Evaluate a boolean expression in VHDL                 */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Eval_Bool_VHDL(tree_list *tree, ptype_list *env);

/***************************************************************************/
/*               Solve an equation with one variable in VHDL               */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern void ResetNbSolved();
extern int  GetNbSolved();
extern void Solve_Equ_VHDL(tree_list *tree, int result, ptype_list *env);

/***************************************************************************/
/*           return the name of the first variable in expression           */
/***************************************************************************/
extern char* Getname_Exp_VHDL(tree_list* tree);

/****************************************************************************/
/* return 1 if loins is excluded in tree                                    */
/****************************************************************************/
extern int Look_Exclude(tree_list *tree, loins_list* loins);

/****************************************************************************/
/*  return non-zero if there is a variable not defined in Eval function     */
/*  put flag to zero for next use                                           */
/****************************************************************************/
extern int Exp_VHDL_Undef();


extern int __Count_Vars_Exp_VHDL(tree_list *tree);

// return the list of variables in an expression
extern chain_list *GetnameS_Exp_VHDL(tree_list *tree);
