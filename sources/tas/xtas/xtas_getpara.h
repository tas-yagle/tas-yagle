/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getpara.h                                              */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Mathieu OKUYAMA                     Date : 07/04/1998     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define XTAS_DEFAULT_PATHITEMS 10

extern void XtasParaSignalCallback          __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasInitializeGetParaPathParam  __P(( XtasPathListSetStruct* )) ;
extern void XtasRemoveParaPathSession       __P(()) ;
extern void XtasParaParamAndCallback        __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasParaParamOrCallback         __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                                )) ;
extern void XtasParaParamMinCallback        __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasParaParamMaxCallback        __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasParaParamCriticCallback     __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasParaParamDualCallback       __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasSetFromOrTo                 __P((
                                                  int , 
                                                  char *
                                               )) ;
extern void XtasGetParaPathsOkCallback      __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasCancelGetParaPathsCallback  __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasParaButtonCallback          __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
