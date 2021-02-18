/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_global.h                                          */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 08/04/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#define FCL    0x4
#define GENIUS 0x5

/***************************************************************************/
/*     Verify that tree is component of pre-defined rule TN or TP          */
/***************************************************************************/
extern void Verif_Transistor(tree_list *tree);

/***************************************************************************/
/*                     Verify if port is correct                           */
/* return list of connectors put on the top of Con                         */
/* env is environment where are listed known variables                     */
/***************************************************************************/
extern chain_list *Verif_Port(tree_list *tree, ptype_list *env,
                              chain_list *Con);

/***************************************************************************/
/*        Verify if generic of component and model are equal               */
/* the list of variables with values is returned on top of Gen             */
/***************************************************************************/
extern ptype_list *Verif_Generic(tree_list *tree, ptype_list *Gen);

/***************************************************************************/
/*         Verify if all signals in tree use variables listed in env       */
/* result is put on top of Sig                                             */
/***************************************************************************/
extern chain_list *Verif_All_Signals(tree_list *tree, ptype_list *env,
                                     chain_list *Sig);

/***************************************************************************/
/*   Verify if all components in tree match with their models in Entity    */
/* result is put on top of Comp                                            */
/***************************************************************************/
extern chain_list *Verif_All_Components(tree_list *tree, chain_list *Entity,
                                        chain_list *Comp);

/***************************************************************************/
/*        Verify in tree all architecture modules attached to entity       */
/* result is put on top of Archi                                           */
/***************************************************************************/
extern chain_list *Verif_Architecture(tree_list *tree, char *entity, 
                                      chain_list *Archi);

/***************************************************************************/
/*        Verify in tree all action modules attached to entity             */
/* result is put on top of Action                                          */
/***************************************************************************/
extern chain_list *Verif_Action(tree_list *tree, char *entity, 
                                chain_list *Action);

/***************************************************************************/
/*    Verify all entity declarations in tree and return the list of entity */
/* result is put on top of Ent                                             */
/***************************************************************************/
extern chain_list *Verif_Entity(tree_list *tree, chain_list *Ent); 

/***************************************************************************/
/* verify if all actions and architectures in tree have their entity named */
/* in the list Ent                                                         */
/* Ent is a list of tree                                                   */
/***************************************************************************/
extern void Verif_Alone(tree_list *tree, chain_list *Ent);

/***************************************************************************/
/*              Verify coherence of VHDL and C describtion                 */
/* return 2 lists typed GENIUS and FCL with the name of built models       */
/***************************************************************************/
extern ptype_list *Verif_All(tree_list *tree); 

