/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_unix.h                                                 */
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


extern int tas_pfd[2];
extern sigjmp_buf XtasMyEnv;

extern void XtasSetBlock       __P((
                                    int ,
                                    int 
                                  )) ;
extern void XtasInitFileErr    __P(()) ;
extern void XtasGetStdoutMess  __P(()) ;
extern void XtasGetOutMess     __P(()) ;
extern void XtasPasqua         __P(()) ;
extern void XtasFirePasqua     __P(()) ;
extern void XtasFatalSysError  __P(()) ;
extern void XtasExit           __P((int)) ;
extern void XtasFatalAppError  __P(( char * )) ;
extern void XtasGetWarningMess __P(()) ;
extern void XtasClearAllStream __P(()) ;
