/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_signals.h                                              */
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

#define XTAS_S_MEMORYINFOS 3
#define XTAS_DEFAULT_SIGITEMS 10

extern Widget          XtasSignalsTopWidget ; 

extern void XtasSignalsSaveCallback          __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsHelpCallback          __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasFileCloseCallback            __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsSetState              __P((int)) ;
extern void XtasInitializeSignalsWindow      __P(()) ;
extern void XtasSigsRemove                   __P(()) ;
extern void XtasSignalsFocusCallback         __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer 
                                                )) ;
extern void XtasFillSigList                  __P(( XtasSigListSetStruct *)) ;
extern void XtasSignalsTreatNextCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatFastFwdCallback  __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatEndCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatPrevCallback     __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatFastRewCallback  __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatTopCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsTreatCallback         __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsButtonsCallback       __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;
extern void XtasSignalsDetailCallback        __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer 
                                                )) ;
extern void XtasSignalsCtkInfoCallback       __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer 
                                                )) ;
extern Widget XtasCreateSignalsMenus         __P((
                                                   Widget ,
                                                   int
                                                )) ;
extern Widget XtasCreateSignalsList          __P((
                                                   Widget ,
                                                   int ,
                                                   XtasSigListSetStruct *
                                                )) ;
extern Widget XtasSignalsList                __P((
                                                   Widget ,
                                                   int ,
                                                   char *
                                                )) ;
extern void XtasDeskSignalsViewCallback      __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                )) ;

