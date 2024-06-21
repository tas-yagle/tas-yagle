/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_global.h                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 09/02/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/
/*     for all architecture field in file, attach it to its own figure      */
/****************************************************************************/
extern void Get_All_Loins(tree_list *tree, chain_list *fcl);

/****************************************************************************/
/*                  build all interfaces in file                            */
/****************************************************************************/
extern void Get_All_Lofig(tree_list *tree, chain_list *fcl);

/****************************************************************************/
/*                build the model for a transistor X                        */
/****************************************************************************/
extern lofig_list *Build_Transistor(char* name);

/****************************************************************************/
/*build the model data structure with the syntaxic tree from the parsed file*/
/****************************************************************************/
extern void Build_All_Models(tree_list *tree, chain_list *fcl);
extern void Build_All_Transistor_Models();

extern void LoadSpiceFCLFiles(lib_entry *lib);

int ispragmacommentsym(char *txt);
int ispragmacommentcoupled(char *txt);
int gns_isunusedsig(losig_list *ls);
