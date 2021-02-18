/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_visualise_path.h                                       */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 24/12/2002     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#define XTAS_HILIGHT    'h'
#define XTAS_EXTRACT    'e'

extern  Widget  XtasVisualPathWidget;


extern  void    XtasVisualPathCallback          __P((
                                                     Widget ,
                                                     XtPointer ,
                                                     XtPointer
                                                   )) ;

extern  void    XtasVisualPathOkCallback        __P((
                                                     Widget ,
                                                     XtPointer ,
                                                     XtPointer
                                                   )) ;

extern  void    XtasHighlightCallback           __P((
                                                     Widget ,
                                                     XtPointer ,
                                                     XtPointer
                                                   )) ;

extern  void    XtasExtractCallback             __P((
                                                     Widget ,
                                                     XtPointer ,
                                                     XtPointer
                                                   )) ;





