/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVDIFF Version 1                                           */
/*    Fichier : ttv_comp.h                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

extern void     diff_ttvfigdiff         __P(( FILE*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              long
                                           ));
extern ht       *diff_htloadnamesig     __P(( ttvfig_list*,
                                              chain_list*
                                           ));
extern void     diff_printdiffsig       __P(( FILE*,
                                              char*,
                                              ttvfig_list*,
                                              long,
                                              long
                                           ));
extern void     diff_printdiffcapa      __P(( FILE*,
                                              char*,
                                              float,
                                              float,
                                              long
                                            ));
extern void     diff_sigcapa            __P(( FILE*,
                                              ttvfig_list*,
                                              chain_list*,
                                              ht*,
                                              long
                                           ));
extern ht       *diff_htsig12           __P(( FILE*,
                                              chain_list*,
                                              ht*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              long,
                                              long
                                           ));
extern char     diff_geteventtype       __P(( ttvevent_list* ));
extern void     diff_printline          __P(( FILE*,
                                              ttvfig_list*,
                                              void*,
                                              long,
                                              long,
                                              char*,
                                              long,
                                              long
                                           ));
extern ttvline_list *diff_getline2      __P(( ht*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvline_list*,
                                              int,
                                              char
                                           ));
extern void     diff_lookline           __P(( FILE*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              chain_list*,
                                              ht*,
                                              short,
                                              long,
                                              char
                                           ));
extern ttvpath_list *diff_getpath2      __P(( ht*,
                                              ttvpath_list*,
                                              ttvpath_list*,
                                              long
                                           ));
extern void     diff_lookpath           __P(( FILE*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              chain_list*,
                                              ht*,
                                              long,
                                              short,
                                              long
                                           ));
extern void     diff_lookinfodiff       __P(( FILE*,
                                              ttvinfo_list*,
                                              ttvinfo_list*
                                           ));
extern void     diff_printdiffnetname   __P(( FILE*,
                                              char*,
                                              ttvfig_list*,
                                              char*,
                                              ttvfig_list*
                                           ));
extern void     diff_signetname         __P(( FILE*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              chain_list*,
                                              ht*
                                           ));
extern void     diff_printdiffmodel     __P(( FILE*,
                                              char*,
                                              ttvfig_list*,
                                              char*,
                                              ttvfig_list*,
                                              char*
                                           ));
extern int      diff_runttvdiff         __P(( FILE*,
                                              ttvfig_list*,
                                              ttvfig_list*,
                                              long
                                           ));
void            diff_lookmodel          __P(( FILE*,
                                              ttvfig_list*,
                                              void*,
                                              ttvfig_list*,
                                              void*,
                                              long,
                                              long
                                           ));
void            diff_printdiffmodeltime __P(( FILE*,
                                              long,
                                              long,
                                              long,
                                              long,
                                              long,
                                              long,
                                              char
                                           ));
int             ttv_testValidSig        __P(( ttvsig_list*
                                           ));

