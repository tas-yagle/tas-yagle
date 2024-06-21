/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getdelay_menu.h                                        */
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

/*-------------------------*/
/* menu structure          */
/*-------------------------*/

static struct XalMenuItem XtasGetDelayMenuView[] = {
    { "Connectors",        &xmPushButtonGadgetClass, 'C', False, False, NULL, NULL,
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_CONNECTOR,
                              (XalMenuItemStruct *)NULL },
    { "Commands",          &xmPushButtonGadgetClass, 'O', False, False, NULL, NULL, 
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_COMMANDS,
                              (XalMenuItemStruct *)NULL },
    { "Registers",         &xmPushButtonGadgetClass, 'R', False, False, NULL, NULL, 
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_MEMORIZE, 
                              (XalMenuItemStruct *)NULL },
    { "Precharged Points", &xmPushButtonGadgetClass, 'P', False, False, NULL, NULL, 
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_PRECHARGE,
                              (XalMenuItemStruct *)NULL },
    { "Break Points",      &xmPushButtonGadgetClass, 'B', False, False, NULL, NULL, 
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_BREAK,
                              (XalMenuItemStruct *)NULL },
    { "Internal Signals", &xmPushButtonGadgetClass, 'S', False, False, NULL, NULL, 
                              XtasStartEndDelayCallback,     (XtPointer)XTAS_C_OTHERSIGS,
                              (XalMenuItemStruct *)NULL },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

static struct XalMenuItem XtasGetDelayStartMenu[] = {
    { "Start", &xmCascadeButtonGadgetClass, 'S', False, False, NULL, NULL, NULL, 
                                           (XtPointer)0, XtasGetDelayMenuView },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};


static struct XalMenuItem XtasGetDelayEndMenu[] = {
    { "End", &xmCascadeButtonGadgetClass, 'E', False, False, NULL, NULL, NULL, 
                                           (XtPointer)0, XtasGetDelayMenuView },
    { NULL, NULL, (char)NULL, False, False, NULL, NULL, NULL, NULL, NULL }
};

