/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_exp_VHDL.h                                        */
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
/*          Control the expression of FOR in VHDL for later searching      */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_HighBound_FOR(tree_list *tree, ptype_list *env);


/***************************************************************************/
/*               eval and verify a scalar expression in VHDL               */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_Exp_VHDL(tree_list *tree, ptype_list *env);

/***************************************************************************/
/*            Verify and Evaluate a boolean expression in VHDL             */
/*                 env list of variables with their values                 */
/***************************************************************************/
extern int Verif_Bool_VHDL(tree_list *tree, ptype_list *env);

