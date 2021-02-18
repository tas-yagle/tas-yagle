/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_init.h                                                 */
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

extern XtAppContext             XtasApplicationContext;
extern XtasMainInfoStruct       *XtasMainParam;
extern char                     XTAS_SWITCH_COLOR_MAP;

extern char            *WORK_LIB_SAVE; 
extern char            *spi_sfx;
extern char            *vhd_sfx;
extern char            *vlg_sfx;


extern Widget			XtasTopLevel;
extern Widget			XtasMainWindow;
extern Widget			XtasDeskMainForm;
extern Widget			XtasFrontWidget;

extern XalMessageWidgetStruct   *XtasErrorWidget;
extern XalMessageWidgetStruct   *XtasWarningWidget;			
extern XalMessageWidgetStruct   *XtasTraceWidget;	

extern XalMessageWidgetStruct   *XtasNotYetWidget;			

extern void XtasAvtlicence                    __P(()) ;
extern void XtasPresent                       __P(()) ;
extern void XtasOkFrontPageCallback           __P((
                                                   Widget ,
                                                   XtPointer ,
                                                   XtPointer
                                                 )) ;
extern XtasMainInfoStruct *XtasInitMainParam  __P(()) ;
extern void XtasInitializeWindows             __P(()) ;
extern void XtasAppInit                       __P(( void )) ;
extern void XtasInitializeRessources          __P((
                                                   int  * ,
                                                   char ** 
                                                 )) ;

extern void XtasEnv                           __P (( void )); /* Fonction definie dans le fichier xtas_tas.c */

extern char XTAS_FORMAT_LOADED;
