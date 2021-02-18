/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_transistor.h                                      */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 23/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#if 0
#define TN_NAME namealloc("TN")               /*transistors predefined rules*/
#define TP_NAME namealloc("TP")               /* model (in,inout,inout) */
#define SOURCE_NAME namealloc("source")       /* names of trans. branches */
#define DRAIN_NAME  namealloc("drain")
#define GRID_NAME   namealloc("grid")
#define BULK_NAME   namealloc("bulk")
#endif


/****************************************************************************/
/*                build the model for a transistor X                        */
/****************************************************************************/
extern lofig_list *Build_Transistor(char* name) ;
extern lotrs_list *BuildFakeTransistor(char type, losig_list *grid, losig_list *drain, losig_list *source, losig_list *bulk);
