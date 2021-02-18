/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_analysis.h                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

int stb_compstberr __P((chain_list **chain1, chain_list **chain2)) ;
void stb_delerrorlist __P((chain_list *chainerr)) ;
chain_list *stb_geterrorlist __P((stbfig_list *stbfig, long margin, int nberror, long *minsetup, long *minhold, int *errnumb)) ;
stbfig_list *stb_analysis __P((ttvfig_list *ttvfig, char analysemode, char graphmode, char stabilitymode, char ctkmode));
void stb_testSTB(ttvfig_list *ttvfig);
stbfig_list *stb_parseSTO(ttvfig_list *ttvfig, char *filename);

