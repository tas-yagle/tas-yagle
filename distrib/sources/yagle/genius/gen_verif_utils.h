/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_utils.h                                           */
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
/* put ident at the END of the list, this element must be unique           */
/* comparison is supposed to be on string                                  */
/* if an elem has the same key, an error is dumped                         */
/***************************************************************************/
extern chain_list *put_inlist(tree_list *ident, chain_list *list); 

/***************************************************************************/
/* return the element of list wich DATA field equals DATA field of ident   */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern tree_list *fetch_inlist(tree_list *ident, chain_list *list); 

/***************************************************************************/
/* put ident with a value at the TOP of the list, this element must unique */
/* comparison is supposed to be on string                                  */
/* if an elem has the same key, an error is dumped                         */
/***************************************************************************/
extern ptype_list *put_value(tree_list *ident, int value, ptype_list *list); 

/***************************************************************************/
/* return the value of the element of list wich DATA field equals DATA     */
/* field of ident                                                          */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern int fetch_value(tree_list *ident, ptype_list *list); 


