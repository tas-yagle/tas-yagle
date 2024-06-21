/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_common.h                                               */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/****************************************************************************/

extern XalBitmapsStruct Xtas_bitmaps[] ;

extern int XtasSizeLong                     __P(( long )) ;
extern void XtasPrintType                   __P(( 
                                                 long  ,
                                                 char * ,
                                                 char * ,
                                                 int
                                               )) ;
extern void XtasCalcSize                    __P(( 
                                                 ttvfig_list *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 int *,
                                                 char ,
                                                 chain_list *,
                                                 long * ,
                                                 long * ,
                                                 long ,
                                                 long 
                                               )) ;
extern void XtasPrintFirstEnd               __P(( 
                                                 FILE *file ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 long
                                               )) ;
extern void XtasPrintLinesepar              __P(( 
                                                 FILE *file ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int ,
                                                 int 
                                               )) ;
extern void XtasPrintLine                   __P(( 
                                                 FILE *file ,
                                                 char * ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 long ,
                                                 int ,
                                                 char *,
                                                 int ,
                                                 char ,
                                                 char ,
                                                 char *,
                                                 int
                                               )) ;
extern chain_list *XtasGetMask              __P(( Widget )) ;
extern void XtasDestroyAllFig              __P(()) ;
extern char *XtasSuppresStringSpace         __P(( char * )) ;
extern void XtasSetLabelString              __P(( Widget ,
                                                  char *
                                               )) ;
extern void XtasMainPopupCallback           __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                               )) ;
extern void XtasReloadInfCallback           __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                               )) ;
extern int XtasAppNotReady                  __P(()) ;
extern void XtasDestroyWidgetCallback       __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                               )) ;
extern void XtasCancelCallback              __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                               )) ;
extern void XtasDummyCallback               __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasNotYetCallback              __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasAddDummyButton              __P(( Widget )) ; 
extern void XtasButtonsChangedCallback      __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasButtonsOkCallback           __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ; 
extern void XtasOptionsButtonsTreat         __P((
                                                  Widget ,
                                                  Widget  * ,
                                                  struct XalButtonsItem [] ,
                                                  struct XalButtonsItem []
                                                )) ;
