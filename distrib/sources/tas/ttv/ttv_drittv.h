/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_drittv.h                                                */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

extern long TTV_SEUIL_MAX ;
extern long TTV_SEUIL_MIN ;
extern long TTV_INFO_CAPARM ;
extern long TTV_INFO_CAPASW ;
extern char *TTV_INFO_TOOLDATE ;
extern short TTV_INFO_ARGC ;
extern char **TTV_INFO_ARGV ;
extern char TTV_PATH_SAVE ;

extern void ttv_printcaracsig __P(( ttvfig_list*, FILE*, ttvsig_list*));
extern void ttv_printname     __P(( ttvfig_list*, FILE*, ttvsig_list* ));
extern char ttv_getdir        __P(( ttvsig_list* ));
extern char ttv_getsigtype    __P(( ttvsig_list*, ttvline_list*, int));
extern void ttv_printcritic   __P(( ttvfig_list*,
                                    FILE*,
                                    ttvevent_list*,
                                    ttvevent_list*,
                                    long
                                 ));
extern void ttv_printline     __P(( ttvfig_list*,
                                    ttvfig_list*,
                                    FILE*,
                                    long,
                                    long,
                                    long
                                 ));
extern void ttv_printhead     __P(( ttvfig_list*,
                                    FILE*,
                                    long,
                                    ttvpath_list*,
                                    ttvpath_list*
                                 ));
extern void ttv_drittv(ttvfig_list *ttvfig, long type, long find, char *filename);
//extern void ttv_drittv        __P(( ttvfig_list*, long, long ));
extern void ttv_drittvold     __P(( ttvfig_list*, long, long ));
