/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_detpath.h                                              */
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
/*                                                                          */
/****************************************************************************/


extern void XtasRemoveDetailPathSession __P(( XtasWindowStruct *)) ;
extern void XtasSaveDetail              __P((
                                             FILE *file ,
                                             XtasDetailPathSetStruct *,
                                             long
                                           )) ;
extern void XtasDetailSaveCallback      __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ; 
extern void XtasDetailSaveSelectFileOk  __P(( )) ;
extern void XtasDetailHelpCallback      __P((
                                             Widget ,
                                             XtPointer  ,
                                             XtPointer
                                           )) ; 
extern void XtasDetailCloseCallback     __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasDetailButtonsCallback   __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasSetDetailInfo           __P((
                                             XtasDetailPathSetStruct *,
                                             ttvcritic_list * 
                                           )) ;
extern void XtasSelectDelayCallback     __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasDetailFocusCallback     __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasDetailFocusNameCallback __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasLosingDetailFocusCallback __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                             )) ;
extern void XtasDetailDetailCallback    __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasCtkInfoCallback         __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasFillDetailPathList      __P(( XtasDetailPathSetStruct * )) ;
extern void XtasFillDetailPathList2     __P(( 
                                             XtasDetailPathSetStruct *,
                                             Widget
                                           )) ;
extern Widget XtasCreateDetailMenus     __P((
                                             Widget ,
                                             XtasWindowStruct *
                                           )) ;
extern Widget XtasCreateDetailList      __P((
                                             Widget ,
                                             XtasWindowStruct * ,
                                             XtasDetailPathSetStruct * ,
                                             int
                                           )) ;
extern Widget XtasPathDetailList        __P((
                                             Widget ,
                                             XtasPathListSetStruct *
                                           )) ;

extern void XtasDetPathDeselect         __P((
                                             Widget
                                           )) ;
