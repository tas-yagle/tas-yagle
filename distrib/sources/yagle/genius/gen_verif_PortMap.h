/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_PortMap.h                                         */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define DEFAULT_ARRAY 0x4   /* for a generic vector, give a minimal size */
#define RAND_BYTE ((rand()&0xF)+DEFAULT_ARRAY) /* value for a generic variable */


/***************************************************************************/
/*  change a tree of generic variables in a list of ptypeput on top of var */
/***************************************************************************/
extern ptype_list *GenTree2chain(tree_list *tree, ptype_list *var);

/***************************************************************************/
/* Verify that all instances of tree match with components Comp and generic*/
/* variables and signals Sig                                               */
/* result put on top of Ins                                                */  
/***************************************************************************/
extern chain_list *Verif_Instance(tree_list *tree, chain_list *Sig,
                                  ptype_list *env, chain_list *Comp,
                                  chain_list *Ins);


