/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_desk.h                                                 */
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

extern Widget XtasDeskButtonsBarWidget;
extern Widget XtasDeskMessageField;
extern Widget XtasDataBaseName;
extern char   XTAS_FORMAT_LOADED;
extern int    XTAS_CTX_LOADED;

extern void XtasSetNewDisplayTypeCallback   __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                                )) ;
extern void XtasSetOldDisplayTypeCallback   __P((
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                                )) ;
extern void XtasDispCancelCallback          __P((
                                                 Widget ,
                                                 XtPointer ,
                                                 XtPointer
                                               )) ;
extern void XtasFileSaveCallback           __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasFileExitCallback           __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasOpenDataBaseCallback       __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasOpenDataBaseOk             __P(( ));
extern void XtasOptionsButtonsCallback     __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasMemoryOkCallback           __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasDisplayOkCallback          __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasOptionsDisplayTypeTreat    __P(( Widget )) ;
extern void XtasOptionsDisplayTypeCallback __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern void XtasOptionsMemorySizeTreat     __P(( Widget )) ;
extern void XtasOptionsMemorySizeCallback  __P((
                                                Widget ,
                                                XtPointer ,
                                                XtPointer
                                              )) ;
extern Widget  XtasCreateMainMenus         __P(( Widget )) ;
extern Widget  XtasCreateDesk              __P(( Widget )) ;

