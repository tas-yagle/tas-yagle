/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_utillo.h                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define YAG_VECTO 0
#define YAG_VECDOWNTO 1

void            yagInitLoconVect __P((void));
void            yagCloseLoconVect __P((void));
locon_list     *yagAddLoconVect __P((lofig_list *ptlofig, char *name, losig_list *ptlosig, char dir, int vec, num_list *, ptype_list *));
void            yagAddGivenLoconVect __P((lofig_list *ptlofig, locon_list *ptlocon, int vec));
void            yagInitLosigVect __P((void));
void            yagCloseLosigVect __P((void));
losig_list     *yagAddLosigVect __P((lofig_list *ptlofig, long sigindex, chain_list *ptnamechain, char type));
