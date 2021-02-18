/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TMA Version 1                                               */
/*    Fichier : tma_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/


/****************************************************************************/

extern void  tma_FreeInfList (inffig_list *ifl);
extern short tma_RetKey    (char *chaine);
extern void  tma_Error     ();
extern void  tma_GetTmaOpt (int argc, char *argv[]);
extern void  tma_DelTmaOpt (int *argc, char **argv[]);
extern void  tma_init      ();
extern int   tmaenv        (long filetype);
extern void  tma_yaginit   ();
