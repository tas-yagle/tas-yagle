/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_paths.h                                                */
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

extern char *XTAS_SIGNAL ;
extern Widget XTAS_FLSPATHRESLABEL ;
extern Widget XTAS_PATHSSELECTEDWIDGET ;

extern void XtasRemovePathSession          __P(( XtasWindowStruct *)) ;
extern void XtasSavePaths                  __P((
                                                 FILE *file ,
                                                 XtasPathListSetStruct *
                                              )) ;
extern void XtasSignalInfoCallback         __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasPathsSaveCallback          __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasPathsSelectFileOk          __P(( )) ;
extern void XtasPathsHelpCallback          __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasPathsCloseCallback         __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasPathsFocusCallback         __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasSignalFocusCallback        __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasLosingPathsFocusCallback    __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                              )) ;
extern void XtasFillPathList                 __P(( XtasPathListSetStruct * )) ;
extern void XtasFillPathList2                __P(( XtasPathListSetStruct * )) ;
extern void XtasSigPathSelectCallback        __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatEndCallback         __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatFastFwdCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatMiddleCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatNextCallback        __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatFastRewCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatTopCallback         __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathTreatPrevCallback        __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasFillSigPathList              __P(( XtasPathListSetStruct *)) ;
extern void XtasSigPathTreatEndCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatFastFwdCallback  __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatNextCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatMiddleCallback   __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatPrevCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatFastRewCallback  __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSigPathTreatTopCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasPathsButtonsCallback         __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern Widget XtasCreatePathsMenus           __P((
                                                   Widget ,
                                                   XtasWindowStruct *
                                                )) ;
extern Widget XtasCreatePathsList            __P((
                                                   Widget ,
                                                   XtasPathListSetStruct * ,
                                                   XtasWindowStruct *
                                                )) ;
extern XtasWindowStruct *XtasPathsList       __P((
                                                   Widget ,
                                                   XtasSearchPathParamStruct *
                                                )) ;
extern void XtasPathsGetPathCmdCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;

extern void XtasPathsGetPathCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer
                                           )) ;
extern void XtasPathsDeselect      __P((
                                        Widget 
                                      )) ;

