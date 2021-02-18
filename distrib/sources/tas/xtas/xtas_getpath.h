/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getpath.h                                              */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
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

extern void XtasParamUpupCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamUpdnCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamDndnCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamDnupCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamMinCallback        __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamMaxCallback        __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamCriticCallback     __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamDualCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasParamNormCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasStartEndSignalCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern Widget XtasSetOrderByStartOrEnd  __P((
                                             int ,
                                             int ,
                                             char * 
                                           )) ;
extern void XtasGetPathsOkCallback      __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasCancelGetPathsCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasMainToolsCallback       __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
