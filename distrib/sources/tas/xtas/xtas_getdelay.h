/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getdelay.h                                             */
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

extern void XtasDelayUpupCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayUpdnCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayDndnCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayDnupCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayMinCallback        __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayMaxCallback        __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayRCCallback         __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasDelayGateCallback       __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasStartEndDelayCallback   __P(( 
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern Widget XtasSetDelayByStartOrEnd  __P((
                                             int ,
                                             char * 
                                           )) ;
extern void XtasGetDelayOkCallback      __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasCancelGetDelayCallback  __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern void XtasGetDelayCallback        __P((
                                             Widget ,
                                             XtPointer ,
                                             XtPointer 
                                           )) ;
extern Widget XtasDelayList             __P((
                                             Widget ,
                                             XtasSearchPathParamStruct *
                                           )) ;
extern void XtasFillDelayList           __P((
                                             XtasDetailPathSetStruct *
                                           )) ;
