/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_files.h                                                */
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

extern Widget XtasDataBaseRequester;
extern Widget XtasInfFileRequester;
extern char *XtasDataFileName;

extern void XtasSelectFileOkCallback  __P((
                                           Widget ,
                                           XtPointer ,
                                           XtPointer 
                                         )) ;
extern void XtasSelectFileCallback    __P((
                                           Widget ,
                                           XtPointer ,
                                           XtPointer 
                                         )) ;

extern void XtasSelectInfFileOkCallback  __P((
                                           Widget ,
                                           XtPointer ,
                                           XtPointer 
                                         )) ;
extern void XtasSelectInfFileCallback    __P((
                                           Widget ,
                                           XtPointer ,
                                           XtPointer 
                                         )) ;


