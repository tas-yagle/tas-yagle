/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_ctx.h                                                   */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef __TTV_CTX_H__
#define __TTV_CTX_H__

extern char *CTX_FILE ;
extern int CTX_LINE ;

extern int                ttv_ctxparse        __P(( ttvfig_list* ));
extern void               ttv_printctxline    __P(( ttvfig_list*,
                                                    FILE *,
                                                    long 
                                                 ));
extern void               ttv_ctxdrive        __P(( ttvfig_list* ));

#endif //__TTV_CTX_H__
