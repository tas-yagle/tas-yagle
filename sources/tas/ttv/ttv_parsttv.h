/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_parsttv.h                                                */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef __TTV_PARSTTV_H__
#define __TTV_PARSTTV_H__

#define TTV_FILE_COMP      'C'                  /* fichier compresse         */
#define TTV_FILE_NOTCOMP   'N'                  /* fichier non compresse     */

extern long TTV_OLD_NEW ;
extern long TTV_LAST_SIG ;
extern long TTV_FILE_POS ;

extern void         ttv_dupline        __P((ttvfig_list*,
                                            ttvfig_list*,
                                            ttvlbloc_list *,
                                            long,
                                            long
                                          )) ;
extern void         ttv_deldupindex    __P(( ttvfig_list*));
extern void         ttv_setdupindex    __P(( ttvfig_list*));
extern ttvfig_list* ttv_getmodeldup    __P(( ttvfig_list*,
                                             long
                                          ));
extern int          ttv_dupttvfig      __P(( ttvfig_list*,
                                             long,
                                             long,
                                             long,
                                             char
                                          ));
extern long         ttv_getfilepos     __P(( ttvfig_list*,
                                             long,
                                             FILE*,
                                             char,
                                             long
                                          ));
extern void         ttv_addfilepos     __P(( ttvfig_list*,
                                             FILE*,
                                             char*
                                          ));
extern void         ttv_parserror      __P(( char*, long ));
extern long         ttv_decodcontype   __P(( char* ));
extern long         ttv_decodlatchtype __P(( char* ));
extern ttvsig_list* ttv_decodsigtype   __P(( ttvfig_list*, long, char ));
extern chain_list*  ttv_parscmd        __P(( ttvfig_list*, 
                                             chain_list*,
                                             ttvsig_list*
                                          ));
extern chain_list*  ttv_parscaracsig   __P(( ttvfig_list*,
                                             chain_list*,
                                             ttvsig_list*
                                          ));
extern chain_list*  ttv_parsttvline    __P(( ttvfig_list*,
                                             ttvsig_list*,
                                             ttvsig_list*,
                                             long,
                                             chain_list*,
                                             char,
                                             long
                                          ));
extern long         ttv_fgetsline      __P(( FILE*, char*, long ));
extern chain_list*  ttv_getlinearg     __P(( char* ));
/*extern ttvfig_list* ttv_parsline       __P(( ttvfig_list*,
                                             char*,
                                             long,
                                             long,
                                             char*,
                                             inffig_list **
                                          ));*/
extern void         ttv_builthtabfig   __P(( ttvfig_list*, long ));
extern void         ttv_builthtabins   __P(( ttvfig_list*, long ));
extern void         ttv_builthtabttvfig __P(( ttvfig_list*, long ));
extern void         ttv_freehtabfig    __P(( ttvfig_list*, long ));
extern void         ttv_freehtabttvfig __P(( ttvfig_list*, long ));
extern void         ttv_calcstatus     __P(( ttvfig_list*, long, char, char ));
extern ttvfig_list* ttv_parsttvfig     __P(( ttvfig_list*, long, long ));

#endif //__TTV_PARSTTV_H__
