/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ttv.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by : Mathieu OKUYAMA                   Date : 05/21/1998     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define XTAS_PATH_UPUP ((long) 0x00000001)
#define XTAS_PATH_DNUP ((long) 0x00000002)
#define XTAS_PATH_UPDN ((long) 0x00000004)
#define XTAS_PATH_DNDN ((long) 0x00000008)


/* FONCTIONS DE XTAS_TTV  */

extern void XtasFreePathTabIndex                          __P(( chain_list *)) ;
extern chain_list *XtasCreatePathTabIndex                 __P(( XtasPathListSetStruct * )) ;
extern int XtasCleanTtvSigList                            __P(( XtasPathListSetStruct * )) ;
extern XtasSearchSigParamStruct *XtasNewSearchSigParam    __P(()) ;
extern XtasSearchSigParamStruct *XtasCopySearchSigParam   __P(( XtasSearchSigParamStruct * )) ;
extern XtasSigListSetStruct *XtasNewSigListSet            __P(( XtasSearchSigParamStruct * )) ;
extern void XtasFreeSearchSigParam                        __P(( XtasSearchSigParamStruct * )) ;
extern void XtasFreeSigListSet                            __P(( XtasSigListSetStruct * )) ;
extern XtasSearchPathParamStruct *XtasNewSearchPathParam  __P(( XtasSearchSigParamStruct * )) ;
extern XtasSearchPathParamStruct *XtasCopySearchPathParam __P(( XtasSearchPathParamStruct * )) ;
extern void XtasFreeSearchPathParam                       __P(( XtasSearchPathParamStruct * )) ;
extern void XtasFreePathListSet                           __P(( XtasPathListSetStruct * )) ;
extern XtasPathListSetStruct *XtasNewPathListSet          __P(( XtasSearchPathParamStruct * )) ;
extern XtasDetailPathSetStruct *XtasNewDetailPathSet      __P(( 
                                                                chain_list *,
                                                                int
                                                             )) ;
extern void XtasFreeDetailPathSetStruct                   __P(( XtasDetailPathSetStruct * )) ;
extern void XtasFreeChainjmplist                          __P(( XtasChainJmpList * )) ;
extern XtasChainJmpList *XtasNewPathjmplist               __P((
                                                                ttvpath_list * ,
                                                                int
                                                             )) ;
extern XtasChainJmpList *XtasNewChainjmplist              __P((
                                                                chain_list * ,
                                                                int
                                                             )) ;
extern XtasChainJmpList *XtasTtvGetSigList                __P(( XtasSigListSetStruct * )) ;
extern XtasChainJmpList *XtasTtvGetParaList               __P(( XtasPathListSetStruct * )) ;
extern XtasChainJmpList *XtasTtvGetPathList               __P(( XtasPathListSetStruct * )) ;
extern XtasDetailPathSetStruct *XtasTtvGetDelayList       __P(( XtasPathListSetStruct * )) ;
extern ttvfig_list *XtasLoadTtvfig                        __P(( char *)) ;
extern void XtasLoadCtx                                   __P(( ttvfig_list *)) ;


