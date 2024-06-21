/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_falsepath.h                                             */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

ht *stb_buildquickaccessht_forfalsepath(ttvfig_list *ttvfig, NameAllocator *NA);
void stb_freeequickaccessht_forfalsepath(ht *h, NameAllocator *NA);

int stb_isfalseslack(stbfig_list *stbfig, ttvevent_list *startck, ttvevent_list *eventin, ttvevent_list *eventout, ttvevent_list *endck, int type);
void stb_freefalseslack(stbfig_list *stbfig);
void stb_addfalsepath(stbfig_list *stbfig, ptype_list *siglist, ht *nametosig,NameAllocator *NA);
void stb_setfalseslack(stbfig_list *sf, inffig_list *ifl);
int stb_hasfalseslack(stbfig_list *stbfig, ttvevent_list *eventout);

